/*======================================================================*
 *   Version 1.00       SERIAL Driver					*
 *			Header defines					*
 *----------------------------------------------------------------------*
 * VERSION   DATE    TIME  BY   CHANGE/COMMENTS				*
 *----------------------------------------------------------------------*
 *   1.00   3/19/85  8:12 DR-K  Added this intro to files		*
 *   1.01   4/24/85 15:30 DR-K	MAXOUTB to 256, add HIWATER, add LOWATER*
 *   1.02   6/27/85 10:47 DR-K	MAXOUTB to 512				*
 *======================================================================*/


/****************************************************************/
/*  first some data structures used by any driver		*/
/****************************************************************/

#define	ERROR		LONG	/* type of error code returns	*/
#define EMASK		LONG

#define E_IllUnitno	-1020

#define	DPF_UADDR	0x8000	/* addresses are from user space*/

#define	DH		struct DriverHdr
DH
{	/*  driver header  */

	UWORD	dh_reserve	;/* type of driver			*/
	UBYTE	dh_nbrunits	;/* max number of units supported	*/
	UBYTE	dh_flags	;/* flag word				*/
	ERROR	(*dh_init)()	;/* pointer to init code		*/
	ERROR	(*dh_subdrv)()	;/* pointer to subdrv code		*/
	ERROR	(*dh_uninit)()	;/* pointer to init code		*/
	ERROR	(*dh_select)()	;/* pointer to select code		*/
	ERROR	(*dh_flush)()	;/* pointer to flush code		*/
	EMASK	(*dh_read)()	;/* pointer to read code		*/
	EMASK	(*dh_write)()	;/* pointer to write code		*/
	ERROR	(*dh_get)()	;/* pointer to get code			*/
	ERROR	(*dh_set)()	;/* pointer to set code			*/
	EMASK	(*dh_special)()	;/* pointer to special code		*/

	/*  all items below are initialized by o/s			*/

        LONG	reserve1	;/* root of sibling list		*/
	LONG	reserve2	;/* ptr to code seg of dvr - filled by install*/
	LONG	dh_mxid		;/*  holds driver level sync mxid	*/
	LONG	dh_rlr		;/*  address of Ready List Root		*/
	LONG	dh_functab	;/*  ptr to os function table		*/
} ;


/****************************************************************/
/*  Console Driver  dependent type definitions 			*/
/****************************************************************/

#define  max(a,b)  ((a) > (b) ? (a) : (b))
#define  min(a,b)  ((a) < (b) ? (a) : (b))
#define  abs(w1)  ((w1) < 0 ? (-w1) : (w1))

#define	CNTRLS	0x13
#define	CNTRLQ	0x11

#define ALTBYT		struct  alterbytestruct
#define RECT 		struct 	rectstruct
#define ORECT		struct 	orectstruct
#define FRAME		struct 	framestruct

ALTBYT
{	BYTE	DUMMY1;
};

ORECT
{
	ORECT	*o_link;
	WORD	o_row;
	WORD	o_col;
	WORD	o_nrow;
	WORD	o_ncol;
};

#define topVF		0x0008	/* this is the topmost VirCon	*/
				/* whether to update the cursor */

#define PL_USE0		0x01	/* Plane 0 usage mask		*/
#define PL_USE1		0x02	/* Plane 1 usage mask		*/
#define PL_USE2		0x04	/* Plane 2 usage mask		*/
#define PL_USEALL	0x07	/* All Planes usage mask	*/
#define PL_NPLANE	3	/* Number of Planes		*/

#define PL_CHAR		0	/* Character Plane Index, Shift	*/
#define PL_ATTR		1	/* Attribute Place Index, Shift	*/
#define PL_COLOR	2	/* Color Plane Index, Shift	*/

#define	CW_PWflg	0x4000	/* Phys update also on this VC op */

