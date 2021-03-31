/*	@(#)diribm.c	2.1		*/
/*  diribm (15 Oct 84) - print directory of IBM diskette */	
	
#include "portab.h"

diribm(argc,argv)
WORD argc;
BYTE *argv[];
{
	BYTE direntry[32];		/* used to store one directory entry */
	WORD column;

	if (argc != 1)
	{
		printf("No parameters allowed\n");
		return;
	}
	if (opendir() == -1)
	{
		printf("Unable to read IBM directory\n");
		return;
	}
	column = 0;
	while ((readdir(direntry)) >= 0)
	{
		write(1,direntry,8);
		write(1," ",1);
		write(1,&direntry[8],3);
		if (column == 4)
		{
			write(1,"\n",1);
			column = 0;
		}
		else
		{
			write(1,"   ",3);
			column++;
		}
	}
}
