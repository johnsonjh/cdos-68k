/*	XIO.C simplified interface to CDOS */

#include "portab.h"
#include "stdio.h"

EXTERN LONG s_open(),s_create(),s_read(),s_write(),s_close(),s_exit();
EXTERN LONG s_seek(),s_delete(),s_rdelim();


/* xread - read bytes from current position in file */

LONG xread(fnum,buffer,size)
LONG fnum;
BYTE *buffer;
LONG size;
{
    MLOCAL WORD delims[4] = { 3, 0x0d, 0x0a, 0x1b };

    if (fnum == STDIN)		/* reading from standard input? */
	return (s_rdelim(0x122,fnum,buffer,size,0L,delims));
    else
	return (s_read(0x100,fnum,buffer,size,0L));
}


/* xwrite - write bytes to current position in file */

LONG xwrite(fnum,buffer,size)
LONG fnum;
BYTE *buffer;
LONG size;
{
    return (s_write(0x100,fnum,buffer,size,0L));
}


/* xopen - open a file */

LONG xopen(fname,access)
BYTE *fname;
WORD access;	/* 0 = read-only, 1 = write-only, 2 = read/write */
{
    MLOCAL WORD oflags[4] = { 0x018, 0x015, 0x01d, 0x00 };

    return (s_open(oflags[access],fname));
}


/* xcreat - create a disk file, allow read/write access */

LONG xcreat(fname,access)
LONG fname;
WORD access;		/* not used */
{
    return (s_create(0,0x40d,fname,0,0,0L));
}


/* xseek - move file pointer around */

LONG xseek(fnum,offset,origin)
LONG fnum;
LONG offset;	/* how much to move */
WORD origin;	/* 0 = beginning, 1 = current, 2 = end of file */
{
    return (s_seek(origin << 8,fnum,offset));
}


/* xclose - do a full close on a file */

LONG xclose(fnum)
LONG fnum;
{
    return (s_close(0,fnum));
}


/* xunlink - delete a file */

LONG xunlink(filename)
BYTE *filename;
{
    return (s_delete(0,filename));
}
