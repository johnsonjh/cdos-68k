************************************************************************
*	Copyright (c) 1985, Digital Research, Inc.  All Rights Reserved.  The
*	Software Code contained in this listing is proprietary to Digital
*	Research, Inc., Monterey, California and is covered by U.S. and other
*	copyright protection.  Unauthorized copying, adaptation, distribution,
*	use or display is prohibited and may be subject to civil and criminal
*	penalties.  Disclosure to others is prohibited.  For the terms and
*	conditions of software code use refer to the appropriate Digital
*	Research License Agreement.
************************************************************************

************************************************************************
*   Version 2.1		MMU.S
*			Assembly memory management routines
*
* IMPORTANT NOTE:
*	This module uses #include, so use CP68 before AS68.
*	The IASM.SUB file does this automatically.
*
*-----------------------------------------------------------------------
* VER	DATE		BY	CHANGE/COMMENTS
*-----------------------------------------------------------------------
* 2.1	11/10/85	MA	Moved addrerr and supmode routines outside
*				of conditionals.  Fixed 68000 bugs in
*				painting bus and address error vectors.
* 2.0	08/07/85	MA	Merge LMD into MD structure.  Converted lmove
*				function into assembly (used to be in HEAP.C).
* 				Change p_mem equate for new PD structure.
*				Call panic if bus error in supervisor mode.
*				Ditto for address error. Changed to
*				include "cpummu.equ".
*******************************************************************

#include "cpummu.equ"

BUSERR	Equ	8		* bus error vector
ADDRERR equ	$c		* address error vector
RGCNTXT	Reg	D0-D7/A0-A6	* Context switch registers to save


************************************************************************
*	External variables and functions
************************************************************************

	.globl	_rlr			* pointer to current running process
	.Globl	_mmuclear,_mmufail	* OEM's mmu routines
	.globl	_doint			* soft interrupt handler
	.globl	_panic			* fatal error handler


*******************************************************************
*	PD - Process Descriptor Equates
*
*	Definition of PD as it appears in STRUCT.H, but formatted
*	for assembler use.
*******************************************************************

P_MEM	Equ	$b0

*******************************************************************
*	MD - physical memory descriptor structure as in STRUCT.H
*******************************************************************

M_LINK	equ	0		* link to next MD
M_START	equ	4		* physical base address
M_LENGTH equ	8		* length of block
M_LBASE equ	12		* logical base address of segment

	.Text

*******************************************************************
*	sysaa(MD)	- System allocate addressability
*	sysfa(address)	- System free addressability
*******************************************************************

	Globl	_sysaa,_sysfa
_sysaa:
_sysfa:	Rts


*******************************************************************
*	INITMMU
*
*	Call OEM's "mmuclear" routine.  Then initialize the bus
*	error and address error vector to point to CDOS's special
*	handlers.
*******************************************************************

	.globl	_initmmu	* defined here

_initmmu:
	jsr	_mmuclear	* Call OEM's initialization routine

* First, set up the vector for address error.  If we're on the 68000,
* put the vector number in high order 8 bits of the vector.

	move.l	#addrerr,d0	* put address error vector in d0
ifeq	MC68010			* if 68000
	add.l	#$03000000,d0	* put vector number in high 8 bits
endc
	move.l	d0,ADDRERR	* Set up vector for address error

* Now set up the vector for the bus error.  If this is an 68010 with
* MMU, vector to the bus error recovery function.  In all other cases,
* vector to doint to abort the process.

ifeq	MMU & MC68010		* if 68000 or no MMU,
	move.l	#addrerr,d0	* treat bus error just like address error
ifne	MC68000			* if 68000
	add.l	#$02000000,d0	* put vector number in high 8 bits
endc
	move.l	d0,BUSERR	* store the vector
endc

ifne	MMU & MC68010		* if 68010 and MMU
	move.l	#pagget,BUSERR	* vector to the page fault handler
endc
	Rts

************************************************************************
*	ADDRERR
*
*	Come here if an address error occurs.  If it occurred in user
*	mode, just jump to _doint to terminate the process.  Otherwise
*	it's very serious and we must call panic.
************************************************************************

ifne	MC68010				* if 68010
addrerr:
	btst	#5,(sp)			* were we in supervisor mode?
endc
ifne	MC68000				* if 68000
addrerr:
	btst	#5,8(sp)		* were we in supervisor mode?
endc
	bne	supmode			* yes - this is VERY bad
	jmp	_doint			* no - it happened in user program


************************************************************************
*	SUPMODE
*
*	Come here if a bus or address error occurs in supervisor mode.
*	Call the panic routine to print a message and halt the machine.
************************************************************************

supmode:
	move.w	#1,-(sp)		* protection error in system
	jsr	_panic			* should never return


***********************************************************************
*	PAGFLT
*
*	Come here to kill a process that's overstepped its boundaries.
*	Let _doint in dispa.s handle the interrupt by terminating the
*	process.
***********************************************************************

Ifne MMU
Ifne MC68010

pagflt:
	movem.l	(sp)+,RGCNTXT		* restore registers
	jmp	_doint			* let _doint handle this error


