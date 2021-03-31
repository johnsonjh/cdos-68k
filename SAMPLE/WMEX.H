
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
 *   Version 1.5        WMEX.H					 *
 *                      Defines for Window Manager.		 *
 *---------------------------------------------------------------*
 *    VERSION   DATE    BY      CHANGE/COMMENTS                  *
 *---------------------------------------------------------------*
 *                                                               *
 *	1.5   08/14/85	jsr	Added S_VCR define.		 *
 *	1.4   07/24/85	jsr	Removed CMAX, MSGSIZ & MSGPIPE,  *
 *				added HLPSIZ, CMAXW & CMAXL.	 *
 *	1.3   07/16/85	jsr	Added OSEVHELP & OSEVRSPP event	 *
 *				names.				 *
 *	1.2   05/10/85	jsr	Added NAMELEN, VCPIPSIZ and	 *
 *				PCPIPSIZ.			 *
 *	1.1   05/06/85	jsr	Added SAPOK and SALEN.		 *
 *	1.0   04/23/85	jsr					 *
 *                                                               *
 *===============================================================*/

#define MAXVC		9			   /* maximum vcons per pcon */

#define	WNDWDESK	0			      /* DESK window (vc000) */
#define	WNDWMSSG	1			      /* MSSG window (vc001) */
#define	WNDWSTAT	2			      /* STAT window (vc002) */
#define	WNDWUSR1	3			      /* USR1 window (vc003) */
#define WNDWUSR2	4			      /* USR2 window (vc004) */
#define	WNDWUSR3	5			      /* USR3 window (vc005) */
#define	WNDWUSR4	6			      /* USR4 window (vc006) */
#define	WNDWUSR5	7			      /* USR5 window (vc007) */
#define	WNDWUSR6	8			      /* USR6 window (vc008) */

#define	RMIN		0				      /* row minimum */
#define	CMIN		0				   /* column minimum */

#define	BRWIDTH		1				 /* ROW border width */
#define	BCWIDTH		1				 /* COL border width */

						     /* internal event types */
#define EV_MESSAGE	0
#define EV_WINDOW	1
#define EV_PREV		2
#define EV_NEXT		3
#define EV_CHAR		4
#define	EV_PRTSCRN	5
#define EV_PRINTER	6

							   /* OS event types */
#define	OSEVDESK	WNDWDESK  /* 0) not used by DESK, just a placeholder */
#define	OSEVMSSG	WNDWMSSG  /* 1) not used by MSSG, just a placeholder */
#define	OSEVSTAT	WNDWSTAT  /* 2) not used by STAT, just a placeholder */
#define	OSEVUSR1	WNDWUSR1	/* 3) USR1 shell process termination */
#define	OSEVUSR2	WNDWUSR2	/* 4) USR2 shell process termination */
#define	OSEVUSR3	WNDWUSR3	/* 5) USR3 shell process termination */
#define	OSEVUSR4	WNDWUSR4	/* 6) USR4 shell process termination */
#define	OSEVUSR5	WNDWUSR5	/* 7) USR5 shell process termination */
#define	OSEVUSR6	WNDWUSR6	/* 8) USR6 shell process termination */
#define	OSEVMPDK	WNDWDESK+MAXVC	/* 9) DESK placeholder, MSGPIPE read */
#define	OSEVMPMG	WNDWMSSG+MAXVC /* 10) MSSG placeholder, MSGPIPE read */
#define	OSEVMPST	WNDWSTAT+MAXVC 	     /* 11) STAT <HELP> MSGPIPE read */
#define	OSEVMPU1	WNDWUSR1+MAXVC		    /* 12) USR1 MSGPIPE read */
#define	OSEVMPU2	WNDWUSR2+MAXVC		    /* 13) USR2 MSGPIPE read */
#define	OSEVMPU3	WNDWUSR3+MAXVC		    /* 14) USR3 MSGPIPE read */
#define	OSEVMPU4	WNDWUSR4+MAXVC		    /* 15) USR4 MSGPIPE read */
#define	OSEVMPU5	WNDWUSR5+MAXVC		    /* 16) USR5 MSGPIPE read */
#define	OSEVMPU6	WNDWUSR6+MAXVC		    /* 17) USR6 MSGPIPE read */
#define	OSEVKEYBD	WNDWUSR6+MAXVC+1       /* 18) KEYBD async read event */
#define	OSEVPRNT	WNDWUSR6+MAXVC+2    /* 19) PRINTER async write event */
#define	OSEVHELP	WNDWUSR6+MAXVC+3    /* 20) STAT <HELP> MSGPIPE write */
#define	OSEVRSPP	WNDWUSR6+MAXVC+4	   /* 21) MSSG RSPPIPE write */
#define	NOSEVENTS	WNDWUSR6+MAXVC+5       /* 22) number of known events */

						/* Definition of flags field */
						 /* WVARS struct flag values */
