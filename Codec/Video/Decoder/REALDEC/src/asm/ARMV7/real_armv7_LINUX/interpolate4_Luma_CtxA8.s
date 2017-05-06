@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2009					*
@*																			*
@*****************************************************************************


	
@R0 = pSrc
@R1 = pDst
@R2 = uSrcPitch
@R3 = uDstPitch
@R4,R5,R12 = Tmp


	@AREA	|.text|, CODE
	.section .text
	
	.global	Interpolate4_H00V00_CtxA8
	.global	Interpolate4_H01V00_CtxA8
	.global	Interpolate4_H02V00_CtxA8
	.global	Interpolate4_H03V00_CtxA8
	.global	Interpolate4_H00V01_CtxA8
	.global	Interpolate4_H01V01_CtxA8
	.global	Interpolate4_H02V01_CtxA8
	.global	Interpolate4_H03V01_CtxA8
	.global	Interpolate4_H00V02_CtxA8
	.global	Interpolate4_H01V02_CtxA8
	.global	Interpolate4_H02V02_CtxA8
	.global	Interpolate4_H03V02_CtxA8
	.global	Interpolate4_H00V03_CtxA8
	.global	Interpolate4_H01V03_CtxA8
	.global	Interpolate4_H02V03_CtxA8
	.global	Interpolate4_H03V03_CtxA8
	
	.global	Interpolate4Add_H00V00_CtxA8
	.global	Interpolate4Add_H01V00_CtxA8
	.global	Interpolate4Add_H02V00_CtxA8
	.global	Interpolate4Add_H03V00_CtxA8
	.global	Interpolate4Add_H00V01_CtxA8
	.global	Interpolate4Add_H01V01_CtxA8
	.global	Interpolate4Add_H02V01_CtxA8
	.global	Interpolate4Add_H03V01_CtxA8
	.global	Interpolate4Add_H00V02_CtxA8
	.global	Interpolate4Add_H01V02_CtxA8
	.global	Interpolate4Add_H02V02_CtxA8
	.global	Interpolate4Add_H03V02_CtxA8
	.global	Interpolate4Add_H00V03_CtxA8
	.global	Interpolate4Add_H01V03_CtxA8
	.global	Interpolate4Add_H02V03_CtxA8
	.global	Interpolate4Add_H03V03_CtxA8

	.MACRO M_pushRegisters
	stmdb	sp!, {r4-r5, lr}
	.ENDM

	.MACRO M_popRegisters
	ldmia	sp!, {r4-r5, pc}
	.ENDM
	
	.MACRO M_pushLR
    str		lr,  [sp, #-4]!
	.ENDM

	.MACRO M_popLR
    ldr		pc,  [sp], #4
	.ENDM
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.MACRO M_StoreData_8x8 pDst, stride
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
   	
	vst1.64		d0, [\pDst], \stride
	vst1.64		d1, [\pDst], \stride
	vst1.64		d2, [\pDst], \stride
	vst1.64		d3, [\pDst], \stride
	vst1.64		d4, [\pDst], \stride
	vst1.64		d5, [\pDst], \stride
	vst1.64		d6, [\pDst], \stride
	vst1.64		d7, [\pDst]
		
	.ENDM
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.MACRO M_loadData_13x5 Pos, Src1
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
   	
	.IF \Pos >= 0
		vld1.32		{q4}, [\Src1], r2
		vld1.32		{q5}, [\Src1], r2
		vld1.32		{q6}, [\Src1], r2
		vld1.32		{q7}, [\Src1], r2
		vld1.32		{q8}, [\Src1], r2
	.ENDIF
	
	.IF \Pos > 0
		vext.8		q4, q4, q9, #\Pos
		vext.8		q5, q5, q9, #\Pos
		vext.8		q6, q6, q9, #\Pos
		vext.8		q7, q7, q9, #\Pos
		vext.8		q8, q8, q9, #\Pos
	.ENDIF
		
	.ENDM
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.MACRO M_loadData_8x8 Pos, Src1, Src2
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
   
	add		\Src2, \Src1, #8
	
	.IF \Pos == 0
		vld1.32		{d0}, [\Src1], r2
		vld1.32		{d1}, [\Src1], r2
		vld1.32		{d2}, [\Src1], r2
		vld1.32		{d3}, [\Src1], r2
		vld1.32		{d4}, [\Src1], r2
		vld1.32		{d5}, [\Src1], r2
		vld1.32		{d6}, [\Src1], r2
		vld1.32		{d7}, [\Src1], r2
	.ENDIF
	
	.IF \Pos > 0
		vld1.32		{d0}, [\Src1], r2
		vld1.32		{d14[0]}, [\Src2], r2
		vld1.32		{d1}, [\Src1], r2
		vld1.32		{d15[0]}, [\Src2], r2
		vld1.32		{d2}, [\Src1], r2
		vld1.32		{d16[0]}, [\Src2], r2
		vld1.32		{d3}, [\Src1], r2
		vld1.32		{d17[0]}, [\Src2], r2
		vld1.32		{d4}, [\Src1], r2
		vld1.32		{d18[0]}, [\Src2], r2
		vld1.32		{d5}, [\Src1], r2
		vld1.32		{d19[0]}, [\Src2], r2
		vld1.32		{d6}, [\Src1], r2
		vld1.32		{d20[0]}, [\Src2], r2
		vld1.32		{d7}, [\Src1], r2
		vld1.32		{d21[0]}, [\Src2], r2
		vext.8		d0, d0, d14, #\Pos
		vext.8		d1, d1, d15, #\Pos
		vext.8		d2, d2, d16, #\Pos
		vext.8		d3, d3, d17, #\Pos
		vext.8		d4, d4, d18, #\Pos
		vext.8		d5, d5, d19, #\Pos
		vext.8		d6, d6, d20, #\Pos
		vext.8		d7, d7, d21, #\Pos
	.ENDIF
		
	.ENDM
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.MACRO M_loadData_9x9 Pos, Src1, Src2
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
   
	add		\Src2, \Src1, #8
	
	.IF \Pos == 0
		vld1.32		{d0}, [\Src1], r2
		vld1.8		{d1[0]}, [\Src2], r2
		vld1.32		{d2}, [\Src1], r2
		vld1.8		{d3[0]}, [\Src2], r2
		vld1.32		{d4}, [\Src1], r2
		vld1.8		{d5[0]}, [\Src2], r2
		vld1.32		{d6}, [\Src1], r2
		vld1.8		{d7[0]}, [\Src2], r2
		vld1.32		{d8}, [\Src1], r2
		vld1.8		{d9[0]}, [\Src2], r2
		vld1.32		{d10}, [\Src1], r2
		vld1.8		{d11[0]}, [\Src2], r2
		vld1.32		{d12}, [\Src1], r2
		vld1.8		{d13[0]}, [\Src2], r2
		vld1.32		{d14}, [\Src1], r2
		vld1.8		{d15[0]}, [\Src2], r2
		vld1.32		{d16}, [\Src1], r2
		vld1.8		{d17[0]}, [\Src2], r2
	.ENDIF
	
	.IF \Pos > 0
		vld1.32		{d0}, [\Src1], r2
		vld1.32		{d1[0]}, [\Src2], r2
		vld1.32		{d2}, [\Src1], r2
		vld1.32		{d3[0]}, [\Src2], r2
		vld1.32		{d4}, [\Src1], r2
		vld1.32		{d5[0]}, [\Src2], r2
		vld1.32		{d6}, [\Src1], r2
		vld1.32		{d7[0]}, [\Src2], r2
		vld1.32		{d8}, [\Src1], r2
		vld1.32		{d9[0]}, [\Src2], r2
		vld1.32		{d10}, [\Src1], r2
		vld1.32		{d11[0]}, [\Src2], r2
		vld1.32		{d12}, [\Src1], r2
		vld1.32		{d13[0]}, [\Src2], r2
		vld1.32		{d14}, [\Src1], r2
		vld1.32		{d15[0]}, [\Src2], r2
		vld1.32		{d16}, [\Src1], r2
		vld1.32		{d17[0]}, [\Src2], r2
		vext.8		q0, q0, q9, #\Pos
		vext.8		q1, q1, q9, #\Pos
		vext.8		q2, q2, q9, #\Pos
		vext.8		q3, q3, q9, #\Pos
		vext.8		q4, q4, q9, #\Pos
		vext.8		q5, q5, q9, #\Pos
		vext.8		q6, q6, q9, #\Pos
		vext.8		q7, q7, q9, #\Pos
		vext.8		q8, q8, q9, #\Pos
	.ENDIF
		
	.ENDM
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.MACRO M_loadData_8x13 Pos, Src1, Src2
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
   
	add		\Src2, \Src1, #8
	
	.IF \Pos == 0
		vld1.32		{d0}, [\Src1], r2
		vld1.32		{d1}, [\Src1], r2
		vld1.32		{d2}, [\Src1], r2
		vld1.32		{d3}, [\Src1], r2
		vld1.32		{d4}, [\Src1], r2
		vld1.32		{d5}, [\Src1], r2
		vld1.32		{d6}, [\Src1], r2
		vld1.32		{d7}, [\Src1], r2
		vld1.32		{d8}, [\Src1], r2
		vld1.32		{d9}, [\Src1], r2
		vld1.32		{d10}, [\Src1], r2
		vld1.32		{d11}, [\Src1], r2
		vld1.32		{d12}, [\Src1], r2
	.ENDIF
	
	.IF \Pos > 0
		vld1.32		{d0}, [\Src1], r2
		vld1.32		{d14[0]}, [\Src2], r2
		vld1.32		{d1}, [\Src1], r2
		vld1.32		{d15[0]}, [\Src2], r2
		vld1.32		{d2}, [\Src1], r2
		vld1.32		{d16[0]}, [\Src2], r2
		vld1.32		{d3}, [\Src1], r2
		vld1.32		{d17[0]}, [\Src2], r2
		vld1.32		{d4}, [\Src1], r2
		vld1.32		{d18[0]}, [\Src2], r2
		vld1.32		{d5}, [\Src1], r2
		vld1.32		{d19[0]}, [\Src2], r2
		vld1.32		{d6}, [\Src1], r2
		vld1.32		{d20[0]}, [\Src2], r2
		vld1.32		{d7}, [\Src1], r2
		vld1.32		{d21[0]}, [\Src2], r2
		vld1.32		{d8}, [\Src1], r2
		vld1.32		{d22[0]}, [\Src2], r2
		vld1.32		{d9}, [\Src1], r2
		vld1.32		{d23[0]}, [\Src2], r2
		vld1.32		{d10}, [\Src1], r2
		vld1.32		{d24[0]}, [\Src2], r2
		vld1.32		{d11}, [\Src1], r2
		vld1.32		{d25[0]}, [\Src2], r2
		vld1.32		{d12}, [\Src1], r2
		vld1.32		{d26[0]}, [\Src2], r2
		vext.8		d0, d0, d14, #\Pos
		vext.8		d1, d1, d15, #\Pos
		vext.8		d2, d2, d16, #\Pos
		vext.8		d3, d3, d17, #\Pos
		vext.8		d4, d4, d18, #\Pos
		vext.8		d5, d5, d19, #\Pos
		vext.8		d6, d6, d20, #\Pos
		vext.8		d7, d7, d21, #\Pos
		vext.8		d8, d8, d22, #\Pos
		vext.8		d9, d9, d23, #\Pos
		vext.8		d10, d10, d24, #\Pos
		vext.8		d11, d11, d25, #\Pos
		vext.8		d12, d12, d26, #\Pos
	.ENDIF
		
	.ENDM
	

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

getSrc_13x5:	@PROC
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    M_pushLR
    
	movs	r12, r0, lsl #30
	beq		LAB0_13x5
	cmps	r12, #0x80000000
	beq		LAB2_13x5
	bhi		LAB3_13x5
LAB1_13x5:
	bic		r12, r0, #3
	M_loadData_13x5 1, r12
    M_popLR
LAB2_13x5:	
	bic		r12, r0, #3
	M_loadData_13x5 2, r12
    M_popLR
LAB3_13x5:	
	bic		r12, r0, #3
	M_loadData_13x5 3, r12
    M_popLR
LAB0_13x5:
	mov		r12, r0
	M_loadData_13x5 0, r12
    M_popLR

	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

getSrc_8x13:		@PROC
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    M_pushLR
    
	pld		[r0]
	pld		[r0, r2]
	movs	r12, r0, lsl #30
	beq		LAB0_8x13
	cmps	r12, #0x80000000
	beq		LAB2_8x13
	bhi		LAB3_8x13
LAB1_8x13:
	bic		r12, r0, #3
	M_loadData_8x13 1, r12, r4
    M_popLR
LAB2_8x13:	
	bic		r12, r0, #3
	M_loadData_8x13 2, r12, r4
    M_popLR
LAB3_8x13:	
	bic		r12, r0, #3
	M_loadData_8x13 3, r12, r4
    M_popLR
LAB0_8x13:	
	mov		r12, r0
	M_loadData_8x13 0, r12, r4
    M_popLR

	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

getSrc_13x8_transpose:	@PROC
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    M_pushLR
    
    mov			r4, r0
    add			r5, r4, #4
    add			r12, r4, #8
    add			lr, r4, #12
	vld4.8		{ d0[0], d1[0],  d2[0],  d3[0] }, [r4], r2
	vld4.8		{ d4[0], d5[0],  d6[0],  d7[0] }, [r5], r2
	vld4.8		{ d8[0], d9[0], d10[0], d11[0] }, [r12], r2
	vld1.8		{ d12[0] }, [lr], r2
	vld4.8		{ d0[1], d1[1],  d2[1],  d3[1] }, [r4], r2
	vld4.8		{ d4[1], d5[1],  d6[1],  d7[1] }, [r5], r2
	vld4.8		{ d8[1], d9[1], d10[1], d11[1] }, [r12], r2
	vld1.8		{ d12[1] }, [lr], r2
	vld4.8		{ d0[2], d1[2],  d2[2],  d3[2] }, [r4], r2
	vld4.8		{ d4[2], d5[2],  d6[2],  d7[2] }, [r5], r2
	vld4.8		{ d8[2], d9[2], d10[2], d11[2] }, [r12], r2
	vld1.8		{ d12[2] }, [lr], r2
	vld4.8		{ d0[3], d1[3],  d2[3],  d3[3] }, [r4], r2
	vld4.8		{ d4[3], d5[3],  d6[3],  d7[3] }, [r5], r2
	vld4.8		{ d8[3], d9[3], d10[3], d11[3] }, [r12], r2
	vld1.8		{ d12[3] }, [lr], r2
	vld4.8		{ d0[4], d1[4],  d2[4],  d3[4] }, [r4], r2
	vld4.8		{ d4[4], d5[4],  d6[4],  d7[4] }, [r5], r2
	vld4.8		{ d8[4], d9[4], d10[4], d11[4] }, [r12], r2
	vld1.8		{ d12[4] }, [lr], r2
	vld4.8		{ d0[5], d1[5],  d2[5],  d3[5] }, [r4], r2
	vld4.8		{ d4[5], d5[5],  d6[5],  d7[5] }, [r5], r2
	vld4.8		{ d8[5], d9[5], d10[5], d11[5] }, [r12], r2
	vld1.8		{ d12[5] }, [lr], r2
	vld4.8		{ d0[6], d1[6],  d2[6],  d3[6] }, [r4], r2
	vld4.8		{ d4[6], d5[6],  d6[6],  d7[6] }, [r5], r2
	vld4.8		{ d8[6], d9[6], d10[6], d11[6] }, [r12], r2
	vld1.8		{ d12[6] }, [lr], r2
	vld4.8		{ d0[7], d1[7],  d2[7],  d3[7] }, [r4], r2
	vld4.8		{ d4[7], d5[7],  d6[7],  d7[7] }, [r5], r2
	vld4.8		{ d8[7], d9[7], d10[7], d11[7] }, [r12], r2
	vld1.8		{ d12[7] }, [lr], r2
	
    M_popLR

	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

AddRow4:	@PROC
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
    mov			r12, r1
	vld1.64		{d8 }, [r1], r3
	vld1.64		{d9 }, [r1], r3
	vld1.64		{d10}, [r1], r3
	vld1.64		{d11}, [r1], r3
	vld1.64		{d12}, [r1], r3
	vld1.64		{d13}, [r1], r3
	vld1.64		{d14}, [r1], r3
	vld1.64		{d15}, [r1]
	
	vrhadd.u8	q0, q0, q4
	vrhadd.u8	q1, q1, q5
	vrhadd.u8	q2, q2, q6
	vrhadd.u8	q3, q3, q7	
	vst1.64		d0, [r12], r3
	vst1.64		d1, [r12], r3
	vst1.64		d2, [r12], r3
	vst1.64		d3, [r12], r3
	vst1.64		d4, [r12], r3
	vst1.64		d5, [r12], r3
	vst1.64		d6, [r12], r3
	vst1.64		d7, [r12]
    
    mov		pc, lr	@return

	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@  13x8 Interpolate Core
@
@  After transpose src data of H, H interpolate @PROCess is the same as V(13x8).
@  So they can share the same core code.
@  h/v filter (1, -5, vf1, vf2, -5, 1)@ vf1,vf2:Input parameters
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ global registers
@d30 = vf1
@d31 = vf2
@q14 = RND
@d0~d12 = input data
@d0~d7  = dst data to store

	.MACRO M_Interpolate_Core	Is_MV2

	@	if (MV == 2) {
	@		RND = 16@
	@		SHIFT_NUM = 5@
	@	} else {
	@		RND = 32@
	@		SHIFT_NUM = 6@
	@	}
	@	v0 = ( (t0 + t5) - 5*(t1 + t4) + vf1*t2 + vf2*t3 + RND) >> SHIFT_NUM@
	@	*pDst = ClampVal(v0)@
	
    @Row 0,1
    
	vaddl.u8	q7, d0, d5	
	vmlal.u8	q7, d2, d30		
	vaddl.u8	q10, d1, d6	
	vaddl.u8	q8, d1, d4	
	vaddl.u8	q11, d2, d5	
	vmlal.u8	q10, d3, d30		
  .IF \Is_MV2 == 1
	vmov.i16	q14, #16		@RND
  .ELSE
	vmov.i16	q14, #32		@RND
  .ENDIF
	vshl.u16	q9, q8, #2	
	vmlal.u8	q7, d3, d31		
	vshl.u16	q12, q11, #2	
	vadd.u16	q8, q8, q9	
	vmlal.u8	q10, d4, d31		
	vadd.u16	q11, q11, q12	
	vsub.s16	q8, q14, q8	
	vsub.s16	q11, q14, q11	
	vadd.s16	q8, q8, q7	
	vaddl.u8	q7, d2, d7		@@@
	vadd.s16	q11, q11, q10	
  .IF \Is_MV2 == 1
	vqshrun.s16	d0, q8, #5	
	vqshrun.s16	d1, q11, #5	
  .ELSE
	vqshrun.s16	d0, q8, #6	
	vqshrun.s16	d1, q11, #6	
  .ENDIF

   @Row 2,3
    
	vmlal.u8	q7, d4, d30		
	vaddl.u8	q10, d3, d8	
	vaddl.u8	q8, d3, d6	
	vmlal.u8	q10, d5, d30		
	vaddl.u8	q11, d4, d7	
	vshl.u16	q9, q8, #2	
	vmlal.u8	q7, d5, d31		
	vshl.u16	q12, q11, #2	
	vadd.u16	q8, q8, q9	
	vmlal.u8	q10, d6, d31		
	vadd.u16	q11, q11, q12	
	vsub.s16	q8, q14, q8	
	vsub.s16	q11, q14, q11	
	vadd.s16	q8, q8, q7	
	vaddl.u8	q7, d4, d9		@@@
	vadd.s16	q11, q11, q10	
  .IF \Is_MV2 == 1
	vqshrun.s16	d2, q8, #5	
	vqshrun.s16	d3, q11, #5	
  .ELSE
	vqshrun.s16	d2, q8, #6	
	vqshrun.s16	d3, q11, #6	
  .ENDIF

   @Row 4,5
    
	vmlal.u8	q7, d6, d30		
	vaddl.u8	q10, d5, d10	
	vaddl.u8	q8, d5, d8	
	vmlal.u8	q10, d7, d30		
	vaddl.u8	q11, d6, d9	
	vshl.u16	q9, q8, #2	
	vmlal.u8	q7, d7, d31		
	vshl.u16	q12, q11, #2	
	vadd.u16	q8, q8, q9	
	vmlal.u8	q10, d8, d31		
	vadd.u16	q11, q11, q12	
	vsub.s16	q8, q14, q8	
	vsub.s16	q11, q14, q11	
	vadd.s16	q8, q8, q7	
	vaddl.u8	q7, d6, d11		@@@
	vadd.s16	q11, q11, q10	
  .IF \Is_MV2 == 1
	vqshrun.s16	d4, q8, #5	
	vqshrun.s16	d5, q11, #5	
  .ELSE
	vqshrun.s16	d4, q8, #6	
	vqshrun.s16	d5, q11, #6	
  .ENDIF

   @Row 6,7
    
	vmlal.u8	q7, d8, d30		
	vaddl.u8	q10, d7, d12	
	vaddl.u8	q8, d7, d10	
	vmlal.u8	q10, d9, d30		
	vaddl.u8	q11, d8, d11	
	vshl.u16	q9, q8, #2	
	vmlal.u8	q7, d9, d31		
	vshl.u16	q12, q11, #2	
	vadd.u16	q8, q8, q9	
	vmlal.u8	q10, d10, d31		
	vadd.u16	q11, q11, q12	
	vsub.s16	q8, q14, q8	
	vsub.s16	q11, q14, q11	
	vadd.s16	q8, q8, q7	
	vadd.s16	q11, q11, q10	
  .IF \Is_MV2 == 1
	vqshrun.s16	d6, q8, #5	
	vqshrun.s16	d7, q11, #5	
  .ELSE
	vqshrun.s16	d6, q8, #6	
	vqshrun.s16	d7, q11, #6	
  .ENDIF
	
	.ENDM	

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	Horizontal 13x5 Interpolate
@   13x5 input -> 8x5 output
@   h filter (1, -5, vf1, vf2, -5, 1)@ vf1/vf2:Input parameters
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.MACRO M_Interpolate_Core_H_13x5	Is_MV2

	@	if (MV == 2) {
	@		RND = 16@
	@		SHIFT_NUM = 5@
	@	} else {
	@		RND = 32@
	@		SHIFT_NUM = 6@
	@	}
	@	v0 = ( (t0 + t5) - 5*(t1 + t4) + vf1*t2 + vf2*t3 + RND) >> SHIFT_NUM@
	@	*pDst = ClampVal(v0)@

@ global registers
@d0~d7 : Can't be covered.
@q4~q8 = input data
@d8~d12 = output data
@d30 = vf1
@d31 = vf2
@q14 = RND


	@Row 8
	
	vext.8		d19, d8, d9, #2
	vext.8		d28, d8, d9, #5
	vaddl.u8	q11, d8, d28	
	vmlal.u8	q11, d19, d30		
	vext.8		d20, d8, d9, #3
	vext.8		d18, d8, d9, #1
	vext.8		d21, d8, d9, #4
  .IF \Is_MV2 == 1
 	vmov.i16	q14, #16		@RND
  .ELSE
	vmov.i16	q14, #32		@RND
  .ENDIF
	vaddl.u8	q12, d18, d21	
	vshl.u16	q13, q12, #2	
	vmlal.u8	q11, d20, d31		
	vadd.u16	q12, q12, q13	
	vext.8		d19, d10, d11, #2
	vsub.s16	q12, q14, q12	
	vext.8		d20, d10, d11, #3
	vext.8		d9, d10, d11, #5
	vadd.s16	q12, q12, q11	
	vaddl.u8	q11, d10, d9	
  .IF \Is_MV2 == 1
	vqshrun.s16	d8, q12, #5		@>>5
  .ELSE
	vqshrun.s16	d8, q12, #6		@>>6
  .ENDIF

	@Row 9
	
	vmlal.u8	q11, d19, d30		
	vext.8		d18, d10, d11, #1
	vext.8		d21, d10, d11, #4
	vaddl.u8	q12, d18, d21	
	vshl.u16	q13, q12, #2	
	vmlal.u8	q11, d20, d31		
	vadd.u16	q12, q12, q13	
	vext.8		d19, d12, d13, #2
	vsub.s16	q12, q14, q12	
	vext.8		d28, d12, d13, #5
	vext.8		d20, d12, d13, #3
	vadd.s16	q12, q12, q11	
	vaddl.u8	q11, d12, d28	
  .IF \Is_MV2 == 1
	vqshrun.s16	d9, q12, #5		@>>5
  .ELSE
	vqshrun.s16	d9, q12, #6		@>>6
  .ENDIF

	@Row 10
	
	vmlal.u8	q11, d19, d30		
	vext.8		d18, d12, d13, #1
	vext.8		d21, d12, d13, #4
  .IF \Is_MV2 == 1
 	vmov.i16	q14, #16		@RND
  .ELSE
	vmov.i16	q14, #32		@RND
  .ENDIF
	vaddl.u8	q12, d18, d21	
	vshl.u16	q13, q12, #2	
	vmlal.u8	q11, d20, d31		
	vadd.u16	q12, q12, q13	
	vext.8		d19, d14, d15, #2
	vsub.s16	q12, q14, q12	
	vext.8		d11, d14, d15, #5
	vext.8		d20, d14, d15, #3
	vadd.s16	q12, q12, q11	
	vaddl.u8	q11, d14, d11	
  .IF \Is_MV2 == 1
	vqshrun.s16	d10, q12, #5		@>>5
  .ELSE
	vqshrun.s16	d10, q12, #6		@>>6
  .ENDIF

	@Row 11
	
	vmlal.u8	q11, d19, d30		
	vext.8		d18, d14, d15, #1
	vext.8		d21, d14, d15, #4
	vaddl.u8	q12, d18, d21	
	vshl.u16	q13, q12, #2	
	vmlal.u8	q11, d20, d31		
	vadd.u16	q12, q12, q13	
	vext.8		d19, d16, d17, #2
	vsub.s16	q12, q14, q12	
	vext.8		d20, d16, d17, #3
	vext.8		d28, d16, d17, #5
	vadd.s16	q12, q12, q11	
	vaddl.u8	q11, d16, d28	
  .IF \Is_MV2 == 1
	vqshrun.s16	d11, q12, #5		@>>5
  .ELSE
	vqshrun.s16	d11, q12, #6		@>>6
  .ENDIF

	@Row 12
	
	vmlal.u8	q11, d19, d30		
	vext.8		d18, d16, d17, #1
	vext.8		d21, d16, d17, #4
	vaddl.u8	q12, d18, d21	
	vshl.u16	q13, q12, #2	
	vmlal.u8	q11, d20, d31		
  .IF \Is_MV2 == 1
 	vmov.i16	q14, #16		@RND
  .ELSE
	vmov.i16	q14, #32		@RND
  .ENDIF
	vadd.u16	q12, q12, q13	
	vsub.s16	q12, q14, q12	
	vadd.s16	q12, q12, q11	
  .IF \Is_MV2 == 1
	vqshrun.s16	d12, q12, #5		@>>5
  .ELSE
	vqshrun.s16	d12, q12, #6		@>>6
  .ENDIF
	
	.ENDM	
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	Vertical Interpolate 
@
@   8x13 input -> 8x8 output
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@input  :d0~d12
@output :d0~d7


Interpolate4_Vert_8x13:	@PROC
    M_pushLR
	M_Interpolate_Core	0
    M_popLR
	@ENDP

Interpolate4_Vert_8x13_MV2:	@PROC
    M_pushLR
	M_Interpolate_Core	1
    M_popLR
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	Horizontal Interpolate 13x8
@
@   13x8 transpose input -> 8x8 output
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@input  :d0~d12
@output :d0~d7

Interpolate4_Horiz_13x8:	@PROC
    M_pushLR

	M_Interpolate_Core	0
	
	vtrn.8		d0, d1
	vtrn.8		d2, d3
	vtrn.8		d4, d5
	vtrn.8		d6, d7
	vtrn.16		d0, d2
	vtrn.16		d1, d3
	vtrn.16		d4, d6
	vtrn.16		d5, d7
	vtrn.32		d0, d4
	vtrn.32		d1, d5
	vtrn.32		d2, d6
	vtrn.32		d3, d7

    M_popLR
	@ENDP

Interpolate4_Horiz_13x8_MV2:	@PROC
    M_pushLR

	M_Interpolate_Core	1
	
	vtrn.8		d0, d1
	vtrn.8		d2, d3
	vtrn.8		d4, d5
	vtrn.8		d6, d7
	vtrn.16		d0, d2
	vtrn.16		d1, d3
	vtrn.16		d4, d6
	vtrn.16		d5, d7
	vtrn.32		d0, d4
	vtrn.32		d1, d5
	vtrn.32		d2, d6
	vtrn.32		d3, d7

    M_popLR
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	Horizontal Interpolate 13x5
@
@   13x5 input -> 8x5 output
@   h filter (1, -5, vf1, vf2, -5, 1)@ vf1/vf2:Input parameters
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@d0~d7 : Can't be covered.
@q4~q8 = input/output data
@d30 = vf1
@d31 = vf2


Interpolate4_Horiz_13x5:		@PROC
    M_pushLR
    
	M_Interpolate_Core_H_13x5 0
	
    M_popLR
	@ENDP

Interpolate4_Horiz_13x5_MV2:		@PROC
    M_pushLR

	M_Interpolate_Core_H_13x5 1

    M_popLR
	@ENDP
	
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	No interpolation required, simple block copy.
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4_H00V00_CtxA8:	@PROC
	M_pushRegisters

	pld		[r0]
	pld		[r0, r2]
	movs	r12, r0, lsl #30
	beq		LAB0_H00V00
	cmps	r12, #0x80000000
	beq		LAB2_H00V00
	bhi		LAB3_H00V00
LAB1_H00V00:
	bic		r0, r0, #3
	M_loadData_8x8	1, r0, r12
    b		H00V00_copy
LAB2_H00V00:	
	bic		r0, r0, #3
	M_loadData_8x8	2, r0, r12
    b		H00V00_copy
LAB3_H00V00:	
	bic		r0, r0, #3
	M_loadData_8x8	3, r0, r12
    b		H00V00_copy
LAB0_H00V00:	
	M_loadData_8x8  0, r0, r12

H00V00_copy:
	M_StoreData_8x8 r1, r3
		
	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	No interpolation required, simple block copy.
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4Add_H00V00_CtxA8:	@PROC
	M_pushRegisters

	pld		[r0]
	pld		[r0, r2]
	movs	r12, r0, lsl #30
	beq		LAB0_H00V00_Add
	cmps	r12, #0x80000000
	beq		LAB2_H00V00_Add
	bhi		LAB3_H00V00_Add
LAB1_H00V00_Add:
	bic		r0, r0, #3
	M_loadData_8x8	1, r0, r12
    b		H00V00_Add_copy
LAB2_H00V00_Add:	
	bic		r0, r0, #3
	M_loadData_8x8	2, r0, r12
    b		H00V00_Add_copy
LAB3_H00V00_Add:	
	bic		r0, r0, #3
	M_loadData_8x8	3, r0, r12
    b		H00V00_Add_copy
LAB0_H00V00_Add:	
	M_loadData_8x8  0, r0, r12

H00V00_Add_copy:
	bl		AddRow4
			
	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,52,20,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4_H01V00_CtxA8:	@PROC
	M_pushRegisters

	@Load 13x8 src data and transpose them to d0~d12
	sub			r0, r0, #2
	bl			getSrc_13x8_transpose
	
	@Do H 13x8 interpolate and the result is in d0~d7
	vmov.i8		d30, #52
	vmov.i8		d31, #20
	bl			Interpolate4_Horiz_13x8	

	@Store data to dst buffer
	M_StoreData_8x8 r1, r3

	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4Add_H01V00_CtxA8:	@PROC
	M_pushRegisters

	@Load 13x8 src data and transpose them to d0~d12
	sub			r0, r0, #2
	bl			getSrc_13x8_transpose
	
	@Do H 13x8 interpolate and the result is in d0~d7
	vmov.i8		d30, #52
	vmov.i8		d31, #20
	bl			Interpolate4_Horiz_13x8	

	bl			AddRow4

	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,20,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4_H02V00_CtxA8:	@PROC
	M_pushRegisters

	@Load 13x8 src data and transpose them to d0~d12
	sub			r0, r0, #2
	bl			getSrc_13x8_transpose
	
	@Do H 13x8 interpolate and the result is in d0~d7
	vmov.i8		q15, #20
	bl			Interpolate4_Horiz_13x8_MV2	

	@Store data to dst buffer
	M_StoreData_8x8 r1, r3

	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4Add_H02V00_CtxA8:	@PROC
	M_pushRegisters

	@Load 13x8 src data and transpose them to d0~d12
	sub			r0, r0, #2
	bl			getSrc_13x8_transpose
	
	@Do H 13x8 interpolate and the result is in d0~d7
	vmov.i8		q15, #20
	bl			Interpolate4_Horiz_13x8_MV2	

	bl			AddRow4

	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4_H03V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,52,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4_H03V00_CtxA8:	@PROC
	M_pushRegisters

	@Load 13x8 src data and transpose them to d0~d12
	sub			r0, r0, #2
	bl			getSrc_13x8_transpose
	
	@Do H 13x8 interpolate and the result is in d0~d7
	vmov.i8		d30, #20
	vmov.i8		d31, #52
	bl			Interpolate4_Horiz_13x8	

	@Store data to dst buffer
	M_StoreData_8x8 r1, r3

	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H03V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4Add_H03V00_CtxA8:	@PROC
	M_pushRegisters

	@Load 13x8 src data and transpose them to d0~d12
	sub			r0, r0, #2
	bl			getSrc_13x8_transpose
	
	@Do H 13x8 interpolate and the result is in d0~d7
	vmov.i8		d30, #20
	vmov.i8		d31, #52
	bl			Interpolate4_Horiz_13x8	

	bl			AddRow4

	M_popRegisters	
	@ENDP
		
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	v filter (1,-5,52,20,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4_H00V01_CtxA8:	@PROC
	M_pushRegisters

	@Load 8x13 src data to d0~d12
	sub			r0, r0, r2, lsl #1
	bl			getSrc_8x13
	
	@Do V 8x13 interpolate and the result is in d0~d7
	vmov.i8		d30, #52
	vmov.i8		d31, #20
	bl			Interpolate4_Vert_8x13
	
	M_StoreData_8x8 r1, r3
	
	M_popRegisters	
	@ENDP
		
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4Add_H00V01_CtxA8:	@PROC
	M_pushRegisters

	@Load 8x13 src data to d0~d12
	sub			r0, r0, r2, lsl #1
	bl			getSrc_8x13
	
	@Do V 8x13 interpolate and the result is in d0~d7
	vmov.i8		d30, #52
	vmov.i8		d31, #20
	bl			Interpolate4_Vert_8x13

	bl			AddRow4

	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h/v filter (1,-5,52,20,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4_H01V01_CtxA8:	@PROC
	M_pushRegisters

	@Load 13x8 src data and transpose them to d0~d12
	sub			r0, r0, #2
	sub			r0, r0, r2, lsl #1
	bl			getSrc_13x8_transpose
	
	@Do H interpolate and the result is in d0~d7
	vmov.i8		d30, #52
	vmov.i8		d31, #20
	bl			Interpolate4_Horiz_13x8

	@Load 13x5 src data to q4~q8
	add			r0, r0, r2, lsl #3
	bl			getSrc_13x5
	
	@Do H 13x5 interpolate and the result is in d8~d12
	bl			Interpolate4_Horiz_13x5	

	@Do V 8x13 interpolate and the result is in d0~d7
	bl			Interpolate4_Vert_8x13
	M_StoreData_8x8 r1, r3

	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4Add_H01V01_CtxA8:	@PROC
	M_pushRegisters

	@Load 13x8 src data and transpose them to d0~d12
	sub			r0, r0, #2
	sub			r0, r0, r2, lsl #1
	bl			getSrc_13x8_transpose
	
	@Do H interpolate and the result is in d0~d7
	vmov.i8		d30, #52
	vmov.i8		d31, #20
	bl			Interpolate4_Horiz_13x8

	@Load 13x5 src data to q4~q8
	add			r0, r0, r2, lsl #3
	bl			getSrc_13x5
	
	@Do H 13x5 interpolate and the result is in d8~d12
	bl			Interpolate4_Horiz_13x5	

	@Do V 8x13 interpolate and the result is in d0~d7
	bl			Interpolate4_Vert_8x13

	bl			AddRow4

	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,20,-5,1) 
@	v filter (1,-5,52,20,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4_H02V01_CtxA8:	@PROC
	M_pushRegisters

	@Load 13x8 src data and transpose them to d0~d12
	sub			r0, r0, #2
	sub			r0, r0, r2, lsl #1
	bl			getSrc_13x8_transpose
	
	@Do H interpolate and the result is in d0~d7
	vmov.i8		q15, #20
	bl			Interpolate4_Horiz_13x8_MV2

	@Load 13x5 src data to q4~q8
	add			r0, r0, r2, lsl #3
	bl			getSrc_13x5
	
	@Do H 13x5 interpolate and the result is in d8~d12
	bl			Interpolate4_Horiz_13x5_MV2	

	@Do V 8x13 interpolate and the result is in d0~d7
	vmov.i8		d30, #52
	bl			Interpolate4_Vert_8x13
	M_StoreData_8x8 r1, r3

	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_InterpolateAdd4_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4Add_H02V01_CtxA8:	@PROC
	M_pushRegisters

	@Load 13x8 src data and transpose them to d0~d12
	sub			r0, r0, #2
	sub			r0, r0, r2, lsl #1
	bl			getSrc_13x8_transpose
	
	@Do H interpolate and the result is in d0~d7
	vmov.i8		q15, #20
	bl			Interpolate4_Horiz_13x8_MV2

	@Load 13x5 src data to q4~q8
	add			r0, r0, r2, lsl #3
	bl			getSrc_13x5
	
	@Do H 13x5 interpolate and the result is in d8~d12
	bl			Interpolate4_Horiz_13x5_MV2	

	@Do V 8x13 interpolate and the result is in d0~d7
	vmov.i8		d30, #52
	bl			Interpolate4_Vert_8x13

	bl			AddRow4

	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4_H03V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,52,-5,1) 
@	v filter (1,-5,52,20,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4_H03V01_CtxA8:	@PROC
	M_pushRegisters

	@Load 13x8 src data and transpose them to d0~d12
	sub			r0, r0, #2
	sub			r0, r0, r2, lsl #1
	bl			getSrc_13x8_transpose
	
	@Do H interpolate and the result is in d0~d7
	vmov.i8		d30, #20
	vmov.i8		d31, #52
	bl			Interpolate4_Horiz_13x8

	@Load 13x5 src data to q4~q8
	add			r0, r0, r2, lsl #3
	bl			getSrc_13x5
	
	@Do H 13x5 interpolate and the result is in d8~d12
	bl			Interpolate4_Horiz_13x5	

	@Do V 8x13 interpolate and the result is in d0~d7
	vmov.i8		d30, #52
	vmov.i8		d31, #20
	bl			Interpolate4_Vert_8x13
	M_StoreData_8x8 r1, r3

	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H03V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4Add_H03V01_CtxA8:	@PROC
	M_pushRegisters

	@Load 13x8 src data and transpose them to d0~d12
	sub			r0, r0, #2
	sub			r0, r0, r2, lsl #1
	bl			getSrc_13x8_transpose
	
	@Do H interpolate and the result is in d0~d7
	vmov.i8		d30, #20
	vmov.i8		d31, #52
	bl			Interpolate4_Horiz_13x8

	@Load 13x5 src data to q4~q8
	add			r0, r0, r2, lsl #3
	bl			getSrc_13x5
	
	@Do H 13x5 interpolate and the result is in d8~d12
	bl			Interpolate4_Horiz_13x5	

	@Do V 8x13 interpolate and the result is in d0~d7
	vmov.i8		d30, #52
	vmov.i8		d31, #20
	bl			Interpolate4_Vert_8x13

	bl			AddRow4

	M_popRegisters	
	@ENDP
		
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	v filter (1,-5,20,20,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4_H00V02_CtxA8:	@PROC
	M_pushRegisters

	@Load 8x13 src data to d0~d12
	sub			r0, r0, r2, lsl #1
	bl			getSrc_8x13
	
	@Do V 8x13 interpolate and the result is in d0~d7
	vmov.i8		q15, #20
	bl			Interpolate4_Vert_8x13_MV2
	M_StoreData_8x8 r1, r3

	M_popRegisters	
	@ENDP
		
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4Add_H00V02_CtxA8:	@PROC
	M_pushRegisters

	@Load 8x13 src data to d0~d12
	sub			r0, r0, r2, lsl #1
	bl			getSrc_8x13
	
	@Do V 8x13 interpolate and the result is in d0~d7
	vmov.i8		q15, #20
	bl			Interpolate4_Vert_8x13_MV2

	bl			AddRow4

	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,52,20,-5,1) 
@	v filter (1,-5,20,20,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4_H01V02_CtxA8:	@PROC
	M_pushRegisters

	@Load 13x8 src data and transpose them to d0~d12
	sub			r0, r0, #2
	sub			r0, r0, r2, lsl #1
	bl			getSrc_13x8_transpose
	
	@Do H interpolate and the result is in d0~d7
	vmov.i8		d30, #52
	vmov.i8		d31, #20
	bl			Interpolate4_Horiz_13x8

	@Load 13x5 src data to q4~q8
	add			r0, r0, r2, lsl #3
	bl			getSrc_13x5
	
	@Do H 13x5 interpolate and the result is in d8~d12
	bl			Interpolate4_Horiz_13x5	

	@Do V 8x13 interpolate and the result is in d0~d7
	vmov.i8		d30, #20
	bl			Interpolate4_Vert_8x13_MV2
	M_StoreData_8x8 r1, r3

	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4Add_H01V02_CtxA8:	@PROC
	M_pushRegisters

	@Load 13x8 src data and transpose them to d0~d12
	sub			r0, r0, #2
	sub			r0, r0, r2, lsl #1
	bl			getSrc_13x8_transpose
	
	@Do H interpolate and the result is in d0~d7
	vmov.i8		d30, #52
	vmov.i8		d31, #20
	bl			Interpolate4_Horiz_13x8

	@Load 13x5 src data to q4~q8
	add			r0, r0, r2, lsl #3
	bl			getSrc_13x5
	
	@Do H 13x5 interpolate and the result is in d8~d12
	bl			Interpolate4_Horiz_13x5	

	@Do V 8x13 interpolate and the result is in d0~d7
	vmov.i8		d30, #20
	bl			Interpolate4_Vert_8x13_MV2

	bl			AddRow4

	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,20,-5,1) 
@	v filter (1,-5,20,20,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4_H02V02_CtxA8:	@PROC
	M_pushRegisters

	@Load 13x8 src data and transpose them to d0~d12
	sub			r0, r0, #2
	sub			r0, r0, r2, lsl #1
	bl			getSrc_13x8_transpose
	
	@Do H interpolate and the result is in d0~d7
	vmov.i8		q15, #20
	bl			Interpolate4_Horiz_13x8_MV2

	@Load 13x5 src data to q4~q8
	add			r0, r0, r2, lsl #3
	bl			getSrc_13x5
	
	@Do H 13x5 interpolate and the result is in d8~d12
	bl			Interpolate4_Horiz_13x5_MV2	

	@Do V 8x13 interpolate and the result is in d0~d7
	bl			Interpolate4_Vert_8x13_MV2
	M_StoreData_8x8 r1, r3

	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4Add_H02V02_CtxA8:	@PROC
	M_pushRegisters

	@Load 13x8 src data and transpose them to d0~d12
	sub			r0, r0, #2
	sub			r0, r0, r2, lsl #1
	bl			getSrc_13x8_transpose
	
	@Do H interpolate and the result is in d0~d7
	vmov.i8		q15, #20
	bl			Interpolate4_Horiz_13x8_MV2

	@Load 13x5 src data to q4~q8
	add			r0, r0, r2, lsl #3
	bl			getSrc_13x5
	
	@Do H 13x5 interpolate and the result is in d8~d12
	bl			Interpolate4_Horiz_13x5_MV2	

	@Do V 8x13 interpolate and the result is in d0~d7
	bl			Interpolate4_Vert_8x13_MV2

	bl			AddRow4

	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4_H03V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,52,-5,1) 
@	v filter (1,-5,20,20,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4_H03V02_CtxA8:	@PROC
	M_pushRegisters

	@Load 13x8 src data and transpose them to d0~d12
	sub			r0, r0, #2
	sub			r0, r0, r2, lsl #1
	bl			getSrc_13x8_transpose
	
	@Do H interpolate and the result is in d0~d7
	vmov.i8		d30, #20
	vmov.i8		d31, #52
	bl			Interpolate4_Horiz_13x8

	@Load 13x5 src data to q4~q8
	add			r0, r0, r2, lsl #3
	bl			getSrc_13x5
	
	@Do H 13x5 interpolate and the result is in d8~d12
	bl			Interpolate4_Horiz_13x5	

	@Do V 8x13 interpolate and the result is in d0~d7
	vmov.i8		d31, #20
	bl			Interpolate4_Vert_8x13_MV2
	M_StoreData_8x8 r1, r3

	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H03V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4Add_H03V02_CtxA8:	@PROC
	M_pushRegisters

	@Load 13x8 src data and transpose them to d0~d12
	sub			r0, r0, #2
	sub			r0, r0, r2, lsl #1
	bl			getSrc_13x8_transpose
	
	@Do H interpolate and the result is in d0~d7
	vmov.i8		d30, #20
	vmov.i8		d31, #52
	bl			Interpolate4_Horiz_13x8

	@Load 13x5 src data to q4~q8
	add			r0, r0, r2, lsl #3
	bl			getSrc_13x5
	
	@Do H 13x5 interpolate and the result is in d8~d12
	bl			Interpolate4_Horiz_13x5	

	@Do V 8x13 interpolate and the result is in d0~d7
	vmov.i8		d31, #20
	bl			Interpolate4_Vert_8x13_MV2

	bl			AddRow4

	M_popRegisters	
	@ENDP
		
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4_H00V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	v filter (1,-5,20,52,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4_H00V03_CtxA8:	@PROC
	M_pushRegisters

	@Load 8x13 src data to d0~d12
	sub			r0, r0, r2, lsl #1
	bl			getSrc_8x13
	
	@Do V 8x13 interpolate and the result is in d0~d7
	vmov.i8		d30, #20
	vmov.i8		d31, #52
	bl			Interpolate4_Vert_8x13
	M_StoreData_8x8 r1, r3

	M_popRegisters	
	@ENDP
		
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H00V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4Add_H00V03_CtxA8:	@PROC
	M_pushRegisters

	@Load 8x13 src data to d0~d12
	sub			r0, r0, r2, lsl #1
	bl			getSrc_8x13
	
	@Do V 8x13 interpolate and the result is in d0~d7
	vmov.i8		d30, #20
	vmov.i8		d31, #52
	bl			Interpolate4_Vert_8x13

	bl			AddRow4

	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4_H01V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,52,20,-5,1) 
@	v filter (1,-5,20,52,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4_H01V03_CtxA8:	@PROC
	M_pushRegisters

	@Load 13x8 src data and transpose them to d0~d12
	sub			r0, r0, #2
	sub			r0, r0, r2, lsl #1
	bl			getSrc_13x8_transpose
	
	@Do H interpolate and the result is in d0~d7
	vmov.i8		d30, #52
	vmov.i8		d31, #20
	bl			Interpolate4_Horiz_13x8

	@Load 13x5 src data to q4~q8
	add			r0, r0, r2, lsl #3
	bl			getSrc_13x5
	
	@Do H 13x5 interpolate and the result is in d8~d12
	bl			Interpolate4_Horiz_13x5	

	@Do V 8x13 interpolate and the result is in d0~d7
	vmov.i8		d30, #20
	vmov.i8		d31, #52
	bl			Interpolate4_Vert_8x13
	M_StoreData_8x8 r1, r3

	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H01V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4Add_H01V03_CtxA8:	@PROC
	M_pushRegisters

	@Load 13x8 src data and transpose them to d0~d12
	sub			r0, r0, #2
	sub			r0, r0, r2, lsl #1
	bl			getSrc_13x8_transpose
	
	@Do H interpolate and the result is in d0~d7
	vmov.i8		d30, #52
	vmov.i8		d31, #20
	bl			Interpolate4_Horiz_13x8

	@Load 13x5 src data to q4~q8
	add			r0, r0, r2, lsl #3
	bl			getSrc_13x5
	
	@Do H 13x5 interpolate and the result is in d8~d12
	bl			Interpolate4_Horiz_13x5	

	@Do V 8x13 interpolate and the result is in d0~d7
	vmov.i8		d30, #20
	vmov.i8		d31, #52
	bl			Interpolate4_Vert_8x13

	bl			AddRow4

	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4_H02V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,20,-5,1) 
@	v filter (1,-5,20,52,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4_H02V03_CtxA8:	@PROC
	M_pushRegisters

	@Load 13x8 src data and transpose them to d0~d12
	sub			r0, r0, #2
	sub			r0, r0, r2, lsl #1
	bl			getSrc_13x8_transpose
	
	@Do H interpolate and the result is in d0~d7
	vmov.i8		q15, #20
	bl			Interpolate4_Horiz_13x8_MV2

	@Load 13x5 src data to q4~q8
	add			r0, r0, r2, lsl #3
	bl			getSrc_13x5
	
	@Do H 13x5 interpolate and the result is in d8~d12
	bl			Interpolate4_Horiz_13x5_MV2	

	@Do V 8x13 interpolate and the result is in d0~d7
	vmov.i8		d31, #52
	bl			Interpolate4_Vert_8x13
	M_StoreData_8x8 r1, r3

	M_popRegisters	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H02V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4Add_H02V03_CtxA8:	@PROC
	M_pushRegisters

	@Load 13x8 src data and transpose them to d0~d12
	sub			r0, r0, #2
	sub			r0, r0, r2, lsl #1
	bl			getSrc_13x8_transpose
	
	@Do H interpolate and the result is in d0~d7
	vmov.i8		q15, #20
	bl			Interpolate4_Horiz_13x8_MV2

	@Load 13x5 src data to q4~q8
	add			r0, r0, r2, lsl #3
	bl			getSrc_13x5
	
	@Do H 13x5 interpolate and the result is in d8~d12
	bl			Interpolate4_Horiz_13x5_MV2	

	@Do V 8x13 interpolate and the result is in d0~d7
	vmov.i8		d31, #52
	bl			Interpolate4_Vert_8x13

	bl			AddRow4

	M_popRegisters	
	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4_H03V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	t0 = p0 + p1
@   v0 = (t0 + t1 + 2) >> 2
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4_H03V03_CtxA8:	@PROC
	M_pushRegisters

@q0~q8 = src data
	
	pld		[r0]
	pld		[r0, r2]
	movs	r12, r0, lsl #30
	beq		LAB0_H03V03
	cmps	r12, #0x80000000
	beq		LAB2_H03V03
	bhi		LAB3_H03V03
LAB1_H03V03:
	bic		r0, r0, #3
	M_loadData_9x9	1, r0, r12
    b		H03V03_interp
LAB2_H03V03:	
	bic		r0, r0, #3
	M_loadData_9x9	2, r0, r12
    b		H03V03_interp
LAB3_H03V03:	
	bic		r0, r0, #3
	M_loadData_9x9	3, r0, r12
    b		H03V03_interp
LAB0_H03V03:	
	M_loadData_9x9	0, r0, r12

H03V03_interp:
		
	vmov.i16	q14, #2
	vext.8		d1,  d0,  d1,  #1	
	vext.8		d3,  d2,  d3,  #1	
	vext.8		d5,  d4,  d5,  #1	
	vext.8		d7,  d6,  d7,  #1	
	vext.8		d9,  d8,  d9,  #1	
	vext.8		d11, d10, d11, #1	
	vext.8		d13, d12, d13, #1	
	vext.8		d15, d14, d15, #1	
	vext.8		d17, d16, d17, #1	
	
	vaddl.u8	q0, d0,  d1	
	vaddl.u8	q1, d2,  d3 	
	vaddl.u8	q2, d4,  d5	
	vaddl.u8	q3, d6,  d7 	
	vaddl.u8	q4, d8,  d9 	
	vaddl.u8	q5, d10, d11	
	vaddl.u8	q6, d12, d13	
	vaddl.u8	q7, d14, d15	
	vaddl.u8	q8, d16, d17	
		
	vadd.u16	q0, q0, q1
	vadd.u16	q1, q1, q2
	vadd.u16	q2, q2, q3
	vadd.u16	q3, q3, q4
	vadd.u16	q4, q4, q5
	vadd.u16	q5, q5, q6
	vadd.u16	q6, q6, q7
	vadd.u16	q7, q7, q8		
		
	vadd.u16	q0, q0, q14
	vadd.u16	q1, q1, q14
	vadd.u16	q2, q2, q14
	vadd.u16	q3, q3, q14
	vadd.u16	q4, q4, q14
	vadd.u16	q5, q5, q14
	vadd.u16	q6, q6, q14
	vadd.u16	q7, q7, q14
		
	vqshrun.s16	d0, q0, #2
	vqshrun.s16	d1, q1, #2
	vqshrun.s16	d2, q2, #2
	vqshrun.s16	d3, q3, #2
	vqshrun.s16	d4, q4, #2
	vqshrun.s16	d5, q5, #2
	vqshrun.s16	d6, q6, #2
	vqshrun.s16	d7, q7, #2
		
	M_StoreData_8x8 r1, r3
		
	M_popRegisters	
	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H03V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
Interpolate4Add_H03V03_CtxA8:	@PROC
	M_pushRegisters

@q0~q8 = src data
	
	pld		[r0]
	pld		[r0, r2]
	movs	r12, r0, lsl #30
	beq		LAB0_H03V03_Add
	cmps	r12, #0x80000000
	beq		LAB2_H03V03_Add
	bhi		LAB3_H03V03_Add
LAB1_H03V03_Add:
	bic		r0, r0, #3
	M_loadData_9x9	1, r0, r12
    b		H03V03_interp_Add
LAB2_H03V03_Add:
	bic		r0, r0, #3
	M_loadData_9x9	2, r0, r12
    b		H03V03_interp_Add
LAB3_H03V03_Add:	
	bic		r0, r0, #3
	M_loadData_9x9	3, r0, r12
    b		H03V03_interp_Add
LAB0_H03V03_Add:	
	M_loadData_9x9	0, r0, r12

H03V03_interp_Add:
		
	vmov.i16	q14, #2
	vext.8		d1,  d0,  d1,  #1	
	vext.8		d3,  d2,  d3,  #1	
	vext.8		d5,  d4,  d5,  #1	
	vext.8		d7,  d6,  d7,  #1	
	vext.8		d9,  d8,  d9,  #1	
	vext.8		d11, d10, d11, #1	
	vext.8		d13, d12, d13, #1	
	vext.8		d15, d14, d15, #1	
	vext.8		d17, d16, d17, #1	
	
	vaddl.u8	q0, d0,  d1	
	vaddl.u8	q1, d2,  d3 	
	vaddl.u8	q2, d4,  d5	
	vaddl.u8	q3, d6,  d7 	
	vaddl.u8	q4, d8,  d9 	
	vaddl.u8	q5, d10, d11	
	vaddl.u8	q6, d12, d13	
	vaddl.u8	q7, d14, d15	
	vaddl.u8	q8, d16, d17	
		
	vadd.u16	q0, q0, q1
	vadd.u16	q1, q1, q2
	vadd.u16	q2, q2, q3
	vadd.u16	q3, q3, q4
	vadd.u16	q4, q4, q5
	vadd.u16	q5, q5, q6
	vadd.u16	q6, q6, q7
	vadd.u16	q7, q7, q8		
		
	vadd.u16	q0, q0, q14
	vadd.u16	q1, q1, q14
	vadd.u16	q2, q2, q14
	vadd.u16	q3, q3, q14
	vadd.u16	q4, q4, q14
	vadd.u16	q5, q5, q14
	vadd.u16	q6, q6, q14
	vadd.u16	q7, q7, q14
		
	vqshrun.s16	d0, q0, #2
	vqshrun.s16	d1, q1, #2
	vqshrun.s16	d2, q2, #2
	vqshrun.s16	d3, q3, #2
	vqshrun.s16	d4, q4, #2
	vqshrun.s16	d5, q5, #2
	vqshrun.s16	d6, q6, #2
	vqshrun.s16	d7, q7, #2
		
	bl			AddRow4
		
	M_popRegisters	
	@ENDP
	
	@END