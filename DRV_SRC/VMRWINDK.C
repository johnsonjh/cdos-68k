/*	@(#)vmrwindk.c	1.5		*/

/*************************************************************************
*  REV		WHEN	  WHO	WHAT
*  ===		====	  ===	===
*  1.5		02/14/86  MA	Put in missing parameter to synxfer() in
*				dskselect() to fix bug reading UNIX disks,
*				and ignore resulting read errors.
*  1.4		01/23/86  MA	Change sector numbering on calls that use
*				HEAD-SEC-CYL numbering to start with 1, not 0.
*				This makes us compatible with IBM drivers.
*				Also, file system barfed on UNIX diskettes,
*				so always return PC-DOS mdb on 48tpi disks,
*				no matter what's actually on them.
*  1.3		12/17/85  MA	Recognize IBM disks not by "IBM" string but
*				by JMP instruction at beginning of boot record.
*  1.25.9	10/31/85  MA	Fixed btrmdb() to not truncate number of
*				sectors if greater than 65535 (40 Meg bug).
*  1.25.8	10/21/85  MA	More arithmetic overflow bugs in rwprep().
*  1.25.7	9/25/85	  MA	Make select a little smarter about supporting
*				odd floppy formats (e.g. 256-byte sectors).
*				More error checks in btr2mdb() and dcb2mdb().
*				Fixed bug in dcb2mdb: sectrk in the DCA is
*				is a byte, not a word.  Also btr2mdb wasn't
*				setting the MDB format byte.
*  1.25.6	9/20/85	  MA	Change retry counts from 10 to 2.
*				Return success if sector not found on
*				select, so FORMAT can open unformatted disk.
*  1.25.5	9/9/85	  MA	Do a config() call in SPECIAL function 8.
*				Put error checking into btr2mdb() and
*				dcb2mdb().  Got rid of all MDBSA macro
*				usage, replaced by setmdb() and getmdb().
*  1.25.4	9/5/85	  MA	Changed read/write system area to only
*				write 512 bytes of TENbug/track0 info.
*  1.25.3	8/30/85	  MA	Fixed 64K byte overflow bug in rwprep.
*  1.25.2	8/28/85	  MA	Changed gbxfrcnt,gpscnt,gxfrcnt	to UWORD.
*  1.25.1	8/23/85   MA	Changed >= to > in calcctl to fix
*				PC-DOS media bug.
*  1.25		?/??/??	  BP,JK Original version by Motorola.
*************************************************************************/

/*=======================================================================*/
/*/---------------------------------------------------------------------\*/
/*|									|*/
/*|   Interrupt driven RWIN disk controller driver for Concurrent DOS   |*/
/*|									|*/
/*|	Complies with External Specification 7.00 dated 1984 Dec 07	|*/
/*|									|*/
/*|			Copyright 1984, Motorola Inc.			|*/
/*|									|*/
/*\---------------------------------------------------------------------/*/
/*=======================================================================*/

/* WARNING: This driver does not queue disk requests.  It assumes that	 */
/* a disk operation completes before another operation is requested.	 */

/* This driver assumes that bad hard disk bad blocks are handled by the	 */
/* file system locking them out and not useing them.			 */

#include "portab.h"
#include "vmecr.h"
#include "vmdriver.h"
#include "system.h"
#include "vmdisk.h"

/* maximum number of units for this driver: */
/*	disk a: unitno=0, (Motorola's #fd02) */
/* 	disk b: unitno=1, (Motorola's #fd03) */
/*	disk c: unitno=2, (Motorola's #hd00) */
/*	disk d: unitno=3, (Motorola's #hd01) */
#define	MAXUNITS	4

/* Determine an RWIN disk driver error code */
#define RWNERR(err) (DEVERR(DVR_RWIN,err))

/* Declaration of local functions (some are referenced before definition) */
MLOCAL LONG	sysrdwr(),	rwprep(),	config(),	synxfer(),
		format();
MLOCAL WORD	dskint(),	calcctl();
MLOCAL UWORD	getsword();
MLOCAL VOID	sndcmd(),	sndcnf(),	dcb2mdb(),	btr2mdb(),
		ddoneasr(),	dxfrasr(),	drwasr();
EXTERN VOID	rddat(),	wrdat();

/************************************************************************/
/*									*/
/*	Driver Header Table - must be first thing in data section	*/
/*									*/
/************************************************************************/
LONG		fd_init(),	dsksubdrv(),	dskuninit(),
		dskselect(),	dskflush(),	dskread(),
		dskwrite(),	dskget(),	dskset(),
		dskspecial();
DRVRHDR	fd_dh =
	{
		0,
		MAXUNITS,
		DFSYNCD,
		fd_init,
		dsksubdrv,
		dskuninit,
		dskselect,
		dskflush,
		dskread,
		dskwrite,
		dskget,
		dskset,
		dskspecial,
		NULL, NULL, NULL, NULL,
		(LONG (**)())NULL,
	};

/************************************************************************/
/*									*/
/*	Disk I/O Addresses and Related Constants			*/
/*									*/
/************************************************************************/

#define	DSKASRPRI	200		/* asr priority			*/

#define	DSKCNTL	((BYTE *) 0xf1c0d0)	/* controller address		*/
#define	DSKCNTLSZ	32		/* controller reg block size	*/

#define	DSKVEC		68	/* disk controller interrupt vector	*/

#define	DCXFER		128	/* bytes per disk controller xfer request */
#define MXPSCNT		255	/* maximum physical sector xfer count	*/

/* controller commands */
#define	DSKSTAT		0x00	/* obtain drive status			*/
#define	DSKRECAL	0x01	/* recalibrate drive (step to track 0)	*/
#define	DSKTFRMT	0x06	/* format track				*/
#define	DSKREAD		0x08	/* read disk				*/
#define	DSKSCAN		0x09	/* scan disk (read without data xfer)	*/
#define DSKWRITE	0x0a	/* write disk				*/
#define	DSKCONFG	0xc0	/* configure controller with drive info	*/

