/*	@(#)ibmdisk.c	2.3		*/
/*  ibmdisk (12 Oct 84) - read IBM PC disk */

/*  This module contains the following user-callable routines:

	OPENIBM - open an IBM file
	READIBM - read the next cluster (1K) from the opened file
	OPENDIR - setup for reading directory entries
	READDIR - read next valid directory entry

    The diskette must be 8 sectors per track, double sided, and
    the file to be read must be in the root directory.
*/

#include "portab.h"
#include "ibmdisk.h"

EXTERN FS	fs;

extern WORD debugflag;

/* structure for one directory entry */

struct {
	BYTE d_name[11];
	BYTE d_attr;
	WORD d_curclust;	/* used only by these routines, not by DOS */
	BYTE d_fill[12];
	WORD d_cluster;		/* starting cluster */
	LONG d_size;
} direntry;

/* buffers for fat & directory */

BYTE dirbuf[MAXSECTSIZE];		/* used for directory search */
BYTE fat[MAXSECTSIZE*MAXFATSIZE];	/* used only to store the FAT */
WORD dirsect;			/* current directory sector */
WORD dirindex;			/* index to current directory entry */
WORD nextdir;			/* index to next directory entry to read */
BYTE name[11];			/* parsed name of open file */
BYTE tpi;			/* current tpi flags for iopb */

/***********   FAT I/O ROUTINES ***********/

/*  nextclust - get next cluster number from FAT, given current cluster no. */

nextclust(cluster)
WORD cluster;
{
	WORD index,fatword;

	index = cluster + (cluster >> 1);	/* multiply by 1.5 */
	fatword = getword(&fat[index]);		/* get word from fat */
	if (cluster & 1) fatword >>= 4;		/* keep high 12 bits */
	if( debugflag )
		printf("nextclust: cluster = %x, index = %x, fatword = %x\n",
			cluster, index, fatword);
	return (fatword & 0xfff);		/* mask off top 4 bits */
}


/*  setnext - set the fat entry for a given cluster to be "next" */

setnext(cluster,next)
WORD cluster,next;
{
	WORD index,fatword,mask;

	if (cluster & 1)
	{
		mask = 0xf;			/* use only high 12 bits */
		next <<= 4;			/* shift next to high 12 */
	}
	else mask = 0xf000;			/* use only low 12 bits */
	index = cluster + (cluster >> 1);	/* multiply by 1.5 */
	fatword = getword(&fat[index]);		/* get word from fat */
	putword(&fat[index],(fatword & mask) | next);	/* modify it */
	if( debugflag )
		printf("setnext: cluster = %x, next = %x, index = %x, fatword = %x\n",
		cluster, next, index, fatword);
}


/*  readfat - read the fat into the fat array */

readfat()
{
	if( ReadSector(fs.disknum,fs.fatstartsec,fs.fatszinsecs,fat) == ERROR )
	{
		printf("FAT read failed.\n");
		exit();
	}
}


/*  writefat - write the FAT array to both fat areas on the disk */

writefat()
{
	WORD i;				/* number of fats to write */

	for (i = 0; i < fs.nfats; i++)
		WriteSector(fs.disknum,(LONG)(fs.fatstartsec+i*fs.fatszinsecs),
			fs.fatszinsecs,fat);
}


/* delchain - delete a chain of clusters (i.e. reclaim them) */

delchain(cluster)
WORD cluster;
{
    WORD next;

    if (cluster == 0) return;
    while (cluster < 0xff8)
    {
	next = nextclust(cluster);	/* save next cluster */
	setnext(cluster,0);		/* set next to 0 (available) */
	cluster = next;
    }
}


/*  allocclust - return the first free cluster number */

allocclust()
{
    WORD i,nclusters;

    nclusters = ((fs.fatszinsecs*fs.bps)*2)/3;
    for (i = 0; i < nclusters; i++)
	if (nextclust(i) == 0) return (i);
    return (-1);
}


/***********  CLUSTER I/O ***********/

/*  readcluster - read the cluster into the buffer array */

readcluster(bp,cluster)
BYTE *bp;
WORD cluster;
{
	
	if( ReadSector(fs.disknum,
		(LONG)((cluster - 2) * fs.clszinsecs + fs.datastartsec),
		fs.clszinsecs,bp) == ERROR )
	{
		printf("Can't read cluster %d\n", cluster);
		exit();
	}
}


