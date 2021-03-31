Reading a:breakmsg.c
>>>a:breakmsg.c
/* -------------------------- breakmsg.c ------------------------------ */
#include "portab.h"

BYTE	brkmodule[] = "BREAK";

BYTE	brkm01[] = "\n\rBREAK is ON\n\r";
BYTE	brkm02[] = "\n\rBREAK is OFF\n\r";
BYTE	brkm03[] = "\n\rThe valid BREAK options are ON and OFF.";
Reading a:chdirmsg.c
>>>a:chdirmsg.c
/* -------------------------- chdirmsg.c ------------------------------- */
/* 1/3/85								*/
/* ------------------------------------------------------------------- */
#include "portab.h"

BYTE	chdmodule[] = "CHDIR";

BYTE	chdm01[] = "\n\r%1 is not a directory name. CHDIR can only be used \
with directories.";
BYTE	chdm02[] = "\n\r%0 can not redefine a physical device %1";




Reading a:defmsg.c
>>>a:defmsg.c
/* ------------------------- defmsg.c ------------------------------ */
/* modified: 4/16/85					             */
/* ----------------------------------------------------------------- */
#include "portab.h"

BYTE	defmodule[] = "DEFINE";

BYTE	defm01[] = "\n\rProcess level logical names: \n\r";
BYTE	defm02[] = "\n\rSystem level logical names: \n\r";
BYTE	defm03[] = "\n\r% = %"; /* LNAME = PNAME */
BYTE	defe01[] = "\n\rDEFINE logical_name=value. %9";
BYTE	defe02[] = "\n\rD - Delete a logical name.  \n\rS - System level.\
\n\rN - Display logical name(s). ";
BYTE	defe03[] = "\n\rOnly logical name should be specified with DELETE option.";
BYTE	defe04[] = "\n\rDELETE option ('D') and LIST option ('N') are conflict \
to each other.";
BYTE	defe05[] = "\n\rConcurrent DOS expected a logical name.  Reenter your\
command line.";
BYTE	defe06[] = "\n\rThe logical name should be specified before '='";
BYTE	defe07[] = "\n\rSyntax error.";
BYTE	defe08[] = "\n\rLogical name should not be more than one word";
BYTE	defe09[] = "\n\rThere is no logical name at the process level.";
BYTE	defe10[] = "\n\rThere is no logical name at the system level.";
BYTE	defe11[] = "logical names";

Reading a:vfymsg.c
>>>a:vfymsg.c
/* -------------------------- vfymsg.c ------------------------------- */
/* ------------------------------------------------------------------- */
#include "portab.h"

BYTE	vfymodule[] = "VERIFY";
BYTE	vfym01[] = "\n\rVerify is ON";
BYTE	vfym02[] = "\n\rVerify is OFF";
BYTE	vfym03[] = "\n\rThe valid verify options are ON and OFF.";
Reading a:eramsg.c
>>>a:eramsg.c
/* -------------------------- eramsg.c ------------------------------- */
/* 4/16/85							       */
/* ------------------------------------------------------------------- */
#include "portab.h"

/* ------------------ error messages and screen prompt ------------------- */
BYTE	eramodule[] = "ERASE";
BYTE	eram01[] = "\n\rAre you sure (Y/N)? ";
BYTE	eram02[] = "\n\r%, delete (Y/N)? ";
BYTE	eram03[] = "Q - Query whether or not to erase the file.";

Reading a:cancemsg.c
>>>a:cancemsg.c
/* -------------------------- cancemsg.c ----------------------------- */
/* 4/16/85							       */
/* ------------------------------------------------------------------- */
#include "portab.h"

BYTE	canmodule[] = "CANCEL";

BYTE	canm01[] = "F - Cancel the process using the Family ID";
BYTE	canm02[] = "Process ID or a Process Name";
BYTE	canm03[] = "\n\rThe Family ID must be a decimal number up to 5 digits\
long.";
BYTE	canm04[] = "\n\rProcess %1 was not found. \n\rMake sure that you \
specified the correct Process Name and/or ID numbers.";
BYTE	canm05[] = "0123456789";
BYTE	canm06[] = "Family ID %1";
BYTE	canm07[] = "\n\rCANCEL could not find a process whose Family ID is %.";
BYTE	canm08[] = "%1 on Window %2";

Reading a:mkdirmsg.c
>>>a:mkdirmsg.c
/* ------------------------- mdmsg.c ------------------------------- */
/* created : 11/26/84						       */
/* ------------------------------------------------------------------- */
#include "portab.h"