/* controller command block control word definitions */
#define	TPI96		0x80	/* specify 96 tpi floppy diskette	*/
#define	TPI48		0x00	/* specify 48 tpi floppy diskette	*/
#define MFM		0x20	/* mfm recording format (double density)*/
#define SS128		0x00	/* sector size: 128 bytes		*/
#define SS256		0x08	/* sector size: 256 bytes		*/
#define SS512		0x10	/* sector size: 512 bytes		*/
#define SS1024		0x18	/* sector size: 1024 bytes		*/
#define DS		0x04	/* double sided media			*/
#define	CIEON		0x02	/* enable command completion interrupt	*/

/* controller status - status register */
#define	BUSY		0x80	/* controller busy			*/
#define	CI		0x40	/* command interrupt			*/
#define	DI		0x10	/* data interrupt			*/
#define	DRQ		0x08	/* data request - can xfer DCXFER bytes	*/
/* controller status - sense block status byte (byte 1) */
#define	RST		0x80	/* reset				*/
#define	RDY		0x08	/* drive ready				*/
/* controller status - sense block error code (byte 0) */
#define	NOERR		0x00	/* no error				*/
#define NOTRK0		0x02	/* no track 0 signal (drive not present)*/
#define INVLLSN		0x03	/* invalid lsn				*/
#define NTRDY		0x04	/* drive not ready or not present	*/
#define SEEKERR		0x05	/* no seek complete			*/
#define	IDNTFND		0x06	/* ID header not found			*/
#define DMNTFND		0x07	/* data mark not found			*/
#define INVLCMD		0x0C	/* invalid command			*/
#define DATAERR		0x12	/* uncorrectable data error		*/
#define	CORRDER		0x13	/* correctable read error		*/
#define WRTPROT		0x14	/* write protected			*/
#define WRTFLT		0x15	/* write fault				*/

#define	DTINTON		0x01	/* enable data interrupt		*/
#define	DTINTOF		0	/* disable data interrupt		*/

/* media information - following entries used by 'get' function		*/
#define MXFFTRCS	4	/* maximum number of fd FAT records	*/
#define MXHFTRCS	30	/* maximum number of hd FAT records	*/
#define MXFFTSZ		2048	/* maximum fd FAT size in bytes		*/
#define MXHFTSZ		4096	/* maximum hd FAT size in bytes		*/
#define MXFDIRSZ	256	/* maximum CDOS fd root directory size	*/
#define MXHDIRSZ	512	/* maximum CDOS hd root directory size	*/

#define	FDSCTSZ		512	/* floppy disk sector size		*/
#define	FDDRSZ		256	/* floppy disk directory size - # entries*/
#define FMD		0xff	/* media descriptor byte for CDOS fd	*/

#define	HDSCTSZ		256	/* hard disk sector size		*/
#define HMD		0xf8	/* media descriptor byte for CDOS hd	*/

/* 5 megabyte hard disk defaults */
#define H5DRS		512	/* CDOS root directory size		*/
#define H5MFR		16	/* maximum number of fat records	*/
#define H5MCYL		305	/* maximum cylinder			*/
#define H5MHD		1	/* maximum head				*/

/* The disk format/system installation utility must set up the disk
   configuration block and booter record with the appropriate disk parameters.
   The disk driver defaults to the 5 megabyte hard disk only.  It obtains
   other disk configurations from the disk configuration block/boot record.
   The MX* defines above must be set to the largest values expected. */
/* How do the MX* defines relate to CP/M disks?  Are they only CDOS? ???? */


/************************************************************************/
/*	Disk I/O Packets						*/
/************************************************************************/

typedef struct			/* disk controller registers	*/
{
	BYTE	diofl0;		/* fill byte			*/
	BYTE	cmdsns;		/* command/sense byte		*/
	BYTE	diofl1;		/* fill byte			*/
	BYTE	intstt;		/* interrupt/status byte	*/
	BYTE	diofl2;		/* fill byte			*/
	BYTE	rst;		/* reset			*/
	BYTE	diofl3;		/* fill byte			*/
	BYTE	ntusd;		/* not used			*/
	BYTE	diofl4;		/* fill byte			*/
	BYTE	data;		/* data				*/
} *DIOP;

typedef struct			/* sense packet			*/
{
	BYTE	ercode;		/* error code			*/
	BYTE	lun;		/* DOS logical unit number	*/
	BYTE	status;		/* status includes controller lun*/
	WORD	pcylnm;		/* physical cylinder number	*/
	BYTE	headnm;		/* head number			*/
	BYTE	sectnm;		/* sector number		*/
	BYTE	n;		/* number sectors left to process*/
	BYTE	snsbt6;		/* sense packet byte 6		*/
	BYTE	snsbt7;		/* sense packet byte 7		*/
	BYTE	snsbt8;		/* sense packet byte 8		*/
	BYTE	snsbt9;		/* sense packet byte 9		*/
} SNSSTR;


/************************************************************************/
/*									*/
/*	Disk Variables							*/
/*									*/
/************************************************************************/

MLOCAL char copyright[] = "Copyright 1984, Motorola Inc.";
MLOCAL char dskver[] = "@(#)vmrwindk.c	1.21	";

/* system map address of disk controller register block */
MLOCAL DIOP	dskcntl;

/* system flag for disk operations */
MLOCAL LONG	dskflag;

/* last sense packet read from disk */
MLOCAL SNSSTR	sns;

/* working MDB for each disk */
MLOCAL MDB	workmdb[MAXUNITS];

