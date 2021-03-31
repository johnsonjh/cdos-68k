
*	@(#)swap.s	1.2	

*
*	These routines are used to handle swapped
*	and misaligned WORDs and LONGs in Concurrent DOS.
*

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
	Rol	#8,D0			* swap its low bytes
	Swap	D0			* put it in high half of D0
	Rol	#8,D0			* swap the high bytes (now in low half)
	Rts				* return long result

*******************************************************************
*	GETWORD (bufptr)
*
*	Get a word that may be on an odd boundary, and reverse
*	the order of the bytes while we're at it.
*******************************************************************

	.globl	_getword
_getword:
	Move.l	4(SP),A0		* get address of the word
	Move.b	1(A0),D0		* get the low byte
	Lsl	#8,D0			* shift it to high byte
	Move.b	(A0),D0			* get the high byte into low of D0
	Rts				* return the word

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
	Rts				* return the long

	.End
Lsl	#8,D0			* shift it to high b