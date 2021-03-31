/* SMALLOC.C */
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

LONG s_malloc(option,mpbptr)
    BYTE  option;
    LONG  mpbptr;
{
	PBLK	p;
	LONG	mpb_size;

	zerofil( &p,(sizeof(PBLK)) );

	mpb_size = 12L;			/* Size of mem para block */

	p.pb_option = option;
	p.pb_buffer = mpbptr;
	p.pb_bufsiz = mpb_size;

	return( __OSIF( F_MALLOC,&p ));
}
