@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2009					*
@*																			*
@*****************************************************************************

#include "../../voASMPort.h"

	@AREA	|.text|, CODE
	.section  .text
	
	.global	ARMV6_Interpolate4Add_H00V00
	.global	ARMV6_Interpolate4Add_H01V00
	.global	ARMV6_Interpolate4Add_H02V00
	.global	ARMV6_Interpolate4Add_H03V00
	.global	ARMV6_Interpolate4Add_H00V01
	.global	ARMV6_Interpolate4Add_H01V01
	.global	ARMV6_Interpolate4Add_H02V01
	.global  ARMV6_Interpolate4Add_H03V01
	.global	ARMV6_Interpolate4Add_H00V02
	.global	ARMV6_Interpolate4Add_H01V02
	.global	ARMV6_Interpolate4Add_H02V02
	.global	ARMV6_Interpolate4Add_H03V02
	.global	ARMV6_Interpolate4Add_H00V03
	.global	ARMV6_Interpolate4Add_H01V03
	.global	ARMV6_Interpolate4Add_H02V03
	.global	ARMV6_Interpolate4Add_H03V03
	.global	ARMV6_MCCopyChroma4Add_H00V00
	.global	ARMV6_MCCopyChroma4Add_H01V00
	.global	ARMV6_MCCopyChroma4Add_H02V00
	.global	ARMV6_MCCopyChroma4Add_H03V00
	.global	ARMV6_MCCopyChroma4Add_H00V01
	.global	ARMV6_MCCopyChroma4Add_H01V01
	.global	ARMV6_MCCopyChroma4Add_H02V01 
	.global	ARMV6_MCCopyChroma4Add_H03V01
	.global	ARMV6_MCCopyChroma4Add_H00V02
	.global	ARMV6_MCCopyChroma4Add_H01V02
	.global	ARMV6_MCCopyChroma4Add_H02V02
	.global	ARMV6_MCCopyChroma4Add_H03V02
	.global	ARMV6_MCCopyChroma4Add_H00V03
	.global	ARMV6_MCCopyChroma4Add_H01V03
	.global	ARMV6_MCCopyChroma4Add_H02V03
	
@R0 = pSrc
@R1 = pDst
@R2 = uSrcPitch
@R3 = uDstPitch


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	No interpolation required, simple block copy.
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
ARMV6_Interpolate4Add_H00V00:	@PROC

    stmdb    sp!,{r4-r11,lr}
	ands    r12,r0,#0x03
	bne		H00V00_notAligned4

