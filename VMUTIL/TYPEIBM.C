/*	@(#)typeibm.c	2.1		*/
/*  typeibm (15 Oct 84) - type an IBM PC file on console */
	
#include "portab.h"
#include "ibmdisk.h"

typeibm(argc,argv)
WORD argc;
BYTE *argv[];
{
	BYTE buffer[MAXCLSIZE];		/* to store one cluster of file data */
	WORD actual;

	if (argc != 2)
	{
		printf("Usage: typeibm <ibm filename>\n");
		return;
	}
	if (openibm(argv[1]) == -1)
	{
		printf("Unable to find IBM PC file %s\n",argv[1]);
		return;
	}
	while ((actual = readibm(buffer)) > 0)
		write(1,buffer,actual);
}
