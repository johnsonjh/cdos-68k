*
*   Copyright 1983, 1984
*   Alcyon Corporation
*   8716 Production Ave.
*   San Diego, CA  92121
*
*   @(#)lmul.s	2.2    8/8/84
*
* long multiply routine without floating point
*  call with:
*       two long values on stack
*  returns:
*       long value in R0 and R1
*
* warning:  no overflow checking or indication!!!!
*
*
*struct {unsigned hiword,loword;};
*
*long lmul(l1,l2)
*long l1,l2;
*{
*
*   register long t1;   // using address register
*   register int sign;
*   register int t2;
*
*   sign = 0;
*   if (l1 < 0) {
*       l1 = -l1;   //make it positive
*       sign++;
*   }
*   if (l2 < 0) {
*       l2 = -l2;   //make it positive
*       sign--;
*   }
*   t1 = l1.loword*l2.loword;
*   t2 = l1.hiword*l2.loword + l2.hiword*l1.loword;
*   t1.hiword = t1.hiword + t2;
*   if (sign)
*       t1 = -t1;   //negate results
*   return(t1);
*}
*
*
.globl  lmul,_lmul
.text
lmul:
_lmul:  
	suba.w  a1,a1       // initialize sign flag
	tst.w   4(sp)       // is first arg negative ?
	bge     L10         //      No
	neg.l   4(sp)       //      Yes, negate it
	addq.w  #1,a1       //          and note via sign flag

L10:    
	tst.w   8(sp)       // is second arg negative ?
	bge     L20         //      No
	neg.l   8(sp)       //      Yes, make it positive
	subq.w  #1,a1       //          and note via sign flag

L20:
	move.w  6(sp),d1    // arg1.loword  
	move.w  10(sp),d0   // arg2.loword
	move.w  d1,d2       // arg1.loword
	mulu    d0,d2       // arg2.loword * arg1.loword
	move.l  d2,a0       // save result
	move.w  4(sp),d2    // arg1.hiword
	beq     L30         //      No MUL necessary
	mulu    d0,d2       // arg1.hiword * arg2.loword
L30:
	move.w  8(sp),d0    // arg2.hiword
	beq     L40         //      No MUL necessary
	mulu    d1,d0       // arg2.hiword * arg1.loword
L40:    
	add.w   d2,d0       // form the sum of 2 lo-hi products
	swap    d0          // shift up 16 bits
	clr.w   d0
	add.l   a0,d0       // add lo-lo product
	move.w  a1,d1       // sign flag set ?
	beq     L50         //      No change
	neg.l   d0          // negate results
L50:
	rts

	.end
  