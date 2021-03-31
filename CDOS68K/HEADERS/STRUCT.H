/******************************************************************************
* STRUCT.H - Kernal Data Structures				Version 1.2.01
*******************************************************************************
* Copyright (c) 1985, Digital Research, Inc. All Rights Reserved. The Software
* Code contained in this listing is proprietary to Digital Research, Inc.,
* Monterey, California and is covered by U.S. and other copyright protection.
* Unauthorized copying, adaptation, distribution, use or display is prohibited
* and may be subject to civil and criminal penalties.  Disclosure to others is
* prohibited.  For the terms and conditions of software code use refer to the
* appropriate Digital Research License Agreement.
*******************************************************************************
* Version   Date   Who  Description
* ======= ======== ===  =======================================================
* v1.2.01 04/16/86 FRH	If New Timer System, remove DATE structure (NEWTICK)
* v1.2.00 04/10/86 FRH	Assume SYSBUILD.H included before this file
* v1.0.06 12/10/85 MA	Changed UDA for 68K with new swi system.
* v1.0.05 11/01/85 FRH	New Swi System Conditionals
* v1.0.04 09/17/85 FRH	Remove p_nrpl and p_jba
*			Add PF1_WINDX and p_flag1 in PD
* v1.0.03 09/10/85 FRH	New FLAG structure
* v1.0.02 08/22/85 FRH	Change Event Flag definitions/names
* v1.0.01 08/18/85 FRH	Add IAPX286 define.
*			Add EV_NGET flag (EVB came from OS Pool)
*			Remove EVDELAY,EVFLAG flag
* v1.0.00 08/06/85	Concurrent DOS 286 1.0 Release Version
******************************************************************************/

#define PD	struct _pd
#define UDA	struct uda
#define MD   	struct md	/* memory descriptor */
#define BDA	struct bda	/* bdos data area */
#define MPB	struct mpb	/* memory parameter block */
#define MCB	struct mcb
#define CPB	struct cpb	/* command parameter block (for cli) */
#define JBA	struct jba	/* jump buffer area (the hutt) */
#define MLD	struct _mld
#define UDC	struct udc	/* user descriptor cache */
#define LDT	struct ldt	/* ldt entry */
#define MTBL	struct _mtbl	/* User Process Memory Table (LDT on 286) */
#define REGSAV	struct regsav	/* Register Save Area */
#define SRTLID	struct srtlid
#define OSSRTL	struct ossrtl
#define LSRTL	struct lsrtl
#define SPB	struct spb	/* Sync Parameter Block	(MXID)	*/
#define FLAG	struct _flag	/* System Flag Descriptor	*/
#define ASR	struct _asr	/* ASR scheduling		*/
#define DATE	struct _date	/* Current Date Structure	*/


#define NODISP indisp++;	/* indisp = 1 */
#define DISPON okdisp();
#define MGET(x) ((x *) mgetblk(sizeof(x)))

typedef ULONG   EVSPEC;

#if	MC68K
typedef LONG	jmp_buf[12];	/* space for D3-D7,A2-A7,PC */

#define	INTOFFS	224		/* n-INTOFFS = actual trap number */
#define	PT_68K	0		/* Index for .68K files. */
#define	PT_REL	1		/* Index for .REL files. */

#endif

#if	IAPX286
typedef LONG	jmp_buf[2];

#define	INTOFFS	256		/* n-INTOFFS = actual trap number */
#define	PT_286	0		/* Index for .286 files. */
#define	PT_CMD	1		/* Index for .CMD files. */
#define	PT_EXE	2		/* Index for .EXE files. */
#define	PT_COM	3		/* Index for .COM files. */

#endif

#ifndef VCB
#define	VCB	struct _vcb	/* virtual console block. */
				/* Dummy VCB definition for PD use. */
VCB
{
    BYTE *v_tnp[4];
    BYTE *v_pcframe;
};
#endif

	/* Size of BDA for allocations */
BDA
{
#if	MC68K
	BYTE bda_fill[136];
#endif

#if	IAPX286
	BYTE bda_fill[100];
#endif
};

#if	MC68K
#define ZPAGESIZE	0x400	/* int vectors */
#endif

