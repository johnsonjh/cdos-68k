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
 *   Version 1.02       VM400.C					 *
 *			MVME400 serial port driver for		 *
 *			Concurrent DOS on the VME/10		 *
 *---------------------------------------------------------------*
 *    VERSION   DATE      BY    CHANGE/COMMENTS                  *
 *---------------------------------------------------------------*
 *    1.00	??/??/??  BJP	Original version		 *
 *    1.01	09/30/85  MA	Allow use of external sp_int().	 *
 *    1.02	10/29/85  MA	Unblock interrupts before doing	 *
 *				asrwait() in sp_write().	 *
 *===============================================================*
 *  INCLUDES:                                                    */

#include "portab.h"
#include "system.h"
#include "vmecr.h"
#include "io.h"
#include "vmqueue.h"
#include "vm400.h"

/************************************************************************
 *
 *	MVME400 Serial Port Driver for Concurrent 4.0
 *	on the VME/10.
 *
 *
 *	This is the single character read/write port driver for use
 *	with the VME/10 Serial Console Driver and the (buffered) Serial
 *	Driver.  It exists only for use as a subdriver, and does not
 *	always adhere to all the Concurrent DOS interface rules for
 *	a driver (i.e. don't try to install this driver as a special
 *	driver controlled directly by the Special Resource Manager).
 *
 *	Copyright 1984, 1985 Motorola Inc.
 *
 ************************************************************************/

/*
 *	CONFIGURATION: The board addresses and interrupt level
 *	may be changed in this driver.  The following information describes the
 *	default values and areas to change when reconfiguring:
 */

#define ASMSPINT	FALSE	/* true if sp_int is in assembly language */
#define MAXUNITS	6 	/* maximum number of units for this driver */

#define MISSING	((SIO *)0)	/* board not there */

/*
 *	BOARD ADDRESSES - SERPORT2, or add additional defines
 *	NOTE: MISSING may be used as a place holder for boards not present
 */

/*
 *	We don't use the first suggested board address f1c1c1,
 *	for convenience reasons (conflict with TENbug, which uses
 *	the first board, possibly, instead of console, so we
 *	avoid using it).
 */

#define	SERPORT2	((SIO *) 0xf1c1a1)	/* second board address */
#define	SERPORT3	((SIO *) 0xf1c181)	/* third board address */
#define	SERPORT4	((SIO *) 0xf1c161)	/* fourth board address */

/************************************************************************
 *									*
 *	Driver Header Table - must be first thing in data section	*
 *									*
 ************************************************************************/

LONG sp_init();		LONG sp_subdrv();	LONG sp_uninit();
LONG sp_select();	LONG sp_flush();	LONG sp_read();
LONG sp_write();	LONG sp_get();		LONG sp_set();
LONG sp_special();

DH	sp_hdr =
{
	NULL,
	MAXUNITS,
	0,
	sp_init,
	sp_subdrv,
	sp_uninit,
	sp_select,
	sp_flush,
	sp_read,
	sp_write,
	sp_get,
	sp_set,
	sp_special,
	NULL, NULL, NULL, NULL,
	NULL
};

/****************************************************************************
 *	Externally and forward defined routines.
 ****************************************************************************/

VOID startasr();
WORD no_device();
EXTERN VOID asrwait();

/************************************************************************/
/*	Serial Port I/O Packets						*/
/************************************************************************/

/*
 *	Structure of MVME400 serial port hardware registers.
 *	Assumes an odd starting address, 16 bit bus.
 */

typedef struct
{
	BYTE	m4_piaad;	/* pia a data */
	BYTE	m4_fill0;	/* fill */
	BYTE	m4_piaac;	/* pia a control */
	BYTE	m4_fill1;	/* fill */
	BYTE	m4_piabd;	/* pia b data */
	BYTE	m4_fill2;	/* fill */
	BYTE	m4_piabc;	/* pia b control */
	BYTE	m4_fill3;	/* fill */
	BYTE	m4_7201d[3];	/* 7201 a data, fill, b data */
	BYTE	m4_fill5;	/* fill */
	BYTE	m4_7201c[3];	/* 7201 a control 0, fill, b control 0 */
} SIO;

/************************************************************************
 *
 *	Serial Port Variables
 *
 ************************************************************************/

MLOCAL BYTE copyright[] = "Copyright 1984, Motorola Inc.";
MLOCAL BYTE sp_ver[] = "%W%	%Q%";

/* physical map address of serial port register blocks - one per board */
SIO	*sp_phys[] = {SERPORT2, SERPORT3, SERPORT4};

/* system map address of serial port register blocks - one per board */
SIO	*sp_syst[MAXUNITS/2]; 

/*
 *	Pointer table to tty information structures
 *	associated with each port; dynamically allocated on each init.
 */

PTTY	*ttys[MAXUNITS];

