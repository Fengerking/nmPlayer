    #include "../c/voWMVDecID.h" 
    .include "wmvdec_member_arm.inc"
    .include "xplatform_arm_asm.h" 

    @AREA    |.text|, CODE
    .section .text

    .IF WMV_OPT_LOOPFILTER_ARM == 1
 
    .IF PRO_VER != 0

    .global  ARMV7_g_FilterHorizontalEdgeV9
    .global  ARMV7_g_FilterHorizontalEdgeV9Last8x4
    .global  ARMV7_g_FilterVerticalEdgeV9
    .global  ARMV7_g_FilterVerticalEdgeV9Last4x8

  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Below registers are shored by ARMV7_g_FilterHorizontalEdgeV9 and ARMV7_g_FilterVerticalEdgeV9.

@r0 == pV5
@r1 == iPixelDistance
@r2 == iStepSize 
@r3 == uchBitField
@r4 == Is8x8Done
@r5 == pDst
@r6 == tmp	
@r12 == ((uchBitField & 0x02)>>1)	
@r14 == (uchBitField & 0x01)	

@golobal
v01					.req	d0	
v02					.req	d1
v03					.req	d2	
v04					.req	d3	
v05					.req	d4	
v06					.req	d5	
v07					.req	d6	
v08					.req	d7
v7_v8_Q				.req	q3	
validFlag			.req	q4		
validFlag_d_l		.req	d8		
validFlag_d_h		.req	d9		
Flag				.req	q5		
absA30				.req	q6		
a					.req	q7		
c					.req	q8		
iMina31_a32			.req	q9	
DEBLOCK_H_0x0004_Q	.req	q15		

@tmp	
DEBLOCK_H_0x8000_Q	.req	iMina31_a32		
v4_v5				.req	q10		
v3_v6				.req	q11		
a31					.req	q12		
a32					.req	q13		
v2_v3				.req	v4_v5		
v6_v7				.req	v3_v6		
dA30				.req	v2_v3		
iStepSize			.req	v4_v5		
tmp_Q				.req	q14	
tmp_Q_d0			.req	d28	
tmp_Q_d1			.req	d29	
tmp2_Q				.req	DEBLOCK_H_0x0004_Q	
tmp2_Q_d0			.req	d30	
tmp2_Q_d1			.req	d31	

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.macro Filter_Core_CtxA8
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	for ( i == 0@ i < 8@ i ++) {     
@       I16_WMV a30@
@       I16_WMV  v4_v5 == v4[i] - v5[i]@
@       c[i]      == abs(v4_v5)>>1@  
@       a30    == (2*(v3[i] - v6[i]) - 5 * v4_v5 + 4) >> 3@
@       absA30[i] == abs(a30)@
@		{
@       	I16_WMV flag1 == v4_v5 & MASK_0x8000@ //sign(v4_v5)
@       	I16_WMV flag2 == a30   & MASK_0x8000@ //sign(a30)
@       	Flag[i] == (flag1 != flag2) ? 1 : 0@                               
@       }
@       
@       {
@           I16_WMV v2_v3 == v2[i] - v3[i]@
@           I16_WMV v6_v7 == v6[i] - v7[i]@
@           I16_WMV a31   == (2 * (v1[i] - v4[i]) - 5 * v2_v3 + 4) >> 3@
@           I16_WMV a32   == (2 * (v5[i] - v8[i]) - 5 * v6_v7 + 4) >> 3@
@		
@		    I16_WMV dA30@
@		    iMina31_a32[i] == min( abs(a31), abs(a32) )@ 
@			dA30 == (5*(absA30[i] - iMina31_a32[i])) >> 3@
@		    a[i] == min(dA30, c[i])@
@		
@			if (v4[i] < v5[i]) 
@				a[i] == -a[i]@
@		}
@	}	
	
	
	vsubl.u8		v4_v5, v04, v05
	vsubl.u8		v3_v6, v03, v06
	vabs.s16		c, v4_v5
	vshl.s16		tmp_Q, v4_v5, #2
	vshl.s16		v3_v6, v3_v6, #1
	vshr.s16		c, c, #1 
	vadd.s16		tmp_Q, tmp_Q, v4_v5
    vmov.s16		DEBLOCK_H_0x0004_Q, #0x04	
	vsub.s16		a, v3_v6, tmp_Q
    vmov.u16		DEBLOCK_H_0x8000_Q, #0x8000	
	vadd.s16		a, a, DEBLOCK_H_0x0004_Q
	vmov.u16		tmp_Q, #0xFFFF
	vshr.s16		a, a, #3
	vand			a31, v4_v5, DEBLOCK_H_0x8000_Q
	vabs.s16		absA30, a 
	vand			a32, a, DEBLOCK_H_0x8000_Q
	vsubl.u8		v2_v3, v02, v03
	veor		    a31, a31, a32
	vsubl.u8		v6_v7, v06, v07
	vtst.16			Flag, a31, tmp_Q
	
	vsubl.u8		a31, v01, v04
	vshl.s16		tmp_Q, v2_v3, #2
	vsubl.u8		a32, v05, v08
	vshl.s16		a31, a31, #1
	vadd.s16		v2_v3, v2_v3, tmp_Q
	vshl.s16		tmp_Q, v6_v7, #2
	vshl.s16		a32, a32, #1
	vadd.s16		v6_v7, v6_v7, tmp_Q
	vsub.s16		a31, a31, v2_v3
	vsub.s16		a32, a32, v6_v7
	vadd.s16		a31, a31, DEBLOCK_H_0x0004_Q
	vadd.s16		a32, a32, DEBLOCK_H_0x0004_Q
	vshr.s16		a31, a31, #3
	vshr.s16		a32, a32, #3
	
	vabs.s16		a31, a31
	vabs.s16		a32, a32
	vmin.u16		iMina31_a32, a31, a32
	vsub.s16		dA30, absA30, iMina31_a32
	vshl.s16		tmp_Q, dA30, #2
	vadd.s16		dA30, dA30, tmp_Q
	vshr.s16		dA30, dA30, #3
	vsubl.u8		tmp_Q, v04, v05
	vmin.s16		a, c, dA30
	
	vmov.s16		tmp2_Q, #0
	vclt.s16		a31, tmp_Q, #0
	vsub.s16		tmp2_Q, tmp2_Q, a	
	

