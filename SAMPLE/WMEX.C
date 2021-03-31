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
 *   Version 2.0        WMEX.C					 *
 *                      Window Manager.				 *
 *---------------------------------------------------------------*
 *    VERSION   DATE    BY      CHANGE/COMMENTS                  *
 *---------------------------------------------------------------*
 *                                                               *
 *	2.0   03/05/86  ma	Fix previous fix so that logoff  *
 *				occurs only when no windows left.*
 *	1.9   02/20/86	ma	Do a logoff if shell returns 	 *
 *				negative error code.		 *
 *	1.8   08/14/85	jsr	Modified to allow dcnum to be a	 *
 *				vcon and not always a physical.	 *
 *	1.7   07/23/85	jsr	Make all MSGPIP errors non-fatal *
 *				for each vcon, fixed MSGPIPE	 *
 *				create size problem, modified	 *
 *				for CCUTLS changes, removed	 *
 *				STDIO defines, modified MSGPIPE	 *
 *				read/write code, set no echo on	 *
 *				special windows.		 *
 *	1.6   07/17/85	jsr	Fixed: Border size = 0 on vcreat *
 *				if no border, don't exit if ROOT,*
 *				unkctrl the desk before exit,	 *
 *				shut off wrap on special windows,*
 *				print and accept REAL window #s, *
 *				cleaned up event stuff, fixed	 *
 *				MSGPIPE create access problem.	 *
 *	1.5   06/06/85	jsr	Fixed: Logoff stuff, 		 *
 *	1.4   05/10/85	jsr	Create mpipes w/o write access,	 *
 *				check for full LONG E_LOGOFF and *
 *				save and restore entry SMODE and *
 *				KMODE.				 *
 *	1.3   05/10/85	jsr	Restructured exit code, fixed	 *
 *				wndw# problem for <HELP>, and	 *
 *				input problem in invcmd(). Also	 *
 *				now create mpipes of unique	 *
 *				names and define con: for shell. *
 *	1.2   05/06/85	jsr	Fixed create(vc) error logic to  *
 *				only abort new window, not WMEX. *
 *				Also added mssgact report in STAT*
 *	1.1   04/26/85	jsr	Added setattrib() escape seq's.  *
 *				Also fixed async error code.	 *
 *	1.0   04/23/85	jsr					 *
 *                                                               *
 *===============================================================*/

/*
	Assumes:
		Defined name "con:" is a virtual or physical console name.
		Defined name "shell" is an executable program to run in
		each virtual console.
		Defined name "prn:" is an output device for PRINTSCREEN.
 */

/*
	Suggested additional features:
	1)	Create all windows as full sized, but set view size as
		configured. Add a new key <WTGL> to toggle window between
		FULL and SIZED.
	2)	If above is implemented, then add another flags field to
		WNDWDESC structure that includes the three flags associated
		with cursor tracking (see VCON table - keyboard mode bits).
		OR in these bits when setting KMODE.
	3)	Add create time options to set foreground and background
		colors, window sizing, and KMODE control bits.
*/

/* include some header files */

								/* C defines */
#include "portab.h"
						   /* Concurrent DOS defines */
#include "flags.h"
#include "wmchar.h"
#include "wmos.h"
							  /* utility defines */
#include "ccutls.h"
#include "utlerrs.h"
						    /* WMEX specific defines */
#include "wmex.h"


/* declare local (global) variables */
						    /* general use variables */
BYTE	mygroup;				       /* LOGIN group number */
BYTE	myuser;						/* LOGIN user number */
BYTE	pathchar[2];						 /* pathchar */
BYTE	topbuf[CMAXW];			   /* buffer for top border assembly */
BYTE	pconame[NAMELEN];			 /* name of physical console */

WORD	tlvl;				       /* top level index for ltop[] */

UWORD	ch16;					    /* 16 bit character cell */
UWORD	wtop;					  /* Screen currently on top */
UWORD	ltop[LTOPMAX];				       /* Last screen on top */
UWORD	nstart;				 /* number of USRx wndws to start up */
UWORD	cnrows;					 /* nrows from CONSOLE table */
UWORD	cncols;					 /* ncols from CONSOLE table */
UWORD	viewrow;				/* viewrow from PCON or VCON */
UWORD	viewcol;				/* viewcol from PCON or VCON */
UWORD	smode, kmode;		 /* entry SMODE and KMODE from console table */

BOOLEAN	fninit;				     /* set if all FNUMs initialized */
BOOLEAN	pflags[MAXVC-WNDWUSR1];	      /* array of process found flags (STAT) */
BOOLEAN	logoff;					    /* logoff requested flag */
BOOLEAN exitflg;						/* exit flag */
BOOLEAN mssgact;				       /* WNDWMSSG is active */
BOOLEAN	statact;				       /* WNDWSTAT is active */
BOOLEAN	killstat;				   /* exit the STATUS window */
BOOLEAN	kcflag;			   /* set for STATUS wndw on most kctrl keys */
BOOLEAN	modeflg;			  /* set if S/KMODE values are valid */

LONG	rcode;						 /* wmex return code */
LONG	wmexpid;					      /* pid of WMEX */
LONG	etable[NOSEVENTS];			     /* array of event masks */

MPIPE	mpipe;				/* message pipe structure for <HELP> */


WVARS	wvars[MAXVC];	  	      /* array of window variable structures */

PCONSOLE pcns;						       /* pcns table */

jumpbuff	jmpbuf;				 /* error abort control info */


						     /* DESK window specials */
LONG	dcnum;					  /* DESK window file number */
RECT	r_dwndw;					 /* DESK window area */
FRAME	f_dwndw;				   /* DESK window area frame */

						     /* MSSG window specials */
LONG	rsppfnum;				 /* reponse pipe file number */
RECT	r_minfoa;				    /* MSSG window info area */
RECT	r_mmorea;				    /* MSSG window more area */
RECT	r_mpmpta;				    /* MSSG window pmpt area */

						     /* STAT window specials */
RECT	r_sinfoa;				    /* STAT window info area */
RECT	r_smorea;				    /* STAT window more area */
RECT	r_spmpta;				    /* STAT window pmpt area */

						     /* PRINTSCREEN specials */
BYTE	*ps_bbase;			       /* base of PRINTSCREEN buffer */
BYTE	*ps_bptr;			    /* pointer to PRINTSCREEN buffer */
UWORD	ps_rcnt;				    /* count of printed rows */
UWORD	ps_nrow;			     /* number of rows to be printed */
UWORD	ps_ncol;		  /* number of columns to be printed per row */
BOOLEAN ps_active;				  /* PRINTSCREEN active flag */
LONG	ps_fnum;				      /* printer file number */


/* cast local procedures */

LONG	wmex();

