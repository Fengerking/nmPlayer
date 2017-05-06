@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@*************************************************************** 
@void Filt_6k_7k(
@     Word16 signal[],                      /* input:  signal                  */
@     Word16 lg,                            /* input:  length of input         */
@     Word16 mem[]                          /* in/out: memory (size=30)        */
@)
@******************************************************************
@ r0    ---  signal[]
@ r1    ---  lg
@ r2    ---  mem[] 
          #include "voAMRWBDecID.h"
          .text   .align 4
	  .globl   _Filt_6k_7k_asm
          .globl   _fir_6k_7k

_Filt_6k_7k_asm:

          stmfd   		r13!, {r4 - r12, r14} 
          sub    		r13, r13, #240              @ x[L_SUBFR16k + (L_FIR - 1)]
          mov     		r8, r0                      @ copy signal[] address
          mov     		r5, r2                      @ copy mem[] address

          mov     		r0, r2
          mov     		r1, r13

	  vld1.s16              {d0, d1, d2, d3}, [r0]!
	  vld1.s16              {d4, d5, d6, d7}, [r0]!

	  vst1.s16              {d0, d1, d2, d3}, [r1]!
	  vst1.s16              {d4, d5, d6}, [r1]!
	  vst1.s16              d7[0], [r1]!
	  vst1.s16              d7[1], [r1]!



          ldr     		r10, Lable1                 @ get fir_7k address     
          mov                   r3, r8                      @ change myMemCopy to Copy, due to Copy will change r3 content
          add     	    	r6, r13, #60                @ get x[L_FIR - 1] address
          mov           	r7, r3                      @ get signal[i]
          @for (i = lg - 1@ i >= 0@ i--)
          @{
          @     x[i + L_FIR - 1] = signal[i] >> 2@
          @}
	  vld1.s16              {q0, q1}, [r7]!		    @ signal[0]  ~ signal[15]
	  vld1.s16              {q2, q3}, [r7]!             @ signal[16] ~ signal[31]
          vld1.s16              {q4, q5}, [r7]!             @ signal[32] ~ signal[47]
	  vld1.s16              {q6, q7}, [r7]!             @ signal[48] ~ signal[63]
	  vld1.s16              {q8, q9}, [r7]!             @ signal[64] ~ signal[79]
	  vshr.s16              q10, q0, #2
          vshr.s16              q11, q1, #2
          vshr.s16              q12, q2, #2
	  vshr.s16              q13, q3, #2
	  vst1.s16              {q10, q11}, [r6]!
	  vshr.s16              q0,  q4, #2
	  vshr.s16              q1,  q5, #2
	  vshr.s16              q10, q6, #2
	  vshr.s16              q11, q7, #2
	  vshr.s16              q2,  q8, #2
	  vshr.s16              q3,  q9, #2
	  vst1.s16              {q12, q13}, [r6]!
	  vst1.s16              {q0, q1}, [r6]!
	  vst1.s16              {q10, q11}, [r6]!
	  vst1.s16              {q2, q3}, [r6]!

          str     		r5, [sp, #-4]               @ PUSH  r5 to stack
          @ not use registers: r4, r10, r12, r14, r5
          mov     		r4, r13 
          mov     		r5, #0                      @ i = 0    
         
          @ r4 --- x[i], r10 ---- fir_6k_7k
          vld1.s16              {q0, q1}, [r10]!           @fir_6k_7k[0]  ~ fir_6k_7k[15]
	  vld1.s16              {q2, q3}, [r10]!           @fir_6k_7k[16] ~ fir_6k_7k[31]
          vmov.s16              d7[3], r5                        @set fir_6k_7K = 0

	  vld1.s16              {q4, q5}, [r4]!            @x[0]  ~ x[15]
	  vld1.s16              {q6, q7}, [r4]!            @x[16] ~ X[31]
	  vld1.s16              {q8}, [r4]! 
          vmov.s16              q15, #0	  
          
LOOP_6K7K:

          vmull.s16             q9, d8, d0[0]                 
          vmull.s16             q10, d9, d1[0] 
          vmull.s16             q11, d9, d0[0]                
          vmull.s16             q12, d10, d1[0]
          vext.8                q4, q4, q5, #2
          vmlal.s16             q9, d10, d2[0]
          vmlal.s16             q10, d11, d3[0]
          vmlal.s16             q11, d11, d2[0]
          vmlal.s16             q12, d12, d3[0]    
          vext.8                q5, q5, q6, #2
          vmlal.s16             q9, d12, d4[0]
          vmlal.s16             q10, d13, d5[0]
          vmlal.s16             q11, d13, d4[0]
          vmlal.s16             q12, d14, d5[0]
          vext.8                q6, q6, q7, #2
          vmlal.s16             q9, d14, d6[0]
          vmlal.s16             q10, d15, d7[0]
          vmlal.s16             q11, d15, d6[0]
	  vmlal.s16             q12, d16, d7[0]
	  vext.8  		q7, q7, q8, #2 

	  vmlal.s16 		q9, d8, d0[1] 
	  vmlal.s16     	q10, d9, d1[1]
	  vext.8 		q8, q8, q15, #2 
	  vmlal.s16 		q11, d9, d0[1] 
	  vmlal.s16 		q12, d10, d1[1]
	  vext.8  		q4, q4, q5, #2
	  vmlal.s16 		q9, d10, d2[1]
	  vmlal.s16 		q10, d11, d3[1]
	  vmlal.s16 		q11, d11, d2[1]
	  vmlal.s16 		q12, d12, d3[1]    
	  vext.8  		q5, q5, q6, #2
	  vmlal.s16 		q9, d12, d4[1]
	  vmlal.s16 		q10, d13, d5[1]
	  vmlal.s16 		q11, d13, d4[1]
	  vmlal.s16 		q12, d14, d5[1]
	  vext.8  		q6, q6, q7, #2
	  vmlal.s16 		q9, d14, d6[1]
	  vmlal.s16 		q10, d15, d7[1]
	  vmlal.s16 		q11, d15, d6[1]
	  vmlal.s16 		q12, d16, d7[1]
	  vext.8  		q7, q7, q8, #2 

	  vmlal.s16 		q9, d8, d0[2] 
	  vmlal.s16 		q10, d9, d1[2]
	  vext.8 		q8, q8, q15,#2 
	  vmlal.s16 		q11, d9, d0[2] 
	  vmlal.s16 		q12, d10, d1[2]
	  vext.8  		q4, q4, q5, #2
	  vmlal.s16 		q9, d10, d2[2]
	  vmlal.s16 		q10, d11, d3[2]
	  vmlal.s16 		q11, d11, d2[2]
	  vmlal.s16 		q12, d12, d3[2]    
	  vext.8  		q5, q5, q6,#2
	  vmlal.s16 		q9, d12, d4[2]
	  vmlal.s16 		q10, d13, d5[2]
	  vmlal.s16 		q11, d13, d4[2]
	  vmlal.s16 		q12, d14, d5[2]
	  vext.8  		q6, q6, q7, #2
	  vmlal.s16 		q9, d14, d6[2]
	  vmlal.s16 		q10, d15, d7[2]
	  vmlal.s16 		q11, d15, d6[2]
	  vmlal.s16 		q12, d16, d7[2]
	  vext.8  		q7, q7, q8,#2 

	  vmlal.s16 		q9, d8, d0[3] 
	  vmlal.s16 		q10, d9, d1[3]
	  vext.8 		q8, q8, q15,#2 
	  vmlal.s16 		q11, d9, d0[3] 
	  vmlal.s16 		q12, d10, d1[3]
	  vext.8  		q4, q4, q5, #2
	  vmlal.s16 		q9, d10, d2[3]
	  vmlal.s16 		q10, d11, d3[3]
	  vmlal.s16 		q11, d11, d2[3]
	  vmlal.s16 		q12, d12, d3[3]    
	  vext.8  		q5, q5, q6,#2
	  vmlal.s16 		q9, d12, d4[3]
	  vmlal.s16 		q10, d13, d5[3]
	  vmlal.s16 		q11, d13, d4[3]
	  vmlal.s16 		q12, d14, d5[3]
	  vext.8  		q6, q6, q7, #2
	  vmlal.s16 		q9, d14, d6[3]
	  vmlal.s16 		q10, d15, d7[3]
	  vmlal.s16 		q11, d15, d6[3]
	  vmlal.s16 		q12, d16, d7[3]
	  vext.8 		q7, q7, q8, #2     

	  vmov.s16  		d8, d9
	  vext.8 		q8, q8, q15,#2 
	  vmov.s16  		d9, d10
	  vadd.s32  		q9, q9, q10
	  vmov.s16  		d10, d11
	  vmov.s16  		d11, d12
	  vadd.s32  		q11, q11, q12
	  vmov.s16  		d12, d13
	  vqrshrn.s32 		d28, q9, #15
	  vmov.s16  		d13, d14
	  vmov.s16  		d14, d15
	  vqrshrn.s32 		d29, q11, #15
	  vmov.s16  		d15, d16

	  vld1.s16  		{q8}, [r4]!
	  add                   r5, r5, #8
	  cmp   		r5, #80
	  vst1.s16  		{d28, d29}, [r3]!
	  blt     		LOOP_6K7K	       

          add     		r0, r13, #160               @x + lg
	  ldr     		r1, [sp, #-4]               @mem address

	  vld1.s16              {d0, d1, d2, d3}, [r0]!
	  vld1.s16              {d4, d5, d6, d7}, [r0]!

	  vst1.s16              {d0, d1, d2, d3}, [r1]!
	  vst1.s16              {d4, d5, d6}, [r1]!
	  vst1.s16              d7[0], [r1]!
	  vst1.s16              d7[1], [r1]!
                    
Filt_6k_7k_end:

          add     		r13, r13, #240  
          ldmfd   		r13!, {r4 - r12, r15} 
 
Lable1:
          .word   		_fir_6k_7k
          @ENdFUNC
          @.ENd


