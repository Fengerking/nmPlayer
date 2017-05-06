@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@*************************************************************** 
@Word32 Dot_product12(                      /* (o) Q31: normalized result (1 < val <= -1) */
@       Word16 x[],                           /* (i) 12bits: x vector                       */
@       Word16 y[],                           /* (i) 12bits: y vector                       */
@       Word16 lg,                            /* (i)    : vector length                     */
@       Word16 * exp                          /* (o)    : exponent of result (0..+30)       */
@)
@****************************************************************
@  x[]   ---  r0
@  y[]   ---  r1
@  lg    ---  r2
@  *exp  ---  r3
          #include "voAMRWBEncID.h"
          .text   .align 4
          .globl   _voAMRWBEncDot_product12_asm

_voAMRWBEncDot_product12_asm:

          stmfd   	r13!, {r4 - r12, r14} 
          mov           r4, #0                                 @ L_sum = 0
          mov           r5, #0                                 @ i = 0

LOOP:
          ldr           r6, [r0], #4
          ldr           r7, [r1], #4
          ldr           r8, [r0], #4
          smlabb        r4, r6, r7, r4
          ldr           r9, [r1], #4
	  smlatt        r4, r6, r7, r4 

	  ldr           r6, [r0], #4
	  smlabb        r4, r8, r9, r4

	  ldr           r7, [r1], #4
	  smlatt        r4, r8, r9, r4	  
	  ldr           r8, [r0], #4

	  smlabb        r4, r6, r7, r4
	  ldr           r9, [r1], #4
	  smlatt        r4, r6, r7, r4
	  add           r5, r5, #8
	  smlabb        r4, r8, r9, r4
	  cmp           r5, r2
	  smlatt        r4, r8, r9, r4
	  blt           LOOP
              
          mov           r12, r4, lsl #1
          add           r12, r12, #1                         @ L_sum = (L_sum << 1)  + 1
	  mov           r4, r12

          cmp           r12, #0
	  rsblt         r4, r12, #0
          clz           r10, r4
          sub           r10, r10, #1                         @ sft = norm_l(L_sum)
          mov           r0, r12, lsl r10                     @ L_sum = L_sum << sft
          rsb           r11, r10, #30                        @ *exp = 30 - sft
          strh          r11, [r3]                     

Dot_product12_end:
		     
          ldmfd         r13!, {r4 - r12, r15} 
          @ENDFUNC
          @.END
 

