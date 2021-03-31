/*	@(#)vm400.h	1.12		*/
/*
 *	Concurrent 4.0 Serial Port Driver header file
 *	for the MVME 400 Dual Serial Port Card.
 */

/*
 *	Setting this flag will result in diagnostic code being
 *	generated.  Things like queue block counts, number
 *	of characters transmitted and received.  It will
 *	not reult in a printout, you have to read memory
 *	with ddt or TENbug.
 */

#define DODIAGNOSTICS	0

#if	DODIAGNOSTICS
#define DIAGNOSE(x)	x
#else
#define DIAGNOSE(x)
#endif

/*
 *	Select Parameter Block
 *
 *	The real important information in this block is the
 *	Keyboard ASR function address, which we call via doasr()
 *	to send keyboard (input) characters to our parent driver.
 */

#define PSELPB	struct pselpb

PSELPB
{
	BYTE	sunit;
	BYTE	sinfo;
	WORD	sflags;
	VOID	(*skeybdasr)();	/* address of keyboard ASR of parent */
};

/*	Flush Parameter Block */

#define PFLUPB	struct pflupbp

PFLUPB
{
	BYTE	funit;
	BYTE	finfo;
	WORD	fflags;
	LONG	fzero;
	BYTE	*fpdaddr;
};

/*	Special Parameter Block */

#define PSPPB	struct psppb

PSPPB
{
	BYTE	spunit;
	BYTE	spoption;
	WORD	spflags;
	BYTE	*sppdaddr;
	BYTE	*spdatabuf;
	LONG	spdbufsiz;
	BYTE	*spparmbuf;
	LONG	sppbufsiz;
};

/*	Special Functions */

/*	Serial Port Get/Set Table */

#define PGSTBL	 struct pgstbl

PGSTBL
{
	WORD	pt_type;	/* port type - see below */
	WORD	pt_state;	/* port state - presently undefined */
	BYTE	pt_baud;	/* port baud rate - see below */
	BYTE	pt_mode;	/* port format control - see below */
	BYTE	pt_control;	/* port modem control - see below */
	BYTE	pt_1fill;	/* fill byte - available for expansion */
};

/*	Get/Set Parameter Block */

#define PGSPB	 struct pgspb

PGSPB
{
	BYTE	gsunit;
	BYTE	gsreserved;
	WORD	gsflags;
	LONG	gsres2;
	BYTE	*gspdaddr;
	PGSTBL	*gsbufadr;
	LONG	gsbufsiz;
};

/*	pt_type definitions */

#define PT_TSER	1	/* serial port */
#define PT_TCHR	2	/* character port */
#define PT_TPAR	4	/* parallel port */

/*	pt_baud definitions */

#define PT_B110  2
#define PT_B150  4
#define PT_B300  5
#define PT_B600  6
#define PT_B1200 7
#define PT_B2400 10
#define PT_B4800 12
#define PT_B9600 14
#define PT_B192K 15

/*	pt_mode definitions - bits per word */

#define PT_M5BIT 0
#define PT_M6BIT 1
#define PT_M7BIT 2
#define PT_M8BIT 3

/*	pt_mode definitions - number of stop bits */

#define PT_MST1  4	/* 1 stop bit */
#define PT_MST1H 8	/* 1-1/2 stop bits */
#define PT_MST2  0x0c	/* 2 stop bits */

/*	pt_mode definitions - parity control */

#define PT_MENPR 0x10	/* parity enabled */
#define PT_MEVPR 0x20	/* even parity */

/*	pt_control definitions */

#define PT_CDTR  2	/* DTR */
#define PT_CRTS  0x20	/* RTS */
#define PT_CTXEN 1	/* transmitter enable */
#define PT_CRXEN 4	/* receiver enable */
#define PT_CXXEN 0x10	/* xon/xoff enable */
#define PT_CDSR  0x20	/* DSR - read only, cannot be "set" */
#define PT_CDCD  0x40	/* DCD - read only, cannot be "set" */

/*	miscellaneous defines for port table */

#define PT_SRDYXMIT	0x01
#define PT_SRECVD	0x02
#define PT_SCHANGE	0x04
#define PT_SPARERR	0x08
#define PT_SOVERRUN	0x10
#define PT_SFRAMEERR	0x20
#define PT_SDCD		0x40
#define PT_SDSR		0x80

/*
 *	Control Registers State Array for the NEC 7201 Chip on Serial Card.
 *	We keep copies because control registers are WRITE ONLY,
 *	when you read them, you get the status register contents.
 */

#define PSTATE	struct pstate

PSTATE
{
	BYTE	cr0;
	BYTE	cr1;
	BYTE	cr2;
	BYTE	cr3;
	BYTE	cr4;
	BYTE	cr5;
};

/*
 *	The following structure defines the pertinent
 *	information about each port.
 */

#define PTTY	struct	ptty

PTTY
{
	WORD	t_flags;	/* Flags copied from parameter block */
	LONG	t_wrflag;	/* System write flag */
	QUEUE	*t_outq;	/* Character output queue */
	PSTATE	t_regst;	/* Control register states */
	PGSTBL	t_gstbl;	/* Port get/set table */
	WORD	t_info;		/* Current state of port */
	LONG	t_caller;	/* PDA of calling process */
	LONG	t_keybdasr;	/* ASR to invoke and pass char on recv intr */
	WORD	t_charmsk;	/* Current char size mask value */

	/* internal diagnostic and monitor info for performance analysis */
#if DODIAGNOSTICS
	LONG	t_wr_count;	/* Total characters written */
	LONG	t_rd_count;	/* Total characters read */
	LONG	t_wr_errs;	/* Total write errors */
	LONG	t_rd_errs;	/* Total read errors */
	LONG	t_wrpastblk;	/* Total times write past an event block */
	LONG	t_wrasrfail;	/* Total times write ASR failed */
	LONG	t_blocks;	/* Total number of blocks */
#endif
};

/*
 *	Define some alias's for the Get and Set Table
 */

#define  t_type     	t_gstbl.pt_type
#define  t_state    	t_gstbl.pt_state
#define  t_baud     	t_gstbl.pt_baud
#define  t_mode     	t_gstbl.pt_mode
#define  t_control  	t_gstbl.pt_control
#define  t_1fill    	t_gstbl.pt_1fill

/*	t_state bits - used in the get/set table */

#define S_CARRIER 	8

/* 	Miscellaneous defines */

#define USERBUF		0x8000

/*	t_info bits - internal state information */

#define I_SELECTED 	0x01
#define I_TXSTARTED	0x02
#define I_QBLOCKED	0x04

/*	Temporary fix for E_NO_FLAGS	 */

#define E_NO_FLAGS 0x4022L

/*	mapphys parameter block - here for lack of a better place */
typedef struct
{
	LONG	mpzero;
	BYTE	*pbegaddr;
	LONG	plength;
} MAPPB;

/*	mapphys memory type */
#define MAPCODE	0	/* physical memory contains code	*/
#define MAPDATA	1	/* physical memory contains data	*/

emory type */
#define MAPCODE	0	/* physical memory contains code	*/
#define MAPDATA	1	/* physical 