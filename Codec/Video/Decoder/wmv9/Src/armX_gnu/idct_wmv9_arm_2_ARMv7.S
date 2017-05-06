    #include "../c/voWMVDecID.h"
    .include "wmvdec_member_arm.inc"
    .include "xplatform_arm_asm.h" 

    @AREA |.text|, CODE, READONLY
    .section .text

    .IF WMV_OPT_IDCT_ARM == 1
    
    .global  ARMV7_IntraBlockDequant8x8
    .global  ARMV7_IntraDequantACPred

    .global  ARMV7_g_IDCTDec_WMV3
    .global  ARMV7_g_8x8IDCT
    .global  ARMV7_g_8x4IDCT
    .global  ARMV7_g_4x8IDCT
    .global  ARMV7_g_4x4IDCT
    

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.macro	M_IntraDequant	in1, in2	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@	
@    for (i = 2@ i < 64@ i += 2) {
@        iValue0 = rgiCoefRecon[i]@
@        if (iValue0) {
@            int signMask = iValue0 >> 31@ // 0 or FFFFFFFF
@            iValue0 = (I16_WMV) ((I32_WMV) iDoubleStepSize * iValue0 + (signMask ^ iStepMinusStepIsEven) - signMask)@
@        }
@        iValue1 = rgiCoefRecon[i+1]@
@        if (iValue1) {
@            int signMask = iValue1 >> 31@ // 0 or FFFFFFFF
@            iValue1 = (I16_WMV) ((I32_WMV) iDoubleStepSize * iValue1 + (signMask ^ iStepMinusStepIsEven) - signMask)@
@        }
@        *(I32_WMV *) (rgiCoefRecon + i) = iValue0 + (iValue1<<16)@
@    }
	vshr.s16	q8, \in1, #15	@signMask
	vshr.s16	q9, \in2, #15		
	veor.16		q10,q8, q15		@signMask ^ iStepMinusStepIsEven
	veor.16		q11,q9, q15
	vsub.s16	q10,q10,q8		@- signMask
	vsub.s16	q11,q11,q9
	vmla.s16	q10,\in1,q14	@+ iDoubleStepSize * iValue0
	vmla.s16	q11,\in2,q14	
	vshl.u32	q12,q8 , #16	@signMask<<16
	vshl.u32	q13,q9 , #16	
	vmov.i64	q8, #-1
	vtst.32		\in1, \in1, q8	@zeroMask = (iValue0 == 0)? 0x0 : ~0x0@
	vtst.32		\in2, \in2, q8	
	vand.32		q10, q10, \in1	@zeroMask & ( iDoubleStepSize * iValue0 + (signMask ^ iStepMinusStepIsEven) - signMask)
	vand.32		q11, q11, \in2
	vadd.s32	q10,q10,q12		@iValue0 + (iValue1<<16)
	vadd.s32	q11,q11,q13		
	vst1.64		{q10}, [lr,:64]!
	vst1.64		{q11}, [lr,:64]!	
	.endm

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA |.text|, CODE, READONLY
    WMV_LEAF_ENTRY ARMV7_IntraBlockDequant8x8
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@r0:iDCStepSize
@r1:iDoubleStepSize
@r2:iStepMinusStepIsEven
@r3:rgiCoefRecon
@r12:rgiCoefRecon[0],rgiCoefRecon[1]
@lr :backup r3

	push		{r4-r5,lr}

	ldr			r12, [r3]
	mov			lr, r3
	vld1.64		{q0}, [r3,:64]!
	vdup.16		q14, r1		@iDoubleStepSize
	vld1.64		{q1}, [r3,:64]!
	vdup.16		q15, r2		@iStepMinusStepIsEven
	vld1.64		{q2}, [r3,:64]!
	vld1.64		{q3}, [r3,:64]!
	vld1.64		{q4}, [r3,:64]!
	vld1.64		{q5}, [r3,:64]!
	vld1.64		{q6}, [r3,:64]!
	vld1.64		{q7}, [r3,:64]
	
	M_IntraDequant	q0, q1
	M_IntraDequant	q2, q3
	M_IntraDequant	q4, q5
	M_IntraDequant	q6, q7
	
@    iValue0 = (rgiCoefRecon[0] * iDCStepSize)@
@    iValue1 = rgiCoefRecon[1]@
@    if(iValue1) {
@        int signMask = iValue1 >> 31@
@        iValue1 = iDoubleStepSize * iValue1+ (signMask ^ iStepMinusStepIsEven) - signMask@
@    }
@    *(I32_WMV *) rgiCoefRecon = iValue0 + (iValue1<<16)@

	lsl		lr, r12, #16
	asr		lr, #16		
	mul		r0, r0, lr			@iValue0 = rgiCoefRecon[0] * iDCStepSize
	asr		r4, r12, #16		@iValue1
	asr		r5, r4 , #15		@signMask = iValue1 >> 31@
	mul		r1, r1, r4			@iDoubleStepSize * iValue1
	eor		r2, r2, r5
	sub		r3, r3, #16*7
	sub		r2, r2, r5
	add		r1, r1, r2
	add		r1, r0, r1, lsl #16	@iValue0 + (iValue1<<16)
	str		r1, [r3]
	
	pop		{r4-r5,pc}
	
	WMV_ENTRY_END	@ARMV7_IntraBlockDequant8x8


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA |.text|, CODE, READONLY
    WMV_LEAF_ENTRY ARMV7_IntraDequantACPred
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@r0:pDct
@r1:pWMVDec->m_rgiCoefRecon
@r2:iDoubleStepSize
@r3:iStepMinusStepIsEven
@lr:backup of pWMVDec->m_rgiCoefRecon[0]
@lr :backup r1

@    for (i = 1@ i < 8@ i++){
@        I32_WMV iLevel1 = pDct [i]@
@        int signMask = iLevel1 >> 31@ // 0 or FFFFFFFF
@        int zeroMask = (iLevel1 == 0)? 0x0 : ~0x0@
@        pWMVDec->m_rgiCoefRecon[i] = zeroMask & (iDoubleStepSize * iLevel1 + (signMask ^ iStepMinusStepIsEven) - signMask)@
@    }
@    for (i = 1@ i < 8@ i++) {
@        int iLevel1 = pDct [i + BLOCK_SIZE]@
@        int signMask2 = iLevel1 >> 31@ // 0 or FFFFFFFF
@        int zeroMask2 = (iLevel1 == 0)? 0x0 : ~0x0@
@        pWMVDec->m_rgiCoefRecon [i << 3] = zeroMask2 & (iDoubleStepSize * iLevel1 + (signMask2 ^ iStepMinusStepIsEven) - signMask2)@
@    }
	
	push		{r4-r5, lr}
	
	ldr			lr, [r1]		@backup
	mov			r4 , r1
	mov			r12 , #8*4
	vld1.64		{q0}, [r0]!
	vdup.16		q14, r2			@iDoubleStepSize
	vld1.64		{q1}, [r0]
	vdup.16		q15, r3			@iStepMinusStepIsEven
	vmov.i64	q13, #-1
	
	vshr.s16	q8, q0 , #15	@signMask2
	vshr.s16	q9, q1 , #15		
	veor.16		q10,q8, q15		@signMask2 ^ iStepMinusStepIsEven
	veor.16		q11,q9, q15
	vsub.s16	q10,q10,q8		@- signMask2
	vsub.s16	q11,q11,q9
	vmla.s16	q10,q0 ,q14		@+ iDoubleStepSize * iLevel1
	vmla.s16	q11,q1 ,q14	
	vtst.16		q6, q0, q13		@zeroMask2 = (iLevel1 == 0)? 0x0 : ~0x0@
	vtst.16		q7, q1, q13	
	vand.16		q10, q10, q6	@zeroMask2 & ( ...)
	vand.16		q11, q11, q7
	vmovl.s16	q2, d20
	vmovl.s16	q3, d21
	vmovl.s16	q4, d22
	vmovl.s16	q5, d23
	vst1.64		{q2}, [r4]!
	vst1.64		{q3}, [r4]!
	vst1.32		d8[1], [r4], r12
	vst1.32		d9[0], [r4], r12
	vst1.32		d9[1], [r4], r12
	vst1.32		d10[0], [r4], r12
	vst1.32		d10[1], [r4], r12
	vst1.32		d11[0], [r4], r12
	vst1.32		d11[1], [r4]
	
	sub			r4, r4, #8*4*7
	str			lr, [r4]
	pop			{r4-r5, pc}

	WMV_ENTRY_END	@ARMV7_IntraDequantACPred
    			
	

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA |.text|, CODE, READONLY
    WMV_LEAF_ENTRY ARMV7_g_IDCTDec_WMV3
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

IDCT_0xFFFF7FFF_D	.req	d30		@for IDCT_Loop2
IDCT_0x00008000_D   .req	d31		@for IDCT_Loop2
IDCT_0x80008000_D   .req	d31		@for IDCT_Loop2
	
    stmdb     sp!, {r4, lr}
    FRAME_PROFILE_COUNT
		
@r0 == piDst
@r1 == iOffsetToNextRowForDCT
@r2 == rgiCoefRecon
@r3 == tmp
@r4 == SrcStride
@q0,q1,q2,q3,q4,q5,q14,q15 == x0,x1,x2,x3,x4,x5,x6,x7
@q6,q7 == data of Pass1_table
@q12,q13 == x4a,x5a
@q8,q9,q10,q11 == tmp

	adr			r12, Pass1_table
	pld			[r2]
    mov			r4, #16					@4*4
	vld1.u64	{q6}, [r12]!
 	vld1.u64	d14, [r12]
   
@IDCT_Loop1   

@		x4 == piSrc0[ i +1*4 ]@      
@		x3 == piSrc0[ i +2*4 ]@
@		x7 == piSrc0[ i +3*4 ]@
@		x1 == piSrc0[ i +4*4 ]@
@		x6 == piSrc0[ i +5*4 ]@
@		x2 == piSrc0[ i +6*4 ]@      
@		x5 == piSrc0[ i +7*4 ]@
@		x0 == piSrc0[ i +0*4 ]@ /* for proper rounding */

	vld1.u64	{q0} , [r2], r4
	vld1.u64	{q4} , [r2], r4
	vld1.u64	{q3} , [r2], r4
	vld1.u64	{q15}, [r2], r4
	vld1.u64	{q1} , [r2], r4
	vld1.u64	{q14}, [r2], r4
	vld1.u64	{q2} , [r2], r4
	vld1.u64	{q5} , [r2]

@		x1 == x1 * W0@   //12
@		x0 == x0 * W0 + (4+(4<<16))@ /* for proper rounding */
	
	vmul.s32	q0, q0, d12[1]
	vmul.s32	q1, q1, d12[1]

@		// zeroth stage
@		y3 == x4 + x5@
@		x8 == W3 * y3@           //15
@		x4a == x8 - W3pW5 * x5@  //24
@		x5a == x8 - W3_W5 * x4@  //6
@		x8 == W7 * y3@           //4
@		x4 == x8 + W1_W7 * x4@   //12
@		x5 == x8 - W1pW7 * x5@   //20

	vadd.s32	q9 , q4, q5			@q9 == y3
	vmul.s32	q8 , q9, d12[0]		@W3 * y3
	vmul.s32	q12, q5, d13[0]		@W3pW5 * x5
	vmul.s32	q13, q4, d14[0]		@W3_W5 * x4
	vmul.s32	q10, q4, d12[1]		@W1_W7 * x4
	vmul.s32	q11, q5, d13[1]		@W1pW7 * x5
	vshl.s32	q9 , #2				@W7 * y3
	vsub.s32	q12, q8 , q12		@ x4a
	vsub.s32	q13, q8 , q13		@ x5a
	vadd.s32	q4 , q9 , q10		@ x4
	vsub.s32	q5 , q9, q11		@ x5

