@************************************************************************
@									                                    *
@	VisualOn, Inc Confidential and Proprietary, 2005		            *
@								 	                                    *
@***********************************************************************/

	@AREA	text, CODE
	.section .text
	
	.global MPEG4DEC_VO_Armv7IdctA
	.global MPEG4DEC_VO_Armv7IdctB
	.global MPEG4DEC_VO_Armv7IdctC

	.align 4
 .macro LOAD_IDCT_SRC
 vld1.64 {d0}, [r3], r12 
 vld1.64 {d1}, [r3], r12 
 vld1.64 {d2}, [r3], r12 
 vld1.64 {d3}, [r3], r12 
 vld1.64 {d4}, [r3], r12 
 vld1.64 {d5}, [r3], r12 
 vld1.64 {d6}, [r3], r12 
 vld1.64 {d7}, [r3]
 .endm 

 .macro LOAD_IDCT_SRCv6
	 ldrd r4, [r3], r12
	 ldrd r6, [r3], r12
	 ldrd r8, [r3], r12
	 ldrd r10, [r3], r12
	 vmov d0, r4, r5
	 vmov d1, r6, r7
	 vmov d2, r8, r9
	 vmov d3, r10, r11

	 ldrd r4, [r3], r12
	 ldrd r6, [r3], r12
	 ldrd r8, [r3], r12
	 ldrd r10, [r3]
	 vmov d4, r4, r5
	 vmov d5, r6, r7
	 vmov d6, r8, r9
	 vmov d7, r10, r11
	.endm 

 .macro MPEG4DEC_VO_Armv7IdctC_VST
   vst1.64 {d0}, [r1], r2   
   vst1.64 {d1}, [r1], r2
   vst1.64 {d2}, [r1], r2
   vst1.64 {d3}, [r1], r2
   vst1.64 {d4}, [r1], r2
   vst1.64 {d5}, [r1], r2
   vst1.64 {d6}, [r1], r2
   vst1.64 {d7}, [r1]
	.endm	

 .macro LOAD_IDCT4x8_COEF		
	mov	r12, #16
 	vmov.s32 d24, #0
	vld1.64	{d8}, [r0], r12		@q4 x0 x1 d8-d9
	vld1.64	{d10}, [r0], r12	@q5 x4 x5 d10-d11
	vld1.64	{d12}, [r0], r12	@q6 x3 x2 d12-d13
	vld1.64	{d14}, [r0], r12	@q7 x7 x6 d14-d15
	vld1.64	{d9}, [r0], r12		@
	vld1.64	{d15}, [r0], r12	@
	vld1.64	{d13}, [r0], r12	@
	vld1.64	{d11}, [r0]		@	
	sub	r0, r0, #112	
	vst1.64	{d24}, [r0], r12	@
	vst1.64	{d24}, [r0], r12	@
	vst1.64	{d24}, [r0], r12	@
	vst1.64	{d24}, [r0], r12	@
	vst1.64	{d24}, [r0], r12	@
	vst1.64	{d24}, [r0], r12	@
	vst1.64	{d24}, [r0], r12	@
	vst1.64	{d24}, [r0]		@			
	.endm
 .macro LOAD_IDCT8x8_COEF		
 	vmov.s32 q12, #0		
	vld1.64	{q4}, [r0]!		@q4 x0 x1 d8-d9
	vld1.64	{q5}, [r0]!		@q5 x4 x5 d10-d11
	vld1.64	{q6}, [r0]!		@q6 x3 x2 d12-d13
	vld1.64	{q7}, [r0]!		@q7 x7 x6 d14-d15
	vld1.64	{q8}, [r0]!		@
	vld1.64	{q9}, [r0]!		@
	vld1.64	{q10}, [r0]!	@
	vld1.64	{q11}, [r0]		@	
	
	sub	r0, r0, #112
	vst1.64	{q12}, [r0]!		@	
	vst1.64	{q12}, [r0]!		@	
	vst1.64	{q12}, [r0]!		@	
	vst1.64	{q12}, [r0]!		@	
	vst1.64	{q12}, [r0]!		@	
	vst1.64	{q12}, [r0]!		@	
	vst1.64	{q12}, [r0]!		@	
	vst1.64	{q12}, [r0]		@		
	.endm	
 .macro LOAD_IDCT_SRC_ADD		
	ldr		r12, [sp]		@ r12 = src_stride
	LOAD_IDCT_SRC			
	vaddw.u8	q8, q8, d0
	vaddw.u8	q9, q9, d1
	vaddw.u8	q10, q10, d2
	vaddw.u8	q11, q11, d3
	vaddw.u8	q4, q4, d4
	vaddw.u8	q5, q5, d5
	vaddw.u8	q6, q6, d6
	vaddw.u8	q7, q7, d7	
	.endm
 .macro STR_IDCT
	vqmovun.s16	d16, q8
	vqmovun.s16	d18, q9
	vqmovun.s16	d20, q10
	vqmovun.s16	d22, q11
	vqmovun.s16	d8, q4
	vqmovun.s16	d10, q5
	vqmovun.s16	d12, q6
	vqmovun.s16	d14, q7		
						
	vst1.64	{d16}, [r1], r2
	vst1.64	{d18}, [r1], r2		
	vst1.64	{d20}, [r1], r2
	vst1.64	{d22}, [r1], r2	
	vst1.64	{d8}, [r1], r2
	vst1.64	{d10}, [r1], r2	
	vst1.64	{d12}, [r1], r2
	vst1.64	{d14}, [r1]
	.endm
 .macro Col8_first4line
	vshll.s16	q12, d9, #11		@x1 = Blk[32] << 11@
	vshll.s16	q13, d8, #11
	vadd.s32	q13, q13, q1		@x0 = (x0 << 11) + 128@

