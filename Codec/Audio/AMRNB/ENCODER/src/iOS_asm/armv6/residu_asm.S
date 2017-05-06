@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@void Residu (
@	     Word16 a[], /* (i)     : prediction coefficients */
@	     Word16 x[], /* (i)     : speech signal           */
@	     Word16 y[], /* (o)     : residual signal         */
@	     Word16 lg   /* (i)     : size of filtering       */
@	     )
        #include "voAMRNBEncID.h"
        .text   .align  4
	.globl  _Vo_Residu_asm

_Vo_Residu_asm:

        stmfd      r13!, {r4 - r12, r14}
        mov        r3, #40

	ldrsh      r4, [r0], #2                          @r4 --- a0
	ldrsh      r6, [r0], #2                          @a1
	ldrsh      r7, [r0], #2                          @a2
	ldrsh      r8, [r0], #2                          @a3
	ldrsh      r9, [r0], #2                          @a4
	pkhbt      r5, r6, r7, lsl #16                   @r5 --- a2, a1
	ldrsh      r7, [r0], #2                          @a5
	pkhbt      r6, r8, r9, lsl #16                   @r6 --- a4, a3
	ldrsh      r8, [r0], #2                          @a6
	ldrsh      r9, [r0], #2                          @a7
	ldrsh      r10, [r0], #2                         @a8
	pkhbt      r7, r7, r8, lsl #16                   @r7 --- a6, a5
	pkhbt      r8, r9, r10, lsl #16                  @r8 --- a8, a7
	ldrsh      r9, [r0], #2                          @a9
	ldrsh      r10, [r0], #2                         @a10
	pkhbt      r9, r9, r10, lsl #16                  @r9 --- a10, a9

        @r0, r10, r11, r12, r14	

LOOP:

        mov        r0, r1
	ldrsh      r10, [r0], #-2                        @x[0]
	ldrsh      r11, [r0], #-2                        @x[-1]
	mul        r14, r4, r10
	ldrsh      r12, [r0], #-2                        @x[-2]
	ldrsh      r10, [r0], #-2                        @x[-3]
	pkhbt      r11, r11, r12, lsl #16                @r11 --- x[-2] x[-1]
	ldrsh      r12, [r0], #-2                        @x[-4]
	smlad      r14, r11, r5, r14
	ldrsh      r11, [r0], #-2                        @x[-5]
	pkhbt      r10, r10, r12, lsl #16                @r10 --- x[-4] x[-3]
	ldrsh      r12, [r0], #-2                        @x[-6]
	smlad      r14, r10, r6, r14
	ldrsh      r10, [r0], #-2                        @x[-7]
	pkhbt      r11, r11, r12, lsl #16                @r11 --- x[-6] x[-5]
	ldrsh      r12, [r0], #-2                        @x[-8]
	smlad      r14, r11, r7, r14
	ldrsh      r11, [r0], #-2                        @x[-9]
	pkhbt      r10, r10, r12, lsl #16                @r10 --- x[-8] x[-7]
	ldrsh      r12, [r0], #-2                        @x[-10]
	smlad      r14, r10, r8, r14
	add        r1, r1, #2
        pkhbt      r11, r11, r12, lsl #16                @r11 --- x[-10] x[-9]	
        smlad      r14, r11, r9, r14

        ssat       r14, #32, r14, lsl #4 
        mov        r10, #0x8000
        qadd       r11, r14, r10
        subs       r3, r3, #1        
        mov        r11, r11, asr #16
	
        strh       r11, [r2], #2                          @y[i] = round(s)
   
	bgt        LOOP

        ldmfd      r13!, {r4 - r12, r15}
        @.END 


     
