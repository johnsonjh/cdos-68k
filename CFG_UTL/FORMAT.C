/***********************************************************************
*  format.c - quick and dirty format for CDOS 68K on VME/10
*
*  VERSION  DATE	WHO	CHANGE
*  =======  ====	===	======
*  1.5	    03/24/86	MA	Close disk after writing
*				booter, to avoid media change errors
*				putting system files.
*  1.4	    02/24/86	MA	Use default drive if none specified. 
*  1.3	    01/23/86	MA	First sector in track is now 1, not 0.
*				This was changed in disk driver, making
*				previous FORMAT incompatible.
***********************************************************************/

/***********************************************************************
*	VME/10 DISK FORMAT (a brief summary)
*
*	1. TENbug sector 0 (256 bytes) contains information about where
*	   the booter is and where the Disk Configuration area is.
*	2. TENbug sector 1 (256 bytes) is the DCA.  It contains information
*	   about how big the disk is (i.e. physical format).
*	3. On the floppy disk, there are 12 more 128-byte sectors
*	   following the TENbug sectors that aren't used.  This fills
*	   up the first track (which is single density).  These sectors
*	   are not considered part of the system area by the driver,
*	   i.e. reads/writes of the system area skip over them.
*	4. The booter sectors are 16 256-byte sectors (on a hard disk)
*	   or 8 512-byte sectors (on the floppy disk).  The first few
*	   bytes of the booter contain a PC-DOS-like BPB (bios parameter
*	   block).  Then follows the booter code and data.  The first
*	   two long words of the BPB are used by TENbug to set up the
*	   initial SP and PC for the booter.  Following the BPB is the
*	   booter code and data.  The booter is read from the BOOTER.68K
*	   file.  The FORMAT utility fills in the BPB itself.
*	5. The FATs follow.  Just like DOS.
*	6. The root directory follows.  Just like DOS.
*
* NOTES:
*	1. The BPB is DOS compatible, so all words are byte swapped to
*	   be like the 8086.  This is done so that the driver can read
*	   both VME/10 format disks and IBM PC disks using common
*	   code.
*	2. The array of MDB structures, called "mdbs", and the "ncyls"
*	   array, contain all the information about disk formats.  To
*	   support new drive capacities and formats, change these two
*	   structures.
*	3. There is some inconsistency in the MDBs regarding the SYSSIZE
*	   and HIDDEN sector counts.  This is the result of the crazy
*	   floppy format on the VME/10, where track 0 is a different
*	   format from the rest of the disk.
***********************************************************************/

#include "portab.h"
#include "stdio.h"


EXTERN LONG xopen(),xclose(),xread();		/* in XIO.C */
EXTERN LONG s_read(),s_write(),s_special();	/* in SYSLIB.L68 */
EXTERN LONG s_define();				/* in SYSLIB.L68 */
EXTERN LONG putsys();				/* in PUTSYS.C */


/* SIZE OF SYSTEM AREA:
	2*256:	two versados sectors for Volume ID and Disk Configuration Area
		that TENbug needs to boot
	8*512:	the booter itself
*/

#define SYSSIZE 0x1200


/* SIZE OF LARGEST POSSIBLE FAT:
	22*256:	the fatsize for a 40 Meg hard disk
*/

#define MAXFATSZ	0x1600


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

#define MDB struct _mdb
MDB {
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
};


/* MDBs for various kinds of disks.  WARNING: mdirsize * 32
   must be an exact multiple of msectsize, or code herein
   will not work right.  */

