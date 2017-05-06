@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2010		            *
@								 	                                    *
@***********************************************************************/

	@AREA	|.text|, CODE
	.section .text
	
	.global	WMMX2_InterpolateDirectLuma
	.global	WMMX2_InterpolateDirectChroma
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	void WMMX2_InterpolateDirectLuma(U8 *pPrev,U8 *pFutr,I32 uPitch,U8 *dst, U32 uDstPitch, I32 iRatio0, I32 iRatio1)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
WMMX2_InterpolateDirectLuma:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
    ldr	        r4,[sp,#36]  @uDstPitch
    ldr	        r5,[sp,#40]  @iRatio0
    ldr	        r6,[sp,#44]  @iRatio1
    mov         r8,#4
    mov         r9,#2
    mov         r10,#4
    
    ands        r11,r0,#0x07
    ands        r14,r1,#0x07
    
    tmcr        wcgr2,r11
	bic         r0,r0,#7
	tmcr        wcgr3,r14
	bic         r1,r1,#7
    
    tbcsth      wr14,r5
    tbcsth      wr15,r6
    tbcsth      wr13,r9     @2
    tmcr        wcgr0,r8
    tmcr        wcgr1,r9    @2
InterpolateDirectLumaLoop:   
    wldrd       wr0,[r0]    @pPrev row0 0-8
    wldrd       wr1,[r0,#8] 
    add         r0, r0, r2
    wldrd       wr2,[r0]    @pPrev row1 0-8
    wldrd       wr3,[r0,#8]
    add         r0, r0, r2   
    wldrd       wr4,[r1]    @pFutr row0 0-8
    wldrd       wr5,[r1,#8] 
    add         r1, r1, r2
    wldrd       wr6,[r1]    @pFutr row1 0-8
    wldrd       wr7,[r1,#8]
    add         r1, r1, r2
    
    walignr2    wr0,wr0,wr1
	walignr2    wr2,wr2,wr3
	walignr3    wr4,wr4,wr5
	walignr3    wr6,wr6,wr7
			
    
    wunpckehub  wr1,wr0
    wunpckelub  wr0,wr0
    wunpckehub  wr3,wr2
    wunpckelub  wr2,wr2
    wunpckehub  wr5,wr4
    wunpckelub  wr4,wr4
    wunpckehub  wr7,wr6
    wunpckelub  wr6,wr6 
    
    wsllhg      wr0,wr0,wcgr0 
    wsllhg      wr1,wr1,wcgr0 
    wsllhg      wr2,wr2,wcgr0 
    wsllhg      wr3,wr3,wcgr0 
    wsllhg      wr4,wr4,wcgr0 
    wsllhg      wr5,wr5,wcgr0 
    wsllhg      wr6,wr6,wcgr0 
    wsllhg      wr7,wr7,wcgr0 
    
    wmulsm      wr0,wr0,wr15
    wmulsm      wr1,wr1,wr15   
    wmulsm      wr2,wr2,wr15
    wmulsm      wr3,wr3,wr15 
    wmulsm      wr4,wr4,wr14
    wmulsm      wr5,wr5,wr14   
    wmulsm      wr6,wr6,wr14
    wmulsm      wr7,wr7,wr14
    
    waddhss    wr0,wr0,wr4
    waddhss    wr1,wr1,wr5
    waddhss    wr2,wr2,wr6
    waddhss    wr3,wr3,wr7
    
    waddhss    wr0,wr0,wr13
    waddhss    wr1,wr1,wr13
    waddhss    wr2,wr2,wr13
    waddhss    wr3,wr3,wr13
    
    wsrahg      wr0,wr0,wcgr1
    wsrahg      wr1,wr1,wcgr1
    wsrahg      wr2,wr2,wcgr1
    wsrahg      wr3,wr3,wcgr1
    
    wpackhus    wr0,wr0,wr1
    wpackhus    wr2,wr2,wr3
    
    wstrd      wr0, [r3]
    add        r3,r3,r4
    wstrd      wr2, [r3]
    add        r3,r3,r4
    
    subs       r10,r10,#1 
	bgt        InterpolateDirectLumaLoop
       
    ldmfd       sp!,{r4-r11,pc}	
    @ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	void WMMX2_InterpolateDirectChroma(U8 *pPrev,U8 *pFutr,I32 uPitch,U8 *dst, U32 uDstPitch, I32 iRatio0, I32 iRatio1)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
WMMX2_InterpolateDirectChroma:	@PROC
    stmfd       sp!,{r4-r11,lr}
    
    ldr	        r5,[sp,#40]  @iRatio0
    ldr	        r6,[sp,#44]  @iRatio1
    ldr	        r4,[sp,#36]  @uDstPitch
    mov         r8,#4
    mov         r9,#2
    
    ands        r11,r0,#0x07
    ands        r14,r1,#0x07
    
    tmcr        wcgr2,r11
	bic         r0,r0,#7
	tmcr        wcgr3,r14
	bic         r1,r1,#7
    
    tbcsth      wr14,r5
    tbcsth      wr15,r6
    tbcsth      wr13,r9     @2
    tmcr        wcgr0,r8
    tmcr        wcgr1,r9    @2
InterpolateDirectChromaLoop: 
    wldrd       wr0,[r0]    @pPrev row0 0-8
    wldrd       wr1,[r0,#8] 
    add         r0, r0, r2
    wldrd       wr2,[r0]    @pPrev row1 0-8
    wldrd       wr3,[r0,#8]
    add         r0, r0, r2 
    wldrd       wr4,[r1]    @pFutr row0 0-8
    wldrd       wr5,[r1,#8] 
    add         r1, r1, r2
    wldrd       wr6,[r1]    @pFutr row1 0-8
    wldrd       wr7,[r1,#8]
    add         r1, r1, r2  
    
    walignr2    wr0,wr0,wr1
	walignr2    wr1,wr2,wr3
	walignr3    wr4,wr4,wr5
	walignr3    wr5,wr6,wr7

    
    wldrd       wr2,[r0]    @pPrev row2 0-8
    wldrd       wr3,[r0,#8] 
    add         r0, r0, r2
    wldrd       wr8,[r0]    @pPrev row3 0-8
    wldrd       wr9,[r0,#8]
    wldrd       wr6,[r1]    @pFutr row2 0-8
    wldrd       wr7,[r1,#8] 
    add         r1, r1, r2
    wldrd       wr10,[r1]    @pFutr row3 0-8
    wldrd       wr11,[r1,#8]
    
    walignr2    wr2,wr2,wr3
	walignr2    wr3,wr8,wr9
	walignr3    wr6,wr6,wr7
	walignr3    wr7,wr10,wr11
    
    wunpckelub  wr0,wr0
    wunpckelub  wr1,wr1
    wunpckelub  wr2,wr2
    wunpckelub  wr3,wr3
    wunpckelub  wr4,wr4
    wunpckelub  wr5,wr5
    wunpckelub  wr6,wr6
    wunpckelub  wr7,wr7 
    
    wsllhg      wr0,wr0,wcgr0 
    wsllhg      wr1,wr1,wcgr0 
    wsllhg      wr2,wr2,wcgr0 
    wsllhg      wr3,wr3,wcgr0 
    wsllhg      wr4,wr4,wcgr0 
    wsllhg      wr5,wr5,wcgr0 
    wsllhg      wr6,wr6,wcgr0 
    wsllhg      wr7,wr7,wcgr0 
    
    wmulsm      wr0,wr0,wr15
    wmulsm      wr1,wr1,wr15   
    wmulsm      wr2,wr2,wr15
    wmulsm      wr3,wr3,wr15 
    wmulsm      wr4,wr4,wr14
    wmulsm      wr5,wr5,wr14   
    wmulsm      wr6,wr6,wr14
    wmulsm      wr7,wr7,wr14
    
    waddhss    wr0,wr0,wr4
    waddhss    wr1,wr1,wr5
    waddhss    wr2,wr2,wr6
    waddhss    wr3,wr3,wr7
    
    waddhss    wr0,wr0,wr13
    waddhss    wr1,wr1,wr13
    waddhss    wr2,wr2,wr13
    waddhss    wr3,wr3,wr13
    
    wsrahg      wr0,wr0,wcgr1
    wsrahg      wr1,wr1,wcgr1
    wsrahg      wr2,wr2,wcgr1
    wsrahg      wr3,wr3,wcgr1
    
    wpackhus    wr0,wr0,wr1
    wpackhus    wr2,wr2,wr3
    
    tmrrc       r7,r8,wr0
	tmrrc       r9,r10,wr2
	
	str		    r7, [r3],r4
	str		    r8, [r3],r4	 
	str		    r9, [r3],r4
	str		    r10,[r3] 
       
    ldmfd       sp!,{r4-r11,pc}	
    @ENDP
    
    @END
    
