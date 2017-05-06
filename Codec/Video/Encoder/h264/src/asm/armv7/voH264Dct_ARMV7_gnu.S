@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************

#include "../../voH264EncID.h"
    @AREA	|.text|, CODE
    
    
    .section .text

.global				Sub4x4Dct_ARMV7
.global				Sub8x8Dct_ARMV7
.global				Sub16x16Dct_ARMV7
.global				Add4x4Idct_ARMV7
.global				Add8x8Idct_ARMV7
.global				Add16x16Idct_ARMV7
.global				Dct4x4DC_ARMV7
.global				Idct4x4Dc_ARMV7	
.global				Sub8x8DctDc_ARMV7
.global				Add8x8IdctDc_ARMV7
.global			  Add16x16IdctDc_ARMV7
.global				Zigzag4x4_ARMV7

 .equ	 FENC_STRIDE, 16
 .equ  FDEC_STRIDE, 32

 .align 8

 
	.macro M_DCT_1D d0, d1, d2, d3, d4, d5, d6, d7
		vadd.s16		\d1, \d5, \d6
		vadd.s16		\d3, \d4, \d7
		vsub.s16		\d6, \d5, \d6
		vsub.s16		\d7, \d4, \d7
		vadd.s16        \d0, \d3, \d1
		vadd.s16        \d4, \d7, \d7
		vsub.s16        \d2, \d3, \d1
		vadd.s16        \d5, \d6, \d6
		vadd.s16        \d1, \d4, \d6
		vsub.s16        \d3, \d7, \d5
	.endm
	
	.macro M_IDCT_1D d4, d5, d6, d7, d0, d1, d2, d3
		vadd.s16		\d4, \d0, \d2
		vsub.s16		\d5, \d0, \d2
		vshr.s16        \d7, \d1, #1
		vshr.s16        \d6, \d3, #1
		vsub.s16        \d7, \d7, \d3
		vadd.s16        \d6, \d6, \d1
	.endm
		
	.macro M_TRANSPOSE4x4_16BIT  d0, d1, d2, d3
		vtrn.32     \d0, \d2
		vtrn.32     \d1, \d3
		vtrn.16     \d0, \d1
		vtrn.16     \d2, \d3
	.endm
	
	.macro M_DCT16x4  Rx0, Rx1
		vld1.64         {q0}, [r1 ,:64], ip
		vld1.64         {q1}, [r2 ,:64], r3
		vld1.64         {q2}, [r1 ,:64], ip
		vld1.64         {q3}, [r2 ,:64], r3
		vld1.64         {q8} , [r1 ,:64], ip
		vld1.64         {q9} , [r2 ,:64], r3
		vld1.64         {q10}, [r1 ,:64], ip
		vld1.64         {q11}, [r2 ,:64], r3
	    
		vsubl.u8        q4, d0,  d2
		vsubl.u8        q5, d4,  d6
		vsubl.u8        q6, d16, d18
		vsubl.u8        q7, d20, d22
		M_DCT_1D        q12, q13, q14, q15, q4, q5, q6, q7
		M_TRANSPOSE4x4_16BIT q12, q13, q14, q15
		vadd.s16		q4,  q12, q15
		vadd.s16		q5,  q13, q14
		vsub.s16		q6 , q12, q15
		vsub.s16		q7 , q13, q14
		vadd.i16        q14, q6 , q6 
		vadd.i16        q15, q7 , q7 
		vadd.i16        d24, d8 , d10
		vadd.i16        d25, d28, d14
		vsub.i16        d26, d8 , d10
		vst1.64         {d24-d25}, [\Rx0 ,:64]!
		vsub.i16        d27, d12, d30
		vst1.64         {d26-d27}, [\Rx0 ,:64]!
		vadd.i16        d24, d9 , d11
		vadd.i16        d25, d29, d15
		vsub.i16        d26, d9 , d11
		vsub.i16        d27, d13, d31
		vst1.64         {d24-d25}, [\Rx0 ,:64]!
		vsubl.u8        q4, d1,  d3
		vsubl.u8        q5, d5,  d7
		vst1.64         {d26-d27}, [\Rx0 ,:64]!
	    
		vsubl.u8        q6, d17, d19
		vsubl.u8        q7, d21, d23
		M_DCT_1D        q12, q13, q14, q15, q4, q5, q6, q7
		M_TRANSPOSE4x4_16BIT q12, q13, q14, q15
		vadd.s16		q4,  q12, q15
		vadd.s16		q5,  q13, q14
		vsub.s16		q6 , q12, q15
		vsub.s16		q7 , q13, q14
		vadd.i16        q14, q6 , q6 
		vadd.i16        q15, q7 , q7 
		vadd.i16        d24, d8 , d10
		vadd.i16        d25, d28, d14
		vsub.i16        d26, d8 , d10
		vst1.64         {d24-d25}, [\Rx1 ,:64]!
		vsub.i16        d27, d12, d30
		vst1.64         {d26-d27}, [\Rx1 ,:64]!
		vadd.i16        d24, d9 , d11
		vadd.i16        d25, d29, d15
		vsub.i16        d26, d9 , d11
		vst1.64         {d24-d25}, [\Rx1 ,:64]!
		vsub.i16        d27, d13, d31
		vst1.64         {d26-d27}, [\Rx1 ,:64]!
	.endm

	.macro 	M_ADD_IDCT8x8  Rx0, Rx1
		vld1.64         {d0-d3}, [r1 ,:64]!
		vld1.64         {d4-d7}, [r1 ,:64]!
		M_IDCT_1D       d8, d10, d12, d14, d0, d1, d2, d3
		M_IDCT_1D       d9, d11, d13, d15, d4, d5, d6, d7
		vadd.s16		q0, q4,  q6
		vsub.s16		q3, q4,  q6
		vadd.s16		q1, q5,  q7
		vsub.s16		q2, q5,  q7
		M_TRANSPOSE4x4_16BIT q0,  q1,  q2,  q3
		M_IDCT_1D       q4, q5,  q6, q7, q0, q1, q2, q3  
		vld1.32         {d20}, [\Rx0 ,:64], r2
		vadd.s16		q0, q4,  q6
		vsub.s16		q3, q4,  q6
		vld1.32         {d21}, [\Rx0 ,:64], r2
		vadd.s16		q1, q5,  q7
		vsub.s16		q2, q5,  q7
		vld1.32         {d22}, [\Rx0 ,:64], r2
		vrshr.s16       q0,  q0,  #6
		vld1.32         {d23}, [\Rx0 ,:64], r2
		vrshr.s16       q1,  q1,  #6
		vrshr.s16       q2,  q2,  #6
		vrshr.s16       q3,  q3,  #6
		vaddw.u8        q0,  q0,  d20
		vaddw.u8        q1,  q1,  d21
		vaddw.u8        q2,  q2,  d22
		vaddw.u8        q3,  q3,  d23
		vqmovun.s16     d0,  q0
		vqmovun.s16     d1,  q1
		vst1.32         {d0}, [\Rx1 ,:64], r2
		vqmovun.s16     d2,  q2
		vst1.32         {d1}, [\Rx1 ,:64], r2
		vqmovun.s16     d3,  q3
		vst1.32         {d2}, [\Rx1 ,:64], r2
		vst1.32         {d3}, [\Rx1 ,:64], r2
	    
		vld1.64         {d0-d3}, [r1 ,:64]!
		vld1.64         {d4-d7}, [r1 ,:64]!
		M_IDCT_1D       d8 , d10, d12, d14, d0, d1, d2, d3
		M_IDCT_1D       d9 , d11, d13, d15, d4, d5, d6, d7
		vadd.s16		q0, q4,  q6
		vsub.s16		q3, q4,  q6
		vadd.s16		q1, q5,  q7
		vsub.s16		q2, q5,  q7
		M_TRANSPOSE4x4_16BIT q0,  q1,  q2,  q3
		M_IDCT_1D       q4, q5,  q6, q7, q0, q1, q2, q3  
		vld1.32         {d20}, [\Rx0 ,:64], r2
		vadd.s16		q0, q4,  q6
		vsub.s16		q3, q4,  q6
		vld1.32         {d21}, [\Rx0 ,:64], r2
		vadd.s16		q1, q5,  q7
		vsub.s16		q2, q5,  q7
		vld1.32         {d22}, [\Rx0 ,:64], r2
		vrshr.s16       q0,  q0,  #6
		vld1.32         {d23}, [\Rx0 ,:64], r2
		vrshr.s16       q1,  q1,  #6
		vrshr.s16       q2,  q2,  #6
		vrshr.s16       q3,  q3,  #6
		vaddw.u8        q0,  q0,  d20
		vaddw.u8        q1,  q1,  d21
		vaddw.u8        q2,  q2,  d22
		vaddw.u8        q3,  q3,  d23
		vqmovun.s16     d0,  q0
		vqmovun.s16     d1,  q1
		vst1.32         {d0}, [\Rx1 ,:64], r2
		vqmovun.s16     d2,  q2
		vst1.32         {d1}, [\Rx1 ,:64], r2
		vqmovun.s16     d3,  q3
		vst1.32         {d2}, [\Rx1 ,:64], r2
		vst1.32         {d3}, [\Rx1 ,:64], r2
	.endm
	
	.macro ADD_IDCT16x4_DC  dx
		vld1.64         {d16-d17}, [r0 ,:64], r3
		vdup.16         d4,  \dx[0]
		vld1.64         {d18-d19}, [r0 ,:64], r3
		vdup.16         d5,  \dx[1]
		vld1.64         {d20-d21}, [r0 ,:64], r3
		vdup.16         d6,  \dx[2]
		vdup.16         d7,  \dx[3]
		vld1.64         {d22-d23}, [r0 ,:64], r3
		vsub.s16        q12, q14, q2
		vsub.s16        q13, q14, q3
		vqmovun.s16     d4,  q2
		vqmovun.s16     d5,  q3
		vqmovun.s16     d6,  q12
		vqmovun.s16     d7,  q13
		vqadd.u8        q8,  q8,  q2
		vqadd.u8        q9,  q9,  q2
		vqadd.u8        q10, q10, q2
		vqsub.u8        q8,  q8,  q3
		vqadd.u8        q11, q11, q2
		vqsub.u8        q9,  q9,  q3
		vst1.64         {d16-d17}, [r2 ,:64], r3
		vqsub.u8        q10, q10, q3
		vst1.64         {d18-d19}, [r2 ,:64], r3
		vqsub.u8        q11, q11, q3
		vst1.64         {d20-d21}, [r2 ,:64], r3
		vst1.64         {d22-d23}, [r2 ,:64], r3
	.endm
	
