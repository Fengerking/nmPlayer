;************************************************************************
;									                                    *
;	VisualOn, Inc. Confidential and Proprietary, 2010		            *
;								 	                                    *
;***********************************************************************/

	AREA	|.text|, CODE
	
	EXPORT	WMMX2_Transform4x4_Add
	EXPORT	WMMX2_Intra16x16ITransform4x4
	EXPORT	WMMX2_ITransform4x4_DCOnly_Add	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	void  ARMV7_Transform4x4_Add( I32 *pQuantBuf, U8 *pDest, U32 uDestStride, const U8 *pPredSrc, U32 uPredPitch )
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

WMMX2_Transform4x4_Add	PROC
    stmfd       sp!,{r4-r11,lr}
    
    ldr	        r12, [sp,#36]  ;uPredPitch
    
    wldrd       wr0,[r0],#8    ;row0 0-1 
	wldrd       wr1,[r0],#8    ;row0 2-3	
	wldrd       wr2,[r0],#8    ;row1 0-1 
	wldrd       wr3,[r0],#8    ;row1 2-3
	wldrd       wr4,[r0],#8    ;row2 0-1 
	wldrd       wr5,[r0],#8    ;row2 2-3
	wldrd       wr6,[r0],#8    ;row3 0-1 
	wldrd       wr7,[r0]       ;row3 2-3
    
    mov         r4,#13
   	mov		    r5,#7
   	mov         r6,#17
   	mov         r7,#0x200
   	mov         r8,#10
   	
	tbcstw      wr13,r4        ;13	
	tbcstw      wr14,r5	       ;7
	tbcstw      wr15,r6        ;17
	tbcstw      wr12,r7        ;0x200
	tmcr        wcgr0,r8       ;10
	
	ldr         r7, [r3],r12
	ldr         r8, [r3],r12
	ldr         r9, [r3],r12
	ldr         r10,[r3]
	
	waddwss     wr8,wr0,wr4    
	waddwss     wr9,wr1,wr5
	wsubwss     wr0,wr0,wr4    
	wsubwss     wr1,wr1,wr5
	
	wmulwl      wr4,wr8,wr13    ;b0  wr4 wr5
	wmulwl      wr5,wr9,wr13 
	wmulwl      wr0,wr0,wr13    ;b1  wr0 wr1
	wmulwl      wr1,wr1,wr13
	
	wmulwl      wr8,wr2,wr14
	wmulwl      wr9,wr3,wr14
	wmulwl      wr10,wr6,wr15
	wmulwl      wr11,wr7,wr15 
	
	wsubwss     wr8,wr8,wr10    ;b2  wr8 wr9
	wsubwss     wr9,wr9,wr11  
	
	wmulwl      wr2,wr2,wr15
	wmulwl      wr3,wr3,wr15
	wmulwl      wr6,wr6,wr14
	wmulwl      wr7,wr7,wr14 
	
	waddwss     wr2,wr2,wr6     ;b3 wr2 wr3
	waddwss     wr3,wr3,wr7 
	
	waddwss     wr6,wr4,wr2     ;pin[0] = b0 + b3
	waddwss     wr7,wr5,wr3	
	wsubwss     wr4,wr4,wr2     ;pin[12] = b0 - b3
	wsubwss     wr5,wr5,wr3	
	waddwss     wr2,wr0,wr8     ;pin[4] = b1 + b2
	waddwss     wr3,wr1,wr9	
	wsubwss     wr0,wr0,wr8     ;pin[8] = b1 - b2
	wsubwss     wr1,wr1,wr9	
	
	wunpckilw   wr8,wr6,wr2     ;pin[0] 0-3
	wunpckilw   wr9,wr0,wr4
	wunpckihw   wr10,wr6,wr2    ;pin[1] 0-3
	wunpckihw   wr11,wr0,wr4
	wunpckilw   wr0,wr7,wr3     ;pin[2] 0-3
	wunpckilw   wr2,wr1,wr5
	wunpckihw   wr3,wr7,wr3     ;pin[3] 0-3
	wunpckihw   wr1,wr1,wr5
	
	waddwss     wr4,wr8,wr0    
	waddwss     wr5,wr9,wr2
	wsubwss     wr8,wr8,wr0    
	wsubwss     wr9,wr9,wr2

	wmulwl      wr4,wr4,wr13   ;b0  wr4 wr5
	wmulwl      wr5,wr5,wr13	
	wmulwl      wr8,wr8,wr13   ;b1  wr8 wr9
	wmulwl      wr9,wr9,wr13 
	
	wmulwl      wr6,wr10,wr14
	wmulwl      wr7,wr11,wr14
	wmulwl      wr0,wr3,wr15
	wmulwl      wr2,wr1,wr15 
	
	wsubwss     wr6,wr6,wr0     ;b2 wr6 wr7
	wsubwss     wr7,wr7,wr2  
	
	wmulwl      wr0,wr10,wr15
	wmulwl      wr2,wr11,wr15
	wmulwl      wr3,wr3,wr14
	wmulwl      wr1,wr1,wr14 
	
	waddwss     wr3,wr0,wr3     ;b3 wr3 wr2
	waddwss     wr2,wr2,wr1 
	
	waddwss     wr0,wr4,wr3     ;pin[0] = (b0 + b3 + 0x200) >> 10
	waddwss     wr1,wr5,wr2
	waddwss     wr0,wr0,wr12
	waddwss     wr1,wr1,wr12
	wsrawg      wr0,wr0,wcgr0
	wsrawg      wr1,wr1,wcgr0
		
	wsubwss     wr3,wr4,wr3     ;pin[3] = (b0 - b3 + 0x200) >> 10
	wsubwss     wr2,wr5,wr2
	waddwss     wr3,wr3,wr12
	waddwss     wr2,wr2,wr12
	wsrawg      wr3,wr3,wcgr0
	wsrawg      wr2,wr2,wcgr0	
		
	waddwss     wr4,wr8,wr6     ;pin[1] = (b1 + b2 + 0x200) >> 10
	waddwss     wr5,wr9,wr7	
	waddwss     wr4,wr4,wr12
	waddwss     wr5,wr5,wr12
	wsrawg      wr4,wr4,wcgr0
	wsrawg      wr5,wr5,wcgr0
	
	wsubwss     wr6,wr8,wr6     ;pin[2] = (b1 - b2 + 0x200) >> 10
	wsubwss     wr7,wr9,wr7
	waddwss     wr6,wr6,wr12
	waddwss     wr7,wr7,wr12
	wsrawg      wr6,wr6,wcgr0
	wsrawg      wr7,wr7,wcgr0
	
	wunpckilw   wr8,wr0,wr4     ;row0 0 - 1
	wunpckihw   wr9,wr0,wr4	    ;row1 0 - 1
	wunpckilw   wr12,wr1,wr5    ;row2 0 - 1
	wunpckihw   wr13,wr1,wr5	;row3 0 - 1
	wunpckilw   wr10,wr6,wr3    ;row0 2 - 3
	wunpckihw   wr11,wr6,wr3    ;row1 2 - 3
	wunpckilw   wr14,wr7,wr2    ;row2 2 - 3
	wunpckihw   wr15,wr7,wr2    ;row3 2 - 3
	
	tmcrr       wr6,r7,r8
	tmcrr       wr7,r9,r10
	
	wpackwss    wr0,wr8,wr10    ;row0 0 - 3
	wpackwss    wr1,wr9,wr11    ;row1 0 - 3
	wpackwss    wr2,wr12,wr14   ;row2 0 - 3
	wpackwss    wr3,wr13,wr15   ;row3 0 - 3		
	
	wunpckelub  wr4,wr6         ;pPredSrc 0         
	wunpckehub  wr5,wr6         ;pPredSrc 1  
	wunpckelub  wr8,wr7         ;pPredSrc 2 
	wunpckehub  wr9,wr7         ;pPredSrc 3 
	
	waddhss     wr0,wr0,wr4
	waddhss     wr1,wr1,wr5
	waddhss     wr2,wr2,wr8
	waddhss     wr3,wr3,wr9
	
	wpackhus    wr0,wr0,wr1
	wpackhus    wr1,wr2,wr3
	tmrrc       r7,r8,wr0
	tmrrc       r9,r10,wr1
	
	str		    r7,[r1],r2
	str		    r8,[r1],r2	 
	str		    r9,[r1],r2
	str		    r10,[r1] 

	ldmfd       sp!,{r4-r11,pc}	
    ENDP
    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	void  ARMV7_Intra16x16ITransform4x4(I32 *pQuantBuf)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

WMMX2_Intra16x16ITransform4x4	PROC
    stmfd       sp!,{r4-r11,lr}
    
    mov         r8,r0
    
    wldrd       wr0,[r8],#8    ;row0 0-1 
	wldrd       wr1,[r8],#8    ;row0 2-3	
	wldrd       wr2,[r8],#8    ;row1 0-1 
	wldrd       wr3,[r8],#8    ;row1 2-3
	wldrd       wr4,[r8],#8    ;row2 0-1 
	wldrd       wr5,[r8],#8    ;row2 2-3
	wldrd       wr6,[r8],#8    ;row3 0-1 
	wldrd       wr7,[r8]       ;row3 2-3
    
    mov         r4,#13
   	mov		    r5,#7
   	mov         r6,#17
   	mov         r7,#48
   	mov         r8,#15
   	
	tbcstw      wr13,r4        ;13	
	tbcstw      wr14,r5	       ;7
	tbcstw      wr15,r6        ;17
	tbcstw      wr12,r7        ;48
	tmcr        wcgr0,r8       ;15
	
	waddwss     wr8,wr0,wr4    
	waddwss     wr9,wr1,wr5
	wsubwss     wr0,wr0,wr4    
	wsubwss     wr1,wr1,wr5
	
	wmulwl      wr4,wr8,wr13   ;b0
	wmulwl      wr5,wr9,wr13 
	wmulwl      wr0,wr0,wr13   ;b1
	wmulwl      wr1,wr1,wr13
	
	wmulwl      wr8,wr2,wr14
	wmulwl      wr9,wr3,wr14
	wmulwl      wr10,wr6,wr15
	wmulwl      wr11,wr7,wr15 
	
	wsubwss     wr8,wr8,wr10    ;b2
	wsubwss     wr9,wr9,wr11  
	
	wmulwl      wr2,wr2,wr15
	wmulwl      wr3,wr3,wr15
	wmulwl      wr6,wr6,wr14
	wmulwl      wr7,wr7,wr14 
	
	waddwss     wr2,wr2,wr6     ;b3
	waddwss     wr3,wr3,wr7 
	
	waddwss     wr6,wr4,wr2     ;pin[0] = b0 + b3
	waddwss     wr7,wr5,wr3	
	wsubwss     wr4,wr4,wr2     ;pin[12] = b0 - b3
	wsubwss     wr5,wr5,wr3	
	waddwss     wr2,wr0,wr8     ;pin[4] = b1 + b2
	waddwss     wr3,wr1,wr9	
	wsubwss     wr0,wr0,wr8     ;pin[8] = b1 - b2
	wsubwss     wr1,wr1,wr9	
	
	wunpckilw   wr8,wr6,wr2     ;pin[0] 0-3
	wunpckilw   wr9,wr0,wr4
	wunpckihw   wr10,wr6,wr2    ;pin[1] 0-3
	wunpckihw   wr11,wr0,wr4
	wunpckilw   wr0,wr7,wr3     ;pin[2] 0-3
	wunpckilw   wr2,wr1,wr5
	wunpckihw   wr3,wr7,wr3     ;pin[3] 0-3
	wunpckihw   wr1,wr1,wr5
	
	waddwss     wr4,wr8,wr0    
	waddwss     wr5,wr9,wr2
	wsubwss     wr8,wr8,wr0    
	wsubwss     wr9,wr9,wr2

	wmulwl      wr4,wr4,wr13   ;b0
	wmulwl      wr5,wr5,wr13	
	wmulwl      wr8,wr8,wr13   ;b1
	wmulwl      wr9,wr9,wr13 
	
	wmulwl      wr6,wr10,wr14
	wmulwl      wr7,wr11,wr14
	wmulwl      wr0,wr3,wr15
	wmulwl      wr2,wr1,wr15 
	
	wsubwss     wr6,wr6,wr0     ;b2
	wsubwss     wr7,wr7,wr2  
	
	wmulwl      wr0,wr10,wr15
	wmulwl      wr2,wr11,wr15
	wmulwl      wr3,wr3,wr14
	wmulwl      wr1,wr1,wr14 
	
	waddwss     wr3,wr0,wr3     ;b3
	waddwss     wr2,wr2,wr1 
	
	waddwss     wr0,wr4,wr3     ;pin[0] = ((b0 + b3) * 48) >> 15
	waddwss     wr1,wr5,wr2
	wmulwl      wr0,wr0,wr12
	wmulwl      wr1,wr1,wr12
	wsrawg      wr0,wr0,wcgr0
	wsrawg      wr1,wr1,wcgr0
		
	wsubwss     wr3,wr4,wr3     ;pin[3] = ((b0 - b3) * 48) >> 15
	wsubwss     wr2,wr5,wr2
	wmulwl      wr2,wr2,wr12
	wmulwl      wr3,wr3,wr12
	wsrawg      wr2,wr2,wcgr0
	wsrawg      wr3,wr3,wcgr0
		
	waddwss     wr4,wr8,wr6     ;pin[1] = ((b1 + b2) * 48) >> 15
	waddwss     wr5,wr9,wr7	
	wmulwl      wr4,wr4,wr12
	wmulwl      wr5,wr5,wr12
	wsrawg      wr4,wr4,wcgr0
	wsrawg      wr5,wr5,wcgr0
	
	wsubwss     wr6,wr8,wr6     ;pin[2] = ((b1 - b2) * 48) >> 15
	wsubwss     wr7,wr9,wr7
	wmulwl      wr6,wr6,wr12
	wmulwl      wr7,wr7,wr12
	wsrawg      wr6,wr6,wcgr0
	wsrawg      wr7,wr7,wcgr0
	
	wunpckilw   wr8,wr0,wr4     ;row0 0 - 1
	wunpckihw   wr9,wr0,wr4	    ;row1 0 - 1
	wunpckilw   wr12,wr1,wr5    ;row2 0 - 1
	wunpckihw   wr13,wr1,wr5	;row3 0 - 1
	wunpckilw   wr10,wr6,wr3    ;row0 2 - 3
	wunpckihw   wr11,wr6,wr3    ;row1 2 - 3
	wunpckilw   wr14,wr7,wr2    ;row2 2 - 3
	wunpckihw   wr15,wr7,wr2    ;row3 2 - 3
	
	wstrd      wr8, [r0],#8     ;row0 0-1 
	wstrd      wr10,[r0],#8     ;row0 2-3
	wstrd      wr9, [r0],#8     ;row1 0-1 
	wstrd      wr11,[r0],#8     ;row1 2-3
	wstrd      wr12,[r0],#8     ;row2 0-1 
	wstrd      wr14,[r0],#8     ;row2 2-3
	wstrd      wr13,[r0],#8     ;row3 0-1 
	wstrd      wr15,[r0]        ;row3 2-3	

	ldmfd       sp!,{r4-r11,pc}	
    ENDP
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	void  ARMV7_ITransform4x4_DCOnly_Add(I32 *pQuantBuf, U8 *pDest, U32 uDestStride, const U8 *pPredSrc, U32 uPredPitch)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

WMMX2_ITransform4x4_DCOnly_Add	PROC
    stmfd       sp!,{r4-r11,lr}   
    
    ldr	        r4,[r0]                   ;v = ((pQuantBuf[0] * 13 * 13) + 0x200) >> 10;
    ldr	        r12, [sp,#36]	          ;uPredPitch
    mov         r10,r1
	mov	        lr,#169                   ;13*13
	
	ldr         r5,[r3],r12
	ldr         r6,[r3],r12
	ldr         r7,[r3],r12
	ldr         r8,[r3]
	
	mul	        r4, lr, r4
	add	        r4, r4, #0x200
	mov	        r4, r4,asr #10
	
	cmp         r4, #0                     ;if (v==0)                       
    bne         outCopyBlock4x4_WMMX2     
		             
CopyBlock4x4_WMMX2
    str         r5,[r1], r2 
    str         r6,[r1], r2 
    str         r7,[r1], r2 
    str         r8,[r1] 
		    		  				                                       
    ldmfd       sp!,{r4-r11,pc}	
outCopyBlock4x4_WMMX2
	blt         little_begin_WMMX2
big_begin_WMMX2	
    tmcrr       wr14,r5,r6
    tmcrr       wr15,r7,r8
    mov         r11,#0xffffff00
    tst         r4,r11
    movne       r4,#0xff
    movmi       r4,#0x00 
    
    tbcstb      wr0,r4
    waddbus     wr14,wr14,wr0
    waddbus     wr15,wr15,wr0
    tmrrc       r5,r6,wr14     
    tmrrc       r7,r8,wr15
    
	str		    r5,[r1],r2
	str		    r6,[r1],r2	 
	str		    r7,[r1],r2
	str		    r8,[r1]                                    
  
    ldmfd   sp!,{r4-r11,pc}     
little_begin_WMMX2                    
    rsb         r12,r4,#0                       
    tmcrr       wr14,r5,r6
    tmcrr       wr15,r7,r8
    mov         r11,#0xffffff00
    tst         r12,r11
    movne       r12,#0xff
    movmi       r12,#0x00          

    tbcstb      wr0,r12
    wsubbus     wr14,wr14,wr0
    wsubbus     wr15,wr15,wr0  
    tmrrc       r5,r6,wr14   
    tmrrc       r7,r8,wr15
    
    str		    r5,[r1],r2
	str		    r6,[r1],r2	 
	str		    r7,[r1],r2
	str		    r8,[r1] 
	
	ldmfd       sp!,{r4-r11,pc}	
    ENDP