;*****************************************************************************
;*																			*
;*		VisualOn, Inc. Confidential and Proprietary, 2010					*
;*																			*
;*****************************************************************************  
	AREA	|.text|, CODE
 
	EXPORT sad_8x8_ARMV7
	EXPORT sad_16x16_ARMV7
	EXPORT ssd_8x8_ARMV7
	EXPORT Var2_8x8_ARMV7
 
FENC_STRIDE EQU	 16
FDEC_STRIDE EQU	 32
 
	ALIGN 8

	MACRO
	M_sad8x8  $align
		vld1.64		{d0},  [r1 $align], r3
		vld1.64		{d16}, [r0 @64], r2
		vld1.64		{d2},  [r1 $align], r3
		vld1.64		{d17}, [r0 @64], r2
		vld1.64		{d4},  [r1 $align], r3
		vld1.64		{d18}, [r0 @64], r2
		vld1.64		{d6},  [r1 $align], r3
		vld1.64		{d19}, [r0 @64], r2
		vld1.64		{d8},  [r1 $align], r3
		vld1.64		{d20}, [r0 @64], r2
		vld1.64		{d10}, [r1 $align], r3
		vld1.64		{d21}, [r0 @64], r2
		vld1.64		{d12}, [r1 $align], r3
		vld1.64		{d22}, [r0 @64], r2
		vld1.64		{d14}, [r1 $align]						   
		vld1.64		{d23}, [r0 @64]

		vabdl.u8	q0, d0,  d16	
		vabal.u8	q0, d2,  d17	
		vabal.u8	q0, d4,  d18		
		vabal.u8	q0, d6,  d19		
		vabal.u8	q0, d8,  d20		
		vabal.u8	q0, d10, d21		
		vabal.u8	q0, d12, d22		
		vabal.u8	q0, d14, d23			
		vadd.u16	d2, d0, d1
		vpaddl.u16	d0, d2
		vpadd.u32	d0, d0,  d0
		vmov.u32    r0, d0[0]
		bx          lr

	MEND

	MACRO
	M_sad16x16  $align
		vld1.64		{q0},  [r1 $align], r3	;row 0
		vld1.64		{q1},  [r0 @64], r2
		vld1.64		{q2},  [r1 $align], r3
		vld1.64		{q3},  [r0 @64], r2
		vld1.64		{q4},  [r1 $align], r3
		vld1.64		{q5},  [r0 @64], r2
		vld1.64		{q6},  [r1 $align], r3
		vld1.64		{q7},  [r0 @64], r2
		vld1.64		{q8},  [r1 $align], r3
		vld1.64		{q9},  [r0 @64], r2
		vld1.64		{q10}, [r1 $align], r3
		vld1.64		{q11}, [r0 @64], r2
		vld1.64		{q12}, [r1 $align], r3
		vld1.64		{q13}, [r0 @64], r2
		
		vabdl.u8	q14, d0,  d2	
		vabdl.u8	q15, d1,  d3	
		vabal.u8	q14, d4,  d6	
		vabal.u8	q15, d5,  d7	
		vld1.64		{q0},  [r1 $align], r3	;row 7
		vld1.64		{q1},  [r0 @64], r2
		vld1.64		{q2},  [r1 $align], r3
		vld1.64		{q3},  [r0 @64], r2
		vabal.u8	q14, d8,  d10	
		vabal.u8	q15, d9,  d11	
		vabal.u8	q14, d12,  d14	
		vabal.u8	q15, d13,  d15	
		vld1.64		{q4},  [r1 $align], r3
		vld1.64		{q5},  [r0 @64], r2
		vld1.64		{q6},  [r1 $align], r3
		vld1.64		{q7},  [r0 @64], r2
		vabal.u8	q14, d16,  d18	
		vabal.u8	q15, d17,  d19	
		vabal.u8	q14, d20,  d22	
		vabal.u8	q15, d21,  d23	
		vld1.64		{q8},  [r1 $align], r3
		vld1.64		{q9},  [r0 @64], r2
		vld1.64		{q10}, [r1 $align], r3
		vld1.64		{q11}, [r0 @64], r2
		vabal.u8	q14, d24,  d26	
		vabal.u8	q15, d25,  d27	
		
		vabal.u8	q14, d0,  d2	
		vabal.u8	q15, d1,  d3	
		vld1.64		{q12}, [r1 $align], r3
		vld1.64		{q13}, [r0 @64], r2
		vld1.64		{q0},  [r1 $align], r3
		vld1.64		{q1},  [r0 @64], r2
		vabal.u8	q14, d4,  d6	
		vabal.u8	q15, d5,  d7	
		vabal.u8	q14, d8,  d10	
		vabal.u8	q15, d9,  d11	
		vld1.64		{q2},  [r1 $align]		;row 15
		vld1.64		{q3},  [r0 @64]
		vabal.u8	q14, d12, d14	
		vabal.u8	q15, d13, d15	
		vabal.u8	q14, d16, d18	
		vabal.u8	q15, d17, d19	
		vabal.u8	q14, d20, d22	
		vabal.u8	q15, d21, d23	
		vabal.u8	q14, d24, d26	
		vabal.u8	q15, d25, d27					
		vabal.u8	q14, d0,  d2	
		vabal.u8	q15, d1,  d3	
		vabal.u8	q14, d4,  d6	
		vabal.u8	q15, d5,  d7	
		vadd.u16	q0, q14, q15

		vadd.u16	d0, d0,  d1
		vpaddl.u16	d0, d0
		vpadd.u32	d0, d0,  d0
		vmov.u32    r0, d0[0]
		bx          lr

	MEND
   
   
