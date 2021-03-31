/*	@(#)vcdrv.h	1.5		*/
/*======================================================================*
 *   Version 1.00	Console Driver					*
 *			Header file					*
 *======================================================================*/

/****************************************************************/
/*  first some data structures used by any driver		*/
/****************************************************************/

#define	DPF_UADDR	0x8000	/* addresses are from user space */

#define	 BACKSPACE	 '\b'
#define	 TAB		 '\t'
#define	 RETURN		 '\r'
#define	 LINEFEED	 '\n'
#define	 ESCAPE		 '\033'
#define	 BLANK		 ' '
#define  NULLC		 '\0'
#define  BELL		 0x07

/****************************************************************/
/*  Console Driver  dependent type definitions 			*/
/****************************************************************/

#define	 MAXCDUNIT	3	/* number of units */
#define	 BITMAP		0	/* unit number of the mem mapped video */

#define  max(a,b)  ((a) > (b) ? (a) : (b))
#define  min(a,b)  ((a) < (b) ? (a) : (b))
#define  abs(w1)  ((w1) < 0 ? (-w1) : (w1))

#define TOPVF		0x0080	/* this is the topmost VirCon	*/
				/* whether to update the cursor */

#define PL_USE0		0x01	/* Plane 0 usage mask		*/
#define PL_UCHAR	0x01	/* Character Plane usage mask	*/
#define PL_USE1		0x02	/* Plane 1 usage mask		*/
#define PL_UATTR	0x02	/* Attribute Plane usage mask	*/
#define PL_USE2		0x04	/* Plane 2 usage mask		*/
#define PL_UEXT		0x04	/* Attribute Plane usage mask	*/

#define PL_USEALL	0x07	/* All Planes usage mask	*/
#define PL_NPLANE	3	/* Number of Planes		*/

#define PL_CHAR		0	/* Character Plane Index, Shift	*/
#define PL_ATTR		1	/* Attribute Place Index, Shift	*/
#define PL_EXT		2	/* Extension Plane Index, Shift	*/

#define	CW_PWFLG	0x4000	/* Phys update also on this VC op */

	/* Bit meanings within the Copy/Alter parmblk option field */
#define	DESTUFRAME	0x01
#define	SRCUFRAME	0x02
#define	ALTER		0x04	/* this is an Alter operation	*/

/************************************************************************/

/*
 *	The FRAME structure defines the static physical characteristics
 *	of a physical or virtual console.  It contains pointers to
 *	the actual character planes containing the physical image of 
 *	the console (char, attribute and extension).
 *
 *	Concerning the bit field of plane usage.  If bit is set ( = 1),
 *	plane pointer indicates an array else points to a single byte that
 *	represents a plane with every byte the same value.
 */

#define FRAME		struct 	framestruct

FRAME
{
	BYTE	*fr_pl[PL_NPLANE];	/* addr of plane data */
	UWORD	fr_nrow;		/* Number of rows in frame	*/
	UWORD	fr_ncol;		/* Number of columns in frame	*/
	UWORD	fr_use;			/* bit field of plane usage. */
};

#define RECT 		struct 	rectstruct

RECT
{
	WORD	r_row;		/* origin row				*/
	WORD	r_col;		/* origin column			*/
	WORD	r_nrow;		/* number of rows			*/
	WORD	r_ncol;		/* number of columns			*/
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
				/* user process owning UFRAME	*/
	FRAME	*cd_dfram;	/* address of destination frame	*/
	RECT	*cd_drect;	/* address of destination rectangle	*/
	FRAME	*cd_sfram;	/* address of source frame	*/
	RECT	*cd_srect;	/* address of source rect	*/
};

#define	CP_CURMOVE	0x1000	/* This copy/alter call will be just  */
				/* a cursor move call		      */
#define	CP_REORDER	0x0800	/* This copy/alter call will have to  */
				/* store the sources v_att,v_mode and */
				/* v_top to destinations	      */
#define CTE	struct  conTablestruct
CTE
{
	UWORD	ct_rows;	/* number of rows			*/
	UWORD	ct_cols;	/* number of columnss			*/
	BYTE	ct_flags;	/* flags				*/
	BYTE	ct_planes;	/* planes being supported		*/
	BYTE	ct_attrp;	/* attribute plane bits supported	*/
	BYTE	ct_extp;	/* extension plane bits supported	*/
	UWORD	ct_country;	/* country code number			*/
	BYTE	ct_nfkys;	/* number of function keys		*/
	BYTE	ct_buttons;	/* number of mouse buttons		*/
	LONG	ct_serial;	/* serial number of the mouse		*/
	UWORD	ct_murow;	/* mouse row units per inch		*/
	UWORD	ct_mucol;	/* mouse column units per inch		*/
	LONG	ct_pcframe;	/* ptr to PC char map if available	*/
	UWORD	(*ct_conv8)();	/* ptr to 8 to 16 char convert routine */
	UWORD	(*ct_conv16)();	/* ptr to 16 to 8 char convert routine */
};

/************* values for flag field above ******************************/

#define	GRAPHIC	0x0001		/* 0==char mapped, 1==bit mapped	*/
#define	NUMPAD	0x0002		/* 0==has a numerical keypad		*/
#define	MOUSE	0x0004		/* 0==supports a mouse			*/
#define	COLOR	0x0008		/* 0==monochrome, 1==color screen	*/

#define MFNKEYS		16	/* number function keys - memory mapped */
#define SFNKEYS		10	/* number function keys - serial mapped */

#define ATTRBITS	0xFF	/* mask of attribute bits supported	*/

