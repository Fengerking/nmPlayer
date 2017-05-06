;*****************************************************************************
;*																			*
;*		VisualOn, Inc. Confidential and Proprietary, 2003					*
;*																			*
;*****************************************************************************

	
	AREA	|.text|, CODE
	
	EXPORT	FastDct_ARMV7

	ALIGN	8
					;	 4176 , 4433 ,4926 , 6436  ,7373  , 9633 ,10703 ,20995 
Const_table		DCW		0x1050,0x1151,0x133E,0x1924,0x1CCD,0x25A1,0x29CF,0x5203

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;VO_VOID fdct_int32(short *const block)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
FastDct_ARMV7 PROC

;lr: can't cover the data

;q0: const data
;q14: #1<<10
;tmp0 - tmp7:d10-d17
;tmp10-tmp13:d20-d23

	ldr			r12, =Const_table
	mov			r2, #8
	mov			r1, #16
	add			r3, r0, #8
	vmov.u32	q14, #1<<10 
	vld1.32		{q0}, [r12]
ROW_LOOP
	
	; process 4 columns per loop
	
	;	tmp0 = ptr_block[0] + ptr_block[56];
	;	tmp7 = ptr_block[0] - ptr_block[56];
	;	tmp1 = ptr_block[8] + ptr_block[48];
	;	tmp6 = ptr_block[8] - ptr_block[48];
	;	tmp2 = ptr_block[16] + ptr_block[40];
	;	tmp5 = ptr_block[16] - ptr_block[40];
	;	tmp3 = ptr_block[24] + ptr_block[32];
	;	tmp4 = ptr_block[24] - ptr_block[32];

	vld4.16		{d20[0],d21[0],d22[0],d23[0]}, [r0], r1
	vld4.16		{d24[0],d25[0],d26[0],d27[0]}, [r3], r1
	vld4.16		{d20[1],d21[1],d22[1],d23[1]}, [r0], r1
	vld4.16		{d24[1],d25[1],d26[1],d27[1]}, [r3], r1
	vld4.16		{d20[2],d21[2],d22[2],d23[2]}, [r0], r1
	vld4.16		{d24[2],d25[2],d26[2],d27[2]}, [r3], r1
	vld4.16		{d20[3],d21[3],d22[3],d23[3]}, [r0], r1
	vld4.16		{d24[3],d25[3],d26[3],d27[3]}, [r3], r1
	sub			r0, r0, r1,lsl #2
	sub			r3, r3, r1,lsl #2
	subs		r2, r2, #4

	vadd.s16	d10, d20, d27
	vsub.s16	d17, d20, d27
	vadd.s16	d11, d21, d26
	vsub.s16	d16, d21, d26
	vadd.s16	d12, d22, d25
	vsub.s16	d15, d22, d25
	vadd.s16	d13, d23, d24
	vsub.s16	d14, d23, d24

	;	tmp10 = tmp0 + tmp3;
	;	tmp13 = tmp0 - tmp3;
	;	tmp11 = tmp1 + tmp2;
	;	tmp12 = tmp1 - tmp2;
	
	vadd.s16	d20, d10, d13	;tmp10
	vsub.s16	d23, d10, d13	;tmp13
	vadd.s16	d21, d11, d12	;tmp11
	vsub.s16	d22, d11, d12	;tmp12
	
	;	ptr_block[0] = (short)(tmp10 + tmp11) << PASS1_BITS;
	;	ptr_block[4] = (short)(tmp10 - tmp11) << PASS1_BITS;
	;	ptr_block[2] =	(short)DESCALE(tmp12 * CONST_4433     + tmp13 * CONST_10703, CONST_BITS - PASS1_BITS);
	;	ptr_block[6] =	(short)DESCALE(tmp12 * (-CONST_10703) + tmp13 * CONST_4433 , CONST_BITS - PASS1_BITS);
	;	z5 = (tmp4 + tmp6) *(-CONST_6436) + (tmp5 + tmp7) * CONST_9633;
	;	z6 = (tmp4 + tmp6) *  CONST_9633  + (tmp5 + tmp7) * CONST_6436;	
	
	vsubl.s16	q2, d20, d21
	vaddl.s16	q1, d20, d21
	vshl.s32	q10, q2, #2	;<<2
	vshl.s32	q9, q1, #2	

	vmull.s16	q1, d23, d1[2]
	vqshrn.s32	d24, q10, #0	;ptr_block[4]
	vmull.s16	q2, d23, d0[1]
	vqshrn.s32	d20, q9, #0		;ptr_block[0]
	vmov.u32	q4, #0 
	vmlal.s16	q1, d22, d0[1]
	vadd.s16	d19, d15, d17	;tmp5 + tmp7
	vmlsl.s16	q2, d22, d1[2]
	vadd.s16	d18, d14, d16	;tmp4 + tmp6
	vmull.s16	q6, d19, d0[3]
	vmov.u32	q3, #0
	vadd.s32	q1, q1, q14		;+RND
	vadd.s32	q2, q2, q14		;+RND
	
	vmull.s16	q5, d19, d1[1]
	vqshrn.s32	d22, q1, #11	;ptr_block[2]
	vmlal.s16	q6, d18, d1[1]	;z6
	vqshrn.s32	d26, q2, #11	;ptr_block[6]
	vmlsl.s16	q5, d18, d0[3]	;z5
	
	;	z4 = tmp4 * (-CONST_4926)  + tmp7 * (-CONST_7373);
	;	z1 = tmp4 * (-CONST_7373)  + tmp7 *   CONST_4926;
	;	z3 = tmp5 * (-CONST_4176)  + tmp6 * (-CONST_20995);
	;	z2 = tmp5 * (-CONST_20995) + tmp6 *   CONST_4176;
	;	z4 += z5;
	;	z3 += z6;
	;	z2 += z5;
	;	z1 += z6;
	;	ptr_block[7] = (short)DESCALE(z4, CONST_BITS - PASS1_BITS);
	;	ptr_block[5] = (short)DESCALE(z3, CONST_BITS - PASS1_BITS);
	;	ptr_block[3] = (short)DESCALE(z2, CONST_BITS - PASS1_BITS);
	;	ptr_block[1] = (short)DESCALE(z1, CONST_BITS - PASS1_BITS);
	
	vmull.s16	q2, d16, d0[0]
	vmlsl.s16	q3, d16, d1[3]
	vmull.s16	q1, d17, d0[2]
	vmlsl.s16	q4, d17, d1[0]
	vmlsl.s16	q2, d15, d1[3]	;z2
	vmlsl.s16	q3, d15, d0[0]	;z3
	vadd.s32	q5, q5, q14		;+RND
	vmlsl.s16	q1, d14, d1[0]	;z1
	vadd.s32	q6, q6, q14		;+RND
	vmlsl.s16	q4, d14, d0[2]	;z4
	
	vadd.s32	q2, q2, q5		;z2 + z5
	vadd.s32	q3, q3, q6		;z3 + z6
	vqshrn.s32	d23, q2, #11	;ptr_block[3]
	vqshrn.s32	d25, q3, #11	;ptr_block[5]
	vadd.s32	q1, q1, q6		;z1 + z6
	vadd.s32	q4, q4, q5		;z4 + z5	
	vqshrn.s32	d21, q1, #11	;ptr_block[1]
	vqshrn.s32	d27, q4, #11	;ptr_block[7]
	
	vst4.16		{d20[0],d21[0],d22[0],d23[0]}, [r0], r1
	vst4.16		{d24[0],d25[0],d26[0],d27[0]}, [r3], r1
	vst4.16		{d20[1],d21[1],d22[1],d23[1]}, [r0], r1
	vst4.16		{d24[1],d25[1],d26[1],d27[1]}, [r3], r1
	vst4.16		{d20[2],d21[2],d22[2],d23[2]}, [r0], r1
	vst4.16		{d24[2],d25[2],d26[2],d27[2]}, [r3], r1
	vst4.16		{d20[3],d21[3],d22[3],d23[3]}, [r0], r1
	vst4.16		{d24[3],d25[3],d26[3],d27[3]}, [r3], r1
	
	bgt			ROW_LOOP
	

