@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2010		            *
@								 	                                    *
@***********************************************************************/

	@AREA	|.text|, CODE
	.section .text
	
	.global	WMMX2_Interpolate4_H00V00
	.global	WMMX2_Interpolate4_H01V00
	.global	WMMX2_Interpolate4_H02V00
	.global	WMMX2_Interpolate4_H03V00
	.global	WMMX2_Interpolate4_H00V01
	.global	WMMX2_Interpolate4_H01V01
	.global	WMMX2_Interpolate4_H02V01
	.global  WMMX2_Interpolate4_H03V01
	.global	WMMX2_Interpolate4_H00V02
	.global	WMMX2_Interpolate4_H01V02
	.global	WMMX2_Interpolate4_H02V02
	.global	WMMX2_Interpolate4_H03V02
	.global	WMMX2_Interpolate4_H00V03
	.global	WMMX2_Interpolate4_H01V03
	.global	WMMX2_Interpolate4_H02V03
	@.global	WMMX2_Interpolate4_H03V03
	
	.global	WMMX2_Interpolate4Add_H00V00
	.global	WMMX2_Interpolate4Add_H01V00
	.global	WMMX2_Interpolate4Add_H02V00
	.global	WMMX2_Interpolate4Add_H03V00
	.global	WMMX2_Interpolate4Add_H00V01
	.global	WMMX2_Interpolate4Add_H01V01
	.global	WMMX2_Interpolate4Add_H02V01
	.global  WMMX2_Interpolate4Add_H03V01
	.global	WMMX2_Interpolate4Add_H00V02
	.global	WMMX2_Interpolate4Add_H01V02
	.global	WMMX2_Interpolate4Add_H02V02
	.global	WMMX2_Interpolate4Add_H03V02
	.global	WMMX2_Interpolate4Add_H00V03
	.global	WMMX2_Interpolate4Add_H01V03
	.global	WMMX2_Interpolate4Add_H02V03
	@.global	WMMX2_Interpolate4Add_H03V03
	
