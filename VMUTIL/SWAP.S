*	@(#)swap.s	2.1	

*******************************************************************
*	SWAP.S
*
*	Assembly routines to handle word/byte swap problems
*	in porting 8086 stuff to 68K.
*******************************************************************
	.Text

*******************************************************************
*	SWAP word
*
*	Swap the two bytes that make up the word argument
*******************************************************************

	.Globl	_swapw
_swapw:
	Move	4(SP),D0
	Rol	#8,D0
	Rts

*******************************************************************
*	SWAP long
*
*	Reverse the order of the bytes in the four-byte argument
*******************************************************************

	.Globl	_swapl
_swapl:
	Move.l	4(SP),D0		* get the long word
	rol	#8,D0			* swap its low bytes
	swap	D0			* put it in high half of D0
	rol	#8,D0			* swap the high bytes (now in low half)
	Rts				* return long result

*******************************************************************
*	GETWORD (bufptr)
*
*	Get a word that may be on an odd boundary, and reverse
*	the order of the bytes while we're at it.
*******************************************************************

	.globl	_getword
_getword:
	move.l	4(sp),a0		* get address of the word
	move.b	1(a0),d0		* get the low byte
	lsl	#8,d0			* shift it to high byte
	move.b	(a0),d0			* get the high byte into low of D0
	rts				* return the word

*******************************************************************
*	GETLONG (bufptr)
*
*	Get a long that may be on an odd boundary, and reverse
*	the order of the bytes while we're at it.
*******************************************************************

	.globl	_getlong
_getlong:
	Move.l	4(SP),A0		* get address of the long
	Move.b	3(A0),D0		* get the low byte
	Lsl	#8,D0			* shift it to high byte
	Move.b	2(A0),D0		* get the high byte into low of D0
	Swap	D0			* put it in high half of D0
	Move.b	1(A0),D0		* get the low byte
	Lsl	#8,D0			* shift it to high byte
	Move.b	(A0),D0			* get the high byte into low of D0
	Rts

*******************************************************************
*	PUTWORD (bufptr,val)
*
*	Store a word that may be on an odd boundary, and reverse
*	the order of the bytes while we're at it.
*******************************************************************

	.globl	_putword
_putword:
	move.l	4(sp),a0		* get address of the word
	move.b	9(sp),(a0)		* store the low byte of word
	move.b	8(sp),1(a0)		* store the high byte of word
	rts

*******************************************************************
*	PUTLONG (bufptr,val)
*
*	Store a long that may be on an odd boundary, and reverse
*	the order of the bytes while we're at it.
*******************************************************************

	.globl	_putlong
_putlong:
	move.l	4(sp),a0		* get address of the word
	move.b	8(sp),3(a0)		* store the high byte of word
	move.b	9(sp),2(a0)		* store the low byte of word
	move.b	10(sp),1(a0)		* store the high byte of word
	move.b	11(sp),(a0)		* store the high byte of word
	rts
	.End