@		// first stage
@		y3 == x6 + x7@
@		x8 == W7 * y3@           //4
@		x4a -== x8 + W1_W7 * x6@ //12
@		x5a +== x8 - W1pW7 * x7@ //20
@		x8 == W3 * y3@           //15
@		x4 +== x8 - W3_W5 * x6@  //6
@		x5 +== x8 - W3pW5 * x7@  //24

	vadd.s32	q9 , q14, q15		@
	vshl.s32	q8 , q9, #2
	vmul.s32	q10, q14, d12[1]		@W1_W7 * x6
	vmul.s32	q11, q15, d13[1]		@W1pW7 * x7
	vmul.s32	q9 , q9, d12[0]		@W3 * y3	
	vsub.s32	q12, q12, q8			
	vadd.s32	q13, q13, q8			
	vsub.s32	q12, q12, q10		@ x4a
	vsub.s32	q13, q13, q11		@ x5a
	vmul.s32	q10, q14, d14[0]	@W3_W5 * x6
	vmul.s32	q11, q15, d13[0]	@W3pW5 * x7	
	vadd.s32	q4 , q4 , q9			
	vadd.s32	q5 , q5 , q9			
	vsub.s32	q4 , q4 , q10		@ x4			
	vsub.s32	q5 , q5 , q11		@ x5			

@		// second stage 
@		x8 == x0 + x1@
@		x0 -== x1@
@		x1 == W6 * x3 - W2 * x2@  //6,  16
@		x3 == W6 * x2 + W2A * x3@ //6,  16

	vmul.s32	q10, q3, d14[0]			@W6 * x3
	vmov.u16	q9, #0x0004				@ rounding
	vadd.s32	q0 , q0, q9
	vmul.s32	q11, q2, d14[0]			@W6 * x2
	vadd.s32	q8, q0 , q1			
	vsub.s32	q0, q0 , q1
	vshl.s32	q1, q2, #4				@W2 * x2
	vshl.s32	q3, q3, #4				@W2A * x3
	vsub.s32	q1, q10, q1				@ x1
	vadd.s32	q3, q11, q3				@ x3

@		// third stage
@		x7 == x8 + x3@
@		x8 -== x3@
@		x3 == x0 + x1@
@		x0 -== x1@

	vadd.s32	q15, q8, q3			
	vsub.s32	q8, q8, q3
	vadd.s32	q3, q0, q1				
	vsub.s32	q0, q0, q1

@	free registers below: q1,q2,q6,q7,q9,q10,q11,q14
@   The out data is stored as follow: 
@   q0: row 0
@   q4: row 1
@   q3: row 2
@   q14:row 3
@   q1: row 4
@   q13:row 5
@   q2: row 6
@   q5: row 7

@		// blk [0,1]
@		b0 == x7 + x4@	// sw: b0 == 12*x0 + 16*x4 + 16*x3 + 15*x7 + 12*x1 + 9*x6 + 6*x2 + 4*x5 + rounding
@		c0 == x3 + x4a@	// sw: c0 == 12*x0 + 15*x4 + 6*x3 + -4*x7 + -12*x1 + -16*x6 + -16*x2 + -9*x5 + rounding
@		b1 == (b0 + 0x8000)>>19@
@		c1 == (c0 + 0x8000)>>19@
@		b0 == ((I16_WMV)b0)>>3@
@		c0 == ((I16_WMV)c0)>>3@
@		blk32[0] == (c0<<16) + b0@
@		blk32[0+4] == (c1<<16) + b1@

    vmov.u32	q14, #0x8000	
    vshl.s64	q9, q0, #0		@ backup x0
    vshl.s64	q6, q4, #0		@ backup x4
    vshl.s64	q7, q5, #0		@ backup x5
    
	vadd.s32	q0, q15, q4			@ b0
	vadd.s32	q10, q3, q12		@ c0	
	vadd.s32	q4, q0, q14			@ b1	
	vadd.s32	q11, q10, q14		@ c1
	vshl.s32	q0, q0, #16
	vshl.s32	q10, q10, #16
	vshr.s32	q0, q0, #19			@ b0
	vshr.s32	q10, q10, #19		@ c0
	vshr.s32	q4, q4, #19			@ b1
	vshr.s32	q11, q11, #19		@ c1	
	vshl.s32	q10, q10, #16
	vshl.s32	q11, q11, #16
	vadd.s32	q0, q0, q10			@ c0 b0 --row 0  q0: 16 06 14 04, 02 02 10 00
	vadd.s32	q4, q4, q11			@ c1 b1 --row 1  q4: 17 07 15 05, 13 03 11 01

@		// blk [6,7]
@		b0 == x3 - x4a@
@		c0 == x7 - x4@
@		b1 == (b0 + 0x8000)>>19@
@		c1 == (c0 + 0x8000)>>19@
@		b0 == ((I16_WMV)b0)>>3@
@		c0 == ((I16_WMV)c0)>>3@
@		blk32[3] == (c0<<16) + b0@
@		blk32[3+4] == (c1<<16) + b1@

	vsub.s32	q2, q3, q12			@ b0
	vsub.s32	q10, q15, q6		@ c0	
	vadd.s32	q5, q2, q14			@ b1	
	vadd.s32	q11, q10, q14		@ c1
	vshl.s32	q2, q2, #16
	vshl.s32	q10, q10, #16
	vshr.s32	q2, q2, #19			@ b0
	vshr.s32	q10, q10, #19		@ c0
	vshr.s32	q5, q5, #19			@ b1
	vshr.s32	q11, q11, #19		@ c1	
	vshl.s32	q10, q10, #16
	vshl.s32	q11, q11, #16
	vadd.s32	q2, q2, q10			@ c0 b0 --row 6  q2: 76 66 74 64, 72 62 70 60
	vadd.s32	q5, q5, q11			@ c1 b1 --row 7  q5: 77 67 75 65, 73 63 71 61

@		// blk [2,3]
@		b0 == x0 + x5a@
@		c0 == x8 + x5@
@		b1 == (b0 + 0x8000)>>19@
@		c1 == (c0 + 0x8000)>>19@
@		b0 == ((I16_WMV)b0)>>3@
@		c0 == ((I16_WMV)c0)>>3@
@		blk32[1] == (c0<<16) + b0@
@		blk32[1+4] == (c1<<16) + b1@

    vmov.u32	q15, #0x8000	
	vadd.s32	q3, q9, q13			@ b0
	vadd.s32	q10, q8, q7			@ c0	
	vadd.s32	q14, q3, q15		@ b1	
	vadd.s32	q11, q10, q15		@ c1
	vshl.s32	q3, q3, #16
	vshl.s32	q10, q10, #16
	vshr.s32	q3, q3, #19			@ b0
	vshr.s32	q10, q10, #19		@ c0
	vshr.s32	q14, q14, #19		@ b1
	vshr.s32	q11, q11, #19		@ c1	
	vshl.s32	q10, q10, #16
	vshl.s32	q11, q11, #16
	vadd.s32	q3, q3, q10			@ c0 b0 --row 2  q3: 36 26 34 24, 32 22 30 20
	vadd.s32	q14, q14, q11		@ c1 b1 --row 3  q14: 37 27 35 25, 33 23 31 21

@		// blk [4,5]
@		b0 == x8 - x5@
@		c0 == x0 - x5a@
@		b1 == (b0 + 0x8000)>>19@
@		c1 == (c0 + 0x8000)>>19@
@		b0 == ((I16_WMV)b0)>>3@
@		c0 == ((I16_WMV)c0)>>3@
@		blk32[2] == (c0<<16) + b0@
@		blk32[2+4] == (c1<<16) + b1@

	vsub.s32	q1, q8, q7			@ b0
	vsub.s32	q10, q9, q13		@ c0	
	vadd.s32	q13, q1, q15		@ b1	
	vadd.s32	q11, q10, q15		@ c1
	vshl.s32	q1, q1, #16
	vshl.s32	q10, q10, #16
	vshr.s32	q1, q1, #19			@ b0
	vshr.s32	q10, q10, #19		@ c0
	vshr.s32	q13, q13, #19		@ b1
	vshr.s32	q11, q11, #19		@ c1	
	vshl.s32	q10, q10, #16
	vshl.s32	q11, q11, #16
	vadd.s32	q1, q1, q10			@ c0 b0 --row 4  q1 : 56 46 54 44 52 42 50 40
	vadd.s32	q13, q13, q11		@ c1 b1 --row 5  q13: 57 47 55 45 53 43 51 41

@              pixels:  
@              p7 p6 p5 p4 p3 p2 p1 p0
@              |  |  |  |  |  |  |  | 
@row 0 - q0  - 16 06 14 04 02 02 10 00     70 60 50 40 30 20 10 00 
@row 1 - q4  - 17 07 15 05 13 03 11 01     71       ...         01 
@row 2 - q3  - 36 26 34 24 32 22 30 20     72       ...         02 
@row 3 - q14 - 37 27 35 25 33 23 31 21  ==> 73       ...         03 
@row 4 - q1  - 56 46 54 44 52 42 50 40     74       ...         04 
@row 5 - q13 - 57 47 55 45 53 43 51 41     75       ...         05 
@row 6 - q2  - 76 66 74 64 72 62 70 60     76       ...         06 
@row 7 - q5  - 77 67 75 65 73 63 71 61     77 67 57 47 37 27 17 07 

	vtrn.32		q0, q3
	vtrn.32		q4, q14
	vtrn.32		q1, q2
	vtrn.32		q13, q5	
	vswp.s64	d1, d2
	vswp.s64	d9, d26
	vswp.s64	d7, d4
	vswp.s64	d29, d10
			

@r0 == piDst
@r1 == iOffsetToNextRowForDCT
@r2 == rgiCoefRecon
@q0,q1,q2,q3,q4,q5,q13,q14 == x0,x1,x2,x3,x4,x5,x6,x7
@q6,q7 == data of Pass2_table
@q11,q12 == x4a,x5a
@q8,q9,q10 == tmp
@q15 == IDCT_0x00008000_D/IDCT_0x80008000_D, IDCT_0xFFFF7FFF_D

	adr			r12, Pass2_table
    vmov.u32	IDCT_0xFFFF7FFF_D, #0xffff7fff	
    vmov.u32	IDCT_0x00008000_D, #0x8000	
	vld1.u64	{q6}, [r12]!
 	vld1.u64	{q7}, [r12]
    
@IDCT_Loop2   

@		x0 == piSrc0[i + 0*4 ] * 6 + 32 + (32<<16) /* rounding */@
@		x4 == piSrc0[i + 1*4 ]@
@		x3 == piSrc0[i + 2*4 ]@
@		x7 == piSrc0[i + 3*4 ]@
@		x1 == piSrc0[i + 4*4 ] * 6@
@		x6 == piSrc0[i + 5*4 ]@
@		x2 == piSrc0[i + 6*4 ]@
@		x5 == piSrc0[i + 7*4 ]@
	
	vadd.s32	q10, q4, q5			@  y4a
	vmul.s32	q0, q0, d12[1]
	vmul.s32	q1, q1, d12[1]
	
@		// zeroth stage
@		y4a == x4 + x5@
@		x8 == 7 * y4a@
@		x4a == x8 - 12 * x5@
@		x5a == x8 - 3 * x4@
@		x8 == 2 * y4a@
@		x4 == x8 + 6 * x4@
@		x5 == x8 - 10 * x5@

	vmul.s32	q8, q10, d13[1]		@7 * y4a
	vmul.s32	q11, q5, d15[0]		@-12 * x5
	vmul.s32	q12, q4, d12[0]		@ -3 * x4
	vmul.s32	q4, q4, d12[1]		@6 * x4
	vmul.s32	q5, q5, d14[1]		@-10 * x5
    vmov.u16	q9, #0x0020			@ rounding
	vadd.s32	q0, q0, q9
	vshl.s32	q9, q10, #1
	vadd.s32	q11, q11, q8			@ x4a
	vadd.s32	q12, q12, q8			@ x5a
	vadd.s32	q4, q4, q9				@ x4
	vadd.s32	q5, q5, q9				@ x5

