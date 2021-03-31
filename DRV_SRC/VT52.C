/*	@(#)vt52.c	1.2		*/
/*
 * WHO	WHEN		WHAT
 * ===  ====		====
 * DRK	11/11/85	Fixed lots of things in the trt table.
 *
 *	DEC VT-52 Terminal Subdriver for Concurrent DOS-68K.
 *
 *	Should work on a VT-52 look-alike also.
 *
 *	NOTES ON MODIFYING TO SUPPORT A NEW TERMINAL:
 *
 *	You will likely have to modify three areas to support
 *	a new terminal:
 *
 *		1. You will have to modify t_8to16()
 *		   to translate 8 Bit characters from the
 *		   keyboard to 16 Bit characters which are
 *		   passed to the Console Resource Manager
 *		   via the Keyboard ASR acquired in select().
 *		   Easy to implement as a simple state machine,
 *		   escstate[] exists to store the state.
 *		2. trt[] array will have to modified to translate
 *		   16 Bit characters passed through twrite()
 *		   to appropriate escape sequence (or simple character).
 *		   May require some in-line code in twrite()
 *		   (as in Insert Character Mode Emulation on TVI-970
 *		   driver).
 *		3. You have to implement a goto (x,y) call in tspecial().
 *
 *	To give you maximum flexibility and room for optimization,
 *	the TGOTO entry point and twrite() are both passed a pointer
 *	to the vcid for the physical image of the serial console.
 *	This should allow some fancy stuff if need be.  It is the
 *	current physical image (before the change you are trying to make).
 */

#include "portab.h"
#include "io.h"
#include "system.h"
#include "vcdrv.h"
#include "vterm.h"
#include "vchar.h"

/*
 *	Local flag to include integer to ASCII string conversion
 *	routine for use by goto(x,y) code.
 *
 *	Not needed for VT-52 type terminals where it is the numeric
 *	integer value plus BLANK (0x20) in the ESCAPE string.
 */

#define NUMERICGOTO	0

/************************************************************************/
/* forward referenced functions                                         */
/************************************************************************/

MLOCAL LONG
	tinit(),
	tsubdrvr(),
	tuninit(),
	tselect(),
	tflush(),
	tread(),
	twrite(),
	tget(),
	tset(),
	tspecial();

/*
 *      Driver header
 *
 *	This header must be the first structure in the data segment
 *	so, don't put any data generating code before this section
 */

DH  t52dh =
{
	DVR_TERM,MAXTUNIT,0,
	tinit,
	tsubdrvr,
	tuninit,
	tselect,
	tflush,
	tread,
	twrite,
	tget,
	tset,
	tspecial,
	0L,0L,0L,0L,0L
};

typedef VOID	(*PINPTR)();

MLOCAL PINPTR	tkb_pin[MAXTUNIT];

MLOCAL BYTE	escstate[MAXTUNIT];	/* 8 bit to 16 bit conversion state variable */
MLOCAL BYTE	td_units[MAXTUNIT];	/* terminal unitno indexed by sub-driver's */
MLOCAL BYTE	sd_units[MAXTUNIT];	/* sub-driver's unitno indexed by drvr_unitno */
MLOCAL DH	*sd_hdr[MAXTUNIT];	/* stores drvr headers of sub-drivers   */
MLOCAL LONG	pcon[MAXTUNIT];		/* stores Physical Console ID for R-M */

MLOCAL UWORD	t_8to16();
EXTERN VOID	vbfill();
EXTERN LONG	e_timer();

