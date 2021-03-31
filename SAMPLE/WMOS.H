
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
 *   Version 1.1        WMOS.H					 *
 *                      Operating System defines for WMEX.C	 *
 *---------------------------------------------------------------*
 *    VERSION   DATE    BY      CHANGE/COMMENTS                  *
 *---------------------------------------------------------------*
 *                                                               *
 *	1.1   05/10/85	jsr	E_LOGOFF = 0x7fffffff.		 *
 *	1.0   04/18/85	jsr					 *
 *                                                               *
 *===============================================================*/

/*
 *	Operating System Definitions
 */

						/* From SYSTEM.H */

/****************************************************************/
/*								*/	
/*	Driver types						*/
/*								*/
/****************************************************************/

#define	DVR_TIME	0x01	/* Timer Driver			*/
#define	DVR_PIPE	0x11	/* Pipe Driver			*/
#define	DVR_DISK	0x21	/* Disk Driver			*/
#define	DVR_CON		0x31	/* Console Driver		*/
#define	DVR_PRN		0x71	/* Printer Driver		*/
#define DVR_SER		0x72	/* Serial Driver		*/

#define DVR_CLOCK	0x7e	/* DOS Clock Driver Emulator	*/
#define DVR_NULL	0x7f	/* Null Device			*/

#define	DVR_PORT	0x81	/* Port Driver			*/
#define	DVR_OEM		0x82	/* 1st OEM Driver		*/

/****************************************************************/
/*								*/	
/*	System information table types				*/
/*								*/
/****************************************************************/

#define	T_PROC		0x00		/* Process table		*/
#define	T_ENV		0x01		/* Environment table		*/
#define	T_TD		0x02		/* Time and date table		*/
#define	T_MEM		0x03		/* Memory table			*/
#define	T_PIPE		0x10		/* Pipe table			*/
#define	T_FILE		0x20		/* Disk File table		*/
#define	T_DISK		DVR_DISK	/* Disk table			*/
#define	T_CON		0x30		/* Screen table			*/
#define	T_PCON		DVR_CON		/* Physical Console table	*/
#define	T_VCON		0x32		/* Virtual Console table	*/
#define	T_MOUSE		0x33		/* Mouse table			*/
#define T_TOP		0x34		/* Top Border (no table)	*/
#define T_BOTTOM	0x35		/* Bottom Border (no table)	*/
#define T_LEFT		0x36		/* Left Border (no table)	*/
#define T_RIGHT		0x37		/* Right Border (no table)	*/
#define	T_SYS		0x40		/* System table			*/
#define	T_FNUM		0x41		/* File number table		*/
#define	T_SDEF		0x42		/* System Defines table		*/
#define	T_PDEF		0x43		/* Process Defines table	*/
#define	T_CMD		0x44		/* Command Environment table	*/
#define	T_DEV		0x45		/* Device table			*/
#define	T_PNAME		0x46		/* Pathname table		*/
#define T_SER		DVR_SER		/* Serial table			*/
#define	T_PRN		DVR_PRN		/* Printer table		*/
#define T_CLOCK		DVR_CLOCK	/* DOS Clock Driver table	*/
#define T_NULL		DVR_NULL	/* Null Device table		*/
#define	T_PORT		DVR_PORT	/* Port table			*/

	/* Note:  Tables from 0x82 -> 0xff are special tables */

/* NOTE: Error codes are WORD values */

#define	E_MEMORY	0x400D	/* Not enough memory available		*/

#define	E_NOMEM		0x4015	/* Non-existant memory			*/
#define	E_MBOUND	0x4016	/* Memory Bound error			*/
#define E_EBOUNDEX	E_MBOUND
#define	E_ILLINS	0x4017	/* Illegal instruction			*/
#define	E_DIVZERO	0x4018	/* Divide by zero			*/
#define	E_BOUND		0x4019	/* Bound exception			*/
#define	E_OFLOW		0x401A	/* Overflow exception			*/
#define	E_PRIV		0x401B	/* Privilege violation			*/
#define	E_TRACE		0x401C	/* Trace				*/
#define	E_BRKPT		0x401D	/* Breakpoint				*/
#define	E_FLOAT		0x401E	/* Floating point exception		*/
#define	E_STACK		0x401F	/* Stack fault				*/
#define	E_NOTON286	0x4020	/* Exception not caught by 286		*/
#define E_EM1		0x4021	/* emulated instruction group 1		*/

					       /* end of SYSTEM.H stuff */

#define	E_LOGOFF	(LONG)0x7FFFFFFF   /* Logoff requested by Shell */

/* table structures */

#define	SERSIZ		8	/* Size of serialization sequence	    */
#define	NAME1		10	/* Size of a name field			    */
#define	NAME2		14	/* Size of a name field			    */
#define	NAME3		18	/* Size of a name field			    */
#define	MOUSIZ		16	/* Size of mouse data and mask fields	    */
#define	BUFSIZ		128	/* Size of general buffer		    */

