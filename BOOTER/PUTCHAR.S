
*	@(#)putchar.s	1.1	

*******************************************************************
*
*	putchar() is used to send characters to the console.
*	Currently it uses a call to the TENbug monitor putchar routine.
*	TENbug automatically performs an LF,CR when an LF is output.
*
*	CP/M `C' stack frame:
*
*
*		A7 -->   longword for proc calls
*			 saved registers
*				.
*			 local var area
*				.
*		A6 -->	 previous value of A6
*			 return address
*			 argument 1
*			 argument 2
*				.
*				.
*
*	for us our byte is at offset '5' on entry (add 8 after we save the
*	registers.
*
*******************************************************************
	
	.Globl	_putchar

_putchar:	
	Move.l	A6,-(SP)	* Save the registers we trash.
	Move.l	A5,-(SP)
	Lea.l	13(SP),A5	* Start character address.
	Lea.l	1(A5),A6	* End character address (start + 1).
	Trap	#15		* TENbug trap handler interface.
	.dc.w	6		* Write string function number.
	Move.l	(SP)+,A5	* Restore registers.	
	Move.l	(SP)+,A6
	Rts

	.End
terface.
	.dc.w	6		* Write string function number.
	Move.l	(SP)+,A5	* Restore registers.	
	Move.l	(SP)+,A6