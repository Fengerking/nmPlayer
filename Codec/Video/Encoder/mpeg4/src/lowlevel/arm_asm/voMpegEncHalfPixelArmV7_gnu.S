@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2005		            *
@								 	                                    *
@***********************************************************************/


	.section .text

	.global Inplace16InterpolateHP_ARMV7
	.global SubBlock_ARMV7 
	.global SubBlockVer_ARMV7 
	.global SubBlockVerRound_ARMV7 
	.global SubBlockHor_ARMV7 
	.global SubBlockHorRound_ARMV7 
	.global SubBlockHorVer_ARMV7
	.global SubBlockHorVerRound_ARMV7

@*******************************************************************************	

@r0 : dst_h
@r1 : dst_v
@r2 : dst_hv
@r3 : src
@r4 : dst_h +16
@r5 : dst_v +16
@r6 : dst_hv +16
@r12 :src +16
@r7 : dst stride
@lr : src stride
@r8 : i
@q0,q1 : a0
@q2,q3 : b0
@q4		:a1
@q5		@b1

Inplace16InterpolateHP_ARMV7:

	stmdb		sp!, {r4-r8,lr}

	add			r4, r0, #16
	ldr			lr, [sp, #24+4]		@src stride
	ldr			r12, [sp, #24+8]	@rounding
	sub			r3, r3, #1	
	add			r5, r1, #16
	mov			r7, #24				@dst stride
	mov			r8, #16
	sub			r3, r3, lr		
	cmp			r12, #0
	add			r6, r2, #16
	add			r12, r3, #16
	beq			inplace16_halfpel_rnd0

	vld1.8      {q0},[r3],lr		@a0
	vld1.8      d2,[r12],lr
	vld1.8      {q2},[r3],lr		@b0
	vld1.8      d6,[r12],lr
	vmov.i16	q15, #1
	
halfpel_rnd1_loop:
		
	vext.8		q4, q0, q1, #1		@a1
	vext.8		q5, q2, q3, #1		@b1
	@V (a1,b1)
	vhadd.u8	q14, q4, q5		
	vhadd.u8	d26, d2, d6			
	vst1.32		{q14},  [r1], r7
	vst1.8		d26[1], [r5], r7
	@H (b0,b1)
	vhadd.u8	q14, q2, q5
	vshr.u32	d26, d6, #8
	vhadd.u8	d26, d26, d6
	vst1.32		{q14},  [r0], r7
	vst1.8		d26[0], [r4], r7
	@HV (a0,a1,b0,b1)
	vaddl.u8	q6, d0, d8			@a0+a1
	vaddl.u8	q7, d1, d9	
	vaddl.u8	q13, d4, d10		@b0+b1
	vaddl.u8	q14, d5, d11	
	vshr.u32	d8, d2, #8
	vshr.u32	d9, d6, #8
	vaddl.u8	q8, d8, d2	
	vaddl.u8	q9, d9, d6	
	vadd.u16	q11, q6, q13
	vadd.u16	q12, q7, q14
	vadd.u16	d18, d16, d18
	vadd.u16	q11, q11, q15		@+1
	vadd.u16	q12, q12, q15
	vadd.u16	d18, d18, d30	
	vshr.u16	q11, q11, #2
	vshr.u16	q12, q12, #2
	vshr.u16	d18, d18, #2
	vmovn.i16	d20, q11
	vmovn.i16	d21, q12	
	vmovn.i16	d22, q9
	vst1.32		{q10},  [r2], r7
	vst1.8		d22[0], [r6], r7

	vshr.u64	q0, q2, #0
	vshr.u64	d2, d6, #0
	vld1.8      {q2},[r3],lr		@b0
	vld1.8      d6,[r12],lr

	subs		r8, r8, #1
	bge			halfpel_rnd1_loop

    ldmia		sp!, {r4-r8,pc}		

inplace16_halfpel_rnd0:	

	vld1.8      {q0},[r3],lr		@a0
	vld1.8      d2,[r12],lr
	vld1.8      {q2},[r3],lr		@b0
	vld1.8      d6,[r12],lr
	vmov.i16	q15, #2
	
halfpel_rnd0_loop:
	
	vext.8		q4, q0, q1, #1		@a1
	vext.8		q5, q2, q3, #1		@b1
	@V (a1,b1)
	vrhadd.u8	q14, q4, q5		
	vrhadd.u8	d26, d2, d6			
	vst1.32		{q14},  [r1], r7
	vst1.8		d26[1], [r5], r7
	@H (b0,b1)
	vrhadd.u8	q14, q2, q5
	vshr.u32	d26, d6, #8
	vrhadd.u8	d26, d26, d6
	vst1.32		{q14},  [r0], r7
	vst1.8		d26[0], [r4], r7
	@HV (a0,a1,b0,b1)
	vaddl.u8	q6, d0, d8			@a0+a1
	vaddl.u8	q7, d1, d9	
	vaddl.u8	q13, d4, d10		@b0+b1
	vaddl.u8	q14, d5, d11	
	vshr.u32	d8, d2, #8
	vshr.u32	d9, d6, #8
	vaddl.u8	q8, d8, d2	
	vaddl.u8	q9, d9, d6	
	vadd.u16	q11, q6, q13
	vadd.u16	q12, q7, q14
	vadd.u16	d18, d16, d18
	vadd.u16	q11, q11, q15		@+1
	vadd.u16	q12, q12, q15
	vadd.u16	d18, d18, d30	
	vshr.u16	q11, q11, #2
	vshr.u16	q12, q12, #2
	vshr.u16	d18, d18, #2
	vmovn.i16	d20, q11
	vmovn.i16	d21, q12	
	vmovn.i16	d22, q9
	vst1.32		{q10},  [r2], r7
	vst1.8		d22[0], [r6], r7

	vshr.u64	q0, q2, #0
	vshr.u64	d2, d6, #0
	vld1.8      {q2},[r3],lr		@b0
	vld1.8      d6,[r12],lr

	subs		r8, r8, #1
	bge			halfpel_rnd0_loop

    ldmia		sp!, {r4-r8,pc}		
	
@***************************************************************
@r0:Src
@r1:dct_data
@r2:Dst
@r3:SrcPitch

SubBlock_ARMV7:	
	mov			r12, r2
	vld1.8      {d0},[r0],r3  
	vld1.8      {d1},[r0],r3   
	vld1.8      {d2},[r0],r3  
	vld1.8      {d3},[r0],r3  
	vld1.8      {d4},[r0],r3  
	vld1.8      {d5},[r0],r3  
	vld1.8      {d6},[r0],r3  
	vld1.8      {d7},[r0]   
	vld1.64     {d8}, [r2],r3  
	vld1.64     {d9}, [r2],r3  
	vld1.64     {d10},[r2],r3  
	vld1.64     {d11},[r2],r3  
	vld1.64     {d12},[r2],r3  
	vld1.64     {d13},[r2],r3  
	vld1.64     {d14},[r2],r3  
	vld1.64     {d15},[r2]   
	vsubl.u8	q8,  d8,  d0
	vsubl.u8	q9,  d9,  d1
	vsubl.u8	q10, d10, d2
	vsubl.u8	q11, d11, d3
	vsubl.u8	q12, d12, d4
	vsubl.u8	q13, d13, d5
	vsubl.u8	q14, d14, d6
	vsubl.u8	q15, d15, d7
	vst1.64     {d0},[r12],r3  
	vst1.64     {d1},[r12],r3  
	vst1.64     {d2},[r12],r3  
	vst1.64     {d3},[r12],r3  
	vst1.64     {d4},[r12],r3  
	vst1.64     {d5},[r12],r3  
	vst1.64     {d6},[r12],r3 
	vst1.64     {d7},[r12]   
	vst1.64		{q8}, [r1]!		
	vst1.64		{q9}, [r1]!		
	vst1.64		{q10},[r1]!		
	vst1.64		{q11},[r1]!		
	vst1.64		{q12},[r1]!		
	vst1.64		{q13},[r1]!		
	vst1.64		{q14},[r1]!		
	vst1.64		{q15},[r1]	
	mov			pc, lr
	
	.MACRO	M_SubBlockVerArmv7	RND
	mov			r12, r2
	vld1.8      {d0},[r0],r3  
	vld1.8      {d1},[r0],r3  
	vld1.8      {d2},[r0],r3  
	vld1.8      {d3},[r0],r3  
	vld1.8      {d4},[r0],r3  
	vld1.8      {d5},[r0],r3  
	vld1.8      {d6},[r0],r3  
	vld1.8      {d7},[r0],r3   
	vld1.8      {d16},[r0]   
	vld1.64     {d8}, [r2],r3  
	vld1.64     {d9}, [r2],r3  
	vld1.64     {d10},[r2],r3  
	vld1.64     {d11},[r2],r3  
	vld1.64     {d12},[r2],r3  
	vld1.64     {d13},[r2],r3  
	vld1.64     {d14},[r2],r3  
	vld1.64     {d15},[r2]   
	.IF \RND == 0
	vrhadd.u8	d0, d0, d1
	vrhadd.u8	d1, d1, d2
	vrhadd.u8	d2, d2, d3
	vrhadd.u8	d3, d3, d4
	vrhadd.u8	d4, d4, d5
	vrhadd.u8	d5, d5, d6
	vrhadd.u8	d6, d6, d7
	vrhadd.u8	d7, d7, d16	
	.ELSE
	vhadd.u8	d0, d0, d1
	vhadd.u8	d1, d1, d2
	vhadd.u8	d2, d2, d3
	vhadd.u8	d3, d3, d4
	vhadd.u8	d4, d4, d5
	vhadd.u8	d5, d5, d6
	vhadd.u8	d6, d6, d7
	vhadd.u8	d7, d7, d16	
	.ENDIF
	vsubl.u8	q8,  d8,  d0
	vsubl.u8	q9,  d9,  d1
	vsubl.u8	q10, d10, d2
	vsubl.u8	q11, d11, d3
	vsubl.u8	q12, d12, d4
	vsubl.u8	q13, d13, d5
	vsubl.u8	q14, d14, d6
	vsubl.u8	q15, d15, d7
	vst1.64     {d0},[r12],r3  
	vst1.64     {d1},[r12],r3  
	vst1.64     {d2},[r12],r3  
	vst1.64     {d3},[r12],r3  
	vst1.64     {d4},[r12],r3  
	vst1.64     {d5},[r12],r3  
	vst1.64     {d6},[r12],r3 
	vst1.64     {d7},[r12]   
	vst1.64		{q8}, [r1]!		
	vst1.64		{q9}, [r1]!		
	vst1.64		{q10},[r1]!		
	vst1.64		{q11},[r1]!		
	vst1.64		{q12},[r1]!		
	vst1.64		{q13},[r1]!		
	vst1.64		{q14},[r1]!		
	vst1.64		{q15},[r1]	
	mov			pc, lr
	.ENDM		@M_SubBlockVerArmv7
		
	.MACRO	M_SubBlockHorArmv7	RND
	add			r12, r0, #8
	vld1.8      {d0},[r0],r3  
	vld1.8      {d16[0]},[r12],r3  
	vld1.8      {d1},[r0],r3  
	vld1.8      {d17[0]},[r12],r3  
	vld1.8      {d2},[r0],r3  
	vld1.8      {d18[0]},[r12],r3  
	vld1.8      {d3},[r0],r3  
	vld1.8      {d19[0]},[r12],r3  
	vld1.8      {d4},[r0],r3  
	vld1.8      {d20[0]},[r12],r3  
	vld1.8      {d5},[r0],r3  
	vld1.8      {d21[0]},[r12],r3  
	vld1.8      {d6},[r0],r3  
	vld1.8      {d22[0]},[r12],r3  
	vld1.8      {d7},[r0]   
	vld1.8      {d23[0]},[r12]   
	vld1.64     {d8}, [r2],r3  
	vld1.64     {d9}, [r2],r3  
	vld1.64     {d10},[r2],r3  
	vld1.64     {d11},[r2],r3  
	vld1.64     {d12},[r2],r3  
	vld1.64     {d13},[r2],r3  
	vld1.64     {d14},[r2],r3  
	vld1.64     {d15},[r2],r3   
	vext.8		d16, d0, d16, #1
	vext.8		d17, d1, d17, #1
	vext.8		d18, d2, d18, #1
	vext.8		d19, d3, d19, #1
	vext.8		d20, d4, d20, #1
	vext.8		d21, d5, d21, #1
	vext.8		d22, d6, d22, #1
	vext.8		d23, d7, d23, #1
	.IF \RND == 0
	vrhadd.u8	d0, d0, d16
	vrhadd.u8	d1, d1, d17
	vrhadd.u8	d2, d2, d18
	vrhadd.u8	d3, d3, d19
	vrhadd.u8	d4, d4, d20
	vrhadd.u8	d5, d5, d21
	vrhadd.u8	d6, d6, d22
	vrhadd.u8	d7, d7, d23	
	.ELSE
	vhadd.u8	d0, d0, d16
	vhadd.u8	d1, d1, d17
	vhadd.u8	d2, d2, d18
	vhadd.u8	d3, d3, d19
	vhadd.u8	d4, d4, d20
	vhadd.u8	d5, d5, d21
	vhadd.u8	d6, d6, d22
	vhadd.u8	d7, d7, d23	
	.ENDIF
	sub			r2, r2, r3, lsl #3
	vsubl.u8	q8,  d8,  d0
	vsubl.u8	q9,  d9,  d1
	vsubl.u8	q10, d10, d2
	vsubl.u8	q11, d11, d3
	vsubl.u8	q12, d12, d4
	vsubl.u8	q13, d13, d5
	vsubl.u8	q14, d14, d6
	vsubl.u8	q15, d15, d7
	vst1.64     {d0},[r2],r3  
	vst1.64     {d1},[r2],r3  
	vst1.64     {d2},[r2],r3  
	vst1.64     {d3},[r2],r3  
	vst1.64     {d4},[r2],r3  
	vst1.64     {d5},[r2],r3  
	vst1.64     {d6},[r2],r3 
	vst1.64     {d7},[r2]   
	vst1.64		{q8}, [r1]!		
	vst1.64		{q9}, [r1]!		
	vst1.64		{q10},[r1]!		
	vst1.64		{q11},[r1]!		
	vst1.64		{q12},[r1]!		
	vst1.64		{q13},[r1]!		
	vst1.64		{q14},[r1]!		
	vst1.64		{q15},[r1]	
	mov			pc, lr
	.ENDM		@M_SubBlockHorArmv7

	.MACRO	M_SubBlockHorVerArmv7	RND
	add			r12, r0, #8
	vld1.8      {d0},[r0],r3  
	vld1.8      {d16[0]},[r12],r3  
	vld1.8      {d1},[r0],r3  
	vld1.8      {d17[0]},[r12],r3  
	vld1.8      {d2},[r0],r3  
	vld1.8      {d18[0]},[r12],r3  
	vld1.8      {d3},[r0],r3  
	vld1.8      {d19[0]},[r12],r3  
	vld1.8      {d4},[r0],r3  
	vld1.8      {d20[0]},[r12],r3  
	vld1.8      {d5},[r0],r3  
	vld1.8      {d21[0]},[r12],r3  
	vld1.8      {d6},[r0],r3  
	vld1.8      {d22[0]},[r12],r3  
	vld1.8      {d7},[r0],r3    
	vld1.8      {d23[0]},[r12],r3   
	vld1.8      {d8},[r0] 
	vld1.8      {d24[0]},[r12]  	
	vext.8		d16, d0, d16, #1
	vext.8		d17, d1, d17, #1
	vext.8		d18, d2, d18, #1
	vext.8		d19, d3, d19, #1
	vext.8		d20, d4, d20, #1
	vext.8		d21, d5, d21, #1
	vext.8		d22, d6, d22, #1
	vext.8		d23, d7, d23, #1
	vext.8		d24, d8, d24, #1	
	vaddl.u8	q13,d6, d22
	vaddl.u8	q14,d7, d23
	vaddl.u8	q15,d8, d24
	vaddl.u8	q4, d0, d16
	vaddl.u8	q5, d1, d17
	vaddl.u8	q6, d2, d18
	vaddl.u8	q7, d3, d19
	vaddl.u8	q8, d4, d20
	vaddl.u8	q9, d5, d21	
	.IF \RND == 0
	vmov.i16	q10, #2
	.ELSE
	vmov.i16	q10, #1
	.ENDIF
	vadd.u16	q4,  q4,  q5
	vadd.u16	q5,  q5,  q6
	vadd.u16	q6,  q6,  q7
	vadd.u16	q7,  q7,  q8
	vadd.u16	q8,  q8,  q9
	vadd.u16	q9,  q9,  q13
	vadd.u16	q13, q13, q14
	vadd.u16	q14, q14, q15
	vadd.u16	q4,  q4,  q10
	vadd.u16	q5,  q5,  q10
	vadd.u16	q6,  q6,  q10
	vadd.u16	q7,  q7,  q10
	vadd.u16	q8,  q8,  q10
	vadd.u16	q9,  q9,  q10
	vadd.u16	q13, q13, q10
	vadd.u16	q14, q14, q10
	vshr.u16	q4,  q4,  #2
	vshr.u16	q5,  q5,  #2
	vmovn.i16	d0, q4
	vshr.u16	q6,  q6,  #2
	vshr.u16	q7,  q7,  #2
	vld1.64     {d8}, [r2],r3  
	vmovn.i16	d1, q5 
	vld1.64     {d9}, [r2],r3  
	vmovn.i16	d2, q6 
	vshr.u16	q8,  q8,  #2
	vshr.u16	q9,  q9,  #2
	vshr.u16	q13, q13, #2
	vshr.u16	q14, q14, #2
	vld1.64     {d10},[r2],r3  
	vmovn.i16	d3, q7 
	vld1.64     {d11},[r2],r3  
	vmovn.i16	d4, q8 
	vld1.64     {d12},[r2],r3  
	vmovn.i16	d5, q9 
	vld1.64     {d13},[r2],r3  
	vmovn.i16	d6, q13
	vld1.64     {d14},[r2],r3  
	vmovn.i16	d7, q14
	vld1.64     {d15},[r2],r3   
	vsubl.u8	q8,  d8,  d0
	vsubl.u8	q9,  d9,  d1
	sub			r2, r2, r3, lsl #3
	vsubl.u8	q10, d10, d2
	vsubl.u8	q11, d11, d3
	vsubl.u8	q12, d12, d4
	vsubl.u8	q13, d13, d5
	vsubl.u8	q14, d14, d6
	vsubl.u8	q15, d15, d7
	vst1.64     {d0},[r2],r3  
	vst1.64     {d1},[r2],r3  
	vst1.64     {d2},[r2],r3  
	vst1.64     {d3},[r2],r3  
	vst1.64     {d4},[r2],r3  
	vst1.64     {d5},[r2],r3  
	vst1.64     {d6},[r2],r3 
	vst1.64     {d7},[r2]   
	vst1.64		{q8}, [r1]!		
	vst1.64		{q9}, [r1]!		
	vst1.64		{q10},[r1]!		
	vst1.64		{q11},[r1]!		
	vst1.64		{q12},[r1]!		
	vst1.64		{q13},[r1]!		
	vst1.64		{q14},[r1]!		
	vst1.64		{q15},[r1]	
	mov			pc, lr
	.ENDM		@M_SubBlockHorVerArmv7

	
SubBlockVer_ARMV7:
	M_SubBlockVerArmv7	0
	
SubBlockVerRound_ARMV7:
	M_SubBlockVerArmv7	1

SubBlockHor_ARMV7:	
	M_SubBlockHorArmv7 0
	
SubBlockHorRound_ARMV7:	
	M_SubBlockHorArmv7 1
	
SubBlockHorVer_ARMV7:
	M_SubBlockHorVerArmv7 0
	
SubBlockHorVerRound_ARMV7:
	M_SubBlockHorVerArmv7 1


	.END
	