@	x4 = W7*x5 + W1*x4@
@	x5 = W7*x4 - W1*x5@

	vmull.s16	q14, d11, D0[0]			@W7*x5
	vmlal.s16	q14, d10, D0[1]			@x4 = W7*x5 + W1*x4@
	
	vmull.s16	q15, d10, D0[0]			@W7*x4
	vmlsl.s16	q15, d11, D0[1]			@x5 = W7*x4 - W1*x5@
	
@	x6 = W3*x7 - W5*x6
@	x7 = W3*x6 - W5*x7@	

	vmull.s16	q4, d14, D0[2]			@W3*x7
	vmlal.s16	q4, d15, D0[3]			@x6 = W3*x7 + W5*x6
	
	vmull.s16	q5, d15, D0[2]			@W3*x6
	vmlsl.s16	q5, d14, D0[3]			@x7 = W3*x6 - W5*x7@
@	x3 = W6*x2 + W2*x3@
@	x2 = W6*x3 - W2*x2@

	vmull.s16	q7, d13, D1[0]			@W6*x2
	vmlal.s16	q7, d12, D1[1]			@x3 = W6*x2 + W2*x3@
	
	vmull.s16	q3, d12, D1[0]			@W6*x3
	vmlsl.s16	q3, d13, D1[1]			@x2 = W6*x3 - W2*x2@
	
@	x8 = x0 + x1@
@	x0 -= x1@
	vadd.s32	q6, q13, q12		@x8 = x0 + x1@
	vsub.s32	q13, q13, q12		@x0 -= x1@
		
@	x1 = x4 + x6@
@	x4 -= x6@
@	x6 = x5 + x7@
@	x5 -= x7@
@	x7 = x8 + x3@
@	x8 -= x3@
@	x3 = x0 + x2@
@	x0 -= x2@

	vadd.s32	q12, q14, q4		@x1 = x4 + x6@
	vsub.s32	q14, q14, q4		@x4 -= x6@
	
	vadd.s32	q4, q15, q5			@x6 = x5 + x7@
	vsub.s32	q15, q15, q5		@x5 -= x7@
	
	vadd.s32	q5, q6, q7			@x7 = x8 + x3@
	vsub.s32	q6, q6, q7			@x8 -= x3@
	
	vadd.s32	q7, q13, q3			@x3 = x0 + x2@
	vsub.s32	q13, q13, q3		@x0 -= x2@

@	x2 = (181 * (x4 + x5) + 128) >> 8@
@	x4 = (181 * (x4 - x5) + 128) >> 8@
					
	vadd.s32	q3, q14, q15
	vmul.s32	q3, q3, D1[1]
	vadd.s32	q3, q3, q1			
	vshr.s32	q3, q3, #8		@x2 = (181 * (x4 + x5) + 128) >> 8@

	vsub.s32	q14, q14, q15	
	vmul.s32	q14, q14, D1[1]
	vadd.s32	q14, q14, q1			
	vshr.s32	q14, q14, #8		@x4 = (181 * (x4 - x5) + 128) >> 8@
		
@	Blk[0] = (idct_t)((x7 + x1) >> 8)@
@	Blk[8] = (idct_t)((x3 + x2) >> 8)@
@	Blk[16] = (idct_t)((x0 + x4) >> 8)@
@	Blk[24] = (idct_t)((x8 + x6) >> 8)@
@	Blk[32] = (idct_t)((x8 - x6) >> 8)@
@	Blk[40] = (idct_t)((x0 - x4) >> 8)@
@	Blk[48] = (idct_t)((x3 - x2) >> 8)@
@	Blk[56] = (idct_t)((x7 - x1) >> 8)@

	vadd.s32	q15, q5, q12		@Blk[ 0] = (idct_t)((x7 + x1) >> 8)@
	vsub.s32	q5, q5, q12			@Blk[56] = (idct_t)((x7 - x1) >> 8)@
	vadd.s32	q12, q7, q3			@Blk[8] = (idct_t)((x3 + x2) >> 8)@
	vsub.s32	q7, q7, q3			@Blk[48] = (idct_t)((x3 - x2) >> 8)@
	vadd.s32	q3, q13, q14		@Blk[16] = (idct_t)((x0 + x4) >> 8)@
	vsub.s32	q13, q13, q14		@Blk[40] = (idct_t)((x0 - x4) >> 8)@
	vadd.s32	q14, q6, q4			@Blk[24] = (idct_t)((x8 + x6) >> 8)@
	vsub.s32	q6, q6, q4			@Blk[32] = (idct_t)((x8 - x6) >> 8)@
 .endm
 
 .macro Col8_last4line

	vmov.s32	q3, #128	
	vshll.s16	q12, d17, #11		@x1 = Blk[32] << 11@
	vshll.s16	q13, d16, #11
	vadd.s32	q13, q13, q3		@x0 = (x0 << 11) + 128@

