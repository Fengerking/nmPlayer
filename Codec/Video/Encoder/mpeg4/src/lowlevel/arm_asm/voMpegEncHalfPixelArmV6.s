
	AREA	|.text|, CODE

	EXPORT	SubBlock_ARMV6
	EXPORT	SubBlockHor_ARMV6	;This function will be not bit-complaint with C function when pixel value is 0xFF.See detail comments in ASM code.
	EXPORT	SubBlockVer_ARMV6	;This function will be not bit-complaint with C function when pixel value is 0xFF.See detail comments in ASM code.
	EXPORT	SubBlockHorVer_ARMV6
	EXPORT	SubBlockHorRound_ARMV6
	EXPORT	SubBlockVerRound_ARMV6
	EXPORT	SubBlockHorVerRound_ARMV6
	

;*****************************************************************************************	
	MACRO
	M_SubBlock_core	$Pos
;*****************************************************************************************	
;R0 Src
;R1 dct_data
;R2 Dst
;R3 SrcPitch
;R12 offset
;lr loop
		
SubBlock_loop_$Pos
	
	IF	$Pos = 0
	ldr		r5, [r0, #4]		
	ldr		r4, [r0], r3		;r4 = s3 s2 s1 s0
	ldrd	r6, [r2]			;r6 = d3 d2 d1 d0
	subs	lr, lr, #1	
	ELSE
	ldr		r4, [r0]			;r4 = s3 s2 s1 s0
	ldr		r5, [r0, #4]		
	ldr		r9, [r0, #8]		
	ldrd	r6, [r2]			;r6 = d3 d2 d1 d0
	rsb		r10, r12, #32
	add		r0, r0, r3
	subs	lr, lr, #1	
	mov		r4, r4, lsr r12
	orr		r4, r4, r5, lsl r10
	mov		r5, r5, lsr r12
	orr		r5, r5, r9, lsl r10
	ENDIF
	
	pld		[r0]
	pld		[r2, r3]
	uxtb16	r8, r4				;r8 = 0 s2 0 s0
	uxtb16	r9, r4, ror #8		;r9 = 0 s3 0 s1
	uxtb16	r10, r6				;r10 = 0 d2 0 d0
	uxtb16	r11, r6, ror #8		;r11 = 0 d3 0 d1
	ssub16	r10, r10, r8		;r10 = 0 c2 0 c0	;c = d - s
	ssub16	r11, r11, r9		;r11 = 0 c3 0 c1
	strd	r4, [r2], r3		;store src to dst	
	pkhbt	r8, r10, r11, lsl #16	;r8 = 0 c1 0 c0
	pkhtb	r9, r11, r10, asr #16	;r9 = 0 c3 0 c2
	strd	r8, [r1], #8	
	uxtb16	r8, r5				;r8 = 0 s2 0 s0
	uxtb16	r9, r5, ror #8		;r9 = 0 s3 0 s1
	uxtb16	r10, r7				;r10 = 0 d2 0 d0
	uxtb16	r11, r7, ror #8		;r11 = 0 d3 0 d1
	ssub16	r10, r10, r8		;r10 = 0 c2 0 c0	;c = d - s
	ssub16	r11, r11, r9		;r11 = 0 c3 0 c1
	pkhbt	r8, r10, r11, lsl #16	;r8 = 0 c1 0 c0
	pkhtb	r9, r11, r10, asr #16	;r9 = 0 c3 0 c2
	strd	r8, [r1], #8
	bgt		SubBlock_loop_$Pos	
	
	MEND	;M_SubBlock_core

;*****************************************************************************************	
	MACRO
	M_SubBlockHor_core	$Pos, $RND
;*****************************************************************************************	
;R0 Src
;R1 dct_data
;R2 Dst
;R3 SrcPitch
;R12 offset
;lr loop
		
	;It can't define label with two paramters(such as SubBlockHor_loop_$RND_$Pos) when builded with RVDS4,so define it as below.
	;But it can pass vs2005 compiler.
	IF	$RND = 1
SubBlockHor_loop_Rnd1_$Pos
	ELSE
SubBlockHor_loop_Rnd0_$Pos
	ENDIF
	
	IF	$Pos = 0
	ldr		r5, [r0, #4]		
	ldrb	r9, [r0, #8]		
	ldr		r4, [r0], r3		;r4 = s3 s2 s1 s0
	ldrd	r6, [r2]			;r6 = d3 d2 d1 d0
	subs	lr, lr, #1	
	ELSE
	ldr		r4, [r0]			;r4 = s3 s2 s1 s0
	ldr		r5, [r0, #4]		
	ldr		r9, [r0, #8]		
	ldrd	r6, [r2]			;r6 = d3 d2 d1 d0
	rsb		r10, r12, #32
	add		r0, r0, r3
	subs	lr, lr, #1	
	mov		r4, r4, lsr r12
	orr		r4, r4, r5, lsl r10
	mov		r5, r5, lsr r12
	orr		r5, r5, r9, lsl r10
	mov		r9, r9, lsr r12
	ENDIF
	
	pld		[r0]
	pld		[r2, r3]
	IF	$RND = 1
	mov		r11, #0xff
	mov		r8, r4, lsr #8
	add		r11, r11, #2
	mov		r10, r5, lsr #8
	orr		r11, r11, r11, lsl #16		;0x01010101
	ELSE
	mov		r8, r4, lsr #8
	mov		r10, r5, lsr #8
	ENDIF
	orr		r8, r8, r5, lsl #32-8
	orr		r9, r10, r9, lsl #32-8
	IF	$RND = 1
	uqadd8	r4, r4, r11		;here will cause not bit-complaint with C function when pixel value is 0xFF,
	uqadd8	r5, r5, r11		;but we can omit this problem because of SAT operator.
	ENDIF
	uhadd8	r4, r8, r4
	uhadd8	r5, r9, r5
	
	uxtb16	r8, r4				;r8 = 0 s2 0 s0
	uxtb16	r9, r4, ror #8		;r9 = 0 s3 0 s1
	uxtb16	r10, r6				;r10 = 0 d2 0 d0
	uxtb16	r11, r6, ror #8		;r11 = 0 d3 0 d1
	ssub16	r10, r10, r8		;r10 = 0 c2 0 c0	;c = d - s
	ssub16	r11, r11, r9		;r11 = 0 c3 0 c1
	strd	r4, [r2], r3		;store src to dst	
	pkhbt	r8, r10, r11, lsl #16	;r8 = 0 c1 0 c0
	pkhtb	r9, r11, r10, asr #16	;r9 = 0 c3 0 c2
	strd	r8, [r1], #8	
	uxtb16	r8, r5				;r8 = 0 s2 0 s0
	uxtb16	r9, r5, ror #8		;r9 = 0 s3 0 s1
	uxtb16	r10, r7				;r10 = 0 d2 0 d0
	uxtb16	r11, r7, ror #8		;r11 = 0 d3 0 d1
	ssub16	r10, r10, r8		;r10 = 0 c2 0 c0	;c = d - s
	ssub16	r11, r11, r9		;r11 = 0 c3 0 c1
	pkhbt	r8, r10, r11, lsl #16	;r8 = 0 c1 0 c0
	pkhtb	r9, r11, r10, asr #16	;r9 = 0 c3 0 c2
	strd	r8, [r1], #8
	
	IF	$RND = 1
	bgt		SubBlockHor_loop_Rnd1_$Pos	
	ELSE
	bgt		SubBlockHor_loop_Rnd0_$Pos	
	ENDIF
	
	MEND	;M_SubBlockHor_core
	
	
;*****************************************************************************************	
	MACRO
	M_SubBlockVer_core	$Pos, $RND
;*****************************************************************************************	
;R0 Src
;R1 dct_data
;R2 Dst
;R3 SrcPitch
;R12 tmp
;lr loop

	;It can't define label with two paramters(such as M_SubBlockVer_core_$RND_$Pos) when builded with RVDS4,so define it as below.
	;But it can pass vs2005 compiler.
	IF	$RND = 1
M_SubBlockVer_Rnd1_core_$Pos
	ELSE
M_SubBlockVer_Rnd0_core_$Pos
	ENDIF
		
	IF	$Pos = 0
	ldr		r5, [r0, #4]		
	ldr		r4, [r0], r3		;r4 = s3 s2 s1 s0
	ELSE
	ldr		r4, [r0]			;r4 = s3 s2 s1 s0
	ldr		r5, [r0, #4]		
	ldr		r9, [r0, #8]		
	add		r0, r0, r3
	mov		r4, r4, lsr #$Pos
	orr		r4, r4, r5, lsl #32-$Pos
	mov		r5, r5, lsr #$Pos
	orr		r5, r5, r9, lsl #32-$Pos
	ENDIF
	
	IF	$RND = 1
SubBlockVer_loop_Rnd1_$Pos
	ELSE
SubBlockVer_loop_Rnd0_$Pos
	ENDIF
	
	IF	$Pos = 0
	
	ldr		r11, [r0, #4]		
	ldr		r10, [r0], r3		;r10 = s3 s2 s1 s0
	ldrd	r6, [r2]			;r6 = d3 d2 d1 d0
	IF	$RND = 1
	mov		r9, #0xff
	add		r9, r9, #2
	subs	lr, lr, #1	
	orr		r9, r9, r9, lsl #16		;0x01010101
	ELSE
	subs	lr, lr, #1	
	ENDIF
	
	ELSE
	
	ldr		r10, [r0]			;r10 = s3 s2 s1 s0
	ldr		r11, [r0, #4]		
	ldr		r8, [r0, #8]		
	ldrd	r6, [r2]			;r6 = d3 d2 d1 d0
    IF	$RND = 1
	mov		r9, #0xff
	add		r9, r9, #2
	subs	lr, lr, #1	
	orr		r9, r9, r9, lsl #16		;0x01010101
	ELSE
	subs	lr, lr, #1	
    ENDIF
	add		r0, r0, r3
	mov		r10, r10, lsr #$Pos
	orr		r10, r10, r11, lsl #32-$Pos
	mov		r11, r11, lsr #$Pos
	orr		r11, r11, r8, lsl #32-$Pos
	
	ENDIF
	
	pld		[r0]
	pld		[r2, r3]
    IF	$RND = 1
	uqadd8	r4, r4, r9		;here will cause not bit-complaint with C function when pixel value is 0xFF,
	uqadd8	r5, r5, r9		;but we can omit this problem because of SAT operator.
    ENDIF
	uhadd8	r4, r10, r4
	uhadd8	r5, r11, r5	
	
	strd	r4, [r2], r3		;store src to dst	
	uxtb16	r8, r4				;r8 = 0 s2 0 s0
	uxtb16	r9, r4, ror #8		;r9 = 0 s3 0 s1
	mov		r12, r5
	mov		r4, r10
	mov		r5, r11
	uxtb16	r10, r6				;r10 = 0 d2 0 d0
	uxtb16	r11, r6, ror #8		;r11 = 0 d3 0 d1
	ssub16	r10, r10, r8		;r10 = 0 c2 0 c0	;c = d - s
	ssub16	r11, r11, r9		;r11 = 0 c3 0 c1
	pkhbt	r8, r10, r11, lsl #16	;r8 = 0 c1 0 c0
	pkhtb	r9, r11, r10, asr #16	;r9 = 0 c3 0 c2
	strd	r8, [r1], #8	
	uxtb16	r8, r12				;r8 = 0 s2 0 s0
	uxtb16	r9, r12, ror #8		;r9 = 0 s3 0 s1
	uxtb16	r10, r7				;r10 = 0 d2 0 d0
	uxtb16	r11, r7, ror #8		;r11 = 0 d3 0 d1
	ssub16	r10, r10, r8		;r10 = 0 c2 0 c0	;c = d - s
	ssub16	r11, r11, r9		;r11 = 0 c3 0 c1
	pkhbt	r8, r10, r11, lsl #16	;r8 = 0 c1 0 c0
	pkhtb	r9, r11, r10, asr #16	;r9 = 0 c3 0 c2
	strd	r8, [r1], #8
	
	IF	$RND = 1
	bgt		SubBlockVer_loop_Rnd1_$Pos	
	ELSE
	bgt		SubBlockVer_loop_Rnd0_$Pos	
	ENDIF
	
	ldmia   sp!, {r4 - r11, pc} 
	
	MEND	;M_SubBlockVer_core
	
;*****************************************************************************************	

STACK_SIZE			EQU		20
STACK_OFFSET_r1		EQU		0
STACK_OFFSET_r2		EQU		4
STACK_OFFSET_r3		EQU		8
STACK_OFFSET_r0		EQU		12
STACK_OFFSET_lr		EQU		16

	
;*****************************************************************************************	
	MACRO
	M_SubBlockHorVer_core	$Pos, $RND
;*****************************************************************************************	
;R0 Src
;R1 dct_data
;R2 Dst
;R3 SrcPitch
;R12 tmp
;lr loop

; avg[p] = ((Src[p] + Src[p+1] + Src[p+SrcPitch] + Src[p+SrcPitch+1] + RNDx) >> 2  //if ($RND==1) RNDx=2 else RNDx=1
; dct_data[p] = Dst[p] - avg[p]
; Dst[p] = avg[p]

	IF	$RND = 1
M_SubBlockHorVer_Rnd2_core_$Pos
	ELSE
M_SubBlockHorVer_Rnd1_core_$Pos
	ENDIF

	sub		sp, sp, #STACK_SIZE
	IF	$Pos = 0
	ldr		r5, [r0, #4]		;b
	ldrb	r9, [r0, #8]		
	ldr		r4, [r0], r3		;a
	ELSE
	ldr		r4, [r0]			;r4 = s3 s2 s1 s0
	ldr		r5, [r0, #4]		
	ldr		r9, [r0, #8]		
	add		r0, r0, r3
	mov		r4, r4, lsr #$Pos
	orr		r4, r4, r5, lsl #32-$Pos
	mov		r5, r5, lsr #$Pos
	orr		r5, r5, r9, lsl #32-$Pos
	mov		r9, r9, lsr #$Pos
	ENDIF
	ldr		r10, =0x03030303
	pld		[r0, r3]
	pld		[r2]
	ldr		r11, =0x3F3F3F3F
	mov		r6, r4, lsr #8		
	mov		r7, r5, lsr #8		
	orr		r6, r6, r5, lsl #32-8	;c
	orr		r7, r7, r9, lsl #32-8	;d

;		q=(a & 0x03030303);		
;		w=(b & 0x03030303);		
;		q+=(c & 0x03030303);	
;		w+=(d & 0x03030303);	
	and		r8, r4, r10			;q
	and		r9, r5, r10			;w
	and		r12, r6, r10
	add		r8, r8, r12
	and		r12, r7, r10
	add		r9, r9, r12
;		a=(a>>2) & 0x3F3F3F3F;	
;		b=(b>>2) & 0x3F3F3F3F;	
;		a+=(c>>2) & 0x3F3F3F3F;	
;		b+=(d>>2) & 0x3F3F3F3F;	
;		c=q;					
;		d=w;						
	and		r4, r11, r4, lsr #2
	and		r5, r11, r5, lsr #2
	and		r6, r11, r6, lsr #2
	and		r7, r11, r7, lsr #2
	add		r4, r4, r6				;a
	add		r5, r5, r7				;b
	mov		r6, r8					;c
	mov		r7, r9					;d
	
	
	IF	$RND = 1
SubBlockHorVer_loop_Rnd2_$Pos
	ELSE
SubBlockHorVer_loop_Rnd1_$Pos
	ENDIF
	
	; pop in r1-r3
;	stm		sp, {r1-r3}
	str		r1, [sp, #STACK_OFFSET_r1]
	str		r2, [sp, #STACK_OFFSET_r2]
	str		r3, [sp, #STACK_OFFSET_r3]
	
	IF	$Pos = 0
	ldr		r9, [r0, #4]		;h
	ldrb	r12, [r0, #8]		
	ldr		r8, [r0], r3		;g
	subs	lr, lr, #1	
	ELSE
	ldr		r8, [r0]			;r8 = s3 s2 s1 s0
	ldr		r9, [r0, #4]		
	ldr		r12, [r0, #8]		
	subs	lr, lr, #1	
	add		r0, r0, r3
	mov		r8, r8, lsr #$Pos
	orr		r8, r8, r9, lsl #32-$Pos
	mov		r9, r9, lsr #$Pos
	orr		r9, r9, r12, lsl #32-$Pos
	mov		r12, r12, lsr #$Pos
	ENDIF
	
	; pop in r0,lr
	add		r2, sp, #12
	pld		[r0, r3]
;	stm		r2, {r0, lr}
	str		r0, [sp, #STACK_OFFSET_r0]
	str		lr, [sp, #STACK_OFFSET_lr]

	ldr		r1, =0x3F3F3F3F
	ldr		r0, =0x03030303
		
	mov		r10, r8, lsr #8		
	mov		r11, r9, lsr #8		
	orr		r10, r10, r9, lsl #32-8		;i
	orr		r11, r11, r12, lsl #32-8	;j

;		q=(g & 0x03030303);						
;		w=(h & 0x03030303);						
;		q+=(i & 0x03030303);					
;		w+=(j & 0x03030303);					
	and		r2, r8, r0			;q
	and		r3, r9, r0			;w
	and		r12, r10, r0
	add		r2, r2, r12
	and		r12, r11, r0
	add		r3, r3, r12
;		g=(g>>2) & 0x3F3F3F3F;					
;		h=(h>>2) & 0x3F3F3F3F;					
;		g+=(i>>2) & 0x3F3F3F3F;					
;		h+=(j>>2) & 0x3F3F3F3F;					
;		i=q;									
;		j=w;									
	and		r8, r1, r8, lsr #2
	and		r9, r1, r9, lsr #2
	and		r10, r1, r10, lsr #2
	and		r11, r1, r11, lsr #2
	add		r8, r8, r10				;g
	add		r9, r9, r11				;h
;	mov		r10, r2					;i
;	mov		r11, r3					;j

	IF	$RND = 1
	ldr		r1, =0x02020202
	ELSE
	ldr		r1, =0x01010101
	ENDIF
;		a+=g;						
;		b+=h;						
;		c+=i+0x02020202;		or c+=i+0x01010101;	
;		d+=j+0x02020202;		or d+=j+0x01010101			
;		a+=(c>>2) & 0x03030303;		
;		b+=(d>>2) & 0x03030303;		
	add		r4, r4, r8
	add		r5, r5, r9
	add		r6, r6, r2
	add		r7, r7, r3
	add		r6, r6, r1
	add		r7, r7, r1
	and		r6, r0, r6, lsr #2
	and		r7, r0, r7, lsr #2
	add		r12, r4, r6			;a
	add		lr, r5, r7			;b
	
;		a=g;
;		b=h;
;		c=i;
;		d=j;
	mov		r6, r2
	ldr		r2, [sp, #STACK_OFFSET_r2]	;	 pop out r2
	mov		r7, r3
	mov		r4, r8
	mov		r5, r9
	
	ldr		r0, [r2]					;r0 = d3 d2 d1 d0
	ldr		r1, [sp, #STACK_OFFSET_r1]	;	 pop out r1
	uxtb16	r8, r12					;r8 = 0 s2 0 s0
	uxtb16	r9, r12, ror #8			;r9 = 0 s3 0 s1
	uxtb16	r10, r0					;r10 = 0 d2 0 d0
	uxtb16	r11, r0, ror #8			;r11 = 0 d3 0 d1
	ssub16	r10, r10, r8			;r10 = 0 c2 0 c0	;c = d - s
	ssub16	r11, r11, r9			;r11 = 0 c3 0 c1
	ldr		r0, [r2, #4]			;r0 = d3 d2 d1 d0
	pkhbt	r8, r10, r11, lsl #16	;r8 = 0 c1 0 c0
	pkhtb	r9, r11, r10, asr #16	;r9 = 0 c3 0 c2
	strd	r8, [r1], #8	
	uxtb16	r8, lr					;r8 = 0 s2 0 s0
	uxtb16	r9, lr, ror #8			;r9 = 0 s3 0 s1
	uxtb16	r10, r0					;r10 = 0 d2 0 d0
	uxtb16	r11, r0, ror #8			;r11 = 0 d3 0 d1
	ssub16	r10, r10, r8			;r10 = 0 c2 0 c0	;c = d - s
	ssub16	r11, r11, r9			;r11 = 0 c3 0 c1
	ldr		r3, [sp, #STACK_OFFSET_r3]	;	 pop out r3
	ldr		r0, [sp, #STACK_OFFSET_r0]	;	 pop out r0
	pkhbt	r8, r10, r11, lsl #16		;r8 = 0 c1 0 c0
	pkhtb	r9, r11, r10, asr #16		;r9 = 0 c3 0 c2
	strd	r8, [r1], #8
	
	str		lr, [r2, #4]				;store src to dst	
	str		r12, [r2], r3				
	ldr		lr, [sp, #STACK_OFFSET_lr]	;	 pop out lr
	pld		[r2]
	
	IF	$RND = 1
	bgt		SubBlockHorVer_loop_Rnd2_$Pos	
	ELSE
	bgt		SubBlockHorVer_loop_Rnd1_$Pos	
	ENDIF
	
	
	add		sp, sp, #STACK_SIZE
	ldmia   sp!, {r4 - r11, pc} 
	
	MEND	;M_SubBlockHorVer_core

;*****************************************************************************************	
;* VO_VOID SubBlockHorRound(uint8_t *Src, int16_t* dct_data, uint8_t *Dst, const int32_t SrcPitch)
;*****************************************************************************************	
	
SubBlockHorRound_ARMV6
	stmdb   sp!, {r4 - r11, lr}
	
	pld		[r0]
	pld		[r2]
	mov		lr, #8
	ands	r12, r0, #3
	bic		r0, r0, #3
	mov		r12, r12, lsl #3
	bne		SubBlockHorRound_Lab123 	
SubBlockHorRound_Lab0
	M_SubBlockHor_core	0, 0
	ldmia   sp!, {r4 - r11, pc} 
SubBlockHorRound_Lab123	
	M_SubBlockHor_core	123, 0
	ldmia   sp!, {r4 - r11, pc} 


;*****************************************************************************************	
;* VO_VOID SubBlockVerRound(uint8_t *Src, int16_t* dct_data, uint8_t *Dst, const int32_t SrcPitch)
;*****************************************************************************************	
	
SubBlockVerRound_ARMV6
	stmdb   sp!, {r4 - r11, lr}
	
	pld		[r0]
	pld		[r2]
	and		r12, r0, #3
	mov		lr, #8
	bic		r0, r0, #3
	ldr		pc, [pc, r12, lsl #2]
	nop
	DCD		M_SubBlockVer_Rnd0_core_0
	DCD		M_SubBlockVer_Rnd0_core_8
	DCD		M_SubBlockVer_Rnd0_core_16
	DCD		M_SubBlockVer_Rnd0_core_24

	M_SubBlockVer_core 0, 0 
	M_SubBlockVer_core 8, 0
	M_SubBlockVer_core 16, 0
	M_SubBlockVer_core 24, 0
	

			
;*****************************************************************************************	
;* VO_VOID SubBlock(uint8_t *Src, int16_t* dct_data, uint8_t *Dst, const int32_t SrcPitch)
;*****************************************************************************************	
	
SubBlock_ARMV6
	stmdb   sp!, {r4 - r11, lr}
	
	pld		[r0]
	pld		[r2]
	mov		lr, #8
	ands	r12, r0, #3
	bic		r0, r0, #3
	mov		r12, r12, lsl #3
	bne		SubBlock_Lab123 	
SubBlock_Lab0
	M_SubBlock_core	0
	ldmia   sp!, {r4 - r11, pc} 
SubBlock_Lab123	
	M_SubBlock_core	123
	ldmia   sp!, {r4 - r11, pc} 
	

;*****************************************************************************************	
;* VO_VOID SubBlockHor(uint8_t *Src, int16_t* dct_data, uint8_t *Dst, const int32_t SrcPitch)
;*****************************************************************************************	
	
SubBlockHor_ARMV6
	stmdb   sp!, {r4 - r11, lr}
	
	pld		[r0]
	pld		[r2]
	mov		lr, #8
	ands	r12, r0, #3
	bic		r0, r0, #3
	mov		r12, r12, lsl #3
	bne		SubBlockHor_Lab123 	
SubBlockHor_Lab0
	M_SubBlockHor_core	0, 1
	ldmia   sp!, {r4 - r11, pc} 
SubBlockHor_Lab123	
	M_SubBlockHor_core	123, 1
	ldmia   sp!, {r4 - r11, pc} 
	

;*****************************************************************************************	
;* VO_VOID SubBlockVer(uint8_t *Src, int16_t* dct_data, uint8_t *Dst, const int32_t SrcPitch)
;*****************************************************************************************	
	
SubBlockVer_ARMV6
	stmdb   sp!, {r4 - r11, lr}
	
	pld		[r0]
	pld		[r2]
	and		r12, r0, #3
	mov		lr, #8
	bic		r0, r0, #3
	ldr		pc, [pc, r12, lsl #2]
	nop
	DCD		M_SubBlockVer_Rnd1_core_0
	DCD		M_SubBlockVer_Rnd1_core_8
	DCD		M_SubBlockVer_Rnd1_core_16
	DCD		M_SubBlockVer_Rnd1_core_24

	M_SubBlockVer_core 0, 1 
	M_SubBlockVer_core 8, 1
	M_SubBlockVer_core 16, 1
	M_SubBlockVer_core 24, 1
	

;*****************************************************************************************	
;* VO_VOID SubBlockHorVer(uint8_t *Src, int16_t* dct_data, uint8_t *Dst, const int32_t SrcPitch)
;*****************************************************************************************	
	
SubBlockHorVer_ARMV6
	stmdb   sp!, {r4 - r11, lr}
	
	pld		[r0]
	pld		[r2]
	and		r12, r0, #3
	mov		lr, #8
	bic		r0, r0, #3
	ldr		pc, [pc, r12, lsl #2]
	nop
	DCD		M_SubBlockHorVer_Rnd2_core_0
	DCD		M_SubBlockHorVer_Rnd2_core_8
	DCD		M_SubBlockHorVer_Rnd2_core_16
	DCD		M_SubBlockHorVer_Rnd2_core_24

	M_SubBlockHorVer_core 0, 1 
	M_SubBlockHorVer_core 8, 1
	M_SubBlockHorVer_core 16, 1
	M_SubBlockHorVer_core 24, 1
			

;*****************************************************************************************	
;* VO_VOID SubBlockHorVerRound(uint8_t *Src, int16_t* dct_data, uint8_t *Dst, const int32_t SrcPitch)
;*****************************************************************************************	
	
SubBlockHorVerRound_ARMV6
	stmdb   sp!, {r4 - r11, lr}
	
	pld		[r0]
	pld		[r2]
	and		r12, r0, #3
	mov		lr, #8
	bic		r0, r0, #3
	ldr		pc, [pc, r12, lsl #2]
	nop
	DCD		M_SubBlockHorVer_Rnd1_core_0
	DCD		M_SubBlockHorVer_Rnd1_core_8
	DCD		M_SubBlockHorVer_Rnd1_core_16
	DCD		M_SubBlockHorVer_Rnd1_core_24

	M_SubBlockHorVer_core 0, 0 
	M_SubBlockHorVer_core 8, 0
	M_SubBlockHorVer_core 16, 0
	M_SubBlockHorVer_core 24, 0
	
	END	