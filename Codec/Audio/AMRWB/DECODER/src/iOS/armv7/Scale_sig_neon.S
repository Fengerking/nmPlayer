@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@*************************************************************** 
@void Scale_sig(
@               Word16 x[],                           /* (i/o) : signal to scale               */
@               Word16 lg,                            /* (i)   : size of x[]                   */
@               Word16 exp                            /* (i)   : exponent: x = round(x << exp) */
@)
@****************************************************************
@  x[]   ---  r0
@  lg    ---  r1
@  exp   ---  r2
          #include "voAMRWBDecID.h"
          .text   .align 4
          .globl   _Scale_sig_asm

_Scale_sig_asm:

          stmfd   	r13!, {r4 - r12, r14} 
          mov           r4, #4
          vmov.s32      q15, #0x8000       
          vdup.s32      q14, r2  
          mov           r5, r0                          @ copy x[] address    
          cmp           r1, #64
          moveq         r4, #1
          beq           LOOP
          cmp           r1, #256
          beq           LOOP
          @312-256 = 56
          vld1.s16      {q0, q1}, [r5]!                 @load 16 Word16 x[]
          vld1.s16      {q2, q3}, [r5]!                 @load 16 Word16 x[] 
          vld1.s16      {q4, q5}, [r5]!                 @load 16 Word16 x[]
          vld1.s16      {q6}, [r5]!

          vshll.s16     q10, d0, #16
          vshll.s16     q11, d1, #16
          vshll.s16     q12, d2, #16
          vshll.s16     q13, d3, #16
          vshl.s32      q10, q10, q14
          vshl.s32      q11, q11, q14
          vshl.s32      q12, q12, q14
          vshl.s32      q13, q13, q14
          vaddhn.s32    d16, q10, q15
          vaddhn.s32    d17, q11, q15
          vaddhn.s32    d18, q12, q15
          vaddhn.s32    d19, q13, q15
          vst1.s16      {q8, q9}, [r0]!                 @store 16 Word16 x[]
          vshll.s16     q10, d4, #16
          vshll.s16     q11, d5, #16
          vshll.s16     q12, d6, #16
          vshll.s16     q13, d7, #16
          vshl.s32      q10, q10, q14
          vshl.s32      q11, q11, q14
          vshl.s32      q12, q12, q14
          vshl.s32      q13, q13, q14
          vaddhn.s32    d16, q10, q15
          vaddhn.s32    d17, q11, q15
          vaddhn.s32    d18, q12, q15
          vaddhn.s32    d19, q13, q15
          vst1.s16      {q8, q9}, [r0]!                 @store 16 Word16 x[]  
          vshll.s16     q10, d8, #16
          vshll.s16     q11, d9, #16
          vshll.s16     q12, d10, #16
          vshll.s16     q13, d11, #16
          vshl.s32      q10, q10, q14
          vshl.s32      q11, q11, q14
          vshl.s32      q12, q12, q14
          vshl.s32      q13, q13, q14
          vaddhn.s32    d16, q10, q15
          vaddhn.s32    d17, q11, q15
          vaddhn.s32    d18, q12, q15
          vaddhn.s32    d19, q13, q15
          vst1.s16      {q8, q9}, [r0]!                 @store 16 Word16 x[]   
          vshll.s16     q10, d12, #16
          vshll.s16     q11, d13, #16
          vshl.s32      q10, q10, q14
          vshl.s32      q11, q11, q14
          vaddhn.s32    d16, q10, q15
          vaddhn.s32    d17, q11, q15
          vst1.s16      {q8}, [r0]!                     @store 8 Word16 x[]                        


LOOP:

          vld1.s16      {q0, q1}, [r5]!                 @load 16 Word16 x[]
          vld1.s16      {q2, q3}, [r5]!                 @load 16 Word16 x[]
          vld1.s16      {q4, q5}, [r5]!                 @load 16 Word16 x[]
          vld1.s16      {q6, q7}, [r5]!                 @load 16 Word16 x[]

          vshll.s16     q8, d0, #16
          vshll.s16     q9, d1, #16
          vshll.s16     q10, d2, #16
          vshll.s16     q11, d3, #16     
          vshl.s32      q8, q8, q14
          vshl.s32      q9, q9, q14
          vshl.s32      q10, q10, q14
          vshl.s32      q11, q11, q14
          vaddhn.s32    d16, q8, q15
          vaddhn.s32    d17, q9, q15
          vaddhn.s32    d18, q10, q15
          vaddhn.s32    d19, q11, q15
          vst1.s16      {q8, q9}, [r0]!                 @store 16 Word16 x[]

   
          vshll.s16     q12, d4, #16
          vshll.s16     q13, d5, #16
          vshll.s16     q10, d6, #16
          vshll.s16     q11, d7, #16
          vshl.s32      q12, q12, q14
          vshl.s32      q13, q13, q14
          vshl.s32      q10, q10, q14
          vshl.s32      q11, q11, q14
          vaddhn.s32    d16, q12, q15
          vaddhn.s32    d17, q13, q15
          vaddhn.s32    d18, q10, q15
          vaddhn.s32    d19, q11, q15
          vst1.s16      {q8, q9}, [r0]!                 @store 16 Word16 x[]

          vshll.s16     q10, d8, #16
          vshll.s16     q11, d9, #16
          vshll.s16     q12, d10, #16
          vshll.s16     q13, d11, #16
          vshl.s32      q10, q10, q14
          vshl.s32      q11, q11, q14
          vshl.s32      q12, q12, q14
          vshl.s32      q13, q13, q14
          vaddhn.s32    d16, q10, q15
          vaddhn.s32    d17, q11, q15
          vaddhn.s32    d18, q12, q15
          vaddhn.s32    d19, q13, q15
          vst1.s16      {q8, q9}, [r0]!                 @store 16 Word16 x[]

          vshll.s16     q10, d12, #16   
          vshll.s16     q11, d13, #16
          vshll.s16     q12, d14, #16
          vshll.s16     q13, d15, #16
          vshl.s32      q10, q10, q14
          vshl.s32      q11, q11, q14
          vshl.s32      q12, q12, q14
          vshl.s32      q13, q13, q14
          vaddhn.s32    d16, q10, q15
          vaddhn.s32    d17, q11, q15
          vaddhn.s32    d18, q12, q15
          vaddhn.s32    d19, q13, q15 
          vst1.s16      {q8, q9}, [r0]!                 @store 16 Word16 x[]  
          subs          r4, r4, #1
          bgt           LOOP     
                                          
Scale_sig_asm_end:

          ldmfd   	r13!, {r4 - r12, r15} 
          @ENdFUNC
          @.ENd
 