@****************************************************************

Sub4x4Dct_ARMV7:
    mov             ip, #FENC_STRIDE
    vld1.32         {d0[]}, [r1 ,:32], ip
    vld1.32         {d1[]}, [r2 ,:32], r3
    vld1.32         {d2[]}, [r1 ,:32], ip
    vld1.32         {d3[]}, [r2 ,:32], r3
    vld1.32         {d4[]}, [r1 ,:32], ip
    vld1.32         {d5[]}, [r2 ,:32], r3
    vld1.32         {d6[]}, [r1 ,:32], ip
    vld1.32         {d7[]}, [r2 ,:32], r3
    
    vsubl.u8        q4, d0,  d1
    vsubl.u8        q5, d2,  d3
    vsubl.u8        q6, d4,  d5
    vsubl.u8        q7, d6,  d7

    M_DCT_1D				d0, d1, d2, d3, d8, d10, d12, d14
    M_TRANSPOSE4x4_16BIT    d0, d1, d2, d3
    M_DCT_1D				d4, d5, d6, d7, d0, d1, d2, d3
    vst1.64         {d4-d7}, [r0 ,:64]
    bx              lr
    
@****************************************************************    
    
Sub8x8Dct_ARMV7:
    stmdb	        sp!, {lr}
    mov             ip, #FENC_STRIDE
    
    vld1.64         {d0}, [r1 ,:64], ip
    vld1.64         {d1}, [r2 ,:64], r3
    vld1.64         {d2}, [r1 ,:64], ip
    vld1.64         {d3}, [r2 ,:64], r3
    vld1.64         {d4}, [r1 ,:64], ip
    vld1.64         {d5}, [r2 ,:64], r3
    vld1.64         {d6}, [r1 ,:64], ip
    vld1.64         {d7}, [r2 ,:64], r3
    vsubl.u8        q4, d0,  d1
    vsubl.u8        q5, d2,  d3
    vsubl.u8        q6, d4,  d5
    vsubl.u8        q7, d6,  d7

    M_DCT_1D        q0, q1, q2, q3, q4, q5, q6, q7
    M_TRANSPOSE4x4_16BIT q0, q1, q2, q3

	vadd.s16		q4,  q0, q3
	vadd.s16		q5,  q1, q2
	vsub.s16		q6 , q0, q3
	vsub.s16		q7 , q1, q2
    vadd.i16        q14, q6 , q6 
    vadd.i16        q15, q7 , q7 
    vadd.i16        d0,  d8 , d10
    vadd.i16        d1,  d28, d14
    vsub.i16        d2,  d8 , d10
    vsub.i16        d3,  d12, d30
    vst1.64         {d0-d1}, [r0 ,:64]!
    vadd.i16        d4,  d9 , d11
    vadd.i16        d5,  d29, d15
    vst1.64         {d2-d3}, [r0 ,:64]!
    vsub.i16        d6,  d9 , d11
    vsub.i16        d7,  d13, d31
    vst1.64         {d4-d5}, [r0 ,:64]!
    vst1.64         {d6-d7}, [r0 ,:64]!
    
    vld1.64         {d0}, [r1 ,:64], ip
    vld1.64         {d1}, [r2 ,:64], r3
    vld1.64         {d2}, [r1 ,:64], ip
    vld1.64         {d3}, [r2 ,:64], r3
    vld1.64         {d4}, [r1 ,:64], ip
    vld1.64         {d5}, [r2 ,:64], r3
    vld1.64         {d6}, [r1 ,:64], ip
    vld1.64         {d7}, [r2 ,:64], r3
    vsubl.u8        q4, d0,  d1
    vsubl.u8        q5, d2,  d3
    vsubl.u8        q6, d4,  d5
    vsubl.u8        q7, d6,  d7

    M_DCT_1D        q0, q1, q2, q3, q4, q5, q6, q7
    M_TRANSPOSE4x4_16BIT q0, q1, q2, q3

	vadd.s16		q4,  q0, q3
	vadd.s16		q5,  q1, q2
	vsub.s16		q6 , q0, q3
	vsub.s16		q7 , q1, q2
    vadd.i16        q14, q6 , q6 
    vadd.i16        q15, q7 , q7 
    vadd.i16        d0,  d8 , d10
    vadd.i16        d1,  d28, d14
    vsub.i16        d2,  d8 , d10
    vsub.i16        d3,  d12, d30
    vst1.64         {d0-d1}, [r0 ,:64]!
    vadd.i16        d4,  d9 , d11
    vadd.i16        d5,  d29, d15
    vst1.64         {d2-d3}, [r0 ,:64]!
    vsub.i16        d6,  d9 , d11
    vsub.i16        d7,  d13, d31
    vst1.64         {d4-d5}, [r0 ,:64]!
    vst1.64         {d6-d7}, [r0 ,:64]!
    ldmia	        sp!, {pc}

