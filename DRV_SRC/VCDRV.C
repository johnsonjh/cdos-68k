/*	@(#)vcdrv.c	1.10		*/
/*======================================================================*
 *   Version 1.00       Console Driver					*
 *			Main Code Section				*
 *======================================================================*/
/*      INCLUDES                                                        */

#include "portab.h"
#include "io.h"
#include "system.h"
#include "vcdrv.h"
#include "vmconmd.h"
#include "vmconibm.h"
#include "vmecr.h"

EXTERN	VOID	vbfill();

/************************************************************************/
/* forward referenced functions                                         */
/************************************************************************/

GLOBAL  LONG
	c_init(),
	c_subdrvr(),
	c_uninit(),
	c_select(),
	c_flush(),
	c_copy(),
	c_write(),
	c_get(),
	c_set(),
	c_serxlat(),
	c_special();

/************************************************************************/
/* local initializations						*/
/************************************************************************/

#define FLAGVAL  0

/************************************************************************/
/*      Driver header							*/
/*  This header must be the first structure in the data segment		*/
/*  so, don't put any data generating code before this section		*/
/************************************************************************/

DH  c_dh =
{       
	DVR_CON,MAXCDUNIT,FLAGVAL,
	c_init,
	c_subdrvr,
	c_uninit,
	c_select,
	c_flush,
	c_copy,
	c_write,
	c_get,
	c_set,
	c_special,
	0L,0L,0L,0L,0L
};

typedef VOID	(*PINPTR)();

PINPTR	ckb_pin[MAXCDUNIT];

GLOBAL VCBLK	*pcb[MAXCDUNIT];/* Physical Virtual console data blocks */

GLOBAL FRAME	v10fr;		/* stores address of screen memory */

WORD    ckb_intrp();		/* forward ref the interupt routine */
BYTE    ranupd[MAXCDUNIT];	/* if update is running */
BYTE	cd_units[MAXCDUNIT];	/* conman's unitno indexed by sub-driver's */
BYTE    sd_units[MAXCDUNIT];    /* sub-driver's unitno indexed by drvr_unitno */
DH      *sd_hdr[MAXCDUNIT];     /* stores drvr headers of sub-drivers   */
LONG	c_rmflg[MAXCDUNIT];	/* a write flag for the CRM per unit	*/
LONG	c_mxid[MAXCDUNIT];	/* an mx region per unit		*/

/************************************************************************/
/*      console parameters						*/
/*									*/
/*	Here are the parameters that the calls to c_get will return	*/
/*	they are up front here, and in order, for speed of byte copying	*/
/*	and ease of changing.						*/
/*									*/
/************************************************************************/

CTE     contab[MAXCDUNIT] =
{
	/*	Memory mapped console definition	*/
	{ IBMNROW, IBMNCOL, COLOR|NUMPAD, PL_UCHAR|PL_UATTR,
	  ATTRBITS, 0, USA, MFNKEYS, 0, 0, 1, 1, 0, 0L, 0L },
	/*	Serial mapped console definitions	*/
	{ 24, IBMNCOL, NUMPAD, PL_UCHAR|PL_UATTR,
	  ATTRBITS, 0, USA, SFNKEYS, 0, 0, 1, 1, 0, 0L, 0L },
	{ 24, IBMNCOL, NUMPAD, PL_UCHAR|PL_UATTR,
	  ATTRBITS, 0, USA, SFNKEYS, 0, 0, 1, 1, 0, 0L, 0L }
};

/************************************************************************/
/*      c_init								*/
/*									*/
/*      Initialization routine for console driver.  We need to cast	*/
/*	"LONG" unit number to "BYTE" because there are really install()	*/
/*	flags in the upper word, need to strip them (we don't use	*/
/*	them).								*/
/*      								*/
/************************************************************************/