@		ls_signbit==y4a&0x8000@
@		y4a == (y4a >> 1) - ls_signbit@
@		y4a == y4a & ~0x8000@
@		y4a == y4a | ls_signbit@
@		x4a +== y4a@
@		x5a +== y4a@

	@	vand		q9, q10, IDCT_0x00008000_D
	vand		d18, d20, IDCT_0x00008000_D
	vand		d19, d21, IDCT_0x00008000_D
	vshr.s32	q10, #1
	vsub.s32	q10, q10, q9
	@	vand		q10, q10, IDCT_0xFFFF7FFF_D
	vand		d20, d20, IDCT_0xFFFF7FFF_D
	vand		d21, d21, IDCT_0xFFFF7FFF_D
	vorr		q10, q10, q9
	vadd.s32	q11, q11, q10		@ x4a
	vadd.s32	q12, q12, q10		@ x5a

@		// first stage
@		y4 == x6 + x7@
@		x8 == 2 * y4@
@		x4a -== x8 + 6 * x6@
@		x5a +== x8 - 10 * x7@
@		x8 == 7 * y4@

	vadd.s32	q10, q13, q14				@  y4
	vshl.s32	q8, q10, #1
	vsub.s32	q11, q11, q8
	vadd.s32	q12, q12, q8
	vmla.s32	q11, q13, d15[1]		@  x4a == x4a -x8 + -6 * x6
	vmla.s32	q12, q14, d14[1]		@  x5a == x5a + x8 + -10 * x7
	vmul.s32	q8, q10, d13[1]		@  x8 == 7 * y4

@		ls_signbit==y4&0x8000@
@		y4 == (y4 >> 1) - ls_signbit@
@		y4 == y4 & ~0x8000@
@		y4 == y4 | ls_signbit@
@		x8 +== y4@
@		x4 +== x8 - 3 * x6@
@		x5 +== x8 - 12 * x7@

	@	vand		q9, q10, IDCT_0x00008000_D
	vand		d18, d20, IDCT_0x00008000_D
	vand		d19, d21, IDCT_0x00008000_D
	vshr.s32	q10, #1
	vsub.s32	q10, q10, q9
	@	vand		q10, q10, IDCT_0xFFFF7FFF_D
	vand		d20, d20, IDCT_0xFFFF7FFF_D
	vand		d21, d21, IDCT_0xFFFF7FFF_D
	vorr		q10, q10, q9
	vadd.s32	q8, q8, q10
	vadd.s32	q4, q4, q8
	vadd.s32	q5, q5, q8
	vmla.s32	q4, q13, d12[0]		@  x4 +== x8 + -3 * x6
	vmla.s32	q5, q14, d15[0]		@  x5 +== x8 + -12 * x7

@		// second stage 
@		x8 == x0 + x1@
@		x0 -== x1@
@		x1 == 8 * (x2 + x3)@
@		x6 == x1 - 5 * x2@
@		x1 -== 11 * x3@

	vmul.s32	q9, q2, d13[0]		@-5 * x2
	vadd.s32	q8, q0, q1
	vsub.s32	q0, q0, q1
	vadd.s32	q1, q2, q3
	vshl.s32	q1, #3
	vadd.s32	q13, q1, q9
	vmla.s32	q1, q3, d14[0]		@x1 +== -11 * x3
	
@		// third stage
@		x7 == x8 + x6@
@		x8 -== x6@
@		x6 == x0 - x1@
@		x0 +== x1@

	vadd.s32	q14, q8, q13
	vsub.s32	q8, q8, q13
	vsub.s32	q13, q0, q1
	vadd.s32	q0, q0, q1

@        // blk0
@        b0 == (x7 + x4)@
@        b1 == (b0 + 0x8000)>>22@
@        b0 == ((I16_WMV)b0)>>6@
@        blk0[ j ] == SATURATE8(b0)@
@        blk0[ j+1] == SATURATE8(b1)@
@        // blk1
@        b0 == (x6 + x4a)@
@        b1 == (b0 + 0x8000)>>22@
@        b0 == ((I16_WMV)b0)>>6@
@        blk1[ j ] == SATURATE8(b0)@
@        blk1[ j+1] == SATURATE8(b1)@

    vmov.u16	IDCT_0x80008000_D, #0x8000	
	vadd.s32	q9 , q14, q4	
	vadd.s32	q10, q13, q11	
	vaddw.u16	q6, q9 , IDCT_0x80008000_D	
	vaddw.u16	q7, q10, IDCT_0x80008000_D	
	vshr.s32	q6, #22
	vshr.s32	q7, #22
	vshr.s16	q9 , #6
	vshr.s16	q10, #6
	vtrn.16		q9, q6
	vtrn.16		q10, q7
	vqmovun.s16	d12, q9
	vqmovun.s16	d13, q10
	vst1.32		d12, [r0], r1
	vst1.32		d13, [r0], r1
        
@        // blk2
@        b0 == (x0 + x5a)@
@        b1 == (b0 + 0x8000)>>22@
@        b0 == ((I16_WMV)b0)>>6@
@        blk2[ j ] == SATURATE8(b0)@
@        blk2[ j+1] == SATURATE8(b1)@
@        // blk3
@        b0 == (x8 + x5)@
@        b1 == (b0 + 0x8000)>>22@
@        b0 == ((I16_WMV)b0)>>6@
@        blk3[ j ] == SATURATE8(b0)@
@        blk3[ j+1] == SATURATE8(b1)@
        
	vadd.s32	q9 , q0, q12	
	vadd.s32	q10, q8, q5	
	vaddw.u16	q6, q9 , IDCT_0x80008000_D	
	vaddw.u16	q7, q10, IDCT_0x80008000_D	
	vshr.s32	q6, #22
	vshr.s32	q7, #22
	vshr.s16	q9 , #6
	vshr.s16	q10, #6
	vtrn.16		q9, q6
	vtrn.16		q10, q7
	vqmovun.s16	d12, q9
	vqmovun.s16	d13, q10
	vst1.32		d12, [r0], r1
	vst1.32		d13, [r0], r1
        
@        // blk4
@        b0 == (x8 - x5)@
@        b1 == (b0 + 0x8000)>>22@
@        b0 == ((I16_WMV)b0)>>6@
@        blk4[ j ] == SATURATE8(b0)@
@        blk4[ j+1] == SATURATE8(b1)@
@        // blk5
@        b0 == (x0 - x5a)@
@        b1 == (b0 + 0x8000)>>22@
@        b0 == ((I16_WMV)b0)>>6@
@        blk5[ j ] == SATURATE8(b0)@
@        blk5[ j+1] == SATURATE8(b1)@

	vsub.s32	q9 , q8, q5	
	vsub.s32	q10, q0, q12	
	vaddw.u16	q6, q9 , IDCT_0x80008000_D	
	vaddw.u16	q7, q10, IDCT_0x80008000_D	
	vshr.s32	q6, #22
	vshr.s32	q7, #22
	vshr.s16	q9 , #6
	vshr.s16	q10, #6
	vtrn.16		q9, q6
	vtrn.16		q10, q7
	vqmovun.s16	d12, q9
	vqmovun.s16	d13, q10
	vst1.32		d12, [r0], r1
	vst1.32		d13, [r0], r1
       
@        // blk6
@        b0 == (x6 - x4a)@
@        b1 == (b0 + 0x8000)>>22@
@        b0 == ((I16_WMV)b0)>>6@
@        blk6[ j ] == SATURATE8(b0)@
@        blk6[ j+1] == SATURATE8(b1)@
@        // blk7
@        b0 == (x7 - x4)@
@        b1 == (b0 + 0x8000)>>22@
@        b0 == ((I16_WMV)b0)>>6@
@        blk7[ j ] == SATURATE8(b0)@
@        blk7[ j+1] == SATURATE8(b1)@

	vsub.s32	q9 , q13, q11	
	vsub.s32	q10, q14, q4	
	vaddw.u16	q6, q9 , IDCT_0x80008000_D	
	vaddw.u16	q7, q10, IDCT_0x80008000_D	
	vshr.s32	q6, #22
	vshr.s32	q7, #22
	vshr.s16	q9 , #6
	vshr.s16	q10, #6
	vtrn.16		q9, q6
	vtrn.16		q10, q7
	vqmovun.s16	d12, q9
	vqmovun.s16	d13, q10
	vst1.32		d12, [r0], r1
	vst1.32		d13, [r0]
	
    ldmia		sp!, {r4, pc}
	
    WMV_ENTRY_END
    @ENDP  @ |ARMV7_g_IDCTDec_WMV3|
    
	.align 8	
Pass1_table:
		.word 15,12,24,20,6,4
	.align 8	
Pass2_table:
		.word -3,6,-5,7,-11,-10,-12,-6			
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA |.text|, CODE, READONLY
    WMV_LEAF_ENTRY ARMV7_g_8x8IDCT
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

IDCT_0xFFFF7FFF_D	.req	d30		@for 8x8IDCT_Loop2
IDCT_0x00008000_D   .req	d31		@for 8x8IDCT_Loop2
IDCT_0x80008000_D	.req	d31		@for 8x8IDCT_Loop2

    stmdb     sp!, {r4, lr}
    FRAME_PROFILE_COUNT
		
@r0 == pSrc
@r1 == piDst
@r2 == tmp
@r3 == iDCTHorzFlags.
@r4 == SrcStride
@q0,q1,q2,q3,q4,q5,q14,q15 == x0,x1,x2,x3,x4,x5,x6,x7
@q6,q7 == data of Pass1_table
@q12,q13 == x4a,x5a
@q8,q9,q10,q11 == tmp

	pld			[r0]
	adr			r12, Pass1_table
    mov			r4, #16			@4*4
    mov			r1, r0
	vld1.u64	{q6}, [r12]!
 	vld1.u64	d14, [r12]
   
@8x8IDCT_Loop1   

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

    tst			r3, #0xFF
    bne			IDCT8x8_FullTransform
    
	vld1.u64	{q10}, [r0]
	vmov.u16	q9, #0x0004		@ rounding
	vmul.s32	q10, q10, d12[1]
    vmov.u32	q15, #0x8000	
	vadd.s32	q10, q10, q9
	vadd.s32	q11, q10, q15		
	vshl.s32	q10, q10, #16
	vshr.s32	q11, q11, #19		
	vshr.s32	q10, q10, #19				
	vshl.s32	q0, q10, #16
	vadd.s32	q10, q10, q0		
	vshl.s32	q1, q11, #16
	vadd.s32	q11, q11, q1		
	vdup.32		q0 , d20[0]
	vdup.32		q4 , d22[0]
	vdup.32		q3 , d20[1]
	vdup.32		q14, d22[1]
	vdup.32		q1 , d21[0]
	vdup.32		q13, d23[0]
	vdup.32		q2 , d21[1]
	vdup.32		q5 , d23[1]
	
	b			IDCT8x8_Loop2_start

	
IDCT8x8_FullTransform:

@		x4 == piSrc0[ i +1*4 ]@      
@		x3 == piSrc0[ i +2*4 ]@
@		x7 == piSrc0[ i +3*4 ]@
@		x1 == piSrc0[ i +4*4 ]@
@		x6 == piSrc0[ i +5*4 ]@
@		x2 == piSrc0[ i +6*4 ]@      
@		x5 == piSrc0[ i +7*4 ]@
@		x0 == piSrc0[ i +0*4 ]@ /* for proper rounding */

	vld1.u64	{q0} , [r0], r4
	vld1.u64	{q4} , [r0], r4
	vld1.u64	{q3} , [r0], r4
	vld1.u64	{q15}, [r0], r4
	vld1.u64	{q1} , [r0], r4
	vld1.u64	{q14}, [r0], r4
	vld1.u64	{q2} , [r0], r4
	vld1.u64	{q5} , [r0]

@		x1 == x1 * W0@   //12
@		x0 == x0 * W0 + (4+(4<<16))@ /* for proper rounding */
	
	vmul.s32	q0, q0, d12[1]
	vmul.s32	q1, q1, d12[1]

