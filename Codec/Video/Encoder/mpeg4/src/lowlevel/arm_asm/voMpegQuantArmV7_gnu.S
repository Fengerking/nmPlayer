
@	/************************************************************************
@	*																		*
@	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
@	************************************************************************/
				
	.section .text

	.global	QuantInter_ARMV7	
	.global	QuantIntraH263_ARMV7	
	.global	ExpandByteToShort_ARMV7	
	
	.extern	__voMPEG4E0220	@quant_multi     @extern const unsigned int quant_multi[32]
	
	
.EQU	SCALE_BITS	,16

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@transfer_8to16copy_c(int16_t * const dst,
@					 const uint8_t * const src,
@					 const uint32_t stride)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@	

ExpandByteToShort_ARMV7:

	add			r3, r1, r2, lsl #2
	add			r12, r0, #16*4
	vld1.32		d0, [r1], r2
	vld1.32		d4, [r3], r2
	vld1.32		d1, [r1], r2
	vld1.32		d5, [r3], r2
	vld1.32		d2, [r1], r2
	vld1.32		d6, [r3], r2
	vld1.32		d3, [r1]
	vld1.32		d7, [r3]	
	vmovl.u8	q4, d0
	vmovl.u8	q8, d4
	vst1.32		{q4}, [r0]!
	vmovl.u8	q5, d1
	vst1.32		{q8}, [r12]!
	vmovl.u8	q9, d5
	vst1.32		{q5}, [r0]!
	vmovl.u8	q6, d2
	vst1.32		{q9}, [r12]!
	vmovl.u8	q10, d6
	vst1.32		{q6}, [r0]!
	vmovl.u8	q7, d3
	vst1.32		{q10}, [r12]!
	vmovl.u8	q11, d7	
	vst1.32		{q7}, [r0]
	vst1.32		{q11}, [r12]
	
	mov			pc, lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@quant_h263_intra_c(int16_t * coeff,
@				   const int16_t * data,
@				   const uint32_t quant,
@				   const uint32_t dcscalar)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@	
@r0: coeff
@r1: data		
@r2: quant
@r3: dcscalar
@r4: quant_m_2		
@r5: mult
@q14:temp
@q4 :for VFP DIV instruction