@	x4 = W7*x5 + W1*x4@
@	x5 = W7*x4 - W1*x5@

	vmull.s16	q14, d23, D0[0]			@W7*x5
	vmlal.s16	q14, d22, D0[1]			@x4 = W7*x5 + W1*x4@
	
	vmull.s16	q15, d22, D0[0]			@W7*x4
	vmlsl.s16	q15, d23, D0[1]			@x5 = W7*x4 - W1*x5@
	
@	x6 = W3*x7 - W5*x6
@	x7 = W3*x6 - W5*x7@	

	vmull.s16	q8, d18, D0[2]			@W3*x7
	vmlal.s16	q8, d19, D0[3]			@x6 = W3*x7 + W5*x6
	
	vmull.s16	q11, d19, D0[2]			@W3*x6
	vmlsl.s16	q11, d18, D0[3]			@x7 = W3*x6 - W5*x7@
@	x3 = W6*x2 + W2*x3@
@	x2 = W6*x3 - W2*x2@

	vmull.s16	q9, d21, D1[0]			@W6*x2
	vmlal.s16	q9, d20, D1[1]			@x3 = W6*x2 + W2*x3@
	
	vmull.s16	q3, d20, D1[0]			@W6*x3
	vmlsl.s16	q3, d21, D1[1]			@x2 = W6*x3 - W2*x2@
	
@	x8 = x0 + x1@
@	x0 -= x1@
	vadd.s32	q10, q13, q12		@x8 = x0 + x1@
	vsub.s32	q13, q13, q12		@x0 -= x1@
		
@	x1 = x4 + x6@
@	x4 -= x6@
@	x6 = x5 + x7@
@	x5 -= x7@
@	x7 = x8 + x3@
@	x8 -= x3@
@	x3 = x0 + x2@
@	x0 -= x2@

	vadd.s32	q12, q14, q8		@x1 = x4 + x6@
	vsub.s32	q14, q14, q8		@x4 -= x6@
	
	vadd.s32	q8, q15, q11			@x6 = x5 + x7@
	vsub.s32	q15, q15, q11		@x5 -= x7@
	
	vadd.s32	q11, q10, q9			@x7 = x8 + x3@
	vsub.s32	q10, q10, q9			@x8 -= x3@
	
	vadd.s32	q9, q13, q3			@x3 = x0 + x2@
	vsub.s32	q13, q13, q3		@x0 -= x2@

@	x2 = (181 * (x4 + x5) + 128) >> 8@
@	x4 = (181 * (x4 - x5) + 128) >> 8@

					
	vadd.s32	q3, q14, q15
	vmul.s32	q3, q3, D1[1]
	vsub.s32	q14, q14, q15	
	vmul.s32	q14, q14, D1[1]
	
	vmov.s32	q15, #128	
		
	vadd.s32	q3, q3, q15			
	vshr.s32	q3, q3, #8		@x2 = (181 * (x4 + x5) + 128) >> 8@
	vadd.s32	q14, q14, q15			
	vshr.s32	q14, q14, #8		@x4 = (181 * (x4 - x5) + 128) >> 8@
		
@	Blk[0] = (idct_t)((x7 + x1) >> 8)@
@	Blk[8] = (idct_t)((x3 + x2) >> 8)@
@	Blk[16] = (idct_t)((x0 + x4) >> 8)@
@	Blk[24] = (idct_t)((x8 + x6) >> 8)@
@	Blk[32] = (idct_t)((x8 - x6) >> 8)@
@	Blk[40] = (idct_t)((x0 - x4) >> 8)@
@	Blk[48] = (idct_t)((x3 - x2) >> 8)@
@	Blk[56] = (idct_t)((x7 - x1) >> 8)@

	vadd.s32	q15, q11, q12		@Blk[ 0] = (idct_t)((x7 + x1) >> 8)@
	vsub.s32	q11, q11, q12			@Blk[56] = (idct_t)((x7 - x1) >> 8)@
	vadd.s32	q12, q9, q3			@Blk[8] = (idct_t)((x3 + x2) >> 8)@
	vsub.s32	q9, q9, q3			@Blk[48] = (idct_t)((x3 - x2) >> 8)@
	vadd.s32	q3, q13, q14		@Blk[16] = (idct_t)((x0 + x4) >> 8)@
	vsub.s32	q13, q13, q14		@Blk[40] = (idct_t)((x0 - x4) >> 8)@
	vadd.s32	q14, q10, q8			@Blk[24] = (idct_t)((x8 + x6) >> 8)@
	vsub.s32	q10, q10, q8			@Blk[32] = (idct_t)((x8 - x6) >> 8)@
	.endm

 .macro Row_8x8line is_8x8
			
@row tow
	vmov.s32	q3, #8192	
	.if \is_8x8>0
	vshll.s16	q12, d17, #8		@x1 = Blk[32] << 8@
	.endif
	vshll.s16	q13, d16, #8
	vadd.s32	q13, q13, q3		@x0 = (x0 << 8) + 128@

@	x4 = W7*x5 + W1*x4@
@	x5 = W7*x4 - W1*x5@

	.if \is_8x8 > 0
	vmull.s16	q14, d23, D0[0]			@W7*x5
	vmlal.s16	q14, d22, D0[1]			@x4 = W7*x5 + W1*x4@
	
	vmull.s16	q15, d22, D0[0]			@W7*x4
	vmlsl.s16	q15, d23, D0[1]			@x5 = W7*x4 - W1*x5@
	