#if	IAPX286
#define ZPAGESIZE	0x600	/* int vectors, bios, dos areas */


UDC			/* user descriptor cache */
{
	UWORD	limit;		/* segment limit (# bytes) */
	UWORD	baselow;	/* low order segment base (0-15) */
	UBYTE	basehigh;	/* high order base (16-23) */
	UBYTE	access;		/* access byte */
};

LDT			/* LDT entry */
{
	UWORD	limit;
	UWORD	baselow;
	UBYTE	basehigh;
	UBYTE	access;
	UWORD	i386;
};
#endif


UDA			/* User Data Area */
{
#if	MC68K
	ULONG	u_dreg[5];	/* D3-D7 */
	ULONG	u_areg[5];	/* A3-A7 */
	ULONG	u_stk0;		/* Supervisor stack pointer */

		/* FORCERUN() SAVE AREA */

	ULONG	u_dsav[5];	/* D3-D7 save */
	ULONG	u_asav[5];	/* A3-A7 save */
	LONG	u_stk0sav;	/* Supervisor Stack save */
	UWORD	u_pflags;	/* PF_INSYS save */
	UWORD	u_filler;	/* makes everything LONG-aligned */

		/* SWI SAVE AREA */
	LONG	u_ustksav;
	LONG	u_switstk;
#endif

#if	IAPX286
	UWORD	u_insuper;	/* iAPX 286 tss layout	*/
	ULONG	u_stk0;		/* System Space Stack	*/
	ULONG	u_stk1;		/* User Space Stack	*/
	UWORD	u_flags;	/* dsptch flags save	*/
	UWORD	u_bp;		/* dsptch bp save	*/
	UWORD	u_ip;
	UDC	u_es_cache;
	UDC	u_cs_cache;
	UDC	u_ss_cache;
	UDC	u_ds_cache;

	UWORD	u_loadall;	/* flag for dispatcher to perform loadall */
				/* values for bits described in gp.h */
	UWORD	u_ldt_sel;
	BYTE	*u_sbase;	/* sys addr of base of loaded program */
	LONG	u_pbase;	/* phys addr of same, N.B.: NOT a pointer */

		/* FORCERUN() SAVE AREA */

	LONG	u_stk0sav;		/* System Stack save */
	UWORD	u_bpsav;		/* BP save */
	UWORD	u_flagsav;		/* CPU Flags save */
	UWORD	u_pflags;		/* PF_INSYS save */

		/* SWI SAVE AREA */
	LONG	u_ustksav;
	UWORD	u_switstk;
#endif

	LONG	u_udasalloc;
	REGSAV	*u_swiregsav;

#if	IAPX286
	/* following set by the program loader.  8086
	 * emulation requires contiguous allocation
	 * They MUST be declared as LONGS because we want to do comparisons
	 * with them.  If declared as BYTE* the Lattice -s switch will
	 * mess us up.
	 */
	LONG	u_base;	/* physical address of bottom of user memory */
	LONG	u_size;		/* physical address of top of user memory */

	WORD	u_insys;	/* process executing in super state */
#endif
	UWORD	u_usysmap[16];	/* user or system exception map:
				 * one bit for each of 256 vectors.
				 *	0 ==> user defined exception
				 *	1 ==> system exception
				 */

			/* The zero page contains the
			 * interrupt page (0-3ff),
			 * the BIOS data area (400-4ff)
			 * and the DOS data area (500-5ff)
			 */
	union {
	ULONG	l[1];
	UWORD	w[1];
	UBYTE	b[ZPAGESIZE];	/* adjust this one */
	WORD	(*proc[1])();	/* as a pointer to a procedure */
	}
		u_zeropage;
};

MTBL
{
#if	MC68K
	WORD	u_dummy;	/* not applicable to 68000 */
#endif

#if	IAPX286
	UWORD	u_nldt;		/* number of ldt slots available
				 * -initialize to MAXLDT
				 */
	UWORD	u_ldttop;	/* indexes next ldt entry to use.
				 */
	UWORD	u_ldtbase;	/* where to begin allocating ldts (after
				 * entries for zeropage, etc. (indexes u_ldt)
				 */
	UWORD	u_lru;		/* where to begin allocting ldts for saddr
				 * in loadall cases (always has value 4 to 7)
				 */
	LDT	u_ldt[1];	/* ldt's used by this task */ /* 6/18/85 gat */
				/* allocated dynamically by getpd() */
#endif
};

	/* REGSAV.H - layout of a process's Register Save Area
	*	These are the user-visible registers
	*		Also, the process SVC Parameter Block
	*/