MDB mdbs [5] = {
{	/* 0 - Floppy disk */
	512,	0,	8*158,	8,	4,	2,	0xff,
	1,	224,	2,	1,	16,	SYSSIZE
},
{	/* 1 - 5 Meg hard disk */
	256,	18,	19566L,	32,	8,	2,	0xf8,
	15,	512,	2,	1,	0,	SYSSIZE
},
{	/* 2 - 10 Meg hard disk */
	256,	18,	39150L,	32,	16,	2,	0xf8,
	15,	512,	4,	1,	0,	SYSSIZE
},
{	/* 3 - 15 Meg hard disk */
	256,	18,	58734L,	32,	16,	2,	0xf8,
	22,	512,	6,	1,	0,	SYSSIZE
},
{	/* 4 - 40 Meg hard disk */
	256,	18,	159342L,32,	64,	2,	0xf8,
	15,	512,	6,	1,	0,	SYSSIZE
}};


/* Number of cylinders for the different kinds of disks (not in the MDB) */

WORD ncyls[5] = { 80, 306, 306, 306, 830 };


/* CP/M-68K program header - see chap. 3  CP/M-68K Programmer's Guide */

struct {
    WORD magic;
    LONG textsize;
    LONG datasize;
    LONG bsssize;
    LONG symsize;
    LONG stksize;
    LONG textstart;
    WORD reloc;
} header;


/* command line switches */

#define MAXSWITCH 4
BYTE swboot,swsys,swvol,swzap;	/* true if the switch is set */
BYTE *swvar[MAXSWITCH] = {		/* pointer to the switch variables */
    &swboot,
    &swsys,
    &swvol,
    &swzap
} ;
BYTE swchar[MAXSWITCH] = {		/* the switch characters */
    'B',
    'S',
    'V',
    'Z'
} ;


/* misc. local variables */

LONG dfnum;		/* disk file number */
LONG ret;		/* return code from cdos */
BYTE sysarea[0x2000];	/* general purpose buffer */
BYTE fat[MAXFATSZ];	/* buffer for largest possible FAT */
WORD datasect;		/* first data sector (after directory) */
WORD type;		/* disk type */
MDB  mdb;		/* copy of our working MDB */
BYTE diskname[16];	/* name of drive being formatted (A:, B:, etc.) */
LONG bfnum;		/* booter file number */
LONG bootlen;		/* length of booter code+data */
BYTE volume[11];	/* volume name */
BYTE linebuf[80];	/* general purpose input line buffer */
LONG badsize;		/* total size of bad areas on disk */
LONG sysfiles;		/* total size of system files */
BYTE defdrv[16];	/* default source drive name */


main(argc,argv)
WORD argc;
BYTE *argv[];
{
    BYTE *p;
    WORD argp,i;

    printf("VME/10 FORMAT  V1.5\r\n");

/* check the options on the command line */

    for (argp = 1; argp < argc; argp++)
    {
	p = argv[argp];		/* point to the argument */
	if (*p++ != '-')	/* check the switch character */
	{			/* must be a drive name */
	    if (diskname[0])	/* drive already specified? */
		error("Drive specified more than once.");
	    strcpy(diskname,argv[1]);	/* save the drive name */
	}
	else			/* must be a switch */
	{
	    for (i = 0; i < MAXSWITCH; i++)	/* look up the switch */
	    {
		if (toupper(*p) == swchar[i])	/* found a match? */
		{
		    *swvar[i] = 1;
		    break;
		}
	    }
	    if (i == MAXSWITCH)			/* switch not valid? */
	    {
		usage();
		s_exit(1L);
	    }
	}
    }

/* check the drive name for validity */

    if (!diskname[0])			/* drive specified? */
	getdef(diskname,sizeof(diskname));	/* get default drive name */
    diskname[0] = toupper(diskname[0]);
    if (diskname[0] < 'A' || diskname[0] > 'D' || diskname[1] != ':' ||
	    diskname[2] != 0)
    {
	printf("Illegal drive: %s",diskname);
	s_exit(1L);
    }

    gettype();			/* get disk type */
    getok();			/* ask if they want to destroy disk */
    opendisk();
    config();
    format();
    getboot();
    putboot();
    getvol();
    putdir();
    closedisk();
    if (swsys)
	sysfiles = putsys(diskname);
    summary();
    s_exit(0L);
}


/* usage - print a usage summary for this program */

