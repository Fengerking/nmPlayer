@************************************************************************
@									*
@	VisualOn, Inc Confidential and Proprietary, 2005		*
@									*
@***********************************************************************/

	@AREA	|text|, CODE
	.section .text
	
 voMpeg2Reg0     .req r0
 voMpeg2Reg1     .req r1
 voMpeg2Reg2     .req r2
 voMpeg2Reg3     .req r3
 voMpeg2Reg4     .req r4
 voMpeg2Reg5     .req r5
 voMpeg2Reg6     .req r6
 voMpeg2Reg7     .req r7
 voMpeg2Reg8     .req r8
 voMpeg2Reg9     .req r9
 voMpeg2Reg10     .req r10
 voMpeg2Reg11     .req r11
 voMpeg2Reg12     .req r12

	
	@-------------------------------------------------
	@.global ArmIdctD 
	@.global ArmIdctE 
	@.global ArmIdctF
	@--------------------------------------------------
	.global __voMPEG2D0185 
	.global __voMPEG2D0238 
	.global __voMPEG2D0241

	.ALIGN 4
Col8: @PROC

@ voMpeg2Reg10 = x0
@ voMpeg2Reg4  = x1
@ voMpeg2Reg2  = x2
@ voMpeg2Reg1  = x3
@ voMpeg2Reg3  = x4
@ voMpeg2Reg12 = x5
@ voMpeg2Reg0  = x6
@ voMpeg2Reg5  = x7
@ voMpeg2Reg11 = x8  
@ voMpeg2Reg9  = tmp (x567)
	
	ldrsh     voMpeg2Reg5, [voMpeg2Reg6, #48]
	ldrsh     voMpeg2Reg0, [voMpeg2Reg6, #80]
	ldrsh     voMpeg2Reg12,[voMpeg2Reg6, #112]
	ldrsh     voMpeg2Reg2, [voMpeg2Reg6, #96]
	ldrsh     voMpeg2Reg1, [voMpeg2Reg6, #32]	
	ldrsh     voMpeg2Reg4, [voMpeg2Reg6, #64]	
	

@x5|x4  W1|W7
@x7|x6  W5|W3
@x1|x0
@x2|x3  W2|W6

	pkhbt     voMpeg2Reg0, voMpeg2Reg0, voMpeg2Reg5, lsl #16		@ voMpeg2Reg0 = x7|x6		 prepare
	ldrsh     voMpeg2Reg10,[voMpeg2Reg6]
	orr       voMpeg2Reg9, voMpeg2Reg12, voMpeg2Reg0			@ voMpeg2Reg9 = x7|x6|x5
	pkhbt     voMpeg2Reg1, voMpeg2Reg1, voMpeg2Reg2, lsl #16		@ voMpeg2Reg1 = x2|x3		 prepare
	ldrsh     voMpeg2Reg3, [voMpeg2Reg6, #16]	
	orr       voMpeg2Reg11, voMpeg2Reg9, voMpeg2Reg1			@ voMpeg2Reg11 = x5|x6|x7|x2|x3
	orrs      voMpeg2Reg11, voMpeg2Reg11, voMpeg2Reg4			@ voMpeg2Reg11 = x5|x6|x7|x3|x2|x1

	bne       COLLABMB			@ x5|x6|x7|x3|x2|x1!=0
 	cmp       voMpeg2Reg3, #0				
	bne       COLLABMA			@ x4!=0

	cmp       voMpeg2Reg10, #0
	beq       COLLABZ			@ x0==0

	mov       voMpeg2Reg10, voMpeg2Reg10, lsl #3
	strh      voMpeg2Reg10, [voMpeg2Reg6]
	strh      voMpeg2Reg10, [voMpeg2Reg6, #0x10]
	strh      voMpeg2Reg10, [voMpeg2Reg6, #0x20]
	strh      voMpeg2Reg10, [voMpeg2Reg6, #0x30]
	strh      voMpeg2Reg10, [voMpeg2Reg6, #0x40]
	strh      voMpeg2Reg10, [voMpeg2Reg6, #0x50]
	strh      voMpeg2Reg10, [voMpeg2Reg6, #0x60]
	strh      voMpeg2Reg10, [voMpeg2Reg6, #0x70]
COLLABZ:
	mov		pc,lr
COLLABMA:			@x0,x4
	mov       voMpeg2Reg2, #0x8D, 30  @ 0x234 = 564
	mov       voMpeg2Reg11, voMpeg2Reg3				
	orr       voMpeg2Reg2, voMpeg2Reg2, #1
	mov       voMpeg2Reg9, voMpeg2Reg3
	mul       voMpeg2Reg2, voMpeg2Reg11, voMpeg2Reg2
	mov       voMpeg2Reg11, #0xB1, 28  @ 0xB10 = 2832
	orr       voMpeg2Reg11, voMpeg2Reg11, #9
	mul       voMpeg2Reg4, voMpeg2Reg9, voMpeg2Reg11
	mov       voMpeg2Reg11, #0x96, 28  @ 0x960 = 2400
	orr       voMpeg2Reg11, voMpeg2Reg11, #8
	mul       voMpeg2Reg5, voMpeg2Reg9, voMpeg2Reg11
	mov       voMpeg2Reg11, #0x19, 26  @ 0x640 = 1600
	mov       voMpeg2Reg1, voMpeg2Reg10, lsl #11
	orr       voMpeg2Reg11, voMpeg2Reg11, #9
	add       voMpeg2Reg1, voMpeg2Reg1, #0x80  @ 0x80 = 128
	mul       voMpeg2Reg0, voMpeg2Reg3, voMpeg2Reg11

	add       voMpeg2Reg3, voMpeg2Reg4, voMpeg2Reg1
	add       voMpeg2Reg11, voMpeg2Reg5, voMpeg2Reg1
	mov       voMpeg2Reg3, voMpeg2Reg3, asr #8
	mov       voMpeg2Reg11, voMpeg2Reg11, asr #8
	strh      voMpeg2Reg3, [voMpeg2Reg6]
	strh      voMpeg2Reg11, [voMpeg2Reg6, #0x10]  @ 0x10 = 16

	add       voMpeg2Reg3, voMpeg2Reg0, voMpeg2Reg1
	add       voMpeg2Reg11, voMpeg2Reg2, voMpeg2Reg1
	mov       voMpeg2Reg3, voMpeg2Reg3, asr #8
	mov       voMpeg2Reg11, voMpeg2Reg11, asr #8
	strh      voMpeg2Reg3, [voMpeg2Reg6, #0x20]  @ 0x20 = 32
	strh      voMpeg2Reg11, [voMpeg2Reg6, #0x30]  @ 0x30 = 48

	sub       voMpeg2Reg3, voMpeg2Reg1, voMpeg2Reg2
	sub       voMpeg2Reg11, voMpeg2Reg1, voMpeg2Reg0
	mov       voMpeg2Reg3, voMpeg2Reg3, asr #8
	mov       voMpeg2Reg11, voMpeg2Reg11, asr #8
	strh      voMpeg2Reg3, [voMpeg2Reg6, #0x40]  @ 0x40 = 64
	strh      voMpeg2Reg11, [voMpeg2Reg6, #0x50]  @ 0x50 = 80

	sub       voMpeg2Reg3, voMpeg2Reg1, voMpeg2Reg5
	sub       voMpeg2Reg11, voMpeg2Reg1, voMpeg2Reg4
	mov       voMpeg2Reg3, voMpeg2Reg3, asr #8
	mov       voMpeg2Reg11, voMpeg2Reg11, asr #8
	strh      voMpeg2Reg3, [voMpeg2Reg6, #0x60]  @ 0x60 = 96
	strh      voMpeg2Reg11, [voMpeg2Reg6, #0x70]  @ 0x70 = 112
	mov		  pc,lr

COLLABMB:					@x0,x1,x2,x3
	orrs      voMpeg2Reg11, voMpeg2Reg9, voMpeg2Reg3	
	bne       COLLABMC			@ voMpeg2Reg11 = x5|x6|x7|x4

	ldr       voMpeg2Reg9, W26			@ voMpeg2Reg9 = W26

	mov       voMpeg2Reg10, voMpeg2Reg10, lsl #11		@ voMpeg2Reg10 = x0 << 11
	smusd	  voMpeg2Reg2, voMpeg2Reg9, voMpeg2Reg1			@ x2 = W6*x3-W2*x2
	smuadx	  voMpeg2Reg1, voMpeg2Reg9, voMpeg2Reg1			@ x3 = W6*x2+W2*x3
	add       voMpeg2Reg10, voMpeg2Reg10, #128		@ voMpeg2Reg10 = (x0<<11)+128
	add       voMpeg2Reg3, voMpeg2Reg10, voMpeg2Reg4, lsl #11		@ voMpeg2Reg3 = x0 + x1->x4
	sub       voMpeg2Reg10, voMpeg2Reg10, voMpeg2Reg4, lsl #11		@ voMpeg2Reg10 = x0 - x1->x0

	add       voMpeg2Reg12, voMpeg2Reg3, voMpeg2Reg1			@ voMpeg2Reg12 = x4 + x3->x5
	sub       voMpeg2Reg3, voMpeg2Reg3, voMpeg2Reg1			@ voMpeg2Reg3 = x4 - x3->x4

	add       voMpeg2Reg1, voMpeg2Reg10, voMpeg2Reg2			@ voMpeg2Reg1 = x0 + x2->x3
	sub       voMpeg2Reg10, voMpeg2Reg10, voMpeg2Reg2			@ voMpeg2Reg10 = x0 - x2 ->x0

	mov       voMpeg2Reg12, voMpeg2Reg12, asr #8		@ x5
	mov       voMpeg2Reg3, voMpeg2Reg3, asr #8		@ x4
	mov       voMpeg2Reg1, voMpeg2Reg1, asr #8		@ x3
	mov       voMpeg2Reg10, voMpeg2Reg10, asr #8		@ x0

	strh      voMpeg2Reg12, [voMpeg2Reg6,#0x00]
	strh      voMpeg2Reg1, [voMpeg2Reg6,#0x10]
	strh      voMpeg2Reg10, [voMpeg2Reg6,#0x20]
	strh      voMpeg2Reg3, [voMpeg2Reg6,#0x30]
	strh      voMpeg2Reg3, [voMpeg2Reg6,#0x40] 
	strh      voMpeg2Reg10, [voMpeg2Reg6,#0x50] 
	strh      voMpeg2Reg1, [voMpeg2Reg6,#0x60] 
	strh      voMpeg2Reg12, [voMpeg2Reg6,#0x70] 
	mov		pc,lr

COLLABMC:					@x0,x1,x2,x3,x4,x5,x6,x7
	ldr	voMpeg2Reg9, W17				@ voMpeg2Reg9 = W17
	pkhbt   voMpeg2Reg3, voMpeg2Reg3, voMpeg2Reg12, lsl #16 		@ voMpeg2Reg3 = x5|x4
	
	smusd	voMpeg2Reg12, voMpeg2Reg9, voMpeg2Reg3			@ x5 = W7*x4-W1*x5
	smuadx	voMpeg2Reg3, voMpeg2Reg9, voMpeg2Reg3			@ x4 = W7*x5+W1*x4

	ldr	voMpeg2Reg9, W53				@ voMpeg2Reg9 = W53
	mov     voMpeg2Reg10, voMpeg2Reg10, lsl #11
	smusd	voMpeg2Reg5, voMpeg2Reg9, voMpeg2Reg0			@ x7 = W3*x6-W5*x7
	add     voMpeg2Reg10, voMpeg2Reg10, #128			@x0 = (x0 << 11) + 128
	smuadx	voMpeg2Reg0, voMpeg2Reg9, voMpeg2Reg0			@ x6 = W3*x7+W5*x6		

	
	add		voMpeg2Reg11, voMpeg2Reg10,voMpeg2Reg4,lsl #11 @x8 = x0 + (x1 << 11)
	sub		voMpeg2Reg10, voMpeg2Reg10,voMpeg2Reg4,lsl #11 @x0 = x0 - (x1 << 11)

	ldr		voMpeg2Reg9, W26				@ voMpeg2Reg9 = W26

	add		voMpeg2Reg4, voMpeg2Reg3, voMpeg2Reg0			@x1 = x4 + x6
	smusd		voMpeg2Reg2, voMpeg2Reg9, voMpeg2Reg1			@ x2 = W6*x3-W2*x2
	sub		voMpeg2Reg3, voMpeg2Reg3, voMpeg2Reg0			@x4 -= x6
	add		voMpeg2Reg0, voMpeg2Reg12,voMpeg2Reg5			@x6 = x5 + x7
	smuadx		voMpeg2Reg1, voMpeg2Reg9, voMpeg2Reg1			@ x3 = W6*x2+W2*x3
		
	sub		voMpeg2Reg12,voMpeg2Reg12,voMpeg2Reg5			@x5 -= x7
	add		voMpeg2Reg5, voMpeg2Reg11,voMpeg2Reg1			@x7 = x8 + x3
	sub		voMpeg2Reg11,voMpeg2Reg11,voMpeg2Reg1			@x8 -= x3
	add		voMpeg2Reg1, voMpeg2Reg10,voMpeg2Reg2			@x3 = x0 + x2
	sub		voMpeg2Reg10,voMpeg2Reg10,voMpeg2Reg2			@x0 -= x2

	add		voMpeg2Reg9, voMpeg2Reg3, voMpeg2Reg12			@x4 + x5
	sub		voMpeg2Reg3, voMpeg2Reg3, voMpeg2Reg12			@x4 - x5
	add		voMpeg2Reg9, voMpeg2Reg9, #128
	add		voMpeg2Reg3, voMpeg2Reg3, #128
	mov		voMpeg2Reg12, #181
	mov		voMpeg2Reg9, voMpeg2Reg9, asr #8
	mov		voMpeg2Reg3, voMpeg2Reg3, asr #8	
	mul		voMpeg2Reg2, voMpeg2Reg9, voMpeg2Reg12			@181 * (x4 + x5)
	mul		voMpeg2Reg3, voMpeg2Reg3, voMpeg2Reg12			@181 * (x4 - x5)

	add		voMpeg2Reg9,voMpeg2Reg5,voMpeg2Reg4			
	sub		voMpeg2Reg5,voMpeg2Reg5,voMpeg2Reg4			
	mov		voMpeg2Reg9,voMpeg2Reg9,asr #8		@(x7 + x1) >> 8
	mov		voMpeg2Reg5,voMpeg2Reg5,asr #8		@(x7 - x1) >> 8
	strh		voMpeg2Reg9,[voMpeg2Reg6,#0x00]
	strh		voMpeg2Reg5,[voMpeg2Reg6,#0x70]

	add		voMpeg2Reg9,voMpeg2Reg1,voMpeg2Reg2
	sub		voMpeg2Reg1,voMpeg2Reg1,voMpeg2Reg2			
	mov		voMpeg2Reg9,voMpeg2Reg9,asr #8		@(x3 + x2) >> 8
	mov		voMpeg2Reg1,voMpeg2Reg1,asr #8		@(x3 - x2) >> 8
	strh		voMpeg2Reg9,[voMpeg2Reg6,#0x10]
	strh		voMpeg2Reg1,[voMpeg2Reg6,#0x60]

	add		voMpeg2Reg9,voMpeg2Reg10,voMpeg2Reg3			
	sub		voMpeg2Reg10,voMpeg2Reg10,voMpeg2Reg3			
	mov		voMpeg2Reg9,voMpeg2Reg9,asr #8		@(x0 + x4) >> 8
	mov		voMpeg2Reg10,voMpeg2Reg10,asr #8		@(x0 - x4) >> 8
	strh		voMpeg2Reg9,[voMpeg2Reg6,#0x20]
	strh		voMpeg2Reg10,[voMpeg2Reg6,#0x50]

	add		voMpeg2Reg9,voMpeg2Reg11,voMpeg2Reg0			
	sub		voMpeg2Reg11,voMpeg2Reg11,voMpeg2Reg0			
	mov		voMpeg2Reg9,voMpeg2Reg9,asr #8		@(x8 + x6) >> 8
	mov		voMpeg2Reg11,voMpeg2Reg11,asr #8		@(x8 - x6) >> 8
	strh		voMpeg2Reg9,[voMpeg2Reg6,#0x30]
	strh		voMpeg2Reg11,[voMpeg2Reg6,#0x40]

	mov		pc,lr
	@ENDP
	
	.ALIGN 4
Row8_half: @PROC

	ldrd		voMpeg2Reg2, [voMpeg2Reg6]		@ voMpeg2Reg2 = d1|d0 voMpeg2Reg3 = d3|d2

	pkhtb		voMpeg2Reg0, voMpeg2Reg3, voMpeg2Reg2, asr #16	@ voMpeg2Reg0 = d3|d1
	 
	mov  		voMpeg2Reg4, voMpeg2Reg3, lsl #16
	mov  		voMpeg2Reg10, voMpeg2Reg2, lsl #16
	mov  		voMpeg2Reg4, voMpeg2Reg4, asr #16		@ voMpeg2Reg4 = d2
	mov  		voMpeg2Reg10, voMpeg2Reg10, asr #16	@ voMpeg2Reg10 = d0
	orrs		voMpeg2Reg9, voMpeg2Reg0, voMpeg2Reg4		@ voMpeg2Reg9 = d3|d2|d1
	
	bne		ROWROWHALB	
	
	add		voMpeg2Reg10, voMpeg2Reg10, #32		@ voMpeg2Reg10 = d0 + 32
	cmp		voMpeg2Reg7, #0
	beq		ROWROWHALA
	pld		[voMpeg2Reg7]
	mov		voMpeg2Reg10, voMpeg2Reg10, asr #6	@ voMpeg2Reg10 = (d0 + 32)>>6
	mov		voMpeg2Reg10, voMpeg2Reg10, lsl #17	@ for asr when add dst[]
	mov		voMpeg2Reg2, voMpeg2Reg10	
	mov		voMpeg2Reg5, voMpeg2Reg10	
	mov		voMpeg2Reg4, voMpeg2Reg10
	mov		voMpeg2Reg12,voMpeg2Reg10
	b		ROWROWHALC
ROWROWHALA:
	mov  		voMpeg2Reg0 , #255
	usat		voMpeg2Reg10, #8, voMpeg2Reg10, asr #6	@ voMpeg2Reg10 = SAT((d0 + 32)>>6)
	and 		voMpeg2Reg10, voMpeg2Reg10, voMpeg2Reg0	
	orr  		voMpeg2Reg10, voMpeg2Reg10, voMpeg2Reg10, lsl #8
	orr  		voMpeg2Reg10, voMpeg2Reg10, voMpeg2Reg10, lsl #16
	str  		voMpeg2Reg10, [voMpeg2Reg8]
	mov  		pc, lr

ROWROWHALB:

	ldr		voMpeg2Reg9, W13			@ voMpeg2Reg9 = W13
	mov  		voMpeg2Reg10, voMpeg2Reg10, lsl #11	@ voMpeg2Reg10 = d0 << 11
	mov		voMpeg2Reg11, #1, 16		@ voMpeg2Reg11 = 65536
	add  		voMpeg2Reg10, voMpeg2Reg10, voMpeg2Reg11		@ d0 = (d0 << 11) + 65536
	
	smuadx		voMpeg2Reg11, voMpeg2Reg9, voMpeg2Reg0		@ d1 = w3*d3+w1*d1
	smusd		voMpeg2Reg3, voMpeg2Reg9, voMpeg2Reg0		@ d3 = w3*d1-w1*d3
	
	
	add		voMpeg2Reg12, voMpeg2Reg10,voMpeg2Reg4,lsl #11	@ d4 = d0 + (d2 << 11)
	sub		voMpeg2Reg10, voMpeg2Reg10,voMpeg2Reg4,lsl #11	@ d0 = d0 - (d2 << 11)


	add		voMpeg2Reg4, voMpeg2Reg12, voMpeg2Reg11		@ blk[0] = voMpeg2Reg4
	sub		voMpeg2Reg12, voMpeg2Reg12, voMpeg2Reg11		@ blk[3] = voMpeg2Reg12

	add		voMpeg2Reg5, voMpeg2Reg10, voMpeg2Reg3		@ blk[1] = voMpeg2Reg5
	sub		voMpeg2Reg2, voMpeg2Reg10, voMpeg2Reg3		@ blk[2] = voMpeg2Reg2

ROWROWHALC:	
	cmp		voMpeg2Reg7, #0
	beq		ROWROWHALD

	ldrb		voMpeg2Reg9, [voMpeg2Reg7]
	ldrb		voMpeg2Reg11, [voMpeg2Reg7, #1]
	
	add		voMpeg2Reg4, voMpeg2Reg9, voMpeg2Reg4, asr #17
	ldrb		voMpeg2Reg9, [voMpeg2Reg7, #2]
	add		voMpeg2Reg5, voMpeg2Reg11, voMpeg2Reg5, asr #17
	ldrb		voMpeg2Reg11, [voMpeg2Reg7, #3]
	add		voMpeg2Reg2, voMpeg2Reg9, voMpeg2Reg2, asr #17
	add		voMpeg2Reg12, voMpeg2Reg11, voMpeg2Reg12, asr #17
	ldr		voMpeg2Reg11, [sp, #4]
	add		voMpeg2Reg7, voMpeg2Reg7, voMpeg2Reg11			@ src += src_stride

	usat		voMpeg2Reg4, #8, voMpeg2Reg4			@ voMpeg2Reg4 = blk[0]
	usat		voMpeg2Reg5, #8, voMpeg2Reg5			@ voMpeg2Reg5 = blk[1]
	usat		voMpeg2Reg2, #8, voMpeg2Reg2			@ voMpeg2Reg2 = blk[2]
	usat		voMpeg2Reg12, #8, voMpeg2Reg12			@ voMpeg2Reg12 = blk[3]
	b		ROWROWHALF
ROWROWHALD:
	usat		voMpeg2Reg4, #8, voMpeg2Reg4, asr #17		@ voMpeg2Reg4 = blk[0]
	usat		voMpeg2Reg5, #8, voMpeg2Reg5, asr #17		@ voMpeg2Reg5 = blk[1]
	usat		voMpeg2Reg2, #8, voMpeg2Reg2, asr #17		@ voMpeg2Reg2 = blk[2]
	usat		voMpeg2Reg12, #8, voMpeg2Reg12, asr #17		@ voMpeg2Reg12 = blk[3]
ROWROWHALF:

	orr		voMpeg2Reg10, voMpeg2Reg4, voMpeg2Reg12, lsl #24
	orr		voMpeg2Reg10, voMpeg2Reg10, voMpeg2Reg5, lsl #8
	orr		voMpeg2Reg10, voMpeg2Reg10, voMpeg2Reg2, lsl #16



	str		voMpeg2Reg10, [voMpeg2Reg8]

	mov		pc,lr
	@ENDP

	.ALIGN 4
Row8: @PROC

@ voMpeg2Reg10 = x0
@ voMpeg2Reg4  = x1
@ voMpeg2Reg2  = x2
@ voMpeg2Reg1  = x3
@ voMpeg2Reg3  = x4
@ voMpeg2Reg12 = x5
@ voMpeg2Reg0  = x6
@ voMpeg2Reg5  = x7
@ voMpeg2Reg11 = x8  
@ voMpeg2Reg9  = tmp (x567)

	ldrd			voMpeg2Reg2, [voMpeg2Reg6]			@ voMpeg2Reg2 = x4|x0 voMpeg2Reg3 = x7|x3
	ldrd			voMpeg2Reg10, [voMpeg2Reg6, #8]			@ voMpeg2Reg10 = x6|x1 voMpeg2Reg11 = x5|x2

	pkhtb			voMpeg2Reg0, voMpeg2Reg3, voMpeg2Reg10, asr #16	@ voMpeg2Reg0 = x7|x6
	pkhbt			voMpeg2Reg1, voMpeg2Reg3, voMpeg2Reg11, lsl #16	@ voMpeg2Reg1 = x2|x3
	mov			voMpeg2Reg4, voMpeg2Reg10, lsl #16
	pkhtb			voMpeg2Reg3, voMpeg2Reg11, voMpeg2Reg2, asr #16	@ voMpeg2Reg3 = x5|x4
	mov			voMpeg2Reg4, voMpeg2Reg4, asr #16		@ voMpeg2Reg4 = x1

	mov			voMpeg2Reg10, voMpeg2Reg2, lsl #16
	orr			voMpeg2Reg9, voMpeg2Reg0, voMpeg2Reg3			@ voMpeg2Reg9 = x7|x6|x5|x4
	mov			voMpeg2Reg10, voMpeg2Reg10, asr #16		@ voMpeg2Reg10 = x0

	orr			voMpeg2Reg9, voMpeg2Reg9, voMpeg2Reg1			@ voMpeg2Reg9 = x7|x6|x5|x4|x3|x2
	orrs			voMpeg2Reg9, voMpeg2Reg9, voMpeg2Reg4			@ voMpeg2Reg9 = x7|x6|x5|x4|x3|x2|x1
	bne			ROWROWB	
	
	add			voMpeg2Reg10, voMpeg2Reg10, #32			@ voMpeg2Reg10 = x0 + 32
	cmp			voMpeg2Reg7, #0
	beq			ROWROWA
	
	mov			voMpeg2Reg10, voMpeg2Reg10, asr #6			@ voMpeg2Reg10 = (x0 + 32)>>6
	pld			[voMpeg2Reg7]
	mov			voMpeg2Reg10, voMpeg2Reg10, lsl #17		@ for asr when add dst[]
	mov			voMpeg2Reg4, voMpeg2Reg10
	mov			voMpeg2Reg2, voMpeg2Reg10
	mov			voMpeg2Reg1, voMpeg2Reg10	
	mov			voMpeg2Reg3, voMpeg2Reg10	
	mov			voMpeg2Reg12, voMpeg2Reg10	
	mov			voMpeg2Reg0, voMpeg2Reg10	
	mov			voMpeg2Reg5, voMpeg2Reg10		
	b	ROWROWC
ROWROWA:
	mov			voMpeg2Reg0 , #255
	usat			voMpeg2Reg10, #8, voMpeg2Reg10, asr #6	@ voMpeg2Reg10 = SAT((x0 + 32)>>6)
	and			voMpeg2Reg10, voMpeg2Reg10, voMpeg2Reg0	
	orr			voMpeg2Reg10, voMpeg2Reg10, voMpeg2Reg10, lsl #8
	orr			voMpeg2Reg10, voMpeg2Reg10, voMpeg2Reg10, lsl #16
	str			voMpeg2Reg10, [voMpeg2Reg8]
	str			voMpeg2Reg10, [voMpeg2Reg8, #4]
	mov			pc, lr

ROWROWB:

	ldr	voMpeg2Reg9, W17				@ voMpeg2Reg9 = W17
	mov     voMpeg2Reg10, voMpeg2Reg10, lsl #11	@ voMpeg2Reg0 = x0 << 11
	smusd	voMpeg2Reg12, voMpeg2Reg9, voMpeg2Reg3			@ x5 = W7*x4-W1*x5
	mov	voMpeg2Reg11, #1, 16			@ voMpeg2Reg11 = 65536
	smuadx	voMpeg2Reg3, voMpeg2Reg9, voMpeg2Reg3			@ x4 = W7*x5+W1*x4
	add     voMpeg2Reg10, voMpeg2Reg10, voMpeg2Reg11		@ x0 = (x0 << 11) + 65536
	ldr	voMpeg2Reg9, W53				@ voMpeg2Reg9 = W53	
	add	voMpeg2Reg11, voMpeg2Reg10,voMpeg2Reg4,lsl #11 @ x8 = x0 + (x1 << 11)
	smusd	voMpeg2Reg5, voMpeg2Reg9, voMpeg2Reg0			@ x7 = W3*x6-W5*x7
	smuadx	voMpeg2Reg0, voMpeg2Reg9, voMpeg2Reg0			@ x6 = W3*x7+W5*x6
	ldr	voMpeg2Reg9, W26				@ voMpeg2Reg9 = W26
	sub	voMpeg2Reg10, voMpeg2Reg10,voMpeg2Reg4,lsl #11 @ x0 = x0 - (x1 << 11)			
			
	smusd	voMpeg2Reg2, voMpeg2Reg9, voMpeg2Reg1			@ x2 = W6*x3-W2*x2
	smuadx	voMpeg2Reg1, voMpeg2Reg9, voMpeg2Reg1			@ x3 = W6*x2+W2*x3

	add		voMpeg2Reg4, voMpeg2Reg3, voMpeg2Reg0			@x1 = x4 + x6
	sub		voMpeg2Reg3, voMpeg2Reg3, voMpeg2Reg0			@x4 -= x6
	add		voMpeg2Reg0, voMpeg2Reg12,voMpeg2Reg5			@x6 = x5 + x7
	sub		voMpeg2Reg12,voMpeg2Reg12,voMpeg2Reg5			@x5 -= x7
	add		voMpeg2Reg5, voMpeg2Reg11,voMpeg2Reg1			@x7 = x8 + x3
	sub		voMpeg2Reg11,voMpeg2Reg11,voMpeg2Reg1			@x8 -= x3
	add		voMpeg2Reg1, voMpeg2Reg10,voMpeg2Reg2			@x3 = x0 + x2
	sub		voMpeg2Reg10,voMpeg2Reg10,voMpeg2Reg2			@x0 -= x2

	add		voMpeg2Reg9, voMpeg2Reg3, voMpeg2Reg12			@x4 + x5
	sub		voMpeg2Reg3, voMpeg2Reg3, voMpeg2Reg12			@x4 - x5
	add		voMpeg2Reg9, voMpeg2Reg9, #128
	add		voMpeg2Reg3, voMpeg2Reg3, #128
	mov		voMpeg2Reg9, voMpeg2Reg9, asr #8
	mov		voMpeg2Reg3, voMpeg2Reg3, asr #8
	mov		voMpeg2Reg12, #181
	mul		voMpeg2Reg2, voMpeg2Reg9, voMpeg2Reg12			@x2 = 181 * ((x4 + x5)>>8)
	mul		voMpeg2Reg3, voMpeg2Reg3, voMpeg2Reg12			@x4 = 181 * ((x4 - x5)>>8)

	add		voMpeg2Reg12, voMpeg2Reg5, voMpeg2Reg4			@(x7 + x1) = blk[0] = voMpeg2Reg12
	sub		voMpeg2Reg4, voMpeg2Reg5, voMpeg2Reg4			@(x7 - x1) = blk[7] = voMpeg2Reg4	

	add		voMpeg2Reg5, voMpeg2Reg1, voMpeg2Reg2  		@(x3 + x2) = blk[1] = voMpeg2Reg5
	sub		voMpeg2Reg2, voMpeg2Reg1, voMpeg2Reg2			@(x3 - x2) = blk[6] = voMpeg2Reg2
	
	add		voMpeg2Reg1, voMpeg2Reg10, voMpeg2Reg3			@(x0 + x4) = blk[2] = voMpeg2Reg1
	sub		voMpeg2Reg3, voMpeg2Reg10, voMpeg2Reg3			@(x0 - x4) = blk[5] = voMpeg2Reg3
	
	add		voMpeg2Reg10, voMpeg2Reg11, voMpeg2Reg0		@(x8 + x6) = blk[3] = voMpeg2Reg10
	sub		voMpeg2Reg0, voMpeg2Reg11, voMpeg2Reg0			@(x8 - x6) = blk[4] = voMpeg2Reg0

ROWROWC	:
	cmp		voMpeg2Reg7, #0
	beq		ROWROWD

	ldrb	voMpeg2Reg9, [voMpeg2Reg7]
	ldrb	voMpeg2Reg11, [voMpeg2Reg7, #1]
	
	add		voMpeg2Reg12, voMpeg2Reg9, voMpeg2Reg12, asr #17
	ldrb	voMpeg2Reg9, [voMpeg2Reg7, #2]
	add		voMpeg2Reg5, voMpeg2Reg11, voMpeg2Reg5, asr #17
	ldrb	voMpeg2Reg11, [voMpeg2Reg7, #3]
	add		voMpeg2Reg1, voMpeg2Reg9, voMpeg2Reg1, asr #17
	ldrb	voMpeg2Reg9, [voMpeg2Reg7, #4]
	add		voMpeg2Reg10, voMpeg2Reg11, voMpeg2Reg10, asr #17
	ldrb	voMpeg2Reg11, [voMpeg2Reg7, #5]
	add		voMpeg2Reg0, voMpeg2Reg9, voMpeg2Reg0, asr #17
	ldrb	voMpeg2Reg9, [voMpeg2Reg7, #6]
	add		voMpeg2Reg3, voMpeg2Reg11, voMpeg2Reg3, asr #17
	ldrb	voMpeg2Reg11, [voMpeg2Reg7, #7]
	add		voMpeg2Reg2, voMpeg2Reg9, voMpeg2Reg2, asr #17
	add		voMpeg2Reg4, voMpeg2Reg11, voMpeg2Reg4, asr #17
	ldr		voMpeg2Reg11, [sp, #4]
	add		voMpeg2Reg7, voMpeg2Reg7, voMpeg2Reg11				@ src += src_stride

	usat	voMpeg2Reg12, #8, voMpeg2Reg12			@ voMpeg2Reg12 = blk[0]
	usat	voMpeg2Reg5, #8, voMpeg2Reg5				@ voMpeg2Reg5 = blk[1]
	usat	voMpeg2Reg1, #8, voMpeg2Reg1				@ voMpeg2Reg1 = blk[2]
	usat	voMpeg2Reg10, #8, voMpeg2Reg10			@ voMpeg2Reg10 = blk[3]
	usat	voMpeg2Reg0, #8, voMpeg2Reg0				@ voMpeg2Reg0 = blk[4]
	usat	voMpeg2Reg3, #8, voMpeg2Reg3				@ voMpeg2Reg3 = blk[5]
	usat	voMpeg2Reg2, #8, voMpeg2Reg2				@ voMpeg2Reg2 = blk[6]
	usat	voMpeg2Reg4, #8, voMpeg2Reg4				@ voMpeg2Reg4 = blk[7]
	b		ROWROWF
ROWROWD:
	usat	voMpeg2Reg12, #8, voMpeg2Reg12, asr #17		@ voMpeg2Reg12 = blk[0]
	usat	voMpeg2Reg5, #8, voMpeg2Reg5,	asr #17			@ voMpeg2Reg5 = blk[1]
	usat	voMpeg2Reg1, #8, voMpeg2Reg1,	asr #17			@ voMpeg2Reg1 = blk[2]
	usat	voMpeg2Reg10, #8, voMpeg2Reg10, asr #17		@ voMpeg2Reg10 = blk[3]
	usat	voMpeg2Reg0, #8, voMpeg2Reg0,	asr #17			@ voMpeg2Reg0 = blk[4]
	usat	voMpeg2Reg3, #8, voMpeg2Reg3,	asr #17			@ voMpeg2Reg3 = blk[5]
	usat	voMpeg2Reg2, #8, voMpeg2Reg2,	asr #17			@ voMpeg2Reg2 = blk[6]
	usat	voMpeg2Reg4, #8, voMpeg2Reg4,	asr #17			@ voMpeg2Reg4 = blk[7]
ROWROWF:

	orr		voMpeg2Reg10, voMpeg2Reg12, voMpeg2Reg10, lsl #24
	orr		voMpeg2Reg10, voMpeg2Reg10, voMpeg2Reg5, lsl #8
	orr		voMpeg2Reg10, voMpeg2Reg10, voMpeg2Reg1, lsl #16

	orr		voMpeg2Reg11, voMpeg2Reg0, voMpeg2Reg3, lsl #8
	orr		voMpeg2Reg11, voMpeg2Reg11, voMpeg2Reg2, lsl #16
	orr		voMpeg2Reg11, voMpeg2Reg11, voMpeg2Reg4, lsl #24


	strd	voMpeg2Reg10, [voMpeg2Reg8]

	mov		pc,lr
	@ENDP

	.ALIGN 4
@ArmIdctF @PROC
__voMPEG2D0241: @PROC
	stmdb   sp!, {voMpeg2Reg4 - voMpeg2Reg12, lr}  @ voMpeg2Reg0=BlockEnd voMpeg2Reg2=DstStride
	ldr		voMpeg2Reg9, [sp, #40]		@ lr = src_stride
	sub		sp, sp, #16
	str		voMpeg2Reg9, [sp, #4]		@ src_stride
	str		voMpeg2Reg2, [sp, #8]		@ dst_stride
	mov		voMpeg2Reg7, voMpeg2Reg3				@ Src
	mov	    voMpeg2Reg8, voMpeg2Reg1				@ Dst
	mov		voMpeg2Reg6, voMpeg2Reg0				@ voMpeg2Reg6 = Block
	

	bl      Col8  
	add     voMpeg2Reg6, voMpeg2Reg6, #2
	bl      Col8  
	add     voMpeg2Reg6, voMpeg2Reg6, #2
	bl      Col8  
	add     voMpeg2Reg6, voMpeg2Reg6, #2
	bl      Col8  
	sub     voMpeg2Reg6, voMpeg2Reg6, #6
	
	bl		Row8
	add		voMpeg2Reg6, voMpeg2Reg6, #32			@Block += 32
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0			
	bl		Row8
	add		voMpeg2Reg6, voMpeg2Reg6, #32			@Block += 32
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0	
	bl		Row8
	add		voMpeg2Reg6, voMpeg2Reg6, #32			@Block += 32
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0	
	bl		Row8

	add			sp, sp, #16
	ldmia   sp!, {voMpeg2Reg4 - voMpeg2Reg12, pc} 

	@ENDP

@ voMpeg2Reg6 Block
@ voMpeg2Reg7 Src
@ voMpeg2Reg8 Dst
@ lr DstStride

	.ALIGN 4
@ArmIdctE @PROC
__voMPEG2D0238: @PROC

	stmdb		sp!, {voMpeg2Reg4 - voMpeg2Reg12, lr}	@ voMpeg2Reg0=BlockEnd voMpeg2Reg2=DstStride
	ldr		voMpeg2Reg9, [sp, #40]		@ lr = src_stride
	sub		sp, sp, #16
	str		voMpeg2Reg9, [sp, #4]		@ src_stride
	str		voMpeg2Reg2, [sp, #8]		@ dst_stride
	mov		voMpeg2Reg7, voMpeg2Reg3			@ Src
	mov		voMpeg2Reg8, voMpeg2Reg1			@ Dst
	mov		voMpeg2Reg6, voMpeg2Reg0			@ voMpeg2Reg6 = Block
	

	bl		Col8  
	add		voMpeg2Reg6, voMpeg2Reg6, #2
	bl		Col8  
	add		voMpeg2Reg6, voMpeg2Reg6, #2
	bl		Col8  
	add		voMpeg2Reg6, voMpeg2Reg6, #2
	bl		Col8  
	add		voMpeg2Reg6, voMpeg2Reg6, #2
	bl		Col8  
	add		voMpeg2Reg6, voMpeg2Reg6, #2
	bl		Col8  
	add		voMpeg2Reg6, voMpeg2Reg6, #2
	bl		Col8  
	add		voMpeg2Reg6, voMpeg2Reg6, #2
	bl		Col8 
	sub		voMpeg2Reg6, voMpeg2Reg6, #14
	
	bl		Row8
	add		voMpeg2Reg6, voMpeg2Reg6, #32			@Block += 32
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0			
	bl		Row8
	add		voMpeg2Reg6, voMpeg2Reg6, #32			@Block += 32
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0	
	bl		Row8
	add		voMpeg2Reg6, voMpeg2Reg6, #32			@Block += 32
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0	
	bl		Row8

	add			sp, sp, #16
	ldmia   sp!, {voMpeg2Reg4 - voMpeg2Reg12, pc}  
	@ENDP

	.ALIGN 4
@ArmIdctD @PROC
__voMPEG2D0185: @PROC

	stmdb   sp!, {voMpeg2Reg4 - voMpeg2Reg12, lr}		@ voMpeg2Reg0=BlockEnd voMpeg2Reg2=DstStride
	ldr		voMpeg2Reg9, [sp, #40]		@ lr = src_stride
	sub		sp, sp, #16
	str		voMpeg2Reg9, [sp, #4]		@ src_stride
	str		voMpeg2Reg2, [sp, #8]		@ dst_stride
	mov		voMpeg2Reg7, voMpeg2Reg3			@ Src
	mov		voMpeg2Reg8, voMpeg2Reg1			@ Dst
	mov		voMpeg2Reg6, voMpeg2Reg0			@ voMpeg2Reg6 = Block
	

	bl		Col8  
	add		voMpeg2Reg6, voMpeg2Reg6, #2
	bl		Col8  
	add		voMpeg2Reg6, voMpeg2Reg6, #2
	bl		Col8  
	add		voMpeg2Reg6, voMpeg2Reg6, #2
	bl		Col8  
	sub		voMpeg2Reg6, voMpeg2Reg6, #6
	
	bl		Row8_half
	add		voMpeg2Reg6, voMpeg2Reg6, #16			@Block += 16
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0		
	bl		Row8_half
	add		voMpeg2Reg6, voMpeg2Reg6, #16			@Block += 16
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0	
	bl		Row8_half
	add		voMpeg2Reg6, voMpeg2Reg6, #16			@Block += 16
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0	
	bl		Row8_half
	add		voMpeg2Reg6, voMpeg2Reg6, #16			@Block += 16
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0	
	bl		Row8_half
	add		voMpeg2Reg6, voMpeg2Reg6, #16			@Block += 16
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0	
	bl		Row8_half
	add		voMpeg2Reg6, voMpeg2Reg6, #16			@Block += 16
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0	
	bl		Row8_half
	add		voMpeg2Reg6, voMpeg2Reg6, #16			@Block += 16
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0	
	bl		Row8_half

	add			sp, sp, #16
	ldmia   sp!, {voMpeg2Reg4 - voMpeg2Reg12, pc}  
	@ENDP


.ALIGN 
W17:		.word 0xb190235
W53:		.word	0x06490968
W26:		.word	0x0a740454
W13:		.word	0x0a740454

	@END