BYTE	mdmodule[] = "MKDIR";



Reading a:rmdirmsg.c
>>>a:rmdirmsg.c

/* ------------------------- rmdirmsg.c ------------------------------- */
/* modified: 4/16/85						       */
/* ------------------------------------------------------------------- */
#include "portab.h"

BYTE	rdmodule[] = "RMDIR";
BYTE	rde01[] = "\n\rYou can not delete the default directory.";



Reading a:securmsg.c
>>>a:securmsg.c
/*
	SECURMSG.C
	Version 1.0
	06/07/85
*/

#include	"portab.h"
#include	"concur.h"

/* Messages for SECURITY */

BYTE	secur00[] =
"SECURITY";						      /* module name */

BYTE	secur01[] =
"WGO";							  /* option switches */

BYTE	secur02[] =
"RWED";							 /* option arguments */

BYTE	secur10[] =
"\r\nCurrent security: ";				    /* report header */

BYTE	secur11[] =
"No privileges.";				     /* in case SECURITY = 0 */

BYTE	secur12[] =
", ";

BYTE	secur13[] =
"switch character (%8)";

BYTE	secur14[] =
"\bn equal sign (=)";

BYTE	secur15[] =
"\r\nAn invalid option arguement was used.\
\r\nValid option arguments are %7.";

/* */
Reading a:shmsgs.c
>>>a:shmsgs.c

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
 *   Version 1.8	shmsgs.c				 *
 *	This file contains the shell specifice error messages	 *
 *	and all of the characters and strings that shell uses	 *
 *---------------------------------------------------------------*
 *    VERSION   DATE    BY      CHANGE/COMMENTS                  *
 *---------------------------------------------------------------*
 *	1.0	01/26/84 KSO -	Created				 *
 *	1.1	04/11/85 KSO -	Added message for help level 1	 *
 *				error handling for default	 *
 *				drive change			 *
 *	1.1	04/15/85 KSO -	Convert verbs to lower case	 *
 *	BETA							 *
 *	1.2	05/17/85 KSO -	Cleaned up messages		 *
 *	1.2	05/21/85 KSO -	Added exception messages	 *
 *	1.3	06/10/85 KSO -	Fixed spelling error for E_PRIV	 *
 *	1.4	06/17/85 KSO -	Added error message for E_LSTACK *
 *	1.4	06/18/85 KSO -	Added error message for ENOTDISK *
 *	1.5	07/02/85 KSO -	Added error message for ENOMEM	 *
 *			 KSO -	Added error message for internal *
 *				error				 *
 *	1.5	07/10/85 KSO -	Add global character variables	 *
 *	FCS1							 *
 *	1.6	09/16/85 JSR -	Added and removed some messages	 *
 *      1.7     09/20/85 tdm -  Added some logoff net error msgs *
 *      1.8     10/25/85 tdm -  Added lof0011, lof0012, lof0013, *
 *				& lof0014 msgs.   		 *
 *								 *
 *===============================================================*
 *  INCLUDES:                                                    */
#include	"portab.h"

	/* Characters used by the shell		*/

BYTE	shexclam	= '!';
BYTE	shlbracket	= '[';
BYTE	shrbracket	= ']';
BYTE	shlparen	= '(';
BYTE	shrparen	= ')';
BYTE	shperiod	= '.';
BYTE	shtilda		= '~';
BYTE	shdquote	= '"';
BYTE	shorbar		= '|';
BYTE	shless		= '<';
BYTE	shgreater	= '>';
BYTE	shsemicolon	= ';';
BYTE	shcolon		= ':';
BYTE	shctrlr		= 0x12;
BYTE	shctrlc		= 0x03;
BYTE	shbs		= '\b';
BYTE	shpercent	= '%';
BYTE	shdollar	= '$';
BYTE	shdefswchar	= '-';

BYTE	shbatlbl	= ':';
BYTE	shsemark	= '*';
					/* valid terminate batch replies */
BYTE	shtba		= 'A';
BYTE	shtbn		= 'N';
BYTE	shtby		= 'Y';

	/* Strings used by the shell		*/

