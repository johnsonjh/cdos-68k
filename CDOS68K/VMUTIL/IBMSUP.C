/*	@(#)ibmsup.c	2.1		*/

#include "portab.h"

VOID movb(s,d,cnt)
REG BYTE *s, *d;
REG WORD cnt;
{
	while( cnt-- > 0 )
		*d++ = *s++;
}

VOID setb(c,p,n)
BYTE c;
BYTE *p;
WORD n;
{
	while( n-- > 0 )
		*p++ = c;
}
