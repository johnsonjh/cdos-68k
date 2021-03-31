************************************************************************
* Copyright (c) 1985, Digital Research, Inc.  All Rights Reserved.  The
* Software Code contained in this listing is proprietary to Digital
* Research, Inc., Monterey, California and is covered by U.S. and other
* copyright protection.  Unauthorized copying, adaptation, distribution,
* use or display is prohibited and may be subject to civil and criminal
* penalties.  Disclosure to others is prohibited.  For the terms and
* conditions of software code use refer to the appropriate Digital
* Research License Agreement.
************************************************************************

************************************************************************
*   Version 4.2		DISPA.S
*			Assembly version of dispatcher (and misc. routines)
*
* IMPORTANT NOTE:
*	This module uses #include, so use CP68 before AS68.
*	The IASM.SUB file does this automatically.
*
*-----------------------------------------------------------------------
* VER	DATE		BY	CHANGE/COMMENTS
*-----------------------------------------------------------------------
* 4.2	06/23/86	DR-K	_divide didn't check overflow from divu
*				instruction. Now call alcyon div routine.
* 4.1	03/20/86	MA	asrwait calls mwait if not in ASR.
* 4.0	03/04/86	MA	Moved expew (user entry point) to SUP68K.S
*				and optimized it a bunch for speed.  Moved
*				tikflg and tikcnt variables to clock driver.
*				Took out uptime and curmpd variables.
* 3.1	11/16/85	MA	Tracked 1.0H changes to ENTRY.A86 for
*				new swi system.
* 3.0	11/10/85	MA	Tracked v1.0H changes to DISP.A86 for new
*				swi system, different UDA layout.
* 3.0	10/22/85	MA	Tracked new Frank changes to disptcher.
*				Fixed trace exception handling to not
*				allow tracing of supervisor code.
* 2.0	09/13/85	MA	Converted Frank's asr and disp stuff
*				in DISP.A86 to 68K assembly.  Moved equates
*				to separate file. Took out swi fix in
*				gotopgm, added _rte symbol.
*				Changed name of drdos to _drdos.
*				Moved call to _tikinit after call to _dinit.
*				Put _breakpt back in.  Added panic.
*				Changed to include "cpummu.equ".
*******************************************************************

#include "struct.equ"		* this is why you need CP68 to assemble
#include "panic.equ"		* and this too
#include "cpummu.equ"		* and this too

ASMDISP	equ	1		* Was DISP converted to assembler?

PRIVEXC	equ	$20		* Exc.Vector address for privilege violation
TRACEXC	equ	$24		* Exc.Vector address for trace exception
TRAP2	Equ	$88		* Exc.Vector Address for Trap #2
TRAP14	Equ	$b8		* Exc.Vector Address for Trap #14

RGCNTXT	Reg	D0-D7/A0-A6	* Context switch registers to save
RGDISP	Reg	D3-D7/A3-A6	* Registers to save in _dsptch
RGBDOS	Reg	D1-D2/A0-A2	* __osif registers to save
RGSTJMP	Reg	D3-D7/A2-A7	* Jump area registers to save

* External variables

	.globl	_indisp,_infork,_rlr,_plr,_prl,_pdx
	.globl	_asrrun,_asrfree,_bdap,_cda,_fep,_memsync

* External functions

ifne	NEWSWI
	.globl	_goswi
endc
ifeq	ASMDISP
	.globl	_disp
endc
	.globl	_tik_open,_tik_ini
	.Globl	_user_ent,_mmuload,_dinit,_minit,_initmmu,_tik_ini
	.globl	_feint,_terminate,_freepd,__evdone,_faret,_iunsyn
	.globl	_initvec,__bzero
	.globl	asrmget,asrinser,_asrinit,_mwait
	.globl	_showreg
	.globl	expew,cpmtrap,ldiv

	.text

*******************************************************************
*	_DRDOS
*
*	Main Entry point for the operating system
*******************************************************************

	.globl	_drdos
_drdos:
	or.w	#$700,sr	* Disable interrupts for initialization
	Move.L	#dspstk,SP	* Get the new stack
	jsr	_initvec	* call OEM's vector initialization routine
	Move	#1024,-(SP)	* clear out the interrupt page
	Move.L	#IPAGE,-(SP)
	Jsr	__bzero
	Add.L	#6,SP
	jsr	_initmmu	* Initialize OEM's MMU

Ifne	MC68010
	move.l	#privcod,PRIVEXC	* set up vector for privilege exception
