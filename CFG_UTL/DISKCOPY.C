/***********************************************************************
*  diskcopy.c - quick and dirty diskette copy for CDOS 68K on VME/10
*
*  VERSION  DATE	WHO	CHANGE
*  =======  ====	===	======
*  1.2      02/21/86	MA	Use default drive for unspecified drives
*  1.1	    02/13/86	MA	Give up on write or format errors.
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
#define SOURCE		0			/* parameter to select() */
#define DEST		1			/* parameter to select() */

#define CONTAB		0x30			/* console table type */

#define SPECIFMT	0x48			/* special init format func */
#define SPECREAD	0x86			/* special read function */
#define SPECWRITE	0xc7			/* special write function */
#define SPECFMT		0x83			/* special format function */


/* FORMAT TRACK special parameter block */

struct {
    BYTE fhead;
    BYTE fzero1;
    WORD fcylinder;
    BYTE fdens;
    BYTE ffill;
    WORD fbytesec;
    WORD fsectrk;
    WORD fsector;
} ftrkpb;


/* MDB - media descriptor block - see fig. 8-6 Concurrent System Guide */

struct {
    WORD msectsize;
    WORD mfirstsec;
    LONG mnsectors;
    WORD msectrk;
    WORD msecblk;
    BYTE mnfats;
    BYTE mfatid;
    WORD mnfrecs;
    WORD mdirsize;
    BYTE mnheads;
    BYTE mformat;
    LONG mhidden;
    LONG msyssize;
} mdb =
{
	512,	0,	8*158,	8,	4,	2,	0xff,
	1,	224,	2,	1,	16,	TRKSIZE+512
};


/* misc. local variables */

LONG fnum[2];		/* file numbers of source and destination drives */
BYTE *drvname[2];	/* names of the source and destination drives */
WORD access[2] =	/* access code for source, destination drives */
     { 0, 2 };		/*  as passed to xopen() */
BYTE *drvtype[2] = 	/* drive types used in error messages */
     { "SOURCE", "DEST" };
LONG ret;		/* return code from cdos */
BYTE *trkbuf;		/* pointer to a track buffer */
LONG tbufsize;		/* size of track buffer in bytes */
BYTE samedisk;		/* true if source and dest drive are the same */
BYTE prompted;		/* true if prompted user already */
BYTE formatting;	/* true if printed formatting message */
BYTE linebuf[80];	/* general purpose input line buffer */
WORD badread;		/* count of bad tracks on source diskette */
WORD badwrite;		/* count of bad tracks on target diskette */
BYTE defdrv[16];	/* default source drive name */


/* main program - figure out command line, allocate buffer space,
	then call docopy() to do the real work
*/

