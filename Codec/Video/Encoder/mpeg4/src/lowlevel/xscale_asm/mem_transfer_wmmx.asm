
@	/************************************************************************
@	*																		*
@	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
@	*																		*
@	************************************************************************/


	.section .text
	.global  transfer_16to8copy_wmmx
	.type    transfer_16to8copy_wmmx, function

transfer_16to8copy_wmmx:
.ALIGN 4
	stmdb		sp!, {lr}		@ save regs used

	wldrd		wr0, [r1]
	wldrd		wr1, [r1, #8]
	add			r1, r1, #16
	wldrd		wr3, [r1]
	wpackhus	wr2, wr0, wr1
	wldrd		wr4, [r1, #8]
	add			r1, r1, #16
	wstrd		wr2, [r0]
	add			r0, r0, r2
	wldrd		wr0, [r1]
	wpackhus	wr2, wr3, wr4
	wldrd		wr1, [r1, #8]
	add			r1, r1, #16
	wstrd		wr2, [r0]
	add			r0, r0, r2
	wldrd		wr3, [r1]
	wpackhus	wr2, wr0, wr1
	wldrd		wr4, [r1, #8]
	add			r1, r1, #16
	wstrd		wr2, [r0]
	add			r0, r0, r2
	wldrd		wr0, [r1]
	wpackhus	wr2, wr3, wr4
	wldrd		wr1, [r1, #8]
	add			r1, r1, #16
	wstrd		wr2, [r0]
	add			r0, r0, r2
	wldrd		wr3, [r1]
	wpackhus	wr2, wr0, wr1
	wldrd		wr4, [r1, #8]
	add			r1, r1, #16
	wstrd		wr2, [r0]
	add			r0, r0, r2
	wldrd		wr0, [r1]
	wpackhus	wr2, wr3, wr4
	wldrd		wr1, [r1, #8]
	add			r1, r1, #16
	wstrd		wr2, [r0]
	add			r0, r0, r2
	wldrd		wr3, [r1]
	wpackhus	wr2, wr0, wr1
	wldrd		wr4, [r1, #8]
	wstrd		wr2, [r0]
	add			r0, r0, r2
	wpackhus	wr2, wr3, wr4
	wstrd		wr2, [r0]
  
    ldmia		sp!, {pc}		@ restore and return   


	.section .text
	.global  transfer_8to16sub_wmmx1
	.type    transfer_8to16sub_wmmx1, function

transfer_8to16sub_wmmx1:
.ALIGN 4
	stmdb		sp!, {r5, r6, lr}		@ save regs used
	ldr			r6, [sp, #12]			@ load ref_stride
    and			r5, r2, #7		@ r5 = 3LSBs of *pRef
	wldrd		wr0, [r1]		@ load current pixel
	bic			r2, r2, #7		@ r2 64-bit aligned 
	wldrd		wr1, [r2]		@ load 1st reference pixel
	tmcr		wcgr0,	r5		@ transfer alignment to wCGR0
	wldrd		wr2, [r2, #8]	@ load 2nd reference pixel

@	wunpckelsb	wr4, wr0
@	wunpckehub	wr5, wr0

	walignr0	wr3, wr1, wr2	@ align reference row
	wsubb		wr4, wr0, wr3	@ crrenct - reference
	wunpckelsb	wr5, wr4
	wunpckehsb	wr6, wr4
	wstrd		wr3, [r1]
	wstrd		wr5, [r0], #8
	wstrd		wr6, [r0], #8

	add			r1, r1, r3
	add			r2, r2, r6

	wldrd		wr0, [r1]
	wldrd		wr1, [r2]
	wldrd		wr2, [r2, #8]
	walignr0	wr3, wr1, wr2	@ align reference row
	wsubb		wr4, wr0, wr3	@ crrenct - reference
	wunpckelsb	wr5, wr4
	wunpckehsb	wr6, wr4
	wstrd		wr3, [r1]
	wstrd		wr5, [r0], #8
	wstrd		wr6, [r0], #8

	add			r1, r1, r3
	add			r2, r2, r6

	wldrd		wr0, [r1]
	wldrd		wr1, [r2]
	wldrd		wr2, [r2, #8]
	walignr0	wr3, wr1, wr2	@ align reference row
	wsubb		wr4, wr0, wr3	@ crrenct - reference
	wunpckelsb	wr5, wr4
	wunpckehsb	wr6, wr4
	wstrd		wr3, [r1]
	wstrd		wr5, [r0], #8
	wstrd		wr6, [r0], #8

	add			r1, r1, r3
	add			r2, r2, r6

	wldrd		wr0, [r1]
	wldrd		wr1, [r2]
	wldrd		wr2, [r2, #8]
	walignr0	wr3, wr1, wr2	@ align reference row
	wsubb		wr4, wr0, wr3	@ crrenct - reference
	wunpckelsb	wr5, wr4
	wunpckehsb	wr6, wr4
	wstrd		wr3, [r1]
	wstrd		wr5, [r0], #8
	wstrd		wr6, [r0], #8

	add			r1, r1, r3
	add			r2, r2, r6

	wldrd		wr0, [r1]
	wldrd		wr1, [r2]
	wldrd		wr2, [r2, #8]
	walignr0	wr3, wr1, wr2	@ align reference row
	wsubb		wr4, wr0, wr3	@ crrenct - reference
	wunpckelsb	wr5, wr4
	wunpckehsb	wr6, wr4
	wstrd		wr3, [r1]
	wstrd		wr5, [r0], #8
	wstrd		wr6, [r0], #8

	add			r1, r1, r3
	add			r2, r2, r6

	wldrd		wr0, [r1]
	wldrd		wr1, [r2]
	wldrd		wr2, [r2, #8]
	walignr0	wr3, wr1, wr2	@ align reference row
	wsubb		wr4, wr0, wr3	@ crrenct - reference
	wunpckelsb	wr5, wr4
	wunpckehsb	wr6, wr4
	wstrd		wr3, [r1]
	wstrd		wr5, [r0], #8
	wstrd		wr6, [r0], #8

	add			r1, r1, r3
	add			r2, r2, r6

	wldrd		wr0, [r1]
	wldrd		wr1, [r2]
	wldrd		wr2, [r2, #8]
	walignr0	wr3, wr1, wr2	@ align reference row
	wsubb		wr4, wr0, wr3	@ crrenct - reference
	wunpckelsb	wr5, wr4
	wunpckehsb	wr6, wr4
	wstrd		wr3, [r1]
	wstrd		wr5, [r0], #8
	wstrd		wr6, [r0], #8

	add			r1, r1, r3
	add			r2, r2, r6

	wldrd		wr0, [r1]
	wldrd		wr1, [r2]
	wldrd		wr2, [r2, #8]
	walignr0	wr3, wr1, wr2	@ align reference row
	wsubb		wr4, wr0, wr3	@ crrenct - reference
	wunpckelsb	wr5, wr4
	wunpckehsb	wr6, wr4
	wstrd		wr3, [r1]
	wstrd		wr5, [r0], #8
	wstrd		wr6, [r0], #8


  
    ldmia		sp!, {r5, r6, pc}		@ restore and return   

	.section .text
	.global  transfer_8to16sub_wmmx
	.type    transfer_8to16sub_wmmx, function

transfer_8to16sub_wmmx:
.ALIGN 4
	stmdb		sp!, {r4-r10, lr}		@ save regs used
	wldrd		wr0, [r1]				@ 1 cur
	add			r4, r1, r3
	wldrd		wr1, [r2], #8			@ 1 ref
	add			r5, r4, r3				
	wldrd		wr2, [r4]				@ 2 cur
	add			r6, r5, r3
	wldrd		wr3, [r2], #8			@ 2 ref
	add			r7, r6, r3
	wldrd		wr4, [r5]				@ 3 cur
	add			r8, r7, r3
	wldrd		wr5, [r2], #8			@ 3 ref
	add			r9, r8, r3
	wldrd		wr6, [r6]				@ 4 cur
	add			r10, r9, r3				
	wldrd		wr7, [r2], #8			@ 4 ref	
	wunpckelub	wr12, wr0
	wldrd		wr8, [r7]				@ 5 cur	
	wunpckehub	wr13, wr0
	wldrd		wr9, [r2], #8			@ 5 ref
	wunpckelub	wr14, wr1
	wldrd		wr10, [r8]				@ 6 cur
	wunpckehub	wr15, wr1
	wldrd		wr11, [r2], #8			@ 6 ref
	wsubh		wr12, wr12, wr14
	wldrd		wr0, [r9]				@ 7 cur
	wsubh		wr13, wr13, wr15
	wstrd		wr12, [r0], #8			@ dct 1
	wunpckelub	wr14, wr2
	wstrd		wr13, [r0], #8			@ dct 2
	wunpckehub	wr15, wr2
	wldrd		wr2, [r2], #8			@ 7 ref
	wunpckelub	wr12, wr3
	wldrd		wr13, [r10]				@ 8 cur
	wsubh		wr14, wr14, wr12
	wstrd		wr14, [r0], #8			@ dct 3
	wunpckehub	wr12, wr3
	wldrd		wr14, [r2], #8			@ 8 ref
	wsubh		wr15, wr15, wr12
	wstrd		wr15, [r0], #8			@ dct 4

	wunpckelub	wr15, wr4
	wstrd		wr1, [r1]				@ store ref 1
	wunpckehub	wr12, wr4
	wstrd		wr3, [r4]				@ store ref 2
	wunpckelub	wr1, wr5
	wunpckehub	wr4, wr5
	wsubh		wr15, wr15, wr1
	wsubh		wr12, wr12, wr4
	wstrd		wr15, [r0], #8			@ dct 5
	wunpckelub	wr1, wr6
	wstrd		wr12, [r0], #8			@ dct 6
	wunpckehub	wr4, wr6
	wunpckelub	wr3, wr7
	wsubh		wr1, wr1, wr3
	wunpckehub	wr15, wr7
	wstrd		wr1, [r0], #8			@ dct 7
	wsubh		wr4, wr4, wr15

	wunpckelub	wr12, wr8
	wstrd		wr4, [r0], #8			@ dct 8
	wunpckehub	wr1, wr8
	wstrd		wr5, [r5], #8			@ store ref 3
	wunpckelub	wr15, wr9
	wstrd		wr7, [r6], #8			@ store ref 4
	wsubh		wr12, wr12, wr15
	wunpckehub	wr8, wr9
	wstrd		wr12, [r0], #8			@ dct 9
	wsubh		wr1, wr1, wr8

	wunpckelub	wr8, wr10
	wstrd		wr1, [r0], #8			@ dct 10
	wunpckehub	wr15, wr10
	wunpckelub	wr3, wr11
	wsubh		wr8, wr8, wr3
	wunpckehub	wr5, wr11
	wstrd		wr8, [r0], #8			@ dct 11
	wsubh		wr15, wr15, wr5

	wunpckelub	wr12, wr0
	wstrd		wr15, [r0], #8			@ dct 12
	wunpckehub	wr5, wr0
	wstrd		wr9, [r7], #8			@ store ref 5
	wunpckelub	wr4, wr2
	wstrd		wr11, [r8], #8			@ store ref 6
	wsubh		wr12, wr12, wr4
	wunpckehub	wr8, wr2
	wstrd		wr12, [r0], #8			@ dct 13
	wsubh		wr5, wr5, wr8


	wunpckelub	wr6, wr13
	wstrd		wr5, [r0], #8			@ dct 14
	wunpckehub	wr7, wr13
	wstrd		wr2, [r9]				@ store ref 7
	wunpckelub	wr8, wr14
	wstrd		wr14, [r10]				@ store ref 8
	wsubh		wr6, wr6, wr8
	wunpckehub	wr9, wr14
	wstrd		wr6, [r0], #8			@ dct 15
	wsubh		wr7, wr7, wr9
	wstrd		wr7, [r0]

    ldmia		sp!, {r4-r10, pc}		@ restore and return   

	.section .text
	.global  transfer_8to16copy_wmmx
	.type    transfer_8to16copy_wmmx, function

transfer_8to16copy_wmmx:
.ALIGN 4
	stmdb			sp!, {lr}		@ save regs used
	wldrd			wr0, [r1]
	add				r1, r1, r2
	wldrd			wr3, [r1]
	add				r1, r1, r2
	wldrd			wr6, [r1]
	add				r1, r1, r2
	wldrd			wr9, [r1]
	add				r1, r1, r2

	wunpckelub		wr1, wr0
	wunpckehub		wr2, wr0
	wunpckelub		wr4, wr3
	wunpckehub		wr5, wr3
	wstrd			wr1, [r0], #8
	wstrd			wr2, [r0], #8
	wstrd			wr4, [r0], #8
	wstrd			wr5, [r0], #8

	wunpckelub		wr7, wr6
	wunpckehub		wr8, wr6
	wunpckelub		wr10, wr9
	wunpckehub		wr11, wr9
	wstrd			wr7, [r0], #8
	wstrd			wr8, [r0], #8
	wstrd			wr10, [r0], #8
	wstrd			wr11, [r0], #8

	wldrd			wr0, [r1]
	add				r1, r1, r2
	wldrd			wr3, [r1]
	add				r1, r1, r2
	wldrd			wr6, [r1]
	add				r1, r1, r2
	wldrd			wr9, [r1]

	wunpckelub		wr1, wr0
	wunpckehub		wr2, wr0
	wunpckelub		wr4, wr3
	wunpckehub		wr5, wr3
	wstrd			wr1, [r0], #8
	wstrd			wr2, [r0], #8
	wstrd			wr4, [r0], #8
	wstrd			wr5, [r0], #8

	wunpckelub		wr7, wr6
	wunpckehub		wr8, wr6
	wunpckelub		wr10, wr9
	wunpckehub		wr11, wr9
	wstrd			wr7, [r0], #8
	wstrd			wr8, [r0], #8
	wstrd			wr10, [r0], #8
	wstrd			wr11, [r0], #8

  
    ldmia		sp!, {pc}		@ restore and return   

	.section .text
	.global  memcopy_wmmx
	.type    memcopy_wmmx, function

memcopy_wmmx:
.ALIGN 4
	stmdb		sp!, {lr}		@ save regs used

CPYLOOP:
	wldrd		wr0, [r1],#8
	subs		r2, r2, #8
	wstrd		wr0, [r0],#8
	bne			CPYLOOP
  
    ldmia		sp!, {pc}		@ restore and return   


	.section .text
	.global  memcopy16x16_wmmx
	.type    memcopy16x16_wmmx, function

memcopy16x16_wmmx:
.ALIGN 4
	stmdb		sp!, {r4, lr}		@ save regs used
	mov			r4, #4
CPY16x16:
	wldrd		wr0, [r0]
	wldrd		wr1, [r0, #8]
	add			r0, r0, r2
	wldrd		wr2, [r0]
	wldrd		wr3, [r0, #8]
	add			r0, r0, r2
	wldrd		wr4, [r0]
	wldrd		wr5, [r0, #8]
	add			r0, r0, r2
	wldrd		wr6, [r0]
	wldrd		wr7, [r0, #8]
	
	wstrd		wr0, [r1], #8
	wstrd		wr1, [r1], #8
	wstrd		wr2, [r1], #8
	wstrd		wr3, [r1], #8
	add			r0, r0, r2
	wstrd		wr4, [r1], #8
	wstrd		wr5, [r1], #8
	wstrd		wr6, [r1], #8
	wstrd		wr7, [r1], #8

	subs		r4, r4, #1
	bne			CPY16x16
  
    ldmia		sp!, {r4, pc}		@ restore and return  

	.section .text
	.global  transfer8x8_copy_wmmx
	.type    transfer8x8_copy_wmmx, function

transfer8x8_copy_wmmx:
.ALIGN 4
	stmdb		sp!, {r5,lr}		@ save regs used

    and			r5, r1, #7			@ r5 = 3LSBs of *pRef
	bic			r1, r1, #7			@ r2 64-bit aligned 
	wldrd		wr0, [r1]			@ load 1st reference pixel
	tmcr		wcgr0,	r5			@ transfer alignment to wCGR0
	wldrd		wr1, [r1, #8]		@ load 2nd reference pixel
	add			r1, r1, r3
	wldrd		wr3, [r1]			@ load 1st reference pixel
	walignr0	wr2, wr0, wr1		@ align reference row
	wldrd		wr4, [r1, #8]		@ load 1st reference pixel
	add			r1, r1, r3
	wstrd		wr2, [r0]			@ current = reference
	add			r0, r0, r2
	wldrd		wr6, [r1]			@ load 1st reference pixel
	walignr0	wr5, wr3, wr4		@ align reference row
	wldrd		wr7, [r1, #8]		@ load 1st reference pixel
	add			r1, r1, r3
	wstrd		wr5, [r0]			@ current = reference
	add			r0, r0, r2
	wldrd		wr9, [r1]			@ load 1st reference pixel
	walignr0	wr8, wr6, wr7		@ align reference row
	wldrd		wr10, [r1, #8]		@ load 1st reference pixel
	add			r1, r1, r3
	wstrd		wr8, [r0]			@ current = reference	
	add			r0, r0, r2
	wldrd		wr0, [r1]			@ load 1st reference pixel
	walignr0	wr11, wr9, wr10		@ align reference row
	wldrd		wr1, [r1, #8]		@ load 2nd reference pixel
	add			r1, r1, r3
	wstrd		wr11, [r0]			@ current = reference	
	add			r0, r0, r2
	wldrd		wr3, [r1]			@ load 1st reference pixel
	walignr0	wr2, wr0, wr1		@ align reference row
	wldrd		wr4, [r1, #8]		@ load 1st reference pixel
	add			r1, r1, r3
	wstrd		wr2, [r0]			@ current = reference
	add			r0, r0, r2
	wldrd		wr6, [r1]			@ load 1st reference pixel
	walignr0	wr5, wr3, wr4		@ align reference row
	wldrd		wr7, [r1, #8]		@ load 1st reference pixel
	add			r1, r1, r3
	wstrd		wr5, [r0]			@ current = reference
	add			r0, r0, r2
	wldrd		wr9, [r1]			@ load 1st reference pixel
	walignr0	wr8, wr6, wr7		@ align reference row
	wldrd		wr10, [r1, #8]		@ load 1st reference pixel
	wstrd		wr8, [r0]			@ current = reference	
	add			r0, r0, r2
	walignr0	wr11, wr9, wr10		@ align reference row
	wstrd		wr11, [r0]			@ current = reference

  
    ldmia		sp!, {r5,pc}		@ restore and return  

	.section .text
	.global  transfer_16to8add_wmmx
	.type    transfer_16to8add_wmmx, function

transfer_16to8add_wmmx:
.ALIGN 4
	stmdb		sp!, {r5,lr}		@ save regs used
	
	wldrd		wr0, [r0]			@ 
	wldrd		wr1, [r1]
	wunpckelub	wr3, wr0
	wldrd		wr2, [r1, #8]
	wunpckehub	wr4, wr0

	waddh		wr1, wr1, wr3
	waddh		wr2, wr2, wr4
	wpackhus	wr2, wr1, wr2
	wstrd		wr2, [r0]

	add			r0, r0, r2
	add			r1, r1, #16
	wldrd		wr4, [r0]			@ 
	wldrd		wr5, [r1]
	wldrd		wr6, [r1, #8]
	wunpckelub	wr7, wr4
	wunpckehub	wr8, wr4

	waddh		wr7, wr7, wr5
	waddh		wr8, wr8, wr6
	wpackhus	wr8, wr7, wr8
	wstrd		wr8, [r0]
 
	add			r0, r0, r2
	add			r1, r1, #16
	wldrd		wr0, [r0]			@ 
	wldrd		wr1, [r1]
	wldrd		wr2, [r1, #8]
	wunpckelub	wr3, wr0
	wunpckehub	wr4, wr0

	waddh		wr1, wr1, wr3
	waddh		wr2, wr2, wr4
	wpackhus	wr2, wr1, wr2
	wstrd		wr2, [r0]
	
	add			r0, r0, r2
	add			r1, r1, #16
	wldrd		wr4, [r0]			@ 
	wldrd		wr5, [r1]
	wldrd		wr6, [r1, #8]
	wunpckelub	wr7, wr4
	wunpckehub	wr8, wr4

	waddh		wr7, wr7, wr5
	waddh		wr8, wr8, wr6
	wpackhus	wr8, wr7, wr8
	wstrd		wr8, [r0]
	
	add			r0, r0, r2
	add			r1, r1, #16
	wldrd		wr0, [r0]			@ 
	wldrd		wr1, [r1]
	wldrd		wr2, [r1, #8]
	wunpckelub	wr3, wr0
	wunpckehub	wr4, wr0

	waddh		wr1, wr1, wr3
	waddh		wr2, wr2, wr4
	wpackhus	wr2, wr1, wr2
	wstrd		wr2, [r0]
	
	add			r0, r0, r2
	add			r1, r1, #16
	wldrd		wr4, [r0]			@ 
	wldrd		wr5, [r1]
	wldrd		wr6, [r1, #8]
	wunpckelub	wr7, wr4
	wunpckehub	wr8, wr4

	waddh		wr7, wr7, wr5
	waddh		wr8, wr8, wr6
	wpackhus	wr8, wr7, wr8
	wstrd		wr8, [r0]
	
	add			r0, r0, r2
	add			r1, r1, #16
	wldrd		wr0, [r0]			@ 
	wldrd		wr1, [r1]
	wldrd		wr2, [r1, #8]
	wunpckelub	wr3, wr0
	wunpckehub	wr4, wr0

	waddh		wr1, wr1, wr3
	waddh		wr2, wr2, wr4
	wpackhus	wr2, wr1, wr2
	wstrd		wr2, [r0]
	
	add			r0, r0, r2
	add			r1, r1, #16
	wldrd		wr4, [r0]			@ 
	wldrd		wr5, [r1]
	wldrd		wr6, [r1, #8]
	wunpckelub	wr7, wr4
	wunpckehub	wr8, wr4

	waddh		wr7, wr7, wr5
	waddh		wr8, wr8, wr6
	wpackhus	wr8, wr7, wr8
	wstrd		wr8, [r0]
	 
    ldmia		sp!, {r5,pc}		@ restore and return  

	.section .text
	.global  transfer_16to8add_arm
	.type    transfer_16to8add_arm, function

transfer_16to8add_arm:
.ALIGN 4
	stmdb		sp!, {r4-r8,lr}		@ save regs used

 
    ldmia		sp!, {r4-r8,pc}		@ restore and return  

	.section .text
	.global  transfer_8to16sub_arm
	.type    transfer_8to16sub_arm, function

transfer_8to16sub_arm:
.ALIGN 4

	stmdb		sp!, {r4 - r11, lr}
	ldrb		r6, [r1]		@ cur[1]
	ldr			r4, [sp, #36]
	ldrb		r7, [r2]		@ ref[1]
	mov			r5, #8
Ltransfer_8to16sub_arm:
	ldrb		r10, [r1, #1]
	subs		r5, r5, #1	
	ldrb		r11, [r2, #1]	
	sub			r9, r6, r7		@ cur[1]-ref[1]	
	strh		r9, [r0], #2	@ dct[1] = cur[1] - ref[1]		
	mov			r8, r7
	ldrb		r6, [r1, #2]		@ cur[1]
	sub			r9, r10, r11
	strh		r9, [r0], #2
	mov			r11, r11, lsl #8
	ldrb		r7, [r2, #2]		@ ref[1]
	orr			r8, r8, r11
	ldrb		r10, [r1, #3]		@ cur[1]
	sub			r9, r6, r7		@ cur[1]-ref[1]
	ldrb		r11, [r2, #3]		@ ref[1]
	mov			r7, r7, lsl #16
	strh		r9, [r0], #2	@ dct[1] = cur[1] - ref[1]
	sub			r9, r10, r11		@ cur[1]-ref[1]
	ldrb		r6, [r1, #4]		@ cur[1]
	orr			r8, r8, r7
	ldrb		r7, [r2, #4]		@ ref[1]
	mov			r11, r11, lsl #24
	strh		r9, [r0], #2	@ dct[1] = cur[1] - ref[1]	
	orr			r8, r8, r11
	ldrb		r10, [r1, #5]	

	str			r8, [r1]

	ldrb		r11, [r2, #5]	
	sub			r9, r6, r7		@ cur[1]-ref[1]	
	strh		r9, [r0], #2	@ dct[1] = cur[1] - ref[1]		
	mov			r8, r7
	ldrb		r6, [r1, #6]		@ cur[1]
	sub			r9, r10, r11
	strh		r9, [r0], #2
	mov			r11, r11, lsl #8
	ldrb		r7, [r2, #6]		@ ref[1]
	orr			r8, r8, r11
	ldrb		r10, [r1, #7]		@ cur[1]
	sub			r9, r6, r7		@ cur[1]-ref[1]
	ldrneb		r6, [r1, r3]		@ cur[1]
	mov			r7, r7, lsl #16
	ldrb		r11, [r2, #7]		@ ref[1]
	addne		r2, r2, r4
	orr			r8, r8, r7
	strh		r9, [r0], #2	@ dct[1] = cur[1] - ref[1]
	sub			r9, r10, r11		@ cur[1]-ref[1]
	ldrneb		r7, [r2]		@ ref[1]
	strh		r9, [r0], #2	@ dct[1] = cur[1] - ref[1]	
	mov			r11, r11, lsl #24
	orr			r8, r8, r11
	str			r8, [r1, #4]
	add			r1, r1, r3

	bne       Ltransfer_8to16sub_arm

	ldmia     sp!, {r4 - r11, pc}


   .end