/*
 *	INTERRUPT LEVEL - We use I/O channel interrupt level 4 (6 to processor).
 *
 *	To change I/O channel interrupt level you will have to modify:
 *	SERVEC (vector number), CHNEN (VME/10 interrupt	control register).
 *	NOTE: all serial boards are on the same level.
 */

#define	SERVEC	69		/* serial port interrupt vector number */
#define	INTEN	0x80		/* Overall interrupt enable */
#define INT4EN	0x40		/* INT4 I/O channel interrupt enable */
#define	CHNEN	INT4EN		/* I/O channel interrupt enable */

/************************************************************************/
/*									*/
/*	Serial Port I/O Constants					*/
/*									*/
/************************************************************************/

#define	SERASRPRI	((BYTE)200)	/* asr priority */

#define	SERPORTSZ	31		/* serial port reg block size */

/* Macro to divide by two to get port on the board given. */

#define BOARD(x)	(x>>1)

/*
 *	Macro to get the a or b side; returns either 0 or 2 depending on an
 *	odd or even unit number.
 */

#define ABSEL(x)	((x&1)<<1)

/*
 *	Macro to write out to 7201 control registers.  Writes to control
 *	register 0 to select the next register to be written to and then
 *	writes to control register 0 again to actually write to the
 *	selected control register.
 */

#define WRITE(y,x,z,w)	*y = x; *y = (z)->t_regst.w

/*
 *	7201 control register initialization values.
 *	NOTE: All values are hexadecimal.
 */

/* 7201 control register 0 operations */

#define SELREG1		0x01
#define SELREG2		0x02
#define SELREG3		0x03
#define SELREG4		0x04
#define SELREG5		0x05
#define REXSTINT	0x10
#define CHANRST		0x18
#define RSTTXINT	0x28
#define ERRRST		0x30
#define EOINT		0x38

/* 7201 control register 1 operations */

#define EXINTEN		0x01
#define TXINTEN		0x02
#define STATAFV		0x04
#define RXINTDS		0x00	/* never set? */
#define RXINT1		0x10	/* never set? */
#define RXINTALP	0x10	/* interrupt on received characters, PE is special */
#define RXINTANP	0030	/* never set? */
#define INTDSMSK	(~(RXINTDS|RXINT1|RXINTALP|RXINTANP|EXINTEN|TXINTEN)&0xFF)

/* 7201 control register 2A operations (2B is interrupt vector) */

#define BOTHINT		0x00
#define PRIRGT		0x04
#define NONVEC		0x00
#define M8086		0x10
#define RTSBP10		0x00

/* 7201 control register 3 operations */

#define RXENABLE	0x01
#define AUTOENA		0x20
#define RX5BITS		0x00
#define RX7BITS		0x40
#define RX6BITS		0x80
#define RX8BITS		0xC0
#define RXSZMSK		(~(RX5BITS|RX6BITS|RX7BITS|RX8BITS)&0xFF)

/* 7201 control register 4 operations */

#define PARENAB		0x01
#define EVENPAR		0x02
#define ODDPAR		0x00
#define SBIT1		0x04
#define SBIT1P5		0x08
#define SBIT2		0x0C
#define SBITMSK		(~(SBIT1|SBIT1P5|SBIT2)&0xFF)
#define CLKX16		0x40

/* 7201 control register 5 operations */

#define RTS		0x02
#define TXENABLE	0x08
#define SENDBRK		0x10
#define TX5BITS		0x00
#define TX7BITS		0x20
#define TX6BITS		0x40
#define TX8BITS		0x60
#define TXSZMSK		(~(TX5BITS|TX6BITS|TX7BITS|TX8BITS)&0xFF)
#define DTR		0x80

/* 7201 status register 0 */

#define INTPNDNG	0x02
#define DCD		0x08
#define BRKABRT		0x80

/* 7201 status register 1 */

#define PARERR		0x10
#define RXOVRUN		0x20
#define CRCFRMER	0x40

/* 7201 status register 2 = int vector */

#define	XMIT		0x00
#define	EXTSTAT		0x01
#define	RECEIVE		0x02
#define	SPECIAL		0x03
#define	INTTYPE		(XMIT|EXTSTAT|SPECIAL|RECEIVE)
#define	CHANNELA	0x04
#define	SR2MASK		(~(XMIT|EXTSTAT|SPECIAL|RECEIVE|CHANNELA)&0xFF)

/* M6821 PIA Register Manipulations */

#define SEL_DATA_DIRECTION	0x00
#define SEL_PERIPH_DATA		0x04

#define OUT_LED_AND_BAUD_JMPRS	0x18
#define OUT_BAUD_SELECT		0xFF

#define FAIL_LED_OFF		0x00		/* 0's whole register!!! */

/************************************************************************
 *
 *	Serial Port Tables
 *
 ************************************************************************/

/*	Initial states for 7201 control registers	*/

