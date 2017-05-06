
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	*		void wmmxGetBlockNxNLumaInBound(avdUInt8 *refStart,				*
	*			avdNativeInt refLnLength, avdUInt8 *outStart, avdNativeInt  *
	*			outLnLength, avdNativeInt blockSize, avdNativeInt dxP,      *
	*			avdNativeInt dyP)											*
	*			N: 4, 8, 16;												*
	*		IL: issue latency; RL: result latency;							*
	*       RC: resource conflict latency									*
	*																		*															*
	************************************************************************/

    .section .text
	.align 2
    .global  wmmxGetBlockNxNLumaInBound
    .type    wmmxGetBlockNxNLumaInBound, function

wmmxGetBlockNxNLumaInBound:
    @ r0: refStart
    @ r1: refLnLength
    @ r2: outStart
    @ r3: outLnLength
    @ r4, r7: blockSize
    @ r5: dxP
	@ r6: dyP
	pld			[r0]
    stmdb		sp!, {r4-r12, lr}		@ save regs used, sp = sp - 40;
	ldr			r4, [sp, #40]			@ get blockSize, "orignal sp" = "current sp" + 40;
	ldr			r5, [sp, #44]			@ get dxP
	ldr			r6, [sp, #48]			@ get dyP
	mov			r7, r4
	cmp			r4, #4
	beq			L_GB4x4LumaInBound
	@ check if dxP = dyP = 0
	orrs		r8, r5, r6
	bne			L_GBNonFullPelLumaInBound

	@@@@@ 8x8 or 16x16 Full Pel start;
	@ data alignment;
	and			r8, r0, #7
	bic			r0, r0, #7
	@ preload data to avoid RL;
	wldrd		wr0, [r0]			@ RL(4)
	wldrd		wr1, [r0, #8]		@ RL(4)
	tmcr		wcgr0, r8			@ RL(3)
	L_GBFullPelLumaInBoundLoop:
		cmp			r4, #16
		wldrdeq		wr3, [r0, #16]		@ RL(4), load 16-23
		add			r0, r0, r1			@ ref jumps to 2nd line
		walignr0	wr2, wr0, wr1
		wldrd		wr5, [r0]			@ RL(4), load 2nd line 0-7
		wstrd		wr2, [r2]			@ RC(2), save 0-7
		wldrd		wr6, [r0, #8]		@ RL(4), load 2nd line 8-15

		walignr0eq	wr4, wr1, wr3
		wstrdeq		wr4, [r2, #8]		@ RC(2), save 8-15
		wldrdeq		wr8, [r0, #16]		@ RL(4), load 2nd line 16-23
		add			r2, r2, r3			@ output jumps to 2nd line;
		walignr0	wr7, wr5, wr6
		wstrd		wr7, [r2]			@ RC(2), save 2nd line 0-7
		walignr0eq	wr7, wr6, wr8
		wstrdeq		wr7, [r2, #8]		@ RC(2)

		subs		r7, r7, #2
		add			r0, r0, r1
		wldrdne		wr0, [r0]			@ RL(4)
		add			r2, r2, r3
		wldrdne		wr1, [r0, #8]		@ RL(4)
		bne			L_GBFullPelLumaInBoundLoop
    ldmia		sp!, {r4-r12, pc}		@ restore and return
	@@@@@ 8x8 or 16x16 Full Pel end;

L_GBNonFullPelLumaInBound:
	@ data alignment; need x_pos -2 to x_pos + 10/18 for 8x8/16x16
	cmp			r6, #0
	bne			L_GBNonZeroDyPLumaInBound
L_GBReuseDyp0:
	sub			r0, r0, #2			@ x_pos - 2;
	and			r8, r0, #7
	tmcr		wcgr0, r8			@ RL(3)
	bic			r0, r0, #7
	@ preload data to avoid RL;
	wldrd		wr0, [r0]			@ RL(4)
	wldrd		wr1, [r0, #8]		@ RL(4)
	@ put [-5(0xfffb), -5, -5, -5] and [20, 20, 20, 20] into wr14/15
	mvn			r8, #4
	tbcsth		wr14, r8			@ [-5(0xfffb), -5, -5, -5]
	mov			r8, #20
	tbcsth		wr15, r8			@ [20, 20, 20, 20]

	@@@@@ 8x8 or 16x16 dyP == 0, No vertical interpolation start;
	mov			r8, #5
	tmcr		wcgr1, r8			@ for shifting;
	@@@@@ 8x8 or 16x16 dyP == 0, No vertical interpolation start;
	cmp			r5, #3
	moveq		r8, #3
	movne		r8, #2
	tmcr		wcgr2, r8			@ for shifting if (dxP&1);
	@ put 0x0010001000100010 into wr13
	mov			r8, #16
	tbcsth		wr13, r8			@ 0x0010001000100010
	L_GBZeorDyPLumaInBoundLoop:
		pld			[r0, r1]
		cmp			r4, #16
		walignr0	wr5, wr0, wr1		@ -2 to 5
		wunpckelub	wr6, wr5			@ -2 to 1 
		wunpckehub	wr7, wr5			@ 2 to 5 ---saved
		waligni		wr8, wr6, wr7, #2   @ -1 to 2 
		wmulsl		wr9, wr8, wr14		@ RC(2), -5 * (-1 to 2)
		waligni		wr10, wr6, wr7, #4  @ 0 to 3 
		wldrd		wr2, [r0, #16]		@ RL(4), load unaligned 16-23 ---saved (until 16x16);
		wmulsl		wr11, wr10, wr15	@ RC(2), 20 * (0 to 3)
		waddh		wr0, wr9, wr6		@ (-2 to 1) - 5 * (-1 to 2)
		waligni		wr4, wr6, wr7, #6   @ 1 to 4 
		wmulsl		wr9, wr4, wr15		@ RC(2), 20 * (1 to 4) 

		walignr0	wr6, wr1, wr2		@ 6 to 13 ---saved (until 16x16)
		walignr2	wr3, wr5, wr6		@ 0to7 if dxP=1, 1to8 if dxP=3;---saved in case (dxP&1) 

		waddh		wr0, wr0, wr11		@ (-2 to 1) - 5 * (-1 to 2) + 20 * (0 to 3)
		wmulsl		wr5, wr7, wr14		@ RC(2), -5 * (2 to 5) 
		waddh		wr0, wr0, wr9		@ (-2to1) - 5*(-1to2) + 20*(0to3) + 20*(1to4)
		waddh		wr0, wr0, wr5		@ (-2to1)-5*(-1to2)+20*(0to3)+20*(1to4)-5*(2to5)

		wunpckelub	wr11, wr6			@ 6 to 9 ---saved (until 16x16)
		wunpckehub	wr1, wr6			@ 10 to 13 ---saved (until 16x16)
		waligni		wr12, wr7, wr11, #2 @ 3 to 6
		waddh		wr0, wr0, wr12		@ [0 to 3], 1 -5 20 20 -5 1 done;

		wmulsl		wr12, wr12, wr14	@ RC(2), -5 * (3to6)
		waligni		wr5, wr7, wr11, #4  @ 4 to 7
		wmulsl		wr5, wr5, wr15		@ RC(2), 20 * (4 to 7) 
		waligni		wr4, wr7, wr11, #6  @ 5 to 8
		wmulsl		wr4, wr4, wr15	    @ RC(2), 20 * (5 to 8) 
		waddh		wr12, wr12, wr7		@ (2to5) - 5 * (3to6)
		wmulsl		wr7, wr11, wr14	    @ RC(2), -5 * (6 to 9)
		waddh		wr12, wr12, wr5  	@ (2to5) - 5 * (3to6) + 20 * (4to7)
		waddh		wr12, wr12, wr4 	@ (2to5) - 5 * (3to6) + 20 * (4to7) + 20 * (5to8)
		waddh		wr12, wr12, wr7		@ 2to5) - 5*(3to6) + 20*(4to7) + 20*(5to8) -5*(6to9)
		waligni		wr4, wr11, wr1, #2  @ 7 to 10
		waddh		wr12, wr12, wr4		@ [4 to 7], 1 -5 20 20 -5 1 done;
		waddh		wr0, wr0, wr13		@ add 16
		waddh		wr12, wr12, wr13	@ add 16
		wsrahg		wr0, wr0, wcgr1		@ shift 5
		wsrahg		wr12, wr12, wcgr1	@ shift 5
		wpackhus	wr0, wr0, wr12		@ RL(2), [0 to 7] ready;
		bne	L_GBZeorDyPLumaInBoundSave		

		@ calculate 8-15 for 16x16;
		@ wr0:  calculated [0 to 7], do not overwrite it;
		@ wr3:  original [0 to 7], do not overwrite it;
		@ wr2:  load unaligned 16 to 23;
		@ wr11: original [6 to 9]
		@ wr1:  original [10 to 13]
		@ wr4:  original [7 to 10]
		@ wr6:  original [6 to 13]

		wldrd		wr12, [r0, #24]		@ RL(4), load unaligned 24-32
		wmulsl		wr4, wr4, wr14		@ RC(2), -5 * (7to10)
		waligni		wr5, wr11, wr1, #4  @ 8 to 11
		wmulsl		wr5, wr5, wr15		@ RC(2), 20 * (8to11)
		waddh		wr4, wr4, wr11		@ (6to9) - 5 * (7to10)

		walignr0	wr10, wr2, wr12		@ 14 to 21
		ands		r9, r5, #1			@ (dxP&1) 
		walignr2ne	wr9, wr6, wr10		@ 8to15 if dxP=1, 9to16 if dxP=3;---saved in case (dxP&1) 
		wunpckelub	wr8, wr10			@ 14 to 17
		wunpckehub	wr10, wr10			@ 15 to 18 

		waligni		wr6, wr11, wr1, #6  @ 9 to 12
		wmulsl		wr6, wr6, wr15		@ RC(2), 20 * (9to12)
		waddh		wr4, wr4, wr5		@ (6to9) - 5 * (7to10) + 20 * (8to11)
		wmulsl		wr5, wr1, wr14		@ RC(2), -5 * (10to13)
		waddh		wr4, wr4, wr6		@ (6to9) - 5 * (7to10) + 20 * (8to11) + 20 * (9to12)
		waddh		wr4, wr4, wr5		@ (6to9)-5*(7to10)+20*(8to11)+20*(9to12)-5*(10to13)
		waligni		wr5, wr1, wr8, #2   @ 11 to 14
		waddh		wr4, wr4, wr5		@ [8 to 11], 1 -5 20 20 -5 1 done;

		wmulsl		wr5, wr5, wr14		@ RC(2), -5 * (11to14)
		waligni		wr6, wr1, wr8, #4   @ 12 to 15
		wmulsl		wr6, wr6, wr15		@ RC(2), 20 * (12to15)
		waddh		wr5, wr5, wr1		@ (10to13) - 5 * (11to14)
		waligni		wr11, wr1, wr8, #6  @ 13 to 16
		wmulsl		wr11, wr11, wr15	@ RC(2), 20 * (13to16)
		waddh		wr5, wr5, wr6		@ (10to13) - 5 * (11to14) + 20 * (12to15)
		wmulsl		wr6, wr8, wr14		@ RC(2), -5 * (14to17)
		waddh		wr5, wr5, wr11		@ (10to13)-5*(11to14)+20*(12to15)+20*(13to16)
		waddh		wr5, wr5, wr6		@ (10to13)-5*(11to14)+20*(12to15)+20*(13to16)-5*(14to17)
		waligni		wr6, wr8, wr10, #2  @ 15 to 18
		waddh		wr5, wr5, wr6		@ [12 to 15], 1 -5 20 20 -5 1 done;

		waddh		wr4, wr4, wr13		@ add 16
		waddh		wr5, wr5, wr13		@ add 16
		wsrahg		wr4, wr4, wcgr1		@ shift 5
		wsrahg		wr5, wr5, wcgr1		@ shift 5
		wpackhus	wr4, wr4, wr5		@ RL(2), [8 to 15] ready;
		wavg2brne	wr4, wr4, wr9		@ avg with the orignal value, if (dxP&1);
		wstrd		wr4, [r2, #8]		@ RC(2), save 0-7

	L_GBZeorDyPLumaInBoundSave:
		ands		r9, r5, #1			@ (dxP&1) 
		wavg2brne	wr0, wr0, wr3		@ avg with the orignal value;
		wstrd		wr0, [r2]			@ RC(2), save 0-7

		subs		r7, r7, #1
		add			r0, r0, r1
		wldrdne		wr0, [r0]			@ RL(4)
		wldrdne		wr1, [r0, #8]		@ RL(4)
		add			r2, r2, r3
		bne			L_GBZeorDyPLumaInBoundLoop
	@@@if call entry is L_GBReuseDyp0,the r6!=0,then return back to caller 
	cmp			r6,#0
	bne			L_GBDxp2orDyp2GoOn1
    ldmia		sp!, {r4-r12, pc}		@ restore and return
	@@@@@ 8x8 or 16x16 dyP == 0, No vertical interpolation end;

L_GBNonZeroDyPLumaInBound:
	cmp			r5, #0
	bne			L_GBNonZeroDxPLumaInBound
	@@@@@ put [-5(0xfffb), -5, -5, -5] and [20, 20, 20, 20] into wr14/15
	mvn			r8, #4
	tbcsth		wr14, r8			@ [-5(0xfffb), -5, -5, -5]
	mov			r8, #20
	tbcsth		wr15, r8			@ [20, 20, 20, 20]
	@ for shifting;
	mov			r8, #5
	tmcr		wcgr1, r8
L_GBReuseDxp0:
	@@@@@ 8x8 or 16x16 dxP == 0, No horizontal interpolation start;
	and			r8, r0, #7
	tmcr		wcgr0, r8			@ RL(3)
	bic			r0, r0, #7
	@@@@@	preload 6 cache lines
	sub			r0, r0, r1,lsl #1	@  y_pos - 2*refLnLength;
	mov			r10,r0
	pld			[r10,r1]	
	add			r10,r10,r1
	pld			[r10,r1]
	add			r10,r10,r1
	pld			[r10,r1]
	add			r10,r10,r1
	pld			[r10,r1]
	add			r10,r10,r1
	pld			[r10,r1]
	add			r10,r10,r1
	pld			[r10,r1]
	add			r10,r10,r1
	pld			[r10,r1]
	mov			r9,r10				@save the 5th line address

	@@@@ put 16 to wr11
	mov			r11, #16
	tbcsth		wr11, r11	
	
	@ preload (yPos-1) line to avoid RL;
	add			r10, r0,r1
	wldrd		wr0, [r10]			@ RL(4)
	wldrd		wr1, [r10, #8]		@ RL(4)
	
	mov			r11,r0 @temp save
	mov			r12,r2 @temp save
	cmp			r4,#16
	moveq		r8,#1				@16x16 control jump
	L_GBZeroDxPLumaInBoundLoop:
	
	
	@ load (yPos) line to avoid RL;
	add			r10, r10,r1
	wldrd		wr2, [r10]			@ RL(4)
	wldrd		wr3, [r10, #8]		@ RL(4)
	
	walignr0	wr1,wr0,wr1			@wr1 store the (yPos-1) line
	wunpckelub	wr6, wr1			@  
	wmulsl		wr4,wr6, wr14		@wr4 store (0 to 3)*-5	
	wunpckehub	wr7, wr1			
	wmulsl		wr5,wr7, wr14		@wr5 store (4 to 7)*-5 
	
	@ preload (yPos+1) line to avoid RL;
	add			r10, r10,r1
	wldrd		wr0, [r10]			@ RL(4)
	wldrd		wr1, [r10, #8]		@ RL(4)

	walignr0	wr2,wr2,wr3			@wr2 store the (yPos) line
	cmp			r6,#1
	wmoveq		wr10,wr2			@save the ypos line if r6==1
	wunpckehub	wr9, wr2			
	wmulsl		wr3, wr9, wr15		@wr3 store (4 to 7)*20
	wunpckelub	wr8, wr2			
	wmulsl		wr2,wr8, wr15		@wr2 store (0 to 3)*20
	@add the (y-1)*-5 + y* line
	waddh		wr4,wr4,wr2			@wr4=(0 to 3)[*(-5)+*20]
	waddh		wr5,wr5,wr3			@wr5=(4 to 7)[*(-5)+*20]
	@ for next loop:add the (y-2) +(y-1)*(-5)
	wmulsl		wr2, wr8, wr14		@wr2 store (0 to 3)*(-5)
	waddh		wr12,wr2, wr6		@wr6 store (0 to 3)*1
	wmulsl		wr3, wr9, wr14		@wr3 store (4 to 7)*(-5)
	waddh		wr13,wr3, wr7		@wr7 store (0 to 3)*1

	@ preload (yPos+2) line to avoid RL;
	add			r10, r10,r1
	wldrd		wr2, [r10]			@ RL(4)
	wldrd		wr3, [r10, #8]		@ RL(4)

	walignr0	wr1,wr0,wr1			@wr1 store the (yPos+1) line
	wmoveq		wr9,wr1				@for next loop:if r6==1,save the line to wr9 for average
	cmp			r6,#3
	wmoveq		wr10,wr1			@save the ypos+1 line if r6==3
	wunpckelub	wr6, wr1			@  
	wmulsl		wr0,wr6, wr15		@ (0 to 3)*20	
	wunpckehub	wr7, wr1			
	wmulsl		wr1,wr7, wr15		@ (4 to 7)*20 
	@add (yPos+1)*20  
	waddh		wr4,wr0,wr4
	waddh		wr5,wr1,wr5
	@ for next loop:add the yPos*20 
	waddh		wr12,wr0,wr12
	waddh		wr13,wr1,wr13
	
	@ preload (yPos-2) line to avoid RL;
	wldrd		wr0, [r11]			@ RL(4)
	wldrd		wr1, [r11, #8]		@ RL(4)

	walignr0	wr2,wr2,wr3			@wr2 store the (yPos+2) line
	wmoveq		wr9,wr2				@for next loop:if r6==3,save the line to wr9 for average
	wunpckehub	wr7,wr2			
	wmulsl		wr3,wr7, wr14		@ (4 to 7)*-5
	wunpckelub	wr6,wr2			
	wmulsl		wr2,wr6, wr14		@ (0 to 3)*-5
	@ add (ypos+2) line
	waddh		wr4,wr4,wr2			
	waddh		wr5,wr5,wr3
	@ for next loop:add the (yPos+1)*20 
	wmulsl		wr2, wr6, wr15		@ (0 to 3)*20
	waddh		wr12,wr12,wr2
	wmulsl		wr3, wr7, wr15		@ (4 to 7)*20
	waddh		wr13,wr13,wr3
	
	@ preload (yPos+3) line to avoid RL;
	add			r10, r10,r1 
	wldrd		wr2, [r10]			@ RL(4)
	wldrd		wr3, [r10, #8]		@ RL(4)
	
	walignr0	wr1,wr0,wr1			@wr1 store the (yPos-2) line
	wunpckelub	wr6, wr1			@  
	wunpckehub	wr7, wr1
	@add (yPos-2)
	waddh		wr4,wr4,wr6			@(0 to 3) five lines
	waddh		wr5,wr5,wr7			@(4 to 7) five lines
	
	@ for next loop:preload (yPos+3) line to avoid RL;
	add			r10, r10,r1
	wldrd		wr0, [r10]			@ RL(4)
	wldrd		wr1, [r10, #8]		@ RL(4)
	
	walignr0	wr2,wr2,wr3			@wr2 store the (yPos+3) line
	wunpckelub	wr6, wr2			@  
	wunpckehub	wr7, wr2
	@add (yPos+3)
	waddh		wr4,wr4,wr6			@(0 to 3)done
	waddh		wr5,wr5,wr7			@(4 to 7)done
	@for next loop:add (yPos+2)*-5
	wmulsl		wr2, wr6, wr14		@ (0 to 3)*-5
	waddh		wr12,wr12,wr2
	wmulsl		wr3, wr7, wr14		@ (4 to 7)*-5
	waddh		wr13,wr13,wr3
	
	walignr0	wr1,wr0,wr1			@for next loop:wr1 store the (yPos+3) line
	wunpckelub	wr6, wr1
	wunpckehub	wr7, wr1
	@for next loop:add(yPos+3)
	waddh		wr12,wr12,wr6		@for next loop:(0 to 3)done
	waddh		wr13,wr13,wr7		@for next loop:(4 to 7)done
	
	@ Clip3((tempLine+16)>>5)
	waddh		wr4, wr4, wr11		@ add 16
	waddh		wr5, wr5, wr11		@ add 16
	wsrahg		wr4, wr4, wcgr1		@ shift 5
	wsrahg		wr5, wr5, wcgr1		@ shift 5
	wpackhus	wr4, wr4, wr5		@ RL(2), [0 to 7] ready;
	
	@for next loop:Clip3((tempLine+16)>>5) 
	waddh		wr12, wr12, wr11		@ add 16
	waddh		wr13, wr13, wr11		@ add 16
	wsrahg		wr12, wr12, wcgr1		@ shift 5
	wsrahg		wr13, wr13, wcgr1		@ shift 5
	wpackhus	wr12, wr12, wr13		@ RL(2), [0 to 7] ready;
	
	@@@if r5!=0,that means the branch is reused by another branch
	cmp			r5,#0
	beq			SavetoOutput
	wldrd		wr5,[r12]			@load the current output line
	add			r11,r12,r3
	wldrd		wr13, [r11]         @load second line,RL(4)
	
	cmp			r4,#16				@instruction scheduling:elminate RL of wldrd
	cmpeq		r8,#1
	
	wavg2br		wr4,  wr4, wr5		@ avg with the orignal value;
	wavg2br		wr12, wr12,wr13		@ avg with the orignal value;
	wstrd		wr4,  [r12]			@ RC(2), save 0-7
	wstrd		wr12, [r11]			@ RC(2), save 0-7
	
	bne			dxpZeroLoopControl	@if r4!=16 && r8!=1,jump to dxpZeroLoopControl
	beq			GoOn16x16	
	@@@save to
	SavetoOutput: 
	ands		r11, r6, #1			@ (dyP&1) 
	wavg2brne	wr4,  wr4, wr10		@ avg with the orignal value;
	wavg2brne	wr12, wr12,wr9		@ avg with the orignal value;
	add			r11, r12, r3
	wstrd		wr4,  [r12]			@ RC(2), save 0-7
	wstrd		wr12, [r11]			@ RC(2), save 0-7			
	
	cmp			r4,#16
	cmpeq		r8,#1
	bne			dxpZeroLoopControl
	@@@@16x16	
	GoOn16x16:
	add			r11,r0,#8			@r11 refer r0+8
	add			r10,r11,r1			@r10 refer (yPos-1) line
	@preload (yPos-1) line						
	wldrd		wr0, [r10]			@ RL(4)
	wldrd		wr1, [r10, #8]		@ RL(4)
	add			r12,r2,#8			@r12 refer r2+8
	sub			r8,r8,#1
	b			L_GBZeroDxPLumaInBoundLoop
	
	dxpZeroLoopControl:
	cmp			r4,#16
	moveq		r8,#1				@16x16 control jump		
	subs		r7, r7, #2
	addne		r2, r2, r3, lsl #1  @go to next two output lines
	addne		r0, r0, r1, lsl #1  @go to next two lines
	addne		r10,r0, r1			@r10 refer the (yPos-1) line
	@preload (yPos-1) line						
	wldrdne		wr0, [r10]			@ RL(4)
	wldrdne		wr1, [r10, #8]		@ RL(4)
	addne		r9, r9, r1			@ r9,the 6th line
	mov			r11,r0 @temp save
	mov			r12,r2 @temp save
	pld			[r9,r1]				@preload one line
	bne			L_GBZeroDxPLumaInBoundLoop
	ldmia		sp!, {r4-r12, pc}		@ restore and return
L_GBNonZeroDxPLumaInBound:@dxp!=2 and dyp!=2
	
	@temp save
	mov			r11,r0
	mov			r12,r2
	mov			r10,r5
	@@@if dyp==3,r0=r0+one line
	cmp			r6,#3
	addeq		r0,r0,r1
	mov			r5,#2		@only do halfPel-calculations
	
	@@@do vertical half-interpolation calculations,reuse the branch 	
	b			L_GBReuseDyp0
	L_GBDxp2orDyp2GoOn1:
	@restore the r0,r2,r5
	mov			r0,r11
	mov			r2,r12
	mov			r5,r10
	mov			r7,r4
	@@@if dxp==3,r0=r0+1
	cmp			r5,#3
	addeq		r0,r0,#1
	mov			r6,#2
	b			L_GBReuseDxp0
	ldmia		sp!, {r4-r12, pc}		@ restore and return

L_GB4x4LumaInBound:
	orrs		r8, r5, r6
	bne			L_GB4x4NonFullPelLumaInBound

	@@@@@ 4x4 Full Pel start;
	@ from ARM Arch Ref Manual A4-37, r0 does not need to be word aligned.
	@ However, it crashed for mis-alignment;
	ldrb		r5, [r0, #1]
	ldrb		r6, [r0, #2]
	ldrb		r8, [r0, #3]
	ldrb		r4, [r0], r1					@ RL(3), Load it byte by byte;
	L_GB4x4FullPelLumaInBound:
		pld			[r0, r1]
		subs		r7, r7, #2
		@ start 2nd line to avoid RL
		ldrb		r9, [r0, #1]
		orr			r4, r4, r5, LSL #8
		ldrb		r10, [r0, #2]
		orr			r4, r4, r6, LSL #16
		ldrb		r11, [r0, #3]
		orr			r4, r4, r8, LSL #24
		ldrb		r12, [r0], r1				@ RL(3), Load it byte by byte;
		str			r4, [r2], r3
		ldrb		r5, [r0, #1]
		orr			r12, r12, r9, LSL #8
		ldrb		r6, [r0, #2]
		orr			r12, r12, r10, LSL #16
		ldrb		r8, [r0, #3]
		orr			r12, r12, r11, LSL #24
		ldrb		r4, [r0], r1				@ RL(3), Load it byte by byte;
		str			r12, [r2], r3
		bne			L_GB4x4FullPelLumaInBound
    ldmia		sp!, {r4-r12, pc}		@ restore and return
	@@@@@ 4x4 Full Pel end;
L_GB4x4NonFullPelLumaInBound:
	cmp			r6, #0
	bne			L_GB4x4NonZeroDyPLumaInBound
L_GB4x4ReuseDyp0:
	@@@@@ 4x4 dyP == 0, No vertical interpolation start;
	@ data alignment; need x_pos -2 to x_pos + 10/18 for 8x8/16x16
	sub			r0, r0, #2			@ x_pos - 2;
	and			r8, r0, #7
	tmcr		wcgr0, r8			@ RL(3)
	bic			r0, r0, #7
	@ preload data to avoid RL;
	wldrd		wr0, [r0]			@ RL(4)
	wldrd		wr1, [r0, #8]		@ RL(4)
	@ put [-5(0xfffb), -5, -5, -5] and [20, 20, 20, 20] into wr14/15
	mvn			r8, #4
	tbcsth		wr14, r8			@ [-5(0xfffb), -5, -5, -5]
	mov			r8, #20
	tbcsth		wr15, r8			@ [20, 20, 20, 20]

	@@@@@ 8x8 or 16x16 dyP == 0, No vertical interpolation start;
	mov			r8, #5
	tmcr		wcgr1, r8			@ for shifting;
	@@@@@ 8x8 or 16x16 dyP == 0, No vertical interpolation start;
	cmp			r5, #3
	moveq		r8, #3
	movne		r8, #2
	tmcr		wcgr2, r8			@ for shifting if (dxP&1);
	@ put 0x0010001000100010 into wr13
	mov			r8, #16
	tbcsth		wr13, r8			@ 0x0010001000100010
	L_GB4x4ZeorDyPLumaInBoundLoop:
		pld			[r0, r1]
		walignr0	wr5, wr0, wr1		@ -2 to 5
		wunpckelub	wr6, wr5			@ -2 to 1 
		wunpckehub	wr7, wr5			@ 2 to 5 ---saved
		waligni		wr8, wr6, wr7, #2   @ -1 to 2 
		wmulsl		wr9, wr8, wr14		@ RC(2), -5 * (-1 to 2)
		waligni		wr10, wr6, wr7, #4  @ 0 to 3 
		wldrw		wr2, [r0, #16]		@ RL(4), load unaligned 16-19;
		wmulsl		wr11, wr10, wr15	@ RC(2), 20 * (0 to 3)
		waddh		wr0, wr9, wr6		@ (-2 to 1) - 5 * (-1 to 2)
		waligni		wr4, wr6, wr7, #6   @ 1 to 4 
		wmulsl		wr9, wr4, wr15		@ RC(2), 20 * (1 to 4) 

		walignr0	wr6, wr1, wr2		@ 6 to 9
		walignr2	wr3, wr5, wr6		@ 0to7 if dxP=1, 1to8 if dxP=3;---saved in case (dxP&1) 

		waddh		wr0, wr0, wr11		@ (-2 to 1) - 5 * (-1 to 2) + 20 * (0 to 3)
		wmulsl		wr5, wr7, wr14		@ RC(2), -5 * (2 to 5) 
		waddh		wr0, wr0, wr9		@ (-2to1) - 5*(-1to2) + 20*(0to3) + 20*(1to4)
		waddh		wr0, wr0, wr5		@ (-2to1)-5*(-1to2)+20*(0to3)+20*(1to4)-5*(2to5)

		wunpckelub	wr11, wr6			@ 6 to 9
		waligni		wr12, wr7, wr11, #2 @ 3 to 6
		waddh		wr0, wr0, wr12		@ [0 to 3], 1 -5 20 20 -5 1 done;

		waddh		wr0, wr0, wr13		@ add 16
		wsrahg		wr0, wr0, wcgr1		@ shift 5
		wpackhus	wr12, wr0, wr12		@ RL(2), [0 to 3] ready;
		ands		r9, r5, #1			@ (dxP&1) 
		wavg2brne	wr12, wr12, wr3		@ avg with the orignal value;

		subs		r7, r7, #1
		add			r0, r0, r1
		wldrdne		wr0, [r0]			@ RL(4)
		wldrdne		wr1, [r0, #8]		@ RL(4)
		wstrw		wr12, [r2]			@ RC(2), save 0-7
		add			r2, r2, r3
		bne			L_GB4x4ZeorDyPLumaInBoundLoop
	@@@if call entry is L_GBReuseDyp0,the r6!=0,then return back to caller 
	cmp			r6,#0
	bne			L_GB4x4Dxp2orDyp2GoOn1
    ldmia		sp!, {r4-r12, pc}		@ restore and return
	@@@@@ 4x4 dyP == 0, No vertical interpolation end;
L_GB4x4NonZeroDyPLumaInBound:
	cmp			r5, #0
	bne			L_GB4x4NonZeroDxPLumaInBound
	@@@@@ put [-5(0xfffb), -5, -5, -5] and [20, 20, 20, 20] into wr14/15
	mvn			r8, #4
	tbcsth		wr14, r8			@ [-5(0xfffb), -5, -5, -5]
	mov			r8, #20
	tbcsth		wr15, r8			@ [20, 20, 20, 20]
	@ for shifting;
	mov			r8, #5
	tmcr		wcgr1, r8
L_GB4x4ReuseDxp0:
	@@@@@ 4x4 dxP == 0, No horizontal interpolation start;
	and			r8, r0, #7
	tmcr		wcgr0, r8				@ RL(3)
	bic			r0, r0, #7
	@@@@@	preload 6 cache lines
	sub			r0, r0, r1,lsl #1	@  y_pos - 2*refLnLength;
	mov			r10,r0
	pld			[r10,r1]	
	add			r10,r10,r1
	pld			[r10,r1]
	add			r10,r10,r1
	pld			[r10,r1]
	add			r10,r10,r1
	pld			[r10,r1]
	add			r10,r10,r1
	pld			[r10,r1]
	add			r10,r10,r1
	pld			[r10,r1]
	add			r9,r10,r1				@save the 6th line address
	

	@ preload (yPos-1) line to avoid RL;
	add			r10, r0,r1
	wldrd		wr0, [r10]			@ RL(4)
	wldrd		wr1, [r10, #8]		@ RL(4)
	
	@@@@ put 16 to wr11
	mov			r8, #16
	tbcsth		wr11, r8
	L_GB4x4ZeroDxPLumaInBoundLoop:@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@loop
	pld			[r9,r1]
	@ load (yPos) line to avoid RL;
	add			r10, r10,r1
	wldrd		wr2, [r10]			@ RL(4)
	wldrd		wr3, [r10, #8]		@ RL(4)
	
	walignr0	wr1,wr0,wr1			@wr1 store the (yPos-1) line
	wunpckelub	wr6, wr1			@wr6 store the (yPos-1) line 
	wmulsl		wr4,wr6, wr14		@wr4 store (0 to 3)*-5	
	
	@ preload (yPos+1) line to avoid RL;
	add			r10, r10,r1
	wldrd		wr0, [r10]			@ RL(4)
	wldrd		wr1, [r10, #8]		@ RL(4)

	walignr0	wr2,wr2,wr3			@wr2 store the (yPos) line
	cmp			r6,#1
	wmoveq		wr10,wr2			@save the ypos line if r6==1
	wunpckelub	wr8, wr2			
	wmulsl		wr2,wr8, wr15		@wr2 store (0 to 3)*20
	@add the (y-1)*-5 + y* line
	waddh		wr4,wr4,wr2			@wr4=(0 to 3)[*(-5)+*20]
	@ for next loop:add the (y-2) +(y-1)*(-5)
	wmulsl		wr2, wr8, wr14		@wr2 store (0 to 3)*(-5)
	waddh		wr12,wr2, wr6		@wr6 store (0 to 3)*1
	
	@ preload (yPos+2) line to avoid RL;
	add			r10, r10,r1
	wldrd		wr2, [r10]			@ RL(4)
	wldrd		wr3, [r10, #8]		@ RL(4)

	walignr0	wr1,wr0,wr1			@wr1 store the (yPos+1) line
	wmoveq		wr9,wr1				@for next loop:if r6==1,save the line to wr9 for average
	cmp			r6,#3
	wmoveq		wr10,wr1			@save the ypos+1 line if r6==3
	wunpckelub	wr6, wr1			@  
	wmulsl		wr0,wr6, wr15		@ (0 to 3)*20	
	@add (yPos+1)*20  
	waddh		wr4,wr0,wr4
	@ for next loop:add the yPos*20 
	waddh		wr12,wr0,wr12
	
	@ preload (yPos-2) line to avoid RL;
	wldrd		wr0, [r0]			@ RL(4)
	wldrd		wr1, [r0, #8]		@ RL(4)

	walignr0	wr2,wr2,wr3			@wr2 store the (yPos+2) line
	wmoveq		wr9,wr2				@for next loop:if r6==3,save the line to wr9 for average
	wunpckelub	wr6,wr2			
	wmulsl		wr2,wr6, wr14		@ (0 to 3)*-5
	@ add (ypos+2) line
	waddh		wr4,wr4,wr2			
	@ for next loop:add the (yPos+1)*20 
	wmulsl		wr2, wr6, wr15		@ (0 to 3)*20
	waddh		wr12,wr12,wr2
	@ preload (yPos+3) line to avoid RL;
	add			r10, r10,r1 
	wldrd		wr2, [r10]			@ RL(4)
	wldrd		wr3, [r10, #8]		@ RL(4)
	
	walignr0	wr1,wr0,wr1			@wr1 store the (yPos-2) line
	wunpckelub	wr6, wr1			@  
	@add (yPos-2)
	waddh		wr4,wr4,wr6			@(0 to 3) five lines
	
	@ for next loop:preload (yPos+3) line to avoid RL;
	add			r10, r10,r1
	wldrd		wr0, [r10]			@ RL(4)
	wldrd		wr1, [r10, #8]		@ RL(4)
	
	walignr0	wr2,wr2,wr3			@wr2 store the (yPos+3) line
	wunpckelub	wr6, wr2			@  
	@add (yPos+3)
	waddh		wr4,wr4,wr6			@(0 to 3)done
	@for next loop:add (yPos+2)*-5
	wmulsl		wr2, wr6, wr14		@ (0 to 3)*-5
	waddh		wr12,wr12,wr2

	walignr0	wr1,wr0,wr1			@for next loop:wr1 store the (yPos+3) line
	wunpckelub	wr6, wr1
	@for next loop:add(yPos+3)
	waddh		wr12,wr12,wr6		@for next loop:(0 to 3)done
	
	@ Clip3((tempLine+16)>>5)
	waddh		wr4, wr4, wr11		@ add 16
	wsrahg		wr4, wr4, wcgr1		@ shift 5
	wpackhus	wr4, wr4, wr4		@ RL(2), [0 to 7] ready;
	
	@for next loop:Clip3((tempLine+16)>>5) 
	waddh		wr12, wr12, wr11		@ add 16
	wsrahg		wr12, wr12, wcgr1		@ shift 5
	wpackhus	wr12, wr12, wr12		@ RL(2), [0 to 7] ready;
	
	@@@if r5!=0,that means the branch is reused by another branch
	cmp			r5,#0
	beq			SavetoOutput4x4
	wldrw		wr5,[r2]			@load the current output line
	add			r11,r2,r3
	wldrw		wr13, [r11]         @load second line,RL(4)
	mov			r1,r1				@eliminate one rl
	wavg2br		wr4,  wr4, wr5		@ avg with the orignal value;
	wavg2br		wr12, wr12,wr13		@ avg with the orignal value;
	wstrw		wr4,  [r2]			@ RC(2), save 0-7
	wstrw		wr12, [r11]			@ RC(2), save 0-7
	b		dxp4x4ZeroLoopControl	

	SavetoOutput4x4: 
	@@@save to 
	ands		r10, r6, #1			@ (dyP&1) 
	wavg2brne	wr4,  wr4, wr10		@ avg with the orignal value;
	wavg2brne	wr12, wr12,wr9		@ avg with the orignal value;
	wstrw		wr4,  [r2]			@ RC(2), save 0-7
	add			r12, r2, r3
	wstrw		wr12, [r12]			@ RC(2), save 0-7			
dxp4x4ZeroLoopControl:	
	subs		r7, r7, #2
	addne		r0, r0, r1, lsl #1  @go to next two lines
	addne		r10,r0, r1			@r10 refer the (yPos-1) line
	@preload (yPos-1) line						
	wldrdne		wr0, [r10]			@ RL(4)
	wldrdne		wr1, [r10, #8]		@ RL(4)
	addne		r9, r9, r1			@ r9,the 6th line
	addne		r2, r2, r3, lsl #1  @go to next two output lines

	bne			L_GB4x4ZeroDxPLumaInBoundLoop
	ldmia		sp!, {r4-r12, pc}		@ restore and return

L_GB4x4NonZeroDxPLumaInBound:@dxp!=2 and dyp!=2
	@temp save
	mov			r11,r0
	mov			r12,r2
	mov			r10,r5
	@@@if dyp==3,r0=r0+one line
	cmp			r6,#3
	addeq		r0,r0,r1
	mov			r5,#2		@only do halfPel-calculations
	
	@@@do vertical half-interpolation calculations,reuse the branch 	
	b			L_GB4x4ReuseDyp0
	L_GB4x4Dxp2orDyp2GoOn1:
	@restore the r0,r2,r5
	mov			r0,r11
	mov			r2,r12
	mov			r5,r10
	mov			r7,r4
	@@@if dxp==3,r0=r0+1
	cmp			r5,#3
	addeq		r0,r0,#1
	mov			r6,#2
	b			L_GB4x4ReuseDxp0
	ldmia		sp!, {r4-r12, pc}		@ restore and return
L_GBLumaInBoundDone:
    ldmia		sp!, {r4-r12, pc}		@ restore and return