usage()
{
    printf("Usage: format <drive name> <option>...\r\n");
    printf("  <drive name> is a:, b:, c: or d:\r\n");
    printf("  <option>.. is zero or more of the following:\r\n");
    printf("    -b    make disk bootable\r\n");
    printf("    -s    make disk bootable and copy all system files.\r\n");
    printf("    -v    prompt for volume name\r\n");
}


/* opendisk - open the disk drive */

opendisk()
{

/*  printf("Opening disk device...please be patient...\r\n");  */

    if ((dfnum = xopen(diskname,2)) < 0)	/* open for read/write */
    {
	printf("Open error: %lx ...format aborted.\r\n ",dfnum);
	s_exit(1L);
    }
}


/* closedisk - close the disk drive */

closedisk()
{
    xclose(dfnum);
}


/* gettype - prompt for disk type, and copy the MDB for that type into "mdb" */

gettype()
{
    if (diskname[0] < 'C')	/* floppy disk? */
	type = 0;		/* always the same type */
    else			/* hard disk - could be 1 of 4 types */
    {
	printf("Drive types:\r\n");
	printf(" 1 = 5 Megabytes\r\n");
	printf(" 2 = 10 Megabytes\r\n");
	printf(" 3 = 15 Megabytes\r\n");
	printf(" 4 = 40 Megabytes\r\n");
	printf("Select a drive type [1-4], else 'ESC' to exit: ");
	if (!(type = getkey()))
	    error("Drive type not specified.");
	type -= '0';
	if (type < 1 || type > 4)
	    error("Illegal drive type.");

    }
    movb(&mdbs[type],&mdb,sizeof(mdb));	/* copy the MDB */

/* set up some other global variables that describe this driver,
   like the first data sector on the drive */

    datasect = mdb.mfirstsec + mdb.mhidden
	     + (mdb.mnfats * mdb.mnfrecs)
	     + (mdb.mdirsize / (mdb.msectsize / 32));
}


/* getok - get user's ok to format the disk */

getok()
{
    if (type == 0)			/* floppy disk? */
	printf("Insert diskette into drive %s\r\n",diskname);
    printf("WARNING, ALL DATA ON DISK DRIVE %s WILL BE LOST!\r\n",diskname);
    printf("Proceed with FORMAT (Y/N)? ");
    if (getkey() != 'Y')
	error("Format aborted.");
}


/* config - send new MDB to driver */

config()
{
/* call the driver to really set up the MDB */

/*  printf("Calling SPECIAL to initialize MDB\r\n"); */
    ret = s_special(0x48,0,dfnum,&mdb,(LONG)sizeof(mdb),0L,0L);
    chkret("F_SPECIAL (function 8)");
}


/* format - lay down the physical disk format */

format()
{
    WORD cyl, head;


    setb(0,fat,sizeof(fat));		/* clear the fat */
    if (swzap)
    {
	printf("Skipping physical format; disk assumed to be preformatted");
	printf(" and perfect.\r\n");
	return;
    }
    printf("Formatting\r\n");
    for (cyl = 0; cyl < ncyls[type]; cyl++)
	for (head = 0; head < mdb.mnheads; head++)
	{
	    printf("Cyl %d Head %d\r",cyl,head);
	    fmtrack(cyl,head);		/* do the physical format */
	    rdtrack(cyl,head);		/* verify the track by reading it */
	}
    printf("                       \r\nFormat complete\r\n");
}


/* fmtrack - format a single track */

fmtrack(cyl,head)
WORD cyl,head;
{

/* set up the format track parameter block */

    ftrkpb.fhead = head;
    ftrkpb.fzero1 = 0;
    ftrkpb.fcylinder = cyl;
    if (type == 0 && cyl == 0 && head == 0)	/* track 0 on floppy? */
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

    ret = s_special(0x83,0,dfnum,0L,0L,&ftrkpb,(LONG)sizeof(ftrkpb));
    if (ret < 0)
    {
	printf("\r\nError %lx formatting track\r\n",ret);
	badtrack(cyl,head);
    }
}