sad_8x8_ARMV7 
	M_sad8x8
	   
sad_16x16_ARMV7 
	M_sad16x16
	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ssd_8x8_ARMV7

	vld1.64		{d0},  [r0 @64], r2
	vld1.64		{d1},  [r1 @64], r3
	vld1.64		{d2},  [r0 @64], r2
	vld1.64		{d3},  [r1 @64], r3
	vld1.64		{d4},  [r0 @64], r2
	vld1.64		{d5},  [r1 @64], r3
	vld1.64		{d6},  [r0 @64], r2
	vld1.64		{d7},  [r1 @64], r3
	vld1.64		{d8},  [r0 @64], r2

    vsubl.u8    q8 , d0 , d1
    vsubl.u8    q9 , d2 , d3   
    vmull.s16   q12, d16, d16
	vld1.64		{d9},  [r1 @64], r3
    vmull.s16   q13, d17, d17
	vld1.64		{d10}, [r0 @64], r2
    vmull.s16   q14, d18, d18
	vld1.64		{d11}, [r1 @64], r3
    vmull.s16   q15, d19, d19
    vsubl.u8    q10, d4 , d5
    vsubl.u8    q11, d6 , d7
    vmlal.s16   q12, d20, d20
	vld1.64		{d12}, [r0 @64], r2
    vmlal.s16   q13, d21, d21
	vld1.64		{d13}, [r1 @64], r3
    vmlal.s16   q14, d22, d22
	vld1.64		{d14}, [r0 @64]
    vmlal.s16   q15, d23, d23
	vld1.64		{d15}, [r1 @64]		

    vsubl.u8    q8 , d8 , d9
    vsubl.u8    q9 , d10, d11   
    vmlal.s16   q12, d16, d16
    vmlal.s16   q13, d17, d17
    vmlal.s16   q14, d18, d18
    vsubl.u8    q10, d12, d13
    vmlal.s16   q15, d19, d19
    vsubl.u8    q11, d14, d15
    vmlal.s16   q12, d20, d20
    vmlal.s16   q13, d21, d21
    vmlal.s16   q14, d22, d22
    vmlal.s16   q15, d23, d23
    
	vadd.s32    q12, q12, q13
	vadd.s32    q14, q14, q15
	vadd.s32    q0 , q12, q14
	
	vadd.s32    d0, d0, d1
	vpadd.s32   d0, d0, d0
	vmov.32     r0, d0[0]
	bx          lr 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Var2_8x8_ARMV7

    ldr         r12,   [sp]
	vld1.64		{d0},  [r0 @64], r2
	vld1.64		{d1},  [r1 @64], r3
	vld1.64		{d2},  [r0 @64], r2
	vld1.64		{d3},  [r1 @64], r3
	vld1.64		{d4},  [r0 @64], r2
	vld1.64		{d5},  [r1 @64], r3
	vld1.64		{d6},  [r0 @64], r2
	vld1.64		{d7},  [r1 @64], r3
	vld1.64		{d8},  [r0 @64], r2
	vld1.64		{d9},  [r1 @64], r3
	vld1.64		{d10}, [r0 @64], r2
	vld1.64		{d11}, [r1 @64], r3
	vld1.64		{d12}, [r0 @64], r2
	vld1.64		{d13}, [r1 @64], r3

    vsubl.u8    q8 , d0 , d1
    vsubl.u8    q9 , d2 , d3   
    vmull.s16   q12, d16, d16
    vadd.s16	q7 , q8 , q9
    vmull.s16   q13, d17, d17
	vld1.64		{d0}, [r0 @64]
    vmull.s16   q14, d18, d18
	vld1.64		{d1}, [r1 @64]		
    vmull.s16   q15, d19, d19
    vsubl.u8    q10, d4 , d5
    vsubl.u8    q11, d6 , d7
    vmlal.s16   q12, d20, d20
    vadd.s16	q7 , q7 , q10
    vmlal.s16   q13, d21, d21
    vadd.s16	q7 , q7 , q11
    vmlal.s16   q14, d22, d22
    vsubl.u8    q8 , d8 , d9
    vmlal.s16   q15, d23, d23   

    vsubl.u8    q9 , d10, d11   
    vmlal.s16   q12, d16, d16
    vadd.s16	q7 , q7 , q8
    vmlal.s16   q13, d17, d17
    vadd.s16	q7 , q7 , q9
    vmlal.s16   q14, d18, d18
    vsubl.u8    q10, d12, d13
    vmlal.s16   q15, d19, d19
    vsubl.u8    q11, d0 , d1
    vmlal.s16   q12, d20, d20
    vadd.s16	q7 , q7 , q10
    vmlal.s16   q13, d21, d21
    vadd.s16	q7 , q7 , q11
    vmlal.s16   q14, d22, d22
	vadd.s16    d14, d14, d15
    vmlal.s16   q15, d23, d23
    
 	vpaddl.s16  d14, d14
	vadd.s32    q12, q12, q13
 	vpaddl.s32  d14, d14
	vadd.s32    q14, q14, q15
 	vabs.s32	d14, d14
	vmov.32     r1 , d14[0]
	vadd.s32    q0 , q12, q14
	mul			r2, r1, r1   	
	vadd.s32    d0, d0, d1
	vpadd.s32   d0, d0, d0
	vmov.32     r3, d0[0]
	str			r3, [r12]
	
	sub			r0, r3, r2, lsr #6	
	bx          lr 


    end