VOID	create();
VOID	message();
VOID	newvcon();
VOID	printscrn();
VOID	setupd();
VOID	setupw();
VOID	status();
VOID	wminit();
VOID	wmshell();

/* declare external procedures */
							  /* from WMUTLS.L86 */
EXTERN	BYTE	*fixnum();
EXTERN	WORD	getpcns();
EXTERN	WORD	nextvc();
EXTERN	BOOLEAN	center();
EXTERN	BOOLEAN	chksproc();
EXTERN	BOOLEAN	statcmd();
EXTERN	LONG	erdelim();
EXTERN	VOID	abortall();
EXTERN	VOID	bldspec();
EXTERN	VOID	bottom();
EXTERN	VOID	clrwrect();
EXTERN	VOID	crlfs();
EXTERN	VOID	dopmpt();
EXTERN	VOID	getvcinfo();
EXTERN	VOID	helpmsg();
EXTERN	VOID	kbevent();
EXTERN	VOID	killall();
EXTERN	VOID	killvcon();
EXTERN	VOID	noabt();
EXTERN	VOID	prnstatic();
EXTERN	VOID	prntext();
EXTERN	VOID	setattrib();
EXTERN	VOID	setconmode();
EXTERN	VOID	setcurpos();
EXTERN	VOID	setvcinfo();
EXTERN	VOID	top();
EXTERN	VOID	vcswitch();
EXTERN	VOID	wrtpline();
							  /* from CCUTLS.L86 */
EXTERN	BYTE	*utskpwht();
EXTERN	BYTE	*utui2ds();
EXTERN	BYTE	*utul2ds();
EXTERN	BOOLEAN	utisroot();
EXTERN	VOID	utfarjmp();
EXTERN	VOID	utgtpch();
EXTERN	VOID	utscat();
EXTERN	VOID	utscopy();
EXTERN	LONG	utprnmsg();
EXTERN	LONG	utslen();
EXTERN	LONG	utstjmp();
							   /* from CCRTL.L86 */
EXTERN	LONG	s_alter();
EXTERN	LONG	s_cancel();
EXTERN	LONG	e_command();
EXTERN	LONG	s_copy();
EXTERN	LONG	s_close();
EXTERN	LONG	s_create();
EXTERN	LONG	s_define();
EXTERN	LONG	s_disable();
EXTERN	LONG	s_get();
EXTERN	LONG	s_kctrl();
EXTERN	LONG	s_lookup();
EXTERN	LONG	s_malloc();
EXTERN	LONG	s_mfree();
EXTERN	LONG	s_open();
EXTERN	LONG	s_order();
EXTERN	LONG	s_rdelim();
EXTERN	LONG	e_read();
EXTERN	LONG	s_return();
EXTERN	LONG	e_termevent();
EXTERN	LONG	s_vccreate();
EXTERN	LONG	s_wait();
EXTERN	LONG	e_write();
EXTERN	LONG	s_write();

/* declare external variables */
							  /* from WMEXDATA.C */
EXTERN	UBYTE	wm0010;
EXTERN	UBYTE	wm0100, wm0110, wm0120;
EXTERN	UBYTE	*wm0260[];
EXTERN	UBYTE	wm0299[];
EXTERN	UBYTE	wm9009[];

EXTERN	WNDWDESC	wd_mssg, wd_stat, wd_usr1, wd_usr2;
EXTERN	WNDWDESC	wd_usr3, wd_usr4, wd_usr5, wd_usr6;

EXTERN	WNDWSPEC	ws_mssg, ws_stat;

							   /* from WMEXNUT.C */
EXTERN	UBYTE	wm8000[], wm8001[], wm8002[], wm8003[];
EXTERN	UBYTE	wm8004[], wm8005[], wm8006[], wm8007[];
EXTERN	UBYTE	wm8100[], wm8110[], wm8130[];
EXTERN	UBYTE	wm8170[];
EXTERN	UBYTE	wm9003[], wm9004;


/* start of code */

/* wmex() : initialization and main routine */

