/*======================================================================*
 *   Version 2.2        Console Driver
 *			C_WRITE function
 *
 *   VERSION	DATE      BY    CHANGE/COMMENTS
 *   =======	====	  ==	===============
 *   2.2	2/18/86   MA	Fixed calls to vbfill() that had reversed
 *				parameters, in the M_WRAP code.
 *   2.1	2/13/86	  MA	Took out Brian's optimization in dirtyfig() 
 *				because it caused data to be lost writing
 *				to non-full-screen virtual consoles.
 *   2.0	1/27/86	  DR-K	various fixes learned in other drivers
 *   1.13	10/1/85   MA	Fixed bugs in INSERT LINE and DELETE LINE
 *				that caused DREDIX to crash.
 *   1.1	11/8/85	  DRK	Fixed wraparound bugs that crashed system.
 *======================================================================*/

#include "portab.h"
#include "io.h"
#include "system.h"
#include "vcdrv.h"
#include "vchar.h"
#include "vmconibm.h"
#include "vterm.h"

EXTERN	FRAME	v10fr;
EXTERN	VCBLK	*pcb[];
EXTERN	BYTE	sd_units[];
EXTERN	DH	*sd_hdr[];
EXTERN	CTE	contab[];

EXTERN  BYTE	ibmtovm[];

EXTERN	BYTE	cd_units[];	/* conman's unitno indexed by sub-driver's */

EXTERN	LONG	c_mxid[];
GLOBAL	CDWRITE	c_wrchn[MAXCDUNIT];	/* stores parm block for re-entering */
EXTERN	LONG	c_rmflg[];

EXTERN  DH	c_dh;

EXTERN  LONG    saddr();

EXTERN  VOID	vbcopy();
EXTERN	VOID	vbfill();

EXTERN	VOID	off_pcursor();
EXTERN	VOID	on_pcursor();

VOID	c_rewrite();	/* forward reference */

/*
 *	NOTES:
 *
 *	When the consoles are reordered, the Console
 *	Resource Manager will copy the updated physical image to the
 *	virtual frame.
 */

