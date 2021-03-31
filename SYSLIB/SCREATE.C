/* SCREATE.C */
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

LONG s_create(option,flags,name,record_size,security,size)
    BYTE  option;
    UWORD flags;
    BYTE  *name;
    UWORD record_size;
    UWORD security;
    LONG  size;
{
    PBLK p;

	struct	_pb
	{
		BYTE	pb_mode;
		BYTE	pb_option;
		WORD	pb_flags;
		LONG	pb_swi;
		LONG	pb_id;
		UWORD	pb_rsize;
		UWORD	pb_security;
		LONG	pb_bufsiz;
	}  *b;

	b = (struct _pb *) &p;
	zerofil(b,sizeof(PBLK));
	b->pb_option = option;
	b->pb_flags = flags;
	b->pb_id = (long)name;
	b->pb_rsize = record_size;
	b->pb_security = security;
	b->pb_bufsiz = size;
	return( __OSIF( F_CREATE,b ) );
}
