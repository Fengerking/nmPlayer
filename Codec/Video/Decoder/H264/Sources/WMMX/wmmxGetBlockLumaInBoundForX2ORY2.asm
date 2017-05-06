
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
    .global  wmmxGetBlockNxNLumaInBoundForX2ORY2
    .type    wmmxGetBlockNxNLumaInBoundForX2ORY2, function

wmmxGetBlockNxNLumaInBoundForX2ORY2:
    @ r0: refStart
    @ r1: refLnLength
    @ r2: outStart
    @ r3: outLnLength
    @ r4, r7: blockSize
    @ r5: dxP
	@ r6: dyP
	@ r12:temp
	pld			[r0]
    stmdb		sp!, {r4-r12, lr}		@ save regs used, sp = sp - 40;
	ldr			r4, [sp, #40]			@ get blockSize, "orignal sp" = "current sp" + 40;
	ldr			r5, [sp, #44]			@ get dxP
	ldr			r6, [sp, #48]			@ get dyP
	ldr			r12,[sp, #52]			@ get temp
	mov			r7, r4
	@@@tempsave
	
	cmp			r4, #4
	beq			L_GB4x4LumaInBound

L_GBdx2LumaInBound:
	cmp			r5, #2
	bne			L_GBdy2LumaInBound
	bic			r12,r12, #7
	mov			r11,r12
	@r0 = r0 - two lines
	sub			r0,	r0,r1,lsl #1
	add			r7, r7,#5			@total calcualted lines is blocksize+5		
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

	L_GBZeorDyPLumaInBoundLoop:
		pld			[r0, r1]
		@cmp		r4, #16
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
		wstrd		wr0, [r12]			@ RC(2), save 0-3
		wstrd		wr12,[r12,#8]       @ RC(2), save 4-7
		cmp			r4, #16
		bne	L_GBZeorDyPLumaInBoundSave		

		@ calculate 8-15 for 16x16;
		@ wr0:  calculated [0 to 7], do not overwrite it;
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
		wstrd		wr4, [r12, #16]		@ RC(2), save 8-11
		wstrd		wr5, [r12, #24]		@ RC(2), save 12-15

	L_GBZeorDyPLumaInBoundSave:
		subs		r7, r7, #1
		add			r0, r0, r1
		wldrdne		wr0, [r0]			@ RL(4)
		wldrdne		wr1, [r0, #8]		@ RL(4)
		add			r12, r12, #96		@ temp array's width is 96 byte(24 int)
		bne			L_GBZeorDyPLumaInBoundLoop

@@@@@@@@@@@@@@@@@@@@calculated temp array[height+5][width] is done!!!
	@now temp is calculated,begin to calculate j(dxp==xyp==2)
	mov			r0,	r11			@redirect r0 to r11(the strat of temp)
	mov			r7, r4			@restore  r7
	mov			r1, #96		@temp array'a width is 64 byte(16 int)
	@ for shifting;
	mov			r8, #10
	tmcr		wcgr1, r8
	@ for shifting;
	mov			r8, #5
	tmcr		wcgr2, r8

	@@@@@	preload 6 cache lines
	@sub		r0, r0, r1,lsl #1	@  y_pos - 2*refLnLength;
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

	@@@@ put 512 to wr11
	mov			r11, #512
	tbcstw		wr14, r11	
	
	@@@@ put 16 to wr11
	mov			r11, #16
	tbcsth		wr13, r11	
	@ preload (yPos-1) line to avoid RL;
	add			r10, r0,r1
	wldrd		wr0, [r10]			@ RL(4)
	wldrd		wr1, [r10, #8]		@ RL(4)
	

	@ put [-5(0xfffb), 20, -5, 20]  into wr15
	mvn			r8, #4
	tbcsth		wr15, r8			
	mov			r8, #20
	tinsrh		wr15, r8,#0		
	tinsrh		wr15, r8,#2					
	
	mov			r11,r0 @temp save
	mov			r12,r2 @temp save
	cmp			r4,#16
	moveq		r8,#1				@16x16 control jump		
	L_GBZeroDxPLumaInBoundLoop:@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@loop
	
	@ load (yPos) line to avoid RL;
	add			r10, r10,r1
	wldrd		wr2, [r10]			@ RL(4)
	wldrd		wr3, [r10, #8]		@ RL(4)
	
	@ load (yPos+1) line to avoid RL;
	add			r10, r10,r1
	wldrd		wr4, [r10]			@ RL(4)
	wldrd		wr5, [r10, #8]		@ RL(4)
	
	wunpckihh   wr8,wr2,wr0			@wr0,wr2,wr0,wr2
	wmadds		wr8,wr8,wr15		@wr8 store (2 to 3) *-5+*20
	wunpckihh   wr10,wr3,wr1		@wr1,wr3,wr1,wr3
	wmadds		wr10,wr10,wr15		@wr10 store (6 to 7) *-5+*20
	cmp			r6,#1
	waddheq		wr6,wr2,wr13		@add 16
	wsrahgeq	wr6,wr6,wcgr2		@shift 5
	waddheq		wr7,wr3,wr13		@add 16
	wsrahgeq	wr7,wr7,wcgr2		@shift 5
	wpackhuseq	wr12,wr6,wr7		@store in wr12 for average

	@ load (yPos+2) line to avoid RL;
	add			r10, r10,r1
	wldrd		wr6, [r10]			@ RL(4)
	wldrd		wr7, [r10, #8]		@ RL(4)
	
	wunpckilh   wr9,wr2,wr0			@wr0,wr2,wr0,wr2
	wmadds		wr9,wr9,wr15		@wr9 store (0 to 1) *-5+*20
	wunpckilh   wr11,wr3,wr1		@wr1,wr3,wr1,wr3
	wmadds		wr11,wr11,wr15		@wr11 store (4 to 5) *-5+*20
	
	wunpckihh   wr0,wr4,wr6			@wr6,wr4,wr6,wr4
	wmadds		wr0,wr0,wr15
	waddw		wr8,wr0,wr8 		@wr8 store  (2 to 3)*-5+*20+*20+*-5
	wunpckihh   wr0,wr5,wr7			@wr7,wr5,wr7,wr5
	wmadds		wr0,wr0,wr15
	waddw		wr10,wr0,wr10 		@wr10 store  (6 to 7)*-5+*20+*20+*-5
	cmp			r6,#3
	waddheq		wr0,wr4,wr13		@add 16
	wsrahgeq	wr0,wr0,wcgr2		@shift 5
	waddheq		wr1,wr5,wr13		@add 16
	wsrahgeq	wr1,wr1,wcgr2		@shift 5
	wpackhuseq	wr12,wr0,wr1		@store in wr12 for average
	@ load (yPos+3) line to avoid RL;
	add			r10, r10,r1
	wldrd		wr0, [r10]			@ RL(4)
	wldrd		wr1, [r10, #8]		@ RL(4)
	
	wunpckilh   wr3,wr4,wr6			@wr6,wr4,wr6,wr4
	wmadds		wr3,wr3,wr15
	waddw		wr9,wr9,wr3			@wr9 store  (0 to 1)*-5+*20+*20+*-5
	wunpckilh   wr3,wr5,wr7			@wr7,wr5,wr7,wr5
	wmadds		wr3,wr3,wr15
	waddw		wr11,wr11,wr3		@wr11 store  (4 to 5)*-5+*20+*20+*-5

	@ load (yPos-2) line to avoid RL;
	wldrd		wr4, [r11]			@ RL(4)
	wldrd		wr5, [r11, #8]		@ RL(4)

	wunpckehsh	wr6,wr0
	waddw		wr8,wr6,wr8		@wr8 store  (2 to 3)*-5+*20+*20+*-5+1
	wunpckelsh	wr6,wr0
	waddw		wr9,wr6,wr9		@wr9 store  (0 to 1)*-5+*20+*20+*-5+1
	
	wunpckehsh	wr6,wr4
	waddw		wr8,wr6,wr8		@wr8 store  (2 to 3)1+*-5+*20+*20+*-5+1 done
	wunpckelsh	wr6,wr4
	waddw		wr9,wr6,wr9		@wr9 store  (0 to 1)1+*-5+*20+*20+*-5+1 done
	
	@ Clip((tempLine+512)>>10)
	waddw		wr8, wr8, wr14		@ add 512
	waddw		wr9, wr9, wr14		@ add 512
	wsrawg		wr8, wr8, wcgr1		@ shift 10
	wsrawg		wr9, wr9, wcgr1		@ shift 10
	wpackwus	wr8, wr9, wr8		@ RL(2)
	
	wunpckehsh	wr6,wr1
	waddw		wr10,wr6,wr10		@wr10 store  (6 to 7)*-5+*20+*20+*-5+1
	wunpckelsh	wr6,wr1
	waddw		wr11,wr6,wr11		@wr11 store  (4 to 5)*-5+*20+*20+*-5+1
	
	wunpckehsh	wr6,wr5
	waddw		wr10,wr6,wr10		@wr10 store  (6 to 7)1+*-5+*20+*20+*-5+1 done
	wunpckelsh	wr6,wr5
	waddw		wr11,wr6,wr11		@wr11 store  (4 to 5)1+*-5+*20+*20+*-5+1 done
	
	@ Clip((tempLine+512)>>10)
	waddw		wr10, wr10, wr14		@ add 512
	waddw		wr11, wr11, wr14		@ add 512
	wsrawg		wr10, wr10, wcgr1		@ shift 10
	wsrawg		wr11, wr11, wcgr1		@ shift 10
	wpackwus	wr10, wr11, wr10		@ RL(2)
	cmp			r6,#2					@ eliminate RL
	wpackhus	wr8, wr8, wr10		@ RL(2)
	@@@ dyp!=2
		
	wavg2brne	wr8, wr8, wr12
	@@@save to
	wstrd		wr8,[r12]
	cmp			r8,#1
	cmpeq		r4,#16
	bne			dxpZeroLoopControl

	@@@@16x16	
	add			r11,r0,#16			@r11 refer r0+8
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
	subs		r7, r7, #1
	addne		r2, r2, r3
	addne		r0, r0, r1
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


@@@@@@@@@@@@@@dy2dy2dy2dy2dy2dy2dy2dy2dy2dy2dy2dy2@@@@@@@@@@@@@@@dy2dy2dy2dy2dy2dy2dy2dy2dy2dy2dy2
L_GBdy2LumaInBound:
	bic			r12, r12, #7
	@@temp save
	tmcr		wcgr3,r12
	tmcr		wcgr2,r2
	tmcr		wcgr1,r3

	mov			r2,r12
	@@@the calculated half-pixel is from(xPos-2..xPos+3),so the total of each line is blocksize+5
	add			r3, r4,#5			@total  is blocksize+5
	@r0 = r0 - two lines-2
	sub			r0,	r0,r1,lsl #1
			
	sub			r0, r0, #2			@ x_pos - 2;
	and			r8, r0, #7
	tmcr		wcgr0, r8			@ RL(3)
	bic			r0, r0, #7

	@ put [-5(0xfffb), -5, -5, -5] and [20, 20, 20, 20] into wr14/15
	mvn			r8, #4
	tbcsth		wr14, r8			@ [-5(0xfffb), -5, -5, -5]
	mov			r8, #20
	tbcsth		wr15, r8			@ [20, 20, 20, 20]

	@@@@@	preload 6 cache lines   
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

	@ preload (yPos-1) line to avoid RL;
	add			r10, r0, r1
	wldrd		wr0, [r10]			@ RL(4)
	wldrd		wr1, [r10, #8]		@ RL(4)
	
	mov			r11,r0 @temp save
	cmp			r4,#16
	moveq		r8,#1				@16x16 control jump

	L_GBZeroDxPLumaInBoundLoopdyp2:
	
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
	
	cmp			r3,#5
	@@@save to if the pixels are not the remained 5
	wstrdne		wr4,  [r12]			@ RC(2), save 0-3
	wstrdne		wr5,  [r12,#8]		@ RC(2), save 4-7
	wstrdne		wr12, [r12,#96]		@ RC(2), save 0-3
	wstrdne		wr13, [r12,#104]	@ RC(2), save 4-7
	@@@save to if the pixels are the remained 5			
	wstrdeq		wr4,  [r12]			@ RC(2), save 0-3
	wstrheq		wr5,  [r12,#8]		@ RC(2), save 4
	wstrdeq		wr12, [r12,#96]		@ RC(2), save 0-7
	wstrheq		wr13, [r12,#104]	@ RC(2), save 4	
	sub			r3,r3,#8			@above code handle 8 pixel
	cmp			r3,#5
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@HandleRemained5Pixel:
	addeq		r11,r11,#8			@r11 refer r0+16
	addeq		r10,r11,r1			@r10 refer (yPos-1) line
	@preload (yPos-1) line						
	wldrdeq		wr0, [r10]			@ RL(4)
	wldrdeq		wr1, [r10, #8]		@ RL(4)
	addeq		r12, r12,#16	 	@r12 refer r2+8
	beq			L_GBZeroDxPLumaInBoundLoopdyp2
	
	cmp			r4,#16
	cmpeq		r8,#1
	@bne			dxpZeroLoopControldyp2
	@@@@16x16	
	addeq			r11,r0,#8			@r11 refer r0+8
	addeq			r10,r11,r1			@r10 refer (yPos-1) line
	@preload (yPos-1) line						
	wldrdeq		wr0, [r10]			@ RL(4)
	wldrdeq		wr1, [r10, #8]		@ RL(4)
	addeq			r12,r12,#16	 		@r12 refer r12+16(8 half-word)
	subeq			r8,r8,#1
	beq			L_GBZeroDxPLumaInBoundLoopdyp2

	dxpZeroLoopControldyp2:
	cmp			r4,#16
	moveq		r8,#1				@16x16 control jump		
	subs		r7, r7, #2
	addne		r2, r2, #192        @go to next two output lines
	addne		r0, r0, r1, lsl #1  @go to next two lines
	addne		r10,r0, r1			@r10 refer the (yPos-1) line
	@preload (yPos-1) line						
	wldrdne		wr0, [r10]			@ RL(4)
	wldrdne		wr1, [r10, #8]		@ RL(4)
	addne		r9, r9, r1			@ r9,the 6th line
	movne		r11,r0 @temp save
	movne		r12,r2 @temp save
	addne		r3, r4,#5			@total  is blocksize+5
	pld			[r9,r1]				@preload one line
	bne			L_GBZeroDxPLumaInBoundLoopdyp2
@@@@@@@@@@@@@@@@@@@@@now the temp is calcualted,then begin to calculate the J pixel(dxp==2 && dyp==2)@@@@@@@@@@@@@
		@@@restore the saved value
		tmrc		r0,wcgr3		@save r12 to r0
		tmrc		r2,wcgr2
		tmrc		r3,wcgr1
		@mov		r0, r12
		mov			r1, #96 @the temp array's width is 64 byte
		mov			r7,r4
		
		pld			[r0]
		@ put [20,20,-5,1]  into wr14,[0,0,1,-5] to wr15
		wzero		wr15
		mvn			r8, #4
		tinsrh		wr14, r8,#1	
		tinsrh		wr15, r8,#0			
		mov			r8, #20
		tinsrh		wr14, r8,#2		
		tinsrh		wr14, r8,#3	
		mov			r8,	#1
		tinsrh		wr14, r8,#0	
		tinsrh		wr15, r8,#1	
		@ preload data to avoid RL;
		wldrd		wr0, [r0]			@ RL(4)
		wldrd		wr1, [r0, #8]		@ RL(4)
		
		@@@@@ 8x8 or 16x16 dyP == 2
		mov			r8, #5
		tmcr		wcgr1, r8			@ for shifting;
		mov			r8, #10
		tmcr		wcgr2, r8			@ for shifting;
		mov			r8, #32
		tmcr		wcgr3, r8			@ for shifting;
		
		@ put 16 into wr13,512 to wr12 
		mov			r8, #16
		tbcsth		wr13, r8			
		mov			r8, #512
		tbcstw		wr12, r8			
	    
		mov			r11,r0 @temp save
		mov			r12,r2 @temp save
		cmp			r4,#16
		moveq		r8,#1  @16x16 control jump
	L_GBZeorDyPLumaInBoundLoopdyp2:
		pld			[r11, r1]
		wldrd		wr10, [r11,#16]		@ RL(4)
		wldrd		wr11, [r11,#24]		@ RL(4)
		wmacsz		wr2,wr0,wr14		@wr2 = 0:((20,20)+(-5,1)) RL(2)
		waligni		wr0,wr0,wr1,#2		@1~4:wr0
		wmacs		wr2,wr1,wr15		@wr2 store 0

		waligni		wr1,wr1,wr10,#2		@5~8:wr1
		waligni		wr10,wr10,wr11,#2	@9~12:wr10,now the wr11 is free for use
		wmacsz		wr3,wr0,wr14		@wr2 = 1:((20,20)+(-5,1)) RL(2)
		waligni		wr0,wr0,wr1,#2		@2~5:wr0
		wmacs		wr3,wr1,wr15		@wr3 store 1		
		waligni		wr1,wr1,wr10,#2		@6~9:wr1
		wpackdus	wr9,wr2,wr3			@ 0_1		RL(2)
		wsrld		wr10,wr10,wr13      @10~12 
		waddw		wr9, wr9, wr12		@ add 512
		wsrawg		wr9, wr9, wcgr2		@ shift 10
		
		cmp			r5,#1
		waddheq		wr4,wr0,wr13		@add 16
		wsrahgeq	wr4,wr4,wcgr1		@shift 5
		waddheq		wr5,wr1,wr13		@add 16
		wsrahgeq	wr5,wr5,wcgr1		@shift 5
		wpackhuseq	wr11,wr4,wr5		@store in wr11 for average	

		wmacsz		wr2,wr0,wr14		@wr2 = 2:((20,20)+(-5,1))
		waligni		wr0,wr0,wr1,#2		@3~6:wr0
		wmacs		wr2,wr1,wr15		@wr2 store 2
	
		waligni		wr1,wr1,wr10,#2		@7~10:wr1
		wsrld		wr10,wr10,wr13      @11~12
		cmp			r5,#3
		waddheq		wr4,wr0,wr13		@add 16
		wsrahgeq	wr4,wr4,wcgr1		@shift 5
		waddheq		wr5,wr1,wr13		@add 16
		wsrahgeq	wr5,wr5,wcgr1		@shift 5
		wpackhuseq	wr11,wr4,wr5		@store in wr11 for average	

		wmacsz		wr3,wr0,wr14		@wr3 = 3:((20,20)+(-5,1)) RL(2)
		waligni		wr0,wr0,wr1,#2		@4~7:wr0
		wmacs		wr3,wr1,wr15		@wr3 = 3
		wpackdus	wr7,wr2,wr3			@ 2_3
		waligni		wr1,wr1,wr10,#2		@8~11:wr1	RL(2)
		waddw		wr7, wr7, wr12		@ add 512
		wsrawg		wr7, wr7, wcgr2		@ shift 10
		wpackwus    wr5, wr9, wr7       @wr5 store 3,2,1,0

		wsrld		wr10,wr10,wr13      @12
		wmacsz		wr2,wr0,wr14		@wr2 = 4:((20,20),(-5,1))
		waligni		wr0,wr0,wr1,#2		@5~8:wr0
		wmacs		wr2,wr1,wr15		@wr2 = 4

		waligni		wr1,wr1,wr10,#2		@9~12:wr1,now the wr10 is free for use

		wmacsz		wr3,wr0,wr14		@wr3 = 5:((20,20),(-5,1)) RL(2)
		waligni		wr0,wr0,wr1,#2		@6~9:wr0
		wmacs		wr3,wr1,wr15		@wr3 = 5				  RL(2)
		waligni		wr1,wr1,wr10,#2		@10~12:wr1
		wpackdus	wr7,wr2,wr3			@ 4_5					RL(2)
		wmacsz		wr2,wr0,wr14		@wr2 = ((20,20),(-5,1)) RL(2)

		waddw		wr7, wr7, wr12		@ add 512
		wsrawg		wr7, wr7, wcgr2		@ shift 10

		waligni		wr0,wr0,wr1,#2		@7~10:wr0
		wmacs		wr2,wr1,wr15		@wr2 = 6				RL(2)

		waligni		wr1,wr1,wr10,#2		@11~12:wr1				RL(2)
		wmacsz		wr3,wr0,wr14		@wr3 = 1:((20,20),(-5,1))
		wmacs		wr3,wr1,wr15		@wr3 = 7				RL(2)
		
		cmp			r4, #16
		wpackdus	wr8,wr2,wr3			@ 6_7					RL(2)
		wzero		wr0
		waddw		wr8, wr8, wr12		@ add 512
		wsrawg		wr8, wr8, wcgr2		@ shift 10
		wpackwus    wr6, wr7, wr8       @wr6 store 7,6,5,4		RL(2)
		wzero		wr0
		wpackhus    wr6, wr5, wr6       @						RL(2)
		wzero		wr0
		wavg2br		wr6, wr6, wr11      @ done!!!!!!!
		wstrd		wr6, [r12]
		
		cmpeq		r8, #1				

		
	
		addeq		r11, r11,#16		@8 half word
		addeq		r12, r12,#8			@8 byte
		wldrdeq		wr0, [r11]			@ RL(4)
		wldrdeq		wr1, [r11, #8]		@ RL(4)
		subeq		r8, r8,#1
		beq			L_GBZeorDyPLumaInBoundLoopdyp2
		@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@loop control
		cmp			r4,#16
		moveq		r8,#1				@16x16 control jump	
		subs		r7, r7, #1
		addne		r0, r0, r1
		wldrdne		wr0, [r0]			@ RL(4)
		wldrdne		wr1, [r0, #8]		@ RL(4)
		addne		r2, r2, r3
		movne		r11,r0 @temp save
		movne		r12,r2 @temp save
		
		bne			L_GBZeorDyPLumaInBoundLoopdyp2
	
    	@@@@@ 8x8 or 16x16 dyP == 2 interpolation end;
ldmia		sp!, {r4-r12, pc}	@ restore and return
@@@@@@@@@@@@@@@@@@@@@@@@@@4x44x44x44x44x44x44x44x44x44x44x44x44x44x44x44x44x44x44x@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
L_GB4x4LumaInBound:
L_GB4x4dx2LumaInBound:
	cmp			r5, #2
	bne			L_GB4x4dy2LumaInBound
	bic			r12, r12, #7
	mov			r11,r12
	@r0 = r0 - two lines
	sub			r0,	r0,r1,lsl #1
	add			r7, r7,#5			@total calcualted lines is blocksize+5		
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

	L_GB4x4ZeorDyPLumaInBoundLoop:
		pld			[r0, r1]
		@cmp			r4, #16
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

		waddh		wr0, wr0, wr11		@ (-2 to 1) - 5 * (-1 to 2) + 20 * (0 to 3)
		wmulsl		wr5, wr7, wr14		@ RC(2), -5 * (2 to 5) 
		waddh		wr0, wr0, wr9		@ (-2to1) - 5*(-1to2) + 20*(0to3) + 20*(1to4)
		waddh		wr0, wr0, wr5		@ (-2to1)-5*(-1to2)+20*(0to3)+20*(1to4)-5*(2to5)

		wunpckelub	wr11, wr6			@ 6 to 9 ---saved (until 16x16)
		wunpckehub	wr1, wr6			@ 10 to 13 ---saved (until 16x16)
		waligni		wr12, wr7, wr11, #2 @ 3 to 6
		waddh		wr0, wr0, wr12		@ [0 to 3], 1 -5 20 20 -5 1 done;
		@@@@save to
		wstrd		wr0, [r12]			@ RC(2), save 0-3
		subs		r7, r7, #1
		add			r0, r0, r1
		wldrdne		wr0, [r0]			@ RL(4)
		wldrdne		wr1, [r0, #8]		@ RL(4)
		add			r12, r12, #96		@ temp array's width is 64 byte(16 int)
		bne			L_GB4x4ZeorDyPLumaInBoundLoop
	@@@@@@@@@@@@@@@@@@@@calculated temp array[height+5][width] is done!!!
	@now temp is calculated,begin to calculate j(dxp==xyp==2)
	mov			r0,	r11			@redirect r0 to r11(the strat of temp)
	mov			r7, r4			@restore  r7
	mov			r1, #96			@temp array'a width is 64 byte(16 int)
	@ for shifting;
	mov			r8, #10
	tmcr		wcgr1, r8
	@ for shifting;
	mov			r8, #5
	tmcr		wcgr2, r8

	@@@@@	preload 6 cache lines
	@sub		r0, r0, r1,lsl #1	@  y_pos - 2*refLnLength;
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

	@@@@ put 512 to wr11
	mov			r11, #512
	tbcstw		wr14, r11	
	
	@@@@ put 16 to wr11
	mov			r11, #16
	tbcsth		wr13, r11	
	@ preload (yPos-1) line to avoid RL;
	add			r10, r0,r1
	wldrd		wr0, [r10]			@ RL(4)
	
	@ put [-5(0xfffb), 20, -5, 20]  into wr15
	mvn			r8, #4
	tbcsth		wr15, r8			@ [-5(0xfffb), -5, -5, -5]
	mov			r8, #20
	tinsrh		wr15, r8,#0		
	tinsrh		wr15, r8,#2			@ [-5(0xfffb), 20, -5, 20]		
	

	L_GB4x4ZeroDxPLumaInBoundLoop:@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@loop
	
	@ load (yPos) line to avoid RL;
	add			r10, r10,r1
	wldrd		wr2, [r10]			@ RL(4)
	
	@ load (yPos+1) line to avoid RL;
	add			r10, r10,r1
	wldrd		wr4, [r10]			@ RL(4)
	
	wunpckihh   wr8,wr2,wr0			@wr0,wr2,wr0,wr2
	wmadds		wr8,wr8,wr15		@wr8 store (2 to 3) *-5+*20

	cmp			r6,#1
	waddheq		wr6,wr2,wr13		@add 16
	wsrahgeq	wr6,wr6,wcgr2		@shift 5
	wpackhuseq	wr12,wr6,wr6		@store in wr12 for average

	@ load (yPos+2) line to avoid RL;
	add			r10, r10,r1
	wldrd		wr6, [r10]			@ RL(4)

	wunpckilh   wr9,wr2,wr0			@wr0,wr2,wr0,wr2
	wmadds		wr9,wr9,wr15		@wr9 store (0 to 1) *-5+*20
	
	wunpckihh   wr0,wr4,wr6			@wr6,wr4,wr6,wr4
	wmadds		wr0,wr0,wr15
	waddw		wr8,wr0,wr8 		@wr8 store  (2 to 3)*-5+*20+*20+*-5

	cmp			r6,#3
	waddheq		wr0,wr4,wr13		@add 16
	wsrahgeq	wr0,wr0,wcgr2		@shift 5
	wpackhuseq	wr12,wr0,wr0		@store in wr12 for average
	@ load (yPos+3) line to avoid RL;
	add			r10, r10,r1
	wldrd		wr0, [r10]			@ RL(4)
	
	wunpckilh   wr3,wr4,wr6			@wr6,wr4,wr6,wr4
	wmadds		wr3,wr3,wr15
	waddw		wr9,wr9,wr3			@wr9 store  (0 to 1)*-5+*20+*20+*-5


	@ load (yPos-2) line to avoid RL;
	wldrd		wr4, [r0]			@ RL(4)

	wunpckehsh	wr6,wr0
	waddw		wr8,wr6,wr8		@wr8 store  (2 to 3)*-5+*20+*20+*-5+1
	wunpckelsh	wr6,wr0
	waddw		wr9,wr6,wr9		@wr9 store  (0 to 1)*-5+*20+*20+*-5+1
	
	wunpckehsh	wr6,wr4
	waddw		wr8,wr6,wr8		@wr8 store  (2 to 3)1+*-5+*20+*20+*-5+1 done
	wunpckelsh	wr6,wr4
	waddw		wr9,wr6,wr9		@wr9 store  (0 to 1)1+*-5+*20+*20+*-5+1 done
	
	@ Clip((tempLine+512)>>10)
	waddw		wr8, wr8, wr14		@ add 512
	waddw		wr9, wr9, wr14		@ add 512
	wsrawg		wr8, wr8, wcgr1		@ shift 10
	wsrawg		wr9, wr9, wcgr1		@ shift 10
	wpackwus	wr8, wr9, wr8		@ RL(2)
	cmp			r6,#2				@eliminate RL
	wpackhus	wr8, wr8, wr8

	@@@ dyp!=2
	wavg2brne	wr8, wr8, wr12
	@@@save to
	wstrw		wr8,[r2]
	subs		r7, r7, #1
	addne		r2, r2, r3
	addne		r0, r0, r1
	addne		r10,r0, r1			@r10 refer the (yPos-1) line
	@preload (yPos-1) line						
	wldrdne		wr0, [r10]			@ RL(4)
	addne		r9, r9, r1			@ r9,the 6th line
	pld			[r9,r1]				@preload one line
	bne			L_GB4x4ZeroDxPLumaInBoundLoop
ldmia		sp!, {r4-r12, pc}		@ restore and return	
L_GB4x4dy2LumaInBound:
	bic			r12, r12, #7
	@@temp save
	tmcr		wcgr3,r12
	tmcr		wcgr2,r2
	tmcr		wcgr1,r3
	mov			r2,r12
	@@@the calculated half-pixel is from(xPos-2..xPos+3),so the total of each line is blocksize+5
	add			r3, r4,#5			@total  is blocksize+5
	@r0 = r0 - two lines-2
	sub			r0,	r0,r1,lsl #1
			
	sub			r0, r0, #2			@ x_pos - 2;
	and			r8, r0, #7
	tmcr		wcgr0, r8			@ RL(3)
	bic			r0, r0, #7

	@ put [-5(0xfffb), -5, -5, -5] and [20, 20, 20, 20] into wr14/15
	mvn			r8, #4
	tbcsth		wr14, r8			@ [-5(0xfffb), -5, -5, -5]
	mov			r8, #20
	tbcsth		wr15, r8			@ [20, 20, 20, 20]

	@@@@@	preload 6 cache lines   
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

	@ preload (yPos-1) line to avoid RL;
	add			r10, r0, r1
	wldrd		wr0, [r10]			@ RL(4)
	wldrd		wr1, [r10, #8]		@ RL(4)
	
	mov			r11,r0 @temp save
    L_GB4x4ZeroDxPLumaInBoundLoopdyp2:@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
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
	
	@@@save to if the pixels are not the remained 1
	wstrd		wr4,  [r12]			@ RC(2), save 0-3
	wstrd		wr5,  [r12,#8]		@ RC(2), save 4-7
	wstrd		wr12, [r12,#96]		@ RC(2), save 0-3
	wstrd		wr13, [r12,#104]		@ RC(2), save 4-7
	

	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@HandleRemained5Pixel:
	add		r11,r11,#8			@r11 refer r0+8
	add		r10,r11,r1			@r10 refer (yPos-1) line
	@preload (yPos-1) line						
	wldrd		wr0, [r10]			@ RL(4)
	wldrd		wr1, [r10, #8]		@ RL(4)
	add		    r12, r12,#16	 	@r12 refer r2+8
    
	@ load (yPos) line to avoid RL;
	add			r10, r10,r1
	wldrd		wr2, [r10]			@ RL(4)
	wldrd		wr3, [r10, #8]		@ RL(4)
	
	walignr0	wr1,wr0,wr1			@wr1 store the (yPos-1) line
	wunpckelub	wr6, wr1			@  
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
	wldrd		wr0, [r11]			@ RL(4)
	wldrd		wr1, [r11, #8]		@ RL(4)

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

	@@@save to if the pixels are the remained 5			
	wstrh		wr4,  [r12]			@ RC(2), save 1
	wstrh		wr12, [r12,#96]		@ RC(2), save 1
LoopControl:
	subs		r7, r7, #2
	addne		r2, r2, #192        @go to next two output lines
	addne		r0, r0, r1, lsl #1  @go to next two lines
	addne		r10,r0, r1			@r10 refer the (yPos-1) line
	@preload (yPos-1) line						
	wldrdne		wr0, [r10]			@ RL(4)
	wldrdne		wr1, [r10, #8]		@ RL(4)
	addne		r9, r9, r1			@ r9,the 6th line
	mov			r11,r0 @temp save
	mov			r12,r2 @temp save
	add			r3, r4,#5			@total  is blocksize+5
	pld			[r9,r1]				@preload one line
	bne			L_GB4x4ZeroDxPLumaInBoundLoopdyp2
@@@@@@@@@@@@@@@@@@@@@now the temp is calcualted,then begin to calculate the J pixel(dxp==2 && dyp==2)@@@@@@@@@@@@@
		@@@restore the saved value
		tmrc		r0,wcgr3		@save r12 to r0
		tmrc		r2,wcgr2
		tmrc		r3,wcgr1
		mov			r1, #96 @the temp array's width is 64 byte
		mov			r7,r4
		
		pld			[r0]
		@ put [20,20,-5,1]  into wr14,[0,0,1,-5] to wr15
		wzero		wr15
		mvn			r8, #4
		tinsrh		wr14, r8,#1	
		tinsrh		wr15, r8,#0			
		mov			r8, #20
		tinsrh		wr14, r8,#2		
		tinsrh		wr14, r8,#3	
		mov			r8,	#1
		tinsrh		wr14, r8,#0	
		tinsrh		wr15, r8,#1	
		@ preload data to avoid RL;
		wldrd		wr0, [r0]			@ RL(4)
		wldrd		wr1, [r0, #8]		@ RL(4)
		
		@@@@@ 8x8 or 16x16 dyP == 2
		mov			r8, #5
		tmcr		wcgr1, r8			@ for shifting;
		mov			r8, #10
		tmcr		wcgr2, r8			@ for shifting;
		mov			r8, #32
		tmcr		wcgr3, r8			@ for shifting;
		
		@ put 16 into wr13,512 to wr12 
		mov			r8, #16
		tbcsth		wr13, r8			
		mov			r8, #512
		tbcstw		wr12, r8			
	    
	L_GB4x4ZeorDyPLumaInBoundLoopdyp2:
		pld			[r0, r1]
		wldrd		wr10, [r0,#16]		@ RL(4)
		wmacsz		wr2,wr0,wr14		@wr2 = 0:((20,20)+(-5,1)) RL(2)
		waligni		wr0,wr0,wr1,#2		@1~4:wr0
		wmacs		wr2,wr1,wr15		@wr2 store 0

		waligni		wr1,wr1,wr10,#2		@5~8:wr1
		wmacsz		wr3,wr0,wr14		@wr2 = 1:((20,20)+(-5,1)) RL(2)
		waligni		wr0,wr0,wr1,#2		@2~5:wr0
		wmacs		wr3,wr1,wr15		@wr3 store 1		
		waligni		wr1,wr1,wr10,#2		@6~9:wr1
		wpackdus	wr9,wr2,wr3			@ 0_1
		cmp			r5,#1
		waddw		wr9, wr9, wr12		@ add 512
		wsrawg		wr9, wr9, wcgr2		@ shift 10
		
		waddheq		wr4,wr0,wr13		@add 16
		wsrahgeq	wr4,wr4,wcgr1		@shift 5
		wpackhuseq	wr11,wr4,wr4		@store in wr11 for average	

		wmacsz		wr2,wr0,wr14		@wr2 = 2:((20,20)+(-5,1))
		waligni		wr0,wr0,wr1,#2		@3~6:wr0
		wmacs		wr2,wr1,wr15		@wr2 store 2
	
		waligni		wr1,wr1,wr10,#2		@7~10:wr1
		cmp			r5,#3
		waddheq		wr4,wr0,wr13		@add 16
		wsrahgeq	wr4,wr4,wcgr1		@shift 5
		wpackhuseq	wr11,wr4,wr4		@store in wr11 for average	

		wmacsz		wr3,wr0,wr14		@wr3 = 3:((20,20)+(-5,1)) RL(2)
		wzero		wr0					@just eliminate the RL
		wmacs		wr3,wr1,wr15		@wr3 = 3
		wpackdus	wr7,wr2,wr3			@ 2_3
		wzero		wr0					@just eliminate the RL
		waddw		wr7, wr7, wr12		@ add 512
		wsrawg		wr7, wr7, wcgr2		@ shift 10
		wpackwus    wr5, wr9, wr7       @wr5 store 3,2,1,0
		wzero		wr0					@just eliminate the RL
		wpackhus    wr5, wr5, wr5
		wzero		wr0					@just eliminate the RL
		wavg2br		wr5, wr5, wr11      @ done!!!!!!!
		wstrw		wr5, [r2]
		@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@loop control
	
		subs		r7, r7, #1
		addne		r0, r0, r1
		wldrdne		wr0, [r0]			@ RL(4)
		wldrdne		wr1, [r0, #8]		@ RL(4)
		addne		r2, r2, r3
		bne			L_GB4x4ZeorDyPLumaInBoundLoopdyp2
	ldmia		sp!, {r4-r12, pc}		@ restore and return	
	


