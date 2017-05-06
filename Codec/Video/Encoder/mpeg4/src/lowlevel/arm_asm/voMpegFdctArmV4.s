
	AREA	|.data|, DATA

	EXPORT	|FastDct_ARMV4|

	AREA	|.pdata|, PDATA
|$T34467| DCD	|FastDct_ARMV4|
	DCD	0x4000e102
	AREA	|.text|, CODE

|FastDct_ARMV4| PROC
	stmdb     sp!, {r4 - r11, lr}
	sub       sp, sp, #8
|$M34465|
	str       r0, [sp, #4]
	mov       r11, r0
	mov       r0, #8
	str       r0, [sp]
	mov       lr, r0
|$L34370|
	ldrsh     r1, [r11, #0xC]			;r1 = in[6]
	ldrsh     r0, [r11, #2]				;r0 = in[1]

	ldrsh     r2, [r11, #0xA]			;r2 = in[5]
	ldrsh     r8, [r11, #4]				;r0 = in[2]

	ldrsh     r7, [r11, #8]				;r1 = in[4]
	add       r5, r0, r1				;r5 = mm1
	sub       r9, r0, r1				;r9 = mm6 --
	ldrsh     r0, [r11, #6]				;r0 = in[3]
	add       r4, r8, r2				;r4 = mm2
	sub       r10, r8, r2				;r10 = mm5 --

	ldrsh     r2, [r11, #0xE]			;r2 = in[7]


	add       r6, r4, r5				;r6 = mm2 + mm1 = mm1 --
	add       r3, r0, r7				;r3 = mm3
	sub       r8, r0, r7				;r8 = mm4 --
	ldrsh     r0, [r11]					;r0 = in[0]
	sub       r4, r5, r4				;r4 = mm1 - mm2 = mm2 --
	add       r1, r0, r2				;r1 = mm0 
	sub       r7, r0, r2				;r7 = mm7 --

	sub       r2, r1, r3				;r2 = mm0 - mm3 == mm3 --
	add       r5, r1, r3				;r5 = mm0 + mm3 == mm0 --

	mov       r3, #0x8A, 24
	add       r0, r2, r4				;r0 = mm3 + mm2
	orr       r3, r3, #0x8C				;r3 = ROT6_C
	mul       r1, r0, r3				;r1 = (mm3 + mm2)*ROT6_C

	mov       r0, #0xC3, 24
	orr       r0, r0, #0xEF				;r1 = ROT6_SmC
	add       r3, r1, #2, 20			;r3 = tmp
	mla       r0, r2, r0, r3
	ldr       r1, =0x1d907				;r1 = ROT6_SpC
	mov       r0, r0, asr #14

	strh      r0, [r11, #4]
	mul       r0, r4, r1

	add       r4, r7, r10				;r4 = mm5 + mm7 = mm3--
	sub       r0, r3, r0
	mov       r1, r0, asr #14
	strh      r1, [r11, #0xC]
	add       r1, r5, r6				;r1 = mm0 + mm1 = mm0--
	mov       r0, r1, lsl #2

	sub       r3, r5, r6				;r3 = mm0 - mm1 = mm1--
	strh      r0, [r11]
	mov       r0, r3, lsl #2			
	ldr       r3, =0x12d06				;r3 = ROT17_C
	strh      r0, [r11, #8]				;
	add       r2, r8, r9				;r2 = mm4 + mm6 
	add       r0, r4, r2				;r0 = mm2 + mm3 
	mul       r3, r0, r3				;r1 = (mm2 + mm3)*ROT17_C

	ldr       r1, =0x1f629				;r1 = ROT17_SpC
	mul       r0, r2, r1				;

	ldr       r2, =0xffff199b
	add       r3, r3, #2, 20			;r3 = tmp
	sub       r0, r3, r0				
	mov       r5, r0, asr #14			;r5 = 
	mov       r0, #0x63, 24
	orr       r0, r0, #0xE3				;r0 = ROT37_SpC
	mul       r1, r4, r0
	sub       r4, r3, r1
	mov       r4, r4, asr #14
	add       r3, r7, r8
	mul       r0, r3, r2
	add       r3, r0, #2, 20
	mov       r0, #6, 18
	orr       r0, r0, #0x57
	mla       r0, r7, r0, r3

	mov       r1, #0x13, 22
	orr       r1, r1, #0x73
	mla       r3, r8, r1, r3
	add       r0, r4, r0, asr #14
	strh      r0, [r11, #2]
	add       r0, r5, r3, asr #14
	mov       r3, #0x29, 20
	orr       r3, r3, #0x1B

	strh      r0, [r11, #0xE]
	add       r1, r10, r9
	mul       r0, r1, r3
	rsb       r3, r0, #2, 20
	ldr       r0, =0x3129d

	mla       r1, r9, r0, r3
	sub       lr, lr, #1
	add       r9, r5, r1, asr #14
	ldr       r2, =0x20d99
	cmp       lr, #0

	mla       r0, r10, r2, r3
	strh      r9, [r11, #6]
	add       r0, r4, r0, asr #14

	strh      r0, [r11, #0xA]

	add       r11, r11, #0x10
	bhi       |$L34370|
	mov       r11, #0x8A, 24
	ldr       lr, [sp, #4]
	orr       r11, r11, #0x8C
|$L34382|

	ldrsh     r1, [lr, #0x60]
	ldrsh     r0, [lr, #0x10]

	ldrsh     r2, [lr, #0x50]
	ldrsh     r5, [lr, #0x20]
	add       r3, r0, r1
	sub       r10, r0, r1
	add       r1, r5, r2
	sub       r9, r5, r2

	ldrsh     r8, [lr, #0x40]
	ldrsh     r0, [lr, #0x30]
	sub       r5, r3, r1
	ldrsh     r4, [lr]

	ldrsh     r2, [lr, #0x70]
	add       r6, r1, r3
	add       r3, r0, r8
	sub       r7, r0, r8
	add       r1, r4, r2
	sub       r8, r4, r2

	sub       r2, r1, r3
	add       r0, r2, r5
	add       r4, r3, r1

	mul       r3, r0, r11
	mov       r1, #0xC3, 24
	orr       r1, r1, #0xEF
	add       r3, r3, #1, 12
	mla       r0, r2, r1, r3

	ldr       r1, =0x1d907
	mul       r1, r5, r1
	mov       r0, r0, asr #21
	strh      r0, [lr, #0x20]
	sub       r0, r3, r1

	mov       r1, r0, asr #21
	strh      r1, [lr, #0x60]

	add       r1, r4, #0xF

	add       r0, r1, r6

	mov       r0, r0, asr #5
	sub       r3, r1, r6

	strh      r0, [lr]
	mov       r0, r3, asr #5

	ldr       r3, =0x12d06
	strh      r0, [lr, #0x40]
	add       r4, r8, r9
	add       r2, r7, r10
	add       r0, r4, r2
	mul       r3, r0, r3
	ldr       r1, =0x1f629
	mul       r1, r2, r1
	add       r3, r3, #1, 12
	sub       r5, r3, r1
	mov       r0, #0x63, 24
	orr       r0, r0, #0xE3
	mul       r1, r4, r0

	add       r0, r8, r7
	sub       r4, r3, r1
	ldr       r1, =0xffff199b
	mul       r3, r0, r1
	mov       r0, #0x13, 22
	orr       r0, r0, #0x73
	mla       r1, r7, r0, r3

	add       r0, r1, r5
	mov       r1, r0, asr #21
	strh      r1, [lr, #0x70]
	mov       r0, #6, 18
	orr       r0, r0, #0x57
	mla       r1, r8, r0, r4

	add       r0, r1, r3
	mov       r1, r0, asr #21
	strh      r1, [lr, #0x10]

	add       r1, r9, r10
	ldr       r0, =0xfffd6fe5
	mul       r3, r1, r0
	ldr       r1, =0x20d99
	mla       r0, r9, r1, r4

	add       r0, r0, r3
	mov       r1, r0, asr #21
	strh      r1, [lr, #0x50]
	ldr       r0, =0x3129d
	mla       r1, r10, r0, r3

	add       r0, r1, r5
	mov       r1, r0, asr #21
	strh      r1, [lr, #0x30]

	add       lr, lr, #2
	ldr       r0, [sp]
	sub       r0, r0, #1
	str       r0, [sp]
	cmp       r0, #0
	bhi       |$L34382|

	add       sp, sp, #8
	ldmia     sp!, {r4 - r11, pc}


	ENDP  ; 

	END
