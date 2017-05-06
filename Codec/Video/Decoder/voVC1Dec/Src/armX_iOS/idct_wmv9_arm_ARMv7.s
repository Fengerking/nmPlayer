@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************

@//************************************************************************
@//
@// Module Name:
@//
@//     idctARM.s
@//
@// Abstract:
@// 
@//     ARM specific transforms
@//     Optimized assembly routines to implement WMV9 8x8, 4x8, 8x4, 4x4 IDCT
@//
@//     Custom build with 
@//          armasm $(InputDir)\$(InputName).s $(OutDir)\$(InputName).obj
@//     and
@//          $(OutDir)\$(InputName).obj
@// 
@// Author:
@// 
@//     Chuang Gu (chuanggu@microsoft.com) Nov. 10, 2002
@//
@// Revision History:
@//
@//*************************************************************************
@//
@// r0 : x0, r1 : x1, ..., r8 : x8@
@// r14: blk[]
@// r9, r10, r11, r12 -> temporal registers
@//
@//*************************************************************************
		#include "../../Src/c/voVC1DID.h"
    .include "wmvdec_member_arm.h"
    .include "xplatform_arm_asm.h" 

    @AREA |.text|, CODE, READONLY
     .text
     .align 4
    
    .if WMV_OPT_IDCT_ARM == 1

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@   WMV9 transform
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .globl  _ARMV7_g_IDCTDec16_WMV3_SSIMD  
    .globl  _ARMV7_g_IDCTDec_WMV3_Pass1_Naked
    .globl  _ARMV7_g_IDCTDec_WMV3_Pass2_Naked
    	
	.align 4	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA    |.text|, CODE
    WMV_LEAF_ENTRY ARMV7_g_IDCTDec16_WMV3_SSIMD
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ r0 == piDst
@ r1 == piSrc
@ r2 == iOffsetToNextRowForDCT
@ r3 == iDCTHorzFlags

