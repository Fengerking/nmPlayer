;************************************************************************
;									                                    *
;	VisualOn, Inc Confidential and Proprietary, 2011		            *
;								 	                                    *
;***********************************************************************/

	AREA	|.text|, CODE

	EXPORT Bit16Armv7IdctA
	EXPORT Bit16Armv7IdctB
	EXPORT Bit16Armv7IdctC
	
	ALIGN 4		
 macro
 	IRDETO_LOAD_IDCT4x8_COEF		
	mov	r12, #16
	vld1.64	{d8}, [r0], r12		;q4 x0 x1 d8-d9
	vld1.64	{d10}, [r0], r12	;q5 x4 x5 d10-d11
	vld1.64	{d12}, [r0], r12	;q6 x3 x2 d12-d13
	vld1.64	{d14}, [r0], r12	;q7 x7 x6 d14-d15
	vld1.64	{d9}, [r0], r12		;
	vld1.64	{d15}, [r0], r12	;
	vld1.64	{d13}, [r0], r12	;
	vld1.64	{d11}, [r0]		;	
 mend
 macro
 	IRDETO_LOAD_IDCT8x8_COEF		
	vld1.64	{q4}, [r0]!		;q4 x0 x1 d8-d9
	vld1.64	{q5}, [r0]!		;q5 x4 x5 d10-d11
	vld1.64	{q6}, [r0]!		;q6 x3 x2 d12-d13
	vld1.64	{q7}, [r0]!		;q7 x7 x6 d14-d15
	vld1.64	{q8}, [r0]!		;
	vld1.64	{q9}, [r0]!		;
	vld1.64	{q10}, [r0]!	;
	vld1.64	{q11}, [r0]		;	
 mend 	

 macro		
	IRDETO_STR_IDCT   $is_8line
  	if $is_8line > 0						
	vst1.64	{q8}, [r1], r2
	vst1.64	{q9}, [r1], r2		
	vst1.64	{q10},[r1], r2
	vst1.64	{q11},[r1], r2	
	vst1.64	{q4}, [r1], r2
	vst1.64	{q5}, [r1], r2	
	vst1.64	{q6}, [r1], r2
	vst1.64	{q7}, [r1]	
   	else
	vqmovun.s16	d16, q8
	vqmovun.s16	d20, q10
	vqmovun.s16	d8, q4
	vqmovun.s16	d12, q6	
						
	vst1.64	{q8}, [r1], r2	
	vst1.64	{q10},[r1], r2
	vst1.64	{q4}, [r1], r2
	vst1.64	{q6}, [r1], r2
   	endif 	  				
 mend	
  macro
 	IRDETO_Col8_first4line
	vshll.s16	q12, d9, #11		;x1 = Blk[32] << 11;
	vshll.s16	q13, d8, #11
	vadd.s32	q13, q13, q1		;x0 = (x0 << 11) + 128;

;	x4 = W7*x5 + W1*x4;
;	x5 = W7*x4 - W1*x5;

	vmull.s16	q14, d11, D0[0]			;W7*x5
	vmlal.s16	q14, d10, D0[1]			;x4 = W7*x5 + W1*x4;
	
	vmull.s16	q15, d10, D0[0]			;W7*x4
	vmlsl.s16	q15, d11, D0[1]			;x5 = W7*x4 - W1*x5;
	
;	x6 = W3*x7 - W5*x6
;	x7 = W3*x6 - W5*x7;	

	vmull.s16	q4, d14, D0[2]			;W3*x7
	vmlal.s16	q4, d15, D0[3]			;x6 = W3*x7 + W5*x6
	
	vmull.s16	q5, d15, D0[2]			;W3*x6
	vmlsl.s16	q5, d14, D0[3]			;x7 = W3*x6 - W5*x7;
;	x3 = W6*x2 + W2*x3;
;	x2 = W6*x3 - W2*x2;

	vmull.s16	q7, d13, D1[0]			;W6*x2
	vmlal.s16	q7, d12, D1[1]			;x3 = W6*x2 + W2*x3;
	
	vmull.s16	q3, d12, D1[0]			;W6*x3
	vmlsl.s16	q3, d13, D1[1]			;x2 = W6*x3 - W2*x2;
	
