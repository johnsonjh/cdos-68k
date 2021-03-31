/***********************************************************************
* ommu.c - OEM-defined MMU functions for CDOS 4.0.
*
* This is the MC68451 version using old algorithms where the ASN for
* a process is just its process ID.  This will not work when more than
* 255 processes are running.  To solve this problem, getasn() and
* freeasn() should be rewritten.
*
* WHEN		WHO	WHAT
* ====		===	====
* 01/15/86	MA	Took out use of "drdos" to adjust "end" because
*			of using LINK68 instead of LO68.
* 12/17/85	MA	Optimizated to not reload MMU if same process
*			is already mapped.
* 08/07/85	MA	Changed to include "cpummu.h"
* 07/15/85	MA	Remove declaration of ospool array.  Add some
*			REG variables.
* 07/03/85	MA	Merge LMD into MD structure.
* 06/17/85	MA	Use _end and _drdos to find end of CDOS.
* 05/20/85	MA	Fixed bug in mmuload: for (offset == 0; ... )
*			should have been: for (offset = 0; ... )
*			Reorganized and sped up mmuload for 68000.
* 05/17/85	MA	Fixed freeasn to clear sdpid.  Check for setsd
*			returning error in mmuload.
* 04/17/85	MA	Increased LENOSM to 24K bytes.
* 04/04/85	MA	Added A2AGS function.
* 3/12/85	MA	Changed start of TPA to 30000 (CDOS is at B00).
* 3/12/85	MA	Put NODISP regions in MMUCLEAR, MMULOAD, and MMUFAIL
* 3/6/85	MA	Added L2PGS function.
* 1/04/85	MA	First created
* 6/11/86	DR-K	add include "sysbuild.h"
***********************************************************************/

#include "portab.h"
#include "sysbuild.h"
#include "struct.h"
#include "system.h"
#include "cpummu.h"

EXTERN WORD indisp;

#define LENOSM	0x10000L	/* length of o.s. memory pool */

#define TENBUG		0	/* are logical addresses same as physical */
				/* in user programs?  Should only be 1 if */
				/* we are using TENbug or stand-alone SID */
				/* to debug user programs. */

#if MMU
#define PAGESIZE 	4096L	/* MMU page size (must be long value) */
#define LOG2SIZE	12	/* log (base 2) of PAGESIZE */
#else
#define PAGESIZE	2L	/* if no MMU, allocate on 2-byte boundary */
#endif

#define PAGEMASK	(PAGESIZE-1L)


/*****************************************************************
*	MMU hardware constants
*****************************************************************/

#if MMU

#define MMUBASE	0xf1a800L	/* VME/10 address */

/* address space table */

#define AST1	*(BYTE *)(MMUBASE+0x02L)	/* user data */
#define AST2	*(BYTE *)(MMUBASE+0x04L)	/* user program */
#define AST5	*(BYTE *)(MMUBASE+0x0aL)	/* supervisor data */
#define AST6	*(BYTE *)(MMUBASE+0x0cL)	/* supervisor program */
#define AST7	*(BYTE *)(MMUBASE+0x0eL)	/* interrupt ack */

/* accumulator registers */

#define	LBA	*(WORD *)(MMUBASE+0x20L)	/* logical base address */
#define	LAM	*(WORD *)(MMUBASE+0x22L)	/* logical address mask */
#define	PBA	*(WORD *)(MMUBASE+0x24L)	/* physical base address */
#define	ASN	*(BYTE *)(MMUBASE+0x26L)	/* address space number */
#define	SSR	*(BYTE *)(MMUBASE+0x27L)	/* segment status register */
#define ASM	*(BYTE *)(MMUBASE+0x28L)	/* address space mask */

/* miscellaneous registers */

#define	DP	*(BYTE *)(MMUBASE+0x29L)	/* descriptor pointer */
#define	RWSSR	*(BYTE *)(MMUBASE+0x31L)	/* read/write seg status reg */
#define	LDO	*(BYTE *)(MMUBASE+0x3fL)	/* load descriptor operation */

/* bits in the segment status register */

