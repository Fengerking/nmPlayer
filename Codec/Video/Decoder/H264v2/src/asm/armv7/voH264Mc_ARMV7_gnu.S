@;*****************************************************************************
@;*																			*
@;*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@;*																			*
@;*****************************************************************************
    @AREA	|.text|, CODE
 #include "../../defineID.h" 
  .section .text

	.global	 MCCopy16x16_ARMV7
	.global	 MCCopy16x8_ARMV7
	.global	 MCCopy8x16_ARMV7
	.global  MCCopy8x8_ARMV7
	.global  MCCopy8x4_ARMV7
	.global  MCCopy4x8_ARMV7
	.global  MCCopy4x4_ARMV7
	.global  MCCopy4x2_ARMV7
	.global  MCCopy2x4_ARMV7
	.global  MCCopy2x2_ARMV7
	.global  get_luma_h_16x16_ARMV7
	.global  get_luma_h_16x8_ARMV7
	.global  get_luma_h_8x16_ARMV7
	.global  get_luma_h_8x8_ARMV7
	.global  get_luma_h_8x4_ARMV7
	.global  get_luma_h_4x8_ARMV7
	.global  get_luma_h_4x4_ARMV7
	.global  avg_get_luma_h_16x16_ARMV7
	.global  avg_get_luma_h_16x8_ARMV7
	.global  avg_get_luma_h_8x16_ARMV7
	.global  avg_get_luma_h_8x8_ARMV7
	.global  avg_get_luma_h_8x4_ARMV7
	.global  avg_get_luma_h_4x8_ARMV7
	.global  avg_get_luma_h_4x4_ARMV7
	.global  get_luma_v_16x16_ARMV7
	.global  get_luma_v_16x8_ARMV7
	.global  get_luma_v_8x16_ARMV7
	.global  get_luma_v_8x8_ARMV7
	.global  get_luma_v_8x4_ARMV7
	.global  get_luma_v_4x8_ARMV7
	.global  get_luma_v_4x4_ARMV7
	.global  avg_get_luma_v_16x16_ARMV7
	.global  avg_get_luma_v_16x8_ARMV7
	.global  avg_get_luma_v_8x16_ARMV7
	.global  avg_get_luma_v_8x8_ARMV7
	.global  avg_get_luma_v_8x4_ARMV7
	.global  avg_get_luma_v_4x8_ARMV7
	.global  avg_get_luma_v_4x4_ARMV7
	.global  get_luma_c_16x16_ARMV7
	.global  get_luma_c_16x8_ARMV7
	.global  get_luma_c_8x16_ARMV7
	.global  get_luma_c_8x8_ARMV7
	.global  get_luma_c_8x4_ARMV7
	.global  get_luma_c_4x8_ARMV7
	.global  get_luma_c_4x4_ARMV7
	.global  add_luma_c_16x16_ARMV7
	.global  add_luma_c_16x8_ARMV7
	.global  add_luma_c_8x16_ARMV7
	.global  add_luma_c_8x8_ARMV7
	.global  add_luma_c_8x4_ARMV7
	.global  add_luma_c_4x8_ARMV7
	.global  add_luma_c_4x4_ARMV7
	.global  avg_dst_luma_h_16x16_ARMV7
	.global  avg_dst_luma_h_16x8_ARMV7
	.global  avg_dst_luma_h_8x16_ARMV7
	.global  avg_dst_luma_h_8x8_ARMV7
	.global  avg_dst_luma_h_8x4_ARMV7
	.global  avg_dst_luma_h_4x8_ARMV7
	.global  avg_dst_luma_h_4x4_ARMV7
	.global  avg_add_luma_h_16x16_ARMV7
	.global  avg_add_luma_h_16x8_ARMV7
	.global  avg_add_luma_h_8x16_ARMV7
	.global  avg_add_luma_h_8x8_ARMV7
	.global  avg_add_luma_h_8x4_ARMV7
	.global  avg_add_luma_h_4x8_ARMV7
	.global  avg_add_luma_h_4x4_ARMV7
	.global  avg_dst_luma_v_16x16_ARMV7
	.global  avg_dst_luma_v_16x8_ARMV7
	.global  avg_dst_luma_v_8x16_ARMV7
	.global  avg_dst_luma_v_8x8_ARMV7
	.global  avg_dst_luma_v_8x4_ARMV7
	.global  avg_dst_luma_v_4x8_ARMV7
	.global  avg_dst_luma_v_4x4_ARMV7
	.global  avg_add_luma_v_16x16_ARMV7
	.global  avg_add_luma_v_16x8_ARMV7
	.global  avg_add_luma_v_8x16_ARMV7
	.global  avg_add_luma_v_8x8_ARMV7
	.global  avg_add_luma_v_8x4_ARMV7
	.global  avg_add_luma_v_4x8_ARMV7
	.global  avg_add_luma_v_4x4_ARMV7
	.global  MCChroma8x8_ARMV7
	.global  MCChroma8x4_ARMV7
	.global  MCChroma4x8_ARMV7
	.global  MCChroma4x4_ARMV7
	.global  MCChroma4x2_ARMV7
	.global  MCChroma2x4_ARMV7
	.global  MCChroma2x2_ARMV7
	.global  ADDMCChroma8x8_ARMV7
	.global  ADDMCChroma8x4_ARMV7
	.global  ADDMCChroma4x8_ARMV7
	.global  ADDMCChroma4x4_ARMV7
	.global  ADDMCChroma4x2_ARMV7
	.global  ADDMCChroma2x4_ARMV7
	.global  ADDMCChroma2x2_ARMV7
	.global  AVG16x16_ARMV7
	.global  AVG16x8_ARMV7
	.global  AVG8x16_ARMV7
	.global  AVG8x8_ARMV7
	.global  AVG8x4_ARMV7
	.global  AVG4x8_ARMV7
	.global  AVG4x4_ARMV7
	.global  AVG4x2_ARMV7
	.global  AVG2x4_ARMV7
	.global  AVG2x2_ARMV7
	.global  offset_mc_prediction_16x16_ARMV7
	.global  offset_mc_prediction_16x8_ARMV7
	.global  offset_mc_prediction_8x16_ARMV7
	.global  offset_mc_prediction_8x8_ARMV7
	.global  offset_mc_prediction_8x4_ARMV7
	.global  offset_mc_prediction_4x8_ARMV7
	.global  offset_mc_prediction_4x4_ARMV7
	.global  offset_mc_prediction_4x2_ARMV7
	.global  offset_mc_prediction_2x4_ARMV7
	.global  offset_mc_prediction_2x2_ARMV7
	.global  weighted_mc_prediction_16x16_ARMV7
	.global  weighted_mc_prediction_16x8_ARMV7
	.global  weighted_mc_prediction_8x16_ARMV7
	.global  weighted_mc_prediction_8x8_ARMV7
	.global  weighted_mc_prediction_8x4_ARMV7
	.global  weighted_mc_prediction_4x8_ARMV7
	.global  weighted_mc_prediction_4x4_ARMV7
	.global  weighted_mc_prediction_4x2_ARMV7
	.global  weighted_bi_prediction_16x16_ARMV7
	.global  weighted_bi_prediction_16x8_ARMV7
	.global  weighted_bi_prediction_8x16_ARMV7
	.global  weighted_bi_prediction_8x8_ARMV7
	.global  weighted_bi_prediction_8x4_ARMV7
	.global  weighted_bi_prediction_4x8_ARMV7
	.global  weighted_bi_prediction_4x4_ARMV7
	.global  weighted_bi_prediction_4x2_ARMV7
	.align 8
	 
	.macro MCChroma8_ROW q0, d0, d1, d2, d3
		vld1.64		{\q0},  [r0], r1
		vext.8      \d1, \d0, \d1, #1
		vmull.u8   q7, \d2, d24
		vmull.u8   q8, \d3, d25
		vmull.u8   q9, \d0, d26
		vmull.u8   q10, \d1, d27
		vadd.u16	q7, q8
		vadd.u16	q7, q9
		vadd.u16	q7, q10
		vqrshrun.s16 	d16,  q7,  #6	@;0-7 solution
		vst1.64     {d16}, [r2],  r3
	.endm

	.macro MCChroma4_ROW q0, d0, d1, d2, d3
		vld1.64		{\q0},  [r0], r1
		vext.8      \d1, \d0, \d1, #1
		vmull.u8   q7, \d2, d24
		vmull.u8   q8, \d3, d25
		vmull.u8   q9, \d0, d26
		vmull.u8   q10, \d1, d27
		vadd.u16	q7, q8
		vadd.u16	q7, q9
		vadd.u16	q7, q10
		vqrshrun.s16 	d16,  q7,  #6	@;0-7 solution
		vst1.32     {d16[0]}, [r2],  r3
	.endm

	.macro ADDMCChroma8_ROW q0, d0, d1, d2, d3
		vld1.64		{\q0},  [r0], r1
		vext.8      \d1, \d0, \d1, #1
		vmull.u8   q7, \d2, d24
		vmull.u8   q8, \d3, d25
		vmull.u8   q9, \d0, d26
		vmull.u8   q10, \d1, d27
		vadd.u16	q7, q8
		vadd.u16	q7, q9
		vadd.u16	q7, q10
		vld1.64     {d17}, [r2]
		vqrshrun.s16 	d16,  q7,  #6	@;0-7 solution
		vrhadd.u8  d16, d16, d17
		vst1.64     {d16}, [r2],  r3
	.endm

	.macro ADDMCChroma4_ROW q0, d0, d1, d2, d3
		vld1.64		{\q0},  [r0], r1
		vext.8      \d1, \d0, \d1, #1
		vmull.u8   q7, \d2, d24
		vmull.u8   q8, \d3, d25
		vmull.u8   q9, \d0, d26
		vmull.u8   q10, \d1, d27
		vadd.u16	q7, q8
		vadd.u16	q7, q9
		vadd.u16	q7, q10
		vld1.32     {d17[0]}, [r2]
		vqrshrun.s16 	d16,  q7,  #6	@;0-7 solution
		vrhadd.u8  d16, d16, d17
		vst1.32     {d16[0]}, [r2],  r3
	.endm
	
	.macro HALF16_H_ROW r, sd
		vld1.64		{q0},  [r0], r1		
		vld1.64		{q13},  [r12], r1
		vext.8		q1,  q0,  q13,  #1	@;b0 ... b15
		vext.8		q2,  q0,  q13,  #2	@;c0 ... c15
		vext.8		q3,  q0,  q13,  #3	@;d0 ... d15
		vext.8		q4,  q0,  q13,  #4	@;e0 ... e15
		vext.8		q5,  q0,  q13,  #5	@;f0 ... f15	
		vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
		vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
		vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7
		vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
		vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
		vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
		vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
		vqrshrun.s16 	d25,  q7,  #5	@;8-15 solution
		vst1.64     {q12}, [\r],  \sd
	.endm
	
	.macro HALF8_H_ROW r, sd
		vld1.64		{q0},  [r0], r1		
		vext.8		d2,  d0,  d1,  #1	@;b0 ... b7
		vext.8		d4,  d0,  d1,  #2	@;c0 ... c7
		vext.8		d6,  d0,  d1,  #3	@;d0 ... d7
		vext.8		d8,  d0,  d1,  #4	@;e0 ... e7
		vext.8		d10,  d0,  d1,  #5	@;f0 ... f7	
		vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
		vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7
		vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
		vst1.64     {d24}, [\r],  \sd
	.endm
	.macro HALF4_H_ROW r, sd
		vld1.64		{q0},  [r0], r1	
		vext.8		d2,  d0,  d1,  #1	@;b0 ... b3
		vext.8		d4,  d0,  d1,  #2	@;c0 ... c3
		vext.8		d6,  d0,  d1,  #3	@;d0 ... d3
		vext.8		d8,  d0,  d1,  #4	@;e0 ... e3
		vext.8		d10,  d0,  d1,  #5	@;f0 ... f3	
		vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
		vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7
		vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
		vst1.32     {d24[0]}, [\r],  \sd
	.endm

	.macro QUARTER16_H_ROW 
		vld1.64		{q0},  [r0], r1		
		vld1.64		{q13},  [r12], r1
		vext.8		q1,  q0,  q13,  #1	@;b0 ... b15
		vext.8		q2,  q0,  q13,  #2	@;c0 ... c15
		vext.8		q3,  q0,  q13,  #3	@;d0 ... d15
		vext.8		q4,  q0,  q13,  #4	@;e0 ... e15
		vext.8		q5,  q0,  q13,  #5	@;f0 ... f15	
		vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
		vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
		vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7
		vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
		vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
		vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
		vld1.64		{q0},  [r4], r5	
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
		vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
		vqrshrun.s16 	d25,  q7,  #5	@;8-15 solution
		vrhadd.u8  q12, q12, q0
		vst1.64     {q12}, [r2],  r3
	.endm

	.macro QUARTER8_H_ROW 
		vld1.64		{q0},  [r0], r1		
		vext.8		d2,  d0,  d1,  #1	@;b0 ... b7
		vext.8		d4,  d0,  d1,  #2	@;c0 ... c7
		vext.8		d6,  d0,  d1,  #3	@;d0 ... d7
		vext.8		d8,  d0,  d1,  #4	@;e0 ... e7
		vext.8		d10,  d0,  d1,  #5	@;f0 ... f7	
		vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
		vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7
		vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
		vld1.64		{d0},  [r4], r5
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
		vrhadd.u8  d24, d24, d0
		vst1.64     {d24}, [r2],  r3
	.endm

	.macro QUARTER4_H_ROW 
		vld1.64		{q0},  [r0], r1		
		vext.8		d2,  d0,  d1,  #1	@;b0 ... b7
		vext.8		d4,  d0,  d1,  #2	@;c0 ... c7
		vext.8		d6,  d0,  d1,  #3	@;d0 ... d7
		vext.8		d8,  d0,  d1,  #4	@;e0 ... e7
		vext.8		d10,  d0,  d1,  #5	@;f0 ... f7	
		vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
		vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7
		vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
		vld1.64		{d0},  [r4], r5
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
		vrhadd.u8  d24, d24, d0
		vst1.32     {d24[0]}, [r2],  r3
	.endm

	.macro AVG16_H_ROW 
		vld1.64		{q0},  [r0], r1		
		vld1.64		{q13},  [r12], r1
		vext.8		q1,  q0,  q13,  #1	@;b0 ... b15
		vext.8		q2,  q0,  q13,  #2	@;c0 ... c15
		vext.8		q3,  q0,  q13,  #3	@;d0 ... d15
		vext.8		q4,  q0,  q13,  #4	@;e0 ... e15
		vext.8		q5,  q0,  q13,  #5	@;f0 ... f15	
		vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
		vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
		vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7
		vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
		vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
		vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
		vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
		vqrshrun.s16 	d25,  q7,  #5	@;8-15 solution
		vld1.64		{q0},  [r2]
		vrhadd.u8  q12, q12, q0
		vst1.64     {q12}, [r2],  r3
	.endm

       .macro AVG8_H_ROW
		vld1.64		{q0},  [r0], r1		
		vext.8		d2,  d0,  d1,  #1	@;b0 ... b7
		vext.8		d4,  d0,  d1,  #2	@;c0 ... c7
		vext.8		d6,  d0,  d1,  #3	@;d0 ... d7
		vext.8		d8,  d0,  d1,  #4	@;e0 ... e7
		vext.8		d10,  d0,  d1,  #5	@;f0 ... f7	
		vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
		vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7
		vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
		vld1.64		{d0},  [r2]
		vrhadd.u8  d24, d24, d0
		vst1.64     {d24}, [r2],  r3
	.endm

	.macro AVG4_H_ROW
		vld1.64		{q0},  [r0], r1		
		vext.8		d2,  d0,  d1,  #1	@;b0 ... b7
		vext.8		d4,  d0,  d1,  #2	@;c0 ... c7
		vext.8		d6,  d0,  d1,  #3	@;d0 ... d7
		vext.8		d8,  d0,  d1,  #4	@;e0 ... e7
		vext.8		d10,  d0,  d1,  #5	@;f0 ... f7	
		vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
		vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7
		vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
		vld1.64		{d0},  [r2]
		vrhadd.u8  d24, d24, d0
		vst1.32     {d24[0]}, [r2],  r3
	.endm

	.macro ADDAVG16_H_ROW 
		vld1.64		{q0},  [r0], r1		
		vld1.64		{q13},  [r12], r1
		vext.8		q1,  q0,  q13,  #1	@;b0 ... b15
		vext.8		q2,  q0,  q13,  #2	@;c0 ... c15
		vext.8		q3,  q0,  q13,  #3	@;d0 ... d15
		vext.8		q4,  q0,  q13,  #4	@;e0 ... e15
		vext.8		q5,  q0,  q13,  #5	@;f0 ... f15	
		vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
		vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
		vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7
		vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
		vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
		vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
		vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
		vqrshrun.s16 	d25,  q7,  #5	@;8-15 solution
		vld1.64		{q0},  [r4],r5
		vrhadd.u8  q12, q12, q0
		vld1.64		{q0},  [r2]
		vrhadd.u8  q12, q12, q0
		vst1.64     {q12}, [r2],  r3
	.endm

       .macro ADDAVG8_H_ROW
		vld1.64		{q0},  [r0], r1		
		vext.8		d2,  d0,  d1,  #1	@;b0 ... b7
		vext.8		d4,  d0,  d1,  #2	@;c0 ... c7
		vext.8		d6,  d0,  d1,  #3	@;d0 ... d7
		vext.8		d8,  d0,  d1,  #4	@;e0 ... e7
		vext.8		d10,  d0,  d1,  #5	@;f0 ... f7	
		vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
		vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7
		vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
		vld1.64		{d0},  [r4], r5
		vrhadd.u8  d24, d24, d0
		vld1.64		{d0},  [r2]
		vrhadd.u8  d24, d24, d0
		vst1.64     {d24}, [r2],  r3
	.endm

       .macro ADDAVG4_H_ROW
		vld1.64		{q0},  [r0], r1		
		vext.8		d2,  d0,  d1,  #1	@;b0 ... b7
		vext.8		d4,  d0,  d1,  #2	@;c0 ... c7
		vext.8		d6,  d0,  d1,  #3	@;d0 ... d7
		vext.8		d8,  d0,  d1,  #4	@;e0 ... e7
		vext.8		d10,  d0,  d1,  #5	@;f0 ... f7	
		vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
		vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7
		vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
		vld1.64		{d0},  [r4], r5
		vrhadd.u8  d24, d24, d0
		vld1.64		{d0},  [r2]
		vrhadd.u8  d24, d24, d0
		vst1.32     {d24[0]}, [r2],  r3
	.endm
	
    .macro HALF16_V_ROW q, r0, r1, sd, d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11
		vld1.64		{\q},  [\r0], r1		@;f0 ... f15
		vaddl.u8	q10,  \d0,  \d1		@;c0+d0 ... c7+d7
		vaddl.u8	q11,  \d2,  \d3		@;c8+d8 ... c15+d15	
		vaddl.u8	q6,  \d4,  \d5		@;a0+f0 ... a7+f7
		vaddl.u8	q7,  \d6,  \d7		@;a8+f8 ... a15+f15
		vaddl.u8	q8,  \d8,  \d9		@;b0+e0 ... b7+e7
		vaddl.u8	q9,  \d10,  \d11	@;b8+e8 ... b15+e15	
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
		vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
		vqrshrun.s16 	d25,  q7,  #5	@;8-15 solution
		vst1.64     {q12}, [\r1],  \sd
	.endm
	.macro HALF8_V_ROW q, r0, r1, sd, d0, d1, d4, d5, d8, d9
		vld1.64		{\q},  [\r0], r1		@f0 ... f15
		vaddl.u8	q10,  \d0,  \d1		@c0+d0 ... c7+d7	
		vaddl.u8	q6,  \d4,  \d5		@a0+f0 ... a7+f7
		vaddl.u8	q8,  \d8,  \d9		@b0+e0 ... b7+e7	
		vmla.s16    q6,  q10,  q15  	@a+20(c+d)+f 0-7
		vmls.s16    q6,  q8,  q14		@a-5(b+e)+20(c+d)+f 0-7
		vqrshrun.s16 	d24,  q6,  #5	@0-7 solution
		vst1.64     {d24}, [\r1],  \sd
	.endm
	.macro HALF4_V_ROW q, r0, r1, sd, d0, d1, d4, d5, d8, d9
		vld1.64		{\q},  [\r0], r1		@f0 ... f15
		vaddl.u8	q10,  \d0,  \d1		@c0+d0 ... c7+d7	
		vaddl.u8	q6,  \d4,  \d5		@a0+f0 ... a7+f7
		vaddl.u8	q8,  \d8,  \d9		@b0+e0 ... b7+e7	
		vmla.s16    q6,  q10,  q15  	@a+20(c+d)+f 0-7
		vmls.s16    q6,  q8,  q14		@a-5(b+e)+20(c+d)+f 0-7
		vqrshrun.s16 	d24,  q6,  #5	@0-7 solution
		vst1.32     {d24[0]}, [\r1],  \sd
	.endm

	.macro QUARTER16_V_ROW q, r0, r1, sd, d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11
		vld1.64		{\q},  [\r0], r1		@;f0 ... f15
		vaddl.u8	q10,  \d0,  \d1		@;c0+d0 ... c7+d7
		vaddl.u8	q11,  \d2,  \d3		@;c8+d8 ... c15+d15	
		vaddl.u8	q6,  \d4,  \d5		@;a0+f0 ... a7+f7
		vaddl.u8	q7,  \d6,  \d7		@;a8+f8 ... a15+f15
		vaddl.u8	q8,  \d8,  \d9		@;b0+e0 ... b7+e7
		vaddl.u8	q9,  \d10,  \d11	@;b8+e8 ... b15+e15	
		vld1.64		{q13},  [r4], r5
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
		vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
		vqrshrun.s16 	d25,  q7,  #5	@;8-15 solution
		vrhadd.u8  q12, q12, q13
		vst1.64     {q12}, [\r1],  \sd
	.endm

	.macro QUARTER8_V_ROW q, r0, r1, sd, d0, d1, d4, d5, d8, d9
		vld1.64		{\q},  [\r0], r1		@;f0 ... f15
		vaddl.u8	q10,  \d0,  \d1		@;c0+d0 ... c7+d7	
		vaddl.u8	q6,  \d4,  \d5		@;a0+f0 ... a7+f7
		vaddl.u8	q8,  \d8,  \d9		@;b0+e0 ... b7+e7	
		vld1.64		{d26},  [r4], r5
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
		vrhadd.u8  d24, d24, d26
		vst1.64     {d24}, [\r1],  \sd
	.endm

	.macro QUARTER4_V_ROW q, r0, r1, sd, d0, d1, d4, d5, d8, d9
		vld1.64		{\q},  [\r0], r1		@;f0 ... f15
		vaddl.u8	q10,  \d0,  \d1		@;c0+d0 ... c7+d7	
		vaddl.u8	q6,  \d4,  \d5		@;a0+f0 ... a7+f7
		vaddl.u8	q8,  \d8,  \d9		@;b0+e0 ... b7+e7	
		vld1.64		{d26},  [r4], r5
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
		vrhadd.u8  d24, d24, d26
		vst1.32     {d24[0]}, [\r1],  \sd
	.endm
	
	.macro AVG16_V_ROW q, r0, r1, sd, d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11
		vld1.64		{\q},  [\r0], r1		@;f0 ... f15
		vaddl.u8	q10,  \d0,  \d1		@;c0+d0 ... c7+d7
		vaddl.u8	q11,  \d2,  \d3		@;c8+d8 ... c15+d15	
		vaddl.u8	q6,  \d4,  \d5		@;a0+f0 ... a7+f7
		vaddl.u8	q7,  \d6,  \d7		@;a8+f8 ... a15+f15
		vaddl.u8	q8,  \d8,  \d9		@;b0+e0 ... b7+e7
		vaddl.u8	q9,  \d10,  \d11	@;b8+e8 ... b15+e15	
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
		vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
		vqrshrun.s16 	d25,  q7,  #5	@;8-15 solution
		vld1.64		{q13},  [r2]
		vrhadd.u8  q12, q12, q13
		vst1.64     {q12}, [\r1],  \sd
	.endm

	.macro AVG8_V_ROW q, r0, r1, sd, d0, d1, d4, d5, d8, d9
		vld1.64		{\q},  [\r0], r1		@;f0 ... f15
		vaddl.u8	q10,  \d0,  \d1		@;c0+d0 ... c7+d7	
		vaddl.u8	q6,  \d4,  \d5		@;a0+f0 ... a7+f7
		vaddl.u8	q8,  \d8,  \d9		@;b0+e0 ... b7+e7	
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
		vld1.64		{d26},  [r2]
		vrhadd.u8  d24, d24, d26
		vst1.64     {d24}, [\r1],  \sd
	.endm

	.macro AVG4_V_ROW q, r0, r1, sd, d0, d1, d4, d5, d8, d9
		vld1.64		{\q},  [\r0], r1		@;f0 ... f15
		vaddl.u8	q10,  \d0,  \d1		@;c0+d0 ... c7+d7	
		vaddl.u8	q6,  \d4,  \d5		@;a0+f0 ... a7+f7
		vaddl.u8	q8,  \d8,  \d9		@;b0+e0 ... b7+e7	
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
		vld1.64		{d26},  [r2]
		vrhadd.u8  d24, d24, d26
		vst1.32     {d24[0]}, [\r1],  \sd
	.endm

	.macro ADDAVG16_V_ROW q, r0, r1, sd, d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11
		vld1.64		{\q},  [\r0], r1		@;f0 ... f15
		vaddl.u8	q10,  \d0,  \d1		@;c0+d0 ... c7+d7
		vaddl.u8	q11,  \d2,  \d3		@;c8+d8 ... c15+d15	
		vaddl.u8	q6,  \d4,  \d5		@;a0+f0 ... a7+f7
		vaddl.u8	q7,  \d6,  \d7		@;a8+f8 ... a15+f15
		vaddl.u8	q8,  \d8,  \d9		@;b0+e0 ... b7+e7
		vaddl.u8	q9,  \d10,  \d11	@;b8+e8 ... b15+e15	
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
		vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
		vqrshrun.s16 	d25,  q7,  #5	@;8-15 solution
		vld1.64		{q13},  [r4], r5
		vrhadd.u8  q12, q12, q13
		vld1.64		{q13},  [r2]
		vrhadd.u8  q12, q12, q13
		vst1.64     {q12}, [\r1],  \sd
	.endm

	.macro ADDAVG8_V_ROW q, r0, r1, sd, d0, d1, d4, d5, d8, d9
		vld1.64		{\q},  [\r0], r1		@;f0 ... f15
		vaddl.u8	q10,  \d0,  \d1		@;c0+d0 ... c7+d7	
		vaddl.u8	q6,  \d4,  \d5		@;a0+f0 ... a7+f7
		vaddl.u8	q8,  \d8,  \d9		@;b0+e0 ... b7+e7	
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
		vld1.64		{d26},  [r4], r5
		vrhadd.u8  d24, d24, d26
		vld1.64		{d26},  [r2]
		vrhadd.u8  d24, d24, d26
		vst1.64     {d24}, [\r1],  \sd
	.endm

	.macro ADDAVG4_V_ROW q, r0, r1, sd, d0, d1, d4, d5, d8, d9
		vld1.64		{\q},  [\r0], r1		@;f0 ... f15
		vaddl.u8	q10,  \d0,  \d1		@;c0+d0 ... c7+d7	
		vaddl.u8	q6,  \d4,  \d5		@;a0+f0 ... a7+f7
		vaddl.u8	q8,  \d8,  \d9		@;b0+e0 ... b7+e7	
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
		vld1.64		{d26},  [r4], r5
		vrhadd.u8  d24, d24, d26
		vld1.64		{d26},  [r2]
		vrhadd.u8  d24, d24, d26
		vst1.32     {d24[0]}, [\r1],  \sd
	.endm
	
	.macro HALF8_C_ROW q0, r0, q1, r1, d0, d1, d2, d3,  d4, d5, d6, d7, d8, d9, d10, d11
		vld1.64		{\q0},  [\r0], r1		@;f0 ... f15
		vaddl.u8	q10,  \d0,  \d1		@;c0+d0 ... c7+d7
		vaddl.u8	q11,  \d2,  \d3		@;c8+d8 ... c15+d15	
		vaddl.u8	q6,  \d4,  \d5		@;a0+f0 ... a7+f7
		vaddl.u8	q7,  \d6,  \d7		@;a8+f8 ... a15+f15
		vaddl.u8	q8,  \d8,  \d9		@;b0+e0 ... b7+e7
		vaddl.u8	q9,  \d10,  \d11		@;b8+e8 ... b15+e15	
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
		vext.8		q8,  q6,  q7,  #2	@;b0 ... b7 
		vext.8		q9,  q6,  q7,  #4	@;c0 ... c7
		vext.8		q10,  q6,  q7,  #6	@;d0 ... d7 
		vext.8		q11,  q6,  q7,  #8	@;e0 ... e7	
		vext.8		q12,  q6,  q7,  #10	@;f0 ... f7 
		vaddl.s16	q7,  d12,  d24		@;a0+f0 ... a3+f3
		vaddl.s16	\q1,  d13,  d25		@;a4+f4 ... a7+f7
		vaddl.s16	q6,  d16,  d22		@;b0+e0 ... b3+e3
		vaddl.s16	q12,  d17,  d23		@;b4+e4 ... b7+e7
		vaddl.s16	q8,  d18,  d20		@;c0+d0 ... c3+d3
		vaddl.s16	q11,  d19,  d21		@;c4+d4 ... c7+d7
		vmov.i32    q9,  #5				@;coeff 5
		vmov.i32    q10,  #20			@;coeff 20	
		vmla.s32    q7,  q8,  q10  		@;a+20(c+d)+f 0-3
		vmla.s32    \q1,  q11,  q10		@;a+20(c+d)+f 4-8
		vmls.s32    q7,  q6,  q9		@;a-5(b+e)+20(c+d)+f 0-3
		vmls.s32    \q1,  q12,  q9		@;a-5(b+e)+20(c+d)+f 4-8
		vqrshrun.s32 	d20,  q7,  #10	@;0-7 solution
		vqrshrun.s32 	d21,  \q1,  #10	@;8-15 solution
		vqmovn		d24.u8,  q10.u16	@;0-7 solution
		vst1.64     {d24}, [\r1],  r3
	.endm
	
    .macro HALF4_C_ROW q0, r0, q1, r1, d0, d1, d2, d3,  d4, d5, d6, d7, d8, d9, d10, d11
		vld1.64		{\q0},  [\r0], r1		@;f0 ... f15
		vaddl.u8	q10,  \d0,  \d1		@;c0+d0 ... c7+d7
		vaddl.u8	q11,  \d2,  \d3		@;c8+d8 ... c15+d15	
		vaddl.u8	q6,  \d4,  \d5		@;a0+f0 ... a7+f7
		vaddl.u8	q7,  \d6,  \d7		@;a8+f8 ... a15+f15
		vaddl.u8	q8,  \d8,  \d9		@;b0+e0 ... b7+e7
		vaddl.u8	q9,  \d10,  \d11		@;b8+e8 ... b15+e15	
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
		vext.8		q8,  q6,  q7,  #2	@;b0 ... b7 
		vext.8		q9,  q6,  q7,  #4	@;c0 ... c7
		vext.8		q10,  q6,  q7,  #6	@;d0 ... d7 
		vext.8		q11,  q6,  q7,  #8	@;e0 ... e7	
		vext.8		q12,  q6,  q7,  #10	@;f0 ... f7 
		vaddl.s16	q7,  d12,  d24		@;a0+f0 ... a3+f3
		vaddl.s16	q6,  d16,  d22		@;b0+e0 ... b3+e3
		vaddl.s16	q8,  d18,  d20		@;c0+d0 ... c3+d3
		vmov.i32    q9,  #5				@;coeff 5
		vmov.i32    q10,  #20			@;coeff 20	
		vmla.s32    q7,  q8,  q10  		@;a+20(c+d)+f 0-3
		vmls.s32    q7,  q6,  q9		@;a-5(b+e)+20(c+d)+f 0-3
		vqrshrun.s32 	d20,  q7,  #10	@;0-7 solution
		vqmovn		d24.u8,  q10.u16	@;0-7 solution
		vst1.32     {d24[0]}, [\r1],  r3
	.endm

	.macro ADDHALF8_C_ROW q0, r0, q1, r1, d0, d1, d2, d3,  d4, d5, d6, d7, d8, d9, d10, d11
		vld1.64		{\q0},  [\r0], r1		@;f0 ... f15
		vaddl.u8	q10,  \d0,  \d1		@;c0+d0 ... c7+d7
		vaddl.u8	q11,  \d2,  \d3		@;c8+d8 ... c15+d15	
		vaddl.u8	q6,  \d4,  \d5		@;a0+f0 ... a7+f7
		vaddl.u8	q7,  \d6,  \d7		@;a8+f8 ... a15+f15
		vaddl.u8	q8,  \d8,  \d9		@;b0+e0 ... b7+e7
		vaddl.u8	q9,  \d10,  \d11		@;b8+e8 ... b15+e15	
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
		vext.8		q8,  q6,  q7,  #2	@;b0 ... b7 
		vext.8		q9,  q6,  q7,  #4	@;c0 ... c7
		vext.8		q10,  q6,  q7,  #6	@;d0 ... d7 
		vext.8		q11,  q6,  q7,  #8	@;e0 ... e7	
		vext.8		q12,  q6,  q7,  #10	@;f0 ... f7 
		vaddl.s16	q7,  d12,  d24		@;a0+f0 ... a3+f3
		vaddl.s16	\q1,  d13,  d25		@;a4+f4 ... a7+f7
		vaddl.s16	q6,  d16,  d22		@;b0+e0 ... b3+e3
		vaddl.s16	q12,  d17,  d23		@;b4+e4 ... b7+e7
		vaddl.s16	q8,  d18,  d20		@;c0+d0 ... c3+d3
		vaddl.s16	q11,  d19,  d21		@;c4+d4 ... c7+d7
		vmov.i32    q9,  #5				@;coeff 5
		vmov.i32    q10,  #20			@;coeff 20	
		vmla.s32    q7,  q8,  q10  		@;a+20(c+d)+f 0-3
		vmla.s32    \q1,  q11,  q10		@;a+20(c+d)+f 4-8
		vmls.s32    q7,  q6,  q9		@;a-5(b+e)+20(c+d)+f 0-3
		vmls.s32    \q1,  q12,  q9		@;a-5(b+e)+20(c+d)+f 4-8
		vqrshrun.s32 	d20,  q7,  #10	@;0-7 solution
		vqrshrun.s32 	d21,  \q1,  #10	@;8-15 solution
		vqmovn		d24.u8,  q10.u16	@;0-7 solution
		vld1.64		{d25},  [r2]
		vrhadd.u8  d24, d24, d25
		vst1.64     {d24}, [\r1],  r3
	.endm
	
    .macro ADDHALF4_C_ROW q0, r0, q1, r1, d0, d1, d2, d3,  d4, d5, d6, d7, d8, d9, d10, d11
		vld1.64		{\q0},  [\r0], r1		@;f0 ... f15
		vaddl.u8	q10,  \d0,  \d1		@;c0+d0 ... c7+d7
		vaddl.u8	q11,  \d2,  \d3		@;c8+d8 ... c15+d15	
		vaddl.u8	q6,  \d4,  \d5		@;a0+f0 ... a7+f7
		vaddl.u8	q7,  \d6,  \d7		@;a8+f8 ... a15+f15
		vaddl.u8	q8,  \d8,  \d9		@;b0+e0 ... b7+e7
		vaddl.u8	q9,  \d10,  \d11		@;b8+e8 ... b15+e15	
		vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
		vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
		vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
		vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
		vext.8		q8,  q6,  q7,  #2	@;b0 ... b7 
		vext.8		q9,  q6,  q7,  #4	@;c0 ... c7
		vext.8		q10,  q6,  q7,  #6	@;d0 ... d7 
		vext.8		q11,  q6,  q7,  #8	@;e0 ... e7	
		vext.8		q12,  q6,  q7,  #10	@;f0 ... f7 
		vaddl.s16	q7,  d12,  d24		@;a0+f0 ... a3+f3
		vaddl.s16	q6,  d16,  d22		@;b0+e0 ... b3+e3
		vaddl.s16	q8,  d18,  d20		@;c0+d0 ... c3+d3
		vmov.i32    q9,  #5				@;coeff 5
		vmov.i32    q10,  #20			@;coeff 20	
		vmla.s32    q7,  q8,  q10  		@;a+20(c+d)+f 0-3
		vmls.s32    q7,  q6,  q9		@;a-5(b+e)+20(c+d)+f 0-3
		vqrshrun.s32 	d20,  q7,  #10	@;0-7 solution
		vqmovn		d24.u8,  q10.u16	@;0-7 solution
		vld1.32		{d25[0]},  [r2]
		vrhadd.u8  d24, d24, d25
		vst1.32     {d24[0]}, [\r1],  r3
	.endm

