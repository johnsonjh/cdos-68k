/*      SERIAL.C */
/************************************************************************
*       Title: Serial driver						*
*                                                                       *
*       Discription: here is a Serial driver which uses SUB-DRIVERS	*
*                                                                       *
*	This Serial driver purpose is to reside between the Miscellany	*
*	Manager and the Port driver. It has limited usefulness, only	*
*	for doing the Get/Set Baud rate of a serial Port, and some odd	*
*	direct port output.						*
*									*
*               Revision History                                        *
*       Revision  Date    Author        Comments                        *
*       1.0       11/16/84 DR-K         first-cut                       *
*	1.1	  12/11/84 KSO		added tab expansion		*
*	1.2	  12/12/84 KSO		translate form feed to clear	*
*					screen for ADM3			*
*	1.3	  12/20/84 KSO		declare salloc as returning	*
*					a pointer			*
*	1.4	  1/13/85  KSO		include system.h instead of	*
*					sys.h				*
*	1.5	  4/10/85  DR-K		s_special pass-thru to pdrv	*
*	1.6	  4/22/85  DR-K		ser_rd does not return until deQ*
*					  has given a character		*
*       1.7	  4/23/85  DR-K		s_enQ sends XOFF at HIWATER	*
*					s_deQ sends XON at LOWATER	*
*	1.8	  6/27/85  DR-K		decrement buff before pb-evpend	*
*					 storage, remove cli/sti in deQ	*
*					 return any flagset() errors	*
*	1.9	  7/29/85  DR-K		move mapu() out of ISR s_enQ()	*
*					 to ASR ser_red()		*
*	2.0	  8/02/85  DR-K		s_flush checks if evpend then	*
*					 flagset.  s_init takes LONG.	*
*					 sdrvcol now sdrvcol[unit]	*
*       2.1	  8/05/85  DR-K		fix HIWATER control S send	*
*	3.0	  3/14/86  DR-K		add delimited/edited reads	*
*************************************************************************/
/*      INCLUDES                                                        */

#include "portab.h"
#include "flags.h"
#include "char.h"
#include "io.h"
#include "system.h"
#include "sdrv.h"

/************************************************************************/
/*      PROGRAM CONTROLS                                                */
#define	COMPU	TRUE	/* is this a CompuPro driver			*/
/************************************************************************/

EXTERN	LONG	sti();
EXTERN	LONG	cli();

/************************************************************************/
/* forward referenced functions                                         */
/************************************************************************/
GLOBAL  LONG
        s_init(),
        s_subdrvr(),
        s_uninit(),
        s_select(),
        s_flush(),
        ser_rd(),
        ser_wrt(),
        ser_get(),
        ser_set(),
        s_enQ(),
        ser_special(),
	ser_rdel();
BYTE	s_deQ();
VOID	ser_red();

/************************************************************************/
/* local initializations                                                */
/************************************************************************/

#define MAXsUNIT 8
#define FLAGVAL  0x10	/* no syncs needed, 8-bit, has delimited read func */

/************************************************************************/
/*      Driver header                                                   */
/*  This header must be the first structure in the data segment         */
/*   so, don't put any data generating code before this section         */
/************************************************************************/

DH  s_dh =
{       DVR_SER,MAXsUNIT,FLAGVAL,
        s_init,
        s_subdrvr,
        s_uninit,
        s_select,
        s_flush,
        ser_rd,
        ser_wrt,
        ser_get,
        ser_set,
        ser_special,
        0L,0L,0L,0L,0L
};

GLOBAL	LONG	RdsFlag[MAXsUNIT] ;
GLOBAL	LONG	WrsFlag[MAXsUNIT] ;

typedef VOID	(*PINPTR)();

PINPTR	skb_pin[MAXsUNIT];
PHYSBLK	*sdev[MAXsUNIT];	/* ptr to Queue and control structs	*/
LONG	scon[MAXsUNIT];		/* stores Physical Console ID for R-M	*/
WORD    ckb_intrp();		/* froward ref the interupt routine	*/
BYTE	ser_unit[MAXsUNIT];	/* misman's unitno indexed by sub-driver's */
BYTE    pt_unit[MAXsUNIT];	/*sub-driver's unitno indexed by drvr_unitno */
DH      *pt_hdr[MAXsUNIT];	/* stores drvr headers of sub-drivers	*/

#define	TABLEN	8
GLOBAL	WORD	sdrvcol[MAXsUNIT];

/************************************************************************/
/*      s_init                                                          */
/************************************************************************/

