	AREA |.text|, CODE, READONLY
	EXPORT  voVC1InvTrans_8x8_ARMV7
	EXPORT  voVC1InvTrans_8x4_ARMV7
	EXPORT  voVC1InvTrans_4x8_ARMV7
	EXPORT  voVC1InvTrans_4x4_ARMV7
	
	EXPORT voVC1InvTrans_8x8_DC_ARMV7
	EXPORT voVC1InvTrans_8x4_DC_ARMV7
	EXPORT voVC1InvTrans_4x8_DC_ARMV7
	EXPORT voVC1InvTrans_4x4_DC_ARMV7
	macro 
	Col8_first4line	
	; q12  S4*W4
; q13  S0*W4 + 4
	vmull.s16	q12, d9, D1[2]
	vmull.s16	q13, d8, D1[3]
	vadd.s32	q13, q13, q1		;x0 = (x0 << 11) + 4;

; A q14 = W1*S1 + W7*S7
; B q15 = W7*S1 - W1*S7

	vmull.s16	q14, d11, D0[0]			;W7*S7
	vmlal.s16	q14, d10, D0[1]			;q14 = W1*S1 + W7*S7
	
	vmull.s16	q15, d10, D0[0]			;W7*S1
	vmlsl.s16	q15, d11, D0[1]			;q15 = W7*S1 - W1*S7
	
;	C q4 = W3*S3 + W5*S5
;	D q5 = W3*S5 - W5*S5	

	vmull.s16	q4, d14, D0[2]			;W3*S3
	vmlal.s16	q4, d15, D0[3]			;q4 = W3*S3 + W5*S5
	
	vmull.s16	q5, d15, D0[2]			;W3*S5
	vmlsl.s16	q5, d14, D0[3]			;q5 = W3*S5 - W5*S5
	
;	G q7 = W6*S6 + W2*S2
;	H q3 = W6*S2 - W2*S6

	vmull.s16	q7, d13, D1[0]			;W6*S6
	vmlal.s16	q7, d12, D1[1]			;q7 = W6*S6 + W2*S2
	
	vmull.s16	q3, d12, D1[0]			;W6*S2
	vmlsl.s16	q3, d13, D1[1]			;q3 = W6*S2 - W2*S6
	
;	E q6  = W4*S0 + W4*S4 + 4
;	F q13 = W4*S0 - W4*S4 + 4
	vadd.s32	q6, q13, q12		;W4*S0 + W4*S4 + 4
	vsub.s32	q13, q13, q12		;W4*S0 - W4*S4 + 4
		

	vadd.s32	q12, q14, q4		;q12 Cd = A + C
	vmul.s32    q14, q14,d4[0];24576
	vmul.s32    q4, q4,d4[1];21845
	vsub.s32	q14, q14, q4		;q14 Ad = A - C ---> Ad = 24576 *A - 21845*C	 
	
	vadd.s32	q4, q15, q5			;q4  Dd =  B + D	
	vmul.s32    q15, q15,d4[0];24576 
	vmul.s32    q5, q5,d4[1];21845 
	vsub.s32	q15, q15, q5		;q15 Bd = B - D ---> Bd = 24576 *B - 21845*D 
	
	vadd.s32	q5, q6, q7			;q5  Fd = E + G
	vsub.s32	q6, q6, q7			;q6  Ed = E - G
	
	vadd.s32	q7, q13, q3			;q7  Hd = F + H
	vsub.s32	q13, q13, q3		;q13 Gd = F - H

;	Bdd q3 = (181 * (Ad + Bd) + 128) >> 8;
;	Add q14 = (181 * (Ad - Bd) + 128) >> 8;
;Ad = 24576*A - 21845*C;
;Bd = 24576*B - 21845*D;
;Add = ((Ad - Bd + 16*1024) >>15);
;Bdd = ((Ad + Bd + 16*1024) >> 15);
	vmov.s32	q2, #0x4000
	vadd.s32	q3, q14, q15    ;Ad + Bd
	vadd.s32	q3, q3, q2	    ;(Ad + Bd + 16*1024)
	vshr.s32	q3, q3, #15		;q3 Bdd = (Ad + Bd + 16*1024) >> 8;
					
	;vadd.s32	q3, q14, q15    ;Ad + Bd
	;vmul.s32	q3, q3, D1[1]   ;(Ad + Bd)*181
	;vadd.s32	q3, q3, q1	    ;(Ad + Bd)*181 + 128
	;vshr.s32	q3, q3, #8		;q3 Bdd = (181 * (Ad + Bd) + 128) >> 8;

	vsub.s32	q14, q14, q15	 
	vadd.s32	q14, q14, q2			
	vshr.s32	q14, q14, #15	  ;q14 Add = (Ad - Bd + 16*1024) >> 8;
	;vsub.s32	q14, q14, q15	 
	;vmul.s32	q14, q14, D1[1]
	;vadd.s32	q14, q14, q1			
	;vshr.s32	q14, q14, #8	  ;q14 Add = (181 * (Ad - Bd) + 128) >> 8;

	vadd.s32	q15, q5, q12		;Blk[ 0] = (idct_t)((x7 + x1) >> 8);
	vsub.s32	q5, q5, q12			;Blk[56] = (idct_t)((x7 - x1) >> 8);
	vadd.s32	q12, q7, q3			;Blk[8] = (idct_t)((x3 + x2) >> 8);
	vsub.s32	q7, q7, q3			;Blk[48] = (idct_t)((x3 - x2) >> 8);
	vadd.s32	q3, q13, q14		;Blk[16] = (idct_t)((x0 + x4) >> 8);
	vsub.s32	q13, q13, q14		;Blk[40] = (idct_t)((x0 - x4) >> 8);
	vadd.s32	q14, q6, q4			;Blk[24] = (idct_t)((x8 + x6) >> 8);
	vsub.s32	q6, q6, q4			;Blk[32] = (idct_t)((x8 - x6) >> 8);
	
	;vshrn.s32	d2, q15, #3  ;0
	;vshrn.s32	d3, q12, #3  ;8
	;vshrn.s32	d4, q3, #3   ;16
	;vshrn.s32	d5, q14, #3  ;24
	;vshrn.s32	d8, q6, #3   ;32	
	;vshrn.s32	d12, q7, #3  ;48
	;vshrn.s32	d14, q5, #3  ;56
	;vshrn.s32	d10, q13, #3 ;40
	mend
	
	macro 
	Col8_last4line
	vld1.s16 	{d9}, [r12] ;d9 24576  21845
; q12  S4*W4
; q13  S0*W4 + 64
	vmov.s32	q3, #4	
	vmull.s16	q12, d17, D1[2]
	vmull.s16	q13, d16, D1[3]
	vadd.s32	q13, q13, q3		

; A q14 = W1*S1 + W7*S7
; B q15 = W7*S1 - W1*S7
	vmull.s16	q14, d23, D0[0]			;W7*S7
	vmlal.s16	q14, d22, D0[1]			;q14 = W1*S1 + W7*S7
	
	vmull.s16	q15, d22, D0[0]			;W7*S1
	vmlsl.s16	q15, d23, D0[1]			;q15 = W7*S1 - W1*S7
	
;	C q8 = W3*S3 + W5*S5
;	D q11 = W3*S5 - W5*S5	
	vmull.s16	q8, d18, D0[2]			;W3*S3
	vmlal.s16	q8, d19, D0[3]			;q8 = W3*S3 + W5*S5
	
	vmull.s16	q11, d19, D0[2]			;W3*S5
	vmlsl.s16	q11, d18, D0[3]			;q11 = W3*S5 - W5*S5
	
;	G q9 = W6*S6 + W2*S2
;	H q3 = W6*S2 - W2*S6
	vmull.s16	q9, d21, D1[0]			;W6*S6 
	vmlal.s16	q9, d20, D1[1]			;q9 = W6*S6 + W2*S2
	
	vmull.s16	q3, d20, D1[0]			;W6*S2
	vmlsl.s16	q3, d21, D1[1]			;q3 = W6*S2 - W2*S6
	