;	x8 = x0 + x1;
;	x0 -= x1;
	vadd.s32	q6, q13, q12		;x8 = x0 + x1;
	vsub.s32	q13, q13, q12		;x0 -= x1;
		
;	x1 = x4 + x6;
;	x4 -= x6;
;	x6 = x5 + x7;
;	x5 -= x7;
;	x7 = x8 + x3;
;	x8 -= x3;
;	x3 = x0 + x2;
;	x0 -= x2;

	vadd.s32	q12, q14, q4		;x1 = x4 + x6;
	vsub.s32	q14, q14, q4		;x4 -= x6;
	
	vadd.s32	q4, q15, q5			;x6 = x5 + x7;
	vsub.s32	q15, q15, q5		;x5 -= x7;
	
	vadd.s32	q5, q6, q7			;x7 = x8 + x3;
	vsub.s32	q6, q6, q7			;x8 -= x3;
	
	vadd.s32	q7, q13, q3			;x3 = x0 + x2;
	vsub.s32	q13, q13, q3		;x0 -= x2;

;	x2 = (181 * (x4 + x5) + 128) >> 8;
;	x4 = (181 * (x4 - x5) + 128) >> 8;
					
	vadd.s32	q3, q14, q15
	vmul.s32	q3, q3, D1[1]
	vadd.s32	q3, q3, q1			
	vshr.s32	q3, q3, #8		;x2 = (181 * (x4 + x5) + 128) >> 8;

	vsub.s32	q14, q14, q15	
	vmul.s32	q14, q14, D1[1]
	vadd.s32	q14, q14, q1			
	vshr.s32	q14, q14, #8		;x4 = (181 * (x4 - x5) + 128) >> 8;
		
;	Blk[0] = (idct_t)((x7 + x1) >> 8);
;	Blk[8] = (idct_t)((x3 + x2) >> 8);
;	Blk[16] = (idct_t)((x0 + x4) >> 8);
;	Blk[24] = (idct_t)((x8 + x6) >> 8);
;	Blk[32] = (idct_t)((x8 - x6) >> 8);
;	Blk[40] = (idct_t)((x0 - x4) >> 8);
;	Blk[48] = (idct_t)((x3 - x2) >> 8);
;	Blk[56] = (idct_t)((x7 - x1) >> 8);

	vadd.s32	q15, q5, q12		;Blk[ 0] = (idct_t)((x7 + x1) >> 8);
	vsub.s32	q5, q5, q12			;Blk[56] = (idct_t)((x7 - x1) >> 8);
	vadd.s32	q12, q7, q3			;Blk[8] = (idct_t)((x3 + x2) >> 8);
	vsub.s32	q7, q7, q3			;Blk[48] = (idct_t)((x3 - x2) >> 8);
	vadd.s32	q3, q13, q14		;Blk[16] = (idct_t)((x0 + x4) >> 8);
	vsub.s32	q13, q13, q14		;Blk[40] = (idct_t)((x0 - x4) >> 8);
	vadd.s32	q14, q6, q4			;Blk[24] = (idct_t)((x8 + x6) >> 8);
	vsub.s32	q6, q6, q4			;Blk[32] = (idct_t)((x8 - x6) >> 8);
 mend
 
  macro
 	IRDETO_Col8_last4line

	vmov.s32	q3, #128	
	vshll.s16	q12, d17, #11		;x1 = Blk[32] << 11;
	vshll.s16	q13, d16, #11
	vadd.s32	q13, q13, q3		;x0 = (x0 << 11) + 128;

;	x4 = W7*x5 + W1*x4;
;	x5 = W7*x4 - W1*x5;

	vmull.s16	q14, d23, D0[0]			;W7*x5
	vmlal.s16	q14, d22, D0[1]			;x4 = W7*x5 + W1*x4;
	
	vmull.s16	q15, d22, D0[0]			;W7*x4
	vmlsl.s16	q15, d23, D0[1]			;x5 = W7*x4 - W1*x5;
	
