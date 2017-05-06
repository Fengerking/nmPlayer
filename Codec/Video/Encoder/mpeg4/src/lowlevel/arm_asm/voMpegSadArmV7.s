;*****************************************************************************
;*																			*
;*		VisualOn, Inc. Confidential and Proprietary, 2003					*
;*																			*
;*****************************************************************************

	
	AREA	|.text|, CODE
	
	EXPORT 	Dev16x16_ARMV7
	EXPORT 	Sad16x16_ARMV7
	EXPORT 	Sad8x8_ARMV7	
	
	
	MACRO 
	M_pushRegisters
	stmdb	sp!, {r4-r7, lr}
	MEND

	MACRO 
	M_popRegisters
	ldmia	sp!, {r4-r7, pc}
	MEND
	


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	MACRO
	M_getCurRef_16x4 $Pos
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	vld1.32		{q8},  [r0], r2
	vld1.32		{q9},  [r0], r2
	vld1.32		{q10}, [r0], r2
	vld1.32		{q11}, [r0], r2
   
	IF $Pos = 0
		vld1.32		{q0}, [r1], r3
		vld1.32		{q1}, [r1], r3
		vld1.32		{q2}, [r1], r3
		vld1.32		{q3}, [r1], r3
	ENDIF
	
	IF $Pos = 1
		vld1.32		{q0}, [r1], r3
		vld1.8		{d8[0]},  [r4], r3
		vld1.32		{q1}, [r1], r3
		vld1.8		{d10[0]},  [r4], r3
		vld1.32		{q2}, [r1], r3
		vld1.8		{d12[0]}, [r4], r3
		vld1.32		{q3}, [r1], r3
		vld1.8		{d14[0]}, [r4], r3
	ENDIF
	IF $Pos = 2
		vld1.32		{q0}, [r1], r3
		vld1.16		{d8[0]},  [r4], r3
		vld1.32		{q1}, [r1], r3
		vld1.16		{d10[0]},  [r4], r3
		vld1.32		{q2}, [r1], r3
		vld1.16		{d12[0]}, [r4], r3
		vld1.32		{q3}, [r1], r3
		vld1.16		{d14[0]}, [r4], r3
	ENDIF
	IF $Pos = 3
		vld1.32		{q0}, [r1], r3
		vld1.32		{d8[0]},  [r4], r3
		vld1.32		{q1}, [r1], r3
		vld1.32		{d10[0]},  [r4], r3
		vld1.32		{q2}, [r1], r3
		vld1.32		{d12[0]}, [r4], r3
		vld1.32		{q3}, [r1], r3
		vld1.32		{d14[0]}, [r4], r3
	ENDIF
	
	;pld	[r1, r3]
	IF $Pos > 0
		vext.8		q0, q0, q4, #$Pos
		vext.8		q1, q1, q5, #$Pos
		vext.8		q2, q2, q6, #$Pos
		vext.8		q3, q3, q7, #$Pos
	ENDIF
	
	MEND	;M_getCurRef_16x4

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	MACRO
	M_getCurRef_16x2 $Pos
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   
	IF $Pos = 0
		vld1.32		{q0}, [r1], r3
		vld1.32		{q1}, [r1], r3
	ENDIF
	
	IF $Pos = 1
		vld1.32		{q0}, [r1], r3
		vld1.8		{d8[0]},  [r4], r3
		vld1.32		{q1}, [r1], r3
		vld1.8		{d10[0]},  [r4], r3
	ENDIF
	IF $Pos = 2
		vld1.32		{q0}, [r1], r3
		vld1.16		{d8[0]},  [r4], r3
		vld1.32		{q1}, [r1], r3
		vld1.16		{d10[0]},  [r4], r3
	ENDIF
	IF $Pos = 3
		vld1.32		{q0}, [r1], r3
		vld1.32		{d8[0]},  [r4], r3
		vld1.32		{q1}, [r1], r3
		vld1.32		{d10[0]},  [r4], r3
	ENDIF
	
	vld1.32		{q8},  [r0], r2
	vld1.32		{q9},  [r0], r2
	
	IF $Pos > 0
		vext.8		q0, q0, q4, #$Pos
		vext.8		q1, q1, q5, #$Pos
	ENDIF
	
	MEND	;M_getCurRef_16x2

	