@	for ( i == 0@ i < 8@ i ++) {   
@		validFlag[i] == (c[i] != 0) ? 1 : 0@
@		validFlag[i] &== (absA30[i] < iStepSize)@
@		validFlag[i] &== (iMina31_a32[i] < absA30[i])@
@	}

	vmov.u16		v7_v8_Q, #0xFFFF
	and				r12, r3, #0x02
	and				r14, r3, #0x01
	lsr				r12, #1
	vdup.16			tmp_Q_d0, r12
	vdup.16			tmp_Q_d1, r14
	vdup.s16		iStepSize, r2
	vtst.16			validFlag, c, v7_v8_Q
	vcgt.s16		a32, iStepSize, absA30
	vcgt.s16		v7_v8_Q, absA30, iMina31_a32
	vand			validFlag, validFlag, a32
	vbit.s16		a, tmp2_Q, a31
	vand			validFlag, validFlag, v7_v8_Q

@
@	{
@		I16_WMV flag2 == validFlag[2]@
@		I16_WMV flag6 == validFlag[6]@
@		for ( i == 0@ i < 4@ i ++) {  // first segment
@			validFlag[i] &== ((uchBitField & 0x02)>>1)@
@			validFlag[i] &== flag2@ 
@			validFlag[i] &== Flag[i]@
@		}
@		for ( i == 4@ i < 8@ i ++) {  // second segment
@			validFlag[i] &== (uchBitField & 0x01)@
@			validFlag[i] &== flag6@
@			validFlag[i] &== Flag[i]@
@		}
@	}

	vmov.u16		a31, #0xFFFF
	vdup.16			tmp2_Q_d0, validFlag_d_l[2]
	vtst.16			tmp_Q, tmp_Q, a31
	vdup.16			tmp2_Q_d1, validFlag_d_h[2]
	vand			validFlag, validFlag, tmp_Q
	vand			validFlag, validFlag, tmp2_Q
