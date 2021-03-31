Reading restmsg.c
>>>restmsg.c
/*****************************************************************
 * "Copyright (C) 1985, Digital Research, Inc.  All Rights       *
 * Reserved.  The Software Code contained in this listing is     *
 * proprietary to Digital Research Inc., Monterey, California    *
 * and is covered by U.S. and other copyright protection.        *
 * Unauthorized copying, adaptation, distribution, use or        *
 * display is prohibited and may be subject to civil and         *
 * criminal penalties.  Disclosure to others is prohibited.  For *
 * the terms and conditions of software code use refer to the    *
 * appropriate Digital Research License Agreement."              *
 *****************************************************************/
/*===============================================================*
 *   Version 1.1        RESTMSG.C				 *
 *			Message module for RESTORE utility.	 *
 *---------------------------------------------------------------*
 *     VERSION   DATE    BY     CHANGE/COMMENTS                  *
 *---------------------------------------------------------------*
 *	1.1	01/12/85 MA	Added this intro header.	 *
 *				Took out #include "concur.h".	 *
 *      1.0     ?/?/??   bw                                      *
 *===============================================================*/


#include	"portab.h"

/* Messages for RESTORE.C */

BYTE	rsmodule[] = "RESTORE";
BYTE	rsm03[] = "\n\r*** Restoring files from diskette % ***";
BYTE	rsm04[] = "\n\r*** Files were backed up % ***\n\r";
BYTE	rsm06[] = "\n\rWarning! Diskette is out of sequence";
BYTE	rsm07[] = "\n\rReplace the diskette or continue";
BYTE	rsm08[] = "\n\rRestore file sequence error";
BYTE	rsm10[] = "\n\r\n\rWarning! File % \n\ris a read only file ";
BYTE	rsm11[] = "\n\rReplace the file (Y/N)? ";
BYTE	rsm12[] = "\n\r\n\rWarning! File % \n\rwas changed after it was \
backed up";
BYTE	rse01[] = "\n\rP - Prompts you for files have changes since the \
last back-up or \n\r    files marked read-only. \
\n\rS - Restores the current directory as well as all the subdirectories";
BYTE	rse02[] = "\n\rThe first paremeter has to be a device name only";

Reading dirmsg.c
>>>dirmsg.c

/* ------------------------- dirmsg.c ------------------------------ */
/* modified: 6/3/85					             */
/* 1.02  20-FEB-86 BRM "\n\r" -> "\r\n"                              */
/* ----------------------------------------------------------------- */
#include "portab.h"

BYTE	dirmodule[] = "DIR";
BYTE	dirm01[] = "\r\n  Volume in drive % has no label ";
BYTE	dirm02[] = "\r\n  Volume in drive % is %";
BYTE	dirm03[] = "\r\n  Directory of %%\r\n";
BYTE	dirm04[] = "<DIR>   ";
BYTE	dirm05[] = "  %-%-%  %:%%\r\n"; /* time date stamp */
BYTE	dirm07[] = "Press any key when you are ready ... ";
BYTE	dirm08[] = "\r\n      % File% bytes free"; /* last line */
BYTE	dirm09[] = "\r\n      % Files% bytes free";
BYTE	dirm10[] = "output device";

BYTE    dire01[] = "\r\nH - Display all hidden and system files.\
\r\nP - Display the directory one screenful at a time.\
\r\nW - Produce a wide display";
BYTE	dire02[] = "\r\nUnknown disk media";


Reading passmsgs.c
>>>passmsgs.c
/*===============================================================*
 *   Version 1.0	passmsgs.c				 *
 *	This file contains the messages displayed by the	 *
 *	PASSWORD command.					 *
 *---------------------------------------------------------------*
 *    VERSION   DATE    BY      CHANGE/COMMENTS                  *
 *---------------------------------------------------------------*
 *	1.0	01/26/84 KSO -	Created				 *
 *	FCS1							 *
 *								 *
 *===============================================================*
 *  INCLUDES:                                                    */
#include	"portab.h"

BYTE	passfile[] = "system:user.tab";

BYTE	pas0000[] = "PASSWORD";

BYTE	pas0001[] = "the %1 file";

BYTE	pas0002[] = "Error opening the %1 file";

BYTE	pas0003[] =
"\r\nThe %1 file was not found.\
\r\nCheck with the system manager.";

BYTE	pas0004[] =
"\r\nThe %1 file could not be opend due to an access conflict.\
\r\nTry again in a few minutes or check with the system manager.";

BYTE	pas0005[] =
"\r\nYou do not have sufficient privileges to perform the operation.\
\r\nCheck with your system manager.";

BYTE	pas0006[] = "Could not set the password";

BYTE	pas0007[] =
"\r\nYou do not have sufficient privileges to perform the operation.\
\r\nOnly a system manager can set someone else's password.";

BYTE	pas0008[] = "Invalid user name";

BYTE	pas0009[] =
"\r\nThe user name %1 is not in the user file.\
\r\nMake sure the name is correct.";

BYTE	pas0010[] = "Invalid password";

BYTE	pas0011[] =
"\r\nThe old password was not correct.\
\r\nMake sure you typed the old password correctly.";

BYTE	pas0012[] = "New password confirmation failed";

BYTE	pas0013[] =
"\r\nThe new password was not confirmed.\
\r\nYou must give the same new password twice.";

BYTE	pas0014[] = "%1's password";

BYTE	*pas0020[] =
{
	"\r\n%0: %5.",
	"\r\n%0: An error occured %6 %7.",
	"\r\n%8 %9"
};
Reading logmsgs.c
>>>logmsgs.c
/*****************************************************************
 * "Copyright (C) 1985, Digital Research, Inc.  All Rights       *
 * Reserved.  The Software Code contained in this listing is     *
 * proprietary to Digital Research Inc., Monterey, California    *
 * and is covered by U.S. and other copyright protection.        *
 * Unauthorized copying, adaptation, distribution, use or        *
 * display is prohibited and may be subject to civil and         *
 * criminal penalties.  Disclosure to others is prohibited.  For *
 * the terms and conditions of software code use refer to the    *
 * appropriate Digital Research License Agreement."              *
 *****************************************************************/