;	x6 = W3*x7 - W5*x6
;	x7 = W3*x6 - W5*x7;	

	vmull.s16	q8, d18, D0[2]			;W3*x7
	vmlal.s16	q8, d19, D0[3]			;x6 = W3*x7 + W5*x6
	
	vmull.s16	q11, d19, D0[2]			;W3*x6
	vmlsl.s16	q11, d18, D0[3]			;x7 = W3*x6 - W5*x7;
;	x3 = W6*x2 + W2*x3;
;	x2 = W6*x3 - W2*x2;

	vmull.s16	q9, d21, D1[0]			;W6*x2
	vmlal.s16	q9, d20, D1[1]			;x3 = W6*x2 + W2*x3;
	
	vmull.s16	q3, d20, D1[0]			;W6*x3
	vmlsl.s16	q3, d21, D1[1]			;x2 = W6*x3 - W2*x2;
	
;	x8 = x0 + x1;
;	x0 -= x1;
	vadd.s32	q10, q13, q12		;x8 = x0 + x1;
	vsub.s32	q13, q13, q12		;x0 -= x1;
		
;	x1 = x4 + x6;
;	x4 -= x6;
;	x6 = x5 + x7;
;	x5 -= x7;
;	x7 = x8 + x3;
;	x8 -= x3;
;	x3 = x0 + x2;
;	x0 -= x2;

	vadd.s32	q12, q14, q8		;x1 = x4 + x6;
	vsub.s32	q14, q14, q8		;x4 -= x6;
	
	vadd.s32	q8, q15, q11			;x6 = x5 + x7;
	vsub.s32	q15, q15, q11		;x5 -= x7;
	
	vadd.s32	q11, q10, q9			;x7 = x8 + x3;
	vsub.s32	q10, q10, q9			;x8 -= x3;
	
	vadd.s32	q9, q13, q3			;x3 = x0 + x2;
	vsub.s32	q13, q13, q3		;x0 -= x2;

;	x2 = (181 * (x4 + x5) + 128) >> 8;
;	x4 = (181 * (x4 - x5) + 128) >> 8;

					
	vadd.s32	q3, q14, q15
	vmul.s32	q3, q3, D1[1]
	vsub.s32	q14, q14, q15	
	vmul.s32	q14, q14, D1[1]
	
	vmov.s32	q15, #128	
		
	vadd.s32	q3, q3, q15			
	vshr.s32	q3, q3, #8		;x2 = (181 * (x4 + x5) + 128) >> 8;
	vadd.s32	q14, q14, q15			
	vshr.s32	q14, q14, #8		;x4 = (181 * (x4 - x5) + 128) >> 8;
		
;	Blk[0] = (idct_t)((x7 + x1) >> 8);
;	Blk[8] = (idct_t)((x3 + x2) >> 8);
;	Blk[16] = (idct_t)((x0 + x4) >> 8);
;	Blk[24] = (idct_t)((x8 + x6) >> 8);
;	Blk[32] = (idct_t)((x8 - x6) >> 8);
;	Blk[40] = (idct_t)((x0 - x4) >> 8);
;	Blk[48] = (idct_t)((x3 - x2) >> 8);
;	Blk[56] = (idct_t)((x7 - x1) >> 8);

	vadd.s32	q15, q11, q12		;Blk[ 0] = (idct_t)((x7 + x1) >> 8);
	vsub.s32	q11, q11, q12			;Blk[56] = (idct_t)((x7 - x1) >> 8);
	vadd.s32	q12, q9, q3			;Blk[8] = (idct_t)((x3 + x2) >> 8);
	vsub.s32	q9, q9, q3			;Blk[48] = (idct_t)((x3 - x2) >> 8);
	vadd.s32	q3, q13, q14		;Blk[16] = (idct_t)((x0 + x4) >> 8);
	vsub.s32	q13, q13, q14		;Blk[40] = (idct_t)((x0 - x4) >> 8);
	vadd.s32	q14, q10, q8			;Blk[24] = (idct_t)((x8 + x6) >> 8);
	vsub.s32	q10, q10, q8			;Blk[32] = (idct_t)((x8 - x6) >> 8);
 mend

 macro
 	IRDETO_Row_8x8line $is_8x8
			
