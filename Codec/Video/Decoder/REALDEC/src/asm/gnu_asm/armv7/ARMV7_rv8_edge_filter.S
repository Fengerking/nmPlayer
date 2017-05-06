@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2005		            *
@	written by John							 	                                    *
@***********************************************************************/

#include "../../../voRVDecID.h"


	@AREA    |.text|, CODE, READONLY
	.section .text

	.global	ARMV7_rv8_edge_filter
		
 .macro edge_filter_V		@q0~7, q14, q15
@diff = [(A - D) + ((C - B) << 2)] >> 3@
@A d0, B d2, C d4, D d6 	
@A d1, B d3, C d5, D d7		
	vsubl.u8	q8, d0, d6
	vsubl.u8	q9, d1, d7	@A-D
	vsubl.u8	q10, d4, d2
	vsubl.u8	q11, d5, d3	@C-B
	vshl.s16 q10, q10, #2		@(C - B) << 2
	vshl.s16 q11, q11, #2		@(C - B) << 2	
	vadd.s16 q8, q8, q10		@[(A - D) + ((C - B) << 2)]	
	vadd.s16 q9, q9, q11		@[(A - D) + ((C - B) << 2)]		
	vqshrn.s16 d16, q8, #3
	vqshrn.s16 d17, q9, #3		@q8 = diff
@A d8, B d10, C d12, D d14 
@A d9, B d11, C d13, D d15
	vsubl.u8	q11, d8, d14
	vsubl.u8	q12, d9, d15	@A-D
	vsubl.u8	q9, d12, d10
	vsubl.u8	q10, d13, d11	@C-B
	vshl.s16 q9, q9, #2		@(C - B) << 2
	vshl.s16 q10, q10, #2		@(C - B) << 2	
	vadd.s16 q11, q11, q9		@[(A - D) + ((C - B) << 2)]	
	vadd.s16 q12, q12, q10		@[(A - D) + ((C - B) << 2)]		
	vqshrn.s16 d22, q11, #3
	vqshrn.s16 d23, q12, #3		@q11 = diff						
@clip3(diff, +str, -str)			
	vneg.s8		q9,q14
	vmin.s8		q8,q8,q14
	vmax.s8		q8,q8,q9
	vneg.s8		q12,q15
	vmin.s8		q11,q11,q15
	vmax.s8		q11,q11,q12
@q0~7, q8, q11: d2,4->d16 d3,5->d17 d10,12->d22 d11,13->d23@ q1~2, q5~6
@lpr[B]  = clip255[lpr[B] + diff]@
@lpr[C]  = clip255[lpr[C] - diff]@
	vcge.s8		q9, q8,#0	
	vclt.s8		q10, q8,#0
	vabs.s8	 	q8,q8	
	vand.u8	 	q9, q9, q8	
	vand.u8	 	q10, q10, q8
	vqadd.u8	q1, q1, q9
	vqsub.u8	q2, q2, q9			
	vqsub.u8 	q1, q1, q10
	vqadd.u8	q2, q2, q10
	
	vcge.s8		q12, q11,#0	
	vclt.s8		q9, q11,#0
	vabs.s8	 	q11,q11	
	vand.u8	 	q12, q12, q11	
	vand.u8	 	q9, q9, q11
	vqadd.u8	q5, q5, q12
	vqsub.u8	q6, q6, q12			
	vqsub.u8 	q5, q5, q9
	vqadd.u8	q6, q6, q9
 .endm

 .macro edge_filter_H		@q6, q7, q0~5,  q14, q15
@diff = [(A - D) + ((C - B) << 2)] >> 3@
@A q6, B q7, C q0, D q1	
	vsubl.u8	q8, d12, d2
	vsubl.u8	q9, d13, d3	@A-D
	vsubl.u8	q10, d0, d14
	vsubl.u8	q11, d1, d15	@C-B
	vshl.s16 q10, q10, #2		@(C - B) << 2
	vshl.s16 q11, q11, #2		@(C - B) << 2	
	vadd.s16 q8, q8, q10		@[(A - D) + ((C - B) << 2)]	
	vadd.s16 q9, q9, q11		@[(A - D) + ((C - B) << 2)]		
	vqshrn.s16 d16, q8, #3
	vqshrn.s16 d17, q9, #3		@q8 = diff
