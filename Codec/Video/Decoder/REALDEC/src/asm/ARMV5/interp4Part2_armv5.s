
	AREA interp4Part2, CODE, READONLY
	;{
C_Interpolate4_H02V02	PROC
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
		ADD      r1,sp,#16
		ADD      r5,r3,#5
        ;B        Ln4604
Ln4336
		LDR		 r6,[sp,#0]
        ;B        Ln4568
Ln4344
        ;B        Ln3980
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
Ln4568
        ;CMP      r6,#0
        BGT      Ln4344

		LDR		 lr,[sp,#4]
		ADD		 r0,r0,r2
		ADD		 r1,r1,lr		
        SUBS     r5,r5,#1
Ln4604
        ;CMP      r5,#0
        BGT      Ln4336

		LDR		 r3,[sp,#0]
		ADD      r0,sp,#16
		LDR		 r1,[sp,#8]
		MOV		 r2,r3,LSL #4
		SUB		 r2,r2,#1
		MOV		 r5,r3
        ;B        Ln4900
Ln4640
        MOV		 r6,r3
        ;B        Ln4876
Ln4648
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
Ln4876
        ;CMP      r6,#0
        BGT      Ln4648
        SUBS     r5,r5,#1
		SUB		 r0,r0,r2
        SUB		 r1,r1,r2
Ln4900
        ;CMP      r5,#0
        BGT      Ln4640
        ADD      sp,sp,#0x160
        LDMFD    sp!,{r4-r11,pc}
;}//
		ENDP
		EXPORT	C_Interpolate4_H02V02

;{
C_Interpolate4_H03V02	PROC
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
        ;B        Ln5244
Ln4960
		LDR		 r6,[sp,#0]
        ;B        Ln5208
Ln4968
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
Ln5208
        ;CMP      r6,#0
        BGT      Ln4968
		LDR		 r7,[sp,#4]
		LDR		 lr,[sp,#16]
		ADD		 r1,r1,r7				
		ADD		 r0,r0,lr
        SUBS     r5,r5,#1
Ln5244        
        ;CMP      r5,#0
        BGT      Ln4960

		LDR		 r3,[sp,#0]
		ADD      r0,sp,#20
		LDR		 r1,[sp,#8]
		MOV		 r2,r3,LSL #4
		SUB		 r2,r2,#1	
		MOV		 r5,r3	
        ;B        Ln5540
Ln5280
        MOV		 r6,r3
        ;B        Ln5516
Ln5288
        LDRB     r9,[r0,#0x20]		;lTemp2
        LDRB     r8,[r0,#0x10]		;lTemp1
        LDRB     r11,[r0,#0x40]		;lTemp4
		LDRB     r7,[r0,#0]			;lTemp0
        LDRB     r10,[r0,#0x30]		;lTemp3        
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
Ln5516
        ;CMP      r6,#0
        BGT      Ln5288
		SUB		 r0,r0,r2
        SUB		 r1,r1,r2        
		SUBS     r5,r5,#1
Ln5540
        CMP      r5,#0
        BGT      Ln5280
        ADD      sp,sp,#0x164
        LDMFD    sp!,{r4-r11,pc}
;}//
		ENDP
		EXPORT	C_Interpolate4_H03V02
		


;{
C_Interpolate4_H00V03	PROC
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
		;B        Ln5904
Ln5620
        LDR      r6,[sp,#12]
        ;B        Ln5868
Ln5628
        LDRB     r9,[r0,#0]			;lTemp2
        LDRB     r8,[r0,-r2]		;lTemp1
        LDRB     r11,[r0,r2,LSL #1]	;lTemp4
        LDRB     r10,[r0,r2]		;lTemp3
        LDRB     r7,[r0,-r2,LSL #1]	;lTemp0
		ADD		 lr,r2,r2,LSL #1
		LDRB     r12,[r0,lr]		;lTemp5        

		MLA	 r7,r3,r10,r7
		ADD		 lr,r8,r11
		RSB		 lr,lr,r9,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r7
		ADD		 lr,lr,r12
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r7,[lr,#0x120]

		MLA	 r8,r3,r11,r8
		ADD		 lr,r9,r12
		RSB		 lr,lr,r10,LSL #2
        STRB     r7,[r1],#0x10
        LDRB     r7,[r0,r2,LSL #2]	;lTemp0
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r8
		ADD		 lr,lr,r7
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r8,[lr,#0x120]
		
		MLA	 r9,r3,r12,r9
		ADD		 lr,r2,r2,LSL #2
        STRB     r8,[r1],#0x10
        LDRB     r8,[r0,lr]			;lTemp1
		ADD		 lr,r10,r7
		RSB		 lr,lr,r11,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r9
		ADD		 lr,lr,r8
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r9,[lr,#0x120]

		MLA	 r10,r3,r7,r10
		ADD		 lr,r2,r2,LSL #1
        STRB     r9,[r1],#0x10
        LDRB     r9,[r0,lr,LSL #1]		;lTemp2
		ADD		 lr,r11,r8
		RSB		 lr,lr,r12,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r10
		ADD		 lr,lr,r9
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r10,[lr,#0x120]

		ADD      r0,r0,r2,lsl #2
        SUBS     r6,r6,#4
        STRB     r10,[r1],#0x10
Ln5868
        ;CMP      r6,#0
        BGT      Ln5628
		LDR		 r7,[sp,#8]
		LDR		 lr,[sp,#4]	
		SUB		 r0,r0,r7	  		
        SUB		 r1,r1,lr
        SUBS     r5,r5,#1
Ln5904
        ;CMP      r5,#0
        BGT      Ln5620
		ADD      sp,sp,#0x10
        LDMFD    sp!,{r4-r11,pc}
;}//
		ENDP
		EXPORT	C_Interpolate4_H00V03

;{
C_Interpolate4_H01V03	PROC
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
        ;B        Ln6252
Ln5968
		LDR		 r6,[sp,#0]
        ;B        Ln6216
Ln5976        
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
Ln6216
        ;CMP      r6,#0
        BGT      Ln5976
		LDR		 r7,[sp,#4]
		LDR		 lr,[sp,#16]
		ADD		 r1,r1,r7
		ADD		 r0,r0,lr
		SUBS     r5,r5,#1		
Ln6252
        ;CMP      r5,#0
        BGT      Ln5968

		LDR		 r5,[sp,#0]
		ADD      r0,sp,#20
		LDR		 r1,[sp,#8]
		MOV		 r3,r5,LSL #4
		SUB		 r3,r3,#1	
        ;B        Ln6564
Ln6288
        LDR		 r6,[sp,#0]
        ;B        Ln6540
Ln6296
        LDRB     r9,[r0,#0x20]		;lTemp2
        LDRB     r8,[r0,#0x10]		;lTemp1
        LDRB     r11,[r0,#0x40]		;lTemp4
		LDRB     r7,[r0,#0]			;lTemp0
        LDRB     r10,[r0,#0x30]		;lTemp3        
		LDRB     r12,[r0,#0x50]		;lTemp5        
		
		MLA	 r7,r10,r2,r7
		ADD		 lr,r8,r11
		RSB		 lr,lr,r9,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r7
		ADD		 lr,lr,r12
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r7,[lr,#0x120]

		MLA	 r8,r11,r2,r8
		ADD		 lr,r9,r12
        STRB     r7,[r1],#0x10
        LDRB     r7,[r0,#0x60]	;lTemp0
		RSB		 lr,lr,r10,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r8
		ADD		 lr,lr,r7
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r8,[lr,#0x120]

		MLA	 r9,r12,r2,r9
		ADD		 lr,r10,r7
        STRB     r8,[r1],#0x10
        LDRB     r8,[r0,#0x70] ;lTemp1
		RSB		 lr,lr,r11,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r9
		ADD		 lr,lr,r8
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r9,[lr,#0x120]

		MLA	 r10,r7,r2,r10
		ADD		 lr,r11,r8
        STRB     r9,[r1],#0x10
        LDRB     r9,[r0,#0x80] ;lTemp2
		RSB		 lr,lr,r12,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r10
		ADD		 lr,lr,r9
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r10,[lr,#0x120]

		ADD      r0,r0,#0x40
        SUBS     r6,r6,#4
        STRB     r10,[r1],#0x10
Ln6540
        ;CMP      r6,#0
        BGT      Ln6296		
		SUB		 r0,r0,r3
        SUB		 r1,r1,r3
		SUBS     r5,r5,#1
Ln6564
        ;CMP      r5,#0
        BGT      Ln6288
        ADD      sp,sp,#0x164
        LDMFD    sp!,{r4-r11,pc}
;}//
		ENDP
		EXPORT	C_Interpolate4_H01V03

;{
C_Interpolate4_H02V03	PROC
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
		ADD      r1,sp,#16
		ADD      r5,r3,#5
        ;B        Ln6892
Ln6624
		LDR		 r6,[sp,#0]
        ;B        Ln6856
Ln6632
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
Ln6856
        ;CMP      r6,#0
        BGT      Ln6632
		LDR		 lr,[sp,#4]
		ADD		 r0,r0,r2
		ADD		 r1,r1,lr		
        SUBS     r5,r5,#1
Ln6892
        ;CMP      r5,#0
        BGT      Ln6624

		LDR		 r5,[sp,#0]
		ADD      r0,sp,#16
		LDR		 r1,[sp,#8]
		MOV		 r3,r5,LSL #4
		SUB		 r3,r3,#1
		MOV		 r2,#52	
        ;B        Ln7204
Ln6928
        LDR		 r6,[sp,#0]
        ;B        Ln7180
Ln6936
        LDRB     r9,[r0,#0x20]		;lTemp2
        LDRB     r8,[r0,#0x10]		;lTemp1
        LDRB     r11,[r0,#0x40]		;lTemp4
        LDRB     r10,[r0,#0x30]		;lTemp3
        LDRB     r7,[r0,#0]			;lTemp0
		LDRB     r12,[r0,#0x50]		;lTemp5        
		
		MLA	 r7,r10,r2,r7
		ADD		 lr,r8,r11
		RSB		 lr,lr,r9,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r7
		ADD		 lr,lr,r12
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r7,[lr,#0x120]

		MLA	 r8,r11,r2,r8
		ADD		 lr,r9,r12
        STRB     r7,[r1],#0x10
        LDRB     r7,[r0,#0x60]	;lTemp0
		RSB		 lr,lr,r10,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r8
		ADD		 lr,lr,r7
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r8,[lr,#0x120]

		MLA	 r9,r12,r2,r9
		ADD		 lr,r10,r7
        STRB     r8,[r1],#0x10
        LDRB     r8,[r0,#0x70] ;lTemp1
		RSB		 lr,lr,r11,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r9
		ADD		 lr,lr,r8
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r9,[lr,#0x120]

		MLA	 r10,r7,r2,r10
		ADD		 lr,r11,r8
        STRB     r9,[r1],#0x10
        LDRB     r9,[r0,#0x80] ;lTemp2
		RSB		 lr,lr,r12,LSL #2
		ADD		 lr,lr,lr,LSL #2
		ADD		 lr,lr,r10
		ADD		 lr,lr,r9
		ADD		 lr,lr,#0x20
        ADD      lr,r4,lr,ASR #6
        LDRB     r10,[lr,#0x120]

		ADD      r0,r0,#0x40
        SUBS     r6,r6,#4
        STRB     r10,[r1],#0x10
Ln7180
        ;CMP      r6,#0
        BGT      Ln6936        
		SUB		 r0,r0,r3
        SUB		 r1,r1,r3
		SUBS     r5,r5,#1
Ln7204
        ;CMP      r5,#0
        BGT      Ln6928
        ADD      sp,sp,#0x160
        LDMFD    sp!,{r4-r11,pc}
;}//
		ENDP
		EXPORT	C_Interpolate4_H02V03

;{
C_Interpolate4_H03V03	PROC
		IMPORT RV30_ClampTbl

        STMFD    sp!,{r4-r11,lr}
        LDR      r4,=RV30_ClampTbl
        MOV      r5,r3
        ;B        Ln7444
Ln7248
        MOV      r6,r3
        ;B        Ln7420
Ln7256
		LDRB     r7,[r0,#0]		;lTemp0
		LDRB     r8,[r0,#1]		;lTemp1
		ADD		 r11,r0,r2
        LDRB     r9,[r11,#0]	;lTemp2
        LDRB     r10,[r11,#1]	;lTemp3
		
		ADD		 lr,r7,r8
		ADD		 lr,lr,r9
		ADD		 lr,lr,r10
		ADD		 lr,lr,#2
        ADD      lr,r4,lr,ASR #2
        LDRB     r12,[lr,#0x120]

		LDRB     r7,[r0,#2]		;lTemp0
		LDRB     r9,[r11,#2]	;lTemp2
		ADD		 lr,r10,r8		
		ADD		 lr,lr,r7
		ADD		 lr,lr,r9
		ADD		 lr,lr,#2
        ADD      lr,r4,lr,ASR #2
        LDRB     lr,[lr,#0x120]

		LDRB     r8,[r0,#3]		;lTemp1
		LDRB     r10,[r11,#3]	;lTemp3
		ORR		 r12,r12,lr,LSL #8
		ADD		 lr,r7,r9
		ADD		 lr,lr,r8
		ADD		 lr,lr,r10
		ADD		 lr,lr,#2
        ADD      lr,r4,lr,ASR #2
        LDRB     lr,[lr,#0x120]

		LDRB     r7,[r0,#4]		;lTemp0
		LDRB     r9,[r11,#4]	;lTemp2
		ORR		 r12,r12,lr,LSL #16
		ADD		 lr,r10,r8
		ADD		 lr,lr,r7
		ADD		 lr,lr,r9
		ADD		 lr,lr,#2
        ADD      lr,r4,lr,ASR #2
        LDRB     lr,[lr,#0x120]

		SUBS	 r6,r6,#4
		ADD      r0,r0,#4
		ORR		 r12,r12,lr,LSL #24
        STR      r12,[r1],#4
Ln7420
        ;CMP      r6,#0
        BGT      Ln7256
        ADD		 r1,r1,#16
        SUB		 r1,r1,r3
        ADD		 r0,r0,r2
        SUB		 r0,r0,r3
        SUBS     r5,r5,#1
Ln7444
        ;CMP      r5,#0
        BGT      Ln7248
        LDMFD    sp!,{r4-r11,pc}
;}//
		ENDP
		EXPORT	C_Interpolate4_H03V03

;{
C_MCCopyChroma4_H00V00	PROC
		STMFD    sp!,{r4,lr}
		ANDS    r3,r0,#0x03
		BNE		Lnc001
		
		LDR		r3,[r0],r2
		LDR		r12,[r0],r2
		STR		r3,[r1],#0x10
		STR		r12,[r1],#0x10

		LDR		r3,[r0],r2
		LDR		r12,[r0],r2
		STR		r3,[r1],#0x10
		STR		r12,[r1],#0x10
		B		Lnc002

Lnc001	SUB		r0,r0,r3
		MOV		r3,r3,LSL #3
		RSB		r12,r3,#0x20
		SUB		r2,r2,#0x04

		LDR		r4,[r0],#0x04
		LDR		lr,[r0],r2
		MOV		r4,r4,LSR r3
		ORR		r4,r4,lr,LSL r12
		STR		r4,[r1],#0x10

		LDR		r4,[r0],#0x04
		LDR		lr,[r0],r2
		MOV		r4,r4,LSR r3
		ORR		r4,r4,lr,LSL r12
		STR		r4,[r1],#0x10

		LDR		r4,[r0],#0x04
		LDR		lr,[r0],r2
		MOV		r4,r4,LSR r3
		ORR		r4,r4,lr,LSL r12
		STR		r4,[r1],#0x10

		LDR		r4,[r0],#0x04
		LDR		lr,[r0],r2
		MOV		r4,r4,LSR r3
		ORR		r4,r4,lr,LSL r12
		STR		r4,[r1],#0x10

Lnc002	LDMFD   sp!,{r4,pc}
;}//
		ENDP
		EXPORT	C_MCCopyChroma4_H00V00
		
;{
C_MCCopyChroma4_H01V00	PROC
        STMFD    sp!,{r4-r7,lr}
		MOV		 lr,#3
		MOV		 r12,#1
        MOV      r3,#4
Ln7524
        LDRB     r4,[r0,#0]
        LDRB     r5,[r0,#1]

        SUBS     r3,r3,#1

		MLA	 r4,lr,r4,r12
		LDRB     r6,[r0,#2]
		ADD		 r4,r4,r5
		MOV		 r7,r4,ASR #2
			
		MLA	 r5,lr,r5,r12
		LDRB     r4,[r0,#3]
		ADD		 r5,r5,r6
		MOV		 r5,r5,ASR #2
		ORR		 r7,r7,r5,LSL #8

		MLA	 r6,lr,r6,r12
		LDRB     r5,[r0,#4]
		ADD		 r6,r6,r4
		MOV		 r6,r6,ASR #2
		ORR		 r7,r7,r6,LSL #16

		MLA	 r4,lr,r4,r12		
		ADD      r0,r0,r2
		ADD		 r4,r4,r5
		MOV		 r4,r4,ASR #2
		ORR		 r7,r7,r4,LSL #24
        STR      r7,[r1],#0x10

        BGT      Ln7524
        LDMFD    sp!,{r4-r7,pc}
;}//
		ENDP
		EXPORT	C_MCCopyChroma4_H01V00
;{
C_MCCopyChroma4_H02V00	PROC
        STMFD    sp!,{r4,r5,lr}
        MOV      r3,#4
Ln7652
        LDRB     r4,[r0,#0]
        LDRB     r5,[r0,#1]
		LDRB     lr,[r0,#2]

		ADD		 r4,r4,#1
		ADD		 r4,r4,r5
		MOV		 r12,r4,ASR #1
		
		LDRB	 r4,[r0,#3]	
		ADD		 r5,r5,#1
		ADD		 r5,r5,lr
		MOV		 r5,r5,ASR #1
		ORR		 r12,r12,r5,LSL #8

		LDRB	 r5,[r0,#4]	
		ADD		 lr,lr,#1
		ADD		 lr,lr,r4
		MOV		 lr,lr,ASR #1
		ORR		 r12,r12,lr,LSL #16

		ADD		 r4,r4,#1
		ADD		 r4,r4,r5
		MOV		 r4,r4,ASR #1
		ORR		 r12,r12,r4,LSL #24		       
        STR      r12,[r1],#0x10
		ADD      r0,r0,r2
		SUBS     r3,r3,#1
        BGT      Ln7652
        LDMFD    sp!,{r4,r5,pc}
;}//
		ENDP
		EXPORT	C_MCCopyChroma4_H02V00
;{
C_MCCopyChroma4_H03V00 
        STMFD    sp!,{r4-r8,lr}
		MOV		 lr,#3
		MOV		 r12,#1
        MOV      r3,#4
Ln7764
		LDRB     r5,[r0,#1]
        LDRB     r4,[r0,#0]        

        SUBS     r3,r3,#1

		MLA	 r8,lr,r5,r12
		LDRB     r6,[r0,#2]
		ADD		 r4,r4,r8
		MOV		 r7,r4,ASR #2
			
		MLA	 r8,lr,r6,r12
		LDRB     r4,[r0,#3]
		ADD		 r5,r5,r8
		MOV		 r5,r5,ASR #2
		ORR		 r7,r7,r5,LSL #8

		MLA	 r8,lr,r4,r12
		LDRB     r5,[r0,#4]
		ADD		 r6,r6,r8
		MOV		 r6,r6,ASR #2
		ORR		 r7,r7,r6,LSL #16

		MLA	 r8,lr,r5,r12		
		ADD      r0,r0,r2
		ADD		 r4,r4,r8
		MOV		 r4,r4,ASR #2
		ORR		 r7,r7,r4,LSL #24
        STR      r7,[r1],#0x10
        BGT      Ln7764
        LDMFD    sp!,{r4-r8,pc}
;}//
		ENDP
		EXPORT	C_MCCopyChroma4_H03V00
;{
C_MCCopyChroma4_H00V01	PROC
        STMFD    sp!,{r4-r7,lr}
		ADD		 r7,r2,r2,LSL #1
		MOV		 lr,#3
		MOV		 r12,#2
        MOV      r3,#4
Ln7896
        LDRB     r4,[r0,#0]
        LDRB     r5,[r0,r2]

        SUBS     r3,r3,#1

		MLA	 r4,lr,r4,r12
		LDRB     r6,[r0,r2,LSL #1]
		ADD		 r4,r4,r5
		MOV		 r4,r4,ASR #2
		STRB	 r4,[r1,#0]			
			
		MLA	 r5,lr,r5,r12
		LDRB     r4,[r0,r7]
		ADD		 r5,r5,r6
		MOV		 r5,r5,ASR #2
		STRB	 r5,[r1,#0x10]

		MLA	 r6,lr,r6,r12
		LDRB     r5,[r0,r2,LSL #2]
		ADD		 r6,r6,r4
		MOV		 r6,r6,ASR #2
		STRB	 r6,[r1,#0x20]

		MLA	 r4,lr,r4,r12		
		ADD      r0,r0,#1
		ADD		 r4,r4,r5
		MOV		 r4,r4,ASR #2		
        STRB	 r4,[r1,#0x30]
		ADD		 r1,r1,#1
        BGT      Ln7896
        LDMFD    sp!,{r4-r7,pc}
;};//
		ENDP	
		EXPORT	C_MCCopyChroma4_H00V01
;{
C_MCCopyChroma4_H00V02	PROC
        STMFD    sp!,{r4,r5,lr}
		ADD		 r12,r2,r2,LSL #1
        MOV      r3,#4
Ln8032
        LDRB     r4,[r0,#0]
        LDRB     r5,[r0,r2]
		LDRB     lr,[r0,r2,LSL #1]
		
		SUBS     r3,r3,#1
		ADD		 r4,r4,r5
		MOV		 r4,r4,ASR #1
		STRB	 r4,[r1,#0]
		
		LDRB	 r4,[r0,r12]			
		ADD		 r5,r5,lr
		MOV		 r5,r5,ASR #1
		STRB	 r5,[r1,#0x10]

		LDRB	 r5,[r0,r2,LSL #2]			
		ADD		 lr,lr,r4
		MOV		 lr,lr,ASR #1
		STRB	 lr,[r1,#0x20]
		
		ADD		 r4,r4,r5
		MOV		 r4,r4,ASR #1
		STRB	 r4,[r1,#0x30]
		ADD		 r1,r1,#1
		ADD      r0,r0,#1		
        BGT      Ln8032
        LDMFD    sp!,{r4,r5,pc}
;};//
		ENDP
		EXPORT	C_MCCopyChroma4_H00V02
;{
C_MCCopyChroma4_H00V03	PROC
        STMFD    sp!,{r4-r8,lr}
		ADD		 r7,r2,r2,LSL #1
		MOV		 lr,#3
		MOV		 r12,#2
        MOV      r3,#4
Ln8136
		LDRB     r5,[r0,r2]
        LDRB     r4,[r0,#0]        

        SUBS     r3,r3,#1

		MLA	 r8,lr,r5,r12
		LDRB     r6,[r0,r2,LSL #1]
		ADD		 r4,r4,r8
		MOV		 r4,r4,ASR #2
		STRB	 r4,[r1,#0]
			
		MLA	 r8,lr,r6,r12
		LDRB     r4,[r0,r7]
		ADD		 r5,r5,r8
		MOV		 r5,r5,ASR #2
		STRB	 r5,[r1,#0x10]

		MLA	 r8,lr,r4,r12
		LDRB     r5,[r0,r2,LSL #2]
		ADD		 r6,r6,r8
		MOV		 r6,r6,ASR #2
		STRB	 r6,[r1,#0x20]

		MLA	 r8,lr,r5,r12		
		ADD      r0,r0,#1
		ADD		 r4,r4,r8
		MOV		 r4,r4,ASR #2
		STRB	 r4,[r1,#0x30]
		ADD		 r1,r1,#1
        BGT      Ln8136
        LDMFD    sp!,{r4-r8,pc}
;};//C_MCCopyChroma4_H00V03
		ENDP
		EXPORT	C_MCCopyChroma4_H00V03
;{
C_MCCopyChroma4_H01V01	PROC
        STMFD    sp!,{r4-r9,lr}
		MOV		 lr,#9		
		MOV		 r12,#7
        ADD      r3,r0,r2
        MOV      r4,#4
Ln8272
		LDRB     r5,[r0,#0]	;lTemp0				
        LDRB     r6,[r0,#1]	;lTemp1
		LDRB     r7,[r3,#0]	;lTemp2
		LDRB     r8,[r3,#1]	;lTemp3		
		
		MLA	 r5,lr,r5,r12
		ADD		 r7,r6,r7			 
		ADD	     r7,r7,r7,LSL #1	
		ADD		 r5,r7,r5
		ADD		 r5,r5,r8		
		MOV		 r9,r5,ASR #4	

		LDRB	 r5,[r0,#2]	
		LDRB	 r7,[r3,#2]	
		MLA	 r6,lr,r6,r12
		ADD		 r8,r5,r8			 
		ADD	     r8,r8,r8,LSL #1	
		ADD		 r6,r8,r6
		ADD		 r6,r6,r7		
		MOV		 r6,r6,ASR #4	
		ORR		 r9,r9,r6,LSL #8

		LDRB	 r6,[r0,#3]	
		LDRB	 r8,[r3,#3]	
		MLA	 r5,lr,r5,r12
		ADD		 r7,r6,r7			 
		ADD	     r7,r7,r7,LSL #1	
		ADD		 r5,r7,r5
		ADD		 r5,r5,r8		
		MOV		 r5,r5,ASR #4	
		ORR		 r9,r9,r5,LSL #16

		LDRB	 r5,[r0,#4]	
		LDRB	 r7,[r3,#4]	
		MLA	 r6,lr,r6,r12
		ADD		 r8,r5,r8			 
		ADD	     r8,r8,r8,LSL #1	
		ADD		 r6,r8,r6
		ADD		 r6,r6,r7		
		MOV		 r6,r6,ASR #4	
		ORR		 r9,r9,r6,LSL #24

        STR      r9,[r1],#0x10
        ADD      r3,r3,r2
        ADD      r0,r0,r2
		SUBS	 r4,r4,#1	
        BGT      Ln8272
        LDMFD    sp!,{r4-r9,pc}
;};//
		ENDP
		EXPORT	C_MCCopyChroma4_H01V01
;{
C_MCCopyChroma4_H02V01	PROC

        STMFD    sp!,{r4-r9,lr}
		MOV		 lr,#3		
		MOV		 r12,#4		
        ADD      r3,r0,r2
        MOV      r4,#4
Ln8476
		LDRB     r5,[r0,#0]	;lTemp0				
        LDRB     r6,[r0,#1]	;lTemp1
		LDRB     r7,[r3,#0]	;lTemp2
		LDRB     r8,[r3,#1]	;lTemp3		
		
		ADD		 r5,r5,r6			
		MLA	 r5,lr,r5,r12
		ADD		 r7,r7,r8
		ADD		 r7,r7,r5		

		LDRB	 r5,[r0,#2]	
		MOV		 r9,r7,ASR #3	
		LDRB	 r7,[r3,#2]			
		ADD		 r6,r6,r5			
		MLA	 r6,lr,r6,r12
		ADD		 r8,r8,r7
		ADD		 r8,r8,r6
		MOV		 r8,r8,ASR #3			
		ORR		 r9,r9,r8,LSL #8

		LDRB	 r6,[r0,#3]			
		LDRB	 r8,[r3,#3]	
		SUBS	 r4,r4,#1	
		ADD		 r5,r5,r6			
		MLA	 r5,lr,r5,r12
		ADD		 r7,r7,r8
		ADD		 r7,r7,r5
		MOV		 r7,r7,ASR #3			
		ORR		 r9,r9,r7,LSL #16

		LDRB	 r5,[r0,#4]			
		LDRB	 r7,[r3,#4]	
		ADD		 r0,r0,r2		
		ADD		 r6,r6,r5			
		MLA	 r6,lr,r6,r12
		ADD		 r8,r8,r7
		ADD		 r8,r8,r6
		MOV		 r8,r8,ASR #3			
		ORR		 r9,r9,r8,LSL #24

        STR      r9,[r1],#0x10
        ADD      r3,r3,r2
        BGT      Ln8476
        LDMFD    sp!,{r4-r9,pc}
;};//
		ENDP
		EXPORT	C_MCCopyChroma4_H02V01

;{
C_MCCopyChroma4_H03V01	PROC
        STMFD    sp!,{r4-r8,lr}
        ADD      r3,r0,r2
        MOV      r12,#4
Ln8664
        LDRB     lr,[r0,#0]
        LDRB     r6,[r3,#1]
        LDRB     r4,[r0,#1]
        LDRB     r5,[r3,#0]

        ADD      lr,lr,r6
        ADD      lr,lr,lr,LSL #1
        ADD      r7,r4,r4,LSL #3
        ADD      lr,r7,lr
        ADD      lr,lr,r5
        ADD      lr,lr,#7
        MOV      r7,lr,LSR #4

        LDRB     r5,[r3,#2]
        LDRB     lr,[r0,#2]
        SUBS     r12,r12,#1
        ADD      r4,r4,r5
        ADD      r4,r4,r4,LSL #1
        ADD      r8,lr,lr,LSL #3
        ADD      r4,r8,r4
        ADD      r4,r4,r6
        ADD      r4,r4,#7
        MOV      r4,r4,LSR #4
		LDRB     r6,[r3,#3]
        ORR      r7,r7,r4,LSL #8
        
        LDRB     r4,[r0,#3]        
        ADD      lr,lr,r6
        ADD      lr,lr,lr,LSL #1
        ADD      r8,r4,r4,LSL #3
        ADD      lr,r8,lr
        ADD      lr,lr,r5
		ADD      lr,lr,#7        
        MOV      lr,lr,LSR #4
        ORR      lr,r7,lr,LSL #16

        LDRB     r7,[r3,#4]
        LDRB     r5,[r0,#4]
        ADD      r0,r0,r2
        ADD      r4,r4,r7
        ADD      r4,r4,r4,LSL #1
        ADD      r5,r5,r5,LSL #3
        ADD      r4,r5,r4
        ADD      r4,r4,r6
        ADD      r4,r4,#7
        MOV      r4,r4,LSR #4
        ORR      lr,lr,r4,LSL #24

        STR      lr,[r1],#0x10
        ADD      r3,r3,r2
        BGT      Ln8664
        LDMFD    sp!,{r4-r8,pc}
;};//C_MCCopyChroma4_H03V01
		ENDP
		EXPORT	C_MCCopyChroma4_H03V01