;row tow
	vmov.s32	q3, #8192	
	if $is_8x8>0
	vshll.s16	q12, d17, #8		;x1 = Blk[32] << 8;
	endif
	vshll.s16	q13, d16, #8
	vadd.s32	q13, q13, q3		;x0 = (x0 << 8) + 128;

;	x4 = W7*x5 + W1*x4;
;	x5 = W7*x4 - W1*x5;

	if $is_8x8 > 0
	vmull.s16	q14, d23, D0[0]			;W7*x5
	vmlal.s16	q14, d22, D0[1]			;x4 = W7*x5 + W1*x4;
	
	vmull.s16	q15, d22, D0[0]			;W7*x4
	vmlsl.s16	q15, d23, D0[1]			;x5 = W7*x4 - W1*x5;
	
;	x6 = W3*x7 - W5*x6
;	x7 = W3*x6 - W5*x7;	

	vmull.s16	q8, d18, D0[2]			;W3*x7
	vmlal.s16	q8, d19, D0[3]			;x6 = W3*x7 + W5*x6
	
	vmull.s16	q11, d19, D0[2]			;W3*x6
	vmlsl.s16	q11, d18, D0[3]			;x7 = W3*x6 - W5*x7;
;	x3 = W6*x2 + W2*x3;
;	x2 = W6*x3 - W2*x2;

	vmull.s16	q9, d21, D1[0]			;W6*x2
	vmlal.s16	q9, d20, D1[1]			;x3 = W6*x2 + W2*x3;
	
	vmull.s16	q3, d20, D1[0]			;W6*x3
	vmlsl.s16	q3, d21, D1[1]			;x2 = W6*x3 - W2*x2;
	else
;x5 = 0
;	vmull.s16	q14, d23, D0[0]			;W7*x5
	vmull.s16	q14, d22, D0[1]			;x4 = W7*x5 + W1*x4;
	
	vmull.s16	q15, d22, D0[0]			;W7*x4
;	vmlsl.s16	q15, d23, D0[1]			;x5 = W7*x4 - W1*x5;
	

;x6 = 0
	vmull.s16	q8, d18, D0[2]			;W3*x7
;	vmlal.s16	q8, d19, D0[3]			;x6 = W3*x7 + W5*x6
	
; x7 = -x7	
;	vmull.s16	q11, d19, D0[2]			;W3*x6
	vmull.s16	q11, d18, D0[3]			;x7 = W3*x6 - W5*x7;

;x2 = 0
;	vmull.s16	q9, d21, D1[0]			;W6*x2
	vmull.s16	q9, d20, D1[1]			;x3 = W6*x2 + W2*x3;
	
	vmull.s16	q3, d20, D1[0]			;W6*x3
;	vmlsl.s16	q3, d21, D1[1]			;x2 = W6*x3 - W2*x2;

	endif
	
	;((x4, x5, x6, x7, x3, x2) + 4)>>3
	vmov.s32	q10, #4
	vadd.s32	q14, q14, q10
	vadd.s32	q15, q15, q10
	vadd.s32	q8, q8, q10
	if $is_8x8 > 0
	vadd.s32	q11, q11, q10
	else
	vsub.s32	q11, q10, q11				;x7 = 4 - x7
	endif
	vadd.s32	q9, q9, q10
	vadd.s32	q3, q3, q10					
	vshr.s32	q14, q14, #3
	vshr.s32	q15, q15, #3	
	vshr.s32	q8, q8, #3
	vshr.s32	q11, q11, #3
	vshr.s32	q9, q9, #3
	vshr.s32	q3, q3, #3		
		
;	x8 = x0 + x1;
;	x0 -= x1;
	if $is_8x8 > 0
	vadd.s32	q10, q13, q12		;x8 = x0 + x1;
	vsub.s32	q13, q13, q12		;x0 -= x1;
	endif
		
