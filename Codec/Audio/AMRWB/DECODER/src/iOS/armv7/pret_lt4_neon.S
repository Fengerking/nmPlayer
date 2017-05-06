@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@*************************************************************** 
@void Pred_lt4(
@     Word16 exc[],                         /* in/out: excitation buffer */
@     Word16 T0,                            /* input : integer pitch lag */
@     Word16 frac,                          /* input : fraction of lag   */
@     Word16 L_subfr                        /* input : subframe size     */
@)
@***************************************************************
@ r0    ---  exc[]
@ r1    ---  T0
@ r2    ---  frac
@ r3    ---  L_subfr
          #include "voAMRWBDecID.h"
          .text   .align 4
          .globl   _Pred_lt4_asm
          .globl   _inter4_2

_Pred_lt4_asm:

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
          add           r11, r11, r2, lsl #1                      @ get inter4_2[] address

          vld4.s16      {q0, q1}, [r11]!                          @load 4 inter4_2[], d0
          vld4.s16      {q2, q3}, [r11]!                          @load 4 inter4_2[], d4
          vld4.s16      {q3, q4}, [r11]!                          @load 4 inter4_2[], d6
          vld4.s16      {q13, q14}, [r11]!                        @load 4 inter4_2[], d26
          vld4.s16      {q8, q9}, [r11]!                          @load 4 inter4_2[], d16
          vld4.s16      {q9, q10}, [r11]!                         @load 4 inter4_2[], d18
          vld4.s16      {q10, q11}, [r11]!                        @load 4 inter4_2[], d20
          vld4.s16      {q11, q12}, [r11]!                        @load 4 inter4_2[], d22
           

          vld1.s16      {q4, q5}, [r4]!                           @load 16 x[]
          vld1.s16      {q6, q7}, [r4]!                           @load 16 x[]

LOOP:
          vqdmull.s16   q15, d8, d0
          vqdmlal.s16   q15, d9, d4
          vqdmlal.s16   q15, d10, d6
          vqdmlal.s16   q15, d11, d26
        
          vqdmlal.s16   q15, d12, d16
          vqdmlal.s16   q15, d13, d18
          vqdmlal.s16   q15, d14, d20
          vqdmlal.s16   q15, d15, d22

          ldrsh         r12, [r4], #2                
          
          vext.s16      d8, d8, d9, #1
          vext.s16      d9, d9, d10, #1
          vext.s16      d10, d10, d11, #1
          vext.s16      d11, d11, d12, #1
          vdup.s16      d24, r12
          vext.s16      d12, d12, d13, #1
          vext.s16      d13, d13, d14, #1
          vext.s16      d14, d14, d15, #1
          vext.s16      d15, d15, d24, #1
          
          vqadd.s32     d30, d30, d31
          vpadd.s32     d30, d30, d30
          add           r8, r8, #1
          vmov.s32      r12, d30[0]
          

          mov           r1, r12, lsl #1                           @ L_sum = (L_sum << 2)
          add           r5, r1, #0x8000                          
          mov           r1, r5, asr #16
          cmp           r8, r3
          strh          r1, [r0], #2                              @ exc[j] = (L_sum + 0x8000) >> 16
          blt           LOOP
                    
pred_lt4_end:
		     
          ldmfd   	r13!, {r4 - r12, r15} 
 
Lable1:
          .word   	_inter4_2
          @ENdFUNC
          @.ENd