LONG wmex()
{
    BYTE		specbuf[BUFSIZ];
    BYTE		alterb[PL_NPLANE * 2];
    LONG		dmask, emask;

    REG	WORD		i, event, vc, dosflgs;


					     /* prevent external termination */
    e_termevent((LONG)&noabt, (LONG)0);
    s_disable();

restart:
						   /* set error control info */
    dcnum = FNUMINIT;
    fninit = exitflg = logoff = FALSE;
					      /* set error abort information */
    if(rcode = utstjmp(jmpbuf))
    {
	killall();		     	   /* go kill off all vcon processes */

bailout:
					      /* any terminate events left ? */
	for(i = OSEVUSR1; ((i <= OSEVUSR6) && (!etable[i])); ++i);

	if(i <= OSEVUSR6)
	    goto eventhndlr;		      /* go wait on TERMINATE events */

						 /* close up special windows */
	killvcon(WNDWSTAT);
	killvcon(WNDWMSSG);
						   /* close up response pipe */
	if(rsppfnum != FNUMINIT)
	{
	    s_close(0x0000, rsppfnum);
	    rsppfnum = FNUMINIT;
	}
						       /* close up PRN: file */
	if(ps_fnum != FNUMINIT)
	{
	    s_mfree((LONG)ps_bbase);
	    s_close(0x0000, ps_fnum);
	    ps_fnum = FNUMINIT;
	}
						       /* close up DESK last */
	if(modeflg)
	    setconmode(dcnum, smode, kmode);

	if(dcnum != FNUMINIT)
	{
	    s_kctrl(dcnum, 0x0001, CI_PSCRN, CI_HELP);	     /* unkctrl DESK */
	    wvars[WNDWDESK].wv_flags &= ~WV_INUSE;
	    s_close(0x0000, dcnum);
	    dcnum = FNUMINIT;
	}

	if(logoff)
	    rcode = E_LOGOFF;

	return(rcode);					 /* RETURN FROM HERE */

    }

					      /* initialize global variables */
    wminit();
					       /* initialize local variables */
    dmask = 0;
					 		/* setup DESK window */
    dosflgs =
      (pcns.pc_planes & wm0100 & W_ATTRIBS) ? (A_CHARPL|A_ATTRPL) : A_CHARPL;
    alterb[ALB_CAND] = alterb[ALB_AAND] = alterb[ALB_EAND] =
      alterb[ALB_EXOR] = 0;
    alterb[ALB_CXOR] = wm0110;
    alterb[ALB_AXOR] = (dosflgs & A_ATTRPL) ? wm0120 : 0;

    if((rcode = s_alter(dosflgs, dcnum, (LONG)0, (LONG)&r_dwndw,
      (LONG)alterb)) < SUCCESS)
	utfarjmp(jmpbuf, rcode);

 		    /* set keyboard mode, kctrl ranges for wmex special keys */
    if((rcode = s_kctrl(dcnum,0x0001, CI_HELP, CI_PSCRN)) < SUCCESS)
	utfarjmp(jmpbuf, rcode);

    setconmode(dcnum, 0x0000, (CKM_NAB|CKM_SXB|CKM_NEC));

				      /* initialize stuff for message window */
    newvcon(WNDWMSSG);
    prnstatic(WNDWMSSG);
    setconmode(wvars[WNDWMSSG].wv_cnum, 0x0000, (CKM_NAB|CKM_NEC));

				       /* initialize stuff for Status Window */
    newvcon(WNDWSTAT);
    prnstatic(WNDWSTAT);
    setconmode(wvars[WNDWSTAT].wv_cnum, 0x0000, (CKM_NAB|CKM_NEC));
					     /* create and open message pipe */
    bldspec(WNDWSTAT, S_PIP, specbuf);
    dosflgs = (A_SET|A_READ|A_SHARE|A_TEMP|A_SECURITY);
    wvars[WNDWSTAT].wv_pnum = s_create((BYTE)0x00, dosflgs, (LONG)specbuf,
      0x0000,(FS_OD|FS_OE|FS_OW|FS_OR),(LONG)sizeof(MPIPE));

    if((etable[OSEVMPST] = e_read((LONG)0, A_FPOFF, wvars[WNDWSTAT].wv_pnum,
      (LONG)&(wvars[WNDWSTAT].wv_mph), (LONG)sizeof(wvars[WNDWSTAT].wv_mph),
      (LONG)0)) < SUCCESS)
	etable[OSEVMPST] = 0;

				    /* fire up nstart number of USRx windows */
startup:
    for(i = 0, vc = WNDWUSR1; i < nstart; ++i, ++vc)
    {
	create(vc);
	if((i == 0) && (!(wvars[vc].wv_flags & WV_INUSE)))
	    utfarjmp(jmpbuf, rcode);
    }

						    /* allow keyboard events */
    if((etable[OSEVKEYBD] = e_read((LONG)0, 0x0000, dcnum, (LONG)&(ch16),
     (LONG)sizeof(ch16), (LONG)0)) < SUCCESS)
    {
	rcode = etable[OSEVKEYBD];
	etable[OSEVKEYBD] = 0;
	utfarjmp(jmpbuf, rcode);
    }

eventhndlr:						    /* EVENT HANDLER */

    for (;;)
    {
							  /* Build WAIT mask */
	emask = 0;
	for (i = 0; i < NOSEVENTS; i++)
	    emask |= etable[i];

					    /* Wait for event(s) to complete */
	emask = s_wait(emask);
	emask |= dmask;			/* OR in any outstanding events too. */
	dmask = 0;

						   /* handle multiple events */
	for(event = 0; event < NOSEVENTS; ++event)
	{
							    /* find an event */
	    if(!(etable[event] & emask))
		continue;

			/* We have an event completion and we know who it is */

	    rcode = s_return(etable[event]);
	    etable[event] = 0;

	    switch(event)
	    {

		case OSEVUSR1:			    /* USRx shell terminated */
		case OSEVUSR2:
		case OSEVUSR3:
		case OSEVUSR4:
		case OSEVUSR5:
		case OSEVUSR6:
						       /* kill off this vcon */
		    killvcon(event);
						    /* go kill all if LOGOFF */
		    if(rcode == E_LOGOFF)
		    {
			logoff = TRUE;
			killall();
		    }
						  /* any open user windows ? */

		    for(i=WNDWUSR1;
		      ((i<MAXVC) && (!(wvars[i].wv_flags&WV_INUSE))); ++i);

						     /* all windows closed ? */
		    if(i >= MAXVC)
		    {
			if((!exitflg) && (!logoff) && (rcode > 0L))
			{		/* not exit or logoff or bad exit? */
			    dmask = s_cancel(etable[OSEVKEYBD]);
			    etable[OSEVKEYBD] = 0;
			    nstart = 1;
			    goto startup;
			}
			else
			{
			    if(utisroot())
				goto restart;
			    else
				goto bailout;
			}
		    }

		    break;

		case OSEVMPST:			      /* STAT <HELP> or else */
		case OSEVMPU1:			      /* USRx sent a message */
		case OSEVMPU2:
		case OSEVMPU3:
		case OSEVMPU4:
		case OSEVMPU5:
		case OSEVMPU6:
		    message(event - MAXVC);
		    break;

		case OSEVKEYBD:			     /* KCTRL keyboard event */
		    kbevent();
		    break;

		case OSEVPRNT:				/* <PSCN> line write */
		    wrtpline();
		    break;

		case OSEVHELP:			/* <HELP> msgpipe write done */
		case OSEVRSPP:		    /* MSSG response pipe write done */
		    rcode = SUCCESS;
		    break;

		case NOSEVENTS:
		default:
		    utfarjmp(jmpbuf, (UR_SOURCE | UR_INTERNAL));
		    break; 
	    }					      /* end of event SWITCH */
	}				   /* end of multiple event FOR loop */
    }				  /* end of event handler FOR loop (endless) */
}						    /* end of WMEX procedure */

/* local utility procedures */

/* wminit() : initialize some global variables and structures */   