@	x6 = W3*x7 - W5*x6
@	x7 = W3*x6 - W5*x7@	

	vmull.s16	q8, d18, D0[2]			@W3*x7
	vmlal.s16	q8, d19, D0[3]			@x6 = W3*x7 + W5*x6
	
	vmull.s16	q11, d19, D0[2]			@W3*x6
	vmlsl.s16	q11, d18, D0[3]			@x7 = W3*x6 - W5*x7@
@	x3 = W6*x2 + W2*x3@
@	x2 = W6*x3 - W2*x2@

	vmull.s16	q9, d21, D1[0]			@W6*x2
	vmlal.s16	q9, d20, D1[1]			@x3 = W6*x2 + W2*x3@
	
	vmull.s16	q3, d20, D1[0]			@W6*x3
	vmlsl.s16	q3, d21, D1[1]			@x2 = W6*x3 - W2*x2@
	.else
@x5 = 0
@	vmull.s16	q14, d23, D0[0]			@W7*x5
	vmull.s16	q14, d22, D0[1]			@x4 = W7*x5 + W1*x4@
	
	vmull.s16	q15, d22, D0[0]			@W7*x4
@	vmlsl.s16	q15, d23, D0[1]			@x5 = W7*x4 - W1*x5@
	

@x6 = 0
	vmull.s16	q8, d18, D0[2]			@W3*x7
@	vmlal.s16	q8, d19, D0[3]			@x6 = W3*x7 + W5*x6
	
@ x7 = -x7	
@	vmull.s16	q11, d19, D0[2]			@W3*x6
	vmull.s16	q11, d18, D0[3]			@x7 = W3*x6 - W5*x7@

@x2 = 0
@	vmull.s16	q9, d21, D1[0]			@W6*x2
	vmull.s16	q9, d20, D1[1]			@x3 = W6*x2 + W2*x3@
	
	vmull.s16	q3, d20, D1[0]			@W6*x3
@	vmlsl.s16	q3, d21, D1[1]			@x2 = W6*x3 - W2*x2@

	.endif
	
	@((x4, x5, x6, x7, x3, x2) + 4)>>3
	vmov.s32	q10, #4
	vadd.s32	q14, q14, q10
	vadd.s32	q15, q15, q10
	vadd.s32	q8, q8, q10
	.if \is_8x8 > 0
	vadd.s32	q11, q11, q10
	.else
	vsub.s32	q11, q10, q11				@x7 = 4 - x7
	.endif
	vadd.s32	q9, q9, q10
	vadd.s32	q3, q3, q10					
	vshr.s32	q14, q14, #3
	vshr.s32	q15, q15, #3	
	vshr.s32	q8, q8, #3
	vshr.s32	q11, q11, #3
	vshr.s32	q9, q9, #3
	vshr.s32	q3, q3, #3		
		
@	x8 = x0 + x1@
@	x0 -= x1@
	.if \is_8x8 > 0
	vadd.s32	q10, q13, q12		@x8 = x0 + x1@
	vsub.s32	q13, q13, q12		@x0 -= x1@
	.endif
		
@	x1 = x4 + x6@
@	x4 -= x6@
@	x6 = x5 + x7@
@	x5 -= x7@
@	x7 = x8 + x3@
@	x8 -= x3@
@	x3 = x0 + x2@
@	x0 -= x2@

	vadd.s32	q12, q14, q8		@x1 = x4 + x6@
	vsub.s32	q14, q14, q8		@x4 -= x6@
	
	vadd.s32	q8, q15, q11			@x6 = x5 + x7@
	vsub.s32	q15, q15, q11		@x5 -= x7@

	.if \is_8x8 > 0
	vadd.s32	q11, q10, q9			@x7 = x8 + x3@
	vsub.s32	q10, q10, q9			@x8 -= x3@
	.else
	vadd.s32	q11, q13, q9			@x7 = x8 + x3@
	vsub.s32	q10, q13, q9			@x8 -= x3@
	.endif
	
	
	vadd.s32	q9, q13, q3			@x3 = x0 + x2@
	vsub.s32	q13, q13, q3		@x0 -= x2@

@	x2 = (181 * (x4 + x5) + 128) >> 8@
@	x4 = (181 * (x4 - x5) + 128) >> 8@

					
	vadd.s32	q3, q14, q15
	vmul.s32	q3, q3, D1[1]
	vsub.s32	q14, q14, q15	
	vmul.s32	q14, q14, D1[1]
	
	vmov.s32	q15, #128	
	vadd.s32	q3, q3, q15			
	vshr.s32	q3, q3, #8		@x2 = (181 * (x4 + x5) + 128) >> 8@
	vadd.s32	q14, q14, q15			
	vshr.s32	q14, q14, #8		@x4 = (181 * (x4 - x5) + 128) >> 8@
		
