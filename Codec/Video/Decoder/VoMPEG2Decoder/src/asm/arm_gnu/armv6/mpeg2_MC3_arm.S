@************************************************************************
@																		*
@	VisualOn, Inc. Confidential and Proprietary, 2005		            *
@																		*
@***********************************************************************/

@R0 src
@R1 dst
@R2 srcpitch
@R3 dstpitch

	@AREA	|.text|, CODE
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
 
 voMpeg2Reg14     .req r14


  .macro GenCodeCpy Name,Sub,Round,Add,Fast,ARM5,ARMv6, ARM11

	.align 8
	.global \Name
\Name: @proc
	stmdb	sp!, {voMpeg2Reg4 - voMpeg2Reg12, lr}
	.if \Fast>0	  
	.if \ARM11>0
	\Sub	\Name.0_v11,0,\Round,\Add,0
	.else
	movs	voMpeg2Reg4,voMpeg2Reg0,lsl #30
	beq		LAB4\Name
	cmp	voMpeg2Reg4,#0x80000000
	beq		LAB2\Name
	bhi		LAB3\Name
LAB1\Name:
	bic		voMpeg2Reg0,voMpeg2Reg0,#3
	\Sub	_8\Name,8,\Round,\Add,\ARMv6
LAB2\Name:	
	bic		voMpeg2Reg0,voMpeg2Reg0,#3
	\Sub	_16\Name,16,\Round,\Add,\ARMv6
LAB3\Name:	
	bic		voMpeg2Reg0,voMpeg2Reg0,#3
	\Sub	_24\Name,24,\Round,\Add,\ARMv6
LAB4\Name:	
	\Sub	_0\Name,0,\Round,\Add,\ARMv6
	.endif
	.else
	\Sub	_s\Name,-1,\Round,\Add,\ARMv6
	.endif
	@endp
  .endm


