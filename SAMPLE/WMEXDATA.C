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
 *   Version 1.3  	WMEXDATA.C 				 *
 *			Configuration and Text data for WMEX.C   *
 *                      Window Manager.				 *
 *---------------------------------------------------------------*
 *    VERSION   DATE    BY      CHANGE/COMMENTS                  *
 *---------------------------------------------------------------*
 *                                                               *
 *	1.3   02/11/86	ldt	Modified STATUS MESSAGE window   *
 *                              descriptors mainly to allow 10   *
 *                              lines of input to the WMEX mssg  *
 *                              pipe from a user.                *
 *	1.2   07/15/85 						 *
 * 								 *
 *===============================================================*/

#include	"portab.h"
#include	"wmos.h"
#include	"wmex.h"


							 /* text information */
							   /* name of module */
UBYTE	wm0000[] =
	"WMEX";

UBYTE	wm0010 = 1;	       /* number of USRx windows to start up (1 - 6) */

						  /* DESK window information */
UBYTE	wm0100	= 0x00;		       /* flags for attribute enable */

UBYTE	wm0110	= ' ';					       /* fill value */

UBYTE	wm0120	= 0x00;					  /* attribute value */


						      /* MESSAGE window text */
UBYTE	wm0200[] =			 /* message window top border header */
	"[ MESSAGE WINDOW ]";

UBYTE	wm0205[] =			 /* no shell command, since no shell */
	"";

UBYTE	*wm0210[] = {				    /* array of HEADER lines */
	"",
	" Message from %1 on Screen %2:",
	""
	};

UBYTE	wm0220[] =				  /* info line format string */
	"%1";

UBYTE	*wm0230[] = {				      /* array of MORE lines */
	""						/* minimum is 1 line */
	};

UBYTE	*wm0240[] = {				    /* array of PROMPT lines */
	""   /* NOTE: This is a placeholder, wm0260 CMD prompts used instead */
	};

UBYTE	*wm0250[] = {				      /* array of HELP lines */
	"",
	" Press <WINDOW> to delay answer."
	};

UBYTE	*wm0260[] = {				     /* array of CMD prompts */
	" Enter response: ",
	" Press Enter to acknowledge: "
	};

UBYTE	*wm0270[] = {				   /* array of CMD help info */
	" An invalid command was selected."
	};

UBYTE	wm0299[] =
	"  %1      -          - Active Message Window";


						       /* STATUS window text */
UBYTE	wm0300[] =			  /* status window top border header */
	"[ STATUS WINDOW ]";

UBYTE	wm0305[] =			 /* no shell command, since no shell */
	"";

UBYTE	*wm0310[] = {				    /* array of HEADER lines */
	"",
	" WNDW# FMYID PROCESS ID    COMMAND LINE   ",
	" ===== ===== ========== =================="
	};

UBYTE	wm0320[] =				  /* info line format string */
	"  %1  %2 %3 %4";

UBYTE	*wm0330[] = {				      /* array of MORE lines */
	"    ... more ... (type M to see more)"
	};

UBYTE	*wm0340[] = {				    /* array of PROMPT lines */
	"Command ? ",
	};

UBYTE	*wm0350[] = {				      /* array of HELP lines */
	"=========================================\
========================================",
	"",
	" Commands:",
	"             C - CREATE Screen",
	"             D - DELETE Screen",
	"        <HELP> - HELP INFORMATION",
	"      <number> - Screen number to select",
	""
	};

UBYTE	*wm0360[] = {				     /* array of CMD prompts */
	"",							  /* general */
	"",							   /* number */
	"",							   /* create */
	"Select a Window Number to Delete: ",			   /* delete */
	"",							     /* more */
	"",							     /* help */
	"",							 /* previous */
	""							     /* next */
	};

UBYTE	*wm0370[] = {				   /* array of CMD help info */
	"An invalid command was selected.",
	"The screen number was out of range, or not active.",
	"The maximum numbers of screens have been created.",
	"The screen number was out of range, or not active.",
	"",
	"The Message window already contains an active message.",
	"",
	""
	};

						  /* USR1 window information */
UBYTE	wm0400[] =
	"[ USER SCREEN #1 ]";

UBYTE	wm0405[] =				       /* shell command line */
	"home:autoexec.bat";
						  /* USR2 window information */
UBYTE	wm0500[] =
	"[ USER SCREEN #2 ]";