REGSAV
{
#if	MC68K
	ULONG	r_dreg[8];	/* D0-D7 */
	ULONG	r_areg[8];	/* A0-A7 */
	ULONG	r_pc;		/* PC */
	UWORD	r_sr;		/* status reg */
#endif

#if	IAPX286
	UWORD	r_ip;
	UWORD	r_flags;
	UWORD	r_ax;
	UWORD	r_cx;
	UWORD	r_dx;
	UWORD	r_bx;
	UWORD	r_sp;
	UWORD	r_bp;
	UWORD	r_si;
	UWORD	r_di;
	UWORD	r_es;
	UWORD	r_cs;
	UWORD	r_ss;
	UWORD	r_ds;
	UWORD	r_errcode;	/* exception error code */
#endif

	BYTE	r_pblk[32];	/* SVC parm block */
};

/* indexes into p_mem array of PD */

#define M_CODE	0x0000
#define M_DATA  0x0001
#define M_HEAP	0x0002
#define M_STACK 0x0003

/* MD flags */

#define M_MEDCHG	0x0010	/* media change on shared code */
#define M_SRTL		0x0020	/* system memory with user access */
#define M_DUP		0x0040	/* duplicate of shared memory descriptor */
#define M_SHARE		0x0080	/* this memory may be shared by other procs */
#define M_SCL		0x0100	/* memory on shared code list */
#define M_FREE		0x0200	/* memory on free list */
#define M_SYSMEM	0x0400	/* memory allocated by system */
#define M_USERMEM	0x0800	/* memory allocate by a user task */
#define LM_NRPL		0x1000	/* what is this used for? */

/*  memory descriptor */

MD
{
    MD	    *m_link;	/* general purpose link */
    LONG    m_start;	/* physical starting address */
    LONG    m_length;	/* physical length */
    LONG    m_lbase;	/* logical starting address */
    MD	    *m_alink;	/* allocated list link */
    MD	    *m_orig;	/* ptr to original MD if sharing */
    WORD    m_flags;	/* M_SHARE, M_DUP, M_SRTL, etc. */
    UWORD   m_waste;	/* unused portion of heap */
    BYTE    m_use;	/* number of users */
    BYTE    m_lock;	/* number of locks in effect */
};

SRTLID
{
	UBYTE	name[8];	/* SRTL file name */
	UWORD	v_major;	/* major version number of SRTL to use */
	UWORD	v_minor;	/* minor version number of SRTL to use */
	ULONG	flags;		/* SRTL flags */
};

OSSRTL
{
	OSSRTL	*s_link;	/* link to next OSSRTL */
	UWORD	ucount;		/* number of process's using this SRTL */
	MD	*srtl_md;	/* SRTL code MD */
	SRTLID	sid;		/* SRTL ID structure */
};

LSRTL
{
	LSRTL	*ls_link;	/* link to next process LSRTL */
	OSSRTL	*s_link;	/* pointer to global SRTL descriptor */
};

#define EV_NOCANCEL	0x0001	/* event will complete */
#define EV_COMPLETE	0x0002	/* event completed */
#define EV_ASR		0x0004	/* event is ASR-originated */
#define EV_CANCEL	0x0008	/* event has been cancelled */
#define EV_MGET		0x0010	/* EVB came from os pool */

#ifndef EVB
#define EVB 	struct evb	/* event block */

EVB		/* event block structure */
{
	EVB	*e_nextp;	/* link to next event on PD event list */
	EVB	*e_link;	/* link to next block on event chain */
	EVB	*e_pred;	/* link to prev block on event chain */
	PD	*e_pd;		/* owner PD */

	LONG	e_swi;		/* swi address */
	LONG	e_parm;		/* parm for request -> event comm */
	WORD	e_flag;
	EVSPEC	e_mask;		/* mask for event notification */
	WORD	e_syserr;	/* word parm	*/

	LONG	e_return;	/* completion code, parm until zombie */
	BYTE 	e_type;
	BYTE	e_prior;
	LONG	e_dh;		/* Pointer to driver header... */
	EVB	**e_root;	/* Root of event list. */
};
#endif