VOID wminit()
{

    BYTE		specbuf[BUFSIZ];
    ENVIRON		env;
    CONSOLE		ct;
    VCONSOLE		vcon;

    REG	WORD		i, vc;

    ps_fnum = rsppfnum = FNUMINIT;
    ps_rcnt = ps_nrow = ps_ncol = 0;
    ps_active = mssgact = statact = modeflg = FALSE;
						  /* initialize event table */
    for (i = 0; i < NOSEVENTS; i++)
	etable[i] = 0;

    nstart =
      ((wm0010 > 0) && (wm0010 < MAXVC - 2)) ? (UWORD)wm0010 : 1;

					  /* close up standard input devices */
    s_close(0x0000, STDIN);
    s_close(0x0000, STDOUT);
    s_close(0x0000, STDERR);

    pathchar[0] = utgtpch();			   /* get the path character */
    pathchar[1] = NULL;
						/* get physical console info */
    vc = getpcns();
							/* open console file */
    if(vc)
	bldspec(vc, (S_DEV|S_VCR|S_CON), specbuf);
    else
	bldspec(0, (S_DEV|S_CON), specbuf);

    if((dcnum = s_open((A_READ|A_WRITE|A_SET|A_SHARE|A_SHFP),
      (LONG)specbuf)) < SUCCESS)
	utfarjmp(jmpbuf, rcode);
					      /* get the CONSOLE information */
    if((rcode = s_get(T_CON, dcnum, (LONG)&ct, (LONG)sizeof(ct))) < SUCCESS)
	utfarjmp(jmpbuf, rcode);

							   /* save size info */
    cnrows = ct.cn_nrows;
    cncols = ct.cn_ncols;
					       /* save entry SMODE and KMODE */
    smode = ct.cn_smode;
    kmode = ct.cn_kmode;
    modeflg = TRUE;

    vcon.vc_viewrow = RMIN;
    vcon.vc_viewcol = CMIN;

    if(vc)				     /* get the VCONSOLE information */
	s_get(T_VCON, dcnum, (LONG)&vcon, (LONG)sizeof(vcon));

    viewrow = vcon.vc_viewrow;
    viewcol = vcon.vc_viewcol;

    tlvl = 0;
    wtop = ltop[tlvl] = MAXVC + 1;

					 /* get group, user and process id's */
    if((rcode = s_get(T_ENV, (LONG)0, (LONG)&env,
      (LONG)sizeof(env))) < SUCCESS)
	utfarjmp(jmpbuf, rcode);
							      /* save a copy */
    myuser = env.en_user;
    mygroup = env.en_group;
    wmexpid = env.en_pid;

    for(i = WNDWDESK; i < MAXVC; ++i)
    {
	switch(i)
	{

	    case WNDWDESK:
		setupd();
		break;

	    case WNDWMSSG:
		setupw(i, &wd_mssg);
		break;

	    case WNDWSTAT:
		setupw(i, &wd_stat);
		break;

	    case WNDWUSR1:
		setupw(i, &wd_usr1);
		break;

	    case WNDWUSR2:
		setupw(i, &wd_usr2);
		break;

	    case WNDWUSR3:
		setupw(i, &wd_usr3);
		break;

	    case WNDWUSR4:
		setupw(i, &wd_usr4);
		break;

	    case WNDWUSR5:
		setupw(i, &wd_usr5);
		break;

	    case WNDWUSR6:
		setupw(i, &wd_usr6);
		break;

	}
    }
    fninit = TRUE;				        /* fnums initialized */
}

/* setupd() : set up DESK RECT and FRAME */

VOID setupd()
{

    REG RECT	*rpw;
    REG FRAME	*fpw;

    wvars[WNDWDESK].wv_flags = WV_INUSE;

    rpw = &r_dwndw;
    fpw = &f_dwndw;
    rpw->r_row = RMIN;
    rpw->r_col = CMIN;
    rpw->r_nrow = fpw->fr_nrow = cnrows;
    rpw->r_ncol = fpw->fr_ncol = cncols;
    fpw->fr_pl[PL_CHAR] = &wm0110;
    fpw->fr_pl[PL_ATTR] =
      (pcns.pc_planes & wm0100 & W_ATTRIBS) ? &wm0120 : (LONG)0;
    fpw->fr_pl[PL_EXTN] = (LONG)0;
    fpw->fr_use = (LONG)0;
}

/* setupw() : set up non-DESK window variables */

VOID setupw(vc, wdp)

REG WORD	vc;
REG WNDWDESC	*wdp;

{

    RECT		*rpt, *rpl;
    RECT		*rpi, *rpm, *rpp;
    FRAME		*fpt, *fpb, *fpl, *fpr;
    WNDWSPEC		*wsp;

    REG	WORD		baflag;
    REG RECT		*rpw;
    REG WVARS		*wvp;

    wvp = &wvars[vc];

    wvp->wv_pid = 0;
    wvp->wv_fid = 0;
    wvp->wv_flags = 0;
    wvp->wv_wdp = wdp;			   /* set pointer to WNDWDESC struct */
    wvp->wv_vnum = wvp->wv_cnum = wvp->wv_tnum = wvp->wv_bnum =
      wvp->wv_lnum = wvp->wv_rnum = wvp->wv_pnum = FNUMINIT;

							/* set RECT pointers */
    rpw = &wvp->wv_rwndw;
    rpt = &wvp->wv_rtbb;
    rpl = &wvp->wv_rlrb;
						       /* set FRAME pointers */
    fpt = &wvp->wv_ftop;
    fpb = &wvp->wv_fbtm;
    fpl = &wvp->wv_flft;
    fpr = &wvp->wv_frgt;

						       /* set origin indexes */
    rpw->r_row = RMIN;
    rpw->r_col = CMIN;

					      /* set lengths to largest size */
    rpw->r_nrow = (wdp->wd_rmax) ? wdp->wd_rmax : cnrows;
    rpw->r_ncol = (wdp->wd_cmax) ? wdp->wd_cmax : cncols;

			  /* adjust lengths to fit, error if not enough room */
    while((rpw->r_row + rpw->r_nrow) > cnrows)
	--rpw->r_nrow;
    if((cnrows < rpw->r_nrow) || (rpw->r_nrow < wdp->wd_rmin))
	utfarjmp(jmpbuf, (UR_SOURCE | UR_PARM));

    while((rpw->r_col + rpw->r_ncol) > cncols)
	--rpw->r_ncol;
    if((cncols < rpw->r_ncol) || (rpw->r_ncol < wdp->wd_cmin))
	utfarjmp(jmpbuf, (UR_SOURCE | UR_PARM));

						 /* check if borders desired */
    if(wdp->wd_flags & W_BORDERS)
    {

					 /* flag = 0 if no border attributes */
	baflag = (wdp->wd_flags & W_ATTRIBS & pcns.pc_planes);

	rpt->r_row = rpl->r_row = RMIN;
	rpt->r_col = rpl->r_col = CMIN;

	rpt->r_nrow = fpt->fr_nrow = fpb->fr_nrow = BRWIDTH;
	rpl->r_ncol = fpl->fr_ncol = fpr->fr_ncol = BCWIDTH;

	rpt->r_ncol = fpt->fr_ncol = fpb->fr_ncol = rpw->r_ncol;

				      /* adjust window RECT size for borders */
	rpw->r_nrow -= (BRWIDTH * 2);
	rpw->r_ncol -= (BCWIDTH * 2);

	rpl->r_nrow = fpl->fr_nrow = fpr->fr_nrow = rpw->r_nrow;

	fpt->fr_pl[PL_CHAR] = (BYTE *)topbuf;
	fpb->fr_pl[PL_CHAR] = &wdp->wd_bfill;
	fpl->fr_pl[PL_CHAR] = &wdp->wd_lfill;
	fpr->fr_pl[PL_CHAR] = &wdp->wd_rfill;
	fpt->fr_pl[PL_ATTR] = fpb->fr_pl[PL_ATTR] = fpl->fr_pl[PL_ATTR] =
	  fpr->fr_pl[PL_ATTR] = (baflag) ? &wdp->wd_battr : (LONG)0;
	fpt->fr_pl[PL_EXTN] = fpb->fr_pl[PL_EXTN] = fpl->fr_pl[PL_EXTN] =
	  fpr->fr_pl[PL_EXTN] = (LONG)0;
	fpt->fr_use = PL_USE0;
	fpb->fr_use = fpl->fr_use = fpr->fr_use = (LONG)0;
    }

    if((vc == WNDWMSSG) || (vc == WNDWSTAT))
    {

	if(vc == WNDWMSSG)
	{
	    wsp = &ws_mssg;
	    rpi = &r_minfoa;
	    rpm = &r_mmorea;
	    rpp = &r_mpmpta;
	}
	else
	{
	    wsp = &ws_stat;
	    rpi = &r_sinfoa;
	    rpm = &r_smorea;
	    rpp = &r_spmpta;
	}

	wsp->ws_actinfo = rpw->r_nrow - wdp->wd_rmin + wsp->ws_mininfo;
	if(wdp->wd_flags & W_BORDERS)
	    wsp->ws_actinfo += (BRWIDTH * 2);

	rpi->r_col = rpm->r_col = rpp->r_col = rpw->r_col;
	rpi->r_ncol = rpm->r_ncol = rpp->r_ncol = rpw->r_ncol;

	rpi->r_row = rpw->r_row + wsp->ws_numhdr;
	rpm->r_row = rpi->r_row + wsp->ws_actinfo;
	rpp->r_row = rpm->r_row + wsp->ws_nummore;
	rpp->r_row = (wvp->wv_wdp->wd_flags & W_PMPTHLP) ?
	  rpp->r_row : rpp->r_row + wsp->ws_numhelp;

	rpi->r_nrow = wsp->ws_actinfo;
	rpm->r_nrow = wsp->ws_nummore;
	rpp->r_nrow = wsp->ws_numpmpt;
    }
}