@	Blk[0] = (idct_t)((x7 + x1) >> 8)@
@	Blk[8] = (idct_t)((x3 + x2) >> 8)@
@	Blk[16] = (idct_t)((x0 + x4) >> 8)@
@	Blk[24] = (idct_t)((x8 + x6) >> 8)@
@	Blk[32] = (idct_t)((x8 - x6) >> 8)@
@	Blk[40] = (idct_t)((x0 - x4) >> 8)@
@	Blk[48] = (idct_t)((x3 - x2) >> 8)@
@	Blk[56] = (idct_t)((x7 - x1) >> 8)@

	vadd.s32	q15, q11, q12		@Blk[ 0] = (idct_t)((x7 + x1) >> 8)@
	vsub.s32	q11, q11, q12			@Blk[56] = (idct_t)((x7 - x1) >> 8)@
	vadd.s32	q12, q9, q3			@Blk[8] = (idct_t)((x3 + x2) >> 8)@
	vsub.s32	q9, q9, q3			@Blk[48] = (idct_t)((x3 - x2) >> 8)@
	vadd.s32	q3, q13, q14		@Blk[16] = (idct_t)((x0 + x4) >> 8)@
	vsub.s32	q13, q13, q14		@Blk[40] = (idct_t)((x0 - x4) >> 8)@
	vadd.s32	q14, q10, q8			@Blk[24] = (idct_t)((x8 + x6) >> 8)@
	vsub.s32	q10, q10, q8			@Blk[32] = (idct_t)((x8 - x6) >> 8)@
	
	
	vshrn.s32	d2, q10, #14
	vshrn.s32	d3, q13, #14
	vshrn.s32	d4, q9, #14
	vshrn.s32	d5, q11, #14
			
	vshrn.s32	d16, q15, #14
	vshrn.s32	d18, q12, #14	
	vshrn.s32	d20, q3, #14
	vshrn.s32	d22, q14, #14	

@row one

	vmov.s32	q3, #8192	
	.if \is_8x8 > 0
	vshll.s16	q12, d9, #8		@x1 = Blk[32] << 8@
	.endif

	vshll.s16	q13, d8, #8
	vadd.s32	q13, q13, q3		@x0 = (x0 << 8) + 8192@

@	x4 = W7*x5 + W1*x4@
@	x5 = W7*x4 - W1*x5@
	.if \is_8x8 > 0

	vmull.s16	q14, d11, D0[0]			@W7*x5
	vmlal.s16	q14, d10, D0[1]			@x4 = W7*x5 + W1*x4@
	
	vmull.s16	q15, d10, D0[0]			@W7*x4
	vmlsl.s16	q15, d11, D0[1]			@x5 = W7*x4 - W1*x5@
	
@	x6 = W3*x7 - W5*x6
@	x7 = W3*x6 - W5*x7@	

	vmull.s16	q4, d14, D0[2]			@W3*x7
	vmlal.s16	q4, d15, D0[3]			@x6 = W3*x7 + W5*x6
	
	vmull.s16	q5, d15, D0[2]			@W3*x6
	vmlsl.s16	q5, d14, D0[3]			@x7 = W3*x6 - W5*x7@
@	x3 = W6*x2 + W2*x3@
@	x2 = W6*x3 - W2*x2@

	vmull.s16	q7, d13, D1[0]			@W6*x2
	vmlal.s16	q7, d12, D1[1]			@x3 = W6*x2 + W2*x3@
	
	vmull.s16	q3, d12, D1[0]			@W6*x3
	vmlsl.s16	q3, d13, D1[1]			@x2 = W6*x3 - W2*x2@
	.else
@	vmull.s16	q14, d11, D0[0]			@W7*x5
	vmull.s16	q14, d10, D0[1]			@x4 = W7*x5 + W1*x4@
	
	vmull.s16	q15, d10, D0[0]			@W7*x4
@	vmlsl.s16	q15, d11, D0[1]			@x5 = W7*x4 - W1*x5@
	

	vmull.s16	q4, d14, D0[2]			@W3*x7
@	vmlal.s16	q4, d15, D0[3]			@x6 = W3*x7 + W5*x6
	
@	vmull.s16	q5, d15, D0[2]			@W3*x6
	vmull.s16	q5, d14, D0[3]			@x7 = W3*x6 - W5*x7@

@	vmull.s16	q7, d13, D1[0]			@W6*x2
	vmull.s16	q7, d12, D1[1]			@x3 = W6*x2 + W2*x3@
	
	vmull.s16	q3, d12, D1[0]			@W6*x3
@	vmlsl.s16	q3, d13, D1[1]			@x2 = W6*x3 - W2*x2@

	.endif
	
	@((x4, x5, x6, x7, x3, x2) + 4)>>3
	vmov.s32	q6, #4
	vadd.s32	q14, q14, q6
	vadd.s32	q15, q15, q6
	vadd.s32	q4, q4, q6
	.if \is_8x8 > 0
	vadd.s32	q5, q5, q6
	.else
	vsub.s32	q5, q6, q5
	.endif

	vadd.s32	q7, q7, q6
	vadd.s32	q3, q3, q6
	vshr.s32	q14, q14, #3
	vshr.s32	q15, q15, #3	
	vshr.s32	q4, q4, #3
	vshr.s32	q5, q5, #3
	vshr.s32	q7, q7, #3
	vshr.s32	q3, q3, #3	
			
@	x8 = x0 + x1@
@	x0 -= x1@
	.if \is_8x8 > 0
	vadd.s32	q6, q13, q12		@x8 = x0 + x1@
	vsub.s32	q13, q13, q12		@x0 -= x1@
	.endif

		
