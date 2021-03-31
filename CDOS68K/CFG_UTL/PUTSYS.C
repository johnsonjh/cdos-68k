/***********************************************************************
*  PUTSYS - routine to copy system files from disk to disk
*
*  VERSION  DATE	WHO	CHANGE
*  =======  ====	===	======
*  1.1	    03/24/86	MA	Retry writes after media change errors.
*  1.0	    ??/??/??	MA	First version. 
***********************************************************************/

#include "portab.h"
#include "stdio.h"


/* CDOS error codes */

#define E_MEDCHG 0x4301L


/* External functions we need */

EXTERN LONG xread(),xwrite(),xopen(),xcreat(),xclose();
EXTERN LONG s_lookup(),s_get(),s_set(),s_malloc();


/* Local variables */

MLOCAL BYTE srcname[80];	/* source filename for copy */
MLOCAL BYTE dstname[80];	/* destination filename */
MLOCAL LONG ret;		/* return code from CDOS */


/* MALLOC parameter block */

MLOCAL struct {
    BYTE *m_start;
    LONG m_min;
    LONG m_max;
} mapb;


/* disk file table structure - see DISKFILE Table in Programmer's Guide */

MLOCAL struct {
    LONG d_key;
    BYTE d_name[18];
    WORD d_attrib;
    WORD d_recsize;
    BYTE d_user;
    BYTE d_group;
    WORD d_protect;
    WORD d_rsvd[3];
    LONG d_size;
    BYTE d_time[8];
} dftab;


/* disk table - used to find out cluster size on destination disk */

MLOCAL struct {
    BYTE dt_name[10];
    BYTE dt_jnk1[22];
    WORD dt_sectsize;
    BYTE dt_jnk2[8];
    WORD dt_secblk;
} disktab;


/* putsys - write all system files to newly formatted disk; return
	total disk space used by the files that were copied */

LONG putsys(diskname)
BYTE *diskname;
{
    BYTE *filename;
    LONG sysfiles;
    LONG clustmask;
    LONG dfnum;
    LONG realsize;

/* open the disk device to figure out cluster size */

    dfnum = xopen(diskname,2);
    if (dfnum < 0)
    {
	printf("Error %lx opening drive %s\r\n",dfnum,diskname);
	return (0L);
    }
    ret = s_get(0x21,dfnum,&disktab,(LONG)sizeof(disktab));
    xclose(dfnum);
    if (ret < 0)
    {
	printf("Error %lx getting disk table for drive %s\r\n",ret,diskname);
	return (0L);
    }
    clustmask = (disktab.dt_sectsize * disktab.dt_secblk - 1) & 0xffffL;
    sysfiles = 0;
    dftab.d_key = 0;

/* allocate a heap for purposes of copying */

    mapb.m_min = 0x1000;		/* at least 4K */
    mapb.m_max = 0x20000;		/* but 128K would be nice too! */
    ret = s_malloc(1,&mapb);		/* try to allocate a heap */
    if (ret < 0)
    {
	printf("Error %lx trying to allocate a mere %ld byte heap!\r\n",
		mapb.m_min);
	return (0L);
    }

/* copy the system file one-by-one to the formatted disk */

    while (searchsys())
    {
	if (copysys(diskname))
	    sysfiles += (dftab.d_size + clustmask) & ~clustmask;
	else
	    return (sysfiles);
    }
    return (sysfiles);
}


/* searchsys - search for next system file in the root directory of the
	default disk, return true if found one; the diskfile table
	information for the found file is left in dftab. */

MLOCAL searchsys()
{
    for (;;)
    {

    /* look up the next system/hidden/normal file */

	ret = s_lookup(0x20,3,"default:/*.*",&dftab,(LONG)sizeof(dftab),
			(LONG)sizeof(dftab),dftab.d_key);
/*	printf("S_lookup returned %lx\r\n",ret); */
	if (ret != 1)
	{
	    if (ret < 0)
		printf("S_LOOKUP error: %lx\r\n",ret);
	    return (FALSE);	/* no more files */
	}

    /* check the system attribute on the file - must be set */

/*	printf("File name: %s  Attrib: %x\r\n",dftab.d_name,dftab.d_attrib); */
	if (dftab.d_attrib & 0x4) return (TRUE);
    }
}


/* copysys - copy the file specified by dftab to the newly formatted disk */

MLOCAL copysys(diskname)
BYTE *diskname;
{
    LONG source,dest;		/* source and destination file numbers */
    WORD ok;

/* construct the source file name and open the file */

    strcpy(srcname,"default:/");
    strcat(srcname,dftab.d_name);
    source = xopen(srcname,0);
    if (source < 0)
    {
	printf("Error opening %s: %lx\r\n",srcname,source);
	return (FALSE);
    }

/* construct the destination file name and create that file */

    strcpy(dstname,diskname);
    strcat(dstname,dftab.d_name);
    dest = xcreat(dstname,0);
    if (dest < 0)
    {
	printf("Error creating %s: %lx\r\n",dstname,dest); 
	xclose(source);
	return (FALSE);
    }

/* copy the file data, set the time and date on the destination, close up */

    printf("Copying %s\r\n",dftab.d_name);
    if (!(ok = copydata(source,dest)))
    {
	if ((ret & 0xffffL) == E_MEDCHG)
	{			/* media change - try again */
	    xclose(dest);
	    dest = xcreat(dstname,0);
	    if (dest < 0)
	    {
		printf("Error creating %s after media change error: %lx\r\n",
			dstname,dest);
		return (FALSE);
	    }
	    if (!(ok = copydata(source,dest)))
		if ((ret & 0xffffL) == E_MEDCHG)
		    printf("Error %lx writing to %s\r\n",ret,dstname);
	}
    }
    if (ok) settable(dest);
    xclose(source);
    xclose(dest);
    return (ok);
}


/* copydata - copy data from source to destination file */

copydata(source,dest)
LONG source,dest;	/* file numbers */
{
    for (;;)
    {

    /* read data from the source file */

	ret = xread(source,mapb.m_start,mapb.m_min);
	if (ret < 0)
	{
	    printf("Error %lx reading from %s\r\n",ret,srcname);
	    return (FALSE);
	}

    /* write the data to the destination file */

	ret = xwrite(dest,mapb.m_start,ret);
	if (ret < 0)
	{
	    if ((ret & 0xffffL) != E_MEDCHG)	/* handle this outside */
		printf("Error %lx writing to %s\r\n",ret,dstname);
	    return (FALSE);
	}
	if (ret < mapb.m_min) return (TRUE);	/* check for end of file */
    }
}


/* settable - set the disk file table for a given file using the
	information in the dftab structure */

settable(fnum)
LONG fnum;		/* the file number */
{
    ret = s_set(0x20,fnum,&dftab,(LONG)sizeof(dftab));
    if (ret < 0)
	printf("Error %lx setting disk file table\r\n",ret);
}
