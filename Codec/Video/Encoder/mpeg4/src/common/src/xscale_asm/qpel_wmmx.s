/************************************************************************
	*																	*
	*		VisualOn, Inc. Confidential and Proprietary, 2003			*
	*																	*
			void qpel_h_8x8(UInt8 *src,
							 int	srcStep,							
							  UInt8 *dest,					
							  int	destStep,
							  short* table,
							  int   rouding,	
							  int	average,
							  int	loopCount					
								)					
															
	*		IL: issue latency; RL: result latency;						*
	*       RC: resource conflict latency								*
	*																	*	
	qpel_h_8x8(uint8_t *Src, uint8_t *Dst, int32_t SrcPitch, int32_t DstPitch, int32_t height, int32_t rd)														*
	************************************************************************/

    .section .text
	.align 2
    .global  wmmx_Qpel_H
    .type    wmmx_Qpel_H, function


    @ r0: src
    @ r1: srcStep
    @ r2: dest
	@ r3: destStep
    pld		[r0]
   	stmdb	sp!, {r4-r12, lr}	@ save regs used, sp = sp - 40;
	ldr		r10, [sp, #40]	@ table, "orignal sp" = "current sp" + 40;
	ldr		r5, [sp, #44]	@ rouding
	ldr		r7, [sp, #48]	@ loop count
	
	mov		r9,#16			
	mov		r10,r4			@temp store table address	
	@ data alignment;
	and			r8, r0, #7
	bic			r0, r0, #7
	@ preload data to avoid RL;
	wldrd		wr0, [r0]			@ RL(4)
	wldrd		wr1, [r0, #8]		@ RL(4)
	wldrd		wr2, [r0, #16]		
	tmcr		wcgr0, r8			@ RL(3)
	sub			r8, r9, r5
	tbcsth		wr15, r8	
	mov			r8, #5
	tmcr		wcgr1, r8
	mov			r9, r7
	mov			r8, #1
	sub			r8, r8, r5
	tbcstb		wr14, r8				@save 1-rouding
loop_h:	
	mov			r4, r10
	wldrd		wr3, [r4], #8				@ table[0][0..3]
	wldrd		wr4, [r4], #8				@ table[0][4..8]
@	wldrd		wr7, [r4], #8				@ table[1][0..3]
@	wldrd		wr8, [r4], #8				@ table[1][4..8]

	walignr0	wr10, wr0, wr1				@ src[0..7]
	wunpckelub	wr5, wr10					@ src[0..3]
	wunpckehub	wr6, wr10					@ src[4..7]
	
	@0
	wmacsz		wr8, wr3, wr5
	wmacs		wr8, wr4, wr6
	
	wldrd		wr3, [r4], #8				@ table[2][0..3]
	wldrd		wr4, [r4], #8				@ table[2][4..8]
	
	@1
	walignr0	wr10, wr1, wr2				@ src[0..7]
	wunpckelub	wr5, wr10					@ src[0..3]
	wunpckehub	wr6, wr10					@ src[4..7]



	wmacsz		wr9, wr6, wr4
	wldrd		wr0, [r0, #16]				@ load 8..15
	wmacs		wr9,wr7,wr5
	
	
	add			r4,r4,#16
	wldrd		wr6, [r4]			@ table[3][0..3]
	wldrd		wr7, [r4,#8]		@ table[3][4..8]
	
	@wpackdss	wr8,wr8,wr9			@ 1..0
	@walignr0	wr11, wr1, wr0		@ src[8..15]
	@walignr0	wr13, wr0, wr1      @wr13 fisrt byte store src[16]
	
	@2
	wmacsz		wr2,wr2,wr4
	wpackdss	wr8,wr8,wr9			@@ 1..0
	wmacs		wr2,wr3,wr5
	walignr0	wr11, wr1, wr0		@@ src[8..15]
	@3
	wmacsz		wr1,wr6,wr4
	walignr0	wr13, wr0, wr1      @@wr13 fisrt byte store src[16]
	wmacs		wr1,wr7,wr5
	waligni		wr12,wr10,wr11,#1	@@ src[1..8]
@	cmp			r6,#2
@	waddbeq		wr14,wr14,wr12			@up_average,temp store1..8
	wpackdss	wr0,wr2,wr1			@ 3..2
	wunpckelub	wr4,wr12			@@ src[1..4]

	wpackwss	wr0,wr8,wr0			@ 3..0
	wunpckehub	wr5,wr12		    @@ src[5..8]
	waddh		wr0,wr0,wr15
	wsrahg		wr0,wr0,wcgr1
	
	@waligni		wr12,wr10,wr11,#1	@ src[1..8]
	@wunpckelub	wr4,wr12			@ src[1..4]
	@wunpckehub	wr5,wr12			@ src[5..8]
	@4
	wmacsz		wr1,wr6,wr4
	waligni		wr12,wr10,wr11,#2	@@src[2..9]
	wmacs		wr1,wr7,wr5
	@5
	
	wunpckelub	wr4,wr12				
	wunpckehub	wr5,wr12	
	
	wmacsz		wr2,wr6,wr4
	waligni		wr12,wr10,wr11,#3	@@src[3..10]
	wmacs		wr2,wr7,wr5	

	@6
	
	wunpckelub	wr4,wr12				
	wunpckehub	wr5,wr12	
	wpackdss	wr1,wr1,wr2			@ 5..4
	
	wmacsz		wr3,wr6,wr4
	waligni		wr12,wr10,wr11,#4	@@src[4..11]
	wmacs		wr3,wr7,wr5	
	@7
	
	wunpckelub	wr4,wr12				
		
	
	wmacsz		wr2,wr6,wr4
	wunpckehub	wr5,wr12			@@
	wmacs		wr2,wr7,wr5	
	waligni		wr12,wr10,wr11,#5	@@src[5..12]
	wpackdss	wr2,wr3,wr2			@ 7..6
	wunpckelub	wr4,wr12			@@
	wpackwss	wr1,wr1,wr2			@ 7..4
	wunpckehub	wr5,wr12			@@
	waddh		wr1,wr1,wr15
	wsrahg		wr1,wr1,wcgr1
	wpackhus	wr0,wr0,wr1			@ 7..0
@	cmp			r6,#0			
@	wavg2bne	wr0,wr0,wr14
	tbcstbne	wr14,r8				@save 1-rouding
	@8
	
	@wunpckelub	wr4,wr12				
	@wunpckehub	wr5,wr12
	
	wmacsz		wr1,wr6,wr4
	waligni		wr12,wr10,wr11,#6	@@src[6..13]
	wmacs		wr1,wr7,wr5
	
@	cmp			r6,#1				@averqage temp store 8..15
@	waddbeq		wr14,wr14,wr11
	@9
	@waligni	wr12,wr10,wr11,#6	@src[6..13]
	wunpckelub	wr4,wr12				
	wunpckehub	wr5,wr12
	
	wmacsz		wr2,wr6,wr4
	waligni		wr12,wr10,wr11,#7	@src[7..14]
	wmacs		wr2,wr7,wr5	
	
	@10
	
	wunpckelub	wr4,wr12				
	wunpckehub	wr5,wr12
	
	wmacsz		wr8,wr6,wr4
	wpackdss	wr1,wr1,wr2			@ 9..8
	wmacs		wr8,wr7,wr5	

	@11
	@waligni	wr12,wr10,wr11,#7	@src[8..15]
	wunpckelub	wr4,wr11				
	@wunpckehub	wr5,wr11
	
	wmacsz		wr9,wr6,wr4
	wunpckehub	wr5,wr11			@@
	wmacs		wr9,wr7,wr5	
	@@@@@@@@@@@@@@@@@@@@@tmcr		wr12,r12			@get src[16]
	add			r4,r4,#16
	wpackdss	wr8,wr8,wr9			@ 11..10
	waligni		wr12,wr11,wr13,#1	@@src[9..16],wr13 store src[16]

@	cmp			r6,#2
@	waddbeq		wr14,wr14,wr12		@up_average temp store9..16	
	wpackwss	wr1,wr1,wr8			@ 11..8	
	waddh		wr1,wr1,wr15
	wsrahg		wr1,wr1,wcgr1
	
	wldrd		wr8, [r4]			@ table[4][0..3]
	wldrd		wr9, [r4,#8]		@ table[4][4..8]
	
	@12
	wunpckelub	wr4,wr12				
	wunpckehub	wr5,wr12
	
	wmacsz		wr2,wr6,wr4
	add			r4,r4,#16
	wmacs		wr2,wr7,wr5	
	wldrd		wr6, [r4]			@ table[5][0..3]
	@13
	wmacsz		wr3,wr8,wr4
	wldrd		wr7, [r4,#8]		@@ table[5][4..8]

	wmacs		wr3,wr9,wr5
	add			r4,r4,#16
	wpackdss	wr2,wr2,wr3	
	@14
	wmacsz		wr3,wr6,wr4
	wldrd		wr8, [r4]			@ table[6][0..3]
	wldrd		wr9, [r4,#8]		@ table[6][4..8]
	wmacs		wr3,wr7,wr5

	@15
	wmacsz		wr4,wr8,wr4
	mov			r4,r10				@@@
	wmacs		wr4,wr9,wr5
	add		r0,r0,r1				@@src
	wpackdss	wr3,wr3,wr4			@ 15..14
	wstrd		wr0,[r2]			@store to dest r2

	wpackwss	wr2,wr2,wr3			@ 15..12
	waddh		wr2,wr2,wr15
	wsrahg		wr2,wr2,wcgr1
	wpackhus	wr1,wr1,wr2			@ 15..8
@	cmp			r6,#0			
@	wavg2bne	wr1,wr1,wr14
	tbcstbne	wr14,r8				@save 1-rouding
	wstrd		wr1,[r2,#8]
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@loop control
	subs		r9,r9,#1
	wldrdne		wr0, [r0]		@ RL(4)
	wldrdne		wr1, [r0, #8]	@ RL(4)
	addne		r2,r2,r3		@dest
	bne			loop_h
ldmia	sp!, {r4-r12, pc} @ restore and return


	