MLOCAL PSTATE i_state[] =
{
	0,				/* A cr0 */
	0,				/* A cr1 */
	BOTHINT|PRIRGT|M8086|RTSBP10,	/* A cr2 */
	RXENABLE|RX8BITS,		/* A cr3 */
	SBIT1|CLKX16,			/* A cr4 */
	TXENABLE|TX8BITS,		/* A cr5 */

	0,				/* B cr0 */
	STATAFV,			/* B cr1 */
	0,				/* B cr2 */
	RXENABLE|RX8BITS,		/* B cr3 */
	SBIT1|CLKX16,			/* B cr4 */
	TXENABLE|TX8BITS		/* B cr5 */
};

/* Initial states for port tables */

MLOCAL PGSTBL	igstbl[] = 
{					/* port a */
	PT_TSER,
	PT_SRDYXMIT | PT_SDSR | PT_SDCD,
	PT_B9600,
	PT_M8BIT | PT_MST1,
	PT_CRXEN | PT_CTXEN | PT_CDTR,
	0,
					/* port b */
	PT_TSER,
	PT_SRDYXMIT | PT_SDSR | PT_SDCD,
	PT_B9600,
	PT_M8BIT | PT_MST1,
	PT_CRXEN | PT_CTXEN | PT_CDTR,
	0
};

/*
 *	Stack save area for the asrwait() call...
 *
 *	The save area is dependent on dispatcher stack size,
 *	should be tuned.
 */

#define SAVESTKSZ	400L

MLOCAL BYTE	*stksave[MAXUNITS];

/*
 *	Some things are only done once during first
 *	board initialization.
 */

MLOCAL BOOLEAN	doneonce = FALSE;

/*
 *	Macros to turn interrupts on and off for serial board.
 *
 *	NOTE: I currently block all level six (I/O channel) interrupts,
 *	this is not good, I want to try and block board or unit interrupts
 *	in the future.  This is this way for expediency.
 */

MLOCAL VMECRP	vmecr;

#define blkints()	(vmecr->cr6 &= (BYTE)~CHNEN)
#define unblkints()	(vmecr->cr6 |= CHNEN)

/*
 *	Save of previous vector from setvec(), will be used to chain
 *	interrupt handlers maybe in future.
 */

LONG	oldvector;

/************************************************************************
 *
 *	Serial Port Driver Entry Points
 *
 ************************************************************************/

/************************************************************************
 *
 *	Initialize Serial Port - Synchronous Function
 *
 ************************************************************************/

