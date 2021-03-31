>>>cc0476.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					    /* cc046x/47x[] = %9 for *cc0500 */

/* cc0476[]: 								     */
/*		--- Generated for the following errors ---		     */
/*	E_CORRUPT on CDOS file access functions.			     */
/*									     */
BYTE	cc0476[] =
"\r\nThe file allocation tables are corrupt.\
\r\nRun CHKDSK on the disk.";
>>>aconv.c
#include	"portab.h"

BYTE	aconv[] = { 'a', 'z', 0xe0, 'A', 'Z', 0x20 };	/* used in case xlat */
>>>backspc.c
#include	"portab.h"

BYTE	backspace[] = "\b \b";			    /* destructive backspace */
>>>bell.c
#include	"portab.h"

BYTE	bell[] = { 0x0d, 0x0a, 0x07, '\0' };		      /* <CRLF> BELL */
>>>blank.c
#include	"portab.h"

BYTE	blank = ' ';
>>>bootdev.c
#include	"portab.h"
#include	"concur.h"

BYTE	bootdev[] = LN_BOOT;				 /* boot device name */
>>>carret.c
#include	"portab.h"

BYTE	carret = '\r';
>>>cc0290.c
#include	"portab.h"
#include	"ccutls.h"

/* cc029x[] messages are used by UTERRMSG and UTPRNMSG			     */


ERRSRC	cc0290[] = {				   /* DOS error source names */
	{ 0x00, 0x00, "KRNL/SUP" },
	{ 0x10, 0x10, "PIPEMAN"  },
	{ 0x20, 0x20, "DISKMAN"  },
	{ 0x21, 0x2f, "DISKDRVR" },
	{ 0x30, 0x30, "CONMAN"   },
	{ 0x31, 0x3f, "CONDRVR"  },
	{ 0x40, 0x40, "COM/LOAD" },
	{ 0x50, 0x50, "OEMMAN"   },
	{ 0x51, 0x5f, "OEMDRVR"  },
	{ 0x60, 0x60, "NETMAN"   },
	{ 0x61, 0x6f, "NETDRVR"  },
	{ 0x70, 0x70, "MISCMAN"  },
	{ 0x71, 0x7f, "MISCDRVR" },
	{ 0x81, 0xfe, "SPECDRVR" },
	{ 0x00, 0xff, "UNKNOWN"  }
	};
	

>>>cc0291.c
/* cc029x[] messages are used by UTERRMSG and UTPRNMSG			     */
#include	"portab.h"

						       /* DOS function names */
BYTE	*cc0291[] = {
	"NONE",
	"F_GET",
	"F_SET",
	"F_LOOKUP",
	"F_CREATE",
	"F_DELETE",
	"F_OPEN",
	"F_CLOSE",
	"F_READ",
	"F_WRITE",
	"F_SPECIAL",
	"F_RENAME",
	"F_DEFINE",
	"F_DEVLOCK",
	"F_INSTALL",
	"F_LOCK",
	"F_COPY",
	"F_ALTER",
	"F_XLAT",
	"F_RWAIT",
	"F_KCTRL",
	"F_ORDER",
	"F_KEYPUT",
	"F_GIVE",
	"F_BWAIT",
	"F_TIMER",
	"F_EXIT",
	"F_ABORT",
	"F_CANCEL",
	"F_WAIT",
	"F_STATUS",
	"F_RETURN",
	"F_EXCEPTION",
	"F_ENABLE",
	"F_DISABLE",
	"F_SWIRET",
	"F_MALLOC",
	"F_MFREE",
	"F_OVERLAY",
	"F_COMMAND",
	"F_CONTROL",
	"F_GSX",
	"F_SEEK"
	};

>>>cc0292.c
/* cc029x[] messages are used by UTERRMSG and UTPRNMSG			     */
#include	"portab.h"

BYTE	cc0292 = '%';			   /* print routine INSERT delimiter */

>>>cc0293.c
/* cc029x[] messages are used by UTERRMSG and UTPRNMSG			     */

#include	"portab.h"

BYTE	cc0293[] =
	"\r\n\r\nReturn code: %9";

>>>cc0294.c
/* cc029x[] messages are used by UTERRMSG and UTPRNMSG			     */

#include	"portab.h"

BYTE	cc0294[] =
	"\r\nError source: %9";

>>>cc0295.c
/* cc029x[] messages are used by UTERRMSG and UTPRNMSG			     */

#include	"portab.h"

BYTE	cc0295[] =
	"\r\nCDOS function: %9";

>>>cc0300.c
#include	"portab.h"

/* cc03xx messages are Concurrent DOS table names. (%4's for *cc0500)	     */

BYTE	cc0300[] = "Process";
>>>cc0301.c
#include	"portab.h"

/* cc03xx messages are Concurrent DOS table names. (%4's for *cc0500)	     */

BYTE	cc0301[] = "Process Environment";
>>>cc0302.c
#include	"portab.h"

/* cc03xx messages are Concurrent DOS table names. (%4's for *cc0500)	     */

BYTE	cc0302[] = "Time and Date";
>>>cc0303.c
#include	"portab.h"

/* cc03xx messages are Concurrent DOS table names. (%4's for *cc0500)	     */

BYTE	cc0303[] = "Memory Information";
>>>cc0304.c
#include	"portab.h"

/* cc03xx messages are Concurrent DOS table names. (%4's for *cc0500)	     */

BYTE	cc0304[] = "Pipe Information";
>>>cc0305.c
#include	"portab.h"

/* cc03xx messages are Concurrent DOS table names. (%4's for *cc0500)	     */

BYTE	cc0305[] = "Disk File Information";
>>>cc0306.c
#include	"portab.h"

/* cc03xx messages are Concurrent DOS table names. (%4's for *cc0500)	     */

BYTE	cc0306[] = "Disk Device Information";
>>>cc0307.c
#include	"portab.h"

/* cc03xx messages are Concurrent DOS table names. (%4's for *cc0500)	     */

BYTE	cc0307[] = "Console Information";
>>>cc0308.c
#include	"portab.h"

/* cc03xx messages are Concurrent DOS table names. (%4's for *cc0500)	     */

BYTE	cc0308[] = "Mouse Information";
>>>cc0309.c
#include	"portab.h"

/* cc03xx messages are Concurrent DOS table names. (%4's for *cc0500)	     */

BYTE	cc0309[] = "Window (vcon) Information";
>>>cc0310.c
#include	"portab.h"

/* cc03xx messages are Concurrent DOS table names. (%4's for *cc0500)	     */

BYTE	cc0310[] = "Physical Console Information";
>>>cc0311.c
#include	"portab.h"

/* cc03xx messages are Concurrent DOS table names. (%4's for *cc0500)	     */

BYTE	cc0311[] = "System Information";
>>>cc0312.c
#include	"portab.h"

/* cc03xx messages are Concurrent DOS table names. (%4's for *cc0500)	     */

BYTE	cc0312[] = "File Number Information";
>>>cc0313.c
#include	"portab.h"

/* cc03xx messages are Concurrent DOS table names. (%4's for *cc0500)	     */

BYTE	cc0313[] = "System Define";
>>>cc0314.c
#include	"portab.h"

/* cc03xx messages are Concurrent DOS table names. (%4's for *cc0500)	     */

BYTE	cc0314[] = "Process Define";
>>>cc0315.c
#include	"portab.h"

/* cc03xx messages are Concurrent DOS table names. (%4's for *cc0500)	     */

BYTE	cc0315[] = "Command Environment";
>>>cc0316.c
#include	"portab.h"

/* cc03xx messages are Concurrent DOS table names. (%4's for *cc0500)	     */

BYTE	cc0316[] = "Device Information";
>>>cc0317.c
#include	"portab.h"

/* cc03xx messages are Concurrent DOS table names. (%4's for *cc0500)	     */

BYTE	cc0317[] = "Path Compression";
>>>cc0318.c
#include	"portab.h"

/* cc03xx messages are Concurrent DOS table names. (%4's for *cc0500)	     */

BYTE	cc0318[] = "Printer Device";
>>>cc0319.c
#include	"portab.h"

/* cc03xx messages are Concurrent DOS table names. (%4's for *cc0500)	     */

BYTE	cc0319[] = "Port Device";
>>>cc0320.c
#include	"portab.h"

/* cc03xx messages are Concurrent DOS table names. (%4's for *cc0500)	     */

BYTE	cc0320[] = "Special Device";

>>>cc0410.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					 /* *cc041x[] = Miscellaneous errors */

BYTE	*cc0410[] = {		    /* this may be due to unavailable memory */
"\r\n%0: Directory levels too deep.",
"\r\n%0: The directory levels are too deep for this operation.",
"%5%6"				   /* %5 = cc0410[1], %6 = cc0420 or nullstr */
	};

>>>cc0411.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					 /* *cc041x[] = Miscellaneous errors */
					    /* %5 = cc0411[1], %6 = cc0447[] */

BYTE	*cc0411[] = {
"\r\n%0: Not enough memory.",
"\r\n%0: There is not enough memory available to complete this operation.",
"%5\r\nOther programs may be using available memory.\
\r\nReenter the command when these programs are finished.%6"
	};

>>>cc0412.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					 /* *cc041x[] = Miscellaneous errors */

BYTE	*cc0412[] = {					   /* %5 = cc0412[1] */
"\r\n%0: Line too long.",
"\r\n%0: %0 found a line longer than 256 characters.",
"%5\r\nUse a text editor to split lines longer than 256 characters\
\r\ninto shorter ones."
	};

>>>cc0413.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					 /* *cc041x[] = Miscellaneous errors */

BYTE	*cc0413[] = {
"\r\n%0: Nonremovable media error.",
"\r\n%0: Both transfer devices are the same,\
\r\nand contain nonremovable media.",
""
	};

>>>cc0414.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					 /* *cc041x[] = Miscellaneous errors */
			     /* cc0413[] = Special message for E_POOL error. */
BYTE	cc0414[] =
"\r\n%0: Possible bad data on drive%1.\
\r\nRun CHKDSK on the disk and verify the validity of any\
\r\nrecently updated files.";

>>>cc0415.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					 /* *cc041x[] = Miscellaneous errors */