/*
 *	t_ewrite()
 *
 *	Write out a buffer for the corresponding terminal unit.
 *	The buffer is a `C' null terminated string.
 *
 *	Called from the twrite() and tspecial() (goto) entry points.
 *
 *	THOUGHT: since it is null terminated, we will never be able
 *	to send a '\0', unless we implement an escaping mechanism.
 *	I don't know of a terminal that uses a null....
 *
 *	APOLOGIES:
 *
 *	In some ways, the below code is hokey.  I wrote the port
 *	driver to have a slack amount of characters beyond an event
 *	return, so when I'm in the middle of the escape sequence and
 *	I get an event to wait for (when my output queue fills), I
 *	can send the rest of the escape sequence and write beyond the
 *	event.  I'm counting on some mutual exclusion properties
 *	of being in an ASR so I don't get TWO events for one buffer
 *	write.  The better alternative would be to take a two tiered
 *	approach to events and to chain (come to think of it, three
 *	tiered approach) my buffers in t_ewrite(), get an event to return
 *	to write entry of console driver, and wait to finish by buffer
 *	write on the event returned from the port driver.
 *
 *	Not very hard, but I didn't have time to finish it right.
 *	This solution of slack is workable, BUT if you don't know
 *	what the slack is and try writing a hideously long buffer,
 *	you could miss sending some characters because of queue overflow.
 *
 */

MLOCAL LONG t_ewrite(unit,bp)
BYTE		unit;
REG BYTE	*bp;
{
	LONG	retcode;
	LONG	event;

	retcode = 0;
	while( *bp )
	{
		/*
		 *	We should never get two event returns!
		 *	We have a certain amount of slack built into
		 *	the serial port subdriver, allowing us to
		 *	enqueue N characters past an event.
		 *	I hope you N is big enough.
		 */

		if( event = (*sd_hdr[unit]->dh_write)(sd_units[unit],*bp++) )
		{
			if( !retcode )
			{
				retcode = event;
				/* if we get an error, return immediately */
				if( retcode < 0 )
					break;
			}
		}
	}
	return retcode;
}

/*
 *	This is the ASR called from below as the keyboard ASR.
 *	It will call the 8 to 16 bit character translation routine
 *	enabled for the port and pass the 16 bit character (if there
 *	is one) to the Console Resource Manager's keyboard ASR passed
 *	in to us in tselect() through the console driver.
 */

MLOCAL LONG tserxlat(ch,portunit)
LONG	ch, portunit;
{
	UWORD	ch16;
	BYTE	tdunit;

	tdunit = td_units[(BYTE)portunit];
	if ( ch16 = t_8to16((BYTE)ch,&escstate[tdunit]) )
		(*tkb_pin[tdunit])(pcon[tdunit],(LONG)ch16);
}

/*
 *	This routine is called from ISR and must get us into
 *	ASR context, because the above routine and the subsequent
 *	resource manager call will last much longer then ISR should.
 */

MLOCAL LONG tserkbasr(ch,portunit)
LONG	ch, portunit;
{
	doasr(tserxlat,ch,portunit,200);
}

/********* terminal specific data and code *********/

/*
 *	Special editing character translation table:
 *	You'll notice, upon careful inspection that the
 *	low byte of the 20xx 16 bit special characters
 *	IS the character to send following an ESC character
 *	to generate a VT-52 edit escape sequence.  Super convenient
 *	for the VT-52, requires translation for other terminals
 *	into an appropriate escape sequence.
 *
 *	NOTE: to save space, table is based at 0x40,
 *	and indexed by (low byte of 16 bit character - 0x40).
 *
 *	We map all undefined strings to the nullstr[].
 */

MLOCAL BYTE	nullstr[] = "";	/* save a little storage by having one nullstr */

#define NULSTR	nullstr		/* indicates sequences I still need to consider */

/*
 *	A terminal may require setting to a known state,
 *	which should correspond to the initial state set
 *	for the console in the console driver.
 *	It should be performed in select, I would think.
 *
 *	Switching virtual consoles is a problem when you consider
 *	the state factors.  You can obviously have completely
 *	different states for each console (wrap/nowrap etc.).
 *	I'm not dealing with these issues now.
 */