/* global variables for passing info to ASR's */
/* decoder information: g* indicates driver global variable		*/
/*	beginning values are used to reset working values if an		*/
/*	operation must be retried due to an error.  note that a dos	*/
/*	disk transfer request might require more than one operation	*/
/*	since an operation is limited to a maximum sector transfer of	*/
/*	255 sectors.							*/
/*	the transfer count (gxfrcnt and gbxfrcmt) is needed since the	*/
/*	controller continues to request data for a write after the	*/
/*	necessary amount of data has been transfered.			*/
MLOCAL BYTE  gunit;		/* disk unit number			*/
MLOCAL WORD  gdskop;		/* operation - read/write		*/
MLOCAL WORD (*gdskfunc)();	/* asr to run on command complete	*/
MLOCAL WORD  gbufflag;		/* user/system address flag		*/
MLOCAL BYTE *gbpdaddr;		/* buffer process descriptor address	*/
MLOCAL BYTE *gepdaddr;		/* event process descriptor address	*/
MLOCAL WORD  grcnt;		/* retry count				*/
/* beginning values */
MLOCAL BYTE *gbbufp;		/* buffer pointer			*/
MLOCAL UWORD  gbxfrcnt;		/* block transfer count			*/
/* working values */
MLOCAL LONG  gnrecs;		/* number of sectors left to transfer	*/
MLOCAL LONG  grecn;		/* beginning record number		*/
MLOCAL UWORD  gpscnt;		/* physical sector count - this transfer*/
MLOCAL UWORD  gxfrcnt;		/* block transfer count - this transfer	*/
MLOCAL BYTE *gbufp;		/* buffer address			*/


/************************************************************************/
/*									*/
/*	Disk Tables							*/
/*									*/
/************************************************************************/

MLOCAL BYTE cnvdsk[MAXUNITS]  = { 2, 3, 0, 1 };  /* convert DOS # to RWIN */
MLOCAL BYTE rcnvdsk[MAXUNITS] = { 2, 3, 0, 1 };	 /* and vice versa */

/************************************************************************/
/*	Media Descriptor Blocks						*/
/************************************************************************/

/* These MDB's are used to initialize the working MDB's when the media
   is not formatted or does not contain MDB information. */

/* MDB contains:
	sector size,	first sector,	number of sectors,
	sectors/track,	sectors/block,	number of fats,
	fat ID,		sectors/fat,	directory size,
	nmber of heads, file system ID,	hidden area size,
	system area size					*/

/* NOTE: The system area size specified for the two 96 tpi DSDD floppy
   diskettes IS the actual number of bytes in the system area.  (Track 0
   must be single density 128-byte sectors in order to be bootable on the
   VME/10.)  Also, in the mf96c10 entry the system area is cylinder
   oriented while the user area is side oriented.  Thus, one of its
   system area tracks is actually "hidden" past the end of the user area. */

/* floppy disk: 96tpi DSDD CDOS VME/10 (Native) */
/* track 0 = 16 SD 128-byte sectors (part of hidden area) */
/* file system id == 1 == 1.5 byte FAT entries */
MLOCAL MDB	mf96n10 =
	{ FDSCTSZ,	0,		8*(160-2),
	  8,		4,		2,
	  FMD,		1,		224,
	  2,		1,		2*8,
	  4*128 + 8*512};

/* floppy disk: 96tpi DSDD CP/M-68K VME/10 */
/* track 0 = 16 SD 128-byte sectors (part of hidden area) */
MLOCAL MDB	mf96c10 =
	{ FDSCTSZ,	0,		8*(160-3),
	  8,		4,		0,
	  0,		0,		FDDRSZ,	/* FDDRSZ correct????*/
	  2,		0,		2*8,
	  16*128 + 2*8*512};

/* floppy disk: 48tpi DSDD CCP/M-86 IBM PC */
MLOCAL MDB	mf48cpc =
	{ FDSCTSZ,	1,		(8*80)-1,
	  8,		4,		0,
	  0,		0,		63,
	  2,		0,		0,
	  0};

/* floppy disk: 48tpi DSDD PCDOS IBM PC */
/* NOTE: this is 8 sec/trk ONLY - controller cannot handle 9 sec/trk! */
MLOCAL MDB	mf48ppc =
	{ FDSCTSZ,	1,		(8*80)-1,
	  8,		2,		2,
	  FMD,		1,		112,
	  2,		1,		0,
	  0};

/* 5 megabyte hard disk */
MLOCAL MDB	mh5 =
	{ HDSCTSZ,	18,		((H5MHD+1)*(H5MCYL+1))*32 - 18,
	  32,		8,		2,
	  HMD,		H5MFR,		H5DRS,
	  H5MHD+1,	1,		0,
	  18*256};


/************************************************************************/
/*									*/
/*	Disk Driver Entry Points					*/
/*									*/
/************************************************************************/

/************************************************************************/
/*	Initialize Disk - Synchronous Function				*/
/*									*/
/*	NOTE: Even though we enable interrupts in this routine, we	*/
/*	don't expect problems.  No interrupts will occur until we send	*/
/*	our first command out in select().				*/
/************************************************************************/

LONG fd_init(unumb)		/* CAUTION - magic function name to CDOS! */
LONG unumb;
{
	MAPPB	pmap;
	VMECRP	vmecr;
	BYTE	unit = (BYTE)unumb;

	if ( unit == 0 )	/* things only done once */
	{
		/* get access to disk controller */
		pmap.mpzero = 0;
		pmap.pbegaddr = DSKCNTL;
		pmap.plength = DSKCNTLSZ;
		dskcntl = (DIOP)mapphys(&pmap);

		/* turn off controller data interrupts */
		dskcntl->intstt = DTINTOF;

		/* get access to VME/10 control registers */
		pmap.pbegaddr = VME10CR;
		pmap.plength = VME10CRSZ;
		vmecr = (VMECRP)mapphys(&pmap);

		/* set up disk interrupt vector */
		setvec(dskint,(LONG)DSKVEC);

		/* enable interrupts in vme/10 cr6 */
		/* I/O channel INT3 and top level mask */
		vmecr->cr6 |= 0xa0;

		/* get system flag - assume that we do */
		dskflag = flagget();

		/* read disk controller sense to get boot disk unit number? */
		/* what would we do with it? */
	}

	/* configure controller for drive */
	switch ( unit )
	{
	case 0:
	case 1:
		/* floppy disk - 96 tpi, ds */
		config(unit, 1, 79, 40);
		return(DVR_RWIN);
	case 2:
	case 3:
		/* hard disk - real config in select */
		config(unit, 0, 0, 255);
		return(DVR_RWIN);
	default:
		return(RWNERR(E_UNITNO));
	}
}

/************************************************************************/
/*	Subdrive - Synchronous Function					*/
/************************************************************************/

