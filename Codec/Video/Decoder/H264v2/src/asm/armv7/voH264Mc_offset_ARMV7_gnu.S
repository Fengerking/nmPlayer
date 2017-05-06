@;*****************************************************************************
@;*																			   *
@;*		VisualOn, Inc. Confidential and Proprietary, 2012					                 *
@;*																			   *
@;*****************************************************************************
    @AREA	|.text|, CODE
 #include "../../defineID.h"   
  .section .text
  
  .global	 offset_block_00_16x16_ARMV7
  .global	 offset_block_00_16x8_ARMV7
  .global	 offset_block_00_8x16_ARMV7
  .global	 offset_block_00_8x8_ARMV7
  .global	 offset_block_00_4x8_ARMV7
  .global	 offset_block_00_8x4_ARMV7
  .global	 offset_block_00_4x4_ARMV7
  .global	 offset_block_00_4x2_ARMV7
  .global	 avg_offset_luma_h_16x16_ARMV7
  .global	 avg_offset_luma_h_16x8_ARMV7
  .global	 avg_offset_luma_h_8x16_ARMV7
  .global	 avg_offset_luma_h_8x8_ARMV7
  .global   offset_luma_h_16x16_ARMV7
  .global   offset_luma_h_16x8_ARMV7
  .global   offset_luma_h_8x16_ARMV7
  .global   offset_luma_h_8x8_ARMV7
  .global   avg_offset_luma_v_16x16_ARMV7
  .global   avg_offset_luma_v_16x8_ARMV7
  .global   avg_offset_luma_v_8x16_ARMV7
  .global   avg_offset_luma_v_8x8_ARMV7
  .global   avg_dst_offset_luma_v_16x16_ARMV7
  .global   avg_dst_offset_luma_v_16x8_ARMV7
  .global   avg_dst_offset_luma_v_8x16_ARMV7
  .global   avg_dst_offset_luma_v_8x8_ARMV7
  .global   avg_dst_offset_luma_h_16x16_ARMV7
  .global   avg_dst_offset_luma_h_16x8_ARMV7
  .global   avg_dst_offset_luma_h_8x16_ARMV7
  .global   avg_dst_offset_luma_h_8x8_ARMV7
  .global   offset_luma_v_16x16_ARMV7
  .global   offset_luma_v_16x8_ARMV7
  .global   offset_luma_v_8x16_ARMV7
  .global   offset_luma_v_8x8_ARMV7
  .global   offset_luma_c_16x16_ARMV7
  .global   offset_luma_c_16x8_ARMV7
  .global   offset_luma_c_8x16_ARMV7
  .global   offset_luma_c_8x8_ARMV7
  .global   OFFSETChroma8x8_ARMV7
  .global   OFFSETChroma8x4_ARMV7
  .global   OFFSETChroma4x8_ARMV7
  .global   OFFSETChroma4x4_ARMV7

  .macro QUARTER16_OFFSET_H_ROW 
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
    vdup.16     q10,  r7
    vaddw.u8  q1, q10, d24
    vaddw.u8  q2, q10, d25
    vqmovun.s16     d6, q1
    vqmovun.s16     d7, q2
    vst1.64     {q3}, [r2],  r3
  .endm

  .macro QUARTER8_OFFSET_H_ROW 
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
    vaddw.u8  q1, q7, d24
    vqmovun.s16     d6, q1
    vst1.64     {d6}, [r2],  r3
  .endm

  .macro HALF16_OFFSET_H_ROW r, sd
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
    vdup.16     q10,  r4
    vaddw.u8  q1, q10, d24
    vaddw.u8  q2, q10, d25
    vqmovun.s16     d6, q1
    vqmovun.s16     d7, q2
    vst1.64     {q3}, [\r],  \sd
  .endm

  .macro HALF8_OFFSET_H_ROW r, sd
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
    vaddw.u8   q1, q7, d24
    vqmovun.s16     d6, q1
    vst1.64     {d6}, [\r],  \sd
  .endm

  .macro QUARTER16_OFFSET_V_ROW q, r0, r1, sd, d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11
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
    vdup.16     q10,  r7
    vaddw.u8  q8, q10, d24
    vaddw.u8  q9, q10, d25
    vqmovun.s16     d24, q8
    vqmovun.s16     d25, q9
    vst1.64     {q12}, [\r1],  \sd
  .endm

  .macro QUARTER8_OFFSET_V_ROW q, r0, r1, sd, d0, d1, d4, d5, d8, d9
    vld1.64		{\q},  [\r0], r1		@;f0 ... f15
    vaddl.u8	q10,  \d0,  \d1		@;c0+d0 ... c7+d7	
    vaddl.u8	q6,  \d4,  \d5		@;a0+f0 ... a7+f7
    vaddl.u8	q8,  \d8,  \d9		@;b0+e0 ... b7+e7	
    vld1.64		{d26},  [r4], r5
    vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
    vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
    vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
    vrhadd.u8  d24, d24, d26
    vaddw.u8  q8, q7, d24
    vqmovun.s16     d24, q8
    vst1.64     {d24}, [\r1],  \sd
  .endm

  .macro AVG16_OFFSET_V_ROW q, r0, r1, sd, d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11
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
    vdup.16     q10,  r4
    vaddw.u8  q8, q10, d24
    vaddw.u8  q9, q10, d25
    vqmovun.s16     d24, q8
    vqmovun.s16     d25, q9
    vst1.64     {q12}, [\r1],  \sd
  .endm

  .macro AVG8_OFFSET_V_ROW q, r0, r1, sd, d0, d1, d4, d5, d8, d9
    vld1.64		{\q},  [\r0], r1		@;f0 ... f15
    vaddl.u8	q10,  \d0,  \d1		@;c0+d0 ... c7+d7	
    vaddl.u8	q6,  \d4,  \d5		@;a0+f0 ... a7+f7
    vaddl.u8	q8,  \d8,  \d9		@;b0+e0 ... b7+e7	
    vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
    vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
    vqrshrun.s16 	d24,  q6,  #5	@;0-7 solution
    vld1.64		{d26},  [r2]
    vrhadd.u8  d24, d24, d26
    vaddw.u8  q8, q7, d24
    vqmovun.s16     d24, q8
    vst1.64     {d24}, [\r1],  \sd
  .endm

  .macro AVG16_OFFSET_H_ROW 
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
    vdup.16     q10,  r4
    vaddw.u8  q8, q10, d24
    vaddw.u8  q9, q10, d25
    vqmovun.s16     d24, q8
    vqmovun.s16     d25, q9
    vst1.64     {q12}, [r2],  r3
  .endm

  .macro AVG8_OFFSET_H_ROW
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
    vaddw.u8  q8, q7, d24
    vqmovun.s16     d24, q8
    vst1.64     {d24}, [r2],  r3
  .endm

  .macro HALF16_OFFSET_V_ROW q, r0, r1, sd, d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11
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
    vdup.16     q10,  r4
    vaddw.u8  q8, q10, d24
    vaddw.u8  q9, q10, d25
    vqmovun.s16     d24, q8
    vqmovun.s16     d25, q9
    vst1.64     {q12}, [\r1],  \sd
  .endm
  
  .macro HALF8_OFFSET_V_ROW q, r0, r1, sd, d0, d1, d4, d5, d8, d9
    vld1.64		{\q},  [\r0], r1		@f0 ... f15
    vaddl.u8	q10,  \d0,  \d1		@c0+d0 ... c7+d7	
    vaddl.u8	q6,  \d4,  \d5		@a0+f0 ... a7+f7
    vaddl.u8	q8,  \d8,  \d9		@b0+e0 ... b7+e7	
    vmla.s16    q6,  q10,  q15  	@a+20(c+d)+f 0-7
    vmls.s16    q6,  q8,  q14		@a-5(b+e)+20(c+d)+f 0-7
    vqrshrun.s16 	d24,  q6,  #5	@0-7 solution
    vaddw.u8  q8, q7, d24
    vqmovun.s16     d24, q8
    vst1.64     {d24}, [\r1],  \sd
  .endm

  .macro HALF8_OFFSET_C_ROW q0, r0, q1, r1, d0, d1, d2, d3,  d4, d5, d6, d7, d8, d9, d10, d11
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
    vdup.16     q11,  r4
    vaddw.u8  q8, q11, d24
    vqmovun.s16     d24, q8
    vst1.64     {d24}, [\r1],  r3
  .endm

  .macro OFFSETChroma8_ROW q0, d0, d1, d2, d3
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
    vaddw.u8  q9, q11, d16
    vqmovun.s16     d16, q9
    vst1.64     {d16}, [r2],  r3
  .endm

  .macro OFFSETChroma4_ROW q0, d0, d1, d2, d3
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
    vaddw.u8  q9, q11, d16
    vqmovun.s16     d16, q9
    vst1.32     {d16[0]}, [r2],  r3
  .endm

