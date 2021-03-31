
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
 *   Version 1.4        CCUTLS.H				 *
 *                      Definitions for modules in CCUTLS.L86	 *
 *---------------------------------------------------------------*
 *    VERSION   DATE    BY      CHANGE/COMMENTS                  *
 *---------------------------------------------------------------*
 *                                                               *
 *	1.4   09/26/85	jsr	Added BPBXFER define.		 *
 *	1.3   08/02/85	jsr	Modified for 68K compile,	 *
 *				changed FMAX define.		 *
 *	1.2   05/17/85	jsr	Added UE_RPHYS and UE_WPHYS.	 *
 *	1.1   05/07/85	jsr	Changed for UTXFRSYS changes.	 *
 *	1.0   03/30/85	jsr					 *
 *                                                               *
 *===============================================================*/

			   /* processor select codes (only 1 should be TRUE) */

#define	P286		FALSE
#define	P68K		TRUE

#if P286

typedef	LONG	jumpbuff[2];

#endif

#if P68K

typedef	LONG	jumpbuff[12];

#endif

		 /* Indexes for NUMCONV[] (in the standard messages library) */
#define	NC0	0
#define	NC1	1
#define	NC2	2
#define	NC3	3
#define	NC4	4
#define	NC5	5
#define	NC6	6
#define	NC7	7
#define	NC8	8
#define	NC9	9
#define	NCA	10
#define	NCB	11
#define	NCC	12
#define	NCD	13
#define	NCE	14
#define	NCF	15

/* Message Control Block Structure and definitions */

#define	HLVLMAX	3		    /* Highest HELP level = 3, (0 is lowest) */
#define	PARMAX	10		    /* Number of INSERT parameters supported */

#define	MCB	struct _mcb
MCB
{
    WORD	utilcode;
    WORD	dosfunc;
    LONG	retcode;
    BYTE	*mptr[HLVLMAX];
    BYTE	*pptr[PARMAX];
};
					/* mptr indexes for HELPLEVELs 1 - 4 */
#define	HLVL1	-1		   /* NOTE: there is no index for HELPLEVEL1 */
#define	HLVL2	0
#define	HLVL3	1
#define	HLVL4	2

#define	PARM0	0				 /* pptr index for INSERT %0 */
#define	PARM1	1
#define	PARM2	2
#define	PARM3	3
#define	PARM4	4
#define	PARM5	5
#define	PARM6	6
#define	PARM7	7
#define	PARM8	8
#define	PARM9	9

/* Error source structure for CCMSGS.L86 */

#define	ERRSRC	struct	_errsrc
ERRSRC
{
    BYTE	es_start;
    BYTE	es_end;
    BYTE	*es_text;
};

/* Miscellaneous */

#define	SOURCE	TRUE				/* source & dest flag values */
#define	DEST	!SOURCE

#define	LOGICAL		TRUE		   /* logical & physical flag values */
#define	PHYSICAL	!LOGICAL

#define	LINELEN	256				     /* max text line length */


/* Defines for the UTT1, UTT2 and UTT3 routines */

#define	STDLIM1	5	      /* number of recognized delimiters (less NULL) */


/* Defines for the UTT2 routine */

#define	STDLIM2	14	      /* number of recognized delimiters (less NULL) */


/* Defines for the UTT3 routine */

#define	STDLIM3	14	      /* number of recognized delimiters (less NULL) */


/* Defines for the UTERRMSG routine */

#define	NAMEFLAG	TRUE	     /* CDOS names (TRUE) or numbers (FALSE) */

#define	FATAL		TRUE				      /* flag values */
#define	NONFATAL	!FATAL


/* Defines for the UTPRNMSG routine */

#define	RLVLMAX	30				  /* maximum recursion depth */

#define	PERR_OK		(LONG)0		   /* UTPRNMSG error codes (non-DOS) */
#define	PERR_RC		(LONG)1				  /* recursion error */


/* Defines for UTI2DS, UTL2DS, UTUI2DS, UTUL2DS, UTI2HS and UTL2HS */

#define	HWBUFSIZ	5				   /* 4 chars + NULL */
#define	HLBUFSIZ	9				   /* 8 chars + NULL */
#define LBUFSIZ		12			    /* NEG + 10 chars + NULL */
#define	IBUFMAX		7		 /* used to adjust UTI2DS return ptr */


/* Defines for the UTXFRSYS routine */

#define	FMAX	(LONG)65536

#define	BPBXFER	37			 /* number of bytes to copy from BPB */

#define	MDF	struct	_mdf			      /* mini DISKFILE table */
MDF
{
    ULONG	mdf_lookid;
    UBYTE	mdf_name[18];				       /* NAME3 = 18 */
    UWORD	mdf_attrib;
};

#define	XFINFO	struct	_xfinfo
XFINFO
{
    BYTE	*bufptr;
    LONG	bufsiz;
    WORD	recsiz;
    WORD	protct;
    LONG	sfsize;
};

#define	SFI	struct	_sfi
SFI
{
    BOOLEAN	eoflag;
    LONG	soff;
    LONG	nbytes;
    BYTE	*bufptr;
    LONG	bufsiz;
};

