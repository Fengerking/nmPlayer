/************************************************************************
	*																	*
	*		VisualOn, Inc. Confidential and Proprietary, 2003			*
	*																	*
	*		WMMXitrans_LeftHalf(avdUInt8 *outStart, avdNativeInt *coef, 
								avdNativeInt outLnLength,
								avdInt16*	quotiety);					*
	*		IL: issue latency; RL: result latency;						*
	*       RC: resource conflict latency
			IS: instruction scheduling									*	
	*																	*															*
	************************************************************************/

    .section .text
	.align 2
    .global  WMMXitrans_LeftHalf
    .type    WMMXitrans_LeftHalf, function

WMMXItrans:
    @ r0: outStart
    @ r1: coef
    @ r2: utLnLength
	@ r3: quotiety1
@@@@ load coeff
	stmdb		sp!, {r4-r12, lr}		@ save regs used, sp = sp - 40;
	wldrd		wr10,[r1]				@RL(4)
	wldrd		wr11,[r1,#8]			@RL(4)
	mov			r8,#1
@@@wr12-wr15 store the coefficient	@RL(4)
	wldrd		wr12,[r3]				@RL(4)
	wldrd		wr13,[r3,#8]			@RL(4)
	add			r3,r3,#16
	wldrd		wr14,[r3]				@RL(4)
	wldrd		wr15,[r3,#8]			@RL(4)
@@@horizontal
	wmov		wr9,wr10
	

	@@@@@first column
    wmacsz		wr0,wr9,wr12			@RL(2),RC(2)
    tmcr		wcgr2,r8
	wmacsz		wr1,wr9,wr13			@RL(2),RC(2)
	wpackdss	wr0,wr0,wr0
	wmacsz		wr2,wr9,wr14			@RL(2),RC(2)
	wpackdss	wr1,wr1,wr1				@RL(2)
	wmacsz		wr3,wr9,wr15			@RL(2),RC(2)
	
	wpackwss	wr0,wr0,wr0
	wpackwss	wr1,wr1,wr1
	wpackdss	wr2,wr2,wr2				@RL(2)
	wpackdss	wr3,wr3,wr3				@RL(2)
	wpackwss	wr2,wr2,wr2	
	wpackwss	wr3,wr3,wr3
	
	wmov		wr9,wr11
	wsrahg		wr0,wr0,wcgr2
	wsrahg		wr1,wr1,wcgr2
	wsrahg		wr2,wr2,wcgr2
	wsrahg		wr3,wr3,wcgr2

	@@@@@second column
    wmacsz		wr4,wr9,wr12			@RL(2),RC(2)
	add			r3,r3,#16
	wmacsz		wr5,wr9,wr13			@RL(2),RC(2)
	wpackdss	wr4,wr4,wr4
	wmacsz		wr6,wr9,wr14			@RL(2),RC(2)
	wpackdss	wr5,wr5,wr5				@RL(2)
	wmacsz		wr7,wr9,wr15			@RL(2),RC(2)
	
	wldrd		wr14,[r3]				@RL(4)
	wldrd		wr15,[r3,#8]			@RL(4)

	wpackwss	wr4,wr4,wr4
	wpackwss	wr5,wr5,wr5
	wpackdss	wr6,wr6,wr6				@RL(2)
	wpackdss	wr7,wr7,wr7				@RL(2)
	wpackwss	wr6,wr6,wr6	
	wpackwss	wr7,wr7,wr7
	wsrahg		wr4,wr4,wcgr2
	wsrahg		wr5,wr5,wcgr2
	wsrahg		wr6,wr6,wcgr2
	wsrahg		wr7,wr7,wcgr2

	@@@1st column
	wmulsl		wr0,wr0,wr14		
	mov			r8,	#6				@IS
	wmulsl		wr4,wr4,wr15
	waddh		wr8,wr0,wr4
	@@@2nd column
	wmulsl		wr1,wr1,wr14
	tmcr		wcgr1, r8			@IS	
	wmulsl		wr5,wr5,wr15
	waddh		wr9,wr1,wr5
	@@@3rd column
	wmulsl		wr2,wr2,wr14
	mov			r8,	#32			@IS
	wmulsl		wr6,wr6,wr15
	waddh		wr10,wr2,wr6
	@@@4th column
	wmulsl		wr3,wr3,wr14
	tbcsth		wr4,r8				@IS
	wmulsl		wr7,wr7,wr15
	waddh		wr11,wr3,wr7
	
	wsrahg		wr8,wr8,wcgr2
	wsrahg		wr9,wr9,wcgr2
	wsrahg		wr10,wr10,wcgr2
	wsrahg		wr11,wr11,wcgr2

	add			r5,r0,r2
	wldrw		wr0,[r0]
	wldrw		wr1,[r5]			@RL(4)
	add			r6,r5,r2
	add			r7,r6,r2
	wldrw		wr2,[r6]
	wldrw		wr3,[r7]
	
	@@@add 128
	waddh		wr8,wr8,wr4
	waddh		wr9,wr9,wr4
	waddh		wr10,wr10,wr4
	waddh		wr11,wr11,wr4	
	@@@shift 8	
	wsrahg		wr8,wr8,wcgr1		
	wsrahg		wr9,wr9,wcgr1
	wsrahg		wr10,wr10,wcgr1
	wsrahg		wr11,wr11,wcgr1
	
	wunpckelub	wr0,wr0
	wunpckelub	wr1,wr1
	wunpckelub	wr2,wr2
	wunpckelub	wr3,wr3
	
	waddh		wr8,wr8,wr0
	waddh		wr9,wr9,wr1
	waddh		wr10,wr10,wr2
	waddh		wr11,wr11,wr3

	wpackhus	wr8,wr8,wr4
	wpackhus	wr9,wr9,wr4		
	wpackhus	wr10,wr10,wr4	
	wpackhus	wr11,wr11,wr4		
	
	
	wstrw		wr8,[r0]
	wstrw		wr9,[r5]
	wstrw		wr10,[r6]
	wstrw		wr11,[r7]	

ldmia	sp!, {r4-r12, pc} @ restore and return
	