LONG    s_init(unitno)
        LONG	unitno;
{
	BYTE	unit;

	unit = (BYTE)unitno;
	sdrvcol[unit] = 0;		/* Start at column 0	*/
	RdsFlag[unit] = FLAGGET();
	WrsFlag[unit] = FLAGGET();

        if (unit > MAXsUNIT) return( E_IllUnitno );
        return( (((LONG)DVR_PORT << 16) | (LONG)DVR_SER) );
                /* we need a PORT subdriver */
}

/************************************************************************/
/*      s_subdrvr                                                       */
/************************************************************************/

LONG    s_subdrvr(pb)

        DPB     *pb;

{
	PHYSBLK	*d;

        ser_unit[pb->dp_option] = pb->dp_unitno ;       /* drvr unitno */
        pt_hdr[pb->dp_unitno] = (DH *) pb->dp_swi ;     /* header address */
        pt_unit[pb->dp_unitno] = pb->dp_option ;       /* sub-drvr unitno */

	d = sdev[pb->dp_option] = (PHYSBLK *) salloc( (LONG)sizeof(PHYSBLK)) ;
	d->Qrear = d->Qfront = d->evpend = d->xoffed = d->Qlen = 0;

        return(E_SUCCESS);
}

/************************************************************************/
/*      s_uninit                                                        */
/************************************************************************/

LONG    s_uninit(unitno)
        LONG	unitno;
{
	LONG	r;
        DPB     pb;
	BYTE	unit;

	unit = (BYTE)unitno;
	r = FLAGREL( RdsFlag[unit] );
	r = FLAGREL( WrsFlag[unit] );

	sfree( sdev[pt_unit[unit]] );

    /* call the sub-drives flush(this unitno) */
        pb.dp_unitno = pt_unit[unit];
        return (*pt_hdr[unit]->dh_flush)(&pb);
}

/************************************************************************/
/*      s_select                                                        */
/************************************************************************/

LONG    s_select(pb)

        CDSELECT	*pb ;
{
        BYTE    unitno,sunit;
        LONG    r ;

        unitno = pb->unitno ;
	sunit = pt_unit[unitno];
        skb_pin[unitno] = (PINPTR) pb->kbd_pin ;
	scon[unitno] = pb->PConId ;

        /* call the sub-drivers select routine */
        pb->unitno = sunit ;
        pb->kbd_pin = (LONG) s_enQ ;
	pb->PConId = (LONG) unitno ;
        return( (*pt_hdr[unitno]->dh_select) (pb) );
}

/************************************************************************/
/*      s_flush                                                         */
/************************************************************************/

LONG    s_flush(pb)

        DPB     *pb;

{
        LONG    r;
	BYTE	unit,sunit;
	PHYSBLK	*pd;

	unit = pb->dp_unitno ;
				/* call the sub-drives flush(this unitno) */
	pb->dp_unitno = sunit = pt_unit[unit];
	if (!(pb->dp_option & 1))			/* on last close */
	        r = (*pt_hdr[unit]->dh_flush)(pb);

	pd = sdev[sunit] ;
/***blotz we may not to check this proc ID for validity ****/
	if ( pd->evpend ) {		/* if there is an event awaiting */
		FLAGSET( pd->flagno,pd->fpdaddr,0L );	/*signal done*/
		pd->evpend = 0;
	    }
        return( r );
}

/************************************************************************/
/*      ser_rd - unbuffer up to bufsiz characters 			*/
/************************************************************************/
LONG ser_rd( pb )
	DPB	*pb ;
{
	BYTE		unit  ;
	LONG		emask ;
	REG BYTE	*buff ;	/* ptr to users buffer			*/
	REG LONG	cnt ;	/* nbr of chars left to transmit	*/
	ERROR		r ;	/* holder for error return codes	*/
	PHYSBLK		*d ;	/* event data holder			*/
	LONG	nbytes = 0 ;	/* number of chars returned */

	unit = pb->dp_unitno ;
	emask = FLAGEVENT( RdsFlag[unit],pb->dp_swi );

	buff = pb->dp_buffer ;
	if ( pb->dp_flags & DPF_UADDR )
		buff = (BYTE *) saddr(buff) ;
	pb->dp_offset = 0L;

	if (pb->dp_flags & A_DELIM)
	   nbytes = ser_rdel(pb);
	 else
	   for( cnt = pb->dp_bufsiz ; ((cnt--) && (*buff++ = s_deQ(unit))) ; )
				nbytes++ ;

	if (nbytes)
	    r = FLAGSET ( RdsFlag[unit], *((BYTE **)(s_dh.dh_rlr)), nbytes );
	else 
	    {
		d = sdev[pt_unit[unit]];
		_bmove(pb,&d->pb,sizeof(DPB));
		d->bpdaddr = pb->dp_pdaddr;
		d->flagno = RdsFlag[unit];
		d->fpdaddr = (LONG)(*((BYTE **)(s_dh.dh_rlr)));
		d->evpend = 1;		/* set the semiphore */
	    }
	return(emask);
}