LONG	c_write( pb )
CDWRITE	*pb;
{
	REG VCBLK	*cid;	/* flag and Virtual frame ptr */
	REG WORD	ch16;
	REG BYTE	unit;
	REG WORD	pframe;
	WORD		sdop;
	WORD		memop;
	WORD		*buff;
	WORD		bcnt, temp;
	WORD		dp, sp;
	REG BYTE	*d;
	DRECT		*dirt;		/* for returning the dirty area */
	WORD		cnt;
	WORD		index;
	LONG		e;
	LONG		retcode;
	DH		*t;

	unit = pb->cdw_unit;
	retcode = 0L;
	t = sd_hdr[unit];
	pframe = FALSE;
	sdop = FALSE;
	memop = FALSE;
	cid = vcblk(pb->cdw_vcid);

	/*
	 *	Check to see if we're doing optimized full screen
	 *	virtual on top writes...
	 */

	if ( pb->cdw_flags & CW_PWFLG )
	{
		if ( unit == BITMAP )
			memop = TRUE;
		else 
		{
			sdop = TRUE;
			/*
			 *	If we are not re-entering via c_rewrite()
			 *	and if asrmx returned an evnum. then we need
			 *	to wait for the sync block
			 *	else we already own the sync and can run
			 *	if we can't have the sync and
			 */
	
			if( (pb != &c_wrchn[unit]) && (e = asrmx(c_mxid[unit])) )
			{  
	
				if ( pb != &c_wrchn[unit] )
				{
					/*
					 *	This is a new call here
					 *	because the pb address is not
					 *	our save area address, so
					 *	copy all info into safe area.
					 */
	
					bcopy(pb,&c_wrchn[unit],sizeof(CDWRITE) );
					pb = &c_wrchn[unit];
				}
				/* schedule to restart when we have mx region */
				nextasr(e,c_rewrite,e,pb,200);
				/* tell CRM that we'll do this write eventually */
				return( flagevent(c_rmflg[unit], 0L) );
			}
		}
		cid = pcb[unit];
		pframe = TRUE;

		/* we copy the virtual console's state info to the physical's block */
		cid->v_att = vcblk(pb->cdw_vcid)->v_att;
		cid->v_mode = vcblk(pb->cdw_vcid)->v_mode;
		cid->v_tempx = vcblk(pb->cdw_vcid)->v_tempx;
		cid->v_tempy = vcblk(pb->cdw_vcid)->v_tempy;
		cid->v_escflg = vcblk(pb->cdw_vcid)->v_escflg;
	}

	cid->v_currow = pb->cdw_row;
	cid->v_curcol = pb->cdw_col;
	dirt = (DRECT *) pb->cdw_dirty;
	dirt->nrow = dirt->ncol = 0;		/* nothing yet */
	dirt->row = cid->v_currow;
	dirt->col = cid->v_curcol;

	buff = pb->cdw_buffer;
	if( pb->cdw_flags & DPF_UADDR )
	{
		mapu(pb->cdw_pd);
		buff = (WORD *)saddr(buff);
	}

	/* convert BYTE count of buffer to number of 16 bit characters */
	bcnt = pb->cdw_bufsiz / 2;

	while( bcnt-- )
	{
		ch16 = *buff++;

		if ( (ch16 & 0xFF00) || (ch16 < BLANK) )		/* special character */
		{
			switch ( (BYTE) (ch16 >> 12) )	/* special function char maybe */
			{
			case OC_ROWGOTO:
				if ((ch16 & 0x0FFF) < frame(cid)->fr_nrow)
				{
					if(sdop)
						 (*t->dh_special)(sd_units[unit],TGOTO,cid,(LONG)ch16&0x0FFF,(LONG)cid->v_curcol);
					cid->v_currow = ch16 & 0xFFF;
				}
				continue;

			case OC_COLGOTO:
				if ( (ch16 & 0x0FFF) < frame(cid)->fr_ncol )
				{
					if(sdop)
						(*t->dh_special)(sd_units[unit],TGOTO,cid,(LONG)cid->v_currow,(LONG)ch16&0x0FFF);
					cid->v_curcol = ch16 & 0xFFF;
				}
				continue;

			case OC_COLOR:
				switch ( ch16 & 0x0F00 )
				{
				case 0:		/* set foreground color */
					cid->v_att = (cid->v_att & (BYTE)0xF0) | (BYTE)(ch16 & 0x0F);
					continue;
				case 0x0100:	/* set background color */
					cid->v_att = (cid->v_att & (BYTE)0x0F) | ((BYTE)(ch16 & 0x0F) << 4);
					continue;
				}

			case OC_EDITFN:
				if(sdop)	/* send it to sub-driver */
					(*t->dh_write)(sd_units[unit],cid,ch16);
				switch(ch16 & 0xFF)
				{
				case OC_INDEXREV:   /* reverse lf needs more work */
					/* fall through */
				case OC_UPCURSOR:
					if (cid->v_currow > 0) --cid->v_currow;
					continue;
				case OC_DOWNCURSOR:
					if (++cid->v_currow == frame(cid)->fr_nrow) --cid->v_currow;
					continue;
				case OC_RIGHTCURSOR:
					if (++cid->v_curcol == frame(cid)->fr_ncol) --cid->v_curcol;
					continue;
				case OC_LEFTCURSOR:
					if (cid->v_curcol > 0) --cid->v_curcol;
					continue;
				case OC_CLEAR:
					cid->v_currow = cid->v_curcol = 0;	/* fall thru */
				case OC_EEOP:
					eeop(cid,cid->v_curcol,cid->v_currow,memop);
					if( !pframe )
					{
						dirtyfig(dirt,cid->v_curcol,cid->v_currow);
						dirtyfig(dirt,frame(cid)->fr_ncol-1,frame(cid)->fr_nrow-1);
					}
					continue;
				case OC_HOME:
					cid->v_currow = cid->v_curcol = 0;
					continue;
				case OC_EEL:
					cid->v_curcol = 0;	/* fall thru */
				case OC_EEOL:
					eeol(cid,cid->v_curcol,cid->v_currow,memop);
					if( !pframe )
					{
						dirtyfig(dirt,cid->v_curcol,cid->v_currow);
						dirtyfig(dirt,frame(cid)->fr_ncol-1,cid->v_currow);
					}
					continue;
				case OC_DOCHINSERT:
					cid->v_mode |= M_INSERTC;
					continue;
				case OC_NOCHINSERT:
					cid->v_mode &= ~M_INSERTC;
					continue;
				case OC_SAVECURS:
					cid->v_tempx = cid->v_currow;
					cid->v_tempy = cid->v_curcol;
					continue;
				case OC_RESTORECURS:
					cid->v_currow = cid->v_tempx;
					cid->v_curcol = cid->v_tempy;
					continue;
				case OC_ENABLECURS:
					cid->v_mode &= ~M_OFFCURSOR;
					if(pb->cdw_flags & TOPVF) on_pcursor(unit);
					continue;
				case OC_DISABLECURS:
					cid->v_mode |= M_OFFCURSOR;
					if(pb->cdw_flags & TOPVF) off_pcursor(unit);
					continue;
				case OC_CHDELETE:	/* delete this char */
					/* First we must calculate position */
					/* of cursor on FRAME */
					index = position(cid,cid->v_currow,cid->v_curcol);
					d = index + frame(cid)->fr_pl[PL_CHAR];
					vbcopy(d+1,d,frame(cid)->fr_ncol - cid->v_curcol - 1);
					*(d + (frame(cid)->fr_ncol - cid->v_curcol - 1)) = BLANK;
					d = index + frame(cid)->fr_pl[PL_ATTR];
					vbcopy(d+1,d,frame(cid)->fr_ncol - cid->v_curcol - 1);
					*(d + (frame(cid)->fr_ncol - cid->v_curcol - 1)) = cid->v_att;
					if (!pframe)
					{
						/* mark the rest of the line as dirty */
						dirtyfig(dirt,cid->v_curcol,cid->v_currow);
						dirtyfig(dirt,frame(cid)->fr_ncol-1,cid->v_currow);
					}
					else
					{
						if( memop )
						{
							WORD	*wp;

							wp = ((WORD *)v10fr.fr_pl[PL_ATTR])
								+ (cid->v_currow * v10fr.fr_ncol + cid->v_curcol);
							vwcopy(wp+1,wp,frame(cid)->fr_ncol - cid->v_curcol - 1);
							*(wp + (frame(cid)->fr_ncol - cid->v_curcol - 1))
								= BLANK | (ibmtovm[cid->v_att&0xFF] << 8);
						}
					}
					continue;
				case OC_INTENSIFY:
					cid->v_att |= IBMINTENSE;
					continue;
				case OC_NOINTENSIFY:
					cid->v_att &= ~IBMINTENSE;
					continue;
				case OC_BLINK:
					cid->v_att |= IBMBLINK;
					continue;
				case OC_NOBLINK:
					cid->v_att &= ~IBMBLINK;
					continue;
				case OC_REVERSE:
					if (!(cid->v_mode & M_REVVIDM))	/* enter reverse video */
					{	
						cid->v_att = (BYTE)((cid->v_att & (BYTE)0x88) | ((cid->v_att & 0x70) >> 4)
							| ((cid->v_att & 0x07) << 4));
						cid->v_mode |= M_REVVIDM;
					}
					continue;
				case OC_NOREVERSE:
					if (cid->v_mode & M_REVVIDM)	/* exit reverse video */
					{
						cid->v_att = (BYTE)((cid->v_att & (BYTE)0x88) | ((cid->v_att & 0x70) >> 4)
							| ((cid->v_att & 0x07) << 4));
						cid->v_mode &= ~M_REVVIDM;
					}
					continue;
				case OC_WRAP:
					cid->v_mode |= M_WRAP; 
					continue;
				case OC_NOWRAP:
					cid->v_mode &= ~M_WRAP; 
					continue;
				case OC_EBOP:	/* delete to beginning of display */
					for (temp = 0; temp < cid->v_currow; temp++)
						eeol(cid,0,temp,memop);
					if (!pframe) dirtyfig(dirt,frame(cid)->fr_ncol-1,0);
					/* fall thru */
				case OC_EBOL:	/* erase to beginning of line */
					temp = position(cid,cid->v_currow,0);
					vbfill(frame(cid)->fr_pl[PL_CHAR]+temp,cid->v_curcol,BLANK);
					vbfill(frame(cid)->fr_pl[PL_ATTR]+temp,cid->v_curcol,cid->v_att);
					if (!pframe) dirtyfig(dirt,0,cid->v_currow);
					else
					{
						vwfill(v10fr.fr_pl[PL_ATTR],cid->v_curcol,BLANK|(ibmtovm[cid->v_att&0xFF] << 8));
					}
					continue;
				case OC_LNINSERT:
					/* insert a blank line at cursor row */
					if (cid->v_top != 0)
					{
						if((cid->v_top + cid->v_currow) < frame(cid)->fr_nrow)
						{
							/* do it the hard way */
							dp = frame(cid)->fr_ncol;
							cnt = (cid->v_top - 1) * frame(cid)->fr_ncol;
							vbcopy(frame(cid)->fr_pl[PL_CHAR],frame(cid)->fr_pl[PL_CHAR]+dp,cnt);
							vbcopy(frame(cid)->fr_pl[PL_ATTR],frame(cid)->fr_pl[PL_ATTR]+dp,cnt);
							sp = (frame(cid)->fr_nrow - 1) * frame(cid)->fr_ncol;
							cnt = frame(cid)->fr_ncol;
							vbcopy(frame(cid)->fr_pl[PL_CHAR]+sp,frame(cid)->fr_pl[PL_CHAR],cnt);
							vbcopy(frame(cid)->fr_pl[PL_ATTR]+sp,frame(cid)->fr_pl[PL_ATTR],cnt);
							sp = (cid->v_currow + cid->v_top) * frame(cid)->fr_ncol;
							dp = sp + frame(cid)->fr_ncol;
							cnt = (frame(cid)->fr_nrow - (cid->v_top + cid->v_currow + 1)) * frame(cid)->fr_ncol;
						}
						else
						{
							sp = (cid->v_top + cid->v_currow) % frame(cid)->fr_nrow;
							cnt = ((cid->v_top - sp) - 1) * frame(cid)->fr_ncol;
							sp *= frame(cid)->fr_ncol;
							dp = sp + frame(cid)->fr_ncol;
						}
					}
					else
					{
						sp = cid->v_currow * frame(cid)->fr_ncol;
						dp = sp + frame(cid)->fr_ncol;
						cnt = (frame(cid)->fr_nrow - cid->v_currow -1) * frame(cid)->fr_ncol;
					}
					vbcopy(frame(cid)->fr_pl[PL_CHAR]+sp,frame(cid)->fr_pl[PL_CHAR]+dp,cnt);
					vbcopy(frame(cid)->fr_pl[PL_ATTR]+sp,frame(cid)->fr_pl[PL_ATTR]+dp,cnt);
					if (memop)
					{
					   sp = cid->v_currow * v10fr.fr_ncol * 2;
					   dp = sp + (v10fr.fr_ncol * 2);
					   cnt = ((v10fr.fr_nrow - cid->v_currow) - 1) * v10fr.fr_ncol;
					   vwcopy(v10fr.fr_pl[PL_ATTR]+sp,v10fr.fr_pl[PL_ATTR]+dp,cnt);
					}
					eeol(cid,0,cid->v_currow,memop);
					cid->v_curcol = 0;
					if (pframe)			/* VME/10 FRAME??? */
						continue;
					dirtyfig(dirt,0,cid->v_currow);
					dirtyfig(dirt,frame(cid)->fr_ncol-1,frame(cid)->fr_nrow-1);
					continue;
				case OC_LNDELETE:   /* delete the line at cursor row */
					if (cid->v_top != 0)
					{
						if((dp = cid->v_top + cid->v_currow) < frame(cid)->fr_nrow)
						{
							/* do it the hard way */
							cnt = (frame(cid)->fr_nrow - (dp+1)) * frame(cid)->fr_ncol;
							dp *= frame(cid)->fr_ncol;
							sp = dp + frame(cid)->fr_ncol;
							if (cnt != frame(cid)->fr_ncol)
							{	/* silly last line copy */
								vbcopy(frame(cid)->fr_pl[PL_CHAR]+sp,frame(cid)->fr_pl[PL_CHAR]+dp,cnt);
								vbcopy(frame(cid)->fr_pl[PL_ATTR]+sp,frame(cid)->fr_pl[PL_ATTR]+dp,cnt);
							}
							dp = (frame(cid)->fr_nrow - 1) * frame(cid)->fr_ncol;
							cnt = frame(cid)->fr_ncol;
							vbcopy(frame(cid)->fr_pl[PL_CHAR],frame(cid)->fr_pl[PL_CHAR]+dp,cnt);
							vbcopy(frame(cid)->fr_pl[PL_ATTR],frame(cid)->fr_pl[PL_ATTR]+dp,cnt);
							dp = 0;
							sp = frame(cid)->fr_ncol;
							cnt = (cid->v_top - 1) * frame(cid)->fr_ncol;
						}
						else
						{
							dp = (dp % frame(cid)->fr_nrow) * frame(cid)->fr_ncol;
							cnt = ((cid->v_top - 1) * frame(cid)->fr_ncol) - dp;
							sp = dp + frame(cid)->fr_ncol;
						}
					}
					else
					{
						dp = cid->v_currow * frame(cid)->fr_ncol;
						sp = dp + frame(cid)->fr_ncol;
						cnt = ((frame(cid)->fr_nrow - cid->v_currow) - 1) * frame(cid)->fr_ncol;
					}
					vbcopy(frame(cid)->fr_pl[PL_CHAR]+sp,frame(cid)->fr_pl[PL_CHAR]+dp,cnt);
					vbcopy(frame(cid)->fr_pl[PL_ATTR]+sp,frame(cid)->fr_pl[PL_ATTR]+dp,cnt);
					if (memop)
					{
					   dp = cid->v_currow * v10fr.fr_ncol * 2;
					   sp = dp + (v10fr.fr_ncol * 2);
					   cnt = ((v10fr.fr_nrow - cid->v_currow) - 1) * v10fr.fr_ncol;
					   vwcopy(v10fr.fr_pl[PL_ATTR]+sp,v10fr.fr_pl[PL_ATTR]+dp,
							  cnt);
					}
					eeol(cid,0,frame(cid)->fr_nrow-1,memop);
					cid->v_curcol = 0;
					if (pframe) continue;
					dirtyfig(dirt,0,cid->v_currow);
					dirtyfig(dirt,frame(cid)->fr_ncol-1,frame(cid)->fr_nrow-1);
					continue;
				}
				break;
			case 0:		/* ASCII character */
				switch (ch16)
				{
				case NULLC:	  /* nulls filter to spaces */
					ch16 = BLANK;
					break;
				case BELL:
					beepbell(unit);
					continue;
				case BACKSPACE:
					if (cid->v_curcol)
					{
						if(sdop)
							(*t->dh_write)(sd_units[unit],cid,BACKSPACE);
						cid->v_curcol--;
					}
					continue;
				case TAB:
					if(sdop)
						(*t->dh_write)(sd_units[unit],cid,TAB);
					do
					{  
						cid->v_curcol++;
					} while(cid->v_curcol & 7);
					continue;
				case RETURN:
					if(sdop)
						(*t->dh_write)(sd_units[unit],cid,RETURN);
					cid->v_curcol = 0;
					continue;

				case LINEFEED:
					if (++cid->v_currow >= frame(cid)->fr_nrow)
					{
						/* scroll the whole vc frame */
						/* optimize by moving v_top */
						cid->v_currow = frame(cid)->fr_nrow -1;
						temp = cid->v_top * frame(cid)->fr_ncol;
						cid->v_top = (cid->v_top+1) % frame(cid)->fr_nrow;

						/* blank the last line */
						vbfill((frame(cid)->fr_pl[PL_CHAR]+temp),frame(cid)->fr_ncol,BLANK);
						vbfill((frame(cid)->fr_pl[PL_ATTR]+temp),frame(cid)->fr_ncol,cid->v_att);

						if (pframe)
						{
							if( unit == BITMAP )
							{
								WORD	*d, *s;

								d = (WORD *)v10fr.fr_pl[PL_ATTR];
								s = d + v10fr.fr_ncol;
								vwcopy(s,d,(v10fr.fr_nrow-1)*v10fr.fr_ncol);
								vwfill(d + (cid->v_currow * v10fr.fr_ncol)
									,v10fr.fr_ncol,BLANK|(ibmtovm[cid->v_att&0xFF]<<8));
							}
						}
						else
						{
							dirt->row = dirt->col = 0;
							dirt->nrow = frame(cid)->fr_nrow;
							dirt->ncol = frame(cid)->fr_ncol;
						}
					}
					else
					{
						if( !pframe )	/* Mark line dirty if dirty chars */
							if (dirt->ncol == 0) dirt->row++;
							else dirt->nrow++;
					}
					if( sdop )
						(*t->dh_write)(sd_units[unit],cid,LINEFEED);
					continue;

				case ESCAPE:	/* NOT IMPLEMENTED YET */
					if(sdop)
						(*t->dh_write)(sd_units[unit],cid,ESCAPE);
					continue;

				default:
					break;
				} /* end of case control: */
			} /* end of switch(special character) */
		} /* End of if(special or control character) */

		/* if we're now beyond the limits of */
		/* this screen, then wraparound to   */
		/* next line if mode is set. Else  */
		/* leave doing nothing.		     */

		if (cid->v_curcol >= frame(cid)->fr_ncol)
		{
			if (cid->v_mode & M_WRAP)
			{
				cid->v_curcol = 0;
				if (++cid->v_currow == frame(cid)->fr_nrow)
				{
					/* scroll the whole vc frame */
					cid->v_currow--;
					temp = cid->v_top * frame(cid)->fr_ncol;
					cid->v_top = (cid->v_top+1) % frame(cid)->fr_nrow;
					/* blank the last line */
					vbfill((frame(cid)->fr_pl[PL_CHAR]+temp),frame(cid)->fr_ncol,BLANK);
					vbfill((frame(cid)->fr_pl[PL_ATTR]+temp),frame(cid)->fr_ncol,cid->v_att);

					if (pframe)
					{
						if( unit == BITMAP )
						{
							WORD	*d, *s;

							d = (WORD *)v10fr.fr_pl[PL_ATTR];
							s = d + v10fr.fr_ncol;
							vwcopy(s,d,(v10fr.fr_nrow-1)*v10fr.fr_ncol);
							vwfill(d+v10fr.fr_ncol*(v10fr.fr_nrow-1),
								v10fr.fr_ncol,BLANK|(ibmtovm[cid->v_att&0xFF]<<8));
						}
					}
					else
					{
						dirt->row = dirt->col = 0;
						dirt->nrow = frame(cid)->fr_nrow;
						dirt->ncol = frame(cid)->fr_ncol;
					}
				}
			}
			else
			{
				cid->v_curcol = frame(cid)->fr_ncol - 1;
				continue;
			}
		}

		/* update the physical or virtual frame */
		index = position(cid,cid->v_currow,cid->v_curcol);

		/*
		 *	If we are insert character mode, we copy up
		 *	the characters on the row at the current
		 *	cursor position before putting the character out.
		 */

		if (cid->v_mode & M_INSERTC)
		{
			/* copy up chars */
			d = frame(cid)->fr_pl[PL_CHAR] + index;
			vbcopy(d,d+1,(frame(cid)->fr_ncol - cid->v_curcol - 1));
			/* copy up attrs */
			d = frame(cid)->fr_pl[PL_ATTR] + index;
			vbcopy(d,d+1,(frame(cid)->fr_ncol - cid->v_curcol - 1));
			if (pframe)
			{
				REG	WORD	*wp;
				
				if( memop )
				{
					wp = ((WORD *) v10fr.fr_pl[PL_ATTR])
						+ cid->v_currow * v10fr.fr_ncol + cid->v_curcol;
					vwcopy(wp,wp+1,(frame(cid)->fr_ncol - cid->v_curcol - 1));
				}
			}
			else
			{
				/* mark the rest of the line as dirty */
				dirtyfig(dirt,cid->v_curcol,cid->v_currow);
				dirtyfig(dirt,frame(cid)->fr_ncol-1,cid->v_currow);
			}
		}

		/* put the character out and then the attribute */
		*(frame(cid)->fr_pl[PL_CHAR] + index) = (BYTE)ch16;
		*(frame(cid)->fr_pl[PL_ATTR] + index) = cid->v_att;
		if (pframe)
		{
			if(sdop)
				(*t->dh_write)(sd_units[unit],cid,ch16);
			else
				*((WORD *)(v10fr.fr_pl[PL_ATTR]
					+ ((cid->v_currow * v10fr.fr_ncol)
					+ cid->v_curcol) * 2))
					= (BYTE)ch16 | (ibmtovm[cid->v_att&0xFF] << 8);
		}
		else
			dirtyfig(dirt,cid->v_curcol,cid->v_currow);
		if ((++cid->v_curcol) >= frame(cid)->fr_ncol)
			cid->v_curcol = frame(cid)->fr_ncol - 1;

	} /* end while characters left */

	/* release the sync block */
	if (sdop) mxrel(c_mxid[unit]);

	if ( (pframe) && (!(sdop)) && ((cid->v_mode & M_OFFCURSOR) == 0) )
		vpos_cursor(cid->v_currow,cid->v_curcol);

	/* pass back the updated current cursor postions */
	dirt->currow = cid->v_currow;
	dirt->curcol = cid->v_curcol;
	if( pframe )	/* restore state info to virtual */
	{	
		vcblk(pb->cdw_vcid)->v_att = cid->v_att;
		vcblk(pb->cdw_vcid)->v_mode = cid->v_mode;
		vcblk(pb->cdw_vcid)->v_tempx = cid->v_tempx;
		vcblk(pb->cdw_vcid)->v_tempy = cid->v_tempy;
		vcblk(pb->cdw_vcid)->v_escflg = cid->v_escflg;
	}

	if ( pb->cdw_flags & DPF_UADDR )  unmapu();
	return(retcode);		/* return event (if there was one... */
} /* end of c_write */


