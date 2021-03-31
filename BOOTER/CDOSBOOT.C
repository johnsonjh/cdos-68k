/*	@(#)cdosboot.c	1.9		*/
/*
 *	This is the boot loader for Concurrent DOS-68K
 *	for the VME/10.
 *
 *	DESIGN NOTES:
 *
 *		- We must provide path searching in the directories
 *		- Support 1.5 and 2 byte FAT entries
 *		- Support both CP/M and COFF object file formats
 *		- Hard disks and floppy diskettes
 *
 *	not included:
 *
 *		- relocation
 *
 *	USAGE:
 *
 *		Interface to TENbug (or prebooter which emulates
 *		TENbug environment).  Default invocation through
 *		the bug (for disk 0):
 *
 *			bo
 *
 *		where we default the system name to CDOS.SYS.
 *
 *		FIRST SHOT: only simple name for root directory search.
 *
 *		Extensions to support file searching:
 *
 *			bo ,,pathname
 *
 *		pathname is a file pathname such as:
 *
 *			\sys\newsys\cdos.obj
 *
 *		'/' as separator is also acceptable.
 */

#include "portab.h"
#include "cdosboot.h"
#include "bootrec.h"

BS	bs;			/* buffer to handle first and last cluster */
FILESYS	fs;			/* record to store file system info */

BYTE	*strtbtstr;
BYTE	*endbtstr;

EXTERN WORD	disknum;

/*
 *	Construct a valid C string (null terminated)
 *	from TENbug string start and end.
 */

MLOCAL WORD MakeValidFilename(bp,size)
BYTE	*bp;
WORD	size;
{
	if( (WORD)(endbtstr-strtbtstr) > size )
	{
		PutMsg("Filename too long");
		return ERROR;
	}

	movb(strtbtstr,bp,(WORD)(endbtstr-strtbtstr));
	bp[endbtstr-strtbtstr] = '\0';			/* null terminate */

	if( bp[0] == '\0' )
		strcpy(bp,CDOSDEFAULT);

	return OK;
}


/*
 *	LoadFile
 *
 *	Load the object file into memory from disk.
 *
 *
 *	NOTE: LoadFile does not currently handle non-contiguous segments.
 */

MLOCAL WORD LoadFile(objinfo,bp)
REG OBJINFO	*objinfo;
REG BS	*bp;
{
	REG BYTE	*curptr;
	REG LONG	size;
	REG WORD	tmp;

	curptr = objinfo->loadaddress;
	size = objinfo->textsize + objinfo->datasize;

	/*
	 *	Load the partial data block remaining from the
	 *	object file header read to the correct
	 *	starting address.
	 */
	
	if( size < (LONG)(tmp = bp->numbytes - bp->current) )
		tmp = (WORD)size;
	movb(&bp->buf[bp->current],curptr,tmp);
	size -= tmp;
	curptr += tmp;
	
	/*
	 *	Now load the remaining full clusters
	 *	while there is remaining full text+data
	 *	clusters.
	 */

	while( size >= (LONG)fs.blksz )
	{
		if( ReadPcdos(curptr) != fs.blksz )
		{
			PutMsg("Incomplete cluster read, can't load o.s.");
			return ERROR;
		}
		curptr += fs.blksz;
		size -= fs.blksz;
	}

	/*
	 *	Load the last partial cluster.
	 */

	if( size )
	{
		if( ReadPcdos(bp->buf) < size )
		{
			PutMsg("Incomplete last block in object file");
			return ERROR;
		}
		movb(bp->buf,curptr,(WORD)size);
		curptr += size;
	}

	/*
	 *	Zero the BSS section of the operating
	 *	system in memory.
	 */
	
	size = objinfo->bsssize;

	while( size-- )
		*curptr++ = 0;
	
	return OK;
}

/*
 *	Put out a diagnostic message, prepend our name,
 *	and terminate with a carriage return line feed.
 */

VOID	PutMsg(s)
BYTE	*s;
{
	PutStr(OURNAME);
	PutStr(s);
	PutStr("\r\n");
}

/*
 *	PutStr
 *
 *	print string out pointed at by s by calling putchar().
 */

VOID PutStr(s)
REG BYTE	*s;
{
	while( *s )
		putchar(*s++);
}

/*
 *	Initialize the internal file system information from the
 *	swapped, misaligned boot block.
 */

extern	BYTE	bootrec;

VOID GetFileSystemInfo()
{
	REG BTREC	*bpb;
	REG FILESYS	*fsp;

	bpb = (BTREC *)&bootrec;		/* get booter base address */
	fsp = &fs;				/* get local REG ptr */

	fsp->sectsize = getword(bpb->sctrsz);
	if( floppy(disknum) )
		fsp->firstsec = FLOPPYFIRSTSECTOR;
	else
		fsp->firstsec = HARDDISKFIRSTSECTOR;
	if( (fsp->nsectors = (LONG)getword(bpb->dksz)) == 0L )
		fsp->nsectors = getlong(bpb->xdksz);
	fsp->secblk = bpb->clstrsz;
	fsp->blksz = fsp->secblk * fsp->sectsize;	/* bytes per cluster */
	fsp->nfats = bpb->numfats;
	fsp->nfrecs = getword(bpb->fatsz);
	fsp->dirsize = getword(bpb->rtdir) * DIRSIZE;
	fsp->dirsect = fsp->firstsec + fsp->nfrecs * fsp->nfats;
	fsp->datasect = fsp->dirsect + fsp->dirsize / fsp->sectsize;

	/*
	 *	HARD-CODED 12 bit FAT entries below.
	 *	When partitions implemented, need to get from
	 *	Partition Table Entry for booted partition.
	 */

	fsp->mformat = ONEANDAHALF;
}

/*
 *	The main entry to the C routines.
 */

VOID Boot()
{
	OBJINFO	objinfo;
	BYTE	filename[NAMESIZE];

	InitDisk();
	GetFileSystemInfo();
	if( MakeValidFilename(filename,NAMESIZE) == ERROR )
		return;
	if( OpenPcdos(filename) == ERROR )
		return;
	if( GetObjectFileInfo(&objinfo,&bs) == ERROR )
		return;
	if( LoadFile(&objinfo,&bs) == ERROR )
		return;
	SetAndGo(objinfo.loadaddress);		/* we jump to o.s. */
	/* no return from SetAndGo */
}
objinfo,&bs) == ERROR )
		return;
	SetAndGo(ob