LONG dsksubdrv(sdpbp)
SUBDRVPB *sdpbp;
{
	return(RWNERR(E_IMPLEMENT));
}

/************************************************************************/
/*	Uninitialize Disk - Synchronous Function			*/
/************************************************************************/

LONG dskuninit(unit)
{
	/* This driver cannot be uninstalled */

	return(RWNERR(E_IMPLEMENT));
}

/************************************************************************/
/*	Select Disk - Synchronous Function				*/
/************************************************************************/

LONG dskselect(selpbp)
REG DSELPBP selpbp;
{
    LONG	retcode, dcb;
    WORD	maxcyl, maxhead;
    BYTE	bfr[FDSCTSZ];
    REG WORD	dsk;

    dsk = selpbp->sunit;

/* determine disk type and size, set up working mdb, and return copy */

    switch ( dsk )
    {
    case 0:
    case 1:		/* floppy disk */
	setmdb(dsk, &mf96n10);	/* VME/10 CDOS disk */

    /* attempt to read first four sectors of disk containing volume ID
	and the DCA (disk configuration area) */

	retcode = synxfer(dsk, 0L, 4L, bfr, DSKREAD, SYSADDR, NULLPTR);
	switch ( (WORD)(retcode & 0xffff) )
	{
	case E_SUCCESS:	/* 96tpi floppy but is it CDOS or CP/M-68K? */

	    if ( (!strncmp(&bfr[248],"EXORMACS",8)) ||
		 (!strncmp(&bfr[248],"MOTOROLA",8)) )
	    {				/* it's a Motorola disk */

	    /* use the DCA sector to make a more accurate MDB */

		if ((dcb=(*((LONG *)(&bfr[144])))) == 1)
		    dcb2mdb(dsk, &bfr[256]);

	    /* check if it is a Concurrent DOS diskette */

		if ( !strncmp(bfr,"CDOS",4) )
		{	/* VME/10 Concurrent DOS */
		    retcode = synxfer(dsk, *((LONG *)(&bfr[20])),
				1L, bfr, DSKREAD, SYSADDR, NULLPTR);
		    if ( retcode == E_SUCCESS )
			btr2mdb(dsk,bfr); /*btr info*/
		}

	    /* check if it is a CP/M diskette */

		else if ( !strncmp(bfr,"CP/M",4) ) /* CP/M-68K */
		    setmdb(dsk, &mf96c10);

	    /* if not CDOS or CP/M and the DCA is not sector 1,
		assume the DCA is on track 1 and try to read it */

		else if ( dcb != 1)
		{
		    retcode = synxfer(dsk,(dcb*256)/workmdb[dsk].sectsize,
					2L,bfr,DSKREAD,SYSADDR,NULLPTR);
		    if ( retcode == E_SUCCESS )
			dcb2mdb(dsk, bfr);
		    else
			retcode = E_SUCCESS;	/* fake success to avoid */
		}				/* errors on raw opens */
	    }
	    break;

	case E_SEC_NOTFOUND:	/* maybe it's a PC disk? */
	    setmdb(dsk, &mf48ppc);	/* 48tpi PC DOS */

	/* read the IBM boot record (first sector on the disk) */

	    retcode = synxfer(dsk, 0L, 1L, bfr, DSKREAD, SYSADDR,NULLPTR);
	    if ( retcode == E_SUCCESS )
	    {
		if (bfr[0] == 0xe9 || bfr[0] == 0xeb)	/* 8086 JMP? */
		    btr2mdb(dsk, bfr);		/* it's a PC-DOS disk */
		else
		    setmdb(dsk,&mf48ppc);	/* used to be mf48cpc */
	    }
	    else
	    {
		setmdb(dsk,&mf96n10); /*unknown -default*/
		retcode = E_SUCCESS;		/* kludge to avoid getting */
	    } 					/* errors on raw disk opens */
	    break;				/* (file system bug) */

	}
	break;

    case 2:
    case 3:		/* hard disk */
	setmdb(dsk, &mh5);	/* 5 megabyte hard disk */
	maxcyl = H5MCYL;
	maxhead = H5MHD;

    /* attempt to read first two sectors of disk */
	retcode = synxfer(dsk, 0L, 2L, bfr, DSKREAD, SYSADDR, NULLPTR);
	if ( retcode == E_SUCCESS )
	{
	    if ( (!strncmp(&bfr[248],"EXORMACS",8)) ||
		 (!strncmp(&bfr[248],"MOTOROLA",8)) )
	    {	/* it's a Motorola disk */
		maxcyl = *((WORD *)(&bfr[256+26])) - 1;
		maxhead = bfr[256+25] - 1;
		if ( !strncmp(bfr,"CDOS",4) )
		{	/* VME/10 Concurrent DOS */
		    retcode = synxfer(dsk, *((LONG *)(&bfr[20])), 1L, bfr,
					DSKREAD, SYSADDR, NULLPTR);
					if ( retcode == E_SUCCESS )
						btr2mdb(dsk,bfr); /*btr info*/
		}
		else 	/* other VME/10 disk, use media info */
		if ((dcb=(*((LONG *)(&bfr[144])))) == 1)
		    dcb2mdb(dsk, &bfr[256]);
		else
		{	/* dcb not sector 1 */
		    retcode = synxfer(dsk,dcb,1L,DSKREAD,SYSADDR,NULLPTR);
		    if ( retcode == E_SUCCESS )
			dcb2mdb(dsk, bfr);
		}
	    }
	}

    /* configure for hard disk, now that we know what it is */

	retcode = config(dsk, maxhead, maxcyl, 255);
	break;

    default:
	return(RWNERR(E_UNITNO));
    }

    getmdb(dsk,selpbp->smdbp);		/* return the MDB to file system */
    return(retcode);
}


/************************************************************************/
/*	Flush Disk - Synchronous Function				*/
/************************************************************************/

LONG dskflush(flpbp)
DFLUPBP flpbp;
{
	return(E_SUCCESS);	/* no internal buffers */
}

/************************************************************************/
/*	Read Disk - Asynchronous Function				*/
/************************************************************************/

