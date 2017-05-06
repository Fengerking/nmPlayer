@************************************************************************
@									*
@	VisualOn, Inc Confidential and Proprietary, 2011		*
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
 voMpeg2Reg13     .req r13
 voMpeg2Reg14     .req r14
 voMpeg2Reg15     .req r15

	@-------------------------------------------------
	.global Bit16ArmIdctA
	.global Bit16ArmIdctB
	.global Bit16ArmIdctC
	.global Bit16ArmIdctG  @huwei 20090904 IDCT4x4
	@--------------------------------------------------

	.ALIGN 4
Bit16ArmIdctC: @PROC
    stmfd   voMpeg2Reg13!, {voMpeg2Reg4, voMpeg2Reg5, voMpeg2Reg6, voMpeg2Reg7, voMpeg2Reg8, voMpeg2Reg9, voMpeg2Reg10, voMpeg2Reg11, voMpeg2Reg14}
	ldr		voMpeg2Reg14,[sp,#36]	@SrcPitch
	mov     voMpeg2Reg2, voMpeg2Reg2, lsl #1
	mov     voMpeg2Reg4, #8
	mov     voMpeg2Reg5, voMpeg2Reg0, lsl #16
	mov     voMpeg2Reg0, voMpeg2Reg5, lsr #16
	orr     voMpeg2Reg0, voMpeg2Reg0, voMpeg2Reg5  
IRDETO_ARMV6_CLOOP:
	str     voMpeg2Reg0, [voMpeg2Reg1, #12] 
	str     voMpeg2Reg0, [voMpeg2Reg1, #8] 
	str     voMpeg2Reg0, [voMpeg2Reg1, #4]
	str     voMpeg2Reg0, [voMpeg2Reg1], voMpeg2Reg2
	subs    voMpeg2Reg4, voMpeg2Reg4, #1
	bgt     IRDETO_ARMV6_CLOOP

    ldmfd   voMpeg2Reg13!, {voMpeg2Reg4, voMpeg2Reg5, voMpeg2Reg6, voMpeg2Reg7, voMpeg2Reg8, voMpeg2Reg9, voMpeg2Reg10, voMpeg2Reg11, voMpeg2Reg15} @retrun end arm_transc8x8   
    
	.ALIGN 4
IRCol8: @PROC
	ldrsh     voMpeg2Reg5, [voMpeg2Reg6, #48]
	ldrsh     voMpeg2Reg0, [voMpeg2Reg6, #80]
	ldrsh     voMpeg2Reg12,[voMpeg2Reg6, #112]
	ldrsh     voMpeg2Reg2, [voMpeg2Reg6, #96]
	ldrsh     voMpeg2Reg1, [voMpeg2Reg6, #32]	
	ldrsh     voMpeg2Reg4, [voMpeg2Reg6, #64]	

	pkhbt     voMpeg2Reg0, voMpeg2Reg0, voMpeg2Reg5, lsl #16		@ voMpeg2Reg0 = x7|x6		 prepare
	ldrsh     voMpeg2Reg10,[voMpeg2Reg6]
	orr       voMpeg2Reg9, voMpeg2Reg12, voMpeg2Reg0			@ voMpeg2Reg9 = x7|x6|x5
	pkhbt     voMpeg2Reg1, voMpeg2Reg1, voMpeg2Reg2, lsl #16		@ voMpeg2Reg1 = x2|x3		 prepare
	ldrsh     voMpeg2Reg3, [voMpeg2Reg6, #16]	
	orr       voMpeg2Reg11, voMpeg2Reg9, voMpeg2Reg1			@ voMpeg2Reg11 = x5|x6|x7|x2|x3
	orrs      voMpeg2Reg11, voMpeg2Reg11, voMpeg2Reg4			@ voMpeg2Reg11 = x5|x6|x7|x3|x2|x1

	bne       IRCOLLABMB			@ x5|x6|x7|x3|x2|x1!=0
 	cmp       voMpeg2Reg3, #0				
	bne       IRCOLLABMA			@ x4!=0

	cmp       voMpeg2Reg10, #0
	beq       IRCOLLABZ			@ x0==0

	mov       voMpeg2Reg10, voMpeg2Reg10, lsl #3
	strh      voMpeg2Reg10, [voMpeg2Reg6]
	strh      voMpeg2Reg10, [voMpeg2Reg6, #0x10]
	strh      voMpeg2Reg10, [voMpeg2Reg6, #0x20]
	strh      voMpeg2Reg10, [voMpeg2Reg6, #0x30]
	strh      voMpeg2Reg10, [voMpeg2Reg6, #0x40]
	strh      voMpeg2Reg10, [voMpeg2Reg6, #0x50]
	strh      voMpeg2Reg10, [voMpeg2Reg6, #0x60]
	strh      voMpeg2Reg10, [voMpeg2Reg6, #0x70]
IRCOLLABZ:
	mov		pc,lr
IRCOLLABMA:			@x0,x4
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

IRCOLLABMB:					@x0,x1,x2,x3
	orrs      voMpeg2Reg11, voMpeg2Reg9, voMpeg2Reg3	
	bne       IRCOLLABMC			@ voMpeg2Reg11 = x5|x6|x7|x4

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
	mov		  pc,lr

IRCOLLABMC:					@x0,x1,x2,x3,x4,x5,x6,x7
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
	strh	voMpeg2Reg9,[voMpeg2Reg6,#0x00]
	strh	voMpeg2Reg5,[voMpeg2Reg6,#0x70]

	add		voMpeg2Reg9,voMpeg2Reg1,voMpeg2Reg2
	sub		voMpeg2Reg1,voMpeg2Reg1,voMpeg2Reg2			
	mov		voMpeg2Reg9,voMpeg2Reg9,asr #8		@(x3 + x2) >> 8
	mov		voMpeg2Reg1,voMpeg2Reg1,asr #8		@(x3 - x2) >> 8
	strh	voMpeg2Reg9,[voMpeg2Reg6,#0x10]
	strh	voMpeg2Reg1,[voMpeg2Reg6,#0x60]

	add		voMpeg2Reg9,voMpeg2Reg10,voMpeg2Reg3			
	sub		voMpeg2Reg10,voMpeg2Reg10,voMpeg2Reg3			
	mov		voMpeg2Reg9,voMpeg2Reg9,asr #8		@(x0 + x4) >> 8
	mov		voMpeg2Reg10,voMpeg2Reg10,asr #8		@(x0 - x4) >> 8
	strh	voMpeg2Reg9,[voMpeg2Reg6,#0x20]
	strh	voMpeg2Reg10,[voMpeg2Reg6,#0x50]

	add		voMpeg2Reg9,voMpeg2Reg11,voMpeg2Reg0			
	sub		voMpeg2Reg11,voMpeg2Reg11,voMpeg2Reg0			
	mov		voMpeg2Reg9,voMpeg2Reg9,asr #8		@(x8 + x6) >> 8
	mov		voMpeg2Reg11,voMpeg2Reg11,asr #8		@(x8 - x6) >> 8
	strh	voMpeg2Reg9,[voMpeg2Reg6,#0x30]
	strh	voMpeg2Reg11,[voMpeg2Reg6,#0x40]

	mov		pc,lr
	@ENDP
	
	.ALIGN 4
IRCol8_4x4: @PROC
	ldrsh	voMpeg2Reg1, [voMpeg2Reg0]
	ldrsh	voMpeg2Reg2, [voMpeg2Reg0, #16]
	ldrsh	voMpeg2Reg3, [voMpeg2Reg0, #32]
	ldrsh	voMpeg2Reg4, [voMpeg2Reg0, #48]
	
	orrs	voMpeg2Reg7, voMpeg2Reg3, voMpeg2Reg4		@ voMpeg2Reg7 = ip2|ip3
	bne	ir_allx1_4_t_lab		@ (ip2|ip3)!=0	
	
 	cmp	voMpeg2Reg2, #0				
	bne	ir_only_x1x2t_lab_4x4		@ x2!=0
	
	cmp	voMpeg2Reg1, #0
	bne	ir_only_x1t_lab_4x4		@ x1!=0
	mov	pc, lr
	
ir_only_x1t_lab_4x4: 
    mov	voMpeg2Reg5, voMpeg2Reg1, lsl #3
	strh	voMpeg2Reg5, [voMpeg2Reg0]
	strh	voMpeg2Reg5, [voMpeg2Reg0, #16]
	strh	voMpeg2Reg5, [voMpeg2Reg0, #32]
	strh	voMpeg2Reg5, [voMpeg2Reg0, #48]
	strh	voMpeg2Reg5, [voMpeg2Reg0, #64]
	strh	voMpeg2Reg5, [voMpeg2Reg0, #80]
	strh	voMpeg2Reg5, [voMpeg2Reg0, #96]
	strh	voMpeg2Reg5, [voMpeg2Reg0, #112]
	mov	pc, lr	
ir_only_x1x2t_lab_4x4:
	smulbb	voMpeg2Reg5, voMpeg2Reg2, voMpeg2Reg10	@A = M(xC1S7, ip[1*8])@
	smulbb	voMpeg2Reg6, voMpeg2Reg2, voMpeg2Reg11	@B = M(xC7S1, ip[1*8])@	
	mov	voMpeg2Reg7, voMpeg2Reg1, lsl #11	@E = Blk[0*8] << 11
	add	voMpeg2Reg7, voMpeg2Reg7, #128	@E += 128@
	sub	voMpeg2Reg3, voMpeg2Reg5, voMpeg2Reg6
	add	voMpeg2Reg4, voMpeg2Reg5, voMpeg2Reg6
	mov	voMpeg2Reg3, voMpeg2Reg3, asr #8	@(A - B)>> 8
	mov	voMpeg2Reg4, voMpeg2Reg4, asr #8	@(A + B)>> 8
	smulbb	voMpeg2Reg3, voMpeg2Reg12, voMpeg2Reg3	@Add = 181 * ((A - B)>> 8)@
	smulbb	voMpeg2Reg4, voMpeg2Reg12, voMpeg2Reg4	@Bdd = 181 * ((A + B)>> 8)@
	
	add	voMpeg2Reg1, voMpeg2Reg7, voMpeg2Reg5	@E + A
	sub	voMpeg2Reg2, voMpeg2Reg7, voMpeg2Reg5	@E - A
	add	voMpeg2Reg8, voMpeg2Reg7, voMpeg2Reg4	@E + Bdd
	sub	voMpeg2Reg4, voMpeg2Reg7, voMpeg2Reg4	@E - Bdd	
	add	voMpeg2Reg5, voMpeg2Reg7, voMpeg2Reg6	@E + B
	sub	voMpeg2Reg6, voMpeg2Reg7, voMpeg2Reg6	@E - B	
	add	voMpeg2Reg9, voMpeg2Reg7, voMpeg2Reg3	@E + Add
	sub	voMpeg2Reg3, voMpeg2Reg7, voMpeg2Reg3	@E - Add
	
	mov	voMpeg2Reg1, voMpeg2Reg1, asr #8	@
	mov	voMpeg2Reg2, voMpeg2Reg2, asr #8	@
	mov	voMpeg2Reg8, voMpeg2Reg8, asr #8	@
	mov	voMpeg2Reg4, voMpeg2Reg4, asr #8	@
	mov	voMpeg2Reg5, voMpeg2Reg5, asr #8	@
	mov	voMpeg2Reg6, voMpeg2Reg6, asr #8	@
	mov	voMpeg2Reg9, voMpeg2Reg9, asr #8	@
	mov	voMpeg2Reg3, voMpeg2Reg3, asr #8	@
	
	strh	voMpeg2Reg1, [voMpeg2Reg0]
	strh	voMpeg2Reg8, [voMpeg2Reg0, #16]
	strh	voMpeg2Reg9, [voMpeg2Reg0, #32]
	strh	voMpeg2Reg5, [voMpeg2Reg0, #48]
	strh	voMpeg2Reg6, [voMpeg2Reg0, #64]
	strh	voMpeg2Reg3, [voMpeg2Reg0, #80]
	strh	voMpeg2Reg4, [voMpeg2Reg0, #96]
	strh	voMpeg2Reg2, [voMpeg2Reg0, #112]	
	mov	pc, lr	
ir_allx1_4_t_lab:            			
	ldr	voMpeg2Reg9, WxC2xC6	@voMpeg2Reg10 = WxC2xC6			
	pkhbt	voMpeg2Reg2, voMpeg2Reg2, voMpeg2Reg4, lsl #16	@ voMpeg2Reg2 = ip3|ip1
					@ voMpeg2Reg3 = ip2	voMpeg2Reg1 = ip0		
	smuad	voMpeg2Reg8, voMpeg2Reg2, voMpeg2Reg10	@@Cd = A + C  M(xC1S7, ip[1*8]) + M(xC3S5, ip[3*8])
	smusd	voMpeg2Reg4, voMpeg2Reg2, voMpeg2Reg11	@@Dd = B + D  M(xC7S1, ip[1*8]) + (-M(xC5S3, ip[3*8]))
	
	smusd	voMpeg2Reg5, voMpeg2Reg2, voMpeg2Reg10	@@Ad = A - C  M(xC1S7, ip[1*8]) - M(xC3S5, ip[3*8])
	smuad	voMpeg2Reg6, voMpeg2Reg2, voMpeg2Reg11	@@Bd = B - D  M(xC7S1, ip[1*8]) - (-M(xC5S3, ip[3*8]))	
	
	sub	voMpeg2Reg7, voMpeg2Reg5, voMpeg2Reg6
	add	voMpeg2Reg6, voMpeg2Reg5, voMpeg2Reg6
	mov	voMpeg2Reg5, voMpeg2Reg7, asr #8	@(Ad - Bd)) >> 8
	mov	voMpeg2Reg6, voMpeg2Reg6, asr #8	@(Ad + Bd)) >> 8
	smulbb	voMpeg2Reg5, voMpeg2Reg12, voMpeg2Reg5	@Add = (181 * (((Ad - Bd)) >> 8))@
	smulbb	voMpeg2Reg6, voMpeg2Reg12, voMpeg2Reg6	@Bdd = (181 * (((Ad + Bd)) >> 8))@	
		
	mov	voMpeg2Reg1, voMpeg2Reg1, asl #11
	add	voMpeg2Reg1, voMpeg2Reg1, #128	@E
	
	smulbt	voMpeg2Reg7, voMpeg2Reg3, voMpeg2Reg9	@G = W2 * Blk[2*8]
	smulbb	voMpeg2Reg2, voMpeg2Reg3, voMpeg2Reg9	@H = W6 * Blk[2*8]@	
			
@ voMpeg2Reg3  free now
	sub	voMpeg2Reg3, voMpeg2Reg1, voMpeg2Reg7	@Ed = E - G@
	add	voMpeg2Reg7, voMpeg2Reg1, voMpeg2Reg7	@Fd = E + G@
	sub	voMpeg2Reg9, voMpeg2Reg1, voMpeg2Reg2	@Gd = E - H@
	add	voMpeg2Reg1, voMpeg2Reg1, voMpeg2Reg2	@Hd = E + H@
	
@ voMpeg2Reg1 free now
	add	voMpeg2Reg2, voMpeg2Reg7, voMpeg2Reg8	@Fd + Cd	0
	sub	voMpeg2Reg7, voMpeg2Reg7, voMpeg2Reg8	@Fd - Cd	7
	add	voMpeg2Reg8, voMpeg2Reg1, voMpeg2Reg6	@Hd + Bdd	1
	sub	voMpeg2Reg1, voMpeg2Reg1, voMpeg2Reg6	@Hd - Bdd	6	
	add	voMpeg2Reg6, voMpeg2Reg3, voMpeg2Reg4	@Ed + Dd	3
	sub	voMpeg2Reg3, voMpeg2Reg3, voMpeg2Reg4	@Ed - Dd	4	
	add	voMpeg2Reg4, voMpeg2Reg9, voMpeg2Reg5	@Gd + Add	2
	sub	voMpeg2Reg5, voMpeg2Reg9, voMpeg2Reg5	@Gd - Add	5
		
	mov	voMpeg2Reg2, voMpeg2Reg2, asr #8	@
	mov	voMpeg2Reg8, voMpeg2Reg8, asr #8	@
	mov	voMpeg2Reg4, voMpeg2Reg4, asr #8	@	
	mov	voMpeg2Reg6, voMpeg2Reg6, asr #8	@
	mov	voMpeg2Reg3, voMpeg2Reg3, asr #8	@
	mov	voMpeg2Reg5, voMpeg2Reg5, asr #8	@
	mov	voMpeg2Reg1, voMpeg2Reg1, asr #8	@
	mov	voMpeg2Reg7, voMpeg2Reg7, asr #8	@

	strh	voMpeg2Reg2, [voMpeg2Reg0]
	strh	voMpeg2Reg8, [voMpeg2Reg0, #16]
	strh	voMpeg2Reg4, [voMpeg2Reg0, #32]
	strh	voMpeg2Reg6, [voMpeg2Reg0, #48]
	strh	voMpeg2Reg3, [voMpeg2Reg0, #64]
	strh	voMpeg2Reg5, [voMpeg2Reg0, #80]
	strh	voMpeg2Reg1, [voMpeg2Reg0, #96]
	strh	voMpeg2Reg7, [voMpeg2Reg0, #112]	
	mov		pc,lr
	@ENDP
	
	.ALIGN 4
IRRow8nosrc_4x4: @PROC
	ldrd		voMpeg2Reg2, [voMpeg2Reg0]	@ voMpeg2Reg2 = x1|x0 voMpeg2Reg3 = x3|x2
	cmp	        voMpeg2Reg3, #0
	bne	ir_allx1_4_t_lab_Rownosrc	@ (ip2|ip3|ip4|ip5| ip6|ip7)!=0	
	
 	movs	voMpeg2Reg3, voMpeg2Reg2, lsr #16		@ voMpeg2Reg3 = x1	

	bne	ir_only_x1x2t_lab_Rownosrc_4x4	@ x2!=0
	ldr	voMpeg2Reg7, [sp, #12]		@ dst = [sp, #12]
	ldr	voMpeg2Reg4, [sp, #8]		@ dst_stride = [sp, #8]
	add	voMpeg2Reg4, voMpeg2Reg7, voMpeg2Reg4	
	str	voMpeg2Reg4, [sp, #12]		@ dst = [sp, #12]		
	cmp	voMpeg2Reg2, #0
	bne	ir_only_x1t_lab_Rownosrc_4x4	@ x1!=0
	mov	voMpeg2Reg2, #0
	str	voMpeg2Reg2, [voMpeg2Reg7]
	str voMpeg2Reg2, [voMpeg2Reg7, #4]
	str voMpeg2Reg2, [voMpeg2Reg7, #8]
	str voMpeg2Reg2, [voMpeg2Reg7, #12]		
	mov	pc, lr
	
ir_only_x1t_lab_Rownosrc_4x4:
	mov	    voMpeg2Reg4, #32
	sxtah	voMpeg2Reg1, voMpeg2Reg4, voMpeg2Reg2		@Blk[0] + 32
	mov		voMpeg2Reg1, voMpeg2Reg1, asr #6		    @ voMpeg2Reg10 = SAT((x0 + 32)>>6)
	mov     voMpeg2Reg4, voMpeg2Reg1, lsl #16
	mov     voMpeg2Reg1, voMpeg2Reg4, lsr #16
	orr     voMpeg2Reg2, voMpeg2Reg1,  voMpeg2Reg4 
	str		voMpeg2Reg2, [voMpeg2Reg7]
	str     voMpeg2Reg2, [voMpeg2Reg7, #4]
	str     voMpeg2Reg2, [voMpeg2Reg7, #8]
	str     voMpeg2Reg2, [voMpeg2Reg7, #12]						
	mov	pc, lr	
	
ir_only_x1x2t_lab_Rownosrc_4x4:
	mov	voMpeg2Reg7, #32
	smultb	voMpeg2Reg5, voMpeg2Reg2, voMpeg2Reg10	@A = M(xC1S7, ip[1*8])@
	smultb	voMpeg2Reg6, voMpeg2Reg2, voMpeg2Reg11	@B = M(xC7S1, ip[1*8])@	
	sxtah	voMpeg2Reg7, voMpeg2Reg7, voMpeg2Reg2	@Blk[0] + 32
	mov	voMpeg2Reg7, voMpeg2Reg7, asl #11	@E = (Blk[0] + 32) << 11	
	
	sub	voMpeg2Reg3, voMpeg2Reg5, voMpeg2Reg6
	add	voMpeg2Reg4, voMpeg2Reg5, voMpeg2Reg6
	mov	voMpeg2Reg3, voMpeg2Reg3, asr #8	@(A - B)>> 8
	mov	voMpeg2Reg4, voMpeg2Reg4, asr #8	@(A + B)>> 8
	mul	voMpeg2Reg3, voMpeg2Reg12, voMpeg2Reg3	@Add = 181 * ((A - B)>> 8)@
	mul	voMpeg2Reg4, voMpeg2Reg12, voMpeg2Reg4	@Bdd = 181 * ((A + B)>> 8)@
	
	add	voMpeg2Reg1, voMpeg2Reg7, voMpeg2Reg5	@E + A
	sub	voMpeg2Reg2, voMpeg2Reg7, voMpeg2Reg5	@E - A
	add	voMpeg2Reg8, voMpeg2Reg7, voMpeg2Reg4	@E + Bdd
	sub	voMpeg2Reg4, voMpeg2Reg7, voMpeg2Reg4	@E - Bdd	
	add	voMpeg2Reg5, voMpeg2Reg7, voMpeg2Reg6	@E + B
	sub	voMpeg2Reg6, voMpeg2Reg7, voMpeg2Reg6	@E - B	
	add	voMpeg2Reg9, voMpeg2Reg7, voMpeg2Reg3	@E + Add
	sub	voMpeg2Reg3, voMpeg2Reg7, voMpeg2Reg3	@E - Add
	
	mov     voMpeg2Reg7, #255
	orr     voMpeg2Reg7, voMpeg2Reg7, voMpeg2Reg7,  lsl #8
	
	mov	voMpeg2Reg1, voMpeg2Reg1, asr #17	@0
	and voMpeg2Reg1, voMpeg2Reg1,voMpeg2Reg7
	mov	voMpeg2Reg2, voMpeg2Reg2, asr #17	@7
	and voMpeg2Reg2, voMpeg2Reg2,voMpeg2Reg7
	mov	voMpeg2Reg8, voMpeg2Reg8, asr #17	@1
	and voMpeg2Reg8, voMpeg2Reg8,voMpeg2Reg7
	mov	voMpeg2Reg4, voMpeg2Reg4, asr #17	@6
	and voMpeg2Reg4, voMpeg2Reg4,voMpeg2Reg7
	mov	voMpeg2Reg5, voMpeg2Reg5, asr #17	@3
	and voMpeg2Reg5, voMpeg2Reg5,voMpeg2Reg7
	mov	voMpeg2Reg3, voMpeg2Reg3, asr #17	@5
	and voMpeg2Reg3, voMpeg2Reg3,voMpeg2Reg7
	mov	voMpeg2Reg6, voMpeg2Reg6, asr #17	@4
	and voMpeg2Reg6, voMpeg2Reg6,voMpeg2Reg7
	mov	voMpeg2Reg9, voMpeg2Reg9, asr #17	@2
	and voMpeg2Reg9, voMpeg2Reg9,voMpeg2Reg7	
	
	orr     voMpeg2Reg8, voMpeg2Reg1, voMpeg2Reg8, lsl #16     @0,1
	ldr		voMpeg2Reg1, [sp, #12]			@ dst = [sp, #12]	
	orr     voMpeg2Reg9, voMpeg2Reg9, voMpeg2Reg5, lsl #16     @2,3
	orr     voMpeg2Reg6, voMpeg2Reg6, voMpeg2Reg3, lsl #16     @4,5
	orr     voMpeg2Reg2, voMpeg2Reg4, voMpeg2Reg2, lsl #16	    @6,7
	ldr	    voMpeg2Reg3, [sp, #8]		    @ dst_stride = [sp, #8]	
	
	str     voMpeg2Reg8, [voMpeg2Reg1]
	str     voMpeg2Reg9, [voMpeg2Reg1, #4]
	str     voMpeg2Reg6, [voMpeg2Reg1, #8]
	str     voMpeg2Reg2, [voMpeg2Reg1, #12]			

	add	voMpeg2Reg3, voMpeg2Reg1, voMpeg2Reg3
	str	voMpeg2Reg3, [sp, #12]		@ dst = [sp, #12]
	mov	pc, lr

ir_allx1_4_t_lab_Rownosrc:
	mov	voMpeg2Reg1, #32			
	ldr	voMpeg2Reg9, WxC2xC6	@voMpeg2Reg10 = WxC2xC6			
	sxtah	voMpeg2Reg1, voMpeg2Reg1, voMpeg2Reg2		@Blk[0] + 32
	mov	voMpeg2Reg1, voMpeg2Reg1, asl #11	@E = (Blk[0] + 32) << 11				
	pkhtb	voMpeg2Reg2, voMpeg2Reg3, voMpeg2Reg2, asr #16	@ voMpeg2Reg2 = ip3|ip1
					@ voMpeg2Reg3 = ip2	voMpeg2Reg1 = ip0		
	smuad	voMpeg2Reg8, voMpeg2Reg2, voMpeg2Reg10	@@Cd = A + C  M(xC1S7, ip[1*8]) + M(xC3S5, ip[3*8])
	smusd	voMpeg2Reg4, voMpeg2Reg2, voMpeg2Reg11	@@Dd = B + D  M(xC7S1, ip[1*8]) + (-M(xC5S3, ip[3*8]))
	
	smusd	voMpeg2Reg5, voMpeg2Reg2, voMpeg2Reg10	@@Ad = A - C  M(xC1S7, ip[1*8]) - M(xC3S5, ip[3*8])
	smuad	voMpeg2Reg6, voMpeg2Reg2, voMpeg2Reg11	@@Bd = B - D  M(xC7S1, ip[1*8]) - (-M(xC5S3, ip[3*8]))	
	
	sub	voMpeg2Reg7, voMpeg2Reg5, voMpeg2Reg6
	add	voMpeg2Reg6, voMpeg2Reg5, voMpeg2Reg6
	mov	voMpeg2Reg5, voMpeg2Reg7, asr #8	@(Ad - Bd)) >> 8
	mov	voMpeg2Reg6, voMpeg2Reg6, asr #8	@(Ad + Bd)) >> 8
	mul	voMpeg2Reg5, voMpeg2Reg12, voMpeg2Reg5	@Add = (181 * (((Ad - Bd)) >> 8))@
	mul	voMpeg2Reg6, voMpeg2Reg12, voMpeg2Reg6	@Bdd = (181 * (((Ad + Bd)) >> 8))@
	
	smulbt	voMpeg2Reg7, voMpeg2Reg3, voMpeg2Reg9	@G = W2 * Blk[2*8]
	smulbb	voMpeg2Reg2, voMpeg2Reg3, voMpeg2Reg9	@H = W6 * Blk[2*8]@	
			
@ voMpeg2Reg3  free now
	sub	voMpeg2Reg3, voMpeg2Reg1, voMpeg2Reg7	@Ed = E - G@
	add	voMpeg2Reg7, voMpeg2Reg1, voMpeg2Reg7	@Fd = E + G@
	sub	voMpeg2Reg9, voMpeg2Reg1, voMpeg2Reg2	@Gd = E - H@
	add	voMpeg2Reg1, voMpeg2Reg1, voMpeg2Reg2	@Hd = E + H@
	
@ voMpeg2Reg1 free now
	add	voMpeg2Reg2, voMpeg2Reg7, voMpeg2Reg8	@Fd + Cd	0
	sub	voMpeg2Reg7, voMpeg2Reg7, voMpeg2Reg8	@Fd - Cd	7
	add	voMpeg2Reg8, voMpeg2Reg1, voMpeg2Reg6	@Hd + Bdd	1
	sub	voMpeg2Reg1, voMpeg2Reg1, voMpeg2Reg6	@Hd - Bdd	6	
	add	voMpeg2Reg6, voMpeg2Reg3, voMpeg2Reg4	@Ed + Dd	3
	sub	voMpeg2Reg3, voMpeg2Reg3, voMpeg2Reg4	@Ed - Dd	4	
	add	voMpeg2Reg4, voMpeg2Reg9, voMpeg2Reg5	@Gd + Add	2
	sub	voMpeg2Reg5, voMpeg2Reg9, voMpeg2Reg5	@Gd - Add	5
	
	mov     voMpeg2Reg9, #255
	orr     voMpeg2Reg9, voMpeg2Reg9, voMpeg2Reg9,  lsl #8	

	mov     voMpeg2Reg2, voMpeg2Reg2, asr #17
	and		voMpeg2Reg2, voMpeg2Reg2,voMpeg2Reg9
	mov     voMpeg2Reg8, voMpeg2Reg8, asr #17
	and		voMpeg2Reg8, voMpeg2Reg8,voMpeg2Reg9
	mov     voMpeg2Reg4, voMpeg2Reg4, asr #17
	and		voMpeg2Reg4, voMpeg2Reg4,voMpeg2Reg9
	mov     voMpeg2Reg6, voMpeg2Reg6, asr #17
	and		voMpeg2Reg6, voMpeg2Reg6,voMpeg2Reg9
	mov     voMpeg2Reg3, voMpeg2Reg3, asr #17
	and		voMpeg2Reg3, voMpeg2Reg3,voMpeg2Reg9
	mov     voMpeg2Reg5, voMpeg2Reg5, asr #17
	and		voMpeg2Reg5, voMpeg2Reg5,voMpeg2Reg9
	mov     voMpeg2Reg1, voMpeg2Reg1, asr #17
	and		voMpeg2Reg1, voMpeg2Reg1,voMpeg2Reg9
	mov     voMpeg2Reg7, voMpeg2Reg7, asr #17	
	and		voMpeg2Reg7, voMpeg2Reg7,voMpeg2Reg9
	
	orr     voMpeg2Reg7, voMpeg2Reg1,  voMpeg2Reg7, lsl #16
	orr     voMpeg2Reg5, voMpeg2Reg3,  voMpeg2Reg5, lsl #16
	orr     voMpeg2Reg4, voMpeg2Reg4,  voMpeg2Reg6, lsl #16
	orr     voMpeg2Reg2, voMpeg2Reg2,  voMpeg2Reg8, lsl #16 
	ldr	    voMpeg2Reg8, [sp, #12]			    @ dst = [sp, #12] 
	
	str    voMpeg2Reg7,  [voMpeg2Reg8, #12]
	str    voMpeg2Reg5,  [voMpeg2Reg8, #8]
	str    voMpeg2Reg4,  [voMpeg2Reg8, #4]
	str    voMpeg2Reg2,  [voMpeg2Reg8]
	ldr    voMpeg2Reg5,  [sp, #8]		@ dst_stride = [sp, #8]						

	add		voMpeg2Reg5, voMpeg2Reg8, voMpeg2Reg5
	str		voMpeg2Reg5, [sp, #12]		@ dst = [sp, #12]
	mov		pc, lr						
	@ENDP
	
	.ALIGN 4
IRRow8: @PROC
	ldrd		voMpeg2Reg2, [voMpeg2Reg6]			@ voMpeg2Reg2 = x4|x0 voMpeg2Reg3 = x7|x3
	ldrd		voMpeg2Reg10, [voMpeg2Reg6, #8]			@ voMpeg2Reg10 = x6|x1 voMpeg2Reg11 = x5|x2

	pkhtb		voMpeg2Reg0, voMpeg2Reg3, voMpeg2Reg10, asr #16	@ voMpeg2Reg0 = x7|x6
	pkhbt		voMpeg2Reg1, voMpeg2Reg3, voMpeg2Reg11, lsl #16	@ voMpeg2Reg1 = x2|x3
	mov         voMpeg2Reg4, voMpeg2Reg10, lsl #16
	pkhtb		voMpeg2Reg3, voMpeg2Reg11, voMpeg2Reg2, asr #16	@ voMpeg2Reg3 = x5|x4
	mov         voMpeg2Reg4, voMpeg2Reg4, asr #16		@ voMpeg2Reg4 = x1

	mov			voMpeg2Reg10, voMpeg2Reg2, lsl #16
	orr			voMpeg2Reg9, voMpeg2Reg0, voMpeg2Reg3			@ voMpeg2Reg9 = x7|x6|x5|x4
	mov			voMpeg2Reg10, voMpeg2Reg10, asr #16		@ voMpeg2Reg10 = x0

	orr			voMpeg2Reg9, voMpeg2Reg9, voMpeg2Reg1			@ voMpeg2Reg9 = x7|x6|x5|x4|x3|x2
	orrs		voMpeg2Reg9, voMpeg2Reg9, voMpeg2Reg4			@ voMpeg2Reg9 = x7|x6|x5|x4|x3|x2|x1
	bne			IRROWROWA	
	
	add			voMpeg2Reg10, voMpeg2Reg10, #32			    @ voMpeg2Reg10 = x0 + 32
	mov			voMpeg2Reg10, voMpeg2Reg10, asr #6		    @ voMpeg2Reg10 = SAT((x0 + 32)>>6)
	mov         voMpeg2Reg0,  voMpeg2Reg10, lsl #16
	mov         voMpeg2Reg10, voMpeg2Reg0,  lsr #16
	orr         voMpeg2Reg0,  voMpeg2Reg0,  voMpeg2Reg10 
	str			voMpeg2Reg0, [voMpeg2Reg8]
	str			voMpeg2Reg0, [voMpeg2Reg8, #4]
	str         voMpeg2Reg0, [voMpeg2Reg8, #8]
	str         voMpeg2Reg0, [voMpeg2Reg8, #12]
	mov			pc, lr
	
IRROWROWA:
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
	
	mov     voMpeg2Reg11, #255
	orr     voMpeg2Reg11, voMpeg2Reg11, voMpeg2Reg11,  lsl #8
	
	mov	voMpeg2Reg12, voMpeg2Reg12,asr #17		  @ voMpeg2Reg12 = blk[0]
	and voMpeg2Reg12, voMpeg2Reg12,voMpeg2Reg11
	mov	voMpeg2Reg5,  voMpeg2Reg5, asr #17			@ voMpeg2Reg5 = blk[1]
	and voMpeg2Reg5,  voMpeg2Reg5, voMpeg2Reg11
	mov	voMpeg2Reg1,  voMpeg2Reg1, asr #17			@ voMpeg2Reg1 = blk[2]
	and voMpeg2Reg1,  voMpeg2Reg1, voMpeg2Reg11
	mov	voMpeg2Reg10, voMpeg2Reg10,asr #17		  @ voMpeg2Reg10 = blk[3]
	and voMpeg2Reg10, voMpeg2Reg10, voMpeg2Reg11
	mov	voMpeg2Reg0,  voMpeg2Reg0, asr #17			@ voMpeg2Reg0 = blk[4]
	and voMpeg2Reg0,  voMpeg2Reg0, voMpeg2Reg11
	mov	voMpeg2Reg3,  voMpeg2Reg3, asr #17			@ voMpeg2Reg3 = blk[5]
	and voMpeg2Reg3,  voMpeg2Reg3, voMpeg2Reg11
	mov	voMpeg2Reg2,  voMpeg2Reg2, asr #17			@ voMpeg2Reg2 = blk[6]
	and voMpeg2Reg2,  voMpeg2Reg2, voMpeg2Reg11
	mov	voMpeg2Reg4,  voMpeg2Reg4, asr #17			@ voMpeg2Reg4 = blk[7]
	and voMpeg2Reg4,  voMpeg2Reg4, voMpeg2Reg11
	
	orr     voMpeg2Reg4,  voMpeg2Reg2,  voMpeg2Reg4,  lsl #16
	orr     voMpeg2Reg3,  voMpeg2Reg0,  voMpeg2Reg3,  lsl #16 
	orr     voMpeg2Reg10, voMpeg2Reg1,  voMpeg2Reg10, lsl #16
	orr     voMpeg2Reg5,  voMpeg2Reg12, voMpeg2Reg5,  lsl #16
	
	str     voMpeg2Reg4,  [voMpeg2Reg8, #12]
	str     voMpeg2Reg3,  [voMpeg2Reg8, #8]
	str     voMpeg2Reg10, [voMpeg2Reg8, #4]
	str     voMpeg2Reg5,  [voMpeg2Reg8]

	mov		pc,lr
	@ENDP

	.ALIGN 4
Bit16ArmIdctA: @PROC
	stmdb   sp!, {voMpeg2Reg4 - voMpeg2Reg12, lr} @ voMpeg2Reg0=BlockEnd voMpeg2Reg2=DstStride
	ldr		voMpeg2Reg9, [sp, #40]		@ lr = src_stride
	sub		sp, sp, #16
	mov     voMpeg2Reg2, voMpeg2Reg2, lsl #1
	str		voMpeg2Reg9, [sp, #4]		@ src_stride
	str		voMpeg2Reg2, [sp, #8]		@ dst_stride
	mov		voMpeg2Reg7, voMpeg2Reg3				@ Src
	mov	    voMpeg2Reg8, voMpeg2Reg1				@ Dst
	mov		voMpeg2Reg6, voMpeg2Reg0				@ voMpeg2Reg6 = Block
	

	bl      IRCol8  
	add     voMpeg2Reg6, voMpeg2Reg6, #2
	bl      IRCol8  
	add     voMpeg2Reg6, voMpeg2Reg6, #2
	bl      IRCol8  
	add     voMpeg2Reg6, voMpeg2Reg6, #2
	bl      IRCol8  
	sub     voMpeg2Reg6, voMpeg2Reg6, #6
	
	bl		IRRow8
	add		voMpeg2Reg6, voMpeg2Reg6, #16			@Block += 16
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0		
	bl		IRRow8
	add		voMpeg2Reg6, voMpeg2Reg6, #16			@Block += 16
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0	
	bl		IRRow8
	add		voMpeg2Reg6, voMpeg2Reg6, #16			@Block += 16
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0	
	bl		IRRow8
	add		voMpeg2Reg6, voMpeg2Reg6, #16			@Block += 16
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0	
	bl		IRRow8
	add		voMpeg2Reg6, voMpeg2Reg6, #16			@Block += 16
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0	
	bl		IRRow8
	add		voMpeg2Reg6, voMpeg2Reg6, #16			@Block += 16
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0	
	bl		IRRow8
	add		voMpeg2Reg6, voMpeg2Reg6, #16			@Block += 16
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0	
	bl		IRRow8

	add			sp, sp, #16
	ldmia   sp!, {voMpeg2Reg4 - voMpeg2Reg12, pc} 

	@ENDP

@ voMpeg2Reg6 Block
@ voMpeg2Reg7 Src
@ voMpeg2Reg8 Dst
@ lr DstStride

	.ALIGN 4
Bit16ArmIdctB: @PROC

	stmdb   sp!, {voMpeg2Reg4 - voMpeg2Reg12, lr}  @ voMpeg2Reg0=BlockEnd voMpeg2Reg2=DstStride
	ldr		voMpeg2Reg9, [sp, #40]		@ lr = src_stride
	sub		sp, sp, #16
	mov     voMpeg2Reg2, voMpeg2Reg2, lsl #1
	str		voMpeg2Reg9, [sp, #4]		@ src_stride
	str		voMpeg2Reg2, [sp, #8]		@ dst_stride
	mov		voMpeg2Reg7, voMpeg2Reg3				@ Src
	mov	    voMpeg2Reg8, voMpeg2Reg1				@ Dst
	mov		voMpeg2Reg6, voMpeg2Reg0				@ voMpeg2Reg6 = Block
	

	bl      IRCol8  
	add     voMpeg2Reg6, voMpeg2Reg6, #2
	bl      IRCol8  
	add     voMpeg2Reg6, voMpeg2Reg6, #2
	bl      IRCol8  
	add     voMpeg2Reg6, voMpeg2Reg6, #2
	bl      IRCol8  
	add     voMpeg2Reg6, voMpeg2Reg6, #2
	bl      IRCol8  
	add     voMpeg2Reg6, voMpeg2Reg6, #2
	bl      IRCol8  
	add     voMpeg2Reg6, voMpeg2Reg6, #2
	bl      IRCol8  
	add     voMpeg2Reg6, voMpeg2Reg6, #2
	bl      IRCol8 
	sub     voMpeg2Reg6, voMpeg2Reg6, #14
	
	bl		IRRow8
	add		voMpeg2Reg6, voMpeg2Reg6, #16			@Block += 16
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0		
	bl		IRRow8
	add		voMpeg2Reg6, voMpeg2Reg6, #16			@Block += 16
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0	
	bl		IRRow8
	add		voMpeg2Reg6, voMpeg2Reg6, #16			@Block += 16
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0	
	bl		IRRow8
	add		voMpeg2Reg6, voMpeg2Reg6, #16			@Block += 16
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0	
	bl		IRRow8
	add		voMpeg2Reg6, voMpeg2Reg6, #16			@Block += 16
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0	
	bl		IRRow8
	add		voMpeg2Reg6, voMpeg2Reg6, #16			@Block += 16
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0	
	bl		IRRow8
	add		voMpeg2Reg6, voMpeg2Reg6, #16			@Block += 16
	ldr		voMpeg2Reg0, [sp, #8]
	add		voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg0	
	bl		IRRow8

	add			sp, sp, #16
	ldmia   sp!, {voMpeg2Reg4 - voMpeg2Reg12, pc}  
	@ENDP

	.ALIGN 4
Bit16ArmIdctG: @PROC
    STMFD    sp!,{voMpeg2Reg4-voMpeg2Reg11,lr}	
@ voMpeg2Reg0 = Block, voMpeg2Reg1 = dst, voMpeg2Reg2 = dst_stride, voMpeg2Reg3 = Src, voMpeg2Reg12 = [sp] = src_stride
	sub	sp, sp, #16	
	mov     voMpeg2Reg2, voMpeg2Reg2, lsl #1
	str	voMpeg2Reg3, [sp, #4]		@ Src = [sp, #4]
	str	voMpeg2Reg2, [sp, #8]		@ dst_stride = [sp, #8]
	str	voMpeg2Reg1, [sp, #12]		@ dst = [sp, #12]

	ldr	voMpeg2Reg10, WxC3xC1	@voMpeg2Reg10 = WxC3xC1
	ldr	voMpeg2Reg11, WxC5xC7	@voMpeg2Reg11 = WxC5xC7
	ldr	voMpeg2Reg12, Wx00xC4	@voMpeg2Reg12 = Wx00xC4
	
	bl      IRCol8_4x4  
	add     voMpeg2Reg0, voMpeg2Reg0, #2
	bl      IRCol8_4x4  
	add     voMpeg2Reg0, voMpeg2Reg0, #2	
	bl      IRCol8_4x4  
	add     voMpeg2Reg0, voMpeg2Reg0, #2	
	bl      IRCol8_4x4  	
	sub     voMpeg2Reg0, voMpeg2Reg0, #6

	bl		IRRow8nosrc_4x4
	add		voMpeg2Reg0, voMpeg2Reg0, #16			@Block += 16		
	bl		IRRow8nosrc_4x4
	add		voMpeg2Reg0, voMpeg2Reg0, #16			@Block += 16	
	bl		IRRow8nosrc_4x4
	add		voMpeg2Reg0, voMpeg2Reg0, #16			@Block += 16	
	bl		IRRow8nosrc_4x4
	add		voMpeg2Reg0, voMpeg2Reg0, #16			@Block += 16	
	bl		IRRow8nosrc_4x4
	add		voMpeg2Reg0, voMpeg2Reg0, #16			@Block += 16	
	bl		IRRow8nosrc_4x4
	add		voMpeg2Reg0, voMpeg2Reg0, #16			@Block += 16	
	bl		IRRow8nosrc_4x4
	add		voMpeg2Reg0, voMpeg2Reg0, #16			@Block += 16	
	bl		IRRow8nosrc_4x4
                                         		
	add	sp, sp, #16
    LDMFD    sp!,{voMpeg2Reg4-voMpeg2Reg11,pc}
        	
	@ENDP

   .ALIGN  4
W17:		.word 0xb190235
W53:		.word	0x06490968
W26:		.word	0x0a740454
W13:		.word	0x0a740454			@huwei 20080124 dowmsample

WxC1xC7:		.word 0x0B190235			@xC7 = D0.S16[0] xC1 = D0.S16[1]		
WxC3xC5:		.word 0x09680649			@xC5 = D0.S16[2] xC3 = D0.S16[3]			
WxC2xC6:		.word 0x0A740454			@xC6 = D1.S16[0] xC2 = D1.S16[1]		
WxC4xC4:		.word 0x00B500B5			@xC4 = D1.S32[1]/D1.S32[2]
Wx80808080:	.word 0x80808080			@
Wx04040000:	.word 0x04040000 


WxC5xC7:		.word 0x06490235			@xC7 = D0.S16[0] xC5 = D0.S16[1]		
WxC3xC1:		.word 0x09680B19			@xC1 = D0.S16[2] xC3 = D0.S16[3]		
Wx00xC4:		.word 0x000000B5			@xC6 = D1.S32[1]/D1.S32[2]

	@END
	