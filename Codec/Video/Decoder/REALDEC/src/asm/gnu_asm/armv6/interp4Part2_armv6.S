@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2009					*
@*																			*
@*****************************************************************************

#include "../../voASMPort.h"

	@AREA	|.text|, CODE
	.section  .text
	
	.global	ARMV6_MCCopyChroma4_H00V00
	.global	ARMV6_MCCopyChroma4_H01V00
	.global	ARMV6_MCCopyChroma4_H02V00
	.global	ARMV6_MCCopyChroma4_H03V00
	.global	ARMV6_MCCopyChroma4_H00V01
	.global	ARMV6_MCCopyChroma4_H01V01
	.global	ARMV6_MCCopyChroma4_H02V01
	.global	ARMV6_MCCopyChroma4_H03V01
	.global	ARMV6_MCCopyChroma4_H00V02
	.global	ARMV6_MCCopyChroma4_H01V02
	.global	ARMV6_MCCopyChroma4_H02V02
	.global	ARMV6_MCCopyChroma4_H03V02
	.global	ARMV6_MCCopyChroma4_H00V03
	.global	ARMV6_MCCopyChroma4_H01V03
	.global	ARMV6_MCCopyChroma4_H02V03


@huwei 20080911 bug_fixed
ARMV6_MCCopyChroma4_H00V00:	@PROC

@	pld		[r0]
@	pld		[r0,r2]
	ands    r12,r0,#0x03
	bne		H00V00_lab1

H00V00_lab0:	
	stmfd   sp!,{r4,lr}
	ldr		r12,[r0],r2
	ldr		lr,[r0],r2
	ldr		r4,[r0],r2
	ldr		r2,[r0]
	str		r12,[r1],r3
	str		lr,[r1],r3
	str		r4,[r1],r3
	str		r2,[r1]
	ldmfd   sp!,{r4,pc}

H00V00_lab1:	
	stmfd   sp!,{r4-r11,lr}
	sub		r0,r0,r12
	mov		r12,r12,lsl #3
	sub		r2,r2,#0x04
	rsb		lr,r12,#0x20
	ldr		r4,[r0],#0x04
	ldr		r5,[r0],r2
	ldr		r6,[r0],#0x04
	ldr		r7,[r0],r2
	mov		r4,r4,LSR r12
	ldr		r8,[r0],#0x04
	orr		r4,r4,r5,lsl lr
	ldr		r9,[r0],r2
	mov		r6,r6,LSR r12
	ldr		r10,[r0],#0x04
	orr		r6,r6,r7,lsl lr
	ldr		r11,[r0]	
	str		r4,[r1],r3
	mov		r8,r8,LSR r12
	str		r6,[r1],r3
	mov		r10,r10,LSR r12
	orr		r8,r8,r9,lsl lr
	str		r8,[r1],r3
	orr		r10,r10,r11,lsl lr
	str		r10,[r1]
	ldmfd   sp!,{r4-r11,pc}
	
	@ENDP


ARMV6_MCCopyChroma4_H01V00:	@PROC
  
    stmfd    sp!,{r4-r9,lr}
@    pld		 [r0]
	mov		 lr,#4
	
