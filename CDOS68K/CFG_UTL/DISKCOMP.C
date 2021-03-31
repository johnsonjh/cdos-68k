/***********************************************************************
* diskcomp.c - quick and dirty diskette compare for CDOS 68K on VME/10
*
*  VERSION  DATE	WHO	CHANGE
*  =======  ====	===	======
*  1.1      02/21/86	MA	Use default drive for unspecified drives
*  1.0	    01/23/86	MA	Initial version.
***********************************************************************/

/***********************************************************************
*	VME/10 DISKETTE FORMAT (a brief summary)
*
*	1. Track 0 of a floppy disk is single density, 128 bytes per
*	sector, and 16 sectors per track.
*	2. All other tracks on the diskette are double density, 512
*	bytes per sector, and eight sectors per track.
*	3. There are 80 cylinders and two heads, for a total of
*	160 tracks.
*	4. This program tries to buffer as many tracks as possible
*	in memory.  It tries to allocate space for 160 tracks,
*	and if that fails, then 150, then 140, etc. subtracting
*	10 each time.
*	5. This program handles only the native VME/10 disk format.
*	The 320K DOS 1.1 format will mess up this program.
*	6. This program was derived from diskcopy.c.  Bugs that you
*	find in one will probably be in the other, and should be
*	fixed at the same time.
***********************************************************************/

#include "portab.h"
#include "stdio.h"

EXTERN LONG xopen(),xclose(),xread();		/* in XIO.C */
EXTERN LONG s_read(),s_write(),s_special();	/* in SYSLIB.L68 */
EXTERN LONG s_malloc(),s_get(),s_set();		/* in SYSLIB.L68 */
EXTERN LONG s_define();				/* in SYSLIB.L68 */


#define TRKSIZE		0x1000L			/* size of one track */
#define TRK0SIZE 	0x200L			/* usable track 0 size */
#define NTRACKS		160			/* number of tracks */
#define FIRST		0			/* parameter to select() */
#define SECOND		1			/* parameter to select() */

#define CONTAB		0x30			/* console table type */

#define SPECREAD	0x86			/* special read function */


/* misc. local variables */

BYTE onetrack[TRKSIZE];	/* buffer for one track */
LONG fnum[2];		/* file numbers of first and second drives */
BYTE *drvname[2];	/* names of the first and second drives */
BYTE *drvtype[2] = 	/* drive types used in error messages */
     { "FIRST", "SECOND" };
LONG ret;		/* return code from cdos */
BYTE *trkbuf;		/* pointer to a huge track buffer */
LONG tbufsize;		/* size of track buffer in bytes */
BYTE samedisk;		/* true if first and second drive are the same */
BYTE prompted;		/* true if prompted user already */
BYTE formatting;	/* true if printed formatting message */
BYTE linebuf[80];	/* general purpose input line buffer */
BYTE compok;		/* true if disks compare OK */
BYTE defdrv[16];	/* default source drive name */


/* main program - figure out command line, allocate buffer space,
	then call docomp() to do the real work
*/

main(argc,argv)
WORD argc;
BYTE *argv[];
{
    LONG trysize;

    printf("VME/10 Diskcomp  V1.1\r\n");

    if (argc > 3)
    {
	printf("Usage: diskcomp <drive name> <drive name>\r\n");
	printf("  <drive name> is a: or b:\r\n");
	s_exit(0L);
    }

/* find out what the source and destination drives are */

    getdef(defdrv,sizeof(defdrv));	/* get default drive name */
    if (argc > 1)			/* source drive present? */
	drvname[FIRST] = argv[1];	/* use it */
    else
	drvname[FIRST] = defdrv;	/* otherwise use default */
    if (argc > 2)			/* dest drive present? */
	drvname[SECOND] = argv[2];	/* use it */
    else
	drvname[SECOND] = defdrv;		/* otherwise use default */

/* check the source and destination drives for valid names */

    if (!chkdrv(FIRST))
	return;
    if (!chkdrv(SECOND))
	return;

/* set samedrive flag if the first and second drives are the same */

    samedisk = !strncmp(drvname[FIRST],drvname[SECOND],3);
    fnum[FIRST] = fnum[SECOND] = 0;

/* try allocate a humongous buffer to hold tracks being copied.
   or a small one if we can't get a big one. */

    trysize = NTRACKS;
    while (trysize)
    {
	if (getbuffer(trysize)) break;
	if (trysize > 10)
	    trysize -= 10;
	else
	    trysize--;
    }
    if (!trysize)
    {
	printf("Couldn't even allocate a 1-track buffer!  Aborting.");
	s_exit(0L);
    }

    while (1)
    {
	docomp();		/* really do the compare */
	printf("Compare another diskette (y/n)? ");
	if (getkey() != 'Y')
	    break;
    }
}