;	x8 = x0 + x1;
;	x0 -= x1;
	vadd.s32	q10, q13, q12		;E q10 = W4*S0 + W4*S4 + 64
	vsub.s32	q13, q13, q12		;F q13 = W4*S0 - W4*S4 + 64

	vadd.s32	q12, q14, q8		;q12 Cd = A + C
	;vsub.s32	q14, q14, q8		;q14 Ad = A - C ----> Ad = 24576 *A - 21845*C
	vmul.s32    q14, q14, d9[0]
	vmul.s32    q8,  q8,  d9[1]
	vsub.s32	q14, q14, q8
	
	vadd.s32	q8, q15, q11		;q8  Dd = B + D
	;vsub.s32	q15, q15, q11		;q15 Bd = B - D ----> Bd = 24576 *B - 21845*D 
	vmul.s32    q15, q15, d9[0]
	vmul.s32    q11, q11, d9[1]
	vsub.s32	q15, q15, q11
	
	vadd.s32	q11, q10, q9		;q11 Fd = E + G
	vsub.s32	q10, q10, q9		;q10 Ed = E - G
	
	vadd.s32	q9, q13, q3			;q9  Hd = F + H
	vsub.s32	q13, q13, q3		;q13 Gd = F - H
	
	vmov.s32	q0, #0x4000	
	vadd.s32    q3, q14, q15		
	vadd.s32    q3, q3,  q0
	vshr.s32	q3, q3,  #15         ;q3 Bdd = (Ad + Bd + 16*1024) >> 15
	
	vsub.s32    q14,  q14, q15		
	vadd.s32    q14,  q14,  q0
	vshr.s32	q14,  q14,  #15       ;q14 Add = (Ad - Bd + 16*1024) >> 15

	vadd.s32	q15, q11, q12		;Blk[ 0] = (idct_t)((Fd + Cd) >> 8);
	vsub.s32	q11, q11, q12		;Blk[56] = (idct_t)((Fd - Cd) >> 8);
	vadd.s32	q12, q9, q3			;Blk[8] = (idct_t)((Hd + Bdd) >> 8);
	vsub.s32	q9, q9, q3			;Blk[48] = (idct_t)((Hd - Bdd) >> 8);
	vadd.s32	q3, q13, q14		;Blk[16] = (idct_t)((Gd + Add) >> 8);
	vsub.s32	q13, q13, q14		;Blk[40] = (idct_t)((Gd - Add) >> 8);
	vadd.s32	q14, q10, q8		;Blk[24] = (idct_t)((Ed + Dd) >> 8);
	vsub.s32	q10, q10, q8		;Blk[32] = (idct_t)((Ed - Dd) >> 8);
	
	vshrn.s32	d9, q10,  #3 ;32
	vshrn.s32	d11, q13, #3 ;40
	vshrn.s32	d13, q9,  #3 ;48
	vshrn.s32	d15, q11, #3 ;56			
	vshrn.s32	d17, q15, #3 ;0
	vshrn.s32	d19, q12, #3 ;8	
	vshrn.s32	d21, q3,  #3 ;16
	vshrn.s32	d23, q14, #3 ;24	
	mend
	
	macro 
	Row_8x8line
	vmov.s32	q3, #64	
	vmull.s16	q12, d17, D1[2]		  ;q12 = S4*W4
	vmull.s16	q13, d16, D1[3]     
	vadd.s32	q13, q13, q3        ;q13  S0*W4 + 64

	vmull.s16	q14, d23, D0[0]			;W7*S7
	vmlal.s16	q14, d22, D0[1]			;A q14 = W1*S1 + W7*S7	
	vmull.s16	q15, d22, D0[0]			;W7*S1
	vmlsl.s16	q15, d23, D0[1]			;B q15 = W7*S1 - W1*S7

	vmull.s16	q8, d18, D0[2]			;W3*S3
	vmlal.s16	q8, d19, D0[3]			;C q8 = W3*S3 + W5*S5	
	vmull.s16	q11, d19, D0[2]			;W3*S5
	vmlsl.s16	q11, d18, D0[3]			;D q11 = W3*S5 - W5*S5

	vmull.s16	q9, d21, D1[0]			;W6*S6
	vmlal.s16	q9, d20, D1[1]			;G q9 = W6*S6 + W2*S2	
	vmull.s16	q3, d20, D1[0]			;W6*S2
	vmlsl.s16	q3, d21, D1[1]			;H q3 = W6*S2 - W2*S6
		
	vadd.s32	q10, q13, q12		;E q10 = S4*W4 + S0*W4 + 64 
	vsub.s32	q13, q13, q12		;F q13 = S0*W4 - S4*W4 + 64    

	vadd.s32	q12, q14, q8		;q12 Cd = A + C     
	;vsub.s32	q14, q14, q8		;q14 Ad = A - C
	vmul.s32  q14, q14,d4[0]  ;24576
	vmul.s32  q8,  q8, d4[1]  ;21845
	vsub.s32	q14, q14, q8		;q14 Ad = A - C ---> Ad = 24576 *A - 21845*C
	
	vadd.s32	q8, q15, q11		;q8  Dd = B + D
	;vsub.s32	q15, q15, q11		;q15 Bd = B - D
	vmul.s32  q15, q15,d4[0]  ;24576 
	vmul.s32  q11, q11,d4[1]  ;21845 
	vsub.s32	q15, q15, q11		;q15 Bd = B - D ---> Bd = 24576 *B - 21845*D

	vadd.s32	q11, q10, q9		;q11 Fd = E + G
	vsub.s32	q10, q10, q9		;q10 Ed = E - G	
	
	vadd.s32	q9, q13, q3			;q9  Hd = F + H
	vsub.s32	q13, q13, q3		;q13 Gd = F - H
	
	vmov.s32	q2, #0x4000
	vadd.s32  q3, q14, q15    ;Ad + Bd
	vadd.s32  q3, q3, q2
	vshr.s32	q3, q3, #15		  ;q3 Bdd = (Ad + Bd + 16*1024) >> 8;
	
	vsub.s32	q14, q14, q15		;Ad - Bd	
	vadd.s32  q14, q14, q2
	vshr.s32	q14, q14, #15	  ;q14 Add = (Ad - Bd + 16*1024) >> 8;

	vadd.s32	q15, q11, q12		;Blk[ 0] = (idct_t)((Fd + Cd) >> 8) 
	vsub.s32	q11, q11, q12		;Blk[56] = (idct_t)((Fd - Cd) >> 8) 
	vadd.s32	q12, q9, q3			;Blk[8] = (idct_t)((Hd + Bdd) >> 8) 
	vsub.s32	q9, q9, q3			;Blk[48] = (idct_t)((Hd - Bdd) >> 8)
	vadd.s32	q3, q13, q14		;Blk[16] = (idct_t)((Gd + Add) >> 8)
	vsub.s32	q13, q13, q14		;Blk[40] = (idct_t)((Gd - Add) >> 8)
	vadd.s32	q14, q10, q8		;Blk[24] = (idct_t)((Ed + Dd) >> 8) 
	vsub.s32	q10, q10, q8		;Blk[32] = (idct_t)((x8 - x6) >> 8)	
	
	vmov.s32	q2, #0x0001
	
	vadd.s32	q10,  q10,  q2			;Blk[32]
	vadd.s32	q13,  q13,  q2		  ;Blk[40]
	vadd.s32	q9,   q9,   q2		  ;Blk[48]
	vadd.s32	q11,  q11,  q2		  ;Blk[56]
	
	vshrn.s32	d2, q10, #7    ;32     
	vshrn.s32	d3, q13, #7    ;40     
	vshrn.s32	d4, q9, #7     ;48    
	vshrn.s32	d5, q11, #7    ;56     
	vshrn.s32	d16, q15, #7   ;0     
	vshrn.s32	d18, q12, #7	 ;8     
	vshrn.s32	d20, q3, #7    ;16     
	vshrn.s32	d22, q14, #7	 ;24     