/* Erase to End of Line */

eeol(vcid,curcol,currow,memop)
REG VCBLK	*vcid;
UWORD	curcol,currow;
WORD	memop;
{
	WORD	temp,cnt;

	temp = position(vcid,currow,curcol);
	cnt = frame(vcid)->fr_ncol - curcol;
	vbfill((frame(vcid)->fr_pl[PL_CHAR] + temp),cnt,BLANK);
	vbfill((frame(vcid)->fr_pl[PL_ATTR] + temp),cnt,vcid->v_att);
	/* if this is the memory mapped console, we need to do that also */
	if( memop )	/* need to reflect clear in VME/10 frame */
		vwfill(v10fr.fr_pl[PL_ATTR]+((currow*v10fr.fr_ncol+curcol)*2),
			cnt, BLANK | (ibmtovm[vcid->v_att&0xFF] << 8));
}

/* Erase to End of Page */

eeop(vcid,curcol,currow,memop)
REG VCBLK	*vcid;
UWORD	curcol,currow;
WORD	memop;
{
	REG UWORD	r;

	eeol(vcid,curcol,r=currow,memop);
	while ( frame(vcid)->fr_nrow != ++r )
		eeol(vcid,0,r,memop);
}

dirtyfig(r,curcol,currow)
REG DRECT	*r;
REG UWORD	curcol, currow;
{
	REG UWORD maxcol,maxrow;

	maxcol = max( r->col + r->ncol, curcol + 1);
	r->col = min( curcol, r->col );
	r->ncol = maxcol - r->col;
	maxrow = max( r->row + r->nrow, currow + 1);
	r->row = min( currow, r->row );
	r->nrow = maxrow - r->row;
}

/* cleans up events and restarts c_write() */
VOID c_rewrite(e,pb)
LONG	e;
CDWRITE	*pb;
{
	s_return( e );
	c_write( pb );
	/* c_write will return to here, we flagset signal the CRM */
	flagset( c_rmflg[(pb->cdw_unit)], *(c_dh.dh_curpd), 0L);
}
                                                         
 */
	flagset( c_rmflg[(pb->cdw_unit)], *(c_dh.dh_curpd), 0L);
}
                                                         
