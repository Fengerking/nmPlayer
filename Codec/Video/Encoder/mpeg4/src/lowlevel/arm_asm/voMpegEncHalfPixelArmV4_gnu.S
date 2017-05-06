

	.section .text

	.align 4
	.global Inplace16InterpolateHP_ARMV4

@*******************************************************************************	

.EQU		dst_h_OFFSET		,24*17		@dst_h  = dst_v + 24*17
.EQU		dst_hv_OFFSET		,24*17*2	@dst_hv = dst_v + 24*17*2

.EQU		STATCK_SIZE			,12
.EQU		REGISTERS_SAVED		,36
.EQU		OFFSET_i			,0
.EQU		OFFSET_0x01010101	,4
.EQU		OFFSET_0x7F7F7F7F	,8
.EQU		OFFSET_dst_stride	,STATCK_SIZE + REGISTERS_SAVED + 0
.EQU		OFFSET_src_stride	,STATCK_SIZE + REGISTERS_SAVED + 4
.EQU		OFFSET_rounding		,STATCK_SIZE + REGISTERS_SAVED + 8

@global registers:
@r1 : dst_v
@r3 : src
@r11: 0x01010101
@r12: 00x7F7F7F7F
	
@*****************************************************************************************	
	.MACRO	M_inplace16_halfpel_core Offset
@*****************************************************************************************	
	
@   x  x   x  x   x   x   x   x   x   x   x    x   x   x   x   x   x   x   x   x        h/hv half pixel
@  0  0  0  0   0   0   0   0   0   0   0    0   0   0   0   0   0   0   0   O   0		pixel
@   x  x   x  x   x   x   x   x   x   x   x    x   x   x   x   x   x   x   x   x        h/hv half pixel 
@  0  1  2  3   4   5   6   7   8   9   10   11  12  13  14  15  16  17  18  19  20     alignment address

@  0  0  0  0   0   0   0   0   0   0   0    0   0   0   0   0   0   0   0   O   0		pixel
@  x  x  x  x   x   x   x   x   x   x   x    x   x   x   x   x   x   x   x   x   x      v half pixel 
@  0  0  0  0   0   0   0   0   0   0   0    0   0   0   0   0   0   0   0   O   0		pixel
@  0  1  2  3   4   5   6   7   8   9   10   11  12  13  14  15  16  17  18  19  20     alignment address

@  |   0-3  |   |    4-7    |   |    8-11    |   |    12-15  |   |   16-19   |         
	
