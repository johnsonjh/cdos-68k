/* ELOCK.C */
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

LONG e_lock(swi,flags,fnum,offset,nbytes)
	UWORD	flags;
	LONG	swi,fnum,offset,nbytes;
{
	PBLK	p;

	zerofil(&p,sizeof(PBLK));

	p.pb_mode = 1;
	p.pb_flags = flags;
	p.pb_id = fnum;
	p.pb_buffer = offset;
	p.pb_bufsiz = nbytes;
	return(__OSIF(F_LOCK,&p));
}
