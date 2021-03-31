
/*	@(#)vmdisk.h	1.21.1		*/
/*************************************************************************
*  REV		WHEN	  WHO	WHAT
*  ===		====	  ===	====
*  1.21.1	9/5/85	  MA	Inserted missing zero fill words in
*				special format track parameter block.
*************************************************************************/

/*
 *	Concurrent 4.0 disk driver header file
 *
 *	This file complies with External Specification
 *	Revision 7.00  dated Dec 7, 1984
 */

/*	Media Descriptor Block */
typedef struct
{
	WORD	sectsize;	/* physical sector size in bytes	*/
	WORD	firstsec;	/* first phys sec num of FAT on tk 0	*/
	LONG	nsectors;	/* number of sectors in logical image	*/
	WORD	sectrk;		/* number of sectors per track		*/
	WORD	secblk;		/* number of sectors per block		*/
	BYTE	nfats;		/* number of file allocation tables	*/
	BYTE	fatid;		/* FAT identification byte		*/
	WORD	nfrecs;		/* number of sectors in a single FAT	*/
	WORD	dirsize;	/* number of entries in root directory	*/
	BYTE	nheads;		/* number of heads			*/
	BYTE	mformat;	/* file system type			*/
	LONG	hidden;		/* number of sectors before partition	*/
	LONG	syssize;	/* number of bytes in system area	*/
} MDB;

/*	format (file system) types */
#define FORMRAW	0		/* raw (including CP/M)			*/
#define	FORM1BF	1		/* 1-1/2 byte FAT entries		*/
#define FORM2BF	2		/* 2 byte FAT entries			*/

/*	Select Parameter Block */
typedef struct
{
	BYTE	sunit;		/* driver unit number			*/
	BYTE	sreserved;	/* reserved				*/
	MDB	*smdbp;		/* pointer to area for MDB		*/
} DSELPB, *DSELPBP;

/*	Flush Parameter Block */
typedef struct
{
	BYTE	funit;		/* driver unit number			*/
	BYTE	freserved;	/* reserved				*/
	WORD	fflags;		/* flags				*/
} DFLUPB, *DFLUPBP;

/*	Read/Write Parameter Block */
typedef struct
{
	BYTE	rwunit;		/* driver unit number			*/
	BYTE	rwreserved;	/* reserved				*/
	WORD	rwflags;	/* flags				*/
	WORD	(*rwswi)();	/* software interrupt routine		*/
	BYTE	*rwpdaddr;	/* initiating process descriptor addr	*/
	BYTE	*rwbuffer;	/* data buffer address			*/
	LONG	rwnrecs;	/* number of physical records		*/
	union			/* either record number or hd/sec/cyl	*/
	{
		LONG	rwrecord;		/* logical sector number*/
		struct
		{
			BYTE	rwhead;		/* physical head number	*/
			BYTE	rwsector;	/* physical sector num	*/
			WORD	rwcylinder;	/* physical cylinder num*/
		} hsc;
	} rw;
} DRWPB, *DRWPBP;

/*	Read/Write Flags */
#define	DRWTHRU	1		/* read/write thru - direct media xfer	*/
#define	DRWHSC	2		/* head/sector/cylinder address		*/
#define DRWVERF	4		/* verify media - do not read to buffer	*/

/*	Special Parameter Block - Track Format */
typedef struct
{
	BYTE	spfhead;	/* head of track to format		*/
	BYTE	spfreserved;	/* reserved				*/
	WORD	spfcylinder;	/* cylinder of track to format		*/
	BYTE	spfdens;	/* format density: 0=single, 1=double	*/
	BYTE	spffill;	/* fill character			*/
	WORD	spfbps;		/* bytes per sector			*/
	WORD	spfspt;		/* sectors per track			*/
	WORD	spfsector;	/* sector number to begin format	*/
	struct			/* used if CHSN flag set in tk format	*/
	{			/*   repeated spfspt times		*/
		BYTE	spfc;	/*	cylinder number put on disk	*/
		BYTE	spfh;	/*	head number put on disk		*/
		BYTE	spfs;	/*	sector number put on disk	*/
		BYTE	spfn;	/*	number bytes in this sector	*/
	} spfchsn;
} DSPF, *DSPFP;

/*	Special Parameter Block */
typedef struct
{
	BYTE	spunit;		/* driver unit number			*/
	BYTE	spfunc;		/* function number			*/
	WORD	spflags;	/* flags				*/
	BYTE	*spsbaddr;	/* special blocking buffer system addr	*/
	BYTE	*sppdaddr;	/* initiating process descriptor addr	*/
	union			/* buffer addr/size or param addr/size	*/
	{
		struct
		{
			BYTE	*spbuffer;	/* buffer address	*/
			LONG	spbufsize;	/* buffer size in bytes	*/
		} spbuf;
		struct
		{
			LONG	spzero1;
			LONG	spzero2;
			DSPFP	spfp;		/* format param blk addr*/
			LONG	spfsz;		/* format param blk size*/
		} spfrtrk;
		struct
		{
			MDB	*spmdbp;	/* MDB pointer		*/
			LONG	spmdbsz;	/* MDB size in bytes	*/
		} spifrmt;
	} sp;
} DSPPB, *DSPPBP;

/*	Special Functions */
#define	DSPRDSYS	0	/* read system area			*/
#define	DSPWRSYS	1	/* write system area			*/
#define	DSPFRSYS	2	/* format system area			*/
#define	DSPFRTRK	3	/* format track				*/
#define	DSPIFRMT	8	/* initialize format (MDB)		*/

