@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************

#include "../../voH264EncID.h"
    @AREA	|.text|, CODE
	    
	.section .text

.global		HpelFilter16X16_V_ARMV7
.global		HpelFilter16X16_H_ARMV7
.global		HpelFilter16X16_C_ARMV7
.global  	Hpel16x16SearchFilter_HV_ARMV7
	
	.align 8
	 
	.macro HALF16X16_V_ROW q, r0, r1, sd, d0, d1, d2, d3,  d4, d5, d6, d7, d8, d9, d10, d11
					vld1.64		{\q},  [\r0], r3		
					vaddl.u8	q10,  \d0,  \d1		
					vaddl.u8	q11,  \d2,  \d3			
					vaddl.u8	q6,  \d4,  \d5		
					vaddl.u8	q7,  \d6,  \d7		
					vaddl.u8	q8,  \d8,  \d9		
					vaddl.u8	q9,  \d10,  \d11	
					vmla.s16    q6,  q10,  q15  	
					vmla.s16    q7,  q11,  q15		
					vmls.s16    q6,  q8,  q14		
					vmls.s16    q7,  q9,  q14		
					vqrshrun.s16 	d24,  q6,  #5	
					vqrshrun.s16 	d25,  q7,  #5	
					vst1.64     {q12}, [\r1, :64],  \sd
	.endm
	
	.macro HALF16X16_C_ROW q0, r0, q1, r1, d0, d1, d2, d3,  d4, d5, d6, d7, d8, d9, d10, d11
					vld1.64		{\q0},  [\r0], r3		
					vaddl.u8	q10,  \d0,  \d1		
					vaddl.u8	q11,  \d2,  \d3			
					vaddl.u8	q6,  \d4,  \d5		
					vaddl.u8	q7,  \d6,  \d7		
					vaddl.u8	q8,  \d8,  \d9		
					vaddl.u8	q9,  \d10,  \d11			
					vmla.s16    q6,  q10,  q15  	
					vmla.s16    q7,  q11,  q15		
					vmls.s16    q6,  q8,  q14		
					vmls.s16    q7,  q9,  q14		
					vext.8		q8,  q6,  q7,  #2	
					vext.8		q9,  q6,  q7,  #4	
					vext.8		q10,  q6,  q7,  #6	 
					vext.8		q11,  q6,  q7,  #8		
					vext.8		q12,  q6,  q7,  #10	
					vaddl.s16	q7,  d12,  d24		
					vaddl.s16	\q1,  d13,  d25		
					vaddl.s16	q6,  d16,  d22		
					vaddl.s16	q12,  d17,  d23		
					vaddl.s16	q8,  d18,  d20		
					vaddl.s16	q11,  d19,  d21		
					vmov.i32    q9,  #5				
					vmov.i32    q10,  #20			
					vmla.s32    q7,  q8,  q10  		
					vmla.s32    \q1,  q11,  q10		
					vmls.s32    q7,  q6,  q9		
					vmls.s32    \q1,  q12,  q9		
					vqrshrun.s32 	d20,  q7,  #10	
					vqrshrun.s32 	d21,  \q1,  #10	
					vqmovn		d24.u8,  q10.u16	
					vst1.64     {d24}, [\r1, :64],  r1
	.endm
	
	.macro HALF16X16_H_ROW r, sd
		vld1.64		{q0},  [r2], r3		
		vld1.64		{q13},  [r12], r3
		vext.8		q1,  q0,  q13,  #1	
		vext.8		q2,  q0,  q13,  #2	
		vext.8		q3,  q0,  q13,  #3	
		vext.8		q4,  q0,  q13,  #4	
		vext.8		q5,  q0,  q13,  #5		
		vaddl.u8	q10,  d4,  d6		
		vaddl.u8	q11,  d5,  d7		
		vaddl.u8	q6,  d0,  d10		
		vaddl.u8	q7,  d1,  d11		
		vaddl.u8	q8,  d2,  d8		
		vaddl.u8	q9,  d3,  d9		
		vmla.s16    q6,  q10,  q15  	
		vmla.s16    q7,  q11,  q15		
		vmls.s16    q6,  q8,  q14		
		vmls.s16    q7,  q9,  q14		
		vqrshrun.s16 	d24,  q6,  #5	
		vqrshrun.s16 	d25,  q7,  #5	
		vst1.64     {q12}, [\r, :64],  \sd
	.endm
	
	.macro HALF16X16_HV_EXTRAH r, sd
		vext.8		d4,  d26,  d27,  #1	
		vext.8		d6,  d26,  d27,  #2	
		vext.8		d8,  d26,  d27,  #3	
		vext.8		d10,  d26,  d27,  #4
		vext.8		d12,  d26,  d27,  #5
		vaddl.u8	q10,  d6,  d8		
		vaddl.u8	q7,  d26,  d12		
		vaddl.u8	q8,  d4,  d10		
		vmla.s16    q7,  q10,  q15  	
		vmls.s16    q7,  q8,  q14		
		vqrshrun.s16 	d24,  q7,  #5	
		vst1.64     {d24}, [\r, :64],  \sd
	.endm
	
	