@
@	// Pick out valid results and store them.
@	{
@		U8_WMV *pVtmp4 == pV5 - iPixelDistance@
@		U8_WMV *pVtmp5 == pV5@
@		for ( i == 0@ i < 8@ i ++) {   
@			if (validFlag[i]) {
@				v4[i] -== a[i]@
@				v5[i] +== a[i]@
@				pVtmp4[i] == v4[i]@
@				pVtmp5[i] == v5[i]@	
@			}
@		}
@	}

	vqmovn.s16		v07, a
	vand			validFlag, validFlag, Flag
	vadd.u8			v08, v05, v07
	vqmovn.u16		tmp2_Q_d0, validFlag
	vsub.u8			v07, v04, v07
	vbit			v04, v07, tmp2_Q_d0
	vbit			v05, v08, tmp2_Q_d0
	
	.endm	@Filter_Core_CtxA8
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA    |.text|, CODE
    WMV_LEAF_ENTRY ARMV7_g_FilterHorizontalEdgeV9
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@r0 == pV5
@r1 == iPixelDistance
@r2 == iStepSize
@r3 == uchBitField
@r4 == Is8x8Done
@r5 == pDst
@r6 == tmp	
@r12 == ((uchBitField & 0x02)>>1)	
@r14 == (uchBitField & 0x01)	

					 
	STMFD	sp!, {r4 - r6, r14}       
	FRAME_PROFILE_COUNT

@	if ( !(uchBitField & 0x03) )
@		goto DEBLOCK_H_8x4@

	ands		r4, r3, #0x03
	beq			DEBLOCK_H_8x4

@	{ // Load 8 rows src data for 8x8 lines.
@		U8_WMV *pVtmp1 == pV5 - (iPixelDistance << 2)@
@		U8_WMV *pVtmp2 == pVtmp1 + iPixelDistance@
@		U8_WMV *pVtmp3 == pVtmp2 + iPixelDistance@
@		U8_WMV *pVtmp4 == pVtmp3 + iPixelDistance@
@		U8_WMV *pVtmp5 == pVtmp4 + iPixelDistance@
@		U8_WMV *pVtmp6 == pVtmp5 + iPixelDistance@
@		U8_WMV *pVtmp7 == pVtmp6 + iPixelDistance@
@		U8_WMV *pVtmp8 == pVtmp7 + iPixelDistance@
@		
@		for ( i == 0@ i < 8@ i ++) {      
@			v1[i] == pVtmp1[i]@
@			v2[i] == pVtmp2[i]@
@			v3[i] == pVtmp3[i]@
@			v4[i] == pVtmp4[i]@
@			v5[i] == pVtmp5[i]@
@			v6[i] == pVtmp6[i]@
@			v7[i] == pVtmp7[i]@
@			v8[i] == pVtmp8[i]@
@		}
@	}
	
	sub			r6, r0, r1, lsl #2
	sub			r5, r0, r1
	vld1.64		v01, [r6], r1
	vld1.64		v02, [r6], r1
	vld1.64		v03, [r6], r1
	vld1.64		v04, [r6], r1
	vld1.64		v05, [r6], r1
	vld1.64		v06, [r6], r1
	vld1.64		v07, [r6], r1
	vld1.64		v08, [r6]

DEBLOCK_H_8x8:
	
	Filter_Core_CtxA8
	
	vst1.64			v04, [r5], r1
	vst1.64			v05, [r5]
	
DEBLOCK_H_8x4:

@	uchBitField >>== 4@
@	if ( 0 ==== (uchBitField & 0x03) )
@		return@
@
@	{ // Load 8 rows src data for 8x4 lines.
@		U8_WMV *pVtmp1 == pV5 - (iPixelDistance << 3)@
@		U8_WMV *pVtmp2 == pVtmp1 + iPixelDistance@
@		U8_WMV *pVtmp3 == pVtmp2 + iPixelDistance@
@		U8_WMV *pVtmp4 == pVtmp3 + iPixelDistance@
@		U8_WMV *pVtmp5 == pVtmp4 + iPixelDistance@
@		U8_WMV *pVtmp6 == pVtmp5 + iPixelDistance@
@		U8_WMV *pVtmp7 == pVtmp6 + iPixelDistance@
@		U8_WMV *pVtmp8 == pVtmp7 + iPixelDistance@
@
@		if ( 0 != Is8x8Done ) {
@			for ( i == 0@ i < 8@ i ++) {      
@				v5[i] == v1[i]@
@				v6[i] == v2[i]@
@				v7[i] == v3[i]@
@				v8[i] == v4[i]@
@			}
@		}
@		else {
@			for ( i == 0@ i < 8@ i ++) {      
@				v5[i] == pVtmp5[i]@
@				v6[i] == pVtmp6[i]@
@				v7[i] == pVtmp7[i]@
@				v8[i] == pVtmp8[i]@
@			}
@		}
@		for ( i == 0@ i < 8@ i ++) {      
@			v1[i] == pVtmp1[i]@
@			v2[i] == pVtmp2[i]@
@			v3[i] == pVtmp3[i]@
@			v4[i] == pVtmp4[i]@
@		}
@	}
@
@	pV5 -== (iPixelDistance << 2)@
@	goto DEBLOCK_H_8x8@

	lsr			r3, r3, #4
	ands		r6, r3, #0x03
	beq			DEBLOCK_H_end
	
	sub			r6, r0, r1, lsl #3