main(argc,argv)
WORD argc;
BYTE *argv[];
{
    LONG trysize;

    printf("VME/10 Diskcopy  V1.2\r\n");

    if (argc > 3)
    {
	printf("Usage: diskcopy <source drive name> <destination drive name>\r\n");
	printf("  <drive name> is a: or b:\r\n");
	s_exit(0L);
    }

/* find out what the source and destination drives are */

    getdef(defdrv,sizeof(defdrv));	/* get default drive name */
    if (argc > 1)			/* source drive present? */
	drvname[SOURCE] = argv[1];	/* use it */
    else
	drvname[SOURCE] = defdrv;	/* otherwise use default */
    if (argc > 2)			/* dest drive present? */
	drvname[DEST] = argv[2];	/* use it */
    else
	drvname[DEST] = defdrv;		/* otherwise use default */

/* check the source and destination drives for valid names */

    if (!chkdrv(SOURCE))
	return;
    if (!chkdrv(DEST))
	return;

/* set samedrive flag if the source and destination drives are the same */

    samedisk = !strncmp(drvname[SOURCE],drvname[DEST],3);
    fnum[SOURCE] = fnum[DEST] = 0;

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
    /* initialize some global flags */

	badread = badwrite = 0;
	formatting = prompted = FALSE;

   /* really do the copy */

	docopy();

   /* check for errors, clean up open files */

	dskclose(SOURCE);		/* close the source disk */
	dskclose(DEST);			/* close the dest disk */
	if (badread)
	    printf("The SOURCE diskette was bad.\r\n");
	if (badwrite)
	    printf("The TARGET diskette was bad.\r\n");
	if (badread + badwrite)
	    printf("The TARGET diskette may be unusable.\r\n");
	else
	    printf("Copy complete.\r\n");
	printf("Copy another (y/n)? ");
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


/* docopy - really do the disk copy */

docopy()
{
    BYTE *buffer;
    LONG buflen;
    WORD srctrk,dsttrk;		/* source, destination track number */
    WORD chunk;			/* number of tracks that fit into trkbuf */
    WORD ntracks;		/* number of tracks to read at once */

/* figure out how many tracks the track buffer can hold */

    chunk = tbufsize / TRKSIZE;

/* now copy the tracks chunk by chunk */

    for (srctrk = 0; srctrk < NTRACKS; srctrk += chunk)
    {
	ntracks = NTRACKS - srctrk;	/* number of tracks left to read */
	if (ntracks > chunk)		/* more than one chunk left? */
	    ntracks = chunk;		/* read only one chunk */

    /* read the tracks from the source drive */

	buffer = trkbuf;		/* pointer to track buffer */
	if (!select(SOURCE))		/* ask user to insert source disk */
	    return;
	for (dsttrk = srctrk; dsttrk < srctrk + ntracks; dsttrk++)
	{
	    if (!rdtrack(fnum[SOURCE],dsttrk,buffer))  /* read one track */
		return;
	    buffer += TRKSIZE;			/* increment buffer pointer */
	}

    /* write the tracks out to the destination drive */

	buffer = trkbuf;			/* pointer to track buffer */
	if (!select(DEST))		/* open the dest drive */
	    return;
	for (dsttrk = srctrk; dsttrk < srctrk + ntracks; dsttrk++)
	{
	    if (!wrtrack(fnum[DEST],dsttrk,buffer))	/* write one track */
		return;
	    buffer += TRKSIZE;			/* increment buffer pointer */
	}
    }
}


/* select - open the desired disk drive if not open, prompt the user
	to insert the proper disk if necessary */

select(disk)
WORD disk;
{
    WORD status;

    if (samedisk)		/* single drive copy? */
    {
	if (disk == SOURCE)
	    dskclose(DEST);
	else
	    dskclose(SOURCE);
	dskprompt(disk);	/* prompt for the new disk */
	keyprompt();		/* prompt user to press a key */
	if (!dskopen(disk))	/* open the selected disk */
	    return (FALSE);
    }
    else			/* different drives */
    {
	if (!prompted)		/* have we prompted user to insert disks? */
	{
	    dskprompt(SOURCE);	/* prompt for source disk */
	    dskprompt(DEST);	/* prompt for dest disk */
	    keyprompt();	/* prompt user to hit a key */
	    if (!dskopen(SOURCE))	/* open the source disk */
		return (FALSE);
	    if (!dskopen(DEST))	/* open the dest disk */
		return (FALSE);
	    prompted = TRUE;	/* set flag so we don't do this again */
	}
    }
    return (TRUE);
}


/* dskopen - open the specified disk drive (source or dest) */

dskopen(disk)
WORD disk;
{
    if ((fnum[disk] = xopen(drvname[disk],access[disk])) < 0)
    {
	if (disk == SOURCE)
	    badread++;
	else
	    badwrite++;
	printf("Error %lx opening %s.\r\n",fnum[disk],drvname[disk]);
	return (FALSE);
    }

/* Send the MDB to the disk driver to override any the driver might
   have set up on the select operation during the xopen above.
   This allows formatting of disks previous formatted on PCs, for
   example, and is necessary if we need to format a track later. */

    if (disk == DEST)
	config(fnum[DEST]);
    return (TRUE);
}


/* dskclose - close the specified disk drive (SOURCE or DEST) */

dskclose(disk)
WORD disk;
{
    if (fnum[disk])			/* not already closed? */
	s_close(0,fnum[disk]);
    fnum[disk] = 0L;		/* mark it closed */
}


/* dskprompt - prompt user to insert the specified disk (SOURCE or DEST) */

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


/* fmtrack - format a single track */

fmtrack(fnum,cyl,head)
LONG fnum;
WORD cyl,head;
{

/* set up the format track parameter block */

    ftrkpb.fhead = head;
    ftrkpb.fzero1 = 0;
    ftrkpb.fcylinder = cyl;
    if (cyl == 0 && head == 0)			/* track 0 on floppy? */
    {						/* single density 128 bytes */
	ftrkpb.fdens = 0;
	ftrkpb.fbytesec = 128;
	ftrkpb.fsectrk = 16;
    }
    else
    {
	ftrkpb.fdens = 1;
	ftrkpb.fbytesec = mdb.msectsize;
	ftrkpb.fsectrk = mdb.msectrk;
    }
    ftrkpb.ffill = 0xe5;
    ftrkpb.fsector = 1;

/* call the driver to really do the formatting */

    ret = s_special(SPECFMT,0,fnum,0L,0L,&ftrkpb,(LONG)sizeof(ftrkpb));
    if (ret < 0)
    {
	printf("\r\nError %lx formatting track\r\n",ret);
	badwrite++;
	return (FALSE);
    }
    else
	return (TRUE);
}


/* rdtrack - read a track */

rdtrack(fnum,track,buffer)
LONG fnum;
WORD track;
BYTE *buffer;
{
    return (dotrack(fnum,track,buffer,SPECREAD));
}


/* wrtrack - write a track */

wrtrack(fnum,track,buffer)
LONG fnum;
WORD track;
BYTE *buffer;
{
    return (dotrack(fnum,track,buffer,SPECWRITE));
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

/* format the track if writing and previously had bad tracks */

    if ((func == SPECWRITE) && formatting)
	if (!fmtrack(fnum,(UWORD)u.hsc.ucyl,(UWORD)u.hsc.uhead))
	    return (FALSE);

/* read/write the the whole track in at once (saves time) */

    ret = s_special(func,0,fnum,buffer,TRKSIZE,u.offset,0L);
    if ((ret < 0) && (func == SPECWRITE) && !formatting)
    {
	printf("Formatting while copying\r\n");
	formatting = TRUE;
	if (!fmtrack(fnum,(UWORD)u.hsc.ucyl,(UWORD)u.hsc.uhead))
	    return (FALSE);
	ret = s_special(func,0,fnum,buffer,TRKSIZE,u.offset,0L);
    }
    if (ret < 0)			/* any errors? */
    {
	if (func == SPECREAD)		/* was is a read? */
	{
	    printf("Error %lx reading SOURCE diskette\r\n",ret);
	    badread++;
	}
	else				/* it was a write */
	{
	    printf("Error %lx writing TARGET diskette\r\n",ret);
	    badwrite++;
	}
	return (FALSE);
    }
    else
	return (TRUE);			/* no errors */
}


/* config - send new MDB to driver */

config(fnum)
LONG fnum;
{
/* call the driver to really set up the MDB */

/*  printf("Calling SPECIAL to initialize MDB\r\n"); */
    ret = s_special(SPECIFMT,0,fnum,&mdb,(LONG)sizeof(mdb),0L,0L);
    if (ret < 0)
	printf("Error %lx initializing format on TARGET drive\r\n",ret);
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

