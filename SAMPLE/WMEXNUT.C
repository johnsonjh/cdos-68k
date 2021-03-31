
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
 *   Version 1.4        WMEXNUT.C				 *
 *                      WMEX None User Text			 *
 *---------------------------------------------------------------*
 *    VERSION   DATE    BY      CHANGE/COMMENTS                  *
 *---------------------------------------------------------------*
 *                                                               *
 *	1.4   07/24/85	jsr	Removed wm8140, wm8150 & wm8160, *
 *				modified wm9005.		 *
 *	1.3   07/15/85	jsr	Added wm9009.			 *
 *	1.2   05/06/85	jsr	Modified wm9007 and wm9008.	 *
 *	1.1   04/26/85	jsr	Added wm9007 and wm9008.	 *
 *	1.0   04/23/85	jsr					 *
 *                                                               *
 *===============================================================*/

#include	"portab.h"

						     /* system logical names */
UBYTE	wm8000[] =
	"con:";					   /* name of console device */

UBYTE	wm8001[] =
	"vc";				      /* virtual console path leadin */

UBYTE	wm8002[] =
	"console";				     /* name of console file */

UBYTE	wm8003[] =
	"mouse";				       /* name of mouse file */

UBYTE	wm8004[] =
	"top";					  /* name of top border file */

UBYTE	wm8005[] =
	"bottom";			       /* name of bottom border file */

UBYTE	wm8006[] =
	"left";					 /* name of left border file */

UBYTE	wm8007[] =
	"right";				/* name of right border file */


UBYTE	wm8100[] =
	"shell";			      /* name of shell to be envoked */

UBYTE	wm8110[] =
	"wmessage";		      /* logical name of message window pipe */

UBYTE	wm8120[] =
	"pi:%w";	       /* partial name of actual message window pipe */

UBYTE	wm8130[] =
	"pi:";					      /* name of pipe device */

UBYTE	wm8170[] =
	"prn:";					   /* name of PRINTER device */


						 /* carriage return/linefeed */
UBYTE	wm9000[] =
	"\r\n";

							         /* formfeed */
UBYTE	wm9002[] =
	"\f";
							    /* wildcard name */
UBYTE	wm9003[] =
	"*";
						   /* pad char for STAT info */
UBYTE	wm9004 =
	' ';

UBYTE	wm9005[] =
	"\b \b";

UBYTE	wm9006 =
	'0';
						/* set foreground attributes */
UBYTE	wm9007[] = {
	0x1b,							      /* ESC */
	0x62,							      /* 'b' */
	0x00			       /* attribute value (will be poked in) */
	};
						/* set background attributes */
UBYTE	wm9008[] = {
	0x1b,							      /* ESC */
	0x63,							      /* 'c' */
	0x00			       /* attribute value (will be poked in) */
	};

					    /* shut off WRAP escape sequence */
UBYTE	wm9009[] = {
	0x1b,							      /* ESC */
	0x77,							      /* 'w' */
	0x00						  /* NULL terminator */
	};

/* */