LONG    c_init(unitno)
LONG	unitno;
{
	CDMAPPB	mpb;		/* for making absolute memory address ours */

	if ((BYTE)unitno >= MAXCDUNIT) return( ED_CON | E_UNITNO );

	/*
	 *	Allocate mutual exclusion flags used to synchronize
	 *	write and copy/alter.
	 */

	c_rmflg[(BYTE)unitno] = flagget();
	c_mxid[(BYTE)unitno] = mxinit();

	/*
	 *	The first unit through here gets to be
	 *	the memory-mapped video.
	 */

	if ((BYTE)unitno == BITMAP)
	{
		mpb.zero = 0;
		mpb.physaddr = (LONG) DISPADR;
		mpb.length = DISPSZ;
		v10fr.fr_pl[PL_CHAR] = (v10fr.fr_pl[PL_ATTR]
			= (BYTE *)mapphys(&mpb,MAPDATA)) + 1;
		v10fr.fr_nrow = V10NROW;
		v10fr.fr_ncol = V10NCOL;
		v10fr.fr_use = PL_UCHAR | PL_UATTR;
		return(((LONG) DVR_KEYBD << 16) | DVR_CON);
	}
	else
	{
		/* we need a PORT subdriver */
		return( ((LONG) DVR_TERM << 16) | DVR_CON );
	}
}

/************************************************************************/
/*	c_subdrvr							*/
/*									*/
/*	The console sub-drive entry is called after the initialization	*/
/*	routine tells install it needs subdriver's.  We must store	*/
/*	the sub-driver driver header address and associate it with	*/
/*	the physical console unit by storing the sub-drive unit		*/
/*	in the sd_units[] array which is indexed by physical console	*/
/*	unit number.  We do a vice-versa with cd_units[].  You must	*/
/*	have guessed by now that the sub-driver unit number does not	*/
/*	correspond to the physical console unit number.			*/
/*									*/
/*	We ignore the access rights of the sub-driver.			*/
/*									*/
/************************************************************************/

LONG    c_subdrvr(pb)
REG SDPB     *pb;
{
	sd_units[pb->sd_unit] = pb->sd_sdunit;		/* sub-drvr unitno */
	sd_hdr[pb->sd_unit] = pb->sd_sdheader;		/* header address */
	cd_units[pb->sd_sdunit] = pb->sd_unit;		/* drvr unitno */

	return(E_SUCCESS);
}

/************************************************************************/
/*      c_uninit                                                        */
/************************************************************************/

LONG    c_uninit(unitno)
BYTE    unitno;
{
	DPBLK	upb;

	upb.dp_unitno = unitno;
	if( c_rmflg[unitno] = flagrel(c_rmflg[unitno]) ) return(c_rmflg[unitno]);
	if ( c_mxid[unitno] = mxuninit(c_mxid[unitno]) ) return(c_mxid[unitno]);
	return( c_flush(&upb) );
}

/************************************************************************/
/*      c_select                                                        */
/************************************************************************/

LONG    c_select(pb)
REG CDSELECT	*pb;
{
	CDSP0		sb;
	BYTE		unitno;
	LONG		r;

	unitno = pb->unitno;

	ckb_pin[unitno] = (PINPTR) pb->kbd_pin;

	/* call the special 'create VC' function to allocate memory */
	sb.cds_unit = unitno;
	sb.cds_option = 0;	/* create */
	if( unitno != BITMAP )
	{
		contab[unitno].ct_rows = IBMNROW-1;
		sb.cds_flags |= SERPHYSCON;	/* force dirty plane allocation */
	}
	sb.cds_rows = contab[unitno].ct_rows;
	sb.cds_cols = contab[unitno].ct_cols;
	pcb[unitno] = (VCBLK *) c_special( &sb );

	/*
	 *	Call the sub-driver's select routine.
	 */

	pb->unitno = sd_units[unitno];
	return( (*sd_hdr[unitno]->dh_select) (pb) );
}

/************************************************************************/
/*      c_flush                                                         */
/************************************************************************/

#define cds(p)	((CDSP0 *)(p))	/* a convenient macro */

LONG    c_flush(pb)
DPBLK     *pb;
{
	LONG    r;
	BYTE	unit;
	CDSP	sp;

	unit = pb->dp_unitno;
	/* call the sub-drives flush(this unitno) */
	pb->dp_unitno = sd_units[unit];
	if( (r = (*sd_hdr[pb->dp_unitno]->dh_flush)(pb)) < 0L )
		return( r );

	/*
	 *	Call special delete VC to sfree the memory
	 *	associated with the Physical Image of the
	 *	console.
	 */

	sp.cds_unit = unit;
	sp.cds_option = DELETEVC;
	sp.cds_vcid = pcb[unit];
	return( c_special( &sp ) );
}

/************************************************************************/
/*      c_get								*/
/*									*/
/*		Get table information about console driver.		*/
/*									*/
/************************************************************************/


/*
 *	A little macro to give us reasonable line length
 *	in creepy code below.
 */

