        #include "voRADecID.h"	
        .text
        .align  4
	.globl	_R8FirstPass

	@AREA	.text, CODE, READONLY

_R8FirstPass: @PROC
	stmdb     sp!, {r4 - r11, lr}
	sub       sp, sp, #0x30
M2649:
	mov       r12, r0
	str       r1, [sp, #4]
	str       r12, [sp]
	ldr       lr, L2652

	cmp       r1, #0
	beq       L2125
L2123:
	ldrd	  r0, [r12,  #0]
	ldrd	  r2, [r12,  #8]	
	ldrd	  r4, [r12, #16]
	ldrd	  r6, [r12, #24]	

	add	  r8, r0, r2				@ ar = x[0] + x[2]@
	sub	  r10, r0, r2				@ br = x[0] - x[2]@
	add	  r9, r1, r3				@ ai = x[1] + x[3]@
	sub	  r11, r1, r3				@ bi = x[1] - x[3]@
	add	  r0, r4, r6				@ cr = x[4] + x[6]@
	sub	  r2, r4, r6				@ dr = x[4] - x[6]@				
	add	  r1, r5, r7				@ ci = x[5] + x[7]@
	sub	  r3, r5, r7				@ di = x[5] - x[7]@
	
	add	  r4, r8, r0				@ sr = ar + cr@
	sub	  r6, r8, r0				@ ur = ar - cr@
	add	  r5, r9, r1				@ si = ai + ci@
	sub	  r7, r9, r1				@ ui = ai - ci@
	
	sub	  r0, r10, r3				@ tr = br - di@
	add	  r8, r10, r3				@ vr = br + di@
	add	  r1, r11, r2				@ ti = bi + dr@
	sub	  r9, r11, r2				@ vi = bi - dr@
	
	str 	  r4, [sp, #8]
	str 	  r5, [sp, #12]
	str 	  r6, [sp, #16]
	str 	  r7, [sp, #20]
	str 	  r0, [sp, #24]
	str 	  r1, [sp, #28]
	str 	  r8, [sp, #32]
	str 	  r9, [sp, #36]
	
	ldrd	  r0, [r12, #32]
	ldrd	  r2, [r12, #40]	
	ldrd	  r4, [r12, #48]
	ldrd	  r6, [r12, #56]	

	add       r8, r0, r2
	sub	  r10, r0, r2		
	add	  r9, r1, r3
	sub	  r11, r1, r3
	add	  r0, r4, r6
	sub	  r2, r4, r6
	add	  r1, r5, r7
	sub	  r3, r5, r7
	
	add	  r4, r8, r0			@ wr = (ar + cr) >> 1@
	sub	  r6, r8, r0			@ yr = (ar - cr) >> 1@
	add	  r5, r9, r1			@ wi = (ai + ci) >> 1@
	sub	  r7, r9, r1			@ yi = (ai - ci) >> 1@
	
	ldr 	  r8, [sp, #8]
	ldr 	  r9, [sp, #12]
	mov       r4, r4, asr #1	
	mov	  r5, r5, asr #1	
	add	  r0, r4, r8, asr #1	@ x[ 0] = (sr >> 1) + wr@
	add	  r1, r5, r9, asr #1	@ x[ 1] = (si >> 1) + wi@
	strd	  r0,  [r12,  #0]

	mov	  r7, r7, asr #1

	rsb	  r0, r4, r8, asr #1	@ x[ 8] = (sr >> 1) - wr@
	rsb	  r1, r5, r9, asr #1	@ x[ 9] = (si >> 1) - wi@

	ldr 	  r8, [sp, #16]	
	ldr 	  r9, [sp, #20]	

	strd	  r0,  [r12, #32]

	mov	  r6, r6, asr #1

	add	  r4, r7, r8, asr #1	@ x[ 4] = (ur >> 1) + yi@
	rsb	  r0, r7, r8, asr #1	@ x[ 5] = (ui >> 1) - yr@
	rsb	  r5, r6, r9, asr #1	@ x[12] = (ur >> 1) - yi@
	add	  r1, r6, r9, asr #1	@ x[13] = (ui >> 1) + yr@
	
	strd	  r4, [r12, #16]
	strd      r0, [r12, #48]	 		
		
	sub	  r8, r10, r3
    add		  r0, r10, r3
	add	  r9, r11, r2
	sub	  r1, r11, r2
	
	sub	  r4, r8, r9
	add	  r5, r8, r9
	sub	  r6, r0, r1
    add		  r7, r0, r1
	
	ldr 	  r8, [sp, #24]	
	ldr 	  r9, [sp, #28]	
	ldr 	  r10, [sp, #32]
	ldr 	  r11, [sp, #36]
	
	smmul	  r0, r4, lr
	mov	  r8, r8, asr #1
	smmul	  r1, r5, lr
	mov	  r9, r9, asr #1
	smmul	  r2, r6, lr
	mov	  r10, r10, asr #1
	smmul	  r3, r7, lr
	mov	  r11, r11, asr #1
	
	sub	  r4, r8, r0
	add	  r6, r8, r0
	sub	  r5, r9, r1
	add	  r7, r9, r1
	strd	  r4, [r12, #24]
	strd	  r6, [r12, #56]
		
	ldr       r8, [sp, #4]	
	add	  r0, r10, r3
	sub	  r4, r10, r3
	sub  	  r1, r11, r2
	add	  r5, r11, r2
	strd	  r0, [r12,  #8]
	sub       r8, r8, #1
	strd	  r4, [r12, #40]
	cmp       r8, #0
	add       r12, r12, #0x40
	str       r8, [sp, #4]
	bhi       L2123
L2125:

	add       sp, sp, #0x30
	ldmia     sp!, {r4 - r11, pc}
L2652:
	.word		0x5a82799a
@M2650

	@ENDP  @ R8FirstPass

	.globl	_R4FirstPass

	@AREA	.text, CODE, READONLY

_R4FirstPass: @ PROC
	stmdb     sp!, {r4 - r11, lr}

M3649:
	mov       lr, r0
	mov	  r12, r1
	cmp       r1, #0
	beq       L3125
L3123:
	ldrd	  r0, [lr,  #0]
	ldrd	  r2, [lr,  #8]	
	ldrd	  r4, [lr, #16]
	ldrd	  r6, [lr, #24]

	add	  r8,  r0, r2
	sub	  r10, r0, r2
	add	  r9,  r1, r3	
	sub	  r11, r1, r3
        add	  r0, r4, r6
	sub	  r2, r4, r6
	add	  r1, r5, r7
	sub	  r3, r5, r7
	
	add	  r4, r8, r0
	sub	  r6, r8, r0
	add	  r5, r9, r1
	sub	  r7, r9, r1
	
	add	  r8, r10, r3
	sub	  r0, r10, r3
	sub	  r9, r11, r2
	add	  r1, r11, r2
	
	strd	  r4, [lr,  #0]
	strd	  r8, [lr,  #8]	
	strd	  r6, [lr, #16]
	strd	  r0, [lr, #24]

	sub       r12, r12, #1
	add       lr, lr, #0x20
	cmp       r12, #0
	bhi       L3123
L3125:
	ldmia     sp!, {r4 - r11, pc}
@M3650
	@ENDP  @ R4FirstPass
	@.END
