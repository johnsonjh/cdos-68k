
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
 *   Version 2.1        WMUTLS.C				 *
 *                      Window Manager utilities.		 *
 *---------------------------------------------------------------*
 *    VERSION   DATE    BY      CHANGE/COMMENTS                  *
 *---------------------------------------------------------------*
 *                                                               *
 *	2.1   02/12/86  MA	Don't echo ANY control chars in  *
 *				erdelim().			 *
 *	2.0   10/3/85	DR-K	in wrtpline() made ps_lbuf into  *
 *				static so the e_write() returning*
 *				won't deallocate the data buffer *
 *				before we're done using	it	 *
 *	1.9   08/14/85	jsr	Modified getpcns() and bldspec() *
 *				to allow dcnum to be a vcon.	 *
 *	1.8   07/29/85	jsr	Fixed s_give problem.		 *
 *	1.7   07/23/85	jsr	Fixed invalid window # check in	 *
 *				statcmd(), allow CR to cause	 *
 *				from STATUS window, changed for	 *
 *				CMAXW stuff and CCUTLS change,	 *
 *				fixed bug in center() routine,	 *
 *				handle echo in erdelim().	 *
 *	1.6   07/17/85	jsr	Print and accept actual wndw #s, *
 *				flush keyboard on special wndw	 *
 *				reads, fixed helpmsg() bldspec() *
 *				call, and cleaned up event stuff.*
 *				Fixed fixnum()with 0 problem.	 *
 *	1.5   06/06/85	jsr	Fixed: killall(),		 *
 *	1.4   05/10/85	jsr	Removed error checks from	 *
 *				setconmode() since it is now in	 *
 *				the exit path.
 *	1.3   05/10/85	jsr	Fixed input problem in invcmd()	 *
 *				and added unique mpipe names.	 *
 *	1.2   05/06/85	jsr	Added logical window #'s,	 *
 *				modified create(vc) error cntl,	 *
 *				and fixed setattrib() problem.	 *
 *	1.1   04/26/85	jsr	Added setattrib() procedure.	 *
 *				Also async error code and	 *
 *				removed FF from PRINTSCREEN.	 *
 *	1.0   04/23/85	jsr					 *
 *                                                               *
 *===============================================================*/

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


/* cast local procedures */

BYTE	*fixnum();

WORD	getpcns();
WORD	nextvc();

BOOLEAN	center();
BOOLEAN	chksproc();
BOOLEAN	statcmd();

LONG	erdelim();

VOID	abortall();
VOID	bldspec();
VOID	clrwrect();
VOID	crlfs();
VOID	dopmpt();
VOID	getvcinfo();
VOID	helpmsg();
VOID	invcmd();
VOID	kbevent();
VOID	killall();
VOID	killvcon();
VOID	noabt();
VOID	prnstatic();
VOID	prntext();
VOID	setattrib();
VOID	setconmode();
VOID	setcurpos();
VOID	setvcinfo();
VOID	top();
VOID	vcswitch();
VOID	wrtpline();

/* declare global variables from WMEX.C */
						    /* general use variables */
EXTERN	BYTE	mygroup;			       /* LOGIN group number */
EXTERN	BYTE	myuser;					/* LOGIN user number */
EXTERN	BYTE	pathchar[];					 /* pathchar */
EXTERN	BYTE	topbuf[];		   /* buffer for top border assembly */
EXTERN	BYTE	pconame[];			 /* name of physical console */

EXTERN	WORD	tlvl;			       /* top level index for ltop[] */

EXTERN	UWORD	ch16;				    /* 16 bit character cell */
EXTERN	UWORD	wtop;				  /* Screen currently on top */
EXTERN	UWORD	ltop[];				       /* Last screen on top */
EXTERN	UWORD	nstart;			 /* number of USRx wndws to start up */
EXTERN	UWORD	cnrows;				 /* nrows from CONSOLE table */
EXTERN	UWORD	cncols;				 /* ncols from CONSOLE table */
EXTERN	UWORD	viewrow;			/* viewrow from PCON or VCON */
EXTERN	UWORD	viewcol;			/* viewcol from PCON or VCON */

EXTERN	BOOLEAN	fninit;			     /* set if all FNUMs initialized */
EXTERN	BOOLEAN	pflags[];	      /* array of process found flags (STAT) */
EXTERN	BOOLEAN	logoff;					 /* logoff requested */
EXTERN	BOOLEAN	exitflg;					/* exit flag */
EXTERN	BOOLEAN	mssgact;			       /* WNDWMSSG is active */
EXTERN	BOOLEAN	statact;			       /* WNDWSTAT is active */
EXTERN	BOOLEAN	killstat;			   /* exit the STATUS window */
EXTERN	BOOLEAN	kcflag;		   /* set for STATUS wndw on most kctrl keys */

EXTERN	LONG	rcode;					 /* wmex return code */
EXTERN	LONG	wmexpid;				      /* pid of WMEX */
EXTERN	LONG	etable[];			     /* array of event masks */

EXTERN	MPIPE	mpipe;			/* message pipe structure for <HELP> */

EXTERN	WVARS	wvars[];	      /* array of window variable structures */

EXTERN	PCONSOLE pcns;					       /* pcns table */

EXTERN	jumpbuff	jmpbuf;			 /* error abort control info */


						     /* DESK window specials */