#define	XFERR000	0		/* no error			     */
#define	XFERR001	1		/* S_LOOKUP(DEFINE),S_GET(DISK,FNUM) */
#define	XFERR002	2		/* S_OPEN (sfile)		     */
#define	XFERR003	3		/* sysfile format err 		     */
#define	XFERR004	4		/* S_MALLOC/S_MFREE err		     */
#define	XFERR005	5		/* S_LOOKUP(DF), S_GET(DF) (sfile)   */
#define	XFERR006	6		/* (fixed && (sdev == ddev))	     */
#define	XFERR007	7		/* S_OPEN (sdev or ddev)	     */
#define	XFERR008	8		/* non-disk device (sdev or ddev)    */
#define	XFERR009	9		/* S_OPEN (ddev)		     */
#define	XFERR010	10		/* S_READ (ddev) (FAT)		     */
#define	XFERR011	11		/* not enough room 4 sysfile (ddev)  */
#define	XFERR012	12		/* S_CREATE (dfile), S_OPEN (dfile)  */
#define	XFERR013	13		/* S_READ (sfile)		     */
#define	XFERR014	14		/* S_WRITE (dfile)		     */
#define	XFERR015	15		/* S_READ (ddev) (BOOTREC)	     */
#define	XFERR016	16		/* S_WRITE (ddev) (BOOTREC)	     */
#define	XFERR017	17		/* S_OPEN (sdev)		     */
#define	XFERR018	18		/* S_READ (sdev) (BOOTREC)	     */
#define	XFERR019	19		/* S_GET(DF) dfile, S_SET(DF) dfile  */
#define	XFERR020	20		/* not enuf room 4 nonsys file (ddev)*/
#define	XFERR021	21		/* no BOOTREC or FAT too large (ddev)*/

/* XFERR022 thru XFERR026 are generated by the utp4dsk() procedure */

#define	XPDSKOFF	21			/* offset for utp4dsk() errs */
#define	XFERRMAX	XPDSKOFF+PDERRMAX	/* highest XFERR###	     */


/* Defines for the UTSTDV routine */

#define	MANDATORY 	TRUE
#define	OPTIONAL	!MANDATORY

#define	SDERR000	0		/* successful operation		     */
#define	SDERR001	1		/* device name too long error	     */
#define	SDERR002	2		/* MANDATORY device not given	     */

#define	SDERRMAX	2		/* highest SDERR###		     */


/* defines for the UTGTPDV routine */

#define	GPDEVFND	(LONG)1		/* pdev found			     */


/* Defines for the UTP4KEY and UTP4DSK routine */

#define	CONMODE		struct	_conmode	       /* from CONSOLE table */
CONMODE					 /* also used by PROCESS prcpmtusr() */
{
	UWORD	cn_numkey;
	UWORD	cn_smode;
	UWORD	cn_kmode;
};

/* Defines for the UTP4KEY routine */

#define	PKERR000	0		/* successful prompt		     */
#define	PKERR001	1		/* S_GET(CONSOLE), S_SET(CONSOLE)    */
#define	PKERR002	2		/* error on S_READ of STDIN	     */

#define	PKERRMAX	2		/* highest PKERR###		     */

/* Defines for the UTP4DSK routine */

#define	PDF_STDCON	0x0001		/* STDIN is a CONSOLE device	     */
#define	PDF_FOPEN	0x0002		/* PDEV is OPEN			     */

#define	PDERR000	0		/* successful prompt 		     */
#define	PDERR001	1		/* S_GET(CON)(DISK), S_SET(CON)	     */
#define	PDERR002	2		/* S_READ of STDIN		     */
#define	PDERR003	3		/* S_LOOKUP(PDEF) ldev to pdev	     */
#define PDERR004	4		/* S_OPEN pdev			     */
#define	PDERR005	5		/* S_SPECIAL (MDCHK)		     */

#define	PDERRMAX	5		/* highest PDERR###		     */


/* Defines for the UTGTDF routine */

#define	GDLITR		TRUE		/* Literal lookup		     */
#define	GDXLAT		FALSE		/* Translation lookup		     */

/* Defines for the UTExxxxx routines */

#define	UE_NONE		0		/* ignore code			     */
#define	UE_DEVONLY	1		/* dev: name only		     */
#define	UE_DVANDFL	2		/* dev: and file names		     */
#define	UE_DEVFIL	3		/* dev:file name		     */
#define	UE_DIRNAME	4		/* directory name		     */
#define	UE_INITF	5		/* S_SPECIAL (INITF)		     */
#define	UE_RDSYS	6		/* S_SPECIAL (RDSYS)		     */
#define	UE_WTSYS	7		/* S_SPECIAL (WTSYS)		     */
#define	UE_FMSYS	8		/* S_SPECIAL (FMSYS)		     */
#define	UE_FMTRK	9		/* S_SPECIAL (FMTRK)		     */
#define	UE_MDCHK	10		/* S_SPECIAL (MDCHK)		     */
#define	UE_RPHYS	11		/* S_SPECIAL (RPHYS)		     */
#define	UE_WPHYS	12		/* S_SPECIAL (WPHYS)		     */

/* */

)		     */
#define	UE_RPHYS	11		/* S_SPECIAL (RPHYS)		     *