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
          #include "voAMRWBDecID.h"
          .section  .text
	  .global   Dot_product12_asm

Dot_product12_asm:

          STMFD   	    r13!, {r4 - r12, r14} 
          MOV               r4, #0                                 @ L_sum = 0
          MOV               r5, #0                                 @ i = 0

LOOP:
          LDR           r6, [r0], #4
          LDR           r7, [r1], #4
          LDR           r8, [r0], #4
          SMLABB        r4, r6, r7, r4
          LDR           r9, [r1], #4
          SMLATT        r4, r6, r7, r4 

          LDR           r6, [r0], #4
          SMLABB        r4, r8, r9, r4

	  LDR           r7, [r1], #4
	  SMLATT        r4, r8, r9, r4	  
	  LDR           r8, [r0], #4

	  SMLABB        r4, r6, r7, r4
	  LDR           r9, [r1], #4
	  SMLATT        r4, r6, r7, r4
	  ADD           r5, r5, #8
	  SMLABB        r4, r8, r9, r4
	  CMP           r5, r2
	  SMLATT        r4, r8, r9, r4
	  BLT           LOOP
              
          MOV           r12, r4, LSL #1
          ADD           r12, r12, #1                         @ L_sum = (L_sum << 1)  + 1

          CLZ           r10, r12
          SUB           r10, r10, #1                         @ sft = norm_l(L_sum)
          MOV           r0, r12, LSL r10                     @ L_sum = L_sum << sft
          RSB           r11, r10, #30                        @ *exp = 30 - sft
          STRH          r11, [r3]                     

Dot_product12_end:
		     
          LDMFD   	    r13!, {r4 - r12, r15} 
          .END
 

