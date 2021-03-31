/****************************************************************
*	VMCLK.C
*
*	This file contains the routines required to 		*
*	manipulate the Motorola M146818 hardware clock		*
*	on the VME/10.						*
*								*
*	For Concurrent DOS 4.0 - bjp				*
*								*
*	- Does not take advantage of any Time of Day features	*
*	  simply generates ticks (these routines).		*
*---------------------------------------------------------------*
*    VERSION   DATE      BY    CHANGE/COMMENTS                  *
*---------------------------------------------------------------*
*    2.01	06/11/86  DR-K	add include sysbuild.h		 *
*    2.00	03/05/86  MA	Converted to new timer system,	 *
*				with new routines tget(),tset(), *
*				and timer().  Also, a_delay() and*
*				tickit() moved here so OEMs can  *
*				change their timer schemes.	 *
*				Renamed tickit() and tikcod() to *
*				tik_asr() and tik_isr() and	 *
*				moved code around between them.	 *
*				Tik_init now sets starting time. *
*    1.02	12/12/85  MA	Added sethwcl() entry point; does*
*				nothing right now.		 *
*    1.01	10/29/85  MA	Change tickit priority to 150	 *
*    1.00	??/??/??  BJP	Original version		 *
*****************************************************************/

#include "portab.h"
#include "sysbuild.h"
#include "system.h"
#include "struct.h"
#include "baspag.h"
#include "vmdriver.h"
#include "vmecr.h"


/*****************************************************************************
*	External functions
*****************************************************************************/

EXTERN EVB *asetup();


/*****************************************************************************
*	External variables
*****************************************************************************/

EXTERN WORD dlatyp;


/*****************************************************************************
*	TIKCNT - total Elapsed ticks since system startup.
*****************************************************************************/

MLOCAL LONG	tikcnt;


/*****************************************************************************
*	ONE_DAY - total number of milliseconds in one day
*****************************************************************************/

#define ONE_DAY	86400000L


/*****************************************************************************
*	The TIMEDATE table.
*
*	In this implementation, this table is updated at every tick.
*	Ideally, we could read most of this information directly
*	from the cmos clock/calendar chip and only update the "time"
*	field on ticks.  The "time" field is used by the routines
*	that put timer events on and off the "dlr" timer event list.
*
*	The kernel gets and sets this table via the "tget" and "tset"
*	routines below.
*****************************************************************************/

MLOCAL struct
{
    union {
	LONG	ymdlong;	/* year, month, day in a single LONG word */
	struct {
	    WORD	year;
	    BYTE	month;
	    BYTE	day;
	} ymd;			/* year, month, day as individuals */
    } u;
    LONG	time;		/* number of milliseconds since midnight */
    WORD	timezone;
    BYTE	weekday;
    BYTE	reserved;
} td;


/*
 *	the tik_asr() ASR priority when set up in the tik_isr() ISR
 */

#define ASRPRI	150

/*	for forward reference	*/

WORD	tik_asr();
WORD	tik_isr();


/*****************************************************************************
*	VME/10-specific hardware definitions
*****************************************************************************/

/* defines used in conjunction with the clock registers		     */

#define  CLK_ADDR 	0xf1a080	/* physical clock address	    */

#define  C_BSET		0x80		/* enable set flag in register B
					 * - this allows update of time of 
					 * day
					 */

#define  C_INTRATE	0x0a		/* set the periodic interrupt rate   */

#define  C_INIT		0x46		/* initial control values in reg. B;
					 * periodic interrupt enable, binary
					 * data, 24 hour clock
					 */

#define  C_DISABLE	(~0x40)		/* disable interrupts for flush
					 * function
					 */

/*
 *	clock vector number.
 */

#define CLKVECNUM	0x4c

/* defines used in conjunction with VME/10 Control Register 0	     */

#define CR0_TIMER_ON	0x02		/* used to set bit 1 in CR0
					   enabling timer */

/* memory map of the VME/10 Clock Registers			     */

typedef struct
{
	BYTE fill0,  c_secs;		/* seconds elapsed in current minute */
	BYTE fill2,  c_sec_alarm;	/* when to generate alarm interrupt  */
	BYTE fill4,  c_mins;		/* minutes elapsed in current hour   */
	BYTE fill6,  c_min_alarm;	/* when to generate alarm interrupt  */
	BYTE fill8,  c_hrs;		/* hours elapsed in current day	     */
	BYTE fill10, c_hrs_alarm;	/* when to generate alarm interrupt  */
	BYTE fill12, c_day_wk;		/* day of week (1-7); sunday=1       */
	BYTE fill14, c_day_mon;		/* day of month (1-31);    0xf1a08e  */
	BYTE fill16, c_mon;		/* month of year (1-12)	   0xf1a090  */
	BYTE fill18, c_yr;		/* year of century (0-99)  0xf1a092  */
	BYTE fill20, c_a;		/* register A		   0xf1a094  */
	BYTE fill22, c_b;		/* register B		   0xf1a096  */
	BYTE fill24, c_c;		/* register C		   0xf1a098  */
	BYTE fill26, c_d;		/* register D		   0xf1a09a  */
} CLK;

