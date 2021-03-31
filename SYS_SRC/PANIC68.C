/* panic68.c - xputs, showregs, ltohex, itohex, ctohex */

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
 *   Version 3		panic68
 *			panic register dump formatting
 *---------------------------------------------------------------*
 * VER	DATE	 BY	CHANGE/COMMENTS
 *
 *  4   08/26/86 DS     Included SYSBUILD.H for STRUCT.H
 *  3	12/10/85 MA	Added XX_MX panic code.
 *  2	07/23/85 MA	Converted to 68K.
 *  1	07/02/85 gat	initial release
 *===============================================================*/

#include	"portab.h"
#include        "sysbuild.h" 
#include	"struct.h"

BYTE	*ctohex(), *itohex(), *ltohex();


#define MAXMSG 19

BYTE *msgtab[MAXMSG] =
{
	"(Non-maskable interrupt)",			/* XX_NMI = 0 */
	"(Bus or Address error in system)",		/* XX_GP = 1 */
	"(Double exception)",				/* XX_DX = 2 */
	"(Failure to call NEXTASR in asr causing event)",
							/* XX_NEXTASR = 3 */
	"(Illegal size request in mgetblk)",		/* XX_MGSIZE = 4 */
	"(Illegal address in free list in mgetblk)",	/* XX_MGADDR = 5 */
	"(Out of Memory Pool)",				/* XX_MGSPACE = 6 */
	"(Bad root in hidden field in mfreblk)",	/* XX_MFROOT = 7 */
	"(Dispatches disabled calling terminate())",	/* XX_TERM = 8 */
	"(Dispatches disabled calling salloc())",	/* XX_SALLOC = 9 */
	"(Dispatches disabled calling mwait())",	/* XX_MWAIT = 10 */
	"(Can not find SWI event in GOSWI)",		/* XX_GOSWI = 11 */
	"(Exited expew with dispatches disabled)",	/* XX_EXPEW = 12 */
	"(Resource manager initialization failed)",	/* XX_INIT = 13 */
	"(Aborting process not in RUN state)",		/* XX_ABANY = 14 */
	"(No e_pred in evremove)",			/* XX_EVREM = 15 */
	"(Mutual exclusion failure)",			/* XX_MX = 16 */
	"(Out of free ASRs)",				/* XX_NOASR = 17 */
	"(Block freed twice in mgetblk)"		/* XX_MFTWO = 18 */
} ;

BYTE *defmsg = "(Unrecognized panic code)";

MLOCAL BYTE ls[] = "         ";
MLOCAL BYTE ws[] = "     ";


/******************************************************************************
 * xputs - print a string
 */
xputs(s)
BYTE	*s;
{
	while(*s)
		xputc(*s++);
}


/******************************************************************************
 * showregs - show registers in "regsave" order
 */
showregs(err,regs)
REG WORD err;
REG REGSAV	*regs;
{
	REG WORD i;

	xputs("Panic ");
	if (err >= MAXMSG || err < 0)
	{
		xputs("( ");
		xputs(itohex(err, ws));
		xputs(")");
		xputs(defmsg);
	}
	else
		xputs(msgtab[err]);
	xputs("\r\nD: ");
	for (i=0; i < 8; i++)
		xputs(ltohex(regs->r_dreg[i], ls));		
	xputs("\r\nA: ");
	for (i=0; i < 8; i++)
		xputs(ltohex(regs->r_areg[i], ls));
	xputs("\r\nPC: ");
	xputs(ltohex(regs->r_pc, ls));
	xputs(" SR: ");
	xputs(itohex(regs->r_sr, ws));
	xputs("\r\nStack dump:\r\n");
	dump(regs->r_areg[7],256);
}


/***********************************************************************
*	DUMP - display some memory in hex
***********************************************************************/

MLOCAL dump(p,n)
REG WORD *p;		/* memory to dump */
REG WORD n;		/* number of bytes to dump */
{
    WORD newline;

    newline = TRUE;
    while (n > 0)
    {
	if (!((LONG)p & 0x0f))		/* time to do a cr/lf */
	    newline = TRUE;
	if (newline)
	{
	    xputs("\r\n");		/* do the cr/lf */
	    xputs(ltohex(p,ls));	/* print the address */
	    newline = FALSE;
	}
	xputs(itohex(*p++,ws));		/* print the data */
	n -= sizeof(WORD);
    }
}


/******************************************************************************
 * ltohex - convert long to 8 byte, leading zero
 *
 */
BYTE*
ltohex(l, s)
LONG	l;	/* long to convert */
BYTE	*s;	/* destination field, at least 8 bytes wide */
{
	itohex((UWORD)(l>>16), s);
	itohex((UWORD)l, s+4);
	return(s);
}

/******************************************************************************
 * itohex - convert int to 4 hex bytes, leading zeroes
 *
 */
BYTE*
itohex(i, s)
WORD	i;	/* word to convert */
BYTE	*s;	/* destination field, at least 4 bytes wide */
{
	ctohex(i>>8, s);
	ctohex(i, s+2);
	return(s);
}

/******************************************************************************
 * ctohex - convert char to 2 hex bytes, leading zeroes
 *
 */
BYTE*
ctohex(c, s)
BYTE	c;	/* byte to convert */
BYTE	*s;	/* destination field, at least 2 bytes wide */
{
	static char hexdig[] = "0123456789ABCDEF";
	*s = hexdig[(c>>4) & 0xf];
	*(s+1) = hexdig[c & 0xf];
	return(s);
}
