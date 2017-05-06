;*****************************************************************************
;*																			*
;*		VisualOn, Inc. Confidential and Proprietary, 2010					*
;*																			*
;*****************************************************************************
    AREA	|.text|, CODE

	EXPORT	HpelFilter16X16_V_ARMV7
	EXPORT	HpelFilter16X16_H_ARMV7
	EXPORT	HpelFilter16X16_C_ARMV7
	EXPORT  Hpel16x16SearchFilter_HV_ARMV7
	
	ALIGN 8
	
	MACRO 
	HALF16X16_V_ROW $q, $r0, $r1, $sd, $d0, $d1, $d2, $d3,  $d4, $d5, $d6, $d7, $d8, $d9, $d10, $d11
		vld1.64		{$q},  [$r0], r3		;f0 ... f15
		vaddl.u8	q10,  $d0,  $d1		;c0+d0 ... c7+d7
		vaddl.u8	q11,  $d2,  $d3		;c8+d8 ... c15+d15	
		vaddl.u8	q6,  $d4,  $d5		;a0+f0 ... a7+f7
		vaddl.u8	q7,  $d6,  $d7		;a8+f8 ... a15+f15
		vaddl.u8	q8,  $d8,  $d9		;b0+e0 ... b7+e7
		vaddl.u8	q9,  $d10,  $d11	;b8+e8 ... b15+e15	
		vmla.s16    q6,  q10,  q15  	;a+20(c+d)+f 0-7
		vmla.s16    q7,  q11,  q15		;a+20(c+d)+f 8-15
		vmls.s16    q6,  q8,  q14		;a-5(b+e)+20(c+d)+f 0-7
		vmls.s16    q7,  q9,  q14		;a-5(b+e)+20(c+d)+f 8-15
		vqrshrun.s16 	d24,  q6,  #5	;0-7 solution
		vqrshrun.s16 	d25,  q7,  #5	;8-15 solution
		vst1.64     {q12}, [$r1 @64],  $sd
	MEND
	
	MACRO 
	HALF16X16_C_ROW $q0, $r0, $q1, $r1, $d0, $d1, $d2, $d3,  $d4, $d5, $d6, $d7, $d8, $d9, $d10, $d11
		vld1.64		{$q0},  [$r0], r3		;f0 ... f15
		vaddl.u8	q10,  $d0,  $d1		;c0+d0 ... c7+d7
		vaddl.u8	q11,  $d2,  $d3		;c8+d8 ... c15+d15	
		vaddl.u8	q6,  $d4,  $d5		;a0+f0 ... a7+f7
		vaddl.u8	q7,  $d6,  $d7		;a8+f8 ... a15+f15
		vaddl.u8	q8,  $d8,  $d9		;b0+e0 ... b7+e7
		vaddl.u8	q9,  $d10,  $d11		;b8+e8 ... b15+e15	
		vmla.s16    q6,  q10,  q15  	;a+20(c+d)+f 0-7
		vmla.s16    q7,  q11,  q15		;a+20(c+d)+f 8-15
		vmls.s16    q6,  q8,  q14		;a-5(b+e)+20(c+d)+f 0-7
		vmls.s16    q7,  q9,  q14		;a-5(b+e)+20(c+d)+f 8-15
		vext.8		q8,  q6,  q7,  #2	;b0 ... b7 
		vext.8		q9,  q6,  q7,  #4	;c0 ... c7
		vext.8		q10,  q6,  q7,  #6	;d0 ... d7 
		vext.8		q11,  q6,  q7,  #8	;e0 ... e7	
		vext.8		q12,  q6,  q7,  #10	;f0 ... f7 
		vaddl.s16	q7,  d12,  d24		;a0+f0 ... a3+f3
		vaddl.s16	$q1,  d13,  d25		;a4+f4 ... a7+f7
		vaddl.s16	q6,  d16,  d22		;b0+e0 ... b3+e3
		vaddl.s16	q12,  d17,  d23		;b4+e4 ... b7+e7
		vaddl.s16	q8,  d18,  d20		;c0+d0 ... c3+d3
		vaddl.s16	q11,  d19,  d21		;c4+d4 ... c7+d7
		vmov.i32    q9,  #5				;coeff 5
		vmov.i32    q10,  #20			;coeff 20	
		vmla.s32    q7,  q8,  q10  		;a+20(c+d)+f 0-3
		vmla.s32    $q1,  q11,  q10		;a+20(c+d)+f 4-8
		vmls.s32    q7,  q6,  q9		;a-5(b+e)+20(c+d)+f 0-3
		vmls.s32    $q1,  q12,  q9		;a-5(b+e)+20(c+d)+f 4-8
		vqrshrun.s32 	d20,  q7,  #10	;0-7 solution
		vqrshrun.s32 	d21,  $q1,  #10	;8-15 solution
		vqmovn		d24.u8,  q10.u16	;0-7 solution
		vst1.64     {d24}, [$r1 @64],  r1
	MEND
	
	MACRO 
	HALF16X16_H_ROW $r, $sd
		vld1.64		{q0},  [r2], r3		
		vld1.64		{q13},  [r12], r3
		vext.8		q1,  q0,  q13,  #1	;b0 ... b15
		vext.8		q2,  q0,  q13,  #2	;c0 ... c15
		vext.8		q3,  q0,  q13,  #3	;d0 ... d15
		vext.8		q4,  q0,  q13,  #4	;e0 ... e15
		vext.8		q5,  q0,  q13,  #5	;f0 ... f15	
		vaddl.u8	q10,  d4,  d6		;c0+d0 ... c7+d7
		vaddl.u8	q11,  d5,  d7		;c8+d8 ... c15+d15	
		vaddl.u8	q6,  d0,  d10		;a0+f0 ... a7+f7
		vaddl.u8	q7,  d1,  d11		;a8+f8 ... a15+f15
		vaddl.u8	q8,  d2,  d8		;b0+e0 ... b7+e7
		vaddl.u8	q9,  d3,  d9		;b8+e8 ... b15+e15
		vmla.s16    q6,  q10,  q15  	;a+20(c+d)+f 0-7
		vmla.s16    q7,  q11,  q15		;a+20(c+d)+f 8-15
		vmls.s16    q6,  q8,  q14		;a-5(b+e)+20(c+d)+f 0-7
		vmls.s16    q7,  q9,  q14		;a-5(b+e)+20(c+d)+f 8-15
		vqrshrun.s16 	d24,  q6,  #5	;0-7 solution
		vqrshrun.s16 	d25,  q7,  #5	;8-15 solution
		vst1.64     {q12}, [$r @64],  $sd
	MEND
	
	MACRO
	HALF16X16_HV_EXTRAH $r, $sd
		vext.8		d4,  d26,  d27,  #1	;b0 ... b15
		vext.8		d6,  d26,  d27,  #2	;c0 ... c15
		vext.8		d8,  d26,  d27,  #3	;d0 ... d15
		vext.8		d10,  d26,  d27,  #4;e0 ... e15
		vext.8		d12,  d26,  d27,  #5;f0 ... f15	
		vaddl.u8	q10,  d6,  d8		;c0+d0 ... c7+d7	
		vaddl.u8	q7,  d26,  d12		;a0+f0 ... a7+f7
		vaddl.u8	q8,  d4,  d10		;b0+e0 ... b7+e7
		vmla.s16    q7,  q10,  q15  	;a+20(c+d)+f 0-7
		vmls.s16    q7,  q8,  q14		;a-5(b+e)+20(c+d)+f 0-7
		vqrshrun.s16 	d24,  q7,  #5	;0-7 solution
		vst1.64     {d24}, [$r @64],  $sd
	MEND