.set ST_piDst_4x4IDCTDec       , 0
.set ST_tmpBuffer_4x4IDCTDec   , 4
.set ST_SIZE_4x4IDCTDec        , 260

    stmdb     sp!, {r4 - r11, lr}
    FRAME_PROFILE_COUNT

    sub       sp, sp, #ST_SIZE_4x4IDCTDec
    str       r0, [sp, #ST_piDst_4x4IDCTDec]

@   g_IDCTDec_WMV3_Pass1(piSrc0, blk32, 4, iDCTHorzFlags)@
    mov       r0, r1
    mov       r2, #4
    add       r1, sp, #ST_tmpBuffer_4x4IDCTDec
    bl        _ARMV7_g_IDCTDec_WMV3_Pass1_Naked

@   g_IDCTDec_WMV3_Pass2(piSrc0, blk16, 4)@
    mov       r2, #4
    ldr       r1, [sp, #ST_piDst_4x4IDCTDec]
    add       r0, sp, #ST_tmpBuffer_4x4IDCTDec
    bl        _ARMV7_g_IDCTDec_WMV3_Pass2_Naked

    add       sp, sp, #ST_SIZE_4x4IDCTDec
    ldmia     sp!, {r4 - r11, pc}
    
    WMV_ENTRY_END	@ARMV7_g_IDCTDec16_WMV3_SSIMD

	.align 4	
Pass1_table:
		.long 15,12,24,20,6,4				

	.align 4	

@
@ Void_WMV g_IDCTDec_WMV3_Pass1(const I32_WMV  *piSrc0, I32_WMV * blk32, const I32_WMV iNumLoops, I32_WMV iDCTHorzFlags)
@ 

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA    |.text|, CODE
    WMV_LEAF_ENTRY ARMV7_g_IDCTDec_WMV3_Pass1_Naked
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@r0 == piSrc0
@r1 == blk32
@r2 == iNumLoops
@r3 == iDCTHorzFlags	
@r4 == pDst2
@r5 == SrcStride == DstStride

P1_RND			.req	d30
P1_0x8000		.req	d31
	
    str			lr,  [sp, #-4]!
    
	pld			[r0]
    adr			r12, Pass1_table
    mov			r5, #16					@4*4
	add			r4, r1, #8
    vmov.u16	P1_RND, #0x0004			@ rounding
    vmov.u32	P1_0x8000, #0x8000	
	vld1.u64	{q5}, [r12]!
 	vld1.u64	{d9}, [r12]
   
IDCT_P1_Loop:   

@		if(!(iDCTHorzFlags&3))
@		{
@			I32_WMV iCurr, iNext@
@			b0 == piSrc0[ i ]*W0 + (4+(4<<16))@ //12
@			b1 == (b0 + 0x8000)>>19@
@			b0 == ((I16_WMV)b0)>>3@
@			iCurr == (b0<<16) + b0@
@			iNext == (b1<<16) + b1@
@			blk32[0] == iCurr@
@			blk32[0+4] == iNext@
@			blk32[1] == iCurr@
@			blk32[1+4] == iNext@
@			blk32[2] == iCurr@
@			blk32[2+4] == iNext@
@			blk32[3] == iCurr@
@			blk32[3+4] == iNext@
@			continue@
@		}

    tst			r3, #15
    bne			P1FullTransform
    
	vld1.u64	d0, [r0]!
	subs		r2, r2, #2
	movne		r3, r3, asr #4
	
	vmul.s32	d0, d0, d10[1]
	vadd.s32	d0, d0, P1_RND
	vadd.s32	d1, d0, P1_0x8000		
	vshl.s32	d0, d0, #16
	vshr.s32	q0, q0, #19		
		
	vshl.s32	q1, q0, #16
	vadd.s32	q6, q0, q1		
	vdup.32		q0, d12[0]
	vdup.32		q2, d12[1]
	vdup.32		q1, d13[0]
	vdup.32		q3, d13[1]

	vst1.32		{q0}, [r1]!
	vst1.32		{q1}, [r1]!
	vst1.32		{q2}, [r1]!
	vst1.32		{q3}, [r1]!
	add			r4, r1, #8
	bne			IDCT_P1_Loop
    ldr			pc,  [sp], #4
	
P1FullTransform:

@		x4 == piSrc0[ i +1*4 ]@      
@		x3 == piSrc0[ i +2*4 ]@
@		x7 == piSrc0[ i +3*4 ]@
@		x1 == piSrc0[ i +4*4 ]@
@		x6 == piSrc0[ i +5*4 ]@
@		x2 == piSrc0[ i +6*4 ]@      
@		x5 == piSrc0[ i +7*4 ]@
@		x0 == piSrc0[ i +0*4 ]@ /* for proper rounding */

	vld1.u64	d0, [r0], r5
	vld1.u64	d4, [r0], r5
	vld1.u64	d3, [r0], r5
	vld1.u64	d7, [r0], r5
	vld1.u64	d1, [r0], r5
	vld1.u64	d6, [r0], r5
	vld1.u64	d2, [r0], r5
	vld1.u64	d5, [r0], r5
	subs		r2, r2, #2
	addne		r0, r0, #8
	subne		r0, r0, r5, lsl #3
	movne		r3, r3, asr #4

@		x1 == x1 * W0@   //12
@		x0 == x0 * W0 + (4+(4<<16))@ /* for proper rounding */
	
	vmul.s32	d0, d0, d10[1]
	vmul.s32	d1, d1, d10[1]

@		// zeroth stage
@		y3 == x4 + x5@
@		x8 == W3 * y3@           //15
@		x4a == x8 - W3pW5 * x5@  //24
@		x5a == x8 - W3_W5 * x4@  //6
@		x8 == W7 * y3@           //4
@		x4 == x8 + W1_W7 * x4@   //12
@		x5 == x8 - W1pW7 * x5@   //20

	vadd.s32	d12, d4, d5				@d12 == y3
	vmul.s32	d8, d12, d10[0]		@W3 * y3
	vmul.s32	d13, d5, d11[0]		@W3pW5 * x5
	vmul.s32	d14, d4, d9[0]		@W3_W5 * x4
	vmul.s32	d15, d4, d10[1]		@W1_W7 * x4
	vmul.s32	d16, d5, d11[1]		@W1pW7 * x5
	vadd.s32	d0, d0, P1_RND
	vshl.s32	d12, #2				@W7 * y3
	vsub.s32	d13, d8, d13		@ x4a
	vsub.s32	d14, d8, d14		@ x5a
	vadd.s32	d4, d12, d15		@ x4
	vsub.s32	d5, d12, d16		@ x5

@		// first stage
@		y3 == x6 + x7@
@		x8 == W7 * y3@           //4
@		x4a -== x8 + W1_W7 * x6@ //12
@		x5a +== x8 - W1pW7 * x7@ //20
@		x8 == W3 * y3@           //15
@		x4 +== x8 - W3_W5 * x6@  //6
@		x5 +== x8 - W3pW5 * x7@  //24

	vadd.s32	d12, d6, d7				@d12 == y3
	vshl.s32	d8, d12, #2
	vmul.s32	d12, d12, d10[0]	@W3 * y3
	vmul.s32	d15, d6, d10[1]		@W1_W7 * x6
	vmul.s32	d16, d7, d11[1]		@W1pW7 * x7
	vmul.s32	d17, d6, d9[0]		@W3_W5 * x6
	vmul.s32	d18, d7, d11[0]		@W3pW5 * x7
	vsub.s32	d13, d13, d8			
	vadd.s32	d14, d14, d8			
	vsub.s32	d13, d13, d15			@ x4a
	vsub.s32	d14, d14, d16			@ x5a
	vadd.s32	d4, d4, d12			
	vadd.s32	d5, d5, d12			
	vsub.s32	d4, d4, d17				@ x4			
	vsub.s32	d5, d5, d18				@ x5			

@		// second stage 
@		x8 == x0 + x1@
@		x0 -== x1@
@		x1 == W6 * x3 - W2 * x2@  //6,  16
@		x3 == W6 * x2 + W2A * x3@ //6,  16

	vmul.s32	d15, d3, d9[0]		@W6 * x3
	vmul.s32	d16, d2, d9[0]		@W6 * x2
	vshl.s32	d17, d2, #4				@W2 * x2
	vshl.s32	d18, d3, #4				@W2A * x3
	vadd.s32	d8, d0, d1			
	vsub.s32	d0, d0, d1
	vsub.s32	d1, d15, d17			@ x1
	vadd.s32	d3, d16, d18			@ x3

@		// third stage
@		x7 == x8 + x3@
@		x8 -== x3@
@		x3 == x0 + x1@
@		x0 -== x1@

	vadd.s32	d7, d8, d3			
	vsub.s32	d8, d8, d3
	vadd.s32	d3, d0, d1				
	vsub.s32	d0, d0, d1

@		// blk [0,1]
@		b0 == x7 + x4@	// sw: b0 == 12*x0 + 16*x4 + 16*x3 + 15*x7 + 12*x1 + 9*x6 + 6*x2 + 4*x5 + rounding
@		c0 == x3 + x4a@	// sw: c0 == 12*x0 + 15*x4 + 6*x3 + -4*x7 + -12*x1 + -16*x6 + -16*x2 + -9*x5 + rounding
@		b1 == (b0 + 0x8000)>>19@
@		c1 == (c0 + 0x8000)>>19@
@		b0 == ((I16_WMV)b0)>>3@
@		c0 == ((I16_WMV)c0)>>3@

	vadd.s32	d16, d7, d4			@ b0
	vadd.s32	d17, d3, d13		@ c0	
	vadd.s32	d18, d16, P1_0x8000		
	vadd.s32	d19, d17, P1_0x8000			
	vshl.s32	q8, q8, #16
	vshr.s32	q9, q9, #19			@ b1 c1
	vshr.s32	q8, q8, #19			@ b0 c0

@		blk32[0] == (c0<<16) + b0@
@		blk32[0+4] == (c1<<16) + b1@

	vswp.s64	d17, d18
	vshl.s32	q9, q9, #16
	vadd.s32	q8, q8, q9		@ d16: b0 c0 b0 c0, d17: b1 c1 b1 c1 
								@	   00 10 02 12,      01 11 03 13
@		// blk [2,3]
@		b0 == x0 + x5a@
@		c0 == x8 + x5@
@		b1 == (b0 + 0x8000)>>19@
@		c1 == (c0 + 0x8000)>>19@
@		b0 == ((I16_WMV)b0)>>3@
@		c0 == ((I16_WMV)c0)>>3@

	vadd.s32	d18, d0, d14		@ b0
	vadd.s32	d19, d8, d5			@ c0	
	vadd.s32	d20, d18, P1_0x8000		
	vadd.s32	d21, d19, P1_0x8000			
	vshl.s32	q9, q9, #16
	vshr.s32	q10, q10, #19		@ b1 c1
	vshr.s32	q9, q9, #19			@ b0 c0

@		blk32[1] == (c0<<16) + b0@
@		blk32[1+4] == (c1<<16) + b1@

	vswp.s64	d19, d20
	vshl.s32	q10, q10, #16
	vadd.s32	q9, q9, q10		@ d18: b0 c0 b0 c0, d19: b1 c1 b1 c1 
								@	   20 30 22 32		 21 31 23 33
@	d16 - 00 10 02 12      00 10 20 30
@	d17 - 01 11 03 13  ==>  01 11 21 31
@	d18 - 20 30 22 32	   02 12 22 32
@	d19 - 21 31 23 33	   03 13 23 33
	vtrn.32		d16, d18
	vtrn.32		d17, d19
	
	vst1.32		d16, [r1], r5
	vst1.32		d17, [r1], r5
	vst1.32		d18, [r1], r5
	vst1.32		d19, [r1], r5	

@		// blk [4,5]
@		b0 == x8 - x5@
@		c0 == x0 - x5a@
@		b1 == (b0 + 0x8000)>>19@
@		c1 == (c0 + 0x8000)>>19@
@		b0 == ((I16_WMV)b0)>>3@
@		c0 == ((I16_WMV)c0)>>3@

	vsub.s32	d16, d8, d5			@ b0
	vsub.s32	d17, d0, d14		@ c0	
	vadd.s32	d18, d16, P1_0x8000		
	vadd.s32	d19, d17, P1_0x8000			
	vshl.s32	q8, q8, #16
	vshr.s32	q9, q9, #19			@ b1 c1
	vshr.s32	q8, q8, #19			@ b0 c0

@		blk32[2] == (c0<<16) + b0@
@		blk32[2+4] == (c1<<16) + b1@

	vswp.s64	d17, d18
	vshl.s32	q9, q9, #16
	vadd.s32	q8, q8, q9		@ d16: b0 c0 b0 c0, d17: b1 c1 b1 c1 
								@	   00 10 02 12,      01 11 03 13

@		// blk [6,7]
@		b0 == x3 - x4a@
@		c0 == x7 - x4@
@		b1 == (b0 + 0x8000)>>19@
@		c1 == (c0 + 0x8000)>>19@
@		b0 == ((I16_WMV)b0)>>3@
@		c0 == ((I16_WMV)c0)>>3@

	vsub.s32	d18, d3, d13		@ b0
	vsub.s32	d19, d7, d4			@ c0	
	vadd.s32	d20, d18, P1_0x8000		
	vadd.s32	d21, d19, P1_0x8000			
	vshl.s32	q9, q9, #16
	vshr.s32	q10, q10, #19		@ b1 c1
	vshr.s32	q9, q9, #19			@ b0 c0

@		blk32[3] == (c0<<16) + b0@
@		blk32[3+4] == (c1<<16) + b1@

	vswp.s64	d19, d20
	vshl.s32	q10, q10, #16
	vadd.s32	q9, q9, q10		@ d18: b0 c0 b0 c0, d19: b1 c1 b1 c1 
								@	   20 30 22 32,      21 31 23 33
@	d16 - 00 10 02 12      00 10 20 30
@	d17 - 01 11 03 13  ==>  01 11 21 31
@	d18 - 20 30 22 32	   02 12 22 32
@	d19 - 21 31 23 33	   03 13 23 33
	vtrn.32		d16, d18
	vtrn.32		d17, d19
	
	vst1.32		d16, [r4], r5
	vst1.32		d17, [r4], r5
	vst1.32		d18, [r4], r5
	vst1.32		d19, [r4], r5	
	bne			IDCT_P1_Loop
	
    ldr			pc,  [sp], #4

    WMV_ENTRY_END	@ARMV7_g_IDCTDec_WMV3_Pass1_Naked

	.align 8	
Pass2_table:
		.word -3,6,-5,7,-11,-10,-12,-6				

@
@Void_WMV g_IDCTDec_WMV3_Pass2(const I32_WMV  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA    |.text|, CODE
    WMV_LEAF_ENTRY ARMV7_g_IDCTDec_WMV3_Pass2_Naked
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@r0 == piSrc0
@r1 == blk16
@r2 == iNumLoops
@r3 == pDst2
@r4 == SrcStride 
@r5 == DstStride

P2_RND			.req	d29
P2_0x8000		.req	d30
P2_0xFFFF7FFF	.req	d31

    str			lr,  [sp, #-4]!
    
	pld			[r0]
    adr			r12, Pass2_table
    mov			r4, #16					@4*4
    mov			r5, #8					@4*2
    vmov.u32	P2_0xFFFF7FFF, #0xffff7fff	
    vmov.u16	P2_RND, #0x0020			@ rounding
    vmov.u32	P2_0x8000, #0x8000	
	vld1.u64	{q5}, [r12]!
	vld1.u64	{q6}, [r12]
    
IDCT_P2_Loop:   

@		x0 == piSrc0[i + 0*4 ] * 6 + 32 + (32<<16) /* rounding */@
@		x4 == piSrc0[i + 1*4 ]@
@		x3 == piSrc0[i + 2*4 ]@
@		x7 == piSrc0[i + 3*4 ]@
@		x1 == piSrc0[i + 4*4 ] * 6@
@		x6 == piSrc0[i + 5*4 ]@
@		x2 == piSrc0[i + 6*4 ]@
@		x5 == piSrc0[i + 7*4 ]@

	vld1.u32	d0, [r0], r4
	vld1.u32	d4, [r0], r4
	vld1.u32	d3, [r0], r4
	vld1.u32	d7, [r0], r4
	vld1.u32	d1, [r0], r4
	vld1.u32	d6, [r0], r4
	vld1.u32	d2, [r0], r4
	vld1.u32	d5, [r0], r4
    add			r3, r1, #64				@32*2
	subs		r2, r2, #2
	addne		r0, r0, #8
	subne		r0, r0, r4, lsl #3
	
	vmul.s32	d0, d0, d10[1]
	vmul.s32	d1, d1, d10[1]
	
@		// zeroth stage
@		y4a == x4 + x5@
@		x8 == 7 * y4a@
@		x4a == x8 - 12 * x5@
@		x5a == x8 - 3 * x4@
@		x8 == 2 * y4a@
@		x4 == x8 + 6 * x4@
@		x5 == x8 - 10 * x5@

	vadd.s32	d15, d4, d5				@  y4a
	vmul.s32	d8, d15, d11[1]		@7 * y4a
	vmul.s32	d9, d5, d13[0]		@-12 * x5
	vmul.s32	d14, d4, d10[0]		@ -3 * x4
	vmul.s32	d4, d4, d10[1]		@6 * x4
	vmul.s32	d5, d5, d12[1]		@-10 * x5
	vadd.s32	d0, d0, P2_RND
	vshl.s32	d18, d15, #1
	vadd.s32	d9, d9, d8				@ x4a
	vadd.s32	d14, d14, d8			@ x5a
	vadd.s32	d4, d4, d18				@ x4
	vadd.s32	d5, d5, d18				@ x5

@		ls_signbit==y4a&0x8000@
@		y4a == (y4a >> 1) - ls_signbit@
@		y4a == y4a & ~0x8000@
@		y4a == y4a | ls_signbit@
@		x4a +== y4a@
@		x5a +== y4a@

	vand		d16, d15, P2_0x8000
	vshr.s32	d15, #1
	vsub.s32	d15, d15, d16
	vand		d15, d15, P2_0xFFFF7FFF
	vorr		d15, d15, d16
	vadd.s32	d9, d9, d15			@ x4a
	vadd.s32	d14, d14, d15		@ x5a

@		// first stage
@		y4 == x6 + x7@
@		x8 == 2 * y4@
@		x4a -== x8 + 6 * x6@
@		x5a +== x8 - 10 * x7@
@		x8 == 7 * y4@

	vadd.s32	d15, d6, d7				@  y4
	vshl.s32	d8, d15, #1
	vsub.s32	d9, d9, d8
	vadd.s32	d14, d14, d8
	vmla.s32	d9, d6, d13[1]		@  x4a == x4a -x8 + -6 * x6
	vmla.s32	d14, d7, d12[1]		@  x5a == x5a + x8 + -10 * x7
	vmul.s32	d8, d15, d11[1]		@  x8 == 7 * y4

@		ls_signbit==y4&0x8000@
@		y4 == (y4 >> 1) - ls_signbit@
@		y4 == y4 & ~0x8000@
@		y4 == y4 | ls_signbit@
@		x8 +== y4@
@		x4 +== x8 - 3 * x6@
@		x5 +== x8 - 12 * x7@

	vand		d16, d15, P2_0x8000
	vshr.s32	d15, #1
	vsub.s32	d15, d15, d16
	vand		d15, d15, P2_0xFFFF7FFF
	vorr		d15, d15, d16
	vadd.s32	d8, d8, d15
	vadd.s32	d4, d4, d8
	vadd.s32	d5, d5, d8
	vmla.s32	d4, d6, d10[0]		@  x4 +== x8 + -3 * x6
	vmla.s32	d5, d7, d13[0]		@  x5 +== x8 + -12 * x7

@		// second stage 
@		x8 == x0 + x1@
@		x0 -== x1@
@		x1 == 8 * (x2 + x3)@
@		x6 == x1 - 5 * x2@
@		x1 -== 11 * x3@

	vmul.s32	d15, d2, d11[0]		@-5 * x2
	vadd.s32	d8, d0, d1
	vsub.s32	d0, d0, d1
	vadd.s32	d1, d2, d3
	vshl.s32	d1, #3
	vadd.s32	d6, d1, d15
	vmla.s32	d1, d3, d12[0]		@x1 +== -11 * x3
	
@		// third stage
@		x7 == x8 + x6@
@		x8 -== x6@
@		x6 == x0 - x1@
@		x0 +== x1@

	vadd.s32	d7, d8, d6
	vsub.s32	d8, d8, d6
	vsub.s32	d6, d0, d1
	vadd.s32	d0, d0, d1

@		// blk0
@		b0 == (x7 + x4)@
@		b1 == (b0 + 0x8000)>>22@
@		b0 == ((I16_WMV)b0)>>6@
@		blk16[ i + 0   + 0*4] == b0@
@		blk16[ i + 32 + 0*4] == b1@
@		// blk1
@		b0 == (x6 + x4a)@
@		b1 == (b0 + 0x8000)>>22@
@		b0 == ((I16_WMV)b0)>>6@
@		blk16[ i + 0 + 1*4] == b0@
@		blk16[ i + 32 + 1*4] == b1@

	vadd.s32	d16, d7, d4	
	vadd.s32	d17, d6, d9	
	vadd.s32	d18, d16, P2_0x8000	
	vadd.s32	d19, d17, P2_0x8000	
	vshr.s32	d18, #22
	vshr.s32	d19, #22
	vmovn.i32	d16, q8				@b0
	vmovn.i32	d17, q9				@b1
	vshr.s16	d16, #6
	vst1.32		d16[0], [r1], r5
	vst1.32		d16[1], [r1], r5
	vst1.32		d17[0], [r3], r5
	vst1.32		d17[1], [r3], r5

@		// blk2
@		b0 == (x0 + x5a)@
@		b1 == (b0 + 0x8000)>>22@
@		b0 == ((I16_WMV)b0)>>6@
@		blk16[ i + 0 + 2*4] == b0@
@		blk16[ i + 32 + 2*4] == b1@
@		// blk3
@		b0 == (x8 + x5)@
@		b1 == (b0 + 0x8000)>>22@
@		b0 == ((I16_WMV)b0)>>6@
@		blk16[ i + 0 + 3*4] == b0@
@		blk16[ i + 32 + 3*4] == b1@

	vadd.s32	d16, d0, d14	
	vadd.s32	d17, d8, d5	
	vadd.s32	d18, d16, P2_0x8000	
	vadd.s32	d19, d17, P2_0x8000	
	vshr.s32	d18, #22
	vshr.s32	d19, #22
	vmovn.i32	d16, q8				@b0
	vmovn.i32	d17, q9				@b1
	vshr.s16	d16, #6
	vst1.32		d16[0], [r1], r5
	vst1.32		d16[1], [r1], r5
	vst1.32		d17[0], [r3], r5
	vst1.32		d17[1], [r3], r5

@		// blk4
@		b0 == (x8 - x5)@
@		b1 == (b0 + 0x8000)>>22@
@		b0 == ((I16_WMV)b0)>>6@
@		blk16[ i + 0 + 4*4] == b0@
@		blk16[ i + 32 + 4*4] == b1@
@		// blk5
@		b0 == (x0 - x5a)@
@		b1 == (b0 + 0x8000)>>22@
@		b0 == ((I16_WMV)b0)>>6@
@		blk16[ i + 0 + 5*4] == b0@
@		blk16[ i + 32 + 5*4] == b1@

	vsub.s32	d16, d8, d5	
	vsub.s32	d17, d0, d14	
	vadd.s32	d18, d16, P2_0x8000	
	vadd.s32	d19, d17, P2_0x8000	
	vshr.s32	d18, #22
	vshr.s32	d19, #22
	vmovn.i32	d16, q8				@b0
	vmovn.i32	d17, q9				@b1
	vshr.s16	d16, #6
	vst1.32		d16[0], [r1], r5
	vst1.32		d16[1], [r1], r5
	vst1.32		d17[0], [r3], r5
	vst1.32		d17[1], [r3], r5

@		// blk6
@		b0 == (x6 - x4a)@
@		b1 == (b0 + 0x8000)>>22@
@		b0 == ((I16_WMV)b0)>>6@
@		blk16[ i + 0 + 6*4] == b0@
@		blk16[ i + 32 + 6*4] == b1@
@		// blk7
@		b0 == (x7 - x4)@
@		b1 == (b0 + 0x8000)>>22@
@		b0 == ((I16_WMV)b0)>>6@
@		blk16[ i + 0 + 7*4] == b0@
@		blk16[ i + 32 + 7*4] == b1@

	vsub.s32	d16, d6, d9	
	vsub.s32	d17, d7, d4	
	vadd.s32	d18, d16, P2_0x8000	
	vadd.s32	d19, d17, P2_0x8000	
	vshr.s32	d18, #22
	vshr.s32	d19, #22
	vmovn.i32	d16, q8				@b0
	vmovn.i32	d17, q9				@b1
	vshr.s16	d16, #6
	vst1.32		d16[0], [r1], r5
	vst1.32		d16[1], [r1], r5
	vst1.32		d17[0], [r3], r5
	vst1.32		d17[1], [r3], r5
	
	addne		r1, r1, #4
	subne		r1, r1, r5, lsl #3
	bne			IDCT_P2_Loop
	
    ldr       pc,  [sp], #4

    WMV_ENTRY_END	@ARMV7_g_IDCTDec_WMV3_Pass2_Naked

  .endif @ WMV_OPT_IDCT_ARM