avg_dst_luma_v_16x16_ARMV7:
	push     	{r4 - r11, r14}
	mov         r5, #2
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vqrshrun 	d25.u8,  q7.s16,  #5@;8-15 solution
	vld1.64		{q13},  [r2]
	vrhadd.u8  q12, q12, q13
	vst1.64     {q12}, [r2],  r3
avg_dst_luma_v_16x16_loop:	
	AVG16_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	AVG16_V_ROW  q1, r12, r2, r3, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	AVG16_V_ROW  q2, r12, r2, r3, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
	AVG16_V_ROW  q3, r12, r2, r3, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5
	AVG16_V_ROW  q4, r12, r2, r3, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
	AVG16_V_ROW  q5, r12, r2, r3, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
	subs        r5, r5, #1
       bne         avg_dst_luma_v_16x16_loop		 
	AVG16_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	AVG16_V_ROW  q1, r12, r2, r3, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	AVG16_V_ROW  q2, r12, r2, r3, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	pop      	{r4 - r11, pc}

avg_add_luma_v_16x16_ARMV7:
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
       mov         r8, #2
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vqrshrun 	d25.u8,  q7.s16,  #5@;8-15 solution
	vld1.64		{q13},  [r4], r5
	vrhadd.u8  q12, q12, q13
	vld1.64		{q13},  [r2]
	vrhadd.u8  q12, q12, q13
	vst1.64     {q12}, [r2],  r3
avg_add_luma_v_16x16_loop:	
	ADDAVG16_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	ADDAVG16_V_ROW  q1, r12, r2, r3, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	ADDAVG16_V_ROW  q2, r12, r2, r3, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
	ADDAVG16_V_ROW  q3, r12, r2, r3, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5
	ADDAVG16_V_ROW  q4, r12, r2, r3, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
	ADDAVG16_V_ROW  q5, r12, r2, r3, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
	subs        r8, r8, #1
       bne         avg_add_luma_v_16x16_loop		 
	ADDAVG16_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	ADDAVG16_V_ROW  q1, r12, r2, r3, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	ADDAVG16_V_ROW  q2, r12, r2, r3, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	pop      	{r4 - r11, pc}

avg_dst_luma_v_16x8_ARMV7:
	
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vqrshrun 	d25.u8,  q7.s16,  #5@;8-15 solution
	vld1.64		{q13},  [r2]
	vrhadd.u8  q12, q12, q13
	vst1.64     {q12}, [r2],  r3
	AVG16_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	AVG16_V_ROW  q1, r12, r2, r3, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	AVG16_V_ROW  q2, r12, r2, r3, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
	AVG16_V_ROW  q3, r12, r2, r3, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5
	AVG16_V_ROW  q4, r12, r2, r3, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
	AVG16_V_ROW  q5, r12, r2, r3, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
	AVG16_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	bx          lr

avg_add_luma_v_16x8_ARMV7:
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vqrshrun 	d25.u8,  q7.s16,  #5@;8-15 solution
	vld1.64		{q13},  [r4], r5
	vrhadd.u8  q12, q12, q13
	vld1.64		{q13},  [r2]
	vrhadd.u8  q12, q12, q13
	vst1.64     {q12}, [r2],  r3	
	ADDAVG16_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	ADDAVG16_V_ROW  q1, r12, r2, r3, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	ADDAVG16_V_ROW  q2, r12, r2, r3, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
	ADDAVG16_V_ROW  q3, r12, r2, r3, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5
	ADDAVG16_V_ROW  q4, r12, r2, r3, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
	ADDAVG16_V_ROW  q5, r12, r2, r3, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9		 
	ADDAVG16_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	pop      	{r4 - r11, pc}

avg_dst_luma_v_8x16_ARMV7:
	push     	{r4 - r11, r14}
	mov         r5, #2
	sub         r0, r0, r1, LSL #1  @src = src - 2*stride
	vld1.64		{d0},  [r0], r1		@src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @r12 = src + 3*stride
	vld1.64		{d2},  [r0], r1		@src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@coeff 5
	vld1.64		{d10},  [r12], r1	@src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{d4},  [r0], r1		@;src c0 c1 ... c15
	vld1.64		{d6},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vld1.64		{d26},  [r2]
	vrhadd.u8  d24, d24, d26
	vst1.64     {d24}, [r2],  r3
