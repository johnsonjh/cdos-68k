*
*	Copyright 1983
*	Alcyon Corporation
*	8716 Production Ave.
*	San Diego, CA  92121
*
*	@(#)ldiv.s	2.2    8/8/84
*
* LDIV - Long division routine.  On exit, d0 = result, d1 = remainder.
*	The remainder is also stored in the global ldivr.
*	Registers modified d0-d2, a0

		.globl _ldivr
		.globl _ldiv
		.globl ldiv

		.comm _ldivr,4

hbit	.equ	$80000000

		.text

_ldiv:
ldiv:
		move.l	d3,a0		// save it
		move.l	4(sp),d1	// dividend
		bge		L10			//		positive
		neg.l	d1			//		negative, so need it positive
L10:
		move.l	8(sp),d3	// divisor
		bgt		L30			//		positive
		blt		L20			//		negative
		move.l	#hbit,d0	//		zero => largest result
		move.l	d0,d1		//			(and remainder)
		bra		L110

L20:
		neg.l	d3			// make divisor positive
L30:
		moveq	#0,d0		// initialize result register
		cmp.l	d1,d3		// is divisor > dividend
		blt		L40			//		no
		bgt		L90			//		yes, return 0 result
		moveq	#1,d0		// they're equal, result is 1
		moveq	#0,d1		//		remainder is 0
		bra		L90

L40:
		moveq	#2,d2		// prepare for hard way loop
		cmp.l	#$10000,d1	// dividend small (2**16-1 or less) ?
		bge		L60			//		no, divide the hard way
		divu	d3,d1		//		yes, easy !
		move.w	d1,d0
		clr.w	d1
		swap	d1
		bra		L90

L50:
		add.l	d2,d2		// asl.l power of 2
L60:
		add.l	d3,d3		// asl.l divisor
		cmp.l	d3,d1		// shifted divisor > dividend
		bhis	L50			//		no
		bra		L80

L70:
		cmp.l	d3,d1		// ugh! divide a bit at a time
		blo		L80
		or.l	d2,d0
		sub.l	d3,d1
L80:
		lsr.l	#1,d3
		lsr.l	#1,d2
		bne		L70

L90:
		tst.w	4(sp)		// dividend negative ?
		bpl		L100		//		no
		neg.l	d0			//		yes, result and remainder change
		neg.l	d1
L100:
		tst.w	8(sp)		// was divisor negative
		bpl		L110		//		no
		neg.l	d0			//		yes, result (only) changes
L110:
		move.l	d1,_ldivr	// store remainder for lrem	
		move.l	a0,d3		// restore it
		rts
	//		yes, result (only)