#define ckspace(s,p,n)	((s) -= sizeof((p))) >= sizeof((n))

LONG	c_get(pb)
REG DPBLK	*pb;
{
	REG CTE		*sp;
	REG CTE		*tp;
	REG WORD	sz;

	tp = (CTE *)pb->dp_buffer;
	if ( pb->dp_flags & DPF_UADDR )
	{
		mapu(pb->dp_pdaddr);
		tp = (CTE *)saddr(tp);
	}

	sp = &contab[pb->dp_unitno];
	sz = (WORD)pb->dp_bufsiz;

	/*
	 *	According to the get/set specification
	 *	we must return whole fields ONLY if the
	 *	buffer given us is less than our table size.
	 *	So you get the creepy code below... it is
	 *	reasonably efficient, the alternatives were
	 *	worse (unneeded tests after we copied all we could,
	 *	artificial constructs).
	 */

	if(sz >= sizeof(tp->ct_rows)) 
	{
	 tp->ct_rows = sp->ct_rows;
	 if( ckspace(sz,tp->ct_rows,tp->ct_cols) )
	 {
	  tp->ct_cols = sp->ct_cols;
	  if( ckspace(sz,tp->ct_cols,tp->ct_flags) )
	  {
	   tp->ct_flags = sp->ct_flags;
	   if( ckspace(sz,tp->ct_flags,tp->ct_planes) )
	   {
	    tp->ct_planes = sp->ct_planes;
	    if( ckspace(sz,tp->ct_planes,tp->ct_attrp) )
	    {
	     tp->ct_attrp = sp->ct_attrp;
	     if( ckspace(sz,tp->ct_attrp,tp->ct_extp) )
	     {
	      tp->ct_extp = sp->ct_extp;
	      if( ckspace(sz,tp->ct_extp,tp->ct_country) )
	      {
	       tp->ct_country = sp->ct_country;
	       if( ckspace(sz,tp->ct_country,tp->ct_nfkys) )
	       {
		tp->ct_nfkys = sp->ct_nfkys;
		if( ckspace(sz,tp->ct_nfkys,tp->ct_buttons) )
		{
		 tp->ct_buttons = sp->ct_buttons;
		 if( ckspace(sz,tp->ct_buttons,tp->ct_serial) )
		 {
		  tp->ct_serial = sp->ct_serial;
		  if( ckspace(sz,tp->ct_serial,tp->ct_murow) )
		  {
		   tp->ct_murow = sp->ct_murow;
		   if( ckspace(sz,tp->ct_murow,tp->ct_mucol) )
		   {
		    tp->ct_mucol = tp->ct_mucol;
		    if( ckspace(sz,tp->ct_mucol,tp->ct_pcframe) )
		    {
		     tp->ct_pcframe = sp->ct_pcframe;
		     if( ckspace(sz,tp->ct_pcframe,tp->ct_conv8) )
		     {
		      tp->ct_conv8 = sp->ct_conv8;
		      if( ckspace(sz,tp->ct_conv8,tp->ct_conv16) )
		      {
		       tp->ct_conv16 = sp->ct_conv16;
		      }
		     }
		    }
		   }
		  }
		 }
		}
	       }
       	      }
	     }
	    }
	   }
	  }
	 }
	}

	if ( pb->dp_flags & DPF_UADDR )
		unmapu();

	return(E_SUCCESS);
}

/****************************************************************/
/*      c_set                					*/
/*								*/
/*	Set table information about console driver.		*/
/*	NOT REQUIRED, and NOT IMPLEMENTED for VME/10.		*/
/*								*/
/****************************************************************/

LONG	c_set(pb)
DPBLK	*pb;
{
	return(ED_CON | E_IMPLEMENT);
}


/************************************************************************/
/*      c_special							*/
/*									*/
/*	Special functions for the console driver:			*/
/*									*/
/*		CREATEVC	- Create a virtual console.		*/
/*		DELETEVC	- Delete an existing virtual console.	*/
/*		PCTOVC		- Convert an IBM PC-lookalike console	*/
/*				  to an optimized virtual console.	*/
/*				  NOT IMPLEMENTED ON VME/10.		*/
/*		VCTOPC		- Convert an optimized virtual console	*/
/*				  to an IBM PC-lookalike console.	*/
/*				  NOT IMPLEMENTED ON VME/10.		*/
/*		PTGETBL		- Call a PORT driver to get the table	*/
/*		PTSETBL		- Call a PORT driver to set the table	*/
/*									*/
/************************************************************************/