endc
	move.l	#trace,TRACEXC	* set up vector for trace exception
	Move.L	#expew,TRAP14	* Set up vector for supervisor calls
	move.l	#cpmtrap,TRAP2	* set up vector for CP/M calls
	Move.w	#1,_indisp	* Signal IN DISPATCHER
	Jsr	_minit		* Initialize the memory manager (non-oem)
	jsr	_asrinit	* initialize the free asr list (vmconfig.c)
	Jsr	_dinit		* Initialize dispatcher
	jsr	_tik_ini	* initialize OEM's timer
	Move.L	_rlr,A0		* Save the stack pointer
	Move.L	p_uda(A0),A0	*  in the UDA of the dispatcher task
	Move.L	SP,u_stk0(A0)
	Jsr	_tik_open	* Start ticking
	and.w	#$f0ff,sr	* Enable interrupts
	Jsr	_mdsptch	* Go start the first task

*******************************************************************
*	BREAKPT
*
*	Call this routine for debugging purposes only.  It
*	causes an illegal instruction exception, which gets
*	us into the debugger.
*******************************************************************

	.globl	_breakpt
_breakpt:
	illegal
	rts


*******************************************************************
*	PRIVCOD
*
*	This code is a kludge put in to handle the MOVE SR,xx
*	instruction, which is privileged.  The Alcyon compiler
*	generates this instruction quite a lot, so this routine
*	patches any programs that try to execute it by changing
*	it to a MOVE CCR,xx.
*
*	This code could be extended to handle other
*	instructions that cause privilege violations.  Right
*	now it just terminates the process if that happens.
*
*	Stack format after register saves:
*	18(sp)	Format/vector word
*	16(sp)	low PC
*	14(sp)	high PC
*	12(sp)	status register
*	10(sp)	low A1
*	 8(sp)	high A1
*	 6(sp)	low A0
*	 4(sp)	high A0
*	 2(sp)	low D0
*	 0(sp)	high D0
*******************************************************************

ifne	MC68010
privcod:
	movem.l	d0/a0-a1,-(sp)		* save d0,a0,a1
	move.l	14(sp),a0		* get saved program counter
	move.l	_rlr,a1			* get process descriptor pointer
	move.l	p_mem(a1),a1		* get pointer to code LMD
	sub.l	m_lbase(a1),a0		* subtract logical code base from PC
	add.l	m_start(a1),a0		* add phys. base to PC to get phys addr
	move.w	(a0),d0			* get the bad instruction
	and.w	#$ffc0,d0		* isolate the opcode portion
	cmp.w	movesr,d0		* "move sr,xx" instruction?
	beq	patchit			* yes - go patch the program
	movem.l	(sp)+,d0/a0-a1		* no - restore d0,a0,a1
	jmp	_doint			* let doint handle this one
patchit:
	move.w	(a0),d0			* get the bad instruction back
	and.w	#$3f,d0			* isolate the effective address bits
	add.w	moveccr,d0		* change it to a "move ccr,xx"
	move.w	d0,(a0)			* patch it over bad instruction
	movem.l	(sp)+,d0/a0-a1		* restore d0,a0,a1
	rte				* try again with new instruction
movesr:
	move	sr,d0			* bad instruction
moveccr:
	move	ccr,d0			* good instruction
endc

*******************************************************************
*	TRACE
*
*	This bit of code handles the trace exception.  If the
*	exception occurred while the processor was in supervisor
*	state, a debugger must have been trying to trace a
*	TRAP instruction.  In this case, the trace bit in the
*	stacked status register is turned off and we just RTE
*	right out again.  This will allow the TRAP to proceed
*	normally, and then when the TRAP handler does its RTE,
*	the TRACE bit will get turned on again, in user mode this time.
*******************************************************************

trace:
	bclr	#7,(sp)			* clear the stacked trace bit
	btst	#5,(sp)			* did trace occur in sup mode?
	beq	_doint			* no - handle normally
	rte				* yes - ignore trace for now


*******************************************************************
*	POKE(addr,val)
*
*	Poke the address with the byte value. This works in real
*	mode and may have to be changed for protected mode.
*******************************************************************

	.Globl	_poke
_poke:	Move.L	4(SP),A0	* Get address
	Move.B	9(SP),(A0)	* Put byte at that address.
	Rts

*******************************************************************
*	quotient=UDIV(devisor,divident,*remainder)
*******************************************************************

	.Globl	_divide

_divide: link	A6,#0
	clr.l	D0
	move.w	12(A6),D0	* get the word dividend onto the stack
	move.l	D0,-(SP)
	move.l	8(A6),-(SP)	* put the divisor onto stack last
	jsr	ldiv		* call alcyon's library divide routine
	add.l	#8,sp		* clean stack
	move.l	14(SP),A0
	move.w	D1,(A0)		* return word remainder at pointer
	unlk	A6
	rts

*******************************************************************
*	UBWORD byte
*
*	Change the byte argument to a word and return it
*******************************************************************

	.Globl	_UBWORD
_UBWORD:
	Clr	D0
	Move.B	5(SP),D0
	Rts

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
*	CLI - Disable interrupts, return old interrupt mask
*
*	Old status register value returned in D0, all other
*	registers left unchanged.  Use the value returned by CLI
*	as the parameter to the next STI.
*******************************************************************

	.Globl	_cli,_no_isr
