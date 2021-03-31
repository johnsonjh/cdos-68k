/* SRWAIT.C */
/* 28 DEC 84, ES=7.00 */

#include <portab.h>
#include "system.h"

#define	PBLK	struct	_pblock
#define RECT	struct  _rect

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

RECT
{
	UWORD	col;
	UWORD	row;
	UWORD	ncol;
	UWORD	nrow;
};


LONG s_rwait(flags,fnum,region)
   WORD  flags;
   LONG  fnum;
   RECT  *region;
{
    PBLK  p;
    UWORD *s,*d;
    BYTE  i;

	zerofil(&p,sizeof(PBLK));
	p.pb_flags = flags;
	p.pb_id = fnum;

	s = (UWORD *) region;
	d = (UWORD *) &p.pb_buffer;

	for(i=0;i<4;i++) *d++ = *s++;

	return( __OSIF(F_RWAIT,&p));
}