QuantIntraH263_ARMV7:
	stmdb	sp!, {r4-r5, lr}
	
	@row 0-3
	
	ldr			r5, =__voMPEG4E0220		@quant_multi
	ldrh		lr, [r1]
	mov			r4, r2, lsl #1			@quant_m_2 = quant << 1@
	mov			r12, r0
	cmp			lr, #0
	ldr			r5, [r5, r2, lsl #2]	@mult = quant_multi[quant]@	
	suble		lr, lr, r3, asr #1
	addgt		lr, lr, r3, asr #1	
	vld1.32		{q0}, [r1]!
	vld1.32		{q1}, [r1]!
	vld1.32		{q2}, [r1]!
	vld1.32		{q3}, [r1]!
	
	vmov		d8, lr, r3	
	vcvt.f32.s32	s16, s16
	vcvt.f32.s32	s17, s17
	vdup.16		q14, r4
	vdup.16		d10, r5
	vclt.s16	q8, q0, #0		@flag "if(acLevel < 0)"
	vclt.s16	q9, q1, #0
	vabs.s16	q0, q0
	vabs.s16	q1, q1
	@(ABSacLevel * mult) >> SCALEBITS@
	vmull.s16	q12, d0, d10
	vclt.s16	q10, q0, q14	@flag "if(ABSacLevel < quant_m_2)"
	vmull.s16	q13, d1, d10
	vclt.s16	q11, q1, q14
	vmull.s16	q6, d2, d10
	vmov.i32	q15, #0
	vmull.s16	q7, d3, d10
	vdiv.f32	s16, s16, s17	@(a/b)
	vshrn.i32	d24, q12, #SCALE_BITS
	vshrn.i32	d25, q13, #SCALE_BITS
	vshrn.i32	d26, q6, #SCALE_BITS
	vshrn.i32	d27, q7, #SCALE_BITS
	vsub.s16	q5, q15, q12
	vbit		q12, q5, q8
	vbit		q12, q15, q10
	vsub.s16	q5, q15, q13
	vbit		q13, q5, q9
	vbit		q13, q15, q11	
	vst1.32		{q12}, [r0]!
	vdup.16		q14, r4
	vst1.32		{q13}, [r0]!
	
	vclt.s16	q8, q2, #0		@flag "if(acLevel < 0)"
	vclt.s16	q9, q3, #0
	vdup.16		d10, r5
	vabs.s16	q2, q2
	vabs.s16	q3, q3
	@(ABSacLevel * mult) >> SCALEBITS@
	vmull.s16	q12, d4, d10
	vclt.s16	q10, q2, q14	@flag "if(ABSacLevel < quant_m_2)"
	vmull.s16	q13, d5, d10
	vclt.s16	q11, q3, q14
	vmull.s16	q6, d6, d10
	vmov.i32	q15, #0
	vmull.s16	q7, d7, d10
	vshrn.i32	d24, q12, #SCALE_BITS
	
	@row 4-7
	
	vld1.32		{q0}, [r1]!
	vshrn.i32	d25, q13, #SCALE_BITS
	vld1.32		{q1}, [r1]!
	vshrn.i32	d26, q6, #SCALE_BITS
	vld1.32		{q2}, [r1]!
	vshrn.i32	d27, q7, #SCALE_BITS
	vld1.32		{q3}, [r1]
	vsub.s16	q5, q15, q12
	vbit		q12, q5, q8
	vbit		q12, q15, q10
	vsub.s16	q5, q15, q13
	vbit		q13, q5, q9
	vbit		q13, q15, q11	
	vst1.32		{q12}, [r0]!
	vdup.16		q14, r4
	vst1.32		{q13}, [r0]!
		
	vdup.16		d10, r5
	vclt.s16	q8, q0, #0		@flag "if(acLevel < 0)"
	vclt.s16	q9, q1, #0
	vabs.s16	q0, q0
	vabs.s16	q1, q1
	@(ABSacLevel * mult) >> SCALEBITS@
	vmull.s16	q12, d0, d10
	vclt.s16	q10, q0, q14	@flag "if(ABSacLevel < quant_m_2)"
	vmull.s16	q13, d1, d10
	vclt.s16	q11, q1, q14
	vmull.s16	q6, d2, d10
	vmov.i32	q15, #0
	vmull.s16	q7, d3, d10
	vshrn.i32	d24, q12, #SCALE_BITS
	vshrn.i32	d25, q13, #SCALE_BITS
	vshrn.i32	d26, q6, #SCALE_BITS
	vshrn.i32	d27, q7, #SCALE_BITS
	vsub.s16	q5, q15, q12
	vbit		q12, q5, q8
	vbit		q12, q15, q10
	vsub.s16	q5, q15, q13
	vbit		q13, q5, q9
	vbit		q13, q15, q11	
	vst1.32		{q12}, [r0]!
	vdup.16		q14, r4
	vst1.32		{q13}, [r0]!
	
	vclt.s16	q8, q2, #0		@flag "if(acLevel < 0)"
	vclt.s16	q9, q3, #0
	vdup.16		d10, r5
	vabs.s16	q2, q2
	vabs.s16	q3, q3
	@(ABSacLevel * mult) >> SCALEBITS@
	vmull.s16	q12, d4, d10
	vclt.s16	q10, q2, q14	@flag "if(ABSacLevel < quant_m_2)"
	vmull.s16	q13, d5, d10
	vclt.s16	q11, q3, q14
	vmull.s16	q6, d6, d10
	vmov.i32	q15, #0
	vmull.s16	q7, d7, d10
	vcvt.s32.f32	s16, s16
	vshrn.i32	d24, q12, #SCALE_BITS
	vshrn.i32	d25, q13, #SCALE_BITS
	vshrn.i32	d26, q6, #SCALE_BITS
	vshrn.i32	d27, q7, #SCALE_BITS
	vsub.s16	q5, q15, q12
	vbit		q12, q5, q8
	vbit		q12, q15, q10
	vsub.s16	q5, q15, q13
	vbit		q13, q5, q9
	vbit		q13, q15, q11	
	vst1.32		{q12}, [r0]!
	vst1.32		{q13}, [r0]	

	@coeff[0] = CLIP(coeff[0], 1, 254);
	vmov		r3, s16
	cmp			r3, #1	
	movlt		r3, #1
	cmpge		r3, #254
	movge		r3, #254	
	strh		r3, [r12]
	ldmia	sp!, {r4-r5, pc}
	

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@quant_h263_inter_c(signed short * coeff,
@				   const signed short * data,
@				   const unsigned int quant)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@	
@r0: coeff
@r1: data		
@r2: quant
@r3: mult
@r4: quant_m_2		
@r5: quant_d_2
@q15:sum

QuantInter_ARMV7:
	stmdb	sp!, {r4-r5, lr}
	
	@row 0-3
	
	ldr			r3, =__voMPEG4E0220		@quant_multi
	vld1.32		{q0}, [r1]!
	mov			r4, r2, lsl #1			@quant_m_2 = quant << 1@
	vld1.32		{q1}, [r1]!
	mov			r5, r2, lsr #1			@quant_d_2 = quant >> 1@
	vld1.32		{q2}, [r1]!
	ldr			r3, [r3, r2, lsl #2]	@mult = quant_multi[quant]@	
	vld1.32		{q3}, [r1]!
	
	vdup.16		q14, r5
	vdup.16		d10, r3
	vmov.i32	q15, #0
	vclt.s16	q8, q0, #0		@flag "if(acLevel < 0)"
	vclt.s16	q9, q1, #0
	vabs.s16	q0, q0
	vabs.s16	q1, q1
	vsub.u16	q0, q0, q14		@tmp = abs(acLevel) - quant_d_2@
	vsub.u16	q1, q1, q14
	vdup.16		q14, r4
	@(tmp * mult) >> SCALEBITS@
	vmull.s16	q12, d0, d10
	vclt.s16	q10, q0, q14	@flag "if(tmp < quant_m_2)"
	vmull.s16	q13, d1, d10
	vclt.s16	q11, q1, q14
	vmull.s16	q6, d2, d10
	vmov.i32	q4, #0
	vmull.s16	q7, d3, d10
	vshrn.i32	d24, q12, #SCALE_BITS
	vshrn.i32	d25, q13, #SCALE_BITS
	vshrn.i32	d26, q6, #SCALE_BITS
	vshrn.i32	d27, q7, #SCALE_BITS
	vadd.s16	q14, q15, q0	@sum += tmp@
	vbif		q15, q14, q10
	vadd.s16	q14, q15, q1	
	vbif		q15, q14, q11	
	vsub.s16	q5, q4, q12
	vbit		q12, q5, q8
	vbit		q12, q4, q10
	vsub.s16	q5, q4, q13
	vbit		q13, q5, q9
	vbit		q13, q4, q11	
	vst1.32		{q12}, [r0]!
	vdup.16		q14, r5
	vst1.32		{q13}, [r0]!
	
	vclt.s16	q8, q2, #0		@flag "if(acLevel < 0)"
	vclt.s16	q9, q3, #0
	vabs.s16	q2, q2
	vabs.s16	q3, q3
	vsub.u16	q2, q2, q14		@tmp = abs(acLevel) - quant_d_2@
	vsub.u16	q3, q3, q14
	vdup.16		d10, r3
	vdup.16		q14, r4
	@(tmp * mult) >> SCALEBITS@
	vmull.s16	q12, d4, d10
	vclt.s16	q10, q2, q14	@flag "if(tmp < quant_m_2)"
	vmull.s16	q13, d5, d10
	vclt.s16	q11, q3, q14
	vmull.s16	q6, d6, d10
	vmov.i32	q4, #0
	vmull.s16	q7, d7, d10
	vshrn.i32	d24, q12, #SCALE_BITS
	vshrn.i32	d25, q13, #SCALE_BITS
	vshrn.i32	d26, q6, #SCALE_BITS
	vshrn.i32	d27, q7, #SCALE_BITS
	vadd.s16	q14, q15, q2	@sum += tmp@
	vbif		q15, q14, q10
	vadd.s16	q14, q15, q3	
	
	@row 4-7
	
	vld1.32		{q0}, [r1]!
	vbif		q15, q14, q11	
	vld1.32		{q1}, [r1]!
	vsub.s16	q5, q4, q12
	vld1.32		{q2}, [r1]!
	vbit		q12, q5, q8
	vld1.32		{q3}, [r1]
	vbit		q12, q4, q10
	vsub.s16	q5, q4, q13
	vbit		q13, q5, q9
	vbit		q13, q4, q11	
	vst1.32		{q12}, [r0]!
	vdup.16		q14, r5
	vst1.32		{q13}, [r0]!
		
	vdup.16		d10, r3
	vclt.s16	q8, q0, #0		@flag "if(acLevel < 0)"
	vclt.s16	q9, q1, #0
	vabs.s16	q0, q0
	vabs.s16	q1, q1
	vsub.u16	q0, q0, q14		@tmp = abs(acLevel) - quant_d_2@
	vsub.u16	q1, q1, q14
	vdup.16		q14, r4
	@(tmp * mult) >> SCALEBITS@
	vmull.s16	q12, d0, d10
	vclt.s16	q10, q0, q14	@flag "if(tmp < quant_m_2)"
	vmull.s16	q13, d1, d10
	vclt.s16	q11, q1, q14
	vmull.s16	q6, d2, d10
	vmov.i32	q4, #0
	vmull.s16	q7, d3, d10
	vshrn.i32	d24, q12, #SCALE_BITS
	vshrn.i32	d25, q13, #SCALE_BITS
	vshrn.i32	d26, q6, #SCALE_BITS
	vshrn.i32	d27, q7, #SCALE_BITS
	vadd.s16	q14, q15, q0	@sum += tmp@
	vbif		q15, q14, q10
	vadd.s16	q14, q15, q1	
	vbif		q15, q14, q11	
	vsub.s16	q5, q4, q12
	vbit		q12, q5, q8
	vbit		q12, q4, q10
	vsub.s16	q5, q4, q13
	vbit		q13, q5, q9
	vbit		q13, q4, q11	
	vst1.32		{q12}, [r0]!
	vdup.16		q14, r5
	vst1.32		{q13}, [r0]!
	
	vclt.s16	q8, q2, #0		@flag "if(acLevel < 0)"
	vclt.s16	q9, q3, #0
	vabs.s16	q2, q2
	vabs.s16	q3, q3
	vsub.u16	q2, q2, q14		@tmp = abs(acLevel) - quant_d_2@
	vsub.u16	q3, q3, q14
	vdup.16		d10, r3
	vdup.16		q14, r4
	@(tmp * mult) >> SCALEBITS@
	vmull.s16	q12, d4, d10
	vclt.s16	q10, q2, q14	@flag "if(tmp < quant_m_2)"
	vmull.s16	q13, d5, d10
	vclt.s16	q11, q3, q14
	vmull.s16	q6, d6, d10
	vmov.i32	q4, #0
	vmull.s16	q7, d7, d10
	vshrn.i32	d24, q12, #SCALE_BITS
	vshrn.i32	d25, q13, #SCALE_BITS
	vshrn.i32	d26, q6, #SCALE_BITS
	vshrn.i32	d27, q7, #SCALE_BITS
	vadd.s16	q14, q15, q2	@sum += tmp@
	vbif		q15, q14, q10
	vadd.s16	q14, q15, q3	
	vbif		q15, q14, q11	
	vsub.s16	q5, q4, q12
	vbit		q12, q5, q8
	vbit		q12, q4, q10
	vsub.s16	q5, q4, q13
	vbit		q13, q5, q9
	vbit		q13, q4, q11	
	vst1.32		{q12}, [r0]!
	vadd.s16	d0, d30, d31
	vst1.32		{q13}, [r0]
	
	vpaddl.s16	d0, d0
	vmov		r4, r5, d0
	add			r0, r4, r5	
	
	ldmia	sp!, {r4-r5, pc}
	
	.END