/* newvcon() : set up new virtual console */

VOID newvcon(vc)

REG WORD 	vc;

{

    BYTE		alterb[PL_NPLANE * 2];
    BYTE		specbuf[BUFSIZ];
    REG	BYTE		rw, cw;
    REG	WORD		dosflgs;
    REG	RECT		*rpw;
    REG WVARS 		*wvp;
    REG VCONSOLE	*vcp;

					      /* set pointers, mark as INUSE */
    wvp = &(wvars[vc]);
    vcp = &(wvp->wv_vct);
    rpw = &(wvp->wv_rwndw);
    wvp->wv_flags = WV_INUSE;

    dosflgs = (A_SIZE | A_TEMP);
    rw = (wvp->wv_wdp->wd_flags & W_BORDERS) ? BRWIDTH : 0;
    cw = (wvp->wv_wdp->wd_flags & W_BORDERS) ? BCWIDTH : 0;
    if((wvp->wv_vnum = s_vccreate(dosflgs, dcnum, rpw->r_nrow,
      rpw->r_ncol, rw, rw, cw, cw)) < SUCCESS)
    {
	rcode = wvp->wv_vnum;
	killvcon(vc);
	return;
    }

    if((vc == WNDWMSSG) || (vc == WNDWSTAT))
    {
	if((rcode = s_order(ORD_BTM, wvp->wv_vnum)) < SUCCESS)
	{
	    killvcon(vc);
	    return;
	}
    }

					  /* get virtual console information */
    getvcinfo(vc);
					       /* set view parms to top/left */
    vcp->vc_viewrow = RMIN;
    vcp->vc_viewcol = CMIN;
							  /* set window size */
    vcp->vc_nrow =  rpw->r_nrow;
    vcp->vc_ncol =  rpw->r_ncol;
						     /* set position row/col */
    vcp->vc_posrow = wvp->wv_wdp->wd_rorg;
    vcp->vc_poscol = wvp->wv_wdp->wd_corg;
    if(wvp->wv_wdp->wd_flags & W_BORDERS)
    {
	vcp->vc_posrow += BRWIDTH;
	vcp->vc_poscol += BCWIDTH;
    }
					 		/* open console file */
    bldspec(vc, (S_DEV|S_VCP|S_CON), specbuf);
    if((wvp->wv_cnum = s_open((A_READ|A_WRITE|A_SET|A_SHARE|A_SHFP),
      (LONG)specbuf)) < SUCCESS)
    {
	rcode = wvp->wv_cnum;
	killvcon(vc);
	return;
    }

						 /* set flags for copy/alter */
    dosflgs =
      (pcns.pc_planes & wvp->wv_wdp->wd_flags & W_ATTRIBS) ?
      (A_CHARPL|A_ATTRPL) : A_CHARPL;

    alterb[ALB_CAND] = alterb[ALB_AAND] = alterb[ALB_EAND] =
      alterb[ALB_EXOR] = 0;

						  /* handle borders if there */
    if(wvp->wv_wdp->wd_flags & W_BORDERS)
    {
								 /* open TOP */
	bldspec(vc, (S_DEV|S_VCP|S_TOP), specbuf);
	if((wvp->wv_tnum = s_open(A_WRITE, (LONG)specbuf)) < SUCCESS)
	{
	    rcode = wvp->wv_tnum;
	    killvcon(vc);
	    return;
	}
								 /* open BTM */
	bldspec(vc, (S_DEV|S_VCP|S_BTM), specbuf);
	if((wvp->wv_bnum = s_open(A_WRITE, (LONG)specbuf)) < SUCCESS)
	{
	    rcode = wvp->wv_bnum;
	    killvcon(vc);
	    return;
	}
								 /* open LFT */
	bldspec(vc, (S_DEV|S_VCP|S_LFT), specbuf);
	if((wvp->wv_lnum = s_open(A_WRITE, (LONG)specbuf)) < SUCCESS)
	{
	    rcode = wvp->wv_lnum;
	    killvcon(vc);
	    return;
	}
								 /* open RGT */
	bldspec(vc, (S_DEV|S_VCP|S_RGT), specbuf);
	if((wvp->wv_rnum = s_open(A_WRITE, (LONG)specbuf)) < SUCCESS)
	{
	    rcode = wvp->wv_rnum;
	    killvcon(vc);
	    return;
	}
					/* center the top border header text */
	if(center((rpw->r_ncol + (BCWIDTH * 2)), wvp->wv_wdp->wd_tbhdr,
	  topbuf, wvp->wv_wdp->wd_tfill))
	{
	    rcode = (UR_SOURCE | UR_PARM);
	    killvcon(vc);
	    return;
	}
							 /* copy TOP pattern */
	if((rcode = s_copy(dosflgs, wvp->wv_tnum, (LONG)0,
	  (LONG)&(wvp->wv_rtbb), (LONG)&(wvp->wv_ftop),
	  (LONG)&(wvp->wv_rtbb))) < SUCCESS)
	{
	    killvcon(vc);
	    return;
	}
							  /* set BTM pattern */
	alterb[ALB_CXOR] = wvp->wv_wdp->wd_bfill;
	alterb[ALB_AXOR] = (dosflgs & A_ATTRPL) ? wvp->wv_wdp->wd_battr : 0;
	if((rcode = s_alter(dosflgs, wvp->wv_bnum, (LONG)0,
	  (LONG)&(wvp->wv_rtbb), (LONG)alterb)) < SUCCESS)
	{
	    killvcon(vc);
	    return;
	}
							  /* set LFT pattern */
	alterb[ALB_CXOR] = wvp->wv_wdp->wd_lfill;
	alterb[ALB_AXOR] = (dosflgs & A_ATTRPL) ? wvp->wv_wdp->wd_battr : 0;
	if((rcode = s_alter(dosflgs, wvp->wv_lnum, (LONG)0,
	  (LONG)&(wvp->wv_rlrb), (LONG)alterb)) < SUCCESS)
	{
	    killvcon(vc);
	    return;
	}
							  /* set RGT pattern */
	alterb[ALB_CXOR] = wvp->wv_wdp->wd_rfill;
	alterb[ALB_AXOR] = (dosflgs & A_ATTRPL) ? wvp->wv_wdp->wd_battr : 0;
	if((rcode = s_alter(dosflgs, wvp->wv_rnum, (LONG)0,
	  (LONG)&(wvp->wv_rlrb), (LONG)alterb)) < SUCCESS)
	{
	    killvcon(vc);
	    return;
	}
    }
						  /* clear the entire window */
    alterb[ALB_CXOR] = wvp->wv_wdp->wd_wfill;
    alterb[ALB_AXOR] = (dosflgs & A_ATTRPL) ? wvp->wv_wdp->wd_wattr : 0;
    if((rcode = s_alter(dosflgs, wvp->wv_cnum, (LONG)0,
      (LONG)&(wvp->wv_rwndw), (LONG)alterb)) < SUCCESS)
    {
	killvcon(vc);
	return;
    }

				 /* send out ESCAPE sequences for attributes */
    setattrib(wvp);

		     /* close USER windows, shut off wrap on SPECIAL windows */
    if((vc != WNDWMSSG) && (vc != WNDWSTAT))
    {
	s_close(0x0000, wvp->wv_cnum);
	wvp->wv_cnum = FNUMINIT;
    }
    else
	s_write(A_EOFOFF, wvp->wv_cnum, (LONG)wm9009, utslen(wm9009), (LONG)0);

					 /* set the vcon tbl with new values */
    setvcinfo(vc);

}