@------------------------------------------
@BArmInter8x4
@------------------------------------------

  .macro CpyBlkRArm ARMv6,Pos
	.if \Pos > 0
	  ldr	voMpeg2Reg5,[voMpeg2Reg0,#8]
	  ldr	voMpeg2Reg6,[voMpeg2Reg0,#4]
	  ldr	voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2
	  ldr	voMpeg2Reg9,[voMpeg2Reg0,#8]
	  ldr	voMpeg2Reg10,[voMpeg2Reg0,#4]
	  ldr	voMpeg2Reg8,[voMpeg2Reg0],voMpeg2Reg2  		
	  mov	voMpeg2Reg5,voMpeg2Reg5,lsl #32-\Pos
	  mov	voMpeg2Reg4,voMpeg2Reg4,lsr #\Pos
	  orr	voMpeg2Reg5,voMpeg2Reg5,voMpeg2Reg6,lsr #\Pos
	  orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6,lsl #32-\Pos
	  mov	voMpeg2Reg9,voMpeg2Reg9,lsl #32-\Pos
	  mov	voMpeg2Reg8,voMpeg2Reg8,lsr #\Pos
	  orr	voMpeg2Reg9,voMpeg2Reg9,voMpeg2Reg10,lsr #\Pos
	  orr	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg10,lsl #32-\Pos	  
	  str	voMpeg2Reg5,[voMpeg2Reg1,#4]
	  str	voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3
	  str	voMpeg2Reg9,[voMpeg2Reg1,#4]
	  str	voMpeg2Reg8,[voMpeg2Reg1],voMpeg2Reg3

	  
	  ldr	voMpeg2Reg5,[voMpeg2Reg0,#8]
	  ldr	voMpeg2Reg6,[voMpeg2Reg0,#4]
	  ldr	voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2
	  ldr	voMpeg2Reg9,[voMpeg2Reg0,#8]
	  ldr	voMpeg2Reg10,[voMpeg2Reg0,#4]
	  ldr	voMpeg2Reg8,[voMpeg2Reg0],voMpeg2Reg2	  		
	  mov	voMpeg2Reg5,voMpeg2Reg5,lsl #32-\Pos
	  mov	voMpeg2Reg4,voMpeg2Reg4,lsr #\Pos
	  orr	voMpeg2Reg5,voMpeg2Reg5,voMpeg2Reg6,lsr #\Pos
	  orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6,lsl #32-\Pos
	  mov	voMpeg2Reg9,voMpeg2Reg9,lsl #32-\Pos
	  mov	voMpeg2Reg8,voMpeg2Reg8,lsr #\Pos
	  orr	voMpeg2Reg9,voMpeg2Reg9,voMpeg2Reg10,lsr #\Pos
	  orr	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg10,lsl #32-\Pos	  
	  str	voMpeg2Reg5,[voMpeg2Reg1,#4]
	  str	voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3
	  str	voMpeg2Reg9,[voMpeg2Reg1,#4]
	  str	voMpeg2Reg8,[voMpeg2Reg1],voMpeg2Reg3  
	  subs	voMpeg2Reg12, voMpeg2Reg12, #4	   	  
	.else
		ldr		voMpeg2Reg5,[voMpeg2Reg0,#4]
		ldr		voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2	
		ldr		voMpeg2Reg7,[voMpeg2Reg0,#4]
		ldr		voMpeg2Reg6,[voMpeg2Reg0],voMpeg2Reg2	
		ldr		voMpeg2Reg9,[voMpeg2Reg0,#4]
		ldr		voMpeg2Reg8,[voMpeg2Reg0],voMpeg2Reg2
		ldr		voMpeg2Reg11,[voMpeg2Reg0,#4]
		ldr		voMpeg2Reg10,[voMpeg2Reg0],voMpeg2Reg2		  
		subs	voMpeg2Reg12, voMpeg2Reg12, #4		  		  	  	  
		str		voMpeg2Reg5,[voMpeg2Reg1,#4]
		str		voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3
		str		voMpeg2Reg7,[voMpeg2Reg1,#4]
		str		voMpeg2Reg6,[voMpeg2Reg1],voMpeg2Reg3
		str		voMpeg2Reg9,[voMpeg2Reg1,#4]
		str		voMpeg2Reg8,[voMpeg2Reg1],voMpeg2Reg3
		str		voMpeg2Reg11,[voMpeg2Reg1,#4]
		str		voMpeg2Reg10,[voMpeg2Reg1],voMpeg2Reg3	  		  		  		  
	.endif
  .endm

  .macro BArmInter8x4 Id, Pos, Round, Add, ARMv6

	ldr		voMpeg2Reg12, [sp, #40]
loop_again\Id:
@	pld [voMpeg2Reg0,voMpeg2Reg2]
	CpyBlkRArm \ARMv6,\Pos 
	bne		loop_again\Id
	ldmia	sp!, {voMpeg2Reg4 - voMpeg2Reg12, pc}
  .endm

@------------------------------------------
@BArmInter2_8x4 
@------------------------------------------

  .macro arm_addblkRow Pos, ARMv6

	.if \Pos > 0
	  ldr	voMpeg2Reg5,[voMpeg2Reg0,#8]
	  ldr	voMpeg2Reg6,[voMpeg2Reg0,#4]
	  ldr	voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2
		
	  mov	voMpeg2Reg5,voMpeg2Reg5,lsl #32-\Pos
	  mov	voMpeg2Reg4,voMpeg2Reg4,lsr #\Pos
	  orr	voMpeg2Reg5,voMpeg2Reg5,voMpeg2Reg6,lsr #\Pos
	  orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6,lsl #32-\Pos
	.else	
	  ldr	voMpeg2Reg5,[voMpeg2Reg0,#4]
	  ldr	voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2	  
	.endif

   .if \ARMv6 > 0
	ldrd	voMpeg2Reg6,[voMpeg2Reg1] 
	uqadd8  voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg14
	uqadd8  voMpeg2Reg5, voMpeg2Reg5, voMpeg2Reg14	
	uhadd8	voMpeg2Reg6, voMpeg2Reg6, voMpeg2Reg4
	uhadd8	voMpeg2Reg7, voMpeg2Reg7, voMpeg2Reg5	

	strd	voMpeg2Reg6,[voMpeg2Reg1], voMpeg2Reg3  
		  			  		  		  	  		  		  
  .else	
	ldr		voMpeg2Reg7,[voMpeg2Reg1,#4]
	ldr		voMpeg2Reg6,[voMpeg2Reg1]
	
	and		voMpeg2Reg9,voMpeg2Reg12,voMpeg2Reg5,lsr #1
	and		voMpeg2Reg8,voMpeg2Reg12,voMpeg2Reg4,lsr #1
	orr		voMpeg2Reg5,voMpeg2Reg7,voMpeg2Reg5
	orr		voMpeg2Reg4,voMpeg2Reg6,voMpeg2Reg4
	and		voMpeg2Reg7,voMpeg2Reg12,voMpeg2Reg7,lsr #1
	and		voMpeg2Reg6,voMpeg2Reg12,voMpeg2Reg6,lsr #1
	add		voMpeg2Reg7,voMpeg2Reg7,voMpeg2Reg9
	add		voMpeg2Reg6,voMpeg2Reg6,voMpeg2Reg8
	and		voMpeg2Reg5,voMpeg2Reg5,voMpeg2Reg14
	and		voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg14
	add		voMpeg2Reg7,voMpeg2Reg7,voMpeg2Reg5
	add		voMpeg2Reg6,voMpeg2Reg6,voMpeg2Reg4

	str		voMpeg2Reg7,[voMpeg2Reg1,#4]
	str		voMpeg2Reg6,[voMpeg2Reg1],voMpeg2Reg3		
   .endif	  	

  .endm

  .macro BArmInter2_8x4  Id, Pos, Round, Add, ARMv6

@	PldBI \ARM5,\Pos
	ldr		voMpeg2Reg14,LABM\Id

	ldr		voMpeg2Reg11, [sp, #40]

@	mov		voMpeg2Reg11,#8
	mvn		voMpeg2Reg12,voMpeg2Reg14,lsl #7

LABLOOP\Id:
@	pld [voMpeg2Reg0,voMpeg2Reg2]
	arm_addblkRow \Pos, \ARMv6
	subs voMpeg2Reg11,voMpeg2Reg11,#1 
	bne  LABLOOP\Id

	ldmia	sp!, {voMpeg2Reg4 - voMpeg2Reg12, pc}
LABM\Id: .word 0x01010101
  .endm
  
@------------------------------------------
@ BArmInter8x4H
@------------------------------------------

  .macro  LoadHorRow Id, Pos, ARMv6

    @ result is voMpeg2Reg4,voMpeg2Reg5 and voMpeg2Reg8,voMpeg2Reg9 (one pixel to the right)
    @ voMpeg2Reg6,voMpeg2Reg7 can be used

    ldr		voMpeg2Reg5,[voMpeg2Reg0,#4]
    ldr		voMpeg2Reg6,[voMpeg2Reg0,#8]
    ldr		voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2

    .if \Pos+8 < 32
	  mov	voMpeg2Reg9,voMpeg2Reg5,lsr #\Pos+8
	  mov	voMpeg2Reg8,voMpeg2Reg4,lsr #\Pos+8
	  orr	voMpeg2Reg9,voMpeg2Reg9,voMpeg2Reg6,lsl #32-\Pos-8
	  orr	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg5,lsl #32-\Pos-8
    .else
	  mov	voMpeg2Reg8,voMpeg2Reg5
	  mov	voMpeg2Reg9,voMpeg2Reg6
    .endif

	.if \Pos > 0
	  mov	voMpeg2Reg4,voMpeg2Reg4,lsr #\Pos
	  mov	voMpeg2Reg6,voMpeg2Reg6,lsl #32-\Pos
	  orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg5,lsl #32-\Pos
	  orr	voMpeg2Reg5,voMpeg2Reg6,voMpeg2Reg5,lsr #\Pos
	.endif
	
  .endm

  .macro CopyHorRow Id, Pos, Round, Add, ARMv6

@voMpeg2Reg14 01010101
@voMpeg2Reg12 7f7f7f7f

	LoadHorRow	\Id,\Pos, \ARMv6
	.if \ARMv6>0
	
	.if \Round>0
	.else
	uqadd8  voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg14
	uqadd8  voMpeg2Reg5, voMpeg2Reg5, voMpeg2Reg14	
	.endif
	
	uhadd8	voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg8
	uhadd8	voMpeg2Reg5, voMpeg2Reg5, voMpeg2Reg9
	
	.if \Add>0
	  ldr	voMpeg2Reg7,[voMpeg2Reg1,#4]
	  ldr	voMpeg2Reg6,[voMpeg2Reg1]

	uqadd8  voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg14
	uqadd8  voMpeg2Reg5, voMpeg2Reg5, voMpeg2Reg14		  
	uhadd8	voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg6
	uhadd8	voMpeg2Reg5, voMpeg2Reg5, voMpeg2Reg7	
	
	.endif	
	  strd	voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3
	  	
	.else
	
	and		voMpeg2Reg6,voMpeg2Reg12,voMpeg2Reg4,lsr #1
	and		voMpeg2Reg7,voMpeg2Reg12,voMpeg2Reg5,lsr #1
	.if \Round>0
	  and	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg8
	  and	voMpeg2Reg5,voMpeg2Reg5,voMpeg2Reg9
	.else
	  orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg8
	  orr	voMpeg2Reg5,voMpeg2Reg5,voMpeg2Reg9
	.endif

	and		voMpeg2Reg8,voMpeg2Reg12,voMpeg2Reg8,lsr #1
	and		voMpeg2Reg9,voMpeg2Reg12,voMpeg2Reg9,lsr #1

	and		voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg14
	and		voMpeg2Reg5,voMpeg2Reg5,voMpeg2Reg14

	add		voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6
	add		voMpeg2Reg5,voMpeg2Reg5,voMpeg2Reg7
	add		voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg8
	add		voMpeg2Reg5,voMpeg2Reg5,voMpeg2Reg9
	
	.if \Add>0
	  ldr	voMpeg2Reg7,[voMpeg2Reg1,#4]
	  ldr	voMpeg2Reg6,[voMpeg2Reg1]
	  
	  and	voMpeg2Reg9,voMpeg2Reg12,voMpeg2Reg5,lsr #1
	  and	voMpeg2Reg8,voMpeg2Reg12,voMpeg2Reg4,lsr #1
	  orr	voMpeg2Reg5,voMpeg2Reg7,voMpeg2Reg5
	  orr	voMpeg2Reg4,voMpeg2Reg6,voMpeg2Reg4
	  and	voMpeg2Reg7,voMpeg2Reg12,voMpeg2Reg7,lsr #1
	  and	voMpeg2Reg6,voMpeg2Reg12,voMpeg2Reg6,lsr #1
	  add	voMpeg2Reg7,voMpeg2Reg7,voMpeg2Reg9
	  add	voMpeg2Reg6,voMpeg2Reg6,voMpeg2Reg8
	  and	voMpeg2Reg5,voMpeg2Reg5,voMpeg2Reg14
	  and	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg14
	  add	voMpeg2Reg5,voMpeg2Reg7,voMpeg2Reg5
	  add	voMpeg2Reg4,voMpeg2Reg6,voMpeg2Reg4
	.endif	
	  str	voMpeg2Reg5,[voMpeg2Reg1,#4]
	  str	voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3
	  
	.endif	  

  .endm

  .macro  BArmInter8x4H Id, Pos, Round, Add, ARMv6

	.if \ARMv6>0
		ldr		voMpeg2Reg14,LABM_ARMv6\Id
		ldr		voMpeg2Reg11, [sp, #40]
@		mvn		voMpeg2Reg12,voMpeg2Reg14,lsl #7

LABLOOP_ARMv6\Id:
@		pld [voMpeg2Reg0,voMpeg2Reg2]
		CopyHorRow_ARMv6 \Id,\Pos,\Round,\Add, \ARMv6
@		subs voMpeg2Reg11,voMpeg2Reg11,#1
		bne  LABLOOP_ARMv6\Id
		ldmia	sp!, {voMpeg2Reg4 - voMpeg2Reg12, pc}
LABM_ARMv6\Id: .word 0x01010101
		  
	.else	  
@		PldBI \ARM5,\Pos
		ldr		voMpeg2Reg14,LABM\Id
		ldr		voMpeg2Reg11, [sp, #40]

@		mov		voMpeg2Reg11,#8
		mvn		voMpeg2Reg12,voMpeg2Reg14,lsl #7
LABLOOP\Id:
		pld [voMpeg2Reg0,voMpeg2Reg2]
		CopyHorRow \Id,\Pos,\Round,\Add, \ARMv6
		subs voMpeg2Reg11,voMpeg2Reg11,#1
		bne  LABLOOP\Id

		ldmia	sp!, {voMpeg2Reg4 - voMpeg2Reg12, pc}
LABM\Id: .word 0x01010101
	.endif
  .endm

  .macro  LoadHorRow_ARMv6 Id, Pos, ARMv6

    @ result is voMpeg2Reg4,voMpeg2Reg5 and voMpeg2Reg8,voMpeg2Reg9 (one pixel to the right)
    @ voMpeg2Reg6,voMpeg2Reg7 can be used

    	ldr	voMpeg2Reg5,[voMpeg2Reg0,#4]
    	ldr	voMpeg2Reg6,[voMpeg2Reg0,#8]
    	ldr	voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2
	subs	voMpeg2Reg11,voMpeg2Reg11,#1
    .if \Pos+8 < 32
	  mov	voMpeg2Reg9,voMpeg2Reg5,lsr #\Pos+8
	  orr	voMpeg2Reg9,voMpeg2Reg9,voMpeg2Reg6,lsl #32-\Pos-8
	  mov	voMpeg2Reg8,voMpeg2Reg4,lsr #\Pos+8	  
	  orr	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg5,lsl #32-\Pos-8
    .else
	  mov	voMpeg2Reg8,voMpeg2Reg5
	  mov	voMpeg2Reg9,voMpeg2Reg6
    .endif

	.if \Pos > 0
	  mov	voMpeg2Reg4,voMpeg2Reg4,lsr #\Pos
	  mov	voMpeg2Reg6,voMpeg2Reg6,lsl #32-\Pos
	  orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg5,lsl #32-\Pos
	  orr	voMpeg2Reg5,voMpeg2Reg6,voMpeg2Reg5,lsr #\Pos
	.endif
	
  .endm

  .macro CopyHorRow_ARMv6 Id, Pos, Round, Add, ARMv6

@voMpeg2Reg14 01010101
@voMpeg2Reg12 7f7f7f7f

	LoadHorRow_ARMv6	\Id,\Pos, \ARMv6
	
	.if \Round>0
	.else
		uqadd8  voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg14
		uqadd8  voMpeg2Reg5, voMpeg2Reg5, voMpeg2Reg14	
	.endif
	
		uhadd8	voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg8
		uhadd8	voMpeg2Reg5, voMpeg2Reg5, voMpeg2Reg9
	
	.if \Add>0
		ldrd	voMpeg2Reg6,[voMpeg2Reg1]
		uqadd8  voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg14
		uqadd8  voMpeg2Reg5, voMpeg2Reg5, voMpeg2Reg14		  
		uhadd8	voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg6
		uhadd8	voMpeg2Reg5, voMpeg2Reg5, voMpeg2Reg7	
	.endif	
	  strd	voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3
  .endm
@------------------------------------------
@ BArmInter8x4V
@------------------------------------------

  .macro  LoadVerRow Id, Pos, Parity, ARMv6
  .if \Parity>0
    @ result is voMpeg2Reg8,voMpeg2Reg9 (voMpeg2Reg10=voMpeg2Reg8>>1,voMpeg2Reg11=voMpeg2Reg9>>1) 
    @ voMpeg2Reg10,voMpeg2Reg11 can be used
    .if \Pos > 0
	  ldr	voMpeg2Reg9,[voMpeg2Reg0,#8]
	  ldr	voMpeg2Reg10,[voMpeg2Reg0,#4]
	  ldr	voMpeg2Reg8,[voMpeg2Reg0],voMpeg2Reg2

	  mov	voMpeg2Reg9,voMpeg2Reg9,lsl #32-\Pos
	  mov	voMpeg2Reg8,voMpeg2Reg8,lsr #\Pos
	  orr	voMpeg2Reg9,voMpeg2Reg9,voMpeg2Reg10,lsr #\Pos
	  orr	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg10,lsl #32-\Pos
    .else
	  ldr	voMpeg2Reg9,[voMpeg2Reg0,#4]
	  ldr	voMpeg2Reg8,[voMpeg2Reg0],voMpeg2Reg2
    .endif
	.if \ARMv6>0
	.else    
	and		voMpeg2Reg11,voMpeg2Reg12,voMpeg2Reg9,lsr #1
	and		voMpeg2Reg10,voMpeg2Reg12,voMpeg2Reg8,lsr #1
	.endif
  .else
    @ result is voMpeg2Reg4,voMpeg2Reg5 (voMpeg2Reg6=voMpeg2Reg4>>1,voMpeg2Reg7=voMpeg2Reg5>>1) 
    @ voMpeg2Reg6,voMpeg2Reg7 can be used

    .if \Pos > 0
	  ldr	voMpeg2Reg5,[voMpeg2Reg0,#8]
	  ldr	voMpeg2Reg6,[voMpeg2Reg0,#4]
	  ldr	voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2

	  mov	voMpeg2Reg5,voMpeg2Reg5,lsl #32-\Pos
	  mov	voMpeg2Reg4,voMpeg2Reg4,lsr #\Pos
	  orr	voMpeg2Reg5,voMpeg2Reg5,voMpeg2Reg6,lsr #\Pos
	  orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6,lsl #32-\Pos
    .else 
	  ldr	voMpeg2Reg5,[voMpeg2Reg0,#4]
	  ldr	voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2    
    .endif
	.if \ARMv6>0 
	.else  	   
	and		voMpeg2Reg7,voMpeg2Reg12,voMpeg2Reg5,lsr #1
	and		voMpeg2Reg6,voMpeg2Reg12,voMpeg2Reg4,lsr #1
        .endif    
  .endif
  .endm

  .macro CopyVerRow Id, Pos, Parity, Round, Add, ARMv6

@voMpeg2Reg14 01010101
@voMpeg2Reg12 7f7f7f7f

	LoadVerRow \Id,\Pos,\Parity, \ARMv6
	.if \ARMv6>0 

		.if \Round>0
		.else
		uqadd8  voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg14
		uqadd8  voMpeg2Reg5, voMpeg2Reg5, voMpeg2Reg14	
		.endif
	
		uhadd8	voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg8
		uhadd8	voMpeg2Reg5, voMpeg2Reg5, voMpeg2Reg9
	
		.if \Add>0
	  	ldrd	voMpeg2Reg10,[voMpeg2Reg1]

		uqadd8  voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg14
		uqadd8  voMpeg2Reg5, voMpeg2Reg5, voMpeg2Reg14		  
		uhadd8	voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg10
		uhadd8	voMpeg2Reg5, voMpeg2Reg5, voMpeg2Reg11				
		.endif	
	  	strd	voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3  
	.else
		.if \Round>0
	    	and	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg8
	    	and	voMpeg2Reg5,voMpeg2Reg5,voMpeg2Reg9
	  	.else
	    	orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg8
	    	orr	voMpeg2Reg5,voMpeg2Reg5,voMpeg2Reg9
	  	.endif
	  	and	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg14
	  	and	voMpeg2Reg5,voMpeg2Reg5,voMpeg2Reg14

	  	add	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6
	  	add	voMpeg2Reg5,voMpeg2Reg5,voMpeg2Reg7

  	  	add	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg10
	  	add	voMpeg2Reg5,voMpeg2Reg5,voMpeg2Reg11

	  	.if \Add>0
	    	ldr	voMpeg2Reg7,[voMpeg2Reg1,#4]
	    	ldr	voMpeg2Reg6,[voMpeg2Reg1]
	    	and	voMpeg2Reg3,voMpeg2Reg12,voMpeg2Reg5,lsr #1
		orr voMpeg2Reg5,voMpeg2Reg7,voMpeg2Reg5
		and	voMpeg2Reg7,voMpeg2Reg12,voMpeg2Reg7,lsr #1
		add voMpeg2Reg7,voMpeg2Reg7,voMpeg2Reg3
		and	voMpeg2Reg3,voMpeg2Reg12,voMpeg2Reg4,lsr #1
		orr voMpeg2Reg4,voMpeg2Reg6,voMpeg2Reg4
		and	voMpeg2Reg6,voMpeg2Reg12,voMpeg2Reg6,lsr #1
		add voMpeg2Reg6,voMpeg2Reg6,voMpeg2Reg3
		and voMpeg2Reg5,voMpeg2Reg5,voMpeg2Reg14
		and voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg14
		add voMpeg2Reg5,voMpeg2Reg5,voMpeg2Reg7
		add voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6
		ldr	voMpeg2Reg3,[sp, #4]		@			
		ldr	voMpeg2Reg7,[sp]		@end src for loop compare
		str	voMpeg2Reg5,[voMpeg2Reg1,#4]
		str	voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3
		.else
		ldr	voMpeg2Reg7,[sp]		@end src for loop compare
		str	voMpeg2Reg5,[voMpeg2Reg1,#4]
		str	voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3
		.endif
	.endif    	
  .endm


  .macro BArmInter8x4V Id, Pos, Round, Add, ARMv6

     .if \ARMv6>0
	ldr		voMpeg2Reg4, [sp, #40]
	mul		voMpeg2Reg4, voMpeg2Reg2, voMpeg2Reg4
	ldr		voMpeg2Reg14,LABM_ARMv6\Id
	add		voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg0
	mov		voMpeg2Reg7, voMpeg2Reg4
	mov		voMpeg2Reg2, voMpeg2Reg2,lsr #1
	@add		voMpeg2Reg7, voMpeg2Reg4, voMpeg2Reg2, lsr #2	
LABLOOP_ARMv6\Id:
	LoadVerRow_ARMv6 \Id,\Pos,0, \ARMv6
	CopyVerRow_ARMv6 \Id,\Pos,1,\Round,\Add, \ARMv6

	cmp		voMpeg2Reg0,voMpeg2Reg7
	bne		LABLOOP_ARMv6\Id
	ldmia	sp!, {voMpeg2Reg4 - voMpeg2Reg12, pc}
LABM_ARMv6\Id: .word 0x01010101
		  
      .else
	  
	ldr		voMpeg2Reg4, [sp, #40]
	sub		sp,sp,#8
	mul		voMpeg2Reg4, voMpeg2Reg2, voMpeg2Reg4
	ldr		voMpeg2Reg14,LABM\Id
	add		voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg0
	mov		voMpeg2Reg2, voMpeg2Reg2, lsr #1

	.if \ARMv6>0 
	mov		voMpeg2Reg7,voMpeg2Reg4
	@add		voMpeg2Reg7,voMpeg2Reg4,voMpeg2Reg2,lsr #2	
	.else  	   
	@add		voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg2,lsr #2
	str		voMpeg2Reg4,[sp]		@end src
	str		voMpeg2Reg3,[sp, #4]	
	.endif 

	mvn		voMpeg2Reg12,voMpeg2Reg14,lsl #7
LABLOOP\Id:

	LoadVerRow \Id,\Pos,0, \ARMv6
	CopyVerRow \Id,\Pos,1,\Round,\Add, \ARMv6

	cmp		voMpeg2Reg0,voMpeg2Reg7
	bne		LABLOOP\Id
	add		sp,sp,#8
	ldmia	sp!, {voMpeg2Reg4 - voMpeg2Reg12, pc}
LABM\Id: .word 0x01010101
	.endif
  .endm


  .macro  LoadVerRow_ARMv6 Id, Pos, Parity, ARMv6
  .if \Parity>0
    @ result is voMpeg2Reg8,voMpeg2Reg9 (voMpeg2Reg10=voMpeg2Reg8>>1,voMpeg2Reg11=voMpeg2Reg9>>1) 
    @ voMpeg2Reg10,voMpeg2Reg11 can be used
    .if \Pos > 0
	  ldr	voMpeg2Reg9,[voMpeg2Reg0,#8]
	  ldr	voMpeg2Reg10,[voMpeg2Reg0,#4]
	  ldr	voMpeg2Reg8,[voMpeg2Reg0],voMpeg2Reg2

	  mov	voMpeg2Reg9,voMpeg2Reg9,lsl #32-\Pos
	  orr	voMpeg2Reg9,voMpeg2Reg9,voMpeg2Reg10,lsr #\Pos
	  mov	voMpeg2Reg8,voMpeg2Reg8,lsr #\Pos	  
	  orr	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg10,lsl #32-\Pos
    .else  
	  @ldrd	voMpeg2Reg8,[voMpeg2Reg0],voMpeg2Reg2
	  ldr   voMpeg2Reg9,[voMpeg2Reg0,#4]
	  ldr   voMpeg2Reg8,[voMpeg2Reg0],voMpeg2Reg2
    .endif
  .else
    @ result is voMpeg2Reg4,voMpeg2Reg5 (voMpeg2Reg6=voMpeg2Reg4>>1,voMpeg2Reg7=voMpeg2Reg5>>1) 
    @ voMpeg2Reg6,voMpeg2Reg7 can be used

    .if \Pos > 0
	  ldr	voMpeg2Reg5,[voMpeg2Reg0,#8]
	  ldr	voMpeg2Reg6,[voMpeg2Reg0,#4]
	  @ldr	voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2
	  ldr	voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2

	  mov	voMpeg2Reg5,voMpeg2Reg5,lsl #32-\Pos
	  orr	voMpeg2Reg5,voMpeg2Reg5,voMpeg2Reg6,lsr #\Pos
	  mov	voMpeg2Reg4,voMpeg2Reg4,lsr #\Pos	  
	  orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6,lsl #32-\Pos
    .else
	  @ldrd	voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2
	  ldr   voMpeg2Reg5,[voMpeg2Reg0, #4]
	  ldr   voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2
    .endif  
  .endif
  .endm

  .macro  CopyVerRow_ARMv6 Id, Pos, Parity, Round, Add, ARMv6

@voMpeg2Reg14 01010101
@voMpeg2Reg12 7f7f7f7f

    LoadVerRow_ARMv6 \Id,\Pos,\Parity, \ARMv6 
    .if \Round>0
    .else
	uqadd8  voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg14
	uqadd8  voMpeg2Reg5, voMpeg2Reg5, voMpeg2Reg14	
    .endif
	
	uhadd8	voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg8
	uhadd8	voMpeg2Reg5, voMpeg2Reg5, voMpeg2Reg9
	
    .if \Add>0
	ldrd	voMpeg2Reg10,[voMpeg2Reg1]

	uqadd8  voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg14
	uqadd8  voMpeg2Reg5, voMpeg2Reg5, voMpeg2Reg14		  
	uhadd8	voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg10
	uhadd8	voMpeg2Reg5, voMpeg2Reg5, voMpeg2Reg11				
    .endif	
	strd	voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3  

  .endm

@------------------------------------------
@ BArmInter8x4HV
@------------------------------------------

@ load needs voMpeg2Reg2,voMpeg2Reg3 for temporary (voMpeg2Reg2 is restored from stack)

  .macro LoadHorVerRow Id, Pos, Parity, ARMv6
  .if \Parity>0

	@read result voMpeg2Reg4,voMpeg2Reg5 and voMpeg2Reg2,voMpeg2Reg3 (one pixel to right)
	@voMpeg2Reg6,voMpeg2Reg7 can be used

    ldr		voMpeg2Reg5,[voMpeg2Reg0,#4]
    ldr		voMpeg2Reg6,[voMpeg2Reg0,#8]
    ldr		voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2

    .if \Pos+8 < 32
	  mov	voMpeg2Reg3,voMpeg2Reg5,lsr #\Pos+8
	  mov	voMpeg2Reg2,voMpeg2Reg4,lsr #\Pos+8
	  orr	voMpeg2Reg3,voMpeg2Reg3,voMpeg2Reg6,lsl #32-\Pos-8
	  orr	voMpeg2Reg2,voMpeg2Reg2,voMpeg2Reg5,lsl #32-\Pos-8
    .else
	  mov	voMpeg2Reg2,voMpeg2Reg5
	  mov	voMpeg2Reg3,voMpeg2Reg6
    .endif

	.if \Pos > 0
	  mov	voMpeg2Reg4,voMpeg2Reg4,lsr #\Pos
	  mov	voMpeg2Reg6,voMpeg2Reg6,lsl #32-\Pos
	  orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg5,lsl #32-\Pos
	  orr	voMpeg2Reg5,voMpeg2Reg6,voMpeg2Reg5,lsr #\Pos
	.endif

	and		voMpeg2Reg6,voMpeg2Reg2,voMpeg2Reg14
	and		voMpeg2Reg2,voMpeg2Reg12,voMpeg2Reg2,lsr #2
	and		voMpeg2Reg7,voMpeg2Reg4,voMpeg2Reg14
	and		voMpeg2Reg4,voMpeg2Reg12,voMpeg2Reg4,lsr #2
	add		voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg2
	add		voMpeg2Reg6,voMpeg2Reg6,voMpeg2Reg7

	and		voMpeg2Reg2,voMpeg2Reg3,voMpeg2Reg14
	and		voMpeg2Reg3,voMpeg2Reg12,voMpeg2Reg3,lsr #2
	and		voMpeg2Reg7,voMpeg2Reg5,voMpeg2Reg14
	and		voMpeg2Reg5,voMpeg2Reg12,voMpeg2Reg5,lsr #2
	add		voMpeg2Reg5,voMpeg2Reg5,voMpeg2Reg3
	add		voMpeg2Reg7,voMpeg2Reg2,voMpeg2Reg7
  .else
	@read result voMpeg2Reg8,voMpeg2Reg9 and voMpeg2Reg2,voMpeg2Reg3 (one pixel to right)
	@voMpeg2Reg10,voMpeg2Reg11 can be used

    ldr		voMpeg2Reg9,[voMpeg2Reg0,#4]
    ldr		voMpeg2Reg10,[voMpeg2Reg0,#8]
    ldr		voMpeg2Reg8,[voMpeg2Reg0],voMpeg2Reg2

    .if \Pos+8 < 32
	  mov	voMpeg2Reg3,voMpeg2Reg9,lsr #\Pos+8
	  mov	voMpeg2Reg2,voMpeg2Reg8,lsr #\Pos+8
	  orr	voMpeg2Reg3,voMpeg2Reg3,voMpeg2Reg10,lsl #32-\Pos-8
	  orr	voMpeg2Reg2,voMpeg2Reg2,voMpeg2Reg9,lsl #32-\Pos-8
    .else
	  mov	voMpeg2Reg2,voMpeg2Reg9
	  mov	voMpeg2Reg3,voMpeg2Reg10
    .endif

	.if \Pos > 0
	  mov	voMpeg2Reg8,voMpeg2Reg8,lsr #\Pos
	  mov	voMpeg2Reg10,voMpeg2Reg10,lsl #32-\Pos
	  orr	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg9,lsl #32-\Pos
	  orr	voMpeg2Reg9,voMpeg2Reg10,voMpeg2Reg9,lsr #\Pos
	.endif

	and		voMpeg2Reg10,voMpeg2Reg2,voMpeg2Reg14
	and		voMpeg2Reg2,voMpeg2Reg12,voMpeg2Reg2,lsr #2
	and		voMpeg2Reg11,voMpeg2Reg8,voMpeg2Reg14
	and		voMpeg2Reg8,voMpeg2Reg12,voMpeg2Reg8,lsr #2
	add		voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg2
	add		voMpeg2Reg10,voMpeg2Reg10,voMpeg2Reg11

	and		voMpeg2Reg2,voMpeg2Reg3,voMpeg2Reg14
	and		voMpeg2Reg3,voMpeg2Reg12,voMpeg2Reg3,lsr #2
	and		voMpeg2Reg11,voMpeg2Reg9,voMpeg2Reg14
	and		voMpeg2Reg9,voMpeg2Reg12,voMpeg2Reg9,lsr #2
	add		voMpeg2Reg9,voMpeg2Reg9,voMpeg2Reg3
	add		voMpeg2Reg11,voMpeg2Reg2,voMpeg2Reg11
  .endif
	ldr		voMpeg2Reg2,[sp]
  .endm

  .macro CopyHorVerRow Id, Pos, Parity, Round, Add, ARMv6

@voMpeg2Reg14 03030303
@voMpeg2Reg12 3f3f3f3f

	LoadHorVerRow \Id,\Pos,\Parity, \ARMv6

	.if \Round>0
	  and voMpeg2Reg3,voMpeg2Reg14,voMpeg2Reg14,lsr #1		@0x01010101
	.else
	  and voMpeg2Reg3,voMpeg2Reg14,voMpeg2Reg14,lsl #1		@0x02020202
	.endif
	
	  add	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg4
	  add	voMpeg2Reg9,voMpeg2Reg9,voMpeg2Reg5
	  add	voMpeg2Reg10,voMpeg2Reg10,voMpeg2Reg6
	  add	voMpeg2Reg11,voMpeg2Reg11,voMpeg2Reg7

	  add	voMpeg2Reg10,voMpeg2Reg10,voMpeg2Reg3
	  add	voMpeg2Reg11,voMpeg2Reg11,voMpeg2Reg3
	  and	voMpeg2Reg10,voMpeg2Reg14,voMpeg2Reg10,lsr #2
	  and	voMpeg2Reg11,voMpeg2Reg14,voMpeg2Reg11,lsr #2

	  .if \Add>0
	    add	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg10 
	    add	voMpeg2Reg9,voMpeg2Reg9,voMpeg2Reg11
		orr	voMpeg2Reg12,voMpeg2Reg12,voMpeg2Reg12,lsl #1  @0x7F7F7F7F
	    ldr	voMpeg2Reg11,[voMpeg2Reg1,#4]
	    ldr	voMpeg2Reg10,[voMpeg2Reg1]
	    and	voMpeg2Reg3,voMpeg2Reg12,voMpeg2Reg9,lsr #1
		orr voMpeg2Reg9,voMpeg2Reg11,voMpeg2Reg9
	    and	voMpeg2Reg11,voMpeg2Reg12,voMpeg2Reg11,lsr #1
		add voMpeg2Reg11,voMpeg2Reg11,voMpeg2Reg3
	    and	voMpeg2Reg3,voMpeg2Reg12,voMpeg2Reg8,lsr #1
		orr voMpeg2Reg8,voMpeg2Reg10,voMpeg2Reg8
	    and	voMpeg2Reg10,voMpeg2Reg12,voMpeg2Reg10,lsr #1
		add voMpeg2Reg10,voMpeg2Reg10,voMpeg2Reg3
		and voMpeg2Reg3,voMpeg2Reg14,voMpeg2Reg14,lsr #1 @0x01010101
		mvn	voMpeg2Reg12,voMpeg2Reg14,lsl #6    @restore voMpeg2Reg12
		and voMpeg2Reg9,voMpeg2Reg9,voMpeg2Reg3
		and voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg3
		add voMpeg2Reg11,voMpeg2Reg11,voMpeg2Reg9
	    str	voMpeg2Reg11,[voMpeg2Reg1,#4]	
	    ldr	voMpeg2Reg11,[sp,#8]  @dstpitch	    	
		add voMpeg2Reg10,voMpeg2Reg10,voMpeg2Reg8
	    ldr	voMpeg2Reg3,[sp,#4]	@end src for loop compare
	    str	voMpeg2Reg10,[voMpeg2Reg1],voMpeg2Reg11
	  .else
	    add	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg10
	    ldr	voMpeg2Reg10,[sp,#8]  @dstpitch
	    add	voMpeg2Reg9,voMpeg2Reg9,voMpeg2Reg11
	    ldr	voMpeg2Reg3,[sp,#4]	@end src for loop compare
	    str	voMpeg2Reg9,[voMpeg2Reg1,#4]
	    str	voMpeg2Reg8,[voMpeg2Reg1],voMpeg2Reg10
	  .endif
  .endm

  .macro BArmInter8x4HV Id, Pos, Round, Add, ARMv6

	.if \ARMv6>0
	ldr		voMpeg2Reg4, [sp, #40]
	sub		sp,sp,#12
	mul		voMpeg2Reg4, voMpeg2Reg2, voMpeg2Reg4
	mov		voMpeg2Reg7, voMpeg2Reg3
	add		voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg0

	@add		voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg2
	mov		voMpeg2Reg2, voMpeg2Reg2, lsr #1
	mov		voMpeg2Reg12, voMpeg2Reg2
	str		voMpeg2Reg4,[sp,#4]	@end src


	ldr		voMpeg2Reg14,LABM_ARMv6\Id

LABLOOP_ARMv6\Id£º
	LoadHorVerRow_ARMv6 \Id,\Pos,0, \ARMv6
	CopyHorVerRow_ARMv6 \Id,\Pos,1,\Round,\Add, \ARMv6
	cmp		voMpeg2Reg0,voMpeg2Reg3
	bne		LABLOOP_ARMv6\Id

	add		sp,sp,#12
	ldmia	sp!, {voMpeg2Reg4 - voMpeg2Reg12, pc}
LABM_ARMv6\Id: .word 0x01010101
		  
	  .else
	ldr		voMpeg2Reg4, [sp, #40]
	sub		sp,sp,#12
	mul		voMpeg2Reg4, voMpeg2Reg2, voMpeg2Reg4
	str		voMpeg2Reg3,[sp,#8]	@dstpitch
	add		voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg0
	
	@add		voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg2
	mov		voMpeg2Reg2, voMpeg2Reg2, lsr #1
	str		voMpeg2Reg2,[sp]		@srcpitch
	str		voMpeg2Reg4,[sp,#4]	@end src


	ldr		voMpeg2Reg14,LABM\Id
	mvn		voMpeg2Reg12,voMpeg2Reg14,lsl #6
	
LABLOOP\Id:
	LoadHorVerRow \Id,\Pos,0, \ARMv6
	CopyHorVerRow \Id,\Pos,1,\Round,\Add, \ARMv6
	cmp		voMpeg2Reg0,voMpeg2Reg3
	bne		LABLOOP\Id

	add		sp,sp,#12
	ldmia	sp!, {voMpeg2Reg4 - voMpeg2Reg12, pc}
LABM\Id: .word 0x03030303
	.endif
  .endm


@ load needs voMpeg2Reg2,voMpeg2Reg3 for temporary (voMpeg2Reg2 is restored from stack)

  .macro LoadHorVerRow_ARMv6 Id, Pos, Parity, ARMv6
  
@	ldr		voMpeg2Reg2,[sp]
	  
  .if \Parity>0
	@read result voMpeg2Reg4,voMpeg2Reg5 and voMpeg2Reg2,voMpeg2Reg3 (one pixel to right)
	@voMpeg2Reg6,voMpeg2Reg7 can be used

    ldr		voMpeg2Reg5,[voMpeg2Reg0,#4]
    ldr		voMpeg2Reg6,[voMpeg2Reg0,#8]
    ldr		voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg12

    .if \Pos+8 < 32
	  mov	voMpeg2Reg3,voMpeg2Reg5,lsr #\Pos+8
	  orr	voMpeg2Reg3,voMpeg2Reg3,voMpeg2Reg6,lsl #32-\Pos-8
	  mov	voMpeg2Reg2,voMpeg2Reg4,lsr #\Pos+8	  
	  orr	voMpeg2Reg2,voMpeg2Reg2,voMpeg2Reg5,lsl #32-\Pos-8
    .else
	  mov	voMpeg2Reg2,voMpeg2Reg5
	  mov	voMpeg2Reg3,voMpeg2Reg6
    .endif

	.if \Pos > 0
	  mov	voMpeg2Reg4,voMpeg2Reg4,lsr #\Pos
	  mov	voMpeg2Reg6,voMpeg2Reg6,lsl #32-\Pos
	  orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg5,lsl #32-\Pos
	  orr	voMpeg2Reg5,voMpeg2Reg6,voMpeg2Reg5,lsr #\Pos
	.endif
	uhadd8	voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg2
	uhadd8	voMpeg2Reg5, voMpeg2Reg5, voMpeg2Reg3
  .else
	@read result voMpeg2Reg8,voMpeg2Reg9 and voMpeg2Reg2,voMpeg2Reg3 (one pixel to right)
	@voMpeg2Reg10,voMpeg2Reg11 can be used

    ldr		voMpeg2Reg9,[voMpeg2Reg0,#4]
    ldr		voMpeg2Reg10,[voMpeg2Reg0,#8]
    ldr		voMpeg2Reg8,[voMpeg2Reg0],voMpeg2Reg12

    .if \Pos+8 < 32
	  mov	voMpeg2Reg3,voMpeg2Reg9,lsr #\Pos+8
	  orr	voMpeg2Reg3,voMpeg2Reg3,voMpeg2Reg10,lsl #32-\Pos-8
	  mov	voMpeg2Reg2,voMpeg2Reg8,lsr #\Pos+8	  
	  orr	voMpeg2Reg2,voMpeg2Reg2,voMpeg2Reg9,lsl #32-\Pos-8
    .else
	  mov	voMpeg2Reg2,voMpeg2Reg9
	  mov	voMpeg2Reg3,voMpeg2Reg10
    .endif

	.if \Pos > 0
	  mov	voMpeg2Reg8,voMpeg2Reg8,lsr #\Pos
	  mov	voMpeg2Reg10,voMpeg2Reg10,lsl #32-\Pos
	  orr	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg9,lsl #32-\Pos
	  orr	voMpeg2Reg9,voMpeg2Reg10,voMpeg2Reg9,lsr #\Pos
	.endif
	uhadd8	voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg2
	uhadd8	voMpeg2Reg9, voMpeg2Reg9, voMpeg2Reg3	
  .endif
  .endm

  .macro CopyHorVerRow_ARMv6 Id, Pos, Parity, Round, Add, ARMv6

@voMpeg2Reg14 03030303
@voMpeg2Reg12 3f3f3f3f

	LoadHorVerRow_ARMv6 \Id,\Pos,\Parity, \ARMv6

	.if \Round>0
	  mov voMpeg2Reg3,voMpeg2Reg14				@0x01010101
	.else
	  mov voMpeg2Reg3, voMpeg2Reg14, lsl #1		@0x02020202
	.endif 
		
	uqadd8  voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg3
	uqadd8  voMpeg2Reg9, voMpeg2Reg9, voMpeg2Reg3	
		   
	uhadd8	voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg4
	uhadd8	voMpeg2Reg9, voMpeg2Reg9, voMpeg2Reg5
@	    ldr	voMpeg2Reg2,[sp,#8]  @dstpitch
	    ldr	voMpeg2Reg3,[sp,#4]	@end src for loop compare 	    
	  .if \Add>0
	  ldrd	voMpeg2Reg10,[voMpeg2Reg1]
	.if \Round>0
	.else
	uqadd8  voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg14
	uqadd8  voMpeg2Reg9, voMpeg2Reg9, voMpeg2Reg14	
	.endif				  
	uhadd8	voMpeg2Reg8, voMpeg2Reg10, voMpeg2Reg8
	uhadd8	voMpeg2Reg9, voMpeg2Reg11, voMpeg2Reg9
	.endif		  
	  strd	voMpeg2Reg8,[voMpeg2Reg1],voMpeg2Reg7 
  .endm

@----------------------------------------------------------------------------------------
@ general unaligned copy (use PldA)
@
@	GenCodeCpy BArmCopy4x8,BArmCopy8x4,0,0,0,1,0
@	GenCodeCpy BArmCopy8x16,BArmCopy16x8,0,0,0,1,0
@---------------------------------------------------------------------------------------
@---------------------------------------------------
@ smaller versions without PldA
@
@	GenCodeCpy BArmInter8x4,BArmInter8x4,0,0,1,0
@	GenCodeCpy BArmInter8x4V,BArmInter8x4V,0,0,0,0
@	GenCodeCpy BArmInter8x4H,BArmInter8x4H,0,0,0,0
@	GenCodeCpy BArmInter8x4HV,BArmInter8x4HV,0,0,0,0
@
@	GenCodeCpy BArmInter2_8x4 ,BArmInter2_8x4 ,0,1,0,0
@	GenCodeCpy BArmInter2_8x4V,BArmInter8x4V,0,1,0,0
@	GenCodeCpy BArmInter2_8x4H,BArmInter8x4H,0,1,0,0
@	GenCodeCpy BArmInter2_8x4HV,BArmInter8x4HV,0,1,0,0
@
@---------------------------------------------------
@ smaller versions with PldA
@
@	GenCodeCpy PldAarm_copyblk,BArmInter8x4,0,0,1,1
@	GenCodeCpy PldAarm_copyblkv,BArmInter8x4V,0,0,0,1
@	GenCodeCpy PldAarm_copyblkh,BArmInter8x4H,0,0,0,1
@	GenCodeCpy PldAarm_copyblkhv,BArmInter8x4HV,0,0,0,1
@
@	GenCodeCpy PldAarm_addblk,BArmInter2_8x4 ,0,1,0,1
@	GenCodeCpy PldAarm_addblkv,BArmInter8x4V,0,1,0,1
@	GenCodeCpy PldAarm_addblkh,BArmInter8x4H,0,1,0,1
@	GenCodeCpy PldAarm_addblkhv,BArmInter8x4HV,0,1,0,1
@
@---------------------------------------------------
@ larger versions with PldA
@ (faster if there is enough intstruction cache available)
@----------------------------------------------------------------------------------------------------------------------------------------------------------------
@   GenCodeCpy		\Name,			\Sub,		\Round,	\Add,	\Fast,	\ARM5,	\ARMv6,	\ARM11
@
@	GenCodeCpy BArmInter8x4,	BArmInter8x4,	0,	0,	1,	1,	1,	0
@	GenCodeCpy BArmInter8x4V,	BArmInter8x4V,	0,	0,	1,	1,	1,	0
@	GenCodeCpy BArmInter8x4H,	BArmInter8x4H,	0,	0,	1,	1,	1,	0
@	GenCodeCpy BArmInter8x4HV,	BArmInter8x4HV,	0,	0,	1,	1,	1,	0
@                                                                 	  	  	  	
@	GenCodeCpy BArmInter2_8x4 ,	BArmInter2_8x4 ,0,	1,	1,	1,  1,	0
@	GenCodeCpy BArmInter2_8x4V,	BArmInter8x4V,	0,	1,	1,	1,	1,	0
@	GenCodeCpy BArmInter2_8x4H,	BArmInter8x4H,	0,	1,	1,	1,	1,	0
@	GenCodeCpy BArmInter2_8x4HV,BArmInter8x4HV,	0,	1,	1,	1,  1,	0
@-------------------------------------------------------------------------------------------------------------------------------------------------------------------	
	@   GenCodeCpy		\Name,			\Sub,		\Round,	\Add,	\Fast,	\ARM5,	\ARMv6,	\ARM11
	
	@GenCodeCpy __voMPEG2D0223,	ArmCopy8x4,				0,	0,	0,	0,	1,	0
	@GenCodeCpy __voMPEG2D0239,	BArmCopy16x8,			0,	0,	0,	0,	1,	0				
	@GenCodeCpy __voMPEG2D0240,	BArmCopy8x4,			0,	0,	0,	0,	1,	0

	GenCodeCpy __voMPEG2D0214,	BArmInter8x4,			0,	0,	1,	1,	1,	0
	GenCodeCpy __voMPEG2D0216,	BArmInter8x4V,			0,	0,	1,	1,	1,	0
	GenCodeCpy __voMPEG2D0215,	BArmInter8x4H,			0,	0,	1,	1,	1,	0
	GenCodeCpy __voMPEG2D0217,	BArmInter8x4HV,         0,	0,	1,	1,	0,	0
                                                                  	  	  	  	
	GenCodeCpy __voMPEG2D0218 ,	BArmInter2_8x4 ,		0,	1,	1,	1,	1,	0
	GenCodeCpy __voMPEG2D0220,	BArmInter8x4V,			0,	1,	1,	1,	1,	0
	GenCodeCpy __voMPEG2D0219,	BArmInter8x4H,			0,	1,	1,	1,	1,	0
	GenCodeCpy __voMPEG2D0221,	BArmInter8x4HV,         0,	1,	1,	1,	0,	0

	@END