HpelFilter16X16_V_ARMV7

;r0  dst
;r1  dst_stride
;r2  src
;r3  src_stride
;q0 - q5 orig data of a0,b0,c0,d0,e0,f0 a1,b1,c1,d1,e1,f1
;q6  a0+f0
;q7  a1+f1
;q8  b0+e0
;q9  b1+e1
;q10 c0+d0
;q11 c1+d1 
;q12 solution
;q14 coeff5
;q15 coeff20
	
	sub         r2, r2, r3, LSL #1  ;src = src - 2*stride
	vld1.64		{q0},  [r2], r3		;src - 2*stride a0 a1 ... a15
	add         r12, r2, r3, LSL #2 ;r12 = src + 3*stride
	vld1.64		{q1},  [r2], r3		;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			;coeff 5
	vld1.64		{q5},  [r12], r3	;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r2], r3		;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		;a8+f8 ... a15+f15
	vld1.64		{q3},  [r2], r3		;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			;coeff 20	
	vld1.64		{q4},  [r2], r3		;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		;a-5(b+e)+20(c+d)+f 8-15
	vqrshrun 	d24.u8,  q6.s16,  #5;0-7 solution
	vqrshrun 	d25.u8,  q7.s16,  #5;8-15 solution
	vst1.64     {q12}, [r0 @64],  r1