Offset_best_sad		EQU	20

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	MACRO
	M_sad16_core $Pos
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
;q0-q3 = ref data 
;q8-q11 = cur data
;q14 = acc sad of first 8 pixels 
;q15 = acc sad of next 8 pixels
;q13 = tmp
;r4  = addr of 17th pixel

sad16_core_$Pos

	;pld		[r1, r3]
	ldr			r12, [sp, #Offset_best_sad]	; r12 = best_sad
	add			r4, r1, #16		;ref + 16

	;0-3 rows
	
  IF $Pos > 0	;Is it faster than ELSE case? need to test it.
	
	M_getCurRef_16x2 $Pos	
	vabdl.u8	q14, d0, d16	
	vabdl.u8	q15, d1, d17	
	vabal.u8	q14, d2, d18		
	vabal.u8	q15, d3, d19			
	vadd.u16	q13, q14, q15
	vadd.u16	d26, d27
	vpaddl.u16	d26, d26
	vpaddl.u32	d26, d26
	vmov		lr, r5, d26
	cmp			lr, r12
	bge			sad16_core_$Pos.exit
	
	M_getCurRef_16x2 $Pos	
	vabal.u8	q14, d0, d16	
	vabal.u8	q15, d1, d17	
	vabal.u8	q14, d2, d18		
	vabal.u8	q15, d3, d19			
	vadd.u16	q13, q14, q15
	vadd.u16	d26, d27
	vpaddl.u16	d26, d26
	vpaddl.u32	d26, d26
	vmov		lr, r5, d26
	cmp			lr, r12
	bge			sad16_core_$Pos.exit
	
  ELSE
	
	M_getCurRef_16x4 $Pos	
	vabdl.u8	q14, d0, d16	
	vabdl.u8	q15, d1, d17	
	vabal.u8	q14, d2, d18		
	vabal.u8	q15, d3, d19		
	vabal.u8	q14, d4, d20		
	vabal.u8	q15, d5, d21		
	vabal.u8	q14, d6, d22		
	vabal.u8	q15, d7, d23		
	
	vadd.u16	q13, q14, q15
	vadd.u16	d26, d27
	vpaddl.u16	d26, d26
	vpaddl.u32	d26, d26
	vmov		lr, r5, d26
	cmp			lr, r12
	bge			sad16_core_$Pos.exit
	
  ENDIF
	
	;4-7 rows
	
	M_getCurRef_16x4 $Pos	
	vabal.u8	q14, d0, d16	
	vabal.u8	q15, d1, d17	
	vabal.u8	q14, d2, d18		
	vabal.u8	q15, d3, d19		
	vabal.u8	q14, d4, d20		
	vabal.u8	q15, d5, d21		
	vabal.u8	q14, d6, d22		
	vabal.u8	q15, d7, d23		
	
	vadd.u16	q13, q14, q15
	vadd.u16	d26, d27
	vpaddl.u16	d26, d26
	vpaddl.u32	d26, d26
	vmov		lr, r5, d26
	cmp			lr, r12
	bge			sad16_core_$Pos.exit

	;8-11 rows
	
	M_getCurRef_16x4 $Pos	
	vabal.u8	q14, d0, d16	
	vabal.u8	q15, d1, d17	
	vabal.u8	q14, d2, d18		
	vabal.u8	q15, d3, d19		
	vabal.u8	q14, d4, d20		
	vabal.u8	q15, d5, d21		
	vabal.u8	q14, d6, d22		
	vabal.u8	q15, d7, d23		
	
	vadd.u16	q13, q14, q15
	vadd.u16	d26, d27
	vpaddl.u16	d26, d26
	vpaddl.u32	d26, d26
	vmov		lr, r5, d26
	cmp			lr, r12
	bge			sad16_core_$Pos.exit

	;12-15 rows
	
	M_getCurRef_16x4 $Pos	
	vabal.u8	q14, d0, d16	
	vabal.u8	q15, d1, d17	
	vabal.u8	q14, d2, d18		
	vabal.u8	q15, d3, d19		
	vabal.u8	q14, d4, d20		
	vabal.u8	q15, d5, d21		
	vabal.u8	q14, d6, d22		
	vabal.u8	q15, d7, d23		
	
	vadd.u16	q13, q14, q15
	vadd.u16	d26, d27
	vpaddl.u16	d26, d26
	vpaddl.u32	d26, d26
	vmov		lr, r5, d26

sad16_core_$Pos.exit
	mov			r0, lr
	M_popRegisters	
	
	MEND	;M_sad16_core
	
	
Offset_pSad		EQU	20+0
Offset_cur_mb	EQU	20+4

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	MACRO
	M_sad16v_core $Pos
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
;q0-q3  = ref data 
;q8-q11 = cur data
;q12 = acc sad of block 0 
;q13 = acc sad of block 1
;q14 = acc sad of block 2 
;q15 = acc sad of block 3
;r4  = addr of 17th pixel

sad16v_core_$Pos

	;pld		[r1, r3]
	ldr			r12, [sp, #Offset_pSad]
	ldr			lr, [sp, #Offset_cur_mb]
	add			r4, r1, #16		;ref + 16

	;0-3 rows
	
	M_getCurRef_16x4 $Pos	
	vst1.32		{q8}, [lr]!
	vst1.32		{q9}, [lr]!
	vst1.32		{q10}, [lr]!
	vst1.32		{q11}, [lr]!
	vabdl.u8	q12, d0, d16	
	vabdl.u8	q13, d1, d17	
	vabal.u8	q12, d2, d18		
	vabal.u8	q13, d3, d19		
	vabal.u8	q12, d4, d20		
	vabal.u8	q13, d5, d21		
	vabal.u8	q12, d6, d22		
	vabal.u8	q13, d7, d23		
		
	;4-7 rows
	
	M_getCurRef_16x4 $Pos	
	vst1.32		{q8}, [lr]!
	vst1.32		{q9}, [lr]!
	vst1.32		{q10}, [lr]!
	vst1.32		{q11}, [lr]!
	vabal.u8	q12, d0, d16	
	vabal.u8	q13, d1, d17	
	vabal.u8	q12, d2, d18		
	vabal.u8	q13, d3, d19		
	vabal.u8	q12, d4, d20		
	vabal.u8	q13, d5, d21		
	vabal.u8	q12, d6, d22		
	vabal.u8	q13, d7, d23		

	;8-11 rows
	
	M_getCurRef_16x4 $Pos	
	vst1.32		{q8}, [lr]!
	vst1.32		{q9}, [lr]!
	vst1.32		{q10}, [lr]!
	vst1.32		{q11}, [lr]!
	vadd.u16	d24, d24, d25
	vadd.u16	d25, d26, d27
	vpadd.u16	d24, d24, d25
	vabdl.u8	q14, d0, d16	
	vabdl.u8	q15, d1, d17	
	vabal.u8	q14, d2, d18		
	vabal.u8	q15, d3, d19		
	vabal.u8	q14, d4, d20		
	vabal.u8	q15, d5, d21		
	vabal.u8	q14, d6, d22		
	vabal.u8	q15, d7, d23		
		
	;12-15 rows
	
	M_getCurRef_16x4 $Pos	
	vst1.32		{q8}, [lr]!
	vst1.32		{q9}, [lr]!
	vst1.32		{q10}, [lr]!
	vst1.32		{q11}, [lr]
	vabal.u8	q14, d0, d16	
	vabal.u8	q15, d1, d17	
	vabal.u8	q14, d2, d18		
	vabal.u8	q15, d3, d19		
	vabal.u8	q14, d4, d20		
	vabal.u8	q15, d5, d21		
	vabal.u8	q14, d6, d22		
	vabal.u8	q15, d7, d23		

	vadd.u16	d28, d28, d29
	vadd.u16	d29, d30, d31
	vpadd.u16	d25, d28, d29
	vpaddl.u16	q12, q12
	vmov		r4, r5, d24
	vmov		r6, r7, d25
	
	str			r4, [r12]
	str			r5, [r12, #4]
	add			r0, r4, r5
	str			r6, [r12, #8]
	add			r0, r0, r6
	str			r7, [r12, #12]
	add			r0, r0, r7
	
	M_popRegisters	
	
	MEND	;M_sad16v_core
	  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	MACRO
	M_sad8_core $Pos
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   
sad8_core_$Pos

   add	r12, r1, #8
   
	IF $Pos = 0
		vld1.32		{d0}, [r1], r3
		vld1.32		{d2}, [r1], r3
		vld1.32		{d4}, [r1], r3
		vld1.32		{d6}, [r1], r3
		vld1.32		{d8}, [r1], r3
		vld1.32		{d10}, [r1], r3
		vld1.32		{d12}, [r1], r3
		vld1.32		{d14}, [r1]
	ENDIF
		
	IF $Pos = 1
		vld1.32		{d0}, [r1], r3
		vld1.8		{d1[0]}, [r12], r3
		vld1.32		{d2}, [r1], r3
		vld1.8		{d3[0]}, [r12], r3
		vld1.32		{d4}, [r1], r3
		vld1.8		{d5[0]}, [r12], r3
		vld1.32		{d6}, [r1], r3
		vld1.8		{d7[0]}, [r12], r3
		vld1.32		{d8}, [r1], r3
		vld1.8		{d9[0]}, [r12], r3
		vld1.32		{d10}, [r1], r3
		vld1.8		{d11[0]}, [r12], r3
		vld1.32		{d12}, [r1], r3
		vld1.8		{d13[0]}, [r12], r3
		vld1.32		{d14}, [r1]
		vld1.8		{d15[0]}, [r12]
	ENDIF
	
	IF $Pos = 2
		vld1.32		{d0}, [r1], r3
		vld1.16		{d1[0]}, [r12], r3
		vld1.32		{d2}, [r1], r3
		vld1.16		{d3[0]}, [r12], r3
		vld1.32		{d4}, [r1], r3
		vld1.16		{d5[0]}, [r12], r3
		vld1.32		{d6}, [r1], r3
		vld1.16		{d7[0]}, [r12], r3
		vld1.32		{d8}, [r1], r3
		vld1.16		{d9[0]}, [r12], r3
		vld1.32		{d10}, [r1], r3
		vld1.16		{d11[0]}, [r12], r3
		vld1.32		{d12}, [r1], r3
		vld1.16		{d13[0]}, [r12], r3
		vld1.32		{d14}, [r1]
		vld1.16		{d15[0]}, [r12]
	ENDIF
	
	IF $Pos = 3
		vld1.32		{q0}, [r1], r3
		vld1.32		{q1}, [r1], r3
		vld1.32		{q2}, [r1], r3
		vld1.32		{q3}, [r1], r3
		vld1.32		{q4}, [r1], r3
		vld1.32		{q5}, [r1], r3
		vld1.32		{q6}, [r1], r3
		vld1.32		{q7}, [r1]
	ENDIF
	
	vld1.32		{d16},  [r0], r2
	vld1.32		{d17},  [r0], r2
	vld1.32		{d18},  [r0], r2
	vld1.32		{d19},  [r0], r2
	vld1.32		{d20},  [r0], r2
	vld1.32		{d21},  [r0], r2
	vld1.32		{d22},  [r0], r2
	vld1.32		{d23},  [r0]
	
	IF $Pos > 0
		vext.8		d0,  d0, d1, #$Pos
		vext.8		d2,  d2, d3, #$Pos
		vext.8		d4,  d4, d5, #$Pos
		vext.8		d6,  d6, d7, #$Pos
		vext.8		d8,  d8, d9, #$Pos
		vext.8		d10, d10, d11, #$Pos
		vext.8		d12, d12, d13, #$Pos
		vext.8		d14, d14, d15, #$Pos
	ENDIF

	vabdl.u8	q0, d0,  d16	
	vabal.u8	q0, d2,  d17	
	vabal.u8	q0, d4,  d18		
	vabal.u8	q0, d6,  d19		
	vabal.u8	q0, d8,  d20		
	vabal.u8	q0, d10, d21		
	vabal.u8	q0, d12, d22		
	vabal.u8	q0, d14, d23			
	vadd.u16	d0, d1
	vpaddl.u16	d0, d0
	vmov		r0, r12, d0
	add			r0, r0, r12

	mov		pc, lr	;return
	
	MEND	;M_sad8_core

	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;uint32_t sad16_c (const uint8_t * const cur, const uint8_t * const ref,
;					const uint32_t cur_stride, const uint32_t ref_stride, const uint32_t best_sad)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	align 8
Sad16x16_ARMV7 PROC
	M_pushRegisters

;R0 cur
;R1 ref
;R2 curPitch
;R3 refPitch

	and		r12, r1, #3
	bic		r1, r1, #3
	ldr		pc, [pc, r12, lsl #2]
	nop
	DCD		sad16_core_0
	DCD		sad16_core_1
	DCD		sad16_core_2
	DCD		sad16_core_3

	ENDP

	M_sad16_core 0 
	M_sad16_core 1
	M_sad16_core 2
	M_sad16_core 3

	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;uint32_t sad8_c (const uint8_t * const cur, const uint8_t * const ref,
;					const uint32_t cur_stride, const uint32_t ref_stride)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	align 8
Sad8x8_ARMV7 PROC

;R0 cur
;R1 ref
;R2 curPitch
;R3 refPitch

	and		r12, r1, #3
	bic		r1, r1, #3
	ldr		pc, [pc, r12, lsl #2]
	nop
	DCD		sad8_core_0
	DCD		sad8_core_1
	DCD		sad8_core_2
	DCD		sad8_core_3

	ENDP

	M_sad8_core 0 
	M_sad8_core 1
	M_sad8_core 2
	M_sad8_core 3		
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;dev16_c(const uint8_t * const cur, const uint32_t stride)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	align 8
Dev16x16_ARMV7 PROC

;R0 cur
;R1 curPitch

	mov			r2, r0
	vld1.32		{q0}, [r2], r1
	vld1.32		{q1}, [r2], r1
	vld1.32		{q2}, [r2], r1
	vld1.32		{q3}, [r2], r1
	vld1.32		{q4}, [r2], r1
	vld1.32		{q5}, [r2], r1
	vaddl.u8	q0, d0, d1
	vld1.32		{q6}, [r2], r1
	vaddl.u8	q1, d2, d3	
	vld1.32		{q7}, [r2], r1
	vadd.u16	q0, q0, q1	
	vld1.32		{q8}, [r2], r1
	vaddl.u8	q1, d4, d5
	vld1.32		{q9}, [r2], r1
	vadd.u16	q0, q0, q1	
	vld1.32		{q10}, [r2], r1
	vaddl.u8	q1, d6, d7	
	vld1.32		{q11}, [r2], r1
	vadd.u16	q0, q0, q1	
	vld1.32		{q12}, [r2], r1
	vaddl.u8	q1, d8, d9
	vld1.32		{q13}, [r2], r1
	vadd.u16	q0, q0, q1	
	vld1.32		{q14}, [r2], r1
	vaddl.u8	q1, d10, d11	
	vld1.32		{q15}, [r2]	
	vadd.u16	q0, q0, q1	
	vaddl.u8	q1, d12, d13
	vadd.u16	q0, q0, q1	
	vaddl.u8	q1, d14, d15	
	vadd.u16	q0, q0, q1	
	vaddl.u8	q1, d16, d17
	vadd.u16	q0, q0, q1	
	vaddl.u8	q1, d18, d19	
	vadd.u16	q0, q0, q1	
	vaddl.u8	q1, d20, d21
	vadd.u16	q0, q0, q1	
	vaddl.u8	q1, d22, d23	
	vadd.u16	q0, q0, q1	
	vaddl.u8	q1, d24, d25
	vadd.u16	q0, q0, q1	
	vaddl.u8	q1, d26, d27	
	vadd.u16	q0, q0, q1	
	vaddl.u8	q1, d28, d29
	vadd.u16	q0, q0, q1	
	vaddl.u8	q1, d30, d31	
	vadd.u16	q0, q0, q1		
	vadd.u16	d0, d0, d1
	vpaddl.u16	d0, d0
	vpaddl.u32	d0, d0
	vshr.u64	d0, #8
	vdup.8		q0, d0[0]		;mean
	
	vabd.u8		q2, q2, q0	
	vabd.u8		q3, q3, q0	
	vabd.u8		q4, q4, q0	
	vabd.u8		q5, q5, q0	
	vabd.u8		q6, q6, q0	
	vabd.u8		q7, q7, q0	
	vabd.u8		q8, q8, q0	
	vabd.u8		q9, q9, q0	
	vabd.u8		q10,q10, q0	
	vabd.u8		q11,q11, q0	
	vabd.u8		q12,q12, q0	
	vabd.u8		q13,q13, q0	
	vabd.u8		q14,q14, q0	
	vabd.u8		q15,q15, q0	
	
	vaddl.u8	q2,  d4, d5
	vaddl.u8	q3,  d6, d7
	vaddl.u8	q4,  d8, d9
	vaddl.u8	q5,  d10, d11
	vaddl.u8	q6,  d12, d13
	vaddl.u8	q7,  d14, d15
	vaddl.u8	q8,  d16, d17
	vaddl.u8	q9,  d18, d19
	vaddl.u8	q10, d20, d21
	vaddl.u8	q11, d22, d23
	vaddl.u8	q12, d24, d25
	vaddl.u8	q13, d26, d27
	vaddl.u8	q14, d28, d29
	vaddl.u8	q15, d30, d31
	
	vadd.u16	q2, q2, q3
	vadd.u16	q2, q2, q4
	vld1.32		{q1}, [r0], r1
	vadd.u16	q2, q2, q5
	vld1.32		{q3}, [r0]
	vadd.u16	q2, q2, q6
	vadd.u16	q2, q2, q7
	vadd.u16	q2, q2, q8
	vadd.u16	q2, q2, q9
	vadd.u16	q2, q2, q10
	vadd.u16	q2, q2, q11
	vadd.u16	q2, q2, q12
	vadd.u16	q2, q2, q13
	vadd.u16	q2, q2, q14
	vadd.u16	q2, q2, q15

	vabd.u8		q1, q1, q0	
	vabd.u8		q3, q3, q0	
	vaddl.u8	q1, d2, d3
	vaddl.u8	q3, d6, d7
	vadd.u16	q2, q2, q1
	vadd.u16	q2, q2, q3		;dev
	vadd.u16	d4, d4, d5
	vpaddl.u16	d4, d4
	vmov		r0, r12, d4
	add			r0, r0, r12
	
	mov		pc, lr	
	ENDP


	END