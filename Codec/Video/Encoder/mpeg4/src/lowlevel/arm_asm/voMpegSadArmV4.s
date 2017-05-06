;*****************************************************************************
;*																			*
;*		VisualOn, Inc. Confidential and Proprietary, 2003					*
;*																			*
;*****************************************************************************

;R0 src_ptr
;R1 dst_ptr
;R2 src stride
;R3 dst stride

	AREA	|.text|, CODE 


  macro 
  CopyBuild $Name,$ARM5 

	align 16
	export $Name
$Name proc
	if $ARM5>0
	pld [r1,r3]
	pld [r0]
	endif
	stmdb	sp!, {r4 - r12, lr}
	ldr			r4, [sp, #40]	; r4 = best_sad
	mov			r2, #16
	mov			lr, #0
$Name.height
	if $ARM5>0
	pld			[r1, r3]
	endif
	
	ldrb		r5, [r0], #1
	ldrb		r6, [r0], #1
	ldrb		r7, [r0], #1
	ldrb		r8, [r0], #1
	ldrb		r9, [r1], #1
	ldrb		r10, [r1], #1
	ldrb		r11, [r1], #1
	ldrb		r12, [r1], #1
	
	;4
	subs		r5, r5, r9
	ldrb		r9, [r1], #1
	rsbmi		r5, r5, #0
	add			lr, lr, r5
	ldrb		r5, [r0], #1

	subs		r6, r6, r10
	ldrb		r10, [r1], #1
	rsbmi		r6, r6, #0
	add			lr, lr, r6
	ldrb		r6, [r0], #1

	subs		r7, r7, r11
	ldrb		r11, [r1], #1
	rsbmi		r7, r7, #0
	add			lr, lr, r7
	ldrb		r7, [r0], #1

	subs		r8, r8, r12
	ldrb		r12, [r1], #1
	rsbmi		r8, r8, #0
	add			lr, lr, r8
	ldrb		r8, [r0], #1

	;8
	subs		r5, r5, r9
	ldrb		r9, [r1], #1
	rsbmi		r5, r5, #0
	add			lr, lr, r5
	ldrb		r5, [r0], #1

	subs		r6, r6, r10
	ldrb		r10, [r1], #1
	rsbmi		r6, r6, #0
	add			lr, lr, r6
	ldrb		r6, [r0], #1

	subs		r7, r7, r11
	ldrb		r11, [r1], #1
	rsbmi		r7, r7, #0
	add			lr, lr, r7
	ldrb		r7, [r0], #1

	subs		r8, r8, r12
	ldrb		r12, [r1], #1
	rsbmi		r8, r8, #0
	add			lr, lr, r8
	ldrb		r8, [r0], #1

	;12
	subs		r5, r5, r9
	ldrb		r9, [r1], #1
	rsbmi		r5, r5, #0
	add			lr, lr, r5
	ldrb		r5, [r0], #1

	subs		r6, r6, r10
	ldrb		r10, [r1], #1
	rsbmi		r6, r6, #0
	add			lr, lr, r6
	ldrb		r6, [r0], #1

	subs		r7, r7, r11
	ldrb		r11, [r1], #1
	rsbmi		r7, r7, #0
	add			lr, lr, r7
	ldrb		r7, [r0], #1

	subs		r8, r8, r12
	ldrb		r12, [r1], #1
	rsbmi		r8, r8, #0
	add			lr, lr, r8
	ldrb		r8, [r0], #1

	;16
	subs		r5, r5, r9
	rsbmi		r5, r5, #0
	add			lr, lr, r5

	subs		r6, r6, r10
	rsbmi		r6, r6, #0
	add			lr, lr, r6

	subs		r7, r7, r11
	rsbmi		r7, r7, #0
	add			lr, lr, r7

	subs		r8, r8, r12
	rsbmi		r8, r8, #0
	add			lr, lr, r8	
	cmp			lr, r4
	bge			$Name.exit
	sub			r1, r1, #16
	add			r1, r1, r3 	

	subs		r2, r2, #1
	bne			$Name.height
$Name.exit
	mov			r0, lr

    ldmia		sp!, {r4-r12, pc}		; restore and return  
		
	
	endp
  mend


	align 16
	export Sad8x8_ARMV4

Sad8x8_ARMV4 proc

	stmdb		sp!, {r4-r12, lr}		; save regs used
	ldrb		r6, [r0]		; load 1st 4 source pel
	mov			r8, #0
	ldrb		r7, [r1]		; load 1st 4 reference pel
	mov			r4, #8
Sad8Armv4_height
;	pld			[r0, r2]
;	pld			[r1, r3]
	ldrb		r9, [r0, #1]
	subs		r5, r6, r7
	ldrb		r10, [r1, #1]
	rsbmi 		r5, r5, #0
	ldrb		r6, [r0, #2]
	add			r8, r8, r5	
	ldrb		r7, [r1, #2]	
	subs		r5, r9, r10
	ldrb		r9, [r0, #3]
	rsbmi 		r5, r5, #0
	ldrb		r10, [r1, #3]
	add			r8, r8, r5
	subs		r5,	r6, r7
	ldrb		r6, [r0, #4]
	rsbmi 		r5, r5, #0
	ldrb		r7, [r1, #4]
	add			r8, r8, r5
;	ldrb		lr, [r0, r2, lsl #1]		@prefetch
	subs		r5,	r9, r10
	ldrb		r9, [r0, #5]
	rsbmi 		r5, r5, #0
 	ldrb		r10, [r1, #5]	  
	add			r8, r8, r5
	subs		r5,	r6, r7
	ldrb		r6, [r0, #6]
	rsbmi 		r5, r5, #0
	ldrb		r7, [r1, #6]
	add			r8, r8, r5
;	ldrb		r12, [r1, r3, lsl #1]		@prefetch
	subs		r5,	r9, r10
	ldrb		r9, [r0, #7]
	rsbmi 		r5, r5, #0
	ldrb		r10, [r1, #7]
	add			r8, r8, r5
	subs		r5, r6, r7
	add			r0, r0, r2
	ldrb		r6, [r0]		; load 1st 4 source pel
	add			r1, r1, r3
	ldrb		r7, [r1]		; load 1st 4 reference pel
	rsbmi 		r5, r5, #0

	add			r8, r8, r5

	subs		r5,	r9, r10
	rsbmi 		r5, r5, #0
	subs		r4, r4, #1
	add			r8, r8, r5
	bne			Sad8Armv4_height
	mov			r0, r8

    ldmia		sp!, {r4-r12, pc}		; restore and return   

	endp

	CopyBuild Sad16x16_ARMV5, 1
	END