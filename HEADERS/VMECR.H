/*	@(#)vmecr.h	1.5		*/
/*
 *	VME/10 Control Map Definitions
 */

/* 	used to describe character and attribute generator RAM */
typedef struct
{
	BYTE	fill;
	BYTE	mem;
} GENRAM;

/*	Character Generator RAM */
#define	CGENADR	((BYTE *)0xf14000)
#define	CGENSZ	4096
typedef GENRAM (*CGENRAM)[CGENSZ];

/*	Attribute Generator RAM */
#define	AGENADR	((BYTE *)0xf15000)
#define	AGENSZ	4096
typedef GENRAM (*AGENRAM)[AGENSZ];

/*	Display RAM */
#define	DISPADR	((WORD *)0xf17000)
#define DISPSZ	8192
/* typedef	WORD (*DISPRAM)[DISPSZ]; defined in v10con.h */

/*	information necessary to access the CRTC (the screen controller)     */

typedef struct			/* map of the registers for the CRTC  */
{
	char	addr_reg;	/* address register selects which 
				 * register to write in register file
				 */
	char	fill2;
	char	reg_file;	/* data to registers goes here	     */
} crtc_map;

#define CRTC_ADDR	((crtc_map *)0xf1a021) /* address of the CRTC	     */
#define CURHIGH		14
#define CURLOW		15
#define CURSTART	10
#define CURS_OFF	0x20	/* bit pattern to turn cursor off */
#define CURS_ON		0	/* bit pattern to turn cursor on */

/*
 *	VME/10 Control Register block.
 */

typedef struct
{
	WORD	grafvcurs;	/* graphics cursor positioning vertical */
	WORD	grafhcurd;	/* graphics cursor positioning horizontal */
	BYTE	fill0;
	BYTE	cr0;		/* control register 0 */
	BYTE	fill1;
	BYTE	cr1;		/* control register 1 */
	BYTE	fill2;
	BYTE	cr2;		/* control register 2 */
	BYTE	fill3;
	BYTE	cr3;		/* control register 3 */
	BYTE	fill4;
	BYTE	cr4;		/* control register 4 */
	BYTE	fill5;
	BYTE	cr5;		/* control register 5 */
	BYTE	fill6;
	BYTE	cr6;		/* control register 6 */
	BYTE	fill7;
	BYTE	grafofst;
} *VMECRP;

#define	VME10CR	((BYTE *)0xf19f00)
#define VME10CRSZ	20

/*
 *	status register for the VME/10 - VMEC1 Processor Board
 */

#define VME10ST		((BYTE *)0xf19f85)
#define VME10STSZ	1

typedef BYTE *VMESTP;

he VME/10 - VMEC1 Processor Board
 */

#define VME10ST		((BYTE *)0xf1