OFFSETChroma8x8_ARMV7:
	push     	{r4 - r11, r14}
	ldr  		r9, [sp, #36]   
	ldr  		r4, [sp, #40]   
	ldr  		r5, [sp, #44]   
	ldr  		r6, [sp, #48]
	ldr  		r7, [sp, #52]
	mov         r8, #2
	vdup.8     d24,  r4	
	vdup.8     d25,  r6
	vdup.8     d26,  r5
	vdup.8     d27,  r7
	vdup.16     q11,  r9
	vld1.64		{q0},  [r0], r1
	vext.8      d1, d0, d1, #1
OFFSETChroma8x8_loop:
	OFFSETChroma8_ROW q1, d2, d3, d0, d1
	OFFSETChroma8_ROW q0, d0, d1, d2, d3
	OFFSETChroma8_ROW q1, d2, d3, d0, d1
	OFFSETChroma8_ROW q0, d0, d1, d2, d3
	subs        r8, r8, #1
       bne         OFFSETChroma8x8_loop	
	pop      	{r4 - r11, pc}

OFFSETChroma8x4_ARMV7:
	push     	{r4 - r11, r14}
	ldr  		r9, [sp, #36]   
	ldr  		r4, [sp, #40]   
	ldr  		r5, [sp, #44]   
	ldr  		r6, [sp, #48]
	ldr  		r7, [sp, #52]
	vdup.8     d24,  r4	
	vdup.8     d25,  r6
	vdup.8     d26,  r5
	vdup.8     d27,  r7
	vdup.16     q11,  r9
	vld1.64		{q0},  [r0], r1
	vext.8      d1, d0, d1, #1
	OFFSETChroma8_ROW q1, d2, d3, d0, d1
	OFFSETChroma8_ROW q0, d0, d1, d2, d3
	OFFSETChroma8_ROW q1, d2, d3, d0, d1
	OFFSETChroma8_ROW q0, d0, d1, d2, d3
	pop      	{r4 - r11, pc}

OFFSETChroma4x8_ARMV7:
	push     	{r4 - r11, r14}
	ldr  		r9, [sp, #36]   
	ldr  		r4, [sp, #40]   
	ldr  		r5, [sp, #44]   
	ldr  		r6, [sp, #48]
	ldr  		r7, [sp, #52]
	mov         r8, #2
	vdup.8     d24,  r4	
	vdup.8     d25,  r6
	vdup.8     d26,  r5
	vdup.8     d27,  r7
	vdup.16     q11,  r9
	vld1.64		{q0},  [r0], r1
	vext.8      d1, d0, d1, #1
OFFSETChroma4x8_loop:
	OFFSETChroma4_ROW q1, d2, d3, d0, d1
	OFFSETChroma4_ROW q0, d0, d1, d2, d3
	OFFSETChroma4_ROW q1, d2, d3, d0, d1
	OFFSETChroma4_ROW q0, d0, d1, d2, d3
	subs        r8, r8, #1
       bne         OFFSETChroma4x8_loop	
	pop      	{r4 - r11, pc}

OFFSETChroma4x4_ARMV7:
	push     	{r4 - r11, r14}
	ldr  		r9, [sp, #36]   
	ldr  		r4, [sp, #40]   
	ldr  		r5, [sp, #44]   
	ldr  		r6, [sp, #48]
	ldr  		r7, [sp, #52]
	vdup.8     d24,  r4	
	vdup.8     d25,  r6
	vdup.8     d26,  r5
	vdup.8     d27,  r7
	vdup.16     q11,  r9
	vld1.64		{q0},  [r0], r1
	vext.8      d1, d0, d1, #1
	OFFSETChroma4_ROW q1, d2, d3, d0, d1
	OFFSETChroma4_ROW q0, d0, d1, d2, d3
	OFFSETChroma4_ROW q1, d2, d3, d0, d1
	OFFSETChroma4_ROW q0, d0, d1, d2, d3	
	pop      	{r4 - r11, pc}

offset_luma_c_16x16_ARMV7:	
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
       mov         r10, #2
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	sub			r0, r0, #2
	add         r5, r0, #8
	add			r7, r2, #8
offset_luma_c_16x16_loop0:
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
	vdup.16     q11,  r4
       vaddw.u8  q8, q11, d24
       vqmovun.s16     d24, q8
	vst1.64     {d24}, [r2],  r3
offset_luma_c_16x16_loop1:
       HALF8_OFFSET_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	HALF8_OFFSET_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	HALF8_OFFSET_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	HALF8_OFFSET_C_ROW  q3, r12, q4, r2, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5		
	HALF8_OFFSET_C_ROW  q4, r12, q5, r2, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7	
	HALF8_OFFSET_C_ROW  q5, r12, q0, r2, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
       subs        r11, r11, #1
       bne         offset_luma_c_16x16_loop1	
       HALF8_OFFSET_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	HALF8_OFFSET_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	HALF8_OFFSET_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
	mov			r0, r5
	mov			r2, r7
	subs        r10, r10, #1
       bne         offset_luma_c_16x16_loop0	
	pop      	{r4 - r11, pc}	

offset_luma_c_16x8_ARMV7:	
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
       mov         r10, #2
	sub         r0, r0, r1, LSL #1  @;src = src - 2*stride
	sub			r0, r0, #2
	add         r5, r0, #8
	add			r7, r2, #8
offset_luma_c_16x8_loop0:
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
	vdup.16     q11,  r4
       vaddw.u8  q8, q11, d24
       vqmovun.s16     d24, q8
	vst1.64     {d24}, [r2],  r3
       HALF8_OFFSET_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	HALF8_OFFSET_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	HALF8_OFFSET_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	HALF8_OFFSET_C_ROW  q3, r12, q4, r2, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5		
	HALF8_OFFSET_C_ROW  q4, r12, q5, r2, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7	
	HALF8_OFFSET_C_ROW  q5, r12, q0, r2, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
       HALF8_OFFSET_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	mov			r0, r5
	mov			r2, r7
	subs        r10, r10, #1
       bne         offset_luma_c_16x8_loop0	
	pop      	{r4 - r11, pc}	

offset_luma_c_8x16_ARMV7:	
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
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
	vdup.16     q11,  r4
       vaddw.u8  q8, q11, d24
       vqmovun.s16     d24, q8
	vst1.64     {d24}, [r2],  r3
offset_luma_c_8x16_loop1:
       HALF8_OFFSET_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	HALF8_OFFSET_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	HALF8_OFFSET_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	HALF8_OFFSET_C_ROW  q3, r12, q4, r2, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5		
	HALF8_OFFSET_C_ROW  q4, r12, q5, r2, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7	
	HALF8_OFFSET_C_ROW  q5, r12, q0, r2, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
       subs        r11, r11, #1
       bne         offset_luma_c_8x16_loop1	
       HALF8_OFFSET_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	HALF8_OFFSET_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	HALF8_OFFSET_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
	pop      	{r4 - r11, pc}	

offset_luma_c_8x8_ARMV7:	
	push     	{r4 - r11, r14}
	ldr		r4, [sp, #36]
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
	vdup.16     q11,  r4
       vaddw.u8  q8, q11, d24
       vqmovun.s16     d24, q8
	vst1.64     {d24}, [r2],  r3
       HALF8_OFFSET_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	HALF8_OFFSET_C_ROW  q1, r12, q2, r2, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	HALF8_OFFSET_C_ROW  q2, r12, q3, r2, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	HALF8_OFFSET_C_ROW  q3, r12, q4, r2, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5		
	HALF8_OFFSET_C_ROW  q4, r12, q5, r2, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7	
	HALF8_OFFSET_C_ROW  q5, r12, q0, r2, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
       HALF8_OFFSET_C_ROW  q0, r12, q1, r2, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	pop      	{r4 - r11, pc}	

offset_luma_v_16x16_ARMV7:
       push     	{r4 - r5, r14}
	ldr		r4, [sp, #12]
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
	vdup.16     q10,  r4
       vaddw.u8  q8, q10, d24
       vaddw.u8  q9, q10, d25
       vqmovun.s16     d24, q8
       vqmovun.s16     d25, q9
	vst1.64     {q12}, [r2],  r3
offset_luma_v_16x16_loop:
	HALF16_OFFSET_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	HALF16_OFFSET_V_ROW  q1, r12, r2, r3, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	HALF16_OFFSET_V_ROW  q2, r12, r2, r3, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
	HALF16_OFFSET_V_ROW  q3, r12, r2, r3, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5
	HALF16_OFFSET_V_ROW  q4, r12, r2, r3, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
	HALF16_OFFSET_V_ROW  q5, r12, r2, r3, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
	subs        r5, r5, #1
       bne         offset_luma_v_16x16_loop
	HALF16_OFFSET_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	HALF16_OFFSET_V_ROW  q1, r12, r2, r3, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	HALF16_OFFSET_V_ROW  q2, r12, r2, r3, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	pop      	{r4 - r5, pc}

offset_luma_v_16x8_ARMV7:
       push     	{r4, r14}
	ldr		r4, [sp, #8]
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
	vdup.16     q10,  r4
       vaddw.u8  q8, q10, d24
       vaddw.u8  q9, q10, d25
       vqmovun.s16     d24, q8
       vqmovun.s16     d25, q9
	vst1.64     {q12}, [r2],  r3
	HALF16_OFFSET_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	HALF16_OFFSET_V_ROW  q1, r12, r2, r3, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	HALF16_OFFSET_V_ROW  q2, r12, r2, r3, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
	HALF16_OFFSET_V_ROW  q3, r12, r2, r3, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5
	HALF16_OFFSET_V_ROW  q4, r12, r2, r3, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
	HALF16_OFFSET_V_ROW  q5, r12, r2, r3, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
	HALF16_OFFSET_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	pop      	{r4, pc}

offset_luma_v_8x16_ARMV7:
	push     	{r4 - r5, r14}
	ldr		r4, [sp, #12]
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
	vdup.16     q7,  r4
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vaddw.u8  q8, q7, d24
       vqmovun.s16     d24, q8
	vst1.64     {d24}, [r2],  r3
offset_luma_v_8x16_loop:
	HALF8_OFFSET_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	HALF8_OFFSET_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	HALF8_OFFSET_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	HALF8_OFFSET_V_ROW  d6, r12, r2, r3, d0, d2, d8, d6, d10, d4
	HALF8_OFFSET_V_ROW  d8, r12, r2, r3, d2, d4, d10, d8, d0, d6
	HALF8_OFFSET_V_ROW  d10, r12, r2, r3, d4, d6, d0, d10, d2, d8
	subs        r5, r5, #1
       bne         offset_luma_v_8x16_loop
	HALF8_OFFSET_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	HALF8_OFFSET_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	HALF8_OFFSET_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	pop      	{r4 - r5, pc}	

offset_luma_v_8x8_ARMV7:
	push     	{r4, r14}
	ldr		r4, [sp, #8]
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
	vdup.16     q7,  r4
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vaddw.u8  q8, q7, d24
       vqmovun.s16     d24, q8
	vst1.64     {d24}, [r2],  r3
	HALF8_OFFSET_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	HALF8_OFFSET_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	HALF8_OFFSET_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	HALF8_OFFSET_V_ROW  d6, r12, r2, r3, d0, d2, d8, d6, d10, d4
	HALF8_OFFSET_V_ROW  d8, r12, r2, r3, d2, d4, d10, d8, d0, d6
	HALF8_OFFSET_V_ROW  d10, r12, r2, r3, d4, d6, d0, d10, d2, d8
	HALF8_OFFSET_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	pop      	{r4, pc}	

avg_dst_offset_luma_h_16x16_ARMV7:
	push     	{r4 - r5, r14}
	ldr		r4, [sp, #12]
       mov         r5, #4
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
avg_dst_offset_luma_h_16x16_loop	:
	AVG16_OFFSET_H_ROW 
	AVG16_OFFSET_H_ROW 
	AVG16_OFFSET_H_ROW 
	AVG16_OFFSET_H_ROW 
	subs        r5, r5, #1
       bne         avg_dst_offset_luma_h_16x16_loop		 
	pop      	{r4 - r5, pc}

avg_dst_offset_luma_h_16x8_ARMV7:
	push     	{r4 - r5, r14}
	ldr		r4, [sp, #12]
       mov         r5, #2
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
avg_dst_offset_luma_h_16x8_loop	:
	AVG16_OFFSET_H_ROW 
	AVG16_OFFSET_H_ROW 
	AVG16_OFFSET_H_ROW 
	AVG16_OFFSET_H_ROW 
	subs        r5, r5, #1
       bne         avg_dst_offset_luma_h_16x8_loop		 
	pop      	{r4 - r5, pc}

avg_dst_offset_luma_h_8x16_ARMV7:
	push     	{r4 - r5, r14}
	ldr		r4, [sp, #12]
       mov         r5, #4
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
	vdup.16     q7,  r4
avg_dst_offset_luma_h_8x16_loop	:
	AVG8_OFFSET_H_ROW 
	AVG8_OFFSET_H_ROW 
	AVG8_OFFSET_H_ROW 
	AVG8_OFFSET_H_ROW 
	subs        r5, r5, #1
       bne         avg_dst_offset_luma_h_8x16_loop		 
	pop      	{r4 - r5, pc}

avg_dst_offset_luma_h_8x8_ARMV7:
	push     	{r4 - r5, r14}
	ldr		r4, [sp, #12]
       mov         r5, #2
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
	vdup.16     q7,  r4
avg_dst_offset_luma_h_8x8_loop	:
	AVG8_OFFSET_H_ROW 
	AVG8_OFFSET_H_ROW 
	AVG8_OFFSET_H_ROW 
	AVG8_OFFSET_H_ROW 
	subs        r5, r5, #1
       bne         avg_dst_offset_luma_h_8x8_loop		 
	pop      	{r4 - r5, pc}

avg_dst_offset_luma_v_16x16_ARMV7:
	push     	{r4 - r5, r14}
	ldr		r4, [sp, #12]
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
	vdup.16     q10,  r4
       vaddw.u8  q8, q10, d24
       vaddw.u8  q9, q10, d25
       vqmovun.s16     d24, q8
       vqmovun.s16     d25, q9
	vst1.64     {q12}, [r2],  r3
avg_dst_offset_luma_v_16x16_loop:	
	AVG16_OFFSET_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	AVG16_OFFSET_V_ROW  q1, r12, r2, r3, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	AVG16_OFFSET_V_ROW  q2, r12, r2, r3, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
	AVG16_OFFSET_V_ROW  q3, r12, r2, r3, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5
	AVG16_OFFSET_V_ROW  q4, r12, r2, r3, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
	AVG16_OFFSET_V_ROW  q5, r12, r2, r3, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
	subs        r5, r5, #1
       bne         avg_dst_offset_luma_v_16x16_loop		 
	AVG16_OFFSET_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	AVG16_OFFSET_V_ROW  q1, r12, r2, r3, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	AVG16_OFFSET_V_ROW  q2, r12, r2, r3, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	pop      	{r4 - r5, pc}

avg_dst_offset_luma_v_16x8_ARMV7:
	push     	{r4 , r14}
	ldr		r4, [sp, #8]
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
	vdup.16     q10,  r4
       vaddw.u8  q8, q10, d24
       vaddw.u8  q9, q10, d25
       vqmovun.s16     d24, q8
       vqmovun.s16     d25, q9
	vst1.64     {q12}, [r2],  r3	
	AVG16_OFFSET_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	AVG16_OFFSET_V_ROW  q1, r12, r2, r3, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	AVG16_OFFSET_V_ROW  q2, r12, r2, r3, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
	AVG16_OFFSET_V_ROW  q3, r12, r2, r3, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5
	AVG16_OFFSET_V_ROW  q4, r12, r2, r3, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
	AVG16_OFFSET_V_ROW  q5, r12, r2, r3, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9		 
	AVG16_OFFSET_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	pop      	{r4, pc}

avg_dst_offset_luma_v_8x16_ARMV7:
	push     	{r4 - r5, r14}
	ldr		r4, [sp, #12]
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
	vdup.16     q7,  r4
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vld1.64		{d26},  [r2]
	vrhadd.u8  d24, d24, d26
	vaddw.u8  q8, q7, d24
       vqmovun.s16     d24, q8
	vst1.64     {d24}, [r2],  r3
avg_dst_offset_luma_v_8x16_loop:
	AVG8_OFFSET_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	AVG8_OFFSET_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	AVG8_OFFSET_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	AVG8_OFFSET_V_ROW  d6, r12, r2, r3, d0, d2, d8, d6, d10, d4
	AVG8_OFFSET_V_ROW  d8, r12, r2, r3, d2, d4, d10, d8, d0, d6
	AVG8_OFFSET_V_ROW  d10, r12, r2, r3, d4, d6, d0, d10, d2, d8
	subs        r5, r5, #1
       bne         avg_dst_offset_luma_v_8x16_loop
	AVG8_OFFSET_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	AVG8_OFFSET_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	AVG8_OFFSET_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	pop      	{r4 - r5, pc}

avg_dst_offset_luma_v_8x8_ARMV7:
	push     	{r4, r14}
	ldr		r4, [sp, #8]
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
	vdup.16     q7,  r4
	vld1.64		{d8},  [r0], r1		@;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		@;b0+e0 ... b7+e7
	vaddl.u8	q10,  d4,  d6		@;c0+d0 ... c7+d7	
	vmla.s16    q6,  q10,  q15  	@;a+20(c+d)+f 0-7
	vmls.s16    q6,  q8,  q14		@;a-5(b+e)+20(c+d)+f 0-7
	vqrshrun 	d24.u8,  q6.s16,  #5@;0-7 solution
	vld1.64		{d26},  [r2]
	vrhadd.u8  d24, d24, d26
	vaddw.u8  q8, q7, d24
       vqmovun.s16     d24, q8
	vst1.64     {d24}, [r2],  r3
	AVG8_OFFSET_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	AVG8_OFFSET_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	AVG8_OFFSET_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	AVG8_OFFSET_V_ROW  d6, r12, r2, r3, d0, d2, d8, d6, d10, d4
	AVG8_OFFSET_V_ROW  d8, r12, r2, r3, d2, d4, d10, d8, d0, d6
	AVG8_OFFSET_V_ROW  d10, r12, r2, r3, d4, d6, d0, d10, d2, d8
	AVG8_OFFSET_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	pop      	{r4 , pc}

avg_offset_luma_v_16x16_ARMV7:
	push     	{r4 - r7, r14}
  	mov		r6, #2
	ldr		r4, [sp, #20]
	ldr		r5, [sp, #24]
	ldr          r7, [sp, #28]
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
	vdup.16     q10,  r7
       vaddw.u8  q8, q10, d24
       vaddw.u8  q9, q10, d25
       vqmovun.s16     d24, q8
       vqmovun.s16     d25, q9
	vst1.64     {q12}, [r2],  r3
avg_offset_luma_v_16x16_loop:
	QUARTER16_OFFSET_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	QUARTER16_OFFSET_V_ROW  q1, r12, r2, r3, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	QUARTER16_OFFSET_V_ROW  q2, r12, r2, r3, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
	QUARTER16_OFFSET_V_ROW  q3, r12, r2, r3, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5
	QUARTER16_OFFSET_V_ROW  q4, r12, r2, r3, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
	QUARTER16_OFFSET_V_ROW  q5, r12, r2, r3, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
	subs        r6, r6, #1
       bne         avg_offset_luma_v_16x16_loop
	QUARTER16_OFFSET_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	QUARTER16_OFFSET_V_ROW  q1, r12, r2, r3, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	QUARTER16_OFFSET_V_ROW  q2, r12, r2, r3, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	pop      	{r4 - r7, pc}

avg_offset_luma_v_16x8_ARMV7:
	push     	{r4 - r7, r14}
	ldr		r4, [sp, #20]
	ldr		r5, [sp, #24]
	ldr          r7, [sp, #28]
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
	vdup.16     q10,  r7
       vaddw.u8  q8, q10, d24
       vaddw.u8  q9, q10, d25
       vqmovun.s16     d24, q8
       vqmovun.s16     d25, q9
	vst1.64     {q12}, [r2],  r3
	QUARTER16_OFFSET_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
	QUARTER16_OFFSET_V_ROW  q1, r12, r2, r3, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
	QUARTER16_OFFSET_V_ROW  q2, r12, r2, r3, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
	QUARTER16_OFFSET_V_ROW  q3, r12, r2, r3, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5
	QUARTER16_OFFSET_V_ROW  q4, r12, r2, r3, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
	QUARTER16_OFFSET_V_ROW  q5, r12, r2, r3, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
	QUARTER16_OFFSET_V_ROW  q0, r12, r2, r3, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
	pop      	{r4 - r7, pc}

avg_offset_luma_v_8x16_ARMV7:
	push     	{r4 - r7, r14}
	mov		r6, #2
	ldr		r4, [sp, #20]
	ldr		r5, [sp, #24]
	ldr          r7, [sp, #28]
	sub         r0, r0, r1, LSL #1  @src = src - 2*stride
	vld1.64		{d0},  [r0], r1		@src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @r12 = src + 3*stride
	vld1.64		{d2},  [r0], r1		@src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@coeff 5
	vdup.16     q7,  r7
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
	vaddw.u8  q8, q7, d24
       vqmovun.s16     d24, q8
	vst1.64     {d24}, [r2],  r3
avg_offset_luma_v_8x16_loop:
	QUARTER8_OFFSET_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	QUARTER8_OFFSET_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	QUARTER8_OFFSET_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	QUARTER8_OFFSET_V_ROW  d6, r12, r2, r3, d0, d2, d8, d6, d10, d4
	QUARTER8_OFFSET_V_ROW  d8, r12, r2, r3, d2, d4, d10, d8, d0, d6
	QUARTER8_OFFSET_V_ROW  d10, r12, r2, r3, d4, d6, d0, d10, d2, d8
	subs        r6, r6, #1
       bne         avg_offset_luma_v_8x16_loop
	QUARTER8_OFFSET_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	QUARTER8_OFFSET_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	QUARTER8_OFFSET_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	pop      	{r4 - r7, pc}

avg_offset_luma_v_8x8_ARMV7:
	push     	{r4 - r7, r14}
	ldr		r4, [sp, #20]
	ldr		r5, [sp, #24]
	ldr          r7, [sp, #28]
	sub         r0, r0, r1, LSL #1  @src = src - 2*stride
	vld1.64		{d0},  [r0], r1		@src - 2*stride a0 a1 ... a15
	add         r12, r0, r1, LSL #2 @r12 = src + 3*stride
	vld1.64		{d2},  [r0], r1		@src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			@coeff 5
	vdup.16     q7,  r7
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
	vaddw.u8  q8, q7, d24
       vqmovun.s16     d24, q8
	vst1.64     {d24}, [r2],  r3
	QUARTER8_OFFSET_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	QUARTER8_OFFSET_V_ROW  d2, r12, r2, r3, d8, d10,d4, d2, d6, d0
	QUARTER8_OFFSET_V_ROW  d4, r12, r2, r3, d10, d0, d6, d4, d8, d2
	QUARTER8_OFFSET_V_ROW  d6, r12, r2, r3, d0, d2, d8, d6, d10, d4
	QUARTER8_OFFSET_V_ROW  d8, r12, r2, r3, d2, d4, d10, d8, d0, d6
	QUARTER8_OFFSET_V_ROW  d10, r12, r2, r3, d4, d6, d0, d10, d2, d8
	QUARTER8_OFFSET_V_ROW  d0, r12, r2, r3, d6, d8, d2, d0, d4, d10
	pop      	{r4 - r7, pc}

offset_luma_h_16x16_ARMV7:
	push     	{r4 - r5, r14}
	ldr		r4, [sp, #12]
       mov         r5, #4
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
offset_luma_h_16x16_loop:
	HALF16_OFFSET_H_ROW r2, r3
	HALF16_OFFSET_H_ROW r2, r3
	HALF16_OFFSET_H_ROW r2, r3
	HALF16_OFFSET_H_ROW r2, r3
	subs        r5, r5, #1
       bne         offset_luma_h_16x16_loop
	pop      	{r4 - r5, pc}

offset_luma_h_16x8_ARMV7:
	push     	{r4 - r5, r14}
	ldr		r4, [sp, #12]
       mov         r5, #2
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
offset_luma_h_16x8_loop:
	HALF16_OFFSET_H_ROW r2, r3
	HALF16_OFFSET_H_ROW r2, r3
	HALF16_OFFSET_H_ROW r2, r3
	HALF16_OFFSET_H_ROW r2, r3
	subs        r5, r5, #1
       bne         offset_luma_h_16x8_loop
	pop      	{r4 - r5, pc}

offset_luma_h_8x16_ARMV7:
	push     	{r4 - r5, r14}
	ldr		r4, [sp, #12]
       mov         r5, #4
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
	vdup.16     q7,  r4
offset_luma_h_8x16_loop:
	HALF8_OFFSET_H_ROW r2, r3
	HALF8_OFFSET_H_ROW r2, r3
	HALF8_OFFSET_H_ROW r2, r3
	HALF8_OFFSET_H_ROW r2, r3
	subs        r5, r5, #1
       bne         offset_luma_h_8x16_loop
	pop      	{r4 - r5, pc}

offset_luma_h_8x8_ARMV7:
	push     	{r4 - r5, r14}
	ldr		r4, [sp, #12]
       mov         r5, #2
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
	vdup.16     q7,  r4
offset_luma_h_8x8_loop:
	HALF8_OFFSET_H_ROW r2, r3
	HALF8_OFFSET_H_ROW r2, r3
	HALF8_OFFSET_H_ROW r2, r3
	HALF8_OFFSET_H_ROW r2, r3
	subs        r5, r5, #1
       bne         offset_luma_h_8x8_loop
	pop      	{r4 - r5, pc}

avg_offset_luma_h_16x16_ARMV7:
  	push     	{r4 - r7, r14}
  	mov		r6, #4
	ldr		r4, [sp, #20]
	ldr		r5, [sp, #24]
	ldr		r7, [sp, #28]
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
avg_offset_luma_h_16x16_loop:
	QUARTER16_OFFSET_H_ROW
	QUARTER16_OFFSET_H_ROW 
	QUARTER16_OFFSET_H_ROW 
	QUARTER16_OFFSET_H_ROW 
	subs        r6, r6, #1
       bne         avg_offset_luma_h_16x16_loop
	pop      	{r4 - r7, pc}

avg_offset_luma_h_16x8_ARMV7:
  	push     	{r4 - r7, r14}
  	mov		r6, #2
	ldr		r4, [sp, #20]
	ldr		r5, [sp, #24]
	ldr		r7, [sp, #28]
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
avg_offset_luma_h_16x8_loop:
	QUARTER16_OFFSET_H_ROW
	QUARTER16_OFFSET_H_ROW 
	QUARTER16_OFFSET_H_ROW 
	QUARTER16_OFFSET_H_ROW 
	subs        r6, r6, #1
       bne         avg_offset_luma_h_16x8_loop
	pop      	{r4 - r7, pc}

avg_offset_luma_h_8x16_ARMV7:
  	push     	{r4 - r7, r14}
  	mov		r6, #4
	ldr		r4, [sp, #20]
	ldr		r5, [sp, #24]
	ldr		r7, [sp, #28]
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
	vdup.16     q7,  r7
avg_offset_luma_h_8x16_loop:
	QUARTER8_OFFSET_H_ROW
	QUARTER8_OFFSET_H_ROW 
	QUARTER8_OFFSET_H_ROW 
	QUARTER8_OFFSET_H_ROW 
	subs        r6, r6, #1
       bne         avg_offset_luma_h_8x16_loop
	pop      	{r4 - r7, pc}
	
avg_offset_luma_h_8x8_ARMV7:
  	push     	{r4 - r7, r14}
  	mov		r6, #2
	ldr		r4, [sp, #20]
	ldr		r5, [sp, #24]
	ldr		r7, [sp, #28]
	sub         r0, r0, #2  		@;src = src - 2
	add			r12,  r0,  #16		@;src + 16
	vmov.i16    q14,  #5			@;coeff 5
	vmov.i16    q15,  #20			@;coeff 20
	vdup.16     q7,  r7
avg_offset_luma_h_8x8_loop:
	QUARTER8_OFFSET_H_ROW
	QUARTER8_OFFSET_H_ROW 
	QUARTER8_OFFSET_H_ROW 
	QUARTER8_OFFSET_H_ROW 
	subs        r6, r6, #1
       bne         avg_offset_luma_h_8x8_loop
	pop      	{r4 - r7, pc}

offset_block_00_16x16_ARMV7:
       push     	{r4 , r14}
       ldr         r4, [sp, #8]
	vdup.16     q14,  r4
	mov		  r4, #4
offset_block_00_16x16_loop:
	vld1.64	  {q0},  [r0], r1
	vaddw.u8  q1, q14, d0
	vaddw.u8  q2, q14, d1
	vqmovun.s16     d6, q1
  	vqmovun.s16     d7, q2
	vst1.64     {q3}, [r2],  r3
	vld1.64	  {q0},  [r0], r1
	vaddw.u8  q1, q14, d0
	vaddw.u8  q2, q14, d1
	vqmovun.s16     d6, q1
  	vqmovun.s16     d7, q2
	vst1.64     {q3}, [r2],  r3
	vld1.64	  {q0},  [r0], r1
	vaddw.u8  q1, q14, d0
	vaddw.u8  q2, q14, d1
	vqmovun.s16     d6, q1
  	vqmovun.s16     d7, q2
	vst1.64     {q3}, [r2],  r3
	vld1.64	  {q0},  [r0], r1
	vaddw.u8  q1, q14, d0
	vaddw.u8  q2, q14, d1
	vqmovun.s16     d6, q1
  	vqmovun.s16     d7, q2
	vst1.64     {q3}, [r2],  r3
	subs        r4, r4, #1
       bne         offset_block_00_16x16_loop		
	pop      	{r4 , pc}

offset_block_00_16x8_ARMV7:
       push     	{r4 , r14}
       ldr         r4, [sp, #8]
	vdup.16     q14,  r4
	mov		  r4, #2
offset_block_00_16x8_loop:
	vld1.64	  {q0},  [r0], r1
	vaddw.u8  q1, q14, d0
	vaddw.u8  q2, q14, d1
	vqmovun.s16     d6, q1
  	vqmovun.s16     d7, q2
	vst1.64     {q3}, [r2],  r3
	vld1.64	  {q0},  [r0], r1
	vaddw.u8  q1, q14, d0
	vaddw.u8  q2, q14, d1
	vqmovun.s16     d6, q1
  	vqmovun.s16     d7, q2
	vst1.64     {q3}, [r2],  r3
	vld1.64	  {q0},  [r0], r1
	vaddw.u8  q1, q14, d0
	vaddw.u8  q2, q14, d1
	vqmovun.s16     d6, q1
  	vqmovun.s16     d7, q2
	vst1.64     {q3}, [r2],  r3
	vld1.64	  {q0},  [r0], r1
	vaddw.u8  q1, q14, d0
	vaddw.u8  q2, q14, d1
	vqmovun.s16     d6, q1
  	vqmovun.s16     d7, q2
	vst1.64     {q3}, [r2],  r3
	subs        r4, r4, #1
       bne         offset_block_00_16x8_loop		
	pop      	{r4 , pc}
  
offset_block_00_8x16_ARMV7:
       push     	{r4 , r14}
       ldr         r4, [sp, #8]
	vdup.16     q14,  r4
	mov		  r4, #4
offset_block_00_8x16_loop:
	vld1.64	  {d0},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d6, q1
	vst1.64     {d6}, [r2],  r3
	vld1.64	  {d0},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d6, q1
	vst1.64     {d6}, [r2],  r3
	vld1.64	  {d0},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d6, q1
	vst1.64     {d6}, [r2],  r3
	vld1.64	  {d0},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d6, q1
	vst1.64     {d6}, [r2],  r3
	subs        r4, r4, #1
       bne         offset_block_00_8x16_loop		
	pop      	{r4 , pc}

offset_block_00_8x8_ARMV7:
       push     	{r4 , r14}
       ldr         r4, [sp, #8]
	vdup.16     q14,  r4
	mov		  r4, #2
offset_block_00_8x8_loop:
	vld1.64	  {d0},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d6, q1
	vst1.64     {d6}, [r2],  r3
	vld1.64	  {d0},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d6, q1
	vst1.64     {d6}, [r2],  r3
	vld1.64	  {d0},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d6, q1
	vst1.64     {d6}, [r2],  r3
	vld1.64	  {d0},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d6, q1
	vst1.64     {d6}, [r2],  r3
	subs        r4, r4, #1
       bne         offset_block_00_8x8_loop		
	pop      	{r4 , pc}

offset_block_00_8x4_ARMV7:
       push     	{r4 , r14}
       ldr         r4, [sp, #8]
	vdup.16     q14,  r4
	vld1.64	  {d0},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d6, q1
	vst1.64     {d6}, [r2],  r3
	vld1.64	  {d0},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d6, q1
	vst1.64     {d6}, [r2],  r3
	vld1.64	  {d0},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d6, q1
	vst1.64     {d6}, [r2],  r3
	vld1.64	  {d0},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d6, q1
	vst1.64     {d6}, [r2],  r3		
	pop      	{r4 , pc}

offset_block_00_4x8_ARMV7:
       push     	{r4 , r14}
       ldr         r4, [sp, #8]
	vdup.16     q14,  r4
	vld1.32	  {d0[0]},  [r0], r1
	vld1.32	  {d0[1]},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d6, q1
	vst1.32     {d6[0]}, [r2],  r3
	vst1.32     {d6[1]}, [r2],  r3
	vld1.32	  {d0[0]},  [r0], r1
	vld1.32	  {d0[1]},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d6, q1
	vst1.32     {d6[0]}, [r2],  r3
	vst1.32     {d6[1]}, [r2],  r3	
	vld1.32	  {d0[0]},  [r0], r1
	vld1.32	  {d0[1]},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d6, q1
	vst1.32     {d6[0]}, [r2],  r3
	vst1.32     {d6[1]}, [r2],  r3
	vld1.32	  {d0[0]},  [r0], r1
	vld1.32	  {d0[1]},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d6, q1
	vst1.32     {d6[0]}, [r2],  r3
	vst1.32     {d6[1]}, [r2],  r3
	pop      	{r4 , pc}

offset_block_00_4x4_ARMV7:
       push     	{r4 , r14}
       ldr         r4, [sp, #8]
	vdup.16     q14,  r4
	vld1.32	  {d0[0]},  [r0], r1
	vld1.32	  {d0[1]},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d6, q1
	vst1.32     {d6[0]}, [r2],  r3
	vst1.32     {d6[1]}, [r2],  r3
	vld1.32	  {d0[0]},  [r0], r1
	vld1.32	  {d0[1]},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d6, q1
	vst1.32     {d6[0]}, [r2],  r3
	vst1.32     {d6[1]}, [r2],  r3		
	pop      	{r4 , pc}
	
offset_block_00_4x2_ARMV7:
       push     	{r4 , r14}
       ldr         r4, [sp, #8]
	vdup.16     q14,  r4
	vld1.32	  {d0[0]},  [r0], r1
	vld1.32	  {d0[1]},  [r0], r1
	vaddw.u8  q1, q14, d0
	vqmovun.s16     d6, q1
	vst1.32     {d6[0]}, [r2],  r3
	vst1.32     {d6[1]}, [r2],  r3		
	pop      	{r4 , pc}

  .end