/************************************************************************/
/*      ser_wrt                                                         */
/*      today it just writes the buffer full of characters to the       */
/*	serial port							*/
/************************************************************************/
LONG	ser_wrt( pb )
	DPB	*pb;
{
	WORD	len,j,bsiz,cnt,sdunit ;
	BYTE	*buff,unit ;
	LONG	evnum,i ;

	unit = pb->dp_unitno;
	sdunit = (WORD) pt_unit[unit];
	buff = pb->dp_buffer ;
	if ( pb->dp_flags & DPF_UADDR )
		buff = (BYTE *) saddr(buff) ;

	for( cnt=pb->dp_bufsiz; cnt--; buff++ )
	{
	    if( *buff == '\n' ) sdrvcol[unit] = 0;	/* Reset column	*/
	    if( *buff == '\t' )			/* Expand tabs	*/
	    {
		if( len = (TABLEN - (sdrvcol[unit] % TABLEN)) )
		    for( j=0; j < len; j++ )
			(*pt_hdr[unit]->dh_write)(sdunit,' ');
		sdrvcol[unit] += len;
	    }
	    else
	    {
		++sdrvcol[unit];
		(*pt_hdr[unit]->dh_write)(sdunit,*buff);
	    }
	}

	evnum = FLAGEVENT( WrsFlag[unit],pb->dp_swi );
	FLAGSET( WrsFlag[unit],*((BYTE **)(s_dh.dh_rlr)),pb->dp_bufsiz );
	return( evnum );
}

/************************************************************************/
/*      ser_get - pass this operation on to the sub-driver		*/
/************************************************************************/
	 	
LONG	ser_get(pb)
	DPB	*pb;
{
	BYTE	unit ;

	unit = pb->dp_unitno ;
	pb->dp_unitno = pt_unit[unit] ;
	return( (*pt_hdr[unit]->dh_get)(pb) );
}

/************************************************************************/
/*      ser_set - pass this operation on to the sub-driver		*/
/************************************************************************/
	 	
LONG	ser_set(pb)
	DPB	*pb;
{
	BYTE	unit ;

	unit = pb->dp_unitno ;
	pb->dp_unitno = pt_unit[unit] ;
	return( (*pt_hdr[unit]->dh_set)(pb) );
}

/************************************************************************/
/*      ser_special                					*/
/************************************************************************/
	 	
LONG	ser_special(pb)
	DPB	*pb;
{
	BYTE	unit;

    if ( (pb->dp_option & 0x7F) == 0x13 )
    {			/** Call a PORT driver to Get/Set table **/
	if ( (unit = pb->dp_unitno) > MAXsUNIT ) return(E_IllUnitno);
	pb->dp_unitno = pt_unit[unit];
	return( (*pt_hdr[unit]->dh_special)(pb) );
    }
    return(ED_CON | E_IMPLEMENT);
}

/************************* end of main routines *****************************/
/************************* start of sub-routines ****************************/

BYTE s_deQ(unit)
	BYTE	unit;
{
	PHYSBLK	*pd ;

	pd = sdev[pt_unit[unit]] ;
	if( pd->Qrear == pd->Qfront ) return( 0 );
	if ( (--(pd->Qlen) < LOWATER) && (pd->xoffed) )
	    {
		pd->xoffed = 0;
		(*pt_hdr[unit]->dh_write)(pt_unit[unit],CNTRLQ);
	    }
	return( pd->Q[(pd->Qfront = ((pd->Qfront+1) % MAXOUTB))]);
}