#define	SSR_ENA		0x01		/* descriptor is enabled */
#define SSR_WP		0x02		/* segment is write protected */
#define	SSR_USED	0x80		/* segment has been used */

#endif		/* if MMU */

/***********************************************************************
*	external declarations
***********************************************************************/

EXTERN WORD end;		/* &end is address of end of bss section */
EXTERN LONG ramok();		/* RAM test routine - oemmmua.s */
EXTERN LONG osmlen;		/* length of mgetblk pool */


/***********************************************************************
*	forward declarations
***********************************************************************/

LONG L2PGS();
LONG L2PG();


/***********************************************************************
*	public variables
***********************************************************************/

BYTE mmupres;			/* true if mmu is present */


/***********************************************************************
*	local variables
***********************************************************************/

MLOCAL PD *curpd;	/* currently mapped process */

MLOCAL BYTE lrudp;	/* least recently used descriptor pointer */

/* map of process IDs indexed by descriptor number */

MLOCAL LONG sdpid[32] =
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/* map of address space numbers indexed by descriptor number */

MLOCAL BYTE sdasn[32] = 
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/* number of descriptors using each ASN, indexed by ASN */

MLOCAL BYTE usedasn[32] =
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
 
MLOCAL BYTE dnum = 31;	/* number of free descriptors */


/***********************************************************************
*	memory configuration tables - used by initfree
*
*	These tables define the initial TPA memory.  These are set
*	up presently for a split TPA on the VME/10.  The first TPA
*	section is just above CDOS.  Its size and length are filled
*	in by initfree, based on the actual size of CDOS.  The section TPA
*	section is the optional 256K byte memory board(s).
***********************************************************************/

/*	    m_link   m_start    m_length   reserved */
MD mdx2 = { (MD *)0, 0x180000L, 0x080000L, 0, 0, 0, 0, 0, 0 };
MD mdx1 = { &mdx2,   0,		0,	   0, 0, 0, 0, 0, 0 };


/***********************************************************************
*	initfree - construct initial TPA free list
*
*	This function sets up a linked list of MDs that describe
*	the initial TPA memory, then returns a pointer to the
*	head of the list.  This function sets up the first
*	three fields of the MDs (link, start, and length), while
*	the caller of this routine will set up the rest later.
*	Each TPA memory chunk is tested for its actual size, and the
*	MD list is adjusted accordingly.
*
*	This function is also responsible for setting the external
*	variable osmlen to the number of bytes that can be used
*	by the system for mgetblk space.  This space is taking from
*	the TPA.
***********************************************************************/

MD *initfree()
{
    REG MD *prev,*curr;
    REG LONG length;
    MD head;	/* dummy MD whose m_link points to first MD on list */

/* set up external variable that tells mgetblk how much memory to
   use for the internal O.S. memory pool */

    osmlen = LENOSM;		/* remaining bytes in o.s. pool */

/* use actual end of CDOS to define the size of the first TPA section */

    mdx1.m_start = L2PGS((LONG)&end);
    mdx1.m_length = 0x60000L - mdx1.m_start;

/* use head as the dummy head of the linked list */

    head.m_link = &mdx1;
    curr = &head;

/* run down the list of blocks testing each one; if not there, unlink it */

    while (curr = (prev = curr)->m_link)	/* run down the MD list */
    {
	if (!(length = ramok(curr->m_start,curr->m_length)))  /* ram there? */
 	{
	    prev->m_link = curr->m_link;	/* no - unlink this MD */
	    curr = prev;			/* back up current MD */
	}
	else
	    curr->m_length = length;		/* yes - set actual length */
    }

    return (head.m_link);			/* return linked list of MDs */
}

	
/***********************************************************************
*	ugetlas - user get logical address space
*
*	This function returns the next free logical address
*	for the current process's memory.  It does this by
*	running down the list of MDs for the current process
*	to find the maximum current logical address used.
*	The next available address is returned.
*
*	If logical addresses are to be the same as physical, just
*	return the physical address, don't do any address translation.
***********************************************************************/