@		// zeroth stage
@		y3 == x4 + x5@
@		x8 == W3 * y3@           //15
@		x4a == x8 - W3pW5 * x5@  //24
@		x5a == x8 - W3_W5 * x4@  //6
@		x8 == W7 * y3@           //4
@		x4 == x8 + W1_W7 * x4@   //12
@		x5 == x8 - W1pW7 * x5@   //20

	vadd.s32	q9 , q4, q5			@q9 == y3
	vmul.s32	q8 , q9, d12[0]		@W3 * y3
	vmul.s32	q12, q5, d13[0]		@W3pW5 * x5
	vmul.s32	q13, q4, d14[0]		@W3_W5 * x4
	vmul.s32	q10, q4, d12[1]		@W1_W7 * x4
	vmul.s32	q11, q5, d13[1]		@W1pW7 * x5
	vshl.s32	q9 , #2				@W7 * y3
	vsub.s32	q12, q8 , q12		@ x4a
	vsub.s32	q13, q8 , q13		@ x5a
	vadd.s32	q4 , q9 , q10		@ x4
	vsub.s32	q5 , q9, q11		@ x5

@		// first stage
@		y3 == x6 + x7@
@		x8 == W7 * y3@           //4
@		x4a -== x8 + W1_W7 * x6@ //12
@		x5a +== x8 - W1pW7 * x7@ //20
@		x8 == W3 * y3@           //15
@		x4 +== x8 - W3_W5 * x6@  //6
@		x5 +== x8 - W3pW5 * x7@  //24

	vadd.s32	q9 , q14, q15		@
	vshl.s32	q8 , q9, #2
	vmul.s32	q10, q14, d12[1]		@W1_W7 * x6
	vmul.s32	q11, q15, d13[1]		@W1pW7 * x7
	vmul.s32	q9 , q9, d12[0]		@W3 * y3	
	vsub.s32	q12, q12, q8			
	vadd.s32	q13, q13, q8			
	vsub.s32	q12, q12, q10		@ x4a
	vsub.s32	q13, q13, q11		@ x5a
	vmul.s32	q10, q14, d14[0]	@W3_W5 * x6
	vmul.s32	q11, q15, d13[0]	@W3pW5 * x7	
	vadd.s32	q4 , q4 , q9			
	vadd.s32	q5 , q5 , q9			
	vsub.s32	q4 , q4 , q10		@ x4			
	vsub.s32	q5 , q5 , q11		@ x5			

@		// second stage 
@		x8 == x0 + x1@
@		x0 -== x1@
@		x1 == W6 * x3 - W2 * x2@  //6,  16
@		x3 == W6 * x2 + W2A * x3@ //6,  16

	vmul.s32	q10, q3, d14[0]			@W6 * x3
	vmov.u16	q9, #0x0004				@ rounding
	vadd.s32	q0 , q0, q9
	vmul.s32	q11, q2, d14[0]			@W6 * x2
	vadd.s32	q8, q0 , q1			
	vsub.s32	q0, q0 , q1
	vshl.s32	q1, q2, #4				@W2 * x2
	vshl.s32	q3, q3, #4				@W2A * x3
	vsub.s32	q1, q10, q1				@ x1
	vadd.s32	q3, q11, q3				@ x3

@		// third stage
@		x7 == x8 + x3@
@		x8 -== x3@
@		x3 == x0 + x1@
@		x0 -== x1@

	vadd.s32	q15, q8, q3			
	vsub.s32	q8, q8, q3
	vadd.s32	q3, q0, q1				
	vsub.s32	q0, q0, q1

@	free registers below: q1,q2,q6,q7,q9,q10,q11,q14
@   The out data is stored as follow: 
@   q0: row 0
@   q4: row 1
@   q3: row 2
@   q14:row 3
@   q1: row 4
@   q13:row 5
@   q2: row 6
@   q5: row 7

@		// blk [0,1]
@		b0 == x7 + x4@	// sw: b0 == 12*x0 + 16*x4 + 16*x3 + 15*x7 + 12*x1 + 9*x6 + 6*x2 + 4*x5 + rounding
@		c0 == x3 + x4a@	// sw: c0 == 12*x0 + 15*x4 + 6*x3 + -4*x7 + -12*x1 + -16*x6 + -16*x2 + -9*x5 + rounding
@		b1 == (b0 + 0x8000)>>19@
@		c1 == (c0 + 0x8000)>>19@
@		b0 == ((I16_WMV)b0)>>3@
@		c0 == ((I16_WMV)c0)>>3@
@		blk32[0] == (c0<<16) + b0@
@		blk32[0+4] == (c1<<16) + b1@

    vmov.u32	q14, #0x8000	
    vshl.s64	q9, q0, #0		@ backup x0
    vshl.s64	q6, q4, #0		@ backup x4
    vshl.s64	q7, q5, #0		@ backup x5
    
	vadd.s32	q0, q15, q4			@ b0
	vadd.s32	q10, q3, q12		@ c0	
	vadd.s32	q4, q0, q14			@ b1	
	vadd.s32	q11, q10, q14		@ c1
	vshl.s32	q0, q0, #16
	vshl.s32	q10, q10, #16
	vshr.s32	q0, q0, #19			@ b0
	vshr.s32	q10, q10, #19		@ c0
	vshr.s32	q4, q4, #19			@ b1
	vshr.s32	q11, q11, #19		@ c1	
	vshl.s32	q10, q10, #16
	vshl.s32	q11, q11, #16
	vadd.s32	q0, q0, q10			@ c0 b0 --row 0  q0: 16 06 14 04, 02 02 10 00
	vadd.s32	q4, q4, q11			@ c1 b1 --row 1  q4: 17 07 15 05, 13 03 11 01

@		// blk [6,7]
@		b0 == x3 - x4a@
@		c0 == x7 - x4@
@		b1 == (b0 + 0x8000)>>19@
@		c1 == (c0 + 0x8000)>>19@
@		b0 == ((I16_WMV)b0)>>3@
@		c0 == ((I16_WMV)c0)>>3@
@		blk32[3] == (c0<<16) + b0@
@		blk32[3+4] == (c1<<16) + b1@

	vsub.s32	q2, q3, q12			@ b0
	vsub.s32	q10, q15, q6		@ c0	
	vadd.s32	q5, q2, q14			@ b1	
	vadd.s32	q11, q10, q14		@ c1
	vshl.s32	q2, q2, #16
	vshl.s32	q10, q10, #16
	vshr.s32	q2, q2, #19			@ b0
	vshr.s32	q10, q10, #19		@ c0
	vshr.s32	q5, q5, #19			@ b1
	vshr.s32	q11, q11, #19		@ c1	
	vshl.s32	q10, q10, #16
	vshl.s32	q11, q11, #16
	vadd.s32	q2, q2, q10			@ c0 b0 --row 6  q2: 76 66 74 64, 72 62 70 60
	vadd.s32	q5, q5, q11			@ c1 b1 --row 7  q5: 77 67 75 65, 73 63 71 61

@		// blk [2,3]
@		b0 == x0 + x5a@
@		c0 == x8 + x5@
@		b1 == (b0 + 0x8000)>>19@
@		c1 == (c0 + 0x8000)>>19@
@		b0 == ((I16_WMV)b0)>>3@
@		c0 == ((I16_WMV)c0)>>3@
@		blk32[1] == (c0<<16) + b0@
@		blk32[1+4] == (c1<<16) + b1@

    vmov.u32	q15, #0x8000	
	vadd.s32	q3, q9, q13			@ b0
	vadd.s32	q10, q8, q7			@ c0	
	vadd.s32	q14, q3, q15		@ b1	
	vadd.s32	q11, q10, q15		@ c1
	vshl.s32	q3, q3, #16
	vshl.s32	q10, q10, #16
	vshr.s32	q3, q3, #19			@ b0
	vshr.s32	q10, q10, #19		@ c0
	vshr.s32	q14, q14, #19		@ b1
	vshr.s32	q11, q11, #19		@ c1	
	vshl.s32	q10, q10, #16
	vshl.s32	q11, q11, #16
	vadd.s32	q3, q3, q10			@ c0 b0 --row 2  q3: 36 26 34 24, 32 22 30 20
	vadd.s32	q14, q14, q11		@ c1 b1 --row 3  q14: 37 27 35 25, 33 23 31 21

@		// blk [4,5]
@		b0 == x8 - x5@
@		c0 == x0 - x5a@
@		b1 == (b0 + 0x8000)>>19@
@		c1 == (c0 + 0x8000)>>19@
@		b0 == ((I16_WMV)b0)>>3@
@		c0 == ((I16_WMV)c0)>>3@
@		blk32[2] == (c0<<16) + b0@
@		blk32[2+4] == (c1<<16) + b1@

	vsub.s32	q1, q8, q7			@ b0
	vsub.s32	q10, q9, q13		@ c0	
	vadd.s32	q13, q1, q15		@ b1	
	vadd.s32	q11, q10, q15		@ c1
	vshl.s32	q1, q1, #16
	vshl.s32	q10, q10, #16
	vshr.s32	q1, q1, #19			@ b0
	vshr.s32	q10, q10, #19		@ c0
	vshr.s32	q13, q13, #19		@ b1
	vshr.s32	q11, q11, #19		@ c1	
	vshl.s32	q10, q10, #16
	vshl.s32	q11, q11, #16
	vadd.s32	q1, q1, q10			@ c0 b0 --row 4  q1 : 56 46 54 44 52 42 50 40
	vadd.s32	q13, q13, q11		@ c1 b1 --row 5  q13: 57 47 55 45 53 43 51 41

@              pixels:  
@              p7 p6 p5 p4 p3 p2 p1 p0
@              |  |  |  |  |  |  |  | 
@row 0 - q0  - 16 06 14 04 02 02 10 00     70 60 50 40 30 20 10 00 
@row 1 - q4  - 17 07 15 05 13 03 11 01     71       ...         01 
@row 2 - q3  - 36 26 34 24 32 22 30 20     72       ...         02 
@row 3 - q14 - 37 27 35 25 33 23 31 21  ==> 73       ...         03 
@row 4 - q1  - 56 46 54 44 52 42 50 40     74       ...         04 
@row 5 - q13 - 57 47 55 45 53 43 51 41     75       ...         05 
@row 6 - q2  - 76 66 74 64 72 62 70 60     76       ...         06 
@row 7 - q5  - 77 67 75 65 73 63 71 61     77 67 57 47 37 27 17 07 

	vtrn.32		q0, q3
	vtrn.32		q4, q14
	vtrn.32		q1, q2
	vtrn.32		q13, q5	
	vswp.s64	d1, d2
	vswp.s64	d9, d26
	vswp.s64	d7, d4
	vswp.s64	d29, d10
			

IDCT8x8_Loop2_start:

@r0 == piDst + 32*2
@r1 == piDst
@r2 == dst stride
@q0,q1,q2,q3,q4,q5,q13,q14 == x0,x1,x2,x3,x4,x5,x6,x7
@q6,q7 == data of Pass2_table
@q11,q12 == x4a,x5a
@q8,q9,q10 == tmp
@q15 == IDCT_0x00008000_D/IDCT_0x80008000_D, IDCT_0xFFFF7FFF_D

	adr			r12, Pass2_table
    mov			r2, #16
    add			r0, r1, #64
	vld1.u64	{q6}, [r12]!
 	vld1.u64	{q7}, [r12]
    vmov.u32	IDCT_0xFFFF7FFF_D, #0xffff7fff	
    vmov.u32	IDCT_0x00008000_D, #0x8000	
    
@8x8IDCT_Loop2   

@		x0 == piSrc0[i + 0*4 ] * 6 + 32 + (32<<16) /* rounding */@
@		x4 == piSrc0[i + 1*4 ]@
@		x3 == piSrc0[i + 2*4 ]@
@		x7 == piSrc0[i + 3*4 ]@
@		x1 == piSrc0[i + 4*4 ] * 6@
@		x6 == piSrc0[i + 5*4 ]@
@		x2 == piSrc0[i + 6*4 ]@
@		x5 == piSrc0[i + 7*4 ]@
	
	vadd.s32	q10, q4, q5			@  y4a
	vmul.s32	q0, q0, d12[1]
	vmul.s32	q1, q1, d12[1]
	