BYTE	shpname[]	= "pi:%p";
BYTE	shprompt[]	= "prompt";
BYTE	shorder[]	= "order";
BYTE	shpath[]	= "path";
BYTE	shstdcmd[]	= "stdcmd";
BYTE	shstartup[]	= "home:autoexec.bat";
BYTE	shshell[]	= "shell";
BYTE	shparm0[]	= "%0";
BYTE	shcmd[]		= "command";
BYTE	shtail[]	= "tail";
BYTE	shexcess[]	= "excess";
BYTE	shcount[]	= "count";
BYTE	shunknown[]	= "???";
BYTE	shdri[]		= "DRI";
BYTE	shversion[]	= "version";
BYTE	shver1[]	= " Concurrent DOS ";
BYTE	shver2[]	= " Ver ";
BYTE	shnodrive[]	= "?:";
BYTE	shdefpmt[]	= "$n$g";
BYTE	shclrline[]	= "\033K";

BYTE	shback[]	= "BACK";
BYTE	shbat[]		= "BAT";
BYTE	shbatch[]	= "BATCH";
BYTE	shexit[]	= "EXIT";
BYTE	shlogoff[]	= "LOGOFF";
BYTE	shrem[]		= "REM";
BYTE	shshift[]	= "SHIFT";

BYTE	shnot[]		= "NOT";
BYTE	sherrlvl[]	= "ERRORLEVEL";
BYTE	shexist[]	= "EXIST";

BYTE	shpidmsg[]	= "\r\nProcess id = ";
BYTE	shpidm2[]	= " started ";

BYTE	shtermbat[]	= "\r\nTerminate batch (Y/N/A) ? ";

BYTE	shdpmt1[]	= "\r\nPlease put the disk containing ";
BYTE	shdpmt2[]	= " into drive ";
BYTE	shdpmt3[]	= "\r\nPress any key to continue ... ";

BYTE	shbatrderr[]	= "\r\nSHELL: Batch read error: ";

BYTE	*shcputype[]	=
{
			"80",
			"85",
			"Z80",
			"86",
			"Z8000",
			"68000",
			"68010",
			"68020",
			"286",
			"386",
			"186"
};
	/* NOTE: MAXCPU is defined in SHELL.H */

BYTE	*shweekday[]	=
{
			"Sun",
			"Mon",
			"Tue",
			"Wed",
			"Thu",
			"Fri",
			"Sat"
};
	/* NOTE: MAXDAY is defined in SHELL.H */			


				       /* Redirection characters	     */
				       /* for SHELL.C ptail() case statement */
				       /* Do not rearrange !!!		     */
BYTE	shptail[]	=
{
			3,		/* number of cases */
			'<',		/* case 1 */
			'>',		/* case 2 */
			'|'		/* case 3 */
};
			         /* Specially defined prompt string chars    */
				 /* for SHTOOLS.C setprompt() case statement */
				 /* Do not rearrange !!!		     */
BYTE	shstpmt[]	=
{
			13,		/* number of cases */
			'$',
			'_',
			'b',
			'd',
			'e',
			'g',
			'h',
			'l',
			'n',
			'p',
			'q',
			't',
			'v'
};

/*----------------------------------------------------------------------*\
 |	Array of command line delimiters.				|
\*----------------------------------------------------------------------*/

BYTE	shdelimiters[]	=
{
	'!',	'+',	'=',	'[',	']',	'~',	'"',
	'|',	'<',	'>',	',',	';',	' ',	'\t',
	'\n',	'\r',	NULL
};

			  /*	<CR>   <LF>  <^R>   <^C>   <^Z>  */
WORD	shrdels[]	= { 5,0x000d,0x000a,0x0012,0x0003,0x001a };

	/* Characters used as options by the shell	*/

BYTE	shpsmall = 'p';
BYTE	shcsmall = 'c';

	/* Error messages				*/

BYTE	bat0000[] = "IF";
BYTE	bat0001[] = "FOR";
BYTE	bat0002[] = "GOTO";
BYTE	bat0003[] = "Syntax";
BYTE	bat0004[] = "parsing the %0 statement.";
BYTE	bat0005[] = "missing";
BYTE	bat0006[] = "invalid";
BYTE	bat0007[] = "CONDITION";
BYTE	bat0008[] = "LABEL";
BYTE	bat0009[] = "VARIABLE";
BYTE	bat0010[] = "IN";
BYTE	bat0011[] = "SET";
BYTE	bat0012[] = "DO";
BYTE	bat0013[] = "COMMAND";
BYTE	bat0014[] = "parameter";
BYTE	bat0015[] = "keyword parameter";
BYTE	bat0016[] = "\r\nThe %1 %2 was %4";
BYTE	bat0017[] = "\r\nThe %0 command should look like:\r\n\t%3";
BYTE	bat0018[] = "IF condition command";
BYTE	bat0019[] = "FOR variable IN (set) DO command";
BYTE	bat0020[] = "GOTO label";
BYTE	bat0021[] = "Out of memory";
BYTE	bat0022[] = "\r\nThere was not enough memory to run the batch file";
BYTE	bat0023[] = "\r\nMake sure your batch file is not nested too deeply.";

