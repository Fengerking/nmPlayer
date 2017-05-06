    .text
    .align 4
    .globl  _g_NewVertFilterX_Only_6_ARMV7
    .globl  _g_NewVertFilterX_Only_4_ARMV7
    .globl  _g_NewHorzFilterX_Only_6_ARMV7
    .globl  _g_NewHorzFilterX_Only_4_ARMV7
    .globl  _voMemoryCpy_8x8_ARMV7
    .globl  _voMemoryCpy_16x16_ARMV7
    
_voMemoryCpy_8x8_ARMV7:
@r0 src
@r1 srcstride
@r2 dst
@r3 dststride
	stmdb		sp!, {r14}	
	vld1.u64		d0, [r0], r1
	vld1.u64		d1, [r0], r1
	vld1.u64		d2, [r0], r1
	vld1.u64		d3, [r0], r1
	vld1.u64		d4, [r0], r1
	vld1.u64		d5, [r0], r1
	vld1.u64		d6, [r0], r1
	vld1.u64		d7, [r0]
	
	vst1.u64   d0, [r2], r3 
	vst1.u64   d1, [r2], r3
	vst1.u64   d2, [r2], r3
	vst1.u64   d3, [r2], r3
	vst1.u64   d4, [r2], r3
	vst1.u64   d5, [r2], r3
	vst1.u64   d6, [r2], r3
	vst1.u64   d7, [r2]   	
	ldmia		sp!, {pc}
	
_voMemoryCpy_16x16_ARMV7:
@r0 src
@r1 srcstride
@r2 dst
@r3 dststride
	stmdb		sp!, {r14}	
	vld1.u64		{q0}, [r0], r1
	vld1.u64		{q1}, [r0], r1
	vld1.u64		{q2}, [r0], r1
	vld1.u64		{q3}, [r0], r1
	vld1.u64		{q4}, [r0], r1
	vld1.u64		{q5}, [r0], r1
	vld1.u64		{q6}, [r0], r1
	vld1.u64		{q7}, [r0], r1	
	vld1.u64		{q8}, [r0], r1
	vld1.u64		{q9}, [r0], r1
	vld1.u64		{q10}, [r0], r1
	vld1.u64		{q11}, [r0], r1
	vld1.u64		{q12}, [r0], r1
	vld1.u64		{q13}, [r0], r1
	vld1.u64		{q14}, [r0], r1
	vld1.u64		{q15}, [r0]
	
	vst1.u64		{q0}, [r2], r3
	vst1.u64		{q1}, [r2], r3
	vst1.u64		{q2}, [r2], r3
	vst1.u64		{q3}, [r2], r3
	vst1.u64		{q4}, [r2], r3
	vst1.u64		{q5}, [r2], r3
	vst1.u64		{q6}, [r2], r3
	vst1.u64		{q7}, [r2], r3	
	vst1.u64		{q8}, [r2], r3
	vst1.u64		{q9}, [r2], r3
	vst1.u64		{q10}, [r2], r3
	vst1.u64		{q11}, [r2], r3
	vst1.u64		{q12}, [r2], r3
	vst1.u64		{q13}, [r2], r3
	vst1.u64		{q14}, [r2], r3
	vst1.u64		{q15}, [r2]  	
	ldmia		sp!, {pc}

