		@AREA DEBLOCK, CODE, READONLY
		.section .text
ARMV6_rv9_h_loop_filter:	@PROC
        .extern ditherR
        .extern ditherL
		
		STMFD	sp!,{r4-r11,lr}
		SUB		sp,sp,#96		
		ADD		r4,sp,#64
				
		@sp		DeltaL
		@sp+16	DeltaL2	
		@sp+32	DeltaR	
		@sp+48	DeltaR2	

		LDR 	r5,[r0,-r1,LSL #1]@pL2[0]
		STMIA	r4,{r1-r3}	
		ADD		lr,r1,r1,LSL #1
		LDR 	r6,[r0,-r1]		  @pL1[0]
		LDR 	r7,[r0,-lr]		  @pL3[0]

		UXTB16	r8,r5
		UXTB16	r9,r6
		UXTB16	r10,r7
		SSUB16	r9,r8,r9
		SSUB16	r10,r8,r10
		SXTH	r8,r9
		STR		r8,[sp,#0x00]
		SXTH	r9,r9,ROR #16
		STR		r9,[sp,#0x08]
		ADD		r8,r8,r9
		SXTH	r9,r10
		STR		r9,[sp,#0x10]
		SXTH	r10,r10,ROR #16
		STR		r10,[sp,#0x18]

		UXTB16	r5,r5,ROR #8
		UXTB16	r6,r6,ROR #8
		UXTB16	r7,r7,ROR #8
		SSUB16	r6,r5,r6
		SSUB16	r7,r5,r7
		SXTH	r10,r6
		STR		r10,[sp,#0x04]
		SXTH	r6,r6,ROR #16
		STR		r6,[sp,#0x0c]
		ADD		r8,r8,r10
		ADDS	r8,r8,r6
		RSBMI	r8,r8,#0	
		SXTH	r10,r7
		STR		r10,[sp,#0x14]
		SXTH	r7,r7,ROR #16
		STR		r7,[sp,#0x1c]

		ADD		r12,r0,r1	@PR2		
		LDR		r5,[r12,#0]
		LDR		r6,[r0,#0]	@PR1
		ADD		lr,r12,r1	
		LDR		r7,[lr,#0]	@PR3
		
		UXTB16 r9,r5
		UXTB16 r10,r6
		UXTB16 r11,r7
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
		ADDS	r9,r9,r6
		RSBMI	r9,r9,#0	
		SXTH	r10,r7
		STR		r10,[sp,#0x34]
		SXTH	r7,r7,ROR #16
		STR		r7,[sp,#0x3c]
		LDR		r10,[sp,#0x14]

		LDR		r4,[sp,#0x8c]	@betaX
		MOV		r5,#3			@Al
		MOV		r6,#3			@Ar
		CMP		r8,r4,LSL #2
		MOVGE	r5,#1		
		CMP		r9,r4,LSL #2
		MOVGE	r6,#1
		STRB	r5,[sp,#92]		@Al	try to combine these two
		STRB	r6,[sp,#93]		@Ar

		ADD		r7,r5,r6
		CMP		r7,#2
		BLE		endh
		LDR		r4,[sp,#0x84]
		ADD		r7,r7,r3
		ADD		r7,r7,r4
		MOV		r7,r7,ASR #1
		STRB	r7,[sp,#94]		@c

		LDR		r4,[sp,#0x98]	@bStrong
		MOV		r12,#0			@b3SmoothLeft
		MOV		lr,#0			@b3SmoothRight		
		CMP		r4,#0
		BEQ		next1h			@This is different from c code implementation	
		CMP		r5,#3			@when bStrong is False go to next1
		MOVEQ	r12,#1
		CMP		r6,#3
		MOVEQ	lr,#1

if2h:	LDR		r9,[sp,#0x90]	@beta2
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

if3h:	CMP		lr,#0
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

if4h:
	  CMP		r12,#0
		CMPNE	lr,#0		
		BEQ		next1h
		LDR		r4,=ditherL
		LDR		r5,=ditherR
		ADD		r4,r4,r2,LSL #1
		ADD		r5,r5,r2,LSL #1
		STR		r4,[sp,#76]		@dithL
		STR		r5,[sp,#80]		@dithR

		MOV		r12,#0			@z=0
loop1h:	
		LDR		r1,[sp,#64]
		STR		r12,[sp,#84]	  @z
		LDRB	r5,[r0,#0] 		  @R1
		LDRB	r4,[r0,-r1]		  @L1		
		SUB		r3,r0,r1,LSL #1
		LDR		r11,[sp,#0x88]	  @alpha		
		SUBS	r10,r5,r4
		BEQ		loop1endh
		
		RSBMI	r10,r10,#0
		MUL		r10,r11,r10
		MOV		r10,r10,ASR #7	@n		
		CMP		r10,#1		
		BGT		loop1endh

		SUB		r3,r3,r1
		LDRB	r8,[r3],r1		  @L3
		LDRB	r6,[r3],r1		  @L2
		LDRB	r7,[r0,r1]		  @R2		
		LDRB	r9,[r0,r1,LSL #1] @R3
		STR		r10,[sp,#88]	  @n			
		
		LDR		r1,[sp,#76]
		LDR		r2,[sp,#80]
		MOV		r12,r12,LSL #1
		LDRH	r1,[r1,r12]		@d1
		LDRH	r2,[r2,r12]		@d2

		ADD		r11,r4,r5		@L1 + R1
		ADD		r11,r11,r6		@+L2
		ADD		r12,r7,r8		@R2+L3
		ADD		r12,r11,r12		@
		MOV		r3,#25
		SMLABB	r12,r3,r12,r11
		@ADD		r11,r4,r5		@L1+R1
		ADD		r12,r12,r1
		MOV		r12,r12,ASR #7	@t1,pL1[z]
		
		ADD		r11,r11,r7		@L1+R1+L2+R2
		@ADD		lr,r6,r9		@L2 + R3
		@ADD		lr,lr,r11
		ADD		lr,r11,r9		@R1+L1+L2+R2+R3 huwei 20080711
		SUB		r11,r11,r6	@L1+R1+R2
		SMLABB	lr,r3,lr,r11
		LDR		r3,[sp,#88]		@n
		ADD		lr,lr,r2
		MOV		lr,lr,ASR #7	@t2,r[z]

		LDRB	r11,[sp,#94]	@c
		CMP		r3,#0
		BEQ		rt1h
		ADD		r3,r11,r4		@L1+c
		CMP		r12,r3
		MOVGT	r12,r3
		SUB		r3,r4,r11		@L1-c
		CMP		r12,r3
		MOVLT	r12,r3
		ADD		r3,r11,r5		@R1+c
		CMP		lr,r3
		MOVGT	lr,r3
		SUB		r3,r5,r11		@R1-c
		CMP		lr,r3
		MOVLT	lr,r3

rt1h:
	  LDR		r3,[sp,#64]		@r1
		STRB	lr,[r0,#0]		@r[z]
		STRB	r12,[r0,-r3]	@pL1[z]		

		LDRB	r10,[r0,-r3,LSL #2]	@L4
		ADD		r3,r3,r3,LSL #1
		LDRB	r11,[r0,r3]		@R4
		ADD		r3,r6,r8		@L2+L3
		ADD		r3,r3,r12		@+rt[-1] 
		ADD		r5,r5,r10		@R1+L4
		ADD		r5,r5,r3
		ADD		r1,r1,r3		@+d1
		MOV		r3,#25
		SMLABB	r5,r3,r5,r1
		ADD		r3,r7,r9		@R2+R3
		MOV		r5,r5,ASR #7	@t1,rt[-2]	

		ADD		r3,r3,lr		@+rt[0]
		ADD		r4,r4,r11		@L1+R4
		ADD		r4,r4,r3
		ADD		r2,r2,r3
		MOV		r3,#25
		SMLABB	r4,r3,r4,r2
		LDR		r1,[sp,#88]		@n
		MOV		r4,r4,ASR #7	@t2,rt[1]
		
		LDRB	r2,[sp,#94]		@c
		CMP		r1,#0
		BEQ		rt2h
		ADD		r3,r6,r2		@L2+c
		CMP		r5,r3
		MOVGT	r5,r3
		SUB		r3,r6,r2		@L2-c
		CMP		r5,r3
		MOVLT	r5,r3
		ADD		r3,r7,r2		@R2+c
		CMP		r4,r3
		MOVGT	r4,r3
		SUB		r3,r7,r2		@R2-c
		CMP		r4,r3
		MOVLT	r4,r3

rt2h:
  	LDR		r3,[sp,#64]			@r
		STRB	r5,[r0,-r3,LSL #1]	@PL2[z]	@stall
		STRB	r4,[r0,r3]			@PR2[z]

		LDR		r1,[sp,#0x94]	@bChroma
		ADD		r10,r10,r5		@L4+rt[-2]
		CMP		r1,#0
		BNE		loop1endh
		ADD		r10,r10,r8
		ADD		r12,r12,r8
		ADD		r12,r12,r10
		MOV     r1,#25
		SMLABB	r12,r1,r12,r10
		ADD		r11,r11,r9		@R4 + R3
		ADD		r12,r12,#64		
		MOV		r12,r12,ASR #7		
		
		ADD		r11,r11,r4		@rt[1]
		ADD		r9,r9,lr		@R3+rt[0]
		ADD		r9,r9,r11
		SMLABB	r9,r1,r9,r11
		LDR		r3,[sp,#64]
		ADD		r9,r9,#64		
		MOV		r9,r9,ASR #7
		STRB	r9,[r0,r3,LSL #1]	@PR3[z]
		ADD		r3,r3,r3,LSL #1
		STRB	r12,[r0,-r3]		@PL3[z]	

loop1endh:
		 LDR		r12,[sp,#84]	
		 ADD		r0,r0,#1
		 ADD		r12,r12,#1
		 CMP		r12,#4
		 BLT		loop1h
		 BGE		endh

next1h:	
		LDRB	r5,[sp,#92]		@Al
		LDRB	r6,[sp,#93]		@Ar	
		LDR		r1,[sp,#64]		@uPicth
		CMP		r5,#1			@stall
		CMPNE	r6,#1
		BEQ		elseh
				
		LDR		r4,[sp,#0x88]	@alpha
		LDR		lr,[sp,#0x8c]	@betaX
		LDRB	r12,[sp,#94]	@c
		MOV		r11,sp			@deltaL		
		LDR		r9,[sp,#0x84]	@Cl		
		LDR		r10,[sp,#72]	@Cr
		
		MOV		r8,#4			@loop variable
loop2h:
  	LDRB	r2,[r0,-r1] 		@L1@pL1[z]@
		LDRB	r3,[r0,#0]			@R1@r[z]		
		LDRB	r5,[r0,-r1,LSL #1]	@L2@pL2[z]@
		LDRB	r6,[r0,r1]			@R2@pR2[z]@
		SUBS	r7,r3,r2		@delta
		BEQ		loop2endh
		RSBMI	r7,r7,#0		@abs(delta)
		MUL		r7,r4,r7		
		MOV		r7,r7,ASR #7
		CMP		r7,#2
		BGT		loop2endh
		SUB		r7,r3,r2		@delta
		ADD		r7,r5,r7,LSL #2	@L2+(delta<<2)
		SUB		r7,r7,r6		@-R2
		ADD		r7,r7,#4
		MOVS	r7,r7,ASR #3	@d
		BEQ		deltaL2h		@This is added to avoid further caculations if d=0
		
		CMP		r7,r12			@d>c
		MOVGT	r7,r12			@d=c
		CMN		r7,r12			@d<-c
		RSBMI	r7,r12,#0		@d=-c			
		
		ADD 	r2,r2,r7		@L1+d
		USAT	r2,#8,r2		
		SUB 	r3,r3,r7		@R1-d
		USAT	r3,#8,r3
		STRB	r2,[r0,-r1]		@pL1[z]
		STRB	r3,[r0,#0]		@r[z]

deltaL2h:
		LDR 	r2,[r11,#16]	@deltaL2[z]
		LDR 	r3,[r11,#0]		@deltaL[z]
		CMP		r2,#0
		ADD		r3,r3,r2
		RSBMI	r2,r2,#0		@abs(deltaL2[z])
		CMP		r2,lr
		BGT		deltaR2h
		SUB		r2,r3,r7
		MOVS	r2,r2,ASR #1	@d2			
		BEQ		deltaR2h		@This is added to avoid further caculations if d2=0
		CMP		r2,r9			@This may become overhead if probability of d2 taking
		MOVGT	r2,r9			@nonzero value is more
		CMN		r2,r9
		RSBMI	r2,r9,#0
		SUB 	r5,r5,r2
		USAT	r5,#8,r5
		STRB	r5,[r0,-r1,LSL #1]	@pL2[z]

deltaR2h:
		LDR 	r2,[r11,#48]	@deltaR2[z]
		LDR 	r3,[r11,#32]	@deltaR[z]		
		CMP		r2,#0
		ADD		r3,r3,r2
		RSBMI	r2,r2,#0
		CMP		r2,lr
		BGT		loop2endh
		ADD		r2,r3,r7		
		MOVS	r2,r2,ASR #1
		BEQ		loop2endh		@same reason of d2
		CMP		r2,r10
		MOVGT	r2,r10
		CMN		r2,r10
		RSBMI	r2,r10,#0
		SUB 	r6,r6,r2
		USAT	r6,#8,r6
		STRB	r6,[r0,r1]		@pR2[z]
loop2endh:
		 ADD	r11,r11,#4	
		 ADD	r0,r0,#1
		 SUBS	r8,r8,#1				 
		 BGT	loop2h
		 BEQ	endh		
elseh:
		LDR		r4,[sp,#0x88]	@alpha
		LDR		lr,[sp,#0x8c]	@betaX
		LDRB	r12,[sp,#94]	@c>>1		
		MOV		r11,sp			@deltaL
		LDR		r9,[sp,#0x84]	@Cl		
		LDR		r10,[sp,#72]	@Cr
		MOV		r12,r12,ASR #1
		MOV		r9,r9,ASR #1
		MOV		r10,r10,ASR #1
		
		MOV		r8,#4			@loop variable
loop3h:	
		LDRB	r2,[r0,-r1] 	@pL1[z]
		LDRB	r3,[r0,#0]		@R1		
		SUBS	r7,r3,r2		@delta	@stall here
		BEQ		loop3endh
		RSBMI	r7,r7,#0		@abs(delta)
		MUL		r7,r4,r7		
		MOV		r7,r7,ASR #7
		CMP		r7,#3
		BGT		loop3endh
		SUB 	r7,r3,r2		@delta
		ADD		r7,r7,#1		@
		MOVS	r7,r7,ASR #1	@d
		BEQ		deltaL22h		@same reason of d

		CMP		r7,r12			@d>c>>1
		MOVGT	r7,r12			@d=c>>1
		CMN		r7,r12			@d<c>>1
		RSBMI	r7,r12,#0		@d=-(c>>1)			
		
		ADD 	r2,r2,r7		@L1+d
		USAT	r2,#8,r2		
		SUB 	r3,r3,r7		@R1-d
		USAT	r3,#8,r3
		STRB	r2,[r0,-r1]		@pL1[z]
		STRB	r3,[r0,#0]		@r[z]

deltaL22h:
		CMP		r5,#1			@Al!=1
		BEQ		deltaR22h
		LDR 	r2,[r11,#16]	@deltaL2[z]
		LDR 	r3,[r11,#0]		@deltaL[z]							
		CMP		r2,#0
		ADD		r3,r2,r3
		RSBMI	r2,r2,#0		@abs(deltaL2[z])
		CMP		r2,lr
		BGT		deltaR22h
		SUB		r2,r3,r7		
		MOVS	r2,r2,ASR #1	@d2
		BEQ		deltaR22h		@same reason of d2
		LDRB	r3,[r0,-r1,LSL #1]	@pL2[z]	
		CMP		r2,r9
		MOVGT	r2,r9	
		CMN		r2,r9
		RSBMI	r2,r9,#0		
		SUB 	r3,r3,r2
		USAT	r3,#8,r3
		STRB	r3,[r0,-r1,LSL #1]	@pL2[z]

deltaR22h:
		CMP		r6,#1			@Ar!=1
		BEQ		loop3endh
		LDR 	r2,[r11,#48]	@deltaR2[z]
		LDR 	r3,[r11,#32]	@deltaR[z]				
		CMP		r2,#0
		ADD		r3,r3,r2
		RSBMI	r2,r2,#0
		CMP		r2,lr
		BGT		loop3endh
		ADD		r2,r3,r7
		MOVS	r2,r2,ASR #1	@d2
		BEQ		loop3endh		@same reason of d2
		LDRB	r3,[r0,r1]		@pR2[z]	
		CMP		r2,r10
		MOVGT	r2,r10
		CMN		r2,r10		
		RSBMI	r2,r10,#0
		SUB 	r2,r3,r2
		USAT	r2,#8,r2
		STRB	r2,[r0,r1]		@pR2[z]	

loop3endh:
		 ADD	r11,r11,#4
		 ADD	r0,r0,#1
		 SUBS	r8,r8,#1
		 BGT	loop3h	
endh:
		ADD		sp,sp,#96
		LDMFD	sp!,{r4-r11,pc}
	@ENDP
	.global	ARMV6_rv9_h_loop_filter
	
ARMV6_rv9_v_loop_filter:	@PROC
        .extern ditherR
        .extern ditherL

		STMFD	sp!,{r4-r11,lr}
		SUB		sp,sp,#96
		
		STR		r1,[sp,#64]		@r1,uPitch
		STR		r2,[sp,#68]		@r2,uMBPos
		STR		r3,[sp,#72]		@r3,Cr
				
		@sp		DeltaL
		@sp+16	DeltaL2	
		@sp+32	DeltaR	
		@sp+48	DeltaR2	

		SUB		r4,r0,#0x03		@rt = pPels		
		LDRB	r6,[r4,#1]			@rt[-2]
		LDRB	r7,[r4,#2]			@rt[-1]
		LDRB	r8,[r4,#3]			@rt[0]
		LDRB	r9,[r4,#4]			@rt[1]
		SUB		r12,r6,r7			@r12 sum deltaL rt[-2] - rt[-1]
		LDRB	r5,[r4,#0]			@rt[-3]
		SUB		lr,r9,r8				@lr sum deltaR   r[1] - r[0]
		LDRB	r10,[r4,#5]			@rt[2]		
		SUB		r6,r6,r5				@rt[-2] - rt[-3]
		STR		r12,[sp,#0x00]
		SUB		r9,r9,r10			@rt[1] - rt[2]
		STR		lr,[sp,#0x20]		
		STR		r6,[sp,#0x10]
		ADD		r4,r4,r1
		STR		r9,[sp,#0x30]				
		
		LDRB	r6,[r4,#1]
		LDRB	r7,[r4,#2]
		LDRB	r8,[r4,#3]
		LDRB	r9,[r4,#4]
		SUB		r7,r6,r7		
		LDRB	r5,[r4,#0]
		ADD		r12,r12,r7
		LDRB	r10,[r4,#5]		
		SUB		r8,r9,r8		
		STR		r7,[sp,#0x04]		
		ADD		lr,lr,r8
		STR		r8,[sp,#0x24]		
		SUB		r6,r6,r5		
		STR		r6,[sp,#0x14]
		SUB		r9,r9,r10
		ADD		r4,r4,r1
		STR		r9,[sp,#0x34]		
		
		LDRB	r6,[r4,#1]
		LDRB	r7,[r4,#2]
		LDRB	r8,[r4,#3]
		LDRB	r9,[r4,#4]
		SUB		r7,r6,r7
		LDRB	r5,[r4,#0]
		ADD		r12,r12,r7
		LDRB	r10,[r4,#5]		
		SUB		r8,r9,r8		
		STR		r7,[sp,#0x08]		
		ADD		lr,lr,r8
		STR		r8,[sp,#0x28]
		SUB		r6,r6,r5
		STR		r6,[sp,#0x18]
		SUB		r9,r9,r10
		ADD		r4,r4,r1
		STR		r9,[sp,#0x38]
		
		LDRB	r6,[r4,#1]
		LDRB	r7,[r4,#2]
		LDRB	r8,[r4,#3]
		LDRB	r9,[r4,#4]
		SUB		r7,r6,r7
		LDRB	r5,[r4,#0]
		ADDS	r12,r12,r7
		LDRB	r10,[r4,#5]
		RSBMI	r12,r12,#0
		STR		r7,[sp,#0x0c]
		SUB		r8,r9,r8
		ADDS	lr,lr,r8
		STR		r8,[sp,#0x2c]
		RSBMI	lr,lr,#0
		SUB		r6,r6,r5
		STR		r6,[sp,#0x1c]
		SUB		r9,r9,r10
		STR		r9,[sp,#0x3c]

		LDR		r4,[sp,#0x8c]	@betaX
		MOV		r5,#3			@Al
		MOV		r6,#3			@Ar
		CMP		r12,r4,LSL #2
		MOVGE	r5,#1		
		STRB	r5,[sp,#92]	@Al
		CMP		lr,r4,LSL #2
		MOVGE	r6,#1		
		STRB	r6,[sp,#93]	@Ar

		ADD		r7,r5,r6
		CMP		r7,#2
		BLE		endv
		LDR		r4,[sp,#0x84]
		ADD		r7,r7,r3
		ADD		r7,r7,r4
		LDR		r4,[sp,#0x98]	@bStrong
		MOV		r7,r7,ASR #1
		STRB	r7,[sp,#94]		@c
		
		MOV		r12,#0			@b3SmoothLeft
		MOV		lr,#0			@b3SmoothRight		
		CMP		r4,#0
		BEQ		next1v			@This is different from c code implementation	
		CMP		r5,#3			@when bStrong is False go to next1
		MOVEQ	r12,#1
		CMP		r6,#3
		MOVEQ	lr,#1

if2v:
	  LDR		r9,[sp,#0x90]		@beta2
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

if3v:
  	CMP		lr,#0
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

if4v:
  	CMP		r12,#0
		CMPNE	lr,#0		
		BEQ		next1v
		LDR		r4,=ditherL
		LDR		r5,=ditherR
		ADD		r4,r4,r2,LSL #1
		ADD		r5,r5,r2,LSL #1
		STR		r4,[sp,#76]		@dithL
		STR		r5,[sp,#80]		@dithR

		MOV		r12,#0			@z=0
loop1v:
  	STR		r12,[sp,#84]	@z
		LDRB	r4,[r0,#-1]		@L1
		LDRB	r5,[r0,#0]		@R1
		LDRB	r6,[r0,#-2]		@L2	It may be modified
		LDRB	r7,[r0,#1]		@R2
		SUBS	r8,r5,r4
		BEQ		loop1endv
		LDR		r9,[sp,#0x88]	@alpha
		RSBMI	r8,r8,#0
		MUL		r8,r9,r8
		MOV		r8,r8,ASR #7	@n		
		CMP		r8,#1		
		BGT		loop1endv
		STR		r8,[sp,#88]		@n
		LDRB	r8,[r0,#-3]		@L3
		LDRB	r9,[r0,#2]		@R3	
		
		LDR		r1,[sp,#76]		@dithL
		LDR		r2,[sp,#80]		@dithR
		MOV		r12,r12,LSL #1
		LDRH	r1,[r1,r12]		@d1
		LDRH	r2,[r2,r12]		@d2

		ADD		r11,r4,r5		@L1 + R1
		ADD		r11,r11,r6		@+L2
		ADD		r12,r7,r8		@R2+L3
		ADD		r12,r11,r12		@
		MOV		r3,#25
		SMLABB	r12,r3,r12,r11
		@ADD		r11,r4,r5		@L1+R1
		ADD		r12,r12,r1
		MOV		r12,r12,ASR #7	@t1,rt[-1]

		ADD		r11,r11,r7		@+R2
		@ADD		lr,r6,r9		@L2 + R3
		@ADD		lr,lr,r11
		ADD		lr,r11,r9		@huwei 20080714
		SUB		r11,r11,r6
		SMLABB	lr,r3,lr,r11
		LDR		r3,[sp,#88]		@n
		ADD		lr,lr,r2
		MOV		lr,lr,ASR #7	@t2,rt[0]

		LDRB	r11,[sp,#94]	@c
		CMP		r3,#0
		BEQ		rt1v
		ADD		r3,r11,r4		@L1+c
		CMP		r12,r3
		MOVGT	r12,r3
		SUB		r3,r4,r11		@L1-c
		CMP		r12,r3
		MOVLT	r12,r3
		ADD		r3,r11,r5		@R1+c
		CMP		lr,r3
		MOVGT	lr,r3
		SUB		r3,r5,r11		@R1-c
		CMP		lr,r3
		MOVLT	lr,r3

rt1v:
  	STRB	r12,[r0,#-1]	@rt[-1]	
		STRB	lr,[r0,#0]		@rt[0]

		LDRB	r10,[r0,#-4]	@L4
		LDRB	r11,[r0,#3]		@R4
		ADD		r3,r6,r8		@L2+L3
		ADD		r3,r3,r12		@+rt[-1] 
		ADD		r5,r5,r10		@R1+L4
		ADD		r5,r5,r3
		ADD		r1,r1,r3		@+d1
		MOV		r3,#25
		SMLABB	r5,r3,r5,r1
		ADD		r3,r7,r9		@R2+R3
		MOV		r5,r5,ASR #7	@t1,rt[-2]	

		ADD		r3,r3,lr		@+rt[0]
		ADD		r4,r4,r11		@L1+R4
		ADD		r4,r4,r3
		ADD		r2,r2,r3
		MOV		r3,#25
		SMLABB	r4,r3,r4,r2
		LDR		r1,[sp,#88]		@n
		MOV		r4,r4,ASR #7	@t2,rt[1]
		
		LDRB	r2,[sp,#94]		@c
		CMP		r1,#0
		BEQ		rt2v
		ADD		r3,r6,r2		@L2+c
		CMP		r5,r3
		MOVGT	r5,r3
		SUB		r3,r6,r2		@L2-c
		CMP		r5,r3
		MOVLT	r5,r3
		ADD		r3,r7,r2		@R2+c
		CMP		r4,r3
		MOVGT	r4,r3
		SUB		r3,r7,r2		@R2-c
		CMP		r4,r3
		MOVLT	r4,r3

rt2v:
  	STRB	r5,[r0,#-2]		@rt[-2]
		STRB	r4,[r0,#1]		@rt[0]

		LDR		r1,[sp,#0x94]	@bChroma
		ADD		r10,r10,r5		@L4+rt[-2]
		CMP		r1,#0
		BNE		loop1endv
		ADD		r10,r10,r8
		ADD		r12,r12,r8
		ADD		r12,r12,r10
		MOV     r1,#25
		SMLABB	r12,r1,r12,r10
		ADD		r11,r11,r9		@R4 + R3
		ADD		r12,r12,#64
		MOV		r12,r12,ASR #7
		STRB	r12,[r0,#-3]
		
		ADD		r11,r11,r4		@rt[1]
		ADD		r9,r9,lr		@R3+rt[0]
		ADD		r9,r9,r11
		MLA		r9,r1,r9,r11
		ADD		r9,r9,#64
		MOV		r9,r9,ASR #7
		STRB	r9,[r0,#2]	

loop1endv: 
		 LDR		r1,[sp,#64]
		 LDR		r12,[sp,#84]	
		 ADD		r0,r0,r1
		 ADD		r12,r12,#1
		 CMP		r12,#4
		 BLT		loop1v
		 BGE		endv

next1v:
  	LDRB	r5,[sp,#92]		@Al
		LDRB	r6,[sp,#93]		@Ar	
		LDR		r1,[sp,#64]		@uPicth
		CMP		r5,#1			@stall
		CMPNE	r6,#1
		BEQ		elsev
				
		LDR		r4,[sp,#0x88]	@alpha
		LDR		lr,[sp,#0x8c]	@betaX
		LDRB	r12,[sp,#94]	@c
		MOV		r11,sp			@deltaL		
		LDR		r9,[sp,#0x84]	@Cl		
		LDR		r10,[sp,#72]	@Cr
		
		MOV		r8,#4			@loop variable
loop2v:
  	LDRB	r2,[r0,#-1] 	@L1
		LDRB	r3,[r0,#0]		@R1		
		LDRB	r5,[r0,#-2]		@L2
		LDRB	r6,[r0,#1]		@R2
		SUBS	r7,r3,r2		@delta
		BEQ		loop2endv
		RSBMI	r7,r7,#0		@abs(delta)
		MUL		r7,r4,r7		
		MOV		r7,r7,ASR #7
		CMP		r7,#2
		BGT		loop2endv
		SUB		r7,r3,r2		@delta
		ADD		r7,r5,r7,LSL #2	@L2+(delta<<2)
		SUB		r7,r7,r6		@-R2
		ADD		r7,r7,#4
		MOVS	r7,r7,ASR #3	@d
		BEQ		deltaL2v		@This is added to avoid further caculations if d=0
		
		CMP		r7,r12			@d>c
		MOVGT	r7,r12			@d=c
		CMN		r7,r12			@d<-c
		RSBMI	r7,r12,#0		@d=-c			
		
		ADD		r2,r2,r7		@L1+d
		USAT	r2,#8,r2		
		SUB 	r3,r3,r7		@R1-d
		USAT	r3,#8,r3
		STRB	r2,[r0,#-1]		@rt[-1]
		STRB	r3,[r0,#0]		@rt[0]

deltaL2v:	
		LDR 	r2,[r11,#16]	@deltaL2[z]
		LDR 	r3,[r11,#0]		@deltaL[z]
		CMP		r2,#0
		ADD		r3,r3,r2
		RSBMI	r2,r2,#0		@abs(deltaL2[z]
		CMP		r2,lr
		BGT		deltaR2v
		SUB		r2,r3,r7
		MOVS	r2,r2,ASR #1	@d2			
		BEQ		deltaR2v		@This is added to avoid further caculations if d2=0
		CMP		r2,r9			@This may become overhead if probability of d2 taking
		MOVGT	r2,r9			@nonzero value is more
		CMN		r2,r9
		RSBMI	r2,r9,#0
		SUB 	r5,r5,r2
		USAT	r5,#8,r5	
		STRB	r5,[r0,#-2]		@rt[-2]

deltaR2v:
		LDR 	r2,[r11,#48]	@deltaR2[z]
		LDR 	r3,[r11,#32]	@deltaR[z]		
		CMP		r2,#0
		ADD		r3,r3,r2
		RSBMI	r2,r2,#0
		CMP		r2,lr
		BGT		loop2endv
		ADD		r2,r3,r7		
		MOVS	r2,r2,ASR #1
		BEQ		loop2endv		@same reason of d2
		CMP		r2,r10
		MOVGT	r2,r10
		CMN		r2,r10
		RSBMI	r2,r10,#0
		SUB 	r6,r6,r2
		USAT	r6,#8,r6
		STRB	r6,[r0,#1]
loop2endv:
		 ADD	r11,r11,#4	
		 ADD	r0,r0,r1
		 SUBS	r8,r8,#1				 
		 BGT	loop2v
		 BEQ	endv		
elsev:	
		LDR		r4,[sp,#0x88]	@alpha
		LDR		lr,[sp,#0x8c]	@betaX
		LDRB	r12,[sp,#94]	@c>>1		
		MOV		r11,sp			@deltaL
		LDR		r9,[sp,#0x84]	@Cl		
		LDR		r10,[sp,#72]	@Cr
		MOV		r12,r12,ASR #1
		MOV		r9,r9,ASR #1
		MOV		r10,r10,ASR #1
		
		MOV		r8,#4			@loop variable
loop3v:
  	LDRB	r2,[r0,#-1] 	@L1
		LDRB	r3,[r0,#0]		@R1		
		SUBS	r7,r3,r2		@delta	@stall here
		BEQ		loop3endv
		RSBMI	r7,r7,#0		@abs(delta)
		MUL		r7,r4,r7		
		MOV		r7,r7,ASR #7
		CMP		r7,#3
		BGT		loop3endv
		SUB 	r7,r3,r2		@delta
		ADD		r7,r7,#1		@
		MOVS	r7,r7,ASR #1	@d
		BEQ		deltaL22v		@same reason of d

		CMP		r7,r12			@d>c>>1
		MOVGT	r7,r12			@d=c>>1
		CMN		r7,r12			@d<c>>1
		RSBMI	r7,r12,#0		@d=-(c>>1)			
		
		ADD 	r2,r2,r7		@L1+d
		USAT	r2,#8,r2		
		SUB 	r3,r3,r7		@R1-d
		USAT	r3,#8,r3
		STRB	r2,[r0,#-1]		@rt[-1]
		STRB	r3,[r0,#0]		@rt[0]

deltaL22v:
		CMP		r5,#1			@Al!=1
		BEQ		deltaR22v
		LDR 	r2,[r11,#16]	@deltaL2[z]
		LDR 	r3,[r11,#0]		@deltaL[z]							
		CMP		r2,#0
		ADD		r3,r2,r3
		RSBMI	r2,r2,#0		@abs(deltaL2[z]
		CMP		r2,lr
		BGT		deltaR22v
		SUB		r2,r3,r7		
		MOVS	r2,r2,ASR #1	@d2
		BEQ		deltaR22v		@same reason of d2
		LDRB	r3,[r0,#-2]		@rt[-2]	
		CMP		r2,r9
		MOVGT	r2,r9	
		CMN		r2,r9
		RSBMI	r2,r9,#0		
		SUB 	r3,r3,r2
		USAT	r3,#8,r3
		STRB	r3,[r0,#-2]		@rt[-2]	@stall

deltaR22v:
		CMP		r6,#1			@Ar!=1
		BEQ		loop3endv
		LDR 	r2,[r11,#48]	@deltaR2[z]
		LDR 	r3,[r11,#32]	@deltaR[z]				
		CMP		r2,#0
		ADD		r3,r3,r2
		RSBMI	r2,r2,#0
		CMP		r2,lr
		BGT		loop3endv
		ADD		r2,r3,r7
		MOVS	r2,r2,ASR #1	@d2
		BEQ		loop3endv		@same reason of d2
		LDRB	r3,[r0,#1]		@rt[1]	
		CMP		r2,r10
		MOVGT	r2,r10
		CMN		r2,r10		
		RSBMI	r2,r10,#0
		SUB		r2,r3,r2
		USAT	r2,#8,r2
		STRB	r2,[r0,#1]

loop3endv:
		 ADD	r11,r11,#4
		 ADD	r0,r0,r1
		 SUBS	r8,r8,#1
		 BGT	loop3v	
endv:
		ADD		sp,sp,#96
		LDMFD	sp!,{r4-r11,pc}
		@ENDP
		.global	ARMV6_rv9_v_loop_filter
		
ARMV6_rv8_edge_filter:	@PROC
	 	STMFD	sp!,{r4-r11,lr}
		SUB		sp,sp,#4
		STR		r0,[sp,#0x00]	@pRec
		LDR		r4,[sp,#0x28]	@pStrengthV(r4)		 
		RSB		r6,r2,r1,LSL #2
		ADD		r6,r6,#4		@update factor
		ADD		r0,r0,#0x04		@r(r0)
		MOV		lr,r3			@j
Ln1:		
		ADD		r4,r4,#1		@pStrengthV += 1
		SUB		r5,r2,#4		@i(r5)
Ln2:		
		LDRB	r8,[r4],#1		@lTemp
		MOV		r7,r0			@lpr(r7) = r
		CMP		r8,#0
		BEQ		Ln3
		LDRB	r9,[r7,#-0x02]	@lpr[-2]
		LDRB	r10,[r7,#0x01]	@lpr[ 1]
		LDRB	r11,[r7,#-0x01]	@lpr[-1]
		LDRB	r12,[r7,#0x00]	@lpr[ 0]
		SUB		r9,r9,r10
		SUB		r10,r11,r12		@stall
		SUB		r9,r9,r10,LSL #2
		MOVS	r9,r9,ASR #3	@d(r9)
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

Ln7:
		ADD		r7,r7,r1		@lpr += uPitch
		LDRB	r9,[r7,#-0x02]	@lpr[-2]
		LDRB	r10,[r7,#0x01]	@lpr[ 1]
		LDRB	r11,[r7,#-0x01]	@lpr[-1]
		LDRB	r12,[r7,#0x00]	@lpr[ 0]		
		SUB		r9,r9,r10
		SUB		r10,r11,r12		@stall
		SUB		r9,r9,r10,LSL #2
		MOVS	r9,r9,ASR #3	@d(r9)
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

Ln8:
		ADD		r7,r7,r1		@lpr += uPitch
		LDRB	r9,[r7,#-0x02]	@lpr[-2]
		LDRB	r10,[r7,#0x01]	@lpr[ 1]
		LDRB	r11,[r7,#-0x01]	@lpr[-1]
		LDRB	r12,[r7,#0x00]	@lpr[ 0]		
		SUB		r9,r9,r10
		SUB		r10,r11,r12		@stall
		SUB		r9,r9,r10,LSL #2
		MOVS	r9,r9,ASR #3	@d(r9)
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

Ln9:
		ADD		r7,r7,r1		@lpr += uPitch
		LDRB	r9,[r7,#-0x02]	@lpr[-2]
		LDRB	r10,[r7,#0x01]	@lpr[ 1]
		LDRB	r11,[r7,#-0x01]	@lpr[-1]
		LDRB	r12,[r7,#0x00]	@lpr[ 0]		
		SUB		r9,r9,r10
		SUB		r10,r11,r12		@stall
		SUB		r9,r9,r10,LSL #2
		MOVS	r9,r9,ASR #3	@d(r9)
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
				
Ln3:
		ADD		r0,r0,#4
		SUBS	r5,r5,#4
		BGT		Ln2
		ANDS    r11,r2,#0x0f
		ADDNE   r4,r4,#2 
		ADD		r0,r0,r6
		SUBS	lr,lr,#4
		BGT		Ln1

		
		@filter horizontal edges
		LDR		r0,[sp,#0x00]	@pRec
		LDR		r4,[sp,#0x2c]	@pStrengthH
		LDR     r5,[sp,#0x30]   @row
		MOV     r6,r3           @Height
		MOV     r7,r0
		CMP     r5,#0           @if 0 == row
		ADDEQ	r7,r7,r1,LSL #2	@lpr(r7)
		ADDEQ	r4,r4,r2,ASR #2 @pStrengthH += (uWidth >> 2)@ 
		SUBEQ	r6,r6,#4		@j(r6)	
		RSB		r3,r2,r1,LSL #2	@update factor	
Ln4:
		MOV		r5,r2			@i(r5)
Ln5:		
		LDRB	r8,[r4],#1
		CMP		r8,#0			@stall
		BEQ		Ln6
		LDRB	r9,[r7,-r1,LSL #1]
		LDRB	r10,[r7,r1]
		LDRB	r11,[r7,-r1]
		LDRB	r12,[r7,#0x00]
		SUB		r9,r9,r10
		SUB		r9,r9,r11,LSL #2
		ADD		r9,r9,r12,LSL #2
		MOVS	r9,r9,ASR #3	@d(r9)
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

Ln10:
  	ADD		r7,r7,#1
		LDRB	r9,[r7,-r1,LSL #1]
		LDRB	r10,[r7,r1]
		LDRB	r11,[r7,-r1]
		LDRB	r12,[r7,#0x00]
		SUB		r9,r9,r10
		SUB		r9,r9,r11,LSL #2
		ADD		r9,r9,r12,LSL #2
		MOVS	r9,r9,ASR #3	@d(r9)
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

Ln11:
  	ADD		r7,r7,#1
		LDRB	r9,[r7,-r1,LSL #1]
		LDRB	r10,[r7,r1]
		LDRB	r11,[r7,-r1]
		LDRB	r12,[r7,#0x00]
		SUB		r9,r9,r10
		SUB		r9,r9,r11,LSL #2
		ADD		r9,r9,r12,LSL #2
		MOVS	r9,r9,ASR #3	@d(r9)
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

Ln12:
  	ADD		r7,r7,#1
		LDRB	r9,[r7,-r1,LSL #1]
		LDRB	r10,[r7,r1]
		LDRB	r11,[r7,-r1]
		LDRB	r12,[r7,#0x00]
		SUB		r9,r9,r10
		SUB		r9,r9,r11,LSL #2
		ADD		r9,r9,r12,LSL #2
		MOVS	r9,r9,ASR #3	@d(r9)
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
Ln13:
  	SUB		r7,r7,#3

Ln6:
		ADD		r7,r7,#4
		SUBS	r5,r5,#4
		BGT		Ln5
		ADD		r7,r7,r3
		SUBS	r6,r6,#4
		BGT		Ln4
		ANDS    r11,r2,#0x0f
		ADDNE   r4,r4,#2 
		ADD		sp,sp,#4
		LDMFD	sp!,{r4-r11,pc}
		@ENDP
		.global	ARMV6_rv8_edge_filter
		@END