/*  writecluster - write the cluster from the buffer array to disk */

writecluster(bp,cluster)
BYTE *bp;
WORD cluster;
{
	if( WriteSector(fs.disknum,
		(LONG)((cluster - 2) * fs.clszinsecs + fs.datastartsec),
		fs.clszinsecs,bp) == ERROR )
	{
		printf("WriteSector failed in readcluster at cluster: %d\n",
			cluster);
		exit();
	}
}


/***********  DIRECTORY I/O ***********/

/*  opendir - read the first directory sector and initialize pointers to it */

opendir()
{
    /* config();				/* determine medium type */
    if( ReadSector(fs.disknum,fs.rtdirstartsec,1,dirbuf) == ERROR )
    {
	printf("ReadSector failed reading directory in opendir().\n");
	exit();
    }
    dirsect = nextdir = 0;
    return (0);
}


/*  readdir - read next directory entry to user-specified buffer.
	The directory sector number is in dirsect+DIRSECT, and
	the index into the directory buffer is in dirindex.
	Return -1 if no more entries. */

readdir(buffer)
BYTE *buffer;
{
    for (;;)
    {
	if (nextdir >= fs.bps)
	{
	    if (++dirsect >= fs.rtdirszinsecs) return (-1);
	    if( ReadSector(fs.disknum,(LONG)(dirsect+fs.rtdirstartsec),
		1,dirbuf) == ERROR )
	    {
		printf("In readdir, ReadSector failed\n");
		return (-1);
	    }
	    nextdir = 0;
	}
	if (dirbuf[nextdir] == 0) return (-1);	/* end of directory */
	if (dirbuf[nextdir] == 0xe5) nextdir += sizeof(direntry);
	else
	{
	    movb(&dirbuf[nextdir],buffer,sizeof(direntry));
	    dirindex = nextdir;			/* let outsiders know where */
	    nextdir += sizeof(direntry);
	    return (0);
	}
    }
}


/*  allocdir - allocate a new directory entry */

allocdir()
{
    opendir();
    for (;;)
    {
	if (nextdir >= fs.bps)		/* get next sector */
	{
	    if (++dirsect >= fs.rtdirszinsecs) return (-1);
	    if( ReadSector(fs.disknum,(LONG)(dirsect+fs.rtdirstartsec),
		1,dirbuf) == ERROR )
	    {
		printf("Can't read directory sector 0x%lx.\n",
			(LONG)(dirsect+fs.rtdirstartsec));
		return(-1);
	    }
	    nextdir = 0;
	}
	if (dirbuf[nextdir] == 0 || dirbuf[nextdir] == 0xe5)	/* is free? */
	{
	    dirindex = nextdir;			/* let outsiders know */
	    return (0);				/*  which direntry is free */
	}
	else nextdir += sizeof(direntry);	/* get next entry */
    }
}


/***********  FILE READ ***********/

/*  readibm - read the next cluster from the IBM file to user
		specified buffer.  The actual number of bytes
		read is returned.  A cluster is 1K bytes. */

readibm(buffer)
BYTE *buffer;
{
	WORD actual;

	if (direntry.d_cluster >= 0xff8 || direntry.d_size == 0)
		return (0);
	readcluster(buffer,direntry.d_cluster);
	if( debugflag )
		printf("readibm: current cluster = %x\n",direntry.d_cluster);
	direntry.d_cluster = nextclust(direntry.d_cluster);
	if( debugflag )
		printf("readibm: next cluster = %x\n",direntry.d_cluster);
	actual = fs.bps*fs.clszinsecs;
	if (direntry.d_size < actual) actual = direntry.d_size;
	direntry.d_size -= actual;
	return (actual);
}


/*  parse - parse a filename, put parsed name in "name" */

parse(fname)
BYTE *fname;
{
    WORD i;
    BYTE c;

    setb(' ',name,sizeof(name));
    i = 0;
    while (c = *fname++)			/* terminated by null */
    {
	if (i >= sizeof(name)) return (-1);
	if (c == '.')				/* start of extension */
	{
	    if (i > 8) return (-1);
	    else i = 8;
	}
	else
	{
	    if (c >= 'a') c -= 'a' - 'A';	/* convert to upper case */
	    name[i++] = c;
	}
    }
    return (0);
}