BYTE	*cc0415[] = {
"\r\n%0: User terminated.",
"\r\n%0: Process terminated by user abort (CTRL-C).",
""
	};

>>>cc0416.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					 /* *cc041x[] = Miscellaneous errors */

BYTE	*cc0416[] = {
"\r\n%0: Process terminated.",
"\r\n%0: Process terminated by external abort.",
""
	};

>>>cc0420.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					   /* cc042x[] = inserts for *cc041x */

					   /* Below = %6 insert for cc0411[] */
BYTE	cc0420[] =
"\r\nA directory path cannot have more than %7 levels.\
\r\nMake sure that you entered your directory path correctly.\
\r\nIf you entered it correctly, you may have to restructure your\
\r\ndirectories so that you have %7 levels or fewer.";

>>>cc0430.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					     /* *cc043x[] = Parameter errors */

BYTE	*cc0430[] = {
	"\r\n%0: Invalid parameter.",
	"\r\n%0: You specified an invalid parameter.",
	"%5%6"				      /* %5 = cc0430[1], %6 = cc044x */
	};

>>>cc0431.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					     /* *cc043x[] = Parameter errors */

BYTE	*cc0431[] = {
	"\r\n%0: Parameter error.",
	"\r\n%0: You specified an invalid number of parameters.",
	"%5%6"			     /* %5 = cc0431[1], %6 = cc0446 or other */
	};

>>>cc0440.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

			    /* cc044x[] = inserts for the *cc043x[] messages */

						  /* cc0440 is %6 for cc0430 */
BYTE	cc0440[] =
"\r\nYour %7 is too long.%8";	       /* %7 = cc045x, %8 = cc0441 or cc0442 */

>>>cc0441.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

			    /* cc044x[] = inserts for the *cc043x[] messages */

				/* Below: %8 for cc0440; %7 = cc045x, %4 = # */
BYTE	cc0441[] =
"\r\nA %7 must be %4 characters or less.\
\r\nReenter your command line.";

>>>cc0442.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

			    /* cc044x[] = inserts for the *cc043x[] messages */

			      /* Below: %8 for cc0440; %7 = cc0451 or cc0454 */
BYTE	cc0442[] =
"\r\nYour complete %7, from the root directory to the level you\
\r\nwant to access, must be 128 characters or less. If you use logical names\
\r\nin your %7, the original 'long form' must be\
\r\n128 characters or less.";

>>>cc0443.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

			    /* cc044x[] = inserts for the *cc043x[] messages */

					/* Below: %6 for cc0430; %7 = cc045x */
BYTE	cc0443[] =
"\r\nConcurrent expected a %7. Reenter your command line.";

>>>cc0444.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

			    /* cc044x[] = inserts for the *cc043x[] messages */

						     /* Below: %6 for cc0430 */
BYTE	cc0444[] =
"\r\nThe date is invalid. Make sure the values for month, day, and year are\
\r\nwithin the correct ranges and that you are using either hyphens (-)\
\r\nor slashes (/) for your separators.";

>>>cc0445.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

			    /* cc044x[] = inserts for the *cc043x[] messages */

					/* Below: %6 for cc0430, %7 = ut0001 */
BYTE	cc0445[] =
"\r\nAn invalid option was used. Valid options are %7.";

>>>cc0446.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

			    /* cc044x[] = inserts for the *cc043x[] messages */

				       /* Below: %6 for *cc0431; %7 = ut0002 */
BYTE	cc0446[] =
"\r\nThe most common form(s) of %0 are: %7";

>>>cc0447.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

		      /* cc0447[] = %6 (extra help information) for cc0411[] */

BYTE	cc0447[] =
"\r\nIf no other programs are running on your system, your computer's\
\r\nmemory is not large enough for this operation.";

>>>cc0448.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

			    /* cc044x[] = inserts for the *cc043x[] messages */

						   /* Special: %x for ut0002 */
BYTE	cc0448[] =
"\r\nFor additional forms of %0, see the Concurrent DOS documentation.";

>>>cc0450.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

		      /* cc045x[] = parameter descriptors (%7's for cc044x[] */

BYTE	cc0450[] = "device name";
>>>cc0451.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

		      /* cc045x[] = parameter descriptors (%7's for cc044x[] */

BYTE	cc0451[] = "path name";
>>>cc0452.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

		      /* cc045x[] = parameter descriptors (%7's for cc044x[] */

BYTE	cc0452[] = "filename";
>>>cc0453.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

		      /* cc045x[] = parameter descriptors (%7's for cc044x[] */

BYTE	cc0453[] = "file extension";
>>>cc0454.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

		      /* cc045x[] = parameter descriptors (%7's for cc044x[] */

BYTE	cc0454[] = "file specification";
>>>cc0455.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

		      /* cc045x[] = parameter descriptors (%7's for cc044x[] */

BYTE	cc0455[] = "disk device name";
>>>cc0456.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

		      /* cc045x[] = parameter descriptors (%7's for cc044x[] */

BYTE	cc0456[] = "floppy disk device name";
>>>cc0457.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

		      /* cc045x[] = parameter descriptors (%7's for cc044x[] */

BYTE	cc0457[] = "hard disk device name";
>>>cc0458.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

		      /* cc045x[] = parameter descriptors (%7's for cc044x[] */

BYTE	cc0458[] = "directory name";
>>>cc0459.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

		      /* cc045x[] = parameter descriptors (%7's for cc044x[] */

BYTE	cc0459[] = "command line";

>>>cc045a.c
#include	"portab.h"

					 /* cc045x[] = parameter descriptors */

BYTE	cc045A[] = "file name (filename.ext)";

>>>cc0460.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					    /* cc046x/47x[] = %9 for *cc0500 */

/* cc0460[]: %1 = device, %4 = cc0490 or nullstr			     */
/*		--- Generated for the following errors ---		     */
/*	E_SPACE		on	S_CREATE				     */
BYTE	cc0460[] =
"\r\nThe directory of %1 is full.%4";

>>>cc0461.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					    /* cc046x/47x[] = %9 for *cc0500 */

/* cc0461[]: %1 = device, %4 = cc0490 or nullstr			     */
/*		--- Generated for the following errors ---		     */
/*	E_SPACE		on	S_WRITE					     */
BYTE	cc0461[] =
"\r\nDisk %1 is full.%4";

>>>cc0462.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					    /* cc046x/47x[] = %9 for *cc0500 */

/* cc0462[]: %1 = device, %2 = file, %4 = cc0491 or nullstr		     */
/*		--- Generated for the following errors ---		     */
/*	E_NO_FILE	on	S_OPEN or S_GET (Diskfile table)	     */
/*	0		on	S_LOOKUP (Diskfile table)		     */
BYTE	cc0462[] =
"\r\nThe file %2 was not found on %1.%4";

>>>cc0463.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					    /* cc046x/47x[] = %9 for *cc0500 */

/* cc0463[]: %1 = device name, %4 = cc0492 or nullstr			     */
/*		--- Generated for the following errors ---		     */
/*	E_NO_FILE	on	S_OPEN or S_GET (Disk,Console,Mouse,	     */
/*				Vconsole,Pconsole,Printer, Port or	     */
/*				Special tables)				     */
/*	0		on	S_LOOKUP (Device or Vconsole table)	     */
BYTE	cc0463[] =
"\r\nThe device %1 was not found.%4";

>>>cc0464.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					    /* cc046x/47x[] = %9 for *cc0500 */

/* cc0464[]:								     */
/*		--- Generated for the following errors ---		     */
/*	E_PATH		on	S_GET, S_SET or S_LOOKUP (Diskfile table) or */
/*				S_CREATE, S_DELETE, S_OPEN or S_RENAME	     */
BYTE	cc0464[] =
"\r\nThe directory path is invalid. Most likely you have entered a path that\
\r\ndoes not exist, a path name that is too long, or you did not use the\
\r\nproper separators in your path.";

>>>cc0465.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					    /* cc046x/47x[] = %9 for *cc0500 */

/* cc0465[]:								     */
/*		--- Generated for the following errors ---		     */
/*	E_CONFLICT	on	S_CREATE, S_OPEN, S_READ or S_WRITE	     */
/*	E_DEVCONFLICT	on	S_OPEN, S_READ or S_WRITE		     */
/*	E_DEVLOCK	on	S_OPEN, S_READ or S_WRITE		     */
BYTE	cc0465[] =
"\r\nThe operation failed due to an access conflict.\
\r\nThe file or device you are trying to access may be in use or locked\
\r\nby another user. This is a temporary situation.";

>>>cc0466.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					    /* cc046x/47x[] = %9 for *cc0500 */

/* cc0466[]:								     */
/*		--- Generated for the following errors ---		     */
/*	E_WPROT		on	S_CREATE or S_WRITE			     */
BYTE	cc0466[] =
"\r\nYour disk is write protected and you can not change \
the information on it.\
\r\nIf you want to change information on the disk, \
alter the write protection tab.";

>>>cc0467.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					    /* cc046x/47x[] = %9 for *cc0500 */

/* cc0467[]:								     */
/*		--- Generated for the following errors ---		     */
/*	E_ACCESS	on	S_CREATE, S_SET, S_DELETE, S_OPEN,	     */
/*				S_READ or S_WRITE			     */
BYTE	cc0467[] =
"\r\nYou do not have sufficient privileges to perform this operation.\
\r\nEither you do not have the proper Owner, Group and World privileges,\
\r\nor you are attempting an operation reserved for system managers.";

>>>cc0468.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					    /* cc046x/47x[] = %9 for *cc0500 */

/* cc0468[]: %4 = cc045x						     */
/*		--- Generated for the following errors ---		     */
/*	E_NAME		on	S_LOOKUP, S_CREATE, S_DELETE,		     */
/*				S_OPEN or S_RENAME			     */
BYTE	cc0468[] =
"\r\nYou specified an invalid %4. You either specified too\
\r\nmany characters or used invalid characters in your %4.";

>>>cc0469.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					    /* cc046x/47x[] = %9 for *cc0500 */

/* cc0469[]: (SPECIAL !!!) %2 = directory path, %4 = cc0493 or nullstr	     */
/*		--- Generated for the following errors ---		     */
/*	E_NO_FILE	on	S_CREATE, S_DELETE, S_OPEN or S_RENAME       */
/*	0		on	S_LOOKUP (Diskfile table)		     */
BYTE	cc0469[] =
"\r\nThe directory was not found.%4";

