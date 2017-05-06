@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2010		            *
@								 	                                    *
@***********************************************************************/

	@AREA	|.text|, CODE
	.section .text
	
	.global	WMMX2_MCCopyChroma4_H00V00
	.global	WMMX2_MCCopyChroma4_H01V00
	.global	WMMX2_MCCopyChroma4_H02V00
	.global	WMMX2_MCCopyChroma4_H03V00
	.global	WMMX2_MCCopyChroma4_H00V01
	.global	WMMX2_MCCopyChroma4_H01V01
	.global	WMMX2_MCCopyChroma4_H02V01
	.global	WMMX2_MCCopyChroma4_H03V01
	.global	WMMX2_MCCopyChroma4_H00V02
	.global	WMMX2_MCCopyChroma4_H01V02
	@.global	WMMX2_MCCopyChroma4_H02V02
	.global	WMMX2_MCCopyChroma4_H03V02
	.global	WMMX2_MCCopyChroma4_H00V03
	.global	WMMX2_MCCopyChroma4_H01V03
	.global	WMMX2_MCCopyChroma4_H02V03
	
	.global	WMMX2_MCCopyChroma4Add_H00V00
	.global	WMMX2_MCCopyChroma4Add_H01V00
	.global	WMMX2_MCCopyChroma4Add_H02V00
	.global	WMMX2_MCCopyChroma4Add_H03V00
	.global	WMMX2_MCCopyChroma4Add_H00V01
	.global	WMMX2_MCCopyChroma4Add_H01V01
	.global	WMMX2_MCCopyChroma4Add_H02V01 
	.global	WMMX2_MCCopyChroma4Add_H03V01
	.global	WMMX2_MCCopyChroma4Add_H00V02
	.global	WMMX2_MCCopyChroma4Add_H01V02
	@.global	WMMX2_MCCopyChroma4Add_H02V02
	.global	WMMX2_MCCopyChroma4Add_H03V02
	.global	WMMX2_MCCopyChroma4Add_H00V03
	.global	WMMX2_MCCopyChroma4Add_H01V03
	.global	WMMX2_MCCopyChroma4Add_H02V03
	
