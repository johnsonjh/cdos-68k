/* vmpanic.c - xputc for VME/10 */

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
 *   Version 1		vmpanic
 *			panic console output for VME/10
 *---------------------------------------------------------------*
 * VER	DATE	 BY	CHANGE/COMMENTS
 *
 *  1	07/23/85 MA	initial version
 *===============================================================*/

#include "portab.h"


MLOCAL WORD row = 0;
MLOCAL WORD col = 0;


/*  VME/10 hardware constants */

#define DISPLAY ((WORD *)0xf17000)		/* display RAM */
#define CR0	((BYTE *)0xf19f05)		/* control register */
#define ATTRIB	0x4c00				/* attributes in upper byte */

xputc(c)
WORD c;
{
    if (!row && !col)				/* never been called? */
	*CR0 = 0x18;				/* enable character RAM */

    switch (c)
    {
	case '\r':
	    for ( ;col < 80; col++)		/* erase to end of line */
		screen(' ');
	    col = 0;				/* then back to column one */
	    break;
	case '\n':
	    row++;				/* assume no scrolling */
	    break;
	default:
	    screen(c);				/* write the character */
	    if (++col == 80)			/* at end of line? */
	    {
		row++;				/* skip to next row */
		col = 0;			/* and back to column 0 */
	    }
	    break;
    }
}


MLOCAL screen(c)
WORD c;
{
    DISPLAY[row*80+col] = ATTRIB + c;
}
                                                                                                              
TRIB + c;
}
                                                                                                              