>>>cc0470.c
#include	"portab.h"

					    /* cc046x/47x[] = %9 for *cc0500 */

/* cc0470[]: %2 = filename.ext, %4 = cc0491 or nullstr			     */
/*		--- Generated for the following errors ---		     */
/*	E_NO_FILE	on	S_OPEN or S_GET (Diskfile table)	     */
/*	0		on	S_LOOKUP (Diskfile table)		     */
BYTE	cc0470[] =
"\r\nThe file %2 was not found.%4";

>>>cc0471.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					    /* cc046x/47x[] = %9 for *cc0500 */

/* cc0471[]: 								     */
/*		--- Generated for the following errors ---		     */
/*	default	err	on	S_GET, S_SET or S_LOOKUP		     */
/*									     */
BYTE	cc0471[] =
"\r\nConcurrent DOS is presently unable to perform the %0 command.\
\r\nThis may be a temporary condition. Reenter the command.\
\r\nIf the error continues, refer to the section on error handling\
\r\nin the Concurrent DOS documentation.";

>>>cc0472.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					    /* cc046x/47x[] = %9 for *cc0500 */

/* cc0472[]: 								     */
/*		--- Generated for the following errors ---		     */
/*	E_IMPLEMENT on any CDOS call.					     */
/*									     */
BYTE	cc0472[] =
"\r\nThis operation is not supported.";


>>>cc0473.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					    /* cc046x/47x[] = %9 for *cc0500 */

/* cc0473[]: 								     */
/*		--- Generated for the following errors ---		     */
/*	E_FULL error on S_CREATE, S_OPEN, S_VCCREATE, etc.		     */
/*									     */
BYTE	cc0473[] =
"\r\nThe system file number table is full.";


>>>cc0474.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					    /* cc046x/47x[] = %9 for *cc0500 */

/* cc0474[]: 								     */
/*		--- Generated for the following errors ---		     */
/*	E_MEDIACHANGE and E_MEDCHGERR for CDOS disk access functions.	     */
/*									     */
BYTE	cc0474[] =
"\r\nThe file system detected a media change.\
\r\nRetry the operation.";


>>>cc0475.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					    /* cc046x/47x[] = %9 for *cc0500 */

/* cc0475[]: 								     */
/*		--- Generated for the following errors ---		     */
/*	E_READONLY on CDOS file access functions.			     */
/*									     */
BYTE	cc0475[] =
"\r\nThe file is marked READ ONLY.";


>>>cc0477.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					    /* cc046x/47x[] = %9 for *cc0500 */

/* cc0477[]: 								     */
/*		--- Generated for the following errors ---		     */
/*	E_RAWMEDIA on CDOS disk access functions.			     */
/*									     */
BYTE	cc0477[] =
"\r\nThe media type is unknown and is not supported by the file system.";


>>>cc0478.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					    /* cc046x/47x[] = %9 for *cc0500 */

/* cc0478[]: 								     */
/*		--- Generated for the following errors ---		     */
/*	E_READY on CDOS device access functions.			     */
/*									     */
BYTE	cc0478[] =
"\r\nThe device is not ready.";


>>>cc0479.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					    /* cc046x/47x[] = %9 for *cc0500 */

/* cc0479[]: 								     */
/*		--- Generated for the following errors ---		     */
/*	E_DKATTACH on CDOS device access functions.			     */
/*									     */
BYTE	cc0479[] =
"\r\nThe device did not respond.";


>>>cc0480.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					       /* cc048x[] = general prompts */

					       /* %5 = cc0578/9, %6 = device */
BYTE	cc0480[] =
"\r\nInsert %5 diskette into drive %6.\
\r\nPress any key when you are ready. ";

>>>cc0481.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

					       /* cc048x[] = general prompts */

						     /* %5 = max#, %6 = bell */
BYTE	cc0481[] =
"\r\nYou entered an invalid selection.\
\r\nValid selections are 1 to %5.\
%6Try again: ";

>>>cc0490.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

	      /* cc049x[] = %4 (extra help information) for *cc0500 (cc046x) */

							    /* %4 for cc0460 */
BYTE	cc0490[] =
"\r\nYou can erase unnecessary files from your disk to free up space.";

>>>cc0491.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

	      /* cc049x[] = %4 (extra help information) for *cc0500 (cc046x) */

							    /* %4 for cc0462 */
BYTE	cc0491[] =
"\r\nMake sure that you specified the correct device, path, and file names.";

>>>cc0492.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

	 /* cc049x[] = %4 (extra help information) for *cc0500 (cc046x/047x) */

							    /* %4 for cc0463 */
BYTE	cc0492[] =
"\r\nMake sure that you specified the correct device name.";

>>>cc0493.c
#include	"portab.h"

/* cc04xx[] general purpose messages					     */

	 /* cc049x[] = %4 (extra help information) for *cc0500 (cc046x/047x) */

							    /* %4 for cc0469 */
BYTE	cc0493[] =
"\r\nMake sure you specified correct directory name.";

>>>cc0500.c
#include	"portab.h"

/*
   cc05xx series messages: These messages are intended to report most DOS
	related errors. This series may require use of the entire parmptr
	block. The basic message is cc0500. The following shows the use of
	the parmptr inserts:

	%0 = module name.
	%1 = device name.
	%2 = filename (if any).
	%3 = area name (if any), generally one of the cc057x series.
	%4 = may be used for extra help information.
	%5 = operation name, generally one of the cc051x/cc052x series.
	%6 = operation name, generally one of the cc053x/cc054x series.
	%7 = device/file/area string, generally of the cc055x series.
	%8 = level 3 help basic message, generally the cc0500[1] message.
	%9 = level 3 help message, generally one of the cc046x series.

	Example message for an S_WRITE that returns an E_SPACE error:

	%0 = "COPY"
	%1 = "a:"
	%2 = "testfile.txt"
	%3 = nullstr (not used).
	%4 = "\r\nYou can erase unneccessary files from your disk to
	      free up space." (cc0490).
	%5 = "WRITE" (cc0526).
	%6 = "WRITING" (cc0546).
	%7 = "%2 on %1" (cc0551).
	%8 = "\r\n%0: An error occurred %6 %7." (cc0500[1]).
	%9 = "\r\nThe disk %1 is full." (cc0461).

	HELPLVL 4:
	    COPY: An error occurred WRITING testfile.txt to a:.
	    The disk a: is full.
	    You can erase unneccessary files from your disk to free up space.

	HELPLVL 3:
	    COPY: An error occurred WRITING testfile.txt to a:.

	HELPLVL 2:
	    COPY: WRITE error.

	HELPLVL 1:
	    This level is reserved by the ERRMSG routine to provide
	    information such as RETURN CODE, ERROR SOURCE and CDOS
	    FUNCTION. An example is shown below.

		Return code: -2145369344.
		Error source: DISKMAN.
		CDOS function: F_WRITE.

*/
						     /* basic message format */
BYTE	*cc0500[] = {
	"\r\n%0: %5 error.",
	"\r\n%0: An error occurred %6 %7.",
	"%8%9"
	};

/* Notes on F_SPECIAL errors: */
/* All use *cc0500 as the message base. */
/* All use cc0529 for %5 */
/* F_SPECIAL(RDSYS): %3 = cc0576, %6 = cc0542, & %7 = cc0561 */
/* F_SPECIAL(WTSYS): %3 = cc0576, %6 = cc0547, & %7 = cc0561 */
/* F_SPECIAL(FMSYS): %3 = cc0576, %6 = cc0536, & %7 = cc0561 */
/* F_SPECIAL(FMTRK): %3 = cc0572, %6 = cc0536, & %7 = cc0561 */
/* F_SPECIAL(MDCHK): %3 = cc0572, %6 = cc0549, & %7 = cc0559 */

>>>cc0510.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc051x and 52x[] = %5 operations */

BYTE	cc0510[] = "Allocate";		  /* F_MALLOC (special: %7 = cc0574) */
>>>cc0511.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc051x and 52x[] = %5 operations */

BYTE	cc0511[] = "Cancel";	      /* F_ABORT (special: %1 = processname) */
>>>cc0512.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc051x and 52x[] = %5 operations */

BYTE	cc0512[] = "Close";					  /* F_CLOSE */
>>>cc0513.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc051x and 52x[] = %5 operations */

BYTE	cc0513[] = "Create";					 /* F_CREATE */
>>>cc0514.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc051x and 52x[] = %5 operations */

BYTE	cc0514[] = "Define";					 /* F_DEFINE */
>>>cc0515.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc051x and 52x[] = %5 operations */

BYTE	cc0515[] = "Delete";					 /* F_DELETE */
>>>cc0516.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc051x and 52x[] = %5 operations */

BYTE	cc0516[] = "Format";			       /* F_SPECIAL (FMTTRK) */
>>>cc0517.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc051x and 52x[] = %5 operations */

BYTE	cc0517[] = "Get";				/* F_GET or F_LOOKUP */
>>>cc0518.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc051x and 52x[] = %5 operations */

BYTE	cc0518[] = "Install";					/* F_INSTALL */
>>>cc0519.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc051x and 52x[] = %5 operations */

BYTE	cc0519[] = "Lock";					   /* F_LOCK */
>>>cc0520.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc051x and 52x[] = %5 operations */

BYTE	cc0520[] = "Open";					   /* F_OPEN */
>>>cc0521.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc051x and 52x[] = %5 operations */

BYTE	cc0521[] = "Overlay";					/* F_OVERLAY */
>>>cc0522.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc051x and 52x[] = %5 operations */

BYTE	cc0522[] = "Read";					   /* F_READ */
>>>cc0523.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc051x and 52x[] = %5 operations */

BYTE	cc0523[] = "Rename";					 /* F_RENAME */
>>>cc0524.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc051x and 52x[] = %5 operations */

BYTE	cc0524[] = "Search";					 /* F_LOOKUP */
>>>cc0525.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc051x and 52x[] = %5 operations */

BYTE	cc0525[] = "Set";					    /* F_SET */
>>>cc0526.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc051x and 52x[] = %5 operations */

BYTE	cc0526[] = "Verify";					   /* F_READ */
>>>cc0527.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc051x and 52x[] = %5 operations */

