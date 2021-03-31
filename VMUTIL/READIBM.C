/*	@(#)readibm.c	2.5		*/
/*  readibm (12 Oct 84) - read an IBM PC file to CP/M file */
	
#include "portab.h"
#include "ibmdisk.h"

iread(argc,argv)
WORD argc;
BYTE *argv[];
{
	WORD actual,fd;
	BYTE *cpmfile;
	BYTE buffer[MAXCLSIZE];/* used to store one cluster of file data */

	if (argc != 2 && argc != 3)
	{
		printf("Usage: readibm <ibm filename> [<cpm filename>]\n");
		return;
	}
	cpmfile = (argc == 2 ? argv[1] : argv[2]);
	if (openibm(argv[1]) == -1)
	{
		printf("Unable to find IBM PC file %s\n",argv[1]);
		return;
	}
	if (strcmp(argv[0],"Read") == 0)
	{
		if ((fd = creata(argv[1],0666)) == -1)
		{
			printf("Unable to create CP/M file %s\n",argv[1]);
			return;
		}
	}
	else
		if ((fd = creatb(cpmfile,0666)) == -1)
		{
			printf("Unable to create CP/M file %s\n",argv[1]);
			return;
		}
	while ((actual = readibm(buffer)) > 0)
	{
		if (write(fd,buffer,actual) < actual)
		{
			printf("Unable to write to CP/M file\n");
			break;
		}
	}
	close(fd);
}