#define WV_INUSE	0x0001

					      /* WNDWDESC struct flag values */
							     /* global flags */
#define	W_BORDERS	0x01		     /* BIT 0: 1 = borders, 0 = none */
#define	W_ATTRIBS	0x02		  /* BIT 1: 1 = attributes, 0 = none */
			     /* internal flags - not for use in USRx windows */
#define	W_INTERNF	0x40	     /* BIT 6: 1 = internal fields, 0 = none */
#define	W_PMPTHLP	0x80	      /* BIT 7: 1 = p1st/h2nd, 0 = h1st/p2nd */

						  /* flags for SPEC building */
#define	S_DEV		0x0001
#define	S_PIP		0x0002
#define	S_VCP		0x0004
#define	S_CON		0x0008
#define	S_MSE		0x0010
#define	S_TOP		0x0020
#define	S_BTM		0x0040
#define	S_LFT		0x0080
#define	S_RGT		0x0100
#define	S_VCR		0x0200

					      /* STAT window command numbers */
#define	SCMDUNK		0				  /* unknown command */
#define	SCMDNUM		1				   /* number command */
#define	SCMDCRT		2				   /* create command */
#define	SCMDDEL		3				   /* delete command */
#define	SCMDMOR		4				     /* more command */
#define	SCMDHLP		5				     /* help command */
#define SCMDPRV		6				 /* previous command */
#define	SCMDNXT		7				     /* next command */

					      /* MSSG window command numbers */
#define	MCMDUNK		0				  /* unknown command */

						/* miscellaneous definitions */
#define	PIMAXM		65536			/* maxmem value for procinfo */

#define	FNUMINIT	(LONG)0		      /* init value for file numbers */

#define	ALLUSED		-1			       /* all windows in use */

#define	LTOPMAX		4		       /* max depth level for ltop[] */

#define	NXTWNDW		1		 /* value used to select next window */
#define	PRVWNDW		-1	     /* value used to select previous window */

#define	CMAXW		80		/* max. number of columns for a pcon */
#define	CMAXL		(LONG)80	/* max. number of columns for a pcon */

#define	MAXMLN		10			  /* max number of msg lines */
#define	MAXRLN		1			  /* max number of rsp lines */

#define	SAPOK		2			     /* index of attrib poke */
#define	SALEN		3			 /* length of attrib strings */

#define	NAMELEN		32			     /* max len of pcon name */

#define	VCPIPSIZ	2		    /* vcon # length for mpipe specs */
#define	PCPIPSIZ	4	       /* process pid length for mpipe specs */

#define	HLPSIZ		256		   /* max length of <HELP> info text */

#define	BYTESIZ		3		      /* size of BYTE decimal string */
#define	WORDSIZ		5		      /* size of WORD decimal string */
#define	LONGSIZ		10		      /* size of LONG decimal string */
						   /* define some structures */
					 /* structure of message pipe header */
#define	MPHDR	struct	_mphdr
MPHDR
{
    UWORD	mp_msgsiz;				  /* size of message */
    LONG	mp_pid;					    /* pid of sender */
    BOOLEAN	mp_rspflg;	       /* response flag (TRUE = rsp wanted ) */
    UBYTE	mp_rspname[NAME1];		    /* name of response pipe */
};

