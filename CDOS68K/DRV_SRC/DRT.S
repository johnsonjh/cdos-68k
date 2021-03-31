***********************************************************
*
*	Driver Services Runtime Library (68K)
*
*	To use these driver services, the driver must
*	have its driver header at location 0 relative to
*	the driver's data segment, and it must be named
*	"dh" so that the following routines can find it.
*	(On the 68K, the driver's data segment could be
*	anywhere in memory, not necessarily at physical
*	location 0.)
***********************************************************

	.data
	.globl	_dh			* driver header is external

	.text

	.globl	_polleve
_polleve:
	move	#0,d0
	bra	doit

	.globl	_doasr
_doasr:
	move	#1,d0
	bra	doit

	.globl	_evasr
_evasr:
	move	#2,d0
	bra	doit

	.globl	_nextasr
_nextasr:
	move	#3,d0
	bra	doit

	.globl	_pcreate
_pcreate:
	move	#4,d0
	bra	doit

	.globl	_setvec
_setvec:
	move	#5,d0
	bra	doit

	.globl	_supif
_supif:
	move	#6,d0
	bra	doit

	.globl	_flagget
_flagget:
	move	#7,d0
	bra	doit

	.globl	_flagrel
_flagrel:
	move	#8,d0
	bra	doit

	.globl	_flagclr
_flagclr:
	move	#9,d0
	bra	doit

	.globl	_flageve
_flageve:
	move	#10,d0
	bra	doit

	.globl	_flagset
_flagset:
	move	#11,d0
	bra	doit

	.globl	_mxinit
_mxinit:
	move	#12,d0
	bra	doit

	.globl	_mxunini
_mxunini:
	move	#13,d0
	bra	doit

	.globl	_mxevent
_mxevent:
	move	#14,d0
	bra	doit

	.globl	_mxrel
_mxrel:
	move	#15,d0
	bra	doit

	.globl	_salloc
_salloc:
	move	#16,d0
doit:
	move.l	_dh+60,a0
	jmp	(a0)

	.globl	_sfree
_sfree:
	move	#17,d0
	bra	doit

	.globl	_paddr
_paddr:
	move	#18,d0
	bra	doit

	.globl	_uaddr
_uaddr:
	move	#19,d0
	bra	doit

	.globl	_saddr
_saddr:
	move	#20,d0
	bra	doit

	.globl	_mapphys
_mapphys:
	move	#21,d0
	bra	doit

	.globl	_mapu
_mapu:
	move	#22,d0
	bra	doit

	.globl	_unmapu
_unmapu:
	move	#23,d0
	bra	doit

	.globl	_noabort
_noabort:
	move	#24,d0
	bra	doit

	.globl	_okabort
_okabort:
	move	#25,d0
	bra	doit

	.globl	_nodisp
_nodisp:
	move	#26,d0
	bra	doit

	.globl	_okdisp
_okdisp:
	move	#27,d0
	bra	doit

	.globl	_mlock
_mlock:
	move	#28,d0
	bra	doit

	.globl	_munlock
_munlock:
	move	#29,d0
	bra	doit

	.globl	_mrange
_mrange:
	move	#30,d0
	bra	doit

	.globl	_dsptch
_dsptch:
	move	#31,d0
	bra	doit

	.globl	_dvrif
_dvrif:
	move	#32,d0
	bra	doit

	.globl	_asrwait
_asrwait:
	move	#33,d0
	bra	doit

	.globl	_asrmx
_asrmx:
	move	#34,d0
	bra	doit

	.end