M_inplace16_halfpel_core_\Offset:	

	@process one row per loop, 17 rows totally
	@process 4 pixels per time, 5 times totally for one row.

	ldr		r10, [sp, #OFFSET_src_stride]
	ldr		r14, [sp, #OFFSET_rounding]
	ldr		r4, [r3]		@a0
	ldr		r5, [r3,#0+4]	@a4
	add		r10, r3, r10
	ldr		r6, [r10]		@b0
	ldr		r7, [r10,#0+4]	@b4
	
inplace16_halfpel_loop_\Offset:	
	
@/**************** alignment pixels 0-3 of cur row ****************/
	
	@process V   (a4,b4)
@if(IsRnd) e ==(a&c) & 0x01010101@		
@else e ==(a|c) & 0x01010101@				
@e+==(a>>1) & 0x7F7F7F7F@					
@e+==(c>>1) & 0x7F7F7F7F@					
		
	cmp		r14, #0			@is rounding?
	
	.IF \Offset != 3
	andne	r8, r4, r6
	orreq	r8, r4, r6
	and		r10, r12, r4, lsr #1
	and		r8, r8, r11
	and		r14, r12, r6, lsr #1
	add		r8, r8, r10
	add		r8, r8, r14
	.ENDIF
	.IF \Offset == 0
	mov		r9, r8, lsr #8
	mov		r10, r8, lsr #16
	mov		r8, r8, lsr #24
	strb	r9, [r1]
	strb	r10, [r1,#1]
	strb	r8, [r1,#2]
	.ENDIF
	.IF \Offset == 1
	mov		r9, r8, lsr #16
	mov		r8, r8, lsr #24
	strb	r9, [r1]
	strb	r8, [r1,#1]
	.ENDIF
	.IF \Offset == 2
	mov		r8, r8, lsr #24
	strb	r8, [r1,#0]
	.ENDIF
	@.IF \Offset == 3
	@ none to process
	@.ENDIF
			
	@process H   (b0,b1)
	mov		r8, r6, lsr #8
	orr		r9, r8, r7, lsl #32-8	@b1
	and		r10, r12, r6, lsr #1
	andne	r8, r6, r9
	orreq	r8, r6, r9
	and		r8, r8, r11
	and		r14, r12, r9, lsr #1
	add		r8, r8, r10
	add		r8, r8, r14
	.IF \Offset == 0
	str 	r8, [r1,#dst_h_OFFSET]
	.ENDIF
	.IF \Offset == 1
	mov		r10, r8, lsr #8
	mov		r14, r8, lsr #16
	mov		r8, r8, lsr #24
	strb	r10, [r1,#dst_h_OFFSET]
	strb	r14, [r1,#dst_h_OFFSET+1]
	strb	r8, [r1,#dst_h_OFFSET+2]
	.ENDIF
	.IF \Offset == 2
	mov		r10, r8, lsr #16
	mov		r8, r8, lsr #24
	strb	r10, [r1,#dst_h_OFFSET]
	strb	r8, [r1,#dst_h_OFFSET+1]
	.ENDIF
	.IF \Offset == 3
	mov		r8, r8, lsr #24
	strb	r8, [r1,#dst_h_OFFSET]
	.ENDIF
	
	@process HV   (a0,a1,b0,b1)

	@PrepareAvg4(b0,b1)
	add		r14, r11, r11, lsl #1	@0x03030303
	and		r8, r6, r14				@(a & 0x03030303)
	and		r10, r9, r14			@(c & 0x03030303)
	add		r8, r8, r10				@q
	and		r6, r0, r6, lsr #2		@(a>>2) & 0x3F3F3F3F@
	and		r9, r0, r9, lsr #2		@(c>>2) & 0x3F3F3F3F@
	add		r6, r6, r9				@a
	@PrepareAvg4(a0,a1)
	mov		r9, r4, lsr #8
	orr		r2, r9, r5, lsl #32-8	@a1
	and		r9, r4, r14				@(a & 0x03030303)
	and		r10, r2, r14			@(c & 0x03030303)
	add		r9, r9, r10				@q
	and		r4, r0, r4, lsr #2		@(a>>2) & 0x3F3F3F3F@
	and		r2, r0, r2, lsr #2		@(c>>2) & 0x3F3F3F3F@
	add		r4, r4, r2				@a
	@Avg4(a,c,g,i,IsRnd)
	add		r6, r6, r4				@a+==g
	add		r9, r8, r9				@c+==i
	add		r9, r9, r11				@c+==i+0x01010101
	addeq	r9, r9, r11				@c+==i+0x02020202
	ldr		r10, [sp, #OFFSET_src_stride]
	and		r9, r14, r9, lsr #2		@(a>>2) & 0x03030303@
	add		r9, r6, r9
	
	mov		r4, r5
	add		r10, r3, r10
	mov		r6, r7
	ldr		r5, [r3, #0+8]	@b0	
	ldr		r7, [r10, #0+8]	@b4	
	
	.IF \Offset == 0
	str 	r9, [r1,#dst_hv_OFFSET]
	.ENDIF
	.IF \Offset == 1
	mov		r10, r9, lsr #8
	mov		r14, r9, lsr #16
	mov		r9, r9, lsr #24
	strb	r10, [r1,#dst_hv_OFFSET]
	strb	r14, [r1,#dst_hv_OFFSET+1]
	strb	r9, [r1,#dst_hv_OFFSET+2]
	.ENDIF
	.IF \Offset == 2
	mov		r10, r9, lsr #16
	mov		r9, r9, lsr #24
	strb	r10, [r1,#dst_hv_OFFSET]
	strb	r9, [r1,#dst_hv_OFFSET+1]
	.ENDIF
	.IF \Offset == 3
	mov		r9, r9, lsr #24
	strb	r9, [r1,#dst_hv_OFFSET]
	.ENDIF
	
@/**************** alignment pixels 4-7 of cur row ****************/

	@process V   (a4,b4)
@if(IsRnd) e ==(a&c) & 0x01010101@		
@else e ==(a|c) & 0x01010101@				
@e+==(a>>1) & 0x7F7F7F7F@					
@e+==(c>>1) & 0x7F7F7F7F@					
	
	andne	r8, r4, r6
	orreq	r8, r4, r6
	and		r10, r12, r4, lsr #1
	and		r8, r8, r11
	and		r14, r12, r6, lsr #1
	add		r8, r8, r10
	add		r8, r8, r14
	.IF \Offset == 3
	str		r8, [r1]
	.ELSE
	mov		r10, r8, lsr #8
	mov		r14, r8, lsr #16
	strb	r8,  [r1,#3-\Offset+0]
	strb	r10, [r1,#3-\Offset+1]
	mov		r8, r8, lsr #24
	strb	r14, [r1,#3-\Offset+2]
	strb	r8,  [r1,#3-\Offset+3]
	.ENDIF
			
	@process H   (b0,b1)
	mov		r8, r6, lsr #8
	orr		r9, r8, r7, lsl #32-8	@b1
	and		r10, r12, r6, lsr #1
	andne	r8, r6, r9
	orreq	r8, r6, r9
	and		r8, r8, r11
	and		r14, r12, r9, lsr #1
	add		r8, r8, r10
	add		r8, r8, r14
	.IF \Offset == 0
	str 	r8, [r1,#dst_h_OFFSET+4]
	.ELSE
	mov		r10, r8, lsr #8
	mov		r14, r8, lsr #16
	strb	r8,  [r1,#dst_h_OFFSET+3-\Offset+1]
	strb	r10, [r1,#dst_h_OFFSET+3-\Offset+2]
	mov		r8, r8, lsr #24
	strb	r14, [r1,#dst_h_OFFSET+3-\Offset+3]
	strb	r8,  [r1,#dst_h_OFFSET+3-\Offset+4]
	.ENDIF
	
	@process HV   (a0,a1,b0,b1)

	@PrepareAvg4(b0,b1)
	add		r14, r11, r11, lsl #1	@0x03030303
	and		r8, r6, r14				@(a & 0x03030303)
	and		r10, r9, r14			@(c & 0x03030303)
	add		r8, r8, r10				@q
	and		r6, r0, r6, lsr #2		@(a>>2) & 0x3F3F3F3F@
	and		r9, r0, r9, lsr #2		@(c>>2) & 0x3F3F3F3F@
	add		r6, r6, r9				@a
	@PrepareAvg4(a0,a1)
	mov		r9, r4, lsr #8
	orr		r2, r9, r5, lsl #32-8	@a1
	and		r9, r4, r14				@(a & 0x03030303)
	and		r10, r2, r14			@(c & 0x03030303)
	add		r9, r9, r10				@q
	and		r4, r0, r4, lsr #2		@(a>>2) & 0x3F3F3F3F@
	and		r2, r0, r2, lsr #2		@(c>>2) & 0x3F3F3F3F@
	add		r4, r4, r2				@a
	@Avg4(a,c,g,i,IsRnd)
	add		r6, r6, r4				@a+==g
	add		r9, r8, r9				@c+==i
	add		r9, r9, r11				@c+==i+0x01010101
	addeq	r9, r9, r11				@c+==i+0x02020202
	ldr		r10, [sp, #OFFSET_src_stride]
	and		r9, r14, r9, lsr #2		@(a>>2) & 0x03030303@
	add		r9, r6, r9
	
	mov		r4, r5
	add		r10, r3, r10
	mov		r6, r7
	ldr		r5, [r3, #0+12]	@b0	
	ldr		r7, [r10, #0+12]@b4	
	
	.IF \Offset == 0
	str 	r9, [r1,#dst_hv_OFFSET+0+4]
	.ELSE
	mov		r10, r9, lsr #8
	mov		r14, r9, lsr #16
	strb	r9,  [r1,#dst_hv_OFFSET+3-\Offset+1]
	strb	r10, [r1,#dst_hv_OFFSET+3-\Offset+2]
	mov		r9, r9, lsr #24
	strb	r14, [r1,#dst_hv_OFFSET+3-\Offset+3]
	strb	r9,  [r1,#dst_hv_OFFSET+3-\Offset+4]
	.ENDIF	
	
@/**************** alignment pixels 8-11 of cur row ****************/

	@process V   (a4,b4)
@if(IsRnd) e ==(a&c) & 0x01010101@		
@else e ==(a|c) & 0x01010101@				
@e+==(a>>1) & 0x7F7F7F7F@					
@e+==(c>>1) & 0x7F7F7F7F@					
	
	andne	r8, r4, r6
	orreq	r8, r4, r6
	and		r10, r12, r4, lsr #1
	and		r8, r8, r11
	and		r14, r12, r6, lsr #1
	add		r8, r8, r10
	add		r8, r8, r14
	.IF \Offset == 3
	str		r8, [r1,#0+4]
	.ELSE
	mov		r10, r8, lsr #8
	mov		r14, r8, lsr #16
	strb	r8,  [r1,#3-\Offset+4+0]
	strb	r10, [r1,#3-\Offset+4+1]
	mov		r8, r8, lsr #24
	strb	r14, [r1,#3-\Offset+4+2]
	strb	r8,  [r1,#3-\Offset+4+3]
	.ENDIF
			
	@process H   (b0,b1)
	mov		r8, r6, lsr #8
	orr		r9, r8, r7, lsl #32-8	@b1
	and		r10, r12, r6, lsr #1
	andne	r8, r6, r9
	orreq	r8, r6, r9
	and		r8, r8, r11
	and		r14, r12, r9, lsr #1
	add		r8, r8, r10
	add		r8, r8, r14
	.IF \Offset == 0
	str 	r8, [r1,#dst_h_OFFSET+0+8]
	.ELSE
	mov		r10, r8, lsr #8
	mov		r14, r8, lsr #16
	strb	r8,  [r1,#dst_h_OFFSET+3-\Offset+4+1]
	strb	r10, [r1,#dst_h_OFFSET+3-\Offset+4+2]
	mov		r8, r8, lsr #24
	strb	r14, [r1,#dst_h_OFFSET+3-\Offset+4+3]
	strb	r8,  [r1,#dst_h_OFFSET+3-\Offset+4+4]
	.ENDIF
		
	@process HV   (a0,a1,b0,b1)

	@PrepareAvg4(b0,b1)
	add		r14, r11, r11, lsl #1	@0x03030303
	and		r8, r6, r14				@(a & 0x03030303)
	and		r10, r9, r14			@(c & 0x03030303)
	add		r8, r8, r10				@q
	and		r6, r0, r6, lsr #2		@(a>>2) & 0x3F3F3F3F@
	and		r9, r0, r9, lsr #2		@(c>>2) & 0x3F3F3F3F@
	add		r6, r6, r9				@a
	@PrepareAvg4(a0,a1)
	mov		r9, r4, lsr #8
	orr		r2, r9, r5, lsl #32-8	@a1
	and		r9, r4, r14				@(a & 0x03030303)
	and		r10, r2, r14			@(c & 0x03030303)
	add		r9, r9, r10				@q
	and		r4, r0, r4, lsr #2		@(a>>2) & 0x3F3F3F3F@
	and		r2, r0, r2, lsr #2		@(c>>2) & 0x3F3F3F3F@
	add		r4, r4, r2				@a
	@Avg4(a,c,g,i,IsRnd)
	add		r6, r6, r4				@a+==g
	add		r9, r8, r9				@c+==i
	add		r9, r9, r11				@c+==i+0x01010101
	addeq	r9, r9, r11				@c+==i+0x02020202
	ldr		r10, [sp, #OFFSET_src_stride]
	and		r9, r14, r9, lsr #2		@(a>>2) & 0x03030303@
	add		r9, r6, r9	
	
	mov		r4, r5
	add		r10, r3, r10
	mov		r6, r7
	ldr		r5, [r3, #0+16]	@b0	
	ldr		r7, [r10, #0+16]@b4	
	
	.IF \Offset == 0
	str 	r9, [r1,#dst_hv_OFFSET+0+8]
	.ELSE
	mov		r10, r9, lsr #8
	mov		r14, r9, lsr #16
	strb	r9,  [r1,#dst_hv_OFFSET+3-\Offset+4+1]
	strb	r10, [r1,#dst_hv_OFFSET+3-\Offset+4+2]
	mov		r9, r9, lsr #24
	strb	r14, [r1,#dst_hv_OFFSET+3-\Offset+4+3]
	strb	r9,  [r1,#dst_hv_OFFSET+3-\Offset+4+4]
	.ENDIF
	
	
@/**************** alignment pixels 12-15 of cur row ****************/

	@process V   (a4,b4)
@if(IsRnd) e ==(a&c) & 0x01010101@		
@else e ==(a|c) & 0x01010101@				
@e+==(a>>1) & 0x7F7F7F7F@					
@e+==(c>>1) & 0x7F7F7F7F@					
	
	andne	r8, r4, r6
	orreq	r8, r4, r6
	and		r10, r12, r4, lsr #1
	and		r8, r8, r11
	and		r14, r12, r6, lsr #1
	add		r8, r8, r10
	add		r8, r8, r14
	.IF \Offset == 3
	str		r8, [r1,#0+8]
	.ELSE
	mov		r10, r8, lsr #8
	mov		r14, r8, lsr #16
	strb	r8,  [r1,#3-\Offset+8+0]
	strb	r10, [r1,#3-\Offset+8+1]
	mov		r8, r8, lsr #24
	strb	r14, [r1,#3-\Offset+8+2]
	strb	r8,  [r1,#3-\Offset+8+3]
	.ENDIF
			
	@process H   (b0,b1)
	mov		r8, r6, lsr #8
	orr		r9, r8, r7, lsl #32-8	@b1
	and		r10, r12, r6, lsr #1
	andne	r8, r6, r9
	orreq	r8, r6, r9
	and		r8, r8, r11
	and		r14, r12, r9, lsr #1
	add		r8, r8, r10
	add		r8, r8, r14
	.IF \Offset == 0
	str 	r8, [r1,#dst_h_OFFSET+0+12]
	.ELSE
	mov		r10, r8, lsr #8
	mov		r14, r8, lsr #16
	strb	r8,  [r1,#dst_h_OFFSET+3-\Offset+8+1]
	strb	r10, [r1,#dst_h_OFFSET+3-\Offset+8+2]
	mov		r8, r8, lsr #24
	strb	r14, [r1,#dst_h_OFFSET+3-\Offset+8+3]
	strb	r8,  [r1,#dst_h_OFFSET+3-\Offset+8+4]
	.ENDIF
	
	@process HV   (a0,a1,b0,b1)

	@PrepareAvg4(b0,b1)
	add		r14, r11, r11, lsl #1	@0x03030303
	and		r8, r6, r14				@(a & 0x03030303)
	and		r10, r9, r14			@(c & 0x03030303)
	add		r8, r8, r10				@q
	and		r6, r0, r6, lsr #2		@(a>>2) & 0x3F3F3F3F@
	and		r9, r0, r9, lsr #2		@(c>>2) & 0x3F3F3F3F@
	add		r6, r6, r9				@a
	@PrepareAvg4(a0,a1)
	mov		r9, r4, lsr #8
	orr		r2, r9, r5, lsl #32-8	@a1
	and		r9, r4, r14				@(a & 0x03030303)
	and		r10, r2, r14			@(c & 0x03030303)
	add		r9, r9, r10				@q
	and		r4, r0, r4, lsr #2		@(a>>2) & 0x3F3F3F3F@
	and		r2, r0, r2, lsr #2		@(c>>2) & 0x3F3F3F3F@
	add		r4, r4, r2				@a
	@Avg4(a,c,g,i,IsRnd)
	add		r6, r6, r4				@a+==g
	add		r9, r8, r9				@c+==i
	add		r9, r9, r11				@c+==i+0x01010101
	addeq	r9, r9, r11				@c+==i+0x02020202
	ldr		r10, [sp, #OFFSET_src_stride]
	and		r9, r14, r9, lsr #2		@(a>>2) & 0x03030303@
	add		r9, r6, r9
	
	mov		r4, r5
	add		r10, r3, r10
	mov		r6, r7
	ldrb	r5, [r3, #0+20]	@b0	
	ldrb	r7, [r10, #0+20]@b4	
	
	.IF \Offset == 0
	str 	r9, [r1,#dst_hv_OFFSET+0+12]
	.ELSE
	mov		r10, r9, lsr #8
	mov		r14, r9, lsr #16
	strb	r9,  [r1,#dst_hv_OFFSET+3-\Offset+8+1]
	strb	r10, [r1,#dst_hv_OFFSET+3-\Offset+8+2]
	mov		r9, r9, lsr #24
	strb	r14, [r1,#dst_hv_OFFSET+3-\Offset+8+3]
	strb	r9,  [r1,#dst_hv_OFFSET+3-\Offset+8+4]
	.ENDIF

@/**************** alignment pixels 16-19 of cur row ****************/

	@process V   (a4,b4)
@if(IsRnd) e ==(a&c) & 0x01010101@		
@else e ==(a|c) & 0x01010101@				
@e+==(a>>1) & 0x7F7F7F7F@					
@e+==(c>>1) & 0x7F7F7F7F@					
		
	andne	r8, r4, r6
	orreq	r8, r4, r6
	and		r10, r12, r4, lsr #1
	and		r8, r8, r11
	and		r14, r12, r6, lsr #1
	add		r8, r8, r10
	add		r8, r8, r14
	.IF \Offset == 0
	mov		r10, r8, lsr #8
	strb	r8, [r1,#3+12]
	strb	r10, [r1,#4+12]
	.ENDIF
	.IF \Offset == 1
	mov		r10, r8, lsr #8
	mov		r14, r8, lsr #16
	strb	r8,  [r1,#2+12]
	strb	r10, [r1,#3+12]
	strb	r14, [r1,#4+12]
	.ENDIF
	.IF \Offset == 2
	mov		r10, r8, lsr #8
	mov		r14, r8, lsr #16
	strb	r8,  [r1,#1+12]
	strb	r10, [r1,#2+12]
	mov		r8, r8, lsr #24
	strb	r14, [r1,#3+12]
	strb	r8,  [r1,#4+12]
	.ENDIF
	.IF \Offset == 3
	str		r8, [r1,#0+12]
	andne	r8, r5, r7
	orreq	r8, r5, r7
	and		r10, r12, r5, lsr #1
	and		r8, r8, r11
	and		r14, r12, r7, lsr #1
	add		r8, r8, r10
	add		r8, r8, r14
	strb	r8, [r1,#0+16]
	.ENDIF
			
	@process H   (b0,b1)
	mov		r8, r6, lsr #8
	orr		r9, r8, r7, lsl #32-8	@b1
	and		r10, r12, r6, lsr #1
	andne	r8, r6, r9
	orreq	r8, r6, r9
	and		r8, r8, r11
	and		r14, r12, r9, lsr #1
	add		r8, r8, r10
	add		r8, r8, r14
	.IF \Offset == 0
	strb 	r8, [r1,#dst_h_OFFSET+0+16]
	.ENDIF
	.IF \Offset == 1
	mov		r10, r8, lsr #8
	strb	r8,  [r1,#dst_h_OFFSET+3+12]
	strb	r10, [r1,#dst_h_OFFSET+4+12]
	.ENDIF
	.IF \Offset == 2
	mov		r10, r8, lsr #8
	mov		r14, r8, lsr #16
	strb	r8,  [r1,#dst_h_OFFSET+2+12]
	strb	r10, [r1,#dst_h_OFFSET+3+12]
	strb	r14, [r1,#dst_h_OFFSET+4+12]
	.ENDIF
	.IF \Offset == 3
	mov		r10, r8, lsr #8
	mov		r14, r8, lsr #16
	strb	r8,  [r1,#dst_h_OFFSET+1+12]
	strb	r10, [r1,#dst_h_OFFSET+2+12]
	mov		r8, r8, lsr #24
	strb	r14, [r1,#dst_h_OFFSET+3+12]
	strb	r8,  [r1,#dst_h_OFFSET+4+12]
	.ENDIF
		
	@process HV   (a0,a1,b0,b1)

	@PrepareAvg4(b0,b1)
	add		r14, r11, r11, lsl #1	@0x03030303
	and		r8, r6, r14				@(a & 0x03030303)
	and		r10, r9, r14			@(c & 0x03030303)
	add		r8, r8, r10				@q
	and		r6, r0, r6, lsr #2		@(a>>2) & 0x3F3F3F3F@
	and		r9, r0, r9, lsr #2		@(c>>2) & 0x3F3F3F3F@
	add		r6, r6, r9				@a
	@PrepareAvg4(a0,a1)
	mov		r9, r4, lsr #8
	orr		r2, r9, r5, lsl #32-8	@a1
	and		r9, r4, r14				@(a & 0x03030303)
	and		r10, r2, r14			@(c & 0x03030303)
	add		r9, r9, r10				@q
	and		r4, r0, r4, lsr #2		@(a>>2) & 0x3F3F3F3F@
	and		r2, r0, r2, lsr #2		@(c>>2) & 0x3F3F3F3F@
	add		r4, r4, r2				@a
	@Avg4(a,c,g,i,IsRnd)
	add		r6, r6, r4				@a+==g
	add		r9, r8, r9				@c+==i
	add		r9, r9, r11				@c+==i+0x01010101
	addeq	r9, r9, r11				@c+==i+0x02020202
	ldr		r8, [sp, #OFFSET_i]
	and		r9, r14, r9, lsr #2		@(a>>2) & 0x03030303@
	add		r6, r6, r9
	
	subs	r8, r8, #1
	
	.IF \Offset == 0
	strb 	r6, [r1,#dst_hv_OFFSET+0+16]
	.ENDIF
	.IF \Offset == 1
	mov		r10, r6, lsr #8
	strb	r6,  [r1,#dst_hv_OFFSET+3+12]
	strb	r10, [r1,#dst_hv_OFFSET+4+12]
	.ENDIF
	.IF \Offset == 2
	mov		r10, r6, lsr #8
	mov		r14, r6, lsr #16
	strb	r6,  [r1,#dst_hv_OFFSET+2+12]
	strb	r10, [r1,#dst_hv_OFFSET+3+12]
	strb	r14, [r1,#dst_hv_OFFSET+4+12]
	.ENDIF
	.IF \Offset == 3
	mov		r10, r6, lsr #8
	mov		r14, r6, lsr #16
	strb	r6,  [r1,#dst_hv_OFFSET+1+12]
	strb	r10, [r1,#dst_hv_OFFSET+2+12]
	mov		r6, r6, lsr #24
	strb	r14, [r1,#dst_hv_OFFSET+3+12]
	strb	r6,  [r1,#dst_hv_OFFSET+4+12]
	.ENDIF

	ldrge	r9, [sp, #OFFSET_src_stride]
	ldrge	r14, [sp, #OFFSET_rounding]
	strge	r8, [sp, #OFFSET_i]
	add		r1, r1, #24
	add		r3, r3, r9
	ldrge	r4, [r3]		@a0
	add		r10, r3, r9
	ldrge	r5, [r3,#0+4]	@a4
	ldrge	r6, [r10]		@b0
	ldrge	r7, [r10,#0+4]	@b4
	
	bge		inplace16_halfpel_loop_\Offset		@17 rows loop
	
	add		sp, sp, #STATCK_SIZE		
    ldmia	sp!, {r4-r11,pc}		
	
	.ENDM	@M_inplace16_halfpel_core
		
	
	
@*******************************************************************************	
@inplace16_interpolate_halfpel_c(uint8_t * const dst_h,
@								uint8_t * const dst_v,
@								uint8_t * const dst_hv,
@								const uint8_t * const src,
@								const int32_t dst_stride,
@								const int32_t src_stride,
@								const uint32_t rounding)
@*******************************************************************************	
		   
	.align 4
Inplace16InterpolateHP_ARMV4:

	stmdb	sp!, {r4-r11,lr}
			
	sub		r3, r3, #1	
	sub		sp, sp, #STATCK_SIZE	
	mov		r4, #16
	ldr		r6, [sp, #OFFSET_src_stride]
	str		r4, [sp, #OFFSET_i]
	and		r7, r3, #3	
	mov		r0, #0x3f
	mov		r11, #0x01
	bic		r3, r3, #3
	orr		r0, r0, r0, lsl #8
	orr		r11, r11, r11, lsl #8
	mov		r12, #0x7f
	orr		r11, r11, r11, lsl #16
	orr		r12, r12, r12, lsl #8
	orr		r0, r0, r0, lsl #16
	sub		r3, r3, r6		
	orr		r12, r12, r12, lsl #16
	
	ldr		pc, [pc, r7, lsl #2]
	nop
	.WORD		M_inplace16_halfpel_core_0
	.WORD		M_inplace16_halfpel_core_1
	.WORD		M_inplace16_halfpel_core_2
	.WORD		M_inplace16_halfpel_core_3

	M_inplace16_halfpel_core 0 
	M_inplace16_halfpel_core 1
	M_inplace16_halfpel_core 2
	M_inplace16_halfpel_core 3

	.END