/**** BLOTZ NOTICE NOTE TEMP WIERD BEWARE CHANGE ASYNC			**/
#define	ALTER		0x10	/* this is an Alter operation	*/
#define	CPPMASK		0x0F	/* mask for ops within copy/Alt	*/
#define	SRCUFRAME	0x08
#define	SRCPFRAME	0x04
#define	DESTUFRAME	0x02
#define	DESTPFRAME	0x01
/*************************************************************************/
/**** BLOTZ NOTICE NOTE TEMP WIERD BEWARE CHANGE ASYNC			**/
/****		when the frank RM is ready to use delete above these	**/
/** #define	ALTER		0x04 */	/* this is an Alter operation	*/
/** #define	CPPMASK		0x07 */	/* mask for ops within copy/Alt	*/
/** #define	SRCUFRAME	0x02 **/
/** #define	DESTUFRAME	0x01 **/
/************************************************************************/
FRAME
{
	BYTE	*fr_pl[PL_NPLANE];	/* addr of plane data */
				/* 0- char 1 - attrib 2 - color */
	UWORD	fr_nrow;	/* Number of columns in frame	*/
	UWORD	fr_ncol;	/* Number of rows in frame	*/
	UWORD	fr_use;		/* bit field of plane usage. If bit = 1,*/
				/* plane pointer indicates an array,	*/
				/* else points to a single byte that 	*/
				/* represents a plane with every byte	*/
				/* the same value.			*/
};

RECT
{
	WORD	r_row;		/* origin column			*/
	WORD	r_col;		/* origin row				*/
	WORD	r_nrow;		/* number of columns			*/
	WORD	r_ncol;		/* number of rows			*/
};

#define CDCOPALT	struct  condrvcopyaltstruct

CDCOPALT
{
	BYTE	cd_unit;	/* unit number			*/
	BYTE	cd_opt;		/* options			*/
	UWORD	cd_flags;	/* you guessed it		*/
	UWORD	cd_crow;	/* current row			*/
	UWORD	cd_ccol;	/* current column		*/
	PD	*cd_pd;		/* process descriptor address of*/
/*****************************************************************************/
/**** BLOTZ NOTICE NOTE TEMP WIERD BEWARE CHANGE ASYNC			**/
/***		NOTICE	BLOTZ	NOTICE			NOTE NOTE NOTE	******/
/***							NOTE NOTE NOTE  ******/
/***	the source and destination frames and rects			******/
/***	need to be put into reverse order when the newest		******/
/***	ASYNC resource manager is in place				******/
/***							NOTE NOTE NOTE	******/
/*****************************************************************************/
				/* user process owning UFRAME	*/
	FRAME	*cd_sfram;	/* address of source frame	*/
	RECT	*cd_srect;	/* address of source rect       */
	FRAME	*cd_dfram;	/* address of destination frame	*/
	RECT	*cd_drect;	/* address of destination rectangle	*/
};

#define	CPBLK	struct	CopyInfoBlkstruct
		/* Implementation specific info for Copy/Alter moves */
CPBLK
{
	VOID	(*cp_rowcopy)();	/* routine to move characters	*/
	WORD	cp_sindex;		/* source index number		*/
	WORD	cp_dindex;		/* destination index		*/
};

#define CTE     struct  conTablestruct
CTE
{
        UWORD   CT_rows         ;/* number of rows                      */
        UWORD   CT_cols         ;/* number of columns                   */
        BYTE    CT_flags        ;/* flags                               */
        BYTE    CT_planes       ;/* planes being supported              */
        BYTE    CT_attrp        ;/* attribute plane bits supported      */
        BYTE    CT_extp         ;/* extension plane bits supported      */
        UWORD   CT_country      ;/* country code number                 */
        BYTE    CT_nfkys        ;/* number of function keys             */
        BYTE    CT_buttons      ;/* number of mouse buttons             */
        LONG    CT_serial       ;/* serial number of the mouse          */
        UWORD   CT_murow        ;/* mouse row units per inch            */
        UWORD   CT_mucol        ;/* mouse column units per inch         */
        LONG    CT_pcframe      ;/* ptr to PC char map if available     */
        UWORD   (*CT_conv8)()	;/* ptr to 8 to 16 char convert routine */
        UWORD   (*CT_conv16)()	;/* ptr to 16 to 8 char convert routine */
} ;


