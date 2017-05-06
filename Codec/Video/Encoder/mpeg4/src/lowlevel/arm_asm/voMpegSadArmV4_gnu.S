@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2003					*
@*																			*
@*****************************************************************************

@R0 src
@R1 dst
@R2 srcpitch
@R3 dstpitch


	.section .text


  .macro  PreLoad ARM5,Pos
    .if \ARM5>0
	  .if \Pos >= 0
  	    .if \Pos > 0
	      pld [r0,r2,lsl #1]
	      add r0,r0,#8
	      pld [r0,r2,lsl #1]
	      sub r0,r0,#8
	    .else
	      pld [r0,r2,lsl #1]
	      add r0,r0,#4
	      pld [r0,r2,lsl #1]
	      sub r0,r0,#4
	    .endif
	  .else
	    pld [r0,r2,lsl #1]
	    add r0,r0,#7
	    pld [r0,r2,lsl #1]
	    sub r0,r0,#7
	  .endif
	.endif
  .endm

  .macro   PreLoad2Init ARM5,Pos
    .if \ARM5>0
	  .if \Pos >= 0
  	    .if \Pos > 0
	      add r10,r2,r2
	      add r10,r10,#8
	    .else
	      add r10,r2,r2
	      add r10,r10,#4
	    .endif
	  .else
	      add r10,r2,r2
	      add r10,r10,#7
	  .endif
	.endif
  .endm

  .macro  PreLoad2 ARM5,Pos
	.if \ARM5>0
      pld [r0,r2,lsl #1]
      pld [r0,r10]
	.endif
  .endm



@uint32_t sad16_c(const uint8_t * const cur,
@		const uint8_t * const ref,
@		const uint32_t cur_stride,
@		const uint32_t ref_stride,
@		const uint32_t best_sad)
@r0 = cur
@r1 = ref
@r2 = cur_stride
@r3 = ref_stride

  .macro  CopyBuild Name,ARM5 

	.align 4
	.global \Name
\Name: 
	.if \ARM5>0
	pld [r1,r3]
	pld [r0]
	.endif
	stmdb	sp!, {r4 - r12, lr}
@	.if \ARM5>0
@	.endif
	ldr			r4, [sp, #40]	@ r4 = best_sad
	mov			r2, #16
	mov			lr, #0
height\Name:
	.if \ARM5>0
	pld			[r1, r3]
	.endif
	
	ldrb		r5, [r0], #1
	ldrb		r6, [r0], #1
	ldrb		r7, [r0], #1
	ldrb		r8, [r0], #1
	ldrb		r9, [r1], #1
	ldrb		r10, [r1], #1
	ldrb		r11, [r1], #1
	ldrb		r12, [r1], #1
	
	@4
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

	@8
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

	@12
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

	@16
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
	bge			exit\Name
	sub			r1, r1, #16
	add			r1, r1, r3 	

	subs		r2, r2, #1
	bne			height\Name
exit\Name:
	mov			r0, lr

    ldmia		sp!, {r4-r12, pc}		@ restore and return  
		
	
	@endp
  .endm


	.align 4
@	.global sad16v2_no4mv_arm

sad16v2_no4mv_arm:


	stmdb		sp!, {r4-r12, lr}		@ save regs used
	ldr			r4, [sp, #44]			@ r4 = cur_mb

	mov			r11, #16
	mov			lr, #0
sad16v2_no4mv_arm_height:

	ldr			r5, [r0]				@ r5 = cur 4 pixels
	ldrb		r6, [r1], #1			@ 0
	ldrb		r7, [r1], #1			@ 1
	ldrb		r8, [r1], #1			@ 2
	ldrb		r9, [r1], #1			@ 3
	ldr			r10, [r0, #4]			
	
	@ 0
	and			r12, r5, #0xff
	subs		r12, r12, r6	
	ldrb		r6, [r1], #1	
	rsbmi 		r12, r12, #0
	add			lr, lr, r12

	and			r12, r5, #0xff00
	mov			r12, r12, lsr #8

	subs		r12, r12, r7
	ldrb		r7, [r1], #1
	rsbmi		r12, r12, #0
	add			lr, lr, r12

	and			r12, r5, #0xff0000
	mov			r12, r12, lsr #16

	subs		r12, r12, r8
	ldrb		r8, [r1], #1
	rsbmi		r12, r12, #0
	add			lr, lr, r12

	mov			r12, r5, lsr #24
	
	subs		r12, r12, r9
	ldrb		r9, [r1], #1
	rsbmi		r12, r12, #0
	add			lr, lr, r12
	
	str			r5, [r4]
	ldr			r5, [r0, #8]

	@ 1
	and			r12, r10, #0xff
	subs		r12, r12, r6	
	ldrb		r6, [r1], #1	
	rsbmi 		r12, r12, #0
	add			lr, lr, r12

	and			r12, r10, #0xff00
	mov			r12, r12, lsr #8

	subs		r12, r12, r7
	ldrb		r7, [r1], #1
	rsbmi		r12, r12, #0
	add			lr, lr, r12

	and			r12, r10, #0xff0000
	mov			r12, r12, lsr #16

	subs		r12, r12, r8
	ldrb		r8, [r1], #1
	rsbmi		r12, r12, #0
	add			lr, lr, r12

	mov			r12, r10, lsr #24
	
	subs		r12, r12, r9
	ldrb		r9, [r1], #1
	rsbmi		r12, r12, #0
	add			lr, lr, r12
	
	str			r10, [r4, #4]
	ldr			r10, [r0, #12]

	@ 2
	and			r12, r5, #0xff
	subs		r12, r12, r6	
	ldrb		r6, [r1], #1	
	rsbmi 		r12, r12, #0
	add			lr, lr, r12

	and			r12, r5, #0xff00
	mov			r12, r12, lsr #8

	subs		r12, r12, r7
	ldrb		r7, [r1], #1
	rsbmi		r12, r12, #0
	add			lr, lr, r12

	and			r12, r5, #0xff0000
	mov			r12, r12, lsr #16

	subs		r12, r12, r8
	ldrb		r8, [r1], #1
	rsbmi		r12, r12, #0
	add			lr, lr, r12

	mov			r12, r5, lsr #24
	
	subs		r12, r12, r9
	ldrb		r9, [r1], #1
	rsbmi		r12, r12, #0
	add			lr, lr, r12
	
	str			r5, [r4, #8]

	@ 3
	and			r12, r10, #0xff
	subs		r12, r12, r6	
	rsbmi 		r12, r12, #0
	add			lr, lr, r12

	and			r12, r10, #0xff00
	mov			r12, r12, lsr #8

	subs		r12, r12, r7
	rsbmi		r12, r12, #0
	add			lr, lr, r12

	and			r12, r10, #0xff0000
	mov			r12, r12, lsr #16

	subs		r12, r12, r8
	rsbmi		r12, r12, #0
	add			lr, lr, r12

	mov			r12, r10, lsr #24
	
	subs		r12, r12, r9
	rsbmi		r12, r12, #0
	add			lr, lr, r12
	
	str			r10, [r4, #12]

	subs		r11, r11, #1
	addne		r4, r4, #16				@ r4 = cur_mb out
	addne		r0, r0, r2
	subne		r1, r1, #16
	addne		r1, r1, r3
	
	bne			sad16v2_no4mv_arm_height
	mov			r0, lr

    ldmia		sp!, {r4-r12, pc}		@ restore and return  

	@endp



	.align 4
	.global Sad8x8_ARMV4

Sad8x8_ARMV4 :

	stmdb		sp!, {r4-r12, lr}		@ save regs used
	ldrb		r6, [r0]		@ load 1st 4 source pel
	mov			r8, #0
	ldrb		r7, [r1]		@ load 1st 4 reference pel
	mov			r4, #8
Sad8Armv4_height:
@	pld			[r0, r2]
@	pld			[r1, r3]
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
@	ldrb		lr, [r0, r2, lsl #1]		@prefetch
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
@	ldrb		r12, [r1, r3, lsl #1]		@prefetch
	subs		r5,	r9, r10
	ldrb		r9, [r0, #7]
	rsbmi 		r5, r5, #0
	ldrb		r10, [r1, #7]
	add			r8, r8, r5
	subs		r5, r6, r7
	add			r0, r0, r2
	ldrb		r6, [r0]		@ load 1st 4 source pel
	add			r1, r1, r3
	ldrb		r7, [r1]		@ load 1st 4 reference pel
	rsbmi 		r5, r5, #0

	add			r8, r8, r5

	subs		r5,	r9, r10
	rsbmi 		r5, r5, #0
	subs		r4, r4, #1
	add			r8, r8, r5
	bne			Sad8Armv4_height
	mov			r0, r8

    ldmia		sp!, {r4-r12, pc}		@ restore and return   

	@endp

	
	CopyBuild Sad16x16_ARMV5, 0
	.END