/*===============================================================*
 *   Version 1.9	logmsgs.c				 *
 *	This file contains the messages displayed by the	 *
 *	LOGON command.						 *
 *---------------------------------------------------------------*
 *    VERSION   DATE    BY      CHANGE/COMMENTS                  *
 *---------------------------------------------------------------*
 *	1.0	01/26/84 KSO	Created				 *
 *	BETA							 *
 *	1.1	06/13/85 KSO	Added error message for error on *
 *				opening the console		 *
 *	1.2	06/30/85 KSO	Replace namerr and passerr with	 *
 *				accesserr			 *
 *	1.3	07/01/85 KSO	Revised file not found error	 *
 *				message				 *
 *	1.4	07/17/85 IAW	Added network messages		 *
 *      1.5     09/17/85 tdm    Added badnode message            *
 *      1.6     09/23/85 tdm    Modified startmsg.		 *
 *      1.6     09/24/85 tdm    Capitalized dos in startmsg.     *
 *      1.7     10/14/85 tdm    Added additional net error msgs. * 
 *	1.8	01/09/86 ma	Added press any key message.	 *
 *      1.9     03/04/86 tdm    Added log0021 message.		 *
 *								 *
 *===============================================================*
 *  INCLUDES:                                                    */
#include	"portab.h"

BYTE    digit[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E',
		   'F'};
BYTE	userfile[] = "system:user.tab";
BYTE	filemsg[] = "\r\nPut the system disk into Drive A: and press any key.";
BYTE	pressmsg[] = "\r\nPress any key to try again.";
BYTE	startmsg[] = "\
\r\n***********************************\
\r\n*                                 *\
\r\n*  Welcome to Concurrent DOS 68K  *\
\r\n*                                 *\
\r\n***********************************\r\n\n";
BYTE    remuser[] = "  user:  ";
BYTE	accesserr[] = "\r\nInvalid user access.";
BYTE	locdenied[] = "\r\nThis user is not permitted to logon to this node.";
BYTE	remdenied[] = "\r\nThis user is not permitted to logon to node: ";
BYTE	remok[] = "\r\nSuccessfully logged on to node: ";
BYTE    remerror[] = "\n\rError logging on to node:  ";
BYTE    remnonode[] = "This node does not exist.\n\r";
BYTE    remconfail[] = "Cannot connect to this node.\n\r";
BYTE    remlogfail[] = "Unable to log on to remote node.\n\r";
BYTE    remdialect[] = "Remote node does not support dialect supported by\
 local node.\n\r";
BYTE    remloggedon[] = "You are already logged on to this node.\n\r";
BYTE    remlocname[] = "You must set your local node name before attempting a\
 remote logon.\n\r";
BYTE    remlogrefused[] = "Remote logon refused.\n\r";
BYTE    remlogperm[] = "You are not permitted to logon to this node.\n\r";
BYTE    remllimit[] = "Log on limit exceeded for this process.\n\r";
BYTE	remnofile[] = "Unable to open \"system:user.tab\" on remote node.\n\r";
BYTE	remfpro[] = "Error while processing \"system:user.tab\" remote node.\n\r";
BYTE	undef[]	= "The error code returned is:  ";
BYTE	nonet[] = "Network software must be installed before attempting remote logons.\n\r";
BYTE    badnode[] = "\n\rInvalid node name specified.\n\r";


BYTE	log0000[] = "LOGON";
BYTE	log0001[] = "Memory allocation";
BYTE	log0002[] = "allocating memory for a read buffer";
BYTE	log0003[] =
"\r\nThe system is overloaded.  Try again in a few minutes";
BYTE	log0004[] = "\r\nCheck with your system manager.";
BYTE	log0005[] = "File open";
BYTE	log0006[] = "opening the %1 file";
BYTE	log0007[] = "\r\nThe %1 file was not found on the system drive";
BYTE	log0008[] = "File access";
BYTE	log0009[] = "\r\nThe %1 file could not be opened for read access";
BYTE	log0010[] = "\r\nThe %1 file could not be opened due to a media error";
BYTE	log0011[] = "File read";
BYTE	log0012[] = "reading the %1 file";
BYTE	log0013[] = "\r\nThe %1 file could not be read.";
BYTE	log0014[] = "Load";
BYTE	log0015[] = "loading the %1 file";
BYTE	log0016[] = "\r\nThe load file format for the %1 command was invalid.";
BYTE	log0017[] = "\r\nThe file type of %1 is not executable.";
BYTE	log0018[] = "\r\nThere was not enough memory to load the %1 command.";
BYTE	log0019[] = "\r\nThe %1 file could not be accessed.";
BYTE	log0020[] = "LOGON: Could not open the console.\r\n";
BYTE    log0021[] = "/\\:";

Reading ckdskmsg.c
>>>ckdskmsg.c
/*
	CKDSKMSG.C
	Version 1.0
	06/10/85
*/

#include	"portab.h"
#include	"concur.h"

/* Messages for CHKDSK */

BYTE	cdm0000[] =
"CHKDSK";						      /* module name */

BYTE	cdm0001[] =
"FV";							  /* option switches */

BYTE	cdm0002[] =
"\r\n%0 [disk:][filespec] -[option]%8";				   /* syntax */

BYTE	cdm0005[] =
"FILE0000CHK";

BYTE	cdm0010[] =
"\r\n\t%5 bytes total disk space.";

BYTE	cdm0011[] =
"\r\n\t%5 bytes in %6 hidden files.";

BYTE	cdm0012[] =
"\r\n\t%5 bytes in %6 directories.";

BYTE	cdm0013[] =
"\r\n\t%5 bytes in %6 user files.";

BYTE	cdm0014[] =
"\r\n\t%5 bytes available on disk.";

BYTE	cdm0015[] =
"\r\n\t%5 bytes total memory.";

BYTE	cdm0016[] =
"\r\n\t%5 bytes free memory.";

BYTE	cdm0017[] =
"\r\n\t%5 bytes would be in %6 recovered files.";

BYTE	cdm0018[] =
"\r\n\t%5 bytes disk space would be freed.";