LONG	s_enQ(ch,unitno)	/* called in ISR context by Port Driver */
	LONG	ch;		/* keyboard input interrupt via pin addr */
	LONG	unitno;
{
	PHYSBLK	*pd ;
	BYTE	unit;

	if ((ch & 0xFF) ==0) return;

	unit = (BYTE) unitno;
	pd = sdev[unit] ;
	if ( pd->evpend == 1 )		/* if there is an event awaiting */
	     {
		pd->evpend++;		/* schedule only once */
		doasr(ser_red,pd,unitno,200);
	     }
	pd->Q[(pd->Qrear = ((pd->Qrear+1) % MAXOUTB))] = (BYTE) ch ;
		/** change to & anding if mod code too long **/
	if ( pd->Qlen++ > HIWATER )
	     {
		(*pt_hdr[ser_unit[unit]]->dh_write)(unit,CNTRLS);
		pd->xoffed = 1;
	     }
	if (pd->Qrear == pd->Qfront)	/* don't enq if Queue is full */
	     {
		pd->Qrear = ((pd->Qrear-1) % MAXOUTB );
		pd->Qlen-- ;
	     }
}

VOID ser_red(pd,unit)
	PHYSBLK	*pd;
	LONG	unit;
{
	REG BYTE	*buff;
	REG LONG	cnt;
	REG LONG	nbytes = 0;
	REG DPB		*pb;

	pb = &pd->pb;
	pd->evpend = 0;
	mapu(pd->bpdaddr);	/* get access to buffer memory */

	if (pb->dp_flags & A_DELIM)
	   nbytes = ser_rdel(pb);
	 else
	   {
	     buff = pb->dp_buffer ;
	     if ( pb->dp_flags & DPF_UADDR ) buff = (BYTE *) saddr(buff) ;
	     for( cnt=pb->dp_bufsiz ; ((cnt--) && (*buff++ = s_deQ(pb->dp_unitno))) ; )
				nbytes++ ;
	   }
	unmapu();
	if (nbytes)
	    return( FLAGSET( pd->flagno,pd->fpdaddr,nbytes ) );	/*signal done*/
	  else pd->evpend = 1;			/* wait for more */
}


/************************************************************************/
/*      ser_rdel - unbuffer up to a delimiter or bufsiz characters 	*/
/*			with possible preinit`ing or editing		*/
/************************************************************************/
LONG ser_rdel( pb )
	DPB	*pb;
{
    BYTE  unit;
    UWORD bit16;
    UWORD bufsiz;
    REG UWORD ch16;
    UWORD *delim;
    LONG r;
    REG WORD i;
    WORD j,
	 more,
	 start, 	/* column position on screen of edit start	*/
	 locate,	/* current cursor column location on screen	*/
	 end;		/* column position of last char on line 	*/
    REG WORD
	 nchar, 	/* number of characters in buffer		*/
	 lchar; 	/* current edit position in buffer		*/
    WORD n8ch,		/* current 8-bit size of buffer 		*/
	 gotdel = FALSE;/* flag indicating detection of delimiter	*/
    REG UWORD *wbuff;
    BYTE  *buffer;
    UWORD pflags;
    WORD editch;
    WORD insert = FALSE;	/* flag for insert character mode */
    WORD ch8;

	unit = pb->dp_unitno;
	bit16 = pb->dp_option & 0x01;	/* should be RT_16BIT */
	pflags = pb->dp_flags;
	bufsiz = (WORD)pb->dp_bufsiz;
	delim = (UWORD *)pb->dp_parm7;
	buffer = (BYTE *)pb->dp_buffer;

	if (pb->dp_flags & DPF_UADDR)
	{
	    delim = (UWORD *)saddr(pb->dp_parm7);
	    buffer = (BYTE *)saddr(pb->dp_buffer);
	}

	lchar = n8ch = pb->dp_offset;
	nchar = end = n8ch >>1;

	locate = sdrvcol[unit];
	start = 0;
	wbuff = (UWORD *)buffer;

	if (pflags & A_PREINIT)
	{
	    pb->dp_flags &= ~A_PREINIT ;
	    more = TRUE;
	    while (more)
	    {
		if (bit16)
		    ch16 = ((UWORD *)buffer)[nchar];
		else
		    ch16 = buffer[nchar];

		if (!ch16)
		    more = FALSE;
		else
		{
		    nchar++;
		    n8ch += ((bit16) ? 2 : 1 );
		    locate += ser_echosiz(ch16);
		    rdecho_ser(unit,ch16);
		}
	    }
	    lchar = n8ch;
	    end = locate;
	}

	editch = FALSE;
	more = TRUE;
	while(more)
	{
	    editch = FALSE;

	    ch16 = (UWORD)(r = s_deQ(pb->dp_unitno));  /* Read 1 char */

		/* check for End of File here !!!!!!   */

	    if (r <= 0)
		break;		   /* need to wait for characters to come in */

			/* Check for Delimiter */

	    for (i = 1; (i <= delim[0]) && (delim[i] != ch16); i++)
				;  /* until end of list or delimiter */
	    if (i <= delim[0])
	    {			/* we have a delimiter */
		gotdel = TRUE;
		/****lchar = nchar;		** put delim at EOL *****/
		if (!(pflags & A_DELINCL))
		    break;
		more = FALSE;
	    }

	    if ( (pflags & A_EDIT) && !(gotdel) )
	    {
		editch = TRUE;
		switch(ch16)
		{
		case CH_BS : 		/* Delete previous Character */
		    if (n8ch) {		/* unless at start */
			n8ch -= ((bit16) ? 2 : 1);
			lchar--;
			nchar--;
			ser_recho(unit,CH_BS);
			ser_recho(unit,' ');	/* distroy the character */
			ser_recho(unit,CH_BS);
			}
		    break;
		case CH_CTRLX :		/* delete line */
		    for(i=0; i<n8ch ; i++)
			{
			ser_recho(unit,CH_BS);
			ser_recho(unit,' ');	/* distroy the character */
			ser_recho(unit,CH_BS);
			}
		    n8ch = lchar = nchar = 0;
		    break;
		default:
		    editch = FALSE;
		}
	    }

	    if (!editch)
	    {

		if (bit16)
		{   /****if((ch16=translate8to16(ch8,tmpbuff))==0)break;****/
		     buffer[lchar++] = 0x00;	/* filler */
		     buffer[lchar++] = (BYTE)ch16;
		     n8ch += 2;
		}
		else
		{
		     buffer[lchar++] = (BYTE)ch16;
		     n8ch++;
		}
			/***blotz need to test pb->dp_option & ??) ***/
			/*** for delimiter echoing option ***/
		if (!(gotdel)) rdecho_ser(unit,ch16);
		more = (n8ch >= bufsiz);
	    }
nochar:	  ;
	}  /* End While */

	if (gotdel)
	{
/****	    if (!(v->v_kbmode & KT_NCser_recho))  ****/
	    	ser_recho(unit,CH_CR);		/* Echo CR if mode set  */

/****	    if (v->v_kbmode & KT_DELECHO)   ****/
/****		ser_recho(unit,CH_LF);****/ /* LF too, if mode so set	*/
	    gotdel = FALSE;
	    r = (LONG)n8ch;
	}
	else
	{		/* time to leave, so pack them vars all up */
	    pb->dp_offset = n8ch;
	    r = 0L;
	}
	return(r);
}

	/* Echo the specified character. Make it visible if not visible */