/* chkdrv - check a drive name for validity,
	return TRUE if name is valid, FALSE otherwise.
*/

chkdrv(disk)
WORD disk;
{
    BYTE *name;

    name = drvname[disk];
    name[0] = toupper(name[0]);
    if (name[0] < 'A' || name[0] > 'B' || name[1] != ':' ||
	    name[2] != 0)
    {
	printf("Illegal %s drive: %s",drvtype[disk],name);
	return (FALSE);
    }
    else
	return (TRUE);
}


/* getbuffer - allocate a track buffer to hold the specified number of
	tracks, return true if successful or false if not.  A side
	effect of this function is that trkbuf and tbufsize are set
	to the address of the track buffer and its size in bytes.
*/

getbuffer(size)
LONG size;
{
    struct {
	BYTE *start;
	LONG min;
	LONG max;
    } mapb;
    LONG realsize;

    realsize = size * TRKSIZE;
    mapb.min = mapb.max = realsize;
    ret = s_malloc(1,&mapb);
    if (ret < 0)
	return (FALSE);
    else
    {
	trkbuf = mapb.start;
	tbufsize = realsize;
	return (TRUE);
    }
}


/* docomp - really do the disk compare */

docomp()
{
    BYTE *buffer;
    LONG buflen;
    WORD srctrk,dsttrk;		/* first, second track number */
    WORD chunk;			/* number of tracks that fit into trkbuf */
    WORD ntracks;		/* number of tracks to read at once */

/* initialize some global flags */

    formatting = prompted = FALSE;
    compok = TRUE;

/* figure out how many tracks the track buffer can hold */

    chunk = tbufsize / TRKSIZE;

/* now compare the tracks chunk by chunk */

    for (srctrk = 0; srctrk < NTRACKS; srctrk += chunk)
    {
	ntracks = NTRACKS - srctrk;	/* number of tracks left to read */
	if (ntracks > chunk)		/* more than one chunk left? */
	    ntracks = chunk;		/* read only one chunk */

    /* read the tracks from the first disk and save them in big buffer */

	buffer = trkbuf;		/* pointer to track buffer */
	select(FIRST);			/* ask user to insert first disk */
	for (dsttrk = srctrk; dsttrk < srctrk + ntracks; dsttrk++)
	{
	    rdtrack(fnum[FIRST],dsttrk,buffer);/* read one track */
	    buffer += TRKSIZE;			/* increment buffer pointer */
	}

    /* read the tracks from the second drive, compare with first */

	buffer = trkbuf;			/* pointer to track buffer */
	select(SECOND);
	for (dsttrk = srctrk; dsttrk < srctrk + ntracks; dsttrk++)
	{
	    rdtrack(fnum[SECOND],dsttrk,onetrack);	/* read one track */
	    cmptrack(onetrack,buffer,dsttrk);		/* compare tracks */
	    buffer += TRKSIZE;			/* increment buffer pointer */
	}
    }
    dskclose(FIRST);				/* close the first disk */
    dskclose(SECOND);				/* close the second disk */
    if (compok)
	printf("Compare OK\r\n");
}


/* select - open the desired disk drive if not open, prompt the user
	to insert the proper disk if necessary */

select(disk)
WORD disk;
{
    if (samedisk)		/* single drive compare? */
    {
	if (disk == FIRST)
	    dskclose(SECOND);
	else
	    dskclose(FIRST);
	dskprompt(disk);	/* prompt for the new disk */
	keyprompt();		/* prompt user to press a key */
	dskopen(disk);		/* open the selected disk */
    }
    else			/* different drives */
    {
	if (!prompted)		/* have we prompted user to insert disks? */
	{
	    dskprompt(FIRST);	/* prompt for first disk */
	    dskprompt(SECOND);	/* prompt for second disk */
	    keyprompt();	/* prompt user to hit a key */
	    dskopen(FIRST);	/* open the first disk */
	    dskopen(SECOND);	/* open the second disk */
	    prompted = TRUE;	/* set flag so we don't do this again */
	}
    }
}