BYTE	cdm0019[] =
"\r\n\t%5 bytes in %6 recovered files.";

BYTE	cdm0020[] =
"\r\n\t%5 bytes in %6 bad clusters.";

BYTE	cdm0021[] =
"\r\n\t%5 bytes in %6 cross linked clusters.";

BYTE	cdm0022[] =
"\r\n\t%5 bytes system memory.";

BYTE	cdm0023[] =
"\r\n\t%5 bytes allocated memory.";

BYTE	cdm0024[] =
"\r\n\r\nDISK:";

BYTE	cdm0025[] =
"\r\nMEMORY:";

BYTE	cdm0026[] =
"\r\n\t%5 bytes in boot area.";

BYTE	cdm0027[] =
"\r\n\t%5 bytes in %6 FATs.";

BYTE	cdm0028[] =
"\r\n\t%5 bytes (%6 entries) in root directory.";

BYTE	cdm0029[] =
"\r\n\t%5 bytes total file space.";


BYTE	cdm0030[] =
"\r\n\tContains %6 non-contiguous blocks.";

BYTE	cdm0031[] =
"\r\n\tAll specified files are contiguous.";


BYTE	cdm0040[] =
"\r\n%5 lost clusters found in %6 chains.";

BYTE	cdm0041[] =
"\r\nConvert lost chains to files (Y / N) ?";

BYTE	cdm0042[] =
"\r\n\r\nLost chains found.";


BYTE	cdm0050[] =
"\r\n\tFAT chain contains an invalid cluster number.";

BYTE	cdm0051[] =
"\r\n\t%5 size adjusted to length of %6 bytes.";

BYTE	cdm0052[] =
"File";

BYTE	cdm0053[] =
"Directory";

BYTE	cdm0054[] =
"\r\n\tDirectory converted into file.";

BYTE	cdm0055[] =
"\r\n\tAllocation error, length is incorrect.";


BYTE	cdm0060[] =
"\r\nWarning ! An error occurred %6 FAT image #%4.";

BYTE	cdm0061[] =
"\r\nAll FAT images are invalid.";


BYTE	cdm0070[] =
"\r\n\tVolume %5    Created %6\r\n";

BYTE	cdm0071[] =
"NONE";

BYTE	cdm0072[] =
", ";

BYTE	cdm0073[] =
"   ";

BYTE	cdm0074[] =
":";

BYTE	cdm0075[] =
" ";


BYTE	cdm0080[] =
"\r\nDirectory %5";

BYTE	cdm0081[] =
"\r\n    %5";


BYTE	cdm0090[] =
"\r\nScanning directory for cross linked chains.";

BYTE	cdm0091[] =
"\r\n\tCross linked on cluster %7.";

BYTE	cdm0092[] =
"\r\nScanning FAT for lost chains.";

BYTE	cdm0093[] =
"\r\nCreating %5";


BYTE	cdm0100[] =
".";							 /* current dir name */

BYTE	cdm0101[] =
"..";							  /* parent dir name */

BYTE	*cdm0110[] = {
"January",
"February",
"March",
"April",
"May",
"June",
"July",
"August",
"September",
"October",
"November",
"December"
};

BYTE	cdm0120[] =
"\r\nNOTE: -F option was not specified.\
\r\nIf errors are encountered, corrections will not be written to disk.\r\n";

BYTE	cdm0130[] =
"\r\nRoot directory full.\
\r\nCannot create any more lost chain files.";


BYTE	*cdm0200[] = {
"\r\n%0: Unknown media type.",
"\r\n%0: The media type to be checked is unknown.",
"%5\r\nThe media may be unformatted or of a type unusable by Concurrent."
};

BYTE	*cdm0210[] = {
"\r\n%0: File allocation table too large.",
"\r\n%0: The size of the file allocation table exceeds the maximum\
\r\n%0 can handle.",
""
};

BYTE	*cdm0220[] = {
"\r\n%0: Cannot create a unique filename.",
"\r\n%0: A unique filename cannot be created.",
"%5\r\nYou must erase some FILE####.CHK files before running %0 again."
};

/* */
Reading copymsgs.c
>>>copymsgs.c
/*
	COPYMSGS.C
	Version 1.0
	07/31/85
*/

#include	"portab.h"
/*#include	"concur.h"*/

/* Messages for COPY.C */


BYTE	cp0000[] =
"COPY";							      /* module name */

BYTE	cp0001[] =
"ABSV";							  /* option switches */

BYTE	cp0002[] =
"\r\n%0 source destination%8";				      /* %8 = cc0448 */


BYTE	*cp0010[] = {					   /* %5 = cp0010[1] */
"\r\n%0: Too many combine files.",
"\r\n%0: The command line specified more than 10 files to be combined.",
"%5\r\nYou can only combine 10 or fewer files at a time."
	};

BYTE	*cp0011[] = {					   /* %5 = cp0011[1] */
"\r\n%0: Source and destinations are the same.",
"\r\n%0: The source and destination file names, paths or devices\
\r\nmust be different.",
"%5 %0 your file to a different location,\
\r\nor give your copy a different name."
	};

BYTE	*cp0012[] = {					   /* %5 = cp0012[1] */
"\r\n%0: Incorrect use of wildcards.",
"\r\n%0: You cannot use the caret (^) wildcard with the %0 command.",
"%5\r\nYou can only use the star (*) and the question mark (?) wildcards.\
\r\nSpecify the files you want to %0, and not the files you\
\r\ndon't want to %0."
	};

BYTE	*cp0013[] = {					   /* %5 = cp0013[1] */
"\r\n%0: Incorrect use of wildcards.",
"\r\n%0: Full Concurrent wildcarding is not supported.",
"%5\r\nMake sure that you enter valid PCDOS - CP/M wildcards."
	};

BYTE	*cp0014[] = {
"\r\n%0: Incorrect use of wildcards.",
"\r\n%0: The names '%6' and '%7'\r\nuse incompatible wildcard specifications.",
""
	};

BYTE	*cp0015[] = {
"\r\n%0: Incorrect use of wildcards.",
"\r\n%0: You cannot use wildcards in the device name.",
""
	};

