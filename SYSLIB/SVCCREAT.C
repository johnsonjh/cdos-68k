/* SVCCREAT.C (PART OF S_CREATE) */
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

LONG s_vccreate(flags,pfnum,rows,columns,top,bottom,left,right)
    UWORD flags;
    LONG  pfnum;
    WORD  rows;
    WORD  columns;
    BYTE  top;
    BYTE  bottom;
    BYTE  left;
    BYTE  right;
{
    PBLK p;

	struct	_pb
	{
		BYTE	pb_mode;
		BYTE	pb_option;
		WORD	pb_flags;
		LONG	pb_swi;
		LONG	pb_pfnum;
		WORD	pb_rows;
		WORD    pb_columns;
		BYTE    pb_top;
		BYTE    pb_bottom;
		BYTE    pb_left;
		BYTE    pb_right;
	}  *b;

	b = (struct _pb *) &p;
	zerofil(b,sizeof(PBLK));
	b->pb_option = 2;		/* create Vconsole    */
	b->pb_flags = flags;
	b->pb_pfnum = pfnum;            /* fnum from parent   */
	b->pb_rows = rows;
	b->pb_columns = columns;
	b->pb_top = top;
	b->pb_bottom = bottom;
	b->pb_left = left;
	b->pb_right = right;

	return( __OSIF( F_CREATE,b ) );
}