@		// zeroth stage
@		y4a == x4 + x5@
@		x8 == 7 * y4a@
@		x4a == x8 - 12 * x5@
@		x5a == x8 - 3 * x4@
@		x8 == 2 * y4a@
@		x4 == x8 + 6 * x4@
@		x5 == x8 - 10 * x5@

	vmul.s32	q8, q10, d13[1]		@7 * y4a
	vmul.s32	q11, q5, d15[0]		@-12 * x5
	vmul.s32	q12, q4, d12[0]		@ -3 * x4
	vmul.s32	q4, q4, d12[1]		@6 * x4
	vmul.s32	q5, q5, d14[1]		@-10 * x5
    vmov.u16	q9, #0x0020			@ rounding
	vadd.s32	q0, q0, q9
	vshl.s32	q9, q10, #1
	vadd.s32	q11, q11, q8			@ x4a
	vadd.s32	q12, q12, q8			@ x5a
	vadd.s32	q4, q4, q9				@ x4
	vadd.s32	q5, q5, q9				@ x5

@		ls_signbit==y4a&0x8000@
@		y4a == (y4a >> 1) - ls_signbit@
@		y4a == y4a & ~0x8000@
@		y4a == y4a | ls_signbit@
@		x4a +== y4a@
@		x5a +== y4a@

	@	vand		q9, q10, IDCT_0x00008000_D
	vand		d18, d20, IDCT_0x00008000_D
	vand		d19, d21, IDCT_0x00008000_D
	vshr.s32	q10, #1
	vsub.s32	q10, q10, q9
	@	vand		q10, q10, IDCT_0xFFFF7FFF_D
	vand		d20, d20, IDCT_0xFFFF7FFF_D
	vand		d21, d21, IDCT_0xFFFF7FFF_D
	vorr		q10, q10, q9
	vadd.s32	q11, q11, q10		@ x4a
	vadd.s32	q12, q12, q10		@ x5a

@		// first stage
@		y4 == x6 + x7@
@		x8 == 2 * y4@
@		x4a -== x8 + 6 * x6@
@		x5a +== x8 - 10 * x7@
@		x8 == 7 * y4@

	vadd.s32	q10, q13, q14				@  y4
	vshl.s32	q8, q10, #1
	vsub.s32	q11, q11, q8
	vadd.s32	q12, q12, q8
	vmla.s32	q11, q13, d15[1]		@  x4a == x4a -x8 + -6 * x6
	vmla.s32	q12, q14, d14[1]		@  x5a == x5a + x8 + -10 * x7
	vmul.s32	q8, q10, d13[1]		@  x8 == 7 * y4

@		ls_signbit==y4&0x8000@
@		y4 == (y4 >> 1) - ls_signbit@
@		y4 == y4 & ~0x8000@
@		y4 == y4 | ls_signbit@
@		x8 +== y4@
@		x4 +== x8 - 3 * x6@
@		x5 +== x8 - 12 * x7@

	@	vand		q9, q10, IDCT_0x00008000_D
	vand		d18, d20, IDCT_0x00008000_D
	vand		d19, d21, IDCT_0x00008000_D
	vshr.s32	q10, #1
	vsub.s32	q10, q10, q9
	@	vand		q10, q10, IDCT_0xFFFF7FFF_D
	vand		d20, d20, IDCT_0xFFFF7FFF_D
	vand		d21, d21, IDCT_0xFFFF7FFF_D
	vorr		q10, q10, q9
	vadd.s32	q8, q8, q10
	vadd.s32	q4, q4, q8
	vadd.s32	q5, q5, q8
	vmla.s32	q4, q13, d12[0]		@  x4 +== x8 + -3 * x6
	vmla.s32	q5, q14, d15[0]		@  x5 +== x8 + -12 * x7

@		// second stage 
@		x8 == x0 + x1@
@		x0 -== x1@
@		x1 == 8 * (x2 + x3)@
@		x6 == x1 - 5 * x2@
@		x1 -== 11 * x3@

	vmul.s32	q9, q2, d13[0]		@-5 * x2
	vadd.s32	q8, q0, q1
	vsub.s32	q0, q0, q1
	vadd.s32	q1, q2, q3
	vshl.s32	q1, #3
	vadd.s32	q13, q1, q9
	vmla.s32	q1, q3, d14[0]		@x1 +== -11 * x3
	
@		// third stage
@		x7 == x8 + x6@
@		x8 -== x6@
@		x6 == x0 - x1@
@		x0 +== x1@

	vadd.s32	q14, q8, q13
	vsub.s32	q8, q8, q13
	vsub.s32	q13, q0, q1
	vadd.s32	q0, q0, q1

@        // blk0
@        b0 == (x7 + x4)@
@        b1 == (b0 + 0x8000)>>22@
@        b0 == ((I16_WMV)b0)>>6@
@        blk0[ j ] == SATURATE8(b0)@
@        blk0[ j+1] == SATURATE8(b1)@
@        // blk1
@        b0 == (x6 + x4a)@
@        b1 == (b0 + 0x8000)>>22@
@        b0 == ((I16_WMV)b0)>>6@
@        blk1[ j ] == SATURATE8(b0)@
@        blk1[ j+1] == SATURATE8(b1)@

    vmov.u16	IDCT_0x80008000_D, #0x8000	
	vadd.s32	q9 , q14, q4	
	vadd.s32	q10, q13, q11	
	vaddw.u16	q6, q9 , IDCT_0x80008000_D	
	vaddw.u16	q7, q10, IDCT_0x80008000_D	
	vshr.s32	q6, #22
	vshr.s32	q7, #22
	vshr.s16	q9 , #6
	vshr.s16	q10, #6
	vmovn.i32	d18, q9				@b0
	vmovn.i32	d19, q10			@b0
	vmovn.i32	d12, q6				@b1
	vmovn.i32	d13, q7				@b1
	vst1.64		{q9}, [r1], r2
	vst1.64		{q6}, [r0], r2
        
@        // blk2
@        b0 == (x0 + x5a)@
@        b1 == (b0 + 0x8000)>>22@
@        b0 == ((I16_WMV)b0)>>6@
@        blk2[ j ] == SATURATE8(b0)@
@        blk2[ j+1] == SATURATE8(b1)@
@        // blk3
@        b0 == (x8 + x5)@
@        b1 == (b0 + 0x8000)>>22@
@        b0 == ((I16_WMV)b0)>>6@
@        blk3[ j ] == SATURATE8(b0)@
@        blk3[ j+1] == SATURATE8(b1)@
        
	vadd.s32	q9 , q0, q12	
	vadd.s32	q10, q8, q5	
	vaddw.u16	q6, q9 , IDCT_0x80008000_D	
	vaddw.u16	q7, q10, IDCT_0x80008000_D	
	vshr.s32	q6, #22
	vshr.s32	q7, #22
	vshr.s16	q9 , #6
	vshr.s16	q10, #6
	vmovn.i32	d18, q9				@b0
	vmovn.i32	d19, q10			@b0
	vmovn.i32	d12, q6				@b1
	vmovn.i32	d13, q7				@b1
	vst1.64		{q9}, [r1], r2
	vst1.64		{q6}, [r0], r2
        
@        // blk4
@        b0 == (x8 - x5)@
@        b1 == (b0 + 0x8000)>>22@
@        b0 == ((I16_WMV)b0)>>6@
@        blk4[ j ] == SATURATE8(b0)@
@        blk4[ j+1] == SATURATE8(b1)@
@        // blk5
@        b0 == (x0 - x5a)@
@        b1 == (b0 + 0x8000)>>22@
@        b0 == ((I16_WMV)b0)>>6@
@        blk5[ j ] == SATURATE8(b0)@
@        blk5[ j+1] == SATURATE8(b1)@

	vsub.s32	q9 , q8, q5	
	vsub.s32	q10, q0, q12	
	vaddw.u16	q6, q9 , IDCT_0x80008000_D	
	vaddw.u16	q7, q10, IDCT_0x80008000_D	
	vshr.s32	q6, #22
	vshr.s32	q7, #22
	vshr.s16	q9 , #6
	vshr.s16	q10, #6
	vmovn.i32	d18, q9				@b0
	vmovn.i32	d19, q10			@b0
	vmovn.i32	d12, q6				@b1
	vmovn.i32	d13, q7				@b1
	vst1.64		{q9}, [r1], r2
	vst1.64		{q6}, [r0], r2
       
@        // blk6
@        b0 == (x6 - x4a)@
@        b1 == (b0 + 0x8000)>>22@
@        b0 == ((I16_WMV)b0)>>6@
@        blk6[ j ] == SATURATE8(b0)@
@        blk6[ j+1] == SATURATE8(b1)@
@        // blk7
@        b0 == (x7 - x4)@
@        b1 == (b0 + 0x8000)>>22@
@        b0 == ((I16_WMV)b0)>>6@
@        blk7[ j ] == SATURATE8(b0)@
@        blk7[ j+1] == SATURATE8(b1)@

	vsub.s32	q9 , q13, q11	
	vsub.s32	q10, q14, q4	
	vaddw.u16	q6, q9 , IDCT_0x80008000_D	
	vaddw.u16	q7, q10, IDCT_0x80008000_D	
	vshr.s32	q6, #22
	vshr.s32	q7, #22
	vshr.s16	q9 , #6
	vshr.s16	q10, #6
	vmovn.i32	d18, q9				@b0
	vmovn.i32	d19, q10			@b0
	vmovn.i32	d12, q6				@b1
	vmovn.i32	d13, q7				@b1
	vst1.64		{q9}, [r1], r2
	vst1.64		{q6}, [r0], r2
	
    ldmia		sp!, {r4, pc}
	
    WMV_ENTRY_END	@ARMV7_g_8x8IDCT
	.align 8	
Pass1_table1:
		.word 15,12,24,20,6,4	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA |.text|, CODE, READONLY
    WMV_LEAF_ENTRY ARMV7_g_8x4IDCT
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