HpelFilter16X16_V_ARMV7:

	sub         r2, r2, r3, LSL #1  
	vld1.64		{q0},  [r2], r3		
	add         r12, r2, r3, LSL #2 
	vld1.64		{q1},  [r2], r3		
	vmov.i16    q14,  #5			
	vld1.64		{q5},  [r12], r3	
	vaddl.u8	q6,  d0,  d10		 	
	vld1.64		{q2},  [r2], r3		
	vaddl.u8	q7,  d1,  d11		
	vld1.64		{q3},  [r2], r3		
	vmov.i16    q15,  #20			
	vld1.64		{q4},  [r2], r3		
	vaddl.u8	q8,  d2,  d8		
	vaddl.u8	q9,  d3,  d9		
	vaddl.u8	q10,  d4,  d6		
	vaddl.u8	q11,  d5,  d7		
	vmla.s16    q6,  q10,  q15  	
	vmla.s16    q7,  q11,  q15		
	vmls.s16    q6,  q8,  q14		
	vmls.s16    q7,  q9,  q14		
	vqrshrun 	d24.u8,  q6.s16,  #5
	vqrshrun 	d25.u8,  q7.s16,  #5
	vst1.64     {q12}, [r0, :64],  r1

	HALF16X16_V_ROW  q0, r12, r0, r1, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11

	HALF16X16_V_ROW  q1, r12, r0, r1, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1

	HALF16X16_V_ROW  q2, r12, r0, r1, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3

	HALF16X16_V_ROW  q3, r12, r0, r1, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5

	HALF16X16_V_ROW  q4, r12, r0, r1, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7

	HALF16X16_V_ROW  q5, r12, r0, r1, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9

	HALF16X16_V_ROW  q0, r12, r0, r1, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11

	HALF16X16_V_ROW  q1, r12, r0, r1, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1

	HALF16X16_V_ROW  q2, r12, r0, r1, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3

	HALF16X16_V_ROW  q3, r12, r0, r1, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5

	HALF16X16_V_ROW  q4, r12, r0, r1, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7

	HALF16X16_V_ROW  q5, r12, r0, r1, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9

	HALF16X16_V_ROW  q0, r12, r0, r1, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11

	HALF16X16_V_ROW  q1, r12, r0, r1, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1

	HALF16X16_V_ROW  q2, r12, r0, r1, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	bx          lr
	
@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
HpelFilter16X16_H_ARMV7:

	sub         r2, r2, #2  		
	add			r12,  r2,  #16		
	vmov.i16    q14,  #5			
	vmov.i16    q15,  #20			
	HALF16X16_H_ROW r0, r1
	HALF16X16_H_ROW r0, r1
	HALF16X16_H_ROW r0, r1
	HALF16X16_H_ROW r0, r1
	HALF16X16_H_ROW r0, r1
	HALF16X16_H_ROW r0, r1
	HALF16X16_H_ROW r0, r1
	HALF16X16_H_ROW r0, r1
	HALF16X16_H_ROW r0, r1
	HALF16X16_H_ROW r0, r1
	HALF16X16_H_ROW r0, r1
	HALF16X16_H_ROW r0, r1
	HALF16X16_H_ROW r0, r1
	HALF16X16_H_ROW r0, r1
	HALF16X16_H_ROW r0, r1
	HALF16X16_H_ROW r0, r1
	bx          lr
