;*****************************************************************************
;*																			*
;*		VisualOn, Inc. Confidential and Proprietary, 2009					*
;*																			*
;*****************************************************************************

	AREA	|.text|, CODE
	
	EXPORT	ARMV6_rv9_h_loop_filter
	EXPORT	ARMV6_rv9_v_loop_filter
		        
H_Offset_SavingReg		EQU		36		        
H_Offset_TmpBuff		EQU		104
H_Stack_Size			EQU		H_Offset_TmpBuff + H_Offset_SavingReg
		        
ARMV6_rv9_h_loop_filter	PROC
		
		IMPORT  ditherR
		IMPORT  ditherL
		
		STMFD	sp!,{r4-r11,lr}
		pld		[r0,-r1,LSL #1]
		SUB		sp,sp,#H_Offset_TmpBuff		
		ADD		r4,sp,#64
				
		;sp		DeltaL
		;sp+16	DeltaL2	
		;sp+32	DeltaR	
		;sp+48	DeltaR2	
		;sp+96	abs(deltaL2)
		;sp+100	abs(deltaR2)

		LDR 	r5,[r0,-r1,LSL #1];pL2[0]
		STMIA	r4,{r1-r3}	
		ADD		lr,r1,r1,LSL #1
		LDR 	r6,[r0,-r1]		  ;pL1[0]
		LDR 	r7,[r0,-lr]		  ;pL3[0]

		UXTB16	r8,r5
		UXTB16	r9,r6
		UXTB16	r10,r7
		SSUB16	r9,r8,r9
		SSUB16	r10,r8,r10
		SXTH	r8,r9
		STR		r8,[sp,#0x00]		;deltaL[0]
		SXTH	r9,r9,ROR #16
		STR		r9,[sp,#0x08]		;deltaL[2]
		ADD		r8,r8,r9			;delta = deltaL[0]+deltaL[1]+deltaL[2]+deltaL[3];
		SXTH	r9,r10
		STR		r9,[sp,#0x10]		;deltaL2[0]
		SXTH	r10,r10,ROR #16
		STR		r10,[sp,#0x18]		;deltaL2[2]
		ADD		r9,r9,r10

		UXTB16	r5,r5,ROR #8
		UXTB16	r6,r6,ROR #8
		UXTB16	r7,r7,ROR #8
		SSUB16	r6,r5,r6
		SSUB16	r7,r5,r7
		SXTH	r10,r6
		STR		r10,[sp,#0x04]		;deltaL[1]
		SXTH	r6,r6,ROR #16
		STR		r6,[sp,#0x0c]		;deltaL[3]
		ADD		r8,r8,r10
		ADDS	r8,r8,r6			;delta = deltaL[0]+deltaL[1]+deltaL[2]+deltaL[3];
		RSBMI	r8,r8,#0			;abs(delta)
		SXTH	r10,r7
		STR		r10,[sp,#0x14]		;deltaL2[1]
		SXTH	r7,r7,ROR #16
		STR		r7,[sp,#0x1c]		;deltaL2[3]

		LDR		r5,[r0,r1]
		ADD		r9,r9,r10
		LDR		r6,[r0]				;PR1
		ADDS	r9,r9,r7			;deltaL2 = deltaL2[0]+deltaL2[1]+deltaL2[2]+deltaL2[3];
		LDR		r7,[r0,r1,LSL #1]	;PR3
		RSBMI	r9,r9,#0			;ABS(deltaL2)
		STR		r9,[sp,#96]			
		pld		[r0,-r1,LSL #1]
		pld		[r0,-r1]
		pld		[r0]
		pld		[r0,r1]
		
		UXTB16	r9,r5
		UXTB16	r10,r6 
		UXTB16	r11,r7
		SSUB16	r10,r9,r10
		SSUB16	r11,r9,r11
		SXTH	r9,r10
		STR		r9,[sp,#0x20]
		SXTH	r10,r10,ROR #16
		STR		r10,[sp,#0x28]
		ADD		r9,r9,r10
		SXTH	r10,r11
		STR		r10,[sp,#0x30]
		SXTH	r11,r11, ROR #16
		STR		r11,[sp,#0x38]
		ADD		r11,r11,r10
		
		UXTB16	r5,r5,ROR #8
		UXTB16	r6,r6,ROR #8
		UXTB16	r7,r7,ROR #8
		SSUB16	r6,r5,r6
		SSUB16	r7,r5,r7
		SXTH	r10,r6
		STR		r10,[sp,#0x24]
		SXTH	r6,r6,ROR #16
		STR		r6,[sp,#0x2c]
		ADD		r9,r9,r10
		ADDS	r9,r9,r6			;delta = deltaR[0]+deltaR[1]+deltaR[2]+deltaR[3];
		RSBMI	r9,r9,#0			;abs(delta)	
		SXTH	r10,r7
		STR		r10,[sp,#0x34]
		SXTH	r7,r7,ROR #16
		STR		r7,[sp,#0x3c]
		ADD		r11,r11,r10
		ADDS	r11,r11,r7			;deltaR2 = deltaR2[0]+deltaR2[1]+deltaR2[2]+deltaR2[3];
		
		LDR		r4,[sp,#H_Stack_Size+8]	;betaX
		RSBMI	r11,r11,#0			;ABS(deltaR2)
		STR		r11,[sp,#100]
		MOV		r5,#3			;Al
		MOV		r6,#3			;Ar
		CMP		r8,r4,LSL #2
		MOVGE	r5,#1		
		CMP		r9,r4,LSL #2
		MOVGE	r6,#1
		STRB	r5,[sp,#92]		;Al	try to combine these two
		STRB	r6,[sp,#93]		;Ar

		ADD		r7,r5,r6
		CMP		r7,#2
	;	BLE		endh
		LDRGT	r4,[sp,#H_Stack_Size]	;Cl
		ADDLE	sp,sp,#H_Offset_TmpBuff
		LDMLEFD	sp!,{r4-r11,pc}
		
		
		ADD		r7,r7,r3		;Cr
		MOV		r12,#0			;b3SmoothLeft
		ADD		r7,r7,r4
		LDR		r4,[sp,#H_Stack_Size+20]	;bStrong
		MOV		r7,r7,ASR #1
		STRB	r7,[sp,#94]		;c

		MOV		lr,#0			;b3SmoothRight		
		CMP		r4,#0
		BEQ		next1h			;This is different from c code implementation	
		CMP		r5,#3			;when bStrong is False go to next1
		MOVEQ	r12,#1
		CMP		r6,#3
		MOVEQ	lr,#1

if2h	LDR		r9,[sp,#H_Stack_Size+12]	;beta2
		LDR		r4,[sp,#96]		;ABS(deltaL2)
		LDR		r5,[sp,#100]	;ABS(deltaR2)
		CMP		r12,#0
		BEQ		if3h
		CMP		r4,r9
		MOVGE	r12,#0

if3h	
		CMP		lr,#0
		BEQ		if4h
		CMP		r5,r9
		MOVGE	lr,#0

if4h	CMP		r12,#0
		CMPNE	lr,#0		
		BEQ		next1h
		
		LDR		r4,=ditherL
		LDR		r5,=ditherR
		MOV		r12,#0			;z=0
		ADD		r4,r4,r2,LSL #1
		ADD		r5,r5,r2,LSL #1
		STR		r4,[sp,#76]		;dithL
		STR		r5,[sp,#80]		;dithR

loop1h	
		LDR		r1,[sp,#64]
		LDRB	r5,[r0,#0] 		  ;R1
		STR		r12,[sp,#84]	  ;z
		LDR		r11,[sp,#H_Stack_Size+4]	  ;alpha		
		LDRB	r4,[r0,-r1]		  ;L1		
		MOV		r12,r12,LSL #1
		SUB		r3,r0,r1
		SUBS	r10,r5,r4
		RSBMI	r10,r10,#0
		BEQ		loop1endh
		
		SMULBB	r10,r11,r10
		SUB		r3,r3,r1,LSL #1
		ADD		r11,r4,r5		;L1 + R1
		MOV		r10,r10,ASR #7	;n		
		CMP		r10,#1		
		BGT		loop1endh

		LDRB	r8,[r3],r1		  ;L3
		LDR		r2,[sp,#80]
		LDRB	r6,[r3],r1		  ;L2
		LDRB	r7,[r0,r1]		  ;R2		
		LDRB	r9,[r0,r1,LSL #1] ;R3		
		LDR		r1,[sp,#76]
		ADD		r11,r11,r6		;+L2
		LDRH	r2,[r2,r12]		;d2
		LDRH	r1,[r1,r12]		;d1

		ADD		r12,r7,r8		;R2+L3
		ADD		r12,r11,r12		;
		MOV		r3,#25
		STR		r10,[sp,#88]	  ;n			
		SMLABB	r12,r3,r12,r11
		ADD		r11,r11,r7		;L1+R1+L2+R2
		ADD		lr,r11,r9		;R1+L1+L2+R2+R3
		SUB		r11,r11,r6		;L1+R1+R2
		ADD		r12,r12,r1
		
		SMLABB	lr,r3,lr,r11
		LDR		r3,[sp,#88]		;n
		MOV		r12,r12,ASR #7	;t1,pL1[z]
		LDRB	r11,[sp,#94]	;c
		ADD		lr,lr,r2
		
		CMP		r3,#0
		MOV		lr,lr,ASR #7	;t2,r[z]
		BEQ		rt1h
		
		ADD		r3,r11,r4		;L1+c
		CMP		r12,r3
		MOVGT	r12,r3
		SUB		r3,r4,r11		;L1-c
		CMP		r12,r3
		MOVLT	r12,r3
		ADD		r3,r11,r5		;R1+c
		CMP		lr,r3
		MOVGT	lr,r3
		SUB		r3,r5,r11		;R1-c
		CMP		lr,r3
		MOVLT	lr,r3

rt1h	LDR		r3,[sp,#64]		;r1
		STRB	lr,[r0,#0]		;r[z]
		STRB	r12,[r0,-r3]	;pL1[z]		

		LDRB	r10,[r0,-r3,LSL #2]	;L4
		ADD		r3,r3,r3,LSL #1
		LDRB	r11,[r0,r3]		;R4
		ADD		r3,r6,r8		;L2+L3
		ADD		r3,r3,r12		;+rt[-1] 
		ADD		r5,r5,r10		;R1+L4
		ADD		r5,r5,r3
		ADD		r1,r1,r3		;+d1
		MOV		r3,#25
		SMLABB	r5,r3,r5,r1
		ADD		r3,r7,r9		;R2+R3
		ADD		r3,r3,lr		;+rt[0]
		ADD		r4,r4,r11		;L1+R4
		ADD		r4,r4,r3

		ADD		r2,r2,r3
		MOV		r3,#25
		MOV		r5,r5,ASR #7	;t1,rt[-2]	
		SMLABB	r4,r3,r4,r2
		LDR		r1,[sp,#88]		;n
		LDRB	r2,[sp,#94]		;c
		MOV		r4,r4,ASR #7	;t2,rt[1]
		CMP		r1,#0
		BEQ		rt2h
		
		ADD		r3,r6,r2		;L2+c
		CMP		r5,r3
		MOVGT	r5,r3
		SUB		r3,r6,r2		;L2-c
		CMP		r5,r3
		MOVLT	r5,r3
		ADD		r3,r7,r2		;R2+c
		CMP		r4,r3
		MOVGT	r4,r3
		SUB		r3,r7,r2		;R2-c
		CMP		r4,r3
		MOVLT	r4,r3

rt2h	LDR		r3,[sp,#64]			;r
		LDR		r1,[sp,#H_Stack_Size+16]	;bChroma
		ADD		r10,r10,r5		;L4+rt[-2]
		ADD		r10,r10,r8
		STRB	r5,[r0,-r3,LSL #1]	;PL2[z]	;stall
		STRB	r4,[r0,r3]			;PR2[z]
		CMP		r1,#0
		BNE		loop1endh
		
		ADD		r12,r12,r8
		ADD		r12,r12,r10
		MOV     r1,#25
		SMLABB	r12,r1,r12,r10
		ADD		r11,r11,r9		;R4 + R3
		ADD		r11,r11,r4		;rt[1]
		ADD		r9,r9,lr		;R3+rt[0]
		ADD		r12,r12,#64		
		
		ADD		r9,r9,r11
		MOV		r12,r12,ASR #7		
		SMLABB	r9,r1,r9,r11
		LDR		r3,[sp,#64]
		ADD		r9,r9,#64		
		MOV		r9,r9,ASR #7
		ADD		r4,r3,r3,LSL #1
		STRB	r9,[r0,r3,LSL #1]	;PR3[z]
		STRB	r12,[r0,-r4]		;PL3[z]	

loop1endh
		 LDR		r12,[sp,#84]	
		 ADD		r0,r0,#1
		 ADD		r12,r12,#1
		 CMP		r12,#4
		 BLT		loop1h
		; BGE		endh
		 ADD		sp,sp,#H_Offset_TmpBuff
		 LDMFD		sp!,{r4-r11,pc}
		 
next1h	
		LDRB	r5,[sp,#92]		;Al
		LDRB	r6,[sp,#93]		;Ar	
		LDR		r1,[sp,#64]		;uPicth
		CMP		r5,#1			;stall
		CMPNE	r6,#1
		BEQ		elseh
				
		LDR		lr,[sp,#H_Stack_Size+8]	;betaX
		MOV		r11,sp			;deltaL		
		LDR		r9,[sp,#H_Stack_Size]	;Cl		
		MOV		r8,#4			;loop variable
		LDR		r10,[sp,#72]	;Cr
		
loop2h	
		LDRB	r2,[r0,-r1] 		;L1;pL1[z];
		LDRB	r3,[r0,#0]			;R1;r[z]		
		LDRB	r5,[r0,-r1,LSL #1]	;L2;pL2[z];
		LDR		r4,[sp,#H_Stack_Size+4]	;alpha
		SUBS	r7,r3,r2		;delta
		RSBMI	r7,r7,#0		;abs(delta)
		BEQ		loop2endh
		
		SMULBB	r4,r4,r7		
		LDRB	r6,[r0,r1]			;R2;pR2[z];
		SUB		r7,r3,r2		;delta
		LDRB	r12,[sp,#94]	;c
		MOV		r4,r4,ASR #7
		CMP		r4,#2
		BGT		loop2endh
		
		ADD		r7,r5,r7,LSL #2	;L2+(delta<<2)
		SUB		r7,r7,r6		;-R2
		ADD		r7,r7,#4
		LDR 	r4,[r11,#48]	;deltaR2[z]
		MOVS	r7,r7,ASR #3	;d
		BEQ		deltaL2h		;This is added to avoid further caculations if d=0
		
		CMP		r7,r12			;d>c
		MOVGT	r7,r12			;d=c
		CMN		r7,r12			;d<-c
		RSBMI	r7,r12,#0		;d=-c			
		
		ADD 	r2,r2,r7		;L1+d
		SUB 	r3,r3,r7		;R1-d
		USAT	r2,#8,r2		
		USAT	r3,#8,r3
		STRB	r2,[r0,-r1]		;pL1[z]
		STRB	r3,[r0,#0]		;r[z]

deltaL2h
		LDR 	r2,[r11,#16]	;deltaL2[z]
		LDR 	r3,[r11,#0]		;deltaL[z]
		LDR 	r12,[r11,#32]	;deltaR[z]		
		CMP		r2,#0
		ADD		r3,r3,r2
		RSBMI	r2,r2,#0		;abs(deltaL2[z])
		CMP		r2,lr
		SUBLE	r2,r3,r7
		BGT		deltaR2h
		
		MOVS	r2,r2,ASR #1	;d2			
		BEQ		deltaR2h		;This is added to avoid further caculations if d2=0
								;This may become overhead if probability of d2 taking
								;nonzero value is more								
		CMP		r2,r9			
		MOVGT	r2,r9			
		CMN		r2,r9
		RSBMI	r2,r9,#0
		SUB 	r5,r5,r2
		USAT	r5,#8,r5
		STRB	r5,[r0,-r1,LSL #1]	;pL2[z]

deltaR2h
		CMP		r4,#0
		ADD		r12,r12,r4
		RSBMI	r4,r4,#0
		CMP		r4,lr
		ADDLE	r4,r12,r7		
		BGT		loop2endh
		
		MOVS	r4,r4,ASR #1
		BEQ		loop2endh		;same reason of d2
		
		CMP		r4,r10
		MOVGT	r4,r10
		CMN		r4,r10
		RSBMI	r4,r10,#0
		SUB 	r6,r6,r4
		USAT	r6,#8,r6
		STRB	r6,[r0,r1]		;pR2[z]
		
loop2endh
		 ADD	r11,r11,#4	
		 SUBS	r8,r8,#1				 
		 ADD	r0,r0,#1
		 BGT	loop2h
	;	 BEQ	endh		
		 ADD	sp,sp,#H_Offset_TmpBuff
		 LDMFD	sp!,{r4-r11,pc}
		 
elseh
		LDR		r9,[sp,#H_Stack_Size]	;Cl		
		LDR		r10,[sp,#72]			;Cr
		MOV		r8,#4					;loop variable
		LDR		lr,[sp,#H_Stack_Size+8]	;betaX
		MOV		r11,sp					;deltaL
		MOV		r9,r9,ASR #1
		MOV		r10,r10,ASR #1
		
loop3h	
		LDRB	r2,[r0,-r1] 	;pL1[z]
		LDRB	r3,[r0,#0]		;R1		
		LDR		r4,[sp,#H_Stack_Size+4]	;alpha
		LDRB	r12,[sp,#94]	;c		
		SUBS	r7,r3,r2		;delta	;stall here
		RSBMI	r7,r7,#0		;abs(delta)
		BEQ		loop3endh
		
		SMULBB	r4,r4,r7	
		MOV		r12,r12,ASR #1	;c>>1
		SUB 	r7,r3,r2		;delta
		MOV		r4,r4,ASR #7
		CMP		r4,#3
		ADDLE	r7,r7,#1		
		BGT		loop3endh
		
		MOVS	r7,r7,ASR #1	;d
		BEQ		deltaL22h		;same reason of d

		CMP		r7,r12			;d>c>>1
		MOVGT	r7,r12			;d=c>>1
		CMN		r7,r12			;d<c>>1
		RSBMI	r7,r12,#0		;d=-(c>>1)			
		ADD 	r2,r2,r7		;L1+d
		SUB 	r3,r3,r7		;R1-d
		USAT	r2,#8,r2		
		USAT	r3,#8,r3
		STRB	r2,[r0,-r1]		;pL1[z]
		STRB	r3,[r0,#0]		;r[z]

deltaL22h
		CMP		r5,#1			;Al!=1
		LDRNE 	r2,[r11,#16]	;deltaL2[z]
		LDRNE 	r3,[r11,#0]		;deltaL[z]
		BEQ		deltaR22h

		CMP		r2,#0
		ADD		r3,r2,r3
		RSBMI	r2,r2,#0		;abs(deltaL2[z])
		CMP		r2,lr
		SUBLE	r2,r3,r7		
		BGT		deltaR22h
		
		MOVS	r2,r2,ASR #1	;d2
		BEQ		deltaR22h		;same reason of d2
		
		LDRB	r3,[r0,-r1,LSL #1]	;pL2[z]	
		CMP		r2,r9
		MOVGT	r2,r9	
		CMN		r2,r9
		RSBMI	r2,r9,#0		
		SUB 	r3,r3,r2
		USAT	r3,#8,r3
		STRB	r3,[r0,-r1,LSL #1]	;pL2[z]

deltaR22h
		CMP		r6,#1			;Ar!=1
		LDRNE 	r2,[r11,#48]	;deltaR2[z]
		LDRNE 	r3,[r11,#32]	;deltaR[z]		
		BEQ		loop3endh

		CMP		r2,#0
		ADD		r3,r3,r2
		RSBMI	r2,r2,#0
		CMP		r2,lr
		ADDLE	r2,r3,r7
		BGT		loop3endh
		
		MOVS	r2,r2,ASR #1	;d2
		BEQ		loop3endh		;same reason of d2
		
		LDRB	r3,[r0,r1]		;pR2[z]	
		CMP		r2,r10
		MOVGT	r2,r10
		CMN		r2,r10		
		RSBMI	r2,r10,#0
		SUB 	r2,r3,r2
		USAT	r2,#8,r2
		STRB	r2,[r0,r1]		;pR2[z]	

loop3endh
		 ADD	r11,r11,#4
		 SUBS	r8,r8,#1
		 ADD	r0,r0,#1
		 BGT	loop3h	
endh
		ADD		sp,sp,#H_Offset_TmpBuff
		LDMFD	sp!,{r4-r11,pc}
	ENDP	;//ARMV6_rv9_h_loop_filter
	
	
ARMV6_rv9_v_loop_filter	PROC

        IMPORT ditherR
        IMPORT ditherL

		STMFD	sp!,{r4-r11,lr}
		pld		[r0]
		SUB		r4,r0,#0x03		;rt = pPels		
		SUB		sp,sp,#96
		STR		r1,[sp,#64]		;r1,uPitch
				
		;sp		DeltaL
		;sp+16	DeltaL2	
		;sp+32	DeltaR	
		;sp+48	DeltaR2	

		LDRB	r6,[r4,#1]			;rt[-2]
		LDRB	r7,[r4,#2]			;rt[-1]
		LDRB	r8,[r4,#3]			;rt[0]
		LDRB	r9,[r4,#4]			;rt[1]
		LDRB	r10,[r4,#5]			;rt[2]		
		LDRB	r5,[r4],r1			;rt[-3]
		STR		r2,[sp,#68]		;r2,uMBPos
		SUB		r12,r6,r7			;r12 sum deltaL rt[-2] - rt[-1]
		STR		r3,[sp,#72]		;r3,Cr
		SUB		lr,r9,r8				;lr sum deltaR   r[1] - r[0]
		SUB		r11,r6,r5				;rt[-2] - rt[-3]
		SUB		r2,r9,r10			;rt[1] - rt[2]
		
		LDRB	r6,[r4,#1]
		STR		r12,[sp,#0x00]
		LDRB	r7,[r4,#2]
		STR		lr,[sp,#0x20]		
		LDRB	r8,[r4,#3]
		STR		r11,[sp,#0x10]
		LDRB	r9,[r4,#4]
		STR		r2,[sp,#0x30]				
		LDRB	r10,[r4,#5]		
		SUB		r7,r6,r7		
		LDRB	r5,[r4],r1
		ADD		r12,r12,r7
		SUB		r8,r9,r8		
		STR		r7,[sp,#0x04]		
		ADD		lr,lr,r8
		STR		r8,[sp,#0x24]		
		SUB		r6,r6,r5		
		SUB		r9,r9,r10
		STR		r6,[sp,#0x14]
		ADD		r11,r11,r6
		
		LDRB	r6,[r4,#1]
		LDRB	r7,[r4,#2]
		STR		r9,[sp,#0x34]		
		LDRB	r8,[r4,#3]
		ADD		r2,r2,r9
		LDRB	r9,[r4,#4]
		LDRB	r10,[r4,#5]		
		LDRB	r5,[r4],r1
		SUB		r7,r6,r7
		ADD		r12,r12,r7
		SUB		r8,r9,r8		
		STR		r7,[sp,#0x08]		
		ADD		lr,lr,r8
		STR		r8,[sp,#0x28]
		SUB		r6,r6,r5
		SUB		r9,r9,r10
		STR		r6,[sp,#0x18]
		ADD		r11,r11,r6
		
		LDRB	r5,[r4]
		LDRB	r6,[r4,#1]
		LDRB	r7,[r4,#2]
		STR		r9,[sp,#0x38]
		LDRB	r8,[r4,#3]
		ADD		r2,r2,r9
		LDRB	r9,[r4,#4]
		LDRB	r10,[r4,#5]
		SUB		r7,r6,r7
		ADDS	r12,r12,r7
		STR		r7,[sp,#0x0c]
		RSBMI	r12,r12,#0
		SUB		r8,r9,r8
		ADDS	lr,lr,r8
		STR		r8,[sp,#0x2c]
		RSBMI	lr,lr,#0
		SUB		r6,r6,r5
		STR		r6,[sp,#0x1c]
		SUB		r9,r9,r10
		STR		r9,[sp,#0x3c]
		ADDS	r11,r11,r6		;delta = deltaL2[0]+deltaL2[1]+deltaL2[2]+deltaL2[3];

		LDR		r4,[sp,#0x8c]	;betaX
		RSBMI	r11,r11,#0
		ADDS	r2,r2,r9		;delta = deltaR2[0]+deltaR2[1]+deltaR2[2]+deltaR2[3];
		RSBMI	r2,r2,#0
		MOV		r5,#3			;Al
		MOV		r6,#3			;Ar
		CMP		r12,r4,LSL #2
		MOVGE	r5,#1		
		STRB	r5,[sp,#92]	;Al
		CMP		lr,r4,LSL #2
		MOVGE	r6,#1		
		STRB	r6,[sp,#93]	;Ar

		ADD		r7,r5,r6
		CMP		r7,#2
	;	BLE		endv
		LDRGT	r4,[sp,#0x84]
		ADDLE	sp,sp,#96
		LDMLEFD	sp!,{r4-r11,pc}
		
		
		ADD		r7,r7,r3
		ADD		r7,r7,r4
		LDR		r4,[sp,#0x98]	;bStrong
		MOV		r7,r7,ASR #1
		STRB	r7,[sp,#94]		;c
		
		MOV		r12,#0			;b3SmoothLeft
		MOV		lr,#0			;b3SmoothRight		
		CMP		r4,#0
		BEQ		next1v			;This is different from c code implementation	
		                        ;when bStrong is False go to next1
		CMP		r5,#3			
		MOVEQ	r12,#1
		CMP		r6,#3
		MOVEQ	lr,#1

if2v	LDR		r9,[sp,#0x90]		;beta2
		CMP		r12,#0
		BEQ		if3v		
		CMP		r11,r9
		MOVGE	r12,#0

if3v	CMP		lr,#0
		BEQ		if4v	
		CMP		r2,r9
		MOVGE	lr,#0

if4v	CMP		r12,#0
		CMPNE	lr,#0
		LDRNE	r2,[sp,#68]		
		LDRNE	r4,=ditherL
		LDRNE	r5,=ditherR
		BEQ		next1v
		
		pld		[r0]
		MOV		r12,#0			;z=0
		ADD		r4,r4,r2,LSL #1
		ADD		r5,r5,r2,LSL #1
		STR		r4,[sp,#76]		;dithL
		STR		r5,[sp,#80]		;dithR

loop1v	
		LDRB	r4,[r0,#-1]		;L1
		LDRB	r5,[r0,#0]		;R1
		LDR		r9,[sp,#0x88]	;alpha
		STR		r12,[sp,#84]	;z
		SUBS	r8,r5,r4
		BEQ		loop1endv
		
		RSBMI	r8,r8,#0
		MOV		r12,r12,LSL #1
		SMULBB	r8,r9,r8
		LDRB	r6,[r0,#-2]		;L2	It may be modified
		LDRB	r7,[r0,#1]		;R2
		MOV		r8,r8,ASR #7	;n		
		CMP		r8,#1		
		BGT		loop1endv
		
		STR		r8,[sp,#88]		;n
		LDRB	r8,[r0,#-3]		;L3
		LDRB	r9,[r0,#2]		;R3	
		pld		[r0,r1]
		LDR		r1,[sp,#76]		;dithL
		LDR		r2,[sp,#80]		;dithR
		ADD		r11,r4,r5		;L1 + R1
		ADD		r11,r11,r6		;+L2
		LDRH	r1,[r1,r12]		;d1
		LDRH	r2,[r2,r12]		;d2
		
		ADD		r12,r7,r8		;R2+L3
		ADD		r12,r11,r12		;
		MOV		r3,#25
		SMLABB	r12,r3,r12,r11
		ADD		r11,r11,r7		;+R2
		ADD		lr,r11,r9		
		SUB		r11,r11,r6
		ADD		r12,r12,r1
		
		SMLABB	lr,r3,lr,r11
		LDR		r3,[sp,#88]		;n
		LDRB	r11,[sp,#94]	;c
		MOV		r12,r12,ASR #7	;t1,rt[-1]
		ADD		lr,lr,r2
		
		CMP		r3,#0
		MOV		lr,lr,ASR #7	;t2,rt[0]
		BEQ		rt1v
		
		ADD		r3,r11,r4		;L1+c
		CMP		r12,r3
		MOVGT	r12,r3
		SUB		r3,r4,r11		;L1-c
		CMP		r12,r3
		MOVLT	r12,r3
		ADD		r3,r11,r5		;R1+c
		CMP		lr,r3
		MOVGT	lr,r3
		SUB		r3,r5,r11		;R1-c
		CMP		lr,r3
		MOVLT	lr,r3

rt1v	
		LDRB	r10,[r0,#-4]	;L4
		ADD		r3,r6,r8		;L2+L3
		STRB	r12,[r0,#-1]	;rt[-1]	
		ADD		r3,r3,r12		;+rt[-1] 
		ADD		r5,r5,r10		;R1+L4
		ADD		r5,r5,r3
		ADD		r1,r1,r3		;+d1
		
		MOV		r3,#25
		STRB	lr,[r0,#0]		;rt[0]
		SMLABB	r5,r3,r5,r1
		LDRB	r11,[r0,#3]		;R4
		ADD		r3,r7,r9		;R2+R3
		ADD		r3,r3,lr		;+rt[0]
		ADD		r4,r4,r11		;L1+R4
		ADD		r4,r4,r3
		ADD		r2,r2,r3
		
		MOV		r3,#25
		MOV		r5,r5,ASR #7	;t1,rt[-2]	
		SMLABB	r4,r3,r4,r2
		LDRB	r2,[sp,#94]		;c
		LDR		r1,[sp,#88]		;n
		ADD		r3,r6,r2		;L2+c
		MOV		r4,r4,ASR #7	;t2,rt[1]
		
		CMP		r1,#0
		BEQ		rt2v
		CMP		r5,r3
		MOVGT	r5,r3
		SUB		r3,r6,r2		;L2-c
		CMP		r5,r3
		MOVLT	r5,r3
		ADD		r3,r7,r2		;R2+c
		CMP		r4,r3
		MOVGT	r4,r3
		SUB		r3,r7,r2		;R2-c
		CMP		r4,r3
		MOVLT	r4,r3

rt2v	
		LDR		r1,[sp,#0x94]	;bChroma
		STRB	r5,[r0,#-2]		;rt[-2]
		STRB	r4,[r0,#1]		;rt[0]
		CMP		r1,#0
		ADD		r10,r10,r5		;L4+rt[-2]
		BNE		loop1endv
		
		ADD		r10,r10,r8
		ADD		r12,r12,r8
		ADD		r12,r12,r10
		MOV     r1,#25
		SMLABB	r12,r1,r12,r10
		ADD		r11,r11,r9		;R4 + R3
		ADD		r9,r9,lr		;R3+rt[0]
		ADD		r11,r11,r4		;rt[1]
		ADD		r9,r9,r11
		ADD		r12,r12,#64
		
		MLA		r9,r1,r9,r11
		MOV		r12,r12,ASR #7
		STRB	r12,[r0,#-3]
		ADD		r9,r9,#64
		MOV		r9,r9,ASR #7
		STRB	r9,[r0,#2]	

loop1endv 
		 LDR		r12,[sp,#84]	
		 LDR		r1,[sp,#64]
		 ADD		r12,r12,#1
		 CMP		r12,#4
		 ADD		r0,r0,r1
		 BLT		loop1v
	;	 BGE		endv
		 ADD		sp,sp,#96
		 LDMFD		sp!,{r4-r11,pc}

next1v	LDRB	r5,[sp,#92]		;Al
		LDRB	r6,[sp,#93]		;Ar	
		LDR		r1,[sp,#64]		;uPicth
		CMP		r5,#1			;stall
		CMPNE	r6,#1
		BEQ		elsev
				
		LDR		lr,[sp,#0x8c]	;betaX
		MOV		r11,sp			;deltaL		
		LDR		r9,[sp,#0x84]	;Cl		
		LDR		r10,[sp,#72]	;Cr
		
		MOV		r8,#4			;loop variable
loop2v	
		LDRB	r2,[r0,#-1] 	;L1
		LDRB	r3,[r0,#0]		;R1		
		LDR		r4,[sp,#0x88]	;alpha
		LDRB	r12,[sp,#94]	;c
		SUBS	r7,r3,r2		;delta
		RSBMI	r7,r7,#0		;abs(delta)
		BEQ		loop2endv
		
		SMULBB	r4,r4,r7		
		LDRB	r5,[r0,#-2]		;L2
		LDRB	r6,[r0,#1]		;R2
		pld		[r0,r1]
		SUB 	r7,r3,r2		;delta
		MOV		r4,r4,ASR #7
		CMP		r4,#2
		BGT		loop2endv
		
		ADD		r7,r5,r7,LSL #2	;L2+(delta<<2)
		SUB		r7,r7,r6		;-R2
		ADD		r7,r7,#4
		LDR 	r4,[r11,#48]	;deltaR2[z]
		MOVS	r7,r7,ASR #3	;d
		BEQ		deltaL2v		;This is added to avoid further caculations if d=0
		
		CMP		r7,r12			;d>c
		MOVGT	r7,r12			;d=c
		CMN		r7,r12			;d<-c
		RSBMI	r7,r12,#0		;d=-c					
		ADD		r2,r2,r7		;L1+d
		SUB 	r3,r3,r7		;R1-d
		USAT	r2,#8,r2		
		USAT	r3,#8,r3
		STRB	r2,[r0,#-1]		;rt[-1]
		STRB	r3,[r0,#0]		;rt[0]

deltaL2v	
		LDR 	r2,[r11,#16]	;deltaL2[z]
		LDR 	r3,[r11,#0]		;deltaL[z]
		LDR 	r12,[r11,#32]	;deltaR[z]		
		CMP		r2,#0
		ADD		r3,r3,r2
		RSBMI	r2,r2,#0		;abs(deltaL2[z]
		CMP		r2,lr
		SUBLE	r2,r3,r7
		BGT		deltaR2v
		
		MOVS	r2,r2,ASR #1	;d2			
		BEQ		deltaR2v		;This is added to avoid further caculations if d2=0
		
		CMP		r2,r9			;This may become overhead if probability of d2 taking
		MOVGT	r2,r9			;nonzero value is more
		CMN		r2,r9
		RSBMI	r2,r9,#0
		SUB 	r5,r5,r2
		USAT	r5,#8,r5	
		STRB	r5,[r0,#-2]		;rt[-2]

deltaR2v
		CMP		r4,#0
		ADD		r12,r12,r4
		RSBMI	r4,r4,#0
		CMP		r4,lr
		ADDLE	r4,r12,r7		
		BGT		loop2endv
		
		MOVS	r4,r4,ASR #1
		BEQ		loop2endv		;same reason of d2
		
		CMP		r4,r10
		MOVGT	r4,r10
		CMN		r4,r10
		RSBMI	r4,r10,#0
		SUB 	r6,r6,r4
		USAT	r6,#8,r6
		STRB	r6,[r0,#1]
loop2endv
		 ADD	r11,r11,#4	
		 SUBS	r8,r8,#1				 
		 ADD	r0,r0,r1
		 BGT	loop2v
	;	 BEQ	endv	
		 ADD	sp,sp,#96
		 LDMFD	sp!,{r4-r11,pc}
		 	
elsev	
		LDRB	r12,[sp,#94]	;c>>1		
		LDR		r9,[sp,#0x84]	;Cl		
		LDR		lr,[sp,#0x8c]	;betaX
		MOV		r11,sp			;deltaL
		MOV		r8,#4			;loop variable
		LDR		r4,[sp,#0x88]	;alpha
		MOV		r12,r12,ASR #1
		MOV		r9,r9,ASR #1
		
loop3v	
		LDRB	r2,[r0,#-1] 	;L1
		LDRB	r3,[r0,#0]		;R1		
		pld		[r0,r1]
		SUBS	r7,r3,r2		;delta	;stall here
		RSBMI	r7,r7,#0		;abs(delta)
		BEQ		loop3endv
		
		SMULBB	r10,r4,r7		
		SUB 	r7,r3,r2		;delta
		ADD		r7,r7,#1		;
		MOV		r10,r10,ASR #7
		CMP		r10,#3
		BGT		loop3endv
		
		MOVS	r7,r7,ASR #1	;d
		BEQ		deltaL22v		;same reason of d

		CMP		r7,r12			;d>c>>1
		MOVGT	r7,r12			;d=c>>1
		CMN		r7,r12			;d<c>>1
		RSBMI	r7,r12,#0		;d=-(c>>1)			
		ADD 	r2,r2,r7		;L1+d
		SUB 	r3,r3,r7		;R1-d
		USAT	r2,#8,r2		
		USAT	r3,#8,r3
		STRB	r2,[r0,#-1]		;rt[-1]
		STRB	r3,[r0,#0]		;rt[0]

deltaL22v
		CMP		r5,#1			;Al!=1
		BEQ		deltaR22v
		
		LDR 	r2,[r11,#16]	;deltaL2[z]
		LDR 	r3,[r11,#0]		;deltaL[z]							
		CMP		r2,#0
		ADD		r3,r2,r3
		RSBMI	r2,r2,#0		;abs(deltaL2[z]
		CMP		r2,lr
		SUBLE	r2,r3,r7		
		BGT		deltaR22v
		
		MOVS	r2,r2,ASR #1	;d2
		BEQ		deltaR22v		;same reason of d2
		
		LDRB	r3,[r0,#-2]		;rt[-2]	
		CMP		r2,r9
		MOVGT	r2,r9	
		CMN		r2,r9
		RSBMI	r2,r9,#0		
		SUB 	r3,r3,r2
		USAT	r3,#8,r3
		STRB	r3,[r0,#-2]		;rt[-2]	;stall

deltaR22v
		CMP		r6,#1			;Ar!=1
		BEQ		loop3endv
		
		LDR 	r2,[r11,#48]	;deltaR2[z]
		LDR 	r3,[r11,#32]	;deltaR[z]				
		LDR		r10,[sp,#72]	;Cr
		CMP		r2,#0
		ADD		r3,r3,r2
		RSBMI	r2,r2,#0
		CMP		r2,lr
		ADDLE	r2,r3,r7
		BGT		loop3endv
		
		MOVS	r2,r2,ASR #1	;d2
		BEQ		loop3endv		;same reason of d2
		
		MOV		r10,r10,ASR #1
		LDRB	r3,[r0,#1]		;rt[1]	
		CMP		r2,r10
		MOVGT	r2,r10
		CMN		r2,r10		
		RSBMI	r2,r10,#0
		SUB		r2,r3,r2
		USAT	r2,#8,r2
		STRB	r2,[r0,#1]

loop3endv
		 ADD	r11,r11,#4
		 SUBS	r8,r8,#1
		 ADD	r0,r0,r1
		 BGT	loop3v	
endv
		ADD		sp,sp,#96
		LDMFD	sp!,{r4-r11,pc}
		ENDP
		EXPORT	ARMV6_rv9_v_loop_filter
		
ARMV6_rv8_edge_filter	PROC
	 	STMFD	sp!,{r4-r11,lr}
		SUB		sp,sp,#4
		STR		r0,[sp,#0x00]	;pRec
		LDR		r4,[sp,#0x28]	;pStrengthV(r4)		 
		RSB		r6,r2,r1,LSL #2
		ADD		r6,r6,#4		;update factor
		ADD		r0,r0,#0x04		;r(r0)
		MOV		lr,r3			;j
Ln1		
		ADD		r4,r4,#1		;pStrengthV += 1
		SUB		r5,r2,#4		;i(r5)
Ln2		
		LDRB	r8,[r4],#1		;lTemp
		MOV		r7,r0			;lpr(r7) = r
		CMP		r8,#0
		BEQ		Ln3
		LDRB	r9,[r7,#-0x02]	;lpr[-2]
		LDRB	r10,[r7,#0x01]	;lpr[ 1]
		LDRB	r11,[r7,#-0x01]	;lpr[-1]
		LDRB	r12,[r7,#0x00]	;lpr[ 0]
		SUB		r9,r9,r10
		SUB		r10,r11,r12		;stall
		SUB		r9,r9,r10,LSL #2
		MOVS	r9,r9,ASR #3	;d(r9)
		BEQ		Ln7
		CMP		r9,r8
		MOVGT	r9,r8
		CMN		r9,r8
		RSBMI	r9,r8,#0x00
		ADD		r11,r11,r9
		USAT	r11,#8,r11	
		SUB		r12,r12,r9
		USAT	r12,#8,r12
		STRB	r11,[r7,#-1]
		STRB	r12,[r7,#0]

Ln7		ADD		r7,r7,r1		;lpr += uPitch
		LDRB	r9,[r7,#-0x02]	;lpr[-2]
		LDRB	r10,[r7,#0x01]	;lpr[ 1]
		LDRB	r11,[r7,#-0x01]	;lpr[-1]
		LDRB	r12,[r7,#0x00]	;lpr[ 0]		
		SUB		r9,r9,r10
		SUB		r10,r11,r12		;stall
		SUB		r9,r9,r10,LSL #2
		MOVS	r9,r9,ASR #3	;d(r9)
		BEQ		Ln8
		CMP		r9,r8
		MOVGT	r9,r8
		CMN		r9,r8
		RSBMI	r9,r8,#0x00
		ADD		r11,r11,r9
		USAT	r11,#8,r11	
		SUB		r12,r12,r9
		USAT	r12,#8,r12
		STRB	r11,[r7,#-1]
		STRB	r12,[r7,#0]

Ln8		ADD		r7,r7,r1		;lpr += uPitch
		LDRB	r9,[r7,#-0x02]	;lpr[-2]
		LDRB	r10,[r7,#0x01]	;lpr[ 1]
		LDRB	r11,[r7,#-0x01]	;lpr[-1]
		LDRB	r12,[r7,#0x00]	;lpr[ 0]		
		SUB		r9,r9,r10
		SUB		r10,r11,r12		;stall
		SUB		r9,r9,r10,LSL #2
		MOVS	r9,r9,ASR #3	;d(r9)
		BEQ		Ln9
		CMP		r9,r8
		MOVGT	r9,r8
		CMN		r9,r8
		RSBMI	r9,r8,#0x00
		ADD		r11,r11,r9
		USAT	r11,#8,r11	
		SUB		r12,r12,r9
		USAT	r12,#8,r12
		STRB	r11,[r7,#-1]
		STRB	r12,[r7,#0]

Ln9		ADD		r7,r7,r1		;lpr += uPitch
		LDRB	r9,[r7,#-0x02]	;lpr[-2]
		LDRB	r10,[r7,#0x01]	;lpr[ 1]
		LDRB	r11,[r7,#-0x01]	;lpr[-1]
		LDRB	r12,[r7,#0x00]	;lpr[ 0]		
		SUB		r9,r9,r10
		SUB		r10,r11,r12		;stall
		SUB		r9,r9,r10,LSL #2
		MOVS	r9,r9,ASR #3	;d(r9)
		BEQ		Ln3
		CMP		r9,r8
		MOVGT	r9,r8
		CMN		r9,r8
		RSBMI	r9,r8,#0x00
		ADD		r11,r11,r9
		USAT	r11,#8,r11	
		SUB		r12,r12,r9
		USAT	r12,#8,r12
		STRB	r11,[r7,#-1]
		STRB	r12,[r7,#0]
		
Ln3		ADD		r0,r0,#4
		SUBS	r5,r5,#4
		BGT		Ln2
		ANDS    r11,r2,#0x0f
		ADDNE   r4,r4,#2 
		ADD		r0,r0,r6
		SUBS	lr,lr,#4
		BGT		Ln1


		;filter horizontal edges
		LDR		r0,[sp,#0x00]	;pRec
		LDR		r4,[sp,#0x2c]	;pStrengthH
		LDR     r5,[sp,#0x30]   ;row
		MOV     r6,r3           ;Height
		MOV     r7,r0
		CMP     r5,#0           ;if 0 == row
		MOVEQ   r11,r2          ;huwei 20090617 real_TCK
		ADDEQ   r11,r11,#15     ;huwei 20090617 real_TCK 
		MOVEQ   r11,r11, ASR #4  ;huwei 20090617 real_TCK
		MOVEQ   r11,r11, LSL #4  ;huwei 20090617 real_TCK
		ADDEQ	r7,r7,r1,LSL #2	;lpr(r7)
		;ADDEQ	r4,r4,r2,ASR #2 ;pStrengthH += (uWidth >> 2); 
		ADDEQ	r4,r4,r11,ASR #2 ;pStrengthH += ((((uWidth + 15) >> 4) << 4) >> 2);ADDEQ	r4,r4,r2,ASR #2 ;huwei 20090617 real_TCK
		SUBEQ	r6,r6,#4		;j(r6)	
		RSB		r3,r2,r1,LSL #2	;update factor	
Ln4
		MOV		r5,r2			;i(r5)
Ln5		
		LDRB	r8,[r4],#1
		CMP		r8,#0			;stall
		BEQ		Ln6
		LDRB	r9,[r7,-r1,LSL #1]
		LDRB	r10,[r7,r1]
		LDRB	r11,[r7,-r1]
		LDRB	r12,[r7,#0x00]
		SUB		r9,r9,r10
		SUB		r9,r9,r11,LSL #2
		ADD		r9,r9,r12,LSL #2
		MOVS	r9,r9,ASR #3	;d(r9)
		BEQ		Ln10
		CMP		r9,r8
		MOVGT	r9,r8
		CMN		r9,r8
		RSBMI	r9,r8,#0
		ADD		r11,r11,r9
		USAT	r11,#8,r11
		SUB		r12,r12,r9
		USAT	r12,#8,r12
		STRB	r11,[r7,-r1]
		STRB	r12,[r7,#0x00]

Ln10	ADD		r7,r7,#1
		LDRB	r9,[r7,-r1,LSL #1]
		LDRB	r10,[r7,r1]
		LDRB	r11,[r7,-r1]
		LDRB	r12,[r7,#0x00]
		SUB		r9,r9,r10
		SUB		r9,r9,r11,LSL #2
		ADD		r9,r9,r12,LSL #2
		MOVS	r9,r9,ASR #3	;d(r9)
		BEQ		Ln11
		CMP		r9,r8
		MOVGT	r9,r8
		CMN		r9,r8
		RSBMI	r9,r8,#0
		ADD		r11,r11,r9
		USAT	r11,#8,r11
		SUB		r12,r12,r9
		USAT	r12,#8,r12
		STRB	r11,[r7,-r1]
		STRB	r12,[r7,#0x00]

Ln11	ADD		r7,r7,#1
		LDRB	r9,[r7,-r1,LSL #1]
		LDRB	r10,[r7,r1]
		LDRB	r11,[r7,-r1]
		LDRB	r12,[r7,#0x00]
		SUB		r9,r9,r10
		SUB		r9,r9,r11,LSL #2
		ADD		r9,r9,r12,LSL #2
		MOVS	r9,r9,ASR #3	;d(r9)
		BEQ		Ln12
		CMP		r9,r8
		MOVGT	r9,r8
		CMN		r9,r8
		RSBMI	r9,r8,#0
		ADD		r11,r11,r9
		USAT	r11,#8,r11
		SUB		r12,r12,r9
		USAT	r12,#8,r12
		STRB	r11,[r7,-r1]
		STRB	r12,[r7,#0x00]

Ln12	ADD		r7,r7,#1
		LDRB	r9,[r7,-r1,LSL #1]
		LDRB	r10,[r7,r1]
		LDRB	r11,[r7,-r1]
		LDRB	r12,[r7,#0x00]
		SUB		r9,r9,r10
		SUB		r9,r9,r11,LSL #2
		ADD		r9,r9,r12,LSL #2
		MOVS	r9,r9,ASR #3	;d(r9)
		BEQ		Ln13
		CMP		r9,r8
		MOVGT	r9,r8
		CMN		r9,r8
		RSBMI	r9,r8,#0
		ADD		r11,r11,r9
		USAT	r11,#8,r11
		SUB		r12,r12,r9
		USAT	r12,#8,r12
		STRB	r11,[r7,-r1]
		STRB	r12,[r7,#0x00]
Ln13	SUB		r7,r7,#3

Ln6		ADD		r7,r7,#4
		SUBS	r5,r5,#4
		BGT		Ln5
		ADD		r7,r7,r3
		ANDS    r11,r2,#0x0f ;//huwei 20090617 real_TCK
		ADDNE   r4,r4,#2
		SUBS	r6,r6,#4 
		BGT		Ln4
		ADD		sp,sp,#4
		LDMFD	sp!,{r4-r11,pc}
		ENDP
		EXPORT	ARMV6_rv8_edge_filter
	END