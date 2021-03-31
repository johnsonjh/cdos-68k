/*	@(#)bootfix.h	1.5		*/

/*
 *	Now for something really disgusting:
 *
 *	TENbug and other Motorola bugs REQUIRE that the first
 *	4 bytes of the object booted in be the initial 
 *	stack pointer, and the next long be the PC.  For consistency
 *	with 86 based filesystems, we collapse the boot info of the bug
 *	into the first fields of the boot record describing the file
 *	system.  It all works out very nicely in fact, but a despicable
 *	programming practice none the less.
 */

typedef struct
{
	LONG	bf_sp;			/* stack pointer, not used as such */
	LONG	bf_pc;			/* initial PC */
	BYTE	bf_68k[3];		/* a three byte signature */
} BTFIX;

/*	Magic little string for boot print */

#define BPSTRING	"bjp"