_no_isr:
_cli:
	Move	SR,D0			* get old interrupt mask
	Or	#$700,SR		* Mask off all interrupts
	and	#$700,d0		* return old interrupt mask
	Rts

*******************************************************************
*	STI(MASK) - Restore value of interrupt mask returned by CLI
*******************************************************************

	.Globl	_sti,_ok_isr:
_ok_isr:
_sti:
	move.w	4(sp),d1		* get old interrupt mask
	and.w	#$700,d1		* isolate the interrupt bits
	move.w	sr,d0			* get current status register
	and.w	#$f0ff,d0		* zero out current interrupt bits
	or.w	d1,d0			* insert new mask
	move.w	d0,sr			* and make this the new status reg
	Rts

*******************************************************************
*	GOTOPGM(&REGSAV)
*
*	After loading in a new process, start executing it,
*	using register values passed in REGSAV area
*******************************************************************

	.Globl	_gotopgm,_rte
_gotopgm:
	or.w	#$700,sr		* Mask off all interrupts
	move.l	4(sp),a0		* pointer to REGSAV area
	move.l	_rlr,a1			* Get process descriptor
	move.w	p_flag(a1),d0		* get process flags
	and.w	#PF_INSWI,d0		* reset to swi stack?
	beq	gpgmsys			* no - use system stack
	move.l	p_swistk(a1),sp		* yes - use swi stack
	bra	gpgmstk
gpgmsys:
	move.l	p_systak(a1),sp		* get base of supervisor stack
gpgmstk:
	add.w	p_stksiz(a1),sp		* reset SP to original top of stack
	Move.L	r_usp(A0),A3		* get user stack pointer
	Move	A3,USP			* restore user stack pointer
Ifne MC68010				* if 68010, then put on format word
	Clr	-(SP)
Endc
	Move.L	r_pc(A0),-(SP)		* push program counter
	Move	r_sr(A0),D0		* get status register
	And	#$80FF,D0		* clear Sup bit and interrupt mask
	Move	D0,-(SP)		* push the status register
	Movem.L	r_dreg(A0),RGCNTXT	* restore all registers
_rte:
	Rte				* go to user program


*******************************************************************
* usergo(addr) -- execute in user mode
*******************************************************************

	.Globl	_usergo
_usergo:

*	Clr	-(SP)
*	Move.L	6(SP),-(SP)

	Move.L	4(SP),-(SP)

Ifne MC68010
	Clr	-(SP)		* If 68010, then put on format word
Endc
	Clr	-(SP)		* put 0 on stack for status register
	Rte			* whoosh

*******************************************************************
*	TAS - Test and set Mutual Exclusion (MUTEX)
*
*	The MUTEX is a WORD!
*******************************************************************

	.Globl	_TAS
_TAS:	Move.L	4(SP),A1
	Move	(A1),D0
	Move	#-1,(A1)
	Rts

*******************************************************************
*	SETDS(address)
*
*	I think that this sets the DS register to the nearest seg
*	that contains the address, but I'm not sure. In any event,
*	that doesn't apply much to the 68000. SETDS also used to
*	set the 'IN SUPERVISOR' bit in the UDA, I don't know why!??
*******************************************************************

	.Globl	_setds
_setds:
*	Move.L	4(SP),A0		* Get the addr of the UDA
*	Move	#1,u_insys(A0)		* Set the bit
	Rts

*******************************************************************
*	ASRWAIT - asrwait(event_number,stack_save_area)
*
*	While in an ASR, if an event is created, the ASR can wait for it
*	through ASRWAIT.  ASRWAIT will copy a portion of the dispatcher
*	stack into the stack_save_area and schedule an ASR to run upon
*	completion of the event.  The scheduled ASR will "return" to the
*	calling ASR with its stack restore.  No parameters are sent
*	and no return code is returned.
*******************************************************************

	.globl	_asrwait

_asrwait:
	tst.w	_infork			* are we in an asr?
	bne	awinfork		* yes - don't call mwait
	jmp	_mwait			* no - jump to mwait, it'll clean up
awinfork:	
	link	a6,#0			* set up stack frame pointer
	movem.l	RGDISP,-(sp)		* save register variables and A6
	move.l	8(a6),a0		* get event (pointer to ASR structure)
	move.l	a0,d0			* test it
	bne	awevent			* non-zero, they really want to wait

* No event, the caller really wants an ASR reschedule, so let's fake it...

	jsr	asrmget			* get an ASR, returned in A0
	move.l	a0,8(a6)		* save it in the stack
	move.b	ASRF_FAKE,asr_flags(a0)	* it's a fake, no aret later
	move.b	asrprior,asr_prior(a0)	* set its priority
	jsr	asrinser		* insert it in asrrun list
	move.l	8(a6),a0		* get it back again