avg_dst_luma_v_8x16_loop:
	AVG8_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	AVG8_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	AVG8_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	AVG8_V_ROW  d6, r12, r2, r3, d0, d2, d8, d6, d10, d4
	AVG8_V_ROW  d8, r12, r2, r3, d2, d4, d10, d8, d0, d6
	AVG8_V_ROW  d10, r12, r2, r3, d4, d6, d0, d10, d2, d8
	subs        r5, r5, #1
       bne         avg_dst_luma_v_8x16_loop
	AVG8_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	AVG8_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	AVG8_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	pop      	{r4 - r11, pc}

avg_add_luma_v_8x16_ARMV7:
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
       mov         r8, #2
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vqrshrun 	d25.u8,  q7.s16,  #5@;8-15 solution
	vld1.64		{q13},  [r4], r5
	vrhadd.u8  q12, q12, q13
	vld1.64		{q13},  [r2]
	vrhadd.u8  q12, q12, q13
	vst1.64     {q12}, [r2],  r3
avg_add_luma_v_8x16_loop:	
	ADDAVG8_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	ADDAVG8_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	ADDAVG8_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	ADDAVG8_V_ROW  d6, r12, r2, r3, d0, d2, d8, d6, d10, d4
	ADDAVG8_V_ROW  d8, r12, r2, r3, d2, d4, d10, d8, d0, d6
	ADDAVG8_V_ROW  d10, r12, r2, r3, d4, d6, d0, d10, d2, d8
	subs        r8, r8, #1
       bne         avg_add_luma_v_8x16_loop		 
	ADDAVG8_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	ADDAVG8_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	ADDAVG8_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	pop      	{r4 - r11, pc}

avg_dst_luma_v_8x8_ARMV7:
	
	sub         r0, r0, r1, LSL #1  @src = src - 2*stride
	vld1.64		{d0},  [r0], r1		@src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @r12 = src + 3*stride
	vld1.64		{d2},  [r0], r1		@src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@coeff 5
	vld1.64		{d10},  [r12], r1	@src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{d4},  [r0], r1		@;src c0 c1 ... c15
	vld1.64		{d6},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vld1.64		{d26},  [r2]
	vrhadd.u8  d24, d24, d26
	vst1.64     {d24}, [r2],  r3
	AVG8_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	AVG8_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	AVG8_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	AVG8_V_ROW  d6, r12, r2, r3, d0, d2, d8, d6, d10, d4
	AVG8_V_ROW  d8, r12, r2, r3, d2, d4, d10, d8, d0, d6
	AVG8_V_ROW  d10, r12, r2, r3, d4, d6, d0, d10, d2, d8
	AVG8_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	bx          lr

