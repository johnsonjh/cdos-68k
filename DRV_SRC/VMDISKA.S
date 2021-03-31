*	@(#)diska.s	1.4	
	.text
*
*	Global Code addresses
*
	.globl	_rddat
	.globl	_wrdat
	.globl	_no_device
*
*	Vector Addresses
*
buserr:	 .equ	$8
*
*	Disk data transfer loops
*	Calling sequence:
*		xxdat(bufadr, ctladr);
*		bufadr is the source/destination data address
*		ctladr is the controller's data register address
*
* without considering wait states movep.l makes
* the faster loop.  however, accounting for wait
* states makes the 68010 loop mode faster.
*
_rddat:	move.l	4(sp),a0
	move.l	8(sp),a1
	move.w	#127,d0
rdlp:	move.b	(a1),(a0)+
	dbf	d0,rdlp
	rts
*
_wrdat:	move.l	4(sp),a0
	move.l	8(sp),a1
	move.w	#127,d0
wrlp:	move.b	(a0)+,(a1)
	dbf	d0,wrlp
	rts
*
*
_no_device:
	move.l	4(sp),a0
	move.l	buserr,-(sp)
	move.l	#ndber,buserr
	move.b	(a0),d0
	moveq.l	#0,d0
	move.l	(sp)+,buserr
	rts
ndber:
	moveq.l #1,d0
	add.l	#58,sp
	move.l	(sp)+,buserr
	rts
*
	.end
),d0
	moveq.l	#0,d0
	move.l	(sp)+,buserr
	rts
ndber:
	moveq.l #1,d0
	add.l	#58,sp
	move.l	(sp)+,b