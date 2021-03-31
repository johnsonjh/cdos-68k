/*	@(#)ibmmain.c	2.17		*/

#include "portab.h"
#include <stdio.h>

static BYTE buf[128];

static BYTE buf0[64], buf1[64], buf2[64], buf3[64], buf4[64], buf5[64];

static WORD	ac;
static BYTE	*av[] =
{
	buf0,
	buf1,
	buf2,
	buf3,
	buf4,
	buf5
};

WORD debugflag = 0;
WORD configured = FALSE;

#define FORMATCMD	0

main()
{
	printf("VMUTIL Version 2.0 (04 Nov 85)\n");

	InitDisk();

	help();

	while(1)
	{
		printf("\nEnter command: ");
		strcpy(av[0],"");
		strcpy(av[1],"");
		strcpy(av[2],"");
		strcpy(av[3],"");
		strcpy(av[4],"");
		strcpy(av[5],"");
		ac = myparse(av);
		if( debugflag )
		{
			printf("ac = %d\n", ac);
			printf("av[0] = %s\n", av[0]);
			printf("av[1] = %s\n", av[1]);
			printf("av[2] = %s\n", av[2]);
			printf("av[3] = %s\n", av[3]);
		}

		if( configured == FALSE &&
			(av[0][0] != 's' && av[0][0] != '+' &&
			 av[0][0] != 'x' && av[0][0] != 'u' &&
			 av[0][0] != 'h' && av[0][0] != '-' &&
			 av[0][0] != 'q' ) )
		{
			printf("You must select a drive first\n");
			continue;
		}

		switch( av[0][0] )
		{
		case '+':
			debugflag = 1;
			break;
		case '-':
			debugflag = 0;
			break;
		case 'h':
			help();
			break;
		case 'u':
			usage();
			break;
		case 'x':
		case 'q':
			exit();
		case 's':
			if( ac == 1 )
			{
				printf("    0 = internal hard disk\n");
				printf("    1 = external hard disk\n");
				printf("    2 = internal floppy disk\n");
				printf("    3 = internal floppy disk\n");
				printf("                 Select which drive? ");
				gets(av[1]);
				ac++;
			}
			dkconf(ac,av);
			configured = TRUE;
			break;
#if	FORMATCMD
		case 'f':
			ibmformat();
			break;
#endif
		case 'R':
			strcpy(av[0],"Read");
			goto read;
		case 'r':
			strcpy(av[0],"read");
		read:
			if( ac == 1 )
			{
				printf("CDOS file name to read from: ");
				gets(av[1]); ac++;
				printf("cpm file name to write to: ");
				gets(av[2]); ac++;
			}
			else if( ac == 2 )		/* automatically fill in the second arg */
			{
				if( av[1][1] == ':' )	/* strip off user # or drive spec */
					strcpy(av[2],av[1]+2);
				else if( av[1][2] == ':' )
					strcpy(av[2],av[1]+3);
				else
					strcpy(av[2],av[1]);
				ac += 1;
			}

			iread(ac,av);
			break;
		case 'W':
			strcpy(av[0],"Write");
			goto write;
		case 'w':
			strcpy(av[0],"write");
		write:
			if( ac == 1 )
			{
				printf("cpm file name to read from: ");
				gets(av[1]); ac++;
				printf("CDOS file name to write to: ");
				gets(av[2]); ac++;
			}
			else if( ac == 2 )		/* automatically fill in the second arg */
			{
				if( av[1][1] == ':' )	/* strip off user # or drive spec */
					strcpy(av[2],av[1]+2);
				else if( av[1][2] == ':' )
					strcpy(av[2],av[1]+3);
				else
					strcpy(av[2],av[1]);
				ac += 1;
			}
			iwrite(ac,av);
			break;
		case 'i':
			if( ac == 1 )
			{
					printf("Booter object to write: ");
					strcpy(av[1],"");
					ac++;
					gets(av[1]);
			}
			initpcdos(ac,av);
			putboot(ac,av);
			break;
		case 'p':
			if( ac == 1 )
			{
					printf("Booter object to write: ");
					strcpy(av[1],"");
					ac++;
					gets(av[1]);
			}
			putboot(ac,av);
			break;
		case 'd':
		case 'D':
			printf("\n");
			diribm(ac,av);
			break;
		case 'e':
			if( ac == 1 )
			{
				printf("Enter CDOS filename to ERASE: ");
				ac++;
				gets(av[1]);
			}
			ierase(ac,av);
			break;
		case 't':
			if( ac == 1 )
			{
				printf("Enter CDOS filename to type: ");
				ac++;
				gets(av[1]);
			}
			typeibm(ac,av);
			break;
		case '\0':			/* simply a carriage return */
			break;
		default:
			printf("Invalid command\n");
			break;
		}
	}
}

help()
{
	printf("Available commands:\n");
	printf("  d - print directory of CDOS disk\n");
	printf("  e - erase a CDOS file\n");
#if	FORMATCMD
	printf("  f - format a CDOS disk\n");
#endif
	printf("  h - print this help summary\n");
	printf("  i - initialize a CDOS file system\n");
	printf("  p - install a booter ONLY in the CDOS file system\n");
	printf("  r - read from CDOS file to CP/M file\n");
	printf("  w - write CP/M file to CDOS file\n");
	printf("  R - read from CDOS file to ASCII CP/M file\n");
	printf("  W - write ASCII CP/M file to CDOS file\n");
	printf("  s - select a disk drive\n");
	printf("  t - type a CDOS file\n");
	printf("  q - exit to CP/M\n");
	printf("  u - command forms and usage\n");
	printf("  x - exit to CP/M\n");
}

usage()
{
	printf("\nCommand usage:\n");
	printf("  dir                        help\n");
	printf("  usage                      initialize bootfile\n");
	printf("  putboot bootfile           select disknum [sizeMb -]\n");
	printf("  read CDOSfile [CPMfile]    write CPMfile [CDOSfile]\n");
	printf("  Read CDOSfile [CPMfile]    Write CPMfile [CDOSfile]\n");
	printf("  erase CDOSfile             type CDOSfile\n");
	printf("  quit                       xit\n");
#if	FORMATCMD
	printf("  format\n");
#endif
	printf("\nOnly the first letter of the COMMAND is required.\n");
	printf("'[]' brackets delimit optional arguments\n");
	printf("You will be prompted for missing required arguments.\n");
	printf("For write calls, leading 'x:' will be removed from the\n");
	printf("filename before writing to the CDOS file system\n");
	printf("You must select a disk first before using other commands.\n");
}

myparse(av)
BYTE	**av;
{
	BYTE	line[256];
	BYTE	c;
	REG BYTE	*lp, *ap;
	WORD	cnt;

	gets(line);
	lp = line;
	cnt = 0;
	while( c = *lp++ )
	{
		if( c == ' ' || c == '\t' )
			continue;
		if( c == '\0' )
			break;
		ap = av[cnt++];
		*ap++ = c;
		while( 1 )
		{
			c = *lp++;
			if( c == ' ' || c == '\t' || c == '\0' )
			{
				lp--;
				*ap =  '\0';
				break;
			}
			*ap++ = c;
		}
	}
	return cnt;
}