@****************************************************************

Sub16x16Dct_ARMV7:

    stmdb	        sp!, {r4-r5, lr}
    mov             ip, #FENC_STRIDE
    add				r4, r0, #128
    add				r5, r4, #128
    add				lr, r5, #128
    
	M_DCT16x4		r0, r4
	M_DCT16x4		r0, r4
    mov				r0, r5
    mov				r4, lr
	M_DCT16x4		r0, r4
	M_DCT16x4		r0, r4

    ldmia	        sp!, {r4-r5, pc}

@****************************************************************

Add4x4Idct_ARMV7 :
    mov             r3, #FDEC_STRIDE
    mov             r2, r0
    
    vld1.64         {d0-d3}, [r1 ,:64]
    M_IDCT_1D       d4, d5, d6, d7, d0, d1, d2, d3
    
    vld1.32         {d8[0]}, [r0 ,:32], r3
    vadd.s16		q0, q2, q3
    vld1.32         {d8[1]}, [r0 ,:32], r3
    vsub.s16		q1, q2, q3

    M_TRANSPOSE4x4_16BIT d0, d1, d3, d2
    
    vld1.32         {d9[1]}, [r0 ,:32], r3
    M_IDCT_1D       d4, d5, d6, d7, d0, d1, d3, d2
    vld1.32         {d9[0]}, [r0 ,:32], r3
    
    vadd.s16		q0, q2, q3
    vsub.s16		q1, q2, q3
    vrshr.s16       q0, q0, #6
    vrshr.s16       q1, q1, #6
    vaddw.u8        q0, q0, d8
    vaddw.u8        q1, q1, d9
    vqmovun.s16     d0, q0
    vqmovun.s16     d2, q1

    vst1.32         {d0[0]}, [r2 ,:32], r3
    vst1.32         {d0[1]}, [r2 ,:32], r3
    vst1.32         {d2[1]}, [r2 ,:32], r3
    vst1.32         {d2[0]}, [r2 ,:32], r3
    bx              lr
    