BYTE	cc0527[] = "Write";					  /* F_WRITE */
>>>cc0528.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc051x and 52x[] = %5 operations */

BYTE	cc0528[] = "Deallocate";				  /* F_MFREE */
>>>cc0529.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc051x and 52x[] = %5 operations */

BYTE	cc0529[] = "Special";					/* F_SPECIAL */

>>>cc0530.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc053x and 54x[] = %6 operations */

BYTE	cc0530[] = "allocating";	  /* F_MALLOC (special: %7 = cc0574) */
>>>cc0531.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc053x and 54x[] = %6 operations */

BYTE	cc0531[] = "canceling";	      /* F_ABORT (special: %1 = processname) */
>>>cc0532.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc053x and 54x[] = %6 operations */

BYTE	cc0532[] = "closing";					  /* F_CLOSE */
>>>cc0533.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc053x and 54x[] = %6 operations */

BYTE	cc0533[] = "creating";					 /* F_CREATE */
>>>cc0534.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc053x and 54x[] = %6 operations */

BYTE	cc0534[] = "defining";					 /* F_DEFINE */
>>>cc0535.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc053x and 54x[] = %6 operations */

BYTE	cc0535[] = "deleting";					 /* F_DELETE */
>>>cc0536.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc053x and 54x[] = %6 operations */

BYTE	cc0536[] = "formatting";		       /* F_SPECIAL (FMTTRK) */
>>>cc0537.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc053x and 54x[] = %6 operations */

BYTE	cc0537[] = "getting";				/* F_GET or F_LOOKUP */
>>>cc0538.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc053x and 54x[] = %6 operations */

BYTE	cc0538[] = "installing";				/* F_INSTALL */
>>>cc0539.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc053x and 54x[] = %6 operations */

BYTE	cc0539[] = "locking";					   /* F_LOCK */
>>>cc0540.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc053x and 54x[] = %6 operations */

BYTE	cc0540[] = "opening";					   /* F_OPEN */
>>>cc0541.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc053x and 54x[] = %6 operations */

BYTE	cc0541[] = "overlaying";				/* F_OVERLAY */
>>>cc0542.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc053x and 54x[] = %6 operations */

BYTE	cc0542[] = "reading";					   /* F_READ */
>>>cc0543.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc053x and 54x[] = %6 operations */

BYTE	cc0543[] = "renaming";					 /* F_RENAME */
>>>cc0544.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc053x and 54x[] = %6 operations */

BYTE	cc0544[] = "searching";					 /* F_LOOKUP */
>>>cc0545.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc053x and 54x[] = %6 operations */

BYTE	cc0545[] = "setting";					    /* F_SET */
>>>cc0546.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc053x and 54x[] = %6 operations */

BYTE	cc0546[] = "verifying";					   /* F_READ */
>>>cc0547.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc053x and 54x[] = %6 operations */

BYTE	cc0547[] = "writing";					  /* F_WRITE */
>>>cc0548.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc053x and 54x[] = %6 operations */

BYTE	cc0548[] = "deallocating";	   /* F_MFREE (special: %7 = cc0574) */
>>>cc0549.c
#include	"portab.h"

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc053x and 54x[] = %6 operations */

BYTE	cc0549[] = "logging";				 /* F_SPECIAL(MDCHK) */
>>>cc0550.c
#include	"portab.h"

			  /* cc055x & cc056x are %7 message modifier strings */

BYTE	cc0550[] = "%1";				     /* device error */
>>>cc0551.c
#include	"portab.h"

			  /* cc055x & cc056x are %7 message modifier strings */

BYTE	cc0551[] = "%2 on %1";			     /* file on device error */
>>>cc0552.c
#include	"portab.h"

			  /* cc055x & cc056x are %7 message modifier strings */

BYTE	cc0552[] = "for %1";				     /* device error */
>>>cc0553.c
#include	"portab.h"

			  /* cc055x & cc056x are %7 message modifier strings */

BYTE	cc0553[] = "for %2 on %1";		     /* file on device error */
>>>cc0554.c
#include	"portab.h"

			  /* cc055x & cc056x are %7 message modifier strings */

BYTE	cc0554[] = "to %1";				     /* device error */
>>>cc0555.c
#include	"portab.h"

			  /* cc055x & cc056x are %7 message modifier strings */

BYTE	cc0555[] = "to %2 on %1";		     /* file on device error */
>>>cc0556.c
#include	"portab.h"

			  /* cc055x & cc056x are %7 message modifier strings */

BYTE	cc0556[] = "from %1";				     /* device error */
>>>cc0557.c
#include	"portab.h"

			  /* cc055x & cc056x are %7 message modifier strings */

BYTE	cc0557[] = "from %2 on %1";		     /* file on device error */
>>>cc0558.c
#include	"portab.h"

			  /* cc055x & cc056x are %7 message modifier strings */

BYTE	cc0558[] = "in the %3 area";			       /* area error */
>>>cc0559.c
#include	"portab.h"

			  /* cc055x & cc056x are %7 message modifier strings */

BYTE	cc0559[] = "in the %3";				       /* area error */
>>>cc0560.c
#include	"portab.h"

			  /* cc055x & cc056x are %7 message modifier strings */

BYTE	cc0560[] = "the %4 table";		       /* table access error */
>>>cc0561.c
#include	"portab.h"

			  /* cc055x & cc056x are %7 message modifier strings */

BYTE	cc0561[] = "the %3";				  /* S_SPECIAL error */
>>>cc0570.c
#include	"portab.h"

					      /* cc057x/58x[] = areas (%3's) */

BYTE	cc0570[] = "boot record";
>>>cc0571.c
#include	"portab.h"

					      /* cc057x/58x[] = areas (%3's) */

BYTE	cc0571[] = "directory";
>>>cc0572.c
#include	"portab.h"

					      /* cc057x/58x[] = areas (%3's) */

BYTE	cc0572[] = "disk";
>>>cc0573.c
#include	"portab.h"

					      /* cc057x/58x[] = areas (%3's) */

BYTE	cc0573[] = "file allocation table";
>>>cc0574.c
#include	"portab.h"

					      /* cc057x/58x[] = areas (%3's) */

BYTE	cc0574[] = "memory";
>>>cc0575.c
#include	"portab.h"

					      /* cc057x/58x[] = areas (%3's) */

BYTE	cc0575[] = "operating system file";
>>>cc0576.c
#include	"portab.h"

					      /* cc057x/58x[] = areas (%3's) */

BYTE	cc0576[] = "sysarea";
>>>cc0577.c
#include	"portab.h"

					      /* cc057x/58x[] = areas (%3's) */

BYTE	cc0577[] = "system";
>>>cc0578.c
#include	"portab.h"

					      /* cc057x/58x[] = areas (%3's) */

BYTE	cc0578[] = "source";
>>>cc0579.c
#include	"portab.h"

					      /* cc057x/58x[] = areas (%3's) */

BYTE	cc0579[] = "destination";
>>>cc0580.c
#include	"portab.h"

					      /* cc057x/58x[] = areas (%3's) */

BYTE	cc0580[] = "format information";
>>>cc0581.c
#include	"portab.h"

					      /* cc057x/58x[] = areas (%3's) */

BYTE	cc0581[] = "master boot partition record";
>>>cc0600.c
#include	"portab.h"

/* *cc06xx[] = messages common to FORMAT, FDISK and DISKCOPY		     */

BYTE	*cc0600[] = {
"\r\n%0: Sector size error.",				   /* %5 = cc0600[1] */
"\r\n%0: The sector size specified for this format is invalid.",
"%5\r\nThe sector size must be 32, 128, 256, 512, 1024, 2048 or 4096 bytes."
	};

>>>cc0601.c
#include	"portab.h"

/* *cc06xx[] = messages common to FORMAT, FDISK and DISKCOPY		     */

BYTE	*cc0601[] = {
"\r\n%0: Sectors per track error.",			   /* %5 = cc0601[1] */
"\r\n%0: The number of sectors per track specified for this format is invalid.",
"%5\r\nThe number of sectors per track must be between 1 and 256."
	};

>>>cc0602.c
#include	"portab.h"

/* *cc06xx[] = messages common to FORMAT, FDISK and DISKCOPY		     */

BYTE	*cc0602[] = {
"\r\n%0: The computer manufacturer does not offer this device format.",
"",
""
	};
>>>cc0700.c
#include	"portab.h"

/* cc070x[] = messages common to BACKUP and RESTORE			     */

BYTE	cc0700[] =
"BACKUPID.@@@";

>>>cc0701.c
#include	"portab.h"

/* cc070x[] = messages common to BACKUP and RESTORE			     */

BYTE	cc0701[] =
"@00";
					   /* %6 below = nullstr or bkmsg030 */
>>>cc0703.c
#include	"portab.h"

/* cc070x[] = messages common to BACKUP and RESTORE			     */

BYTE	cc0703[] =
"\r\nInsert the backup diskette #%5 in drive %3.%6";

>>>cc0704.c
#include	"portab.h"

/* cc070x[] = messages common to BACKUP and RESTORE			     */

BYTE	cc0704[] =
"\r\nPress any key when you are ready ... %5";			/* %5 = bell */
				       /* %6 for cc0431, %7 below = cc0578/9 */
>>>cc0705.c
#include	"portab.h"

/* cc070x[] = messages common to BACKUP and RESTORE			     */

BYTE	cc0705[] =
"\r\n%0 expects the %7 device to be a hard disk.";  /* use cc430/443/457 !!! */

>>>cc0706.c
#include	"portab.h"

/* cc070x[] = messages common to BACKUP and RESTORE			     */

BYTE	*cc0706[] = {
"\r\n%0: No files found.",
"\r\n%0: Warning! No files were found to %0.",
"\r\n%0: The system could not %0 the file(s) you specified.\
\r\nYou may have told %0 to look in the wrong place, or specified\
\r\na file that does not exist. Make sure that you specified the correct\
\r\n device, path, and file names."
	};

>>>cc0800.c
#include	"portab.h"

/* cc08xx[] =  messages common to FORMAT, SYS and UTXFRSYS()		     */

BYTE	cc0800[] =
"\r\nSystem transferred.";

