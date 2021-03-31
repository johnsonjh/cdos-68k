/*	@(#)putboot.c	2.12.1		*/

#include "portab.h"
#include <stdio.h>
#include "ibmdisk.h"
#include "bootrec.h"
#include "bootfix.h"
#include "motbug.h"

/*
 *	This program creates the boot image in a Concurrent DOS
 *	file system.  The object file should be relocated (absolute).
 *	The file header will be stripped off and the initial PC and SP
 *	required by TENbug will be inserted IN the object's boot record.
 *	The bss section will be added and zeroed.
 *
 *	The resultant TENbug image is copied to the Concurrent
 *	disk set up in the structure "fs".
 */

EXTERN FS	fs;	/* file system and disk info */
EXTERN WORD	debugflag;

/*
 *	two magic numbers exist for object files
 *
 *	we don't muck with non-contiguous segments.
 */

#define MAGIC1	0x601a		/* contiguous segments */
#define MAGIC2	0x601b		/* non-contiguous segments */

/*
 *	define the header structure where we get the object file
 *	layout.
 */

#define OH	struct obj_header

OH
{
	int	magic;
	long	textsize;	/* all sizes in BYTES! */
	long	datasize;
	long	bsssize;
	long	symsize;
	long	reserved;	/* not used */
	long	textstart;
	int	relocinfo;	/* if 0 - relocatable; */
};				/* if 1 - no relocation bits exist; */

/*
 *	Size of stack we want for booter.  A good guess.
 *	Actually reset by current booter, but required to be
 *	sane by TENbug.
 */

#define RESSTACK (6*1024)

FILE	*fopenb();

