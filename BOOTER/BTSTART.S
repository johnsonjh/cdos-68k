*	@(#)btstart.s	1.7	
*
*	This is the Concurrent DOS-68K boot entry for
*	the VME/10.
*
*	This code (plus the main C source of the booter)
*	resides in the hidden area of the file system of a
*	floppy disk or a partition on disk.
*
*	We expect the registers to be set up by TENbug.  See comment
*	in initialization below.
*
*	As of this version of the boot loader, we are loaded into
*	high memory, while the system is loaded on the VME/10 at the
*	base of B00 (above TENbug work space RAM).  You could reverse
*	them and play games.  Try not to clash them in memory.
*	We need textsize+datasize+bsssize+STKSIZ for our memory.
*	We're not very small in memory, but we're very nice.
*
*	This booter resides in a PC-DOS partition (or full disk
*	partition) or on a floppy disk.
*

	.globl	_bootrec	* The real boot record (swapped and misaligned)
	.globl	_Boot
	.globl	_disknum
	.globl	_strtbtstr
	.globl	_endbtstr
	.globl	_Exit
	.globl	_SetAndGo

	.globl	_end		* loader supplied values NOT RELOCATED!
	.globl	_edata

*	Stack stuff.  Our stack is STKSIZ big, we add that to
*	_end, the C pseudo variable, whose address lies just beyond
*	the BSS section of our program.

	STKSIZ = 20000

	.text

*******************************************************************
*
*	Boot loader entry point.
*
*******************************************************************

*	Branch over the Boot Record - actually part of record

_bootrec:

*	Define the Boot Record - the information is filled in by boot
*	loader install program.
*
*	Note: this funky block with its misaligned words was defined
*	for the Intel chips in MS-DOS, it is obviously a problem with
*	the M68010 and M68000.  We'll create a valid block later which
*	we can work with from this information.

	.dc.b	0,0,0			* 3 byte Intel short jump, unused
	.dc.b	0,0,0,0,0,0,0,0		* 8 byte OEM id

*	Embedded in the Boot Record is the BIOS Parameter Block (BPB)
*	which defines the structure of the PC-DOS file system.
*	We'll define all storage allocation as a sequence
*	of bytes (.dc.b 0,0 == WORD), to avoid any alignment hassles.

	.dc.b	0, 0			* nbr of bytes per sector
	.dc.b	0			* number of sectors per block (cluster)
	.dc.b	0, 0			* number of reserved sectors
	.dc.b	0			* number of File Allocation Tables (FAT)
	.dc.b	0, 0			* number of root directory entries
	.dc.b	0, 0			* number of sectors per logical image
	.dc.b	0			* media descriptor byte
	.dc.b	0, 0			* number of sectors per FAT

*	remainder of Boot Record describes physical media

	.dc.b	0, 0			* sectors per track
	.dc.b	0, 0			* number of heads
	.dc.b	0, 0			* number of hidden sectors

*	additional boot record info for Revision 7.0 External Specication

	.dc.b	0, 0, 0, 0		* extended disk size if above == 0
	.dc.b	0, 0			* first sector num after root dir
	.dc.b	0, 0, 0, 0		* code load base
	.dc.b	0, 0, 0, 0		* code length
	.dc.b	0, 0, 0, 0		* data load base
	.dc.b	0, 0, 0, 0		* data length
	
*	Start of boot loader code proper.
*
*	We need to do a number of things before invoking the
*	`C' boot loader.  We need to:
*	
*	- Save TENbug information passed to us in the
*	  registers. Registers of interest to us:
*
*		D0	The drive number
*		A5	Start of boot string
*		A6	End of boot string + 1
*
*	  We use the boot string to determine what file to boot in.
*	  We pass the drive number to the operating system so it
*	  can determine the BOOT DRIVE for configuration.
*
*	- Prepare stack for entry into main C routine,
*	  fetching the required parameters from TENbug.
*
*	- Set up `C' environment by zeroing the BSS section of
*	  the boot loader.

start:

*	Zero BSS - We trash A0 and A1.
*
*	The pseudo-variable "_edata" is supplied by loader,
*	it's address is just past the end of the initialized data section
*	or the start of the BSS section.  "_end"s address is just beyond
*	the end of BSS.  We loop through and zero the BSS section.

	Move.l	#_edata,A0	* _edata and _end not relocated by loader
	Add.l	#_bootrec,A0	* so we do some calculations from _bootrec
	Move.l	#_end,A1
	Add.l	#_bootrec,A1
clrloop:
	Clr.b	(A0)+
	Cmp.l	A0,A1
	Bhi	clrloop

*	Save TENbug information.
*
*	NOTE: we later copy the boot string passed to us by TENbug
*	rather than null terminating the string delimited by the
*	two registers (A5 and A6) right here.  I'm just playing it
*	safe, I don't like to modify memory whose layout I'm unacquainted
*	with.

	Move.w	D0,_disknum	* Drive number
	Move.l	A5,_strtbtstr	* Start of boot string
	Move.l	A6,_endbtstr	* End of boot string

*	Intitialize our stack pointer.

	Move.l	#_end,SP
	Add.l	#_bootrec,SP
	Add.l	#STKSIZ,SP

*	Jump to the booter entry

	Jsr	_Boot

*	If we return because of error, then we drop into idle loop.
*	If successful, we'll never get here.
*
*	NOTE: the alternate entry "_Exit" is called from some error
*	conditions in the boot loader where I don't want to unwind
*	the stack and pass an error condition back up.  Sorry Yourdon.

_Exit:
idle:	Bra	idle


*******************************************************************
*
*	_SetAndGo
*
*	We restore the TENbug registers we're interested in and jump
*	indirect to the operating system.  Our job is done then.
*
*	ASSUMPTION: o.s. sets its own stack pointer.
*
*	NOTE: There are some other registers which the O.S. booted
*	may be interested in.  For now we just restore the disk
*	drive number.  Perhaps in the future we'll include the
*	boot string or restore ALL the TENbug environment.
*	
*	NOTE: We may in the future have to pass information
*	describing the partition we just booted from when partitioning
*	is supported.
*
*******************************************************************

_SetAndGo:
	Move.w	_disknum,D0
	Move.l	4(SP),A0	* Jump to address passed on stack.
	Jmp	(A0)		* We never return from Jmp.

	.End


	Move.w	_disknum,D0
	Move.l	4(SP),A0	* Jump to address pa