>>>cc0801.c
#include	"portab.h"

/* cc08xx[] =  messages common to FORMAT, SYS and UTXFRSYS()		     */

BYTE	cc0801[] =
"\r\nTransferring %2.";

>>>cc0802.c
#include	"portab.h"

/* cc08xx[] =  messages common to FORMAT, SYS and UTXFRSYS()		     */

BYTE	cc0802[] =
"\r\nCDOS loader installed.";

>>>cc0810.c
#include	"portab.h"

/* cc08xx[] =  messages common to FORMAT, SYS and UTXFRSYS()		     */

BYTE	*cc0810[] = {
"\r\n%0: Concurrent DOS loader format error.",
"\r\n%0: The internal format of the Concurrent DOS loader file (%1)\
\r\nis incorrect.",
""
	};

>>>cc0811.c
#include	"portab.h"

/* cc08xx[] =  messages common to FORMAT, SYS and UTXFRSYS()		     */

BYTE	*cc0811[] = {
"\r\n%0: Space for CDOS loader is occupied.",
"\r\n%0: The Concurrent DOS loader must reside at the very start of the\
\r\nfile area on drive %1.",
"%5\r\nPlease delete any unneccessary files and try again."/* %5 = cc0811[1] */
	};

>>>cc0812.c
#include	"portab.h"

/* cc08xx[] =  messages common to FORMAT, SYS and UTXFRSYS()		     */

BYTE	*cc0812[] = {
"\r\n%0: Insufficient disk space.",
"\r\n%0: There is not enough room on %1 to hold %2.",
"%5%6"					      /* %5 = cc0812[1], %6 = cc0490 */
	};

>>>cc0813.c
#include	"portab.h"

/* cc08xx[] =  messages common to FORMAT, SYS and UTXFRSYS()		     */

BYTE	*cc0813[] = {					   /* %5 = cc0813[1] */
"\r\n%0: Non-bootable disk.",
"\r\n%0: The destination disk (%1) is not formatted to\
\r\nreceive operating system files.",
""
	};

>>>cc0900.c
#include	"portab.h"

/* cc09xx[] = messages common to CANCEL and PROCESS			     */

BYTE	cc0900[] =					   /* %7 for *cc0500 */
"the requested process";

>>>cc0910.c
#include	"portab.h"

/* cc09xx[] = messages common to CANCEL and PROCESS			     */

BYTE	cc0910[] =					   /* %9 for *cc0500 */
"\r\nThe process does not exist.";
>>>comma.c
#include	"portab.h"

BYTE	comma = ',';
>>>coname.c
#include	"portab.h"
#include	"concur.h"

BYTE	coname[] = LN_CNS;				     /* console name */
>>>crlfmsg.c
#include	"portab.h"

BYTE	crlfmsg[] = "\r\n";					   /* <CRLF> */
>>>dblslant.c
#include	"portab.h"

BYTE	dblslant[] = "//";
>>>defname.c
#include	"portab.h"
#include	"concur.h"

BYTE	defname[] = LN_DEF;				     /* default name */
>>>devdelim.c
#include	"portab.h"

BYTE	devdelim = ':';
>>>dir1dlim.c
#include	"portab.h"

BYTE	dir1delim = '/';
>>>dir2dlim.c
#include	"portab.h"

BYTE	dir2delim = '\\';
>>>dir3dlim.c
#include	"portab.h"

BYTE	dir3delim = '.';
>>>dlims.c
#include	"portab.h"
						/* 6 command line delimiters */
BYTE	dlims[] = { ' ', '\t', ',', ';', '=', '\0' };
>>>eofchar.c
#include	"portab.h"

BYTE	eofchar = 0x1a;
>>>equal.c
#include	"portab.h"

BYTE	equal = '=';
>>>erarstcp.c
#include	"portab.h"

BYTE	erarstcp[] = {	/* erase beg.line, restore cp (0x206f,0x206b - 16bit)*/
0x1b, 'o', 0x1b, 'k', NULL
	};
>>>eschar.c
#include	"portab.h"

BYTE	eschar = 0x1b;
>>>extdelim.c
#include	"portab.h"

BYTE	extdelim = '.';
>>>helplvl.c
#include	"portab.h"
#include	"concur.h"

BYTE	helplvl[] = LN_HLP;			   /* helplevel logical name */
>>>icdldr.c
#include	"portab.h"

BYTE	icdldr[] = "CDOSLDR.IMG";	   /* name of CDOS Loader Image file */
>>>ifile.c
#include	"portab.h"
#include	"concur.h"

BYTE	ifile[] = "CDOS.IMG";	   /* name of non-bootable system image file */
>>>linefeed.c
#include	"portab.h"

BYTE	linefeed = '\n';
>>>neg.c
#include	"portab.h"

BYTE	neg = '-';		    /* used in numeric to string conversions */
>>>netnode.c
#include	"portab.h"

BYTE	netnode[] = "::";
>>>nextline.c
#include	"portab.h"

BYTE	nextline[] = "\r\n%5";				     /* <CRLF> (msg) */
>>>nullmsg.c
#include	"portab.h"

							    /* blank message */
BYTE	*nullmsg[] = {			  /* used mainly for INTERNAL errors */
	"",
	"",
	""
	};

>>>nullstr.c
#include	"portab.h"

BYTE	nullstr[] = "";					     /* blank string */
>>>numconv.c
#include	"portab.h"

BYTE	numconv[] = "0123456789ABCDEF";	   /* used for hex and decimal convs */
>>>plus.c
#include	"portab.h"

BYTE	plus = '+';
>>>rstcp.c
#include	"portab.h"

BYTE	rstcp[] = {	   /* restore cursor position (0x206b if 16 bit out) */
0x1b, 'k', NULL
	};
>>>savecp.c
#include	"portab.h"

BYTE	savecp[] = {	      /* save cursor position (0x206a if 16 bit out) */
0x1b, 'j', NULL
	};
>>>scdldr.c
#include	"portab.h"

BYTE	scdldr[] = "CDOSLDR.SYS";	     /* name of CDOS Loader Sys file */
>>>semicol.c
#include	"portab.h"

BYTE	semicol = ';';
>>>sfile.c
#include	"portab.h"
#include	"concur.h"

BYTE	sfile[] = "CDOS.SYS";	       /* name of bootable system image file */
>>>sname.c
#include	"portab.h"
#include	"concur.h"

BYTE	sname[] = LN_SWIT;			    /* switch character name */
>>>stdename.c
#include	"portab.h"
#include	"concur.h"

BYTE	stdename[] = LN_STDE;					   /* stderr */
>>>stdiname.c
#include	"portab.h"
#include	"concur.h"

BYTE	stdiname[] = LN_STDI;					    /* stdin */
>>>stdmsgs.c
/*
	STDMSGS.C
	Version 1.3
	09/27/85

	These are the messages that comprise the CCMSGS.L86 library.
	They are maintained here in a single file in hopes that future
	message building can be performed using a "STRIP"-like utility.

*/

#include	"portab.h"
#include	"concur.h"
#include	"ccutls.h"

/* Messages for general usage */

						  /* useful character values */
BYTE	eschar = 0x1b;
BYTE	comma = ',';
BYTE	semicol = ';';
BYTE	equal = '=';
BYTE	neg = '-';		    /* used in numeric to string conversions */
BYTE	plus = '+';

						/* parsing chars and strings */
BYTE	carret = '\r';
BYTE	linefeed = '\n';
BYTE	eofchar = 0x1a;

BYTE	blank = ' ';
BYTE	tab = '\t';
BYTE	dir1delim = '/';
BYTE	dir2delim = '\\';
BYTE	dir3delim = '.';
BYTE	devdelim = ':';
BYTE	extdelim = '.';
BYTE	wildfield = '*';
BYTE	wildchar = '?';
BYTE	wildnot = '^';
BYTE	wildfile[] = "*.*";				/* wildcard filename */
BYTE	wilddev[] = "*:";			     /* wildcard device name */

BYTE	netnode[] = "::";
BYTE	dblslant[] = "//";
						/* 6 command line delimiters */
BYTE	dlims[] = { ' ', '\t', ',', ';', '=', '\0' };

						   /* useful utility strings */
BYTE	nullstr[] = "";					     /* blank string */
BYTE	crlfmsg[] = "\r\n";					   /* <CRLF> */
BYTE	nextline[] = "\r\n%5";				     /* <CRLF> (msg) */
BYTE	bell[] = { 0x0d, 0x0a, 0x07, '\0' };		      /* <CRLF> BELL */
BYTE	backspace[] = "\b \b";			    /* destructive backspace */
BYTE	yestxt[] = "Y";						      /* YES */
BYTE	notxt[] = "N";						       /* NO */

BYTE	savcurpos[] = {	      /* save cursor position (0x206a if 16 bit out) */
0x1b, 'j', NULL
	};

BYTE	rstcurpos[] = {	   /* restore cursor position (0x206b if 16 bit out) */
0x1b, 'k', NULL
	};

BYTE	erarstcp[] = {	/* erase beg.line, restore cp (0x206f,0x206b - 16bit)*/
0x1b, 'o', 0x1b, 'k', NULL
	};

							/* Conversion arrays */
BYTE	numconv[] = "0123456789ABCDEF";	   /* used for hex and decimal convs */
BYTE	aconv[] = { 'a', 'z', 0xe0, 'A', 'Z', 0x20 };	/* used in case xlat */

					     /* useful DEFINED logical names */
BYTE	stdiname[] = LN_STDI;					    /* stdin */
BYTE	stdoname[] = LN_STDO;					   /* stdout */
BYTE	stdename[] = LN_STDE;					   /* stderr */
BYTE	bootdev[] = LN_BOOT;				 /* boot device name */
BYTE	ifile[] = "CDOS.IMG";	   /* name of non-bootable system image file */
BYTE	sfile[] = "CDOS.SYS";	       /* name of bootable system image file */
BYTE	icdldr[] = "CDOSLDR.IMG";	   /* name of CDOS Loader Image file */
BYTE	scdldr[] = "CDOSLDR.SYS";	     /* name of CDOS Loader Sys file */
BYTE	sname[] = LN_SWIT;			    /* switch character name */
BYTE	defname[] = LN_DEF;				     /* default name */
BYTE	coname[] = LN_CNS;				     /* console name */
BYTE	helplvl[] = LN_HLP;			   /* helplevel logical name */

				  /* valid filename characters, count is 1st */