EXTERN	LONG	dcnum;				  /* DESK window file number */
EXTERN	RECT	r_dwndw;				 /* DESK window area */
EXTERN	FRAME	f_dwndw;			   /* DESK window area frame */

						     /* MSSG window specials */
EXTERN	LONG	rsppfnum;			/* response pipe file number */
EXTERN	RECT	r_minfoa;			    /* MSSG window info area */
EXTERN	RECT	r_mmorea;			    /* MSSG window more area */
EXTERN	RECT	r_mpmpta;			    /* MSSG window pmpt area */

						     /* STAT window specials */
EXTERN	RECT	r_sinfoa;			    /* STAT window info area */
EXTERN	RECT	r_smorea;			    /* STAT window more area */
EXTERN	RECT	r_spmpta;			    /* STAT window pmpt area */

						     /* PRINTSCREEN specials */
EXTERN	BYTE	*ps_bbase;		       /* base of PRINTSCREEN buffer */
EXTERN	BYTE	*ps_bptr;		    /* pointer to PRINTSCREEN buffer */
EXTERN	UWORD	ps_rcnt;			    /* count of printed rows */
EXTERN	UWORD	ps_nrow;		     /* number of rows to be printed */
EXTERN	UWORD	ps_ncol;	  /* number of columns to be printed per row */
EXTERN	BOOLEAN ps_active;			  /* PRINTSCREEN active flag */
EXTERN	LONG	ps_fnum;			      /* printer file number */

/* declare external procedures */
							      /* from WMEX.C */
EXTERN	VOID	printscrn();
							   /* from CCRTL.L86 */
EXTERN	LONG	s_abort();
EXTERN	LONG	s_alter();
EXTERN	LONG	s_cancel();
EXTERN	LONG	s_close();
EXTERN	LONG	s_define();
EXTERN	LONG	s_get();
EXTERN	LONG	s_give();
EXTERN	LONG	s_lookup();
EXTERN	LONG	s_mfree();
EXTERN	LONG	s_open();
EXTERN	LONG	s_order();
EXTERN	LONG	e_read();
EXTERN	LONG	s_return();
EXTERN	LONG	s_set();
EXTERN	LONG	s_swiret();
EXTERN	LONG	s_wait();
EXTERN	LONG	e_write();
EXTERN	LONG	s_write();
							  /* from CCUTLS.L86 */
EXTERN	BYTE	*utui2ds();
EXTERN	WORD	utds2i();
EXTERN	VOID	utclrprm();
EXTERN	VOID	utfarjmp();
EXTERN	VOID	utl2hs();
EXTERN	VOID	utscat();
EXTERN	VOID	utscopy();
EXTERN	LONG	utprnmsg();
EXTERN	LONG	utslen();

/* declare external variables */
							  /* from WMEXDATA.C */
EXTERN	UBYTE		wm0000[];
EXTERN	UBYTE		wm1000[];
EXTERN	UBYTE		wm2000, *wm2001[];
EXTERN	WNDWSPEC	ws_mssg, ws_stat;
							   /* from WMEXNUT.C */
EXTERN	UBYTE	wm8000[], wm8001[], wm8002[], wm8003[];
EXTERN	UBYTE	wm8004[], wm8005[], wm8006[], wm8007[];
EXTERN	UBYTE	wm8120[];
EXTERN	UBYTE	wm9000[], wm9003[], wm9004;
EXTERN	UBYTE	wm9005[], wm9006, wm9007[], wm9008[];
							  /* from CCMSGS.L86 */
EXTERN	BYTE	devdelim;

/* start of code */

/* top() : put window into foreground if not already there */

VOID top(vc)

REG WORD	vc;

{

    REG	WORD	index;
    REG	WVARS	*wvp;

    wvp = &(wvars[vc]);
    s_give(wvp->wv_vnum);

    if (vc != wtop)
    {
	index = tlvl;
	if((vc != WNDWMSSG) && (vc != WNDWSTAT))
	    index = 0;
	ltop[index] = wtop = vc;

	s_order(ORD_TOP, wvp->wv_vnum);
    }
}

/* bottom() : put window into background */

VOID bottom(vc)

REG WORD	vc;

{
    wtop = ltop[tlvl];
    s_give(wvars[wtop].wv_vnum);
    s_order(ORD_BTM, wvars[vc].wv_vnum);
}

/* getvcinfo() : get Virtual Console Info */

VOID getvcinfo(vc)

REG WORD	vc;

{

    REG WVARS	*wvp;

    wvp = &wvars[vc];

    if((rcode = s_get(T_VCON, wvp->wv_vnum, (LONG)&(wvp->wv_vct),
      (LONG)sizeof(wvp->wv_vct))) < SUCCESS)
	killvcon(vc);
}

/* setvcinfo() : set Virtual Console Info */

VOID setvcinfo(vc)

REG WORD	vc;	

{
    if((rcode = s_set(T_VCON, wvars[vc].wv_vnum, (LONG)&(wvars[vc].wv_vct),
      (LONG)sizeof(wvars[vc].wv_vct))) < SUCCESS)
	killvcon(vc);
}

/* vcswitch() : switch to another USER window */

VOID vcswitch(adder)

REG WORD	adder;