/* rdtrack - read a track to verify that formatting was OK */

rdtrack(cyl,head)
WORD cyl,head;
{
    LONG size;			/* size of one track */
    union {
	struct {
	    BYTE uhead;
	    BYTE usect;
	    WORD ucyl;
	} hsc;
	LONG offset;
    } u;

/* compute the size of one track */

    size = mdb.msectrk * mdb.msectsize;
    if (type == 0 && head == 0 && cyl == 0)	/* floppy disk and track 0? */
	size /= 2;				/* single density track */

/* pack the head, sector and cylinder into a single longword */

    u.hsc.uhead = head;
    u.hsc.usect = 1;
    u.hsc.ucyl = cyl;

/* read the the whole track in at once (saves time) */

    if (size > sizeof(sysarea))
    {
	printf("Holy cow, your system area isn't big enough for a track!");
	s_exit(1L);
    }
    ret = s_special(0x86,0,dfnum,sysarea,size,u.offset,0L);
    if (ret < 0)
    {
	printf("\r\nError %lx reading track\r\n",ret);
	badtrack(cyl,head);
    }
}


/* badtrack - mark a track as bad by setting the FAT entries for the
   clusters containting the bad track to FF7.  If the bad track
   is one that is used by the directory, FATs or booter, the disk
   is not usable, so exit.  */

badtrack(cyl,head)
WORD cyl,head;
{
    LONG sector,endsect,longcyl,longhead;
    WORD cluster,endcluster;

/* compute the actual sector number of the first sector on the track,
   and see if it is one of the system sectors that must be good. */

    longcyl = cyl & 0xffffL;
    longhead = head;
    sector = ((cyl * mdb.mnheads) + head) * mdb.msectrk;
    if (sector < datasect)
    {
	printf("Bad track: cylinder %d head %d ... disk not usable.\r\n",
		cyl,head);
	s_exit(1L);
    }

/* mark as BAD all FAT entries for clusters that lie on this track */

    endsect = sector + mdb.msectrk - 1;		/* last sector on track */
    cluster = (sector - datasect)/mdb.msecblk + 2;
    endcluster = (endsect - datasect)/mdb.msecblk + 2;
#if 0
    printf("\r\nBadtrack: cyl %d head %d sect %ld clust %d endclust %d\r\n",
		cyl,head,sector,cluster,endclust);
#endif
    while (cluster <= endcluster)
    {
	setfat(cluster,0xff7);
	badsize += mdb.msecblk * mdb.msectsize;
	cluster++;
    }
}


/* setfat - set a FAT entry */

setfat(cluster,value)
WORD cluster,value;
{
    WORD offset,fatword,mask;

    if (cluster & 1)
    {
	mask = 0xf;			/* use only high 12 bits */
	value <<= 4;			/* shift value to high 12 bits */
    }
    else
    {
	mask = 0xf000;			/* use only low 12 bits */
	value &= 0xfff;			/* ignore upper 4 bits of value */
    }
    offset = cluster + (cluster >> 1);	/* multiple cluster by 1.5 */
    fatword = getword(&fat[offset]);	/* get word from FAT */
    putword(&fat[offset],(fatword & mask) | value);	/* modify it */
}

/* TENbug diagnostic bytes that have to be written to offset 0x40 in
   very first sector on the disk.  TENbug only checks this on the
   internal hard disk, as far as I can tell. */

MLOCAL LONG diaglist[16] = {
    0x0f1e2d3c,0x4b5a6978,0x8796a5b4,0xc3d2e1f0,
    0x0f1e2d3c,0x4b5a6978,0x8796a5b4,0xc3d2e1f0,
    0xf1f2f4f8,0xf9fafcfe,0xff7fbfdf,0xef6fafcf,
    0x4f8f0f07,0x0b0d0e06,0x0a0c0408,0x04020100
} ;


