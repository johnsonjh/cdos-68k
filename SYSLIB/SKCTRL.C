/* SKCTRL.C */
/* 7 SEP 84, ES=6.01 */
/* 28 DEC 84, ES=7.00 */

#include <portab.h>
#include "system.h"

#define	PBLK	struct	_pblock

EXTERN LONG __OSIF();
EXTERN VOID zerofil();

PBLK
{
	BYTE	pb_mode;
	BYTE	pb_option;
	WORD	pb_flags;
	LONG	pb_swi;
	LONG	pb_id;
	LONG	pb_buffer;
	LONG	pb_bufsiz;
	LONG	pb_p1;
	LONG	pb_p2;
	LONG	pb_p3;
};

LONG s_kctrl(fnum,nranges,begend)
    LONG  fnum;
    BYTE  nranges;
    UWORD begend;
{
    PBLK p;
    UWORD *b,*q;
    BYTE  i;

	zerofil(&p,sizeof(PBLK));
	b = (UWORD *)(&begend);
	p.pb_id = fnum;
	nranges *= 2;
	for (i=0, q=(UWORD *)(&p.pb_buffer); i < nranges; i++)
		*q++ = *b++;	/* copy all words */
	return( __OSIF(F_KCTRL,&p));
}

