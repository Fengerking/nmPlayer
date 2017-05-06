
	AREA	|.rdata|, DATA, READONLY

	EXPORT	|YUYVToRGB16|

	AREA	|.text|, CODE, ARM

|YUYVToRGB16| PROC
	stmdb		sp!, {r4 - r12, lr}
	sub			sp, sp, #24
	ldr			r6, [r0, #16]			; r6 = _ccInWidth 
	str			r6, [sp, #4]			; store _ccInWidth
	ldr			r12, [r0, #20]			; r12 = _ccInHeight
	str			r12, [sp, #8]			; store _ccInHeight

	ldr			r8, [r0, #24]			; r8 = **_outBuf
	ldr			r9, [r0, #28]			; r9 = **_inBuf

	ldr			r8, [r8]				; r8 = &_outBuf[0]
	ldr			r9, [r9]				; r9 = Y = &_inBuf[0]
	ldr 		r4, [r9], #4				; r4 = 1st VYUY
	ldr			r12, [r0, #36]			; r12 = outStride(char)
	sub			r12, r12, r6, lsl #1
	str			r12, [sp, #12]			; store offset of stride


	ldr			r1, =-5989466			; r1 = ConstV2
	ldr			r5, =14847836			; lr = ConstU1
	ldr			r6, =-2885681			; r1 = ConstU2

LYUYVToRGB16Height_ARMv4
	ldr			r3, [sp, #4]			; load _ccInWidth
LYUYVToRGB16Width_ARMv4
	subs		r3, r3, #2
	ldr			lr, =11769217				; lr = ConstV1
	and			r7, r4, #0xff				; r7 = Y
	and			r10, r4, #0xff00			; 
	mov			r10, r10, lsr #8			; r10 = U 
	and			r11, r4, #0xff0000			;			
	mov			r11, r11, lsr #16			; r11 = Y
	mov			r4, r4, lsr	#24				; r4 = V

	sub			r10, r10, #0x80				; r10 = U - 128
	mul			r12, r10, r5				; r12 = a2
	sub			r4, r4, #0x80				; r4 = V - 128
	mul			r0, r4, lr					; r0 = a0
	mov			r12, r12, asr #23			; r12 = a2
	mul			lr, r4, r1					; lr = ConstV2(V - 128)
	ldr 		r4, [r9], #4				; r4 = 1st VYUY preload
	mov			r0, r0, asr #23				; r0 = a0

	mla			r10, r10, r6, lr			; r10 = ConstV2(V - 128) + ConstU2(U - 128) = a1
	add			lr, r7, r0					; lr = a3 + a0
	mov			lr, lr, asr #3				; lr = (a3 + a0)>>3
	ldrb		lr, [r2, +lr]				; lr = R1
	mov			r10, r10, asr #23			; r10 = a1
	add			r0, r11, r0					; r0 = a3 + a0
	mov			r0, r0, asr #3				; r0 = (a3 + a0)>>3
	ldrb		r0, [r2, +r0]				; r0 = R2

	eor			lr, lr, r0, lsl #16

	add			r0, r7, r10					; r0 = a3 + a1
	mov			r0, r0, asr #3				; r0 = (a3 + a1)>>3
	ldrb		r0, [r2, +r0]				; r0 = G1

	add			r10, r11, r10				; r10 = a3 + a1
	mov			r10, r10, asr #3			; r10 = (a3 + a1)>>3
	ldrb		r10, [r2, +r10]				; r10 = G2

	eor			r0, r0, r10, lsl #16

	add			r7, r7, r12					; r7 = a3 + a2
	mov			r7, r7, asr #3				; r7 = (a3 + a2)>>3
	ldrb		r7, [r2, +r7]				; r7 = B1

	eor			r0, r0, lr, lsl #5	
	add			r11, r11, r12				; r11 = a3 + a2
	mov			r11, r11, asr #3			; r11 = (a3 + a2)>>3
	ldrb		r11, [r2, +r11]				; r11 = B2

	eor			r7, r7, r11, lsl #16
	eor			r7, r7, r0, lsl #6 
	str			r7, [r8], #4	
	bne			LYUYVToRGB16Width_ARMv4
	ldr			r0, [sp, #12]				; r0 = offset of stride
	add			r8, r8, r0
	ldr			r0, [sp, #8]
	subs		r0, r0, #1
	strne		r0, [sp, #8]
	bne			LYUYVToRGB16Height_ARMv4	

	add			sp, sp, #24
	ldmia		sp!, {r4 - r12, pc}


	ENDP  ; |YUYVToRGB16_ARMv4|

	EXPORT	|YVYUToRGB16|

	AREA	|.text|, CODE, ARM

|YVYUToRGB16| PROC
	stmdb		sp!, {r4 - r12, lr}

	ldmia		sp!, {r4 - r12, pc}


	ENDP  ; |YVYUToRGB16|

	EXPORT	|VYUYToRGB16|

	AREA	|.text|, CODE, ARM

|VYUYToRGB16| PROC
	stmdb		sp!, {r4 - r12, lr}

	ldmia		sp!, {r4 - r12, pc}


	ENDP  ; |VYUYToRGB16|

	EXPORT	|UYVYToRGB16|

	AREA	|.text|, CODE, ARM

|UYVYToRGB16| PROC
	stmdb		sp!, {r4 - r12, lr}

	ldmia		sp!, {r4 - r12, pc}


	ENDP  ; |UYVYToRGB16|

	EXPORT	|YUYVToRGB16_downsample|


	AREA	|.text|, CODE, ARM

|YUYVToRGB16_downsample| PROC
	stmdb		sp!, {r4 - r12, lr}
	sub			sp, sp, #24
	ldr			r3, [r0, #16]			; r3 = _ccInWidth 
	str			r3, [sp, #4]			; store _ccInWidth
	ldr			r12, [r0, #20]			; r12 = _ccInHeight
	str			r12, [sp, #8]			; store _ccInHeight

	ldr			r8, [r0, #24]			; r8 = **_outBuf
	ldr			r9, [r0, #28]			; r9 = **_inBuf

	ldr			r8, [r8]				; r8 = &_outBuf[0]
	ldr			r9, [r9]				; r9 = Y = &_inBuf[0]
	add			r1, r9, r3, lsl #1		; r1 = next line

	ldr 		r4, [r9], #4			; r4 = 1st line VYUY
	ldr			r12, [r0, #36]			; r12 = outStride(char)
	ldr			r5, [r1], #4			; r5 = 2nd line VYUY

	sub			r12, r12, r3 
	str			r12, [sp, #12]			; store offset of stride		

LYUYVToRGB16Height_downsample
;	ldr			r3, [sp, #4]			; load _ccInWidth
LYUYVToRGB16Width_downsample
	subs		r3, r3, #4
	ldr			r12, =11769217			; r12 = ConstV1	
	;;1st two pixels
	and			r7, r4, #0xff				; r7 = Y1
	and			r10, r4, #0xff00			; 
	mov			r10, r10, lsr #8			; r10 = U1 
	and			r11, r4, #0xff0000			;			
	mov			r11, r11, lsr #16			; r11 = Y2
	mov			r0, r4, lsr	#24				; r0 = V1

	ldr			r4, [r9], #4				; preload next two pixels
	add			r7, r7, r11					; r7 = Y1 + Y2

	and			r11, r5, #0xff00			; 
	mov			r11, r11, lsr #8			; r11 = U2 
	add			r10, r10, r11
	mov			r10, r10, lsr #1			; r10 = U

	mov			r11, r5, lsr #24			; r0 = V2
	add			r0, r0, r11
	mov			r0, r0, lsr #1				; r0 = V

	and			r11, r5, #0xff				; r7 = Y3	
	add			r7, r7, r11					; r7 = Y1 + Y2 + Y3
	and			r11, r5, #0xff0000			;			
	mov			r11, r11, lsr #16			; r11 = Y4
	add			r7, r7, r11					; r7 = Y1 + Y2 + Y3 + Y4
	mov			r7, r7, lsr #2				; r7 = Y

	ldr			r5, [r1], #4				; preload next two pixels

	ldr			r6, =14847836
	sub			r10, r10, #0x80				; r10 = U - 128
	mul			r11, r10, r6				; r11 = a2
	sub			r0, r0, #0x80				; r0 = V - 128
	mul			lr, r0, r12					; lr = a0
	mov			r11, r11, asr #23			; r11 = a2
	ldr			r6, =-5989466
	mul			r0, r0, r6					; r0 = ConstV2(V - 128)
	mov			lr, lr, asr #23				; lr = a0

	ldr			r6, =-2885681
	mla			r10, r10, r6, r0			; r10 = ConstV2(V - 128) + ConstU2(U - 128) = a1

	add			lr, r7, lr					; lr = a3 + a0
	mov			lr, lr, asr #3				; lr = (a3 + a0)>>3
	ldrb		lr, [r2, +lr]				; lr = R
	mov			r10, r10, asr #23			; r10 = a1
	add			r0, r7, r10					; r0 = a3 + a1
	mov			r0, r0, asr #3				; r0 = (a3 + a1)>>3
	ldrb		r0, [r2, +r0]				; r0 = G 

	add			r7, r7, r11					; r7 = a3 + a2
	mov			r7, r7, asr #3				; r7 = (a3 + a2)>>3
	ldrb		r7, [r2, +r7]				; r7 = B 

	eor			lr, r0, lr, lsl	#5
	eor			lr, r7, lr, lsl #6 

	;; 2nd two pixels
	and			r7, r4, #0xff				; r7 = Y1
	and			r10, r4, #0xff00			; 
	mov			r10, r10, lsr #8			; r10 = U1 
	and			r11, r4, #0xff0000			;			
	mov			r11, r11, lsr #16			; r11 = Y2
	mov			r0, r4, lsr	#24				; r0 = V1

	ldrne		r4, [r9], #4				; preload next two pixels
	add			r7, r7, r11					; r7 = Y1 + Y2

	and			r11, r5, #0xff00			; 
	mov			r11, r11, lsr #8			; r11 = U2 
	add			r10, r10, r11
	mov			r10, r10, lsr #1			; r10 = U

	mov			r11, r5, lsr #24			; r0 = V2
	add			r0, r0, r11
	mov			r0, r0, lsr #1				; r0 = V

	and			r11, r5, #0xff				; r7 = Y3	
	add			r7, r7, r11					; r7 = Y1 + Y2 + Y3
	and			r11, r5, #0xff0000			;			
	mov			r11, r11, lsr #16			; r11 = Y4
	add			r7, r7, r11					; r7 = Y1 + Y2 + Y3 + Y4
	mov			r7, r7, lsr #2				; r7 = Y

	ldrne		r5, [r1], #4				; preload next two pixels

	ldr			r6, =14847836
	sub			r10, r10, #0x80				; r10 = U - 128
	mul			r11, r10, r6				; r11 = a2
	sub			r0, r0, #0x80				; r0 = V - 128
	mul			r12, r0, r12				; r12 = a0
	mov			r11, r11, asr #23			; r11 = a2
	ldr			r6, =-5989466
	mul			r0, r0, r6					; r0 = ConstV2(V - 128)
	mov			r12, r12, asr #23			; r12 = a0

	ldr			r6, =-2885681
	mla			r10, r10, r6, r0			; r10 = ConstV2(V - 128) + ConstU2(U - 128) = a1

	add			r12, r7, r12				; r12 = a3 + a0
	mov			r12, r12, asr #3			; r12 = (a3 + a0)>>3
	ldrb		r12, [r2, +r12]				; r12 = R
	mov			r10, r10, asr #23			; r10 = a1
	add			r0, r7, r10					; r0 = a3 + a1
	mov			r0, r0, asr #3				; r0 = (a3 + a1)>>3
	ldrb		r0, [r2, +r0]				; r0 = G 

	add			r7, r7, r11					; r7 = a3 + a2
	mov			r7, r7, asr #3				; r7 = (a3 + a2)>>3
	ldrb		r7, [r2, +r7]				; r7 = B 

	eor			r12, r0, r12, lsl #5
	eor			r12, r7, r12, lsl #6 

	eor			lr, lr, r12, lsl #16

	str			lr, [r8], #4
	bne			LYUYVToRGB16Width_downsample
	ldr			r0, [sp, #8]
	subs		r0, r0, #2
	strne		r0, [sp, #8]

	ldrne		r3, [sp, #4]				; load _ccInWidth
	addne		r9, r9, r3, lsl #1
	addne		r1, r1, r3, lsl #1
	ldrne		r4, [r9], #4
	ldrne		r5, [r1], #4

	ldrne		r0, [sp, #12]				; r0 = offset of stride
	addne		r8, r8, r0

	bne			LYUYVToRGB16Height_downsample	


	add			sp, sp, #24
	ldmia		sp!, {r4 - r12, pc}


	ENDP  ; |YUYVToRGB16_downsample|



	EXPORT	|YUYVToRGB16_rotation_90L|


	AREA	|.text|, CODE, ARM

|YUYVToRGB16_rotation_90L| PROC
	stmdb		sp!, {r4 - r12, lr}
	sub			sp, sp, #24
	ldr			r3, [r0, #16]			; r3 = _ccInWidth 
	str			r3, [sp, #4]			; store _ccInWidth
	ldr			r12, [r0, #20]			; r12 = _ccInHeight
	str			r12, [sp, #8]			; store _ccInHeight

	ldr			r8, [r0, #24]			; r8 = **_outBuf
	ldr			r9, [r0, #28]			; r9 = **_inBuf

	ldr			r8, [r8]				; r8 = &_outBuf[0]
	ldr			r9, [r9]				; r9 = Y = &_inBuf[0]
	add			r1, r9, r3, lsl #1		; r1 = next line

	ldr 		r4, [r9], #4			; r4 = 1st VYUY
	ldr			r12, [r0, #36]			; r12 = outStride(char)

	sub			r5, r3, #1
	mul			r5, r12, r5
	add			r8, r8, r5				; r8 = outbuf start
	str			r8, [sp, #12] 


LYUYVToRGB16_rotation_90LHeight_ARMv4
;	ldr			r3, [sp, #4]			; load _ccInWidth
LYUYVToRGB16_rotation_90LWidth_ARMv4
	subs		r3, r3, #2
	ldr			lr, =11769217				; lr = ConstV1
	ldr			r6, =-2885681				; r6 = ConstU2
	ldr			r5, =14847836				; r5 = ConstU1

	and			r7, r4, #0xff				; r7 = Y
	and			r10, r4, #0xff00			; 
	mov			r10, r10, lsr #8			; r10 = U 
	and			r11, r4, #0xff0000			;			
	mov			r11, r11, lsr #16			; r11 = Y
	mov			r4, r4, lsr	#24				; r4 = V

	sub			r10, r10, #0x80				; r10 = U - 128
	mul			r5, r10, r5					; r5 = a2
	sub			r4, r4, #0x80				; r4 = V - 128
	mul			r0, r4, lr					; r0 = a0
	ldr			lr, =-5989466				; lr = ConstV2
	mov			r5, r5, asr #23				; r5 = a2
	mul			lr, r4, lr					; lr = ConstV2(V - 128)

	ldr 		r4, [r1], #4				; r4 = next line VYUY preload
	mov			r0, r0, asr #23				; r0 = a0
	mla			r10, r10, r6, lr			; r10 = ConstV2(V - 128) + ConstU2(U - 128) = a1

	add			lr, r7, r0					; lr = a3 + a0
	mov			lr, lr, asr #3				; lr = (a3 + a0)>>3
	ldrb		lr, [r2, +lr]				; lr = R1
	mov			r10, r10, asr #23			; r10 = a1
	add			r0, r11, r0					; r0 = a3 + a0
	mov			r0, r0, asr #3				; r0 = (a3 + a0)>>3
	ldrb		r0, [r2, +r0]				; r0 = R2

	eor			lr, lr, r0, lsl #16

	add			r0, r7, r10					; r0 = a3 + a1
	mov			r0, r0, asr #3				; r0 = (a3 + a1)>>3
	ldrb		r0, [r2, +r0]				; r0 = G1

	add			r10, r11, r10				; r10 = a3 + a1
	mov			r10, r10, asr #3			; r10 = (a3 + a1)>>3
	ldrb		r10, [r2, +r10]				; r10 = G2

	eor			r0, r0, r10, lsl #16

	add			r7, r7, r5					; r7 = a3 + a2
	mov			r7, r7, asr #3				; r7 = (a3 + a2)>>3
	ldrb		r7, [r2, +r7]				; r7 = B1

	eor			r0, r0, lr, lsl #5	
	add			r11, r11, r5				; r11 = a3 + a2
	mov			r11, r11, asr #3			; r11 = (a3 + a2)>>3
	ldrb		r11, [r2, +r11]				; r11 = B2

	eor			r7, r7, r11, lsl #16
	eor			r7, r7, r0, lsl #6 

	;;;next line VYUY
	and			r10, r4, #0xff00			; 
	mov			r10, r10, lsr #8			; r10 = U 
	mov			r5, r4, lsr	#24				; r5 = V

	ldr			lr, =14847836				; lr = ConstU1		
	sub			r10, r10, #0x80				; r10 = U - 128
	mul			r0, r10, lr					; r0 = a2
	ldr			lr, =11769217				; lr = ConstV1
	sub			r5, r5, #0x80				; r5 = V - 128
	mul			r11, r5, lr					; r11 = a0
	ldr			lr, =-5989466				; lr = ConstV2
	mov			r0, r0, asr #23				; r0 = a2
	mul			lr, r5, lr					; lr = ConstV2(V - 128)
	mla			r10, r10, r6, lr			; r10 = ConstV2(V - 128) + ConstU2(U - 128) = a1

	mov			r11, r11, asr #23			; r11 = a0
	and			r5, r4, #0xff				; r5 = Y
	and			r6, r4, #0xff0000			;			
	mov			r6, r6, lsr #16				; r6 = Y

	ldrne 		r4, [r9], #4				; r4 = first line VYUY preload


	add			lr, r5, r11					; lr = a3 + a0
	mov			lr, lr, asr #3				; lr = (a3 + a0)>>3
	ldrb		lr, [r2, +lr]				; lr = R1
	mov			r10, r10, asr #23			; r10 = a1
	add			r11, r6, r11				; r11 = a3 + a0
	mov			r11, r11, asr #3			; r11 = (a3 + a0)>>3
	ldrb		r11, [r2, +r11]				; r11 = R2

	eor			lr, lr, r11, lsl #16

	add			r11, r5, r10				; r11 = a3 + a1
	mov			r11, r11, asr #3			; r11 = (a3 + a1)>>3
	ldrb		r11, [r2, +r11]				; r11 = G1

	add			r10, r6, r10				; r10 = a3 + a1
	mov			r10, r10, asr #3			; r10 = (a3 + a1)>>3
	ldrb		r10, [r2, +r10]				; r10 = G2

	eor			r11, r11, r10, lsl #16

	add			r5, r5, r0					; r5 = a3 + a2
	mov			r5, r5, asr #3				; r5 = (a3 + a2)>>3
	ldrb		r5, [r2, +r5]				; r5 = B1

	eor			r11, r11, lr, lsl #5	
	add			r6, r6, r0					; r6 = a3 + a2
	mov			r6, r6, asr #3				; r6 = (a3 + a2)>>3
	ldrb		r6, [r2, +r6]				; r6 = B2

	eor			r5, r5, r6, lsl #16
	ldr			r10, =0xffff
	eor			r5, r5, r11, lsl #6 
	ldr			r11, =0xffff0000
	
	and			r10, r7, r10
	eor			r10, r10, r5, lsl #16
	and			r11, r5, r11
	eor			r11, r11, r7, lsr #16
	str			r10, [r8], -r12
	str			r11, [r8], -r12

	bne			LYUYVToRGB16_rotation_90LWidth_ARMv4
	ldr			r0, [sp, #8]
	subs		r0, r0, #2
	strne		r0, [sp, #8]

	ldrne		r3, [sp, #4]				; load _ccInWidth
	addne		r9, r9, r3, lsl #1
	addne		r1, r1, r3, lsl #1
	ldrne		r4, [r9], #4

	ldrne		r8, [sp, #12]				; 
	addne		r8, r8, #4
	strne		r8, [sp, #12]

	bne			LYUYVToRGB16_rotation_90LHeight_ARMv4	

	add			sp, sp, #24
	ldmia		sp!, {r4 - r12, pc}


	ENDP  ; |YUYVToRGB16_rotation_90L|



	EXPORT	|YUYVToRGB16_rotation_90R|


	AREA	|.text|, CODE, ARM

|YUYVToRGB16_rotation_90R| PROC
	stmdb		sp!, {r4 - r12, lr}
	sub			sp, sp, #24
	ldr			r3, [r0, #16]			; r3 = _ccInWidth 
	str			r3, [sp, #4]			; store _ccInWidth
	ldr			r12, [r0, #20]			; r12 = _ccInHeight
	str			r12, [sp, #8]			; store _ccInHeight

	ldr			r8, [r0, #24]			; r8 = **_outBuf
	ldr			r9, [r0, #28]			; r9 = **_inBuf

	ldr			r8, [r8]				; r8 = &_outBuf[0]
	ldr			r9, [r9]				; r9 = Y = &_inBuf[0]
	add			r1, r9, r3, lsl #1		; r1 = next line

	ldr 		r4, [r9], #4			; r4 = 1st VYUY
	ldr			r12, [r0, #36]			; r12 = outStride(char)

	sub			r5, r12, #4
	add			r8, r8, r5				; r8 = outbuf start
	str			r8, [sp, #12] 


LYUYVToRGB16_rotation_90RHeight_ARMv4
;	ldr			r3, [sp, #4]			; load _ccInWidth
LYUYVToRGB16_rotation_90RWidth_ARMv4
	subs		r3, r3, #2
	ldr			lr, =11769217				; lr = ConstV1
	ldr			r6, =-2885681				; r6 = ConstU2
	ldr			r5, =14847836				; r5 = ConstU1

	and			r7, r4, #0xff				; r7 = Y
	and			r10, r4, #0xff00			; 
	mov			r10, r10, lsr #8			; r10 = U 
	and			r11, r4, #0xff0000			;			
	mov			r11, r11, lsr #16			; r11 = Y
	mov			r4, r4, lsr	#24				; r4 = V

	sub			r10, r10, #0x80				; r10 = U - 128
	mul			r5, r10, r5					; r5 = a2
	sub			r4, r4, #0x80				; r4 = V - 128
	mul			r0, r4, lr					; r0 = a0
	ldr			lr, =-5989466				; lr = ConstV2
	mov			r5, r5, asr #23				; r5 = a2
	mul			lr, r4, lr					; lr = ConstV2(V - 128)

	ldr 		r4, [r1], #4				; r4 = next line VYUY preload
	mov			r0, r0, asr #23				; r0 = a0
	mla			r10, r10, r6, lr			; r10 = ConstV2(V - 128) + ConstU2(U - 128) = a1

	add			lr, r7, r0					; lr = a3 + a0
	mov			lr, lr, asr #3				; lr = (a3 + a0)>>3
	ldrb		lr, [r2, +lr]				; lr = R1
	mov			r10, r10, asr #23			; r10 = a1
	add			r0, r11, r0					; r0 = a3 + a0
	mov			r0, r0, asr #3				; r0 = (a3 + a0)>>3
	ldrb		r0, [r2, +r0]				; r0 = R2

	eor			lr, lr, r0, lsl #16

	add			r0, r7, r10					; r0 = a3 + a1
	mov			r0, r0, asr #3				; r0 = (a3 + a1)>>3
	ldrb		r0, [r2, +r0]				; r0 = G1

	add			r10, r11, r10				; r10 = a3 + a1
	mov			r10, r10, asr #3			; r10 = (a3 + a1)>>3
	ldrb		r10, [r2, +r10]				; r10 = G2

	eor			r0, r0, r10, lsl #16

	add			r7, r7, r5					; r7 = a3 + a2
	mov			r7, r7, asr #3				; r7 = (a3 + a2)>>3
	ldrb		r7, [r2, +r7]				; r7 = B1

	eor			r0, r0, lr, lsl #5	
	add			r11, r11, r5				; r11 = a3 + a2
	mov			r11, r11, asr #3			; r11 = (a3 + a2)>>3
	ldrb		r11, [r2, +r11]				; r11 = B2

	eor			r7, r7, r11, lsl #16
	eor			r7, r7, r0, lsl #6 

	;;;next line VYUY
	and			r10, r4, #0xff00			; 
	mov			r10, r10, lsr #8			; r10 = U 
	mov			r5, r4, lsr	#24				; r5 = V

	ldr			lr, =14847836				; lr = ConstU1		
	sub			r10, r10, #0x80				; r10 = U - 128
	mul			r0, r10, lr					; r0 = a2
	ldr			lr, =11769217				; lr = ConstV1
	sub			r5, r5, #0x80				; r5 = V - 128
	mul			r11, r5, lr					; r11 = a0
	ldr			lr, =-5989466				; lr = ConstV2
	mov			r0, r0, asr #23				; r0 = a2
	mul			lr, r5, lr					; lr = ConstV2(V - 128)
	mla			r10, r10, r6, lr			; r10 = ConstV2(V - 128) + ConstU2(U - 128) = a1

	mov			r11, r11, asr #23			; r11 = a0
	and			r5, r4, #0xff				; r5 = Y
	and			r6, r4, #0xff0000			;			
	mov			r6, r6, lsr #16				; r6 = Y

	ldrne 		r4, [r9], #4				; r4 = first line VYUY preload


	add			lr, r5, r11					; lr = a3 + a0
	mov			lr, lr, asr #3				; lr = (a3 + a0)>>3
	ldrb		lr, [r2, +lr]				; lr = R1
	mov			r10, r10, asr #23			; r10 = a1
	add			r11, r6, r11				; r11 = a3 + a0
	mov			r11, r11, asr #3			; r11 = (a3 + a0)>>3
	ldrb		r11, [r2, +r11]				; r11 = R2

	eor			lr, lr, r11, lsl #16

	add			r11, r5, r10				; r11 = a3 + a1
	mov			r11, r11, asr #3			; r11 = (a3 + a1)>>3
	ldrb		r11, [r2, +r11]				; r11 = G1

	add			r10, r6, r10				; r10 = a3 + a1
	mov			r10, r10, asr #3			; r10 = (a3 + a1)>>3
	ldrb		r10, [r2, +r10]				; r10 = G2

	eor			r11, r11, r10, lsl #16

	add			r5, r5, r0					; r5 = a3 + a2
	mov			r5, r5, asr #3				; r5 = (a3 + a2)>>3
	ldrb		r5, [r2, +r5]				; r5 = B1

	eor			r11, r11, lr, lsl #5	
	add			r6, r6, r0					; r6 = a3 + a2
	mov			r6, r6, asr #3				; r6 = (a3 + a2)>>3
	ldrb		r6, [r2, +r6]				; r6 = B2

	eor			r5, r5, r6, lsl #16
	ldr			r10, =0xffff
	eor			r5, r5, r11, lsl #6 
	ldr			r11, =0xffff0000
	
	and			r10, r5, r10
	eor			r10, r10, r7, lsl #16
	and			r11, r7, r11
	eor			r11, r11, r5, lsr #16
	str			r10, [r8], +r12
	str			r11, [r8], +r12

	bne			LYUYVToRGB16_rotation_90RWidth_ARMv4
	ldr			r0, [sp, #8]
	subs		r0, r0, #2
	strne		r0, [sp, #8]

	ldrne		r3, [sp, #4]				; load _ccInWidth
	addne		r9, r9, r3, lsl #1
	addne		r1, r1, r3, lsl #1
	ldrne		r4, [r9], #4

	ldrne		r8, [sp, #12]				; 
	subne		r8, r8, #4
	strne		r8, [sp, #12]

	bne			LYUYVToRGB16_rotation_90RHeight_ARMv4	

	add			sp, sp, #24
	ldmia		sp!, {r4 - r12, pc}


	ENDP  ; |YUYVToRGB16_rotation_90R|
	END