@A q2, B q3, C q4, D q5
	vsubl.u8	q11, d4, d10
	vsubl.u8	q12, d5, d11	@A-D
	vsubl.u8	q9, d8, d6
	vsubl.u8	q10, d9, d7	@C-B
	vshl.s16 q9, q9, #2		@(C - B) << 2
	vshl.s16 q10, q10, #2		@(C - B) << 2	
	vadd.s16 q11, q11, q9		@[(A - D) + ((C - B) << 2)]	
	vadd.s16 q12, q12, q10		@[(A - D) + ((C - B) << 2)]		
	vqshrn.s16 d22, q11, #3
	vqshrn.s16 d23, q12, #3		@q11 = diff				
@clip3(diff, +str, -str)			
	vneg.s8		q9,q14
	vmin.s8		q8,q8,q14
	vmax.s8		q8,q8,q9
	vneg.s8		q12,q15
	vmin.s8		q11,q11,q15
	vmax.s8		q11,q11,q12
@q0~7, 	      q8, q11: d2,4->d16 d3,5->d17 d10,12->d22 d11,13->d23@ q1~2, q5~6
@q6,q7, q0~5, q8, q11: q6~7,q0~1->q8 q2~5->q11
@lpr[B]  = clip255[lpr[B] + diff]@
@lpr[C]  = clip255[lpr[C] - diff]@
	vcge.s8		q9, q8,#0	
	vclt.s8		q10, q8,#0
	vabs.s8	 	q8,q8	
	vand.u8	 	q9, q9, q8	
	vand.u8	 	q10, q10, q8
	vqadd.u8	q7, q7, q9
	vqsub.u8	q0, q0, q9			
	vqsub.u8 	q7, q7, q10
	vqadd.u8	q0, q0, q10
	
	vcge.s8		q12, q11,#0	
	vclt.s8		q9, q11,#0
	vabs.s8	 	q11,q11	
	vand.u8	 	q12, q12, q11	
	vand.u8	 	q9, q9, q11
	vqadd.u8	q3, q3, q12
	vqsub.u8	q4, q4, q12			
	vqsub.u8 	q3, q3, q9
	vqadd.u8	q4, q4, q9
 .endm		
