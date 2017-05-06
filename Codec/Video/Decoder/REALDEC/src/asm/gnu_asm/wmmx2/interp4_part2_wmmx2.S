@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2010		            *
@								 	                                    *
@***********************************************************************/

	@AREA	|.text|, CODE
	.section .text
	
	.global	WMMX2_Interpolate4_H03V03
	.global	WMMX2_Interpolate4Add_H03V03
	.global	WMMX2_MCCopyChroma4_H02V02
	.global  WMMX2_MCCopyChroma4Add_H02V02
	
@R0 = pSrc
@R1 = pDst
@R2 = uSrcPitch
@R3 = uDstPitch

	.macro	PrepareData 
	  add        r5,r0,#1   
    and        r14,r0,#0x07
		tmcr       wcgr0,r14
		bic        r0,r0,#7
		mov        r8,#48
		mov        r9,#56			
				
		and        r14,r5,#0x07
		tmcr       wcgr1,r14
		bic        r5,r5,#7
		tmcr       wcgr2,r8      @48
		tmcr       wcgr3,r9      @56
		
		wldrd      wr0,[r0]      @row0 0-7 
		wldrd      wr1,[r0,#8]
		wldrd      wr2,[r5]      @row0 1-8 
		wldrd      wr3,[r5,#8]
		
		add        r4,r0,r2
		add        r6,r5,r2
		mov	       r10,#4	
		
		walignr0   wr0,wr0,wr1
		walignr1   wr2,wr2,wr3
	.endm

	.macro	GetSrc2x8_0
		wldrd      wr4,[r4]      @row1 0-7 
		wldrd      wr5,[r4,#8]
		add	       r4,r4,r2
		wldrd      wr6,[r6]      @row1 1-8 
		wldrd      wr7,[r6,#8]
		add	       r6,r6,r2
				
		walignr0   wr4,wr4,wr5			
		walignr1   wr6,wr6,wr7	
	.endm
	
	.macro	GetSrc2x8_1		
		wldrd      wr0,[r4]      @row1 0-7 
		wldrd      wr1,[r4,#8]
		add	       r4,r4,r2
		wldrd      wr2,[r6]      @row1 1-8 
		wldrd      wr3,[r6,#8]
		add	       r6,r6,r2
		
		walignr0   wr0,wr0,wr1			
		walignr1   wr2,wr2,wr3	
	.endm

@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4_H03V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	t0 = p0 + p1
@   v0 = (t0 + t1 + 2) >> 2
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4_H03V03:	@PROC
    stmfd      sp!,{r4-r11,lr}
    
    PrepareData
   
H03V03MainLoop:
    GetSrc2x8_0	
	wavg4r    wr8,wr0,wr4
	wavg4r    wr9,wr2,wr6
	wsrldg    wr9,wr9,wcgr2     @#48
	wslldg    wr9,wr9,wcgr3     @#56
	wor       wr8,wr8,wr9
	wstrd     wr8,[r1]
	add       r1,r1,r3
	
	GetSrc2x8_1
	wavg4r    wr8,wr4,wr0
	wavg4r    wr9,wr6,wr2
	wsrldg    wr9,wr9,wcgr2     @#48
	wslldg    wr9,wr9,wcgr3     @#56
	wor       wr8,wr8,wr9
	wstrd     wr8,[r1]
	add       r1,r1,r3	
	
	subs      r10,r10,#1 
	bgt       H03V03MainLoop	
	   
    ldmfd     sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4Add_H03V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	t0 = p0 + p1
@   v0 = (t0 + t1 + 2) >> 2
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4Add_H03V03:	@PROC
    stmfd      sp!,{r4-r11,lr}
   
	PrepareData
    
AddH03V03MainLoop:
    GetSrc2x8_0
    wldrd     wr10,[r1]	
	wavg4r    wr8,wr0,wr4
	wavg4r    wr9,wr2,wr6
	wsrldg    wr9,wr9,wcgr2     @#48
	wslldg    wr9,wr9,wcgr3     @#56
	wor       wr8,wr8,wr9
	wavg2br   wr8,wr8,wr10 
	wstrd     wr8,[r1]
	add       r1,r1,r3
	
	GetSrc2x8_1
	wldrd     wr10,[r1]
	wavg4r    wr8,wr4,wr0
	wavg4r    wr9,wr6,wr2
	wsrldg    wr9,wr9,wcgr2     @#48
	wslldg    wr9,wr9,wcgr3     @#56
	wor       wr8,wr8,wr9
	wavg2br   wr8,wr8,wr10 
	wstrd     wr8,[r1]
	add       r1,r1,r3
	
	subs       r10,r10,#1 
	bgt        AddH03V03MainLoop	
	   
    ldmfd      sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@   void RV_FASTCALL C_MCCopyChroma4_H02V02(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@   f00 = (p00 + p01 + p10 + p11 + 1)>>2
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4_H02V02:	@PROC
    stmfd      sp!,{r4-r11,lr}
    	   
	and        r14,r0,#0x07
	tmcr       wcgr0,r14
	bic        r0,r0,#7
H03V03Chroma4MainLoop:
	wldrd      wr0,[r0]      @row0 0-7 
	wldrd      wr1,[r0,#8]
	add	       r0,r0,r2
	wldrd      wr2,[r0]      @row1 0-7 
	wldrd      wr3,[r0,#8]
	add	       r0,r0,r2
	wldrd      wr4,[r0]      @row2 0-7 
	wldrd      wr5,[r0,#8]
	add	       r0,r0,r2
	wldrd      wr6,[r0]      @row3 0-7 
	wldrd      wr7,[r0,#8]
	add	       r0,r0,r2
	wldrd      wr8,[r0]      @row3 0-7 
	wldrd      wr9,[r0,#8]

	walignr0   wr0,wr0,wr1	
	walignr0   wr2,wr2,wr3
	walignr0   wr4,wr4,wr5
	walignr0   wr6,wr6,wr7
	walignr0   wr8,wr8,wr9
	
	wavg4      wr0,wr0,wr2
	wavg4      wr1,wr2,wr4
	wavg4      wr2,wr4,wr6
	wavg4      wr3,wr6,wr8
	
	wslld      wr0,wr0,#32
	wslld      wr2,wr2,#32
	
	waligni    wr0,wr0,wr1,#4
	waligni    wr2,wr2,wr3,#4
	
	tmrrc      r4,r5,wr0
	tmrrc      r6,r7,wr2
	
	str		   r4,[r1],r3
	str		   r5,[r1],r3
	str		   r6,[r1],r3
	str		   r7,[r1]
    
    ldmfd      sp!,{r4-r11,pc}	
    @ENDP

@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_MCCopyChroma4Add_H02V02(const U8 *pRef, U8 *pDst, U32 uPitch, U32 uDstPitch)
@
@   f00 = (p00 + p01 + p10 + p11 + 1)>>2
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_MCCopyChroma4Add_H02V02:	@PROC
    stmfd      sp!,{r4-r11,lr}
    
	and        r14,r0,#0x07
	mov        r10,r1
	tmcr       wcgr0,r14
	bic        r0,r0,#7
AddH03V03Chroma4MainLoop:
	wldrd      wr0,[r0]      @row0 0-7 
	wldrd      wr1,[r0,#8]
	add	       r0,r0,r2
	wldrd      wr2,[r0]      @row1 0-7 
	wldrd      wr3,[r0,#8]
	add	       r0,r0,r2
	wldrd      wr4,[r0]      @row2 0-7 
	wldrd      wr5,[r0,#8]
	add	       r0,r0,r2
	wldrd      wr6,[r0]      @row3 0-7 
	wldrd      wr7,[r0,#8]
	add	       r0,r0,r2
	wldrd      wr8,[r0]      @row3 0-7 
	wldrd      wr9,[r0,#8]
	
	ldr		   r4,[r10],r3
	ldr		   r5,[r10],r3
	ldr		   r6,[r10],r3
	ldr		   r7,[r10]

	walignr0   wr0,wr0,wr1	
	walignr0   wr2,wr2,wr3
	walignr0   wr4,wr4,wr5
	walignr0   wr6,wr6,wr7
	walignr0   wr8,wr8,wr9
	
	wavg4      wr0,wr0,wr2
	wavg4      wr1,wr2,wr4
	wavg4      wr2,wr4,wr6
	wavg4      wr3,wr6,wr8
	
	tmcrr      wr14,r4,r5
	tmcrr      wr15,r6,r7
	
	wslld      wr0,wr0,#32
	wslld      wr2,wr2,#32
	
	waligni    wr0,wr0,wr1,#4
	waligni    wr2,wr2,wr3,#4
	
	wavg2br    wr0,wr0,wr14 
	wavg2br    wr2,wr2,wr15 
	
	tmrrc      r4,r5,wr0
	tmrrc      r6,r7,wr2
	
	str		   r4,[r1],r3
	str		   r5,[r1],r3
	str		   r6,[r1],r3
	str		   r7,[r1]
    
    ldmfd      sp!,{r4-r11,pc}	
    @ENDP

@END