@	x1 = x4 + x6@
@	x4 -= x6@
@	x6 = x5 + x7@
@	x5 -= x7@
@	x7 = x8 + x3@
@	x8 -= x3@
@	x3 = x0 + x2@
@	x0 -= x2@

	vadd.s32	q12, q14, q4		@x1 = x4 + x6@
	vsub.s32	q14, q14, q4		@x4 -= x6@
	
	vadd.s32	q4, q15, q5			@x6 = x5 + x7@
	vsub.s32	q15, q15, q5		@x5 -= x7@

	.if \is_8x8 > 0
	vadd.s32	q5, q6, q7			@x7 = x8 + x3@
	vsub.s32	q6, q6, q7			@x8 -= x3@
	.else
	vadd.s32	q5, q13, q7			@x7 = x8 + x3@
	vsub.s32	q6, q13, q7			@x8 -= x3@
	.endif
	
	
	vadd.s32	q7, q13, q3			@x3 = x0 + x2@
	vsub.s32	q13, q13, q3		@x0 -= x2@

@	x2 = (181 * (x4 + x5) + 128) >> 8@
@	x4 = (181 * (x4 - x5) + 128) >> 8@
		
	vadd.s32	q3, q14, q15
	vmul.s32	q3, q3, D1[1]
	vsub.s32	q14, q14, q15	
	vmul.s32	q14, q14, D1[1]
		
	vmov.s32	q15, #128		
	vadd.s32	q3, q3, q15			
	vshr.s32	q3, q3, #8		@x2 = (181 * (x4 + x5) + 128) >> 8@
	vadd.s32	q14, q14, q15			
	vshr.s32	q14, q14, #8		@x4 = (181 * (x4 - x5) + 128) >> 8@
		
@	Blk[0] = (idct_t)((x7 + x1) >> 8)@
@	Blk[8] = (idct_t)((x3 + x2) >> 8)@
@	Blk[16] = (idct_t)((x0 + x4) >> 8)@
@	Blk[24] = (idct_t)((x8 + x6) >> 8)@
@	Blk[32] = (idct_t)((x8 - x6) >> 8)@
@	Blk[40] = (idct_t)((x0 - x4) >> 8)@
@	Blk[48] = (idct_t)((x3 - x2) >> 8)@
@	Blk[56] = (idct_t)((x7 - x1) >> 8)@

	vadd.s32	q15, q5, q12		@Blk[ 0] = (idct_t)((x7 + x1) >> 8)@
	vsub.s32	q5, q5, q12			@Blk[56] = (idct_t)((x7 - x1) >> 8)@
	vadd.s32	q12, q7, q3			@Blk[8] = (idct_t)((x3 + x2) >> 8)@
	vsub.s32	q7, q7, q3			@Blk[48] = (idct_t)((x3 - x2) >> 8)@
	vadd.s32	q3, q13, q14		@Blk[16] = (idct_t)((x0 + x4) >> 8)@
	vsub.s32	q13, q13, q14		@Blk[40] = (idct_t)((x0 - x4) >> 8)@
	vadd.s32	q14, q6, q4			@Blk[24] = (idct_t)((x8 + x6) >> 8)@
	vsub.s32	q6, q6, q4			@Blk[32] = (idct_t)((x8 - x6) >> 8)@
	
	vshrn.s32	d17, q15, #14
	vshrn.s32	d19, q12, #14	
	vshrn.s32	d21, q3, #14
	vshrn.s32	d23, q14, #14
	
	vshrn.s32	d9, q6, #14
	vshrn.s32	d13, q7, #14
	vshrn.s32	d15, q5, #14
	vshrn.s32	d11, q13, #14		
	
	vswp.s32	d2, d8
	vswp.s32	d3, d10
	vswp.s32	d4, d12	
	vswp.s32	d5, d14	
	.endm
 	
 .macro Col8_4x8

	LOAD_IDCT4x8_COEF

	Col8_first4line
		
	vshrn.s32	d16, q15, #8
	vshrn.s32	d18, q12, #8	
	vshrn.s32	d20, q3, #8
	vshrn.s32	d22, q14, #8
	
	vshrn.s32	d8, q6, #8
	vshrn.s32	d12, q7, #8
	vshrn.s32	d14, q5, #8
	vshrn.s32	d10, q13, #8		

	vmov.s64		d9, #0
	vmov.s64		d11, #0
	vmov.s64		d13, #0
	vmov.s64		d15, #0
	vmov.s64		d17, #0
	vmov.s64		d19, #0
	vmov.s64		d21, #0
	vmov.s64		d23, #0
	
@	Transpose	
			
	VTRN.16 q8, q9
	VTRN.16 q10, q11
	VTRN.16 q4, q5
	VTRN.16 q6, q7
	VTRN.32 q8, q10
	VTRN.32 q9, q11
	VTRN.32 q4, q6
	VTRN.32 q5, q7
	
									@q8  x0 x1 d16-d17
	vswp.s32	d19, d23			@q11 x4 x5 d18-d19
									@q10 x3 x2 d20-d21	
									@q9  x7 x6 d22-d23
	vswp.s64	q9, q11									
		
									@q4 x0 x1 d8-d9
	vswp.s32	d11, d15			@q5 x4 x5 d10-d11
									@q6 x3 x2 d12-d13	
									@q7 x7 x6 d14-d15								