/*
 *	Initial VT-52 state:
 *
 *	- Try to put terminal into ANSI mode first (sorry mom).
 *	- Enable VT-52 Mode (for look-alikes, we'll try it)
 *	- Enable alternate key pad mode
 *	- Turn Wrap Off (discard at end of line).
 *
 *	SPECIAL SUPPORT FOR VT-52 LOOK-ALIKES;
 *
 *	Try to send out the code for a VT-100 type terminal with
 *	alternate VT-52 support to push it into VT-52 Mode.
 *	We can only try...
 *
 *	APOLOGIES:
 *
 *	Visual Attributes are not supported in this terminal
 *	driver (nor are they supported for COPY operations in the
 *	Console Driver as of July, 1985).
 */

/*	THIS INITIALIZATION MAY HAVE TO BE TUNED FOR YOUR VT52 LOOKALIKE! */
#if 1
MLOCAL BYTE	tinitstr[] = "\033<\033[?2l\033=\033w";
#else
MLOCAL BYTE	tinitstr[] = "";
#endif


#define TBLTOP	0x77
#define TBLBASE	0x40

MLOCAL BYTE	*trt[] =
{
	"\033@",	/* 0x40 - Enter insert Character Mode */
	"\033A",	/* 0x41 - Cursor up */
	"\033B",	/* 0x42 - Cursor down */
	"\033C",	/* 0x43 - Cursor right */
	"\033D",	/* 0x44 - Cursor left */
	"\033E",	/* 0x45 - Clear display */
	nullstr,	/* 0x46 */
	nullstr,	/* 0x47 */
	"\033H",	/* 0x48 - Cursor home */
	"\033I",	/* 0x49 - Reverse index */
	"\033J",	/* 0x4a - Erase to end of page */
	"\033K",	/* 0x4b - Erase to end of line */
	"\033L",	/* 0x4c - Insert blank line */
	"\033M",	/* 0x4d - Delete line */
	"\033N",	/* 0x4e - Delete character */
	"\033O",	/* 0x4f - Exit insert character mode */
	nullstr,	/* 0x50 */
	nullstr,	/* 0x51 */
	nullstr,	/* 0x52 */
	nullstr,	/* 0x53 */
	nullstr,	/* 0x54 */
	nullstr,	/* 0x55 */
	nullstr,	/* 0x56 */
	nullstr,	/* 0x57 */
	nullstr,	/* 0x58 */
	nullstr,	/* 0x59 */
	nullstr,	/* 0x5a */
	nullstr,	/* 0x5b */
	nullstr,	/* 0x5c */
	nullstr,	/* 0x5d */
	nullstr,	/* 0x5e */
	nullstr,	/* 0x5f */
	nullstr,	/* 0x60 */
	nullstr,	/* 0x61 */
	nullstr,	/* 0x62 */
	nullstr,	/* 0x63 */
	"\033b",	/* 0x64 - Erase to beginning of display */
	"\033y5",	/* 0x65 - Enable cursor - Blinking block cursor */
	"\033x5",	/* 0x66 - Disable cursor */
	nullstr,	/* 0x67 */
	nullstr,	/* 0x68 */
	nullstr,	/* 0x69 */
	"\033j",	/* 0x6a - Save cursor position */
	"\033k",	/* 0x6b - Restore cursor position */
	"\033l",	/* 0x6c - Erase entire line */
	nullstr,	/* 0x6d */
	nullstr,	/* 0x6e */
	"\033o",	/* 0x6f - Erase to beginning of line */
	"\033p",	/* 0x70 - Enter reverse video mode */
	"\033q",	/* 0x71 - Exit reverse video mode */
	"\033s4",	/* 0x72 - Enter intensify mode */
	"\033s2",	/* 0x73 - Enter blink mode */
	"\033s0",	/* 0x74 - Exit blink mode */
	"\033s0",	/* 0x75 - Exit intensify mode */
	"\033v",	/* 0x76 - Wrap at end of line */
	"\033w"		/* 0x77 - Discard at end of line */
};

#if	NUMERICGOTO

/*
 *	We assume exclusion because we should be in ASR land.
 *	So I think we can get away with the static local buffer,
 *	which is only used internally to this C module.
 */

MLOCAL BYTE	itb[12];
MLOCAL WORD	iti;