/* pd defines */

/* p_stat */
#define		PS_RUN		0
#define		PS_BLOCK	1
#define		PS_RESTORE	2
#define 	PS_TERM		4
#define		PS_MWAIT	6
#define		PS_SHARE	8

/* p_flag */
#define		PF_SYSPROC	0x0001
#define		PF_LOCKED	0x0002
#define		PF_INSWI	0x0004	/* In SWI Context */
#define		PF_ORIGINAL	0x0008
#define		PF_NRPL		0x0010
#define		PF_RESOURCE	0x0020
#define		PF_SLAVE	0x0040	/* Controlled process */
#define 	PF_CTLC		0x0080	/* Abort attempt while in No Abort */
#define		PF_INSYS	0x0100	/* Running SVC or FE Function */
#define		PF_TEMPKEEP	0x0200	/* No Abort */
#define		PF_LOADALL	0x0400	/* 8086 Emulation */
#define		PF_SHOLD	0x0800	/* shared code holder */
#define		PF_SUPER	0x1000	/* Started as Super User */
#define		PF_SAVSTK	0x2000	/* Dos frontend errmode flag. */
#define		PF_ABORT	0x4000	/* Process Irrevocably Aborting. */
#define		PF_TERMSWI	0x8000	/* running term event swi */

/* p_flag1 */

#define		PF1_WINDX	0x0001	/* Window currently switching */

PD 
{
	PD	*p_link;	/* Current State List (PRL) */
	PD	*p_thread;	/* Process Thread Field (thrdrt)*/
	REGSAV	*p_regsav;	/* Addr of Register Save Area */
	UDA	*p_uda;		/* Addr of User Data Area */
/* 0x10 */
	BYTE	p_name[10];	/* name */
	BYTE	p_stat;		/* Current State */
	BYTE	p_prior;	/* Scheduling Priority */
	WORD	p_flag;		/* Process Flags (1st word) */
	WORD	p_type;		/* type of process */
/* 0x20 */
	EVSPEC	p_evbits;	/* event bits in use */
	EVSPEC	p_evwait;	/* event wait mask */
	EVSPEC	p_evflg;	/* completed events */
	EVB	*p_swilist;	/* event list of pending SWI's */
/* 0x30 */
	EVB	*p_evlist;	/* Event Blocks corresponding to evbits */
	EVB	*p_term;	/* wait for this proc to terminate */
	PD	*p_curmem;  	/* current memory context */
	PD	*p_parent;	/* Parent Process */
/* 0x40 */
		/* Environment Table Info */
	LONG	p_stdfil[4];	/* std in,out,err,ovl */
/* 0x50 */
	WORD	p_secur;	/* default security */
	WORD	p_flag1;	/* Process Flags (2nd Word) */
	BYTE	p_user;		/* Current User ID */
	BYTE	p_group;	/* Current Group ID */
	WORD	p_fid;		/* Family ID */
	LONG 	p_pid;		/* Process ID */
	WORD	p_rnid;		/* Remote Node ID */
	WORD	p_rfid;		/* Remote Family ID */
/* 0x60 */
	LONG	p_rpid;		/* Remote Process ID */

		/* End Environment Table Info */

	BYTE	p_cid;		/* Console ID */
	BYTE	p_vcid;		/* Virtual Console ID */
	BYTE	p_tkcnt;	/* tempkeep count, no abort region */
	BYTE	p_abort;
	BDA	*p_bda;		/* bdos data area pointer */
	VCB	*p_cda;		/* cio data area */
/* 0x70 */
	BYTE	*p_feptr;	/* front end data area */
	BYTE	*p_def;		/* DEFINE table */
	BYTE	*p_mget;	/* mgetblk memory owned by this process */
	BYTE	*p_xlat;	/* xlat table */
/* 0x80 */
	EVB	*p_emaster;	/* controlled process master block field */
	EVB	*p_eslave;	/* controlling process slave block field */
	PD	*p_master;	/* controlling process */
	LONG	p_savwait;	/* events to wait for upon SWI completion */
/* 0x90 */
	LONG	p_swistk;	/* SWI system stack */
	WORD	p_swienable;	/* SWI disable count */
	UWORD	p_stksiz;	/* MAIN System stack size */
	BYTE	*p_systak;	/* MAIN System stack */
	BYTE	*p_cmdenv;	/* Command Environment */
/* 0xa0 */
	BYTE	*p_npx;		/* Numeric processor state save */
	LSRTL	*p_srtl;	/* Process Shared Runtime Lib. list */
	MTBL	*p_ldt;		/* User Space MMU information */
	LONG	p_maxmem;	/* Maximum memory allowed for this process */
/* 0xb0 */
	MD	*p_mem[4];	/* code, data, heap, stack */
/* 0xc0 */
	MLD	*p_level;
	MD	*p_stay;	/* term and stay resident memory */
	LONG	p_abortc;	/* abort code save */
/* 0xcc */

};		/* pd is 0xcc => 204 bytes */

