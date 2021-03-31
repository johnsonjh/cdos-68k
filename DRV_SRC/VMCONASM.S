*	@(#)vmconasm.s	1.52
*
*    VERSION	DATE      BY    CHANGE/COMMENTS
*    =======	====	  ==	===============
*    1.52	10/03/85  MA	itovcp fixed (was copying 1 too many bytes)
*    1.51	9/9/85	  MA	vwcopy fixed
*    1.50	8/26/85   MA	vbcdopy fixed
*
*	Assembly language support routines for the Concurrent DOS
*	console driver for the VME/10.
*
*	These are here purely for performance considerations!
*
*	ALSO: these are meant to be called from `C' routines!!
*	We will frequently trash temporary registers (we assume
*	D0, D1, A0, and A1 are trashable).
*

	.Text

DEBUG	equ	0

*	
*
*	 The following set of Copy routines are used in write()/copy()
*	 when copying between frames.
*

*
*	Your basic byte copy. We do consider the direction
*	and possible overlap though.  If overlapped, we reverse
*	direction of copy.
*
*
*	VOID	vbcopy(s,d,count)
*	REG BYTE *s, *d;
*	REG WORD count;
*	{
*		if( d > s )
*		{
*			s += count;
*			d += count;
*			while( count-- )
*				*(--d) = *(--s);
*		}
*		else
*		{
*			while( count-- )
*				*d++ = *s++;
*		}
*	}
*	

	.globl	_vbcopy
	.globl	_bcopy

_vbcopy:
_bcopy:
	Move.l	4(SP),A0
	Move.l	8(SP),A1
ifne	DEBUG
	bsr	chk2
endc
	Move.w	12(SP),D0

	Cmp.l	A1,A0
	Bcc.b	b11lsn

	Add.w	D0,A0
	Add.w	D0,A1
	Bra.b	b11lsr
b11lr:
	Move.b	-(A0),-(A1)
b11lsr:	Dbra	D0,b11lr
	Rts

b11ln:
	Move.b	(A0)+,(A1)+
b11lsn:	Dbra	D0,b11ln
	Rts

*
*	Your basic byte copy with dirty frame handling. Called
*	from the copy() routine.  We do consider the direction
*	and possible overlap though.  If overlapped, we reverse
*	direction of copy.
*
*
*	VOID	vbdcopy(s,d,count,dirty)
*	REG BYTE *s, *d;
*	REG WORD count;
*	REG BYTE *dirty;
*	{
*		if( d > s )
*		{
*			s += count;
*			d += count;
*			if( dirty )
*			{
*				dirty += count;
*				while( count-- )
*				{
*					--dirty;
*					if( *(--d) != *(--s) )
*					{
*						*dirty = 1;
*						*d = *s;
*					}
*				}
*			}
*			else
*				while( count-- )
*					*(--d) = *(--s);
*		}
*		else
*		{
*			if( dirty )
*				while( count-- )
*				{
*					if( *d != *s )
*						*dirty = 1;
*					dirty++;
*					*d++ = *s++;
*				}
*			else
*				while( count-- )
*					*d++ = *s++;
*		}
*	}
*	

	.globl	_vbdcopy

_vbdcopy:
	Move.l	A2,-(SP)
	Move.l	8(SP),A0	* the source address
	Move.l	12(SP),A1	* the destination address
	Move.w	16(SP),D0	* the byte count
	Move.l	18(SP),A2	* the dirty plane, 0 if none
ifne	DEBUG
	bsr	chk2
endc
*
*	Do we need to reverse direction of copy?
*
	Cmp.l	A1,A0
	Bcc.b	bdnorm
*
*	Reverse copying...
*
	Add.w	D0,A0		* end of source
	Add.w	D0,A1		* end of dest
	Move.l	A2,D1		* Do we have a dirty plane?
	Beq.b	bdlsr
*
*	Yes, we have a dirty plane, so we have to use it.

*
	add.w	d0,a2		* end of dirty plane
ifne	DEBUG
	bsr	chk3
endc
	Bra.b	dyrlps
dyrlp:	Sub	#1,A2
	Move.b	-(A1),D1
	Cmp.b	-(A0),D1
	Beq	dyrlps
	Move.b	#1,(A2)
	Move.b	(A0),(A1)
dyrlps:	Dbra	D0,dyrlp
	Bra.b	bdexit
*
*	No dirty plane, we can go tight loop...
*
bdlr:	Move.b	-(A0),-(A1)
bdlsr:	Dbra	D0,bdlr
	Bra.b	bdexit
*
*	Normal copying...
*
bdnorm:	Move.l	A2,D1		* Do we have a dirty plane?
	Beq.b	bdlsn
*
*	Yes, we have a dirty plane...
*
	Bra.b	bdynls
bdynl:	Move.b	(A0),D1
	Cmp.b	(A1),D1
	Beq.b	nsame
	Move.b	#1,(A2)
nsame:	Add	#1,A2
	Move.b	(A0)+,(A1)+
bdynls:	Dbra	D0,bdynl
	Bra.b	bdexit
*
*	No, we don't have a dirty plane...
*
bdln:	Move.b	(A0)+,(A1)+
bdlsn:	Dbra	D0,bdln

bdexit:	Move.l	(SP)+,A2
	Rts

*
*	Your basic word copy. Again considering direction.
*	Count is number of words.
*
*
*	VOID	vwcopy(s,d,count)
*	REG WORD *s, *d;
*	REG WORD count;
*	{
*		while( count-- )
*			*d++ = *s++;
*	}
*	

	.globl	_vwcopy

_vwcopy:
	Move.l	4(SP),A0
	Move.l	8(SP),A1
ifne	DEBUG
	bsr	chk2
endc
	Move.w	12(SP),D0	* get the length (in words)

	Cmp.l	A1,A0		* need to do a backwards copy?
	Bcc.b	w11lsn		* no - skip this

	Add.w	D0,A0		* add length to source pointer
	Add.w	D0,A0
	Add.w	D0,A1		* add length to dest pointer
	Add.w	D0,A1
	Bra.b	w11lsr
w11lr:
	Move.w	-(A0),-(A1)	* copy them bytes
w11lsr:	Dbra	D0,w11lr
	Rts

w11ln:
	Move.w	(A0)+,(A1)+
w11lsn:	Dbra	D0,w11ln
	Rts

*	
*	VOID vbfill(d,count,ch)
*	REG BYTE *d;
*	REG WORD count;
*	REG BYTE ch;
*	{
*		while( count-- )
*			*d++ = ch;
*	}
*	

	.globl	_vbfill

_vbfill:
	Move.l	4(SP),A0
ifne	DEBUG
	bsr	chk1
endc
	Move.w	8(SP),D0
	Move.b	11(SP),D1
	Bra.b	c1ls

c1l:
	Move.b	D1,(A0)+
c1ls:	Dbra	D0,c1l
	Rts

*	
*	VOID vbdfill(d,count,ch,dirty)
*	REG BYTE *d;
*	REG WORD count;
*	REG BYTE ch;
*	REG BYTE *dirty;
*	{
*		if( dirty )
*			while( count-- )
*			{
*				if( *d != ch )
*					*dirty = 1;
*				*d++ = ch;
*				dirty++;
*			}
*		else
*			while( count-- )
*				*d++ = ch;
*	}
*	

	.globl	_vbdfill

_vbdfill:
	Move.l	4(SP),A0	* Starting destination address
	Move.w	8(SP),D0	* Count of bytes
	Move.b	11(SP),D1	* Byte to fill with
	Move.l	12(SP),A1	* Dirty plane pointer
ifne	DEBUG
	bsr	chk1
endc
*
*	Do we have a dirty plane?
*
	Tst.l	12(SP)
	Beq.b	ncls
*
*	Yes, we have a dirty plane
*
ifne	DEBUG
	bsr	chk2
endc
	Bra.b	ycls
ycl:	Cmp.b	(A0),D1
	Beq.b	csame
	Move.b	#1,(A1)
csame:	Add	#1,A1
	Move.b	D1,(A0)+
ycls:	Dbra	D0,ycl
	Rts
*
*	No, we don't have a dirty plane
*
ncl:	Move.b	D1,(A0)+
ncls:	Dbra	D0,ncl
	Rts

*	
*	VOID vwfill(d,count,ch)
*	REG WORD *d;
*	REG WORD count;
*	REG WORD w;
*	{
*		while( count-- )
*			*d++ = w;
*	}
*	

	.globl	_vwfill

_vwfill:
	Move.l	4(SP),A0
ifne	DEBUG
	bsr	chk1
endc
	Move.w	8(SP),D0
	Move.w	10(SP),D1
	Bra.b	wc1ls

wc1l:
	Move.w	D1,(A0)+
wc1ls:	Dbra	D0,wc1l
	Rts

*
*	Alter routine called from copy()/alter().
*	
*	VOID vbalter(d,andc,xorc,count,dirty)
*	REG BYTE	*d, andc, xorc;
*	REG WORD	count;
*	REG WORD	*dirty;
*	{
*		if( dirty )
*		{
*			while( count-- )
*			{
*				BYTE	tmp;
*				
*				tmp = ((*d & andc) ^ xorc);
*				if( *d != tmp )
*					*dirty = 1;
*				*d++ = tmp;
*				dirty++;
*			}
*		}
*		else
*			while( count-- )
*			{
*		 		*d = ((*d & andc) ^ xorc);
*		 		d++;
*			}
*	}
*	

	.globl	_vbalter

_vbalter:
	Movem.l	D2-D3,-(SP)	* We need another register
	Move.l	12(SP),A0
ifne	DEBUG
	bsr	chk1
endc
	Move.b	17(SP),D0	* andc
	Move.b	19(SP),D1	* xorc
	Move.w	20(SP),D2	* count
	Move.l	22(SP),A1	* Possible dirty plane
*
*	See if we have a dirty plane.
*
	Move.l	A1,D3
	Tst.l	D3
	Beq.b	bnals
*
*	Yes we have a dirty plane...
*
ifne	DEBUG
	bsr	chk2
endc
	Bra.b	byals
byal:	Move.b	(A0),D3		* Fetch byte to alter
	And.b	D0,D3
	Eor.b	D1,D3
	Cmp.b	(A0),D3
	Beq.b	basame
	Move.b	#1,(A1)
basame:	Add	#1,A1
	Move.b	D3,(A0)+
byals:	Dbra	D2,byal
	Bra.b	altex
*
*	No dirty plane, straightforward alter
*
bnal:	Move.b	(A0),D3		* Fetch byte to alter
	And.b	D0,D3
	Eor.b	D1,D3
	Move.b	D3,(A0)+
bnals:	Dbra	D2,bnal

altex:	Movem.l	(SP)+,D2-D3
	Rts

*
*	Routine to copy changes from IBM physical console
*	to real VME/10 console.
*
*	
*	itovcp(s,d,count,p)
*	REG BYTE	*s, *d;
*	REG WORD	count;
*	WORD	p;
*	{
*		if( p == PL_CHAR )
*			while( count-- )
*			{
*				*d = *s++ & 0x7f;
*				d += 2;
*			}
*		else if( p == PL_ATTR )
*			while( count-- )
*			{
*				*d = ibmtovm[*s++];
*				d += 2;
*			}
*	}
*	
*	

	.globl	_itovcp
	.globl	_ibmtovm

_itovcp:
	Move.l	A2,-(SP)
	Move.l	8(SP),A0	* source ptr
	Move.l	12(SP),A1	* destination ptr
ifne	DEBUG
	bsr	chk2
endc
	Move.w	16(SP),D0	* count

	Tst.w	18(SP)		* Is plane == 0 ?
	Bne.b	titvl1
	Bra.b	itvls0		* MA 10/3/85

itvl0:				* We are copying plane 0.
	Move.b	(A0)+,D1
	And.b	#$7F,D1		* Strip off high bit of byte.
	Move.b	D1,(A1)
	Addq.l	#2,A1
itvls0:	Dbra	D0,itvl0
	Bra.b	itvret

titvl1:
	Cmp	#1,18(SP)	* We are copying plane 1.
	Bne.b	itvret

	Move.l	#_ibmtovm,A2
	Clr.w	D1
	Bra.b	itvls1

itvl1:
	Move.b	(A0)+,D1
	Move.b	0(A2,D1.w),(A1)
	Addq.l	#2,A1
itvls1:	Dbra	D0,itvl1

itvret:
	Move.l	(SP)+,A2
	Rts

ifne	DEBUG
	.globl	_panic

BLORCH	equ	$28000

chk3:
	cmp.l	#BLORCH,a2
	bcs	crash
chk2:
	cmp.l	#BLORCH,a1
	bcs	crash
chk1:
	cmp.l	#BLORCH,a0
	bcs	crash
	rts
crash:
	move.w	#99,-(sp)
	jsr	_panic
self:
	bra	self
endc

	.End
cs	crash
chk1:
	cmp.l	#BLORCH,a0
	bcs	crash
	rts
crash:
	move.w	#99,-(sp)
	jsr	_panic
self:
	bra	self
e