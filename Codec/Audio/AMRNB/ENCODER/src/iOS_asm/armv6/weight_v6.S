@**************************************************************
@* Copyright 2003~2010 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@void Weight_Ai (
@    Word16 a[],         /* (i)     : a[M+1]  LPC coefficients   (M=10)    */
@    const Word16 fac[], /* (i)     : Spectral expansion factors.          */
@    Word16 a_exp[]      /* (o)     : Spectral expanded LPC coefficients   */
@)
        #include "voAMRNBEncID.h"
    	.text   .align    4
  	.globl   _Vo_weight_ai 

_Vo_weight_ai:
        stmfd          r13!, {r4 - r12, r14} 
        mov            r12,  #0x4000 
        ldrsh          r3, [r0], #2                    @r3---a[0]

        ldrsh          r4, [r1], #2                    @r4---fac[0]
        ldrsh          r5, [r0], #2                    @r5---a[1]

        ldrsh          r6, [r1], #2                    @r6---fac[1]
        ldrsh          r7, [r0], #2                    @r7---a[2]

        ldrsh          r8, [r1], #2                    @r8---fac[2]
	ldrsh          r9, [r0], #2                    @r9---a[3]

	mla            r10, r4, r5, r12                @fac[0] * a[1]
	mla            r11, r6, r7, r12                @fac[1] * a[2]

	mov            r4, r10, asr #15
	mov            r5, r11, asr #15

	mla            r14, r8, r9, r12                @fac[2] * a[3]
	
	ldrsh          r6, [r1], #2                    @fac[3]
	mov            r14, r14, asr #15

	ldrsh          r7, [r0], #2                    @a[4]

	strh           r3, [r2], #2
	strh           r4, [r2], #2
	strh           r5, [r2], #2
	strh           r14, [r2], #2

	@pkhbt          r8, r3, r4, lsl #16
	@pkhbt          r9, r5, r14, lsl #16

	ldrsh          r4, [r1], #2                    @fac[4]
	ldrsh          r5, [r0], #2                    @a[5]
        @STR            r8, [r2], #4
	@STR            r9, [r2], #4
	ldrsh          r8, [r1], #2                    @fac[5]
	ldrsh          r9, [r0], #2                    @a[6]

	mla            r3, r6, r7, r12                 @fac[3] * a[4]
	mla            r6, r4, r5, r12                 @fac[4] * a[5]
	mla            r7, r8, r9, r12                 @fac[5] * a[6]

	mov            r3, r3, asr #15
	mov            r6, r6, asr #15
	mov            r7, r7, asr #15

	ldrsh          r4, [r1], #2                    @fac[6]
	ldrsh          r5, [r0], #2                    @a[7]
	@pkhbt          r14, r3, r6, lsl #16
	strh           r3, [r2], #2
	strh           r6, [r2], #2
	ldrsh          r8, [r1], #2                    @fac[7]
	ldrsh          r9, [r0], #2                    @a[8]
        @STR            r14, [r2], #4
	mla            r14, r4, r5, r12                @fac[6] * a[7]
	mla            r3, r8, r9, r12                 @fac[7] * a[8]
	mov            r14, r14, asr #15

	ldrsh          r4, [r1], #2                    @fac[8]
	ldrsh          r5, [r0], #2                    @a[9]
        @pkhbt          r8, r7, r14, lsl #16
        strh           r7, [r2], #2
	strh           r14, [r2], #2
	mla            r9, r4, r5, r12                 @fac[8] * a[9]
	mov            r3, r3, asr #15
	mov            r9, r9, asr #15
	@STR            r8, [r2], #4

	ldrsh          r4, [r1], #2                    @fac[9]
	ldrsh          r5, [r0], #2                    @a[10]
        @pkhbt          r7, r3, r9, lsl #16
	strh           r3, [r2], #2
	strh           r9, [r2], #2
	mla            r8, r4, r5, r12                 @fac[9] * a[10]
	@STR            r7, [r2], #4
	mov            r4, r8, asr #15
	strh           r4, [r2]
  
weight_asm_end: 
 
        ldmfd          r13!, {r4 - r12, r15}  
        @.END

