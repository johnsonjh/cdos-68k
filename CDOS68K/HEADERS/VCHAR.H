/*	@(#)vchar.h	1.1		*/
/*
 *	Defines for magic numbers in the 16 bit output character
 *	set for Concurrent DOS.
 */

/* high nibble of 16 bit character */
#define OC_NOSEE	0x1	/* Non-visible characters that take no space */
#define OC_EDITFN	0x2
#define OC_ROWGOTO	0x3
#define OC_COLGOTO	0x4
#define OC_COLOR	0x5	/* 50 set foreground, 51 set background */

/* mask to extract the absolute new row or column if OC_ROWGOTO or OC_COLGOTO */
#define OC_GOTOMASK	0x0fff

/* define the particular edit functions if high nibble == OC_EDITFN */
#define OC_DOCHINSERT	0x40	/* Enter insert character mode */
#define OC_NOCHINSERT	0x4F	/* Exit insert character mode */
#define OC_UPCURSOR	0x41
#define OC_DOWNCURSOR	0x42
#define OC_RIGHTCURSOR	0x43
#define OC_LEFTCURSOR	0x44
#define OC_CLEAR	0x45
#define OC_HOME		0x48
#define OC_INDEXREV	0x49	/* Reverse Index (Reverse Line Feed) */
#define OC_EEOP		0x4a	/* Erase to End Of Page */
#define OC_EEOL		0x4b	/* Erase to End Of Line */
#define OC_EEL		0x6C	/* Erase Entire Line */
#define OC_LNINSERT	0x4c	/* Insert blank line */
#define OC_LNDELETE	0x4d	/* Delete Line */
#define OC_CHDELETE	0x4e	/* Delete Character */
#define OC_EBOP		0x64	/* Erase to Beginning of Display */
#define OC_ENABLECURS	0x65	/* Enable Cursor */
#define OC_DISABLECURS	0x66	/* Disable Cursor */
#define OC_SAVECURS	0x6A	/* Save the cursor position */
#define OC_RESTORECURS	0x6b	/* Restore Cursor Position */
#define OC_EBOL		0x6f	/* Erase to Beginning of Line */
#define OC_REVERSE	0x70	/* Enter Reverse Video */
#define OC_NOREVERSE	0x71	/* Exit Reverse Video */
#define OC_INTENSIFY	0x72	/* Enter Intensify Mode */
#define OC_NOINTENSIFY	0x75	/* Exit Intensify Mode */
#define OC_BLINK	0x73	/* Enter Blink Mode */
#define OC_NOBLINK	0x74	/* Exit Blink Mode */
#define OC_WRAP		0x76	/* Wrap at end of line */
#define OC_NOWRAP	0x77	/* Discard at End of Line */

/*
 *	Defines for 16 bit input character set.
 */

#define IC_SPECIAL	0x2000

#define IC_HELP		0x00
#define IC_WINDOW	0x01
#define IC_NEXT		0x02
#define IC_PREVIOUS	0x03
#define IC_PRSCR	0x04
#define IC_BREAK	0x05
#define IC_REDRAW	0x06
#define IC_BEGIN	0x07
#define IC_END		0x08
#define IC_INSERT	0x09
#define IC_DELETE	0x0A
#define IC_SYSREQ	0x0B

/*	Cursor Movement	*/

#define IC_UP		0x10
#define IC_DOWN		0x11
#define IC_LEFT		0x12
#define IC_RIGHT	0x13
#define IC_PAGEUP	0x14
#define IC_PAGEDN	0x15
#define IC_PAGELT	0x16
#define IC_PAGERT	0x17
#define IC_HOME		0x18
#define IC_REVTAB	0x19

/*	Numeric Keypad	*/

#define IC_ZERO		0x30
#define IC_ONE		0x31
#define IC_TWO		0x32
#define IC_THREE	0x33
#define IC_FOUR		0x34
#define IC_FIVE		0x35
#define IC_SIX		0x36
#define IC_SEVEN	0x37
#define IC_EIGHT	0x38
#define IC_NINE		0x39
#define IC_A		0x3A
#define IC_B		0x3B
#define IC_C		0x3C
#define IC_D		0x3D
#define IC_E		0x3E
#define IC_F		0x3F
#define IC_ENTER	0x40
#define IC_COMMA	0x41
#define IC_MINUS	0x42
#define IC_PERIOD	0x43
#define IC_PLUS		0x44
#define IC_DIVIDE	0x45
#define IC_MULTIPLY	0x46
#define IC_EQUAL	0x47

/*
 *	Some useful bits in 16 bit character.
 */

#define IC_SHIFTED	0x0400
#define IC_FNKEY	0x1000
fine IC_EQUAL	0x47

/*
 *	Some useful bits in 16 bit character.
 */

#define IC_SHIFTED	0x0400
#