/* wmshell() : fire off a shell for window */

VOID wmshell(vc)

REG WORD	vc;

{

    BYTE	buf1[BUFSIZ], buf2[BUFSIZ];
    REG WORD	dosflgs;
    PINFO	p;
    PROCESS	proc;
    REG WVARS	*wvp;

    wvp = &(wvars[vc]);

							  /* define new con: */
    bldspec(vc, (S_DEV|S_VCP), buf1);
    if((rcode = s_define(0x0000, (LONG)wm8000, (LONG)buf1,
      utslen(buf1))) < SUCCESS)
    {
	killvcon(vc);
	return;
    }

/* NOTE: This causes STDIO to be redefined indirectly, since it is defined   */
/*	 on entry to con:/console (which is also what "con" is defined as).  */

					       /* get defined name for SHELL */
    if((rcode = s_lookup(T_PNAME, 0x0000, (LONG)wm8100, (LONG)buf1,
      (LONG)BUFSIZ, (LONG)BUFSIZ, (LONG)0)) < SUCCESS)
    {
	killvcon(vc);
	return;
    }

					/* setup PINFO structure for COMMAND */
    p.pi_addm = 0;
    p.pi_maxm = PIMAXM;
    p.pi_prior = SHLPRI;
    utscopy(p.pi_pname, wm8100);

    wvp->wv_pid = wvp->wv_fid = 0;
					     /* create and open message pipe */
    bldspec(vc, S_PIP, buf2);
    dosflgs = (A_SET|A_READ|A_SHARE|A_TEMP|A_SECURITY);
    wvp->wv_pnum = s_create((BYTE)0x00, dosflgs, (LONG)buf2, 0x0000,
      (FS_OD|FS_OE|FS_OW|FS_OR), CMAXL);

/* NOTE: CMAXL must be at least as large as an MPHDR structure. */


					      /* define wmessage = pipe name */
    s_define(0x0000, (LONG)wm8110, (LONG)buf2, utslen(buf2));

			      /* async read message header from wvp->wv_pnum */
    if((etable[vc + MAXVC] = e_read((LONG)0, A_FPOFF, wvp->wv_pnum,
      (LONG)&(wvp->wv_mph), (LONG)sizeof(wvp->wv_mph), (LONG)0)) < SUCCESS)
	etable[vc + MAXVC] = 0;

    utscopy(buf2, wvp->wv_wdp->wd_cmdline);

					       /* start up the SHELL process */
    if((etable[vc] = e_command((LONG)0, (LONG)&(wvp->wv_pid), A_NEWFMLY, buf1,
      buf2, (LONG)BUFSIZ, (LONG)&p)) < SUCCESS)
    {
	rcode = etable[vc];
	etable[vc] = 0;
	killvcon(vc);
	return;
    }
						  /* get process's family id */
    s_get(T_PROC, wvp->wv_pid, (LONG)&proc, (LONG)sizeof(proc));
    wvp->wv_fid = proc.p_fid;

}

/* message() : display message and handle response */

VOID message(rvc)

REG WORD	rvc;					   /* requestor vcon */