BYTE	*cp0016[] = {					   /* %5 = cp0016[1] */
	"\r\n%0: Record size mismatch.",
	"\r\n%0: The record sizes of the source files do not match.",
	"%5\r\nYou cannot combine files with different record sizes."
	};

BYTE	*cp0017[] = {
	"\r\n%0: No sources found.",
	"\r\n%0: No source specified was found to exist.",
	"%5%6"				    /* %5 = cp0017[1], %6 = cc0491[] */
	};

BYTE	*cp0018[] = {
	"\r\n%0: Buffer too small.",
	"\r\n%0: A single record will not fit in the available buffer space.",
	""
	};

BYTE	*cp0019[] = {
	"\r\n%0: An error occurred renaming the temporary file.",
	"%5\r\n%6 could not be renamed to %7.",
	""
	};


BYTE	cp0020[] =
"\r\n\r\nCopying -";

BYTE	cp0021[] =
"\r\n\r\nCombining -";

BYTE	cp0022[] =
"\r\n\r\n%5 %6 copied.";

BYTE	cp0023[] =
"\r\n\r\n%5 %6 combined.";

BYTE	cp0024[] =
"file";

BYTE	cp0025[] =
"files";

BYTE	cp0026[] =
"%7%8";					/* for disk prompting, %7 = cc0578/9 */

BYTE	cp0027[] =
" (%9)";				     /* %8 for cp0026, %9 = filename */


BYTE	cp0030[] =
"%C";							   /* temp file name */

BYTE	cp0031[] =
".TMP";

/* */
Reading procmsgs.c
>>>procmsgs.c
/*
	PROCMSGS.C
	Version 1.0
	08/14/85
*/

/* Message module for PROCESS.C */

#include	"portab.h"
#include	"concur.h"


BYTE	prc000[] =					      /* module name */
"PROCESS";

BYTE	prc001[] =					  /* option switches */
"EFMPU";				       /* Also used as %7 for cc0445 */
							  /* command strings */
BYTE	prc002[] =
"HELP";

BYTE	prc003[] =
"VIEW";

BYTE	prc004[] =
"CANCEL";

BYTE	prc005[] =
"ID";


BYTE	*prc010[] = {					 /* syntax help info */
"\r\n\r\n%0\r\n%0 VIEW\r\n%0 VIEW - option\r\n%0 CANCEL ID=##########",
"\r\n%0",
"\tProvides this help information on how to use %0.",
""						   /* null string terminator */
	};

BYTE	*prc011[] = {
"\r\n%0 VIEW -E -F -M -P -U",
"\tProvides a report of processes currently running. Information",
"\tincludes Process Name, Process ID, Window Number, Screen Number",
"\tFamily, Group and User IDs, and the State of the process.",
"\t\t-E - provides Extra information including the Parent Process",
"\t\t     ID, Process Priority and Event Flags.",
"\t\t-F - displays only those processes that match the user's",
"\t\t     Family ID. Otherwise, all processes on the (local)",
"\t\t     system are displayed.",
"\t\t-M - provides memory statistics such as Code Start and Size,",
"\t\t     Data Start and Size, Heap Start and Size, and whether",
"\t\t     the memory is locked or can be swapped.",
"\t\t-P - displays output one page at a time.",
"\t\t-U - displays only those processes that match the user's",
"\t\t     Group and User IDs. Otherwise, all processes on the",
"\t\t     (local) system are displayed.",
""						   /* null string terminator */
	};

BYTE	*prc012[] = {
"\r\n%0 CANCEL ID = ##########",
"\tCancel a process by specifying its Process ID number.",
"\tThe Process ID can be up to 10 decimal digits long.\r\n",
""						   /* null string terminator */
	};

						  /* standard report headers */
BYTE	prc020[] =
"name: ";

BYTE	prc021[] =
"  pid : ";

BYTE	prc022[] =
"  wndw: ";

BYTE	prc023[] =
"  pcon: ";

BYTE	prc024[] =
"fmly: ";

BYTE	prc025[] =
"  grp : ";

BYTE	prc026[] =
"  user: ";

BYTE	prc027[] =
"  stat: ";

							/* /E report headers */
BYTE	prc030[] =
"prty: ";

BYTE	prc031[] =
"  ppid: ";

BYTE	prc032[] =
"  (system process) ";

BYTE	prc033[] =
"  (user process)   ";

BYTE	prc034[] =
"  (swi context)    ";

BYTE	prc035[] =
"  (main context)   ";

BYTE	prc036[] =
"evnt:";

BYTE	prc037[] =
"      (system manager)";

							/* /M report headers */
BYTE	prc040[] =
"maxm: ";

BYTE	prc041[] =
"  code: ";

BYTE	prc042[] =
"  data: ";

BYTE	prc043[] =
"  heap: ";

BYTE	prc044[] =
"  size: ";

BYTE	prc045[] =
"(locked)         ";

BYTE	prc046[] =
"(swap)           ";

							   /* process states */
BYTE	*prc050[] = {
"running    ",
"waiting    ",
"terminating",
"shared code",
"unknown    "
	};

							  /* status messages */
BYTE	prc060[] =
"Process deleted.\r\n";

BYTE	prc061[] =
"Process not deleted.\r\n";

BYTE	prc062[] =
"No processes were found to VIEW.\r\n";

BYTE	prc063[] =
"\r\n%0 %1 MODE\r\n\r\n";

BYTE	prc064[] =
"Process has been notified to terminate.\r\n";

						      /* continuation prompt */
BYTE	prc070[] =
"Press any key to continue: ";

BYTE	prc071[] =
"H";

							    /* miscellaneous */
BYTE	prc081[] =
"*";					      /* wildname for process search */

BYTE	prc082[] =
" *";

BYTE	prc083[] =
"----------------------------------------\
----------------------------------------";

BYTE	prc084[] =					    /* %7 for cc0443 */
"decimal number";

BYTE	prc085[] =					    /* %7 for cc0443 */
"maximum of 10 decimal digits";

BYTE	prc086[] =					    /* %7 for cc0443 */
"valid switch character (%8)";

