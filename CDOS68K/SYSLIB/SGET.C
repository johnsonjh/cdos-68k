/* SGET.C */
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

LONG s_get(table,id,buffer,bufsiz)
    BYTE table;
    LONG id;
    BYTE *buffer;
    LONG bufsiz;
{
    PBLK p;

	zerofil(&p,sizeof(PBLK));
	p.pb_option = table;
	p.pb_id = id;
	p.pb_buffer = (LONG)buffer;
	p.pb_bufsiz = bufsiz;
	return( __OSIF( F_GET,&p ) );
}