MLOCAL CLK	*clk;

/* Control register info */

VMECRP	vmecr;


/*****************************************************************************
*	TIK_INIT	
*
*	Initialize the timer and time of day when system is booted
*****************************************************************************/

VOID tik_init()
{
    MAPPB	pmap;		/* physical map block for mapphys()   */

/* zero the number of ticks since Creation */

    tikcnt = 0;

/* initialize the time/date structure */

    td.u.ymd.year = 1986;
    td.u.ymd.month = 4;
    td.u.ymd.day = 8;
    td.time = 0L;			/* Current Time = Midnight.	*/

/* get access to vme/10 control registers */

    pmap.mpzero = 0;
    pmap.pbegaddr = VME10CR;
    pmap.plength = VME10CRSZ;
    vmecr = (VMECRP)mapphys(&pmap);

/* get access clock registers */

    pmap.mpzero = 0;
    pmap.pbegaddr = (BYTE *)CLK_ADDR;
    pmap.plength = sizeof(CLK);
    clk = (CLK *)mapphys(&pmap);

/* initialize interrupt handler for the clock */

    setvec(tik_isr,(LONG)CLKVECNUM);
}


/*****************************************************************************
*	TIK_OPEN
*
*	enable the timer interrupt
*****************************************************************************/

VOID tik_open()
{
    vmecr->cr0 |= CR0_TIMER_ON;	/* enable timer in CR0 only	     */

    clk->c_b |= C_BSET;		/* inhibit update so can set clock   */
    clk->c_a  = C_INTRATE;	/* set periodic interrupt rate	     */
    clk->c_c;			/* reset interrupt flag		     */
    clk->c_b  = C_INIT;		/* initialize and start timer	     */
}


/*****************************************************************************
*	TIK_ISR()
*
*	the clock interrupt handler.
*
*	note: since the clock has been initialized to interrupt
*	64 times a second and the time field of the TIMEDATE structure
*	contains a LONG value of milliseconds elapsed
*	since the last tick (interrupt).  We adjust the elapsed
*	time by truncating the fractional part of 15.625 msecs
*	and send 15 msecs seven times and 20 msecs (15 + 8*.625)
*	the eighth tick.
*
*	After updating the TIMEDATE structure, this routine sees
*	if there any events on the timer list ("dlr") that have
*	expired, and if so, schedules the tick_asr() routine to
*	satisfy those events.
*****************************************************************************/


/*	the following must be LONG values	*/

#define ELAPSED			15L
#define CORR_ELAPSED		20L	/* correct every 8 ticks */

WORD tik_isr()
{
    REG LONG n;			/* number of milliseconds since last tick */
    REG WORD nd;		/* number days in month. */

/* renable the tick interrupt by reading clock Reg C */

    clk->c_c;

/* update the time (and date if we've gone base midnight */

    if ((++tikcnt) & 7)			/* bump number of ticks */
	n = ELAPSED;
    else
	n = CORR_ELAPSED;		/* every eighth tick is corrected */

    if ((td.time += n) >= ONE_DAY)	/* If a day has gone by... */
    {				
	td.time -= ONE_DAY;
				/* Is it possible for a new month to begin? */
				/* If so, start it. Maybe new year, etc. */

	if ((++td.u.ymd.day) > 28)
	{

	/* Now let's have some fun ...
	 *	Welcome to the Julian calendar
	 */

	    if (td.u.ymd.month == 2)
		nd = 28 + ((td.u.ymd.year % 4) == 0)
		   - ((td.u.ymd.year % 100) == 0)
		   + ((td.u.ymd.year % 400) == 0);
	    else
		nd = ((td.u.ymd.month > 7)^(td.u.ymd.month % 2)) + 30;


	    if (td.u.ymd.day > nd)
	    {
		td.u.ymd.day = 1;
		if ((++td.u.ymd.month) > 12)
		{
		    ++td.u.ymd.year;
		    td.u.ymd.month = 1;
		}
	    }
	}
    }
    
/* see if any timer events have expired */

    if (dlr)					/* anything on the list? */
    {
	if (dlr->e_parm <= n)			/* it it time to wake up? */
	    doasr(tik_asr,n,0L,ASRPRI);		/* yes - let tik_asr do it */
	else
	    dlr->e_parm -= n;			/* no - decrement its delay */
    }

/* Do a dispatch to allow tik_asr() to run, and to cause a time slice.
   In real-time systems that don't use time-slicing we should only
   return TRUE if we had to call doasr() above, and FALSE otherwise
*/

    return(TRUE);
}


/*****************************************************************************
*	TIK_ASR()
*
*	Check all events waiting for a specific time, and wake up
*	those that have completed.
******************************************************************************/

