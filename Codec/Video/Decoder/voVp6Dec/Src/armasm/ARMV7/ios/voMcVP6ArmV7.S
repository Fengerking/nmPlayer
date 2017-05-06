@************************************************************************
@									                                    *
@	VisualOn, Inc Confidential and Proprietary, 2009		            *
@	written by John							 	                                    *
@***********************************************************************/
	@AREA	 .text , CODE, READONLY
#include "../../../../Inc/voVP6DecID.h"
	.text
	.globl	 _FilterBlock1dBil_wRecon_Armv7
	.globl	 _FilterBlock2dBil_wRecon_Armv7
	.globl	 _FilterBlock1d_wRecon_Armv7
	.globl	 _FilterBlock2d_wRecon_Armv7			
	.align 4
	
_FilterBlock1dBil_wRecon_Armv7:  @PROC
@void FilterBlock1dBil_wRecon_C
@(
@	UINT8  *SrcPtr, 	r0
@	UINT8  *dstPtr,		r1
@	INT32	PixelStep,	r2
@	INT32	SrcStride,	r3
@	INT32	LineStep,	r4
@	FILTER_DATA  *Filter 	r5
@)
@{
@	UINT32 i, j@
@	// accomodate incrementing SrcPtr++ each time.
@	SrcStride -= 8@
@	for (i = 0@ i < 8@ i++)
@	{
@		for (j = 0@ j < 8@ j++)
@		{
@			INT16 Temp@
@			// Apply filter 
@			// NOTE: Rounding doesn't improve accuracy but is 
@			//       easier to implement on certain platforms.
@			Temp = (INT16)((((INT32)SrcPtr[0]	 * Filter[0])
@				 +	((INT32)SrcPtr[PixelStep]*Filter[1])
@				 +	(FILTER_WEIGHT/2) ) >> FILTER_SHIFT )@
@			dstPtr[j] = (UINT8)Temp@
@			SrcPtr++@
@		}
@		// Next row...
@		SrcPtr   += SrcStride@
@		//diffPtr += BLOCK_HEIGHT_WIDTH@
@		dstPtr += LineStep@
@	}
@}
@[sp, #36] = LineStep, [sp, #40] = Filter
	stmdb	sp!, {r4 - r11, lr}
	ldr	r4,[sp,#36]	@LineStep
	ldr	r5,[sp,#40]	@Filter
	cmp	r2, #1
	bne	FilterBlock1dBil_wRecon_Armv7_V
	
	ldrd	r6, [r5]
	vmov.u16 q15, #64			
	vdup.u8 d28, r6	
	vdup.u8 d29, r7	
	
	vld1.8 {q0}, [r0], r3		@d0 lTemp0= pSrc[0~7]
	vld1.8 {q1}, [r0], r3		@d2 lTemp0= pSrc[0~7]
	vld1.8 {q2}, [r0], r3		@d4 lTemp0= pSrc[0~7]
	vld1.8 {q3}, [r0], r3		@d6 lTemp0= pSrc[0~7]
	vld1.8 {q4}, [r0], r3		@d0 lTemp0= pSrc[0~7]
	vld1.8 {q5}, [r0], r3		@d2 lTemp0= pSrc[0~7]
	vld1.8 {q6}, [r0], r3		@d4 lTemp0= pSrc[0~7]
	vld1.8 {q7}, [r0]		@d6 lTemp0= pSrc[0~7]		
	
	vmull.u8 q8, d0, d28			@5*lTemp0
	vmull.u8 q9, d2, d28			@5*lTemp0
	vmull.u8 q10, d4, d28			@5*lTemp0
	vmull.u8 q11, d6, d28			@5*lTemp0
					
	vext.u8 d24, d0, d1, #1		@lTemp1= pSrc[1~8]
	vext.u8 d25, d2, d3, #1		@lTemp1= pSrc[1~8]
	vext.u8 d26, d4, d5, #1		@lTemp1= pSrc[1~8]
	vext.u8 d27, d6, d7, #1		@lTemp1= pSrc[1~8]
	
	vmlal.u8 q8, d24, d29		@3*lTemp1
	vmlal.u8 q9, d25, d29		@3*lTemp1
	vmlal.u8 q10, d26, d29		@3*lTemp1
	vmlal.u8 q11, d27, d29		@3*lTemp1
@Two		
	vmull.u8 q0, d8, d28			@5*lTemp0
	vmull.u8 q1, d10, d28			@5*lTemp0
	vmull.u8 q2, d12, d28			@5*lTemp0
	vmull.u8 q3, d14, d28			@5*lTemp0
					
	vext.u8 d8, d8, d9, #1		@lTemp1= pSrc[1~8]
	vext.u8 d9, d10, d11, #1		@lTemp1= pSrc[1~8]
	vext.u8 d10, d12, d13, #1		@lTemp1= pSrc[1~8]
	vext.u8 d11, d14, d15, #1		@lTemp1= pSrc[1~8]		
	
	vmlal.u8 q0, d8, d29		@3*lTemp1
	vmlal.u8 q1, d9, d29		@3*lTemp1
	vmlal.u8 q2, d10, d29		@3*lTemp1
	vmlal.u8 q3, d11, d29		@3*lTemp1
	
	vadd.u16 q8, q8, q15		@+8
	vadd.u16 q9, q9, q15		@+8
	vadd.u16 q10, q10, q15		@+8
	vadd.u16 q11, q11, q15		@+8	
	vadd.u16 q0, q0, q15		@+8
	vadd.u16 q1, q1, q15		@+8
	vadd.u16 q2, q2, q15		@+8
	vadd.u16 q3, q3, q15		@+8				

	vqshrn.u16 d16, q8, #7
	vqshrn.u16 d17, q9, #7
	vqshrn.u16 d18, q10, #7
	vqshrn.u16 d19, q11, #7
	vqshrn.u16 d20, q0, #7
	vqshrn.u16 d21, q1, #7
	vqshrn.u16 d22, q2, #7
	vqshrn.u16 d23, q3, #7

	vst1.64	{d16}, [r1], r4
	vst1.64	{d17}, [r1], r4 
	vst1.64	{d18}, [r1], r4 
	vst1.64	{d19}, [r1], r4
	vst1.64	{d20}, [r1], r4
	vst1.64	{d21}, [r1], r4 
	vst1.64	{d22}, [r1], r4 
	vst1.64	{d23}, [r1]	
	ldmia	sp!, {r4 - r11, pc} 
							
FilterBlock1dBil_wRecon_Armv7_V:

	ldrd	r6, [r5]
	vmov.u16 q15, #64			
	vdup.u8 d28, r6	
	vdup.u8 d29, r7	
	
	vld1.8 {d0}, [r0], r3		@d0 lTemp0= pSrc[0~7]
	vld1.8 {d1}, [r0], r3		@d2 lTemp0= pSrc[0~7]
	vld1.8 {d2}, [r0], r3		@d4 lTemp0= pSrc[0~7]	
	vld1.8 {d3}, [r0], r3		@d6 lTemp0= pSrc[0~7]		
	vld1.8 {d4}, [r0], r3		@d6 lTemp0= pSrc[0~7]
	vld1.8 {d5}, [r0], r3		@d0 lTemp0= pSrc[0~7]
	vld1.8 {d6}, [r0], r3		@d2 lTemp0= pSrc[0~7]
	vld1.8 {d7}, [r0], r3		@d4 lTemp0= pSrc[0~7]	
	vld1.8 {d8}, [r0]		@d6 lTemp0= pSrc[0~7]		

	vmull.u8 q8, d0, d28			@5*lTemp0
	vmlal.u8 q8, d1, d29			@3*lTemp1
		
	vmull.u8 q9, d1, d28			@5*lTemp0	
	vmlal.u8 q9, d2, d29			@3*lTemp1
	
	vmull.u8 q10, d2, d28			@5*lTemp0
	vmlal.u8 q10, d3, d29			@3*lTemp1
			
	vmull.u8 q11, d3, d28			@5*lTemp0	
	vmlal.u8 q11, d4, d29			@3*lTemp1
	
	vmull.u8 q0, d4, d28			@5*lTemp0
	vmlal.u8 q0, d5, d29			@3*lTemp1
			
	vmull.u8 q1, d5, d28			@5*lTemp0	
	vmlal.u8 q1, d6, d29			@3*lTemp1
	
	vmull.u8 q2, d6, d28			@5*lTemp0
	vmlal.u8 q2, d7, d29			@3*lTemp1
			
	vmull.u8 q3, d7, d28			@5*lTemp0	
	vmlal.u8 q3, d8, d29			@3*lTemp1	

	vadd.u16 q8, q8, q15		@+8
	vadd.u16 q9, q9, q15		@+8
	vadd.u16 q10, q10, q15		@+8
	vadd.u16 q11, q11, q15		@+8	
	vadd.u16 q0, q0, q15		@+8
	vadd.u16 q1, q1, q15		@+8
	vadd.u16 q2, q2, q15		@+8
	vadd.u16 q3, q3, q15		@+8				

	vqshrn.u16 d16, q8, #7
	vqshrn.u16 d17, q9, #7
	vqshrn.u16 d18, q10, #7
	vqshrn.u16 d19, q11, #7
	vqshrn.u16 d20, q0, #7
	vqshrn.u16 d21, q1, #7
	vqshrn.u16 d22, q2, #7
	vqshrn.u16 d23, q3, #7

	vst1.64	{d16}, [r1], r4
	vst1.64	{d17}, [r1], r4 
	vst1.64	{d18}, [r1], r4 
	vst1.64	{d19}, [r1], r4
	vst1.64	{d20}, [r1], r4
	vst1.64	{d21}, [r1], r4 
	vst1.64	{d22}, [r1], r4 
	vst1.64	{d23}, [r1]	
	ldmia	sp!, {r4 - r11, pc}
	@ENDP     
	
	
_FilterBlock2dBil_wRecon_Armv7:  @PROC
@void FilterBlock2dBil_wRecon_C 
@(
@	UINT8 *SrcPtr, 		r0
@	UINT8 *dstPtr, 		r1
@	UINT32 SrcPixelsPerLine,r2
@	UINT32 LineStep, 	r3
@	FILTER_DATA * HFilter, 	r4
@	FILTER_DATA * VFilter 	r5 
@)
@{
@	INT32 FData[BLOCK_HEIGHT_WIDTH*11]@	// Temp data bufffer used in filtering	
@	// First filter 1-D horizontally...
@	FilterBlock2dBilFirstPass_wRecon_C ( SrcPtr, FData, SrcPixelsPerLine, HFilter )@	
@	// then 1-D vertically...
@	FilterBlock2dBilSecondPass_wRecon_C ( (UINT32 *)FData, dstPtr, LineStep, VFilter )@
@}
@[sp, #36] = LineStep, [sp, #40] = Filter
	stmdb	sp!, {r4 - r11, lr}
	ldr	r4,[sp,#36]	@HFilter
	ldr	r5,[sp,#40]	@VFilter
	
	ldrd	r6, [r4]			
	vdup.u8 d28, r6	
	vdup.u8 d29, r7	
@H	
	vld1.8 {q0}, [r0], r2		@d0 lTemp0= pSrc[0~7]
	vld1.8 {q1}, [r0], r2		@d2 lTemp0= pSrc[0~7]
	vld1.8 {q2}, [r0], r2		@d4 lTemp0= pSrc[0~7]
	vld1.8 {q3}, [r0], r2		@d6 lTemp0= pSrc[0~7]
	vld1.8 {q4}, [r0], r2		@d0 lTemp0= pSrc[0~7]
	vld1.8 {q5}, [r0], r2		@d2 lTemp0= pSrc[0~7]
	vld1.8 {q6}, [r0], r2		@d4 lTemp0= pSrc[0~7]
	vld1.8 {q7}, [r0], r2		@d6 lTemp0= pSrc[0~7]		
	vld1.8 {q15}, [r0]		@d30 lTemp0= pSrc[0~7]
		
	vmull.u8 q8, d0, d28			@5*lTemp0
	vmull.u8 q9, d2, d28			@5*lTemp0
	vmull.u8 q10, d4, d28			@5*lTemp0
	vmull.u8 q11, d6, d28			@5*lTemp0
					
	vext.u8 d24, d0, d1, #1		@lTemp1= pSrc[1~8]
	vext.u8 d25, d2, d3, #1		@lTemp1= pSrc[1~8]
	vext.u8 d26, d4, d5, #1		@lTemp1= pSrc[1~8]
	vext.u8 d27, d6, d7, #1		@lTemp1= pSrc[1~8]
	
	vmlal.u8 q8, d24, d29		@3*lTemp1
	vmlal.u8 q9, d25, d29		@3*lTemp1
	vmlal.u8 q10, d26, d29		@3*lTemp1
	vmlal.u8 q11, d27, d29		@3*lTemp1
@Two		
	vmull.u8 q0, d8, d28			@5*lTemp0
	vmull.u8 q1, d10, d28			@5*lTemp0
	vmull.u8 q2, d12, d28			@5*lTemp0
	vmull.u8 q3, d14, d28			@5*lTemp0	
					
	vext.u8 d8, d8, d9, #1		@lTemp1= pSrc[1~8]	
	vext.u8 d9, d10, d11, #1		@lTemp1= pSrc[1~8]
	vext.u8 d10, d12, d13, #1		@lTemp1= pSrc[1~8]
	vext.u8 d11, d14, d15, #1		@lTemp1= pSrc[1~8]			
	
	vmlal.u8 q0, d8, d29		@3*lTemp1
	vmlal.u8 q1, d9, d29		@3*lTemp1
	vmlal.u8 q2, d10, d29		@3*lTemp1
	vmlal.u8 q3, d11, d29		@3*lTemp1
	
	vmull.u8 q4, d30, d28			@5*lTemp0
	vext.u8 d12, d30, d31, #1		@lTemp1= pSrc[1~8]		
	vmlal.u8 q4, d12, d29		@3*lTemp1	
	
	vmov.u16 q15, #64	
	vadd.u16 q8, q8, q15		@+8
	vadd.u16 q9, q9, q15		@+8
	vadd.u16 q10, q10, q15		@+8
	vadd.u16 q11, q11, q15		@+8	
	vadd.u16 q5, q0, q15		@+8
	vadd.u16 q1, q1, q15		@+8
	vadd.u16 q2, q2, q15		@+8
	vadd.u16 q3, q3, q15		@+8				
	vadd.u16 q4, q4, q15		@+8
	
	vshr.u16 q8, q8, #7
	vshr.u16 q9, q9, #7
	vshr.u16 q10, q10, #7
	vshr.u16 q11, q11, #7
	vshr.u16 q5, q5, #7
	vshr.u16 q1, q1, #7
	vshr.u16 q2, q2, #7
	vshr.u16 q3, q3, #7
	vshr.u16 q4, q4, #7			
@V        
	ldrd	r6, [r5]
	vmov	s0, s1, r6, r7	@d0[0], d0[2]
		          
	vmull.u16	q6, d16, d0[0]		@5*lTemp0@
	vmull.u16	q7, d17, d0[0]		@5*lTemp0@	
	vmlal.u16	q6, d18, d0[2]		@3*lTemp0@
	vmlal.u16	q7, d19, d0[2]		@3*lTemp0@
	
	vmull.u16	q12, d18, d0[0]		@5*lTemp0@
	vmull.u16	q13, d19, d0[0]		@5*lTemp0@	
	vmlal.u16	q12, d20, d0[2]		@3*lTemp0@
	vmlal.u16	q13, d21, d0[2]		@3*lTemp0@
	
	vmull.u16	q14, d20, d0[0]		@5*lTemp0@
	vmull.u16	q8, d21, d0[0]		@5*lTemp0@	
	vmlal.u16	q14, d22, d0[2]		@3*lTemp0@
	vmlal.u16	q8, d23, d0[2]		@3*lTemp0@
	
	vmull.u16	q9, d22, d0[0]		@5*lTemp0@
	vmull.u16	q10, d23, d0[0]		@5*lTemp0@	
	vmlal.u16	q9, d10, d0[2]		@3*lTemp0@
	vmlal.u16	q10, d11, d0[2]		@3*lTemp0@		
	
	vaddw.u16 q6, q6, d30		@+8
	vaddw.u16 q7, q7, d30		@+8
	vaddw.u16 q12, q12, d30		@+8
	vaddw.u16 q13, q13, d30		@+8
	vaddw.u16 q14, q14, d30		@+8
	vaddw.u16 q8, q8, d30		@+8
	vaddw.u16 q9, q9, d30		@+8
	vaddw.u16 q10, q10, d30		@+8		
				
	vqshrn.u32 d22, q6, #7
	vqshrn.u32 d23, q7, #7
	vqshrn.u32 d12, q12, #7
	vqshrn.u32 d13, q13, #7
	vqshrn.u32 d14, q14, #7
	vqshrn.u32 d15, q8, #7	
	vqshrn.u32 d24, q9, #7
	vqshrn.u32 d25, q10, #7

	vqmovn.u16	d22, q11
	vqmovn.u16	d12, q6
	vqmovn.u16	d14, q7
	vqmovn.u16	d24, q12
	
	vst1.64	{d22}, [r1], r3
	vst1.64	{d12}, [r1], r3 
	vst1.64	{d14}, [r1], r3 
	vst1.64	{d24}, [r1], r3	
@Tow
	vmull.u16	q6, d10, d0[0]		@5*lTemp0@
	vmull.u16	q7, d11, d0[0]		@5*lTemp0@	
	vmlal.u16	q6, d2, d0[2]		@3*lTemp0@
	vmlal.u16	q7, d3, d0[2]		@3*lTemp0@
	
	vmull.u16	q12, d2, d0[0]		@5*lTemp0@
	vmull.u16	q13, d3, d0[0]		@5*lTemp0@	
	vmlal.u16	q12, d4, d0[2]		@3*lTemp0@
	vmlal.u16	q13, d5, d0[2]		@3*lTemp0@
	
	vmull.u16	q14, d4, d0[0]		@5*lTemp0@
	vmull.u16	q8, d5, d0[0]		@5*lTemp0@	
	vmlal.u16	q14, d6, d0[2]		@3*lTemp0@
	vmlal.u16	q8, d7, d0[2]		@3*lTemp0@
	
	vmull.u16	q9, d6, d0[0]		@5*lTemp0@
	vmull.u16	q10, d7, d0[0]		@5*lTemp0@	
	vmlal.u16	q9, d8, d0[2]		@3*lTemp0@
	vmlal.u16	q10, d9, d0[2]		@3*lTemp0@		
	
	vaddw.u16 q6, q6, d30		@+8
	vaddw.u16 q7, q7, d30		@+8
	vaddw.u16 q12, q12, d30		@+8
	vaddw.u16 q13, q13, d30		@+8
	vaddw.u16 q14, q14, d30		@+8
	vaddw.u16 q8, q8, d30		@+8
	vaddw.u16 q9, q9, d30		@+8
	vaddw.u16 q10, q10, d30		@+8		
				
	vqshrn.u32 d22, q6, #7
	vqshrn.u32 d23, q7, #7
	vqshrn.u32 d12, q12, #7
	vqshrn.u32 d13, q13, #7
	vqshrn.u32 d14, q14, #7
	vqshrn.u32 d15, q8, #7	
	vqshrn.u32 d24, q9, #7
	vqshrn.u32 d25, q10, #7

	vqmovn.u16	d22, q11
	vqmovn.u16	d12, q6
	vqmovn.u16	d14, q7
	vqmovn.u16	d24, q12
	
	vst1.64	{d22}, [r1], r3
	vst1.64	{d12}, [r1], r3 
	vst1.64	{d14}, [r1], r3 
	vst1.64	{d24}, [r1]	
	
	ldmia	sp!, {r4 - r11, pc}
	
	
_FilterBlock1d_wRecon_Armv7:  @PROC
@void FilterBlock1dBil_wRecon_C
@(
@	UINT8  *SrcPtr, 	r0
@	UINT8  *dstPtr,		r1
@	INT32	PixelStep,	r2
@	INT32	SrcStride,	r3
@	INT32	LineStep,	r4
@	FILTER_DATA  *Filter 	r5
@)
@{
@	UINT32 i, j@
@	INT32  Temp@ 
@
@	// SrcPtr is increment each time in the inner loop, 8 in all.
@	SrcPixelsPerLine -= 8@
@	for (i = 0@ i < 8@ i++)
@	{
@		for (j = 0@ j < 8@ j++)
@		{
@			// Apply filter...
@			Temp =  -((INT32)SrcPtr[-(INT32)PixelStep]* Filter[0]) +
@					((INT32)SrcPtr[0]* Filter[1]) +
@					((INT32)SrcPtr[PixelStep]* Filter[2]) +
@					-((INT32)SrcPtr[2*PixelStep]* Filter[3]) + 
@					(FILTER_WEIGHT >> 1)@       // Rounding
@			// Normalize back to 0-255
@			Temp = Temp >> FILTER_SHIFT@
@			if ( Temp < 0 ) Temp = 0@
@			else if ( Temp > 255 ) Temp = 255@
@			
@			dstPtr[j] = (UINT8)Temp@
@			SrcPtr++@
@		}	
@		// Next row...
@		SrcPtr  += SrcPixelsPerLine@
@		//diffPtr += 8@
@		dstPtr  += LineStep@
@	}
@}
@[sp, #36] = LineStep, [sp, #40] = Filter
	stmdb	sp!, {r4 - r11, lr}
	ldr	r4,[sp,#36]	@LineStep
	ldr	r5,[sp,#40]	@Filter
	cmp	r2, #1
	ldrd	r6, [r5], #8
	ldrd	r8, [r5]	
	vmov.u16 q9, #64			
	vdup.u8 d28, r6	
	vdup.u8 d29, r7
	vdup.u8 d30, r8	
	vdup.u8 d31, r9
				
	bne	FilterBlock1d_wRecon_Armv7_V	
	sub	r0, r0, #1	
	vld1.8 {q1}, [r0], r3		@d2 lTemp0= pSrc[-1~6] d3 = pSrc[7~14]
	vld1.8 {q2}, [r0], r3		@d4 lTemp0= pSrc[-1~6] d5 = pSrc[7~14]
	vld1.8 {q3}, [r0], r3		@d6 lTemp0= pSrc[-1~6] d7 = pSrc[7~14]
	vld1.8 {q4}, [r0], r3		@d8 lTemp0= pSrc[-1~6] d9 = pSrc[7~14]
			
@first 4 line									    
	vext.u8 d20, d2, d3, #1		@lTemp1= pSrc[0~7]
	vext.u8 d21, d2, d3, #2		@lTemp2= pSrc[1~8]
	vext.u8 d3, d2, d3, #3		@lTemp3= pSrc[2~9]
	    
	vext.u8 d22, d4, d5, #1		@lTemp1= pSrc[0~7]
	vext.u8 d23, d4, d5, #2		@lTemp2= pSrc[1~8]
	vext.u8 d5, d4, d5, #3		@lTemp3= pSrc[2~9]
	             
	vext.u8 d24, d6, d7, #1		@lTemp1= pSrc[0~7]
	vext.u8 d25, d6, d7, #2		@lTemp2= pSrc[1~8]
	vext.u8 d7, d6, d7, #3		@lTemp3= pSrc[2~9]
	
	vext.u8 d26, d8, d9, #1		@lTemp1= pSrc[0~7]
	vext.u8 d27, d8, d9, #2		@lTemp2= pSrc[1~8]
	vext.u8 d9, d8, d9, #3		@lTemp3= pSrc[2~9]
	
	vmull.u8 q5, d20, d29		@5*lTemp0
	vmull.u8 q6, d22, d29		@5*lTemp0
	vmull.u8 q7, d24, d29		@5*lTemp0
	vmull.u8 q8, d26, d29		@5*lTemp0
	
	vmlal.u8 q5, d21, d30		@3*lTemp1
	vmlal.u8 q6, d23, d30		@3*lTemp1
	vmlal.u8 q7, d25, d30		@3*lTemp1
	vmlal.u8 q8, d27, d30		@3*lTemp1
	
	vmull.u8 q10, d2, d28		@-5*lTemp0
	vmull.u8 q11, d4, d28		@-5*lTemp0
	vmull.u8 q12, d6, d28		@-5*lTemp0
	vmull.u8 q13, d8, d28		@-5*lTemp0
	
	vmlal.u8 q10, d3, d31		@-3*lTemp1
	vmlal.u8 q11, d5, d31		@-3*lTemp1
	vmlal.u8 q12, d7, d31		@-3*lTemp1
	vmlal.u8 q13, d9, d31		@-3*lTemp1	
	
	vadd.u16 q5, q5, q9		@+64	
	vadd.u16 q6, q6, q9		@+64
	vadd.u16 q7, q7, q9		@+64	
	vadd.u16 q8, q8, q9		@+64
	
	vsubl.u16 q0, d10, d20		@
	vsubl.u16 q5, d11, d21		@		
	vsubl.u16 q10, d12, d22		@
	vsubl.u16 q6, d13, d23		@
	vsubl.u16 q11, d14, d24		@
	vsubl.u16 q7, d15, d25		@
	vsubl.u16 q12, d16, d26		@
	vsubl.u16 q8, d17, d27		@
	
	vqshrun.s32 d0, q0, #7
	vqshrun.s32 d1, q5, #7
	vqshrun.s32 d2, q10, #7
	vqshrun.s32 d3, q6, #7
	vqshrun.s32 d4, q11, #7
	vqshrun.s32 d5, q7, #7
	vqshrun.s32 d6, q12, #7
	vqshrun.s32 d7, q8, #7
	
	vqmovn.u16	d0, q0	
	vqmovn.u16	d1, q1
	vqmovn.u16	d2, q2
	vqmovn.u16	d3, q3						
	
	vst1.64	{d0}, [r1], r4
	vst1.64	{d1}, [r1], r4 
	vst1.64	{d2}, [r1], r4 
	vst1.64	{d3}, [r1], r4		

	vld1.8 {q1}, [r0], r3		@d2 lTemp0= pSrc[-1~6] d3 = pSrc[7~14]
	vld1.8 {q2}, [r0], r3		@d4 lTemp0= pSrc[-1~6] d5 = pSrc[7~14]
	vld1.8 {q3}, [r0], r3		@d6 lTemp0= pSrc[-1~6] d7 = pSrc[7~14]
	vld1.8 {q4}, [r0], r3		@d8 lTemp0= pSrc[-1~6] d9 = pSrc[7~14]
			
@last 4 line									    
	vext.u8 d20, d2, d3, #1		@lTemp1= pSrc[0~7]
	vext.u8 d21, d2, d3, #2		@lTemp2= pSrc[1~8]
	vext.u8 d3, d2, d3, #3		@lTemp3= pSrc[2~9]
	    
	vext.u8 d22, d4, d5, #1		@lTemp1= pSrc[0~7]
	vext.u8 d23, d4, d5, #2		@lTemp2= pSrc[1~8]
	vext.u8 d5, d4, d5, #3		@lTemp3= pSrc[2~9]
	             
	vext.u8 d24, d6, d7, #1		@lTemp1= pSrc[0~7]
	vext.u8 d25, d6, d7, #2		@lTemp2= pSrc[1~8]
	vext.u8 d7, d6, d7, #3		@lTemp3= pSrc[2~9]
	
	vext.u8 d26, d8, d9, #1		@lTemp1= pSrc[0~7]
	vext.u8 d27, d8, d9, #2		@lTemp2= pSrc[1~8]
	vext.u8 d9, d8, d9, #3		@lTemp3= pSrc[2~9]
	
	vmull.u8 q5, d20, d29		@5*lTemp0
	vmull.u8 q6, d22, d29		@5*lTemp0
	vmull.u8 q7, d24, d29		@5*lTemp0
	vmull.u8 q8, d26, d29		@5*lTemp0
	
	vmlal.u8 q5, d21, d30		@3*lTemp1
	vmlal.u8 q6, d23, d30		@3*lTemp1
	vmlal.u8 q7, d25, d30		@3*lTemp1
	vmlal.u8 q8, d27, d30		@3*lTemp1
	
	vmull.u8 q10, d2, d28		@-5*lTemp0
	vmull.u8 q11, d4, d28		@-5*lTemp0
	vmull.u8 q12, d6, d28		@-5*lTemp0
	vmull.u8 q13, d8, d28		@-5*lTemp0
	
	vmlal.u8 q10, d3, d31		@-3*lTemp1
	vmlal.u8 q11, d5, d31		@-3*lTemp1
	vmlal.u8 q12, d7, d31		@-3*lTemp1
	vmlal.u8 q13, d9, d31		@-3*lTemp1	
	
	vadd.u16 q5, q5, q9		@+64	
	vadd.u16 q6, q6, q9		@+64
	vadd.u16 q7, q7, q9		@+64	
	vadd.u16 q8, q8, q9		@+64
	
	vsubl.u16 q0, d10, d20		@
	vsubl.u16 q5, d11, d21		@		
	vsubl.u16 q10, d12, d22		@
	vsubl.u16 q6, d13, d23		@
	vsubl.u16 q11, d14, d24		@
	vsubl.u16 q7, d15, d25		@
	vsubl.u16 q12, d16, d26		@
	vsubl.u16 q8, d17, d27		@
	
	vqshrun.s32 d0, q0, #7
	vqshrun.s32 d1, q5, #7
	vqshrun.s32 d2, q10, #7
	vqshrun.s32 d3, q6, #7
	vqshrun.s32 d4, q11, #7
	vqshrun.s32 d5, q7, #7
	vqshrun.s32 d6, q12, #7
	vqshrun.s32 d7, q8, #7
	
	vqmovn.u16	d0, q0	
	vqmovn.u16	d1, q1
	vqmovn.u16	d2, q2
	vqmovn.u16	d3, q3						
	
	vst1.64	{d0}, [r1], r4
	vst1.64	{d1}, [r1], r4 
	vst1.64	{d2}, [r1], r4 
	vst1.64	{d3}, [r1]				
	ldmia	sp!, {r4 - r11, pc} 
							
FilterBlock1d_wRecon_Armv7_V:
	sub	r0, r0, r3
	vld1.8 {d0}, [r0], r3		@d0 lTemp0= pSrc[0~7]
	vld1.8 {d1}, [r0], r3		@d1 lTemp0= pSrc[0~7]
	vld1.8 {d2}, [r0], r3		@d2 lTemp0= pSrc[0~7]	
	vld1.8 {d3}, [r0], r3		@d3 lTemp0= pSrc[0~7]		
	vld1.8 {d4}, [r0], r3		@d4 lTemp0= pSrc[0~7]
	vld1.8 {d5}, [r0], r3		@d5 lTemp0= pSrc[0~7]
	vld1.8 {d6}, [r0], r3		@d6 lTemp0= pSrc[0~7]
	vld1.8 {d7}, [r0], r3		@d7 lTemp0= pSrc[0~7]	
	vld1.8 {d8}, [r0], r3		@d8 lTemp0= pSrc[0~7]
	vld1.8 {d9}, [r0], r3		@d8 lTemp0= pSrc[0~7]

	vmull.u8 q5, d1, d29		@5*lTemp0
	vmull.u8 q6, d2, d29		@5*lTemp0
	vmull.u8 q7, d3, d29		@5*lTemp0
	vmull.u8 q8, d4, d29		@5*lTemp0
	
	vmlal.u8 q5, d2, d30		@3*lTemp1
	vmlal.u8 q6, d3, d30		@3*lTemp1
	vmlal.u8 q7, d4, d30		@3*lTemp1
	vmlal.u8 q8, d5, d30		@3*lTemp1
	
	vmull.u8 q10, d0, d28		@-5*lTemp0
	vmull.u8 q11, d1, d28		@-5*lTemp0
	vmull.u8 q12, d2, d28		@-5*lTemp0
	vmull.u8 q13, d3, d28		@-5*lTemp0
	
	vmlal.u8 q10, d3, d31		@-3*lTemp1
	vmlal.u8 q11, d4, d31		@-3*lTemp1
	vmlal.u8 q12, d5, d31		@-3*lTemp1
	vmlal.u8 q13, d6, d31		@-3*lTemp1	
	
	vadd.u16 q5, q5, q9		@+64	
	vadd.u16 q6, q6, q9		@+64
	vadd.u16 q7, q7, q9		@+64	
	vadd.u16 q8, q8, q9		@+64
	
	vsubl.u16 q0, d10, d20		@
	vsubl.u16 q5, d11, d21		@		
	vsubl.u16 q10, d12, d22		@
	vsubl.u16 q6, d13, d23		@
	vsubl.u16 q11, d14, d24		@
	vsubl.u16 q7, d15, d25		@
	vsubl.u16 q12, d16, d26		@
	vsubl.u16 q8, d17, d27		@
	
	vqshrun.s32 d0, q0, #7
	vqshrun.s32 d1, q5, #7
	vqshrun.s32 d2, q10, #7
	vqshrun.s32 d3, q6, #7
	vqmovn.u16	d0, q0	
	vqmovn.u16	d1, q1	
	vst1.64	{d0}, [r1], r4
	vst1.64	{d1}, [r1], r4 
			
	vqshrun.s32 d0, q11, #7
	vqshrun.s32 d1, q7, #7
	vqshrun.s32 d2, q12, #7
	vqshrun.s32 d3, q8, #7
	vqmovn.u16	d0, q0	
	vqmovn.u16	d1, q1	
	vst1.64	{d0}, [r1], r4
	vst1.64	{d1}, [r1], r4	
		
	vmull.u8 q5, d5, d29		@5*lTemp0
	vmull.u8 q6, d6, d29		@5*lTemp0
	vmull.u8 q7, d7, d29		@5*lTemp0
	vmull.u8 q8, d8, d29		@5*lTemp0
	
	vmlal.u8 q5, d6, d30		@3*lTemp1
	vmlal.u8 q6, d7, d30		@3*lTemp1
	vmlal.u8 q7, d8, d30		@3*lTemp1
	vmlal.u8 q8, d9, d30		@3*lTemp1
	
	vmull.u8 q10, d4, d28		@-5*lTemp0
	vmull.u8 q11, d5, d28		@-5*lTemp0
	vmull.u8 q12, d6, d28		@-5*lTemp0
	vmull.u8 q13, d7, d28		@-5*lTemp0
	
	vld1.8 {d4}, [r0]		@d8 lTemp0= pSrc[0~7]
		
	vmlal.u8 q10, d7, d31		@-3*lTemp1
	vmlal.u8 q11, d8, d31		@-3*lTemp1
	vmlal.u8 q12, d9, d31		@-3*lTemp1
	vmlal.u8 q13, d4, d31		@-3*lTemp1	
	
	vadd.u16 q5, q5, q9		@+64	
	vadd.u16 q6, q6, q9		@+64
	vadd.u16 q7, q7, q9		@+64	
	vadd.u16 q8, q8, q9		@+64
	
	vsubl.u16 q2, d10, d20		@
	vsubl.u16 q5, d11, d21		@		
	vsubl.u16 q10, d12, d22		@
	vsubl.u16 q6, d13, d23		@
	vsubl.u16 q11, d14, d24		@
	vsubl.u16 q7, d15, d25		@
	vsubl.u16 q12, d16, d26		@
	vsubl.u16 q8, d17, d27		@
	
	vqshrun.s32 d4, q2, #7
	vqshrun.s32 d5, q5, #7
	vqshrun.s32 d6, q10, #7
	vqshrun.s32 d7, q6, #7
	vqshrun.s32 d8, q11, #7
	vqshrun.s32 d9, q7, #7
	vqshrun.s32 d10, q12, #7
	vqshrun.s32 d11, q8, #7
	
	vqmovn.u16	d4, q2	
	vqmovn.u16	d5, q3
	vqmovn.u16	d6, q4
	vqmovn.u16	d7, q5

	vst1.64	{d4}, [r1], r4
	vst1.64	{d5}, [r1], r4 
	vst1.64	{d6}, [r1], r4 
	vst1.64	{d7}, [r1]		
	ldmia	sp!, {r4 - r11, pc}
	@ENDP     
	
	
_FilterBlock2d_wRecon_Armv7:  @PROC
@void FilterBlock2d_wRecon_C 
@(
@	UINT8 *SrcPtr, 		r0
@	UINT8 *dstPtr, 		r1
@	UINT32 SrcPixelsPerLine,r2
@	UINT32 LineStep, 	r3
@	FILTER_DATA * HFilter, 	r4
@	FILTER_DATA * VFilter 	r5 
@)
@{
@	INT32 FData[BLOCK_HEIGHT_WIDTH*11]@	// Temp data bufffer used in filtering
@
@	// First filter 1-D horizontally...
@	FilterBlock2dFirstPass_wRecon_C ( SrcPtr-SrcPixelsPerLine, FData, SrcPixelsPerLine, HFilter )@
@	
@	// then filter verticaly...
@	FilterBlock2dSecondPass_wRecon_C ( FData+BLOCK_HEIGHT_WIDTH, dstPtr, LineStep, VFilter )@
@}
@[sp, #36] = LineStep, [sp, #40] = Filter
	stmdb	sp!, {r4 - r11, lr}
	ldr	r4,[sp,#36]	@HFilter
	ldr	r5,[sp,#40]	@VFilter
	sub	sp, sp, #176
	vmov.u16 q9, #64	
@H	
	ldrd	r6, [r4], #8
	ldrd	r8, [r4]
	sub	r0, r0, r2					
	vdup.u8 d28, r6	
	vdup.u8 d29, r7
	vdup.u8 d30, r8	
	vdup.u8 d31, r9
	sub	r0, r0, #1	
	vld1.8 {q1}, [r0], r2		@d2 lTemp0= pSrc[-1~6] d3 = pSrc[7~14]
	vld1.8 {q2}, [r0], r2		@d4 lTemp0= pSrc[-1~6] d5 = pSrc[7~14]
	vld1.8 {q3}, [r0], r2		@d6 lTemp0= pSrc[-1~6] d7 = pSrc[7~14]
	vld1.8 {q4}, [r0], r2		@d8 lTemp0= pSrc[-1~6] d9 = pSrc[7~14]
			
@first 4 line									    
	vext.u8 d20, d2, d3, #1		@lTemp1= pSrc[0~7]
	vext.u8 d21, d2, d3, #2		@lTemp2= pSrc[1~8]
	vext.u8 d3, d2, d3, #3		@lTemp3= pSrc[2~9]
	    
	vext.u8 d22, d4, d5, #1		@lTemp1= pSrc[0~7]
	vext.u8 d23, d4, d5, #2		@lTemp2= pSrc[1~8]
	vext.u8 d5, d4, d5, #3		@lTemp3= pSrc[2~9]
	             
	vext.u8 d24, d6, d7, #1		@lTemp1= pSrc[0~7]
	vext.u8 d25, d6, d7, #2		@lTemp2= pSrc[1~8]
	vext.u8 d7, d6, d7, #3		@lTemp3= pSrc[2~9]
	
	vext.u8 d26, d8, d9, #1		@lTemp1= pSrc[0~7]
	vext.u8 d27, d8, d9, #2		@lTemp2= pSrc[1~8]
	vext.u8 d9, d8, d9, #3		@lTemp3= pSrc[2~9]
	
	vmull.u8 q5, d20, d29		@5*lTemp0
	vmull.u8 q6, d22, d29		@5*lTemp0
	vmull.u8 q7, d24, d29		@5*lTemp0
	vmull.u8 q8, d26, d29		@5*lTemp0
	
	vmlal.u8 q5, d21, d30		@3*lTemp1
	vmlal.u8 q6, d23, d30		@3*lTemp1
	vmlal.u8 q7, d25, d30		@3*lTemp1
	vmlal.u8 q8, d27, d30		@3*lTemp1
	
	vmull.u8 q10, d2, d28		@-5*lTemp0
	vmull.u8 q11, d4, d28		@-5*lTemp0
	vmull.u8 q12, d6, d28		@-5*lTemp0
	vmull.u8 q13, d8, d28		@-5*lTemp0
	
	vmlal.u8 q10, d3, d31		@-3*lTemp1
	vmlal.u8 q11, d5, d31		@-3*lTemp1
	vmlal.u8 q12, d7, d31		@-3*lTemp1
	vmlal.u8 q13, d9, d31		@-3*lTemp1	
	
	vadd.u16 q5, q5, q9		@+64	
	vadd.u16 q6, q6, q9		@+64
	vadd.u16 q7, q7, q9		@+64	
	vadd.u16 q8, q8, q9		@+64
	
	vsubl.u16 q0, d10, d20		@
	vsubl.u16 q5, d11, d21		@		
	vsubl.u16 q10, d12, d22		@
	vsubl.u16 q6, d13, d23		@
	vsubl.u16 q11, d14, d24		@
	vsubl.u16 q7, d15, d25		@
	vsubl.u16 q12, d16, d26		@
	vsubl.u16 q8, d17, d27		@
	
	vqshrun.s32 d0, q0, #7
	vqshrun.s32 d1, q5, #7
	vqshrun.s32 d2, q10, #7
	vqshrun.s32 d3, q6, #7
	vqshrun.s32 d4, q11, #7
	vqshrun.s32 d5, q7, #7
	vqshrun.s32 d6, q12, #7
	vqshrun.s32 d7, q8, #7
					
	vst1.64 {q0}, [sp]!
	vst1.64 {q1}, [sp]!
	vst1.64 {q2}, [sp]!
	vst1.64 {q3}, [sp]!	

	vld1.8 {q1}, [r0], r2		@d2 lTemp0= pSrc[-1~6] d3 = pSrc[7~14]
	vld1.8 {q2}, [r0], r2		@d4 lTemp0= pSrc[-1~6] d5 = pSrc[7~14]
	vld1.8 {q3}, [r0], r2		@d6 lTemp0= pSrc[-1~6] d7 = pSrc[7~14]
	vld1.8 {q4}, [r0], r2		@d8 lTemp0= pSrc[-1~6] d9 = pSrc[7~14]
			
@last 4 line									    
	vext.u8 d20, d2, d3, #1		@lTemp1= pSrc[0~7]
	vext.u8 d21, d2, d3, #2		@lTemp2= pSrc[1~8]
	vext.u8 d3, d2, d3, #3		@lTemp3= pSrc[2~9]
	    
	vext.u8 d22, d4, d5, #1		@lTemp1= pSrc[0~7]
	vext.u8 d23, d4, d5, #2		@lTemp2= pSrc[1~8]
	vext.u8 d5, d4, d5, #3		@lTemp3= pSrc[2~9]
	             
	vext.u8 d24, d6, d7, #1		@lTemp1= pSrc[0~7]
	vext.u8 d25, d6, d7, #2		@lTemp2= pSrc[1~8]
	vext.u8 d7, d6, d7, #3		@lTemp3= pSrc[2~9]
	
	vext.u8 d26, d8, d9, #1		@lTemp1= pSrc[0~7]
	vext.u8 d27, d8, d9, #2		@lTemp2= pSrc[1~8]
	vext.u8 d9, d8, d9, #3		@lTemp3= pSrc[2~9]
	
	vmull.u8 q5, d20, d29		@5*lTemp0
	vmull.u8 q6, d22, d29		@5*lTemp0
	vmull.u8 q7, d24, d29		@5*lTemp0
	vmull.u8 q8, d26, d29		@5*lTemp0
	
	vmlal.u8 q5, d21, d30		@3*lTemp1
	vmlal.u8 q6, d23, d30		@3*lTemp1
	vmlal.u8 q7, d25, d30		@3*lTemp1
	vmlal.u8 q8, d27, d30		@3*lTemp1
	
	vmull.u8 q10, d2, d28		@-5*lTemp0
	vmull.u8 q11, d4, d28		@-5*lTemp0
	vmull.u8 q12, d6, d28		@-5*lTemp0
	vmull.u8 q13, d8, d28		@-5*lTemp0
	
	vmlal.u8 q10, d3, d31		@-3*lTemp1
	vmlal.u8 q11, d5, d31		@-3*lTemp1
	vmlal.u8 q12, d7, d31		@-3*lTemp1
	vmlal.u8 q13, d9, d31		@-3*lTemp1	
	
	vadd.u16 q5, q5, q9		@+64	
	vadd.u16 q6, q6, q9		@+64
	vadd.u16 q7, q7, q9		@+64	
	vadd.u16 q8, q8, q9		@+64
	
	vsubl.u16 q0, d10, d20		@
	vsubl.u16 q5, d11, d21		@		
	vsubl.u16 q10, d12, d22		@
	vsubl.u16 q6, d13, d23		@
	vsubl.u16 q11, d14, d24		@
	vsubl.u16 q7, d15, d25		@
	vsubl.u16 q12, d16, d26		@
	vsubl.u16 q8, d17, d27		@
	
	vqshrun.s32 d0, q0, #7
	vqshrun.s32 d1, q5, #7
	vqshrun.s32 d2, q10, #7
	vqshrun.s32 d3, q6, #7
	vqshrun.s32 d4, q11, #7
	vqshrun.s32 d5, q7, #7
	vqshrun.s32 d6, q12, #7
	vqshrun.s32 d7, q8, #7
					
	vst1.64 {q0}, [sp]!
	vst1.64 {q1}, [sp]!
	vst1.64 {q2}, [sp]!
	vst1.64 {q3}, [sp]!					
	
	vld1.8 {q1}, [r0], r2		@d2 lTemp0= pSrc[-1~6] d3 = pSrc[7~14]
	vld1.8 {q2}, [r0], r2		@d4 lTemp0= pSrc[-1~6] d5 = pSrc[7~14]
	vld1.8 {q3}, [r0]		@d6 lTemp0= pSrc[-1~6] d7 = pSrc[7~14]
			
@last 8~11 line									    
	vext.u8 d20, d2, d3, #1		@lTemp1= pSrc[0~7]
	vext.u8 d21, d2, d3, #2		@lTemp2= pSrc[1~8]
	vext.u8 d3, d2, d3, #3		@lTemp3= pSrc[2~9]
	    
	vext.u8 d22, d4, d5, #1		@lTemp1= pSrc[0~7]
	vext.u8 d23, d4, d5, #2		@lTemp2= pSrc[1~8]
	vext.u8 d5, d4, d5, #3		@lTemp3= pSrc[2~9]
	             
	vext.u8 d24, d6, d7, #1		@lTemp1= pSrc[0~7]
	vext.u8 d25, d6, d7, #2		@lTemp2= pSrc[1~8]
	vext.u8 d7, d6, d7, #3		@lTemp3= pSrc[2~9]
	
	vmull.u8 q5, d20, d29		@5*lTemp0
	vmull.u8 q6, d22, d29		@5*lTemp0
	vmull.u8 q7, d24, d29		@5*lTemp0
	
	vmlal.u8 q5, d21, d30		@3*lTemp1
	vmlal.u8 q6, d23, d30		@3*lTemp1
	vmlal.u8 q7, d25, d30		@3*lTemp1
	
	vmull.u8 q10, d2, d28		@-5*lTemp0
	vmull.u8 q11, d4, d28		@-5*lTemp0
	vmull.u8 q12, d6, d28		@-5*lTemp0
	
	vmlal.u8 q10, d3, d31		@-3*lTemp1
	vmlal.u8 q11, d5, d31		@-3*lTemp1
	vmlal.u8 q12, d7, d31		@-3*lTemp1	
	
	vadd.u16 q5, q5, q9		@+64	
	vadd.u16 q6, q6, q9		@+64
	vadd.u16 q7, q7, q9		@+64	
	
	vsubl.u16 q0, d10, d20		@
	vsubl.u16 q5, d11, d21		@		
	vsubl.u16 q10, d12, d22		@
	vsubl.u16 q6, d13, d23		@
	vsubl.u16 q11, d14, d24		@
	vsubl.u16 q7, d15, d25		@
@	vsubl.u16 q12, d16, d26		@
@	vsubl.u16 q8, d17, d27		@
	
	vqshrun.s32 d0, q0, #7
	vqshrun.s32 d1, q5, #7
	vqshrun.s32 d2, q10, #7
	vqshrun.s32 d3, q6, #7
	vqshrun.s32 d4, q11, #7
	vqshrun.s32 d5, q7, #7
@	vqshrun.s32 d6, q12, #7
@	vqshrun.s32 d7, q8, #7
					
	vst1.64 {q0}, [sp]!
	vst1.64 {q1}, [sp]!
	vst1.64 {q2}, [sp]
@	vst1.64 {q3}, [sp]!
	
	sub	sp, sp, #160	
@V        
	ldrd	r6, [r5], #8
	ldrd	r8, [r5]
	vmov	s0, s1, r6, r7	@d0[0], d0[2]
	vmov	s2, s3, r8, r9	@d1[0], d1[2]
	
	vld1.64 {q1}, [sp]!		@-1
	vld1.64 {q2}, [sp]!		@0 line 0
	vld1.64 {q3}, [sp]!		@1 line 1
	vld1.64 {q4}, [sp]!		@2 line 2
	vld1.64 {q5}, [sp]!		@3 line 2		
@line 1-2	@q1, q2, q3, q4, q5						          
	vmull.u16	q10, d4, d0[2]		@5*lTemp0@
	vmull.u16	q11, d5, d0[2]		@5*lTemp0@
	vmull.u16	q12, d6, d0[2]		@5*lTemp0@
	vmull.u16	q13, d7, d0[2]		@5*lTemp0@	
		
	vmlal.u16	q10, d6, d1[0]		@3*lTemp0@
	vmlal.u16	q11, d7, d1[0]		@3*lTemp0@
	vmlal.u16	q12, d8, d1[0]		@3*lTemp0@
	vmlal.u16	q13, d9, d1[0]		@3*lTemp0@

	vmull.u16	q14, d2, d0[0]		@-5*lTemp0@
	vmull.u16	q15, d3, d0[0]		@-5*lTemp0@
	vmull.u16	q7, d4, d0[0]		@-5*lTemp0@
	vmull.u16	q8, d5, d0[0]		@-5*lTemp0@	
		
	vmlal.u16	q14, d8, d1[2]		@-3*lTemp0@
	vmlal.u16	q15, d9, d1[2]		@-3*lTemp0@
	vmlal.u16	q7, d10, d1[2]		@-3*lTemp0@
	vmlal.u16	q8, d11, d1[2]		@-3*lTemp0@
	
	vaddw.u16 q10, q10, d18		@+64	
	vaddw.u16 q11, q11, d18		@+64
	vaddw.u16 q12, q12, d18		@+64
	vaddw.u16 q13, q13, d18		@+64
	
	vsub.s32 q10, q10, q14		@
	vsub.s32 q11, q11, q15		@
	vsub.s32 q12, q12, q7		@
	vsub.s32 q13, q13, q8		@				
               
	vqshrun.s32 d20, q10, #7
	vqshrun.s32 d21, q11, #7	
	vqshrun.s32 d22, q12, #7
	vqshrun.s32 d23, q13, #7	

	vqmovn.u16	d20, q10
	vqmovn.u16	d22, q11
	
	vst1.64	{d20}, [r1], r3 
	vst1.64	{d22}, [r1], r3	
	
	vld1.64 {q1}, [sp]!		@
	vld1.64 {q2}, [sp]!		@			
@line 3-4	@q3, q4, q5, q1, q2						          
	vmull.u16	q10, d8, d0[2]		@5*lTemp0@
	vmull.u16	q11, d9, d0[2]		@5*lTemp0@
	vmull.u16	q12, d10, d0[2]		@5*lTemp0@
	vmull.u16	q13, d11, d0[2]		@5*lTemp0@	
		
	vmlal.u16	q10, d10, d1[0]		@3*lTemp0@
	vmlal.u16	q11, d11, d1[0]		@3*lTemp0@
	vmlal.u16	q12, d2, d1[0]		@3*lTemp0@
	vmlal.u16	q13, d3, d1[0]		@3*lTemp0@

	vmull.u16	q14, d6, d0[0]		@-5*lTemp0@
	vmull.u16	q15, d7, d0[0]		@-5*lTemp0@
	vmull.u16	q7, d8, d0[0]		@-5*lTemp0@
	vmull.u16	q8, d9, d0[0]		@-5*lTemp0@	
		
	vmlal.u16	q14, d2, d1[2]		@-3*lTemp0@
	vmlal.u16	q15, d3, d1[2]		@-3*lTemp0@
	vmlal.u16	q7, d4, d1[2]		@-3*lTemp0@
	vmlal.u16	q8, d5, d1[2]		@-3*lTemp0@
	
	vaddw.u16 q10, q10, d18		@+64	
	vaddw.u16 q11, q11, d18		@+64
	vaddw.u16 q12, q12, d18		@+64
	vaddw.u16 q13, q13, d18		@+64
	
	vsub.s32 q10, q10, q14		@
	vsub.s32 q11, q11, q15		@
	vsub.s32 q12, q12, q7		@
	vsub.s32 q13, q13, q8		@				
               
	vqshrun.s32 d20, q10, #7
	vqshrun.s32 d21, q11, #7	
	vqshrun.s32 d22, q12, #7
	vqshrun.s32 d23, q13, #7	

	vqmovn.u16	d20, q10
	vqmovn.u16	d22, q11
	
	vst1.64	{d20}, [r1], r3 
	vst1.64	{d22}, [r1], r3	

	vld1.64 {q3}, [sp]!		@
	vld1.64 {q4}, [sp]!		@			
@line 5-6	@q5, q1, q2, q3, q4						          
	vmull.u16	q10, d2, d0[2]		@5*lTemp0@
	vmull.u16	q11, d3, d0[2]		@5*lTemp0@
	vmull.u16	q12, d4, d0[2]		@5*lTemp0@
	vmull.u16	q13, d5, d0[2]		@5*lTemp0@	
		
	vmlal.u16	q10, d4, d1[0]		@3*lTemp0@
	vmlal.u16	q11, d5, d1[0]		@3*lTemp0@
	vmlal.u16	q12, d6, d1[0]		@3*lTemp0@
	vmlal.u16	q13, d7, d1[0]		@3*lTemp0@

	vmull.u16	q14, d10, d0[0]		@-5*lTemp0@
	vmull.u16	q15, d11, d0[0]		@-5*lTemp0@
	vmull.u16	q7, d2, d0[0]		@-5*lTemp0@
	vmull.u16	q8, d3, d0[0]		@-5*lTemp0@	
		
	vmlal.u16	q14, d6, d1[2]		@-3*lTemp0@
	vmlal.u16	q15, d7, d1[2]		@-3*lTemp0@
	vmlal.u16	q7, d8, d1[2]		@-3*lTemp0@
	vmlal.u16	q8, d9, d1[2]		@-3*lTemp0@
	
	vaddw.u16 q10, q10, d18		@+64	
	vaddw.u16 q11, q11, d18		@+64
	vaddw.u16 q12, q12, d18		@+64
	vaddw.u16 q13, q13, d18		@+64
	
	vsub.s32 q10, q10, q14		@
	vsub.s32 q11, q11, q15		@
	vsub.s32 q12, q12, q7		@
	vsub.s32 q13, q13, q8		@				
               
	vqshrun.s32 d20, q10, #7
	vqshrun.s32 d21, q11, #7	
	vqshrun.s32 d22, q12, #7
	vqshrun.s32 d23, q13, #7	

	vqmovn.u16	d20, q10
	vqmovn.u16	d22, q11
	
	vst1.64	{d20}, [r1], r3 
	vst1.64	{d22}, [r1], r3	

	vld1.64 {q5}, [sp]!		@
	vld1.64 {q1}, [sp]!		@			
@line 7-8	@q2, q3, q4, q5, q1					          
	vmull.u16	q10, d6, d0[2]		@5*lTemp0@
	vmull.u16	q11, d7, d0[2]		@5*lTemp0@
	vmull.u16	q12, d8, d0[2]		@5*lTemp0@
	vmull.u16	q13, d9, d0[2]		@5*lTemp0@	
		
	vmlal.u16	q10, d8, d1[0]		@3*lTemp0@
	vmlal.u16	q11, d9, d1[0]		@3*lTemp0@
	vmlal.u16	q12, d10, d1[0]		@3*lTemp0@
	vmlal.u16	q13, d11, d1[0]		@3*lTemp0@

	vmull.u16	q14, d4, d0[0]		@-5*lTemp0@
	vmull.u16	q15, d5, d0[0]		@-5*lTemp0@
	vmull.u16	q7, d6, d0[0]		@-5*lTemp0@
	vmull.u16	q8, d7, d0[0]		@-5*lTemp0@	
		
	vmlal.u16	q14, d10, d1[2]		@-3*lTemp0@
	vmlal.u16	q15, d11, d1[2]		@-3*lTemp0@
	vmlal.u16	q7, d2, d1[2]		@-3*lTemp0@
	vmlal.u16	q8, d3, d1[2]		@-3*lTemp0@
	
	vaddw.u16 q10, q10, d18		@+64	
	vaddw.u16 q11, q11, d18		@+64
	vaddw.u16 q12, q12, d18		@+64
	vaddw.u16 q13, q13, d18		@+64
	
	vsub.s32 q10, q10, q14		@
	vsub.s32 q11, q11, q15		@
	vsub.s32 q12, q12, q7		@
	vsub.s32 q13, q13, q8		@				
               
	vqshrun.s32 d20, q10, #7
	vqshrun.s32 d21, q11, #7	
	vqshrun.s32 d22, q12, #7
	vqshrun.s32 d23, q13, #7	

	vqmovn.u16	d20, q10
	vqmovn.u16	d22, q11
	
	vst1.64	{d20}, [r1], r3 
	vst1.64	{d22}, [r1], r3	

	ldmia	sp!, {r4 - r11, pc}
	@ENDP
	@.END	