#define	DEFINE	struct	_define				/* for PDEF and SDEF */
DEFINE
{
	ULONG	def_lookid;
	UBYTE	def_lname[NAME1];
	UBYTE	def_prefix[BUFSIZ];
};

#define	CMDENV	struct	_cmdenv
CMDENV
{
	UBYTE	cmd_file[BUFSIZ];
	UBYTE	cmd_string[BUFSIZ];
	UBYTE	cmd_info;	/* parent env. strings here, variable length */
};

#define	CONSOLE	struct	_console
CONSOLE
{
	UWORD	cn_numkey;
	UWORD	cn_smode;
	UWORD	cn_kmode;
	UWORD	cn_currow;
	UWORD	cn_curcol;
	UWORD	cn_nrows;
	UWORD	cn_ncols;
	UBYTE	cn_vcnum;
	UBYTE	cn_type;
	UBYTE	cn_name[NAME1];
};

						 /* console smode bit values */
#define	CSM_DUMB	0x0001		       /* disables ANSI/VT52 support */
#define	CSM_SXB		0x0002			   /* sixteen bit characters */
#define	CSM_NOC		0x0004					/* no cursor */
#define	CSM_CRLF	0x0008			    /* convert lf's to cr/lf */
						 /* console kmode bit values */
#define	CKM_NAB		0x0001			  /* disable aborts (CTRL-C) */
#define	CKM_NSC		0x0002		     /* disable scroll (CTRLs S & Q) */
#define	CKM_NXL		0x0004				   /* no translation */
#define	CKM_NV52	0x0008				 /* no VT52 decoding */
#define	CKM_SXB		0x0010			   /* sixteen bit characters */
#define	CKM_NEC		0x0020					  /* no echo */
#define	CKM_NCZ		0x0040				     /* no control z */
#define	CKM_TOG		0x0080				/* toggle characters */
#define	CKM_CRLF	0x0100			 /* convert CR or LF to CRLF */

						  /* subset of console table */
#define	MINICON    struct _minicon
MINICON
{
    UWORD	mc_tahead;		   /* # of keys in type-ahead buffer */
    UWORD	mc_sc;					      /* Screen Mode */
    UWORD	mc_kb;					    /* Keyboard Mode */
    UWORD	mc_currow;				      /* current row */
    UWORD	mc_curcol;
};


#define	ENVIRON	struct	_environ
ENVIRON
{
	ULONG	en_stdin;
	ULONG	en_stdout;
	ULONG	en_stderr;
	ULONG	en_overlay;
	UWORD	en_security;
	UWORD	en_rsvd;
	UBYTE	en_user;
	UBYTE	en_group;
	UWORD	en_fid;
	ULONG	en_pid;
	UWORD	en_rnid;
	UWORD	en_rfid;
	ULONG	en_rpid;
};

#define	FILENUM	struct	_filenum
FILENUM
{
	ULONG	fn_filenum;
	UBYTE	fn_access;
	UBYTE	fn_table;
	UWORD	fn_rsvd;
	ULONG	fn_infosize;
	UBYTE	fn_infodata;		/* infodata here, variable length    */
};

					  /* filenum table access bit values */
#define	FNA_SET		0x0001			       /* set access allowed */
#define	FNA_EXC		0x0002			   /* execute access allowed */
#define	FNA_WRT		0x0004			     /* write access allowed */
#define	FNA_RD		0x0008			      /* read access allowed */

#define	MOUSE	struct	_mouse
MOUSE
{
	WORD	mou_row;
	WORD	mou_col;
	WORD	mou_pixrow;
	WORD	mou_pixcol;
	WORD	mou_click;
	BYTE	mou_height;
	BYTE	mou_width;
	WORD	mou_hotrow;
	WORD	mou_hotcol;
	UWORD	mou_mask[MOUSIZ];
	UWORD	mou_data[MOUSIZ];
};

#define	PCONSOLE	struct	_pconsole
PCONSOLE
{
	UBYTE	pc_name[NAME1];
	UBYTE	pc_nvc;
	UBYTE	pc_cid;
	UWORD	pc_rows;
	UWORD	pc_cols;
	UWORD	pc_crows;
	UWORD	pc_ccols;
	UBYTE	pc_flags;
	UBYTE	pc_planes;
	UBYTE	pc_attrp;
	UBYTE	pc_extp;
	UWORD	pc_country;
	UBYTE	pc_nfkeys;
	UBYTE	pc_buttons;
	ULONG	pc_serial;
};

					   /* pconsole table flag bit values */
#define	PCF_GRP		0x0001				 /* graphics console */
#define	PCF_NNK		0x0002			      /* no numerical keypad */
#define	PCF_MSS		0x0004			   /* mouse support provided */
#define	PCF_CLR		0x0008				    /* color console */
					  /* pconsole table plane bit values */
#define	PCP_CHAR	0x0001			/* character plane supported */
#define	PCP_ATTR	0x0002			/* attribute plane supported */
#define	PCP_EXTN	0x0004			/* extension plane supported */