/* getboot - set up the sysarea buffer to contain the proper TENbug
   information for booting, followed by the booter itself.  */

getboot()
{
    readboot();				/* read booter.68k file */

/* set up the volume ID sector (TENbug sector 0) */

    movb("CDOS",sysarea,4);		/* volume ID */
    syslong(0x14,type == 0 ? 8L : 2L);	/* first versados sector to transfer */
    sysword(0x18,16);			/* number of sectors to transfer */
    syslong(0x1e,header.textstart);	/* where to load booter */
    movb(diaglist,&sysarea[0x40],sizeof(diaglist));	/* TENbug diags */
    syslong(0x90,1L);		/* address of configuration sector */
    sysarea[0x94] = 1;		/* length of configuration sector */
    movb("MOTOROLA",&sysarea[0xf8],8);	/* silly string */

/* set up the configuration sector (TENbug sector 1) */

    sysword(0x108,type == 0 ? 0x0f : 0x10);	/* attributes word */
    sysarea[0x118] = mdb.msectrk;		/* sectors per track */
    sysarea[0x119] = mdb.mnheads;		/* number of heads */
    sysword(0x11a,ncyls[type]);			/* number of cylinders */
    sysword(0x11e,mdb.msectsize);		/* sector size */
    sysword(0x124,ncyls[type]/2);		/* precompensation cylinder */

/* set up the funky DOS-like boot record (also known as the BPB or
   BIOS Parameter Block).  It is described in the IBM DOS Tech Ref manual */

    syslong(0x200,bootlen+header.bsssize+header.textstart+0x1000L); /* sp */
    syslong(0x204,header.textstart + 0x34L);	/* PC skips past bootrec */
    dosword(0x20b,mdb.msectsize);	/* bytes per sector */
    sysarea[0x20d] = mdb.msecblk;	/* sectors per cluster */
    dosword(0x20e,mdb.mfirstsec);	/* reserved sectors */
    sysarea[0x210] = mdb.mnfats;	/* number of fats */
    dosword(0x211,mdb.mdirsize);	/* number of root dir entries */
    if (mdb.mnsectors >= 0x10000L)	/* have to use extended size? */
    {
	dosword(0x213,0);		/* this means use extension for size */
	doslong(0x21e,mdb.mnsectors);	/* extension contains actual size */
    }
    else
	dosword(0x213,(WORD)mdb.mnsectors);	/* number of sectors */
    sysarea[0x215] = type == 0 ? 0xff : 0xf8;	/* media type - silly */
    dosword(0x216,mdb.mnfrecs);		/* number of sectors in a single FAT */
    dosword(0x218,mdb.msectrk);		/* sectors per track */
    dosword(0x21a,mdb.mnheads);		/* number of heads */
    dosword(0x21c,(WORD)mdb.mhidden);	/* number of hidden sectors */
}


/* readboot - read the BOOTER.68K text and data into the system area buffer
   just after the two TENBUG sectors */

readboot()
{
    LONG actual;

    if (!swboot & !swsys) return;		/* -B and -S not specified */

    if ((bfnum = xopen("booter.68k",0)) < 0)
	bfnum = xopen("system:/booter.68k",0);
    if (bfnum < 0)
    {
	printf("Unable to open BOOTER.68K: error code %lx\r\n",bfnum);
	return;
    }
    if ((actual = xread(bfnum,&header,(LONG)sizeof(header))) != sizeof(header))
    {
	printf("Error reading boot header: return code was %lx, should be %x.\r\n",
		actual,sizeof(header));
	printf("Unable to read header from booter file.\r\n");
	return;
    }
    bootlen = header.textsize + header.datasize;
    if (bootlen > 0x1000 || bootlen < 0x100)
    {
	printf("Invalid booter length: %lx\r\n",bootlen);
	return;
    }
    if (header.magic != 0x601a && header.magic != 0x601c)
    {
	printf("Invalid booter magic word: %x\r\n",header.magic);
	return;
    }
    if ((actual = xread(bfnum,&sysarea[0x200],bootlen)) != bootlen)
	printf("Error reading booter: return code was %lx, should be %lx.\r\n",
		actual,bootlen);
    xclose(bfnum);
}