LONG sp_init(unit)
LONG	unit;		/* contains unit (low order byte) and install flags */
{
	MAPPB		pmap;
	WORD		sp_int();
	WORD		strayintr();
	REG PTTY	*tty;

	/* check for a valid unit number */

	if((BYTE)unit >= MAXUNITS) return(ED_PORT | E_UNIT);

	/*
	 *	Things done only once at initialization.
	 */
	
	if( !doneonce )
	{
		/* get access to VME/10 control registers */
		pmap.pbegaddr = VME10CR;
		pmap.plength = VME10CRSZ;
		vmecr = (VMECRP)mapphys(&pmap);

		/*
		 *	Turn off interrupts just in case, we'll turn
		 *	them on below after we initialize the first board.
		 */

		blkints();

		/* set up serial interrupt vector */
		oldvector = (LONG)setvec(sp_int,(LONG)SERVEC);
		vmecr->cr6 |= INTEN;	/* enable all SCM MPU interrupts */

		doneonce = TRUE;
	}

	/*
	 *	We do the hardware initialization on a per board basis,
	 *	initializing two ports at a time (A and B).
	 *
	 *	We take advantage of the fact here that the units are
	 *	initialized in order, and we allocate the ports (associate
	 *	with the unit number) in order also.
	 */

	if( !(unit & 1) )	/* 1st port initialization for board */
	{
		REG BYTE	*caddra, *caddrb;
		REG SIO 	*sio;
		REG PTTY	*ttyb;

		/* convert physical memory to system memory */
		if( sp_phys[BOARD((BYTE)unit)] != MISSING )
		{
			pmap.mpzero = 0;
			pmap.pbegaddr = (BYTE *)sp_phys[BOARD((BYTE)unit)];
			pmap.plength = SERPORTSZ;
			sp_syst[BOARD((BYTE)unit)] = (SIO *)mapphys(&pmap,MAPDATA);
		}
		else
		{
			sp_syst[BOARD((BYTE)unit)] = MISSING;
			unblkints();
			return(E_SUCCESS);
		}

		/* verify that a 400 card is present for this unit number */
		if( no_device(&(sp_syst[BOARD((BYTE)unit)]->m4_piaac)) )
		{
			sp_syst[BOARD((BYTE)unit)] = MISSING;
			unblkints();
			return(E_SUCCESS);
		}

		/*
		 *	Allocate a tty structure for both ports on
		 *	this board.
		 */

		if( (tty = ttys[(BYTE)unit] = (PTTY *)salloc((LONG)sizeof(PTTY))) == 0L)
			return E_SUCCESS;	/* what error return? */

		if( (ttyb = ttys[(BYTE)unit+1] = (PTTY *)salloc((LONG)sizeof(PTTY))) == 0L)
			return E_SUCCESS;	/* what error return? */

		/* Clear the tty structure */
		_bfill(tty,(WORD)sizeof(PTTY),0);
		_bfill(ttyb,(WORD)sizeof(PTTY),0);

		/* Set initial state of port */
		_bmove(&i_state[0],&tty->t_regst,(WORD)sizeof(PSTATE));
		_bmove(&i_state[1],&ttyb->t_regst,(WORD)sizeof(PSTATE));

		sio = sp_syst[BOARD((BYTE)unit)];
		caddra = &sio->m4_7201c[0];	/* A side */
		caddrb = &sio->m4_7201c[2];	/* B side */

		blkints();	/* block while manipulating port registers */

		/*
		 *	Initialize the 7201 control registers
		 *
		 *	From the NEC 7201 manual we read that we
		 *	should initialize control register 2 first,
		 *	we do both ports first.  We then need to
		 *	initialize control register 4 to set the
		 *	protocol (async).  After that we can initialize
		 *	the remaining registers in any order.
		 */

		/*
		 *	7201 leaves control register 0 selected after
		 *	each operation, so when we write to reset,
		 *	no need for prior select.
		 */

		*caddra = 0;
		*caddra = CHANRST;
		*caddrb = 0;
		*caddrb = CHANRST;

		WRITE(caddra,SELREG2,tty,cr2);
		WRITE(caddrb,SELREG2,ttyb,cr2);

		WRITE(caddra,SELREG4,tty,cr4);
		WRITE(caddra,SELREG1|RSTTXINT,tty,cr1);
		WRITE(caddra,SELREG3,tty,cr3);
		WRITE(caddra,SELREG5,tty,cr5);

		WRITE(caddrb,SELREG4,ttyb,cr4);
		WRITE(caddrb,SELREG1|RSTTXINT,ttyb,cr1);
		WRITE(caddrb,SELREG3,ttyb,cr3);
		WRITE(caddrb,SELREG5,ttyb,cr5);

		/*
		 *	Here we initialize the 6821 PIA on the 400
		 *	card.  The PIA is used to:
		 *
		 *		- Select and Read the Baud Rate Jumpers.
		 *		- Set the Baud Rate.
		 *		- Read the state of or Set the FAIL LED.
		 *		- Interrupt on DSR transition and determine 
		 *		  state of DSR.
		 *
		 *	Port B of the PIA is used as output to set the
		 *	Baud Rate.  We need to configure the Data Direction
		 *	Register (DDR) of the PIA B Peripheral port to be
		 *	output - this is where we set the Baud Rate for each
		 *	port (PB0-PB3 Port 2, PB4-PB7 Port 1).
		 *
		 *	The Port A DDR is used to control the other functions
		 *	supported in the PIA, we select and set the DDR
		 *	for input or output (1 == "bit on" == output).  The
		 *	bits are:
		 *
		 *		0-2	Baud Rate Jumpers
		 *		3	Select for port 1 or 2 jumpers
		 *			(0->port 1, 1->port 2)
		 *		4	Control of FAIL LED (1->on, 0->off)
		 *		5	FAIL LED status (1->on, 0->off)
		 *		6	DSR port 1 status
		 *		7	DSR port 2 status
		 */

		sio->m4_piaac = SEL_DATA_DIRECTION;
		sio->m4_piaad = OUT_LED_AND_BAUD_JMPRS;
		sio->m4_piabc = SEL_DATA_DIRECTION;
		sio->m4_piabd = OUT_BAUD_SELECT;
		sio->m4_piabc = SEL_PERIPH_DATA;
		sio->m4_piaac = SEL_PERIPH_DATA;
		sio->m4_piaad = FAIL_LED_OFF;

		sio->m4_piabd = (BYTE)0xEE;	/* both to 9600 */

		/*
		 *	Do we want to read initial Baud Rate
		 *	from jumpers here???  Someday maybe...
		 */

		unblkints();		/* enable serial interrupts */
	}
	else		/* 2nd Port initialization for board */
	{
		/* We really initialized on init of previous unit */
		if( sp_syst[BOARD((BYTE)unit)] == MISSING )
			return E_SUCCESS;
		tty = ttys[(BYTE)unit];
	}

	/*
	 *	Get transmit system flag.
	 */
	
	if((tty->t_wrflag = flagget()) == E_NO_FLAGS)
		return(ED_PORT | E_NO_FLAGS);
	
	/* Set up initial state of table for this unit */
	_bmove(&igstbl[(BYTE)unit],&tty->t_gstbl,(WORD)sizeof(PGSTBL));

	/* Set up circular queue for writes */
	tty->t_outq = getqueue();

	/* Set up initial character size mask for interrupt masking */
	switch((tty->t_gstbl.pt_mode) & PT_M8BIT)
	{
		case PT_M5BIT : tty->t_charmsk = 0x1F; break;
		case PT_M6BIT : tty->t_charmsk = 0x3F; break;
		case PT_M7BIT : tty->t_charmsk = 0x7F; break;
		case PT_M8BIT : tty->t_charmsk = 0xFF; break;
	}

	/* Get stack save area for asrwait()'s */
	if( (stksave[(BYTE)unit] = (BYTE *)salloc(SAVESTKSZ)) == 0L )
		return E_SUCCESS;	/* What return code???  E_MEM */

	unblkints();		/* Turn on int's just in case */
	return(DVR_PORT);
}

