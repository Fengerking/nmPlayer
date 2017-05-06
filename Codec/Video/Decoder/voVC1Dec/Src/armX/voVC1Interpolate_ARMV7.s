    AREA |.text|, CODE, READONLY
    EXPORT  voInterpolateBlockBicubic_ARMV7
    EXPORT  voInterpolateBlockBilinear_ARMV7
    EXPORT  voInterpolateBlockBilinear_11_ARMV7
    EXPORT  voInterpolateBlockBilinear_10_ARMV7
    EXPORT  voInterpolateBlockBilinear_01_ARMV7
    
voInterpolateBlockBilinear_01_ARMV7
;r0 = pSrc
;r1 = iSrcStride
;r2 = pDst
;r3 = iDstStride
;r4 = iXFrac =0
;r5 = iYFrac =2
;r6 = iRndCtl
;r7 = b1MV
  stmdb     sp!, {r4-r12, r14}

  ldr			r4, [sp, #40+0]
	ldr			r5, [sp, #40+4]
	ldr			r6, [sp, #40+8]  ;iRndCtrl
	ldr			r7, [sp, #40+12] ;b1MV

	and			r6, r6, #0xff
	rsb			r6, r6, #8		;iRndCtrl = 8 - ( iRndCtrl&0xff);		
	vdup.16		q7, r6			;iRndCtrl	
	
	vdup.16		d0, r4          ;iXFrac
	vdup.8		d1, r5          ;iYFrac
	
	cmp			r7, #0			;b1MV
	bne			BL01_16_loop_Start
    
	vld1.u8		d0, [r0], r1
	mov			r12, #8
BL01_8_loop
    
	vld1.u8		d1, [r0], r1
	vld1.u8		d2, [r0], r1
							
	vaddl.u8	q8 , d0, d1
	vaddl.u8	q9 , d1, d2
    pld			[r0]
    pld			[r0, r1]
    
	vshl.i16	q8 , #3
	vshl.i16	q9 , #3
	vadd.i16	q8 , q7
	vadd.i16	q9 , q7
	
	vshrn.u16	d16, q8, #4
	vshrn.u16	d17, q9, #4
	
	vshr.s32 	d0, d2, #0  ;vmov	d0, d2	;for next loop
	
	vst1.u32	d16 , [r2], r3	;k0
	vst1.u32	d17 , [r2], r3 
	
	subs		r12, r12, #2
	bne			BL01_8_loop
    
    ldmia		sp!, {r4-r12, pc}    
	
BL01_16_loop_Start

	vld1.u8		{q0}, [r0], r1
	mov			r12, #16
	
BL01_16_loop

	vld1.u8		{q1}, [r0], r1
	vld1.u8		{q2}, [r0], r1
    pld			[r0]
    pld			[r0, r1]

	vaddl.u8	q8 , d0, d2		;q8 = |z7 |z6 |z5 |z4 |z3 |z2 |z1|z0|
	vaddl.u8	q9 , d1, d3		;q9 = |z15|z14|z13|z12|z11|z10|z9|z8|
	vaddl.u8	q10, d2, d4		;(z16 = x15 + y15, ..., z0 = x0 + y0)
	vaddl.u8	q11, d3, d5							
		
	vshr.s32 	q0, q2, #0  ;vmov	q0, q2		; for next loop
	
	vshl.i16	q8 , #3
	vshl.i16	q9 , #3
	vshl.i16	q10, #3
	vshl.i16	q11, #3
	vadd.i16	q8 , q7
	vadd.i16	q9 , q7
	vadd.i16	q10, q7
	vadd.i16	q11, q7
	
	vshrn.u16	d16, q8 , #4
	vshrn.u16	d17, q9 , #4
	vshrn.u16	d18, q10, #4
	vshrn.u16	d19, q11, #4
	
	vst1.u32	{q8}, [r2], r3	;k1
	vst1.u32	{q9}, [r2], r3
	
	subs		r12, r12, #2
	bne			BL01_16_loop	
  ldmia		sp!, {r4-r12, pc}
    
voInterpolateBlockBilinear_10_ARMV7
;r0 = pSrc
;r1 = iSrcStride
;r2 = pDst
;r3 = iDstStride
;r4 = iXFrac =2
;r5 = iYFrac =0
;r6 = iRndCtl
;r7 = b1MV
    stmdb     sp!, {r4-r12, r14}

    ldr			r4, [sp, #40+0]
	ldr			r5, [sp, #40+4]
	ldr			r6, [sp, #40+8]  ;iRndCtrl
	ldr			r7, [sp, #40+12] ;b1MV

	and			r6, r6, #0xff
	rsb			r6, r6, #8		;iRndCtrl = 8 - ( iRndCtrl&0xff);		
	vdup.16		q7, r6			;iRndCtrl	
	
	vdup.16		d0, r4          ;iXFrac
	vdup.8		d1, r5          ;iYFrac
	
	cmp			r7, #0			;b1MV
	bne			BL10_16_loop_Start    
	mov			r12, #8
BL10_8_loop
	add			r5, r0, #8
	vld1.u8		d0, [r0], r1
	vld1.u8		d1, [r0], r1
	vld1.u8		d8[0], [r5], r1
	vld1.u8		d9[0], [r5], r1	
    pld			[r0]
    pld			[r0, r1]
					 
;	PF0 + PF1	
	vext.8		d8 , d0, d8 , #1	;d8 = |x8|x7|x6|x5|x4|x3|x2|x1|
	vext.8		d9 , d1, d9 , #1
	vaddl.u8	q8 , d8 , d0		;q8 = |m7|m6|m5|m4|m3|m2|m1|m0|
	vaddl.u8	q9 , d9 , d1		;m7 = x7 + y7 ... m0 = x0 + y0
	
;	(((PF0 + PF1)<<3) + iRndCtrl)>>4
	vshl.i16	q8 , #3			
	vshl.i16	q9 , #3
	vadd.i16	q8 , q7			
	vadd.i16	q9 , q7
	
	vshrn.u16	d16, q8 , #4			
	vshrn.u16	d17, q9 , #4
	
	vst1.u32	d16, [r2], r3		;k0
	vst1.u32	d17, [r2], r3

	subs		r12, r12, #2	; 2 lines per loop
	bne			BL10_8_loop

	ldmia     sp!, {r4-r12, pc}
	
BL10_16_loop_Start
	add			r4, r0, #16		;point column 16
	mov			r12, #16
BL10_16_loop
	vld1.u8		{q0}, [r0], r1
	vld1.u8		{q1}, [r0], r1

	; q4,q5 = |x16|x15|x14|x13|x12|x11|x10|x9|x8|x7|x6|x5|x4|x3|x2|x1|
	vld1.u8		d8[0] , [r4], r1
	vld1.u8		d10[0], [r4], r1						 
    pld			[r0]
    pld			[r0, r1]
	vext.8		q4, q0, q4, #1	
	vext.8		q5, q1, q5, #1

	vaddl.u8	q8 , d8 , d0	;PF0 + PF1
	vaddl.u8	q9 , d9 , d1
	vaddl.u8	q10, d10, d2
	vaddl.u8	q11, d11, d3
	vshl.i16	q8 , #3			;(PF0 + PF1)<<3
	vshl.i16	q9 , #3
	vshl.i16	q10, #3
	vshl.i16	q11, #3
	vadd.i16	q8 , q7			;((PF0 + PF1)<<3) + iRndCtrl
	vadd.i16	q9 , q7
	vadd.i16	q10, q7
	vadd.i16	q11, q7
	
	vshrn.u16	d16, q8 , #4			;(((PF0 + PF1)<<3) + iRndCtrl)>>4
	vshrn.u16	d17, q9 , #4
	vshrn.u16	d18, q10, #4
	vshrn.u16	d19, q11, #4
	
	vst1.u32	{q8}, [r2], r3	;k0
	vst1.u32	{q9}, [r2], r3	
	
	subs		r12, r12, #2
	bne			BL10_16_loop
	ldmia     sp!, {r4-r12, pc}

voInterpolateBlockBilinear_11_ARMV7
;r0 = pSrc
;r1 = iSrcStride
;r2 = pDst
;r3 = iDstStride
;r4 = iXFrac  = 2
;r5 = iYFrac  = 2
;r6 = iRndCtl
;r7 = b1MV
  stmdb     sp!, {r4-r12, r14}
	ldr			r4, [sp, #40+0]
	ldr			r5, [sp, #40+4]
	ldr			r6, [sp, #40+8]  ;iRndCtrl
	ldr			r7, [sp, #40+12] ;b1MV

	and			r6, r6, #0xff
	rsb			r6, r6, #8		;iRndCtrl = 8 - ( iRndCtrl&0xff);		
	vdup.16		q7, r6			;iRndCtrl	
	
	vdup.16		d0, r4          ;iXFrac
	vdup.8		d1, r5          ;iYFrac
	
	cmp			r7, #0			;b1MV
	bne			BL11_16_loop_Start    

	add			r5, r0, #8		;point column 8
	vld1.u8		d0, [r0], r1
	vld1.u8		d4[0], [r5], r1	;|x8|; d4[0] <==> q2.u16[0]
	mov			r12, #8
BL11_8_loop
	vld1.u8		d1, [r0], r1
	vld1.u8		d2, [r0], r1
	vld1.u8		d6[0], [r5], r1	; d6[0] <==> q3.u16[0]; |x8|
	vld1.u8		d8[0], [r5], r1	; d8[0] <==> q4.u16[0]; |x8|
    pld			[r0]
    pld			[r0, r1]
	
	vaddl.u8	q8 , d0, d1		;q8 = |z7|z6|z5|z4|z3|z2|z1|z0|;(z7 = x7 + y7, ..., z0 = x0 + y0)
	vaddl.u8	q9 , d1, d2
	
	;for next loop				
	vshr.s32 	d0, d2, #0  ;vmov	d0, d2

	;q5,q6 = |x|x|x|x|x|x|x|z8| ;(z8 = x7+y7)
	vaddl.u8	q5, d4, d6
	vaddl.u8	q6, d6, d8
	
	;for next loop
	vdup.u16		d4, d8[0]
	
	vext.16		q5, q8, q5, #1	;q3 = |z8|z7|z6|z5|z4|z3|z2|z1|
	vext.16		q6, q9, q6, #1	;q4 = |z8|z7|z6|z5|z4|z3|z2|z1|

	vadd.u16	q8 , q5, q8
	vadd.u16	q9 , q6, q9 
	vshl.i16	q8 , #2			
	vshl.i16	q9 , #2
	vadd.i16	q8 , q7			
	vadd.i16	q9 , q7
	
	vshrn.u16	d16, q8 , #4			
	vshrn.u16	d17, q9 , #4
	
	vst1.u32	d16, [r2], r3		;k0
	vst1.u32	d17, [r2], r3

	subs		r12, r12, #2
	bne			BL11_8_loop

  ldmia	sp!, {r4-r12, pc}
	
BL11_16_loop_Start
	add			r5, r0, #16			;point column 16
	vld1.u8		{q0}, [r0] , r1		;load first line 16 bytes data
	vld1.u8		d6[0], [r5], r1	;q3[0] <==> d6[0] 
	mov			r12, #16
BL11_16_loop
	vld1.u8		{q1}, [r0], r1
	vld1.u8		{q2}, [r0], r1
	vld1.u8		d8[0] , [r5], r1	;q4[0] <==> d8[0]  
	vld1.u8		d10[0], [r5], r1	;q5[0] <==> d10[0] 
    pld			[r0]
    pld			[r0, r1]
	
	vaddl.u8	q8 , d0, d2		;
	vaddl.u8	q9 , d1, d3		;
	
	vaddl.u8	q10, d2, d4     ;
	vaddl.u8	q11, d3, d5     ;

	; for next loop	
	vshr.s32 	q0, q2, #0  ;vmov	q0, q2

	vaddl.u8	q1, d6, d8	   ;
	    
	vaddl.u8	q2, d8, d10
	
	; for next loop
	vdup.u16		d6, d10[0]	; <==>vmov q3, q5
	
	vext.16		q12, q8 , q9 , #1	
	vext.16		q13, q9 , q1 , #1	
	vext.16		q14, q10, q11, #1	
	vext.16		q15, q11, q2 , #1	
	
	vadd.u16	q8 , q12, q8		
	vadd.u16	q9 , q13, q9 
	vadd.u16	q10, q14, q10
	vadd.u16	q11, q15, q11

	vshl.i16	q8 , #2			
	vshl.i16	q9 , #2
	vshl.i16	q10, #2
	vshl.i16	q11, #2
	
	vadd.i16	q8 , q7			
	vadd.i16	q9 , q7
	vadd.i16	q10, q7
	vadd.i16	q11, q7	
	
	vshrn.u16	d16, q8 , #4			
	vshrn.u16	d17, q9 , #4
	vshrn.u16	d18, q10, #4
	vshrn.u16	d19, q11, #4
	
	vst1.u32	{q8}, [r2], r3	;k0
	vst1.u32	{q9}, [r2], r3	

	subs		r12, r12, #2
	bne			BL11_16_loop
	ldmia     sp!, {r4 - r12, pc}
    
voInterpolateBlockBilinear_ARMV7
;r0 = pSrc
;r1 = iSrcStride
;r2 = pDst
;r3 = iDstStride
;r4 = iXFrac
;r5 = iYFrac
;r6 = iRndCtl
;r7 = b1MV
;/**********************/
	
  stmdb     sp!, {r4-r12, r14}
	ldr			r4, [sp, #40+0]
	ldr			r5, [sp, #40+4]
	ldr			r6, [sp, #40+8]  ;iRndCtrl
	ldr			r7, [sp, #40+12] ;b1MV

	and			r6, r6, #0xff
	rsb			r6, r6, #8		;iRndCtrl = 8 - ( iRndCtrl&0xff);		
	vdup.16		q1, r6			;iRndCtrl	
	
	vdup.16		d0, r4          ;iXFrac
	vdup.8		d1, r5          ;iYFrac
	
	cmp			r7, #0			;b1MV
	bne			BL_16_loop_Start  

	add			r11, r0, #8		;point column 8
	vld1.u8		d4, [r0], r1
	vld1.u8		d5[0], [r11], r1	;|x8|
	mov			r12, #8
BL_8_loop
	vld1.u8		d6, [r0], r1
	vld1.u8		d8, [r0], r1
	vld1.u8		d7[0], [r11], r1	;; |y8|
	vld1.u8		d9[0], [r11], r1
	
    pld			[r0]
    pld			[r0, r1]

	vdup.16		q0, r5     ;; zou
	
	vsubl.u8    q10,d6,d4  ;zou
	vsubl.u8    q12,d7,d5  ;zou		
	vsubl.u8    q11,d8,d6  ;zou     ;next line
	vsubl.u8    q13,d9,d7  ;zou
	vmul.s16	q14, q11, q0
	vmul.s16	q15, q13, q0	
	vmul.s16	q13, q12, q0
	vmul.s16	q12, q10, q0
	
	vdup.16		d0, r4	
	;;;;;		
	vshll.u8	q10, d4, #2		;;(pT[0]<<2)
	vshll.u8	q6,  d5, #2	
	vshll.u8	q11, d6, #2	
	vshll.u8	q7,  d7, #2	
	
	;;backup for next loop				
	vshr.s32 	q2, q4, #0   ;;vmov   q2, q4
	
	;; + (pT[0]<<2)	;current line
	vadd.s16	q12, q12, q10		;;|z7|z6|z5|z4|z3|z2|z1|z0|
	vadd.s16	q13, q13, q6		;;|x|x|x|x|x|x|x|z8|
	;; + (pT[0]<<2)	;next line
	vadd.s16	q14, q14, q11		;;|z7|z6|z5|z4|z3|z2|z1|z0|
	vadd.s16	q15, q15, q7		;;|x|x|x|x|x|x|x|z8|
	
	vext.16		q10, q12, q13, #1	;;q10 == |z8|z7|z6|z5|z4|z3|z2|z1|
	vext.16		q11, q14, q15, #1	;;q11 == |z8|z7|z6|z5|z4|z3|z2|z1|

	vsub.s16	q10, q10, q12		;;(b - a)
	vsub.s16	q11, q11, q14			
	vmul.s16	q13, q10, d0[0]	    ;;(b - a) * iXFrac
	vmul.s16	q15, q11, d0[0]	
		
	vshl.u16	q12, q12, #2	
	vshl.u16	q14, q14, #2
	
	vadd.s16	q10, q13, q12			;; + (a <<2)
	vadd.s16	q11, q15, q14			
	vadd.s16	q10, q10, q1			;; + iRndCtrl
	vadd.s16	q11, q11, q1	
	
	vshrn.u16	d20, q10, #4			;; >> 4 ;|m7|m6|m5|m4|m3|m2|m1|m0|
	vshrn.u16	d21, q11, #4			;; >> 4 ;|m7|m6|m5|m4|m3|m2|m1|m0|
	
	vst1.u32	{d20}, [r2], r3
	vst1.u32	{d21}, [r2], r3

	subs		r12, r12, #2
	bne			BL_8_loop
  ldmia	sp!, {r4-r12, pc}
	
BL_16_loop_Start
	add			r11, r0, #16			 
	vld1.u8		{q2}, [r0] , r1			;load first line 16 bytes data
	vld1.u8		d10[0], [r11], r1	    ;load the byte of extra data
	mov			r12, #16

BL_16_loop
	vld1.u8		{q3}, [r0], r1
	vld1.u8		{q4}, [r0], r1
	vld1.u8		d11[0], [r11], r1	;  
	vld1.u8		d12[0], [r11], r1	;
	
    pld			[r0]
    pld			[r0, r1]    
    ;;;;;;;;;;;;;;;;;;
	vdup.16		    q0, r5 	   ;iYFrac
	vsubl.u8        q10, d6, d4
	vsubl.u8        q12, d7, d5	
	vsubl.u8        q11, d8, d6
	vsubl.u8        q13, d9, d7	
	vsubl.u8        q8, d11, d10
	vsubl.u8        q9, d12, d11
	
	vmul.s16        q14, q11, q0    
	vmul.s16        q15, q13, q0 
	vmul.s16	    q9,  q9,  q0
	
	vmul.s16        q13, q12, q0    
	vmul.s16        q12, q10, q0 
	vmul.s16	    q8,  q8,  q0	
	
	vdup.16		d0, r4
	;;;;;;;;;;;;;;;;;;;;;;;;;
		
		;(pT[0]<<2)	;current line
	vshll.u8	q10, d4, #2		
	vshll.u8	q11, d5, #2				
	vshr.s32 	q2, q4, #0   ;vmov q2, q4	 ;backup for next loop
	
	vshll.u8	q4, d10, #2					
	vshr.s32 	d10, d12, #0  ;vmov d10, d12	;backup for next loop 
		;(pT[0]<<2)	;next line
	vshll.u8	q6, d6, #2		
	vshll.u8	q7, d7, #2	
	vshll.u8	q3, d11, #2
		
		; + (pT[0]<<2)	;current line
	vadd.s16	q12, q12, q10	;|z7|z6|z5|z4|z3|z2|z1|z0|
	vadd.s16	q13, q13, q11	;|z15|z14|z13|z12|z11|z10|z9|z8|
	vadd.s16	q10, q8, q4		;|x|x|x|x|x|x|x|z16|	
		; + (pT[0]<<2)	;next line
	vadd.s16	q14, q14, q6	;|z7|z6|z5|z4|z3|z2|z1|z0|
	vadd.s16	q15, q15, q7	;|z15|z14|z13|z12|z11|z10|z9|z8|
	vadd.s16	q11, q9, q3		;|x|x|x|x|x|x|x|z16|

	vext.16		q8, q12, q13,  #1	;q6 = |z8|z7|z6|z5|z4|z3|z2|z1|
	vext.16		q9, q13, q10,  #1	;q7 = |z16|z15|z14|z13|z12|z11|z10|z9|
	vext.16		q10, q14, q15, #1	;q8 = |z8|z7|z6|z5|z4|z3|z2|z1|
	vext.16		q11, q15, q11, #1	;q9 = |z16|z15|z14|z13|z12|z11|z10|z9|

	vsub.s16	q8 , q8, q12		;(b - a)
	vsub.s16	q9 , q9, q13
	vsub.s16	q10, q10, q14			
	vsub.s16	q11, q11, q15
	vmul.s16	q8,  q8,  d0[0]		;(b - a) * iXFrac
	vmul.s16	q9,  q9,  d0[0]
	vmul.s16	q10, q10, d0[0]	
	vmul.s16	q11, q11, d0[0]
		
	vshl.u16	q12, q12, #2			
	vshl.u16	q13, q13, #2	
	vshl.u16	q14, q14, #2
	vshl.u16	q15, q15, #2
	
	vadd.s16	q8,  q8,  q12			; + (a <<2)
	vadd.s16	q9,  q9,  q13
	vadd.s16	q10, q10, q14
	vadd.s16	q11, q11, q15
	
	vadd.s16	q8,  q8,  q1			; + iRndCtrl
	vadd.s16	q9,  q9,  q1
	vadd.s16	q10, q10, q1
	vadd.s16	q11, q11, q1
	
	vshrn.u16	d16,  q8,  #4	;|m7|m6|m5|m4|m3|m2|m1|m0|
	vshrn.u16	d17,  q9,  #4	;|m15|m14|m13|m12|m11|m10|m9|m8|	
	vshrn.u16	d18,  q10, #4	
	vshrn.u16	d19,  q11, #4
	
	vst1.u32	{q8}, [r2], r3	;k0
	vst1.u32	{q9}, [r2], r3	

	subs		r12, r12, #2
	bne			BL_16_loop

	ldmia     sp!, {r4 - r12, pc}
    
voInterpolateBlockBicubic_ARMV7
;pSrc,  iSrcStride,
;pDst, iDstStride,
;r4: ishiftV,
;r5: pMainLoop->m_ui00030003PiRndCtl,//round V
;r6: pMainLoop->m_ui00400040MiRndCtl,//round H
;r7: pV,
;r8: pH,
;r9: b1MV
	stmdb     sp!, {r4 - r11, r14}
	ldr			r4, [sp, #36+0]   ;iShiftV
  	ldr			r5, [sp, #36+4]   ;iRound32 V
  	ldr			r6, [sp, #36+8]   ;iRound32 H  	
  	ldr			r7, [sp, #36+12]  ;pV
  	
  	mov  r6,r6,lsr #16
  	ldrb		r8, [r7]          ;PV0
  	ldrb		r9, [r7, #1]      ;PV1
  	ldrb		r10, [r7, #2]     ;PV2
  	ldrb		r11, [r7, #3]     ;PV3
  	rsb			r8, r8, #0		
  	rsb			r11, r11, #0	
	vdup.8		d24, r8			; abs(v0)
	vdup.8		d25, r9			; v1
	vdup.8		d26, r10		; v2
	vdup.8		d27, r11		; abs(v3)
	
	ldr			r7,  [sp, #36+16]
	ldr		    r8,  [r7]
	vdup.32		d0, r8			; h3,h2,h1,h0
	vmovl.s8	q0, d0			; extend to 16 bits

	
  	ldr			r12, [sp, #36+20]  ;b1MV
  	
  	rsb			r4, r4, #0
	vdup.16		q14, r4		 ;ishiftV
  	
  	cmp			r12, #0
	bne			VHF_16_loop_start
	add			r7, r0, #8
	vld1.u8		{q6}, [r0], r1
	vld1.u8		{q7}, [r0], r1
	vld1.u8		{q8}, [r0], r1
	mov			r8, #8 ;loop
VHFX_8_loop
	vld1.u8		{q9}, [r0], r1	
	vdup.16		q1, r5			; + iRound32V
	vdup.16		q2, r5			;	
	
	vmlsl.u8	q1, d12, d24		
	vmlsl.u8	q2, d13, d24
	vshr.s32 	q6, q7, #0	
	
	vmlal.u8	q1, d14, d25		
	vmlal.u8	q2, d15, d25
	vshr.s32 	q7, q8, #0	
	
	vmlal.u8	q1, d16, d26
	vmlal.u8	q2, d17, d26
	vshr.s32 	q8, q9, #0	
	
	vmlsl.u8	q1, d18, d27		
	vmlsl.u8	q2, d19, d27
	
	vshl.s16  q1, q1, q14    ;q1  q2 
	vshl.s16  q2, q2, q14 
	
	;;;;;;;;;Horiz;;;;;;;;;;;;;
	;q1  q2
	vext.16		q4,  q1, q2, #1	; r1
	vext.16		q5,  q1, q2, #2	; r1
	vext.16		q15, q1, q2, #3	; r1
	vdup.32		q9,  r6
	vdup.32		q2,  r6
	;vdup.16		d18,  r6			; + iRound32H
	;vmovl.s16	q9, d18
	;vdup.16		d4,  r6			; + iRound32H
	;vmovl.s16	q2, d4
	
	vmlal.s16	q9, d2,  d0[0]
	vmlal.s16	q2, d3,  d0[0]
	
	vmlal.s16	q9, d8,  d0[1]
	vmlal.s16	q2, d9,  d0[1]
	
	vmlal.s16	q9, d10,  d0[2]
	vmlal.s16	q2, d11,  d0[2]
	
	vmlal.s16	q9, d30, d0[3]
	vmlal.s16	q2, d31, d0[3]
	
	vqshrun.s32  d18, q9, #7
	vqshrun.s32  d19, q2, #7
	vqshrun.s16  d18,  q9, #0
	
	subs    r8,#1	
	vst1.u32    d18, [r2], r3	
	bgt			VHFX_8_loop
	ldmia	sp!, {r4 - r11, pc}

VHF_16_loop_start
	;load src	
	add			r7, r0, #16
	vld1.u8		{q6}, [r0], r1
	vld1.u8		{q7}, [r0], r1
	vld1.u8		{q8}, [r0], r1
	;extra data
	vld1.u8		{d20}, [r7], r1
	vld1.u8		{d21}, [r7], r1
	vld1.u8		{d22}, [r7], r1
	
	mov			r8, #16 ;loop
VHFX_16_loop
	vld1.u8		{q9}, [r0], r1	
	vld1.u8		{d23}, [r7], r1;extra data

	vdup.16		q1, r5			; + iRound32V
	vdup.16		q2, r5			;	
	vdup.16		q3, r5			; 
	
	vdup.16		q14, r4		 ;ishiftV
	; - o0*abs(v0)
	vmlsl.u8	q1, d12, d24		
	vmlsl.u8	q2, d13, d24	
	vshr.s32 	q6, q7, #0			
	; + o1*v1
	vmlal.u8	q1, d14, d25		
	vmlal.u8	q2, d15, d25		
	vshr.s32 	q7, q8, #0		
	; + o2*v2
	vmlal.u8	q1, d16, d26
	vmlal.u8	q2, d17, d26
	vshr.s32 	q8, q9, #0	
	; - o3*abs(v3)
	vmlsl.u8	q1, d18, d27		
	vmlsl.u8	q2, d19, d27		
	
	vshl.s16  q1, q1, q14    ;q6  q7 
	vshl.s16  q2, q2, q14    	
	;;;extra data;;;;;       ;q6  q7  q13
	vmlsl.u8	q3, d20, d24
	vmlal.u8	q3, d21, d25
	vmlal.u8	q3, d22, d26
	vmlsl.u8	q3, d23, d27
	
	vshr.s32 	d20, d21, #0
	vshr.s32 	d21, d22, #0	
	vshr.s32 	d22, d23, #0
	
	vshl.s16 	q3, q3, q14
	;;;;;;;;;;;;;;;;;;	
	;q1  q2  q3
	;;;;;;;;;Horiz;;;;;;;;;;;;
	vext.16		q4,  q1, q2, #1	; r1
	vext.16		q5,  q1, q2, #2	; r1
	vext.16		q15,  q1, q2, #3	; r1
	
	vdup.32		q9,  r6			; + iRound32H
	vdup.32		q14,  r6		; + iRound32H
	
	vmlal.s16	q9, d2, d0[0]
	vmlal.s16	q14, d3, d0[0]	
	vmlal.s16	q9, d8, d0[1]
	vmlal.s16	q14, d9, d0[1]	
	vmlal.s16	q9, d10, d0[2]
	vmlal.s16	q14, d11, d0[2]	
	vmlal.s16	q9, d30, d0[3]
	vmlal.s16	q14, d31, d0[3]
	
	vqshrun.s32  d2, q9, #7
	vqshrun.s32  d3, q14, #7
	vqshrun.s16  d2,  q1, #0
	
	;vmla.s16	q9, q1, d0[0]
	;vmla.s16	q9, q4, d0[1]
	;vmla.s16	q9, q5, d0[2]
	;vmla.s16	q9, q15, d0[3]
	
	vext.16		q4,  q2, q3,  #1	; r1
	vext.16		q5,  q2, q3,  #2	; r1
	vext.16		q15, q2, q3,  #3	; r1
	
	vdup.32		q9,  r6			; + iRound32H
	vdup.32		q14,  r6		; + iRound32H
	
	vmlal.s16	q9, d4, d0[0]
	vmlal.s16	q14, d5, d0[0]	
	vmlal.s16	q9, d8, d0[1]
	vmlal.s16	q14, d9, d0[1]	
	vmlal.s16	q9, d10, d0[2]
	vmlal.s16	q14, d11, d0[2]	
	vmlal.s16	q9, d30, d0[3]
	vmlal.s16	q14, d31, d0[3]
	
	vqshrun.s32  d18, q9, #7
	vqshrun.s32  d19, q14, #7
	vqshrun.s16  d3,  q9, #0
	
	;vmla.s16	q1, q2,  d0[0]
	;vmla.s16	q1, q4,  d0[1]
	;vmla.s16	q1, q5,  d0[2]
	;vmla.s16	q1, q15, d0[3]
	
	;vqshrun.s16  d18, q9, #7
	;vqshrun.s16  d19, q1, #7	
	;;;;;;;;
	subs    r8,#1
	
	vst1.u32    {q1}, [r2], r3
	
	bgt			VHFX_16_loop	
	
	ldmia	sp!, {r4 - r11, pc}
	end

   