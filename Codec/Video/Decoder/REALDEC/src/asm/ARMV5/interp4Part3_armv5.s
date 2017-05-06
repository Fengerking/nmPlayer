
	AREA interp4Part3, CODE, READONLY
	;{
C_MCCopyChroma4_H01V02	PROC
        STMFD    sp!,{r4-r7,lr}
        ADD      r3,r0,r2
        MOV      r12,#4
Ln8868
        LDRB     lr,[r0,#0]
        LDRB     r5,[r3,#0]
        LDRB     r4,[r0,#1]
        LDRB     r6,[r3,#1]

        ADD      lr,lr,r5
        ADD      lr,lr,lr,LSL #1
        ADD      lr,lr,r4
        ADD      lr,lr,r6
        ADD      lr,lr,#4
        MOV      r7,lr,LSR #3

        LDRB     lr,[r0,#2]
        ADD      r4,r4,r6
        LDRB     r5,[r3,#2]
        ADD      r4,r4,r4,LSL #1
        ADD      r4,r4,lr
        ADD      r4,r4,r5
        ADD      r4,r4,#4
        MOV      r4,r4,LSR #3
        ORR      r7,r7,r4,LSL #8

        ADD      lr,lr,r5
        LDRB     r4,[r0,#3]
        LDRB     r6,[r3,#3]
        ADD      lr,lr,lr,LSL #1
        ADD      lr,lr,r4
        ADD      lr,lr,r6
        ADD      lr,lr,#4
        MOV      lr,lr,LSR #3
        ORR      lr,r7,lr,LSL #16

        LDRB     r5,[r0,#4]
        LDRB     r7,[r3,#4]
        ADD      r4,r4,r6
        ADD      r4,r4,r4,LSL #1
        ADD      r4,r4,r5
        ADD      r4,r4,r7
        ADD      r4,r4,#4
        MOV      r4,r4,LSR #3
        ORR      lr,lr,r4,LSL #24

        SUBS     r12,r12,#1        
        STR      lr,[r1],#0x10
        ADD      r3,r3,r2
        ADD      r0,r0,r2
        BGT      Ln8868
        LDMFD    sp!,{r4-r7,pc}
;}//
		ENDP
		EXPORT	C_MCCopyChroma4_H01V02

;{
C_MCCopyChroma4_H02V02	PROC
        STMFD    sp!,{r4-r7,lr}
        ADD      r3,r0,r2
        MOV      r12,#4
Ln9060
        LDRB     lr,[r0,#0]
        LDRB     r4,[r0,#1]
        LDRB     r5,[r3,#0]
        LDRB     r6,[r3,#1]

        ADD      lr,lr,r4
        ADD      lr,lr,r5
        ADD      lr,lr,r6
        ADD      lr,lr,#1
        MOV      r7,lr,LSR #2

        LDRB     lr,[r0,#2]
        LDRB     r5,[r3,#2]
        SUBS     r12,r12,#1
        ADD      r4,r4,lr
        ADD      r4,r4,r6
        ADD      r4,r4,r5
        ADD      r4,r4,#1
        MOV      r4,r4,LSR #2
        ORR      r7,r7,r4,LSL #8
		
		LDRB     r6,[r3,#3]
        LDRB     r4,[r0,#3]                
		ADD      lr,lr,#1
        ADD      lr,lr,r6
        ADD      lr,lr,r5
        ADD      lr,lr,r4        
        MOV      lr,lr,LSR #2
        LDRB     r5,[r0,#4]
        ORR      lr,r7,lr,LSL #16

        LDRB     r7,[r3,#4]
        ADD      r6,r6,r5
        ADD      r6,r6,r4
        ADD      r6,r6,r7
        ADD      r6,r6,#1
        MOV      r6,r6,LSR #2
        ORR      lr,lr,r6,LSL #24
        STR      lr,[r1],#0x10

        ADD      r3,r3,r2
        ADD      r0,r0,r2
        BGT      Ln9060
        LDMFD    sp!,{r4-r7,pc}
;};//C_MCCopyChroma4_H02V02
		ENDP
		EXPORT	C_MCCopyChroma4_H02V02
;{
C_MCCopyChroma4_H03V02	PROC
        STMFD    sp!,{r4-r8,lr}
        ADD      r3,r0,r2
        MOV      r12,#4
Ln9232
        LDRB     r4,[r0,#1]
        LDRB     r6,[r3,#1]
        LDRB     lr,[r0,#0]
        LDRB     r5,[r3,#0]

        ADD      r7,r4,r6
        ADD      r7,r7,r7,LSL #1
        ADD      lr,r7,lr
        ADD      lr,lr,r5
        ADD      lr,lr,#4
        MOV      r7,lr,LSR #3

        LDRB     lr,[r0,#2]
        LDRB     r5,[r3,#2]
        SUBS     r12,r12,#1        
		ADD      r4,r4,#4
        ADD      r8,lr,r5
        ADD      r8,r8,r8,LSL #1
        ADD      r4,r8,r4
        ADD      r4,r4,r6        
        MOV      r4,r4,LSR #3
        ORR      r7,r7,r4,LSL #8

        LDRB     r4,[r0,#3]
        LDRB     r6,[r3,#3]
		ADD      lr,lr,r5
        ADD      lr,lr,#4
        ADD      r8,r4,r6
        ADD      r8,r8,r8,LSL #1
        ADD      lr,r8,lr        
        MOV      lr,lr,LSR #3
        ORR      lr,r7,lr,LSL #16

        LDRB     r7,[r3,#4]
        LDRB     r5,[r0,#4]
        ADD      r0,r0,r2
        ADD      r3,r3,r2
        ADD      r5,r5,r7
        ADD      r5,r5,r5,LSL #1
        ADD      r4,r5,r4
        ADD      r4,r4,r6
        ADD      r4,r4,#4
        MOV      r4,r4,LSR #3
        ORR      lr,lr,r4,LSL #24

        STR      lr,[r1],#0x10
        BGT      Ln9232
        LDMFD    sp!,{r4-r8,pc}
;};//
		ENDP
		EXPORT C_MCCopyChroma4_H03V02

;{
C_MCCopyChroma4_H01V03	PROC
        STMFD    sp!,{r4-r7,lr}
        ADD      r3,r0,r2
        MOV      r12,#4
Ln9420
        LDRB     lr,[r0,#0]
        LDRB     r7,[r3,#1]
        LDRB     r5,[r3,#0]
        LDRB     r4,[r0,#1]

        ADD      lr,r7,lr
        ADD      lr,lr,lr,LSL #1
        ADD      r5,r5,r5,LSL #3
        ADD      lr,r5,lr
        ADD      lr,lr,r4
        LDRB     r5,[r3,#2]
        ADD      lr,lr,#7
        MOV      r6,lr,LSR #4

        LDRB     lr,[r0,#2]
        ADD      r4,r5,r4
        ADD      r4,r4,r4,LSL #1
        ADD      r7,r7,r7,LSL #3
        ADD      r4,r7,r4
        ADD      r4,r4,lr
        ADD      r4,r4,#7
        MOV      r4,r4,LSR #4
        ORR      r7,r6,r4,LSL #8

        LDRB     r6,[r3,#3]
        LDRB     r4,[r0,#3]
        ADD      r5,r5,r5,LSL #3
        ADD      lr,r6,lr
        ADD      lr,lr,lr,LSL #1
        ADD      lr,r5,lr
        ADD      lr,lr,r4
        ADD      lr,lr,#7
        MOV      lr,lr,LSR #4
        ORR      lr,r7,lr,LSL #16

        LDRB     r7,[r3,#4]
        LDRB     r5,[r0,#4]
        ADD      r6,r6,r6,LSL #3
        ADD      r4,r7,r4
        ADD      r4,r4,r4,LSL #1
        ADD      r4,r6,r4
        ADD      r4,r4,r5
        ADD      r4,r4,#7
        MOV      r4,r4,LSR #4
        ORR      lr,lr,r4,LSL #24

        SUBS      r12,r12,#1        
        STR      lr,[r1],#0x10
        ADD      r0,r0,r2
        ADD      r3,r3,r2
        BGT      Ln9420
        LDMFD    sp!,{r4-r7,pc}
;};//C_MCCopyChroma4_H01V03
		ENDP
		EXPORT	C_MCCopyChroma4_H01V03

;{
C_MCCopyChroma4_H02V03	PROC
        STMFD    sp!,{r4-r7,lr}
        ADD      r3,r0,r2
        MOV      r12,#4
Ln9624
        LDRB     r5,[r3,#0]
        LDRB     r6,[r3,#1]
        LDRB     lr,[r0,#0]
        LDRB     r4,[r0,#1]

        ADD      r5,r5,r6
        ADD      r5,r5,r5,LSL #1
        ADD      lr,lr,r4
        ADD      lr,lr,r5
        ADD      lr,lr,#4
        MOV      r7,lr,LSR #3

        LDRB     r5,[r3,#2]
        LDRB     lr,[r0,#2]
        SUBS     r12,r12,#1
        ADD      r6,r6,r5
        ADD      r6,r6,r6,LSL #1
        ADD      r4,r4,lr
        ADD      r4,r4,r6
        ADD      r4,r4,#4
        MOV      r4,r4,LSR #3
		LDRB     r6,[r3,#3]
        ORR      r7,r7,r4,LSL #8

        
        LDRB     r4,[r0,#3]		        
        ADD      r5,r5,r6
        ADD      r5,r5,r5,LSL #1
        ADD      lr,lr,r4
        ADD      lr,lr,r5
        ADD      lr,lr,#4
        MOV      lr,lr,LSR #3
        ORR      lr,r7,lr,LSL #16

        LDRB     r5,[r0,#4]
        LDRB     r7,[r3,#4]
        ADD      r3,r3,r2
        ADD      r4,r4,r5
        ADD      r5,r6,r7
        ADD      r5,r5,r5,LSL #1
        ADD      r4,r4,r5
        ADD      r4,r4,#4
        MOV      r4,r4,LSR #3
        ORR      lr,lr,r4,LSL #24
        STR      lr,[r1],#0x10

        ADD      r0,r0,r2
        BGT      Ln9624
        LDMFD    sp!,{r4-r7,pc}
;};//C_MCCopyChroma4_H02V03
		ENDP
		EXPORT	C_MCCopyChroma4_H02V03
		
;{
C_MCCopyChroma4_H03V03	PROC
        STMFD    sp!,{r4-r8,lr}
        ADD      r3,r0,r2
        MOV      r12,#4
Ln9812
        LDRB     r4,[r0,#1]
        LDRB     r5,[r3,#0]
        LDRB     r6,[r3,#1]
        LDRB     lr,[r0,#0]

        ADD      r5,r5,r4
        ADD      r5,r5,r5,LSL #1
        ADD      r7,r6,r6,LSL #3
        ADD      r5,r7,r5
        ADD      lr,r5,lr
        ADD      lr,lr,#7
        MOV      r7,lr,LSR #4

        LDRB     lr,[r0,#2]
        LDRB     r5,[r3,#2]
        SUBS     r12,r12,#1
        ADD      r6,r6,lr
        ADD      r6,r6,r6,LSL #1
        ADD      r8,r5,r5,LSL #3
        ADD      r6,r8,r6
        ADD      r4,r6,r4
        ADD      r4,r4,#7
        MOV      r4,r4,LSR #4
        ORR      r7,r7,r4,LSL #8

        LDRB     r4,[r0,#3]
        LDRB     r6,[r3,#3]        
		ADD      lr,lr,#7
        ADD      r5,r5,r4
        ADD      r5,r5,r5,LSL #1
        ADD      r8,r6,r6,LSL #3
        ADD      r5,r8,r5
        ADD      lr,r5,lr        
        MOV      lr,lr,LSR #4
        ORR      lr,r7,lr,LSL #16

        LDRB     r5,[r0,#4]
        LDRB     r7,[r3,#4]
        ADD      r3,r3,r2
        ADD      r5,r6,r5
        ADD      r5,r5,r5,LSL #1
        ADD      r7,r7,r7,LSL #3
        ADD      r5,r7,r5
        ADD      r4,r5,r4
        ADD      r4,r4,#7
        MOV      r4,r4,LSR #4
        ORR      lr,lr,r4,LSL #24
        STR      lr,[r1],#0x10

        ADD      r0,r0,r2
        BGT      Ln9812
        LDMFD    sp!,{r4-r8,pc}
;};//C_MCCopyChroma4_H03V03
		ENDP
		EXPORT	C_MCCopyChroma4_H03V03

;{
C_Interpolate4_H00V00	PROC
		STMFD	sp!,{r4-r9,lr}				

		RSB		r12,r3,#0x10
		SUB		lr,r2,r3
		
		ANDS	r4,r0,#0x03
		BNE		Ln3		

		MOV		r4,r3			   ;dstRow	
Ln1		MOV		r5,r3			   ;j
		
Ln2		LDR		r6,[r0],#0x04      ;pTempSrc[0]
		LDR		r7,[r0],#0x04      ;pTempSrc[1]
		STR		r6,[r1],#0x04      ;pTempDst[0]
		STR		r7,[r1],#0x04      ;pTempDst[1]
		
		SUBS	r5,r5,#0x08
		BGT		Ln2
		ADD		r0,r0,lr
		ADD		r1,r1,r12

		SUBS	r4,r4,#1
		BGT		Ln1
		BLE		Ln6

Ln3		SUB		r0,r0,r4
		MOV		r4,r4,LSL #3		;i = i<<3;
		RSB		r5,r4,#0x20			;32 - i

		MOV		r6,r3
Ln4		MOV		r7,r3

Ln5		LDR		r8,[r0],#0x04		;pTempSrc[0]
		LDR		r9,[r0],#0x04		;pTempSrc[1]		
		
		MOV		r8,r8,LSR r4
		ORR		r8,r8,r9,LSL r5
		STR		r8,[r1],#0x04 

		LDR		r8,[r0,#0x00]		;pTempSrc[2]
		MOV		r9,r9,LSR r4
		ORR		r9,r9,r8,LSL r5
		STR		r9,[r1],#0x04
		
		SUBS	r7,r7,#0x08
		BGT		Ln5
		ADD		r0,r0,lr
		ADD		r1,r1,r12
				
		SUBS	r6,r6,#0x01
		BGT		Ln4
		
Ln6		LDMFD	sp!,{r4-r9,pc}
;}//C_Interpolate4_H00V00
		ENDP
		EXPORT	C_Interpolate4_H00V00