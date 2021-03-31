/*	@(#)vcdrcopy.c	1.9		*/
/*======================================================================*
 *   Version 1.9        Console Driver
 *			Copy/Alter function
 *
 *   VERSION	DATE      BY    CHANGE/COMMENTS
 *   =======	====	  ==	===============
 *   1.9	10/3/85   MA	Fixed the bug in copying frame to itself
 *				with overlap (caused DREDIX to reverse
 *				the order of lines on screen after insert).
 *   2.0	1/28/86	 DR-K	store/set pcurs,modes at REORDER
 *======================================================================*/

/*
 *	General Notes:
 *
 *	The code below looks quite complicated, but is really pretty
 *	straightforward.  First this entry point handles both Copy
 *	and Alter, a bit tells us which we are doing.  The approach
 *	for both is simple: we have a destination frame which we
 *	are to update, with either a copy or alter.  To do this we
 *	run two nested loops:
 *
 *		for <all planes>
 *			for <every row we are modifying>
 *				perform alter or copy...
 *
 *	For the alter operation, we are given an array of bytes,
 *	which we take two at a time (the AND byte and the EOR byte)
 *	and apply to each row of the destination plane, row by row.
 *	We repeat while more planes, but we do check if there really
 *	is a destination plane (it may be A SINGLE BYTE PLANE).
 *
 *	For the copy operation, we COPY a source frame to a destination
 *	frame.  The source and destination are bounded by a rectangle
 *	which we clip to make the same size.  Again we watch for
 *	SINGLE BYTE PLANES.
 *
 *	Read the System Guide for a description of the Frames and
 *	Rectangles.
 *
 *	Some disclaimers: this code was ported from a Concurrent DOS
 *	prototype and is more misleading than it has to be.  But
 *	I hesitate to blindly change variable names, it would diverge
 *	to much from other versions of the driver.  So a little
 *	explanation of the purpose of some of the variables (you
 *	can't tell the players without a score card):
 *
 *		srow, drow - are NOT row numbers but rather
 *			     the source and destination indices
 *			     into the frame's byte array representation
 *			     of the plane.  Actually they are the:
 *			     starting row * number of columns + the
 *			     column offset into the starting row.
 *
 *		isrow, idrow - Of course, to correspond to the above,
 *			     must be the number of bytes to add to
 *			     srow and drow to get the starting position
 *			     (byte index) of the next row to alter or copy.
 *			     It is basically the number of columns.
 *
 *		ndrow, nsrow - the number of bytes in a
 *			     plane, used to wrap around the virtual
 *			     tops vstop and vdtop.
 *
 *	The extension plane support is valid, but I don't guarantee
 *	all aspects of single byte planes are covered.
 */

#include "portab.h"
#include "io.h"
#include "system.h"
#include "vcdrv.h"

EXTERN	FRAME	v10fr;		/* VME/10 */
EXTERN	VCBLK	*pcb[];
EXTERN	BYTE	ranupd[];
EXTERN  LONG    saddr();
EXTERN	VOID	c_update();
EXTERN	VOID	on_pcursor();
EXTERN	VOID	off_pcursor();

/************************************************************************/
/*      c_copy - copy/alter a destination rectangle                     */
/************************************************************************/

