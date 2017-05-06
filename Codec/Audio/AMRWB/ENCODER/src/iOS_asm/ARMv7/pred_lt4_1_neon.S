@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@*   VisualOn, Inc. Confidential and Proprietary, 2003-2010 
@*  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@void Pred_lt4(
@     Word16 exc[],                         /* in/out: excitation buffer */
@     Word16 T0,                            /* input : integer pitch lag */
@     Word16 frac,                          /* input : fraction of lag   */
@     Word16 L_subfr                        /* input : subframe size     */
@)
@***********************************************************************
@ r0    ---  exc[]
@ r1    ---  T0
@ r2    ---  frac
@ r3    ---  L_subfr
          #include "voAMRWBEncID.h"
          .text   .align   4 
          .globl   _pred_lt4_asm
          .globl   _voAMRWBEncinter4_2

_pred_lt4_asm:

          stmfd   	r13!, {r4 - r12, r14} 
          sub           r4, r0, r1, lsl #1                        @ x = exc - T0
          rsb           r2, r2, #0                                @ frac = - frac
          sub           r4, r4, #30                               @ x -= L_INTERPOL2 - 1
          cmp           r2, #0
          addlt         r2, r2, #4                                @ frac += UP_SAMP
          sublt         r4, r4, #2                                @ x--

          ldr           r11, Lable1
          rsb           r2, r2, #3                                @ k = UP_SAMP - 1 - frac
          mov           r8, #0                                    @ j = 0
	  add           r11, r11, r2, lsl #6                      @ get inter4_2[k][]

	  vld1.s16      {q0, q1}, [r11]!
	  vld1.s16      {q2, q3}, [r11]!
          
	  mov           r6, #0x8000 

          vld1.s16      {q4, q5}, [r4]!                           @load 16 x[]
          vld1.s16      {q6, q7}, [r4]!                           @load 16 x[]

LOOP:
          vqdmull.s16   q15, d8, d0
          vqdmlal.s16   q15, d9, d1
          vqdmlal.s16   q15, d10, d2
          vqdmlal.s16   q15, d11, d3
        
          vqdmlal.s16   q15, d12, d4
          vqdmlal.s16   q15, d13, d5
          vqdmlal.s16   q15, d14, d6
          vqdmlal.s16   q15, d15, d7

          ldrsh         r12, [r4], #2                
          
          vext.s16      d8, d8, d9, #1
          vext.s16      d9, d9, d10, #1
          vext.s16      d10, d10, d11, #1
          vext.s16      d11, d11, d12, #1
          vdup.s16      d24, r12
          vext.s16      d12, d12, d13, #1
          vext.s16      d13, d13, d14, #1
     
          vqadd.s32     d30, d30, d31
	  mov           r11, #0x8000          
          vpadd.s32     d30, d30, d30
          add           r8, r8, #1
          vmov.s32      r12, d30[0]
          vext.s16      d14, d14, d15, #1          

          qadd          r1, r12, r12                              @ L_sum = (L_sum << 2)
          vext.s16      d15, d15, d24, #1
          qadd          r5, r1, r6                         
          mov           r1, r5, ASR #16
          cmp           r8, r3
          strh          r1, [r0], #2                              @ exc[j] = (L_sum + 0x8000) >> 16
          blt           LOOP
                    
pred_lt4_end:
		     
          ldmfd   	r13!, {r4 - r12, r15} 
 
Lable1:
          .word   	_voAMRWBEncinter4_2
          @ENdFUNC
          @.ENd

