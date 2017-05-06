@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@*************************************************************** 
@Word32 dot_product12(                      /* (o) q31: normalized result (1 < val <= -1) */
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
          .text   .align 4
          .globl   _Dot_product12_asm

_Dot_product12_asm:

          stmfd   	    r13!, {r4 - r12, r14} 
          vld1.s16          {q0, q1}, [r0]!               @load 16 Word16 x[]
          vld1.s16          {q2, q3}, [r0]!               @load 16 Word16 x[]
          vld1.s16          {q4, q5}, [r0]!               @load 16 Word16 x[]
          vld1.s16          {q6, q7}, [r0]!               @load 16 Word16 x[]
          vqdmull.s16       q15, d0, d0
          vqdmlal.s16       q15, d1, d1
          vqdmlal.s16       q15, d2, d2
          vqdmlal.s16       q15, d3, d3
          vqdmlal.s16       q15, d4, d4
          vqdmlal.s16       q15, d5, d5
          vqdmlal.s16       q15, d6, d6
          vqdmlal.s16       q15, d7, d7                                       
          vqdmlal.s16       q15, d8, d8
          vqdmlal.s16       q15, d9, d9
          vqdmlal.s16       q15, d10, d10
          vqdmlal.s16       q15, d11, d11
          vqdmlal.s16       q15, d12, d12
          vqdmlal.s16       q15, d13, d13
          vqdmlal.s16       q15, d14, d14
          vqdmlal.s16       q15, d15, d15

          cmp               r2, #64
          beq               Lable
          vld1.s16          {q0, q1}, [r0]!               @load 16 Word16 x[]   
          vqdmlal.s16       q15, d0, d0             
          vqdmlal.s16       q15, d1, d1
          vqdmlal.s16       q15, d2, d2
          vqdmlal.s16       q15, d3, d3
Lable:              
          vqadd.s32         d30, d30, d31
          vpadd.s32         d30, d30, d30
          vmov.s32          r12, d30[0]        

          add               r12, r12, #1                         @ L_sum = (L_sum << 1)  + 1
          clz               r10, r12
          sub               r10, r10, #1                         @ sft = norm_l(L_sum)
          mov               r0,  r12, lsl r10                     @ L_sum = L_sum << sft
          rsb               r11, r10, #30                        @ *exp = 30 - sft
          strh              r11, [r3]                     

Dot_product12_end:
		     
          ldmfd   	    r13!, {r4 - r12, r15} 
          @ENDFUNC
          @.ENd
 