*******************************************************************
*	PAGGET
*
* Routine to give a process the address space it needs when a bus-
* error indicates that it's tried to access an address that
* isn't currently mapped by the MMU.
*
* The 68010 is nice enough to make a stack frame containing the exact
* address that caused the bus error.  This routine uses that address
* to determine which memory segment in the running process contains
* that address, then calls the OEM's "mmufail" routine to set up what-
* ever MMU registers are necessary to recover from the bus error.
* The mmufail routine returns true (1) if recovery is possible, and
* false (0) if not.  In the former case, we merely return from the
* exception, allowing the 68010 to restart the instruction.  In the latter
* case, we terminate the current process.
*
* If the Bus Error occured while in supervisor mode (check that by ANDing
* the first word on the stack with $2000 - if nonzero, then it was in
* SUP mode...) then it was a legit bus error, and we should do something
* nasty ... one possibility: Complete the instruction ourselves via the
* procedure on page 46 of Motorola M68000 16/32-Bit Microprocessor,
* 4th edition.  Right now, we just kill the system by calling panic.
*
* Register usage:
*	A0	PD pointer
*	A1	MD pointer
*	D0	scratch
*	D1	fault address
*	D2	index to rlr->p_mem[]

pagget:	
	btst	#5,(sp)			* were we in supervisor mode?
	bne	supmode			* yes - this is VERY bad
	Movem.L	RGCNTXT,-(SP)		* Save some registers
	Move.L	70(SP),d1		* Get the fault address
	Move.L	_rlr,A0			* Get the process descriptor
	clr.l	d2			* d2 = index to rlr->p_mem[]
	bra	getpmem			* go get rlr->p_mem[0]
checkmd:
	move.l	a1,d0			* is this MD pointer null?
	beq	nxtpmem			* if so, try next p_mem[] MD pointer
	Move.L	d1,D0			* get fault address
	Sub.L	M_LBASE(A1),d0		* compute offset from base
	Blt	nextmd			* if less than 0, try next MD in chain
	cmp.l	M_LENGTH(a1),d0		* is offset < length of segment?
	blt	found			* yes, we've found right MD
nextmd:
	move.l	M_LINK(a1),a1		* Try next MD in chain
	bra	checkmd
nxtpmem:
	add.l	#4,d2			* index to next rlr->p_mem[i]
getpmem:
	cmp.l	#16,d2			* have we tried all 4 MD lists?
	beq	pagflt			* if no more MDs, page fault
	move.l	a0,a1			* copy PD pointer
	add.l	d2,a1			* add p_mem index
	move.l	P_MEM(a1),a1		* get the MD pointer
	bra	checkmd			* see if it countains fault address
found:					* mmufail(fail,mdptr,rlr)
	move.l	a0,-(sp)		* push the rlr (PD pointer)
	move.l	a1,-(sp)		* push the MD pointer
	move.l	d1,-(sp)		* push the address that caused error
	jsr	_mmufail		* call OEM's routine to recover
	add.l	#12,sp			* pop parameters
	tst.w	d0			* was OEM able to recover?
	beq	pagflt			* no - terminate the process
	Movem.L	(SP)+,RGCNTXT		* yes - restore registers
	Rte				* restart the fault instruction
Endc
Endc

************************************************************************
* LMOVE - move bytes (for blocks bigger then 64K)
*
* Calling convention:
*	BYTE *source,*dest;
*	LONG count;
*	lmove(source,dest,count);
*
* Register usage during dbra loops:
*	D0	byte count
*	D1	long count
*	D2	64K block count
*	A0	source
*	A1	dest
************************************************************************

	.globl	_lmove
_lmove:
	move.l	12(sp),d2		* get byte count
	ble	lret			* if (count <= 0) return
	move.l	4(sp),a0		* get source pointer
	move.l	8(sp),a1		* get dest pointer
	move.w	a0,d0			* compute sum of bits 0 of
	and.w	#1,d0			*  source and dest address
	move.w	a1,d1
	and.w	#1,d1
	add.w	d1,d0			* sum is in d0
	dbra	d0,lm1			* check for zero
	bra	lmove			* it was zero, use long move
lm1:
	dbra	d0,lm2			* check for 1
	bra	bmove			* it was 1, use byte move
lm2:					* it was 2
	move.b	(a0)+,(a1)+		* copy a byte to force word alignment
	sub.l	#1,d2			* and decrement the count
lmove:
	move.l	d2,d0			* copy byte count
	and.w	#3,d0			* compute no. of leftover bytes
	lsr.l	#2,d2			* compute no. of longwords
	move.l	d2,d1			* copy to 64K block count
	swap	d1			* put 64K block count in low word
	bra	endloop1		* go copy the long words first
loop1:
	move.l	(a0)+,(a1)+		* copy a long word
endloop1:
	dbra	d2,loop1		* copy up to 64K bytes
	dbra	d1,loop1		* copy next 64K chunk
	bra	endloop2		* now go copy the leftover bytes
loop2:
	move.b	(a0)+,(a1)+		* copy leftover odd bytes (up to 3)
endloop2:
	dbra	d0,loop2
lret:
	rts

* come here if byte move must be used because of differing alignments
* of source and destination pointers

bmove:
	move.l	d2,d1			* copy byte count
	swap	d1			* compute 64K block count
	bra	endloop3		* go copy some bytes
loop3:
	move.b	(a0)+,(a1)+		* copy a byte
endloop3:
	dbra	d2,loop3		* copy up to 64K bytes
	dbra	d1,loop3		* repeat for next 64K chunk
	rts

	.end
	* copy a byte
endloop3:
	dbra	d2,loop3		* cop