LONG c_copy( pb )
CDCOPALT   *pb;
{
	BYTE		unit,option,pl2use,*altb;
	REG FRAME	*sf,*df;
	FRAME		lsf,ldf;	/* local copies of user frames */
	RECT		*psr,*pdr;	/* adjusted ptr to rectangles	*/
	RECT		sr, dr;		/* local copies of rectangles */

	WORD		vstop,vdtop,	/* ptr to topline */
			srow,drow;	/* the row we are working on now */
	REG WORD	ndrow;		/* number of rows in this frame */
	REG WORD	idrow;		/* increment between rows */
	WORD		rmax;
	UBYTE		*dp;		/* current ptr into a plane */
	UBYTE		*dirt_p;
	WORD		p,r;		/* temp vars */
	BYTE		pframe;		/* destination is to physical	*/
	WORD		isvp;		/* destination is to VME-10 screen */
	UWORD		flags;

	isvp = FALSE;
	vstop = vdtop = 0;
	pframe = FALSE;
	unit = pb->cd_unit;
	option = pb->cd_opt;
	flags = pb->cd_flags;

	/* translate all user addresses to system memory addresses */
	if ( flags & DPF_UADDR )
	{
		mapu( pb->cd_pd );
		pdr = (RECT *)saddr( pb->cd_drect );
		psr = (RECT *)saddr( pb->cd_srect );
	}
	else
	{
		pdr = pb->cd_drect;
		psr = pb->cd_srect;
	}

	if (option & DESTUFRAME)	/* destination */
	{
		if ( flags & DPF_UADDR )
		{
			/* make a local copy */
			vbcopy(saddr( pb->cd_dfram ),df = &ldf,(WORD)sizeof(FRAME));
			/* fixup plane addresses */
			for (p = 0; p < PL_NPLANE; p++)
				df->fr_pl[p] = (BYTE *) saddr( df->fr_pl[p] );
		}
		else df = pb->cd_dfram;
	}
	else
	{
		if (!( df = pb->cd_dfram ))  /* if to VFRAME */
		{
			df = (FRAME *)pcb[unit];		/*  if to PFRAME */
			pframe = TRUE;
			if (unit == BITMAP)
			{
				isvp = TRUE;
				pframe = FALSE;
			}
		}
		vdtop = vcblk(df)->v_top;
	}

	/* We may only be moving the cursor, get new values and exit */
	if (flags & CP_CURMOVE)
	{
		vcblk(df)->v_cursx = pb->cd_crow;
		vcblk(df)->v_cursy = pb->cd_ccol;
		goto exitcopy;
	}

	/*	Make a local copy of destination rectangle, we may modify it. */
	vbcopy( pdr, &dr, (WORD)sizeof(RECT) );

	/* clip destination rectangle to be within frame */
	if (clip(&dr,0,0,df->fr_nrow,df->fr_ncol) == FAILURE)
		goto exitcopy;

	dirt_p = 0L;
	pl2use = flags & PL_USEALL & df->fr_use;

	/* end of common code section */

	if (option & ALTER)		/* if this is an ALTER operation ***/
	{
		altb = (BYTE *) (&(pb->cd_sfram));
		rmax = dr.r_row + dr.r_nrow;
		ndrow = df->fr_nrow * (idrow = df->fr_ncol);
		drow = (((dr.r_row + vdtop) * df->fr_ncol) + dr.r_col) % ndrow;
		/* for each plane */
		for (p = 0; p < PL_NPLANE; p++, altb += 2)
		{
			REG WORD  tmpdrow;  /* the row we are working on */

			/* first check to see if plane is being used */
			if (!( pl2use & (1<<p)))
				continue;
			tmpdrow = drow;
			for (r = dr.r_row; r < rmax; r++) /* for each row */
			{
				if (pframe) dirt_p = pcb[unit]->v_dirty + tmpdrow;
				vbalter((df->fr_pl[p] + tmpdrow),
					*altb,*(altb+1),dr.r_ncol,dirt_p);
				if ( isvp )
					itovcp((df->fr_pl[p] + tmpdrow),
					    ((WORD *)v10fr.fr_pl[p])+(r*df->fr_ncol+dr.r_col), dr.r_ncol, p);
				tmpdrow = (tmpdrow + idrow) % ndrow;
				if (tmpdrow < 0) tmpdrow += ndrow;
			}
		}
	}  /* end of alter() */
	else	 /* it's a copy() operation */
	{
		REG WORD  	nsrow;	/* number of rows in this frame */
		REG WORD	isrow;	/* increment between rows */
		WORD	ploc;	/* physical VME/10 screen location */
		WORD	pinc;	/* physical screen increment */

		if (option & SRCUFRAME)		/* source */
		{
			if ( flags & DPF_UADDR )
			{
				/* make a local copy */
				vbcopy(saddr(pb->cd_sfram),sf = &lsf,(WORD)sizeof(FRAME));
				/* fixup plane addresses */
				for (p = 0; p < PL_NPLANE; p++)
					sf->fr_pl[p] = (BYTE *) saddr( sf->fr_pl[p] );
			}
			else sf = pb->cd_sfram;
		}
		else
		{
			if (!( sf = pb->cd_sfram ))  /* if from PFRAME */
				sf = (FRAME *)pcb[unit];
			vstop = vcblk(sf)->v_top;
			if( !(option & DESTUFRAME) && (flags & CP_REORDER) )
			{
				vcblk(df)->v_att = vcblk(sf)->v_att;
				vcblk(df)->v_escflg = vcblk(sf)->v_escflg;
				vcblk(df)->v_tempx = vcblk(sf)->v_tempx;
				vcblk(df)->v_tempy = vcblk(sf)->v_tempy;
					/* preserve the state of PCFRAME and */
					/* PCURSOFF bits */
				vcblk(df)->v_mode &= PCFRAME + M_PCURSOFF;
				vcblk(df)->v_mode |= (vcblk(sf)->v_mode & ~(PCFRAME+M_PCURSOFF) );
					/* if destination is physical and */
					/* pcurs on/off != vcurs on/off then */
					/* call pcurson() or pcursoff()	*/
				if (!(pb->cd_dfram))
					if (vcblk(df)->v_mode & M_PCURSOFF)
							off_pcursor(unit);
						else on_pcursor(unit);
			}
		}

		_bmove( psr, &sr, (WORD)sizeof(RECT) );

		/* if not all single byte planes */
		if (sf->fr_use & PL_USEALL)
		{
			if (clip(&sr,0,0,sf->fr_nrow,sf->fr_ncol) == FAILURE)
				goto exitcopy;
			/* Force rectangles to be the same size		*/
			dr.r_ncol = sr.r_ncol = min(sr.r_ncol,dr.r_ncol);
			dr.r_nrow = sr.r_nrow = min(sr.r_nrow,dr.r_nrow);
		}

		rmax = dr.r_nrow + dr.r_row;
		nsrow = sf->fr_nrow * sf->fr_ncol;
		ndrow = df->fr_nrow * df->fr_ncol;

		srow = sr.r_row + vstop;
		drow = dr.r_row + vdtop;
		isrow = sf->fr_ncol;
		idrow = df->fr_ncol;

		/*
		 *	If we are copying a frame to itself, we
		 *	check for overlap of the destination and
		 *	source rectangles and reverse the direction
		 *	of the copy if they overlap.  Otherwise
		 *	we'd be copying already modified source to
		 *	the destination (think about it).
		 */

		if (isvp)
		{
			ploc = dr.r_row;
			pinc = df->fr_ncol;
		}

		if (sf == df && dr.r_row >= sr.r_row )
		{
			srow += sr.r_nrow - 1;
			isrow = -isrow;
			drow += dr.r_nrow - 1;
			idrow = -idrow;
			if (isvp)
			{
				ploc += dr.r_nrow - 1;
				pinc = -pinc;
			}
		}

		srow = (srow * sf->fr_ncol + sr.r_col) % nsrow;
		drow = (drow * df->fr_ncol + dr.r_col) % ndrow;
		if (isvp)
			ploc = ploc * df->fr_ncol + dr.r_col;

		/*  Copy Source RECT to Destination RECT	*/
		/*  Do nothing if Destination Use bit is off	*/

		for (p = 0; p < PL_NPLANE; p++)	/* for each plane */
		{
			REG WORD  tmpsrow,tmpdrow;	/* the row we are working on */
			REG BYTE  msk;
			WORD temploc;			/* phys. location */

			if (!(pl2use & (msk = 1<<p)))
				continue;
			tmpsrow = srow;
			tmpdrow = drow;
			if (isvp)
				temploc = ploc;
			for (r = dr.r_row; r < rmax; r++)	/* for each row */
			{
				dp = df->fr_pl[p] + tmpdrow;
				if (pframe) dirt_p = pcb[unit]->v_dirty + tmpdrow;
				if (sf->fr_use & msk)
					vbdcopy((sf->fr_pl[p] + tmpsrow),dp,sr.r_ncol,dirt_p);
				else	/* 1 byte plane */ 
					vbdfill(dp,dr.r_ncol,*sf->fr_pl[p],dirt_p);
				if ( isvp )
				{
					itovcp((df->fr_pl[p] + tmpdrow),
					    ((WORD *)v10fr.fr_pl[p])+temploc, dr.r_ncol, p);
					temploc += pinc;
				}
				tmpsrow = (tmpsrow + isrow) % nsrow;
				if (tmpsrow < 0) tmpsrow += nsrow;
				tmpdrow = (tmpdrow + idrow) % ndrow;
				if (tmpdrow < 0) tmpdrow += ndrow;
			}
		}
	}  /* end of copy() */

exitcopy:

	/*
	 *	If destination is not to User memory
	 *	then we might have to fixup the cursor
	 *	and start physical update process.
	 */

	if (!(option & DESTUFRAME))
	{
		if (flags & TOPVF)
		{
			vcblk(df)->v_cursx = pb->cd_crow;
			vcblk(df)->v_cursy = pb->cd_ccol;
			if ( (unit == BITMAP) && !(vcblk(df)->v_mode & M_OFFCURSOR) )
				vpos_cursor(vcblk(df)->v_cursx,vcblk(df)->v_cursy);
		}
		if (pframe)
		{
			nodisp();
			ranupd[unit]++;
			if(ranupd[unit] == 1)
				doasr(c_update, 0L, (LONG)unit, 200);
			okdisp();
		}

	}

	if ( flags & DPF_UADDR )
		unmapu();

	return( E_SUCCESS );
}
)
		unmapu();

	return( E_SUCCESS );
}


		unmapu();

	return( E_SUCCESS );
}