{

    REG WORD	i, j;

    j = 0;
    i = (wtop+adder);

    while(TRUE)
    {
        if(i >= MAXVC)
	{
	    i = (mssgact) ? WNDWMSSG : WNDWUSR1;
	    ++j;
	}

	if((i < WNDWUSR1) && (!mssgact))
	{
	    i = (MAXVC-1);
	    ++j;
	}

	if((i < WNDWMSSG) && (mssgact))
	{
	    i = (MAXVC-1);
	    ++j;
	}

	if ((j >= 2) || ((i != WNDWSTAT) && (wvars[i].wv_flags & WV_INUSE)))
	    break;

	i += adder;
    }

    if(j < 2)
	top(i);
}

/* killall() : close up all open files and exit */

VOID killall()
{

    REG	WORD	i, vc;
    LONG	cmask, dmask;

    exitflg = TRUE;

    if(fninit)
    {
	cmask = 0;

	for(i = OSEVMPST; i < NOSEVENTS; ++i)
	{
	    if(etable[i])
	    {
		cmask |= etable[i];
		etable[i] = 0;
	    }
	}

	if(cmask)
	{
	    if((dmask = s_cancel(cmask)) > 0)/* cncl all non-terminate evnts */
	    {
		cmask = 1;
		while(cmask)
		{
		    if(dmask & cmask)
			s_return(cmask);
		    cmask <<= 1;
		}
	    }
	}

	for(vc = WNDWUSR1; vc < MAXVC; ++vc)
	{
	    if(wvars[vc].wv_flags & WV_INUSE)
		abortall(vc);		       /* abort all FAMILY processes */
	    else
		killvcon(vc);			  /* clean up any open files */
	}
    }
}

/* killvcon() : delete a vcon */

VOID killvcon(vc)

REG WORD	vc;

{

    REG WORD	i;
    LONG	dmask;
    REG WVARS	*wvp;

    wvp = &(wvars[vc]);

    wvp->wv_flags &= ~WV_INUSE;

    if(wvp->wv_pnum != FNUMINIT)
    {
	if((vc >= WNDWSTAT) && (etable[vc + MAXVC]))
	{
						      /* cancel msgpipe read */
	    if((dmask = s_cancel(etable[vc + MAXVC])) > 0)
		s_return(dmask);
	    etable[vc+ MAXVC] = 0;
	}

	s_close(0x0000, wvp->wv_pnum);
	wvp->wv_pnum = FNUMINIT;
    }

    if(wvp->wv_tnum != FNUMINIT)
    {
	s_close(0x0000, wvp->wv_tnum);
	wvp->wv_tnum = FNUMINIT;
    }

    if(wvp->wv_bnum != FNUMINIT)
    {
	s_close(0x0000, wvp->wv_bnum);
	wvp->wv_bnum = FNUMINIT;
    }

    if(wvp->wv_lnum != FNUMINIT)
    {
	s_close(0x0000, wvp->wv_lnum);
	wvp->wv_lnum = FNUMINIT;
    }

    if(wvp->wv_rnum != FNUMINIT)
    {
	s_close(0x0000, wvp->wv_rnum);
	wvp->wv_rnum = FNUMINIT;
    }

    if(wvp->wv_cnum != FNUMINIT)
    {
	s_close(0x0000, wvp->wv_cnum);
	wvp->wv_cnum = FNUMINIT;
    }
						/* close up vcon last (TEMP) */
    if(wvp->wv_vnum != FNUMINIT)
    {
	s_close(0x0000, wvp->wv_vnum);
	wvp->wv_vnum = FNUMINIT;
    }

    if(vc == wtop)
    {
	if(wvars[ltop[0]].wv_flags & WV_INUSE)
	    top(ltop[0]);
	else
	{
	    for(i=WNDWUSR1;((i<MAXVC)&&(!(wvars[i].wv_flags & WV_INUSE)));++i);

	    if(i < MAXVC)
		top(i);
	}
    }
}

/* center() : create a centered message */

BOOLEAN center(ncols, hdr, buf, fill)

REG UWORD	ncols;					     /* window width */
REG BYTE	*hdr;					   /* header message */
REG BYTE	*buf;	 /* pointer to message assembly buffer (no lenchk !) */
BYTE		fill;					     /* filler value */

{

    REG WORD		i, j;
    LONG		hlen;

    if((hlen = utslen(hdr)) > ncols)
	return(TRUE);

    j = ncols - hlen;

    for(i = (j >> 1); i > 0; *buf++ = fill, --i);

    utscopy(buf, hdr);
    buf += utslen(buf);

    i = (j >> 1);
    if(j & 1)
	++i;

    for(; i > 0; *buf++ = fill, --i);

    return(FALSE);
}

/* getpcns() : get physical console information. Called ONLY by wminit(). */