/**/
Reading sortmsgs.c
>>>sortmsgs.c
/*
	SORTMSGS.C
	Version 1.1
	07/23/85
*/

#include	"portab.h"
#include	"concur.h"

/* Messages for SORT.C */

BYTE	sort00[] =
"SORT";							      /* module name */
BYTE	sort01[] =
"+R";							  /* option switches */
BYTE	sort02[] =
"\r\n%0 < filespec\
\r\n%0 -options < filespec\
\r\nCOMMAND | %0\
\r\nCOMMAND | %0 -options";

BYTE	sort10   = '?';				  /* command line format req */

BYTE	*sort20[] = {
"\r\n%0: Too many lines.",
"\r\n%0: %0 can only handle a maximum of %6 lines of text.",
""
	};

BYTE	*sort21[] = {
"\r\n%0: File too large.",
"\r\n%0: %0 can only handle files up to %6 bytes long.",
""
	};

/* */
Reading renmsg.c
>>>renmsg.c
/* ------------------------- renmsg.c ------------------------------- */
/* created : 12/12/84						       */
/* ------------------------------------------------------------------- */
#include "portab.h"

BYTE	renmodule[] = "RENAME";
BYTE	renm01[] = "\r\nRENAME expected two file specifications. Reenter \
your command line.";
BYTE	renm03[] = "%1 to %2";



Reading ffmsgs.c
>>>ffmsgs.c
/*  ffmsgs.c - ffpifile,ffunkopt,ffsyserr,ffhelp,fffnf,ffhdr,ffftr  */

/****************************************************************************

				   ffmsgs.c

			  text and message routines 
			    for file fix utility

****************************************************************************/

#include	"portab.h"


#define	DCRLF	msgdcrlf
#define	CRLF	msgcrlf 
#define	CR	msgcr 
#define	PUTS(x)	printf("%s",x)


/*
**  externs
*/

EXTERN BYTE	*pgmname ;


/*
**  versno -
**	version number
*/

GLOBAL BYTE	*versno = "01.08" ;

/*
**  global characters used  in command line parsing
*/

GLOBAL BYTE	dot = '.' ;
GLOBAL BYTE	slash = '/' ;
GLOBAL BYTE	space = ' ' ;
GLOBAL BYTE	null = '\0' ;

#ifndef	wildfield
GLOBAL BYTE	wildfield = '*' ;
#endif
#ifndef	wildchar
GLOBAL BYTE	wildchar = '?' ;
#endif
#ifndef	wildnot
GLOBAL BYTE	wildnot = '^' ;
#endif

/*
**  optstr - options string
*/

GLOBAL	BYTE	*optstr = "hHvVtTdDfFiI" ;


/*
**  General Messages
*/

GLOBAL BYTE *msgnull =	"" ;
GLOBAL BYTE *msgcr = 	"\r" ;
GLOBAL BYTE *msgcrlf =	"\r\n" ;
GLOBAL BYTE *msgdcrlf =	"\r\n\r\n" ;
GLOBAL BYTE *msgvers =	"Recover File Utility Version " ;
GLOBAL BYTE *msgsyserr= "System Error: " ;

/*
**  Messages used by ffdir module
*/

GLOBAL BYTE *msg001 = 	"Search Path: " ;
GLOBAL BYTE *msg002 =	"Path Left: " ;
GLOBAL BYTE *msg003a =	"Checking Dir Entry ";
GLOBAL BYTE *msg003b =	"(dec)" ;

GLOBAL BYTE *msg040 =	"File Found In Entry " ;


/*
**  Error Messages used by fferr module
*/

GLOBAL BYTE *em0003a =	"Unknown Option At \\" ;
GLOBAL BYTE *em0003b =	"\\ " ;
GLOBAL BYTE *em0004 =	"Delete Option Requires a Destination File" ;
GLOBAL BYTE *em0005 =	"You Must Specify an Input File" ;
GLOBAL BYTE *em0006 =	"The Disk Containing " ;
GLOBAL BYTE *em0006a =	"Is of Unknown Format" ;
GLOBAL BYTE *em0007 =	"There Is NOT ENOUGH MEMORY to Run" ;
GLOBAL BYTE *em0008 =	"FILE NOT FOUND " ;
GLOBAL BYTE *em0008a =	"\tLogical File Name:\t" ;
GLOBAL BYTE *em0008b =	"\tEvaluated as:\t\t" ;
GLOBAL BYTE *em0009 =	"CDOS Error Code " ;
GLOBAL BYTE *em0011a=	"Is a Root Directory -- " ;
GLOBAL BYTE *em0011b =	"Is a SubDirectory -- " ;
GLOBAL BYTE *em0011c =	"Can\'t Fix Sub/Directories" ;
GLOBAL BYTE *em0011d =	"Is a Volume Label -- Can't Fix Labels" ;
GLOBAL BYTE *em0011e =	"Is Read Only" ;
GLOBAL BYTE *em0011f =	"Is a Hidden File -- Can't Fix Hidden Files" ;
GLOBAL BYTE *em0011g =	"Is a System File -- Can't Fix System Files" ;
GLOBAL BYTE *em0012 =	"Getopt Returned an Unknown Index" ;
GLOBAL BYTE *em0013 =	"Get Table Returned a Value Greater Than 1" ;
GLOBAL BYTE *em0014 =	"Error returned from MFREE" ;
GLOBAL BYTE *em0015 =	"Path Table Returned Path without Device Name" ;
GLOBAL BYTE *em0016 =	"File Lookup Returns More Than One Table" ;
GLOBAL BYTE *em0017 =	"File Open Returns 0" ;
GLOBAL BYTE *em0018 =	"File Table Returns Value Greater Than 1" ;
GLOBAL BYTE *em0019 =	"Bad Return From Disk I/O Request" ;
GLOBAL BYTE *em0020 =	"Not Enough Fat Buffer Space" ;
GLOBAL BYTE *em0021 =	"No Room in Fat For New Clusters" ;
GLOBAL BYTE *em0025a =	"WRITE ERROR ON MAIN FAT" ;
GLOBAL BYTE *em0025b =	"Write Error on Auxilary Fat Number " ;
GLOBAL BYTE *em0026 = 	"Read Error on Main Fat" ;
GLOBAL BYTE *em0027 = 	"Write Error on Directory" ;
GLOBAL BYTE *em0028 =	"Read Error on SubDirectory" ;
GLOBAL BYTE *em0029 =	"No Files Were Found Matching the Path " ;
GLOBAL BYTE *em0030a =	"The file " ;
GLOBAL BYTE *em0030b =	" Was Not Fixed" ;