LONG dskread(rwpbp)
REG DRWPBP rwpbp;
{
	return(rwprep(rwpbp, DSKREAD));
}

/************************************************************************/
/*	Write Disk - Asynchronous Function				*/
/************************************************************************/

LONG dskwrite (rwpbp)
REG DRWPBP rwpbp;
{
	return(rwprep(rwpbp, DSKWRITE));
}

/************************************************************************/
/*	Get Disk Information - Synchronous Function			*/
/************************************************************************/

LONG dskget(gpbp)
DGETPBP gpbp;
{
	switch ( gpbp->gunit )
	{
	case 0:
	case 1:
		/* floppy disk */
		gpbp->gdtype = DGTRMED;
		gpbp->gmaxrs = FDSCTSZ;
		gpbp->gopdraddr = NULLPTR;
		gpbp->gmaxfatrecs = MXFFTRCS;
		gpbp->gmaxfatsiz = MXFFTSZ;
		gpbp->gmaxdirsiz = MXFDIRSZ;
		break;
	case 2:
	case 3:
		/* hard disk */
		gpbp->gdtype = 0;
		gpbp->gmaxrs = HDSCTSZ;
		gpbp->gopdraddr = NULLPTR;
		gpbp->gmaxfatrecs = MXHFTRCS;
		gpbp->gmaxfatsiz = MXHFTSZ;
		gpbp->gmaxdirsiz = MXHDIRSZ;
		break;
	}
	return(E_SUCCESS);
}

/************************************************************************/
/*	Set Disk Information - Synchronous Function			*/
/************************************************************************/

LONG dskset()
{
	/* no set operation for disk driver */

	return(RWNERR(E_IMPLEMENT));
}

/************************************************************************/
/*	Special Disk Operation - Synchronous Function			*/
/************************************************************************/

LONG dskspecial(sppbp)
DSPPBP sppbp;
{
	LONG retcode, psn, nsect, nhead, ncyl;
	WORD dsk = sppbp->spunit;

	switch ( sppbp->spfunc)
	{
	case DSPRDSYS:	/* read system area of disk */
		/* does not support reading of system area of CP/M-68K
		   VME/10 diskettes */
		return(sysrdwr(sppbp, DSKREAD));

	case DSPWRSYS:	/* write system area of disk */
		/* does not support writing of system area of CP/M-68K
		   VME/10 diskettes */
		return(sysrdwr(sppbp, DSKWRITE));

	case DSPFRSYS:	/* format system area of disk */
		if ( !(workmdb[dsk].syssize) )
			return(RWNERR(E_INVCMD));
		/* assume floppy disk format for VME/10, at least 2 tracks */
		if ( !(retcode = format(dsk, 0L)) ) retcode = format(dsk, 8L);
		if ( !retcode && (workmdb[dsk].syssize > 16*128 + 8*512) )
			retcode = format(dsk, 16L);	/* third track */
		return(retcode);

	case DSPFRTRK:	/* format track of disk */
		if ( sppbp->spflags & DSPFTCHSN )
			return(RWNERR(E_INVCMD)); /* no chsn reqs */
		/* we ignore density, bytes/sector, sectors/track & fill char */
		psn =(workmdb[dsk].nheads * sppbp->sp.spfrtrk.spfp->spfcylinder
		      + sppbp->sp.spfrtrk.spfp->spfhead) * workmdb[dsk].sectrk;
		return(format(dsk, psn));

	case DSPIFRMT:	/* initialize format (MDB) */
	    /* check correct size? */
		setmdb(dsk, sppbp->sp.spifrmt.spmdbp);

	    /* Try to compute total number of cylinders on the disk from
		the other information in the MDB.  Then send the configuration
		to the controller */

		nsect = workmdb[dsk].nsectors + (dsk < 2 ? 16L : 18L);
		nhead = workmdb[dsk].nheads & 0xffL;
		ncyl = (nsect / nhead) / (dsk < 2 ? 8L : 32L);
		return (config(dsk,(WORD)nhead-1,(WORD)ncyl-1,(WORD)ncyl/2));

	default:
		return(RWNERR(E_IMPLEMENT));
	}
}

 
/************************************************************************/
/*									*/
/*	Support Routines						*/
/*									*/
/************************************************************************/


/************************************************************************/
/*	Send disk command packet					*/
/************************************************************************/

MLOCAL VOID sndcmd(dsk, psn, n, cmd)
REG WORD dsk, n, cmd;
REG LONG psn;
{
	/* write the packet to the controller */
	/* the dskcntl references must NOT be reordered */

	dskcntl->intstt = DTINTON;		/* enable data interrupt */

	dskcntl->cmdsns = cmd;			/* command - byte 0 */
	/* following line assumes psn <= 21 bits long */
	dskcntl->cmdsns = (cnvdsk[dsk] << 5) | (psn >> 16);	/* byte 1 */
	dskcntl->cmdsns = (psn >> 8);				/* byte 2 */
	dskcntl->cmdsns = psn;					/* byte 3 */
	dskcntl->cmdsns = n;					/* byte 4 */
	dskcntl->cmdsns = calcctl(dsk, psn);			/* byte 5 */
}

/************************************************************************/
/*	Caclulate control byte for disk command				*/
/************************************************************************/

MLOCAL WORD calcctl(dsk, psn)
WORD dsk;
LONG psn;
{
	REG WORD  ctl;

	ctl = CIEON | MFM | DS | TPI48; /* default: dsdd, 48tpi */

	/* the hard disk only looks at the interrupt enable bit */

	if ( dsk < 2 )
	{	/* Determine ctl value for floppy disk command */
		if ( (workmdb[dsk].syssize) && (psn < 8) )
		     /* assumes entire track 0 read/written at once, i.e.
		     cannot start at psns 8 - 15 of track 0		*/
			ctl = CIEON | SS128 | DS;
		else
		{
			switch ( workmdb[dsk].sectsize )
			{
			case 256:	ctl |= SS256;	break;
			case 512:	ctl |= SS512;	break;
			case 1024:	ctl |= SS1024;	break;
			default:
			case 128:	ctl |= SS128;	break;
			}
		}
		if ( workmdb[dsk].nsectors > 8*80 )
			ctl |= TPI96;
	}
	return(ctl);
}