@****************************************************************

Add8x8Idct_ARMV7:
    mov             r2, #FDEC_STRIDE
    mov				r3, r0
	M_ADD_IDCT8x8	r0, r3
    bx				lr

@****************************************************************

Add16x16Idct_ARMV7:
    stmdb	        sp!, {r4-r7, lr}
    mov             r2, #FDEC_STRIDE
    mov				r3, r0
	add				r4, r0, #8
	add				r5, r0, #8
	add				r6, r0, #8*FDEC_STRIDE
	add				r7, r0, #8*FDEC_STRIDE
	add				r12, r0, #8*FDEC_STRIDE+8
	add				lr , r0, #8*FDEC_STRIDE+8
	
	M_ADD_IDCT8x8	r0, r3
	M_ADD_IDCT8x8	r4, r5
	M_ADD_IDCT8x8	r6, r7
	M_ADD_IDCT8x8	r12, lr
	
    ldmia	        sp!, {r4-r7, pc}
@****************************************************************

Dct4x4DC_ARMV7  :

    vld1.64         {d0-d3}, [r0 ,:64]
    vmov.s16        d31, #1
    vadd.s16        d4, d0, d1
    vsub.s16        d5, d0, d1
    vadd.s16        d6, d2, d3
    vsub.s16        d7, d2, d3
    vadd.s16        d0, d4, d6
    vsub.s16        d2, d4, d6
    vadd.s16        d3, d5, d7
    vsub.s16        d1, d5, d7
    vtrn.16			q0, q1
    vadd.s16        q2, q0, q1
    vsub.s16        q3, q0, q1
    vtrn.32         d4,  d5
    vtrn.32         d6,  d7
    vadd.s16        d16, d4,  d31
    vadd.s16        d17, d6,  d31
    vrhadd.s16      d0,  d4,  d5
    vhsub.s16       d1,  d16, d5
    vhsub.s16       d2,  d17, d7
    vrhadd.s16      d3,  d6,  d7
    vst1.64         {d0-d3}, [r0 ,:64]
    bx              lr