awevent:
	move.l	12(a6),a1		* get stack_save_area
	move.l	a1,asr_stk(a0)		* save it for later
	move.l	asrstack,d0		* get base of this ASR's stack
	sub.l	sp,d0			* subtract current stack pointer
	lsr.w	d0			* no. of stack words used by this ASR
	move.w	d0,asr_stklen(a0)	* save no. of stack words
	sub.w	#1,d0			* decrement for loop
awloop:
	move.w	(sp)+,(a1)+		* copy a stack word to save area
	dbra	d0,awloop		* repeat
	or.b	#ASRF_STK,asr_flags(a0)
	move.b	asrprior,asr_prior(a0)
	jmp	run_asr

*******************************************************************
*	SCHED (p)
*	PD *p;
*
*	Insert a process into the PRL in priority order.
*******************************************************************

	.globl	sched,_sched
_sched:					* C entry point
	move.l	4(sp),a0		* get PD pointer
sched:					* ASM entry point - pd in A0
	move.l	#_prl,a1		* get offset of PRL
	move.b	p_prior(a0),d0		* get process's priority
	move.b	p_flag(a0),d1		* get its flag
	and.b	#PF_RESOURCE,d1		* isolate the RESOURCE flag setting
	move.b	#PS_RUN,p_stat(a0)	* set process status to running
ifne	p_link
	sub.l	#p_link,a1		* adjust root pointer so loop works
endc

ip_next:
	move.l	a1,a2			* save previous pointer in A2
	move.l	p_link(a1),a1		* get next PD in list into A1
	move.l	a1,d2			* at end of list?
	beq	ip_linkit		* yes - link it here
	cmp.b	p_prior(a1),d0		* compare priorities
	bcs	ip_linkit		* our priority is less than PD in list
	bhi	ip_next			* our priority is greater
	tst.b	d1			* same priority - RESOURCE flag on?
	beq	ip_next			* no - keep looking
ip_linkit:
	move.l	a1,p_link(a0)		* link the next process
	move.l	a0,p_link(a2)		* link the prev process
	and.b	#$ff-PF_RESOURCE,p_flag(a0)	* turn off RESOURCE flag
	rts

*******************************************************************
*	DSPTCH
*
*	First increment the indisp counter.  If it is 1 after
*	the increment, then it's OK to dispatch.  Otherwise
*	just return, because dispatches are currently disabled.
*
*	Save the "C" register context of the current process,
*	then call _disp to figure out the next process to run.
*	Finally restore the register context of the new process
*	and return to it.
*
*	_mdsptch is an alternate entry point that doesn't check
*	the indisp counter.  
*******************************************************************

	.Globl	_dsptch,_mdsptch
_dsptch:
	add.w	#1,_indisp		* bump number of guys in dispatcher
	cmp.w	#1,_indisp		* were we already in the dispatcher?
	bne	no_disp			* yes - can't dispatch now
_mdsptch:
	move.w	sr,d0			* save interrupt mask
	or.w	#$700,sr		* disable interrupts
	move.l	_rlr,a0			* get PD of current process
	cmp.b	#PS_RUN,p_stat(a0)	* process running?
	bne	do_disp			* nope -- it's OK to dispatch
	move.l	_plr,d1			* check poll list
	or.l	_asrrun,d1		* and asr run list
	bne	do_disp			* there's an asr or poll - go do it
	move.l	_prl,d1			* check first guy on process ready list
	beq	restint			* no ready processes
	move.l	d1,a1			* A1 = first ready process
	move.b	p_prior(a1),d1		* get priority of first ready process
	cmp.b	p_prior(a0),d1		* is rlr's priority better than prl's?
	bls	do_disp			* no - OK to dispatch
restint:
	move.w	d0,sr			* restore interrupt mask
no_disp:
	Sub.w	#1,_indisp		* unable to dispatch now - return
	Rts
do_disp:				* interrupts must be disabled here
	move.l	p_uda(a0),a1		* get UDA of current process
	movem.l	RGDISP,u_dreg(a1)	* save D3-D7/A3-A6 in UDA
	move.l	sp,u_stk0(a1)		* save supervisor SP in required place
	move.l	usp,a2			* get user stack pointer
	move.l	a2,u_usp(a1)		* save user stack pointer
	move.l	#_pdx,_rlr		* dispatcher is now the current process
	move.l	#dspstk,sp		* switch to dispatcher stack
	move.w	d0,sr			* re-enable interrupts
	move.b	p_stat(a0),d1		* get status of old rlr
*
* TRY_RUN
* =======
*
try_run:
	cmp.b	#PS_RUN,d1		* does it want to run again?
	bne	try_mwait		* no - see if it wants to wait
no_mwait:				* yes - process wants to run again
	bsr	sched			* reinsert it in ready list
	bra	run_asr			* then go check for asrs to run