/************************************************************************/
/*	Send disk configuration packet					*/
/************************************************************************/

MLOCAL VOID sndcnf(dsk, mxhd, mxcl, prcmp)
REG WORD dsk, mxhd, mxcl, prcmp;
{
	/* write the configuration packet to the controller */
	/* the dskcntl references must NOT be reordered */

	dskcntl->cmdsns = DSKCONFG;		/* command - byte 0 */
	dskcntl->cmdsns = (cnvdsk[dsk] << 5);		/* byte 1 */
	/* following line assumes mxcl <= 13 bits long */
	dskcntl->cmdsns = (mxhd << 5) | (mxcl >> 8);	/* byte 2 */
	dskcntl->cmdsns = mxcl;				/* byte 3 */
	dskcntl->cmdsns = prcmp;			/* byte 4 */
	dskcntl->cmdsns = CIEON;			/* byte 5 */
}

/************************************************************************/
/*	Configure Disk Controller - waits				*/
/************************************************************************/

MLOCAL LONG config(unit, mxhd, mxcl, prcmp)
WORD unit, mxhd, mxcl, prcmp;
{
	LONG mask;

	gdskfunc = ddoneasr;
	gepdaddr = *fd_dh.drvrrlrp;
	flagclr(dskflag);
	sndcnf(unit, mxhd, mxcl, prcmp);
	/*	no drive returns E_UNITNO		*/
	/*	no disk in drive returns E_SUCCESS	*/
	mask = flagevent(dskflag,NULLPTR);
	supif((WORD)F_WAIT,mask);
	return(supif((WORD)F_RETURN,mask));
}

/************************************************************************/
/*	Format Track - waits						*/
/************************************************************************/

MLOCAL LONG format(dsk, psn)
REG WORD dsk;
REG LONG psn;
{
	LONG	mask;

	gdskfunc = ddoneasr;
	gepdaddr = *fd_dh.drvrrlrp;
	flagclr(dskflag);
	sndcmd(dsk, psn, 0, DSKTFRMT);
	mask = flagevent(dskflag,NULLPTR);
	supif((WORD)F_WAIT,mask);
	return(supif((WORD)F_RETURN,mask));
}

/************************************************************************/
/*	Asynchronous Read/Write Preparation				*/
/************************************************************************/

MLOCAL LONG rwprep(rwpbp, op)
REG DRWPBP rwpbp;
WORD	   op;
{
	UWORD chunks;
	REG MDB *m;
	REG LONG longsect,longcyl,longhead;

	/* set up globals for transfer */
	gunit = rwpbp->rwunit;
	m = &workmdb[gunit];
	gdskop = op;
	gepdaddr = *fd_dh.drvrrlrp;
	gbufflag = rwpbp->rwflags & USRADDR;
	gbpdaddr = rwpbp->rwpdaddr;
	gbufp = rwpbp->rwbuffer;
	gbbufp = gbufp;
	if ( rwpbp->rwflags & DRWHSC )	/* DRWHSC implies physical disk addr */
	{
	    longcyl = rwpbp->rw.hsc.rwcylinder & 0xffffL;
	    longhead = rwpbp->rw.hsc.rwhead;
	    longsect = rwpbp->rw.hsc.rwsector - 1;	/* 1, 2, 3, etc. */
	    grecn = ((longcyl*(LONG)m->nheads)+longhead) * (LONG)m->sectrk +
		    longsect;
	}
	else
	    grecn = rwpbp->rw.rwrecord + m->hidden;
	gnrecs = rwpbp->rwnrecs;
	gdskfunc = drwasr;
	gpscnt = (UWORD)gnrecs;
	if ( gpscnt > MXPSCNT ) gpscnt = MXPSCNT;
	grcnt = 2;
	chunks = m->sectsize / DCXFER;	/* avoid overflow */
	gxfrcnt = gpscnt * chunks;
	gbxfrcnt = gxfrcnt;
	/* start transfer */
	flagclr(dskflag);
	sndcmd(gunit, grecn, gpscnt, gdskop);
	return(flagevent(dskflag,rwpbp->rwswi));
}

/************************************************************************/
/*	Synchronous transfer - waits					*/
/************************************************************************/

MLOCAL LONG synxfer(dsk, psn, nsct, bfrp, op, bflg, bpdp)
WORD dsk, op, bflg;
LONG psn, nsct;
BYTE *bfrp, *bpdp;
{
	LONG	mask;
	WORD	sctsiz;

	/* set up globals */
	gunit = dsk;
	grecn = psn;
	gnrecs = nsct;
	gpscnt = gnrecs;
	if ( gpscnt > MXPSCNT ) gpscnt = MXPSCNT;
	gdskop = op;
	sctsiz = workmdb[dsk].sectsize;
	if ( dsk < 2 )	/* special check for VME/10 bootable floppies */
		if ( (workmdb[dsk].syssize) && (psn < 8) )
		     /* assumes entire track 0 read/written at onec, i.e.
		     cannot start at psns 8 - 15 of track 0		*/
			sctsiz = 128;
	gxfrcnt = gpscnt * sctsiz / DCXFER;
	gbxfrcnt = gxfrcnt;
	gbufflag = bflg;
	gbpdaddr = bpdp;
	gbufp = bfrp;
	gbbufp =gbufp;
	gdskfunc = drwasr;
	gepdaddr = *fd_dh.drvrrlrp;
	grcnt = 2;
	/* start transfer */
	flagclr(dskflag);
	sndcmd(gunit, grecn, gpscnt, gdskop);
	mask = flagevent(dskflag,NULLPTR);
	supif((WORD)F_WAIT,mask);
	return(supif((WORD)F_RETURN,mask));
}

/************************************************************************/
/*	Update MDB from Disk Configuration Block			*/
/************************************************************************/