@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
HpelFilter16X16_C_ARMV7:	
	
	push     	{r4 - r10, r14}
	sub         r2, r2, r3, LSL #1  
	sub			r2, r2, #2
	add         r5, r2, #8
	add			r7, r0, #8
	vld1.64		{q0},  [r2], r3		
	add         r12, r2, r3, LSL #2 
	vld1.64		{q1},  [r2], r3		
	vmov.i16    q14,  #5			
	vld1.64		{q5},  [r12], r3	
	vaddl.u8	q6,  d0,  d10		 	
	vld1.64		{q2},  [r2], r3		
	vaddl.u8	q7,  d1,  d11		
	vld1.64		{q3},  [r2], r3		
	vmov.i16    q15,  #20			
	vld1.64		{q4},  [r2], r3		
	vaddl.u8	q8,  d2,  d8		
	vaddl.u8	q9,  d3,  d9		
	vaddl.u8	q10,  d4,  d6		
	vaddl.u8	q11,  d5,  d7		
	vmla.s16    q6,  q10,  q15  	
	vmla.s16    q7,  q11,  q15		
	vmls.s16    q6,  q8,  q14		
	vmls.s16    q7,  q9,  q14		
	vext.8		q8,  q6,  q7,  #2	
	vext.8		q9,  q6,  q7,  #4	
	vext.8		q10,  q6,  q7,  #6	
	vext.8		q11,  q6,  q7,  #8		
	vext.8		q12,  q6,  q7,  #10	
	vaddl.s16	q7,  d12,  d24		
	vaddl.s16	q0,  d13,  d25		
	vaddl.s16	q6,  d16,  d22		
	vaddl.s16	q12,  d17,  d23		
	vaddl.s16	q8,  d18,  d20		
	vaddl.s16	q11,  d19,  d21		
	vmov.i32    q9,  #5				
	vmov.i32    q10,  #20			
	vmla.s32    q7,  q8,  q10  		
	vmla.s32    q0,  q11,  q10		
	vmls.s32    q7,  q6,  q9		
	vmls.s32    q0,  q12,  q9		
	vqrshrun 	d20.u16,  q7.s32,  #10	
	vqrshrun 	d21.u16,  q0.s32,  #10	
	vqmovn		d24.u8,  q10.u16	
	vst1.64     {d24}, [r0, :64],  r1
@row 2 0-7
    HALF16X16_C_ROW  q0, r12, q1, r0, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
@row 3 0-7
	HALF16X16_C_ROW  q1, r12, q2, r0, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
@row 4 0-7
	HALF16X16_C_ROW  q2, r12, q3, r0, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
@row 5 0-7
	HALF16X16_C_ROW  q3, r12, q4, r0, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5	
@row 6 0-7	
	HALF16X16_C_ROW  q4, r12, q5, r0, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
@row 7 0-7	
	HALF16X16_C_ROW  q5, r12, q0, r0, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
@row 8 0-7
    HALF16X16_C_ROW  q0, r12, q1, r0, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
@row 9 0-7
	HALF16X16_C_ROW  q1, r12, q2, r0, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
@row 10 0-7
	HALF16X16_C_ROW  q2, r12, q3, r0, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
@row 11 0-7
	HALF16X16_C_ROW  q3, r12, q4, r0, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5	
@row 12 0-7	
	HALF16X16_C_ROW  q4, r12, q5, r0, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
@row 13 0-7	
	HALF16X16_C_ROW  q5, r12, q0, r0, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9	
@row 14 0-7
    HALF16X16_C_ROW  q0, r12, q1, r0, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
@row 15 0-7
	HALF16X16_C_ROW  q1, r12, q2, r0, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