/************************************************************************/
/*	Subdrive - Synchronous Function					*/
/************************************************************************/

LONG sp_subdrv()	/* an argument is ignored here */
{
	return(ED_PORT | E_UNWANTED);
}

/************************************************************************/
/*	Uninitialize Serial Port - Synchronous Function			*/
/************************************************************************/

LONG sp_uninit(unit)
LONG	unit;
{
	PTTY	*tty;

	tty = ttys[unit];

	/* Release system flags for this unit */
	if(flagrel(tty->t_wrflag) < 0L)
		return(ED_PORT | E_CONFLICT);

	/* Release memory for this unit */
	sfree(tty->t_outq);
	sfree(tty);

	return(E_SUCCESS);
}

/************************************************************************/
/*	Select Serial Port - Synchronous Function			*/
/************************************************************************/

LONG sp_select(sp)
PSELPB	*sp;
{
	SIO		*sio;
	REG PTTY	*tty;

	if(( sp->sunit >= MAXUNITS ) || ((sio = sp_syst[BOARD(sp->sunit)]) == MISSING ))
		return(ED_PORT | E_DKATTACH);

	tty = ttys[sp->sunit];		/* get tty struct */

	/* check to see if we were already selected */
	if( tty->t_info & I_SELECTED )
		return E_SUCCESS;	/* return select error??? */

	/* Fetch the keyboard (receiver) ASR to invoke on receiver interrupt */
	tty->t_keybdasr = (LONG)sp->skeybdasr;

	/*
	 *	Initialize salient registers in 7201 and
	 *	set I_SELECTED information bit.
	 *
	 *	We enable receiver interrupt, special receive
	 *	conditions (parity error, framing error...) and
	 *	transmitter interrupts.
	 *
	 *	NOTE: when we handle external/status interrupts
	 *	we will need to or in EXINTEN to cr1.  Used
	 *	for carrier drop handling.
	 *	
	 *	We will also have to write REXSTINT to control
	 *	0 to reset external/status interrupt conditions
	 *	during initialization below.
	 *
	 *	The following code will be used to properly
	 *	set the carrier state:
	 *
	 *	if( sio->m4_7201c[ABSEL(sp->sunit)] & DCD)
	 *		tty->t_state |= PT_SDCD;
	 *	else
	 *		tty->t_state &= ~PT_SDCD;
	 *
	 */

	tty->t_regst.cr5 |= RTS|DTR;
	tty->t_regst.cr1 |= TXINTEN|RXINTALP;
	tty->t_info |= I_SELECTED;		/* our internal state info */

	blkints();
	WRITE(&sio->m4_7201c[ABSEL(sp->sunit)],SELREG5,tty,cr5);
	WRITE(&sio->m4_7201c[ABSEL(sp->sunit)],SELREG1,tty,cr1);
	unblkints();
	return(E_SUCCESS);
}

/************************************************************************/
/*	Flush Serial Port - Synchronous Function			*/
/************************************************************************/

LONG sp_flush(fp)
PFLUPB	*fp;
{
	REG PTTY	*tty;
	REG BYTE	*caddr;

	tty = ttys[fp->funit];

	if( !(tty->t_info & I_SELECTED))
		return E_SUCCESS;

	tty->t_regst.cr5 &= ~(RTS|DTR);
	tty->t_regst.cr1 &= INTDSMSK;

	caddr = &(sp_syst[BOARD(fp->funit)]->m4_7201c[ABSEL(fp->funit)]);

	blkints();
	WRITE(caddr,SELREG5,tty,cr5);
	WRITE(caddr,SELREG1,tty,cr1);
	unblkints();

	tty->t_info &= ~I_SELECTED;
	return(E_SUCCESS);	/* no internal buffers */
}

/************************************************************************/
/*	Read Serial Port - Asynchronous Function			*/
/************************************************************************/

LONG sp_read()
{
	return(ED_PORT | E_IMPLEMENT);
}

/************************************************************************/
/*									*/
/*	Write Serial Port - Asynchronous Function			*/
/*									*/
/*	Our general approach:						*/
/*									*/
/*	We receive characters one by one to write out.			*/
/*	We buffer each character in a circular output queue,		*/
/*	immediately returning E_SUCCESS if our queue is below		*/
/*	the HIWATER mark.						*/
/*									*/
/*	If we hit the high water mark, we pull an asrwait()		*/
/*	to reschedule ourself when we go low water.			*/
/*									*/
/************************************************************************/