/*	Special Function Format Track Flags */
#define DSPFTCHSN	1	/* use CHSN table to format track	*/

/*	Get Parameter Block */
typedef struct
{
	BYTE	gunit;		/* driver unit number			*/
	BYTE	greserved;	/* reserved				*/
	WORD	gflags;		/* flags				*/
	WORD	gdtype;		/* disk drive type			*/
	WORD	gmaxrs;		/* maximum physical record size in bytes*/
	BYTE	*gopdraddr;	/* address of open door byte		*/
	WORD	gmaxfatrecs;	/* maximum number of records in a FAT	*/
	WORD	gmaxfatsiz;	/* maximum size of a FAT in bytes	*/
	WORD	gmaxdirsiz;	/* maximum number of root dir entries	*/
} DGETPB, *DGETPBP;

/*	Get Disk Types */
#define	DGTRMED	1		/* removable media			*/
#define	DGTODS	2		/* open door support			*/
#define	DGTMEMD	4		/* memory drive				*/

/*	Device Type */
#define DVR_RWIN	0x21	/* RWIN disk driver			*/

/*	Partition Table Entry - entries are byte swapped */
typedef struct
{
	BYTE	bootind;	/* active boot partition indicator	*/
	BYTE	shead;		/* start head				*/
	BYTE	ssector;	/* starting sector (lower 6 bits)	*/
	BYTE	scylinder;	/* start cylinder less 2 msbits		*/
	BYTE	sysird;		/* system type: 1=DOS, 0=other		*/
	BYTE	ehead;		/* ending head				*/
	BYTE	esector;	/* ending sector (lower 6 bits)		*/
	BYTE	ecylinder;	/* end cylinder less 2 msbits		*/
	BYTE	relsecl[4];	/* relative start sector		*/
	BYTE	nsecsl[4];	/* number of sectors			*/
} PTE;

/*	Master Boot and Partition Record */
typedef struct
{
	BYTE	bootcode[446];	/* offset to PTE in master boot record	*/
	PTE	pte[4];		/* 4 partition table entries		*/
	WORD	signature;	/* == $55AA				*/
} MBPR;

/*	Boot Record Format - entries are byte swapped */
typedef struct
{				/* NOTE: STRUCTURE NOT WORD ALLIGNED!!	*/
	BYTE	jmp[3];		/* first three bytes are jump (branch)	*/
	BYTE	name[8];	/* eight byte OEM name and version	*/
	BYTE	sctrsz[2];	/* bytes per sector			*/
	BYTE	clstrsz;	/* sectors per cluster (power of 2)	*/
	BYTE	rsvsc[2];	/* reserved sectors			*/
	BYTE	numfats;	/* number of FATS			*/
	BYTE	rtdir[2];	/* number of root dir entries		*/
	BYTE	dksz[2];	/* number of sectors in logical disk	*/
	BYTE	meddsc;		/* media descriptor ID			*/
	BYTE	fatsz[2];	/* number of sectors in a FAT		*/
	BYTE	spt[2];		/* sectors per track			*/
	BYTE	hds[2];		/* number of heads			*/
	BYTE	hidsc[2];	/* hidden sectors			*/
	BYTE	xdksz[4];	/* extended disk size if dksz 0 	*/
	BYTE	fdats[2];	/* first sector num after root dir	*/
	BYTE	cdld[4];	/* code load base			*/
	BYTE	cdln[4];	/* code length				*/
	BYTE	dtld[4];	/* data load base			*/
	BYTE	dtln[4];	/* data length				*/
} BTREC;

/*	Disk Driver Error Return Values */

/*	All error retruns are of the form:				*/
/*									*/
/* bit: 31|30	   24|23	16|15				0|	*/
/*	+-+----------+------------+------------------------------+	*/
/*	|1| RESERVED |   SOURCE   |	     ERROR CODE		 |	*/
/*	+-+----------+------------+------------------------------+	*/
/* where: RESERVED = 0, SOURCE = driver type, and			*/
/*	  ERROR CODE is one of the following:				*/

/************************************************************************/
/*									*/
/*	Driver Error Definitions - Range 0x0 - 0x3FFF			*/
/*									*/
/*	Note the order and definition of the following 16 error codes	*/
/*	must not change.  These error codes are to be returned to the	*/
/*	appropriate Resource Manager.					*/
/************************************************************************/

#define E_WPROT		0x00L	/* write protect violation		*/
#define E_UNITNO	0x01L	/* illegal unit number			*/
#define E_READY		0x02L	/* drive not ready			*/
#define E_INVCMD	0x03L	/* invalid command issued		*/
#define E_CRC		0x04L	/* crc error on I/O			*/
#define E_BADPB		0x05L	/* bad parameter block			*/
#define E_SEEK		0x06L	/* seek operation failed		*/
#define E_UNKNOWNMEDIA	0x07L	/* unknown media present		*/
#define E_SEC_NOTFOUND	0x08L	/* required sector not found		*/
#define E_DKATTACH	0x09L	/* attachment did not respond		*/
#define E_WRITEFAULT	0x0AL	/* write fault				*/
#define E_READFAULT	0x0BL	/* read fault				*/
#define E_GENERAL	0x0CL	/* general failure			*/
#define E_RES1		0x0DL	/* reserved				*/
#define E_RES2		0x0EL	/* reserved				*/
#define E_RES3		0x0FL	/* reserved				*/


#define E_RES1		0x0DL	/* r