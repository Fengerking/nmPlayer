
	AREA DEBLOCK, CODE, READONLY
ARMV4_rv9_h_loop_filter	PROC
        IMPORT ditherR
        IMPORT ditherL
		
		STMFD	sp!,{r4-r11,lr}
		SUB		sp,sp,#96
		
		STR		r1,[sp,#64]		;r1,uPitch
		STR		r2,[sp,#68]		;r2,uMBPos
		STR		r3,[sp,#72]		;r3,Cr
				
		;sp		DeltaL
		;sp+16	DeltaL2	
		;sp+32	DeltaR	
		;sp+48	DeltaR2	

		SUB		r4,r0,r1		;PL1
		SUB		r12,r4,r1		;PL2		

		LDRB	r5,[r12,#0]		;pL2[0]
		LDRB	r6,[r4,#0]		;pL1[0]
		SUB		lr,r12,r1		;PL3
		LDRB	r7,[lr,#0]		;pL3[0]
		SUB		r8,r5,r6
		STR		r8,[sp,#0x00]	;r8 sum deltaL
		SUB		r7,r5,r7		

		LDRB	r5,[r12,#1]
		LDRB	r6,[r4,#1]
		STR		r7,[sp,#0x10]
		LDRB	r7,[lr,#1]
		SUB		r6,r5,r6
		STR		r6,[sp,#0x04]
		ADD		r8,r6,r8		
		SUB		r7,r5,r7		

		LDRB	r5,[r12,#2]
		LDRB	r6,[r4,#2]
		STR		r7,[sp,#0x14]
		LDRB	r7,[lr,#2]
		SUB		r6,r5,r6
		STR		r6,[sp,#0x08]
		ADD		r8,r6,r8
		SUB		r7,r5,r7		

		LDRB	r5,[r12,#3]
		LDRB	r6,[r4,#3]
		STR		r7,[sp,#0x18]
		LDRB	r7,[lr,#3]
		SUB		r6,r5,r6
		STR		r6,[sp,#0x0c]
		ADDS	r8,r6,r8
		SUB		r7,r5,r7
		STR		r7,[sp,#0x1c]
		RSBMI	r8,r8,#0	;abs(sum deltaL)

		ADD		r12,r0,r1	;PR2		
		LDRB	r5,[r12,#0]
		LDRB	r6,[r0,#0]
		ADD		lr,r12,r1	;PR3
		LDRB	r7,[lr,#0]
		SUB		r9,r5,r6	;r9 (sum deltaR)	
		STR		r9,[sp,#0x20]
		SUB		r7,r5,r7		

		LDRB	r5,[r12,#1]
		LDRB	r6,[r0,#1]
		STR		r7,[sp,#0x30]
		LDRB	r7,[lr,#1]
		SUB		r6,r5,r6
		STR		r6,[sp,#0x24]
		ADD		r9,r6,r9		
		SUB		r7,r5,r7		

		LDRB	r5,[r12,#2]
		LDRB	r6,[r0,#2]
		STR		r7,[sp,#0x34]
		LDRB	r7,[lr,#2]
		SUB		r6,r5,r6
		STR		r6,[sp,#0x28]
		ADD		r9,r6,r9
		SUB		r7,r5,r7		

		LDRB	r5,[r12,#3]
		LDRB	r6,[r0,#3]
		STR		r7,[sp,#0x38]
		LDRB	r7,[lr,#3]
		SUB		r6,r5,r6
		STR		r6,[sp,#0x2c]
		ADDS	r9,r6,r9
		SUB		r7,r5,r7
		STR		r7,[sp,#0x3c]
		RSBMI	r9,r9,#0		;abs(sum deltal)	


		LDR		r4,[sp,#0x8c]	;betaX
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
		BLE		endh
		LDR		r4,[sp,#0x84]
		ADD		r7,r7,r3
		ADD		r7,r7,r4
		MOV		r7,r7,ASR #1
		STRB	r7,[sp,#94]		;c

		LDR		r4,[sp,#0x98]	;bStrong
		MOV		r12,#0			;b3SmoothLeft
		MOV		lr,#0			;b3SmoothRight		
		CMP		r4,#0
		BEQ		next1h			;This is different from c code implementation	
		CMP		r5,#3			;when bStrong is False go to next1
		MOVEQ	r12,#1
		CMP		r6,#3
		MOVEQ	lr,#1

if2h	LDR		r9,[sp,#0x90]	;beta2
		CMP		r12,#0
		BEQ		if3h
		LDR		r4,[sp,#0x10]
		LDR		r5,[sp,#0x14]
		LDR		r6,[sp,#0x18]
		ADD		r8,r4,r5
		LDR		r7,[sp,#0x1c]
		ADD		r8,r8,r6
		ADDS	r8,r8,r7		
		RSBMI	r8,r8,#0
		CMP		r8,r9
		MOVGE	r12,#0

if3h	CMP		lr,#0
		BEQ		if4h		
		LDR		r4,[sp,#0x30]
		LDR		r5,[sp,#0x34]
		LDR		r6,[sp,#0x38]
		ADD		r8,r4,r5
		LDR		r7,[sp,#0x3c]
		ADD		r8,r8,r6
		ADDS	r8,r8,r7
		RSBMI	r8,r8,#0
		CMP		r8,r9
		MOVGE	lr,#0

if4h	CMP		r12,#0
		CMPNE	lr,#0		
		BEQ		next1h
		LDR		r4,=ditherL
		LDR		r5,=ditherR
		ADD		r4,r4,r2,LSL #1
		ADD		r5,r5,r2,LSL #1
		STR		r4,[sp,#76]		;dithL
		STR		r5,[sp,#80]		;dithR

		MOV		r12,#0			;z=0
loop1h	
		LDR		r1,[sp,#64]
		STR		r12,[sp,#84]	;z
		SUB		r3,r0,r1,LSL #1
		SUB		r3,r3,r1
		LDRB	r8,[r3],r1		;L3
		LDRB	r6,[r3],r1		;L2		
		LDRB	r4,[r3],r1		;L1
		LDRB	r5,[r3],r1		;R1		
		LDRB	r7,[r3],r1		;R2		
		LDRB	r9,[r3],r1		;R3
		SUBS	r10,r5,r4
		BEQ		loop1endh
		LDR		r11,[sp,#0x88]	;alpha
		RSBMI	r10,r10,#0
		MUL		r10,r11,r10
		MOV		r10,r10,ASR #7	;n		
		CMP		r10,#1		
		BGT		loop1endh
		STR		r10,[sp,#88]		;n			
		
		LDR		r1,[sp,#76]
		LDR		r2,[sp,#80]
		MOV		r12,r12,LSL #1
		LDRH	r1,[r1,r12]		;d1
		LDRH	r2,[r2,r12]		;d2

		ADD		r11,r4,r5		;L1 + R1
		ADD		r11,r11,r6		;+L2
		ADD		r12,r7,r8		;R2+L3
		ADD		r12,r11,r12		;
		MOV		r3,#25
		MLA	r12,r3,r12,r11
		ADD		r11,r4,r5		;L1+R1
		ADD		r12,r12,r1
		MOV		r12,r12,ASR #7	;t1,pL1[z]
		
		ADD		r11,r11,r7		;+R2
		ADD		lr,r6,r9		;L2 + R3
		ADD		lr,lr,r11
		MLA	lr,r3,lr,r11
		LDR		r3,[sp,#88]		;n
		ADD		lr,lr,r2
		MOV		lr,lr,ASR #7	;t2,r[z]

		LDRB	r11,[sp,#94]	;c
		CMP		r3,#0
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
		MLA	r5,r3,r5,r1
		ADD		r3,r7,r9		;R2+R3
		MOV		r5,r5,ASR #7	;t1,rt[-2]	

		ADD		r3,r3,lr		;+rt[0]
		ADD		r4,r4,r11		;L1+R4
		ADD		r4,r4,r3
		ADD		r2,r2,r3
		MOV		r3,#25
		MLA	r4,r3,r4,r2
		LDR		r1,[sp,#88]		;n
		MOV		r4,r4,ASR #7	;t2,rt[1]
		
		LDRB	r2,[sp,#94]		;c
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
		STRB	r5,[r0,-r3,LSL #1]	;PL2[z]	;stall
		STRB	r4,[r0,r3]			;PR2[z]

		LDR		r1,[sp,#0x94]	;bChroma
		ADD		r10,r10,r5		;L4+rt[-2]
		CMP		r1,#0
		BNE		loop1endh
		ADD		r10,r10,r8
		ADD		r12,r12,r8
		ADD		r12,r12,r10
		MOV     r1,#25
		MLA	r12,r1,r12,r10
		ADD		r11,r11,r9		;R4 + R3
		ADD		r12,r12,#64		
		MOV		r12,r12,ASR #7		
		
		ADD		r11,r11,r4		;rt[1]
		ADD		r9,r9,lr		;R3+rt[0]
		ADD		r9,r9,r11
		MLA	r9,r1,r9,r11
		LDR		r3,[sp,#64]
		ADD		r9,r9,#64		
		MOV		r9,r9,ASR #7
		STRB	r9,[r0,r3,LSL #1]	;PR3[z]
		ADD		r3,r3,r3,LSL #1
		STRB	r12,[r0,-r3]		;PL3[z]	

loop1endh
		 LDR		r12,[sp,#84]	
		 ADD		r0,r0,#1
		 ADD		r12,r12,#1
		 CMP		r12,#4
		 BLT		loop1h
		 BGE		endh

next1h	LDRB	r5,[sp,#92]		;Al
		LDRB	r6,[sp,#93]		;Ar	
		LDR		r1,[sp,#64]		;uPicth
		CMP		r5,#1			;stall
		CMPNE	r6,#1
		BEQ		elseh
				
		LDR		r4,[sp,#0x88]	;alpha
		LDR		lr,[sp,#0x8c]	;betaX
		LDRB	r12,[sp,#94]	;c
		MOV		r11,sp			;deltaL		
		LDR		r9,[sp,#0x84]	;Cl		
		LDR		r10,[sp,#72]	;Cr
		
		MOV		r8,#4			;loop variable
loop2h	LDRB	r2,[r0,-r1] 		;L1;pL1[z];
		LDRB	r3,[r0,#0]			;R1;r[z]		
		LDRB	r5,[r0,-r1,LSL #1]	;L2;pL2[z];
		LDRB	r6,[r0,r1]			;R2;pR2[z];
		SUBS	r7,r3,r2		;delta
		BEQ		loop2endh
		RSBMI	r7,r7,#0		;abs(delta)
		MUL		r7,r4,r7		
		MOV		r7,r7,ASR #7
		CMP		r7,#2
		BGT		loop2endh
		SUB		r7,r3,r2		;delta
		ADD		r7,r5,r7,LSL #2	;L2+(delta<<2)
		SUB		r7,r7,r6		;-R2
		ADD		r7,r7,#4
		MOVS	r7,r7,ASR #3	;d
		BEQ		deltaL2h		;This is added to avoid further caculations if d=0
		
		CMP		r7,r12			;d>c
		MOVGT	r7,r12			;d=c
		CMN		r7,r12			;d<-c
		RSBMI	r7,r12,#0		;d=-c			
		
		ADDS	r2,r2,r7		;L1+d
		MOVMI	r2,#0
		CMP		r2,#255
		MOVGT	r2,#255
		STRB	r2,[r0,-r1]		;pL1[z]
		SUBS	r3,r3,r7		;R1-d
		MOVMI	r3,#0
		CMP		r3,#255
		MOVGT	r3,#255
		STRB	r3,[r0,#0]		;r[z]

deltaL2h
		LDR 	r2,[r11,#16]	;deltaL2[z]
		LDR 	r3,[r11,#0]		;deltaL[z]
		CMP		r2,#0
		ADD		r3,r3,r2
		RSBMI	r2,r2,#0		;abs(deltaL2[z]
		CMP		r2,lr
		BGT		deltaR2h
		SUB		r2,r3,r7
		MOVS	r2,r2,ASR #1	;d2			
		BEQ		deltaR2h		;This is added to avoid further caculations if d2=0
		CMP		r2,r9			;This may become overhead if probability of d2 taking
		MOVGT	r2,r9			;nonzero value is more
		CMN		r2,r9
		RSBMI	r2,r9,#0
		SUBS	r5,r5,r2
		MOVMI	r5,#0			;clamp table is not used,even if used same performance
		CMP		r5,#255
		MOVGT	r5,#255
		STRB	r5,[r0,-r1,LSL #1]	;pL2[z]

deltaR2h
		LDR 	r2,[r11,#48]	;deltaR2[z]
		LDR 	r3,[r11,#32]	;deltaR[z]		
		CMP		r2,#0
		ADD		r3,r3,r2
		RSBMI	r2,r2,#0
		CMP		r2,lr
		BGT		loop2endh
		ADD		r2,r3,r7		
		MOVS	r2,r2,ASR #1
		BEQ		loop2endh		;same reason of d2
		CMP		r2,r10
		MOVGT	r2,r10
		CMN		r2,r10
		RSBMI	r2,r10,#0
		SUBS	r6,r6,r2
		MOVMI	r6,#0			;clamp table is not used
		CMP		r6,#255
		MOVGT	r6,#255
		STRB	r6,[r0,r1]		;pR2[z]
loop2endh
		 ADD	r11,r11,#4	
		 ADD	r0,r0,#1
		 SUBS	r8,r8,#1				 
		 BGT	loop2h
		 BEQ	endh		
elseh
		LDR		r4,[sp,#0x88]	;alpha
		LDR		lr,[sp,#0x8c]	;betaX
		LDRB	r12,[sp,#94]	;c>>1		
		MOV		r11,sp			;deltaL
		LDR		r9,[sp,#0x84]	;Cl		
		LDR		r10,[sp,#72]	;Cr
		MOV		r12,r12,ASR #1
		MOV		r9,r9,ASR #1
		MOV		r10,r10,ASR #1
		
		MOV		r8,#4			;loop variable
loop3h	LDRB	r2,[r0,-r1] 	;pL1[z]
		LDRB	r3,[r0,#0]		;R1		
		SUBS	r7,r3,r2		;delta	;stall here
		BEQ		loop3endh
		RSBMI	r7,r7,#0		;abs(delta)
		MUL		r7,r4,r7		
		MOV		r7,r7,ASR #7
		CMP		r7,#3
		BGT		loop3endh
		SUB 	r7,r3,r2		;delta
		ADD		r7,r7,#1		;
		MOVS	r7,r7,ASR #1	;d
		BEQ		deltaL22h		;same reason of d

		CMP		r7,r12			;d>c>>1
		MOVGT	r7,r12			;d=c>>1
		CMN		r7,r12			;d<c>>1
		RSBMI	r7,r12,#0		;d=-(c>>1)			
		
		ADDS	r2,r2,r7		;L1+d
		MOVMI	r2,#0
		CMP		r2,#255
		MOVGT	r2,#255	
		STRB	r2,[r0,-r1]		;pL1[z]
		SUBS	r3,r3,r7		;R1-d
		MOVMI	r3,#0
		CMP		r3,#255
		MOVGT	r3,#255	
		STRB	r3,[r0,#0]		;r[z]

deltaL22h
		CMP		r5,#1			;Al!=1
		BEQ		deltaR22h
		LDR 	r2,[r11,#16]	;deltaL2[z]
		LDR 	r3,[r11,#0]		;deltaL[z]							
		CMP		r2,#0
		ADD		r3,r2,r3
		RSBMI	r2,r2,#0		;abs(deltaL2[z]
		CMP		r2,lr
		BGT		deltaR22h
		SUB		r2,r3,r7		
		MOVS	r2,r2,ASR #1	;d2
		BEQ		deltaR22h		;same reason of d2
		LDRB	r3,[r0,-r1,LSL #1]	;pL2[z]	
		CMP		r2,r9
		MOVGT	r2,r9	
		CMN		r2,r9
		RSBMI	r2,r9,#0		
		SUBS	r3,r3,r2
		MOVMI	r3,#0
		CMP		r3,#255
		MOVGT	r3,#255
		STRB	r3,[r0,-r1,LSL #1]	;pL2[z]

deltaR22h
		CMP		r6,#1			;Ar!=1
		BEQ		loop3endh
		LDR 	r2,[r11,#48]	;deltaR2[z]
		LDR 	r3,[r11,#32]	;deltaR[z]				
		CMP		r2,#0
		ADD		r3,r3,r2
		RSBMI	r2,r2,#0
		CMP		r2,lr
		BGT		loop3endh
		ADD		r2,r3,r7
		MOVS	r2,r2,ASR #1	;d2
		BEQ		loop3endh		;same reason of d2
		LDRB	r3,[r0,r1]		;pR2[z]	
		CMP		r2,r10
		MOVGT	r2,r10
		CMN		r2,r10		
		RSBMI	r2,r10,#0
		SUBS	r2,r3,r2
		MOVMI	r2,#0
		CMP		r2,#255
		MOVGT	r2,#255
		STRB	r2,[r0,r1]		;pR2[z]	

loop3endh
		 ADD	r11,r11,#4
		 ADD	r0,r0,#1
		 SUBS	r8,r8,#1
		 BGT	loop3h	
endh
		ADD		sp,sp,#96
		LDMFD	sp!,{r4-r11,pc}
	ENDP
	EXPORT	ARMV4_rv9_h_loop_filter
	

ARMV4_rv9_v_loop_filter	PROC
        IMPORT ditherR
        IMPORT ditherL

		STMFD	sp!,{r4-r11,lr}
		SUB		sp,sp,#96
		
		STR		r1,[sp,#64]		;r1,uPitch
		STR		r2,[sp,#68]		;r2,uMBPos
		STR		r3,[sp,#72]		;r3,Cr
				
		;sp		DeltaL
		;sp+16	DeltaL2	
		;sp+32	DeltaR	
		;sp+48	DeltaR2	

		MOV		r4,r0			;rt = pPels
		LDRB	r5,[r4,#-3]
		LDRB	r6,[r4,#-2]
		LDRB	r7,[r4,#-1]
		LDRB	r8,[r4,#0]
		LDRB	r9,[r4,#1]
		LDRB	r10,[r4,#2]
		SUB		r12,r6,r7		;r12 sum deltaL
		SUB		lr,r9,r8		;lr sum deltaR
		STR		r12,[sp,#0x00]		
		STR		lr,[sp,#0x20]		
		SUB		r6,r6,r5
		SUB		r9,r9,r10
		STR		r6,[sp,#0x10]		
		STR		r9,[sp,#0x30]		

		ADD		r4,r4,r1
		LDRB	r5,[r4,#-3]
		LDRB	r6,[r4,#-2]
		LDRB	r7,[r4,#-1]
		LDRB	r8,[r4,#0]
		LDRB	r9,[r4,#1]
		LDRB	r10,[r4,#2]
		SUB		r7,r6,r7
		ADD		r12,r12,r7
		SUB		r8,r9,r8
		ADD		lr,lr,r8
		STR		r7,[sp,#0x04]		
		STR		r8,[sp,#0x24]		
		SUB		r6,r6,r5
		SUB		r9,r9,r10
		STR		r6,[sp,#0x14]		
		STR		r9,[sp,#0x34]		

		ADD		r4,r4,r1
		LDRB	r5,[r4,#-3]
		LDRB	r6,[r4,#-2]
		LDRB	r7,[r4,#-1]
		LDRB	r8,[r4,#0]
		LDRB	r9,[r4,#1]
		LDRB	r10,[r4,#2]
		SUB		r7,r6,r7
		ADD		r12,r12,r7
		SUB		r8,r9,r8
		ADD		lr,lr,r8
		STR		r7,[sp,#0x08]		
		STR		r8,[sp,#0x28]		
		SUB		r6,r6,r5
		SUB		r9,r9,r10
		STR		r6,[sp,#0x18]		
		STR		r9,[sp,#0x38]		

		ADD		r4,r4,r1
		LDRB	r5,[r4,#-3]
		LDRB	r6,[r4,#-2]
		LDRB	r7,[r4,#-1]
		LDRB	r8,[r4,#0]
		LDRB	r9,[r4,#1]
		LDRB	r10,[r4,#2]
		SUB		r7,r6,r7
		ADDS	r12,r12,r7
		RSBMI	r12,r12,#0
		SUB		r8,r9,r8
		ADDS	lr,lr,r8
		RSBMI	lr,lr,#0
		STR		r7,[sp,#0x0c]		
		STR		r8,[sp,#0x2c]		
		SUB		r6,r6,r5
		SUB		r9,r9,r10
		STR		r6,[sp,#0x1c]		
		STR		r9,[sp,#0x3c]		


		LDR		r4,[sp,#0x8c]	;betaX
		MOV		r5,#3			;Al
		MOV		r6,#3			;Ar
		CMP		r12,r4,LSL #2
		MOVGE	r5,#1		
		CMP		lr,r4,LSL #2
		MOVGE	r6,#1
		STRB	r5,[sp,#92]	;Al	try to combine these two
		STRB	r6,[sp,#93]	;Ar

		ADD		r7,r5,r6
		CMP		r7,#2
		BLE		endv
		LDR		r4,[sp,#0x84]
		ADD		r7,r7,r3
		ADD		r7,r7,r4
		MOV		r7,r7,ASR #1
		STRB	r7,[sp,#94]		;c

		LDR		r4,[sp,#0x98]	;bStrong
		MOV		r12,#0			;b3SmoothLeft
		MOV		lr,#0			;b3SmoothRight		
		CMP		r4,#0
		BEQ		next1v			;This is different from c code implementation	
		CMP		r5,#3			;when bStrong is False go to next1
		MOVEQ	r12,#1
		CMP		r6,#3
		MOVEQ	lr,#1

if2v	LDR		r9,[sp,#0x90]		;beta2
		CMP		r12,#0
		BEQ		if3v
		LDR		r4,[sp,#0x10]
		LDR		r5,[sp,#0x14]
		LDR		r6,[sp,#0x18]
		ADD		r8,r4,r5
		LDR		r7,[sp,#0x1c]
		ADD		r8,r8,r6
		ADDS	r8,r8,r7		
		RSBMI	r8,r8,#0
		CMP		r8,r9
		MOVGE	r12,#0

if3v	CMP		lr,#0
		BEQ		if4v		
		LDR		r4,[sp,#0x30]
		LDR		r5,[sp,#0x34]
		LDR		r6,[sp,#0x38]
		ADD		r8,r4,r5
		LDR		r7,[sp,#0x3c]
		ADD		r8,r8,r6
		ADDS	r8,r8,r7
		RSBMI	r8,r8,#0
		CMP		r8,r9
		MOVGE	lr,#0

if4v	CMP		r12,#0
		CMPNE	lr,#0		
		BEQ		next1v
		LDR		r4,=ditherL
		LDR		r5,=ditherR
		ADD		r4,r4,r2,LSL #1
		ADD		r5,r5,r2,LSL #1
		STR		r4,[sp,#76]		;dithL
		STR		r5,[sp,#80]		;dithR

		MOV		r12,#0			;z=0
loop1v	STR		r12,[sp,#84]	;z
		LDRB	r4,[r0,#-1]		;L1
		LDRB	r5,[r0,#0]		;R1
		LDRB	r6,[r0,#-2]		;L2	It may be modified
		LDRB	r7,[r0,#1]		;R2
		SUBS	r8,r5,r4
		BEQ		loop1endv
		LDR		r9,[sp,#0x88]	;alpha
		RSBMI	r8,r8,#0
		MUL		r8,r9,r8
		MOV		r8,r8,ASR #7	;n		
		CMP		r8,#1		
		BGT		loop1endv
		STR		r8,[sp,#88]		;n
		LDRB	r8,[r0,#-3]		;L3
		LDRB	r9,[r0,#2]		;R3	
		
		LDR		r1,[sp,#76]
		LDR		r2,[sp,#80]
		MOV		r12,r12,LSL #1
		LDRH	r1,[r1,r12]		;d1
		LDRH	r2,[r2,r12]		;d2

		ADD		r11,r4,r5		;L1 + R1
		ADD		r11,r11,r6		;+L2
		ADD		r12,r7,r8		;R2+L3
		ADD		r12,r11,r12		;
		MOV		r3,#25
		MLA	r12,r3,r12,r11
		ADD		r11,r4,r5		;L1+R1
		ADD		r12,r12,r1
		MOV		r12,r12,ASR #7	;t1,rt[-1]

		ADD		r11,r11,r7		;+R2
		ADD		lr,r6,r9		;L2 + R3
		ADD		lr,lr,r11
		MLA	lr,r3,lr,r11
		LDR		r3,[sp,#88]		;n
		ADD		lr,lr,r2
		MOV		lr,lr,ASR #7	;t2,rt[0]

		LDRB	r11,[sp,#94]	;c
		CMP		r3,#0
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

rt1v	STRB	r12,[r0,#-1]	;rt[-1]	
		STRB	lr,[r0,#0]		;rt[0]

		LDRB	r10,[r0,#-4]	;L4
		LDRB	r11,[r0,#3]		;R4
		ADD		r3,r6,r8		;L2+L3
		ADD		r3,r3,r12		;+rt[-1] 
		ADD		r5,r5,r10		;R1+L4
		ADD		r5,r5,r3
		ADD		r1,r1,r3		;+d1
		MOV		r3,#25
		MLA	r5,r3,r5,r1
		ADD		r3,r7,r9		;R2+R3
		MOV		r5,r5,ASR #7	;t1,rt[-2]	

		ADD		r3,r3,lr		;+rt[0]
		ADD		r4,r4,r11		;L1+R4
		ADD		r4,r4,r3
		ADD		r2,r2,r3
		MOV		r3,#25
		MLA	r4,r3,r4,r2
		LDR		r1,[sp,#88]		;n
		MOV		r4,r4,ASR #7	;t2,rt[1]
		
		LDRB	r2,[sp,#94]		;c
		CMP		r1,#0
		BEQ		rt2v
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

rt2v	STRB	r5,[r0,#-2]		;rt[-2]
		STRB	r4,[r0,#1]		;rt[0]

		LDR		r1,[sp,#0x94]	;bChroma
		ADD		r10,r10,r5		;L4+rt[-2]
		CMP		r1,#0
		BNE		loop1endv
		ADD		r10,r10,r8
		ADD		r12,r12,r8
		ADD		r12,r12,r10
		MOV     r1,#25
		MLA	r12,r1,r12,r10
		ADD		r11,r11,r9		;R4 + R3
		ADD		r12,r12,#64
		MOV		r12,r12,ASR #7
		STRB	r12,[r0,#-3]
		
		ADD		r11,r11,r4		;rt[1]
		ADD		r9,r9,lr		;R3+rt[0]
		ADD		r9,r9,r11
		MLA		r9,r1,r9,r11
		ADD		r9,r9,#64
		MOV		r9,r9,ASR #7
		STRB	r9,[r0,#2]	

loop1endv 
		 LDR		r1,[sp,#64]
		 LDR		r12,[sp,#84]	
		 ADD		r0,r0,r1
		 ADD		r12,r12,#1
		 CMP		r12,#4
		 BLT		loop1v
		 BGE		endv

next1v	LDRB	r5,[sp,#92]		;Al
		LDRB	r6,[sp,#93]		;Ar	
		LDR		r1,[sp,#64]		;uPicth
		CMP		r5,#1			;stall
		CMPNE	r6,#1
		BEQ		elsev
				
		LDR		r4,[sp,#0x88]	;alpha
		LDR		lr,[sp,#0x8c]	;betaX
		LDRB	r12,[sp,#94]	;c
		MOV		r11,sp			;deltaL		
		LDR		r9,[sp,#0x84]	;Cl		
		LDR		r10,[sp,#72]	;Cr
		
		MOV		r8,#4			;loop variable
loop2v	LDRB	r2,[r0,#-1] 	;L1
		LDRB	r3,[r0,#0]		;R1		
		LDRB	r5,[r0,#-2]		;L2
		LDRB	r6,[r0,#1]		;R2
		SUBS	r7,r3,r2		;delta
		BEQ		loop2endv
		RSBMI	r7,r7,#0		;abs(delta)
		MUL		r7,r4,r7		
		MOV		r7,r7,ASR #7
		CMP		r7,#2
		BGT		loop2endv
		SUB		r7,r3,r2		;delta
		ADD		r7,r5,r7,LSL #2	;L2+(delta<<2)
		SUB		r7,r7,r6		;-R2
		ADD		r7,r7,#4
		MOVS	r7,r7,ASR #3	;d
		BEQ		deltaL2v		;This is added to avoid further caculations if d=0
		
		CMP		r7,r12			;d>c
		MOVGT	r7,r12			;d=c
		CMN		r7,r12			;d<-c
		RSBMI	r7,r12,#0		;d=-c			
		
		ADDS	r2,r2,r7		;L1+d
		MOVMI	r2,#0
		CMP		r2,#255
		MOVGT	r2,#255
		STRB	r2,[r0,#-1]		;rt[-1]		
		SUBS	r3,r3,r7		;R1-d
		MOVMI	r3,#0
		CMP		r3,#255
		MOVGT	r3,#255
		STRB	r3,[r0,#0]		;rt[0]

deltaL2v	
		LDR 	r2,[r11,#16]	;deltaL2[z]
		LDR 	r3,[r11,#0]		;deltaL[z]
		CMP		r2,#0
		ADD		r3,r3,r2
		RSBMI	r2,r2,#0		;abs(deltaL2[z]
		CMP		r2,lr
		BGT		deltaR2v
		SUB		r2,r3,r7
		MOVS	r2,r2,ASR #1	;d2			
		BEQ		deltaR2v		;This is added to avoid further caculations if d2=0
		CMP		r2,r9			;This may become overhead if probability of d2 taking
		MOVGT	r2,r9			;nonzero value is more
		CMN		r2,r9
		RSBMI	r2,r9,#0
		SUBS	r5,r5,r2
		MOVMI	r5,#0			;clamp table is not used,even if used same performance
		CMP		r5,#255
		MOVGT	r5,#255
		STRB	r5,[r0,#-2]		;rt[-2]

deltaR2v
		LDR 	r2,[r11,#48]	;deltaR2[z]
		LDR 	r3,[r11,#32]	;deltaR[z]		
		CMP		r2,#0
		ADD		r3,r3,r2
		RSBMI	r2,r2,#0
		CMP		r2,lr
		BGT		loop2endv
		ADD		r2,r3,r7		
		MOVS	r2,r2,ASR #1
		BEQ		loop2endv		;same reason of d2
		CMP		r2,r10
		MOVGT	r2,r10
		CMN		r2,r10
		RSBMI	r2,r10,#0
		SUBS	r6,r6,r2
		MOVMI	r6,#0			;clamp table is not used
		CMP		r6,#255
		MOVGT	r6,#255
		STRB	r6,[r0,#1]
loop2endv
		 ADD	r11,r11,#4	
		 ADD	r0,r0,r1
		 SUBS	r8,r8,#1				 
		 BGT	loop2v
		 BEQ	endv		
elsev	
		LDR		r4,[sp,#0x88]	;alpha
		LDR		lr,[sp,#0x8c]	;betaX
		LDRB	r12,[sp,#94]	;c>>1		
		MOV		r11,sp			;deltaL
		LDR		r9,[sp,#0x84]	;Cl		
		LDR		r10,[sp,#72]	;Cr
		MOV		r12,r12,ASR #1
		MOV		r9,r9,ASR #1
		MOV		r10,r10,ASR #1
		
		MOV		r8,#4			;loop variable
loop3v	LDRB	r2,[r0,#-1] 	;L1
		LDRB	r3,[r0,#0]		;R1		
		SUBS	r7,r3,r2		;delta	;stall here
		BEQ		loop3endv
		RSBMI	r7,r7,#0		;abs(delta)
		MUL		r7,r4,r7		
		MOV		r7,r7,ASR #7
		CMP		r7,#3
		BGT		loop3endv
		SUB 	r7,r3,r2		;delta
		ADD		r7,r7,#1		;
		MOVS	r7,r7,ASR #1	;d
		BEQ		deltaL22v		;same reason of d

		CMP		r7,r12			;d>c>>1
		MOVGT	r7,r12			;d=c>>1
		CMN		r7,r12			;d<c>>1
		RSBMI	r7,r12,#0		;d=-(c>>1)			
		
		ADDS	r2,r2,r7		;L1+d
		MOVMI	r2,#0
		CMP		r2,#255
		MOVGT	r2,#255	
		STRB	r2,[r0,#-1]		;rt[-1]
		SUBS	r3,r3,r7		;R1-d
		MOVMI	r3,#0
		CMP		r3,#255
		MOVGT	r3,#255	
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
		BGT		deltaR22v
		SUB		r2,r3,r7		
		MOVS	r2,r2,ASR #1	;d2
		BEQ		deltaR22v		;same reason of d2
		LDRB	r3,[r0,#-2]		;rt[-2]	
		CMP		r2,r9
		MOVGT	r2,r9	
		CMN		r2,r9
		RSBMI	r2,r9,#0		
		SUBS	r3,r3,r2
		MOVMI	r3,#0
		CMP		r3,#255
		MOVGT	r3,#255
		STRB	r3,[r0,#-2]		;rt[-2]	;stall

deltaR22v
		CMP		r6,#1			;Ar!=1
		BEQ		loop3endv
		LDR 	r2,[r11,#48]	;deltaR2[z]
		LDR 	r3,[r11,#32]	;deltaR[z]				
		CMP		r2,#0
		ADD		r3,r3,r2
		RSBMI	r2,r2,#0
		CMP		r2,lr
		BGT		loop3endv
		ADD		r2,r3,r7
		MOVS	r2,r2,ASR #1	;d2
		BEQ		loop3endv		;same reason of d2
		LDRB	r3,[r0,#1]		;rt[1]	
		CMP		r2,r10
		MOVGT	r2,r10
		CMN		r2,r10		
		RSBMI	r2,r10,#0
		SUBS	r2,r3,r2
		MOVMI	r2,#0
		CMP		r2,#255
		MOVGT	r2,#255
		STRB	r2,[r0,#1]

loop3endv
		 ADD	r11,r11,#4
		 ADD	r0,r0,r1
		 SUBS	r8,r8,#1
		 BGT	loop3v	
endv
		ADD		sp,sp,#96
		LDMFD	sp!,{r4-r11,pc}
		ENDP
		EXPORT	ARMV4_rv9_v_loop_filter
		
ARMV4_rv8_edge_filter	PROC
	    IMPORT ClampTbl

		STMFD	sp!,{r4-r11,lr}
		SUB		sp,sp,#0x08
		LDR		lr,=ClampTbl
		LDR		r4,[sp,#0x2c]	;pStrengthV(r4)
		STR		r0,[sp,#0x00]
		STR		r3,[sp,#0x04]
		ADD		r0,r0,#4		;r
		MOV		r6,r3			;j
		RSB		r3,r2,r1,LSL #2
		ADD		r3,r3,#4		
Ln1		
		ADD		r4,r4,#1		;pStrengthV += 1
		SUB		r5,r2,#4		;i(r5)
Ln2		LDRB	r8,[r4],#1		;lTemp
		MOV		r7,r0			;lpr(r7) = r
		LDRB	r9,[r7,#-0x02]	;lpr[-2]
		CMP		r8,#0
		BEQ		Ln3		
		LDRB	r10,[r7,#0x01]	;lpr[ 1]
		LDRB	r11,[r7,#-0x01]	;lpr[-1]
		LDRB	r12,[r7,#0x00]	;lpr[ 0]		
		SUB		r9,r9,r10
		SUB		r9,r9,r11,LSL #2
		ADD		r9,r9,r12,LSL #2
		MOVS	r9,r9,ASR #3	;d(r9)
		BEQ		Ln7
		CMP		r9,r8
		MOVGT	r9,r8
		CMN		r9,r8
		RSBMI	r9,r8,#0x00
		ADD		r11,r11,r9
		ADD		r10,r11,lr
		LDRB    r11,[r10,#0x120]
		SUB		r12,r12,r9
		ADD		r10,r12,lr
		LDRB    r12,[r10,#0x120]
		STRB	r11,[r7,#-1]
		STRB	r12,[r7,#0]		;stall

Ln7		ADD		r7,r7,r1		;lpr += uPitch
		LDRB	r9,[r7,#-0x02]	;lpr[-2]
		LDRB	r10,[r7,#0x01]	;lpr[ 1]
		LDRB	r11,[r7,#-0x01]	;lpr[-1]
		LDRB	r12,[r7,#0x00]	;lpr[ 0]		
		SUB		r9,r9,r10
		SUB		r9,r9,r11,LSL #2
		ADD		r9,r9,r12,LSL #2
		MOVS	r9,r9,ASR #3	;d(r9)
		BEQ		Ln8
		CMP		r9,r8
		MOVGT	r9,r8
		CMN		r9,r8
		RSBMI	r9,r8,#0x00
		ADD		r11,r11,r9
		ADD		r10,r11,lr
		LDRB    r11,[r10,#0x120]
		SUB		r12,r12,r9
		ADD		r10,r12,lr
		LDRB    r12,[r10,#0x120]
		STRB	r11,[r7,#-1]
		STRB	r12,[r7,#0]		;stall

Ln8		ADD		r7,r7,r1		;lpr += uPitch
		LDRB	r9,[r7,#-0x02]	;lpr[-2]
		LDRB	r10,[r7,#0x01]	;lpr[ 1]
		LDRB	r11,[r7,#-0x01]	;lpr[-1]
		LDRB	r12,[r7,#0x00]	;lpr[ 0]		
		SUB		r9,r9,r10
		SUB		r9,r9,r11,LSL #2
		ADD		r9,r9,r12,LSL #2
		MOVS	r9,r9,ASR #3	;d(r9)
		BEQ		Ln9
		CMP		r9,r8
		MOVGT	r9,r8
		CMN		r9,r8
		RSBMI	r9,r8,#0x00
		ADD		r11,r11,r9
		ADD		r10,r11,lr
		LDRB    r11,[r10,#0x120]
		SUB		r12,r12,r9
		ADD		r10,r12,lr
		LDRB    r12,[r10,#0x120]
		STRB	r11,[r7,#-1]
		STRB	r12,[r7,#0]		;stall

Ln9		ADD		r7,r7,r1		;lpr += uPitch
		LDRB	r9,[r7,#-0x02]	;lpr[-2]
		LDRB	r10,[r7,#0x01]	;lpr[ 1]
		LDRB	r11,[r7,#-0x01]	;lpr[-1]
		LDRB	r12,[r7,#0x00]	;lpr[ 0]		
		SUB		r9,r9,r10
		SUB		r9,r9,r11,LSL #2
		ADD		r9,r9,r12,LSL #2
		MOVS	r9,r9,ASR #3	;d(r9)
		BEQ		Ln3
		CMP		r9,r8
		MOVGT	r9,r8
		CMN		r9,r8
		RSBMI	r9,r8,#0x00
		ADD		r11,r11,r9
		ADD		r10,r11,lr
		LDRB    r11,[r10,#0x120]
		SUB		r12,r12,r9
		ADD		r10,r12,lr
		LDRB    r12,[r10,#0x120]
		STRB	r11,[r7,#-1]
		STRB	r12,[r7,#0]		;stall
				
Ln3		ADD		r0,r0,#4
		SUBS	r5,r5,#4
		BGT		Ln2
		ANDS    r11,r2,#0x0f
		ADDNE   r4,r4,#2 
		ADD		r0,r0,r3
		SUBS	r6,r6,#4
		BGT		Ln1


		;filter horizontal edges
		LDR		r0,[sp,#0x00]   ;pRec
		LDR		r3,[sp,#0x04]   ;Height
		LDR		r4,[sp,#0x30]	;pStrengthH
		LDR     r5,[sp,#0x34]   ;row
		MOV     r6,r3
		MOV     r7,r0
		CMP     r5,#0           ;if 0 == row
		ADDEQ	r4,r4,r2,ASR #2 ;pStrengthH += (uWidth >> 2);
		ADDEQ	r7,r7,r1,LSL #2	;lpr(r7)
		SUBEQ	r6,r6,#4		;j(r6)
		RSB		r3,r2,r1,LSL #2		
Ln4		
		MOV		r5,r2			;i(r5)		;r0 is free in this loop
Ln5		
		LDRB	r8,[r4],#1		
		LDRB	r9,[r7,-r1,LSL #1]
		LDRB	r10,[r7,r1]
		CMP		r8,#0
		BEQ		Ln6				
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
		ADD		r10,r11,lr
		LDRB	r11,[r10,#0x120]
		SUB		r12,r12,r9
		ADD		r10,r12,lr
		LDRB	r12,[r10,#0x120]
		STRB	r11,[r7,-r1]
		STRB	r12,[r7,#0x00]	;stall

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
		ADD		r10,r11,lr
		LDRB	r11,[r10,#0x120]
		SUB		r12,r12,r9
		ADD		r10,r12,lr
		LDRB	r12,[r10,#0x120]
		STRB	r11,[r7,-r1]
		STRB	r12,[r7,#0x00]	;stall

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
		ADD		r10,r11,lr
		LDRB	r11,[r10,#0x120]
		SUB		r12,r12,r9
		ADD		r10,r12,lr
		LDRB	r12,[r10,#0x120]
		STRB	r11,[r7,-r1]
		STRB	r12,[r7,#0x00]	;stall

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
		ADD		r10,r11,lr
		LDRB	r11,[r10,#0x120]
		SUB		r12,r12,r9
		ADD		r10,r12,lr
		LDRB	r12,[r10,#0x120]
		STRB	r11,[r7,-r1]		
		STRB	r12,[r7,#0x00]	;stall
Ln13	SUB		r7,r7,#3

Ln6		ADD		r7,r7,#4
		SUBS	r5,r5,#4
		BGT		Ln5
		ANDS    r11,r2,#0x0f
		ADDNE   r4,r4,#2 
		ADD		r7,r7,r3
		SUBS	r6,r6,#4
		BGT		Ln4
		ADD		sp,sp,#0x08
		LDMFD	sp!,{r4-r11,pc}
		ENDP
		EXPORT	ARMV4_rv8_edge_filter
		END