*
* TRY_MWAIT
* =========
*
try_mwait:
	cmp.b	#PS_MWAIT,d1		* process wants to wait?
ifne	NEWSWI
	bne	try_restore		* no - see if need to restore context
endc
ifeq	NEWSWI
	bne	try_term		* no - see if he wants to die
endc
is_mwait:
	move.l	p_evbits(a0),d0		* p->evwait &= p->evbits
	and.l	d0,p_evwait(a0)
ifeq	NEWSWI
	tst.l	p_swipend(a0)		* if ((!p->p_swipend) &&
	bne	do_mwait
endc
	move.l	p_evwait(a0),d0		*    ((!p->p_evwait) ||
	beq	no_mwait
	and.l	p_evflg(a0),d0		*    (p->evwait & p->evflg)))
	bne	no_mwait		*    { sched(p); break; }
do_mwait:
	or.b	#PF_RESOURCE,p_flag(a0)	* set resource process flag
	bra	run_asr			* go check for asrs to run
*
* TRY_RESTORE
* ===========
*
ifne	NEWSWI
try_restore:
	cmp.b	#PS_RESTORE,d1		* need to restore saved context?
	bne	try_term		* no - see if it needs to terminate
*
* restore this process's previous context as saved in FORCERUN()
*
	move.l	a1,a2			* copy uda pointer to a2
	lea	u_dsav(a1),a2		* get address of saved uda registers
	move.w	#10,d1			* need to copy 11 registers
restloop:
	move.l	(a2)+,(a1)+		* copy one register
	dbra	d1,restloop		* keep going for all 11
	move.l	p_uda(a0),a1		* get back uda pointer
	move.w	p_flag(a0),d1		* get process flags
	and.w	#$ffff-PF_INSYS,d1	* turn off INSYS flag
	or.w	u_pflags(a1),d1		* restore saved state of INSYS flag
	move.w	d1,p_flag(a0)		* and store in process flags
	move.l	p_savwait(a0),d1	* get saved event flags to wait for
	beq	rest_run		* nothing to wait for, let's run
	move.l	d1,p_evwait(a0)		* restore event flags to wait for
	move.b	#PS_MWAIT,p_stat(a0)	* this process must wait
	bra	is_mwait
rest_run:
	move.b	#PS_RUN,p_stat(a0)	* this process can run
	bra	no_mwait
endc
*
* TRY_TERM
* ========
*
try_term:
	cmp.b	#PS_TERM,d1		* process trying to terminate?
	bne	try_block		* no - p_stat is bad
	move.l	a0,-(sp)
	jsr	_freepd			* yes - free up the PD
	move.l	_memsync,(sp)		* also release memsync
	jsr	_iunsyn			*  if we have it
	add.l	#4,sp			* pop parameter
	bra	run_asr			* go see if ASRs need to run
try_block:
	move.b	#PS_BLOCK,p_stat(a0)	* block forever if illegal p_stat
*
* RUN_ASR
* =======
* Run all scheduled ASRs.  It's OK to forget about the old PD now...
*
run_asr:
	move.w	sr,d0			* save interrupt mask
	or.w	#$700,sr		* disable interrupts
	move.l	sp,asrstack		* save asr stack pointer
	move.l	_asrrun,a0		* get first ASR on list
	move.l	a0,d1			* anything there?
	bne	do_an_asr		* yes - do it
	move.w	d0,sr			* no - re-enable interrupts
	bra	run_poll		* and go do some polling
do_an_asr:
	move.w	#1,_infork
	move.l	asr_link(a0),_asrrun	* unlink the ASR from ASRRUN
	move.l	asr_code(a0),_curasr	* save code pointer
	move.w	d0,sr			* re-enable interrupts
	move.b	asr_prior(a0),asrprior	* save the ASR's priority
	move.b	asr_flags(a0),d0	* get the ASR flags
	move.b	d0,d1			* make two copies
	and.b	#ASRF_STK,d0		* is the STK bit set?
	beq	no_asr_stk		* no - just run it plain and simple
	and.b	#ASRF_FAKE,d1		* is the FAKE bit set?
	beq	notfake			* no - they really waited for an event
	clr.l	d0			* yes - fake return code
	bra	got_aret
notfake:
	move.l	a0,-(sp)		* save the ASR pointer for later
	move.l	a0,-(sp)		* ret = faret(asr)
	jsr	_faret
	add.l	#4,sp
	move.l	(sp)+,a0		* restore ASR pointer from long ago
got_aret:
	move.w	sr,d0			* save interrupt mask
	or.w	#$700,sr		* disable interrupts
	move.w	asr_stklen(a0),d1	* stack length in words
	move.l	asrstack,a1		* get old base of ASR stack
	sub.w	d1,a1			* adjust for stack size
	sub.w	d1,a1			* add twice for byte count
	move.l	a1,sp			* this is our new stack pointer
	move.l	asr_stk(a0),a2		* get address of ASR stack_save_area
	sub.w	#1,d1			* decrement word count for loop
