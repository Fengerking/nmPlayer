@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2005		            *
@								 	                                    *
@***********************************************************************/

@R0 src
@R1 dst
@R2 srcpitch
@R3 dstpitch

#include "../../voMpeg4DID.h"
	@AREA	|.text|, CODE
	.text

  .macro GenCodeCpy @Name,Sub,Round,Add,Fast,ARM5,ARMv6,ARM11

	.align 4 
	.globl _$0
_$0: @proc
	stmdb	sp!, {r4 - r12, lr}
@	.if $4>0	  
@	.if $7>0
@	$1	_0_v11$0,0,$2,$3,0
@	.else
	movs	r4,r0,lsl #30
	beq		LAB4$0
	cmp	r4,#0x80000000
	beq		LAB2$0
	bhi		LAB3$0
LAB1$0:
	bic		r0,r0,#3
	$1	_8$0,8,$2,$3,$6
LAB2$0:	
	bic		r0,r0,#3
	$1	_16$0,16,$2,$3,$6
LAB3$0:
	bic		r0,r0,#3
	$1	_24$0,24,$2,$3,$6
LAB4$0:	
	$1	_0$0,0,$2,$3,$6
@	.endif
@	.else
@	$1	_s$0,-1,$2,$3,$6
@	.endif
	@endp
  .endmacro