H01V00_loop:
    ldrb     r4,[r0,#0]
    ldrb     r5,[r0,#1]
	ldrb     r6,[r0,#2]
	ldrb     r7,[r0,#3]
	ldrb     r8,[r0,#4]
	add      r0,r0,r2
    subs     lr,lr,#1
@    pld		 [r0]

	add		 r9,r5,#1
	add		 r4,r4,r4,lsl #1
	add		 r9,r9,r4
	add		 r12,r6,#1
	mov		 r9,r9,asr #2	
	add		 r5,r5,r5,lsl #1
	add		 r12,r12,r5
	add		 r4,r7,#1
	mov		 r12,r12,asr #2	
	add		 r6,r6,r6,lsl #1
	orr		 r9,r9,r12,lsl #8
	add		 r4,r4,r6
	add		 r12,r8,#1
	add		 r7,r7,r7,lsl #1
	add		 r12,r12,r7
	mov		 r4,r4,asr #2
	mov		 r12,r12,asr #2
	orr		 r9,r9,r4,lsl #16
	orr		 r9,r9,r12,lsl #24
			
    str      r9,[r1],r3
    bgt      H01V00_loop
    
    ldmfd    sp!,{r4-r9,pc}
	@ENDP


ARMV6_MCCopyChroma4_H02V00:	@PROC
 
    stmfd    sp!,{r4-r8,lr}
@    pld		 [r0]
    mov      r4,#4
    
H02V00_loop:
    ldrb     r5,[r0,#0]
    ldrb     r6,[r0,#1]
	ldrb     r7,[r0,#2]
	ldrb	 r8,[r0,#3]	
	ldrb	 lr,[r0,#4]	
	add      r0,r0,r2
	subs     r4,r4,#1
@    pld		 [r0]

	add		 r5,r5,#1
	add		 r5,r5,r6
	add		 r6,r6,#1
	mov		 r12,r5,asr #1
	add		 r6,r6,r7
	add		 r7,r7,#1
	mov		 r6,r6,asr #1
	add		 r5,r7,r8
	add		 r8,r8,#1
	orr		 r12,r12,r6,lsl #8
	add		 r8,r8,lr
	mov		 r5,r5,asr #1
	mov		 r6,r8,asr #1
	orr		 r12,r12,r5,lsl #16
	orr		 r12,r12,r6,lsl #24		
    str      r12,[r1],r3
	
    bgt      H02V00_loop
    
    ldmfd    sp!,{r4-r8,pc}
	@ENDP
	

ARMV6_MCCopyChroma4_H03V00:	@PROC 
 
    stmfd    sp!,{r4-r9,lr}
@    pld		 [r0]
	mov		 lr,#4
	
H03V00_loop:
    ldrb     r4,[r0,#0]
    ldrb     r5,[r0,#1]
	ldrb     r6,[r0,#2]
	ldrb     r7,[r0,#3]
	ldrb     r8,[r0,#4]
	add      r0,r0,r2
    subs     lr,lr,#1
@    pld		 [r0]

	add		 r9,r4,#1
	add		 r4,r5,r5,lsl #1
	add		 r9,r9,r4
	add		 r12,r5,#1
	mov		 r9,r9,asr #2	
	add		 r5,r6,r6,lsl #1
	add		 r12,r12,r5
	add		 r4,r6,#1
	mov		 r12,r12,asr #2	
	add		 r6,r7,r7,lsl #1
	orr		 r9,r9,r12,lsl #8
	add		 r4,r4,r6
	add		 r12,r7,#1
	add		 r7,r8,r8,lsl #1
	add		 r12,r12,r7
	mov		 r4,r4,asr #2
	mov		 r12,r12,asr #2
	orr		 r9,r9,r4,lsl #16
	orr		 r9,r9,r12,lsl #24
			
    str      r9,[r1],r3
    bgt      H03V00_loop
    
    ldmfd    sp!,{r4-r9,pc}
	@ENDP


ARMV6_MCCopyChroma4_H00V01:	@PROC
    
    stmfd    sp!,{r4-r9,lr}
@    pld		 [r0]
@    pld		 [r0,r2]
	add      r1,r1,r3,lsl #2
	sub		 r1,r1,#1
	mov		 lr,#4
	
H00V01_loop:
    ldrb     r4,[r0],r2
    subs     lr,lr,#1
    ldrb     r5,[r0],r2
	sub      r1,r1,r3,lsl #2
	ldrb     r6,[r0],r2
	add		 r1,r1,#1
	ldrb     r7,[r0],r2
	add		 r9,r5,#2
	ldrb     r8,[r0]
	sub      r0,r0,r2,lsl #2
	add		 r0,r0,#1
	add		 r4,r4,r4,lsl #1
	add		 r9,r9,r4
	add		 r12,r6,#2
	mov		 r9,r9,asr #2	
	add		 r5,r5,r5,lsl #1
    strb     r9,[r1],r3
	add		 r12,r12,r5
	add		 r4,r7,#2
	mov		 r12,r12,asr #2	
	add		 r6,r6,r6,lsl #1
    strb     r12,[r1],r3
	add		 r4,r4,r6
	add		 r12,r8,#2
	mov		 r4,r4,asr #2
	add		 r7,r7,r7,lsl #1
	add		 r12,r12,r7
    strb     r4,[r1],r3
	mov		 r12,r12,asr #2
    strb     r12,[r1],r3
			
    bgt      H00V01_loop
    
    ldmfd    sp!,{r4-r9,pc}
	@ENDP		


ARMV6_MCCopyChroma4_H01V01:	@PROC

    stmfd    sp!,{r4-r11,lr}	
@    pld		 [r0]
@    pld		 [r0,r2]
	mov		 r12,#7
    add      r4,r0,r2
	mov		 r5,#9	
    mov      lr,#4
    
H01V01_loop:
	ldrb     r11,[r0,#0]@lTemp0				
    ldrb     r6,[r0,#1]	@lTemp1
	ldrb     r7,[r4,#0]	@lTemp2
	ldrb     r8,[r4,#1]	@lTemp3		
	
	smlabb	 r11,r5,r11,r12
	ldrb	 r9,[r0,#2]	
	add		 r7,r6,r7			 
	ldrb	 r10,[r4,#2]	
	add	     r7,r7,r7,lsl #1	
	add		 r11,r7,r11
	add		 r11,r11,r8		

	smlabb	 r6,r5,r6,r12
	mov		 r11,r11,asr #4	
	add		 r8,r9,r8			 
	ldrb	 r7,[r0,#3]	
	add	     r8,r8,r8,lsl #1	
	add		 r6,r8,r6
	add		 r6,r6,r10		
	ldrb	 r8,[r4,#3]	
	mov		 r6,r6,asr #4	

	smlabb	 r9,r5,r9,r12
	orr		 r11,r11,r6,lsl #8
	add		 r10,r7,r10			 
	ldrb	 r6,[r0,#4]	
	add	     r10,r10,r10,lsl #1	
	add		 r9,r10,r9
	add		 r9,r9,r8		
	ldrb	 r10,[r4,#4]	
	mov		 r9,r9,asr #4	

	smlabb	 r7,r5,r7,r12
	add		 r8,r6,r8			 
	orr		 r11,r11,r9,lsl #16
	add	     r8,r8,r8,lsl #1	
	add		 r7,r8,r7
	add		 r7,r7,r10		
    add      r4,r4,r2
	mov		 r7,r7,asr #4	
    add      r0,r0,r2
	orr		 r11,r11,r7,lsl #24

@    pld		 [r0,r2]
	subs	 lr,lr,#1	
    str      r11,[r1],r3
    bgt      H01V01_loop
    
    ldmfd    sp!,{r4-r11,pc}
	@ENDP	
	
	
ARMV6_MCCopyChroma4_H02V01:	@PROC

    stmfd    sp!,{r4-r11,lr}
@    pld		 [r0]
@    pld		 [r0,r2]
	mov		 r12,#4		
    add      r4,r0,r2
	mov		 r5,#3		
    mov      lr,#4
    
H02V01_loop:
	ldrb     r11,[r0,#0]@lTemp0				
    ldrb     r6,[r0,#1]	@lTemp1
	ldrb     r7,[r4,#0]	@lTemp2
	ldrb     r8,[r4,#1]	@lTemp3		
	
	add		 r11,r11,r6		
	ldrb	 r9,[r0,#2]	
	smlabb	 r11,r5,r11,r12
	add		 r7,r7,r8
	add		 r6,r6,r9		
	ldrb	 r10,[r4,#2]	
	add		 r7,r7,r11	
	
	smlabb	 r6,r5,r6,r12
	mov		 r11,r7,asr #3
	add		 r8,r8,r10
	add		 r8,r8,r6
	ldrb	 r7,[r0,#3]			
	mov		 r8,r8,asr #3			
	ldrb	 r6,[r4,#3]	
	add		 r9,r9,r7
	orr		 r11,r11,r8,lsl #8
				
	smlabb	 r9,r5,r9,r12
	ldrb	 r8,[r0,#4]
	add		 r10,r10,r6	
	add		 r10,r10,r9
	ldrb	 r9,[r4,#4]
	add		 r7,r7,r8				
	mov		 r10,r10,asr #3			
	
	smlabb	 r7,r5,r7,r12
	orr		 r11,r11,r10,lsl #16
	add		 r6,r6,r9
	add		 r6,r6,r7
	add		 r0,r0,r2		
	mov		 r6,r6,asr #3			
@    pld		 [r0,r2]
    add      r4,r4,r2
	orr		 r11,r11,r6,lsl #24

	subs	 lr,lr,#1	
    str      r11,[r1],r3
    bgt      H02V01_loop

    ldmfd    sp!,{r4-r11,pc}
	@ENDP	
	
	
ARMV6_MCCopyChroma4_H03V01:	@PROC

    stmfd    sp!,{r4-r11,lr}
@    pld		 [r0]
@    pld		 [r0,r2]
	mov		 r12,#7		
    add      r4,r0,r2
	mov		 r5,#9		
    mov      lr,#4
    @str		 r3,[sp,#-4]
    sub      sp,sp,#0x04
    str      r3,[sp,#0]
    
H03V01_loop:
 	ldrb     r11,[r0,#0]@lTemp0				
	ldrb     r8,[r4,#1]	@lTemp3		
    ldrb     r6,[r0,#1]	@lTemp1
	ldrb     r7,[r4,#0]	@lTemp2
	ldrb	 r10,[r4,#2]
	
	subs	 lr,lr,#1
	add		 r11,r11,r8			 
	smlabb	 r9,r5,r6,r7
	add	     r11,r11,r11,lsl #1		
	ldrb	 r7,[r0,#2]		
	add		 r11,r11,r12
	add		 r11,r11,r9
		
	add		 r6,r6,r10	
	smlabb	 r9,r5,r7,r8		 
	mov		 r11,r11,asr #4	
	add	     r8,r6,r6,lsl #1	
	ldrb	 r6,[r0,#3]	
	add		 r8,r8,r12
	add		 r9,r8,r9
	ldrb	 r8,[r4,#3]		
	mov		 r3,r9,asr #4	

	smlabb	 r9,r5,r6,r10
	add		 r7,r7,r8			 
	orr		 r11,r11,r3,lsl #8	
	add	     r10,r7,r7,lsl #1	
	ldrb	 r7,[r0,#4]	
	add		 r10,r10,r12
	add		 r9,r10,r9
	ldrb	 r10,[r4,#4]	
	mov		 r3,r9,asr #4	
	
	smlabb	 r9,r5,r7,r8  
    add      r4,r4,r2
	add		 r6,r6,r10			 
	orr		 r11,r11,r3,lsl #16
	add	     r6,r6,r6,lsl #1	
	add		 r8,r6,r12
	add		 r9,r8,r9      
  @ldr		 r3,[sp,#-4]
  ldr    r3,[sp,#0]
	mov		 r9,r9,asr #4	
	add      r0,r0,r2	
	orr		 r11,r11,r9,lsl #24

@    pld		 [r0,r2]
    str      r11,[r1],r3
    bgt      H03V01_loop
    
    add      sp,sp,#0x04    
    ldmfd    sp!,{r4-r11,pc}
	@ENDP	
	
	
ARMV6_MCCopyChroma4_H00V02:	@PROC
   
    stmfd    sp!,{r4-r8,lr}
@    pld		 [r0]
@    pld		 [r0,r2]
	add      r1,r1,r3,lsl #2
	sub		 r1,r1,#1
    mov      r4,#4
    
H00V02_loop:
    ldrb     r5,[r0],r2
	sub      r1,r1,r3,lsl #2
    ldrb     r6,[r0],r2
	add		 r1,r1,#1
	ldrb     r7,[r0],r2
	subs     r4,r4,#1
	ldrb	 r8,[r0],r2	
	add		 r5,r5,r6
	ldrb	 lr,[r0]	
	sub      r0,r0,r2,lsl #2
	add		 r6,r6,r7
	add		 r0,r0,#1
	mov		 r5,r5,asr #1
	mov		 r6,r6,asr #1
    strb     r5,[r1],r3
	add		 r7,r7,r8
    strb     r6,[r1],r3
	mov		 r7,r7,asr #1
	add		 r8,r8,lr
    strb     r7,[r1],r3
	mov		 r8,r8,asr #1
    strb     r8,[r1],r3
	
    bgt      H00V02_loop
    
    ldmfd    sp!,{r4-r8,pc}
	@ENDP	
	
	
ARMV6_MCCopyChroma4_H01V02:	@PROC

    stmfd    sp!,{r4-r11,lr}
@    pld		 [r0]
@    pld		 [r0,r2]
	mov		 r12,#4		
    add      r4,r0,r2
	mov		 r5,#3		
    mov      lr,#4
    
H01V02_loop:
	ldrb     r11,[r0,#0]@lTemp0				
	ldrb     r7,[r4,#0]	@lTemp2
    ldrb     r6,[r0,#1]	@lTemp1
	ldrb     r8,[r4,#1]	@lTemp3		
	
	add		 r11,r11,r7		
	ldrb	 r9,[r0,#2]	
	smlabb	 r11,r5,r11,r12
	add		 r6,r6,r8		
	ldrb	 r10,[r4,#2]	
	add		 r7,r6,r11	
	
	smlabb	 r6,r5,r6,r12
	mov		 r11,r7,asr #3
	add		 r8,r9,r10
	add		 r6,r8,r6
	ldrb	 r7,[r0,#3]			
	mov		 r10,r6,asr #3			
	ldrb	 r6,[r4,#3]	
	orr		 r11,r11,r10,lsl #8
				
	smlabb	 r9,r5,r8,r12
	ldrb	 r8,[r0,#4]
	add		 r6,r7,r6	
	ldrb	 r7,[r4,#4]
	add		 r9,r6,r9
		
	smlabb	 r6,r5,r6,r12
	mov		 r9,r9,asr #3			
	add		 r8,r8,r7
	orr		 r11,r11,r9,lsl #16
	add		 r6,r6,r8
	add		 r0,r0,r2		
	mov		 r6,r6,asr #3			
@    pld		 [r0,r2]
    add      r4,r4,r2
	orr		 r11,r11,r6,lsl #24

	subs	 lr,lr,#1	
    str      r11,[r1],r3
    bgt      H01V02_loop

    ldmfd    sp!,{r4-r11,pc}
	@ENDP	
	
	
ARMV6_MCCopyChroma4_H02V02:	@PROC

    stmfd    sp!,{r4-r10,lr}
@    pld		 [r0]
@    pld		 [r0,r2]
    add      r4,r0,r2
    mov      lr,#4
    
H02V02_loop:
    ldrb     r5,[r0,#0]
    ldrb     r6,[r0,#1]
    ldrb     r7,[r0,#2]
    ldrb     r8,[r4,#0]
    ldrb     r9,[r4,#1]
    ldrb     r10,[r4,#2]

    add      r6,r6,#1
    add      r5,r5,r8
    add      r6,r6,r9
    add      r5,r5,r6    
    ldrb     r8,[r0,#3]
    ldrb     r9,[r4,#3]
    add      r7,r7,r10
    mov      r12,r5,LSR #2
    add      r6,r6,r7
    ldrb     r5,[r0,#4]
    mov      r6,r6,LSR #2
    add      r8,r8,#1
    add      r8,r8,r9
    orr      r12,r12,r6,lsl #8    
    ldrb     r6,[r4,#4]
    add      r0,r0,r2
@    pld		 [r0,r2]    
    add      r7,r7,r8   
    add      r5,r5,r6
    mov      r7,r7,LSR #2
    add      r5,r5,r8
    orr      r12,r12,r7,lsl #16
    mov      r5,r5,LSR #2
    add      r4,r4,r2
    orr      r12,r12,r5,lsl #24    
    subs     lr,lr,#1
    str      r12,[r1],r3
    bgt      H02V02_loop
    
    ldmfd    sp!,{r4-r10,pc}
	@ENDP	


	
ARMV6_MCCopyChroma4_H03V02:	@PROC

    stmfd    sp!,{r4-r11,lr}
@    pld		 [r0]
@    pld		 [r0,r2]
	mov		 r12,#4		
    add      r4,r0,r2
	mov		 r5,#3		
    mov      lr,#4
    
H03V02_loop:
    ldrb     r6,[r0,#1]	@lTemp1
	ldrb     r8,[r4,#1]	@lTemp3		
	ldrb     r11,[r0,#0]@lTemp0				
	ldrb     r7,[r4,#0]	@lTemp2
	
	add		 r6,r6,r8		
	ldrb	 r9,[r0,#2]	
	smlabb	 r8,r5,r6,r12
	ldrb	 r10,[r4,#2]	
	add		 r11,r11,r7		
	subs	 lr,lr,#1
	
	add		 r9,r9,r10
	add		 r7,r8,r11	
	smlabb	 r10,r5,r9,r12
	mov		 r11,r7,asr #3
	ldrb	 r7,[r0,#3]			
	add		 r6,r10,r6
	ldrb	 r10,[r4,#3]	
	mov		 r6,r6,asr #3			
		
	ldrb	 r8,[r0,#4]				
	orr		 r11,r11,r6,lsl #8
	add		 r7,r7,r10
	ldrb	 r6,[r4,#4]
	add		 r10,r7,r7, lsl #1
	add		 r10,r10,r12
			
	add		 r8,r8,r6
	add		 r9,r9,r10	
	smlabb	 r6,r5,r8,r12
	mov		 r9,r9,asr #3			
	add		 r0,r0,r2		
@    pld		 [r0,r2]
	add		 r6,r6,r7
	orr		 r11,r11,r9,lsl #16
	mov		 r6,r6,asr #3			
    add      r4,r4,r2
	orr		 r11,r11,r6,lsl #24

    str      r11,[r1],r3
    bgt      H03V02_loop
    
    ldmfd    sp!,{r4-r11,pc}
	@ENDP	


ARMV6_MCCopyChroma4_H00V03:	@PROC

    stmfd    sp!,{r4-r9,lr}
@    pld		 [r0]
@    pld		 [r0,r2]
	add      r1,r1,r3,lsl #2
	sub		 r1,r1,#1
	mov		 lr,#4
	
H00V03_loop:
    ldrb     r4,[r0],r2
	sub      r1,r1,r3,lsl #2
    ldrb     r5,[r0],r2
	add		 r1,r1,#1
	ldrb     r6,[r0],r2
    subs     lr,lr,#1
	ldrb     r7,[r0],r2
	add		 r9,r4,#2
	ldrb     r8,[r0]
	sub      r0,r0,r2,lsl #2
	add		 r4,r5,r5,lsl #1
	add		 r0,r0,#1
	add		 r9,r9,r4
	add		 r12,r5,#2
	mov		 r9,r9,asr #2	
	add		 r5,r6,r6,lsl #1
    strb     r9,[r1],r3
	add		 r12,r12,r5
	add		 r4,r6,#2
	mov		 r12,r12,asr #2	
	add		 r6,r7,r7,lsl #1
    strb     r12,[r1],r3
	add		 r4,r4,r6
	add		 r12,r7,#2
	add		 r7,r8,r8,lsl #1
	add		 r12,r12,r7
	mov		 r4,r4,asr #2
    strb     r4,[r1],r3
	mov		 r12,r12,asr #2
    strb     r12,[r1],r3
			
    bgt      H00V03_loop
    
    ldmfd    sp!,{r4-r9,pc}
	@ENDP	
	
	
ARMV6_MCCopyChroma4_H01V03:	@PROC

    stmfd    sp!,{r4-r11,lr}
@    pld		 [r0]
@    pld		 [r0,r2]
	mov		 r12,#7		
    add      r4,r0,r2
	mov		 r5,#9		
    mov      lr,#4
    @str		 r3,[sp,#-4]
    sub    sp,sp,#0x04
    str    r3,[sp,#0]
    
H01V03_loop:
 	ldrb     r11,[r0,#0]@lTemp0				
	ldrb     r8,[r4,#1]	@lTemp3		
    ldrb     r6,[r0,#1]	@lTemp1
	ldrb     r7,[r4,#0]	@lTemp2
	ldrb	 r10,[r4,#2]
	
	subs	 lr,lr,#1
	add		 r11,r11,r8			 
	smlabb	 r9,r5,r7,r6
	add	     r11,r11,r11,lsl #1		
	ldrb	 r7,[r0,#2]	
	add		 r11,r11,r12
	add		 r11,r11,r9
	
	add		 r6,r6,r10		
	smlabb	 r9,r5,r8,r7	 
	mov		 r11,r11,asr #4	
	add	     r8,r6,r6,lsl #1	
	ldrb	 r6,[r0,#3]	
	add		 r8,r8,r12
	add		 r9,r8,r9
	ldrb	 r8,[r4,#3]		
	mov		 r3,r9,asr #4	

	smlabb	 r9,r5,r10,r6	
	add		 r7,r7,r8	
	orr		 r11,r11,r3,lsl #8		 		
	add	     r10,r7,r7,lsl #1	
	ldrb	 r7,[r0,#4]	
	add		 r10,r10,r12
	add		 r9,r10,r9
	ldrb	 r10,[r4,#4]	
	mov		 r3,r9,asr #4	
	
	smlabb	 r9,r5,r8,r7  
    add      r4,r4,r2
	add		 r6,r6,r10			 
	orr		 r11,r11,r3,lsl #16
	add	     r6,r6,r6,lsl #1	
	add		 r8,r6,r12
	add		 r9,r8,r9      
  @ldr		 r3,[sp,#-4]
  ldr    r3,[sp,#0]
	mov		 r9,r9,asr #4	
	add      r0,r0,r2	
	orr		 r11,r11,r9,lsl #24

@    pld		 [r0,r2]
    str      r11,[r1],r3
    bgt      H01V03_loop
    
    add    sp,sp,#0x04
    ldmfd    sp!,{r4-r11,pc}	
    @ENDP	
	
	
ARMV6_MCCopyChroma4_H02V03:	@PROC

    stmfd    sp!,{r4-r10,lr}
@    pld		 [r0]
@    pld		 [r0,r2]
    add      r4,r0,r2
    mov      lr,#4
    
H02V03_loop:
	ldrb     r8,[r4,#0]	
	ldrb     r9,[r4,#1]	
	ldrb     r5,[r0,#0]		
    ldrb     r6,[r0,#1]	
    ldrb     r7,[r0,#2]	
	ldrb     r10,[r4,#2]		
	
	subs	 lr,lr,#1
	add		 r8,r8,r9	
	add		 r5,r5,r6		
	add		 r8,r8,r8,lsl #1
	add		 r5,r5,r8	
	add		 r5,r5,#4		
	add		 r9,r9,r10	
	mov		 r12,r5,asr #3
	
	add		 r6,r6,r7	
	add		 r9,r9,r9,lsl #1
	add		 r6,r6,r9	
	ldrb	 r5,[r0,#3]	
	add		 r6,r6,#4	
	ldrb	 r8,[r4,#3]		
	mov		 r6,r6,asr #3	
		
	ldrb	 r9,[r4,#4]	
	orr		 r12,r12,r6,lsl #8		
	ldrb	 r6,[r0,#4]	
	add		 r10,r10,r8	
	add		 r7,r7,r5	
	add		 r10,r10,r10,lsl #1
	add		 r7,r7,r10	
	add		 r7,r7,#4	
	add		 r8,r9,r8	
	mov		 r7,r7,asr #3
	add		 r5,r6,r5	
		
	add		 r8,r8,r8,lsl #1
	orr		 r12,r12,r7,lsl #16
	add		 r5,r5,r8	
	add		 r5,r5,#4	
	add		 r0,r0,r2		
@    pld		 [r0,r2]
	mov		 r5,r5,asr #3
    add      r4,r4,r2
	orr		 r12,r12,r5,lsl #24

    str      r12,[r1],r3
    bgt      H02V03_loop
    
    ldmfd    sp!,{r4-r10,pc}
	@ENDP	
	
		
	@END



