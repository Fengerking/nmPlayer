@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@****************************************************************
@vo_postProcessEnc_asm(
@		&exc[i_subfr],
@		code,
@		gain_code,
@		pitch_fac,
@		tempShift)@
@{
@	for (i = 0@ i < L_SUBFR@ i++)@
@	{
@		L_temp = L_mult (exc[i + i_subfr], pitch_fac)@
@		L_temp = L_mac (L_temp, code[i], gain_code)@
@		L_temp = L_shl (L_temp, tempShift)@
@		exc[i + i_subfr] = round (L_temp)@           
@	}
@}
        #include "voAMRNBEncID.h"
        .text   .align   4
	.globl   _vo_postProcessEnc_asm
      
_vo_postProcessEnc_asm:

        stmfd          r13!, {r4 - r12, r14}  
        mov            r8, #0x8000
        mov            r7, r0                               @copy &exc[i_subfr] address
        vdup.s32       q12, r8    
        
        vld1.s16       {d0, d1, d2, d3}, [r7]!
        vld1.s16       {d4, d5, d6, d7}, [r7]!
        vld1.s16       {d8, d9}, [r7]!

        vdup.s16       d22, r3                             @tmp1 --> pitch_fac
        vdup.s16       d23, r2                             @tmp2 --> gain_code


        vld1.s16       {d10, d11, d12, d13}, [r1]!
        vld1.s16       {d14, d15, d16, d17}, [r1]!
        vld1.s16       {d18, d19}, [r1]!

        vqdmull.s16    q10, d0, d22
        vqdmlal.s16    q10, d10, d23
        vqshl.s32      q10, q10, #2
        vaddhn.s32     d0, q10, q12


        vqdmull.s16    q10, d1, d22
        vqdmlal.s16    q10, d11, d23
        vqshl.s32      q10, q10, #2
        vaddhn.s32     d1, q10, q12


        vqdmull.s16    q10, d2, d22
        vqdmlal.s16    q10, d12, d23
        vqshl.s32      q10, q10, #2
        vaddhn.s32     d2, q10, q12


        vqdmull.s16    q10, d3, d22
        vqdmlal.s16    q10, d13, d23
        vqshl.s32      q10, q10, #2
        vaddhn.s32     d3, q10, q12


        vqdmull.s16    q10, d4, d22
        vqdmlal.s16    q10, d14, d23
        vqshl.s32      q10, q10, #2
        vaddhn.s32     d4, q10, q12


        vqdmull.s16    q10, d5, d22
        vqdmlal.s16    q10, d15, d23
        vqshl.s32      q10, q10, #2
        vaddhn.s32     d5, q10, q12


        vqdmull.s16    q10, d6, d22
        vqdmlal.s16    q10, d16, d23
        vqshl.s32      q10, q10, #2
        vaddhn.s32     d6, q10, q12


        vqdmull.s16    q10, d7, d22
        vqdmlal.s16    q10, d17, d23
        vqshl.s32      q10, q10, #2
        vaddhn.s32     d7, q10, q12


        vqdmull.s16    q10, d8, d22
        vqdmlal.s16    q10, d18, d23
        vqshl.s32      q10, q10, #2
        vaddhn.s32     d8, q10, q12


        vqdmull.s16    q10, d9, d22
        vqdmlal.s16    q10, d19, d23
        vqshl.s32      q10, q10, #2
        vaddhn.s32     d9, q10, q12


        vst1.s16       {q0, q1}, [r0]!
        vst1.s16       {q2, q3}, [r0]!
        vst1.s16       {q4}, [r0]!                  

vo_postProcessEnc_asm_end: 
 
        ldmfd      r13!, {r4 - r12, r15}
    
        @.ENd