/* dskopen - open the specified disk drive (first or second) */

dskopen(disk)
WORD disk;
{
    if ((fnum[disk] = xopen(drvname[disk],0)) < 0)
    {
	printf("Error %lx opening %s.  Aborting.",fnum[disk],drvname[disk]);
	s_exit(0L);
    }
}


/* dskclose - close the specified disk drive (FIRST or SECOND) */

dskclose(disk)
WORD disk;
{
    if (fnum[disk])			/* not already closed? */
	s_close(0,fnum[disk]);
    fnum[disk] = 0L;		/* mark it closed */
}


/* dskprompt - prompt user to insert the specified disk (FIRST or SECOND) */

dskprompt(disk)
WORD disk;
{
    printf("Insert %s diskette in Drive %s\r\n",drvtype[disk],drvname[disk]);
}


/* keyprompt - prompt user to hit any key, then wait for the key */

keyprompt()
{
    BYTE ch;

    printf("Press any key when ready...");
    getkey();
}


/* rdtrack - read a track */

rdtrack(fnum,track,buffer)
LONG fnum;
WORD track;
BYTE *buffer;
{
    dotrack(fnum,track,buffer,SPECREAD);
}


/* dotrack - read or write a track */

dotrack(fnum,track,buffer,func)
LONG fnum;
WORD track;
BYTE *buffer;
WORD func;			/* special function number */
{
    union {
	struct {
	    BYTE uhead;
	    BYTE usect;
	    WORD ucyl;
	} hsc;
	LONG offset;
    } u;

/* pack the head, sector and cylinder into a single longword */

    u.hsc.uhead = track & 1;
    u.hsc.usect = 1;
    u.hsc.ucyl = track >> 1;

/* read/write the the whole track in at once (saves time) */

    ret = s_special(func,0,fnum,buffer,TRKSIZE,u.offset,0L);
    if (ret < 0)			/* any errors? */
	printf("Error %lx reading diskette\r\n",ret);
}


/* cmptrack - compare tracks, print error message if different */

cmptrack(buf1,buf2,track)
REG BYTE *buf1,*buf2;
WORD track;
{
    WORD head,cyl;
    REG WORD count;

    if (track == 0)		/* track 0? */
	count = TRK0SIZE;	/* only care about first 512 bytes */
    else
	count = TRKSIZE;

    while (count--)
    {
	if (*buf1++ != *buf2++)
	{
	    head = track & 1;
	    cyl = track >> 1;
	    printf("Compare error on head %d, cylinder %d\r\n",head,cyl);
	    compok = FALSE;
	    return;
	}
    }
}


/* getkey - get a single key from the keyboard, convert it to upper case,
	return it */

getkey()
{
    BYTE ch;

/* throw away any typed-ahead characters */

    while (tahead())
	s_read(0,STDIN,&ch,1L,0L);

/* read one character (non-buffered) */

    ret = s_read(0,STDIN,&ch,1L,0L);
    printf("\r\n");
    if (ret <= 0)
	return (0);
    else
	return (toupper(ch) & 0xff);
}


/* tahead - return number of characters in type-ahead buffer */

tahead()
{
    WORD tahead;

    if ((ret = s_get(CONTAB,STDIN,&tahead,(LONG)sizeof(tahead))) < 0)
    {
	printf("Unable to GET console table: error %lx\r\n",ret);
	return (0);
    }
    return (tahead);
}


/* toupper - convert character to upper case */

toupper(c)
BYTE c;
{
    if (c >= 'a' && c <= 'z')
	return(c - 'a' + 'A');
    else
	return (c);
}


/* getdef - get the literal value of default: */

getdef(buf,size)
BYTE *buf;
WORD size;
{
    if (s_define(0x4002,"default:",buf,(LONG)size) < 0)
	strcpy(buf,"a:");
}