@****************************************************************

Idct4x4Dc_ARMV7:

    vld1.64         {d0-d3}, [r0 ,:64]
    vadd.s16        d4, d0, d1
    vsub.s16        d5, d0, d1
    vadd.s16        d6, d2, d3
    vsub.s16        d7, d2, d3
    vadd.s16        d0, d4, d6
    vsub.s16        d2, d4, d6
    vadd.s16        d3, d5, d7
    vsub.s16        d1, d5, d7
    vtrn.16			q0, q1
    vadd.s16        q2, q0, q1
    vsub.s16        q3, q0, q1
    vtrn.32			d4, d5
    vtrn.32			d6, d7
    vadd.s16        d0, d4, d5
    vsub.s16        d1, d4, d5
    vadd.s16        d3, d6, d7
    vsub.s16        d2, d6, d7
    vst1.64         {d0-d3}, [r0 ,:64]
    bx              lr


@****************************************************************

Sub8x8DctDc_ARMV7   :

    mov             ip,  #FENC_STRIDE
    mov		  r3,  #FDEC_STRIDE
    vld1.64         {d16}, [r1 ,:64], ip
    vld1.64         {d17}, [r2 ,:64], r3
    vld1.64         {d18}, [r1 ,:64], ip
    vld1.64         {d19}, [r2 ,:64], r3
    vld1.64         {d20}, [r1 ,:64], ip
    vld1.64         {d21}, [r2 ,:64], r3
    vld1.64         {d22}, [r1 ,:64], ip
    vld1.64         {d23}, [r2 ,:64], r3
    vld1.64         {d24}, [r1 ,:64], ip
    vld1.64         {d25}, [r2 ,:64], r3
    vld1.64         {d26}, [r1 ,:64], ip
    vld1.64         {d27}, [r2 ,:64], r3
    vld1.64         {d28}, [r1 ,:64], ip
    vld1.64         {d29}, [r2 ,:64], r3
    vld1.64         {d30}, [r1 ,:64], ip
    vld1.64         {d31}, [r2 ,:64], r3
    vsubl.u8        q8,  d16, d17
    vsubl.u8        q9,  d18, d19
    vsubl.u8        q10, d20, d21
    vadd.s16        q0,  q8,  q9
    vsubl.u8        q11, d22, d23
    vadd.s16        q0,  q0,  q10
    vsubl.u8        q12, d24, d25
    vadd.s16        q0,  q0,  q11
    vsubl.u8        q13, d26, d27
    vsubl.u8        q14, d28, d29
    vadd.s16        q1,  q12, q13
    vpadd.s16       d0,  d0,  d1
    vadd.s16        q1,  q1,  q14
    vsubl.u8        q15, d30, d31
    vadd.s16        q1,  q1,  q15
    vpadd.s16       d2,  d2,  d3
    vpadd.s16       d0,  d0,  d2
    vst1.64         {d0}, [r0 ,:64]
    bx              lr