MLOCAL VOID dcb2mdb(dsk, bfr)
WORD dsk;
BYTE bfr[];
{
    MDB m;		/* temporary storage for mdb under construction */

    getmdb(dsk,&m);		/* get the current MDB */

    if (!(m.sectsize = *((WORD *)(&bfr[30])))) return;
    if (!(chksize(dsk,m.sectsize))) return;
    if (!(m.sectrk = *((BYTE *)(&bfr[24])))) return;
    if (!(m.nheads = bfr[25])) return;
    m.mformat = 0;		/* assume RAW media until proven otherwise */

    setmdb(dsk,&m);		/* update the current MDB */
}

/************************************************************************/
/*	GETSWORD - Get swapped word					*/
/************************************************************************/

MLOCAL UWORD getsword(wp)
REG BYTE *wp;
{
	/* what we want is unsigned chars instead of hokey mask */
	return((WORD)(wp[0]&0xff) + (wp[1] << 8) );
}

/************************************************************************/
/*	GETSLONG - Get swapped longword					*/
/************************************************************************/

MLOCAL ULONG getslong(wp)
REG BYTE *wp;
{
	REG LONG temp1,temp2;
	
	temp1 = getsword(wp) & 0xffffL;		/* low word */
	temp2 = getsword(wp+2);			/* high word */
	return ((temp2 << 16) + temp1);		/* combine them */
}

/************************************************************************/
/*	BTR2MDB - Update MDB from Boot Record				*/
/************************************************************************/

MLOCAL VOID btr2mdb(dsk, bfr)
WORD dsk;
REG BTREC *bfr;
{
    MDB m;		/* temporary storage for MDB under construction */

    getmdb(dsk,&m);		/* get current MDB */

    if (!(m.sectsize = getsword(bfr->sctrsz))) return;
    if (!(chksize(dsk,m.sectsize))) return;
    m.firstsec = getsword(bfr->rsvsc);
    if ( !(m.nsectors = getsword(bfr->dksz)) )
	m.nsectors = getslong(bfr->xdksz);
/*	m.nsectors = (bfr->xdksz[3] << 24) + (bfr->xdksz[2] << 16) +
		     (bfr->xdksz[1] << 8) + bfr->xdksz[0]; */
    if (!m.nsectors) return;
    if (!(m.sectrk = getsword(bfr->spt))) return;
    if (!(m.secblk = bfr->clstrsz)) return;
    if (!(m.nfats = bfr->numfats)) return;
    m.fatid = bfr->meddsc;
    if (!(m.nfrecs = getsword(bfr->fatsz))) return;
    if (!(m.dirsize = getsword(bfr->rtdir))) return;
    if (!(m.nheads = getsword(bfr->hds))) return;
    m.mformat = 1;		/* assume 1 1/2 byte FAT entries */
    m.hidden = getsword(bfr->hidsc);
/* don't know syssize - assume default ???? */

    setmdb(dsk,&m);		/* set current MDB */
}

/***********************************************************************
*	CHKSIZE - check for valid sector size
***********************************************************************/

MLOCAL chksize(dsk,sectsize)
WORD dsk,sectsize;
{
    WORD n;

    if (dsk >= 2)			/* hard disk? */
	return (sectsize == HDSCTSZ);	/* only allow 256 byte sectors */

    for (n = 128; n <= FDSCTSZ; n <<= 1)	/* check powers of two */
	if (sectsize == n) return (TRUE);
    return (FALSE);			/* didn't match any possibilities */
}


/***********************************************************************
*	SETMDB - set the working MDB for a given drive
*	GETMDB - get the working MDB
***********************************************************************/

MLOCAL setmdb(dsk,mdbp)
WORD dsk;
MDB *mdbp;
{
    mdbcpy(&workmdb[dsk],mdbp);
}

MLOCAL getmdb(dsk,mdbp)
WORD dsk;
MDB *mdbp;
{
    mdbcpy(mdbp,&workmdb[dsk]);
}

MLOCAL mdbcpy(dest,source)
REG BYTE *dest;
REG BYTE *source;
{
    REG WORD len;

    len = sizeof(MDB);
    while (len--)
	*dest++ = *source++;
}


/************************************************************************/
/*	System Area Read/Write						*/
/************************************************************************/

MLOCAL LONG sysrdwr(sppbp, op)
DSPPBP sppbp;
WORD op;
{ /* assumes system area is that of VME/10 floppy diskette */
	LONG	syssz, rxfrsc, retcode;
	WORD	bfflg;
	WORD	dsk = sppbp->spunit;
	BYTE	*bfp;

	if ( !(syssz = workmdb[dsk].syssize) )
		return(RWNERR(E_INVCMD));		/* no system area */
	if ( syssz != sppbp->sp.spbuf.spbufsize )
		return(RWNERR(E_BADPB));		/* wrong buffer size */
	bfflg = sppbp->spflags & USRADDR;
	bfp = sppbp->sp.spbuf.spbuffer;
	if ( bfflg && (retcode = mrange(bfp, syssz)) )
		return(retcode);			/* buffer range error*/
	/* handle TENbug sectors separately */
	if ( retcode = synxfer(dsk, 0L, dsk < 2 ? 4L : 2L, bfp, op,
				bfflg, sppbp->sppdaddr) )
		return(retcode);
	/* now transfer rest of the system area (i.e. booter) */
	rxfrsc = (syssz - 512) / workmdb[dsk].sectsize;
	return(synxfer(dsk, dsk < 2 ? 8L : 2L, rxfrsc, bfp+512, op,
			bfflg, sppbp->sppdaddr));
}


/************************************************************************/
/*									*/
/*	Asynchronous Routines						*/
/*									*/
/************************************************************************/

/************************************************************************/
/*	Disk Done							*/
/************************************************************************/

MLOCAL VOID ddoneasr()
{
	REG LONG	retcode;

	switch ( sns.ercode )	/* map rwin error to CDOS error */
	{
	case NOERR:	retcode = E_SUCCESS;			break;
	case NTRDY:	retcode = RWNERR(E_READY);		break;
	case NOTRK0:	retcode = RWNERR(E_UNITNO);		break;
	case SEEKERR:	retcode = RWNERR(E_SEEK);		break;
	case INVLLSN:
	case IDNTFND:
	case DMNTFND:	retcode = RWNERR(E_SEC_NOTFOUND);	break;
	case INVLCMD:	retcode = RWNERR(E_INVCMD);		break;
	case DATAERR:	retcode = RWNERR(E_READFAULT);		break;
	case WRTPROT:	retcode = RWNERR(E_WPROT);		break;
	case WRTFLT:	retcode = RWNERR(E_WRITEFAULT);		break;
	default:	retcode = RWNERR(E_GENERAL);		break;
	}
	flagset(dskflag, gepdaddr, retcode);
}