WORD getpcns()
{

    BYTE		buf[BUFSIZ];
    REG WORD		vc;
    REG	BYTE		*ptr;

    vc = 0;
    ptr = buf;
    *ptr = NULL;
					       /* get the definition of con: */
    s_define(A_RETURN, (LONG)wm8000, (LONG)buf, (LONG)BUFSIZ);
    if(!*ptr)
	s_define((A_SYSTEM|A_RETURN), (LONG)wm8000, (LONG)buf, (LONG)BUFSIZ);
    if(!*ptr)
	utfarjmp(jmpbuf, (UR_SOURCE|UR_INTERNAL));

					 /* isolate the physical device name */
    ptr = (BYTE *)(ptr + utslen(buf));
    while((ptr >= buf) && (*ptr != devdelim))
	--ptr;
    ++ptr;
    if((*ptr == wm8001[0]) && (*(ptr + 1) == wm8001[1]))
	vc = utds2i(ptr + 2);				 /* save vcon number */
    *ptr = NULL;
							   /* save pcon name */
    utscopy(pconame, buf);

						 /* open the physical device */
    if((dcnum = s_open(0x0000, (LONG)buf)) < SUCCESS)
	utfarjmp(jmpbuf, dcnum);

				     /* get the physical console information */
    if((rcode = s_get(T_PCON,dcnum,(LONG)&pcns,(LONG)sizeof(pcns))) < SUCCESS)
	utfarjmp(jmpbuf, rcode);

					       /* close the physical console */
    s_close(0x0000, dcnum);
    dcnum = FNUMINIT;

    return(vc);
}

/* setconmode() : Set keyboard and screen mode */

VOID setconmode(fnum, sc, kb)

LONG		fnum;
REG UWORD	sc,kb;

{

    MINICON	ctable;

    s_get(T_CON, fnum, (LONG)&ctable, (LONG)sizeof(ctable));

    ctable.mc_sc = sc;
    ctable.mc_kb = kb;

    s_set(T_CON, fnum, (LONG)&ctable, (LONG)sizeof(ctable));
}

/* nextvc() : finds the first unused USRx console slot */

WORD	nextvc()
{

    REG WORD	vc;

    for (vc = WNDWUSR1;((vc < MAXVC) && (wvars[vc].wv_flags & WV_INUSE));++vc);

    if (vc >= MAXVC)
	return(ALLUSED);

    return(vc);
}

/* wrtpline() : write a line to printer for PRINTSCREEN */

BYTE	ps_lbuf[CMAXW+3];	      /* line buffer for PRINTSCREEN */

VOID wrtpline()
{

    REG BYTE	*ptr;
    REG UWORD	i;

    if(ps_rcnt >= ps_nrow)		    /* no more to write, so clean up */
    {
	s_mfree((LONG)ps_bbase);			/* free up PSCRN buf */
	s_close(0x0000, ps_fnum);			    /* close up prn: */

	ps_fnum = FNUMINIT;
	ps_rcnt = ps_nrow = ps_ncol = 0;
	ps_active = FALSE;
    }

    if(ps_rcnt < ps_nrow)			    /* more lines to write ? */
    {
	ptr = ps_lbuf;

	for(i = 0; i < ps_ncol; ++i, ++ptr, ++ps_bptr)
	    *ptr = *ps_bptr;			 /* copy line to line buffer */

	*ptr = NULL;
	utscat(ptr, wm9000);			       /* add an ending CRLF */

						     /* issue an async write */
	if((etable[OSEVPRNT] = e_write((LONG)0, A_BOFOFF, ps_fnum,
	  (LONG)ps_lbuf, utslen(ps_lbuf), (LONG)0)) < SUCCESS)
	{
	    rcode = etable[OSEVPRNT];
	    etable[OSEVPRNT] = 0;
	    utfarjmp(jmpbuf, rcode);
	}

	++ps_rcnt;			/* increment number of lines printed */
    }
}

/* bldspec() : build a specification name */

VOID bldspec(vc, code, buf)

REG WORD	vc;
REG UWORD	code;
REG BYTE	*buf;

{

    REG BYTE	*ptr;
    BYTE	nbuf[LBUFSIZ];

    *buf = NULL;

    if(code & S_DEV)
	utscat(buf, pconame);

    if(code & S_PIP)
    {
	utscat(buf, wm8120);
	utl2hs(wmexpid, nbuf);
	ptr = (BYTE *)((nbuf + utslen(nbuf)) - PCPIPSIZ);
	utscat(buf, ptr);
	ptr = utui2ds(wvars[vc].wv_vct.vc_vcnum, nbuf);
	ptr = (BYTE *)((ptr + utslen(ptr)) - VCPIPSIZ);
	utscat(buf, ptr);
    }

    if((code & S_VCP) || (code & S_VCR))
    {
	utscat(buf, wm8001);
	if(code & S_VCP)
	    ptr = utui2ds(wvars[vc].wv_vct.vc_vcnum, nbuf);
	else
	    ptr = utui2ds(vc, nbuf);
	ptr = (BYTE *)((ptr + utslen(ptr)) - BYTESIZ);
	utscat(buf, ptr);
	utscat(buf, pathchar);
    }

    if(code & S_CON)
	utscat(buf, wm8002);

    if(code & S_MSE)
	utscat(buf, wm8003);

    if(code & S_TOP)
	utscat(buf, wm8004);

    if(code & S_BTM)
	utscat(buf, wm8005);

    if(code & S_LFT)
	utscat(buf, wm8006);

    if(code & S_RGT)
	utscat(buf, wm8007);

}

/* prnstatic() : print out static data for special windows */

VOID prnstatic(vc)

REG WORD		vc;