#define USA		1670	/* country code */

#define	PCFRAME		0x80	/* this frame is a PC look-alike */

/******** a Virtual Console Data Block ***************/
#define	VCBLK	struct	VirConDataBlock

VCBLK
{
	FRAME	v_frame;	/* description of this frame */
	UWORD	v_top,		/* where the topline of screen scrolled to */
		v_currow,
		v_curcol,
		v_cursx,	/* where the cursor should be left */
		v_cursy,
		v_tempx,	/* used to save cursor position */
		v_tempy;
	BYTE	v_escflg,	/* how deep into the esc handler */
		v_mode,		/* what mode we are in now */
		v_att,		/* the current write attribute	*/
		fill,
		*v_dirty;	/* a plane of boolean dirty markers */
	WORD	v_dbuf;		/* stores ptr into dirty when nextasr */
};

	/* Fields used in v_mode above */
#define M_INSERTC	0x1	/* Insert character mode active */
#define M_REVVIDM	0x4	/* Reverse video mode active */
#define M_WRAP		0x8	/* Wrap around at end of line */
#define M_OFFCURSOR	0x10	/* The cursor is off. */
#define	M_PCURSOFF	0x20	/* Physical Cursor turned off */

/*
 *	frame()
 *
 *	Coerce a VCBLK pointer to be a FRAME pointer.
 *
 *	Rationale: Since the FRAME is the first part of a VCBLK, which
 *	describes a physical or virtual console in the driver,
 *	we can coerce a pointer to a VCBLK (which is this driver's
 *	implementation of the unique vcid described in the specification)
 *	to be a pointer to a frame.  We make use of this feature in
 *	special(), copy() and write() extensively.  It helps us avoid
 *	needless replication of pointers and improve register variable
 *	allocation.
 */

#define frame(v)	((FRAME *)(v))

/*
 *	vcblk()
 *
 *	The inverse macro.
 */

#define vcblk(f)	((VCBLK *)(f))

/*
 *	position()
 *
 *	We essentially linearize the x,y coordinate of the byte
 *	in the frame to yield and index into a one dimensional
 *	BYTE array.
 */

#define position(v,r,c)	(((((v)->v_top + (r)) % frame((v))->fr_nrow) \
				* frame((v))->fr_ncol) + (c))

#define	DRECT	struct	DirtyRectBlock
DRECT
{
	WORD	currow,		/* current cursor */
		curcol;
	WORD	row;		/* origin column			*/
	WORD	col;		/* origin row				*/
	WORD	nrow;		/* number of columns			*/
	WORD	ncol;		/* number of rows			*/
};

/*
 *	Information for write entry point
 */

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

/*
 *	We chain writes through nextasr() when the console is
 *	a serial console.
 */

#define WRCHN	struct CoNWrChain
WRCHN
{
	LONG	ch_flag;
	WORD	ch_state;
	PD	*ch_curpd;
	LONG	ch_chain;
	CDWRITE	ch_pb;
};

#define WRITECHAIN	0x01
#define WRCHNPRI	200

	/* first Special block - used for VC Create */
#define	CDSP0   struct Spcl0ConDrvStruct
CDSP0
{
	BYTE	cds_unit;	/* unit number			*/
	BYTE	cds_option;	/* function number		*/
	UWORD	cds_flags;
	UWORD	cds_rows;	/* number of rows desired	*/
	UWORD	cds_cols;	/* number of columns desired	*/
	BYTE	*cds_pl0;	/* pre-existing character plane ptr */
};

	/* other Special block - used for all other functions */
#define	CDSP   struct SpclConDrvStruct
CDSP
{
	BYTE	cds_unit;	/* unit number			*/
	BYTE	cds_option;	/* function number		*/
	UWORD	cds_flags;	/* what do you think?		*/
	VCBLK	*cds_vcid;	/* ptr to existing vcid		*/
};

/*
 *	Special Functions
 */

#define CREATEVC	0
#define DELETEVC	1
#define VCTOPC		2
#define PCTOVC		3
#define PTGETBL		0x13	/* Why are there two??? */
#define PTSETBL		0x93

/*
 *	These flags are used internally by the driver.
 *	SERPHYSCON indicates that the frame we are allocating
 *	is the IBM physical frame for a serial port mapped console.
 *	It forces allocation of a dirty plane...
 */

#define	SERPHYSCON		0x0080	/* on == creating a physical	*/

#define	CDSELECT   struct	SelectConDrvStruct
CDSELECT
{
	BYTE	unitno;		/* unit number 			*/
	BYTE	cdse_two;	/* unused			*/
	WORD	cdse_three;	/* unused			*/
	LONG	kbd_pin;	/* address of KEYBOARD ASR	*/
	LONG	m_pin;		/* address of MOUSE ASR		*/
	LONG	mbut_pin;	/* address of BUTTON ASR	*/
	LONG	pconid;		/* Physical Console Id		*/
};

#define	CDMAPPB	struct	MapPhysAddrStruct
CDMAPPB
{
	LONG	zero;		/* not used */
	LONG	physaddr;	/* physical address to translate */
	LONG	length;
};

#define MAPDATA		1	/* We want to map data area with map phys */

#define SDPB	struct SubdriveParmBlock
SDPB
{
	BYTE	sd_unit;
	BYTE	sd_sdunit;
	WORD	sd_access;
	DH	*sd_sdheader;
};

/*
 *	Subdriver link ins for console driver to keyboard
 *	and terminal subdrivers.
 */

#define DVR_KEYBD	0x75
#define DVR_TERM	0x76
er link ins for 