putboot(argc,argv)
WORD	argc;
BYTE	**argv;
{
	REG FILE	*input;
	OH		hd;
	REG WORD	*pw;
	REG LONG	used;
	REG LONG	size;
	REG BTREC	*bp;
	BTFIX	*bfp;
	VOLID	*vp;
	DCA	*dp;
	BYTE	booter[5000];	/* a little magic, need to hold 4K booter + */
	BYTE	*name;

	used = 0L;

	if( argc != 2 )
	{
		printf("Usage: putboot object\n");
		return;
	}
	else
		name = argv[1];

	if( ! strcmp(name,"") )
		goto skipgetobject;
	else if( (input=fopenb(name,"r")) == NULL )
	{
		printf("putboot: can't open input file -> %s\n", name);
		return;
	}

	fread(&hd,sizeof(hd),1,input);

	if( hd.magic != MAGIC1 )
	{
		printf("putboot: bad magic number in object file\n");
		return;
	}

	if( hd.relocinfo == 0 )
	{
		printf("putboot: file is not absolute, it is relocatable\n");
		return;
	}

	if( debugflag )
	{
		printf("header info: magic = %x\n", hd.magic);
		printf("             tsize = %lx\n", hd.textsize);
		printf("             dsize = %lx\n", hd.datasize);
		printf("             bsize = %lx\n", hd.bsssize);
		printf("           symsize = %lx\n", hd.symsize);
		printf("          reserved = %lx\n", hd.reserved);	/* not used */
		printf("	     start = %lx\n", hd.textstart);
		printf("	     reloc = %x\n\n", hd.relocinfo);
	}
	else
		printf("putboot: Reading object header.\n");

	/*
	 *	now copy out the text and data.
	 */

	size = hd.textsize + hd.datasize;
	pw = (WORD *)booter;

	while( !feof(input) && size > 0  && used < BTSZBYTES )
	{
		fread(pw++,sizeof(WORD),1,input);
		used += sizeof(WORD);
		size -= sizeof(WORD);
	}

	if( size > 0 )
	{
		printf("Something's went wrong putting the booter\n");
		printf("text and data not copied = %ld\n", size);
		printf("boot buffer used = %d\n", used);
		printf("feof(input) = %d\n", feof(input));
		return;
	}

	/*
	 *	Zero unused portion of boot buffer.
	 */

	if( (LONG)pw & 1 )		/* word align just in case */
		pw = (WORD *)((LONG)pw + 1);

	while( used < BTSZBYTES )
	{
		*pw++ = 0;
		used += sizeof(WORD);
	}

	/*
	 *	Initialize boot record.
	 */
	
skipgetobject:
	if( !strcmp(name,"") )	/* no object specified, read existing data */
	{
		if( ReadSector(fs.disknum,fs.bootstartsec,fs.numbootsecs,booter) == ERROR )
		{
			printf("putboot: Couldn't write booter out with WriteSector\n");
			return;
		}
	}

	bp = (BTREC *)booter;

	putword(bp->sctrsz,fs.bps);
	bp->clstrsz = fs.clszinsecs;
	putword(bp->rsvsc,fs.reservedsectors);
	bp->numfats = fs.nfats;
	putword(bp->rtdir,fs.numrtdirentries);
	if( fs.nsects <= 65535L )
		putword(bp->dksz,(UWORD)fs.nsects);
	else
	{
		putword(bp->dksz,0);
		putlong(bp->xdksz,fs.nsects);
	}
	bp->meddsc = fs.mediatype;
	putword(bp->fatsz,fs.fatszinsecs);
	putword(bp->spt,(WORD)fs.spt);
	putword(bp->hds,(WORD)fs.heads);
	putword(bp->hidsc,(WORD)fs.hiddensectors);

	/*
	 *	Now we must apply a final patch to the boot record
	 *	to store an initial PC value for TENbug (and other
	 *	Motorola bugs) to load the booter.
	 *
	 *	But we are going to do some not very nice things...
	 *
	 *	We will still have to initialize the initial PC value,
	 *	this is where TENbug will transfer execution to.
	 *	But we will play a game with the SP (first long word)
	 *	we will not modify it and still let it be the short
	 *	(byte) branch beyond the boot record (see boot loader
	 *	assembly language start up).  Therefore it is meaningless
	 *	as a SP value, the booter MUST initialize the stack pointer
	 *	before using the stack (mine does, does yours?)
	 */
	
	bfp = (BTFIX *)booter;

	/*
	 *	If we are installing a new booter, then
	 *	we need to store the initial PC and SP
	 *	required by TENbug.
	 *
	 *	We initialize the PC to just beyond the Boot Record.
	 */

	if( strcmp(name,"") )
	{
		bfp->bf_pc = hd.textstart + sizeof(BTREC);
		bfp->bf_sp = hd.textstart + hd.textsize
				+ hd.datasize + hd.bsssize + RESSTACK;
		if( bfp->bf_sp & 1 )			/* word align in case */
			bfp->bf_sp += 1;

	}

	/*
	 *	Pad unused portions of field with unique string...
	 */

	strncpy(bfp->bf_68k,BPSTRING,(WORD)sizeof(bfp->bf_68k));

	/*
	 *	some more stuff to be filled in: code+data load and size,
	 *	first data sector.
	 *
	 *	if i only knew what they meant...
	 */

	if( WriteSector(fs.disknum,fs.bootstartsec,fs.numbootsecs,booter) == ERROR )
	{
		printf("putboot: Couldn't write booter out with WriteSector\n");
		return;
	}

	/*
	 *	Now, just for grins, let's make sure the
	 *	boot block for TENbug is pointing at the
	 *	the booter we just put, and that the load address
	 *	is the booter base address (textstart).
	 */

	vp = (VOLID *)booter;		/* use as temp buffer */
	dp = (DCA *)&booter[256];

	if( ReadSector(fs.disknum,0L,2*fs.bps/256,vp) == ERROR )
	{
		printf("putboot: Could not read TENbug boot block\n");
		return;
	}

	vp->vsector = fs.bootstartsec;

	/*
	 * The following line assumes bps is multiple of 256
	 * We need to convert "real sectors" and sizes to "VERSAdos"
	 * sectors and sizes, TENbug is smart enough to take the
	 * disk configuration info and boot from VERSAdos sector
	 * numbering.  I wish it wasn't so smart.
	 */

	vp->vsects = fs.numbootsecs * (fs.bps / 256);
	vp->dest = hd.textstart;

	/*
	 * Set up the pointer to the disk configuration area (sector 1)
	 */

	vp->dcasect = 1;		/* DCA is a sector 1 */
	vp->dcalen = 1;			/* and occupies 1 sector */

	movb("CDOS",vp->name,4);
	movb("MOTOROLA",vp->exormacs,8);
	movb("Concurrent DOS-68K  ",vp->description,
		(WORD)sizeof(vp->description));

	dp->dca_attr = fs.attr;
	dp->dca_spt = fs.spt;
	dp->dca_heads = fs.heads;
	dp->dca_cylinders = fs.cylinders;
	dp->dca_bps = fs.bps;
	dp->dca_precomp = fs.precomp;

	if( WriteSector(fs.disknum,0L,2*fs.bps/256,vp) == ERROR )
	{
		printf("Couldn't initialize boot and configuration info\n");
		return;
	}
}
