/* SALTER.C */
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

LONG s_alter(flags,fnum,dframe,drect,alterb)
	UWORD	flags;
	LONG	fnum,dframe,drect;
	BYTE	*alterb;
{
	PBLK 	p;
	int	i;
	struct	_pb
	{
		BYTE	mode;
		BYTE	option;
		UWORD	flags;
		LONG	swi;
		LONG	fnum;
		LONG	dframe;
		LONG	drect;
		UBYTE	alterb[6];
		WORD	res;
	} *b;

	b = (struct _pb *) &p;
	zerofil(b,sizeof(PBLK));
	b->flags = flags;
	b->fnum = fnum;
	b->dframe = dframe;
	b->drect = drect;
	for(i=0;i<6;i++) b->alterb[i] = *alterb++;

	return(__OSIF(F_ALTER,b));
}
