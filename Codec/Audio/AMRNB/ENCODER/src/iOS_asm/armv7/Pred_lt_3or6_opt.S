@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@****************************************************************
@void Pred_lt_3or6 (
@    Word16 exc[],     /* in/out: excitation buffer                         */
@    Word16 T0,        /* input : integer pitch lag                         */
@    Word16 frac,      /* input : fraction of lag                           */
@    Word16 flag3      /* input : if set, upsampling rate = 3 (6 otherwise) */
@)
        #include "voAMRNBEncID.h"
        .text     .align   4
	.globl    _Pred_lt_3or6_asm
	.globl    _inter_6

@******************************
@ constant
@******************************

@******************************
@ ARM register 
@******************************
@ exc[]                RN                 r0
@ T0                   RN                 r1
@ frac                 RN                 r2
@ flag3                RN                 r3
         
@******************************
@ Neon register 
@******************************

_Pred_lt_3or6_asm:

        stmfd           r13!, {r4 - r12, r14}  
        sub             r8, r0, r1, lsl #1             @ get the x0 = &exc[-T0]
        rsb             r2, r2, #0                     @ frac = -(frac)
        cmp             r3, #0
        beq             Lable1
        mov             r2, r2, lsr #1                 @ frac = (frac >>1)@
Lable1:
        cmp             r2, #0
        bge             Lable2
        add             r2, r2, #6                     @ frac = frac + UP_SAMP_MAX
        sub             r8, r8, #2                     @ x0--
Lable2:
        rsb             r9, r2, #6                     @ frac2 = UP_SAMP_MAX - frac
        ldr             r5, Table
        
        mov             r10, #40                       @ j = L_subFR
        add             r6, r5, r2, lsl #1             @ c1 = &inter_6[frac]
        add             r7, r5, r9, lsl #1             @ c2 = &inter_6[frac2]
        
        ldrsh           r12, [r6]                      @ c1[0]
        ldrsh           r14, [r7]                      @ c2[0]
        vdup.s16        d24, r12
        vdup.s16        d25, r14
        vmov.s32        q11, #0x8000

        add             r12, r8, #2
        
        vld1.s16        d0, [r8]!
        vld1.s16        d10, [r12]!
        vqdmull.s16     q10, d0, d24
        vqdmlal.s16     q10, d10, d25
        vaddhn.s32      d0, q10, q11       
        vst1.s16        d0, [r0]!
        
        vld1.s16        d1, [r8]!
        vld1.s16        d11, [r12]!
        vqdmull.s16     q10, d1, d24
        vqdmlal.s16     q10, d11, d25
        vaddhn.s32      d1, q10, q11    
        vst1.s16        d1, [r0]!
    
        vld1.s16        d2, [r8]!
        vld1.s16        d12, [r12]!
        vqdmull.s16     q10, d2, d24
        vqdmlal.s16     q10, d12, d25
        vaddhn.s32      d2, q10, q11       
        vst1.s16        d2, [r0]!

        vld1.s16        d3, [r8]!
        vld1.s16        d13, [r12]!
        vqdmull.s16     q10, d3, d24
        vqdmlal.s16     q10, d13, d25
        vaddhn.s32      d3, q10, q11     
        vst1.s16        d3, [r0]!
     
        vld1.s16        d4, [r8]!
        vld1.s16        d14, [r12]!
        vqdmull.s16     q10, d4, d24
        vqdmlal.s16     q10, d14, d25
        vaddhn.s32      d4, q10, q11      
        vst1.s16        d4, [r0]!
     
        vld1.s16        d5, [r8]!
        vld1.s16        d15, [r12]!
        vqdmull.s16     q10, d5, d24
        vqdmlal.s16     q10, d15, d25
        vaddhn.s32      d5, q10, q11     
        vst1.s16        d5, [r0]!
     
        vld1.s16        d6, [r8]!
        vld1.s16        d16, [r12]!
        vqdmull.s16     q10, d6, d24
        vqdmlal.s16     q10, d16, d25
        vaddhn.s32      d6, q10, q11     
        vst1.s16        d6, [r0]!
       
        vld1.s16        d7, [r8]!
        vld1.s16        d17, [r12]!
        vqdmull.s16     q10, d7, d24
        vqdmlal.s16     q10, d17, d25
        vaddhn.s32      d7, q10, q11   
        vst1.s16        d7, [r0]!
      
        vld1.s16        d8, [r8]!
        vld1.s16        d18, [r12]!
        vqdmull.s16     q10, d8, d24
        vqdmlal.s16     q10, d18, d25
        vaddhn.s32      d8, q10, q11      
        vst1.s16        d8, [r0]!
        
        vld1.s16        d9, [r8]!
        vld1.s16        d19, [r12]!
        vqdmull.s16     q10, d9, d24
        vqdmlal.s16     q10, d19, d25
        vaddhn.s32      d9, q10, q11   
        vst1.s16        d9, [r0]! 

Pred_lt_3or6_asm_end: 
 
        ldmfd           r13!, {r4 - r12, r15}
    
Table:
        .word      _inter_6

        @.ENd
