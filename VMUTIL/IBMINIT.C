/*	@(#)ibminit.c	2.6		*/
/*  vmformat (09 AUG 84) - PC-DOS (CDOS) initialization routines */

#include "portab.h"
#include "ibmdisk.h"

EXTERN FS	fs;

/* initpcdos() - fats, and directory */

initpcdos(argc,argv)
WORD	argc;
BYTE	**argv;
{
	/* We will need a pretty big buffer */
	BYTE	sector[MAXRTDIRENTRIES*DIRENTRYSZBYTES];
	WORD	nfats;
	BYTE	buf;

	/* write the FATS and directory */

	printf("All File System data on the diskette will now be destroyed.\n");
	printf("Is this what you want (y/n)? ");
	read(0,&buf,1);
	printf("\n");
	if (buf != 'y' && buf != 'Y')
	{
		printf("Init aborted\n");
		return;
	}
	printf("Writing %d FATs of %d sector(s) each...\n",fs.nfats,fs.fatszinsecs);
	for (nfats = 0; nfats < fs.nfats; nfats++)
	{
		setb(0,sector,sizeof(sector));
		sector[0] = sector[1] = sector[2] = fs.mediatype;
		if( WriteSector(fs.disknum,
			(LONG)(fs.fatstartsec+nfats*fs.fatszinsecs),
			fs.fatszinsecs,sector) == ERROR)
		{
			printf("COULD NOT WRITE FATS\n");
			return;
		}
	}
	setb(0,sector,sizeof(sector));
	printf("Writing directory of %d sectors...\n",fs.rtdirszinsecs);
	if( WriteSector(fs.disknum,fs.rtdirstartsec,fs.rtdirszinsecs,sector) == ERROR )
	{
		printf("Could not write the directory.\n");
		return;
	}
}


#if	FORMATCMD

ibmformat()
{
	BYTE buf;
	BYTE status;

	printf("Floppy format for CONCURRENT DOS 68k v2.0\n");
	printf("Insert a new diskette in the disk drive.\n");
	printf("All data on the diskette will now be destroyed.\n");
	printf("Is this what you want (y/n)? ");
	read(0,&buf,1);
	printf("\n");
	if (buf != 'y' && buf != 'Y')
	{
		printf("Format aborted\n");
		return;
	}

/*	checkstatus(config());	*/
/*	checkstatus(format());	*/
	printf("All done!\n");
}

#endif