@g_NewVertFilterX_Only_C(
@			pSrc,  iSrcStride, 
@			pDst,iDstStride, 
@			6, 
@			pMainLoop->m_ui001f001fPiRndCtl,  
@			pV, 
@			2<<b1MV, 
@			pMainLoop->m_ui00ff00ff, 
@			b1MV)@
_g_NewVertFilterX_Only_6_ARMV7:
	stmdb		sp!, {r4 - r11, r14}	
	ldr			r8, [sp, #36+0]   @iShift
  	ldr			r5, [sp, #36+4]   @iRound32
  	ldr			r6, [sp, #36+8]   @pV
  	ldr			r7, [sp, #36+12]  @iNumHorzLoop  	
  	ldr			r4, [sp, #36+16]  @uiMask
  	ldr			r12,[sp, #36+20]  @b1mv 	
  	
  	ldrb		r8, [r6]          @v0
  	ldrb		r9, [r6, #1]      @v1
  	ldrb		r10, [r6, #2]     @v2
  	ldrb		r11, [r6, #3]     @v3
  	rsb			r8, r8, #0		
  	rsb			r11, r11, #0	
	vdup.8		d24, r8			@ abs(v0)
	vdup.8		d25, r9			@ v1
	vdup.8		d26, r10		@ v2
	vdup.8		d27, r11		@ abs(v3)
	vdup.32		q11, r4			@ q11 uiMask
	
	cmp			r12, #0
	bne			VFX_16_loop_start_6
		
	vld1.u8		d0, [r0], r1
	vld1.u8		d1, [r0], r1
	vld1.u8		d2, [r0], r1
	add			r7, r7, #(8-4)<<8
VFX_8_loop_6:
	vld1.u8		d3, [r0], r1
	vld1.u8		d4, [r0], r1
	vld1.u8		d5, [r0], r1
	vld1.u8		d6, [r0], r1
	
	vdup.16		q6,  r5			@ t0
	vdup.16		q7,  r5			@ t1
	vdup.16		q8,  r5			@ t2
	vdup.16		q9,  r5			@ t3
	
	vmlsl.u8	q6,  d0, d24	@v0	
	vmlsl.u8	q7,  d1, d24		
	vmlsl.u8	q8,  d2, d24		
	vmlsl.u8	q9,  d3, d24
	vshr.s32 	d0,  d4, #0
	
	vmlal.u8	q6,  d1, d25	@v1	
	vmlal.u8	q7,  d2, d25		
	vmlal.u8	q8,  d3, d25		
	vmlal.u8	q9,  d4, d25
	vshr.s32 	d1,  d5, #0
	
	vmlal.u8	q6,  d2, d26	@v2	
	vmlal.u8	q7,  d3, d26		
	vmlal.u8	q8,  d4, d26		
	vmlal.u8	q9,  d5, d26
	vshr.s32 	d2, d6, #0
	
	vmlsl.u8	q6,  d3, d27	@v3	
	vmlsl.u8	q7,  d4, d27		
	vmlsl.u8	q8,  d5, d27		
	vmlsl.u8	q9,  d6, d27
	
	vqshrun.s16  d12, q6, #6
	vqshrun.s16  d14, q7, #6
	vqshrun.s16  d16, q8, #6
	vqshrun.s16  d18, q9, #6
	
	vst1.u32	{d12}, [r2], r3	@k0
	vst1.u32	{d14}, [r2], r3	@k1
	vst1.u32	{d16}, [r2], r3	
	vst1.u32	{d18}, [r2], r3	
	
	subs		r7, r7, #4<<8
	bge			VFX_8_loop_6
	ldmia		sp!, {r4 - r11, pc}	
	
VFX_16_loop_start_6:
	@load src
	add			r8, r2, #8
	vld1.u8		{q0}, [r0], r1
	vld1.u8		{q1}, [r0], r1
	vld1.u8		{q2}, [r0], r1	
	add			r7, r7, #(16-2)<<8	
VFX_16_loop_6:
	vld1.u8		{q3}, [r0], r1
	vld1.u8		{q4}, [r0], r1

	vdup.16		q6, r5			@ + iRound32
	vdup.16		q7, r5			@
	vdup.16		q8, r5			@ 
	vdup.16		q9, r5			@ 
	
	@ - o0*abs(v0)
	vmlsl.u8	q6, d0, d24		
	vmlsl.u8	q7, d1, d24	
	vmlsl.u8	q8, d2, d24		
	vmlsl.u8	q9, d3, d24	
	vshr.s32 	q0, q2, #0
		
	@ + o1*v1
	vmlal.u8	q6, d2, d25		
	vmlal.u8	q7, d3, d25	
	vmlal.u8	q8, d4, d25		
	vmlal.u8	q9, d5, d25	
	vshr.s32 	q1, q3, #0
		
	@ + o2*v2
	vmlal.u8	q6, d4, d26		
	vmlal.u8	q7, d5, d26		
	vmlal.u8	q8, d6, d26		
	vmlal.u8	q9, d7, d26	
	vshr.s32 	q2, q4, #0
	
	@ - o3*abs(v3)
	vmlsl.u8	q6, d6, d27		
	vmlsl.u8	q7, d7, d27		
	vmlsl.u8	q8, d8, d27		
	vmlsl.u8	q9, d9, d27	
	
	vqshrun.s16  d12, q6, #6
	vqshrun.s16  d14, q7, #6
	vqshrun.s16  d16, q8, #6
	vqshrun.s16  d18, q9, #6
	
	vst1.u32	{d12}, [r2], r3	@k0
	vst1.u32	{d16}, [r2], r3	@k1
	vst1.u32	{d14}, [r8], r3	
	vst1.u32	{d18}, [r8], r3	

	subs		r7, r7, #2<<8
	bge			VFX_16_loop_6
	
	ldmia		sp!, {r4 - r11, pc}	
	
_g_NewVertFilterX_Only_4_ARMV7:
	stmdb		sp!, {r4 - r11, r14}	
	ldr			r8, [sp, #36+0]   @iShift
  	ldr			r5, [sp, #36+4]   @iRound32
  	ldr			r6, [sp, #36+8]   @pV
  	ldr			r7, [sp, #36+12]  @iNumHorzLoop  	
  	ldr			r4, [sp, #36+16]  @uiMask
  	ldr			r12,[sp, #36+20]  @b1mv 	
  	
  	ldrb		r8, [r6]          @v0
  	ldrb		r9, [r6, #1]      @v1
  	ldrb		r10, [r6, #2]     @v2
  	ldrb		r11, [r6, #3]     @v3
  	rsb			r8, r8, #0		
  	rsb			r11, r11, #0	
	vdup.8		d24, r8			@ abs(v0)
	vdup.8		d25, r9			@ v1
	vdup.8		d26, r10		@ v2
	vdup.8		d27, r11		@ abs(v3)
	vdup.32		q11, r4			@ q11 uiMask
	
	cmp			r12, #0
	bne			VFX_16_loop_start_4
		
	vld1.u8		d0, [r0], r1
	vld1.u8		d1, [r0], r1
	vld1.u8		d2, [r0], r1
	add			r7, r7, #(8-4)<<8
VFX_8_loop_4:
	vld1.u8		d3, [r0], r1
	vld1.u8		d4, [r0], r1
	vld1.u8		d5, [r0], r1
	vld1.u8		d6, [r0], r1
	
	vdup.16		q6,  r5			@ t0
	vdup.16		q7,  r5			@ t1
	vdup.16		q8,  r5			@ t2
	vdup.16		q9,  r5			@ t3
	
	vmlsl.u8	q6,  d0, d24	@v0	
	vmlsl.u8	q7,  d1, d24		
	vmlsl.u8	q8,  d2, d24		
	vmlsl.u8	q9,  d3, d24
	vshr.s32 	d0,  d4, #0
	
	vmlal.u8	q6,  d1, d25	@v1	
	vmlal.u8	q7,  d2, d25		
	vmlal.u8	q8,  d3, d25		
	vmlal.u8	q9,  d4, d25
	vshr.s32 	d1,  d5, #0
	
	vmlal.u8	q6,  d2, d26	@v2	
	vmlal.u8	q7,  d3, d26		
	vmlal.u8	q8,  d4, d26		
	vmlal.u8	q9,  d5, d26
	vshr.s32 	d2, d6, #0
	
	vmlsl.u8	q6,  d3, d27	@v3	
	vmlsl.u8	q7,  d4, d27		
	vmlsl.u8	q8,  d5, d27		
	vmlsl.u8	q9,  d6, d27
	
	vqshrun.s16  d12, q6, #4
	vqshrun.s16  d14, q7, #4
	vqshrun.s16  d16, q8, #4
	vqshrun.s16  d18, q9, #4
	
	vst1.u32	{d12}, [r2], r3	@k0
	vst1.u32	{d14}, [r2], r3	@k1
	vst1.u32	{d16}, [r2], r3	
	vst1.u32	{d18}, [r2], r3	
	
	subs		r7, r7, #4<<8
	bge			VFX_8_loop_4
	ldmia		sp!, {r4 - r11, pc}	
	
VFX_16_loop_start_4:
	@load src
	add			r8, r2, #8
	vld1.u8		{q0}, [r0], r1
	vld1.u8		{q1}, [r0], r1
	vld1.u8		{q2}, [r0], r1	
	add			r7, r7, #(16-2)<<8	
VFX_16_loop_4:
	vld1.u8		{q3}, [r0], r1
	vld1.u8		{q4}, [r0], r1

	vdup.16		q6, r5			@ + iRound32
	vdup.16		q7, r5			@
	vdup.16		q8, r5			@ 
	vdup.16		q9, r5			@ 
	
	@ - o0*abs(v0)
	vmlsl.u8	q6, d0, d24		
	vmlsl.u8	q7, d1, d24	
	vmlsl.u8	q8, d2, d24		
	vmlsl.u8	q9, d3, d24	
	vshr.s32 	q0, q2, #0
		
	@ + o1*v1
	vmlal.u8	q6, d2, d25		
	vmlal.u8	q7, d3, d25	
	vmlal.u8	q8, d4, d25		
	vmlal.u8	q9, d5, d25	
	vshr.s32 	q1, q3, #0
		
	@ + o2*v2
	vmlal.u8	q6, d4, d26		
	vmlal.u8	q7, d5, d26		
	vmlal.u8	q8, d6, d26		
	vmlal.u8	q9, d7, d26	
	vshr.s32 	q2, q4, #0
	
	@ - o3*abs(v3)
	vmlsl.u8	q6, d6, d27		
	vmlsl.u8	q7, d7, d27		
	vmlsl.u8	q8, d8, d27		
	vmlsl.u8	q9, d9, d27	
	
	vqshrun.s16  d12, q6, #4
	vqshrun.s16  d14, q7, #4
	vqshrun.s16  d16, q8, #4
	vqshrun.s16  d18, q9, #4
	
	vst1.u32	{d12}, [r2], r3	@k0
	vst1.u32	{d16}, [r2], r3	@k1
	vst1.u32	{d14}, [r8], r3	
	vst1.u32	{d18}, [r8], r3	

	subs		r7, r7, #2<<8
	bge			VFX_16_loop_4
	
	ldmia		sp!, {r4 - r11, pc}	
	


@U32_WMV g_NewHorzFilterX_Only_C(U8_WMV *pSrc, 
@                                I32_WMV iSrcStride,
@                                U8_WMV *pDst, 
@                                I32_WMV iDstStride,
@                               const I32_WMV iShift,
@                               const I32_WMV iRound2_32, 
@                               const I8_WMV * const pH, 
@                               Bool_WMV b1MV)
_g_NewHorzFilterX_Only_6_ARMV7:
	stmdb     sp!, {r4 - r11, r14}
	ldr			r4, [sp, #36+0]   @iShift
  	ldr			r5, [sp, #36+4]   @iRound32
  	ldr			r6, [sp, #36+8]   @pH  	
  	ldr			r7, [sp, #36+12]  @b1mv
  	 
  	ldrb		r8, [r6]          @v0
  	ldrb		r9, [r6, #1]      @v1
  	ldrb		r10, [r6, #2]     @v2
  	ldrb		r11, [r6, #3]     @v3
  	rsb			r8, r8, #0		
  	rsb			r11, r11, #0	
	vdup.8		d24, r8			@ abs(v0)
	vdup.8		d25, r9			@ v1
	vdup.8		d26, r10		@ v2
	vdup.8		d27, r11		@ abs(v3)
	
  	cmp			r7, #0
	bne			HFX_16_loop_Start_6 
	mov			r4,  #8
    add			r8,  r0, #8
HFX_8_loop_6:
@	load one row of src data
	vld1.u32	{d2},  [r0],  r1
	vld1.u32	d30[0], [r8], r1
	
	vld1.u32	{d12},  [r0],  r1
	vld1.u32	d31[0], [r8], r1
	
	vext.8		d3, d2, d30, #1	@ r1
	vext.8		d4, d2, d30, #2	@ r2
	vext.8		d5, d2, d30, #3	@ r3
	
	vext.8		d13, d12, d31, #1	@ r1
	vext.8		d14, d12, d31, #2	@ r2
	vext.8		d15, d12, d31, #3	@ r3
	
	vdup.16		q5, r5				@ o0
	vdup.16		q14, r5				@ o0
	
	vmlsl.u8	q5, d2, d24	@ + r0 * h0
	vmlal.u8	q5, d3, d25	@ + r1 * h1
	vmlal.u8	q5, d4, d26	@ + r2 * h2
	vmlsl.u8	q5, d5, d27	@ + r3 * h3
	
	vmlsl.u8	q14, d12, d24	@ + r0 * h0
	vmlal.u8	q14, d13, d25	@ + r1 * h1
	vmlal.u8	q14, d14, d26	@ + r2 * h2
	vmlsl.u8	q14, d15, d27	@ + r3 * h3
	
	subs		r4, r4, #2
	vqshrun.s16	d10, q5, #6
	vqshrun.s16	d11, q14, #6
	vst1.u32	{d10}, [r2], r3		
	vst1.u32	{d11}, [r2], r3						
	bne			HFX_8_loop_6
	ldmia		sp!, {r4 - r11, pc}		
	
HFX_16_loop_Start_6:
	mov			r4,  #16
    add			r8,  r0, #16
HFX_16_loop_6:
@	load one row of src data	
	vld1.u32	{q1},  [r0], r1
	vld1.u32	d30[0], [r8], r1
	vext.8		q2, q1, q15, #1	@ r1
	vext.8		q3, q1, q15, #2	@ r2
	vext.8		q4, q1, q15, #3	@ r3
	
	@ + iRound2_32@ 
	vdup.16		q5, r5				@ o0
	vdup.16		q6, r5				@ o0	
	vmlsl.u8	q5, d2, d24	@ + r0 * h0
	vmlsl.u8	q6, d3, d24	@ + r0 * h0	
	vmlal.u8	q5, d4, d25	@ + r1 * h1
	vmlal.u8	q6, d5, d25	@ + r1 * h1	
	vmlal.u8	q5, d6, d26	@ + r2 * h2
	vmlal.u8	q6, d7, d26	@ + r2 * h2	
	vmlsl.u8	q5, d8, d27	@ + r3 * h3
	vmlsl.u8	q6, d9, d27	@ + r3 * h3

	subs		r4, r4, #1
	vqshrun.s16	d10, q5, #6
	vqshrun.s16	d11, q6, #6

	vst1.u32	{q5}, [r2], r3							
	bne			HFX_16_loop_6	
	ldmia	  sp!, {r4 - r11, pc}
	
_g_NewHorzFilterX_Only_4_ARMV7:
	stmdb     sp!, {r4 - r11, r14}
	ldr			r4, [sp, #36+0]   @iShift
  	ldr			r5, [sp, #36+4]   @iRound32
  	ldr			r6, [sp, #36+8]   @pH  	
  	ldr			r7, [sp, #36+12]  @b1mv
  	 
  	ldrb		r8, [r6]          @v0
  	ldrb		r9, [r6, #1]      @v1
  	ldrb		r10, [r6, #2]     @v2
  	ldrb		r11, [r6, #3]     @v3
  	rsb			r8, r8, #0		
  	rsb			r11, r11, #0	
	vdup.8		d24, r8			@ abs(v0)
	vdup.8		d25, r9			@ v1
	vdup.8		d26, r10		@ v2
	vdup.8		d27, r11		@ abs(v3)
	
  	cmp			r7, #0
	bne			HFX_16_loop_Start_4 
	mov			r4,  #8
    add			r8,  r0, #8
HFX_8_loop_4:
@	load one row of src data
	vld1.u32	{d2},  [r0],  r1
	vld1.u32	d30[0], [r8], r1
	
	vld1.u32	{d12},  [r0],  r1
	vld1.u32	d31[0], [r8], r1
	
	vext.8		d3, d2, d30, #1	@ r1
	vext.8		d4, d2, d30, #2	@ r2
	vext.8		d5, d2, d30, #3	@ r3
	
	vext.8		d13, d12, d31, #1	@ r1
	vext.8		d14, d12, d31, #2	@ r2
	vext.8		d15, d12, d31, #3	@ r3
	
	vdup.16		q5, r5				@ o0
	vdup.16		q14, r5				@ o0
	
	vmlsl.u8	q5, d2, d24	@ + r0 * h0
	vmlal.u8	q5, d3, d25	@ + r1 * h1
	vmlal.u8	q5, d4, d26	@ + r2 * h2
	vmlsl.u8	q5, d5, d27	@ + r3 * h3
	
	vmlsl.u8	q14, d12, d24	@ + r0 * h0
	vmlal.u8	q14, d13, d25	@ + r1 * h1
	vmlal.u8	q14, d14, d26	@ + r2 * h2
	vmlsl.u8	q14, d15, d27	@ + r3 * h3
	
	subs		r4, r4, #2
	vqshrun.s16	d10, q5, #4
	vqshrun.s16	d11, q14, #4
	vst1.u32	{d10}, [r2], r3		
	vst1.u32	{d11}, [r2], r3						
	bne			HFX_8_loop_4
	ldmia		sp!, {r4 - r11, pc}		
	
HFX_16_loop_Start_4:
	mov			r4,  #16
    add			r8,  r0, #16
HFX_16_loop_4:
@	load one row of src data	
	vld1.u32	{q1},  [r0], r1
	vld1.u32	d30[0], [r8], r1
	vext.8		q2, q1, q15, #1	@ r1
	vext.8		q3, q1, q15, #2	@ r2
	vext.8		q4, q1, q15, #3	@ r3
	
	@ + iRound2_32@ 
	vdup.16		q5, r5				@ o0
	vdup.16		q6, r5				@ o0	
	vmlsl.u8	q5, d2, d24	@ + r0 * h0
	vmlsl.u8	q6, d3, d24	@ + r0 * h0	
	vmlal.u8	q5, d4, d25	@ + r1 * h1
	vmlal.u8	q6, d5, d25	@ + r1 * h1	
	vmlal.u8	q5, d6, d26	@ + r2 * h2
	vmlal.u8	q6, d7, d26	@ + r2 * h2	
	vmlsl.u8	q5, d8, d27	@ + r3 * h3
	vmlsl.u8	q6, d9, d27	@ + r3 * h3

	subs		r4, r4, #1
	vqshrun.s16	d10, q5, #4
	vqshrun.s16	d11, q6, #4

	vst1.u32	{q5}, [r2], r3							
	bne			HFX_16_loop_4	
	ldmia	  sp!, {r4 - r11, pc}
  @.end 
