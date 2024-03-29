/*	@(#)vcdrtool.c	1.6		*/

#include "portab.h"
#include "io.h"
#include "system.h"
#include "vcdrv.h"
#include "vmconmd.h"
#include "vmecr.h"
#include "vterm.h"

EXTERN VCBLK	*pcb[] ;
EXTERN BYTE	ranupd[];
EXTERN DH	*sd_hdr[];
EXTERN BYTE	sd_units[];
EXTERN LONG	c_mxid[];

EXTERN LONG	s_return();

/* an ASR to update the physical screen */

VOID update(wrtok,unit)
REG LONG wrtok;
LONG unit;
{
	REG UWORD	p;
	UWORD		r, c, psize;
	LONG		vtop;
	REG VCBLK	*v;
	UBYTE		*dp;
	UBYTE		*cp;
	REG UBYTE	ch;
	REG DH		*t;		/* pointer to terminal sudriver hdr */

	if (wrtok) s_return(wrtok);
	wrtok = 0;

	v = (VCBLK *) pcb[(BYTE)unit];
	psize = frame(v)->fr_nrow * frame(v)->fr_ncol;
	t = sd_hdr[(BYTE)unit];

	/*
	 *	If we're starting from 0, then we will make a full
	 *	sweep unless we need to reschedule ourselves because
	 *	of a block from the sub-driver write() call.
	 *
	 *	Since we will make a full sweep of the whole plane,
	 *	set ranupd[(BYTE)unit] to 1, to optimize away other sweeps.
	 */

	if (!(p = v->v_dbuf))
		ranupd[(BYTE)unit] = 1;

	vtop = (LONG)((p + (v->v_top * frame(v)->fr_ncol)) % psize);
	cp = frame(v)->fr_pl[PL_CHAR] + vtop;
	dp = v->v_dirty + vtop;

	while ( p < psize && !wrtok )
	{
		if (*dp)	/* if this position has changed (dirty==1) */
		{
			c = p % frame(v)->fr_ncol;
			r = p / frame(v)->fr_ncol;
			if (v->v_currow != r || v->v_curcol != c)
			{
				wrtok = (*t->dh_special)
					(sd_units[(BYTE)unit],TGOTO,v,(LONG)r,(LONG)c);
				v->v_currow = r;
				v->v_curcol = c;
				continue;
			}
			else
			{
				/* Cursor in the right place */
				/* Write the character to the port driver */
				ch = *cp;
				/* filter funky 8 bit and control chars */
				if ( ch < ' ' || ch & 0x80 )
					ch = ' ';
				wrtok = (*t->dh_write)(sd_units[(BYTE)unit],v,ch);
				*dp = 0;
				v->v_curcol++;
			}
		}
		p++;
		if ( ++vtop < psize )
		{
			dp++;
			cp++;
		}
		else
		{
			/* we just wrapped around */
			vtop = 0;
			cp = frame(v)->fr_pl[PL_CHAR];
			dp = v->v_dirty;
		}
	}

	/* position cursor */
	if( !wrtok )
	{
		wrtok = (*t->dh_special)
			(sd_units[(BYTE)unit],TGOTO,v,
			(LONG)v->v_cursx,(LONG)v->v_cursy);
		v->v_currow = v->v_cursx;
		v->v_curcol = v->v_cursy;
	}

	if (wrtok)
	{
		/* We've overflowed the Port driver */
		/* Wait until it's ready and continue */
		v->v_dbuf = p;
		nextasr(wrtok,update,wrtok,unit,200);
		return;
	}

	v->v_dbuf = 0;

	/* We are done with active pass, but more dirty	*/
	/* regions may have appeared behind us if	*/
	/* ranupd[unit] is greater than 1.		*/

	if ( --ranupd[(BYTE)unit] > 0)
		doasr(update,0L,unit,200);
	else
		mxrel( c_mxid[(BYTE)unit] );	/* release the sync block now */
}

clip(d,row,col,nrow,ncol)
REG RECT *d;
WORD row, col, nrow, ncol;
{
	WORD		dr,dc;
	REG WORD	r,c;

	dr = d->r_row + d->r_nrow - 1;
	r = row + nrow - 1;
	dc = d->r_col + d->r_ncol - 1;
	c = col + ncol - 1;

	d->r_row = max(d->r_row,row);
	d->r_col = max(d->r_col,col);

	r = min(r,dr);
	c = min(c,dc);

	d->r_nrow = r - d->r_row + 1;
	d->r_ncol = c - d->r_col + 1;

	if ((d->r_row > r) || (d->r_col > c))
		return(FAILURE);

	return(SUCCESS);
}


off_pcursor(unit)	/*** turn off the physical cursor ****/
BYTE	unit;
{
	REG VCBLK	*v;

	v = (VCBLK *)pcb[unit];
	v->v_mode |= M_PCURSOFF;	/* mark it as physically off now */
	if (unit == BITMAP)
	 {
		CRTC_ADDR->addr_reg = CURSTART;
		CRTC_ADDR->reg_file = CURS_OFF;
	 }
	else
	 {
		(*sd_hdr[unit]->dh_write)(sd_units[unit],v,ESCAPE);
		(*sd_hdr[unit]->dh_write)(sd_units[unit],v,'x');
		(*sd_hdr[unit]->dh_write)(sd_units[unit],v,'5');
	 }
}

on_pcursor(unit)	/*** turn on the physical cursor ****/
BYTE	unit;
{
	REG VCBLK	*v;
	
	v = (VCBLK *)pcb[unit];
	v->v_mode &= ~M_PCURSOFF;	/* mark it as physically on now */
	if (unit == BITMAP)
	 {
		CRTC_ADDR->addr_reg = CURSTART;
		CRTC_ADDR->reg_file = CURS_ON;
	 }
	else
	 {
		(*sd_hdr[unit]->dh_write)(sd_units[unit],v,ESCAPE);
		(*sd_hdr[unit]->dh_write)(sd_units[unit],v,'y');
		(*sd_hdr[unit]->dh_write)(sd_units[unit],v,'5');
	 }
}

beepbell(u)	/*** beep the physical bell ***/
BYTE	u;
{
	if ((BYTE)u != BITMAP)
		(*sd_hdr[(BYTE)u]->dh_write)(sd_units[(BYTE)u],pcb[(BYTE)u],0x0007);
}

vpos_cursor(r,c)
WORD	r,c;
{
	REG WORD curpos;	/* byte offset to cursor postion */

	if ( c >= V10NCOL ) c = V10NCOL - 1;
	curpos = r * V10NCOL + c;
	/* WE NEED TO MAP THIS !!!!! */
	CRTC_ADDR->addr_reg = CURLOW;
	CRTC_ADDR->reg_file = (BYTE)curpos;
	CRTC_ADDR->addr_reg = CURHIGH;
	CRTC_ADDR->reg_file = (BYTE)(curpos >> 8);
}

/*** c_update is called from a doasr in c_copy() to force update to be run ***/
/***  from ASR context, which is already true from CRM's vccopy(), but not ***/
/***  when s_copy() enters from the system entry.  c_update() gets us into ***/
/***  the mxregion, so that full-screen write optimizations won't write at ***/
/***  the same time							   ***/

c_update(null,unit)
LONG	null,unit;
{			/* run update in mxregion */
	LONG	e;
	if ( e = asrmx( c_mxid[unit] )  )
		nextasr( e, update, e, (LONG)unit, 200);
	   else
		update( 0L, unit );
}
	
                                                                                                                          

                                                                                                                          