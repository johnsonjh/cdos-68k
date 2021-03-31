/*	@(#)vmconmd.h	1.1		*/
/*
 *	This header file contains the VME/10 console
 *	specific information for the Concrurent DOS
 *	console driver.
 */

#define V10NROW	25	/* number of rows on the VME/10 screen */
#define V10NCOL	80	/* number of columns on the VME/10 screen */

/*
 *	define the DISPRAM type which describes a character
 *	mapped video plane - the physical display memory.
 */

typedef WORD (* DISPRAM)[V10NCOL];

/*
 *	Define a macro function to force a virtual frame
 *	plane access to be WORD oriented according to a
 *	DISPRAM description;
 *
 *		f -	is the frame pointer
 *		r -	is the row index
 *		c -	is the column index
 */

#define wordview(f,r,c)		(((DISPRAM)f->fr_pl[PL_ATTR])[r][c])

/************************************************************************
 *	Define information to manipulate the VME/10 block		*
 *	of the character map.						*
 *									*
 *	A word in the character and attribute RAM			*
 *	corresponds to a character on the screen.			*
 *									*
 *	The high byte of the word corresponds to the attributes		*
 *	of the character to be displayed, and the low byte		*
 *	(less the high bit) is the seven bit ASCII character		*
 *	to display.							*
 ************************************************************************/

#define PH_BACK		0x0004
#define CHARMASK	0x3f

/* character attributes	*/

#define OPTION1		0x0080

#define RED		0x0100	/* bit 0 - color or intensity bit 3 */
#define BLUE		0x0200	/* bit 1 - color or intensity bit 2 */
#define GREEN		0x0400	/* bit 2 - color or intensity bit 1 */

#define V_INVERSE	0x08	/* bit 3 - inverse video */
#define V_UNDERLINE	0x10	/* bit 4 - underline character */
#define V_BLINK		0x20	/* bit 5 - blink character */
#define V_CHARON	0x40	/* bit 6 - don't display character */

/* for monochrome monitors */
#define V_LOWINTENSE	0x04
#define V_HIGHINTENSE	0x07

#define V_INTENSIFY	0x03

#define OPTION2		0x8000	/* bit 7 - software attribute 1 */

	/* default attributes in character RAM */
#define INITATTR	(V_LOWINTENSE | V_CHARON)
#define	BACKVID		0x4a20	/* background video character	       */

aracter R