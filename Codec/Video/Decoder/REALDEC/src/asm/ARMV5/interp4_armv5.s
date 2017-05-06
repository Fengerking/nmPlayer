
	AREA interp4, CODE, READONLY
	;{
C_Interpolate4_H01V00	PROC
	    IMPORT RV30_ClampTbl

        STMFD    sp!,{r4-r11,lr}
		SUB		 sp,sp,#16
        LDR      r4,=RV30_ClampTbl
		SUB		 r2,r2,r3
		STR	     r2,[sp,#0]
		RSB		 r2,r3,#16
		STR	     r2,[sp,#8]
		STR	     r3,[sp,#12]
        MOV      r5,r3
		MOV		 r3,#52		
        ;B        Ln368
Ln32
		LDR	     r6,[sp,#12]        
        ;B        Ln340
Ln40
        LDRB     r9,[r0,#0]		;lTemp2
        LDRB     r8,[r0,#-1]	;lTemp1
        LDRB     r11,[r0,#2]	;lTemp4
		LDRB     r7,[r0,#-2]	;lTemp0
        LDRB     r10,[r0,#1]	;lTemp3        
		LDRB     r12,[r0,#3]	;lTemp5
		
		MLA	 r7,r3,r9,r7
		ADD		 lr,r8,r11
		RSB		 lr,lr,r10,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r7
		ADD		 lr,lr,r12
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r2,[lr,#0x120]

		MLA	 r8,r3,r10,r8
		ADD		 lr,r9,r12
		RSB		 lr,lr,r11,LSL #2
        LDRB     r7,[r0,#4]	;lTemp0
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r8
		ADD		 lr,lr,r7
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r8,[lr,#0x120]

		
		MLA	 r9,r3,r11,r9
		ADD		 lr,r10,r7
		RSB		 lr,lr,r12,LSL #2
        ORR      r2,r2,r8,LSL #8
        LDRB     r8,[r0,#5]	;lTemp0
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r9
		ADD		 lr,lr,r8
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r9,[lr,#0x120]

		MLA	 r10,r3,r12,r10
		ADD		 lr,r11,r8
		RSB		 lr,lr,r7,LSL #2
        ORR      r2,r2,r9,LSL #16
        LDRB     r9,[r0,#6]	;lTemp0
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r10
		ADD		 lr,lr,r9
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r10,[lr,#0x120]

        ADD      r0,r0,#4
        SUBS     r6,r6,#4
		ORR      r2,r2,r10,LSL #24
        STR      r2,[r1],#4
Ln340
        ;CMP      r6,#0
        BGT      Ln40
        LDR		 r7,[sp,#8]
		LDR	     r2,[sp,#0]
		ADD		 r1,r1,r7
        ADD		 r0,r0,r2
        SUBS     r5,r5,#1
Ln368
        ;CMP      r5,#0
        BGT      Ln32
		ADD		 sp,sp,#16
        LDMFD    sp!,{r4-r11,pc}
;}//
		ENDP
		EXPORT	C_Interpolate4_H01V00
		
;{
C_Interpolate4_H02V00	PROC
		IMPORT   RV30_ClampTbl

        STMFD    sp!,{r4-r11,lr}
		SUB		 sp,sp,#8	
        LDR      r4,=RV30_ClampTbl        
		SUB		 r2,r2,r3
		STR		 r2,[sp,#0]
        MOV      r5,r3
        ;B        Ln652
Ln408
        MOV      r6,r3
        ;B        Ln624
Ln416
        LDRB     r9,[r0,#0]		;lTemp2
        LDRB     r8,[r0,#-1]	;lTemp1
        LDRB     r11,[r0,#2]	;lTemp4
        LDRB     r10,[r0,#1]	;lTemp3
        LDRB     r7,[r0,#-2]	;lTemp0
		LDRB     r12,[r0,#3]	;lTemp5
		
		ADD		 lr,r9,r10	
		RSB		 lr,r8,lr,LSL #2
		SUB		 lr,lr,r11
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r7
		ADD		 lr,lr,r12
		ADD		 lr,lr,#0x10
        ADD      lr,r4,lr,ASR #5
        LDRB     r2,[lr,#0x120]

		ADD		 lr,r10,r11
		RSB		 lr,r9,lr,LSL #2
        LDRB     r7,[r0,#4]	;lTemp0
		SUB		 lr,lr,r12
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r8
		ADD		 lr,lr,r7
		ADD		 lr,lr,#0x10
        ADD      lr,r4,lr,ASR #5
        LDRB     r8,[lr,#0x120]

		ADD		 lr,r11,r12
		RSB		 lr,r10,lr,LSL #2
        ORR      r2,r2,r8,LSL #8
        LDRB     r8,[r0,#5]	;lTemp0
		SUB		 lr,lr,r7
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r9
		ADD		 lr,lr,r8
		ADD		 lr,lr,#0x10
        ADD      lr,r4,lr,ASR #5
        LDRB     r9,[lr,#0x120]

		ADD		 lr,r12,r7
		RSB		 lr,r11,lr,LSL #2
        ORR      r2,r2,r9,LSL #16
        LDRB     r9,[r0,#6]	;lTemp0
		SUB		 lr,lr,r8
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r10
		ADD		 lr,lr,r9
		ADD		 lr,lr,#0x10
        ADD      lr,r4,lr,ASR #5
        LDRB     r10,[lr,#0x120]

        ADD      r0,r0,#4
        SUBS     r6,r6,#4
		ORR      r2,r2,r10,LSL #24
        STR      r2,[r1],#4
Ln624
        ;CMP      r6,#0
        BGT      Ln416
        ADD		 r1,r1,#16
        SUB		 r1,r1,r3
		LDR		 r2,[sp,#0]
  		SUBS     r5,r5,#1
        ADD		 r0,r0,r2        
Ln652
        ;CMP      r5,#0
        BGT      Ln408
		ADD		 sp,sp,#8
        LDMFD    sp!,{r4-r11,pc}
;}//
		ENDP
		EXPORT	C_Interpolate4_H02V00
		
C_Interpolate4_H03V00	PROC
;{
		IMPORT RV30_ClampTbl

        STMFD    sp!,{r4-r11,lr}
		SUB		 sp,sp,#16
        LDR      r4,=RV30_ClampTbl
		SUB		 r2,r2,r3
		STR	     r2,[sp,#0]
		RSB		 r2,r3,#16
		STR	     r2,[sp,#8]
		STR	     r3,[sp,#12]
        MOV      r5,r3
		MOV		 r3,#52		
        ;B        Ln960
Ln700
        LDR	     r6,[sp,#12]        
        ;B        Ln932
Ln708
        LDRB     r9,[r0,#0]		;lTemp2
        LDRB     r8,[r0,#-1]	;lTemp1        
		LDRB     r7,[r0,#-2]	;lTemp0
        LDRB     r10,[r0,#1]	;lTemp3
		LDRB     r11,[r0,#2]	;lTemp4        
		LDRB     r12,[r0,#3]	;lTemp5		
		
		MLA	 r7,r3,r10,r7
		ADD		 lr,r8,r11
		RSB		 lr,lr,r9,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r7
		ADD		 lr,lr,r12
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r2,[lr,#0x120]

		MLA	 r8,r3,r11,r8
		ADD		 lr,r9,r12
		RSB		 lr,lr,r10,LSL #2
        LDRB     r7,[r0,#4]	;lTemp0
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r8
		ADD		 lr,lr,r7
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r8,[lr,#0x120]
		
		MLA	 r9,r3,r12,r9
		ADD		 lr,r10,r7
		RSB		 lr,lr,r11,LSL #2
        ORR      r2,r2,r8,LSL #8
        LDRB     r8,[r0,#5]	;lTemp0
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r9
		ADD		 lr,lr,r8
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r9,[lr,#0x120]

		MLA	 r10,r3,r7,r10
		ADD		 lr,r11,r8
		RSB		 lr,lr,r12,LSL #2
        ORR      r2,r2,r9,LSL #16
        LDRB     r9,[r0,#6]	;lTemp0
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r10
		ADD		 lr,lr,r9
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r10,[lr,#0x120]

        ADD      r0,r0,#4
        SUBS     r6,r6,#4
		ORR      r2,r2,r10,LSL #24
        STR      r2,[r1],#4
Ln932
        ;CMP      r6,#0
        BGT      Ln708
        LDR		 r7,[sp,#8]
		LDR	     r2,[sp,#0]
		ADD		 r1,r1,r7
        ADD		 r0,r0,r2
        SUBS     r5,r5,#1
Ln960
        ;CMP      r5,#0
        BGT      Ln700
		ADD		 sp,sp,#16
        LDMFD    sp!,{r4-r11,pc}
 ;}
        ENDP
		EXPORT	C_Interpolate4_H03V00
;{
C_Interpolate4_H00V01	PROC
		IMPORT RV30_ClampTbl

        STMFD    sp!,{r4-r11,lr}
		SUB      sp,sp,#0x10
		MOV		 r4,r3,LSL #4
		SUB		 r4,r4,#1
		STR		 r4,[sp,#4]
		MUL		 r4,r2,r3
		SUB		 r4,r4,#1
		STR		 r4,[sp,#8]
        LDR      r4,=RV30_ClampTbl
        MOV      r5,r3
		STR		 r3,[sp,#12]
		MOV		 r3,#52
        ;B        Ln1324
Ln1040
        LDR      r6,[sp,#12]
		;B        Ln1288
Ln1048
        LDRB     r9,[r0,#0]			;lTemp2
        LDRB     r8,[r0,-r2]		;lTemp1
        LDRB     r11,[r0,r2,LSL #1]	;lTemp4
        LDRB     r10,[r0,r2]		;lTemp3
        LDRB     r7,[r0,-r2,LSL #1]	;lTemp0
		ADD		 lr,r2,r2,LSL #1
		LDRB     r12,[r0,lr]		;lTemp5        

		MLA	 r7,r3,r9,r7
		ADD		 lr,r8,r11
		RSB		 lr,lr,r10,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r7
		ADD		 lr,lr,r12
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r7,[lr,#0x120]

		MLA	 r8,r3,r10,r8
		ADD		 lr,r9,r12
		RSB		 lr,lr,r11,LSL #2
        STRB     r7,[r1],#0x10
        LDRB     r7,[r0,r2,LSL #2]	;lTemp0
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r8
		ADD		 lr,lr,r7
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r8,[lr,#0x120]
		
		MLA	 r9,r3,r11,r9
		ADD		 lr,r2,r2,LSL #2
        STRB     r8,[r1],#0x10
        LDRB     r8,[r0,lr]			;lTemp1
		ADD		 lr,r10,r7
		RSB		 lr,lr,r12,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r9
		ADD		 lr,lr,r8
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r9,[lr,#0x120]

		MLA	 r10,r3,r12,r10
		ADD		 lr,r2,r2,LSL #1
        STRB     r9,[r1],#0x10
        LDRB     r9,[r0,lr,LSL #1]		;lTemp2
		ADD		 lr,r11,r8
		RSB		 lr,lr,r7,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r10
		ADD		 lr,lr,r9
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r10,[lr,#0x120]

		ADD      r0,r0,r2,lsl #2
        SUBS     r6,r6,#4
        STRB     r10,[r1],#0x10

Ln1288
        ;CMP      r6,#0
        BGT      Ln1048
		LDR		 r7,[sp,#8]
		LDR		 lr,[sp,#4]	
		SUB		 r0,r0,r7	
        SUB		 r1,r1,lr
        SUBS     r5,r5,#1
Ln1324
        ;CMP      r5,#0
        BGT      Ln1040
		ADD      sp,sp,#0x10
        LDMFD    sp!,{r4-r11,pc}
;}//
		ENDP
		EXPORT	C_Interpolate4_H00V01

;{
C_Interpolate4_H01V01 PROC
		IMPORT RV30_ClampTbl

		STMFD    sp!,{r4-r11,lr}
        SUB      sp,sp,#0x164

        STR		 r3,[sp,#0]	
		RSB		 r4,r3,#0x10
		STR		 r4,[sp,#4]
        LDR      r4,=RV30_ClampTbl
		STR		 r1,[sp,#8]
		SUB      r0,r0,r2,LSL #1
		SUB		 r2,r2,r3
		STR		 r2,[sp,#16]
		MOV		 r2,#52
		ADD      r1,sp,#20
		ADD      r5,r3,#5
        ;B        Ln1672
Ln1388
		LDR		 r6,[sp,#0]
        ;B        Ln1636
Ln1396
        LDRB     r9,[r0,#0]		;lTemp2
        LDRB     r8,[r0,#-1]	;lTemp1
        LDRB     r11,[r0,#2]	;lTemp4
		LDRB     r7,[r0,#-2]	;lTemp0
        LDRB     r10,[r0,#1]	;lTemp3        
		LDRB     r12,[r0,#3]	;lTemp5
		
		MLA	 r7,r9,r2,r7
		ADD		 lr,r8,r11
		RSB		 lr,lr,r10,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r7
		ADD		 lr,lr,r12
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r3,[lr,#0x120]

		MLA	 r8,r10,r2,r8
		ADD		 lr,r9,r12
        LDRB     r7,[r0,#4]	;lTemp0
		RSB		 lr,lr,r11,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r8
		ADD		 lr,lr,r7
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r8,[lr,#0x120]

		MLA	 r9,r11,r2,r9
		ADD		 lr,r10,r7
		ORR	     r3,r3,r8,LSL #8        
        LDRB     r8,[r0,#5]	;lTemp1
		RSB		 lr,lr,r12,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r9
		ADD		 lr,lr,r8
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r9,[lr,#0x120]

		MLA	 r10,r12,r2,r10
		ADD		 lr,r11,r8
		ORR	     r3,r3,r9,LSL #16        
        LDRB     r9,[r0,#6]	;lTemp2
		RSB		 lr,lr,r7,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r10
		ADD		 lr,lr,r9
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r10,[lr,#0x120]

        ADD      r0,r0,#4
        SUBS     r6,r6,#4
		ORR	     r3,r3,r10,LSL #24
        
		STR      r3,[r1],#4                
Ln1636
        ;CMP      r6,#0
        BGT      Ln1396		
		LDR		 r7,[sp,#4]
		LDR      lr,[sp,#16]
		ADD		 r1,r1,r7				
		ADD		 r0,r0,lr
        SUBS     r5,r5,#1
Ln1672
        ;CMP      r5,#0
        BGT      Ln1388

		LDR		 r5,[sp,#0]
		ADD      r0,sp,#20
		LDR		 r1,[sp,#8]
		MOV		 r3,r5,LSL #4
		SUB		 r3,r3,#1	
        ;B        Ln1984
Ln1708
        LDR		 r6,[sp,#0]
        ;B        Ln1960
Ln1716
        LDRB     r9,[r0,#0x20]		;lTemp2
        LDRB     r8,[r0,#0x10]		;lTemp1
        LDRB     r11,[r0,#0x40]		;lTemp4
		LDRB     r7,[r0,#0]			;lTemp0
        LDRB     r10,[r0,#0x30]		;lTemp3        
		LDRB     r12,[r0,#0x50]		;lTemp5        
		
		MLA	 r7,r9,r2,r7
		ADD		 lr,r8,r11
		RSB		 lr,lr,r10,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r7
		ADD		 lr,lr,r12
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r7,[lr,#0x120]

		MLA	 r8,r10,r2,r8
		ADD		 lr,r9,r12
        STRB     r7,[r1],#0x10
        LDRB     r7,[r0,#0x60]	;lTemp0
		RSB		 lr,lr,r11,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r8
		ADD		 lr,lr,r7
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r8,[lr,#0x120]

		MLA	 r9,r11,r2,r9
		ADD		 lr,r10,r7
        STRB     r8,[r1],#0x10
        LDRB     r8,[r0,#0x70]			;lTemp1
		RSB		 lr,lr,r12,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r9
		ADD		 lr,lr,r8
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r9,[lr,#0x120]

		MLA	 r10,r12,r2,r10
		ADD		 lr,r11,r8
        STRB     r9,[r1],#0x10
        LDRB     r9,[r0,#0x80]			;lTemp2
		RSB		 lr,lr,r7,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r10
		ADD		 lr,lr,r9
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r10,[lr,#0x120]

		ADD      r0,r0,#0x40
        SUBS     r6,r6,#4
        STRB     r10,[r1],#0x10
Ln1960
        ;CMP      r6,#0
        BGT      Ln1716		
		SUB		 r0,r0,r3
        SUB		 r1,r1,r3        
		SUBS     r5,r5,#1
Ln1984
        ;CMP      r5,#0
        BGT      Ln1708
        ADD      sp,sp,#0x164
        LDMFD    sp!,{r4-r11,pc}
;}//
		ENDP
		EXPORT	C_Interpolate4_H01V01

;{
C_Interpolate4_H02V01	PROC

		IMPORT RV30_ClampTbl

        STMFD    sp!,{r4-r11,lr}
        SUB      sp,sp,#0x160
        STR		 r3,[sp,#0]	
		RSB		 r4,r3,#0x10
		STR		 r4,[sp,#4]
        LDR      r4,=RV30_ClampTbl
		STR		 r1,[sp,#8]
		SUB      r0,r0,r2,LSL #1
		SUB		 r2,r2,r3
		STR      r2,[sp,#12]
		ADD      r1,sp,#16
		ADD      r5,r3,#5
        ;B        Ln2312
Ln2044
		LDR		 r6,[sp,#0]
        ;B        Ln2276
Ln2052
        LDRB     r9,[r0,#0]		;lTemp2
        LDRB     r8,[r0,#-1]	;lTemp1
        LDRB     r11,[r0,#2]	;lTemp4
		LDRB     r7,[r0,#-2]	;lTemp0
        LDRB     r10,[r0,#1]	;lTemp3        
		LDRB     r12,[r0,#3]	;lTemp5        
		
		ADD		 lr,r9,r10	
		RSB		 lr,r8,lr,LSL #2
		SUB		 lr,lr,r11 
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r7		
		ADD		 lr,lr,r12
		ADD		 lr,lr,#0x10
        ADD      lr,r4,lr,ASR #5
        LDRB     r3,[lr,#0x120]

		ADD		 lr,r10,r11
		RSB		 lr,r9,lr,LSL #2
        LDRB     r7,[r0,#4]	;lTemp0
		SUB		 lr,lr,r12
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r8		
		ADD		 lr,lr,r7
		ADD		 lr,lr,#0x10
        ADD      lr,r4,lr,ASR #5
        LDRB     r8,[lr,#0x120]

		ADD		 lr,r11,r12
		RSB		 lr,r10,lr,LSL #2
		ORR	     r3,r3,r8,LSL #8        
        LDRB     r8,[r0,#5]	;lTemp1
		SUB		 lr,lr,r7
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r9		
		ADD		 lr,lr,r8
		ADD		 lr,lr,#0x10
        ADD      lr,r4,lr,ASR #5
        LDRB     r9,[lr,#0x120]

		ADD		 lr,r12,r7
		RSB		 lr,r11,lr,LSL #2
		ORR	     r3,r3,r9,LSL #16        
        LDRB     r9,[r0,#6]	;lTemp2
		SUB		 lr,lr,r8
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r10		
		ADD		 lr,lr,r9
		ADD		 lr,lr,#0x10
        ADD      lr,r4,lr,ASR #5
        LDRB     r10,[lr,#0x120]

        ADD      r0,r0,#4
        SUBS     r6,r6,#4
		ORR	     r3,r3,r10,LSL #24
        
		STR      r3,[r1],#4                
Ln2276
        ;CMP      r6,#0
        BGT      Ln2052				
		LDR		 lr,[sp,#4]
		ADD		 r0,r0,r2
		ADD		 r1,r1,lr	  						
        SUBS     r5,r5,#1
Ln2312        
        ;CMP      r5,#0
        BGT      Ln2044

		MOV		 r2,#52	
		LDR		 r5,[sp,#0]
		ADD      r0,sp,#16
		LDR		 r1,[sp,#8]
		MOV		 r3,r5,LSL #4
		SUB		 r3,r3,#1	
        ;B       Ln2624
Ln2348
        LDR		 r6,[sp,#0]
        ;B       Ln2600
Ln2356
        LDRB     r9,[r0,#0x20]		;lTemp2
        LDRB     r8,[r0,#0x10]		;lTemp1
        LDRB     r11,[r0,#0x40]		;lTemp4
		LDRB     r7,[r0,#0]			;lTemp0
        LDRB     r10,[r0,#0x30]		;lTemp3        
		LDRB     r12,[r0,#0x50]		;lTemp5        
		
		MLA	 r7,r9,r2,r7
		ADD		 lr,r8,r11
		RSB		 lr,lr,r10,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r7
		ADD		 lr,lr,r12
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r7,[lr,#0x120]

		MLA	 r8,r10,r2,r8
		ADD		 lr,r9,r12
        STRB     r7,[r1],#0x10
        LDRB     r7,[r0,#0x60]	;lTemp0
		RSB		 lr,lr,r11,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r8
		ADD		 lr,lr,r7
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r8,[lr,#0x120]

		MLA	 r9,r11,r2,r9
		ADD		 lr,r10,r7
        STRB     r8,[r1],#0x10
        LDRB     r8,[r0,#0x70]			;lTemp1
		RSB		 lr,lr,r12,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r9
		ADD		 lr,lr,r8
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r9,[lr,#0x120]

		MLA	 r10,r12,r2,r10
		ADD		 lr,r11,r8
        STRB     r9,[r1],#0x10
        LDRB     r9,[r0,#0x80]			;lTemp2
		RSB		 lr,lr,r7,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r10
		ADD		 lr,lr,r9
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r10,[lr,#0x120]

		ADD      r0,r0,#0x40
        SUBS     r6,r6,#4
        STRB     r10,[r1],#0x10
Ln2600
        ;CMP      r6,#0
        BGT      Ln2356        
		SUB		 r0,r0,r3
        SUB		 r1,r1,r3
		SUBS     r5,r5,#1
Ln2624
        ;CMP      r5,#0
        BGT      Ln2348
        ADD      sp,sp,#0x160
        LDMFD    sp!,{r4-r11,pc}
;}//
		ENDP
		EXPORT	C_Interpolate4_H02V01
		
;{
C_Interpolate4_H03V01	PROC
		IMPORT RV30_ClampTbl

        STMFD    sp!,{r4-r11,lr}
        SUB      sp,sp,#0x164

        STR		 r3,[sp,#0]	
		RSB		 r4,r3,#0x10
		STR		 r4,[sp,#4]
        LDR      r4,=RV30_ClampTbl
		STR		 r1,[sp,#8]
		SUB      r0,r0,r2,LSL #1
		SUB		 r2,r2,r3
		STR		 r2,[sp,#16]
		MOV		 r2,#52
		ADD      r1,sp,#20
		ADD      r5,r3,#5
        ;B        Ln2972
Ln2688
		LDR		 r6,[sp,#0]
        ;B        Ln2936
Ln2696
        LDRB     r9,[r0,#0]		;lTemp2
        LDRB     r8,[r0,#-1]	;lTemp1
        LDRB     r11,[r0,#2]	;lTemp4
		LDRB     r7,[r0,#-2]	;lTemp0
        LDRB     r10,[r0,#1]	;lTemp3        
		LDRB     r12,[r0,#3]	;lTemp5
		
		MLA	 r7,r10,r2,r7
		ADD		 lr,r8,r11
		RSB		 lr,lr,r9,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r7
		ADD		 lr,lr,r12
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r3,[lr,#0x120]

		MLA	 r8,r11,r2,r8
		ADD		 lr,r9,r12
        LDRB     r7,[r0,#4]	;lTemp0
		RSB		 lr,lr,r10,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r8
		ADD		 lr,lr,r7
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r8,[lr,#0x120]

		MLA	 r9,r12,r2,r9
		ADD		 lr,r10,r7
		ORR	     r3,r3,r8,LSL #8        
        LDRB     r8,[r0,#5]	;lTemp1
		RSB		 lr,lr,r11,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r9
		ADD		 lr,lr,r8
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r9,[lr,#0x120]

		MLA	 r10,r7,r2,r10
		ADD		 lr,r11,r8
		ORR	     r3,r3,r9,LSL #16        
        LDRB     r9,[r0,#6]	;lTemp2
		RSB		 lr,lr,r12,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r10
		ADD		 lr,lr,r9
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r10,[lr,#0x120]

        ADD      r0,r0,#4
        SUBS     r6,r6,#4
		ORR	     r3,r3,r10,LSL #24
        
		STR      r3,[r1],#4                
Ln2936
        ;CMP      r6,#0
        BGT      Ln2696		
		LDR		 r7,[sp,#4]
		LDR      lr,[sp,#16]		
		ADD		 r1,r1,r7		
		ADD		 r0,r0,lr
        SUBS     r5,r5,#1
Ln2972
        ;CMP      r5,#0
        BGT      Ln2688

		LDR		 r5,[sp,#0]
		ADD      r0,sp,#20
		LDR		 r1,[sp,#8]
		MOV		 r3,r5,LSL #4
		SUB		 r3,r3,#1
        ;B        Ln3284
Ln3008
        LDR		 r6,[sp,#0]
        ;B        Ln3260
Ln3016
        LDRB     r9,[r0,#0x20]		;lTemp2
        LDRB     r8,[r0,#0x10]		;lTemp1
        LDRB     r11,[r0,#0x40]		;lTemp4
		LDRB     r7,[r0,#0]			;lTemp0
        LDRB     r10,[r0,#0x30]		;lTemp3        
		LDRB     r12,[r0,#0x50]		;lTemp5        
		
		MLA	 r7,r9,r2,r7
		ADD		 lr,r8,r11
		RSB		 lr,lr,r10,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r7
		ADD		 lr,lr,r12
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r7,[lr,#0x120]

		MLA	 r8,r10,r2,r8
		ADD		 lr,r9,r12
        STRB     r7,[r1],#0x10
        LDRB     r7,[r0,#0x60]	;lTemp0
		RSB		 lr,lr,r11,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r8
		ADD		 lr,lr,r7
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r8,[lr,#0x120]

		MLA	 r9,r11,r2,r9
		ADD		 lr,r10,r7
        STRB     r8,[r1],#0x10
        LDRB     r8,[r0,#0x70]			;lTemp1
		RSB		 lr,lr,r12,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r9
		ADD		 lr,lr,r8
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r9,[lr,#0x120]

		MLA	 r10,r12,r2,r10
		ADD		 lr,r11,r8
        STRB     r9,[r1],#0x10
        LDRB     r9,[r0,#0x80]			;lTemp2
		RSB		 lr,lr,r7,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r10
		ADD		 lr,lr,r9
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r10,[lr,#0x120]

		ADD      r0,r0,#0x40
        SUBS     r6,r6,#4
        STRB     r10,[r1],#0x10
Ln3260
        ;CMP      r6,#0
        BGT      Ln3016
		SUB		 r0,r0,r3
        SUB		 r1,r1,r3        
		SUBS     r5,r5,#1
Ln3284
        ;CMP      r5,#0
        BGT      Ln3008
        ADD      sp,sp,#0x164
        LDMFD    sp!,{r4-r11,pc}
;}//
		ENDP
		EXPORT C_Interpolate4_H03V01
;{
C_Interpolate4_H00V02	PROC
		IMPORT RV30_ClampTbl

        STMFD    sp!,{r4-r11,lr}
		SUB		 sp,sp,#12
		MUL		 r4,r2,r3
		SUB		 r4,r4,#1
		STR		 r4,[sp,#4]
		MOV		 r4,r3,LSL #4
		SUB		 r4,r4,#1
		STR		 r4,[sp,#8]
        LDR      r4,=RV30_ClampTbl        
		STR		 r3,[sp,#0]	
        MOV      r5,r3
		ADD		 r3,r2,r2,LSL #1	;3*uSrcPitch
        ;B        Ln3632
Ln3364
        LDR      r6,[sp,#0]
        ;B        Ln3596
Ln3372
        LDRB     r9,[r0,#0]			;lTemp2
        LDRB     r8,[r0,-r2]		;lTemp1
        LDRB     r11,[r0,r2,LSL #1]	;lTemp4
        LDRB     r10,[r0,r2]		;lTemp3
        LDRB     r7,[r0,-r2,LSL #1]	;lTemp0
		LDRB     r12,[r0,r3]		;lTemp5        
		
		ADD		 lr,r9,r10	
		RSB		 lr,r8,lr,LSL #2
		SUB		 lr,lr,r11
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r7
		ADD		 lr,lr,r12
		ADD		 lr,lr,#0x10
        ADD      lr,r4,lr,ASR #5
        LDRB     r7,[lr,#0x120]

		ADD		 lr,r10,r11
		RSB		 lr,r9,lr,LSL #2
        STRB     r7,[r1],#0x10
        LDRB     r7,[r0,r2,LSL #2]	;lTemp0
		SUB		 lr,lr,r12
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r8
		ADD		 lr,lr,r7
		ADD		 lr,lr,#0x10
        ADD      lr,r4,lr,ASR #5
        LDRB     r8,[lr,#0x120]

		ADD		 lr,r11,r12
		RSB		 lr,r10,lr,LSL #2
        STRB     r8,[r1],#0x10
		ADD		 r8,r2,r2,LSL #2
        LDRB     r8,[r0,r8]	;lTemp0		
		SUB		 lr,lr,r7
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r9
		ADD		 lr,lr,r8
		ADD		 lr,lr,#0x10
        ADD      lr,r4,lr,ASR #5
        LDRB     r9,[lr,#0x120]

		ADD		 lr,r12,r7
		RSB		 lr,r11,lr,LSL #2
        STRB     r9,[r1],#0x10
        LDRB     r9,[r0,r3,LSL #1]	;lTemp0		
		SUB		 lr,lr,r8
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r10
		ADD		 lr,lr,r9
		ADD		 lr,lr,#0x10
        ADD      lr,r4,lr,ASR #5
        LDRB     r10,[lr,#0x120]

        ADD      r0,r0,r2,LSL #2
        SUBS     r6,r6,#4
        STRB     r10,[r1],#0x10                
Ln3596
        ;CMP      r6,#0
        BGT      Ln3372
        LDR		 r12,[sp,#4]
		LDR		 lr,[sp,#8]
		SUB		 r0,r0,r12
		SUB		 r1,r1,lr
        SUBS     r5,r5,#1
Ln3632
        ;CMP      r5,#0
        BGT      Ln3364
		ADD		 sp,sp,#12	
        LDMFD    sp!,{r4-r11,pc}
;}//
		ENDP
		EXPORT	C_Interpolate4_H00V02
;{
C_Interpolate4_H01V02	PROC
		IMPORT RV30_ClampTbl

        STMFD    sp!,{r4-r11,lr}
        SUB      sp,sp,#0x164

        STR		 r3,[sp,#0]	
		RSB		 r4,r3,#0x10
		STR		 r4,[sp,#4]		
        LDR      r4,=RV30_ClampTbl
		STR		 r1,[sp,#8]
		SUB      r0,r0,r2,LSL #1
		SUB		 r2,r2,r3
		STR		 r2,[sp,#16]
		MOV		 r2,#52
		ADD      r1,sp,#20
		ADD      r5,r3,#5
        ;B        Ln3980
Ln3696
		LDR		 r6,[sp,#0]
        ;B        Ln3944
Ln3704
        LDRB     r9,[r0,#0]		;lTemp2
        LDRB     r8,[r0,#-1]	;lTemp1
        LDRB     r11,[r0,#2]	;lTemp4
		LDRB     r7,[r0,#-2]	;lTemp0
        LDRB     r10,[r0,#1]	;lTemp3        
		LDRB     r12,[r0,#3]	;lTemp5        
		
		MLA	 r7,r9,r2,r7
		ADD		 lr,r8,r11
		RSB		 lr,lr,r10,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r7
		ADD		 lr,lr,r12
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r3,[lr,#0x120]

		MLA	 r8,r10,r2,r8
		ADD		 lr,r9,r12
        LDRB     r7,[r0,#4]	;lTemp0
		RSB		 lr,lr,r11,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r8
		ADD		 lr,lr,r7
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r8,[lr,#0x120]

		MLA	 r9,r11,r2,r9
		ADD		 lr,r10,r7
		ORR	     r3,r3,r8,LSL #8        
        LDRB     r8,[r0,#5]	;lTemp1
		RSB		 lr,lr,r12,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r9
		ADD		 lr,lr,r8
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r9,[lr,#0x120]

		MLA	 r10,r12,r2,r10
		ADD		 lr,r11,r8
		ORR	     r3,r3,r9,LSL #16        
        LDRB     r9,[r0,#6]	;lTemp2
		RSB		 lr,lr,r7,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r10
		ADD		 lr,lr,r9
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r10,[lr,#0x120]

        ADD      r0,r0,#4
        SUBS     r6,r6,#4
		ORR	     r3,r3,r10,LSL #24
        
		STR      r3,[r1],#4
Ln3944
        ;CMP      r6,#0
        BGT      Ln3704
		LDR		 r7,[sp,#4]
		LDR		 lr,[sp,#16]
		ADD		 r1,r1,r7		
		ADD		 r0,r0,lr
		SUBS     r5,r5,#1
Ln3980        
        ;CMP      r5,#0
        BGT      Ln3696

		LDR		 r3,[sp,#0]
		ADD      r0,sp,#20
		LDR		 r1,[sp,#8]
		MOV		 r2,r3,LSL #4
		SUB		 r2,r2,#1	
		MOV		 r5,r3	 
        ;B       Ln4276
Ln4016
        MOV		 r6,r3
        ;B       Ln4252
Ln4024
        LDRB     r9,[r0,#0x20]		;lTemp2
        LDRB     r8,[r0,#0x10]		;lTemp1
        LDRB     r11,[r0,#0x40]		;lTemp4
        LDRB     r10,[r0,#0x30]		;lTemp3
        LDRB     r7,[r0,#0]			;lTemp0
		LDRB     r12,[r0,#0x50]		;lTemp5        
		
		ADD		 lr,r9,r10
		RSB		 lr,r8,lr,LSL #2
		SUB		 lr,lr,r11
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r7
		ADD		 lr,lr,r12
		ADD		 lr,lr,#0x10
        ADD      lr,r4,lr,ASR #5
        LDRB     r7,[lr,#0x120]

		ADD		 lr,r10,r11
		RSB		 lr,r9,lr,LSL #2
        STRB     r7,[r1],#0x10
        LDRB     r7,[r0,#0x60]	;lTemp0
		SUB		 lr,lr,r12
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r8		
		ADD		 lr,lr,r7
		ADD		 lr,lr,#0x10
        ADD      lr,r4,lr,ASR #5
        LDRB     r8,[lr,#0x120]

		ADD		 lr,r11,r12
		RSB		 lr,r10,lr,LSL #2
        STRB     r8,[r1],#0x10
        LDRB     r8,[r0,#0x70]			;lTemp1
		SUB		 lr,lr,r7
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r9		
		ADD		 lr,lr,r8
		ADD		 lr,lr,#0x10
        ADD      lr,r4,lr,ASR #5
        LDRB     r9,[lr,#0x120]

		ADD		 lr,r12,r7
		RSB		 lr,r11,lr,LSL #2
        STRB     r9,[r1],#0x10
        LDRB     r9,[r0,#0x80]			;lTemp2
		SUB		 lr,lr,r8
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r10		
		ADD		 lr,lr,r9
		ADD		 lr,lr,#0x10
        ADD      lr,r4,lr,ASR #5
        LDRB     r10,[lr,#0x120]

		ADD      r0,r0,#0x40
        SUBS     r6,r6,#4
        STRB     r10,[r1],#0x10
Ln4252
        ;CMP      r6,#0
        BGT      Ln4024		
		SUB		 r0,r0,r2
        SUB		 r1,r1,r2        
		SUBS     r5,r5,#1
Ln4276
        ;CMP      r5,#0
        BGT      Ln4016
        ADD      sp,sp,#0x164
        LDMFD    sp!,{r4-r11,pc}
;}//
		ENDP
		EXPORT	C_Interpolate4_H01V02