/************************************************************************/
/*	Data Transfer Routine - queued by data interrupt		*/
/************************************************************************/

MLOCAL VOID dxfrasr()
{
	REG BYTE *lbufp;

	if ( gbufflag )			/* if user memory buffer address */
	{
		mapu(gbpdaddr);		/*    then get access to it	*/
		lbufp = saddr(gbufp);
	}
	else lbufp = gbufp;		/*    else use system address	*/
	while ( ((dskcntl->intstt & (DRQ|BUSY|CI)) == (DRQ|BUSY)) && gxfrcnt )
	{	/* while controller has data and we want to transfer data */
		switch ( gdskop )	/* transfer data */
		{
		case DSKREAD:
			rddat(lbufp,&(dskcntl->data));
			break;
		case DSKWRITE:
			wrdat(lbufp,&(dskcntl->data));
			break;
		}
		gbufp += DCXFER;	/* update global and		*/
		lbufp += DCXFER;	/*    local data buffer pointers*/
		gxfrcnt--;		/*    and tansfer block count	*/
	}
	if ( gbufflag ) unmapu();	/* if we mapped user mem undo it*/
	if ( (!(dskcntl->intstt & CI)) && gxfrcnt )
		/* if no cmd interrupt and not done */
		dskcntl->intstt = DTINTON;	/* turn on data interrupt */
}


/************************************************************************/
/*	Disk Read/Write with error retry/correction			*/
/************************************************************************/

MLOCAL VOID drwasr()
{
	REG BYTE *bufp;
	LONG erofst;	/* offset from bufp of location to correct */

	if ( sns.ercode )			/* if error */
	{
		if ( sns.ercode != CORRDER )	/*  if error not correctable*/
		{
			if ( --grcnt )		/*   if dec retry count */
			{
				/* restore transfer variables */
				gbufp = gbbufp;
				gxfrcnt = gbxfrcnt;
				/* re-issue command */
				sndcmd(gunit, grecn, gpscnt, gdskop);
			}
			else ddoneasr();	/*   else report dos error */
			return;
		}
		else
		{	/* correct read data - winchester only */
			/* error offset in last 256 bytes transferred */
			erofst = (sns.snsbt6 << 8) + sns.snsbt7;
			if ( gbufflag )		/* if user buffer address */
			{
				mapu(gbpdaddr);	/*  get access to it */
				/* beginning address of last 256 byte xfer */
				bufp = (BYTE *)((LONG)saddr(gbufp) - 256);
			}
			else bufp = (BYTE *)((LONG)gbufp - 256);/*use sys adr*/
			bufp[erofst] ^= sns.snsbt8;	/* fix the error */
			bufp[erofst+1] ^= sns.snsbt9;
			if ( gbufflag ) unmapu(); /* if mapped addr unmap */
			if ( gxfrcnt )	/* if more to read - continue command */
			{
				sndcmd(gunit, grecn + gpscnt - sns.n, sns.n,
					gdskop);
				return;
			}

			/* Should probably check for consistent correctable
			error (snsbt6-9 same twice) then write corrected
			sector back to disk.  Refer to Winchester Disk
			Controller User's Manual section 4.3 Disk Error
			Recovery page 4-15. */
		}
	}
	if ( gnrecs -= gpscnt )	/* more to do */
	{
		gbbufp = gbufp;		/* set up globals for next command */
		grecn += gpscnt;
		gpscnt = gnrecs;
		if ( gpscnt > MXPSCNT ) gpscnt = MXPSCNT;
		gxfrcnt = gpscnt * workmdb[gunit].sectsize / DCXFER;
		gbxfrcnt = gxfrcnt;
		grcnt = 10;
		sndcmd(gunit, grecn, gpscnt, gdskop);
	}
	else flagset(dskflag, gepdaddr, E_SUCCESS);	/* done */
}


/************************************************************************/
/*									*/
/*	Disk Interrupt Service Routine					*/
/*									*/
/************************************************************************/

MLOCAL WORD dskint()
{
	if ( dskcntl->intstt & CI )		/* command interrupt */
	{
		dskcntl->intstt = DTINTOF;	/* turn off data interrupt */

		/* turn off command interrupt by reading */
		/* the sense block from the controller */
		/* only done on command complete interrupt */
		/* the dskcntl references must NOT be reordered */

		sns.ercode = dskcntl->cmdsns;
		sns.status = dskcntl->cmdsns;
		sns.lun = rcnvdsk[(sns.status >> 5) & 0x3];
		sns.pcylnm = dskcntl->cmdsns;
		sns.pcylnm = (sns.pcylnm << 8) + dskcntl->cmdsns;
		sns.headnm = dskcntl->cmdsns;
		sns.sectnm = sns.headnm & 0x1f;
		sns.headnm = sns.headnm >> 5;
		sns.n = dskcntl->cmdsns;
		sns.snsbt6 = dskcntl->cmdsns;
		sns.snsbt7 = dskcntl->cmdsns;
		sns.snsbt8 = dskcntl->cmdsns;
		sns.snsbt9 = dskcntl->cmdsns;

		/* queue asr to check for completion */
		doasr(gdskfunc,(LONG) 0,(LONG) 0,DSKASRPRI);
	}
	if ( dskcntl->intstt & DI )		/* data interrupt */
	{
		dskcntl->intstt = DTINTOF;	/* turn off data interrupt */
		doasr(dxfrasr,(LONG) 0,(LONG) 0,DSKASRPRI); /* queue xfr func */
	}
	return(FALSE);		/* don't force dispatch */
}

                                                   
queue xfr func */
	}
	return(FALSE);		/* don't force dispatch */
}

                                                   