/*
**  messages used by ffinit
*/

GLOBAL BYTE	*msg00b =	"bytes" ;
GLOBAL BYTE	*msg006 =	"Minimum Required Buffer Space: " ;
GLOBAL BYTE	*msg007 =	"Buffer Space Allocated: " ;
GLOBAL BYTE	*msg008 =	"Cluster Buffer Size: " ;
GLOBAL BYTE	*msg009 =	"SubDir Buffer Size: " ;
GLOBAL BYTE	*msg010 =	"Root Buffer Size: " ;
GLOBAL BYTE	*msg011 =	"Fat Buffer Size: " ;

/*
**  messages used by main module
*/

GLOBAL BYTE	*msg012 =	"Fix Switch " ;
GLOBAL BYTE	*msg012a =	"disabled" ;
GLOBAL BYTE	*msg012b =	"enabled" ;
GLOBAL BYTE	*msg012c =	"Debug Switch On" ;
GLOBAL BYTE	*msg013 =	"Fat Trace: " ;
GLOBAL BYTE	*msg014 =	"**** READ ERROR " ;
GLOBAL BYTE	*msg014a =	"On Cluster " ;
GLOBAL BYTE	*msg014b =	"\tCluster Will Be Moved To Cluster " ;
GLOBAL BYTE	*msg015a =	"**** WRITE ERROR " ;
GLOBAL BYTE	*msg015b =	"On New Cluster" ;
GLOBAL BYTE	*msg016 =	"**** CLUSTER WRITTEN SUCCESSFULLY" ;
GLOBAL BYTE	*msg017a =	" Has " ;
GLOBAL BYTE	*msg017b =	" Clusters and " ;
GLOBAL BYTE	*msg017c =	" Extents. " ;
GLOBAL BYTE	*msg019a =	"\tRead of Sector " ;
GLOBAL BYTE	*msg019b =	" Within That Cluster Failed With Error Code " ;
GLOBAL BYTE	*msg020 =	"Number Of Clusters With Read Errors:\t" ;
GLOBAL BYTE	*msg021 =	"Number Of Clusters Successfully Moved:\t" ;

GLOBAL BYTE	*msg030 =	"Attempting to Fix File " ;

/*
**  Help Messages
*/


#define	MCRLF	"\r\n"

GLOBAL BYTE *msghelp[] = 
{
	"Usage Is: ",
	MCRLF, 		/* CRLF, */
	"\trecfile",		/*  pgmname, */
	" [options] filename [filename...]",
	MCRLF,		/*  CRLF, */
	"Where Valid Options Begin With the Switch Character Followed By:",
	MCRLF,		/*  CRLF, */
	"\th (or H) - Display This Message",
	MCRLF,		/*  CRLF ,  */
	"\tv (or V) - Display Current Version",
	MCRLF,		/*  CRLF ,  */
	"\tf( or F) - Actually Fix the File",
	MCRLF,		/*  CRLF ,  */
	"\tt( or T) - Do Not Display the Fat Trace of Clusters Examined",
	MCRLF,		/*  CRLF ,  */
} ;

#define	NBRHELP	(  sizeof(msghelp) / sizeof(BYTE *)  )

GLOBAL int   nohelpmsgs = NBRHELP ;


/*****************************
**  ffehelp -
**	print the help message
*/

VOID	ffehelp()
{
	int	i ;

	for( i = 0 ; i < nohelpmsgs ; ++i ) 
	{
		PUTS( msghelp[i] ) ;
	}
}	

/******************************
**  ffsyserr -
**	print system error message
*/

VOID	ffsyserr(s)
	char	*s ;
{
	printf("%s%s: %s%s\t%s%s",
		CRLF, pgmname, msgsyserr, CRLF, s, DCRLF) ;
}




/***************************************************************************
**  ffunkopt -
**	unknown option error
*/

VOID	ffunkopt( opt )
	char	*opt ;
{
	ffpgmname() ;
	printf("%s%s%s%s" , em0003a , opt , em0003b , CRLF) ;
}

/***************************************************************************
**  ffhdr -
*/

VOID	ffhdr()
{
	printf("%s%s---------------%s%s---------------%s%s",
		DCRLF,DCRLF,msgvers,versno,DCRLF,DCRLF) ;
}

/***************************************************************************
**  ffftr -
*/

VOID	ffftr(cnt)
	int	cnt ;			/*  nbr of files fixed		*/
{
	printf("%s",DCRLF) ;

	if( cnt )
	  printf("Total Number of Files Fixed: %d%s",cnt,CRLF) ;
	else
	  printf("***** NO FILES WERE FIXED *****%s",CRLF) ;
}

Reading compmsg.c
>>>compmsg.c
/* -------------------------- compmsg.c ------------------------------- */
/* ------------------------------------------------------------------- */
#include "portab.h"

BYTE	copmodule[] = "COMPARE";
BYTE	copm01[] = "\n\r\n\r%  and  %";
BYTE	copm02[] = "\n\r\n\rCompare more files (Y/N)? ";
BYTE	copm03[] = "\n\rEnter first filespec \n\r";
BYTE	copm04[] = "\n\r\n\rEnter second filespec or drive name \n\r";
BYTE	copm05[] = "\n\rFiles compare ok";
BYTE	copm06[] = "\n\r\n\r% Mismatches - ending compare";
BYTE	copm07[] = "\n\r\n\rCompare error at offset %H (decimal %) ";
BYTE	copm08[] = "\n\rFiles are different sizes.";
BYTE	copm09[] = "\n\rFile 1 = %H\n\rFile 2 = %H";
BYTE	copm10[] = "\n\rYou can only specify 2 filespecs with COMP.";
BYTE	copm12[] = "\n\r% - File not found";