avg_add_luma_v_8x8_ARMV7:
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	vld1.64		{d0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{d2},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{d10},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{d4},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{d6},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vld1.64		{d26},  [r4], r5
	vrhadd.u8  d24, d24, d26
	vld1.64		{d26},  [r2]
	vrhadd.u8  d24, d24, d26
	vst1.64     {d24}, [r2],  r3	
	ADDAVG8_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	ADDAVG8_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	ADDAVG8_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	ADDAVG8_V_ROW  d6, r12, r2, r3, d0, d2, d8, d6, d10, d4
	ADDAVG8_V_ROW  d8, r12, r2, r3, d2, d4, d10, d8, d0, d6
	ADDAVG8_V_ROW  d10, r12, r2, r3, d4, d6, d0, d10, d2, d8
	ADDAVG8_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	pop      	{r4 - r11, pc}

avg_dst_luma_v_8x4_ARMV7:
	
	sub         r0, r0, r1, LSL #1  @src = src - 2*stride
	vld1.64		{d0},  [r0], r1		@src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @r12 = src + 3*stride
	vld1.64		{d2},  [r0], r1		@src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@coeff 5
	vld1.64		{d10},  [r12], r1	@src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{d4},  [r0], r1		@;src c0 c1 ... c15
	vld1.64		{d6},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vld1.64		{d26},  [r2]
	vrhadd.u8  d24, d24, d26
	vst1.64     {d24}, [r2],  r3
	AVG8_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	AVG8_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	AVG8_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	bx          lr

avg_add_luma_v_8x4_ARMV7:
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	vld1.64		{d0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{d2},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{d10},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{d4},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{d6},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vld1.64		{d26},  [r4], r5
	vrhadd.u8  d24, d24, d26
	vld1.64		{d26},  [r2]
	vrhadd.u8  d24, d24, d26
	vst1.64     {d24}, [r2],  r3	
	ADDAVG8_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	ADDAVG8_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	ADDAVG8_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	pop      	{r4 - r11, pc}

avg_dst_luma_v_4x8_ARMV7:
	
	sub         r0, r0, r1, LSL #1  @src = src - 2*stride
	vld1.64		{d0},  [r0], r1		@src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @r12 = src + 3*stride
	vld1.64		{d2},  [r0], r1		@src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@coeff 5
	vld1.64		{d10},  [r12], r1	@src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{d4},  [r0], r1		@;src c0 c1 ... c15
	vld1.64		{d6},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vld1.64		{d26},  [r2]
	vrhadd.u8  d24, d24, d26
	vst1.32     {d24[0]}, [r2],  r3
	AVG4_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	AVG4_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	AVG4_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	AVG4_V_ROW  d6, r12, r2, r3, d0, d2, d8, d6, d10, d4
	AVG4_V_ROW  d8, r12, r2, r3, d2, d4, d10, d8, d0, d6
	AVG4_V_ROW  d10, r12, r2, r3, d4, d6, d0, d10, d2, d8
	AVG4_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	bx          lr

avg_add_luma_v_4x8_ARMV7:
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	vld1.64		{d0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{d2},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{d10},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{d4},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{d6},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vld1.64		{d26},  [r4], r5
	vrhadd.u8  d24, d24, d26
	vld1.64		{d26},  [r2]
	vrhadd.u8  d24, d24, d26
	vst1.32     {d24[0]}, [r2],  r3	
	ADDAVG4_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	ADDAVG4_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	ADDAVG4_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	ADDAVG4_V_ROW  d6, r12, r2, r3, d0, d2, d8, d6, d10, d4
	ADDAVG4_V_ROW  d8, r12, r2, r3, d2, d4, d10, d8, d0, d6
	ADDAVG4_V_ROW  d10, r12, r2, r3, d4, d6, d0, d10, d2, d8
	ADDAVG4_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	pop      	{r4 - r11, pc}

avg_dst_luma_v_4x4_ARMV7:
	
	sub         r0, r0, r1, LSL #1  @src = src - 2*stride
	vld1.64		{d0},  [r0], r1		@src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @r12 = src + 3*stride
	vld1.64		{d2},  [r0], r1		@src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@coeff 5
	vld1.64		{d10},  [r12], r1	@src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{d4},  [r0], r1		@;src c0 c1 ... c15
	vld1.64		{d6},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vld1.64		{d26},  [r2]
	vrhadd.u8  d24, d24, d26
	vst1.32     {d24[0]}, [r2],  r3
	AVG4_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	AVG4_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	AVG4_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	bx          lr
	
avg_add_luma_v_4x4_ARMV7:
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	vld1.64		{d0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{d2},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{d10},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{d4},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{d6},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vld1.64		{d26},  [r4], r5
	vrhadd.u8  d24, d24, d26
	vld1.64		{d26},  [r2]
	vrhadd.u8  d24, d24, d26
	vst1.32     {d24[0]}, [r2],  r3	
	ADDAVG4_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	ADDAVG4_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	ADDAVG4_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	pop      	{r4 - r11, pc}


avg_dst_luma_h_16x16_ARMV7:
	push     	{r4 - r11, r14}
       mov         r5, #4
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
avg_dst_luma_h_16x16_loop	:
	AVG16_H_ROW 
	AVG16_H_ROW 
	AVG16_H_ROW 
	AVG16_H_ROW 
	subs        r5, r5, #1
       bne         avg_dst_luma_h_16x16_loop		 
	pop      	{r4 - r11, pc}

avg_dst_luma_h_16x8_ARMV7:
       push     	{r4 - r11, r14}
       mov         r5, #2
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
avg_dst_luma_h_16x8_loop	:
	AVG16_H_ROW 
	AVG16_H_ROW 
	AVG16_H_ROW 
	AVG16_H_ROW 
	subs        r5, r5, #1
       bne         avg_dst_luma_h_16x8_loop		 
	pop      	{r4 - r11, pc}

avg_dst_luma_h_8x16_ARMV7:
	push     	{r4 - r11, r14}
       mov         r5, #4
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
avg_dst_luma_h_8x16_loop	:
	AVG8_H_ROW 
	AVG8_H_ROW 
	AVG8_H_ROW 
	AVG8_H_ROW 
	subs        r5, r5, #1
       bne         avg_dst_luma_h_8x16_loop		 
	pop      	{r4 - r11, pc}

avg_dst_luma_h_8x8_ARMV7:
	push     	{r4 - r11, r14}
       mov         r5, #2
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
avg_dst_luma_h_8x8_loop	:
	AVG8_H_ROW 
	AVG8_H_ROW 
	AVG8_H_ROW 
	AVG8_H_ROW 
	subs        r5, r5, #1
       bne         avg_dst_luma_h_8x8_loop		 
	pop      	{r4 - r11, pc}

avg_dst_luma_h_8x4_ARMV7:
	
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20	
	AVG8_H_ROW 
	AVG8_H_ROW 
	AVG8_H_ROW 
	AVG8_H_ROW 
	bx          lr

avg_dst_luma_h_4x8_ARMV7:
	push     	{r4 - r11, r14}
       mov         r5, #2
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
avg_dst_luma_h_4x8_loop	:
	AVG4_H_ROW 
	AVG4_H_ROW 
	AVG4_H_ROW 
	AVG4_H_ROW 
	subs        r5, r5, #1
       bne         avg_dst_luma_h_4x8_loop		 
	pop      	{r4 - r11, pc}

avg_dst_luma_h_4x4_ARMV7:

	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20	
	AVG4_H_ROW 
	AVG4_H_ROW 
	AVG4_H_ROW 
	AVG4_H_ROW 
	bx          lr

avg_add_luma_h_16x16_ARMV7:
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
       mov         r8, #4
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
avg_add_luma_h_16x16_loop	:
	ADDAVG16_H_ROW 
	ADDAVG16_H_ROW 
	ADDAVG16_H_ROW 
	ADDAVG16_H_ROW 
	subs        r8, r8, #1
       bne         avg_add_luma_h_16x16_loop		 
	pop      	{r4 - r11, pc}

avg_add_luma_h_16x8_ARMV7:
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
       mov         r8, #2
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
avg_add_luma_h_16x8_loop	:
	ADDAVG16_H_ROW 
	ADDAVG16_H_ROW 
	ADDAVG16_H_ROW 
	ADDAVG16_H_ROW 
	subs        r8, r8, #1
       bne         avg_add_luma_h_16x8_loop		 
	pop      	{r4 - r11, pc}

avg_add_luma_h_8x16_ARMV7:
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
       mov         r8, #4
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
avg_add_luma_h_8x16_loop	:
	ADDAVG8_H_ROW 
	ADDAVG8_H_ROW 
	ADDAVG8_H_ROW 
	ADDAVG8_H_ROW 
	subs        r8, r8, #1
       bne         avg_add_luma_h_8x16_loop		 
	pop      	{r4 - r11, pc}

avg_add_luma_h_8x4_ARMV7:
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
	ADDAVG8_H_ROW 
	ADDAVG8_H_ROW 
	ADDAVG8_H_ROW 
	ADDAVG8_H_ROW 	 
	pop      	{r4 - r11, pc}

avg_add_luma_h_8x8_ARMV7:
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
       mov         r8, #2
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
avg_add_luma_h_8x8_loop	:
	ADDAVG8_H_ROW 
	ADDAVG8_H_ROW 
	ADDAVG8_H_ROW 
	ADDAVG8_H_ROW 
	subs        r8, r8, #1
       bne         avg_add_luma_h_8x8_loop		 
	pop      	{r4 - r11, pc}

avg_add_luma_h_4x8_ARMV7:
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
       mov         r8, #2
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
avg_add_luma_h_4x8_loop	:
	ADDAVG4_H_ROW 
	ADDAVG4_H_ROW 
	ADDAVG4_H_ROW 
	ADDAVG4_H_ROW 
	subs        r8, r8, #1
       bne         avg_add_luma_h_4x8_loop		 
	pop      	{r4 - r11, pc}

avg_add_luma_h_4x4_ARMV7:
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
	ADDAVG4_H_ROW 
	ADDAVG4_H_ROW 
	ADDAVG4_H_ROW 
	ADDAVG4_H_ROW 	 
	pop      	{r4 - r11, pc}
	
avg_get_luma_v_16x16_ARMV7:
	push     	{r4 - r11, r14}
  	mov		r6, #2
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15
	vld1.64		{q13},  [r4], r5
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vqrshrun 	d25.u8,  q7.s16,  #5@;8-15 solution
	vrhadd.u8  q12, q12, q13
	vst1.64     {q12}, [r2],  r3
avg_get_luma_v_16x16_loop:
	QUARTER16_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	QUARTER16_V_ROW  q1, r12, r2, r3, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	QUARTER16_V_ROW  q2, r12, r2, r3, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
	QUARTER16_V_ROW  q3, r12, r2, r3, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5
	QUARTER16_V_ROW  q4, r12, r2, r3, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
	QUARTER16_V_ROW  q5, r12, r2, r3, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
	subs        r6, r6, #1
       bne         avg_get_luma_v_16x16_loop
	QUARTER16_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	QUARTER16_V_ROW  q1, r12, r2, r3, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	QUARTER16_V_ROW  q2, r12, r2, r3, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	pop      	{r4 - r11, pc}

avg_get_luma_v_16x8_ARMV7:
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15
	vld1.64		{q13},  [r4], r5
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vqrshrun 	d25.u8,  q7.s16,  #5@;8-15 solution
	vrhadd.u8  q12, q12, q13
	vst1.64     {q12}, [r2],  r3
	QUARTER16_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	QUARTER16_V_ROW  q1, r12, r2, r3, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	QUARTER16_V_ROW  q2, r12, r2, r3, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
	QUARTER16_V_ROW  q3, r12, r2, r3, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5
	QUARTER16_V_ROW  q4, r12, r2, r3, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
	QUARTER16_V_ROW  q5, r12, r2, r3, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
	QUARTER16_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	pop      	{r4 - r11, pc}

avg_get_luma_v_8x16_ARMV7:
	push     	{r4 - r11, r14}
	mov		r6, #2
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
	sub         r0, r0, r1, LSL #1  @src = src - 2*stride
	vld1.64		{d0},  [r0], r1		@src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @r12 = src + 3*stride
	vld1.64		{d2},  [r0], r1		@src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@coeff 5
	vld1.64		{d10},  [r12], r1	@src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{d4},  [r0], r1		@;src c0 c1 ... c15
	vld1.64		{d6},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7	
	vld1.64		{d26},  [r4], r5
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vrhadd.u8  d24, d24, d26
	vst1.64     {d24}, [r2],  r3
avg_get_luma_v_8x16_loop:
	QUARTER8_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	QUARTER8_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	QUARTER8_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	QUARTER8_V_ROW  d6, r12, r2, r3, d0, d2, d8, d6, d10, d4
	QUARTER8_V_ROW  d8, r12, r2, r3, d2, d4, d10, d8, d0, d6
	QUARTER8_V_ROW  d10, r12, r2, r3, d4, d6, d0, d10, d2, d8
	subs        r6, r6, #1
       bne         avg_get_luma_v_8x16_loop
	QUARTER8_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	QUARTER8_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	QUARTER8_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	pop      	{r4 - r11, pc}	

avg_get_luma_v_8x8_ARMV7:
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
	sub         r0, r0, r1, LSL #1  @src = src - 2*stride
	vld1.64		{d0},  [r0], r1		@src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @r12 = src + 3*stride
	vld1.64		{d2},  [r0], r1		@src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@coeff 5
	vld1.64		{d10},  [r12], r1	@src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{d4},  [r0], r1		@;src c0 c1 ... c15
	vld1.64		{d6},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7	
	vld1.64		{d26},  [r4], r5
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vrhadd.u8  d24, d24, d26
	vst1.64     {d24}, [r2],  r3
	QUARTER8_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	QUARTER8_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	QUARTER8_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	QUARTER8_V_ROW  d6, r12, r2, r3, d0, d2, d8, d6, d10, d4
	QUARTER8_V_ROW  d8, r12, r2, r3, d2, d4, d10, d8, d0, d6
	QUARTER8_V_ROW  d10, r12, r2, r3, d4, d6, d0, d10, d2, d8
	QUARTER8_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	pop      	{r4 - r11, pc}	

avg_get_luma_v_8x4_ARMV7:
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
	sub         r0, r0, r1, LSL #1  @src = src - 2*stride
	vld1.64		{d0},  [r0], r1		@src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @r12 = src + 3*stride
	vld1.64		{d2},  [r0], r1		@src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@coeff 5
	vld1.64		{d10},  [r12], r1	@src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{d4},  [r0], r1		@;src c0 c1 ... c15
	vld1.64		{d6},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7	
	vld1.64		{d26},  [r4], r5
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vrhadd.u8  d24, d24, d26
	vst1.64     {d24}, [r2],  r3
	QUARTER8_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	QUARTER8_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	QUARTER8_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	pop      	{r4 - r11, pc}	

avg_get_luma_v_4x8_ARMV7:
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
	sub         r0, r0, r1, LSL #1  @src = src - 2*stride
	vld1.64		{d0},  [r0], r1		@src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @r12 = src + 3*stride
	vld1.64		{d2},  [r0], r1		@src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@coeff 5
	vld1.64		{d10},  [r12], r1	@src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{d4},  [r0], r1		@;src c0 c1 ... c15
	vld1.64		{d6},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vld1.64		{d26},  [r4], r5
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vrhadd.u8  d24, d24, d26
	vst1.32     {d24[0]}, [r2],  r3
	QUARTER4_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	QUARTER4_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	QUARTER4_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	QUARTER4_V_ROW  d6, r12, r2, r3, d0, d2, d8, d6, d10, d4
	QUARTER4_V_ROW  d8, r12, r2, r3, d2, d4, d10, d8, d0, d6
	QUARTER4_V_ROW  d10, r12, r2, r3, d4, d6, d0, d10, d2, d8
	QUARTER4_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	pop      	{r4 - r11, pc}

avg_get_luma_v_4x4_ARMV7:
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
	sub         r0, r0, r1, LSL #1  @src = src - 2*stride
	vld1.64		{d0},  [r0], r1		@src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @r12 = src + 3*stride
	vld1.64		{d2},  [r0], r1		@src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@coeff 5
	vld1.64		{d10},  [r12], r1	@src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{d4},  [r0], r1		@;src c0 c1 ... c15
	vld1.64		{d6},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vld1.64		{d26},  [r4], r5
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vrhadd.u8  d24, d24, d26
	vst1.32     {d24[0]}, [r2],  r3
	QUARTER4_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	QUARTER4_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	QUARTER4_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	pop      	{r4 - r11, pc}

avg_get_luma_h_16x16_ARMV7:
  	push     	{r4 - r11, r14}
  	mov		r6, #4
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
avg_get_luma_h_16x16_loop:
	QUARTER16_H_ROW 
	QUARTER16_H_ROW 
	QUARTER16_H_ROW 
	QUARTER16_H_ROW 
	subs        r6, r6, #1
       bne         avg_get_luma_h_16x16_loop
	pop      	{r4 - r11, pc}

avg_get_luma_h_16x8_ARMV7:
  	push     	{r4 - r11, r14}
  	mov		r6, #2
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
avg_get_luma_h_16x8_loop:
	QUARTER16_H_ROW 
	QUARTER16_H_ROW 
	QUARTER16_H_ROW 
	QUARTER16_H_ROW 
	subs        r6, r6, #1
       bne         avg_get_luma_h_16x8_loop
	pop      	{r4 - r11, pc}

avg_get_luma_h_8x16_ARMV7:
  	push     	{r4 - r11, r14}
  	mov		r6, #4
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
avg_get_luma_h_8x16_loop:
	QUARTER8_H_ROW 
	QUARTER8_H_ROW 
	QUARTER8_H_ROW 
	QUARTER8_H_ROW 
	subs        r6, r6, #1
       bne         avg_get_luma_h_8x16_loop
	pop      	{r4 - r11, pc}

avg_get_luma_h_8x8_ARMV7:
  	push     	{r4 - r11, r14}
  	mov		r6, #2
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
avg_get_luma_h_8x8_loop:
	QUARTER8_H_ROW 
	QUARTER8_H_ROW 
	QUARTER8_H_ROW 
	QUARTER8_H_ROW 
	subs        r6, r6, #1
       bne         avg_get_luma_h_8x8_loop
	pop      	{r4 - r11, pc}

avg_get_luma_h_8x4_ARMV7:
  	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
	QUARTER8_H_ROW 
	QUARTER8_H_ROW 
	QUARTER8_H_ROW 
	QUARTER8_H_ROW 
	pop      	{r4 - r11, pc}

avg_get_luma_h_4x8_ARMV7:
  	push     	{r4 - r11, r14}
  	mov		r6, #2
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
avg_get_luma_h_4x8_loop:
	QUARTER4_H_ROW 
	QUARTER4_H_ROW 
	QUARTER4_H_ROW 
	QUARTER4_H_ROW 
	subs        r6, r6, #1
       bne         avg_get_luma_h_4x8_loop
	pop      	{r4 - r11, pc}

avg_get_luma_h_4x4_ARMV7:
  	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
	ldr		r5, [sp, #40]
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
	QUARTER4_H_ROW 
	QUARTER4_H_ROW 
	QUARTER4_H_ROW 
	QUARTER4_H_ROW 
	pop      	{r4 - r11, pc}
	
get_luma_c_16x16_ARMV7:	
	push     	{r4 - r11, r14}
       mov         r10, #2
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	sub			r0, r0, #2
	add         r5, r0, #8
	add			r7, r2, #8
get_luma_c_16x16_loop0:
	mov         r11, #2
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vext.8		q8,  q6,  q7,  #2	@;b0 ... b7 
	vext.8		q9,  q6,  q7,  #4	@;c0 ... c7
	vext.8		q10,  q6,  q7,  #6	@;d0 ... d7 
	vext.8		q11,  q6,  q7,  #8	@;e0 ... e7	
	vext.8		q12,  q6,  q7,  #10	@;f0 ... f7 
	vaddl.s16	q7,  d12,  d24		@;a0+f0 ... a3+f3
	vaddl.s16	q0,  d13,  d25		@;a4+f4 ... a7+f7
	vaddl.s16	q6,  d16,  d22		@;b0+e0 ... b3+e3
	vaddl.s16	q12,  d17,  d23		@;b4+e4 ... b7+e7
	vaddl.s16	q8,  d18,  d20		@;c0+d0 ... c3+d3
	vaddl.s16	q11,  d19,  d21		@;c4+d4 ... c7+d7
	vmov.i32    q9,  #5				@;coeff 5
	vmov.i32    q10,  #20			@;coeff 20	
	vmla.s32    q7,  q8,  q10  		@;a+20(c+d)+f 0-3
	vmla.s32    q0,  q11,  q10		@;a+20(c+d)+f 4-8
	vmls.s32    q7,  q6,  q9		@;a-5(b+e)+20(c+d)+f 0-3
	vmls.s32    q0,  q12,  q9		@;a-5(b+e)+20(c+d)+f 4-8
	vqrshrun 	d20.u16,  q7.s32,  #10	@;0-7 solution
	vqrshrun 	d21.u16,  q0.s32,  #10	@;8-15 solution
	vqmovn		d24.u8,  q10.u16	@;0-7 solution
	vst1.64     {d24}, [r2],  r3
get_luma_c_16x16_loop1:
       HALF8_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	HALF8_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	HALF8_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	HALF8_C_ROW  q3, r12, q4, r2, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5		
	HALF8_C_ROW  q4, r12, q5, r2, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7	
	HALF8_C_ROW  q5, r12, q0, r2, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
       subs        r11, r11, #1
       bne         get_luma_c_16x16_loop1	
       HALF8_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	HALF8_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	HALF8_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
	mov			r0, r5
	mov			r2, r7
	subs        r10, r10, #1
       bne         get_luma_c_16x16_loop0	
	pop      	{r4 - r11, pc}	

add_luma_c_16x16_ARMV7:	
	push     	{r4 - r11, r14}
       mov         r10, #2
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	sub			r0, r0, #2
	add         r5, r0, #8
	add			r7, r2, #8
add_luma_c_16x16_loop0:
	mov         r11, #2
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vext.8		q8,  q6,  q7,  #2	@;b0 ... b7 
	vext.8		q9,  q6,  q7,  #4	@;c0 ... c7
	vext.8		q10,  q6,  q7,  #6	@;d0 ... d7 
	vext.8		q11,  q6,  q7,  #8	@;e0 ... e7	
	vext.8		q12,  q6,  q7,  #10	@;f0 ... f7 
	vaddl.s16	q7,  d12,  d24		@;a0+f0 ... a3+f3
	vaddl.s16	q0,  d13,  d25		@;a4+f4 ... a7+f7
	vaddl.s16	q6,  d16,  d22		@;b0+e0 ... b3+e3
	vaddl.s16	q12,  d17,  d23		@;b4+e4 ... b7+e7
	vaddl.s16	q8,  d18,  d20		@;c0+d0 ... c3+d3
	vaddl.s16	q11,  d19,  d21		@;c4+d4 ... c7+d7
	vmov.i32    q9,  #5				@;coeff 5
	vmov.i32    q10,  #20			@;coeff 20	
	vmla.s32    q7,  q8,  q10  		@;a+20(c+d)+f 0-3
	vmla.s32    q0,  q11,  q10		@;a+20(c+d)+f 4-8
	vmls.s32    q7,  q6,  q9		@;a-5(b+e)+20(c+d)+f 0-3
	vmls.s32    q0,  q12,  q9		@;a-5(b+e)+20(c+d)+f 4-8
	vqrshrun 	d20.u16,  q7.s32,  #10	@;0-7 solution
	vqrshrun 	d21.u16,  q0.s32,  #10	@;8-15 solution
	vqmovn		d24.u8,  q10.u16	@;0-7 solution
	vld1.64		{d25},  [r2]
	vrhadd.u8  d24, d24, d25
	vst1.64     {d24}, [r2],  r3
add_luma_c_16x16_loop1:
       ADDHALF8_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	ADDHALF8_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	ADDHALF8_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	ADDHALF8_C_ROW  q3, r12, q4, r2, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5		
	ADDHALF8_C_ROW  q4, r12, q5, r2, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7	
	ADDHALF8_C_ROW  q5, r12, q0, r2, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
       subs        r11, r11, #1
       bne         add_luma_c_16x16_loop1	
       ADDHALF8_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	ADDHALF8_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	ADDHALF8_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
	mov			r0, r5
	mov			r2, r7
	subs        r10, r10, #1
       bne         add_luma_c_16x16_loop0	
	pop      	{r4 - r11, pc}	

get_luma_c_16x8_ARMV7:	
	push     	{r4 - r11, r14}
       mov         r10, #2
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	sub			r0, r0, #2
	add         r5, r0, #8
	add			r7, r2, #8
get_luma_c_16x8_loop0:
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vext.8		q8,  q6,  q7,  #2	@;b0 ... b7 
	vext.8		q9,  q6,  q7,  #4	@;c0 ... c7
	vext.8		q10,  q6,  q7,  #6	@;d0 ... d7 
	vext.8		q11,  q6,  q7,  #8	@;e0 ... e7	
	vext.8		q12,  q6,  q7,  #10	@;f0 ... f7 
	vaddl.s16	q7,  d12,  d24		@;a0+f0 ... a3+f3
	vaddl.s16	q0,  d13,  d25		@;a4+f4 ... a7+f7
	vaddl.s16	q6,  d16,  d22		@;b0+e0 ... b3+e3
	vaddl.s16	q12,  d17,  d23		@;b4+e4 ... b7+e7
	vaddl.s16	q8,  d18,  d20		@;c0+d0 ... c3+d3
	vaddl.s16	q11,  d19,  d21		@;c4+d4 ... c7+d7
	vmov.i32    q9,  #5				@;coeff 5
	vmov.i32    q10,  #20			@;coeff 20	
	vmla.s32    q7,  q8,  q10  		@;a+20(c+d)+f 0-3
	vmla.s32    q0,  q11,  q10		@;a+20(c+d)+f 4-8
	vmls.s32    q7,  q6,  q9		@;a-5(b+e)+20(c+d)+f 0-3
	vmls.s32    q0,  q12,  q9		@;a-5(b+e)+20(c+d)+f 4-8
	vqrshrun 	d20.u16,  q7.s32,  #10	@;0-7 solution
	vqrshrun 	d21.u16,  q0.s32,  #10	@;8-15 solution
	vqmovn		d24.u8,  q10.u16	@;0-7 solution
	vst1.64     {d24}, [r2],  r3
    HALF8_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	HALF8_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	HALF8_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	HALF8_C_ROW  q3, r12, q4, r2, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5		
	HALF8_C_ROW  q4, r12, q5, r2, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7	
	HALF8_C_ROW  q5, r12, q0, r2, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
    HALF8_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
       mov			r0, r5
	mov			r2, r7
	subs        r10, r10, #1
       bne         get_luma_c_16x8_loop0	
	pop      	{r4 - r11, pc}	

add_luma_c_16x8_ARMV7:	
	push     	{r4 - r11, r14}
       mov         r10, #2
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	sub			r0, r0, #2
	add         r5, r0, #8
	add			r7, r2, #8
add_luma_c_16x8_loop0:
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vext.8		q8,  q6,  q7,  #2	@;b0 ... b7 
	vext.8		q9,  q6,  q7,  #4	@;c0 ... c7
	vext.8		q10,  q6,  q7,  #6	@;d0 ... d7 
	vext.8		q11,  q6,  q7,  #8	@;e0 ... e7	
	vext.8		q12,  q6,  q7,  #10	@;f0 ... f7 
	vaddl.s16	q7,  d12,  d24		@;a0+f0 ... a3+f3
	vaddl.s16	q0,  d13,  d25		@;a4+f4 ... a7+f7
	vaddl.s16	q6,  d16,  d22		@;b0+e0 ... b3+e3
	vaddl.s16	q12,  d17,  d23		@;b4+e4 ... b7+e7
	vaddl.s16	q8,  d18,  d20		@;c0+d0 ... c3+d3
	vaddl.s16	q11,  d19,  d21		@;c4+d4 ... c7+d7
	vmov.i32    q9,  #5				@;coeff 5
	vmov.i32    q10,  #20			@;coeff 20	
	vmla.s32    q7,  q8,  q10  		@;a+20(c+d)+f 0-3
	vmla.s32    q0,  q11,  q10		@;a+20(c+d)+f 4-8
	vmls.s32    q7,  q6,  q9		@;a-5(b+e)+20(c+d)+f 0-3
	vmls.s32    q0,  q12,  q9		@;a-5(b+e)+20(c+d)+f 4-8
	vqrshrun 	d20.u16,  q7.s32,  #10	@;0-7 solution
	vqrshrun 	d21.u16,  q0.s32,  #10	@;8-15 solution
	vqmovn		d24.u8,  q10.u16	@;0-7 solution
	vld1.64		{d25},  [r2]
	vrhadd.u8  d24, d24, d25
	vst1.64     {d24}, [r2],  r3
       ADDHALF8_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	ADDHALF8_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	ADDHALF8_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	ADDHALF8_C_ROW  q3, r12, q4, r2, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5		
	ADDHALF8_C_ROW  q4, r12, q5, r2, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7	
	ADDHALF8_C_ROW  q5, r12, q0, r2, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
       ADDHALF8_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	mov			r0, r5
	mov			r2, r7
	subs        r10, r10, #1
       bne         add_luma_c_16x8_loop0	
	pop      	{r4 - r11, pc}	
	

get_luma_c_8x16_ARMV7:	
	
	push     	{r4 - r11, r14}
	mov         r11, #2
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	sub			r0, r0, #2
	add         r5, r0, #8
	add			r7, r2, #8
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vext.8		q8,  q6,  q7,  #2	@;b0 ... b7 
	vext.8		q9,  q6,  q7,  #4	@;c0 ... c7
	vext.8		q10,  q6,  q7,  #6	@;d0 ... d7 
	vext.8		q11,  q6,  q7,  #8	@;e0 ... e7	
	vext.8		q12,  q6,  q7,  #10	@;f0 ... f7 
	vaddl.s16	q7,  d12,  d24		@;a0+f0 ... a3+f3
	vaddl.s16	q0,  d13,  d25		@;a4+f4 ... a7+f7
	vaddl.s16	q6,  d16,  d22		@;b0+e0 ... b3+e3
	vaddl.s16	q12,  d17,  d23		@;b4+e4 ... b7+e7
	vaddl.s16	q8,  d18,  d20		@;c0+d0 ... c3+d3
	vaddl.s16	q11,  d19,  d21		@;c4+d4 ... c7+d7
	vmov.i32    q9,  #5				@;coeff 5
	vmov.i32    q10,  #20			@;coeff 20	
	vmla.s32    q7,  q8,  q10  		@;a+20(c+d)+f 0-3
	vmla.s32    q0,  q11,  q10		@;a+20(c+d)+f 4-8
	vmls.s32    q7,  q6,  q9		@;a-5(b+e)+20(c+d)+f 0-3
	vmls.s32    q0,  q12,  q9		@;a-5(b+e)+20(c+d)+f 4-8
	vqrshrun 	d20.u16,  q7.s32,  #10	@;0-7 solution
	vqrshrun 	d21.u16,  q0.s32,  #10	@;8-15 solution
	vqmovn		d24.u8,  q10.u16	@;0-7 solution
	vst1.64     {d24}, [r2],  r3
get_luma_c_8x16_loop1:
       HALF8_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	HALF8_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	HALF8_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	HALF8_C_ROW  q3, r12, q4, r2, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5		
	HALF8_C_ROW  q4, r12, q5, r2, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7	
	HALF8_C_ROW  q5, r12, q0, r2, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
       subs        r11, r11, #1
       bne         get_luma_c_8x16_loop1	
       HALF8_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	HALF8_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	HALF8_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3				
	pop      	{r4 - r11, pc}	

add_luma_c_8x16_ARMV7:	
	push     	{r4 - r11, r14}
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	sub			r0, r0, #2
	add         r5, r0, #8
	add			r7, r2, #8
	mov         r11, #2
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vext.8		q8,  q6,  q7,  #2	@;b0 ... b7 
	vext.8		q9,  q6,  q7,  #4	@;c0 ... c7
	vext.8		q10,  q6,  q7,  #6	@;d0 ... d7 
	vext.8		q11,  q6,  q7,  #8	@;e0 ... e7	
	vext.8		q12,  q6,  q7,  #10	@;f0 ... f7 
	vaddl.s16	q7,  d12,  d24		@;a0+f0 ... a3+f3
	vaddl.s16	q0,  d13,  d25		@;a4+f4 ... a7+f7
	vaddl.s16	q6,  d16,  d22		@;b0+e0 ... b3+e3
	vaddl.s16	q12,  d17,  d23		@;b4+e4 ... b7+e7
	vaddl.s16	q8,  d18,  d20		@;c0+d0 ... c3+d3
	vaddl.s16	q11,  d19,  d21		@;c4+d4 ... c7+d7
	vmov.i32    q9,  #5				@;coeff 5
	vmov.i32    q10,  #20			@;coeff 20	
	vmla.s32    q7,  q8,  q10  		@;a+20(c+d)+f 0-3
	vmla.s32    q0,  q11,  q10		@;a+20(c+d)+f 4-8
	vmls.s32    q7,  q6,  q9		@;a-5(b+e)+20(c+d)+f 0-3
	vmls.s32    q0,  q12,  q9		@;a-5(b+e)+20(c+d)+f 4-8
	vqrshrun 	d20.u16,  q7.s32,  #10	@;0-7 solution
	vqrshrun 	d21.u16,  q0.s32,  #10	@;8-15 solution
	vqmovn		d24.u8,  q10.u16	@;0-7 solution
	vld1.64		{d25},  [r2]
	vrhadd.u8  d24, d24, d25
	vst1.64     {d24}, [r2],  r3
add_luma_c_8x16_loop1:
       ADDHALF8_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	ADDHALF8_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	ADDHALF8_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	ADDHALF8_C_ROW  q3, r12, q4, r2, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5		
	ADDHALF8_C_ROW  q4, r12, q5, r2, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7	
	ADDHALF8_C_ROW  q5, r12, q0, r2, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
       subs        r11, r11, #1
       bne         add_luma_c_8x16_loop1	
       ADDHALF8_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	ADDHALF8_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	ADDHALF8_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	pop      	{r4 - r11, pc}	

get_luma_c_8x8_ARMV7:	
	
	push     	{r4 - r10, r14}
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	sub			r0, r0, #2
	add         r5, r0, #8
	add			r7, r2, #8
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vext.8		q8,  q6,  q7,  #2	@;b0 ... b7 
	vext.8		q9,  q6,  q7,  #4	@;c0 ... c7
	vext.8		q10,  q6,  q7,  #6	@;d0 ... d7 
	vext.8		q11,  q6,  q7,  #8	@;e0 ... e7	
	vext.8		q12,  q6,  q7,  #10	@;f0 ... f7 
	vaddl.s16	q7,  d12,  d24		@;a0+f0 ... a3+f3
	vaddl.s16	q0,  d13,  d25		@;a4+f4 ... a7+f7
	vaddl.s16	q6,  d16,  d22		@;b0+e0 ... b3+e3
	vaddl.s16	q12,  d17,  d23		@;b4+e4 ... b7+e7
	vaddl.s16	q8,  d18,  d20		@;c0+d0 ... c3+d3
	vaddl.s16	q11,  d19,  d21		@;c4+d4 ... c7+d7
	vmov.i32    q9,  #5				@;coeff 5
	vmov.i32    q10,  #20			@;coeff 20	
	vmla.s32    q7,  q8,  q10  		@;a+20(c+d)+f 0-3
	vmla.s32    q0,  q11,  q10		@;a+20(c+d)+f 4-8
	vmls.s32    q7,  q6,  q9		@;a-5(b+e)+20(c+d)+f 0-3
	vmls.s32    q0,  q12,  q9		@;a-5(b+e)+20(c+d)+f 4-8
	vqrshrun 	d20.u16,  q7.s32,  #10	@;0-7 solution
	vqrshrun 	d21.u16,  q0.s32,  #10	@;8-15 solution
	vqmovn		d24.u8,  q10.u16	@;0-7 solution
	vst1.64     {d24}, [r2],  r3
    HALF8_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	HALF8_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	HALF8_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	HALF8_C_ROW  q3, r12, q4, r2, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5		
	HALF8_C_ROW  q4, r12, q5, r2, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7	
	HALF8_C_ROW  q5, r12, q0, r2, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
    HALF8_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11					
	pop      	{r4 - r10, pc}	

add_luma_c_8x8_ARMV7:	
	push     	{r4 - r11, r14}
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	sub			r0, r0, #2
	add         r5, r0, #8
	add			r7, r2, #8
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vext.8		q8,  q6,  q7,  #2	@;b0 ... b7 
	vext.8		q9,  q6,  q7,  #4	@;c0 ... c7
	vext.8		q10,  q6,  q7,  #6	@;d0 ... d7 
	vext.8		q11,  q6,  q7,  #8	@;e0 ... e7	
	vext.8		q12,  q6,  q7,  #10	@;f0 ... f7 
	vaddl.s16	q7,  d12,  d24		@;a0+f0 ... a3+f3
	vaddl.s16	q0,  d13,  d25		@;a4+f4 ... a7+f7
	vaddl.s16	q6,  d16,  d22		@;b0+e0 ... b3+e3
	vaddl.s16	q12,  d17,  d23		@;b4+e4 ... b7+e7
	vaddl.s16	q8,  d18,  d20		@;c0+d0 ... c3+d3
	vaddl.s16	q11,  d19,  d21		@;c4+d4 ... c7+d7
	vmov.i32    q9,  #5				@;coeff 5
	vmov.i32    q10,  #20			@;coeff 20	
	vmla.s32    q7,  q8,  q10  		@;a+20(c+d)+f 0-3
	vmla.s32    q0,  q11,  q10		@;a+20(c+d)+f 4-8
	vmls.s32    q7,  q6,  q9		@;a-5(b+e)+20(c+d)+f 0-3
	vmls.s32    q0,  q12,  q9		@;a-5(b+e)+20(c+d)+f 4-8
	vqrshrun 	d20.u16,  q7.s32,  #10	@;0-7 solution
	vqrshrun 	d21.u16,  q0.s32,  #10	@;8-15 solution
	vqmovn		d24.u8,  q10.u16	@;0-7 solution
	vld1.64		{d25},  [r2]
	vrhadd.u8  d24, d24, d25
	vst1.64     {d24}, [r2],  r3
       ADDHALF8_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	ADDHALF8_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	ADDHALF8_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	ADDHALF8_C_ROW  q3, r12, q4, r2, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5		
	ADDHALF8_C_ROW  q4, r12, q5, r2, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7	
	ADDHALF8_C_ROW  q5, r12, q0, r2, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9	
       ADDHALF8_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11		
	pop      	{r4 - r11, pc}
	
get_luma_c_8x4_ARMV7:	
	
	push     	{r4 - r10, r14}
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	sub			r0, r0, #2
	add         r5, r0, #8
	add			r7, r2, #8
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vext.8		q8,  q6,  q7,  #2	@;b0 ... b7 
	vext.8		q9,  q6,  q7,  #4	@;c0 ... c7
	vext.8		q10,  q6,  q7,  #6	@;d0 ... d7 
	vext.8		q11,  q6,  q7,  #8	@;e0 ... e7	
	vext.8		q12,  q6,  q7,  #10	@;f0 ... f7 
	vaddl.s16	q7,  d12,  d24		@;a0+f0 ... a3+f3
	vaddl.s16	q0,  d13,  d25		@;a4+f4 ... a7+f7
	vaddl.s16	q6,  d16,  d22		@;b0+e0 ... b3+e3
	vaddl.s16	q12,  d17,  d23		@;b4+e4 ... b7+e7
	vaddl.s16	q8,  d18,  d20		@;c0+d0 ... c3+d3
	vaddl.s16	q11,  d19,  d21		@;c4+d4 ... c7+d7
	vmov.i32    q9,  #5				@;coeff 5
	vmov.i32    q10,  #20			@;coeff 20	
	vmla.s32    q7,  q8,  q10  		@;a+20(c+d)+f 0-3
	vmla.s32    q0,  q11,  q10		@;a+20(c+d)+f 4-8
	vmls.s32    q7,  q6,  q9		@;a-5(b+e)+20(c+d)+f 0-3
	vmls.s32    q0,  q12,  q9		@;a-5(b+e)+20(c+d)+f 4-8
	vqrshrun 	d20.u16,  q7.s32,  #10	@;0-7 solution
	vqrshrun 	d21.u16,  q0.s32,  #10	@;8-15 solution
	vqmovn		d24.u8,  q10.u16	@;0-7 solution
	vst1.64     {d24}, [r2],  r3
    HALF8_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	HALF8_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	HALF8_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3						
	pop      	{r4 - r10, pc}	

add_luma_c_8x4_ARMV7:	
	push     	{r4 - r11, r14}
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	sub			r0, r0, #2
	add         r5, r0, #8
	add			r7, r2, #8
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vext.8		q8,  q6,  q7,  #2	@;b0 ... b7 
	vext.8		q9,  q6,  q7,  #4	@;c0 ... c7
	vext.8		q10,  q6,  q7,  #6	@;d0 ... d7 
	vext.8		q11,  q6,  q7,  #8	@;e0 ... e7	
	vext.8		q12,  q6,  q7,  #10	@;f0 ... f7 
	vaddl.s16	q7,  d12,  d24		@;a0+f0 ... a3+f3
	vaddl.s16	q0,  d13,  d25		@;a4+f4 ... a7+f7
	vaddl.s16	q6,  d16,  d22		@;b0+e0 ... b3+e3
	vaddl.s16	q12,  d17,  d23		@;b4+e4 ... b7+e7
	vaddl.s16	q8,  d18,  d20		@;c0+d0 ... c3+d3
	vaddl.s16	q11,  d19,  d21		@;c4+d4 ... c7+d7
	vmov.i32    q9,  #5				@;coeff 5
	vmov.i32    q10,  #20			@;coeff 20	
	vmla.s32    q7,  q8,  q10  		@;a+20(c+d)+f 0-3
	vmla.s32    q0,  q11,  q10		@;a+20(c+d)+f 4-8
	vmls.s32    q7,  q6,  q9		@;a-5(b+e)+20(c+d)+f 0-3
	vmls.s32    q0,  q12,  q9		@;a-5(b+e)+20(c+d)+f 4-8
	vqrshrun 	d20.u16,  q7.s32,  #10	@;0-7 solution
	vqrshrun 	d21.u16,  q0.s32,  #10	@;8-15 solution
	vqmovn		d24.u8,  q10.u16	@;0-7 solution
	vld1.64		{d25},  [r2]
	vrhadd.u8  d24, d24, d25
	vst1.64     {d24}, [r2],  r3
       ADDHALF8_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	ADDHALF8_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	ADDHALF8_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3			
	pop      	{r4 - r11, pc}

get_luma_c_4x8_ARMV7:	
	
	push     	{r4 - r10, r14}
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	sub			r0, r0, #2
	add         r5, r0, #8
	add			r7, r2, #8
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vext.8		q8,  q6,  q7,  #2	@;b0 ... b7 
	vext.8		q9,  q6,  q7,  #4	@;c0 ... c7
	vext.8		q10,  q6,  q7,  #6	@;d0 ... d7 
	vext.8		q11,  q6,  q7,  #8	@;e0 ... e7	
	vext.8		q12,  q6,  q7,  #10	@;f0 ... f7 
	vaddl.s16	q7,  d12,  d24		@;a0+f0 ... a3+f3
	vaddl.s16	q6,  d16,  d22		@;b0+e0 ... b3+e3
	vaddl.s16	q8,  d18,  d20		@;c0+d0 ... c3+d3
	vmov.i32    q9,  #5				@;coeff 5
	vmov.i32    q10,  #20			@;coeff 20	
	vmla.s32    q7,  q8,  q10  		@;a+20(c+d)+f 0-3
	vmls.s32    q7,  q6,  q9		@;a-5(b+e)+20(c+d)+f 0-3
	vqrshrun 	d20.u16,  q7.s32,  #10	@;0-7 solution
	vqmovn		d24.u8,  q10.u16	@;0-7 solution
	vst1.32     {d24[0]}, [r2],  r3
    HALF4_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	HALF4_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	HALF4_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	HALF4_C_ROW  q3, r12, q4, r2, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5		
	HALF4_C_ROW  q4, r12, q5, r2, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7	
	HALF4_C_ROW  q5, r12, q0, r2, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
    HALF4_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11					
	pop      	{r4 - r10, pc}

add_luma_c_4x8_ARMV7:	
	
	push     	{r4 - r10, r14}
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	sub			r0, r0, #2
	add         r5, r0, #8
	add			r7, r2, #8
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vext.8		q8,  q6,  q7,  #2	@;b0 ... b7 
	vext.8		q9,  q6,  q7,  #4	@;c0 ... c7
	vext.8		q10,  q6,  q7,  #6	@;d0 ... d7 
	vext.8		q11,  q6,  q7,  #8	@;e0 ... e7	
	vext.8		q12,  q6,  q7,  #10	@;f0 ... f7 
	vaddl.s16	q7,  d12,  d24		@;a0+f0 ... a3+f3
	vaddl.s16	q6,  d16,  d22		@;b0+e0 ... b3+e3
	vaddl.s16	q8,  d18,  d20		@;c0+d0 ... c3+d3
	vmov.i32    q9,  #5				@;coeff 5
	vmov.i32    q10,  #20			@;coeff 20	
	vmla.s32    q7,  q8,  q10  		@;a+20(c+d)+f 0-3
	vmls.s32    q7,  q6,  q9		@;a-5(b+e)+20(c+d)+f 0-3
	vqrshrun 	d20.u16,  q7.s32,  #10	@;0-7 solution
	vqmovn		d24.u8,  q10.u16	@;0-7 solution
	vld1.32		{d25[0]},  [r2]
	vrhadd.u8  d24, d24, d25
	vst1.32     {d24[0]}, [r2],  r3
       ADDHALF4_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	ADDHALF4_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	ADDHALF4_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	ADDHALF4_C_ROW  q3, r12, q4, r2, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5		
	ADDHALF4_C_ROW  q4, r12, q5, r2, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7	
	ADDHALF4_C_ROW  q5, r12, q0, r2, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
       ADDHALF4_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11					
	pop      	{r4 - r10, pc}

get_luma_c_4x4_ARMV7:	
	
	push     	{r4 - r10, r14}
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	sub			r0, r0, #2
	add         r5, r0, #8
	add			r7, r2, #8
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vext.8		q8,  q6,  q7,  #2	@;b0 ... b7 
	vext.8		q9,  q6,  q7,  #4	@;c0 ... c7
	vext.8		q10,  q6,  q7,  #6	@;d0 ... d7 
	vext.8		q11,  q6,  q7,  #8	@;e0 ... e7	
	vext.8		q12,  q6,  q7,  #10	@;f0 ... f7 
	vaddl.s16	q7,  d12,  d24		@;a0+f0 ... a3+f3
	vaddl.s16	q6,  d16,  d22		@;b0+e0 ... b3+e3
	vaddl.s16	q8,  d18,  d20		@;c0+d0 ... c3+d3
	vmov.i32    q9,  #5				@;coeff 5
	vmov.i32    q10,  #20			@;coeff 20	
	vmla.s32    q7,  q8,  q10  		@;a+20(c+d)+f 0-3
	vmls.s32    q7,  q6,  q9		@;a-5(b+e)+20(c+d)+f 0-3
	vqrshrun 	d20.u16,  q7.s32,  #10	@;0-7 solution
	vqmovn		d24.u8,  q10.u16	@;0-7 solution
	vst1.32     {d24[0]}, [r2],  r3
    HALF4_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	HALF4_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	HALF4_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3						
	pop      	{r4 - r10, pc}

add_luma_c_4x4_ARMV7:	
	
	push     	{r4 - r10, r14}
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	sub			r0, r0, #2
	add         r5, r0, #8
	add			r7, r2, #8
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vext.8		q8,  q6,  q7,  #2	@;b0 ... b7 
	vext.8		q9,  q6,  q7,  #4	@;c0 ... c7
	vext.8		q10,  q6,  q7,  #6	@;d0 ... d7 
	vext.8		q11,  q6,  q7,  #8	@;e0 ... e7	
	vext.8		q12,  q6,  q7,  #10	@;f0 ... f7 
	vaddl.s16	q7,  d12,  d24		@;a0+f0 ... a3+f3
	vaddl.s16	q6,  d16,  d22		@;b0+e0 ... b3+e3
	vaddl.s16	q8,  d18,  d20		@;c0+d0 ... c3+d3
	vmov.i32    q9,  #5				@;coeff 5
	vmov.i32    q10,  #20			@;coeff 20	
	vmla.s32    q7,  q8,  q10  		@;a+20(c+d)+f 0-3
	vmls.s32    q7,  q6,  q9		@;a-5(b+e)+20(c+d)+f 0-3
	vqrshrun 	d20.u16,  q7.s32,  #10	@;0-7 solution
	vqmovn		d24.u8,  q10.u16	@;0-7 solution
	vld1.32		{d25[0]},  [r2]
	vrhadd.u8  d24, d24, d25
	vst1.32     {d24[0]}, [r2],  r3
       ADDHALF4_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	ADDHALF4_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	ADDHALF4_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3						
	pop      	{r4 - r10, pc}

get_luma_v_16x16_ARMV7:
	push     	{r4 - r11, r14}
       mov         r5, #2
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vqrshrun 	d25.u8,  q7.s16,  #5@;8-15 solution
	vst1.64     {q12}, [r2],  r3
get_luma_v_16x16_loop:
	HALF16_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	HALF16_V_ROW  q1, r12, r2, r3, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	HALF16_V_ROW  q2, r12, r2, r3, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
	HALF16_V_ROW  q3, r12, r2, r3, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5
	HALF16_V_ROW  q4, r12, r2, r3, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
	HALF16_V_ROW  q5, r12, r2, r3, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
	subs        r5, r5, #1
       bne         get_luma_v_16x16_loop	
	HALF16_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	HALF16_V_ROW  q1, r12, r2, r3, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	HALF16_V_ROW  q2, r12, r2, r3, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	pop      	{r4 - r11, pc}	

get_luma_v_16x8_ARMV7:
	
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	vld1.64		{q0},  [r0], r1		@;src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @;r12 = src + 3*stride
	vld1.64		{q1},  [r0], r1		@;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@;coeff 5
	vld1.64		{q5},  [r12], r1	@;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r0], r1		@;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		@;a8+f8 ... a15+f15
	vld1.64		{q3},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{q4},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		@;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		@;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		@;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		@;a-5(b+e)+20(c+d)+f 8-15
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vqrshrun 	d25.u8,  q7.s16,  #5@;8-15 solution
	vst1.64     {q12}, [r2],  r3
	HALF16_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	HALF16_V_ROW  q1, r12, r2, r3, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	HALF16_V_ROW  q2, r12, r2, r3, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
	HALF16_V_ROW  q3, r12, r2, r3, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5
	HALF16_V_ROW  q4, r12, r2, r3, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
	HALF16_V_ROW  q5, r12, r2, r3, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
	HALF16_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	bx          lr	

get_luma_v_8x16_ARMV7:
	push     	{r4 - r11, r14}
       mov         r5, #2
	sub         r0, r0, r1, LSL #1  @src = src - 2*stride
	vld1.64		{d0},  [r0], r1		@src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @r12 = src + 3*stride
	vld1.64		{d2},  [r0], r1		@src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@coeff 5
	vld1.64		{d10},  [r12], r1	@src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{d4},  [r0], r1		@;src c0 c1 ... c15
	vld1.64		{d6},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vst1.64     {d24}, [r2],  r3
get_luma_v_8x16_loop:
	HALF8_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	HALF8_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	HALF8_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	HALF8_V_ROW  d6, r12, r2, r3, d0, d2, d8, d6, d10, d4
	HALF8_V_ROW  d8, r12, r2, r3, d2, d4, d10, d8, d0, d6
	HALF8_V_ROW  d10, r12, r2, r3, d4, d6, d0, d10, d2, d8
	subs        r5, r5, #1
       bne         get_luma_v_8x16_loop
	HALF8_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	HALF8_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	HALF8_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	pop      	{r4 - r11, pc}	

get_luma_v_8x8_ARMV7:
	
	sub         r0, r0, r1, LSL #1  @src = src - 2*stride
	vld1.64		{d0},  [r0], r1		@src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @r12 = src + 3*stride
	vld1.64		{d2},  [r0], r1		@src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@coeff 5
	vld1.64		{d10},  [r12], r1	@src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{d4},  [r0], r1		@;src c0 c1 ... c15
	vld1.64		{d6},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vst1.64     {d24}, [r2],  r3
	HALF8_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	HALF8_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	HALF8_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	HALF8_V_ROW  d6, r12, r2, r3, d0, d2, d8, d6, d10, d4
	HALF8_V_ROW  d8, r12, r2, r3, d2, d4, d10, d8, d0, d6
	HALF8_V_ROW  d10, r12, r2, r3, d4, d6, d0, d10, d2, d8
	HALF8_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	bx          lr	
	
get_luma_v_8x4_ARMV7:
	
	sub         r0, r0, r1, LSL #1  @src = src - 2*stride
	vld1.64		{d0},  [r0], r1		@src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @r12 = src + 3*stride
	vld1.64		{d2},  [r0], r1		@src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@coeff 5
	vld1.64		{d10},  [r12], r1	@src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{d4},  [r0], r1		@;src c0 c1 ... c15
	vld1.64		{d6},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vst1.64     {d24}, [r2],  r3
	HALF8_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	HALF8_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	HALF8_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	bx          lr

get_luma_v_4x8_ARMV7:
	
	sub         r0, r0, r1, LSL #1  @src = src - 2*stride
	vld1.64		{d0},  [r0], r1		@src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @r12 = src + 3*stride
	vld1.64		{d2},  [r0], r1		@src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@coeff 5
	vld1.64		{d10},  [r12], r1	@src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{d4},  [r0], r1		@;src c0 c1 ... c15
	vld1.64		{d6},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vst1.32     {d24[0]}, [r2],  r3
	HALF4_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	HALF4_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	HALF4_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	HALF4_V_ROW  d6, r12, r2, r3, d0, d2, d8, d6, d10, d4
	HALF4_V_ROW  d8, r12, r2, r3, d2, d4, d10, d8, d0, d6
	HALF4_V_ROW  d10, r12, r2, r3, d4, d6, d0, d10, d2, d8
	HALF4_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	bx          lr	
	
get_luma_v_4x4_ARMV7:
	
	sub         r0, r0, r1, LSL #1  @src = src - 2*stride
	vld1.64		{d0},  [r0], r1		@src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @r12 = src + 3*stride
	vld1.64		{d2},  [r0], r1		@src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@coeff 5
	vld1.64		{d10},  [r12], r1	@src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		@;a0+f0 ... a7+f7  	
	vld1.64		{d4},  [r0], r1		@;src c0 c1 ... c15
	vld1.64		{d6},  [r0], r1		@;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			@;coeff 20	
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vst1.32     {d24[0]}, [r2],  r3
	HALF4_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	HALF4_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	HALF4_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	bx          lr
	
get_luma_h_16x16_ARMV7:
	push     	{r4 - r11, r14}
       mov         r5, #4
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
get_luma_h_16x16_loop:
	HALF16_H_ROW r2, r3
	HALF16_H_ROW r2, r3
	HALF16_H_ROW r2, r3
	HALF16_H_ROW r2, r3
	subs        r5, r5, #1
       bne         get_luma_h_16x16_loop
	pop      	{r4 - r11, pc}

get_luma_h_16x8_ARMV7:
	push     	{r4 - r11, r14}
       mov         r5, #2
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
get_luma_h_16x8_loop:
	HALF16_H_ROW r2, r3
	HALF16_H_ROW r2, r3
	HALF16_H_ROW r2, r3
	HALF16_H_ROW r2, r3
	subs        r5, r5, #1
       bne         get_luma_h_16x8_loop
	pop      	{r4 - r11, pc}

get_luma_h_8x16_ARMV7:
	push     	{r4 - r11, r14}
       mov         r5, #4
	sub         r0, r0, #2  		@;src = src - 2
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
get_luma_h_8x16_loop:
	HALF8_H_ROW r2, r3
	HALF8_H_ROW r2, r3
	HALF8_H_ROW r2, r3
	HALF8_H_ROW r2, r3
	subs        r5, r5, #1
       bne         get_luma_h_8x16_loop
	pop      	{r4 - r11, pc}

get_luma_h_8x8_ARMV7:
	push     	{r4 - r11, r14}
       mov         r5, #2
	sub         r0, r0, #2  		@;src = src - 2
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
get_luma_h_8x8_loop:
	HALF8_H_ROW r2, r3
	HALF8_H_ROW r2, r3
	HALF8_H_ROW r2, r3
	HALF8_H_ROW r2, r3
	subs        r5, r5, #1
       bne         get_luma_h_8x8_loop
	pop      	{r4 - r11, pc}

get_luma_h_8x4_ARMV7:
	sub         r0, r0, #2  		@;src = src - 2
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20	
	HALF8_H_ROW r2, r3
	HALF8_H_ROW r2, r3
	HALF8_H_ROW r2, r3
	HALF8_H_ROW r2, r3
	bx          lr

get_luma_h_4x8_ARMV7:
	push     	{r4 - r11, r14}
       mov         r5, #2
	sub         r0, r0, #2  		@;src = src - 2
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
get_luma_h_4x8_loop:
	HALF4_H_ROW r2, r3
	HALF4_H_ROW r2, r3
	HALF4_H_ROW r2, r3
	HALF4_H_ROW r2, r3
	subs        r5, r5, #1
       bne         get_luma_h_4x8_loop
	pop      	{r4 - r11, pc}

get_luma_h_4x4_ARMV7:
	sub         r0, r0, #2  		@;src = src - 2
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20	
	HALF4_H_ROW r2, r3
	HALF4_H_ROW r2, r3
	HALF4_H_ROW r2, r3
	HALF4_H_ROW r2, r3
	bx          lr

MCCopy16x16_ARMV7:
	
	vld1.64		{d0, d1},  [r0], r1
	vld1.64		{d2, d3},  [r0], r1
	vld1.64		{d4, d5},  [r0], r1
	pld             [r0, r1, lsl #2]
	vld1.64		{d6, d7},  [r0], r1
	pld             [r0]        
	pld             [r0, r1]        
	pld             [r0, r1, lsl #1]
	vst1.64     {d0, d1}, [r2 ],  r3
	vst1.64     {d2, d3}, [r2 ],  r3	
	vst1.64     {d4, d5}, [r2 ],  r3
	vst1.64     {d6, d7}, [r2 ],  r3
	vld1.64		{d0, d1},  [r0], r1
	vld1.64		{d2, d3},  [r0], r1
	vld1.64		{d4, d5},  [r0], r1
	pld             [r0, r1, lsl #2]
	vld1.64		{d6, d7},  [r0], r1
	pld             [r0]        
	pld             [r0, r1]        
	pld             [r0, r1, lsl #1]
	vst1.64     {d0, d1}, [r2 ],  r3
	vst1.64     {d2, d3}, [r2 ],  r3	
	vst1.64     {d4, d5}, [r2 ],  r3
	vst1.64     {d6, d7}, [r2 ],  r3
	vld1.64		{d0, d1},  [r0], r1
	vld1.64		{d2, d3},  [r0], r1
	vld1.64		{d4, d5},  [r0], r1
	pld             [r0, r1, lsl #2]
	vld1.64		{d6, d7},  [r0], r1
	pld             [r0]        
	pld             [r0, r1]        
	pld             [r0, r1, lsl #1]
	vst1.64     {d0, d1}, [r2 ],  r3
	vst1.64     {d2, d3}, [r2 ],  r3	
	vst1.64     {d4, d5}, [r2 ],  r3
	vst1.64     {d6, d7}, [r2 ],  r3
	vld1.64		{d0, d1},  [r0], r1
	vld1.64		{d2, d3},  [r0], r1
	vld1.64		{d4, d5},  [r0], r1
	pld             [r0, r1, lsl #2]
	vld1.64		{d6, d7},  [r0], r1
	pld             [r0]        
	pld             [r0, r1]        
	pld             [r0, r1, lsl #1]
	vst1.64     {d0, d1}, [r2 ],  r3
	vst1.64     {d2, d3}, [r2 ],  r3	
	vst1.64     {d4, d5}, [r2 ],  r3
	vst1.64     {d6, d7}, [r2 ],  r3
	bx          lr

MCCopy16x8_ARMV7:

	vld1.64		{q0},  [r0], r1
	vld1.64		{q1},  [r0], r1
	vst1.64     {q0}, [r2 ],  r3
	vst1.64     {q1}, [r2 ],  r3
	vld1.64		{q0},  [r0], r1
	vld1.64		{q1},  [r0], r1
	vst1.64     {q0}, [r2 ],  r3
	vst1.64     {q1}, [r2 ],  r3
	vld1.64		{q0},  [r0], r1
	vld1.64		{q1},  [r0], r1
	vst1.64     {q0}, [r2 ],  r3
	vst1.64     {q1}, [r2 ],  r3
	vld1.64		{q0},  [r0], r1
	vld1.64		{q1},  [r0], r1
	vst1.64     {q0}, [r2 ],  r3
	vst1.64     {q1}, [r2 ],  r3
	bx          lr	
	
MCCopy8x16_ARMV7:
	
	vld1.64		{d0},  [r0], r1
	vld1.64		{d1},  [r0], r1
	vst1.64     {d0}, [r2],  r3
	vst1.64     {d1}, [r2],  r3
	vld1.64		{d0},  [r0], r1
	vld1.64		{d1},  [r0], r1
	vst1.64     {d0}, [r2],  r3
	vst1.64     {d1}, [r2],  r3
	vld1.64		{d0},  [r0], r1
	vld1.64		{d1},  [r0], r1
	vst1.64     {d0}, [r2],  r3
	vst1.64     {d1}, [r2],  r3
	vld1.64		{d0},  [r0], r1
	vld1.64		{d1},  [r0], r1
	vst1.64     {d0}, [r2],  r3
	vst1.64     {d1}, [r2],  r3
	vld1.64		{d0},  [r0], r1
	vld1.64		{d1},  [r0], r1
	vst1.64     {d0}, [r2],  r3
	vst1.64     {d1}, [r2],  r3
	vld1.64		{d0},  [r0], r1
	vld1.64		{d1},  [r0], r1
	vst1.64     {d0}, [r2],  r3
	vst1.64     {d1}, [r2],  r3
	vld1.64		{d0},  [r0], r1
	vld1.64		{d1},  [r0], r1
	vst1.64     {d0}, [r2],  r3
	vst1.64     {d1}, [r2],  r3
	vld1.64		{d0},  [r0], r1
	vld1.64		{d1},  [r0], r1
	vst1.64     {d0}, [r2],  r3
	vst1.64     {d1}, [r2],  r3
	bx          lr

	
MCCopy8x8_ARMV7:
	
	vld1.64		{d0},  [r0], r1
	vld1.64		{d1},  [r0], r1
	vld1.64		{d2},  [r0], r1
	vld1.64		{d3},  [r0], r1
	vst1.64     {d0}, [r2],  r3
	vst1.64     {d1}, [r2],  r3	
	vst1.64     {d2}, [r2],  r3
	vst1.64     {d3}, [r2],  r3
	vld1.64		{d0},  [r0], r1
	vld1.64		{d1},  [r0], r1
	vld1.64		{d2},  [r0], r1
	vld1.64		{d3},  [r0], r1
	vst1.64     {d0}, [r2],  r3
	vst1.64     {d1}, [r2],  r3	
	vst1.64     {d2}, [r2],  r3
	vst1.64     {d3}, [r2],  r3
	bx          lr

MCCopy8x4_ARMV7:
	
	vld1.64		{d0},  [r0], r1
	vld1.64		{d1},  [r0], r1
	vst1.64     {d0}, [r2],  r3
	vst1.64     {d1}, [r2],  r3
	vld1.64		{d0},  [r0], r1
	vld1.64		{d1},  [r0], r1
	vst1.64     {d0}, [r2],  r3
	vst1.64     {d1}, [r2],  r3
	bx          lr

MCCopy4x8_ARMV7:
	
	vld1.32		{d0[0]},  [r0], r1
	vld1.32		{d1[0]},  [r0], r1
	vst1.32     {d0[0]}, [r2],  r3
	vst1.32     {d1[0]}, [r2],  r3
	vld1.32		{d0[0]},  [r0], r1
	vld1.32		{d1[0]},  [r0], r1
	vst1.32     {d0[0]}, [r2],  r3
	vst1.32     {d1[0]}, [r2],  r3
	vld1.32		{d0[0]},  [r0], r1
	vld1.32		{d1[0]},  [r0], r1
	vst1.32     {d0[0]}, [r2],  r3
	vst1.32     {d1[0]}, [r2],  r3
	vld1.32		{d0[0]},  [r0], r1
	vld1.32		{d1[0]},  [r0], r1
	vst1.32     {d0[0]}, [r2],  r3
	vst1.32     {d1[0]}, [r2],  r3
	bx          lr
	
MCCopy4x4_ARMV7:
	
	vld1.32		{d0[0]},  [r0], r1
	vld1.32		{d1[0]},  [r0], r1
	vst1.32     {d0[0]}, [r2],  r3
	vst1.32     {d1[0]}, [r2],  r3
	vld1.32		{d0[0]},  [r0], r1
	vld1.32		{d1[0]},  [r0], r1
	vst1.32     {d0[0]}, [r2],  r3
	vst1.32     {d1[0]}, [r2],  r3
	bx          lr
	
MCCopy4x2_ARMV7:
	
	vld1.32		{d0[0]},  [r0], r1
	vld1.32		{d1[0]},  [r0], r1
	vst1.32     {d0[0]}, [r2],  r3
	vst1.32     {d1[0]}, [r2],  r3
	bx          lr

MCCopy2x4_ARMV7:
	
	vld1.16	{d0[0]},  [r0], r1
	vld1.16	{d0[1]},  [r0], r1
	vld1.16	{d0[2]},  [r0], r1
	vld1.16	{d0[3]},  [r0], r1	
	vst1.16	{d0[0]},  [r2], r3
	vst1.16	{d0[1]},  [r2], r3
	vst1.16	{d0[2]},  [r2], r3
	vst1.16	{d0[3]},  [r2], r3
	bx          lr

MCCopy2x2_ARMV7:
	
	vld1.16	{d0[0]},  [r0], r1
	vld1.16	{d0[1]},  [r0], r1	
	vst1.16	{d0[0]},  [r2], r3
	vst1.16	{d0[1]},  [r2], r3
	bx          lr
	
MCChroma8x8_ARMV7:
	push     	{r4 - r11, r14}
	ldr  		r4, [sp, #36]   
	ldr  		r5, [sp, #40]   
	ldr  		r6, [sp, #44]   
	ldr  		r7, [sp, #48]
	mov         r8, #2
	vdup.8     d24,  r4	
	vdup.8     d25,  r6
	vdup.8     d26,  r5
	vdup.8     d27,  r7
	vld1.64		{q0},  [r0], r1
	vext.8      d1, d0, d1, #1
MCChroma8x8_loop:
	MCChroma8_ROW q1, d2, d3, d0, d1
	MCChroma8_ROW q0, d0, d1, d2, d3
	MCChroma8_ROW q1, d2, d3, d0, d1
	MCChroma8_ROW q0, d0, d1, d2, d3
	subs        r8, r8, #1
       bne         MCChroma8x8_loop	
	pop      	{r4 - r11, pc}

ADDMCChroma8x8_ARMV7:
	push     	{r4 - r11, r14}
	ldr  		r4, [sp, #36]   
	ldr  		r5, [sp, #40]   
	ldr  		r6, [sp, #44]   
	ldr  		r7, [sp, #48]
	mov         r8, #2
	vdup.8     d24,  r4	
	vdup.8     d25,  r6
	vdup.8     d26,  r5
	vdup.8     d27,  r7
	vld1.64		{q0},  [r0], r1
	vext.8      d1, d0, d1, #1
ADDMCChroma8x8_loop:
	ADDMCChroma8_ROW q1, d2, d3, d0, d1
	ADDMCChroma8_ROW q0, d0, d1, d2, d3
	ADDMCChroma8_ROW q1, d2, d3, d0, d1
	ADDMCChroma8_ROW q0, d0, d1, d2, d3
	subs        r8, r8, #1
       bne         ADDMCChroma8x8_loop	
	pop      	{r4 - r11, pc}

MCChroma8x4_ARMV7:
	push     	{r4 - r11, r14}
	ldr  		r4, [sp, #36]   
	ldr  		r5, [sp, #40]   
	ldr  		r6, [sp, #44]   
	ldr  		r7, [sp, #48]
	vdup.8     d24,  r4	
	vdup.8     d25,  r6
	vdup.8     d26,  r5
	vdup.8     d27,  r7
	vld1.64		{q0},  [r0], r1
	vext.8      d1, d0, d1, #1
	MCChroma8_ROW q1, d2, d3, d0, d1
	MCChroma8_ROW q0, d0, d1, d2, d3
	MCChroma8_ROW q1, d2, d3, d0, d1
	MCChroma8_ROW q0, d0, d1, d2, d3	
	pop      	{r4 - r11, pc}	

ADDMCChroma8x4_ARMV7:
	push     	{r4 - r11, r14}
	ldr  		r4, [sp, #36]   
	ldr  		r5, [sp, #40]   
	ldr  		r6, [sp, #44]   
	ldr  		r7, [sp, #48]
	vdup.8     d24,  r4	
	vdup.8     d25,  r6
	vdup.8     d26,  r5
	vdup.8     d27,  r7
	vld1.64		{q0},  [r0], r1
	vext.8      d1, d0, d1, #1
	ADDMCChroma8_ROW q1, d2, d3, d0, d1
	ADDMCChroma8_ROW q0, d0, d1, d2, d3
	ADDMCChroma8_ROW q1, d2, d3, d0, d1
	ADDMCChroma8_ROW q0, d0, d1, d2, d3	
	pop      	{r4 - r11, pc}	

MCChroma4x8_ARMV7:
	push     	{r4 - r11, r14}
	ldr  		r4, [sp, #36]   
	ldr  		r5, [sp, #40]   
	ldr  		r6, [sp, #44]   
	ldr  		r7, [sp, #48]
	mov         r8, #2
	vdup.8     d24,  r4	
	vdup.8     d25,  r6
	vdup.8     d26,  r5
	vdup.8     d27,  r7
	vld1.64		{d0},  [r0], r1
	vext.8      d1, d0, d1, #1
MCChroma4x8_loop:
	MCChroma4_ROW d2, d2, d3, d0, d1
	MCChroma4_ROW d0, d0, d1, d2, d3
	MCChroma4_ROW d2, d2, d3, d0, d1
	MCChroma4_ROW d0, d0, d1, d2, d3
	subs        r8, r8, #1
       bne         MCChroma4x8_loop	
	pop      	{r4 - r11, pc}

ADDMCChroma4x8_ARMV7:
	push     	{r4 - r11, r14}
	ldr  		r4, [sp, #36]   
	ldr  		r5, [sp, #40]   
	ldr  		r6, [sp, #44]   
	ldr  		r7, [sp, #48]
	mov         r8, #2
	vdup.8     d24,  r4	
	vdup.8     d25,  r6
	vdup.8     d26,  r5
	vdup.8     d27,  r7
	vld1.64		{d0},  [r0], r1
	vext.8      d1, d0, d1, #1
ADDMCChroma4x8_loop:
	ADDMCChroma4_ROW d2, d2, d3, d0, d1
	ADDMCChroma4_ROW d0, d0, d1, d2, d3
	ADDMCChroma4_ROW d2, d2, d3, d0, d1
	ADDMCChroma4_ROW d0, d0, d1, d2, d3
	subs        r8, r8, #1
       bne         ADDMCChroma4x8_loop	
	pop      	{r4 - r11, pc}

MCChroma4x4_ARMV7:
	push     	{r4 - r11, r14}
	ldr  		r4, [sp, #36]   
	ldr  		r5, [sp, #40]   
	ldr  		r6, [sp, #44]   
	ldr  		r7, [sp, #48]
	vdup.8     d24,  r4	
	vdup.8     d25,  r6
	vdup.8     d26,  r5
	vdup.8     d27,  r7
	vld1.64		{d0},  [r0], r1
	vext.8      d1, d0, d1, #1
	MCChroma4_ROW d2, d2, d3, d0, d1
	MCChroma4_ROW d0, d0, d1, d2, d3
	MCChroma4_ROW d2, d2, d3, d0, d1
	MCChroma4_ROW d0, d0, d1, d2, d3	
	pop      	{r4 - r11, pc}	

ADDMCChroma4x4_ARMV7:
	push     	{r4 - r11, r14}
	ldr  		r4, [sp, #36]   
	ldr  		r5, [sp, #40]   
	ldr  		r6, [sp, #44]   
	ldr  		r7, [sp, #48]
	vdup.8     d24,  r4	
	vdup.8     d25,  r6
	vdup.8     d26,  r5
	vdup.8     d27,  r7
	vld1.64		{d0},  [r0], r1
	vext.8      d1, d0, d1, #1
	ADDMCChroma4_ROW d2, d2, d3, d0, d1
	ADDMCChroma4_ROW d0, d0, d1, d2, d3
	ADDMCChroma4_ROW d2, d2, d3, d0, d1
	ADDMCChroma4_ROW d0, d0, d1, d2, d3	
	pop      	{r4 - r11, pc}	

MCChroma4x2_ARMV7:
	push     	{r4 - r11, r14}
	ldr  		r4, [sp, #36]   
	ldr  		r5, [sp, #40]   
	ldr  		r6, [sp, #44]   
	ldr  		r7, [sp, #48]
	vdup.8     d24,  r4	
	vdup.8     d25,  r6
	vdup.8     d26,  r5
	vdup.8     d27,  r7
	vld1.64		{d0},  [r0], r1
	vext.8      d1, d0, d1, #1
	MCChroma4_ROW d2, d2, d3, d0, d1
	MCChroma4_ROW d0, d0, d1, d2, d3
	pop      	{r4 - r11, pc}	

ADDMCChroma4x2_ARMV7:
	push     	{r4 - r11, r14}
	ldr  		r4, [sp, #36]   
	ldr  		r5, [sp, #40]   
	ldr  		r6, [sp, #44]   
	ldr  		r7, [sp, #48]
	vdup.8     d24,  r4	
	vdup.8     d25,  r6
	vdup.8     d26,  r5
	vdup.8     d27,  r7
	vld1.64		{d0},  [r0], r1
	vext.8      d1, d0, d1, #1
	ADDMCChroma4_ROW d2, d2, d3, d0, d1
	ADDMCChroma4_ROW d0, d0, d1, d2, d3
	pop      	{r4 - r11, pc}	

MCChroma2x4_ARMV7:
	push     	{r4 - r11, r14}
	ldr  		r4, [sp, #36]   
	ldr  		r5, [sp, #40]   
	ldr  		r6, [sp, #44]   
	ldr  		r7, [sp, #48]
	vdup.8     d24,  r4	
	vdup.8     d25,  r5
	vdup.8     d26,  r6
	vdup.8     d27,  r7
	vld1.64		{d0},  [r0], r1
	vld1.64		{d1},  [r0], r1
	vld1.64		{d2},  [r0], r1
	vld1.64		{d3},  [r0], r1
	vld1.64		{d4},  [r0], r1
	vext.8      q4, q0, q4, #1
	vext.8      q5, q1, q5, #1
	vext.8      d5, d4, d5, #1
	vtrn.16	 d0, d1
	vtrn.16	 d2, d3
	vtrn.32	 d0, d2                     @d0 00
       vext.8      d1, d0, d4, #2         @d1 01 
	vtrn.16	 d8, d9
	vtrn.16	 d10, d11
	vtrn.32	 d8, d10                   @d8 10
	vext.8      d11, d8, d5, #2        @d11 11
	vmull.u8   q6, d0, d24
	vmull.u8   q1, d1, d25
	vmull.u8   q2, d8, d26
	vmull.u8   q3, d11, d27
	vadd.u16	q6, q1
	vadd.u16	q6, q2
	vadd.u16	q6, q3
	vqrshrun.s16 	d16,  q6,  #6	@;0-7 solution
	vst1.16     {d16[0]}, [r2],  r3
	vst1.16     {d16[1]}, [r2],  r3
	vst1.16     {d16[2]}, [r2],  r3
	vst1.16     {d16[3]}, [r2],  r3	
	pop      	{r4 - r11, pc}	

ADDMCChroma2x4_ARMV7:
	push     	{r4 - r11, r14}
	ldr  		r4, [sp, #36]   
	ldr  		r5, [sp, #40]   
	ldr  		r6, [sp, #44]   
	ldr  		r7, [sp, #48]
	vdup.8     d24,  r4	
	vdup.8     d25,  r5
	vdup.8     d26,  r6
	vdup.8     d27,  r7
	vld1.64		{d0},  [r0], r1
	vld1.64		{d1},  [r0], r1
	vld1.64		{d2},  [r0], r1
	vld1.64		{d3},  [r0], r1
	vld1.64		{d4},  [r0], r1
	vext.8      q4, q0, q4, #1
	vext.8      q5, q1, q5, #1
	vext.8      d5, d4, d5, #1
	vtrn.16	 d0, d1
	vtrn.16	 d2, d3
	vtrn.32	 d0, d2                     @d0 00
       vext.8      d1, d0, d4, #2         @d1 01 
	vtrn.16	 d8, d9
	vtrn.16	 d10, d11
	vtrn.32	 d8, d10                   @d8 10
	vext.8      d11, d8, d5, #2        @d11 11
	vmull.u8   q6, d0, d24
	vmull.u8   q1, d1, d25
	vmull.u8   q2, d8, d26
	vmull.u8   q3, d11, d27
	vadd.u16	q6, q1
	vadd.u16	q6, q2
	vadd.u16	q6, q3
	mov         r5, r2
	vld1.16     {d17[0]}, [r5],  r3
	vld1.16     {d17[1]}, [r5],  r3
	vld1.16     {d17[2]}, [r5],  r3
	vld1.16     {d17[3]}, [r5],  r3
	vqrshrun.s16 	d16,  q6,  #6	@;0-7 solution
	vrhadd.u8  d16, d16, d17
	vst1.16     {d16[0]}, [r2],  r3
	vst1.16     {d16[1]}, [r2],  r3
	vst1.16     {d16[2]}, [r2],  r3
	vst1.16     {d16[3]}, [r2],  r3	
	pop      	{r4 - r11, pc}	

MCChroma2x2_ARMV7:
	push     	{r4 - r11, r14}
	ldr  		r4, [sp, #36]   
	ldr  		r5, [sp, #40]   
	ldr  		r6, [sp, #44]   
	ldr  		r7, [sp, #48]
	vdup.8     d24,  r4	
	vdup.8     d25,  r5
	vdup.8     d26,  r6
	vdup.8     d27,  r7
	vld1.64		{d0},  [r0], r1
	vld1.64		{d1},  [r0], r1
	vld1.64		{d2},  [r0], r1
	vext.8      q4, q0, q4, #1
	vext.8      d5, d2, d3, #1
	vtrn.16	 d0, d1
	vtrn.32	 d0, d2			@d0 00
       vext.8      d1, d0, d2, #2         @d1 01 
	vtrn.16	 d8, d9 
	vtrn.32	 d8, d5			@d8 10
	vext.8      d11, d8, d5, #2        @d11 11
	vmull.u8   q6, d0, d24
	vmull.u8   q1, d1, d25
	vmull.u8   q2, d8, d26
	vmull.u8   q3, d11, d27
	vadd.u16	q6, q1
	vadd.u16	q6, q2
	vadd.u16	q6, q3
	vqrshrun.s16 	d16,  q6,  #6	@;0-7 solution
	vst1.16     {d16[0]}, [r2],  r3
	vst1.16     {d16[1]}, [r2],  r3
	pop      	{r4 - r11, pc}	

ADDMCChroma2x2_ARMV7:
	push     	{r4 - r11, r14}
	ldr  		r4, [sp, #36]   
	ldr  		r5, [sp, #40]   
	ldr  		r6, [sp, #44]   
	ldr  		r7, [sp, #48]
	vdup.8     d24,  r4	
	vdup.8     d25,  r5
	vdup.8     d26,  r6
	vdup.8     d27,  r7
	vld1.64		{d0},  [r0], r1
	vld1.64		{d1},  [r0], r1
	vld1.64		{d2},  [r0], r1
	vext.8      q4, q0, q4, #1
	vext.8      d5, d2, d3, #1
	vtrn.16	 d0, d1
	vtrn.32	 d0, d2			@d0 00
       vext.8      d1, d0, d2, #2         @d1 01 
	vtrn.16	 d8, d9 
	vtrn.32	 d8, d5			@d8 10
	vext.8      d11, d8, d5, #2        @d11 11
	vmull.u8   q6, d0, d24
	vmull.u8   q1, d1, d25
	vmull.u8   q2, d8, d26
	vmull.u8   q3, d11, d27
	vadd.u16	q6, q1
	vadd.u16	q6, q2
	vadd.u16	q6, q3
	mov         r5, r2
	vld1.16     {d17[0]}, [r5],  r3
	vld1.16     {d17[1]}, [r5],  r3
	vqrshrun.s16 	d16,  q6,  #6	@;0-7 solution
	vrhadd.u8  d16, d16, d17
	vst1.16     {d16[0]}, [r2],  r3
	vst1.16     {d16[1]}, [r2],  r3
	pop      	{r4 - r11, pc}	

AVG16x16_ARMV7:
	push     	{r4 - r11, r14}
	mov			r5, #4
AVG16x16_loop:
	vld1.64	{q0},  [r0 ]
	vld1.64	{q1},  [r2], r3
	vld1.64	{q2},  [r2], r3
	vrhadd.u8  q0, q0, q1
	vst1.64     {q0}, [r0 ],  r1
	vld1.64	{q0},  [r0 ]
	vld1.64	{q1},  [r2], r3
	vrhadd.u8  q0, q0, q2
	vst1.64     {q0}, [r0 ],  r1
	vld1.64	{q0},  [r0 ]
	vld1.64	{q2},  [r2], r3
	vrhadd.u8  q0, q0, q1
	vst1.64     {q0}, [r0 ],  r1
	vld1.64	{q0},  [r0 ]
	vrhadd.u8  q0, q0, q2
	vst1.64     {q0}, [r0 ],  r1
	subs        r5, r5, #1
       bne         AVG16x16_loop
	pop      	{r4 - r11, pc}	

AVG16x8_ARMV7:
	push     	{r4 - r11, r14}
	mov			r5, #2
AVG16x8_loop:
	vld1.64	{q0},  [r0 ]
	vld1.64	{q1},  [r2], r3
	vld1.64	{q2},  [r2], r3
	vrhadd.u8  q0, q0, q1
	vst1.64     {q0}, [r0 ],  r1
	vld1.64	{q0},  [r0]
	vld1.64	{q1},  [r2], r3
	vrhadd.u8  q0, q0, q2
	vst1.64     {q0}, [r0 ],  r1
	vld1.64	{q0},  [r0 ]
	vld1.64	{q2},  [r2], r3
	vrhadd.u8  q0, q0, q1
	vst1.64     {q0}, [r0 ],  r1
	vld1.64	{q0},  [r0 ]
	vrhadd.u8  q0, q0, q2
	vst1.64     {q0}, [r0 ],  r1
	subs        r5, r5, #1
       bne         AVG16x8_loop
	pop      	{r4 - r11, pc}	

AVG8x16_ARMV7:
	push     	{r4 - r11, r14}
	mov			r4, r0
	mov			r5, #4
AVG8x16_loop:
	vld1.64	{d0},  [r0], r1
	vld1.64	{d1},  [r0], r1
	vld1.64	{d2},  [r2], r3
	vld1.64	{d3},  [r2], r3
	vrhadd.u8  q0, q0, q1
	vst1.64	{d0},  [r4], r1
	vst1.64	{d1},  [r4], r1
	vld1.64	{d0},  [r0], r1
	vld1.64	{d1},  [r0], r1
	vld1.64	{d2},  [r2], r3
	vld1.64	{d3},  [r2], r3
	vrhadd.u8  q0, q0, q1
	vst1.64	{d0},  [r4], r1
	vst1.64	{d1},  [r4], r1
	subs        r5, r5, #1
       bne         AVG8x16_loop
	pop      	{r4 - r11, pc}

AVG8x8_ARMV7:
	push     	{r4 - r11, r14}
	mov			r4, r0
	mov			r5, #2
AVG8x8_loop:
	vld1.64	{d0},  [r0], r1
	vld1.64	{d1},  [r0], r1
	vld1.64	{d2},  [r2], r3
	vld1.64	{d3},  [r2], r3
	vrhadd.u8  q0, q0, q1
	vst1.64	{d0},  [r4], r1
	vst1.64	{d1},  [r4], r1
	vld1.64	{d0},  [r0], r1
	vld1.64	{d1},  [r0], r1
	vld1.64	{d2},  [r2], r3
	vld1.64	{d3},  [r2], r3
	vrhadd.u8  q0, q0, q1
	vst1.64	{d0},  [r4], r1
	vst1.64	{d1},  [r4], r1
	subs        r5, r5, #1
       bne         AVG8x8_loop
	pop      	{r4 - r11, pc}

AVG8x4_ARMV7:
	push     	{r4 - r11, r14}
	mov			r4, r0
	vld1.64	{d0},  [r0], r1
	vld1.64	{d1},  [r0], r1
	vld1.64	{d2},  [r2], r3
	vld1.64	{d3},  [r2], r3
	vrhadd.u8  q0, q0, q1
	vst1.64	{d0},  [r4], r1
	vst1.64	{d1},  [r4], r1
	vld1.64	{d0},  [r0], r1
	vld1.64	{d1},  [r0], r1
	vld1.64	{d2},  [r2], r3
	vld1.64	{d3},  [r2], r3
	vrhadd.u8  q0, q0, q1
	vst1.64	{d0},  [r4], r1
	vst1.64	{d1},  [r4], r1
	pop      	{r4 - r11, pc}

AVG4x8_ARMV7:
	push     	{r4 - r11, r14}
	mov			r4, r0
	vld1.32	{d0[0]},  [r0], r1
	vld1.32	{d0[1]},  [r0], r1
	vld1.32	{d1[0]},  [r0], r1
	vld1.32	{d1[1]},  [r0], r1
	vld1.32	{d2[0]},  [r2], r3
	vld1.32	{d2[1]},  [r2], r3
	vld1.32	{d3[0]},  [r2], r3
	vld1.32	{d3[1]},  [r2], r3
	vrhadd.u8  q0, q0, q1
	vst1.32	{d0[0]},  [r4], r1
	vst1.32	{d0[1]},  [r4], r1
	vst1.32	{d1[0]},  [r4], r1
	vst1.32	{d1[1]},  [r4], r1
	vld1.32	{d0[0]},  [r0], r1
	vld1.32	{d0[1]},  [r0], r1
	vld1.32	{d1[0]},  [r0], r1
	vld1.32	{d1[1]},  [r0], r1
	vld1.32	{d2[0]},  [r2], r3
	vld1.32	{d2[1]},  [r2], r3
	vld1.32	{d3[0]},  [r2], r3
	vld1.32	{d3[1]},  [r2], r3
	vrhadd.u8  q0, q0, q1
	vst1.32	{d0[0]},  [r4], r1
	vst1.32	{d0[1]},  [r4], r1
	vst1.32	{d1[0]},  [r4], r1
	vst1.32	{d1[1]},  [r4], r1
	pop      	{r4 - r11, pc}

AVG4x4_ARMV7:
	push     	{r4 - r11, r14}
	mov			r4, r0
	vld1.32	{d0[0]},  [r0], r1
	vld1.32	{d0[1]},  [r0], r1
	vld1.32	{d1[0]},  [r0], r1
	vld1.32	{d1[1]},  [r0], r1
	vld1.32	{d2[0]},  [r2], r3
	vld1.32	{d2[1]},  [r2], r3
	vld1.32	{d3[0]},  [r2], r3
	vld1.32	{d3[1]},  [r2], r3
	vrhadd.u8  q0, q0, q1
	vst1.32	{d0[0]},  [r4], r1
	vst1.32	{d0[1]},  [r4], r1
	vst1.32	{d1[0]},  [r4], r1
	vst1.32	{d1[1]},  [r4], r1
	pop      	{r4 - r11, pc}

AVG4x2_ARMV7:
	push     	{r4 - r11, r14}
	mov			r4, r0
	vld1.32	{d0[0]},  [r0], r1
	vld1.32	{d0[1]},  [r0], r1
	vld1.32	{d2[0]},  [r2], r3
	vld1.32	{d2[1]},  [r2], r3
	vrhadd.u8  d0, d0, d2
	vst1.32	{d0[0]},  [r4], r1
	vst1.32	{d0[1]},  [r4], r1
	pop      	{r4 - r11, pc}

AVG2x4_ARMV7:
	push     	{r4 - r11, r14}
	mov			r4, r0
	vld1.16	{d0[0]},  [r0], r1
	vld1.16	{d0[1]},  [r0], r1
	vld1.16	{d0[2]},  [r0], r1
	vld1.16	{d0[3]},  [r0], r1
	vld1.16	{d2[0]},  [r2], r3
	vld1.16	{d2[1]},  [r2], r3
	vld1.16	{d2[2]},  [r2], r3
	vld1.16	{d2[3]},  [r2], r3
	vrhadd.u8  d0, d0, d2
	vst1.16	{d0[0]},  [r4], r1
	vst1.16	{d0[1]},  [r4], r1
	vst1.16	{d0[2]},  [r4], r1
	vst1.16	{d0[3]},  [r4], r1
	pop      	{r4 - r11, pc}

AVG2x2_ARMV7:
	push     	{r4 - r11, r14}
	mov			r4, r0
	vld1.16	{d0[0]},  [r0], r1
	vld1.16	{d0[1]},  [r0], r1
	vld1.16	{d2[0]},  [r2], r3
	vld1.16	{d2[1]},  [r2], r3
	vrhadd.u8  d0, d0, d2
	vst1.16	{d0[0]},  [r4], r1
	vst1.16	{d0[1]},  [r4], r1
	pop      	{r4 - r11, pc}
	
offset_mc_prediction_16x16_ARMV7:
	vdup.16     q14,  r2
	mov		  r3, #4
offset_16x16_loop:
	vld1.64	  {q0},  [r0]
	vaddw.u8  q1, q14, d0
	vaddw.u8  q2, q14, d1
	vqmovun.s16     d6, q1
  	vqmovun.s16     d7, q2
	vst1.64     {q3}, [r0],  r1
	vld1.64	  {q0},  [r0]
	vaddw.u8  q1, q14, d0
	vaddw.u8  q2, q14, d1
	vqmovun.s16     d6, q1
  	vqmovun.s16     d7, q2
	vst1.64     {q3}, [r0],  r1
	vld1.64	  {q0},  [r0]
	vaddw.u8  q1, q14, d0
	vaddw.u8  q2, q14, d1
	vqmovun.s16     d6, q1
  	vqmovun.s16     d7, q2
	vst1.64     {q3}, [r0],  r1
	vld1.64	  {q0},  [r0]
	vaddw.u8  q1, q14, d0
	vaddw.u8  q2, q14, d1
	vqmovun.s16     d6, q1
  	vqmovun.s16     d7, q2
	vst1.64     {q3}, [r0],  r1
	subs        r3, r3, #1
       bne         offset_16x16_loop		
	bx          lr

offset_mc_prediction_16x8_ARMV7:
	vdup.16     q14,  r2
	mov		  r3, #2
offset_16x8_loop:
	vld1.64	  {q0},  [r0]
	vaddw.u8  q1, q14, d0
	vaddw.u8  q2, q14, d1
	vqmovun.s16     d6, q1
  	vqmovun.s16     d7, q2
	vst1.64     {q3}, [r0],  r1
	vld1.64	  {q0},  [r0]
	vaddw.u8  q1, q14, d0
	vaddw.u8  q2, q14, d1
	vqmovun.s16     d6, q1
  	vqmovun.s16     d7, q2
	vst1.64     {q3}, [r0],  r1
	vld1.64	  {q0},  [r0]
	vaddw.u8  q1, q14, d0
	vaddw.u8  q2, q14, d1
	vqmovun.s16     d6, q1
  	vqmovun.s16     d7, q2
	vst1.64     {q3}, [r0],  r1
	vld1.64	  {q0},  [r0]
	vaddw.u8  q1, q14, d0
	vaddw.u8  q2, q14, d1
	vqmovun.s16     d6, q1
  	vqmovun.s16     d7, q2
	vst1.64     {q3}, [r0],  r1
	subs        r3, r3, #1
       bne         offset_16x8_loop		
	bx          lr

offset_mc_prediction_8x16_ARMV7:
	vdup.16     q14,  r2
	mov		  r3, #4
offset_8x16_loop:
	vld1.64	  {d0},  [r0]
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d2, q1
	vst1.64     {d2}, [r0],  r1
	vld1.64	  {d0},  [r0]
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d2, q1
	vst1.64     {d2}, [r0],  r1
	vld1.64	  {d0},  [r0]
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d2, q1
	vst1.64     {d2}, [r0],  r1
	vld1.64	  {d0},  [r0]
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d2, q1
	vst1.64     {d2}, [r0],  r1
	subs        r3, r3, #1
       bne         offset_8x16_loop		
	bx          lr

offset_mc_prediction_8x8_ARMV7:
	vdup.16     q14,  r2
	mov		  r3, #2
offset_8x8_loop:
	vld1.64	  {d0},  [r0]
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d2, q1
	vst1.64     {d2}, [r0],  r1
	vld1.64	  {d0},  [r0]
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d2, q1
	vst1.64     {d2}, [r0],  r1
	vld1.64	  {d0},  [r0]
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d2, q1
	vst1.64     {d2}, [r0],  r1
	vld1.64	  {d0},  [r0]
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d2, q1
	vst1.64     {d2}, [r0],  r1
	subs        r3, r3, #1
       bne         offset_8x8_loop		
	bx          lr

offset_mc_prediction_8x4_ARMV7:
	vdup.16     q14,  r2
	vld1.64	  {d0},  [r0]
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d2, q1
	vst1.64     {d2}, [r0],  r1
	vld1.64	  {d0},  [r0]
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d2, q1
	vst1.64     {d2}, [r0],  r1
	vld1.64	  {d0},  [r0]
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d2, q1
	vst1.64     {d2}, [r0],  r1
	vld1.64	  {d0},  [r0]
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d2, q1
	vst1.64     {d2}, [r0],  r1		
	bx          lr

offset_mc_prediction_4x8_ARMV7:
	vdup.16     q14,  r2
	mov		  r3, r0
	vld1.32	  {d0[0]},  [r0], r1
	vld1.32	  {d0[1]},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d2, q1
	vst1.32     {d2[0]}, [r3],  r1
	vst1.32     {d2[1]}, [r3],  r1
	vld1.32	  {d0[0]},  [r0], r1
	vld1.32	  {d0[1]},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d2, q1
	vst1.32     {d2[0]}, [r3],  r1
	vst1.32     {d2[1]}, [r3],  r1	
	vld1.32	  {d0[0]},  [r0], r1
	vld1.32	  {d0[1]},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d2, q1
	vst1.32     {d2[0]}, [r3],  r1
	vst1.32     {d2[1]}, [r3],  r1
	vld1.32	  {d0[0]},  [r0], r1
	vld1.32	  {d0[1]},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d2, q1
	vst1.32     {d2[0]}, [r3],  r1
	vst1.32     {d2[1]}, [r3],  r1	
	bx          lr

offset_mc_prediction_4x4_ARMV7:
	vdup.16     q14,  r2
	mov		  r3, r0
	vld1.32	  {d0[0]},  [r0], r1
	vld1.32	  {d0[1]},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d2, q1
	vst1.32     {d2[0]}, [r3],  r1
	vst1.32     {d2[1]}, [r3],  r1
	vld1.32	  {d0[0]},  [r0], r1
	vld1.32	  {d0[1]},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d2, q1
	vst1.32     {d2[0]}, [r3],  r1
	vst1.32     {d2[1]}, [r3],  r1	
	bx          lr

offset_mc_prediction_4x2_ARMV7:
	vdup.16     q14,  r2
	mov		  r3, r0
	vld1.32	  {d0[0]},  [r0], r1
	vld1.32	  {d0[1]},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d2, q1
	vst1.32     {d2[0]}, [r3],  r1
	vst1.32     {d2[1]}, [r3],  r1
	bx          lr

offset_mc_prediction_2x4_ARMV7:
	vdup.16     q14,  r2
	mov		  r3, r0
	vld1.16	  {d0[0]},  [r0], r1
	vld1.16	  {d0[1]},  [r0], r1
	vld1.16	  {d0[2]},  [r0], r1
	vld1.16	  {d0[3]},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d2, q1
	vst1.16     {d2[0]}, [r3],  r1
	vst1.16     {d2[1]}, [r3],  r1
	vst1.16     {d2[2]}, [r3],  r1
	vst1.16     {d2[3]}, [r3],  r1
	bx          lr

offset_mc_prediction_2x2_ARMV7:
	vdup.16     q14,  r2
	mov		  r3, r0
	vld1.16	  {d0[0]},  [r0], r1
	vld1.16	  {d0[1]},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d2, q1
	vst1.16     {d2[0]}, [r3],  r1
	vst1.16     {d2[1]}, [r3],  r1
	bx          lr
	
        .macro  weight_16 add
        vdup.8          d0,  r2
1:      subs            ip,  ip,  #2
        vld1.8          {d10-d11},[r0], r1
        vmull.u8        q4,  d0,  d10
        pld             [r0]
        vmull.u8        q5,  d0,  d11
        vld1.8          {d28-d29},[r0], r1
        vmull.u8        q12, d0,  d28
        pld             [r0]
        vmull.u8        q13, d0,  d29
        \add            q4,  q8,  q4
        vrshl.s16       q4,  q4,  q9
        \add            q5,  q8,  q5
        vrshl.s16       q5,  q5,  q9
        vqmovun.s16     d2,  q4
        vqmovun.s16     d3,  q5
        \add            q12, q8,  q12
        vrshl.s16       q12, q12, q9
        \add            q13, q8,  q13
        vrshl.s16       q13, q13, q9
        vqmovun.s16     d4, q12
        vqmovun.s16     d5, q13
        vst1.8          {d2- d3}, [r3], r1
        vst1.8          {d4-d5},[r3], r1
        bne             1b
        pop             {r4, pc}
        .endm	

        .macro  weight_8 add
        vdup.8          d0,  r2
1:      subs            ip,  ip,  #2
        vld1.8          {d8},[r0], r1
        vmull.u8        q3,  d0,  d8
        pld             [r0]
        vld1.8          {d9},[r0], r1
        vmull.u8        q4, d0,  d9
        \add            q3,  q8,  q3
        pld             [r0]
        vrshl.s16       q3,  q3,  q9
        vqmovun.s16     d4,  q3
        \add            q4, q8,  q4
        vrshl.s16       q4, q4, q9
        vqmovun.s16     d5,  q4
        vst1.8          {d4},[r3], r1
        vst1.8          {d5},[r3], r1
        bne             1b
        pop             {r4, pc}
        .endm

        .macro  weight_4 add
        vdup.8          d0,  r2
        vmov            q4,  q8
        vmov            q5, q8
1:      subs            ip,  ip,  #4
        vld1.32         {d2[0]},[r0], r1
        vld1.32         {d2[1]},[r0], r1
        vmull.u8        q4,  d0,  d2
        pld             [r0]
        blt             2f
        vld1.32         {d4[0]},[r0], r1
        vld1.32         {d4[1]},[r0], r1
        vmull.u8        q5, d0,  d4
        pld             [r0]
        \add            q4,  q8,  q4
        vrshl.s16       q4,  q4,  q9
        vqmovun.s16     d4,  q4
        \add            q5, q8,  q5
        vrshl.s16       q5, q5, q9
        vqmovun.s16     d2,  q5
        vmov            q5, q8
        vst1.32         {d4[0]},[r3], r1
        vst1.32         {d4[1]},[r3], r1
        vmov            q4,  q8
        vst1.32         {d2[0]},[r3], r1
        vst1.32         {d2[1]},[r3], r1
        bne             1b
        pop             {r4, pc}
2:      \add            q4,  q8,  q4
        vrshl.s16       q4,  q4,  q9
        vqmovun.s16     d4,  q4
        vst1.32         {d4[0]},[r3], r1
        vst1.32         {d4[1]},[r3], r1
        pop             {r4, pc}
        .endm

weighted_mc_prediction_16x16_ARMV7:
        mov             ip,  #16
        push            {r4, lr}
        ldr             r4,  [sp, #8]
        cmp             r4,  #1
        lsl             r3,  r3,  r4
        vdup.16         q8,  r3
        mov             r3,  r0
        ble             20f
        rsb             lr,  r4,  #1
        vdup.16         q9,  lr
        cmp             r2,  #0
        blt             10f
        weight_16       vhadd.s16
10:     rsb             r2,  r2,  #0
        weight_16       vhsub.s16
20:     rsb             lr,  r4,  #0
        vdup.16         q9,  lr
        cmp             r2,  #0
        blt             10f
        weight_16       vadd.s16
10:     rsb             r2,  r2,  #0
        weight_16       vsub.s16
        
weighted_mc_prediction_16x8_ARMV7:
        mov             ip,  #8
        push            {r4, lr}
        ldr             r4,  [sp, #8]
        cmp             r4,  #1
        lsl             r3,  r3,  r4
        vdup.16         q8,  r3
        mov             r3,  r0
        ble             20f
        rsb             lr,  r4,  #1
        vdup.16         q9,  lr
        cmp             r2,  #0
        blt             10f
        weight_16       vhadd.s16
10:     rsb             r2,  r2,  #0
        weight_16       vhsub.s16
20:     rsb             lr,  r4,  #0
        vdup.16         q9,  lr
        cmp             r2,  #0
        blt             10f
        weight_16       vadd.s16
10:     rsb             r2,  r2,  #0
        weight_16       vsub.s16

weighted_mc_prediction_8x16_ARMV7:
        mov             ip,  #16
        push            {r4, lr}
        ldr             r4,  [sp, #8]
        cmp             r4,  #1
        lsl             r3,  r3,  r4
        vdup.16         q8,  r3
        mov             r3,  r0
        ble             20f
        rsb             lr,  r4,  #1
        vdup.16         q9,  lr
        cmp             r2,  #0
        blt             10f
        weight_8       vhadd.s16
10:     rsb             r2,  r2,  #0
        weight_8       vhsub.s16
20:     rsb             lr,  r4,  #0
        vdup.16         q9,  lr
        cmp             r2,  #0
        blt             10f
        weight_8       vadd.s16
10:     rsb             r2,  r2,  #0
        weight_8       vsub.s16

weighted_mc_prediction_8x8_ARMV7:
        mov             ip,  #8
        push            {r4, lr}
        ldr             r4,  [sp, #8]
        cmp             r4,  #1
        lsl             r3,  r3,  r4
        vdup.16         q8,  r3
        mov             r3,  r0
        ble             20f
        rsb             lr,  r4,  #1
        vdup.16         q9,  lr
        cmp             r2,  #0
        blt             10f
        weight_8       vhadd.s16
10:     rsb             r2,  r2,  #0
        weight_8       vhsub.s16
20:     rsb             lr,  r4,  #0
        vdup.16         q9,  lr
        cmp             r2,  #0
        blt             10f
        weight_8       vadd.s16
10:     rsb             r2,  r2,  #0
        weight_8       vsub.s16

weighted_mc_prediction_8x4_ARMV7:
        mov             ip,  #4
        push            {r4, lr}
        ldr             r4,  [sp, #8]
        cmp             r4,  #1
        lsl             r3,  r3,  r4
        vdup.16         q8,  r3
        mov             r3,  r0
        ble             20f
        rsb             lr,  r4,  #1
        vdup.16         q9,  lr
        cmp             r2,  #0
        blt             10f
        weight_8       vhadd.s16
10:     rsb             r2,  r2,  #0
        weight_8       vhsub.s16
20:     rsb             lr,  r4,  #0
        vdup.16         q9,  lr
        cmp             r2,  #0
        blt             10f
        weight_8       vadd.s16
10:     rsb             r2,  r2,  #0
        weight_8       vsub.s16
        
weighted_mc_prediction_4x8_ARMV7:
        mov             ip,  #8
        push            {r4, lr}
        ldr             r4,  [sp, #8]
        cmp             r4,  #1
        lsl             r3,  r3,  r4
        vdup.16         q8,  r3
        mov             r3,  r0
        ble             20f
        rsb             lr,  r4,  #1
        vdup.16         q9,  lr
        cmp             r2,  #0
        blt             10f
        weight_4       vhadd.s16
10:     rsb             r2,  r2,  #0
        weight_4       vhsub.s16
20:     rsb             lr,  r4,  #0
        vdup.16         q9,  lr
        cmp             r2,  #0
        blt             10f
        weight_4       vadd.s16
10:     rsb             r2,  r2,  #0
        weight_4       vsub.s16

weighted_mc_prediction_4x4_ARMV7:
        mov             ip,  #4
        push            {r4, lr}
        ldr             r4,  [sp, #8]
        cmp             r4,  #1
        lsl             r3,  r3,  r4
        vdup.16         q8,  r3
        mov             r3,  r0
        ble             20f
        rsb             lr,  r4,  #1
        vdup.16         q9,  lr
        cmp             r2,  #0
        blt             10f
        weight_4       vhadd.s16
10:     rsb             r2,  r2,  #0
        weight_4       vhsub.s16
20:     rsb             lr,  r4,  #0
        vdup.16         q9,  lr
        cmp             r2,  #0
        blt             10f
        weight_4       vadd.s16
10:     rsb             r2,  r2,  #0
        weight_4       vsub.s16   

 weighted_mc_prediction_4x2_ARMV7:
        mov             ip,  #2
        push            {r4, lr}
        ldr             r4,  [sp, #8]
        cmp             r4,  #1
        lsl             r3,  r3,  r4
        vdup.16         q8,  r3
        mov             r3,  r0
        ble             20f
        rsb             lr,  r4,  #1
        vdup.16         q9,  lr
        cmp             r2,  #0
        blt             10f
        weight_4       vhadd.s16
10:     rsb             r2,  r2,  #0
        weight_4       vhsub.s16
20:     rsb             lr,  r4,  #0
        vdup.16         q9,  lr
        cmp             r2,  #0
        blt             10f
        weight_4       vadd.s16
10:     rsb             r2,  r2,  #0
        weight_4       vsub.s16    
        
        .macro  biweight_16 macs, macd
        vdup.8          d0,  r4
        vdup.8          d1,  r5
        vmov            q2,  q8
        vmov            q3,  q8
1:      subs            ip,  ip,  #2
        vld1.8          {d20-d21},[r0], r1
        \macd           q2,  d0,  d20
        pld             [r0]
        \macd           q3,  d0,  d21
        vld1.8          {d22-d23},[r2], r3
        \macs           q2,  d1,  d22
        pld             [r2]
        \macs           q3,  d1,  d23
        vmov            q12, q8
        vld1.8          {d28-d29},[r0], r1
        vmov            q13, q8
        \macd           q12, d0,  d28
        pld             [r0]
        \macd           q13, d0,  d29
        vld1.8          {d30-d31},[r2], r3
        \macs           q12, d1,  d30
        pld             [r2]
        \macs           q13, d1,  d31
        vshl.s16        q2,  q2,  q9
        vshl.s16        q3,  q3,  q9
        vqmovun.s16     d4,  q2
        vqmovun.s16     d5,  q3
        vshl.s16        q12, q12, q9
        vshl.s16        q13, q13, q9
        vqmovun.s16     d24, q12
        vqmovun.s16     d25, q13
        vmov            q3,  q8
        vst1.8          {d4- d5}, [r6], r1
        vmov            q2,  q8
        vst1.8          {d24-d25},[r6], r1
        bne             1b
        pop             {r4-r7, pc}
        .endm

       .macro  biweight_8 macs, macd
        vdup.8          d0,  r4
        vdup.8          d1,  r5
        vmov            q1,  q8
        vmov            q10, q8
1:      subs            ip,  ip,  #2
        vld1.8          {d4},[r0], r1
        \macd           q1,  d0,  d4
        pld             [r0]
        vld1.8          {d5},[r2], r3
        \macs           q1,  d1,  d5
        pld             [r2]
        vld1.8          {d6},[r0], r1
        \macd           q10, d0,  d6
        pld             [r0]
        vld1.8          {d7},[r2], r3
        \macs           q10, d1,  d7
        pld             [r2]
        vshl.s16        q1,  q1,  q9
        vqmovun.s16     d2,  q1
        vshl.s16        q10, q10, q9
        vqmovun.s16     d4,  q10
        vmov            q10, q8
        vst1.8          {d2},[r6], r1
        vmov            q1,  q8
        vst1.8          {d4},[r6], r1
        bne             1b
        pop             {r4-r7, pc}
        .endm

        .macro  biweight_4 macs, macd
        vdup.8          d0,  r4
        vdup.8          d1,  r5
        vmov            q1,  q8
        vmov            q10, q8
1:      subs            ip,  ip,  #4
        vld1.32         {d4[0]},[r0], r1
        vld1.32         {d4[1]},[r0], r1
        \macd           q1,  d0,  d4
        pld             [r0]
        vld1.32         {d5[0]},[r2], r3
        vld1.32         {d5[1]},[r2], r3
        \macs           q1,  d1,  d5
        pld             [r2]
        blt             2f
        vld1.32         {d6[0]},[r0], r1
        vld1.32         {d6[1]},[r0], r1
        \macd           q10, d0,  d6
        pld             [r0]
        vld1.32         {d7[0]},[r2], r3
        vld1.32         {d7[1]},[r2], r3
        \macs           q10, d1,  d7
        pld             [r2]
        vshl.s16        q1,  q1,  q9
        vqmovun.s16     d2,  q1
        vshl.s16        q10, q10, q9
        vqmovun.s16     d4,  q10
        vmov            q10, q8
        vst1.32         {d2[0]},[r6], r1
        vst1.32         {d2[1]},[r6], r1
        vmov            q1,  q8
        vst1.32         {d4[0]},[r6], r1
        vst1.32         {d4[1]},[r6], r1
        bne             1b
        pop             {r4-r7, pc}
2:      vshl.s16        q1,  q1,  q9
        vqmovun.s16     d2,  q1
        vst1.32         {d2[0]},[r6], r1
        vst1.32         {d2[1]},[r6], r1
        pop             {r4-r7, pc}
        .endm


weighted_bi_prediction_16x16_ARMV7:
        mov             ip,  #16
	 push            {r4-r7, lr}
        ldr		r4, [sp, #20]
	 ldr		r5, [sp, #24]
	 ldr		r6, [sp, #28]
	 ldr		r7, [sp, #32]
        lsr             lr,  r4,  #31
        add             r6,  r6,  #1
        eors            lr,  lr,  r5,  lsr #30
        orr             r6,  r6,  #1
        vdup.16         q9,  r7
        lsl             r6,  r6,  r7
        vmvn            q9,  q9
        vdup.16         q8,  r6
        mov             r6,  r0
        beq             10f
        subs            lr,  lr,  #1
        beq             20f
        subs            lr,  lr,  #1
        beq             30f
        b               40f
10:   biweight_16     vmlal.u8, vmlal.u8
20:     rsb             r4,  r4,  #0
        biweight_16     vmlal.u8, vmlsl.u8
30:     rsb             r4,  r4,  #0
        rsb             r5,  r5,  #0
        biweight_16     vmlsl.u8, vmlsl.u8
40:     rsb             r5,  r5,  #0
        biweight_16     vmlsl.u8, vmlal.u8	


weighted_bi_prediction_16x8_ARMV7:
        mov             ip,  #8
	 push            {r4-r7, lr}
        ldr		r4, [sp, #20]
	 ldr		r5, [sp, #24]
	 ldr		r6, [sp, #28]
	 ldr		r7, [sp, #32]
        lsr             lr,  r4,  #31
        add             r6,  r6,  #1
        eors            lr,  lr,  r5,  lsr #30
        orr             r6,  r6,  #1
        vdup.16         q9,  r7
        lsl             r6,  r6,  r7
        vmvn            q9,  q9
        vdup.16         q8,  r6
        mov             r6,  r0
        beq             10f
        subs            lr,  lr,  #1
        beq             20f
        subs            lr,  lr,  #1
        beq             30f
        b               40f
10:   biweight_16     vmlal.u8, vmlal.u8
20:     rsb             r4,  r4,  #0
        biweight_16     vmlal.u8, vmlsl.u8
30:     rsb             r4,  r4,  #0
        rsb             r5,  r5,  #0
        biweight_16     vmlsl.u8, vmlsl.u8
40:     rsb             r5,  r5,  #0
        biweight_16     vmlsl.u8, vmlal.u8	

weighted_bi_prediction_8x16_ARMV7:
        mov             ip,  #16
	 push            {r4-r7, lr}
        ldr		r4, [sp, #20]
	 ldr		r5, [sp, #24]
	 ldr		r6, [sp, #28]
	 ldr		r7, [sp, #32]
        lsr             lr,  r4,  #31
        add             r6,  r6,  #1
        eors            lr,  lr,  r5,  lsr #30
        orr             r6,  r6,  #1
        vdup.16         q9,  r7
        lsl             r6,  r6,  r7
        vmvn            q9,  q9
        vdup.16         q8,  r6
        mov             r6,  r0
        beq             10f
        subs            lr,  lr,  #1
        beq             20f
        subs            lr,  lr,  #1
        beq             30f
        b               40f
10:   biweight_8     vmlal.u8, vmlal.u8
20:     rsb             r4,  r4,  #0
        biweight_8     vmlal.u8, vmlsl.u8
30:     rsb             r4,  r4,  #0
        rsb             r5,  r5,  #0
        biweight_8     vmlsl.u8, vmlsl.u8
40:     rsb             r5,  r5,  #0
        biweight_8     vmlsl.u8, vmlal.u8	

weighted_bi_prediction_8x8_ARMV7:
        mov             ip,  #8
	 push            {r4-r7, lr}
        ldr		r4, [sp, #20]
	 ldr		r5, [sp, #24]
	 ldr		r6, [sp, #28]
	 ldr		r7, [sp, #32]
        lsr             lr,  r4,  #31
        add             r6,  r6,  #1
        eors            lr,  lr,  r5,  lsr #30
        orr             r6,  r6,  #1
        vdup.16         q9,  r7
        lsl             r6,  r6,  r7
        vmvn            q9,  q9
        vdup.16         q8,  r6
        mov             r6,  r0
        beq             10f
        subs            lr,  lr,  #1
        beq             20f
        subs            lr,  lr,  #1
        beq             30f
        b               40f
10:   biweight_8     vmlal.u8, vmlal.u8
20:     rsb             r4,  r4,  #0
        biweight_8     vmlal.u8, vmlsl.u8
30:     rsb             r4,  r4,  #0
        rsb             r5,  r5,  #0
        biweight_8     vmlsl.u8, vmlsl.u8
40:     rsb             r5,  r5,  #0
        biweight_8     vmlsl.u8, vmlal.u8	

weighted_bi_prediction_8x4_ARMV7:
        mov             ip,  #4
	 push            {r4-r7, lr}
        ldr		r4, [sp, #20]
	 ldr		r5, [sp, #24]
	 ldr		r6, [sp, #28]
	 ldr		r7, [sp, #32]
        lsr             lr,  r4,  #31
        add             r6,  r6,  #1
        eors            lr,  lr,  r5,  lsr #30
        orr             r6,  r6,  #1
        vdup.16         q9,  r7
        lsl             r6,  r6,  r7
        vmvn            q9,  q9
        vdup.16         q8,  r6
        mov             r6,  r0
        beq             10f
        subs            lr,  lr,  #1
        beq             20f
        subs            lr,  lr,  #1
        beq             30f
        b               40f
10:   biweight_8     vmlal.u8, vmlal.u8
20:     rsb             r4,  r4,  #0
        biweight_8     vmlal.u8, vmlsl.u8
30:     rsb             r4,  r4,  #0
        rsb             r5,  r5,  #0
        biweight_8     vmlsl.u8, vmlsl.u8
40:     rsb             r5,  r5,  #0
        biweight_8     vmlsl.u8, vmlal.u8	
	
weighted_bi_prediction_4x8_ARMV7:
        mov             ip,  #8
	 push            {r4-r7, lr}
        ldr		r4, [sp, #20]
	 ldr		r5, [sp, #24]
	 ldr		r6, [sp, #28]
	 ldr		r7, [sp, #32]
        lsr             lr,  r4,  #31
        add             r6,  r6,  #1
        eors            lr,  lr,  r5,  lsr #30
        orr             r6,  r6,  #1
        vdup.16         q9,  r7
        lsl             r6,  r6,  r7
        vmvn            q9,  q9
        vdup.16         q8,  r6
        mov             r6,  r0
        beq             10f
        subs            lr,  lr,  #1
        beq             20f
        subs            lr,  lr,  #1
        beq             30f
        b               40f
10:   biweight_4     vmlal.u8, vmlal.u8
20:   rsb             r4,  r4,  #0
        biweight_4     vmlal.u8, vmlsl.u8
30:     rsb             r4,  r4,  #0
        rsb             r5,  r5,  #0
        biweight_4     vmlsl.u8, vmlsl.u8
40:     rsb             r5,  r5,  #0
        biweight_4     vmlsl.u8, vmlal.u8

weighted_bi_prediction_4x4_ARMV7:
        mov             ip,  #4
	 push            {r4-r7, lr}
        ldr		r4, [sp, #20]
	 ldr		r5, [sp, #24]
	 ldr		r6, [sp, #28]
	 ldr		r7, [sp, #32]
        lsr             lr,  r4,  #31
        add             r6,  r6,  #1
        eors            lr,  lr,  r5,  lsr #30
        orr             r6,  r6,  #1
        vdup.16         q9,  r7
        lsl             r6,  r6,  r7
        vmvn            q9,  q9
        vdup.16         q8,  r6
        mov             r6,  r0
        beq             10f
        subs            lr,  lr,  #1
        beq             20f
        subs            lr,  lr,  #1
        beq             30f
        b               40f
10:   biweight_4     vmlal.u8, vmlal.u8
20:     rsb             r4,  r4,  #0
        biweight_4     vmlal.u8, vmlsl.u8
30:     rsb             r4,  r4,  #0
        rsb             r5,  r5,  #0
        biweight_4     vmlsl.u8, vmlsl.u8
40:     rsb             r5,  r5,  #0
        biweight_4     vmlsl.u8, vmlal.u8

weighted_bi_prediction_4x2_ARMV7:
        mov             ip,  #2
	 push            {r4-r7, lr}
        ldr		r4, [sp, #20]
	 ldr		r5, [sp, #24]
	 ldr		r6, [sp, #28]
	 ldr		r7, [sp, #32]
        lsr             lr,  r4,  #31
        add             r6,  r6,  #1
        eors            lr,  lr,  r5,  lsr #30
        orr             r6,  r6,  #1
        vdup.16         q9,  r7
        lsl             r6,  r6,  r7
        vmvn            q9,  q9
        vdup.16         q8,  r6
        mov             r6,  r0
        beq             10f
        subs            lr,  lr,  #1
        beq             20f
        subs            lr,  lr,  #1
        beq             30f
        b               40f
10:   biweight_4     vmlal.u8, vmlal.u8
20:     rsb             r4,  r4,  #0
        biweight_4     vmlal.u8, vmlsl.u8
30:     rsb             r4,  r4,  #0
        rsb             r5,  r5,  #0
        biweight_4     vmlsl.u8, vmlsl.u8
40:     rsb             r5,  r5,  #0
        biweight_4     vmlsl.u8, vmlal.u8


	
	.end
