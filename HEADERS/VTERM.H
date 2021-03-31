/*	@(#)vterm.h	1.1		*/
/*
 *	Terminal SubDriver Header File
 */

#define MAXTUNIT	2

/*
 *	Special entry point functions.
 */

#define TGOTO	1
#define ED_TERM	(0x80000000L | (DVR_TERM << 16))	/* Terminal Driver */
pecial entry point functions