/*  openibm - search directory for IBM file.  If not found, return -1.
		Otherwise, read the directory entry into direntry,
		and return 0. */

openibm(fname)
BYTE *fname;
{
    if (parse(fname) == -1) return (-1);	/* bad filename */
    opendir();					/* rewind the directory */
    while(readdir(&direntry) != -1)		/* end of directory? */
    {
	if (strncmp(direntry.d_name,name,sizeof(name)) == 0)
	{
	    direntry.d_cluster = swapw(direntry.d_cluster);
	    direntry.d_size = swapl(direntry.d_size);
	    if( debugflag )
		printf("openibm: cluster = %x, size = %lx (swapped)\n",
			direntry.d_cluster, direntry.d_size);
	    readfat();				
	    return (0);
	}
    }
    return (-1);		/* couldn't find the file */
}


/***********  FILE WRITE ***********/

/* creatibm - create an IBM PC file (open for write) */

creatibm(fname)
BYTE *fname;
{
    if (parse(fname) == -1) return (-1);	/* bad filename */
    if (openibm(fname) == -1)			/* not found */
    {
	if (allocdir() == -1)
	{
	    printf("Can't allocate a directory entry, directory full.\n");
	    return (-1);			/* directory full */
	}
	setb(0,&direntry,sizeof(direntry));
	movb(name,direntry.d_name,sizeof(name));
						/* make nice filename */
	readfat();				/* get the fat */
    }
    else					/* found - delete old file */
    {						/* already got fat */
	delchain(direntry.d_cluster);		/* reclaim the clusters */
	direntry.d_cluster = 0;			/* set file size to 0 */
	direntry.d_size = 0;
    }
    return (0);					/* no errors! */
}

/* eraibm - erase an IBM PC file */

eraibm(fname)
BYTE *fname;
{
    if (parse(fname) == -1) return (-1);	/* bad filename */
    if (openibm(fname) == -1)			/* not found */
    {
	printf("Can't find file %s\n", fname);
	return (-1);
    }
    else					/* found - delete file */
    {						/* already got fat */
	delchain(direntry.d_cluster);		/* reclaim the clusters */
	direntry.d_name[0] = 0xe5;		/* mark as erased */
	direntry.d_cluster = 0;			/* set file size to 0 */
	direntry.d_size = 0;
	closeibm();
    }
    return (0);					/* no errors! */
}


/*  writeibm -  write the next cluster from user specified buffer
	to an IBM file.  The actual number of bytes
	written is returned.  A cluster is 1K bytes.  If less than
	1K bytes are written, this must be the last write to the
	file before it is closed.  */

writeibm(buffer,count)
BYTE *buffer;
WORD count;
{
    WORD freeclust;

    if (count > fs.clszinsecs*fs.bps) return (-1);	/* more than 1K! */
    if ((freeclust = allocclust()) == -1)	/* allocate a new cluster */
    {
	printf("Can't allocate a data cluster, disk full!!!\n");
	return (-1);				/* disk full! */
    }
    if( debugflag )
	printf("writeibm: freeclust = %x\n", freeclust);
    setnext(freeclust,0xfff);			/* this is new end of chain */
    if (direntry.d_cluster == 0)		/* first cluster in file? */
	direntry.d_cluster = freeclust;		/* make this first cluster */
    else
	setnext(direntry.d_curclust,freeclust);	/* else link to end of chain */
    if( debugflag )
	printf("writeibm: d_curclust = %x\n", direntry.d_curclust);
    direntry.d_curclust = freeclust;		/* make this current cluster */
    writecluster(buffer,freeclust);		/* write the data */
    direntry.d_size += count;			/* update the file size */
    return (count);
}


/* closeibm - close a file that was open for write */

closeibm()
{
    direntry.d_curclust = 0;
    direntry.d_cluster = swapw(direntry.d_cluster);
    direntry.d_size = swapl(direntry.d_size);
    direntry.d_attr = 0x20;			/* set archive attr */
    movb(&direntry,&dirbuf[dirindex],sizeof(direntry));
    WriteSector(fs.disknum,(LONG)(dirsect+fs.rtdirstartsec),1,dirbuf);
    writefat();
}
