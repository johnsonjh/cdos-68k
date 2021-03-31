*
*	Copyright 1983
*	Alcyon Corporation
*	8716 Production Ave.
*	San Diego, CA  92121
*
*	@(#)lrem.s	2.2    8/8/84
*

		.globl	_ldiv
		.globl	_lrem
		.globl	lrem

		.globl	_ldivr
	
		.text

_lrem:
lrem:
		move.l	8(sp),-(sp)
		move.l	4+4(sp),-(sp)
		jsr		_ldiv
		addq.l	#8,sp
		move.l	d1,d0
		rts

		.text

_lrem:
lrem:
		move.l	8(sp),-(sp)
		move.l	4+4