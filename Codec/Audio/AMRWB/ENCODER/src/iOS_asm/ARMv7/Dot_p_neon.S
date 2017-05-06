@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@        VisualOn, Inc. Confidential and Proprietary, 2003-2010 
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   
@**********************************************************************/
@Word32 dot_product12(                      /* (o) q31: normalized result (1 < val <= -1) */
@       Word16 x[],                           /* (i) 12bits: x vector                       */
@       Word16 y[],                           /* (i) 12bits: y vector                       */
@       Word16 lg,                            /* (i)    : vector length                     */
@       Word16 * exp                          /* (o)    : exponent of result (0..+30)       */
@)
@************************************************************************
@  x[]   ---  r0
@  y[]   ---  r1
@  lg    ---  r2
@  *exp  ---  r3
          #include "voAMRWBEncID.h"
          .text   .align    4
          .globl    _voAMRWBEncDot_product12_asm

_voAMRWBEncDot_product12_asm:

          stmfd   	    r13!, {r4 - r12, r14}
	  cmp               r0, r1
	  beq               LOOP_EQ

          vld1.s16          {q0, q1}, [r0]!               @load 16 Word16 x[]
          vld1.s16          {q2, q3}, [r0]!               @load 16 Word16 x[]
          vld1.s16          {q4, q5}, [r0]!               @load 16 Word16 x[]
          vld1.s16          {q6, q7}, [r0]!               @load 16 Word16 x[]
	  vld1.s16          {q8, q9}, [r1]!               @load 16 Word16 y[]
	  vld1.s16          {q10, q11}, [r1]!             @load 16 Word16 y[]
	  vld1.s16          {q12, q13}, [r1]!             @load 16 Word16 y[]

          vmull.s16         q15, d16, d0
          vmlal.s16         q15, d17, d1               
          vmlal.s16         q15, d18, d2
          vmlal.s16         q15, d19, d3
	  vld1.s16          {q0, q1}, [r1]!               @load 16 Word16 y[]   
          vmlal.s16         q15, d20, d4       
          vmlal.s16         q15, d21, d5
          vmlal.s16         q15, d22, d6
          vmlal.s16         q15, d23, d7                                       
          vmlal.s16         q15, d24, d8
          vmlal.s16         q15, d25, d9
          vmlal.s16         q15, d26, d10
          vmlal.s16         q15, d27, d11
          vmlal.s16         q15, d0, d12
          vmlal.s16         q15, d1, d13
          vmlal.s16         q15, d2, d14
          vmlal.s16         q15, d3, d15

          cmp               r2, #64
          beq               Lable1
          vld1.s16          {q0, q1}, [r0]!               @load 16 Word16 x[]  
	  vld1.s16          {q2, q3}, [r1]! 
          vmlal.s16         q15, d4, d0             
          vmlal.s16         q15, d5, d1
          vmlal.s16         q15, d6, d2
          vmlal.s16         q15, d7, d3
	  bl                Lable1

LOOP_EQ:
          vld1.s16          {q0, q1}, [r0]!
	  vld1.s16          {q2, q3}, [r0]!
	  vld1.s16          {q4, q5}, [r0]!
	  vld1.s16          {q6, q7}, [r0]!
	  vmull.s16         q15, d0, d0
	  vmlal.s16         q15, d1, d1
	  vmlal.s16         q15, d2, d2
	  vmlal.s16         q15, d3, d3
	  vmlal.s16         q15, d4, d4
	  vmlal.s16         q15, d5, d5
	  vmlal.s16         q15, d6, d6
	  vmlal.s16         q15, d7, d7
	  vmlal.s16         q15, d8, d8
	  vmlal.s16         q15, d9, d9
	  vmlal.s16         q15, d10, d10
	  vmlal.s16         q15, d11, d11
	  vmlal.s16         q15, d12, d12
	  vmlal.s16         q15, d13, d13
	  vmlal.s16         q15, d14, d14
	  vmlal.s16         q15, d15, d15

	  cmp               r2, #64
	  beq               Lable1
	  vld1.s16          {q0, q1}, [r0]!
	  vmlal.s16         q15, d0, d0
	  vmlal.s16         q15, d1, d1
	  vmlal.s16         q15, d2, d2
	  vmlal.s16         q15, d3, d3

Lable1: 

          vqadd.s32         d30, d30, d31
          vpadd.s32         d30, d30, d30
          vmov.s32          r12, d30[0]        

	  add               r12, r12, r12
          add               r12, r12, #1                         @ L_sum = (L_sum << 1)  + 1
	  mov               r4, r12
	  cmp               r12, #0
	  rsblt             r4, r12, #0
          clz               r10, r4
          sub               r10, r10, #1                         @ sft = norm_l(L_sum)
          mov               r0, r12, lsl r10                     @ L_sum = L_sum << sft
          rsb               r11, r10, #30                        @ *exp = 30 - sft
          strh              r11, [r3]                     

Dot_product12_end:
		     
          ldmfd   	    r13!, {r4 - r12, r15} 

          @.ENd

