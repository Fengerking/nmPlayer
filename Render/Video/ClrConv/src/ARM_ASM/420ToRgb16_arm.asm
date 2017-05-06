
	AREA	|.rdata|, DATA, READONLY

	EXPORT	|YUV420ToRGB16_v5|

	IMPORT	|ccClip31|

	AREA	|.text|, CODE, ARM

|YUV420ToRGB16_v5| PROC
	stmdb		sp!, {r4 - r12, lr}
	sub			sp, sp, #16
	ldr			r6, [r0, #16]			; r6 = _ccInWidth 
	str			r6, [sp, #4]			; store _ccInWidth
	ldr			r12, [r0, #20]			; r12 = _ccInHeight
	str			r12, [sp, #8]			; store _ccInHeight
	
	ldr			r8, [r0, #24]			; r8 = **_outBuf
	ldr			r9, [r0, #28]			; r9 = **_inBuf

	ldr			r8,  [r8]				; r8 = &_outBuf[0]
	ldr			r11, [r9]				; r11 = Y = &_inBuf[0]
	ldr			r10, [r9, #4]			; r10 = U = &_inBuf[1]
	ldr			r9,  [r9, #8]			; r9 = V = &_inBuf[2]

	sub			r7, r12, #1				; r7 = (_ccInHeight - 1)			
	mul			r7, r7, r6				; r7 = (_ccInHeight - 1) * _ccInWidth 
	mov			r7, r7, lsl #1			; r12 = (_ccInHeight - 1) * (_ccInWidth<<1)

	mov			r12, #6
	sub			r12, r12, r1			; left shift for g

	add			r8, r8, r7				; start of outbuf	

;	adr			r4, LClip31
;	ldr			r2, [r4]				; r2 = ccClip31

L420ToRGB16Height
	ldr			r3, [sp, #4]			; load _ccInWidth

L420ToRGB16Width

	ldrb		r4, [r9], #1				; r4 = V
	sub			r4, r4, #0x80				; r4 = V - 128
	mov			r5, #0x66					; r5 = ccWConstV1
	smulbb		r5, r4, r5					; r5 =a0
		
	ldr			lr, =0xffcc					; lr = ccWConstV2
	smulbb		lr, r4, lr					; lr = ccWConstV2(V - 128)
	ldrb		r4, [r10], #1				; r4 = U
	sub			r4, r4, #0x80				; r4 = U - 128
	ldr			r1, =0xffe7					; r1 = ccWConstU2
	smlabb		r1, r1, r4, lr				; r1 = ccWConstV2(V - 128) + ccWConstU2(U - 128) = a1
	
	ldr			lr, =0x81					; lr = ccWConstU1
	smulbb		r4, r4, lr					; r4 = ccWConstU1(U - 128) = a2

	ldrb		r0, [r11]					; r0 = Y
	sub			r0, r0, #0x10				; r4 = Y - 16
	ldr			lr, =0x4a					; lr = 	ccWConstY
	smulbb		r0, r0, lr					; r0 = 	ccWConstY(Y - 16) = a3

	add			lr, r0, r5					; lr = a3 + a0
	mov			lr, lr, asr #9				; lr = (a3 + a0)>>9
	ldrb		lr, [r2, +lr]				; lr = r

	add			r7, r0, r1					; r7 = a3 + a1
	mov			r7, r7, asr #9				; r7 = (a3 + a1)>>9
	ldrb		r7, [r2, +r7]				; r7 = g

	add			r0, r0, r4					; r0 = a3 + a2
	mov			r0, r0, asr #9				; r0 = (a3 + a2)>>9
	ldrb		r0, [r2, +r0]				; r0 = b

	eor			lr, r7, lr, lsl	#5
	eor			lr, r0, lr, lsl r12 

	strh		lr, [r8]					; store 16 bit rgb

	ldrb		r0, [r11, #1]				; r0 = Y
	sub			r0, r0, #0x10				; r4 = Y - 16
	ldr			lr, =0x4a					; lr = 	ccWConstY
	smulbb		r0, r0, lr					; r0 = 	ccWConstY(Y - 16) = a3

	add			lr, r0, r5					; lr = a3 + a0
	mov			lr, lr, asr #9				; lr = (a3 + a0)>>9
	ldrb		lr, [r2, +lr]				; lr = r

	add			r7, r0, r1					; r7 = a3 + a1
	mov			r7, r7, asr #9				; r7 = (a3 + a1)>>9
	ldrb		r7, [r2, +r7]				; r7 = g

	add			r0, r0, r4					; r0 = a3 + a2
	mov			r0, r0, asr #9				; r0 = (a3 + a2)>>9
	ldrb		r0, [r2, +r0]				; r0 = b

	eor			lr, r7, lr, lsl	#5
	eor			lr, r0, lr, lsl r12 

	strh		lr, [r8, #2]				; store 16 bit rgb	

	add			lr, r11, r6					; next Y line
	ldrb		r0, [lr]					; r0 = Y
	sub			r0, r0, #0x10				; r4 = Y - 16
	ldr			lr, =0x4a					; lr = 	ccWConstY
	smulbb		r0, r0, lr					; r0 = 	ccWConstY(Y - 16) = a3

	add			lr, r0, r5					; lr = a3 + a0
	mov			lr, lr, asr #9				; lr = (a3 + a0)>>9
	ldrb		lr, [r2, +lr]				; lr = r

	add			r7, r0, r1					; r7 = a3 + a1
	mov			r7, r7, asr #9				; r7 = (a3 + a1)>>gshit
	ldrb		r7, [r2, +r7]				; r7 = g

	add			r0, r0, r4					; r0 = a3 + a2
	mov			r0, r0, asr #9				; r0 = (a3 + a2)>>9
	ldrb		r0, [r2, +r0]				; r0 = b

	eor			lr, r7, lr, lsl	#5
	eor			lr, r0, lr, lsl r12 
	
	mov			r7, r6, lsl #1				; r7 =  _ccInWidth<<1
	sub			r7, r8, r7					; r7 = loc - (_ccInWidth<<1)
	strh		lr, [r7]					; store 16 bit rgb	

	add			lr, r11, r6					; next Y line
	ldrb		r0, [lr, #1]				; r0 = Y
	sub			r0, r0, #0x10				; r4 = Y - 16
	ldr			lr, =0x4a					; lr = 	ccWConstY
	smulbb		r0, r0, lr					; r0 = 	ccWConstY(Y - 16) = a3

	add			lr, r0, r5					; lr = a3 + a0
	mov			lr, lr, asr #9				; lr = (a3 + a0)>>9
	ldrb		lr, [r2, +lr]				; lr = r

	add			r7, r0, r1					; r7 = a3 + a1
	mov			r7, r7, asr #9				; r7 = (a3 + a1)>>9
	ldrb		r7, [r2, +r7]				; r7 = g

	add			r0, r0, r4					; r0 = a3 + a2
	mov			r0, r0, asr #9				; r0 = (a3 + a2)>>9
	ldrb		r0, [r2, +r0]				; r0 = b

	eor			lr, r7, lr, lsl	#5
	eor			lr, r0, lr, lsl r12 
	
	mov			r7, r6, lsl #1				; r7 =  _ccInWidth<<1
	sub			r1, r8, r7					; r7 = loc - (_ccInWidth<<1)
	strh		lr, [r1, #2]				; store 16 bit rgb	

	add			r11, r11, #2
	add			r8, r8, #4					; r8 = loc + 4

	subs		r3, r3, #2
	bne			L420ToRGB16Width
	mov			r0, r6, lsl #2
	sub			r8, r8, r7
	sub			r8, r8, r0
	add			r11, r11, r6
	ldr			r5, [sp, #8]
	subs		r5, r5, #2
	strne		r5, [sp, #8]
	bne			L420ToRGB16Height	


	add			sp, sp, #16
	ldmia		sp!, {r4 - r12, pc}


	ENDP  ; |YUV420ToRGB16_v5|

	EXPORT	|YUV420ToRGB16|


	AREA	|.text|, CODE, ARM

|YUV420ToRGB16| PROC
	stmdb		sp!, {r4 - r12, lr}
	sub			sp, sp, #32
	ldr			r3, [r0, #4]			; r3 = _ccOutInWidth 
	str			r3, [sp, #4]			; store _ccOutWidth
	ldr			r12, [r0, #8]			; r12 = _ccOutHeight
	str			r12, [sp, #8]			; store _ccOutHeight
	
	ldr			r8, [r0, #24]			; r8 = **_outBuf
	ldr			r9, [r0, #28]			; r9 = **_inBuf

	ldr			r8,  [r8]				; r8 = &_outBuf[0]
	ldr			r11, [r9]				; r11 = Y = &_inBuf[0]
	ldr			r10, [r9, #4]			; r10 = U = &_inBuf[1]
	ldr			r9,  [r9, #8]			; r9 = V = &_inBuf[2]

	ldr			r12, [r0, #36]			; r12 = outStride(char)

	ldr			r6, [r0, #44]			; r6 = inStrideY(char)
	sub			r4, r6, r3				; r4 = offset(Y)
	str			r4, [sp, #20]
	ldr			r5, [r0, #48]			; r5 = inStrideU(char)
	sub			r4, r5, r3, lsr #1		; r4 = offset(U)
	str			r4, [sp, #24]
	ldr			r5, [r0, #52]			; r5 = inStrideV(char)
	sub			r4, r5, r3, lsr #1		; r4 = offset(V)
	str			r4, [sp, #28]			

L420ToRGB16Height_ARMv4
	str			r3, [sp, #12]
L420ToRGB16Width_ARMv4
	ldrb		r4, [r9], #1				; r4 = V
	mov			r5, #0x66					; r5 = ccWConstV1
	ldr			lr, =0xffffffcc				; lr = ccWConstV2
	sub			r4, r4, #0x80				; r4 = V - 128
	mul			r5, r4, r5					; r5 = a0
	ldrb		r7, [r10], #1				; r7 = U
	mul			lr, r4, lr					; lr = ccWConstV2(V - 128)

	ldr			r0, =0x81					; r0 = ccWConstU1
	ldr			r1, =0xffffffe7				; r1 = ccWConstU2
	sub			r7, r7, #0x80				; r7 = U - 128
	mul			r0, r7, r0					; r0 = ccWConstU1(U - 128) = a2
	ldrb		r4, [r11]					; r4 = Y
	mla			r1, r1, r7, lr				; r1 = ccWConstV2(V - 128) + ccWConstU2(U - 128) = a1
	
	sub			r4, r4, #0x10				; r4 = Y - 16
	ldr			lr, =0x4a					; lr = 	ccWConstY
	mul			r4, r4, lr					; r4 = 	ccWConstY(Y - 16) = a3

	add			lr, r4, r5					; lr = a3 + a0
	mov			lr, lr, asr #9				; lr = (a3 + a0)>>9
	ldrb		lr, [r2, +lr]				; lr = r

	add			r7, r4, r1					; r7 = a3 + a1
	mov			r7, r7, asr #9				; r7 = (a3 + a1)>>9
	ldrb		r7, [r2, +r7]				; r7 = g

	add			r4, r0, r4					; r4 = a3 + a2
	mov			r4, r4, asr #9				; r4 = (a3 + a2)>>9
	ldrb		r4, [r2, +r4]				; r4 = b

	eor			r3, r7, lr, lsl	#5
	eor			r3, r4, r3, lsl #6 

	ldrb		r4, [r11, #1]				; r4 = Y
	ldr			lr, =0x4a					; lr = 	ccWConstY
	sub			r4, r4, #0x10				; r4 = Y - 16

	mul			r4, r4, lr					; r4 = 	ccWConstY(Y - 16) = a3

	add			lr, r4, r5					; lr = a3 + a0
	mov			lr, lr, asr #9				; lr = (a3 + a0)>>9
	ldrb		lr, [r2, +lr]				; lr = r

	add			r7, r4, r1					; r7 = a3 + a1
	mov			r7, r7, asr #9				; r7 = (a3 + a1)>>9
	ldrb		r7, [r2, +r7]				; r7 = g

	add			r4, r0, r4					; r4 = a3 + a2
	mov			r4, r4, asr #9				; r4 = (a3 + a2)>>9
	ldrb		r4, [r2, +r4]				; r4 = b

	eor			lr, r7, lr, lsl	#5
	eor			lr, r4, lr, lsl #6 
	eor			r3, r3, lr, lsl #16
	str			r3, [r8 ]					; store 16 bit rgb	

	add			lr, r11, r6					; next Y line
	ldrb		r4, [lr]					; r4 = Y
	ldr			lr, =0x4a					; lr = 	ccWConstY
	sub			r4, r4, #0x10				; r4 = Y - 16

	mul			r4, r4, lr					; r4 = 	ccWConstY(Y - 16) = a3

	add			lr, r4, r5					; lr = a3 + a0
	mov			lr, lr, asr #9				; lr = (a3 + a0)>>9
	ldrb		lr, [r2, +lr]				; lr = r

	add			r7, r4, r1					; r7 = a3 + a1
	mov			r7, r7, asr #9				; r7 = (a3 + a1)>>gshit
	ldrb		r7, [r2, +r7]				; r7 = g

	add			r4, r0, r4					; r4 = a3 + a2
	mov			r4, r4, asr #9				; r4 = (a3 + a2)>>9
	ldrb		r4, [r2, +r4]				; r4 = b

	eor			r3, r7, lr, lsl	#5
	eor			r3, r4, r3, lsl #6 
	
	add			r7, r8, r12					; r7 = loc - (_ccInWidth<<1)

	add			lr, r11, r6					; next Y line
	ldrb		r4, [lr, #1]				; r4 = Y
	ldr			lr, =0x4a					; lr = 	ccWConstY
	sub			r4, r4, #0x10				; r4 = Y - 16
	mul			r4, r4, lr					; r4 = 	ccWConstY(Y - 16) = a3

	add			lr, r4, r5					; lr = a3 + a0
	mov			lr, lr, asr #9				; lr = (a3 + a0)>>9
	ldrb		lr, [r2, +lr]				; lr = r

	add			r7, r4, r1					; r7 = a3 + a1
	mov			r7, r7, asr #9				; r7 = (a3 + a1)>>9
	ldrb		r7, [r2, +r7]				; r7 = g

	add			r4, r0, r4					; r4 = a3 + a2
	mov			r4, r4, asr #9				; r4 = (a3 + a2)>>9
	ldrb		r4, [r2, +r4]				; r0 = b

	eor			lr, r7, lr, lsl	#5
	eor			lr, r4, lr, lsl #6 
	
	add			r1, r8, r12					; r7 = loc - (_ccInWidth<<1)
	eor			r3, r3, lr, lsl #16
	str			r3, [r1]	 				; store 16 bit rgb	

	add			r11, r11, #2
	ldr			r3, [sp, #12]
	add			r8, r8, #4					; r8 = loc + 4

	subs		r3, r3, #2
	strne		r3, [sp, #12]
	bne			L420ToRGB16Width_ARMv4
	ldr			r3, [sp, #4]				; r3 = inWidth
	sub			r8, r8, r3, lsl #1
	add			r8, r8, r12, lsl #1
	add			r11, r11, r6
	ldr			r4, [sp, #20]
	add			r11, r11, r4
	ldr			r1, [sp, #24]
	add			r10, r10, r1
	ldr			r0, [sp, #28]
	add			r9, r9, r0
	ldr			r5, [sp, #8]
	subs		r5, r5, #2
	strne		r5, [sp, #8]
	bne			L420ToRGB16Height_ARMv4	


	add			sp, sp, #32
	ldmia		sp!, {r4 - r12, pc}


	ENDP  ; |YUV420ToRGB16_ARMv4|


	EXPORT	|YUV420ToRGB16_downsample|


	AREA	|.text|, CODE, ARM

|YUV420ToRGB16_downsample| PROC
	stmdb		sp!, {r4 - r12, lr}
	sub			sp, sp, #32
	ldr			r3, [r0, #16]			; r3 = _ccInWidth 
	str			r3, [sp, #4]			; store _ccInWidth
	ldr			r12, [r0, #20]			; r12 = _ccInHeight
	str			r12, [sp, #8]			; store _ccInHeight
	
	ldr			r8, [r0, #24]			; r8 = **_outBuf
	ldr			r9, [r0, #28]			; r9 = **_inBuf

	ldr			r8,  [r8]				; r8 = &_outBuf[0]
	ldr			r11, [r9]				; r11 = Y = &_inBuf[0]
	ldr			r10, [r9, #4]			; r10 = U = &_inBuf[1]
	ldr			r9,  [r9, #8]			; r9 = V = &_inBuf[2]

	ldr			lr, [r0, #36]			; r1 = outStride(short)
	str			lr, [sp, #12]			; store outStride
	ldr			r1, [r0, #44]			; r1 = inStrideY(char)
	str			r1, [sp, #16]			; strore stride(Y)

	add			r12, r11, r1			; r12 = next Y address

	ldr			r6, [r0, #44]			; r6 = inStrideY(char)
	sub			r4, r6, r3				; r4 = offset(Y)
	str			r4, [sp, #20]
	ldr			r5, [r0, #48]			; r5 = inStrideU(char)
	sub			r4, r5, r3, lsr #1		; r4 = offset(U)
	str			r4, [sp, #24]
	ldr			r5, [r0, #52]			; r5 = inStrideV(char)
	sub			r4, r5, r3, lsr #1		; r4 = offset(V)
	str			r4, [sp, #28]	

	ldr			r6, =0x4a				; r6 = 	ccWConstY
	ldr			lr, =0xffffffe7			; lr = ccWConstU2
	
		
		
L420ToRGB16Height_downsample

L420ToRGB16Width_downsample
	ldrb		r0, [r11], #1				; r0 = Y1
	ldrb		r1, [r11], #1				; r1 = Y2
	ldrb		r4, [r12], #1				; r4 = Y3	
	ldrb		r5, [r12], #1				; r5 = Y4
	add			r0, r0, r1
	add			r0, r0, #2
	add			r4, r4, r5
	ldrb		r5, [r9], #1				; r5 = V
	add			r4, r0, r4
	mov			r4, r4, asr #2				; r4 = Y
	ldrb		r7, [r10], #1				; r7 = U
	sub			r4, r4, #0x10				; r4 = Y - 16
	mul			r4, r4, r6					; r4 = 	ccWConstY(Y - 16) = a3

	mov			r0, #0x66					; r0 = ccWConstV1
	ldr			r1, =0xffffffcc				; r1 = ccWConstV2
	sub			r5, r5, #0x80				; r5 = V - 128
	mul			r0, r5, r0					; r0 = ccWConstV1(V - 128) = a0
	mul			r1, r5, r1					; r1 = ccWConstV2(V - 128)

	ldr			r5, =0x81					; r5 = ccWConstU1
	sub			r7, r7, #0x80				; r7 = U - 128
	mul			r5, r5, r7					; r5 = ccWConstU1(U - 128) = a2
	mla			r1, r7, lr, r1				; r1 = ccWConstV2(V - 128) + ccWConstU2(U - 128) = a1
	
	add			r0, r4, r0					; r0 = a3 + a0
	mov			r0, r0, asr #9				; r0 = (a3 + a0)>>9
	ldrb		r0, [r2, +r0]				; r0 = r

	add			r1, r4, r1					; r1 = a3 + a1
	mov			r1, r1, asr #9				; r1 = (a3 + a1)>>9
	ldrb		r1, [r2, +r1]				; r1 = g

	add			r5, r5, r4					; r5 = a3 + a2
	mov			r5, r5, asr #9				; r5 = (a3 + a2)>>9
	ldrb		r5, [r2, +r5]				; r5 = b

	eor			r0, r1, r0, lsl	#5
	eor			r0, r5, r0, lsl #6 

	strh		r0, [r8], #2				; store 16 bit rgb

	subs		r3, r3, #2
	bne			L420ToRGB16Width_downsample
	ldr			r5, [sp, #8]
	subs		r5, r5, #2
	strne		r5, [sp, #8]

	ldrne		r3, [sp, #4]				; load _ccInWidth
	ldrne		r1, [sp, #12]				; r1 = outStride
	subne		r8, r8, r3					; update _outBuf
	addne		r8, r8, r1
	ldrne		r1, [sp, #16]
	subne		r11, r11, r3
	addne		r11, r11, r1, lsl #1
	addne		r12, r11, r1
	ldrne		r1, [sp, #24]
	addne		r10, r10, r1
	ldrne		r1, [sp, #28]
	addne		r9, r9, r1			

	bne			L420ToRGB16Height_downsample	


	add			sp, sp, #32
	ldmia		sp!, {r4 - r12, pc}


	ENDP  ; |YUV420ToRGB16_downsample|



	EXPORT	|YUV420ToRGB16_rotation_90L|


	AREA	|.text|, CODE, ARM

|YUV420ToRGB16_rotation_90L| PROC
	stmdb		sp!, {r4 - r12, lr}
	sub			sp, sp, #36
	ldr			r3, [r0, #4]			; r3 = _ccOutWidth 
	str			r3, [sp, #4]			; store _ccOutWidth
	ldr			r8, [r0, #24]			; r8 = **_outBuf
	ldr			r9, [r0, #28]			; r9 = **_inBuf
	ldr			r8,  [r8]				; r8 = &_outBuf[0]
	ldr			r11, [r9]				; r11 = Y = &_inBuf[0]
	ldr			r10, [r9, #4]			; r10 = U = &_inBuf[1]
	ldr			r1, [r0, #44]			; r1 = inStrideY(char)
	str			r1, [sp, #32]			; strore stride(Y)

	add			r6, r11, r1				; next line y
	ldrb		r5, [r10], #1			; r5 = U
	ldrb		r12, [r11], #1			; r12 = Y
	ldr			r9,  [r9, #8]			; r9 = V = &_inBuf[2]
	ldrb		r7, [r6], #1			; r7 = Y


	ldr			lr, [r0, #8]			; lr = _ccOutHeight
	str			lr, [sp, #8]			; store _ccOutHeight
	ldr			lr, [r0, #36]			; lr = outStride(char)
	str			lr, [sp, #16]	

	sub			r4, r3, #1
	mul			lr, lr, r4				; lr = outStride  * (_OutInWidth - 1) 
	add			r8, r8, lr	
	str			r8, [sp, #12]			; store the start of __outBuf
	ldrb		r4, [r9], #1			; r4 = V

	sub			r1, r1, r3				; r4 = offset(Y)
	str			r1, [sp, #20]
	ldr			lr, [r0, #48]			; lr = inStrideU(char)
	sub			r1, lr, r3, lsr #1		; r1 = offset(U)
	str			r1, [sp, #24]
	ldr			lr, [r0, #52]			; lr = inStrideV(char)
	sub			r1, lr, r3, lsr #1		; r1 = offset(V)
	str			r1, [sp, #28]	

		
L420ToRGB16Height_rotation_90L

L420ToRGB16Width_rotation_90L
	subs		r3, r3, #2

	ldr			lr, =11769217				; lr = ccWConstV1
	ldr			r1, =-5989466				; r1 = ccWConstV2

	sub			r4, r4, #0x80				; r4 = V - 128
	mul			r0, r4, lr					; r0 = a0
	sub			r5, r5, #0x80				; r5 = V - 128
	ldr			lr, =-2885681				; lr = ccWConstU2
	mul			r4, r4, r1
	ldr			r1, =14847836				; r1 = ccConstU1

	mov			r0, r0, asr #23				; r0 = a0
	mla			lr, r5, lr, r4				; lr = ccWConstU2(U - 128) + ccWConstV2(V - 128)
	add			r4, r12, r0					; r4 = a3 + a0
	mov			r4, r4, asr #3				; lr = (a3 + a0)>>3
	mul			r1, r5, r1					; r1 = a2
	mov			lr, lr, asr #23				; lr = a1
	ldrb		r4, [r2, +r4]				; lr = r
	mov			r1, r1, asr #23				; r1 = a2
	add			r5, r12, lr					; r7 = a3 + a1
	mov			r5, r5, asr #3				; r7 = (a3 + a1)>>3
	add			r12, r12, r1				; r4 = a3 + a2
	ldrb		r5, [r2, +r5]				; r7 = g
	mov			r12, r12, asr #3			; r4 = (a3 + a2)>>3
	eor			r4, r5, r4, lsl	#5
	ldrb		r12, [r2, +r12]				; r4 = b
	add			r5, r7, r1					; r5 = a3 + a2
	mov			r5, r5, asr #3				; r5 = (a3 + a2)>>3
	eor			r4, r12, r4, lsl #6 
	ldrb		r5, [r2, +r5]				; r5 = b
	ldrb		r12, [r11], #1				; r12 = Y
	eor			r4, r4, r5, lsl #16

	add			r5, r7, r0					; r5 = a3 + a0
	mov			r5, r5, asr #3				; l5 = (a3 + a0)>>3
	add			r7, r7, lr					; r7 = a3 + a1
	ldrb		r5, [r2, +r5]				; l5 = r
	mov			r7, r7, asr #3				; r7 = (a3 + a1)>>3
	ldrb		r7, [r2, +r7]				; r7 = g
	eor			r4, r4, r5, lsl #27	
	add			r5, r12, r0					; r5 = a3 + a0			
	eor			r4, r4, r7, lsl #22	
	ldr			r7, [sp, #16]	
	mov			r5, r5, asr #3				; r5 = (a3 + a0)>>3
	str			r4, [r8], -r7
	ldrb		r5, [r2, +r5]				; r5 = r
	
	add			r4, r12, lr					; r7 = a3 + a1
	mov			r4, r4, asr #3				; r7 = (a3 + a1)>>3
	add			r12, r12, r1				; r12 = a3 + a2
	ldrb		r4, [r2, +r4]				; r7 = g
	mov			r12, r12, asr #3			; r12 = (a3 + a2)>>3
	eor			r5, r4, r5, lsl	#5

	ldrb		r4, [r6], #1

	ldrb		r12, [r2, +r12]				; r12 = b
	add			r1, r4, r1					; r1 = a3 + a2
	eor			r5, r12, r5, lsl #6 
	mov			r1, r1, asr #3				; r1 = (a3 + a2)>>3
	ldrneb		r12, [r11], #1				; r12 = Y
	ldrb		r1, [r2, +r1]				; r1 = b
	add			r0, r4, r0					; r0 = a3 + a0		
	eor			r1, r5, r1, lsl #16
	add			lr, r4, lr					; lr = a3 + a1
	ldrb		r5, [r10], #1				; r5 = U
	ldrb		r4, [r9], #1				; r4 = V
	mov			r0, r0, asr #3				; r0 = (a3 + a0)>>3
	ldrb		r0, [r2, +r0]				; r0 = r
	mov			lr, lr, asr #3				; lr = (a3 + a1)>>3
	eor			r0, r1, r0, lsl #27	

	ldrb		lr, [r2, +lr]				; lr = g

	eor			r0, r0, lr, lsl #22		
	str			r0, [r8], -r7

	ldrneb		r7, [r6], #1				; r12 = Y

	bne			L420ToRGB16Width_rotation_90L

	ldr			r1, [sp, #8]
	subs		r1, r1, #2
	strne		r1, [sp, #8]
	ldrne		r3, [sp, #4]
	ldrne		r1, [sp, #20]
	addne		r11, r6, r1
	ldrne		r1, [sp, #32]
	addne		r6, r11, r1
	ldrneb		r12, [r11], #1				; r12 = Y
	ldrneb		r7, [r6], #1				; r7 = Y
	ldrne		r1, [sp, #24]
	addne		r10, r10, r1
	ldrne		r1, [sp, #28]
	addne		r9, r9, r1	
	ldrne		r8, [sp, #12]
	addne		r8, r8, #4	
	strne		r8, [sp, #12]	
 
	bne			L420ToRGB16Height_rotation_90L
	add			sp, sp, #36
	ldmia		sp!, {r4 - r12, pc}


	ENDP  ; |YUV420ToRGB16_rotation_90L|



	EXPORT	|YUV420ToRGB16_rotation_90R|


	AREA	|.text|, CODE, ARM

|YUV420ToRGB16_rotation_90R| PROC
	stmdb		sp!, {r4 - r12, lr}
	sub			sp, sp, #32
	ldr			r3, [r0, #4]			; r3 = _ccOutWidth 
	str			r3, [sp, #4]			; store _ccOutWidth
	ldr			r8, [r0, #24]			; r8 = **_outBuf
	ldr			r9, [r0, #28]			; r9 = **_inBuf
	ldr			r8,  [r8]				; r8 = &_outBuf[0]
	ldr			r6, [r9]				; r11 = Y = &_inBuf[0]
	ldr			r10, [r9, #4]			; r10 = U = &_inBuf[1]
	ldr			r1, [r0, #44]			; r1 = inStrideY(char)
	str			r1, [sp, #32]			; strore stride(Y)

	add			r11, r6, r1				; next line y
	ldrb		r5, [r10], #1			; r5 = U
	ldrb		r12, [r11], #1			; r12 = Y
	ldr			r9,  [r9, #8]			; r9 = V = &_inBuf[2]
	ldrb		r7, [r6], #1			; r7 = Y
	ldrb		r4, [r9], #1			; r4 = V

	ldr			lr, [r0, #36]			; r1 = outStride(char)
	str			lr, [sp, #16]
		
	ldr			lr, [r0, #8]			; r1 = _ccOutHeight
	str			lr, [sp, #8]			; store _ccOutHeight

	mov			lr, lr, lsl #1

	add			r8, r8, lr	
	sub			r8, r8, #4
	str			r8, [sp, #12]			; store the start of __outBuf

	sub			r1, r1, r3				; r4 = offset(Y)
	str			r1, [sp, #20]
	ldr			lr, [r0, #48]			; lr = inStrideU(char)
	sub			r1, lr, r3, lsr #1		; r1 = offset(U)
	str			r1, [sp, #24]
	ldr			lr, [r0, #52]			; lr = inStrideV(char)
	sub			r1, lr, r3, lsr #1		; r1 = offset(V)
	str			r1, [sp, #28]	

		
L420ToRGB16Height_rotation_90R

L420ToRGB16Width_rotation_90R
	subs		r3, r3, #2

	ldr			lr, =11769217				; lr = ccWConstV1
	ldr			r1, =-5989466				; r1 = ccWConstV2

	sub			r4, r4, #0x80				; r4 = V - 128
	mul			r0, r4, lr					; r0 = a0
	sub			r5, r5, #0x80				; r5 = V - 128
	ldr			lr, =-2885681				; lr = ccWConstU2
	mul			r4, r4, r1
	ldr			r1, =14847836				; r1 = ccConstU1

	mov			r0, r0, asr #23				; r0 = a0
	mla			lr, r5, lr, r4				; lr = ccWConstU2(U - 128) + ccWConstV2(V - 128)
	add			r4, r12, r0					; r4 = a3 + a0
	mov			r4, r4, asr #3				; lr = (a3 + a0)>>3
	mul			r1, r5, r1					; r1 = a2
	mov			lr, lr, asr #23				; lr = a1
	ldrb		r4, [r2, +r4]				; lr = r
	mov			r1, r1, asr #23				; r1 = a2
	add			r5, r12, lr					; r7 = a3 + a1
	mov			r5, r5, asr #3				; r7 = (a3 + a1)>>3
	add			r12, r12, r1				; r4 = a3 + a2
	ldrb		r5, [r2, +r5]				; r7 = g
	mov			r12, r12, asr #3			; r4 = (a3 + a2)>>3
	eor			r4, r5, r4, lsl	#5
	ldrb		r12, [r2, +r12]				; r4 = b
	add			r5, r7, r1					; r5 = a3 + a2
	mov			r5, r5, asr #3				; r5 = (a3 + a2)>>3
	eor			r4, r12, r4, lsl #6 
	ldrb		r5, [r2, +r5]				; r5 = b
	ldrb		r12, [r11], #1				; r12 = Y
	eor			r4, r4, r5, lsl #16

	add			r5, r7, r0					; r5 = a3 + a0
	mov			r5, r5, asr #3				; l5 = (a3 + a0)>>3
	add			r7, r7, lr					; r7 = a3 + a1
	ldrb		r5, [r2, +r5]				; l5 = r
	mov			r7, r7, asr #3				; r7 = (a3 + a1)>>3
	ldrb		r7, [r2, +r7]				; r7 = g
	eor			r4, r4, r5, lsl #27	
	add			r5, r12, r0					; r5 = a3 + a0			
	eor			r4, r4, r7, lsl #22	
	ldr			r7, [sp, #16]	
	mov			r5, r5, asr #3				; r5 = (a3 + a0)>>3
	str			r4, [r8], +r7
	ldrb		r5, [r2, +r5]				; r5 = r
	
	add			r4, r12, lr					; r7 = a3 + a1
	mov			r4, r4, asr #3				; r7 = (a3 + a1)>>3
	add			r12, r12, r1				; r12 = a3 + a2
	ldrb		r4, [r2, +r4]				; r7 = g
	mov			r12, r12, asr #3			; r12 = (a3 + a2)>>3
	eor			r5, r4, r5, lsl	#5

	ldrb		r4, [r6], #1

	ldrb		r12, [r2, +r12]				; r12 = b
	add			r1, r4, r1					; r1 = a3 + a2
	eor			r5, r12, r5, lsl #6 
	mov			r1, r1, asr #3				; r1 = (a3 + a2)>>3
	ldrneb		r12, [r11], #1				; r12 = Y
	ldrb		r1, [r2, +r1]				; r1 = b
	add			r0, r4, r0					; r0 = a3 + a0		
	eor			r1, r5, r1, lsl #16
	add			lr, r4, lr					; lr = a3 + a1
	ldrb		r5, [r10], #1				; r5 = U
	ldrb		r4, [r9], #1				; r4 = V
	mov			r0, r0, asr #3				; r0 = (a3 + a0)>>3
	ldrb		r0, [r2, +r0]				; r0 = r
	mov			lr, lr, asr #3				; lr = (a3 + a1)>>3
	eor			r0, r1, r0, lsl #27	

	ldrb		lr, [r2, +lr]				; lr = g

	eor			r0, r0, lr, lsl #22		
	str			r0, [r8], +r7

	ldrneb		r7, [r6], #1				; r12 = Y

	bne			L420ToRGB16Width_rotation_90R

	ldr			r1, [sp, #8]
	subs		r1, r1, #2
	strne		r1, [sp, #8]
	ldrne		r3, [sp, #4]
	ldrne		r1, [sp, #20]
	addne		r6, r11, r1
	ldrne		r1, [sp, #32]
	addne		r11, r6, r1
	ldrneb		r12, [r11], #1				; r12 = Y
	ldrneb		r7, [r6], #1				; r12 = Y
	ldrne		r1, [sp, #24]
	addne		r10, r10, r1
	ldrne		r1, [sp, #28]
	addne		r9, r9, r1
	ldrne		r8, [sp, #12]
	subne		r8, r8, #4	
	strne		r8, [sp, #12]	
 
	bne			L420ToRGB16Height_rotation_90R
	add			sp, sp, #32
	ldmia		sp!, {r4 - r12, pc}



	ENDP  ; |YUV420ToRGB16_rotation_90R|



	EXPORT	|YUVPlanarToRGB16_resize_rotation_up2|
	IMPORT	|ccClip31|

	AREA	|.text|, CODE, ARM

|YUVPlanarToRGB16_resize_rotation_up2| PROC
 
 	;tpVideoType                  outType;		0    
    ;int                          outWidth;		4        
    ;int                          outHeight;    8
    ;int                          outStride;	12
    ;tpVideoType                  inType;		16
    ;int                          inWidth;		20
    ;int                          inHeight;		24
	;int                          inStride;		28
    ;unsigned char                *outBuf[3];	32       
    ;unsigned char                *inBuf[3];	36      
    ;int                          parameter;
	;int                          isResize;
    ;int                          isRotationOut;
	;int                          isInterlaced; /*only support PROGRESSIVE_INPUT for arm version*/


	stmdb		sp!, {r4-r12, lr}		; save regs used
	; load input hieght, width, and buffers; max 4 ldr;
	sub			sp, sp, #28
	ldr			r1, [r0, #20]			; r1 = inWidth
	ldr			r2, [r0, #24]			; r2 = inHeight
	ldr			r3, [r0, #12]			; r3 = outStride
	ldr			r8, [r0, #28]			; r8 = inStride
	ldr			r5, [r0, #32]			; r5 = outBufY
	ldr			r6, [r0, #44]			; r6 = inBufY
	ldr			r4, [r0, #48]			; r4 = inBufU
	ldr			r7, [r0, #52]			; r7 = inBufV

	mov			r9, r1, lsl #1			; inWidth *2
	sub			r9, r9, #1				; inWidth *2 -1
	mul			r9, r9, r3				; (inWidteh *2 - 1)*outStride
	add			r5, r5, r9				; r5 = start_out_buf			
	str			r5, [sp, #4]
	str			r1, [sp, #8]			; [sp, #8] = inWidth
	sub			r9, r8, r1				; r9 = diff of inStride
	str			r9, [sp, #12]
	str			r8, [sp, #16]			; [sp, #16] = inStride
	str			r2, [sp, #20]

	add			r8, r6, r8				; r8 = next line srcY
	ldr			r9, CLIP_DATA
	ldr			r0, [r9]					; r0 = clip tab

LYUVPlanarToRGB16_resize_height_arm
		str			r1, [sp, #24]
		ldrb			r9, [r4], #1		; U	
		ldrb			r10, [r7], #1		; V
		ldrb			r11, [r6], #1		; Y1
		ldrb			r12, [r8], #1		; Y2
LYUVPlanarToRGB16_resize_width_arm
			ldr			r1, [sp, #24]
			subs		r1, r1, #2
			str			r1, [sp, #24]

			ldr			lr, =11769217				; lr = ccWConstV1
			ldr			r1, =-5989466				; r1 = ccWConstV2

			sub			r10, r10, #0x80				; r5 = V - 128
			mul			r2, r10, lr					; r2 = a0
			sub			r9, r9, #0x80				; r4 = U - 128
			mul			r10, r10, r1

			ldr			r1, =14847836				; r1 = ccConstU1	
			ldr			lr, =-2885681				; lr = ccWConstU2

			mul			r1, r9, r1					; r1 = a2	
			mov			r2, r2, asr #23				; r2 = a0
			mla			lr, r9, lr, r10				; lr = ccWConstU2(U - 128) + ccWConstV2(V - 128)
			mov			lr, lr, asr #23				; lr = a1
			mov			r1, r1, asr #23				; r1 = a2
			
			; 0 pixel
			add			r9, r11, r2					; r9 = a3 + a0
			mov			r9, r9, asr #3				; r9 = (a3 + a0)>>3
			ldrb		r9, [r0, +r9]				; r9 = r

			add			r10, r11, lr				; r10 = a3 + a1
			mov			r10, r10, asr #3			; r10 = (a3 + a1)>>3
			ldrb		r10, [r0, +r10]				; r10 = g

			add			r11, r11, r1				; r11 = a3 + a2
			mov			r11, r11, asr #3			; r11 = (a3 + a2)>>3
			ldrb		r11, [r0, +r11]				; r11 = b

			eor			r10, r10, r9, lsl #5
			eor			r10, r11, r10, lsl #6			
			
			mov			r9, r10, lsl #16
			eor			r10, r10, r9
			
			str			r10, [r5]
			str			r10, [r5, -r3]		
			add			r5, r5, #4
			ldrb		r11, [r6], #1

			; 1 pixel
			add			r9, r12, r2					; r9 = a3 + a0
			mov			r9, r9, asr #3				; r9 = (a3 + a0)>>3
			ldrb		r9, [r0, +r9]				; r9 = r

			add			r10, r12, lr				; r10 = a3 + a1
			mov			r10, r10, asr #3			; r10 = (a3 + a1)>>3
			ldrb		r10, [r0, +r10]				; r10 = g

			add			r12, r12, r1				; r12 = a3 + a2
			mov			r12, r12, asr #3			; r12 = (a3 + a2)>>3
			ldrb		r12, [r0, +r12]				; r12 = b

			eor			r10, r10, r9, lsl #5
			eor			r10, r12, r10, lsl #6			
			
			mov			r9, r10, lsl #16
			eor			r10, r10, r9

			str			r10, [r5]
			str			r10, [r5, -r3]		
			sub			r5, r5, #4
			sub			r5, r5, r3, lsl #1

			ldrb		r12, [r8], #1
			; 2 pixel
			add			r9, r11, r2					; r9 = a3 + a0
			mov			r9, r9, asr #3				; r9 = (a3 + a0)>>3
			ldrb		r9, [r0, +r9]				; r9 = r

			add			r10, r11, lr				; r10 = a3 + a1
			mov			r10, r10, asr #3			; r10 = (a3 + a1)>>3
			ldrb		r10, [r0, +r10]				; r10 = g

			add			r11, r11, r1				; r11 = a3 + a2
			mov			r11, r11, asr #3			; r11 = (a3 + a2)>>3
			ldrb		r11, [r0, +r11]				; r11 = b

			eor			r10, r10, r9, lsl #5
			eor			r10, r11, r10, lsl #6			
			
			mov			r9, r10, lsl #16
			eor			r10, r10, r9
			
			str			r10, [r5]
			str			r10, [r5, -r3]		
			add			r5, r5, #4
			ldrneb		r9, [r4], #1		; U	
			ldrneb		r10, [r7], #1		; V
			ldrneb		r11, [r6], #1

			; 3 pixel
			add			r2, r12, r2					; r2 = a3 + a0
			mov			r2, r2, asr #3				; r2 = (a3 + a0)>>3
			ldrb		r2, [r0, +r2]				; r2 = r

			add			lr, r12, lr					; lr = a3 + a1
			mov			lr, lr, asr #3				; lr = (a3 + a1)>>3
			ldrb		lr, [r0, +lr]				; lr = g

			add			r1, r12, r1					; r1 = a3 + a2
			mov			r1, r1, asr #3				; r1 = (a3 + a2)>>3
			ldrb		r1, [r0, +r1]				; r1 = b

			ldrneb		r12, [r8], #1

			eor			lr, lr, r2, lsl #5
			eor			lr, r1, lr, lsl #6			
			
			mov			r1, lr, lsl #16
			eor			r1, r1, lr

			str			r1, [r5]
			str			r1, [r5, -r3]		
			sub			r5, r5, #4
			sub			r5, r5, r3, lsl #1

		
		bne			LYUVPlanarToRGB16_resize_width_arm
		ldr			r2, [sp, #20]
		subs		r2, r2, #2
		strne		r2, [sp, #20]

		ldrne		r5, [sp, #4]
		addne		r5, r5, #8
		strne		r5, [sp, #4]
		ldrne		r1, [sp, #12]		; in_stride diff
		ldrne		lr, [sp, #16]		; in_stride 
		addne		r6, r6, r1
		addne		r6, r6, lr			; Y0
		addne		r8, r6, lr			; Y1
		addne		r4, r4, r1, lsr #1	; U
		addne		r7, r7, r1, lsr #1	; V
		ldrne		r1, [sp, #8]

		bne			LYUVPlanarToRGB16_resize_height_arm
	add			sp, sp, #28
	ldmia		sp!, {r4-r12, pc}		; restore and return
	ENDP  ; |YUVPlanarToRGB16_resize|


|CLIP_DATA|
	DCD       |ccClip31|


	END
