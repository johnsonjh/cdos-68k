
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
 *   Version 1.0        WMCHAR.H				 *
 *                      16 bit character definitions for WMEX.	 *
 *---------------------------------------------------------------*
 *    VERSION   DATE    BY      CHANGE/COMMENTS                  *
 *---------------------------------------------------------------*
 *                                                               *
 *	1.0   04/23/85	jsr					 *
 *                                                               *
 *===============================================================*/

/* Normal characters */

#define	CH_BS		0x0008		/* backspace		*/
#define	CH_NL		0x000a		/* newline (linefeed)	*/
#define	CH_CR		0x000d		/* carriage return	*/

#define	CH_0		0x0030		/* 0			*/
#define	CH_1		0x0031		/* 1			*/
#define	CH_2		0x0032		/* 2			*/
#define	CH_3		0x0033		/* 3			*/
#define	CH_4		0x0034		/* 4			*/
#define	CH_5		0x0035		/* 5			*/
#define	CH_6		0x0036		/* 6			*/
#define	CH_7		0x0037		/* 7			*/
#define	CH_8		0x0038		/* 8			*/
#define	CH_9		0x0039		/* 9			*/

#define	CH_C		0x0043		/* C			*/
#define	CH_D		0x0044		/* D			*/
#define	CH_M		0x004d		/* M			*/

#define	CH_LC		0x0063		/* c			*/
#define	CH_LD		0x0064		/* d			*/
#define	CH_LM		0x006d		/* m			*/


/* Special characters */

#define	CI_HELP		0x2000		/* Help Key		*/
#define CI_WINDOW	0x2001		/* Window Key		*/
#define CI_NEXT		0x2002		/* Next Key		*/
#define CI_PREV		0x2003		/* Previous Key		*/
#define	CI_PSCRN	0x2004		/* Print Screen		*/

/* */

