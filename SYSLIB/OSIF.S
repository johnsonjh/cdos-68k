	.text
	.globl	___osif
	.globl	___OSIF
___osif:
___OSIF:
	move	4(sp),d0		* function number
	move.l	6(sp),d1		* parameter block pointer
	trap	#14			* enter concurrent dos
	rts				* return (error code in d0.l)

	.end

                           