BYTE	valchar[] = {
	52,
	'A','B','C','D','E','F','G','H','I','J','K','L','M',
	'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
	'0','1','2','3','4','5','6','7','8','9',
	'!','#','$','%','&','\'','(',
	')','-','/','@','\\','_','`','{','}'
	};

/* cc029x[] messages are used by UTERRMSG and UTPRNMSG			     */

ERRSRC	cc0290[] = {				   /* DOS error source names */
	{ 0x00, 0x00, "KRNL/SUP" },
	{ 0x10, 0x10, "PIPEMAN"  },
	{ 0x20, 0x20, "DISKMAN"  },
	{ 0x21, 0x2f, "DISKDRVR" },
	{ 0x30, 0x30, "CONMAN"   },
	{ 0x31, 0x3f, "CONDRVR"  },
	{ 0x40, 0x40, "COM/LOAD" },
	{ 0x50, 0x50, "OEMMAN"   },
	{ 0x51, 0x5f, "OEMDRVR"  },
	{ 0x60, 0x60, "NETMAN"   },
	{ 0x61, 0x6f, "NETDRVR"  },
	{ 0x70, 0x70, "MISCMAN"  },
	{ 0x71, 0x7f, "MISCDRVR" },
	{ 0x81, 0xfe, "SPECDRVR" },
	{ 0x00, 0xff, "UNKNOWN"  }
	};
	

						       /* DOS function names */
BYTE	*cc0291[] = {
	"NONE",
	"F_GET",
	"F_SET",
	"F_LOOKUP",
	"F_CREATE",
	"F_DELETE",
	"F_OPEN",
	"F_CLOSE",
	"F_READ",
	"F_WRITE",
	"F_SPECIAL",
	"F_RENAME",
	"F_DEFINE",
	"F_DEVLOCK",
	"F_INSTALL",
	"F_LOCK",
	"F_COPY",
	"F_ALTER",
	"F_XLAT",
	"F_RWAIT",
	"F_KCTRL",
	"F_ORDER",
	"F_KEYPUT",
	"F_GIVE",
	"F_BWAIT",
	"F_TIMER",
	"F_EXIT",
	"F_ABORT",
	"F_CANCEL",
	"F_WAIT",
	"F_STATUS",
	"F_RETURN",
	"F_EXCEPTION",
	"F_ENABLE",
	"F_DISABLE",
	"F_SWIRET",
	"F_MALLOC",
	"F_MFREE",
	"F_OVERLAY",
	"F_COMMAND",
	"F_CONTROL",
	"F_GSX",
	"F_SEEK"
	};

BYTE	cc0292 = '%';			   /* print routine INSERT delimiter */

BYTE	cc0293[] =
	"\r\nReturn code: %9";

BYTE	cc0294[] =
	"\r\nError source: %9";

BYTE	cc0295[] =
	"\r\nCDOS function: %9";

/*

	Below are the standard messages used by Concurrent DOS utilities.
	These are messages that are used by two or more utilities. Any
	messages that are specific to a single utility are contained in
	a separate file. The messages come in two (2) basic forms. The 
	first form (declared as BYTE *cc#####[]) contains an array of
	messages. Each array element is a different version of the same
	message. Only one version will be printed according to the current
	HELPLVL setting. The second basic message form (declared as
	BYTE cc#####[]) is a single character string that does not have
	other HELPLVL versions. This type of message may be used as a
	standalone message, or more frequently as an %INSERT parameter
	for other messages. %INSERT parameters (denoted as %0 - %9) are used
	to piece strings together.  The %INSERT parameters used depend
	on the message being printed, but a general usage may be:

		%0 = module name
		%1 = device 1
		%2 = file 1
		%3 = device 2
		%4 = file 2
		%5 = variable (msg use)
		%6 = variable (msg use)
		%7 = variable
		%8 = variable
		%9 = variable

	Also note, that the %INSERT parameters shown are those that are
	intended for use under general circumstances, and are shown to
	provide an example of usage and to document general message
	constructions. Keep in mind however, that %INSERT parameters may be
	used by various utilities with substrings other than those noted here.

*/

							    /* blank message */
BYTE	*nullmsg[] = {			  /* used mainly for INTERNAL errors */
	"",
	"",
	""
	};


/* cc03xx messages are Concurrent DOS table names. (%4's for *cc0500)	     */

BYTE	cc0300[] = "Process";
BYTE	cc0301[] = "Process Environment";
BYTE	cc0302[] = "Time and Date";
BYTE	cc0303[] = "Memory Information";
BYTE	cc0304[] = "Pipe Information";
BYTE	cc0305[] = "Disk File Information";
BYTE	cc0306[] = "Disk Device Information";
BYTE	cc0307[] = "Console Information";
BYTE	cc0308[] = "Mouse Information";
BYTE	cc0309[] = "Window (vcon) Information";
BYTE	cc0310[] = "Physical Console Information";
BYTE	cc0311[] = "System Information";
BYTE	cc0312[] = "File Number Information";
BYTE	cc0313[] = "System Define";
BYTE	cc0314[] = "Process Define";
BYTE	cc0315[] = "Command Environment";
BYTE	cc0316[] = "Device Information";
BYTE	cc0317[] = "Path Compression";
BYTE	cc0318[] = "Printer Device";
BYTE	cc0319[] = "Port Device";
BYTE	cc0320[] = "Special Device";


/* cc04xx[] general purpose messages					     */

		     	  /* cc041x thru cc047x are general purpose messages */

					 /* *cc041x[] = Miscellaneous errors */

BYTE	*cc0410[] = {		    /* this may be due to unavailable memory */
"\r\n%0: Directory levels too deep.",
"\r\n%0: The directory levels are too deep for this operation.",
"%5%6"				   /* %5 = cc0410[1], %6 = cc0420 or nullstr */
	};

					    /* %5 = cc0411[1], %6 = cc0447[] */
BYTE	*cc0411[] = {
"\r\n%0: Not enough memory.",
"\r\n%0: There is not enough memory available to complete this operation.",
"%5\r\nOther programs may be using available memory.\
\r\nReenter the command when these programs are finished.%6"
	};

BYTE	*cc0412[] = {					   /* %5 = cc0412[1] */
"\r\n%0: Line too long.",
"\r\n%0: %0 found a line longer than 256 characters.",
"%5\r\nUse a text editor to split lines longer than 256 characters\
\r\ninto shorter ones."
	};

BYTE	*cc0413[] = {
"\r\n%0: Nonremovable media error.",
"\r\n%0: Both transfer devices are the same,\
\r\nand contain nonremovable media.",
""
	};

			     /* cc0413[] = Special message for E_POOL error. */
BYTE	cc0414[] =
"\r\n%0: Possible bad data on drive%1.\
\r\nRun CHKDSK on the disk and verify the validity of any\
\r\nrecently updated files.";

BYTE	*cc0415[] = {
"\r\n%0: User terminated.",
"\r\n%0: Process terminated by user abort (CTRL-C).",
""
	};

BYTE	*cc0416[] = {
"\r\n%0: Process terminated.",
"\r\n%0: Process terminated by external abort.",
""
	};


					   /* cc042x[] = inserts for *cc041x */
BYTE	cc0420[] =
"\r\nA directory path cannot have more than %7 levels.\
\r\nMake sure that you entered your directory path correctly.\
\r\nIf you entered it correctly, you may have to restructure your\
\r\ndirectories so that you have %7 levels or fewer.";

					     /* *cc043x[] = Parameter errors */
BYTE	*cc0430[] = {
	"\r\n%0: Invalid parameter.",
	"\r\n%0: You specified an invalid parameter.",
	"%5%6"				      /* %5 = cc0430[1], %6 = cc044x */
	};

BYTE	*cc0431[] = {
	"\r\n%0: Parameter error.",
	"\r\n%0: You specified an invalid number of parameters.",
	"%5%6"			     /* %5 = cc0431[1], %6 = cc0446 or other */
	};


			    /* cc044x[] = inserts for the *cc043x[] messages */

						  /* cc0440 is %6 for cc0430 */
BYTE	cc0440[] =
"\r\nYour %7 is too long.%8";	       /* %7 = cc045x, %8 = cc0441 or cc0442 */

				/* Below: %8 for cc0440; %7 = cc045x, %4 = # */
BYTE	cc0441[] =
"\r\nA %7 must be %4 characters or less.\
\r\nReenter your command line.";

			      /* Below: %8 for cc0440; %7 = cc0451 or cc0454 */
BYTE	cc0442[] =
"\r\nYour complete %7, from the root directory to the level you\
\r\nwant to access, must be 128 characters or less. If you use logical names\
\r\nin your %7, the original 'long form' must be\
\r\n128 characters or less.";

					/* Below: %6 for cc0430; %7 = cc045x */
BYTE	cc0443[] =
"\r\nConcurrent expected a %7. Reenter your command line.";

						     /* Below: %6 for cc0430 */
BYTE	cc0444[] =
"\r\nThe date is invalid. Make sure the values for month, day, and year are\
\r\nwithin the correct ranges and that you are using either hyphens (-)\
\r\nor slashes (/) for your separators.";

					/* Below: %6 for cc0430, %7 = ut0001 */
BYTE	cc0445[] =
"\r\nAn invalid option was used. Valid options are %7.";

				       /* Below: %6 for *cc0431; %7 = ut0002 */
BYTE	cc0446[] =
"\r\nThe most common form(s) of %0 are: %7";

		      /* cc0447[] = %6 (extra help information) for cc0411[] */
BYTE	cc0447[] =
"\r\nIf no other programs are running on your system, your computer's\
\r\nmemory is not large enough for this operation.";

						 /* cc0448[] = %x for ut0002 */
BYTE	cc0448[] =
"\r\nFor additional forms of %0, see the Concurrent DOS documentation.";

					 /* cc045x[] = parameter descriptors */
				   /* Below: cc0450 - 57 = %7's for cc044x[] */
