/*	@(#)eraibm.c	1.1		*/
/*	eraseibm - erase an ibm file	*/
	
#include "portab.h"
#include "ibmdisk.h"

ierase(argc,argv)
WORD argc;
BYTE *argv[];
{
	if (argc != 2 )
	{
		printf("Usage: eraseibm filename\n");
		return;
	}

	eraibm(argv[1]);
}