@	cmp			r4, #0
@	beq			DEBLOCK_H_pre8x4
@	
@	vshl.s32	v05, v01, #0
@	vshl.s32	v06, v02, #0
@	vshl.s32	v07, v03, #0
@	vshl.s32	v08, v04, #0
@	vld1.u64	v01, [r6], r1
@	vld1.u64	v02, [r6], r1
@	vld1.u64	v03, [r6], r1
@	vld1.u64	v04, [r6], r1
@	sub			r0, r0, r1, lsl #2
@	sub			r5, r0, r1
@	b			DEBLOCK_H_8x8
@
@DEBLOCK_H_pre8x4

	vld1.64		v01, [r6], r1
	vld1.64		v02, [r6], r1
	vld1.64		v03, [r6], r1
	vld1.64		v04, [r6], r1
	vld1.64		v05, [r6], r1
	vld1.64		v06, [r6], r1
	vld1.64		v07, [r6], r1
	vld1.64		v08, [r6]
	sub			r0, r0, r1, lsl #2
	sub			r5, r0, r1
	b			DEBLOCK_H_8x8
	
DEBLOCK_H_end:
	LDMFD   sp!, {r4 - r6, PC}

	WMV_ENTRY_END  @ARMV7_g_FilterHorizontalEdgeV9

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA    |.text|, CODE
    WMV_LEAF_ENTRY ARMV7_g_FilterHorizontalEdgeV9Last8x4
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@r0 == pV5
@r1 == iPixelDistance
@r2 == iStepSize
@r3 == uchBitField
@r4 == Is8x8Done
@r5 == pDst
@r6 == tmp	
@r12 == ((uchBitField & 0x02)>>1)	
@r14 == (uchBitField & 0x01)	
					 
	STMFD	sp!, {r4 - r6, r14}     
	FRAME_PROFILE_COUNT

	sub			r6, r0, r1, lsl #2
	sub			r5, r0, r1
	vld1.64		v01, [r6], r1
	vld1.64		v02, [r6], r1
	vld1.64		v03, [r6], r1
	vld1.64		v04, [r6], r1
	vld1.64		v05, [r6], r1
	vld1.64		v06, [r6], r1
	vld1.64		v07, [r6], r1
	vld1.64		v08, [r6]
	
	Filter_Core_CtxA8
	
	vst1.64			v04, [r5], r1
	vst1.64			v05, [r5]

	LDMFD   sp!, {r4 - r6, PC}

	WMV_ENTRY_END  @ARMV7_g_FilterHorizontalEdgeV9Last8x4
	
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA    |.text|, CODE
    WMV_LEAF_ENTRY ARMV7_g_FilterVerticalEdgeV9
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@r0 == pV5
@r1 == iPixelDistance
@r2 == iStepSize
@r3 == uchBitField
@r4 == Is8x8Done
@r5 == pDst
@r6 == tmp	
@r12 == ((uchBitField & 0x02)>>1)	
@r14 == (uchBitField & 0x01)	

					 
	STMFD	sp!, {r4 - r6, r14}       
	FRAME_PROFILE_COUNT

@	if ( !(uchBitField & 0x03) )
@		goto DEBLOCK_V_4x8@

	ands		r4, r3, #0x03
	beq			DEBLOCK_V_4x8