LONG	c_special(pb)
REG CDSP	*pb;
{
	REG FRAME	*f;
	REG BYTE	*temp;
	REG BYTE	pl;
	BYTE		unit,*pchar,*pattr,*pext,*pdirty;
	REG WORD	psize;
	LONG		r;			/* retc and plane memory size */

	unit = pb->cds_unit;

	switch (pb->cds_option)
	{
	case CREATEVC:
		psize = cds(pb)->cds_cols * cds(pb)->cds_rows;
		/* word align - round up, for the 68000/68010 */
		if( psize & 1 ) psize += 1;
		pl = contab[unit].ct_planes;

		/*
		 *	Allocate the memory for our physical console
		 *	and VCBLK.
		 *
		 *	Because we must translate our IBM image to
		 *	the VME/10 memory map, we never point directly
		 *	at the VME/10 memory map with the BITMAP pcb.
		 */

		temp = (BYTE *)salloc((LONG)(((pl & PL_CHAR) ? psize : 2)
			+ ((pl & PL_ATTR) ? psize : 2)
			+ ((pl & PL_EXT) ? psize : 2)
			+ ((cds(pb)->cds_flags & SERPHYSCON) ? psize : 0)
			+ (sizeof(VCBLK))));

		if (pl & PL_UCHAR)
		{
			pchar = temp;
			temp += psize;
			vbfill(pchar,psize,BLANK);
		}
		else
		{
			temp = (pchar = temp) + 2;
			*pchar = BLANK;
		}

		if (pl & PL_UATTR)
		{ 
			pattr = temp;
			temp += psize; 
			/* initialize attribute plane */
			vbfill(pattr,psize,IBMINITATTR);
		}
		else
		{
			pattr = temp;
			temp += 2;
			*pattr = IBMINITATTR;
		}

		if (pl & PL_UEXT)
		{
			pext = temp;
			temp += psize;
			vbfill(pext,psize,NULLC);
		}
		else
		{
			pext = temp;
			temp += 2;
			*pext = NULLC;
		}

		/* if this is to be a serial physical console */
		/* create and zero a dirty plane */
		if (cds(pb)->cds_flags & SERPHYSCON)
		{
			temp = (pdirty = temp) + psize;
			vbfill(pdirty, psize, 0);
		}
		f = (FRAME *) temp;
		vbfill((VCBLK *)f, sizeof(VCBLK), 0);

		f->fr_pl[PL_CHAR] = pchar;
		f->fr_pl[PL_ATTR] = pattr;
		f->fr_pl[PL_EXT] = pext;
		vcblk(f)->v_dirty = pdirty;
		f->fr_nrow = cds(pb)->cds_rows;
		f->fr_ncol = cds(pb)->cds_cols;
		f->fr_use = pl;

		/* Invalidate current row and column */
		vcblk(f)->v_curcol = f->fr_ncol;
		vcblk(f)->v_currow = f->fr_nrow;

		vcblk(f)->v_att = IBMINITATTR;
		return( (LONG)f );

	case DELETEVC: 
		if ( !( f = (FRAME *)pb->cds_vcid ) )
			if(!( f = (FRAME *) pcb[unit]) )  /* already freed */
				return(E_SUCCESS);
		/* give back the memory */
		return( r = sfree((BYTE *) f->fr_pl[PL_CHAR]) );

	case VCTOPC:		/* Console from a PC look-alike */
		return(ED_CON | E_IMPLEMENT);

	case PCTOVC:		/* Convert to an optimized Virtual */
		return(ED_CON | E_IMPLEMENT);

	case PTGETBL:
	case PTSETBL:
		if ( unit >= MAXCDUNIT ) return(ED_CON | E_UNITNO);
		if ( unit == BITMAP ) return(ED_CON | E_IMPLEMENT);
		pb->cds_unit = sd_units[unit];
		r = (*((pb->cds_option == PTGETBL) ? sd_hdr[unit]->dh_get : sd_hdr[unit]->dh_set))(pb);
		pb->cds_unit = unit;	/* restore original unit number */
		return r;
	} /* end of case */

	return(ED_CON | E_IMPLEMENT);
}

/************************* end of main routines *****************************/
ase */

	return(ED_CON | E_IMPL
NT);
}

/************************* end of main routines *****************************/
ase */

	return(ED_CON | E_IMPL