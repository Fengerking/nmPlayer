
@	/************************************************************************
@	*																		*
@	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
@	*																		*
@	************************************************************************/



	.section .text
	.global  SubBlock_WMMX
	.type    SubBlock_WMMX, function

@void SubBlock(uint8_t *ref, int16_t* dct_data, uint8_t *cur, const int32_t SrcPitch)
SubBlock_WMMX:
.ALIGN 4
	stmdb		sp!, {r4,r5,r6, lr}		@ save regs used

	mov			r6, r2						@ back cur
	wldrd		wr2, [r2]					@ cur 0
	add			r2, r2, r3
	and			r5, r0, #7
	wldrd		wr5, [r2]					@ cur 1
	add			r2, r2, r3
	bic			r0, r0, #7
	wldrd		wr0, [r0]					@ ref 0
	tmcr		wcgr0, r5
	wldrd		wr1, [r0, #8]				@ ref 0
	add			r0, r0, r3					@ ref + stride	
	wldrd		wr6, [r0]					@ ref 1			
	walignr0	wr0, wr0, wr1				@ ref0_a
	wldrd		wr7, [r0, #8]				@ ref 1
	add			r0, r0, r3					@ ref + stride
	wldrd		wr8, [r2]					@ cur 2
	add			r2, r2, r3
	wunpckelub	wr3, wr2					@ cur 0-3
	wldrd		wr9, [r2]					@ cur 3
	add			r2, r2, r3
	wunpckehub	wr2, wr2					@ cur 4-7
	wldrd		wr10, [r0]					@ ref 2
	wunpckelub	wr1, wr0					@ ref 0-3
	wldrd		wr11, [r0, #8]				@ ref 2
	add			r0, r0, r3					@ ref + stride
	wldrd		wr12, [r0]					@ ref 3
	wunpckehub	wr4, wr0					@ ref 4-7
	wldrd		wr13, [r0, #8]				@ ref 3
	wsubh		wr3, wr3, wr1				@ cur - ref 0-3
	wsubh		wr2, wr2, wr4				@ cur - ref 4-7
	add			r0, r0, r3					@ ref + stride
	wstrd		wr3, [r1], #8
	wunpckelub	wr1, wr5					@ cur 0-3
	wstrd		wr2, [r1], #8
	wunpckehub	wr5, wr5					@ cur 4-7
	walignr0	wr6, wr6, wr7				@ ref1_a
	wunpckelub	wr4, wr6
	wunpckehub	wr2, wr6
	wsubh		wr1, wr1, wr4
	wsubh		wr5, wr5, wr2
	walignr0	wr10, wr10, wr11			@ ref2_a
	wstrd		wr1, [r1], #8
	wunpckelub	wr4, wr8					@ cur 2
	wstrd		wr5, [r1], #8
	wunpckehub	wr8, wr8

	wunpckelub	wr1, wr10					@ cur 2
	wunpckehub	wr5, wr10
	wsubh		wr1, wr4, wr1
	wsubh		wr5, wr8, wr5
	walignr0	wr12, wr12, wr13			@ ref3_a
	wstrd		wr1, [r1], #8
	wunpckelub	wr8, wr12					@ 
	wstrd		wr5, [r1], #8
	wunpckehub	wr1, wr12					@
	wunpckelub	wr4, wr9
	wunpckehub	wr9, wr9
	wsubh		wr4, wr4, wr8
	wsubh		wr9, wr9, wr1
	wstrd		wr4, [r1], #8
	wstrd		wr9, [r1], #8

	wstrd		wr0, [r6]
	add			r6, r6, r3
	wstrd		wr6, [r6]
	add			r6, r6, r3
	wstrd		wr10, [r6]
	add			r6, r6, r3
	wstrd		wr12, [r6]
	add			r6, r6, r3

	wldrd		wr2, [r2]					@ cur 0
	add			r2, r2, r3
	wldrd		wr5, [r2]					@ cur 1
	add			r2, r2, r3
	wldrd		wr0, [r0]					@ ref 0
	tmcr		wcgr0, r5
	wldrd		wr1, [r0, #8]				@ ref 0
	add			r0, r0, r3					@ ref + stride	
	wldrd		wr6, [r0]					@ ref 1			
	walignr0	wr0, wr0, wr1				@ ref0_a
	wldrd		wr7, [r0, #8]				@ ref 1
	add			r0, r0, r3					@ ref + stride
	wldrd		wr8, [r2]					@ cur 2
	add			r2, r2, r3
	wunpckelub	wr3, wr2					@ cur 0-3
	wldrd		wr9, [r2]					@ cur 3
	add			r2, r2, r3
	pld			[r2]
	wunpckehub	wr2, wr2					@ cur 4-7
	wldrd		wr10, [r0]					@ ref 2
	wunpckelub	wr1, wr0					@ ref 0-3
	wldrd		wr11, [r0, #8]				@ ref 2
	add			r0, r0, r3					@ ref + stride
	wldrd		wr12, [r0]					@ ref 3
	wunpckehub	wr4, wr0					@ ref 4-7
	wldrd		wr13, [r0, #8]				@ ref 3
	wsubh		wr3, wr3, wr1				@ cur - ref 0-3
	wsubh		wr2, wr2, wr4				@ cur - ref 4-7
	add			r0, r0, r3					@ ref + stride
	wstrd		wr3, [r1], #8
	wunpckelub	wr1, wr5					@ cur 0-3
	wstrd		wr2, [r1], #8
	wunpckehub	wr5, wr5					@ cur 4-7
	pld			[r0]
	walignr0	wr6, wr6, wr7				@ ref1_a
	wunpckelub	wr4, wr6
	wunpckehub	wr2, wr6
	wsubh		wr1, wr1, wr4
	wsubh		wr5, wr5, wr2
	walignr0	wr10, wr10, wr11			@ ref2_a
	wstrd		wr1, [r1], #8
	wunpckelub	wr4, wr8					@ cur 2
	wstrd		wr5, [r1], #8
	wunpckehub	wr8, wr8

	wunpckelub	wr1, wr10					@ cur 2
	wunpckehub	wr5, wr10
	wsubh		wr1, wr4, wr1
	wsubh		wr5, wr8, wr5
	walignr0	wr12, wr12, wr13			@ ref3_a
	wstrd		wr1, [r1], #8
	wunpckelub	wr8, wr12					@ 
	wstrd		wr5, [r1], #8
	wunpckehub	wr1, wr12					@
	wunpckelub	wr4, wr9
	wunpckehub	wr9, wr9
	wsubh		wr4, wr4, wr8
	wsubh		wr9, wr9, wr1
	wstrd		wr4, [r1], #8
	wstrd		wr9, [r1], #8

	wstrd		wr0, [r6]
	add			r6, r6, r3
	wstrd		wr6, [r6]
	add			r6, r6, r3
	wstrd		wr10, [r6]
	add			r6, r6, r3
	wstrd		wr12, [r6]
	add			r6, r6, r3


    ldmia		sp!, {r4, r5,r6, pc}		@ restore and return   

@void SubBlock(uint8_t *ref, int16_t* dct_data, uint8_t *cur, const int32_t SrcPitch)
@   (i+j+k+l+3)/4 = (s+t+1)/2 - (ij&kl)&st
@   (i+j+k+l+2)/4 = (s+t+1)/2 - (ij|kl)&st rounding == 0
@   (i+j+k+l+1)/4 = (s+t+1)/2 - (ij&kl)|st rounding == 1
@   (i+j+k+l+0)/4 = (s+t+1)/2 - (ij|kl)|st
@ with  s=(i+j+1)/2, t=(k+l+1)/2, ij = i^j, kl = k^l, st = s^t.

	.section .text
	.global  SubBlockHorVer_WMMX0
	.type    SubBlockHorVer_WMMX0, function

SubBlockHorVer_WMMX0:
.ALIGN 4
	stmdb		sp!, {r4,r5,r6,r7, lr}		@ save regs used
	
	mov			r7, r2
	wldrd		wr10, [r2]				@ cur0
	add			r2, r2, r3
	wldrd		wr11, [r2]				@ cur1
	add			r2, r2, r3
	wldrd		wr12, [r2]				@ cur2
	add			r2, r2, r3
	wldrd		wr13, [r2]				@ cur3


	and			r5, r0, #7
	bic			r0, r0, #7
	wldrd		wr0, [r0]				@ ref0
	add			r6, r5, #1
	wldrd		wr1, [r0, #8]			@ ref0
	add			r0, r0, r3				@ source add stride
	cmp			r6, #8
	wldrd		wr2, [r0]				@ ref1
	tmcr		wcgr0, r5
	wldrd		wr3, [r0, #8]			@ ref1
	add			r0, r0, r3
	wldrd		wr4, [r0]				@ ref2
	tmcr		wcgr1, r6
	wldrd		wr5, [r0, #8]			@ ref2
	add			r0, r0, r3
	wldrd		wr6, [r0]				@ ref3
	add			r2, r2, r3
	wldrd		wr7, [r0, #8]			@ ref3				
	add			r0, r0, r3
	wldrd		wr8, [r0]				@ ref4
	wldrd		wr9, [r0, #8]			@ ref4
	add			r0, r0, r3

	mov			r6, #1
	@0
	walignr0	wr14, wr0, wr1			@ wr14 = i
	walignr1ne	wr1, wr0, wr1			@ wr1 = j
	walignr0	wr15, wr2, wr3			@ wr15 = k
	walignr1ne	wr3, wr2, wr3			@ wr3 = l

	wxor		wr0, wr14, wr1			@ wr0 = ij
	wxor		wr2, wr15, wr3			@ wr2 = kl
	wor			wr0, wr0, wr2			@ wr0 = ij|kl

	wavg2br		wr14, wr14, wr1			@ wr14 = (i+j+1)/2 = s
	wavg2br		wr2, wr15, wr3			@ wr2 = (k+l+1)/2 = t
	wxor		wr1, wr14, wr2			@ wr1 = st
	wand		wr0, wr0, wr1			@ wr0 = (ij|kl)&st
	wavg2br		wr14, wr14, wr2			@ wr14 = (s+t+1)/2

	tbcstb		wr2, r6
	wand		wr0, wr0, wr2
		
	wsubb		wr14, wr14, wr0			@ ref0
	wstrd		wr14, [r7]
	add			r7, r7, r3

	wunpckelub	wr0, wr14				@ ref0_0
	wunpckehub	wr1, wr14				@ ref0_1

	wunpckelub	wr2, wr10				@ cur0_0
	wunpckehub	wr10, wr10				@ cur0_1

	wsubh		wr2, wr2, wr0
	wstrd		wr2, [r1], #8
	wsubh		wr10, wr10, wr1
	wstrd		wr10, [r1], #8

	wldrd		wr0, [r0]				@ ref5
	wldrd		wr1, [r0, #8]			@ ref5
	add			r0, r0, r3

	@1
	walignr0	wr14, wr4, wr5			@ wr15 = k
	walignr1ne	wr5, wr4, wr5			@ wr3 = l
	
	wxor		wr4, wr15, wr3			@ wr4 = ij
	wxor		wr10, wr14, wr5			@ wr10 = kl
	wor			wr10, wr4, wr10			@ wr10 = ij|kl

	wavg2br		wr15, wr15, wr3			@ wr14 = (i+j+1)/2 = s
	wavg2br		wr4, wr14, wr5			@ wr4 = (k+l+1)/2 = t

	wxor		wr2, wr15, wr4			@ wr2 = st
	wand		wr10, wr10, wr2			@ wr10 = (ij|kl)&st

	wldrd		wr2, [r0]				@ ref6
	wldrd		wr3, [r0, #8]			@ ref6
	add			r0, r0, r3

	wavg2br		wr15, wr15, wr4			@ wr15 = (s+t+1)/2

	tbcstb		wr4, r6
	wand		wr10, wr10, wr4
		
	wsubb		wr15, wr15, wr10		@ ref0
	wstrd		wr15, [r7]
	add			r7, r7, r3

	wunpckelub	wr4, wr15				@ ref0_0
	wunpckehub	wr15, wr15				@ ref0_1

	wunpckelub	wr10, wr11				@ cur0_0
	wunpckehub	wr11, wr11				@ cur0_1

	wsubh		wr10, wr10, wr4
	wstrd		wr10, [r1], #8
	wsubh		wr11, wr11, wr15
	wstrd		wr11, [r1], #8

	wldrd		wr10, [r2]				@ cur4
	add			r2, r2, r3

	@2
	walignr0	wr15, wr6, wr7			@ wr15 = k
	walignr1ne	wr7, wr6, wr7			@ wr7 = l
	
	wxor		wr6, wr14, wr5			@ wr6 = ij
	wxor		wr11, wr15, wr7			@ wr11 = kl
	wor			wr11, wr11, wr6			@ wr11 = ij|kl

	wavg2br		wr14, wr14, wr5			@ wr14 = (i+j+1)/2 = s
	wavg2br		wr6, wr15, wr7			@ wr6 = (k+l+1)/2 = t

	wxor		wr4, wr14, wr6			@ wr4 = st
	wand		wr11, wr11, wr4			@ wr11 = (ij|kl)&st

	wldrd		wr4, [r0]				@ ref7
	wldrd		wr5, [r0, #8]			@ ref7
	add			r0, r0, r3

	wavg2br		wr14, wr14, wr6			@ wr14 = (s+t+1)/2

	tbcstb		wr6, r6
	wand		wr11, wr11, wr6
	
	wsubb		wr14, wr14, wr11		@ ref0
	wstrd		wr14, [r7]
	add			r7, r7, r3

	wunpckelub	wr6, wr14				@ ref0_0
	wunpckehub	wr14, wr14				@ ref0_1

	wunpckelub	wr11, wr12				@ cur0_0
	wunpckehub	wr12, wr12				@ cur0_1

	wsubh		wr11, wr11, wr6
	wstrd		wr11, [r1], #8
	wsubh		wr12, wr12, wr14
	wstrd		wr12, [r1], #8

	wldrd		wr11, [r2]				@ cur5
	add			r2, r2, r3

	@3
	walignr0	wr14, wr8, wr9			@ wr14 = k
	walignr1ne	wr9, wr8, wr9			@ wr9 = l
	
	wxor		wr8, wr15, wr7			@ wr8 = ij
	wxor		wr6, wr14, wr9			@ wr6 = kl
	wor			wr8, wr8, wr6			@ wr8 = ij|kl

	wavg2br		wr15, wr15, wr7			@ wr15 = (i+j+1)/2 = s
	wavg2br		wr12, wr14, wr9			@ wr12 = (k+l+1)/2 = t

	wxor		wr6, wr15, wr12			@ wr6 = st
	wand		wr8, wr8, wr6			@ wr8 = (ij|kl)&st

	wldrd		wr6, [r0]				@ ref8
	wldrd		wr7, [r0, #8]			@ ref8
	add			r0, r0, r3

	wavg2br		wr15, wr15, wr12		@ wr15 = (s+t+1)/2

	tbcstb		wr12, r6
	wand		wr8, wr8, wr12
	
	wsubb		wr15, wr15, wr8			@ ref0
	wstrd		wr15, [r7]
	add			r7, r7, r3

	wunpckelub	wr12, wr15				@ ref0_0
	wunpckehub	wr15, wr15				@ ref0_1

	wunpckelub	wr8, wr13				@ cur0_0
	wunpckehub	wr13, wr13				@ cur0_1

	wsubh		wr8, wr8, wr12
	wstrd		wr8, [r1], #8
	wsubh		wr13, wr13, wr15
	wstrd		wr13, [r1], #8

	wldrd		wr12, [r2]				@ cur6
	add			r2, r2, r3
	wldrd		wr13, [r2]				@ cur7
	add			r2, r2, r3

	@4
	walignr0	wr15, wr0, wr1			@ wr15 = k
	walignr1ne	wr1, wr0, wr1			@ wr1 = l
	
	wxor		wr0, wr14, wr9			@ wr0 = ij
	wxor		wr8, wr15, wr1			@ wr8 = kl
	wor			wr8, wr8, wr0			@ wr8 = ij|kl

	wavg2br		wr14, wr14, wr9			@ wr14 = (i+j+1)/2 = s
	wavg2br		wr9, wr15, wr1			@ wr9 = (k+l+1)/2 = t

	wxor		wr0, wr14, wr9			@ wr0 = st
	wand		wr8, wr8, wr0			@ wr8 = (ij|kl)&st

	wavg2br		wr14, wr14, wr9			@ wr14 = (s+t+1)/2

	tbcstb		wr9, r6
	wand		wr8, wr8, wr9
	
	wsubb		wr14, wr14, wr8			@ ref0
	wstrd		wr14, [r7]
	add			r7, r7, r3

	wunpckelub	wr0, wr14				@ ref0_0
	wunpckehub	wr9, wr14				@ ref0_1

	wunpckelub	wr8, wr10				@ cur0_0
	wunpckehub	wr10, wr10				@ cur0_1

	wsubh		wr8, wr8, wr0
	wstrd		wr8, [r1], #8
	wsubh		wr10, wr10, wr9
	wstrd		wr10, [r1], #8

	@5
	walignr0	wr14, wr2, wr3			@ wr14 = k
	walignr1ne	wr3, wr2, wr3			@ wr3 = l
	
	wxor		wr2, wr15, wr1			@ wr2 = ij
	wxor		wr8, wr14, wr3			@ wr8 = kl
	wor			wr8, wr8, wr2			@ wr8 = ij|kl

	wavg2br		wr15, wr15, wr1			@ wr15 = (i+j+1)/2 = s
	wavg2br		wr9, wr14, wr3			@ wr9 = (k+l+1)/2 = t

	wxor		wr0, wr15, wr9			@ wr0 = st
	wand		wr8, wr8, wr0			@ wr8 = (ij|kl)&st

	wavg2br		wr15, wr15, wr9			@ wr15 = (s+t+1)/2

	tbcstb		wr9, r6
	wand		wr8, wr8, wr9
	
	wsubb		wr15, wr15, wr8			@ ref0
	wstrd		wr15, [r7]
	add			r7, r7, r3

	wunpckelub	wr9, wr15				@ ref0_0
	wunpckehub	wr15, wr15				@ ref0_1

	wunpckelub	wr8, wr11				@ cur0_0
	wunpckehub	wr11, wr11				@ cur0_1

	wsubh		wr8, wr8, wr9
	wstrd		wr8, [r1], #8
	wsubh		wr11, wr11, wr15
	wstrd		wr11, [r1], #8

	@6
	walignr0	wr15, wr4, wr5			@ wr15 = k
	walignr1ne	wr5, wr4, wr5			@ wr5 = l
	
	wxor		wr2, wr14, wr3			@ wr2 = ij
	wxor		wr8, wr15, wr5			@ wr8 = kl
	wor			wr8, wr8, wr2			@ wr8 = ij|kl

	wavg2br		wr14, wr14, wr3			@ wr14 = (i+j+1)/2 = s
	wavg2br		wr9, wr15, wr5			@ wr9 = (k+l+1)/2 = t

	wxor		wr0, wr14, wr9			@ wr0 = st
	wand		wr8, wr8, wr0			@ wr8 = (ij|kl)&st

	wavg2br		wr14, wr14, wr9			@ wr14 = (s+t+1)/2

	tbcstb		wr9, r6
	wand		wr8, wr8, wr9
		
	wsubb		wr14, wr14, wr8			@ ref0
	wstrd		wr14, [r7]
	add			r7, r7, r3

	wunpckelub	wr9, wr14				@ ref0_0
	wunpckehub	wr14, wr14				@ ref0_1

	wunpckelub	wr8, wr12				@ cur0_0
	wunpckehub	wr12, wr12				@ cur0_1

	wsubh		wr8, wr8, wr9
	wstrd		wr8, [r1], #8
	wsubh		wr12, wr12, wr14
	wstrd		wr12, [r1], #8

	@7
	walignr0	wr14, wr6, wr7			@ wr15 = k
	walignr1ne	wr7, wr6, wr7			@ wr5 = l
	
	wxor		wr2, wr15, wr5			@ wr2 = ij
	wxor		wr8, wr14, wr7			@ wr8 = kl
	wor			wr8, wr8, wr2			@ wr8 = ij|kl

	wavg2br		wr15, wr15, wr5			@ wr15 = (i+j+1)/2 = s
	wavg2br		wr9, wr14, wr7			@ wr9 = (k+l+1)/2 = t

	wxor		wr0, wr15, wr9			@ wr0 = st
	wand		wr8, wr8, wr0			@ wr8 = (ij|kl)&st

	wavg2br		wr15, wr15, wr9			@ wr15 = (s+t+1)/2

	tbcstb		wr9, r6
	wand		wr8, wr8, wr9
	
	wsubb		wr15, wr15, wr8			@ ref0
	wstrd		wr15, [r7]
	add			r7, r7, r3

	wunpckelub	wr9, wr15				@ ref0_0
	wunpckehub	wr14, wr15				@ ref0_1

	wunpckelub	wr8, wr13				@ cur0_0
	wunpckehub	wr13, wr13				@ cur0_1

	wsubh		wr8, wr8, wr9
	wstrd		wr8, [r1], #8
	wsubh		wr13, wr13, wr14
	wstrd		wr13, [r1], #8


    ldmia		sp!, {r4, r5,r6,r7, pc}		@ restore and return   


@void SubBlock(uint8_t *ref, int16_t* dct_data, uint8_t *cur, const int32_t SrcPitch)
@   (i+j+k+l+3)/4 = (s+t+1)/2 - (ij&kl)&st
@   (i+j+k+l+2)/4 = (s+t+1)/2 - (ij|kl)&st rounding == 0
@   (i+j+k+l+1)/4 = (s+t+1)/2 - (ij&kl)|st rounding == 1
@   (i+j+k+l+0)/4 = (s+t+1)/2 - (ij|kl)|st
@ with  s=(i+j+1)/2, t=(k+l+1)/2, ij = i^j, kl = k^l, st = s^t.

	.section .text
	.global  SubBlockHorVer_WMMX1
	.type    SubBlockHorVer_WMMX1, function

SubBlockHorVer_WMMX1:
.ALIGN 4
	stmdb		sp!, {r4,r5,r6,r7, lr}		@ save regs used
	
	mov			r7, r2
	wldrd		wr10, [r2]				@ cur0
	add			r2, r2, r3
	wldrd		wr11, [r2]				@ cur1
	add			r2, r2, r3
	wldrd		wr12, [r2]				@ cur2
	add			r2, r2, r3
	wldrd		wr13, [r2]				@ cur3


	and			r5, r0, #7
	bic			r0, r0, #7
	wldrd		wr0, [r0]				@ ref0
	add			r6, r5, #1
	wldrd		wr1, [r0, #8]			@ ref0
	add			r0, r0, r3				@ source add stride
	cmp			r6, #8
	wldrd		wr2, [r0]				@ ref1
	tmcr		wcgr0, r5
	wldrd		wr3, [r0, #8]			@ ref1
	add			r0, r0, r3
	wldrd		wr4, [r0]				@ ref2
	tmcr		wcgr1, r6
	wldrd		wr5, [r0, #8]			@ ref2
	add			r0, r0, r3
	wldrd		wr6, [r0]				@ ref3
	add			r2, r2, r3
	wldrd		wr7, [r0, #8]			@ ref3				
	add			r0, r0, r3
	wldrd		wr8, [r0]				@ ref4
	wldrd		wr9, [r0, #8]			@ ref4
	add			r0, r0, r3

	mov			r6, #1
	@0
	walignr0	wr14, wr0, wr1			@ wr14 = i
	walignr1ne	wr1, wr0, wr1			@ wr1 = j
	walignr0	wr15, wr2, wr3			@ wr15 = k
	walignr1ne	wr3, wr2, wr3			@ wr3 = l

	wxor		wr0, wr14, wr1			@ wr0 = ij
	wxor		wr2, wr15, wr3			@ wr2 = kl
	wand		wr0, wr0, wr2			@ wr0 = ij&kl

	wavg2br		wr14, wr14, wr1			@ wr14 = (i+j+1)/2 = s
	wavg2br		wr2, wr15, wr3			@ wr2 = (k+l+1)/2 = t
	wxor		wr1, wr14, wr2			@ wr1 = st
	wor			wr0, wr0, wr1			@ wr0 = (ij&kl)|st
	wavg2br		wr14, wr14, wr2			@ wr14 = (s+t+1)/2

	tbcstb		wr2, r6
	wand		wr0, wr0, wr2
		
	wsubb		wr14, wr14, wr0			@ ref0
	wstrd		wr14, [r7]
	add			r7, r7, r3

	wunpckelub	wr0, wr14				@ ref0_0
	wunpckehub	wr1, wr14				@ ref0_1

	wunpckelub	wr2, wr10				@ cur0_0
	wunpckehub	wr10, wr10				@ cur0_1

	wsubh		wr2, wr2, wr0
	wstrd		wr2, [r1], #8
	wsubh		wr10, wr10, wr1
	wstrd		wr10, [r1], #8

	wldrd		wr0, [r0]				@ ref5
	wldrd		wr1, [r0, #8]			@ ref5
	add			r0, r0, r3

	@1
	walignr0	wr14, wr4, wr5			@ wr15 = k
	walignr1ne	wr5, wr4, wr5			@ wr3 = l
	
	wxor		wr4, wr15, wr3			@ wr4 = ij
	wxor		wr10, wr14, wr5			@ wr10 = kl
	wand		wr10, wr4, wr10			@ wr10 = ij&kl

	wavg2br		wr15, wr15, wr3			@ wr14 = (i+j+1)/2 = s
	wavg2br		wr4, wr14, wr5			@ wr4 = (k+l+1)/2 = t

	wxor		wr2, wr15, wr4			@ wr2 = st
	wor 		wr10, wr10, wr2			@ wr10 = (ij&kl)|st

	wldrd		wr2, [r0]				@ ref6
	wldrd		wr3, [r0, #8]			@ ref6
	add			r0, r0, r3

	wavg2br		wr15, wr15, wr4			@ wr15 = (s+t+1)/2

	tbcstb		wr4, r6
	wand		wr10, wr10, wr4
		
	wsubb		wr15, wr15, wr10		@ ref0
	wstrd		wr15, [r7]
	add			r7, r7, r3

	wunpckelub	wr4, wr15				@ ref0_0
	wunpckehub	wr15, wr15				@ ref0_1

	wunpckelub	wr10, wr11				@ cur0_0
	wunpckehub	wr11, wr11				@ cur0_1

	wsubh		wr10, wr10, wr4
	wstrd		wr10, [r1], #8
	wsubh		wr11, wr11, wr15
	wstrd		wr11, [r1], #8

	wldrd		wr10, [r2]				@ cur4
	add			r2, r2, r3

	@2
	walignr0	wr15, wr6, wr7			@ wr15 = k
	walignr1ne	wr7, wr6, wr7			@ wr7 = l
	
	wxor		wr6, wr14, wr5			@ wr6 = ij
	wxor		wr11, wr15, wr7			@ wr11 = kl
	wand		wr11, wr11, wr6			@ wr11 = ij&kl

	wavg2br		wr14, wr14, wr5			@ wr14 = (i+j+1)/2 = s
	wavg2br		wr6, wr15, wr7			@ wr6 = (k+l+1)/2 = t

	wxor		wr4, wr14, wr6			@ wr4 = st
	wor 		wr11, wr11, wr4			@ wr11 = (ij&kl)|st

	wldrd		wr4, [r0]				@ ref7
	wldrd		wr5, [r0, #8]			@ ref7
	add			r0, r0, r3

	wavg2br		wr14, wr14, wr6			@ wr14 = (s+t+1)/2

	tbcstb		wr6, r6
	wand		wr11, wr11, wr6
	
	wsubb		wr14, wr14, wr11		@ ref0
	wstrd		wr14, [r7]
	add			r7, r7, r3

	wunpckelub	wr6, wr14				@ ref0_0
	wunpckehub	wr14, wr14				@ ref0_1

	wunpckelub	wr11, wr12				@ cur0_0
	wunpckehub	wr12, wr12				@ cur0_1

	wsubh		wr11, wr11, wr6
	wstrd		wr11, [r1], #8
	wsubh		wr12, wr12, wr14
	wstrd		wr12, [r1], #8

	wldrd		wr11, [r2]				@ cur5
	add			r2, r2, r3

	@3
	walignr0	wr14, wr8, wr9			@ wr14 = k
	walignr1ne	wr9, wr8, wr9			@ wr9 = l
	
	wxor		wr8, wr15, wr7			@ wr8 = ij
	wxor		wr6, wr14, wr9			@ wr6 = kl
	wand		wr8, wr8, wr6			@ wr8 = ij&kl

	wavg2br		wr15, wr15, wr7			@ wr15 = (i+j+1)/2 = s
	wavg2br		wr12, wr14, wr9			@ wr12 = (k+l+1)/2 = t

	wxor		wr6, wr15, wr12			@ wr6 = st
	wor 		wr8, wr8, wr6			@ wr8 = (ij&kl)|st

	wldrd		wr6, [r0]				@ ref8
	wldrd		wr7, [r0, #8]			@ ref8
	add			r0, r0, r3

	wavg2br		wr15, wr15, wr12		@ wr15 = (s+t+1)/2

	tbcstb		wr12, r6
	wand		wr8, wr8, wr12
	
	wsubb		wr15, wr15, wr8			@ ref0
	wstrd		wr15, [r7]
	add			r7, r7, r3

	wunpckelub	wr12, wr15				@ ref0_0
	wunpckehub	wr15, wr15				@ ref0_1

	wunpckelub	wr8, wr13				@ cur0_0
	wunpckehub	wr13, wr13				@ cur0_1

	wsubh		wr8, wr8, wr12
	wstrd		wr8, [r1], #8
	wsubh		wr13, wr13, wr15
	wstrd		wr13, [r1], #8

	wldrd		wr12, [r2]				@ cur6
	add			r2, r2, r3
	wldrd		wr13, [r2]				@ cur7
	add			r2, r2, r3

	@4
	walignr0	wr15, wr0, wr1			@ wr15 = k
	walignr1ne	wr1, wr0, wr1			@ wr1 = l
	
	wxor		wr0, wr14, wr9			@ wr0 = ij
	wxor		wr8, wr15, wr1			@ wr8 = kl
	wand		wr8, wr8, wr0			@ wr8 = ij&kl

	wavg2br		wr14, wr14, wr9			@ wr14 = (i+j+1)/2 = s
	wavg2br		wr9, wr15, wr1			@ wr9 = (k+l+1)/2 = t

	wxor		wr0, wr14, wr9			@ wr0 = st
	wor			wr8, wr8, wr0			@ wr8 = (ij&kl)|st

	wavg2br		wr14, wr14, wr9			@ wr14 = (s+t+1)/2

	tbcstb		wr9, r6
	wand		wr8, wr8, wr9
	
	wsubb		wr14, wr14, wr8			@ ref0
	wstrd		wr14, [r7]
	add			r7, r7, r3

	wunpckelub	wr0, wr14				@ ref0_0
	wunpckehub	wr9, wr14				@ ref0_1

	wunpckelub	wr8, wr10				@ cur0_0
	wunpckehub	wr10, wr10				@ cur0_1

	wsubh		wr8, wr8, wr0
	wstrd		wr8, [r1], #8
	wsubh		wr10, wr10, wr9
	wstrd		wr10, [r1], #8

	@5
	walignr0	wr14, wr2, wr3			@ wr14 = k
	walignr1ne	wr3, wr2, wr3			@ wr3 = l
	
	wxor		wr2, wr15, wr1			@ wr2 = ij
	wxor		wr8, wr14, wr3			@ wr8 = kl
	wand		wr8, wr8, wr2			@ wr8 = ij&kl

	wavg2br		wr15, wr15, wr1			@ wr15 = (i+j+1)/2 = s
	wavg2br		wr9, wr14, wr3			@ wr9 = (k+l+1)/2 = t

	wxor		wr0, wr15, wr9			@ wr0 = st
	wor			wr8, wr8, wr0			@ wr8 = (ij&kl)|st

	wavg2br		wr15, wr15, wr9			@ wr15 = (s+t+1)/2

	tbcstb		wr9, r6
	wand		wr8, wr8, wr9
	
	wsubb		wr15, wr15, wr8			@ ref0
	wstrd		wr15, [r7]
	add			r7, r7, r3

	wunpckelub	wr9, wr15				@ ref0_0
	wunpckehub	wr15, wr15				@ ref0_1

	wunpckelub	wr8, wr11				@ cur0_0
	wunpckehub	wr11, wr11				@ cur0_1

	wsubh		wr8, wr8, wr9
	wstrd		wr8, [r1], #8
	wsubh		wr11, wr11, wr15
	wstrd		wr11, [r1], #8

	@6
	walignr0	wr15, wr4, wr5			@ wr15 = k
	walignr1ne	wr5, wr4, wr5			@ wr5 = l
	
	wxor		wr2, wr14, wr3			@ wr2 = ij
	wxor		wr8, wr15, wr5			@ wr8 = kl
	wand		wr8, wr8, wr2			@ wr8 = ij&kl

	wavg2br		wr14, wr14, wr3			@ wr14 = (i+j+1)/2 = s
	wavg2br		wr9, wr15, wr5			@ wr9 = (k+l+1)/2 = t

	wxor		wr0, wr14, wr9			@ wr0 = st
	wor			wr8, wr8, wr0			@ wr8 = (ij&kl)|st

	wavg2br		wr14, wr14, wr9			@ wr14 = (s+t+1)/2

	tbcstb		wr9, r6
	wand		wr8, wr8, wr9
		
	wsubb		wr14, wr14, wr8			@ ref0
	wstrd		wr14, [r7]
	add			r7, r7, r3

	wunpckelub	wr9, wr14				@ ref0_0
	wunpckehub	wr14, wr14				@ ref0_1

	wunpckelub	wr8, wr12				@ cur0_0
	wunpckehub	wr12, wr12				@ cur0_1

	wsubh		wr8, wr8, wr9
	wstrd		wr8, [r1], #8
	wsubh		wr12, wr12, wr14
	wstrd		wr12, [r1], #8

	@7
	walignr0	wr14, wr6, wr7			@ wr15 = k
	walignr1ne	wr7, wr6, wr7			@ wr5 = l
	
	wxor		wr2, wr15, wr5			@ wr2 = ij
	wxor		wr8, wr14, wr7			@ wr8 = kl
	wand		wr8, wr8, wr2			@ wr8 = ij&kl

	wavg2br		wr15, wr15, wr5			@ wr15 = (i+j+1)/2 = s
	wavg2br		wr9, wr14, wr7			@ wr9 = (k+l+1)/2 = t

	wxor		wr0, wr15, wr9			@ wr0 = st
	wor			wr8, wr8, wr0			@ wr8 = (ij&kl)|st

	wavg2br		wr15, wr15, wr9			@ wr15 = (s+t+1)/2

	tbcstb		wr9, r6
	wand		wr8, wr8, wr9
	
	wsubb		wr15, wr15, wr8			@ ref0
	wstrd		wr15, [r7]
	add			r7, r7, r3

	wunpckelub	wr9, wr15				@ ref0_0
	wunpckehub	wr14, wr15				@ ref0_1

	wunpckelub	wr8, wr13				@ cur0_0
	wunpckehub	wr13, wr13				@ cur0_1

	wsubh		wr8, wr8, wr9
	wstrd		wr8, [r1], #8
	wsubh		wr13, wr13, wr14
	wstrd		wr13, [r1], #8


    ldmia		sp!, {r4, r5,r6,r7, pc}		@ restore and return   


@void SubBlock(uint8_t *ref, int16_t* dct_data, uint8_t *cur, const int32_t SrcPitch)
	.section .text
	.global  SubBlockHor_WMMX0
	.type    SubBlockHor_WMMX0, function

SubBlockHor_WMMX0:
.ALIGN 4
	stmdb		sp!, {r5,r6,r7, lr}		@ save regs used

	mov			r7, r2
	wldrd		wr10, [r2]				@ cur0
	add			r2, r2, r3
	wldrd		wr11, [r2]				@ cur1
	add			r2, r2, r3
	wldrd		wr12, [r2]				@ cur2
	add			r2, r2, r3
	wldrd		wr13, [r2]				@ cur3
	add			r2, r2, r3
	and			r5, r0, #7
	bic			r0, r0, #7
	wldrd		wr0, [r0]				@ ref0
	add			r6, r5, #1
	wldrd		wr1, [r0, #8]			@ ref0
	add			r0, r0, r3
	wldrd		wr2, [r0]				@ ref1
	cmp			r6, #8					@ copare #8 if equal will not do align
	wldrd		wr3, [r0, #8]			@ ref1
	add			r0, r0, r3
	wldrd		wr4, [r0]				@ ref2
	tmcr		wcgr0, r5
	wldrd		wr5, [r0, #8]			@ ref2
	add			r0, r0, r3
	wldrd		wr6, [r0]				@ ref3
	tmcrne		wcgr1, r6
	wldrd		wr7, [r0, #8]			@ ref3
	add			r0, r0, r3

	wldrd		wr14, [r2]				@ cur4
	add			r2, r2, r3
	wldrd		wr15, [r2]				@ cur5
	add			r2, r2, r3
	wldrd		wr9, [r2]				@ cur6
			
	walignr0	wr8, wr0, wr1			@ ref0
	walignr1ne	wr1, wr0, wr1
	wavg2br		wr8, wr8, wr1
	wstrd		wr8, [r7]
	add			r7, r7, r3

	wunpckelub	wr0, wr8				@ ref0_0
	wunpckehub	wr1, wr8				@ ref0_1

	wunpckelub	wr8, wr10				@ cur0_0
	wunpckehub	wr10, wr10				@ cur0_1

	wsubh		wr8, wr8, wr0
	wstrd		wr8, [r1], #8
	wsubh		wr10, wr10, wr1
	wstrd		wr10, [r1], #8
	
	walignr0	wr8, wr2, wr3			@ ref1
	wldrd		wr0, [r0]				@ ref4
	walignr1ne	wr3, wr2, wr3
	wldrd		wr1, [r0, #8]			@ ref4
	wavg2br		wr8, wr8, wr3
	add			r0, r0, r3
	wstrd		wr8, [r7]
	add			r7, r7, r3

	wunpckelub	wr2, wr8				@ ref1_0
	wunpckehub	wr8, wr8				@ ref1_1

	wunpckelub	wr10, wr11				@ cur1_0
	wunpckehub	wr11, wr11				@ cur1_1

	wsubh		wr10, wr10, wr2
	wstrd		wr10, [r1], #8
	wsubh		wr11, wr11, wr8
	wstrd		wr11, [r1], #8


	walignr0	wr8, wr4, wr5			@ ref2
	wldrd		wr2, [r0]				@ ref5
	walignr1ne	wr5, wr4, wr5
	wldrd		wr3, [r0, #8]			@ ref5
	wavg2br		wr8, wr8, wr5
	add			r0, r0, r3
	wstrd		wr8, [r7]
	add			r7, r7, r3
	wldrd		wr4, [r0]				@ ref6
	wunpckelub	wr10, wr8				@ ref2_0
	wldrd		wr5, [r0, #8]			@ ref6
	wunpckehub	wr8, wr8				@ ref2_1
	add			r0, r0, r3
	wunpckelub	wr11, wr12				@ cur2_0
	wunpckehub	wr12, wr12				@ cur2_1

	wsubh		wr11, wr11, wr10
	wstrd		wr11, [r1], #8
	wsubh		wr12, wr12, wr8
	wstrd		wr12, [r1], #8

	add			r2, r2, r3
	wldrd		wr12, [r2]				@ cur7

	walignr0	wr8, wr6, wr7			@ ref3
	walignr1ne	wr7, wr6, wr7
	wldrd		wr6, [r0]				@ ref7
	wavg2br		wr8, wr8, wr7
	wstrd		wr8, [r7]
	add			r7, r7, r3
	wldrd		wr7, [r0, #8]			@ ref7
	wunpckelub	wr10, wr8				@ ref3_0
	wunpckehub	wr8, wr8				@ ref3_1

	wunpckelub	wr11, wr13				@ cur3_0
	wunpckehub	wr13, wr13				@ cur3_1

	wsubh		wr11, wr11, wr10
	wstrd		wr11, [r1], #8
	wsubh		wr13, wr13, wr8
	wstrd		wr13, [r1], #8


	walignr0	wr8, wr0, wr1			@ ref4
	walignr1ne	wr1, wr0, wr1
	wavg2br		wr8, wr8, wr1
	wstrd		wr8, [r7]
	add			r7, r7, r3

	wunpckelub	wr10, wr8				@ ref4_0
	wunpckehub	wr8, wr8				@ ref4_1

	wunpckelub	wr11, wr14				@ cur4_0
	wunpckehub	wr13, wr14				@ cur4_1

	wsubh		wr11, wr11, wr10
	wstrd		wr11, [r1], #8
	wsubh		wr13, wr13, wr8
	wstrd		wr13, [r1], #8

	walignr0	wr8, wr2, wr3			@ ref5
	walignr1ne	wr3, wr2, wr3
	wavg2br		wr8, wr8, wr3
	wstrd		wr8, [r7]
	add			r7, r7, r3

	wunpckelub	wr10, wr8				@ ref5_0
	wunpckehub	wr8, wr8				@ ref5_1

	wunpckelub	wr11, wr15				@ cur5_0
	wunpckehub	wr13, wr15				@ cur5_1

	wsubh		wr11, wr11, wr10
	wstrd		wr11, [r1], #8
	wsubh		wr13, wr13, wr8
	wstrd		wr13, [r1], #8


	walignr0	wr8, wr4, wr5			@ ref6
	walignr1ne	wr5, wr4, wr5
	wavg2br		wr8, wr8, wr5
	wstrd		wr8, [r7]
	add			r7, r7, r3

	wunpckelub	wr10, wr8				@ ref6_0
	wunpckehub	wr8, wr8				@ ref6_1

	wunpckelub	wr11, wr9				@ cur6_0
	wunpckehub	wr13, wr9				@ cur6_1

	wsubh		wr11, wr11, wr10
	wstrd		wr11, [r1], #8
	wsubh		wr13, wr13, wr8
	wstrd		wr13, [r1], #8

	walignr0	wr8, wr6, wr7			@ ref7
	walignr1ne	wr7, wr6, wr7
	wavg2br		wr8, wr8, wr7
	wstrd		wr8, [r7]

	wunpckelub	wr10, wr8				@ ref7_0
	wunpckehub	wr8, wr8				@ ref7_1

	wunpckelub	wr11, wr12				@ cur7_0
	wunpckehub	wr13, wr12				@ cur7_1

	wsubh		wr11, wr11, wr10
	wstrd		wr11, [r1], #8
	wsubh		wr13, wr13, wr8
	wstrd		wr13, [r1], #8

    ldmia		sp!, {r5,r6,r7, pc}		@ restore and return 


	.section .text
	.global  SubBlockHor_WMMX1
	.type    SubBlockHor_WMMX1, function

SubBlockHor_WMMX1:
.ALIGN 4
	stmdb		sp!, {r5,r6,r7, lr}		@ save regs used

	mov			r7, r2
	wldrd		wr10, [r2]				@ cur0
	add			r2, r2, r3
	wldrd		wr11, [r2]				@ cur1
	add			r2, r2, r3
	wldrd		wr12, [r2]				@ cur2
	add			r2, r2, r3
	wldrd		wr13, [r2]				@ cur3
	add			r2, r2, r3
	and			r5, r0, #7
	bic			r0, r0, #7
	wldrd		wr0, [r0]				@ ref0
	add			r6, r5, #1
	wldrd		wr1, [r0, #8]			@ ref0
	add			r0, r0, r3
	wldrd		wr2, [r0]				@ ref1
	cmp			r6, #8					@ copare #8 if equal will not do align
	wldrd		wr3, [r0, #8]			@ ref1
	add			r0, r0, r3
	wldrd		wr4, [r0]				@ ref2
	tmcr		wcgr0, r5
	wldrd		wr5, [r0, #8]			@ ref2
	add			r0, r0, r3
	wldrd		wr6, [r0]				@ ref3
	tmcrne		wcgr1, r6
	wldrd		wr7, [r0, #8]			@ ref3
	add			r0, r0, r3

	wldrd		wr14, [r2]				@ cur4
	add			r2, r2, r3
	wldrd		wr15, [r2]				@ cur5
	add			r2, r2, r3
	wldrd		wr9, [r2]				@ cur6
			
	walignr0	wr8, wr0, wr1			@ ref0
	walignr1ne	wr1, wr0, wr1
	wavg2b		wr8, wr8, wr1
	wstrd		wr8, [r7]
	add			r7, r7, r3

	wunpckelub	wr0, wr8				@ ref0_0
	wunpckehub	wr1, wr8				@ ref0_1

	wunpckelub	wr8, wr10				@ cur0_0
	wunpckehub	wr10, wr10				@ cur0_1

	wsubh		wr8, wr8, wr0
	wstrd		wr8, [r1], #8
	wsubh		wr10, wr10, wr1
	wstrd		wr10, [r1], #8
	
	walignr0	wr8, wr2, wr3			@ ref1
	wldrd		wr0, [r0]				@ ref4
	walignr1ne	wr3, wr2, wr3
	wldrd		wr1, [r0, #8]			@ ref4
	wavg2b		wr8, wr8, wr3
	add			r0, r0, r3
	wstrd		wr8, [r7]
	add			r7, r7, r3

	wunpckelub	wr2, wr8				@ ref1_0
	wunpckehub	wr8, wr8				@ ref1_1

	wunpckelub	wr10, wr11				@ cur1_0
	wunpckehub	wr11, wr11				@ cur1_1

	wsubh		wr10, wr10, wr2
	wstrd		wr10, [r1], #8
	wsubh		wr11, wr11, wr8
	wstrd		wr11, [r1], #8


	walignr0	wr8, wr4, wr5			@ ref2
	wldrd		wr2, [r0]				@ ref5
	walignr1ne	wr5, wr4, wr5
	wldrd		wr3, [r0, #8]			@ ref5
	wavg2b		wr8, wr8, wr5
	add			r0, r0, r3
	wstrd		wr8, [r7]
	add			r7, r7, r3
	wldrd		wr4, [r0]				@ ref6
	wunpckelub	wr10, wr8				@ ref2_0
	wldrd		wr5, [r0, #8]			@ ref6
	wunpckehub	wr8, wr8				@ ref2_1
	add			r0, r0, r3
	wunpckelub	wr11, wr12				@ cur2_0
	wunpckehub	wr12, wr12				@ cur2_1

	wsubh		wr11, wr11, wr10
	wstrd		wr11, [r1], #8
	wsubh		wr12, wr12, wr8
	wstrd		wr12, [r1], #8

	add			r2, r2, r3
	wldrd		wr12, [r2]				@ cur7

	walignr0	wr8, wr6, wr7			@ ref3
	walignr1ne	wr7, wr6, wr7
	wldrd		wr6, [r0]				@ ref7
	wavg2b		wr8, wr8, wr7
	wstrd		wr8, [r7]
	add			r7, r7, r3
	wldrd		wr7, [r0, #8]			@ ref7
	wunpckelub	wr10, wr8				@ ref3_0
	wunpckehub	wr8, wr8				@ ref3_1

	wunpckelub	wr11, wr13				@ cur3_0
	wunpckehub	wr13, wr13				@ cur3_1

	wsubh		wr11, wr11, wr10
	wstrd		wr11, [r1], #8
	wsubh		wr13, wr13, wr8
	wstrd		wr13, [r1], #8


	walignr0	wr8, wr0, wr1			@ ref4
	walignr1ne	wr1, wr0, wr1
	wavg2b		wr8, wr8, wr1
	wstrd		wr8, [r7]
	add			r7, r7, r3

	wunpckelub	wr10, wr8				@ ref4_0
	wunpckehub	wr8, wr8				@ ref4_1

	wunpckelub	wr11, wr14				@ cur4_0
	wunpckehub	wr13, wr14				@ cur4_1

	wsubh		wr11, wr11, wr10
	wstrd		wr11, [r1], #8
	wsubh		wr13, wr13, wr8
	wstrd		wr13, [r1], #8

	walignr0	wr8, wr2, wr3			@ ref5
	walignr1ne	wr3, wr2, wr3
	wavg2b		wr8, wr8, wr3
	wstrd		wr8, [r7]
	add			r7, r7, r3

	wunpckelub	wr10, wr8				@ ref5_0
	wunpckehub	wr8, wr8				@ ref5_1

	wunpckelub	wr11, wr15				@ cur5_0
	wunpckehub	wr13, wr15				@ cur5_1

	wsubh		wr11, wr11, wr10
	wstrd		wr11, [r1], #8
	wsubh		wr13, wr13, wr8
	wstrd		wr13, [r1], #8


	walignr0	wr8, wr4, wr5			@ ref6
	walignr1ne	wr5, wr4, wr5
	wavg2b		wr8, wr8, wr5
	wstrd		wr8, [r7]
	add			r7, r7, r3

	wunpckelub	wr10, wr8				@ ref6_0
	wunpckehub	wr8, wr8				@ ref6_1

	wunpckelub	wr11, wr9				@ cur6_0
	wunpckehub	wr13, wr9				@ cur6_1

	wsubh		wr11, wr11, wr10
	wstrd		wr11, [r1], #8
	wsubh		wr13, wr13, wr8
	wstrd		wr13, [r1], #8

	walignr0	wr8, wr6, wr7			@ ref7
	walignr1ne	wr7, wr6, wr7
	wavg2b		wr8, wr8, wr7
	wstrd		wr8, [r7]

	wunpckelub	wr10, wr8				@ ref7_0
	wunpckehub	wr8, wr8				@ ref7_1

	wunpckelub	wr11, wr12				@ cur7_0
	wunpckehub	wr13, wr12				@ cur7_1

	wsubh		wr11, wr11, wr10
	wstrd		wr11, [r1], #8
	wsubh		wr13, wr13, wr8
	wstrd		wr13, [r1], #8

    ldmia		sp!, {r5,r6,r7, pc}		@ restore and return 
	
@void SubBlock(uint8_t *ref, int16_t* dct_data, uint8_t *cur, const int32_t SrcPitch)	
	.section .text
	.global  SubBlockVer_WMMX0
	.type    SubBlockVer_WMMX0, function

SubBlockVer_WMMX0:
.ALIGN 4
	stmdb		sp!, {r5,r6,r7, lr}		@ save regs used

	mov			r7, r2
	wldrd		wr10, [r2]				@ cur0
	add			r2, r2, r3
	wldrd		wr11, [r2]				@ cur1
	add			r2, r2, r3
	wldrd		wr12, [r2]				@ cur2
	add			r2, r2, r3
	wldrd		wr13, [r2]				@ cur3
	and			r5, r0, #7
	bic			r0, r0, #7
@	sub			r0, r0, r3				@ ref - stride

	wldrd		wr0, [r0]				@ ref0
	add			r2, r2, r3
	wldrd		wr1, [r0, #8]			@ ref0
	add			r0, r0, r3
	wldrd		wr2, [r0]				@ ref1
	wldrd		wr3, [r0, #8]			@ ref1
	add			r0, r0, r3
	wldrd		wr4, [r0]				@ ref2
	tmcr		wcgr0, r5
	wldrd		wr5, [r0, #8]			@ ref2
	add			r0, r0, r3
	wldrd		wr6, [r0]				@ ref3
	wldrd		wr7, [r0, #8]			@ ref3
	add			r0, r0, r3

	wldrd		wr14, [r2]				@ cur4
	add			r2, r2, r3
	wldrd		wr15, [r2]				@ cur5
	add			r2, r2, r3
	wldrd		wr9, [r2]				@ cur6
			
	walignr0	wr0, wr0, wr1			@ ref0
	walignr0	wr8, wr2, wr3			@ ref1
	wavg2br		wr0, wr0, wr8
	wstrd		wr0, [r7]
	add			r7, r7, r3

	wunpckelub	wr1, wr0				@ 
	wunpckehub	wr0, wr0				@ 
	wunpckelub	wr2, wr10				@ cur0_0
	wunpckehub	wr10, wr10				@ cur0_1


	wsubh		wr2, wr2, wr1
	wstrd		wr2, [r1], #8
	wsubh		wr10, wr10, wr0
	wstrd		wr10, [r1], #8
	
	wldrd		wr0, [r0]				@ ref4
	wldrd		wr1, [r0, #8]			@ ref4
	add			r0, r0, r3

	wldrd		wr2, [r0]				@ ref5
	wldrd		wr3, [r0, #8]			@ ref5
	add			r0, r0, r3


	walignr0	wr10, wr4, wr5			@ ref2
	wavg2br		wr8, wr10, wr8
	wstrd		wr8, [r7]
	add			r7, r7, r3

	wunpckelub	wr4, wr8				@ ref1_0
	wunpckehub	wr5, wr8				@ ref1_1

	wunpckelub	wr8, wr11				@ cur1_0
	wunpckehub	wr11, wr11				@ cur1_1

	wsubh		wr8, wr8, wr4
	wstrd		wr8, [r1], #8
	wsubh		wr11, wr11, wr5
	wstrd		wr11, [r1], #8


	
	walignr0	wr8, wr6, wr7			@ ref3
	wldrd		wr4, [r0]				@ ref6
	wavg2br		wr10, wr8, wr10
	wldrd		wr5, [r0, #8]			@ ref6
	add			r0, r0, r3
	wstrd		wr10, [r7]
	add			r7, r7, r3

	wunpckelub	wr6, wr10				@ ref2_0
	wunpckehub	wr7, wr10				@ ref2_1

	wunpckelub	wr11, wr12				@ cur2_0
	wunpckehub	wr12, wr12				@ cur2_1


	wsubh		wr11, wr11, wr6
	wstrd		wr11, [r1], #8
	wsubh		wr12, wr12, wr7
	wstrd		wr12, [r1], #8

	add			r2, r2, r3
	wldrd		wr12, [r2]				@ cur7

	walignr0	wr10, wr0, wr1			@ ref4
	wldrd		wr6, [r0]				@ ref7
	wavg2br		wr8, wr8, wr10
	wldrd		wr7, [r0, #8]			@ ref7
	add			r0, r0, r3
	wstrd		wr8, [r7]
	add			r7, r7, r3

	wunpckelub	wr0, wr8				@ ref3_0
	wunpckehub	wr1, wr8				@ ref3_1

	wunpckelub	wr11, wr13				@ cur3_0
	wunpckehub	wr13, wr13				@ cur3_1

	wsubh		wr11, wr11, wr0
	wstrd		wr11, [r1], #8
	wsubh		wr13, wr13, wr1
	wstrd		wr13, [r1], #8

	walignr0	wr8, wr2, wr3			@ ref5
	wldrd		wr0, [r0]				@ ref8
	wavg2br		wr10, wr8, wr10
	wldrd		wr1, [r0, #8]			@ ref8
	add			r0, r0, r3
	wstrd		wr10, [r7]
	add			r7, r7, r3


	wunpckelub	wr2, wr10				@ ref4_0
	wunpckehub	wr3, wr10				@ ref4_1

	wunpckelub	wr11, wr14				@ cur4_0
	wunpckehub	wr13, wr14				@ cur4_1

	wsubh		wr11, wr11, wr2
	wstrd		wr11, [r1], #8
	wsubh		wr13, wr13, wr3
	wstrd		wr13, [r1], #8


	walignr0	wr10, wr4, wr5			@ ref6
	wavg2br		wr8, wr8, wr10
	wstrd		wr8, [r7]
	add			r7, r7, r3

	wunpckelub	wr4, wr8				@ ref5_0
	wunpckehub	wr5, wr8				@ ref5_1

	wunpckelub	wr11, wr15				@ cur5_0
	wunpckehub	wr13, wr15				@ cur5_1

	wsubh		wr11, wr11, wr4
	wstrd		wr11, [r1], #8
	wsubh		wr13, wr13, wr5
	wstrd		wr13, [r1], #8


	walignr0	wr8, wr6, wr7			@ ref7
	wavg2br		wr10, wr8, wr10
	wstrd		wr10, [r7]
	add			r7, r7, r3

	wunpckelub	wr6, wr10				@ ref6_0
	wunpckehub	wr7, wr10				@ ref6_1

	wunpckelub	wr11, wr9				@ cur6_0
	wunpckehub	wr13, wr9				@ cur6_1

	wsubh		wr11, wr11, wr6
	wstrd		wr11, [r1], #8
	wsubh		wr13, wr13, wr7
	wstrd		wr13, [r1], #8

	walignr0	wr10, wr0, wr1			@ ref8
	wavg2br		wr8, wr8, wr10
	wstrd		wr8, [r7]

	wunpckelub	wr11, wr8				@ ref7_0
	wunpckehub	wr10, wr8				@ ref7_1

	wunpckelub	wr14, wr12				@ cur7_0
	wunpckehub	wr13, wr12				@ cur7_1

	wsubh		wr11, wr14, wr11
	wstrd		wr11, [r1], #8
	wsubh		wr13, wr13, wr10
	wstrd		wr13, [r1], #8

    ldmia		sp!, {r5,r6, r7, pc}		@ restore and return
	
	.section .text
	.global  SubBlockVer_WMMX1
	.type    SubBlockVer_WMMX1, function

SubBlockVer_WMMX1:
.ALIGN 4
	stmdb		sp!, {r5,r6,r7, lr}		@ save regs used

	mov			r7, r2
	wldrd		wr10, [r2]				@ cur0
	add			r2, r2, r3
	wldrd		wr11, [r2]				@ cur1
	add			r2, r2, r3
	wldrd		wr12, [r2]				@ cur2
	add			r2, r2, r3
	wldrd		wr13, [r2]				@ cur3
	and			r5, r0, #7
	bic			r0, r0, #7
@	sub			r0, r0, r3				@ ref - stride

	wldrd		wr0, [r0]				@ ref0
	add			r2, r2, r3
	wldrd		wr1, [r0, #8]			@ ref0
	add			r0, r0, r3
	wldrd		wr2, [r0]				@ ref1
	wldrd		wr3, [r0, #8]			@ ref1
	add			r0, r0, r3
	wldrd		wr4, [r0]				@ ref2
	tmcr		wcgr0, r5
	wldrd		wr5, [r0, #8]			@ ref2
	add			r0, r0, r3
	wldrd		wr6, [r0]				@ ref3
	wldrd		wr7, [r0, #8]			@ ref3
	add			r0, r0, r3

	wldrd		wr14, [r2]				@ cur4
	add			r2, r2, r3
	wldrd		wr15, [r2]				@ cur5
	add			r2, r2, r3
	wldrd		wr9, [r2]				@ cur6
			
	walignr0	wr0, wr0, wr1			@ ref0
	walignr0	wr8, wr2, wr3			@ ref1
	wavg2b		wr0, wr0, wr8
	wstrd		wr0, [r7]
	add			r7, r7, r3

	wunpckelub	wr1, wr0				@ 
	wunpckehub	wr0, wr0				@ 
	wunpckelub	wr2, wr10				@ cur0_0
	wunpckehub	wr10, wr10				@ cur0_1


	wsubh		wr2, wr2, wr1
	wstrd		wr2, [r1], #8
	wsubh		wr10, wr10, wr0
	wstrd		wr10, [r1], #8
	
	wldrd		wr0, [r0]				@ ref4
	wldrd		wr1, [r0, #8]			@ ref4
	add			r0, r0, r3

	wldrd		wr2, [r0]				@ ref5
	wldrd		wr3, [r0, #8]			@ ref5
	add			r0, r0, r3


	walignr0	wr10, wr4, wr5			@ ref2
	wavg2b		wr8, wr10, wr8
	wstrd		wr8, [r7]
	add			r7, r7, r3

	wunpckelub	wr4, wr8				@ ref1_0
	wunpckehub	wr5, wr8				@ ref1_1

	wunpckelub	wr8, wr11				@ cur1_0
	wunpckehub	wr11, wr11				@ cur1_1

	wsubh		wr8, wr8, wr4
	wstrd		wr8, [r1], #8
	wsubh		wr11, wr11, wr5
	wstrd		wr11, [r1], #8


	
	walignr0	wr8, wr6, wr7			@ ref3
	wldrd		wr4, [r0]				@ ref6
	wavg2b		wr10, wr8, wr10
	wldrd		wr5, [r0, #8]			@ ref6
	add			r0, r0, r3
	wstrd		wr10, [r7]
	add			r7, r7, r3

	wunpckelub	wr6, wr10				@ ref2_0
	wunpckehub	wr7, wr10				@ ref2_1

	wunpckelub	wr11, wr12				@ cur2_0
	wunpckehub	wr12, wr12				@ cur2_1


	wsubh		wr11, wr11, wr6
	wstrd		wr11, [r1], #8
	wsubh		wr12, wr12, wr7
	wstrd		wr12, [r1], #8

	add			r2, r2, r3
	wldrd		wr12, [r2]				@ cur7

	walignr0	wr10, wr0, wr1			@ ref4
	wldrd		wr6, [r0]				@ ref7
	wavg2b		wr8, wr8, wr10
	wldrd		wr7, [r0, #8]			@ ref7
	add			r0, r0, r3
	wstrd		wr8, [r7]
	add			r7, r7, r3

	wunpckelub	wr0, wr8				@ ref3_0
	wunpckehub	wr1, wr8				@ ref3_1

	wunpckelub	wr11, wr13				@ cur3_0
	wunpckehub	wr13, wr13				@ cur3_1

	wsubh		wr11, wr11, wr0
	wstrd		wr11, [r1], #8
	wsubh		wr13, wr13, wr1
	wstrd		wr13, [r1], #8

	walignr0	wr8, wr2, wr3			@ ref5
	wldrd		wr0, [r0]				@ ref8
	wavg2b		wr10, wr8, wr10
	wldrd		wr1, [r0, #8]			@ ref8
	add			r0, r0, r3
	wstrd		wr10, [r7]
	add			r7, r7, r3


	wunpckelub	wr2, wr10				@ ref4_0
	wunpckehub	wr3, wr10				@ ref4_1

	wunpckelub	wr11, wr14				@ cur4_0
	wunpckehub	wr13, wr14				@ cur4_1

	wsubh		wr11, wr11, wr2
	wstrd		wr11, [r1], #8
	wsubh		wr13, wr13, wr3
	wstrd		wr13, [r1], #8


	walignr0	wr10, wr4, wr5			@ ref6
	wavg2b		wr8, wr8, wr10
	wstrd		wr8, [r7]
	add			r7, r7, r3

	wunpckelub	wr4, wr8				@ ref5_0
	wunpckehub	wr5, wr8				@ ref5_1

	wunpckelub	wr11, wr15				@ cur5_0
	wunpckehub	wr13, wr15				@ cur5_1

	wsubh		wr11, wr11, wr4
	wstrd		wr11, [r1], #8
	wsubh		wr13, wr13, wr5
	wstrd		wr13, [r1], #8


	walignr0	wr8, wr6, wr7			@ ref7
	wavg2b		wr10, wr8, wr10
	wstrd		wr10, [r7]
	add			r7, r7, r3

	wunpckelub	wr6, wr10				@ ref6_0
	wunpckehub	wr7, wr10				@ ref6_1

	wunpckelub	wr11, wr9				@ cur6_0
	wunpckehub	wr13, wr9				@ cur6_1

	wsubh		wr11, wr11, wr6
	wstrd		wr11, [r1], #8
	wsubh		wr13, wr13, wr7
	wstrd		wr13, [r1], #8

	walignr0	wr10, wr0, wr1			@ ref8
	wavg2b		wr8, wr8, wr10
	wstrd		wr8, [r7]

	wunpckelub	wr11, wr8				@ ref7_0
	wunpckehub	wr10, wr8				@ ref7_1

	wunpckelub	wr14, wr12				@ cur7_0
	wunpckehub	wr13, wr12				@ cur7_1

	wsubh		wr11, wr14, wr11
	wstrd		wr11, [r1], #8
	wsubh		wr13, wr13, wr10
	wstrd		wr13, [r1], #8

    ldmia		sp!, {r5,r6, r7, pc}		@ restore and return
			
   .end