BYTE	*ugetlas(mdp,pdp)
REG MD	*mdp;			/* physical memory descriptor */
REG PD	*pdp;			/* current process descriptor */
{
#if TENBUG	/* logical addresses are same as physical addresses */

    return (mdp->m_start);		/* just return physical address */

#else
#if MMU		/* logical addresses may be different from physical */

    REG MD *oldmdp;
    REG WORD memtype;
    REG LONG max;

/* MMU is present, look at all segments already allocated */

    max = 0L;					/* maximum logical address */
    for (memtype = M_CODE; memtype <= M_STACK ; memtype++)
    {

    /* try each md on the list */

	for (oldmdp = pdp->p_mem[memtype]; oldmdp; oldmdp = oldmdp->m_link)
	    if (oldmdp->m_lbase + L2PG(oldmdp->m_length) > max)
		max = oldmdp->m_lbase + L2PG(oldmdp->m_length);
    }
    return((max+0xffffL) & ~0xffffL);	/* round up to next 64K */

#else					/* no MMU */

    return (mdp->m_start);		/* just return physical address */

#endif
#endif
}


/*****************************************************************
*	L2PG - length to physical granularity for user tasks
*
*	Translate the value S to the minimum allocatable physical
*	memory granularity for user tasks
*****************************************************************/

LONG L2PG(s)
LONG s;
{
	return((s + PAGEMASK) & ~(PAGEMASK));
}


/*****************************************************************
*	L2PGS - length to physical granularity for system tasks
*
*	Translate the value S to the minimum allocatable physical
*	memory granularity for system tasks.
*****************************************************************/

LONG L2PGS(s)
LONG s;
{
	return((s + 1L) & ~1L);
}


/*****************************************************************
*	A2AG - address to address granularity
*
*	Translate the address S to the next lowest address that
*	is on a granularity boundary for user tasks.
*****************************************************************/

LONG A2AG(s)
LONG s;
{
	return(s & ~PAGEMASK);
}


/*****************************************************************
*	A2AGS - address to address granularity (system)
*
*	Translate the address S to the next lowest address that
*	is on a granularity boundary for supervisor tasks.
*****************************************************************/

LONG A2AGS(s)
LONG s;
{
	return(s & ~1L);	/* align on word boundaries */
}


/***********************************************************************
*	mmuclear - MMU initialization
*
*	This function clears the MMU hardware so that no user descriptors
*	are enabled, and the supervisor has access to all of memory.
***********************************************************************/

mmuclear()
{
    REG BYTE dpnum;

    mmupres = MMU;		/* let outside routines know if MMU present */

#if MMU

    NODISP

/* set current PD to 0 so that mmuload will work next time */

    curpd = 0;

/* initialize some variables */

    lrudp = 0;			/* index to least recently used descriptor */
    dnum = 31;			/* number of free descriptors */

/* set up descriptor 0 for supervisor so that logical = physical */

    if (setsd(0,0,0,0,0))
	return (FALSE);		/* load descriptor failed */

/* disable descriptors 1 - 31 */

    for (dpnum = 1; dpnum < 32; dpnum++)
    {
	DP = dpnum;		/* tell MMU which descriptor to work with */
	RWSSR &= ~SSR_ENA;	/* clear the enable bit in its status reg */
	freeasn(dpnum);		/* mark this descriptor as free */
    }

    DISPON

    return (TRUE);
#else
    return (FALSE);		/* no MMU */
#endif
}


/***********************************************************************
*	mmuload - load the mmu for bringing a process into context
*
*	This function calls sdload() to load the segment descriptors
*	for a process, then writes the address space number to the
*	address space table on the MMU.
***********************************************************************/

mmuload(pdp)
PD *pdp;
{
#if MMU
    NODISP				/* can't allow dispatches here */
    if (curpd != pdp)			/* already mapped to this process? */
    {
	curpd == pdp;			/* yes - set new current process */
	AST1 = AST2 = sdload(pdp);	/* load the address space nos. */
    }
    DISPON
#endif
}

    
/***********************************************************************
*	sdload - load the segment descriptors for a new process
*
*	This function loads the segment descriptors for a process,
*	then returns the address space number assigned to the process.
*	Because the 68010 can recover from bus faults,
*	this function doesn't load any descriptors on that processor.
*	Instead, we let	the bus fault handler (mmufail) do the
*	loading when needed.
*
*	On the 68010 this function just returns the process ID as
*	the ASN.  This won't work if there are more than 256
*	processes, because two processes will share the same ID.
*	Someday this code will have to be expanded to ensure that
*	the address space table number is written with a unique value.
*
*	On the 68000, this function loads all necessary descriptors to
*	map all the process's memory.  Right now it uses one descriptor
*	for each page, which means large programs run out of descriptors
*	real fast.  Someday we must find a way to minimize descriptor usage.
***********************************************************************/

