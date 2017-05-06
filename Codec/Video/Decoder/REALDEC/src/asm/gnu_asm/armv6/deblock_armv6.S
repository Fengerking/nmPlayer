@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2009					*
@*																			*
@*****************************************************************************

#include "../../voASMPort.h"

	@AREA	|.text|, CODE
	.section  .text
	
	.global	ARMV6_rv9_h_loop_filter
	.global	ARMV6_rv9_v_loop_filter

	
  .equ H_Offset_SavingReg,  36	        
  .equ H_Offset_TmpBuff, 104
  .equ H_Stack_Size, H_Offset_TmpBuff + H_Offset_SavingReg
		        
ARMV6_rv9_h_loop_filter:	@PROC
		
		@.reference  ditherR
		@.reference  ditherL
		
		stmfd	sp!,{r4-r11,lr}
		pld		[r0,-r1,lsl #1]
		sub		sp,sp,#H_Offset_TmpBuff		
		add		r4,sp,#64
				
		@sp		DeltaL
		@sp+16	DeltaL2	
		@sp+32	DeltaR	
		@sp+48	DeltaR2	
		@sp+96	abs(deltaL2)
		@sp+100	abs(deltaR2)

		ldr 	r5,[r0,-r1,lsl #1]@pL2[0]
		stmia	r4,{r1-r3}	
		add		lr,r1,r1,lsl #1
		ldr 	r6,[r0,-r1]		  @pL1[0]
		ldr 	r7,[r0,-lr]		  @pL3[0]

		uxtb16	r8,r5
		uxtb16	r9,r6
		uxtb16	r10,r7
		ssub16	r9,r8,r9
		ssub16	r10,r8,r10
		sxth	r8,r9
		str		r8,[sp,#0x00]		@deltaL[0]
		sxth	r9,r9,ror #16
		str		r9,[sp,#0x08]		@deltaL[2]
		add		r8,r8,r9			@delta = deltaL[0]+deltaL[1]+deltaL[2]+deltaL[3]@
		sxth	r9,r10
		str		r9,[sp,#0x10]		@deltaL2[0]
		sxth	r10,r10,ror #16
		str		r10,[sp,#0x18]		@deltaL2[2]
		add		r9,r9,r10

		uxtb16	r5,r5,ror #8
		uxtb16	r6,r6,ror #8
		uxtb16	r7,r7,ror #8
		ssub16	r6,r5,r6
		ssub16	r7,r5,r7
		sxth	r10,r6
		str		r10,[sp,#0x04]		@deltaL[1]
		sxth	r6,r6,ror #16
		str		r6,[sp,#0x0c]		@deltaL[3]
		add		r8,r8,r10
		adds	r8,r8,r6			@delta = deltaL[0]+deltaL[1]+deltaL[2]+deltaL[3]@
		rsbmi	r8,r8,#0			@abs(delta)
		sxth	r10,r7
		str		r10,[sp,#0x14]		@deltaL2[1]
		sxth	r7,r7,ror #16
		str		r7,[sp,#0x1c]		@deltaL2[3]

		ldr		r5,[r0,r1]
		add		r9,r9,r10
		ldr		r6,[r0]				@PR1
		adds	r9,r9,r7			@deltaL2 = deltaL2[0]+deltaL2[1]+deltaL2[2]+deltaL2[3]@
		ldr		r7,[r0,r1,lsl #1]	@PR3
		rsbmi	r9,r9,#0			@ABS(deltaL2)
		str		r9,[sp,#96]			
		pld		[r0,-r1,lsl #1]
		pld		[r0,-r1]
		pld		[r0]
		pld		[r0,r1]
		
		uxtb16	r9,r5
		uxtb16	r10,r6 
		uxtb16	r11,r7
		ssub16	r10,r9,r10
		ssub16	r11,r9,r11
		sxth	r9,r10
		str		r9,[sp,#0x20]
		sxth	r10,r10,ror #16
		str		r10,[sp,#0x28]
		add		r9,r9,r10
		sxth	r10,r11
		str		r10,[sp,#0x30]
		sxth	r11,r11, ror #16
		str		r11,[sp,#0x38]
		add		r11,r11,r10
		
		uxtb16	r5,r5,ror #8
		uxtb16	r6,r6,ror #8
		uxtb16	r7,r7,ror #8
		ssub16	r6,r5,r6
		ssub16	r7,r5,r7
		sxth	r10,r6
		str		r10,[sp,#0x24]
		sxth	r6,r6,ror #16
		str		r6,[sp,#0x2c]
		add		r9,r9,r10
		adds	r9,r9,r6			@delta = deltaR[0]+deltaR[1]+deltaR[2]+deltaR[3]@
		rsbmi	r9,r9,#0			@abs(delta)	
		sxth	r10,r7
		str		r10,[sp,#0x34]
		sxth	r7,r7,ror #16
		str		r7,[sp,#0x3c]
		add		r11,r11,r10
		adds	r11,r11,r7			@deltaR2 = deltaR2[0]+deltaR2[1]+deltaR2[2]+deltaR2[3]@
		
		ldr		r4,[sp,#H_Stack_Size+8]	@betaX
		rsbmi	r11,r11,#0			@ABS(deltaR2)
		str		r11,[sp,#100]
		mov		r5,#3			@Al
		mov		r6,#3			@Ar
		cmp		r8,r4,lsl #2
		movge	r5,#1		
		cmp		r9,r4,lsl #2
		movge	r6,#1
		strb	r5,[sp,#92]		@Al	try to combine these two
		strb	r6,[sp,#93]		@Ar

		add		r7,r5,r6
		cmp		r7,#2
	@	BLE		endh
		ldrgt	r4,[sp,#H_Stack_Size]	@Cl
		addle	sp,sp,#H_Offset_TmpBuff
		ldmlefd	sp!,{r4-r11,pc}
		
		
		add		r7,r7,r3		@Cr
		mov		r12,#0			@b3SmoothLeft
		add		r7,r7,r4
		ldr		r4,[sp,#H_Stack_Size+20]	@bStrong
		mov		r7,r7,asr #1
		strb	r7,[sp,#94]		@c

		mov		lr,#0			@b3SmoothRight		
		cmp		r4,#0
		beq		next1h			@This is different from c code implementation	
		cmp		r5,#3			@when bStrong is False go to next1
		moveq	r12,#1
		cmp		r6,#3
		moveq	lr,#1

if2h:	
    ldr		r9,[sp,#H_Stack_Size+12]	@beta2
		ldr		r4,[sp,#96]		@ABS(deltaL2)
		ldr		r5,[sp,#100]	@ABS(deltaR2)
		cmp		r12,#0
		beq		if3h
		cmp		r4,r9
		movge	r12,#0

if3h:	
		cmp		lr,#0
		beq		if4h
		cmp		r5,r9
		movge	lr,#0

if4h:
  	cmp		r12,#0
		cmpne	lr,#0		
		beq		next1h
		
		adr		r4,asmditherL
		adr		r5,asmditherR
		mov		r12,#0			@z=0
		add		r4,r4,r2,lsl #1
		add		r5,r5,r2,lsl #1
		str		r4,[sp,#76]		@dithL
		str		r5,[sp,#80]		@dithR

loop1h:	
		ldr		r1,[sp,#64]
		ldrb	r5,[r0,#0] 		  @R1
		str		r12,[sp,#84]	  @z
		ldr		r11,[sp,#H_Stack_Size+4]	  @alpha		
		ldrb	r4,[r0,-r1]		  @L1		
		mov		r12,r12,lsl #1
		sub		r3,r0,r1
		subs	r10,r5,r4
		rsbmi	r10,r10,#0
		beq		loop1endh
		
		smulbb	r10,r11,r10
		sub		r3,r3,r1,lsl #1
		add		r11,r4,r5		@L1 + R1
		mov		r10,r10,asr #7	@n		
		cmp		r10,#1		
		bgt		loop1endh

		ldrb	r8,[r3],r1		  @L3
		ldr		r2,[sp,#80]
		ldrb	r6,[r3],r1		  @L2
		ldrb	r7,[r0,r1]		  @R2		
		ldrb	r9,[r0,r1,lsl #1] @R3		
		ldr		r1,[sp,#76]
		add		r11,r11,r6		@+L2
		ldrh	r2,[r2,r12]		@d2
		ldrh	r1,[r1,r12]		@d1

		add		r12,r7,r8		@R2+L3
		add		r12,r11,r12		@
		mov		r3,#25
		str		r10,[sp,#88]	  @n			
		smlabb	r12,r3,r12,r11
		add		r11,r11,r7		@L1+R1+L2+R2
		add		lr,r11,r9		@R1+L1+L2+R2+R3
		sub		r11,r11,r6		@L1+R1+R2
		add		r12,r12,r1
		
		smlabb	lr,r3,lr,r11
		ldr		r3,[sp,#88]		@n
		mov		r12,r12,asr #7	@t1,pL1[z]
		ldrb	r11,[sp,#94]	@c
		add		lr,lr,r2
		
		cmp		r3,#0
		mov		lr,lr,asr #7	@t2,r[z]
		beq		rt1h
		
		add		r3,r11,r4		@L1+c
		cmp		r12,r3
		movgt	r12,r3
		sub		r3,r4,r11		@L1-c
		cmp		r12,r3
		movlt	r12,r3
		add		r3,r11,r5		@R1+c
		cmp		lr,r3
		movgt	lr,r3
		sub		r3,r5,r11		@R1-c
		cmp		lr,r3
		movlt	lr,r3

rt1h:
  	ldr		r3,[sp,#64]		@r1
		strb	lr,[r0,#0]		@r[z]
		strb	r12,[r0,-r3]	@pL1[z]		

		ldrb	r10,[r0,-r3,lsl #2]	@L4
		add		r3,r3,r3,lsl #1
		ldrb	r11,[r0,r3]		@R4
		add		r3,r6,r8		@L2+L3
		add		r3,r3,r12		@+rt[-1] 
		add		r5,r5,r10		@R1+L4
		add		r5,r5,r3
		add		r1,r1,r3		@+d1
		mov		r3,#25
		smlabb	r5,r3,r5,r1
		add		r3,r7,r9		@R2+R3
		add		r3,r3,lr		@+rt[0]
		add		r4,r4,r11		@L1+R4
		add		r4,r4,r3

		add		r2,r2,r3
		mov		r3,#25
		mov		r5,r5,asr #7	@t1,rt[-2]	
		smlabb	r4,r3,r4,r2
		ldr		r1,[sp,#88]		@n
		ldrb	r2,[sp,#94]		@c
		mov		r4,r4,asr #7	@t2,rt[1]
		cmp		r1,#0
		beq		rt2h
		
		add		r3,r6,r2		@L2+c
		cmp		r5,r3
		movgt	r5,r3
		sub		r3,r6,r2		@L2-c
		cmp		r5,r3
		movlt	r5,r3
		add		r3,r7,r2		@R2+c
		cmp		r4,r3
		movgt	r4,r3
		sub		r3,r7,r2		@R2-c
		cmp		r4,r3
		movlt	r4,r3

rt2h:
  	ldr		r3,[sp,#64]			@r
		ldr		r1,[sp,#H_Stack_Size+16]	@bChroma
		add		r10,r10,r5		@L4+rt[-2]
		add		r10,r10,r8
		strb	r5,[r0,-r3,lsl #1]	@PL2[z]	@stall
		strb	r4,[r0,r3]			@PR2[z]
		cmp		r1,#0
		bne		loop1endh
		
		add		r12,r12,r8
		add		r12,r12,r10
		mov     r1,#25
		smlabb	r12,r1,r12,r10
		add		r11,r11,r9		@R4 + R3
		add		r11,r11,r4		@rt[1]
		add		r9,r9,lr		@R3+rt[0]
		add		r12,r12,#64		
		
		add		r9,r9,r11
		mov		r12,r12,asr #7		
		smlabb	r9,r1,r9,r11
		ldr		r3,[sp,#64]
		add		r9,r9,#64		
		mov		r9,r9,asr #7
		add		r4,r3,r3,lsl #1
		strb	r9,[r0,r3,lsl #1]	@PR3[z]
		strb	r12,[r0,-r4]		@PL3[z]	

loop1endh:
		 ldr		r12,[sp,#84]	
		 add		r0,r0,#1
		 add		r12,r12,#1
		 cmp		r12,#4
		 blt		loop1h
		@ bge		endh
		 add		sp,sp,#H_Offset_TmpBuff
		 ldmfd		sp!,{r4-r11,pc}
		 
next1h:	
		ldrb	r5,[sp,#92]		@Al
		ldrb	r6,[sp,#93]		@Ar	
		ldr		r1,[sp,#64]		@uPicth
		cmp		r5,#1			@stall
		cmpne	r6,#1
		beq		elseh
				
		ldr		lr,[sp,#H_Stack_Size+8]	@betaX
		mov		r11,sp			@deltaL		
		ldr		r9,[sp,#H_Stack_Size]	@Cl		
		mov		r8,#4			@loop variable
		ldr		r10,[sp,#72]	@Cr
		
loop2h:	
		ldrb	r2,[r0,-r1] 		@L1@pL1[z]@
		ldrb	r3,[r0,#0]			@R1@r[z]		
		ldrb	r5,[r0,-r1,lsl #1]	@L2@pL2[z]@
		ldr		r4,[sp,#H_Stack_Size+4]	@alpha
		subs	r7,r3,r2		@delta
		rsbmi	r7,r7,#0		@abs(delta)
		beq		loop2endh
		
		smulbb	r4,r4,r7		
		ldrb	r6,[r0,r1]			@R2@pR2[z]@
		sub		r7,r3,r2		@delta
		ldrb	r12,[sp,#94]	@c
		mov		r4,r4,asr #7
		cmp		r4,#2
		bgt		loop2endh
		
		add		r7,r5,r7,lsl #2	@L2+(delta<<2)
		sub		r7,r7,r6		@-R2
		add		r7,r7,#4
		ldr 	r4,[r11,#48]	@deltaR2[z]
		movs	r7,r7,asr #3	@d
		beq		deltaL2h		@This is added to avoid further caculations if d=0
		
		cmp		r7,r12			@d>c
		movgt	r7,r12			@d=c
		cmn		r7,r12			@d<-c
		rsbmi	r7,r12,#0		@d=-c			
		
		add 	r2,r2,r7		@L1+d
		sub 	r3,r3,r7		@R1-d
		usat	r2,#8,r2		
		usat	r3,#8,r3
		strb	r2,[r0,-r1]		@pL1[z]
		strb	r3,[r0,#0]		@r[z]

deltaL2h:
		ldr 	r2,[r11,#16]	@deltaL2[z]
		ldr 	r3,[r11,#0]		@deltaL[z]
		ldr 	r12,[r11,#32]	@deltaR[z]		
		cmp		r2,#0
		add		r3,r3,r2
		rsbmi	r2,r2,#0		@abs(deltaL2[z])
		cmp		r2,lr
		suble	r2,r3,r7
		bgt		deltaR2h
		
		movs	r2,r2,asr #1	@d2			
		beq		deltaR2h		@This is added to avoid further caculations if d2=0
								@This may become overhead if probability of d2 taking
								@nonzero value is more								
		cmp		r2,r9			
		movgt	r2,r9			
		cmn		r2,r9
		rsbmi	r2,r9,#0
		sub 	r5,r5,r2
		usat	r5,#8,r5
		strb	r5,[r0,-r1,lsl #1]	@pL2[z]
		b deltaR2h
.align 4	
asmditherR:
.short 64, 48, 96, 32, 80, 48, 48, 64, 64, 64, 80, 48, 32, 96, 48, 64
asmditherL:
.short 64, 80, 32, 96, 48, 80, 64, 48, 80, 64, 80, 48, 96, 32, 80, 64
deltaR2h:
		cmp		r4,#0
		add		r12,r12,r4
		rsbmi	r4,r4,#0
		cmp		r4,lr
		addle	r4,r12,r7		
		bgt		loop2endh
		
		movs	r4,r4,asr #1
		beq		loop2endh		@same reason of d2
		
		cmp		r4,r10
		movgt	r4,r10
		cmn		r4,r10
		rsbmi	r4,r10,#0
		sub 	r6,r6,r4
		usat	r6,#8,r6
		strb	r6,[r0,r1]		@pR2[z]
		
loop2endh:
		 add	r11,r11,#4	
		 subs	r8,r8,#1				 
		 add	r0,r0,#1
		 bgt	loop2h
	@	 beq	endh		
		 add	sp,sp,#H_Offset_TmpBuff
		 ldmfd	sp!,{r4-r11,pc}
		 
elseh:
		ldr		r9,[sp,#H_Stack_Size]	@Cl		
		ldr		r10,[sp,#72]			@Cr
		mov		r8,#4					@loop variable
		ldr		lr,[sp,#H_Stack_Size+8]	@betaX
		mov		r11,sp					@deltaL
		mov		r9,r9,asr #1
		mov		r10,r10,asr #1
		
loop3h:	
		ldrb	r2,[r0,-r1] 	@pL1[z]
		ldrb	r3,[r0,#0]		@R1		
		ldr		r4,[sp,#H_Stack_Size+4]	@alpha
		ldrb	r12,[sp,#94]	@c		
		subs	r7,r3,r2		@delta	@stall here
		rsbmi	r7,r7,#0		@abs(delta)
		beq		loop3endh
		
		smulbb	r4,r4,r7	
		mov		r12,r12,asr #1	@c>>1
		sub 	r7,r3,r2		@delta
		mov		r4,r4,asr #7
		cmp		r4,#3
		addle	r7,r7,#1		
		bgt		loop3endh
		
		movs	r7,r7,asr #1	@d
		beq		deltaL22h		@same reason of d

		cmp		r7,r12			@d>c>>1
		movgt	r7,r12			@d=c>>1
		cmn		r7,r12			@d<c>>1
		rsbmi	r7,r12,#0		@d=-(c>>1)			
		add 	r2,r2,r7		@L1+d
		sub 	r3,r3,r7		@R1-d
		usat	r2,#8,r2		
		usat	r3,#8,r3
		strb	r2,[r0,-r1]		@pL1[z]
		strb	r3,[r0,#0]		@r[z]

deltaL22h:
		cmp		r5,#1			@Al!=1
		ldrne 	r2,[r11,#16]	@deltaL2[z]
		ldrne 	r3,[r11,#0]		@deltaL[z]
		beq		deltaR22h

		cmp		r2,#0
		add		r3,r2,r3
		rsbmi	r2,r2,#0		@abs(deltaL2[z])
		cmp		r2,lr
		suble	r2,r3,r7		
		bgt		deltaR22h
		
		movs	r2,r2,asr #1	@d2
		beq		deltaR22h		@same reason of d2
		
		ldrb	r3,[r0,-r1,lsl #1]	@pL2[z]	
		cmp		r2,r9
		movgt	r2,r9	
		cmn		r2,r9
		rsbmi	r2,r9,#0		
		sub 	r3,r3,r2
		usat	r3,#8,r3
		strb	r3,[r0,-r1,lsl #1]	@pL2[z]

deltaR22h:
		cmp		r6,#1			@Ar!=1
		ldrne 	r2,[r11,#48]	@deltaR2[z]
		ldrne 	r3,[r11,#32]	@deltaR[z]		
		beq		loop3endh

		cmp		r2,#0
		add		r3,r3,r2
		rsbmi	r2,r2,#0
		cmp		r2,lr
		addle	r2,r3,r7
		bgt		loop3endh
		
		movs	r2,r2,asr #1	@d2
		beq		loop3endh		@same reason of d2
		
		ldrb	r3,[r0,r1]		@pR2[z]	
		cmp		r2,r10
		movgt	r2,r10
		cmn		r2,r10		
		rsbmi	r2,r10,#0
		sub 	r2,r3,r2
		usat	r2,#8,r2
		strb	r2,[r0,r1]		@pR2[z]	

loop3endh:
		 add	r11,r11,#4
		 subs	r8,r8,#1
		 add	r0,r0,#1
		 bgt	loop3h	
endh:
		add		sp,sp,#H_Offset_TmpBuff
		ldmfd	sp!,{r4-r11,pc}
	 @ENDP	@//ARMV6_rv9_h_loop_filter
	
	
ARMV6_rv9_v_loop_filter:	@PROC

        @.extern ditherR
        @.extern ditherL

		stmfd	sp!,{r4-r11,lr}
		pld		[r0]
		sub		r4,r0,#0x03		@rt = pPels		
		sub		sp,sp,#96
		str		r1,[sp,#64]		@r1,uPitch
				
		@sp		DeltaL
		@sp+16	DeltaL2	
		@sp+32	DeltaR	
		@sp+48	DeltaR2	

		ldrb	r6,[r4,#1]			@rt[-2]
		ldrb	r7,[r4,#2]			@rt[-1]
		ldrb	r8,[r4,#3]			@rt[0]
		ldrb	r9,[r4,#4]			@rt[1]
		ldrb	r10,[r4,#5]			@rt[2]		
		ldrb	r5,[r4],r1			@rt[-3]
		str		r2,[sp,#68]		@r2,uMBPos
		sub		r12,r6,r7			@r12 sum deltaL rt[-2] - rt[-1]
		str		r3,[sp,#72]		@r3,Cr
		sub		lr,r9,r8				@lr sum deltaR   r[1] - r[0]
		sub		r11,r6,r5				@rt[-2] - rt[-3]
		sub		r2,r9,r10			@rt[1] - rt[2]
		
		ldrb	r6,[r4,#1]
		str		r12,[sp,#0x00]
		ldrb	r7,[r4,#2]
		str		lr,[sp,#0x20]		
		ldrb	r8,[r4,#3]
		str		r11,[sp,#0x10]
		ldrb	r9,[r4,#4]
		str		r2,[sp,#0x30]				
		ldrb	r10,[r4,#5]		
		sub		r7,r6,r7		
		ldrb	r5,[r4],r1
		add		r12,r12,r7
		sub		r8,r9,r8		
		str		r7,[sp,#0x04]		
		add		lr,lr,r8
		str		r8,[sp,#0x24]		
		sub		r6,r6,r5		
		sub		r9,r9,r10
		str		r6,[sp,#0x14]
		add		r11,r11,r6
		
		ldrb	r6,[r4,#1]
		ldrb	r7,[r4,#2]
		str		r9,[sp,#0x34]		
		ldrb	r8,[r4,#3]
		add		r2,r2,r9
		ldrb	r9,[r4,#4]
		ldrb	r10,[r4,#5]		
		ldrb	r5,[r4],r1
		sub		r7,r6,r7
		add		r12,r12,r7
		sub		r8,r9,r8		
		str		r7,[sp,#0x08]		
		add		lr,lr,r8
		str		r8,[sp,#0x28]
		sub		r6,r6,r5
		sub		r9,r9,r10
		str		r6,[sp,#0x18]
		add		r11,r11,r6
		
		ldrb	r5,[r4]
		ldrb	r6,[r4,#1]
		ldrb	r7,[r4,#2]
		str		r9,[sp,#0x38]
		ldrb	r8,[r4,#3]
		add		r2,r2,r9
		ldrb	r9,[r4,#4]
		ldrb	r10,[r4,#5]
		sub		r7,r6,r7
		adds	r12,r12,r7
		str		r7,[sp,#0x0c]
		rsbmi	r12,r12,#0
		sub		r8,r9,r8
		adds	lr,lr,r8
		str		r8,[sp,#0x2c]
		rsbmi	lr,lr,#0
		sub		r6,r6,r5
		str		r6,[sp,#0x1c]
		sub		r9,r9,r10
		str		r9,[sp,#0x3c]
		adds	r11,r11,r6		@delta = deltaL2[0]+deltaL2[1]+deltaL2[2]+deltaL2[3]@

		ldr		r4,[sp,#0x8c]	@betaX
		rsbmi	r11,r11,#0
		adds	r2,r2,r9		@delta = deltaR2[0]+deltaR2[1]+deltaR2[2]+deltaR2[3]@
		rsbmi	r2,r2,#0
		mov		r5,#3			@Al
		mov		r6,#3			@Ar
		cmp		r12,r4,lsl #2
		movge	r5,#1		
		strb	r5,[sp,#92]	@Al
		cmp		lr,r4,lsl #2
		movge	r6,#1		
		strb	r6,[sp,#93]	@Ar

		add		r7,r5,r6
		cmp		r7,#2
	@	BLE		endv
		ldrgt	r4,[sp,#0x84]
		addle	sp,sp,#96
		ldmlefd	sp!,{r4-r11,pc}
		
		
		add		r7,r7,r3
		add		r7,r7,r4
		ldr		r4,[sp,#0x98]	@bStrong
		mov		r7,r7,asr #1
		strb	r7,[sp,#94]		@c
		
		mov		r12,#0			@b3SmoothLeft
		mov		lr,#0			@b3SmoothRight		
		cmp		r4,#0
		beq		next1v			@This is different from c code implementation	
		                        @when bStrong is False go to next1
		cmp		r5,#3			
		moveq	r12,#1
		cmp		r6,#3
		moveq	lr,#1

if2v:
  	ldr		r9,[sp,#0x90]		@beta2
		cmp		r12,#0
		beq		if3v		
		cmp		r11,r9
		movge	r12,#0

if3v:
  	cmp		lr,#0
		beq		if4v	
		cmp		r2,r9
		movge	lr,#0

if4v:
  	cmp		r12,#0
		cmpne	lr,#0
		ldrne	r2,[sp,#68]		
		adrne	r4,asmditherL
		adrne	r5,asmditherR
		beq		next1v
		
		pld		[r0]
		mov		r12,#0			@z=0
		add		r4,r4,r2,lsl #1
		add		r5,r5,r2,lsl #1
		str		r4,[sp,#76]		@dithL
		str		r5,[sp,#80]		@dithR

loop1v:	
		ldrb	r4,[r0,#-1]		@L1
		ldrb	r5,[r0,#0]		@R1
		ldr		r9,[sp,#0x88]	@alpha
		str		r12,[sp,#84]	@z
		subs	r8,r5,r4
		beq		loop1endv
		
		rsbmi	r8,r8,#0
		mov		r12,r12,lsl #1
		smulbb	r8,r9,r8
		ldrb	r6,[r0,#-2]		@L2	It may be modified
		ldrb	r7,[r0,#1]		@R2
		mov		r8,r8,asr #7	@n		
		cmp		r8,#1		
		bgt		loop1endv
		
		str		r8,[sp,#88]		@n
		ldrb	r8,[r0,#-3]		@L3
		ldrb	r9,[r0,#2]		@R3	
		pld		[r0,r1]
		ldr		r1,[sp,#76]		@dithL
		ldr		r2,[sp,#80]		@dithR
		add		r11,r4,r5		@L1 + R1
		add		r11,r11,r6		@+L2
		ldrh	r1,[r1,r12]		@d1
		ldrh	r2,[r2,r12]		@d2
		
		add		r12,r7,r8		@R2+L3
		add		r12,r11,r12		@
		mov		r3,#25
		smlabb	r12,r3,r12,r11
		add		r11,r11,r7		@+R2
		add		lr,r11,r9		
		sub		r11,r11,r6
		add		r12,r12,r1
		
		smlabb	lr,r3,lr,r11
		ldr		r3,[sp,#88]		@n
		ldrb	r11,[sp,#94]	@c
		mov		r12,r12,asr #7	@t1,rt[-1]
		add		lr,lr,r2
		
		cmp		r3,#0
		mov		lr,lr,asr #7	@t2,rt[0]
		beq		rt1v
		
		add		r3,r11,r4		@L1+c
		cmp		r12,r3
		movgt	r12,r3
		sub		r3,r4,r11		@L1-c
		cmp		r12,r3
		movlt	r12,r3
		add		r3,r11,r5		@R1+c
		cmp		lr,r3
		movgt	lr,r3
		sub		r3,r5,r11		@R1-c
		cmp		lr,r3
		movlt	lr,r3

rt1v:	
		ldrb	r10,[r0,#-4]	@L4
		add		r3,r6,r8		@L2+L3
		strb	r12,[r0,#-1]	@rt[-1]	
		add		r3,r3,r12		@+rt[-1] 
		add		r5,r5,r10		@R1+L4
		add		r5,r5,r3
		add		r1,r1,r3		@+d1
		
		mov		r3,#25
		strb	lr,[r0,#0]		@rt[0]
		smlabb	r5,r3,r5,r1
		ldrb	r11,[r0,#3]		@R4
		add		r3,r7,r9		@R2+R3
		add		r3,r3,lr		@+rt[0]
		add		r4,r4,r11		@L1+R4
		add		r4,r4,r3
		add		r2,r2,r3
		
		mov		r3,#25
		mov		r5,r5,asr #7	@t1,rt[-2]	
		smlabb	r4,r3,r4,r2
		ldrb	r2,[sp,#94]		@c
		ldr		r1,[sp,#88]		@n
		add		r3,r6,r2		@L2+c
		mov		r4,r4,asr #7	@t2,rt[1]
		
		cmp		r1,#0
		beq		rt2v
		cmp		r5,r3
		movgt	r5,r3
		sub		r3,r6,r2		@L2-c
		cmp		r5,r3
		movlt	r5,r3
		add		r3,r7,r2		@R2+c
		cmp		r4,r3
		movgt	r4,r3
		sub		r3,r7,r2		@R2-c
		cmp		r4,r3
		movlt	r4,r3

rt2v:	
		ldr		r1,[sp,#0x94]	@bChroma
		strb	r5,[r0,#-2]		@rt[-2]
		strb	r4,[r0,#1]		@rt[0]
		cmp		r1,#0
		add		r10,r10,r5		@L4+rt[-2]
		bne		loop1endv
		
		add		r10,r10,r8
		add		r12,r12,r8
		add		r12,r12,r10
		mov     r1,#25
		smlabb	r12,r1,r12,r10
		add		r11,r11,r9		@R4 + R3
		add		r9,r9,lr		@R3+rt[0]
		add		r11,r11,r4		@rt[1]
		add		r9,r9,r11
		add		r12,r12,#64
		
		MLA		r9,r1,r9,r11
		mov		r12,r12,asr #7
		strb	r12,[r0,#-3]
		add		r9,r9,#64
		mov		r9,r9,asr #7
		strb	r9,[r0,#2]	

loop1endv: 
		 ldr		r12,[sp,#84]	
		 ldr		r1,[sp,#64]
		 add		r12,r12,#1
		 cmp		r12,#4
		 add		r0,r0,r1
		 blt		loop1v
	@	 bge		endv
		 add		sp,sp,#96
		 ldmfd		sp!,{r4-r11,pc}

next1v:
  	ldrb	r5,[sp,#92]		@Al
		ldrb	r6,[sp,#93]		@Ar	
		ldr		r1,[sp,#64]		@uPicth
		cmp		r5,#1			@stall
		cmpne	r6,#1
		beq		elsev
				
		ldr		lr,[sp,#0x8c]	@betaX
		mov		r11,sp			@deltaL		
		ldr		r9,[sp,#0x84]	@Cl		
		ldr		r10,[sp,#72]	@Cr
		
		mov		r8,#4			@loop variable
loop2v:	
		ldrb	r2,[r0,#-1] 	@L1
		ldrb	r3,[r0,#0]		@R1		
		ldr		r4,[sp,#0x88]	@alpha
		ldrb	r12,[sp,#94]	@c
		subs	r7,r3,r2		@delta
		rsbmi	r7,r7,#0		@abs(delta)
		beq		loop2endv
		
		smulbb	r4,r4,r7		
		ldrb	r5,[r0,#-2]		@L2
		ldrb	r6,[r0,#1]		@R2
		pld		[r0,r1]
		sub 	r7,r3,r2		@delta
		mov		r4,r4,asr #7
		cmp		r4,#2
		bgt		loop2endv
		
		add		r7,r5,r7,lsl #2	@L2+(delta<<2)
		sub		r7,r7,r6		@-R2
		add		r7,r7,#4
		ldr 	r4,[r11,#48]	@deltaR2[z]
		movs	r7,r7,asr #3	@d
		beq		deltaL2v		@This is added to avoid further caculations if d=0
		
		cmp		r7,r12			@d>c
		movgt	r7,r12			@d=c
		cmn		r7,r12			@d<-c
		rsbmi	r7,r12,#0		@d=-c					
		add		r2,r2,r7		@L1+d
		sub 	r3,r3,r7		@R1-d
		usat	r2,#8,r2		
		usat	r3,#8,r3
		strb	r2,[r0,#-1]		@rt[-1]
		strb	r3,[r0,#0]		@rt[0]

deltaL2v:	
		ldr 	r2,[r11,#16]	@deltaL2[z]
		ldr 	r3,[r11,#0]		@deltaL[z]
		ldr 	r12,[r11,#32]	@deltaR[z]		
		cmp		r2,#0
		add		r3,r3,r2
		rsbmi	r2,r2,#0		@abs(deltaL2[z]
		cmp		r2,lr
		suble	r2,r3,r7
		bgt		deltaR2v
		
		movs	r2,r2,asr #1	@d2			
		beq		deltaR2v		@This is added to avoid further caculations if d2=0
		
		cmp		r2,r9			@This may become overhead if probability of d2 taking
		movgt	r2,r9			@nonzero value is more
		cmn		r2,r9
		rsbmi	r2,r9,#0
		sub 	r5,r5,r2
		usat	r5,#8,r5	
		strb	r5,[r0,#-2]		@rt[-2]

deltaR2v:
		cmp		r4,#0
		add		r12,r12,r4
		rsbmi	r4,r4,#0
		cmp		r4,lr
		addle	r4,r12,r7		
		bgt		loop2endv
		
		movs	r4,r4,asr #1
		beq		loop2endv		@same reason of d2
		
		cmp		r4,r10
		movgt	r4,r10
		cmn		r4,r10
		rsbmi	r4,r10,#0
		sub 	r6,r6,r4
		usat	r6,#8,r6
		strb	r6,[r0,#1]
loop2endv:
		 add	r11,r11,#4	
		 subs	r8,r8,#1				 
		 add	r0,r0,r1
		 bgt	loop2v
	@	 beq	endv	
		 add	sp,sp,#96
		 ldmfd	sp!,{r4-r11,pc}
		 	
elsev:	
		ldrb	r12,[sp,#94]	@c>>1		
		ldr		r9,[sp,#0x84]	@Cl		
		ldr		lr,[sp,#0x8c]	@betaX
		mov		r11,sp			@deltaL
		mov		r8,#4			@loop variable
		ldr		r4,[sp,#0x88]	@alpha
		mov		r12,r12,asr #1
		mov		r9,r9,asr #1
		
loop3v:	
		ldrb	r2,[r0,#-1] 	@L1
		ldrb	r3,[r0,#0]		@R1		
		pld		[r0,r1]
		subs	r7,r3,r2		@delta	@stall here
		rsbmi	r7,r7,#0		@abs(delta)
		beq		loop3endv
		
		smulbb	r10,r4,r7		
		sub 	r7,r3,r2		@delta
		add		r7,r7,#1		@
		mov		r10,r10,asr #7
		cmp		r10,#3
		bgt		loop3endv
		
		movs	r7,r7,asr #1	@d
		beq		deltaL22v		@same reason of d

		cmp		r7,r12			@d>c>>1
		movgt	r7,r12			@d=c>>1
		cmn		r7,r12			@d<c>>1
		rsbmi	r7,r12,#0		@d=-(c>>1)			
		add 	r2,r2,r7		@L1+d
		sub 	r3,r3,r7		@R1-d
		usat	r2,#8,r2		
		usat	r3,#8,r3
		strb	r2,[r0,#-1]		@rt[-1]
		strb	r3,[r0,#0]		@rt[0]

deltaL22v:
		cmp		r5,#1			@Al!=1
		beq		deltaR22v
		
		ldr 	r2,[r11,#16]	@deltaL2[z]
		ldr 	r3,[r11,#0]		@deltaL[z]							
		cmp		r2,#0
		add		r3,r2,r3
		rsbmi	r2,r2,#0		@abs(deltaL2[z]
		cmp		r2,lr
		suble	r2,r3,r7		
		bgt		deltaR22v
		
		movs	r2,r2,asr #1	@d2
		beq		deltaR22v		@same reason of d2
		
		ldrb	r3,[r0,#-2]		@rt[-2]	
		cmp		r2,r9
		movgt	r2,r9	
		cmn		r2,r9
		rsbmi	r2,r9,#0		
		sub 	r3,r3,r2
		usat	r3,#8,r3
		strb	r3,[r0,#-2]		@rt[-2]	@stall

deltaR22v:
		cmp		r6,#1			@Ar!=1
		beq		loop3endv
		
		ldr 	r2,[r11,#48]	@deltaR2[z]
		ldr 	r3,[r11,#32]	@deltaR[z]				
		ldr		r10,[sp,#72]	@Cr
		cmp		r2,#0
		add		r3,r3,r2
		rsbmi	r2,r2,#0
		cmp		r2,lr
		addle	r2,r3,r7
		bgt		loop3endv
		
		movs	r2,r2,asr #1	@d2
		beq		loop3endv		@same reason of d2
		
		mov		r10,r10,asr #1
		ldrb	r3,[r0,#1]		@rt[1]	
		cmp		r2,r10
		movgt	r2,r10
		cmn		r2,r10		
		rsbmi	r2,r10,#0
		sub		r2,r3,r2
		usat	r2,#8,r2
		strb	r2,[r0,#1]

loop3endv:
		 add	r11,r11,#4
		 subs	r8,r8,#1
		 add	r0,r0,r1
		 bgt	loop3v	
endv:
		add		sp,sp,#96
		ldmfd	sp!,{r4-r11,pc}
		@ENDP
		@.global	ARMV6_rv9_v_loop_filter
		
ARMV6_rv8_edge_filter:	@PROC
	 	stmfd	sp!,{r4-r11,lr}
		sub		sp,sp,#4
		str		r0,[sp,#0x00]	@pRec
		ldr		r4,[sp,#0x28]	@pStrengthV(r4)		 
		rsb		r6,r2,r1,lsl #2
		add		r6,r6,#4		@update factor
		add		r0,r0,#0x04		@r(r0)
		mov		lr,r3			@j
Ln1:		
		add		r4,r4,#1		@pStrengthV += 1
		sub		r5,r2,#4		@i(r5)
Ln2:		
		ldrb	r8,[r4],#1		@lTemp
		mov		r7,r0			@lpr(r7) = r
		cmp		r8,#0
		beq		Ln3
		ldrb	r9,[r7,#-0x02]	@lpr[-2]
		ldrb	r10,[r7,#0x01]	@lpr[ 1]
		ldrb	r11,[r7,#-0x01]	@lpr[-1]
		ldrb	r12,[r7,#0x00]	@lpr[ 0]
		sub		r9,r9,r10
		sub		r10,r11,r12		@stall
		sub		r9,r9,r10,lsl #2
		movs	r9,r9,asr #3	@d(r9)
		beq		Ln7
		cmp		r9,r8
		movgt	r9,r8
		cmn		r9,r8
		rsbmi	r9,r8,#0x00
		add		r11,r11,r9
		usat	r11,#8,r11	
		sub		r12,r12,r9
		usat	r12,#8,r12
		strb	r11,[r7,#-1]
		strb	r12,[r7,#0]

Ln7:
		add		r7,r7,r1		@lpr += uPitch
		ldrb	r9,[r7,#-0x02]	@lpr[-2]
		ldrb	r10,[r7,#0x01]	@lpr[ 1]
		ldrb	r11,[r7,#-0x01]	@lpr[-1]
		ldrb	r12,[r7,#0x00]	@lpr[ 0]		
		sub		r9,r9,r10
		sub		r10,r11,r12		@stall
		sub		r9,r9,r10,lsl #2
		movs	r9,r9,asr #3	@d(r9)
		beq		Ln8
		cmp		r9,r8
		movgt	r9,r8
		cmn		r9,r8
		rsbmi	r9,r8,#0x00
		add		r11,r11,r9
		usat	r11,#8,r11	
		sub		r12,r12,r9
		usat	r12,#8,r12
		strb	r11,[r7,#-1]
		strb	r12,[r7,#0]

Ln8:
		add		r7,r7,r1		@lpr += uPitch
		ldrb	r9,[r7,#-0x02]	@lpr[-2]
		ldrb	r10,[r7,#0x01]	@lpr[ 1]
		ldrb	r11,[r7,#-0x01]	@lpr[-1]
		ldrb	r12,[r7,#0x00]	@lpr[ 0]		
		sub		r9,r9,r10
		sub		r10,r11,r12		@stall
		sub		r9,r9,r10,lsl #2
		movs	r9,r9,asr #3	@d(r9)
		beq		Ln9
		cmp		r9,r8
		movgt	r9,r8
		cmn		r9,r8
		rsbmi	r9,r8,#0x00
		add		r11,r11,r9
		usat	r11,#8,r11	
		sub		r12,r12,r9
		usat	r12,#8,r12
		strb	r11,[r7,#-1]
		strb	r12,[r7,#0]

Ln9:
		add		r7,r7,r1		@lpr += uPitch
		ldrb	r9,[r7,#-0x02]	@lpr[-2]
		ldrb	r10,[r7,#0x01]	@lpr[ 1]
		ldrb	r11,[r7,#-0x01]	@lpr[-1]
		ldrb	r12,[r7,#0x00]	@lpr[ 0]		
		sub		r9,r9,r10
		sub		r10,r11,r12		@stall
		sub		r9,r9,r10,lsl #2
		movs	r9,r9,asr #3	@d(r9)
		beq		Ln3
		cmp		r9,r8
		movgt	r9,r8
		cmn		r9,r8
		rsbmi	r9,r8,#0x00
		add		r11,r11,r9
		usat	r11,#8,r11	
		sub		r12,r12,r9
		usat	r12,#8,r12
		strb	r11,[r7,#-1]
		strb	r12,[r7,#0]
		
Ln3:
		add		r0,r0,#4
		subs	r5,r5,#4
		bgt		Ln2
		ands    r11,r2,#0x0f
		addne   r4,r4,#2 
		add		r0,r0,r6
		subs	lr,lr,#4
		bgt		Ln1


		@filter horizontal edges
		ldr		r0,[sp,#0x00]	@pRec
		ldr		r4,[sp,#0x2c]	@pStrengthH
		ldr     r5,[sp,#0x30]   @row
		mov     r6,r3           @Height
		mov     r7,r0
		cmp     r5,#0           @if 0 == row
		moveq   r11,r2          @huwei 20090617 real_TCK
		addeq   r11,r11,#15     @huwei 20090617 real_TCK 
		moveq   r11,r11, asr #4  @huwei 20090617 real_TCK
		moveq   r11,r11, lsl #4  @huwei 20090617 real_TCK
		addeq	r7,r7,r1,lsl #2	@lpr(r7)
		@addeq	r4,r4,r2,asr #2 @pStrengthH += (uWidth >> 2)@ 
		addeq	r4,r4,r11,asr #2 @pStrengthH += ((((uWidth + 15) >> 4) << 4) >> 2)@addeq	r4,r4,r2,asr #2 @huwei 20090617 real_TCK
		subeq	r6,r6,#4		@j(r6)	
		rsb		r3,r2,r1,lsl #2	@update factor	
Ln4:
		mov		r5,r2			@i(r5)
Ln5:		
		ldrb	r8,[r4],#1
		cmp		r8,#0			@stall
		beq		Ln6
		ldrb	r9,[r7,-r1,lsl #1]
		ldrb	r10,[r7,r1]
		ldrb	r11,[r7,-r1]
		ldrb	r12,[r7,#0x00]
		sub		r9,r9,r10
		sub		r9,r9,r11,lsl #2
		add		r9,r9,r12,lsl #2
		movs	r9,r9,asr #3	@d(r9)
		beq		Ln10
		cmp		r9,r8
		movgt	r9,r8
		cmn		r9,r8
		rsbmi	r9,r8,#0
		add		r11,r11,r9
		usat	r11,#8,r11
		sub		r12,r12,r9
		usat	r12,#8,r12
		strb	r11,[r7,-r1]
		strb	r12,[r7,#0x00]

Ln10:
  	add		r7,r7,#1
		ldrb	r9,[r7,-r1,lsl #1]
		ldrb	r10,[r7,r1]
		ldrb	r11,[r7,-r1]
		ldrb	r12,[r7,#0x00]
		sub		r9,r9,r10
		sub		r9,r9,r11,lsl #2
		add		r9,r9,r12,lsl #2
		movs	r9,r9,asr #3	@d(r9)
		beq		Ln11
		cmp		r9,r8
		movgt	r9,r8
		cmn		r9,r8
		rsbmi	r9,r8,#0
		add		r11,r11,r9
		usat	r11,#8,r11
		sub		r12,r12,r9
		usat	r12,#8,r12
		strb	r11,[r7,-r1]
		strb	r12,[r7,#0x00]

Ln11:
  	add		r7,r7,#1
		ldrb	r9,[r7,-r1,lsl #1]
		ldrb	r10,[r7,r1]
		ldrb	r11,[r7,-r1]
		ldrb	r12,[r7,#0x00]
		sub		r9,r9,r10
		sub		r9,r9,r11,lsl #2
		add		r9,r9,r12,lsl #2
		movs	r9,r9,asr #3	@d(r9)
		beq		Ln12
		cmp		r9,r8
		movgt	r9,r8
		cmn		r9,r8
		rsbmi	r9,r8,#0
		add		r11,r11,r9
		usat	r11,#8,r11
		sub		r12,r12,r9
		usat	r12,#8,r12
		strb	r11,[r7,-r1]
		strb	r12,[r7,#0x00]

Ln12:
  	add		r7,r7,#1
		ldrb	r9,[r7,-r1,lsl #1]
		ldrb	r10,[r7,r1]
		ldrb	r11,[r7,-r1]
		ldrb	r12,[r7,#0x00]
		sub		r9,r9,r10
		sub		r9,r9,r11,lsl #2
		add		r9,r9,r12,lsl #2
		movs	r9,r9,asr #3	@d(r9)
		beq		Ln13
		cmp		r9,r8
		movgt	r9,r8
		cmn		r9,r8
		rsbmi	r9,r8,#0
		add		r11,r11,r9
		usat	r11,#8,r11
		sub		r12,r12,r9
		usat	r12,#8,r12
		strb	r11,[r7,-r1]
		strb	r12,[r7,#0x00]
Ln13:
  	sub		r7,r7,#3

Ln6:
		add		r7,r7,#4
		subs	r5,r5,#4
		bgt		Ln5
		add		r7,r7,r3
		ands  r11,r2,#0x0f @//huwei 20090617 real_TCK
		addne r4,r4,#2
		subs	r6,r6,#4 
		bgt		Ln4
		add		sp,sp,#4
		ldmfd	sp!,{r4-r11,pc}
		@ENDP
		.global	ARMV6_rv8_edge_filter
	@END