/* EWRITE.C */
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

LONG e_write(swi,flags,fnum,buffer,bufsiz,offset)
    LONG  swi;
    UWORD flags;
    LONG  fnum;
    BYTE  *buffer;
    LONG  bufsiz;
    LONG  offset;
{
    PBLK p;

	zerofil(&p,sizeof(PBLK));
/*	p.pb_option = 1;	*/
	p.pb_mode = 1;
	p.pb_flags = flags;
	p.pb_swi = swi;
	p.pb_id = fnum;
	p.pb_buffer = (LONG)buffer;
	p.pb_bufsiz = bufsiz;
	p.pb_p1 = offset;
	return( __OSIF( F_WRITE,&p ) );
}