#if MMU

MLOCAL sdload(pdp)
REG PD *pdp;	/* pointer to PD of process to load into MMU */
{
    REG WORD i,j;
    REG LONG pid;		/* copy of process id */
    REG WORD asn;		/* address space number for process */

#if MC68000
    
    REG LONG offset;		/* offset into memory segment */
    REG MD *mdp;		/* pointer to MD */
    WORD pages;			/* total program memory size in pages */

/* determine total memory requirements of process */

    pages = 0;
    for (i = 0; i < 4; i++)
    {
	mdp = pdp->p_mem[i];	/* get first MD pointer */
	while (mdp)
	{
	    pages += L2PG(mdp->m_length) >> LOG2SIZE;
	    mdp = mdp->m_link;
	}
    }
    if (pages >= 32)			/* too big for this MMU? */
    {
	DISPON				/* we'll never return */
	terminate(EM_KERN|E_BOUND);	/* kill this process */
    }
    if (pages == 0)			/* is it a system process? */
	return(0);			/* yes - user address space # 0 */

/* check if this process already has its descriptors in context */

    for (i = 1; i < 32; i++)		/* check all descriptors */
    {
	if (sdpid[i] == pdp->p_pid)	/* is this one used by this process? */
	    return (sdasn[i]);		/* yes - return its ASN */
    }

/* free up descriptors until we have enough for this process */

    while (pages > dnum)
    {
	for (i = 1; i < 32; i++)	/* reclaim up to 32 processes */
	{
	    if (++lrudp >= 32)		/* least recently used descriptor */
		lrudp = 1;
	    if ((pid = sdpid[lrudp]))	/* is this descriptor used? */
	    {				/* if so, save the victim's ID */
		for (j = 1; j < 32; j++)	/* check all descriptors */
		{
		    if (sdpid[j] == pid)	/* owned by victim process? */
		    {
			freeasn(j);		/* mark descriptor as unused */
			++dnum;			/* one more free descriptor */
			DP = j;			/* tell MMU which descriptor */
			RWSSR &= ~SSR_ENA;	/* tell MMU to disable it */
		    }
		}
	    }
	}
    }

/* create descriptors for each MD belonging to the process */

    j = 1;				/* index to sdpid */
    for (i = 0; i < 3; i++)		/* for each memory type */
    {
	mdp = pdp->p_mem[i];		/* get first MD in chain */
	while (mdp)			/* while still more MDs on chain */
	{
	    for (offset = 0; offset < mdp->m_length; offset += PAGESIZE)
	    {
		while (sdpid[j]) j++;	/* find a free descriptor */
		asn = getasn(pdp->p_pid,j);	/* get unique ASN */
		if (!setsd(j,					/* DP */
			   asn,					/* ASN */
			   (WORD)((mdp->m_lbase + offset) >> 8),/* LBA */
			   (WORD)(~(PAGEMASK >> 8)),		/* LAM */
			   (WORD)((mdp->m_start + offset) >> 8)	/* PBA */
			  )
		    )
		    --dnum;			/* one less descriptor free */
		else
		    freeasn(j);			/* didn't work, free this sd */
	    }
	    mdp = mdp->m_link;			/* get next MD in chain */
	}
    }
    return (asn);				/* return the assigned asn */

#else	/* it's a 68010 - hooray for restartable instructions */

    return(pdp->p_pid);				/* use process ID as ASN */

#endif
}
#endif


/***********************************************************************
*	mmufail - bus error handler
*
*	This function is called when there is a bus error, i.e. a page
*	fault.  It sets up a new descriptor in the MMU to map the new
*	page properly, so that the instruction that caused error can be
*	restarted.
***********************************************************************/