@R0 = pSrc
@R1 = pDst
@R2 = uSrcPitch
@R3 = uDstPitch

	.macro GetSrc2x5	
		wldrd     wr0,[r0]       @row0 0-7 
		wldrd     wr1,[r0,#8]
		add	      r0,r0,r2
		wldrd     wr2,[r0]       @row1 0-7 
		wldrd     wr3,[r0,#8]
		add	      r0,r0,r2
		
		walignr1  wr0,wr0,wr1
		wsrldg    wr1,wr0,wcgr0  @row0 1-7
		walignr1  wr2,wr2,wr3
		wsrldg    wr3,wr2,wcgr0  @row1 1-7					
	.endm
	
	.macro	Interpolate4Horiz2x4        
        wunpckelub wr0,wr0
        wunpckelub wr1,wr1
        wunpckelub wr2,wr2
        wunpckelub wr3,wr3
        
        wmulsl     wr0,wr0,wr14  @0 - 3
		waddhss    wr0,wr0,wr1
		waddhss    wr0,wr0,wr13
		wsrahg     wr0,wr0,wcgr2
		
		wmulsl     wr2,wr2,wr14  @0 - 3
		waddhss    wr2,wr2,wr3
		waddhss    wr2,wr2,wr13
		wsrahg     wr2,wr2,wcgr2
				
		wpackhus   wr0,wr0,wr2	 
	.endm
	
	.macro	Interpolate4Vert2x4        
        wunpckelub wr0,wr0
        wunpckelub wr1,wr1
        wunpckelub wr9,wr3
        
        wmulsl     wr0,wr0,wr14  @0 - 3
		waddhss    wr0,wr0,wr1
		waddhss    wr0,wr0,wr13
		wsrahg     wr0,wr0,wcgr2
		
		wmulsl     wr1,wr1,wr14  @0 - 3
		waddhss    wr1,wr1,wr9
		waddhss    wr1,wr1,wr13
		wsrahg     wr1,wr1,wcgr2
				
		wpackhus   wr0,wr0,wr1		 
	.endm
	
	.macro	Store2x4    AddFlag
	 		
		.if \AddFlag > 0
			wavg2br    wr0,wr0,wr15 
		.endif
		
		tmrrc       r7,r8,wr0
		str		    r7,[r1],r3
		str		    r8,[r1],r3				 
	.endm
	
	.macro	StoreHV2x4    AddFlag
	 		
		.if \AddFlag > 0
			wavg2br    wr8,wr8,wr15 
		.endif
		
		tmrrc       r7,r8,wr8
		str		    r7,[r1],r3
		str		    r8,[r1],r3					 
	.endm
	
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@-----------------------------------------------------------------------------------------------------
WMMX2_MCCopyChroma4_H00V00:	@PROC
    STMFD   sp!,{r4-r11,lr}
    
	ANDS    r12,r0,#0x03
	BNE		H00V00Chroma4_notAligned4
H00V00Chroma4_Aligned4:	
	
	LDR		r12,[r0],r2
	LDR		lr,[r0],r2
	LDR		r4,[r0],r2
	LDR		r2,[r0]
	STR		r12,[r1],r3
	STR		lr,[r1],r3
	STR		r4,[r1],r3
	STR		r2,[r1]
	
	LDMFD   sp!,{r4-r11,pc}
H00V00Chroma4_notAligned4:

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
	
	@ENDP

@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4Add_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@-----------------------------------------------------------------------------------------------------
WMMX2_MCCopyChroma4Add_H00V00:	@PROC
	stmfd      sp!,{r4-r11,lr}
  
    mov        r5,#32
    tmcr       wcgr2,r5  
      
	ands       r14,r0,#0x07
	mov        r6,r1
	tmcr       wcgr1,r14
	bne		   AddH00V00Chroma4_notAligned8
AddH00V00Chroma4_Aligned8:	
	wldrd      wr0,[r0]        @0
	add  	   r0,r0,r2
	wldrd      wr1,[r0]        @1
	add  	   r0,r0,r2
	wldrd      wr2,[r0]        @2
	add  	   r0,r0,r2
	wldrd      wr3,[r0]        @3
	
	ldr		   r4,[r6],r3
	ldr		   r5,[r6],r3
	ldr		   r7,[r6],r3
	ldr		   r8,[r6]
	
	wslldg     wr0,wr0,wcgr2
	tmcrr      wr4,r4,r5
	wslldg     wr2,wr2,wcgr2
	tmcrr      wr5,r7,r8	
	waligni    wr0,wr0,wr1,#4
	waligni    wr2,wr2,wr3,#4	
	
	wavg2br    wr0,wr0,wr4
	wavg2br    wr2,wr2,wr5
	
	tmrrc      r4,r5,wr0
	tmrrc      r7,r8,wr2
	
	str		   r4,[r1],r3
	str		   r5,[r1],r3
	str		   r7,[r1],r3
	str		   r8,[r1]
	
	ldmfd      sp!,{r4-r11,pc}
AddH00V00Chroma4_notAligned8:
    bic        r0,r0,#7
    
    wldrd      wr0,[r0]      @0
	wldrd      wr1,[r0,#8]
	add	       r0,r0,r2	
	wldrd      wr2,[r0]      @1
	wldrd      wr3,[r0,#8]
	add	       r0,r0,r2	
	wldrd      wr4,[r0]      @2
	wldrd      wr5,[r0,#8]
	add	       r0,r0,r2	
	wldrd      wr6,[r0]      @3
	wldrd      wr7,[r0,#8]
	
	ldr		   r4,[r6],r3
	ldr		   r5,[r6],r3
	ldr		   r7,[r6],r3
	ldr		   r8,[r6]
	
	walignr1   wr0,wr0,wr1
	walignr1   wr1,wr2,wr3
	walignr1   wr2,wr4,wr5
	walignr1   wr3,wr6,wr7
	
	wslldg     wr0,wr0,wcgr2
	tmcrr      wr4,r4,r5
	wslldg     wr2,wr2,wcgr2
	tmcrr      wr5,r7,r8	
	waligni    wr0,wr0,wr1,#4
	waligni    wr2,wr2,wr3,#4		
	
	wavg2br    wr0,wr0,wr4
	wavg2br    wr2,wr2,wr5
	
	tmrrc      r4,r5,wr0
	tmrrc      r7,r8,wr2
	
	str		   r4,[r1],r3
	str		   r5,[r1],r3
	str		   r7,[r1],r3
	str		   r8,[r1]
	
	ldmfd      sp!,{r4-r11,pc}
	
	@ENDP
	
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4_H01V00(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (3,1) 
@	h0 = (3*p00 + p01 + 1 ) >> 2 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4_H01V00:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov         r4,#1
   	mov		    r5,#3
   	mov         r6,#2
   	mov         r7,#8
   	
	tbcsth      wr13,r4
	tbcsth      wr14,r5	   
    tmcr        wcgr2,r6    
    tmcr        wcgr0,r7 
    
    mov	        r11,#2
    ands        r14,r0,#0x07   
H01V00Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
H01V00Chroma4Loop:	
    GetSrc2x5	
    Interpolate4Horiz2x4
    Store2x4       0
    
    subs        r11,r11,#1 
	bgt         H01V00Chroma4Loop
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP

@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4Add_H01V00(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (3,1) 
@	h0 = (3*p00 + p01 + 1 ) >> 2 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4Add_H01V00:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov         r4,#1
   	mov		    r5,#3
   	mov         r6,#2
   	mov         r7,#8
   	
	tbcsth      wr13,r4
	tbcsth      wr14,r5	   
    tmcr        wcgr2,r6    
    tmcr        wcgr0,r7 
 
    mov         r10,r1   
    mov	        r11,#2
    ands        r14,r0,#0x07
AddH01V00Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
AddH01V00Chroma4Loop:	
    GetSrc2x5
    
    ldr		   r4,[r10],r3
	ldr		   r5,[r10],r3
	
    Interpolate4Horiz2x4
    tmcrr      wr15,r4,r5
    Store2x4       1
        
    subs        r11,r11,#1 
	bgt         AddH01V00Chroma4Loop
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4_H02V00(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (1,1) 
@	h0 = (p00 + p01 + 1 ) >> 1 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4_H02V00:	@PROC
    stmfd       sp!,{r4-r11,lr}
 
    mov         r5,#8
    tmcr        wcgr0,r5 
    mov         r5,#32
    tmcr        wcgr2,r5    
    
    ands        r14,r0,#0x07
H02V00Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
H02V00Chroma4Loop:	
	wldrd     wr0,[r0]       @row0 0-7 
	wldrd     wr1,[r0,#8]
	add	      r0,r0,r2
	wldrd     wr2,[r0]       @row1 0-7 
	wldrd     wr3,[r0,#8]
	add	      r0,r0,r2
	wldrd     wr4,[r0]       @row2 0-7 
	wldrd     wr5,[r0,#8]
	add	      r0,r0,r2
	wldrd     wr6,[r0]       @row3 0-7 
	wldrd     wr7,[r0,#8]
	
	walignr1  wr0,wr0,wr1
	walignr1  wr1,wr2,wr3
	walignr1  wr2,wr4,wr5
	walignr1  wr3,wr6,wr7
	
	wsrldg    wr4,wr0,wcgr0  @row0 1-7
	wsrldg    wr5,wr1,wcgr0  @row1 1-7
	wsrldg    wr6,wr2,wcgr0  @row2 1-7
	wsrldg    wr7,wr3,wcgr0  @row3 1-7
	
	wslldg    wr0,wr0,wcgr2
	wslldg    wr2,wr2,wcgr2	
	wslldg    wr4,wr4,wcgr2
	wslldg    wr6,wr6,wcgr2	
	waligni   wr0,wr0,wr1,#4
	waligni   wr2,wr2,wr3,#4
	waligni   wr4,wr4,wr5,#4
	waligni   wr6,wr6,wr7,#4
	
	wavg2br   wr0,wr0,wr4
	wavg2br   wr2,wr2,wr6	
	tmrrc     r4,r5,wr0
	tmrrc     r6,r7,wr2
	
	str		   r4,[r1],r3
	str		   r5,[r1],r3
	str		   r6,[r1],r3
	str		   r7,[r1]

	ldmfd       sp!,{r4-r11,pc}	
    @ENDP

@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4Add_H02V00(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (1,1) 
@	h0 = (p00 + p01 + 1 ) >> 1 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4Add_H02V00:	@PROC
    stmfd       sp!,{r4-r11,lr}
 
    mov         r5,#8
    tmcr        wcgr0,r5 
    mov         r5,#32
    tmcr        wcgr2,r5    
    
    mov         r10,r1
    ands        r14,r0,#0x07
AddH02V00Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
AddH02V00Chroma4Loop:
	wldrd     wr0,[r0]       @row0 0-7 
	wldrd     wr1,[r0,#8]
	add	      r0,r0,r2
	wldrd     wr2,[r0]       @row1 0-7 
	wldrd     wr3,[r0,#8]
	add	      r0,r0,r2
	wldrd     wr4,[r0]       @row2 0-7 
	wldrd     wr5,[r0,#8]
	add	      r0,r0,r2
	wldrd     wr6,[r0]       @row3 0-7 
	wldrd     wr7,[r0,#8]
	
	ldr		   r4,[r10],r3
	ldr		   r5,[r10],r3
	ldr		   r7,[r10],r3
	ldr		   r8,[r10]
	
	walignr1  wr0,wr0,wr1
	walignr1  wr1,wr2,wr3
	walignr1  wr2,wr4,wr5
	walignr1  wr3,wr6,wr7
	
	wsrldg    wr4,wr0,wcgr0  @row0 1-7
	wsrldg    wr5,wr1,wcgr0  @row1 1-7
	wsrldg    wr6,wr2,wcgr0  @row2 1-7
	wsrldg    wr7,wr3,wcgr0  @row3 1-7
	
	tmcrr      wr8,r4,r5
	tmcrr      wr9,r6,r7
	
	wslldg    wr0,wr0,wcgr2
	wslldg    wr2,wr2,wcgr2	
	wslldg    wr4,wr4,wcgr2
	wslldg    wr6,wr6,wcgr2	
	waligni   wr0,wr0,wr1,#4
	waligni   wr2,wr2,wr3,#4
	waligni   wr4,wr4,wr5,#4
	waligni   wr6,wr6,wr7,#4
	
	wavg2br   wr0,wr0,wr4
	wavg2br   wr2,wr2,wr6
	wavg2br   wr0,wr0,wr8
	wavg2br   wr2,wr2,wr9	
	tmrrc     r4,r5,wr0
	tmrrc     r6,r7,wr2
	
	str		   r4,[r1],r3
	str		   r5,[r1],r3
	str		   r6,[r1],r3
	str		   r7,[r1]
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4_H03V00(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (1,3) 
@	h0 = (p00 + 3*p01 + 1 ) >> 2 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4_H03V00:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov         r4,#1
   	mov		    r5,#3
   	mov         r6,#2
   	mov         r7,#8
   	
	tbcsth      wr13,r4	
	tbcsth      wr14,r5	    
    tmcr        wcgr2,r6  
    tmcr        wcgr0,r7 
    
    mov         r10,r1
    mov	        r11,#2
    ands        r14,r0,#0x07
H03V00Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
H03V00Chroma4Loop:	
	wldrd     wr0,[r0]       @row0 0-7 
	wldrd     wr1,[r0,#8]
	add	      r0,r0,r2
	wldrd     wr2,[r0]       @row1 0-7 
	wldrd     wr3,[r0,#8]
	add	      r0,r0,r2
	
	walignr1  wr1,wr0,wr1
	wsrldg    wr0,wr1,wcgr0  @row0 1-7
	walignr1  wr3,wr2,wr3
	wsrldg    wr2,wr3,wcgr0  @row1 1-7	
	
    Interpolate4Horiz2x4
    Store2x4       0
    
    subs        r11,r11,#1 
	bgt         H03V00Chroma4Loop
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP

@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4Add_H03V00(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (1,3) 
@	h0 = (p00 + 3*p01 + 1 ) >> 2 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4Add_H03V00:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov         r4,#1
   	mov		    r5,#3
   	mov         r6,#2
   	mov         r7,#8
   	
	tbcsth      wr13,r4	
	tbcsth      wr14,r5	    
    tmcr        wcgr2,r6  
    tmcr        wcgr0,r7 
    
    mov	        r11,#2
    ands        r14,r0,#0x07
AddH03V00Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
AddH03V00Chroma4Loop:	
	wldrd     wr0,[r0]       @row0 0-7 
	wldrd     wr1,[r0,#8]
	add	      r0,r0,r2
	wldrd     wr2,[r0]       @row1 0-7 
	wldrd     wr3,[r0,#8]
	add	      r0,r0,r2
	
	ldr		  r4,[r10],r3
	ldr		  r5,[r10],r3
	
	walignr1  wr1,wr0,wr1
	wsrldg    wr0,wr1,wcgr0  @row0 1-7
	walignr1  wr3,wr2,wr3
	wsrldg    wr2,wr3,wcgr0  @row1 1-7
	tmcrr     wr15,r4,r5	
	
    Interpolate4Horiz2x4
    Store2x4       1
    
    subs        r11,r11,#1 
	bgt         AddH03V00Chroma4Loop
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4_H00V01(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	v filter (3,1)
@	v0 = (3*p00 + p10 + 2 ) >> 2 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4_H00V01:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov         r4,#2
   	mov		    r5,#3
   	
	tbcsth      wr13,r4	
	tbcsth      wr14,r5	    
    tmcr        wcgr2,r4 
    
    mov	        r11,#2
    ands        r14,r0,#0x07
H00V01Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
	wldrd       wr0,[r0]       @row0 0-7 
	wldrd       wr1,[r0,#8]
	add	        r0,r0,r2
	walignr1    wr0,wr0,wr1
H00V01Chroma4Loop:	
	wldrd       wr1,[r0]       @row1 0-7 
	wldrd       wr2,[r0,#8]
	add	        r0,r0,r2
	wldrd       wr3,[r0]       @row2 0-7 
	wldrd       wr4,[r0,#8]
	add	        r0,r0,r2
	
	walignr1    wr1,wr1,wr2
	walignr1    wr3,wr3,wr4
	
    Interpolate4Vert2x4
    Store2x4       0
    wmov        wr0,wr3
    
    subs        r11,r11,#1 
	bgt         H00V01Chroma4Loop
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP

@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4Add_H00V01(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	v filter (3,1)
@	v0 = (3*p00 + p10 + 2 ) >> 2 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4Add_H00V01:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov         r4,#2
   	mov		    r5,#3
   	
	tbcsth      wr13,r4	
	tbcsth      wr14,r5	    
    tmcr        wcgr2,r4 
    
    mov         r10,r1
    mov	        r11,#2
    ands        r14,r0,#0x07
AddH00V01Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
	wldrd       wr0,[r0]       @row0 0-7 
	wldrd       wr1,[r0,#8]
	add	        r0,r0,r2
	walignr1    wr0,wr0,wr1
AddH00V01Chroma4Loop:	
	wldrd       wr1,[r0]       @row1 0-7 
	wldrd       wr2,[r0,#8]
	add	        r0,r0,r2
	wldrd       wr3,[r0]       @row2 0-7 
	wldrd       wr4,[r0,#8]
	add	        r0,r0,r2
	
	walignr1    wr1,wr1,wr2
	walignr1    wr3,wr3,wr4
	
	ldr		  r4,[r10],r3
	ldr		  r5,[r10],r3
	
    Interpolate4Vert2x4
    tmcrr     wr15,r4,r5
    Store2x4       1
    wmov        wr0,wr3
    
    subs        r11,r11,#1 
	bgt         AddH00V01Chroma4Loop
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4_H01V01(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (3,1)
@	v filter (3,1)
@	h0 = (3*p00 + p01)@	v0 = (3*h0 + h1 + 7 ) >> 4   OR  v0 = (9*p00 + 3*(p01 + p10) + p11 + 7) >> 4
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4_H01V01:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov         r4,#7
   	mov		    r5,#3
   	mov         r6,#4
   	mov         r7,#8
   	
	tbcsth      wr13,r4	
	tbcsth      wr14,r5	    
    tmcr        wcgr2,r6     
    tmcr        wcgr0,r7 
    
    mov	        r11,#2
    ands        r14,r0,#0x07
H01V01Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
	wldrd       wr2,[r0]       @row0 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row0 1-7
	wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3	
    wmulsl      wr4,wr2,wr14  @0 - 3
	waddhss     wr5,wr4,wr3 	
H01V01Chroma4Loop:	
    wldrd       wr0,[r0]       @row1 0-7 
	wldrd       wr1,[r0,#8]
	add	        r0,r0,r2
	wldrd       wr2,[r0]       @row2 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	
	walignr1    wr0,wr0,wr1
	wsrldg      wr1,wr0,wcgr0  @row1 1-7	
    wunpckelub  wr0,wr0
    wunpckelub  wr1,wr1   
    walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row2 1-7	
    wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3
    
	wmulsl      wr6,wr0,wr14   @0 - 3
	waddhss     wr12,wr6,wr1
	wmulsl      wr7,wr5,wr14
	waddhss     wr8,wr7,wr12
	waddhss     wr8,wr8,wr13
	wsrahg      wr8,wr8,wcgr2
	
	wmulsl      wr6,wr2,wr14   @0 - 3
	waddhss     wr5,wr6,wr3
	wmulsl      wr7,wr12,wr14
	waddhss     wr10,wr7,wr5
	waddhss     wr10,wr10,wr13
	wsrahg      wr10,wr10,wcgr2
	wpackhus    wr8,wr8,wr10
	
	StoreHV2x4      0
    
    subs        r11,r11,#1 
	bgt         H01V01Chroma4Loop
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP

@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4Add_H01V01(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (3,1)
@	v filter (3,1)
@	h0 = (3*p00 + p01)@	v0 = (3*h0 + h1 + 7 ) >> 4   OR  v0 = (9*p00 + 3*(p01 + p10) + p11 + 7) >> 4
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4Add_H01V01:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov         r4,#7
   	mov		    r5,#3
   	mov         r6,#4
   	mov         r7,#8
   	
	tbcsth      wr13,r4	
	tbcsth      wr14,r5	    
    tmcr        wcgr2,r6     
    tmcr        wcgr0,r7 
    
    mov         r10,r1
    mov	        r11,#2
    ands        r14,r0,#0x07
AddH01V01Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
	wldrd       wr2,[r0]       @row0 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row0 1-7
	wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3	
    wmulsl      wr4,wr2,wr14  @0 - 3
	waddhss     wr5,wr4,wr3 	
AddH01V01Chroma4Loop:	
    wldrd       wr0,[r0]       @row1 0-7 
	wldrd       wr1,[r0,#8]
	add	        r0,r0,r2
	wldrd       wr2,[r0]       @row2 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	
	ldr		    r4,[r10],r3
	ldr		    r5,[r10],r3
	
	walignr1    wr0,wr0,wr1
	wsrldg      wr1,wr0,wcgr0  @row1 1-7	
    wunpckelub  wr0,wr0
    wunpckelub  wr1,wr1    
    walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row2 1-7	
    wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3
    
    tmcrr       wr15,r4,r5
    
	wmulsl      wr6,wr0,wr14   @0 - 3
	waddhss     wr12,wr6,wr1
	wmulsl      wr7,wr5,wr14
	waddhss     wr8,wr7,wr12
	waddhss     wr8,wr8,wr13
	wsrahg      wr8,wr8,wcgr2
	
	wmulsl      wr6,wr2,wr14   @0 - 3
	waddhss     wr5,wr6,wr3
	wmulsl      wr7,wr12,wr14
	waddhss     wr10,wr7,wr5
	waddhss     wr10,wr10,wr13
	wsrahg      wr10,wr10,wcgr2
	wpackhus    wr8,wr8,wr10
	
	StoreHV2x4      1
    
    subs        r11,r11,#1 
	bgt         AddH01V01Chroma4Loop
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4_H02V01(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (3,1)
@	v filter (1,1)
@	h0 = (3*p00 + p01)@	v0 = (h0 + h1 + 4 ) >> 4   OR  v0 = (3*(p00 + p01) + p10 + p11 + 4) >> 3
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4_H02V01:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov         r4,#4
   	mov		    r5,#3
   	mov         r7,#8
   	
	tbcsth      wr13,r4	
	tbcsth      wr14,r5	    
    tmcr        wcgr2,r5    
    tmcr        wcgr0,r7 
    
    mov	        r11,#2
    ands        r14,r0,#0x07
H02V01Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
	wldrd       wr2,[r0]       @row0 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row0 1-7
	wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3	 
	waddhss     wr5,wr2,wr3    @0 - 3	
H02V01Chroma4Loop:	
    wldrd       wr0,[r0]       @row1 0-7 
	wldrd       wr1,[r0,#8]
	add	        r0,r0,r2
	wldrd       wr2,[r0]       @row2 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	
	walignr1    wr0,wr0,wr1
	wsrldg      wr1,wr0,wcgr0  @row1 1-7	
    wunpckelub  wr0,wr0
    wunpckelub  wr1,wr1
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row2 1-7	
    wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3
      
	waddhss     wr12,wr0,wr1    @0 - 3
	wmulsl      wr7,wr5,wr14
	waddhss     wr8,wr7,wr12
	waddhss     wr8,wr8,wr13
	wsrahg      wr8,wr8,wcgr2
	
	waddhss     wr5,wr2,wr3    @0 - 3
	wmulsl      wr7,wr12,wr14
	waddhss     wr10,wr7,wr5
	waddhss     wr10,wr10,wr13
	wsrahg      wr10,wr10,wcgr2
	wpackhus    wr8,wr8,wr10
	
	StoreHV2x4      0
    
    subs        r11,r11,#1 
	bgt         H02V01Chroma4Loop
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP

@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4Add_H02V01(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (3,1)
@	v filter (1,1)
@	h0 = (3*p00 + p01)@	v0 = (h0 + h1 + 4 ) >> 4   OR  v0 = (3*(p00 + p01) + p10 + p11 + 4) >> 3
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4Add_H02V01:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov         r4,#4
   	mov		    r5,#3
   	mov         r7,#8
   	
	tbcsth      wr13,r4	
	tbcsth      wr14,r5	    
    tmcr        wcgr2,r5    
    tmcr        wcgr0,r7 
    
    mov         r10,r1
    mov	        r11,#2
    ands        r14,r0,#0x07
AddH02V01Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
	wldrd       wr2,[r0]       @row0 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row0 1-7
	wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3	 
	waddhss     wr5,wr2,wr3    @0 - 3	
AddH02V01Chroma4Loop:	
    wldrd       wr0,[r0]       @row1 0-7 
	wldrd       wr1,[r0,#8]
	add	        r0,r0,r2
	wldrd       wr2,[r0]       @row2 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	
	ldr		    r4,[r10],r3
	ldr		    r5,[r10],r3
	
	walignr1    wr0,wr0,wr1
	wsrldg      wr1,wr0,wcgr0  @row1 1-7	
    wunpckelub  wr0,wr0
    wunpckelub  wr1,wr1
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row2 1-7	
    wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3
    
    tmcrr       wr15,r4,r5
      
	waddhss     wr12,wr0,wr1    @0 - 3
	wmulsl      wr7,wr5,wr14
	waddhss     wr8,wr7,wr12
	waddhss     wr8,wr8,wr13
	wsrahg      wr8,wr8,wcgr2
	
	waddhss     wr5,wr2,wr3    @0 - 3
	wmulsl      wr7,wr12,wr14
	waddhss     wr10,wr7,wr5
	waddhss     wr10,wr10,wr13
	wsrahg      wr10,wr10,wcgr2
	wpackhus    wr8,wr8,wr10
	
	StoreHV2x4      1
    
    subs        r11,r11,#1 
	bgt         AddH02V01Chroma4Loop
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4_H03V01(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (1,3)
@	v filter (3,1)
@	h0 = (p00 + 3*p01)@	v0 = (3*h0 + h1 + 7 ) >> 4   OR  v0 = (3*(p00 + p11) + 9*p01 + p10 + 7) >> 4
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4_H03V01:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov         r4,#7
   	mov		    r5,#3
   	mov         r6,#4
   	mov         r7,#8
   	
	tbcsth      wr13,r4	
	tbcsth      wr14,r5	   
    tmcr        wcgr2,r6    
    tmcr        wcgr0,r7 
    
    mov	        r11,#2
    ands        r14,r0,#0x07
H03V01Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
	wldrd       wr2,[r0]       @row0 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row0 1-7
	wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3	
    wmulsl      wr4,wr3,wr14   @0 - 3
	waddhss     wr5,wr4,wr2 	
H03V01Chroma4Loop:	
    wldrd       wr0,[r0]       @row1 0-7 
	wldrd       wr1,[r0,#8]
	add	        r0,r0,r2
	wldrd       wr2,[r0]       @row2 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	
	walignr1    wr0,wr0,wr1
	wsrldg      wr1,wr0,wcgr0  @row1 1-7	
    wunpckelub  wr0,wr0
    wunpckelub  wr1,wr1
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row2 1-7	
    wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3
    
	wmulsl      wr6,wr1,wr14   @0 - 3
	waddhss     wr12,wr6,wr0
	wmulsl      wr7,wr5,wr14
	waddhss     wr8,wr7,wr12
	waddhss     wr8,wr8,wr13
	wsrahg      wr8,wr8,wcgr2
	
	wmulsl      wr6,wr3,wr14   @0 - 3
	waddhss     wr5,wr6,wr2
	wmulsl      wr7,wr12,wr14
	waddhss     wr10,wr7,wr5
	waddhss     wr10,wr10,wr13
	wsrahg      wr10,wr10,wcgr2
	wpackhus    wr8,wr8,wr10
	
	StoreHV2x4      0
    
    subs        r11,r11,#1 
	bgt         H03V01Chroma4Loop
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP

@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4_H03V01(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (1,3)
@	v filter (3,1)
@	h0 = (p00 + 3*p01)@	v0 = (3*h0 + h1 + 7 ) >> 4   OR  v0 = (3*(p00 + p11) + 9*p01 + p10 + 7) >> 4
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4Add_H03V01:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov         r4,#7
   	mov		    r5,#3
   	mov         r6,#4
   	mov         r7,#8
   	
	tbcsth      wr13,r4	
	tbcsth      wr14,r5	   
    tmcr        wcgr2,r6    
    tmcr        wcgr0,r7 
    
    mov         r10,r1
    mov	        r11,#2
    ands        r14,r0,#0x07
AddH03V01Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
	wldrd       wr2,[r0]       @row0 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row0 1-7
	wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3	
    wmulsl      wr4,wr3,wr14   @0 - 3
	waddhss     wr5,wr4,wr2 	
AddH03V01Chroma4Loop:	
    wldrd       wr0,[r0]       @row1 0-7 
	wldrd       wr1,[r0,#8]
	add	        r0,r0,r2
	wldrd       wr2,[r0]       @row2 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	
	ldr		    r4,[r10],r3
	ldr		    r5,[r10],r3
	
	walignr1    wr0,wr0,wr1
	wsrldg      wr1,wr0,wcgr0  @row1 1-7	
    wunpckelub  wr0,wr0
    wunpckelub  wr1,wr1
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row2 1-7	
    wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3
    
    tmcrr       wr15,r4,r5
    
	wmulsl      wr6,wr1,wr14   @0 - 3
	waddhss     wr12,wr6,wr0
	wmulsl      wr7,wr5,wr14
	waddhss     wr8,wr7,wr12
	waddhss     wr8,wr8,wr13
	wsrahg      wr8,wr8,wcgr2
	
	wmulsl      wr6,wr3,wr14   @0 - 3
	waddhss     wr5,wr6,wr2
	wmulsl      wr7,wr12,wr14
	waddhss     wr10,wr7,wr5
	waddhss     wr10,wr10,wr13
	wsrahg      wr10,wr10,wcgr2
	wpackhus    wr8,wr8,wr10
	
	StoreHV2x4      1
    
    subs        r11,r11,#1 
	bgt         AddH03V01Chroma4Loop
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4_H00V02(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	v filter (1,1) 
@	v0 = (p00 + p10 ) >> 1 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4_H00V02:	@PROC
    stmfd       sp!,{r4-r11,lr} 
    
    mov         r5,#32
    tmcr        wcgr2,r5    

    ands        r14,r0,#0x07
H00V02Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
H00V02Chroma4Loop:	
	wldrd     wr0,[r0]       @row0 0-7 
	wldrd     wr1,[r0,#8]
	add	      r0,r0,r2
	wldrd     wr2,[r0]       @row1 0-7 
	wldrd     wr3,[r0,#8]
	add	      r0,r0,r2
	wldrd     wr4,[r0]       @row2 0-7 
	wldrd     wr5,[r0,#8]
	add	      r0,r0,r2
	wldrd     wr6,[r0]       @row3 0-7 
	wldrd     wr7,[r0,#8]
	add	      r0,r0,r2
	wldrd     wr8,[r0]       @row4 0-7 
	wldrd     wr9,[r0,#8]
	
	walignr1  wr0,wr0,wr1    @row0 0-7 
	wslldg    wr1,wr0,wcgr2
	wsrldg    wr0,wr1,wcgr2
	walignr1  wr2,wr2,wr3    @row1 0-7 
	wslldg    wr3,wr2,wcgr2
	wsrldg    wr2,wr3,wcgr2
	walignr1  wr4,wr4,wr5    @row2 0-7 
	wslldg    wr5,wr4,wcgr2
	wsrldg    wr4,wr5,wcgr2
	walignr1  wr6,wr6,wr7    @row3 0-7 
	wslldg    wr7,wr6,wcgr2
	wsrldg    wr6,wr7,wcgr2
	walignr1  wr8,wr8,wr9    @row4 0-7 
	wslldg    wr9,wr8,wcgr2
	wsrldg    wr8,wr9,wcgr2
	
	wor       wr0,wr0,wr3
	wor       wr1,wr2,wr5
	wor       wr2,wr4,wr7
	wor       wr3,wr6,wr9
	
	wavg2b    wr0,wr0,wr1
	wavg2b    wr2,wr2,wr3
	tmrrc     r4,r5,wr0
	tmrrc     r6,r7,wr2
	
	str		  r4,[r1],r3
	str		  r5,[r1],r3
	str		  r6,[r1],r3
	str		  r7,[r1]
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP

@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4Add_H00V02(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	v filter (1,1) 
@	v0 = (p00 + p10 ) >> 1 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4Add_H00V02:	@PROC
    stmfd      sp!,{r4-r11,lr} 
    
    mov        r5,#32
    tmcr       wcgr2,r5
    
    mov        r10,r1    
    ands       r14,r0,#0x07
AddH00V02Chroma4:
	tmcr      wcgr1,r14
	bic       r0,r0,#7
AddH00V02Chroma4Loop:	
	wldrd     wr0,[r0]       @row0 0-7 
	wldrd     wr1,[r0,#8]
	add	      r0,r0,r2
	wldrd     wr2,[r0]       @row1 0-7 
	wldrd     wr3,[r0,#8]
	add	      r0,r0,r2
	wldrd     wr4,[r0]       @row2 0-7 
	wldrd     wr5,[r0,#8]
	add	      r0,r0,r2
	wldrd     wr6,[r0]       @row3 0-7 
	wldrd     wr7,[r0,#8]
	add	      r0,r0,r2
	wldrd     wr8,[r0]       @row4 0-7 
	wldrd     wr9,[r0,#8]
	
	ldr		  r4,[r10],r3
	ldr		  r5,[r10],r3	
	ldr		  r6,[r10],r3
	ldr		  r7,[r10]		
	
	walignr1  wr0,wr0,wr1    @row0 0-7 
	wslldg    wr1,wr0,wcgr2
	wsrldg    wr0,wr1,wcgr2
	walignr1  wr2,wr2,wr3    @row1 0-7 
	wslldg    wr3,wr2,wcgr2
	wsrldg    wr2,wr3,wcgr2
	walignr1  wr4,wr4,wr5    @row2 0-7 
	wslldg    wr5,wr4,wcgr2
	wsrldg    wr4,wr5,wcgr2
	walignr1  wr6,wr6,wr7    @row3 0-7 
	wslldg    wr7,wr6,wcgr2
	wsrldg    wr6,wr7,wcgr2
	walignr1  wr8,wr8,wr9    @row4 0-7 
	wslldg    wr9,wr8,wcgr2
	wsrldg    wr8,wr9,wcgr2
	
	tmcrr     wr11,r4,r5
	tmcrr     wr12,r6,r7
	
	wor       wr0,wr0,wr3
	wor       wr1,wr2,wr5
	wor       wr2,wr4,wr7
	wor       wr3,wr6,wr9
	
	wavg2b    wr0,wr0,wr1
	wavg2b    wr2,wr2,wr3
	wavg2br   wr0,wr0,wr11
	wavg2br   wr2,wr2,wr12		
	tmrrc     r4,r5,wr0
	tmrrc     r6,r7,wr2
	
	str		  r4,[r1],r3
	str		  r5,[r1],r3
	str		  r6,[r1],r3
	str		  r7,[r1]
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4_H01V02(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (3,1)
@	v filter (1,1)
@	h0 = (3*p00 + p01)@	v0 = (h0 + h1 + 4 ) >> 3   OR  v0 = (3*(p00 + p10) + p01 + p11 + 4) >> 3
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4_H01V02:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov         r4,#4
   	mov		    r5,#3
   	mov         r7,#8
   	
	tbcsth      wr13,r4	
	tbcsth      wr14,r5	    
    tmcr        wcgr2,r5     
    tmcr        wcgr0,r7 
    
    mov	        r11,#2
    ands        r14,r0,#0x07
H01V02Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
	wldrd       wr2,[r0]       @row0 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row0 1-7
	wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3	
    wmulsl      wr4,wr2,wr14  @0 - 3
	waddhss     wr5,wr4,wr3 	
H01V02Chroma4Loop:	
    wldrd       wr0,[r0]       @row1 0-7 
	wldrd       wr1,[r0,#8]
	add	        r0,r0,r2
	wldrd       wr2,[r0]       @row2 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	
	walignr1    wr0,wr0,wr1
	wsrldg      wr1,wr0,wcgr0  @row1 1-7	
    wunpckelub  wr0,wr0
    wunpckelub  wr1,wr1
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row2 1-7	
    wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3
    
	wmulsl      wr6,wr0,wr14   @0 - 3
	waddhss     wr12,wr6,wr1
	waddhss     wr8,wr5,wr12
	waddhss     wr8,wr8,wr13
	wsrahg      wr8,wr8,wcgr2
	
	wmulsl      wr6,wr2,wr14   @0 - 3
	waddhss     wr5,wr6,wr3
	waddhss     wr10,wr12,wr5
	waddhss     wr10,wr10,wr13
	wsrahg      wr10,wr10,wcgr2
	wpackhus    wr8,wr8,wr10
	
	StoreHV2x4      0
    
    subs        r11,r11,#1 
	bgt         H01V02Chroma4Loop
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP

@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4Add_H01V02(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (3,1)
@	v filter (1,1)
@	h0 = (3*p00 + p01)@	v0 = (h0 + h1 + 4 ) >> 3   OR  v0 = (3*(p00 + p10) + p01 + p11 + 4) >> 3
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4Add_H01V02:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov         r4,#4
   	mov		    r5,#3
   	mov         r7,#8
   	
	tbcsth      wr13,r4	
	tbcsth      wr14,r5	    
    tmcr        wcgr2,r5     
    tmcr        wcgr0,r7 
    
    mov         r10,r1
    mov	        r11,#2
    ands        r14,r0,#0x07
AddH01V02Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
	wldrd       wr2,[r0]       @row0 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row0 1-7
	wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3	
    wmulsl      wr4,wr2,wr14  @0 - 3
	waddhss     wr5,wr4,wr3 	
AddH01V02Chroma4Loop:	
    wldrd       wr0,[r0]       @row1 0-7 
	wldrd       wr1,[r0,#8]
	add	        r0,r0,r2
	wldrd       wr2,[r0]       @row2 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	
	ldr		    r4,[r10],r3
	ldr		    r5,[r10],r3
	
	walignr1    wr0,wr0,wr1
	wsrldg      wr1,wr0,wcgr0  @row1 1-7	
    wunpckelub  wr0,wr0
    wunpckelub  wr1,wr1
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row2 1-7	
    wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3
    
    tmcrr       wr15,r4,r5
    
	wmulsl      wr6,wr0,wr14   @0 - 3
	waddhss     wr12,wr6,wr1
	waddhss     wr8,wr5,wr12
	waddhss     wr8,wr8,wr13
	wsrahg      wr8,wr8,wcgr2
	
	wmulsl      wr6,wr2,wr14   @0 - 3
	waddhss     wr5,wr6,wr3
	waddhss     wr10,wr12,wr5
	waddhss     wr10,wr10,wr13
	wsrahg      wr10,wr10,wcgr2
	wpackhus    wr8,wr8,wr10
	
	StoreHV2x4      1
    
    subs        r11,r11,#1 
	bgt         AddH01V02Chroma4Loop
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4_H03V02(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (1,3)
@	v filter (1,1)
@	h0 = (p00 + 3*p01)@	v0 = (h0 + h1 + 1 ) >> 2   OR  v0 = ( (p00 + p10) + 3*(p01 + p11) + 4) >> 3
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4_H03V02:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov         r4,#4
   	mov		    r5,#3
   	mov         r7,#8
   	
	tbcsth      wr13,r4	
	tbcsth      wr14,r5	   
    tmcr        wcgr2,r5    
    tmcr        wcgr0,r7 
    
    mov	        r11,#2
    ands        r14,r0,#0x07
H03V02Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
	wldrd       wr2,[r0]       @row0 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row0 1-7
	wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3	
    wmulsl      wr4,wr3,wr14  @0 - 3
	waddhss     wr5,wr4,wr2 	
H03V02Chroma4Loop:	
    wldrd       wr0,[r0]       @row1 0-7 
	wldrd       wr1,[r0,#8]
	add	        r0,r0,r2
	wldrd       wr2,[r0]       @row2 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	
	walignr1    wr0,wr0,wr1
	wsrldg      wr1,wr0,wcgr0  @row1 1-7	
    wunpckelub  wr0,wr0
    wunpckelub  wr1,wr1
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row2 1-7	
    wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3
    
	wmulsl      wr6,wr1,wr14   @0 - 3
	waddhss     wr12,wr6,wr0
	waddhss     wr8,wr5,wr12
	waddhss     wr8,wr8,wr13
	wsrahg      wr8,wr8,wcgr2
	
	wmulsl      wr6,wr3,wr14   @0 - 3
	waddhss     wr5,wr6,wr2
	waddhss     wr10,wr12,wr5
	waddhss     wr10,wr10,wr13
	wsrahg      wr10,wr10,wcgr2
	wpackhus    wr8,wr8,wr10
	
	StoreHV2x4      0
    
    subs        r11,r11,#1 
	bgt         H03V02Chroma4Loop
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP

@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4Add_H03V02(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (1,3)
@	v filter (1,1)
@	h0 = (p00 + 3*p01)@	v0 = (h0 + h1 + 1 ) >> 2   OR  v0 = ( (p00 + p10) + 3*(p01 + p11) + 4) >> 3
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4Add_H03V02:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
    mov         r4,#4
   	mov		    r5,#3
   	mov         r7,#8
   	
	tbcsth      wr13,r4	
	tbcsth      wr14,r5	   
    tmcr        wcgr2,r5    
    tmcr        wcgr0,r7 
    
    mov         r10,r1
    mov	        r11,#2
    ands        r14,r0,#0x07
AddH03V02Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
	wldrd       wr2,[r0]       @row0 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row0 1-7
	wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3	
    wmulsl      wr4,wr3,wr14  @0 - 3
	waddhss     wr5,wr4,wr2 	
AddH03V02Chroma4Loop:	
    wldrd       wr0,[r0]       @row1 0-7 
	wldrd       wr1,[r0,#8]
	add	        r0,r0,r2
	wldrd       wr2,[r0]       @row2 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	
	ldr		    r4,[r10],r3
	ldr		    r5,[r10],r3
	
	walignr1    wr0,wr0,wr1
	wsrldg      wr1,wr0,wcgr0  @row1 1-7	
    wunpckelub  wr0,wr0
    wunpckelub  wr1,wr1
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row2 1-7	
    wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3
    
    tmcrr       wr15,r4,r5
    
	wmulsl      wr6,wr1,wr14   @0 - 3
	waddhss     wr12,wr6,wr0
	waddhss     wr8,wr5,wr12
	waddhss     wr8,wr8,wr13
	wsrahg      wr8,wr8,wcgr2
	
	wmulsl      wr6,wr3,wr14   @0 - 3
	waddhss     wr5,wr6,wr2
	waddhss     wr10,wr12,wr5
	waddhss     wr10,wr10,wr13
	wsrahg      wr10,wr10,wcgr2
	wpackhus    wr8,wr8,wr10
	
	StoreHV2x4      1
    
    subs        r11,r11,#1 
	bgt         AddH03V02Chroma4Loop
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4_H00V03(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	v filter (1,3) 
@	v0 = (p00 + 3*p10 + 2 ) >> 2 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4_H00V03:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov         r4,#2
   	mov		    r5,#3
   	
	tbcsth      wr13,r4	
	tbcsth      wr14,r5	    
    tmcr        wcgr2,r4 
    
    mov	        r11,#2
    ands        r14,r0,#0x07
H00V03Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
	wldrd       wr0,[r0]       @row0 0-7 
	wldrd       wr1,[r0,#8]
	add	        r0,r0,r2
	walignr1    wr1,wr0,wr1
H00V03Chroma4Loop:
	wldrd       wr0,[r0]       @row1 0-7 
	wldrd       wr2,[r0,#8]
	add	        r0,r0,r2
	wldrd       wr3,[r0]       @row2 0-7 
	wldrd       wr4,[r0,#8]
	add	        r0,r0,r2
	
	walignr1    wr0,wr0,wr2
	walignr1    wr3,wr3,wr4
	
    wunpckelub wr0,wr0
    wunpckelub wr1,wr1
    wunpckelub wr9,wr3
    
    wmulsl     wr8,wr0,wr14  @0 - 3
	waddhss    wr8,wr8,wr1
	waddhss    wr8,wr8,wr13
	wsrahg     wr8,wr8,wcgr2
	
	wmulsl     wr10,wr9,wr14  @0 - 3
	waddhss    wr10,wr10,wr0
	waddhss    wr10,wr10,wr13
	wsrahg     wr10,wr10,wcgr2			
	wpackhus   wr8,wr8,wr10	
	
    StoreHV2x4       0
    wmov        wr1,wr3
    	   
    subs        r11,r11,#1 
	bgt         H00V03Chroma4Loop
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP

@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4Add_H00V03(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	v filter (1,3) 
@	v0 = (p00 + 3*p10 + 2 ) >> 2 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4Add_H00V03:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov         r4,#2
   	mov		    r5,#3
   	
	tbcsth      wr13,r4	
	tbcsth      wr14,r5	    
    tmcr        wcgr2,r4 
    
    mov         r10,r1
    mov	        r11,#2
    ands        r14,r0,#0x07
AddH00V03Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
	wldrd       wr0,[r0]       @row0 0-7 
	wldrd       wr1,[r0,#8]
	add	        r0,r0,r2
	walignr1    wr1,wr0,wr1
AddH00V03Chroma4Loop:	
	wldrd       wr0,[r0]       @row1 0-7 
	wldrd       wr2,[r0,#8]
	add	        r0,r0,r2
	wldrd       wr3,[r0]       @row2 0-7 
	wldrd       wr4,[r0,#8]
	add	        r0,r0,r2
	
	ldr		    r4,[r10],r3
	ldr		    r5,[r10],r3
	
	walignr1    wr0,wr0,wr2
	walignr1    wr3,wr3,wr4
	
    wunpckelub wr0,wr0
    wunpckelub wr1,wr1
    wunpckelub wr9,wr3
    
    wmulsl     wr8,wr0,wr14  @0 - 3
	waddhss    wr8,wr8,wr1
	waddhss    wr8,wr8,wr13
	wsrahg     wr8,wr8,wcgr2
	
	tmcrr      wr15,r4,r5
	
	wmulsl     wr9,wr9,wr14  @0 - 3
	waddhss    wr10,wr9,wr0
	waddhss    wr10,wr10,wr13
	wsrahg     wr10,wr10,wcgr2			
	wpackhus   wr8,wr8,wr10	
	
    StoreHV2x4       1
    wmov        wr1,wr3
    
    subs        r11,r11,#1 
	bgt         AddH00V03Chroma4Loop
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4_H01V03(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (3,1)
@	v filter (1,3)
@	h0 = (3*p00 + p01)@	v0 = (h0 + 3*h1 + 1 ) >> 2   OR  v0 = ( 3*(p00 + p11) + 9*p10 + p01 + 7) >> 4
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4_H01V03:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov         r4,#7
   	mov		    r5,#3
   	mov         r6,#4
   	mov         r7,#8
   	
	tbcsth      wr13,r4	
	tbcsth      wr14,r5	    
    tmcr        wcgr2,r6     
    tmcr        wcgr0,r7 
    
    mov	        r11,#2
    ands        r14,r0,#0x07
H01V03Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
	wldrd       wr2,[r0]       @row0 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row0 1-7
	wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3	
    wmulsl      wr4,wr2,wr14  @0 - 3
	waddhss     wr5,wr4,wr3 	
H01V03Chroma4Loop:	
    wldrd       wr0,[r0]       @row1 0-7 
	wldrd       wr1,[r0,#8]
	add	        r0,r0,r2
    wldrd       wr2,[r0]       @row2 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
		
	walignr1    wr0,wr0,wr1
	wsrldg      wr1,wr0,wcgr0  @row1 1-7	
    wunpckelub  wr0,wr0
    wunpckelub  wr1,wr1
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row2 1-7	
    wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3
    
	wmulsl      wr6,wr0,wr14   @0 - 3
	waddhss     wr12,wr6,wr1
	wmulsl      wr7,wr12,wr14
	waddhss     wr8,wr7,wr5
	waddhss     wr8,wr8,wr13
	wsrahg      wr8,wr8,wcgr2
	
	wmulsl      wr6,wr2,wr14   @0 - 3
	waddhss     wr5,wr6,wr3
	wmulsl      wr7,wr5,wr14
	waddhss     wr10,wr7,wr12
	waddhss     wr10,wr10,wr13
	wsrahg      wr10,wr10,wcgr2
	wpackhus    wr8,wr8,wr10
	
	StoreHV2x4       0
    
    subs        r11,r11,#1 
	bgt         H01V03Chroma4Loop
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP

@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4Add_H01V03(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (3,1)
@	v filter (1,3)
@	h0 = (3*p00 + p01)@	v0 = (h0 + 3*h1 + 1 ) >> 2   OR  v0 = ( 3*(p00 + p11) + 9*p10 + p01 + 7) >> 4
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4Add_H01V03:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov         r4,#7
   	mov		    r5,#3
   	mov         r6,#4
   	mov         r7,#8
   	
	tbcsth      wr13,r4	
	tbcsth      wr14,r5	    
    tmcr        wcgr2,r6     
    tmcr        wcgr0,r7 
    
    mov         r10,r1
    mov	        r11,#2
    ands        r14,r0,#0x07
AddH01V03Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
	wldrd       wr2,[r0]       @row0 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row0 1-7
	wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3	
    wmulsl      wr4,wr2,wr14  @0 - 3
	waddhss     wr5,wr4,wr3 	
AddH01V03Chroma4Loop:	
    wldrd       wr0,[r0]       @row1 0-7 
	wldrd       wr1,[r0,#8]
	add	        r0,r0,r2
    wldrd       wr2,[r0]       @row2 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	
	ldr		    r4,[r10],r3
	ldr		    r5,[r10],r3
		
	walignr1    wr0,wr0,wr1
	wsrldg      wr1,wr0,wcgr0  @row1 1-7	
    wunpckelub  wr0,wr0
    wunpckelub  wr1,wr1
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row2 1-7	
    wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3
    
    tmcrr       wr15,r4,r5
    
	wmulsl      wr6,wr0,wr14   @0 - 3
	waddhss     wr12,wr6,wr1
	wmulsl      wr7,wr12,wr14
	waddhss     wr8,wr7,wr5
	waddhss     wr8,wr8,wr13
	wsrahg      wr8,wr8,wcgr2
	
	wmulsl      wr6,wr2,wr14   @0 - 3
	waddhss     wr5,wr6,wr3
	wmulsl      wr7,wr5,wr14
	waddhss     wr10,wr7,wr12
	waddhss     wr10,wr10,wr13
	wsrahg      wr10,wr10,wcgr2
	wpackhus    wr8,wr8,wr10
	
	StoreHV2x4       1
    
    subs        r11,r11,#1 
	bgt         AddH01V03Chroma4Loop
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4_H02V03(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (1,1)
@	v filter (1,3)
@	h0 = (p00 + p01)@	v0 = (h0 + 3*h1 + 1 ) >> 2   OR  v0 = ( (p00 + p01) + 3*(p10 + p11) + 4) >> 3
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4_H02V03:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov         r4,#4
   	mov		    r5,#3
   	mov         r7,#8
   	
	tbcsth      wr13,r4	
	tbcsth      wr14,r5	    
    tmcr        wcgr2,r5     
    tmcr        wcgr0,r7 
    
    mov	        r11,#2
    ands        r14,r0,#0x07
H02V03Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
	wldrd       wr2,[r0]       @row0 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row0 1-7
	wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3	
	waddhss     wr5,wr2,wr3    @0 - 3	
H02V03Chroma4Loop:	
    wldrd       wr0,[r0]       @row1 0-7 
	wldrd       wr1,[r0,#8]
	add	        r0,r0,r2
	wldrd       wr2,[r0]       @row2 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	
	walignr1    wr0,wr0,wr1
	wsrldg      wr1,wr0,wcgr0  @row1 1-7	
    wunpckelub  wr0,wr0
    wunpckelub  wr1,wr1
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row2 1-7	
    wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3
    
	waddhss     wr12,wr0,wr1
	wmulsl      wr7,wr12,wr14
	waddhss     wr8,wr5,wr7
	waddhss     wr8,wr8,wr13
	wsrahg      wr8,wr8,wcgr2
	
	waddhss     wr5,wr2,wr3
	wmulsl      wr7,wr5,wr14
	waddhss     wr10,wr12,wr7
	waddhss     wr10,wr10,wr13
	wsrahg      wr10,wr10,wcgr2
	wpackhus    wr8,wr8,wr10
	
	StoreHV2x4      0
    
    subs        r11,r11,#1 
	bgt         H02V03Chroma4Loop
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP

@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4Add_H02V03(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (1,1)
@	v filter (1,3)
@	h0 = (p00 + p01)@	v0 = (h0 + 3*h1 + 1 ) >> 2   OR  v0 = ( (p00 + p01) + 3*(p10 + p11) + 4) >> 3
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4Add_H02V03:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov         r4,#4
   	mov		    r5,#3
   	mov         r7,#8
   	
	tbcsth      wr13,r4	
	tbcsth      wr14,r5	    
    tmcr        wcgr2,r5     
    tmcr        wcgr0,r7 
    
    mov         r10,r1
    mov	        r11,#2
    ands        r14,r0,#0x07
AddH02V03Chroma4:
	tmcr        wcgr1,r14
	bic         r0,r0,#7
	wldrd       wr2,[r0]       @row0 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row0 1-7
	wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3	
	waddhss     wr5,wr2,wr3    @0 - 3	
AddH02V03Chroma4Loop:	
    wldrd       wr0,[r0]       @row1 0-7 
	wldrd       wr1,[r0,#8]
	add	        r0,r0,r2
	wldrd       wr2,[r0]       @row2 0-7 
	wldrd       wr3,[r0,#8]
	add	        r0,r0,r2
	
	ldr		    r4,[r10],r3
	ldr		    r5,[r10],r3
	
	walignr1    wr0,wr0,wr1
	wsrldg      wr1,wr0,wcgr0  @row1 1-7	
    wunpckelub  wr0,wr0
    wunpckelub  wr1,wr1
	walignr1    wr2,wr2,wr3
	wsrldg      wr3,wr2,wcgr0  @row2 1-7	
    wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3
    
    tmcrr       wr15,r4,r5
    
	waddhss     wr12,wr0,wr1
	wmulsl      wr7,wr12,wr14
	waddhss     wr8,wr5,wr7
	waddhss     wr8,wr8,wr13
	wsrahg      wr8,wr8,wcgr2
	
	waddhss     wr5,wr2,wr3
	wmulsl      wr7,wr5,wr14
	waddhss     wr10,wr12,wr7
	waddhss     wr10,wr10,wr13
	wsrahg      wr10,wr10,wcgr2
	wpackhus    wr8,wr8,wr10
	
	StoreHV2x4      1
    
    subs        r11,r11,#1 
	bgt         AddH02V03Chroma4Loop
		
	ldmfd       sp!,{r4-r11,pc}	
    @ENDP