;	x1 = x4 + x6;
;	x4 -= x6;
;	x6 = x5 + x7;
;	x5 -= x7;
;	x7 = x8 + x3;
;	x8 -= x3;
;	x3 = x0 + x2;
;	x0 -= x2;

	vadd.s32	q12, q14, q8		;x1 = x4 + x6;
	vsub.s32	q14, q14, q8		;x4 -= x6;
	
	vadd.s32	q8, q15, q11			;x6 = x5 + x7;
	vsub.s32	q15, q15, q11		;x5 -= x7;

	if $is_8x8 > 0
	vadd.s32	q11, q10, q9			;x7 = x8 + x3;
	vsub.s32	q10, q10, q9			;x8 -= x3;
	else
	vadd.s32	q11, q13, q9			;x7 = x8 + x3;
	vsub.s32	q10, q13, q9			;x8 -= x3;
	endif
	
	
	vadd.s32	q9, q13, q3			;x3 = x0 + x2;
	vsub.s32	q13, q13, q3		;x0 -= x2;

;	x2 = (181 * (x4 + x5) + 128) >> 8;
;	x4 = (181 * (x4 - x5) + 128) >> 8;

					
	vadd.s32	q3, q14, q15
	vmul.s32	q3, q3, D1[1]
	vsub.s32	q14, q14, q15	
	vmul.s32	q14, q14, D1[1]
	
	vmov.s32	q15, #128	
	vadd.s32	q3, q3, q15			
	vshr.s32	q3, q3, #8		;x2 = (181 * (x4 + x5) + 128) >> 8;
	vadd.s32	q14, q14, q15			
	vshr.s32	q14, q14, #8		;x4 = (181 * (x4 - x5) + 128) >> 8;
		
;	Blk[0] = (idct_t)((x7 + x1) >> 8);
;	Blk[8] = (idct_t)((x3 + x2) >> 8);
;	Blk[16] = (idct_t)((x0 + x4) >> 8);
;	Blk[24] = (idct_t)((x8 + x6) >> 8);
;	Blk[32] = (idct_t)((x8 - x6) >> 8);
;	Blk[40] = (idct_t)((x0 - x4) >> 8);
;	Blk[48] = (idct_t)((x3 - x2) >> 8);
;	Blk[56] = (idct_t)((x7 - x1) >> 8);

	vadd.s32	q15, q11, q12		;Blk[ 0] = (idct_t)((x7 + x1) >> 8);
	vsub.s32	q11, q11, q12			;Blk[56] = (idct_t)((x7 - x1) >> 8);
	vadd.s32	q12, q9, q3			;Blk[8] = (idct_t)((x3 + x2) >> 8);
	vsub.s32	q9, q9, q3			;Blk[48] = (idct_t)((x3 - x2) >> 8);
	vadd.s32	q3, q13, q14		;Blk[16] = (idct_t)((x0 + x4) >> 8);
	vsub.s32	q13, q13, q14		;Blk[40] = (idct_t)((x0 - x4) >> 8);
	vadd.s32	q14, q10, q8			;Blk[24] = (idct_t)((x8 + x6) >> 8);
	vsub.s32	q10, q10, q8			;Blk[32] = (idct_t)((x8 - x6) >> 8);
	
	
	vshrn.s32	d2, q10, #14
	vshrn.s32	d3, q13, #14
	vshrn.s32	d4, q9, #14
	vshrn.s32	d5, q11, #14
			
	vshrn.s32	d16, q15, #14
	vshrn.s32	d18, q12, #14	
	vshrn.s32	d20, q3, #14
	vshrn.s32	d22, q14, #14	

;row one

	vmov.s32	q3, #8192	
	if $is_8x8 > 0
	vshll.s16	q12, d9, #8		;x1 = Blk[32] << 8;
	endif

	vshll.s16	q13, d8, #8
	vadd.s32	q13, q13, q3		;x0 = (x0 << 8) + 8192;