IDCT_0x00008000_Q	.req	q14		@for 8x4IDCT_Loop2
IDCT_0xFFFF7FFF_Q	.req	q15		@for 8x4IDCT_Loop2
IDCT_0x00008000_D   .req	d31		 
	
    str			lr,  [sp, #-4]!
   
@8x4IDCT_Loop1   

@r0 == piSrc0
@r1 == pDst
@r2 == SrcStride
@r3 == iDCTHorzFlags	
    
	pld			[r0]
    adr			r12, Pass1_table1
    mov			r2, #16					@4*4
    vmov.u32	IDCT_0x00008000_D, #0x8000	
	vld1.u64	{q5}, [r12]!
 	vld1.u64	{d9}, [r12]

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

    tst			r3, #0x0F
    bne			IDCT8x4_FullTransform
	vld1.64		d0, [r0]
    vmov.u16	d2, #0x0004			@ rounding
	vmul.s32	d0, d0, d10[1]
	vadd.s32	d0, d0, d2
	vadd.s32	d1, d0, IDCT_0x00008000_D		
	vshl.s32	d0, d0, #16
	vshr.s32	q0, q0, #19				
	vshl.s32	q1, q0, #16
	vadd.s32	q3, q0, q1	
	vdup.32		q4, d6[0]
	vdup.32		q6, d6[1]
	vdup.32		q5, d7[0]
	vdup.32		q7, d7[1]
	b			IDCT8x4_Loop2_Start
	
IDCT8x4_FullTransform:

@		x4 == piSrc0[ i +1*4 ]@      
@		x3 == piSrc0[ i +2*4 ]@
@		x7 == piSrc0[ i +3*4 ]@
@		x1 == piSrc0[ i +4*4 ]@
@		x6 == piSrc0[ i +5*4 ]@
@		x2 == piSrc0[ i +6*4 ]@      
@		x5 == piSrc0[ i +7*4 ]@
@		x0 == piSrc0[ i +0*4 ]@ /* for proper rounding */

@	d0 - 03 02 01 00 
@	d4 - 13 12 11 10 
@	d3 - 23 22 21 20 
@	d7 - 33 32 31 30 
@	d1 - 43 42 41 40 
@	d6 - 53 51 51 50 
@	d2 - 63 62 61 60 
@	d5 - 73 72 71 70 

	vld1.64		d0, [r0], r2
	vld1.64		d4, [r0], r2
	vld1.64		d3, [r0], r2
	vld1.64		d7, [r0], r2
	vld1.64		d1, [r0], r2
	vld1.64		d6, [r0], r2
	vld1.64		d2, [r0], r2
	vld1.64		d5, [r0]

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

	vadd.s32	d12, d4, d5			@d12 == y3
	vmul.s32	d8, d12, d10[0]		@W3 * y3
	vmul.s32	d13, d5, d11[0]		@W3pW5 * x5
	vmul.s32	d14, d4, d9[0]		@W3_W5 * x4
	vmul.s32	d15, d4, d10[1]		@W1_W7 * x4
	vmul.s32	d16, d5, d11[1]		@W1pW7 * x5
    vmov.u16	d4, #0x0004			@ rounding
	vadd.s32	d0, d0, d4
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

	vadd.s32	d12, d6, d7			@d12 == y3
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
@		blk32[0] == (c0<<16) + b0@
@		blk32[0+4] == (c1<<16) + b1@

	vadd.s32	d16, d7, d4			@ b0
	vadd.s32	d17, d3, d13		@ c0	
	vadd.s32	d18, d16, IDCT_0x00008000_D		
	vadd.s32	d19, d17, IDCT_0x00008000_D			
	vshl.s32	q8, q8, #16
	vshr.s32	q9, q9, #19			@ b1 c1
	vshr.s32	q8, q8, #19			@ b0 c0
	vswp.s64	d17, d18
	vshl.s32	q9, q9, #16
	vadd.s32	q8, q8, q9		@ d16: c0 b0 c0 b0, d17: c1 b1 c1 b1 
								@	   12 02 10 00,      13 03 11 01
																
@		// blk [2,3]
@		b0 == x0 + x5a@
@		c0 == x8 + x5@
@		b1 == (b0 + 0x8000)>>19@
@		c1 == (c0 + 0x8000)>>19@
@		b0 == ((I16_WMV)b0)>>3@
@		c0 == ((I16_WMV)c0)>>3@
@		blk32[1] == (c0<<16) + b0@
@		blk32[1+4] == (c1<<16) + b1@

	vadd.s32	d18, d0, d14		@ b0
	vadd.s32	d19, d8, d5			@ c0	
	vadd.s32	d20, d18, IDCT_0x00008000_D		
	vadd.s32	d21, d19, IDCT_0x00008000_D			
	vshl.s32	q9, q9, #16
	vshr.s32	q10, q10, #19		@ b1 c1
	vshr.s32	q9, q9, #19			@ b0 c0
	vswp.s64	d19, d20
	vshl.s32	q10, q10, #16
	vadd.s32	q9, q9, q10		@ d18: c0 b0 c0 b0, d19: c1 b1 c1 b1 
								@	   32 22 30 20		 33 23 31 21
@	d16 - 12 02 10 00      30 20 10 00
@	d17 - 13 03 11 01  ==>  31 21 11 01
@	d18 - 32 22 30 20	   32 22 12 02
@	d19 - 33 23 31 21	   33 23 13 03
	vtrn.32		d16, d18
	vtrn.32		d17, d19

@		// blk [4,5]
@		b0 == x8 - x5@
@		c0 == x0 - x5a@
@		b1 == (b0 + 0x8000)>>19@
@		c1 == (c0 + 0x8000)>>19@
@		b0 == ((I16_WMV)b0)>>3@
@		c0 == ((I16_WMV)c0)>>3@
@		blk32[2] == (c0<<16) + b0@
@		blk32[2+4] == (c1<<16) + b1@

	vsub.s32	d20, d8, d5			@ b0
	vsub.s32	d21, d0, d14		@ c0	
	vadd.s32	d22, d20, IDCT_0x00008000_D		
	vadd.s32	d23, d21, IDCT_0x00008000_D			
	vshl.s32	q10, q10, #16
	vshr.s32	q11, q11, #19			@ b1 c1
	vshr.s32	q10, q10, #19			@ b0 c0
	vswp.s64	d21, d22
	vshl.s32	q11, q11, #16
	vadd.s32	q10, q10, q11	@ d20: c0 b0 c0 b0, d21: c1 b1 c1 b1 
								@	   52 42 50 40,      53 43 51 41
@		// blk [6,7]
@		b0 == x3 - x4a@
@		c0 == x7 - x4@
@		b1 == (b0 + 0x8000)>>19@
@		c1 == (c0 + 0x8000)>>19@
@		b0 == ((I16_WMV)b0)>>3@
@		c0 == ((I16_WMV)c0)>>3@
@		blk32[3] == (c0<<16) + b0@
@		blk32[3+4] == (c1<<16) + b1@

	vsub.s32	d22, d3, d13		@ b0
	vsub.s32	d23, d7, d4			@ c0	
	vadd.s32	d24, d22, IDCT_0x00008000_D		
	vadd.s32	d25, d23, IDCT_0x00008000_D			
	vshl.s32	q11, q11, #16
	vshr.s32	q12, q12, #19		@ b1 c1
	vshr.s32	q11, q11, #19		@ b0 c0
	vswp.s64	d23, d24
	vshl.s32	q12, q12, #16
	vadd.s32	q11, q11, q12	@ d22: c0 b0 c0 b0, d23: c1 b1 c1 b1 
								@	   72 62 70 60,      73 63 71 61
@	d20 - 12 02 10 00      30 20 10 00
@	d21 - 13 03 11 01  ==>  31 21 11 01
@	d22 - 32 22 30 20	   32 22 12 02
@	d23 - 33 23 31 21	   33 23 13 03
	vtrn.32		d20, d22
	vtrn.32		d21, d23

@ 02 02 10 00      
@ 13 03 11 01      
@ 32 22 30 20     70 60 50 40 30 20 10 00 - q4
@ 33 23 31 21  ==> 71       ...         01 - q5 
@ 52 42 50 40     72       ...         02 - q6 
@ 53 43 51 41     73 63 53 43 33 23 13 03 - q7 
@ 72 62 70 60      
@ 73 63 71 61      
	vshr.s32	d8 , d16, #0
	vshr.s32	d10, d17, #0
	vshr.s32	d12, d18, #0
	vshr.s32	d14, d19, #0
	vshr.s32	d9 , d20, #0
	vshr.s32	d11, d21, #0
	vshr.s32	d13, d22, #0
	vshr.s32	d15, d23, #0
	
	
IDCT8x4_Loop2_Start:	
	
@r0 == tmp
@r1 == pDst
@r2 == DstStride
@r3 == pDst2
	
@8x4IDCT_Loop2
    
	mov			r0, #-16
	mov			r12, #6
    mov			r2, #16					@4*4
    add			r3, r1, #64				@32*2
	vmov		d0, r0, r12
    vmov.u32	IDCT_0xFFFF7FFF_Q, #0xffff7fff	
    vmov.u32	IDCT_0x00008000_Q, #0x8000	

@        x4 == piSrc0[i + 0*4 ]@
@        x5 == piSrc0[i + 1*4 ]@
@        x6 == piSrc0[i + 2*4 ]@
@        x7 == piSrc0[i + 3*4 ]@
        
@		x3 == (x4 - x6)@ 
@		x6 +== x4@
@		x4 == 8 * x6 + 32 + (32<<16)@ //rounding
@		x8 == 8 * x3 + 32 + (32<<16)@   //rounding

	vmov.u16	q10, #0x0020	@ rounding
	vsub.s32	q3, q4, q6			
	vadd.s32	q6, q6, q4	
	vshl.s32	q4, q6, #3		
	vshl.s32	q8, q3, #3
	vadd.s32	q4, q4, q10	
	vadd.s32	q8, q8, q10	

@		ls_signbit==x6&0x8000@
@		x6 == (x6 >> 1) - ls_signbit@
@		x6 == x6 & ~0x8000@
@		x6 == x6 | ls_signbit@
@		ls_signbit==x3&0x8000@
@		x3 == (x3 >> 1) - ls_signbit@
@		x3 == x3 & ~0x8000@
@		x3 == x3 | ls_signbit@

	vand		q10, q6, IDCT_0x00008000_Q			
	vand		q11, q3, IDCT_0x00008000_Q			
	vshr.s32	q6, #1
	vshr.s32	q3, #1
	vsub.s32	q6, q6, q10			
	vsub.s32	q3, q3, q11			
	vand		q6, q6, IDCT_0xFFFF7FFF_Q			
	vand		q3, q3, IDCT_0xFFFF7FFF_Q			
	vorr		q6, q6, q10			
	vorr		q3, q3, q11			

@		x4 +== x6@ // guaranteed to have enough head room
@		x8 +== x3 @
@		x1 == 5 * ( x5 + x7)@
@		x5a == x1 + 6 * x5@
@		x5 ==  x1 - 16 * x7@

	vadd.s32	q10, q5, q7	
	vmul.s32	q9 , q5 , d0[1]		@ x5a
	vshl.s32	q5 , q10, #2
	vadd.s32	q5 , q10, q5	
	vadd.s32	q4 , q4 , q6	
	vadd.s32	q9 , q5 , q9	
	vmla.s32	q5 , q7, d0[0]
	vadd.s32	q8 , q8 , q3	
		        
@		// blk0
@		// sw: b0 == 17*x4 + 22*x5 + 17*x6 + 10*x7
@		b0 == (x4 + x5a)@
@		b1 == (b0 + 0x8000)>>22@
@		b0 == ((I16_WMV)b0)>>6@
@		blk16[ i + 0  + 0*4] == b0@
@		blk16[ i + 32 + 0*4] == b1@
@		// blk1
@		// sw: b0 == 17*x4 + 10*x5 + -17*x6 + -22*x7
@		b0 == (x8 + x5)@
@		b1 == (b0 + 0x8000)>>22@
@		b0 == ((I16_WMV)b0)>>6@
@		blk16[ i + 0  + 1*4] == b0@
@		blk16[ i + 32 + 1*4] == b1@

	vadd.s32	q10, q4, q9	
	vadd.s32	q11, q8, q5	
	vadd.s32	q12, q10, IDCT_0x00008000_Q	
	vadd.s32	q13, q11, IDCT_0x00008000_Q	
	vshr.s32	q12, #22
	vshr.s32	q13, #22
	vmovn.i32	d20, q10				@b0
	vmovn.i32	d21, q11				@b0
	vmovn.i32	d22, q12				@b1
	vmovn.i32	d23, q13				@b1
	vshr.s16	q10, #6
	vst1.64		{q10}, [r1], r2
	vst1.64		{q11}, [r3], r2

@		// blk2
@		// sw: b0 == 17*x4 + -10*x5 + -17*x6 + 22*x7
@		b0 == (x8 - x5)@
@		b1 == (b0 + 0x8000)>>22@
@		b0 == ((I16_WMV)b0)>>6@
@		blk16[ i + 0  + 2*4] == b0@
@		blk16[ i + 32 + 2*4] == b1@
@		// blk3
@		// sw: b0 == 17*x4 + -22*x5 + 17*x6 + -10*x7
@		b0 == (x4 - x5a)@
@		b1 == (b0 + 0x8000)>>22@
@		b0 == ((I16_WMV)b0)>>6@
@		blk16[ i + 0  + 3*4] == b0@
@		blk16[ i + 32 + 3*4] == b1@

	vsub.s32	q10, q8, q5	
	vsub.s32	q11, q4, q9	
	vadd.s32	q12, q10, IDCT_0x00008000_Q	
	vadd.s32	q13, q11, IDCT_0x00008000_Q	
	vshr.s32	q12, #22
	vshr.s32	q13, #22
	vmovn.i32	d20, q10				@b0
	vmovn.i32	d21, q11				@b0
	vmovn.i32	d22, q12				@b1
	vmovn.i32	d23, q13				@b1
	vshr.s16	q10, #6
	vst1.64		{q10}, [r1], r2
	vst1.64		{q11}, [r3], r2
	
    ldr			pc,  [sp], #4
	
    WMV_ENTRY_END	@ARMV7_g_8x4IDCT

	.align 8	
Pass3_table:	
		.word 10,17,-32,12	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA |.text|, CODE, READONLY
    WMV_LEAF_ENTRY ARMV7_g_4x8IDCT
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

IDCT_0x00008000_Q	.req	q14		@for 4x8IDCT_Loop1
IDCT_0xFFFF7FFF_D	.req	d30		@for 4x8IDCT_Loop2
IDCT_0x00008000_D   .req	d31		@for 4x8IDCT_Loop2

    str			lr,  [sp, #-4]!
    
	pld			[r0]
	adr			r12, Pass3_table
    vmov.u32	IDCT_0x00008000_Q, #0x8000	
    vmov.u32	IDCT_0x00008000_D, #0x8000	
    vmov.u32	IDCT_0xFFFF7FFF_D, #0xffff7fff	
    mov			r2, #16					@4*4
    vld1.64		{q0}, [r12]
    
@4x8IDCT_Loop1

@r0 == piSrc0
@r1 == pDst
@r2 == SrcStride
@r3 == iDCTHorzFlags	
    
@		if(!(iDCTHorzFlags&3))
@		{
@			I32_WMV iCurr, iNext@
@			b0 == piSrc0[ i ]*17 + (4+(4<<16))@
@			b1 == (b0 + 0x8000)>>19@
@			b0 == ((I16_WMV)b0)>>3@
@			iCurr == (b0<<16) + b0@
@			iNext == (b1<<16) + b1@
@			blk32[0] == iCurr@
@			blk32[0+4] == iNext@
@			blk32[1] == iCurr@
@			blk32[1+4] == iNext@   
@			continue@
@		}

    tst			r3, #0xFF
    bne			IDCT4x8_FullTransform
	vld1.u64	{q2}, [r0]	
    vmov.u16	q8, #0x0004			@ rounding
	vmul.s32	q2, q2, d0[1]
	vadd.s32	q2, q2, q8
	vadd.s32	q3, q2, IDCT_0x00008000_Q		
	vshl.s32	q2, q2, #16			@b0
	vshr.s32	q3, q3, #19			@b1
	vshr.s32	q2, q2, #19
	vshl.s32	q5, q3, #16
	vshl.s32	q4, q2, #16
	vadd.s32	q8, q2, q4			@iCurr
	vadd.s32	q9, q3, q5			@iNext
	vdup.32		d0, d16[0]
	vdup.32		d4, d18[0]
	vdup.32		d3, d16[1]
	vdup.32		d7, d18[1]
	vdup.32		d1, d17[0]
	vdup.32		d6, d19[0]
	vdup.32		d2, d17[1]
	vdup.32		d5, d19[1]
	b			IDCT4x8_Loop2_Start
	
IDCT4x8_FullTransform:
    
@		x4 == piSrc[ i +0*8 ]@      
@		x5 == piSrc[ i +1*8 ]@      
@		x6 == piSrc[ i +2*8 ]@
@		x7 == piSrc[ i +3*8 ]@
@	q4 - 07 06 05 04, 03 02 01 00 
@	q5 - 17 16 15 14, 13 12 11 10 
@	q6 - 27 26 25 24, 23 22 21 20 
@	q7 - 37 36 35 34, 33 32 31 30 

	vld1.u64	{q4}, [r0], r2
	vld1.u64	{q5}, [r0], r2
	vld1.u64	{q6}, [r0], r2
	vld1.u64	{q7}, [r0]
	
@        x0 == 17 * (x4 + x6) + 4 + (4<<16)@ //rounding
@        x1 == 17 * (x4 - x6) + 4 + (4<<16)@ //rounding
@        x8 == 10 * (x5 + x7)@
@        x2 == x8 + 12 * x5@
@        x3 == x8 - 32 * x7@
	
	vadd.s32	q10, q4, q6			@ (x4 + x6)
	vsub.s32	q11, q4, q6			@ (x4 - x6)	
	vmul.s32	q10, q10, d0[1]		@ 17 * (x4 + x6)
	vmul.s32	q11, q11, d0[1]		@ 17 * (x4 - x6)	
	vadd.s32	q12, q5, q7			@ (x5 + x7)
	vmul.s32	q12, q12, d0[0]		@ x8 == 10 * (x5 + x7)
	vmul.s32	q13, q7 , d1[0]		@ -32 * x7	
    vmov.u16	q8, #0x0004			@ rounding
	vadd.s32	q10, q10, q8		@ 4 + (4<<16)
	vadd.s32	q11, q11, q8		@ 4 + (4<<16)	
	vadd.s32	q13, q12, q13		@ x8 - 32 * x7
	vmla.s32	q12, q5, d1[1]		@ x8 + 12 * x5

@        // blk [0,1]
@        b0 == x0 + x2@
@        c0 == x1 + x3@
@        b1 == (b0 + 0x8000)>>19@
@        b0 == ((I16_WMV)b0)>>3@        
@        c1 == (c0 + 0x8000)>>19@
@        c0 == ((I16_WMV)c0)>>3@
@        blk32[0]   == (c0<<16) + b0@
@        blk32[0+4] == (c1<<16) + b1@

	vadd.s32	q4, q11, q13				@c0 == x1 + x3
	vadd.s32	q0, q10, q12				@b0 == x0 + x2
	vadd.s32	q9, q4, IDCT_0x00008000_Q	@c1
	vadd.s32	q3, q0, IDCT_0x00008000_Q	@b1	
	vshl.s32	q0, q0, #16
	vshl.s32	q4, q4, #16
	vshr.s32	q0, q0, #19
	vshr.s16	q4, q4, #3
	vshr.s32	q3, q3, #19
	vshr.s32	q9, q9, #19
	vshl.s32	q9, q9, #16
	
	@ row 0 -- d0: 02 02 10 00
	@ row 1 -- d4: 13 03 11 01
	@ row 4 -- d1: 16 06 14 04
	@ row 5 -- d6: 17 07 15 05
	vadd.s32	q0, q4, q0			@ c0 b0 : 16 06 14 04, 02 02 10 00
	vadd.s32	q2, q9, q3			@ c1 b1 : 17 07 15 05, 13 03 11 01
@	vshr.s32	d0, d0, #0			
@	vshr.s32	d4, d4, #0
@	vshr.s32	d1, d1, #0			
	vshr.s32	d6, d5, #0
	
@        // blk [2,3]
@        b0 == x1 - x3@
@        c0 == x0 - x2@
@        b1 == (b0 + 0x8000)>>19@
@        b0 == ((I16_WMV)b0)>>3@
@        c1 == (c0 + 0x8000)>>19@
@        c0 == ((I16_WMV)c0)>>3@
@        blk32[1]   == (c0<<16) + b0@
@        blk32[1+4] == (c1<<16) + b1@

	vsub.s32	q6 , q11, q13				@b0
	vsub.s32	q7 , q10, q12				@c0
	vadd.s32	q10, q6, IDCT_0x00008000_Q	@b1
	vadd.s32	q11, q7, IDCT_0x00008000_Q	@c1	
	vshl.s32	q6, q6, #16
	vshl.s32	q7, q7, #16
	vshr.s32	q6, q6, #19
	vshr.s16	q7, q7, #3
	vshr.s32	q10, q10, #19
	vshr.s32	q11, q11, #19
	vshl.s32	q11, q11, #16
	
	@ row 2 -- d3: 02 02 10 00
	@ row 3 -- d7: 13 03 11 01
	@ row 6 -- d2: 36 26 34 24
	@ row 7 -- d5: 37 27 35 25
	vadd.s32	q6, q6, q7			@ c0 b0 : 36 26 34 24, 32 22 30 20
	vadd.s32	q7, q10, q11		@ c1 b1 : 37 27 35 25, 33 23 31 21
	vshr.s32	d3, d12, #0			
	vshr.s32	d7, d14, #0
	vshr.s32	d2, d13, #0			
	vshr.s32	d5, d15, #0

@ row 0 --	d0 - 02 02 10 00      30 20 10 00
@ row 1 --	d4 - 13 03 11 01      31 21 11 01
@ row 2 --	d3 - 32 22 30 20	  32 22 12 02
@ row 3 --	d7 - 33 23 31 21	  33 23 13 03
@ row 4 --	d1 - 16 06 14 04  ==>  34 24 14 04
@ row 5 --	d6 - 17 07 15 05      35 25 15 05
@ row 6 --	d2 - 36 26 34 24	  36 26 16 06
@ row 7 --	d5 - 37 27 35 25	  37 27 17 07
	vtrn.32		d0, d3
	vtrn.32		d4, d7
	vtrn.32		d1, d2
	vtrn.32		d6, d5
	    
	    
IDCT4x8_Loop2_Start:

@r0 == tmp
@r1 == pDst
@r2 == DstStride
@r3 == pDst2
    
    adr			r12, Pass2_table
    mov			r2, #8					@4*2
    add			r3, r1, #64				@32*2
	vld1.u64	{q5}, [r12]!
	vld1.u64	{q6}, [r12]
    
@4x8IDCT_Loop2   

@		x0 == piSrc0[i + 0*4 ] * 6 + 32 + (32<<16) /* rounding */@
@		x4 == piSrc0[i + 1*4 ]@
@		x3 == piSrc0[i + 2*4 ]@
@		x7 == piSrc0[i + 3*4 ]@
@		x1 == piSrc0[i + 4*4 ] * 6@
@		x6 == piSrc0[i + 5*4 ]@
@		x2 == piSrc0[i + 6*4 ]@
@		x5 == piSrc0[i + 7*4 ]@
	
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
    vmov.u16	d16, #0x0020		@ rounding
	vadd.s32	d0, d0, d16
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

	vand		d16, d15, IDCT_0x00008000_D
	vshr.s32	d15, #1
	vsub.s32	d15, d15, d16
	vand		d15, d15, IDCT_0xFFFF7FFF_D
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

	vand		d16, d15, IDCT_0x00008000_D
	vshr.s32	d15, #1
	vsub.s32	d15, d15, d16
	vand		d15, d15, IDCT_0xFFFF7FFF_D
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
	vadd.s32	d18, d16, IDCT_0x00008000_D	
	vadd.s32	d19, d17, IDCT_0x00008000_D	
	vshr.s32	d18, #22
	vshr.s32	d19, #22
	vmovn.i32	d16, q8				@b0
	vmovn.i32	d17, q9				@b1
	vshr.s16	d16, #6
	vst1.32		d16[0], [r1], r2
	vst1.32		d16[1], [r1], r2
	vst1.32		d17[0], [r3], r2
	vst1.32		d17[1], [r3], r2

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
	vadd.s32	d18, d16, IDCT_0x00008000_D	
	vadd.s32	d19, d17, IDCT_0x00008000_D	
	vshr.s32	d18, #22
	vshr.s32	d19, #22
	vmovn.i32	d16, q8				@b0
	vmovn.i32	d17, q9				@b1
	vshr.s16	d16, #6
	vst1.32		d16[0], [r1], r2
	vst1.32		d16[1], [r1], r2
	vst1.32		d17[0], [r3], r2
	vst1.32		d17[1], [r3], r2

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
	vadd.s32	d18, d16, IDCT_0x00008000_D	
	vadd.s32	d19, d17, IDCT_0x00008000_D	
	vshr.s32	d18, #22
	vshr.s32	d19, #22
	vmovn.i32	d16, q8				@b0
	vmovn.i32	d17, q9				@b1
	vshr.s16	d16, #6
	vst1.32		d16[0], [r1], r2
	vst1.32		d16[1], [r1], r2
	vst1.32		d17[0], [r3], r2
	vst1.32		d17[1], [r3], r2

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
	vadd.s32	d18, d16, IDCT_0x00008000_D	
	vadd.s32	d19, d17, IDCT_0x00008000_D	
	vshr.s32	d18, #22
	vshr.s32	d19, #22
	vmovn.i32	d16, q8				@b0
	vmovn.i32	d17, q9				@b1
	vshr.s16	d16, #6
	vst1.32		d16[0], [r1], r2
	vst1.32		d16[1], [r1]
	vst1.32		d17[0], [r3], r2
	vst1.32		d17[1], [r3]
	    
    ldr       pc,  [sp], #4

    WMV_ENTRY_END	@ARMV7_g_4x8IDCT
    
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA |.text|, CODE, READONLY
    WMV_LEAF_ENTRY ARMV7_g_4x4IDCT
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


IDCT_0xFFFF7FFF_D	.req	d30		@for 4x4IDCT_Loop2
IDCT_0x00008000_D   .req	d31		

    str			lr,  [sp, #-4]!
    
	pld			[r0]
	adr			r12, Pass3_table
    mov			r2, #16					@4*4
    vmov.u32	IDCT_0xFFFF7FFF_D, #0xffff7fff	
    vmov.u32	IDCT_0x00008000_D, #0x8000	
    vld1.64		{q0}, [r12]
   
@IDCT4x4_Loop1

@r0 == piSrc0
@r1 == pDst
@r2 == SrcStride == DstStride
@r3 == iDCTHorzFlags	
    
@		if(!(iDCTHorzFlags&3))
@		{
@			I32_WMV iCurr, iNext@
@			b0 == piSrc0[ i ]*17 + (4+(4<<16))@
@			b1 == (b0 + 0x8000)>>19@
@			b0 == ((I16_WMV)b0)>>3@
@			iCurr == (b0<<16) + b0@
@			iNext == (b1<<16) + b1@
@			blk32[0] == iCurr@
@			blk32[0+4] == iNext@
@			blk32[1] == iCurr@
@			blk32[1+4] == iNext@   
@			continue@
@		}

    tst			r3, #0x0F
    bne			IDCT4x4_FullTransform
    
	vld1.u64	d2, [r0]!	
    vmov.u16	d29, #0x0004	
	vmul.s32	d2, d2, d0[1]
	vadd.s32	d2, d2, d29
	vadd.s32	d3, d2, IDCT_0x00008000_D		
	vshl.s32	d2, d2, #16
	vshr.s32	q1, q1, #19		
		
	vshl.s32	q2, q1, #16
	vadd.s32	q6, q1, q2		
	vdup.32		d4, d12[0]
	vdup.32		d5, d13[0]
	vdup.32		d6, d12[1]
	vdup.32		d7, d13[1]
	b			IDCT4x4_Loop2_start
	
IDCT4x4_FullTransform:

	@ pixel data
@		x4 == piSrc[ i +0*8 ]@      
@		x5 == piSrc[ i +1*8 ]@      
@		x6 == piSrc[ i +2*8 ]@
@		x7 == piSrc[ i +3*8 ]@
@	d4 - 00 01 02 03 
@	d5 - 10 11 12 13 
@	d6 - 20 21 22 23
@	d7 - 30 31 32 33

	vld1.u64	d4, [r0], r2
	vld1.u64	d5, [r0], r2
	vld1.u64	d6, [r0], r2
	vld1.u64	d7, [r0]
	
@        x0 == 17 * (x4 + x6) + 4 + (4<<16)@ //rounding
@        x1 == 17 * (x4 - x6) + 4 + (4<<16)@ //rounding
@        x8 == 10 * (x5 + x7)@
@        x2 == x8 + 12 * x5@
@        x3 == x8 - 32 * x7@
	
	vadd.s32	d10, d4, d6			@ (x4 + x6)
	vsub.s32	d11, d4, d6			@ (x4 - x6)	
	vmul.s32	d10, d10, d0[1]		@ 17 * (x4 + x6)
	vmul.s32	d11, d11, d0[1]		@ 17 * (x4 - x6)	
	vadd.s32	d12, d5, d7			@ (x5 + x7)
	vmul.s32	d12, d12, d0[0]		@ x8 == 10 * (x5 + x7)
	vmul.s32	d13, d7 , d1[0]		@ -32 * x7	
    vmov.u16	d29, #0x0004	
	vadd.s32	d10, d10, d29		@ 4 + (4<<16)
	vadd.s32	d11, d11, d29		@ 4 + (4<<16)	
	vadd.s32	d13, d12, d13		@ x8 - 32 * x7
	vmla.s32	d12, d5, d1[1]		@ x8 + 12 * x5

@        // blk [0,1]
@        b0 == x0 + x2@
@        c0 == x1 + x3@
@        b1 == (b0 + 0x8000)>>19@
@        b0 == ((I16_WMV)b0)>>3@        
@        c1 == (c0 + 0x8000)>>19@
@        c0 == ((I16_WMV)c0)>>3@

	vadd.s32	d5, d11, d13				@c0 == x1 + x3
	vadd.s32	d4, d10, d12				@b0 == x0 + x2
	vadd.s32	d9, d5, IDCT_0x00008000_D	@c1
	vadd.s32	d8, d4, IDCT_0x00008000_D	@b1
	vshl.s32	q2, q2, #16
	vshr.s32	q2, q2, #19
	vshr.s32	q4, q4, #19
	
@        blk32[0]   == (c0<<16) + b0@
@        blk32[0+4] == (c1<<16) + b1@
	vswp.s64	d5, d8
	vshl.s32	q4, q4, #16
	vadd.s32	q2, q2, q4		@ d4: b0 c0 b0 c0, d5: b1 c1 b1 c1 
								@ d4: 00 10 02 12, d5: 01 11 03 13
@        // blk [2,3]
@        b0 == x1 - x3@
@        c0 == x0 - x2@
@        b1 == (b0 + 0x8000)>>19@
@        b0 == ((I16_WMV)b0)>>3@
@        c1 == (c0 + 0x8000)>>19@
@        c0 == ((I16_WMV)c0)>>3@

	vsub.s32	d6, d11, d13				@b0
	vsub.s32	d7, d10, d12				@c0
	vadd.s32	d10, d6, IDCT_0x00008000_D	@b1
	vadd.s32	d11, d7, IDCT_0x00008000_D	@c1
	vshl.s32	q3, q3, #16
	vshr.s32	q3, q3, #19
	vshr.s32	q5, q5, #19
	
@        blk32[1]   == (c0<<16) + b0@
@        blk32[1+4] == (c1<<16) + b1@

	vswp.s64	d7, d10
	vshl.s32	q5, q5, #16
	vadd.s32	q3, q3, q5		@ d6: b0 c0 b0 c0, d5: b1 c1 b1 c1 
								@ d6: 20 30 22 32, d7: 21 31 23 33
@	d4 - 00 10 02 12      00 10 20 30
@	d5 - 01 11 03 13  ==>  01 11 21 31
@	d6 - 20 30 22 32	  02 12 22 32
@	d7 - 21 31 23 33	  03 13 23 33
	vtrn.32		d4, d6
	vtrn.32		d5, d7
	    
	    
IDCT4x4_Loop2_start:

@r0 == tmp
@r1 == pDst
@r2 == DstStride
@r3 == pDst2
	    
	pld			[r0]
	mov			r12, #-16
	mov			r14, #6
    mov			r2, #8					@4*2
    add			r3, r1, #64				@32*2
	vmov		d0, r12, r14
    
@IDCT4x4_Loop2   

@        x4 == piSrc0[i + 0*4 ]@
@        x5 == piSrc0[i + 1*4 ]@
@        x6 == piSrc0[i + 2*4 ]@
@        x7 == piSrc0[i + 3*4 ]@
                
@		x3 == (x4 - x6)@ 
@		x6 +== x4@
@		x4 == 8 * x6 + 32 + (32<<16)@ //rounding
@		x8 == 8 * x3 + 32 + (32<<16)@   //rounding

    vmov.u16	d29, #0x0020	
	vsub.s32	d3, d4, d6			
	vadd.s32	d6, d6, d4	
	vshl.s32	d4, d6, #3		
	vshl.s32	d8, d3, #3
	vadd.s32	d4, d4, d29	
	vadd.s32	d8, d8, d29	

@		ls_signbit==x6&0x8000@
@		x6 == (x6 >> 1) - ls_signbit@
@		x6 == x6 & ~0x8000@
@		x6 == x6 | ls_signbit@
@		ls_signbit==x3&0x8000@
@		x3 == (x3 >> 1) - ls_signbit@
@		x3 == x3 & ~0x8000@
@		x3 == x3 | ls_signbit@

	vand		d10, d6, IDCT_0x00008000_D			
	vand		d11, d3, IDCT_0x00008000_D			
	vshr.s32	d6, #1
	vshr.s32	d3, #1
	vsub.s32	d6, d6, d10			
	vsub.s32	d3, d3, d11			
	vand		d6, d6, IDCT_0xFFFF7FFF_D			
	vand		d3, d3, IDCT_0xFFFF7FFF_D			
	vorr		d6, d6, d10			
	vorr		d3, d3, d11			

@		x4 +== x6@ // guaranteed to have enough head room
@		x8 +== x3 @
@		x1 == 5 * ( x5 + x7)@
@		x5a == x1 + 6 * x5@
@		x5 ==  x1 - 16 * x7@

	vadd.s32	d10, d5, d7	
	vmul.s32	d9 , d5 , d0[1]		@ x5a
	vshl.s32	d5 , d10, #2
	vadd.s32	d5 , d10, d5	
	vadd.s32	d4 , d4 , d6	
	vadd.s32	d9 , d5 , d9	
	vmla.s32	d5 , d7, d0[0]
	vadd.s32	d8 , d8 , d3	
		        
@		// blk0
@		// sw: b0 == 17*x4 + 22*x5 + 17*x6 + 10*x7
@		b0 == (x4 + x5a)@
@		b1 == (b0 + 0x8000)>>22@
@		b0 == ((I16_WMV)b0)>>6@
@		blk16[ i + 0  + 0*4] == b0@
@		blk16[ i + 32 + 0*4] == b1@
@		// blk1
@		// sw: b0 == 17*x4 + 10*x5 + -17*x6 + -22*x7
@		b0 == (x8 + x5)@
@		b1 == (b0 + 0x8000)>>22@
@		b0 == ((I16_WMV)b0)>>6@
@		blk16[ i + 0  + 1*4] == b0@
@		blk16[ i + 32 + 1*4] == b1@

	vadd.s32	d10, d4, d9	
	vadd.s32	d11, d8, d5	
	vadd.s32	d12, d10, IDCT_0x00008000_D	
	vadd.s32	d13, d11, IDCT_0x00008000_D	
	vshr.s32	d12, #22
	vshr.s32	d13, #22
	vmovn.i32	d10, q5				@b0
	vmovn.i32	d11, q6				@b1
	vshr.s16	d10, #6
	vst1.32		d10[0], [r1], r2
	vst1.32		d10[1], [r1], r2
	vst1.32		d11[0], [r3], r2
	vst1.32		d11[1], [r3], r2

@		// blk2
@		// sw: b0 == 17*x4 + -10*x5 + -17*x6 + 22*x7
@		b0 == (x8 - x5)@
@		b1 == (b0 + 0x8000)>>22@
@		b0 == ((I16_WMV)b0)>>6@
@		blk16[ i + 0  + 2*4] == b0@
@		blk16[ i + 32 + 2*4] == b1@
@		// blk3
@		// sw: b0 == 17*x4 + -22*x5 + 17*x6 + -10*x7
@		b0 == (x4 - x5a)@
@		b1 == (b0 + 0x8000)>>22@
@		b0 == ((I16_WMV)b0)>>6@
@		blk16[ i + 0  + 3*4] == b0@
@		blk16[ i + 32 + 3*4] == b1@

	vsub.s32	d10, d8, d5	
	vsub.s32	d11, d4, d9	
	vadd.s32	d12, d10, IDCT_0x00008000_D	
	vadd.s32	d13, d11, IDCT_0x00008000_D	
	vshr.s32	d12, #22
	vshr.s32	d13, #22
	vmovn.i32	d10, q5				@b0
	vmovn.i32	d11, q6				@b1
	vshr.s16	d10, #6
	vst1.32		d10[0], [r1], r2
	vst1.32		d10[1], [r1]
	vst1.32		d11[0], [r3], r2
	vst1.32		d11[1], [r3]
		    
    ldr       pc,  [sp], #4

    WMV_ENTRY_END	@ARMV7_g_4x4IDCT

    .ENDIF @ .IF WMV_OPT_IDCT_ARM == 1


    .END 