LONG sp_write(unit,c)
UBYTE	unit;
UBYTE	c;
{
	REG PTTY	*tty;
	LONG		ret_code;

	ret_code = 0;
	tty = ttys[unit];

	/*
	 *	Keep track of number of characters written after
	 *	we've issued a block.
	 */

	DIAGNOSE(if( tty->t_info & I_QBLOCKED ) tty->t_wrpastblk++);
	
	/*
	 *	CRITICAL REGION:  we manipulate the queue from
	 *	an isr, so we will block interrupts.
	 */

	blkints();
	enqueue(tty->t_outq,c);

	/*
	 *	Have we already enabled transmitter interrupts
	 *	for this port?  If not (i.e. queue was empty,
	 *	now we have a character), then enable.
	 *
	 *	We need only write the character
	 *	to the 7201 data port.  The transmit interrupt is ready
	 *	(initialized in select, and reset at each transmitter interrupt)
	 *	and the transition of the character from the data register
	 *	to the serial output register will immediately generate an
	 *	interrupt.  But, we're blocking, remember?
	 */

	if( !(tty->t_info & I_TXSTARTED) )
	{
		tty->t_info |= I_TXSTARTED;
		sp_syst[BOARD((BYTE)unit)]->m4_7201d[ABSEL((BYTE)unit)] = dequeue(tty->t_outq);
		DIAGNOSE(tty->t_wr_count++);
	}

	if( qhiwater(tty->t_outq) && !(tty->t_info & I_QBLOCKED) )
	{
		DIAGNOSE(tty->t_blocks++);
		tty->t_info |= I_QBLOCKED;
		tty->t_caller = (LONG)*sp_hdr.dh_curpd;
		flagclr(tty->t_wrflag);
		ret_code = flagevent(tty->t_wrflag,0L);
		unblkints();
		asrwait(ret_code,stksave[(BYTE)unit]);
		return (E_SUCCESS);
	}

	unblkints();
	return (E_SUCCESS);
}


/************************************************************************/
/*	Get Serial Port Information - Synchronous Function		*/
/************************************************************************/

LONG sp_get(get_pb)
PGSPB	*get_pb;
{
	REG PGSTBL 	*bp;
	LONG		b_size;
	LONG		a_size;
	PGSTBL 		*t_pntr;
	BYTE		i;

	/* Set up pointers with info from the pb */
	bp = get_pb->gsbufadr;
	b_size = get_pb->gsbufsiz;
	t_pntr = &(ttys[get_pb->gsunit]->t_gstbl);

	/* Check for a bp in user space */
	if (get_pb->gsflags & USERBUF)
	{
		mapu(get_pb->gspdaddr);
		bp = (PGSTBL *)saddr(bp);
	}

	/* Check bp size and get port type */
	if(b_size >= sizeof(bp->pt_type)) 
  	{
	  bp->pt_type = t_pntr->pt_type;

	/* Get the port state */
	  if((b_size -= sizeof(bp->pt_type)) >= sizeof(bp->pt_state))
	  {
	    bp->pt_state = t_pntr->pt_state;

	/* Get the port baud rate */
	    if((b_size -= sizeof(bp->pt_state)) >= sizeof(bp->pt_baud))
	    {
	      bp->pt_baud = t_pntr->pt_baud;

	/* Get the port modem control */
	      if((b_size -= sizeof(bp->pt_baud)) >= sizeof(bp->pt_mode))
	      {
	        bp->pt_mode = t_pntr->pt_mode;

	/* Get the port format control */
	        if((b_size -= sizeof(bp->pt_mode)) >= sizeof(bp->pt_control))
	  	{
	          bp->pt_control = t_pntr->pt_control;

	/* Get the port fill byte */
	          if((b_size -= sizeof(bp->pt_control)) >= sizeof(bp->pt_1fill))
	    	           bp->pt_1fill = t_pntr->pt_1fill;
	  	}
	      }
	    }
	  }
	}

	if (get_pb->gsflags & USERBUF)
        	unmapu();

	return(E_SUCCESS);
}


/************************************************************************/
/*	Set Serial Port Information - Synchronus Function		*/
/*									*/
/*	We must reflect changes made to the table in the hardware.	*/
/*									*/
/************************************************************************/