;q0: const data
;q15: #1<<4
;q14: #1<<17 
;tmp0 - tmp7:d10-d17
;tmp10-tmp13:d20-d23

	sub			r0, r0, r1,lsl #3
	mov			r2, #8
	mov			r1, #16
	vmov.u32	q15, #1<<4
	vmov.u32	q14, #1<<17 
COL_LOOP
	
	; process 4 columns per loop
	
	;	tmp0 = ptr_block[0] + ptr_block[56];
	;	tmp7 = ptr_block[0] - ptr_block[56];
	;	tmp1 = ptr_block[8] + ptr_block[48];
	;	tmp6 = ptr_block[8] - ptr_block[48];
	;	tmp2 = ptr_block[16] + ptr_block[40];
	;	tmp5 = ptr_block[16] - ptr_block[40];
	;	tmp3 = ptr_block[24] + ptr_block[32];
	;	tmp4 = ptr_block[24] - ptr_block[32];

	add			r3, r0, r1, lsl #2
	vld1.32		d20, [r0], r1
	vld1.32		d24, [r3], r1
	vld1.32		d21, [r0], r1
	vld1.32		d25, [r3], r1
	vld1.32		d22, [r0], r1
	vld1.32		d26, [r3], r1
	vld1.32		d23, [r0]
	vld1.32		d27, [r3]
	sub			r0, r0, #64-16
	subs		r2, r2, #4

	vadd.s16	d12, d22, d25
	vsub.s16	d15, d22, d25
	vadd.s16	d13, d23, d24
	vsub.s16	d14, d23, d24
	vadd.s16	d11, d21, d26
	vsub.s16	d16, d21, d26
	vadd.s16	d10, d20, d27
	vsub.s16	d17, d20, d27

	;	tmp10 = tmp0 + tmp3;
	;	tmp13 = tmp0 - tmp3;
	;	tmp11 = tmp1 + tmp2;
	;	tmp12 = tmp1 - tmp2;
	
	vadd.s16	d20, d10, d13	;tmp10
	vsub.s16	d23, d10, d13	;tmp13
	vadd.s16	d21, d11, d12	;tmp11
	vsub.s16	d22, d11, d12	;tmp12
	
	;	ptr_block[0]  = (short)DESCALE(tmp10 + tmp11, PASS1_BITS+3);
	;	ptr_block[32] = (short)DESCALE(tmp10 - tmp11, PASS1_BITS+3);
	;	ptr_block[16] = (short)DESCALE(tmp12 * CONST_4433     + tmp13 * CONST_10703, CONST_BITS + PASS1_BITS+3);
	;	ptr_block[48] = (short)DESCALE(tmp12 * (-CONST_10703) + tmp13 * CONST_4433 , CONST_BITS + PASS1_BITS+3);
	;	z5 = (tmp4 + tmp6) *(-CONST_6436) + (tmp5 + tmp7) * CONST_9633;
	;	z6 = (tmp4 + tmp6) *  CONST_9633  + (tmp5 + tmp7) * CONST_6436;	
	
	vsubl.s16	q2, d20, d21
	vaddl.s16	q1, d20, d21
	vadd.s32	q10, q2, q15
	vadd.s32	q9 , q1, q15	;+RND	

	vmull.s16	q1, d23, d1[2]
	vqshrn.s32	d24, q10, #5	;ptr_block[32]
	vmull.s16	q2, d23, d0[1]
	vqshrn.s32	d20, q9, #5		;ptr_block[0]
	vmov.u32	q4, #0 
	vmlal.s16	q1, d22, d0[1]
	vadd.s16	d19, d15, d17	;tmp5 + tmp7
	vmlsl.s16	q2, d22, d1[2]
	vadd.s16	d18, d14, d16	;tmp4 + tmp6
	vmull.s16	q6, d19, d0[3]
	vmov.u32	q3, #0
	vadd.s32	q1, q1, q14		;+RND
	vadd.s32	q2, q2, q14		;+RND
	vshr.s32	q1, q1, #16	;
	vshr.s32	q2, q2, #16	;
	
	vmull.s16	q5, d19, d1[1]
	vqshrn.s32	d22, q1, #2		;ptr_block[16]
	vmlal.s16	q6, d18, d1[1]	;z6
	vqshrn.s32	d26, q2, #2		;ptr_block[48]
	vmlsl.s16	q5, d18, d0[3]	;z5
	
	;	z4 = tmp4 * (-CONST_4926)  + tmp7 * (-CONST_7373);
	;	z1 = tmp4 * (-CONST_7373)  + tmp7 *   CONST_4926;
	;	z3 = tmp5 * (-CONST_4176)  + tmp6 * (-CONST_20995);
	;	z2 = tmp5 * (-CONST_20995) + tmp6 *   CONST_4176;
	;	z4 += z5;
	;	z3 += z6;
	;	z2 += z5;
	;	z1 += z6;
	;	ptr_block[56] = (short)DESCALE(z4 + z5, CONST_BITS + PASS1_BITS+3);
	;	ptr_block[40] = (short)DESCALE(z3 + z6, CONST_BITS + PASS1_BITS+3);
	;	ptr_block[24] = (short)DESCALE(z2 + z5, CONST_BITS + PASS1_BITS+3);
	;	ptr_block[8]  = (short)DESCALE(z1 + z6, CONST_BITS + PASS1_BITS+3);
	
	vmull.s16	q2, d16, d0[0]
	vmlsl.s16	q3, d16, d1[3]
	vmull.s16	q1, d17, d0[2]
	vmlsl.s16	q4, d17, d1[0]
	vst1.32		d20, [r0], r1
	vmlsl.s16	q2, d15, d1[3]	;z2
	vmlsl.s16	q3, d15, d0[0]	;z3
	vadd.s32	q5, q5, q14		;+RND
	vmlsl.s16	q1, d14, d1[0]	;z1
	vadd.s32	q6, q6, q14		;+RND
	vmlsl.s16	q4, d14, d0[2]	;z4
	
	vadd.s32	q2, q2, q5		;z2 + z5
	vadd.s32	q3, q3, q6		;z3 + z6
	vadd.s32	q1, q1, q6		;z1 + z6
	vadd.s32	q4, q4, q5		;z4 + z5
	
	vshr.s32	q1, q1, #16	
	vshr.s32	q2, q2, #16	
	vqshrn.s32	d21, q1, #2		;ptr_block[8]
	vqshrn.s32	d23, q2, #2		;ptr_block[24]
	vst1.32		d21, [r0], r1
	vshr.s32	q3, q3, #16	
	vst1.32		d22, [r0], r1
	vshr.s32	q4, q4, #16	
	vst1.32		d23, [r0], r1
	vqshrn.s32	d25, q3, #2		;ptr_block[40]
	vst1.32		d24, [r0], r1
	vqshrn.s32	d27, q4, #2		;ptr_block[56]
	
	vst1.32		d25, [r0], r1
	vst1.32		d26, [r0], r1
	vst1.32		d27, [r0]
	subgt		r0, r0, #128-16-8
	
	bgt			COL_LOOP
	
	mov		pc, lr 
	ENDP	;fdct_int32


	END