;*****************************************************************************
;*																			*
;*		VisualOn, Inc. Confidential and Proprietary, 2009					*
;*																			*
;*****************************************************************************

	AREA	|.text|, CODE
	
	EXPORT	ARMV6_MCCopyChroma4_H00V00
	EXPORT	ARMV6_MCCopyChroma4_H01V00
	EXPORT	ARMV6_MCCopyChroma4_H02V00
	EXPORT	ARMV6_MCCopyChroma4_H03V00
	EXPORT	ARMV6_MCCopyChroma4_H00V01
	EXPORT	ARMV6_MCCopyChroma4_H01V01
	EXPORT	ARMV6_MCCopyChroma4_H02V01
	EXPORT	ARMV6_MCCopyChroma4_H03V01
	EXPORT	ARMV6_MCCopyChroma4_H00V02
	EXPORT	ARMV6_MCCopyChroma4_H01V02
	EXPORT	ARMV6_MCCopyChroma4_H02V02
	EXPORT	ARMV6_MCCopyChroma4_H03V02
	EXPORT	ARMV6_MCCopyChroma4_H00V03
	EXPORT	ARMV6_MCCopyChroma4_H01V03
	EXPORT	ARMV6_MCCopyChroma4_H02V03


;huwei 20080911 bug_fixed
ARMV6_MCCopyChroma4_H00V00	PROC

;	pld		[r0]
;	pld		[r0,r2]
	ANDS    r12,r0,#0x03
	BNE		H00V00_lab1

H00V00_lab0	
	STMFD   sp!,{r4,lr}
	LDR		r12,[r0],r2
	LDR		lr,[r0],r2
	LDR		r4,[r0],r2
	LDR		r2,[r0]
	STR		r12,[r1],r3
	STR		lr,[r1],r3
	STR		r4,[r1],r3
	STR		r2,[r1]
	LDMFD   sp!,{r4,pc}

H00V00_lab1	
	STMFD   sp!,{r4-r11,lr}
	SUB		r0,r0,r12
	MOV		r12,r12,LSL #3
	SUB		r2,r2,#0x04
	RSB		lr,r12,#0x20
	LDR		r4,[r0],#0x04
	LDR		r5,[r0],r2
	LDR		r6,[r0],#0x04
	LDR		r7,[r0],r2
	MOV		r4,r4,LSR r12
	LDR		r8,[r0],#0x04
	ORR		r4,r4,r5,LSL lr
	LDR		r9,[r0],r2
	MOV		r6,r6,LSR r12
	LDR		r10,[r0],#0x04
	ORR		r6,r6,r7,LSL lr
	LDR		r11,[r0]	
	STR		r4,[r1],r3
	MOV		r8,r8,LSR r12
	STR		r6,[r1],r3
	MOV		r10,r10,LSR r12
	ORR		r8,r8,r9,LSL lr
	STR		r8,[r1],r3
	ORR		r10,r10,r11,LSL lr
	STR		r10,[r1]
	LDMFD   sp!,{r4-r11,pc}
	
	ENDP


ARMV6_MCCopyChroma4_H01V00	PROC
  
    STMFD    sp!,{r4-r9,lr}
;    pld		 [r0]
	MOV		 lr,#4
	