LONG sp_set(set_pb)
PGSPB	*set_pb;
{
	REG PGSTBL	*bp;
	LONG		b_size;
	LONG		a_size;
	PGSTBL		*tbl;
	BYTE		i;
	REG PTTY	*tty;
	REG SIO		*sio;
	REG UBYTE	*caddr;
	REG UBYTE	baud, baudreg, charsize;

	/* Set up pointers with info from the pb */
	b_size = set_pb->gsbufsiz;
	tbl = &(ttys[set_pb->gsunit]->t_gstbl);
	tty = ttys[set_pb->gsunit];
	sio = sp_syst[BOARD(set_pb->gsunit)];
	caddr = &sio->m4_7201c[ABSEL(set_pb->gsunit)];

	/* Check for a bp in user space */
	bp = set_pb->gsbufadr;
	if (set_pb->gsflags & USERBUF)
	{
		mapu(set_pb->gspdaddr);
		bp = (PGSTBL *)saddr(bp);
	}

	/*
	 *	I apologize for the ugly set of parentheses
	 *	and if's below.  We only can copy whole
	 *	fields (by Concurrent definition) and may receive
	 *	a partial table.  You consider the options,
	 *	blame Michael Krutz for the code below.
	 */

	/* Check bp size and set port type */
	if(b_size >= sizeof(bp->pt_type)) 
	{
		/* Cannot set port type */

	/* Set the port state */
	if((b_size -= sizeof(bp->pt_type)) >= sizeof(bp->pt_state))
	{
		/* Cannot set port state */

	/* Set the port baud rate */
	if((b_size -= sizeof(bp->pt_state)) >= sizeof(bp->pt_baud))
	{
		tbl->pt_baud = bp->pt_baud;
		blkints();
		if (set_pb->gsunit & 1)
			sio->m4_piabd = (sio->m4_piabd & 0xF0)
				| bp->pt_baud;			/* B port */
		else
			sio->m4_piabd = (sio->m4_piabd & 0x0F)
				| (bp->pt_baud << 4);		/* A port */
		unblkints();

	/* Set the port mode control */
	if((b_size -= sizeof(bp->pt_baud)) >= sizeof(bp->pt_mode))
	{
	        tbl->pt_mode = bp->pt_mode;

		/* Now set the memory register images of 7201 chip */
		/* set character size */
		if( (bp->pt_mode & PT_M8BIT) == PT_M8BIT )
			{ charsize = TX8BITS; tty->t_charmsk = 0xFF; }
		else if( bp->pt_mode & PT_M7BIT )
			{ charsize = TX7BITS; tty->t_charmsk = 0x7F; }
		else if( bp->pt_mode & PT_M6BIT )
			{ charsize = TX6BITS; tty->t_charmsk = 0x3f; }
		else					/* PT_M5BIT */
			{ charsize = TX5BITS; tty->t_charmsk = 0x1F; }
		tty->t_regst.cr5 = (tty->t_regst.cr5 & TXSZMSK) | charsize;
		tty->t_regst.cr3 = (tty->t_regst.cr3 & RXSZMSK) | (charsize << 1);

		/* set number of stop bits */
		tty->t_regst.cr4 &= SBITMSK;		/* first clear stop bit field */
		if( (bp->pt_mode & PT_MST2) == PT_MST2 )
			tty->t_regst.cr4 |= SBIT2;
		else if( bp->pt_mode & PT_MST1 )
			tty->t_regst.cr4 |= SBIT1;
		else
			tty->t_regst.cr4 |= SBIT1P5;

		/* set up parity if any */
		if( bp->pt_mode & PT_MENPR )
		{
			tty->t_regst.cr4 |= PARENAB;
			if( bp->pt_mode & PT_MEVPR )
				tty->t_regst.cr4 |= EVENPAR;
			else
				tty->t_regst.cr4 &= ~EVENPAR;
		}
		else
			tty->t_regst.cr4 &= ~PARENAB;

	/* Set the port format control */
	if((b_size -= sizeof(bp->pt_mode)) >= sizeof(bp->pt_control))
	{
		tbl->pt_control = bp->pt_control;
		if( bp->pt_control & PT_CRXEN )		/* enable receiver? */
			tty->t_regst.cr3 |= RXENABLE;
		else
			tty->t_regst.cr3 &= ~RXENABLE;
		
	} } } } }	/* Ugly set of parentheses, aren't they? */

	/*
	 *	Now we have updated the table values
	 *	stored in the driver, we need to actually 
	 *	tweak the hardware.
	 */

	blkints();
	WRITE(caddr,SELREG4,tty,cr4);
	WRITE(caddr,SELREG1,tty,cr1);
	WRITE(caddr,SELREG3,tty,cr3);
	WRITE(caddr,SELREG5,tty,cr5);
	unblkints();

	if (set_pb->gsflags & USERBUF)
        	unmapu();

	return(E_SUCCESS);
}


/************************************************************************/
/*	Special Operation - Synchronous Function			*/
/************************************************************************/

/*
 *	Kinda kludgey - the console and serial drivers call
 *	this special entry to get/set the port table.
 *	We must support it.
 */

#define PGET	0x13
#define PSET	0x93

LONG sp_special(pb)
REG DPBLK *pb;
{
	if( (pb->dp_option & 0x7f) == 0x13 )
	{
		pb->dp_buffer = (BYTE *)pb->dp_offset;
		pb->dp_bufsiz = pb->dp_delim;
	}

	switch( pb->dp_option & 0x00FF)
	{
	case PGET:
		sp_get(pb);
		return E_SUCCESS;
	case PSET:
		sp_set(pb);
		return E_SUCCESS;
	default:
		return(ED_PORT | E_IMPLEMENT);
	}
}

 
/************************************************************************
 *
 *	Support Routines
 *
 ************************************************************************/