;row one
	vmov.s32	q3, #64	                            
	vmull.s16	q12, d9, D1[2]		  ;q12 = S4*W4    
	vmull.s16	q13, d8, D1[3]                     
	vadd.s32	q13, q13, q3        ;q13  S0*W4 + 64

	vmull.s16	q14, d11, D0[0]			;W7*S7                
	vmlal.s16	q14, d10, D0[1]			;A q14 = W1*S1 + W7*S7	
	vmull.s16	q15, d10, D0[0]			;W7*S1                
	vmlsl.s16	q15, d11, D0[1]			;B q15 = W7*S1 - W1*S7

	vmull.s16	q4, d14, D0[2]			;W3*S3                
	vmlal.s16	q4, d15, D0[3]			;C q4 = W3*S3 + W5*S5	
	vmull.s16	q5, d15, D0[2]			;W3*S5                
	vmlsl.s16	q5, d14, D0[3]			;D q5 = W3*S5 - W5*S5

	vmull.s16	q7, d13, D1[0]			;W6*S6               
	vmlal.s16	q7, d12, D1[1]			;G q7 = W6*S6 + W2*S2
	vmull.s16	q3, d12, D1[0]			;W6*S2               
	vmlsl.s16	q3, d13, D1[1]			;H q3 = W6*S2 - W2*S6

	vadd.s32	q6, q13, q12		;E q6  = S4*W4 + S0*W4 + 64
	vsub.s32	q13, q13, q12		;F q13 = S0*W4 - S4*W4 + 64

	vld1.s16 	{d0}, [r12] 		;d9 24576  21845

	vadd.s32	q12, q14, q4		;q12 Cd = A + C
	;vsub.s32	q14, q14, q4		;q14 Ad = A - C
	vmul.s32  q14, q14,d0[0]  ;24576 
	vmul.s32  q4,  q4, d0[1]  ;21845 
	vsub.s32	q14, q14, q4		 ;q14 Ad = A - C ---> Ad = 24576 *A - 21845*C
	
	vadd.s32	q4, q15, q5			;q4  Dd = B + D
	;vsub.s32	q15, q15, q5		;q15 Bd = B - D  
	vmul.s32  q15, q15,d0[0]  ;24576 
	vmul.s32  q5,  q5, d0[1]  ;21845 
	vsub.s32	q15, q15, q5		 ;q15 Bd = B - D ---> Bd = 24576 *B - 21845*D 	

	vadd.s32	q5, q6, q7			;q5 Fd = E + G
	vsub.s32	q6, q6, q7			;q6 Ed = E - G	
	
	vadd.s32	q7, q13, q3			;q7  Hd = F + H 
	vsub.s32	q13, q13, q3		;q13 Gd = F - H 
	
	vmov.s32	q0, #0x4000
	vadd.s32	q3, q14, q15		
	vadd.s32	q3, q3,  q0
	vshr.s32	q3, q3,  #15         ;q3 Bdd = (Ad + Bd + 16*1024) >> 15
	
	vsub.s32	q14,  q14, q15		
	vadd.s32	q14,  q14,  q0
	vshr.s32	q14,  q14,  #15			;q14 Add = (Ad - Bd + 16*1024) >> 15 
	
	vmov.s32	q0, #0x0001

	vadd.s32	q15, q5, q12		;Blk[ 0] = (idct_t)((x7 + x1) >> 8);
	vsub.s32	q5, q5, q12			;Blk[56] = (idct_t)((x7 - x1) >> 8);
	vadd.s32	q12, q7, q3			;Blk[8] = (idct_t)((x3 + x2) >> 8);
	vsub.s32	q7, q7, q3			;Blk[48] = (idct_t)((x3 - x2) >> 8);
	vadd.s32	q3, q13, q14		;Blk[16] = (idct_t)((x0 + x4) >> 8);
	vsub.s32	q13, q13, q14		;Blk[40] = (idct_t)((x0 - x4) >> 8);
	vadd.s32	q14, q6, q4			;Blk[24] = (idct_t)((x8 + x6) >> 8);
	vsub.s32	q6, q6, q4			;Blk[32] = (idct_t)((x8 - x6) >> 8);
	
	
	vadd.s32	q6,  q6,  q0			;Blk[32]
	vadd.s32	q13, q13, q0		  ;Blk[40]
	vadd.s32	q7,  q7,  q0		  ;Blk[48]
	vadd.s32	q5,  q5,  q0		  ;Blk[56]
	
	vshrn.s32	d17, q15, #7 ;0
	vshrn.s32	d19, q12, #7 ;8
	vshrn.s32	d21, q3,  #7 ;16
	vshrn.s32	d23, q14, #7 ;24
	vshrn.s32	d9,  q6,  #7 ;32
	vshrn.s32	d13, q7,  #7 ;48
	vshrn.s32	d15, q5,  #7 ;56
	vshrn.s32	d11, q13, #7 ;40	
	mend	
;voVC1InvTrans_8x8_DC_ARMV7	
;(unsigned char* pDest,
;int dststride,
;unsigned char* pRef0,
;unsigned char* pRef1,
;int refstride,
;short *block,
;unsigned long *table0,
;unsigned long *table1)
	align 4
