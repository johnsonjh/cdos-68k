/*	@(#)dkconf.c	2.9		*/

#include "portab.h"
#include "ibmdisk.h"

FS	fs;

LONG atol();

dkconf(argc,argv)
WORD	argc;
BYTE	*argv[];
{
	WORD	drvnum;
	WORD	size;
	LONG	maxsecs;
	LONG	nsecs;
	WORD	badtrcylinders;
	BYTE	buf[256];
	WORD	failed;
	LONG	minsecs;

	if( (argc != 2 && argc != 3 && argc != 4)
		|| (argc==4 ? strcmp(argv[3],"-") : FALSE) )
	{
		printf("Usage: dkconf drive# [disksize -]\n");
		return;
	}

	switch(drvnum=atoi(argv[1]))
	{
	case 0:		/* hard disk */
	case 1:
		fs.bps = HBPS;

		fs.bootstartsec = HBTSTARTSEC;
		fs.numbootsecs = BTSZBYTES / fs.bps;

		fs.numrtdirentries = HNUMDIRENTRIES;
		fs.rtdirszinsecs = HNUMDIRENTRIES * DIRENTRYSZBYTES / fs.bps;
		fs.nfats = NFATS;

		fs.hiddensectors = 0;
		/* must be under 1 track in sectors */
		fs.reservedsectors = TENBUGSECTORS + fs.numbootsecs;

		fs.mediatype = HMEDIA;

		fs.attr = HATTR;
		fs.spt = HSPT;

		failed = FALSE;
	picksize:
		if( argc < 3 || failed )
		{
			printf("\n");
			printf("  5 Megabyte\n");
			printf("  10 Megabyte\n");
			printf("  15 Megabyte\n");
			printf("  40 Megabyte Micropolis type\n");
			printf("         What size drive do you have? ");
			strcpy(buf,"0");
			gets(buf);
			size = atoi(buf);
		}
		else
			size = atoi(argv[2]);
			
		switch( size )
		{
		case 5:
			fs.clszinsecs = H5SPCLUSTER;
			fs.fatszinsecs = H5SPFAT;

			fs.heads = H5HEADS;	/* number of heads */
			fs.cylinders = H5CYL;	/* number of cylinder */
			fs.precomp = H5PRECOMP;
			badtrcylinders = H5BTCYLS;
			break;
		
		case 10:
			fs.clszinsecs = H10SPCLUSTER;
			fs.fatszinsecs = H10SPFAT;

			fs.heads = H10HEADS;	/* number of heads */
			fs.cylinders = H10CYL;	/* number of cylinder */
			fs.precomp = H10PRECOMP;
			badtrcylinders = H10BTCYLS;
			break;

		case 15:
			fs.clszinsecs = H15SPCLUSTER;
			fs.fatszinsecs = H15SPFAT;

			fs.heads = H15HEADS;	/* number of heads */
			fs.cylinders = H15CYL;	/* number of cylinder */
			fs.precomp = H15PRECOMP;
			badtrcylinders = H15BTCYLS;
			break;

		case 40:
			fs.clszinsecs = H40SPCLUSTER;
			fs.fatszinsecs = H40SPFAT;

			fs.heads = H40HEADS;	/* number of heads */
			fs.cylinders = H40CYL;	/* number of cylinder */
			fs.precomp = H40PRECOMP;
			badtrcylinders = H40BTCYLS;
			break;

		default:
			printf("%d MegaByte drive not supported.\n", size);
			printf("Choose one of the sizes listed\n");
			failed = TRUE;
			goto picksize;
		}

		/*
		 *	Avoiding some sign extension.... there are better ways.
		 */

		fs.nsects = fs.cylinders - badtrcylinders;
		fs.nsects = fs.nsects * fs.heads;
		fs.nsects = fs.nsects * HSPT;
 		maxsecs = fs.nsects;
		minsecs = (LONG)(fs.bootstartsec + fs.numbootsecs +
		(fs.fatszinsecs * fs.nfats) + fs.rtdirszinsecs + 100L);

	picksecs:
		if( argc != 4 )
		{
			printf("\nYou can decrease the size of the filesystem\n");
			printf("Currently %ld %d byte sectors\n", maxsecs, fs.bps);
			printf("This is your maximum size, minimum size is %ld\n",minsecs);
			nsecs = 0;
			printf("Newsize? (<CR> == maximum) ");
			strcpy(buf,"0");
			gets(buf);
			if( !(nsecs = atol(buf)) )
				nsecs = maxsecs;
		}
		else	/* '-' specified, go for the max */
			nsecs = maxsecs;

		if( nsecs > maxsecs || nsecs < minsecs )
		{
			printf("%ld sectors invalid, choose again\n", nsecs);
			goto picksecs;
		}
		else
			printf("File system size %ld in bytes\n", fs.nsects = nsecs);

		break;

	case 2:			/* floppy disk */
		fs.bps = FBPS;

		fs.bootstartsec = FBTSTARTSEC;
		fs.numbootsecs = BTSZBYTES / fs.bps;

		fs.clszinsecs = FSPCLUSTER;
		fs.fatszinsecs = FSPFAT;
		fs.rtdirszinsecs = FNUMDIRENTRIES * DIRENTRYSZBYTES / fs.bps;
		fs.nfats = NFATS;

		/* boot record info */
		fs.numrtdirentries = FNUMDIRENTRIES;
		fs.hiddensectors = FHIDDEN;
		fs.reservedsectors = 0;

		fs.nsects = FNSECTORS;

		fs.mediatype = FMEDIA;

		fs.attr = FATTR;
		fs.spt = FSPT;
		fs.heads = FHEADS;	/* number of heads */
		fs.cylinders = FCYL;	/* number of cylinder */
		fs.precomp = FPRECOMP;

		break;

	default:
		printf("Drive %d not supported.\n", drvnum);
		return;
	}

	fs.disknum = drvnum;
	fs.bpc = fs.clszinsecs * fs.bps;
	fs.fatstartsec = fs.bootstartsec + fs.numbootsecs;
	fs.rtdirstartsec = fs.fatstartsec + (fs.fatszinsecs * fs.nfats);
	fs.datastartsec = fs.rtdirstartsec + fs.rtdirszinsecs;

	/*
	 *	SndCnf expects max head number and max cylinder
	 *	number, both are numbered starting with 0,
	 *	so we subtract '1'.
	 */

	SndCnf(fs.disknum, fs.heads - 1, fs.cylinders - 1, fs.precomp);

	return;
}
