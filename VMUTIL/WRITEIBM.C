/*	@(#)writeibm.c	2.7		*/
/*  writeibm (13 Nov 84) - write a CP/M file to an IBM PC file */
	
#include "portab.h"
#include "ibmdisk.h"

EXTERN FS	fs;

MLOCAL BYTE buffer[MAXCLSIZE];/* used to store one cluster of file data */

iwrite(argc,argv)
WORD argc;
BYTE *argv[];
{
	REG WORD i,more,actual,fd,ascii;
	BYTE *ibmfile;

	if (argc != 2 && argc != 3)
	{
		printf("Usage: writeibm <cpm filename> [<ibm filename>]\n");
		return;
	}
	ibmfile = (argc == 2 ? argv[1] : argv[2]);

	ascii = (strcmp(argv[0],"Write") == 0 );
	if ((fd = openb(argv[1],0)) == -1)
	{
		printf("Unable to find CP/M file %s\n",argv[1]);
		return;
	}
	if (creatibm(ibmfile) == -1)
	{
		printf("Unable to create IBM file %s\n",ibmfile);
		close(fd);
		return;
	}
	more = TRUE;
	while (more && ((actual = read(fd,buffer,fs.bpc)) > 0))
	{
		if (ascii)
		{
			for (i = 0; i < actual; i++)
			{
				if (buffer[i] == 0x1a)
				{
					while (i < actual)
						buffer[i++] = 0x1a;
					more = FALSE;
					break;
				}
			}
		}
		if (writeibm(buffer,actual) == -1)
		{
			printf("Unable to write to IBM file\n");
			break;
		}
	}
	close(fd);
	closeibm();
}