rdecho_ser(unit,ch16)
    BYTE  unit;
    UWORD ch16;
{
    REG WORD high;
    REG WORD low;

	high = ch16 >> 8;
	low = ch16 & 0x00ff;

	if (!high)
	{
	    if (low < 32)
	    {				/* Control Character */
		ser_recho(unit,'^');
		ser_recho(unit,low + 0x40);
		return;
	    }				/* Normal "8-bit" character */
	    ser_recho(unit,low);
	    return;
	}
	if (high & 0x80)
	    ser_recho(unit,ch16);		/* Foreign Language 16-bit Character */
	else
	{				/* Strange Character (like CTRL-SHIFT-xx) */
	    ser_recho(unit,'#');
	    ser_recho(unit,'#');
	}
}

	/* Echo the specified character unless we are in NO ECHO mode */

ser_recho(unit,ch16)
    REG BYTE  unit;
    REG UWORD ch16;
{
	WORD len,j ;

	    if( ch16 == '\n' ) sdrvcol[unit] = 0;	/* Reset column	*/
	    if( ch16 == '\t' )			/* Expand tabs	*/
	    {
		if( len = (TABLEN - (sdrvcol[unit] % TABLEN)) )
		    for( j=0; j < len; j++ )
			(*pt_hdr[unit]->dh_write)(pt_unit[unit],' ');
		sdrvcol[unit] += len;
	    }
	    else
	    {
		++sdrvcol[unit];
		(*pt_hdr[unit]->dh_write)(pt_unit[unit],(BYTE)ch16);
	    }
}

	/* Returns number of "screen positions" that will be used
	 * if we echo this character
	 */

WORD ser_echosiz(ch16)
    UWORD ch16;
{
    REG WORD high;
    REG WORD low;

	high = ch16 >> 8;
	low = ch16 & 0x00ff;

	if (!high)
	{
	    if (low < 32)
		return(2);
	    return(1);
	}
	else
	    return(2);
}
                                                    
ow < 32)
		return(2);
	    return(1);
	}
	else
	    return(2);
}
                                                    