@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
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
          .text    .align 4
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

LOOP:
          mov           r12, r4                                   @ copy x[] address
          mov           r9, #0                                    @ i = 0
          mov           r14, #0                                   @ L_sum = 0L
          mov           r10, r11

          @ used registers : r0 -- exc[], r2 -- k, r3 -- L_subfr, r4 -- x, r8 -- j
          @ r11 -- inter4_2[], r14 -- 0

LOOP1:
          ldrsh         r1, [r12], #2                             @ x[i]
          ldrsh         r5, [r10], #8                             @ inter4_2[k]

          ldrsh         r6, [r12], #2                             @ x[i+1]
          ldrsh         r7, [r10], #8                             @ inter4_2[k + UP_SAMP]
          mla           r14, r1, r5, r14

          ldrsh         r1, [r12], #2                             @ x[i+2]
          ldrsh         r5, [r10],#8                              @ inter4_2[]
          mla           r14, r6, r7, r14

          ldrsh         r6, [r12], #2                             @ x[i+3]
          ldrsh         r7, [r10], #8
          mla           r14, r1, r5, r14

          ldrsh         r1, [r12], #2                             @ x[i+4]
          ldrsh         r5, [r10], #8                       
          mla           r14, r6, r7, r14

          ldrsh         r6, [r12], #2                             @ x[i+5]
          ldrsh         r7, [r10], #8
          mla           r14, r1, r5, r14

          ldrsh         r1, [r12], #2                             @ x[i+6]
          ldrsh         r5, [r10], #8
          mla           r14, r6, r7, r14                          
 
          ldrsh         r6, [r12], #2                             @ x[i+7]
          ldrsh         r7, [r10], #8
          mla           r14, r1, r5, r14

          ldrsh         r1, [r12], #2                             @ x[i]
          ldrsh         r5, [r10], #8                             @ inter4_2[k]
          mla           r14, r6, r7, r14

          ldrsh         r6, [r12], #2                             @ x[i+1]
          ldrsh         r7, [r10], #8                             @ inter4_2[k + UP_SAMP]
          mla           r14, r1, r5, r14

          ldrsh         r1, [r12], #2                             @ x[i+2]
          ldrsh         r5, [r10],#8                              @ inter4_2[]
          mla           r14, r6, r7, r14

          ldrsh         r6, [r12], #2                             @ x[i+3]
          ldrsh         r7, [r10], #8
          mla           r14, r1, r5, r14

          ldrsh         r1, [r12], #2                             @ x[i+4]
          ldrsh         r5, [r10], #8                       
          mla           r14, r6, r7, r14

          ldrsh         r6, [r12], #2                             @ x[i+5]
          ldrsh         r7, [r10], #8
          mla           r14, r1, r5, r14

          ldrsh         r1, [r12], #2                             @ x[i+6]
          ldrsh         r5, [r10], #8
          mla           r14, r6, r7, r14                          
 
          ldrsh         r6, [r12], #2                             @ x[i+7]
          ldrsh         r7, [r10], #8
          mla           r14, r1, r5, r14
         
          add           r9, r9, #16                                @ i += 8
          mla           r14, r6, r7, r14

          cmp           r9, #32                                   @ L_INTERPOL2 << 1
          blt           LOOP1

          mov           r1, r14, lsl #2                           @ L_sum = (L_sum << 2)
          add           r4, r4, #2                                @ x++
          add           r5, r1, #0x8000                           
          add           r8, r8, #1                                @ j++
          mov           r1, r5, asr #16
          cmp           r8, r3
          strh          r1, [r0], #2                              @ exc[j] = (L_sum + 0x8000) >> 16
          blt           LOOP
                    
pred_lt4_end:
		     
          ldmfd   	r13!, {r4 - r12, r15} 
 
Lable1:
          .word   	_inter4_2
          @.END