;	x4 = W7*x5 + W1*x4;
;	x5 = W7*x4 - W1*x5;
	if $is_8x8 > 0

	vmull.s16	q14, d11, D0[0]			;W7*x5
	vmlal.s16	q14, d10, D0[1]			;x4 = W7*x5 + W1*x4;
	
	vmull.s16	q15, d10, D0[0]			;W7*x4
	vmlsl.s16	q15, d11, D0[1]			;x5 = W7*x4 - W1*x5;
	
;	x6 = W3*x7 - W5*x6
;	x7 = W3*x6 - W5*x7;	

	vmull.s16	q4, d14, D0[2]			;W3*x7
	vmlal.s16	q4, d15, D0[3]			;x6 = W3*x7 + W5*x6
	
	vmull.s16	q5, d15, D0[2]			;W3*x6
	vmlsl.s16	q5, d14, D0[3]			;x7 = W3*x6 - W5*x7;
;	x3 = W6*x2 + W2*x3;
;	x2 = W6*x3 - W2*x2;

	vmull.s16	q7, d13, D1[0]			;W6*x2
	vmlal.s16	q7, d12, D1[1]			;x3 = W6*x2 + W2*x3;
	
	vmull.s16	q3, d12, D1[0]			;W6*x3
	vmlsl.s16	q3, d13, D1[1]			;x2 = W6*x3 - W2*x2;
	else
;	vmull.s16	q14, d11, D0[0]			;W7*x5
	vmull.s16	q14, d10, D0[1]			;x4 = W7*x5 + W1*x4;
	
	vmull.s16	q15, d10, D0[0]			;W7*x4
;	vmlsl.s16	q15, d11, D0[1]			;x5 = W7*x4 - W1*x5;
	

	vmull.s16	q4, d14, D0[2]			;W3*x7
;	vmlal.s16	q4, d15, D0[3]			;x6 = W3*x7 + W5*x6
	
;	vmull.s16	q5, d15, D0[2]			;W3*x6
	vmull.s16	q5, d14, D0[3]			;x7 = W3*x6 - W5*x7;

;	vmull.s16	q7, d13, D1[0]			;W6*x2
	vmull.s16	q7, d12, D1[1]			;x3 = W6*x2 + W2*x3;
	
	vmull.s16	q3, d12, D1[0]			;W6*x3
;	vmlsl.s16	q3, d13, D1[1]			;x2 = W6*x3 - W2*x2;

	endif
	
	;((x4, x5, x6, x7, x3, x2) + 4)>>3
	vmov.s32	q6, #4
	vadd.s32	q14, q14, q6
	vadd.s32	q15, q15, q6
	vadd.s32	q4, q4, q6
	if $is_8x8 > 0
	vadd.s32	q5, q5, q6
	else
	vsub.s32	q5, q6, q5
	endif

	vadd.s32	q7, q7, q6
	vadd.s32	q3, q3, q6
	vshr.s32	q14, q14, #3
	vshr.s32	q15, q15, #3	
	vshr.s32	q4, q4, #3
	vshr.s32	q5, q5, #3
	vshr.s32	q7, q7, #3
	vshr.s32	q3, q3, #3	
			
;	x8 = x0 + x1;
;	x0 -= x1;
	if $is_8x8 > 0
	vadd.s32	q6, q13, q12		;x8 = x0 + x1;
	vsub.s32	q13, q13, q12		;x0 -= x1;
	endif

		
;	x1 = x4 + x6;
;	x4 -= x6;
;	x6 = x5 + x7;
;	x5 -= x7;
;	x7 = x8 + x3;
;	x8 -= x3;
;	x3 = x0 + x2;
;	x0 -= x2;

	vadd.s32	q12, q14, q4		;x1 = x4 + x6;
	vsub.s32	q14, q14, q4		;x4 -= x6;
	
	vadd.s32	q4, q15, q5			;x6 = x5 + x7;
	vsub.s32	q15, q15, q5		;x5 -= x7;

	if $is_8x8 > 0
	vadd.s32	q5, q6, q7			;x7 = x8 + x3;
	vsub.s32	q6, q6, q7			;x8 -= x3;
	else
	vadd.s32	q5, q13, q7			;x7 = x8 + x3;
	vsub.s32	q6, q13, q7			;x8 -= x3;
	endif
	
	
	vadd.s32	q7, q13, q3			;x3 = x0 + x2;
	vsub.s32	q13, q13, q3		;x0 -= x2;

