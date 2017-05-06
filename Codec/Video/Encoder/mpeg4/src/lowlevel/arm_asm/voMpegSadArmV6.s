;*****************************************************************************
;*																			*
;*		VisualOn, Inc. Confidential and Proprietary, 2003					*
;*																			*
;*****************************************************************************

;R0 src
;R1 dst
;R2 srcpitch
;R3 dstpitch

	EXPORT 	Dev16x16_ARMV6
	EXPORT 	Sad16x16_ARMV6
	EXPORT 	Sad8x8_ARMV6

	AREA	|.text|, CODE
		
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;unsigned int
;Dev16Armv6(const unsigned char * const cur,
;		       const unsigned int stride)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	
;r0:cur
;r1:stride		

Dev16x16_ARMV6

	stmdb	sp!, {r4 - r11, lr}

	pld		[r0]
	mov		r12, #0
	mov		r10, #0
	mov		r14, r0
	mov		r11, #8     ;loop num
	pld		[r14, r1]
ACC_MB
	ldrd	r2, [r14]
	ldrd	r4, [r14, #8]
	pld		[r14, r1, lsl #1]	
	add		r14, r14, r1
	usada8	r2, r2, r12, r10
	usad8	r3, r3, r12
	ldrd	r6, [r14]	
	ldrd	r8, [r14, #8]	
	usada8	r4, r4, r12, r2
	usada8	r5, r5, r12, r3
	pld		[r14, r1, lsl #1]	
	usada8	r6, r6, r12, r4
	usada8	r7, r7, r12, r5
	subs	r11, r11, #1
	usada8	r8, r8, r12, r6
	usada8	r9, r9, r12, r7
	add		r14, r14, r1	
	add		r10, r8, r9 	 ;acc value
	bne		ACC_MB
	
	mov		r12, r10, lsr #8  ;average value
	mov		r14, r0
	mov		r11, #8
	orr		r12, r12, r12, lsl #8
	mov		r10, #0
	orr		r12, r12, r12, lsl #16 ;pack as |a|a|a|a|
ACC_MB_ABS_DIFF 
	ldrd	r2, [r14]
	ldrd	r4, [r14, #8]
	add		r14, r14, r1
	usada8	r2, r2, r12, r10
	usad8	r3, r3, r12
	ldrd	r6, [r14]	
	ldrd	r8, [r14, #8]	
	usada8	r4, r4, r12, r2
	usada8	r5, r5, r12, r3
	subs	r11, r11, #1
	usada8	r6, r6, r12, r4
	usada8	r7, r7, r12, r5
	add		r14, r14, r1	
	usada8	r8, r8, r12, r6
	usada8	r9, r9, r12, r7
	add		r10, r8, r9 	
	bne		ACC_MB_ABS_DIFF	
	
	mov		r0, r10		;return value
	ldmia	sp!, {r4 - r11, pc}


	MACRO
	M_Sad16ArmV6	$Pos

;R0 cur
;R1 ref
;R2 loop
;R3 ref_stride
;lr sad
	
M_Sad16ArmV6_$Pos
	
	pld		[r1, r3]
	mov		lr, #16
	mov		r2, #0
Sad16ArmV6_$Pos.height

	ldr		r4, [r1]
	ldr		r5, [r1, #4]
	ldr		r6, [r1, #8] 
	ldrd	r8, [r0], #8
	ldr		r7, [r1, #12]
	ldrd	r10, [r0], #8
  IF $Pos > 0
	ldr		r12, [r1, #16]
	mov		r4, r4, lsr #$Pos
	orr		r4, r4, r5, lsl #32-$Pos
	mov		r5, r5, lsr #$Pos
	orr		r5, r5, r6, lsl #32-$Pos
	mov		r6, r6, lsr #$Pos
	orr		r6, r6, r7, lsl #32-$Pos
	mov		r7, r7, lsr #$Pos
	orr		r7, r7, r12, lsl #32-$Pos
  ENDIF
	pld		[r1,r3,lsl #1]
	add		r1, r1, r3

	ldr		r12, [sp, #36]	; r12 = best_sad
	usad8	r8, r8, r4
	usad8	r9, r9, r5
	usad8	r10, r10, r6
	usad8	r11, r11, r7
	add		r8, r8, r9
	add		r8, r8, r10
	add		r8, r8, r11
	add		r2, r2, r8
	cmp		r2, r12
	bge		Sad16ArmV6_$Pos.exit

	subs	lr, lr, #1
	bne		Sad16ArmV6_$Pos.height
Sad16ArmV6_$Pos.exit	
	mov		r0, r2
	ldmia	sp!, {r4 - r11, pc}

	MEND

Sad16x16_ARMV6	PROC
	stmdb   sp!, {r4 - r11, lr}
	pld		[r1]
	mov		lr, #8
	and		r12, r1, #3
	bic		r1, r1, #3
	ldr		pc, [pc, r12, lsl #2]
	nop
	DCD		M_Sad16ArmV6_0
	DCD		M_Sad16ArmV6_8
	DCD		M_Sad16ArmV6_16
	DCD		M_Sad16ArmV6_24

	ENDP

	M_Sad16ArmV6 0 
	M_Sad16ArmV6 8
	M_Sad16ArmV6 16
	M_Sad16ArmV6 24


	MACRO
	M_Sad8ArmV6	$Pos

M_Sad8ArmV6_$Pos
	pld		[r1,r3]
	mov		r12, #8
	mov		lr, #0
Sad8ArmV6_$Pos.height
	ldr		r4, [r1]
	ldr		r5, [r1, #4]
  if $Pos > 0
	ldr		r6, [r1, #8]
	ldrd	r8, [r0], r2
	pld		[r1,r3,lsl #1]
	mov		r4, r4, lsr #$Pos
	orr		r4, r4, r5, lsl #32-$Pos
	mov		r5, r5, lsr #$Pos
	orr		r5, r5, r6, lsl #32-$Pos
  else
	ldrd	r8, [r0], r2
	pld		[r1,r3,lsl #1]
  endif
	subs	r12, r12, #1	
	add		r1, r1, r3
	usad8	r8, r8, r4
	usad8	r9, r9, r5
	add		lr, lr, r8
	add		lr, lr, r9 
	bne		Sad8ArmV6_$Pos.height
	mov		r0, lr
	ldmia	sp!, {r4 - r9, pc}	
	
	MEND


Sad8x8_ARMV6	PROC
	stmdb   sp!, {r4 - r9, lr}
	
	pld		[r1]
	mov		lr, #8
	and		r12, r1, #3
	bic		r1, r1, #3
	ldr		pc, [pc, r12, lsl #2]
	nop
	DCD		M_Sad8ArmV6_0
	DCD		M_Sad8ArmV6_8
	DCD		M_Sad8ArmV6_16
	DCD		M_Sad8ArmV6_24

	ENDP

	M_Sad8ArmV6 0 
	M_Sad8ArmV6 8
	M_Sad8ArmV6 16
	M_Sad8ArmV6 24

	END