;row 2
;q0 f0 ... f15 d0 d1
;q1 a0 ... a15 d2 d3
;q2 b0 ... b15 d4 d5
;q3 c0 ... c15 d6 d7
;q4 d0 ... d15 d8 d9
;q5 e0 ... e15 d10 d11
	HALF16X16_V_ROW  q0, r12, r0, r1, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
;row 3
;q0 e0 ... e15 d0 d1
;q1 f0 ... f15 d2 d3
;q2 a0 ... a15 d4 d5
;q3 b0 ... b15 d6 d7
;q4 c0 ... c15 d8 d9
;q5 d0 ... d15 d10 d11
	HALF16X16_V_ROW  q1, r12, r0, r1, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
;row 4
;q0 d0 ... d15 d0 d1
;q1 e0 ... e15 d2 d3
;q2 f0 ... f15 d4 d5
;q3 a0 ... a15 d6 d7
;q4 b0 ... b15 d8 d9
;q5 c0 ... c15 d10 d11
	HALF16X16_V_ROW  q2, r12, r0, r1, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
;row 5
;q0 c0 ... c15 d0 d1
;q1 d0 ... d15 d2 d3
;q2 e0 ... e15 d4 d5
;q3 f0 ... f15 d6 d7
;q4 a0 ... a15 d8 d9
;q5 b0 ... b15 d10 d11
	HALF16X16_V_ROW  q3, r12, r0, r1, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5
;row 6
;q0 b0 ... b15 d0 d1
;q1 c0 ... c15 d2 d3
;q2 d0 ... d15 d4 d5
;q3 e0 ... e15 d6 d7
;q4 f0 ... f15 d8 d9
;q5 a0 ... a15 d10 d11
	HALF16X16_V_ROW  q4, r12, r0, r1, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
;row 7
;q0 a0 ... a15 d0 d1
;q1 b0 ... b15 d2 d3
;q2 c0 ... c15 d4 d5
;q3 d0 ... d15 d6 d7
;q4 e0 ... e15 d8 d9
;q5 f0 ... f15 d10 d11
	HALF16X16_V_ROW  q5, r12, r0, r1, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
;row 8
;q0 f0 ... f15 d0 d1
;q1 a0 ... a15 d2 d3
;q2 b0 ... b15 d4 d5
;q3 c0 ... c15 d6 d7
;q4 d0 ... d15 d8 d9
;q5 e0 ... e15 d10 d11
	HALF16X16_V_ROW  q0, r12, r0, r1, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
;row 9
;q0 e0 ... e15 d0 d1
;q1 f0 ... f15 d2 d3
;q2 a0 ... a15 d4 d5
;q3 b0 ... b15 d6 d7
;q4 c0 ... c15 d8 d9
;q5 d0 ... d15 d10 d11
	HALF16X16_V_ROW  q1, r12, r0, r1, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
;row 10
;q0 d0 ... d15 d0 d1
;q1 e0 ... e15 d2 d3
;q2 f0 ... f15 d4 d5
;q3 a0 ... a15 d6 d7
;q4 b0 ... b15 d8 d9
;q5 c0 ... c15 d10 d11
	HALF16X16_V_ROW  q2, r12, r0, r1, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
;row 11
;q0 c0 ... c15 d0 d1
;q1 d0 ... d15 d2 d3
;q2 e0 ... e15 d4 d5
;q3 f0 ... f15 d6 d7
;q4 a0 ... a15 d8 d9
;q5 b0 ... b15 d10 d11
	HALF16X16_V_ROW  q3, r12, r0, r1, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5
;row 12
;q0 b0 ... b15 d0 d1
;q1 c0 ... c15 d2 d3
;q2 d0 ... d15 d4 d5
;q3 e0 ... e15 d6 d7
;q4 f0 ... f15 d8 d9
;q5 a0 ... a15 d10 d11
	HALF16X16_V_ROW  q4, r12, r0, r1, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
;row 13
;q0 a0 ... a15 d0 d1
;q1 b0 ... b15 d2 d3
;q2 c0 ... c15 d4 d5
;q3 d0 ... d15 d6 d7
;q4 e0 ... e15 d8 d9
;q5 f0 ... f15 d10 d11
	HALF16X16_V_ROW  q5, r12, r0, r1, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
;row 14
;q0 f0 ... f15 d0 d1
;q1 a0 ... a15 d2 d3
;q2 b0 ... b15 d4 d5
;q3 c0 ... c15 d6 d7
;q4 d0 ... d15 d8 d9
;q5 e0 ... e15 d10 d11
	HALF16X16_V_ROW  q0, r12, r0, r1, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