@q8  x0 x1 d16-d17
@q11 x4 x5 d22-d23
@q10 x3 x2 d20-d21	
@q9  x7 x6 d18-d19	
	
@q4 x0 x1 d8-d9
@q5 x4 x5 d10-d11
@q6 x3 x2 d12-d13
@q7 x7 x6 d14-d15
										
	Row_8x8line 0
		
@	Transpose	

	VTRN.16 q8, q9
	VTRN.16 q10, q11
	VTRN.16 q4, q5
	VTRN.16 q6, q7
	VTRN.32 q8, q10
	VTRN.32 q9, q11
	VTRN.32 q4, q6
	VTRN.32 q5, q7
	VSWP d17, d8
	VSWP d19, d10
	VSWP d21, d12
	VSWP d23, d14	
	
	cmp			r3, #0
	beq			IDCT_CPY_NOSRC_4x8
	LOAD_IDCT_SRC_ADD				
IDCT_CPY_NOSRC_4x8:	
	STR_IDCT					
	.endm

  .macro	Col8_8x8
	LOAD_IDCT8x8_COEF
	
	vswp.s32	d9, d16				@q8  x0 x1 d16-17
	vswp.s32	d15, d18			@q11 x4 x5 d22-23
	vswp.s32	d13, d20			@q10 x3 x2 d20-21	
	vswp.s32	d11, d22			@q9  x7 x6 d18-19		
	
	
	Col8_first4line
		
	vshrn.s32	d2, q15, #8
	vshrn.s32	d3, q12, #8	
	vshrn.s32	d4, q3, #8
	vshrn.s32	d5, q14, #8
	
	vshrn.s32	d8, q6, #8
	vshrn.s32	d12, q7, #8
	vshrn.s32	d14, q5, #8
	vshrn.s32	d10, q13, #8		

	Col8_last4line
	
	
	vshrn.s32	d9, q10, #8
	vshrn.s32	d11, q13, #8
	vshrn.s32	d13, q9, #8
	vshrn.s32	d15, q11, #8
			
	vshrn.s32	d17, q15, #8
	vshrn.s32	d19, q12, #8	
	vshrn.s32	d21, q3, #8
	vshrn.s32	d23, q14, #8	
	
	vswp.s32	d2, d16
	vswp.s32	d3, d18
	vswp.s32	d4, d20	
	vswp.s32	d5, d22
	
@	Transpose	
			
	VTRN.16 q8, q9
	VTRN.16 q10, q11
	VTRN.16 q4, q5
	VTRN.16 q6, q7
	VTRN.32 q8, q10
	VTRN.32 q9, q11
	VTRN.32 q4, q6
	VTRN.32 q5, q7
	
									@q8  x0 x1 d16-d17
	vswp.s32	d19, d23			@q11 x4 x5 d18-d19
									@q10 x3 x2 d20-d21	
									@q9  x7 x6 d22-d23
	vswp.s64	q9, q11									
		
									@q4 x0 x1 d8-d9
	vswp.s32	d11, d15			@q5 x4 x5 d10-d11
									@q6 x3 x2 d12-d13	
									@q7 x7 x6 d14-d15								
@q8  x0 x1 d16-d17
@q11 x4 x5 d22-d23
@q10 x3 x2 d20-d21	
@q9  x7 x6 d18-d19	
	
@q4 x0 x1 d8-d9
@q5 x4 x5 d10-d11
@q6 x3 x2 d12-d13
@q7 x7 x6 d14-d15
										
	Row_8x8line	1
		
@	Transpose
	
	VTRN.16 q8, q9
	VTRN.16 q10, q11
	VTRN.16 q4, q5
	VTRN.16 q6, q7
	VTRN.32 q8, q10
	VTRN.32 q9, q11
	VTRN.32 q4, q6
	VTRN.32 q5, q7
	VSWP d17, d8
	VSWP d19, d10
	VSWP d21, d12
	VSWP d23, d14	
	
	cmp			r3, #0
	beq			IDCT_CPY_NOSRC_8x8
	LOAD_IDCT_SRC_ADD				
IDCT_CPY_NOSRC_8x8:	
	STR_IDCT					
	.endm
	
	.align 4
W_table1:	
		.word 0x0B190235
		.word 0x06490968		
		.word 0x0A740454	
		.word 0x000000b5		@181= D1[1]	
	.align 4
MPEG4DEC_VO_Armv7IdctA: @PROC
@ r0 = Block, r1 = dst, r2 = dst_stride, r3 = Src, r12 = [sp] = src_stride

	adr			r12, W_table1
	vld1.s32 	{q0}, [r12]	  
	vmov.s32	q1, #128	

	Col8_4x8  

 	mov		pc,lr

	@ENDP
	.align 4
W_table:	
		.word 0x0B190235
		.word 0x06490968		
		.word 0x0A740454	
		.word 0x000000b5		@181= D1[1]	
	.align 4
MPEG4DEC_VO_Armv7IdctB: @PROC
@ r0 = Block, r1 = dst, r2 = dst_stride, r3 = Src, r12 = [sp] = src_stride

	adr			r12, W_table
	vld1.s32 	{q0}, [r12]	  
	vmov.s32	q1, #128
	
	Col8_8x8  
	
 	mov		pc,lr
	@ENDP	