{

    BYTE		*parms[PARMAX];
    REG WORD		i;
    REG	WORD		hrow;
    REG	RECT		*rpp;
    REG WVARS		*wvp;
    REG WNDWSPEC	*wsp;

    utclrprm(parms);
    wvp = &(wvars[vc]);

    switch(vc)
    {
	case WNDWMSSG:
	    wsp = &ws_mssg;
	    i = wsp->ws_numhdr + wsp->ws_actinfo + wsp->ws_nummore;
	    setcurpos(wvp->wv_cnum, wvp->wv_rwndw.r_row, wvp->wv_rwndw.r_col);
	    crlfs(i, wvp->wv_cnum);
	    rpp = &r_mpmpta;
	    break;

	case WNDWSTAT:
	    wsp = &ws_stat;
	    setcurpos(wvp->wv_cnum, wvp->wv_rwndw.r_row, wvp->wv_rwndw.r_col);
	    prntext(wsp->ws_numhdr, wsp->ws_whdr, wvp->wv_cnum, parms);
	    i = wsp->ws_actinfo + wsp->ws_nummore;
	    crlfs(i, wvp->wv_cnum);
	    rpp = &r_spmpta;
	    break;

	default:
	    utfarjmp(jmpbuf, (UR_SOURCE | UR_INTERNAL));
	    break;
    }

    hrow = (wvp->wv_wdp->wd_flags & W_PMPTHLP) ?
      rpp->r_row + wsp->ws_numpmpt : rpp->r_row - wsp->ws_numhelp;

    dopmpt(wvp, wsp, rpp);
    setcurpos(wvp->wv_cnum, hrow, rpp->r_col);
    prntext(wsp->ws_numhelp, wsp->ws_hptxt, wvp->wv_cnum, parms);
}

/* prntext() : print text */

VOID prntext(n, p, fnum, pp)

REG WORD	n;
REG BYTE	**p;
LONG		fnum;
BYTE		pp[];

{

    REG	WORD	i;

    for(i = 0; i < n; ++p, ++i)
    {
	utprnmsg(*p, pp, fnum);
	crlfs(1, fnum);
    }
}

/* dopmpt() : print prompt */

VOID dopmpt(wvp, wsp, rpp)

REG WVARS	*wvp;
REG WNDWSPEC	*wsp;
REG RECT	*rpp;

{

    BYTE	*parms[PARMAX];
    BYTE	**p;
    REG	WORD	i, n;

    utclrprm(parms);
    clrwrect(wvp, rpp);
    setcurpos(wvp->wv_cnum, rpp->r_row, rpp->r_col);

    p = wsp->ws_pmtxt;
    n = wsp->ws_numpmpt;

    for(i = 1; i <= n; ++p, ++i)
    {
	utprnmsg(*p, parms, wvp->wv_cnum);
	if(i != n)
	    crlfs(1, wvp->wv_cnum);
    }
}

/* crlfs() : print n carriage return/linefeeds to fnum */

VOID crlfs(n, fnum)

REG WORD	n;
LONG		fnum;

{

    LONG	len;
    REG	WORD	i;

    len = utslen(wm9000);
    for(i = 0; i < n; ++i)
	s_write(A_EOFOFF, fnum, (LONG)wm9000, len, (LONG)0);
}

/* clrwrect() : clear a window rectangle */

VOID clrwrect(wvp, rp)

REG WVARS	*wvp;
REG RECT	*rp;

{

    BYTE	alterb[PL_NPLANE * 2];
    REG	UWORD	dosflgs;
						    /* clear the window rect */
    dosflgs =
      (pcns.pc_planes & wvp->wv_wdp->wd_flags & W_ATTRIBS) ?
      (A_CHARPL|A_ATTRPL) : A_CHARPL;

    alterb[ALB_CXOR] = wvp->wv_wdp->wd_wfill;
    alterb[ALB_AXOR] = (dosflgs & A_ATTRPL) ? wvp->wv_wdp->wd_wattr : 0;
    alterb[ALB_CAND] = alterb[ALB_AAND] = alterb[ALB_EAND] =
      alterb[ALB_EXOR] = 0;

    if((rcode = s_alter(dosflgs, wvp->wv_cnum, (LONG)0,
      (LONG)rp, (LONG)alterb)) < SUCCESS)
	utfarjmp(jmpbuf, rcode);

}

/* setcurpos() : set cursor position */

VOID setcurpos(fnum, row, col)

LONG			fnum;
REG UWORD		row, col;

{

    MINICON		ctable;

    if((rcode = s_get(T_CON, fnum, (LONG)&ctable,
      (LONG)sizeof(ctable))) < SUCCESS)
	utfarjmp(jmpbuf, rcode);

    ctable.mc_currow = row;
    ctable.mc_curcol = col;

    if((rcode = s_set(T_CON, fnum, (LONG)&ctable,
      (LONG)sizeof(ctable))) < SUCCESS)
	utfarjmp(jmpbuf, rcode);
}

/* invcmd() : invalid command in special window */

VOID invcmd(cmd, wvp, wsp, rpp, pp)

REG WORD	cmd;
REG WVARS	*wvp;
REG WNDWSPEC	*wsp;
RECT		*rpp;
BYTE		*pp[];