@row 16 0-7
	HALF16X16_C_ROW  q2, r12, q3, r0, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3		
	vld1.64		{q0},  [r5], r3		
	add         r6, r5, r3, LSL #2 
	vld1.64		{q1},  [r5], r3		
	vmov.i16    q14,  #5			
	vld1.64		{q5},  [r6], r3	
	vaddl.u8	q6,  d0,  d10		 	
	vld1.64		{q2},  [r5], r3		
	vaddl.u8	q7,  d1,  d11		
	vld1.64		{q3},  [r5], r3		
	vmov.i16    q15,  #20			
	vld1.64		{q4},  [r5], r3		
	vaddl.u8	q8,  d2,  d8		
	vaddl.u8	q9,  d3,  d9		
	vaddl.u8	q10,  d4,  d6		
	vaddl.u8	q11,  d5,  d7		
	vmla.s16    q6,  q10,  q15  	
	vmla.s16    q7,  q11,  q15		
	vmls.s16    q6,  q8,  q14		
	vmls.s16    q7,  q9,  q14		
	vext.8		q8,  q6,  q7,  #2	
	vext.8		q9,  q6,  q7,  #4	
	vext.8		q10,  q6,  q7,  #6	
	vext.8		q11,  q6,  q7,  #8		
	vext.8		q12,  q6,  q7,  #10	 
	vaddl.s16	q7,  d12,  d24		
	vaddl.s16	q0,  d13,  d25		
	vaddl.s16	q6,  d16,  d22		
	vaddl.s16	q12,  d17,  d23		
	vaddl.s16	q8,  d18,  d20		
	vaddl.s16	q11,  d19,  d21		
	vmov.i32    q9,  #5				
	vmov.i32    q10,  #20				
	vmla.s32    q7,  q8,  q10  		
	vmla.s32    q0,  q11,  q10		
	vmls.s32    q7,  q6,  q9		
	vmls.s32    q0,  q12,  q9		
	vqrshrun 	d20.u16,  q7.s32,  #10	
	vqrshrun 	d21.u16,  q0.s32,  #10	
	vqmovn		d24.u8,  q10.u16	
	vst1.64     {d24}, [r7, :64],  r1
@row 2 8-15
    HALF16X16_C_ROW  q0, r6, q1, r7, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
@row 3 8-15
	HALF16X16_C_ROW  q1, r6, q2, r7, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
@row 4 8-15
	HALF16X16_C_ROW  q2, r6, q3, r7, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
@row 5 8-15
	HALF16X16_C_ROW  q3, r6, q4, r7, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5	
@row 6 8-15	
	HALF16X16_C_ROW  q4, r6, q5, r7, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
@row 7 8-15	
	HALF16X16_C_ROW  q5, r6, q0, r7, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
@row 8 8-15
    HALF16X16_C_ROW  q0, r6, q1, r7, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
@row 9 8-15
	HALF16X16_C_ROW  q1, r6, q2, r7, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
@row 10 8-15
	HALF16X16_C_ROW  q2, r6, q3, r7, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
@row 11 8-15
	HALF16X16_C_ROW  q3, r6, q4, r7, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5	
@row 12 8-15	
	HALF16X16_C_ROW  q4, r6, q5, r7, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
@row 13 8-15	
	HALF16X16_C_ROW  q5, r6, q0, r7, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9	
@row 14 8-15
    HALF16X16_C_ROW  q0, r6, q1, r7, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
@row 15 8-15
	HALF16X16_C_ROW  q1, r6, q2, r7, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
