/* SLOCK.C */
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

LONG s_lock(flags,fnum,offset,nbytes)
	UWORD	flags;
	LONG	fnum,offset,nbytes;
{
	PBLK	p;

	zerofil(&p,sizeof(PBLK));

	p.pb_flags = flags;
	p.pb_id = fnum;
	p.pb_buffer = offset;
	p.pb_bufsiz = nbytes;
	return(__OSIF(F_LOCK,&p));
}