H01V00_loop
    LDRB     r4,[r0,#0]
    LDRB     r5,[r0,#1]
	LDRB     r6,[r0,#2]
	LDRB     r7,[r0,#3]
	LDRB     r8,[r0,#4]
	ADD      r0,r0,r2
    SUBS     lr,lr,#1
;    pld		 [r0]

	ADD		 r9,r5,#1
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
	MOV		 r4,r4,ASR #2
	MOV		 r12,r12,ASR #2
	ORR		 r9,r9,r4,LSL #16
	ORR		 r9,r9,r12,LSL #24
			
    STR      r9,[r1],r3
    BGT      H01V00_loop
    
    LDMFD    sp!,{r4-r9,pc}
	ENDP


ARMV6_MCCopyChroma4_H02V00	PROC
 
    STMFD    sp!,{r4-r8,lr}
;    pld		 [r0]
    MOV      r4,#4
    
H02V00_loop
    LDRB     r5,[r0,#0]
    LDRB     r6,[r0,#1]
	LDRB     r7,[r0,#2]
	LDRB	 r8,[r0,#3]	
	LDRB	 lr,[r0,#4]	
	ADD      r0,r0,r2
	SUBS     r4,r4,#1
;    pld		 [r0]

	ADD		 r5,r5,#1
	ADD		 r5,r5,r6
	ADD		 r6,r6,#1
	MOV		 r12,r5,ASR #1
	ADD		 r6,r6,r7
	ADD		 r7,r7,#1
	MOV		 r6,r6,ASR #1
	ADD		 r5,r7,r8
	ADD		 r8,r8,#1
	ORR		 r12,r12,r6,LSL #8
	ADD		 r8,r8,lr
	MOV		 r5,r5,ASR #1
	MOV		 r6,r8,ASR #1
	ORR		 r12,r12,r5,LSL #16
	ORR		 r12,r12,r6,LSL #24		
    STR      r12,[r1],r3
	
    BGT      H02V00_loop
    
    LDMFD    sp!,{r4-r8,pc}
	ENDP
	

ARMV6_MCCopyChroma4_H03V00	PROC 
 
    STMFD    sp!,{r4-r9,lr}
;    pld		 [r0]
	MOV		 lr,#4
	
H03V00_loop
    LDRB     r4,[r0,#0]
    LDRB     r5,[r0,#1]
	LDRB     r6,[r0,#2]
	LDRB     r7,[r0,#3]
	LDRB     r8,[r0,#4]
	ADD      r0,r0,r2
    SUBS     lr,lr,#1
;    pld		 [r0]

	ADD		 r9,r4,#1
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
	MOV		 r4,r4,ASR #2
	MOV		 r12,r12,ASR #2
	ORR		 r9,r9,r4,LSL #16
	ORR		 r9,r9,r12,LSL #24
			
    STR      r9,[r1],r3
    BGT      H03V00_loop
    
    LDMFD    sp!,{r4-r9,pc}
	ENDP


ARMV6_MCCopyChroma4_H00V01	PROC
    
    STMFD    sp!,{r4-r9,lr}
;    pld		 [r0]
;    pld		 [r0,r2]
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
	ADD		 r0,r0,#1
	ADD		 r4,r4,r4,LSL #1
	ADD		 r9,r9,r4
	ADD		 r12,r6,#2
	MOV		 r9,r9,ASR #2	
	ADD		 r5,r5,r5,LSL #1
    STRB     r9,[r1],r3
	ADD		 r12,r12,r5
	ADD		 r4,r7,#2
	MOV		 r12,r12,ASR #2	
	ADD		 r6,r6,r6,LSL #1
    STRB     r12,[r1],r3
	ADD		 r4,r4,r6
	ADD		 r12,r8,#2
	MOV		 r4,r4,ASR #2
	ADD		 r7,r7,r7,LSL #1
	ADD		 r12,r12,r7
    STRB     r4,[r1],r3
	MOV		 r12,r12,ASR #2
    STRB     r12,[r1],r3
			
    BGT      H00V01_loop
    
    LDMFD    sp!,{r4-r9,pc}
	ENDP		


ARMV6_MCCopyChroma4_H01V01	PROC

    STMFD    sp!,{r4-r11,lr}	
;    pld		 [r0]
;    pld		 [r0,r2]
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
	ADD		 r7,r8,r7
	ADD		 r7,r7,r10		
    ADD      r4,r4,r2
	MOV		 r7,r7,ASR #4	
    ADD      r0,r0,r2
	ORR		 r11,r11,r7,LSL #24

;    pld		 [r0,r2]
	SUBS	 lr,lr,#1	
    STR      r11,[r1],r3
    BGT      H01V01_loop
    
    LDMFD    sp!,{r4-r11,pc}
	ENDP	
	
	
ARMV6_MCCopyChroma4_H02V01	PROC

    STMFD    sp!,{r4-r11,lr}
;    pld		 [r0]
;    pld		 [r0,r2]
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
	ADD		 r6,r6,r7
	ADD		 r0,r0,r2		
	MOV		 r6,r6,ASR #3			
;    pld		 [r0,r2]
    ADD      r4,r4,r2
	ORR		 r11,r11,r6,LSL #24

	SUBS	 lr,lr,#1	
    STR      r11,[r1],r3
    BGT      H02V01_loop

    LDMFD    sp!,{r4-r11,pc}
	ENDP	
	
	
ARMV6_MCCopyChroma4_H03V01	PROC

    STMFD    sp!,{r4-r11,lr}
;    pld		 [r0]
;    pld		 [r0,r2]
	MOV		 r12,#7		
    ADD      r4,r0,r2
	MOV		 r5,#9		
    MOV      lr,#4
    ;STR		 r3,[sp,#-4]
    SUB      sp,sp,#0x04
    STR      r3,[sp,#0]
    
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
  ;LDR		 r3,[sp,#-4]
  LDR    r3,[sp,#0]
	MOV		 r9,r9,ASR #4	
	ADD      r0,r0,r2	
	ORR		 r11,r11,r9,LSL #24

;    pld		 [r0,r2]
    STR      r11,[r1],r3
    BGT      H03V01_loop
    
    ADD      sp,sp,#0x04    
    LDMFD    sp!,{r4-r11,pc}
	ENDP	
	
	
ARMV6_MCCopyChroma4_H00V02	PROC
   
    STMFD    sp!,{r4-r8,lr}
;    pld		 [r0]
;    pld		 [r0,r2]
	ADD      r1,r1,r3,LSL #2
	SUB		 r1,r1,#1
    MOV      r4,#4
    
H00V02_loop
    LDRB     r5,[r0],r2
	SUB      r1,r1,r3,LSL #2
    LDRB     r6,[r0],r2
	ADD		 r1,r1,#1
	LDRB     r7,[r0],r2
	SUBS     r4,r4,#1
	LDRB	 r8,[r0],r2	
	ADD		 r5,r5,r6
	LDRB	 lr,[r0]	
	SUB      r0,r0,r2,LSL #2
	ADD		 r6,r6,r7
	ADD		 r0,r0,#1
	MOV		 r5,r5,ASR #1
	MOV		 r6,r6,ASR #1
    STRB     r5,[r1],r3
	ADD		 r7,r7,r8
    STRB     r6,[r1],r3
	MOV		 r7,r7,ASR #1
	ADD		 r8,r8,lr
    STRB     r7,[r1],r3
	MOV		 r8,r8,ASR #1
    STRB     r8,[r1],r3
	
    BGT      H00V02_loop
    
    LDMFD    sp!,{r4-r8,pc}
	ENDP	
	
	
ARMV6_MCCopyChroma4_H01V02	PROC

    STMFD    sp!,{r4-r11,lr}
;    pld		 [r0]
;    pld		 [r0,r2]
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
	ADD		 r6,r6,r8
	ADD		 r0,r0,r2		
	MOV		 r6,r6,ASR #3			
;    pld		 [r0,r2]
    ADD      r4,r4,r2
	ORR		 r11,r11,r6,LSL #24

	SUBS	 lr,lr,#1	
    STR      r11,[r1],r3
    BGT      H01V02_loop

    LDMFD    sp!,{r4-r11,pc}
	ENDP	
	
	
ARMV6_MCCopyChroma4_H02V02	PROC

    STMFD    sp!,{r4-r10,lr}
;    pld		 [r0]
;    pld		 [r0,r2]
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
;    pld		 [r0,r2]    
    ADD      r7,r7,r8   
    ADD      r5,r5,r6
    MOV      r7,r7,LSR #2
    ADD      r5,r5,r8
    ORR      r12,r12,r7,LSL #16
    MOV      r5,r5,LSR #2
    ADD      r4,r4,r2
    ORR      r12,r12,r5,LSL #24    
    SUBS     lr,lr,#1
    STR      r12,[r1],r3
    BGT      H02V02_loop
    
    LDMFD    sp!,{r4-r10,pc}
	ENDP	


	
ARMV6_MCCopyChroma4_H03V02	PROC

    STMFD    sp!,{r4-r11,lr}
;    pld		 [r0]
;    pld		 [r0,r2]
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
	ADD		 r0,r0,r2		
;    pld		 [r0,r2]
	ADD		 r6,r6,r7
	ORR		 r11,r11,r9,LSL #16
	MOV		 r6,r6,ASR #3			
    ADD      r4,r4,r2
	ORR		 r11,r11,r6,LSL #24

    STR      r11,[r1],r3
    BGT      H03V02_loop
    
    LDMFD    sp!,{r4-r11,pc}
	ENDP	


ARMV6_MCCopyChroma4_H00V03	PROC

    STMFD    sp!,{r4-r9,lr}
;    pld		 [r0]
;    pld		 [r0,r2]
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
	ADD		 r4,r5,r5,LSL #1
	ADD		 r0,r0,#1
	ADD		 r9,r9,r4
	ADD		 r12,r5,#2
	MOV		 r9,r9,ASR #2	
	ADD		 r5,r6,r6,LSL #1
    STRB     r9,[r1],r3
	ADD		 r12,r12,r5
	ADD		 r4,r6,#2
	MOV		 r12,r12,ASR #2	
	ADD		 r6,r7,r7,LSL #1
    STRB     r12,[r1],r3
	ADD		 r4,r4,r6
	ADD		 r12,r7,#2
	ADD		 r7,r8,r8,LSL #1
	ADD		 r12,r12,r7
	MOV		 r4,r4,ASR #2
    STRB     r4,[r1],r3
	MOV		 r12,r12,ASR #2
    STRB     r12,[r1],r3
			
    BGT      H00V03_loop
    
    LDMFD    sp!,{r4-r9,pc}
	ENDP	
	
	
ARMV6_MCCopyChroma4_H01V03	PROC

    STMFD    sp!,{r4-r11,lr}
;    pld		 [r0]
;    pld		 [r0,r2]
	MOV		 r12,#7		
    ADD      r4,r0,r2
	MOV		 r5,#9		
    MOV      lr,#4
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
	ADD		 r9,r8,r9      
  ;LDR		 r3,[sp,#-4]
  LDR    r3,[sp,#0]
	MOV		 r9,r9,ASR #4	
	ADD      r0,r0,r2	
	ORR		 r11,r11,r9,LSL #24

;    pld		 [r0,r2]
    STR      r11,[r1],r3
    BGT      H01V03_loop
    
    ADD    sp,sp,#0x04
    LDMFD    sp!,{r4-r11,pc}	
    ENDP	
	
	
ARMV6_MCCopyChroma4_H02V03	PROC

    STMFD    sp!,{r4-r10,lr}
;    pld		 [r0]
;    pld		 [r0,r2]
    ADD      r4,r0,r2
    MOV      lr,#4
    
H02V03_loop
	LDRB     r8,[r4,#0]	
	LDRB     r9,[r4,#1]	
	LDRB     r5,[r0,#0]		
    LDRB     r6,[r0,#1]	
    LDRB     r7,[r0,#2]	
	LDRB     r10,[r4,#2]		
	
	SUBS	 lr,lr,#1
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
		
	ADD		 r8,r8,r8,LSL #1
	ORR		 r12,r12,r7,LSL #16
	ADD		 r5,r5,r8	
	ADD		 r5,r5,#4	
	ADD		 r0,r0,r2		
;    pld		 [r0,r2]
	MOV		 r5,r5,ASR #3
    ADD      r4,r4,r2
	ORR		 r12,r12,r5,LSL #24

    STR      r12,[r1],r3
    BGT      H02V03_loop
    
    LDMFD    sp!,{r4-r10,pc}
	ENDP	
	
		
	END