@	{ // Load 8 column src data for 8x8 lines.
@		U8_WMV *pVtmp == pV5@
@		for ( i == 0@ i < 8@ i ++) { 
@			v1[i] == pVtmp[0]@
@			v2[i] == pVtmp[1]@
@			v3[i] == pVtmp[2]@
@			v4[i] == pVtmp[3]@
@			v5[i] == pVtmp[4]@
@			v6[i] == pVtmp[5]@
@			v7[i] == pVtmp[6]@
@			v8[i] == pVtmp[7]@
@			pVtmp +== iPixelDistance@
@		}
@	}
	
	mov			r6, r0
	add			r5, r0, #3
	
	vld1.32		v01, [r6], r1
	vld1.32		v02, [r6], r1
	vld1.32		v03, [r6], r1
	vld1.32		v04, [r6], r1
	vld1.32		v05, [r6], r1
	vld1.32		v06, [r6], r1
	vld1.32		v07, [r6], r1
	vld1.32		v08, [r6]
	
@	Transpose
@v01 - 00 01 02 03 04 05 06 07      00 10 20 30 40 50 60 70
@v02 - 10       ...         17      01       ...         71
@v03 - 20       ...         27      02       ...         72
@v04 - 30       ...         37 ====>  03       ...         73
@v05 - 40       ...         47      04       ...         74
@v06 - 50       ...         57      05       ...         75
@v07 - 60       ...         67      06       ...         76
@v08 - 70 71 72 73 74 75 76 77      07 17 27 37 47 57 67 77

	vtrn.8		v01, v02
	vtrn.8		v03, v04
	vtrn.8		v05, v06
	vtrn.8		v07, v08
	vtrn.16		v01, v03
	vtrn.16		v02, v04
	vtrn.16		v05, v07
	vtrn.16		v06, v08
	vtrn.32		v01, v05
	vtrn.32		v02, v06
	vtrn.32		v03, v07
	vtrn.32		v04, v08

DEBLOCK_V_8x8:
	
	Filter_Core_CtxA8
	
	vst2.8		{ v04[0], v05[0] }, [r5], r1
	vst2.8		{ v04[1], v05[1] }, [r5], r1
	vst2.8		{ v04[2], v05[2] }, [r5], r1
	vst2.8		{ v04[3], v05[3] }, [r5], r1
	vst2.8		{ v04[4], v05[4] }, [r5], r1
	vst2.8		{ v04[5], v05[5] }, [r5], r1
	vst2.8		{ v04[6], v05[6] }, [r5], r1
	vst2.8		{ v04[7], v05[7] }, [r5]
	
DEBLOCK_V_4x8:

@	uchBitField >>== 4@
@	if ( 0 ==== (uchBitField & 0x03) )
@		return@

	lsr			r3, r3, #4
	ands		r6, r3, #0x03
	beq			DEBLOCK_V_end

@	{ // Load 8 columns src data for 4x8 columns.
@		U8_WMV *pVtmp == pV5 - 4@
@#if 0
@		for ( i == 0@ i < 8@ i ++) {      
@			v1[i] == pVtmp[0]@
@			v2[i] == pVtmp[1]@
@			v3[i] == pVtmp[2]@
@			v4[i] == pVtmp[3]@
@			v5[i] == pVtmp[4]@
@			v6[i] == pVtmp[5]@
@			v7[i] == pVtmp[6]@
@			v8[i] == pVtmp[7]@
@			pVtmp +== iPixelDistance@
@		}
@#else
@		if ( 0 != Is8x8Done ) {
@			for ( i == 0@ i < 8@ i ++) {      
@				v5[i] == v1[i]@
@				v6[i] == v2[i]@
@				v7[i] == v3[i]@
@				v8[i] == v4[i]@
@			}
@		}
@		else {
@			for ( i == 0@ i < 8@ i ++) {      
@				v5[i] == pVtmp[4]@
@				v6[i] == pVtmp[5]@
@				v7[i] == pVtmp[6]@
@				v8[i] == pVtmp[7]@
@				pVtmp +== iPixelDistance@
@			}
@		}
@		pVtmp == pV5 - 4@
@		for ( i == 0@ i < 8@ i ++) {      
@			v1[i] == pVtmp[0]@
@			v2[i] == pVtmp[1]@
@			v3[i] == pVtmp[2]@
@			v4[i] == pVtmp[3]@
@			pVtmp +== iPixelDistance@
@		}
@#endif
@	}
@	pV5 -== 4@
@	goto DEBLOCK_V_8x8@
	
	sub			r6, r0, #4
	cmp			r4, #0
	beq			DEBLOCK_V_pre4x8
	
	vshl.s32	v05, v01, #0
	vshl.s32	v06, v02, #0
	vshl.s32	v07, v03, #0
	vshl.s32	v08, v04, #0
	vld4.8		{ v01[0], v02[0], v03[0], v04[0] }, [r6], r1
	vld4.8		{ v01[1], v02[1], v03[1], v04[1] }, [r6], r1
	vld4.8		{ v01[2], v02[2], v03[2], v04[2] }, [r6], r1
	vld4.8		{ v01[3], v02[3], v03[3], v04[3] }, [r6], r1
	vld4.8		{ v01[4], v02[4], v03[4], v04[4] }, [r6], r1
	vld4.8		{ v01[5], v02[5], v03[5], v04[5] }, [r6], r1
	vld4.8		{ v01[6], v02[6], v03[6], v04[6] }, [r6], r1
	vld4.8		{ v01[7], v02[7], v03[7], v04[7] }, [r6]
	sub			r5, r0, #1
	b			DEBLOCK_V_8x8