@R0 = pSrc
@R1 = pDst
@R2 = uSrcPitch
@R3 = uDstPitch

	.macro	GetSrc1x13
		wldrd      wr0,[r0]		
		wldrd      wr1,[r0,#8]		
	    wldrd      wr11,[r0,#16]	    		    		
		add        r0,r0,r2    
		     
		walignr1   wr11,wr1,wr11       @row0 8-15	
		walignr1   wr0,wr0,wr1		   @row0 0-7
		pld		   [r0]
	
	.endm
	
	.macro	Interpolate4Horiz1x8
        waligni    wr1,wr0,wr11,#1
        waligni    wr2,wr0,wr11,#2
        waligni    wr3,wr0,wr11,#3
        waligni    wr4,wr0,wr11,#4
        waligni    wr5,wr0,wr11,#5
        
        wunpckelub wr6,wr0
        wunpckelub wr7,wr1
        wunpckelub wr8,wr2
        wunpckelub wr9,wr3
        wunpckelub wr10,wr4
        wunpckelub wr11,wr5
        
        wmulsl     wr8,wr8,wr14         @0 - 3
        waddhss    wr6,wr6,wr11        
		waddhss    wr7,wr7,wr10
		wmulsl     wr9,wr9,wr15
		wsllhg     wr12,wr7,wcgr0
		waddhss    wr7,wr7,wr12		

		waddhss    wr12,wr6,wr8
		waddhss    wr12,wr12,wr9
		wsubhss    wr12,wr12,wr7
		waddhss    wr12,wr12,wr13
		wsrahg     wr12,wr12,wcgr2
		       
        wunpckehub wr6,wr2
        wunpckehub wr7,wr3
        wunpckehub wr8,wr4
        wunpckehub wr9,wr5
  
        wmulsl     wr6,wr6,wr14        @4 - 7
        waddhss    wr9,wr10,wr9        
		waddhss    wr8,wr11,wr8
		wmulsl     wr7,wr7,wr15
		wsllhg     wr0,wr8,wcgr0
		waddhss    wr8,wr8,wr0		

		waddhss    wr11,wr9,wr6
		waddhss    wr11,wr11,wr7
		wsubhss    wr11,wr11,wr8
		waddhss    wr11,wr11,wr13
		wsrahg     wr11,wr11,wcgr2	 
	.endm
	
	.macro	StoreHoriz1x8    AddFlag 	
		wpackhus   wr11,wr12,wr11
		
		.if \AddFlag > 0
			wldrd     wr5,[r1]
			wavg2br   wr11,wr11,wr5 
		.endif
		
		wstrd      wr11,[r1]
		add        r1,r1,r3		 
	.endm
	
	.macro	GetSrc6x8
	    wldrd     wr1,[r0,#8]   @row0 0-7 
		wldrd     wr0,[r0]
		add       r0,r0,r2
		wldrd     wr3,[r0,#8]   @row1 0-7     
		wldrd     wr2,[r0]
		add       r0,r0,r2 
		wldrd     wr5,[r0,#8]   @row2 0-7 
		wldrd     wr4,[r0] 
		add       r0,r0,r2 
		wldrd     wr7,[r0,#8]   @row3 0-7 
		wldrd     wr6,[r0] 
		add       r0,r0,r2
		wldrd     wr9,[r0,#8]   @row4 0-7 
		wldrd     wr8,[r0]
		add       r0,r0,r2
		wldrd     wr11,[r0,#8]  @row5 0-7 
		wldrd     wr10,[r0]
		add       r0,r0,r2
		
		pld		   [r0]	
		
		walignr1  wr0,wr0,wr1		
		walignr1  wr1,wr2,wr3	
		walignr1  wr2,wr4,wr5		
		walignr1  wr3,wr6,wr7	
		walignr1  wr4,wr8,wr9		
		walignr1  wr5,wr10,wr11				
	.endm
	
	.macro	UnpckeSrcLow6x4
	    wunpckelub wr6,wr0
        wunpckelub wr7,wr1
        wunpckelub wr8,wr2
        wunpckelub wr9,wr3
        wunpckelub wr10,wr4
        wunpckelub wr11,wr5
	.endm
	
	.macro	UnpckeSrcHigh6x4
	    wunpckehub wr6,wr0
        wunpckehub wr7,wr1
        wunpckehub wr8,wr2
        wunpckehub wr9,wr3
        wunpckehub wr10,wr4
        wunpckehub wr11,wr5
	.endm
	
	.macro	Interpolate4Vert1x4Row
        waddhss    wr6,wr6,wr11        @0 - 3
		waddhss    wr7,wr7,wr10
		wsllhg     wr11,wr7,wcgr0
		waddhss    wr7,wr11,wr7
		wmulsl     wr8,wr8,wr14
		wmulsl     wr9,wr9,wr15

		waddhss    wr11,wr6,wr8
		waddhss    wr11,wr11,wr9
		wsubhss    wr11,wr11,wr7
		waddhss    wr11,wr11,wr13
		wsrahg     wr11,wr11,wcgr2
	.endm
	
	.macro	StoreVert1x8    AddFlag 	
		wpackhus   wr11,wr12,wr11
		
		.if \AddFlag > 0
			wavg2br   wr11,wr11,wr0 
		.endif
		
		wstrd       wr11,[r1]
		add         r1,r1,r3				 
	.endm
	
	.macro	Interpolate4VertHoriz1x8
        waligni    wr1,wr0,wr11,#1
        waligni    wr2,wr0,wr11,#2
        waligni    wr3,wr0,wr11,#3
        waligni    wr4,wr0,wr11,#4
        waligni    wr5,wr0,wr11,#5
        
        wunpckelub wr6,wr0
        wunpckelub wr7,wr1
        wunpckelub wr8,wr2
        wunpckelub wr9,wr3
        wunpckelub wr10,wr4
        wunpckelub wr11,wr5
        
        wmulsl     wr8,wr8,wr14         @0 - 3
        waddhss    wr6,wr6,wr11        
		waddhss    wr7,wr7,wr10
		wmulsl     wr9,wr9,wr15
		wsllhg     wr12,wr7,wcgr0
		waddhss    wr7,wr7,wr12		

		waddhss    wr12,wr6,wr8
		waddhss    wr12,wr12,wr9
		wsubhss    wr12,wr12,wr7
		waddhss    wr12,wr12,wr13
		wsrahg     wr12,wr12,wcgr2
		       
        wunpckehub wr6,wr2
        wunpckehub wr7,wr3
        wunpckehub wr8,wr4
        wunpckehub wr9,wr5
  
        wmulsl     wr6,wr6,wr14        @4 - 7
        waddhss    wr9,wr10,wr9        
		waddhss    wr8,wr11,wr8
		wmulsl     wr7,wr7,wr15
		wsllhg     wr0,wr8,wcgr0
		waddhss    wr8,wr8,wr0		

		waddhss    wr11,wr9,wr6
		waddhss    wr11,wr11,wr7
		wsubhss    wr11,wr11,wr8
		waddhss    wr11,wr11,wr13
		wsrahg     wr11,wr11,wcgr2	 
		
		wpackhus   wr11,wr12,wr11		
		wstrd      wr11,[r12],#8	 
	.endm
	
	.macro	GetVertSrc6x8
		wldrd      wr0,[r12],#8       @row0 0-7 
		wldrd      wr1,[r12],#8       @row1 0-7
		wldrd      wr2,[r12],#8       @row2 0-7 
		wldrd      wr3,[r12],#8       @row3 0-7
		wldrd      wr4,[r12],#8       @row4 0-7 
		wldrd      wr5,[r12],#8       @row5 0-7	
	.endm
	
@-----------------------------------------------------------------------------------------------------
@void RV_FASTCALL C_Interpolate4_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4_H00V00:	@PROC
    stmdb      sp!,{r4-r11,lr}
	ands       r14,r0,#0x07
	tmcr       wcgr1,r14
	bne		  H00V00_notAligned8	
H00V00_Aligned8:
	wldrd     wr0,[r0]        @0
	add       r0,r0,r2
	wldrd     wr1,[r0]        @1
	add       r0,r0,r2
	wldrd     wr2,[r0]        @2
	add       r0,r0,r2
	wldrd     wr3,[r0]        @3
	add       r0,r0,r2
	wldrd     wr4,[r0]        @4
	add       r0,r0,r2
	wldrd     wr5,[r0]        @5
	add       r0,r0,r2
	wldrd     wr6,[r0]        @6
	add       r0,r0,r2
	wldrd     wr7,[r0]        @7
	
	wstrd     wr0,[r1]
	add       r1,r1,r3
	wstrd     wr1,[r1]
	add       r1,r1,r3
	wstrd     wr2,[r1]
	add       r1,r1,r3
	wstrd     wr3,[r1]
	add       r1,r1,r3
	wstrd     wr4,[r1]
	add       r1,r1,r3
	wstrd     wr5,[r1]
	add       r1,r1,r3
	wstrd     wr6,[r1]
	add       r1,r1,r3
	wstrd     wr7,[r1]
	
    ldmia     sp!,{r4-r11,pc}

H00V00_notAligned8:
    bic       r0,r0,#7
   
    wldrd     wr1,[r0,#8] 
	wldrd     wr0,[r0]          @0
	add       r0,r0,r2
	wldrd     wr3,[r0,#8]	
	wldrd     wr2,[r0]          @1
	add       r0,r0,r2
	wldrd     wr5,[r0,#8]
	wldrd     wr4,[r0]          @2
	add       r0,r0,r2
	wldrd     wr7,[r0,#8]
	wldrd     wr6,[r0]          @3
	add       r0,r0,r2
	wldrd     wr9,[r0,#8]
	wldrd     wr8,[r0]          @4
	add       r0,r0,r2
	wldrd     wr11,[r0,#8]	
	wldrd     wr10,[r0]         @5
	add       r0,r0,r2
	wldrd     wr13,[r0,#8]
	wldrd     wr12,[r0]         @6
	add       r0,r0,r2
	wldrd     wr15,[r0,#8]
	wldrd     wr14,[r0]         @7
		
	walignr1  wr0,wr0,wr1       @0
	wstrd     wr0,[r1]
	add       r1,r1,r3
	walignr1  wr2,wr2,wr3       @1
	wstrd     wr2,[r1]
	add       r1,r1,r3         
	walignr1  wr4,wr4,wr5       @2
	wstrd     wr4,[r1]
	add       r1,r1,r3
	walignr1  wr6,wr6,wr7       @3
	wstrd     wr6,[r1]
	add       r1,r1,r3
	walignr1  wr8,wr8,wr9       @4
	wstrd     wr8,[r1]
	add       r1,r1,r3
	walignr1  wr10,wr10,wr11    @5
	wstrd     wr10,[r1]
	add       r1,r1,r3
	walignr1  wr12,wr12,wr13    @6
	wstrd     wr12,[r1]
	add       r1,r1,r3
	walignr1  wr14,wr14,wr15    @7
	wstrd     wr14,[r1]
	
    ldmia     sp!,{r4-r11,pc}
	@ENDP

@-----------------------------------------------------------------------------------------------------
@void RV_FASTCALL C_Interpolate4Add_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4Add_H00V00:	@PROC
    stmdb     sp!,{r4-r11,lr}
    
    mov       r6,r1 
	ands      r14,r0,#0x07
	tmcr      wcgr1,r14
	bne		  AddH00V00_notAligned8	
AddH00V00_Aligned8:
	wldrd     wr0,[r0]        @0
	add       r0,r0,r2
	wldrd     wr8,[r6]
	add       r6,r6,r3
	wldrd     wr1,[r0]        @1
	add       r0,r0,r2
	wldrd     wr9,[r6]
	add       r6,r6,r3
	wldrd     wr2,[r0]        @2
	add       r0,r0,r2
	wldrd     wr10,[r6]
	add       r6,r6,r3
	wldrd     wr3,[r0]        @3
	add       r0,r0,r2
	wldrd     wr11,[r6]
	add       r6,r6,r3
	wldrd     wr4,[r0]        @4
	add       r0,r0,r2
	wldrd     wr12,[r6]
	add       r6,r6,r3
	wldrd     wr5,[r0]        @5
	add       r0,r0,r2
	wldrd     wr13,[r6]
	add       r6,r6,r3
	wldrd     wr6,[r0]        @6
	add       r0,r0,r2
	wldrd     wr14,[r6]
	add       r6,r6,r3
	wldrd     wr7,[r0]        @7
	wldrd     wr15,[r6]
	
	wavg2br   wr0,wr0,wr8
	wstrd     wr0,[r1]
	add       r1,r1,r3
	wavg2br   wr1,wr1,wr9
	wstrd     wr1,[r1]
	add       r1,r1,r3
	wavg2br   wr2,wr2,wr10
	wstrd     wr2,[r1]
	add       r1,r1,r3
	wavg2br   wr3,wr3,wr11
	wstrd     wr3,[r1]
	add       r1,r1,r3
	wavg2br   wr4,wr4,wr12
	wstrd     wr4,[r1]
	add       r1,r1,r3
	wavg2br   wr5,wr5,wr13
	wstrd     wr5,[r1]
	add       r1,r1,r3
	wavg2br   wr6,wr6,wr14
	wstrd     wr6,[r1]
	add       r1,r1,r3
	wavg2br   wr7,wr7,wr15
	wstrd     wr7,[r1]
	
    ldmia     sp!,{r4-r11,pc}

AddH00V00_notAligned8:
    bic       r0,r0,#7
   
    wldrd     wr1,[r0,#8] 
	wldrd     wr0,[r0]          @0
	add       r0,r0,r2
	wldrd     wr8,[r6]
	add       r6,r6,r3
	wldrd     wr3,[r0,#8]	
	wldrd     wr2,[r0]          @1
	add       r0,r0,r2
	wldrd     wr9,[r6]
	add       r6,r6,r3
	wldrd     wr5,[r0,#8]
	wldrd     wr4,[r0]          @2
	add       r0,r0,r2
	wldrd     wr10,[r6]
	add       r6,r6,r3
	wldrd     wr7,[r0,#8]
	wldrd     wr6,[r0]          @3
	add       r0,r0,r2
	wldrd     wr11,[r6]
	add       r6,r6,r3
	
	walignr1  wr0,wr0,wr1       @0
	wavg2br   wr0,wr0,wr8
	wstrd     wr0,[r1]
	add       r1,r1,r3
	walignr1  wr2,wr2,wr3       @1
	wavg2br   wr2,wr2,wr9
	wstrd     wr2,[r1]
	add       r1,r1,r3         
	walignr1  wr4,wr4,wr5       @2
	wavg2br   wr4,wr4,wr10
	wstrd     wr4,[r1]
	add       r1,r1,r3
	walignr1  wr6,wr6,wr7       @3
	wavg2br   wr6,wr6,wr11
	wstrd     wr6,[r1]
	add       r1,r1,r3
	
    wldrd     wr1,[r0,#8] 
	wldrd     wr0,[r0]          @4
	add       r0,r0,r2
	wldrd     wr8,[r6]
	add       r6,r6,r3
	wldrd     wr3,[r0,#8]	
	wldrd     wr2,[r0]          @5
	add       r0,r0,r2
	wldrd     wr9,[r6]
	add       r6,r6,r3
	wldrd     wr5,[r0,#8]
	wldrd     wr4,[r0]          @6
	add       r0,r0,r2
	wldrd     wr10,[r6]
	add       r6,r6,r3
	wldrd     wr7,[r0,#8]
	wldrd     wr6,[r0]          @7
	wldrd     wr11,[r6]
	
	walignr1  wr0,wr0,wr1       @4
	wavg2br   wr0,wr0,wr8
	wstrd     wr0,[r1]
	add       r1,r1,r3
	walignr1  wr2,wr2,wr3       @5
	wavg2br   wr2,wr2,wr9
	wstrd     wr2,[r1]
	add       r1,r1,r3         
	walignr1  wr4,wr4,wr5       @6
	wavg2br   wr4,wr4,wr10
	wstrd     wr4,[r1]
	add       r1,r1,r3
	walignr1  wr6,wr6,wr7       @7
	wavg2br   wr6,wr6,wr11
	wstrd     wr6,[r1]
	
    ldmia     sp!,{r4-r11,pc}

	@ENDP
	
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,52,20,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4_H01V00:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov        r4,#32
   	mov		   r5,#52
   	mov        r7,#20
   	mov        r8,#2
   	mov        r10,#6
   	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8    
    tmcr       wcgr2,r10 
    
    mov	       r6,#8
    sub		   r0, r0, #2
    ands       r14,r0,#0x07   
H01V00Interpolate4:
	tmcr      wcgr1,r14
	bic       r0,r0,#7
H01V00Interpolate4Loop:	
    GetSrc1x13
    Interpolate4Horiz1x8
    StoreHoriz1x8    0
    subs       r6,r6,#1 
	bgt        H01V00Interpolate4Loop
		
	ldmfd      sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4Add_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,52,20,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4Add_H01V00:	@PROC
    stmfd       sp!,{r4-r11,lr}
    
    mov        r4,#32
   	mov		   r5,#52
   	mov        r7,#20
   	mov        r8,#2
   	mov        r10,#6
   	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8    
    tmcr       wcgr2,r10 
    
    mov	       r6,#8
    sub		   r0, r0, #2
    ands       r14,r0,#0x07
AddH01V00Interpolate4:
	tmcr      wcgr1,r14
	bic       r0,r0,#7
AddH01V00Interpolate4Loop:	
    GetSrc1x13
    Interpolate4Horiz1x8
    StoreHoriz1x8    1
    subs       r6,r6,#1 
	bgt        AddH01V00Interpolate4Loop
		
	ldmfd      sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,20,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4_H02V00:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov        r4,#16
   	mov		   r5,#20
   	mov        r7,#20
   	mov        r8,#2
   	mov        r10,#5
   	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8   
    tmcr       wcgr2,r10 
    
    mov	       r6,#8
    sub		   r0, r0, #2
    ands       r14,r0,#0x07
H02V00Interpolate4:
	tmcr      wcgr1,r14
	bic       r0,r0,#7
H02V00Interpolate4Loop:	
    GetSrc1x13
    Interpolate4Horiz1x8
    StoreHoriz1x8    0
    subs       r6,r6,#1 
	bgt        H02V00Interpolate4Loop
		
	ldmfd      sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4Add_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,20,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4Add_H02V00:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov        r4,#16
   	mov		   r5,#20
   	mov        r7,#20
   	mov        r8,#2
   	mov        r10,#5
   	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8   
    tmcr       wcgr2,r10 
    
    mov	       r6,#8
    sub		   r0, r0, #2
    ands       r14,r0,#0x07
AddH02V00Interpolate4:
	tmcr      wcgr1,r14
	bic       r0,r0,#7
AddH02V00Interpolate4Loop:	
    GetSrc1x13
    Interpolate4Horiz1x8
    StoreHoriz1x8    1
    subs       r6,r6,#1 
	bgt        AddH02V00Interpolate4Loop
		
	ldmfd      sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4_H03V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,52,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4_H03V00:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov        r4,#32
   	mov		   r5,#20
   	mov        r7,#52
   	mov        r8,#2
   	mov        r10,#6
   	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8    
    tmcr       wcgr2,r10 
    
    mov	       r6,#8
    sub		   r0, r0, #2
    ands       r14,r0,#0x07
H03V00Interpolate4:
	tmcr      wcgr1,r14
	bic       r0,r0,#7
H03V00Interpolate4Loop:	
    GetSrc1x13
    Interpolate4Horiz1x8
    StoreHoriz1x8    0
    subs       r6,r6,#1 
	bgt        H03V00Interpolate4Loop
		
	ldmfd      sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4_H03V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,52,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4Add_H03V00:	@PROC
    stmfd       sp!,{r4-r11,lr}
   
   	mov        r4,#32
   	mov		   r5,#20
   	mov        r7,#52
   	mov        r8,#2
   	mov        r10,#6
   	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8    
    tmcr       wcgr2,r10  
    
    mov	       r6,#8
    sub		   r0, r0, #2
    ands       r14,r0,#0x07
AddH03V00Interpolate4:
	tmcr      wcgr1,r14
	bic       r0,r0,#7
AddH03V00Interpolate4Loop:	
    GetSrc1x13
    Interpolate4Horiz1x8
    StoreHoriz1x8    1
    subs       r6,r6,#1 
	bgt        AddH03V00Interpolate4Loop
		
	ldmfd      sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	RV_FASTCALL C_Interpolate4_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	v filter (1,-5,52,20,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4_H00V01:	@PROC
	stmfd      sp!,{r4-r11,lr}
	
	mov        r4,#32
	mov		   r5,#52
	mov        r7,#20
	mov        r8,#2
	mov        r10,#6
	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8    
    tmcr       wcgr2,r10 
   
    mov        r11,#8    
    sub        r0,r0,r2,lsl #1
    ands       r14,r0,#0x07
H00V01Interpolate4:
	tmcr       wcgr1,r14
	bic        r0,r0,#7
	GetSrc6x8  
H00V01Interpolate4Loop:	         
    UnpckeSrcLow6x4
    Interpolate4Vert1x4Row       @row0 0-3 
    wmov       wr12,wr11
    UnpckeSrcHigh6x4
    Interpolate4Vert1x4Row       @row0 4-7
    StoreVert1x8       0 
    
    wldrd      wr7,[r0,#8] 
    wldrd      wr6,[r0]
    add        r0,r0,r2
    pld		   [r0]	
    wmov       wr0,wr1
    wmov       wr1,wr2
    wmov       wr2,wr3
    wmov       wr3,wr4
    wmov       wr4,wr5	
	walignr1   wr5,wr6,wr7         
    
    subs       r11,r11,#1 
	bgt        H00V01Interpolate4Loop	

	ldmfd      sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4Add_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	v filter (1,-5,52,20,-5,1) 
@-----------------------------------------------------------------------------------------------------
 .ALIGN 8
WMMX2_Interpolate4Add_H00V01:	@PROC
	stmfd      sp!,{r4-r11,lr}
	
	mov        r4,#32
	mov		   r5,#52
	mov        r7,#20
	mov        r8,#2
	mov        r10,#6
	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8    
    tmcr       wcgr2,r10 
    
    mov        r11,#8  
    mov        r9,r1    
    sub        r0,r0,r2,lsl #1
    ands       r14,r0,#0x07
AddH00V01Interpolate4:
	tmcr       wcgr1,r14
	bic        r0,r0,#7
	GetSrc6x8  
AddH00V01Interpolate4Loop:	         
    UnpckeSrcLow6x4
    Interpolate4Vert1x4Row       @row0 0-3 
    wmov       wr12,wr11
    UnpckeSrcHigh6x4
    wldrd     wr0,[r9]
    add       r9,r9,r3    
    Interpolate4Vert1x4Row       @row0 4-7
    StoreVert1x8       1 
    
    wldrd      wr7,[r0,#8] 
    wldrd      wr6,[r0]
    add        r0,r0,r2
    pld		   [r0]	
    wmov       wr0,wr1
    wmov       wr1,wr2
    wmov       wr2,wr3
    wmov       wr3,wr4
    wmov       wr4,wr5	
	walignr1   wr5,wr6,wr7         
    
    subs       r11,r11,#1 
	bgt        AddH00V01Interpolate4Loop	

	ldmfd      sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h/v filter (1,-5,52,20,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4_H01V01:	@PROC
    stmfd      sp!,{r4-r11,lr}
    sub        sp,sp,#112            @13*8 + 8
   
   	mov        r4,#32
   	mov		   r5,#52
   	mov        r7,#20
   	mov        r8,#2
   	mov        r10,#6
   	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8   
    tmcr       wcgr2,r10 
    
    mov	       r6,#13
    add        r12,sp,#8
    bic        r12,r12,#7
    sub        r0,r0,r2,lsl #1
    sub		   r0,r0,#2
    ands       r14,r0,#0x07
H01V01HorizInterpolate4:
	tmcr       wcgr1,r14
	bic        r0,r0,#7
H01V01HorizInterpolate4Loop:	
    GetSrc1x13
    Interpolate4VertHoriz1x8
    subs       r6,r6,#1 
	bgt        H01V01HorizInterpolate4Loop
	
	sub        r12,r12,#104
	mov        r11,#8     
    GetVertSrc6x8                 @row0 0-3 
H01V01VertInterpolate4Loop:
    UnpckeSrcLow6x4
    Interpolate4Vert1x4Row       @row0 0-3 
    wmov       wr12,wr11
    UnpckeSrcHigh6x4   
    Interpolate4Vert1x4Row       @row0 4-7
    StoreVert1x8       0 
        
    wmov       wr0,wr1
    wmov       wr1,wr2
    wmov       wr2,wr3
    wmov       wr3,wr4
    wmov       wr4,wr5	
	wldrd      wr5,[r12],#8 
	
	subs       r11,r11,#1 
	bgt        H01V01VertInterpolate4Loop	
    
    add        sp,sp,#112 		
	ldmfd      sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4Add_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h/v filter (1,-5,52,20,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4Add_H01V01:	@PROC
    stmfd      sp!,{r4-r11,lr}
    sub        sp,sp,#112            @13*8 + 8
   
   	mov        r4,#32
   	mov		   r5,#52
   	mov        r7,#20
   	mov        r8,#2
   	mov        r10,#6
   	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8   
    tmcr       wcgr2,r10 
    
    mov	       r6,#13
    add        r12,sp,#8
    bic        r12,r12,#7
    mov        r9,r1 
    sub        r0,r0,r2,lsl #1
    sub		   r0,r0,#2
    ands       r14,r0,#0x07
AddH01V01HorizInterpolate4:
	tmcr       wcgr1,r14
	bic        r0,r0,#7
AddH01V01HorizInterpolate4Loop:	
    GetSrc1x13
    Interpolate4VertHoriz1x8
    subs       r6,r6,#1 
	bgt        AddH01V01HorizInterpolate4Loop
	
	sub        r12,r12,#104
	mov        r11,#8     
    GetVertSrc6x8                 @row0 0-3 
AddH01V01VertInterpolate4Loop:
    UnpckeSrcLow6x4
    Interpolate4Vert1x4Row       @row0 0-3 
    wmov       wr12,wr11
    UnpckeSrcHigh6x4 
    wldrd      wr0,[r9]
    add        r9,r9,r3  
    Interpolate4Vert1x4Row       @row0 4-7
    StoreVert1x8       1 
    
    add        r0,r0,r2
    wmov       wr0,wr1
    wmov       wr1,wr2
    wmov       wr2,wr3
    wmov       wr3,wr4
    wmov       wr4,wr5
    wldrd      wr5,[r12],#8	  
	
	subs       r11,r11,#1 
	bgt        AddH01V01VertInterpolate4Loop	
    
    add        sp,sp,#112 		
	ldmfd      sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,20,-5,1) 
@	v filter (1,-5,52,20,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4_H02V01:	@PROC
    stmfd       sp!,{r4-r11,lr}
    sub         sp,sp,#112             @13*8 + 8
    
    mov        r4,#16
   	mov		   r5,#20
   	mov        r7,#20
   	mov        r8,#2
   	mov        r10,#5
   	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8    
    tmcr       wcgr2,r10 
    
    mov	       r6,#13
    add        r12,sp,#8
    bic        r12,r12,#7
    sub        r0,r0,r2,lsl #1
    sub		   r0,r0,#2
    ands       r14,r0,#0x07
H02V01HorizInterpolate4:
	tmcr       wcgr1,r14
	bic        r0,r0,#7
H02V01HorizInterpolate4Loop:	
    GetSrc1x13
    Interpolate4VertHoriz1x8
    subs       r6,r6,#1 
	bgt        H02V01HorizInterpolate4Loop
	
	mov        r5,#32
	tbcsth     wr13,r5
	mov		   r5,#52
	tbcsth     wr14,r5
	mov        r5,#20
	tbcsth     wr15,r5	
    mov        r5,#6
    tmcr       wcgr2,r5 
    
    sub        r12,r12,#104 
    mov        r11,#8   
    GetVertSrc6x8 
H02V01VertInterpolate4Loop:
    UnpckeSrcLow6x4
    Interpolate4Vert1x4Row       @row0 0-3 
    wmov       wr12,wr11
    UnpckeSrcHigh6x4   
    Interpolate4Vert1x4Row       @row0 4-7
    StoreVert1x8       0 
   
    add        r0,r0,r2
    wmov       wr0,wr1
    wmov       wr1,wr2
    wmov       wr2,wr3
    wmov       wr3,wr4
    wmov       wr4,wr5	
	wldrd      wr5,[r12],#8 
	
	subs       r11,r11,#1 
	bgt        H02V01VertInterpolate4Loop
	
	add        sp,sp,#112 		
	ldmfd      sp!,{r4-r11,pc}	
    @ENDP	
    
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4Add_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,20,-5,1) 
@	v filter (1,-5,52,20,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4Add_H02V01:	@PROC
    stmfd       sp!,{r4-r11,lr}
    sub         sp,sp,#112             @13*8 + 8
   
   	mov        r4,#16
   	mov		   r5,#20
   	mov        r7,#20
   	mov        r8,#2
   	mov        r10,#5
   	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8    
    tmcr       wcgr2,r10 
    
    mov	       r6,#13
    add        r12,sp,#8
    bic        r12,r12,#7
    sub        r0,r0,r2,lsl #1
    sub		   r0,r0,#2
    mov        r9,r1 
    ands       r14,r0,#0x07
AddH02V01HorizInterpolate4:
	tmcr       wcgr1,r14
	bic        r0,r0,#7
AddH02V01HorizInterpolate4Loop:	
    GetSrc1x13
    Interpolate4VertHoriz1x8
    subs       r6,r6,#1 
	bgt        AddH02V01HorizInterpolate4Loop
	
	mov        r5,#32
	tbcsth     wr13,r5
	mov		   r5,#52
	tbcsth     wr14,r5
	mov        r5,#20
	tbcsth     wr15,r5	
    mov        r5,#6
    tmcr       wcgr2,r5 
    
    sub        r12,r12,#104 
    mov        r11,#8   
    GetVertSrc6x8 
AddH02V01VertInterpolate4Loop:
    UnpckeSrcLow6x4
    Interpolate4Vert1x4Row       @row0 0-3 
    wmov       wr12,wr11
    UnpckeSrcHigh6x4 
    wldrd      wr0,[r9]
    add        r9,r9,r3   
    Interpolate4Vert1x4Row       @row0 4-7
    StoreVert1x8       1 
   
    add        r0,r0,r2
    wmov       wr0,wr1
    wmov       wr1,wr2
    wmov       wr2,wr3
    wmov       wr3,wr4
    wmov       wr4,wr5	
	wldrd      wr5,[r12],#8 
	
	subs       r11,r11,#1 
	bgt        AddH02V01VertInterpolate4Loop
	
	add        sp,sp,#112 		
	ldmfd      sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4_H03V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,52,-5,1) 
@	v filter (1,-5,52,20,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4_H03V01:	@PROC
    stmfd       sp!,{r4-r11,lr}
    sub         sp,sp,#112            @13*8 + 8
   
   	mov        r4,#32
   	mov		   r5,#20
   	mov        r7,#52
   	mov        r8,#2
   	mov        r10,#6
   	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8    
    tmcr       wcgr2,r10 
    
    mov	       r6,#13
    add        r12,sp,#8
    bic        r12,r12,#7
    sub        r0,r0,r2,lsl #1
    sub		   r0,r0,#2
    ands       r14,r0,#0x07
H03V01HorizInterpolate4:
	tmcr       wcgr1,r14
	bic        r0,r0,#7
H03V01HorizInterpolate4Loop:	
    GetSrc1x13
    Interpolate4VertHoriz1x8
    subs       r6,r6,#1 
	bgt        H03V01HorizInterpolate4Loop
	
	mov        r5,#32
	tbcsth     wr13,r5
	mov		   r5,#52
	tbcsth     wr14,r5
	mov        r5,#20
	tbcsth     wr15,r5	
    mov        r5,#6
    tmcr       wcgr2,r5 
    
    sub        r12,r12,#104
    mov        r11,#8   
    GetVertSrc6x8    	
H03V01VertInterpolate4Loop:
    UnpckeSrcLow6x4
    Interpolate4Vert1x4Row       @row0 0-3 
    wmov       wr12,wr11
    UnpckeSrcHigh6x4   
    Interpolate4Vert1x4Row       @row0 4-7
    StoreVert1x8       0 
   
    add        r0,r0,r2
    wmov       wr0,wr1
    wmov       wr1,wr2
    wmov       wr2,wr3
    wmov       wr3,wr4
    wmov       wr4,wr5	
	wldrd      wr5,[r12],#8 
	
	subs       r11,r11,#1 
	bgt        H03V01VertInterpolate4Loop
	
	add        sp,sp,#112 		
	ldmfd      sp!,{r4-r11,pc}	
    @ENDP	
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4Add_H03V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,52,-5,1) 
@	v filter (1,-5,52,20,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4Add_H03V01:	@PROC
    stmfd       sp!,{r4-r11,lr}
    sub         sp,sp,#112             @13*8 + 8
   
   	mov        r4,#32
   	mov		   r5,#20
   	mov        r7,#52
   	mov        r8,#2
   	mov        r10,#6
   	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8    
    tmcr       wcgr2,r10 
    
    mov	       r6,#13
    add        r12,sp,#8
    bic        r12,r12,#7
    sub        r0,r0,r2,lsl #1
    sub		   r0,r0,#2
    mov        r9,r1 
    ands       r14,r0,#0x07
AddH03V01HorizInterpolate4:
	tmcr       wcgr1,r14
	bic        r0,r0,#7
AddH03V01HorizInterpolate4Loop:	
    GetSrc1x13
    Interpolate4VertHoriz1x8
    subs       r6,r6,#1 
	bgt        AddH03V01HorizInterpolate4Loop
	
	mov        r5,#32
	tbcsth     wr13,r5
	mov		   r5,#52
	tbcsth     wr14,r5
	mov        r5,#20
	tbcsth     wr15,r5	
    mov        r5,#6
    tmcr       wcgr2,r5 
    
    sub        r12,r12,#104
    mov        r11,#8   
    GetVertSrc6x8    	
AddH03V01VertInterpolate4Loop:
    UnpckeSrcLow6x4
    Interpolate4Vert1x4Row       @row0 0-3 
    wmov       wr12,wr11
    UnpckeSrcHigh6x4
    wldrd      wr0,[r9],r3
    add        r9,r9,r3   
    Interpolate4Vert1x4Row       @row0 4-7
    StoreVert1x8       1 
   
    add        r0,r0,r2
    wmov       wr0,wr1
    wmov       wr1,wr2
    wmov       wr2,wr3
    wmov       wr3,wr4
    wmov       wr4,wr5	
	wldrd      wr5,[r12],#8 
	
	subs       r11,r11,#1 
	bgt        AddH03V01VertInterpolate4Loop
	
	add        sp,sp,#112 		
	ldmfd      sp!,{r4-r11,pc}	
    @ENDP	
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	v filter (1,-5,20,20,-5,1) 
@-----------------------------------------------------------------------------------------------------
    .ALIGN 8
WMMX2_Interpolate4_H00V02:	@PROC
	stmfd      sp!,{r4-r11,lr}
	
	mov        r4,#16
	mov		   r5,#20
	mov        r6,#20
	mov        r7,#2
	mov        r10,#5
	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r6		
    tmcr       wcgr0,r7    
    tmcr       wcgr2,r10 
    
    mov        r11,#8
    sub        r0,r0,r2,lsl #1
    ands       r14,r0,#0x07
H00V02Interpolate4:
	tmcr       wcgr1,r14
	bic        r0,r0,#7
	GetSrc6x8  
H00V02Interpolate4Loop:	         
    UnpckeSrcLow6x4
    Interpolate4Vert1x4Row       @row0 0-3 
    wmov       wr12,wr11
    UnpckeSrcHigh6x4
    Interpolate4Vert1x4Row       @row0 4-7
    StoreVert1x8       0 
    
    wldrd      wr7,[r0,#8] 
    wldrd      wr6,[r0]
    add        r0,r0,r2
    pld		   [r0]	
    wmov       wr0,wr1
    wmov       wr1,wr2
    wmov       wr2,wr3
    wmov       wr3,wr4
    wmov       wr4,wr5	
	walignr1   wr5,wr6,wr7         
    
    subs       r11,r11,#1 
	bgt        H00V02Interpolate4Loop	

	ldmfd      sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4Add_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	v filter (1,-5,20,20,-5,1) 
@-----------------------------------------------------------------------------------------------------
 .ALIGN 8
WMMX2_Interpolate4Add_H00V02:	@PROC
	stmfd      sp!,{r4-r11,lr}
	
	mov        r4,#16
	mov		   r5,#20
	mov        r6,#20
	mov        r7,#2
	mov        r10,#5
	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r6		
    tmcr       wcgr0,r7    
    tmcr       wcgr2,r10 
    
    mov        r11,#8  
    mov        r9,r1  
    sub        r0,r0,r2,lsl #1
    ands       r14,r0,#0x07
AddH00V02Interpolate4:
	tmcr       wcgr1,r14
	bic        r0,r0,#7
	GetSrc6x8  
AddH00V02Interpolate4Loop:	         
    UnpckeSrcLow6x4
    Interpolate4Vert1x4Row       @row0 0-3 
    wmov       wr12,wr11
    UnpckeSrcHigh6x4
    wldrd     wr0,[r9]
    add       r9,r9,r3    
    Interpolate4Vert1x4Row       @row0 4-7
    StoreVert1x8       1 
    
    wldrd      wr7,[r0,#8] 
    wldrd      wr6,[r0]
    add        r0,r0,r2
    pld		   [r0]	
    wmov       wr0,wr1
    wmov       wr1,wr2
    wmov       wr2,wr3
    wmov       wr3,wr4
    wmov       wr4,wr5	
	walignr1   wr5,wr6,wr7         
    
    subs       r11,r11,#1 
	bgt        AddH00V02Interpolate4Loop	

	ldmfd      sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,52,20,-5,1) 
@	v filter (1,-5,20,20,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4_H01V02:	@PROC
    stmfd       sp!,{r4-r11,lr}
    sub         sp,sp,#112            @13*8 + 8
   
   	mov        r4,#32
   	mov		   r5,#52
   	mov        r7,#20
   	mov        r8,#2
   	mov        r10,#6
   	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8    
    tmcr       wcgr2,r10 
    
    mov	       r6,#13
    add        r12,sp,#8
    bic        r12,r12,#7
    sub        r0,r0,r2,lsl #1
    sub		   r0,r0,#2
    ands       r14,r0,#0x07
H01V02HorizInterpolate4:
	tmcr       wcgr1,r14
	bic        r0,r0,#7
H01V02HorizInterpolate4Loop:	
    GetSrc1x13
    Interpolate4VertHoriz1x8
    subs       r6,r6,#1 
	bgt        H01V02HorizInterpolate4Loop
	
	mov        r5,#16
	tbcsth     wr13,r5
	mov		   r5,#20
	tbcsth     wr14,r5
	mov        r5,#20
	tbcsth     wr15,r5	
    mov        r5,#5
    tmcr       wcgr2,r5 
    
    sub        r12,r12,#104
    mov        r11,#8   
    GetVertSrc6x8    	
H01V02VertInterpolate4Loop:
    UnpckeSrcLow6x4
    Interpolate4Vert1x4Row       @row0 0-3 
    wmov       wr12,wr11
    UnpckeSrcHigh6x4   
    Interpolate4Vert1x4Row       @row0 4-7
    StoreVert1x8       0 
   
    add        r0,r0,r2
    wmov       wr0,wr1
    wmov       wr1,wr2
    wmov       wr2,wr3
    wmov       wr3,wr4
    wmov       wr4,wr5	
	wldrd      wr5,[r12],#8 
	
	subs       r11,r11,#1 
	bgt        H01V02VertInterpolate4Loop
	
	add        sp,sp,#112 		
	ldmfd      sp!,{r4-r11,pc}	
    @ENDP	
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4Add_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,52,20,-5,1) 
@	v filter (1,-5,20,20,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4Add_H01V02:	@PROC
    stmfd       sp!,{r4-r11,lr}
    sub         sp,sp,#112            @13*8 + 8
   
   	mov        r4,#32
   	mov		   r5,#52
   	mov        r7,#20
   	mov        r8,#2
   	mov        r10,#6
   	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8    
    tmcr       wcgr2,r10 
    
    mov	       r6,#13
    add        r12,sp,#8
    bic        r12,r12,#7
    sub        r0,r0,r2,lsl #1
    sub		   r0, r0, #2
    mov        r9,r1 
    ands       r14,r0,#0x07
AddH01V02HorizInterpolate4:
	tmcr       wcgr1,r14
	bic        r0,r0,#7
AddH01V02HorizInterpolate4Loop:	
    GetSrc1x13
    Interpolate4VertHoriz1x8
    subs       r6,r6,#1 
	bgt        AddH01V02HorizInterpolate4Loop
	
	mov        r5,#16
	tbcsth     wr13,r5
	mov		   r5,#20
	tbcsth     wr14,r5
	mov        r5,#20
	tbcsth     wr15,r5	
    mov        r5,#5
    tmcr       wcgr2,r5
    
    sub        r12,r12,#104
    mov        r11,#8   
    GetVertSrc6x8    	
AddH01V02VertInterpolate4Loop:
    UnpckeSrcLow6x4
    Interpolate4Vert1x4Row       @row0 0-3 
    wmov       wr12,wr11
    UnpckeSrcHigh6x4
    wldrd      wr0,[r9],r3
    add        r9,r9,r3   
    Interpolate4Vert1x4Row       @row0 4-7
    StoreVert1x8       1 
   
    add        r0,r0,r2
    wmov       wr0,wr1
    wmov       wr1,wr2
    wmov       wr2,wr3
    wmov       wr3,wr4
    wmov       wr4,wr5	
	wldrd      wr5,[r12],#8 
	
	subs       r11,r11,#1 
	bgt        AddH01V02VertInterpolate4Loop
	
	add        sp,sp,#112 		
	ldmfd      sp!,{r4-r11,pc}	
    @ENDP 
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,20,-5,1) 
@	v filter (1,-5,20,20,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4_H02V02:	@PROC
    stmfd       sp!,{r4-r11,lr}
    sub         sp,sp,#112            @13*8 + 8
   
   	mov        r4,#16
   	mov		   r5,#20
   	mov        r7,#20
   	mov        r8,#2
   	mov        r10,#5
   	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8   
    tmcr       wcgr2,r10 
    
    mov	       r6,#13
    add        r12,sp,#8
    bic        r12,r12,#7
    sub        r0,r0,r2,lsl #1
    sub		   r0, r0, #2
    ands       r14,r0,#0x07
H02V02HorizInterpolate4:
	tmcr       wcgr1,r14
	bic        r0,r0,#7
H02V02HorizInterpolate4Loop:	
    GetSrc1x13
    Interpolate4VertHoriz1x8
    subs       r6,r6,#1 
	bgt        H02V02HorizInterpolate4Loop
	
	mov        r5,#16
	tbcsth     wr13,r5
	mov		   r5,#20
	tbcsth     wr14,r5
	mov        r5,#20
	tbcsth     wr15,r5	
    mov        r5,#5
    tmcr       wcgr2,r5 
    
    sub        r12,r12,#104
    mov        r11,#8   
    GetVertSrc6x8    	
H02V02VertInterpolate4Loop:
    UnpckeSrcLow6x4
    Interpolate4Vert1x4Row       @row0 0-3 
    wmov       wr12,wr11
    UnpckeSrcHigh6x4   
    Interpolate4Vert1x4Row       @row0 4-7
    StoreVert1x8       0 
   
    add        r0,r0,r2
    wmov       wr0,wr1
    wmov       wr1,wr2
    wmov       wr2,wr3
    wmov       wr3,wr4
    wmov       wr4,wr5	
	wldrd      wr5,[r12],#8 
	
	subs       r11,r11,#1 
	bgt        H02V02VertInterpolate4Loop
	
	add        sp,sp,#112 		
	ldmfd      sp!,{r4-r11,pc}	
    @ENDP	
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4Add_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,20,-5,1) 
@	v filter (1,-5,20,20,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4Add_H02V02:	@PROC
    stmfd       sp!,{r4-r11,lr}
    sub         sp,sp,#112            @13*8 + 8
   
   	mov        r4,#16
   	mov		   r5,#20
   	mov        r7,#20
   	mov        r8,#2
   	mov        r10,#5
   	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8   
    tmcr       wcgr2,r10 
    
    mov	       r6,#13
    add        r12,sp,#8
    bic        r12,r12,#7
    sub        r0,r0,r2,lsl #1
    sub		   r0, r0, #2
    mov        r9,r1 
    ands       r14,r0,#0x07
AddH02V02HorizInterpolate4:
	tmcr       wcgr1,r14
	bic        r0,r0,#7
AddH02V02HorizInterpolate4Loop:	
    GetSrc1x13
    Interpolate4VertHoriz1x8
    subs       r6,r6,#1 
	bgt        AddH02V02HorizInterpolate4Loop
	
	mov        r5,#16
	tbcsth     wr13,r5
	mov		   r5,#20
	tbcsth     wr14,r5
	mov        r5,#20
	tbcsth     wr15,r5	
    mov        r5,#5
    tmcr       wcgr2,r5 
    
    sub        r12,r12,#104
    mov        r11,#8   
    GetVertSrc6x8    	
AddH02V02VertInterpolate4Loop:
    UnpckeSrcLow6x4
    Interpolate4Vert1x4Row       @row0 0-3 
    wmov       wr12,wr11
    UnpckeSrcHigh6x4
    wldrd      wr0,[r9],r3
    add        r9,r9,r3   
    Interpolate4Vert1x4Row       @row0 4-7
    StoreVert1x8       1 
   
    add        r0,r0,r2
    wmov       wr0,wr1
    wmov       wr1,wr2
    wmov       wr2,wr3
    wmov       wr3,wr4
    wmov       wr4,wr5	
	wldrd      wr5,[r12],#8 
	
	subs       r11,r11,#1 
	bgt        AddH02V02VertInterpolate4Loop
	
	add        sp,sp,#112 		
	ldmfd      sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4_H03V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,52,-5,1) 
@	v filter (1,-5,20,20,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4_H03V02:	@PROC
    stmfd       sp!,{r4-r11,lr}
    sub         sp,sp,#112            @13*8 + 8
   
   	mov        r4,#32
   	mov		   r5,#20
   	mov        r7,#52
   	mov        r8,#2
   	mov        r10,#6
   	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8   
    tmcr       wcgr2,r10 
    
    mov	       r6,#13
    add        r12,sp,#8
    bic        r12,r12,#7
    sub        r0,r0,r2,lsl #1
    sub		   r0, r0, #2
    ands       r14,r0,#0x07
H03V02HorizInterpolate4:
	tmcr       wcgr1,r14
	bic        r0,r0,#7
H03V02HorizInterpolate4Loop:	
    GetSrc1x13
    Interpolate4VertHoriz1x8
    subs       r6,r6,#1 
	bgt        H03V02HorizInterpolate4Loop
	
	mov        r5,#16
	tbcsth     wr13,r5
	mov		   r5,#20
	tbcsth     wr14,r5
	mov        r5,#20
	tbcsth     wr15,r5	
    mov        r5,#5
    tmcr       wcgr2,r5 
    
    sub        r12,r12,#104
    mov        r11,#8   
    GetVertSrc6x8    	
H03V02VertInterpolate4Loop:
    UnpckeSrcLow6x4
    Interpolate4Vert1x4Row       @row0 0-3 
    wmov       wr12,wr11
    UnpckeSrcHigh6x4   
    Interpolate4Vert1x4Row       @row0 4-7
    StoreVert1x8       0 
   
    add        r0,r0,r2
    wmov       wr0,wr1
    wmov       wr1,wr2
    wmov       wr2,wr3
    wmov       wr3,wr4
    wmov       wr4,wr5	
	wldrd      wr5,[r12],#8 
	
	subs       r11,r11,#1 
	bgt        H03V02VertInterpolate4Loop
	
	add        sp,sp,#112 		
	ldmfd      sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4Add_H03V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,52,-5,1) 
@	v filter (1,-5,20,20,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4Add_H03V02:	@PROC
    stmfd       sp!,{r4-r11,lr}
    sub         sp,sp,#112            @13*8 + 8
   
   	mov        r4,#32
   	mov		   r5,#20
   	mov        r7,#52
   	mov        r8,#2
   	mov        r10,#6
   	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8   
    tmcr       wcgr2,r10 
    
    mov	       r6,#13
    add        r12,sp,#8
    bic        r12,r12,#7
    sub        r0,r0,r2,lsl #1
    sub		   r0, r0, #2
    ands       r14,r0,#0x07	
    mov        r9,r1 
    ands       r14,r0,#0x07
AddH03V02HorizInterpolate4:
	tmcr       wcgr1,r14
	bic        r0,r0,#7
AddH03V02HorizInterpolate4Loop:	
    GetSrc1x13
    Interpolate4VertHoriz1x8
    subs       r6,r6,#1 
	bgt        AddH03V02HorizInterpolate4Loop
	
	mov        r5,#16
	tbcsth     wr13,r5
	mov		   r5,#20
	tbcsth     wr14,r5
	mov        r5,#20
	tbcsth     wr15,r5	
    mov        r5,#5
    tmcr       wcgr2,r5 
    
    sub        r12,r12,#104
    mov        r11,#8   
    GetVertSrc6x8    	
AddH03V02VertInterpolate4Loop:
    UnpckeSrcLow6x4
    Interpolate4Vert1x4Row       @row0 0-3 
    wmov       wr12,wr11
    UnpckeSrcHigh6x4
    wldrd      wr0,[r9],r3
    add        r9,r9,r3   
    Interpolate4Vert1x4Row       @row0 4-7
    StoreVert1x8       1 
   
    add        r0,r0,r2
    wmov       wr0,wr1
    wmov       wr1,wr2
    wmov       wr2,wr3
    wmov       wr3,wr4
    wmov       wr4,wr5	
	wldrd      wr5,[r12],#8 
	
	subs       r11,r11,#1 
	bgt        AddH03V02VertInterpolate4Loop
	
	add        sp,sp,#112 		
	ldmfd      sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4_H00V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	v filter (1,-5,20,52,-5,1)
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4_H00V03:	@PROC
	stmfd      sp!,{r4-r11,lr}
	
	mov        r4,#32
	mov		   r5,#20
	mov        r7,#52
	mov        r8,#2
	mov        r10,#6
	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8   
    tmcr       wcgr2,r10 
    
    mov        r11,#8
    sub        r0,r0,r2,lsl #1
    ands       r14,r0,#0x07
H00V03Interpolate4:
	tmcr       wcgr1,r14
	bic        r0,r0,#7
	GetSrc6x8  
H00V03Interpolate4Loop:	         
    UnpckeSrcLow6x4
    Interpolate4Vert1x4Row       @row0 0-3 
    wmov       wr12,wr11
    UnpckeSrcHigh6x4
    Interpolate4Vert1x4Row       @row0 4-7
    StoreVert1x8       0 
    
    wldrd      wr7,[r0,#8] 
    wldrd      wr6,[r0]
    add        r0,r0,r2
    pld		   [r0]	
    wmov       wr0,wr1
    wmov       wr1,wr2
    wmov       wr2,wr3
    wmov       wr3,wr4
    wmov       wr4,wr5	
	walignr1   wr5,wr6,wr7         
    
    subs       r11,r11,#1 
	bgt        H00V03Interpolate4Loop	

	ldmfd      sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4Add_H00V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	v filter (1,-5,20,52,-5,1)
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4Add_H00V03:	@PROC
	stmfd      sp!,{r4-r11,lr}
	
	mov        r4,#32
	mov		   r5,#20
	mov        r7,#52
	mov        r8,#2
	mov        r10,#6
	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8   
    tmcr       wcgr2,r10 
    
    mov        r11,#8  
    mov        r9,r1  
    sub        r0,r0,r2,lsl #1
    ands       r14,r0,#0x07
AddH00V03Interpolate4:
	tmcr       wcgr1,r14
	bic        r0,r0,#7
	GetSrc6x8  
AddH00V03Interpolate4Loop:	         
    UnpckeSrcLow6x4
    Interpolate4Vert1x4Row       @row0 0-3 
    wmov       wr12,wr11
    UnpckeSrcHigh6x4
    wldrd     wr0,[r9]
    add       r9,r9,r3    
    Interpolate4Vert1x4Row       @row0 4-7
    StoreVert1x8       1 
    
    wldrd      wr7,[r0,#8] 
    wldrd      wr6,[r0]
    add        r0,r0,r2
    pld		   [r0]	
    wmov       wr0,wr1
    wmov       wr1,wr2
    wmov       wr2,wr3
    wmov       wr3,wr4
    wmov       wr4,wr5	
	walignr1   wr5,wr6,wr7         
    
    subs       r11,r11,#1 
	bgt        AddH00V03Interpolate4Loop	

	ldmfd      sp!,{r4-r11,pc}	
    @ENDP
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4_H01V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,52,20,-5,1) 
@	v filter (1,-5,20,52,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4_H01V03:	@PROC
    stmfd       sp!,{r4-r11,lr}
    sub         sp,sp,#112            @13*8 + 8
   
   	mov        r4,#32
   	mov		   r5,#52
   	mov        r7,#20
   	mov        r8,#2
   	mov        r10,#6
   	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8    
    tmcr       wcgr2,r10 
    
    mov	       r6,#13
    add        r12,sp,#8
    bic        r12,r12,#7
    sub        r0,r0,r2,lsl #1
    sub		   r0, r0, #2
    ands       r14,r0,#0x07
H01V03HorizInterpolate4:
	tmcr       wcgr1,r14
	bic        r0,r0,#7
H01V03HorizInterpolate4Loop:	
    GetSrc1x13
    Interpolate4VertHoriz1x8
    subs       r6,r6,#1 
	bgt        H01V03HorizInterpolate4Loop
	
	mov        r5,#32
	tbcsth     wr13,r5
	mov		   r5,#20
	tbcsth     wr14,r5
	mov        r5,#52
	tbcsth     wr15,r5	
    mov        r5,#6
    tmcr       wcgr2,r5 
    
    sub        r12,r12,#104
    mov        r11,#8   
    GetVertSrc6x8    	
H01V03VertInterpolate4Loop:
    UnpckeSrcLow6x4
    Interpolate4Vert1x4Row       @row0 0-3 
    wmov       wr12,wr11
    UnpckeSrcHigh6x4   
    Interpolate4Vert1x4Row       @row0 4-7
    StoreVert1x8       0 
   
    add        r0,r0,r2
    wmov       wr0,wr1
    wmov       wr1,wr2
    wmov       wr2,wr3
    wmov       wr3,wr4
    wmov       wr4,wr5	
	wldrd      wr5,[r12],#8 
	
	subs       r11,r11,#1 
	bgt        H01V03VertInterpolate4Loop
	
	add        sp,sp,#112 		
	ldmfd      sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4Add_H01V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,52,20,-5,1) 
@	v filter (1,-5,20,52,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4Add_H01V03:	@PROC
    stmfd       sp!,{r4-r11,lr}
    sub         sp,sp,#112            @13*8 + 8
   
   	mov        r4,#32
   	mov		   r5,#52
   	mov        r7,#20
   	mov        r8,#2
   	mov        r10,#6
   	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8    
    tmcr       wcgr2,r10 
    
    mov	       r6,#13
    add        r12,sp,#8
    bic        r12,r12,#7
    sub        r0,r0,r2,lsl #1
    sub		   r0, r0, #2
    mov        r9,r1 
    ands       r14,r0,#0x07
AddH01V03HorizInterpolate4:
	tmcr       wcgr1,r14
	bic        r0,r0,#7
AddH01V03HorizInterpolate4Loop:	
    GetSrc1x13
    Interpolate4VertHoriz1x8
    subs       r6,r6,#1 
	bgt        AddH01V03HorizInterpolate4Loop
	
	mov        r5,#32
	tbcsth     wr13,r5
	mov		   r5,#20
	tbcsth     wr14,r5
	mov        r5,#52
	tbcsth     wr15,r5	
    mov        r5,#6
    tmcr       wcgr2,r5 
    
    sub        r12,r12,#104
    mov        r11,#8   
    GetVertSrc6x8    	
AddH01V03VertInterpolate4Loop:
    UnpckeSrcLow6x4
    Interpolate4Vert1x4Row       @row0 0-3 
    wmov       wr12,wr11
    UnpckeSrcHigh6x4
    wldrd      wr0,[r9],r3
    add        r9,r9,r3   
    Interpolate4Vert1x4Row       @row0 4-7
    StoreVert1x8       1 
   
    add        r0,r0,r2
    wmov       wr0,wr1
    wmov       wr1,wr2
    wmov       wr2,wr3
    wmov       wr3,wr4
    wmov       wr4,wr5	
	wldrd      wr5,[r12],#8 
	
	subs       r11,r11,#1 
	bgt        AddH01V03VertInterpolate4Loop
	
	add        sp,sp,#112 		
	ldmfd      sp!,{r4-r11,pc}	
    @ENDP

@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4_H02V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,20,-5,1) 
@	v filter (1,-5,20,52,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4_H02V03:	@PROC
    stmfd       sp!,{r4-r11,lr}
    sub         sp,sp,#112            @13*8 + 8
   
   	mov        r4,#16
   	mov		   r5,#20
   	mov        r7,#20
   	mov        r8,#2
   	mov        r10,#5
   	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8   
    tmcr       wcgr2,r10 
    
    mov	       r6,#13
    add        r12,sp,#8
    bic        r12,r12,#7
    sub        r0,r0,r2,lsl #1
    sub		   r0, r0, #2
    ands       r14,r0,#0x07
H02V03HorizInterpolate4:
	tmcr       wcgr1,r14
	bic        r0,r0,#7
H02V03HorizInterpolate4Loop:	
    GetSrc1x13
    Interpolate4VertHoriz1x8
    subs       r6,r6,#1 
	bgt        H02V03HorizInterpolate4Loop
	
	mov        r5,#32
	tbcsth     wr13,r5
	mov		   r5,#20
	tbcsth     wr14,r5
	mov        r5,#52
	tbcsth     wr15,r5	
    mov        r5,#6
    tmcr       wcgr2,r5 
    
    sub        r12,r12,#104
    mov        r11,#8   
    GetVertSrc6x8    	
H02V03VertInterpolate4Loop:
    UnpckeSrcLow6x4
    Interpolate4Vert1x4Row       @row0 0-3 
    wmov       wr12,wr11
    UnpckeSrcHigh6x4   
    Interpolate4Vert1x4Row       @row0 4-7
    StoreVert1x8       0 
   
    add        r0,r0,r2
    wmov       wr0,wr1
    wmov       wr1,wr2
    wmov       wr2,wr3
    wmov       wr3,wr4
    wmov       wr4,wr5	
	wldrd      wr5,[r12],#8 
	
	subs       r11,r11,#1 
	bgt        H02V03VertInterpolate4Loop
	
	add        sp,sp,#112 		
	ldmfd      sp!,{r4-r11,pc}	
    @ENDP
    
@-----------------------------------------------------------------------------------------------------
@	void RV_FASTCALL C_Interpolate4Add_H02V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@
@	h filter (1,-5,20,20,-5,1) 
@	v filter (1,-5,20,52,-5,1) 
@-----------------------------------------------------------------------------------------------------
   .ALIGN 8
WMMX2_Interpolate4Add_H02V03:	@PROC
    stmfd      sp!,{r4-r11,lr}
    sub        sp,sp,#112            @13*8 + 8
   
   	mov        r4,#16
   	mov		   r5,#20
   	mov        r7,#20
   	mov        r8,#2
   	mov        r10,#5
   	
	tbcsth     wr13,r4	
	tbcsth     wr14,r5	
	tbcsth     wr15,r7		
    tmcr       wcgr0,r8   
    tmcr       wcgr2,r10 
    
    mov	       r6,#13
    add        r12,sp,#8
    bic        r12,r12,#7
    sub        r0,r0,r2,lsl #1
    sub		   r0, r0, #2
    mov        r9,r1 
    ands       r14,r0,#0x07
AddH02V03HorizInterpolate4:
	tmcr       wcgr1,r14
	bic        r0,r0,#7
AddH02V03HorizInterpolate4Loop:	
    GetSrc1x13
    Interpolate4VertHoriz1x8
    subs       r6,r6,#1 
	bgt        AddH02V03HorizInterpolate4Loop
	
	mov        r5,#32
	tbcsth     wr13,r5
	mov		   r5,#20
	tbcsth     wr14,r5
	mov        r5,#52
	tbcsth     wr15,r5	
    mov        r5,#6
    tmcr       wcgr2,r5 
    
    sub        r12,r12,#104
    mov        r11,#8   
    GetVertSrc6x8    	
AddH02V03VertInterpolate4Loop:
    UnpckeSrcLow6x4
    Interpolate4Vert1x4Row       @row0 0-3 
    wmov       wr12,wr11
    UnpckeSrcHigh6x4
    wldrd      wr0,[r9],r3
    add        r9,r9,r3   
    Interpolate4Vert1x4Row       @row0 4-7
    StoreVert1x8       1 
   
    add        r0,r0,r2
    wmov       wr0,wr1
    wmov       wr1,wr2
    wmov       wr2,wr3
    wmov       wr3,wr4
    wmov       wr4,wr5	
	wldrd      wr5,[r12],#8 
	
	subs       r11,r11,#1 
	bgt        AddH02V03VertInterpolate4Loop
	
	add        sp,sp,#112 		
	ldmfd      sp!,{r4-r11,pc}	
    @ENDP