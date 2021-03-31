/* ESPECIAL.C */
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

LONG e_special(swi,func,flags,fnum,databuf,dbufsiz,parmbuf,pbufsiz)
	UBYTE	func;
	UWORD	flags;
	LONG	swi,fnum,databuf,dbufsiz,parmbuf,pbufsiz;
{
	PBLK	p;

	zerofil(&p,sizeof(PBLK));

	p.pb_mode = 1;
	p.pb_option = func;
	p.pb_flags = flags;
	p.pb_swi = swi;
	p.pb_id = fnum;
	p.pb_buffer = databuf;
	p.pb_bufsiz = dbufsiz;
	p.pb_p1 = parmbuf;
	p.pb_p2 = pbufsiz;
	return(__OSIF(F_SPECIAL,&p));
}
