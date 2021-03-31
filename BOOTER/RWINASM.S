*	@(#)rwinasm.s	1.3	
	.text
*
*	Global Code addresses
*
	.globl	_RdDataXfer

*
*	Disk data transfer loop
*	Calling sequence:
*		RdDataXfer(bufadr, ctladr);
*		bufadr is the source/destination data address
*		ctladr is the controller's data register address
*
* without considering wait states movep.l makes
* the faster loop.  however, accounting for wait
* states makes the 68010 loop mode faster.
*
_RdDataXfer:
	move.l	4(sp),a0
	move.l	8(sp),a1
	move.w	#127,d0
rdlp:	move.b	(a1),(a0)+
	dbf	d0,rdlp
	rts
*
	.end

*
_RdDataXfer:
	move.l	4(sp),a0
	move.l	8(sp),a1
	move.w	#127,d0
rdlp:	move.b	(a1),(a0)+
	