{

    BYTE	c;
    REG	BYTE	**p;
    WORD	i;
    RECT	hrect;

					    /* calculate HELP info area RECT */
    hrect.r_col = rpp->r_col;
    hrect.r_ncol = rpp->r_ncol;
    hrect.r_nrow = wsp->ws_numhelp;
    hrect.r_row = (wvp->wv_wdp->wd_flags & W_PMPTHLP) ?
      rpp->r_row + wsp->ws_numpmpt : rpp->r_row - wsp->ws_numhelp;
						    /* clear HELP area info */
    clrwrect(wvp, &hrect);
    setcurpos(wvp->wv_cnum, hrect.r_row, hrect.r_col);

    for(i = cmd, p = wsp->ws_chtxt; i > 0; ++p, --i);
						  /* print command help text */
    utprnmsg(*p, pp, wvp->wv_cnum);
							/* clear PROMPT area */
    clrwrect(wvp, rpp);
    setcurpos(wvp->wv_cnum, rpp->r_row, rpp->r_col);
						   /* print continue message */
    utprnmsg(wm1000, pp, wvp->wv_cnum);
							    /* read keyboard */
    if((rcode = erdelim(wvp->wv_cnum, &c, (LONG)sizeof(c))) < SUCCESS)
	utfarjmp(jmpbuf, rcode);
					  /* restore original HELP info area */
    clrwrect(wvp, &hrect);
    setcurpos(wvp->wv_cnum, hrect.r_row, hrect.r_col);
    prntext(wsp->ws_numhelp, wsp->ws_hptxt, wvp->wv_cnum, pp);
}

/* statcmd() : handle STAT window commands (exit TRUE for STAT exit) */

BOOLEAN statcmd(wvp, wsp, rpp, more)

REG WVARS	*wvp;
REG WNDWSPEC	*wsp;
REG RECT	*rpp;
BOOLEAN		more;

{

    BYTE	inpbuf[4];
    BYTE	*parms[PARMAX];
    BYTE	*ptr, *ptr1;
    BYTE	**p;
    BOOLEAN	retry, statexit, invalid;
    REG	WORD	i, j;

    retry = TRUE;
    statexit = FALSE;
    utclrprm(parms);

    while(retry)
    {
	for(i = 0; i < 4; ++i)
	    inpbuf[i] = NULL;

	dopmpt(wvp, wsp, rpp);

	killstat = kcflag = FALSE;

							   /* get some input */
	if((rcode = erdelim(wvp->wv_cnum, inpbuf,
	  (LONG)sizeof(inpbuf))) < SUCCESS)
	    return(statexit);
					/* exit WNDWSTAT if <PREV> or <NEXT> */
	if(killstat)
	    return(TRUE);

	ptr = (BYTE *)(inpbuf + rcode - 1);

	if(*ptr != CH_CR)
	{
	    invcmd(SCMDUNK, wvp, wsp, rpp, parms);
	    continue;
	}

	if(rcode == 1)						/* CR only ? */
	    return(TRUE);			 /* exit STATUS WINDOW if so */

	switch(inpbuf[0])
	{
	    case CH_0:
	    case CH_1:
	    case CH_2:
	    case CH_3:
	    case CH_4:
	    case CH_5:
	    case CH_6:
	    case CH_7:
	    case CH_8:
	    case CH_9:

		*ptr = NULL;
		i = utds2i(inpbuf);		   /* convert numeric string */

		for(j = WNDWMSSG; j < MAXVC; ++j)
		    if((wvars[j].wv_flags & WV_INUSE) &&
		      (i == wvars[j].wv_vct.vc_vcnum))
			break;

		if( (j >= MAXVC) || (j == WNDWSTAT) ||
		  ((j == WNDWMSSG) && (!mssgact)) )
		{
		    invcmd(SCMDNUM, wvp, wsp, rpp, parms);
		    break;
		}

		top(j);
		retry = FALSE;
		statexit = TRUE;
		break;

	    case CH_C:
	    case CH_LC:
		if(inpbuf[1] != CH_CR)
		{
		    invcmd(SCMDUNK, wvp, wsp, rpp, parms);
		    break;
		}

		i = nextvc();

		if(i != ALLUSED)		  /* handle error if ALLUSED */
		{
		    create(i);
		    retry = FALSE;
		    statexit = TRUE;
		}

		else
		    invcmd(SCMDCRT, wvp, wsp, rpp, parms);

		break;

	    case CH_D:
	    case CH_LD:
		if(inpbuf[1] != CH_CR)
		{
		    invcmd(SCMDUNK, wvp, wsp, rpp, parms);
		    break;
		}

		invalid = TRUE;
		while(invalid)
		{
		    for(i = 0; i < 4; ++i)
			inpbuf[i] = NULL;

		    clrwrect(wvp, rpp);
		    setcurpos(wvp->wv_cnum, rpp->r_row, rpp->r_col);

		    for(i = SCMDDEL, p = wsp->ws_cptxt; i > 0; ++p, --i);
		    utprnmsg(*p, parms, wvp->wv_cnum);

		    if((rcode = erdelim(wvp->wv_cnum, inpbuf,
		      (LONG)sizeof(inpbuf))) < SUCCESS)
			return(statexit);

		    ptr = (BYTE *)(inpbuf + rcode - 1);

		    if(*ptr != CH_CR)
		    {
			invcmd(SCMDUNK, wvp, wsp, rpp, parms);
			continue;
		    }

		    if(rcode == 1)				/* CR only ? */
			return(TRUE);		 /* exit STATUS WINDOW if so */

		    switch(inpbuf[0])
		    {
			case CH_0:
			case CH_1:
			case CH_2:
			case CH_3:
			case CH_4:
			case CH_5:
			case CH_6:
			case CH_7:
			case CH_8:
			case CH_9:
			    *ptr = NULL;
			    i = utds2i(inpbuf);	   /* convert numeric string */

			    for(j = WNDWUSR1; j < MAXVC; ++j)
				if((wvars[j].wv_flags & WV_INUSE) &&
				  (i == wvars[j].wv_vct.vc_vcnum))
				    break;

			    if( (j >= MAXVC) || (j < WNDWUSR1) )
			    {
				invcmd(SCMDDEL, wvp, wsp, rpp, parms);
				break;
			    }

			    s_abort(wvars[j].wv_pid);
			    invalid = retry = FALSE;
			    break;

			default:
			    invcmd(SCMDDEL, wvp, wsp, rpp, parms);
			    break;
		    }
		}
		break;

	    case CH_M:
	    case CH_LM:
		if(inpbuf[1] != CH_CR)
		{
		    invcmd(SCMDUNK, wvp, wsp, rpp, parms);
		    break;
		}

		if(more)
		    retry = FALSE;
		break;

	    default:
		invcmd(SCMDUNK, wvp, wsp, rpp, parms);
		break;
	}
    }

    return(statexit);

}