;	x2 = (181 * (x4 + x5) + 128) >> 8;
;	x4 = (181 * (x4 - x5) + 128) >> 8;
		
	vadd.s32	q3, q14, q15
	vmul.s32	q3, q3, D1[1]
	vsub.s32	q14, q14, q15	
	vmul.s32	q14, q14, D1[1]
		
	vmov.s32	q15, #128		
	vadd.s32	q3, q3, q15			
	vshr.s32	q3, q3, #8		;x2 = (181 * (x4 + x5) + 128) >> 8;
	vadd.s32	q14, q14, q15			
	vshr.s32	q14, q14, #8		;x4 = (181 * (x4 - x5) + 128) >> 8;
		
;	Blk[0] = (idct_t)((x7 + x1) >> 8);
;	Blk[8] = (idct_t)((x3 + x2) >> 8);
;	Blk[16] = (idct_t)((x0 + x4) >> 8);
;	Blk[24] = (idct_t)((x8 + x6) >> 8);
;	Blk[32] = (idct_t)((x8 - x6) >> 8);
;	Blk[40] = (idct_t)((x0 - x4) >> 8);
;	Blk[48] = (idct_t)((x3 - x2) >> 8);
;	Blk[56] = (idct_t)((x7 - x1) >> 8);

	vadd.s32	q15, q5, q12		;Blk[ 0] = (idct_t)((x7 + x1) >> 8);
	vsub.s32	q5, q5, q12			;Blk[56] = (idct_t)((x7 - x1) >> 8);
	vadd.s32	q12, q7, q3			;Blk[8] = (idct_t)((x3 + x2) >> 8);
	vsub.s32	q7, q7, q3			;Blk[48] = (idct_t)((x3 - x2) >> 8);
	vadd.s32	q3, q13, q14		;Blk[16] = (idct_t)((x0 + x4) >> 8);
	vsub.s32	q13, q13, q14		;Blk[40] = (idct_t)((x0 - x4) >> 8);
	vadd.s32	q14, q6, q4			;Blk[24] = (idct_t)((x8 + x6) >> 8);
	vsub.s32	q6, q6, q4			;Blk[32] = (idct_t)((x8 - x6) >> 8);
	
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
 mend
 
;  macro	
IRDETO_Col8_4x8

	IRDETO_LOAD_IDCT4x8_COEF

	IRDETO_Col8_first4line
		
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
	
;	Transpose	
			
	VTRN.16 q8, q9
	VTRN.16 q10, q11
	VTRN.16 q4, q5
	VTRN.16 q6, q7
	VTRN.32 q8, q10
	VTRN.32 q9, q11
	VTRN.32 q4, q6
	VTRN.32 q5, q7
	
									;q8  x0 x1 d16-d17
	vswp.s32	d19, d23			;q11 x4 x5 d18-d19
									;q10 x3 x2 d20-d21	
									;q9  x7 x6 d22-d23
	vswp.s64	q9, q11									
		
									;q4 x0 x1 d8-d9
	vswp.s32	d11, d15			;q5 x4 x5 d10-d11
									;q6 x3 x2 d12-d13	
									;q7 x7 x6 d14-d15								
;q8  x0 x1 d16-d17
;q11 x4 x5 d22-d23
;q10 x3 x2 d20-d21	
;q9  x7 x6 d18-d19	
	
;q4 x0 x1 d8-d9
;q5 x4 x5 d10-d11
;q6 x3 x2 d12-d13
;q7 x7 x6 d14-d15
										
	IRDETO_Row_8x8line 0
		
;	Transpose	
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
 	mov		pc,lr				
; mend

