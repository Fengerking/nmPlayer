	@AREA	 .rdata , DATA, READONLY
	.text
	.globl	 _voPackUV	
	.globl  _voPackUV_FLIP_Y
	.align 4
	
_voPackUV_FLIP_Y: @PROC
	stmdb       sp!, {r4 - r11, lr}
	ldr	r4, [sp, #36]
	ldr	r12, [sp, #40]
	ldr	r14, [sp, #44]
	ldr	r6, [sp, #48]
	sub r5, r12, #1
	mla r0, r5, r6, r0

	sub	r3, r3, r14		@offsetU
	sub	r4, r4, r14		@offsetV	
UV_FYH_loop:
	mov	r4, r14, asr #3
UV_FYW_loop:
	ldrd			r6, [r1], #8		@u0~3 4~7
	ldrd			r8, [r2], #8		@v0~3 4~7

	pkhbt			r5, r6, r8, lsl #16	@v1v0u1u0	
	pkhtb			r11, r8, r6, asr #16	@v3v2u3u2		
	uxtb16		r6, r5			@[]v0[]u0
	uxtb16		r8, r5, ror #8		@[]v1[]u1
	uxtb16		r5, r11			@[]v2[]u2
	uxtb16		r11, r11, ror #8	@[]v3[]u3
	
	orr			r6, r6, r6, asr #8	@[]v0v0u0
	orr			r8, r8, r8, asr #8	@[]v1v1u1
	orr			r5, r5, r5, asr #8	@[]v0v0u0	
	orr			r11, r11, r11, asr #8	@[]v1v1u1	
	pkhbt			r10, r6, r8, lsl #16	@v1u1v0u0
	pkhbt			r11, r5, r11, lsl #16	@v1u1v0u0
	pkhbt			r5, r7, r9, lsl #16	@v1v0u1u0			
	strd			r10, [r0], #8		@v1u1v0u0 v3u3v2u2
@tow	
	pkhtb			r11, r9, r7, asr #16	@v3v2u3u2		
	uxtb16		r6, r5			@[]v0[]u0
	uxtb16		r8, r5, ror #8		@[]v1[]u1
	uxtb16		r5, r11			@[]v2[]u2
	uxtb16		r11, r11, ror #8	@[]v3[]u3
	
	orr			r6, r6, r6, asr #8	@[]v0v0u0
	orr			r8, r8, r8, asr #8	@[]v1v1u1
	orr			r5, r5, r5, asr #8	@[]v0v0u0
	orr			r11, r11, r11, asr #8	@[]v1v1u1		
	pkhbt			r10, r6, r8, lsl #16	@v1u1v0u0
	pkhbt			r11, r5, r11, lsl #16	@v1u1v0u0	
	subs        		r4, r4, #1		
	strd			r10, [r0], #8		@v1u1v0u0 v3u3v2u2
	bne         UV_FYW_loop

	ands r4, r14, #0x07
	beq         UV_FYW_loop_out
		
UV_FYW_loop2:	
	ldrb			r6, [r1], #1
	ldrb			r7, [r2], #1				
	subs      r4, r4, #1
	strb			r6, [r0], #1
	strb			r7, [r0], #1		
	bne         UV_FYW_loop2
	
UV_FYW_loop_out:	
	ldr			r6, [sp, #48]		@stridedest	
	ldr			r14, [sp, #44]	
	add 		r1, r1, r3	
	add 		r2, r2, r3
	add			r8, r6, r14, lsl #1	@offsetdest
	sub 		r0, r0, r8			
	subs    r12, r12, #1	
			
	bne     UV_FYH_loop

	ldmia   sp!, {r4 - r11, pc}
	
_voPackUV: @PROC
	stmdb       sp!, {r4 - r11, lr}
	ldr	r14, [sp, #44]	
	ldr	r4, [sp, #36]
	ldr	r12, [sp, #40]
	sub	r3, r3, r14		@offsetU
	sub	r4, r4, r14		@offsetV	
UV_Height_loop:

	mov	r4, r14, asr #3

UV_Width_loop:

	ldrd			r6, [r1], #8		@u0~3 4~7
	ldrd			r8, [r2], #8		@v0~3 4~7

	pkhbt			r5, r6, r8, lsl #16	@v1v0u1u0	
	pkhtb			r11, r8, r6, asr #16	@v3v2u3u2		
	uxtb16		r6, r5			@[]v0[]u0
	uxtb16		r8, r5, ror #8		@[]v1[]u1
	uxtb16		r5, r11			@[]v2[]u2
	uxtb16		r11, r11, ror #8	@[]v3[]u3
	
	orr			r6, r6, r6, asr #8	@[]v0v0u0
	orr			r8, r8, r8, asr #8	@[]v1v1u1
	orr			r5, r5, r5, asr #8	@[]v0v0u0	
	orr			r11, r11, r11, asr #8	@[]v1v1u1	
	pkhbt			r10, r6, r8, lsl #16	@v1u1v0u0
	pkhbt			r11, r5, r11, lsl #16	@v1u1v0u0
	pkhbt			r5, r7, r9, lsl #16	@v1v0u1u0		
	strd			r10, [r0], #8		@v1u1v0u0 v3u3v2u2
@tow	
	pkhtb			r11, r9, r7, asr #16	@v3v2u3u2		
	uxtb16		r6, r5			@[]v0[]u0
	uxtb16		r8, r5, ror #8		@[]v1[]u1
	uxtb16		r5, r11			@[]v2[]u2
	uxtb16		r11, r11, ror #8	@[]v3[]u3
	
	orr			r6, r6, r6, asr #8	@[]v0v0u0
	orr			r8, r8, r8, asr #8	@[]v1v1u1
	orr			r5, r5, r5, asr #8	@[]v0v0u0
	orr			r11, r11, r11, asr #8	@[]v1v1u1		
	pkhbt			r10, r6, r8, lsl #16	@v1u1v0u0
	pkhbt			r11, r5, r11, lsl #16	@v1u1v0u0	
	subs        		r4, r4, #1
	strd			r10, [r0], #8		@v1u1v0u0 v3u3v2u2
	bne         UV_Width_loop

	ands r4, r14, #0x07
	beq         UV_Width_loop_out
		
UV_Width_loop2:	
	ldrb			r6, [r1], #1
	ldrb			r7, [r2], #1				
	subs        		r4, r4, #1
	strb			r6, [r0], #1
	strb			r7, [r0], #1		
	bne         UV_Width_loop2
	
UV_Width_loop_out:	
	ldr	r6, [sp, #48]		@stridedest	
	ldr			r14, [sp, #44]	
	add        		r1, r1, r3	
	add        		r2, r2, r3
	sub			r8, r6, r14, lsl #1	@offsetdest				
	subs        		r12, r12, #1
	add        		r0, r0, r8	
			
	bne         UV_Height_loop

	ldmia       sp!, {r4 - r11, pc}
			