/* chksproc() : check if status process meets requirements */
/*		Return TRUE if process okayed, and poke vc value. */

BOOLEAN chksproc(proc, vc)

REG PROCESS	*proc;
REG WORD	*vc;

{

    REG WORD	i;
    REG BOOLEAN	ret;

    ret = FALSE;
    if((proc->p_cuser == myuser) && (proc->p_cgroup == mygroup))
    {
			    /* my child process, but is vcon known to WMEX ? */
	for(i = WNDWUSR1; i < MAXVC; ++i)
	{
	    if((wvars[i].wv_vct.vc_vcnum == proc->p_vcid) &&
		      (wvars[i].wv_flags & WV_INUSE) &&
		      (!pflags[i - WNDWUSR1]))
	    {
		ret = TRUE;
		*vc = i;
		break;
	    }
	}
    }

    return(ret);

}

/* noabt() : dummy swi routine */

VOID noabt()
{
    s_swiret((LONG)0);
}

/* abortall() : abort all processes with vc's family id */

VOID abortall(vc)

REG WORD	vc;

{

    LONG	key, nfound;
    PROCESS	proc;
    REG WVARS	*wvp;

    wvp = &(wvars[vc]);

    key = 0;
    nfound = 1;
						      /* check all processes */
    while(nfound = s_lookup(T_PROC, 0x0000, (LONG)wm9003, (LONG)&proc,
      (LONG)sizeof(proc), (LONG)sizeof(proc), key))
    {

	if(nfound <= 0)
	    return;

	key = proc.p_pid;
			  /* if this process is in vc's family then abort it */
	if(proc.p_fid == wvp->wv_fid)
	    s_abort(proc.p_pid);
	
    }
}

/* fixnum() : convert leading zeros to spaces and return pointer for width */

BYTE *fixnum(buf, typesiz)

REG BYTE	*buf;
REG WORD	typesiz;

{

    REG BYTE	*ptr;

    ptr = buf;
    while((*ptr == wm9006) && (ptr < (BYTE *)(buf + LBUFSIZ - 2)))
	*ptr++ = wm9004;
    return((BYTE *)(buf + LBUFSIZ - typesiz - 1));
}

/* helpmsg() : issue <HELP> message */
					   /* send help info to message pipe */
VOID helpmsg()
{

    BYTE		specbuf[BUFSIZ];
    LONG		hpnum;
    REG	BYTE		*ptr, *ptr1;
    REG	WORD		i;

    if(!etable[OSEVHELP])		    /* no outstanding <HELP> event ? */
    {
	kcflag = TRUE;
	mpipe.mpi_msgsiz = 0;
	mpipe.mpi_pid = wmexpid;
	mpipe.mpi_rspflg = FALSE;
	mpipe.mpi_rspname[0] = NULL;

	ptr = mpipe.mpi_msgbuf;
	for(i = 0; i < wm2000; ++i)
	{
	    ptr1 = wm2001[i];
	    while(*ptr1)
	    {
		*ptr++ = *ptr1++;
		++mpipe.mpi_msgsiz;
	    }
	}

	bldspec(WNDWSTAT, S_PIP, specbuf);
	if((hpnum = s_open((A_WRITE|A_SHARE), (LONG)specbuf)) >= SUCCESS)
	{
	    if((etable[OSEVHELP] = e_write((LONG)0, A_FPOFF, hpnum,
	      (LONG)&mpipe, (LONG)(sizeof(MPHDR) + mpipe.mpi_msgsiz),
	      (LONG)0)) < SUCCESS)
	    	etable[OSEVHELP] = 0;

	    s_close(0x0000, hpnum);
	}
    }
}

/* kbevent() : handle special character keyboard events */