mmufail(fail,mdp,pdp)
REG LONG fail;		/* logical address that caused the page fault */
REG MD *mdp;		/* memory descriptor */
REG PD *pdp;		/* process descriptor */
{
#if (MMU & MC68010)
    REG BYTE status;			/* copy of segment status REG */
    REG WORD mask;			/* logical address mask */
    REG WORD asn;			/* address space number */
    REG WORD result;			/* result of setds function call */

    NODISP

    for (;;)				/* search for unused descriptor */
    {
	if (++lrudp == 32) lrudp = 1;	/* recycle descriptors */
	DP = lrudp;			/* tell MMU which descriptor to use */
	if ((status = RWSSR) & SSR_ENA)	/* is this descriptor enabled? */
	{
	    if (status & SSR_USED)	/* was it used recently */
		RWSSR = status & ~SSR_USED;	/* yes - mark it unused */
	    else break;			/* no - use this one */
	}
	else break;			/* no - use this one */
    }

/* we found a descriptor to used, compute the LAM and ASN */

    mask = ~(PAGEMASK >> 8);		/* compute the LAM */
    freeasn(lrudp);			/* zap old ASN in this sd */
    asn = getasn(pdp->p_pid,lrudp);	/* get the ASN for this process */

/* write the ASN assigned to this process into the address space table */

    AST1 = asn;				/* user data */
    AST2 = asn;				/* user program */

/* set up the segment descriptor to map the desired memory page */

    result = (!setsd(
		lrudp,				/* DP */
		asn,				/* ASN */
		(WORD)(fail >> 8) & mask,	/* LBA */
		mask,				/* LAM */
		(WORD)((fail - mdp->m_lbase + mdp->m_start) >> 8) & mask));
						/* PBA */
    DISPON

    return (result);
#else
    return (FALSE);
#endif
}


/***********************************************************************
*	setsd - set up a segment descriptor
*
*	This function loads an
*	MC68451 descriptor using the passed values of dp, asn, lba,
*	lam, and pba.  The result of the load descriptor operation
*	is returned: 0 if successful, or non-zero if unsuccessful.
***********************************************************************/

#if MMU
setsd(dp,asn,lba,lam,pba)
BYTE dp;	/* descriptor number */
BYTE asn;	/* address space number */
WORD lba;	/* logical base address */
WORD lam;	/* logical address mask */
WORD pba;	/* physical base address */
{
    DP = dp;			/* tell MMU which descriptor */
    ASN = asn;
    LBA = lba;
    LAM = lam;
    PBA = pba;
    ASM = 0xff;			/* address space mask */
    SSR = SSR_ENA;		/* enable the descriptor */
    return (LDO);		/* load descriptor, return error code */
}


/***********************************************************************
*	getasn - get a unique address space number
*
*	This function gets a unique address space number for a process
*	given the process ID and the descriptor number that is about to
*	be used by the process.  The descriptor is marked as used by
*	this process, and a unique address space number for this process
*	is returned.  It is assumed that the descriptor has already
*	been marked as unused by freeasn or by setting sdpid to 0, before
*	entry to this function.
*
*	The MMU itself is not touched by this function; only the sdasn,
*	spdid, and usedasn arrays are changed.
***********************************************************************/

getasn(pid,dp)
LONG pid;
WORD dp;
{
    sdasn[dp] = pid;			/* record the ASN used by this sd */
    sdpid[dp] = pid;
    return (pid);			/* return the unique ASN */
}

/***********************************************************************
*	freeasn - free up a segment descriptor and the associated asn
*
*	This function is the inverse of getasn.  It marks the descriptor
*	specified as unused.  If there are no more descriptors used by
*	the process, the address space number used by that process is
*	also marked as unused.  The MMU itself is not touched by this
*	function; only the sdasn, spdid, and usedasn arrays are changed.
***********************************************************************/

freeasn(dp)
WORD dp;
{
    sdasn[dp] = 0;
    sdpid[dp] = 0;
}

#endif
                                                                    
{
    sdasn[dp] = 0;
    sdpid[dp] = 0;
}

#endif
                                                                    