MPEG4DEC_VO_Armv7IdctC: @PROC	
        STMFD    sp!,{r4-r11,lr}
		@ r0 = v, r1 = Dst, r2 = DstPitch, r3 = Src
	ldrsh	r12, [r0]
	mov	r5, #0
	cmp	r3, #0	
	add	r12, r12, #4	
	strh	r5, [r0]	
	mov	r0, r12, asr #3
@	int32_t OutD = (Block[0]+4)>>3;		
	beq	IDCT_CPY_NOSRC_ARMv6	
		
	@const uint8_t* SrcEnd = Src + 8*SrcPitch (int SrcPitch = 8;)
		ldr		r14,[sp,#36]	@SrcPitch
        cmp     r0, #0    @if (v==0)                       
	bne     outCopyBlock8x8_asm_ARMv6     

	CopyBlock8x8_asm_ARMv6:
	ldrd     r4, [r3], r14        
	ldrd     r6, [r3], r14 
	ldrd     r8, [r3], r14        
	ldrd     r10, [r3], r14	
	strd     r4, [r1], r2 
	strd     r6, [r1], r2 
	strd     r8, [r1], r2 
	strd     r10, [r1], r2 

	ldrd     r4, [r3], r14        
	ldrd     r6, [r3], r14 
	ldrd     r8, [r3], r14        
	ldrd     r10, [r3], r14	
	strd     r4, [r1], r2 
	strd     r6, [r1], r2 
	strd     r8, [r1], r2 
	strd     r10, [r1], r2
        LDMFD    sp!,{r4-r11,pc}

outCopyBlock8x8_asm_ARMv6:

	blt     little_begin_ARMv6 
	
big_begin_ARMv6_ARMv6:	
	
	orr     r12, r0, r0, lsl #8            
	orr     r0, r12, r12, lsl #16    
	
	ldrd     r4, [r3], r14        
	ldrd     r6, [r3], r14   			             
	uqadd8	 r8, r4, r0
	uqadd8	 r9, r5, r0
	uqadd8	 r10, r6, r0
	uqadd8	 r11, r7, r0
	strd     r8, [r1], r2 
	strd     r10, [r1], r2 

	ldrd     r4, [r3], r14        
	ldrd     r6, [r3], r14   			             
	uqadd8	 r8, r4, r0
	uqadd8	 r9, r5, r0
	uqadd8	 r10, r6, r0
	uqadd8	 r11, r7, r0
	strd     r8, [r1], r2 
	strd     r10, [r1], r2 

	ldrd     r4, [r3], r14        
	ldrd     r6, [r3], r14   			             
	uqadd8	 r8, r4, r0
	uqadd8	 r9, r5, r0
	uqadd8	 r10, r6, r0
	uqadd8	 r11, r7, r0
	strd     r8, [r1], r2 
	strd     r10, [r1], r2 
	
	ldrd     r4, [r3], r14        
	ldrd     r6, [r3], r14   			             
	uqadd8	 r8, r4, r0
	uqadd8	 r9, r5, r0
	uqadd8	 r10, r6, r0
	uqadd8	 r11, r7, r0
	strd     r8, [r1], r2 
	strd     r10, [r1], r2 													                                             
        LDMFD    sp!,{r4-r11,pc}      
	
little_begin_ARMv6:

	rsb     r12, r0, #0                       
	orr     r12, r12, r12, lsl #8            
	orr     r0, r12, r12, lsl #16            


	ldrd     r4, [r3], r14        
	ldrd     r6, [r3], r14   			             
	uqsub8	 r8, r4, r0
	uqsub8	 r9, r5, r0
	uqsub8	 r10, r6, r0
	uqsub8	 r11, r7, r0
	strd     r8, [r1], r2 
	strd     r10, [r1], r2 
	
	ldrd     r4, [r3], r14        
	ldrd     r6, [r3], r14   			             
	uqsub8	 r8, r4, r0
	uqsub8	 r9, r5, r0
	uqsub8	 r10, r6, r0
	uqsub8	 r11, r7, r0
	strd     r8, [r1], r2 
	strd     r10, [r1], r2 
	
	ldrd     r4, [r3], r14        
	ldrd     r6, [r3], r14   			             
	uqsub8	 r8, r4, r0
	uqsub8	 r9, r5, r0
	uqsub8	 r10, r6, r0
	uqsub8	 r11, r7, r0
	strd     r8, [r1], r2 
	strd     r10, [r1], r2 

	ldrd     r4, [r3], r14        
	ldrd     r6, [r3], r14   			             
	uqsub8	 r8, r4, r0
	uqsub8	 r9, r5, r0
	uqsub8	 r10, r6, r0
	uqsub8	 r11, r7, r0
	strd     r8, [r1], r2 
	strd     r10, [r1], r2 
        LDMFD    sp!,{r4-r11,pc}  
	
IDCT_CPY_NOSRC_ARMv6:	
@	OutD = SAT_new(OutD)@
	usat	r12, #8, r0
	orr     r10, r12, r12, lsl #8
	orr     r10, r10, r10, lsl #16
	mov	r11, r10	
		
        strd     r10, [r1], r2 
        strd     r10, [r1], r2 
        strd     r10, [r1], r2 
        strd     r10, [r1], r2
        strd     r10, [r1], r2 
        strd     r10, [r1], r2 
        strd     r10, [r1], r2 
        strd     r10, [r1]  
              		
        LDMFD    sp!,{r4-r11,pc}		        
									

	.END

