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
 *   Version 1.9	vmconfig.c				 *
 *	This file contains the system configuration module, it	 *
 *	is responsible for initializing resource managers,	 *
 *	loading drivers, and starting up the BOOTINIT process	 *
 *---------------------------------------------------------------*
 *    VERSION   DATE    BY      CHANGE/COMMENTS                  *
 *---------------------------------------------------------------*
 *	1.9	07/23/86 DR-K	reverse pt0/pt1 order in s_install*
 *				for dvrunit.			 *
 *	1.8	06/11/86 DR-K	Changed ver to "1.2", set date	 *
 *				added include "sysbuild.h"	 *
 *	1.7.1	04/08/86 MA	Changed "1.1D+" to "1.15" to 	 *
 *				keep marketing folks happy !@$%%!*
 *	1.7	03/25/86 MA,	Add conditional stuff to allow	 *
 *			 DR-K	removal of console resource mgr. *
 *	1.6	10/25/85 MA	Changed asrs to a fixed array.   *
 *				Added nmods public variable.	 *
 *				Changed fnumtab to a pointer,    *
 *				not an array, like 286 CONFIG	 *
 *	1.5	08/18/85 MA	Added nasrs configuration, and	 *
 *				and initialize asrs in initasr().*
 *	1.4	08/15/85 MA	Changes for serial consoles	 *
 *  	1.3	08/12/85 MA	noman must return E_SUCCESS on	 *
 *				I_PABORT, I_PCREATE and I_INIT,	 *
 *				else E_IMPLEMENT.		 *
 *	BETA							 *
 *		07/25/85 MA	Added ASR table; file system	 *
 *				and kernel config variables	 *
 *	1.2	07/04/85 KSO	Set up for BOOTINIT		 *
 *	1.1	05/23/85 MA	Set up the window manager stuff. *
 *				Got rid of the serial drivers.	 *
 *				Put in CP/M front end as a	 *
 *				resource manager		 *
 *				Allow other default drives than	 *
 *				flpy1:				 *
 *				Make a loadable window manager.	 *
 *				Lowercase KBD: and CON:		 *
 *				Bring it up to the Beta CONFIG	 *
 *				(with fnumtab1, etc).		 *
 *				Use boot drive as default drive	 *
 *	1.0	??/??/?? ???	Created				 *
 *****************************************************************
 * INCLUDES							 */
#include	"portab.h"
#include	"sysbuild.h"
#include	"system.h"
#include	"sup.h"
#include	"flags.h"
#include	"dh.h"
#include	"struct.h"

#define CONMAN		1		/* we have a console resource mgr */
#define	TWOCONSOLES	1		/* we have two physical consoles */
#define MAX_FNUM	128		/* size of file number table */

/***********************
************************
** FILE SYSTEM TUNING **
************************
***********************/

#define	DIRFATSZ	0x4000L
#define	DATASZ		0x1400L
#define	PATHQUOTA	0x400
#define	TOTALQUOTA	0x800

GLOBAL LONG	dirfatsz = DIRFATSZ;
GLOBAL LONG	datasz = DATASZ;
GLOBAL UWORD	pathquota = PATHQUOTA;
GLOBAL UWORD	totalquota = TOTALQUOTA;

/****************
* kernel/memory *
****************/

#define NASRS	256		/* total number of asrs */
#define	DEFSTK	1536L		/* default stack size for process */

GLOBAL LONG	defstk = DEFSTK;	/* size of process stack */
GLOBAL UWORD	maxldt = 0;		/* number LDT slots per process */

	/* Release info		*/

BYTE	reldate[] =
	"June 27, 1986\r\nRelease V1.2\r\n";
BYTE	signon[] =
	"\033EConcurrent DOS 68K Version 1.1\r\nVME/10 I/O System\r\n";

	/* OS function calls	*/

EXTERN	LONG	s_cancel();
EXTERN	LONG	s_close();
EXTERN	LONG	s_define();
EXTERN	LONG	s_exit();
EXTERN	LONG	s_install();
EXTERN	LONG	s_open();
EXTERN	LONG	s_write();
EXTERN	LONG	e_command();
EXTERN	LONG	install();


	/* BOOTINIT load info	*/