;  macro	
IRDETO_Col8_8x8
 
	IRDETO_LOAD_IDCT8x8_COEF
		
	vswp.s32	d9, d16				;q8  x0 x1 d16-17
	vswp.s32	d15, d18			;q11 x4 x5 d22-23
	vswp.s32	d13, d20			;q10 x3 x2 d20-21	
	vswp.s32	d11, d22			;q9  x7 x6 d18-19		
	
	
	IRDETO_Col8_first4line
		
	vshrn.s32	d2, q15, #8
	vshrn.s32	d3, q12, #8	
	vshrn.s32	d4, q3, #8
	vshrn.s32	d5, q14, #8
	
	vshrn.s32	d8, q6, #8
	vshrn.s32	d12, q7, #8
	vshrn.s32	d14, q5, #8
	vshrn.s32	d10, q13, #8		

	IRDETO_Col8_last4line
	
	
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
	
;	Transpose	
			
	VTRN.16 q8, q9
	VTRN.16 q10, q11
	VTRN.16 q4, q5
	VTRN.16 q6, q7
	VTRN.32 q8, q10
	VTRN.32 q9, q11
	VTRN.32 q4, q6
	VTRN.32 q5, q7
	
									;q8  x0 x1 d16-d17
	vswp.s32	d19, d23			;q11 x4 x5 d18-d19
									;q10 x3 x2 d20-d21	
									;q9  x7 x6 d22-d23
	vswp.s64	q9, q11									
		
									;q4 x0 x1 d8-d9
	vswp.s32	d11, d15			;q5 x4 x5 d10-d11
									;q6 x3 x2 d12-d13	
									;q7 x7 x6 d14-d15								
;q8  x0 x1 d16-d17
;q11 x4 x5 d22-d23
;q10 x3 x2 d20-d21	
;q9  x7 x6 d18-d19	
	
;q4 x0 x1 d8-d9
;q5 x4 x5 d10-d11
;q6 x3 x2 d12-d13
;q7 x7 x6 d14-d15
									
	IRDETO_Row_8x8line	1
		
;	Transpose	
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
 	mov		pc,lr					
; mend	

	ALIGN 4
Bit16Armv7IdctA PROC
; r0 = Block, r1 = dst, r2 = dst_stride, r3 = Src, r12 = [sp] = src_stride
    PUSH     {lr}
	ldr			r12, =IRDETO_W_table
	mov			r2, r2, lsl #1
	vld1.s32 	{q0}, [r12]	  
	vmov.s32	q1, #128	

	bl IRDETO_Col8_4x8  	
IRDETO_IDCT_CPY_NOSRC_4x8	
	IRDETO_STR_IDCT 1	
	
    POP      {pc}

	ENDP

	ALIGN 4
Bit16Armv7IdctB PROC
; r0 = Block, r1 = dst, r2 = dst_stride, r3 = Src, r12 = [sp] = src_stride
    PUSH        {lr}
	ldr			r12, =IRDETO_W_table
	mov         r2, r2, lsl #1
	vld1.s32 	{q0}, [r12]	  
	vmov.s32	q1, #128
	
	bl IRDETO_Col8_8x8  		
IRDETO_IDCT_CPY_NOSRC_8x8	
	IRDETO_STR_IDCT 1	
	
    POP      {pc}
	ENDP

Bit16Armv7IdctC PROC	
	stmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r14}
	ldr		r14,[sp,#36]	;SrcPitch
	mov     r2, r2, lsl #1
	mov     r4, #8
	mov     r5, r0, lsl #16
	mov     r0, r5, lsr #16
	orr     r0, r0, r5  
IRDETO_ARMV7_CLOOP
	str     r0, [r1, #12] 
	str     r0, [r1, #8] 
	str     r0, [r1, #4]
	str     r0, [r1], r2
	subs    r4, r4, #1
	bgt     IRDETO_ARMV7_CLOOP
		              
	ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} ;retrun end arm_transc8x8          
;endp

	ALIGN 4	
IRDETO_W_table	
		dcd 0x0B190235			;w7 = D0.S16[0]	w1 = D0.S16[1]		
		dcd 0x06490968			;w3 = D0.S16[2] w5 = D0.S16[3]			
		dcd 0x0A740454			;w6 = D1.S16[0] w2 = D1.S16[1]		
		dcd 0x000000b5			;181= D1.S32[1]	 	
					
	END