BYTE	cc0450[] = "device name";
BYTE	cc0451[] = "path name";
BYTE	cc0452[] = "filename";
BYTE	cc0453[] = "file extension";
BYTE	cc0454[] = "file specification";
BYTE	cc0455[] = "disk device name";
BYTE	cc0456[] = "floppy disk device name";
BYTE	cc0457[] = "hard disk device name";
BYTE	cc0458[] = "directory name";
BYTE	cc0459[] = "command line";
BYTE	cc045A[] = "file name (filename.ext)";


					    /* cc046x/47x[] = %9 for *cc0500 */

/* cc0460[]: %1 = device, %4 = cc0490 or nullstr			     */
/*		--- Generated for the following errors ---		     */
/*	E_SPACE		on	S_CREATE				     */
BYTE	cc0460[] =
"\r\nThe directory of %1 is full.%4";

/* cc0461[]: %1 = device, %4 = cc0490 or nullstr			     */
/*		--- Generated for the following errors ---		     */
/*	E_SPACE		on	S_WRITE					     */
BYTE	cc0461[] =
"\r\nDisk %1 is full.%4";

/* cc0462[]: %1 = device, %2 = file, %4 = cc0491 or nullstr		     */
/*		--- Generated for the following errors ---		     */
/*	E_NO_FILE	on	S_OPEN or S_GET (Diskfile table)	     */
/*	0		on	S_LOOKUP (Diskfile table)		     */
BYTE	cc0462[] =
"\r\nThe file %2 was not found on %1.%4";

/* cc0463[]: %1 = device name, %4 = cc0492 or nullstr			     */
/*		--- Generated for the following errors ---		     */
/*	E_NO_FILE	on	S_OPEN or S_GET (Disk,Console,Mouse,	     */
/*				Vconsole,Pconsole,Printer, Port or	     */
/*				Special tables)				     */
/*	0		on	S_LOOKUP (Device or Vconsole table)	     */
BYTE	cc0463[] =
"\r\nThe device %1 was not found.%4";

/* cc0464[]:								     */
/*		--- Generated for the following errors ---		     */
/*	E_PATH		on	S_GET, S_SET or S_LOOKUP (Diskfile table) or */
/*				S_CREATE, S_DELETE, S_OPEN or S_RENAME	     */
BYTE	cc0464[] =
"\r\nThe directory path is invalid. Most likely you have entered a path that\
\r\ndoes not exist, a path name that is too long, or you did not use the\
\r\nproper separators in your path.";

/* cc0465[]:								     */
/*		--- Generated for the following errors ---		     */
/*	E_CONFLICT	on	S_CREATE, S_OPEN, S_READ or S_WRITE	     */
/*	E_DEVCONFLICT	on	S_OPEN, S_READ or S_WRITE		     */
/*	E_DEVLOCK	on	S_OPEN, S_READ or S_WRITE		     */
BYTE	cc0465[] =
"\r\nThe operation failed due to an access conflict.\
\r\nThe file or device you are trying to access may be in use or locked\
\r\nby another user. This is a temporary situation.";

/* cc0466[]:								     */
/*		--- Generated for the following errors ---		     */
/*	E_WPROT		on	S_CREATE or S_WRITE			     */
BYTE	cc0466[] =
"\r\nYour disk is write protected and you can not change \
the information on it.\
\r\nIf you want to change information on the disk, \
alter the write protection tab.";

/* cc0467[]:								     */
/*		--- Generated for the following errors ---		     */
/*	E_ACCESS	on	S_CREATE, S_SET, S_DELETE, S_OPEN,	     */
/*				S_READ or S_WRITE			     */
BYTE	cc0467[] =
"\r\nYou do not have sufficient privileges to perform this operation.\
\r\nEither you do not have the proper Owner, Group and World privileges,\
\r\nor you are attempting an operation reserved for system managers.";

/* cc0468[]: %4 = cc045x						     */
/*		--- Generated for the following errors ---		     */
/*	E_NAME		on	S_LOOKUP, S_CREATE, S_DELETE,		     */
/*				S_OPEN or S_RENAME			     */
BYTE	cc0468[] =
"\r\nYou specified an invalid %4. You either specified too\
\r\nmany characters or used invalid characters in your %4.";

/* cc0469[]: (SPECIAL !!!) %2 = directory path, %4 = cc0493 or nullstr	     */
/*		--- Generated for the following errors ---		     */
/*	E_NO_FILE	on	S_CREATE, S_DELETE, S_OPEN or S_RENAME       */
/*	0		on	S_LOOKUP (Diskfile table)		     */
BYTE	cc0469[] =
"\r\nThe directory was not found.%4";


/* cc0470[]: %2 = filename.ext, %4 = cc0491 or nullstr			     */
/*		--- Generated for the following errors ---		     */
/*	E_NO_FILE	on	S_OPEN or S_GET (Diskfile table)	     */
/*	0		on	S_LOOKUP (Diskfile table)		     */
BYTE	cc0470[] =
"\r\nThe file %2 was not found.%4";

/* cc0471[]:								     */
/*		--- Generated for the following errors ---		     */
/*	default	err	on	S_GET, S_SET or S_LOOKUP		     */
/*									     */
BYTE	cc0471[] =
"\r\nConcurrent DOS is presently unable to perform the %0 command.\
\r\nThis may be a temporary condition. Reenter the command.\
\r\nIf the error continues, refer to the section on error handling\
\r\nin the Concurrent DOS documentation.";

/* cc0472[]: 								     */
/*		--- Generated for the following errors ---		     */
/*	E_IMPLEMENT on any CDOS call.					     */
/*									     */
BYTE	cc0472[] =
"\r\nThis operation is not supported.";

/* cc0473[]: 								     */
/*		--- Generated for the following errors ---		     */
/*	E_FULL error on S_CREATE, S_OPEN, S_VCCREATE, etc.		     */
/*									     */
BYTE	cc0473[] =
"\r\nThe system file number table is full.";

/* cc0474[]: 								     */
/*		--- Generated for the following errors ---		     */
/*	E_MEDIACHANGE and E_MEDCHGERR for CDOS disk access functions.	     */
/*									     */
BYTE	cc0474[] =
"\r\nThe file system detected a media change.\
\r\nRetry the operation.";

/* cc0475[]: 								     */
/*		--- Generated for the following errors ---		     */
/*	E_READONLY on CDOS file access functions.			     */
/*									     */
BYTE	cc0475[] =
"\r\nThe file is marked READ ONLY.";

/* cc0476[]: 								     */
/*		--- Generated for the following errors ---		     */
/*	E_CORRUPT on CDOS file access functions.			     */
/*									     */
BYTE	cc0476[] =
"\r\nThe file allocation tables are corrupt.\
\r\nRun CHKDSK on the disk.";

/* cc0477[]: 								     */
/*		--- Generated for the following errors ---		     */
/*	E_RAWMEDIA on CDOS disk access functions.			     */
/*									     */
BYTE	cc0477[] =
"\r\nThe media type is unknown and is not supported by the file system.";

/* cc0478[]: 								     */
/*		--- Generated for the following errors ---		     */
/*	E_READY on CDOS device access functions.			     */
/*									     */
BYTE	cc0478[] =
"\r\nThe device is not ready.";

/* cc0479[]: 								     */
/*		--- Generated for the following errors ---		     */
/*	E_DKATTACH on CDOS device access functions.			     */
/*									     */
BYTE	cc0479[] =
"\r\nThe device did not respond.";


					       /* cc048x[] = general prompts */

					       /* %5 = cc0578/9, %6 = device */
BYTE	cc0480[] =
"\r\nInsert %5 diskette into drive %6.\
\r\nPress any key when you are ready. ";

						     /* %5 = max#, %6 = bell */
BYTE	cc0481[] =
"\r\nYou entered an invalid selection.\
\r\nValid selections are 1 to %5.\
%6Try again: ";


	      /* cc049x[] = %4 (extra help information) for *cc0500 (cc046x) */

							    /* %4 for cc0460 */
BYTE	cc0490[] =
"\r\nYou can erase unnecessary files from your disk to free up space.";

							    /* %4 for cc0462 */
BYTE	cc0491[] =
"\r\nMake sure that you specified the correct device, path, and file names.";

							    /* %4 for cc0463 */
BYTE	cc0492[] =
"\r\nMake sure that you specified the correct device name.";

							    /* %4 for cc0469 */
BYTE	cc0493[] =
"\r\nMake sure you specified the correct directory name.";



/*
   cc05xx series messages: These messages are intended to report most DOS
	related errors. This series may require use of the entire parmptr
	block. The basic message is cc0500. The following shows the use of
	the parmptr inserts:

	%0 = module name.
	%1 = device name.
	%2 = filename (if any).
	%3 = area name (if any), generally one of the cc057x series.
	%4 = may be used for extra help information.
	%5 = operation name, generally one of the cc051x/cc052x series.
	%6 = operation name, generally one of the cc053x/cc054x series.
	%7 = device/file/area string, generally of the cc055x series.
	%8 = level 3 help basic message, generally the cc0500[1] message.
	%9 = level 3 help message, generally one of the cc046x series.

	Example message for an S_WRITE that returns an E_SPACE error:

	%0 = "COPY"
	%1 = "a:"
	%2 = "testfile.txt"
	%3 = nullstr (not used).
	%4 = "\r\nYou can erase unneccessary files from your disk to
	      free up space." (cc0490).
	%5 = "WRITE" (cc0526).
	%6 = "WRITING" (cc0546).
	%7 = "%2 on %1" (cc0551).
	%8 = "\r\n%0: An error occurred %6 %7." (cc0500[1]).
	%9 = "\r\nThe disk %1 is full." (cc0461).

	HELPLVL 4:
	    COPY: An error occurred WRITING testfile.txt to a:.
	    The disk a: is full.
	    You can erase unneccessary files from your disk to free up space.

	HELPLVL 3:
	    COPY: An error occurred WRITING testfile.txt to a:.

	HELPLVL 2:
	    COPY: WRITE error.

	HELPLVL 1:
	    This level is reserved by the ERRMSG routine to provide
	    information such as the CDOS function, CDOS error code and
	    UTILITY error code. It is printed for all HELPLVLs and looks
	    like:

	CDOS function: F_WRITE  CDOS code: -37  COPY code: 9
*/
						     /* basic message format */