DEBLOCK_V_pre4x8:
	
	vld1.64		v01, [r6], r1
	vld1.64		v02, [r6], r1
	vld1.64		v03, [r6], r1
	vld1.64		v04, [r6], r1
	vld1.64		v05, [r6], r1
	vld1.64		v06, [r6], r1
	vld1.64		v07, [r6], r1
	vld1.64		v08, [r6]

	vtrn.8		v01, v02
	vtrn.8		v03, v04
	vtrn.8		v05, v06
	vtrn.8		v07, v08
	vtrn.16		v01, v03
	vtrn.16		v02, v04
	vtrn.16		v05, v07
	vtrn.16		v06, v08
	vtrn.32		v01, v05
	vtrn.32		v02, v06
	vtrn.32		v03, v07
	vtrn.32		v04, v08
	
	sub			r5, r0, #1
	b			DEBLOCK_V_8x8
	
DEBLOCK_V_end:
	LDMFD   sp!, {r4 - r6, PC}

	WMV_ENTRY_END  @ARMV7_g_FilterVerticalEdgeV9


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA    |.text|, CODE 
    WMV_LEAF_ENTRY ARMV7_g_FilterVerticalEdgeV9Last4x8
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@r0 == pV5
@r1 == iPixelDistance
@r2 == iStepSize
@r3 == uchBitField
@r4 == Is8x8Done
@r5 == pDst
@r6 == tmp	
@r12 == ((uchBitField & 0x02)>>1)	
@r14 == (uchBitField & 0x01)	
					 
	STMFD	sp!, {r4 - r6, r14}     
	FRAME_PROFILE_COUNT

	mov			r6, r0
	add			r5, r0, #3
		
	vld1.64		v01, [r6], r1
	vld1.64		v02, [r6], r1
	vld1.64		v03, [r6], r1
	vld1.64		v04, [r6], r1
	vld1.64		v05, [r6], r1
	vld1.64		v06, [r6], r1
	vld1.64		v07, [r6], r1
	vld1.64		v08, [r6]

	vtrn.8		v01, v02
	vtrn.8		v03, v04
	vtrn.8		v05, v06
	vtrn.8		v07, v08
	vtrn.16		v01, v03
	vtrn.16		v02, v04
	vtrn.16		v05, v07
	vtrn.16		v06, v08
	vtrn.32		v01, v05
	vtrn.32		v02, v06
	vtrn.32		v03, v07
	vtrn.32		v04, v08
	
	Filter_Core_CtxA8
	
	vst2.8		{ v04[0], v05[0] }, [r5], r1
	vst2.8		{ v04[1], v05[1] }, [r5], r1
	vst2.8		{ v04[2], v05[2] }, [r5], r1
	vst2.8		{ v04[3], v05[3] }, [r5], r1
	vst2.8		{ v04[4], v05[4] }, [r5], r1
	vst2.8		{ v04[5], v05[5] }, [r5], r1
	vst2.8		{ v04[6], v05[6] }, [r5], r1
	vst2.8		{ v04[7], v05[7] }, [r5]

	LDMFD   sp!, {r4 - r6, PC}

	WMV_ENTRY_END  @ARMV7_g_FilterVerticalEdgeV9Last4x8
	
  
	.ENDIF @ PRO_VER
	.ENDIF @ WMV_OPT_LOOPFILTER_ARM
            
  .END
  