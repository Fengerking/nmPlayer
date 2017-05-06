
	AREA InterP, CODE, READONLY
	
ARMV4_Interpolate_H00V00 PROC
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
		ENDP
		EXPORT ARMV4_Interpolate_H00V00

;__asm void RV_FASTCALL  C_Interpolate_H01V00(
;	const U8 *pSrc,
;	U8 *pDst,
;	U32 uSrcPitch,
;	I32 iBlockSize
;)
;{
ARMV4_Interpolate_H01V00 PROC
		IMPORT RV30_ClampTbl

        STMFD    sp!,{r4-r11,lr}
		SUB		 sp,sp,#12
        LDR      r4,=RV30_ClampTbl
		SUB		 r2,r2,r3
		STR	     r2,[sp,#0]
		STR		 r3,[sp,#4]
		RSB		 r5,r3,#16
		STR		 r5,[sp,#8]
		MOV		 lr,#6	
		MOV		 r12,#8	
        MOV      r5,r3
        ;B        Ln284
Ln92
        LDR      r6,[sp,#4]
        ;B        Ln264
Ln100
        LDRB     r9,[r0,#1]		;lTemp2
        LDRB     r8,[r0,#0]		;lTemp1        
        LDRB     r10,[r0,#2]	;lTemp3
        LDRB     r7,[r0,#-1]	;lTemp0
		
		ADD		 r11,r9,r8,LSL #1
		MLA	 r11,lr,r11,r12
		ADD		 r7,r7,r10
		SUB		 r7,r11,r7
		ADD		 r7,r4,r7,ASR #4
		LDRB	 r2,[r7,#0x120]	

		LDRB	 r7,[r0,#3]	
		ADD		 r11,r10,r9,LSL #1
		MLA	 r11,lr,r11,r12
		ADD		 r8,r8,r7
		SUB		 r8,r11,r8
		ADD		 r8,r4,r8,ASR #4
		LDRB	 r3,[r8,#0x120]	

		LDRB	 r8,[r0,#4]				
		ADD		 r11,r7,r10,LSL #1
		ORR		 r2,r2,r3,LSL #8
		MLA	 r11,lr,r11,r12
		ADD		 r9,r9,r8
		SUB		 r9,r11,r9
		ADD		 r9,r4,r9,ASR #4
		LDRB	 r3,[r9,#0x120]		 	

		LDRB	 r9,[r0,#5]				
		ADD		 r11,r8,r7,LSL #1
		ORR		 r2,r2,r3,LSL #16
		MLA	 r11,lr,r11,r12
		ADD		 r10,r10,r9
		SUB		 r10,r11,r10
		ADD		 r10,r4,r10,ASR #4
		LDRB	 r3,[r10,#0x120]		 	

		SUBS	 r6,r6,#4	
		ADD		 r0,r0,#4
		ORR      r2,r2,r3,LSL #24
        STR      r2,[r1],#4
Ln264
        ;CMP      r6,#0
        BGT      Ln100
		LDR	     r2,[sp,#0]        
  		LDR      r3,[sp,#8]
        ADD		 r0,r0,r2
		ADD		 r1,r1,r3
        SUBS     r5,r5,#1
Ln284
        ;CMP      r5,#0
        BGT      Ln92
		ADD		 sp,sp,#12
        LDMFD    sp!,{r4-r11,pc}
        ENDP
		EXPORT ARMV4_Interpolate_H01V00

;__asm void RV_FASTCALL  C_Interpolate_H02V00(
;	const U8 *pSrc,
;	U8 *pDst,
;;	U32 uSrcPitch,
;	I32 iBlockSize
;)
;{
ARMV4_Interpolate_H02V00 PROC
		IMPORT RV30_ClampTbl

        STMFD    sp!,{r4-r11,lr}
		SUB		 sp,sp,#12
        LDR      r4,=RV30_ClampTbl
		SUB		 r2,r2,r3
		STR	     r2,[sp,#0]
		STR		 r3,[sp,#4]
		RSB		 r5,r3,#16
		STR		 r5,[sp,#8]
		MOV		 lr,#6	
		MOV		 r12,#8	
        MOV      r5,r3
        ;B        Ln512
Ln320
        LDR      r6,[sp,#4]
        ;B        Ln492
Ln328
        LDRB     r9,[r0,#1]		;lTemp2
        LDRB     r8,[r0,#0]		;lTemp1        
        LDRB     r10,[r0,#2]	;lTemp3
        LDRB     r7,[r0,#-1]	;lTemp0
		
		ADD		 r11,r8,r9,LSL #1
		MLA	 r11,lr,r11,r12
		ADD		 r7,r7,r10
		SUB		 r7,r11,r7
		ADD		 r7,r4,r7,ASR #4
		LDRB	 r2,[r7,#0x120]	

		LDRB	 r7,[r0,#3]	
		ADD		 r11,r9,r10,LSL #1
		MLA	 r11,lr,r11,r12
		ADD		 r8,r8,r7
		SUB		 r8,r11,r8
		ADD		 r8,r4,r8,ASR #4
		LDRB	 r3,[r8,#0x120]	

		LDRB	 r8,[r0,#4]				
		ADD		 r11,r10,r7,LSL #1
		ORR		 r2,r2,r3,LSL #8
		MLA	 r11,lr,r11,r12
		ADD		 r9,r9,r8
		SUB		 r9,r11,r9
		ADD		 r9,r4,r9,ASR #4
		LDRB	 r3,[r9,#0x120]		 	

		LDRB	 r9,[r0,#5]				
		ADD		 r11,r7,r8,LSL #1
		ORR		 r2,r2,r3,LSL #16
		MLA	 r11,lr,r11,r12
		ADD		 r10,r10,r9
		SUB		 r10,r11,r10
		ADD		 r10,r4,r10,ASR #4
		LDRB	 r3,[r10,#0x120]		 	

		SUBS	 r6,r6,#4	
		ADD		 r0,r0,#4
		ORR      r2,r2,r3,LSL #24
        STR      r2,[r1],#4
Ln492
        ;CMP      r6,#0
        BGT      Ln328
		LDR	     r2,[sp,#0]        
  		LDR      r3,[sp,#8]
        ADD		 r0,r0,r2
		ADD		 r1,r1,r3
        SUBS     r5,r5,#1
Ln512
        ;CMP      r5,#0
        BGT      Ln320
		ADD		 sp,sp,#12
        LDMFD    sp!,{r4-r11,pc}
        ENDP
		EXPORT ARMV4_Interpolate_H02V00
;__asm void RV_FASTCALL  C_Interpolate_H00V01(
;	const U8 *pSrc,
;	U8 *pDst,
;	U32 uSrcPitch,
;	I32 iBlockSize
;)
;{
ARMV4_Interpolate_H00V01	PROC
		IMPORT RV30_ClampTbl

        STMFD    sp!,{r4-r11,lr}
		SUB		 sp,sp,#8
        LDR      r4,=RV30_ClampTbl
		MUL		 r5,r2,r3
		MOV		 r6,r3,LSL #4
		SUB		 r6,r6,#1
		SUB		 r5,r5,#1	
		STR	     r5,[sp,#0]
		STR		 r6,[sp,#4]
		MOV		 lr,#6	
		MOV		 r12,#8	
        MOV      r5,r3
        ;B        Ln788
Ln572
        MOV      r6,r3
        ;B        Ln760
Ln580
        LDRB     r9,[r0,r2]			;lTemp2
        LDRB     r8,[r0,#0]			;lTemp1        
        LDRB     r10,[r0,r2,LSL #1]	;lTemp3
        LDRB     r7,[r0,-r2]		;lTemp0
		
		ADD		 r11,r9,r8,LSL #1
		MLA	 r11,lr,r11,r12
		ADD		 r7,r7,r10
		SUB		 r7,r11,r7
		ADD		 r7,r4,r7,ASR #4
		LDRB	 r11,[r7,#0x120]	

		ADD		 r7,r2,r2,LSL #1
		LDRB	 r7,[r0,r7]
		STRB	 r11,[r1],#16		
		ADD		 r11,r10,r9,LSL #1
		MLA	 r11,lr,r11,r12
		ADD		 r8,r8,r7
		SUB		 r8,r11,r8
		ADD		 r8,r4,r8,ASR #4
		LDRB	 r11,[r8,#0x120]	

		LDRB	 r8,[r0,r2,LSL #2]
		STRB	 r11,[r1],#16		;cycle stall here
		ADD		 r11,r7,r10,LSL #1		
		MLA	 r11,lr,r11,r12
		ADD		 r9,r9,r8
		SUB		 r9,r11,r9
		ADD		 r9,r4,r9,ASR #4
		LDRB	 r11,[r9,#0x120]		 	

		ADD		 r9,r2,r2,LSL #2
		LDRB	 r9,[r0,r9]				
		STRB	 r11,[r1],#16
		ADD		 r11,r8,r7,LSL #1		
		MLA	 r11,lr,r11,r12
		ADD		 r10,r10,r9
		SUB		 r10,r11,r10
		ADD		 r10,r4,r10,ASR #4
		LDRB	 r11,[r10,#0x120]

		SUBS	 r6,r6,#4	
		ADD		 r0,r0,r2,LSL #2
        STRB     r11,[r1],#16
Ln760
        ;CMP      r6,#0
        BGT      Ln580
		LDR	     r7,[sp,#0]        
  		LDR      r8,[sp,#4]
        SUB		 r0,r0,r7
		SUB		 r1,r1,r8
        SUBS     r5,r5,#1
Ln788
        ;CMP      r5,#0
        BGT      Ln572
		ADD		 sp,sp,#8
        LDMFD    sp!,{r4-r11,pc}
        ENDP
		EXPORT	ARMV4_Interpolate_H00V01


;__asm void RV_FASTCALL  C_Interpolate_H01V01(
;	const U8 *pSrc,
;	U8 *pDst,
;	U32 uSrcPitch,
;	I32 iBlockSize
;)
;{
ARMV4_Interpolate_H01V01 PROC
		IMPORT RV30_ClampTbl

        STMFD    sp!,{r4-r11,lr}
        SUB      sp,sp,#0x4c0
		MOV		 r4,sp
		SUB		 r0,r0,r2
		SUB		 r2,r2,r3			
		RSB		 r12,r3,#16
		MOV		 lr,#6
		ADD      r5,r3,#3
        ;B        Ln992
Ln832
        MOV      r6,r3
        ;B        Ln972
Ln840
        LDRB     r9,[r0,#1]		;lTemp2
        LDRB     r8,[r0,#0]		;lTemp1        
        LDRB     r10,[r0,#2]	;lTemp3
        LDRB     r7,[r0,#-1]	;lTemp0
		
		ADD		 r11,r9,r8,LSL #1
		MUL	 r11,lr,r11
		ADD		 r7,r7,r10
		SUB		 r7,r11,r7
		STR		 r7,[r4],#4

		LDRB	 r7,[r0,#3]		
		ADD		 r11,r10,r9,LSL #1
		MUL	 r11,lr,r11
		ADD		 r8,r8,r7
		SUB		 r8,r11,r8
		STR		 r8,[r4],#4

		LDRB	 r8,[r0,#4]				
		ADD		 r11,r7,r10,LSL #1
		MUL	 r11,lr,r11
		ADD		 r9,r9,r8
		SUB		 r9,r11,r9
		STR		 r9,[r4],#4

		LDRB	 r9,[r0,#5]				
		ADD		 r11,r8,r7,LSL #1
		MUL	 r11,lr,r11
		ADD		 r10,r10,r9
		SUB		 r10,r11,r10
		STR		 r10,[r4],#4

		SUBS	 r6,r6,#4	
		ADD		 r0,r0,#4               
Ln972
        ;CMP      r6,#0
        BGT      Ln840
		ADD		 r4,r4,r12,LSL #2  				
		ADD		 r0,r0,r2		
        SUBS     r5,r5,#1
Ln992
        ;CMP      r5,#0
        BGT      Ln832

		LDR      r4,=RV30_ClampTbl
		MOV		 r2,r3,LSL #4
		SUB		 r2,r2,#1	
		MOV		 r0,sp
		MOV		 r12,#0x80	
		MOV		 r5,r3
        ;B        Ln1240
Ln1032
        MOV		 r6,r3
        ;B        Ln1220
Ln1040
        LDR      r9,[r0,#0x80]		;lTemp2
        LDR      r8,[r0,#0x40]		;lTemp1        
        LDR      r10,[r0,#0xc0]		;lTemp3
        LDR      r7,[r0,#0]			;lTemp0
		
		ADD		 r11,r9,r8,LSL #1
		MLA	 r11,lr,r11,r12
		ADD		 r7,r7,r10
		SUB		 r7,r11,r7
		ADD		 r7,r4,r7,ASR #8
		LDRB	 r7,[r7,#0x120]			
		
		ADD		 r11,r10,r9,LSL #1
		MLA	 r11,lr,r11,r12
		STRB	 r7,[r1],#16
		LDR		 r7,[r0,#0x100]
		SUB		 r8,r11,r8
		SUB		 r8,r8,r7
		ADD		 r8,r4,r8,ASR #8
		LDRB	 r8,[r8,#0x120]	

		ADD		 r11,r7,r10,LSL #1		
		MLA	 r11,lr,r11,r12
		STRB	 r8,[r1],#16
		LDR		 r8,[r0,#0x140]
		SUB		 r9,r11,r9
		SUB		 r9,r9,r8
		ADD		 r9,r4,r9,ASR #8
		LDRB	 r9,[r9,#0x120]		 	

		ADD		 r11,r8,r7,LSL #1		
		MLA	 r11,lr,r11,r12
		STRB	 r9,[r1],#16
		LDR		 r9,[r0,#0x180]
		SUB		 r10,r11,r10
		SUB		 r10,r10,r9
		ADD		 r10,r4,r10,ASR #8
		LDRB	 r10,[r10,#0x120]

		SUBS	 r6,r6,#4	
		ADD		 r0,r0,#0x100
        STRB     r10,[r1],#16
Ln1220
        ;CMP      r6,#0
        BGT      Ln1040
		SUB		 r0,r0,r2,LSL #2
        SUB		 r1,r1,r2        
		SUBS     r5,r5,#1
Ln1240
        ;CMP      r5,#0
        BGT      Ln1032
        ADD      sp,sp,#0x4c0
        LDMFD    sp!,{r4-r11,pc}
        ENDP
		EXPORT	ARMV4_Interpolate_H01V01


;__asm void RV_FASTCALL  C_Interpolate_H02V01(
;	const U8 *pSrc,
;	U8 *pDst,
;	U32 uSrcPitch,
;	I32 iBlockSize
;)
;{
ARMV4_Interpolate_H02V01	PROC
		IMPORT RV30_ClampTbl

        STMFD    sp!,{r4-r11,lr}
        SUB      sp,sp,#0x4c0
		MOV		 r4,sp
		SUB		 r0,r0,r2
		SUB		 r2,r2,r3			
		RSB		 r12,r3,#16
		MOV		 lr,#6
		ADD      r5,r3,#3
        ;B        Ln1448
Ln1288
        MOV      r6,r3
        ;B        Ln1428
Ln1296
        LDRB     r9,[r0,#1]		;lTemp2
        LDRB     r8,[r0,#0]		;lTemp1        
        LDRB     r10,[r0,#2]	;lTemp3
        LDRB     r7,[r0,#-1]	;lTemp0
		
		ADD		 r11,r8,r9,LSL #1
		MUL	 r11,lr,r11
		ADD		 r7,r7,r10
		SUB		 r7,r11,r7
		STR		 r7,[r4],#4

		LDRB	 r7,[r0,#3]		
		ADD		 r11,r9,r10,LSL #1
		MUL	 r11,lr,r11
		ADD		 r8,r8,r7
		SUB		 r8,r11,r8
		STR		 r8,[r4],#4

		LDRB	 r8,[r0,#4]				
		ADD		 r11,r10,r7,LSL #1
		MUL	 r11,lr,r11
		ADD		 r9,r9,r8
		SUB		 r9,r11,r9
		STR		 r9,[r4],#4

		LDRB	 r9,[r0,#5]				
		ADD		 r11,r7,r8,LSL #1
		MUL	 r11,lr,r11
		ADD		 r10,r10,r9
		SUB		 r10,r11,r10
		STR		 r10,[r4],#4

		SUBS	 r6,r6,#4	
		ADD		 r0,r0,#4               
Ln1428
        ;CMP      r6,#0
        BGT      Ln1296
		ADD		 r4,r4,r12,LSL #2  				
		ADD		 r0,r0,r2		
        SUBS     r5,r5,#1
Ln1448
        ;CMP      r5,#0
        BGT      Ln1288

		LDR      r4,=RV30_ClampTbl
		MOV		 r2,r3,LSL #4
		SUB		 r2,r2,#1	
		MOV		 r0,sp
		MOV		 r12,#0x80	
		MOV		 r5,r3
        ;B        Ln1696
Ln1488
        MOV      r6,r3
        ;B        Ln1676
Ln1496        
        LDR      r9,[r0,#0x80]		;lTemp2
        LDR      r8,[r0,#0x40]		;lTemp1        
        LDR      r10,[r0,#0xc0]		;lTemp3
        LDR      r7,[r0,#0]			;lTemp0
		
		ADD		 r11,r9,r8,LSL #1
		MLA	 r11,lr,r11,r12
		ADD		 r7,r7,r10
		SUB		 r7,r11,r7
		ADD		 r7,r4,r7,ASR #8
		LDRB	 r7,[r7,#0x120]			
		
		ADD		 r11,r10,r9,LSL #1
		MLA	 r11,lr,r11,r12
		STRB	 r7,[r1],#16
		LDR		 r7,[r0,#0x100]
		SUB		 r8,r11,r8
		SUB		 r8,r8,r7
		ADD		 r8,r4,r8,ASR #8
		LDRB	 r8,[r8,#0x120]	

		ADD		 r11,r7,r10,LSL #1		
		MLA	 r11,lr,r11,r12
		STRB	 r8,[r1],#16
		LDR		 r8,[r0,#0x140]
		SUB		 r9,r11,r9
		SUB		 r9,r9,r8
		ADD		 r9,r4,r9,ASR #8
		LDRB	 r9,[r9,#0x120]		 	

		ADD		 r11,r8,r7,LSL #1		
		MLA	 r11,lr,r11,r12
		STRB	 r9,[r1],#16
		LDR		 r9,[r0,#0x180]
		SUB		 r10,r11,r10
		SUB		 r10,r10,r9
		ADD		 r10,r4,r10,ASR #8
		LDRB	 r10,[r10,#0x120]

		SUBS	 r6,r6,#4	
		ADD		 r0,r0,#0x100
        STRB     r10,[r1],#16
Ln1676
        ;CMP      r6,#0
        BGT      Ln1496
		SUB		 r0,r0,r2,LSL #2
        SUB		 r1,r1,r2        
		SUBS     r5,r5,#1
Ln1696
        ;CMP      r5,#0
        BGT      Ln1488
        ADD      sp,sp,#0x4c0
        LDMFD    sp!,{r4-r11,pc}
        ENDP
		EXPORT	ARMV4_Interpolate_H02V01

;__asm void RV_FASTCALL  C_Interpolate_H00V02(
;	const U8 *pSrc,
;	U8 *pDst,
;	U32 uSrcPitch,
;	I32 iBlockSize
;)
;{
ARMV4_Interpolate_H00V02	PROC
		IMPORT RV30_ClampTbl

        STMFD    sp!,{r4-r11,lr}
		SUB		 sp,sp,#8
        LDR      r4,=RV30_ClampTbl
		MUL		 r5,r2,r3
		MOV		 r6,r3,LSL #4
		SUB		 r6,r6,#1
		SUB		 r5,r5,#1	
		STR	     r5,[sp,#0]
		STR		 r6,[sp,#4]
		MOV		 lr,#6	
		MOV		 r12,#8	
        MOV      r5,r3
        ;B        Ln1976
Ln1760
        MOV      r6,r3
        ;B        Ln1948
Ln1768
        LDRB     r9,[r0,r2]			;lTemp2
        LDRB     r8,[r0,#0]			;lTemp1        
        LDRB     r10,[r0,r2,LSL #1]	;lTemp3
        LDRB     r7,[r0,-r2]		;lTemp0
		
		ADD		 r11,r8,r9,LSL #1
		MLA	 r11,lr,r11,r12
		ADD		 r7,r7,r10
		SUB		 r7,r11,r7
		ADD		 r7,r4,r7,ASR #4
		LDRB	 r11,[r7,#0x120]	

		ADD		 r7,r2,r2,LSL #1
		LDRB	 r7,[r0,r7]
		STRB	 r11,[r1],#16		
		ADD		 r11,r9,r10,LSL #1
		MLA	 r11,lr,r11,r12
		ADD		 r8,r8,r7
		SUB		 r8,r11,r8
		ADD		 r8,r4,r8,ASR #4
		LDRB	 r11,[r8,#0x120]	

		LDRB	 r8,[r0,r2,LSL #2]
		STRB	 r11,[r1],#16		;cycle stall here
		ADD		 r11,r10,r7,LSL #1		
		MLA	 r11,lr,r11,r12
		ADD		 r9,r9,r8
		SUB		 r9,r11,r9
		ADD		 r9,r4,r9,ASR #4
		LDRB	 r11,[r9,#0x120]		 	

		ADD		 r9,r2,r2,LSL #2
		LDRB	 r9,[r0,r9]				
		STRB	 r11,[r1],#16
		ADD		 r11,r7,r8,LSL #1		
		MLA	 r11,lr,r11,r12
		ADD		 r10,r10,r9
		SUB		 r10,r11,r10
		ADD		 r10,r4,r10,ASR #4
		LDRB	 r11,[r10,#0x120]

		SUBS	 r6,r6,#4	
		ADD		 r0,r0,r2,LSL #2
        STRB     r11,[r1],#16
Ln1948
        ;CMP      r6,#0
        BGT      Ln1768
		LDR	     r7,[sp,#0]        
  		LDR      r8,[sp,#4]
        SUB		 r0,r0,r7
		SUB		 r1,r1,r8
        SUBS     r5,r5,#1
Ln1976
        ;CMP      r5,#0
        BGT      Ln1760
		ADD		 sp,sp,#8
        LDMFD    sp!,{r4-r11,pc}
        ENDP
		EXPORT	ARMV4_Interpolate_H00V02

;__asm void RV_FASTCALL  C_Interpolate_H01V02(
;	const U8 *pSrc,
;	U8 *pDst,
;	U32 uSrcPitch,
;	I32 iBlockSize
;)
;{
ARMV4_Interpolate_H01V02	PROC
		IMPORT RV30_ClampTbl

        STMFD    sp!,{r4-r11,lr}
        SUB      sp,sp,#0x4c0
		MOV		 r4,sp
		SUB		 r0,r0,r2
		SUB		 r2,r2,r3			
		RSB		 r12,r3,#16
		MOV		 lr,#6
		ADD      r5,r3,#3
        ;B        Ln2180
Ln2020
        MOV      r6,r3
        ;B        Ln2160
Ln2028
        LDRB     r9,[r0,#1]		;lTemp2
        LDRB     r8,[r0,#0]		;lTemp1        
        LDRB     r10,[r0,#2]	;lTemp3
        LDRB     r7,[r0,#-1]	;lTemp0
		
		ADD		 r11,r9,r8,LSL #1
		MUL	 r11,lr,r11
		ADD		 r7,r7,r10
		SUB		 r7,r11,r7
		STR		 r7,[r4],#4

		LDRB	 r7,[r0,#3]		
		ADD		 r11,r10,r9,LSL #1
		MUL	 r11,lr,r11
		ADD		 r8,r8,r7
		SUB		 r8,r11,r8
		STR		 r8,[r4],#4

		LDRB	 r8,[r0,#4]				
		ADD		 r11,r7,r10,LSL #1
		MUL	 r11,lr,r11
		ADD		 r9,r9,r8
		SUB		 r9,r11,r9
		STR		 r9,[r4],#4

		LDRB	 r9,[r0,#5]				
		ADD		 r11,r8,r7,LSL #1
		MUL	 r11,lr,r11
		ADD		 r10,r10,r9
		SUB		 r10,r11,r10
		STR		 r10,[r4],#4

		SUBS	 r6,r6,#4	
		ADD		 r0,r0,#4               
Ln2160
        ;CMP      r6,#0
        BGT      Ln2028
		ADD		 r4,r4,r12,LSL #2  				
		ADD		 r0,r0,r2		
        SUBS     r5,r5,#1
Ln2180
        ;CMP      r5,#0
        BGT      Ln2020

		LDR      r4,=RV30_ClampTbl
		MOV		 r2,r3,LSL #4
		SUB		 r2,r2,#1	
		MOV		 r0,sp
		MOV		 r12,#0x80	
		MOV		 r5,r3
        ;B        Ln2428
Ln2220
        MOV      r6,r3
        ;B        Ln2408
Ln2228
        LDR      r9,[r0,#0x80]		;lTemp2
        LDR      r8,[r0,#0x40]		;lTemp1        
        LDR      r10,[r0,#0xc0]		;lTemp3
        LDR      r7,[r0,#0]			;lTemp0
		
		ADD		 r11,r8,r9,LSL #1
		MLA	 r11,lr,r11,r12
		ADD		 r7,r7,r10
		SUB		 r7,r11,r7
		ADD		 r7,r4,r7,ASR #8
		LDRB	 r7,[r7,#0x120]			
		
		ADD		 r11,r9,r10,LSL #1
		MLA	 r11,lr,r11,r12
		STRB	 r7,[r1],#16
		LDR		 r7,[r0,#0x100]
		SUB		 r8,r11,r8
		SUB		 r8,r8,r7
		ADD		 r8,r4,r8,ASR #8
		LDRB	 r8,[r8,#0x120]	

		ADD		 r11,r10,r7,LSL #1		
		MLA	 r11,lr,r11,r12
		STRB	 r8,[r1],#16
		LDR		 r8,[r0,#0x140]
		SUB		 r9,r11,r9
		SUB		 r9,r9,r8
		ADD		 r9,r4,r9,ASR #8
		LDRB	 r9,[r9,#0x120]		 	

		ADD		 r11,r7,r8,LSL #1		
		MLA	 r11,lr,r11,r12
		STRB	 r9,[r1],#16
		LDR		 r9,[r0,#0x180]
		SUB		 r10,r11,r10
		SUB		 r10,r10,r9
		ADD		 r10,r4,r10,ASR #8
		LDRB	 r10,[r10,#0x120]

		SUBS	 r6,r6,#4	
		ADD		 r0,r0,#0x100
        STRB     r10,[r1],#16
Ln2408
        ;CMP      r6,#0
        BGT      Ln2228
		SUB		 r0,r0,r2,LSL #2
        SUB		 r1,r1,r2        
		SUBS     r5,r5,#1
Ln2428
        ;CMP      r5,#0
        BGT      Ln2220
        ADD      sp,sp,#0x4c0
        LDMFD    sp!,{r4-r11,pc}
        ENDP
		EXPORT	ARMV4_Interpolate_H01V02

;__asm void RV_FASTCALL  C_Interpolate_H02V02(
;	const U8 *pSrc,
;	U8 *pDst,
;	U32 uSrcPitch,
;	I32 iBlockSize
;)
;{
ARMV4_Interpolate_H02V02	PROC
		IMPORT RV30_ClampTbl

        STMFD    sp!,{r4-r10,lr}
        SUB      sp,sp,#0x480
		MOV		 r4,sp
		SUB		 r2,r2,r3			
		RSB		 r12,r3,#16		
		ADD      r5,r3,#2
        ;B        Ln2632
Ln2476
        MOV      r6,r3
        ;B        Ln2612
Ln2484
		LDRB     r7,[r0,#0]		;lTemp0        
        LDRB     r8,[r0,#1]		;lTemp1        
        LDRB     r9,[r0,#2]		;lTemp2
		
		ADD		 r7,r7,r7,LSL #1
		ADD		 r10,r8,r8,LSL #3
		ADD		 r7,r10,r7,LSL #1
		ADD		 r7,r7,r9		
		STR		 r7,[r4],#4

		LDRB	 r7,[r0,#3]
		ADD		 r8,r8,r8,LSL #1
		ADD		 r10,r9,r9,LSL #3
		ADD		 r8,r10,r8,LSL #1
		ADD		 r8,r8,r7
		STR		 r8,[r4],#4						

		LDRB	 r8,[r0,#4]
		ADD		 r9,r9,r9,LSL #1
		ADD		 r10,r7,r7,LSL #3
		ADD		 r9,r10,r9,LSL #1
		ADD		 r9,r9,r8
		STR		 r9,[r4],#4						

		LDRB	 r9,[r0,#5]
		ADD		 r7,r7,r7,LSL #1
		ADD		 r10,r8,r8,LSL #3
		ADD		 r7,r10,r7,LSL #1
		ADD		 r7,r7,r9
		STR		 r7,[r4],#4						

		SUBS	 r6,r6,#4	
		ADD		 r0,r0,#4               
Ln2612
        ;CMP      r6,#0
        BGT      Ln2484
		ADD		 r4,r4,r12,LSL #2  				
		ADD		 r0,r0,r2		
        SUBS     r5,r5,#1
Ln2632
        ;CMP      r5,#0
        BGT      Ln2476

		LDR      r4,=RV30_ClampTbl
		MOV		 r2,r3,LSL #4
		SUB		 r2,r2,#1	
		MOV		 r0,sp
		MOV		 lr,#6
		MOV		 r12,#0x80	
		MOV		 r5,r3
        ;B        Ln2876
Ln2672
        MOV      r6,r3
        ;B        Ln2856
Ln2680
		LDR      r7,[r0,#0]			;lTemp0
		LDR      r8,[r0,#0x40]		;lTemp1
		LDR      r9,[r0,#0x80]		;lTemp2        
		
		MLA	 r7,lr,r7,r12
		ADD		 r10,r8,r8,LSL #3
		ADD		 r7,r7,r10
		ADD		 r7,r7,r9
		ADD		 r7,r4,r7,ASR #8
		LDRB	 r7,[r7,#0x120]			
		
		MLA	 r8,lr,r8,r12
		ADD		 r10,r9,r9,LSL #3
		STRB     r7,[r1],#16		
		LDR		 r7,[r0,#0xc0]
		ADD		 r8,r8,r10
		ADD		 r8,r8,r7
		ADD		 r8,r4,r8,ASR #8
		LDRB	 r8,[r8,#0x120]			

		MLA	 r9,lr,r9,r12
		ADD		 r10,r7,r7,LSL #3
		STRB     r8,[r1],#16		
		LDR		 r8,[r0,#0x100]	
		ADD		 r9,r9,r10
		ADD		 r9,r9,r8
		ADD		 r9,r4,r9,ASR #8
		LDRB	 r9,[r9,#0x120]

		MLA	 r7,lr,r7,r12
		ADD		 r10,r8,r8,LSL #3
		STRB     r9,[r1],#16		
		LDR		 r9,[r0,#0x140]	
		ADD		 r7,r7,r10
		ADD		 r7,r7,r9
		ADD		 r7,r4,r7,ASR #8
		LDRB	 r7,[r7,#0x120]

		SUBS	 r6,r6,#4	
		ADD		 r0,r0,#0x100
        STRB     r7,[r1],#16
Ln2856
        ;CMP      r6,#0
        BGT      Ln2680
		SUB		 r0,r0,r2,LSL #2
        SUB		 r1,r1,r2        
		SUBS     r5,r5,#1
Ln2876
        ;CMP      r5,#0
        BGT      Ln2672
        ADD      sp,sp,#0x480
        LDMFD    sp!,{r4-r10,pc}
        ENDP
		EXPORT	ARMV4_Interpolate_H02V02

;__asm void RV_FASTCALL C_MCCopyChroma_H00V00(
;	const U8 *pSrc,
;	U8 *pDst,
;	U32 uSrcPitch
;)
;{
ARMV4_MCCopyChroma_H00V00	PROC
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
		ENDP
		EXPORT  ARMV4_MCCopyChroma_H00V00

;__asm void RV_FASTCALL  C_MCCopyChroma_H01V00(
;	const U8 *pSrc,
;	U8 *pDst,
;	U32 uSrcPitch
;)
;{
ARMV4_MCCopyChroma_H01V00	PROC
        STMFD    sp!,{r4-r8,lr}
		MOV		 lr,#5	
		MOV		 r12,#4
        MOV      r3,#4
Ln2960
        LDRB     r4,[r0,#0]		;lTemp0
        LDRB     r5,[r0,#1]		;lTemp1
		LDRB	 r6,[r0,#2]		;lTemp0
        
		MLA	 r4,lr,r4,r12
		ADD		 r7,r5,r5,LSL #1
		ADD		 r4,r4,r7	
		MOV		 r8,r4,ASR #3
		
		MLA	 r5,lr,r5,r12
		ADD		 r7,r6,r6,LSL #1
		ADD		 r5,r5,r7	
		MOV		 r5,r5,ASR #3

		LDRB	 r4,[r0,#3]
		ORR		 r8,r8,r5,LSL #8
		MLA	 r6,lr,r6,r12
		ADD		 r7,r4,r4,LSL #1
		ADD		 r6,r6,r7	
		MOV		 r6,r6,ASR #3
		
		LDRB	 r5,[r0,#4]	
		ORR		 r8,r8,r6,LSL #16
		MLA	 r4,lr,r4,r12
		ADD		 r7,r5,r5,LSL #1
		ADD		 r4,r4,r7	
		MOV		 r4,r4,ASR #3	
		ORR		 r8,r8,r4,LSL #24		
		STR      r8,[r1],#0x10			

		ADD		 r0,r0,r2
		SUBS	 r3,r3,#1
        BGT      Ln2960
        LDMFD    sp!,{r4-r8,pc}
        ENDP
		EXPORT  ARMV4_MCCopyChroma_H01V00

;__asm void RV_FASTCALL  C_MCCopyChroma_H02V00(
;	const U8 *pSrc,
;	U8 *pDst,
;	U32 uSrcPitch
;)
;{
ARMV4_MCCopyChroma_H02V00	PROC
        STMFD    sp!,{r4-r8,lr}
		MOV		 lr,#3	
		MOV		 r12,#4
        MOV      r3,#4
Ln3104
        LDRB     r4,[r0,#0]		;lTemp0
        LDRB     r5,[r0,#1]		;lTemp1
		LDRB	 r6,[r0,#2]		;lTemp0
        
		MLA	 r4,lr,r4,r12
		ADD		 r7,r5,r5,LSL #2
		ADD		 r4,r4,r7	
		MOV		 r8,r4,ASR #3
		
		MLA	 r5,lr,r5,r12
		ADD		 r7,r6,r6,LSL #2
		ADD		 r5,r5,r7	
		MOV		 r5,r5,ASR #3

		LDRB	 r4,[r0,#3]
		ORR		 r8,r8,r5,LSL #8
		MLA	 r6,lr,r6,r12
		ADD		 r7,r4,r4,LSL #2
		ADD		 r6,r6,r7	
		MOV		 r6,r6,ASR #3
		
		LDRB	 r5,[r0,#4]	
		ORR		 r8,r8,r6,LSL #16
		MLA	 r4,lr,r4,r12
		ADD		 r7,r5,r5,LSL #2
		ADD		 r4,r4,r7	
		MOV		 r4,r4,ASR #3	
		ORR		 r8,r8,r4,LSL #24		
		STR      r8,[r1],#0x10			

		ADD		 r0,r0,r2
		SUBS	 r3,r3,#1
        BGT      Ln3104
        LDMFD    sp!,{r4-r8,pc}
        ENDP
    EXPORT  ARMV4_MCCopyChroma_H02V00


;__asm void RV_FASTCALL  C_MCCopyChroma_H00V01(
;	const U8 *pSrc,
;	U8 *pDst,
;	U32 uSrcPitch
;)
;{
ARMV4_MCCopyChroma_H00V01	PROC
        STMFD    sp!,{r4-r8,lr}
		MOV		 lr,#5	
		MOV		 r12,#4
		ADD		 r8,r2,r2,LSL #1
        MOV      r3,#4
Ln3252
        LDRB     r4,[r0,#0]			;lTemp0
        LDRB     r5,[r0,r2]	    ;lTemp1
		LDRB	 r6,[r0,r2,LSL #1]	;lTemp0
        
		MLA	 r4,lr,r4,r12
		ADD		 r7,r5,r5,LSL #1
		ADD		 r4,r4,r7	
		MOV		 r4,r4,ASR #3
		STRB     r4,[r1,#0]
		LDRB	 r4,[r0,r8]
		
		MLA	 r5,lr,r5,r12
		ADD		 r7,r6,r6,LSL #1
		ADD		 r5,r5,r7	
		MOV		 r5,r5,ASR #3
		STRB     r5,[r1,#0x10]
		LDRB	 r5,[r0,r2,LSL #2]	

		MLA	 r6,lr,r6,r12
		ADD		 r7,r4,r4,LSL #1
		ADD		 r6,r6,r7	
		MOV		 r6,r6,ASR #3
		STRB     r6,[r1,#0x20]		
		
		MLA	 r4,lr,r4,r12
		ADD		 r7,r5,r5,LSL #1
		ADD		 r4,r4,r7	
		MOV		 r4,r4,ASR #3			
		STRB     r4,[r1,#0x30]		

		ADD		 r0,r0,#1
		ADD		 r1,r1,#1
		SUBS	 r3,r3,#1
        BGT      Ln3252
        LDMFD    sp!,{r4-r8,pc}
        ENDP
    EXPORT  ARMV4_MCCopyChroma_H00V01

;__asm void RV_FASTCALL  C_MCCopyChroma_H00V02(
;	const U8 *pSrc,
;	U8 *pDst,
;	U32 uSrcPitch
;)
;{
ARMV4_MCCopyChroma_H00V02	PROC
        STMFD    sp!,{r4-r8,lr}
		MOV		 lr,#3	
		MOV		 r12,#4
		ADD		 r8,r2,r2,LSL #1
        MOV      r3,#4
Ln3404
        LDRB     r4,[r0,#0]			 ;lTemp0
        LDRB     r5,[r0,r2]			 ;lTemp1
		LDRB	 r6,[r0,r2,LSL #1]	 ;lTemp0
        
		MLA	 r4,lr,r4,r12
		ADD		 r7,r5,r5,LSL #2
		ADD		 r4,r4,r7	
		MOV		 r4,r4,ASR #3
		STRB     r4,[r1,#0]
		LDRB	 r4,[r0,r8]
		
		MLA	 r5,lr,r5,r12
		ADD		 r7,r6,r6,LSL #2
		ADD		 r5,r5,r7	
		MOV		 r5,r5,ASR #3
		STRB     r5,[r1,#0x10]
		LDRB	 r5,[r0,r2,LSL #2]	

		MLA	 r6,lr,r6,r12
		ADD		 r7,r4,r4,LSL #2
		ADD		 r6,r6,r7	
		MOV		 r6,r6,ASR #3
		STRB     r6,[r1,#0x20]		
		
		MLA	 r4,lr,r4,r12
		ADD		 r7,r5,r5,LSL #2
		ADD		 r4,r4,r7	
		MOV		 r4,r4,ASR #3			
		STRB     r4,[r1,#0x30]		

		ADD		 r0,r0,#1
		ADD		 r1,r1,#1
		SUBS	 r3,r3,#1
        BGT      Ln3404
        LDMFD    sp!,{r4-r8,pc}
        ENDP
    EXPORT ARMV4_MCCopyChroma_H00V02

;__asm void RV_FASTCALL  C_MCCopyChroma_H01V01(
;	const U8 *pSrc,
;	U8 *pDst,
;	U32 uSrcPitch
;)
;{
ARMV4_MCCopyChroma_H01V01	PROC
        STMFD    sp!,{r4-r10,lr}
		MOV		 lr,#25		
		MOV		 r12,#32
		MOV		 r10,#9
        ADD      r3,r0,r2
        MOV      r4,#4
Ln3560
		LDRB     r5,[r0,#0]	;lTemp0		
        LDRB     r6,[r0,#1]	;lTemp1
        LDRB     r7,[r3,#0]	;lTemp2        
        LDRB     r8,[r3,#1]	;lTemp3
		
		MLA	 r5,lr,r5,r12
		ADD		 r7,r6,r7
		MLA	 r5,r10,r8,r5
		RSB		 r7,r7,r7,LSL #4	 
		ADD		 r5,r5,r7
		MOV		 r9,r5,ASR #6	

		LDRB	 r5,[r0,#2]	
		LDRB	 r7,[r3,#2]	
		MLA	 r6,lr,r6,r12
		ADD		 r8,r5,r8
		MLA	 r6,r10,r7,r6
		RSB		 r8,r8,r8,LSL #4	 
		ADD		 r6,r6,r8
		MOV		 r6,r6,ASR #6
		ORR		 r9,r9,r6,LSL #8

		LDRB	 r6,[r0,#3]	
		LDRB	 r8,[r3,#3]	
		MLA	 r5,lr,r5,r12
		ADD		 r7,r6,r7
		MLA	 r5,r10,r8,r5
		RSB		 r7,r7,r7,LSL #4	 
		ADD		 r5,r5,r7
		MOV		 r5,r5,ASR #6	
		ORR		 r9,r9,r5,LSL #16

		LDRB	 r5,[r0,#4]	
		LDRB	 r7,[r3,#4]	
		MLA	 r6,lr,r6,r12
		ADD		 r8,r5,r8
		MLA	 r6,r10,r7,r6
		RSB		 r8,r8,r8,LSL #4	 
		ADD		 r6,r6,r8
		MOV		 r6,r6,ASR #6
		ORR		 r9,r9,r6,LSL #24

        STR      r9,[r1],#0x10
        ADD      r3,r3,r2
        ADD      r0,r0,r2
		SUBS	 r4,r4,#1	
        BGT      Ln3560
        LDMFD    sp!,{r4-r10,pc}
        ENDP
    EXPORT  ARMV4_MCCopyChroma_H01V01

;__asm void RV_FASTCALL  C_MCCopyChroma_H02V01(
;	const U8 *pSrc,
;	U8 *pDst,
;	U32 uSrcPitch
;)
;{
ARMV4_MCCopyChroma_H02V01	PROC
        STMFD    sp!,{r4-r10,lr}
		MOV		 lr,#9		
		MOV		 r12,#32
		MOV		 r10,#25
        ADD      r3,r0,r2
        MOV      r4,#4
Ln3784
		LDRB     r5,[r0,#0]	;lTemp0		
		LDRB     r7,[r3,#0]	;lTemp2        
		LDRB     r8,[r3,#1]	;lTemp3
        LDRB     r6,[r0,#1]	;lTemp1                
		
		MLA	 r7,lr,r7,r12
		ADD		 r5,r5,r8
		MLA	 r7,r10,r6,r7
		RSB		 r5,r5,r5,LSL #4	
		ADD		 r5,r5,r7
		MOV		 r9,r5,ASR #6	

		LDRB	 r7,[r3,#2]	
		LDRB	 r5,[r0,#2]			
		MLA	 r8,lr,r8,r12
		ADD		 r6,r6,r7
		MLA	 r8,r10,r5,r8
		RSB		 r6,r6,r6,LSL #4	
		ADD		 r6,r6,r8
		MOV		 r6,r6,ASR #6	
		ORR		 r9,r9,r6,LSL #8

		LDRB	 r8,[r3,#3]	
		LDRB	 r6,[r0,#3]			
		MLA	 r7,lr,r7,r12
		ADD		 r5,r5,r8
		MLA	 r7,r10,r6,r7
		RSB		 r5,r5,r5,LSL #4	
		ADD		 r5,r5,r7
		MOV		 r5,r5,ASR #6	
		ORR		 r9,r9,r5,LSL #16

		LDRB	 r7,[r3,#4]	
		LDRB	 r5,[r0,#4]			
		MLA	 r8,lr,r8,r12
		ADD		 r6,r6,r7
		MLA	 r8,r10,r5,r8
		RSB		 r6,r6,r6,LSL #4	
		ADD		 r6,r6,r8
		MOV		 r6,r6,ASR #6	
		ORR		 r9,r9,r6,LSL #24

        STR      r9,[r1],#0x10
        ADD      r3,r3,r2
        ADD      r0,r0,r2
		SUBS	 r4,r4,#1	
        BGT      Ln3784
        LDMFD    sp!,{r4-r10,pc}
        ENDP
		EXPORT	ARMV4_MCCopyChroma_H02V01

;__asm void RV_FASTCALL  C_MCCopyChroma_H01V02(
;	const U8 *pSrc,
;	U8 *pDst,
;	U32 uSrcPitch
;)
;{
ARMV4_MCCopyChroma_H01V02	PROC
        STMFD    sp!,{r4-r10,lr}
		MOV		 lr,#25		
		MOV		 r12,#32
		MOV		 r10,#9
        ADD      r3,r0,r2
        MOV      r4,#4
Ln3992
		LDRB     r5,[r0,#0]	;lTemp0		
		LDRB     r7,[r3,#0]	;lTemp2        
		LDRB     r8,[r3,#1]	;lTemp3
        LDRB     r6,[r0,#1]	;lTemp1                
		
		MLA	 r7,lr,r7,r12
		ADD		 r5,r5,r8
		MLA	 r7,r10,r6,r7
		RSB		 r5,r5,r5,LSL #4	
		ADD		 r5,r5,r7
		MOV		 r9,r5,ASR #6	

		LDRB	 r7,[r3,#2]	
		LDRB	 r5,[r0,#2]			
		MLA	 r8,lr,r8,r12
		ADD		 r6,r6,r7
		MLA	 r8,r10,r5,r8
		RSB		 r6,r6,r6,LSL #4	
		ADD		 r6,r6,r8
		MOV		 r6,r6,ASR #6	
		ORR		 r9,r9,r6,LSL #8

		LDRB	 r8,[r3,#3]	
		LDRB	 r6,[r0,#3]			
		MLA	 r7,lr,r7,r12
		ADD		 r5,r5,r8
		MLA	 r7,r10,r6,r7
		RSB		 r5,r5,r5,LSL #4	
		ADD		 r5,r5,r7
		MOV		 r5,r5,ASR #6	
		ORR		 r9,r9,r5,LSL #16

		LDRB	 r7,[r3,#4]	
		LDRB	 r5,[r0,#4]			
		MLA	 r8,lr,r8,r12
		ADD		 r6,r6,r7
		MLA	 r8,r10,r5,r8
		RSB		 r6,r6,r6,LSL #4	
		ADD		 r6,r6,r8
		MOV		 r6,r6,ASR #6	
		ORR		 r9,r9,r6,LSL #24

        STR      r9,[r1],#0x10
        ADD      r3,r3,r2
        ADD      r0,r0,r2
		SUBS	 r4,r4,#1	
        BGT      Ln3992
        LDMFD    sp!,{r4-r10,pc}
        ENDP
		EXPORT ARMV4_MCCopyChroma_H01V02

;__asm void RV_FASTCALL  C_MCCopyChroma_H02V02(
;	const U8 *pSrc,
;	U8 *pDst,
;	U32 uSrcPitch
;)
;{
ARMV4_MCCopyChroma_H02V02	PROC
        STMFD    sp!,{r4-r10,lr}
		MOV		 lr,#9		
		MOV		 r12,#32
		MOV		 r10,#25
        ADD      r3,r0,r2
        MOV      r4,#4
Ln4200
		LDRB     r5,[r0,#0]	;lTemp0		
        LDRB     r6,[r0,#1]	;lTemp1
        LDRB     r7,[r3,#0]	;lTemp2        
        LDRB     r8,[r3,#1]	;lTemp3
		
		MLA	 r5,lr,r5,r12
		ADD		 r7,r6,r7
		MLA	 r5,r10,r8,r5
		RSB		 r7,r7,r7,LSL #4	 
		ADD		 r5,r5,r7
		MOV		 r9,r5,ASR #6	

		LDRB	 r5,[r0,#2]	
		LDRB	 r7,[r3,#2]	
		MLA	 r6,lr,r6,r12
		ADD		 r8,r5,r8
		MLA	 r6,r10,r7,r6
		RSB		 r8,r8,r8,LSL #4	 
		ADD		 r6,r6,r8
		MOV		 r6,r6,ASR #6
		ORR		 r9,r9,r6,LSL #8

		LDRB	 r6,[r0,#3]	
		LDRB	 r8,[r3,#3]	
		MLA	 r5,lr,r5,r12
		ADD		 r7,r6,r7
		MLA	 r5,r10,r8,r5
		RSB		 r7,r7,r7,LSL #4	 
		ADD		 r5,r5,r7
		MOV		 r5,r5,ASR #6	
		ORR		 r9,r9,r5,LSL #16

		LDRB	 r5,[r0,#4]	
		LDRB	 r7,[r3,#4]	
		MLA	 r6,lr,r6,r12
		ADD		 r8,r5,r8
		MLA	 r6,r10,r7,r6
		RSB		 r8,r8,r8,LSL #4	 
		ADD		 r6,r6,r8
		MOV		 r6,r6,ASR #6
		ORR		 r9,r9,r6,LSL #24

        STR      r9,[r1],#0x10
        ADD      r3,r3,r2
        ADD      r0,r0,r2
		SUBS	 r4,r4,#1	
        BGT      Ln4200
        LDMFD    sp!,{r4-r10,pc}
        ENDP
		EXPORT	ARMV4_MCCopyChroma_H02V02
;__asm void RV_FASTCALL C_Interpolate2_H00V02(
;	const U8 *pSrc,
;	U8 *pDst,
;	U32 uSrcPitch,
;	I32 iBlockSize
;)
;{
ARMV4_Interpolate2_H00V02	PROC
        STMFD    sp!,{r4-r9,lr}
		ADD		 r12,r2,r2,LSL #1
		MUL		 r8,r2,r3
		MOV		 r9,r3,LSL #0x04
		SUB		 r9,r9,#0x01	;((DEST_PITCH * iBlockSize) - 1);
		SUB		 r8,r8,#0x01	;((uSrcPitch * iBlockSize) - 1);	
		
        MOV      r6,r3
Ln021	MOV		 r7,r3

Ln022   LDRB     r4,[r0,#0]
        LDRB     r5,[r0,r2]
		SUBS     r7,r7,#0x04
		LDRB     lr,[r0,r2,LSL #1]

		ADD		 r4,r4,r5
		MOV		 r4,r4,ASR #1
		STRB	 r4,[r1],#0x10
				
		ADD		 r5,r5,lr
		LDRB	 r4,[r0,r12]
		MOV		 r5,r5,ASR #1
		STRB	 r5,[r1],#0x10
		
		ADD		 lr,lr,r4
		LDRB	 r5,[r0,r2,LSL #2]
		MOV		 lr,lr,ASR #1
		STRB	 lr,[r1],#0x10
		
		ADD		 r4,r4,r5
		MOV		 r4,r4,ASR #1
		STRB	 r4,[r1],#0x10
		ADD		 r0,r0,r2,LSL #2
		BGT		 Ln022
		SUB		 r1,r1,r9
		SUB      r0,r0,r8
		SUBS	 r6,r6,#0x01
        BGT      Ln021
        LDMFD    sp!,{r4-r9,pc}
        ENDP
		EXPORT	ARMV4_Interpolate2_H00V02
