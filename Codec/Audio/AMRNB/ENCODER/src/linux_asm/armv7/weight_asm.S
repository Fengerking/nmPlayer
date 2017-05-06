@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@void Weight_Ai (
@    Word16 a[],         /* (i)     : a[M+1]  LPC coefficients   (M=10)    */
@    const Word16 fac[], /* (i)     : Spectral expansion factors.          */
@    Word16 a_exp[]      /* (o)     : Spectral expanded LPC coefficients   */
@)
        #include "voAMRNBEncID.h"
        .section  .text
	.global   Vo_weight_ai

Vo_weight_ai:
        STMFD          r13!, {r4 - r12, r14} 
        MOV            r12,  #0x8000 
        LDRSH          r4, [r0], #2                    @r4---a[0]
        LDRSH          r7, [r1], #2                    @r7---fac[0]
        LDRSH          r5, [r0], #2                    @r5---a[1]
        LDRSH          r8, [r1], #2                    @r8---fac[1]
        LDRSH          r6, [r0], #2                    @r6---a[2]
        VMOV.S32       Q4, #0x8000
        MUL            r9,  r5, r7
        STRH           r4, [r2], #2                    @a_exp[0] = a[0]
        VLD1.S16       {D2, D3}, [r1]!                 @ get 8 fac[]
        ADD            r10, r12, r9, LSL #1
        MUL            r11, r6, r8
        MOV            r10, r10, LSR #16             
        VLD1.S16       {D0, D1}, [r0]!                 @ get 8 a[]
        ADD            r4, r12, r11, LSL #1
        VMOV.S32       Q5, #0x8000
        MOV            r4, r4, LSR #16
        STRH           r10, [r2], #2
        VQDMLAL.S16    Q4, D0, D2                      @ get the mul<<1 + 0x8000 result
        VQDMLAL.S16    Q5, D1, D3 
        STRH           r4 , [r2], #2

        VSHRN.S32      D4, Q4, #16
        VSHRN.S32      D5, Q5, #16                     @ c1, c2 have right shift 16 bits 
        VST1.S16       {D4, D5}, [r2]! 

weight_asm_end: 
 
        LDMFD      r13!, {r4 - r12, r15}  
        .END
