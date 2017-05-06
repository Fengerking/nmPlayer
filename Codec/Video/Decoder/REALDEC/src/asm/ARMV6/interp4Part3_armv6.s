;*****************************************************************************
;*																			*
;*		VisualOn, Inc. Confidential and Proprietary, 2009					*
;*																			*
;*****************************************************************************

	AREA	|.text|, CODE
	
	EXPORT	ARMV6_Interpolate4Add_H00V00
	EXPORT	ARMV6_Interpolate4Add_H01V00
	EXPORT	ARMV6_Interpolate4Add_H02V00
	EXPORT	ARMV6_Interpolate4Add_H03V00
	EXPORT	ARMV6_Interpolate4Add_H00V01
	EXPORT	ARMV6_Interpolate4Add_H01V01
	EXPORT	ARMV6_Interpolate4Add_H02V01
	EXPORT  ARMV6_Interpolate4Add_H03V01
	EXPORT	ARMV6_Interpolate4Add_H00V02
	EXPORT	ARMV6_Interpolate4Add_H01V02
	EXPORT	ARMV6_Interpolate4Add_H02V02
	EXPORT	ARMV6_Interpolate4Add_H03V02
	EXPORT	ARMV6_Interpolate4Add_H00V03
	EXPORT	ARMV6_Interpolate4Add_H01V03
	EXPORT	ARMV6_Interpolate4Add_H02V03
	EXPORT	ARMV6_Interpolate4Add_H03V03
	EXPORT	ARMV6_MCCopyChroma4Add_H00V00
	EXPORT	ARMV6_MCCopyChroma4Add_H01V00
	EXPORT	ARMV6_MCCopyChroma4Add_H02V00
	EXPORT	ARMV6_MCCopyChroma4Add_H03V00
	EXPORT	ARMV6_MCCopyChroma4Add_H00V01
	EXPORT	ARMV6_MCCopyChroma4Add_H01V01
	EXPORT	ARMV6_MCCopyChroma4Add_H02V01 
	EXPORT	ARMV6_MCCopyChroma4Add_H03V01
	EXPORT	ARMV6_MCCopyChroma4Add_H00V02
	EXPORT	ARMV6_MCCopyChroma4Add_H01V02
	EXPORT	ARMV6_MCCopyChroma4Add_H02V02
	EXPORT	ARMV6_MCCopyChroma4Add_H03V02
	EXPORT	ARMV6_MCCopyChroma4Add_H00V03
	EXPORT	ARMV6_MCCopyChroma4Add_H01V03
	EXPORT	ARMV6_MCCopyChroma4Add_H02V03
	
;R0 = pSrc
;R1 = pDst
;R2 = uSrcPitch
;R3 = uDstPitch


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	void  C_Interpolate4Add_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;
;	No interpolation required, simple block copy.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ARMV6_Interpolate4Add_H00V00	PROC

    stmdb    sp!,{r4-r11,lr}
	ands    r12,r0,#0x03
	bne		H00V00_notAligned4

H00V00_Aligned4
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

H00V00_notAligned4

	mov		r10, r1
	sub		r0,r0,r12
	mov		r11, #8
	mov		r12,r12,LSL #3
	rsb		lr,r12,#32	
H00V00_loop	
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

	ENDP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	void  C_Interpolate4Add_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;