MLOCAL VOID sitoa(n)
WORD	n;
{
	if( n <= 0 )
		return;
	sitoa(n/10);
	itb[iti++] = '0' + (n % 10);
}

MLOCAL BYTE	*itoa(n)
WORD	n;
{
	iti = 0;

	sitoa(n);
	if( iti == 0 )
		itb[iti++] = '0';	/* handle zero special */
	itb[iti] = '\0';

	return itb;
}

#endif

/*
 *	The terminal subdriver write entry point.
 *
 *	In matching the VT-52 modes, some simulation of a hardware
 *	feature may be required.
 *
 *	Here we need to simulate the insert character mode for normal
 *	(not special) characters.  We do this by checking the mode
 *	in the "v" vcblk, and issuing an insert of one character.
 *	We return this along with the the character.
 */

MLOCAL LONG	twrite(unit,v,c16)
BYTE		unit;
REG VCBLK	*v;
REG UWORD	c16;
{
	BYTE	s[20];		/* should be enough to cover largest esc seq. */
	UBYTE	c8;

	c8 = (UBYTE)c16;
	strcpy(s,nullstr);

	if( c16 & 0xFF00 )	/* if a SPECIAL character */
	{
		if ( c8 >= TBLBASE && c8 <= TBLTOP )
			strcpy(s,trt[c8-TBLBASE]);
	}
	else			/* it is a simple, 8 bit character */
	{
		WORD	n;
	
		s[0]= '\0';			/* initially a null string */
#if 0
		if( c8 <= '~' && c8 >= ' ' )
				if( v->v_mode & M_INSERTC )
					strcpy(s,"\033[1@");
	/* ANSI insert one character mode when in vt52 mode is silly */
#endif
		s[n = strlen(s)] = c8;
		s[n + 1] = '\0';
	}
	return t_ewrite(unit,s);
}

/*
 *	t_8to16()
 *
 *	Convert an 8-bit character into a 16-bit character.  The
 *	routine converts 8-bit character escape sequences to a 16-bit
 *	character, "state" is the state variable, which is associated
 *	with each port.
 *
 *	Concerning the state info of the 8 to 16 bit character conversion:
 *
 *	We use the state info to decode escape sequences into 16 BIT
 *	characters for Concurrent DOS.  The state is used directly
 *	as a case in the switch statement.
 *
 *	KEYS NOT DEFINED:
 *
 *		IC_PRSCR
 *		IC_BREAK	0x05
 *		IC_REDRAW	0x06
 *		IC_BEGIN	0x07
 *		IC_END		0x08
 *		IC_INSERT	0x09
 *		IC_DELETE	0x0A
 *		IC_SYSREQ	0x0B
 *		IC_PAGEUP	0x14
 *		IC_PAGEDN	0x15
 *		IC_PAGELT	0x16
 *		IC_PAGERT	0x17
 *		IC_HOME		0x18
 *		IC_REVTAB	0x19
 *		IC_A		0x3A
 *		IC_B		0x3B
 *		IC_C		0x3C
 *		IC_D		0x3D
 *		IC_E		0x3E
 *		IC_F		0x3F
 *		IC_PLUS		0x44
 *		IC_DIVIDE	0x45
 *		IC_MULTIPLY	0x46
 *		IC_EQUAL	0x47
 */

GLOBAL	BYTE	stksave[128];

