
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	*		void wmmxGBNxNChromaInBound(avdNativeInt x0,					*
	*			avdNativeInt y0, avdUInt8 *refStart,						*
	*			avdNativeInt refLnLength, avdUInt8 *outStart, avdNativeInt  *
	*			outLnLength, avdNativeInt blockSize)						*
	*			N: 2, 4, 8;													*
	*		IL: issue latency; RL: result latency;							*
	*       RC: resource conflict latency									*
	*																		*
	************************************************************************/

    .section .text
	.align 2
    .global  wmmxGBNxNChromaInBound
    .type    wmmxGBNxNChromaInBound, function

wmmxGBNxNChromaInBound:
    @ r2: refStart
    @ r3: refLnLength
    @ r4: outStart
    @ r5: outLnLength
    @ r6: blockSize
	@ r8: (refStart-aligned + 1), since aligned only takes #imm3, 
	@     we can not use walignr1, if r8 = 8;
    @ r9: coefA1
	@ r10: coefA2
    @ r11: coefB1
	@ r12: coefB2
    stmdb		sp!, {r4-r12, lr}		@ save regs used, sp = sp - 40;
	mul			r12, r0, r1				@ IL(1), RL(4), RC(3), coefB2=x0 * y0
	ldr			r4, [sp, #40]			@ RL(3), get outStart, "orignal sp" = "current sp" + 40;
	ldr			r5, [sp, #44]			@ RL(3), get outLnLength
	ldr			r6, [sp, #48]			@ RL(3), get blockSize
	cmp			r12, #0					@ 0=x0 * y0;
	bne			L_GBChromaInBoundNonZeroX0Y0
	
	@ For now, just support 8x8 and 0=x0 * y0;
	cmp			r6, #8
	bne			L_GBChromaInBoundNonZeroX0Y0

	and			r8, r2, #7
	bic			r2, r2, #7
	@ preload data to avoid RL;
	wldrd		wr0, [r2]			@ RL(4), 1st unaligned 0-7
	wldrd		wr1, [r2, #8]		@ RL(4), 1st unaligned 8-15	
	add			r2, r2, r3;
	tmcr		wcgr0, r8			@ RL(3), for 0-7 aligned;

	cmp			r0, #0
	bne			L_GB8x8ChromaInBoundNonZeroX0
L_GB8x8ChromaInBoundZeroX0:
	@ 8x8, x= 0;
	cmp			r1, #0
	beq			L_GB8x8ChromaInBoundZeroX0Y0

	@ move 4 constant and >>3; 
	mov			r7, #3
	tmcr		wcgr2, r7			@ RL(3)
	mov			r7, #4
	tbcsth		wr11, r7			@ wr11: [4, 4, 4, 4];

	wldrd		wr2, [r2]			@ RL(4), 2nd unaligned 0-7
	wldrd		wr3, [r2, #8]		@ RL(4), 2nd unaligned 8-15	
	tbcsth		wr12, r1			@ wr12: [y0, y0, y0, y0];
	walignr0	wr4, wr0, wr1		@ input 1st line 0-7
	wunpckelub	wr0, wr4			@ 1st line 0-3
	wunpckehub	wr1, wr4			@ 1st line 4-7
	L_GB8x8ChromaInBoundZeroX0Loop:
		subs		r6, r6, #1
		addne		r2, r2, r3
		walignr0	wr4, wr2, wr3		@ input 1st line 0-7
		wunpckelub	wr2, wr4			@ 2nd line 0-3
		wunpckehub	wr3, wr4			@ 2nd line 4-7
		wsubh		wr4, wr2, wr0
		wmulsl		wr4, wr4, wr12		@ RC(2), 0-3, (ref1[k] - ref0[k]) * y0
		wsubh		wr5, wr3, wr1
		wmulsl		wr5, wr5, wr12		@ RC(2), 4-7, (ref1[k] - ref0[k]) * y0
		waddh		wr4, wr4, wr11		
		wsrahg		wr4, wr4, wcgr2		@ ((ref1[k] - ref0[k]) * y0 + 4)>>3
		waddh		wr5, wr5, wr11		
		wsrahg		wr5, wr5, wcgr2		@ ((ref1[k] - ref0[k]) * y0 + 4)>>3
		waddh		wr4, wr4, wr0		
		waddh		wr5, wr5, wr1		

		wmovne		wr0, wr2			@ prepare for next cycle;
		wmovne		wr1, wr3			@ prepare for next cycle;
		wldrdne		wr2, [r2]			@ RL(4), 2nd unaligned 0-7
		wpackhus	wr4, wr4, wr5		@ RL(2), [0 to 7] ready;
		wldrdne		wr3, [r2, #8]		@ RL(4), 2nd unaligned 8-15	
		wstrd		wr4, [r4]			@ store first line;
		addne		r4, r4, r5
		bne			L_GB8x8ChromaInBoundZeroX0Loop
    ldmia		sp!, {r4-r12, pc}		@ restore and return
L_GB8x8ChromaInBoundZeroX0Y0:
		@ 8x8, x= 0, y = 0;
		subs		r6, r6, #1
		walignr0	wr4, wr0, wr1		@ input 1st line 0-7
		wldrdne		wr0, [r2]			@ RL(4), 2nd unaligned 0-7
		wldrdne		wr1, [r2, #8]		@ RL(4), 2nd unaligned 8-15	
		addne		r2, r2, r3;
		wstrd		wr4, [r4]			@ store first line;
		addne		r4, r4, r5
		bne			L_GB8x8ChromaInBoundZeroX0Y0
    ldmia		sp!, {r4-r12, pc}		@ restore and return

L_GB8x8ChromaInBoundNonZeroX0:
	@ 8x8, y= 0;
	@ move 4 constant and >>3; 
	mov			r7, #3
	tmcr		wcgr2, r7			@ RL(3)
	mov			r7, #4
	tbcsth		wr11, r7			@ wr11: [4, 4, 4, 4];

	add			r8, r8, #1			@ 
	tmcr		wcgr1, r8			@ RL(3), for 1-8 aligned;
	tbcsth		wr12, r0			@ wr12: [x0, x0, x0, x0];
	L_GB8x8ChromaInBoundNonZeroX0Loop:
		walignr0	wr8, wr0, wr1		@ input 1st line 0-7
		wunpckelub	wr9, wr8			@ 1st line 0-3
		wunpckehub	wr7, wr8			@ 1st line 4-7
		cmp			r8, #8
		walignr1ne	wr1, wr0, wr1		@ input 1st line 1-8
		wunpckelub	wr2, wr1			@ 1st line 1-3
		wunpckehub	wr3, wr1			@ 1st line 5-8
		wsubh		wr4, wr2, wr9
		wmulsl		wr4, wr4, wr12		@ RC(2), 0-3, (ref0[k+1] - ref0[k]) * x0
		wsubh		wr5, wr3, wr7
		wmulsl		wr5, wr5, wr12		@ RC(2), 4-7, (ref0[k+1] - ref0[k]) * x0
		waddh		wr4, wr4, wr11		
		wsrahg		wr4, wr4, wcgr2		@ ((ref0[k+1] - ref0[k]) * x0 + 4)>>3
		waddh		wr5, wr5, wr11		
		wsrahg		wr5, wr5, wcgr2		@ ((ref0[k+1] - ref0[k]) * x0 + 4)>>3
		@ use waddb after packing to save one cycle;
		@waddh		wr4, wr4, wr9		
		@waddh		wr5, wr5, wr7
				
		wpackhss	wr4, wr4, wr5		@ RL(2); could < 0, so use ss;
		subs		r6, r6, #1
		wldrdne		wr0, [r2]			@ RL(4), 2nd unaligned 0-7
		wldrdne		wr1, [r2, #8]		@ RL(4), 2nd unaligned 8-15	
		waddb		wr4, wr4, wr8		@ [0 to 7] ready;
		addne		r2, r2, r3
		wstrd		wr4, [r4]			@ store first line;
		addne		r4, r4, r5
		bne			L_GB8x8ChromaInBoundNonZeroX0Loop
    ldmia		sp!, {r4-r12, pc}		@ restore and return

L_GBChromaInBoundNonZeroX0Y0:
	cmp			r6, #2
	rsb			r10, r12, r0, LSL #3	@ coefA2 = (x0<<3) - coefB2;
	pld			[r2]					@ 
	rsb			r11, r12, r1, LSL #3	@ coefB1 = (y0<<3) - coefB2;
	add			r9, r10, r11
	add			r9, r9, r12
	rsb			r9, r9, #64				@ coefA1 = 64 - (coefB2 + coefB1 + coefA2)
	beq			L_GB2x2ChromaInBound
	@@@@@ 4x4 and 8x8 start;
	@ wr11: [32, 32, 32, 32] constant
	@ wr12: coefA1
	@ wr13: coefA2
	@ wr14: coefB1
	@ wr15: coefB2
	@ data alignment;

	and			r8, r2, #7
	bic			r2, r2, #7
	@ preload data to avoid RL;
	wldrd		wr0, [r2]			@ RL(4), 1st unaligned 0-7
	wldrd		wr5, [r2, #8]		@ RL(4), 1st unaligned 8-15	
	add			r2, r2, r3;
	tmcr		wcgr0, r8			@ RL(3), for 0-7 aligned;
	@ broadcast the constant
	tbcsth		wr12, r9			@ wr12: coefA1
	tbcsth		wr13, r10			@ wr13: coefA2
	add			r8, r8, #1			@ 
	cmp			r8, #8				@ set the flag;
	tmcrne		wcgr1, r8			@ RL(3), for 1-8 aligned;
	tbcsth		wr14, r11			@ wr14: coefB1
	tbcsth		wr15, r12			@ wr15: coefB2

	@ move 32 constant and >>6; 
	mov			r7, #6
	tmcr		wcgr2, r7			@ RL(3)
	mov			r7, #32
	tbcsth		wr11, r7			@ wr11: [32, 32, 32, 32];

	wldrd		wr2, [r2]			@ RL(4), 2nd unaligned 0-7	
	wldrd		wr3, [r2, #8]		@ RL(4), 2nd unaligned 8-15	
	add			r2, r2, r3;
	@ first line
	walignr0	wr4, wr0, wr5		@ input 1st line 0-7 -- saved for 8x8
	@ if r8 != 8, wr5 need to be aligned;
	walignr1ne	wr5, wr0, wr5		@ input 1st line 1-8 -- saved for 8x8
	wunpckelub	wr6, wr4			@ 1st line 0-3
	wmulsl		wr0, wr6, wr12		@ RC(2), coefA1 * (1st line 0-3)
	wunpckelub	wr7, wr5			@ 1st line 1-4
	wmulsl		wr1, wr7, wr13		@ RC(2), coefA2 * (1st line 1-4)

	walignr0	wr8, wr2, wr3		@ input 2nd line 0-7 -- saved for 8x8
	walignr1ne	wr3, wr2, wr3		@ input 2nd line 1-8 -- saved for 8x8
	waddh		wr0, wr0, wr1		@ coefA1*(1st0-3) + coefA2*(1st1-4)
	wunpckelub	wr7, wr8			@ 2nd line 0-3
	wmulsl		wr2, wr7, wr14		@ RC(2), coefB1 * (2nd line 0-3)
	wunpckelub	wr10, wr3			@ 2nd line 1-4
	wmulsl		wr9, wr10, wr15		@ RC(2), coefB2 * (2nd line 1-4)
	waddh		wr2, wr2, wr0		@ coefA1*(1st0-3) + coefA2*(1st1-4) + coefB1*(2nd0-3)

	@ load 3rd line
	wmulsl		wr7, wr7, wr12		@ RC(2), coefA1 * (2nd line 0-3)
	wldrd		wr0, [r2]			@ RL(4), 1st unaligned 0-7
	wmulsl		wr10, wr10, wr13	@ RC(2), coefA2 * (2nd line 1-4)
	wldrd		wr1, [r2, #8]		@ RL(4), 1st unaligned 8-15	
	add			r2, r2, r3;
	waddh		wr10, wr10, wr7     @ coefA1*(2nd0-3) + coefA2*(2nd1-4)		
	waddh		wr2, wr2, wr9		@ coefA1*(1st0-3)+coefA2*(1st1-4)+coefB1*(2nd0-3)+coefB2*(2nd1-4)
	waddh		wr2, wr2, wr11		@ add 32
	wsrlhg		wr2, wr2, wcgr2		@ >>6, 1st line 0-3 done; -- saved

	@ 2nd line
	walignr0	wr6, wr0, wr1		@ input 3rd line 0-7 -- saved for 8x8
	walignr1ne	wr1, wr0, wr1		@ input 3rd line 1-8 -- saved for 8x8
	wunpckelub	wr0, wr6			@ 3rd line 0-3
	wmulsl		wr9, wr0, wr14		@ RC(2), coefB1 * (3rd line 0-3)
	wunpckelub	wr7, wr1			@ 3rd line 1-4
	waddh		wr10, wr10, wr9     @ coefA1*(2nd0-3) + coefA2*(2nd1-4)	+ coefB1*(3rd0-3)	
	wmulsl		wr9, wr7, wr15		@ RC(2), coefB2 * (3rd line 1-4)
	waddh		wr10, wr10, wr11	@ add 32
	waddh		wr10, wr10, wr9     @ coefA1*(2nd0-3)+coefA2*(2nd1-4)+coefB1*(3rd0-3)+coefB2*(3rd1-4)	

	wmulsl		wr0, wr0, wr12		@ RC(2), coefA1 * (3rd line 0-3)
	wsrlhg		wr10, wr10, wcgr2	@ >>6, 2nd line 0-3 done; -- saved
	wmulsl		wr7, wr7, wr13		@ RC(2), coefA2 * (3rd line 1-4)
	@ run out of register; split 8x8 and 4x4;
	cmp			r6, #8
	waddh		wr9, wr0, wr7		@ coefA1 * (3rd line 0-3) + coefA2 * (3rd line 1-4); -- saved;
	bne			L_GB4x4ChromaInBoundContinue

	@ continue for 8x8
	cmp			r8, #8				@ reset the flag;
	wunpckehub	wr4, wr4			@ 1st line 4-7
	wmulsl		wr4, wr4, wr12		@ RC(2), coefA1 * (1st line 4-7)
	wunpckehub	wr5, wr5			@ 1st line 5-8
	wmulsl		wr5, wr5, wr13		@ RC(2), coefA2 * (1st line 5-8)

	wunpckehub	wr8, wr8			@ 2nd line 4-7
	wmulsl		wr0, wr8, wr14		@ RC(2), coefB1 * (2nd line 4-7)
	waddh		wr4, wr4, wr5		@ coefA1*(1st4-7) + coefA2*(1st5-8) 
	wunpckehub	wr3, wr3			@ 2nd line 5-8
	wmulsl		wr7, wr3, wr15		@ RC(2), coefB2 * (2nd line 5-8)
	waddh		wr4, wr4, wr0		@ coefA1*(1st4-7) + coefA2*(1st5-8) + coefB1*(2nd4-7)
	waddh		wr4, wr4, wr7		@ coefA1*(1st4-7)+coefA2*(1st5-8)+coefB1*(2nd4-7)+coefB2*(2nd5-8)

	@ load 4th line
	wldrd		wr7, [r2]			@ RL(4), 1st unaligned 0-7
	wmulsl		wr8, wr8, wr12		@ RC(2), coefA1 * (2nd line 4-7)
	wldrd		wr0, [r2, #8]		@ RL(4), 1st unaligned 8-15	
	add			r2, r2, r3;
	wmulsl		wr3, wr3, wr13		@ RC(2), coefA2 * (2nd line 5-8)
	waddh		wr4, wr4, wr11		@ add 32
	wsrlhg		wr4, wr4, wcgr2		@ >>6, 1st line 5-7 done;
	waddh		wr8, wr8, wr3		@ coefA1*(2nd4-7) + coefA2*(2nd5-8)
	wpackhus	wr4, wr2, wr4		@ RL(2), 1st line 0-7 ready;
	wunpckehub	wr6, wr6			@ 3rd line 4-7
	wmulsl		wr2, wr6, wr14		@ RC(2), coefB1 * (3rd line 4-7)
	wstrd		wr4, [r4]			@ store first line;
	add			r4, r4, r5
	 
	wunpckehub	wr1, wr1			@ 3rd line 5-8
	wmulsl		wr4, wr1, wr15		@ RC(2), coefB2 * (3rd line 5-8)
	waddh		wr8, wr8, wr2		@ coefA1*(2nd4-7) + coefA2*(2nd5-8) + coefB2*(3rd5-8)
	walignr0	wr2, wr7, wr0		@ input 4th line 0-7 
	waddh		wr8, wr8, wr4		@ coefA1*(2nd4-7)+coefA2*(2nd5-8)+coefB2*(3rd5-8)+coefB2*(3rd5-8)
	wmulsl		wr6, wr6, wr12		@ RC(2), coefA1 * (3rd line 4-7)
	walignr1ne	wr0, wr7, wr0		@ input 4th line 1-8 
	waddh		wr8, wr8, wr11		@ add 32
	wsrlhg		wr8, wr8, wcgr2		@ >>6, 2nd line 5-7 done;
	wpackhus	wr8, wr10, wr8		@ RL(2), 2nd line 0-7 ready;
	wmulsl		wr7, wr1, wr13		@ RC(2), coefA2 * (3rd line 5-8)
	wstrd		wr8, [r4]			@ store 2nd line;
	add			r4, r4, r5

	@ used above: wr0, wr2, wr3, wr6, wr7
	wunpckelub	wr1, wr2			@ 4th line 0-3
	wmulsl		wr8, wr1, wr14		@ RC(2), coefB1 * (4th line 0-3)
	wunpckelub	wr4, wr0			@ 4th line 5-8
	wmulsl		wr3, wr4, wr15		@ RC(2), coefB2 * (4th line 1-4)
	waddh		wr9, wr9, wr8
	waddh		wr3, wr3, wr9		@ coefA1*(3rd0-3)+coefA2*(3rd1-4)+coefB1*(4th0-3)+coefB2*(4th1-4)
	wmulsl		wr1, wr1, wr12		@ RC(2), coefA1 * (4th line 0-3)
	waddh		wr3, wr3, wr11		@ add 32
	wmulsl		wr4, wr4, wr13		@ RC(2), coefA2 * (4th line 1-4)
	wsrlhg		wr3, wr3, wcgr2		@ >>6, 3rd line 0-3 done;
	waddh		wr1, wr1, wr4		@ coefA1*(4th0-3) + coefA2*(4th1-4)

	@ load 5th line
	wldrd		wr10, [r2]			@ RL(4), 5th unaligned 0-7
	wldrd		wr9, [r2, #8]		@ RL(4), 5th unaligned 8-15	
	add			r2, r2, r3;
	wunpckehub	wr2, wr2			@ 4th line 4-7
	wmulsl		wr4, wr2, wr14		@ RC(2), coefB1 * (4th line 4-7)
	wunpckehub	wr0, wr0			@ 4th line 5-8
	wmulsl		wr5, wr0, wr15		@ RC(2), coefB2 * (4th line 5-8)
	walignr0	wr8, wr10, wr9		@ input 5th line 0-7 
	walignr1ne	wr9, wr10, wr9		@ input 5th line 1-8 
	waddh		wr7, wr7, wr6		@ coefA1*(3rd4-7)+coefA2*(3rd5-8)
	waddh		wr7, wr7, wr4 
	waddh		wr7, wr7, wr5		@ coefA1*(3rd4-7)+coefA2*(3rd5-8)+coefB1*(4th4-7)+coefB2*(4th5-8)
	waddh		wr7, wr7, wr11		@ add 32
	wsrlhg		wr7, wr7, wcgr2		@ >>6, 3rd line 4-7 done;
	wmulsl		wr2, wr2, wr12		@ RC(2), coefA1 * (4th line 4-7)
	wpackhus	wr3, wr3, wr7		@ RL(2), 3rd line 0-7 ready;
	wmulsl		wr0, wr0, wr13		@ RC(2), coefA2 * (4th line 5-8)
	wstrd		wr3, [r4]			@ store 3rd line;
	add			r4, r4, r5
	waddh		wr0, wr0, wr2		@ coefA1*(4th4-7) + coefA2*(4th5-8)

	@ used above: wr0, wr1, wr8, wr9
	wunpckelub	wr4, wr8			@ 5th line 0-3
	wmulsl		wr6, wr4, wr14		@ RC(2), coefB1 * (5th line 0-3)
	wunpckelub	wr5, wr9			@ 5th line 1-4
	wmulsl		wr7, wr5, wr15		@ RC(2), coefB2 * (5th line 1-4)
	waddh		wr1, wr1, wr6		@ coefA1*(4th0-3)+coefA2*(4th1-4)+coefB1*(5th0-3)
	wmulsl		wr4, wr4, wr12		@ RC(2), coefA1 * (5th line 0-3)
	waddh		wr1, wr1, wr7		@ coefA1*(4th0-3)+coefA2*(4th1-4)+coefB1*(5th0-3)+coefB2*(5th1-4)
	wmulsl		wr5, wr5, wr13		@ RC(2), coefA2 * (5th line 1-4)
	waddh		wr1, wr1, wr11		@ add 32
	waddh		wr4, wr4, wr5		@ coefA1*(5th0-3) + coefA2*(5th1-4)
	wsrlhg		wr1, wr1, wcgr2		@ >>6, 4th line 0-3 done;

	@ load 6th line
	wldrd		wr2, [r2]			@ RL(4), 6th unaligned 0-7
	wldrd		wr7, [r2, #8]		@ RL(4), 6th unaligned 8-15	
	add			r2, r2, r3;
	wunpckehub	wr8, wr8			@ 5th line 4-7
	wmulsl		wr3, wr8, wr14		@ RC(2), coefB1 * (5th line 4-7)
	wunpckehub	wr9, wr9			@ 5th line 5-8
	wmulsl		wr5, wr9, wr15		@ RC(2), coefB2 * (5th line 5-8)
	walignr0	wr6, wr2, wr7		@ input 6th line 0-7 
	waddh		wr0, wr0, wr3
	walignr1ne	wr7, wr2, wr7		@ input 6th line 1-8 
	waddh		wr0, wr0, wr5		@ coefA1*(4th4-7)+coefA2*(4th5-8)+coefB1*(5th4-7)+coefB2*(5th5-8)
	waddh		wr0, wr0, wr11		@ add 32
	wsrlhg		wr0, wr0, wcgr2		@ >>6, 4th line 4-7 done;
	wmulsl		wr8, wr8, wr12		@ RC(2), coefA1 * (5th line 4-7)
	wpackhus	wr1, wr1, wr0		@ RL(2), 4th line 0-7 ready;
	wmulsl		wr9, wr9, wr13		@ RC(2), coefA2 * (5th line 5-8)
	wstrd		wr1, [r4]			@ store 4th line;
	add			r4, r4, r5
	waddh		wr8, wr8, wr9		@ coefA1*(5th4-7) + coefA2*(5th5-8)
	
	@ used above wr4, wr6, wr7, wr8
	wunpckelub	wr0, wr6			@ 6th line 0-3
	wmulsl		wr2, wr0, wr14		@ RC(2), coefB1 * (6th line 0-3)
	wunpckelub	wr1, wr7			@ 6th line 1-4
	wmulsl		wr3, wr1, wr15		@ RC(2), coefB2 * (6th line 1-4)
	waddh		wr4, wr4, wr2		@ coefA1*(5th0-3)+coefA2*(5th1-4)+coefB1*(6th0-3)
	wmulsl		wr0, wr0, wr12		@ RC(2), coefA1 * (6th line 0-3)
	waddh		wr4, wr4, wr3		@ coefA1*(5th0-3)+coefA2*(5th1-4)+coefB1*(6th0-3)+coefB2*(6th1-4)
	wmulsl		wr1, wr1, wr13		@ RC(2), coefA2 * (6th line 1-4)
	waddh		wr4, wr4, wr11		@ add 32
	waddh		wr0, wr0, wr1		@ coefA1*(6th0-3) + coefA2*(6th1-4)
	wsrlhg		wr4, wr4, wcgr2		@ >>6, 5th line 0-3 done;

	@ load 7th line
	wldrd		wr1, [r2]			@ RL(4), 7th unaligned 0-7
	wldrd		wr2, [r2, #8]		@ RL(4), 7th unaligned 8-15	
	add			r2, r2, r3;
	wunpckehub	wr6, wr6			@ 6th line 4-7
	wmulsl		wr9, wr6, wr14		@ RC(2), coefB1 * (6th line 4-7)
	wunpckehub	wr7, wr7			@ 6th line 5-8
	wmulsl		wr10, wr7, wr15		@ RC(2), coefB2 * (7th line 5-8)
	walignr0	wr3, wr1, wr2		@ input 7th line 0-7 
	waddh		wr8, wr8, wr9
	walignr1ne	wr2, wr1, wr2		@ input 7th line 1-8 
	waddh		wr8, wr8, wr10		@ coefA1*(5th4-7)+coefA2*(5th5-8)+coefB1*(6th4-7)+coefB2*(6th5-8)
	waddh		wr8, wr8, wr11		@ add 32
	wsrlhg		wr8, wr8, wcgr2		@ >>6, 5th line 4-7 done;
	wmulsl		wr6, wr6, wr12		@ RC(2), coefA1 * (6th line 4-7)
	wpackhus	wr4, wr4, wr8		@ RL(2), 5th line 0-7 ready;
	wmulsl		wr7, wr7, wr13		@ RC(2), coefA2 * (6th line 5-8)
	wstrd		wr4, [r4]			@ store 5th line;
	add			r4, r4, r5
	waddh		wr6, wr6, wr7		@ coefA1*(6th4-7) + coefA2*(6th5-8)
	
	@ used above wr0, wr2, wr3, wr6
	wunpckelub	wr4, wr3			@ 7th line 0-3
	wmulsl		wr1, wr4, wr14		@ RC(2), coefB1 * (7th line 0-3)
	wunpckelub	wr5, wr2			@ 7th line 1-4
	wmulsl		wr7, wr5, wr15		@ RC(2), coefB2 * (7th line 1-4)
	waddh		wr0, wr0, wr1		@ coefA1*(6th0-3)+coefA2*(6th1-4)+coefB1*(7th0-3)
	wmulsl		wr4, wr4, wr12		@ RC(2), coefA1 * (7th line 0-3)
	waddh		wr0, wr0, wr7		@ coefA1*(6th0-3)+coefA2*(6th1-4)+coefB1*(7th0-3)+coefB2*(7th1-4)
	wmulsl		wr5, wr5, wr13		@ RC(2), coefA2 * 7th line 1-4)
	waddh		wr0, wr0, wr11		@ add 32
	waddh		wr4, wr4, wr5		@ coefA1*(7th0-3) + coefA2*(7th1-4)
	wsrlhg		wr0, wr0, wcgr2		@ >>6, 6th line 0-3 done;

	@ load 8th line
	wldrd		wr10, [r2]			@ RL(4), 8th unaligned 0-7
	wldrd		wr9, [r2, #8]		@ RL(4), 8th unaligned 8-15	
	add			r2, r2, r3;
	wunpckehub	wr3, wr3			@ 7th line 4-7
	wmulsl		wr5, wr3, wr14		@ RC(2), coefB1 * (7th line 4-7)
	wunpckehub	wr2, wr2			@ 6th line 5-8
	wmulsl		wr7, wr2, wr15		@ RC(2), coefB2 * (8th line 5-8)
	walignr0	wr1, wr10, wr9		@ input 8th line 0-7 
	waddh		wr6, wr6, wr5
	walignr1ne	wr9, wr10, wr9		@ input 8th line 1-8 
	waddh		wr6, wr6, wr7		@ coefA1*(6th4-7)+coefA2*(6th5-8)+coefB1*(7th4-7)+coefB2*(7th5-8)
	waddh		wr6, wr6, wr11		@ add 32
	wsrlhg		wr6, wr6, wcgr2		@ >>6, 6th line 4-7 done;
	wmulsl		wr3, wr3, wr12		@ RC(2), coefA1 * (7th line 4-7)
	wpackhus	wr0, wr0, wr6		@ RL(2), 6th line 0-7 ready;
	wmulsl		wr2, wr2, wr13		@ RC(2), coefA2 * (7th line 5-8)
	wstrd		wr0, [r4]			@ store 6th line;
	add			r4, r4, r5
	waddh		wr3, wr3, wr2		@ coefA1*(7th4-7) + coefA2*(7th5-8)
	
	@ used above wr1, wr3, wr4, wr9
	wunpckelub	wr0, wr1			@ 8th line 0-3
	wmulsl		wr2, wr0, wr14		@ RC(2), coefB1 * (8th line 0-3)
	wunpckelub	wr5, wr9			@ 7th line 1-4
	wmulsl		wr7, wr5, wr15		@ RC(2), coefB2 * (8th line 1-4)
	waddh		wr4, wr4, wr2		@ coefA1*(7th0-3)+coefA2*(7th1-4)+coefB1*(8th0-3)
	wmulsl		wr0, wr0, wr12		@ RC(2), coefA1 * (8th line 0-3)
	waddh		wr4, wr4, wr7		@ coefA1*(7th0-3)+coefA2*(7th1-4)+coefB1*(8th0-3)+coefB2*(8th1-4)
	wmulsl		wr5, wr5, wr13		@ RC(2), coefA2 * 8th line 1-4)
	waddh		wr4, wr4, wr11		@ add 32
	waddh		wr0, wr0, wr5		@ coefA1*(8th0-3) + coefA2*(8th1-4)
	wsrlhg		wr4, wr4, wcgr2		@ >>6, 7th line 0-3 done;

	@ load 9th line
	wldrd		wr5, [r2]			@ RL(4), 9th unaligned 0-7
	wldrd		wr2, [r2, #8]		@ RL(4), 9th unaligned 8-15	
	wunpckehub	wr1, wr1			@ 8th line 4-7
	wmulsl		wr6, wr1, wr14		@ RC(2), coefB1 * (8th line 4-7)
	wunpckehub	wr9, wr9			@ 8th line 5-8
	wmulsl		wr7, wr9, wr15		@ RC(2), coefB2 * (9th line 5-8)
	walignr0	wr8, wr5, wr2		@ input 9th line 0-7 
	waddh		wr3, wr3, wr6
	walignr1ne	wr2, wr5, wr2		@ input 9th line 1-8 
	waddh		wr3, wr3, wr7		@ coefA1*(7th4-7)+coefA2*(7th5-8)+coefB1*(8th4-7)+coefB2*(8th5-8)
	waddh		wr3, wr3, wr11		@ add 32
	wsrlhg		wr3, wr3, wcgr2		@ >>6, 7th line 4-7 done;
	wmulsl		wr1, wr1, wr12		@ RC(2), coefA1 * (8th line 4-7)
	wpackhus	wr4, wr4, wr3		@ RL(2), 8th line 0-7 ready;
	wmulsl		wr9, wr9, wr13		@ RC(2), coefA2 * (8th line 5-8)
	wstrd		wr4, [r4]			@ store 7th line;
	add			r4, r4, r5
	waddh		wr1, wr9, wr1		@ coefA1*(8th4-7) + coefA2*(8th5-8)
	
	@ used above wr1, wr3, wr4, wr9
	wunpckelub	wr4, wr8			@ 9th line 0-3
	wmulsl		wr3, wr4, wr14		@ RC(2), coefB1 * (9th line 0-3)
	wunpckelub	wr6, wr2			@ 9th line 1-4
	wmulsl		wr7, wr6, wr15		@ RC(2), coefB2 * (9th line 1-4)
	waddh		wr0, wr0, wr3		@ coefA1*(8th0-3)+coefA2*(8th1-4)+coefB1*(9th0-3)
	waddh		wr0, wr0, wr7		@ coefA1*(8th0-3)+coefA2*(8th1-4)+coefB1*(9th0-3)+coefB2*(9th1-4)
	waddh		wr0, wr0, wr11		@ add 32
	wsrlhg		wr0, wr0, wcgr2		@ >>6, 8th line 0-3 done;

	wunpckehub	wr8, wr8			@ 9th line 4-7
	wmulsl		wr8, wr8, wr14		@ RC(2), coefB1 * (9th line 4-7)
	wunpckehub	wr2, wr2			@ 9th line 5-8
	wmulsl		wr2, wr2, wr15		@ RC(2), coefB2 * (9th line 5-8)
	waddh		wr1, wr1, wr8
	waddh		wr1, wr1, wr2		@ coefA1*(8th4-7)+coefA2*(8th5-8)+coefB1*(9th4-7)+coefB2*(9th5-8)
	waddh		wr1, wr1, wr11		@ add 32
	wsrlhg		wr1, wr1, wcgr2		@ >>6, 8th line 4-7 done;
	wpackhus	wr0, wr0, wr1		@ RL(2), 8th line 0-7 ready;
	@ TBD: stall one cycle;
	wstrd		wr0, [r4]			@ store 8th line;
    ldmia		sp!, {r4-r12, pc}	@ restore and return

L_GB4x4ChromaInBoundContinue:
	@ wr9: coefA1 * (3rd line 0-3) + coefA2 * (3rd line 1-4);
	@ wr2: 1st line 0-3 done;
	@ wr10: 2nd line 0-3 done;

	@ load 4th line
	wldrd		wr0, [r2]			@ RL(4), 4th unaligned 0-7
	wldrd		wr1, [r2, #8]		@ RL(4), 4th unaligned 8-15	
	add			r2, r2, r3;
	wpackhus	wr2, wr2, wr2		@ RL(2), 1st line 0-3 	
	cmp			r8, #8				@ reset the flag;
	wpackhus	wr10, wr10, wr10	@ RL(2), 2nd line 0-3 	
	wstrw		wr2, [r4]			@ store 1st line;
	walignr0	wr3, wr0, wr1		@ input 4th line 0-7 
	add			r4, r4, r5
	walignr1ne	wr1, wr0, wr1		@ input 4th line 1-8
	wstrw		wr10, [r4]			@ store 2nd line;
	add			r4, r4, r5	 

	@ load 5th line
	wldrd		wr4, [r2]			@ RL(4), 5th unaligned 0-7
	wldrd		wr5, [r2, #8]		@ RL(4), 5th unaligned 8-15	
	wunpckelub	wr3, wr3			@ 4th line 0-3
	wmulsl		wr6, wr3, wr14		@ RC(2), coefB1 * (3rd line 0-3)
	wunpckelub	wr1, wr1			@ 4th line 1-4
	wmulsl		wr7, wr1, wr15		@ RC(2), coefB2 * (3rd line 1-4)
	waddh		wr9, wr9, wr6
	waddh		wr9, wr9, wr7		@ coefA1*(3rd0-3)+coefA2*(3rd1-4)+coefB1*(4th0-3)+coefB2*(4th1-4)
	waddh		wr9, wr9, wr11		@ add 32
	wsrlhg		wr9, wr9, wcgr2		@ >>6, 3rd line 0-3 done;
	wpackhus	wr9, wr9, wr9		@ RL(2), 3rd line 0-3 ready;
	wmulsl		wr3, wr3, wr12		@ RC(2), coefA1 * (3rd line 0-3)
	wstrw		wr9, [r4]			@ store 2nd line;
	wmulsl		wr1, wr1, wr13		@ RC(2), coefA2 * (3rd line 1-4)
	add			r4, r4, r5	 

	walignr0	wr9, wr4, wr5		@ input 5th line 0-7 
	waddh		wr1, wr1, wr3		@ coefA1*(4th0-3)+coefA2*(4th1-4)
	walignr1ne	wr5, wr4, wr5		@ input 5th line 1-8 
	wunpckelub	wr9, wr9			@ 5th line 0-3
	wmulsl		wr9, wr9, wr14		@ RC(2), coefB1 * (5th line 0-3)
	wunpckelub	wr5, wr5			@ 5th line 1-4
	wmulsl		wr5, wr5, wr15		@ RC(2), coefB2 * (5th line 1-4)
	waddh		wr9, wr9, wr1		@ 
	waddh		wr9, wr9, wr5		@ 
	waddh		wr9, wr9, wr11		@ add 32
	wsrlhg		wr9, wr9, wcgr2		@ >>6, 4th line 0-3 done;
	wpackhus	wr9, wr9, wr9		@ RL(2), 4th line 0-3 ready;
	@ TBD: stall one cycle;
	wstrw		wr9, [r4]			@ store 7th line;
    ldmia		sp!, {r4-r12, pc}		@ restore and return
	@ 4x4 done;
	@@@@@ 8x8 or 4x4 end;

L_GB2x2ChromaInBound:
	@@@@@ 2x2 start;
    @ r2: refStart
    @ r3: refLnLength
    @ r4: outStart
    @ r5: outLnLength
    @ r9: coefA1
	@ r10: coefA2
    @ r11: coefB1
	@ r12: coefB2

    ldrb    r0, [r2]	                      @  ref0[y0]
    ldrb    r1, [r2, #1]					  @  ref0[y0 + 1]
    ldrb    r6, [r2, #2]					  @  ref0[y0 + 2]
	mul     r0, r0, r9						  @  RL(4), RC(3), coefA1*ref0[y0]
	add		r2, r2, r3
    ldrb    r7, [r2]	                      @  ref1[y0]
    ldrb    r8, [r2, #1]                      @  ref1[y0 + 1]
    mla     r0, r1, r10, r0                   @  RL(4), RC(3), coefA1*ref0[y0] + coefA2*ref0[y0+1]
	@ TBD: stall due to RL and RC;
    mla     r0, r7, r11, r0                   @  RL(4), RC(3), 
    mla     r0, r8, r12, r0                   @  RL(4), RC(3), 
	mul     r1, r1, r9						  @  RL(4), RC(3), coefA1*ref0[y0]
    add     r0, r0, #32                       @  
    mov     r0, r0, asr #6                    @  
    strb    r0, [r4]	                      @  store byteOutStart[y0];

    mla     r1, r6, r10, r1                   @  RL(4), RC(3), coefA1*ref0[y0] + coefA2*ref0[y0+1]
    ldrb    r0, [r2, #2]                      @  ref1[y0 + 2]
	add		r2, r2, r3
	ldrb	r3, [r2]						  @  3rd line ref2[y0]
    mla     r1, r8, r11, r1                   @  RL(4), RC(3),
	ldrb	r6, [r2, #1]					  @  3rd line ref2[y0 + 1]
	ldrb	r2, [r2, #2]					  @  3rd line ref2[y0 + 2] 
    mla     r1, r0, r12, r1                   @  RL(4), RC(3), 
	mul		r7, r7, r9						  @  RL(4), RC(3), 
    add     r1, r1, #32                       @  
    mov     r1, r1, asr #6                    @ 
    strb    r1, [r4, #1]					  @  store byteOutStart[y0 + 1];

    mla     r7, r8, r10, r7                   @  RL(4), RC(3), coefA1*ref0[y0] + coefA2*ref0[y0+1]
	add		r4, r4, r5
    mla     r7, r3, r11, r7                   @  RL(4), RC(3), 
    mla     r7, r6, r12, r7                   @  RL(4), RC(3), 
	mul     r8, r8, r9						  @  RL(4), RC(3), coefA1*ref0[y0]
    add     r7, r7, #32                       @  
    mov     r7, r7, asr #6                    @  
    strb    r7, [r4]	                      @  store byteOutStart[y0];

    mla     r8, r0, r10, r8                   @  RL(4), RC(3), coefA1*ref0[y0] + coefA2*ref0[y0+1]
    mla     r8, r6, r11, r8                   @  RL(4), RC(3), 
    mla     r8, r2, r12, r8                   @  RL(4), RC(3), 
    add     r8, r8, #32                       @  
    mov     r8, r8, asr #6                    @  
    strb    r8, [r4, #1]                      @  store byteOutStart[y0];
	@@@@@ 2x2 end;
    ldmia	sp!, {r4-r12, pc}		@ restore and return