@------------------------------------------
@ArmInter8x8
@------------------------------------------

  .macro CpyBlkRArm @ARMv6,Pos
	.if $1 > 0
	  ldr	r5,[r0,#8]
	  ldr	r6,[r0,#4]
	  ldr	r4,[r0],r2
	  ldr	r9,[r0,#8]
	  ldr	r10,[r0,#4]
	  ldr	r8,[r0],r2  		
	  mov	r5,r5,lsl #32-$1
	  mov	r4,r4,lsr #$1
	  orr	r5,r5,r6,lsr #$1
	  orr	r4,r4,r6,lsl #32-$1
	  mov	r9,r9,lsl #32-$1
	  mov	r8,r8,lsr #$1
	  orr	r9,r9,r10,lsr #$1
	  orr	r8,r8,r10,lsl #32-$1	  
	  str	r5,[r1,#4]
	  str	r4,[r1],r3
	  str	r9,[r1,#4]
	  str	r8,[r1],r3
	  
	  ldr	r5,[r0,#8]
	  ldr	r6,[r0,#4]
	  ldr	r4,[r0],r2
	  ldr	r9,[r0,#8]
	  ldr	r10,[r0,#4]
	  ldr	r8,[r0],r2	  		
	  mov	r5,r5,lsl #32-$1
	  mov	r4,r4,lsr #$1
	  orr	r5,r5,r6,lsr #$1
	  orr	r4,r4,r6,lsl #32-$1
	  mov	r9,r9,lsl #32-$1
	  mov	r8,r8,lsr #$1
	  orr	r9,r9,r10,lsr #$1
	  orr	r8,r8,r10,lsl #32-$1	  
	  str	r5,[r1,#4]
	  str	r4,[r1],r3
	  str	r9,[r1,#4]
	  str	r8,[r1],r3  
		subs	r12, r12, #4	   	  
	.else
		  ldr	r5,[r0,#4]
		  ldr	r4,[r0],r2	
		  ldr	r7,[r0,#4]
		  ldr	r6,[r0],r2	
		  ldr	r9,[r0,#4]
		  ldr	r8,[r0],r2
		  ldr	r11,[r0,#4]
		  ldr	r10,[r0],r2		  
			subs	r12, r12, #4		  		  	  	  
		  str	r5,[r1,#4]
		  str	r4,[r1],r3
		  str	r7,[r1,#4]
		  str	r6,[r1],r3
		  str	r9,[r1,#4]
		  str	r8,[r1],r3
		  str	r11,[r1,#4]
		  str	r10,[r1],r3
	.endif
  .endmacro

  .macro ArmInter8x8 @Id, Pos, Round, Add, ARMv6

	ldr		r12, [sp, #40]
loop_again$0:
@	pld [r0,r2]
	CpyBlkRArm $4,$1 
	bne		loop_again$0
	ldmia	sp!, {r4 - r12, pc}
  .endmacro

@------------------------------------------
@ArmCopy16x16: no aligment!, only used in Copy()
@------------------------------------------

  .macro arm_copyblk16x16Row @ARMv6

    .if $0>0
		ldrd	r4,[r0, #8]
		ldrd	r6,[r0], r2
		ldrd	r8,[r0, #8]
		ldrd	r10,[r0], r2		
 		
		strd	r4,[r1, #8]
		strd	r6,[r1], r3
		strd	r8,[r1, #8]
		strd	r10,[r1], r3	
		
		ldrd	r4,[r0, #8]
		ldrd	r6,[r0], r2
		ldrd	r8,[r0, #8]
		ldrd	r10,[r0], r2		
	subs r12,r12,#4 		
		strd	r4,[r1, #8]
		strd	r6,[r1], r3
		strd	r8,[r1, #8]
		strd	r10,[r1], r3				
	.else
		ldr		r7,[r0,#12]
		ldr		r6,[r0,#8]
		ldr		r5,[r0,#4]
		ldr		r4,[r0],r2
		ldr		r11,[r0,#12]
		ldr		r10,[r0,#8]
		ldr		r9,[r0,#4]
		ldr		r8,[r0],r2		
		
		str		r7,[r1,#12]
		str		r6,[r1,#8]
		str		r5,[r1,#4]
		str		r4,[r1],r3
		str		r11,[r1,#12]
		str		r10,[r1,#8]
		str		r9,[r1,#4]
		str		r8,[r1],r3	
		
		ldr		r7,[r0,#12]
		ldr		r6,[r0,#8]
		ldr		r5,[r0,#4]
		ldr		r4,[r0],r2
		ldr		r11,[r0,#12]
		ldr		r10,[r0,#8]
		ldr		r9,[r0,#4]
		ldr		r8,[r0],r2		
	subs r12,r12,#4 		
		str		r7,[r1,#12]
		str		r6,[r1,#8]
		str		r5,[r1,#4]
		str		r4,[r1],r3
		str		r11,[r1,#12]
		str		r10,[r1,#8]
		str		r9,[r1,#4]
		str		r8,[r1],r3				
	.endif	

  .endmacro

  .macro ArmCopy16x16 @Id, Pos, Round, Add, ARMv6

	ldr		r12, [sp, #40]
@	mov		r12, #16

LABLOOP$0:
@	pld [r0,r2]
	arm_copyblk16x16Row $4
	bne  LABLOOP$0

	ldmia	sp!, {r4 - r12, pc}
  .endmacro

@------------------------------------------
@ArmCopy8x8: no aligment!, only used in Copy()
@------------------------------------------

  .macro ArmCopy8x8 @Id, Pos, Round, Add, ARMv6

    .if $4>0

		ldrd	r4,[r0], r2
		ldrd	r6,[r0], r2
		ldrd	r8,[r0], r2
		ldrd	r10,[r0], r2
		strd	r4,[r1], r3
		strd	r6,[r1], r3
		strd	r8,[r1], r3
		strd	r10,[r1], r3

		ldrd	r4,[r0], r2
		ldrd	r6,[r0], r2
		ldrd	r8,[r0], r2
		ldrd	r10,[r0]
		strd	r4,[r1], r3
		strd	r6,[r1], r3
		strd	r8,[r1], r3
		strd	r10,[r1], r3	
	.else

	ldr		r7,[r0,#4]
	ldr		r6,[r0],r2
	ldr		r5,[r0,#4]
	ldr		r4,[r0],r2
	ldr		r11,[r0,#4]
	ldr		r10,[r0],r2
	ldr		r9,[r0,#4]
	ldr		r8,[r0],r2	
	str		r7,[r1,#4]
	str		r6,[r1],r3
	str		r5,[r1,#4]
	str		r4,[r1],r3
	str		r11,[r1,#4]
	str		r10,[r1],r3
	str		r9,[r1,#4]
	str		r8,[r1],r3	

	ldr		r7,[r0,#4]
	ldr		r6,[r0],r2
	ldr		r5,[r0,#4]
	ldr		r4,[r0],r2
	ldr		r11,[r0,#4]
	ldr		r10,[r0],r2
	ldr		r9,[r0,#4]
	ldr		r8,[r0],r2	
	str		r7,[r1,#4]
	str		r6,[r1],r3
	str		r5,[r1,#4]
	str		r4,[r1],r3
	str		r11,[r1,#4]
	str		r10,[r1],r3
	str		r9,[r1,#4]
	str		r8,[r1],r3

	.endif

	ldmia	sp!, {r4 - r12, pc}
  .endmacro

@------------------------------------------
@ArmInter2_8x8 
@------------------------------------------

  .macro arm_addblkRow @Pos, ARMv6

	.if $0 > 0
	  ldr	r5,[r0,#8]
	  ldr	r6,[r0,#4]
	  ldr	r4,[r0],r2
		
	  mov	r5,r5,lsl #32-$0
	  mov	r4,r4,lsr #$0
	  orr	r5,r5,r6,lsr #$0
	  orr	r4,r4,r6,lsl #32-$0
	.else
@		.if $1 > 0
@		  ldrd	r4,[r0],r2	  		  		  	  		  		  
@		.else	
	  ldr	r5,[r0,#4]
	  ldr	r4,[r0],r2
@		.endif	  
	.endif

		.if $1 > 0
		  ldrd	r6,[r1]  
		  
	uqadd8  r4, r4, r14
	uqadd8  r5, r5, r14	
	uhadd8	r6, r6, r4
	uhadd8	r7, r7, r5	

		  strd	r6,[r1], r3  
		  			  		  		  	  		  		  
		.else	
	ldr		r7,[r1,#4]
	ldr		r6,[r1]
	
	and		r9,r12,r5,lsr #1
	and		r8,r12,r4,lsr #1
	orr		r5,r7,r5
	orr		r4,r6,r4
	and		r7,r12,r7,lsr #1
	and		r6,r12,r6,lsr #1
	add		r7,r7,r9
	add		r6,r6,r8
	and		r5,r5,r14
	and		r4,r4,r14
	add		r7,r7,r5
	add		r6,r6,r4

	str		r7,[r1,#4]
	str		r6,[r1],r3		
		.endif	  	

  .endmacro

  .macro ArmInter2_8x8  @Id, Pos, Round, Add, ARMv6


	  .if $4>0
	ldr		r14,LABM$0
	ldr		r12, [sp, #40]
@	mvn		r12,r14,lsl #7
LABLOOP_ARMv6$0:
@	pld [r0,r2]
	.if $1 > 0
	  ldr	r5,[r0,#8]
	  ldr	r10,[r0,#4]
	  ldr	r4,[r0],r2
	  ldr	r7,[r0,#8]
	  ldr	r8,[r0,#4]
	  ldr	r6,[r0],r2	  
		
	  mov	r5,r5,lsl #32-$1
	  mov	r4,r4,lsr #$1
	  orr	r5,r5,r10,lsr #$1
	  orr	r4,r4,r10,lsl #32-$1
		
	  mov	r7,r7,lsl #32-$1
	  mov	r6,r6,lsr #$1
	  orr	r7,r7,r8,lsr #$1
	  orr	r6,r6,r8,lsl #32-$1	  
	.else
@		  ldrd	r4,[r0],r2
@		  ldrd	r6,[r0],r2	

		  ldr	r5,[r0,#4]
		  ldr	r4,[r0],r2	
		  ldr	r7,[r0,#4]
		  ldr	r6,[r0],r2	
		    	  		  		  	  		  		  
	.endif
	
		  ldrd	r8,[r1]  
		  ldrd	r10,[r1, r3 ] 		  
	uqadd8  r4, r4, r14
	uqadd8  r5, r5, r14	
	uqadd8  r6, r6, r14
	uqadd8  r7, r7, r14	
		
	uhadd8	r8, r8, r4
	uhadd8	r9, r9, r5	
	uhadd8	r10, r10, r6
	uhadd8	r11, r11, r7		

		  strd	r8,[r1], r3  
		  strd	r10,[r1], r3  		  
	subs r12,r12,#2 
	bne  LABLOOP_ARMv6$0
	ldmia	sp!, {r4 - r12, pc}
LABM$0: .word 0x01010101
		  
	  .else
	  
@	PldBI \ARM5,\Pos
	ldr		r14,LABM$0

	ldr		r11, [sp, #40]

@	mov		r11,#8
	mvn		r12,r14,lsl #7

LABLOOP$0:
@	pld [r0,r2]
	arm_addblkRow $1, $4
	subs r11,r11,#1 
	bne  LABLOOP$0

	ldmia	sp!, {r4 - r12, pc}
LABM$0: .word 0x01010101
	.endif
  .endmacro
  
@------------------------------------------
@ ArmInter8x8H
@------------------------------------------

  .macro LoadHorRow @Id, Pos, ARMv6

    @ result is r4,r5 and r8,r9 (one pixel to the right)
    @ r6,r7 can be used

    ldr		r5,[r0,#4]
    ldr		r6,[r0,#8]
    ldr		r4,[r0],r2

    .if $1+8 < 32
	  mov	r9,r5,lsr #$1+8
	  mov	r8,r4,lsr #$1+8
	  orr	r9,r9,r6,lsl #32-$1-8
	  orr	r8,r8,r5,lsl #32-$1-8
    .else
	  mov	r8,r5
	  mov	r9,r6
    .endif

	.if $1 > 0
	  mov	r4,r4,lsr #$1
	  mov	r6,r6,lsl #32-$1
	  orr	r4,r4,r5,lsl #32-$1
	  orr	r5,r6,r5,lsr #$1
	.endif
	
  .endmacro

  .macro CopyHorRow @Id, Pos, Round, Add, ARMv6

@r14 01010101
@r12 7f7f7f7f

	LoadHorRow	$0,$1, $4
	.if $4>0
	
	.if $2>0
	.else
	uqadd8  r4, r4, r14
	uqadd8  r5, r5, r14	
	.endif
	
	uhadd8	r4, r4, r8
	uhadd8	r5, r5, r9
	
	.if $3>0
	  ldr	r7,[r1,#4]
	  ldr	r6,[r1]

	uqadd8  r4, r4, r14
	uqadd8  r5, r5, r14		  
	uhadd8	r4, r4, r6
	uhadd8	r5, r5, r7	
	
	.endif	
	  strd	r4,[r1],r3
	  	
	.else
	
	and		r6,r12,r4,lsr #1
	and		r7,r12,r5,lsr #1
	.if $2>0
	  and	r4,r4,r8
	  and	r5,r5,r9
	.else
	  orr	r4,r4,r8
	  orr	r5,r5,r9
	.endif

	and		r8,r12,r8,lsr #1
	and		r9,r12,r9,lsr #1

	and		r4,r4,r14
	and		r5,r5,r14

	add		r4,r4,r6
	add		r5,r5,r7
	add		r4,r4,r8
	add		r5,r5,r9
	
	.if $3>0
	  ldr	r7,[r1,#4]
	  ldr	r6,[r1]
	  
	  and	r9,r12,r5,lsr #1
	  and	r8,r12,r4,lsr #1
	  orr	r5,r7,r5
	  orr	r4,r6,r4
	  and	r7,r12,r7,lsr #1
	  and	r6,r12,r6,lsr #1
	  add	r7,r7,r9
	  add	r6,r6,r8
	  and	r5,r5,r14
	  and	r4,r4,r14
	  add	r5,r7,r5
	  add	r4,r6,r4
	.endif	
	  str	r5,[r1,#4]
	  str	r4,[r1],r3
	  
	.endif	  

  .endmacro

  .macro ArmInter8x8H @Id, Pos, Round, Add, ARMv6


	  .if $4>0
@	ldr		r14,LABM_ARMv6$0
	ldr		r11, [sp, #40]
@	mvn		r12,r14,lsl #7

LABLOOP_ARMv6$0:
@	pld [r0,r2]
	CopyHorRow_ARMv6 $0,$1,$2,$3, $4
@	subs r11,r11,#1
	bne  LABLOOP_ARMv6$0
	ldmia	sp!, {r4 - r12, pc}
@LABM_ARMv6$0: .word 0x01010101
		  
	  .else
	  
@	PldBI \ARM5,\Pos
	ldr		r14,LABM$0

	ldr		r11, [sp, #40]

@	mov		r11,#8
	mvn		r12,r14,lsl #7

LABLOOP$0:
	pld [r0,r2]
	CopyHorRow $0,$1,$2,$3, $4
	subs r11,r11,#1
	bne  LABLOOP$0

	ldmia	sp!, {r4 - r12, pc}
LABM$0: .word 0x01010101
	.endif
  .endmacro

  .macro LoadHorRow_ARMv6 @Id, Pos, ARMv6

    @ result is r4,r5 and r8,r9 (one pixel to the right)
    @ r6,r7 can be used

    ldr		r5,[r0,#4]
    ldr		r6,[r0,#8]
    ldr		r4,[r0],r2
	subs r11,r11,#1
    .if $1+8 < 32
	  mov	r9,r5,lsr #$1+8
	  orr	r9,r9,r6,lsl #32-$1-8
	  mov	r8,r4,lsr #$1+8	  
	  orr	r8,r8,r5,lsl #32-$1-8
    .else
	  mov	r8,r5
	  mov	r9,r6
    .endif

	.if $1 > 0
	  mov	r4,r4,lsr #$1
	  mov	r6,r6,lsl #32-$1
	  orr	r4,r4,r5,lsl #32-$1
	  orr	r5,r6,r5,lsr #$1
	.endif
	
  .endmacro

  .macro CopyHorRow_ARMv6 @Id, Pos, Round, Add, ARMv6

@r14 01010101
@r12 7f7f7f7f

	LoadHorRow_ARMv6	$0,$1, $4
	
	.if $3>0
	  ldrd	r6,[r1] 	
	.endif
	
		.if $2>0
		@calc (a+b)>>1
			uhadd8	r4, r4, r8
			uhadd8	r5, r5, r9
		.else
		@calc (a+b+1)>>1 as b-((b-a)>>1)
			uhsub8	r4, r8, r4
			uhsub8	r5, r9, r5
			usub8	r4, r8, r4
			usub8	r5, r9, r5
		.endif	
	
	.if $3>0
	@calc (a+b+1)>>1 as b-((b-a)>>1)
			uhsub8	r4, r6, r4
			uhsub8	r5, r7, r5
			usub8	r4, r6, r4
			usub8	r5, r7, r5
	.endif	
	  strd	r4,[r1],r3
  .endmacro
@------------------------------------------
@ ArmInter8x8V
@------------------------------------------

  .macro LoadVerRow @Id, Pos, Parity, ARMv6
  .if $2>0
    @ result is r8,r9 (r10=r8>>1,r11=r9>>1) 
    @ r10,r11 can be used
    .if $1 > 0
	  ldr	r9,[r0,#8]
	  ldr	r10,[r0,#4]
	  ldr	r8,[r0],r2

	  mov	r9,r9,lsl #32-$1
	  mov	r8,r8,lsr #$1
	  orr	r9,r9,r10,lsr #$1
	  orr	r8,r8,r10,lsl #32-$1
    .else
@	.if $3>0    
@	  ldrd	r8,[r0],r2
@	.else   
	  ldr	r9,[r0,#4]
	  ldr	r8,[r0],r2
@	.endif
    .endif
	.if $3>0
	.else    
	and		r11,r12,r9,lsr #1
	and		r10,r12,r8,lsr #1
	.endif
  .else
    @ result is r4,r5 (r6=r4>>1,r7=r5>>1) 
    @ r6,r7 can be used

    .if $1 > 0
	  ldr	r5,[r0,#8]
	  ldr	r6,[r0,#4]
	  ldr	r4,[r0],r2

	  mov	r5,r5,lsl #32-$1
	  mov	r4,r4,lsr #$1
	  orr	r5,r5,r6,lsr #$1
	  orr	r4,r4,r6,lsl #32-$1
    .else
@	.if $3>0    
@	  ldrd	r4,[r0],r2
@	.else   
	  ldr	r5,[r0,#4]
	  ldr	r4,[r0],r2
@	.endif    
    .endif
	.if $3>0 
	.else  	   
	and		r7,r12,r5,lsr #1
	and		r6,r12,r4,lsr #1
	.endif    
  .endif
  .endmacro

  .macro CopyVerRow @Id, Pos, Parity, Round, Add, ARMv6

@r14 01010101
@r12 7f7f7f7f

	LoadVerRow $0,$1,$2, $5
	.if $5>0 
    .if $2>0   
	.if $3>0
	.else
	uqadd8  r4, r4, r14
	uqadd8  r5, r5, r14	
	.endif
	
	uhadd8	r4, r4, r8
	uhadd8	r5, r5, r9
	
	.if $4>0
	  ldrd	r10,[r1]

	uqadd8  r4, r4, r14
	uqadd8  r5, r5, r14		  
	uhadd8	r4, r4, r10
	uhadd8	r5, r5, r11				
	.endif	
	  strd	r4,[r1],r3  
	.else
	
	.if $3>0
	.else
	uqadd8  r8, r8, r14
	uqadd8  r9, r9, r14	
	.endif
	
	uhadd8	r8, r8, r4
	uhadd8	r9, r9, r5
	
	.if $4>0
	  ldrd	r10,[r1]

	uqadd8  r8, r8, r14
	uqadd8  r9, r9, r14		  
	uhadd8	r8, r8, r10
	uhadd8	r9, r9, r11			
	.endif		
	  strd	r8,[r1],r3  
	.endif
    .else
    .if $2>0
	  .if $3>0
	    and	r4,r4,r8
	    and	r5,r5,r9
	  .else
	    orr	r4,r4,r8
	    orr	r5,r5,r9
	  .endif
	  and	r4,r4,r14
	  and	r5,r5,r14

	  add	r4,r4,r6
	  add	r5,r5,r7

  	  add	r4,r4,r10
	  add	r5,r5,r11

	  .if $4>0
	    ldr	r7,[r1,#4]
	    ldr	r6,[r1]
	    and	r3,r12,r5,lsr #1
		orr r5,r7,r5
	    and	r7,r12,r7,lsr #1
		add r7,r7,r3
	    and	r3,r12,r4,lsr #1
		orr r4,r6,r4
	    and	r6,r12,r6,lsr #1
		add r6,r6,r3
		and r5,r5,r14
		and r4,r4,r14
		add r5,r5,r7
		add r4,r4,r6
		ldr	r3,[sp, #4]		@			
		ldr	r7,[sp]		@end src for loop compare
	    str	r5,[r1,#4]
	    str	r4,[r1],r3
	  .else
		ldr	r7,[sp]		@end src for loop compare
	    str	r5,[r1,#4]
	    str	r4,[r1],r3
	  .endif
	.else
	  .if $3>0
	    and	r8,r8,r4
	    and	r9,r9,r5
	  .else
	    orr	r8,r8,r4
	    orr	r9,r9,r5
	  .endif
	  and	r8,r8,r14
	  and	r9,r9,r14

	  add	r8,r8,r10
	  add	r9,r9,r11
  	  add	r8,r8,r6
	  add	r9,r9,r7

	  .if $4>0
	    ldr	r11,[r1,#4]
	    ldr	r10,[r1]
	    and	r3,r12,r9,lsr #1
		orr r9,r11,r9
	    and	r11,r12,r11,lsr #1
		add r11,r11,r3
	    and	r3,r12,r8,lsr #1
		orr r8,r10,r8
	    and	r10,r12,r10,lsr #1
		add r10,r10,r3
		and r9,r9,r14
		and r8,r8,r14
		add r11,r11,r9
		ldr	r3,[sp, #4]		@			
		add r10,r10,r8
	    str	r11,[r1,#4]
	    str	r10,[r1],r3
	  .else
	    str	r9,[r1,#4]
	    str	r8,[r1],r3
	  .endif
	.endif
	.endif    	
  .endmacro


  .macro ArmInter8x8V @Id, Pos, Round, Add, ARMv6


	  .if $4>0
	ldr		r4, [sp, #40]
@	sub		sp,sp,#8
	mul		r4, r2, r4
@	ldr		r14,LABM_ARMv6$0
	add		r4, r4, r0
	add		r7,r4,r2	
@	mvn		r12,r14,lsl #7
@	pld [r0,r2]
	LoadVerRow_ARMv6 $0,$1,1, $4
LABLOOP_ARMv6$0:
@	pld [r0,r2]
	CopyVerRow_ARMv6 $0,$1,0,$2,$3, $4
@	pld [r0,r2]
	CopyVerRow_ARMv6 $0,$1,1,$2,$3, $4
@	cmp		r0,r7
	bne		LABLOOP_ARMv6$0
@	add		sp,sp,#8
	ldmia	sp!, {r4 - r12, pc}
@LABM_ARMv6$0: .word 0x01010101
		  
	  .else
	  
	ldr		r4, [sp, #40]
	sub		sp,sp,#8
	mul		r4, r2, r4
	ldr		r14,LABM$0
	add		r4, r4, r0

@	add		r4,r0,r2,lsl #3
	.if $4>0 
	add		r7,r4,r2	
	.else  	   
	add		r4,r4,r2
	str		r4,[sp]		@end src
	str		r3,[sp, #4]		@end src	
	.endif 

	mvn		r12,r14,lsl #7

@	pld [r0,r2]
	LoadVerRow $0,$1,1, $4
LABLOOP$0:
@	pld [r0,r2]

	CopyVerRow $0,$1,0,$2,$3, $4
@	pld [r0,r2]
	CopyVerRow $0,$1,1,$2,$3, $4

	cmp		r0,r7
	bne		LABLOOP$0
	add		sp,sp,#8
	ldmia	sp!, {r4 - r12, pc}
LABM$0: .word 0x01010101
	.endif
  .endmacro


  .macro LoadVerRow_ARMv6 @Id, Pos, Parity, ARMv6
  .if $2>0
    @ result is r8,r9 (r10=r8>>1,r11=r9>>1) 
    @ r10,r11 can be used
    .if $1 > 0
	  ldr	r9,[r0,#8]
	  ldr	r10,[r0,#4]
	  ldr	r8,[r0],r2
		
		  mov	r9,r9,lsl #32-$1
		  orr	r9,r9,r10,lsr #$1
		  mov	r8,r8,lsr #$1	  
		  orr	r8,r8,r10,lsl #32-$1
    .else  
		@ldrd	r8,[r0],r2
		  ldr	r9,[r0,#4]
		  ldr	r8,[r0],r2
    .endif
  .else
    @ result is r4,r5 (r6=r4>>1,r7=r5>>1) 
    @ r6,r7 can be used

    .if $1 > 0
	  ldr	r5,[r0,#8]
	  ldr	r6,[r0,#4]
	  ldr	r4,[r0],r2
		  mov	r5,r5,lsl #32-$1
		  orr	r5,r5,r6,lsr #$1
		  mov	r4,r4,lsr #$1	  
		  orr	r4,r4,r6,lsl #32-$1
    .else
@	  ldrd	r4,[r0],r2   
	  ldr	r5,[r0,#4]
	  ldr	r4,[r0],r2	  
   .endif  
  .endif
  .endmacro

  .macro CopyVerRow_ARMv6 @Id, Pos, Parity, Round, Add, ARMv6

@r14 01010101
@r12 7f7f7f7f

	LoadVerRow_ARMv6 $0,$1,$2, $5
    .if $2>0   
		.if $4>0
			ldrd	r10,[r1]
		.endif
		
		.if $3>0
		@calc (a+b)>>1
			uhadd8	r4, r4, r8
			uhadd8	r5, r5, r9
		.else
		@calc (a+b+1)>>1 as b-((b-a)>>1)
			uhsub8	r4, r8, r4
			uhsub8	r5, r9, r5
			usub8	r4, r8, r4
			usub8	r5, r9, r5
		.endif
		
		.if $4>0
		@calc (a+b+1)>>1 as b-((b-a)>>1)
			uhsub8	r4, r10, r4
			uhsub8	r5, r11, r5
			cmp		r0,r7
			usub8	r4, r10, r4
			usub8	r5, r11, r5
		.else
			cmp		r0,r7
		.endif
	  strd	r4,[r1],r3  
	.else
	
	.if $4>0
		ldrd	r10,[r1]
	.endif
	
	.if $3>0
	@calc (a+b)>>1
		uhadd8	r8, r8, r4
		uhadd8	r9, r9, r5
	.else
	@calc (a+b+1)>>1 as b-((b-a)>>1)
		uhsub8	r8, r4, r8
		uhsub8	r9, r5, r9
		usub8	r8, r4, r8
		usub8	r9, r5, r9
	.endif
	
	.if $4>0
	@calc (a+b+1)>>1 as b-((b-a)>>1)
		uhsub8	r8, r10, r8
		uhsub8	r9, r11, r9
		usub8	r8, r10, r8
		usub8	r9, r11, r9
	.endif

	  strd	r8,[r1],r3  
	.endif
  .endmacro
@------------------------------------------
@ ArmInter8x8HV
@------------------------------------------

@ load needs r2,r3 for temporary (r2 is restored from stack)

  .macro LoadHorVerRow @Id, Pos, Parity, ARMv6
  .if $2>0

	@read result r4,r5 and r2,r3 (one pixel to right)
	@r6,r7 can be used

    ldr		r5,[r0,#4]
    ldr		r6,[r0,#8]
    ldr		r4,[r0],r2

    .if $1+8 < 32
	  mov	r3,r5,lsr #$1+8
	  mov	r2,r4,lsr #$1+8
	  orr	r3,r3,r6,lsl #32-$1-8
	  orr	r2,r2,r5,lsl #32-$1-8
    .else
	  mov	r2,r5
	  mov	r3,r6
    .endif

	.if $1 > 0
	  mov	r4,r4,lsr #$1
	  mov	r6,r6,lsl #32-$1
	  orr	r4,r4,r5,lsl #32-$1
	  orr	r5,r6,r5,lsr #$1
	.endif

	and		r6,r2,r14
	and		r2,r12,r2,lsr #2
	and		r7,r4,r14
	and		r4,r12,r4,lsr #2
	add		r4,r4,r2
	add		r6,r6,r7

	and		r2,r3,r14
	and		r3,r12,r3,lsr #2
	and		r7,r5,r14
	and		r5,r12,r5,lsr #2
	add		r5,r5,r3
	add		r7,r2,r7
  .else
	@read result r8,r9 and r2,r3 (one pixel to right)
	@r10,r11 can be used

    ldr		r9,[r0,#4]
    ldr		r10,[r0,#8]
    ldr		r8,[r0],r2

    .if $1+8 < 32
	  mov	r3,r9,lsr #$1+8
	  mov	r2,r8,lsr #$1+8
	  orr	r3,r3,r10,lsl #32-$1-8
	  orr	r2,r2,r9,lsl #32-$1-8
    .else
	  mov	r2,r9
	  mov	r3,r10
    .endif

	.if $1 > 0
	  mov	r8,r8,lsr #$1
	  mov	r10,r10,lsl #32-$1
	  orr	r8,r8,r9,lsl #32-$1
	  orr	r9,r10,r9,lsr #$1
	.endif

	and		r10,r2,r14
	and		r2,r12,r2,lsr #2
	and		r11,r8,r14
	and		r8,r12,r8,lsr #2
	add		r8,r8,r2
	add		r10,r10,r11

	and		r2,r3,r14
	and		r3,r12,r3,lsr #2
	and		r11,r9,r14
	and		r9,r12,r9,lsr #2
	add		r9,r9,r3
	add		r11,r2,r11
  .endif
	ldr		r2,[sp]
  .endmacro

  .macro CopyHorVerRow @Id, Pos, Parity, Round, Add, ARMv6

@r14 03030303
@r12 3f3f3f3f

	LoadHorVerRow $0,$1,$2, $5

	.if $3>0
	  and r3,r14,r14,lsr #1		@0x01010101
	.else
	  and r3,r14,r14,lsl #1		@0x02020202
	.endif
    .if $2>0
	  add	r8,r8,r4
	  add	r9,r9,r5
	  add	r10,r10,r6
	  add	r11,r11,r7

	  add	r10,r10,r3
	  add	r11,r11,r3
	  and	r10,r14,r10,lsr #2
	  and	r11,r14,r11,lsr #2

	  .if $4>0
	    add	r8,r8,r10 
	    add	r9,r9,r11
		orr	r12,r12,r12,lsl #1  @0x7F7F7F7F
	    ldr	r11,[r1,#4]
	    ldr	r10,[r1]
	    and	r3,r12,r9,lsr #1
		orr r9,r11,r9
	    and	r11,r12,r11,lsr #1
		add r11,r11,r3
	    and	r3,r12,r8,lsr #1
		orr r8,r10,r8
	    and	r10,r12,r10,lsr #1
		add r10,r10,r3
		and r3,r14,r14,lsr #1 @0x01010101
		mvn	r12,r14,lsl #6    @restore r12
		and r9,r9,r3
		and r8,r8,r3
		add r11,r11,r9
	    str	r11,[r1,#4]	
	    ldr	r11,[sp,#8]  @dstpitch	    	
		add r10,r10,r8
	    ldr	r3,[sp,#4]	@end src for loop compare
	    str	r10,[r1],r11
	  .else
	    add	r8,r8,r10
	    ldr	r10,[sp,#8]  @dstpitch
	    add	r9,r9,r11
	    ldr	r3,[sp,#4]	@end src for loop compare
	    str	r9,[r1,#4]
	    str	r8,[r1],r10
	  .endif
	.else
	  add	r4,r4,r8
	  add	r5,r5,r9
	  add	r6,r6,r10
	  add	r7,r7,r11

	  add	r6,r6,r3
	  add	r7,r7,r3
	  and	r6,r14,r6,lsr #2
	  and	r7,r14,r7,lsr #2

	  .if $4>0
	    add	r4,r4,r6
	    add	r5,r5,r7
		orr	r12,r12,r12,lsl #1  @0x7F7F7F7F
	    ldr	r7,[r1,#4]
	    ldr	r6,[r1]
	    and	r3,r12,r5,lsr #1
		orr r5,r7,r5
	    and	r7,r12,r7,lsr #1
		add r7,r7,r3
	    and	r3,r12,r4,lsr #1
		orr r4,r6,r4
	    and	r6,r12,r6,lsr #1
		add r6,r6,r3
		and r3,r14,r14,lsr #1 @0x01010101
		mvn	r12,r14,lsl #6    @restore r12
		and r5,r5,r3
		and r4,r4,r3
	    ldr	r3,[sp,#8]  @dstpitch		
		add r7,r7,r5
		add r6,r6,r4
	    str	r7,[r1,#4]
	    str	r6,[r1],r3
	  .else
	    ldr	r3,[sp,#8]  @dstpitch
	    add	r4,r4,r6
	    add	r5,r5,r7
	    str	r5,[r1,#4]
	    str	r4,[r1],r3
	  .endif
	.endif
  .endmacro

  .macro ArmInter8x8HV @Id, Pos, Round, Add, ARMv6

	ldr		r4, [sp, #40]
	sub		sp,sp,#12
	mul		r4, r2, r4
	str		r3,[sp,#8]	@dstpitch
	add		r4, r4, r0

@	add		r4,r0,r2,lsl #3
	add		r4,r4,r2
	str		r2,[sp]		@srcpitch
	str		r4,[sp,#4]	@end src


	ldr		r14,LABM$0
	mvn		r12,r14,lsl #6
	pld [r0,r2]
	LoadHorVerRow $0,$1,1, $4
LABLOOP$0:
	pld [r0,r2]
	CopyHorVerRow $0,$1,0,$2,$3, $4
	pld [r0,r2]
	CopyHorVerRow $0,$1,1,$2,$3, $4
	cmp		r0,r3
	bne		LABLOOP$0

	add		sp,sp,#12
	ldmia	sp!, {r4 - r12, pc}
LABM$0: .word 0x03030303
  .endmacro


@ load needs r2,r3 for temporary (r2 is restored from stack)

  .macro LoadHorVerRow_ARMv6 @Id, Pos, Parity, ARMv6
  
@	ldr		r2,[sp]
	  
  .if $2>0
	@read result r4,r5 and r2,r3 (one pixel to right)
	@r6,r7 can be used

    ldr		r5,[r0,#4]
    ldr		r6,[r0,#8]
    ldr		r4,[r0],r12

    .if $1+8 < 32
@	  mov	r3,r5,lsr #$1+8
@	  orr	r3,r3,r6,lsl #32-$1-8
@	  mov	r2,r4,lsr #$1+8	  
@	  orr	r2,r2,r5,lsl #32-$1-8
@------------changed by Harry
	  mov	r2,r5,lsl #32-$1-8
	  mov	r3,r6,lsl #32-$1-8
	  orr	r2,r2,r4,lsr #$1+8
	  orr	r3,r3,r5,lsr #$1+8
    .else
	  mov	r2,r5
	  mov	r3,r6
    .endif

	.if $1 > 0
@	  mov	r4,r4,lsr #$1
@	  mov	r6,r6,lsl #32-$1
@	  orr	r4,r4,r5,lsl #32-$1
@	  orr	r5,r6,r5,lsr #$1
@------------changed by Harry
	  mov	r4,r4,lsr #$1
	  orr	r4,r4,r5,lsl #32-$1
	  mov	r5,r5,lsr #$1
	  orr	r5,r5,r6,lsl #32-$1  
	.endif
	uhadd8	r4, r4, r2
	uhadd8	r5, r5, r3
  .else
	@read result r8,r9 and r2,r3 (one pixel to right)
	@r10,r11 can be used

    ldr		r9,[r0,#4]
    ldr		r10,[r0,#8]
    ldr		r8,[r0],r12

    .if $1+8 < 32
@	  mov	r3,r9,lsr #$1+8
@	  orr	r3,r3,r10,lsl #32-$1-8
@	  mov	r2,r8,lsr #$1+8	  
@	  orr	r2,r2,r9,lsl #32-$1-8
@------------changed by Harry
	  mov	r2,r9,lsl #32-$1-8
	  mov	r3,r10,lsl #32-$1-8
	  orr	r2,r2,r8,lsr #$1+8
	  orr	r3,r3,r9,lsr #$1+8
    .else
	  mov	r2,r9
	  mov	r3,r10
    .endif

	.if $1 > 0
@	  mov	r8,r8,lsr #$1
@	  mov	r10,r10,lsl #32-$1
@	  orr	r8,r8,r9,lsl #32-$1
@	  orr	r9,r10,r9,lsr #$1
@------------changed by Harry
	  mov	r8,r8,lsr #$1
	  orr	r8,r8,r9,lsl #32-$1
	  mov	r9,r9,lsr #$1
	  orr	r9,r9,r10,lsl #32-$1  
	.endif
	uhadd8	r8, r8, r2
	uhadd8	r9, r9, r3	
  .endif
  .endmacro

  .macro CopyHorVerRow_ARMv6 @Id, Pos, Parity, Round, Add, ARMv6

@r14 03030303
@r12 3f3f3f3f

	LoadHorVerRow_ARMv6 $0,$1,$2, $5

	.if $3>0
	  mov r3,r14				@0x01010101
	.else
	  mov r3, r14, lsl #1		@0x02020202
	.endif 
		
    .if $2>0
	uqadd8  r8, r8, r3
	uqadd8  r9, r9, r3	
		   
	uhadd8	r8, r8, r4
	uhadd8	r9, r9, r5
@	    ldr	r2,[sp,#8]  @dstpitch
	    ldr	r3,[sp,#4]	@end src for loop compare 	    
	  .if $4>0
	  ldrd	r10,[r1]
	.if $3>0
	.else
	uqadd8  r8, r8, r14
	uqadd8  r9, r9, r14	
	.endif	
	cmp		r0,r3				  
	uhadd8	r8, r10, r8
	uhadd8	r9, r11, r9
	.else	
	cmp		r0,r3	
	.endif		  

	  strd	r8,[r1],r7 

	.else

	uqadd8  r4, r4, r3
	uqadd8  r5, r5, r3	
		   
	uhadd8	r4, r4, r8
	uhadd8	r5, r5, r9
@	    ldr	r2,[sp,#8]  @dstpitch
	    	
	  .if $4>0
	  ldrd	r10,[r1]
	.if $3>0
	.else
	uqadd8  r4, r4, r14
	uqadd8  r5, r5, r14	
	.endif			  
	uhadd8	r4, r10, r4
	uhadd8	r5, r11, r5
	  .endif
	  strd	r4,[r1],r7   
	.endif
  .endmacro

@---------------------------------------------------

@   GenCodeCpy			$Name,			$Sub,		$Round,	$Add,	$Fast,	$ARM5,	$ARMv6,	$ARM11
@@ for ARMv4
@	GenCodeCpy		Armv4Copy8x8,		ArmCopy8x8,	0,	0,	0,	1,	0,	0	@ok
@	GenCodeCpy		Armv4Copy16x16,		ArmCopy16x16,	0,	0,	0,	1,	0,	0	@ok
@	GenCodeCpy		Armv4Inter8x8,		ArmInter8x8,	0,	0,	1,	1,	0,	0	@ok
	
@	GenCodeCpy		Armv4Inter8x8V,		ArmInter8x8V,	0,	0,	1,	1,	0,	0	@ok
@	GenCodeCpy		Armv4Inter8x8H,		ArmInter8x8H,	0,	0,	1,	1,	0,	0	@ok
@	GenCodeCpy		Armv4Inter8x8HV,	ArmInter8x8HV,	0,	0,	1,	1,	0,	0	@ok
	
@	GenCodeCpy		Armv4Inter8x8VRD,	ArmInter8x8V,	1,	0,	1,	1,	0,	0	@ok
@	GenCodeCpy		Armv4Inter8x8HRD,	ArmInter8x8H,	1,	0,	1,	1,	0,	0	@ok
@	GenCodeCpy		Armv4Inter8x8HVRD,	ArmInter8x8HV,	1,	0,	1,	1,	0,	0	@ok
                                                                  	  	  	  	
@	GenCodeCpy		Armv4Inter2_8x8V,	ArmInter8x8V,	0,	1,	1,	1,	0,	0	@ok
@	GenCodeCpy		Armv4Inter2_8x8H,	ArmInter8x8H,	0,	1,	1,	1,	0,	0	@ok
@	GenCodeCpy		Armv4Inter2_8x8HV,	ArmInter8x8HV,	0,	1,	1,	1,	0,	0	@ok
@	GenCodeCpy		Armv4Inter2_8x8,	ArmInter2_8x8,	0,	1,	1,	1,	0,	0	@ok

@ for ARMv6
	GenCodeCpy		Arm11Copy8x8,		ArmCopy8x8,	0,	0,	0,	1,	1,	0	@ok
	GenCodeCpy		Arm11Copy16x16,		ArmCopy16x16,	0,	0,	0,	1,	1,	0	@ok
	GenCodeCpy		Arm11Inter8x8,		ArmInter8x8,	0,	0,	1,	1,	1,	0	@ok
	
	GenCodeCpy		Arm11Inter8x8V,		ArmInter8x8V,	0,	0,	1,	1,	1,	0	@ok
	GenCodeCpy		Arm11Inter8x8H,		ArmInter8x8H,	0,	0,	1,	1,	1,	0	@ok
	GenCodeCpy		Arm11Inter8x8HV,	ArmInter8x8HV,	0,	0,	1,	1,	0,	0	@ok
	
	GenCodeCpy		Arm11Inter8x8VRD,	ArmInter8x8V,	1,	0,	1,	1,	1,	0	@ok
	GenCodeCpy		Arm11Inter8x8HRD,	ArmInter8x8H,	1,	0,	1,	1,	1,	0	@ok
	GenCodeCpy		Arm11Inter8x8HVRD,	ArmInter8x8HV,	1,	0,	1,	1,	0,	0	@ok
                                                                  	  	  	  	
	GenCodeCpy		Arm11Inter2_8x8V,	ArmInter8x8V,	0,	1,	1,	1,	1,	0	@ok
	GenCodeCpy		Arm11Inter2_8x8H,	ArmInter8x8H,	0,	1,	1,	1,	1,	0	@ok
	GenCodeCpy		Arm11Inter2_8x8HV,	ArmInter8x8HV,	0,	1,	1,	1,	0,	0	@ok
	GenCodeCpy		Arm11Inter2_8x8,	ArmInter2_8x8,	0,	1,	1,	1,	1,	0	@ok	

	@.END