@row 16 8-15
	HALF16X16_C_ROW  q2, r6, q3, r7, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3		
	pop      	{r4 - r10, pc}
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
Hpel16x16SearchFilter_HV_ARMV7:
@r0 dsth
@r1 dstv
@r2 src
@r3 src_stride
	push     	{r4 - r10, r14}
	mov			r5, r2
	mov			r6, #32
	sub         r2, r2, r3, LSL #1  
	sub         r2, r2, r3
	vld1.64		{q0},  [r2], r3		
	add         r12, r2, r3, LSL #2 
	vld1.64		{q1},  [r2], r3		
	vmov.i16    q14,  #5			
	vld1.64		{q5},  [r12], r3	
	vaddl.u8	q6,  d0,  d10		 	
	vld1.64		{q2},  [r2], r3		
	vaddl.u8	q7,  d1,  d11		
	vld1.64		{q3},  [r2], r3		
	vmov.i16    q15,  #20				
	vld1.64		{q4},  [r2], r3		
	vaddl.u8	q8,  d2,  d8		
	vaddl.u8	q9,  d3,  d9		
	vaddl.u8	q10,  d4,  d6		
	vaddl.u8	q11,  d5,  d7		
	vmla.s16    q6,  q10,  q15  	
	vmla.s16    q7,  q11,  q15		
	vmls.s16    q6,  q8,  q14		
	vmls.s16    q7,  q9,  q14		
	vqrshrun 	d24.u8,  q6.s16,  #5
	vqrshrun 	d25.u8,  q7.s16,  #5
	vst1.64     {q12}, [r1, :64],  r6
@row 2
	HALF16X16_V_ROW  q0, r12, r1, r6, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
@row 3
	HALF16X16_V_ROW  q1, r12, r1, r6, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
@row 4 
	HALF16X16_V_ROW  q2, r12, r1, r6, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
@row 5
	HALF16X16_V_ROW  q3, r12, r1, r6, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5
@row 6
	HALF16X16_V_ROW  q4, r12, r1, r6, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
@row 7
	HALF16X16_V_ROW  q5, r12, r1, r6, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
@row 8
	HALF16X16_V_ROW  q0, r12, r1, r6, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
@row 9
	HALF16X16_V_ROW  q1, r12, r1, r6, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
@row 10
	HALF16X16_V_ROW  q2, r12, r1, r6, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
@row 11
	HALF16X16_V_ROW  q3, r12, r1, r6, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5
@row 12
	HALF16X16_V_ROW  q4, r12, r1, r6, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
@row 13
	HALF16X16_V_ROW  q5, r12, r1, r6, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
@row 14
	HALF16X16_V_ROW  q0, r12, r1, r6, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
@row 15
	HALF16X16_V_ROW  q1, r12, r1, r6, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
@row 16
	HALF16X16_V_ROW  q2, r12, r1, r6, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
@row 17
	HALF16X16_V_ROW  q3, r12, r1, r6, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5	
	sub         r2, r5, #3  		
	add			r5, r0, #16
	add			r12,  r2,  #16		
	HALF16X16_H_ROW r0, r6
	HALF16X16_HV_EXTRAH r5, r6
	HALF16X16_H_ROW r0, r6
	HALF16X16_HV_EXTRAH r5, r6
	HALF16X16_H_ROW r0, r6
	HALF16X16_HV_EXTRAH r5, r6
	HALF16X16_H_ROW r0, r6
	HALF16X16_HV_EXTRAH r5, r6
	HALF16X16_H_ROW r0, r6
	HALF16X16_HV_EXTRAH r5, r6
	HALF16X16_H_ROW r0, r6
	HALF16X16_HV_EXTRAH r5, r6
	HALF16X16_H_ROW r0, r6
	HALF16X16_HV_EXTRAH r5, r6
	HALF16X16_H_ROW r0, r6
	HALF16X16_HV_EXTRAH r5, r6
	HALF16X16_H_ROW r0, r6
	HALF16X16_HV_EXTRAH r5, r6
	HALF16X16_H_ROW r0, r6
	HALF16X16_HV_EXTRAH r5, r6
	HALF16X16_H_ROW r0, r6
	HALF16X16_HV_EXTRAH r5, r6
	HALF16X16_H_ROW r0, r6
	HALF16X16_HV_EXTRAH r5, r6
	HALF16X16_H_ROW r0, r6
	HALF16X16_HV_EXTRAH r5, r6
	HALF16X16_H_ROW r0, r6
	HALF16X16_HV_EXTRAH r5, r6
	HALF16X16_H_ROW r0, r6
	HALF16X16_HV_EXTRAH r5, r6
	HALF16X16_H_ROW r0, r6
	HALF16X16_HV_EXTRAH r5, r6
	pop      	{r4 - r10, pc}	
	
	
	.end