;row 15
;q0 e0 ... e15 d0 d1
;q1 f0 ... f15 d2 d3
;q2 a0 ... a15 d4 d5
;q3 b0 ... b15 d6 d7
;q4 c0 ... c15 d8 d9
;q5 d0 ... d15 d10 d11
	HALF16X16_V_ROW  q1, r12, r0, r1, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
;row 16
;q0 d0 ... d15 d0 d1
;q1 e0 ... e15 d2 d3
;q2 f0 ... f15 d4 d5
;q3 a0 ... a15 d6 d7
;q4 b0 ... b15 d8 d9
;q5 c0 ... c15 d10 d11
	HALF16X16_V_ROW  q2, r12, r0, r1, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
	bx          lr
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
HpelFilter16X16_H_ARMV7
;r0  dst
;r1  dst_stride
;r2  src
;r3  src_stride
;q0 - q5 orig data of a0,b0,c0,d0,e0,f0 a1,b1,c1,d1,e1,f1
;q6  a0+f0
;q7  a1+f1
;q8  b0+e0
;q9  b1+e1
;q10 c0+d0
;q11 c1+d1 
;q12 solution
;q14 coeff5
;q15 coeff20
	sub         r2, r2, #2  		;src = src - 2
	add			r12,  r2,  #16		;src + 16
	vmov.i16    q14,  #5			;coeff 5
	vmov.i16    q15,  #20			;coeff 20	
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
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
HpelFilter16X16_C_ARMV7	
	
	push     	{r4 - r10, r14}
	sub         r2, r2, r3, LSL #1  ;src = src - 2*stride
	sub			r2, r2, #2
	add         r5, r2, #8
	add			r7, r0, #8
	vld1.64		{q0},  [r2], r3		;src - 2*stride a0 a1 ... a15
	add         r12, r2, r3, LSL #2 ;r12 = src + 3*stride
	vld1.64		{q1},  [r2], r3		;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			;coeff 5
	vld1.64		{q5},  [r12], r3	;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r2], r3		;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		;a8+f8 ... a15+f15
	vld1.64		{q3},  [r2], r3		;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			;coeff 20	
	vld1.64		{q4},  [r2], r3		;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		;a-5(b+e)+20(c+d)+f 8-15
	vext.8		q8,  q6,  q7,  #2	;b0 ... b7 
	vext.8		q9,  q6,  q7,  #4	;c0 ... c7
	vext.8		q10,  q6,  q7,  #6	;d0 ... d7 
	vext.8		q11,  q6,  q7,  #8	;e0 ... e7	
	vext.8		q12,  q6,  q7,  #10	;f0 ... f7 
	vaddl.s16	q7,  d12,  d24		;a0+f0 ... a3+f3
	vaddl.s16	q0,  d13,  d25		;a4+f4 ... a7+f7
	vaddl.s16	q6,  d16,  d22		;b0+e0 ... b3+e3
	vaddl.s16	q12,  d17,  d23		;b4+e4 ... b7+e7
	vaddl.s16	q8,  d18,  d20		;c0+d0 ... c3+d3
	vaddl.s16	q11,  d19,  d21		;c4+d4 ... c7+d7
	vmov.i32    q9,  #5				;coeff 5
	vmov.i32    q10,  #20			;coeff 20	
	vmla.s32    q7,  q8,  q10  		;a+20(c+d)+f 0-3
	vmla.s32    q0,  q11,  q10		;a+20(c+d)+f 4-8
	vmls.s32    q7,  q6,  q9		;a-5(b+e)+20(c+d)+f 0-3
	vmls.s32    q0,  q12,  q9		;a-5(b+e)+20(c+d)+f 4-8
	vqrshrun 	d20.u16,  q7.s32,  #10	;0-7 solution
	vqrshrun 	d21.u16,  q0.s32,  #10	;8-15 solution
	vqmovn		d24.u8,  q10.u16	;0-7 solution
	vst1.64     {d24}, [r0 @64],  r1
;row 2 0-7
    HALF16X16_C_ROW  q0, r12, q1, r0, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
;row 3 0-7
	HALF16X16_C_ROW  q1, r12, q2, r0, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
;row 4 0-7
	HALF16X16_C_ROW  q2, r12, q3, r0, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
;row 5 0-7
	HALF16X16_C_ROW  q3, r12, q4, r0, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5	