MLOCAL UWORD	t_8to16(ch8,state)
UBYTE	ch8;
UBYTE	*state;
{
	UWORD	ch16;
	LONG	emask;
	
	ch16 = 0;

	switch(*state)		/* use state machine */
	{
	case 0:
		if (ch8 != ESCAPE) ch16 = (UWORD) ch8;
		  else
		    {	/* got an excape char we wait to see if the */
			/* next character comes within 1/10 sec	    */
			*state = 1;
			emask = e_timer(0L,0,100L);
			asrwait(emask,stksave);
			if (*state == 1)
			  {
			    ch16 = 0x1B;
			    *state = 0;
			  }
		    }
		break;
	case 1:
		*state = 0;	/* single ESCAPE */
		switch(ch8)
		{
		case ESCAPE:
			ch16 = ESCAPE;
			*state = 1;
			break;
		case 'O':
			*state = 3;
			break;
		case '?':
			*state = 2;
			break;
		case 'A':
			ch16 = IC_SPECIAL | IC_UP;
			break;
		case 'B':
			ch16 = IC_SPECIAL | IC_DOWN;
			break;
		case 'C':
			ch16 = IC_SPECIAL | IC_RIGHT;
			break;
		case 'D':
			ch16 = IC_SPECIAL | IC_LEFT;
			break;
		case 'P':
			ch16 = IC_SPECIAL | IC_WINDOW;
			break;
		case 'Q':
			ch16 = IC_SPECIAL | IC_NEXT;
			break;
		case 'R':
			ch16 = IC_SPECIAL | IC_PREVIOUS;
			break;
		case 'S':
			ch16 = IC_SPECIAL | IC_HELP;
			break;
		}
		break;

	case 2:	
		*state = 0;	/* ESCAPE ? x */
		switch(ch8)
		{
		case 'p':
			ch16 = IC_SPECIAL | IC_ZERO;
			break;
		case 'q':
			ch16 = IC_SPECIAL | IC_ONE;
			break;
		case 'r':
			ch16 = IC_SPECIAL | IC_TWO;
			break;
		case 's':
			ch16 = IC_SPECIAL | IC_THREE;
			break;
		case 't':
			ch16 = IC_SPECIAL | IC_FOUR;
			break;
		case 'u':
			ch16 = IC_SPECIAL | IC_FIVE;
			break;
		case 'v':
			ch16 = IC_SPECIAL | IC_SIX;
			break;
		case 'w':
			ch16 = IC_SPECIAL | IC_SEVEN;
			break;
		case 'x':
			ch16 = IC_SPECIAL | IC_EIGHT;
			break;
		case 'y':
			ch16 = IC_SPECIAL | IC_NINE;
			break;
		case 'l':
			ch16 = IC_SPECIAL | IC_COMMA;
			break;
		case 'M':
			ch16 = IC_SPECIAL | IC_ENTER;
			break;
		case 'm':
			ch16 = IC_SPECIAL | IC_MINUS;
			break;
		case 'n':
			ch16 = IC_SPECIAL | IC_PERIOD;
			break;
		}
		break;
	}
	return(ch16);
}

/******** Main Entry Points for Terminal SubDriver **********/

/*
 *      tinit
 */

MLOCAL LONG    tinit(unitno)
LONG	unitno;
{
	CDMAPPB	mpb;		/* for making absolute memory address ours */

	if ((BYTE)unitno >= MAXTUNIT) return( ED_TERM | E_UNITNO );

	/* we need a PORT subdriver */
	return( ((LONG) DVR_PORT << 16) + DVR_TERM );
}

/*
 *	tsubdrvr
 *
 *	The terminal sub-drive entry is called after the initialization
 *	routine tells install it needs subdriver's.  We must store
 *	the sub-driver driver header address and associate it with
 *	the terminal unit by storing the sub-drive unit number (port driver)
 *	in an array which is indexed by the terminal unit number.
 *	We also do a vice-versa to be able to figure out which terminal
 *	unit is associated with a given subdriver unit number.  You must
 *	have guessed by now that the sub-driver unit number does not
 *	necessarily correspond to the terminal unit number.
 *
 *	We ignore the access rights of the sub-driver.
 */

MLOCAL LONG    tsubdrvr(pb)
REG SDPB     *pb;
{
	sd_units[pb->sd_unit] = pb->sd_sdunit;	/* sub-drvr unitno */
	sd_hdr[pb->sd_unit] = pb->sd_sdheader;	/* header address */
	td_units[pb->sd_sdunit] = pb->sd_unit;	/* drvr unitno */

	return(E_SUCCESS);
}

/************************************************************************/
/*      tuninit								*/
/************************************************************************/