gotloop:
	move.w	(a2)+,(a1)+		* copy stack from stack_save_area
	dbra	d1,gotloop		*  to dispatcher stack

* stack is now as in ASRWAIT at stack save:
*	A7 ->	D3-D7 A3-A6 Old A6 RET ASR STK_SAVE
*		0     20    36     40  44  48

	movem.l	(sp)+,RGDISP		* restore register variables
	move.l	8(a6),a0		* restore ASR pointer
	move.l	_asrfree,asr_link(a0)	* free the ASR
	move.l	a0,_asrfree
	unlk	a6
	move.w	d0,sr			* reenable interrupts
	rts				* resume in the ASR that waited

no_asr_stk:
	move.l	a0,-(sp)		* save the ASR pointer for later
	move.l	asr_2data(a0),-(sp)	* pass the two parameters
	move.l	asr_1data(a0),-(sp)
	move.l	asr_code(a0),a0		* get the code address
	move.l	a0,d1			* is it zero?
	beq	badasr			* yes - call panic
	jsr	(a0)			* call the ASR
asrrecov:
	add.l	#8,sp
	move.l	(sp)+,a0		* restore the ASR pointer
	move.w	sr,d0			* save interrupt mask
	or.w	#$700,sr		* disable interrupts
	move.l	_asrfree,asr_link(a0)	* free the ASR
	move.l	a0,_asrfree
	move.w	d0,sr			* reenable interrupts
	bra	run_asr			* go run the next ASR

* Attempting to call an ASR at 0.  This usually
* means failure to call NEXTASR from an ASR that
* caused an event.

badasr:
	move.l	asr_stk(a0),d0		* D0 is offending ASR
	move.l	asr_evb(a0),d1		* D1 is offending EVB
	move.w	#XX_NEXTASR,-(sp)
	bsr	_panic			* this should never return
	bra	asrrecov		* but just in case...
*
* RUN_POLL
* ========
* Call all Poll Routines
*
run_poll:
	move.w	#2,_infork
	move.l	_plr,a0			* get first poll event on list
next_poll:
	move.l	a0,d0			* any more poll events?
	beq	inc_idle		* nope
	move.l	_prl,d0			* get first process on ready list
	beq	do_poll			* no ready processes, skip this
	move.l	d0,a1			* put process into A1
	move.b	p_prior(a1),d0		* get process's priority
	cmp.b	e_prior(a0),d0		* process's prior better than event's?
	bls	inc_idle		* yes - don't bother running poll
do_poll:
	move.l	e_link(a0),-(sp)	* save next poll event
	move.l	a0,-(sp)		* save this poll event
	move.l	e_parm(a0),a0		* get code address of poll routine
	move.w	sr,-(sp)		* save the interrupt mask
	jsr	(a0)			* call the poll routine
	move.w	(sp)+,sr		* restore the interrupt emask
	move.l	(sp)+,a0		* restore poll event
	tst.w	d0			* poll routine returned TRUE?
	beq	end_poll		* no - go check next event
	move.l	a0,-(sp)		* _evdone(event)
	jsr	__evdone		* this takes the pollevent
	add.l	#4,sp			*  off the plr
end_poll:
	move.l	(sp)+,a0		* get pointer to next event
	bra	next_poll		* no - keep polling
*
* INC_IDLE
* ========
*
inc_idle:
	clr.w	_infork			* no longer in asr land
	add.l	#1,_idlecnt		* increment idle count
	move.w	sr,d0			* save interrupt mask
	or.w	#$700,sr		* disable interrupts yet again
	move.l	_prl,d1			* anything on the prl?
	bne	test_asr		* yes - go run him
go_back:
	move.w	d0,sr			* enable interrupts
	bra	run_asr			* and go check for asrs to run
test_asr:
	tst.l	_asrrun			* any asrs to run?
	bne	go_back			* yes - run them first
	move.l	d1,a0			* no - run the first guy on the prl
	move.l	p_link(a0),_prl		* unlink him from the prl
	move.w	d0,-(sp)		* save interrupt mask
	move.l	a0,-(sp)		* and the pd
	move.l	p_curmem(a0),-(sp)	* pass memory context pd to mmuload
	jsr	_mmuload		* call OEM's routine to load the MMU
	add.l	#4,sp			* pop the parameter to mmuload
	move.l	(sp)+,a0		* restore the pd pointer
	move.w	(sp)+,d0		* restore the saved interrupt mask
	move.l	p_bda(a0),_bdap		* set up the file system pointer
	move.l	p_cda(a0),_cda		* and the console system pointer
	move.l	p_feptr(a0),_fep	* and the front end data area pointer
