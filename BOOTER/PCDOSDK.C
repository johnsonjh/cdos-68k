/*	@(#)pcdosdk.c	1.7		*/
/*
 *	This module contains the PC-DOS disk routines used by the
 *	VME/10 booter for Concurrent DOS-68K.
 *
 *	ASSUMPTIONS:
 *
 *	- The file to be read must be in the root directory.  (*** will
 *	  change to allow path searching).
 */

#include "portab.h"
#include "cdosboot.h"

/*	MACROS	*/

/* convert to upper case */
#define TOUPPER(c)	if ((c) >= 'a') (c) -= 'a' - 'A'

/*	CONSTANTS	*/

/*	EXTERNALS	*/

extern FILESYS	fs;

/* structure for one directory entry */

typedef struct
{
	BYTE d_name[11];
	BYTE d_attr;
	WORD d_curclust;	/* used only by these routines, not by DOS */
	BYTE d_fill[12];
	WORD d_cluster;		/* starting cluster */
	LONG d_size;
} DIRENTRY;

MLOCAL DIRENTRY direntry;

/* buffers for fat */

MLOCAL BYTE fat[MAXFATBUF];		/* used only to store the FAT */
MLOCAL BYTE name[11];			/* parsed name of open file */


/***********   FAT I/O ROUTINES ***********/

/*  NextClust - get next cluster number from FAT, given current cluster no. */

MLOCAL WORD NextCluster(cluster)
REG WORD cluster;
{
	WORD index;
	REG WORD fatword;

	if( fs.mformat == ONEANDAHALF )
	{
		index = cluster + (cluster / 2);	/* multiply by 1.5 */
		fatword = getword(&fat[index]);		/* get word from fat */
		if (cluster & 1) fatword >>= 4;		/* keep high 12 bits */
		return (fatword & 0xfff);		/* mask off top 4 */
	}
	else if( fs.mformat == TWO )		/* two byte fat entries */
	{
		index = cluster * 2;			/* multiply by 2 */
		fatword = getword(&fat[index]);		/* get word from fat */
		return (fatword);
	}
	else
	{
		PutMsg("Format byte is bad, can't do FAT entry calculation.");
		Exit();
	}
}


/*  readfat - read the fat into the fat array */

MLOCAL VOID ReadFat()
{
	if( (fs.nfrecs*fs.sectsize) > MAXFATBUF )
	{
		PutMsg("Can't buffer FAT, MAXFATBUF is too small.");
		Exit();
	}

	/*
	 *	WE NEED TO IMPLEMENT READING THE OTHER FAT TABLE!
	 *	JUST TRY FIRST COPY FOR NOW.
	 */

	ReadSector((LONG)fs.firstsec,fs.nfrecs,fat);
}


/***********  CLUSTER I/O ***********/

/*
 *	ReadCluster
 *
 *	Read the cluster into the buffer array
 *
 *	NOTE: I made this a simple macro.
 */

#define ReadCluster(bp,cluster)	\
	ReadSector((LONG)((cluster - 2) * fs.secblk + fs.datasect),fs.secblk,bp)


/***********  DIRECTORY I/O ***********/

/*	SearchDir
 *
 *	Read the directory entry into buffer which corresponds to
 *	the name parameter.
 *
 *	Return ERROR if no match and no more entries.  Else return OK.
 */

SearchDir(dp)
REG DIRENTRY *dp;
{
	WORD dirsect;		/* current directory sector */
	WORD nextdir;		/* index to next directory entry to read */
	BYTE dirbuf[MAXSECTSIZE];	/* used for directory search */

	dirsect = 0;
	nextdir = 0;

	ReadSector((LONG)fs.dirsect,1,dirbuf);

	for (;;)
	{
		if (nextdir >= fs.sectsize)
		{
			if (++dirsect >= fs.dirsize) return ERROR;
			ReadSector((LONG)(dirsect+fs.dirsect),1,dirbuf);
			nextdir = 0;
		}
		if (dirbuf[nextdir] == 0)	/* end of directory */
			return ERROR;
		if (dirbuf[nextdir] == 0xe5)	/* available entry */
			nextdir += sizeof(DIRENTRY);
		else
		{
			movb(&dirbuf[nextdir],dp,sizeof(DIRENTRY));
			if (strncmp(dp->d_name,name,sizeof(name)) == 0)
			{
				/* fix swapped pointers */
				dp->d_cluster = swapw(dp->d_cluster);
				dp->d_size = swapl(dp->d_size);
				ReadFat();				
				return OK;
			}
			else
				nextdir += sizeof(DIRENTRY);
		}
	}
}


/***********  FILE READ ***********/

/*	ReadPcdos
 *
 *	Read the next cluster from the file to the user
 *	specified buffer.  The actual number of bytes
 *	read is returned.
 */

ReadPcdos(buffer)
BYTE *buffer;
{
	WORD actual;

	if (direntry.d_cluster >= 0xff8 || direntry.d_size == 0)
		return (0);
	ReadCluster(buffer,direntry.d_cluster);
	direntry.d_cluster = NextCluster(direntry.d_cluster);
	actual = direntry.d_size < fs.blksz ? direntry.d_size : fs.blksz;
	direntry.d_size -= actual;
	return (actual);
}


/*  Parse - parse a filename, put parsed name in "name" */

Parse(fname)
BYTE *fname;
{
	WORD i;
	BYTE c;

	setb(' ',name,sizeof(name));
	i = 0;
	while (c = *fname++)			/* terminated by null */
	{
		if (i >= sizeof(name)) return ERROR;
		if (c == '.')				/* start of extension */
		{
			if (i > 8) return ERROR;
			else i = 8;
		}
		else
		{
			TOUPPER(c);
			name[i++] = c;
		}
	}
	return OK;
}


/*	OpenPcdos
 *
 *	Search directory for pcdos file.  If not found, return ERROR.
 *	Otherwise, read the directory entry into direntry,
 *	and return OK.
 */

OpenPcdos(fname)
BYTE *fname;
{
	if (Parse(fname) == ERROR)
	{
		PutMsg("Bad file name.");
		return ERROR;
	}
	if( SearchDir(&direntry) == ERROR )
	{
		PutStr(OURNAME);
		PutStr("Can't find file: ");
		PutStr(fname);
		PutStr("\r\n");
		return ERROR;		/* couldn't find the file */
	}
	return OK;
}
"Can't find file: ");
		PutStr(fname);
		PutStr("\