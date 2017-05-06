
    INCLUDE wmvdec_member_arm.inc
    INCLUDE xplatform_arm_asm.h 
    IF UNDER_CE != 0
    INCLUDE kxarm.h
    ENDIF

    IF WMV_OPT_MOTIONCOMP_ARM=1

    AREA |.text|, CODE, READONLY
       
    EXPORT  ARMV7_g_NewVertFilterX
    EXPORT  ARMV7_g_NewHorzFilterX
	EXPORT  ARMV7_g_NewVertFilter0LongNoGlblTbl
    EXPORT  ARMV7_g_InterpolateBlock_00_SSIMD
    EXPORT  ARMV7_g_AddNull_SSIMD
    EXPORT  ARMV7_g_InterpolateBlockBilinear_SSIMD 
    EXPORT  ARMV7_g_InterpolateBlockBilinear_SSIMD_11
    EXPORT  ARMV7_g_InterpolateBlockBilinear_SSIMD_01
    EXPORT  ARMV7_g_InterpolateBlockBilinear_SSIMD_10

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;Void_WMV  g_InterpolateBlockBilinear_SSIMD (const U8_WMV *pSrc, 
;                                           I32_WMV iSrcStride, 
;                                           U8_WMV *pDst, 
;                                           I32_WMV iXFrac, 
;                                           I32_WMV iYFrac, 
;                                           I32_WMV iRndCtrl, 
;                                           Bool_WMV b1MV)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;{  
;    I32_WMV i, j, k0,k1;
;    I32_WMV iNumLoops = 8<<b1MV;
;    U8_WMV *pD ; 
;    const U8_WMV  *pT ;
;    iRndCtrl = 8 - ( iRndCtrl&0xff); 
;    pT = pSrc;	
;    pD = pDst;
;
;	if( 0 == b1MV )	{
;	I16_WMV a,b,c,d;
;	    a = (pT[0]<<2)+  ( pT[iSrcStride] - pT[0])* iYFrac;
;	    for (i = 0; i < 8; i++) 
;	    {
;	//0
;	        b = (pT[1]<<2)+  ( pT[iSrcStride+1] - pT[1])* iYFrac;
;	        c = (pT[2]<<2)+  ( pT[iSrcStride+2] - pT[2])* iYFrac;
;	        k0 = ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4;
;	        k1 = ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4;
;	        *(I16_WMV *)(pD+0) = (U8_WMV) k0;
;	        *(I16_WMV *)(pD+20) = (U8_WMV) k1;
;	
;	        d = (pT[3]<<2)+  ( pT[iSrcStride+3] - pT[3])* iYFrac;
;	        a = (pT[4]<<2)+  ( pT[iSrcStride+4] - pT[4])* iYFrac;
;	        k0 = ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4;
;	        k1 = ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4;
;	        *(I16_WMV *)(pD+2) = (U8_WMV) k0;
;	        *(I16_WMV *)(pD+22) = (U8_WMV) k1;
;	//1
;	        b = (pT[5]<<2)+  ( pT[iSrcStride+5] - pT[5])* iYFrac;
;	        c = (pT[6]<<2)+  ( pT[iSrcStride+6] - pT[6])* iYFrac;
;	        k0 = ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4;
;	        k1 = ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4;
;	        *(I16_WMV *)(pD+4) = (U8_WMV) k0;
;	        *(I16_WMV *)(pD+24) = (U8_WMV) k1;
;	
;	        d = (pT[7]<<2)+  ( pT[iSrcStride+7] - pT[7])* iYFrac;
;	        a = (pT[8]<<2)+  ( pT[iSrcStride+8] - pT[8])* iYFrac;
;	        k0 = ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4;
;	        k1 = ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4;
;	        *(I16_WMV *)(pD+6) = (U8_WMV) k0;
;	        *(I16_WMV *)(pD+26) = (U8_WMV) k1;
;
;	        pT += iSrcStride;
;	        pD += 40;
;	    } 
;	else {
;		I16_WMV a,b,c,d;
;        a = (pT[0]<<2)+  ( pT[iSrcStride] - pT[0])* iYFrac;
;	    for (i = 0; i < 16; i++) 
;	    {
;//0
;        b = (pT[1]<<2)+  ( pT[iSrcStride+1] - pT[1])* iYFrac;
;        c = (pT[2]<<2)+  ( pT[iSrcStride+2] - pT[2])* iYFrac;
;        k0 = ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4;
;        k1 = ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4;
;        *(I16_WMV *)(pD+0) = (U8_WMV) k0;
;        *(I16_WMV *)(pD+20) = (U8_WMV) k1;
;
;        d = (pT[3]<<2)+  ( pT[iSrcStride+3] - pT[3])* iYFrac;
;        a = (pT[4]<<2)+  ( pT[iSrcStride+4] - pT[4])* iYFrac;
;        k0 = ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4;
;        k1 = ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4;
;        *(I16_WMV *)(pD+2) = (U8_WMV) k0;
;        *(I16_WMV *)(pD+22) = (U8_WMV) k1;
;//1
;        b = (pT[5]<<2)+  ( pT[iSrcStride+5] - pT[5])* iYFrac;
;        c = (pT[6]<<2)+  ( pT[iSrcStride+6] - pT[6])* iYFrac;
;        k0 = ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4;
;        k1 = ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4;
;        *(I16_WMV *)(pD+4) = (U8_WMV) k0;
;        *(I16_WMV *)(pD+24) = (U8_WMV) k1;
;
;        d = (pT[7]<<2)+  ( pT[iSrcStride+7] - pT[7])* iYFrac;
;        a = (pT[8]<<2)+  ( pT[iSrcStride+8] - pT[8])* iYFrac;
;        k0 = ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4;
;        k1 = ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4;
;        *(I16_WMV *)(pD+6) = (U8_WMV) k0;
;        *(I16_WMV *)(pD+26) = (U8_WMV) k1;
;	//2
;	        b = (pT[9]<<2)+  ( pT[iSrcStride+9] - pT[9])* iYFrac;
;	        c = (pT[10]<<2)+  ( pT[iSrcStride+10] - pT[10])* iYFrac;
;	        k0 = ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4;
;	        k1 = ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4;
;	        *(I16_WMV *)(pD+8) = (U8_WMV) k0;
;	        *(I16_WMV *)(pD+28) = (U8_WMV) k1;
;	
;	        d = (pT[11]<<2)+  ( pT[iSrcStride+11] - pT[11])* iYFrac;
;	        a = (pT[12]<<2)+  ( pT[iSrcStride+12] - pT[12])* iYFrac;
;	        k0 = ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4;
;	        k1 = ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4;
;	        *(I16_WMV *)(pD+10) = (U8_WMV) k0;
;	        *(I16_WMV *)(pD+30) = (U8_WMV) k1;
;	//3
;	        b = (pT[13]<<2)+  ( pT[iSrcStride+13] - pT[13])* iYFrac;
;	        c = (pT[14]<<2)+  ( pT[iSrcStride+14] - pT[14])* iYFrac;
;	        k0 = ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4;
;	        k1 = ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4;
;	        *(I16_WMV *)(pD+12) = (U8_WMV) k0;
;	        *(I16_WMV *)(pD+32) = (U8_WMV) k1;
;	
;	        d = (pT[15]<<2)+  ( pT[iSrcStride+15] - pT[15])* iYFrac;
;	        a = (pT[16]<<2)+  ( pT[iSrcStride+16] - pT[16])* iYFrac;
;	        k0 = ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4;
;	        k1 = ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4;
;	        *(I16_WMV *)(pD+14) = (U8_WMV) k0;
;	        *(I16_WMV *)(pD+34) = (U8_WMV) k1;
;
;        pT += iSrcStride;
;        pD += 40;
;    } 
;}
;}

    AREA  |.text|, CODE
    WMV_LEAF_ENTRY ARMV7_g_InterpolateBlockBilinear_SSIMD

;r0 = pSrc
;r1 = iSrcStride
;r2 = pDst
;r3 = iXFrac
;r4 = iYFrac
;r5 = iRndCtl, addr of column 8 or 16
;r6 = b1MV
;r7 = pDst + 20
;r12 = iNumLoops;
;r14 = 40;
;q0 = iYFrac, iXFrac (d0 = iXFrac,d1 = iYFrac)
;q1 = iRndCtl
	
    stmdb     sp!, {r4-r7, r14}
    FRAME_PROFILE_COUNT
    
  ;  iRndCtrl = 8 - ( iRndCtrl&0xff);

    pld			[r0]
    pld			[r0, r1]
	ldr			r4, [sp, #20+0]
	ldr			r5, [sp, #20+4]
	ldr			r6, [sp, #20+8]
	add			r7, r2, #20
	mov			r14, #40
	and			r5, r5, #0xff
	rsb			r5, r5, #8		;iRndCtrl
	
	vdup.16		q1, r5			;iRndCtrl	
	vdup.16		d0, r3
	vdup.8		d1, r4
	
	cmp			r6, #0			;b1MV
	bne			IBB_16_loop_Start    

	add			r5, r0, #8		;point column 8
	vld1.u8		d4, [r0], r1
	vld1.u8		d5[0], [r5], r1	;|x8|
	mov			r12, #8

IBB_8_loop

;r5 : addr of column 8

;	q2 = |x|x|x|x|x|x|x|x8|x7|x6|x5|x4|x3|x2|x1|x0|	;current line
;	q3 = |x|x|x|x|x|x|x|y8|y7|y6|y5|y4|y3|y2|y1|y0|	;next line
;	q4 ...
;	d5.u16[0]: |x8|
;	d7.u16[0]: |y8|
;	d9.u16[0] ...

	vld1.u8		d6, [r0], r1
	vld1.u8		d8, [r0], r1
	vld1.u8		d7[0], [r5], r1	; |y8|
	vld1.u8		d9[0], [r5], r1
	
    pld			[r0]
    pld			[r0, r1]

;        a = (pT[0]<<2) + ( pT[iSrcStride]   - pT[0])* iYFrac;
;        b = (pT[1]<<2) + ( pT[iSrcStride+1] - pT[1])* iYFrac;
;        c = (pT[2]<<2) + ( pT[iSrcStride+2] - pT[2])* iYFrac;
;		...
;        a = (pT[8]<<2) + ( pT[iSrcStride+8] - pT[8])* iYFrac;
	
	;vsub.s8		q10, q3, q2		;current line;	( pT[iSrcStride] - pT[0]) 
	;vsub.s8		q11, q4, q3		;next line
	;vmull.s8	q12, d20, d1	;( pT[iSrcStride] - pT[0]) * iYFrac
	;vmull.s8	q13, d21, d1
	;vmull.s8	q14, d22, d1
	;vmull.s8	q15, d23, d1
	vdup.16		q0, r4  ; zou
	
	vsubl.u8            q10,d6,d4  ;zou
	vsubl.u8            q12,d7,d5  ;zou		
	vsubl.u8            q11,d8,d6  ;zou     ;next line
	vsubl.u8            q13,d9,d7  ;zou
	vmul.s16	q14, q11, q0
	vmul.s16	q15, q13, q0	
	vmul.s16	q13, q12, q0
	vmul.s16	q12, q10, q0
	
	vdup.16		d0, r3
		
	vshll.u8	q10, d4, #2		;(pT[0]<<2)
	vshll.u8	q6, d5, #2	
	vshll.u8	q11, d6, #2	
	vshll.u8	q7, d7, #2	
	
	;backup for next loop				
	vshr.s32 	q2, q4, #0   ;vmov   q2, q4
	
		; + (pT[0]<<2)	;current line
	vadd.s16	q12, q12, q10		;|z7|z6|z5|z4|z3|z2|z1|z0|
	vadd.s16	q13, q13, q6		;|x|x|x|x|x|x|x|z8|
		; + (pT[0]<<2)	;next line
	vadd.s16	q14, q14, q11		;|z7|z6|z5|z4|z3|z2|z1|z0|
	vadd.s16	q15, q15, q7		;|x|x|x|x|x|x|x|z8|

;        k0 = ((a <<2) + (b - a) * iXFrac + iRndCtrl) >> 4;
;        k1 = ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4;
;		...
	
	vext.16		q10, q12, q13, #1	;q10 = |z8|z7|z6|z5|z4|z3|z2|z1|
	vext.16		q11, q14, q15, #1	;q11 = |z8|z7|z6|z5|z4|z3|z2|z1|

	vsub.s16	q10, q10, q12		;(b - a)
	vsub.s16	q11, q11, q14			
	vmul.s16	q13, q10, d0[0]	;(b - a) * iXFrac
	vmul.s16	q15, q11, d0[0]	
		
	vshl.u16	q12, q12, #2	
	vshl.u16	q14, q14, #2
	
	vadd.s16	q10, q13, q12			; + (a <<2)
	vadd.s16	q11, q15, q14			
	vadd.s16	q10, q10, q1			; + iRndCtrl
	vadd.s16	q11, q11, q1	
	
	vshr.u16	q10, q10, #4			; >> 4 ;|m7|m6|m5|m4|m3|m2|m1|m0|
	vshr.u16	q11, q11, #4			; >> 4 ;|m7|m6|m5|m4|m3|m2|m1|m0|

;	*(I16_WMV *)(pDst + j) = (U8_WMV) k0;
;	*(I16_WMV *)(pDst + j + 20) = (U8_WMV) k1;

	; q8,q9 = |m7|m6|m5|m4|m3|m2|m1|m0| => {d6,d8 = |m6|m4|m2|m0|, d7,d9 = |m7|m5|m3|m1|}
	
	vmovn.i32	d10, q10		;d6 = |m6|m4|m2|m0|
	vmovn.i32	d12, q11 
	vshr.u32	q10, #16		;q8 = |0|m7|0|m5|0|m3|0|m1|
	vshr.u32	q11, #16
	vmovn.i32	d11, q10		;d7 = |m7|m5|m3|m1|
	vmovn.i32	d13, q11 
	
	vst1.u32	d10, [r2], r14		;k0
	vst1.u32	d12, [r2], r14
	vst1.u32	d11, [r7], r14		;k1
	vst1.u32	d13, [r7], r14

	subs		r12, r12, #2
	bne			IBB_8_loop

    ldmia	sp!, {r4-r7, pc}
	
IBB_16_loop_Start  

;r5 : addr of column 16
;	q2 = |x15|x14|x13|x12|x11|x10|x9|x8|x7|x6|x5|x4|x3|x2|x1|x0|
;	     |				d1			   |			d0		   |
;	q3 = |y15|y14|y13|y12|y11|y10|y9|y8|y7|y6|y5|y4|y3|y2|y1|y0|
;	     |				d3			   |			d2		   |
;	q4...
;	d10.u16[0]: |x8|
;	d11.u16[0]: |y8| 
;	d12.u16[0] ...

	add			r5, r0, #16			 
	vld1.u8		{q2}, [r0] , r1			;load first line 16 bytes data
	vld1.u8		d10[0], [r5], r1	;load the byte of column 16
	mov			r12, #16

IBB_16_loop

	vld1.u8		{q3}, [r0], r1
	vld1.u8		{q4}, [r0], r1
	vld1.u8		d11[0], [r5], r1	;  
	vld1.u8		d12[0], [r5], r1	;
	
    pld			[r0]
    pld			[r0, r1]
	
;        a = (pT[0]<<2) + ( pT[iSrcStride]   - pT[0])* iYFrac;
;        b = (pT[1]<<2) + ( pT[iSrcStride+1] - pT[1])* iYFrac;
;        c = (pT[2]<<2) + ( pT[iSrcStride+2] - pT[2])* iYFrac;
;		...
;        a = (pT[16]<<2) + ( pT[iSrcStride+16] - pT[16])* iYFrac;

	;vsub.s8		q10, q3, q2		;( pT[iSrcStride] - pT[0])
	;vsub.s8		q11, q4, q3
	;vsub.s8		d13, d11, d10
	;vsub.s8		d14, d12, d11
	vdup.16		q0, r4 	
	vsubl.u8        q10, d6,d4
	vsubl.u8        q12, d7,d5	
	vsubl.u8        q11, d8,d6
	vsubl.u8        q13, d9,d7	
	vsubl.u8        q8, d11,d10
	vsubl.u8        q9, d12,d11
	
	;vmull.s8	q12, d20, d1	;( pT[iSrcStride] - pT[0]) * iYFrac ;current line
	;vmull.s8	q13, d21, d1
	;vmull.s8	q8, d13, d1
	;vmull.s8	q14, d22, d1	;( pT[iSrcStride] - pT[0]) * iYFrac ;next line
	;vmull.s8	q15, d23, d1
	;vmull.s8	q9, d14, d1
	vmul.s16        q14, q11, q0    
	vmul.s16        q15, q13, q0 
	vmul.s16	    q9,  q9,  q0
	
	vmul.s16        q13, q12, q0    
	vmul.s16        q12, q10, q0 
	vmul.s16	    q8,  q8,  q0	
	
	vdup.16		d0, r3
		
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

;        k0 = ((a <<2) + (b - a) * iXFrac + iRndCtrl) >> 4;
;        k1 = ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4;
;		...

	vext.16		q8, q12, q13, #1	;q6 = |z8|z7|z6|z5|z4|z3|z2|z1|
	vext.16		q9, q13, q10, #1	;q7 = |z16|z15|z14|z13|z12|z11|z10|z9|
	vext.16		q10, q14, q15, #1	;q8 = |z8|z7|z6|z5|z4|z3|z2|z1|
	vext.16		q11, q15, q11, #1	;q9 = |z16|z15|z14|z13|z12|z11|z10|z9|

	vsub.s16	q8 , q8, q12		;(b - a)
	vsub.s16	q9 , q9, q13
	vsub.s16	q10, q10, q14			
	vsub.s16	q11, q11, q15
	vmul.s16	q8,  q8,  d0[0]	;(b - a) * iXFrac
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
	
	vshr.u16	q8,  q8,  #4	;|m7|m6|m5|m4|m3|m2|m1|m0|
	vshr.u16	q9,  q9,  #4	;|m15|m14|m13|m12|m11|m10|m9|m8|	
	vshr.u16	q10, q10, #4	
	vshr.u16	q11, q11, #4	


;	*(I16_WMV *)(pDst + j) = (U8_WMV) k0;
;	*(I16_WMV *)(pDst + j + 20) = (U8_WMV) k1;

;   {q8/q10; q9/q11} => {q12,q14 = |m14|m12|m10|m8|m6|m4|m2|m0|; q13,q12 = |m15|m13|m11|m9|m7|m5|m3|m1| }
	
	vmovn.i32	d24, q8 		;d24 = |m6|m4|m2|m0|
	vmovn.i32	d25, q9 		;d25 = |m14|m12|m10|m8| 
	vmovn.i32	d26, q10
	vmovn.i32	d27, q11
	vshr.u32	q8,  #16		;q8 = |0|m7|0|m5|0|m3|0|m1|
	vshr.u32	q9,  #16		;q9 = |0|m15|0|m13|0|m11|0|m9|
	vshr.u32	q10, #16
	vshr.u32	q11, #16
	vmovn.i32	d28, q8 		;d28 = |m7|m5|m3|m1|
	vmovn.i32	d29, q9 		;d29 = |m15|m13|m11|m9| 
	vmovn.i32	d30, q10
	vmovn.i32	d31, q11
	
	vst1.u32	{q12}, [r2], r14	;k0
	vst1.u32	{q13}, [r2], r14	
	vst1.u32	{q14}, [r7], r14	;k1
	vst1.u32	{q15}, [r7], r14

	subs		r12, r12, #2
	bne			IBB_16_loop

	ldmia     sp!, {r4 - r7, pc}	
	
    WMV_ENTRY_END	;ARMV7_g_InterpolateBlockBilinear_SSIMD


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;Void_WMV  g_InterpolateBlockBilinear_SSIMD_11 (const U8_WMV *pSrc, 
;                                           I32_WMV iSrcStride, 
;                                           U8_WMV *pDst, 
;                                           I32_WMV iXFrac, 
;                                           I32_WMV iYFrac, 
;                                           I32_WMV iRndCtrl, 
;                                           Bool_WMV b1MV)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    AREA  |.text|, CODE
    WMV_LEAF_ENTRY ARMV7_g_InterpolateBlockBilinear_SSIMD_11
	
;r0=pSrc
;r1=iSrcStride
;r2=pDst
;r3=iRndCtl
;r6 = pDst + 20
;r12 = loop num
;r14 = 40
;q7  = iRndCtl

    stmdb     sp!, {r4-r7, r14}
    FRAME_PROFILE_COUNT
    
  ;  iRndCtrl = 8 - ( iRndCtrl&0xff);

    pld			[r0]
    pld			[r0, r1]
	ldr			r3, [sp, #20+4]
	ldr			r4, [sp, #20+8]
	add			r6, r2, #20
	mov			r14, #40
	and			r3, r3, #0xff
	rsb			r3, r3, #8		 ;iRndCtrl
	vdup.16		q7, r3			;iRndCtrl	

	cmp			r4, #0			;b1MV
	bne			IBB_11_16_loop_Start    

	add			r5, r0, #8		;point column 8
	vld1.u8		d0, [r0], r1
	vld1.u8		d4[0], [r5], r1	;|x8|; d4[0] <==> q2.u16[0]
	mov			r12, #8
IBB_11_8_loop

;r5 : addr of column 8
;q0, q2 : src data for next loop. 

;	d0 = |x7|x6|x5|x4|x3|x2|x1|x0|	;current line
;	d1 = |y7|y6|y5|y4|y3|y2|y1|y0|	;next line
;	d2 ...
;	q2.u16[0]: |x8|
;	q3.u16[0]: |y8|
;	q4.u16[0]

	vld1.u8		d1, [r0], r1
	vld1.u8		d2, [r0], r1
	vld1.u8		d6[0], [r5], r1	; d6[0] <==> q3.u16[0]; |x8|
	vld1.u8		d8[0], [r5], r1	; d8[0] <==> q4.u16[0]; |x8|
    pld			[r0]
    pld			[r0, r1]

;    PF0 = pT[0] + pT[iSrcStride+0];
;    PF1 = pT[1] + pT[iSrcStride+1];
;    PF2 = pT[2] + pT[iSrcStride+2];
;    PF3 = pT[3] + pT[iSrcStride+3];
	
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

;	k0 = (((PF0 + PF1) << 2) + iRndCtrl) >> 4;
;	k1 = (((PF1 + PF2) << 2) + iRndCtrl) >> 4;
;	...
	
	;q8,q9 = |m7|m6|m5|m4|m3|m2|m1|m0|
	vadd.u16	q8 , q5, q8
	vadd.u16	q9 , q6, q9 
	vshl.i16	q8 , #2			
	vshl.i16	q9 , #2
	vadd.i16	q8 , q7			
	vadd.i16	q9 , q7
	vshr.u16	q8 , #4			
	vshr.u16	q9 , #4

;	*(I16_WMV *)(pDst + j) = (U8_WMV) k0;
;	*(I16_WMV *)(pDst + j + 20) = (U8_WMV) k1;

	; q8,q9 = |m7|m6|m5|m4|m3|m2|m1|m0| => {d6,d8 = |m6|m4|m2|m0|, d7,d9 = |m7|m5|m3|m1|}
	
	vmovn.i32	d6, q8		;d6 = |m6|m4|m2|m0|
	vmovn.i32	d8, q9 
	vshr.u32	q8, #16		;q8 = |0|m7|0|m5|0|m3|0|m1|
	vshr.u32	q9, #16
	vmovn.i32	d7, q8		;d7 = |m7|m5|m3|m1|
	vmovn.i32	d9, q9 
	
	vst1.u32	d6, [r2], r14		;k0
	vst1.u32	d8, [r2], r14
	vst1.u32	d7, [r6], r14		;k1
	vst1.u32	d9, [r6], r14

	subs		r12, r12, #2
	bne			IBB_11_8_loop

    ldmia	sp!, {r4-r7, pc}
	
IBB_11_16_loop_Start  

;r5 : addr of column 16
;q0, q3 : src data for next loop. 

;	q0 = |x15|x14|x13|x12|x11|x10|x9|x8|x7|x6|x5|x4|x3|x2|x1|x0|
;	     |				d1			   |			d0		   |
;	q1 = |y15|y14|y13|y12|y11|y10|y9|y8|y7|y6|y5|y4|y3|y2|y1|y0|
;	     |				d3			   |			d2		   |
;	q2...

	add			r5, r0, #16			;point column 16
	vld1.u8		{q0}, [r0] , r1		;load first line 16 bytes data
	vld1.u8		d6[0], [r5], r1	;q3[0] <==> d6[0] 
	mov			r12, #16

IBB_11_16_loop

	vld1.u8		{q1}, [r0], r1
	vld1.u8		{q2}, [r0], r1
	vld1.u8		d8[0] , [r5], r1	;q4[0] <==> d8[0]  
	vld1.u8		d10[0], [r5], r1	;q5[0] <==> d10[0] 
    pld			[r0]
    pld			[r0, r1]
	
;    PF0 = pT[0] + pT[iSrcStride+0];
;    PF1 = pT[1] + pT[iSrcStride+1];
;    PF2 = pT[2] + pT[iSrcStride+2];
;    PF3 = pT[3] + pT[iSrcStride+3];

	; q8,q10 = |z7 |z6 |z5 |z4 |z3 |z2 |z1|z0|
	; q9,q11 = |z15|z14|z13|z12|z11|z10|z9|z8|
	; (z16 = x16 + y16, ..., z0 = x0 + y0)
	vaddl.u8	q8 , d0, d2		
	vaddl.u8	q9 , d1, d3		
	vaddl.u8	q10, d2, d4
	vaddl.u8	q11, d3, d5

	; for next loop	
	vshr.s32 	q0, q2, #0  ;vmov	q0, q2

	vaddl.u8	q1, d6, d8	
	vaddl.u8	q2, d8, d10
	
	; for next loop
	vdup.u16		d6, d10[0]	; <==>vmov q3, q5
	
	;q1,q5 = |z16|z15|z14|z13|z12|z11|z10|z9|
	;q2,q4 = |z8 |z7 |z6 |z5 |z4 |z3 |z2 |z1|
	vext.16		q12, q8 , q9 , #1	
	vext.16		q13, q9 , q1 , #1	
	vext.16		q14, q10, q11, #1	
	vext.16		q15, q11, q2 , #1	

;	k0 = (((PF0 + PF1) << 2) + iRndCtrl) >> 4;
;	k1 = (((PF1 + PF2) << 2) + iRndCtrl) >> 4;
;	...

	; q8,q10 = |m7 |m6 |m5 |m4 |m3 |m2 |m1|m0|
	; q9,q11 = |m15|m14|m13|m12|m11|m10|m9|m8|
	; m15 = z16+z15, ... , m0 = z1+z0
	
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
	vshr.u16	q8 , #4			
	vshr.u16	q9 , #4
	vshr.u16	q10, #4
	vshr.u16	q11, #4


;	*(I16_WMV *)(pDst + j) = (U8_WMV) k0;
;	*(I16_WMV *)(pDst + j + 20) = (U8_WMV) k1;

  ; {q8/q10; q9/q11} => {q4,q6 = |m14|m12|m10|m8|m6|m4|m2|m0|; q5,q7 = |m15|m13|m11|m9|m7|m5|m3|m1| }
	
	vmovn.i32	d8 , q8		;d8 = |m6|m4|m2|m0|
	vmovn.i32	d9 , q9		;d9 = |m14|m12|m10|m8| 
	vmovn.i32	d10, q10
	vmovn.i32	d11, q11
	vshr.u32	q8 , #16	;q8 = |0|m7|0|m5|0|m3|0|m1|
	vshr.u32	q9 , #16	;q9 = |0|m15|0|m13|0|m11|0|m9|
	vshr.u32	q10, #16
	vshr.u32	q11, #16
	vmovn.i32	d16, q8		;d12 = |m7|m5|m3|m1|
	vmovn.i32	d17, q9		;d13 = |m15|m13|m11|m9| 
	vmovn.i32	d18, q10
	vmovn.i32	d19, q11
	
	vst1.u32	{q4}, [r2], r14	;k0
	vst1.u32	{q5}, [r2], r14	
	vst1.u32	{q8}, [r6], r14	;k1
	vst1.u32	{q9}, [r6], r14

	subs		r12, r12, #2
	bne			IBB_11_16_loop

	ldmia     sp!, {r4 - r7, pc}
	
	WMV_ENTRY_END	;ARMV7_g_InterpolateBlockBilinear_SSIMD_11



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;Void_WMV  g_InterpolateBlockBilinear_SSIMD_10 (const U8_WMV *pSrc, 
;                                           I32_WMV iSrcStride, 
;                                           U8_WMV *pDst, 
;                                           I32_WMV iXFrac, 
;                                           I32_WMV iYFrac, 
;                                           I32_WMV iRndCtrl, 
;                                           Bool_WMV b1MV)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    AREA  |.text|, CODE
    WMV_LEAF_ENTRY ARMV7_g_InterpolateBlockBilinear_SSIMD_10

;r0=pSrc
;r1=iSrcStride
;r2=pDst
;r3=iRndCtl
;r6 = pDst + 20
;r12 = loop num
;r14 = 40
;q7  = iRndCtl

    stmdb     sp!, {r4-r7, r14}
    FRAME_PROFILE_COUNT
    
  ;  I32_WMV iNumLoops = 8<<b1MV;    
  ;  iRndCtrl = 8 - ( iRndCtrl&0xff);

    pld			[r0]
    pld			[r0, r1]
	ldr			r3, [sp, #20+4]
	ldr			r4, [sp, #20+8]
	mov			r14, #40
	add			r6, r2, #20			;pDst + 20
	and			r3, r3, #0xff
	rsb			r3, r3, #8		;iRndCtrl
	vdup.16		q7, r3			;iRndCtrl	
	
	cmp			r4, #0
	bne			IBB_10_16_loop_Start
    
	mov			r12, #8
IBB_10_8_loop

;r5 = pSrc + 8
;d0,d1 = pSrc[]...
;q8,q9 = adding result.

;	load src 8 bytes data of per line.
;	d0 = |x7|x6|x5|x4|x3|x2|x1|x0|	;current line
;	d1 = |y7|y6|y5|y4|y3|y2|y1|y0|	;next line

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
	vshr.u16	q8 , #4			
	vshr.u16	q9 , #4

;	*(I16_WMV *)(pDst + j) = (U8_WMV) k0;
;	*(I16_WMV *)(pDst + j + 20) = (U8_WMV) k1;

	; q8 = |m7|m6|m5|m4|m3|m2|m1|m0| => {d0 = |m6|m4|m2|m0|, d1 = |m7|m5|m3|m1|}
	; q9 = |m7|m6|m5|m4|m3|m2|m1|m0| => {d2 = |m6|m4|m2|m0|, d3 = |m7|m5|m3|m1|}	
	vmovn.i32	d0, q8		;d0 = |m6|m4|m2|m0|
	vmovn.i32	d2, q9 
	vshr.u32	q8, #16		;q8 = |0|m7|0|m5|0|m3|0|m1|
	vshr.u32	q9, #16
	vmovn.i32	d1, q8		;d1 = |m7|m5|m3|m1|
	vmovn.i32	d3, q9 
	
	vst1.u32	d0, [r2], r14		;k0
	vst1.u32	d2, [r2], r14
	vst1.u32	d1, [r6], r14		;k1
	vst1.u32	d3, [r6], r14

	subs		r12, r12, #2	; 2 lines per loop
	bne			IBB_10_8_loop

	ldmia     sp!, {r4-r7, pc}
	
IBB_10_16_loop_Start

;r4 = pSrc + 16
;q0~q3 = pSrc[]....
;q8~q15 = adding result.

	add			r4, r0, #16		;point column 16
	mov			r12, #16

IBB_10_16_loop

	;load src 16 bytes data per line
	;q0 = |x15|x14|x13|x12|x11|x10|x9|x8|x7|x6|x5|x4|x3|x2|x1|x0|
	;     |				d1			   |			d0		   |
	;q1 = |y15|y14|y13|y12|y11|y10|y9|y8|y7|y6|y5|y4|y3|y2|y1|y0|
	;     |				d3			   |			d2		   |

	vld1.u8		{q0}, [r0], r1
	vld1.u8		{q1}, [r0], r1

	; q4,q5 = |x16|x15|x14|x13|x12|x11|x10|x9|x8|x7|x6|x5|x4|x3|x2|x1|
	vld1.u8		d8[0] , [r4], r1
	vld1.u8		d10[0], [r4], r1						 
    pld			[r0]
    pld			[r0, r1]
	vext.8		q4, q0, q4, #1	
	vext.8		q5, q1, q5, #1

;	k0 = (((PF0 + PF1) << 3) + iRndCtrl) >> 4;
;	k1 = (((PF1 + PF2) << 3) + iRndCtrl) >> 4;
;	...

	; q0+q4 => q9,q8; q1+q5 => q11,q10
	; q8/q10 = |m7|m6|m5|m4|m3|m2|m1|m0|
	; q9/q11 = |m15|m14|m13|m12|m11|m10|m9|m8|
	; (m15 = x16+x15, ... , m0 = x1+x0)
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
	vshr.u16	q8 , #4			;(((PF0 + PF1)<<3) + iRndCtrl)>>4
	vshr.u16	q9 , #4
	vshr.u16	q10, #4
	vshr.u16	q11, #4

;	*(I16_WMV *)(pDst + j) = (U8_WMV) k0;
;	*(I16_WMV *)(pDst + j + 20) = (U8_WMV) k1;

  ; {q8/q10; q9/q11} => {q0,q2 = |m14|m12|m10|m8|m6|m4|m2|m0|; q1,q3 = |m15|m13|m11|m9|m7|m5|m3|m1| }
	
	vmovn.i32	d0 , q8		;d0 = |m6|m4|m2|m0|
	vmovn.i32	d1 , q9		;d1 = |m14|m12|m10|m8| 
	vmovn.i32	d2 , q10
	vmovn.i32	d3 , q11
	vshr.u32	q8 , #16	;q8 = |0|m7|0|m5|0|m3|0|m1|
	vshr.u32	q9 , #16	;q9 = |0|m15|0|m13|0|m11|0|m9|
	vshr.u32	q10, #16
	vshr.u32	q11, #16
	vmovn.i32	d4 , q8		;d8 = |m7|m5|m3|m1|
	vmovn.i32	d5 , q9		;d9 = |m15|m13|m11|m9| 
	vmovn.i32	d6 , q10
	vmovn.i32	d7 , q11
	
	vst1.u32	{q0}, [r2], r14	;k0
	vst1.u32	{q1}, [r2], r14	
	vst1.u32	{q2}, [r6], r14	;k1
	vst1.u32	{q3}, [r6], r14
	
	subs		r12, r12, #2
	bne			IBB_10_16_loop

	ldmia     sp!, {r4-r7, pc}
	
	WMV_ENTRY_END	;ARMV7_g_InterpolateBlockBilinear_SSIMD_10

    
    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;Void_WMV  g_InterpolateBlockBilinear_SSIMD_01 (const U8_WMV *pSrc, 
;                                           I32_WMV iSrcStride, 
;                                           U8_WMV *pDst, 
;                                           I32_WMV iXFrac, 
;                                           I32_WMV iYFrac, 
;                                           I32_WMV iRndCtrl, 
;                                           Bool_WMV b1MV)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    AREA  |.text|, CODE
    WMV_LEAF_ENTRY ARMV7_g_InterpolateBlockBilinear_SSIMD_01

;r0=pSrc
;r1=iSrcStride
;r2=pDst
;r3=iRndCtl
;r6 = pDst + 20
;r12 = loop num
;r14 = 40
;q7  = iRndCtl


    stmdb     sp!, {r4-r7, r14}
    FRAME_PROFILE_COUNT
    
  ;  I32_WMV iNumLoops = 8<<b1MV;    
  ;  iRndCtrl = 8 - ( iRndCtrl&0xff);

    pld			[r0]
    pld			[r0, r1]
	ldr			r3, [sp, #20+4]
	ldr			r4, [sp, #20+8]
	add			r6, r2, #20		;pDst + 20
	mov			r14, #40
	and			r3, r3, #0xff
	rsb			r3, r3, #8		
	vdup.16		q7, r3			;iRndCtrl
	
	cmp			r4, #0
	bne			IBB_01_16_loop_Start
    
	vld1.u8		d0, [r0], r1
	mov			r12, #8
IBB_01_8_loop

;	load src 8 bytes data of per line.
;	d0 = |x7|x6|x5|x4|x3|x2|x1|x0|
;	d1 = |y7|y6|y5|y4|y3|y2|y1|y0|
    
	vld1.u8		d1, [r0], r1
	vld1.u8		d2, [r0], r1

;    PF0 = pT[0] + pT[iSrcStride+0];
;    PF1 = pT[1] + pT[iSrcStride+1];
;    PF2 = pT[2] + pT[iSrcStride+2];
;    PF3 = pT[3] + pT[iSrcStride+3];
							
	vaddl.u8	q8 , d0, d1
	vaddl.u8	q9 , d1, d2
    pld			[r0]
    pld			[r0, r1]
	
;    k0 = ((PF0 << 3) + iRndCtrl) >> 4;
;    k1 = ((PF1 << 3) + iRndCtrl) >> 4;
	vshl.i16	q8 , #3
	vshl.i16	q9 , #3
	vadd.i16	q8 , q7
	vadd.i16	q9 , q7
	vshr.u16	q8 , #4
	vshr.u16	q9 , #4
	
	vshr.s32 	d0, d2, #0  ;vmov	d0, d2	;for next loop
	
;    *(I16_WMV *)(pDst + j) = (U8_WMV) k0;
;    *(I16_WMV *)(pDst + j + 20) = (U8_WMV) k1;
	
	; q8,q9 = |z7|z6|z5|z4|z3|z2|z1|z0| => {d2,d4 = |z6|z4|z2|z0|, d3,d5 = |z7|z5|z3|z1|}
	
	vmovn.i32	d2 , q8		;d2 = |z6|z4|z2|z0|
	vmovn.i32	d4 , q9 
	vshr.u32	q8 , #16	;q8 = |0|z7|0|z5|0|z3|0|z1|
	vshr.u32	q9 , #16
	vmovn.i32	d3 , q8		;d3 = |z7|z5|z3|z1|
	vmovn.i32	d5 , q9 
	
	vst1.u32	d2 , [r2], r14	;k0
	vst1.u32	d4 , [r2], r14  
	vst1.u32	d3 , [r6], r14	;k1
	vst1.u32	d5 , [r6], r14
	
	subs		r12, r12, #2
	bne			IBB_01_8_loop
    
    ldmia		sp!, {r4-r7, pc}    
	
IBB_01_16_loop_Start    

;	load src 16 bytes data per line
;	q0 = |x15|x14|x13|x12|x11|x10|x9|x8|x7|x6|x5|x4|x3|x2|x1|x0|
;	     |				d1			   |			d0		   |
;	q1 = |y15|y14|y13|y12|y11|y10|y9|y8|y7|y6|y5|y4|y3|y2|y1|y0|
;	     |				d3			   |			d2		   |
;	...

	vld1.u8		{q0}, [r0], r1
	mov			r12, #16
	
IBB_01_16_loop

	vld1.u8		{q1}, [r0], r1
	vld1.u8		{q2}, [r0], r1
    pld			[r0]
    pld			[r0, r1]

;    PF0 = pT[0] + pT[iSrcStride+0];
;    PF1 = pT[1] + pT[iSrcStride+1];
;    PF2 = pT[2] + pT[iSrcStride+2];
;    PF3 = pT[3] + pT[iSrcStride+3];

	vaddl.u8	q8 , d0, d2		;q8 = |z7 |z6 |z5 |z4 |z3 |z2 |z1|z0|
	vaddl.u8	q9 , d1, d3		;q9 = |z15|z14|z13|z12|z11|z10|z9|z8|
	vaddl.u8	q10, d2, d4		;(z16 = x15 + y15, ..., z0 = x0 + y0)
	vaddl.u8	q11, d3, d5							
		
	vshr.s32 	q0, q2, #0  ;vmov	q0, q2		; for next loop
	
;    k0 = ((PF0 << 3) + iRndCtrl) >> 4;
;    k1 = ((PF1 << 3) + iRndCtrl) >> 4;
	
	vshl.i16	q8 , #3
	vshl.i16	q9 , #3
	vshl.i16	q10, #3
	vshl.i16	q11, #3
	vadd.i16	q8 , q7
	vadd.i16	q9 , q7
	vadd.i16	q10, q7
	vadd.i16	q11, q7
	vshr.u16	q8 , #4
	vshr.u16	q9 , #4
	vshr.u16	q10, #4
	vshr.u16	q11, #4
	
	; write the results into Dst buffer.
  ; {q8/q10; q9/q11} => {q2,q4 = |m14|m12|m10|m8|m6|m4|m2|m0|; q3,q5 = |m15|m13|m11|m9|m7|m5|m3|m1| }
	
	vmovn.i32	d4 , q8		;d4 = |m6|m4|m2|m0|
	vmovn.i32	d5 , q9		;d6 = |m14|m12|m10|m8| 
	vmovn.i32	d6 , q10
	vmovn.i32	d7 , q11
	vshr.u32	q8 , #16	;q8 = |0|m7|0|m5|0|m3|0|m1|
	vshr.u32	q9 , #16	;q9 = |0|m15|0|m13|0|m11|0|m9|
	vshr.u32	q10, #16
	vshr.u32	q11, #16
	vmovn.i32	d8 , q8		;d5 = |m7|m5|m3|m1|
	vmovn.i32	d9 , q9		;d7 = |m15|m13|m11|m9| 
	vmovn.i32	d10, q10
	vmovn.i32	d11, q11
	
	vst1.u32	{q2}, [r2], r14	;k0
	vst1.u32	{q3}, [r2], r14	
	vst1.u32	{q4}, [r6], r14	;k1
	vst1.u32	{q5}, [r6], r14
	
	subs		r12, r12, #2
	bne			IBB_01_16_loop	
    ldmia		sp!, {r4-r7, pc}
    
    WMV_ENTRY_END	;ARMV7_g_InterpolateBlockBilinear_SSIMD_01


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;U32_WMV g_NewVertFilterX(const U8_WMV  *pSrc,
;                             const I32_WMV iSrcStride, 
;                             U8_WMV * pDst, 
;                             const I32_WMV iShift, 
;                             const I32_WMV iRound32, 
;                             const I8_WMV * const pV, 
;                             I32_WMV iNumHorzLoop, 
;                             const U32_WMV uiMask,
;                             Bool_WMV b1MV
;                             )
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    AREA  |.text|, CODE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

VFX_Core_Acc

	stmdb		sp!, {r14}
	
	; + iRound32
	vdup.16		q6, r4			; t0
	vdup.16		q7, r4			; t1
	vdup.16		q8, r4			; t2
	vdup.16		q9, r4			; t3
	; - o0*abs(v0)
	vmlsl.u8	q6, d0, d24		
	vmlsl.u8	q7, d1, d24		
	vmlsl.u8	q8, d2, d24		
	vmlsl.u8	q9, d3, d24		
	vshr.s32 	d0, d4, #0
	; + o1*v1
	vmlal.u8	q6, d1, d25		
	vmlal.u8	q7, d2, d25		
	vmlal.u8	q8, d3, d25		
	vmlal.u8	q9, d4, d25		
	vshr.s32 	d1, d5, #0
	; + o2*v2
	vmlal.u8	q6, d2, d26		
	vmlal.u8	q7, d3, d26		
	vmlal.u8	q8, d4, d26		
	vmlal.u8	q9, d5, d26		
	vshr.s32 	d2, d6, #0
	; - o3*abs(v3)
	vmlsl.u8	q6, d3, d27		
	vmlsl.u8	q7, d4, d27		
	vmlsl.u8	q8, d5, d27		
	vmlsl.u8	q9, d6, d27		
	
	; overflow |= t0;
	; t0 >>= iShift;
	; t0 &= uiMask;
	vorr		q14, q14, q6
	vshl.u16	q6, q6, q15
	vand		q6, q6, q11
	vorr		q14, q14, q7
	vshl.u16	q7, q7, q15
	vand		q7, q7, q11
	vorr		q14, q14, q8
	vshl.u16	q8, q8, q15
	vand		q8, q8, q11
	vorr		q14, q14, q9
	vshl.u16	q9, q9, q15
	vand		q9, q9, q11	
		
	; *(U32_WMV *)pDst = t0;             
	vmovn.i32	d20, q6		;d20 = |m6|m4|m2|m0|
	vshr.u32	q6, #16		;q6 = |0|m7|x|m5|x|m3|x|m1|
	vmovn.i32	d21, q6		;d21 = |m7|m5|m3|m1|
	vmovn.i32	d12, q7		 
	vshr.u32	q7, #16	
	vmovn.i32	d13, q7	 
	vmovn.i32	d14, q8
	vshr.u32	q8, #16
	vmovn.i32	d15, q8
	vmovn.i32	d16, q9
	vshr.u32	q9, #16
	vmovn.i32	d17, q9	
		
	ldmia		sp!, {pc}

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
VFX_extra_data_for_HFX	
	
	stmdb		sp!, {r14}
	vld1.u8		d0, [r7], r1
	vld1.u8		d1, [r7], r1
	vld1.u8		d2, [r7], r1	
    vmov.i32	q14, #0					;overflow
VFX_loop_for_HFX
	vld1.u8		d3, [r7], r1
	vld1.u8		d4, [r7], r1
	vld1.u8		d5, [r7], r1
	vld1.u8		d6, [r7], r1
    pld			[r7]
    pld			[r7, r1]
    pld			[r7, r1,lsl #1]
    add			r14, r1, r1, lsl #1
    pld			[r7, r14]
	bl			VFX_Core_Acc
	vst1.u32	d20[0], [r2], r3	;k0
	vst1.u32	d21[0], [r5], r3	;k1
	vst1.u32	d12[0], [r2], r3	
	vst1.u32	d13[0], [r5], r3	
	vst1.u32	d14[0], [r2], r3	
	vst1.u32	d15[0], [r5], r3	
	vst1.u32	d16[0], [r2], r3	
	vst1.u32	d17[0], [r5], r3	
	subs		r6, r6, #4
	bne			VFX_loop_for_HFX
	
   	vmov.i64	d21, #0x0000ffffffffffff			
    vand		d28, d21
   	vmov		r0, r1, d28
   	orr			r10, r10, r11
   	orr			r0, r0, r1
   	orr			r0, r0, r10
	
	ldmia		sp!, {pc}
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	    
    WMV_LEAF_ENTRY ARMV7_g_NewVertFilterX

;r0 = pSrc
;r1 = iSrcStride
;r2 = pDst
;r3 = iShift, iDstStride(40)
;r4 = iRound32
;r5 = pV / pDst + 20
;r6 = (i<<8)+iNumHorzLoop
;r7 = uiMask, pSrc + 8 / pSrc + 16
;r8  = v0
;r9  = v1
;r10 = v2
;r11 = v3
;r12, r14 = tmp
;q11 = uiMask
;q12 = v0,v1
;q13 = v2,v3
;q14 = overflow
;q15 = iShift
	
	stmdb		sp!, {r4 - r11, r14}
	FRAME_PROFILE_COUNT

    pld			[r0]
    pld			[r0, r1]
    pld			[r0, r1, lsl #1]
    add			r14, r1, r1, lsl #1
    pld			[r0, r14]
    add			r14, r14, r1
    pld			[r0, r14]
    
    ldr			r4, [sp, #36+0]
    ldr			r5, [sp, #36+4]
    ldr			r6, [sp, #36+8]
    ldr			r7, [sp, #36+12]
    ldr			r12, [sp, #36+16]   
    rsb			r3, r3, #0
    vmov.i32	q14, #0			;overflow
	vdup.16		q15, r3		
    ldrb		r8, [r5]
    ldrb		r9, [r5, #1]
    ldrb		r10, [r5, #2]
    ldrb		r11, [r5, #3]
	mov			r3, #40	
    add			r5, r2, #20
    rsb			r8, r8, #0		
    rsb			r11, r11, #0	
	vdup.8		d24, r8			; abs(v0)
	vdup.8		d25, r9			; v1
	vdup.8		d26, r10		; v2
	vdup.8		d27, r11		; abs(v3)
	vdup.32		q11, r7			; uiMask

	cmp			r12, #0
	bne			VFX_16_loop_start
	
;	t0 = o0*v0 + o1*v1 + o2*v2 +o3*v3 + iRound32;	
;	   = -o0*abs(v0) + o1*v1 + o2*v2 - o3*abs(v0) + iRound32;

;	o0: d0 = |x7|x6|x5|x4|x3|x2|x1|x0| / |x|x|x|x|x|x10|x9|x8|
;	o1: d1 = ...
;	o2: d2 = ...
;	o3: d3 = ...	

	add			r7, r0, #8
	vld1.u8		d0, [r0], r1
	vld1.u8		d1, [r0], r1
	vld1.u8		d2, [r0], r1
	add			r6, r6, #(8-4)<<8
	
VFX_8_loop
	vld1.u8		d3, [r0], r1
	vld1.u8		d4, [r0], r1
	vld1.u8		d5, [r0], r1
	vld1.u8		d6, [r0], r1
    pld			[r0]
    pld			[r0, r1]
    pld			[r0, r1, lsl #1]
    add			r14, r1, r1, lsl #1
    pld			[r0, r14]
	bl			VFX_Core_Acc
	vst1.u32	d20, [r2], r3	;k0
	vst1.u32	d21, [r5], r3	;k1
	vst1.u32	d12, [r2], r3	
	vst1.u32	d13, [r5], r3
	vst1.u32	d14, [r2], r3	
	vst1.u32	d15, [r5], r3	
	vst1.u32	d16, [r2], r3	
	vst1.u32	d17, [r5], r3
	subs		r6, r6, #4<<8
	bge			VFX_8_loop
	
	vorr		d29, d28, d29
	vmov		r10, r11, d29
	ands		r12, r6, #1
	orreq		r0, r10, r11
	ldmeqia		sp!, {r4 - r11, pc}
	
	; for HFX
	sub			r2, r2, r3, lsl #3
	mov			r6, #8
    add			r5, r2, #28
    add			r2, r2, #8
	bl			VFX_extra_data_for_HFX
	ldmia		sp!, {r4 - r11, pc}
	
VFX_16_loop_start

;	t0 = o0*v0 + o1*v1 + o2*v2 +o3*v3 + iRound32;	
;	   = -o0*abs(v0) + o1*v1 + o2*v2 - o3*abs(v0) + iRound32;

;	o0: q0 = |x15|x14|x13|x12|x11|x10|x9|x8|x7|x6|x5|x4|x3|x2|x1|x0| / |x|x|x|x|x|x18|x17|x16|
;	o1: q1 = ...
;	o2: q2 = ...
;	o3: q3 = ...	

	add			r7, r0, #16
	vld1.u8		{q0}, [r0], r1
	vld1.u8		{q1}, [r0], r1
	vld1.u8		{q2}, [r0], r1
	add			r6, r6, #(16-2)<<8
	
VFX_16_loop
	vld1.u8		{q3}, [r0], r1
	vld1.u8		{q4}, [r0], r1

	; + iRound32
	vdup.16		q6, r4			; t0, current row
	vdup.16		q7, r4			;
	vdup.16		q8, r4			; t1, next row
	vdup.16		q9, r4			; 
	
    pld			[r0]
    pld			[r0, r1]
	
	; - o0*abs(v0)
	vmlsl.u8	q6, d0, d24		
	vmlsl.u8	q7, d1, d24	
	vmlsl.u8	q8, d2, d24		
	vmlsl.u8	q9, d3, d24	
	vshr.s32 	q0, q2, #0
		
	; + o1*v1
	vmlal.u8	q6, d2, d25		
	vmlal.u8	q7, d3, d25	
	vmlal.u8	q8, d4, d25		
	vmlal.u8	q9, d5, d25	
	vshr.s32 	q1, q3, #0
		
	; + o2*v2
	vmlal.u8	q6, d4, d26		
	vmlal.u8	q7, d5, d26		
	vmlal.u8	q8, d6, d26		
	vmlal.u8	q9, d7, d26	
	vshr.s32 	q2, q4, #0
	
	; - o3*abs(v3)
	vmlsl.u8	q6, d6, d27		
	vmlsl.u8	q7, d7, d27		
	vmlsl.u8	q8, d8, d27		
	vmlsl.u8	q9, d9, d27	
	
	; overflow |= t0;
	; t0 >>= iShift;
	; t0 &= uiMask;
	
	vorr		q14, q14, q6
	vshl.u16	q6, q6, q15
	vand		q6, q6, q11
	vorr		q14, q14, q7
	vshl.u16	q7, q7, q15
	vand		q7, q7, q11
	vorr		q14, q14, q8
	vshl.u16	q8, q8, q15
	vand		q8, q8, q11
	vorr		q14, q14, q9
	vshl.u16	q9, q9, q15
	vand		q9, q9, q11	

	; *(U32_WMV *)pDst = t0;             
	
	vmovn.i32	d6, q6		;d0 = |m6|m4|m2|m0|
	vmovn.i32	d7, q7		;d1 = |m14|m12|m10|m8| 
	vmovn.i32	d8, q8
	vmovn.i32	d9, q9
	vshr.u32	q6, #16		;q6 = |0|m7|x|m5|x|m3|x|m1|
	vshr.u32	q7, #16		;q7 = |0|m15|x|m13|x|m11|x|m9|
	vshr.u32	q8, #16
	vshr.u32	q9, #16
	vmovn.i32	d10, q6		;d2 = |m7|m5|m3|m1|
	vmovn.i32	d11, q7		;d3 = |m15|m13|m11|m9|	 
	vmovn.i32	d20, q8
	vmovn.i32	d21, q9	
	
	vst1.u32	{q3}, [r2], r3	;k0
	vst1.u32	{q4}, [r2], r3	;k1
	vst1.u32	{q5}, [r5], r3	
	vst1.u32	{q10}, [r5], r3	

	subs		r6, r6, #2<<8
	bge			VFX_16_loop
	
	vorr		d29, d28, d29
	vmov		r10, r11, d29
	ands		r12, r6, #1
	orreq		r0, r10, r11
	ldmeqia		sp!, {r4 - r11, pc}

	; for HFX
	sub			r2, r2, r3, lsl #4
	mov			r6, #16
    add			r5, r2, #36
    add			r2, r2, #16
	bl			VFX_extra_data_for_HFX
	ldmia		sp!, {r4 - r11, pc}

     WMV_ENTRY_END	;  ARMV7_g_NewVertFilterX


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;    U32_WMV g_NewHorzFilterX(U8_WMV *pF, 
;                              const I32_WMV iShift, 
;                              const I32_WMV iRound2_32, 
;                              const I8_WMV * const pH, 
;                              Bool_WMV b1MV)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    
    
    AREA  |.text|, CODE
    WMV_LEAF_ENTRY ARMV7_g_NewHorzFilterX
	
;r0 = pF
;r1 = iShift
;r2 = iRound2_32
;r3 = pH, i
;r4 = h3,h2,h1,h0
;r5 = pDst1 = pF
;r6 = pDst2 = pF + 20
;r7 = 40
;r8 = pF + 8 / pF + 16
;r9 = pF + 20
;r10 = pF + 28 / pF + 36
;q0 = h3,h2,h1,h0
;q7 = overflow

    stmdb     sp!, {r4 - r10, r14}
    FRAME_PROFILE_COUNT
    
	pld			[r0]
	pld			[r0, r7]
	
    ldr			r12, [sp, #32+0]	;1mv
    ldr			r4, [r3]
    mov			r5, r0
    add			r6, r0, #20
    mov			r7, #40
    add			r9, r0, #20
    vmov.i32	q7, #0			;overflow
	vdup.32		d0, r4			; h3,h2,h1,h0
	vmov.i16	q15, #0x00ff		
	vmovl.s8	q0, d0			; extend to 16 bits
    rsb			r1, r1, #0
	vdup.16		q14, r1			; shift
    
	cmp			r12, #0
	bne			HFX_16_loop_Start
	
	mov			r3, #8
    add			r8, r0, #8
    add			r10, r0, #28
HFX_8_loop    

;	q1,q2:current row , q1 = |x|x|x10|x8|x6|x4|x2|x0|, q2 = |x|x|x|x9|x7|x5|x3|x1|
;	q3,q4:next row    , q3 = |x|x|y10|y8|y6|y4|y2|y0|, q4 = |x|x|x|y9|y7|y5|y3|y1|
	
	vld1.u32	d2, [r0], r7
	vld1.u32	d3[0], [r8], r7  ;d3.U32[0]
	vld1.u32	d4, [r9], r7
	vld1.u32	d5[0], [r10], r7
	vld1.u32	d6, [r0], r7
	vld1.u32	d7[0], [r8], r7
	vld1.u32	d8, [r9], r7
	vld1.u32	d9[0], [r10], r7

	pld			[r0]
	pld			[r0, r7]
	
;   o0 = m0 * h0  + m1 * h1 + m2 * h2 + m3 * h3 + iRound2_32;
;   o1 = m1 * h0  + m2 * h1 + m3 * h2 + m4 * h3 + iRound2_32;
;	x x 10  8  6 4 2 0 - m0          - q1, q3
;	x x  x  9  7 5 3 1 - m1          - q2, q4
;	x x  x  x  8 6 4 2 - m2 (m0>>16) - q8, q11
;	x x  x  x  9 7 5 3 - m3 (m1>>16) - q9, q12
;	x x  x  x 10 8 6 4 - m4 (m0>>32) - q10, q13
							
	; + iRound2_32; 
	vdup.16		q5, r2			; d10:o0, d11:o1, current row
	vdup.16		q6, r2			; d10:o0, d11:o1, next row		
	
	vmla.s16	d10, d2, d0[0]	; + m0 * h0
	vmla.s16	d11, d4, d0[0]	; + m1 * h0
	vmla.s16	d12, d6, d0[0]	
	vmla.s16	d13, d8, d0[0]	
	vext.16		d16, d2, d3, #1		; m2
	vext.16		d22, d6, d7, #1		; m2
	
	vmla.s16	d10, d4 , d0[1]	; + m1 * h1
	vmla.s16	d11, d16, d0[1]	; + m2 * h1
	vmla.s16	d12, d8 , d0[1]		
	vmla.s16	d13, d22, d0[1]		
	vext.16		d18, d4, d5, #1		; m3
	vext.16		d24, d8, d9, #1		; m3
	
	vmla.s16	d10, d16, d0[2]	; + m2 * h2
	vmla.s16	d11, d18, d0[2]	; + m3 * h2
	vmla.s16	d12, d22, d0[2]		
	vmla.s16	d13, d24, d0[2]		
	vext.16		d20, d2, d3, #2		; m4
	vext.16		d26, d6, d7, #2		; m4
	
	vmla.s16	d10, d18, d0[3]	; + m3 * h3
	vmla.s16	d11, d20, d0[3]	; + m4 * h3
	vmla.s16	d12, d24, d0[3]		
	vmla.s16	d13, d26, d0[3]			

	subs		r3, r3, #2
	
;    overflow |= o0;
;    overflow |= o1;
	vorr		q7, q7, q5
	vorr		q7, q7, q6

;    o0 >>= iShift;
;    o1 >>= iShift;
	vshl.s16	q5, q5, q14
	vshl.s16	q6, q6, q14

;    o0 &= 0x00ff00ff;
;    o1 &= 0x00ff00ff;
	vand		q5, q5, q15
	vand		q6, q6, q15
    
;    *(U32_WMV *)pF = o0;
;    *(U32_WMV *)(pF+20) = o1;
	
	vst1.u32	d10, [r5], r7		;o0
	vst1.u32	d11, [r6], r7		;o1
	vst1.u32	d12, [r5], r7
	vst1.u32	d13, [r6], r7

	bne			HFX_8_loop

	vorr		d14, d14, d15
	vmov		r0, r1, d14
	orr			r0, r0, r1
    ldmia		sp!, {r4 - r10, pc}

HFX_16_loop_Start

	mov			r3, #16
    add			r8, r0, #16
    add			r10, r0, #36
HFX_16_loop

;	load one row of src data , 
;	q1 = |x14|x12|x10|x8|x6|x4|x2|x0|, q2 = |x15|x13|x11|x9|x7|x5|x3|x1|, 
;	q3[0] = |x|x|x18|x16|, q4[0] = |x|x|x19|x17|
	
	vld1.u32	{q1}, [r0], r7
	vld1.u32	d6[0], [r8], r7  ;d6.u32[0]
	vld1.u32	{q2}, [r9], r7
	vld1.u32	d8[0], [r10], r7
	
	pld			[r0, r7]

;   o0 = m0 * h0  + m1 * h1 + m2 * h2 + m3 * h3 + iRound2_32;
;   o1 = m1 * h0  + m2 * h1 + m3 * h2 + m4 * h3 + iRound2_32;
;	x x 18 16 14 12 10  8  6 4 2 0  - m0          - q1
;	x x  x 17 15 13 11  9  7 5 3 1  - m1          - q2
;	x x  x  x 16 14 12 10  8 6 4 2  - m2 (m0>>16) - q8
;	x x  x  x 17 15 13 11  9 7 5 3  - m3 (m1>>16) - q9
;	x x  x  x 18 16 14 12 10 8 6 4  - m4 (m0>>32) - q10
							
	; + iRound2_32; 
	vdup.16		q5, r2				; o0
	vshr.s32 	q6, q5,#0				; o1
	
	vmla.s16	q5, q1, d0[0]	; + m0 * h0
	vmla.s16	q6, q2, d0[0]	; + m1 * h0
	vext.16		q8, q1, q3, #1		; m2
	
	vmla.s16	q5, q2, d0[1]	; + m1 * h1
	vmla.s16	q6, q8, d0[1]	; + m2 * h1
	vext.16		q9, q2, q4, #1		; m3
	
	vmla.s16	q5, q8, d0[2]	; + m2 * h2
	vmla.s16	q6, q9, d0[2]	; + m3 * h2
	vext.16		q10, q1, q3, #2		; m4
	
	vmla.s16	q5, q9, d0[3]	; + m3 * h3
	vmla.s16	q6, q10, d0[3]	; + m4 * h3

	subs		r3, r3, #1
	
;    overflow |= o0;
;    overflow |= o1;
	vorr		q7, q7, q5
	vorr		q7, q7, q6

;    o0 >>= iShift;
;    o1 >>= iShift;
	vshl.s16	q5, q5, q14
	vshl.s16	q6, q6, q14

;    o0 &= 0x00ff00ff;
;    o1 &= 0x00ff00ff;
	vand		q5, q5, q15
	vand		q6, q6, q15
    
;    *(U32_WMV *)pF = o0;
;    *(U32_WMV *)(pF+20) = o1;
	
	vst1.u32	{q5}, [r5], r7		;o0
	vst1.u32	{q6}, [r6], r7		;o1
							
	bne			HFX_16_loop
	
	vorr		d14, d14, d15
	vmov		r0, r1, d14
	orr			r0, r0, r1
    ldmia		sp!, {r4 - r10, pc}
	
    WMV_ENTRY_END	;ARMV7_g_NewHorzFilterX

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Void_WMV g_InterpolateBlock_00_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, 
;										I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV) // iXFrac == 0; iYFrac == 0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    AREA  |.text|, CODE
    WMV_LEAF_ENTRY ARMV7_g_InterpolateBlock_00_SSIMD
	
;r0 = pSrc
;r1 = iSrcStride
;r2 = pDst
;r3 = b1MV
;r4 = i
;r5 = pDst + 20
;r6 = 40
;r7 = pDst + 28
	
IBB_0x00FF_Q	QN	q4		
	
    stmdb   sp!, {r4 - r7, r14}
    
    pld			[r0]
    pld			[r0, r1]
    pld			[r0, r1, lsl #1]
    add			r14, r1, r1, lsl #1
    pld			[r0, r14]
    ldr			r3, [ sp, #20+8]    
    mov			r6, #40
    add			r5, r2, #20
    add			r7, r2, #28
	cmp			r3, #0
    vmov.u16	IBB_0x00FF_Q, #0x00FF	
	bne			IB00_Loop16_2_start
	
    pld			[r0, r1, lsl #2]
    add			r14, r1, r1, lsl #2
    pld			[r0, r14]
    add			r14, r14, r1
    pld			[r0, r14]
    add			r14, r14, r1
    pld			[r0, r14]
	mov			r4, #8
IB00_Loop8_1    
;	q0 = |x7|x6|x5|x4|x3|x2|x1|x0|x7|x6|x5|x4|x3|x2|x1|x0|
;	q1 = ...
	vld1.8		d0, [r0], r1
	vld1.8		d1, [r0], r1
	vld1.8		d2, [r0], r1
	vld1.8		d3, [r0], r1
	subs		r4, r4, #4
	vand		q8 , q0, IBB_0x00FF_Q	;q8 = |0|x6|0|x4|0|x2|0|x0|0|x6|0|x4|0|x2|0|x0| 
	vand		q10, q1, IBB_0x00FF_Q		
	vext.8		q0 , q0, q15, #1		;q0 = |x|x7|x6|x5|x4|x3|x2|x1|x0|x7|x6|x5|x4|x3|x2|x1|
	vext.8		q1 , q1, q15, #1		
	vand		q9 , q0, IBB_0x00FF_Q	;q9 = |0|x7|0|x5|0|x3|0|x1|0|x7|0|x5|0|x3|0|x1| 
	vand		q11, q1, IBB_0x00FF_Q		
	vst1.32		d16, [r2], r6	;k0
	vst1.32		d17, [r2], r6	
	vst1.32		d20, [r2], r6	
	vst1.32		d21, [r2], r6	
	vst1.32		d18, [r5], r6	;k1
	vst1.32		d19, [r5], r6	
	vst1.32		d22, [r5], r6	
	vst1.32		d23, [r5], r6	
	bne			IB00_Loop8_1
	
	mov			r0, #0
	ldmia		sp!, {r4 - r7, pc}

IB00_Loop16_2_start

	mov		r4, #16
IB00_Loop16_2
;	q0 = |x15|x14|x13|x12|x11|x10|x9|x8|x7|x6|x5|x4|x3|x2|x1|x0|
;	q1 = ...
;	q2 = ...
;	q3 = ...
	vld1.8		{q0}, [r0], r1
	vld1.8		{q1}, [r0], r1
	vld1.8		{q2}, [r0], r1
	vld1.8		{q3}, [r0], r1
	subs		r4, r4, #4
    pld			[r0]
    pld			[r0, r1]
    pld			[r0, r1, lsl #1]
    add			r14, r1, r1, lsl #1
    pld			[r0, r14]
	vand		q8 , q0, IBB_0x00FF_Q		;q8 = |0|x14|0|x12|0|x10|0|x8|0|x6|0|x4|0|x2|0|x0| 
	vand		q10, q1, IBB_0x00FF_Q		
	vand		q12, q2, IBB_0x00FF_Q			 
	vand		q14, q3, IBB_0x00FF_Q		
	vext.8		q0 , q0, q15, #1			;q0 = |x|x15|x14|x13|x12|x11|x10|x9|x8|x7|x6|x5|x4|x3|x2|x1|
	vext.8		q1 , q1, q15, #1		
	vext.8		q2 , q2, q15, #1		
	vext.8		q3 , q3, q15, #1		
	vand		q9 , q0, IBB_0x00FF_Q		;q9 = |0|x15|0|x13|0|x11|0|x9|0|x7|0|x5|0|x3|0|x1| 
	vand		q11, q1, IBB_0x00FF_Q		
	vand		q13, q2, IBB_0x00FF_Q			 
	vand		q15, q3, IBB_0x00FF_Q		
	vst1.64		{d16,d17}, [r2], r6		
	vst1.64		{d20,d21}, [r2], r6	
	vst1.64		{d24,d25}, [r2], r6	
	vst1.64		{d28,d29}, [r2], r6	
	vst1.32		d18, [r5], r6			
	vst1.32		d19, [r7], r6	
	vst1.32		d22, [r5], r6	
	vst1.32		d23, [r7], r6	
	vst1.32		d26, [r5], r6	
	vst1.32		d27, [r7], r6	
	vst1.32		d30, [r5], r6	
	vst1.32		d31, [r7], r6		
	bne			IB00_Loop16_2
	
	mov			r0, #0
	ldmia		sp!, {r4 - r7, pc}
	    
    WMV_ENTRY_END	;ARMV7_g_InterpolateBlock_00_SSIMD
    
    WMV_LEAF_ENTRY ARMV7_g_NewVertFilter0LongNoGlblTbl

;r0 = pSrc
;r1 = iSrcStride
;r2 = pDst
;r3 = b1MV
;r14 = i
	
    FRAME_PROFILE_COUNT
    stmdb		sp!, {r4 - r11, r14}
    
    mov		r4, r0
    sub		sp, sp, #12
    mov		r5, r1
    str		r3, [sp, #8]
    mov		r6, r2
    
    bl		ARMV7_g_InterpolateBlock_00_SSIMD		;;;;;;;
    add		sp, sp, #12
 
	cmp		r3, #0
	bne		NVFL_Loop16_1_start
	
    add		r4, r4, #8
    add		r6, r6, #8
    pld		[r4, r5, lsl #1]
    add		r12, r5, r5, lsl #1
    pld		[r4, r12]
	mov		r14, #8
NVFL_Loop8_1
     ;  *(I32_WMV *)(pDst + 8)       = pSrc[8   ] | (pSrc[8+2]<<16);
     ;  *(I32_WMV *)(pDst + 8 + 20)  = pSrc[8+1 ] | (pSrc[8+3]<<16);     
	ldrb	r1, [r4, #1]
	ldrb	r2, [r4, #2]
	ldrb	r3, [r4, #3]
	ldrb	r0, [r4], r5
	ldrb	r9, [r4, #1]
	ldrb	r10, [r4, #2]
	ldrb	r11, [r4, #3]
	ldrb	r8, [r4], r5
	subs	r14, r14, #2
	pld		[r4, r5, lsl #1]
	add		r12, r5, r5, lsl #1
	pld		[r4, r12]
	orr	r1, r1, r3, lsl #16
	orr	r0, r0, r2, lsl #16
	str	r1, [r6, #20]
	str	r0, [r6], #40
	orr	r9, r9, r11, lsl #16
	orr	r8, r8, r10, lsl #16
	str	r9, [r6, #20]
	str	r8, [r6], #40
	bne	NVFL_Loop8_1
    
    mov			r0, #0
    ldmia		sp!, {r4 - r11, pc}

NVFL_Loop16_1_start

    add		r4, r4, #16
    add		r6, r6, #16
    pld		[r4, r5, lsl #1]
    add		r12, r5, r5, lsl #1
    pld		[r4, r12]
	mov		r14, #16
NVFL_Loop16_1
     ;	*(I32_WMV *)(pDst + 16)       = pSrc[16   ] | (pSrc[16+2]<<16);
     ;	*(I32_WMV *)(pDst + 16 + 20)  = pSrc[16+1 ] | (pSrc[16+3]<<16);
	ldrb	r1, [r4, #1]
	ldrb	r2, [r4, #2]
	ldrb	r3, [r4, #3]
	ldrb	r0, [r4], r5
	ldrb	r9, [r4, #1]
	ldrb	r10, [r4, #2]
	ldrb	r11, [r4, #3]
	ldrb	r8, [r4], r5
	subs	r14, r14, #2
	pld		[r4, r5, lsl #1]
	add		r12, r5, r5, lsl #1
	pld		[r4, r12]
	orr	r1, r1, r3, lsl #16
	orr	r0, r0, r2, lsl #16
	str	r1, [r6, #20]
	str	r0, [r6], #40
	orr	r9, r9, r11, lsl #16
	orr	r8, r8, r10, lsl #16
	str	r9, [r6, #20]
	str	r8, [r6], #40
	bne	NVFL_Loop16_1
     
    mov			r0, #0
    ldmia		sp!, {r4 - r11, pc}

    WMV_ENTRY_END	;ARMV7_g_NewVertFilter0LongNoGlblTbl


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Void_WMV g_AddNull_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV iPitch)
;{
;    I32_WMV iy;
;    U32_WMV u0,u1,u2,u3, y0,y1;  
;    for (iy = 0; iy < 8; iy++) 
;    {     
;        u0 = pRef[0];
;        u1 = pRef[0 + 5];
;        u2 = pRef[1];
;        u3 = pRef[1 + 5];
;
;        pRef += 10;
;        
;        y0 = (u0) | ((u1) << 8);
;        y1 = (u2) | ((u3) << 8);
;        
;        *(U32_WMV *)ppxlcDst = y0;
;        *(U32_WMV *)(ppxlcDst + 4)= y1;
;        ppxlcDst += iPitch;
;    }
;}

    AREA  |.text|, CODE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    WMV_LEAF_ENTRY ARMV7_g_AddNull_SSIMD
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
;r0 = ppxlcDst
;r1 = pRef
;r2 = iPitch

    stmdb     sp!, {r4, r14}
    FRAME_PROFILE_COUNT

	add			r3, r1, #20
	mov			r14, r1
	mov			r12, #40
	vld1.32		d8 , [r3], r12
	vld1.32		d9 , [r3], r12
	vld1.32		d10, [r3], r12
	vld1.32		d11, [r3], r12
	vld1.32		d12, [r3], r12
	vld1.32		d13, [r3], r12
	vld1.32		d14, [r3], r12
	vld1.32		d15, [r3]
	vld1.64		d0 , [r14], r12
	vld1.64		d1 , [r14], r12
	vld1.64		d2 , [r14], r12
	vld1.64		d3 , [r14], r12
	vld1.64		d4 , [r14], r12
	vld1.64		d5 , [r14], r12
	vld1.64		d6 , [r14], r12
	vld1.64		d7 , [r14]
	
	vshl.u32	q4, #8
	vshl.u32	q5, #8
	vshl.u32	q6, #8
	vshl.u32	q7, #8
	
	vorr		q0, q4
	vorr		q1, q5
	vorr		q2, q6
	vorr		q3, q7
	
	vst1.u32	d0, [r0], r2
	vst1.u32	d1, [r0], r2
	vst1.u32	d2, [r0], r2
	vst1.u32	d3, [r0], r2
	vst1.u32	d4, [r0], r2
	vst1.u32	d5, [r0], r2
	vst1.u32	d6, [r0], r2
	vst1.u32	d7, [r0]

    ldmia     sp!, {r4, pc}

    WMV_ENTRY_END  ;  g_AddNull_SSIMD


    ENDIF ; WMV_OPT_MOTIONCOMP_ARM= 1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


    END 