VOID kbevent()
{

    BOOLEAN	flag;

    if (rcode != (LONG)sizeof(ch16))
	utfarjmp(jmpbuf, (UR_SOURCE | UR_INTERNAL));


    flag = TRUE;
    switch(ch16)
    {
	case CI_HELP:
						   /* give back the keyboard */
	    if((rcode = s_give(wvars[wtop].wv_vnum)) < SUCCESS)
		utfarjmp(jmpbuf, rcode);

	    helpmsg();
	    break;

	case CI_WINDOW:
	    if(statact)
	    {
		top(WNDWSTAT);
		break;
	    }

	    flag = FALSE;
						   /* allow another kb event */
	    if((etable[OSEVKEYBD] = e_read((LONG)0, 0x0000,
	      dcnum, (LONG)&(ch16), (LONG)sizeof(ch16),
	      (LONG)0)) < SUCCESS)
	    {
		rcode = etable[OSEVKEYBD];
		etable[OSEVKEYBD] = 0;
		utfarjmp(jmpbuf, rcode);
	    }

	    status();
	    break;

	case CI_NEXT:
	    kcflag = TRUE;
	    vcswitch(NXTWNDW);
	    break;

	case CI_PREV:
	    kcflag = TRUE;
	    vcswitch(PRVWNDW);
	    break;

        case CI_PSCRN:
						   /* give back the keyboard */
	    if((rcode = s_give(wvars[wtop].wv_vnum)) < SUCCESS)
		utfarjmp(jmpbuf, rcode);

	    printscrn();
	    break;

	default:
	    utfarjmp(jmpbuf, (UR_SOURCE | UR_INTERNAL));
	    break;
    }

    if(flag)
    {
					       /* restart the keyboard event */
	if((etable[OSEVKEYBD] = e_read((LONG)0, 0x0000, dcnum,
	  (LONG)&ch16, (LONG)sizeof(ch16), (LONG)0)) < SUCCESS)
	{
	    rcode = etable[OSEVKEYBD];
	    etable[OSEVKEYBD] = 0;
	    utfarjmp(jmpbuf, rcode);
	}
    }
}

/* erdelim() : Asynchronous RDELIM with special character handling */
/* 	       For use on special window consoles. Also performs echo */

LONG erdelim(fnum, buf, bufsiz)

LONG	fnum;
BYTE	*buf;
LONG	bufsiz;

{

    BOOLEAN		sflag, rflag;
    LONG		ret, count, lclmask, dmask, emask;
    REG	BYTE		*ptr;

    ptr = buf;
    rflag = FALSE;
    count = lclmask = 0;
    sflag = (fnum == wvars[WNDWSTAT].wv_cnum) ? TRUE : FALSE;

    if((lclmask = e_read((LONG)0, A_FLUSH, fnum, (LONG)ptr,
      (LONG)sizeof(*ptr), (LONG)0)) < SUCCESS)
    {
	ret = lclmask;
	lclmask = 0;
	goto erdexit;
    }

    while(TRUE)
    {
	if(rflag)
	{
	    if((lclmask = e_read((LONG)0, 0x0000, fnum, (LONG)ptr,
	      (LONG)sizeof(*ptr), (LONG)0)) < SUCCESS)
	    {
		ret = lclmask;
		lclmask = 0;
		break;
	    }
	    rflag = FALSE;
	}

	emask = s_wait(lclmask | etable[OSEVKEYBD]);

	if(emask & lclmask)
	{
	    rflag = TRUE;

	    ret = s_return(lclmask);

	    lclmask = 0;

	    if(ret < SUCCESS)
		break;

	    if(*ptr != CH_BS)
	    {
/*		if(*ptr != CH_NL)  */
		if  (*ptr >= ' ')		/* MA 2/12/86 */
		    s_write(A_EOFOFF, fnum, (LONG)ptr,
		      (LONG)sizeof(*ptr), (LONG)0);

		++count;

		if((count >= bufsiz) || (*ptr == CH_CR))
		{
		    ret = count;
		    break;
		}
		*++ptr = NULL;
	    }

	    if(*ptr == CH_BS)
	    {
		if(count > 0)
		{
		    --ptr;
		    --count;
		    s_write(A_EOFOFF,fnum,(LONG)wm9005,utslen(wm9005),(LONG)0);
		}
	    }
	}

	if(emask & etable[OSEVKEYBD])
	{
	    rcode = s_return(etable[OSEVKEYBD]);
	    etable[OSEVKEYBD] = 0;

	    kbevent();

	    if(sflag && kcflag)
	    {
		if((dmask = s_cancel(lclmask)) > 0)
		    s_return(dmask);
		killstat = TRUE;
		lclmask = 0;
		ret = 0;
		break;
	    }
	}
    }

erdexit:
    return(ret);
}

/* setattrib() : send out ESCAPE sequences for window attributes */

VOID setattrib(wvp)

REG WVARS		*wvp;

{

    REG	BYTE		c;
    REG WNDWDESC	*wdp;
   
    wdp = wvp->wv_wdp;

    if(wdp->wd_flags & W_ATTRIBS)
    {
	c = wdp->wd_wattr;
				       /* poke attribute values into strings */
	wm9007[SAPOK] = c & 0x0f;
	wm9008[SAPOK] = ((c >> 4) & 0x0f);
					/* write out ESCAPE sequence strings */
	s_write(A_EOFOFF, wvp->wv_cnum, (LONG)wm9007,
	  (LONG)SALEN, (LONG)0);

	s_write(A_EOFOFF, wvp->wv_cnum, (LONG)wm9008,
	  (LONG)SALEN, (LONG)0);
    }
}

/* */