#define	PINFO	struct	_pinfo
PINFO
{
	BYTE	pi_pname[10];	/* Process name				*/ 
	BYTE	pi_prior;	/* Process priority			*/
	BYTE	pi_fill; 	/* Filler				*/
	LONG	pi_maxmem;	/* Max. mem. to give to the process	*/
	LONG	pi_reserve;	/* Reserved				*/
};

#define	BOOT_PRIOR    180	/* Default process priority		*/
#define	DFLT_MEM      0x20000L	/* Default process memroy size		*/

	/* Bootinit process	*/

BYTE	bootproc[] = "BOOTINIT";
BYTE	bootcmd[]  = "boot:command.68K";
BYTE	boottail[] = "boot:config.bat !exit";

#define	TAILLEN	(LONG)sizeof(boottail)

	/* Driver Headers	*/

EXTERN	DH	null_dh;
EXTERN	DH	fd_dh;
EXTERN	DH	sp_hdr;

#if	CONMAN
EXTERN	DH	t52dh;
EXTERN	DH	kbhdr;
EXTERN	DH	c_dh;
#endif

EXTERN	DH	s_dh;

	/* Driver access flags	*/

#define	ACC	A_DEVLOCK+A_SHARE+A_READ+A_WRITE+A_SET
#define	ACC1	ACC+A_REMOVE

	/* File number table	*/

WORD	max_fnums = MAX_FNUM;	/* Max file number table size		*/
MLOCAL	FNUMT	fntab[MAX_FNUM];/* File number table entry		*/
FNUMT	*fnumtab = fntab;	/* global pointer to file number table	*/

	/* Resource Manager Entry Points	*/

EXTERN	LONG	diskman();
EXTERN	LONG	kernman();
EXTERN	LONG	pipeman();
EXTERN	LONG	misman();
EXTERN	LONG	superman();
#if	CONMAN
EXTERN	LONG	conman();
#endif

EXTERN	LONG	cmdentry();
EXTERN	LONG	cpmfeman();
	LONG	noman();

#define NMODS 11
GLOBAL WORD nmods = NMODS;		/* number of modules in table below */
GLOBAL LONG (*modules[NMODS])() = {
	kernman,		/* 0  - Kernal Module		*/
	pipeman,		/* 1  - Pipe Manager Module	*/
	diskman,		/* 2  - Disk Manager Module	*/
#if	CONMAN
	conman,			/* 3  - Console Manager Module	*/
#else
	noman,			/* 3 -  Console Manager Module	*/
#endif
	cmdentry,		/* 4  - Command Module		*/
	noman,			/* 5  - Extra RM Slot		*/
	noman,			/* 6  - Network Manager Module	*/
	misman,			/* 7  - Misc. Manager Module	*/
	superman,		/* 8  - Supervisor Module	*/
	noman,			/* 9  - DOS Front End		*/
	cpmfeman		/* 10 - CPM Front End		*/
};