#define	PROCESS	struct	_process
PROCESS
{
	ULONG	p_pid;
	UWORD	p_fid;
	UBYTE	p_cid;
	UBYTE	p_vcid;
	UBYTE	p_name[NAME1];
	UBYTE	p_state;
	UBYTE	p_prior;
	ULONG	p_maxmem;
	UWORD	p_flags;
	UBYTE	p_cuser;
	UBYTE	p_cgroup;
	ULONG	p_parent;
	ULONG	p_events;
	ULONG	p_code;
	ULONG	p_csize;
	ULONG	p_data;
	ULONG	p_dsize;
	ULONG	p_heap;
	ULONG	p_hsize;
};

					    /* process table flag bit values */
#define	PCF_SYS		0x0001				   /* system process */
#define	PCF_LKM		0x0002				 /* locked in memory */
#define	PCF_SWI		0x0004			   /* running in SWI context */
#define	PCF_SUP		0x0008			     /* originally superuser */


#define	VCONSOLE	struct	_vconsole
VCONSOLE
{
	ULONG	vc_lookid;
	UWORD	vc_mode;
	UBYTE	vc_vcnum;
	UBYTE	vc_type;
	WORD	vc_viewrow;
	WORD	vc_viewcol;
	WORD	vc_nrow;
	WORD	vc_ncol;
	WORD	vc_posrow;
	WORD	vc_poscol;
	WORD	vc_rows;
	WORD	vc_cols;
	BYTE	vc_top;
	BYTE	vc_bottom;
	BYTE	vc_left;
	BYTE	vc_right;
};

				    /* virtual console table mode bit values */
#define	VCM_FZB		0x0001				    /* freeze border */
#define	VCM_AVC		0x0002			   /* allow auto view change */
#define	VCM_KCE		0x0004		     /* keep cursor on edge (on AVC) */
#define	VCM_AVO		0x0008		       /* auto view change on output */
					  /* virtual console type bit values */
#define	VCT_GRP		(BYTE)0x01			    /* graphics vcon */


#define PL_NPLANE	3
#define FRAME	struct	_frame
FRAME
{
    BYTE	*fr_pl[PL_NPLANE];	/* char, attrib and extension planes */
    UWORD	fr_nrow;
    UWORD	fr_ncol;
    UWORD	fr_use;
};

#define PL_USE0		0x01			       /* Plane 0 usage mask */
#define PL_USE1		0x02			       /* Plane 1 usage mask */
#define PL_USE2		0x04			       /* Plane 2 usage mask */
#define PL_USEALL	0x07			    /* All Planes usage mask */

#define PL_CHAR		0		     /* Character Plane Index, Shift */
#define PL_ATTR		1		     /* Attribute Place Index, Shift */
#define PL_EXTN		2		     /* Extension Plane Index, Shift */


#define RECT	struct	_rect
RECT
{
    WORD	r_row;
    WORD	r_col;
    WORD	r_nrow;
    WORD	r_ncol;
};
						/* command process info	     */
#define	PINFO		struct	_pinfo
PINFO
{
	BYTE	pi_pname[NAME1];
	BYTE	pi_prior;
	BYTE	pi_rsvd1;
	LONG	pi_maxm;
	LONG	pi_addm;
};


#define	SMO_XPND	0x0000			     /* options for S_MALLOC */
#define	SMO_NEW		0x0001
				      /* memory parameter block for S_MALLOC */
#define	MPB	struct	_mpb
MPB
{
	ULONG	mpb_start;
	ULONG	mpb_minact;
	ULONG	mpb_max;
};

						  /* alterbyte array indexes */
#define	ALB_CAND	0
#define	ALB_CXOR	1
#define	ALB_AAND	2
#define	ALB_AXOR	3
#define	ALB_EAND	4
#define	ALB_EXOR	5

					     /* standard device file numbers */
#define	STDIN		(LONG)0
#define	STDOUT		(LONG)1
#define	STDERR		(LONG)2

							    /* file security */

#define	FS_OD	0x0001		/* owner delete/set			     */
#define	FS_OE	0x0002		/* owner execute			     */
#define	FS_OW	0x0004		/* owner write				     */
#define	FS_OR	0x0008		/* owner read				     */
#define	FS_GD	0x0010		/* group delete/set			     */
#define	FS_GE	0x0020		/* group execute			     */
#define	FS_GW	0x0040		/* group write				     */
#define	FS_GR	0x0080		/* group read				     */
#define	FS_WD	0x0100		/* world delete/set			     */
#define	FS_WE	0x0200		/* world execute			     */
#define	FS_WW	0x0400		/* world write				     */
#define	FS_WR	0x0800		/* world read				     */

#define	ORD_BTM		0		   /* value used to ORDER btm window */
#define	ORD_TOP		-1		   /* value used to ORDER top window */

#define	SHLPRI		200		       /* process priority for SHELL */

/* */

