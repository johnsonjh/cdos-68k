*	@(#)support.s	1.1	
*
*	Assembly language support routines for the Concurrent DOS
*	boot loader for the VME/10.
*
*	These are here purely for performance considerations!
*
*	ALSO: these are meant to be called from `C' routines!!
*	We will frequently trash temporary registers (we assume
*	D0, D1, A0, and A1 are trashable).
*

	.Text

*
*	Your basic byte move.
*
*
*	VOID	movb(s,d,count)
*	REG BYTE *s, *d;
*	REG WORD count;
*	{
*		while( count-- )
*			*d++ = *s++;
*	}
*	

	.globl	_movb

_movb:
	Move.l	4(SP),A0
	Move.l	8(SP),A1
	Move.w	12(SP),D0
	Bra.b	blps
blp:
	Move.b	(A0)+,(A1)+
blps:	Dbra	D0,blp
	Rts

*	
*	VOID setb(ch,d,count)
*	REG BYTE ch;
*	REG BYTE *d;
*	REG WORD count;
*	{
*		while( count-- )
*			*d++ = ch;
*	}
*	

	.globl	_setb

_setb:
	Move.b	5(SP),D1
	Move.l	6(SP),A0
	Move.w	10(SP),D0
	Bra.b	c1ls
c1l:
	Move.b	D1,(A0)+
c1ls:	Dbra	D0,c1l
	Rts

	.End

	Move.b	5(SP),D1
	Move.l	6(SP),A0
	Move.w	10(SP),D0
	Bra.b	c1ls
c1l:
	Move.b	D1,(A0)