@void rv8_edge_filter(U8 *src, stride, Width, Height,U8	*pStrengthV, U8	*pStrengthH)
@{
@	I32 i, j, Width2@
@	U8 *pV, *pH@
@	U8 *pCur,*lpr@
@	pV = pStrengthV@
@	pH = pStrengthH@
@	Width2 = Width & 0x0ff0@
@	for (j = 0@ j < Height@ j += 8)
@	{
@		pCur = src + j*stride@
@		lpr = pCur -2@
@		//need write an 16x8 functions
@		for (i = Width2@ i > 0 @ i -= 16)
@		{
@			//first 8pixel
@			rv8filter(&pCur[0],pV[0], 1, stride)@
@			rv8filter(&lpr[4], pH[1], stride, 1)@
@			pCur += 16@
@			pV += 4@
@			lpr += 16@	
@			pH += 4@
@			//end first 8pixel
@		}
@		if(Width%16)
@		{
@			//need write an 8x8 functions
@			//only 8pixel
@			rv8filter(&pCur[0],pV[0], 1, stride)@
@			rv8filter(&lpr[0], pH[0], stride, 1)@
@			pCur += 8@
@			pV += 2@
@			lpr += 8@	
@			pH += 2@
@		}
@		//need write an 2x2 functions
@		//only the left 2pixel of H
@		rv8filter(&lpr[0], pH[0], stride, 1)@
@		rv8filter(&lpr[0+4*stride], pH[0+(Width/4)+1], stride, 1)@
@		pH += 1@
@		pV = pV + (Width/4)@
@		pH = pH + (Width/4)+1@
@	}
@} /* rv8_edge_filter */
ARMV7_rv8_edge_filter: @PROC
@(U8 *src, stride, Width, Height,U8 *pStrengthV, U8*pStrengthH)
@     r0,  r1,     r2,    r3,     [sp, #0x30],   [sp, #0x34] 
@r6 can use in this function  
	stmdb       sp!, {r4 - r11, lr}		
	sub         sp, sp, #0xC
	ldr         r4, [sp, #0x30]	@pStrengthV
	ldr         r5, [sp, #0x34]	@pStrengthH
	sub	r0, r0, #2
	tst         r2, #0xF
	mov	r6, #0
	movne	r6, #2	
		
@	I32 i, j, Width2@
@	U8 *pV, *pH@
@	U8 *pCur,*lpr@
@	pV = pStrengthV@
@	pH = pStrengthH@
@	Width2 = Width & 0x0ff0@
@	for (j = 0@ j < Height@ j += 8)
@	{
@		pCur = src + j*stride@
@		lpr = pCur -2@
rv8_edge_filter_big_loop:
@r0~6
	and	r14, r2, #0xFF0	@Width2		
	mov	r12, r0	
	vmov.u64 q13, #0		
@		//need write an 16x8 functions
@		for (i = Width2@ i > 0 @ i -= 16)
@		{
@			//first 8pixel
@			rv8filter(&pCur[0],pV[0], 1, stride)@
@			rv8filter(&lpr[4], pH[1], stride, 1)@
@			pCur += 16@
@			pV += 4@
@			lpr += 16@	
@			pH += 4@
@			//end first 8pixel
@		}
rv8_edge_filter_lit_loop:
@ pCur = r11@ pV = r4@ pH = r5@ stride = r1@
@16x8	r7, r8, r9, r10, r11	@big r0~6, @lit r12, r14
	mov	r11, r12
	add	r7, r4, r6
	ldr	r8,[r7, r2, lsr #2]	
	ldr	r7,[r4], #4
	add	r9, r5, r6		
	ldr	r10,[r9, r2, lsr #2]	
	ldr	r9,[r5], #4			
@V
	vld1.u8 {q0}, [r11], r1		@A d0 lTemp0=  pSrc[-2~5] d1 =  pSrc[6~13]
	vld1.u8 {q1}, [r11], r1		@B d2 lTemp0=  pSrc[-2~5] d3 =  pSrc[6~13]	
	vld1.u8 {q2}, [r11], r1		@C d4 lTemp0=  pSrc[-2~5] d5 =  pSrc[6~13]
	vld1.u8 {q3}, [r11], r1		@D d6 lTemp0=  pSrc[-2~5] d7 =  pSrc[6~13]
	
	vld1.u8 {q4}, [r11], r1		@A d8 lTemp0=  pSrc[-2~5] d9 =  pSrc[6~13]
	vld1.u8 {q5}, [r11], r1		@B d10 lTemp0= pSrc[-2~5] d11 = pSrc[6~13]	
	vld1.u8 {q6}, [r11], r1		@C d12 lTemp0= pSrc[-2~5] d13 = pSrc[6~13]
	vld1.u8 {q7}, [r11]		@D d14 lTemp0= pSrc[-2~5] d15 = pSrc[6~13]
	
	vtrn.8 q0, q1
	vtrn.8 q2, q3
	vtrn.8 q4, q5
	vtrn.8 q6, q7
	vtrn.16 q0, q2
	vtrn.16 q1, q3
	vtrn.16 q4, q6
	vtrn.16 q5, q7
	vtrn.32 q0, q4
	vtrn.32 q1, q5
	vtrn.32 q2, q6
	vtrn.32 q3, q7
	
	vdup.32		q14,r7 	
	vdup.32		q15,r8	
	vtrn.8		d28,d29
	vtrn.8		d30,d31
	vtrn.32		d28,d30
	vtrn.32		d29,d31	
	vtrn.16		d28,d30
	vtrn.16		d29,d31	
	vswp d29, d30					
@	str = q14, q15
@q0~7, q14, q15		
	edge_filter_V	
					
@
	vtrn.8 q0, q1
	vtrn.8 q2, q3
	vtrn.8 q4, q5
	vtrn.8 q6, q7
	vtrn.16 q0, q2
	vtrn.16 q1, q3
	vtrn.16 q4, q6
	vtrn.16 q5, q7
	vtrn.32 q0, q4
	vtrn.32 q1, q5
	vtrn.32 q2, q6
	vtrn.32 q3, q7	

	sub	r11, r11, r1
	vst1.u8 {q6}, [r11], r1		@C d12 lTemp0= pSrc[-2~5] d13 = pSrc[6~13]
	vst1.u8 {q7}, [r11]		@D d14 lTemp0= pSrc[-2~5] d15 = pSrc[6~13]
@H	
	sub	r11, r12, r1, lsl #1	
	vld1.u8 {q6}, [r11], r1		@A d12 lTemp0= pSrc[-2~5] d13 = pSrc[6~13]
	vld1.u8 {q7}, [r11]		@B d14 lTemp0= pSrc[-2~5] d15 = pSrc[6~13]
				
	vdup.32		q14,r9 	
	vdup.32		q15,r10	
	vtrn.8		d28,d29
	vtrn.8		d30,d31
	vtrn.32		d28,d30
	vtrn.32		d29,d31	
	vtrn.16		d28,d30
	vtrn.16		d29,d31	
	vswp d29, d30
	vtrn.32		d28,d30
	vtrn.32		d29,d31
					
	vext.u8 q14, q13, q14, #14
	vswp d26, d27		
	vext.u8 q15, q13, q15, #14
	
	lsr	r10, r10, #24
	orr	r10, r10, r10, lsl #8		
	lsr	r9, r9, #24
	orr	r9, r9, r9, lsl #8	
	vmov.u16 d27[3], r9
	vmov.u16 d26[3], r10	
@	str = q14, q15

@	q6, q7, q0~5,  q14, q15
	edge_filter_H	

	vst1.u8 {q7}, [r11], r1		@B d2 lTemp0=  pSrc[-2~5] d3 =  pSrc[6~13]	
	vst1.u8 {q0}, [r11], r1		@C d4 lTemp0=  pSrc[-2~5] d5 =  pSrc[6~13]
	vst1.u8 {q1}, [r11], r1		@D d6 lTemp0=  pSrc[-2~5] d7 =  pSrc[6~13]

	vst1.u8 {q2}, [r11], r1		@A d8 lTemp0=  pSrc[-2~5] d9 =  pSrc[6~13]
	vst1.u8 {q3}, [r11], r1		@B d10 lTemp0= pSrc[-2~5] d11 = pSrc[6~13]	
	vst1.u8 {q4}, [r11], r1		@C d12 lTemp0= pSrc[-2~5] d13 = pSrc[6~13]
	vst1.u8 {q5}, [r11]		@D d14 lTemp0= pSrc[-2~5] d15 = pSrc[6~13]
	
	subs        r14, r14, #16
	add	    r12, r12, #16	
	bne         rv8_edge_filter_lit_loop
@		if(Width%16)
@		{
@			//need write an 8x8 functions
@			//only 8pixel
@			rv8filter(&pCur[0],pV[0], 1, stride)@
@			rv8filter(&lpr[0], pH[0], stride, 1)@
@			pCur += 8@
@			pV += 2@
@			lpr += 8@	
@			pH += 2@
@		}
	tst         r2, #0xF
	beq         ONLY_luma_skip
@ pCur = r11@ pV = r4@ pH = r5@ stride = r1@	
@8x8			@@@@r0~6, r11, r12, r14		

	mov	r11, r12
	add	r7, r4, r6
	ldr	r8,[r7, r2, lsr #2]	
	ldr	r7,[r4], #4
	add	r9, r5, r6		
	ldr	r10,[r9, r2, lsr #2]	
	ldr	r9,[r5], #4			
@V
	vld1.u8 {d0}, [r11], r1		@A d0 lTemp0=  pSrc[-2~5] d1 =  pSrc[6~13]
	vld1.u8 {d2}, [r11], r1		@B d2 lTemp0=  pSrc[-2~5] d3 =  pSrc[6~13]	
	vld1.u8 {d4}, [r11], r1		@C d4 lTemp0=  pSrc[-2~5] d5 =  pSrc[6~13]
	vld1.u8 {d6}, [r11], r1		@D d6 lTemp0=  pSrc[-2~5] d7 =  pSrc[6~13]
	
	vld1.u8 {d8}, [r11], r1		@A d8 lTemp0=  pSrc[-2~5] d9 =  pSrc[6~13]
	vld1.u8 {d10}, [r11], r1		@B d10 lTemp0= pSrc[-2~5] d11 = pSrc[6~13]	
	vld1.u8 {d12}, [r11], r1		@C d12 lTemp0= pSrc[-2~5] d13 = pSrc[6~13]
	vld1.u8 {d14}, [r11]		@D d14 lTemp0= pSrc[-2~5] d15 = pSrc[6~13]
	
	vtrn.8 q0, q1
	vtrn.8 q2, q3
	vtrn.8 q4, q5
	vtrn.8 q6, q7
	vtrn.16 q0, q2
	vtrn.16 q1, q3
	vtrn.16 q4, q6
	vtrn.16 q5, q7
	vtrn.32 q0, q4
	vtrn.32 q1, q5
	vtrn.32 q2, q6
	vtrn.32 q3, q7
	
	vdup.32		q14,r7 	
	vdup.32		q15,r8	
	vtrn.8		d28,d29
	vtrn.8		d30,d31
	vtrn.32		d28,d30
	vtrn.32		d29,d31	
	vtrn.16		d28,d30
	vtrn.16		d29,d31	
	vswp d29, d30					
@	str = q14, q15
@q0~7, q14, q15		
	edge_filter_V	
					
@
	vtrn.8 q0, q1
	vtrn.8 q2, q3
	vtrn.8 q4, q5
	vtrn.8 q6, q7
	vtrn.16 q0, q2
	vtrn.16 q1, q3
	vtrn.16 q4, q6
	vtrn.16 q5, q7
	vtrn.32 q0, q4
	vtrn.32 q1, q5
	vtrn.32 q2, q6
	vtrn.32 q3, q7	

	sub	r11, r11, r1
	vst1.u8 {d12}, [r11], r1		@C d12 lTemp0= pSrc[-2~5] d13 = pSrc[6~13]
	vst1.u8 {d14}, [r11]		@D d14 lTemp0= pSrc[-2~5] d15 = pSrc[6~13]
@H	
	sub	r11, r12, r1, lsl #1	
	vld1.u8 {d12}, [r11], r1		@A d12 lTemp0= pSrc[-2~5] d13 = pSrc[6~13]
	vld1.u8 {d14}, [r11]		@B d14 lTemp0= pSrc[-2~5] d15 = pSrc[6~13]
				
	vdup.32		q14,r9 	
	vdup.32		q15,r10	
	vtrn.8		d28,d29
	vtrn.8		d30,d31
	vtrn.32		d28,d30
	vtrn.32		d29,d31	
	vtrn.16		d28,d30
	vtrn.16		d29,d31	
	vswp d29, d30
	vtrn.32		d28,d30
	vtrn.32		d29,d31
					
	vext.u8 q14, q13, q14, #14
	vswp d26, d27		
	vext.u8 q15, q13, q15, #14

	@do this is for do 16x2
	uxth r9, r9
	uxth r10, r10

@	str = q14, q15

@	q6, q7, q0~5,  q14, q15
	edge_filter_H	

	vst1.u8 {d14}, [r11], r1		@B d2 lTemp0=  pSrc[-2~5] d3 =  pSrc[6~13]	
	vst1.u8 {d0}, [r11], r1		@C d4 lTemp0=  pSrc[-2~5] d5 =  pSrc[6~13]
	vst1.u8 {d2}, [r11], r1		@D d6 lTemp0=  pSrc[-2~5] d7 =  pSrc[6~13]

	vst1.u8 {d4}, [r11], r1		@A d8 lTemp0=  pSrc[-2~5] d9 =  pSrc[6~13]
	vst1.u8 {d6}, [r11], r1		@B d10 lTemp0= pSrc[-2~5] d11 = pSrc[6~13]	
	vst1.u8 {d8}, [r11], r1		@C d12 lTemp0= pSrc[-2~5] d13 = pSrc[6~13]
	vst1.u8 {d10}, [r11]		@D d14 lTemp0= pSrc[-2~5] d15 = pSrc[6~13]

	add	    r12, r12, #8
	
@		//need write an 2x2 functions
@		//only the left 2pixel of H
@		rv8filter(&lpr[0], pH[0], stride, 1)@
@		rv8filter(&lpr[0+4*stride], pH[0+(Width/4)+1], stride, 1)@
@		pH += 1@
@		pV = pV + (Width/4)@
@		pH = pH + (Width/4)+1@
ONLY_luma_skip:
@ pCur = r11@ pV = r4@ pH = r5@ stride = r1@	
@2x2_H			@@@@r0~6, r11, r12, r14, H1 = r9(4), H2 = r10(4)
@big r0~6, 		@lit r12, r14 @16x8	r7, r8, r9, r10, r11

	lsr	r8, r9, #8
	lsr	r7, r10, #8
@diff = [(A - D) + ((C - B) << 2)] >> 3@	
		CMP		r8,#0			@stall
		BEQ		H2x2_2
		LDRB	r9,[r12,-r1,LSL #1]		@A
		LDRB	r11,[r12,-r1]			@B		
		LDRB	r14,[r12,#0x00]			@C
		LDRB	r10,[r12,r1]			@D				
		SUB		r9,r9,r10
		SUB		r9,r9,r11,LSL #2
		ADD		r9,r9,r14,LSL #2
		MOVS	r9,r9,ASR #3	@d(r9)
		BEQ		Ln1a
		CMP		r9,r8
		MOVGT	r9,r8
		CMN		r9,r8
		RSBMI	r9,r8,#0
		ADD		r11,r11,r9
		USAT	r11,#8,r11
		SUB		r14,r14,r9
		USAT	r14,#8,r14
		STRB	r11,[r12,-r1]
		STRB	r14,[r12,#0x00]
Ln1a:
		ADD		r12,r12,#1
		LDRB	r9,[r12,-r1,LSL #1]
		LDRB	r10,[r12,r1]
		LDRB	r11,[r12,-r1]
		LDRB	r14,[r12,#0x00]
		SUB		r9,r9,r10
		SUB		r9,r9,r11,LSL #2
		ADD		r9,r9,r14,LSL #2
		MOVS	r9,r9,ASR #3	@d(r9)
		SUBEQ	r12,r12,#1		
		BEQ		H2x2_2
		CMP		r9,r8
		MOVGT	r9,r8
		CMN		r9,r8
		RSBMI	r9,r8,#0
		ADD		r11,r11,r9
		USAT	r11,#8,r11
		SUB		r14,r14,r9
		USAT	r14,#8,r14
		STRB	r11,[r12,-r1]
		STRB	r14,[r12,#0x00]	
		SUB	r12,r12,#1
H2x2_2:
		ADD	r12, r12, r1, lsl #2
			
		CMP	r7,#0			@stall
		BEQ	H2x2_2_END
		LDRB	r9,[r12,-r1,LSL #1]
		LDRB	r11,[r12,-r1]		
		LDRB	r14,[r12,#0x00]		
		LDRB	r10,[r12,r1]
				
		SUB		r9,r9,r10
		SUB		r9,r9,r11,LSL #2
		ADD		r9,r9,r14,LSL #2
		MOVS	r9,r9,ASR #3	@d(r9)
		BEQ		Ln1b
		CMP		r9,r7
		MOVGT	r9,r7
		CMN		r9,r7
		RSBMI	r9,r7,#0
		ADD		r11,r11,r9
		USAT	r11,#8,r11
		SUB		r14,r14,r9
		USAT	r14,#8,r14
		STRB	r11,[r12,-r1]
		STRB	r14,[r12,#0x00]

Ln1b:
		ADD		r12,r12,#1
		LDRB	r9,[r12,-r1,LSL #1]
		LDRB	r10,[r12,r1]
		LDRB	r11,[r12,-r1]
		LDRB	r14,[r12,#0x00]
		SUB		r9,r9,r10
		SUB		r9,r9,r11,LSL #2
		ADD		r9,r9,r14,LSL #2
		MOVS	r9,r9,ASR #3	@d(r9)
		BEQ		H2x2_2_END
		CMP		r9,r7
		MOVGT	r9,r7
		CMN		r9,r7
		RSBMI	r9,r7,#0
		ADD		r11,r11,r9
		USAT	r11,#8,r11
		SUB		r14,r14,r9
		USAT	r14,#8,r14
		STRB	r11,[r12,-r1]
		STRB	r14,[r12,#0x00]

H2x2_2_END:
@	}
	subs	r3, r3, #8	@Height
	addne	r0, r0, r1, lsl #3
	add	r4, r2, lsr #2
	add	r5, r2, lsr #2			
	bne	rv8_edge_filter_big_loop

rv8_edge_filter_end_only_chroma:

	add         sp, sp, #0xC
	ldmia       sp!, {r4 - r11, pc}

	@endp  @ |rv8_edge_filter|
	@end
