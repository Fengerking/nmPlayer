
@	/************************************************************************
@	*																		*
@	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
@	*																		*
@	************************************************************************/


	.section .text
	.global  interpolate8x8_halfpel_hv_wmmx
	.type    interpolate8x8_halfpel_hv_wmmx, function

interpolate8x8_halfpel_hv_wmmx:
.ALIGN 4
	stmdb		sp!, {r4,r5,r6, lr}		@ save regs used
	ldr			r4, [sp, #16]			@ load rounding
	and			r5, r1, #7
	add			r6, r5, #1
	cmp			r6, #8
	bic			r1, r1, #7
	wldrd		wr0, [r1]				@ load 1st eight source pixels
	tmcr		wcgr1, r5
	wldrd		wr1, [r1, #8]			@ load 2nd eight source pixels
	tmcrne		wcgr2, r6
	mov			r6, #2
	add			r1, r1, r3				@ source add stride
	sub			r5, r6, r4
	wldrd		wr2, [r1]				@ load 3rd eight source pixels
	tbcsth		wr15, r5				@ rounding
	wldrd		wr3, [r1, #8]			@ load 4th eight source pixels
	tmcr		wcgr0, r6				@ for right shift 2


	walignr1	wr4, wr0, wr1
	walignr1	wr5, wr2, wr3
	walignr2ne	wr1, wr0, wr1
	walignr2ne	wr3, wr2, wr3

	wunpckelub	wr0, wr4
	wunpckehub	wr2, wr4
	wunpckelub	wr6, wr5
	wunpckehub	wr7, wr5
	wunpckelub	wr8, wr1
	wunpckehub	wr9, wr1
	wunpckelub	wr10, wr3
	wunpckehub	wr11, wr3

	waddh		wr12, wr0, wr6
	add			r1, r1, r3
	wldrd		wr0, [r1]				@ load 1st eight source pixels
	waddh		wr12, wr12, wr8
	wldrd		wr1, [r1, #8]			@ load 2nd eight source pixels
	waddh		wr12, wr12, wr10
	waddh		wr12, wr12, wr15
	wsrahg		wr12, wr12, wcgr0

	waddh		wr13, wr2, wr7
	waddh		wr13, wr13, wr9
	waddh		wr13, wr13, wr11
	waddh		wr13, wr13, wr15
	wsrahg		wr13, wr13, wcgr0	
	
	wpackhus	wr12, wr12, wr13			@ 1st 8 pixels ready
	wstrd		wr12, [r0]	

	add			r0, r0, r2
	walignr1	wr4, wr0, wr1
	walignr2ne	wr1, wr0, wr1
	wunpckelub	wr0, wr4
	wunpckehub	wr2, wr4
	wunpckelub	wr6, wr5
	wunpckehub	wr7, wr5
	wunpckelub	wr8, wr1
	wunpckehub	wr9, wr1
	wunpckelub	wr10, wr3
	wunpckehub	wr11, wr3

	waddh		wr12, wr0, wr6
	add			r1, r1, r3
	wldrd		wr0, [r1]				@ load 1st eight source pixels
	waddh		wr12, wr12, wr8
	wldrd		wr3, [r1, #8]			@ load 2nd eight source pixels
	waddh		wr12, wr12, wr10
	waddh		wr12, wr12, wr15
	wsrahg		wr12, wr12, wcgr0

	waddh		wr13, wr2, wr7
	waddh		wr13, wr13, wr9
	waddh		wr13, wr13, wr11
	waddh		wr13, wr13, wr15
	wsrahg		wr13, wr13, wcgr0	
	
	wpackhus	wr12, wr12, wr13			@ 1st 8 pixels ready
	wstrd		wr12, [r0]

	add			r0, r0, r2
	walignr1	wr5, wr0, wr3
	walignr2ne	wr3, wr0, wr3
	wunpckelub	wr0, wr4
	wunpckehub	wr2, wr4
	wunpckelub	wr6, wr5
	wunpckehub	wr7, wr5
	wunpckelub	wr8, wr1
	wunpckehub	wr9, wr1
	wunpckelub	wr10, wr3
	wunpckehub	wr11, wr3

	waddh		wr12, wr0, wr6
	add			r1, r1, r3
	wldrd		wr0, [r1]				@ load 1st eight source pixels
	waddh		wr12, wr12, wr8
	wldrd		wr1, [r1, #8]			@ load 2nd eight source pixels
	waddh		wr12, wr12, wr10
	waddh		wr12, wr12, wr15
	wsrahg		wr12, wr12, wcgr0

	waddh		wr13, wr2, wr7
	waddh		wr13, wr13, wr9
	waddh		wr13, wr13, wr11
	waddh		wr13, wr13, wr15
	wsrahg		wr13, wr13, wcgr0	
	
	wpackhus	wr12, wr12, wr13			@ 1st 8 pixels ready
	wstrd		wr12, [r0]

	add			r0, r0, r2
	walignr1	wr4, wr0, wr1
	walignr2ne	wr1, wr0, wr1
	wunpckelub	wr0, wr4
	wunpckehub	wr2, wr4
	wunpckelub	wr6, wr5
	wunpckehub	wr7, wr5
	wunpckelub	wr8, wr1
	wunpckehub	wr9, wr1
	wunpckelub	wr10, wr3
	wunpckehub	wr11, wr3

	waddh		wr12, wr0, wr6
	add			r1, r1, r3
	wldrd		wr0, [r1]				@ load 1st eight source pixels
	waddh		wr12, wr12, wr8
	wldrd		wr3, [r1, #8]			@ load 2nd eight source pixels
	waddh		wr12, wr12, wr10
	waddh		wr12, wr12, wr15
	wsrahg		wr12, wr12, wcgr0

	waddh		wr13, wr2, wr7
	waddh		wr13, wr13, wr9
	waddh		wr13, wr13, wr11
	waddh		wr13, wr13, wr15
	wsrahg		wr13, wr13, wcgr0	
	
	wpackhus	wr12, wr12, wr13			@ 1st 8 pixels ready
	wstrd		wr12, [r0]

	add			r0, r0, r2
	walignr1	wr5, wr0, wr3
	walignr2ne	wr3, wr0, wr3

	wunpckelub	wr0, wr4
	wunpckehub	wr2, wr4
	wunpckelub	wr6, wr5
	wunpckehub	wr7, wr5
	wunpckelub	wr8, wr1
	wunpckehub	wr9, wr1
	wunpckelub	wr10, wr3
	wunpckehub	wr11, wr3

	waddh		wr12, wr0, wr6
	add			r1, r1, r3
	wldrd		wr0, [r1]				@ load 1st eight source pixels
	waddh		wr12, wr12, wr8
	wldrd		wr1, [r1, #8]			@ load 2nd eight source pixels
	waddh		wr12, wr12, wr10
	waddh		wr12, wr12, wr15
	wsrahg		wr12, wr12, wcgr0

	waddh		wr13, wr2, wr7
	waddh		wr13, wr13, wr9
	waddh		wr13, wr13, wr11
	waddh		wr13, wr13, wr15
	wsrahg		wr13, wr13, wcgr0	
	
	wpackhus	wr12, wr12, wr13			@ 1st 8 pixels ready
	wstrd		wr12, [r0]

	add			r0, r0, r2
	walignr1	wr4, wr0, wr1
	walignr2ne	wr1, wr0, wr1

	wunpckelub	wr0, wr4
	wunpckehub	wr2, wr4
	wunpckelub	wr6, wr5
	wunpckehub	wr7, wr5
	wunpckelub	wr8, wr1
	wunpckehub	wr9, wr1
	wunpckelub	wr10, wr3
	wunpckehub	wr11, wr3

	waddh		wr12, wr0, wr6
	add			r1, r1, r3
	wldrd		wr0, [r1]				@ load 1st eight source pixels
	waddh		wr12, wr12, wr8
	wldrd		wr3, [r1, #8]			@ load 2nd eight source pixels
	waddh		wr12, wr12, wr10
	waddh		wr12, wr12, wr15
	wsrahg		wr12, wr12, wcgr0

	waddh		wr13, wr2, wr7
	waddh		wr13, wr13, wr9
	waddh		wr13, wr13, wr11
	waddh		wr13, wr13, wr15
	wsrahg		wr13, wr13, wcgr0	
	
	wpackhus	wr12, wr12, wr13			@ 1st 8 pixels ready
	wstrd		wr12, [r0]

	add			r0, r0, r2
	walignr1	wr5, wr0, wr3
	walignr2ne	wr3, wr0, wr3

	wunpckelub	wr0, wr4
	wunpckehub	wr2, wr4
	wunpckelub	wr6, wr5
	wunpckehub	wr7, wr5
	wunpckelub	wr8, wr1
	wunpckehub	wr9, wr1
	wunpckelub	wr10, wr3
	wunpckehub	wr11, wr3

	waddh		wr12, wr0, wr6
	add			r1, r1, r3
	waddh		wr12, wr12, wr8
	wldrd		wr0, [r1]				@ load 1st eight source pixels
	waddh		wr12, wr12, wr10
	wldrd		wr1, [r1, #8]			@ load 2nd eight source pixels
	waddh		wr12, wr12, wr15
	wsrahg		wr12, wr12, wcgr0
	waddh		wr13, wr2, wr7
	waddh		wr13, wr13, wr9
	waddh		wr13, wr13, wr11
	waddh		wr13, wr13, wr15
	wsrahg		wr13, wr13, wcgr0	
	wpackhus	wr12, wr12, wr13			@ 1st 8 pixels ready
	wstrd		wr12, [r0]

	add			r0, r0, r2
	walignr1	wr4, wr0, wr1
	walignr2ne	wr1, wr0, wr1
	wunpckelub	wr0, wr4
	wunpckehub	wr2, wr4
	wunpckelub	wr6, wr5
	wunpckehub	wr7, wr5
	wunpckelub	wr8, wr1
	wunpckehub	wr9, wr1
	wunpckelub	wr10, wr3
	wunpckehub	wr11, wr3

	waddh		wr12, wr0, wr6
	waddh		wr12, wr12, wr8
	waddh		wr12, wr12, wr10
	waddh		wr12, wr12, wr15
	wsrahg		wr12, wr12, wcgr0

	waddh		wr13, wr2, wr7
	waddh		wr13, wr13, wr9
	waddh		wr13, wr13, wr11
	waddh		wr13, wr13, wr15
	wsrahg		wr13, wr13, wcgr0	
	
	wpackhus	wr12, wr12, wr13			@ 1st 8 pixels ready
	wstrd		wr12, [r0]

    ldmia		sp!, {r4, r5,r6, pc}		@ restore and return   


	.section .text
	.global  interpolate8x8_halfpel_h_wmmx0
	.type    interpolate8x8_halfpel_h_wmmx0, function

interpolate8x8_halfpel_h_wmmx0:
.ALIGN 4
	stmdb		sp!, {r5,r6, lr}		@ save regs used
	and			r5, r1, #7
	bic			r1, r1, #7
	wldrd		wr0, [r1]				@ load 1st eight source pixels
	add			r6, r5, #1
	wldrd		wr1, [r1, #8]			@ load 2nd eight source pixels
	cmp			r6, #8					@ copare #8 if equal will not do align
	add			r1, r1, r3
	tmcr		wcgr0, r5
	wldrd		wr4, [r1]				@ load 1st eight source pixels
	tmcrne		wcgr1, r6
	wldrd		wr5, [r1, #8]			@ load 2nd eight source pixels
	walignr0	wr2, wr0, wr1
	walignr1ne	wr1, wr0, wr1
	add			r1, r1, r3
	wldrd		wr0, [r1]				@ load 1st eight source pixels
	wavg2br		wr3, wr2, wr1
	wstrd		wr3, [r0]
	add			r0, r0, r2	
	wldrd		wr1, [r1, #8]			@ load 2nd eight source pixels
	walignr0	wr6, wr4, wr5
	walignr1ne	wr5, wr4, wr5
	add			r1, r1, r3
	wldrd		wr4, [r1]				@ load 1st eight source pixels
	wavg2br		wr7, wr6, wr5
	wstrd		wr7, [r0]
	add			r0, r0, r2
	wldrd		wr5, [r1, #8]			@ load 2nd eight source pixels		
	walignr0	wr2, wr0, wr1
	walignr1ne	wr1, wr0, wr1
	add			r1, r1, r3
	wldrd		wr0, [r1]				@ load 1st eight source pixels
	wavg2br		wr3, wr2, wr1
	wstrd		wr3, [r0]
	add			r0, r0, r2	
	wldrd		wr1, [r1, #8]			@ load 2nd eight source pixels
	walignr0	wr6, wr4, wr5
	walignr1ne	wr5, wr4, wr5
	add			r1, r1, r3
	wldrd		wr4, [r1]				@ load 1st eight source pixels
	wavg2br		wr7, wr6, wr5
	wstrd		wr7, [r0]
	add			r0, r0, r2	
	wldrd		wr5, [r1, #8]			@ load 2nd eight source pixels
	walignr0	wr2, wr0, wr1
	walignr1ne	wr1, wr0, wr1
	add			r1, r1, r3
	wldrd		wr0, [r1]				@ load 1st eight source pixels
	wavg2br		wr3, wr2, wr1
	wstrd		wr3, [r0]
	add			r0, r0, r2	
	wldrd		wr1, [r1, #8]			@ load 2nd eight source pixels
	walignr0	wr6, wr4, wr5
	walignr1ne	wr5, wr4, wr5
	add			r1, r1, r3
	wldrd		wr4, [r1]				@ load 1st eight source pixels
	wavg2br		wr7, wr6, wr5
	wstrd		wr7, [r0]
	add			r0, r0, r2	
	wldrd		wr5, [r1, #8]			@ load 2nd eight source pixels
	walignr0	wr2, wr0, wr1
	walignr1ne	wr1, wr0, wr1
	wavg2br		wr3, wr2, wr1
	wstrd		wr3, [r0]
	add			r0, r0, r2	
	walignr0	wr6, wr4, wr5
	walignr1ne	wr5, wr4, wr5
	wavg2br		wr7, wr6, wr5
	wstrd		wr7, [r0]

    ldmia		sp!, {r5,r6, pc}		@ restore and return 


	.section .text
	.global  interpolate8x8_halfpel_h_wmmx1
	.type    interpolate8x8_halfpel_h_wmmx1, function

interpolate8x8_halfpel_h_wmmx1:
.ALIGN 4
	stmdb		sp!, {r5,r6, lr}		@ save regs used
	and			r5, r1, #7
	bic			r1, r1, #7
	wldrd		wr0, [r1]				@ load 1st eight source pixels
	add			r6, r5, #1
	wldrd		wr1, [r1, #8]			@ load 2nd eight source pixels
	cmp			r6, #8					@ copare #8 if equal will not do align
	add			r1, r1, r3
	tmcr		wcgr0, r5
	wldrd		wr4, [r1]				@ load 1st eight source pixels
	tmcrne		wcgr1, r6
	wldrd		wr5, [r1, #8]			@ load 2nd eight source pixels
	walignr0	wr2, wr0, wr1
	walignr1ne	wr1, wr0, wr1
	add			r1, r1, r3
	wldrd		wr0, [r1]				@ load 1st eight source pixels
	wavg2b		wr3, wr2, wr1
	wstrd		wr3, [r0]
	add			r0, r0, r2	
	wldrd		wr1, [r1, #8]			@ load 2nd eight source pixels
	walignr0	wr6, wr4, wr5
	walignr1ne	wr5, wr4, wr5
	add			r1, r1, r3
	wldrd		wr4, [r1]				@ load 1st eight source pixels
	wavg2b		wr7, wr6, wr5
	wstrd		wr7, [r0]
	add			r0, r0, r2
	wldrd		wr5, [r1, #8]			@ load 2nd eight source pixels		
	walignr0	wr2, wr0, wr1
	walignr1ne	wr1, wr0, wr1
	add			r1, r1, r3
	wldrd		wr0, [r1]				@ load 1st eight source pixels
	wavg2b		wr3, wr2, wr1
	wstrd		wr3, [r0]
	add			r0, r0, r2	
	wldrd		wr1, [r1, #8]			@ load 2nd eight source pixels
	walignr0	wr6, wr4, wr5
	walignr1ne	wr5, wr4, wr5
	add			r1, r1, r3
	wldrd		wr4, [r1]				@ load 1st eight source pixels
	wavg2b		wr7, wr6, wr5
	wstrd		wr7, [r0]
	add			r0, r0, r2	
	wldrd		wr5, [r1, #8]			@ load 2nd eight source pixels
	walignr0	wr2, wr0, wr1
	walignr1ne	wr1, wr0, wr1
	add			r1, r1, r3
	wldrd		wr0, [r1]				@ load 1st eight source pixels
	wavg2b		wr3, wr2, wr1
	wstrd		wr3, [r0]
	add			r0, r0, r2	
	wldrd		wr1, [r1, #8]			@ load 2nd eight source pixels
	walignr0	wr6, wr4, wr5
	walignr1ne	wr5, wr4, wr5
	add			r1, r1, r3
	wldrd		wr4, [r1]				@ load 1st eight source pixels
	wavg2b		wr7, wr6, wr5
	wstrd		wr7, [r0]
	add			r0, r0, r2	
	wldrd		wr5, [r1, #8]			@ load 2nd eight source pixels
	walignr0	wr2, wr0, wr1
	walignr1ne	wr1, wr0, wr1
	wavg2b		wr3, wr2, wr1
	wstrd		wr3, [r0]
	add			r0, r0, r2	
	walignr0	wr6, wr4, wr5
	walignr1ne	wr5, wr4, wr5
	wavg2b		wr7, wr6, wr5
	wstrd		wr7, [r0]
    ldmia		sp!, {r5,r6, pc}		@ restore and return
	
	
	.section .text
	.global  interpolate8x8_halfpel_v_wmmx0
	.type    interpolate8x8_halfpel_v_wmmx0, function

interpolate8x8_halfpel_v_wmmx0:
.ALIGN 4
	stmdb		sp!, {r5,r6, lr}		@ save regs used
	and			r5, r1, #7
	bic			r1, r1, #7
	wldrd		wr0, [r1]				@ load 1st eight source pixels
	tmcr		wcgr0, r5
	wldrd		wr1, [r1, #8]			@ load 2nd eight source pixels
	add			r1, r1, r3
	wldrd		wr2, [r1]
	wldrd		wr3, [r1, #8]
	add			r1, r1, r3
	walignr0	wr4, wr0, wr1
	wldrd		wr8, [r1]
	walignr0	wr5, wr2, wr3
	wldrd		wr9, [r1, #8]
	wavg2br		wr6, wr5, wr4
	wstrd		wr6, [r0]
	add			r1, r1, r3
	wldrd		wr0, [r1]
	walignr0	wr4, wr8, wr9
	wldrd		wr1, [r1, #8]
	wavg2br		wr7, wr4, wr5
	add			r0, r0, r2
	wstrd		wr7, [r0]
	add			r1, r1, r3
	wldrd		wr8, [r1]
	walignr0	wr5, wr0, wr1
	wldrd		wr9, [r1, #8]
	wavg2br		wr6, wr4, wr5
	add			r0, r0, r2
	wstrd		wr6, [r0]
	add			r1, r1, r3
	wldrd		wr0, [r1]
	walignr0	wr4, wr8, wr9
	wldrd		wr1, [r1, #8]
	wavg2br		wr7, wr4, wr5
	add			r0, r0, r2
	wstrd		wr7, [r0]
	add			r1, r1, r3
	wldrd		wr8, [r1]
	walignr0	wr5, wr0, wr1
	wldrd		wr9, [r1, #8]
	wavg2br		wr6, wr4, wr5
	add			r0, r0, r2
	wstrd		wr6, [r0]
	add			r1, r1, r3
	wldrd		wr0, [r1]
	walignr0	wr4, wr8, wr9
	wldrd		wr1, [r1, #8]
	wavg2br		wr7, wr4, wr5
	add			r0, r0, r2
	wstrd		wr7, [r0]
	add			r1, r1, r3
	wldrd		wr8, [r1]
	walignr0	wr5, wr0, wr1
	wldrd		wr9, [r1, #8]
	wavg2br		wr6, wr4, wr5
	add			r0, r0, r2
	wstrd		wr6, [r0]
	walignr0	wr4, wr8, wr9
	wavg2br		wr7, wr4, wr5
	add			r0, r0, r2
	wstrd		wr7, [r0]

    ldmia		sp!, {r5,r6, pc}		@ restore and return
	
	.section .text
	.global  interpolate8x8_halfpel_v_wmmx1
	.type    interpolate8x8_halfpel_v_wmmx1, function

interpolate8x8_halfpel_v_wmmx1:
.ALIGN 4
	stmdb		sp!, {r5,r6, lr}		@ save regs used
	and			r5, r1, #7
	bic			r1, r1, #7
	wldrd		wr0, [r1]				@ load 1st eight source pixels
	tmcr		wcgr0, r5
	wldrd		wr1, [r1, #8]			@ load 2nd eight source pixels
	add			r1, r1, r3
	wldrd		wr2, [r1]
	wldrd		wr3, [r1, #8]
	add			r1, r1, r3
	walignr0	wr4, wr0, wr1
	wldrd		wr8, [r1]
	walignr0	wr5, wr2, wr3
	wldrd		wr9, [r1, #8]
	wavg2b		wr6, wr5, wr4
	wstrd		wr6, [r0]
	add			r1, r1, r3
	wldrd		wr0, [r1]
	walignr0	wr4, wr8, wr9
	wldrd		wr1, [r1, #8]
	wavg2b		wr7, wr4, wr5
	add			r0, r0, r2
	wstrd		wr7, [r0]
	add			r1, r1, r3
	wldrd		wr8, [r1]
	walignr0	wr5, wr0, wr1
	wldrd		wr9, [r1, #8]
	wavg2b		wr6, wr4, wr5
	add			r0, r0, r2
	wstrd		wr6, [r0]
	add			r1, r1, r3
	wldrd		wr0, [r1]
	walignr0	wr4, wr8, wr9
	wldrd		wr1, [r1, #8]
	wavg2b		wr7, wr4, wr5
	add			r0, r0, r2
	wstrd		wr7, [r0]
	add			r1, r1, r3
	wldrd		wr8, [r1]
	walignr0	wr5, wr0, wr1
	wldrd		wr9, [r1, #8]
	wavg2b		wr6, wr4, wr5
	add			r0, r0, r2
	wstrd		wr6, [r0]
	add			r1, r1, r3
	wldrd		wr0, [r1]
	walignr0	wr4, wr8, wr9
	wldrd		wr1, [r1, #8]
	wavg2b		wr7, wr4, wr5
	add			r0, r0, r2
	wstrd		wr7, [r0]
	add			r1, r1, r3
	wldrd		wr8, [r1]
	walignr0	wr5, wr0, wr1
	wldrd		wr9, [r1, #8]
	wavg2b		wr6, wr4, wr5
	add			r0, r0, r2
	wstrd		wr6, [r0]
	walignr0	wr4, wr8, wr9
	wavg2b		wr7, wr4, wr5
	add			r0, r0, r2
	wstrd		wr7, [r0]

    ldmia		sp!, {r5,r6, pc}		@ restore and return
	
.if 0			
	.section .text
	.global  inplace16_interpolate_halfpel_wmmx0
	.type    inplace16_interpolate_halfpel_wmmx0, function

inplace16_interpolate_halfpel_wmmx0:
@   (i+j+k+l+3)/4 = (s+t+1)/2 - (ij&kl)&st
@   (i+j+k+l+2)/4 = (s+t+1)/2 - (ij|kl)&st
@   (i+j+k+l+1)/4 = (s+t+1)/2 - (ij&kl)|st
@   (i+j+k+l+0)/4 = (s+t+1)/2 - (ij|kl)|st
@ with  s=(i+j+1)/2, t=(k+l+1)/2, ij = i^j, kl = k^l, st = s^t.
.ALIGN 4
	stmdb		sp!, {r5-r8, lr}		@ save regs used
	ldr			r7, [sp, #20]			@ r7 = dst_stride
	ldr			r8, [sp, #24]			@ r8 = src_stride
	sub			r3, r3, #1				@ src - 1
	sub			r3, r3, r8				@ src - 1 - src_stride
	and			r5, r3, #7		
	bic			r3, r3, #7
	wldrd		wr0, [r3]				@ 1st line 1st eight source pixels
	tmcr		wcgr0, r5
	wldrd		wr1, [r3, #8]			@ lst 2nd pel
	add			r5, r5, #1	
	wldrd		wr2, [r3, #16]			@ 1st 3rd pel				
	tmcr		wcgr1, r5
	wldrd		wr3, [r3, #24]			@ 1st 4th pel
	mov			r6, #17					@ block height = 17
	mov			r7, #1
	add			r3, r3, r8				@ src + src_stride
	walignr0	wr4, wr0, wr1			@ src - 1 - src_stride = i
	wldrd		wr0, [r3]				@ 2nd line 1st eight source pixels
	walignr0	wr6, wr1, wr2
	wldrd		wr1, [r3, #8]			@ 2nd 2nd pel
	walignr0	wr8, wr2, wr3
	wldrd		wr2, [r3, #16]
	waligni		wr5, wr4, wr6, #1
	wldrd		wr3, [r3, #24]
	waligni		wr7, wr6, wr8, #1
	waligni		wr9, wr8, wr6, #1
Lheight:
	subs		r6, r6, #1
	addne		r3, r3, r8				@ src + src_stride
	walignr0	wr10, wr0, wr1			@ src - 1 = k
	walignr0	wr12, wr1, wr2
	walignr0	wr14, wr2, wr3
	waligni		wr11, wr10, wr12, #1
	waligni		wr13, wr12, wr14, #1
	waligni		wr15, wr14, wr12, #1	
	
	@ calculate v
	wavg2br		wr0, wr5, wr11			@ v:(j+l+1)/2
@	wstrd		wr0, [r1], #8			@ store v
	wstrd		wr0, [r0]
	wavg2br		wr1, wr7, wr13
@	wstrd		wr1, [r1], #8
	wstrd		wr1, [r0, #8]
	wavg2br		wr2, wr9, wr15		
@	wstrd		wr2, [r1],#8
	wstrd		wr2, [r0, #16]
	
	@ calculate h and hv
	@ 1st h and hv
	wavg2br		wr0, wr4, wr5			@ (i+j+1)/2=s
	wavg2br		wr1, wr10, wr11			@ (k+l+1)/2=t
@	wstrd		wr1, [r0], #8			@ store h
	wstrd		wr1, [r0, #24]			@ store h
	wavg2br		wr2, wr0, wr1			@ (s+t+1)/2
	wxor		wr0, wr0, wr1			@ s^t
	wxor		wr1, wr4, wr5			@ i^j
	wxor		wr3, wr10, wr11			@ k^l
	wor			wr1, wr1, wr3			@ i^j|i^j
	wand		wr1, wr1, wr0			@ (k^l|i^j)&s^t	
	tbcstb		wr3, r7
	wand		wr1, wr1, wr3
	wsubb		wr2, wr2, wr1			@ (s+t+1)/2 - (k^l|i^j)&s^t	
@	wstrd		wr2, [r2], #8			@ store hv
	wstrd		wr2, [r0, #48]			@ store hv	
	
	@ 2nd h and hv
	wavg2br		wr0, wr6, wr7			@ (i+j+1)/2=s
	wavg2br		wr1, wr12, wr13			@ (k+l+1)/2=t
@	wstrd		wr1, [r0], #8			@ store h
	wstrd		wr1, [r0, #32]			@ store h
	wavg2br		wr2, wr0, wr1			@ (s+t+1)/2
	wxor		wr0, wr0, wr1			@ s^t
	wxor		wr1, wr6, wr7			@ i^j
	wxor		wr3, wr12, wr13			@ k^l
	wor			wr1, wr1, wr3			@ i^j|k^l
	wand		wr1, wr1, wr0			@ (k^l|i^j)&s^t	
	tbcstb		wr3, r7
	wand		wr1, wr1, wr3
	wsubb		wr2, wr2, wr1			@ (s+t+1)/2 - (k^l|i^j)&s^t	
@	wstrd		wr2, [r2], #8			@ store hv	
	wstrd		wr2, [r0, #56] 			@ store hv		

	@ 3rd h and hv
	wavg2br		wr0, wr8, wr9			@ (i+j+1)/2=s
	wavg2br		wr1, wr14, wr15			@ (k+l+1)/2=t
@	wstrd		wr1, [r0], #8			@ store h
	wstrd		wr1, [r0, #40]			@ store h
	wavg2br		wr2, wr0, wr1			@ (s+t+1)/2
	wxor		wr0, wr0, wr1			@ s^t
	wxor		wr1, wr8, wr9			@ i^j
	wxor		wr3, wr14, wr15			@ k^l
	wor			wr1, wr1, wr3			@ i^j|k^l
	wand		wr1, wr1, wr0			@ (k^l|i^j)&s^t	
	wldrdne		wr0, [r3]
	tbcstb		wr3, r7
	wand		wr1, wr1, wr3
	wsubb		wr2, wr2, wr1			@ (s+t+1)/2 - (k^l|i^j)&s^t	
@	wstrd		wr2, [r2], #8			@ store hv
	wstrd		wr2, [r0, #64]			@ store hv
	
	addne		r0, r0 ,#72
	wmovne		wr4, wr10
	wldrdne		wr1, [r3, #8]
	wmovne		wr5, wr11
	wldrdne		wr2, [r3, #16]
	wmovne		wr6, wr12
	wldrdne		wr3, [r3, #24]
	wmovne		wr7, wr13
	wmovne		wr8, wr14
	wmovne		wr9, wr15
	bne			Lheight		

    ldmia		sp!, {r5-r8,pc}		@ restore and return 


	.section .text
	.global  inplace16_interpolate_halfpel_wmmx1
	.type    inplace16_interpolate_halfpel_wmmx1, function

inplace16_interpolate_halfpel_wmmx1:
@   (i+j+k+l+3)/4 = (s+t+1)/2 - (ij&kl)&st
@   (i+j+k+l+2)/4 = (s+t+1)/2 - (ij|kl)&st
@   (i+j+k+l+1)/4 = (s+t+1)/2 - (ij&kl)|st
@   (i+j+k+l+0)/4 = (s+t+1)/2 - (ij|kl)|st
@ with  s=(i+j+1)/2, t=(k+l+1)/2, ij = i^j, kl = k^l, st = s^t.
.ALIGN 4
	stmdb		sp!, {r5-r8, lr}		@ save regs used
	ldr			r7, [sp, #20]			@ r7 = dst_stride
	ldr			r8, [sp, #24]			@ r8 = src_stride
	sub			r3, r3, #1				@ src - 1
	sub			r3, r3, r8				@ src - 1 - src_stride
	and			r5, r3, #7		
	bic			r3, r3, #7
	wldrd		wr0, [r3]				@ 1st line 1st eight source pixels
	tmcr		wcgr0, r5
	wldrd		wr1, [r3, #8]			@ lst 2nd pel
	add			r5, r5, #1	
	wldrd		wr2, [r3, #16]			@ 1st 3rd pel				
	tmcr		wcgr1, r5
	wldrd		wr3, [r3, #24]			@ 1st 4th pel
	mov			r6, #17					@ block height = 17
	mov			r7, #1
	add			r3, r3, r8				@ src + src_stride
	walignr0	wr4, wr0, wr1			@ src - 1 - src_stride = i
	wldrd		wr0, [r3]				@ 2nd line 1st eight source pixels
	walignr0	wr6, wr1, wr2
	wldrd		wr1, [r3, #8]			@ 2nd 2nd pel
	walignr0	wr8, wr2, wr3
	wldrd		wr2, [r3, #16]
	waligni		wr5, wr4, wr6, #1
	wldrd		wr3, [r3, #24]
	waligni		wr7, wr6, wr8, #1
	waligni		wr9, wr8, wr6, #1

Lheight2:
	subs		r6, r6, #1
	addne		r3, r3, r8				@ src + src_stride
	walignr0	wr10, wr0, wr1			@ src - 1 = k
	walignr0	wr12, wr1, wr2
	walignr0	wr14, wr2, wr3
	waligni		wr11, wr10, wr12, #1
	waligni		wr13, wr12, wr14, #1
	waligni		wr15, wr14, wr12, #1	
	
	@ calculate v
	wavg2b		wr0, wr5, wr11			@ v:(j+l)/2
@	wstrd		wr0, [r1], #8			@ store v
	wstrd		wr0, [r0]
	wavg2b		wr1, wr7, wr13
@	wstrd		wr1, [r1], #8
	wstrd		wr1, [r0, #8]
	wavg2b		wr2, wr9, wr15		
@	wstrd		wr2, [r1],#8
	wstrd		wr2, [r0, #16]	

	@ calculate h and hv
	@ 1st h and hv
	wavg2br		wr0, wr4, wr5			@ (i+j+1)/2=s
	wavg2b		wr1, wr10, wr11			@ h=(k+l)/2
@	wstrd		wr1, [r0], #8			@ store h
	wstrd		wr1, [r0, #24]			@ store h
	wavg2br		wr1, wr10, wr11			@ (k+l+1)/2=t
	wavg2br		wr2, wr0, wr1			@ (s+t+1)/2
	wxor		wr0, wr0, wr1			@ s^t
	wxor		wr1, wr4, wr5			@ i^j
	wxor		wr3, wr10, wr11			@ k^l
	wand		wr1, wr1, wr3			@ i^j&k^l
	wor			wr1, wr1, wr0			@ (k^l&i^j)|s^t	
	tbcstb		wr3, r7
	wand		wr1, wr1, wr3
	wsubb		wr2, wr2, wr1			@ (s+t+1)/2 - (k^l&i^j)|s^t	
@	wstrd		wr2, [r2], #8			@ store hv
	wstrd		wr2, [r0, #48]			@ store hv	
	
	@ 2nd h and hv
	wavg2br		wr0, wr6, wr7			@ (i+j+1)/2=s
	wavg2b		wr1, wr12, wr13			@ h=(k+l)/2
@	wstrd		wr1, [r0], #8			@ store h
	wstrd		wr1, [r0, #32]			@ store h
	wavg2br		wr1, wr12, wr13			@ (k+l+1)/2=t
	wavg2br		wr2, wr0, wr1			@ (s+t+1)/2
	wxor		wr0, wr0, wr1			@ s^t
	wxor		wr1, wr6, wr7			@ i^j
	wxor		wr3, wr12, wr13			@ k^l
	wand		wr1, wr1, wr3			@ i^j&k^l
	wor			wr1, wr1, wr0			@ (k^l&i^j)|s^t	
	tbcstb		wr3, r7
	wand		wr1, wr1, wr3
	wsubb		wr2, wr2, wr1			@ (s+t+1)/2 - (k^l&i^j)|s^t	
@	wstrd		wr2, [r2], #8			@ store hv	
	wstrd		wr2, [r0, #56] 			@ store hv

	@ 3rd h and hv
	wavg2br		wr0, wr8, wr9			@ (i+j+1)/2=s
	wavg2b		wr1, wr14, wr15			@ h=(k+l)/2
@	wstrd		wr1, [r0], #8			@ store h
	wstrd		wr1, [r0, #40]			@ store h
	wavg2br		wr1, wr14, wr15			@ (k+l+1)/2=t
	wavg2br		wr2, wr0, wr1			@ (s+t+1)/2
	wxor		wr0, wr0, wr1			@ s^t
	wxor		wr1, wr8, wr9			@ i^j
	wxor		wr3, wr14, wr15			@ k^l
	wand		wr1, wr1, wr3			@ i^j&k^l
	wor			wr1, wr1, wr0			@ (k^l&i^j)|s^t	
	wldrdne		wr0, [r3]
	tbcstb		wr3, r7
	wand		wr1, wr1, wr3
	wsubb		wr2, wr2, wr1			@ (s+t+1)/2 - (k^l&i^j)|s^t	
@	wstrd		wr2, [r2], #8			@ store hv
	wstrd		wr2, [r0, #64]			@ store hv
	
	addne		r0, r0, #72
	wmovne		wr4, wr10
	wldrdne		wr1, [r3, #8]
	wmovne		wr5, wr11
	wldrdne		wr2, [r3, #16]
	wmovne		wr6, wr12
	wldrdne		wr3, [r3, #24]
	wmovne		wr7, wr13
	wmovne		wr8, wr14
	wmovne		wr9, wr15
	bne			Lheight2				

    ldmia		sp!, {r5-r8,pc}		@ restore and return 	
.else
	.section .text
	.global  inplace16_interpolate_halfpel_wmmx0
	.type    inplace16_interpolate_halfpel_wmmx0, function

inplace16_interpolate_halfpel_wmmx0:
@   (i+j+k+l+3)/4 = (s+t+1)/2 - (ij&kl)&st
@   (i+j+k+l+2)/4 = (s+t+1)/2 - (ij|kl)&st
@   (i+j+k+l+1)/4 = (s+t+1)/2 - (ij&kl)|st
@   (i+j+k+l+0)/4 = (s+t+1)/2 - (ij|kl)|st
@ with  s=(i+j+1)/2, t=(k+l+1)/2, ij = i^j, kl = k^l, st = s^t.
.ALIGN 4
	stmdb		sp!, {r5-r8, lr}		@ save regs used
	ldr			r7, [sp, #20]			@ r7 = dst_stride
	ldr			r8, [sp, #24]			@ r8 = src_stride
	sub			r3, r3, #1				@ src - 1
	sub			r3, r3, r8				@ src - 1 - src_stride
	and			r5, r3, #7		
	bic			r3, r3, #7
	wldrd		wr0, [r3]				@ 1st line 1st eight source pixels
	tmcr		wcgr0, r5
	wldrd		wr1, [r3, #8]			@ lst 2nd pel
	add			r5, r5, #1	
	wldrd		wr2, [r3, #16]			@ 1st 3rd pel				
	tmcr		wcgr1, r5
	wldrd		wr3, [r3, #24]			@ 1st 4th pel
	mov			r6, #17					@ block height = 17
	mov			r7, #1
	add			r3, r3, r8				@ src + src_stride
	walignr0	wr4, wr0, wr1			@ src - 1 - src_stride = i
	wldrd		wr0, [r3]				@ 2nd line 1st eight source pixels
	walignr0	wr6, wr1, wr2
	wldrd		wr1, [r3, #8]			@ 2nd 2nd pel
	walignr0	wr8, wr2, wr3
	wldrd		wr2, [r3, #16]
	waligni		wr5, wr4, wr6, #1
	wldrd		wr3, [r3, #24]
	waligni		wr7, wr6, wr8, #1
	waligni		wr9, wr8, wr6, #1
Lheight:
	subs		r6, r6, #1
	addne		r3, r3, r8				@ src + src_stride
	walignr0	wr10, wr0, wr1			@ src - 1 = k
	walignr0	wr12, wr1, wr2
	walignr0	wr14, wr2, wr3
	waligni		wr11, wr10, wr12, #1
	waligni		wr13, wr12, wr14, #1
	waligni		wr15, wr14, wr12, #1	
	
	@ calculate v
	wavg2br		wr0, wr5, wr11			@ v:(j+l+1)/2
	wstrd		wr0, [r1], #8			@ store v
	wavg2br		wr1, wr7, wr13
	wstrd		wr1, [r1], #8
	wavg2br		wr2, wr9, wr15		
	wstrd		wr2, [r1],#8
	
	@ calculate h and hv
	@ 1st h and hv
	wavg2br		wr0, wr4, wr5			@ (i+j+1)/2=s
	wavg2br		wr1, wr10, wr11			@ (k+l+1)/2=t
	wstrd		wr1, [r0], #8			@ store h
	wavg2br		wr2, wr0, wr1			@ (s+t+1)/2
	wxor		wr0, wr0, wr1			@ s^t
	wxor		wr1, wr4, wr5			@ i^j
	wxor		wr3, wr10, wr11			@ k^l
	wor			wr1, wr1, wr3			@ i^j|i^j
	wand		wr1, wr1, wr0			@ (k^l|i^j)&s^t	
	tbcstb		wr3, r7
	wand		wr1, wr1, wr3
	wsubb		wr2, wr2, wr1			@ (s+t+1)/2 - (k^l|i^j)&s^t	
	wstrd		wr2, [r2], #8			@ store hv		
	
	@ 2nd h and hv
	wavg2br		wr0, wr6, wr7			@ (i+j+1)/2=s
	wavg2br		wr1, wr12, wr13			@ (k+l+1)/2=t
	wstrd		wr1, [r0], #8			@ store h
	wavg2br		wr2, wr0, wr1			@ (s+t+1)/2
	wxor		wr0, wr0, wr1			@ s^t
	wxor		wr1, wr6, wr7			@ i^j
	wxor		wr3, wr12, wr13			@ k^l
	wor			wr1, wr1, wr3			@ i^j|k^l
	wand		wr1, wr1, wr0			@ (k^l|i^j)&s^t	
	tbcstb		wr3, r7
	wand		wr1, wr1, wr3
	wsubb		wr2, wr2, wr1			@ (s+t+1)/2 - (k^l|i^j)&s^t	
	wstrd		wr2, [r2], #8			@ store hv			

	@ 3rd h and hv
	wavg2br		wr0, wr8, wr9			@ (i+j+1)/2=s
	wavg2br		wr1, wr14, wr15			@ (k+l+1)/2=t
	wstrd		wr1, [r0], #8			@ store h
	wavg2br		wr2, wr0, wr1			@ (s+t+1)/2
	wxor		wr0, wr0, wr1			@ s^t
	wxor		wr1, wr8, wr9			@ i^j
	wxor		wr3, wr14, wr15			@ k^l
	wor			wr1, wr1, wr3			@ i^j|k^l
	wand		wr1, wr1, wr0			@ (k^l|i^j)&s^t	
	wldrdne		wr0, [r3]
	tbcstb		wr3, r7
	wand		wr1, wr1, wr3
	wsubb		wr2, wr2, wr1			@ (s+t+1)/2 - (k^l|i^j)&s^t	
	wstrd		wr2, [r2], #8			@ store hv
	
	
	wmovne		wr4, wr10
	wldrdne		wr1, [r3, #8]
	wmovne		wr5, wr11
	wldrdne		wr2, [r3, #16]
	wmovne		wr6, wr12
	wldrdne		wr3, [r3, #24]
	wmovne		wr7, wr13
	wmovne		wr8, wr14
	wmovne		wr9, wr15
	bne			Lheight		

    ldmia		sp!, {r5-r8,pc}		@ restore and return 


	.section .text
	.global  inplace16_interpolate_halfpel_wmmx1
	.type    inplace16_interpolate_halfpel_wmmx1, function

inplace16_interpolate_halfpel_wmmx1:
@   (i+j+k+l+3)/4 = (s+t+1)/2 - (ij&kl)&st
@   (i+j+k+l+2)/4 = (s+t+1)/2 - (ij|kl)&st
@   (i+j+k+l+1)/4 = (s+t+1)/2 - (ij&kl)|st
@   (i+j+k+l+0)/4 = (s+t+1)/2 - (ij|kl)|st
@ with  s=(i+j+1)/2, t=(k+l+1)/2, ij = i^j, kl = k^l, st = s^t.
.ALIGN 4
	stmdb		sp!, {r5-r8, lr}		@ save regs used
	ldr			r7, [sp, #20]			@ r7 = dst_stride
	ldr			r8, [sp, #24]			@ r8 = src_stride
	sub			r3, r3, #1				@ src - 1
	sub			r3, r3, r8				@ src - 1 - src_stride
	and			r5, r3, #7		
	bic			r3, r3, #7
	wldrd		wr0, [r3]				@ 1st line 1st eight source pixels
	tmcr		wcgr0, r5
	wldrd		wr1, [r3, #8]			@ lst 2nd pel
	add			r5, r5, #1	
	wldrd		wr2, [r3, #16]			@ 1st 3rd pel				
	tmcr		wcgr1, r5
	wldrd		wr3, [r3, #24]			@ 1st 4th pel
	mov			r6, #17					@ block height = 17
	mov			r7, #1
	add			r3, r3, r8				@ src + src_stride
	walignr0	wr4, wr0, wr1			@ src - 1 - src_stride = i
	wldrd		wr0, [r3]				@ 2nd line 1st eight source pixels
	walignr0	wr6, wr1, wr2
	wldrd		wr1, [r3, #8]			@ 2nd 2nd pel
	walignr0	wr8, wr2, wr3
	wldrd		wr2, [r3, #16]
	waligni		wr5, wr4, wr6, #1
	wldrd		wr3, [r3, #24]
	waligni		wr7, wr6, wr8, #1
	waligni		wr9, wr8, wr6, #1

Lheight2:
	subs		r6, r6, #1
	addne		r3, r3, r8				@ src + src_stride
	walignr0	wr10, wr0, wr1			@ src - 1 = k
	walignr0	wr12, wr1, wr2
	walignr0	wr14, wr2, wr3
	waligni		wr11, wr10, wr12, #1
	waligni		wr13, wr12, wr14, #1
	waligni		wr15, wr14, wr12, #1	
	
	@ calculate v
	wavg2b		wr0, wr5, wr11			@ v:(j+l)/2
	wstrd		wr0, [r1], #8			@ store v
	wavg2b		wr1, wr7, wr13
	wstrd		wr1, [r1], #8
	wavg2b		wr2, wr9, wr15		
	wstrd		wr2, [r1],#8
	
	@ calculate h and hv
	@ 1st h and hv
	wavg2br		wr0, wr4, wr5			@ (i+j+1)/2=s
	wavg2b		wr1, wr10, wr11			@ h=(k+l)/2
	wstrd		wr1, [r0], #8			@ store h
	wavg2br		wr1, wr10, wr11			@ (k+l+1)/2=t
	wavg2br		wr2, wr0, wr1			@ (s+t+1)/2
	wxor		wr0, wr0, wr1			@ s^t
	wxor		wr1, wr4, wr5			@ i^j
	wxor		wr3, wr10, wr11			@ k^l
	wand		wr1, wr1, wr3			@ i^j&k^l
	wor			wr1, wr1, wr0			@ (k^l&i^j)|s^t	
	tbcstb		wr3, r7
	wand		wr1, wr1, wr3
	wsubb		wr2, wr2, wr1			@ (s+t+1)/2 - (k^l&i^j)|s^t	
	wstrd		wr2, [r2], #8			@ store hv		
	
	@ 2nd h and hv
	wavg2br		wr0, wr6, wr7			@ (i+j+1)/2=s
	wavg2b		wr1, wr12, wr13			@ h=(k+l)/2
	wstrd		wr1, [r0], #8			@ store h
	wavg2br		wr1, wr12, wr13			@ (k+l+1)/2=t
	wavg2br		wr2, wr0, wr1			@ (s+t+1)/2
	wxor		wr0, wr0, wr1			@ s^t
	wxor		wr1, wr6, wr7			@ i^j
	wxor		wr3, wr12, wr13			@ k^l
	wand		wr1, wr1, wr3			@ i^j&k^l
	wor			wr1, wr1, wr0			@ (k^l&i^j)|s^t	
	tbcstb		wr3, r7
	wand		wr1, wr1, wr3
	wsubb		wr2, wr2, wr1			@ (s+t+1)/2 - (k^l&i^j)|s^t	
	wstrd		wr2, [r2], #8			@ store hv			

	@ 3rd h and hv
	wavg2br		wr0, wr8, wr9			@ (i+j+1)/2=s
	wavg2b		wr1, wr14, wr15			@ h=(k+l)/2
	wstrd		wr1, [r0], #8			@ store h
	wavg2br		wr1, wr14, wr15			@ (k+l+1)/2=t
	wavg2br		wr2, wr0, wr1			@ (s+t+1)/2
	wxor		wr0, wr0, wr1			@ s^t
	wxor		wr1, wr8, wr9			@ i^j
	wxor		wr3, wr14, wr15			@ k^l
	wand		wr1, wr1, wr3			@ i^j&k^l
	wor			wr1, wr1, wr0			@ (k^l&i^j)|s^t	
	wldrdne		wr0, [r3]
	tbcstb		wr3, r7
	wand		wr1, wr1, wr3
	wsubb		wr2, wr2, wr1			@ (s+t+1)/2 - (k^l&i^j)|s^t	
	wstrd		wr2, [r2], #8			@ store hv	
	
	wmovne		wr4, wr10
	wldrdne		wr1, [r3, #8]
	wmovne		wr5, wr11
	wldrdne		wr2, [r3, #16]
	wmovne		wr6, wr12
	wldrdne		wr3, [r3, #24]
	wmovne		wr7, wr13
	wmovne		wr8, wr14
	wmovne		wr9, wr15
	bne			Lheight2				

    ldmia		sp!, {r5-r8,pc}		@ restore and return 

.endif

@inplace16_interpolate_halfpel_c(uint8_t * const dst_h,
@								uint8_t * const dst_v,
@								uint8_t * const dst_hv,
@								const uint8_t * const src,
@								const int32_t dst_stride,
@								const int32_t src_stride,
@								const uint32_t rounding)

	.section .text
	.global  inplace16_interpolate_halfpel_arm
	.type    inplace16_interpolate_halfpel_arm, function

inplace16_interpolate_halfpel_arm:
@   (i+j+k+l+3)/4 = (s+t+1)/2 - (ij&kl)&st
@   (i+j+k+l+2)/4 = (s+t+1)/2 - (ij|kl)&st
@   (i+j+k+l+1)/4 = (s+t+1)/2 - (ij&kl)|st
@   (i+j+k+l+0)/4 = (s+t+1)/2 - (ij|kl)|st
@ with  s=(i+j+1)/2, t=(k+l+1)/2, ij = i^j, kl = k^l, st = s^t.
.ALIGN 4
	stmdb		sp!, {r4-r12, lr}		@ save regs used
	ldr			r5, [sp, #44]			@ r5 = src_stride
	ldr			r1, [sp, #48]			@ rounding
	sub			r3, r3, #1				@ src - 1
	sub			r6, r3, r5				@ r6 = src - src_stride - 1
	ldrb		r2, [r3], #1			@ [src-1]
	mov			r7, #1
	ldrb		r9, [r6], #1			@ [src-src_stride-1]
	sub			r1, r7, r1				@ r1 = 2 - rounding				
	ldrb		r4, [r3], #1			@ [src]
	mov			r7, #17					@ hight
	ldrb		r10, [r6], #1 			@ [src-src_stride]
@	sub			r7, r7, #1
Lheight3:
	sub			r7, r7, #1
	ldrb		r12, [r3, r5]	
	mov			r8, #17					@ width	 
Lwidth3:
	subs		r8, r8, #1


	add			r11, r2, r4				@ [src - 1] + [src]
	add			r11, r11, r1			@ [src - 1] + [src] + k
	mov			r11, r11, lsr #1		@ ([src - 1] + [src] + k)>>1
	strb		r11, [r0]				@ store h
	
	add			r11, r4, r10			@ [src] + [src - src_stride]
	add			r11, r11, r1			@ [src] + [src - src_stride] + k
	mov			r11, r11, lsr #1		@ ([src] + [src - src_stride] + k)>>1
	strb		r11, [r0, #408]			@ store v

	add			r11, r2, r4
	add			r11, r11, r9
	add			r11, r11, r10
	movne		r2, r4
	movne		r9, r10
	ldrneb		r4, [r3], #1			@ [src]
	add			r11, r11, r1
	ldrneb		r10, [r6], #1 			@ [src-src_stride]	
	add			r11, r11, #1
	mov			r11, r11, lsr #2
	strb		r11, [r0, #816]			@ store hv
	
	addne		r0, r0, #1				@ dst + 1
	bne			Lwidth3
	cmp			r7, #0
	sub			r3, r3, #18
	add			r3, r3, r5
	ldrneb		r2, [r3], #1			@ [src-1]
	sub			r6, r6, #18
	ldrneb		r4, [r3], #1			@ [src]
	add			r6, r6, r5
	ldrneb		r9, [r6], #1			@ [src-src_stride-1]
	add			r0, r0, #8
	ldrneb		r10, [r6], #1 			@ [src-src_stride]

	bne			Lheight3

    ldmia		sp!, {r4-r12,pc}		@ restore and return 

@inplace16_interpolate_halfpel_c(uint8_t * cur ,//r0
@								const uint8_t * const ref, //r1
@								const int32_t ref_stride, // r2
@								const uint32_t * sad_tmp) //r4


	.section .text
	.global  inplace16_halfpel_sad_arm
	.type    inplace16_halfpel_sad_arm, function

inplace16_halfpel_sad_arm:
@   (i+j+k+l+3)/4 = (s+t+1)/2 - (ij&kl)&st
@   (i+j+k+l+2)/4 = (s+t+1)/2 - (ij|kl)&st
@   (i+j+k+l+1)/4 = (s+t+1)/2 - (ij&kl)|st
@   (i+j+k+l+0)/4 = (s+t+1)/2 - (ij|kl)|st
@ with  s=(i+j+1)/2, t=(k+l+1)/2, ij = i^j, kl = k^l, st = s^t.
.ALIGN 4
	stmdb		sp!, {r4-r12, lr}		@ save regs used
	sub			r1, r1, #1				@ r1 = ref - 1
	sub			r5, r1, r2				@ r5 = ref - ref_stride - 1


	ldrb		lr, [r0], #1			@ cur 1
	ldrb		r6, [r5], #1			@ [ref-ref_stride-1]
	ldrb		r7, [r5], #1


	ldrb		r8, [r1], #1
	ldrb		r9, [r1], #1

	add			r6, r6, r8
	add			r8, r7, r9
	add			r8, r8, #1
	mov			r8, r8, lsr #0x1		@ r8 = v

	add			r6, r6, r8
	add			r6, r6, #1
	mov			r6, r6, lsr #0x2		@ r6 = hv


	subs		r10, lr, r8
	rsbmi		r10, r10, #0			@ r10L = v0



	and			r10, lr,#0xff00
	mov			r10, r10, lsr #8 


	



				
	ldrb		r4, [r5], #1			@ [ref-ref_stride]
	mov			r7, #17					@ hight
	ldrb		r10, [r6], #1 			@ [ref]

	ldr			r8, [r0]				@ r8 = cr
	add			r6, r6, r7
	add			r7, r4, r11				@ r7 = [ref-ref_stride] + [ref]
	add			r9, r7, r3				@ r9 = v = 	[ref-ref_stride] + [ref] + k
	mov			r9, r9, lsr #1
	
@@first line of hv and v
	
Lheightsad:
	sub			r7, r7, #1
	ldrb		r12, [r3, r5]	
	mov			r8, #17					@ width	 
Lwidthsad:
	subs		r8, r8, #1
	

	add			r11, r2, r4				@ [src - 1] + [src]
	add			r11, r11, r1			@ [src - 1] + [src] + k
	mov			r11, r11, lsr #1		@ ([src - 1] + [src] + k)>>1
	strb		r11, [r0]				@ store h
	
	add			r11, r4, r10			@ [src] + [src - src_stride]
	add			r11, r11, r1			@ [src] + [src - src_stride] + k
	mov			r11, r11, lsr #1		@ ([src] + [src - src_stride] + k)>>1
	strb		r11, [r0, #408]			@ store v

	add			r11, r2, r4
	add			r11, r11, r9
	add			r11, r11, r10
	movne		r2, r4
	movne		r9, r10
	ldrneb		r4, [r3], #1			@ [src]
	add			r11, r11, r1
	ldrneb		r10, [r6], #1 			@ [src-src_stride]	
	add			r11, r11, #1
	mov			r11, r11, lsr #2
	strb		r11, [r0, #816]			@ store hv
	
	addne		r0, r0, #1				@ dst + 1
	bne			Lwidthsad
	cmp			r7, #0
	sub			r3, r3, #18
	add			r3, r3, r5
	ldrneb		r2, [r3], #1			@ [src-1]
	sub			r6, r6, #18
	ldrneb		r4, [r3], #1			@ [src]
	add			r6, r6, r5
	ldrneb		r9, [r6], #1			@ [src-src_stride-1]
	add			r0, r0, #8
	ldrneb		r10, [r6], #1 			@ [src-src_stride]

	bne			Lheightsad

    ldmia		sp!, {r4-r12,pc}		@ restore and return 



	.section .text
	.global  halfpel_sad_wmmx00
	.type    halfpel_sad_wmmx00, function
	
halfpel_sad_wmmx00:
@   (i+j+k+l+3)/4 = (s+t+1)/2 - (ij&kl)&st
@   (i+j+k+l+2)/4 = (s+t+1)/2 - (ij|kl)&st
@   (i+j+k+l+1)/4 = (s+t+1)/2 - (ij&kl)|st
@   (i+j+k+l+0)/4 = (s+t+1)/2 - (ij|kl)|st
@ with  s=(i+j+1)/2, t=(k+l+1)/2, ij = i^j, kl = k^l, st = s^t.
.ALIGN 4
@ r0: src
@ r1: cur
@ r2: stride
@ r3: tmp_sad8
	stmdb		sp!, {r5-r8, lr}		@ save regs used
	wzero		wr0
	wzero		wr1
	wzero		wr2
	wzero		wr3

	sub			r0, r0, #1				@ src-1
	sub			r0, r0, r2				@ src-1-src_stride
	and			r5, r0, #7		
	bic			r0, r0, #7
	tmcr		wcgr0, r5
	mov			r5, #16
	tmcr		wcgr1, r5				@ right shift 16
	mov			r5, #32
	tmcr		wcgr2, r5				@ right shift 32
	mov			r5, #48
	tmcr		wcgr3, r5				@ right shift 48
	mov			r7, #1
	mov			r4, r0					@ bak for cur
	
	@@@@ 1 line
	wldrd		wr4, [r0]				@ 1st line 1st eight source pixels
	wldrd		wr5, [r0, #8]			@ lst 2nd pel
	wldrd		wr6, [r0, #16]			@ 1st 3rd pel				
	wldrd		wr7, [r0, #24]			@ 1st 4th pel

	walignr0	wr8, wr5, wr4			@ src-1-src_stride=i0
	walignr0	wr9, wr6, wr5			@ i1		bak		
	walignr0	wr10, wr7, wr6			@ i2		bak

	@@@@@@@@@@@ 1st pixels
	waligni		wr11, wr9, wr8, #1		@ src-src_stride=j0 bak for v	
	wavg2br		wr14, wr11, wr8			@ (i+j+1)/2=s bak for hv
	wxor		wr8, wr11, wr8			@ i^j=ij  wr8 no more use now

	add			r0, r0 ,r2
	wldrd		wr4, [r0]
	wldrd		wr5, [r0, #8]			@ should be reloaded later
	wldrd		wr6, [r0, #16]			@ should be reloaded later
	wldrd		wr7, [r0, #24]

	walignr0	wr4, wr5, wr4			@ src-1=k0 bak for next line
	walignr0	wr5, wr6, wr5			@ k1		bak for next line
	walignr0	wr6, wr7, wr6			@ k2		bak for next line

	waligni		wr12, wr5, wr4, #1		@ src=l0		bak for v
	wavg2br		wr7, wr12, wr4			@ (k+l+1)/2=t  bak for hv-0 and h-1
	wxor		wr13, wr12, wr4			@ k^l=kl

	wor			wr13, wr13, wr8			@ ij|kl
	wxor		wr8, wr7, wr14			@ s^t=st
	wand		wr8, wr8, wr13			@ (ij|kl)&st
	tbcstb		wr13, r7
	wand		wr13, wr13, wr8
	wavg2br		wr14, wr14, wr7			@ (s+t+1)/2
	wsubb		wr14, wr14, wr13		@ (s+t+1)/2-(ij|kl)&st bak hv-1

	wldrd		wr15, [r1]				@ cur
	wavg2br		wr11, wr11, wr12		@ v
	wzero		wr13
	wsadb		wr13, wr15, wr11		@ sad0-v0
	waddh		wr0, wr0, wr13			@ store sad v0 wr0-0

	wzero		wr13
	wsadb		wr13, wr15, wr7			@ sad8-h0
	waddh		wr1, wr1, wr13			@ store sad h0 wr3-0

	wzero		wr13
	wsadb		wr13, wr15, wr14		@ sad16-hv0
	waddh		wr2, wr2, wr13			@ store sad hv0 wr2-0

	@@@@@@@@@@@ 2nd 8 pixels
	waligni		wr11, wr10, wr9, #1		@ src-src_stride=j1 bak for v	
	wavg2br		wr12, wr11, wr10		@ (i+j+1)/2=s bak for hv
	wxor		wr9, wr11, wr9			@ i^j=ij  wr9 no more use now

	waligni		wr13, wr6, wr5, #1		@ src=l1		bak for v
	wavg2br		wr8, wr13, wr5			@ (k+l+1)/2=t  bak for hv and h
	wxor		wr15, wr13, wr5			@ k^l=kl

	wor			wr15, wr15, wr9			@ ij|kl
	wxor		wr9, wr12, wr8			@ s^t=st
	wand		wr9, wr9, wr15			@ (ij|kl)&st
	tbcstb		wr15, r7
	wand		wr9, wr9, wr15
	wavg2br		wr12, wr12, wr8			@ (s+t+1)/2
	wsubb		wr12, wr12, wr9			@ (s+t+1)/2-(ij|kl)&st bak hv 

	wavg2br		wr11, wr11, wr13		@ v

	wldrd		wr15, [r1]				@ cur-8
	wldrd		wr9, [r1, #8]			@ cur

	wzero		wr13
	wsadb		wr13, wr9, wr11			@ sad0-v0
	wslldg		wr13, wr13, wcgr1
	waddh		wr0, wr0, wr13			@ store sad v0 wr0-1

	wzero		wr13
	wsadb		wr13, wr9, wr8			@ sad8-h0
	wslldg		wr13, wr13, wcgr1
	waddh		wr1, wr1, wr13			@ store sad h0 wr1-1

	wzero		wr13
	wsadb		wr13, wr9, wr12			@ sad16-hv0
	wslldg		wr13, wr13, wcgr1
	waddh		wr2, wr2, wr13			@ store sad hv0 wr2-1
	
	waligni		wr7, wr8, wr7, #1		@ for h1
	wzero		wr13
	wsadb		wr13, wr15, wr7			@ sad12-h1
	wslldg		wr13, wr13, wcgr2
	waddh		wr1, wr1, wr13			@ store sad h1 wr1-2

	waligni		wr14, wr12, wr14, #1		@ for hv1
	wzero		wr13
	wsadb		wr13, wr15, wr14		@ sad20-hv1
	wslldg		wr13, wr13, wcgr2
	waddh		wr2, wr2, wr13			@ store sad hv1 wr2-2 

	@@@@@@@@@@@ 3rd pixels

	waligni		wr11, wr9, wr10, #1		@ src-src_stride=j2 	
	wavg2br		wr13, wr11, wr10		@ (i+j+1)/2=s bak for hv
	wxor		wr10, wr11, wr10		@ i^j=ij  wr10 no more use now

	waligni		wr14, wr5, wr6, #1		@ src=l2		
	wavg2br		wr7, wr14, wr6			@ (k+l+1)/2=t  bak for hv and h
	wxor		wr11, wr14, wr6			@ k^l=kl

	wor			wr10, wr11, wr10		@ ij|kl
	wxor		wr14, wr13, wr7			@ s^t=st
	wand		wr14, wr14, wr10		@ (ij|kl)&st
	tbcstb		wr10, r7
	wand		wr14, wr14, wr10
	wavg2br		wr11, wr7, wr13			@ (s+t+1)/2
	wsubb		wr11, wr11, wr14		@ (s+t+1)/2-(ij|kl)&st bak hv 
	
	waligni		wr7, wr7, wr8, #1		@ for h1
	wzero		wr13
	wsadb		wr13, wr9, wr7			@ sad12-h1
	wslldg		wr13, wr13, wcgr3
	waddh		wr1, wr1, wr13			@ store sad h1 wr1-2

	waligni		wr11, wr11, wr12, #1	@ for hv1
	wzero		wr13
	wsadb		wr13, wr9, wr11			@ sad20-hv1
	wslldg		wr13, wr13, wcgr3 
	waddh		wr2, wr2, wr13			@ store sad hv1 wr2-2 

	@@@@ 2 line

	add			r1, r1, r2				@ cur+cur_stride
	@@@@@@@@@@@ 1st pixels
	waligni		wr11, wr5, wr4, #1		@ src-src_stride=j0 bak for v	
	wavg2br		wr12, wr11, wr4			@ (i+j+1)/2=s bak for hv
	wxor		wr4, wr11, wr4			@ i^j=ij  wr4 no more use now

	add			r0, r0 ,r2
	wldrd		wr7, [r0]
	wldrd		wr8, [r0, #8]			@ should be reloaded later
	wldrd		wr10, [r0, #16]			@ should be reloaded later
	wldrd		wr13, [r0, #24]

	walignr0	wr7, wr8, wr7			@ src-1=k0 bak for next line
	walignr0	wr8, wr10, wr8			@ k1		bak for next line
	walignr0	wr10, wr13, wr10		@ k2		bak for next line

	waligni		wr13, wr8, wr7, #1		@ src=l0		bak for v
	
		@@@calculate sad of v1-0 and hv2-0 
	wavg2br		wr11, wr11, wr13		@ (j+l+1)/2	bak for sad v0
	wzero		wr14
	wsadb		wr14, wr11, wr15		@ sad4-v1
	wslldg		wr14, wr14, wcgr2		@
	waddh		wr1, wr1, wr14			@ store sad v1 wr1-2

	wxor		wr14, wr7, wr13			@ k^l=kl
	wor			wr14, wr14, wr4			@ ij|kl

	wavg2br		wr13, wr7, wr13			@ (k+l+1)/2=t  bak for hv-0 and next h-1

	wxor		wr4, wr12, wr13			@ s^t=st
	wand		wr4, wr4, wr14			@ (ij|kl)&st
	tbcstb		wr14, r7
	wand		wr4, wr4, wr14
	wavg2br		wr14, wr12, wr13		@ (s+t+1)/2
	wsubb		wr14, wr14, wr4			@ (s+t+1)/2-(ij|kl)&st bak next hv-1
	
	wzero		wr4
	wsadb		wr4, wr15, wr14			@ sad24 hv2
	waddh		wr3, wr3, wr4			@ store sad hv2 wr3-0
		@@@calculate sad h0-0 v0-0 hv0-0
	wldrd		wr12, [r1]				@ cur	
	wzero		wr4
	wsadb		wr4, wr12, wr11			@ sad v0
	waddh		wr0, wr0, wr4
	
	wzero		wr4
	wsadb		wr4, wr12, wr13			@ sad h0
	waddh		wr1, wr1, wr4
	
	wzero		wr4
	wsadb		wr4, wr12, wr14			@ sad hv0
	waddh		wr2, wr2, wr4	


	@@@@@@@@@@@ 2nd 8 pixels
	waligni		wr11, wr6, wr5, #1		@ src-src_stride=j1 bak for v	
	wavg2br		wr4, wr11, wr5			@ (i+j+1)/2=s bak for hv
	wxor		wr5, wr11, wr5			@ i^j=ij  wr5 no more use now

	waligni		wr12, wr10, wr8, #1		@ src=l1		bak for v
	wavg2br		wr13, wr8, wr12			@ (k+l+1)/2=t  bak for hv and h
	wxor		wr9, wr8, wr12			@ k^l=kl

	wor			wr5, wr5, wr9			@ ij|kl
	wxor		wr9, wr4, wr13			@ s^t=st
	wand		wr9, wr9, wr5			@ (ij|kl)&st
	tbcstb		wr5, r7
	wand		wr9, wr9, wr5
	wavg2br		wr4, wr4, wr13			@ (s+t+1)/2
	wsubb		wr4, wr4, wr9			@ (s+t+1)/2-(ij|kl)&st bak hv 

	wavg2br		wr11, wr11, wr13		@ v
		@@@@@calculate sad v1-1
	wldrd		wr12, [r6, #8]
	wzero		wr5
	wsadb		wr5, wr11, wr12			@ sad v1-1
	wslldg		wr5, wr5, wcgr3
	waddh		wr1, wr1, wr5			@ store sad v1-1 wr1-3
	wzero		wr5
	wsadb		wr5, wr12, wr4			@ sad hv2-1
	wslldg		wr5, wr5, wcgr1
	waddh		wr3, wr3 ,wr5			@ store sad hv2-1 wr3-1

		@@@@@calculate sad hv3-0
	waligni		wr9, wr4, wr14, #1
	wzero		wr5
	wsadb		wr5, wr9, wr15			@ sad hv3-0
	wslldg		wr5, wr5, wcgr2
	waddh		wr3, wr3, wr5			@ store sad hv3-0 wr3-2

		@@@@@@sad h0-1 hv0-1 v0-1
	wldrd		wr9, [r1, #8]
	wzero		wr5
	wsadb		wr5, wr9, wr13			@ sad h0-1
	wslldg		wr5 ,wr5, wcgr1
	waddh		wr1, wr1, wr5			@ store sad h0-1 wr1-1
	
	wzero		wr5
	wsadb		wr5, wr9, wr11			@ sadv0-1
	wslldg		wr0, wr0, wcgr1
	waddh		wr0, wr0, wr5			@ store sad v0-1 wr0-1

	wzero		wr5
	wsadb		wr5, wr9, wr4			@ sad hv0-1
	wslldg		wr5, wr5, wcgr1
	waddh		wr2, wr2, wr5			@ store sad hv0-1 wr2-1

	@@@@@@@@@@@ 3rd pixels

	waligni		wr11, wr9, wr6, #1		@ src-src_stride=j2 	
	wavg2br		wr14, wr11, wr6			@ (i+j+1)/2=s bak for hv
	wxor		wr6, wr11, wr6			@ i^j=ij  wr10 no more use now

	waligni		wr15, wr9, wr10, #1		@ src=l2		
	wavg2br		wr5, wr10, wr15			@ (k+l+1)/2=t  bak for hv and h
	wxor		wr11, wr10, wr15		@ k^l=kl

	wor			wr6, wr11, wr6			@ ij|kl
	wxor		wr11, wr5, wr14			@ s^t=st
	wand		wr6, wr11, wr6			@ (ij|kl)&st
	tbcstb		wr15, r7
	wand		wr6, wr6, wr15
	wavg2br		wr11, wr14, wr5			@ (s+t+1)/2
	wsubb		wr11, wr11, wr6			@ (s+t+1)/2-(ij|kl)&st bak hv 
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	waligni		wr5, wr5, wr13, #1		@ for h1-1
	wzero		wr6
	wsadb		wr6, wr5, wr9			@ sad12-h1-1
	wslldg		wr6, wr6, wcgr3
	waddh		wr1, wr1, wr6			@ store sad h1 wr1-2

	waligni		wr5, wr11, wr4, #1			@ for hv1
	wzero		wr6
	wsadb		wr6, wr5, wr12			@ sad hv3-1
	wslldg		wr6, wr6, wcgr3
	waddh		wr3, wr3, wr6			@ store sad hv3 wr3-3 
	
	wzero		wr6
	wsadb		wr6, wr5, wr9
	wslldg		wr6, wr6, wcgr3			@ store sad hv


		
    ldmia		sp!, {r5-r8,pc}		@ restore and return 
	
		
   .end