@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************
    @AREA	|.text|, CODE
 #include "../../defineID.h"  
    .section .text

	.global	DeblockChromaV_ARMV7
	.global	DeblockIntraChromaV_ARMV7
	.global	DeblockChromaH_ARMV7
	.global	DeblockIntraChromaH_ARMV7
	.global	DeblockLumaV_ARMV7
	.global	DeblockIntraLumaV_ARMV7
	.global	DeblockLumaH_ARMV7
	.global	DeblockIntraLumaH_ARMV7
	
	.align 8

DeblockChromaV_ARMV7:

	
 .equ		DCV_OffsetRegSaving, 36
 .equ		DCV_Offset_beta, DCV_OffsetRegSaving + 0
	.equ	DCV_Offset_tc, DCV_OffsetRegSaving + 4
	push     	{r4 - r11, r14}	
	ldr			r4, [sp, #DCV_Offset_beta]
	ldr			r5, [sp, #DCV_Offset_tc]
		
	sub			r8, r0, r2, lsl #1		
	sub			r6, r1, r2, lsl #1		
	vld1.64		{d6}, [r8],	r2			
	vld1.64		{d8}, [r8],	r2			
	vld1.64		{d10}, [r8], r2			
	vld1.64		{d12}, [r8]				
	vld1.64		{d7}, [r6],	r2			
	vld1.64		{d9}, [r6],	r2			
	vld1.64		{d11}, [r6], r2			
	vld1.64		{d13}, [r6]				
	
	vdup.8		q13, r3					
	vabd.u8		q10, q5, q6				
	vdup.8		q12, r4					
	vabd.u8		q11, q4, q3				
		
	
	vclt.u8		q0, q11, q12			
	vclt.u8		q10, q10, q12			
	vabd.u8		q11, q5, q4				
	vand.u8		q0, q0, q10
	vclt.u8		q10, q11, q13			
	vand.u8		q0, q0, q10
	
	ldr			r6, [r5]				
	vdup.32		q15, r6
	vzip.8		d31, d30
	vshr.s64 	d30, d31, #0
	vcgt.s8		q10, q15, #0			
	vand.u8		q0, q0, q10
	
	vmov.i16	q1, #4
	vsubl.u8	q12, d10, d8
	vshl.s16	q12, q12, #2			
	vsubl.u8	q13, d11, d9
	vshl.s16	q13, q13, #2			
	vsubl.u8	q10, d6, d12			
	vsubl.u8	q11, d7, d13			
	vadd.s16	q12, q12, q1			
	vadd.s16	q13, q13, q1			
	vadd.s16	q12, q12, q10			
	vadd.s16	q13, q13, q11			
	vshrn.s16	d28, q12, #3			
	vshrn.s16	d29, q13, #3			
	
	vmin.s8		q14, q15, q14
	vneg.s8		q15, q15
	vmax.s8		q14, q15, q14
	vand.u8		q1, q0, q14
	
	vmovl.s8	q14, d2					
	vmovl.s8	q15, d3					
	vaddw.u8	q14, q14, d8			
	vaddw.u8	q15, q15, d9			
	vqmovun.s16 d8, q14
	vqmovun.s16 d9, q15
	
	vmovl.u8	q14, d10
	vmovl.u8	q15, d11
	sub			r8, r0, r2
	vsubw.s8	q14, q14, d2			
	vsubw.s8	q15, q15, d3			
	sub			r6, r1, r2
	vqmovun.s16 d10, q14
	vqmovun.s16 d11, q15
	
	vst1.64		d8, [r8], r2
	vst1.64		d10, [r8]
	vst1.64		d9, [r6], r2
	vst1.64		d11, [r6]
		
	pop      	{r4 - r11, pc}
	
	
DeblockIntraChromaV_ARMV7:	
	
 .equ		DICV_OffsetRegSaving, 36
 .equ		DICV_Offset_beta, DICV_OffsetRegSaving + 0

	push     	{r4 - r11, r14}	
	ldr			r4, [sp, #DICV_Offset_beta]
		
	sub			r8, r0, r2, lsl #1		@;src1-2*src_stirde
	sub			r6, r1, r2, lsl #1		@;src2-2*src_stirde
	vld1.64		{d6}, [r8],	r2			@;src1 p1
	vld1.64		{d8}, [r8],	r2			@;src1 p0
	vld1.64		{d10}, [r8], r2			@;src1 q0
	vld1.64		{d12}, [r8]				@;src1 q1
	vld1.64		{d7}, [r6],	r2			@;src2 p1
	vld1.64		{d9}, [r6],	r2			@;src2 p0
	vld1.64		{d11}, [r6], r2			@;src2 q0
	vld1.64		{d13}, [r6]				@;src2 q1
	
	vdup.8		q13, r3					@;alpha
	vabd.u8		q10, q5, q6				@;abs(q0-q1)
	vdup.8		q12, r4					@;beta
	vabd.u8		q11, q4, q3				@;abs(p0-p1)
	@;vsub.s8		q14, q5, q4				@;q0-p0
	
	vclt.u8		q0, q11, q12			@;abs(p0-p1) < beta
	vclt.u8		q10, q10, q12			@;abs(q0-q1) < beta
	vabd.u8		q11, q5, q4				@;abs(q0-p0)
	vand.u8		q0, q0, q10
	vclt.u8		q10, q11, q13			@;abs(q0-p0) < alpha
	vand.u8		q0, q0, q10
	
	vaddl.u8	q12, d8, d12			@;p0+q1 src1
	vaddl.u8	q13, d9, d13			@;p0+q1 src2
	vshll.u8	q14, d6, #1				@;p1<<1 src1
	vshll.u8	q15, d7, #1				@;p1<<1 src2
	
	vshll.u8	q10, d12, #1			@;q1<<1 src1
	vadd.s16	q12, q12, q14			@;p1<<1+p0+q1 src1
	vadd.s16	q13, q13, q15			@;p1<<1+p0+q1 src2
	vshll.u8	q15, d13, #1			@;q1<<1 src2	
	
	vrshrn.s16	d28, q12, #2			@;(p1<<1+p0+q1+2)>>2 src1
	vaddl.u8	q12, d10, d6			@;q0+p1 src1
	vrshrn.s16	d29, q13, #2			@;(p1<<1+p0+q1+2)>>2 src2
	vaddl.u8	q13, d11, d7			@;q0+p1 src2
	vadd.s16	q12, q12, q10			@;q1<<1+q0+p1 src1
	vadd.s16	q13, q13, q15			@;q1<<1+q0+p1 src2
	vceq.u8		q10, q0, #0
	vrshrn.s16	d30, q12, #2			@;(q1<<1+q0+p1+2)>>2 src1
	
	vand.s8		q12, q14, q0
	vrshrn.s16	d31, q13, #2			@;(q1<<1+q0+p1+2)>>2 src2
	vand.s8		q13, q4, q10			
	sub			r8, r0, r2
	vorr.s8		q4, q12, q13			@;new p0
	
	vand.s8		q12, q15, q0
	sub			r6, r1, r2
	vand.s8		q13, q5, q10
	vorr.s8		q5, q12, q13			@;new q0
	
	vst1.64		d8, [r8], r2
	vst1.64		d10, [r8]
	vst1.64		d9, [r6], r2
	vst1.64		d11, [r6]
	
	pop      	{r4 - r11, pc}
	
DeblockChromaH_ARMV7:
@;r0 src1
@;r1 src2
@;r2 src_stride
@;r3 alpha
@;r4 beta
@;r5 tc	
	
 .equ		DCH_OffsetRegSaving, 36
 .equ		DCH_Offset_beta, DCH_OffsetRegSaving + 0
 .equ		DCH_Offset_tc, DCH_OffsetRegSaving + 4
	push     	{r4 - r11, r14}	
	ldr			r4, [sp, #DCH_Offset_beta]
	ldr			r5, [sp, #DCH_Offset_tc]
		
	sub			r8, r0, #2				@;src1-2
	sub			r6, r1, #2				@;src2-2
	vld4.8		{d6[0],d7[0],d8[0],d9[0]}, [r8], r2			@;src1 pix0
	vld4.8		{d6[1],d7[1],d8[1],d9[1]}, [r8], r2			@;src1 pix1
	vld4.8		{d6[2],d7[2],d8[2],d9[2]}, [r8], r2			@;src1 pix2
	vld4.8		{d6[3],d7[3],d8[3],d9[3]}, [r8], r2			@;src1 pix3
	vld4.8		{d6[4],d7[4],d8[4],d9[4]}, [r8], r2			@;src1 pix4
	vld4.8		{d6[5],d7[5],d8[5],d9[5]}, [r8], r2			@;src1 pix5
	vld4.8		{d6[6],d7[6],d8[6],d9[6]}, [r8], r2			@;src1 pix6
	vld4.8		{d6[7],d7[7],d8[7],d9[7]}, [r8]				@;src1 pix7
	
	vld4.8		{d13[0],d14[0],d15[0],d16[0]}, [r6], r2		@;src2 pix0
	vld4.8		{d13[1],d14[1],d15[1],d16[1]}, [r6], r2		@;src2 pix1
	vld4.8		{d13[2],d14[2],d15[2],d16[2]}, [r6], r2		@;src2 pix2
	vld4.8		{d13[3],d14[3],d15[3],d16[3]}, [r6], r2		@;src2 pix3
	vld4.8		{d13[4],d14[4],d15[4],d16[4]}, [r6], r2		@;src2 pix4
	vld4.8		{d13[5],d14[5],d15[5],d16[5]}, [r6], r2		@;src2 pix5
	vld4.8		{d13[6],d14[6],d15[6],d16[6]}, [r6], r2		@;src2 pix6
	vld4.8		{d13[7],d14[7],d15[7],d16[7]}, [r6]			@;src2 pix7
	
	vshr.s32	d10, d8, #0
	vshr.s32	d8, d7, #0
	vshr.s32 	d12, d9, #0									 		    

	vshr.s32 	d7, d13, #0
	vshr.s32 	d9, d14, #0
	vshr.s32 	d11, d15, #0
	vshr.s32 	d13, d16, #0
	
	vdup.8		q13, r3					@;alpha
	vabd.u8		q10, q5, q6				@;abs(q0-q1)
	vdup.8		q12, r4					@;beta
	vabd.u8		q11, q4, q3				@;abs(p0-p1)
	@;vsub.s8		q14, q5, q4				@;q0-p0
	
	vclt.u8		q0, q11, q12			@;abs(p0-p1) < beta
	vclt.u8		q10, q10, q12			@;abs(q0-q1) < beta
	vabd.u8		q11, q5, q4				@;abs(q0-p0)
	vand.u8		q0, q0, q10
	vclt.u8		q10, q11, q13			@;abs(q0-p0) < alpha
	vand.u8		q0, q0, q10
	
	ldr			r6, [r5]				@;tc
	vdup.32		q15, r6
	vzip.8		d31, d30
	vshr.s64 	d30, d31, #0
	vcgt.s8		q10, q15, #0			@;tc <= 0
	vand.u8		q0, q0, q10
	
	vmov.i16	q1, #4
	vsubl.u8	q12, d10, d8
	vshl.s16	q12, q12, #2			@;(q0-p0)<<2 src1
	vsubl.u8	q13, d11, d9
	vshl.s16	q13, q13, #2			@;(q0-p0)<<2 src2
	vsubl.u8	q10, d6, d12			@;p1-q1 src1
	vsubl.u8	q11, d7, d13			@;p1-q1 src2
	vadd.s16	q12, q12, q1			@;+4 src1
	vadd.s16	q13, q13, q1			@;+4 src2
	vadd.s16	q12, q12, q10			@;+(p1-q1) src1
	vadd.s16	q13, q13, q11			@;+(p1-q1) src1
	vshrn.s16	d28, q12, #3			@;>>3 src1
	vshrn.s16	d29, q13, #3			@;>>3 src2
	
	vmin.s8		q14, q15, q14
	vneg.s8		q15, q15
	vmax.s8		q14, q15, q14
	vand.u8		q1, q0, q14
	
	vmovl.s8	q14, d2					@;delta src1
	vmovl.s8	q15, d3					@;delta src2
	vaddw.u8	q14, q14, d8			@;p0 + delta src1
	vaddw.u8	q15, q15, d9			@;p0 + delta src2
	vqmovun.s16 d8, q14
	vqmovun.s16 d9, q15
	
	vmovl.u8	q14, d10
	vmovl.u8	q15, d11
	sub			r8, r0, #1
	vsubw.s8	q14, q14, d2			@;q0 - delta src1
	vsubw.s8	q15, q15, d3			@;q0 - delta src2
	sub			r6, r1, #1
	vqmovun.s16 d10, q14
	vqmovun.s16 d11, q15
	vswp.64		d9, d10
	
	vst2.8		{d8[0],d9[0]}, [r8], r2
	vst2.8		{d8[1],d9[1]}, [r8], r2
	vst2.8		{d8[2],d9[2]}, [r8], r2
	vst2.8		{d8[3],d9[3]}, [r8], r2
	vst2.8		{d8[4],d9[4]}, [r8], r2
	vst2.8		{d8[5],d9[5]}, [r8], r2
	vst2.8		{d8[6],d9[6]}, [r8], r2
	vst2.8		{d8[7],d9[7]}, [r8]
		
	vst2.8		{d10[0],d11[0]}, [r6], r2
	vst2.8		{d10[1],d11[1]}, [r6], r2
	vst2.8		{d10[2],d11[2]}, [r6], r2
	vst2.8		{d10[3],d11[3]}, [r6], r2
	vst2.8		{d10[4],d11[4]}, [r6], r2
	vst2.8		{d10[5],d11[5]}, [r6], r2
	vst2.8		{d10[6],d11[6]}, [r6], r2
	vst2.8		{d10[7],d11[7]}, [r6]
		
	pop      	{r4 - r11, pc}
	
DeblockIntraChromaH_ARMV7:
@;r0 src1
@;r1 src2
@;r2 src_stride
@;r3 alpha
@;r4 beta	
	
 .equ		DICH_OffsetRegSaving, 36
 .equ		DICH_Offset_beta, DICH_OffsetRegSaving + 0

	push     	{r4 - r11, r14}	
	ldr			r4, [sp, #DICH_Offset_beta]
		
	sub			r8, r0, #2				@;src1-2
	sub			r6, r1, #2				@;src2-2
	vld4.8		{d6[0],d7[0],d8[0],d9[0]}, [r8], r2			@;src1 pix0
	vld4.8		{d6[1],d7[1],d8[1],d9[1]}, [r8], r2			@;src1 pix1
	vld4.8		{d6[2],d7[2],d8[2],d9[2]}, [r8], r2			@;src1 pix2
	vld4.8		{d6[3],d7[3],d8[3],d9[3]}, [r8], r2			@;src1 pix3
	vld4.8		{d6[4],d7[4],d8[4],d9[4]}, [r8], r2			@;src1 pix4
	vld4.8		{d6[5],d7[5],d8[5],d9[5]}, [r8], r2			@;src1 pix5
	vld4.8		{d6[6],d7[6],d8[6],d9[6]}, [r8], r2			@;src1 pix6
	vld4.8		{d6[7],d7[7],d8[7],d9[7]}, [r8]				@;src1 pix7
	
	vld4.8		{d13[0],d14[0],d15[0],d16[0]}, [r6], r2		@;src2 pix0
	vld4.8		{d13[1],d14[1],d15[1],d16[1]}, [r6], r2		@;src2 pix1
	vld4.8		{d13[2],d14[2],d15[2],d16[2]}, [r6], r2		@;src2 pix2
	vld4.8		{d13[3],d14[3],d15[3],d16[3]}, [r6], r2		@;src2 pix3
	vld4.8		{d13[4],d14[4],d15[4],d16[4]}, [r6], r2		@;src2 pix4
	vld4.8		{d13[5],d14[5],d15[5],d16[5]}, [r6], r2		@;src2 pix5
	vld4.8		{d13[6],d14[6],d15[6],d16[6]}, [r6], r2		@;src2 pix6
	vld4.8		{d13[7],d14[7],d15[7],d16[7]}, [r6]			@;src2 pix7
	
	vshr.s32	d10, d8, #0
	vshr.s32	d8, d7, #0
	vshr.s32 	d12, d9, #0									 		    

	vshr.s32 	d7, d13, #0
	vshr.s32 	d9, d14, #0
	vshr.s32 	d11, d15, #0
	vshr.s32 	d13, d16, #0
	
	vdup.8		q13, r3					@;alpha
	vabd.u8		q10, q5, q6				@;abs(q0-q1)
	vdup.8		q12, r4					@;beta
	vabd.u8		q11, q4, q3				@;abs(p0-p1)
	@;vsub.s8		q14, q5, q4				@;q0-p0
	
	vclt.u8		q0, q11, q12			@;abs(p0-p1) < beta
	vclt.u8		q10, q10, q12			@;abs(q0-q1) < beta
	vabd.u8		q11, q5, q4				@;abs(q0-p0)
	vand.u8		q0, q0, q10
	vclt.u8		q10, q11, q13			@;abs(q0-p0) < alpha
	vand.u8		q0, q0, q10
	
	vaddl.u8	q12, d8, d12			@;p0+q1 src1
	vaddl.u8	q13, d9, d13			@;p0+q1 src2
	vshll.u8	q14, d6, #1				@;p1<<1 src1
	vshll.u8	q15, d7, #1				@;p1<<1 src2
	
	vshll.u8	q10, d12, #1			@;q1<<1 src1
	vadd.s16	q12, q12, q14			@;p1<<1+p0+q1 src1
	vadd.s16	q13, q13, q15			@;p1<<1+p0+q1 src2
	vshll.u8	q15, d13, #1			@;q1<<1 src2	
	
	vrshrn.s16	d28, q12, #2			@;(p1<<1+p0+q1+2)>>2 src1
	vaddl.u8	q12, d10, d6			@;q0+p1 src1
	vrshrn.s16	d29, q13, #2			@;(p1<<1+p0+q1+2)>>2 src2
	vaddl.u8	q13, d11, d7			@;q0+p1 src2
	vadd.s16	q12, q12, q10			@;q1<<1+q0+p1 src1
	vadd.s16	q13, q13, q15			@;q1<<1+q0+p1 src2
	vceq.u8		q10, q0, #0
	vrshrn.s16	d30, q12, #2			@;(q1<<1+q0+p1+2)>>2 src1
	
	vand.s8		q12, q14, q0
	vrshrn.s16	d31, q13, #2			@;(q1<<1+q0+p1+2)>>2 src2
	vand.s8		q13, q4, q10			
	sub			r8, r0, #1
	vorr.s8		q4, q12, q13			@;new p0
	
	vand.s8		q12, q15, q0
	sub			r6, r1, #1
	vand.s8		q13, q5, q10
	vorr.s8		q5, q12, q13			@;new q0
	vswp.64		d9, d10
	
	vst2.8		{d8[0],d9[0]}, [r8], r2
	vst2.8		{d8[1],d9[1]}, [r8], r2
	vst2.8		{d8[2],d9[2]}, [r8], r2
	vst2.8		{d8[3],d9[3]}, [r8], r2
	vst2.8		{d8[4],d9[4]}, [r8], r2
	vst2.8		{d8[5],d9[5]}, [r8], r2
	vst2.8		{d8[6],d9[6]}, [r8], r2
	vst2.8		{d8[7],d9[7]}, [r8]
		
	vst2.8		{d10[0],d11[0]}, [r6], r2
	vst2.8		{d10[1],d11[1]}, [r6], r2
	vst2.8		{d10[2],d11[2]}, [r6], r2
	vst2.8		{d10[3],d11[3]}, [r6], r2
	vst2.8		{d10[4],d11[4]}, [r6], r2
	vst2.8		{d10[5],d11[5]}, [r6], r2
	vst2.8		{d10[6],d11[6]}, [r6], r2
	vst2.8		{d10[7],d11[7]}, [r6]
	
	pop      	{r4 - r11, pc}
	
DeblockLumaV_ARMV7:
@;r0 src1
@;r1 src_stride
@;r2 alpha
@;r3 beta
@;r4 tc	
	
 .equ 	DLV_OffsetRegSaving, 36
 .equ		DLV_Offset_tc, DLV_OffsetRegSaving + 0
	push     	{r4 - r11, r14}	
	ldr			r4, [sp, #DLV_Offset_tc]			@;tc
	
	sub			r8, r0, r1, lsl #2
	vld1.64		{q1}, [r8], r1						@;p3
	vld1.64		{q2}, [r8], r1						@;p2
	vld1.64		{q3}, [r8], r1						@;p1
	vld1.64		{q4}, [r8], r1						@;p0
	vld1.64		{q5}, [r8], r1						@;q0
	vld1.64		{q6}, [r8], r1						@;q1
	vld1.64		{q7}, [r8], r1						@;q2
	vld1.64		{q8}, [r8]	 						@;q3
	
	vabd.u8		q10, q5, q6							@;abs(q1-q0)
	vdup.8		q12, r3								@;beta
	vabd.u8		q11, q4, q3							@;abs(p1-p0)
	vdup.8		q13, r2								@;alpha
	@;vsub.s8		q14, q5, q4							@;p0-q0
	
	vclt.u8		q0, q10, q12						@;abs(q1-q0)<beta
	vclt.u8		q10, q11, q12						@;abs(p1-p0)<beta
	vabd.u8		q11, q5 , q4						@;abs(p0-q0)
	
	vclt.u8		q15, q11, q13						@;abs(p0-q0)<alpha
	vand.u8		q0, q0, q10
	vabd.u8		q13, q4, q2							@;abs(p2-p0)
	vand.u8		q0, q0, q15
	
	ldr			r6, [r4]				@;tc
	vdup.32		q15, r6
	vzip.8		d31, d30
	vshr.s32 	d30, d31, #0
	vzip.16		d30, d31
	vcge.s8		q10, q15, #0			@;tc < 0
	vand.u8		q0, q0, q10
	
	vabd.u8		q9, q5, q7							@;abs(q2-q0)
	vclt.u8		q10, q13, q12						@;abs(p2-p0)<beta
	vsubl.u8	q13, d11, d9						@;p0-q0 8-15
	vshl.s16	q13, q13, #2						@;(p0-q0)<<2 8-15
	vclt.u8		q9, q9, q12							@;abs(q2-q0)<beta
	
	vsubl.u8	q12, d10, d8						@;p0-q0 0-7
	vshl.s16	q12, q12, #2						@;(p0-q0)<<2 0-7
	vmov.i8		d2, #4
	vsubl.u8	q8, d7, d13							@;p1-q1 8-15
	vaddw.s8	q12, q12, d2
	vaddw.s8    q13, q13, d2
	vsubl.u8	q1, d6, d12							@;p1-q1 0-7
	vadd.s16	q1, q1, q12							@;(p0-q0)<<2+(p1-q1)+4 0-7
	vadd.s16	q8, q8, q13							@;(p0-q0)<<2+(p1-q1)+4 8-15
	
	vshrn.s16	d28, q1, #3							@;((p0-q0)<<2+(p1-q1)+4)>>3 0-7
	vabs.s8		q1, q9
	vshrn.s16	d29, q8, #3							@;((p0-q0)<<2+(p1-q1)+4)>>3 8-15
	vabs.s8		q8, q10
	
	vadd.s8		q1, q1, q15							@;abs(q2-q0)<beta tc++
	vaddl.u16.u8.u8	q12, d8, d10						@;p0+q0 0-7
	vadd.s8		q1, q1, q8							@;abs(p2-p0)<beta tc++
	vaddl.u16.u8.u8	q13, d9, d11						@;p0+q0 8-15
	
	vneg.s8		q8, q1
	vmin.s8		q14, q14, q1
	vmax.s8		q14, q14, q8
	vshr.s32	q8, q15, #0							@;tc
	vand.u8		q1, q14, q0							@;delta
	
	vmovl.s8	q14, d2								@;delta 0-7
	vmovl.s8	q15, d3								@;delta 8-15
	vaddw.u8	q14, q14, d8						@;p0+delta 0-7
	vqmovun.s16	d8, q14
	vaddw.u8	q15, q15, d9						@;p0+delta 8-15
	vmovl.u8	q14, d10							@;q0	0-7
	vqmovun.s16 d9, q15	
	
	vmovl.u8	q15, d11							@;q0 8-15
	vsubw.s8	q14, q14, d2						@;q0-delta 0-7
	vsubw.s8	q15, q15, d3						@;q0-delta 8-15
	vmov.i16	q1, #1
	vmovl.u8	q11, d4								@;p2 0-7
	vqmovun.s16	d10, q14
	vqmovun.s16 d11, q15
	
	vmovl.u8	q15, d5								@;p2 8-15
	vhadd.s16	q12, q12, q1						@;(p0+q0+1)>>1 0-7
	vhadd.s16	q13, q13, q1						@;(p0+q0+1)>>1 8-15
	
	vhadd.s16	q14, q12, q11						@;(p2+(p0+q0+1)>>1)>>1 0-7
	vhadd.s16	q15, q13, q15						@;(p2+(p0+q0+1)>>1)>>1 8-15
	vsubw.s8	q14, q14, d6						@;(p2+(p0+q0+1)>>1)>>1-p1 0-7
	vsubw.s8	q15, q15, d7						@;(p2+(p0+q0+1)>>1)>>1-p1 8-15
	vmovn.s16	d22, q14		
	vmovn.s16	d23, q15	
	
	vneg.s8		q15, q8
	vmin.s8		q11, q8, q11
	vmax.s8		q11, q15, q11
	vmovl.u8	q15, d15							@;q2 8-15
	vand.u8		q11, q11, q0
	vand.u8		q11, q11, q10
	vadd.u8		q3, q3, q11							@;p1+clip
	
	vmovl.u8	q11,d14								@;q2 0-7
	vhadd.s16	q15, q13, q15						@;(q2+(p0+q0+1)>>1)>>1 8-15
	vhadd.s16	q14, q12, q11						@;(q2+(p0+q0+1)>>1)>>1 0-7					
	
	vsubw.s8	q15, q15, d13						@;(q2+(p0+q0+1)>>1)>>1-q1 8-15
	vsubw.s8	q14, q14, d12						@;(q2+(p0+q0+1)>>1)>>1-q1 0-7
	vmovn.s16	d22, q14
	vmovn.s16	d23, q15
	
	vneg.s8		q1, q8
	vmin.s8		q11, q8, q11
	vand.u8		q9, q9, q0
	sub			r8, r0, r1, lsl #1
	vmax.s8		q11, q1, q11
	vand.u8		q11, q11, q9
	
	vst1.64		{q3}, [r8], r1
	vadd.u8		q6, q6, q11								@;q1+clip
	
	vst1.64		{q4}, [r8], r1
	vst1.64		{q5}, [r8], r1
	vst1.64		{q6}, [r8]
		
	pop      	{r4 - r11, pc}

DeblockIntraLumaV_ARMV7:	
@;r0 src1
@;r1 src_stride
@;r2 alpha
@;r3 beta	
	
	push     	{r4 - r11, r14}	
	
	sub			r8, r0, r1, lsl #2
	vld1.64		{q1}, [r8], r1						@;p3
	vld1.64		{q2}, [r8], r1						@;p2
	vld1.64		{q3}, [r8], r1						@;p1
	vld1.64		{q4}, [r8], r1						@;p0
	vld1.64		{q5}, [r8], r1						@;q0
	vld1.64		{q6}, [r8], r1						@;q1
	vld1.64		{q7}, [r8], r1						@;q2
	vld1.64		{q8}, [r8]	 						@;q3
	
	vabd.u8		q10, q5, q6							@;abs(q1-q0)
	vdup.8		q12, r3								@;beta
	vabd.u8		q11, q4, q3							@;abs(p1-p0)
	vdup.8		q13, r2								@;alpha
	;vsub.s8		q14, q5, q4							@;p0-q0
	
	vclt.u8		q0, q10, q12						@;abs(q1-q0)<beta
	vclt.u8		q10, q11, q12						@;abs(p1-p0)<beta
	vabd.u8		q11, q5 , q4						@;abs(p0-q0)
	
	vclt.u8		q15, q11, q13						@;abs(p0-q0)<alpha
	vand.u8		q0, q0, q10
	vabd.u8		q13, q4, q2							@;abs(p2-p0)
	vand.u8		q0, q0, q15
	
	mov			r8, #2
	add			r8, r8, r2, lsr #2					@;(alpha >> 2) + 2
	vabd.u8		q9, q5, q7							@;abs(q2-q0)
	vclt.u8		q10, q13, q12						@;abs(p2-p0)<beta
	vclt.u8		q9, q9, q12							@;abs(q2-q0)<beta
	
	vdup.8		q15, r8
	
	vaddl.u8	q12, d8, d10						@;p0+q0 0-7
	vaddl.u8	q13, d9, d11						@;p0+q0 8-15
	
	vclt.u8		q11, q11, q15						@;abs(p0-q0)<((alpha >> 2)+2)
	vmov.i8		d30, #4
	vaddw.u8	q12, q12, d12						@;p0+q0+q1 0-7
	vaddw.u8	q13, q13, d13						@;p0+q0+q1 8-15
	vand.s8		q9, q11, q9							@;abs(q2-q0)<beta && abs(p0-q0)<((alpha >> 2)+2)
	vand.s8		q10, q11, q10						@;abs(p2-p0)<beta && abs(p0-q0)<((alpha >> 2)+2)
	
	@;pix[0*xstride] = ( p1 + 2*p0 + 2*q0 + 2*q1 + q2 + 4 ) >> 3
	vshl.s16	q12, #1								@;(p0+q0+q1)<<1 0-7
	vshl.s16	q13, #1								@;(p0+q0+q1)<<1 8-15
	vaddw.s8	q12, q12, d30						@;((p0+q0+q1)<<1)+4 0-7
	vaddw.s8	q13, q13, d30						@;((p0+q0+q1)<<1)+4 8-15
	
	vmov.i8		d28, #2
	
	vaddw.u8	q12, q12, d14						@;((p0+q0+q1)<<1)+q2+4 0-7
	vaddw.u8	q13, q13, d15						@;((p0+q0+q1)<<1)+q2+4 8-15
	vaddw.u8	q12, q12, d6						@;((p0+q0+q1)<<1)+p1+q2+4 0-7
	vaddw.u8	q13, q13, d7						@;((p0+q0+q1)<<1)+p1+q2+4 8-15
	
	vshrn.s16	d22, q12, #3						@;(((p0+q0+q1)<<1)+p1+q2+4)>>3 0-7
	vshll.u8	q12, d12, #1						@;2*q1 0-7
	vshrn.s16	d23, q13, #3						@;(((p0+q0+q1)<<1)+p1+q2+4)>>3 8-15
	vshll.u8	q13, d13, #1						@;2*q1 8-15
	
	@;pix[ 0*xstride] = ( 2*q1 + q0 + p1 + 2 ) >> 2
	vaddw.s8	q12, q12, d28						@;2*q1+2 0-7
	vaddw.s8	q13, q13, d28						@;2*q1+2 8-15
	vaddw.u8	q12, q12, d10						@;2*q1+q0+2 0-7
	vaddw.u8	q13, q13, d11						@;2*q1+q0+2 8-15
	vaddw.u8	q12, q12, d6						@;2*q1+q0+p1+2 0-7
	vaddw.u8	q13, q13, d7						@;2*q1+q0+p1+2 8-15
	
	vand.s8		q14, q11, q9
	vshrn.s16	d30, q12, #2						@;(2*q1+q0+p1+2)>>2 0-7
	vceq.u8		q12, q9, #0
	vshrn.s16	d31, q13, #2						@;(2*q1+q0+p1+2)>>2 8-15
	vaddl.u8	q13, d9, d11						@;p0+q0 8-15
	vand.s8		q15, q12, q15
	vorr.s8		q14, q15, q14
	vaddl.u8	q12, d8, d10						@;p0+q0 0-7
	vand.s8		q15, q14, q0
	
	vceq.u8		q14, q0, #0
	vaddw.u8	q13, q13, d13						@;p0+q0+q1 8-15
	vand.s8		q14, q5, q14
	vaddw.u8	q12, q12, d12						@;p0+q0+q1 0-7
	vorr.s8		q11, q15, q14		
	
	@;pix[1*xstride] = ( p0 + q0 + q1 + q2 + 2 ) >> 2
	@;vmov.i8		d30, #2
	vaddw.u8	q12, q12, d14						@;p0+q0+q1+q2 0-7
	vaddw.u8	q13, q13, d15						@;p0+q0+q1+q2 8-15
	@;vaddw.s8	q12, q12, d30						;p0+q0+q1+q2+2 0-7
	@;vaddw.s8	q13, q13, d30						;p0+q0+q1+q2+2 8-15
	vrshrn.s16	d28, q12, #2						@;(p0+q0+q1+q2+2)>>2 0-7
	vrshrn.s16	d29, q13, #2						@;(p0+q0+q1+q2+2)>>2 8-15
	
	@;pix[2*xstride] = ( 2*q3 + 3*q2 + q1 + q0 + p0 + 4 ) >> 3
	vshll.u8	q15, d16, #1						@;2*q3 0-7
	vadd.s16	q12, q12, q15						@;2*q3+p0+q0+q1+q2 0-7
	vshll.u8	q15, d17, #1						@;2*q3 8-15
	vadd.s16	q13, q13, q15						@;2*q3+p0+q0+q1+q2 8-15
	vshll.u8	q15, d14, #1						@;2*q2 0-7
	vadd.s16	q12, q12, q15						@;2*q3+p0+q0+q1+3*q2 0-7
	vshll.u8	q15, d15, #1						@;2*q2 8-15
	vadd.s16	q13, q13, q15						@;2*q3+p0+q0+q1+3*q2 8-15
	
	vand.s8		q9, q9, q0
	vrshrn.s16	d30, q12, #3						@;(2*q3+p0+q0+q1+3*q2+4)>>3 0-7	
	vrshrn.s16	d31, q13, #3						@;(2*q3+p0+q0+q1+3*q2+4)>>3 8-15
	
	vand.s8		q14, q14, q9
	vand.s8		q15, q15, q9
	vceq.u8		q9, q9, #0
	
	vand.s8		q12, q6, q9
	vand.s8		q13, q7, q9
	vorr.s8		q8, q12, q14
	vorr.s8		q9, q13, q15
	
	@;pix[-1*xstride] = ( p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4 ) >> 3
	vaddl.u8	q12, d8, d10						@;p0+q0 0-7
	vaddl.u8	q13, d9, d11						@;p0+q0 8-15
	vaddw.u8	q12, q12, d6						@;p0+q0+p1 0-7
	vaddw.u8	q13, q13, d7						@;p0+q0+p1 8-15
	vshl.s16	q12, #1								@;2(p0+q0+p1) 0-7
	vshl.s16	q13, #1								@;2(p0+q0+p1) 8-15
	vaddw.u8	q12, q12, d4						@;2(p0+q0+p1)+p2 0-7
	vaddw.u8	q13, q13, d5						@;2(p0+q0+p1)+p2 8-15
	vaddw.u8	q12, q12, d12						@;2(p0+q0+p1)+p2+q1 0-7
	vaddw.u8	q13, q13, d13						@;2(p0+q0+p1)+p2+q1 8-15
	vrshrn.s16	d30, q12, #3						@;(2(p0+q0+p1)+p2+q1+4)>>3 0-7
	vrshrn.s16	d31, q13, #3						@;(2(p0+q0+p1)+p2+q1+4)>>3 8-15
	
	@;pix[-1*xstride] = ( 2*p1 + p0 + q1 + 2 ) >> 2
	vshll.u8	q12, d6, #1							@;2*p1 0-7
	vshll.u8	q13, d7, #1							@;2*p1 8-15
	vaddw.u8	q12, q12, d8						@;2*p1+p0 0-7
	vaddw.u8	q13, q13, d9						@;2*p1+p0 8-15
	vaddw.u8	q12, q12, d12						@;2*p1+p0+q1 0-7
	vaddw.u8	q13, q13, d13						@;2*p1+p0+q1 8-15
	vrshrn.s16	d28, q12, #2						@;(2*p1+p0+q1+2)>>2 0-7
	vrshrn.s16	d29, q13, #2						@;(2*p1+p0+q1+2)>>2 8-15
	
	vand.s8		q15, q15, q10
	vceq.u8		q12, q10, #0
	vceq.u8		q13, q0, #0
	vand.s8		q14, q12, q14
	vand.s8 	q13, q4, q13
	vorr.s8		q14, q15, q14
	vand.s8		q12, q14, q0
	vorr.s8		q15, q12, q13
	
	@;pix[-2*xstride] = ( p2 + p1 + p0 + q0 + 2 ) >> 2
	vaddl.u8	q12, d8, d10						@;p0+q0 0-7
	vaddl.u8	q13, d9, d11						@;p0+q0 8-15
	vaddw.u8	q12, q12, d6						@;p0+q0+p1 0-7
	vaddw.u8	q13, q13, d7						@;p0+q0+p1 8-15
	vaddw.u8	q12, q12, d4						@;p0+q0+p1+p2 0-7
	vaddw.u8	q13, q13, d5						@;p0+q0+p1+p2 8-15
	vrshrn.s16	d28, q12, #2					@	;(p0+q0+p1+p2+2)>>2 0-7
	vrshrn.s16	d29, q13, #2						@;(p0+q0+p1+p2+2)>>2 8-15
	
	@;pix[-3*xstride] = ( 2*p3 + 3*p2 + p1 + p0 + q0 + 4 ) >> 3
	vshr.s32	q5, q11, #0
	
	vshll.u8	q11, d2, #1							@;2*p3 0-7
	vadd.s16	q12, q12, q11						@;2*p3+p0+q0+p1+p2 0-7
	vshll.u8	q11, d3, #1							@;2*p3 8-15
	vadd.s16	q13, q13, q11						@;2*p3+p0+q0+p1+p2 8-15
	vshll.u8	q11, d4, #1							@;2*p2 0-7
	vadd.s16	q12, q12, q11						@;2*p3+p0+q0+p1+3*p2 0-7
	vshll.u8	q11, d5, #1							@;2*p2 8-15
	vadd.s16	q11, q13, q11						@;2*p3+p0+q0+p1+3*p2 8-15
	vrshrn.s16	d26, q12, #3						@;(2*p3+p0+q0+p1+3*p2+4)>>3 0-7
	vrshrn.s16	d27, q11, #3						@;(2*p3+p0+q0+p1+3*p2+4)>>3 8-15
	
	vand.s8		q10, q10, q0
	sub			r8, r0, r1, lsl #1
	vand.s8		q13, q13, q10
	vand.s8		q14, q14, q10
	vceq.u8		q10, q10, #0
	sub			r8, r8, r1
	vand.s8		q11, q2, q10
	vand.s8		q12, q3, q10
	vorr.s8		q2, q11, q13
	vorr.s8		q3, q12, q14
	
	vshr.s32	q4, q15, #0
	vshr.s32	q6, q8, #0
	vshr.s32	q7, q9, #0
	
	vst1.64		{q2}, [r8], r1
	vst1.64		{q3}, [r8], r1
	vst1.64		{q4}, [r8], r1
	vst1.64		{q5}, [r8], r1
	vst1.64		{q6}, [r8], r1
	vst1.64		{q7}, [r8]
		
	pop      	{r4 - r11, pc}
	
DeblockLumaH_ARMV7:
@;r0 src1
@;r1 src_stride
@;r2 alpha
@;r3 beta
@;r4 tc	
	
 .equ		DLH_OffsetRegSaving, 36
 .equ		DLH_Offset_tc, DLH_OffsetRegSaving + 0
	push     	{r4 - r11, r14}	
	ldr			r4, [sp, #DLH_Offset_tc]			@;tc
	
	sub			r8, r0, #4
	mov			r6, r0
	vld4.8		{d2[0],d3[0],d4[0],d5[0]}, [r8], r1
	vld4.8		{d10[0],d11[0],d12[0],d13[0]}, [r6], r1
	vld4.8		{d2[1],d3[1],d4[1],d5[1]}, [r8], r1
	vld4.8		{d10[1],d11[1],d12[1],d13[1]}, [r6], r1
	vld4.8		{d2[2],d3[2],d4[2],d5[2]}, [r8], r1
	vld4.8		{d10[2],d11[2],d12[2],d13[2]}, [r6], r1
	vld4.8		{d2[3],d3[3],d4[3],d5[3]}, [r8], r1
	vld4.8		{d10[3],d11[3],d12[3],d13[3]}, [r6], r1
	vld4.8		{d2[4],d3[4],d4[4],d5[4]}, [r8], r1
	vld4.8		{d10[4],d11[4],d12[4],d13[4]}, [r6], r1
	vld4.8		{d2[5],d3[5],d4[5],d5[5]}, [r8], r1
	vld4.8		{d10[5],d11[5],d12[5],d13[5]}, [r6], r1
	vld4.8		{d2[6],d3[6],d4[6],d5[6]}, [r8], r1
	vld4.8		{d10[6],d11[6],d12[6],d13[6]}, [r6], r1
	vld4.8		{d2[7],d3[7],d4[7],d5[7]}, [r8], r1
	vld4.8		{d10[7],d11[7],d12[7],d13[7]}, [r6], r1
	vld4.8		{d6[0],d7[0],d8[0],d9[0]}, [r8], r1
	vld4.8		{d14[0],d15[0],d16[0],d17[0]}, [r6], r1
	vld4.8		{d6[1],d7[1],d8[1],d9[1]}, [r8], r1
	vld4.8		{d14[1],d15[1],d16[1],d17[1]}, [r6], r1
	vld4.8		{d6[2],d7[2],d8[2],d9[2]}, [r8], r1
	vld4.8		{d14[2],d15[2],d16[2],d17[2]}, [r6], r1
	vld4.8		{d6[3],d7[3],d8[3],d9[3]}, [r8], r1
	vld4.8		{d14[3],d15[3],d16[3],d17[3]}, [r6], r1
	vld4.8		{d6[4],d7[4],d8[4],d9[4]}, [r8], r1
	vld4.8		{d14[4],d15[4],d16[4],d17[4]}, [r6], r1
	vld4.8		{d6[5],d7[5],d8[5],d9[5]}, [r8], r1
	vld4.8		{d14[5],d15[5],d16[5],d17[5]}, [r6], r1
	vld4.8		{d6[6],d7[6],d8[6],d9[6]}, [r8], r1
	vld4.8		{d14[6],d15[6],d16[6],d17[6]}, [r6], r1
	vld4.8		{d6[7],d7[7],d8[7],d9[7]}, [r8]
	vld4.8		{d14[7],d15[7],d16[7],d17[7]}, [r6]
	vswp		d3, d6
	vswp		d5, d8
	vswp		d11, d14
	vswp		d13, d16
	vswp		q2, q3
	vswp		q6, q7
	
	vabd.u8		q10, q5, q6							@;abs(q1-q0)
	vdup.8		q12, r3								@;beta
	vabd.u8		q11, q4, q3							@;abs(p1-p0)
	vdup.8		q13, r2								@;alpha
	;vsub.s8		q14, q5, q4							@;p0-q0
	
	vclt.u8		q0, q10, q12						@;abs(q1-q0)<beta
	vclt.u8		q10, q11, q12						@;abs(p1-p0)<beta
	vabd.u8		q11, q5 , q4						@;abs(p0-q0)
	
	vclt.u8		q15, q11, q13						@;abs(p0-q0)<alpha
	vand.u8		q0, q0, q10
	vabd.u8		q13, q4, q2							@;abs(p2-p0)
	vand.u8		q0, q0, q15
	
	ldr			r6, [r4]				@;tc
	vdup.32		q15, r6
	vzip.8		d31, d30
	vshr.s32 	d30, d31, #0
	vzip.16		d30, d31
	vcge.s8		q10, q15, #0			@;tc < 0
	vand.u8		q0, q0, q10
	
	vabd.u8		q9, q5, q7							@;abs(q2-q0)
	vclt.u8		q10, q13, q12						@;abs(p2-p0)<beta
	vsubl.u8	q13, d11, d9						@;p0-q0 8-15
	vshl.s16	q13, q13, #2						@;(p0-q0)<<2 8-15
	vclt.u8		q9, q9, q12							@;abs(q2-q0)<beta
	
	vsubl.u8	q12, d10, d8						@;p0-q0 0-7
	vshl.s16	q12, q12, #2						@;(p0-q0)<<2 0-7
	vmov.i8		d2, #4
	vsubl.u8	q8, d7, d13							@;p1-q1 8-15
	vaddw.s8	q12, q12, d2
	vaddw.s8    q13, q13, d2
	vsubl.u8	q1, d6, d12							@;p1-q1 0-7
	vadd.s16	q1, q1, q12							@;(p0-q0)<<2+(p1-q1)+4 0-7
	vadd.s16	q8, q8, q13							@;(p0-q0)<<2+(p1-q1)+4 8-15
	
	vshrn.s16	d28, q1, #3							@;((p0-q0)<<2+(p1-q1)+4)>>3 0-7
	vabs.s8		q1, q9
	vshrn.s16	d29, q8, #3							@;((p0-q0)<<2+(p1-q1)+4)>>3 8-15
	vabs.s8		q8, q10
	
	vadd.s8		q1, q1, q15							@;abs(q2-q0)<beta tc++
	vaddl.u16.u8.u8	q12, d8, d10						@;p0+q0 0-7
	vadd.s8		q1, q1, q8							@;abs(p2-p0)<beta tc++
	vaddl.u16.u8.u8	q13, d9, d11						@;p0+q0 8-15
	
	vneg.s8		q8, q1
	vmin.s8		q14, q14, q1
	vmax.s8		q14, q14, q8
	vshr.s32	q8, q15, #0							@;tc
	vand.u8		q1, q14, q0							@;delta
	
	vmovl.s8	q14, d2								@;delta 0-7
	vmovl.s8	q15, d3								@;delta 8-15
	vaddw.u8	q14, q14, d8						@;p0+delta 0-7
	vqmovun.s16	d8, q14
	vaddw.u8	q15, q15, d9						@;p0+delta 8-15
	vmovl.u8	q14, d10							@;q0	0-7
	vqmovun.s16 d9, q15	
	
	vmovl.u8	q15, d11							@;q0 8-15
	vsubw.s8	q14, q14, d2						@;q0-delta 0-7
	vsubw.s8	q15, q15, d3						@;q0-delta 8-15
	vmov.i16	q1, #1
	vmovl.u8	q11, d4								@;p2 0-7
	vqmovun.s16	d10, q14
	vqmovun.s16 d11, q15
	
	vmovl.u8	q15, d5								@;p2 8-15
	vhadd.s16	q12, q12, q1						@;(p0+q0+1)>>1 0-7
	vhadd.s16	q13, q13, q1						@;(p0+q0+1)>>1 8-15
	
	vhadd.s16	q14, q12, q11						@;(p2+(p0+q0+1)>>1)>>1 0-7
	vhadd.s16	q15, q13, q15						@;(p2+(p0+q0+1)>>1)>>1 8-15
	vsubw.s8	q14, q14, d6						@;(p2+(p0+q0+1)>>1)>>1-p1 0-7
	vsubw.s8	q15, q15, d7						@;(p2+(p0+q0+1)>>1)>>1-p1 8-15
	vmovn.s16	d22, q14		
	vmovn.s16	d23, q15	
	
	vneg.s8		q15, q8
	vmin.s8		q11, q8, q11
	vmax.s8		q11, q15, q11
	vmovl.u8	q15, d15							@;q2 8-15
	vand.u8		q11, q11, q0
	vand.u8		q11, q11, q10
	vadd.u8		q3, q3, q11							@;p1+clip
	
	vmovl.u8	q11,d14								@;q2 0-7
	vhadd.s16	q15, q13, q15						@;(q2+(p0+q0+1)>>1)>>1 8-15
	vhadd.s16	q14, q12, q11						@;(q2+(p0+q0+1)>>1)>>1 0-7					
	
	vsubw.s8	q15, q15, d13						@;(q2+(p0+q0+1)>>1)>>1-q1 8-15
	vsubw.s8	q14, q14, d12						@;(q2+(p0+q0+1)>>1)>>1-q1 0-7
	vmovn.s16	d22, q14
	vmovn.s16	d23, q15
	
	vneg.s8		q1, q8
	vmin.s8		q11, q8, q11
	vand.u8		q9, q9, q0
	sub			r8, r0, #2
	vmax.s8		q11, q1, q11
	vand.u8		q11, q11, q9
	vadd.u8		q6, q6, q11								@;q1+clip
	
	vswp		d7, d8
	vswp		d11, d12
	vswp		q4, q5
	vst4.8		{d6[0],d7[0],d8[0],d9[0]}, [r8], r1
	vst4.8		{d6[1],d7[1],d8[1],d9[1]}, [r8], r1
	vst4.8		{d6[2],d7[2],d8[2],d9[2]}, [r8], r1
	vst4.8		{d6[3],d7[3],d8[3],d9[3]}, [r8], r1
	vst4.8		{d6[4],d7[4],d8[4],d9[4]}, [r8], r1
	vst4.8		{d6[5],d7[5],d8[5],d9[5]}, [r8], r1
	vst4.8		{d6[6],d7[6],d8[6],d9[6]}, [r8], r1
	vst4.8		{d6[7],d7[7],d8[7],d9[7]}, [r8], r1
	vst4.8		{d10[0],d11[0],d12[0],d13[0]}, [r8], r1
	vst4.8		{d10[1],d11[1],d12[1],d13[1]}, [r8], r1
	vst4.8		{d10[2],d11[2],d12[2],d13[2]}, [r8], r1
	vst4.8		{d10[3],d11[3],d12[3],d13[3]}, [r8], r1
	vst4.8		{d10[4],d11[4],d12[4],d13[4]}, [r8], r1
	vst4.8		{d10[5],d11[5],d12[5],d13[5]}, [r8], r1
	vst4.8		{d10[6],d11[6],d12[6],d13[6]}, [r8], r1
	vst4.8		{d10[7],d11[7],d12[7],d13[7]}, [r8]
		
	pop      	{r4 - r11, pc}
	
DeblockIntraLumaH_ARMV7:	
@;r0 src1
@;r1 src_stride
@;r2 alpha
@;r3 beta	
	
	push     	{r4 - r11, r14}	
	
	sub			r8, r0, #4
	mov			r6, r0
	vld4.8		{d2[0],d3[0],d4[0],d5[0]}, [r8], r1
	vld4.8		{d10[0],d11[0],d12[0],d13[0]}, [r6], r1
	vld4.8		{d2[1],d3[1],d4[1],d5[1]}, [r8], r1
	vld4.8		{d10[1],d11[1],d12[1],d13[1]}, [r6], r1
	vld4.8		{d2[2],d3[2],d4[2],d5[2]}, [r8], r1
	vld4.8		{d10[2],d11[2],d12[2],d13[2]}, [r6], r1
	vld4.8		{d2[3],d3[3],d4[3],d5[3]}, [r8], r1
	vld4.8		{d10[3],d11[3],d12[3],d13[3]}, [r6], r1
	vld4.8		{d2[4],d3[4],d4[4],d5[4]}, [r8], r1
	vld4.8		{d10[4],d11[4],d12[4],d13[4]}, [r6], r1
	vld4.8		{d2[5],d3[5],d4[5],d5[5]}, [r8], r1
	vld4.8		{d10[5],d11[5],d12[5],d13[5]}, [r6], r1
	vld4.8		{d2[6],d3[6],d4[6],d5[6]}, [r8], r1
	vld4.8		{d10[6],d11[6],d12[6],d13[6]}, [r6], r1
	vld4.8		{d2[7],d3[7],d4[7],d5[7]}, [r8], r1
	vld4.8		{d10[7],d11[7],d12[7],d13[7]}, [r6], r1
	vld4.8		{d6[0],d7[0],d8[0],d9[0]}, [r8], r1
	vld4.8		{d14[0],d15[0],d16[0],d17[0]}, [r6], r1
	vld4.8		{d6[1],d7[1],d8[1],d9[1]}, [r8], r1
	vld4.8		{d14[1],d15[1],d16[1],d17[1]}, [r6], r1
	vld4.8		{d6[2],d7[2],d8[2],d9[2]}, [r8], r1
	vld4.8		{d14[2],d15[2],d16[2],d17[2]}, [r6], r1
	vld4.8		{d6[3],d7[3],d8[3],d9[3]}, [r8], r1
	vld4.8		{d14[3],d15[3],d16[3],d17[3]}, [r6], r1
	vld4.8		{d6[4],d7[4],d8[4],d9[4]}, [r8], r1
	vld4.8		{d14[4],d15[4],d16[4],d17[4]}, [r6], r1
	vld4.8		{d6[5],d7[5],d8[5],d9[5]}, [r8], r1
	vld4.8		{d14[5],d15[5],d16[5],d17[5]}, [r6], r1
	vld4.8		{d6[6],d7[6],d8[6],d9[6]}, [r8], r1
	vld4.8		{d14[6],d15[6],d16[6],d17[6]}, [r6], r1
	vld4.8		{d6[7],d7[7],d8[7],d9[7]}, [r8]
	vld4.8		{d14[7],d15[7],d16[7],d17[7]}, [r6]
	vswp		d3, d6
	vswp		d5, d8
	vswp		d11, d14
	vswp		d13, d16
	vswp		q2, q3
	vswp		q6, q7	 						
	
	vabd.u8		q10, q5, q6							@;abs(q1-q0)
	vdup.8		q12, r3								@;beta
	vabd.u8		q11, q4, q3							@;abs(p1-p0)
	vdup.8		q13, r2								@;alpha
	@;vsub.s8		q14, q5, q4							@;p0-q0
	
	vclt.u8		q0, q10, q12						@;abs(q1-q0)<beta
	vclt.u8		q10, q11, q12						@;abs(p1-p0)<beta
	vabd.u8		q11, q5 , q4						@;abs(p0-q0)
	
	vclt.u8		q15, q11, q13						@;abs(p0-q0)<alpha
	vand.u8		q0, q0, q10
	vabd.u8		q13, q4, q2							@;abs(p2-p0)
	vand.u8		q0, q0, q15
	
	mov			r8, #2
	add			r8, r8, r2, lsr #2					@;(alpha >> 2) + 2
	vabd.u8		q9, q5, q7							@;abs(q2-q0)
	vclt.u8		q10, q13, q12						@;abs(p2-p0)<beta
	vclt.u8		q9, q9, q12							@;abs(q2-q0)<beta
	
	vdup.8		q15, r8
	
	vaddl.u8	q12, d8, d10						@;p0+q0 0-7
	vaddl.u8	q13, d9, d11						@;p0+q0 8-15
	
	vclt.u8		q11, q11, q15						@;abs(p0-q0)<((alpha >> 2)+2)
	vmov.i8		d30, #4
	vaddw.u8	q12, q12, d12						@;p0+q0+q1 0-7
	vaddw.u8	q13, q13, d13						@;p0+q0+q1 8-15
	vand.s8		q9, q11, q9							@;abs(q2-q0)<beta && abs(p0-q0)<((alpha >> 2)+2)
	vand.s8		q10, q11, q10						@;abs(p2-p0)<beta && abs(p0-q0)<((alpha >> 2)+2)
	
	@;pix[0*xstride] = ( p1 + 2*p0 + 2*q0 + 2*q1 + q2 + 4 ) >> 3
	vshl.s16	q12, #1								@;(p0+q0+q1)<<1 0-7
	vshl.s16	q13, #1								@;(p0+q0+q1)<<1 8-15
	vaddw.s8	q12, q12, d30						@;((p0+q0+q1)<<1)+4 0-7
	vaddw.s8	q13, q13, d30						@;((p0+q0+q1)<<1)+4 8-15
	
	vmov.i8		d28, #2
	
	vaddw.u8	q12, q12, d14						@;((p0+q0+q1)<<1)+q2+4 0-7
	vaddw.u8	q13, q13, d15						@;((p0+q0+q1)<<1)+q2+4 8-15
	vaddw.u8	q12, q12, d6						@;((p0+q0+q1)<<1)+p1+q2+4 0-7
	vaddw.u8	q13, q13, d7						@;((p0+q0+q1)<<1)+p1+q2+4 8-15
	
	vshrn.s16	d22, q12, #3						@;(((p0+q0+q1)<<1)+p1+q2+4)>>3 0-7
	vshll.u8	q12, d12, #1						@;2*q1 0-7
	vshrn.s16	d23, q13, #3						@;(((p0+q0+q1)<<1)+p1+q2+4)>>3 8-15
	vshll.u8	q13, d13, #1						@;2*q1 8-15
	
	@;pix[ 0*xstride] = ( 2*q1 + q0 + p1 + 2 ) >> 2
	vaddw.s8	q12, q12, d28						@;2*q1+2 0-7
	vaddw.s8	q13, q13, d28						@;2*q1+2 8-15
	vaddw.u8	q12, q12, d10						@;2*q1+q0+2 0-7
	vaddw.u8	q13, q13, d11						@;2*q1+q0+2 8-15
	vaddw.u8	q12, q12, d6						@;2*q1+q0+p1+2 0-7
	vaddw.u8	q13, q13, d7						@;2*q1+q0+p1+2 8-15
	
	vand.s8		q14, q11, q9
	vshrn.s16	d30, q12, #2						@;(2*q1+q0+p1+2)>>2 0-7
	vceq.u8		q12, q9, #0
	vshrn.s16	d31, q13, #2						@;(2*q1+q0+p1+2)>>2 8-15
	vaddl.u8	q13, d9, d11						@;p0+q0 8-15
	vand.s8		q15, q12, q15
	vorr.s8		q14, q15, q14
	vaddl.u8	q12, d8, d10						@;p0+q0 0-7
	vand.s8		q15, q14, q0
	
	vceq.u8		q14, q0, #0
	vaddw.u8	q13, q13, d13						@;p0+q0+q1 8-15
	vand.s8		q14, q5, q14
	vaddw.u8	q12, q12, d12						@;p0+q0+q1 0-7
	vorr.s8		q11, q15, q14		
	
	@;pix[1*xstride] = ( p0 + q0 + q1 + q2 + 2 ) >> 2
	@;vmov.i8		d30, #2
	vaddw.u8	q12, q12, d14						@;p0+q0+q1+q2 0-7
	vaddw.u8	q13, q13, d15						@;p0+q0+q1+q2 8-15
	@;vaddw.s8	q12, q12, d30						@;p0+q0+q1+q2+2 0-7
	@;vaddw.s8	q13, q13, d30						;p0+q0+q1+q2+2 8-15
	vrshrn.s16	d28, q12, #2						@;(p0+q0+q1+q2+2)>>2 0-7
	vrshrn.s16	d29, q13, #2						@;(p0+q0+q1+q2+2)>>2 8-15
	
	@;pix[2*xstride] = ( 2*q3 + 3*q2 + q1 + q0 + p0 + 4 ) >> 3
	vshll.u8	q15, d16, #1						@;2*q3 0-7
	vadd.s16	q12, q12, q15						@;2*q3+p0+q0+q1+q2 0-7
	vshll.u8	q15, d17, #1						@;2*q3 8-15
	vadd.s16	q13, q13, q15						@;2*q3+p0+q0+q1+q2 8-15
	vshll.u8	q15, d14, #1						@;2*q2 0-7
	vadd.s16	q12, q12, q15						@;2*q3+p0+q0+q1+3*q2 0-7
	vshll.u8	q15, d15, #1						@;2*q2 8-15
	vadd.s16	q13, q13, q15						@;2*q3+p0+q0+q1+3*q2 8-15
	
	vand.s8		q9, q9, q0
	vrshrn.s16	d30, q12, #3						@;(2*q3+p0+q0+q1+3*q2+4)>>3 0-7	
	vrshrn.s16	d31, q13, #3						@;(2*q3+p0+q0+q1+3*q2+4)>>3 8-15
	
	vand.s8		q14, q14, q9
	vand.s8		q15, q15, q9
	vceq.u8		q9, q9, #0
	
	vand.s8		q12, q6, q9
	vand.s8		q13, q7, q9
	vorr.s8		q8, q12, q14
	vorr.s8		q9, q13, q15
	
	@;pix[-1*xstride] = ( p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4 ) >> 3
	vaddl.u8	q12, d8, d10						@;p0+q0 0-7
	vaddl.u8	q13, d9, d11						@;p0+q0 8-15
	vaddw.u8	q12, q12, d6						@;p0+q0+p1 0-7
	vaddw.u8	q13, q13, d7						@;p0+q0+p1 8-15
	vshl.s16	q12, #1								@;2(p0+q0+p1) 0-7
	vshl.s16	q13, #1								@;2(p0+q0+p1) 8-15
	vaddw.u8	q12, q12, d4						@;2(p0+q0+p1)+p2 0-7
	vaddw.u8	q13, q13, d5						@;2(p0+q0+p1)+p2 8-15
	vaddw.u8	q12, q12, d12						@;2(p0+q0+p1)+p2+q1 0-7
	vaddw.u8	q13, q13, d13						@;2(p0+q0+p1)+p2+q1 8-15
	vrshrn.s16	d30, q12, #3						@;(2(p0+q0+p1)+p2+q1+4)>>3 0-7
	vrshrn.s16	d31, q13, #3						@;(2(p0+q0+p1)+p2+q1+4)>>3 8-15
	
	@;pix[-1*xstride] = ( 2*p1 + p0 + q1 + 2 ) >> 2
	vshll.u8	q12, d6, #1							@;2*p1 0-7
	vshll.u8	q13, d7, #1							@;2*p1 8-15
	vaddw.u8	q12, q12, d8						@;2*p1+p0 0-7
	vaddw.u8	q13, q13, d9						@;2*p1+p0 8-15
	vaddw.u8	q12, q12, d12						@;2*p1+p0+q1 0-7
	vaddw.u8	q13, q13, d13						@;2*p1+p0+q1 8-15
	vrshrn.s16	d28, q12, #2						@;(2*p1+p0+q1+2)>>2 0-7
	vrshrn.s16	d29, q13, #2						@;(2*p1+p0+q1+2)>>2 8-15
	
	vand.s8		q15, q15, q10
	vceq.u8		q12, q10, #0
	vceq.u8		q13, q0, #0
	vand.s8		q14, q12, q14
	vand.s8 	q13, q4, q13
	vorr.s8		q14, q15, q14
	vand.s8		q12, q14, q0
	vorr.s8		q15, q12, q13
	
	@;pix[-2*xstride] = ( p2 + p1 + p0 + q0 + 2 ) >> 2
	vaddl.u8	q12, d8, d10						@;p0+q0 0-7
	vaddl.u8	q13, d9, d11						@;p0+q0 8-15
	vaddw.u8	q12, q12, d6						@;p0+q0+p1 0-7
	vaddw.u8	q13, q13, d7						@;p0+q0+p1 8-15
	vaddw.u8	q12, q12, d4						@;p0+q0+p1+p2 0-7
	vaddw.u8	q13, q13, d5						@;p0+q0+p1+p2 8-15
	vrshrn.s16	d28, q12, #2						@;(p0+q0+p1+p2+2)>>2 0-7
	vrshrn.s16	d29, q13, #2						@;(p0+q0+p1+p2+2)>>2 8-15
	
	@;pix[-3*xstride] = ( 2*p3 + 3*p2 + p1 + p0 + q0 + 4 ) >> 3
	vshr.s32	q5, q11, #0
	
	vshll.u8	q11, d2, #1							@;2*p3 0-7
	vadd.s16	q12, q12, q11						@;2*p3+p0+q0+p1+p2 0-7
	vshll.u8	q11, d3, #1							@;2*p3 8-15
	vadd.s16	q13, q13, q11						@;2*p3+p0+q0+p1+p2 8-15
	vshll.u8	q11, d4, #1							@;2*p2 0-7
	vadd.s16	q12, q12, q11						@;2*p3+p0+q0+p1+3*p2 0-7
	vshll.u8	q11, d5, #1							@;2*p2 8-15
	vadd.s16	q11, q13, q11						@;2*p3+p0+q0+p1+3*p2 8-15
	vrshrn.s16	d26, q12, #3						@;(2*p3+p0+q0+p1+3*p2+4)>>3 0-7
	vrshrn.s16	d27, q11, #3						@;(2*p3+p0+q0+p1+3*p2+4)>>3 8-15
	
	vand.s8		q10, q10, q0
	sub			r8, r0, #3
	vand.s8		q13, q13, q10
	vand.s8		q14, q14, q10
	vceq.u8		q10, q10, #0
	mov			r6, r0
	vand.s8		q11, q2, q10
	vand.s8		q12, q3, q10
	vorr.s8		q2, q11, q13
	vorr.s8		q3, q12, q14
	
	vshr.s32	q4, q15, #0
	vshr.s32	q6, q8, #0
	vshr.s32	q7, q9, #0
	
	vswp		d5, d6
	vswp		d7, d8
	vswp		d7, d6
	vswp		d11, d12
	vswp		d13, d14
	vswp		d12, d13
	vst3.8		{d4[0],d5[0],d6[0]}, [r8], r1
	vst3.8		{d10[0],d11[0],d12[0]}, [r6], r1
	vst3.8		{d4[1],d5[1],d6[1]}, [r8], r1
	vst3.8		{d10[1],d11[1],d12[1]}, [r6], r1
	vst3.8		{d4[2],d5[2],d6[2]}, [r8], r1
	vst3.8		{d10[2],d11[2],d12[2]}, [r6], r1
	vst3.8		{d4[3],d5[3],d6[3]}, [r8], r1
	vst3.8		{d10[3],d11[3],d12[3]}, [r6], r1
	vst3.8		{d4[4],d5[4],d6[4]}, [r8], r1
	vst3.8		{d10[4],d11[4],d12[4]}, [r6], r1
	vst3.8		{d4[5],d5[5],d6[5]}, [r8], r1
	vst3.8		{d10[5],d11[5],d12[5]}, [r6], r1
	vst3.8		{d4[6],d5[6],d6[6]}, [r8], r1
	vst3.8		{d10[6],d11[6],d12[6]}, [r6], r1
	vst3.8		{d4[7],d5[7],d6[7]}, [r8], r1
	vst3.8		{d10[7],d11[7],d12[7]}, [r6], r1
	vst3.8		{d7[0],d8[0],d9[0]}, [r8], r1
	vst3.8		{d13[0],d14[0],d15[0]}, [r6], r1
	vst3.8		{d7[1],d8[1],d9[1]}, [r8], r1
	vst3.8		{d13[1],d14[1],d15[1]}, [r6], r1
	vst3.8		{d7[2],d8[2],d9[2]}, [r8], r1
	vst3.8		{d13[2],d14[2],d15[2]}, [r6], r1
	vst3.8		{d7[3],d8[3],d9[3]}, [r8], r1
	vst3.8		{d13[3],d14[3],d15[3]}, [r6], r1
	vst3.8		{d7[4],d8[4],d9[4]}, [r8], r1
	vst3.8		{d13[4],d14[4],d15[4]}, [r6], r1
	vst3.8		{d7[5],d8[5],d9[5]}, [r8], r1
	vst3.8		{d13[5],d14[5],d15[5]}, [r6], r1
	vst3.8		{d7[6],d8[6],d9[6]}, [r8], r1
	vst3.8		{d13[6],d14[6],d15[6]}, [r6], r1
	vst3.8		{d7[7],d8[7],d9[7]}, [r8]
	vst3.8		{d13[7],d14[7],d15[7]}, [r6]
		
	pop      	{r4 - r11, pc}
		
	.end
	