/* putboot - write the TENbug sectors and booter to the disk */

putboot()
{

/* write it to the disk! */

    ret = s_special(0x41,0,dfnum,sysarea,(LONG)SYSSIZE,0L,0L);
    if (ret < 0)
	printf("Error %lx writing system area\r\n",ret);
}
    

/* getvol - prompt for and read a volume name from the user */

getvol()
{
    LONG actual;

    if (!swvol) return;			/* they don't want a volume name */
    printf("Volume name (1 to 11 chars): ");
    actual = xread(STDIN,linebuf,(LONG)sizeof(linebuf));
    printf("\r\n");
    if (actual <= 0)
	swvol = FALSE;
    else
    {
	if (actual > sizeof(volume))		/* volume name too long? */
	    actual = sizeof(volume);		/* truncate it */
	setb(' ',volume,sizeof(volume));	/* space fill volume */
	movb(linebuf,volume,(WORD)actual);	/* save volume name */
    }
}

    
/* putdir - write out the FATS and directory */

putdir()
{
    LONG offset;
    WORD nfats;
    LONG fatsize;
    WORD dirrecs;

/* write out the FATS */

    offset = mdb.mfirstsec * mdb.msectsize;	/* offset of first FAT */
    fatsize = mdb.mnfrecs * mdb.msectsize;	/* size of one FAT */
    setb(mdb.mfatid,fat,3);			/* set magic bytes in FAT */
    for (nfats = 0; nfats < mdb.mnfats; nfats++)
    {
	ret = s_write(0,dfnum,fat,fatsize,offset);
	if (ret != fatsize)
	{
	    printf("Error writing FAT: return was %lx, should be %lx\r\n",
			ret,fatsize);
	}
	offset += fatsize;
    }

/* set up the volume name directory entry, if required */

    setb(0,sysarea,mdb.msectsize);		/* clear directory sector */
    if (swvol)					/* volume name required? */
    {						/* create volume dir entry */
	movb(volume,sysarea,sizeof(volume));	/* first put in the name */
	sysarea[sizeof(volume)] = 8;		/* flag it as a volume name */
    }

/* write out the directory records */

    dirrecs = mdb.mdirsize / (mdb.msectsize / 32);
    while (dirrecs--)				/* more records to write? */
    {
	ret = s_write(0,dfnum,sysarea,(LONG)mdb.msectsize,offset);
	if (ret != mdb.msectsize)
	{
	    printf("Error writing directory: return was %lx, should be %x\r\n",
			ret,mdb.msectsize);
	}
	offset += mdb.msectsize;
	setb(0,sysarea,mdb.msectsize);	/* clear directory sector */
    }
}


/* summary - print a summary of disk size statistics */

summary()
{
    LONG n,fatsize,dirsize,hidden;

    n = mdb.msectsize;
    n *= mdb.mnsectors;
    fatsize = mdb.mnfrecs * mdb.msectsize * mdb.mnfats;	/* size of FATS */
    dirsize = mdb.mdirsize * 32;		/* size of directory */
    hidden = mdb.mfirstsec * mdb.msectsize;	/* stuff before FATS */
    printf("%11ld bytes of total disk space\r\n",n);
    if (sysfiles)
	printf("%11ld bytes used by system\r\n",sysfiles);
    if (badsize)
	printf("%11ld bytes in bad sectors\r\n",badsize);
    printf("%11ld bytes available on disk\r\n",n-sysfiles-badsize-
	    fatsize-dirsize-hidden);
}


/* getdef - get the literal value of default: */

getdef(buf,size)
BYTE *buf;
WORD size;
{
    if (s_define(0x4002,"default:",buf,(LONG)size) < 0)
	strcpy(buf,"a:");
}