{

    BYTE		vbuf[LBUFSIZ];
    BYTE		specbuf[BUFSIZ];
    BYTE		msgbuf[CMAXW + 1];
    BYTE		*ptr, *wptr;
    BYTE		*parms[PARMAX];
    WORD		i;
    WORD		linecnt;
    BOOLEAN		rflag;
    LONG		msgcnt, readcnt, wrtcnt;
    LONG		emask;
    RECT		hdrect;
    RECT		*rpi, *rpm, *rpp;
    PROCESS		proc;
    REG WVARS		*wvp, *wvc;
    REG WNDWSPEC	*wsp;


    if((mssgact) || (rcode != (LONG)sizeof(MPHDR)))
	return;

    if(++tlvl >= LTOPMAX)
	utfarjmp(jmpbuf, (UR_SOURCE|UR_INTERNAL));

				      /* activate and display message window */
    top(WNDWMSSG);

					   /* initialize important variables */
    mssgact = TRUE;
    wvp = &(wvars[WNDWMSSG]);
    wvc = &(wvars[rvc]);
    wsp = &ws_mssg;
    rpi = &r_minfoa;
    rpm = &r_mmorea;
    rpp = &r_mpmpta;

    utclrprm(parms);

    hdrect.r_row = wvp->wv_rwndw.r_row;
    hdrect.r_col = wvp->wv_rwndw.r_col;
    hdrect.r_ncol = wvp->wv_rwndw.r_ncol;
    hdrect.r_nrow = wsp->ws_numhdr;

						     /* get the process name */
    proc.p_name[0] = NULL;
    s_get(T_PROC, wvc->wv_mph.mp_pid, (LONG)&proc, (LONG)sizeof(proc));

					  /* null terminate the process name */
    ptr = (BYTE *)(proc.p_name + NAME1 - 1);
    while((ptr >= proc.p_name) && ((*ptr == wm9004) || (*ptr == NULL)))
	--ptr;
    *++ptr = NULL;

    parms[1] = proc.p_name;
					  /* convert vcnum to printable form */
    utui2ds(wvc->wv_vct.vc_vcnum, vbuf);
    parms[2] = utskpwht(fixnum(vbuf, BYTESIZ));
						  /* print the window header */
    setcurpos(wvp->wv_cnum, hdrect.r_row, hdrect.r_col);
    prntext(wsp->ws_numhdr, wsp->ws_whdr, wvp->wv_cnum, parms);

    linecnt = 0;
    rflag = TRUE;
    utclrprm(parms);
    msgcnt = wvc->wv_mph.mp_msgsiz;
    setcurpos(wvp->wv_cnum, rpi->r_row, rpi->r_col);
    while((msgcnt) && (linecnt < wsp->ws_actinfo))
    {
	if(rflag)
	{
	    readcnt = (msgcnt > CMAXL) ? CMAXL : msgcnt;
	    if((etable[rvc + MAXVC] = e_read((LONG)0, 0x0000, wvc->wv_pnum,
	      (LONG)&msgbuf, readcnt, (LONG)0)) < SUCCESS)
	    {
		etable[rvc + MAXVC] = 0;
		msgcnt = 0;
		break;
	    }
	}

	emask = s_wait(etable[rvc + MAXVC] | etable[OSEVKEYBD]);

	rflag = FALSE;
	if(emask & etable[OSEVKEYBD])
	{
	    rcode = s_return(etable[OSEVKEYBD]);
	    etable[OSEVKEYBD] = 0;

	    kbevent();
	}

	if(emask & etable[rvc + MAXVC])
	{
	    rflag = TRUE;
	    readcnt = s_return(etable[rvc + MAXVC]);
	    etable[rvc + MAXVC] = 0;

	    if(readcnt <= 0)
	    {
		msgcnt = 0;
		break;
	    }

	    wptr = msgbuf;
	    msgcnt -= readcnt;

	    for(ptr = msgbuf, wrtcnt = 0;
	      ((readcnt) && (linecnt<wsp->ws_actinfo));
	      ++ptr, --readcnt)
	    {
		++wrtcnt;

		if(*ptr == CH_NL)
		{
		    s_write(A_EOFOFF,wvp->wv_cnum,(LONG)wptr,wrtcnt,(LONG)0);
		    wptr = ptr + 1;
		    wrtcnt = 0;
		    ++linecnt;
		}
	    }

	    if(wrtcnt)
		s_write(A_EOFOFF,wvp->wv_cnum,(LONG)wptr,wrtcnt,(LONG)0);

	}
    }

    clrwrect(wvp, rpp);
    setcurpos(wvp->wv_cnum, rpp->r_row, rpp->r_col);
					  /* prompt for response or continue */
    ptr = (wvc->wv_mph.mp_rspflg) ? wm0260[0] : wm0260[1];
    utprnmsg(ptr, parms, wvp->wv_cnum);

			     /* read in response, command or acknowledgement */
    readcnt = erdelim(wvp->wv_cnum, msgbuf, CMAXL);

    if(readcnt < 0)
    {
	msgbuf[0] = CH_CR;
	readcnt = 1;
    }

    ptr = (BYTE *)&msgbuf[(WORD)(readcnt - 1)];

    if(*ptr != CH_CR)
    {
	*++ptr = CH_CR;
	++readcnt;
    }

    if(wvc->wv_mph.mp_rspflg)
    {
	utscopy(specbuf, wm8130);
	utscat(specbuf, wvc->wv_mph.mp_rspname);

	if((rsppfnum = s_open((A_WRITE|A_SHARE), (LONG)specbuf)) >= SUCCESS)
	{
	    if((etable[OSEVRSPP] = e_write((LONG)0, A_BOFOFF, rsppfnum,
	      (LONG)msgbuf, readcnt, (LONG)0)) < SUCCESS)
		etable[OSEVRSPP] = 0;

	    s_close(0x0000, rsppfnum);
	    rsppfnum = FNUMINIT;
	}
    }
						/* un-display message window */
    if(--tlvl < 0)
	utfarjmp(jmpbuf, (UR_SOURCE|UR_INTERNAL));

    bottom(WNDWMSSG);

    mssgact = FALSE;

					     /* init dynamic areas of window */
    clrwrect(wvp, &hdrect);
    clrwrect(wvp, rpi);
    clrwrect(wvp, rpm);
    clrwrect(wvp, rpp);
	
		/* restart async read from pipe for next app. message */

    if((etable[rvc + MAXVC] = e_read((LONG)0, A_FPOFF, wvc->wv_pnum,
     (LONG)&(wvc->wv_mph), (LONG)sizeof(wvc->wv_mph), (LONG)0)) < SUCCESS)
	etable[rvc + MAXVC] = 0;
}

/* status() : display status window and respond to keyboard input for same */