H00V00_Aligned4:
	mov		r12, r1
	ldr		r5, [r0,#4]
	ldr		r9, [r12,#4]
	ldr		r4, [r0], r2
	ldr		r8, [r12], r3
	ldr		r7, [r0,#4]
	ldr		r11,[r12,#4]
	ldr		r6, [r0], r2 
	ldr		r10,[r12], r3
	uhadd8	r5, r5, r9
	uhadd8	r4, r4, r8
	str		r5, [r1,#4]
	str		r4, [r1], r3
	uhadd8	r7, r7, r11
	uhadd8	r6, r6, r10
	str		r7, [r1,#4]
	ldr		r5, [r0,#4]
	str		r6, [r1], r3
	
	ldr		r9, [r12,#4]
	ldr		r4, [r0], r2
	ldr		r8, [r12], r3
	ldr		r7, [r0,#4]
	ldr		r11,[r12,#4]
	ldr		r6, [r0], r2 
	ldr		r10,[r12], r3
	uhadd8	r5, r5, r9
	uhadd8	r4, r4, r8
	str		r5, [r1,#4]
	str		r4, [r1], r3
	uhadd8	r7, r7, r11
	uhadd8	r6, r6, r10
	str		r7, [r1,#4]
	ldr		r5, [r0,#4]	
	str		r6, [r1], r3
	
	ldr		r9, [r12,#4]
	ldr		r4, [r0], r2
	ldr		r8, [r12], r3
	ldr		r7, [r0,#4]
	ldr		r11,[r12,#4]
	ldr		r6, [r0], r2 
	ldr		r10,[r12], r3
	uhadd8	r5, r5, r9
	uhadd8	r4, r4, r8
	str		r5, [r1,#4]
	str		r4, [r1], r3
	uhadd8	r7, r7, r11
	uhadd8	r6, r6, r10
	str		r7, [r1,#4]
	ldr		r5, [r0,#4]
	str		r6, [r1], r3

	ldr		r9, [r12,#4]
	ldr		r4, [r0], r2
	ldr		r8, [r12], r3
	ldr		r7, [r0,#4]
	ldr		r11,[r12,#4]
	ldr		r6, [r0] 
	ldr		r10,[r12]
	uhadd8	r5, r5, r9
	uhadd8	r4, r4, r8
	str		r5, [r1,#4]
	str		r4, [r1], r3
	uhadd8	r7, r7, r11
	uhadd8	r6, r6, r10
	str		r7, [r1,#4]
	str		r6, [r1]
	
    ldmia   sp!,{r4-r11,pc}

H00V00_notAligned4:

	mov		r10, r1
	sub		r0,r0,r12
	mov		r11, #8
	mov		r12,r12,lsl #3
	rsb		lr,r12,#32	
H00V00_loop:	
	ldr		r5, [r0, #4]
	ldr		r6, [r0, #8]
	ldr		r4, [r0], r2
	ldr		r8, [r10, #4]
	ldr		r7, [r10], r3
	mov		r4, r4, lsr r12
	orr		r4, r4, r5, lsl lr
	mov		r5, r5, lsr r12
	orr		r5, r5, r6, lsl lr
	uhadd8	r5, r5, r8
	uhadd8	r4, r4, r7
 	str		r5, [r1, #4]
 	str		r4, [r1], r3
	ldr		r5, [r0, #4]
	ldr		r6, [r0, #8]
	ldr		r4, [r0], r2
	ldr		r8, [r10, #4]
	ldr		r7, [r10], r3
	subs	r11, r11, #2
	mov		r4, r4, lsr r12
	orr		r4, r4, r5, lsl lr
	mov		r5, r5, lsr r12
	orr		r5, r5, r6, lsl lr
	uhadd8	r5, r5, r8
	uhadd8	r4, r4, r7
 	str		r5, [r1, #4]
 	str		r4, [r1], r3
 	bne		H00V00_loop
 	
    ldmia   sp!,{r4-r11,pc}

	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,52,20,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
ARMV6_Interpolate4Add_H01V00:	@PROC
	    
    stmfd    sp!,{r4-r11,lr}
	pld		 [r0]
	pld		 [r0,r2]
	mov		 r4,#52	    	
	@str	     r2,[sp,#-4]
	sub    sp,sp,#0x08
	str    r2,[sp,#0]   
	mov      lr,#8
	@str	     r3,[sp,#-8]
	str      r3,[sp,#4]
		
H01V00_row_loop:
	mov	     r6,#8
	
H01V00_col_loop:

	@ t0 = ( 52*p2 + 5*(p3<<2 - (p1+p4)) + (p0+p5) + 32 ) >> 6
	
    ldrb     r9,[r0,#0]		@lTemp2
	ldrb     r7,[r0,#-2]	@lTemp0
    ldrb     r8,[r0,#-1]	@lTemp1
    ldrb     r11,[r0,#2]	@lTemp4
    ldrb     r10,[r0,#1]	@lTemp3
	smlabb	 r7,r4,r9,r7     
	add		 r5,r8,r11
	ldrb     r12,[r0,#3]	@lTemp5						
	rsb		 r5,r5,r10,lsl #2
	add		 r7,r7,#0x20
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r7
	add		 r5,r5,r12
	ldrb     r7,[r0,#4]		@lTemp0
    usat     r3,#8,r5,asr #6		

	smlabb	 r8,r4,r10,r8
	add		 r5,r9,r12
	rsb		 r5,r5,r11,lsl #2
	add		 r8,r8,r7	
	add		 r5,r5,r5,lsl #2        
	add		 r5,r5,r8
	add		 r5,r5,#0x20
	ldrb     r8,[r0,#5]		@lTemp1
    usat     r2,#8,r5,asr #6				
	
	smlabb	 r9,r4,r11,r9
	add		 r5,r10,r7
	orr		 r3,r3,r2,lsl #8
	rsb		 r5,r5,r12,lsl #2                
	add		 r9,r9,r8	
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r9
	add		 r5,r5,#0x20
	ldrb     r9,[r0,#6]		@lTemp2
    usat     r2,#8,r5,asr #6				

	smlabb	 r10,r4,r12,r10
	add		 r5,r11,r8
	orr		 r3,r3,r2,lsl #16
	rsb		 r5,r5,r7,lsl #2
	ldr		 r11,[r1]
	add		 r10,r10,r9
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r10
	add		 r5,r5,#0x20
	add      r0,r0,#4
    usat     r2,#8,r5,asr #6
	subs     r6,r6,#4
	orr		 r3,r3,r2,lsl #24
	uhadd8	 r3,r3,r11
    str      r3,[r1],#4

    bgt      H01V00_col_loop
    
	@ldr	     r2,[sp,#-4]
	@ldr	     r3,[sp,#-8]
	ldr    r2,[sp,#0]
	ldr    r3,[sp,#4]
	sub		 r1,r1,#8		
	sub		 r0,r0,#8
	subs     lr,lr,#1
    add		 r0,r0,r2         
	add		 r1,r1,r3
	pld		 [r0,r2]
    bgt      H01V00_row_loop
    
    add    sp,sp,#0x08    
    ldmfd    sp!,{r4-r11,pc}

	@ENDP
		
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,20,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
ARMV6_Interpolate4Add_H02V00:	@PROC

    stmfd    sp!,{r4-r11,lr}
	pld		 [r0]
	pld		 [r0,r2]
    mov      lr,#8 
    @str	     r2,[sp,#-4]
    sub    sp,sp,#0x04
    str    r2,[sp,#0]
    
H02V00_row_loop:
    mov      r6,#8
    
H02V00_col_loop:

    ldrb     r9,[r0,#0]		@lTemp2
	ldrb     r10,[r0,#1]	@lTemp3
	ldrb     r11,[r0,#2]	@lTemp4
    ldrb     r8,[r0,#-1]	@lTemp1        
	add		 r5,r9,r10	
    ldrb     r7,[r0,#-2]	@lTemp0
	rsb		 r5,r8,r5,lsl #2
	ldrb     r12,[r0,#3]	@lTemp5	
	
	sub		 r5,r5,r11
	add		 r7,r7,#0x10
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r7
	add		 r5,r5,r12
	ldrb     r7,[r0,#4]		@lTemp0
    usat     r4,#8,r5,asr #5		

	add		 r5,r10,r11
	add		 r8,r8,#0x10
	rsb		 r5,r9,r5,lsl #2        
	sub		 r5,r5,r12
	add		 r8,r8,r7
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r8
	ldrb     r8,[r0,#5]		@lTemp1
	usat     r2,#8,r5,asr #5
			
	add		 r5,r11,r12
	add		 r9,r9,#0x10
	orr		 r4,r4,r2,lsl #8
	rsb		 r5,r10,r5,lsl #2        
	sub		 r5,r5,r7
	add		 r9,r9,r8 
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r9
	ldrb     r9,[r0,#6]		@lTemp2
	usat     r2,#8,r5,asr #5	
	
	add		 r5,r12,r7
	add		 r10,r10,#0x10
	orr		 r4,r4,r2,lsl #16
	rsb		 r5,r11,r5,lsl #2        
	sub		 r5,r5,r8
	ldr		 r11,[r1]
	add		 r10,r10,r9
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r10
	add      r0,r0,#4
	usat     r2,#8,r5,asr #5
	subs     r6,r6,#4
	orr		 r4,r4,r2,lsl #24   
	uhadd8 	 r4,r4,r11
    str      r4,[r1],#4

    bgt      H02V00_col_loop
    
    @ldr	     r2,[sp,#-4]
    ldr    r2,[sp,#0]
    sub		 r1,r1,#8
	sub		 r0,r0,#8
    add		 r1,r1,r3 
    subs     lr,lr,#1
    add		 r0,r0,r2   
	pld		 [r0,r2]
	

    bgt      H02V00_row_loop
    
    add      sp,sp,#0x04
    ldmfd    sp!,{r4-r11,pc}

	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H03V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,52,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
ARMV6_Interpolate4Add_H03V00:	@PROC

    stmfd    sp!,{r4-r11,lr}
	pld		 [r0]
	pld		 [r0,r2]
	mov		 r4,#52
	@str	     r2,[sp,#-4]
	sub    sp,sp,#0x08
	str    r2,[sp,#0]
	mov      lr,#8
	@str	     r3,[sp,#-8]
	str      r3,[sp,#4]
	    	
H03V00_row_loop:
    mov	     r6,#8
    
H03V00_col_loop:

	ldrb     r7,[r0,#-2]	@lTemp0
    ldrb     r10,[r0,#1]	@lTemp3
	ldrb     r8,[r0,#-1]	@lTemp1     
	ldrb     r11,[r0,#2]	@lTemp4 
	ldrb     r9,[r0,#0]		@lTemp2       
	smlabb	 r7,r4,r10,r7			 
	add		 r5,r8,r11
	ldrb     r12,[r0,#3]	@lTemp5						
	
	rsb		 r5,r5,r9,lsl #2
	add		 r7,r7,#0x20
	add		 r5,r5,r5,lsl #2		
	add		 r5,r5,r7
	add		 r5,r5,r12	
	ldrb     r7,[r0,#4]	@lTemp0
	usat     r2,#8,r5,asr #6		

	smlabb	 r8,r4,r11,r8
	add		 r5,r9,r12
	rsb		 r5,r5,r10,lsl #2        
	add		 r8,r8,r7	
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r8
	add		 r5,r5,#0x20
	ldrb     r8,[r0,#5]		@lTemp1
	usat     r3,#8,r5,asr #6				
	
	smlabb	 r9,r4,r12,r9	
	add		 r5,r10,r7
	orr		 r2,r2,r3,lsl #8
	rsb		 r5,r5,r11,lsl #2
	add		 r9,r9,r8
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r9
	add		 r5,r5,#0x20
	ldrb     r9,[r0,#6]		@lTemp2
	usat     r3,#8,r5,asr #6				

	smlabb	 r10,r4,r7,r10	
	add		 r5,r11,r8
	orr		 r2,r2,r3,lsl #16
	rsb		 r5,r5,r12,lsl #2        
 	ldr		 r11,[r1]
	add		 r10,r10,r9
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r10
	add		 r5,r5,#0x20
	add      r0,r0,#4
	usat     r3,#8,r5,asr #6
	subs     r6,r6,#4
	orr		 r2,r2,r3,lsl #24        
	uhadd8 	 r2,r2,r11
    str      r2,[r1],#4

    bgt      H03V00_col_loop
    
	@ldr	     r2,[sp,#-4]
	@ldr	     r3,[sp,#-8]
	ldr    r2,[sp,#0]
	ldr    r3,[sp,#4]	
	sub		 r1,r1,#8
	sub		 r0,r0,#8
	subs     lr,lr,#1
    add		 r0,r0,r2   
    add		 r1,r1,r3     
	pld		 [r0,r2]	

    bgt      H03V00_row_loop
    
    add    sp,sp,#0x08    
    ldmfd    sp!,{r4-r11,pc}

    @ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	v filter (1,-5,52,20,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
ARMV6_Interpolate4Add_H00V01:	@PROC
	
    stmfd    sp!,{r4-r11,lr}
	pld		 [r0]
	mov      r5,#8 
	mov		 r4,#52	
	@str	     r5,[sp,#-4]
	sub    sp,sp,#0x08
	str    r5,[sp,#0]        
	add		 lr,r2,r2,lsl #1	@3*uSrcPitch
	
H00V01_col_loop:
	mov	     r6,#8
	@str	     r6,[sp,#-8]
	str      r6,[sp,#4]        
	
H00V01_row_loop:

	@ t0 = p0
	@ v0 = ( 52*t2 + 5*(t3<<2 - (t1+t4)) + (t0+t5) + 32 ) >> 6
	
    ldrb     r9,[r0]			@lTemp2
	ldrb     r7,[r0,-r2,lsl #1]	@lTemp0
    ldrb     r8,[r0,-r2]		@lTemp1
    ldrb     r11,[r0,r2,lsl #1]	@lTemp4
    ldrb     r10,[r0,r2]		@lTemp3
    ldrb	 r6,[r1]
	smlabb	 r7,r4,r9,r7         
	add		 r5,r8,r11
	ldrb     r12,[r0,lr]		@lTemp5						
	rsb		 r5,r5,r10,lsl #2
	add		 r7,r7,#0x20
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r7
	add		 r6,r6,#1
	add		 r5,r5,r12
	ldrb     r7,[r0,r2,lsl #2]	@lTemp0
    usat     r5,#8,r5,asr #6		
	pld		 [r0,r2]
	uhadd16	 r5,r5,r6

	smlabb	 r8,r4,r10,r8
    strb     r5,[r1],r3        
	add		 r5,r9,r12
    ldrb	 r6,[r1]
	rsb		 r5,r5,r11,lsl #2
	add		 r8,r8,r7	
	add		 r5,r5,r5,lsl #2        
	add		 r5,r5,r8
	add		 r8,lr,r2,lsl #1	@5*uSrcPitch
	add		 r5,r5,#0x20
	ldrb     r8,[r0,r8]			@lTemp1
    usat     r5,#8,r5,asr #6				
	add		 r6,r6,#1
	uhadd16	 r5,r5,r6
	
	smlabb	 r9,r4,r11,r9
    strb     r5,[r1],r3        
	add		 r5,r10,r7
    ldrb	 r6,[r1]
	rsb		 r5,r5,r12,lsl #2                
	add		 r9,r9,r8	
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r9
	add		 r5,r5,#0x20
	ldrb     r9,[r0,lr,lsl #1]	@lTemp2
    usat     r5,#8,r5,asr #6				
	add		 r6,r6,#1
	uhadd16	 r5,r5,r6

	smlabb	 r10,r4,r12,r10
    strb     r5,[r1],r3        
	add		 r5,r11,r8
    ldrb	 r12,[r1]
	@ldr	     r6,[sp,#-8]
	ldr    r6,[sp,#4]        
	rsb		 r5,r5,r7,lsl #2
	add		 r10,r10,r9
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r10
	add		 r5,r5,#0x20
	add      r0,r0,r2,lsl #2
  usat     r5,#8,r5,asr #6
	add		 r12,r12,#1
	uhadd16	 r5,r5,r12
	subs     r6,r6,#4
  strb     r5,[r1],r3        
	@str	     r6,[sp,#-8]
	str      r6,[sp,#4]        

    bgt      H00V01_row_loop
    
	@ldr	     r5,[sp,#-4]
	ldr    r5,[sp,#0]
	sub		 r0,r0,r2,lsl #3
	sub		 r1,r1,r3,lsl #3		
  add		 r0,r0,#1         
	subs   r5,r5,#1
	add		 r1,r1,#1
	@str	     r5,[sp,#-4]
	str    r5,[sp,#0]

  bgt      H00V01_col_loop
  
  add    sp,sp,#0x08  
  ldmfd    sp!,{r4-r11,pc}

	@ENDP


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h/v filter (1,-5,52,20,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
ARMV6_Interpolate4Add_H01V01:	@PROC
	    
.equ TMP_BUFF_STRIDE, 8	@shared by HxVy functions(when x and y not equals 0).
	    
    stmfd    sp!,{r4-r11,lr}
    sub		 r0,r0,r2,lsl #1
	pld		 [r0]
	pld		 [r0,r2]
	mov		 r4,#52	    	
  @sub      sp,sp,#TMP_BUFF_STRIDE*13+12
  sub      sp,sp,#TMP_BUFF_STRIDE*13+16
	mov      lr,#8+5
	str		 r1,[sp,#0]	
	str		 r2,[sp,#4]	
	str	     r3,[sp,#8]   
	@add		 r1,sp,#12		@&buff[TMP_BUFF_STRIDE*13]
	add		 r1,sp,#16		@&buff[TMP_BUFF_STRIDE*13]

@@@@@@@@ H interpolate @@@@@@@@@
		
H01V01_H_row_loop:
	mov	     r6,#8
	
H01V01_H_col_loop:

	@ t0 = ( 52*p2 + 5*(p3<<2 - (p1+p4)) + (p0+p5) + 32 ) >> 6
	
    ldrb     r9,[r0,#0]		@lTemp2
	ldrb     r7,[r0,#-2]	@lTemp0
    ldrb     r8,[r0,#-1]	@lTemp1
    ldrb     r11,[r0,#2]	@lTemp4
    ldrb     r10,[r0,#1]	@lTemp3
	smlabb	 r7,r4,r9,r7     
	add		 r5,r8,r11
	ldrb     r12,[r0,#3]	@lTemp5						
	rsb		 r5,r5,r10,lsl #2
	add		 r7,r7,#0x20
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r7
	add		 r5,r5,r12
	ldrb     r7,[r0,#4]		@lTemp0
    usat     r3,#8,r5,asr #6		

	smlabb	 r8,r4,r10,r8
	add		 r5,r9,r12
	rsb		 r5,r5,r11,lsl #2
	add		 r8,r8,r7	
	add		 r5,r5,r5,lsl #2        
	add		 r5,r5,r8
	add		 r5,r5,#0x20
	ldrb     r8,[r0,#5]		@lTemp1
    usat     r2,#8,r5,asr #6				
	
	smlabb	 r9,r4,r11,r9
	add		 r5,r10,r7
	orr		 r3,r3,r2,lsl #8
	rsb		 r5,r5,r12,lsl #2                
	add		 r9,r9,r8	
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r9
	add		 r5,r5,#0x20
	ldrb     r9,[r0,#6]		@lTemp2
    usat     r2,#8,r5,asr #6				

	smlabb	 r10,r4,r12,r10
	add		 r5,r11,r8
	orr		 r3,r3,r2,lsl #16
	rsb		 r5,r5,r7,lsl #2
	add		 r10,r10,r9
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r10
	add		 r5,r5,#0x20
	add      r0,r0,#4
    usat     r2,#8,r5,asr #6
	subs     r6,r6,#4
	orr		 r3,r3,r2,lsl #24
    str      r3,[r1],#4

    bgt      H01V01_H_col_loop
    
	ldr	     r2,[sp,#4]
	sub		 r0,r0,#8
	subs     lr,lr,#1
    add		 r0,r0,r2         
	pld		 [r0,r2]
    bgt      H01V01_H_row_loop

@@@@@@@@ V interpolate @@@@@@@@@
    
	@add		 r0,sp,#12		@&buff[TMP_BUFF_STRIDE*13]
	add		 r0,sp,#16		@&buff[TMP_BUFF_STRIDE*13]
	ldr		 r1,[sp]	
	ldr	   r3,[sp,#8]
	pld		 [r0]
	mov    lr,#8 
	mov		 r4,#52	
	@str	   lr,[sp,#-4]
	str    lr,[sp,#12]
	
H01V01_V_col_loop:
	mov	     r6,#8
	
H01V01_V_row_loop:

	@ v0 = ( 52*t2 + 5*(t3<<2 - (t1+t4)) + (t0+t5) + 32 ) >> 6
	
  ldrb     r9,[r0,#16]		@lTemp2
	ldrb     r7,[r0]			@lTemp0
  ldrb     r8,[r0,#8]			@lTemp1
  ldrb     r11,[r0,#32]		@lTemp4
  ldrb     r10,[r0,#24]		@lTemp3
  ldrb	 lr,[r1]
	smlabb	 r7,r4,r9,r7         
	add		 r5,r8,r11
	ldrb     r12,[r0,#40]		@lTemp5						
	pld		 [r0,#TMP_BUFF_STRIDE]
	rsb		 r5,r5,r10,lsl #2
	add		 r7,r7,#0x20
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r7
	add		 r5,r5,r12
	ldrb     r7,[r0,#48]		@lTemp0
    usat     r5,#8,r5,asr #6		
	add		 lr,lr,#1
	uhadd16	 r5,r5,lr

	smlabb	 r8,r4,r10,r8
    strb     r5,[r1],r3        
	add		 r5,r9,r12
    ldrb	 lr,[r1]
	rsb		 r5,r5,r11,lsl #2
	add		 r8,r8,r7	
	add		 r5,r5,r5,lsl #2        
	add		 r5,r5,r8
	add		 r5,r5,#0x20
	ldrb     r8,[r0,#56]		@lTemp1
    usat     r5,#8,r5,asr #6				
	add		 lr,lr,#1
	uhadd16	 r5,r5,lr
	
	smlabb	 r9,r4,r11,r9
    strb     r5,[r1],r3        
	add		 r5,r10,r7
    ldrb	 lr,[r1]
	rsb		 r5,r5,r12,lsl #2                
	add		 r9,r9,r8	
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r9
	add		 r5,r5,#0x20
	ldrb     r9,[r0,#64]		@lTemp2
    usat     r5,#8,r5,asr #6				
	add		 lr,lr,#1
	uhadd16	 r5,r5,lr

	smlabb	 r10,r4,r12,r10
    strb     r5,[r1],r3        
	add		 r5,r11,r8
    ldrb	 lr,[r1]
	rsb		 r5,r5,r7,lsl #2
	add		 r10,r10,r9
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r10
	add		 r5,r5,#0x20
	add      r0,r0,#32
    usat     r5,#8,r5,asr #6
	add		 lr,lr,#1
	uhadd16	 r5,r5,lr
	subs     r6,r6,#4
  strb     r5,[r1],r3        

  bgt      H01V01_V_row_loop
    
	@ldr	     lr,[sp,#-4]
	ldr    lr,[sp,#12]
	sub		 r0,r0,#63			@-8<<3 +1
	sub		 r1,r1,r3,lsl #3		
	subs   lr,lr,#1
	add		 r1,r1,#1
	@str	   lr,[sp,#-4]
	str    lr,[sp,#12]

  bgt      H01V01_V_col_loop    
    
  @add      sp,sp,#TMP_BUFF_STRIDE*13+12
  add      sp,sp,#TMP_BUFF_STRIDE*13+16
  ldmfd    sp!,{r4-r11,pc}

	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,20,-5,1) 
@	v filter (1,-5,52,20,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
ARMV6_Interpolate4Add_H02V01:	@PROC
	    	    
  stmfd    sp!,{r4-r11,lr}
  sub		 r0,r0,r2,lsl #1
	pld		 [r0]
	pld		 [r0,r2]
  @sub      sp,sp,#TMP_BUFF_STRIDE*13+12
  sub      sp,sp,#TMP_BUFF_STRIDE*13+16
	mov      lr,#8+5
	str		 r1,[sp,#0]	
	str		 r2,[sp,#4]	
	str	     r3,[sp,#8]   
	@add		 r1,sp,#12		@&buff[TMP_BUFF_STRIDE*13]
	add		 r1,sp,#16		@&buff[TMP_BUFF_STRIDE*13]

@@@@@@@@ H interpolate @@@@@@@@@
		
H02V01_H_row_loop:
	mov	     r6,#8
	
H02V01_H_col_loop:
	
    ldrb     r9,[r0,#0]		@lTemp2
	ldrb     r10,[r0,#1]	@lTemp3
	ldrb     r11,[r0,#2]	@lTemp4
    ldrb     r8,[r0,#-1]	@lTemp1        
	add		 r5,r9,r10	
    ldrb     r7,[r0,#-2]	@lTemp0
	rsb		 r5,r8,r5,lsl #2
	ldrb     r12,[r0,#3]	@lTemp5	
	
	sub		 r5,r5,r11
	add		 r7,r7,#0x10
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r7
	add		 r5,r5,r12
	ldrb     r7,[r0,#4]		@lTemp0
    usat     r4,#8,r5,asr #5		

	add		 r5,r10,r11
	add		 r8,r8,#0x10
	rsb		 r5,r9,r5,lsl #2        
	sub		 r5,r5,r12
	add		 r8,r8,r7
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r8
	ldrb     r8,[r0,#5]		@lTemp1
	usat     r2,#8,r5,asr #5
			
	add		 r5,r11,r12
	add		 r9,r9,#0x10
	orr		 r4,r4,r2,lsl #8
	rsb		 r5,r10,r5,lsl #2        
	sub		 r5,r5,r7
	add		 r9,r9,r8 
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r9
	ldrb     r9,[r0,#6]		@lTemp2
	usat     r2,#8,r5,asr #5	
	
	add		 r5,r12,r7
	add		 r10,r10,#0x10
	orr		 r4,r4,r2,lsl #16
	rsb		 r5,r11,r5,lsl #2        
	sub		 r5,r5,r8
	add		 r10,r10,r9
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r10
	add      r0,r0,#4
	usat     r2,#8,r5,asr #5
	subs     r6,r6,#4
	orr		 r4,r4,r2,lsl #24   
    str      r4,[r1],#4

    bgt      H02V01_H_col_loop
    
    ldr	     r2,[sp,#4]
	sub		 r0,r0,#8
    subs     lr,lr,#1
    add		 r0,r0,r2   
	pld		 [r0,r2]
    bgt      H02V01_H_row_loop

@@@@@@@@ V interpolate @@@@@@@@@
    
	@add		 r0,sp,#12		@&buff[TMP_BUFF_STRIDE*13]
	add		 r0,sp,#16		@&buff[TMP_BUFF_STRIDE*13]
	ldr		 r1,[sp]	
	ldr	   r3,[sp,#8]
	pld		 [r0]
	mov    lr,#8 
	mov		 r4,#52	
	@str	   lr,[sp,#-4]
	str	   lr,[sp,#12]
	
H02V01_V_col_loop:
	mov	     r6,#8
	
H02V01_V_row_loop:

	@ v0 = ( 52*t2 + 5*(t3<<2 - (t1+t4)) + (t0+t5) + 32 ) >> 6
	
    ldrb     r9,[r0,#16]		@lTemp2
	ldrb     r7,[r0]			@lTemp0
    ldrb     r8,[r0,#8]			@lTemp1
    ldrb     r11,[r0,#32]		@lTemp4
    ldrb     r10,[r0,#24]		@lTemp3
    ldrb	 lr,[r1]
	smlabb	 r7,r4,r9,r7         
	add		 r5,r8,r11
	ldrb     r12,[r0,#40]		@lTemp5						
	pld		 [r0,#TMP_BUFF_STRIDE]
	rsb		 r5,r5,r10,lsl #2
	add		 r7,r7,#0x20
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r7
	add		 r5,r5,r12
	ldrb     r7,[r0,#48]		@lTemp0
    usat     r5,#8,r5,asr #6		
	add		 lr,lr,#1
	uhadd16	 r5,r5,lr

	smlabb	 r8,r4,r10,r8
    strb     r5,[r1],r3        
	add		 r5,r9,r12
    ldrb	 lr,[r1]
	rsb		 r5,r5,r11,lsl #2
	add		 r8,r8,r7	
	add		 r5,r5,r5,lsl #2        
	add		 r5,r5,r8
	add		 r5,r5,#0x20
	ldrb     r8,[r0,#56]		@lTemp1
    usat     r5,#8,r5,asr #6				
	add		 lr,lr,#1
	uhadd16	 r5,r5,lr
	
	smlabb	 r9,r4,r11,r9
    strb     r5,[r1],r3        
	add		 r5,r10,r7
    ldrb	 lr,[r1]
	rsb		 r5,r5,r12,lsl #2                
	add		 r9,r9,r8	
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r9
	add		 r5,r5,#0x20
	ldrb     r9,[r0,#64]		@lTemp2
    usat     r5,#8,r5,asr #6				
	add		 lr,lr,#1
	uhadd16	 r5,r5,lr

	smlabb	 r10,r4,r12,r10
  strb     r5,[r1],r3        
	add		 r5,r11,r8
  ldrb	 lr,[r1]
	rsb		 r5,r5,r7,lsl #2
	add		 r10,r10,r9
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r10
	add		 r5,r5,#0x20
	add      r0,r0,#32
  usat     r5,#8,r5,asr #6
	add		 lr,lr,#1
	uhadd16	 r5,r5,lr
	subs     r6,r6,#4
  strb     r5,[r1],r3        

  bgt      H02V01_V_row_loop
    
	@ldr	     lr,[sp,#-4]
	ldr	   lr,[sp,#12]
	sub		 r0,r0,#63			@-8<<3 +1
	sub		 r1,r1,r3,lsl #3		
	subs   lr,lr,#1
	add		 r1,r1,#1
	@str	   lr,[sp,#-4]
	str	   lr,[sp,#12]

  bgt      H02V01_V_col_loop    
    
 @add      sp,sp,#TMP_BUFF_STRIDE*13+12
  add      sp,sp,#TMP_BUFF_STRIDE*13+16
  ldmfd    sp!,{r4-r11,pc}

	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H03V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,52,-5,1) 
@	v filter (1,-5,52,20,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
ARMV6_Interpolate4Add_H03V01:	@PROC
	    	    
  stmfd    sp!,{r4-r11,lr}
  sub		 r0,r0,r2,lsl #1
	pld		 [r0]
	pld		 [r0,r2]
	mov		 r4,#52	    	
  @sub      sp,sp,#TMP_BUFF_STRIDE*13+12
  sub    sp,sp,#TMP_BUFF_STRIDE*13+16
	mov    lr,#8+5
	str		 r1,[sp,#0]	
	str		 r2,[sp,#4]	
	str	   r3,[sp,#8]   
	@add		 r1,sp,#12		@&buff[TMP_BUFF_STRIDE*13]
	add		 r1,sp,#16		@&buff[TMP_BUFF_STRIDE*13]

@@@@@@@@ H interpolate @@@@@@@@@
		
H03V01_H_row_loop:
	mov	     r6,#8
	
H03V01_H_col_loop:
	
	ldrb     r7,[r0,#-2]	@lTemp0
    ldrb     r10,[r0,#1]	@lTemp3
	ldrb     r8,[r0,#-1]	@lTemp1     
	ldrb     r11,[r0,#2]	@lTemp4 
	ldrb     r9,[r0,#0]		@lTemp2       
	smlabb	 r7,r4,r10,r7			 
	add		 r5,r8,r11
	ldrb     r12,[r0,#3]	@lTemp5						
	
	rsb		 r5,r5,r9,lsl #2
	add		 r7,r7,#0x20
	add		 r5,r5,r5,lsl #2		
	add		 r5,r5,r7
	add		 r5,r5,r12	
	ldrb     r7,[r0,#4]	@lTemp0
	usat     r2,#8,r5,asr #6		

	smlabb	 r8,r4,r11,r8
	add		 r5,r9,r12
	rsb		 r5,r5,r10,lsl #2        
	add		 r8,r8,r7	
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r8
	add		 r5,r5,#0x20
	ldrb     r8,[r0,#5]		@lTemp1
	usat     r3,#8,r5,asr #6				
	
	smlabb	 r9,r4,r12,r9	
	add		 r5,r10,r7
	orr		 r2,r2,r3,lsl #8
	rsb		 r5,r5,r11,lsl #2
	add		 r9,r9,r8
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r9
	add		 r5,r5,#0x20
	ldrb     r9,[r0,#6]		@lTemp2
	usat     r3,#8,r5,asr #6				

	smlabb	 r10,r4,r7,r10	
	add		 r5,r11,r8
	orr		 r2,r2,r3,lsl #16
	rsb		 r5,r5,r12,lsl #2        
	add		 r10,r10,r9
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r10
	add		 r5,r5,#0x20
	add      r0,r0,#4
	usat     r3,#8,r5,asr #6
	subs     r6,r6,#4
	orr		 r2,r2,r3,lsl #24        
    str      r2,[r1],#4

    bgt      H03V01_H_col_loop
    
	ldr	     r2,[sp,#4]
	sub		 r0,r0,#8
	subs     lr,lr,#1
    add		 r0,r0,r2   
	pld		 [r0,r2]	

    bgt      H03V01_H_row_loop

@@@@@@@@ V interpolate @@@@@@@@@
    
	@add		 r0,sp,#12		@&buff[TMP_BUFF_STRIDE*13]
	add		 r0,sp,#16		@&buff[TMP_BUFF_STRIDE*13]
	ldr		 r1,[sp]	
	ldr	   r3,[sp,#8]
	pld		 [r0]
	mov    lr,#8 
	mov		 r4,#52	
	@str	   lr,[sp,#-4]
	str	   lr,[sp,#12]
	
H03V01_V_col_loop:
	mov	     r6,#8
	
H03V01_V_row_loop:

	@ v0 = ( 52*t2 + 5*(t3<<2 - (t1+t4)) + (t0+t5) + 32 ) >> 6
	
    ldrb     r9,[r0,#16]		@lTemp2
	ldrb     r7,[r0]			@lTemp0
    ldrb     r8,[r0,#8]			@lTemp1
    ldrb     r11,[r0,#32]		@lTemp4
    ldrb     r10,[r0,#24]		@lTemp3
    ldrb	 lr,[r1]
	smlabb	 r7,r4,r9,r7         
	add		 r5,r8,r11
	ldrb     r12,[r0,#40]		@lTemp5						
	pld		 [r0,#TMP_BUFF_STRIDE]
	rsb		 r5,r5,r10,lsl #2
	add		 r7,r7,#0x20
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r7
	add		 r5,r5,r12
	ldrb     r7,[r0,#48]		@lTemp0
    usat     r5,#8,r5,asr #6		
	add		 lr,lr,#1
	uhadd16	 r5,r5,lr

	smlabb	 r8,r4,r10,r8
    strb     r5,[r1],r3        
	add		 r5,r9,r12
    ldrb	 lr,[r1]
	rsb		 r5,r5,r11,lsl #2
	add		 r8,r8,r7	
	add		 r5,r5,r5,lsl #2        
	add		 r5,r5,r8
	add		 r5,r5,#0x20
	ldrb     r8,[r0,#56]		@lTemp1
    usat     r5,#8,r5,asr #6				
	add		 lr,lr,#1
	uhadd16	 r5,r5,lr
	
	smlabb	 r9,r4,r11,r9
    strb     r5,[r1],r3        
	add		 r5,r10,r7
    ldrb	 lr,[r1]
	rsb		 r5,r5,r12,lsl #2                
	add		 r9,r9,r8	
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r9
	add		 r5,r5,#0x20
	ldrb     r9,[r0,#64]		@lTemp2
    usat     r5,#8,r5,asr #6				
	add		 lr,lr,#1
	uhadd16	 r5,r5,lr

	smlabb	 r10,r4,r12,r10
  strb     r5,[r1],r3        
	add		 r5,r11,r8
  ldrb	 lr,[r1]
	rsb		 r5,r5,r7,lsl #2
	add		 r10,r10,r9
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r10
	add		 r5,r5,#0x20
	add      r0,r0,#32
  usat     r5,#8,r5,asr #6
	add		 lr,lr,#1
	uhadd16	 r5,r5,lr
	subs     r6,r6,#4
  strb     r5,[r1],r3        

  bgt      H03V01_V_row_loop
    
	@ldr	     lr,[sp,#-4]
	ldr	   lr,[sp,#12]
	sub		 r0,r0,#63			@-8<<3 +1
	sub		 r1,r1,r3,lsl #3		
	subs   lr,lr,#1
	add		 r1,r1,#1
	@str	   lr,[sp,#-4]
	str	   lr,[sp,#12]

  bgt      H03V01_V_col_loop    
    
  @add      sp,sp,#TMP_BUFF_STRIDE*13+12
  add      sp,sp,#TMP_BUFF_STRIDE*13+16
  ldmfd    sp!,{r4-r11,pc}

	@ENDP


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	v filter (1,-5,20,20,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
ARMV6_Interpolate4Add_H00V02:	@PROC
	
    stmfd    sp!,{r4-r11,lr}
	pld		 [r0]
	add		 r4,r2,r2,lsl #1	@3*uSrcPitch
    mov      r5,#8
    @str		 r5,[sp,#-4]
    sub    sp,sp,#0x08
    str    r5,[sp,#0]
        
H00V02_col_loop:
    mov      r6,#8
    @str		 r6,[sp,#-8]
    str      r6,[sp,#4]
    
H00V02_row_loop:

    ldrb     r9,[r0,#0]			@lTemp2
	ldrb     r10,[r0,r2]		@lTemp3
	ldrb     r11,[r0,r2,lsl #1]	@lTemp4
    ldrb     r8,[r0,-r2]		@lTemp1        
    ldrb	 r6,[r1]
	add		 r5,r9,r10	
    ldrb     r7,[r0,-r2,lsl #1]	@lTemp0
	rsb		 r5,r8,r5,lsl #2
	ldrb     r12,[r0,r4]		@lTemp5	
	pld		 [r0,r2]
	
	sub		 r5,r5,r11
	add		 r7,r7,#0x10
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r7
	add		 r5,r5,r12
	ldrb     r7,[r0,r2,lsl #2]	@lTemp0
    usat     lr,#8,r5,asr #5		
	add		 r6,r6,#1
	uhadd16	 lr,lr,r6

	add		 r5,r10,r11
    strb     lr,[r1],r3
	add		 r8,r8,#0x10
    ldrb	 r6,[r1]
	rsb		 r5,r9,r5,lsl #2        
	sub		 r5,r5,r12
	add		 r8,r8,r7
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r8
	add		 r8,r2,r2,lsl #2
	usat     r5,#8,r5,asr #5
	add		 r6,r6,#1
	uhadd16	 r5,r5,r6
	ldrb     r8,[r0,r8]			@lTemp1
    strb     r5,[r1],r3

	add		 r5,r11,r12
    ldrb	 r6,[r1]
	add		 r9,r9,#0x10
	rsb		 r5,r10,r5,lsl #2        
	sub		 r5,r5,r7
	add		 r9,r9,r8 
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r9
	ldrb     r9,[r0,r4,lsl #1]	@lTemp2
	usat     lr,#8,r5,asr #5
	add		 r6,r6,#1
	uhadd16	 r5,r5,r6

	add		 r5,r12,r7
  strb     lr,[r1],r3
	add		 r10,r10,#0x10
  ldrb	 r12,[r1]
	rsb		 r5,r11,r5,lsl #2        
  @ldr		 r6,[sp,#-8]
  ldr    r6,[sp,#4]
	sub		 r5,r5,r8
	add		 r10,r10,r9
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r10
	add      r0,r0,r2,lsl #2
	usat     r5,#8,r5,asr #5   
	add		 r12,r12,#1
	uhadd16	 r5,r5,r12
  subs     r6,r6,#4
  strb     r5,[r1],r3
  @str		 r6,[sp,#-8]
  str      r6,[sp,#4]

  bgt      H00V02_row_loop
    
  @ldr		 r5,[sp,#-4]
  ldr    r5,[sp,#0]
	sub		 r0,r0,r2,lsl #3
	sub		 r1,r1,r3,lsl #3		
  add		 r0,r0,#1
  subs     r5,r5,#1
  add		 r1,r1,#1 
  @str		 r5,[sp,#-4]
  str    r5,[sp,#0]
    
  bgt      H00V02_col_loop
  
  add      sp,sp,#0x08  
  ldmfd    sp!,{r4-r11,pc}
    
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,52,20,-5,1) 
@	v filter (1,-5,20,20,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
ARMV6_Interpolate4Add_H01V02:	@PROC
	
    stmfd    sp!,{r4-r11,lr}

    sub		 r0,r0,r2,lsl #1
	pld		 [r0]
	pld		 [r0,r2]
	mov		 r4,#52	    	
  @sub      sp,sp,#TMP_BUFF_STRIDE*13+12
  sub    sp,sp,#TMP_BUFF_STRIDE*13+16
	mov    lr,#8+5
	str		 r1,[sp,#0]	
	str		 r2,[sp,#4]	
	str	   r3,[sp,#8]   
	@add		 r1,sp,#12		@&buff[TMP_BUFF_STRIDE*13]
	add		 r1,sp,#16		@&buff[TMP_BUFF_STRIDE*13]
	
@@@@@@@@ H interpolate @@@@@@@@@
		
H01V02_H_row_loop:
	mov	     r6,#8
	
H01V02_H_col_loop:

	@ t0 = ( 52*p2 + 5*(p3<<2 - (p1+p4)) + (p0+p5) + 32 ) >> 6
	
    ldrb     r9,[r0,#0]		@lTemp2
	ldrb     r7,[r0,#-2]	@lTemp0
    ldrb     r8,[r0,#-1]	@lTemp1
    ldrb     r11,[r0,#2]	@lTemp4
    ldrb     r10,[r0,#1]	@lTemp3
	smlabb	 r7,r4,r9,r7     
	add		 r5,r8,r11
	ldrb     r12,[r0,#3]	@lTemp5						
	rsb		 r5,r5,r10,lsl #2
	add		 r7,r7,#0x20
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r7
	add		 r5,r5,r12
	ldrb     r7,[r0,#4]		@lTemp0
    usat     r3,#8,r5,asr #6		

	smlabb	 r8,r4,r10,r8
	add		 r5,r9,r12
	rsb		 r5,r5,r11,lsl #2
	add		 r8,r8,r7	
	add		 r5,r5,r5,lsl #2        
	add		 r5,r5,r8
	add		 r5,r5,#0x20
	ldrb     r8,[r0,#5]		@lTemp1
    usat     r2,#8,r5,asr #6				
	
	smlabb	 r9,r4,r11,r9
	add		 r5,r10,r7
	orr		 r3,r3,r2,lsl #8
	rsb		 r5,r5,r12,lsl #2                
	add		 r9,r9,r8	
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r9
	add		 r5,r5,#0x20
	ldrb     r9,[r0,#6]		@lTemp2
    usat     r2,#8,r5,asr #6				

	smlabb	 r10,r4,r12,r10
	add		 r5,r11,r8
	orr		 r3,r3,r2,lsl #16
	rsb		 r5,r5,r7,lsl #2
	add		 r10,r10,r9
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r10
	add		 r5,r5,#0x20
	add      r0,r0,#4
    usat     r2,#8,r5,asr #6
	subs     r6,r6,#4
	orr		 r3,r3,r2,lsl #24
    str      r3,[r1],#4

    bgt      H01V02_H_col_loop
    
	ldr	     r2,[sp,#4]
	sub		 r0,r0,#8
	subs     lr,lr,#1
    add		 r0,r0,r2         
	pld		 [r0,r2]

    bgt      H01V02_H_row_loop
    
@@@@@@@@ V interpolate @@@@@@@@@

	@add		 r0,sp,#12		@&buff[TMP_BUFF_STRIDE*13]
	add		 r0,sp,#16		@&buff[TMP_BUFF_STRIDE*13]
	ldr		 r1,[sp]	
	ldr	     r3,[sp,#8]
	pld		 [r0]
  mov      r5,#8
  str		 r5,[sp]
        
H01V02_V_col_loop:
  mov      r6,#8
  @str		 r6,[sp,#-4]
  str		 r6,[sp,#12]
    
H01V02_V_row_loop:

    ldrb     r9,[r0,#16]		@lTemp2
	ldrb     r10,[r0,#24]		@lTemp3
	ldrb     r11,[r0,#32]		@lTemp4
    ldrb     r8,[r0,#8]			@lTemp1        
    ldrb	 r6,[r1]
	add		 r5,r9,r10	
    ldrb     r7,[r0]			@lTemp0
	rsb		 r5,r8,r5,lsl #2
	ldrb     r12,[r0,#40]		@lTemp5	
	
	sub		 r5,r5,r11
	add		 r7,r7,#0x10
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r7
	add		 r5,r5,r12
	ldrb     r7,[r0,#48]		@lTemp0
    usat     lr,#8,r5,asr #5		
	add		 r6,r6,#1
	uhadd16	 lr,lr,r6

	add		 r5,r10,r11
    strb     lr,[r1],r3
	add		 r8,r8,#0x10
    ldrb	 r6,[r1]
	rsb		 r5,r9,r5,lsl #2        
	sub		 r5,r5,r12
	add		 r8,r8,r7
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r8
	usat     r5,#8,r5,asr #5
	add		 r6,r6,#1
	uhadd16	 r5,r5,r6
	ldrb     r8,[r0,#56]		@lTemp1
    strb     r5,[r1],r3

	add		 r5,r11,r12
    ldrb	 r6,[r1]
	add		 r9,r9,#0x10
	rsb		 r5,r10,r5,lsl #2        
	sub		 r5,r5,r7
	add		 r9,r9,r8 
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r9
	ldrb     r9,[r0,#64]		@lTemp2
	usat     lr,#8,r5,asr #5
	add		 r6,r6,#1
	uhadd16	 lr,lr,r6

	add		 r5,r12,r7
  strb     lr,[r1],r3
	add		 r10,r10,#0x10
  ldrb	 r12,[r1]
  @ldr		 r6,[sp,#-4]
  ldr		 r6,[sp,#12]
	rsb		 r5,r11,r5,lsl #2        
	sub		 r5,r5,r8
	add		 r10,r10,r9
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r10
	add      r0,r0,#32
	usat     r5,#8,r5,asr #5   
	add		 r12,r12,#1
	uhadd16	 r5,r5,r12
  subs     r6,r6,#4
  strb     r5,[r1],r3
  @str		 r6,[sp,#-4]
  str		   r6,[sp,#12]

    bgt      H01V02_V_row_loop
    
    ldr		 r5,[sp]
	sub		 r1,r1,r3,lsl #3		
	sub		 r0,r0,#63
    add		 r1,r1,#1 
    subs     r5,r5,#1
    str		 r5,[sp]
    
    bgt      H01V02_V_col_loop
    
    @add      sp,sp,#TMP_BUFF_STRIDE*13+12
    add      sp,sp,#TMP_BUFF_STRIDE*13+16
    ldmfd    sp!,{r4-r11,pc}
    
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,20,-5,1) 
@	v filter (1,-5,20,20,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
ARMV6_Interpolate4Add_H02V02:	@PROC

  stmfd    sp!,{r4-r11,lr}
  sub		 r0,r0,r2,lsl #1
	pld		 [r0]
	pld		 [r0,r2]
  @sub      sp,sp,#TMP_BUFF_STRIDE*13+12
  sub      sp,sp,#TMP_BUFF_STRIDE*13+16
	mov      lr,#8+5
	str		 r1,[sp,#0]	
	str		 r2,[sp,#4]	
	str	     r3,[sp,#8]   
	@add		 r1,sp,#12		@&buff[TMP_BUFF_STRIDE*13]
	add		 r1,sp,#16		@&buff[TMP_BUFF_STRIDE*13]

@@@@@@@@ H interpolate @@@@@@@@@
		
H02V02_H_row_loop:
	mov	     r6,#8
	
H02V02_H_col_loop:
	
    ldrb     r9,[r0,#0]		@lTemp2
	ldrb     r10,[r0,#1]	@lTemp3
	ldrb     r11,[r0,#2]	@lTemp4
    ldrb     r8,[r0,#-1]	@lTemp1        
	add		 r5,r9,r10	
    ldrb     r7,[r0,#-2]	@lTemp0
	rsb		 r5,r8,r5,lsl #2
	ldrb     r12,[r0,#3]	@lTemp5	
	
	sub		 r5,r5,r11
	add		 r7,r7,#0x10
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r7
	add		 r5,r5,r12
	ldrb     r7,[r0,#4]		@lTemp0
    usat     r4,#8,r5,asr #5		

	add		 r5,r10,r11
	add		 r8,r8,#0x10
	rsb		 r5,r9,r5,lsl #2        
	sub		 r5,r5,r12
	add		 r8,r8,r7
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r8
	ldrb     r8,[r0,#5]		@lTemp1
	usat     r2,#8,r5,asr #5
			
	add		 r5,r11,r12
	add		 r9,r9,#0x10
	orr		 r4,r4,r2,lsl #8
	rsb		 r5,r10,r5,lsl #2        
	sub		 r5,r5,r7
	add		 r9,r9,r8 
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r9
	ldrb     r9,[r0,#6]		@lTemp2
	usat     r2,#8,r5,asr #5	
	
	add		 r5,r12,r7
	add		 r10,r10,#0x10
	orr		 r4,r4,r2,lsl #16
	rsb		 r5,r11,r5,lsl #2        
	sub		 r5,r5,r8
	add		 r10,r10,r9
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r10
	add      r0,r0,#4
	usat     r2,#8,r5,asr #5
	subs     r6,r6,#4
	orr		 r4,r4,r2,lsl #24   
    str      r4,[r1],#4

    bgt      H02V02_H_col_loop
    
    ldr	     r2,[sp,#4]
	sub		 r0,r0,#8
    subs     lr,lr,#1
    add		 r0,r0,r2   
	pld		 [r0,r2]
    bgt      H02V02_H_row_loop
    
@@@@@@@@ V interpolate @@@@@@@@@

	@add		 r0,sp,#12		@&buff[TMP_BUFF_STRIDE*13]
	add		 r0,sp,#16		@&buff[TMP_BUFF_STRIDE*13]
	ldr		 r1,[sp]	
	ldr	   r3,[sp,#8]
	pld		 [r0]
  mov    r5,#8
  str		 r5,[sp]
        
H02V02_V_col_loop:
    mov      r6,#8
    @str		 r6,[sp,#-4]
    str		 r6,[sp,#12]
    
H02V02_V_row_loop:

    ldrb     r9,[r0,#16]		@lTemp2
	ldrb     r10,[r0,#24]		@lTemp3
	ldrb     r11,[r0,#32]		@lTemp4
    ldrb     r8,[r0,#8]			@lTemp1        
	ldrb	 r6,[r1]	
	add		 r5,r9,r10	
    ldrb     r7,[r0]			@lTemp0
	rsb		 r5,r8,r5,lsl #2
	ldrb     r12,[r0,#40]		@lTemp5	
	
	sub		 r5,r5,r11
	add		 r7,r7,#0x10
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r7
	add		 r5,r5,r12
	ldrb     r7,[r0,#48]		@lTemp0
    usat     lr,#8,r5,asr #5		
	add		 r6,r6,#1
	uhadd16	 lr,lr,r6

	add		 r5,r10,r11
    strb     lr,[r1],r3
	add		 r8,r8,#0x10
	ldrb	 r6,[r1]	
	rsb		 r5,r9,r5,lsl #2        
	sub		 r5,r5,r12
	add		 r8,r8,r7
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r8
	usat     r5,#8,r5,asr #5
	add		 r6,r6,#1
	uhadd16	 r5,r5,r6
	ldrb     r8,[r0,#56]		@lTemp1
    strb     r5,[r1],r3

	add		 r5,r11,r12
	ldrb	 r6,[r1]	
	add		 r9,r9,#0x10
	rsb		 r5,r10,r5,lsl #2        
	sub		 r5,r5,r7
	add		 r9,r9,r8 
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r9
	ldrb     r9,[r0,#64]		@lTemp2
	usat     lr,#8,r5,asr #5
	add		 r6,r6,#1
	uhadd16	 lr,lr,r6

	add		 r5,r12,r7
  strb     lr,[r1],r3
	add		 r10,r10,#0x10
	ldrb		 r12,[r1]	
  @ldr		 r6,[sp,#-4]
  ldr		 r6,[sp,#12]
	rsb		 r5,r11,r5,lsl #2        
	sub		 r5,r5,r8
	add		 r10,r10,r9
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r10
	add      r0,r0,#32
	usat     r5,#8,r5,asr #5   
	add		 r12,r12,#1
	uhadd16	 r5,r5,r12
    subs     r6,r6,#4
    strb     r5,[r1],r3
    @str		 r6,[sp,#-4]
    str		 r6,[sp,#12]

    bgt      H02V02_V_row_loop
    
    ldr		 r5,[sp]
	sub		 r1,r1,r3,lsl #3		
	sub		 r0,r0,#63
    add		 r1,r1,#1 
    subs     r5,r5,#1
    str		 r5,[sp]
    
    bgt      H02V02_V_col_loop
    
    @add      sp,sp,#TMP_BUFF_STRIDE*13+12
    add      sp,sp,#TMP_BUFF_STRIDE*13+16
    ldmfd    sp!,{r4-r11,pc}
    
	@ENDP


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H03V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,52,-5,1) 
@	v filter (1,-5,20,20,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
ARMV6_Interpolate4Add_H03V02:	@PROC

    stmfd    sp!,{r4-r11,lr}
    sub		 r0,r0,r2,lsl #1
	pld		 [r0]
	pld		 [r0,r2]
	mov		 r4,#52	    	
  @sub      sp,sp,#TMP_BUFF_STRIDE*13+12
  sub      sp,sp,#TMP_BUFF_STRIDE*13+16
	mov      lr,#8+5
	str		 r1,[sp,#0]	
	str		 r2,[sp,#4]	
	str	     r3,[sp,#8]   
	@add		 r1,sp,#12		@&buff[TMP_BUFF_STRIDE*13]
	add		 r1,sp,#16		@&buff[TMP_BUFF_STRIDE*13]

@@@@@@@@ H interpolate @@@@@@@@@
		
H03V02_H_row_loop:
	mov	     r6,#8
	
H03V02_H_col_loop:
	
	ldrb     r7,[r0,#-2]	@lTemp0
    ldrb     r10,[r0,#1]	@lTemp3
	ldrb     r8,[r0,#-1]	@lTemp1     
	ldrb     r11,[r0,#2]	@lTemp4 
	ldrb     r9,[r0,#0]		@lTemp2       
	smlabb	 r7,r4,r10,r7			 
	add		 r5,r8,r11
	ldrb     r12,[r0,#3]	@lTemp5						
	
	rsb		 r5,r5,r9,lsl #2
	add		 r7,r7,#0x20
	add		 r5,r5,r5,lsl #2		
	add		 r5,r5,r7
	add		 r5,r5,r12	
	ldrb     r7,[r0,#4]	@lTemp0
	usat     r2,#8,r5,asr #6		

	smlabb	 r8,r4,r11,r8
	add		 r5,r9,r12
	rsb		 r5,r5,r10,lsl #2        
	add		 r8,r8,r7	
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r8
	add		 r5,r5,#0x20
	ldrb     r8,[r0,#5]		@lTemp1
	usat     r3,#8,r5,asr #6				
	
	smlabb	 r9,r4,r12,r9	
	add		 r5,r10,r7
	orr		 r2,r2,r3,lsl #8
	rsb		 r5,r5,r11,lsl #2
	add		 r9,r9,r8
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r9
	add		 r5,r5,#0x20
	ldrb     r9,[r0,#6]		@lTemp2
	usat     r3,#8,r5,asr #6				

	smlabb	 r10,r4,r7,r10	
	add		 r5,r11,r8
	orr		 r2,r2,r3,lsl #16
	rsb		 r5,r5,r12,lsl #2        
	add		 r10,r10,r9
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r10
	add		 r5,r5,#0x20
	add      r0,r0,#4
	usat     r3,#8,r5,asr #6
	subs     r6,r6,#4
	orr		 r2,r2,r3,lsl #24        
    str      r2,[r1],#4

    bgt      H03V02_H_col_loop
    
	ldr	     r2,[sp,#4]
	sub		 r0,r0,#8
	subs     lr,lr,#1
    add		 r0,r0,r2   
	pld		 [r0,r2]	

    bgt      H03V02_H_row_loop

    
@@@@@@@@ V interpolate @@@@@@@@@

	@add		 r0,sp,#12		@&buff[TMP_BUFF_STRIDE*13]
	add		 r0,sp,#16		@&buff[TMP_BUFF_STRIDE*13]
	ldr		 r1,[sp]	
	ldr	     r3,[sp,#8]
	pld		 [r0]
    mov      r5,#8
    str		 r5,[sp]
        
H03V02_V_col_loop:
    mov      r6,#8
    @str		 r6,[sp,#-4]
    str		 r6,[sp,#12]
    
H03V02_V_row_loop:

    ldrb     r9,[r0,#16]		@lTemp2
	ldrb     r10,[r0,#24]		@lTemp3
	ldrb     r11,[r0,#32]		@lTemp4
    ldrb     r8,[r0,#8]			@lTemp1        
	ldrb	 r6,[r1]	
	add		 r5,r9,r10	
    ldrb     r7,[r0]			@lTemp0
	rsb		 r5,r8,r5,lsl #2
	ldrb     r12,[r0,#40]		@lTemp5	
	
	sub		 r5,r5,r11
	add		 r7,r7,#0x10
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r7
	add		 r5,r5,r12
	ldrb     r7,[r0,#48]		@lTemp0
    usat     lr,#8,r5,asr #5		
	add		 r6,r6,#1
	uhadd16	 lr,lr,r6

	add		 r5,r10,r11
    strb     lr,[r1],r3
	add		 r8,r8,#0x10
	ldrb	 r6,[r1]	
	rsb		 r5,r9,r5,lsl #2        
	sub		 r5,r5,r12
	add		 r8,r8,r7
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r8
	usat     r5,#8,r5,asr #5
	add		 r6,r6,#1
	uhadd16	 r5,r5,r6
	ldrb     r8,[r0,#56]		@lTemp1
    strb     r5,[r1],r3

	add		 r5,r11,r12
	ldrb	 r6,[r1]	
	add		 r9,r9,#0x10
	rsb		 r5,r10,r5,lsl #2        
	sub		 r5,r5,r7
	add		 r9,r9,r8 
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r9
	ldrb     r9,[r0,#64]		@lTemp2
	usat     lr,#8,r5,asr #5
	add		 r6,r6,#1
	uhadd16	 lr,lr,r6

	add		 r5,r12,r7
    strb     lr,[r1],r3
	add		 r10,r10,#0x10
	ldrb		 r12,[r1]	
  @ldr		 r6,[sp,#-4]
  ldr		 r6,[sp,#12]
	rsb		 r5,r11,r5,lsl #2        
	sub		 r5,r5,r8
	add		 r10,r10,r9
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r10
	add      r0,r0,#32
	usat     r5,#8,r5,asr #5   
	add		 r12,r12,#1
	uhadd16	 r5,r5,r12
    subs     r6,r6,#4
    strb     r5,[r1],r3
    @str		 r6,[sp,#-4]
    str		 r6,[sp,#12]

    bgt      H03V02_V_row_loop
    
    ldr		 r5,[sp]
	sub		 r1,r1,r3,lsl #3		
	sub		 r0,r0,#63
    add		 r1,r1,#1 
    subs     r5,r5,#1
    str		 r5,[sp]
    
    bgt      H03V02_V_col_loop
    
    @add      sp,sp,#TMP_BUFF_STRIDE*13+12
    add      sp,sp,#TMP_BUFF_STRIDE*13+16
    ldmfd    sp!,{r4-r11,pc}

	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H00V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	v filter (1,-5,20,52,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
ARMV6_Interpolate4Add_H00V03:	@PROC

    stmfd    sp!,{r4-r11,lr}
	pld		 [r0]
	pld		 [r0,r2]
  mov    r5,#8	
	mov		 r4,#52
  @str	   r5,[sp,#-4]
  sub    sp,sp,#0x08
  str    r5,[sp,#0]
	add		 lr,r2,r2,lsl #1	@3*uSrcPitch
    	
H00V03_col_loop:
  mov	     r6,#8
  @str	     r6,[sp,#-8]
  str      r6,[sp,#4]
    
H00V03_row_loop:

	ldrb     r7,[r0,-r2,lsl #1]	@lTemp0
    ldrb     r10,[r0,r2]		@lTemp3
	ldrb     r8,[r0,-r2]		@lTemp1     
	ldrb     r11,[r0,r2,lsl #1]	@lTemp4  
	ldrb     r9,[r0]			@lTemp2      
	ldrb	 r6,[r1]	
	smlabb	 r7,r4,r10,r7		 
	add		 r5,r8,r11
	ldrb     r12,[r0,lr]		@lTemp5						
	
	rsb		 r5,r5,r9,lsl #2
	add		 r7,r7,#0x20
	add		 r5,r5,r5,lsl #2		
	add		 r5,r5,r7
	add		 r5,r5,r12	
	ldrb     r7,[r0,r2,lsl #2]	@lTemp0
	usat     r5,#8,r5,asr #6		
	add		 r6,r6,#1
	uhadd16	 r5,r5,r6

	smlabb	 r8,r4,r11,r8
	strb	 r5,[r1],r3
	add		 r5,r9,r12
	ldrb	 r6,[r1]	
	rsb		 r5,r5,r10,lsl #2        
	add		 r8,r8,r7	
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r8
	add		 r8,lr,r2,lsl #1
	add		 r5,r5,#0x20
	ldrb     r8,[r0,r8]		@lTemp1
	usat     r5,#8,r5,asr #6				
	add		 r6,r6,#1
	uhadd16	 r5,r5,r6
	
	smlabb	 r9,r4,r12,r9
	strb	 r5,[r1],r3
	add		 r5,r10,r7
	ldrb	 r6,[r1]	
	rsb		 r5,r5,r11,lsl #2
	add		 r9,r9,r8
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r9
	add		 r5,r5,#0x20
	ldrb     r9,[r0,lr,lsl #1]		@lTemp2
	usat     r5,#8,r5,asr #6				
	add		 r6,r6,#1
	uhadd16	 r5,r5,r6

	smlabb	 r10,r4,r7,r10
	strb	 r5,[r1],r3
	add		 r5,r11,r8
	ldrb	 r7,[r1]	
  @ldr	     r6,[sp,#-8]
  ldr    r6,[sp,#4]
	rsb		 r5,r5,r12,lsl #2        
	add		 r10,r10,r9
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r10
	add		 r5,r5,#0x20
	add      r0,r0,r2,lsl #2
	usat     r5,#8,r5,asr #6
	add		 r7,r7,#1
	uhadd16	 r5,r5,r7
	subs     r6,r6,#4
	strb	 r5,[r1],r3
    @str	     r6,[sp,#-8]
    str      r6,[sp,#4]

    bgt      H00V03_row_loop
    
	@ldr	     r5,[sp,#-4]
	ldr    r5,[sp,#0]	
	sub		 r1,r1,r3,lsl #3
	sub		 r0,r0,r2,lsl #3
	add		 r1,r1,#1
	subs     r5,r5,#1
    add		 r0,r0,#1        
	pld		 [r0,r2]
	@str	     r5,[sp,#-4]
	str    r5,[sp,#0]	

    bgt      H00V03_col_loop
    
    add     sp,sp,#0x08    
    ldmfd    sp!,{r4-r11,pc}

    @ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H01V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,52,20,-5,1) 
@	v filter (1,-5,20,52,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
ARMV6_Interpolate4Add_H01V03:	@PROC
	
    stmfd    sp!,{r4-r11,lr}

    sub		 r0,r0,r2,lsl #1
	pld		 [r0]
	pld		 [r0,r2]
	mov		 r4,#52	    	
  @sub      sp,sp,#TMP_BUFF_STRIDE*13+12
  sub      sp,sp,#TMP_BUFF_STRIDE*13+16
	mov      lr,#8+5
	str		 r1,[sp,#0]	
	str		 r2,[sp,#4]	
	str	     r3,[sp,#8]   
	@add		 r1,sp,#12		@&buff[TMP_BUFF_STRIDE*13]
	add		 r1,sp,#16		@&buff[TMP_BUFF_STRIDE*13]
	
@@@@@@@@ H interpolate @@@@@@@@@
		
H01V03_H_row_loop:
	mov	     r6,#8
	
H01V03_H_col_loop:

	@ t0 = ( 52*p2 + 5*(p3<<2 - (p1+p4)) + (p0+p5) + 32 ) >> 6
	
    ldrb     r9,[r0,#0]		@lTemp2
	ldrb     r7,[r0,#-2]	@lTemp0
    ldrb     r8,[r0,#-1]	@lTemp1
    ldrb     r11,[r0,#2]	@lTemp4
    ldrb     r10,[r0,#1]	@lTemp3
	smlabb	 r7,r4,r9,r7     
	add		 r5,r8,r11
	ldrb     r12,[r0,#3]	@lTemp5						
	rsb		 r5,r5,r10,lsl #2
	add		 r7,r7,#0x20
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r7
	add		 r5,r5,r12
	ldrb     r7,[r0,#4]		@lTemp0
    usat     r3,#8,r5,asr #6		

	smlabb	 r8,r4,r10,r8
	add		 r5,r9,r12
	rsb		 r5,r5,r11,lsl #2
	add		 r8,r8,r7	
	add		 r5,r5,r5,lsl #2        
	add		 r5,r5,r8
	add		 r5,r5,#0x20
	ldrb     r8,[r0,#5]		@lTemp1
    usat     r2,#8,r5,asr #6				
	
	smlabb	 r9,r4,r11,r9
	add		 r5,r10,r7
	orr		 r3,r3,r2,lsl #8
	rsb		 r5,r5,r12,lsl #2                
	add		 r9,r9,r8	
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r9
	add		 r5,r5,#0x20
	ldrb     r9,[r0,#6]		@lTemp2
    usat     r2,#8,r5,asr #6				

	smlabb	 r10,r4,r12,r10
	add		 r5,r11,r8
	orr		 r3,r3,r2,lsl #16
	rsb		 r5,r5,r7,lsl #2
	add		 r10,r10,r9
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r10
	add		 r5,r5,#0x20
	add      r0,r0,#4
    usat     r2,#8,r5,asr #6
	subs     r6,r6,#4
	orr		 r3,r3,r2,lsl #24
    str      r3,[r1],#4

    bgt      H01V03_H_col_loop
    
	ldr	     r2,[sp,#4]
	sub		 r0,r0,#8
	subs     lr,lr,#1
    add		 r0,r0,r2         
	pld		 [r0,r2]

    bgt      H01V03_H_row_loop
    
@@@@@@@@ V interpolate @@@@@@@@@

	@add		 r0,sp,#12		@&buff[TMP_BUFF_STRIDE*13]
	add		 r0,sp,#16		@&buff[TMP_BUFF_STRIDE*13]
	ldr		 r1,[sp]	
	ldr	     r3,[sp,#8]
	pld		 [r0]
  mov      r5,#8
	mov		 r4,#52
  str		 r5,[sp]
    	
H01V03_col_loop:
    mov	     r6,#8
    @str		 r6,[sp,#-4]
    str		 r6,[sp,#12]
    
H01V03_row_loop:

	ldrb     r7,[r0]			@lTemp0
    ldrb     r10,[r0,#24]		@lTemp3
	ldrb     r8,[r0,#8]			@lTemp1     
	ldrb     r11,[r0,#32]		@lTemp4  
	ldrb     r9,[r0,#16]		@lTemp2      
	ldrb	 r6,[r1]	
	smlabb	 r7,r4,r10,r7		 
	add		 r5,r8,r11
	ldrb     r12,[r0,#40]		@lTemp5						
	
	rsb		 r5,r5,r9,lsl #2
	add		 r7,r7,#0x20
	add		 r5,r5,r5,lsl #2		
	add		 r5,r5,r7
	add		 r5,r5,r12	
	ldrb     r7,[r0,#48]		@lTemp0
	usat     r5,#8,r5,asr #6		
	add		 r6,r6,#1
	uhadd16	 r5,r5,r6

	smlabb	 r8,r4,r11,r8
	strb	 r5,[r1],r3
	ldrb	 r6,[r1]	
	add		 r5,r9,r12
	rsb		 r5,r5,r10,lsl #2        
	add		 r8,r8,r7	
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r8
	add		 r5,r5,#0x20
	ldrb     r8,[r0,#56]		@lTemp1
	usat     r5,#8,r5,asr #6				
	add		 r6,r6,#1
	uhadd16	 r5,r5,r6
	
	smlabb	 r9,r4,r12,r9
	strb	 r5,[r1],r3
	add		 r5,r10,r7
	ldrb	 r6,[r1]	
	rsb		 r5,r5,r11,lsl #2
	add		 r9,r9,r8
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r9
	add		 r5,r5,#0x20
	ldrb     r9,[r0,#64]		@lTemp2
	usat     r5,#8,r5,asr #6				
	add		 r6,r6,#1
	uhadd16	 r5,r5,r6

	smlabb	 r10,r4,r7,r10
	strb	 r5,[r1],r3
	add		 r5,r11,r8
	ldrb	 r7,[r1]	
	rsb		 r5,r5,r12,lsl #2        
  @ldr		 r6,[sp,#-4]
  ldr		 r6,[sp,#12]
	add		 r10,r10,r9
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r10
	add		 r5,r5,#0x20
	add      r0,r0,#32
	usat     r5,#8,r5,asr #6
	add		 r7,r7,#1
	uhadd16	 r5,r5,r7
	subs     r6,r6,#4
	strb	 r5,[r1],r3
  @str		 r6,[sp,#-4]
  str		 r6,[sp,#12]

  bgt      H01V03_row_loop
    
	ldr	    r5,[sp]	
	sub		 r1,r1,r3,lsl #3
	sub		 r0,r0,#63
	add		 r1,r1,#1
	subs     r5,r5,#1
	str	     r5,[sp]	

    bgt      H01V03_col_loop
    
    @add      sp,sp,#TMP_BUFF_STRIDE*13+12
    add      sp,sp,#TMP_BUFF_STRIDE*13+16
    ldmfd    sp!,{r4-r11,pc}

	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H02V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,20,-5,1) 
@	v filter (1,-5,20,52,-5,1) 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
ARMV6_Interpolate4Add_H02V03:	@PROC
	
  stmfd    sp!,{r4-r11,lr}
  sub		 r0,r0,r2,lsl #1
	pld		 [r0]
	pld		 [r0,r2]
  @sub      sp,sp,#TMP_BUFF_STRIDE*13+12
  sub      sp,sp,#TMP_BUFF_STRIDE*13+16
	mov      lr,#8+5
	str		 r1,[sp,#0]	
	str		 r2,[sp,#4]	
	str	     r3,[sp,#8]   
	@add		 r1,sp,#12		@&buff[TMP_BUFF_STRIDE*13]
	add		 r1,sp,#16		@&buff[TMP_BUFF_STRIDE*13

@@@@@@@@ H interpolate @@@@@@@@@
		
H02V03_H_row_loop:
	mov	     r6,#8
	
H02V03_H_col_loop:
	
    ldrb     r9,[r0,#0]		@lTemp2
	ldrb     r10,[r0,#1]	@lTemp3
	ldrb     r11,[r0,#2]	@lTemp4
    ldrb     r8,[r0,#-1]	@lTemp1        
	add		 r5,r9,r10	
    ldrb     r7,[r0,#-2]	@lTemp0
	rsb		 r5,r8,r5,lsl #2
	ldrb     r12,[r0,#3]	@lTemp5	
	
	sub		 r5,r5,r11
	add		 r7,r7,#0x10
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r7
	add		 r5,r5,r12
	ldrb     r7,[r0,#4]		@lTemp0
    usat     r4,#8,r5,asr #5		

	add		 r5,r10,r11
	add		 r8,r8,#0x10
	rsb		 r5,r9,r5,lsl #2        
	sub		 r5,r5,r12
	add		 r8,r8,r7
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r8
	ldrb     r8,[r0,#5]		@lTemp1
	usat     r2,#8,r5,asr #5
			
	add		 r5,r11,r12
	add		 r9,r9,#0x10
	orr		 r4,r4,r2,lsl #8
	rsb		 r5,r10,r5,lsl #2        
	sub		 r5,r5,r7
	add		 r9,r9,r8 
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r9
	ldrb     r9,[r0,#6]		@lTemp2
	usat     r2,#8,r5,asr #5	
	
	add		 r5,r12,r7
	add		 r10,r10,#0x10
	orr		 r4,r4,r2,lsl #16
	rsb		 r5,r11,r5,lsl #2        
	sub		 r5,r5,r8
	add		 r10,r10,r9
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r10
	add      r0,r0,#4
	usat     r2,#8,r5,asr #5
	subs     r6,r6,#4
	orr		 r4,r4,r2,lsl #24   
    str      r4,[r1],#4

    bgt      H02V03_H_col_loop
    
    ldr	     r2,[sp,#4]
	sub		 r0,r0,#8
    subs     lr,lr,#1
    add		 r0,r0,r2   
	pld		 [r0,r2]
    bgt      H02V03_H_row_loop

    
@@@@@@@@ V interpolate @@@@@@@@@

	@add		 r0,sp,#12		@&buff[TMP_BUFF_STRIDE*13]
	add		 r0,sp,#16		@&buff[TMP_BUFF_STRIDE*13]
	ldr		 r1,[sp]	
	ldr	     r3,[sp,#8]
	pld		 [r0]
  mov      r5,#8
	mov		 r4,#52
  str		 r5,[sp]
    	
H02V03_col_loop:
    mov	     r6,#8
    @str		 r6,[sp,#-4]
    str		 r6,[sp,#12]
    
H02V03_row_loop:

	ldrb     r7,[r0]			@lTemp0
    ldrb     r10,[r0,#24]		@lTemp3
	ldrb     r8,[r0,#8]			@lTemp1     
	ldrb     r11,[r0,#32]		@lTemp4  
	ldrb     r9,[r0,#16]		@lTemp2      
	ldrb	 r6,[r1]	
	smlabb	 r7,r4,r10,r7		 
	add		 r5,r8,r11
	ldrb     r12,[r0,#40]		@lTemp5						
	
	rsb		 r5,r5,r9,lsl #2
	add		 r7,r7,#0x20
	add		 r5,r5,r5,lsl #2		
	add		 r5,r5,r7
	add		 r5,r5,r12	
	ldrb     r7,[r0,#48]		@lTemp0
	usat     r5,#8,r5,asr #6		
	add		 r6,r6,#1
	uhadd16	 r5,r5,r6

	smlabb	 r8,r4,r11,r8
	strb	 r5,[r1],r3
	add		 r5,r9,r12
	ldrb	 r6,[r1]	
	rsb		 r5,r5,r10,lsl #2        
	add		 r8,r8,r7	
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r8
	add		 r5,r5,#0x20
	ldrb     r8,[r0,#56]		@lTemp1
	usat     r5,#8,r5,asr #6				
	add		 r6,r6,#1
	uhadd16	 r5,r5,r6
	
	smlabb	 r9,r4,r12,r9
	strb	 r5,[r1],r3
	add		 r5,r10,r7
	ldrb	 r6,[r1]	
	rsb		 r5,r5,r11,lsl #2
	add		 r9,r9,r8
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r9
	add		 r5,r5,#0x20
	ldrb     r9,[r0,#64]		@lTemp2
	usat     r5,#8,r5,asr #6				
	add		 r6,r6,#1
	uhadd16	 r5,r5,r6

	smlabb	 r10,r4,r7,r10
	strb	 r5,[r1],r3
	add		 r5,r11,r8
	ldrb	 r7,[r1]	
	rsb		 r5,r5,r12,lsl #2        
  @ldr		 r6,[sp,#-4]
  ldr		 r6,[sp,#12]
	add		 r10,r10,r9
	add		 r5,r5,r5,lsl #2
	add		 r5,r5,r10
	add		 r5,r5,#0x20
	add      r0,r0,#32
	usat     r5,#8,r5,asr #6
	add		 r7,r7,#1
	uhadd16	 r5,r5,r7
	subs     r6,r6,#4
	strb	 r5,[r1],r3
  @str		 r6,[sp,#-4]
  str		 r6,[sp,#12]

  bgt      H02V03_row_loop
    
	ldr	     r5,[sp]	
	sub		 r1,r1,r3,lsl #3
	sub		 r0,r0,#63
	add		 r1,r1,#1
	subs     r5,r5,#1
	str	     r5,[sp]	

    bgt      H02V03_col_loop
    
    @add      sp,sp,#TMP_BUFF_STRIDE*13+12
    add      sp,sp,#TMP_BUFF_STRIDE*13+16
    ldmfd    sp!,{r4-r11,pc}

	@ENDP


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_Interpolate4Add_H03V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	t0 = p0 + p1
@   v0 = (t0 + t1 + 2) >> 2
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
ARMV6_Interpolate4Add_H03V03:	@PROC

    stmfd    sp!,{r4-r11,lr}
    pld		 [r0]
    pld		 [r0,r2]
    mov      lr,#8
	
H03V03_row_loop:
    mov      r6,#8
    
H03V03_col_loop:

	ldrb     r7,[r0,#0]		@lTemp0
	ldrb     r8,[r0,#1]		@lTemp1
	add		 r11,r0,r2
    ldrb     r9,[r11,#0]	@lTemp2
    ldrb     r10,[r11,#1]	@lTemp3
	
	add		 r5,r7,r8
	add		 r5,r5,r9
	add		 r5,r5,r10
	ldrb     r7,[r0,#2]		@lTemp0
	add		 r5,r5,#2

	ldrb     r9,[r11,#2]	@lTemp2
    usat     r12,#8,r5,asr #2
	add		 r5,r10,r8		
	add		 r5,r5,r7
	add		 r5,r5,r9
	add		 r5,r5,#2

	ldrb     r8,[r0,#3]		@lTemp1
	usat     r5,#8,r5,asr #2
	ldrb     r10,[r11,#3]	@lTemp3
	orr		 r12,r12,r5,lsl #8
	add		 r5,r7,r9
	add		 r5,r5,r8
	add		 r5,r5,r10
	add		 r5,r5,#2

	ldrb     r7,[r0,#4]		@lTemp0
	usat     r5,#8,r5,asr #2
	ldrb     r9,[r11,#4]	@lTemp2
	orr		 r12,r12,r5,lsl #16
	add		 r5,r10,r8
	add		 r5,r5,r7
	add		 r5,r5,r9
	add		 r5,r5,#2

	ldr		 r7,[r1]	
	usat     r5,#8,r5,asr #2
	add      r0,r0,#4
	orr		 r12,r12,r5,lsl #24
	add		 r7,r7,#1
	@uhadd16	 r12,r12,r7
	uhadd8	 r12,r12,r7              @20110530 bug fixed
	subs	 r6,r6,#4
    str      r12,[r1],#4
    
    bgt      H03V03_col_loop
    
    add		 r1,r1,r3
    add		 r0,r0,r2
    sub		 r1,r1,#8
    sub		 r0,r0,#8
    subs     lr,lr,#1
    pld		 [r0,r2]
    
    bgt      H03V03_row_loop
    
    ldmfd    sp!,{r4-r11,pc}
        
	@ENDP
	

ARMV6_MCCopyChroma4Add_H00V00:	@PROC

	pld		[r0]
	pld		[r0,r2]
	ands    r12,r0,#0x03
	bne		H00V00_lab1

H00V00_lab0:	
	stmfd   sp!,{r4-r8,lr}
	ldr		r12,[r0],r2
	ldr		r5,[r1],r3
	ldr		lr,[r0],r2
	ldr		r6,[r1],r3
	ldr		r4,[r0],r2
	ldr		r7,[r1],r3
	ldr		r2,[r0]
	ldr		r8,[r1],r3
	uhadd8	r5,r5,r12
	sub		r1,r1,r3,lsl #2
	uhadd8	r6,r6,lr
	str		r5,[r1],r3
	str		r6,[r1],r3
	uhadd8	r7,r7,r4
	str		r7,[r1],r3
	uhadd8	r8,r8,r2	
	str		r8,[r1]
	ldmfd   sp!,{r4-r8,pc}

H00V00_lab1:	
	stmfd   sp!,{r4-r11,lr}
	sub		r0,r0,r12
	sub		r2,r2,#0x04
	ldr		r4,[r0],#0x04
	mov		r12,r12,lsl #3
	ldr		r5,[r0],r2
	rsb		lr,r12,#0x20
	ldr		r8,[r1],r3
	ldr		r6,[r0],#0x04
	ldr		r9,[r1],r3
	ldr		r7,[r0],r2
	mov		r4,r4,LSR r12
	orr		r4,r4,r5,lsl lr
	ldr		r10,[r1],r3
	ldr		r11,[r1],r3	
	uhadd8	r8,r8,r4
	sub		r1,r1,r3,lsl #2
	ldr		r4,[r0],#0x04
	mov		r6,r6,LSR r12
	ldr		r5,[r0],r2	
	orr		r6,r6,r7,lsl lr
	str		r8,[r1],r3
	uhadd8	r9,r9,r6
	mov		r4,r4,LSR r12
	ldr		r6,[r0],#0x04
	orr		r4,r4,r5,lsl lr
	ldr		r7,[r0],r2
	str		r9,[r1],r3
	uhadd8	r10,r10,r4	
	mov		r6,r6,LSR r12
	orr		r6,r6,r7,lsl lr
	uhadd8	r11,r11,r6
	str		r10,[r1],r3
	str		r11,[r1]
	
	ldmfd   sp!,{r4-r11,pc}
	
	@ENDP


ARMV6_MCCopyChroma4Add_H01V00:	@PROC
  
    stmfd    sp!,{r4-r9,lr}
    pld		 [r0]
	mov		 lr,#4
	
H01V00_loop:
    ldrb     r4,[r0,#0]
    ldrb     r5,[r0,#1]
	ldrb     r6,[r0,#2]
	ldrb     r7,[r0,#3]
	ldrb     r8,[r0,#4]
	add      r0,r0,r2

	add		 r9,r5,#1
    pld		 [r0]
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
	ldr		 r7,[r1]	
	mov		 r4,r4,asr #2
	mov		 r12,r12,asr #2
	orr		 r9,r9,r4,lsl #16
	orr		 r9,r9,r12,lsl #24
	uhadd8	 r9,r9,r7
    subs     lr,lr,#1
    str      r9,[r1],r3
			
    bgt      H01V00_loop
    
    ldmfd    sp!,{r4-r9,pc}
	@ENDP


ARMV6_MCCopyChroma4Add_H02V00:	@PROC
 
    stmfd    sp!,{r4-r8,lr}
    pld		 [r0]
    mov      r4,#4
    
H02V00_loop:
    ldrb     r5,[r0,#0]
    ldrb     r6,[r0,#1]
	ldrb     r7,[r0,#2]
	ldrb	 r8,[r0,#3]	
	ldrb	 lr,[r0,#4]	
	add      r0,r0,r2

	add		 r5,r5,#1
    pld		 [r0]
	add		 r5,r5,r6
	add		 r6,r6,#1
	mov		 r12,r5,asr #1
	add		 r6,r6,r7
	add		 r7,r7,#1
	mov		 r6,r6,asr #1
	add		 r5,r7,r8
	add		 r8,r8,#1
	ldr		 r7,[r1]	
	orr		 r12,r12,r6,lsl #8
	add		 r8,r8,lr
	mov		 r5,r5,asr #1
	mov		 r6,r8,asr #1
	orr		 r12,r12,r5,lsl #16
	orr		 r12,r12,r6,lsl #24		
	uhadd8	 r12,r12,r7
	subs     r4,r4,#1
    str      r12,[r1],r3
	
    bgt      H02V00_loop
    
    ldmfd    sp!,{r4-r8,pc}
	@ENDP
	

ARMV6_MCCopyChroma4Add_H03V00:	@PROC 
 
    stmfd    sp!,{r4-r9,lr}
    pld		 [r0]
	mov		 lr,#4
	
H03V00_loop:
    ldrb     r4,[r0,#0]
    ldrb     r5,[r0,#1]
	ldrb     r6,[r0,#2]
	ldrb     r7,[r0,#3]
	ldrb     r8,[r0,#4]
	add      r0,r0,r2

	add		 r9,r4,#1
    pld		 [r0]
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
	ldr		 r7,[r1]	
	mov		 r4,r4,asr #2
	mov		 r12,r12,asr #2
	orr		 r9,r9,r4,lsl #16
	orr		 r9,r9,r12,lsl #24
	uhadd8	 r9,r9,r7
    subs     lr,lr,#1
    str      r9,[r1],r3
			
    bgt      H03V00_loop
    
    ldmfd    sp!,{r4-r9,pc}
	@ENDP


ARMV6_MCCopyChroma4Add_H00V01:	@PROC
    
    stmfd    sp!,{r4-r10,lr}
    pld		 [r0]
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
	ldrb	 r10,[r1]	
	add		 r0,r0,#1
	add		 r4,r4,r4,lsl #1
	add		 r9,r9,r4
	add		 r12,r6,#2
	mov		 r9,r9,asr #2	
	add		 r10,r10,#1
	uhadd16	 r9,r9,r10
	add		 r5,r5,r5,lsl #1
	ldrb	 r10,[r1,r3]	
    strb     r9,[r1],r3
	add		 r12,r12,r5
	add		 r4,r7,#2
	mov		 r12,r12,asr #2	
	add		 r10,r10,#1
	uhadd16	 r12,r12,r10
	add		 r6,r6,r6,lsl #1
    strb     r12,[r1],r3
	add		 r4,r4,r6
	ldrb	 r10,[r1]	
	add		 r12,r8,#2
	mov		 r4,r4,asr #2
	add		 r7,r7,r7,lsl #1
	add		 r10,r10,#1
	uhadd16	 r4,r4,r10
	ldrb	 r10,[r1,r3]	
	add		 r12,r12,r7
    strb     r4,[r1],r3
	mov		 r12,r12,asr #2
	add		 r10,r10,#1
	uhadd16	 r12,r12,r10
    strb     r12,[r1],r3
			
    bgt      H00V01_loop
    
    ldmfd    sp!,{r4-r10,pc}
	@ENDP		


ARMV6_MCCopyChroma4Add_H01V01:	@PROC

    stmfd    sp!,{r4-r11,lr}	
    pld		 [r0]
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
	ldr		 r6,[r1]	
	add		 r7,r8,r7
	add		 r7,r7,r10		
    add      r4,r4,r2
	mov		 r7,r7,asr #4	
    add      r0,r0,r2
	orr		 r11,r11,r7,lsl #24

    pld		 [r0,r2]
	uhadd8	 r11,r11,r6
	subs	 lr,lr,#1	
    str      r11,[r1],r3
    bgt      H01V01_loop
    
    ldmfd    sp!,{r4-r11,pc}
	@ENDP	
	
	
ARMV6_MCCopyChroma4Add_H02V01:	@PROC

    stmfd    sp!,{r4-r11,lr}
    pld		 [r0]
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
	ldr		 r8,[r1]	
	add		 r6,r6,r7
	add		 r0,r0,r2		
	mov		 r6,r6,asr #3			
    pld		 [r0,r2]
    add      r4,r4,r2
	orr		 r11,r11,r6,lsl #24

	uhadd8	 r11,r11,r8
	subs	 lr,lr,#1	
    str      r11,[r1],r3
    bgt      H02V01_loop

    ldmfd    sp!,{r4-r11,pc}
	@ENDP	
	
	
ARMV6_MCCopyChroma4Add_H03V01:	@PROC

  stmfd    sp!,{r4-r11,lr}
  pld		 [r0]
	mov		 r12,#7		
  add      r4,r0,r2
	mov		 r5,#9		
  mov      lr,#4
  @str		 r3,[sp,#-4]
  sub    sp,sp,#0x04
  str    r3,[sp,#0]
    
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
	ldr		 r6,[r1]	
  @ldr		 r3,[sp,#-4]
  ldr    r3,[sp,#0]
	mov		 r9,r9,asr #4	
	add      r0,r0,r2	
	orr		 r11,r11,r9,lsl #24

	uhadd8	 r11,r11,r6
  pld		 [r0,r2]
  str      r11,[r1],r3
  bgt      H03V01_loop
  
  
  add    sp,sp,#0x04  
  ldmfd    sp!,{r4-r11,pc}
  
	@ENDP	
	
	
ARMV6_MCCopyChroma4Add_H00V02:	@PROC
   
    stmfd    sp!,{r4-r9,lr}
    pld		 [r0]
	add      r1,r1,r3,lsl #2
	sub		 r1,r1,#1
    mov      r4,#4
H00V02_loop:
    ldrb     r5,[r0],r2
	sub      r1,r1,r3,lsl #2
    ldrb     r6,[r0],r2
	add		 r1,r1,#1
	ldrb     r7,[r0],r2
	ldrb	 r9,[r1]	
	ldrb	 r8,[r0],r2	
	add		 r5,r5,r6
	ldrb	 lr,[r0]	
	sub      r0,r0,r2,lsl #2
	ldrb	 r12,[r1]	
	add		 r6,r6,r7
	add		 r0,r0,#1
	mov		 r5,r5,asr #1
	add		 r9,r9,#1
	uhadd16	 r5,r5,r9
	mov		 r6,r6,asr #1
    strb     r5,[r1],r3
	add		 r12,r12,#1
	ldrb	 r9,[r1]	
	uhadd16	 r6,r6,r12
	add		 r7,r7,r8
    strb     r6,[r1],r3
	mov		 r7,r7,asr #1
	ldrb	 r12,[r1]	
	add		 r9,r9,#1
	uhadd16	 r7,r7,r9
	add		 r8,r8,lr
    strb     r7,[r1],r3
	mov		 r8,r8,asr #1
	add		 r12,r12,#1
	uhadd16	 r8,r8,r12
	subs     r4,r4,#1
    strb     r8,[r1],r3
	
    bgt      H00V02_loop
    
    ldmfd    sp!,{r4-r9,pc}
	@ENDP	
	
	
ARMV6_MCCopyChroma4Add_H01V02:	@PROC

    stmfd    sp!,{r4-r11,lr}
    pld		 [r0]
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
	ldr		 r7,[r1]	
	add		 r6,r6,r8
	add		 r0,r0,r2		
	mov		 r6,r6,asr #3			
    pld		 [r0,r2]
    add      r4,r4,r2
	orr		 r11,r11,r6,lsl #24
	add		 r7,r7,#1
	uhadd8	 r11,r11,r7
	subs	 lr,lr,#1	
    str      r11,[r1],r3

    bgt      H01V02_loop

    ldmfd    sp!,{r4-r11,pc}
	@ENDP	
	
	
ARMV6_MCCopyChroma4Add_H02V02:	@PROC

    stmfd    sp!,{r4-r10,lr}
    pld		 [r0]
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
	ldr		 r9,[r1]	
    pld		 [r0,r2]    
    add      r7,r7,r8   
    add      r5,r5,r6
    mov      r7,r7,LSR #2
    add      r5,r5,r8
    orr      r12,r12,r7,lsl #16
    mov      r5,r5,LSR #2
    add      r4,r4,r2
    orr      r12,r12,r5,lsl #24    
	uhadd8	 r12,r12,r9
    subs     lr,lr,#1
    str      r12,[r1],r3
    bgt      H02V02_loop
    
    ldmfd    sp!,{r4-r10,pc}
	@ENDP	


	
ARMV6_MCCopyChroma4Add_H03V02:	@PROC

    stmfd    sp!,{r4-r11,lr}
    pld		 [r0]
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
	ldr		 r8,[r1]	
	add		 r0,r0,r2		
	add		 r6,r6,r7
    pld		 [r0,r2]
	orr		 r11,r11,r9,lsl #16
	mov		 r6,r6,asr #3			
    add      r4,r4,r2
	orr		 r11,r11,r6,lsl #24
	uhadd8	 r11,r11,r8
    str      r11,[r1],r3

    bgt      H03V02_loop
    
    ldmfd    sp!,{r4-r11,pc}
	@ENDP	


ARMV6_MCCopyChroma4Add_H00V03:	@PROC

    stmfd    sp!,{r4-r10,lr}
    pld		 [r0]
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
	ldrb	 r10,[r1]	
	add		 r4,r5,r5,lsl #1
	add		 r0,r0,#1
	add		 r9,r9,r4
	add		 r12,r5,#2
	mov		 r9,r9,asr #2	
	add		 r10,r10,#1
	uhadd16	 r9,r9,r10
	ldrb	 r10,[r1,r3]	
	add		 r5,r6,r6,lsl #1
    strb     r9,[r1],r3
	add		 r12,r12,r5
	add		 r4,r6,#2
	mov		 r12,r12,asr #2	
	add		 r10,r10,#1
	uhadd16	 r12,r12,r10
	add		 r6,r7,r7,lsl #1
	ldrb	 r10,[r1,r3]	
    strb     r12,[r1],r3
	add		 r4,r4,r6
	add		 r12,r7,#2
	add		 r7,r8,r8,lsl #1
	add		 r12,r12,r7
	mov		 r4,r4,asr #2
	add		 r10,r10,#1
	uhadd16	 r4,r4,r10
	ldrb	 r10,[r1,r3]	
    strb     r4,[r1],r3
	mov		 r12,r12,asr #2
	add		 r10,r10,#1
	uhadd16	 r12,r12,r10
    strb     r12,[r1],r3
			
    bgt      H00V03_loop
    
    ldmfd    sp!,{r4-r10,pc}
	@ENDP	
	
	
ARMV6_MCCopyChroma4Add_H01V03:	@PROC

  stmfd    sp!,{r4-r11,lr}
  pld		 [r0]
	mov		 r12,#7		
  add    r4,r0,r2
	mov		 r5,#9		
  mov    lr,#4
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
	ldr		 r6,[r1]	
	add		 r9,r8,r9      
  @ldr		 r3,[sp,#-4]
  ldr    r3,[sp,#0]
	mov		 r9,r9,asr #4	
	add      r0,r0,r2	
	orr		 r11,r11,r9,lsl #24
	uhadd8	 r11,r11,r6
  pld		 [r0,r2]
  str      r11,[r1],r3

  bgt      H01V03_loop
  
  add    sp,sp,#0x04    
  ldmfd    sp!,{r4-r11,pc}	
  @ENDP	
	
	
ARMV6_MCCopyChroma4Add_H02V03:	@PROC

    stmfd    sp!,{r4-r10,lr}
    pld		 [r0]
    add      r4,r0,r2
    mov      lr,#4
    
H02V03_loop:
	ldrb     r8,[r4,#0]	
	ldrb     r9,[r4,#1]	
	ldrb     r5,[r0,#0]		
    ldrb     r6,[r0,#1]	
    ldrb     r7,[r0,#2]	
	ldrb     r10,[r4,#2]		
	
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
		
	ldr		 r6,[r1]	
	add		 r8,r8,r8,lsl #1
	orr		 r12,r12,r7,lsl #16
	add		 r5,r5,r8	
	add		 r5,r5,#4	
	add		 r0,r0,r2		
	mov		 r5,r5,asr #3
    pld		 [r0,r2]
    add      r4,r4,r2
	orr		 r12,r12,r5,lsl #24
	uhadd8	 r12,r12,r6
	subs	 lr,lr,#1
    str      r12,[r1],r3

    bgt      H02V03_loop
    
    ldmfd    sp!,{r4-r10,pc}
	@ENDP	
	
	@END