Reading datemsg.c
>>>datemsg.c
/* -------------------------- datemsg.c ------------------------------- */
/* ------------------------------------------------------------------- */
#include "portab.h"

BYTE	dtmodule[] = "DATE";
BYTE	datm01[] = "\n\rCurrent date is ";
BYTE	datm02[] = "\n\rEnter new date : ";
BYTE	datm03[] = "\n\rD - Display (but do not change) the date";
BYTE	dtsun[] = "Sun ";
BYTE	dtmon[] = "Mon ";
BYTE	dttue[] = "Tue ";
BYTE	dtwed[] = "Wed ";
BYTE	dtthu[] = "Thu ";
BYTE	dtfri[] = "Fri ";
BYTE	dtsat[] = "Sat ";

Reading brcommsg.c
>>>brcommsg.c

/*****************************************************************
 * "Copyright (C) 1985, Digital Research, Inc.  All Rights       *
 * Reserved.  The Software Code contained in this listing is     *
 * proprietary to Digital Research Inc., Monterey, California    *
 * and is covered by U.S. and other copyright protection.        *
 * Unauthorized copying, adaptation, distribution, use or        *
 * display is prohibited and may be subject to civil and         *
 * criminal penalties.  Disclosure to others is prohibited.  For *
 * the terms and conditions of software code use refer to the    *
 * appropriate Digital Research License Agreement."              *
 *****************************************************************/

/*===============================================================*
 *   Version 1.2        BRCOMMSG.C                               *
 *---------------------------------------------------------------*
 *     VERSION  DATE	 BY      CHANGE/COMMENTS                 *
 *---------------------------------------------------------------*
 *	1.2	3/05/86  mei    Message adjustment.		 *
 *  	1.1 	2/21/86  mei	More messages for error handling,*
 *				added header.			 *
 *  	1.0 	7/7/85   bw 		Initial version          *
 *===============================================================*/

/* Message module for BACKUP utility. */

#include	"portab.h"
#include	"concur.h"

/* Messages for BACKUP.C */

BYTE	brm01[] = "BACKUP";
BYTE	brm02[] = "RESTORE";
BYTE	brm03[] = "\n\rInsert backup diskette #% in drive %";
BYTE	brm04[] = "\n\rPress any key when you are ready ...\n\r";

BYTE	bre01[] = "\n\r%0 expected two drive specifications.";
BYTE	bre02[] = "\n\rInvalid file specification.";
BYTE	bre03[] = "\n\rSource and target drives are the same.";
BYTE	bre04[] = "\r\nDiskette in drive %3 is not a backup diskette.";
BYTE	bre05[] = "\n\rUnknown disk media in drive %.\n\r";
BYTE	bre06[] = "\n\rDrive % is empty. Please insert a disk.";
Reading findmsg.c
>>>findmsg.c
/* -------------------------- findmsg.c ------------------------------ */
/* 4/16/85							       */
/* ------------------------------------------------------------------- */
#include "portab.h"

BYTE	finmodule[] = "FIND";
BYTE	finm01[] = "\n\r\n\r----------- %";
BYTE	finm02[] = ": %";
BYTE	finm03[] = "\n\r[%]";
BYTE	finm04[] = "C - Count the number of matches.  \
\n\rN - display the line number of each match.  V - Display lines that \
\n\rdo not include a match.";
BYTE	finm05[] = "FIND \"string\" filespec. %9";
BYTE	finm06[] = "\n\rYou cannot use wildcards in the filespec.";
Reading fsetmsg.c
>>>fsetmsg.c
/* -------------------------- fsetmsg.c ------------------------------ */
/* 4/16/85							       */
/* ------------------------------------------------------------------- */
#include "portab.h"

BYTE	fsemodule[] = "FSET";
BYTE	fsedlims[] = { ' ', '\t', ',', ';', '\0' };
BYTE	fsem01[] = "                        ";
BYTE	fsem04[] = "H - Hidden, A - Archive, S - System, R - Read-only, \
\n\rO - Owner, G - Group, W - World.";
BYTE	fsem06[] = "FSET filespec  or  FSET filespec -options. %9";


Reading moremsg.c
>>>moremsg.c
/* ------------------------- moremsg.c ------------------------------- */
/* created : 11/26/84						       */
/* ------------------------------------------------------------------- */
#include "portab.h"

BYTE	momodule[] = "MORE";
BYTE	morm03[] = "\n\r-- More --\n\r";

Reading volmsg.c
>>>volmsg.c
/* --------------------------- volmsg.c ---------------------------- */
/* created: 11/26/84						     */
/* ----------------------------------------------------------------- */
#include "portab.h"

BYTE	volmodule[] = "VOL";
BYTE	volm01[]  = "\n\rVolume in drive % is %";
BYTE	volm02[]  = "\n\rVolume in drive % has no label";
Reading timemsg.c
>>>timemsg.c
/* ---------------------------- timemsg.c ----------------------------- */
/* modified: 4/16/85                                                    */
/* -------------------------------------------------------------------- */
#include "portab.h"

BYTE	timodule[] = "TIME";
BYTE	timm01[] = "\n\rCurrent time is ";
BYTE	timm02[] = "\n\rEnter new time : ";
BYTE	timm03[] = 
"\r\nThe time is invalid.  Make sure the values for hour, minute, and\
\r\nsecond are in the correct ranges, and that you are using a\
\r\ncolon (:) for your separator.";
BYTE	timm04[] = "D - Display, but do not change, the time.";



Reading treemsg.c
>>>treemsg.c
/* -------------------------- treemsg.c ------------------------------- */
/* 4/16/85								*/
/* ------------------------------------------------------------------- */
#include "portab.h"