#define	MPIPE	struct	_mpipe
MPIPE
{
    UWORD	mpi_msgsiz;				  /* size of message */
    LONG	mpi_pid;				    /* pid of sender */
    BOOLEAN	mpi_rspflg;	       /* response flag (TRUE = rsp wanted ) */
    UBYTE	mpi_rspname[NAME1];		    /* name of response pipe */
    UBYTE	mpi_msgbuf[HLPSIZ];		      /* buffer for messages */
};

#define WNDWDESC	struct _wndwdesc
WNDWDESC
{
    UWORD	wd_rmin;		    /* minimum number of rows needed */
    UWORD	wd_cmin;		 /* minimum number of columns needed */
    UWORD	wd_rmax;	 /* maximum number of rows (0 = screen size) */
    UWORD	wd_cmax;      /* maximum number of columns (0 = screen size) */
    UWORD	wd_rorg;				       /* row origin */
    UWORD	wd_corg;				    /* column origin */
    UBYTE	wd_flags;			/* see flag bit values above */
    UBYTE	wd_wfill;		/* character filler value for window */
    UBYTE	wd_tfill;		  /* character filler for top border */
    UBYTE	wd_bfill;	       /* character filler for bottom border */
    UBYTE	wd_lfill;		 /* character filler for left border */
    UBYTE	wd_rfill;		/* character filler for right border */
    UBYTE	wd_wattr;		      /* attribute filler for window */
    UBYTE	wd_battr;		     /* attribute filler for borders */
    UBYTE	*wd_tbhdr;			 /* ptr to top border header */
    UBYTE	*wd_cmdline;		      /* ptr to shell's command line */
};

				  /* special items for mssg and stat windows */
#define	WNDWSPEC	struct _wndwspec
WNDWSPEC
{
    UWORD	ws_numhdr;	    /* count of window header array elements */
    UBYTE	**ws_whdr;	/* ptr to array of ubyte ptrs to header text */
    UWORD	ws_mininfo;		     /* minimum number of info lines */
    UWORD	ws_actinfo;		     /* actual info lines being used */
    UBYTE	*ws_inftxt;	      /* pointer to info text formatter line */
    UWORD	ws_nummore;			     /* number of more lines */
    UBYTE	**ws_mrtxt;	  /* ptr to array of ubyte ptrs to more text */
    UWORD	ws_numpmpt;			   /* number of prompt lines */
    UBYTE	**ws_pmtxt;    /* ptr to array of ubyte ptrs to prompt lines */
    UWORD	ws_numhelp;			     /* number of help lines */
    UBYTE	**ws_hptxt;	 /* ptr to array of ubyte ptrs to help lines */
    UBYTE	**ws_cptxt;	/* ptr to array of ubyte ptrs to CMD prompts */
    UBYTE	**ws_chtxt;   /* ptr to array of ubyte ptrs to CMD help info */
};

#define	WVARS	struct	_wvars
WVARS
{
    UWORD	wv_flags;					    /* flags */
    LONG	wv_pid;				    /* process id of "shell" */
    UWORD	wv_fid;			     /* process family id of "shell" */
    LONG	wv_vnum;			 /* file number for vconsole */
    LONG	wv_cnum;			  /* file number for console */
    LONG	wv_tnum;		 /* file number for top border FRAME */
    LONG	wv_bnum;		 /* file number for btm border FRAME */
    LONG	wv_lnum;		 /* file number for lft border FRAME */
    LONG	wv_rnum;		 /* file number for rgt border FRAME */
    LONG	wv_pnum;			/* file number for MSSG pipe */
    RECT	wv_rwndw;				      /* window RECT */
    RECT	wv_rtbb;			      /* top/btm border RECT */
    RECT	wv_rlrb;			      /* lft/rgt border RECT */
    FRAME	wv_ftop;				 /* top border FRAME */
    FRAME	wv_fbtm;				 /* btm border FRAME */
    FRAME	wv_flft;				 /* lft border FRAME */
    FRAME	wv_frgt;				 /* rgt border FRAME */
    MPHDR	wv_mph;				 /* MSSG pipe message header */
    VCONSOLE	wv_vct;				       /* VCONSOLE structure */
    WNDWDESC	*wv_wdp;		    /* pointer to WNDWDESC structure */
};


/* */

