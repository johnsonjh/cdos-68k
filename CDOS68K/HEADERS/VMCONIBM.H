/*	@(#)vmconibm.h	1.2		*/
/*
 *	This header file contains information about the
 *	IBM PC memory mapped monitor.
 *
 *	Concurrent DOS supports the IBM PC as a standard,
 *	all user's write to an image of a PC monitor,
 *	it is up to the console driver to appropriately map the
 *	PC image to the real, physical console.
 */

#define IBMNROW	25
#define IBMNCOL	80

/*
 *	The attribute is the odd byte of the display address
 *	and is associated with a character.
 */

#define IBMBLINK	0x80
#define IBMINTENSE	0x08

/*	Background color */

#define IBMBRED		0x40
#define IBMBGREEN	0x20
#define IBMBBLUE	0x10

/*	Foreground color */

#define IBMFRED		0x04
#define IBMFGREEN	0x02
#define IBMFBLUE	0x01

/*
 *	The monochrome monitor on the IBM PC uses
 *	the color fields to specify:
 *
 *		- character display
 *		- normal video
 *		- reverse video
 *		- underline
 */

#define IBMNOSHOW	0x00
#define IBMNORMAL	0x07
#define IBMREVERSE	0x70
#define IBMUNDLINE	0x01		/* FOR NORMAL ONLY ???? */

#define IBMINITATTR	IBMNORMAL
IBMNORMAL	0x07
#define IBMREVERSE	0x70
#define IBMUNDLINE	0x01		/* FOR NORMAL ONL