BYTE	sh0000[] = "SHELL";
BYTE	sh0001[] = "File not found";
BYTE	sh0002[] = "locating the %1 command.";
BYTE	sh0003[] = "\r\nThe %1 command could not be found";
BYTE	sh0004[] = "\r\nMake sure the command name is correct.";

BYTE	sh0010[] = "STDIN";
BYTE	sh0011[] = "STDOUT";
BYTE	sh0012[] = "STDERR";
BYTE	sh0013[] = "previously redirected";
BYTE	sh0014[] = "Standard I/O";
BYTE	sh0015[] = "\r\nThe %1 parameter was %3";
BYTE	sh0016[] = "\r\n%1 can be redirected to only one place at a time";
BYTE	sh0017[] = "\r\nMake sure that the %1 parameter is specified.";
BYTE	sh0018[] = "\r\nTwo or more commands are required for a pipe line.";
BYTE	sh0019[] = "receiving command";
BYTE	sh0020[] = "parsing the %2 command";

BYTE	sh0030[] = "%4 the %2 file %1.";
BYTE	sh0031[] =
	"\r\nYou do not have sufficient privileges to %3 the %2 file %1";
BYTE	sh0032[] = "\r\nThe %2 file %1 does not exist";
BYTE	sh0033[] = "\r\nThe path %1 is not valid";
BYTE	sh0034[] = "\r\nCheck for typing errors.";
BYTE	sh0035[] = "\r\nThe disk is full.";

BYTE	sh0036[] = "locating the %1 device.";
BYTE	sh0037[] = "\r\nThe device %1 was not found";
BYTE	sh0038[] = "\r\nMake sure you typed the device name correctly.";

BYTE	sh0039[] = "Load";
BYTE	sh0040[] = "loading the %1 command";
BYTE	sh0041[] = "\r\nThe load file format for the %1 command was invalid";
BYTE	sh0042[] = "\r\nCheck the contents of the %1 file.";
BYTE	sh0043[] = "\r\nThe file type of %1 is not executable";
BYTE	sh0044[] = "\r\nMake sure the file type is correctly typed.";
BYTE	sh0045[] =
	"\r\nThere was not enough free memory to load the %1 command";
BYTE	sh0046[] =
	"\r\nCheck for loaded programs that may not be needed.";
BYTE	sh0047[] =
	"\r\nOnly 31 programs may be loaded per console at any given time.";
BYTE	sh0048[] = "\r\nThe %5 failed due to an access conflict.";
BYTE	sh0049[] = "\r\nThe %1 command file is currently being updated.";
BYTE	sh0050[] =
	"\r\nYou do not have sufficient privilege to %5 the %1 command.";
BYTE	sh0051[] = "\r\nCheck the file attributes of the %1 command.";
BYTE	sh0052[] = "Device not found";
BYTE	sh0053[] = "Device";
BYTE	sh0054[] = "setting the default device to %1.";
BYTE	sh0055[] = "\r\nThe default device can only be set to a disk device";
BYTE	sh0056[] =
"\r\nSHELL: There was not enough memory available to run the shell.";
BYTE	sh0057[] = "\r\nSHELL: There was an internal shell error.\r\n";

BYTE	sh0060[] = "Non-existent memory";
BYTE	sh0061[] = "Memory bound";
BYTE	sh0062[] = "Illegal instruction";
BYTE	sh0063[] = "Divide by zero";
BYTE	sh0064[] = "Bound";
BYTE	sh0065[] = "Overflow";
BYTE	sh0066[] = "Privilege violation";
BYTE	sh0067[] = "Trace";
BYTE	sh0068[] = "Breakpoint";
BYTE	sh0069[] = "Floating point";
BYTE	sh0070[] = "Stack fault";
BYTE	sh0071[] = "Untrappable";
BYTE	sh0072[] = "Emulated instruction group 1";

BYTE	sh0080[] = "\r\nThe operating system aborted the %1 command";
BYTE	sh0081[] = "\r\nCheck the contents of the command file.";

BYTE	sh0082[] = "\r\nA stack has not been defined in the command header";

