*
*	@(#)vprobe.s	1.1	
*
*MH*********************** Module Header ********************************
*									*
* Module Name: vmprobe.s						*
*									*
* Description:								*
*		This module contains the function used to determine	*
*	whether or not a device (i.e. a byte) is present in memory.	*
*	A memory Probe Function.					*
*									*
*	It is used in Concurrent DOS-68K for the VME/10			*
* 									*
* Contents:								*
*		no_device						*
* 									*
* Module-Wide Globals:							*
*									*
*	None.								*
*									*
* Notes:								*
*									*
*	None.								*
*									*
*************************************************************************

*************************************************************************
*									*
*	E Q U A T E S							*
*									*
*************************************************************************

buserr:	.equ	$8			* Bus error vector address
ERRSTKSZ:	.equ	58		* Bus error stack size on 68010
PRESENT:	.equ	0		* Byte is present (no bus error)
NOTPRESENT:	.equ	1		* Byte is not present

*FH********************** Function Header *******************************
*									*
* Function Name: no_device						*
*									*
* Action:								*
*		This function attempts to access a byte, and		*
*	determines whether or not the memory map (or RAM or		*
*	whatever) is present by trapping a possible BUS ERROR.		*
*	We make use of the bus error vector.				*
*									*
* Functions Called:							*
*									*
*		None.							*
*									*
* External References:							*
*									*
*		None.							*
*									*
* Parameters:								*
*									*
*	Receives: A byte address to check				*
*									*
* 	Returns: A word value, 1 is NOT PRESENT, 0 is PRESENT		*
* 									*
* Design Notes:								*
*		We attempt to READ the address.  This may cause		*
*	problems if it is a write-only register of a device,		*
*	returning a bus error when device is really there.		*
*	Or, is reading a memory mapped register resets or changes	*
*	the state of the device.					*
*									*
*************************************************************************

_no_device:
	Move.l	4(SP),A0
	Move.l	buserr,-(SP)		* Save bus error vector contents
	Move.l	#ndber,buserr		* Take over vector
	Move.b	(A0),D0			* Attempt to access address given us

	Moveq.l	#PRESENT,D0		* NO BUS ERROR, we just dropped thru
	Move.l	(SP)+,buserr		* Restore original bus error vector
	Rts

ndber:
	Moveq.l	#NOTPRESENT,D0		* We got here through bus error
	Add.l	#ERRSTKSZ,SP		* address not there, pop bus error stack
	Move.l	(SP)+,buserr		* and restore original bus error vector
	Rts

	.End

,SP		* address not there, pop bus error stack
	Move.l	(SP)+,buserr		* and restore original bus error vector
	