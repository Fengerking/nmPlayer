@AREA	 .rdata , DATA, READONLY
	.section .rdata
	.global	 voPackUV	
	.global	 voPackUV_FLIP_Y
	@AREA	 .text , CODE, READONLY
	.section .text

PackUV_V7:
@void PackUV(unsigned char* dstUV, unsigned char* srcU, unsigned char* srcV, int strideU, int strideV, int rows, int width, int stridedest)
@{
@
@    unsigned char* pUV = (unsigned char*) dstUV@
@    unsigned char* pU = (unsigned char*) srcU@
@    unsigned char* pV = (unsigned char*) srcV@
@    int offsetU = strideU - width@
@    int offsetV = strideV - width@
	@    int offsetdest = stridedest - width*2@
@    for (int i = 0@ i < rows@ i++)
@    {
@        for (int j = 0@ j < width@ j++)
@        {
@            *pUV++ = *pU++@
@            *pUV++ = *pV++@
@        }
@        pU += offsetU@
@        pV += offsetV@
	@	 pUV+=offsetdest@
@    }
@} 
@r0 = dstuv, r1 = srcu, r2 = srcv, r3 = strideu
@r4[sp, #36] = stridev, r5[sp, #40] = rows, r14[sp, #44] = width, r7[sp, #48] =stridedest
	stmdb       sp!, {r4 - r11, lr}	
	ldr	r4, [sp, #36]
	ldr	r5, [sp, #40]
	ldr	r7, [sp, #48]
	LSL	r6, r3, #2
	LSL	r8, r4, #2
	LSL	r9, r7, #2
	sub	r6, r6, #16
	sub	r8, r8, #16
	sub	r9, r9, #32				
@r0~7 r10~r14	
UV_Height_loop:
	ldr	r14, [sp, #44]
@1 line
	mov	r11, r1
	mov	r12, r2	
	mov	r10, r0

				
UV_Width_loop:
	VLD1.64  {q0},[r11], r3		@u
	VLD1.64  {q1},[r12], r4		@v
	 
	VLD1.64  {q2},[r11], r3		@u
	VLD1.64  {q3},[r12], r4		@v
	
	VLD1.64  {q4},[r11], r3		@u
	VLD1.64  {q5},[r12], r4		@v
	
	VLD1.64  {q6},[r11], r3		@u
	VLD1.64  {q7},[r12], r4		@v
	
	VTRN.8  q0, q1
	VTRN.8  q2, q3
	VTRN.8  q4, q5	
	VTRN.8  q6, q7
	
			
	VTRN.16  q0, q1	
	VTRN.16  q2, q3	
	VTRN.16  q4, q5	
	VTRN.16  q6, q7	
			
	VTRN.32  q0, q1
	VTRN.32  q2, q3	
	VTRN.32  q4, q5	
	VTRN.32  q6, q7
		
	VSWP d1, d2	
	VSWP d5, d6	
	VSWP d9, d10	
	VSWP d13, d14									
	
	VST1.64  {q0, q1}, [r10], r7	@uv
	VST1.64  {q2, q3}, [r10], r7	@uv
	VST1.64  {q4, q5}, [r10], r7	@uv
	VST1.64  {q6, q7}, [r10], r7	@uv				
	subs        		r14, r14, #16
	sub        		r11, r11, r6
	sub        		r12, r12, r8
	sub        		r10, r10, r9		
	bne         UV_Width_loop
			
	add        		r1, r1, r6
	add        		r2, r2, r8
	add        		r0, r0, r9
	add	r1, r1, #16
	add	r2, r2, #16
	add	r0, r0, #32						
	subs        		r5, r5, #4			
	bne         UV_Height_loop

	ldmia       sp!, {r4 - r11, pc}

PackUV_V6:
@void PackUV(unsigned char* dstUV, unsigned char* srcU, unsigned char* srcV, int strideU, int strideV, int rows, int width, int stridedest)
@{
@
@    unsigned char* pUV = (unsigned char*) dstUV@
@    unsigned char* pU = (unsigned char*) srcU@
@    unsigned char* pV = (unsigned char*) srcV@
@    int offsetU = strideU - width@
@    int offsetV = strideV - width@
	@    int offsetdest = stridedest - width*2@
@    for (int i = 0@ i < rows@ i++)
@    {
@        for (int j = 0@ j < width@ j++)
@        {
@            *pUV++ = *pU++@
@            *pUV++ = *pV++@
@        }
@        pU += offsetU@
@        pV += offsetV@
	@	 pUV+=offsetdest@
@    }
@} 
@r0 = dstuv, r1 = srcu, r2 = srcv, r3 = strideu
@[sp, #36] = stridev, [sp, #40] = rows, [sp, #44] = width
	stmdb       sp!, {r4 - r11, lr}
	ldr	r14, [sp, #44]	
	ldr	r4, [sp, #36]
	ldr	r12, [sp, #40]
	sub	r3, r3, r14		@offsetU
	sub	r4, r4, r14		@offsetV	
UV_Height_loop_V6:

	movs	r4, r14, asr #3
	beq	lt_than_8

UV_Width_loop_V6:

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
	bne         UV_Width_loop_V6

lt_than_8:
	ands r4, r14, #0x07
	beq         UV_Width_loop_V6_out
		
UV_Width_loop_V62:	
	ldrb			r6, [r1], #1
	ldrb			r7, [r2], #1				
	subs        		r4, r4, #1
	strb			r6, [r0], #1
	strb			r7, [r0], #1		
	bne         UV_Width_loop_V62
	
UV_Width_loop_V6_out:	
	ldr	r6, [sp, #48]		@stridedest	
	ldr			r14, [sp, #44]	
	add        		r1, r1, r3	
	add        		r2, r2, r3
	sub			r8, r6, r14, lsl #1	@offsetdest				
	subs        		r12, r12, #1
	add        		r0, r0, r8	
			
	bne         UV_Height_loop_V6

	ldmia       sp!, {r4 - r11, pc}
		
		
voPackUV: @PROC
		PUSH     {r4-r11,lr}
		SUB      sp,sp,#0x1c
		MOV      r4,r0
		MOV      r5,r1
		MOV      r6,r2
		MOV      r7,r3
		LDRD     r8,r9,[sp,#0x44]
		BIC      r10,r9,#0xf
		AND      r0,r9,#0xf
		STR      r0,[sp,#0x18]
		BIC      r11,r8,#3
		AND      r0,r8,#3
		STR      r0,[sp,#0x14]
		CMP      r10,#0
		BEQ      Lab_B31C
		CMP      r11,#0
		BEQ      Lab_B31C
		LDR      r0,[sp,#0x40]
		LDR      r3,[sp,#0x4c]
		STR      r3,[sp,#0xc]
		MOV      r3,r7
		MOV      r2,r6
		MOV      r1,r5
		STM      sp,{r0,r11}
		MOV      r0,r4
		STR      r10,[sp,#8]
		BL       PackUV_V7
		LDR      r0,[sp,#0x18]
		CMP      r0,#0
		BEQ      Lab_B2E4
		LDR      r0,[sp,#0x40]
		LDR      r2,[sp,#0x18]
		LDR      r3,[sp,#0x4c]
		STRD     r2,r3,[sp,#8]
		ADD      r2,r6,r10
		ADD      r1,r5,r10
		STM      sp,{r0,r11}
		ADD      r0,r4,r10,LSL #1
		MOV      r3,r7
		BL       PackUV_V6           
Lab_B2E4:   
		LDR      r0,[sp,#0x14]          
		CMP      r0,#0
		BEQ      Lab_B340
		LDR      r0,[sp,#0x40]
		LDR      r1,[sp,#0x14]
		LDR      r3,[sp,#0x4c]
		STM      sp,{r0,r1,r9}
		MLA      r2,r0,r11,r6
		MLA      r1,r7,r11,r5
		STR      r3,[sp,#0xc]
		MLA      r0,r3,r11,r4
		MOV      r3,r7
		BL       PackUV_V6
		B        Lab_B340
Lab_B31C:   
		LDR      r0,[sp,#0x40]
		LDR      r3,[sp,#0x4c]
		STR      r3,[sp,#0xc]
		MOV      r3,r7
		MOV      r2,r6
		MOV      r1,r5
		STM      sp,{r0,r8,r9}
		MOV      r0,r4
		BL       PackUV_V6
Lab_B340: 
		ADD      sp,sp,#0x1c
		POP      {r4-r11,pc}         			
		
PackUV_FY_V7:
@void PackUV(unsigned char* dstUV, unsigned char* srcU, unsigned char* srcV, int strideU, int strideV, int rows, int width, int stridedest) 
@r0 = dstuv, r1 = srcu, r2 = srcv, r3 = strideu
@r4[sp, #36] = stridev, r5[sp, #40] = rows, r14[sp, #44] = width, r7[sp, #48] =stridedest
	stmdb       sp!, {r4 - r11, lr}	
	ldr	r4, [sp, #36]
	ldr	r5, [sp, #40]
	ldr	r7, [sp, #48]
	sub r6, r5, #1
	mla r0, r6, r7, r0
	LSL	r6, r3, #2
	LSL	r8, r4, #2
	LSL	r9, r7, #2
	sub	r6, r6, #16
	sub	r8, r8, #16
	add	r9, r9, #32				
@r0~7 r10~r14	
UV_FY_Height_loop:
	ldr	r14, [sp, #44]
@1 line
	mov	r11, r1
	mov	r12, r2	
	mov	r10, r0

				
UV_FY_Width_loop:
	VLD1.64  {q0},[r11], r3		@u
	VLD1.64  {q1},[r12], r4		@v
	 
	VLD1.64  {q2},[r11], r3		@u
	VLD1.64  {q3},[r12], r4		@v
	
	VLD1.64  {q4},[r11], r3		@u
	VLD1.64  {q5},[r12], r4		@v
	
	VLD1.64  {q6},[r11], r3		@u
	VLD1.64  {q7},[r12], r4		@v
	
	VTRN.8  q0, q1
	VTRN.8  q2, q3
	VTRN.8  q4, q5	
	VTRN.8  q6, q7
	
			
	VTRN.16  q0, q1	
	VTRN.16  q2, q3	
	VTRN.16  q4, q5	
	VTRN.16  q6, q7	
			
	VTRN.32  q0, q1
	VTRN.32  q2, q3	
	VTRN.32  q4, q5	
	VTRN.32  q6, q7
		
	VSWP d1, d2	
	VSWP d5, d6	
	VSWP d9, d10	
	VSWP d13, d14									
	
	VST1.64  {q0, q1}, [r10]	@uv
	sub      r10, r10, r7
	VST1.64  {q2, q3}, [r10]	@uv
	sub      r10, r10, r7
	VST1.64  {q4, q5}, [r10]	@uv
	sub      r10, r10, r7
	VST1.64  {q6, q7}, [r10]	@uv	
	sub      r10, r10, r7			
	subs     r14, r14, #16
	sub      r11, r11, r6
	sub      r12, r12, r8
	add      r10, r10, r9		
	bne      UV_FY_Width_loop
			
	add      r1, r1, r6
	add      r2, r2, r8
	sub      r0, r0, r9
	add      r1, r1, #16
	add      r2, r2, #16
	add	     r0, r0, #32						
	subs     r5, r5, #4			
	bne      UV_FY_Height_loop

	ldmia       sp!, {r4 - r11, pc}

PackUV_FY_V6:
@void PackUV(unsigned char* dstUV, unsigned char* srcU, unsigned char* srcV, int strideU, int strideV, int rows, int width, int stridedest)
@r0 = dstuv, r1 = srcu, r2 = srcv, r3 = strideu
@[sp, #36] = stridev, [sp, #40] = rows, [sp, #44] = width
	stmdb       sp!, {r4 - r11, lr}
	ldr	r12, [sp, #40]
	ldr r14, [sp, #48]
	sub r4, r12 , #1
	mla r0, r4, r14, r0
	ldr	r14, [sp, #44]	
	ldr	r4, [sp, #36]
	
	sub	r3, r3, r14		@offsetU
	sub	r4, r4, r14		@offsetV	
UV_FY_Height_loop_V6:

	movs	r4, r14, asr #3
	beq	lt_FY_than_8

UV_FY_Width_loop_V6:

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
	bne         UV_FY_Width_loop_V6

lt_FY_than_8:
	ands r4, r14, #0x07
	beq         UV_FY_Width_loop_V6_out
		
UV_FY_Width_loop_V62:	
	ldrb			r6, [r1], #1
	ldrb			r7, [r2], #1				
	subs      r4, r4, #1
	strb			r6, [r0], #1
	strb			r7, [r0], #1		
	bne       UV_FY_Width_loop_V62
	
UV_FY_Width_loop_V6_out:	
	ldr	r6, [sp, #48]		@stridedest	
	ldr			r14, [sp, #44]	
	add     r1, r1, r3	
	add     r2, r2, r3
	add			r8, r6, r14, lsl #1	@offsetdest				
	subs    r12, r12, #1
	sub     r0, r0, r8	
			
	bne     UV_FY_Height_loop_V6

	ldmia   sp!, {r4 - r11, pc}
		
		
voPackUV_FLIP_Y: @PROC
		PUSH     {r4-r11,lr}
		SUB      sp,sp,#0x1c
		MOV      r4,r0
		MOV      r5,r1
		MOV      r6,r2
		MOV      r7,r3
		LDRD     r8,r9,[sp,#0x44]
		BIC      r10,r9,#0xf
		AND      r0,r9,#0xf
		STR      r0,[sp,#0x18]
		BIC      r11,r8,#3
		AND      r0,r8,#3
		STR      r0,[sp,#0x14]
		CMP      r10,#0
		BEQ      Lab_FY_B31C
		CMP      r11,#0
		BEQ      Lab_FY_B31C
		LDR      r0,[sp,#0x40]
		LDR      r3,[sp,#0x4c]
		STR      r3,[sp,#0xc]
		MOV      r3,r7
		MOV      r2,r6
		MOV      r1,r5
		STM      sp,{r0,r11}
		MOV      r0,r4
		STR      r10,[sp,#8]
		BL       PackUV_FY_V7
		LDR      r0,[sp,#0x18]
		CMP      r0,#0
		BEQ      Lab_FY_B2E4
		LDR      r0,[sp,#0x40]
		LDR      r2,[sp,#0x18]
		LDR      r3,[sp,#0x4c]
		STRD     r2,r3,[sp,#8]
		ADD      r2,r6,r10
		ADD      r1,r5,r10
		STM      sp,{r0,r11}
		ADD      r0,r4,r10,LSL #1
		MOV      r3,r7
		BL       PackUV_FY_V6           
Lab_FY_B2E4:   
		LDR      r0,[sp,#0x14]          
		CMP      r0,#0
		BEQ      Lab_B340
		LDR      r0,[sp,#0x40]
		LDR      r1,[sp,#0x14]
		LDR      r3,[sp,#0x4c]
		STM      sp,{r0,r1,r9}
		MLA      r2,r0,r11,r6
		MLA      r1,r7,r11,r5
		STR      r3,[sp,#0xc]
		MLA      r0,r3,r11,r4
		MOV      r3,r7
		BL       PackUV_FY_V6
		B        Lab_FY_B340
Lab_FY_B31C:   
		LDR      r0,[sp,#0x40]
		LDR      r3,[sp,#0x4c]
		STR      r3,[sp,#0xc]
		MOV      r3,r7
		MOV      r2,r6
		MOV      r1,r5
		STM      sp,{r0,r8,r9}
		MOV      r0,r4
		BL       PackUV_FY_V6
Lab_FY_B340: 
		ADD      sp,sp,#0x1c
		POP      {r4-r11,pc}   