CONFIG()
{
    REG	WORD i;
    REG	LONG ofn;

	for (i=0; i < nmods; i++)
	{
	    nodisp();
	    (*modules[i])(I_INIT,0L);
	    okdisp();
	}
		/* Null device driver	*/

	install("null:",ACC,&null_dh);

		/* Floppy drivers	*/

	install("fd0:",ACC,&fd_dh);
	install("fd1:",ACC,&fd_dh);
	install("fd2:",ACC,&fd_dh);
	install("fd3:",ACC,&fd_dh);

		/* Port driver		*/

	install("pt0:",ACC1,&sp_hdr);

			/* Serial driver 	*/
			/* use port0 for ser: or no console resource manager */
	if( install("ser:",ACC1,&s_dh) == (EM_SUP | E_SUBDEV) )
	    s_install(3,0,"pt0:","ser:");

#if	CONMAN
				/* Keyboard driver	*/
	install("kb0:",ACC1,&kbhdr);
				/* Console driver	*/
	if( install("con0:",ACC1,&c_dh) == (EM_SUP | E_SUBDEV) )
	    s_install(3,0,"kb0:","con0:");

#if	TWOCONSOLES
				/* put a terminal driver against port1 */
	if ( install("trm0:",ACC1,&t52dh) == (EM_SUP | E_SUBDEV) )
	   if ( s_install(2,ACC1,"pt1:","pt0:") >= 0 )
		s_install(3,0,"pt1:","trm0:");

	if ( s_install(2,ACC1,"con1:","con0:") == (EM_SUP | E_SUBDEV) )
		s_install(3,0,"trm0:","con1:");
#endif
#endif
		/* Set up REQUIRED defines	*/

#if	CONMAN
	s_define( 0,"stdin","con",0L );
	s_define( 0,"stdout","con",0L );
	s_define( 0,"stderr","con",0L );
	s_define( 0,"stdcmd","con",0L );
	s_define( 0,"con","con:console", 0L);
	s_define( 0,"con:","con0:", 0L);
#else
	s_define( 0,"stdin","ser",0L );
	s_define( 0,"stdout","ser",0L );
	s_define( 0,"stderr","ser",0L );
	s_define( 0,"stdcmd","ser",0L );
	s_define( 0,"ser","ser:", 0L);
	s_define( 0,"ser:","ser0:", 0L);
#endif

		/* Define system:, home:,	*/
		/* boot:, and default:		*/

	vmedflt();

		/* Print startup banner		*/

	ofn = s_open( 0x0055,"stdout" );
	s_write( 0,ofn,signon,(LONG)sizeof(signon),0L );
	s_write( 0,ofn,reldate,(LONG)sizeof(reldate),0L );

#if	CONMAN
	s_close( 0,ofn );
#endif

		/* Start the bootinit process	*/

	bootinit();

		/* Terminate the config process	*/

	s_exit( 0L );
}

/***********************************************************************
*	Dummy resource manager
***********************************************************************/

LONG noman(f,p)
    WORD f;
    LONG p;
{
	if (f <= I_PABORT)
	    return(E_SUCCESS);
	return(EM_SUP | E_IMPLEMENT);
}

/***********************************************************************
*	BOOTINIT loader
***********************************************************************/

WORD	bootinit()
{
    REG WORD	i;
    REG BYTE	*p;
    REG BYTE	*q;
    LONG	pid;
    PINFO	pi;

	pi.pi_prior = BOOT_PRIOR;
	pi.pi_maxmem = DFLT_MEM;
	pi.pi_reserve = 0L;
	p = bootproc;
	for( q = pi.pi_pname,i = 0; (i < 10) && (*q = *p); p++,q++ );
	s_cancel( e_command( 0L,&pid,0,bootcmd,boottail,TAILLEN,&pi ) );
}

/***********************************************************************
*	VME/10 boot drive stuff
***********************************************************************/

EXTERN WORD disknum;		/* in ommua.s - contains VME/10 boot drive */

MLOCAL BYTE *vmename[4] = { "fd2:", "fd3:", "fd0:", "fd1:" } ;

vmedflt()
{
	BYTE *dfltdrv;

	dfltdrv = vmename[disknum];

	s_define( A_SYSTEM,"system:",dfltdrv,0L );
	s_define( A_SYSTEM,"boot:",dfltdrv,0L );
	s_define( 0,"home:",dfltdrv,0L );
	s_define( 0,"default:",dfltdrv,0L );
}


/************
* ASR Table *
************/

	/* Initial Threaded List of unused ASR Blocks :
	   An ASR Block is used for each scheduled or pending ASR.
	   The system panics if this list is empty.  This implies
	   that are too many pending ASR's. */

GLOBAL	ASR	*asrfree = 0L;
MLOCAL ASR asrtab[NASRS];			/* fixed table of ASRs */


/***********************************************************************
*	ASRINIT - set up the initial free ASR list
*
*	Must be called before any interrupts are enabled.
***********************************************************************/

asrinit()
{
    REG ASR *a;
    REG WORD i;

    for (i = 0; i < NASRS; i++)
    {
	a = &asrtab[i];				/* get next asr in array */
	a->asr_link = asrfree;			/* link it into free list */
	asrfree = a;
    }
}