/************************************************************************
 *
 *	Serial Interrupt Service Routine
 *
 *	This interrupt routine is invoked on all serial interrupts.
 *
 *	It may also be invoked by other devices which may be
 *	interrupting on the same vector.  So we need to scan
 *	each port and determine if there are any generating an
 *	interrupt, and we need to consider two types of interrupts:
 *	receiver interrupts and transmitter interrupts.
 *
 *	Now, receiver interrupts are completely asynchronous to
 *	the system (e.g. someone typing on a terminal) and data
 *	received is handed off to the keyboard ASR gotten in select()
 *	and stored in the tty structure.
 *
 *	A short comment on critical regions: we avoid some of the issues
 *	of critical regions in the manipulation of the queues.  Obviously,
 *	we must assure that two processes do not modify a queue (nearly)
 *	simultaneously, screwing up our counter, head and tail pointers.
 *	We assure this by masking interrupts at this point.  Another
 *	way would be to relegate all such operations to ASR land.
 *
 ************************************************************************/

#if ASMSPINT
/* see serint.s for this routine */
#else
typedef	WORD	(*RPTR)();

WORD sp_int()
{
	REG PTTY	*tty;
	REG SIO		*sio;
	REG UBYTE	*caddr, *daddr;
	REG UBYTE	s0, s1, s2;
	REG UBYTE	unit;

	for (unit = 0; unit < MAXUNITS; unit += 2)	/* for all boards */
	{
		if ((sio = sp_syst[BOARD(unit)]) == MISSING)
			continue;

		/* for both ports on board, reset intr at bottom of loop */
		for (;;sio->m4_7201c[0] = EOINT)
		{
			REG UBYTE	u,c;

			/* Get vector to determine interrupt source and type */
			sio->m4_7201c[2] = SELREG2;
			s2 = sio->m4_7201c[2];

			/* After a fetch of s2, INTPNDNG becomes valid */
			if (!((s0 = sio->m4_7201c[0]) & INTPNDNG))
				break;		/* done for this board */
	
			if( s2&CHANNELA )	/* Channel A Interrupt */
			{
				daddr = &sio->m4_7201d[0];
				caddr = &sio->m4_7201c[0];
				u = unit;
				tty = ttys[u];
			}
			else			/* Channel B Interrupt */
			{
				daddr = &sio->m4_7201d[2];
				caddr = &sio->m4_7201c[2];
				u = unit + 1;
				tty = ttys[u];
			}
	
			switch( s2 & INTTYPE )
			{
			case XMIT:			/* XMIT buffer empty intr */
				*caddr = RSTTXINT;	/* reset XMIT interrupt */
				if( !qempty(tty->t_outq) )
				{
					*daddr = dequeue(tty->t_outq);
					DIAGNOSE(tty->t_wr_count++);

					/*
					 *	If we previously blocked ourselves upon
					 *	hitting hi water, should we now unblock
					 *	ourselves (are we at low water)?
					 */

					if( (tty->t_info & I_QBLOCKED) && qlowater(tty->t_outq) )
					{
						tty->t_info &= ~I_QBLOCKED;
						flagset(tty->t_wrflag,tty->t_caller,0L);
					}
				}
				else
					tty->t_info &= ~I_TXSTARTED;
				break;
	
			case EXTSTAT:	/* External/Status interrupt */
				/* NOT IMPLEMENTED YET */
				break;
	
			case RECEIVE:	/* Received character available intr */
				/* ASSUME FOR NOW XON/XOFF DONE BY PARENT */
				DIAGNOSE(tty->t_rd_count++);
				(*(RPTR)tty->t_keybdasr)((LONG)(*daddr & tty->t_charmsk),(LONG)u);
				break;
	
			case SPECIAL:	/* Special receive condition interrupt */

				/*
				 *	We get the error info, fetch the character
				 *	which caused the error and reset the error
				 *	condition.
				 *
				 *	For now, we just log the errors.
				 *
				 *	NOTE: break is an "external/status" interrupt.
				 */

				*caddr = SELREG1;
				s1 = *caddr;
				c = *daddr;
				*caddr = ERRRST;
				DIAGNOSE(tty->t_rd_errs++);
				break;

			} /* end of switch statement */
		} /* end of for loop for single board interrupt processing */
	}  /* end of for loop for polling boards */

	/*
	 *	For interrupt chaining, we will call "oldvector"
	 *	as a function if it was non-zero when we did
	 *	the setvec() in init().
	 *
	 *	if( oldvector )
	 *		(*(RPTR)oldvector)();
	 *
	 */
	
	return 0;	/* don't force dispatches */
}
#endif
