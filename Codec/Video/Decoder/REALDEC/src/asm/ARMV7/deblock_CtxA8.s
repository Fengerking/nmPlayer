;*****************************************************************************
;*																			*
;*		VisualOn, Inc. Confidential and Proprietary, 2009					*
;*																			*
;*****************************************************************************


	
;R0 = pQuantBuf
;R1 = pDst
;R2 = uDstPitch
;R3 = pPredSrc
;R14, never cover r14
;R12 = Tmp


	AREA	|.text|, CODE
	
	EXPORT	ARMV7_rv9_h_loop_filter_Y
	EXPORT	ARMV7_rv9_h_loop_filter_UV
	EXPORT	ARMV7_rv9_v_loop_filter_Y
	EXPORT	ARMV7_rv9_v_loop_filter_UV
	EXPORT	ARMV7_rv9_h_loop_filter_2block
	
	ALIGN 8		
Table_ditherLR
	dcw	64, 80, 32, 96, 48, 80, 64, 48, 80, 64, 80, 48, 96, 32, 80, 64, 64, 48, 96, 32, 80, 48, 48, 64, 64, 64, 80, 48, 32, 96, 48, 64

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	MACRO 
	M_rv9_deblock_normal
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	ands		r12, r5, r9
    ldr			r7, [sp, #OFFSET_alpha_h]
	lsreq		r8, #1				;c
	lsreq		r3, #1				;Cr
	lsreq		r4, #1				;Cl

;r0 = src
;r1 = step
;r3 = Cr / lim_q1	
;r4 = Cl / lim_p1	
;r5 = Al / filter_p1	
;r6 = betaX
;r7 = alpha
;r8 = c / lim_p0q0
;r9 = Ar / filter_q1	
;r10, r11, r12, r14 =  tmp
	
;d1 = R3,L3    
;d2 = R2,L2    
;d3 = R1,L1    
;q4 =  deltaR,deltaL   or diff_q1q0, diff_p1p0
;q5 =  deltaR2,deltaL2 or diff_q1q2, diff_p1p2
;q6 =  -diff,diff
;d30 = const_0    
;d31 = validFlag
;q9, q10, q11, q13, q14 = tmp
	
;static  void rv40_weak_loop_filter(U8 *src, const int step, int filter_p1, int filter_q1,int alpha, int beta,
;									int lim_p0q0,int lim_q1, int lim_p1,int diff_p1p0, int diff_q1q0,int diff_p1p2, int diff_q1q2)
;{
;	int t, u, diff;
;
;	t = src[0*step] - src[-1*step];
;	if(!t)
;		return;
;	u = (alpha * FFABS(t)) >> 7;
;	if(u > 3 - (filter_p1 && filter_q1))
;		return;

    vrev64.32	d4, d3			;d3 = R1,L1 ; d4 = L1,R1
	vdup.16		d16, r7			;alpha
	vsubl.u8	q7, d4, d3		;t = d14
	vmov.i16	d18, #3
	vabs.s16	d15, d14
	vcgt.u16	d31, d14, d30
	vmul.s16	d16, d15
	vdup.16		d17, r12		;(filter_p1 && filter_q1)
	vshl.s16	d14, #2
	vsub.s16	d18, d18, d17
	vcgt.u16	d19, d17, d30	;flag of (filter_p1 && filter_q1)
	vshr.s16	d16, #7			;u
    vrev64.32	d4, d2			;d2 = R2,L2 ; d4 = L2,R2
	vcge.s16	d18, d18, d16

;	t <<= 2;
;	if(filter_p1 && filter_q1)
;		t += src[-2*step] - src[1*step];
;	diff = CLIP_SYMM((t + 4) >> 3, lim_p0q0);

	vsubl.u8	q8, d2, d4		;tt = d16
	vand		d31, d18
	vadd.s16	d16, d16, d14
	vmov.i16	d17, #4
	vbit		d14, d16, d19
	vdup.16		d18, r8
	vadd.s16	d17, d17, d14
	vshr.s16	d17, #3 
	vsub.s16	d19, d30, d18	;-lim_p0q0
	vmax.s16	d12, d17, d19
	vmov		d14, r5, r9
	vmin.s16	d12, d12, d18	;diff = CLIP_SYMM((t + 4) >> 3, lim_p0q0);

;	if(filter_p1 || filter_q1){
;		src[-1*step] = ClampTbl[CLAMP_BIAS + src[-1*step] + diff];
;		src[ 0*step] = ClampTbl[CLAMP_BIAS + src[ 0*step] - diff];
;	}

	vmovl.u8	q8, d3
	vsub.s16	d13, d30, d12	;-diff
	vcgt.u32	d14, d14, d30
	vadd.s16	q8, q8, q6
	vmovl.s32	q7, d14			;flag of (filter_q1, filter_p1)
	vqshrun.s16	d19, q8, #0
	vorr		d21, d14, d15	;(filter_p1 || filter_q1)

;	if(FFABS(diff_p1p2) <= beta && filter_p1){
;		t = (diff_p1p0 + diff_p1p2 - diff) >> 1;
;		src[-2*step] = ClampTbl[CLAMP_BIAS + src[-2*step] - CLIP_SYMM(t, lim_p1)];
;	}
;	if(FFABS(diff_q1q2) <= beta && filter_q1){
;		t = (diff_q1q0 + diff_q1q2 + diff) >> 1;
;		src[ 1*step] = ClampTbl[CLAMP_BIAS + src[ 1*step] - CLIP_SYMM(t, lim_q1)];
;	}
;}
	
	vswp		d12, d13		;q6 = diff, -diff
	vadd.s16	q4, q4, q5		;(diff_q1q0,diff_p1p0) + (diff_q1q2,diff_p1p2)
	vdup.16		d24, r4
	vdup.16		d25, r3			;q12 = lim_q1, lim_p1
	vhadd.s16	q4, q4, q6		;th,tl
	vsub.s16	d26, d30, d24	;q13 = -(lim_q1, lim_p1)
	vsub.s16	d27, d30, d25
	vmin.s16	q4, q4, q12		;CLIP_SYMM()
	vdup.16		q11, r6			;beta
	vmax.s16	q4, q4, q13
	vmovl.u8	q14, d2
	vabs.s16	q5, q5
	vsub.s16	q14, q14, q4
	vcge.u16	q11, q11, q5
	vqshrun.s16	d18, q14, #0
	vand		q11, q7
	vmovn.i16	d31, q15
	vmovn.i16	d20, q11
	vdup.32		q15, d31[1]
	vand		q15, q10
	vbit		q1, q9, q15	
	MEND

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	MACRO 
	M_rv9_deblock_strong_Y
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;			for(z=0;z<4;z++) 
;			{
;				L1 = (I32) pL1[z];
;				R1 = (I32) r[z];
;				delta = (R1 -L1);
;				if(delta == 0) continue;
;				n = (abs(delta)*alpha)>>7;
;				if(n>1) continue;
;				L2 = (I32) pL2[z];
;				R2 = (I32) pR2[z];
;				L3 = (I32) pL3[z];
;				R3 = (I32) pR3[z];
;				L4 = (I32) pL4[z];
;				R4 = (I32) pR4[z];
;				d1 = ditherL[uMBPos+z];
;				d2 = ditherR[uMBPos+z];
;				t1 = (U8)((25*(L3 + R2) + 26*(L2 + L1 + R1) + d1)>>7);
;				t2 = (U8)((25*(R3 + L2) + 26*(R2 + R1 + L1) + d2)>>7);

    vdup.16		d5, r5
    vabs.s16	d6, d6
    vmul.s16	d5, d6, d5
	add			r7, r7, r2, lsl #1	;ditherL[uMBPos]
	ldr			r12, =0x1a0019
	add			r9, r7, #16<<1		;ditherR[uMBPos]
    vrev64.32	d9, d2				;d2 = R2,L2 ; d9 = L2,R2
    vdup.32		d7, r12
	vaddl.u8	q5, d1, d9			;R3 + L2, L3 + R2
	vaddl.u8	q6, d2, d3			;R2 + R1, L2 + L1
    vmov.s16	d8, #1
	vaddw.u8	q6, d4				;(R2 + R1 + L1), (L2 + L1 + R1)
	vmul.u16	q5, d7[0]			;*25
    vshr.s16	d5, #7				;n
    vcge.s16	d8, d8, d5
	vld1.16		d14, [r7]			;d1
    vand		d31, d8
	vmla.u16	q5, q6, d7[1]		;*25 + *26
    vcgt.u16	d5, d5, d30
	vmovn.i16	d31, q15
	vld1.16		d15, [r9]			;d2
	vdup.32		d31, d31[1]
	vhadd.u16	q5, q5, q7			;( +d1)>>1
	
;				if(n) {
;					if ((I32)t1 > (L1+c))
;						t1 = (U8)(L1+c);
;					if ((I32)t1 < (L1-c))
;						t1 = (U8)(L1-c);					
;					if ((I32)t2 > (R1+c))
;						t2 = (U8)(R1+c);
;					if ((I32)t2 < (R1-c))
;						t2 = (U8)(R1-c);
;				} 
;				pL1[z] = t1;
;				r[z] = t2;

	vdup.8		d8, r8				;c
	vshr.u16	q5, #6				;q5 = t2,t1
	vmov.s16	q14, #0xff
	vaddl.u8	q10, d3, d8		;(R1+c), (L1+c)
	vand		q5, q14
	vsubl.u8	q8, d3, d8		;(R1-c), (L1-c)
	vmin.s16	q9, q5, q10
	vrev64.32	d20, d3
	vmax.s16	q9, q9, q8
	vshl.s16	d4, d5, #0		;n
	vaddl.u8	q8, d0, d20		;(R4 + L1),(L4 + R1)
	vbit		q5, q9, q2		;t2, t1

;				t1 = (U8)((25*(L4 + R1) + 26*(L3 + L2 + t1) + d1)>>7);
;				t2 = (U8)((25*(R4 + L1) + 26*(R3 + R2 + t2) + d2)>>7);
;				if(n) {
;					if ((I32)t1 > (L2+c))
;						t1 = (U8)(L2+c);
;					if ((I32)t1 < (L2-c))
;						t1 = (U8)(L2-c);					
;					if ((I32)t2 > (R2+c))
;						t2 = (U8)(R2+c);
;					if ((I32)t2 < (R2-c))
;						t2 = (U8)(R2-c);
;				} 
;				pL2[z] = t1;
;				pR2[z] = t2;

	vmla.s16	q7, q8, d7[0]		;*25 + d2/d1
	vaddl.u8	q6, d1, d2			;R3 + R2, L3 + L2
	vmovn.i16	d18, q5
	vadd.u16	q5, q6, q5			; + t2/t1
	vbit		d3, d18, d31		;pL1[z] = t1;r[z] = t2;
	vmla.s16	q7, q5, d7[1]		;+ *26
	vaddl.u8	q10, d2, d8			;(R2+c), (L2+c)
	vsubl.u8	q8, d2, d8			;(R2-c), (L2-c)
	vaddl.u8	q6, d0, d1			;R3 + R4, L3 + L4
	vaddl.u8	q5, d1, d3			;R3 + r[z],L3 + pL1[z]
	vshr.s16	q7, #7				;q7 = t2,t1
	vand		q7, q14
	vmin.s16	q9, q7, q10
	vmov.i16	q11, #64
	vmax.s16	q9, q9, q8
	
;			//	if(!bChroma)
;			//	{
;					pL3[z] = (U8)((26*(L3 + L4 + t1) + 25*(L3 + pL1[z]) + 64)>>7);
;					pR3[z] = (U8)((26*(R3 + R4 + t2) + 25*(R3 +   r[z]) + 64)>>7);
;			//	}

	vmla.s16	q11, q5, d7[0]		;*25 + d2/d1
	vbit		q7, q9, q2			;t2, t1
	vmovn.i16	d18, q7
	vadd.u16	q6, q6, q7			;+ t2/t1
	vbit		d2, d18, d31		;pL2[z] = t1;pR2[z] = t2;
	vmla.s16	q11, q6, d7[1]		;+ *26
	vshrn.s16	d16, q11, #7			
	vbit		d1, d16, d31		;pL3[z] = ;pR3[z] = ;
	
	MEND

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	MACRO 
	M_rv9_deblock_strong_UV
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    vdup.16		d5, r5
    vabs.s16	d6, d6
    vmul.s16	d5, d6, d5
	add			r7, r7, r2, lsl #1	;ditherL[uMBPos]
	ldr			r12, =0x1a0019
	add			r9, r7, #16<<1		;ditherR[uMBPos]
    vrev64.32	d9, d2				;d2 = R2,L2 ; d9 = L2,R2
    vdup.32		d7, r12
	vaddl.u8	q5, d1, d9			;R3 + L2, L3 + R2
	vaddl.u8	q6, d2, d3			;R2 + R1, L2 + L1
    vmov.s16	d8, #1
	vaddw.u8	q6, d4				;(R2 + R1 + L1), (L2 + L1 + R1)
	vmul.u16	q5, d7[0]			;*25
    vshr.s16	d5, #7				;n
    vcge.s16	d8, d8, d5
	vld1.16		d14, [r7]			;d1
    vand		d31, d8
	vmla.u16	q5, q6, d7[1]		;*25 + *26
    vcgt.u16	d5, d5, d30
	vmovn.i16	d31, q15
	vld1.16		d15, [r9]			;d2
	vdup.32		d31, d31[1]
	vhadd.u16	q5, q5, q7			;( +d1)>>1

	vdup.8		d8, r8				;c
	vshr.u16	q5, #6				;q5 = t2,t1
	vmov.s16	q14, #0xff
	vaddl.u8	q10, d3, d8			;(R1+c), (L1+c)
	vand		q5, q14
	vsubl.u8	q8, d3, d8			;(R1-c), (L1-c)
	vmin.s16	q9, q5, q10
	vrev64.32	d20, d3
	vmax.s16	q9, q9, q8
	vshl.s16	d4, d5, #0			;n
	vaddl.u8	q8, d0, d20			;(R4 + L1),(L4 + R1)
	vbit		q5, q9, q2			;t2, t1

	vmla.s16	q7, q8, d7[0]		;*25 + d2/d1
	vaddl.u8	q6, d1, d2			;R3 + R2, L3 + L2
	vmovn.i16	d18, q5
	vadd.u16	q5, q6, q5			; + t2/t1
	vbit		d3, d18, d31		;pL1[z] = t1;r[z] = t2;
	vmla.s16	q7, q5, d7[1]		;+ *26
	vaddl.u8	q10, d2, d8			;(R2+c), (L2+c)
	vsubl.u8	q8, d2, d8			;(R2-c), (L2-c)
	vshr.s16	q7, #7				;q7 = t2,t1
	vand		q7, q14
	vmin.s16	q9, q7, q10
	vmax.s16	q9, q9, q8
	vbit		q7, q9, q2			;t2, t1
	vmovn.i16	d18, q7
	vbit		d2, d18, d31		;pL2[z] = t1;pR2[z] = t2;
	
	MEND

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;static void rv9_h_loop_filter(
;								U8 *pPels,			/* first of 4 */
;								U32 uPitch,
;								U32 uMBPos,			/* offset in position from MB zero pixel for this edge (0,4,8,or 12) */
;								U32 Cr,				/* clip for right */
;								U32 Cl,				/* clip for left */
;								I32 alpha,
;								I32 betaX,
;								I32 beta2,
;								RV_Boolean bChroma,
;								RV_Boolean bStrong		/* true when strong filter enabled */
;								)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
ARMV7_rv9_h_loop_filter_Y	PROC

OFFSET_SaveRegisters_h	EQU		36	
OFFSET_Cl_h				EQU		OFFSET_SaveRegisters + 0
OFFSET_alpha_h			EQU		OFFSET_SaveRegisters + 4
OFFSET_betaX_h			EQU		OFFSET_SaveRegisters + 8
OFFSET_beta2_h			EQU		OFFSET_SaveRegisters + 12
OFFSET_bStrong_h		EQU		OFFSET_SaveRegisters + 20

    stmdb		sp!, {r4 - r11, r14}

; d1 = R3,L3    
; d2 = R2,L2    
; d3 = R1,L1    
; d7 = const_0x0000,0000,0020,0019; 26/25    
; q14 = const_0x00ff00ff...
; d30 = const_0    
; d31 = validFlag    
;r3 = Cr	
;r4 = Cl	
;r5 = alpha
;r6 = betaX
;r7 = beta2
;r14 = bStrong / c
     
    sub			r12, r0, r1, lsl #1
    sub			r12, r12, r1
    vld1.32		{d1[0]}, [r12], r1
    vld1.32		{d2[0]}, [r12], r1
    vld1.32		{d3[0]}, [r12], r1
    vld1.32		{d3[1]}, [r12], r1
    vld1.32		{d2[1]}, [r12], r1
    vld1.32		{d1[1]}, [r12]
    vmov.s32	d30, #0
    ldr			r6, [sp, #OFFSET_betaX_h]
    ldr			r4, [sp, #OFFSET_Cl_h]
    ldr			r7, [sp, #OFFSET_beta2_h]
    ldr			r14, [sp, #OFFSET_bStrong_h]
    
;deltaL  = L2 - L1    
;deltaL2 = L2 - L3
;deltaR  = R2 - R1
;deltaR2 = R2 - R3

	vsubl.u8	q4, d2, d3			;deltaR,deltaL
	vsubl.u8	q5, d2, d1			;deltaR2,deltaL2

;	Al = Ar = 1;
;	delta = deltaL[0]+deltaL[1]+deltaL[2]+deltaL[3];
;	if (abs(delta) >= (betaX << 2))	
;		Al = 0;	
;	delta = deltaR[0]+deltaR[1]+deltaR[2]+deltaR[3];
;	if (abs(delta) >= (betaX << 2)) 	
;		Ar = 0;	
	
	vpaddl.s16	q6, q4
	vpaddl.s16	q7, q5
	vpaddl.s32	q6, q6
	vpaddl.s32	q7, q7
	vmovn.i32	d16, q6
	lsl			r12, r6, #2
	vmovn.i32	d17, q7
	vdup.i32	d18, r12
	vdup.i32	d19, r7			;q9 = beta2, betaX
	vabs.s32	q8, q8			;abs(deltaR,deltaL)
	vcgt.s32	q6, q9, q8
	vmov.s32	q10, #0x01
	vand		q6, q10
	
	vmov.i32	r5, r9, d12		;r9=Ar, r5=Al
	vmov.i32	r10, r11, d13	;r11=b3SmoothRight2, r10=b3SmoothLeft2
	
;	if((Al+Ar) > 0)
;	{

	adds		r8, r5, r9	
	ldmleia		sp!, {r4 - r11, pc}		;if((Al+Ar) <= 0) return;
	
;		c = Al + Ar + ((U8)(Cr + Cl)>>1) + 1;
;		if (bStrong){
;			b3SmoothLeft = (Al == 1);
;			b3SmoothRight = (Ar == 1);
;		}else{
;			/* not strong, force select weak below */
;			b3SmoothLeft = FALSE;
;			b3SmoothRight = FALSE;
;		}
;		if (b3SmoothLeft){
;			delta = deltaL2[0]+deltaL2[1]+deltaL2[2]+deltaL2[3];
;			if (abs(delta) >= beta2) 
;				b3SmoothLeft = FALSE;
;		}
;		if (b3SmoothRight){
;			delta = deltaR2[0]+deltaR2[1]+deltaR2[2]+deltaR2[3];
;			if (abs(delta) >= beta2) 
;				b3SmoothRight = FALSE;
;		}

	cmp			r14, #0
	movne		r7, r5		;b3SmoothLeft
	movne		r12, r9		;b3SmoothRight
	moveq		r7, #0 
	moveq		r12, #0
	cmp			r7, #0
	andne		r7, r10
	cmp			r12, #0
	add			r10, r3, r4
	add			r8, #1
	andne		r12, r11
	add			r8, r8, r10, lsr #1		;c

;		if(b3SmoothLeft && b3SmoothRight) {

	ands		r7, r12
	beq			h_loop_filter_Y_normal
	
h_loop_filter_Y_Strong

    sub			r12, r0, r1, lsl #2
    add			r11, r0, r1, lsl #1
    ldr			r5, [sp, #OFFSET_alpha_h]
    add			r11, r11, r1
	ldr			r7, =Table_ditherLR
    vrev64.32	d4, d3				; d3 = R1,L1 ; d4 = L1,R1
    vld1.32		{d0[0]}, [r12]
    vsubl.u8	q3, d4, d3
    vld1.32		{d0[1]}, [r11]
    vcgt.u16	d31, d6, d30			

	M_rv9_deblock_strong_Y
	
    sub			r12, r0, r1, lsl #1
    sub			r12, r12, r1
    vst1.32		{d1[0]}, [r12], r1
    vst1.32		{d2[0]}, [r12], r1
    vst1.32		{d3[0]}, [r12], r1
    vst1.32		{d3[1]}, [r12], r1
    vst1.32		{d2[1]}, [r12], r1
    vst1.32		{d1[1]}, [r12]
    
	ldmia		sp!, {r4 - r11, pc}		

;			} 
;		} 
;		else {

h_loop_filter_Y_normal

;			if(!Al || !Ar){
;				c >>= 1;
;				Cl>>= 1;
;				Cr>>= 1;
;			}
;			for(z = 0; z < 4; z++, r++){
;				rv40_weak_loop_filter(r, uPitch, Al, Ar, alpha, betaX, c, Cr, Cl,deltaL[z], deltaR[z],deltaL2[z], deltaR2[z]);
;			}
	
	M_rv9_deblock_normal
	
    sub			r12, r0, r1, lsl #1
    vst1.32		{d2[0]}, [r12], r1
    vst1.32		{d3[0]}, [r12], r1
    vst1.32		{d3[1]}, [r12], r1
    vst1.32		{d2[1]}, [r12]

	ldmia		sp!, {r4 - r11, pc}		
	
;		}
;	} //if((Al+Ar) > 0)	

	ENDP	;ARMV7_rv9_h_loop_filter_Y


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	
ARMV7_rv9_h_loop_filter_UV	PROC
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	

    stmdb		sp!, {r4 - r11, r14}

; d1 = R3,L3    
; d2 = R2,L2    
; d3 = R1,L1    
; d7 = const_0x0000,0000,0020,0019; 26/25    
; q14 = const_0x00ff00ff...
; d30 = const_0    
; d31 = validFlag    
;r3 = Cr	
;r4 = Cl	
;r5 = alpha
;r6 = betaX
;r7 = beta2
;r14 = bStrong / c
     
    sub			r12, r0, r1, lsl #1
    sub			r12, r12, r1
    vld1.32		{d1[0]}, [r12], r1
    vld1.32		{d2[0]}, [r12], r1
    vld1.32		{d3[0]}, [r12], r1
    vld1.32		{d3[1]}, [r12], r1
    vld1.32		{d2[1]}, [r12], r1
    vld1.32		{d1[1]}, [r12]
    vmov.s32	d30, #0
    ldr			r6, [sp, #OFFSET_betaX_h]
    ldr			r4, [sp, #OFFSET_Cl_h]
    ldr			r7, [sp, #OFFSET_beta2_h]
    ldr			r14, [sp, #OFFSET_bStrong_h]
    
;deltaL  = L2 - L1    
;deltaL2 = L2 - L3
;deltaR  = R2 - R1
;deltaR2 = R2 - R3

	vsubl.u8	q4, d2, d3			;deltaR,deltaL
	vsubl.u8	q5, d2, d1			;deltaR2,deltaL2

;	Al = Ar = 1;
;	delta = deltaL[0]+deltaL[1]+deltaL[2]+deltaL[3];
;	if (abs(delta) >= (betaX << 2))	
;		Al = 0;	
;	delta = deltaR[0]+deltaR[1]+deltaR[2]+deltaR[3];
;	if (abs(delta) >= (betaX << 2)) 	
;		Ar = 0;	
	
	vpaddl.s16	q6, q4
	vpaddl.s16	q7, q5
	vpaddl.s32	q6, q6
	vpaddl.s32	q7, q7
	vmovn.i32	d16, q6
	lsl			r12, r6, #2
	vmovn.i32	d17, q7
	vdup.i32	d18, r12
	vdup.i32	d19, r7			;q9 = beta2, betaX
	vabs.s32	q8, q8			;abs(deltaR,deltaL)
	vcgt.s32	q6, q9, q8
	vmov.s32	q10, #0x01
	vand		q6, q10
	
	vmov.i32	r5, r9, d12		;r9=Ar, r5=Al
	vmov.i32	r10, r11, d13	;r11=b3SmoothRight2, r10=b3SmoothLeft2
	
;	if((Al+Ar) > 0)
;	{

	adds		r8, r5, r9	
	ldmleia		sp!, {r4 - r11, pc}		;if((Al+Ar) <= 0) return;
	
;		c = Al + Ar + ((U8)(Cr + Cl)>>1) + 1;
;		if (bStrong){
;			b3SmoothLeft = (Al == 1);
;			b3SmoothRight = (Ar == 1);
;		}else{
;			/* not strong, force select weak below */
;			b3SmoothLeft = FALSE;
;			b3SmoothRight = FALSE;
;		}
;		if (b3SmoothLeft){
;			delta = deltaL2[0]+deltaL2[1]+deltaL2[2]+deltaL2[3];
;			if (abs(delta) >= beta2) 
;				b3SmoothLeft = FALSE;
;		}
;		if (b3SmoothRight){
;			delta = deltaR2[0]+deltaR2[1]+deltaR2[2]+deltaR2[3];
;			if (abs(delta) >= beta2) 
;				b3SmoothRight = FALSE;
;		}

	cmp			r14, #0
	movne		r7, r5		;b3SmoothLeft
	movne		r12, r9		;b3SmoothRight
	moveq		r7, #0 
	moveq		r12, #0
	cmp			r7, #0
	andne		r7, r10
	cmp			r12, #0
	add			r10, r3, r4
	add			r8, #1
	andne		r12, r11
	add			r8, r8, r10, lsr #1		;c

;		if(b3SmoothLeft && b3SmoothRight) {

	ands		r7, r12
	beq			h_loop_filter_UV_normal
	
h_loop_filter_UV_Strong

    sub			r12, r0, r1, lsl #2
    add			r11, r0, r1, lsl #1
    ldr			r5, [sp, #OFFSET_alpha_h]
    add			r11, r11, r1
	ldr			r7, =Table_ditherLR
    vrev64.32	d4, d3				; d3 = R1,L1 ; d4 = L1,R1
    vld1.32		{d0[0]}, [r12]
    vsubl.u8	q3, d4, d3
    vld1.32		{d0[1]}, [r11]
    vcgt.u16	d31, d6, d30			

	M_rv9_deblock_strong_UV
	
    sub			r12, r0, r1, lsl #1
    vst1.32		{d2[0]}, [r12], r1
    vst1.32		{d3[0]}, [r12], r1
    vst1.32		{d3[1]}, [r12], r1
    vst1.32		{d2[1]}, [r12]
    
	ldmia		sp!, {r4 - r11, pc}		

;			} 
;		} 
;		else {

h_loop_filter_UV_normal

;			if(!Al || !Ar){
;				c >>= 1;
;				Cl>>= 1;
;				Cr>>= 1;
;			}
;			for(z = 0; z < 4; z++, r++){
;				rv40_weak_loop_filter(r, uPitch, Al, Ar, alpha, betaX, c, Cr, Cl,deltaL[z], deltaR[z],deltaL2[z], deltaR2[z]);
;			}
	
	M_rv9_deblock_normal
	
    sub			r12, r0, r1, lsl #1
    vst1.32		{d2[0]}, [r12], r1
    vst1.32		{d3[0]}, [r12], r1
    vst1.32		{d3[1]}, [r12], r1
    vst1.32		{d2[1]}, [r12]

	ldmia		sp!, {r4 - r11, pc}		
	
;		}
;	} //if((Al+Ar) > 0)	

	ENDP	;ARMV7_rv9_h_loop_filter_UV


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;static void rv9_v_loop_filter(
;								U8 *pPels,			/* first of 4 */
;								U32 uPitch,
;								U32 uMBPos,			/* offset in position from MB zero pixel for this edge (0,4,8,or 12) */
;								U32 Cr,				/* clip for right */
;								U32 Cl,				/* clip for left */
;								I32 alpha,
;								I32 betaX,
;								I32 beta2,
;								RV_Boolean bChroma,
;								RV_Boolean bStrong		/* true when strong filter enabled */
;								)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
ARMV7_rv9_v_loop_filter_Y	PROC

    stmdb		sp!, {r4 - r11, r14}

; d1 = R3,L3    
; d2 = R2,L2    
; d3 = R1,L1    
; d7 = const_0x0000,0000,0020,0019; 26/25    
; q14 = const_0x00ff00ff...
; d30 = const_0    
; d31 = validFlag    
;r3 = Cr	
;r4 = Cl	
;r5 = alpha
;r6 = betaX
;r7 = beta2
;r14 = bStrong / c
     
;    sub			r11, r0, #4
;    mov			r12, r0
;    vld4.8		{d0[0],d1[0],d2[0],d3[0]}, [r11], r1
;    vld4.8		{d0[1],d1[1],d2[1],d3[1]}, [r11], r1
;    vld4.8		{d0[2],d1[2],d2[2],d3[2]}, [r11], r1
;    vld4.8		{d0[3],d1[3],d2[3],d3[3]}, [r11]
;    vld4.8		{d3[4],d4[4],d5[4],d6[4]}, [r12], r1
;    vld4.8		{d3[5],d4[5],d5[5],d6[5]}, [r12], r1
;    vld4.8		{d3[6],d4[6],d5[6],d6[6]}, [r12], r1
;    vld4.8		{d3[7],d4[7],d5[7],d6[7]}, [r12]
;    vrev64.32	d4, d4
;    vrev64.32	d5, d5
;    vrev64.32	d6, d6
;    vtrn.32		d2, d4
;    vtrn.32		d1, d5
;    vtrn.32		d0, d6

    sub			r12, r0, #4
    vld1.32		d0, [r12], r1
    vld1.32		d1, [r12], r1
    vld1.32		d2, [r12], r1
    vld1.32		d3, [r12]
    
    ldr			r6, [sp, #OFFSET_betaX_h]
    ldr			r4, [sp, #OFFSET_Cl_h]
    ldr			r7, [sp, #OFFSET_beta2_h]
    ldr			r14, [sp, #OFFSET_bStrong_h]
    vmov.s32	d30, #0
    
	vtrn.8		d0, d1
	vtrn.8		d2, d3
	vtrn.16		d0, d2
	vtrn.16		d1, d3
	vrev64.32	d0, d0
	vrev64.32	d1, d1
	vtrn.32		d3, d0
	vtrn.32		d2, d1
	vrev64.32	d0, d0
	vrev64.32	d1, d1
    
;deltaL  = L2 - L1    
;deltaL2 = L2 - L3
;deltaR  = R2 - R1
;deltaR2 = R2 - R3

	vsubl.u8	q4, d2, d3			;deltaR,deltaL
	vsubl.u8	q5, d2, d1			;deltaR2,deltaL2

;	Al = Ar = 1;
;	delta = deltaL[0]+deltaL[1]+deltaL[2]+deltaL[3];
;	if (abs(delta) >= (betaX << 2))	
;		Al = 0;	
;	delta = deltaR[0]+deltaR[1]+deltaR[2]+deltaR[3];
;	if (abs(delta) >= (betaX << 2)) 	
;		Ar = 0;	
	
	vpaddl.s16	q6, q4
	vpaddl.s16	q7, q5
	vpaddl.s32	q6, q6
	vpaddl.s32	q7, q7
	vmovn.i32	d16, q6
	lsl			r12, r6, #2
	vmovn.i32	d17, q7
	vdup.i32	d18, r12
	vdup.i32	d19, r7			;q9 = beta2, betaX
	vabs.s32	q8, q8			;abs(deltaR,deltaL)
	vcgt.s32	q6, q9, q8
	vmov.s32	q10, #0x01
	vand		q6, q10
	
	vmov.i32	r5, r9, d12		;r9=Ar, r5=Al
	vmov.i32	r10, r11, d13	;r11=b3SmoothRight2, r10=b3SmoothLeft2
	
;	if((Al+Ar) > 0)
;	{

	adds		r8, r5, r9	
	ldmleia		sp!, {r4 - r11, pc}		;if((Al+Ar) <= 0) return;
	
;		c = Al + Ar + ((U8)(Cr + Cl)>>1) + 1;
;		if (bStrong){
;			b3SmoothLeft = (Al == 1);
;			b3SmoothRight = (Ar == 1);
;		}else{
;			/* not strong, force select weak below */
;			b3SmoothLeft = FALSE;
;			b3SmoothRight = FALSE;
;		}
;		if (b3SmoothLeft){
;			delta = deltaL2[0]+deltaL2[1]+deltaL2[2]+deltaL2[3];
;			if (abs(delta) >= beta2) 
;				b3SmoothLeft = FALSE;
;		}
;		if (b3SmoothRight){
;			delta = deltaR2[0]+deltaR2[1]+deltaR2[2]+deltaR2[3];
;			if (abs(delta) >= beta2) 
;				b3SmoothRight = FALSE;
;		}

	cmp			r14, #0
	movne		r7, r5		;b3SmoothLeft
	movne		r12, r9		;b3SmoothRight
	moveq		r7, #0 
	moveq		r12, #0
	cmp			r7, #0
	andne		r7, r10
	cmp			r12, #0
	add			r10, r3, r4
	add			r8, #1
	andne		r12, r11
	add			r8, r8, r10, lsr #1		;c

;		if(b3SmoothLeft && b3SmoothRight) {

	ands		r7, r12
	beq			v_loop_filter_Y_normal
	
v_loop_filter_Y_Strong

    ldr			r5, [sp, #OFFSET_alpha_h]
	ldr			r7, =Table_ditherLR
    vrev64.32	d4, d3				; d3 = R1,L1 ; d4 = L1,R1
    vsubl.u8	q3, d4, d3
    vcgt.u16	d31, d6, d30			

	M_rv9_deblock_strong_Y
	
    vdup.32		d4, d2[1]
    vdup.32		d5, d1[1]
    sub			r11, r0, #3
    mov			r12, r0
    vst3.8		{d1[0],d2[0],d3[0]}, [r11], r1
    vst3.8		{d3[4],d4[4],d5[4]}, [r12], r1
    vst3.8		{d1[1],d2[1],d3[1]}, [r11], r1
    vst3.8		{d3[5],d4[5],d5[5]}, [r12], r1
    vst3.8		{d1[2],d2[2],d3[2]}, [r11], r1
    vst3.8		{d3[6],d4[6],d5[6]}, [r12], r1
    vst3.8		{d1[3],d2[3],d3[3]}, [r11]
    vst3.8		{d3[7],d4[7],d5[7]}, [r12]    
    
	ldmia		sp!, {r4 - r11, pc}		

;			} 
;		} 
;		else {

v_loop_filter_Y_normal

;			if(!Al || !Ar){
;				c >>= 1;
;				Cl>>= 1;
;				Cr>>= 1;
;			}
;			for(z = 0; z < 4; z++, r++){
;				rv40_weak_loop_filter(r, uPitch, Al, Ar, alpha, betaX, c, Cr, Cl,deltaL[z], deltaR[z],deltaL2[z], deltaR2[z]);
;			}
	
	M_rv9_deblock_normal
	
    vdup.32		d4, d3[1]
    vdup.32		d5, d2[1]
    sub			r12, r0, #2
    vst4.8		{d2[0],d3[0],d4[0],d5[0]}, [r12], r1
    vst4.8		{d2[1],d3[1],d4[1],d5[1]}, [r12], r1
    vst4.8		{d2[2],d3[2],d4[2],d5[2]}, [r12], r1
    vst4.8		{d2[3],d3[3],d4[3],d5[3]}, [r12]
 
	ldmia		sp!, {r4 - r11, pc}		
	
;		}
;	} //if((Al+Ar) > 0)	

	ENDP	;ARMV7_rv9_v_loop_filter_Y


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	
ARMV7_rv9_v_loop_filter_UV	PROC
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	

    stmdb		sp!, {r4 - r11, r14}

; d1 = R3,L3    
; d2 = R2,L2    
; d3 = R1,L1    
; d7 = const_0x0000,0000,0020,0019; 26/25    
; q14 = const_0x00ff00ff...
; d30 = const_0    
; d31 = validFlag    
;r3 = Cr	
;r4 = Cl	
;r5 = alpha
;r6 = betaX
;r7 = beta2
;r14 = bStrong / c
     
    sub			r12, r0, #4
    vld1.32		d0, [r12], r1
    vld1.32		d1, [r12], r1
    vld1.32		d2, [r12], r1
    vld1.32		d3, [r12]
    
    ldr			r6, [sp, #OFFSET_betaX_h]
    ldr			r4, [sp, #OFFSET_Cl_h]
    ldr			r7, [sp, #OFFSET_beta2_h]
    ldr			r14, [sp, #OFFSET_bStrong_h]
    vmov.s32	d30, #0
    
	vtrn.8		d0, d1
	vtrn.8		d2, d3
	vtrn.16		d0, d2
	vtrn.16		d1, d3
	vrev64.32	d0, d0
	vrev64.32	d1, d1
	vtrn.32		d3, d0
	vtrn.32		d2, d1
	vrev64.32	d0, d0
	vrev64.32	d1, d1
    
;deltaL  = L2 - L1    
;deltaL2 = L2 - L3
;deltaR  = R2 - R1
;deltaR2 = R2 - R3

	vsubl.u8	q4, d2, d3			;deltaR,deltaL
	vsubl.u8	q5, d2, d1			;deltaR2,deltaL2

;	Al = Ar = 1;
;	delta = deltaL[0]+deltaL[1]+deltaL[2]+deltaL[3];
;	if (abs(delta) >= (betaX << 2))	
;		Al = 0;	
;	delta = deltaR[0]+deltaR[1]+deltaR[2]+deltaR[3];
;	if (abs(delta) >= (betaX << 2)) 	
;		Ar = 0;	
	
	vpaddl.s16	q6, q4
	vpaddl.s16	q7, q5
	vpaddl.s32	q6, q6
	vpaddl.s32	q7, q7
	vmovn.i32	d16, q6
	lsl			r12, r6, #2
	vmovn.i32	d17, q7
	vdup.i32	d18, r12
	vdup.i32	d19, r7			;q9 = beta2, betaX
	vabs.s32	q8, q8			;abs(deltaR,deltaL)
	vcgt.s32	q6, q9, q8
	vmov.s32	q10, #0x01
	vand		q6, q10
	
	vmov.i32	r5, r9, d12		;r9=Ar, r5=Al
	vmov.i32	r10, r11, d13	;r11=b3SmoothRight2, r10=b3SmoothLeft2
	
;	if((Al+Ar) > 0)
;	{

	adds		r8, r5, r9	
	ldmleia		sp!, {r4 - r11, pc}		;if((Al+Ar) <= 0) return;
	
;		c = Al + Ar + ((U8)(Cr + Cl)>>1) + 1;
;		if (bStrong){
;			b3SmoothLeft = (Al == 1);
;			b3SmoothRight = (Ar == 1);
;		}else{
;			/* not strong, force select weak below */
;			b3SmoothLeft = FALSE;
;			b3SmoothRight = FALSE;
;		}
;		if (b3SmoothLeft){
;			delta = deltaL2[0]+deltaL2[1]+deltaL2[2]+deltaL2[3];
;			if (abs(delta) >= beta2) 
;				b3SmoothLeft = FALSE;
;		}
;		if (b3SmoothRight){
;			delta = deltaR2[0]+deltaR2[1]+deltaR2[2]+deltaR2[3];
;			if (abs(delta) >= beta2) 
;				b3SmoothRight = FALSE;
;		}

	cmp			r14, #0
	movne		r7, r5		;b3SmoothLeft
	movne		r12, r9		;b3SmoothRight
	moveq		r7, #0 
	moveq		r12, #0
	cmp			r7, #0
	andne		r7, r10
	cmp			r12, #0
	add			r10, r3, r4
	add			r8, #1
	andne		r12, r11
	add			r8, r8, r10, lsr #1		;c

;		if(b3SmoothLeft && b3SmoothRight) {

	ands		r7, r12
	beq			v_loop_filter_UV_normal
	
v_loop_filter_UV_Strong

    ldr			r5, [sp, #OFFSET_alpha_h]
	ldr			r7, =Table_ditherLR
    vrev64.32	d4, d3				; d3 = R1,L1 ; d4 = L1,R1
    vsubl.u8	q3, d4, d3
    vcgt.u16	d31, d6, d30			

	M_rv9_deblock_strong_UV
	
    vdup.32		d4, d3[1]
    vdup.32		d5, d2[1]
    sub			r12, r0, #2
    vst4.8		{d2[0],d3[0],d4[0],d5[0]}, [r12], r1
    vst4.8		{d2[1],d3[1],d4[1],d5[1]}, [r12], r1
    vst4.8		{d2[2],d3[2],d4[2],d5[2]}, [r12], r1
    vst4.8		{d2[3],d3[3],d4[3],d5[3]}, [r12]
    
	ldmia		sp!, {r4 - r11, pc}		

;			} 
;		} 
;		else {

v_loop_filter_UV_normal

;			if(!Al || !Ar){
;				c >>= 1;
;				Cl>>= 1;
;				Cr>>= 1;
;			}
;			for(z = 0; z < 4; z++, r++){
;				rv40_weak_loop_filter(r, uPitch, Al, Ar, alpha, betaX, c, Cr, Cl,deltaL[z], deltaR[z],deltaL2[z], deltaR2[z]);
;			}
	
	M_rv9_deblock_normal
	
    vdup.32		d4, d3[1]
    vdup.32		d5, d2[1]
    sub			r12, r0, #2
    vst4.8		{d2[0],d3[0],d4[0],d5[0]}, [r12], r1
    vst4.8		{d2[1],d3[1],d4[1],d5[1]}, [r12], r1
    vst4.8		{d2[2],d3[2],d4[2],d5[2]}, [r12], r1
    vst4.8		{d2[3],d3[3],d4[3],d5[3]}, [r12]

	ldmia		sp!, {r4 - r11, pc}		
	
;		}
;	} //if((Al+Ar) > 0)	

	ENDP	;ARMV7_rv9_v_loop_filter_UV

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;static void rv9_h_loop_filter_2block(
;									 U8 *pPels,			/* first of 4 */
;									 U32 uPitch,
;									 U32 Cr[],				/* clip for right */
;									 U32 Cl[],				/* clip for left */
;									 I32 alpha,
;									 I32 betaX,
;									 I32 beta2,
;									 U32 yuv_h_deblock
;									 )
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ARMV7_rv9_h_loop_filter_2block	PROC

OFFSET_SaveRegisters	EQU		36	
OFFSET_alpha			EQU		OFFSET_SaveRegisters + 0
OFFSET_betaX			EQU		OFFSET_SaveRegisters + 4
OFFSET_beta2			EQU		OFFSET_SaveRegisters + 8
OFFSET_yuv_h_deblock	EQU		OFFSET_SaveRegisters + 12

	
CONST_0x200_Q			QN		q13

    stmdb		sp!, {r4 - r11, r14}
	
	sub			r12, r0, r1, lsl #1
	sub			r12, r12, r1
	
	ldr			r4, [sp, #OFFSET_yuv_h_deblock]
	vld1.64		d0, [r12], r1						;-3*uPitch
	vld1.64		d1, [r12], r1						;-2*uPitch
	vld1.64		d2, [r12], r1						;-1*uPitch
	vld1.64		d3, [r12], r1						; 0*uPitch
	vld1.64		d4, [r12], r1						; 1*uPitch
	vld1.64		d5, [r12]							; 2*uPitch
	ldr			r5, [sp, #OFFSET_betaX]
		
	tst			r4, #1
	
	vsubl.u8	q3, d1, d2			;deltaL
	vsubl.u8	q4, d4, d3			;deltaR
	vpaddl.s16	q5, q3
	vpaddl.s16	q6, q4
	vpaddl.s32	q5, q5				;sum_p1p0
	vpaddl.s32	q6, q6				;sum_q1q0
	vtrn.32		q6, q5
	vabs.s32	q6, q6				;|sum_p1p0[1]|sum_q1q0[1]|sum_p1p0[0]|sum_q1q0[0]|
	
	mov			r11, r5, lsl #2		;betaX << 2
	vdup.32		q5, r11
	vmov.i32	q7, #1
	vcgt.u32	q6, q5, q6			
	vand		q6, q7
	vmov		r9, r8, d12			;r8 =filter_p1[0],r9 =filter_q1[0]
	vmov		r11, r10, d13		;r10=filter_p1[1],r11=filter_q1[1]
	
	moveq		r8, #0				;filter_p1[0]
	moveq		r9, #0				;filter_q1[0]	
	tst			r4, #2
	moveq		r10, #0				;filter_p1[1]
	moveq		r11, #0				;filter_q1[1]
	
;	if(!filter_p1[0] && !filter_q1[0] && !filter_p1[1] && !filter_q1[1])
;		return;
	orr			r7, r8, r9
	orr			r7, r7, r10
	orrs		r7, r7, r11
	beq			h_2block_filter_end

;	lims[0] = filter_p1[0] + filter_q1[0] + ((Cr[0] + Cl[0]) >> 1) + 1;
;	lims[1] = filter_p1[1] + filter_q1[1] + ((Cr[1] + Cl[1]) >> 1) + 1;
	ldr			r4, [r2]		;Cr[0]
	ldr			r7, [r3]		;Cl[0]
	ldr			r6, [r2, #4]	;Cr[1]
	ldr			r14, [r3, #4]	;Cl[1]
	vdup.16		d14, r8
	vdup.16		d15, r10
	vdup.16		d16, r9
	vdup.16		d17, r11
	ands		r12, r8, r9		;if(!filter_p1[0] || !filter_q1[0])
	
	add			r8, r8, r9
	add			r12, r4, r7
	add			r8, #1
	add			r9, r10, r11
	add			r8, r8, r12, asr #1	;lims[0]
	add			r9, #1
	add			r12, r6, r14
	
;	if(!filter_p1[0] || !filter_q1[0]){
;		lims[0]    >>= 1;
;		Cr_temp[0]   = Cr[0] >> 1; 
;		Cl_temp[0]   = Cl[0] >> 1;
;	}

	asreq		r8, #1
	add			r9, r9, r12, asr #1	;lims[1]
	asreq		r4, #1				;Cr_temp[0]
	asreq		r7, #1				;Cl_temp[0]

;	if(!filter_p1[1] || !filter_q1[1]){
;		lims[1]   >>= 1;
;		Cr_temp[1]  = Cr[1] >> 1; 
;		Cl_temp[1]  = Cl[1] >> 1;
;	}

	ands		r12, r10, r11		;if(!filter_p1[1] || !filter_q1[1])
	asreq		r9, #1
	asreq		r6, #1 				;Cr_temp[1]
	asreq		r14, #1				;Cl_temp[1]
	
	ldr			r12, [sp, #OFFSET_alpha]
	vsubl.u8	q5, d1, d0			;deltaL2
	vsubl.u8	q6, d4, d5			;deltaR2

;	ptr = pPels;
;	for(i = 0; i < 4; i++, ptr++)
;		rv40_weak_loop_filter(ptr, uPitch, filter_p1[0], filter_q1[0], alpha, betaX, lims[0], Cr_temp[0], Cl_temp[0],deltaL[0][i], deltaR[0][i],
;		deltaL2[0][i], deltaR2[0][i]);
;	ptr = pPels + 4;
;	for(i = 0; i < 4; i++, ptr++)
;		rv40_weak_loop_filter(ptr, uPitch, filter_p1[1], filter_q1[1], alpha, betaX, lims[1], Cr_temp[1], Cl_temp[1],deltaL[1][i], deltaR[1][i], 
;		deltaL2[1][i], deltaR2[1][i]);

;q3  =  deltaL / diff_p1p0
;q4  =  deltaR / diff_q1q0
;q5  =  deltaL2 / diff_p1p2
;q6  =  deltaR2 / diff_q1q2
;q7  =  |filter_p1[1]|filter_p1[0]|
;q8  =  |filter_q1[1]|filter_q1[0]|
;q12 =  diff
;q15 = flag of valid data
;q9, q10, q11, q13, q14 = tmp

;r12      = alpha
;r5       = betaX
;r14, r7  =  Cl / lim_p1
;r6, r4   =  Cr / lim_q1
;r9, r8   =  |lims[1]|lims[0]| / lim_p0q0
;r2, r3, r10, r11, r12 =  tmp

;	t = src[0*step] - src[-1*step];
;	if(!t)
;		return;
;	u = (alpha * FFABS(t)) >> 7;
;	if(u > 3 - (filter_p1 && filter_q1))
;		return;

	vsubl.u8	q13, d3, d2		;t
	vmov.i32	q14, #0
	vdup.16		q12, r12		;alpha
	
	vabs.s16	q11, q13
	vcgt.u16	q15, q13, q14
	vmul.s16	q11, q12
	vand.16		q9, q7, q8		;(filter_p1 && filter_q1)
	vmov.i16	q10, #3
	vshl.s16	q13, #2
	vsub.s16	q12, q10, q9
	vshr.s16	q11, #7			;u
	vcgt.u16	q9, q9, q14
	vcge.s16	q11, q12, q11

;	t <<= 2;
;	if(filter_p1 && filter_q1)
;		t += src[-2*step] - src[1*step];
;	diff = CLIP_SYMM((t + 4) >> 3, lim_p0q0);

	vsubl.u8	q10, d1, d4		;src[-2*step] - src[1*step]
	vand		q15, q11
	vadd.s16	q10, q10, q13
	vmov.i16	q12, #4
	vbit		q13, q10, q9
	vdup.16		d22, r8
	vadd.s16	q13, q13, q12
	vdup.16		d23, r9
	vshr.s16	q13, #3 
	
	vsub.s16	q14, q14, q11	;-lim_p0q0
	vmax.s16	q12, q14, q13
	vmovl.u8	q13, d2
	vmin.s16	q12, q12, q11	;diff = CLIP_SYMM((t + 4) >> 3, lim_p0q0);
	
;	if(filter_p1 || filter_q1){
;		src[-1*step] = ClampTbl[CLAMP_BIAS + src[-1*step] + diff];
;		src[ 0*step] = ClampTbl[CLAMP_BIAS + src[ 0*step] - diff];
;	}

	vmovl.u8	q14, d3
	vadd.s16	q13, q13, q12		;src[-1*step] + diff
	vsub.s16	q14, q14, q12		;src[ 0*step] - diff
	vqshrun.s16	d22, q13, #0
	vqshrun.s16	d23, q14, #0
	
	vorr		q13, q7, q8			;(filter_p1 || filter_q1)
	vmov.i32	q10, #0
	vcgt.u16	q14, q13, q10
	vdup.16		d18, r7
	vand		q14, q15			;(filter_p1 || filter_q1) & validFlag
	vmovn.i16	d26, q14
	vdup.16		d19, r14			; q9 = lim_p1
	vbit		d2, d22, d26
	vbit		d3, d23, d26
	
;	if(FFABS(diff_p1p2) <= beta && filter_p1){
;		t = (diff_p1p0 + diff_p1p2 - diff) >> 1;
;		src[-2*step] = ClampTbl[CLAMP_BIAS + src[-2*step] - CLIP_SYMM(t, lim_p1)];
;	}

	vadd.s16	q13, q3, q5
	vsub.s16	q13, q13, q12
	vshr.s16	q13, #1
	vsub.s16	q3, q10, q9		; -lim_p1
	vmax.s16	q3, q3, q13
	vmovl.u8	q14, d1
	vmin.s16	q3, q9, q3		;CLIP_SYMM(t, lim_p1)
	vdup.16		q13, r5
	vsub.s16	q14, q14, q3	
	vabs.s16	q5, q5
	vqshrun.s16	d18, q14, #0
	
	vcge.u16	q11, q13, q5
	vcgt.u16	q14, q7, q10
	vand		q11, q14
	vand		q11, q15
	vmovn.i16	d22, q11
	vbit		d1, d18, d22


;	if(FFABS(diff_q1q2) <= beta && filter_q1){
;		t = (diff_q1q0 + diff_q1q2 + diff) >> 1;
;		src[ 1*step] = ClampTbl[CLAMP_BIAS + src[ 1*step] - CLIP_SYMM(t, lim_q1)];
;	}

	vdup.16		d18, r4
	vdup.16		d19, r6			; q9 = lim_q1
	vadd.s16	q13, q4, q6
	vadd.s16	q13, q13, q12
	vsub.s16	q4, q10, q9		; -lim_q1
	vshr.s16	q13, #1
	vmax.s16	q4, q4, q13
	vmovl.u8	q14, d4
	vmin.s16	q4, q9, q4		;CLIP_SYMM(t, lim_q1)
	vdup.16		q13, r5 
	vsub.s16	q14, q14, q4	
	vabs.s16	q6, q6
	vqshrun.s16	d18, q14, #0
	
	vcge.u16	q11, q13, q6
	vcgt.u16	q14, q8, q10
	vand		q11, q14
	sub			r12, r0, r1, lsl #1
	vand		q11, q15
	vst1.64		d1, [r12], r1
	vmovn.i16	d22, q11
	vst1.64		d2, [r12], r1
	vbit		d4, d18, d22
	
	vst1.64		d3, [r12], r1
	vst1.64		d4, [r12]
		
h_2block_filter_end
	ldmia		sp!, {r4 - r11, pc}		
	ENDP	;ARMV7_rv9_h_loop_filter_2block
		
	
	END