VOID status()
{

    BYTE		*ptr, *ptr1;
    BYTE		fbuf[LBUFSIZ];
    BYTE		pbuf[LBUFSIZ];
    BYTE		vbuf[LBUFSIZ];
    BYTE		*parms[PARMAX];
    BYTE		specbuf[BUFSIZ * 2];
    WORD		i;
    LONG		key, nfound;
    LONG		tkey, tnf;
    RECT		*rpi, *rpm, *rpp;
    CMDENV		cmd;
    PROCESS		proc;
    REG WORD		pcnt;
    REG BOOLEAN		more;
    REG WVARS		*wvp;
    REG WNDWSPEC	*wsp;


/* Add some code to display some info if mssgact ? */

    if(++tlvl >= LTOPMAX)
	utfarjmp(jmpbuf, (UR_SOURCE|UR_INTERNAL));

				       /* activate and display status window */
    top(WNDWSTAT);

    statact = TRUE;
    wvp = &(wvars[WNDWSTAT]);
    wsp = &ws_stat;
    rpi = &r_sinfoa;
    rpm = &r_smorea;
    rpp = &r_spmpta;

    utclrprm(parms);

    key = 0;
    pcnt = 0;
    nfound = 1;
    more = FALSE;
    setcurpos(wvp->wv_cnum, rpi->r_row, rpi->r_col);
    for(i = 0; i < (MAXVC - WNDWUSR1); ++i)
	pflags[i] = FALSE;

    while(nfound || more)		       /* while something to display */
    {
	if((!pcnt) && (!more) && (mssgact))
	{
	    ptr = wm0299;
	    ptr1 = (BYTE *)(&(ptr));
	    utui2ds((WORD)wvars[WNDWMSSG].wv_vct.vc_vcnum, vbuf);
	    parms[1] = fixnum(vbuf, BYTESIZ);
	    prntext(1, ptr1, wvp->wv_cnum, parms);
	    ++pcnt;
	}

	if((nfound = s_lookup(T_PROC, 0x0000, (LONG)wm9003, (LONG)&proc,
	  (LONG)sizeof(proc), (LONG)sizeof(proc), key)) < SUCCESS)
	    nfound = 0;

	key = proc.p_pid;

	if(nfound)			   /* something found, displayable ? */
	{
	    if(chksproc(&proc, &i))
	    {

		pflags[i - WNDWUSR1] = TRUE;

		utui2ds(wvars[i].wv_vct.vc_vcnum, vbuf);
		utui2ds(proc.p_fid, fbuf);
		utul2ds(proc.p_pid, pbuf);

		parms[1] = fixnum(vbuf, BYTESIZ);
		parms[2] = fixnum(fbuf, WORDSIZ);
		parms[3] = fixnum(pbuf, LONGSIZ);

					/* get command info for this process */
		cmd.cmd_file[0] = cmd.cmd_string[0] = NULL;
		s_get(T_CMD,proc.p_pid,(LONG)&cmd,(LONG)sizeof(cmd));

						     /* isolate process name */
		ptr = specbuf;
		ptr1 = proc.p_name;
		for(i = 0; i < NAME1; ++i)
		    *ptr++ = *ptr1++;
		--ptr;
		while((ptr >= specbuf) &&
		  ((*ptr == wm9004) || (*ptr == NULL)))
		    --ptr;
		*++ptr = wm9004;
		*++ptr = NULL;

					      /* process name & command tail */
		utscat(specbuf, cmd.cmd_string);
		parms[4] = specbuf;

					      /* print a line of status info */
		ptr = (BYTE *)&wsp->ws_inftxt;
		prntext(1, ptr, wvp->wv_cnum, parms);

		++pcnt;
		if((pcnt >= wsp->ws_actinfo) && (!more))
		{
		    tnf = 1;
		    tkey = key;
		    while(tnf)
		    {
			if((tnf = s_lookup(T_PROC, 0x0000, (LONG)wm9003,
			  (LONG)&proc, (LONG)sizeof(proc), (LONG)sizeof(proc),
			  tkey)) < SUCCESS)
			    tnf = 0;

			tkey = proc.p_pid;

			if(tnf)
			{
			    if(chksproc(&proc, &i))
			    {
				nfound = 1;
				more = TRUE;
				utclrprm(parms);
				setcurpos(wvp->wv_cnum,rpm->r_row,rpm->r_col);
				prntext(wsp->ws_nummore, wsp->ws_mrtxt,
				  wvp->wv_cnum, parms);
				break;
			    }
			    else
				nfound = 0;
			}
		    }
		}
	    }
	}

	if((!nfound) || (pcnt >= wsp->ws_actinfo))
	{
	    if(statcmd(wvp, wsp, rpp, more))
	    {
		nfound = 0;
		more = FALSE;
	    }
	}

	if(nfound || more)
	{
	    if(!nfound)
	    {
		key = 0;
		nfound = 1;
		more = FALSE;
		clrwrect(wvp, rpm);
		for(i = 0; i < (MAXVC - WNDWUSR1); ++i)
		    pflags[i] = FALSE;
		pcnt = wsp->ws_actinfo;
	    }
	    if(pcnt >= wsp->ws_actinfo)
	    {
		pcnt = 0;
		clrwrect(wvp, rpi);
		setcurpos(wvp->wv_cnum, rpi->r_row, rpi->r_col);
	    } 
	}
    }

						 /* un-display status window */
    if(--tlvl < 0)
	utfarjmp(jmpbuf, (UR_SOURCE|UR_INTERNAL));

    bottom(WNDWSTAT);

    statact = FALSE;
						      /* clear dynamic areas */
    clrwrect(wvp, rpi);
    clrwrect(wvp, rpm);
    clrwrect(wvp, rpp);
}

/* printscrn() : print view of top screen */

VOID printscrn()
{

    MPB			mpb;
    RECT		rect;
    FRAME		frame;

    REG	WVARS		*wvp;
    REG VCONSOLE	*vcp;

    if(ps_active)		     /* ignore request if already active !!! */
	return;

    ps_rcnt = 0;			    /* clear the printed row counter */
    kcflag = TRUE;

    getvcinfo(wtop);			/* get current info about TOP window */

    wvp = &(wvars[wtop]);
    vcp = &(wvp->wv_vct);

    rect.r_row = RMIN;
    rect.r_col = CMIN;

    frame.fr_use = PL_USE0;
    frame.fr_pl[PL_ATTR] = frame.fr_pl[PL_EXTN] = (LONG)0;

    ps_nrow = rect.r_nrow = frame.fr_nrow = vcp->vc_nrow;  /* save size info */
    ps_ncol = rect.r_ncol = frame.fr_ncol = vcp->vc_ncol;

    mpb.mpb_start = 0;			   /* set up MPB for s_malloc() call */
    mpb.mpb_minact = mpb.mpb_max = (ps_nrow * ps_ncol);

    if((rcode = s_malloc(SMO_NEW, (LONG)&mpb)) < SUCCESS)
	return;

						      /* set buffer pointers */
    ps_bptr = ps_bbase = frame.fr_pl[PL_CHAR] = (BYTE *)mpb.mpb_start;

					  /* copy data from screen to buffer */
    if((rcode = s_copy(A_CHARPL, wvp->wv_vnum, (LONG)&frame, (LONG)&rect,
      (LONG)0, (LONG)&rect)) < SUCCESS)
    {
	s_mfree((LONG)ps_bbase);
	return;
    }

							     /* open up prn: */
    if((ps_fnum = s_open(A_WRITE, (LONG)wm8170)) < SUCCESS)
    {
	s_mfree((LONG)ps_bbase);
	return;
    }

    ps_active = TRUE;		     /* set activity flag and accept request */
    wrtpline();					 /* write the first line out */
}

/* create() : create a new virtual console */

VOID	create(vc)

REG WORD	vc;

{

    LONG		fnum;
    REG WORD		dosflgs;
    REG	RECT		*rp;
    REG WVARS		*wvp;

						       /* create a USRx vcon */
    newvcon(vc);
						   /* put this window on top */
    top(vc);
							 /* fire off a shell */
    wmshell(vc);
}

/* */

