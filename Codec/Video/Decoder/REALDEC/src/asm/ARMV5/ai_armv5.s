
		AREA AI, CODE, READONLY
	;void RV_CDECL C_PlanarPredict(
	;PU8 pBlock,		/* pointer to upper left pel of block */
	;U32 uPitch,		/* of reference plane */
	;U8 *pPredBuf	/* predictors go here */
	;)      
C_PlanarPredict PROC
   IMPORT	 RV30_ClampTbl
        STMFD    sp!,{r4-r9,lr}
		SUB		 sp,sp,#0x20
		MOV		 r3,sp			;topp
		ADD		 r4,sp,#0x10	;leftp

		SUB		 r6,r0,r1		;(pBlock-uPitch)
		LDRB	 r5,[r6,#-1]	;t0
	
		LDR		 r7,[r6,#0]
		LDR		 r8,[r6,#4]
		STR		 r7,[r3,#0]		;topp[0]
		STR		 r8,[r3,#4]		;topp[1]
		LDR		 r7,[r6,#8]
		LDR		 r8,[r6,#12]
		STR		 r7,[r3,#8]		;topp[2]
		STR		 r8,[r3,#12]	;topp[3]
		
		SUB		 r6,r0,#1  		;pp = pBlock - 1;
		
		MOV		 r12,#4
loop1	LDRB	 r7,[r6],r1
		LDRB	 r8,[r6],r1
		LDRB	 r9,[r6],r1
		LDRB	 lr,[r6],r1
		ORR		 r7,r7,r8,LSL #8
		ORR		 r7,r7,r9,LSL #16
		ORR		 r7,r7,lr,LSL #24
		STR		 r7,[r4],#4
		SUBS	 r12,r12,#1
		BGT		 loop1
								
		
		LDRB	 r4,[r3,#8]		;r12=iH,r3=tp
		LDRB	 r6,[r3,#6]
		LDRB	 r7,[r3,#9]
		LDRB	 r8,[r3,#5]
		SUB		 r12,r4,r6
		SUB		 r9,r7,r8
		ADD		 r12,r12,r9,LSL #1

		LDRB	 r4,[r3,#10]
		LDRB	 r6,[r3,#4]
		LDRB	 r7,[r3,#11]
		LDRB	 r8,[r3,#3]
		SUB		 r9,r4,r6
		ADD		 r9,r9,r9,LSL #1
		ADD		 r12,r12,r9
		SUB		 r9,r7,r8
		ADD		 r12,r12,r9,LSL #2

		LDRB	 r4,[r3,#12]
		LDRB	 r6,[r3,#2]
		LDRB	 r7,[r3,#13]
		LDRB	 r8,[r3,#1]
		SUB		 r9,r4,r6
		ADD		 r9,r9,r9,LSL #2
		ADD		 r12,r12,r9
		SUB		 r9,r7,r8
		ADD		 r8,r9,r9,LSL #2
		ADD		 r9,r9,r8
		ADD		 r12,r12,r9

		LDRB	 r4,[r3,#14]
		LDRB	 r6,[r3,#0]
		LDRB	 r7,[r3,#15]
		SUB		 r9,r4,r6
		RSB		 r9,r9,r9,LSL #3
		ADD		 r12,r12,r9
		SUB		 r9,r7,r5
		ADD		 r12,r12,r9,LSL #3


		ADD		 r3,sp,#0x10
		LDRB	 r4,[r3,#8]		;lr=iV,r3=lp
		LDRB	 r6,[r3,#6]
		LDRB	 r7,[r3,#9]
		LDRB	 r8,[r3,#5]
		SUB		 lr,r4,r6
		SUB		 r9,r7,r8
		ADD		 lr,lr,r9,LSL #1

		LDRB	 r4,[r3,#10]
		LDRB	 r6,[r3,#4]
		LDRB	 r7,[r3,#11]
		LDRB	 r8,[r3,#3]
		SUB		 r9,r4,r6
		ADD		 r9,r9,r9,LSL #1
		ADD		 lr,lr,r9
		SUB		 r9,r7,r8
		ADD		 lr,lr,r9,LSL #2

		LDRB	 r4,[r3,#12]
		LDRB	 r6,[r3,#2]
		LDRB	 r7,[r3,#13]
		LDRB	 r8,[r3,#1]
		SUB		 r9,r4,r6
		ADD		 r9,r9,r9,LSL #2
		ADD		 lr,lr,r9
		SUB		 r9,r7,r8
		ADD		 r8,r9,r9,LSL #2
		ADD		 r9,r9,r8
		ADD		 lr,lr,r9

		LDRB	 r4,[r3,#14]
		LDRB	 r6,[r3,#0]
		LDRB	 r7,[r3,#15]		
		SUB		 r9,r4,r6
		RSB		 r9,r9,r9,LSL #3
		ADD		 lr,lr,r9
		SUB		 r9,r7,r5
		ADD		 lr,lr,r9,LSL #3

		LDRB	 r8,[sp,#15]		;tp[15]
		ADD		 r3,r12,r12,ASR #2	
		MOV		 r3,r3,ASR #4		;b
		ADD		 r4,lr,lr,ASR #2
		MOV		 r4,r4,ASR #4		;c

		ADD		 r8,r8,r7			;a
		ADD		 r5,r3,r4
		RSB		 r5,r5,r5,LSL #3
		RSB		 r5,r5,#16
		ADD		 r5,r5,r8,LSL #4	;lTemp1

		LDR      r6,=RV30_ClampTbl	;ClampVal
		MOV		 r12,#16
loop2	MOV		 r7,r5				;temp
		MOV		 lr,#16
loop3	ADD		 r8,r6,r7,ASR #5
		LDRB     r8,[r8,#0x120]
		
		ADD		r7,r7,r3
		ADD		r9,r6,r7,ASR #5
		LDRB    r9,[r9,#0x120]

		ADD		r7,r7,r3
		ADD		r0,r6,r7,ASR #5
		LDRB    r0,[r0,#0x120]
		ORR		r8,r8,r9,LSL #8

		ADD		r7,r7,r3
		ADD		r9,r6,r7,ASR #5
		LDRB    r9,[r9,#0x120]
		ORR		r8,r8,r0,LSL #16
		SUBS	lr,lr,#4
		ORR		r8,r8,r9,LSL #24
		
		STR		r8,[r2],#4	
		ADD		r7,r7,r3
		BGT		loop3
		ADD		r5,r5,r4
		SUBS	r12,r12,#1
		BGT		loop2

        ADD      sp,sp,#0x20
        LDMFD    sp!,{r4-r9,pc}    
       
        
      
	ENDP
	EXPORT C_PlanarPredict
