@	/************************************************************************
@	*																		*
@	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
@	*																		*
@	************************************************************************/


	.section .text
	.global  UYVYorVYUYtoPlanar420_WMMX
	.type    UYVYorVYUYtoPlanar420_WMMX, function

UYVYorVYUYtoPlanar420_WMMX:
.ALIGN 4
@uyvy_to_yv12_c(uint8_t * y_dst, uint8_t * u_dst, uint8_t * v_dst,
@				int32_t y_dst_stride, int32_t uv_dst_stride,
@				uint8_t * uyvy_src, int32_t width, int32_t height)

    stmdb		sp!, {r4-r12, lr}		@ save regs used
	@ load input hieght, width, and buffers; max 4 ldr;
	ldr			r4, [sp, #40]			@ r4 = uv_dst_stride
	ldr			r5, [sp, #44]			@ r5 = uyvy_src
	ldr			r6, [sp, #48]			@ r6 = width
	ldr			r7, [sp, #52]			@ r7 = height
	
	add			r8, r5, r6, lsl #1		@ r8 = inBuf2
	add			r9, r0, r3				@ r9 = outBufY2

	mov			r10, #8								
	tmcr		wcgr0, r10				@ for shift 8
	mov			r10, #0xff
	tbcsth		wr14, r10				@ wr14 = 0x00ff00ff00ff00ff

	LUYVYorVYUYtoPlanar420_WMMXStartRow:
		mov r10, r6								@width
		LUYVYorVYUYtoPlanar420_WMMXStartCol:
			subs		r10, r10, #16				@ prepare for next 16 pixl
			@@@@@@@@@ start Fist line 16 pixels;
			wldrd		wr0, [r5], #8			@ YVYU
			wldrd		wr1, [r5], #8			@ 
			wldrd		wr2, [r5], #8			
			wldrd		wr3, [r5], #8

			wldrd		wr4, [r8], #8
			wldrd		wr5, [r8], #8
			wldrd		wr6, [r8], #8
			wldrd		wr7, [r8], #8
			
			wand		wr12, wr0, wr14			@0V0U
			wand		wr13, wr1, wr14			@0V0U
			wpackhus	wr8, wr12, wr13			@ wr8 = VUVU

			wand		wr12, wr2, wr14			@0V0U
			wand		wr13, wr3, wr14			@0V0U

			wpackhus	wr9, wr12, wr13			@ wr9 = VUVU
			wand		wr12, wr8, wr14
			wand		wr13, wr9, wr14

			wpackhus	wr10, wr12, wr13		@ wr10 = UUUU

			wsrlhg		wr8, wr8, wcgr0			@ wr8 = 0V0V
			wsrlhg		wr9, wr9, wcgr0			@ wr9 = 0V0V
			wpackhus	wr8, wr8, wr9			@ wr8 = VVVV	
			
			wsrlhg		wr0, wr0, wcgr0			@ wr0 = 0Y0Y
			wsrlhg		wr1, wr1, wcgr0			@ wr1 = 0Y0Y
			wsrlhg		wr2, wr2, wcgr0			@ wr2 = 0Y0Y
			wsrlhg		wr3, wr3, wcgr0			@ wr3 = 0Y0Y
			
			wpackhus	wr0, wr0, wr1			@ wr0 = YYYY
			wpackhus	wr2, wr2, wr3			@ wr2 = YYYY
			
			wstrd		wr0, [r0], #8			@ store Y							
			wstrd		wr2, [r0], #8

			@@@@@@@@@ start Sencode line 16 pixels;
			wand		wr0, wr4, wr14
			wand		wr1, wr5, wr14
			wpackhus	wr11, wr0, wr1			@ wr11 = VUVU
			wand		wr0, wr6, wr14
			wand		wr1, wr7, wr14
			wpackhus	wr12, wr0, wr1			@ wr12 = VUVU
			wand		wr0, wr11, wr14
			wand		wr1, wr12, wr14
			wpackhus	wr13, wr0, wr1			@ wr13 = UUUU

			wsrlhg		wr11, wr11, wcgr0		@ wr11 = 0V0V
			wsrlhg		wr12, wr12, wcgr0		@ wr12 = 0V0V
			wpackhus	wr11, wr11, wr12		@ wr11 = VVVV	

			wavg2br		wr10, wr10, wr13		@ wr10 = UUUU
			wavg2br		wr8, wr8, wr11			@ wr8 = VVVV

			wstrd		wr10, [r1], #8			@ store U
			wstrd		wr8, [r2], #8			@ store V
			
			wsrlhg		wr4, wr4, wcgr0			@ wr4 = 0Y0Y
			wsrlhg		wr5, wr5, wcgr0			@ wr5 = 0Y0Y
			wsrlhg		wr6, wr6, wcgr0			@ wr6 = 0Y0Y
			wsrlhg		wr7, wr7, wcgr0			@ wr7 = 0Y0Y
			
			wpackhus	wr4, wr4, wr5			@ wr4 = YYYY
			wpackhus	wr6, wr6, wr7			@ wr6 = YYYY
			
			wstrd		wr4, [r9], #8			@ store Y							
			wstrd		wr6, [r9], #8
			@@@@@@@@@ 16 PIXEL END
			bne			LUYVYorVYUYtoPlanar420_WMMXStartCol
			
		subs		r7, r7, #2				@ forward 2 lines
		
		@ update input
		addne		r5, r5, r6, lsl #1		@ r5 = inBuf2
		addne		r8, r5, r6, lsl #1		@ r8 = inBuf2
		@ update output
		subne		r0, r0, r6
		addne		r0, r0, r3, lsl #1		@ outbufY
		addne		r9, r0, r3				@ outBufY2
		subne		r1, r1, r6, lsr #1		@ outBufU
		addne		r1, r1, r4
		subne		r2, r2, r6, lsr #1		@ outBufV
		addne		r2, r2, r4	
		bne			LUYVYorVYUYtoPlanar420_WMMXStartRow

    ldmia		sp!, {r4-r12, pc}		@ restore and return