UBYTE	wm0505[] =				       /* shell command line */
	"home:autoexec.bat";
						  /* USR3 window information */
UBYTE	wm0600[] =
	"[ USER SCREEN #3 ]";

UBYTE	wm0605[] =				       /* shell command line */
	"home:autoexec.bat";
						  /* USR4 window information */
UBYTE	wm0700[] =
	"[ USER SCREEN #4 ]";

UBYTE	wm0705[] =				       /* shell command line */
	"home:autoexec.bat";
						  /* USR5 window information */
UBYTE	wm0800[] =
	"[ USER SCREEN #5 ]";

UBYTE	wm0805[] =				       /* shell command line */
	"home:autoexec.bat";
						  /* USR6 window information */
UBYTE	wm0900[] =
	"[ USER SCREEN #6 ]";

UBYTE	wm0905[] =				       /* shell command line */
	"home:autoexec.bat";


							/* Error help prompt */
UBYTE	wm1000[] =
	"Press any key to continue: ";

						    /* STAT window HELP info */
UBYTE	wm2000 = 7;			     /* number of lines of HELP info */

UBYTE	*wm2001[] = {
	"\r\n",
	" After selecting a window, the <PREV>, <NEXT>\r\n",
	" and <WINDOW> keys can be used as follows:\r\n",
	"\r\n",
	"   <PREV> select the next lower Window Number\r\n",
	"   <NEXT> select the next higher Window Number\r\n",
	" <WINDOW> return to the Status Window\r\n"
	};

						       /* configuration data */
					    /* descriptor for MESSAGE window */
WNDWDESC	wd_mssg =
{
    19, 40, 21, 0, 0, 0,
    (W_BORDERS|W_INTERNF|W_PMPTHLP),
    ' ', '=', '=', '#', '#',
    0x00, 0x00,
    wm0200, wm0205
};

					     /* descriptor for STATUS window */
WNDWDESC	wd_stat =
{
    19, 40, 21, 0, 0, 0,
    (W_BORDERS|W_INTERNF),
    ' ', '=', '=', '#', '#',
    0x00, 0x00,
    wm0300, wm0305
};
					       /* descriptor for USR1 window */
WNDWDESC	wd_usr1 =
{
    1, 1, 0, 0, 0, 0,
    0,
    ' ', ' ', ' ', ' ', ' ',
    0, 0,
    wm0400, wm0405
};
					       /* descriptor for USR2 window */
WNDWDESC	wd_usr2 =
{
    1, 1, 0, 0, 0, 0,
    0,
    ' ', ' ', ' ', ' ', ' ',
    0, 0,
    wm0500, wm0505
};
					       /* descriptor for USR3 window */
WNDWDESC	wd_usr3 =
{
    1, 1, 0, 0, 0, 0,
    0,
    ' ', ' ', ' ', ' ', ' ',
    0, 0,
    wm0600, wm0605
};
					       /* descriptor for USR4 window */
WNDWDESC	wd_usr4 =
{
    1, 1, 0, 0, 0, 0,
    0,
    ' ', ' ', ' ', ' ', ' ',
    0, 0,
    wm0700, wm0705
};
					       /* descriptor for USR5 window */
WNDWDESC	wd_usr5 =
{
    1, 1, 0, 0, 0, 0,
    0,
    ' ', ' ', ' ', ' ', ' ',
    0, 0,
    wm0800, wm0805
};
					       /* descriptor for USR6 window */
WNDWDESC	wd_usr6 =
{
    1, 1, 0, 0, 0, 0,
    0,
    ' ', ' ', ' ', ' ', ' ',
    0, 0,
    wm0900, wm0905
};

					/* SPECial items for defined windows */

		/* NOTE: numbers in 1st column below should total the RMIN   */
		/* value in the associated WNDWDESC table. If BORDERS are    */
		/* enabled for this window then add two (2) more to total    */
		/* below to account for them.				     */

					    /* descriptor for MESSAGE window */
WNDWSPEC	ws_mssg =
{
    3, wm0210,
    10, 0, wm0220,
    1, wm0230,
    1, wm0240,
    2, wm0250,
       wm0260,
       wm0270
};
					     /* descriptor for STATUS window */
WNDWSPEC	ws_stat =
{
    3, wm0310,
    4, 0, wm0320,
    1, wm0330,
    1, wm0340,
    8, wm0350,
       wm0360,
       wm0370
};

/* */
