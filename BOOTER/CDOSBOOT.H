/*	@(#)cdosboot.h	1.6		*/
/*
 *	Header file for Concurrent DOS-68K booter for the
 *	VME/10.
 */

/*
 *	This structure defines the object file information
 *	required to load the operating system object.
 */

typedef struct
{
	LONG	textsize;
	LONG	datasize;
	LONG	bsssize;
	BYTE	*loadaddress;
	BYTE	*startaddress;
} OBJINFO;

/*
 *	File system description.  Information is obtained from
 *	the Boot Parameter Block (BPB).
 */

typedef struct
{
	WORD	sectsize;	/* physical sector size in bytes	*/
	WORD	firstsec;	/* first phys sec num of FAT on tk 0	*/
	LONG	fatsect;	/* first phys scetor number of FAT	*/
	LONG	nsectors;	/* number of sectors in logical image	*/
	WORD	secblk;		/* number of sectors per cluster	*/
	WORD	blksz;		/* number of bytes per cluster		*/
	BYTE	nfats;		/* number of file allocation tables	*/
	WORD	nfrecs;		/* number of sectors in a single FAT	*/
	WORD	dirsize;	/* number of entries in root directory	*/
	LONG	dirsect;	/* physical sector number of directory  */
	LONG	datasect;	/* physical sec. num. of data start     */
	BYTE	mformat;	/* file system type			*/
} FILESYS;

/* values for fs.mformat - the number of bytes in a FAT entry */

#define ONEANDAHALF	1
#define TWO		2

/*
 *	Some Constants.  You may have to tune these depending
 *	on your FAT table size and cluster sizes.  These are
 *	pretty maximum sizes, though.  I would think you would
 *	more likely run out of memory before you overextend
 *	these values, depending on the booter location in memory.
 */

#define NAMESIZE	128	/* Maximum filename size (to allow pathnames */

/*
 *	Used in pcdosdk.c.
 */

#define MAXFATBUF	(36*256)
#define MAXSECTSIZE	1024
#define MAXCLUSTER	(16*1024) /* maximum cluster size in bytes expected */

/*
 *	Buffer structure for cluster reads, size and buffer.
 */

typedef struct
{
	WORD	numbytes;
	BYTE	buf[MAXCLUSTER];
	WORD	current;
} BS;

/*
 *	Default operating system file name to load.
 */

#define CDOSDEFAULT	"CDOS.SYS"

#define OURNAME		"Concurrent-68K Booter: "

/*
 *	Some hardware/filesystem implementation stuff.
 *
 *	You would probably have to change if your disk formats change.
 *	Not for partitioning.
 */

#define FLOPPYFIRSTSECTOR	16
#define HARDDISKFIRSTSECTOR	18

#define floppy(d)	((d) == 2 || (d) == 3)	/* is it a floppy disk? */

/*
 *	Some PC-DOS constants.
 */

#define DIRSIZE		32

/*
 *	Return flags.
 */

#define ERROR	(-1)
#define OK	(0)

/*
 *	Some function declarations.
 */

EXTERN UWORD	getword();
EXTERN VOID	PutStr();
EXTERN VOID	PutMsg();
EXTERN VOID	movb();
EXTERN VOID	Exit();
EXTERN LONG	getlong();
EXTERN LONG	swapl();
                                                                                                                 