/************************************************************************/
/*  Driver Parameter Block  definition                                  */
/************************************************************************/
#define DPB     struct DrvrParmBlock
DPB
{
        BYTE    dp_unitno;      /* unit number for this request */
        BYTE    dp_option;      /* some options */
        UWORD   dp_flags;       /* flags */
        LONG    dp_swi;         /* address of software interrupt */
        LONG    dp_pdaddr;      /* address of callers process discriptor */
        BYTE    *dp_buffer;     /* pointer to the data buffer */
        LONG    dp_bufsiz;      /* size of this buffer */
        LONG    dp_offset;      /* offset for I/O */
        LONG    dp_parm7;       /* some parameter */
        LONG    dp_parm8;       /* another parameter */
};

#define	PCFRAME	0x80	/* this frame is a PC look-alike */

/******** a Virtual Console Data Block ***************/
#define	VCBLK	struct	VirConDataBlock
VCBLK
{
	FRAME	v_frame		;/* description of this frame */
	UWORD	v_top		,/* where the topline of screen scrolled to */
		v_currow,
		v_curcol,
		v_cursx,	/* where the cursor should be left */
		v_cursy,
		v_tempx,
		v_tempy ;
	BYTE	v_escflg,	/*how deep into the esc handler */
		v_mode,		/*what mode we are in now*/
				/* bit 0 == 1 insert char mode	*/
				/*     1 == 1 delete this char	*/
				/*     7 == 1 this is a PCframe	*/
		v_att ;		/* the current write attribute	*/
} ;

#define	DRECT	struct	DirtyRectBlock
DRECT
{
	WORD	currow,		/* current cursor */
		curcol ;
	WORD	row;		/* origin column			*/
	WORD	col;		/* origin row				*/
	WORD	nrow;		/* number of columns			*/
	WORD	ncol;		/* number of rows			*/
};

#define	CDWRITE	struct	ConDrvWriteBlock
CDWRITE
{
	BYTE	cdw_unit;	/* unit number			*/
	BYTE	cdw_two;	/* unused			*/
	UWORD	cdw_flags;
	LONG	cdw_vcid;	/* vcon id # - 0 if PFRAME	*/
	PD	*cdw_pd;	/* ptr to PD of process owning BUFFER */
	WORD	*cdw_buffer;	/* ptr to character buffer	*/
	LONG	cdw_bufsiz;	/* buffer size in bytes		*/
	UWORD	cdw_row;	/* row position for start of write */
	UWORD	cdw_col;	/* column position for start of write	*/
	DRECT	*cdw_dirty;	/* ptr to structure containing new cursor
				 * position and info on Dirty region	*/
};

#define	CDSPECIAL   struct	SpclConDrvStruct
CDSPECIAL
{
	BYTE	cds_unit;	/* unit number			*/
	BYTE	cds_option;	/* function number		*/
	UWORD	cds_flags;
	UWORD	cds_rows;	/* number of rows desired	*/
	UWORD	cds_cols;	/* number of columns desired	*/
	BYTE	*cds_pl0;	/* pre-existing character plane ptr */
};
#define	PL0GIVEN	0x0040	/* on == cds_pl0 is provided	*/

#define	CDSELECT   struct	SelectConDrvStruct
CDSELECT
{
	BYTE	unitno;		/* unit number 			*/
	BYTE	cdse_two;	/* unused			*/
	WORD	cdse_three;	/* unused			*/
	LONG	kbd_pin;	/* address of KEYBOARD ASR	*/
	LONG	m_pin;		/* address of MOUSE ASR		*/
	LONG	mbut_pin;	/* address of BUTTON ASR	*/
	LONG	PConId;		/* Physical Console Id		*/
};

#define	CDMAPPB	struct	MapPhysAddrStruct
CDMAPPB
{
	LONG	zero;		/* not used */
	LONG	physaddr;	/* physical address to translate */
	LONG	length;
};

#define MAXOUTB	512	/* input Queue length */
#define	HIWATER	MAXOUTB-20
#define	LOWATER	20

#define	PHYSBLK	struct	PhysDevQBlock
PHYSBLK
{
	int	Qrear;
	int	Qfront;
	int	Qlen;
	WORD	xoffed;
	WORD	evpend;		/* data relevent to pending event */
	LONG	bpdaddr;
	LONG	fpdaddr;
	DPB	pb;		/* current read parm block */
	LONG	flagno;
	char	Q[MAXOUTB];
};
                                                
B	pb;		/* current read parm block */
	LONG	flagno;
	char	Q[MAXOUTB];
};
                                                