BYTE	tremodule[] = "TREE";
BYTE	trem01[] = "\n\rDirectory Path Listing.  ";
BYTE	trem02[] = "\n\r\n\r\n\rPath: %";
BYTE	trem03[] = "\n\r\n\rSubdirectories: %";
BYTE	trem04[] = "\n\r                %";
BYTE	trem05[] = "\n\r\n\rSubdirectories: None";
BYTE	trem06[] = "\n\r\n\rFiles         : %";
BYTE	trem07[] = "\n\r              : %";
BYTE	trem08[] = "\n\r\n\rFiles         : None";
BYTE	trem09[] = "F - List the files in each subdirectory.";
BYTE	trem10[] = "\n\r\n\rThere are no files on drive %.";
BYTE	trem11[] = "\n\r\n\rThere are no subdirectories on drive %."; 
BYTE	trem12[] = "The volume has no label";
BYTE	trem13[] = "The volume is %";

Reading typemsg.c
>>>typemsg.c
/* -------------------------- typemsg.c ------------------------------- */
/* 4/16/85							       */
/* ------------------------------------------------------------------- */
#include "portab.h"

BYTE	tymodule[] = "TYPE";
BYTE	typm03[] = "\n\rYou can only use one filespec with TYPE.\n\rReenter \
your command line.";
BYTE	typm04[] = "\n\rYou cannot use wildcards with TYPE.";


Reading vermsg.c
>>>vermsg.c

/* ------------------------- vermsg.c ------------------------------ */
/* created: 11/27/84						     */
/* modified: 1/15/85						     */
/* ----------------------------------------------------------------- */
#include "portab.h"

BYTE	vermodule[] 	= "VER";
BYTE	verm01[] 	= "Concurrent DOS-";
BYTE	verm02[]	= "\n\rUnknown operating system type";
BYTE	verm03[]	= "\n\r%  %% Version %";
/* BYTE	verm04[]	= "\n\r%% Version %"; */
BYTE	verm05[]	= "80"; 	/* 1 */
BYTE	verm06[]	= "85";		/* 2 */
BYTE	verm07[]	= "Z80";	/* 3 */
BYTE	verm08[]	= "86";		/* 4 */
BYTE	verm09[]	= "Z8000";	/* 5 */
BYTE	verm10[]	= "68000";	/* 6 */
BYTE	verm11[]	= "68010";	/* 7 */
BYTE	verm12[]	= "68020";	/* 8 */
BYTE	verm13[]	= "286";	/* 9 */
BYTE	verm14[]	= "386";	/* 10 */
BYTE	verm15[]	= "186";	/* 11 */

/* -------------------- end of vermsg.c ----------------------------- */

Reading dsetmsg.c
>>>dsetmsg.c


/* ------------------------- dsetmsg.c ------------------------------ */
/* created: 1/31/85					             */
/*  1.1 13-FEB-86  BRM  Added messages for -M option.                */
/* ----------------------------------------------------------------- */
#include "portab.h"

BYTE	dsemodule[] = "DISKSET";

BYTE	dsem01[] = "\n\rFile protection : ON"; /* on or off */
BYTE	dsem02[] = "\n\rFile protection : OFF";
BYTE	dsem03[] = "\n\rVolume label    : None";
BYTE	dsem04[] = "\n\rVolume label    : %";
BYTE	dsem05[] = "\n\rThe valid file protection options are ON and OFF.";
BYTE	dsem06[] = "\n\rTo set file protection, you must first write a \
volume label on \n\ryour disk with the 'L' option.";
BYTE	dsem08[] = "\n\rThe volume label must be 11 characters or less.";
BYTE	dsem09[] = " -P -L -M :\r\n\r\nP - Turn protection ON or OFF.  \
\r\nL - Write a volume label on the disk.\
\r\nM - Set mixed upper/lower case mode ON or OFF";

BYTE    dsem10[] = "\n\rThe valid mode options are ON and OFF.";
BYTE    dsem11[] = "\n\rTo set the volume's mode, you must first write a \
volume label on \n\ryour disk with the 'L' option.";

BYTE    dsem12[] = "\n\rMixed mode (upper and lower case) in use on volume.";
BYTE    dsem13[] = "\n\rUpper case only mode in use on volume.";
  
Reading bkupmsg.c
>>>bkupmsg.c
/*****************************************************************
 * "Copyright (C) 1985, Digital Research, Inc.  All Rights       *
 * Reserved.  The Software Code contained in this listing is     *
 * proprietary to Digital Research Inc., Monterey, California    *
 * and is covered by U.S. and other copyright protection.        *
 * Unauthorized copying, adaptation, distribution, use or        *
 * display is prohibited and may be subject to civil and         *
 * criminal penalties.  Disclosure to others is prohibited.  For *
 * the terms and conditions of software code use refer to the    *
 * appropriate Digital Research License Agreement."              *
 *****************************************************************/
/*===============================================================*
 *   Version 1.2        BKUPMSG.C				 *
 *                      Message module for BACKUP utility	 *
 *---------------------------------------------------------------*
 *     VERSION   DATE    BY     CHANGE/COMMENTS                  *
 *---------------------------------------------------------------*
 *      1.2	02/14/86 mei	added 2 msgs for disk write error*
 *				recovery			 *
 *	1.1	01/12/85 MA	Added this intro header.	 *
 *				Took out #include "concur.h".	 *
 *      1.0     ?/?/??   bw                                      *
 *===============================================================*/

#include	"portab.h"

/* Messages for BACKUP.C */

BYTE	bkmodule[] = "\n\rBACKUP";	
BYTE	bkm02[] = "\r\nWarning! All files on diskette #% will be erased.";
BYTE	bkm04[] = "\n\r*** Backing up files to diskette % ***";
BYTE	bke01[] = "\n\rThe second parameter has to be a device name only.";

BYTE	*bke03[] = {
"\r\n%0: Insufficient disk space.",
"\n\r%0: There is no room on %1.",
""
};

BYTE	bke06[] = "\n\rA - Adds files to the backup disk.\
\n\rM - Only backs up files modified since the last backup.\
\n\rS - Backs up the current directory as well as all the subdirectories.\
\n\rD:mm/dd/yy - Only backs up files modified on or after the specified date.";

BYTE	bke07[] = "\n\rReplace bad disk #% in drive %.";
BYTE	bke08[] = "\n\rDiskette #% will be rewritten.\n\r";