BYTE	*cc0500[] = {
	"\r\n%0: %5 error.",
	"\r\n%0: An error occurred %6 %7.",
	"%8%9"
	};

				  /* cc051x, 52x, 53x and 54x[] = operations */
					 /* cc051x and 52x[] = %5 operations */
BYTE	cc0510[] = "Allocate";		  /* F_MALLOC (special: %7 = cc0574) */
BYTE	cc0511[] = "Cancel";	      /* F_ABORT (special: %1 = processname) */
BYTE	cc0512[] = "Close";					  /* F_CLOSE */
BYTE	cc0513[] = "Create";					 /* F_CREATE */
BYTE	cc0514[] = "Define";					 /* F_DEFINE */
BYTE	cc0515[] = "Delete";					 /* F_DELETE */
BYTE	cc0516[] = "Format";			       /* F_SPECIAL (FMTTRK) */
BYTE	cc0517[] = "Get";				/* F_GET or F_LOOKUP */
BYTE	cc0518[] = "Install";					/* F_INSTALL */
BYTE	cc0519[] = "Lock";					   /* F_LOCK */
BYTE	cc0520[] = "Open";					   /* F_OPEN */
BYTE	cc0521[] = "Overlay";					/* F_OVERLAY */
BYTE	cc0522[] = "Read";					   /* F_READ */
BYTE	cc0523[] = "Rename";					 /* F_RENAME */
BYTE	cc0524[] = "Search";					 /* F_LOOKUP */
BYTE	cc0525[] = "Set";					    /* F_SET */
BYTE	cc0526[] = "Verify";					   /* F_READ */
BYTE	cc0527[] = "Write";					  /* F_WRITE */
BYTE	cc0528[] = "Deallocate";				  /* F_MFREE */
BYTE	cc0529[] = "Special";					/* F_SPECIAL */

					 /* cc053x and 54x[] = %6 operations */
BYTE	cc0530[] = "allocating";	  /* F_MALLOC (special: %7 = cc0574) */
BYTE	cc0531[] = "canceling";	      /* F_ABORT (special: %1 = processname) */
BYTE	cc0532[] = "closing";					  /* F_CLOSE */
BYTE	cc0533[] = "creating";					 /* F_CREATE */
BYTE	cc0534[] = "defining";					 /* F_DEFINE */
BYTE	cc0535[] = "deleting";					 /* F_DELETE */
BYTE	cc0536[] = "formatting";		       /* F_SPECIAL (FMTTRK) */
BYTE	cc0537[] = "getting";				/* F_GET or F_LOOKUP */
BYTE	cc0538[] = "installing";				/* F_INSTALL */
BYTE	cc0539[] = "locking";					   /* F_LOCK */
BYTE	cc0540[] = "opening";					   /* F_OPEN */
BYTE	cc0541[] = "overlaying";				/* F_OVERLAY */
BYTE	cc0542[] = "reading";					   /* F_READ */
BYTE	cc0543[] = "renaming";					 /* F_RENAME */
BYTE	cc0544[] = "searching";					 /* F_LOOKUP */
BYTE	cc0545[] = "setting";					    /* F_SET */
BYTE	cc0546[] = "verifying";					   /* F_READ */
BYTE	cc0547[] = "writing";					  /* F_WRITE */
BYTE	cc0548[] = "deallocating";	   /* F_MFREE (special: %7 = cc0574) */
BYTE	cc0549[] = "logging";				 /* F_SPECIAL(MDCHK) */

			     /* cc055x/56x[]= basic message modifiers (%7's) */
BYTE	cc0550[] = "%1";				     /* device error */
BYTE	cc0551[] = "%2 on %1";			     /* file on device error */
BYTE	cc0552[] = "for %1";				     /* device error */
BYTE	cc0553[] = "for %2 on %1";		     /* file on device error */
BYTE	cc0554[] = "to %1";				     /* device error */
BYTE	cc0555[] = "to %2 on %1";		     /* file on device error */
BYTE	cc0556[] = "from %1";				     /* device error */
BYTE	cc0557[] = "from %2 on %1";		     /* file on device error */
BYTE	cc0558[] = "in the %3 area";			       /* area error */
BYTE	cc0559[] = "in the %3";				       /* area error */
BYTE	cc0560[] = "the %4 table";		       /* table access error */
BYTE	cc0561[] = "the %3";				  /* S_SPECIAL error */

					      /* cc057x/58x[] = areas (%3's) */
BYTE	cc0570[] = "boot record";
BYTE	cc0571[] = "directory";
BYTE	cc0572[] = "disk";
BYTE	cc0573[] = "file allocation table";
BYTE	cc0574[] = "memory";
BYTE	cc0575[] = "operating system file";
BYTE	cc0576[] = "sysarea";
BYTE	cc0577[] = "system";
BYTE	cc0578[] = "source";
BYTE	cc0579[] = "destination";
BYTE	cc0580[] = "format information";
BYTE	cc0581[] = "master boot partition record";

/* Notes on F_SPECIAL errors: */
/* All use *cc0500 as the message base. */
/* All use cc0529 for %5 */
/* F_SPECIAL(INITF): %3 = cc0580, %6 = cc0545, & %7 = cc0550 */
/* F_SPECIAL(RDSYS): %3 = cc0576, %6 = cc0542, & %7 = cc0561 */
/* F_SPECIAL(WTSYS): %3 = cc0576, %6 = cc0547, & %7 = cc0561 */
/* F_SPECIAL(FMSYS): %3 = cc0576, %6 = cc0536, & %7 = cc0561 */
/* F_SPECIAL(FMTRK): %3 = cc0572, %6 = cc0536, & %7 = cc0561 */
/* F_SPECIAL(MDCHK): %3 = cc0572, %6 = cc0549, & %7 = cc0559 */


/* *cc06xx[] = messages common to FORMAT, FDISK and DISKCOPY		     */

BYTE	*cc0600[] = {
"\r\n%0: Sector size error.",				   /* %5 = cc0600[1] */
"\r\n%0: The sector size specified for this format is invalid.",
"%5\r\nThe sector size must be 32, 128, 256, 512, 1024, 2048 or 4096 bytes."
	};

BYTE	*cc0601[] = {
"\r\n%0: Sectors per track error.",			   /* %5 = cc0601[1] */
"\r\n%0: The number of sectors per track specified for this format is invalid.",
"%5\r\nThe number of sectors per track must be between 1 and 256."
	};

BYTE	*cc0602[] = {
"\r\n%0: The computer manufacturer does not offer this device format.",
"",
""
	};


/* cc070x[] = messages common to BACKUP and RESTORE			     */

BYTE	cc0700[] =
"BACKUPID.@@@";

BYTE	cc0701[] =
"@00";
					   /* %6 below = nullstr or bkmsg030 */
BYTE	cc0703[] =
"\r\nInsert the backup diskette #%5 in drive %3.%6";

BYTE	cc0704[] =
"\r\nPress any key when you are ready ... %5";			/* %5 = bell */
				       /* %6 for cc0431, %7 below = cc0578/9 */
BYTE	cc0705[] =
"\r\n%0 expects the %7 device to be a hard disk.";  /* use cc430/443/457 !!! */

BYTE	*cc0706[] = {
"\r\n%0: No files found.",
"\r\n%0: Warning! No files were found to %0.",
"\r\n%0: The system could not %0 the file(s) you specified.\
\r\nYou may have told %0 to look in the wrong place, or specified\
\r\na file that does not exist. Make sure that you specified the correct\
\r\n device, path, and file names."
	};


/* cc08xx[] =  messages common to FORMAT, SYS and UTXFRSYS()		     */

BYTE	cc0800[] =
"\r\nSystem transferred.";

BYTE	cc0801[] =
"\r\nTransferring %2.";

BYTE	cc0802[] =
"\r\nCDOS loader installed.";


BYTE	*cc0810[] = {
"\r\n%0: Concurrent DOS loader format error.",
"\r\n%0: The internal format of the Concurrent DOS loader file (%1)\
\r\nis incorrect.",
""
	};

BYTE	*cc0811[] = {
"\r\n%0: Space for CDOS loader is occupied.",
"\r\n%0: The Concurrent DOS loader must reside at the very start of the\
\r\nfile area on drive %1.",
"%5\r\nPlease delete any unneccessary files and try again."/* %5 = cc0811[1] */
	};

BYTE	*cc0812[] = {
"\r\n%0: Insufficient disk space.",
"\r\n%0: There is not enough room on %1 to hold %2.",
"%5%6"					      /* %5 = cc0812[1], %6 = cc0490 */
	};

BYTE	*cc0813[] = {					   /* %5 = cc0813[1] */
"\r\n%0: Non-bootable disk.",
"\r\n%0: The destination disk (%1) is not formatted to receive\
\r\noperating system files.",
""
	};


/* cc09xx[] = messages common to CANCEL and PROCESS			     */

BYTE	cc0900[] =					   /* %7 for *cc0500 */
"the requested process";

BYTE	cc0910[] =					   /* %9 for *cc0500 */
"\r\nThe process does not exist.";

/**/
>>>stdoname.c
#include	"portab.h"
#include	"concur.h"

BYTE	stdoname[] = LN_STDO;					   /* stdout */
>>>tab.c
#include	"portab.h"

BYTE	tab = '\t';
>>>valchar.c
#include	"portab.h"
				  /* valid filename characters, count is 1st */
BYTE	valchar[] = {
	52,
	'A','B','C','D','E','F','G','H','I','J','K','L','M',
	'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
	'0','1','2','3','4','5','6','7','8','9',
	'!','#','$','%','&','\'','(',
	')','-','/','@','\\','_','`','{','}'
	};
>>>wildchar.c
#include	"portab.h"

BYTE	wildchar = '?';
>>>wilddev.c
#include	"portab.h"

BYTE	wilddev[] = "*:";			     /* wildcard device name */
>>>wildfile.c
#include	"portab.h"

BYTE	wildfile[] = "*.*";				/* wildcard filename */
>>>wildnot.c
#include	"portab.h"

BYTE	wildnot = '^';
>>>wldfield.c
#include	"portab.h"

BYTE	wildfield = '*';
>>>yntxt.c
#include	"portab.h"

BYTE	yestxt[] = "Y";						      /* YES */
BYTE	notxt[] = "N";						       /* NO */
 '*';
>>>yntxt.