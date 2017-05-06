@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2005		            *
@								 	                                    *
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

  .macro PldA ARM5,Pos
    .if \ARM5>0
	  .if \Pos >= 0
  	    .if \Pos > 0
	      pld [voMpeg2Reg0,voMpeg2Reg2,lsl #1]
	      add voMpeg2Reg0,voMpeg2Reg0,#8
	      pld [voMpeg2Reg0,voMpeg2Reg2,lsl #1]
	      sub voMpeg2Reg0,voMpeg2Reg0,#8
	     .else
	      pld [voMpeg2Reg0,voMpeg2Reg2,lsl #1]
	      add voMpeg2Reg0,voMpeg2Reg0,#4
	      pld [voMpeg2Reg0,voMpeg2Reg2,lsl #1]
	      sub voMpeg2Reg0,voMpeg2Reg0,#4
	    .endif
	  .else
	    pld [voMpeg2Reg0,voMpeg2Reg2,lsl #1]

	    add voMpeg2Reg0,voMpeg2Reg0,#7
	    pld [voMpeg2Reg0,voMpeg2Reg2,lsl #1]
	    sub voMpeg2Reg0,voMpeg2Reg0,#7
	  .endif
	.endif
  .endm

  .macro PldBI ARM5,Pos
    .if \ARM5>0
	  .if \Pos >= 0
  	    .if \Pos > 0
	      add voMpeg2Reg10,voMpeg2Reg2,voMpeg2Reg2
	      add voMpeg2Reg10,voMpeg2Reg10,#8
	    .else
	      add voMpeg2Reg10,voMpeg2Reg2,voMpeg2Reg2
	      add voMpeg2Reg10,voMpeg2Reg10,#4
	    .endif
	  .else
	      add voMpeg2Reg10,voMpeg2Reg2,voMpeg2Reg2
	      add voMpeg2Reg10,voMpeg2Reg10,#7
	  .endif
	.endif
  .endm

  .macro PldB ARM5,Pos
	.if \ARM5>0
      pld [voMpeg2Reg0,voMpeg2Reg2,lsl #1]
      pld [voMpeg2Reg0,voMpeg2Reg10]
	.endif
  .endm

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
@ArmInter4x8
@------------------------------------------

  .macro CpyBlkRArm ARMv6,Pos
	.if \Pos > 0

	  ldr	voMpeg2Reg6,[voMpeg2Reg0,#4]
	  ldr	voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2
		
	  mov	voMpeg2Reg4,voMpeg2Reg4,lsr #\Pos
	  orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6,lsl #32-\Pos

	  str	voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3
	.else
		  ldr	voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2
		  str	voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3
	.endif
  .endm

  .macro ArmInter4x8 Id, Pos, Round, Add, ARMv6

	ldr		voMpeg2Reg12, [sp, #40]
loop_again\Id:
	pld [voMpeg2Reg0,voMpeg2Reg2]
	CpyBlkRArm \ARMv6,\Pos 
	subs	voMpeg2Reg12, voMpeg2Reg12, #1
	bne		loop_again\Id


@	PldBI \ARM5,\Pos
@	PldB \ARM5,\Pos
@	CpyBlkRArm \Pos 
@	PldB \ARM5,\Pos
@	CpyBlkRArm \Pos 
@	PldB \ARM5,\Pos
@	CpyBlkRArm \Pos 
@	PldB \ARM5,\Pos
@	CpyBlkRArm \Pos 
@	PldB \ARM5,\Pos
@	CpyBlkRArm \Pos 
@	PldB \ARM5,\Pos
@	CpyBlkRArm \Pos 
@	PldB \ARM5,\Pos
@	CpyBlkRArm \Pos 
@	PldB \ARM5,\Pos
@	CpyBlkRArm \Pos 

	ldmia	sp!, {voMpeg2Reg4 - voMpeg2Reg12, pc}
  .endm

@------------------------------------------
@CpyBlkMArm
@------------------------------------------

  .macro CpyBlkMRArm ARMv6,Pos
	.if \Pos > 0

	  ldr	voMpeg2Reg14,[voMpeg2Reg0,#16]
	  ldr	voMpeg2Reg6,[voMpeg2Reg0,#12]
	  ldr	voMpeg2Reg12,[voMpeg2Reg0,#8]
		
	  mov	voMpeg2Reg14,voMpeg2Reg14,lsl #32-\Pos
	  mov	voMpeg2Reg12,voMpeg2Reg12,lsr #\Pos
	  orr	voMpeg2Reg14,voMpeg2Reg14,voMpeg2Reg6,lsr #\Pos
	  orr	voMpeg2Reg12,voMpeg2Reg12,voMpeg2Reg6,lsl #32-\Pos

	  ldr	voMpeg2Reg5,[voMpeg2Reg0,#8]
	  ldr	voMpeg2Reg6,[voMpeg2Reg0,#4]
	  ldr	voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2
		
	  mov	voMpeg2Reg5,voMpeg2Reg5,lsl #32-\Pos
	  mov	voMpeg2Reg4,voMpeg2Reg4,lsr #\Pos
	  orr	voMpeg2Reg5,voMpeg2Reg5,voMpeg2Reg6,lsr #\Pos
	  orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6,lsl #32-\Pos
	.else
	.if \ARMv6 > 0
	ldrd	voMpeg2Reg6,[voMpeg2Reg0,#8]
	ldrd	voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2
	.else
	  ldr	voMpeg2Reg5,[voMpeg2Reg0,#4]
	  ldr	voMpeg2Reg12,[voMpeg2Reg0,#8]
	  ldr	voMpeg2Reg14,[voMpeg2Reg0,#12]
	  ldr	voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2
	.endif
	.endif
	.if \ARMv6 > 0
	strd	voMpeg2Reg6,[voMpeg2Reg1,#8]
	strd	voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3
	.else
	str		voMpeg2Reg5,[voMpeg2Reg1,#4]
	str		voMpeg2Reg12,[voMpeg2Reg1,#8]
	str		voMpeg2Reg14,[voMpeg2Reg1,#12]
	str		voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3
	.endif
  .endm

  .macro CpyBlkMArm Id, Pos, Round, Add, ARMv6

@	PldBI \ARM5,\Pos

	ldr		voMpeg2Reg11, [sp, #40]

@	mov		voMpeg2Reg11,#16
LABLOOP\Id:
	pld [voMpeg2Reg0,voMpeg2Reg2]
	CpyBlkMRArm \ARMv6,\Pos
	subs voMpeg2Reg11,voMpeg2Reg11,#1
	bne  LABLOOP\Id

	ldmia	sp!, {voMpeg2Reg4 - voMpeg2Reg12, pc}
  .endm

@------------------------------------------
@ArmCopy8x16: no aligment!, only used in Copy()
@------------------------------------------

  .macro arm_copyblk8x16Row ARMv6
    .if \ARMv6>0
		ldrd	voMpeg2Reg6,[voMpeg2Reg0], voMpeg2Reg2
		strd	voMpeg2Reg6,[voMpeg2Reg1], voMpeg2Reg3
	.else
		ldr		voMpeg2Reg5,[voMpeg2Reg0,#4]
		ldr		voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2
		str		voMpeg2Reg5,[voMpeg2Reg1,#4]
		str		voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3
	.endif

  .endm

  .macro ArmCopy8x16 Id, Pos, Round, Add, ARMv6


	ldr		voMpeg2Reg11, [sp, #40]

@	mov		voMpeg2Reg11,#15

LABLOOP\Id:
	pld [voMpeg2Reg0,voMpeg2Reg2]
	arm_copyblk8x16Row \ARMv6
	subs voMpeg2Reg11,voMpeg2Reg11,#1 
	bne  LABLOOP\Id

	@unroll last (no PldA needed)
	arm_copyblk8x16Row \ARMv6
	ldmia	sp!, {voMpeg2Reg4 - voMpeg2Reg12, pc}
  .endm

@------------------------------------------
@ArmCopy4x8: no aligment!, only used in Copy()
@------------------------------------------

  .macro ArmCopy4x8 Id, Pos, Round, Add, ARMv6

	mov		voMpeg2Reg11,#3

LABLOOP\Id:
	ldr		voMpeg2Reg6,[voMpeg2Reg0],voMpeg2Reg2
	ldr		voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2
	str		voMpeg2Reg6,[voMpeg2Reg1],voMpeg2Reg3
	str		voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3
	subs		voMpeg2Reg11,voMpeg2Reg11,#1 
	bne		LABLOOP\Id

	@unroll last (no PldA needed)
	ldr		voMpeg2Reg6,[voMpeg2Reg0],voMpeg2Reg2
	ldr		voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2
	str		voMpeg2Reg6,[voMpeg2Reg1],voMpeg2Reg3
	str		voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3

	ldmia	sp!, {voMpeg2Reg4 - voMpeg2Reg12, pc}
  .endm

@------------------------------------------
@ArmInter2_4x8 
@------------------------------------------

  .macro arm_addblkRow Pos, ARMv6

	.if \Pos < 0
	  ldrb	voMpeg2Reg4,[voMpeg2Reg0]
	  ldrb	voMpeg2Reg6,[voMpeg2Reg0,#1]
	  orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6,lsl #8
	  ldrb	voMpeg2Reg6,[voMpeg2Reg0,#2]
	  orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6,lsl #16
	  ldrb	voMpeg2Reg6,[voMpeg2Reg0,#3]
	  orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6,lsl #24
	  add	voMpeg2Reg0,voMpeg2Reg0,voMpeg2Reg2
	.else
	  .if \Pos > 0
	    ldr	voMpeg2Reg6,[voMpeg2Reg0,#4]
	    ldr	voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2
		mov	voMpeg2Reg4,voMpeg2Reg4,lsr #\Pos
	    orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6,lsl #32-\Pos
	  .else
	    ldr	voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2
	  .endif
	.endif
	  .if \ARMv6 > 0
		ldr		voMpeg2Reg6,[voMpeg2Reg1]  
		uqadd8  voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg14
		uhadd8	voMpeg2Reg6, voMpeg2Reg6, voMpeg2Reg4
		str		voMpeg2Reg6,[voMpeg2Reg1], voMpeg2Reg3 
	  .else
		ldr		voMpeg2Reg6,[voMpeg2Reg1]
		and		voMpeg2Reg8,voMpeg2Reg12,voMpeg2Reg4,lsr #1
		orr		voMpeg2Reg4,voMpeg2Reg6,voMpeg2Reg4
		and		voMpeg2Reg6,voMpeg2Reg12,voMpeg2Reg6,lsr #1
		add		voMpeg2Reg6,voMpeg2Reg6,voMpeg2Reg8
		and		voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg14
		add		voMpeg2Reg6,voMpeg2Reg6,voMpeg2Reg4
		str		voMpeg2Reg6,[voMpeg2Reg1],voMpeg2Reg3
	  .endif
  .endm

  .macro ArmInter2_4x8  Id, Pos, Round, Add, ARMv6

@	PldBI \ARM5,\Pos
	ldr		voMpeg2Reg14,LABM\Id

	ldr		voMpeg2Reg11, [sp, #40]

@	mov		voMpeg2Reg11,#8
	mvn		voMpeg2Reg12,voMpeg2Reg14,lsl #7

LABLOOP\Id:
	pld [voMpeg2Reg0,voMpeg2Reg2]
	arm_addblkRow \Pos, \ARMv6
	subs voMpeg2Reg11,voMpeg2Reg11,#1 
	bne  LABLOOP\Id

	ldmia	sp!, {voMpeg2Reg4 - voMpeg2Reg12, pc}
LABM\Id: .word 0x01010101
  .endm

@------------------------------------------
@ ArmInter4x8H
@------------------------------------------

  .macro LoadHorRow Id, Pos

    @ result is voMpeg2Reg4,voMpeg2Reg5 and voMpeg2Reg8,voMpeg2Reg9 (one pixel to the right)
    @ voMpeg2Reg6,voMpeg2Reg7 can be used

	.if \Pos < 0

	ldrb	voMpeg2Reg4,[voMpeg2Reg0]
	ldrb	voMpeg2Reg6,[voMpeg2Reg0,#1]
	ldrb	voMpeg2Reg5,[voMpeg2Reg0,#4]
	orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6,lsl #8
	ldrb	voMpeg2Reg6,[voMpeg2Reg0,#2]
	orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6,lsl #16
	ldrb	voMpeg2Reg6,[voMpeg2Reg0,#3]
	orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6,lsl #24

	mov		voMpeg2Reg8,voMpeg2Reg4,lsr #8
	orr		voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg5,lsl #24

	add		voMpeg2Reg0,voMpeg2Reg0,voMpeg2Reg2

	.else

    ldr		voMpeg2Reg5,[voMpeg2Reg0,#4]
    ldr		voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2

    .if \Pos+8 < 32
	  mov	voMpeg2Reg8,voMpeg2Reg4,lsr #\Pos+8
	  orr	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg5,lsl #32-\Pos-8
    .else
	  mov	voMpeg2Reg8,voMpeg2Reg5
    .endif

	.if \Pos > 0
	  mov	voMpeg2Reg4,voMpeg2Reg4,lsr #\Pos
	  orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg5,lsl #32-\Pos
	.endif
	.endif
  .endm

  .macro CopyHorRow Id, Pos, Round, Add, ARMv6

@voMpeg2Reg14 01010101
@voMpeg2Reg12 7f7f7f7f

	LoadHorRow	\Id,\Pos
	  .if \ARMv6>0
	
	    .if \Round>0
	    .else
	      uqadd8  voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg14
	    .endif
	    
	    uhadd8	voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg8
	
	    .if \Add>0
	       ldr	voMpeg2Reg6,[voMpeg2Reg1]
	       uqadd8  voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg14	  
	       uhadd8	voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg6	
	    .endif	
	    str	voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3
	  .else
    	and		voMpeg2Reg6,voMpeg2Reg12,voMpeg2Reg4,lsr #1
	      .if \Round>0
	         and	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg8
	      .else
	         orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg8
	      .endif
	      and		voMpeg2Reg8,voMpeg2Reg12,voMpeg2Reg8,lsr #1
	      and		voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg14
	      add		voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6
	      add		voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg8
	      .if \Add>0
	         ldr	voMpeg2Reg6,[voMpeg2Reg1]
	         and	voMpeg2Reg8,voMpeg2Reg12,voMpeg2Reg4,lsr #1
	         orr	voMpeg2Reg4,voMpeg2Reg6,voMpeg2Reg4
	         and	voMpeg2Reg6,voMpeg2Reg12,voMpeg2Reg6,lsr #1
	         add	voMpeg2Reg6,voMpeg2Reg6,voMpeg2Reg8
	         and	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg14
	         add	voMpeg2Reg6,voMpeg2Reg6,voMpeg2Reg4
	         str	voMpeg2Reg6,[voMpeg2Reg1],voMpeg2Reg3
	      .else
	      str	voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3
	      .endif
	    .endif
	
  .endm

  .macro ArmInter4x8H Id, Pos, Round, Add, ARMv6

@	PldBI \ARM5,\Pos
	ldr		voMpeg2Reg14,LABM\Id

	ldr		voMpeg2Reg11, [sp, #40]

@	mov		voMpeg2Reg11,#8
	mvn		voMpeg2Reg12,voMpeg2Reg14,lsl #7

LABLOOP\Id:
	pld [voMpeg2Reg0,voMpeg2Reg2]
	CopyHorRow \Id,\Pos,\Round,\Add, \ARMv6
	subs voMpeg2Reg11,voMpeg2Reg11,#1
	bne  LABLOOP\Id

	ldmia	sp!, {voMpeg2Reg4 - voMpeg2Reg12, pc}
LABM\Id: .word 0x01010101
  .endm

@------------------------------------------
@ ArmInter4x8V
@------------------------------------------

  .macro LoadVerRow Id, Pos, Parity
  .if \Parity>0
    @ result is voMpeg2Reg8,voMpeg2Reg9 (voMpeg2Reg10=voMpeg2Reg8>>1,voMpeg2Reg11=voMpeg2Reg9>>1) 
    @ voMpeg2Reg10,voMpeg2Reg11 can be used

	.if \Pos < 0
 	  ldrb	voMpeg2Reg8,[voMpeg2Reg0]
	  ldrb	voMpeg2Reg10,[voMpeg2Reg0,#1]
	  orr	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg10,lsl #8 
	  ldrb	voMpeg2Reg10,[voMpeg2Reg0,#2]
	  orr	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg10,lsl #16
	  ldrb	voMpeg2Reg10,[voMpeg2Reg0,#3]
	  orr	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg10,lsl #24
	  add	voMpeg2Reg0,voMpeg2Reg0,voMpeg2Reg2
	.else
    .if \Pos > 0
	  ldr	voMpeg2Reg10,[voMpeg2Reg0,#4]
	  ldr	voMpeg2Reg8,[voMpeg2Reg0],voMpeg2Reg2

	  mov	voMpeg2Reg8,voMpeg2Reg8,lsr #\Pos
	  orr	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg10,lsl #32-\Pos
    .else
	  ldr	voMpeg2Reg8,[voMpeg2Reg0],voMpeg2Reg2
    .endif
	.endif
	and		voMpeg2Reg10,voMpeg2Reg12,voMpeg2Reg8,lsr #1
  .else
    @ result is voMpeg2Reg4,voMpeg2Reg5 (voMpeg2Reg6=voMpeg2Reg4>>1,voMpeg2Reg7=voMpeg2Reg5>>1) 
    @ voMpeg2Reg6,voMpeg2Reg7 can be used

	.if \Pos < 0
 	  ldrb	voMpeg2Reg4,[voMpeg2Reg0]
	  ldrb	voMpeg2Reg6,[voMpeg2Reg0,#1]
	  orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6,lsl #8
	  ldrb	voMpeg2Reg6,[voMpeg2Reg0,#2]
	  orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6,lsl #16
	  ldrb	voMpeg2Reg6,[voMpeg2Reg0,#3]
	  orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6,lsl #24
	  add	voMpeg2Reg0,voMpeg2Reg0,voMpeg2Reg2
	.else
    .if \Pos > 0
	  ldr	voMpeg2Reg6,[voMpeg2Reg0,#4]
	  ldr	voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2

	  mov	voMpeg2Reg4,voMpeg2Reg4,lsr #\Pos
	  orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6,lsl #32-\Pos
    .else
	  ldr	voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2
    .endif
	.endif
	
	and		voMpeg2Reg6,voMpeg2Reg12,voMpeg2Reg4,lsr #1
  .endif
  .endm

  .macro CopyVerRow Id, Pos, Parity, Round, Add, ARMv6

@voMpeg2Reg14 01010101
@voMpeg2Reg12 7f7f7f7f

	LoadVerRow \Id,\Pos,\Parity
	  .if \ARMv6>0 
        .if \Parity>0   
	      .if \Round>0
	      .else
	        uqadd8  voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg14
	      .endif	
	      uhadd8	voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg8	
	      .if \Add>0
	        ldr	voMpeg2Reg10,[voMpeg2Reg1]
	        uqadd8  voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg14  
	        uhadd8	voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg10			
	      .endif	
	        str	voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3  
	    .else
	      .if \Round>0
	      .else
	        uqadd8  voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg14
	      .endif
	      	
	      uhadd8	voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg4
	      .if \Add>0
	        ldr	voMpeg2Reg10,[voMpeg2Reg1]

	        uqadd8  voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg14	  
	        uhadd8	voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg10		
	      .endif		
	      str	voMpeg2Reg8,[voMpeg2Reg1],voMpeg2Reg3  
	    .endif
    .else

    .if \Parity>0
	  .if \Round>0
	    and	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg8
	  .else
	    orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg8
	  .endif
	  and	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg14

	  add	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6

  	  add	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg10

	  .if \Add>0
	    ldr	voMpeg2Reg6,[voMpeg2Reg1]
	    and	voMpeg2Reg3,voMpeg2Reg12,voMpeg2Reg4,lsr #1
	    orr voMpeg2Reg4,voMpeg2Reg6,voMpeg2Reg4
	    and	voMpeg2Reg6,voMpeg2Reg12,voMpeg2Reg6,lsr #1
	    add voMpeg2Reg6,voMpeg2Reg6,voMpeg2Reg3
	    and voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg14
	    add voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6
	    ldr	voMpeg2Reg3,[sp, #4]
	    ldr	voMpeg2Reg7,[sp]		@end src for loop compare
	    str	voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3
	  .else
	    ldr	voMpeg2Reg7,[sp]		@end src for loop compare
	    str	voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3
	  .endif
	.else
	  .if \Round>0
	    and	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg4
	  .else
	    orr	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg4
	  .endif
	  and	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg14

	  add	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg10
  	  add	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg6

	  .if \Add>0
	    ldr	voMpeg2Reg10,[voMpeg2Reg1]
	    and	voMpeg2Reg3,voMpeg2Reg12,voMpeg2Reg8,lsr #1
	    orr voMpeg2Reg8,voMpeg2Reg10,voMpeg2Reg8
	    and	voMpeg2Reg10,voMpeg2Reg12,voMpeg2Reg10,lsr #1
	    add voMpeg2Reg10,voMpeg2Reg10,voMpeg2Reg3
	    and voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg14
	    ldr	voMpeg2Reg3,[sp, #4]		
	    add voMpeg2Reg10,voMpeg2Reg10,voMpeg2Reg8
	    str	voMpeg2Reg10,[voMpeg2Reg1],voMpeg2Reg3
	  .else
	    str	voMpeg2Reg8,[voMpeg2Reg1],voMpeg2Reg3
	  .endif
	.endif
   .endif
  .endm


  .macro ArmInter4x8V Id, Pos, Round, Add, ARMv6

	ldr		voMpeg2Reg4, [sp, #40]
	sub		sp,sp,#8
	mul		voMpeg2Reg4, voMpeg2Reg2, voMpeg2Reg4
	ldr		voMpeg2Reg14,LABM\Id
	add		voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg0

@	add		voMpeg2Reg4,voMpeg2Reg0,voMpeg2Reg2,lsl #3
	add		voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg2
	mov		voMpeg2Reg7,voMpeg2Reg4
	str		voMpeg2Reg4,[sp]		@end src
	str		voMpeg2Reg3,[sp, #4]	@end src	


	mvn		voMpeg2Reg12,voMpeg2Reg14,lsl #7

	pld [voMpeg2Reg0,voMpeg2Reg2]
	LoadVerRow \Id,\Pos,1
LABLOOP\Id:
	pld [voMpeg2Reg0,voMpeg2Reg2]
	CopyVerRow \Id,\Pos,0,\Round,\Add, \ARMv6
	pld [voMpeg2Reg0,voMpeg2Reg2]
	CopyVerRow \Id,\Pos,1,\Round,\Add, \ARMv6

	cmp		voMpeg2Reg0,voMpeg2Reg7
	bne		LABLOOP\Id
	add		sp,sp,#8
	ldmia	sp!, {voMpeg2Reg4 - voMpeg2Reg12, pc}
LABM\Id: .word 0x01010101
  .endm

@------------------------------------------
@ ArmInter4x8HV
@------------------------------------------

@ load needs voMpeg2Reg2,voMpeg2Reg3 for temporary (voMpeg2Reg2 is restored from stack)

  .macro LoadHorVerRow Id, Pos, Parity
  .if \Parity>0

	@read result voMpeg2Reg4,voMpeg2Reg5 and voMpeg2Reg2,voMpeg2Reg3 (one pixel to right)
	@voMpeg2Reg6,voMpeg2Reg7 can be used

    .if \Pos<0

	ldrb	voMpeg2Reg4,[voMpeg2Reg0]
	ldrb	voMpeg2Reg6,[voMpeg2Reg0,#1]
	ldrb	voMpeg2Reg5,[voMpeg2Reg0,#4]
	orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6,lsl #8
	ldrb	voMpeg2Reg6,[voMpeg2Reg0,#2]
	orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6,lsl #16
	ldrb	voMpeg2Reg6,[voMpeg2Reg0,#3]
	orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6,lsl #24
	add	voMpeg2Reg0,voMpeg2Reg0,voMpeg2Reg2

	mov		voMpeg2Reg2,voMpeg2Reg4,lsr #8
	orr		voMpeg2Reg2,voMpeg2Reg2,voMpeg2Reg5,lsl #24

	.else
    ldr		voMpeg2Reg5,[voMpeg2Reg0,#4]
    ldr		voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg2

    .if \Pos+8 < 32
	  mov	voMpeg2Reg2,voMpeg2Reg4,lsr #\Pos+8
	  orr	voMpeg2Reg2,voMpeg2Reg2,voMpeg2Reg5,lsl #32-\Pos-8
    .else
	  mov	voMpeg2Reg2,voMpeg2Reg5
    .endif

	.if \Pos > 0
	  mov	voMpeg2Reg4,voMpeg2Reg4,lsr #\Pos
	  orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg5,lsl #32-\Pos
	.endif
	.endif

	and		voMpeg2Reg6,voMpeg2Reg2,voMpeg2Reg14
	and		voMpeg2Reg2,voMpeg2Reg12,voMpeg2Reg2,lsr #2
	and		voMpeg2Reg7,voMpeg2Reg4,voMpeg2Reg14
	and		voMpeg2Reg4,voMpeg2Reg12,voMpeg2Reg4,lsr #2
	add		voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg2
	add		voMpeg2Reg6,voMpeg2Reg6,voMpeg2Reg7

  .else
	@read result voMpeg2Reg8,voMpeg2Reg9 and voMpeg2Reg2,voMpeg2Reg3 (one pixel to right)
	@voMpeg2Reg10,voMpeg2Reg11 can be used

    .if \Pos<0

	ldrb	voMpeg2Reg8,[voMpeg2Reg0]
	ldrb	voMpeg2Reg10,[voMpeg2Reg0,#1]
	ldrb	voMpeg2Reg9,[voMpeg2Reg0,#4]
	orr	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg10,lsl #8
	ldrb	voMpeg2Reg10,[voMpeg2Reg0,#2]
	orr	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg10,lsl #16
	ldrb	voMpeg2Reg10,[voMpeg2Reg0,#3]
	orr	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg10,lsl #24
	add	voMpeg2Reg0,voMpeg2Reg0,voMpeg2Reg2
	
	mov		voMpeg2Reg2,voMpeg2Reg8,lsr #8
	orr		voMpeg2Reg2,voMpeg2Reg2,voMpeg2Reg9,lsl #24

	.else
    ldr		voMpeg2Reg9,[voMpeg2Reg0,#4]
    ldr		voMpeg2Reg8,[voMpeg2Reg0],voMpeg2Reg2

    .if \Pos+8 < 32
	  mov	voMpeg2Reg2,voMpeg2Reg8,lsr #\Pos+8
	  orr	voMpeg2Reg2,voMpeg2Reg2,voMpeg2Reg9,lsl #32-\Pos-8
    .else
	  mov	voMpeg2Reg2,voMpeg2Reg9
    .endif

	.if \Pos > 0
	  mov	voMpeg2Reg8,voMpeg2Reg8,lsr #\Pos
	  orr	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg9,lsl #32-\Pos
	.endif
	.endif

	and		voMpeg2Reg10,voMpeg2Reg2,voMpeg2Reg14
	and		voMpeg2Reg2,voMpeg2Reg12,voMpeg2Reg2,lsr #2
	and		voMpeg2Reg11,voMpeg2Reg8,voMpeg2Reg14
	and		voMpeg2Reg8,voMpeg2Reg12,voMpeg2Reg8,lsr #2
	add		voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg2
	add		voMpeg2Reg10,voMpeg2Reg10,voMpeg2Reg11
  .endif
	ldr		voMpeg2Reg2,[sp]
  .endm

  .macro CopyHorVerRow Id, Pos, Parity, Round, Add, ARMv6

@voMpeg2Reg14 03030303
@voMpeg2Reg12 3f3f3f3f

	LoadHorVerRow \Id,\Pos,\Parity

	.if \Round>0
	  and voMpeg2Reg3,voMpeg2Reg14,voMpeg2Reg14,lsr #1		@0x01010101
	.else
	  and voMpeg2Reg3,voMpeg2Reg14,voMpeg2Reg14,lsl #1		@0x02020202
	.endif
    .if \Parity>0
	  add	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg4
	  add	voMpeg2Reg10,voMpeg2Reg10,voMpeg2Reg6

	  add	voMpeg2Reg10,voMpeg2Reg10,voMpeg2Reg3
	  and	voMpeg2Reg10,voMpeg2Reg14,voMpeg2Reg10,lsr #2

	  .if \Add>0
	    add	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg10 
	    orr	voMpeg2Reg12,voMpeg2Reg12,voMpeg2Reg12,lsl #1  @0x7F7F7F7F
	    ldr	voMpeg2Reg10,[voMpeg2Reg1]
	    and	voMpeg2Reg3,voMpeg2Reg12,voMpeg2Reg8,lsr #1
	    orr voMpeg2Reg8,voMpeg2Reg10,voMpeg2Reg8
	    and	voMpeg2Reg10,voMpeg2Reg12,voMpeg2Reg10,lsr #1
	    add voMpeg2Reg10,voMpeg2Reg10,voMpeg2Reg3
	    and voMpeg2Reg3,voMpeg2Reg14,voMpeg2Reg14,lsr #1 @0x01010101
	    mvn	voMpeg2Reg12,voMpeg2Reg14,lsl #6    @restore voMpeg2Reg12
	    and voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg3	
	    ldr	voMpeg2Reg11,[sp,#8]  @dstpitch	    	
	    add voMpeg2Reg10,voMpeg2Reg10,voMpeg2Reg8
	    ldr	voMpeg2Reg3,[sp,#4]	@end src for loop compare
	    str	voMpeg2Reg10,[voMpeg2Reg1],voMpeg2Reg11
	  .else
	    add	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg10
	    ldr	voMpeg2Reg10,[sp,#8]  @dstpitch
	    ldr	voMpeg2Reg3,[sp,#4]	@end src for loop compare
	    str	voMpeg2Reg8,[voMpeg2Reg1],voMpeg2Reg10
	  .endif
	.else
	  add	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg8
	  add	voMpeg2Reg6,voMpeg2Reg6,voMpeg2Reg10


	  add	voMpeg2Reg6,voMpeg2Reg6,voMpeg2Reg3
	  and	voMpeg2Reg6,voMpeg2Reg14,voMpeg2Reg6,lsr #2

	  .if \Add>0
	    add	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6
	    orr	voMpeg2Reg12,voMpeg2Reg12,voMpeg2Reg12,lsl #1  @0x7F7F7F7F
	    ldr	voMpeg2Reg6,[voMpeg2Reg1]
	    and	voMpeg2Reg3,voMpeg2Reg12,voMpeg2Reg4,lsr #1
	    orr voMpeg2Reg4,voMpeg2Reg6,voMpeg2Reg4
	    and	voMpeg2Reg6,voMpeg2Reg12,voMpeg2Reg6,lsr #1
	    add voMpeg2Reg6,voMpeg2Reg6,voMpeg2Reg3
	    and voMpeg2Reg3,voMpeg2Reg14,voMpeg2Reg14,lsr #1 @0x01010101
	    mvn	voMpeg2Reg12,voMpeg2Reg14,lsl #6    @restore voMpeg2Reg12
	    and voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg3
	    ldr	voMpeg2Reg3,[sp,#8]  @dstpitch		
	    add voMpeg2Reg6,voMpeg2Reg6,voMpeg2Reg4
	    str	voMpeg2Reg6,[voMpeg2Reg1],voMpeg2Reg3
	  .else
	    ldr	voMpeg2Reg3,[sp,#8]  @dstpitch
	    add	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg6
	    str	voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg3
	  .endif
	.endif
  .endm

  .macro ArmInter4x8HV Id, Pos, Round, Add, ARMv6
	.if \ARMv6>0
	  ldr		voMpeg2Reg4, [sp, #40]
	  sub		sp,sp,#12
	  mul		voMpeg2Reg4, voMpeg2Reg2, voMpeg2Reg4
@	  str		voMpeg2Reg3,[sp,#8]	@dstpitch
	  mov		voMpeg2Reg7, voMpeg2Reg3
	  add		voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg0

@	  add		voMpeg2Reg4,voMpeg2Reg0,voMpeg2Reg2,lsl #3
	  add		voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg2
@	  str		voMpeg2Reg2,[sp]		@srcpitch
	  mov		voMpeg2Reg12, voMpeg2Reg2
	  str		voMpeg2Reg4,[sp,#4]	@end src

	  ldr		voMpeg2Reg14,LABM_ARMv6\Id
@	  mvn		voMpeg2Reg12,voMpeg2Reg14,lsl #6
@	  pld [voMpeg2Reg0,voMpeg2Reg2]
	  LoadHorVerRow_ARMv6 \Id,\Pos,1, \ARMv6
LABLOOP_ARMv6\Id:
@	  pld [voMpeg2Reg0,voMpeg2Reg2]
	  CopyHorVerRow_ARMv6 \Id,\Pos,0,\Round,\Add, \ARMv6
@	  pld [voMpeg2Reg0,voMpeg2Reg2]
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

@	  add		voMpeg2Reg4,voMpeg2Reg0,voMpeg2Reg2,lsl #3
	  add		voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg2
	  str		voMpeg2Reg2,[sp]		@srcpitch
	  str		voMpeg2Reg4,[sp,#4]	@end src


	  ldr		voMpeg2Reg14,LABM\Id
	  mvn		voMpeg2Reg12,voMpeg2Reg14,lsl #6
	  pld [voMpeg2Reg0,voMpeg2Reg2]
	  LoadHorVerRow \Id,\Pos,1
LABLOOP\Id:
	  pld [voMpeg2Reg0,voMpeg2Reg2]
	  CopyHorVerRow \Id,\Pos,0,\Round,\Add, \ARMv6
	  pld [voMpeg2Reg0,voMpeg2Reg2]
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
    ldr		voMpeg2Reg4,[voMpeg2Reg0],voMpeg2Reg12

    .if \Pos+8 < 32
	  mov	voMpeg2Reg2,voMpeg2Reg4,lsr #\Pos+8	  
	  orr	voMpeg2Reg2,voMpeg2Reg2,voMpeg2Reg5,lsl #32-\Pos-8
    .else
	  mov	voMpeg2Reg2,voMpeg2Reg5
    .endif

	.if \Pos > 0
	  mov	voMpeg2Reg4,voMpeg2Reg4,lsr #\Pos
	  orr	voMpeg2Reg4,voMpeg2Reg4,voMpeg2Reg5,lsl #32-\Pos
	.endif
	uhadd8	voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg2
  .else
	@read result voMpeg2Reg8,voMpeg2Reg9 and voMpeg2Reg2,voMpeg2Reg3 (one pixel to right)
	@voMpeg2Reg10,voMpeg2Reg11 can be used

    ldr		voMpeg2Reg9,[voMpeg2Reg0,#4]
    ldr		voMpeg2Reg8,[voMpeg2Reg0],voMpeg2Reg12

    .if \Pos+8 < 32
	  mov	voMpeg2Reg2,voMpeg2Reg8,lsr #\Pos+8	  
	  orr	voMpeg2Reg2,voMpeg2Reg2,voMpeg2Reg9,lsl #32-\Pos-8
    .else
	  mov	voMpeg2Reg2,voMpeg2Reg9
    .endif

	.if \Pos > 0
	  mov	voMpeg2Reg8,voMpeg2Reg8,lsr #\Pos
	  orr	voMpeg2Reg8,voMpeg2Reg8,voMpeg2Reg9,lsl #32-\Pos
	.endif
	uhadd8	voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg2
  .endif
  .endm

  .macro CopyHorVerRow_ARMv6 Id, Pos, Parity, Round, Add, ARMv6

@voMpeg2Reg14 03030303
@voMpeg2Reg12 3f3f3f3f

	LoadHorVerRow_ARMv6 \Id,\Pos,\Parity, \ARMv6

	  .if \Round>0
	    mov voMpeg2Reg3, voMpeg2Reg14				@0x01010101
	  .else
	    mov voMpeg2Reg3, voMpeg2Reg14, lsl #1		@0x02020202
	  .endif 
		
      .if \Parity>0
	    uqadd8  voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg3		   
	    uhadd8	voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg4
@	    ldr	voMpeg2Reg2,[sp,#8]  @dstpitch
	    ldr	voMpeg2Reg3,[sp,#4]	@end src for loop compare 	    
	    .if \Add>0
	      ldr	voMpeg2Reg10,[voMpeg2Reg1]
	      .if \Round>0
	      .else
	        uqadd8  voMpeg2Reg8, voMpeg2Reg8, voMpeg2Reg14
	      .endif				  
	      uhadd8	voMpeg2Reg8, voMpeg2Reg10, voMpeg2Reg8
	    .endif		  
	    str	voMpeg2Reg8,[voMpeg2Reg1],voMpeg2Reg7 
      .else
	    uqadd8  voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg3		   
	    uhadd8	voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg8
@	    ldr	voMpeg2Reg2,[sp,#8]  @dstpitch
	    	
	    .if \Add>0
	      ldr	voMpeg2Reg10,[voMpeg2Reg1]
	      .if \Round>0
	      .else
	        uqadd8  voMpeg2Reg4, voMpeg2Reg4, voMpeg2Reg14
	      .endif			  
  	      uhadd8	voMpeg2Reg4, voMpeg2Reg10, voMpeg2Reg4
	    .endif
	    str	voMpeg2Reg4,[voMpeg2Reg1],voMpeg2Reg7   
	  .endif
  .endm

@---------------------------------------------------
@ general unaligned copy (use PldA)
@
@	GenCodeCpy ArmCopy4x8,ArmCopy4x8,0,0,0,1,1,0
@	GenCodeCpy ArmCopy8x16,ArmCopy8x16,0,0,0,1,1,0
@---------------------------------------------------
@ general unaligned copy (use PldA)


@---------------------------------------------------
@ smaller versions without PldA
@
@	GenCodeCpy ArmInter4x8,ArmInter4x8,0,0,1,0
@	GenCodeCpy ArmInter4x8V,ArmInter4x8V,0,0,0,0
@	GenCodeCpy ArmInter4x8H,ArmInter4x8H,0,0,0,0
@	GenCodeCpy ArmInter4x8HV,ArmInter4x8HV,0,0,0,0
@
@	GenCodeCpy ArmInter2_4x8 ,ArmInter2_4x8 ,0,1,0,0
@	GenCodeCpy ArmInter2_4x8V,ArmInter4x8V,0,1,0,0
@	GenCodeCpy ArmInter2_4x8H,ArmInter4x8H,0,1,0,0
@	GenCodeCpy ArmInter2_4x8HV,ArmInter4x8HV,0,1,0,0
@
@---------------------------------------------------
@ smaller versions with PldA
@
@	GenCodeCpy PldAarm_copyblk,ArmInter4x8,0,0,1,1
@	GenCodeCpy PldAarm_copyblkv,ArmInter4x8V,0,0,0,1
@	GenCodeCpy PldAarm_copyblkh,ArmInter4x8H,0,0,0,1
@	GenCodeCpy PldAarm_copyblkhv,ArmInter4x8HV,0,0,0,1
@
@	GenCodeCpy PldAarm_addblk,ArmInter2_4x8 ,0,1,0,1
@	GenCodeCpy PldAarm_addblkv,ArmInter4x8V,0,1,0,1
@	GenCodeCpy PldAarm_addblkh,ArmInter4x8H,0,1,0,1
@	GenCodeCpy PldAarm_addblkhv,ArmInter4x8HV,0,1,0,1
@
@---------------------------------------------------
@ larger versions with PldA
@ (faster if there is enough intstruction cache available)
@---------------------------------------------------------------------------------------------
@   GenCodeCpy		\Name,			\Sub,		\Round,	\Add,	\Fast,	\ARM5,	\ARMv6,	\ARM11
@
@	GenCodeCpy ArmInter4x8,		ArmInter4x8,	0,	0,	1,	1,	1,	0
@	GenCodeCpy ArmInter4x8V,	ArmInter4x8V,	0,	0,	1,	1,	1,	0
@	GenCodeCpy ArmInter4x8H,	ArmInter4x8H,	0,	0,	1,	1,	1,	0
@	GenCodeCpy ArmInter4x8HV,	ArmInter4x8HV,	0,	0,	1,	1,	1,	0
@                                                                 	  	  	  	
@	GenCodeCpy ArmInter2_4x8 ,	ArmInter2_4x8 ,	0,	1,	1,	1,	1,	0
@	GenCodeCpy ArmInter2_4x8V,	ArmInter4x8V,	0,	1,	1,	1,	1,	0
@	GenCodeCpy ArmInter2_4x8H,	ArmInter4x8H,	0,	1,	1,	1,	1,	0
@	GenCodeCpy ArmInter2_4x8HV,	ArmInter4x8HV,	0,	1,	1,	1,	1,	0
@-----------------------------------------------------------------------------------------------
@   GenCodeCpy		\Name,			\Sub,		\Round,	\Add,	\Fast,	\ARM5,	\ARMv6,	\ARM11

	GenCodeCpy __voMPEG2D0194,ArmCopy4x8,		0,	0,	0,	0,	1,	0
	GenCodeCpy __voMPEG2D0195,ArmCopy8x16,		0,	0,	0,	0,	1,	0
	
	GenCodeCpy __voMPEG2D0186,	ArmInter4x8,	0,	0,	1,	1,	1,	0
	GenCodeCpy __voMPEG2D0187,	ArmInter4x8V,	0,	0,	1,	1,	1,	0
	GenCodeCpy __voMPEG2D0188,	ArmInter4x8H,	0,	0,	1,	1,	1,	0
	GenCodeCpy __voMPEG2D0189,	ArmInter4x8HV,	0,	0,	1,	1,	0,	0
                                                                  	  	  	  	
	GenCodeCpy __voMPEG2D0190,	ArmInter2_4x8 ,	0,	1,	1,	1,	1,	0
	GenCodeCpy __voMPEG2D0191,	ArmInter4x8V,	0,	1,	1,	1,	1,	0
	GenCodeCpy __voMPEG2D0192,	ArmInter4x8H,	0,	1,	1,	1,	1,	0
	GenCodeCpy __voMPEG2D0193,	ArmInter4x8HV,	0,	1,	1,	1,	0,	0
	@END
	