@****************************************************************

Add8x8IdctDc_ARMV7:

    mov             r2,  #FDEC_STRIDE
    mov             r12,  r0
    vld1.64         {d16}, [r1 ,:64]
    vld1.64         {d0}, [r0 ,:64], r2
    vmov.i16        q15, #0
    vld1.64         {d1}, [r0 ,:64], r2
    vld1.64         {d2}, [r0 ,:64], r2
    vld1.64         {d3}, [r0 ,:64], r2
    vld1.64         {d4}, [r0 ,:64], r2
    vld1.64         {d5}, [r0 ,:64], r2
    vld1.64         {d6}, [r0 ,:64], r2
    vrshr.s16       d16, d16, #6
    vld1.64         {d7}, [r0 ,:64]
    vdup.16         d20, d16[0]
    vdup.16         d21, d16[1]
    vdup.16         d22, d16[2]
    vdup.16         d23, d16[3]
    vsub.s16        q12, q15, q10
    vsub.s16        q13, q15, q11

    vqmovun.s16     d20, q10
    vqmovun.s16     d22, q11
    vqmovun.s16     d24, q12
    vqmovun.s16     d26, q13

    vshr.s32            d21, d20, #0
    vshr.s32            d23, d22, #0
    vqadd.u8        q0,  q0,  q10
    vqadd.u8        q1,  q1,  q10
    vshr.s32            d25, d24, #0
    vqadd.u8        q2,  q2,  q11
    vqadd.u8        q3,  q3,  q11
    vqsub.u8        q0,  q0,  q12
    vqsub.u8        q1,  q1,  q12

    vst1.64         {d0}, [r12 ,:64], r2
    vshr.s32        d27, d26, #0
    vst1.64         {d1}, [r12 ,:64], r2
    vqsub.u8        q2,  q2,  q13
    vst1.64         {d2}, [r12 ,:64], r2
    vqsub.u8        q3,  q3,  q13
    vst1.64         {d3}, [r12 ,:64], r2
    vst1.64         {d4}, [r12 ,:64], r2
    vst1.64         {d5}, [r12 ,:64], r2
    vst1.64         {d6}, [r12 ,:64], r2
    vst1.64         {d7}, [r12 ,:64]
    bx              lr

@****************************************************************

Add16x16IdctDc_ARMV7:

    vld1.64         {d0-d3}, [r1 ,:64]
    mov             r2,  r0
    mov             r3,  #FDEC_STRIDE
    vmov.i16        q14, #0
    vrshr.s16       q0, #6
    vrshr.s16       q1, #6

    ADD_IDCT16x4_DC d0
    ADD_IDCT16x4_DC d1
    ADD_IDCT16x4_DC d2
    ADD_IDCT16x4_DC d3
    bx              lr	
    
@****************************************************************

zigzag_table:
	.byte    0,1,   8,9,   2,3,   4,5
	.byte		 2,3,   8,9,   16,17, 10,11
	.byte 	 12,13, 6,7,   14,15, 20,21
	.byte		 10,11, 12,13, 6,7,   14,15
	
	
Zigzag4x4_ARMV7:
    ldr          r2, =zigzag_table
    vld1.64     {d0-d3}, [r1]
    vld1.64     {d4-d7}, [r2]
    vtbl.8      d16, {d0-d1}, d4
    vtbl.8      d17, {d1-d3}, d5
    vtbl.8      d18, {d0-d2}, d6
    vtbl.8      d19, {d2-d3}, d7
    vst1.64     {d16-d19}, [r0]
    bx          lr

    
    .end