tik_asr(n)
REG LONG n;			/* Number of milliseconds since last tick. */
{
    if (dlr)
    {
	while (dlr && (dlr->e_parm <= n))
	{
	    n -= dlr->e_parm;
	    _evdone(dlr);
	}
	if (dlr)
	    dlr->e_parm -= n;
    }
}


/*****************************************************************************
*	TIK_CLOSE
*****************************************************************************/

VOID tik_close()
{
    clk->c_b &= C_DISABLE;		/* turn off interrupts */
}


/*****************************************************************************
*	DLACAN(p)
*
*	Routine called when cancelling a timer event.
******************************************************************************/

LONG dlacan(p)
REG EVB *p;
{
	if (p->e_link)
	    p->e_link->e_parm += (LONG)p->e_parm;
	return(0);
}


/*****************************************************************************
*	A_DELAY(c,aswi)
*
*	Wait C milliseconds, and run ASWI on completion.
******************************************************************************/

LONG a_delay(c,aswi)
REG LONG c;			/* number of milliseconds to wait */
    BYTE *aswi;			/* SWI to run when time expires */
{
    REG EVB *p,*q,*e;

    if (e = asetup(aswi))
    {
	e->e_type = dlatyp;
	if (++c == 0)
	    c-- ;
	NODISP
	q = (EVB *)((BYTE *)&dlr - elinkoff);
	for (p = dlr; p; p = (q = p)->e_link)
	{
	    if (c <= p->e_parm)
		break;
	    c -= p->e_parm;
	}
	e->e_pred = q;
	q->e_link = e;
	e->e_parm = c;
	e->e_link = p;
	if (p)
	{
	    p->e_pred = e;
	    p->e_parm -= c;
	}
	DISPON
	return(e->e_mask);
    }
    else
	return(EM_KERN | E_EMASK);
}


/*****************************************************************************
*	TIMER(c,aswi,abs)
*
*	Wait C milliseconds, or until absolute time C, and run ASWI
*	on completion.  The "ABS" flag tells whether this is a relative
*	or absolute delay.
*
*	This function is called by the kernel when a program does a TIMER SVC.
*	Right now it just translates to an a_delay()
*	call, but some OEMs might implement their delay
*	list using absolute, not relative times, and so might need
*	to change this code and/or a_delay().
******************************************************************************/

LONG timer(c,aswi,abs)
REG LONG c;
    BYTE *aswi;
    BYTE abs;
{
    if (abs)			/* wait till absolute time? */
	if ((c -= td.time) < 0)	/* convert to relative time */
	    c += ONE_DAY;	/* must be next day */
    return (a_delay(c,aswi));
}


/*****************************************************************************
*	TGET - get the current TIMEDATE table
*
*	This function is called by the kernel when a program does a GET
*	on the TIMEDATE table.
*
*	Right now, this function merely makes a copy of the "td" structure,
*	but it could also read the clock/calendar chip for better accuracy.
*****************************************************************************/

tget(buf,size)
REG LONG *buf;		/* pointer to destination buffer */
UWORD size;		/* number of bytes caller needs to get */
{
    if (size == 8)	/* optimize for calls that need td.time FAST */
    {
	buf[0] = td.u.ymdlong;		/* get year,month,day */
	buf[1] = td.time;		/* get time */
    }
    else		/* everybody else can be slow */
    {
	if (size > sizeof(td))		/* they want too much? */
	    size = sizeof(td);
	if (size >= 10)			/* they want the weekday too? */
	    getday();			/* fill in weekday properly */
	_bmove(&td,buf,size);	/* copy the bytes */
    }
}

/***************************************************************************
*	GETDAY
*
*	Set td.weekday to the day of the week based on the current year,
*	date, and month.
****************************************************************************/

MLOCAL WORD modays[] =	/* Alcyon C complains about UWORD (grrr!) */
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

MLOCAL getday()
{
    REG UWORD years, days, months;

    if (td.u.ymd.year < 1980)
	td.weekday = 0;

    years = td.u.ymd.year - 1980;
    days = td.u.ymd.day - 1;
    months = td.u.ymd.month - 1;

    td.weekday = (365 * years + (years+3)/4 + modays[months] + days +
	(!(years & 3) && months > 1) + 2) % 7;
}


/*****************************************************************************
*	TSET - set the current time and date
*
*	This function is called by the kernel when a program does a SET
*	on the TIMEDATE table.
*
*	Right now, this function merely copies the data into the local
*	"td" structure, but it should also set the clock/calendar
*	chip (if present).
*****************************************************************************/

tset(buf,size)
REG LONG *buf;		/* pointer to source buffer */
UWORD size;		/* number of bytes caller wants to set */
{
    if (size > sizeof(td))		/* size too big? */
	size = sizeof(td);
    _bmove(buf,&td,size);		/* copy the bytes */

}
                                    
too big? */
	size = sizeof(td);
    _bmove(buf,&td,size);		/* copy the bytes */

}
                                    