MLOCAL LONG    tuninit(unitno)
BYTE    unitno;
{
	DPBLK	upb;

	upb.dp_unitno = unitno;
	return( tflush(&upb) );
}

/************************************************************************/
/*      tread								*/
/************************************************************************/

MLOCAL LONG    tread()
{
	return( ED_CON | E_IMPLEMENT );
}

/************************************************************************/
/*      tselect                                                        */
/************************************************************************/

MLOCAL LONG    tselect(pb)
REG CDSELECT	*pb;
{
	LONG	r;
	BYTE	unit;

	unit = pb->unitno;
	tkb_pin[unit] = (PINPTR) pb->kbd_pin;
	pcon[unit] = pb->pconid;

	/*
	 *	Call the sub-drivers select routine.  NOTE: we patch
	 *	serial port drivers to call our terminal specific PIN
	 *	routine.
	 *
	 *	This routine will asynchronously pass the input characters
	 *	to the real PIN routine passed to us from the Resource
	 *	Manager through the console driver.
	 *
	 *	We do this because we need to translate
	 *	the 8 bit character input to 16 bit characters (handling
	 *	escape sequences of course.)
	 */

	pb->kbd_pin = (LONG)tserkbasr;
	pb->unitno = sd_units[unit];
	r = (*sd_hdr[unit]->dh_select)(pb);
	pb->unitno = unit;

	if( r < 0 )
		return r;

	/*
	 *	We may have to send an initialization sequence
	 *	to try to get terminal into a nice mode we can
	 *	deal with.  Try to send out the initialization
	 *	string.
	 */
	
	t_ewrite(unit,tinitstr);

	return E_SUCCESS;
}

/*
 *      tflush
 */


MLOCAL LONG    tflush(pb)
DPBLK     *pb;
{
	LONG	r;
	BYTE	unit;

	unit = pb->dp_unitno;
	pb->dp_unitno = sd_units[unit];
	r = (*sd_hdr[unit]->dh_flush)(pb);
	pb->dp_unitno = unit;
	return r;
}

/*
 *      tget
 */

MLOCAL LONG	tget(pb)
REG DPBLK	*pb;
{
	LONG	r;
	BYTE	unit;

	unit = pb->dp_unitno;
	pb->dp_unitno = sd_units[unit];
	r = (*sd_hdr[unit]->dh_get)(pb);
	pb->dp_unitno = unit;
	return r;
}

/*
 *      tset
 */

MLOCAL LONG	tset(pb)
REG DPBLK	*pb;
{
	LONG	r;
	BYTE	unit;

	unit = pb->dp_unitno;
	pb->dp_unitno = sd_units[unit];
	r = (*sd_hdr[unit]->dh_get)(pb);
	pb->dp_unitno = unit;
	return r;
}


/*
 *      tspecial
 *
 *	Special functions for the terminal subdriver.
 */

MLOCAL LONG	tspecial(unit,function,arg0,arg1,arg2)
BYTE	unit;
WORD	function;
LONG	arg0;
LONG	arg1;
LONG	arg2;
{
	switch( function )
	{
	case TGOTO:
	{
		BYTE	s[5];
	
		/*
		 *	Construct and emit the terminal dependent
		 *	goto (x,y) sequence.
		 *
		 *	For now, no optimizations, force to row and
		 *	column passed in arg1 and arg2, respectively.
		 *	
		 *	VT-52 cursor addressing:
		 *
		 *	HOME is (1,1), and we have to bias the values
		 *	with 0x20.
		 *
		 *	Later, would be nice to write generic substitution
		 *	subroutine, for use by other terminal handlers.
		 */
		
		strcpy(s,"\033Ylc");
		s[2] = BLANK + (WORD)arg1;	/* line */
		s[3] = BLANK + (WORD)arg2;	/* column */
		return t_ewrite(unit,s);
	}

	default:
		return E_SUCCESS;	/* only one function in special now */
	}
}

                                                                                                                            
                                                                                                                            