;	h filter (1,-5,52,20,-5,1) 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ARMV6_Interpolate4Add_H01V00	PROC
	    
    STMFD    sp!,{r4-r11,lr}
	pld		 [r0]
	pld		 [r0,r2]
	MOV		 r4,#52	    	
	;STR	     r2,[sp,#-4]
	SUB    sp,sp,#0x08
	STR    r2,[sp,#0]   
	MOV      lr,#8
	;STR	     r3,[sp,#-8]
	STR      r3,[sp,#4]
		
H01V00_row_loop
	MOV	     r6,#8
	
H01V00_col_loop

	; t0 = ( 52*p2 + 5*(p3<<2 - (p1+p4)) + (p0+p5) + 32 ) >> 6
	
    LDRB     r9,[r0,#0]		;lTemp2
	LDRB     r7,[r0,#-2]	;lTemp0
    LDRB     r8,[r0,#-1]	;lTemp1
    LDRB     r11,[r0,#2]	;lTemp4
    LDRB     r10,[r0,#1]	;lTemp3
	SMLABB	 r7,r4,r9,r7     
	ADD		 r5,r8,r11
	LDRB     r12,[r0,#3]	;lTemp5						
	RSB		 r5,r5,r10,LSL #2
	ADD		 r7,r7,#0x20
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r7
	ADD		 r5,r5,r12
	LDRB     r7,[r0,#4]		;lTemp0
    USAT     r3,#8,r5,ASR #6		

	SMLABB	 r8,r4,r10,r8
	ADD		 r5,r9,r12
	RSB		 r5,r5,r11,LSL #2
	ADD		 r8,r8,r7	
	ADD		 r5,r5,r5,LSL #2        
	ADD		 r5,r5,r8
	ADD		 r5,r5,#0x20
	LDRB     r8,[r0,#5]		;lTemp1
    USAT     r2,#8,r5,ASR #6				
	
	SMLABB	 r9,r4,r11,r9
	ADD		 r5,r10,r7
	ORR		 r3,r3,r2,LSL #8
	RSB		 r5,r5,r12,LSL #2                
	ADD		 r9,r9,r8	
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r9
	ADD		 r5,r5,#0x20
	LDRB     r9,[r0,#6]		;lTemp2
    USAT     r2,#8,r5,ASR #6				

	SMLABB	 r10,r4,r12,r10
	ADD		 r5,r11,r8
	ORR		 r3,r3,r2,LSL #16
	RSB		 r5,r5,r7,LSL #2
	LDR		 r11,[r1]
	ADD		 r10,r10,r9
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r10
	ADD		 r5,r5,#0x20
	ADD      r0,r0,#4
    USAT     r2,#8,r5,ASR #6
	SUBS     r6,r6,#4
	ORR		 r3,r3,r2,LSL #24
	UHADD8	 r3,r3,r11
    STR      r3,[r1],#4

    BGT      H01V00_col_loop
    
	;LDR	     r2,[sp,#-4]
	;LDR	     r3,[sp,#-8]
	LDR    r2,[sp,#0]
	LDR    r3,[sp,#4]
	SUB		 r1,r1,#8		
	SUB		 r0,r0,#8
	SUBS     lr,lr,#1
    ADD		 r0,r0,r2         
	ADD		 r1,r1,r3
	pld		 [r0,r2]
    BGT      H01V00_row_loop
    
    ADD    sp,sp,#0x08    
    LDMFD    sp!,{r4-r11,pc}

	ENDP
		
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	void  C_Interpolate4Add_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;
;	h filter (1,-5,20,20,-5,1) 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ARMV6_Interpolate4Add_H02V00	PROC

    STMFD    sp!,{r4-r11,lr}
	pld		 [r0]
	pld		 [r0,r2]
    MOV      lr,#8 
    ;STR	     r2,[sp,#-4]
    SUB    sp,sp,#0x04
    STR    r2,[sp,#0]
    
H02V00_row_loop
    MOV      r6,#8
    
H02V00_col_loop

    LDRB     r9,[r0,#0]		;lTemp2
	LDRB     r10,[r0,#1]	;lTemp3
	LDRB     r11,[r0,#2]	;lTemp4
    LDRB     r8,[r0,#-1]	;lTemp1        
	ADD		 r5,r9,r10	
    LDRB     r7,[r0,#-2]	;lTemp0
	RSB		 r5,r8,r5,LSL #2
	LDRB     r12,[r0,#3]	;lTemp5	
	
	SUB		 r5,r5,r11
	ADD		 r7,r7,#0x10
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r7
	ADD		 r5,r5,r12
	LDRB     r7,[r0,#4]		;lTemp0
    USAT     r4,#8,r5,ASR #5		

	ADD		 r5,r10,r11
	ADD		 r8,r8,#0x10
	RSB		 r5,r9,r5,LSL #2        
	SUB		 r5,r5,r12
	ADD		 r8,r8,r7
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r8
	LDRB     r8,[r0,#5]		;lTemp1
	USAT     r2,#8,r5,ASR #5
			
	ADD		 r5,r11,r12
	ADD		 r9,r9,#0x10
	ORR		 r4,r4,r2,LSL #8
	RSB		 r5,r10,r5,LSL #2        
	SUB		 r5,r5,r7
	ADD		 r9,r9,r8 
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r9
	LDRB     r9,[r0,#6]		;lTemp2
	USAT     r2,#8,r5,ASR #5	
	
	ADD		 r5,r12,r7
	ADD		 r10,r10,#0x10
	ORR		 r4,r4,r2,LSL #16
	RSB		 r5,r11,r5,LSL #2        
	SUB		 r5,r5,r8
	LDR		 r11,[r1]
	ADD		 r10,r10,r9
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r10
	ADD      r0,r0,#4
	USAT     r2,#8,r5,ASR #5
	SUBS     r6,r6,#4
	ORR		 r4,r4,r2,LSL #24   
	UHADD8 	 r4,r4,r11
    STR      r4,[r1],#4

    BGT      H02V00_col_loop
    
    ;LDR	     r2,[sp,#-4]
    LDR    r2,[sp,#0]
    SUB		 r1,r1,#8
	SUB		 r0,r0,#8
    ADD		 r1,r1,r3 
    SUBS     lr,lr,#1
    ADD		 r0,r0,r2   
	pld		 [r0,r2]
	

    BGT      H02V00_row_loop
    
    ADD      sp,sp,#0x04
    LDMFD    sp!,{r4-r11,pc}

	ENDP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	void  C_Interpolate4Add_H03V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;
;	h filter (1,-5,20,52,-5,1) 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ARMV6_Interpolate4Add_H03V00	PROC

    STMFD    sp!,{r4-r11,lr}
	pld		 [r0]
	pld		 [r0,r2]
	MOV		 r4,#52
	;STR	     r2,[sp,#-4]
	SUB    sp,sp,#0x08
	STR    r2,[sp,#0]
	MOV      lr,#8
	;STR	     r3,[sp,#-8]
	STR      r3,[sp,#4]
	    	
H03V00_row_loop
    MOV	     r6,#8
    
H03V00_col_loop

	LDRB     r7,[r0,#-2]	;lTemp0
    LDRB     r10,[r0,#1]	;lTemp3
	LDRB     r8,[r0,#-1]	;lTemp1     
	LDRB     r11,[r0,#2]	;lTemp4 
	LDRB     r9,[r0,#0]		;lTemp2       
	SMLABB	 r7,r4,r10,r7			 
	ADD		 r5,r8,r11
	LDRB     r12,[r0,#3]	;lTemp5						
	
	RSB		 r5,r5,r9,LSL #2
	ADD		 r7,r7,#0x20
	ADD		 r5,r5,r5,LSL #2		
	ADD		 r5,r5,r7
	ADD		 r5,r5,r12	
	LDRB     r7,[r0,#4]	;lTemp0
	USAT     r2,#8,r5,ASR #6		

	SMLABB	 r8,r4,r11,r8
	ADD		 r5,r9,r12
	RSB		 r5,r5,r10,LSL #2        
	ADD		 r8,r8,r7	
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r8
	ADD		 r5,r5,#0x20
	LDRB     r8,[r0,#5]		;lTemp1
	USAT     r3,#8,r5,ASR #6				
	
	SMLABB	 r9,r4,r12,r9	
	ADD		 r5,r10,r7
	ORR		 r2,r2,r3,LSL #8
	RSB		 r5,r5,r11,LSL #2
	ADD		 r9,r9,r8
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r9
	ADD		 r5,r5,#0x20
	LDRB     r9,[r0,#6]		;lTemp2
	USAT     r3,#8,r5,ASR #6				

	SMLABB	 r10,r4,r7,r10	
	ADD		 r5,r11,r8
	ORR		 r2,r2,r3,LSL #16
	RSB		 r5,r5,r12,LSL #2        
 	LDR		 r11,[r1]
	ADD		 r10,r10,r9
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r10
	ADD		 r5,r5,#0x20
	ADD      r0,r0,#4
	USAT     r3,#8,r5,ASR #6
	SUBS     r6,r6,#4
	ORR		 r2,r2,r3,LSL #24        
	UHADD8 	 r2,r2,r11
    STR      r2,[r1],#4

    BGT      H03V00_col_loop
    
	;LDR	     r2,[sp,#-4]
	;LDR	     r3,[sp,#-8]
	LDR    r2,[sp,#0]
	LDR    r3,[sp,#4]	
	SUB		 r1,r1,#8
	SUB		 r0,r0,#8
	SUBS     lr,lr,#1
    ADD		 r0,r0,r2   
    ADD		 r1,r1,r3     
	pld		 [r0,r2]	

    BGT      H03V00_row_loop
    
    ADD    sp,sp,#0x08    
    LDMFD    sp!,{r4-r11,pc}

    ENDP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	void  C_Interpolate4Add_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;
;	v filter (1,-5,52,20,-5,1) 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ARMV6_Interpolate4Add_H00V01	PROC
	
    STMFD    sp!,{r4-r11,lr}
	pld		 [r0]
	MOV      r5,#8 
	MOV		 r4,#52	
	;STR	     r5,[sp,#-4]
	SUB    sp,sp,#0x08
	STR    r5,[sp,#0]        
	ADD		 lr,r2,r2,LSL #1	;3*uSrcPitch
	
H00V01_col_loop
	MOV	     r6,#8
	;STR	     r6,[sp,#-8]
	STR      r6,[sp,#4]        
	
H00V01_row_loop

	; t0 = p0
	; v0 = ( 52*t2 + 5*(t3<<2 - (t1+t4)) + (t0+t5) + 32 ) >> 6
	
    LDRB     r9,[r0]			;lTemp2
	LDRB     r7,[r0,-r2,LSL #1]	;lTemp0
    LDRB     r8,[r0,-r2]		;lTemp1
    LDRB     r11,[r0,r2,LSL #1]	;lTemp4
    LDRB     r10,[r0,r2]		;lTemp3
    LDRB	 r6,[r1]
	SMLABB	 r7,r4,r9,r7         
	ADD		 r5,r8,r11
	LDRB     r12,[r0,lr]		;lTemp5						
	RSB		 r5,r5,r10,LSL #2
	ADD		 r7,r7,#0x20
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r7
	ADD		 r6,r6,#1
	ADD		 r5,r5,r12
	LDRB     r7,[r0,r2,LSL #2]	;lTemp0
    USAT     r5,#8,r5,ASR #6		
	pld		 [r0,r2]
	UHADD16	 r5,r5,r6

	SMLABB	 r8,r4,r10,r8
    STRB     r5,[r1],r3        
	ADD		 r5,r9,r12
    LDRB	 r6,[r1]
	RSB		 r5,r5,r11,LSL #2
	ADD		 r8,r8,r7	
	ADD		 r5,r5,r5,LSL #2        
	ADD		 r5,r5,r8
	ADD		 r8,lr,r2,LSL #1	;5*uSrcPitch
	ADD		 r5,r5,#0x20
	LDRB     r8,[r0,r8]			;lTemp1
    USAT     r5,#8,r5,ASR #6				
	ADD		 r6,r6,#1
	UHADD16	 r5,r5,r6
	
	SMLABB	 r9,r4,r11,r9
    STRB     r5,[r1],r3        
	ADD		 r5,r10,r7
    LDRB	 r6,[r1]
	RSB		 r5,r5,r12,LSL #2                
	ADD		 r9,r9,r8	
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r9
	ADD		 r5,r5,#0x20
	LDRB     r9,[r0,lr,LSL #1]	;lTemp2
    USAT     r5,#8,r5,ASR #6				
	ADD		 r6,r6,#1
	UHADD16	 r5,r5,r6

	SMLABB	 r10,r4,r12,r10
    STRB     r5,[r1],r3        
	ADD		 r5,r11,r8
    LDRB	 r12,[r1]
	;LDR	     r6,[sp,#-8]
	LDR    r6,[sp,#4]        
	RSB		 r5,r5,r7,LSL #2
	ADD		 r10,r10,r9
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r10
	ADD		 r5,r5,#0x20
	ADD      r0,r0,r2,LSL #2
  USAT     r5,#8,r5,ASR #6
	ADD		 r12,r12,#1
	UHADD16	 r5,r5,r12
	SUBS     r6,r6,#4
  STRB     r5,[r1],r3        
	;STR	     r6,[sp,#-8]
	STR      r6,[sp,#4]        

    BGT      H00V01_row_loop
    
	;LDR	     r5,[sp,#-4]
	LDR    r5,[sp,#0]
	SUB		 r0,r0,r2,LSL #3
	SUB		 r1,r1,r3,LSL #3		
  ADD		 r0,r0,#1         
	SUBS   r5,r5,#1
	ADD		 r1,r1,#1
	;STR	     r5,[sp,#-4]
	STR    r5,[sp,#0]

  BGT      H00V01_col_loop
  
  ADD    sp,sp,#0x08  
  LDMFD    sp!,{r4-r11,pc}

	ENDP


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	void  C_Interpolate4Add_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;
;	h/v filter (1,-5,52,20,-5,1) 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ARMV6_Interpolate4Add_H01V01	PROC
	    
TMP_BUFF_STRIDE		EQU		8	;shared by HxVy functions(when x and y not equals 0).
	    
    STMFD    sp!,{r4-r11,lr}
    SUB		 r0,r0,r2,LSL #1
	pld		 [r0]
	pld		 [r0,r2]
	MOV		 r4,#52	    	
  ;SUB      sp,sp,#TMP_BUFF_STRIDE*13+12
  SUB      sp,sp,#TMP_BUFF_STRIDE*13+16
	MOV      lr,#8+5
	STR		 r1,[sp,#0]	
	STR		 r2,[sp,#4]	
	STR	     r3,[sp,#8]   
	;ADD		 r1,sp,#12		;&buff[TMP_BUFF_STRIDE*13]
	ADD		 r1,sp,#16		;&buff[TMP_BUFF_STRIDE*13]

;;;;;;;; H interpolate ;;;;;;;;;
		
H01V01_H_row_loop
	MOV	     r6,#8
	
H01V01_H_col_loop

	; t0 = ( 52*p2 + 5*(p3<<2 - (p1+p4)) + (p0+p5) + 32 ) >> 6
	
    LDRB     r9,[r0,#0]		;lTemp2
	LDRB     r7,[r0,#-2]	;lTemp0
    LDRB     r8,[r0,#-1]	;lTemp1
    LDRB     r11,[r0,#2]	;lTemp4
    LDRB     r10,[r0,#1]	;lTemp3
	SMLABB	 r7,r4,r9,r7     
	ADD		 r5,r8,r11
	LDRB     r12,[r0,#3]	;lTemp5						
	RSB		 r5,r5,r10,LSL #2
	ADD		 r7,r7,#0x20
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r7
	ADD		 r5,r5,r12
	LDRB     r7,[r0,#4]		;lTemp0
    USAT     r3,#8,r5,ASR #6		

	SMLABB	 r8,r4,r10,r8
	ADD		 r5,r9,r12
	RSB		 r5,r5,r11,LSL #2
	ADD		 r8,r8,r7	
	ADD		 r5,r5,r5,LSL #2        
	ADD		 r5,r5,r8
	ADD		 r5,r5,#0x20
	LDRB     r8,[r0,#5]		;lTemp1
    USAT     r2,#8,r5,ASR #6				
	
	SMLABB	 r9,r4,r11,r9
	ADD		 r5,r10,r7
	ORR		 r3,r3,r2,LSL #8
	RSB		 r5,r5,r12,LSL #2                
	ADD		 r9,r9,r8	
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r9
	ADD		 r5,r5,#0x20
	LDRB     r9,[r0,#6]		;lTemp2
    USAT     r2,#8,r5,ASR #6				

	SMLABB	 r10,r4,r12,r10
	ADD		 r5,r11,r8
	ORR		 r3,r3,r2,LSL #16
	RSB		 r5,r5,r7,LSL #2
	ADD		 r10,r10,r9
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r10
	ADD		 r5,r5,#0x20
	ADD      r0,r0,#4
    USAT     r2,#8,r5,ASR #6
	SUBS     r6,r6,#4
	ORR		 r3,r3,r2,LSL #24
    STR      r3,[r1],#4

    BGT      H01V01_H_col_loop
    
	LDR	     r2,[sp,#4]
	SUB		 r0,r0,#8
	SUBS     lr,lr,#1
    ADD		 r0,r0,r2         
	pld		 [r0,r2]
    BGT      H01V01_H_row_loop

;;;;;;;; V interpolate ;;;;;;;;;
    
	;ADD		 r0,sp,#12		;&buff[TMP_BUFF_STRIDE*13]
	ADD		 r0,sp,#16		;&buff[TMP_BUFF_STRIDE*13]
	LDR		 r1,[sp]	
	LDR	   r3,[sp,#8]
	pld		 [r0]
	MOV    lr,#8 
	MOV		 r4,#52	
	;STR	   lr,[sp,#-4]
	STR    lr,[sp,#12]
	
H01V01_V_col_loop
	MOV	     r6,#8
	
H01V01_V_row_loop

	; v0 = ( 52*t2 + 5*(t3<<2 - (t1+t4)) + (t0+t5) + 32 ) >> 6
	
  LDRB     r9,[r0,#16]		;lTemp2
	LDRB     r7,[r0]			;lTemp0
  LDRB     r8,[r0,#8]			;lTemp1
  LDRB     r11,[r0,#32]		;lTemp4
  LDRB     r10,[r0,#24]		;lTemp3
  LDRB	 lr,[r1]
	SMLABB	 r7,r4,r9,r7         
	ADD		 r5,r8,r11
	LDRB     r12,[r0,#40]		;lTemp5						
	pld		 [r0,#TMP_BUFF_STRIDE]
	RSB		 r5,r5,r10,LSL #2
	ADD		 r7,r7,#0x20
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r7
	ADD		 r5,r5,r12
	LDRB     r7,[r0,#48]		;lTemp0
    USAT     r5,#8,r5,ASR #6		
	ADD		 lr,lr,#1
	UHADD16	 r5,r5,lr

	SMLABB	 r8,r4,r10,r8
    STRB     r5,[r1],r3        
	ADD		 r5,r9,r12
    LDRB	 lr,[r1]
	RSB		 r5,r5,r11,LSL #2
	ADD		 r8,r8,r7	
	ADD		 r5,r5,r5,LSL #2        
	ADD		 r5,r5,r8
	ADD		 r5,r5,#0x20
	LDRB     r8,[r0,#56]		;lTemp1
    USAT     r5,#8,r5,ASR #6				
	ADD		 lr,lr,#1
	UHADD16	 r5,r5,lr
	
	SMLABB	 r9,r4,r11,r9
    STRB     r5,[r1],r3        
	ADD		 r5,r10,r7
    LDRB	 lr,[r1]
	RSB		 r5,r5,r12,LSL #2                
	ADD		 r9,r9,r8	
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r9
	ADD		 r5,r5,#0x20
	LDRB     r9,[r0,#64]		;lTemp2
    USAT     r5,#8,r5,ASR #6				
	ADD		 lr,lr,#1
	UHADD16	 r5,r5,lr

	SMLABB	 r10,r4,r12,r10
    STRB     r5,[r1],r3        
	ADD		 r5,r11,r8
    LDRB	 lr,[r1]
	RSB		 r5,r5,r7,LSL #2
	ADD		 r10,r10,r9
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r10
	ADD		 r5,r5,#0x20
	ADD      r0,r0,#32
    USAT     r5,#8,r5,ASR #6
	ADD		 lr,lr,#1
	UHADD16	 r5,r5,lr
	SUBS     r6,r6,#4
  STRB     r5,[r1],r3        

  BGT      H01V01_V_row_loop
    
	;LDR	     lr,[sp,#-4]
	LDR    lr,[sp,#12]
	SUB		 r0,r0,#63			;-8<<3 +1
	SUB		 r1,r1,r3,LSL #3		
	SUBS   lr,lr,#1
	ADD		 r1,r1,#1
	;STR	   lr,[sp,#-4]
	STR    lr,[sp,#12]

  BGT      H01V01_V_col_loop    
    
  ;ADD      sp,sp,#TMP_BUFF_STRIDE*13+12
  ADD      sp,sp,#TMP_BUFF_STRIDE*13+16
  LDMFD    sp!,{r4-r11,pc}

	ENDP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	void  C_Interpolate4Add_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;
;	h filter (1,-5,20,20,-5,1) 
;	v filter (1,-5,52,20,-5,1) 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ARMV6_Interpolate4Add_H02V01	PROC
	    	    
  STMFD    sp!,{r4-r11,lr}
  SUB		 r0,r0,r2,LSL #1
	pld		 [r0]
	pld		 [r0,r2]
  ;SUB      sp,sp,#TMP_BUFF_STRIDE*13+12
  SUB      sp,sp,#TMP_BUFF_STRIDE*13+16
	MOV      lr,#8+5
	STR		 r1,[sp,#0]	
	STR		 r2,[sp,#4]	
	STR	     r3,[sp,#8]   
	;ADD		 r1,sp,#12		;&buff[TMP_BUFF_STRIDE*13]
	ADD		 r1,sp,#16		;&buff[TMP_BUFF_STRIDE*13]

;;;;;;;; H interpolate ;;;;;;;;;
		
H02V01_H_row_loop
	MOV	     r6,#8
	
H02V01_H_col_loop
	
    LDRB     r9,[r0,#0]		;lTemp2
	LDRB     r10,[r0,#1]	;lTemp3
	LDRB     r11,[r0,#2]	;lTemp4
    LDRB     r8,[r0,#-1]	;lTemp1        
	ADD		 r5,r9,r10	
    LDRB     r7,[r0,#-2]	;lTemp0
	RSB		 r5,r8,r5,LSL #2
	LDRB     r12,[r0,#3]	;lTemp5	
	
	SUB		 r5,r5,r11
	ADD		 r7,r7,#0x10
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r7
	ADD		 r5,r5,r12
	LDRB     r7,[r0,#4]		;lTemp0
    USAT     r4,#8,r5,ASR #5		

	ADD		 r5,r10,r11
	ADD		 r8,r8,#0x10
	RSB		 r5,r9,r5,LSL #2        
	SUB		 r5,r5,r12
	ADD		 r8,r8,r7
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r8
	LDRB     r8,[r0,#5]		;lTemp1
	USAT     r2,#8,r5,ASR #5
			
	ADD		 r5,r11,r12
	ADD		 r9,r9,#0x10
	ORR		 r4,r4,r2,LSL #8
	RSB		 r5,r10,r5,LSL #2        
	SUB		 r5,r5,r7
	ADD		 r9,r9,r8 
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r9
	LDRB     r9,[r0,#6]		;lTemp2
	USAT     r2,#8,r5,ASR #5	
	
	ADD		 r5,r12,r7
	ADD		 r10,r10,#0x10
	ORR		 r4,r4,r2,LSL #16
	RSB		 r5,r11,r5,LSL #2        
	SUB		 r5,r5,r8
	ADD		 r10,r10,r9
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r10
	ADD      r0,r0,#4
	USAT     r2,#8,r5,ASR #5
	SUBS     r6,r6,#4
	ORR		 r4,r4,r2,LSL #24   
    STR      r4,[r1],#4

    BGT      H02V01_H_col_loop
    
    LDR	     r2,[sp,#4]
	SUB		 r0,r0,#8
    SUBS     lr,lr,#1
    ADD		 r0,r0,r2   
	pld		 [r0,r2]
    BGT      H02V01_H_row_loop

;;;;;;;; V interpolate ;;;;;;;;;
    
	;ADD		 r0,sp,#12		;&buff[TMP_BUFF_STRIDE*13]
	ADD		 r0,sp,#16		;&buff[TMP_BUFF_STRIDE*13]
	LDR		 r1,[sp]	
	LDR	   r3,[sp,#8]
	pld		 [r0]
	MOV    lr,#8 
	MOV		 r4,#52	
	;STR	   lr,[sp,#-4]
	STR	   lr,[sp,#12]
	
H02V01_V_col_loop
	MOV	     r6,#8
	
H02V01_V_row_loop

	; v0 = ( 52*t2 + 5*(t3<<2 - (t1+t4)) + (t0+t5) + 32 ) >> 6
	
    LDRB     r9,[r0,#16]		;lTemp2
	LDRB     r7,[r0]			;lTemp0
    LDRB     r8,[r0,#8]			;lTemp1
    LDRB     r11,[r0,#32]		;lTemp4
    LDRB     r10,[r0,#24]		;lTemp3
    LDRB	 lr,[r1]
	SMLABB	 r7,r4,r9,r7         
	ADD		 r5,r8,r11
	LDRB     r12,[r0,#40]		;lTemp5						
	pld		 [r0,#TMP_BUFF_STRIDE]
	RSB		 r5,r5,r10,LSL #2
	ADD		 r7,r7,#0x20
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r7
	ADD		 r5,r5,r12
	LDRB     r7,[r0,#48]		;lTemp0
    USAT     r5,#8,r5,ASR #6		
	ADD		 lr,lr,#1
	UHADD16	 r5,r5,lr

	SMLABB	 r8,r4,r10,r8
    STRB     r5,[r1],r3        
	ADD		 r5,r9,r12
    LDRB	 lr,[r1]
	RSB		 r5,r5,r11,LSL #2
	ADD		 r8,r8,r7	
	ADD		 r5,r5,r5,LSL #2        
	ADD		 r5,r5,r8
	ADD		 r5,r5,#0x20
	LDRB     r8,[r0,#56]		;lTemp1
    USAT     r5,#8,r5,ASR #6				
	ADD		 lr,lr,#1
	UHADD16	 r5,r5,lr
	
	SMLABB	 r9,r4,r11,r9
    STRB     r5,[r1],r3        
	ADD		 r5,r10,r7
    LDRB	 lr,[r1]
	RSB		 r5,r5,r12,LSL #2                
	ADD		 r9,r9,r8	
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r9
	ADD		 r5,r5,#0x20
	LDRB     r9,[r0,#64]		;lTemp2
    USAT     r5,#8,r5,ASR #6				
	ADD		 lr,lr,#1
	UHADD16	 r5,r5,lr

	SMLABB	 r10,r4,r12,r10
  STRB     r5,[r1],r3        
	ADD		 r5,r11,r8
  LDRB	 lr,[r1]
	RSB		 r5,r5,r7,LSL #2
	ADD		 r10,r10,r9
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r10
	ADD		 r5,r5,#0x20
	ADD      r0,r0,#32
  USAT     r5,#8,r5,ASR #6
	ADD		 lr,lr,#1
	UHADD16	 r5,r5,lr
	SUBS     r6,r6,#4
  STRB     r5,[r1],r3        

  BGT      H02V01_V_row_loop
    
	;LDR	     lr,[sp,#-4]
	LDR	   lr,[sp,#12]
	SUB		 r0,r0,#63			;-8<<3 +1
	SUB		 r1,r1,r3,LSL #3		
	SUBS   lr,lr,#1
	ADD		 r1,r1,#1
	;STR	   lr,[sp,#-4]
	STR	   lr,[sp,#12]

  BGT      H02V01_V_col_loop    
    
 ;ADD      sp,sp,#TMP_BUFF_STRIDE*13+12
  ADD      sp,sp,#TMP_BUFF_STRIDE*13+16
  LDMFD    sp!,{r4-r11,pc}

	ENDP
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	void  C_Interpolate4Add_H03V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;
;	h filter (1,-5,20,52,-5,1) 
;	v filter (1,-5,52,20,-5,1) 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ARMV6_Interpolate4Add_H03V01	PROC
	    	    
  STMFD    sp!,{r4-r11,lr}
  SUB		 r0,r0,r2,LSL #1
	pld		 [r0]
	pld		 [r0,r2]
	MOV		 r4,#52	    	
  ;SUB      sp,sp,#TMP_BUFF_STRIDE*13+12
  SUB    sp,sp,#TMP_BUFF_STRIDE*13+16
	MOV    lr,#8+5
	STR		 r1,[sp,#0]	
	STR		 r2,[sp,#4]	
	STR	   r3,[sp,#8]   
	;ADD		 r1,sp,#12		;&buff[TMP_BUFF_STRIDE*13]
	ADD		 r1,sp,#16		;&buff[TMP_BUFF_STRIDE*13]

;;;;;;;; H interpolate ;;;;;;;;;
		
H03V01_H_row_loop
	MOV	     r6,#8
	
H03V01_H_col_loop
	
	LDRB     r7,[r0,#-2]	;lTemp0
    LDRB     r10,[r0,#1]	;lTemp3
	LDRB     r8,[r0,#-1]	;lTemp1     
	LDRB     r11,[r0,#2]	;lTemp4 
	LDRB     r9,[r0,#0]		;lTemp2       
	SMLABB	 r7,r4,r10,r7			 
	ADD		 r5,r8,r11
	LDRB     r12,[r0,#3]	;lTemp5						
	
	RSB		 r5,r5,r9,LSL #2
	ADD		 r7,r7,#0x20
	ADD		 r5,r5,r5,LSL #2		
	ADD		 r5,r5,r7
	ADD		 r5,r5,r12	
	LDRB     r7,[r0,#4]	;lTemp0
	USAT     r2,#8,r5,ASR #6		

	SMLABB	 r8,r4,r11,r8
	ADD		 r5,r9,r12
	RSB		 r5,r5,r10,LSL #2        
	ADD		 r8,r8,r7	
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r8
	ADD		 r5,r5,#0x20
	LDRB     r8,[r0,#5]		;lTemp1
	USAT     r3,#8,r5,ASR #6				
	
	SMLABB	 r9,r4,r12,r9	
	ADD		 r5,r10,r7
	ORR		 r2,r2,r3,LSL #8
	RSB		 r5,r5,r11,LSL #2
	ADD		 r9,r9,r8
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r9
	ADD		 r5,r5,#0x20
	LDRB     r9,[r0,#6]		;lTemp2
	USAT     r3,#8,r5,ASR #6				

	SMLABB	 r10,r4,r7,r10	
	ADD		 r5,r11,r8
	ORR		 r2,r2,r3,LSL #16
	RSB		 r5,r5,r12,LSL #2        
	ADD		 r10,r10,r9
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r10
	ADD		 r5,r5,#0x20
	ADD      r0,r0,#4
	USAT     r3,#8,r5,ASR #6
	SUBS     r6,r6,#4
	ORR		 r2,r2,r3,LSL #24        
    STR      r2,[r1],#4

    BGT      H03V01_H_col_loop
    
	LDR	     r2,[sp,#4]
	SUB		 r0,r0,#8
	SUBS     lr,lr,#1
    ADD		 r0,r0,r2   
	pld		 [r0,r2]	

    BGT      H03V01_H_row_loop

;;;;;;;; V interpolate ;;;;;;;;;
    
	;ADD		 r0,sp,#12		;&buff[TMP_BUFF_STRIDE*13]
	ADD		 r0,sp,#16		;&buff[TMP_BUFF_STRIDE*13]
	LDR		 r1,[sp]	
	LDR	   r3,[sp,#8]
	pld		 [r0]
	MOV    lr,#8 
	MOV		 r4,#52	
	;STR	   lr,[sp,#-4]
	STR	   lr,[sp,#12]
	
H03V01_V_col_loop
	MOV	     r6,#8
	
H03V01_V_row_loop

	; v0 = ( 52*t2 + 5*(t3<<2 - (t1+t4)) + (t0+t5) + 32 ) >> 6
	
    LDRB     r9,[r0,#16]		;lTemp2
	LDRB     r7,[r0]			;lTemp0
    LDRB     r8,[r0,#8]			;lTemp1
    LDRB     r11,[r0,#32]		;lTemp4
    LDRB     r10,[r0,#24]		;lTemp3
    LDRB	 lr,[r1]
	SMLABB	 r7,r4,r9,r7         
	ADD		 r5,r8,r11
	LDRB     r12,[r0,#40]		;lTemp5						
	pld		 [r0,#TMP_BUFF_STRIDE]
	RSB		 r5,r5,r10,LSL #2
	ADD		 r7,r7,#0x20
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r7
	ADD		 r5,r5,r12
	LDRB     r7,[r0,#48]		;lTemp0
    USAT     r5,#8,r5,ASR #6		
	ADD		 lr,lr,#1
	UHADD16	 r5,r5,lr

	SMLABB	 r8,r4,r10,r8
    STRB     r5,[r1],r3        
	ADD		 r5,r9,r12
    LDRB	 lr,[r1]
	RSB		 r5,r5,r11,LSL #2
	ADD		 r8,r8,r7	
	ADD		 r5,r5,r5,LSL #2        
	ADD		 r5,r5,r8
	ADD		 r5,r5,#0x20
	LDRB     r8,[r0,#56]		;lTemp1
    USAT     r5,#8,r5,ASR #6				
	ADD		 lr,lr,#1
	UHADD16	 r5,r5,lr
	
	SMLABB	 r9,r4,r11,r9
    STRB     r5,[r1],r3        
	ADD		 r5,r10,r7
    LDRB	 lr,[r1]
	RSB		 r5,r5,r12,LSL #2                
	ADD		 r9,r9,r8	
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r9
	ADD		 r5,r5,#0x20
	LDRB     r9,[r0,#64]		;lTemp2
    USAT     r5,#8,r5,ASR #6				
	ADD		 lr,lr,#1
	UHADD16	 r5,r5,lr

	SMLABB	 r10,r4,r12,r10
  STRB     r5,[r1],r3        
	ADD		 r5,r11,r8
  LDRB	 lr,[r1]
	RSB		 r5,r5,r7,LSL #2
	ADD		 r10,r10,r9
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r10
	ADD		 r5,r5,#0x20
	ADD      r0,r0,#32
  USAT     r5,#8,r5,ASR #6
	ADD		 lr,lr,#1
	UHADD16	 r5,r5,lr
	SUBS     r6,r6,#4
  STRB     r5,[r1],r3        

  BGT      H03V01_V_row_loop
    
	;LDR	     lr,[sp,#-4]
	LDR	   lr,[sp,#12]
	SUB		 r0,r0,#63			;-8<<3 +1
	SUB		 r1,r1,r3,LSL #3		
	SUBS   lr,lr,#1
	ADD		 r1,r1,#1
	;STR	   lr,[sp,#-4]
	STR	   lr,[sp,#12]

  BGT      H03V01_V_col_loop    
    
  ;ADD      sp,sp,#TMP_BUFF_STRIDE*13+12
  ADD      sp,sp,#TMP_BUFF_STRIDE*13+16
  LDMFD    sp!,{r4-r11,pc}

	ENDP


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	void  C_Interpolate4Add_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;
;	v filter (1,-5,20,20,-5,1) 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ARMV6_Interpolate4Add_H00V02	PROC
	
    STMFD    sp!,{r4-r11,lr}
	pld		 [r0]
	ADD		 r4,r2,r2,LSL #1	;3*uSrcPitch
    MOV      r5,#8
    ;STR		 r5,[sp,#-4]
    SUB    sp,sp,#0x08
    STR    r5,[sp,#0]
        
H00V02_col_loop
    MOV      r6,#8
    ;STR		 r6,[sp,#-8]
    STR      r6,[sp,#4]
    
H00V02_row_loop

    LDRB     r9,[r0,#0]			;lTemp2
	LDRB     r10,[r0,r2]		;lTemp3
	LDRB     r11,[r0,r2,LSL #1]	;lTemp4
    LDRB     r8,[r0,-r2]		;lTemp1        
    LDRB	 r6,[r1]
	ADD		 r5,r9,r10	
    LDRB     r7,[r0,-r2,LSL #1]	;lTemp0
	RSB		 r5,r8,r5,LSL #2
	LDRB     r12,[r0,r4]		;lTemp5	
	pld		 [r0,r2]
	
	SUB		 r5,r5,r11
	ADD		 r7,r7,#0x10
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r7
	ADD		 r5,r5,r12
	LDRB     r7,[r0,r2,LSL #2]	;lTemp0
    USAT     lr,#8,r5,ASR #5		
	ADD		 r6,r6,#1
	UHADD16	 lr,lr,r6

	ADD		 r5,r10,r11
    STRB     lr,[r1],r3
	ADD		 r8,r8,#0x10
    LDRB	 r6,[r1]
	RSB		 r5,r9,r5,LSL #2        
	SUB		 r5,r5,r12
	ADD		 r8,r8,r7
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r8
	ADD		 r8,r2,r2,LSL #2
	USAT     r5,#8,r5,ASR #5
	ADD		 r6,r6,#1
	UHADD16	 r5,r5,r6
	LDRB     r8,[r0,r8]			;lTemp1
    STRB     r5,[r1],r3

	ADD		 r5,r11,r12
    LDRB	 r6,[r1]
	ADD		 r9,r9,#0x10
	RSB		 r5,r10,r5,LSL #2        
	SUB		 r5,r5,r7
	ADD		 r9,r9,r8 
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r9
	LDRB     r9,[r0,r4,LSL #1]	;lTemp2
	USAT     lr,#8,r5,ASR #5
	ADD		 r6,r6,#1
	UHADD16	 r5,r5,r6

	ADD		 r5,r12,r7
  STRB     lr,[r1],r3
	ADD		 r10,r10,#0x10
  LDRB	 r12,[r1]
	RSB		 r5,r11,r5,LSL #2        
  ;LDR		 r6,[sp,#-8]
  LDR    r6,[sp,#4]
	SUB		 r5,r5,r8
	ADD		 r10,r10,r9
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r10
	ADD      r0,r0,r2,LSL #2
	USAT     r5,#8,r5,ASR #5   
	ADD		 r12,r12,#1
	UHADD16	 r5,r5,r12
  SUBS     r6,r6,#4
  STRB     r5,[r1],r3
  ;STR		 r6,[sp,#-8]
  STR      r6,[sp,#4]

  BGT      H00V02_row_loop
    
  ;LDR		 r5,[sp,#-4]
  LDR    r5,[sp,#0]
	SUB		 r0,r0,r2,LSL #3
	SUB		 r1,r1,r3,LSL #3		
  ADD		 r0,r0,#1
  SUBS     r5,r5,#1
  ADD		 r1,r1,#1 
  ;STR		 r5,[sp,#-4]
  STR    r5,[sp,#0]
    
  BGT      H00V02_col_loop
  
  ADD      sp,sp,#0x08  
  LDMFD    sp!,{r4-r11,pc}
    
	ENDP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	void  C_Interpolate4Add_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;
;	h filter (1,-5,52,20,-5,1) 
;	v filter (1,-5,20,20,-5,1) 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ARMV6_Interpolate4Add_H01V02	PROC
	
    STMFD    sp!,{r4-r11,lr}

    SUB		 r0,r0,r2,LSL #1
	pld		 [r0]
	pld		 [r0,r2]
	MOV		 r4,#52	    	
  ;SUB      sp,sp,#TMP_BUFF_STRIDE*13+12
  SUB    sp,sp,#TMP_BUFF_STRIDE*13+16
	MOV    lr,#8+5
	STR		 r1,[sp,#0]	
	STR		 r2,[sp,#4]	
	STR	   r3,[sp,#8]   
	;ADD		 r1,sp,#12		;&buff[TMP_BUFF_STRIDE*13]
	ADD		 r1,sp,#16		;&buff[TMP_BUFF_STRIDE*13]
	
;;;;;;;; H interpolate ;;;;;;;;;
		
H01V02_H_row_loop
	MOV	     r6,#8
	
H01V02_H_col_loop

	; t0 = ( 52*p2 + 5*(p3<<2 - (p1+p4)) + (p0+p5) + 32 ) >> 6
	
    LDRB     r9,[r0,#0]		;lTemp2
	LDRB     r7,[r0,#-2]	;lTemp0
    LDRB     r8,[r0,#-1]	;lTemp1
    LDRB     r11,[r0,#2]	;lTemp4
    LDRB     r10,[r0,#1]	;lTemp3
	SMLABB	 r7,r4,r9,r7     
	ADD		 r5,r8,r11
	LDRB     r12,[r0,#3]	;lTemp5						
	RSB		 r5,r5,r10,LSL #2
	ADD		 r7,r7,#0x20
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r7
	ADD		 r5,r5,r12
	LDRB     r7,[r0,#4]		;lTemp0
    USAT     r3,#8,r5,ASR #6		

	SMLABB	 r8,r4,r10,r8
	ADD		 r5,r9,r12
	RSB		 r5,r5,r11,LSL #2
	ADD		 r8,r8,r7	
	ADD		 r5,r5,r5,LSL #2        
	ADD		 r5,r5,r8
	ADD		 r5,r5,#0x20
	LDRB     r8,[r0,#5]		;lTemp1
    USAT     r2,#8,r5,ASR #6				
	
	SMLABB	 r9,r4,r11,r9
	ADD		 r5,r10,r7
	ORR		 r3,r3,r2,LSL #8
	RSB		 r5,r5,r12,LSL #2                
	ADD		 r9,r9,r8	
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r9
	ADD		 r5,r5,#0x20
	LDRB     r9,[r0,#6]		;lTemp2
    USAT     r2,#8,r5,ASR #6				

	SMLABB	 r10,r4,r12,r10
	ADD		 r5,r11,r8
	ORR		 r3,r3,r2,LSL #16
	RSB		 r5,r5,r7,LSL #2
	ADD		 r10,r10,r9
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r10
	ADD		 r5,r5,#0x20
	ADD      r0,r0,#4
    USAT     r2,#8,r5,ASR #6
	SUBS     r6,r6,#4
	ORR		 r3,r3,r2,LSL #24
    STR      r3,[r1],#4

    BGT      H01V02_H_col_loop
    
	LDR	     r2,[sp,#4]
	SUB		 r0,r0,#8
	SUBS     lr,lr,#1
    ADD		 r0,r0,r2         
	pld		 [r0,r2]

    BGT      H01V02_H_row_loop
    
;;;;;;;; V interpolate ;;;;;;;;;

	;ADD		 r0,sp,#12		;&buff[TMP_BUFF_STRIDE*13]
	ADD		 r0,sp,#16		;&buff[TMP_BUFF_STRIDE*13]
	LDR		 r1,[sp]	
	LDR	     r3,[sp,#8]
	pld		 [r0]
  MOV      r5,#8
  STR		 r5,[sp]
        
H01V02_V_col_loop
  MOV      r6,#8
  ;STR		 r6,[sp,#-4]
  STR		 r6,[sp,#12]
    
H01V02_V_row_loop

    LDRB     r9,[r0,#16]		;lTemp2
	LDRB     r10,[r0,#24]		;lTemp3
	LDRB     r11,[r0,#32]		;lTemp4
    LDRB     r8,[r0,#8]			;lTemp1        
    LDRB	 r6,[r1]
	ADD		 r5,r9,r10	
    LDRB     r7,[r0]			;lTemp0
	RSB		 r5,r8,r5,LSL #2
	LDRB     r12,[r0,#40]		;lTemp5	
	
	SUB		 r5,r5,r11
	ADD		 r7,r7,#0x10
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r7
	ADD		 r5,r5,r12
	LDRB     r7,[r0,#48]		;lTemp0
    USAT     lr,#8,r5,ASR #5		
	ADD		 r6,r6,#1
	UHADD16	 lr,lr,r6

	ADD		 r5,r10,r11
    STRB     lr,[r1],r3
	ADD		 r8,r8,#0x10
    LDRB	 r6,[r1]
	RSB		 r5,r9,r5,LSL #2        
	SUB		 r5,r5,r12
	ADD		 r8,r8,r7
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r8
	USAT     r5,#8,r5,ASR #5
	ADD		 r6,r6,#1
	UHADD16	 r5,r5,r6
	LDRB     r8,[r0,#56]		;lTemp1
    STRB     r5,[r1],r3

	ADD		 r5,r11,r12
    LDRB	 r6,[r1]
	ADD		 r9,r9,#0x10
	RSB		 r5,r10,r5,LSL #2        
	SUB		 r5,r5,r7
	ADD		 r9,r9,r8 
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r9
	LDRB     r9,[r0,#64]		;lTemp2
	USAT     lr,#8,r5,ASR #5
	ADD		 r6,r6,#1
	UHADD16	 lr,lr,r6

	ADD		 r5,r12,r7
  STRB     lr,[r1],r3
	ADD		 r10,r10,#0x10
  LDRB	 r12,[r1]
  ;LDR		 r6,[sp,#-4]
  LDR		 r6,[sp,#12]
	RSB		 r5,r11,r5,LSL #2        
	SUB		 r5,r5,r8
	ADD		 r10,r10,r9
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r10
	ADD      r0,r0,#32
	USAT     r5,#8,r5,ASR #5   
	ADD		 r12,r12,#1
	UHADD16	 r5,r5,r12
  SUBS     r6,r6,#4
  STRB     r5,[r1],r3
  ;STR		 r6,[sp,#-4]
  STR		   r6,[sp,#12]

    BGT      H01V02_V_row_loop
    
    LDR		 r5,[sp]
	SUB		 r1,r1,r3,LSL #3		
	SUB		 r0,r0,#63
    ADD		 r1,r1,#1 
    SUBS     r5,r5,#1
    STR		 r5,[sp]
    
    BGT      H01V02_V_col_loop
    
    ;ADD      sp,sp,#TMP_BUFF_STRIDE*13+12
    ADD      sp,sp,#TMP_BUFF_STRIDE*13+16
    LDMFD    sp!,{r4-r11,pc}
    
	ENDP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	void  C_Interpolate4Add_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;
;	h filter (1,-5,20,20,-5,1) 
;	v filter (1,-5,20,20,-5,1) 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ARMV6_Interpolate4Add_H02V02	PROC

  STMFD    sp!,{r4-r11,lr}
  SUB		 r0,r0,r2,LSL #1
	pld		 [r0]
	pld		 [r0,r2]
  ;SUB      sp,sp,#TMP_BUFF_STRIDE*13+12
  SUB      sp,sp,#TMP_BUFF_STRIDE*13+16
	MOV      lr,#8+5
	STR		 r1,[sp,#0]	
	STR		 r2,[sp,#4]	
	STR	     r3,[sp,#8]   
	;ADD		 r1,sp,#12		;&buff[TMP_BUFF_STRIDE*13]
	ADD		 r1,sp,#16		;&buff[TMP_BUFF_STRIDE*13]

;;;;;;;; H interpolate ;;;;;;;;;
		
H02V02_H_row_loop
	MOV	     r6,#8
	
H02V02_H_col_loop
	
    LDRB     r9,[r0,#0]		;lTemp2
	LDRB     r10,[r0,#1]	;lTemp3
	LDRB     r11,[r0,#2]	;lTemp4
    LDRB     r8,[r0,#-1]	;lTemp1        
	ADD		 r5,r9,r10	
    LDRB     r7,[r0,#-2]	;lTemp0
	RSB		 r5,r8,r5,LSL #2
	LDRB     r12,[r0,#3]	;lTemp5	
	
	SUB		 r5,r5,r11
	ADD		 r7,r7,#0x10
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r7
	ADD		 r5,r5,r12
	LDRB     r7,[r0,#4]		;lTemp0
    USAT     r4,#8,r5,ASR #5		

	ADD		 r5,r10,r11
	ADD		 r8,r8,#0x10
	RSB		 r5,r9,r5,LSL #2        
	SUB		 r5,r5,r12
	ADD		 r8,r8,r7
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r8
	LDRB     r8,[r0,#5]		;lTemp1
	USAT     r2,#8,r5,ASR #5
			
	ADD		 r5,r11,r12
	ADD		 r9,r9,#0x10
	ORR		 r4,r4,r2,LSL #8
	RSB		 r5,r10,r5,LSL #2        
	SUB		 r5,r5,r7
	ADD		 r9,r9,r8 
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r9
	LDRB     r9,[r0,#6]		;lTemp2
	USAT     r2,#8,r5,ASR #5	
	
	ADD		 r5,r12,r7
	ADD		 r10,r10,#0x10
	ORR		 r4,r4,r2,LSL #16
	RSB		 r5,r11,r5,LSL #2        
	SUB		 r5,r5,r8
	ADD		 r10,r10,r9
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r10
	ADD      r0,r0,#4
	USAT     r2,#8,r5,ASR #5
	SUBS     r6,r6,#4
	ORR		 r4,r4,r2,LSL #24   
    STR      r4,[r1],#4

    BGT      H02V02_H_col_loop
    
    LDR	     r2,[sp,#4]
	SUB		 r0,r0,#8
    SUBS     lr,lr,#1
    ADD		 r0,r0,r2   
	pld		 [r0,r2]
    BGT      H02V02_H_row_loop
    
;;;;;;;; V interpolate ;;;;;;;;;

	;ADD		 r0,sp,#12		;&buff[TMP_BUFF_STRIDE*13]
	ADD		 r0,sp,#16		;&buff[TMP_BUFF_STRIDE*13]
	LDR		 r1,[sp]	
	LDR	   r3,[sp,#8]
	pld		 [r0]
  MOV    r5,#8
  STR		 r5,[sp]
        
H02V02_V_col_loop
    MOV      r6,#8
    ;STR		 r6,[sp,#-4]
    STR		 r6,[sp,#12]
    
H02V02_V_row_loop

    LDRB     r9,[r0,#16]		;lTemp2
	LDRB     r10,[r0,#24]		;lTemp3
	LDRB     r11,[r0,#32]		;lTemp4
    LDRB     r8,[r0,#8]			;lTemp1        
	LDRB	 r6,[r1]	
	ADD		 r5,r9,r10	
    LDRB     r7,[r0]			;lTemp0
	RSB		 r5,r8,r5,LSL #2
	LDRB     r12,[r0,#40]		;lTemp5	
	
	SUB		 r5,r5,r11
	ADD		 r7,r7,#0x10
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r7
	ADD		 r5,r5,r12
	LDRB     r7,[r0,#48]		;lTemp0
    USAT     lr,#8,r5,ASR #5		
	ADD		 r6,r6,#1
	UHADD16	 lr,lr,r6

	ADD		 r5,r10,r11
    STRB     lr,[r1],r3
	ADD		 r8,r8,#0x10
	LDRB	 r6,[r1]	
	RSB		 r5,r9,r5,LSL #2        
	SUB		 r5,r5,r12
	ADD		 r8,r8,r7
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r8
	USAT     r5,#8,r5,ASR #5
	ADD		 r6,r6,#1
	UHADD16	 r5,r5,r6
	LDRB     r8,[r0,#56]		;lTemp1
    STRB     r5,[r1],r3

	ADD		 r5,r11,r12
	LDRB	 r6,[r1]	
	ADD		 r9,r9,#0x10
	RSB		 r5,r10,r5,LSL #2        
	SUB		 r5,r5,r7
	ADD		 r9,r9,r8 
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r9
	LDRB     r9,[r0,#64]		;lTemp2
	USAT     lr,#8,r5,ASR #5
	ADD		 r6,r6,#1
	UHADD16	 lr,lr,r6

	ADD		 r5,r12,r7
  STRB     lr,[r1],r3
	ADD		 r10,r10,#0x10
	LDRB		 r12,[r1]	
  ;LDR		 r6,[sp,#-4]
  LDR		 r6,[sp,#12]
	RSB		 r5,r11,r5,LSL #2        
	SUB		 r5,r5,r8
	ADD		 r10,r10,r9
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r10
	ADD      r0,r0,#32
	USAT     r5,#8,r5,ASR #5   
	ADD		 r12,r12,#1
	UHADD16	 r5,r5,r12
    SUBS     r6,r6,#4
    STRB     r5,[r1],r3
    ;STR		 r6,[sp,#-4]
    STR		 r6,[sp,#12]

    BGT      H02V02_V_row_loop
    
    LDR		 r5,[sp]
	SUB		 r1,r1,r3,LSL #3		
	SUB		 r0,r0,#63
    ADD		 r1,r1,#1 
    SUBS     r5,r5,#1
    STR		 r5,[sp]
    
    BGT      H02V02_V_col_loop
    
    ;ADD      sp,sp,#TMP_BUFF_STRIDE*13+12
    ADD      sp,sp,#TMP_BUFF_STRIDE*13+16
    LDMFD    sp!,{r4-r11,pc}
    
	ENDP


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	void  C_Interpolate4Add_H03V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;
;	h filter (1,-5,20,52,-5,1) 
;	v filter (1,-5,20,20,-5,1) 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ARMV6_Interpolate4Add_H03V02	PROC

    STMFD    sp!,{r4-r11,lr}
    SUB		 r0,r0,r2,LSL #1
	pld		 [r0]
	pld		 [r0,r2]
	MOV		 r4,#52	    	
  ;SUB      sp,sp,#TMP_BUFF_STRIDE*13+12
  SUB      sp,sp,#TMP_BUFF_STRIDE*13+16
	MOV      lr,#8+5
	STR		 r1,[sp,#0]	
	STR		 r2,[sp,#4]	
	STR	     r3,[sp,#8]   
	;ADD		 r1,sp,#12		;&buff[TMP_BUFF_STRIDE*13]
	ADD		 r1,sp,#16		;&buff[TMP_BUFF_STRIDE*13]

;;;;;;;; H interpolate ;;;;;;;;;
		
H03V02_H_row_loop
	MOV	     r6,#8
	
H03V02_H_col_loop
	
	LDRB     r7,[r0,#-2]	;lTemp0
    LDRB     r10,[r0,#1]	;lTemp3
	LDRB     r8,[r0,#-1]	;lTemp1     
	LDRB     r11,[r0,#2]	;lTemp4 
	LDRB     r9,[r0,#0]		;lTemp2       
	SMLABB	 r7,r4,r10,r7			 
	ADD		 r5,r8,r11
	LDRB     r12,[r0,#3]	;lTemp5						
	
	RSB		 r5,r5,r9,LSL #2
	ADD		 r7,r7,#0x20
	ADD		 r5,r5,r5,LSL #2		
	ADD		 r5,r5,r7
	ADD		 r5,r5,r12	
	LDRB     r7,[r0,#4]	;lTemp0
	USAT     r2,#8,r5,ASR #6		

	SMLABB	 r8,r4,r11,r8
	ADD		 r5,r9,r12
	RSB		 r5,r5,r10,LSL #2        
	ADD		 r8,r8,r7	
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r8
	ADD		 r5,r5,#0x20
	LDRB     r8,[r0,#5]		;lTemp1
	USAT     r3,#8,r5,ASR #6				
	
	SMLABB	 r9,r4,r12,r9	
	ADD		 r5,r10,r7
	ORR		 r2,r2,r3,LSL #8
	RSB		 r5,r5,r11,LSL #2
	ADD		 r9,r9,r8
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r9
	ADD		 r5,r5,#0x20
	LDRB     r9,[r0,#6]		;lTemp2
	USAT     r3,#8,r5,ASR #6				

	SMLABB	 r10,r4,r7,r10	
	ADD		 r5,r11,r8
	ORR		 r2,r2,r3,LSL #16
	RSB		 r5,r5,r12,LSL #2        
	ADD		 r10,r10,r9
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r10
	ADD		 r5,r5,#0x20
	ADD      r0,r0,#4
	USAT     r3,#8,r5,ASR #6
	SUBS     r6,r6,#4
	ORR		 r2,r2,r3,LSL #24        
    STR      r2,[r1],#4

    BGT      H03V02_H_col_loop
    
	LDR	     r2,[sp,#4]
	SUB		 r0,r0,#8
	SUBS     lr,lr,#1
    ADD		 r0,r0,r2   
	pld		 [r0,r2]	

    BGT      H03V02_H_row_loop

    
;;;;;;;; V interpolate ;;;;;;;;;

	;ADD		 r0,sp,#12		;&buff[TMP_BUFF_STRIDE*13]
	ADD		 r0,sp,#16		;&buff[TMP_BUFF_STRIDE*13]
	LDR		 r1,[sp]	
	LDR	     r3,[sp,#8]
	pld		 [r0]
    MOV      r5,#8
    STR		 r5,[sp]
        
H03V02_V_col_loop
    MOV      r6,#8
    ;STR		 r6,[sp,#-4]
    STR		 r6,[sp,#12]
    
H03V02_V_row_loop

    LDRB     r9,[r0,#16]		;lTemp2
	LDRB     r10,[r0,#24]		;lTemp3
	LDRB     r11,[r0,#32]		;lTemp4
    LDRB     r8,[r0,#8]			;lTemp1        
	LDRB	 r6,[r1]	
	ADD		 r5,r9,r10	
    LDRB     r7,[r0]			;lTemp0
	RSB		 r5,r8,r5,LSL #2
	LDRB     r12,[r0,#40]		;lTemp5	
	
	SUB		 r5,r5,r11
	ADD		 r7,r7,#0x10
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r7
	ADD		 r5,r5,r12
	LDRB     r7,[r0,#48]		;lTemp0
    USAT     lr,#8,r5,ASR #5		
	ADD		 r6,r6,#1
	UHADD16	 lr,lr,r6

	ADD		 r5,r10,r11
    STRB     lr,[r1],r3
	ADD		 r8,r8,#0x10
	LDRB	 r6,[r1]	
	RSB		 r5,r9,r5,LSL #2        
	SUB		 r5,r5,r12
	ADD		 r8,r8,r7
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r8
	USAT     r5,#8,r5,ASR #5
	ADD		 r6,r6,#1
	UHADD16	 r5,r5,r6
	LDRB     r8,[r0,#56]		;lTemp1
    STRB     r5,[r1],r3

	ADD		 r5,r11,r12
	LDRB	 r6,[r1]	
	ADD		 r9,r9,#0x10
	RSB		 r5,r10,r5,LSL #2        
	SUB		 r5,r5,r7
	ADD		 r9,r9,r8 
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r9
	LDRB     r9,[r0,#64]		;lTemp2
	USAT     lr,#8,r5,ASR #5
	ADD		 r6,r6,#1
	UHADD16	 lr,lr,r6

	ADD		 r5,r12,r7
    STRB     lr,[r1],r3
	ADD		 r10,r10,#0x10
	LDRB		 r12,[r1]	
  ;LDR		 r6,[sp,#-4]
  LDR		 r6,[sp,#12]
	RSB		 r5,r11,r5,LSL #2        
	SUB		 r5,r5,r8
	ADD		 r10,r10,r9
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r10
	ADD      r0,r0,#32
	USAT     r5,#8,r5,ASR #5   
	ADD		 r12,r12,#1
	UHADD16	 r5,r5,r12
    SUBS     r6,r6,#4
    STRB     r5,[r1],r3
    ;STR		 r6,[sp,#-4]
    STR		 r6,[sp,#12]

    BGT      H03V02_V_row_loop
    
    LDR		 r5,[sp]
	SUB		 r1,r1,r3,LSL #3		
	SUB		 r0,r0,#63
    ADD		 r1,r1,#1 
    SUBS     r5,r5,#1
    STR		 r5,[sp]
    
    BGT      H03V02_V_col_loop
    
    ;ADD      sp,sp,#TMP_BUFF_STRIDE*13+12
    ADD      sp,sp,#TMP_BUFF_STRIDE*13+16
    LDMFD    sp!,{r4-r11,pc}

	ENDP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	void  C_Interpolate4Add_H00V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;
;	v filter (1,-5,20,52,-5,1) 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ARMV6_Interpolate4Add_H00V03	PROC

    STMFD    sp!,{r4-r11,lr}
	pld		 [r0]
	pld		 [r0,r2]
  MOV    r5,#8	
	MOV		 r4,#52
  ;STR	   r5,[sp,#-4]
  SUB    sp,sp,#0x08
  STR    r5,[sp,#0]
	ADD		 lr,r2,r2,LSL #1	;3*uSrcPitch
    	
H00V03_col_loop
  MOV	     r6,#8
  ;STR	     r6,[sp,#-8]
  STR      r6,[sp,#4]
    
H00V03_row_loop

	LDRB     r7,[r0,-r2,LSL #1]	;lTemp0
    LDRB     r10,[r0,r2]		;lTemp3
	LDRB     r8,[r0,-r2]		;lTemp1     
	LDRB     r11,[r0,r2,LSL #1]	;lTemp4  
	LDRB     r9,[r0]			;lTemp2      
	LDRB	 r6,[r1]	
	SMLABB	 r7,r4,r10,r7		 
	ADD		 r5,r8,r11
	LDRB     r12,[r0,lr]		;lTemp5						
	
	RSB		 r5,r5,r9,LSL #2
	ADD		 r7,r7,#0x20
	ADD		 r5,r5,r5,LSL #2		
	ADD		 r5,r5,r7
	ADD		 r5,r5,r12	
	LDRB     r7,[r0,r2,LSL #2]	;lTemp0
	USAT     r5,#8,r5,ASR #6		
	ADD		 r6,r6,#1
	UHADD16	 r5,r5,r6

	SMLABB	 r8,r4,r11,r8
	STRB	 r5,[r1],r3
	ADD		 r5,r9,r12
	LDRB	 r6,[r1]	
	RSB		 r5,r5,r10,LSL #2        
	ADD		 r8,r8,r7	
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r8
	ADD		 r8,lr,r2,LSL #1
	ADD		 r5,r5,#0x20
	LDRB     r8,[r0,r8]		;lTemp1
	USAT     r5,#8,r5,ASR #6				
	ADD		 r6,r6,#1
	UHADD16	 r5,r5,r6
	
	SMLABB	 r9,r4,r12,r9
	STRB	 r5,[r1],r3
	ADD		 r5,r10,r7
	LDRB	 r6,[r1]	
	RSB		 r5,r5,r11,LSL #2
	ADD		 r9,r9,r8
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r9
	ADD		 r5,r5,#0x20
	LDRB     r9,[r0,lr,LSL #1]		;lTemp2
	USAT     r5,#8,r5,ASR #6				
	ADD		 r6,r6,#1
	UHADD16	 r5,r5,r6

	SMLABB	 r10,r4,r7,r10
	STRB	 r5,[r1],r3
	ADD		 r5,r11,r8
	LDRB	 r7,[r1]	
  ;LDR	     r6,[sp,#-8]
  LDR    r6,[sp,#4]
	RSB		 r5,r5,r12,LSL #2        
	ADD		 r10,r10,r9
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r10
	ADD		 r5,r5,#0x20
	ADD      r0,r0,r2,LSL #2
	USAT     r5,#8,r5,ASR #6
	ADD		 r7,r7,#1
	UHADD16	 r5,r5,r7
	SUBS     r6,r6,#4
	STRB	 r5,[r1],r3
    ;STR	     r6,[sp,#-8]
    STR      r6,[sp,#4]

    BGT      H00V03_row_loop
    
	;LDR	     r5,[sp,#-4]
	LDR    r5,[sp,#0]	
	SUB		 r1,r1,r3,LSL #3
	SUB		 r0,r0,r2,LSL #3
	ADD		 r1,r1,#1
	SUBS     r5,r5,#1
    ADD		 r0,r0,#1        
	pld		 [r0,r2]
	;STR	     r5,[sp,#-4]
	STR    r5,[sp,#0]	

    BGT      H00V03_col_loop
    
    ADD     sp,sp,#0x08    
    LDMFD    sp!,{r4-r11,pc}

    ENDP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	void  C_Interpolate4Add_H01V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;
;	h filter (1,-5,52,20,-5,1) 
;	v filter (1,-5,20,52,-5,1) 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ARMV6_Interpolate4Add_H01V03	PROC
	
    STMFD    sp!,{r4-r11,lr}

    SUB		 r0,r0,r2,LSL #1
	pld		 [r0]
	pld		 [r0,r2]
	MOV		 r4,#52	    	
  ;SUB      sp,sp,#TMP_BUFF_STRIDE*13+12
  SUB      sp,sp,#TMP_BUFF_STRIDE*13+16
	MOV      lr,#8+5
	STR		 r1,[sp,#0]	
	STR		 r2,[sp,#4]	
	STR	     r3,[sp,#8]   
	;ADD		 r1,sp,#12		;&buff[TMP_BUFF_STRIDE*13]
	ADD		 r1,sp,#16		;&buff[TMP_BUFF_STRIDE*13]
	
;;;;;;;; H interpolate ;;;;;;;;;
		
H01V03_H_row_loop
	MOV	     r6,#8
	
H01V03_H_col_loop

	; t0 = ( 52*p2 + 5*(p3<<2 - (p1+p4)) + (p0+p5) + 32 ) >> 6
	
    LDRB     r9,[r0,#0]		;lTemp2
	LDRB     r7,[r0,#-2]	;lTemp0
    LDRB     r8,[r0,#-1]	;lTemp1
    LDRB     r11,[r0,#2]	;lTemp4
    LDRB     r10,[r0,#1]	;lTemp3
	SMLABB	 r7,r4,r9,r7     
	ADD		 r5,r8,r11
	LDRB     r12,[r0,#3]	;lTemp5						
	RSB		 r5,r5,r10,LSL #2
	ADD		 r7,r7,#0x20
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r7
	ADD		 r5,r5,r12
	LDRB     r7,[r0,#4]		;lTemp0
    USAT     r3,#8,r5,ASR #6		

	SMLABB	 r8,r4,r10,r8
	ADD		 r5,r9,r12
	RSB		 r5,r5,r11,LSL #2
	ADD		 r8,r8,r7	
	ADD		 r5,r5,r5,LSL #2        
	ADD		 r5,r5,r8
	ADD		 r5,r5,#0x20
	LDRB     r8,[r0,#5]		;lTemp1
    USAT     r2,#8,r5,ASR #6				
	
	SMLABB	 r9,r4,r11,r9
	ADD		 r5,r10,r7
	ORR		 r3,r3,r2,LSL #8
	RSB		 r5,r5,r12,LSL #2                
	ADD		 r9,r9,r8	
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r9
	ADD		 r5,r5,#0x20
	LDRB     r9,[r0,#6]		;lTemp2
    USAT     r2,#8,r5,ASR #6				

	SMLABB	 r10,r4,r12,r10
	ADD		 r5,r11,r8
	ORR		 r3,r3,r2,LSL #16
	RSB		 r5,r5,r7,LSL #2
	ADD		 r10,r10,r9
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r10
	ADD		 r5,r5,#0x20
	ADD      r0,r0,#4
    USAT     r2,#8,r5,ASR #6
	SUBS     r6,r6,#4
	ORR		 r3,r3,r2,LSL #24
    STR      r3,[r1],#4

    BGT      H01V03_H_col_loop
    
	LDR	     r2,[sp,#4]
	SUB		 r0,r0,#8
	SUBS     lr,lr,#1
    ADD		 r0,r0,r2         
	pld		 [r0,r2]

    BGT      H01V03_H_row_loop
    
;;;;;;;; V interpolate ;;;;;;;;;

	;ADD		 r0,sp,#12		;&buff[TMP_BUFF_STRIDE*13]
	ADD		 r0,sp,#16		;&buff[TMP_BUFF_STRIDE*13]
	LDR		 r1,[sp]	
	LDR	     r3,[sp,#8]
	pld		 [r0]
  MOV      r5,#8
	MOV		 r4,#52
  STR		 r5,[sp]
    	
H01V03_col_loop
    MOV	     r6,#8
    ;STR		 r6,[sp,#-4]
    STR		 r6,[sp,#12]
    
H01V03_row_loop

	LDRB     r7,[r0]			;lTemp0
    LDRB     r10,[r0,#24]		;lTemp3
	LDRB     r8,[r0,#8]			;lTemp1     
	LDRB     r11,[r0,#32]		;lTemp4  
	LDRB     r9,[r0,#16]		;lTemp2      
	LDRB	 r6,[r1]	
	SMLABB	 r7,r4,r10,r7		 
	ADD		 r5,r8,r11
	LDRB     r12,[r0,#40]		;lTemp5						
	
	RSB		 r5,r5,r9,LSL #2
	ADD		 r7,r7,#0x20
	ADD		 r5,r5,r5,LSL #2		
	ADD		 r5,r5,r7
	ADD		 r5,r5,r12	
	LDRB     r7,[r0,#48]		;lTemp0
	USAT     r5,#8,r5,ASR #6		
	ADD		 r6,r6,#1
	UHADD16	 r5,r5,r6

	SMLABB	 r8,r4,r11,r8
	STRB	 r5,[r1],r3
	LDRB	 r6,[r1]	
	ADD		 r5,r9,r12
	RSB		 r5,r5,r10,LSL #2        
	ADD		 r8,r8,r7	
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r8
	ADD		 r5,r5,#0x20
	LDRB     r8,[r0,#56]		;lTemp1
	USAT     r5,#8,r5,ASR #6				
	ADD		 r6,r6,#1
	UHADD16	 r5,r5,r6
	
	SMLABB	 r9,r4,r12,r9
	STRB	 r5,[r1],r3
	ADD		 r5,r10,r7
	LDRB	 r6,[r1]	
	RSB		 r5,r5,r11,LSL #2
	ADD		 r9,r9,r8
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r9
	ADD		 r5,r5,#0x20
	LDRB     r9,[r0,#64]		;lTemp2
	USAT     r5,#8,r5,ASR #6				
	ADD		 r6,r6,#1
	UHADD16	 r5,r5,r6

	SMLABB	 r10,r4,r7,r10
	STRB	 r5,[r1],r3
	ADD		 r5,r11,r8
	LDRB	 r7,[r1]	
	RSB		 r5,r5,r12,LSL #2        
  ;LDR		 r6,[sp,#-4]
  LDR		 r6,[sp,#12]
	ADD		 r10,r10,r9
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r10
	ADD		 r5,r5,#0x20
	ADD      r0,r0,#32
	USAT     r5,#8,r5,ASR #6
	ADD		 r7,r7,#1
	UHADD16	 r5,r5,r7
	SUBS     r6,r6,#4
	STRB	 r5,[r1],r3
  ;STR		 r6,[sp,#-4]
  STR		 r6,[sp,#12]

  BGT      H01V03_row_loop
    
	LDR	    r5,[sp]	
	SUB		 r1,r1,r3,LSL #3
	SUB		 r0,r0,#63
	ADD		 r1,r1,#1
	SUBS     r5,r5,#1
	STR	     r5,[sp]	

    BGT      H01V03_col_loop
    
    ;ADD      sp,sp,#TMP_BUFF_STRIDE*13+12
    ADD      sp,sp,#TMP_BUFF_STRIDE*13+16
    LDMFD    sp!,{r4-r11,pc}

	ENDP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	void  C_Interpolate4Add_H02V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;
;	h filter (1,-5,20,20,-5,1) 
;	v filter (1,-5,20,52,-5,1) 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ARMV6_Interpolate4Add_H02V03	PROC
	
  STMFD    sp!,{r4-r11,lr}
  SUB		 r0,r0,r2,LSL #1
	pld		 [r0]
	pld		 [r0,r2]
  ;SUB      sp,sp,#TMP_BUFF_STRIDE*13+12
  SUB      sp,sp,#TMP_BUFF_STRIDE*13+16
	MOV      lr,#8+5
	STR		 r1,[sp,#0]	
	STR		 r2,[sp,#4]	
	STR	     r3,[sp,#8]   
	;ADD		 r1,sp,#12		;&buff[TMP_BUFF_STRIDE*13]
	ADD		 r1,sp,#16		;&buff[TMP_BUFF_STRIDE*13

;;;;;;;; H interpolate ;;;;;;;;;
		
H02V03_H_row_loop
	MOV	     r6,#8
	
H02V03_H_col_loop
	
    LDRB     r9,[r0,#0]		;lTemp2
	LDRB     r10,[r0,#1]	;lTemp3
	LDRB     r11,[r0,#2]	;lTemp4
    LDRB     r8,[r0,#-1]	;lTemp1        
	ADD		 r5,r9,r10	
    LDRB     r7,[r0,#-2]	;lTemp0
	RSB		 r5,r8,r5,LSL #2
	LDRB     r12,[r0,#3]	;lTemp5	
	
	SUB		 r5,r5,r11
	ADD		 r7,r7,#0x10
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r7
	ADD		 r5,r5,r12
	LDRB     r7,[r0,#4]		;lTemp0
    USAT     r4,#8,r5,ASR #5		

	ADD		 r5,r10,r11
	ADD		 r8,r8,#0x10
	RSB		 r5,r9,r5,LSL #2        
	SUB		 r5,r5,r12
	ADD		 r8,r8,r7
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r8
	LDRB     r8,[r0,#5]		;lTemp1
	USAT     r2,#8,r5,ASR #5
			
	ADD		 r5,r11,r12
	ADD		 r9,r9,#0x10
	ORR		 r4,r4,r2,LSL #8
	RSB		 r5,r10,r5,LSL #2        
	SUB		 r5,r5,r7
	ADD		 r9,r9,r8 
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r9
	LDRB     r9,[r0,#6]		;lTemp2
	USAT     r2,#8,r5,ASR #5	
	
	ADD		 r5,r12,r7
	ADD		 r10,r10,#0x10
	ORR		 r4,r4,r2,LSL #16
	RSB		 r5,r11,r5,LSL #2        
	SUB		 r5,r5,r8
	ADD		 r10,r10,r9
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r10
	ADD      r0,r0,#4
	USAT     r2,#8,r5,ASR #5
	SUBS     r6,r6,#4
	ORR		 r4,r4,r2,LSL #24   
    STR      r4,[r1],#4

    BGT      H02V03_H_col_loop
    
    LDR	     r2,[sp,#4]
	SUB		 r0,r0,#8
    SUBS     lr,lr,#1
    ADD		 r0,r0,r2   
	pld		 [r0,r2]
    BGT      H02V03_H_row_loop

    
;;;;;;;; V interpolate ;;;;;;;;;

	;ADD		 r0,sp,#12		;&buff[TMP_BUFF_STRIDE*13]
	ADD		 r0,sp,#16		;&buff[TMP_BUFF_STRIDE*13]
	LDR		 r1,[sp]	
	LDR	     r3,[sp,#8]
	pld		 [r0]
  MOV      r5,#8
	MOV		 r4,#52
  STR		 r5,[sp]
    	
H02V03_col_loop
    MOV	     r6,#8
    ;STR		 r6,[sp,#-4]
    STR		 r6,[sp,#12]
    
H02V03_row_loop

	LDRB     r7,[r0]			;lTemp0
    LDRB     r10,[r0,#24]		;lTemp3
	LDRB     r8,[r0,#8]			;lTemp1     
	LDRB     r11,[r0,#32]		;lTemp4  
	LDRB     r9,[r0,#16]		;lTemp2      
	LDRB	 r6,[r1]	
	SMLABB	 r7,r4,r10,r7		 
	ADD		 r5,r8,r11
	LDRB     r12,[r0,#40]		;lTemp5						
	
	RSB		 r5,r5,r9,LSL #2
	ADD		 r7,r7,#0x20
	ADD		 r5,r5,r5,LSL #2		
	ADD		 r5,r5,r7
	ADD		 r5,r5,r12	
	LDRB     r7,[r0,#48]		;lTemp0
	USAT     r5,#8,r5,ASR #6		
	ADD		 r6,r6,#1
	UHADD16	 r5,r5,r6

	SMLABB	 r8,r4,r11,r8
	STRB	 r5,[r1],r3
	ADD		 r5,r9,r12
	LDRB	 r6,[r1]	
	RSB		 r5,r5,r10,LSL #2        
	ADD		 r8,r8,r7	
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r8
	ADD		 r5,r5,#0x20
	LDRB     r8,[r0,#56]		;lTemp1
	USAT     r5,#8,r5,ASR #6				
	ADD		 r6,r6,#1
	UHADD16	 r5,r5,r6
	
	SMLABB	 r9,r4,r12,r9
	STRB	 r5,[r1],r3
	ADD		 r5,r10,r7
	LDRB	 r6,[r1]	
	RSB		 r5,r5,r11,LSL #2
	ADD		 r9,r9,r8
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r9
	ADD		 r5,r5,#0x20
	LDRB     r9,[r0,#64]		;lTemp2
	USAT     r5,#8,r5,ASR #6				
	ADD		 r6,r6,#1
	UHADD16	 r5,r5,r6

	SMLABB	 r10,r4,r7,r10
	STRB	 r5,[r1],r3
	ADD		 r5,r11,r8
	LDRB	 r7,[r1]	
	RSB		 r5,r5,r12,LSL #2        
  ;LDR		 r6,[sp,#-4]
  LDR		 r6,[sp,#12]
	ADD		 r10,r10,r9
	ADD		 r5,r5,r5,LSL #2
	ADD		 r5,r5,r10
	ADD		 r5,r5,#0x20
	ADD      r0,r0,#32
	USAT     r5,#8,r5,ASR #6
	ADD		 r7,r7,#1
	UHADD16	 r5,r5,r7
	SUBS     r6,r6,#4
	STRB	 r5,[r1],r3
  ;STR		 r6,[sp,#-4]
  STR		 r6,[sp,#12]

  BGT      H02V03_row_loop
    
	LDR	     r5,[sp]	
	SUB		 r1,r1,r3,LSL #3
	SUB		 r0,r0,#63
	ADD		 r1,r1,#1
	SUBS     r5,r5,#1
	STR	     r5,[sp]	

    BGT      H02V03_col_loop
    
    ;ADD      sp,sp,#TMP_BUFF_STRIDE*13+12
    ADD      sp,sp,#TMP_BUFF_STRIDE*13+16
    LDMFD    sp!,{r4-r11,pc}

	ENDP


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	void  C_Interpolate4Add_H03V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;
;	t0 = p0 + p1
;   v0 = (t0 + t1 + 2) >> 2
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ARMV6_Interpolate4Add_H03V03	PROC

    STMFD    sp!,{r4-r11,lr}
    pld		 [r0]
    pld		 [r0,r2]
    MOV      lr,#8
	
H03V03_row_loop
    MOV      r6,#8
    
H03V03_col_loop

	LDRB     r7,[r0,#0]		;lTemp0
	LDRB     r8,[r0,#1]		;lTemp1
	ADD		 r11,r0,r2
    LDRB     r9,[r11,#0]	;lTemp2
    LDRB     r10,[r11,#1]	;lTemp3
	
	ADD		 r5,r7,r8
	ADD		 r5,r5,r9
	ADD		 r5,r5,r10
	LDRB     r7,[r0,#2]		;lTemp0
	ADD		 r5,r5,#2

	LDRB     r9,[r11,#2]	;lTemp2
    USAT     r12,#8,r5,ASR #2
	ADD		 r5,r10,r8		
	ADD		 r5,r5,r7
	ADD		 r5,r5,r9
	ADD		 r5,r5,#2

	LDRB     r8,[r0,#3]		;lTemp1
	USAT     r5,#8,r5,ASR #2
	LDRB     r10,[r11,#3]	;lTemp3
	ORR		 r12,r12,r5,LSL #8
	ADD		 r5,r7,r9
	ADD		 r5,r5,r8
	ADD		 r5,r5,r10
	ADD		 r5,r5,#2

	LDRB     r7,[r0,#4]		;lTemp0
	USAT     r5,#8,r5,ASR #2
	LDRB     r9,[r11,#4]	;lTemp2
	ORR		 r12,r12,r5,LSL #16
	ADD		 r5,r10,r8
	ADD		 r5,r5,r7
	ADD		 r5,r5,r9
	ADD		 r5,r5,#2

	LDR		 r7,[r1]	
	USAT     r5,#8,r5,ASR #2
	ADD      r0,r0,#4
	ORR		 r12,r12,r5,LSL #24
	ADD		 r7,r7,#1
	UHADD8	 r12,r12,r7
	SUBS	 r6,r6,#4
    STR      r12,[r1],#4
    
    BGT      H03V03_col_loop
    
    ADD		 r1,r1,r3
    ADD		 r0,r0,r2
    SUB		 r1,r1,#8
    SUB		 r0,r0,#8
    SUBS     lr,lr,#1
    pld		 [r0,r2]
    
    BGT      H03V03_row_loop
    
    LDMFD    sp!,{r4-r11,pc}
        
	ENDP
	

ARMV6_MCCopyChroma4Add_H00V00	PROC

	pld		[r0]
	pld		[r0,r2]
	ANDS    r12,r0,#0x03
	BNE		H00V00_lab1

H00V00_lab0	
	STMFD   sp!,{r4-r8,lr}
	LDR		r12,[r0],r2
	LDR		r5,[r1],r3
	LDR		lr,[r0],r2
	LDR		r6,[r1],r3
	LDR		r4,[r0],r2
	LDR		r7,[r1],r3
	LDR		r2,[r0]
	LDR		r8,[r1],r3
	UHADD8	r5,r5,r12
	SUB		r1,r1,r3,LSL #2
	UHADD8	r6,r6,lr
	STR		r5,[r1],r3
	STR		r6,[r1],r3
	UHADD8	r7,r7,r4
	STR		r7,[r1],r3
	UHADD8	r8,r8,r2	
	STR		r8,[r1]
	LDMFD   sp!,{r4-r8,pc}

H00V00_lab1	
	STMFD   sp!,{r4-r11,lr}
	SUB		r0,r0,r12
	SUB		r2,r2,#0x04
	LDR		r4,[r0],#0x04
	MOV		r12,r12,LSL #3
	LDR		r5,[r0],r2
	RSB		lr,r12,#0x20
	LDR		r8,[r1],r3
	LDR		r6,[r0],#0x04
	LDR		r9,[r1],r3
	LDR		r7,[r0],r2
	MOV		r4,r4,LSR r12
	ORR		r4,r4,r5,LSL lr
	LDR		r10,[r1],r3
	LDR		r11,[r1],r3	
	UHADD8	r8,r8,r4
	SUB		r1,r1,r3,LSL #2
	LDR		r4,[r0],#0x04
	MOV		r6,r6,LSR r12
	LDR		r5,[r0],r2	
	ORR		r6,r6,r7,LSL lr
	STR		r8,[r1],r3
	UHADD8	r9,r9,r6
	MOV		r4,r4,LSR r12
	LDR		r6,[r0],#0x04
	ORR		r4,r4,r5,LSL lr
	LDR		r7,[r0],r2
	STR		r9,[r1],r3
	UHADD8	r10,r10,r4	
	MOV		r6,r6,LSR r12
	ORR		r6,r6,r7,LSL lr
	UHADD8	r11,r11,r6
	STR		r10,[r1],r3
	STR		r11,[r1]
	
	LDMFD   sp!,{r4-r11,pc}
	
	ENDP


ARMV6_MCCopyChroma4Add_H01V00	PROC
  
    STMFD    sp!,{r4-r9,lr}
    pld		 [r0]
	MOV		 lr,#4
	
H01V00_loop
    LDRB     r4,[r0,#0]
    LDRB     r5,[r0,#1]
	LDRB     r6,[r0,#2]
	LDRB     r7,[r0,#3]
	LDRB     r8,[r0,#4]
	ADD      r0,r0,r2

	ADD		 r9,r5,#1
    pld		 [r0]
	ADD		 r4,r4,r4,LSL #1
	ADD		 r9,r9,r4
	ADD		 r12,r6,#1
	MOV		 r9,r9,ASR #2	
	ADD		 r5,r5,r5,LSL #1
	ADD		 r12,r12,r5
	ADD		 r4,r7,#1
	MOV		 r12,r12,ASR #2	
	ADD		 r6,r6,r6,LSL #1
	ORR		 r9,r9,r12,LSL #8
	ADD		 r4,r4,r6
	ADD		 r12,r8,#1
	ADD		 r7,r7,r7,LSL #1
	ADD		 r12,r12,r7
	LDR		 r7,[r1]	
	MOV		 r4,r4,ASR #2
	MOV		 r12,r12,ASR #2
	ORR		 r9,r9,r4,LSL #16
	ORR		 r9,r9,r12,LSL #24
	UHADD8	 r9,r9,r7
    SUBS     lr,lr,#1
    STR      r9,[r1],r3
			
    BGT      H01V00_loop
    
    LDMFD    sp!,{r4-r9,pc}
	ENDP


ARMV6_MCCopyChroma4Add_H02V00	PROC
 
    STMFD    sp!,{r4-r8,lr}
    pld		 [r0]
    MOV      r4,#4
    
H02V00_loop
    LDRB     r5,[r0,#0]
    LDRB     r6,[r0,#1]
	LDRB     r7,[r0,#2]
	LDRB	 r8,[r0,#3]	
	LDRB	 lr,[r0,#4]	
	ADD      r0,r0,r2

	ADD		 r5,r5,#1
    pld		 [r0]
	ADD		 r5,r5,r6
	ADD		 r6,r6,#1
	MOV		 r12,r5,ASR #1
	ADD		 r6,r6,r7
	ADD		 r7,r7,#1
	MOV		 r6,r6,ASR #1
	ADD		 r5,r7,r8
	ADD		 r8,r8,#1
	LDR		 r7,[r1]	
	ORR		 r12,r12,r6,LSL #8
	ADD		 r8,r8,lr
	MOV		 r5,r5,ASR #1
	MOV		 r6,r8,ASR #1
	ORR		 r12,r12,r5,LSL #16
	ORR		 r12,r12,r6,LSL #24		
	UHADD8	 r12,r12,r7
	SUBS     r4,r4,#1
    STR      r12,[r1],r3
	
    BGT      H02V00_loop
    
    LDMFD    sp!,{r4-r8,pc}
	ENDP
	

ARMV6_MCCopyChroma4Add_H03V00	PROC 
 
    STMFD    sp!,{r4-r9,lr}
    pld		 [r0]
	MOV		 lr,#4
	
H03V00_loop
    LDRB     r4,[r0,#0]
    LDRB     r5,[r0,#1]
	LDRB     r6,[r0,#2]
	LDRB     r7,[r0,#3]
	LDRB     r8,[r0,#4]
	ADD      r0,r0,r2

	ADD		 r9,r4,#1
    pld		 [r0]
	ADD		 r4,r5,r5,LSL #1
	ADD		 r9,r9,r4
	ADD		 r12,r5,#1
	MOV		 r9,r9,ASR #2	
	ADD		 r5,r6,r6,LSL #1
	ADD		 r12,r12,r5
	ADD		 r4,r6,#1
	MOV		 r12,r12,ASR #2	
	ADD		 r6,r7,r7,LSL #1
	ORR		 r9,r9,r12,LSL #8
	ADD		 r4,r4,r6
	ADD		 r12,r7,#1
	ADD		 r7,r8,r8,LSL #1
	ADD		 r12,r12,r7
	LDR		 r7,[r1]	
	MOV		 r4,r4,ASR #2
	MOV		 r12,r12,ASR #2
	ORR		 r9,r9,r4,LSL #16
	ORR		 r9,r9,r12,LSL #24
	UHADD8	 r9,r9,r7
    SUBS     lr,lr,#1
    STR      r9,[r1],r3
			
    BGT      H03V00_loop
    
    LDMFD    sp!,{r4-r9,pc}
	ENDP


ARMV6_MCCopyChroma4Add_H00V01	PROC
    
    STMFD    sp!,{r4-r10,lr}
    pld		 [r0]
	ADD      r1,r1,r3,LSL #2
	SUB		 r1,r1,#1
	MOV		 lr,#4
	
H00V01_loop
    LDRB     r4,[r0],r2
    SUBS     lr,lr,#1
    LDRB     r5,[r0],r2
	SUB      r1,r1,r3,LSL #2
	LDRB     r6,[r0],r2
	ADD		 r1,r1,#1
	LDRB     r7,[r0],r2
	ADD		 r9,r5,#2
	LDRB     r8,[r0]
	SUB      r0,r0,r2,LSL #2
	LDRB	 r10,[r1]	
	ADD		 r0,r0,#1
	ADD		 r4,r4,r4,LSL #1
	ADD		 r9,r9,r4
	ADD		 r12,r6,#2
	MOV		 r9,r9,ASR #2	
	ADD		 r10,r10,#1
	UHADD16	 r9,r9,r10
	ADD		 r5,r5,r5,LSL #1
	LDRB	 r10,[r1,r3]	
    STRB     r9,[r1],r3
	ADD		 r12,r12,r5
	ADD		 r4,r7,#2
	MOV		 r12,r12,ASR #2	
	ADD		 r10,r10,#1
	UHADD16	 r12,r12,r10
	ADD		 r6,r6,r6,LSL #1
    STRB     r12,[r1],r3
	ADD		 r4,r4,r6
	LDRB	 r10,[r1]	
	ADD		 r12,r8,#2
	MOV		 r4,r4,ASR #2
	ADD		 r7,r7,r7,LSL #1
	ADD		 r10,r10,#1
	UHADD16	 r4,r4,r10
	LDRB	 r10,[r1,r3]	
	ADD		 r12,r12,r7
    STRB     r4,[r1],r3
	MOV		 r12,r12,ASR #2
	ADD		 r10,r10,#1
	UHADD16	 r12,r12,r10
    STRB     r12,[r1],r3
			
    BGT      H00V01_loop
    
    LDMFD    sp!,{r4-r10,pc}
	ENDP		


ARMV6_MCCopyChroma4Add_H01V01	PROC

    STMFD    sp!,{r4-r11,lr}	
    pld		 [r0]
	MOV		 r12,#7
    ADD      r4,r0,r2
	MOV		 r5,#9	
    MOV      lr,#4
    
H01V01_loop
	LDRB     r11,[r0,#0];lTemp0				
    LDRB     r6,[r0,#1]	;lTemp1
	LDRB     r7,[r4,#0]	;lTemp2
	LDRB     r8,[r4,#1]	;lTemp3		
	
	SMLABB	 r11,r5,r11,r12
	LDRB	 r9,[r0,#2]	
	ADD		 r7,r6,r7			 
	LDRB	 r10,[r4,#2]	
	ADD	     r7,r7,r7,LSL #1	
	ADD		 r11,r7,r11
	ADD		 r11,r11,r8		

	SMLABB	 r6,r5,r6,r12
	MOV		 r11,r11,ASR #4	
	ADD		 r8,r9,r8			 
	LDRB	 r7,[r0,#3]	
	ADD	     r8,r8,r8,LSL #1	
	ADD		 r6,r8,r6
	ADD		 r6,r6,r10		
	LDRB	 r8,[r4,#3]	
	MOV		 r6,r6,ASR #4	

	SMLABB	 r9,r5,r9,r12
	ORR		 r11,r11,r6,LSL #8
	ADD		 r10,r7,r10			 
	LDRB	 r6,[r0,#4]	
	ADD	     r10,r10,r10,LSL #1	
	ADD		 r9,r10,r9
	ADD		 r9,r9,r8		
	LDRB	 r10,[r4,#4]	
	MOV		 r9,r9,ASR #4	

	SMLABB	 r7,r5,r7,r12
	ADD		 r8,r6,r8			 
	ORR		 r11,r11,r9,LSL #16
	ADD	     r8,r8,r8,LSL #1	
	LDR		 r6,[r1]	
	ADD		 r7,r8,r7
	ADD		 r7,r7,r10		
    ADD      r4,r4,r2
	MOV		 r7,r7,ASR #4	
    ADD      r0,r0,r2
	ORR		 r11,r11,r7,LSL #24

    pld		 [r0,r2]
	UHADD8	 r11,r11,r6
	SUBS	 lr,lr,#1	
    STR      r11,[r1],r3
    BGT      H01V01_loop
    
    LDMFD    sp!,{r4-r11,pc}
	ENDP	
	
	
ARMV6_MCCopyChroma4Add_H02V01	PROC

    STMFD    sp!,{r4-r11,lr}
    pld		 [r0]
	MOV		 r12,#4		
    ADD      r4,r0,r2
	MOV		 r5,#3		
    MOV      lr,#4
    
H02V01_loop
	LDRB     r11,[r0,#0];lTemp0				
    LDRB     r6,[r0,#1]	;lTemp1
	LDRB     r7,[r4,#0]	;lTemp2
	LDRB     r8,[r4,#1]	;lTemp3		
	
	ADD		 r11,r11,r6		
	LDRB	 r9,[r0,#2]	
	SMLABB	 r11,r5,r11,r12
	ADD		 r7,r7,r8
	ADD		 r6,r6,r9		
	LDRB	 r10,[r4,#2]	
	ADD		 r7,r7,r11	
	
	SMLABB	 r6,r5,r6,r12
	MOV		 r11,r7,ASR #3
	ADD		 r8,r8,r10
	ADD		 r8,r8,r6
	LDRB	 r7,[r0,#3]			
	MOV		 r8,r8,ASR #3			
	LDRB	 r6,[r4,#3]	
	ADD		 r9,r9,r7
	ORR		 r11,r11,r8,LSL #8
				
	SMLABB	 r9,r5,r9,r12
	LDRB	 r8,[r0,#4]
	ADD		 r10,r10,r6	
	ADD		 r10,r10,r9
	LDRB	 r9,[r4,#4]
	ADD		 r7,r7,r8				
	MOV		 r10,r10,ASR #3			
	
	SMLABB	 r7,r5,r7,r12
	ORR		 r11,r11,r10,LSL #16
	ADD		 r6,r6,r9
	LDR		 r8,[r1]	
	ADD		 r6,r6,r7
	ADD		 r0,r0,r2		
	MOV		 r6,r6,ASR #3			
    pld		 [r0,r2]
    ADD      r4,r4,r2
	ORR		 r11,r11,r6,LSL #24

	UHADD8	 r11,r11,r8
	SUBS	 lr,lr,#1	
    STR      r11,[r1],r3
    BGT      H02V01_loop

    LDMFD    sp!,{r4-r11,pc}
	ENDP	
	
	
ARMV6_MCCopyChroma4Add_H03V01	PROC

  STMFD    sp!,{r4-r11,lr}
  pld		 [r0]
	MOV		 r12,#7		
  ADD      r4,r0,r2
	MOV		 r5,#9		
  MOV      lr,#4
  ;STR		 r3,[sp,#-4]
  SUB    sp,sp,#0x04
  STR    r3,[sp,#0]
    
H03V01_loop
 	LDRB     r11,[r0,#0];lTemp0				
	LDRB     r8,[r4,#1]	;lTemp3		
  LDRB     r6,[r0,#1]	;lTemp1
	LDRB     r7,[r4,#0]	;lTemp2
	LDRB	 r10,[r4,#2]
	
	SUBS	 lr,lr,#1
	ADD		 r11,r11,r8			 
	SMLABB	 r9,r5,r6,r7
	ADD	     r11,r11,r11,LSL #1		
	LDRB	 r7,[r0,#2]		
	ADD		 r11,r11,r12
	ADD		 r11,r11,r9
		
	ADD		 r6,r6,r10	
	SMLABB	 r9,r5,r7,r8		 
	MOV		 r11,r11,ASR #4	
	ADD	     r8,r6,r6,LSL #1	
	LDRB	 r6,[r0,#3]	
	ADD		 r8,r8,r12
	ADD		 r9,r8,r9
	LDRB	 r8,[r4,#3]		
	MOV		 r3,r9,ASR #4	

	SMLABB	 r9,r5,r6,r10
	ADD		 r7,r7,r8			 
	ORR		 r11,r11,r3,LSL #8	
	ADD	     r10,r7,r7,LSL #1	
	LDRB	 r7,[r0,#4]	
	ADD		 r10,r10,r12
	ADD		 r9,r10,r9
	LDRB	 r10,[r4,#4]	
	MOV		 r3,r9,ASR #4	
	
	SMLABB	 r9,r5,r7,r8  
  ADD      r4,r4,r2
	ADD		 r6,r6,r10			 
	ORR		 r11,r11,r3,LSL #16
	ADD	     r6,r6,r6,LSL #1	
	ADD		 r8,r6,r12
	ADD		 r9,r8,r9      
	LDR		 r6,[r1]	
  ;LDR		 r3,[sp,#-4]
  LDR    r3,[sp,#0]
	MOV		 r9,r9,ASR #4	
	ADD      r0,r0,r2	
	ORR		 r11,r11,r9,LSL #24

	UHADD8	 r11,r11,r6
  pld		 [r0,r2]
  STR      r11,[r1],r3
  BGT      H03V01_loop
  
  
  ADD    sp,sp,#0x04  
  LDMFD    sp!,{r4-r11,pc}
  
	ENDP	
	
	
ARMV6_MCCopyChroma4Add_H00V02	PROC
   
    STMFD    sp!,{r4-r9,lr}
    pld		 [r0]
	ADD      r1,r1,r3,LSL #2
	SUB		 r1,r1,#1
    MOV      r4,#4
H00V02_loop
    LDRB     r5,[r0],r2
	SUB      r1,r1,r3,LSL #2
    LDRB     r6,[r0],r2
	ADD		 r1,r1,#1
	LDRB     r7,[r0],r2
	LDRB	 r9,[r1]	
	LDRB	 r8,[r0],r2	
	ADD		 r5,r5,r6
	LDRB	 lr,[r0]	
	SUB      r0,r0,r2,LSL #2
	LDRB	 r12,[r1]	
	ADD		 r6,r6,r7
	ADD		 r0,r0,#1
	MOV		 r5,r5,ASR #1
	ADD		 r9,r9,#1
	UHADD16	 r5,r5,r9
	MOV		 r6,r6,ASR #1
    STRB     r5,[r1],r3
	ADD		 r12,r12,#1
	LDRB	 r9,[r1]	
	UHADD16	 r6,r6,r12
	ADD		 r7,r7,r8
    STRB     r6,[r1],r3
	MOV		 r7,r7,ASR #1
	LDRB	 r12,[r1]	
	ADD		 r9,r9,#1
	UHADD16	 r7,r7,r9
	ADD		 r8,r8,lr
    STRB     r7,[r1],r3
	MOV		 r8,r8,ASR #1
	ADD		 r12,r12,#1
	UHADD16	 r8,r8,r12
	SUBS     r4,r4,#1
    STRB     r8,[r1],r3
	
    BGT      H00V02_loop
    
    LDMFD    sp!,{r4-r9,pc}
	ENDP	
	
	
ARMV6_MCCopyChroma4Add_H01V02	PROC

    STMFD    sp!,{r4-r11,lr}
    pld		 [r0]
	MOV		 r12,#4		
    ADD      r4,r0,r2
	MOV		 r5,#3		
    MOV      lr,#4
    
H01V02_loop
	LDRB     r11,[r0,#0];lTemp0				
	LDRB     r7,[r4,#0]	;lTemp2
    LDRB     r6,[r0,#1]	;lTemp1
	LDRB     r8,[r4,#1]	;lTemp3		
	
	ADD		 r11,r11,r7		
	LDRB	 r9,[r0,#2]	
	SMLABB	 r11,r5,r11,r12
	ADD		 r6,r6,r8		
	LDRB	 r10,[r4,#2]	
	ADD		 r7,r6,r11	
	
	SMLABB	 r6,r5,r6,r12
	MOV		 r11,r7,ASR #3
	ADD		 r8,r9,r10
	ADD		 r6,r8,r6
	LDRB	 r7,[r0,#3]			
	MOV		 r10,r6,ASR #3			
	LDRB	 r6,[r4,#3]	
	ORR		 r11,r11,r10,LSL #8
				
	SMLABB	 r9,r5,r8,r12
	LDRB	 r8,[r0,#4]
	ADD		 r6,r7,r6	
	LDRB	 r7,[r4,#4]
	ADD		 r9,r6,r9
		
	SMLABB	 r6,r5,r6,r12
	MOV		 r9,r9,ASR #3			
	ADD		 r8,r8,r7
	ORR		 r11,r11,r9,LSL #16
	LDR		 r7,[r1]	
	ADD		 r6,r6,r8
	ADD		 r0,r0,r2		
	MOV		 r6,r6,ASR #3			
    pld		 [r0,r2]
    ADD      r4,r4,r2
	ORR		 r11,r11,r6,LSL #24
	ADD		 r7,r7,#1
	UHADD8	 r11,r11,r7
	SUBS	 lr,lr,#1	
    STR      r11,[r1],r3

    BGT      H01V02_loop

    LDMFD    sp!,{r4-r11,pc}
	ENDP	
	
	
ARMV6_MCCopyChroma4Add_H02V02	PROC

    STMFD    sp!,{r4-r10,lr}
    pld		 [r0]
    ADD      r4,r0,r2
    MOV      lr,#4
    
H02V02_loop
    LDRB     r5,[r0,#0]
    LDRB     r6,[r0,#1]
    LDRB     r7,[r0,#2]
    LDRB     r8,[r4,#0]
    LDRB     r9,[r4,#1]
    LDRB     r10,[r4,#2]

    ADD      r6,r6,#1
    ADD      r5,r5,r8
    ADD      r6,r6,r9
    ADD      r5,r5,r6    
    LDRB     r8,[r0,#3]
    LDRB     r9,[r4,#3]
    ADD      r7,r7,r10
    MOV      r12,r5,LSR #2
    ADD      r6,r6,r7
    LDRB     r5,[r0,#4]
    MOV      r6,r6,LSR #2
    ADD      r8,r8,#1
    ADD      r8,r8,r9
    ORR      r12,r12,r6,LSL #8    
    LDRB     r6,[r4,#4]
    ADD      r0,r0,r2
	LDR		 r9,[r1]	
    pld		 [r0,r2]    
    ADD      r7,r7,r8   
    ADD      r5,r5,r6
    MOV      r7,r7,LSR #2
    ADD      r5,r5,r8
    ORR      r12,r12,r7,LSL #16
    MOV      r5,r5,LSR #2
    ADD      r4,r4,r2
    ORR      r12,r12,r5,LSL #24    
	UHADD8	 r12,r12,r9
    SUBS     lr,lr,#1
    STR      r12,[r1],r3
    BGT      H02V02_loop
    
    LDMFD    sp!,{r4-r10,pc}
	ENDP	


	
ARMV6_MCCopyChroma4Add_H03V02	PROC

    STMFD    sp!,{r4-r11,lr}
    pld		 [r0]
	MOV		 r12,#4		
    ADD      r4,r0,r2
	MOV		 r5,#3		
    MOV      lr,#4
    
H03V02_loop
    LDRB     r6,[r0,#1]	;lTemp1
	LDRB     r8,[r4,#1]	;lTemp3		
	LDRB     r11,[r0,#0];lTemp0				
	LDRB     r7,[r4,#0]	;lTemp2
	
	ADD		 r6,r6,r8		
	LDRB	 r9,[r0,#2]	
	SMLABB	 r8,r5,r6,r12
	LDRB	 r10,[r4,#2]	
	ADD		 r11,r11,r7		
	SUBS	 lr,lr,#1
	
	ADD		 r9,r9,r10
	ADD		 r7,r8,r11	
	SMLABB	 r10,r5,r9,r12
	MOV		 r11,r7,ASR #3
	LDRB	 r7,[r0,#3]			
	ADD		 r6,r10,r6
	LDRB	 r10,[r4,#3]	
	MOV		 r6,r6,ASR #3			
		
	LDRB	 r8,[r0,#4]				
	ORR		 r11,r11,r6,LSL #8
	ADD		 r7,r7,r10
	LDRB	 r6,[r4,#4]
	ADD		 r10,r7,r7, LSL #1
	ADD		 r10,r10,r12
			
	ADD		 r8,r8,r6
	ADD		 r9,r9,r10	
	SMLABB	 r6,r5,r8,r12
	MOV		 r9,r9,ASR #3			
	LDR		 r8,[r1]	
	ADD		 r0,r0,r2		
	ADD		 r6,r6,r7
    pld		 [r0,r2]
	ORR		 r11,r11,r9,LSL #16
	MOV		 r6,r6,ASR #3			
    ADD      r4,r4,r2
	ORR		 r11,r11,r6,LSL #24
	UHADD8	 r11,r11,r8
    STR      r11,[r1],r3

    BGT      H03V02_loop
    
    LDMFD    sp!,{r4-r11,pc}
	ENDP	


ARMV6_MCCopyChroma4Add_H00V03	PROC

    STMFD    sp!,{r4-r10,lr}
    pld		 [r0]
	ADD      r1,r1,r3,LSL #2
	SUB		 r1,r1,#1
	MOV		 lr,#4
	
H00V03_loop
    LDRB     r4,[r0],r2
	SUB      r1,r1,r3,LSL #2
    LDRB     r5,[r0],r2
	ADD		 r1,r1,#1
	LDRB     r6,[r0],r2
    SUBS     lr,lr,#1
	LDRB     r7,[r0],r2
	ADD		 r9,r4,#2
	LDRB     r8,[r0]
	SUB      r0,r0,r2,LSL #2
	LDRB	 r10,[r1]	
	ADD		 r4,r5,r5,LSL #1
	ADD		 r0,r0,#1
	ADD		 r9,r9,r4
	ADD		 r12,r5,#2
	MOV		 r9,r9,ASR #2	
	ADD		 r10,r10,#1
	UHADD16	 r9,r9,r10
	LDRB	 r10,[r1,r3]	
	ADD		 r5,r6,r6,LSL #1
    STRB     r9,[r1],r3
	ADD		 r12,r12,r5
	ADD		 r4,r6,#2
	MOV		 r12,r12,ASR #2	
	ADD		 r10,r10,#1
	UHADD16	 r12,r12,r10
	ADD		 r6,r7,r7,LSL #1
	LDRB	 r10,[r1,r3]	
    STRB     r12,[r1],r3
	ADD		 r4,r4,r6
	ADD		 r12,r7,#2
	ADD		 r7,r8,r8,LSL #1
	ADD		 r12,r12,r7
	MOV		 r4,r4,ASR #2
	ADD		 r10,r10,#1
	UHADD16	 r4,r4,r10
	LDRB	 r10,[r1,r3]	
    STRB     r4,[r1],r3
	MOV		 r12,r12,ASR #2
	ADD		 r10,r10,#1
	UHADD16	 r12,r12,r10
    STRB     r12,[r1],r3
			
    BGT      H00V03_loop
    
    LDMFD    sp!,{r4-r10,pc}
	ENDP	
	
	
ARMV6_MCCopyChroma4Add_H01V03	PROC

  STMFD    sp!,{r4-r11,lr}
  pld		 [r0]
	MOV		 r12,#7		
  ADD    r4,r0,r2
	MOV		 r5,#9		
  MOV    lr,#4
  ;STR		 r3,[sp,#-4]
  SUB    sp,sp,#0x04
  STR    r3,[sp,#0]
    
H01V03_loop
 	LDRB     r11,[r0,#0];lTemp0				
	LDRB     r8,[r4,#1]	;lTemp3		
    LDRB     r6,[r0,#1]	;lTemp1
	LDRB     r7,[r4,#0]	;lTemp2
	LDRB	 r10,[r4,#2]
	
	SUBS	 lr,lr,#1
	ADD		 r11,r11,r8			 
	SMLABB	 r9,r5,r7,r6
	ADD	     r11,r11,r11,LSL #1		
	LDRB	 r7,[r0,#2]	
	ADD		 r11,r11,r12
	ADD		 r11,r11,r9
	
	ADD		 r6,r6,r10		
	SMLABB	 r9,r5,r8,r7	 
	MOV		 r11,r11,ASR #4	
	ADD	     r8,r6,r6,LSL #1	
	LDRB	 r6,[r0,#3]	
	ADD		 r8,r8,r12
	ADD		 r9,r8,r9
	LDRB	 r8,[r4,#3]		
	MOV		 r3,r9,ASR #4	

	SMLABB	 r9,r5,r10,r6	
	ADD		 r7,r7,r8	
	ORR		 r11,r11,r3,LSL #8		 		
	ADD	     r10,r7,r7,LSL #1	
	LDRB	 r7,[r0,#4]	
	ADD		 r10,r10,r12
	ADD		 r9,r10,r9
	LDRB	 r10,[r4,#4]	
	MOV		 r3,r9,ASR #4	
	
	SMLABB	 r9,r5,r8,r7  
    ADD      r4,r4,r2
	ADD		 r6,r6,r10			 
	ORR		 r11,r11,r3,LSL #16
	ADD	     r6,r6,r6,LSL #1	
	ADD		 r8,r6,r12
	LDR		 r6,[r1]	
	ADD		 r9,r8,r9      
  ;LDR		 r3,[sp,#-4]
  LDR    r3,[sp,#0]
	MOV		 r9,r9,ASR #4	
	ADD      r0,r0,r2	
	ORR		 r11,r11,r9,LSL #24
	UHADD8	 r11,r11,r6
  pld		 [r0,r2]
  STR      r11,[r1],r3

  BGT      H01V03_loop
  
  ADD    sp,sp,#0x04    
  LDMFD    sp!,{r4-r11,pc}	
  ENDP	
	
	
ARMV6_MCCopyChroma4Add_H02V03	PROC

    STMFD    sp!,{r4-r10,lr}
    pld		 [r0]
    ADD      r4,r0,r2
    MOV      lr,#4
    
H02V03_loop
	LDRB     r8,[r4,#0]	
	LDRB     r9,[r4,#1]	
	LDRB     r5,[r0,#0]		
    LDRB     r6,[r0,#1]	
    LDRB     r7,[r0,#2]	
	LDRB     r10,[r4,#2]		
	
	ADD		 r8,r8,r9	
	ADD		 r5,r5,r6		
	ADD		 r8,r8,r8,LSL #1
	ADD		 r5,r5,r8	
	ADD		 r5,r5,#4		
	ADD		 r9,r9,r10	
	MOV		 r12,r5,ASR #3
	
	ADD		 r6,r6,r7	
	ADD		 r9,r9,r9,LSL #1
	ADD		 r6,r6,r9	
	LDRB	 r5,[r0,#3]	
	ADD		 r6,r6,#4	
	LDRB	 r8,[r4,#3]		
	MOV		 r6,r6,ASR #3	
		
	LDRB	 r9,[r4,#4]	
	ORR		 r12,r12,r6,LSL #8		
	LDRB	 r6,[r0,#4]	
	ADD		 r10,r10,r8	
	ADD		 r7,r7,r5	
	ADD		 r10,r10,r10,LSL #1
	ADD		 r7,r7,r10	
	ADD		 r7,r7,#4	
	ADD		 r8,r9,r8	
	MOV		 r7,r7,ASR #3
	ADD		 r5,r6,r5	
		
	LDR		 r6,[r1]	
	ADD		 r8,r8,r8,LSL #1
	ORR		 r12,r12,r7,LSL #16
	ADD		 r5,r5,r8	
	ADD		 r5,r5,#4	
	ADD		 r0,r0,r2		
	MOV		 r5,r5,ASR #3
    pld		 [r0,r2]
    ADD      r4,r4,r2
	ORR		 r12,r12,r5,LSL #24
	UHADD8	 r12,r12,r6
	SUBS	 lr,lr,#1
    STR      r12,[r1],r3

    BGT      H02V03_loop
    
    LDMFD    sp!,{r4-r10,pc}
	ENDP	
	
	END