voVC1InvTrans_8x8_DC_ARMV7
	stmdb   sp!, {r4,r5,r6,r12,lr}
	ldr			r4, [ sp, #20]  ; refstride
	ldr			r5, [ sp, #24]  ; block
	ldrsh     r5, [r5]   ;dc
	mov     r12,#12
	mul     r5,r5,r12  ;E = ((src[0]*12 + 4)>>3) *12 + 64;
	add     r5,r5,#4
	mov     r5,r5,asr #3
	mul     r5,r5,r12
	add     r5,r5,#64  ;E
	add     r6,r5,#1   ;E+1
	mov     r5,r5,asr #7
	mov     r6,r6,asr #7     
	
	;;;;;;;;;;;;;;;;
	;;;;;;;write to dest;;;;;;;
	cmp			r2, #0
	beq			IDCT_NOREF_8x8_DC
	cmp         r3, #0
	beq         IDCT_ONEREF_8x8_DC
	vld1.64 {d0}, [r2], r4 	
 	vld1.64 {d2}, [r2], r4 	
 	vld1.64 {d4}, [r2], r4 	
 	vld1.64 {d6}, [r2], r4  	
 	vld1.64 {d24}, [r2], r4  	
 	vld1.64 {d26}, [r2], r4  	
 	vld1.64 {d28}, [r2], r4  	
 	vld1.64 {d30}, [r2]
 	
 	vld1.64 {d1}, [r3], r4 
 	vld1.64 {d3}, [r3], r4
 	vld1.64 {d5}, [r3], r4
 	vld1.64 {d7}, [r3], r4
 	vld1.64 {d25}, [r3], r4
 	vld1.64 {d27}, [r3], r4
 	vld1.64 {d29}, [r3], r4
 	vld1.64 {d31}, [r3]
 	
 	vrhadd.u8 d0, d0, d1
 	vrhadd.u8 d1, d2, d3
 	vrhadd.u8 d2, d4, d5
 	vrhadd.u8 d3, d6, d7
 	vrhadd.u8 d4, d24, d25
 	vrhadd.u8 d5, d26, d27
 	vrhadd.u8 d6, d28, d29
 	vrhadd.u8 d7, d30, d31
 	
 	bl SHIFT_WRITE
	
IDCT_ONEREF_8x8_DC
	vld1.64 {d0}, [r2], r4 
 	vld1.64 {d1}, [r2], r4
 	vld1.64 {d2}, [r2], r4 
 	vld1.64 {d3}, [r2], r4 
 	vld1.64 {d4}, [r2], r4 
 	vld1.64 {d5}, [r2], r4 
 	vld1.64 {d6}, [r2], r4 
 	vld1.64 {d7}, [r2]
 	
SHIFT_WRITE	
	vdup.s16  q15 ,r5
	vdup.s16  q14 ,r6
			
	vaddw.u8	q8, q15, d0
	vaddw.u8	q9, q15, d1
	vaddw.u8	q10, q15, d2
	vaddw.u8	q11, q15, d3
	vaddw.u8	q4, q14, d4
	vaddw.u8	q5, q14, d5
	vaddw.u8	q6, q14, d6
	vaddw.u8	q7, q14, d7
	
	vqmovun.s16	d16, q8
	vqmovun.s16	d18, q9
	vqmovun.s16	d20, q10
	vqmovun.s16	d22, q11
	vqmovun.s16	d8,  q4
	vqmovun.s16	d10, q5
	vqmovun.s16	d12, q6
	vqmovun.s16	d14, q7		
						
	vst1.64	{d16}, [r0], r1
	vst1.64	{d18}, [r0], r1		
	vst1.64	{d20}, [r0], r1
	vst1.64	{d22}, [r0], r1	
	vst1.64	{d8},  [r0], r1
	vst1.64	{d10}, [r0], r1	
	vst1.64	{d12}, [r0], r1
	vst1.64	{d14}, [r0]	
	
	ldmia		sp!, {r4,r5,r6,r12, pc}	
	
IDCT_NOREF_8x8_DC
	vdup.s16  q15 ,r5
	vdup.s16  q14 ,r6
	
	vqmovun.s16	d16, q15
	vqmovun.s16	d8, q14
						
	vst1.64	{d16}, [r0], r1
	vst1.64	{d16}, [r0], r1		
	vst1.64	{d16}, [r0], r1
	vst1.64	{d16}, [r0], r1	
	vst1.64	{d8},  [r0], r1
	vst1.64	{d8}, [r0], r1	
	vst1.64	{d8}, [r0], r1
	vst1.64	{d8}, [r0]		
	;;;;;;;;;;;;;;;;;;;;;;;;;;;	
	
	ldmia		sp!, {r4,r5,r6,r12, pc}

	align 4	
voVC1InvTrans_8x8_ARMV7
	stmdb   sp!, {r4,r5,r12,lr}
	ldr			r4, [ sp, #16]  ; refstride
	ldr			r5, [ sp, #20]  ; block
	ldr			r12, [ sp, #24]  ; dct_coeff
	;ldr			r12, =W_table
	vld1.s16 	{q0}, [r12]!
	vld1.s16 	{d4}, [r12] ;d4
	vmov.s32	q1, #4
	
	;load src data
	vld1.64	{q4}, [r5]!		;q4 x0 x1 d8-d9
	vld1.64	{q5}, [r5]!		;q5 x4 x5 d10-d11
	vld1.64	{q6}, [r5]!		;q6 x3 x2 d12-d13
	vld1.64	{q7}, [r5]!		;q7 x7 x6 d14-d15
	vld1.64	{q8}, [r5]!		;
	vld1.64	{q9}, [r5]!		;
	vld1.64	{q10}, [r5]!	;
	vld1.64	{q11}, [r5]		;
	
	vswp.s32	d9, d16		;q8  x0 x1 d16-17
	vswp.s32	d15, d18	;q11 x4 x5 d22-23
	vswp.s32	d13, d20	;q10 x3 x2 d20-21	
	vswp.s32	d11, d22	;q9  x7 x6 d18-19
	
	;first four line	
	Col8_first4line
	vshrn.s32	d2, q15, #3  ;0
	vshrn.s32	d3, q12, #3  ;8
	vshrn.s32	d4, q3, #3   ;16
	vshrn.s32	d5, q14, #3  ;24
	vshrn.s32	d8, q6, #3   ;32	
	vshrn.s32	d12, q7, #3  ;48
	vshrn.s32	d14, q5, #3  ;56
	vshrn.s32	d10, q13, #3 ;40
	;last four line
	Col8_last4line	
	
	;;;;;;;
	vswp.s32	d2, d16
	vswp.s32	d3, d18
	vswp.s32	d4, d20	
	vswp.s32	d5, d22
	
	;;;;;;;;;;;;;;
	;Transpose	
	vtrn.16 q8, q9
	vtrn.16 q10, q11
	vtrn.16 q4, q5
	vtrn.16 q6, q7
	vtrn.32 q8, q10
	vtrn.32 q9, q11
	vtrn.32 q4, q6
	vtrn.32 q5, q7
	
	vswp.s32	d19, d23
	vswp.s64	q9, q11
	vswp.s32	d11, d15
	
	;;;;;;;;
	;ldr			r12, =W_table
	ldr			r12, [ sp, #24]  ; dct_coeff
	vld1.s16 	{q0}, [r12]!
	vld1.s16 	{d4}, [r12] ;d4
	
	Row_8x8line		
	
	vswp.s32	d2, d8
	vswp.s32	d3, d10
	vswp.s32	d4, d12	
	vswp.s32	d5, d14
	
	;;;;;;;;;;;;;;;;
	;;;;;;;write to dest;;;;;;;
	cmp			r2, #0
	beq			IDCT_CPY_8x8
	cmp         r3, #0
	beq         IDCT_ONEREF_8x8
	vld1.64 {d0}, [r2], r4 	
 	vld1.64 {d2}, [r2], r4 	
 	vld1.64 {d4}, [r2], r4 	
 	vld1.64 {d6}, [r2], r4  	
 	vld1.64 {d24}, [r2], r4  	
 	vld1.64 {d26}, [r2], r4  	
 	vld1.64 {d28}, [r2], r4  	
 	vld1.64 {d30}, [r2]
 	
 	vld1.64 {d1}, [r3], r4 
 	vld1.64 {d3}, [r3], r4
 	vld1.64 {d5}, [r3], r4
 	vld1.64 {d7}, [r3], r4
 	vld1.64 {d25}, [r3], r4
 	vld1.64 {d27}, [r3], r4
 	vld1.64 {d29}, [r3], r4
 	vld1.64 {d31}, [r3]
 	
 	vrhadd.u8 d0, d0, d1
 	vrhadd.u8 d1, d2, d3
 	vrhadd.u8 d2, d4, d5
 	vrhadd.u8 d3, d6, d7
 	vrhadd.u8 d4, d24, d25
 	vrhadd.u8 d5, d26, d27
 	vrhadd.u8 d6, d28, d29
 	vrhadd.u8 d7, d30, d31
 	
 	vaddw.u8	q8, q8, d0
	vaddw.u8	q9, q9, d1
	vaddw.u8	q10, q10, d2
	vaddw.u8	q11, q11, d3
	vaddw.u8	q4, q4, d4
	vaddw.u8	q5, q5, d5
	vaddw.u8	q6, q6, d6
	vaddw.u8	q7, q7, d7
	
	bl  IDCT_CPY_8x8
IDCT_ONEREF_8x8
	vld1.64 {d0}, [r2], r4 
 	vld1.64 {d1}, [r2], r4
 	vld1.64 {d2}, [r2], r4 
 	vld1.64 {d3}, [r2], r4 
 	vld1.64 {d4}, [r2], r4 
 	vld1.64 {d5}, [r2], r4 
 	vld1.64 {d6}, [r2], r4 
 	vld1.64 {d7}, [r2]
 			
	vaddw.u8	q8, q8, d0
	vaddw.u8	q9, q9, d1
	vaddw.u8	q10, q10, d2
	vaddw.u8	q11, q11, d3
	vaddw.u8	q4, q4, d4
	vaddw.u8	q5, q5, d5
	vaddw.u8	q6, q6, d6
	vaddw.u8	q7, q7, d7
IDCT_CPY_8x8
	vqmovun.s16	d16, q8
	vqmovun.s16	d18, q9
	vqmovun.s16	d20, q10
	vqmovun.s16	d22, q11
	vqmovun.s16	d8, q4
	vqmovun.s16	d10, q5
	vqmovun.s16	d12, q6
	vqmovun.s16	d14, q7		
						
	vst1.64	{d16}, [r0], r1
	vst1.64	{d18}, [r0], r1		
	vst1.64	{d20}, [r0], r1
	vst1.64	{d22}, [r0], r1	
	vst1.64	{d8},  [r0], r1
	vst1.64	{d10}, [r0], r1	
	vst1.64	{d12}, [r0], r1
	vst1.64	{d14}, [r0]		
	;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	ldmia		sp!, {r4,r5,r12, pc}
	
	align 4
voVC1InvTrans_8x4_DC_ARMV7
	stmdb   sp!, {r4,r5,r12,lr}
	ldr			r4, [ sp, #16]  ; refstride
	ldr			r5, [ sp, #20]  ; block
	ldrsh     r5, [r5]   ;dc
	
	mov     r12,#12
	mul     r5,r5,r12  ;E = ((src[0]*12 + 4)>>3) *12 + 64;
	add     r5,r5,#4
	mov     r5,r5,asr #3
	mov     r12,#17
	mul     r5,r5,r12
	add     r5,r5,#64  ;E
	mov     r5,r5,asr #7
    
	
	;;;;;;;;;;;;;;;;
	;;;;;;;write to dest;;;;;;;
	cmp			r2, #0
	beq			IDCT_NOREF_8x4_DC
	cmp         r3, #0
	beq         IDCT_ONEREF_8x4_DC
	vld1.64 {d0}, [r2], r4 	
 	vld1.64 {d2}, [r2], r4 	
 	vld1.64 {d4}, [r2], r4 	
 	vld1.64 {d6}, [r2], r4
 	
 	vld1.64 {d1}, [r3], r4 
 	vld1.64 {d3}, [r3], r4
 	vld1.64 {d5}, [r3], r4
 	vld1.64 {d7}, [r3], r4
 	
 	vrhadd.u8 d0, d0, d1
 	vrhadd.u8 d1, d2, d3
 	vrhadd.u8 d2, d4, d5
 	vrhadd.u8 d3, d6, d7
 	
 	bl SHIFT_WRITE84
	
IDCT_ONEREF_8x4_DC
	vld1.64 {d0}, [r2], r4 
 	vld1.64 {d1}, [r2], r4
 	vld1.64 {d2}, [r2], r4 
 	vld1.64 {d3}, [r2]
 	
SHIFT_WRITE84 	
	vdup.s16  q15 ,r5		
	vaddw.u8	q8, q15, d0
	vaddw.u8	q9, q15, d1
	vaddw.u8	q10, q15, d2
	vaddw.u8	q11, q15, d3
	
	vqmovun.s16	d16, q8
	vqmovun.s16	d18, q9
	vqmovun.s16	d20, q10
	vqmovun.s16	d22, q11		
						
	vst1.64	{d16}, [r0], r1
	vst1.64	{d18}, [r0], r1		
	vst1.64	{d20}, [r0], r1
	vst1.64	{d22}, [r0]	
	
	ldmia		sp!, {r4,r5,r12, pc}	
IDCT_NOREF_8x4_DC
	vdup.s16  q15 ,r5
	vqmovun.s16	d16, q15						
	vst1.64	{d16}, [r0], r1
	vst1.64	{d16}, [r0], r1		
	vst1.64	{d16}, [r0], r1
	vst1.64	{d16}, [r0]		
	;;;;;;;;;;;;;;;;;;;;;;;;;;;		
	ldmia		sp!, {r4,r5,r12, pc}
	align 4
voVC1InvTrans_8x4_ARMV7
	stmdb   sp!, {r4,r5,r12,lr}
	ldr			r4, [ sp, #16]  ;refstride
	ldr			r5, [ sp, #20]  ;block
	vmov.s32	q1, #4
	mov r12,#16
	vld1.64	{d8},  [r5],r12	;0
	vld1.64	{d10}, [r5],r12	;1
	vld1.64	{d12}, [r5],r12	;2
	vld1.64	{d14}, [r5],r12	;3
	vld1.64	{d9},  [r5],r12	;4
	vld1.64	{d15}, [r5],r12	;5
	vld1.64	{d13}, [r5],r12	;6
	vld1.64	{d11}, [r5]		;7
	
	ldr			r12, [ sp, #24]  ;Table
	;ldr			r12, =W_table
	vld1.s16 	{q0}, [r12]!
	vld1.s16 	{d4}, [r12] ;d4
	;;;;;;;;;;;;;;;;;;;;;;;;;
	Col8_first4line	
	vshrn.s32	d2, q15, #3  ;0
	vshrn.s32	d4, q12, #3  ;8
	vshrn.s32	d6, q3, #3   ;16
	vshrn.s32	d8, q14, #3  ;24
	vshrn.s32	d3, q6, #3   ;32
	vshrn.s32	d5, q13, #3 ;40	
	vshrn.s32	d7, q7, #3  ;48
	vshrn.s32	d9, q5, #3  ;56
	
	;;;;;;;;;;;;;;;;;;;;;;;;;	
	vtrn.16 q1,q2
	vtrn.16 q3,q4
	
	vtrn.32 q1,q3
	vtrn.32 q2,q4
	;;;;;;;;;;;;;;;;;;;;;;;;;;
	;ldr			r12, =W_table2
	ldr			r12, [ sp, #28]  ;Table
	vld1.s16 	{q0}, [r12]!
	;q1
	;q2
	;q3
	;q4
	vmov.s32	q15, #64
	;first four col
	vmull.s16 q5,  d2, d0[0] ;q5  s0*17 	
	vmull.s16 q6,  d6, d0[0] ;q6  s2*17	
	vmull.s16 q7,  d4, d0[1] ;q7  s1*22	
	vmull.s16 q8,  d4, d0[2] ;q8  s1*10	
	vmull.s16 q9,  d8, d0[1] ;q9  s3*22	
	vmull.s16 q10, d8, d0[2] ;q10 s3*10
	
	;t1 = 17 * (src[ 0] + src[2]) + 64;
  ;t2 = 17 * (src[ 0] - src[2]) + 64;
	;t3 = 22 * src[ 1] + 10 * src[3];
	;t4 = 22 * src[3] - 10 * src[ 1];
	vadd.s32  q11, q5, q6  
	vadd.s32  q11, q11, q15 ;t1 q11 = 17 * (src[ 0] + src[2])+64
    
	vsub.s32  q12, q5, q6  
	vadd.s32  q12, q12, q15  ;t2 q12 = 17 * (src[ 0] - src[2])+64
    
	vadd.s32  q13, q7, q10  ;t3 q13 = 22 * src[ 1] + 10 * src[3]
	vsub.s32  q14, q9, q8   ;t4 q14 = 22 * src[3] - 10 * src[ 1]	
    
	vadd.s32  q5, q11, q13  ;t1 + t3
	vsub.s32  q6, q12, q14  ;t2 - t4
	vadd.s32  q7, q12, q14  ;t2 + t4
	vsub.s32  q8, q11, q13  ;t1 - t3
    
	vshrn.s32 d2, q5, #7
	vshrn.s32 d4, q6, #7
	vshrn.s32 d6, q7, #7
	vshrn.s32 d8, q8, #7 
    
	;second four col 
	vmull.s16 q5,  d3, d0[0] ;q5  s0*17 	
	vmull.s16 q6,  d7, d0[0] ;q6  s2*17	
	vmull.s16 q7,  d5, d0[1] ;q7  s1*22	
	vmull.s16 q8,  d5, d0[2] ;q8  s1*10	
	vmull.s16 q9,  d9, d0[1] ;q9  s3*22	
	vmull.s16 q10, d9, d0[2] ;q10 s3*10 
	
	vadd.s32  q11, q5, q6  
  vadd.s32  q11, q11, q15 ;t1 q11 = 17 * (src[ 0] + src[2])+64
  vsub.s32  q12, q5, q6  
  vadd.s32  q12, q12, q15  ;t2 q12 = 17 * (src[ 0] - src[2])+64    
  vadd.s32  q13, q7, q10  ;t3 q13 = 22 * src[ 1] + 10 * src[3]
  vsub.s32  q14, q9, q8   ;t4 q14 = 22 * src[3] - 10 * src[ 1]
  vadd.s32  q5, q11, q13  ;t1 + t3
  vsub.s32  q6, q12, q14  ;t2 - t4
  vadd.s32  q7, q12, q14  ;t2 + t4
  vsub.s32  q8, q11, q13  ;t1 - t3
  
  vshrn.s32 d3, q5, #7
  vshrn.s32 d5, q6, #7
  vshrn.s32 d7, q7, #7
  vshrn.s32 d9, q8, #7 
  ;  ;;;;;;write to dest;;;;;;;
	cmp			r2, #0
	beq			IDCT_CPY_8x4
	cmp			r3, #0
	beq			IDCT_ONEREF_8x4
	
	vld1.64 {d10}, [r2], r4 
 	vld1.64 {d12}, [r2], r4
 	vld1.64 {d14}, [r2], r4 
 	vld1.64 {d16}, [r2]
 	
 	vld1.64 {d11}, [r3], r4 
 	vld1.64 {d13}, [r3], r4
 	vld1.64 {d15}, [r3], r4 
 	vld1.64 {d17}, [r3]
 	
 	vrhadd.u8 d10, d10, d11
 	vrhadd.u8 d11, d12, d13
 	vrhadd.u8 d12, d14, d15
 	vrhadd.u8 d13, d16, d17
 	
 	vaddw.u8	q1, q1, d10
	vaddw.u8	q2, q2, d11
	vaddw.u8	q3, q3, d12
	vaddw.u8	q4, q4, d13	
	bl IDCT_CPY_8x4
	
IDCT_ONEREF_8x4
	vld1.64 {d10}, [r2], r4 
 	vld1.64 {d11}, [r2], r4
 	vld1.64 {d12}, [r2], r4 
 	vld1.64 {d13}, [r2]
 			
	vaddw.u8	q1, q1, d10
	vaddw.u8	q2, q2, d11
	vaddw.u8	q3, q3, d12
	vaddw.u8	q4, q4, d13
IDCT_CPY_8x4
	vqmovun.s16	d2, q1
	vqmovun.s16	d4, q2
	vqmovun.s16	d6, q3
	vqmovun.s16	d8, q4	
						
	vst1.64	{d2}, [r0], r1
	vst1.64	{d4}, [r0], r1		
	vst1.64	{d6}, [r0], r1
	vst1.64	{d8}, [r0]
	
	ldmia	sp!, {r4,r5,r12, pc}
	
	align 4
voVC1InvTrans_4x8_DC_ARMV7
	stmdb   sp!, {r4,r5,r6,r12,lr}
	ldr			r4, [ sp, #20]  ; refstride
	ldr			r5, [ sp, #24]  ; block
	ldrsh     r5, [r5]   ;dc
	mov     r12,#17
	mul     r5,r5,r12  ;E = ((17*block[0] + 4)>>3) *12 + 64;
	add     r5,r5,#4
	mov     r5,r5,asr #3
	mov     r12,#12
	mul     r5,r5,r12
	add     r5,r5,#64  ;E
	add     r6,r5,#1   ;E+1
	mov     r5,r5,asr #7
	mov     r6,r6,asr #7
    
	
	;;;;;;;;;;;;;;;;
	;;;;;;;write to dest;;;;;;;
	cmp			r2, #0
	beq			IDCT_NOREF_4x8_DC
	cmp         r3, #0
	beq         IDCT_ONEREF_4x8_DC
	vld1.64 {d0}, [r2], r4 	
 	vld1.64 {d2}, [r2], r4 	
 	vld1.64 {d4}, [r2], r4 	
 	vld1.64 {d6}, [r2], r4  	
 	vld1.64 {d24}, [r2], r4  	
 	vld1.64 {d26}, [r2], r4  	
 	vld1.64 {d28}, [r2], r4  	
 	vld1.64 {d30}, [r2]
 	
 	vld1.64 {d1}, [r3], r4 
 	vld1.64 {d3}, [r3], r4
 	vld1.64 {d5}, [r3], r4
 	vld1.64 {d7}, [r3], r4
 	vld1.64 {d25}, [r3], r4
 	vld1.64 {d27}, [r3], r4
 	vld1.64 {d29}, [r3], r4
 	vld1.64 {d31}, [r3]
 	
 	vrhadd.u8 d0, d0, d1
 	vrhadd.u8 d1, d2, d3
 	vrhadd.u8 d2, d4, d5
 	vrhadd.u8 d3, d6, d7
 	vrhadd.u8 d4, d24, d25
 	vrhadd.u8 d5, d26, d27
 	vrhadd.u8 d6, d28, d29
 	vrhadd.u8 d7, d30, d31
 	
 	bl SHIFT_WRITE48
	
IDCT_ONEREF_4x8_DC
	vld1.64 {d0}, [r2], r4 
 	vld1.64 {d1}, [r2], r4
 	vld1.64 {d2}, [r2], r4 
 	vld1.64 {d3}, [r2], r4 
 	vld1.64 {d4}, [r2], r4 
 	vld1.64 {d5}, [r2], r4 
 	vld1.64 {d6}, [r2], r4 
 	vld1.64 {d7}, [r2]
 	
SHIFT_WRITE48
	vdup.s16  q15 ,r5
	vdup.s16  q14 ,r6 			
	vaddw.u8	q8, q15, d0
	vaddw.u8	q9, q15, d1
	vaddw.u8	q10, q15, d2
	vaddw.u8	q11, q15, d3
	vaddw.u8	q4, q14, d4
	vaddw.u8	q5, q14, d5
	vaddw.u8	q6, q14, d6
	vaddw.u8	q7, q14, d7
	
	vqmovun.s16	d16, q8
	vqmovun.s16	d18, q9
	vqmovun.s16	d20, q10
	vqmovun.s16	d22, q11
	vqmovun.s16	d8,  q4
	vqmovun.s16	d10, q5
	vqmovun.s16	d12, q6
	vqmovun.s16	d14, q7		
						
	vst1.32	{d16[0]}, [r0], r1
	vst1.32	{d18[0]}, [r0], r1		
	vst1.32	{d20[0]}, [r0], r1
	vst1.32	{d22[0]}, [r0], r1	
	vst1.32	{d8[0]},  [r0], r1
	vst1.32	{d10[0]}, [r0], r1	
	vst1.32	{d12[0]}, [r0], r1
	vst1.32	{d14[0]}, [r0]	
	
	ldmia   sp!, {r4,r5,r6,r12,pc}	
	
IDCT_NOREF_4x8_DC
	vdup.s16  q15 ,r5
	vdup.s16  q14 ,r6 
	vqmovun.s16	d16, q15
	vqmovun.s16	d8, q14
						
	vst1.32	{d16[0]}, [r0], r1
	vst1.32	{d16[0]}, [r0], r1		
	vst1.32	{d16[0]}, [r0], r1
	vst1.32	{d16[0]}, [r0], r1	
	vst1.32	{d8[0]},  [r0], r1
	vst1.32	{d8[0]}, [r0], r1	
	vst1.32	{d8[0]}, [r0], r1
	vst1.32	{d8[0]}, [r0]		
	;;;;;;;;;;;;;;;;;;;;;;;;;;;	
	
	ldmia   sp!, {r4,r5,r6,r12,pc}
	align 4
voVC1InvTrans_4x8_ARMV7
	stmdb   sp!, {r4,r5,r12,lr}
	ldr			r4, [ sp, #16]  ;refstride
	ldr			r5, [ sp, #20]  ;block
	vmov.s32	q1, #4
	mov r12,#16
	vld1.8	{q1}, [r5],r12	;0
	vld1.8	{q2}, [r5],r12	;1
	vld1.8	{q3}, [r5],r12	;2
	vld1.8	{q4}, [r5]   	;3
	;vld1.64	{d9},  [r4],r12	;4
	;vld1.64	{d15}, [r4],r12	;5
	;vld1.64	{d13}, [r4],r12	;6
	;vld1.64	{d11}, [r4]		;7
	
	;ldr			r12, =W_table2
	ldr			r12, [ sp, #28]  ;Table
	vld1.s16 	{q0}, [r12]!
	vmov.s32	q15, #4
	;;;;;;;;;;;;;;;
	;first four col
	vmull.s16 q5,  d2, d0[0] ;q5  s0*17 	
	vmull.s16 q6,  d6, d0[0] ;q6  s2*17	
	vmull.s16 q7,  d4, d0[1] ;q7  s1*22	
	vmull.s16 q8,  d4, d0[2] ;q8  s1*10	
	vmull.s16 q9,  d8, d0[1] ;q9  s3*22	
	vmull.s16 q10, d8, d0[2] ;q10 s3*10
	
  vadd.s32  q11, q5, q6  
  vadd.s32  q11, q11, q15 ;t1 q11 = 17 * (src[ 0] + src[2])+64    
  vsub.s32  q12, q5, q6  
  vadd.s32  q12, q12, q15  ;t2 q12 = 17 * (src[ 0] - src[2])+64    
  vadd.s32  q13, q7, q10  ;t3 q13 = 22 * src[ 1] + 10 * src[3]
  vsub.s32  q14, q9, q8   ;t4 q14 = 22 * src[3] - 10 * src[ 1] 
     
  vadd.s32  q10, q11, q13  ;t1 + t3
  vsub.s32  q13, q11, q13  ;t1 - t3    
  vsub.s32  q11, q12, q14  ;t2 - t4
  vadd.s32  q12, q12, q14  ;t2 + t4
  
  
  vshrn.s32 d8, q10, #3
  vshrn.s32 d10, q11, #3
  vshrn.s32 d12, q12, #3
  vshrn.s32 d14, q13, #3 
    	
	;second four col 
  ;vmull.s16 q1,  d3, d0[0] ;q1  s0*17 	
	;vmull.s16 q2,  d7, d0[0] ;q2  s2*17	
	;vmull.s16 q3,  d5, d0[1] ;q3  s1*22	
	;vmull.s16 q8,  d5, d0[2] ;q8  s1*10	
	;vmull.s16 q9,  d9, d0[1] ;q9  s3*22	
	;vmull.s16 q10, d9, d0[2] ;q10 s3*10 
	vmull.s16 q1,  d3, d0[0] ;q1  s0*17
	vmull.s16 q3,  d7, d0[0] ;q3  s2*17	
	vmull.s16 q8,  d5, d0[2] ;q8  s1*10	
	vmull.s16 q2,  d5, d0[1] ;q2  s1*22	
	vmull.s16 q9,  d9, d0[1] ;q9  s3*22
	vmull.s16 q10, d9, d0[2] ;q10 s3*10
	
	vadd.s32  q11, q1, q3  
  vadd.s32  q11, q11, q15 ;t1 q11 = 17 * (src[ 0] + src[2])+64
    vsub.s32  q12, q1, q3  
  vadd.s32  q12, q12, q15  ;t2 q12 = 17 * (src[ 0] - src[2])+64    
  vadd.s32  q13, q2, q10  ;t3 q13 = 22 * src[ 1] + 10 * src[3]
  vsub.s32  q14, q9, q8   ;t4 q14 = 22 * src[3] - 10 * src[ 1]
  
  vadd.s32  q10, q11, q13  ;t1 + t3
  vsub.s32  q13, q11, q13  ;t1 - t3
  vsub.s32  q11, q12, q14  ;t2 - t4
  vadd.s32  q12, q12, q14  ;t2 + t4    
  
  vshrn.s32 d9,  q10, #3
  vshrn.s32 d11, q11, #3
  vshrn.s32 d13, q12, #3
  vshrn.s32 d15, q13, #3   
    
  vtrn.s16  q4, q5
  vtrn.s16  q6, q7
  
  vtrn.s32  q4, q6
  vtrn.s32  q5, q7
    
    vswp.s32  d11,d15
  ;;;Row;;;;;;;;
  ;ldr			r12, =W_table
  ldr			r12, [ sp, #24]  ;Table
	vld1.s16 	{q0}, [r12]!
	vld1.s16 	{d4}, [r12] ;d4
	
  vmov.s32	q3,  #64	                            
	vmull.s16	q12, d9, D1[2]		  ;q12 = S4*W4    
	vmull.s16	q13, d8, D1[3]                     
	vadd.s32	q13, q13, q3          ;q13  S0*W4 + 64

	vmull.s16	q14, d11, D0[0]			;W7*S7                
	vmlal.s16	q14, d10, D0[1]			;A q14 = W1*S1 + W7*S7	
	vmull.s16	q15, d10, D0[0]			;W7*S1                
	vmlsl.s16	q15, d11, D0[1]			;B q15 = W7*S1 - W1*S7

	vmull.s16	q4, d14, D0[2]			;W3*S3                
	vmlal.s16	q4, d15, D0[3]			;C q4 = W3*S3 + W5*S5	
	vmull.s16	q5, d15, D0[2]			;W3*S5                
	vmlsl.s16	q5, d14, D0[3]			;D q5 = W3*S5 - W5*S5

	vmull.s16	q7, d13, D1[0]			;W6*S6               
	vmlal.s16	q7, d12, D1[1]			;G q7 = W6*S6 + W2*S2
	vmull.s16	q3, d12, D1[0]			;W6*S2               
	vmlsl.s16	q3, d13, D1[1]			;H q3 = W6*S2 - W2*S6

	vadd.s32	q6, q13, q12		;E q6  = S4*W4 + S0*W4 + 64
	vsub.s32	q13, q13, q12		;F q13 = S0*W4 - S4*W4 + 64

	vld1.s16 	{d0}, [r12] 		;d9 24576  21845

	vadd.s32	q12, q14, q4		;q12 Cd = A + C
	;vsub.s32	q14, q14, q4		;q14 Ad = A - C
	vmul.s32  q14, q14,d0[0]  ;24576 
	vmul.s32  q4,  q4, d0[1]  ;21845 
	vsub.s32	q14, q14, q4		 ;q14 Ad = A - C ---> Ad = 24576 *A - 21845*C
	
	vadd.s32	q4, q15, q5			;q4  Dd = B + D
	;vsub.s32	q15, q15, q5		;q15 Bd = B - D  
	vmul.s32  q15, q15,d0[0]  ;24576 
	vmul.s32  q5,  q5, d0[1]  ;21845 
	vsub.s32	q15, q15, q5		 ;q15 Bd = B - D ---> Bd = 24576 *B - 21845*D 	

	vadd.s32	q5, q6, q7			;q5 Fd = E + G
	vsub.s32	q6, q6, q7			;q6 Ed = E - G	
	
	vadd.s32	q7, q13, q3			;q7  Hd = F + H 
	vsub.s32	q13, q13, q3		;q13 Gd = F - H 
	
	vmov.s32	q0, #0x4000
	vadd.s32	q3, q14, q15		
	vadd.s32	q3, q3,  q0
	vshr.s32	q3, q3,  #15         ;q3 Bdd = (Ad + Bd + 16*1024) >> 15
	
	vsub.s32	q14,  q14, q15		
	vadd.s32	q14,  q14,  q0
	vshr.s32	q14,  q14,  #15			;q14 Add = (Ad - Bd + 16*1024) >> 15
	
	vmov.s32	q0, #0x0001

	vadd.s32	q15, q5, q12		;Blk[ 0] = (idct_t)((x7 + x1) >> 8);
	vsub.s32	q5, q5, q12			;Blk[56] = (idct_t)((x7 - x1) >> 8);
	vadd.s32	q12, q7, q3			;Blk[8] = (idct_t)((x3 + x2) >> 8);
	vsub.s32	q7, q7, q3			;Blk[48] = (idct_t)((x3 - x2) >> 8);
	vadd.s32	q3, q13, q14		;Blk[16] = (idct_t)((x0 + x4) >> 8);
	vsub.s32	q13, q13, q14		;Blk[40] = (idct_t)((x0 - x4) >> 8);
	vadd.s32	q14, q6, q4			;Blk[24] = (idct_t)((x8 + x6) >> 8);
	vsub.s32	q6, q6, q4			;Blk[32] = (idct_t)((x8 - x6) >> 8);
	
	
	vadd.s32	q6,  q6,  q0			  ;Blk[32]
	vadd.s32	q13, q13, q0		    ;Blk[40]
	vadd.s32	q7,  q7,  q0		    ;Blk[48]
	vadd.s32	q5,  q5,  q0		    ;Blk[56]	
	
	vshrn.s32	d16, q15, #7 ;0
	vshrn.s32	d18, q12, #7 ;8
	vshrn.s32	d20, q3,  #7 ;16
	vshrn.s32	d22, q14, #7 ;24
	vshrn.s32	d8,  q6,  #7 ;32
	vshrn.s32	d12, q7,  #7 ;48
	vshrn.s32	d14, q5,  #7 ;56
	vshrn.s32	d10, q13, #7 ;40
	
	;;;;;;write to dest;;;;;;;
	cmp			r2, #0
	beq			IDCT_CPY_4x8
	cmp			r3, #0
	beq			IDCT_ONEREF_4x8
	vld1.64 {d0}, [r2], r4 	
 	vld1.64 {d2}, [r2], r4 	
 	vld1.64 {d4}, [r2], r4 	
 	vld1.64 {d6}, [r2], r4  	
 	vld1.64 {d24}, [r2], r4  	
 	vld1.64 {d26}, [r2], r4  	
 	vld1.64 {d28}, [r2], r4  	
 	vld1.64 {d30}, [r2]
 	
 	vld1.64 {d1}, [r3], r4 
 	vld1.64 {d3}, [r3], r4
 	vld1.64 {d5}, [r3], r4
 	vld1.64 {d7}, [r3], r4
 	vld1.64 {d25}, [r3], r4
 	vld1.64 {d27}, [r3], r4
 	vld1.64 {d29}, [r3], r4
 	vld1.64 {d31}, [r3]
 	
 	vrhadd.u8 d0, d0, d1
 	vrhadd.u8 d1, d2, d3
 	vrhadd.u8 d2, d4, d5
 	vrhadd.u8 d3, d6, d7
 	vrhadd.u8 d4, d24, d25
 	vrhadd.u8 d5, d26, d27
 	vrhadd.u8 d6, d28, d29
 	vrhadd.u8 d7, d30, d31
 	
 	vaddw.u8	q8, q8, d0
	vaddw.u8	q9, q9, d1
	vaddw.u8	q10, q10, d2
	vaddw.u8	q11, q11, d3
	vaddw.u8	q4, q4, d4
	vaddw.u8	q5, q5, d5
	vaddw.u8	q6, q6, d6
	vaddw.u8	q7, q7, d7
	bl IDCT_CPY_4x8
	
IDCT_ONEREF_4x8
	vld1.8 {d0}, [r2], r4 
 	vld1.8 {d1}, [r2], r4
 	vld1.8 {d2}, [r2], r4 
 	vld1.8 {d3}, [r2], r4 
 	vld1.8 {d4}, [r2], r4 
 	vld1.8 {d5}, [r2], r4 
 	vld1.8 {d6}, [r2], r4 
 	vld1.8 {d7}, [r2]
 			
	vaddw.u8	q8, q8, d0
	vaddw.u8	q9, q9, d1
	vaddw.u8	q10, q10, d2
	vaddw.u8	q11, q11, d3
	vaddw.u8	q4, q4, d4
	vaddw.u8	q5, q5, d5
	vaddw.u8	q6, q6, d6
	vaddw.u8	q7, q7, d7
IDCT_CPY_4x8
	vqmovun.s16	d16, q8
	vqmovun.s16	d18, q9
	vqmovun.s16	d20, q10
	vqmovun.s16	d22, q11
	vqmovun.s16	d8, q4
	vqmovun.s16	d10, q5
	vqmovun.s16	d12, q6
	vqmovun.s16	d14, q7		
						
	vst1.32	{d16[0]}, [r0], r1
	vst1.32	{d18[0]}, [r0], r1		
	vst1.32	{d20[0]}, [r0], r1
	vst1.32	{d22[0]}, [r0], r1	
	vst1.32	{d8[0]},  [r0], r1
	vst1.32	{d10[0]}, [r0], r1	
	vst1.32	{d12[0]}, [r0], r1
	vst1.32	{d14[0]}, [r0]	
	ldmia	sp!, {r4,r5,r12, pc}	
	
	align 4
voVC1InvTrans_4x4_DC_ARMV7
	stmdb   sp!, {r4,r5,r12,lr}
	ldr			r4, [ sp, #16]  ; refstride
	ldr			r5, [ sp, #20]  ; block
	ldrsh     r5, [r5]   ;dc
	mov     r12,#17
	mul     r5,r5,r12  ;E = ((17*block[0] + 4)>>3) *12 + 64;
	add     r5,r5,#4
	mov     r5,r5,asr #3
	mul     r5,r5,r12
	add     r5,r5,#64  ;E
	mov     r5,r5,asr #7   
	
	;;;;;;;;;;;;;;;;
	;;;;;;;write to dest;;;;;;;
	cmp			r2, #0
	beq			IDCT_NOREF_4x4_DC
	cmp         r3, #0
	beq         IDCT_ONEREF_4x4_DC
	vld1.64 {d0}, [r2], r4 	
 	vld1.64 {d2}, [r2], r4 	
 	vld1.64 {d4}, [r2], r4 	
 	vld1.64 {d6}, [r2]
 	
 	vld1.64 {d1}, [r3], r4 
 	vld1.64 {d3}, [r3], r4
 	vld1.64 {d5}, [r3], r4
 	vld1.64 {d7}, [r3]
 	
 	vrhadd.u8 d0, d0, d1
 	vrhadd.u8 d1, d2, d3
 	vrhadd.u8 d2, d4, d5
 	vrhadd.u8 d3, d6, d7
 	
 	bl SHIFT_WRITE44
	
IDCT_ONEREF_4x4_DC
	vld1.64 {d0}, [r2], r4 
 	vld1.64 {d1}, [r2], r4
 	vld1.64 {d2}, [r2], r4 
 	vld1.64 {d3}, [r2]
 	
SHIFT_WRITE44
	vdup.s16  q15 ,r5  			
	vaddw.u8	q8, q15, d0
	vaddw.u8	q9, q15, d1
	vaddw.u8	q10, q15, d2
	vaddw.u8	q11, q15, d3
	
	vqmovun.s16	d16, q8
	vqmovun.s16	d18, q9
	vqmovun.s16	d20, q10
	vqmovun.s16	d22, q11		
						
	vst1.32	{d16[0]}, [r0], r1
	vst1.32	{d18[0]}, [r0], r1		
	vst1.32	{d20[0]}, [r0], r1
	vst1.32	{d22[0]}, [r0]	
	
	ldmia		sp!, {r4,r5,r12, pc}	
	
IDCT_NOREF_4x4_DC
	vdup.s16  q15 ,r5  
	vqmovun.s16	d16, q15
						
	vst1.32	{d16[0]}, [r0], r1
	vst1.32	{d16[0]}, [r0], r1		
	vst1.32	{d16[0]}, [r0], r1
	vst1.32	{d16[0]}, [r0]	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;	
	
	ldmia		sp!, {r4,r5,r12, pc}
	align 4
voVC1InvTrans_4x4_ARMV7
	stmdb   sp!, {r4,r5,r12,lr}
	ldr		r4, [ sp, #16]
	ldr		r5, [ sp, #20]
	mov 		r12,#16
	vld1.64	{d2}, [r5],r12	;0
	vld1.64	{d4}, [r5],r12	;1
	vld1.64	{d6}, [r5],r12	;2
	vld1.64	{d8}, [r5] 	;3
	;vld1.64	{d9},  [r4],r12	;4
	;vld1.64	{d15}, [r4],r12	;5
	;vld1.64	{d13}, [r4],r12	;6
	;vld1.64	{d11}, [r4]		;7
	
	;ldr			r12, =W_table2
	ldr			r12, [ sp, #28]  ;Table
	vld1.s16 	{q0}, [r12]!
	vmov.s32	q15, #4
	;;;;;;;;;;;;;;;;;;
	;first four col
	vmull.s16 q5,  d2, d0[0] ;q5  s0*17 	
	vmull.s16 q6,  d6, d0[0] ;q6  s2*17	
	vmull.s16 q7,  d4, d0[1] ;q7  s1*22	
	vmull.s16 q8,  d4, d0[2] ;q8  s1*10	
	vmull.s16 q9,  d8, d0[1] ;q9  s3*22	
	vmull.s16 q10, d8, d0[2] ;q10 s3*10
	
  vadd.s32  q11, q5, q6  
  vadd.s32  q11, q11, q15 ;t1 q11 = 17 * (src[ 0] + src[2])+64    
  vsub.s32  q12, q5, q6  
  vadd.s32  q12, q12, q15  ;t2 q12 = 17 * (src[ 0] - src[2])+64    
  vadd.s32  q13, q7, q10  ;t3 q13 = 22 * src[ 1] + 10 * src[3]
  vsub.s32  q14, q9, q8   ;t4 q14 = 22 * src[3] - 10 * src[ 1] 
     
  vadd.s32  q10, q11, q13  ;t1 + t3
  vsub.s32  q13, q11, q13  ;t1 - t3    
  vsub.s32  q11, q12, q14  ;t2 - t4
  vadd.s32  q12, q12, q14  ;t2 + t4

  vshrn.s32 d2, q10, #3
  vshrn.s32 d4, q11, #3
  vshrn.s32 d6, q12, #3
  vshrn.s32 d8, q13, #3     
  ;;;;;;;;;;;;;;;;;;;;;;
  vtrn.16   d2, d4
  vtrn.16   d6, d8
  
  vtrn.32   d2, d6
  vtrn.32   d4, d8    
  ;;;;;;;Row ;;;;;;;;;;;
  vmov.s32	q15, #64
  vmull.s16 q5,  d2, d0[0] ;q5  s0*17 	
	vmull.s16 q6,  d6, d0[0] ;q6  s2*17	
	vmull.s16 q7,  d4, d0[1] ;q7  s1*22	
	vmull.s16 q8,  d4, d0[2] ;q8  s1*10	
	vmull.s16 q9,  d8, d0[1] ;q9  s3*22	
	vmull.s16 q10, d8, d0[2] ;q10 s3*10
	
  vadd.s32  q11, q5, q6  
  vadd.s32  q11, q11, q15  ;t1 q11 = 17 * (src[ 0] + src[2])+64    
  vsub.s32  q12, q5, q6  
  vadd.s32  q12, q12, q15  ;t2 q12 = 17 * (src[ 0] - src[2])+64    
  vadd.s32  q13, q7, q10   ;t3 q13 = 22 * src[ 1] + 10 * src[3]
  vsub.s32  q14, q9, q8    ;t4 q14 = 22 * src[3] - 10 * src[ 1] 
     
  vadd.s32  q10, q11, q13  ;t1 + t3
  vsub.s32  q13, q11, q13  ;t1 - t3    
  vsub.s32  q11, q12, q14  ;t2 - t4
  vadd.s32  q12, q12, q14  ;t2 + t4

  vshrn.s32 d8,  q10, #7
  vshrn.s32 d10, q11, #7
  vshrn.s32 d12, q12, #7
  vshrn.s32 d14, q13, #7
  
  ;;;;;;write to dest;;;;;;;
	cmp			r2, #0
	beq			IDCT_CPY_4x4
	cmp			r3, #0
	beq			IDCT_ONEREF_4x4
	vld1.64 {d0}, [r2], r4 
 	vld1.64 {d2}, [r2], r4
 	vld1.64 {d4}, [r2], r4 
 	vld1.64 {d6}, [r2]
 	
 	vld1.64 {d1}, [r3], r4 
 	vld1.64 {d3}, [r3], r4
 	vld1.64 {d5}, [r3], r4 
 	vld1.64 {d7}, [r3]
 	
 	vrhadd.u8 d0, d0, d1
 	vrhadd.u8 d1, d2, d3
 	vrhadd.u8 d2, d4, d5
 	vrhadd.u8 d3, d6, d7
 	
 	vaddw.u8	q4, q4, d0
	vaddw.u8	q5, q5, d1
	vaddw.u8	q6, q6, d2
	vaddw.u8	q7, q7, d3
	bl IDCT_CPY_4x4
	
IDCT_ONEREF_4x4
	vld1.8 {d0}, [r2], r4 
 	vld1.8 {d1}, [r2], r4
 	vld1.8 {d2}, [r2], r4 
 	vld1.8 {d3}, [r2] 
 			
	vaddw.u8	q4, q4, d0
	vaddw.u8	q5, q5, d1
	vaddw.u8	q6, q6, d2
	vaddw.u8	q7, q7, d3
IDCT_CPY_4x4
	vqmovun.s16	d8,  q4
	vqmovun.s16	d10, q5
	vqmovun.s16	d12, q6
	vqmovun.s16	d14, q7		
						
	vst1.32	{d8[0]},  [r0], r1
	vst1.32	{d10[0]}, [r0], r1		
	vst1.32	{d12[0]}, [r0], r1
	vst1.32	{d14[0]}, [r0]	
	
	ldmia	sp!, {r4,r5,r12, pc}

;;;;;;;;;;;
    align 4
;W_table	
;		dcd 0x00100004			;w7 = D0.S16[0]	w1 = D0.S16[1]	 ;D0.S32[0]	
;		dcd 0x0009000f			;w3 = D0.S16[2] w5 = D0.S16[3]	 ;D0.S32[1]		
;		dcd 0x00100006			;w6 = D1.S16[0] w2 = D1.S16[1]   ;D1.S32[0]
;		dcd 0x000c000c          ;w4 = D1.S16[2] w0 = D1.S16[3] 	 ;D1.S32[1]	
;		dcd 0x00006000		                                     ;D4.S32[0]
;		dcd 0x00005555		                                     ;D4.S32[1]
;		dcd 0x00004000		                                     ;D5.S32[0]	
;W_table2	
;		dcd 0x00160011			
;		dcd 0x000a000a																										

	end