;row 6 0-7	
	HALF16X16_C_ROW  q4, r12, q5, r0, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
;row 7 0-7	
	HALF16X16_C_ROW  q5, r12, q0, r0, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
;row 8 0-7
    HALF16X16_C_ROW  q0, r12, q1, r0, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
;row 9 0-7
	HALF16X16_C_ROW  q1, r12, q2, r0, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
;row 10 0-7
	HALF16X16_C_ROW  q2, r12, q3, r0, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
;row 11 0-7
	HALF16X16_C_ROW  q3, r12, q4, r0, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5	
;row 12 0-7	
	HALF16X16_C_ROW  q4, r12, q5, r0, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
;row 13 0-7	
	HALF16X16_C_ROW  q5, r12, q0, r0, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9	
;row 14 0-7
    HALF16X16_C_ROW  q0, r12, q1, r0, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
;row 15 0-7
	HALF16X16_C_ROW  q1, r12, q2, r0, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
;row 16 0-7
	HALF16X16_C_ROW  q2, r12, q3, r0, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3		
	vld1.64		{q0},  [r5], r3		;src - 2*stride a0 a1 ... a15
	add         r6, r5, r3, LSL #2 ;r12 = src + 3*stride
	vld1.64		{q1},  [r5], r3		;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			;coeff 5
	vld1.64		{q5},  [r6], r3	;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r5], r3		;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		;a8+f8 ... a15+f15
	vld1.64		{q3},  [r5], r3		;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			;coeff 20	
	vld1.64		{q4},  [r5], r3		;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		;a-5(b+e)+20(c+d)+f 8-15
	vext.8		q8,  q6,  q7,  #2	;b0 ... b7 
	vext.8		q9,  q6,  q7,  #4	;c0 ... c7
	vext.8		q10,  q6,  q7,  #6	;d0 ... d7 
	vext.8		q11,  q6,  q7,  #8	;e0 ... e7	
	vext.8		q12,  q6,  q7,  #10	;f0 ... f7 
	vaddl.s16	q7,  d12,  d24		;a0+f0 ... a3+f3
	vaddl.s16	q0,  d13,  d25		;a4+f4 ... a7+f7
	vaddl.s16	q6,  d16,  d22		;b0+e0 ... b3+e3
	vaddl.s16	q12,  d17,  d23		;b4+e4 ... b7+e7
	vaddl.s16	q8,  d18,  d20		;c0+d0 ... c3+d3
	vaddl.s16	q11,  d19,  d21		;c4+d4 ... c7+d7
	vmov.i32    q9,  #5				;coeff 5
	vmov.i32    q10,  #20			;coeff 20	
	vmla.s32    q7,  q8,  q10  		;a+20(c+d)+f 0-3
	vmla.s32    q0,  q11,  q10		;a+20(c+d)+f 4-8
	vmls.s32    q7,  q6,  q9		;a-5(b+e)+20(c+d)+f 0-3
	vmls.s32    q0,  q12,  q9		;a-5(b+e)+20(c+d)+f 4-8
	vqrshrun 	d20.u16,  q7.s32,  #10	;0-7 solution
	vqrshrun 	d21.u16,  q0.s32,  #10	;8-15 solution
	vqmovn		d24.u8,  q10.u16	;0-7 solution
	vst1.64     {d24}, [r7 @64],  r1
;row 2 8-15
    HALF16X16_C_ROW  q0, r6, q1, r7, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
;row 3 8-15
	HALF16X16_C_ROW  q1, r6, q2, r7, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
;row 4 8-15
	HALF16X16_C_ROW  q2, r6, q3, r7, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
;row 5 8-15
	HALF16X16_C_ROW  q3, r6, q4, r7, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5	
;row 6 8-15	
	HALF16X16_C_ROW  q4, r6, q5, r7, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
;row 7 8-15	
	HALF16X16_C_ROW  q5, r6, q0, r7, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
;row 8 8-15
    HALF16X16_C_ROW  q0, r6, q1, r7, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
;row 9 8-15
	HALF16X16_C_ROW  q1, r6, q2, r7, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
;row 10 8-15
	HALF16X16_C_ROW  q2, r6, q3, r7, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3	
;row 11 8-15
	HALF16X16_C_ROW  q3, r6, q4, r7, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5	
;row 12 8-15	
	HALF16X16_C_ROW  q4, r6, q5, r7, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
;row 13 8-15	
	HALF16X16_C_ROW  q5, r6, q0, r7, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9	
;row 14 8-15
    HALF16X16_C_ROW  q0, r6, q1, r7, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11	