MPB
{
	MD	*mp_mfl; /* free list */
	MD	*mp_mal; /* allocation list */
	MD	*mp_rover; /* roving pointer */
	WORD	mp_waste; /* maximum storage waste */
	MD	*mp_thrdrt; /* memory thread list root */
};

MLD	/* memory level descriptor for procedures */
{
	MLD	*ml_link;
	MD	*ml_lmd;
	WORD	ml_level;
	jmp_buf	*ml_jb;
	LONG	ml_base;
	WORD	ml_ldtbase;
	WORD	ml_ldttop;
};

/**************************************************************
*
*	SPB - Sync Parameter Block (MX Region Descriptor)
*
***************************************************************/

SPB
{
	WORD	sy_tas;		/* Test and Set - 0=unused */
	SPB	*sy_link;	/* Next SPB */
	PD	*sy_owner;	/* current owner if set */
	EVB	*sy_wait;	/* Event List (waiting to own) */
};

/**************************************************************
*
*	FLAG - System Flag Descriptor (ala FLAGSET,FLAGEVENT)
*
***************************************************************/

FLAG
{
	PD	*flg_pd;	/* process performing (FLAGSET)	*/
	LONG	flg_ret;	/* completion code (FLAGSET)	*/
	EVB	*flg_evb;	/* Event Block for FLAGEVENT	*/
	UWORD	flg_state;	/* Current Flag State		*/
};

	/* flg_state Definitions */

#define	FLG_CLEAR	0
#define	FLG_PENDING	1
#define FLG_COMPLETE	2
#define	FLG_CANCEL	3

/****************************************************************
*
*	ASR - ASR scheduling Data Definition
*
*****************************************************************/

ASR
{
	ASR	*asr_link;		/* next ASR in list	*/
	VOID	(*asr_code)();		/* Addr of ASR routine	*/
	LONG	asr_1data;		/* 1st Parm		*/
	LONG	asr_2data;		/* 2nd Parm		*/

	EVB	*asr_evb;		/* Event to wait for	*/
	BYTE	*asr_stk;		/* Stack Frame		*/
	WORD	asr_stklen;
	BYTE	asr_flags;		/* Flags		*/
	BYTE	asr_prior;		/* Scheduling Priority	*/
	LONG	asr_dh;			/* Driver Header Addr	*/
					/*  used by 286 to set  */
					/*  DS before calling   */
};

	/* ASR_FLAGS definitions */

#define	ASRF_RUN	0x0	/* DOASR - runnable		*/
#define ASRF_WAIT	0x1	/* waiting for event		*/
#define ASRF_STK	0x2	/* ASRWAIT			*/
#define	ASRF_FAKE	0x4	/* ASRWAIT - event=0		*/

#if NEWTICK == 0
/*****************************************************************
*
*	DATE - Current Date Structure
*		Format of 'curdate'
*
*****************************************************************/

DATE
{
	WORD	da_year;
	BYTE	da_month;
	BYTE	da_day;
};
#endif		/* End of NEWTICK == 0 */
                                                                                      
;
#endif		/* End of NEWTICK == 0 */
                                                                                      