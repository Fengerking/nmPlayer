@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2009					*
@*																			*
@*****************************************************************************


	
@R0 = pQuantBuf
@R1 = pDst
@R2 = uDstPitch
@R3 = pPredSrc
@R14, never cover r14
@R12 = Tmp

	@AREA	|.text|, CODE
	.section .text
	
	.global	ARMV7_Transform4x4_Add
	.global	ARMV7_ITransform4x4_DCOnly_Add
	.global	ARMV7_Intra16x16ITransform4x4
	
  .equ OFFSET_SaveRegisters , 0

  CONST_0x200_Q	.req q13
  CONST_13_Q	.req	q14
  CONST_17_Q	.req	q15
  CONST_169_Q	.req	q15

	
	.macro M_pushLR
    str		lr,  [sp, #-4]!
	.endm

	.macro M_popLR
    ldr		pc,  [sp], #4
	.endm
	
	.macro M_LRtoPC
	  mov			pc, lr
	.endm
	

	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.MACRO M_IDCT4x4_Core
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ input:  q0-q3 = a0-a3
@ output: q0-q3 = x0-x3
    
	@	b0 = (a0 + a2) * 13@
	@	b1 = (a0 - a2) * 13@
	@	b2 = a1 * 7  - a3 * 17@
	@	b3 = a1 * 17 + a3 * 7@
	@	pin[0] = b0 + b3@
	@	pin[12] = b0 - b3@
	@	pin[4] = b1 + b2@
	@	pin[8] = b1 - b2@
	
	vadd.s32	q4, q0, q2
	vsub.s32	q5, q0, q2
	vmul.s32	q4, q4, CONST_13_Q
	vshl.i32	q6, q1, #3
	vshl.i32	q9, q3, #3
	vmul.s32	q5, q5, CONST_13_Q
	vsub.s32	q6, q6, q1			@a1 * 7
	vsub.s32	q9, q9, q3			@a3 * 7
	vmls.s32	q6, q3, CONST_17_Q	@a1 * 7- a3 * 17
	vshl.i32	q10, q1, #4			@a1 * 16
	vadd.s32	q7, q9, q1		
	vadd.s32	q7, q7, q10			@a1 * 17 + a3 * 7
	vadd.s32	q0, q4, q7			@b0 + b3
	vsub.s32	q3, q4, q7			@b0 - b3
	vadd.s32	q1, q5, q6			@b1 + b2
	vsub.s32	q2, q5, q6			@b1 - b2

	.ENDM

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	void RV_FASTCALL ARMV7_Transform4x4_Add( I32 *pQuantBuf, U8 *pDest, U32 uDestStride, const U8 *pPredSrc, U32 uPredPitch )
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ARMV7_Transform4x4_Add:	@PROC
	
	ldr		r12, [sp, #OFFSET_SaveRegisters]
	
	@	a0 = pin[0]@
	@	a1 = pin[4]@
	@	a2 = pin[8]@
	@	a3 = pin[12]@
	
	vld1.64		{q0}, [r0]!		
	vld1.64		{q1}, [r0]!
	vld1.64		{q2}, [r0]!
	vld1.64		{q3}, [r0]	
	
	pld			[r3]
	pld			[r3, r12]
	pld			[r3, r12, lsl #1]
	vmov.i32	CONST_0x200_Q, #0x200
	vmov.i32	CONST_13_Q, #13
	vmov.i32	CONST_17_Q, #17
	
	M_IDCT4x4_Core		@horizontal

	@4x4 Transpose
	vtrn.32		q0, q1
	vtrn.32		q2, q3
	vswp		d1, d4
	vswp		d3, d6
	
	M_IDCT4x4_Core		@vertical
	
	vld1.32		d16[0], [r3], r12	
	vld1.32		d16[1], [r3], r12
	vld1.32		d17[0], [r3], r12
	vld1.32		d17[1], [r3]	
	
	@	x0  = (x0 + 0x200) >> 10@
	@	x3  = (x3 + 0x200) >> 10@
	@	x1  = (x1 + 0x200) >> 10@
	@	x2  = (x2 + 0x200) >> 10@
	
	vadd.s32	q0, q0, CONST_0x200_Q
	vadd.s32	q1, q1, CONST_0x200_Q
	vshr.s32	q0, #10
	vadd.s32	q2, q2, CONST_0x200_Q
	vshr.s32	q1, #10
	vadd.s32	q3, q3, CONST_0x200_Q
	vshr.s32	q2, #10
	vshr.s32	q3, #10
	
	@	x0 += pPredSrc[0]@
	@	x1 += pPredSrc[1]@
	@	x2 += pPredSrc[2]@
	@	x3 += pPredSrc[3]@

	vmovn.i32		d0, q0
	vmovn.i32		d1, q1
	vmovn.i32		d2, q2
	vmovn.i32		d3, q3
	vtrn.16			d0, d1		@4x4 Transpose
	vtrn.16			d2, d3
	vmovl.u8		q9 , d16
	vtrn.32			d0, d2
	vtrn.32			d1, d3
	vmovl.u8		q10, d17
	vadd.s16		q0, q0, q9 
	vadd.s16		q1, q1, q10

	@	SAT(x0)
	@	SAT(x1)
	@	SAT(x2)
	@	SAT(x3)
	
	vqshrun.s16		d0, q0, #0	
	vqshrun.s16		d1, q1, #0	

	@	pDest[0]  = (U8)x0@
	@	pDest[1]  = (U8)x1@
	@	pDest[2]  = (U8)x2@
	@	pDest[3]  = (U8)x3@
	
	vst1.32		d0[0], [r1], r2	
	vst1.32		d0[1], [r1], r2
	vst1.32		d1[0], [r1], r2
	vst1.32		d1[1], [r1]	
		
	M_LRtoPC
	@ENDP	@ARMV7_Transform4x4_Add
	
	

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	void RV_FASTCALL ARMV7_ITransform4x4_DCOnly_Add(I32 *pQuantBuf, U8 *pDest, U32 uDestStride, const U8 *pPredSrc, U32 uPredPitch)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ARMV7_ITransform4x4_DCOnly_Add:	@PROC
		
	@v = ((pQuantBuf[0] * 13 * 13) + 0x200) >> 10@
	
	vld1.32			{d0[],d1[]}, [r0]	
	ldr				r12, [sp, #OFFSET_SaveRegisters]
	vmov.i32		CONST_169_Q, #169
	vmov.i32		CONST_0x200_Q, #0x200
	vld1.32			d16[0], [r3], r12	
	vld1.32			d16[1], [r3], r12
	vmla.s32		CONST_0x200_Q, q0, CONST_169_Q	@q13 = CONST_0x200_Q
	vld1.32			d17[0], [r3], r12
	vld1.32			d17[1], [r3]	
	vmovl.u8		q9 , d16
	vmovl.u8		q10, d17
	vshr.s32		q0, q13, #10
	
	vmovn.i32		d0, q0
	vshr.s64		d1, d0, #0
	vadd.s16		q2, q0, q9 
	vadd.s16		q3, q0, q10
	vqshrun.s16		d0, q2, #0	
	vqshrun.s16		d1, q3, #0	

	vst1.32		d0[0], [r1], r2	
	vst1.32		d0[1], [r1], r2
	vst1.32		d1[0], [r1], r2
	vst1.32		d1[1], [r1]		
		
	M_LRtoPC
	@ENDP	@ARMV7_ITransform4x4_DCOnly_Add
	

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	void RV_FASTCALL ARMV7_Intra16x16ITransform4x4(I32 *pQuantBuf)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ARMV7_Intra16x16ITransform4x4:	@PROC
	
	mov			r12, r0
	vld1.64		{q0}, [r0]!		
	vld1.64		{q1}, [r0]!
	vld1.64		{q2}, [r0]!
	vld1.64		{q3}, [r0]	
	vmov.i32	CONST_13_Q, #13
	vmov.i32	CONST_17_Q, #17
	
	M_IDCT4x4_Core		@horizontal

	@4x4 Transpose
	vtrn.32		q0, q1
	vtrn.32		q2, q3
	vswp		d1, d4
	vswp		d3, d6
	
	M_IDCT4x4_Core		@vertical
	
	@4x4 Transpose
	vtrn.32		q0, q1
	vtrn.32		q2, q3
	vswp		d1, d4
	vswp		d3, d6
	
	vshl.s32	q4, q0, #1
	vshl.s32	q5, q1, #1
	vadd.s32	q0, q0, q4
	vshl.s32	q6, q2, #1
	vshr.s32	q0, q0, #11
	vadd.s32	q1, q1, q5
	vshl.s32	q7, q3, #1
	vshr.s32	q1, q1, #11	
	vadd.s32	q2, q2, q6
	vadd.s32	q3, q3, q7	
	vshr.s32	q2, q2, #11
	vshr.s32	q3, q3, #11
		
	vst1.64	  {q0}, [r12]!
	vst1.64		{q1}, [r12]!
	vst1.64		{q2}, [r12]!
	vst1.64		{q3}, [r12]	
		
	M_LRtoPC
	@ENDP	@ARMV7_Intra16x16ITransform4x4
	@END