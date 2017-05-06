@************************************************************************
@									                                    *
@	VisualOn, Inc Confidential and Proprietary, 2005		            *
@								 	                                    *
@***********************************************************************/

	@AREA	|text|, CODE
	#include "../../../../code/voMJPEGDecID.h"
	.section .text
	
	.global IDCT_Block8x8_8X8_ARMv6  
	.global IDCT_Block8x8_4X4_ARMv6 
	.global IDCT_Block8x8_2X2_ARMv6 
	.global IDCT_Block8x8_1X1_ARMv6 

	.ALIGN 4
Col8: @PROC

@ r10 = x0
@ r4  = x1
@ r2  = x2
@ r1  = x3
@ r3  = x4
@ r12 = x5
@ r0  = x6
@ r5  = x7
@ r11 = x8  
@ r6  = tmp (x567)

	ldrsh     r10,[r7]
	ldrsh     r3, [r7, #16]
	ldrsh     r1, [r7, #32]
	ldrsh     r5, [r7, #48]

	ldrsh     r4, [r7, #64]
	ldrsh     r0, [r7, #80]
	ldrsh     r2, [r7, #96]
	ldrsh     r12,[r7, #112]

@x5|x4  W1|W7
@x7|x6  W5|W3
@x1|x0
@x2|x3  W2|W6

	pkhbt     r0, r0, r5, lsl #16   @ r0 = x7|x6		 prepare
	pkhbt     r1, r1, r2, lsl #16	@ r1 = x2|x3		 prepare


	orr       r6, r12, r0			@ r6 = x7|x6|x5
	orr       r11, r6, r1			@ r11 = x5|x6|x7|x2|x3
	orrs      r11, r11, r4			@ r11 = x5|x6|x7|x3|x2|x1

	bne       COLLABMB			@ x5|x6|x7|x3|x2|x1!=0
 	cmp       r3, #0				
	bne       COLLABMA			@ x4!=0

	cmp       r10, #0
	beq       COLLABZ			@ x0==0

	mov       r10, r10, lsl #3
	strh      r10, [r7]
	strh      r10, [r7, #0x10]
	strh      r10, [r7, #0x20]
	strh      r10, [r7, #0x30]
	strh      r10, [r7, #0x40]
	strh      r10, [r7, #0x50]
	strh      r10, [r7, #0x60]
	strh      r10, [r7, #0x70]
COLLABZ:
	mov		pc,lr
COLLABMA:							@x0,x4
	mov       r9, r3				
	mov       r2, #0x8D, 30  @ 0x234 = 564
	orr       r2, r2, #1
	mov       r6, r3
	mul       r2, r9, r2
	mov       r9, #0xB1, 28  @ 0xB10 = 2832
	orr       r9, r9, #9
	mul       r4, r6, r9
	mov       r9, #0x96, 28  @ 0x960 = 2400
	orr       r9, r9, #8
	mul       r5, r6, r9
	mov       r9, #0x19, 26  @ 0x640 = 1600
	mov       r1, r10, lsl #11
	orr       r9, r9, #9
	mul       r0, r3, r9
	add       r1, r1, #0x80  @ 0x80 = 128

	add       r3, r4, r1
	add       r9, r5, r1
	mov       r3, r3, asr #8
	mov       r9, r9, asr #8
	strh      r3, [r7]
	strh      r9, [r7, #0x10]  @ 0x10 = 16

	add       r3, r0, r1
	add       r9, r2, r1
	mov       r3, r3, asr #8
	mov       r9, r9, asr #8
	strh      r3, [r7, #0x20]  @ 0x20 = 32
	strh      r9, [r7, #0x30]  @ 0x30 = 48

	sub       r3, r1, r2
	sub       r9, r1, r0
	mov       r3, r3, asr #8
	mov       r9, r9, asr #8
	strh      r3, [r7, #0x40]  @ 0x40 = 64
	strh      r9, [r7, #0x50]  @ 0x50 = 80

	sub       r3, r1, r5
	sub       r9, r1, r4
	mov       r3, r3, asr #8
	mov       r9, r9, asr #8
	strh      r3, [r7, #0x60]  @ 0x60 = 96
	strh      r9, [r7, #0x70]  @ 0x70 = 112
	mov		  pc,lr

COLLABMB:							@x0,x1,x2,x3
	orrs      r9, r6, r3	
	bne       COLLABMC			@ r9 = x5|x6|x7|x4

	mov       r10, r10, lsl #11		@ r10 = x0 << 11
	add       r10, r10, #128		@ r10 = (x0<<11)+128
	add       r3, r10, r4, lsl #11	@ r3 = x0 + x1->x4
	sub       r10, r10, r4, lsl #11	@ r10 = x0 - x1->x0

	ldr		  r6, W26				@ r6 = W26

	smusd	  r2, r6, r1			@ x2 = W6*x3-W2*x2
	smuadx	  r1, r6, r1			@ x3 = W6*x2+W2*x3

	add       r12, r3, r1			@ r12 = x4 + x3->x5
	sub       r3, r3, r1			@ r3 = x4 - x3->x4

	add       r1, r10, r2			@ r1 = x0 + x2->x3
	sub       r10, r10, r2			@ r10 = x0 - x2 ->x0

	mov       r12, r12, asr #8		@ x5
	mov       r3, r3, asr #8		@ x4
	mov       r1, r1, asr #8		@ x3
	mov       r10, r10, asr #8		@ x0

	strh      r12, [r7,#0x00]
	strh      r1, [r7,#0x10]
	strh      r10, [r7,#0x20]
	strh      r3, [r7,#0x30]
	strh      r3, [r7,#0x40] 
	strh      r10, [r7,#0x50] 
	strh      r1, [r7,#0x60] 
	strh      r12, [r7,#0x70] 
	mov		pc,lr

COLLABMC:						@x0,x1,x2,x3,x4,x5,x6,x7
	ldr		r6, W17				@ r6 = W17
	pkhbt   r3, r3, r12, lsl #16 @ r3 = x5|x4
	
	smusd	r12, r6, r3			@ x5 = W7*x4-W1*x5
	smuadx	r3, r6, r3			@ x4 = W7*x5+W1*x4

	ldr		r6, W53				@ r6 = W53	
	smusd	r5, r6, r0			@ x7 = W3*x6-W5*x7
	smuadx	r0, r6, r0			@ x6 = W3*x7+W5*x6		

	mov     r10, r10, lsl #11
	add     r10, r10, #128		@x0 = (x0 << 11) + 128
	add		r11, r10,r4,lsl #11 @x8 = x0 + (x1 << 11)
	sub		r10, r10,r4,lsl #11 @x0 = x0 - (x1 << 11)

	ldr		r6, W26				@ r6 = W26

	smusd	r2, r6, r1			@ x2 = W6*x3-W2*x2
	smuadx	r1, r6, r1			@ x3 = W6*x2+W2*x3

	add		r4, r3, r0			@x1 = x4 + x6
	sub		r3, r3, r0			@x4 -= x6
	add		r0, r12,r5			@x6 = x5 + x7
	sub		r12,r12,r5			@x5 -= x7
	add		r5, r11,r1			@x7 = x8 + x3
	sub		r11,r11,r1			@x8 -= x3
	add		r1, r10,r2			@x3 = x0 + x2
	sub		r10,r10,r2			@x0 -= x2

	add		r6, r3, r12			@x4 + x5
	sub		r3, r3, r12			@x4 - x5
	add		r6, r6, #128
	add		r3, r3, #128
	mov		r6, r6, asr #8
	mov		r3, r3, asr #8
	mov		r12, #181
	mul		r2, r6, r12			@181 * (x4 + x5)
	mul		r3, r3, r12			@181 * (x4 - x5)

	add		r6,r5,r4			
	sub		r5,r5,r4			
	mov		r6,r6,asr #8		@(x7 + x1) >> 8
	mov		r5,r5,asr #8		@(x7 - x1) >> 8
	strh	r6,[r7,#0x00]
	strh	r5,[r7,#0x70]

	add		r6,r1,r2
	sub		r1,r1,r2			
	mov		r6,r6,asr #8		@(x3 + x2) >> 8
	mov		r1,r1,asr #8		@(x3 - x2) >> 8
	strh	r6,[r7,#0x10]
	strh	r1,[r7,#0x60]

	add		r6,r10,r3			
	sub		r10,r10,r3			
	mov		r6,r6,asr #8		@(x0 + x4) >> 8
	mov		r10,r10,asr #8		@(x0 - x4) >> 8
	strh	r6,[r7,#0x20]
	strh	r10,[r7,#0x50]

	add		r6,r11,r0			
	sub		r11,r11,r0			
	mov		r6,r6,asr #8		@(x8 + x6) >> 8
	mov		r11,r11,asr #8		@(x8 - x6) >> 8
	strh	r6,[r7,#0x30]
	strh	r11,[r7,#0x40]

	mov		pc,lr
	@ENDP
	
	.ALIGN 4
Col1: @PROC


	ldrsh     r10,[r7]
	ldrsh     r3, [r7, #16]
	ldrsh     r1, [r7, #32]
	ldrsh     r5, [r7, #48]

	ldrsh     r4, [r7, #64]
	ldrsh     r0, [r7, #80]
	ldrsh     r2, [r7, #96]
	ldrsh     r12,[r7, #112]

	@x5|x4  W1|W7
	@x7|x6  W5|W3
	@x1|x0
	@x2|x3  W2|W6

	pkhbt     r0, r0, r5, lsl #16   @ r0 = x7|x6		 prepare
	pkhbt     r1, r1, r2, lsl #16	@ r1 = x2|x3		 prepare


	orr       r6, r12, r0			@ r6 = x7|x6|x5
	orr       r11, r6, r1			@ r11 = x5|x6|x7|x2|x3
	orrs      r11, r11, r4			@ r11 = x5|x6|x7|x3|x2|x1

	bne       COL1LABMB			@ x5|x6|x7|x3|x2|x1!=0
 	cmp       r3, #0				
	bne       COL1LABMA			@ x4!=0

	cmp       r10, #0
	beq       COL1LABZ			@ x0==0

	mov       r10, r10, lsl #3
	strh      r10, [r7]
COL1LABZ:
	mov		pc,lr
COL1LABMA:							@x0,x4
	mov       r6, r3
	mov       r11, #0xB1, 28  @ 0xB10 = 2832
	orr       r11, r11, #9
	mul       r4, r6, r11
	mov       r1, r10, lsl #11
	add       r1, r1, #0x80  @ 0x80 = 128

	add       r3, r4, r1
	mov       r3, r3, asr #8
	strh      r3, [r7]

	mov		  pc,lr

COL1LABMB:							@x0,x1,x2,x3
	orrs      r11, r6, r3	
	bne       COL1LABMC			@ r11 = x5|x6|x7|x4

	mov       r10, r10, lsl #11		@ r10 = x0 << 11
	add       r10, r10, #128		@ r10 = (x0<<11)+128
	add       r3, r10, r4, lsl #11	@ r3 = x0 + x1->x4

	ldr		  r6, W26				@ r6 = W26

	smuadx	  r1, r6, r1			@ x3 = W6*x2+W2*x3
	add       r12, r3, r1			@ r12 = x4 + x3->x5
	mov       r12, r12, asr #8		@ x5

	strh      r12, [r7,#0x00]

	mov		pc,lr

COL1LABMC:						@x0,x1,x2,x3,x4,x5,x6,x7
	ldr		r6, W17				@ r6 = W17
	pkhbt   r3, r3, r12, lsl #16 @ r3 = x5|x4
	
	smusd	r12, r6, r3			@ x5 = W7*x4-W1*x5
	smuadx	r3, r6, r3			@ x4 = W7*x5+W1*x4

	ldr		r6, W53				@ r6 = W53	
	smuadx	r0, r6, r0			@ x6 = W3*x7+W5*x6		

	mov     r10, r10, lsl #11
	add     r10, r10, #128		@x0 = (x0 << 11) + 128
	add		r11, r10,r4,lsl #11 @x8 = x0 + (x1 << 11)

	ldr		r6, W26				@ r6 = W26
	smuadx	r1, r6, r1			@ x3 = W6*x2+W2*x3
	add		r4, r3, r0			@x1 = x4 + x6
	add		r5, r11,r1			@x7 = x8 + x3

	add		r6,r5,r4					
	mov		r6,r6,asr #8		@(x7 + x1) >> 8
	strh	r6,[r7,#0x00]

	mov		pc,lr
	@ENDP

	.ALIGN 4
Row8: @PROC

@ r10 = x0
@ r4  = x1
@ r2  = x2
@ r1  = x3
@ r3  = x4
@ r12 = x5
@ r0  = x6
@ r5  = x7
@ r11 = x8  
@ r6  = tmp (x567)

	ldrd		r2, [r7]				@ r2 = x4|x0 r3 = x7|x3
	ldrd		r10, [r7, #8]			@ r10 = x6|x1 r11 = x5|x2

	pkhtb		r0, r3, r10, asr #16	@ r0 = x7|x6
	pkhbt		r1, r3, r11, lsl #16	@ r1 = x2|x3

	pkhtb		r3, r11, r2, asr #16	@ r3 = x5|x4
	 
	mov			r4, r10, lsl #16
	mov			r4, r4, asr #16			@ r4 = x1

	mov			r10, r2, lsl #16
	mov			r10, r10, asr #16		@ r10 = x0

	orr			r6, r0, r3				@ r6 = x7|x6|x5|x4
	orr			r6, r6, r1				@ r6 = x7|x6|x5|x4|x3|x2
	orrs		r6, r6, r4				@ r6 = x7|x6|x5|x4|x3|x2|x1
	bne			ROWROWB	
	
	add			r10, r10, #32			@ r10 = x0 + 32
ROWROWA:
	mov			r0 , #255
	usat		r10, #8, r10, asr #6	@ r10 = SAT((x0 + 32)>>6)
	and			r10, r10, r0	
	orr			r10, r10, r10, lsl #8
	orr			r10, r10, r10, lsl #16
	str			r10, [r8]
	str			r10, [r8, #4]
	mov			pc, lr

ROWROWB:

	mov     r10, r10, lsl #11	@ r0 = x0 << 11
	mov		r11, #1, 16			@ r11 = 65536
	add     r10, r10, r11		@ x0 = (x0 << 11) + 65536
	add		r11, r10,r4,lsl #11 @ x8 = x0 + (x1 << 11)
	sub		r10, r10,r4,lsl #11 @ x0 = x0 - (x1 << 11)

	ldr		r6, W17				@ r6 = W17
	
	smusd	r12, r6, r3			@ x5 = W7*x4-W1*x5
	smuadx	r3, r6, r3			@ x4 = W7*x5+W1*x4

	ldr		r6, W53				@ r6 = W53	
	smusd	r5, r6, r0			@ x7 = W3*x6-W5*x7
	smuadx	r0, r6, r0			@ x6 = W3*x7+W5*x6		

	ldr		r6, W26				@ r6 = W26

	smusd	r2, r6, r1			@ x2 = W6*x3-W2*x2
	smuadx	r1, r6, r1			@ x3 = W6*x2+W2*x3

	add		r4, r3, r0			@x1 = x4 + x6
	sub		r3, r3, r0			@x4 -= x6
	add		r0, r12,r5			@x6 = x5 + x7
	sub		r12,r12,r5			@x5 -= x7
	add		r5, r11,r1			@x7 = x8 + x3
	sub		r11,r11,r1			@x8 -= x3
	add		r1, r10,r2			@x3 = x0 + x2
	sub		r10,r10,r2			@x0 -= x2

	add		r6, r3, r12			@x4 + x5
	sub		r3, r3, r12			@x4 - x5
	add		r6, r6, #128
	add		r3, r3, #128
	mov		r6, r6, asr #8
	mov		r3, r3, asr #8
	mov		r12, #181
	mul		r2, r6, r12			@x2 = 181 * ((x4 + x5)>>8)
	mul		r3, r3, r12			@x4 = 181 * ((x4 - x5)>>8)

	add		r12, r5, r4			@(x7 + x1) = blk[0] = r12
	sub		r4, r5, r4			@(x7 - x1) = blk[7] = r4	

	add		r5, r1, r2  		@(x3 + x2) = blk[1] = r5
	sub		r2, r1, r2			@(x3 - x2) = blk[6] = r2
	
	add		r1, r10, r3			@(x0 + x4) = blk[2] = r1
	sub		r3, r10, r3			@(x0 - x4) = blk[5] = r3
	
	add		r10, r11, r0		@(x8 + x6) = blk[3] = r10
	sub		r0, r11, r0			@(x8 - x6) = blk[4] = r0

ROWROWC:	
ROWROWD:
	usat	r12, #8, r12, asr #17		@ r12 = blk[0]
	usat	r5, #8, r5,	asr #17			@ r5 = blk[1]
	usat	r1, #8, r1,	asr #17			@ r1 = blk[2]
	usat	r10, #8, r10, asr #17		@ r10 = blk[3]
	usat	r0, #8, r0,	asr #17			@ r0 = blk[4]
	usat	r3, #8, r3,	asr #17			@ r3 = blk[5]
	usat	r2, #8, r2,	asr #17			@ r2 = blk[6]
	usat	r4, #8, r4,	asr #17			@ r4 = blk[7]
ROWROWF:

	orr		r10, r12, r10, lsl #24
	orr		r10, r10, r5, lsl #8
	orr		r10, r10, r1, lsl #16

	orr		r11, r0, r3, lsl #8
	orr		r11, r11, r2, lsl #16
	orr		r11, r11, r4, lsl #24


	strd	r10, [r8]

	mov		pc,lr
	@ENDP

@	MCol8 Col8, 0, 16
@	MRow8 Row8, 1

@ r0 Block[0]
@ r7 Block
@ r7 Src
@ r8 Dst



@ r7 Block
@ r7 Src
@ r8 Dst

	.ALIGN 4
Row82: @PROC

	ldrd		r2, [r7]				@ r2 = x4|x0 r3 = x7|x3
	ldrd		r10, [r7, #8]			@ r10 = x6|x1 r11 = x5|x2

	pkhtb		r0, r3, r10, asr #16	@ r0 = x7|x6
	pkhbt		r1, r3, r11, lsl #16	@ r1 = x2|x3

	pkhtb		r3, r11, r2, asr #16	@ r3 = x5|x4
	 
	mov			r4, r10, lsl #16
	mov			r4, r4, asr #16			@ r4 = x1

	mov			r10, r2, lsl #16
	mov			r10, r10, asr #16		@ r10 = x0

	orr			r6, r0, r3				@ r6 = x7|x6|x5|x4
	orr			r6, r6, r1				@ r6 = x7|x6|x5|x4|x3|x2
	orrs		r6, r6, r4				@ r6 = x7|x6|x5|x4|x3|x2|x1
	bne			ROW82ROWB	
	
	add			r10, r10, #32			@ r10 = x0 + 32
ROW82ROWA:
	mov			r0 , #255
	usat		r10, #8, r10, asr #6	@ r10 = SAT((x0 + 32)>>6)
	and			r10, r10, r0	
	orr			r10, r10, r10, lsl #8
	orr			r10, r10, r10, lsl #16
	str			r10, [r8]
	mov			pc, lr

ROW82ROWB:

	mov     r10, r10, lsl #11	@ r0 = x0 << 11
	mov		r11, #1, 16			@ r11 = 65536
	add     r10, r10, r11		@ x0 = (x0 << 11) + 65536
	add		r11, r10,r4,lsl #11 @ x8 = x0 + (x1 << 11)
	sub		r10, r10,r4,lsl #11 @ x0 = x0 - (x1 << 11)

	ldr		r6, W17				@ r6 = W17
	
	smusd	r12, r6, r3			@ x5 = W7*x4-W1*x5
	smuadx	r3, r6, r3			@ x4 = W7*x5+W1*x4

	ldr		r6, W53				@ r6 = W53	
	smusd	r5, r6, r0			@ x7 = W3*x6-W5*x7
	smuadx	r0, r6, r0			@ x6 = W3*x7+W5*x6		

	ldr		r6, W26				@ r6 = W26

	smusd	r2, r6, r1			@ x2 = W6*x3-W2*x2
	smuadx	r1, r6, r1			@ x3 = W6*x2+W2*x3

	add		r4, r3, r0			@x1 = x4 + x6
	sub		r3, r3, r0			@x4 -= x6
	add		r0, r12,r5			@x6 = x5 + x7
	sub		r12,r12,r5			@x5 -= x7
	add		r5, r11,r1			@x7 = x8 + x3
	sub		r11,r11,r1			@x8 -= x3
	add		r1, r10,r2			@x3 = x0 + x2
	sub		r10,r10,r2			@x0 -= x2

	add		r6, r3, r12			@x4 + x5
	sub		r3, r3, r12			@x4 - x5
	add		r6, r6, #128
	add		r3, r3, #128
	mov		r6, r6, asr #8
	mov		r3, r3, asr #8
	mov		r12, #181
	mul		r2, r6, r12			@x2 = 181 * ((x4 + x5)>>8)
	mul		r3, r3, r12			@x4 = 181 * ((x4 - x5)>>8)

	add		r12, r5, r4			@(x7 + x1) = blk[0] = r12
	sub		r2, r1, r2			@(x3 - x2) = blk[6] = r2
	
	add		r1, r10, r3			@(x0 + x4) = blk[2] = r1
	sub		r0, r11, r0			@(x8 - x6) = blk[4] = r0

ROW82ROWC:	
ROW82ROWD:
	usat	r12, #8, r12, asr #17		@ r12 = blk[0]
	usat	r1, #8, r1,	asr #17			@ r1  = blk[2]
	usat	r0, #8, r0,	asr #17			@ r0  = blk[4]
	usat	r2, #8, r2,	asr #17			@ r2  = blk[6]
ROW82ROWF:

	orr		r10, r12, r2, lsl #24
	orr		r10, r10, r1, lsl #8
	orr		r10, r10, r0, lsl #16

	str	    r10, [r8]

	mov		pc,lr
	@ENDP
	
		.ALIGN 4
Row84: @PROC

	ldrd		r2, [r7]				@ r2 = x4|x0 r3 = x7|x3
	ldrd		r10, [r7, #8]			@ r10 = x6|x1 r11 = x5|x2

	pkhtb		r0, r3, r10, asr #16	@ r0 = x7|x6
	pkhbt		r1, r3, r11, lsl #16	@ r1 = x2|x3

	pkhtb		r3, r11, r2, asr #16	@ r3 = x5|x4
	 
	mov			r4, r10, lsl #16
	mov			r4, r4, asr #16			@ r4 = x1

	mov			r10, r2, lsl #16
	mov			r10, r10, asr #16		@ r10 = x0

	orr			r6, r0, r3				@ r6 = x7|x6|x5|x4
	orr			r6, r6, r1				@ r6 = x7|x6|x5|x4|x3|x2
	orrs		r6, r6, r4				@ r6 = x7|x6|x5|x4|x3|x2|x1
	bne			ROW84ROWB	
	
	add			r10, r10, #32			@ r10 = x0 + 32
ROW84ROWA:
	mov			r0 , #255
	usat		r10, #8, r10, asr #6	@ r10 = SAT((x0 + 32)>>6)
	and			r10, r10, r0	
	orr			r10, r10, r10, lsl #8
	strh		r10, [r8]
	mov			pc, lr

ROW84ROWB:

	mov     r10, r10, lsl #11	@ r0 = x0 << 11
	mov		r11, #1, 16			@ r11 = 65536
	add     r10, r10, r11		@ x0 = (x0 << 11) + 65536
	add		r11, r10,r4,lsl #11 @ x8 = x0 + (x1 << 11)
	sub		r10, r10,r4,lsl #11 @ x0 = x0 - (x1 << 11)

	ldr		r6, W17				@ r6 = W17
	
	smusd	r12, r6, r3			@ x5 = W7*x4-W1*x5
	smuadx	r3, r6, r3			@ x4 = W7*x5+W1*x4

	ldr		r6, W53				@ r6 = W53	
	smusd	r5, r6, r0			@ x7 = W3*x6-W5*x7
	smuadx	r0, r6, r0			@ x6 = W3*x7+W5*x6		

	ldr		r6, W26				@ r6 = W26

	smuadx	r1, r6, r1			@ x3 = W6*x2+W2*x3

	add		r4, r3, r0			@x1 = x4 + x6
	sub		r3, r3, r0			@x4 -= x6
	add		r0, r12,r5			@x6 = x5 + x7
	sub		r12,r12,r5			@x5 -= x7
	add		r5, r11,r1			@x7 = x8 + x3
	sub		r11,r11,r1			@x8 -= x3

	add		r12, r5, r4			@(x7 + x1) = blk[0] = r12
	sub		r0, r11, r0			@(x8 - x6) = blk[4] = r0

ROW84ROWC:	
ROW84ROWD:
	usat	r12, #8, r12, asr #17		@ r12 = blk[0]
	usat	r0, #8, r0,	asr #17			@ r0  = blk[4]
ROW84ROWF:
	orr		r10, r12, r0, lsl #8
	
	strh	r10, [r8]

	mov		pc,lr
	@ENDP
	
			.ALIGN 4
Row88: @PROC

	ldrd		r2, [r7]				@ r2 = x4|x0 r3 = x7|x3
	ldrd		r10, [r7, #8]			@ r10 = x6|x1 r11 = x5|x2

	pkhtb		r0, r3, r10, asr #16	@ r0 = x7|x6
	pkhbt		r1, r3, r11, lsl #16	@ r1 = x2|x3

	pkhtb		r3, r11, r2, asr #16	@ r3 = x5|x4
	 
	mov			r4, r10, lsl #16
	mov			r4, r4, asr #16			@ r4 = x1

	mov			r10, r2, lsl #16
	mov			r10, r10, asr #16		@ r10 = x0

	orr			r6, r0, r3				@ r6 = x7|x6|x5|x4
	orr			r6, r6, r1				@ r6 = x7|x6|x5|x4|x3|x2
	orrs		r6, r6, r4				@ r6 = x7|x6|x5|x4|x3|x2|x1
	bne			ROW88ROWB	
	
	add			r10, r10, #32			@ r10 = x0 + 32
ROW88ROWA:
	mov			r0 , #255
	usat		r10, #8, r10, asr #6	@ r10 = SAT((x0 + 32)>>6)
	and			r10, r10, r0	
	strb		r10, [r8]
	mov			pc, lr

ROW88ROWB:

	mov     r10, r10, lsl #11	@ r0 = x0 << 11
	mov		r11, #1, 16			@ r11 = 65536
	add     r10, r10, r11		@ x0 = (x0 << 11) + 65536
	add		r11, r10,r4,lsl #11 @ x8 = x0 + (x1 << 11)
	sub		r10, r10,r4,lsl #11 @ x0 = x0 - (x1 << 11)

	ldr		r6, W17				@ r6 = W17
	
	smusd	r12, r6, r3			@ x5 = W7*x4-W1*x5
	smuadx	r3, r6, r3			@ x4 = W7*x5+W1*x4

	ldr		r6, W53				@ r6 = W53	
	smusd	r5, r6, r0			@ x7 = W3*x6-W5*x7
	smuadx	r0, r6, r0			@ x6 = W3*x7+W5*x6		

	ldr		r6, W26				@ r6 = W26

	smuadx	r1, r6, r1			@ x3 = W6*x2+W2*x3

	add		r4, r3, r0			@x1 = x4 + x6
	sub		r12,r12,r5			@x5 -= x7
	add		r5, r11,r1			@x7 = x8 + x3
	add		r12, r5, r4			@(x7 + x1) = blk[0] = r12

ROW88ROWC:	
ROW88ROWD:
	usat	r12, #8, r12, asr #17		@ r12 = blk[0]
ROW88ROWF:	
	strb	r12, [r8]

	mov		pc,lr
	@ENDP

	.ALIGN 4
IDCT_Block8x8_8X8_ARMv6: @PROC

	stmdb   sp!, {r4 - r12, lr}  @ r0=BlockEnd r2=DstStride
	ldr		r6, [sp, #40]		@ lr = src_stride
	sub		sp, sp, #16
	str		r6, [sp, #4]		@ src_stride
	str		r2, [sp, #8]		@ dst_stride
@	mov		r7, r3				@ Src
	mov	    r8, r1				@ Dst
	mov		r7, r0				@ r7 = Block
	

	bl      Col8  
	add     r7, r7, #2
	bl      Col8  
	add     r7, r7, #2
	bl      Col8  
	add     r7, r7, #2
	bl      Col8  
	add     r7, r7, #2
	bl      Col8  
	add     r7, r7, #2
	bl      Col8  
	add     r7, r7, #2
	bl      Col8  
	add     r7, r7, #2
	bl      Col8 
	sub     r7, r7, #14
	
	bl		Row8
	add		r7, r7, #16			@Block += 16
	ldr		r0, [sp, #8]
	add		r8, r8, r0		
	bl		Row8
	add		r7, r7, #16			@Block += 16
	ldr		r0, [sp, #8]
	add		r8, r8, r0	
	bl		Row8
	add		r7, r7, #16			@Block += 16
	ldr		r0, [sp, #8]
	add		r8, r8, r0	
	bl		Row8
	add		r7, r7, #16			@Block += 16
	ldr		r0, [sp, #8]
	add		r8, r8, r0	
	bl		Row8
	add		r7, r7, #16			@Block += 16
	ldr		r0, [sp, #8]
	add		r8, r8, r0	
	bl		Row8
	add		r7, r7, #16			@Block += 16
	ldr		r0, [sp, #8]
	add		r8, r8, r0	
	bl		Row8
	add		r7, r7, #16			@Block += 16
	ldr		r0, [sp, #8]
	add		r8, r8, r0	
	bl		Row8

	add			sp, sp, #16
	ldmia   sp!, {r4 - r12, pc}  
	@ENDP
	
		.ALIGN 4
IDCT_Block8x8_4X4_ARMv6: @PROC

	stmdb   sp!, {r4 - r12, lr}  @ r0=BlockEnd r2=DstStride
	ldr		r6, [sp, #40]		@ lr = src_stride
	sub		sp, sp, #16
	mov     r6, r6, lsl #1
	str		r6, [sp, #4]		@ src_stride
	str		r2, [sp, #8]		@ dst_stride
@	mov		r7, r3				@ Src
	mov	    r8, r1				@ Dst
	mov		r7, r0				@ r7 = Block	

	bl      Col8  
	add     r7, r7, #2
	bl      Col8  
	add     r7, r7, #2
	bl      Col8  
	add     r7, r7, #2
	bl      Col8  
	add     r7, r7, #2
	bl      Col8  
	add     r7, r7, #2
	bl      Col8  
	add     r7, r7, #2
	bl      Col8  
	add     r7, r7, #2
	bl      Col8 
	sub     r7, r7, #14
	
	bl		Row82
	add		r7, r7, #32			@Block += 32
	ldr		r0, [sp, #8]
	add		r8, r8, r0		
	bl		Row82
	add		r7, r7, #32			@Block += 32
	ldr		r0, [sp, #8]
	add		r8, r8, r0	
	bl		Row82
	add		r7, r7, #32			@Block += 32
	ldr		r0, [sp, #8]
	add		r8, r8, r0	
	bl		Row82

	add			sp, sp, #16
	ldmia   sp!, {r4 - r12, pc}  
	@ENDP
	
	.ALIGN 4
IDCT_Block8x8_2X2_ARMv6: @PROC

	stmdb   sp!, {r4 - r12, lr}  @ r0=BlockEnd r2=DstStride
	ldr		r6, [sp, #40]		@ lr = src_stride
	sub		sp, sp, #16
	mov     r6, r6, lsl #2
	str		r6, [sp, #4]		@ src_stride
	str		r2, [sp, #8]		@ dst_stride
@	mov		r7, r3				@ Src
	mov	    r8, r1				@ Dst
	mov		r7, r0				@ r7 = Block	

	bl      Col8  
	add     r7, r7, #2
	bl      Col8  
	add     r7, r7, #2
	bl      Col8  
	add     r7, r7, #2
	bl      Col8  
	add     r7, r7, #2
	bl      Col8  
	add     r7, r7, #2
	bl      Col8  
	add     r7, r7, #2
	bl      Col8  
	add     r7, r7, #2
	bl      Col8 
	sub     r7, r7, #14
	
	bl		Row84
	add		r7, r7, #64			@Block += 64
	ldr		r0, [sp, #8]
	add		r8, r8, r0		
	bl		Row84

	add			sp, sp, #16
	ldmia   sp!, {r4 - r12, pc}  
	@ENDP
	
		.ALIGN 4
IDCT_Block8x8_1X1_ARMv6: @PROC

	stmdb   sp!, {r4 - r12, lr}  @ r0=BlockEnd r2=DstStride
	ldr		r6, [sp, #40]		@ lr = src_stride
	sub		sp, sp, #16
	mov     r6, r6, lsl #3
	str		r6, [sp, #4]		@ src_stride
	str		r2, [sp, #8]		@ dst_stride
@	mov		r7, r3				@ Src
	mov	    r8, r1				@ Dst
	mov		r7, r0				@ r7 = Block	

	bl      Col1  
	add     r7, r7, #2
	bl      Col1  
	add     r7, r7, #2
	bl      Col1  
	add     r7, r7, #2
	bl      Col1  
	add     r7, r7, #2
	bl      Col1  
	add     r7, r7, #2
	bl      Col1  
	add     r7, r7, #2
	bl      Col1  
	add     r7, r7, #2
	bl      Col1 
	sub     r7, r7, #14
	
	bl		Row88

	add			sp, sp, #16
	ldmia   sp!, {r4 - r12, pc}  
	@ENDP



.ALIGN 
W17:		.word 0xb190235
W53:		.word	0x06490968
W26:		.word	0x0a740454


	@END