;row 15 8-15
	HALF16X16_C_ROW  q1, r6, q2, r7, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
;row 16 8-15
	HALF16X16_C_ROW  q2, r6, q3, r7, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3		
	pop      	{r4 - r10, pc}
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
Hpel16x16SearchFilter_HV_ARMV7
;r0 dsth
;r1 dstv
;r2 src
;r3 src_stride
	push     	{r4 - r10, r14}
	mov			r5, r2
	mov			r6, #32
	sub         r2, r2, r3, LSL #1  ;src = src - 2*stride
	sub         r2, r2, r3
	vld1.64		{q0},  [r2], r3		;src - 2*stride a0 a1 ... a15
	add         r12, r2, r3, LSL #2 ;r12 = src + 3*stride
	vld1.64		{q1},  [r2], r3		;src - stride b0 b1 ... b15
	vmov.i16    q14,  #5			;coeff 5
	vld1.64		{q5},  [r12], r3	;src + 3*stride	f0 f1 ... f15
	vaddl.u8	q6,  d0,  d10		;a0+f0 ... a7+f7  	
	vld1.64		{q2},  [r2], r3		;src c0 c1 ... c15
	vaddl.u8	q7,  d1,  d11		;a8+f8 ... a15+f15
	vld1.64		{q3},  [r2], r3		;src + stride d0 d1 ... d15
	vmov.i16    q15,  #20			;coeff 20	
	vld1.64		{q4},  [r2], r3		;src + 2*stride	e0 e1 ... e15
	vaddl.u8	q8,  d2,  d8		;b0+e0 ... b7+e7
	vaddl.u8	q9,  d3,  d9		;b8+e8 ... b15+e15
	vaddl.u8	q10,  d4,  d6		;c0+d0 ... c7+d7
	vaddl.u8	q11,  d5,  d7		;c8+d8 ... c15+d15	
	vmla.s16    q6,  q10,  q15  	;a+20(c+d)+f 0-7
	vmla.s16    q7,  q11,  q15		;a+20(c+d)+f 8-15
	vmls.s16    q6,  q8,  q14		;a-5(b+e)+20(c+d)+f 0-7
	vmls.s16    q7,  q9,  q14		;a-5(b+e)+20(c+d)+f 8-15
	vqrshrun 	d24.u8,  q6.s16,  #5;0-7 solution
	vqrshrun 	d25.u8,  q7.s16,  #5;8-15 solution
	vst1.64     {q12}, [r1 @64],  r6
;row 2
	HALF16X16_V_ROW  q0, r12, r1, r6, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
;row 3
	HALF16X16_V_ROW  q1, r12, r1, r6, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
;row 4 
	HALF16X16_V_ROW  q2, r12, r1, r6, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
;row 5
	HALF16X16_V_ROW  q3, r12, r1, r6, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5
;row 6
	HALF16X16_V_ROW  q4, r12, r1, r6, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
;row 7
	HALF16X16_V_ROW  q5, r12, r1, r6, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
;row 8
	HALF16X16_V_ROW  q0, r12, r1, r6, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
;row 9
	HALF16X16_V_ROW  q1, r12, r1, r6, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
;row 10
	HALF16X16_V_ROW  q2, r12, r1, r6, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
;row 11
	HALF16X16_V_ROW  q3, r12, r1, r6, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5
;row 12
	HALF16X16_V_ROW  q4, r12, r1, r6, d2, d4, d3, d5, d10, d8, d11, d9, d0, d6, d1, d7
;row 13
	HALF16X16_V_ROW  q5, r12, r1, r6, d4, d6, d5, d7, d0, d10, d1, d11, d2, d8, d3, d9
;row 14
	HALF16X16_V_ROW  q0, r12, r1, r6, d6, d8, d7, d9, d2, d0, d3, d1, d4, d10, d5, d11
;row 15
	HALF16X16_V_ROW  q1, r12, r1, r6, d8, d10, d9, d11, d4, d2, d5, d3, d6, d0, d7, d1
;row 16
	HALF16X16_V_ROW  q2, r12, r1, r6, d10, d0, d11, d1, d6, d4, d7, d5, d8, d2, d9, d3
;row 17
	HALF16X16_V_ROW  q3, r12, r1, r6, d0, d2, d1, d3, d8, d6, d9, d7, d10, d4, d11, d5	
	sub         r2, r5, #3  		;src = src - 2
	add			r5, r0, #16
	add			r12,  r2,  #16		;src + 16
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
	END