BYTE	*sh0100[] =
{
	"\r\n%0: %2 exception.",
	"%6 %7",
	"%6 %7 %9"
};

BYTE	*sh0101[] =
{
	"\r\n%0: %5.",
	"%2 %6",
	"%3 %9"
};

					/* for XLOGOFF.C */
BYTE	lof0000[] = "User: ";
BYTE	lof0001[] = " logged off ";
BYTE	lof0002[] = "noname";
BYTE	lof0003[] = "name";
BYTE    lof0004[] = "Node: ";
BYTE    lof0005[] = "Error trying to logoff ";
BYTE    lof0006[] = "Logged off Node: ";
BYTE	lof0007[] = " ";
BYTE    lof0008[] = "Node does not exist.";
BYTE    lof0009[] = "Not logged on to specified node.";
BYTE    lof0010[] = "Local node name not set.";
BYTE	lof0011[] = "Error:  An invalid parameter was passed to logoff.";
BYTE    lof0012[] = "Local logoffs require a null command tail.  Network \
logoffs require\n\ra node name followed by an optional user name.";
BYTE	lof0013[] = "The error code returned is:  ";
BYTE    lof0014[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A',
		     'B', 'C', 'D', 'E', 'F'};


					/* for XCLS.C */
BYTE	cls0000[] = "\033\105";
					/* for XECHO.C */
BYTE	ech0000[] = "ECHO is ";
BYTE	ech0001[] = "OFF";
BYTE	ech0002[] = "ON";
					/* for XLIST.C */
BYTE	lst0000[] = "\r\n                        List of Internal Commands";
BYTE	lst0001[] = "\r\n                        =========================";
					/* for XORDER.C */
BYTE	ord0000[] = "ORDER = ";
BYTE	ord0001[] = "\r\nORDER: Search order has not been defined.\r\n";
					/* for XPATH.C */
BYTE	pat0000[] = "PATH = ";
BYTE	pat0001[] = "PATH: No search path has been defined.\r\n";
					/* for CTTY.C */
BYTE	cttmodule[] = "CTTY";
BYTE	ctty000[] = "con";
BYTE	ctty001[] = "con:console";
					/* for DVRLINK.C */
BYTE	dlk0000[] = "\r\nDVRLINK: Install failed.\r\n";
BYTE	dlk0001[] = "\r\nDVRLINK: Missing parameters.\r\n";
					/* for DVRLOAD.C */
BYTE	dld0000[] = "\r\nDVRLOAD: Install failed.\r\n";
BYTE	dld0001[] = "\r\nDVRLOAD: Illegal option.\r\n";
BYTE	dld0002[] = "\r\nDVRLOAD: Missing parameters.\r\n";
					/* for DVRUNIT.C */
BYTE	dut0000[] = "\r\nDVRUNIT: Install failed.\r\n";
BYTE	dut0001[] = "\r\nDVRUNIT: Illegal option.\r\n";
BYTE	dut0002[] = "\r\nDVRUNIT: Missing parameters.\r\n";
					/* for DVRUNLK.C */
BYTE	duk0000[] = "\r\nDVRUNLNK: Uninstall failed.\r\n";
					/* for DVRTOOLS.C */
BYTE	dvrflgs[] =
{
		9,			/* number of cases */
		'E',
		'L',
		'M',
		'N',
		'P',
		'R',
		'S',
		'W',
		'V'
};


Reading a:asnmsgs.c
>>>a:asnmsgs.c
/*
	ASNMSGS.C
	Version	1.0
	05/01/85
*/

/* Message module for ASSIGN.C */

#include	"portab.h"
#include	"concur.h"

BYTE	asn000[] =
"ASSIGN";

BYTE	*asn010[] = {
LN_DRA,
LN_DRB,
LN_DRC,
LN_DRD,
LN_DRE,
LN_DRF,
LN_DRG,
LN_DRH,
LN_DRI,
LN_DRJ,
LN_DRK,
LN_DRL,
LN_DRM,
LN_DRN,
LN_DRO,
LN_DRP
	};

BYTE	asn020[] =					   /* %6 for *cc0430 */
"\r\nOnly the letters A through P may be used for a disk drive name.";

BYTE	asn021[] =					   /* %6 for *cc0430 */
"\r\n%0 expected an equal (=) sign after the disk device name.";

BYTE	asn022[] =					   /* %7 for *cc0500 */
"the logical name %4";					/* %4 = logical name */

/**/
