/************************************************************************
	*																	*
	*		VisualOn, Inc. Confidential and Proprietary, 2003			*
	*																	*
	*		void WMMXSIAvgBlock(avdUInt8 *img,							*
	*							avdUInt8 *tmp_block,					*
	*							avdNativeInt idx8x8,					*
	*							avdNativeInt imgwidth)					*
	*			idx8x8: 2,4, 8,16											*
	*		IL: issue latency; RL: result latency;						*
	*       RC: resource conflict latency								*
	*																	*															*
	************************************************************************/

    .section .text
	.align 2
    .global  WMMXSIAvgBlock
    .type    WMMXSIAvgBlock, function

WMMXSILumaAvgBlock:
    @ r0: img
    @ r1: tmp_block
    @ r2: idx8x8
    @ r3: img width
    
   	stmdb	sp!, {r4-r12, lr}	@ save regs used, sp = sp - 40;
	
	mov			r4, r0			  	@ r4 temp save r0 for store
	cmp			r2,#4
	ble			LumaAVG4x4
	wldrd		wr0,[r0]
	wldrd		wr4,[r1]			@RL4
	cmp			r2,#16
	bne			LumaAVG8x8	
Loop16x16:
	wldrd		wr1,[r0,#8]
	wldrd		wr5,[r1,#8]
	wcmpeqb		wr15,wr0,wr4	@RL(2)
	add			r0,r0,r3
	add			r1,r1,#16
	torcb		r15
	wavg2breq   wr12,wr0,wr4
	wstrdeq		wr12,[r4]

	wldrd		wr0,[r0]		@load next line
	
	wcmpeqb		wr15,wr1,wr5	@RL(2)
	wldrd		wr4,[r1]		@RL4
	torcb		r15
	wavg2breq   wr13,wr1,wr5
	wstrdeq		wr13,[r4,#8]
	
	wldrd		wr1,[r0,#8]
	wldrd		wr5,[r1,#8]
	@@handle next line
	wcmpeqb		wr15,wr0,wr4	@RL(2)
	add			r4,r4,r3
	torcb		r15
	wavg2breq   wr12,wr0,wr4
	wstrdeq		wr12,[r4]
	
	wcmpeqb		wr15,wr1,wr5	@RL(2)
	add			r0,r0,r3
	add			r1,r1,#16
	torcb		r15
	wavg2breq   wr13,wr1,wr5
	wstrdeq		wr13,[r4,#8]

	@@@@@@@@@@@@@@@@@@@@@@@@@@@loop control
	subs		r2,r2,#2
	@add		r0,r0,r3
	@addne		r1,r1,#16
	wldrdne		wr0,[r0]
	wldrdne		wr4,[r1]			@RL4
	addne		r4,r4,r3
	bne			Loop16x16
ldmia	sp!, {r4-r12, pc} @ restore and return
LumaAVG8x8:
	add			r0,r0,r3		@load next line
	add			r1,r1,#16
	wldrd		wr1,[r0]
	
	wcmpeqb		wr15,wr0,wr4	@RL(2)
	wldrd		wr5,[r1]		@RL4
	torcb		r15
	wavg2breq   wr12,wr0,wr4
	wstrdeq		wr12,[r4]

	add			r0,r0,r3
	add			r1,r1,#16
	
	wcmpeqb		wr15,wr1,wr5	@RL(2)
	add			r4,r4,r3
	torcb		r15
	wavg2breq   wr12,wr1,wr5
	wstrdeq		wr12,[r4]
@@@loop control	
	subs		r2,r2,#2
	wldrdne		wr0,[r0]
	wldrdne		wr4,[r1]			@RL4
	addne		r4,r4,r3
	bne			LumaAVG8x8
ldmia	sp!, {r4-r12, pc} @ restore and return
LumaAVG4x4:
	cmp			r2,#2
	beq			LumaAVG2x2		
	wldrw		wr0,[r0]
	wldrw		wr4,[r1]			@RL4
	add			r0,r0,r3			@load second line
	add			r1,r1,#16

	wldrw		wr1,[r0]
	wcmpeqb		wr15,wr0,wr4		@RL(2)
	wldrw		wr5,[r1]			@RL4
	torcb		r15
	wavg2breq   wr12,wr0,wr4
	wstrweq		wr12,[r4]
	@@@second line
	add			r0,r0,r3			@load third line
	add			r1,r1,#16
	wldrw		wr0,[r0]
	wldrw		wr4,[r1]			@RL4

	wcmpeqb		wr15,wr1,wr5		@RL(2)
	add			r4,r4,r3
	torcb		r15
	wavg2breq   wr12,wr1,wr5
	wstrweq		wr12,[r4]
	@@@third line 
	
	add			r0,r0,r3			@load fourth line
	add			r1,r1,#16
	wldrw		wr1,[r0]
	wldrw		wr5,[r1]			@RL4

	wcmpeqb		wr15,wr0,wr4	@RL(2)
	add			r4,r4,r3
	torcb		r15
	wavg2breq   wr12,wr0,wr4
	wstrweq		wr12,[r4]
	@@@forth line

	wcmpeqb		wr15,wr1,wr5	@RL(2)
	add			r4,r4,r3
	torcb		r15
	wavg2breq   wr12,wr1,wr5
	wstrweq		wr12,[r4]
ldmia	sp!, {r4-r12, pc} @ restore and return
LumaAVG2x2:
	wldrh		wr0,[r0]
	wldrh		wr4,[r1]			@RL4
	add			r0,r0,r3			@load second line
	add			r1,r1,#16

	wldrh		wr1,[r0]
	wcmpeqb		wr15,wr0,wr4		@RL(2)
	wldrh		wr5,[r1]			@RL4
	torcb		r15
	wavg2breq   wr12,wr0,wr4
	wstrheq		wr12,[r4]
	wzero		wr14
	@@@second line
	wcmpeqb		wr15,wr1,wr5		@RL(2)
	add			r4,r4,r3
	torcb		r15
	wavg2breq   wr12,wr1,wr5
	wstrheq		wr12,[r4]
ldmia	sp!, {r4-r12, pc} @ restore and return	

	
