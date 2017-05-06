	.section .text
	@.extern	AAD_srtdata66
	@.extern	AAD_srtdata20
	@.extern	AAD_srtidata81
	
	.global	AAD_srtidata516

	@AREA	.text, CODE, READONLY

AAD_srtidata516: @ PROC
	stmdb     sp!, {r4 - r11, lr}
	sub		sp,sp,#8
	str		r2,[sp,#0]				@:&nmltTab[0]
	str		r3,[sp,#4]				@:&cos4sin4tab[0]
M2882:
	mov       r2, r0
	mov       lr, r1
	@ldr       r0, [pc, #0x8C]			@:nmltTab
	ldr		  r0,[sp,#0]
	ldr       r3, [r0, +r2, lsl #2]		@:r3:nmlt
	@ldr       r0, [pc, #0x80]			@con4sin4tabOffset
	ldr		  r0,[sp,#44]
	add       r1, lr, r3, lsl #2		
	sub       r5, r1, #8
	ldr       r2, [r0, +r2, lsl #2]
	@ldr       r1, [pc, #0x6C]			@cos4sin4tab
	ldr			r1,[sp,#4]

	movs      r12, r3, asr #2
	add       r4, r1, r2, lsl #2
	beq       L2236
L2881:
	ldrd      r0, [r4, #0]
	ldrd	  r2, [lr, #0]
	ldrd	  r6, [r5, #0]	
	add		  r8, r2, r7
	smmul	  r11, r1, r8
	sub		  r10, r0, r1, lsl #1
	smmls	  r9, r0, r7, r11
	smmla	  r8, r10, r2, r11
	ldrd	  r0, [r4, #8]
	rsb		  r9, r9, #0	
	add		  r10, r6, r3
	strd	  r8, [lr, #0] 		
	smmul 	  r11, r1, r10
	sub		  r2, r0, r1, lsl #1
	smmls     r9, r0, r3, r11
	add		  lr, lr, #8	
	smmla	  r8, r2, r6, r11
	rsb		  r9, r9, #0
	add		  r4, r4, #16	
	strd	  r8, [r5, #0]
	sub       r12, r12, #1
	sub		  r5, r5, #8	
	cmp       r12, #0
	bhi       L2881
L2236:
	add		  sp,sp,#8
	ldmia     sp!, {r4 - r11, pc}
@L2885:
@	.word       cook_cos4sin4tab
@	.word       cook_cos4sin4tabOffset
@	.word       cook_nmltTab
@M2883:

	@ENDP  @ AAD_srtidata516

	.global	AAD_srtidata517

	@AREA	.text, CODE, READONLY

AAD_srtidata517: @ PROC
	stmdb     sp!, {r4 - r11, lr}
	sub       sp, sp, #8 
M3882:
	str       r2, [sp, #0]
	str		  r3, [sp,#4]
	mov       lr, r1
	mov       r2, r0
	@ldr       r0, [pc, #0xA4]
	ldr			r0,[sp,#4]
	ldr       r3, [r0, +r2, lsl #2]
	@ldr       r0, [pc, #0x98]
	ldr			r0,[sp,#48]
	add       r1, lr, r3, lsl #2
	sub       r5, r1, #8
	ldr       r2, [r0, +r2, lsl #2]
	@ldr       r1, [pc, #0x84]
	ldr			r1,[sp,#44]
	
	movs      r12, r3, asr #2
	add       r4, r1, r2, lsl #2
	beq       L3236
L3881:
	ldr       r8, [sp, #0]
	ldrd      r0, [r4, #0]
	ldrd	  r2, [lr, #0]
	ldrd	  r6, [r5, #0]	
	mov       r2, r2, asr r8
	mov       r3, r3, asr r8
	mov       r6, r6, asr r8
	mov       r7, r7, asr r8
	add		  r8, r2, r7
	smmul	  r11, r1, r8
	sub		  r10, r0, r1, lsl #1
	smmls	  r9, r0, r7, r11
	smmla	  r8, r10, r2, r11
	ldrd	  r0, [r4, #8]
	rsb		  r9, r9, #0	
	add		  r10, r6, r3
	strd	  r8, [lr, #0] 		
	smmul 	  r11, r1, r10
	sub		  r2, r0, r1, lsl #1
	smmls     r9, r0, r3, r11
	add		  lr, lr, #8	
	smmla	  r8, r2, r6, r11
	rsb		  r9, r9, #0
	add		  r4, r4, #16	
	strd	  r8, [r5, #0]
	sub       r12, r12, #1
	sub		  r5, r5, #8	
	cmp       r12, #0
	bhi       L3881
L3236:
	add       sp, sp, #8 
	ldmia     sp!, {r4 - r11, pc}
@L3885:
@	.word				cook_cos4sin4tab
@	.word       cook_cos4sin4tabOffset
@	.word       cook_nmltTab
@M3883

	@ENDP  @ AAD_srtidata517
	.END