disp_end:				* A0 = new PD, D0 = old interrupt mask
	move.l	a0,_rlr			* make this the current process
	move.l	p_uda(a0),a0		* Get the UDA for this process
	move.l	u_usp(a0),a1		* get old user stack pointer
	move.l	a1,usp			* restore the usp
	move.l	u_stk0(a0),sp		* Restore old supervisor stack pointer
	movem.l	u_dreg(a0),RGDISP	* And get the rest of the regs
	clr.w	_indisp			* allow dispatches
	move.w	d0,sr			* allow interrupts again
	rts				* return where we left off

*******************************************************************
*	SETJMP(env)
*
*	Store the return address from this routine into the space
*	pointed to by ENV and return 0. This routine is used to set
*	up a wery wong jump for LONGJMP.
*******************************************************************

	.Globl	_setjmp
_setjmp:
	Move.L	4(SP),A0		* Get address of jump buffer
	Movem.L	RGSTJMP,(A0)		* Store most of the registers
	move.l	(sp),44(a0)		* save PC at end of jump buffer
	Clr.L	D0			* Make a SETJMP return
	rts				* return to caller

*******************************************************************
*	LONGJMP(env,ret)
*
*	Return to the address set by SETJMP and stored in ENV, but
*	have SETJMP (which we're returning to) return RET instead
*	of 0 this time.
*******************************************************************

	.Globl	_longjmp
_longjmp:
	Move.L	4(SP),A0		* Get address of jump buffer
	Move.L	8(SP),D0		* Set up return value
	Movem.L	(A0),RGSTJMP		* Restore most of the registers
	move.l	44(A0),(sp)		* Restore the PC to the stack
	Rts				* Return from SETJMP again.

*******************************************************************
*	SETVEC(address,vector)
*
*	Set the interrupt vector so that it calls ADDRESS when
*	tripped. Sets the list of addresses in IPAGE.
*******************************************************************

	.Globl	_SETVEC,_setvec
_setvec:
_SETVEC:
	Move.L	8(SP),A0		* A1 = 4*vec#
	Add.L	A0,A0
	Add.L	A0,A0
	Move.L	A0,A1
	Add.L	#IPAGE,A0		* A2 = IPAGE+4*vec#
	Move.L	(A0),D1			* Get the old ISR address (0 if none)
	Move.L	4(SP),(A0)		* Set the new ISR.

	clr.l	d0
Ifeq MC68010				* if 68000, put vector # in upper byte
	move.l	8(sp),D0		* get vector number
	ror.l	#8,d0			* move it to upper 8 bits.
Endc
	Add.L	#_doint,D0		* Make a full address from it.
	Move.L	D0,(A1)			* And put it away.
	Move.L	D1,D0			* Return old IVEC address.
	Rts				* That's it!

*******************************************************************
*	LONG GETINT(intnum)
*	WORD intnum
*
*	This function returns the interrupt vector indicated by
*	the intnum parameter.  What is returned is not the actual
*	vector (which may not even be initialized), but the address
*	that a C program can jump to to simulate an interrupt
*	(after pushing the appropriate stack frame info.)
*******************************************************************

	.globl	_getint
_getint:
	clr.l	d0
ifeq	MC68010				* if MC68010 == 0
	move.w	4(sp),d0		* get the vector number
	ror.l	#8,d0			* put it in the high 8 bits
endc
	add.l	#_doint,d0		* doint is the real handler
	Rts

*******************************************************************
*	DOINT
*
*	Come here for all interrupts except bus and privilege errors.
*	Figure out what the interrupt number is by either
*	looking at the vector offset word on the stack (68010), or
*	the high 8 bits of the program counter (68000 kludge).
*	Use the interrupt number as in index into IPAGE, a table
*	of interrupt service routine addresses.  If the IPAGE entry
*	is zero, the interrupt isn't being handled by an ISR, so
*	call the front end interrupt handler (feint) to figure out
*	what to do.  Otherwise, call the ISR, then check if the
*	ISR wants a dispatch.  If it does, and this
*	is not a nested interrupt (i.e. we're not
*	interrupting an interrupt), then do a dispatch before
*	returning to the interrupted program.
*******************************************************************

	.globl	_doint
_doint:
	Movem.L	RGCNTXT,-(SP)		* Save system state on stack

Ifne MC68010				* if 68010
	Move	66(SP),D1		* Get format word from stack frame
Endc
Ifeq MC68010				* if 68000
	bsr	dave			* find out our current PC
	nop				* can't bsr to next instruction
dave:					* this is Dave Clarke's idea
	move.w	(sp)+,d1		* get high word of current PC
	add.l	#2,sp			* get rid of the low word of the PC
	lsr	#6,d1			* shift the vector number down
Endc

	and.l	#$3fc,d1		* Strip off everything but vector
	move.l	#IPAGE,a0		* Get ISR address into A0
	move.l	0(a0,d1),a0
	lsr.l	#2,d1			* convert vector offset into int num
	move.l	a0,d0			* Is this a hardware interrupt?
	beq	softint			* No, go do a software interrupt
	move.l	d1,-(sp)		* pass interrupt number to ISR
	jsr	(a0)			* Call the ISR
	add.l	#4,sp			* pop the parameter
	or.w	d0,dodisp		* Set the dodisp flag returned by ISR
	or.w	#$700,sr		* Disable ints while we check dodisp
	move.w	60(sp),d0		* get old interrupt mask
	and.w	#$700,d0		* isolate the mask bits
	bne	iret			* If nested, don't dispatch yet
	tst.w	dodisp			* Did any of the ISRs want dispatches?
	beq	iret			* Nope, skip the dispatch
	clr.w	dodisp			* Yep, clear the dodisp flag
	move.w	#$2000,sr		* enable interrupts
	bsr	_dsptch			* Do a dispatch
iret:
	movem.l	(sp)+,RGCNTXT		* Restore all them registers
	rte				* Return to interrupted task

* Come here for a software interrupt - either a front end or a trap
* or something that doesn't have a vector set up.  Save the registers
* in the process's REGSAV area, then call the front end interrupt handler.
* The front end returns a pointer to the REGSAV on return, so we
* exit via GOTOPGM instead of an RTE.
*
* Note that D1 contains the interrupt number, and that D0-D7/A0-A6,
* and the exception stack frame are all on the stack.
* These stack items are all popped before the front end is called.

softint:
	move.l	_rlr,a0			* get address of process descriptor
	move.l	p_regsav(a0),a0		* get address of REGSAV area
	move.w	#14,d0			* loop count for 15 registers
loop1:
	move.l	(sp)+,(a0)+		* pop D0-D7/A0-A6 into REGSAV area
	dbra	d0,loop1

	move.l	usp,a1			* save user A7 into REGSAV
	move.l	a1,(a0)+
	move.w	(sp)+,4(a0)		* pop status register into REGSAV
	and.w	#$7fff,4(a0)		* clear the trace flag
	move.l	(sp)+,(a0)		* pop program counter into REGSAV
ifne	MC68010
	tst.w	(sp)+			* get rid of format/vector word
endc
	move.w	d1,-(sp)		* push it as parameter to feint
	jsr	_feint			* call front end interrupt handler
	add.l	#2,sp			* pop the parameter
	move.l	d0,-(sp)		* pass address of REGSAV to gotopgm
	bsr	_gotopgm		* goto user program - shouldn't return,

*******************************************************************
*	GOUSER
*
*	Switch the stacks to the system stack pursuant to returning
*	to the dispatcher. Of course, since the 68000 does all this
*	for you, there seems little point in it ... but nevertheless.
*******************************************************************

gouser:	Rts


*******************************************************************
*	PANIC - print a message, dump registers, stop the machine
*
*	Note: the USP is not saved.  The value that is saved for
*	A7 is the SSP.  In the future, we can expand the REGSAV
*	definition in struct.equ and struct.h to hold more stuff.
*******************************************************************

	.globl	_panic
_panic:
	move.w	sr,panreg+r_sr		* save the status register
	or.w	#$700,sr		* disable interrupts
	movem.l	D0-D7/A0-A6,panreg	* save most registers
	move.l	(sp),panreg+r_pc	* save the return address
	move.w	4(sp),d0		* get the panic code for later use
	move.l	sp,d1			* get the stack pointer
	add.l	#6,d1			* adjust for stuff that got pushed
	move.l	d1,panreg+r_usp		* save the sup. stack pointer
	move.l	#panreg,-(sp)		* pass the address of regsave area
	move.w	d0,-(sp)		* pass the panic code ...
	jsr	_showreg		* ... to a routine that prints stuff
	add.l	#6,sp			* pop parameters
blorch:	bra	blorch			* kill the machine
	movem.l	panreg,D0-D7/A0-A6	* restore registers
	move.w	panreg+r_sr,sr		* restore the status register
	rts				* should never get here, but...


*******************************************************************
*	Data Segment - Initialized Data
*******************************************************************

		.data
	
		.globl _idlecnt

_idlecnt:	.dc.l	0
dodisp:		.dc.w	0			* true if ISR wants a dispatch

*******************************************************************
*	BSS Segment - Uninitialized Data
*******************************************************************

		.bss

		.globl	_curasr
_curasr:	.ds.l	1		* currently executing asr
asrstack:	.ds.l	1
asrprior:	.ds.b	1
asrfill:	.ds.b	1
panreg:		.ds.b	$80		* REGSAV area for panic

		.ds.l	$400
dspstk:		.ds.l	1		* Default Supervisor Stack

IPAGE:		.ds.l	256		* Interrupt vector jump table
vectab:		.ds.l	256		* The actual interrupt vector table

		.end
 Stack

IPAGE:		.ds.l	25