
	AREA ITRFRM, CODE, READONLY
C_ITransform4x4_Add		PROC
        STMFD    sp!,{r4-r12,lr}
        MOV		 lr,#13
        MOV     r4,r2      	 ;pin = in
        MOV     r9,#4      	 ;r9 loop var
Ln16
		LDR		r5,[r4]            ;r5 = a0
		LDR		r6,[r4, #0x20]     ;r6 = a2 
		SUBS	r9,r9,#1		   ;decrement loop var       			

        ADD		r10,r5,r6 	       ;b0 = a0 + a2
		MUL		r10,lr,r10		   ;b0 = (a0 + a2)*13
        SUB		r11,r5,r6 	       ;b1 = a0 - a2
		MUL		r11,lr,r11		   ;b1 = (a0 - a2)*13
		
		LDR		r5,[r4, #0x10]      ;r5 = a1
		LDR		r6,[r4, #0x30]      ;r6 = a3
		
        RSB      r7,r5,r5,LSL #3    ;b2 = a1 * 7
        ADD      r12,r6,r6,LSL #4  ;Temp(r12) = a3*17
        SUB		 r7,r7,r12			  ;b2(r7) = b2 - Temp
        ADD      r5,r5,r5,LSL #4    ;b3 = a1*17
        RSB      r6,r6,r6,LSL #3    ;Temp= a3 * 7
        ADD		 r12,r5,r6	          ;b3(r12) = b3 + Temp		
        
        ADD		 r5,r10,r12	       ;pin[0] = b0 + b3
        ADD		 r6,r11,r7		   ;pin[4] = b1 + b2
        STR		 r5,[r4], #4
        STR		 r6,[r4,#12]
        SUB		 r5,r11,r7			;pin[8] = b1 - b2
        SUB      r6,r10,r12		    ;pin[12] = b0 - b3	        
        STR		 r5,[r4,#28]
        STR		 r6,[r4,#44]        

        BNE      Ln16
        
        MOV      r9,#4
        MOV      r4,r2      	   ;pin = in
Ln116
		LDR		  r5,[r4]          ;r5 = a0
		LDR		  r6,[r4,#8]     ;r6 = a2     			

        ADD		 r10,r5,r6 	       ;b0 = a0 + a2
		MUL		 r10,lr,r10
        SUB		 r11,r5,r6 	       ;b1 = a0 - a2
		MUL		 r11,lr,r11
		
		LDR      r5,[r4, #4]      ;r5 = a1
		LDR      r6,[r4, #12]    ;r6 = a3
		ADD		 r4,r4,#16      ;pin += 4				
		
        RSB      r7,r5,r5,LSL #3	  ;b2 = a1 * 7
        ADD      r12,r6,r6,LSL #4  ;Temp(r12) = a3*17
        SUB		 r7,r7,r12			  ;b2(r7) = b2 - Temp
        ADD      r5,r5,r5,LSL #4   ;b3 = a1*17
        RSB      r6,r6,r6,LSL #3   ;Temp= a3 * 7
        ADD		 r12,r5,r6			 ;b3(r12) = b3 + Temp		
        
        ADD		 r5,r10,r12	       ;pin[0] = (b0 + b3 + 0x200) >> 10
        LDRB	 r8,[r0]
        ADD      r5,r5,#0x200
        MOV      r5,r5,ASR #10 
        ADD		 r5, r5, r8
               
        ADD		r6,r11,r7		   ;pin[1] = (b1 + b2 + 0x200) >> 10
        LDRB    r8,[r0,#1]
        ADD      r6,r6,#0x200
        MOV     r6,r6,ASR #10
        ADD		 r6, r6, r8                
        
        SUB		 r7,r11,r7	       ;pin[2] = (b1 - b2 + 0x200) >> 10
        LDRB    r8,[r0, #2]
        ADD      r7,r7,#0x200
        MOV     r7,r7,ASR #10 
        ADD      r7, r7, r8
                               
        SUB      r8,r10,r12		   ;pin[3] = (b0 - b3 + 0x200) >> 10
        LDRB    r10,[r0,#3]   	        
        ADD      r8,r8,#0x200
        MOV     r8,r8,ASR #10
        ADD      r8,r8,r10    
        ADD		 r0,r0,r1           ;pPred += predPitch  
        
		LDR		  r10, [sp, #40]
		ORR       r11, r5, r6
		ORR       r11, r11, r7
		ORR       r11, r11, r8
		BICS       r11, r11, #0xFF  ; 0xFF = 255
		BEQ		  Row4_Write
		
		MOV		  r11, #0xFFFFFF00

		TST		  r5, r11
		MOVNE  r5, #0xFF
		MOVMI	  r5, #0x00

		TST		   r6, r11
		MOVNE   r6, #0xFF
		MOVMI	   r6, #0x00

		TST		   r7, r11
		MOVNE   r7, #0xFF
		MOVMI	   r7, #0x00

		TST         r8, r11
		MOVNE   r8, #0xFF
		MOVMI	   r8, #0x00

Row4_Write              
        ORR		  r5, r5, r8, LSL #24
        ORR		  r5, r5, r6, LSL #8
        ORR		  r5, r5, r7, LSL #16
        STR		  r5, [r10]
        ADD       r10, r10, r3		;pDst += uPitch
        STR		  r10, [sp, #40]      
        
        SUBS     r9,r9,#1		   ;decrement loop var         
        BNE      Ln116

        LDMFD    sp!,{r4-r12,pc}
        ;MOV 	 pc,lr	
		ENDP
		EXPORT	C_ITransform4x4_Add
		
		AREA ITRFRM, CODE, READONLY
C_BITransform4x4_Add		PROC
       
        STMFD    sp!,{r4-r12,lr}
		MOV		 lr,#13
        MOV      r4,r1      	 ;pin = in
        MOV      r9,#4      	 ;r9 loop var
LnB16
		LDR      r5,[r4,#0x00]     ;r5 = a0
		LDR      r7,[r4,#0x20]     ;r7 = a2
		LDR      r6,[r4,#0x10]     ;r6 = a1
		LDR      r8,[r4,#0x30]     ;r8 = a3	
		
		SUBS     r9,r9,#1	 ;decrement loop var      
			
        ADD		 r10,r5,r7 	 ;b0 = a0 + a2
		MUL      r10,lr,r10
        SUB		 r11,r5,r7 	 ;b1 = a0 - a2
		MUL      r11,lr,r11
		
        RSB      r7,r6,r6,LSL #3    ;b2 = a1 * 7
        ADD      r12,r8,r8,LSL #4  ;Temp(r12) = a3*17
        SUB		 r7,r7,r12             ;b2(r9) = b2 - Temp
        ADD      r5,r6,r6,LSL #4    ;b3 = a1*17
        RSB      r6,r8,r8,LSL #3    ;Temp= a3 * 7
        ADD		 r12,r5,r6	          ;b3(r12) = b3 + Temp		
        
        ADD		 r5,r10,r12			  ;pin[0] = b0 + b3
		SUB      r8,r10,r12			  ;pin[12] = b0 - b3
        ADD		 r6,r11,r7		      ;pin[4] = b1 + b2
        SUB		 r7,r11,r7	          ;pin[8] = b1 - b2
        STR		 r5,[r4],#4 
        STR		 r6,[r4,#12]
        STR		 r7,[r4,#28]
        STR		 r8,[r4,#44]       
          
        BNE      LnB16
        
        MOV      r9,#4
        MOV      r4,r1      	 ;pin = in
LnB116
		LDR		 r5,[r4]
		LDR		 r6,[r4, #4]
		LDR		 r7,[r4, #8]
		LDR		 r8,[r4, #12]
		ADD		 r4,r4,#16          ;pin += 4		     			

        ADD		 r10,r5,r7 	       ;b0 = a0 + a2		
		MUL      r10,lr,r10
        SUB		 r11,r5,r7 	       ;b1 = a0 - a2		
		MUL      r11,lr,r11
		
        RSB      r7,r6,r6,LSL #3    ;b2 = a1 * 7
        ADD      r12,r8,r8,LSL #4  ;Temp(r12) = a3*17
        SUB		 r7,r7,r12			  ;b2(r9) = b2 - Temp
        ADD      r5,r6,r6,LSL #4    ;b3 = a1*17
        RSB      r6,r8,r8,LSL #3    ;Temp= a3 * 7
        ADD		 r12,r5,r6			  ;b3(r12) = b3 + Temp		
        
        ADD		 r5,r10,r12	         ;pin[0] = (b0 + b3 + 0x200) >> 10
        LDRB	 r8,[r0]
        ADD      r5,r5,#0x200
        MOV     r5,r5,ASR #10
        ADD		 r5,r5,r8
                    
		ADD		 r6,r11,r7			   ;pin[1] = (b1 + b2 + 0x200) >> 10
		LDRB	 r8,[r0, #1]
        ADD      r6,r6,#0x200
        MOV     r6,r6,ASR #10 
        ADD		 r6, r6, r8
                               
        SUB		 r7,r11,r7			    ;pin[2] = (b1 - b2 + 0x200) >> 10
        LDRB	 r8,[r0, #2]
        ADD      r7,r7,#0x200
        MOV     r7,r7,ASR #10
        ADD		 r7,r7,r8
        
        SUB      r8,r10,r12		    ;pin[3] = (b0 - b3 + 0x200) >> 10  `
        LDRB	 r10,[r0, #3] 	        
        ADD      r8,r8,#0x200		
        MOV     r8,r8,ASR #10
        ADD		 r8,r8,r10
        
		ORR       r11, r5, r6
		ORR       r11, r11, r7
		ORR       r11, r11, r8
		BICS       r11, r11, #0xFF  ; 0xFF = 255
		BEQ		  Row4_Write_2
		
		MOV		  r11, #0xFFFFFF00

		TST		  r5, r11
		MOVNE  r5, #0xFF
		MOVMI	  r5, #0x00

		TST		   r6, r11
		MOVNE   r6, #0xFF
		MOVMI	   r6, #0x00

		TST		   r7, r11
		MOVNE   r7, #0xFF
		MOVMI	   r7, #0x00

		TST         r8, r11
		MOVNE   r8, #0xFF
		MOVMI	   r8, #0x00

Row4_Write_2                           
        ORR		  r5, r5, r8, LSL #24
        ORR		  r5, r5, r6, LSL #8
        ORR		  r5, r5, r7, LSL #16
        STR		  r5, [r0]
        ADD       r0, r0, r2
        
        SUBS     r9,r9,#1	 ;decrement loop var    
        BNE       LnB116

        LDMFD    sp!,{r4-r12,pc}
		ENDP
		EXPORT	C_BITransform4x4_Add
		
		AREA ITRFRM, CODE, READONLY
C_ITransform4x4_DCOnly_Add11		PROC
        stmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r14}
        
		ldr		   r4,[r2]
		mov	   lr,#13
		mul	   r4, lr, r4
		mul	   r4, lr, r4
		add	   r4, r4, #0x200
		mov	   r4,r4,asr #10

		ldr		   r9,[sp,#36]	;Dst
        cmp     r4, #0    ;if (v==0)                       
        bne     outCopyBlock4x4_asm_ARMv4     
		             
CopyBlock4x4_asm_ARMv6
		ldr     r4, [r0], r1        
 		ldr     r6, [r0], r1 
 		ldr     r8, [r0], r1        
 		ldr     r10, [r0]	
        str     r4, [r9], r3 
        str     r6, [r9], r3 
        str     r8, [r9], r3 
        str     r10, [r9] 
		    		  				                                       
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15}

outCopyBlock4x4_asm_ARMv4
		mov	 r8, #4
		ldr     r10, MaskCarry
		str		r8,[sp,#36]	;count 
		blt     little_begin_ARMv4 

big_begin_ARMv4	
        orr     r12, r4, r4, lsl #8            
        orr     r5, r12, r12, lsl #16                                          
loop_do_ARMv4
		;a = ((uint32_t*)Src)[0];                
        ldr     r8, [r0], r1                    
                       
		;b = a + v;
        add     r14, r5, r8                      

		;c = a & v;
        and     r7, r5, r8                        

		;a ^= v;	
        eor     r6, r8, r5                        

		;a &= ~b;
        mvn     r8, r14                           

        and     r8, r6, r8                        

		;a |= c;	
        orr      r7, r8, r7                        

		;a &= MaskCarry;
        and     r12, r7, r10                      

		;c = a << 1;	b -= c;	
        sub     r14, r14, r12, lsl #1             

		;b |= c - (a >> 7);
        mov     r7, r12, lsr #7                   
        rsb     r12, r7, r12, lsl #1              
        orr     r11, r14, r12                                                                  
              
		ldr		r8,[sp,#36]	;count    		                        
		;((uint32_t*)Dst)[0] = b;
        str     r11, [r9], r3                                       

		subs	r8, r8, #1     
		str		r8,[sp,#36]	;count                 
        bne   loop_do_ARMv4  											                                             
   
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15}   
	
little_begin_ARMv4                     
        rsb     r12, r4, #0                       
        orr     r12, r12, r12, lsl #8            
        orr     r5, r12, r12, lsl #16
little_loop_do_ARMv4              
		;a = ((uint32_t*)Src)[0];                
        ldr     r8, [r0], r1                  

		;a = ~a;	
        mvn   r8, r8                                    

		;b = a + v;
        add     r14, r5, r8                      

		;c = a & v;
        and     r7, r5, r8                        

		;a ^= v;	
        eor     r6, r8, r5                        

		;a &= ~b;
        mvn     r8, r14                           

        and     r8, r6, r8                        

		;a |= c;	
        orr     r7, r8, r7                        

		;a &= MaskCarry;
        and     r12, r7, r10                      

		;c = a << 1;	b -= c;	
        sub     r14, r14, r12, lsl #1             

		;b |= c - (a >> 7);
        mov     r7, r12, lsr #7                   
        rsb     r12, r7, r12, lsl #1              
        orr     r11, r14, r12                                                                                
                      
		;b = ~b;	
        mvn   r11, r11  
		;b = ~b;
        mvn   r12, r12   
		ldr		r8,[sp,#36]	;count 		      		                        
		;((uint32_t*)Dst)[0] = b;
        str     r11, [r9],r3                                        
                   
		subs	r8, r8, #1     
		str		r8,[sp,#36]	;count                                
        bne   little_loop_do_ARMv4                             

        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} ;C_ITransform4x4_DCOnly_Add
        ENDP
		EXPORT	C_ITransform4x4_DCOnly_Add11 
|MaskCarry|  
		DCD	0x80808080		;uint32_t MaskCarry = 0x80808080U;


;__asm void RV_FASTCALL C_Intra16x16ITransform4x4(I32 *in)
;{
C_Intra16x16ITransform4x4	PROC
        STMFD    sp!,{r4-r8}
		MOV		 r8,#13
        MOV      r1,r0      	   ;pin = in
        MOV      r2,#4      	   ;r2 loop var
Ln272
		LDR      r5,[r1, #0]       ;r5 = a0
		LDR      r6,[r1, #8]       ;r6 = a2

        SUBS     r2,r2,#1		   ;decrement loop var        			

        ADD		 r3,r5,r6 	       ;b0 = a0 + a2
		MUL   r3,r8,r3
        SUB		 r4,r5,r6 	       ;b1 = a0 - a2
		MUL   r4,r8,r4
		
		LDR      r5,[r1, #4]       ;r5 = a1
		LDR      r6,[r1, #12]      ;r6 = a3
		
        RSB      r7,r5,r5,LSL #3   ;b2 = a1 * 7
        ADD      r12,r6,r6,LSL #4  ;Temp(r12) = a3*17
        SUB		 r7,r7,r12         ;b2(r7) = b2 - Temp
        ADD      r5,r5,r5,LSL #4   ;b3 = a1*17
        RSB      r6,r6,r6,LSL #3   ;Temp= a3 * 7
        ADD		 r12,r5,r6	       ;b3(r12) = b3 + Temp		
        
        ADD		 r5,r3,r12	       ;pin[0] = b0 + b3
        ADD		 r6,r4,r7		   ;pin[1] = b1 + b2
        STR		 r5,[r1], #4
        STR		 r6,[r1], #4
        SUB		 r5,r4,r7	       ;pin[2] = b1 - b2
        SUB      r6,r3,r12		   ;pin[3] = b0 - b3	        
        STR		 r5,[r1], #4
        STR		 r6,[r1], #4        

        BNE      Ln272
        
        MOV      r2,#4
        MOV      r1,r0      	   ;pin = in
Ln368
		LDR      r5,[r1,#0x00]     ;r5 = a0
		LDR      r6,[r1,#0x20]     ;r6 = a2

        SUBS     r2,r2,#1		   ;decrement loop var        			

        ADD		 r3,r5,r6 	       ;b0 = a0 + a2
		MUL   r3,r8,r3
        SUB		 r4,r5,r6 	       ;b1 = a0 - a2
		MUL   r4,r8,r4
		
		LDR      r5,[r1, #0x10]    ;r5 = a1
		LDR      r6,[r1, #0x30]    ;r6 = a3
		
        RSB      r7,r5,r5,LSL #3   ;b2 = a1 * 7
        ADD      r12,r6,r6,LSL #4  ;Temp(r12) = a3*17
        SUB		 r7,r7,r12         ;b2(r7) = b2 - Temp
        ADD      r5,r5,r5,LSL #4   ;b3 = a1*17
        RSB      r6,r6,r6,LSL #3   ;Temp = a3 * 7
        ADD		 r12,r5,r6	       ;b3(r12) = b3 + Temp		
        
        ADD		 r5,r3,r12	       ;pin[0]  = ((b0 + b3) * 3) >> 11
        RSB      r5,r5,r5,LSL #2
        MOV      r5,r5,ASR #11
        ADD		 r6,r4,r7		   ;pin[4]  = ((b1 + b2) * 3) >> 11
        RSB      r6,r6,r6,LSL #2
        MOV      r6,r6,ASR #11
        STR		 r5,[r1], #4
        STR		 r6,[r1, #12]
        SUB		 r5,r4,r7	       ;pin[8]  = ((b1 - b2) * 3) >> 11
        RSB      r5,r5,r5,LSL #2
        MOV      r5,r5,ASR #11
        SUB      r6,r3,r12		   ;pin[12] = ((b0 - b3) * 3) >> 11        
        RSB      r6,r6,r6,LSL #2
        MOV      r6,r6,ASR #11
        STR		 r5,[r1, #28]
        STR		 r6,[r1, #44]

        BNE      Ln368

        LDMFD    sp!,{r4-r8}
        MOV 	 pc,lr	
;}
		ENDP
		EXPORT	C_Intra16x16ITransform4x4

;__asm void RV_FASTCALL C_ITransform4x4_DCOnly(I32 *in)
;{
C_ITransform4x4_DCOnly	PROC

		STR      lr,[sp,#-4]!
        LDR      r1,[r0,#0]
        MOV      r2,#0xa9
        MUL      r1,r2,r1
        ADD      r1,r1,#0x200
        MOV      r1,r1,ASR #10
		MOV		 r2,r1
		MOV		 r3,r1
		MOV		 lr,r1
		STMIA    r0!,{r1-r3,lr}
		STMIA    r0!,{r1-r3,lr}
		STMIA    r0!,{r1-r3,lr}
		STMIA    r0!,{r1-r3,lr}
		LDR      pc,[sp],#4        
;}
		ENDP
		EXPORT	C_ITransform4x4_DCOnly
		


;{
C_DirectB_InterpolMB_Lu		PROC
		STMFD	sp!,{r4-r10,lr}
		LDR		r4,[sp,#36]			;bSkipInterp
		LDR		lr,[sp,#32]			;uPitch		
		CMP		r4,#0
		BEQ		Lu2
		MOV		r12,#0x10
Lu1		LDMIA	r1,{r4-r7}
		SUBS	r12,r12,#1	
		ADD		r1,r1,r3
		STMIA	r0,{r4-r7}
		ADD		r0,r0,lr
		BGT		Lu1
		BEQ		Lu7

Lu2		LDR		r4,[sp,#40]			;uFwdRatio		
		LDR		r5,[sp,#44]			;uBwdRatio
		MOV		r12,#0x10
		CMP		r4,r5
		BEQ		Lu5

		SUB		lr,lr,#0x10			;uPitch - 16
		SUB		r3,r3,#0x10			;uPredPitch - 16
		MOV		r10,#0x01
		MOV		r10,r10,LSL #13
Lu3		MOV		r6,#0x10
Lu4		LDRB	r7,[r2],#0x01		;lpFutr[i]
		LDRB	r8,[r1],#0x01		;lpPrev[i]
		SUBS	r6,r6,#0x04
		MLA		r7,r4,r7,r10
		MLA		r8,r5,r8,r7

		LDRB	r7,[r2],#0x01		;lpFutr[i]
		MOV		r9,r8,ASR #14		
		LDRB	r8,[r1],#0x01		;lpPrev[i]		
		MLA		r7,r4,r7,r10
		STRB	r9,[r0],#0x01
		MLA		r8,r5,r8,r7	
				
		LDRB	r7,[r2],#0x01		;lpFutr[i]
		MOV		r9,r8,ASR #14			
		LDRB	r8,[r1],#0x01		;lpPrev[i]		
		MLA		r7,r4,r7,r10
		STRB	r9,[r0],#0x01
		MLA		r8,r5,r8,r7	
				
		LDRB	r7,[r2],#0x01		;lpFutr[i]
		MOV		r9,r8,ASR #14			
		LDRB	r8,[r1],#0x01		;lpPrev[i]		
		MLA		r7,r4,r7,r10
		STRB	r9,[r0],#0x01
		MLA		r8,r5,r8,r7
		MOV		r9,r8,ASR #14			
		STRB	r9,[r0],#0x01		
		BGT		Lu4
		ADD		r0,r0,lr
		ADD		r1,r1,r3
		ADD		r2,r2,r3
		SUBS	r12,r12,#1
		BGT		Lu3
		BEQ		Lu7

Lu5		MOV		r4,#0x80
		ORR		r4,r4,r4,LSL #0x08
		ORR		r4,r4,r4,LSL #0x10		;0x80808080
		MOV		r12,#0x10
Lu6		LDR		r5,[r1,#0x00]
		LDR		r6,[r2,#0x00]
		LDR		r7,[r1,#0x04]
		LDR		r8,[r2,#0x04]
		ADDS	r9,r5,r6
		EOR		r10,r5,r6
		ANDS	r10,r4,r10,RRX
		ADC		r9,r10,r9,LSR #1
		STR		r9,[r0,#0x00]
		ADDS	r9,r7,r8
		EOR		r10,r7,r8
		ANDS	r10,r4,r10,RRX
		ADC		r9,r10,r9,LSR #1
		STR		r9,[r0,#0x04]
		LDR		r5,[r1,#0x08]
		LDR		r6,[r2,#0x08]
		LDR		r7,[r1,#0x0c]
		LDR		r8,[r2,#0x0c]
		ADDS	r9,r5,r6
		EOR		r10,r5,r6
		ANDS	r10,r4,r10,RRX
		ADC		r9,r10,r9,LSR #1
		STR		r9,[r0,#0x08]
		ADDS	r9,r7,r8
		EOR		r10,r7,r8
		ANDS	r10,r4,r10,RRX
		ADC		r9,r10,r9,LSR #1
		STR		r9,[r0,#0x0c]
		ADD		r0,r0,lr
		ADD		r1,r1,r3
		ADD		r2,r2,r3
		SUBS	r12,r12,#1
		BGT		Lu6			
Lu7		LDMFD	sp!,{r4-r10,pc}
;};//C_DirectB_InterpolMB_Lu
		ENDP
		EXPORT	C_DirectB_InterpolMB_Lu

;{
C_DirectB_InterpolMB_Cr	PROC
		STMFD	sp!,{r4-r10,lr}
		LDR		r4,[sp,#36]			;bSkipInterp
		LDR		lr,[sp,#32]			;uPitch		
		CMP		r4,#0
		BEQ		Lc2
		SUB		r3,r3,#0x04			;uPredPitch - 4
		SUB		lr,lr,#0x04			;uPitch - 4
		MOV		r12,#0x08
Lc1		LDR 	r4,[r1],#0x04
		LDR 	r5,[r1],r3		
		STR		r4,[r0],#0x04
		STR		r5,[r0],lr
		LDR 	r4,[r1],#0x04
		LDR 	r5,[r1],r3		
		STR		r4,[r0],#0x04
		STR		r5,[r0],lr
		LDR 	r4,[r1],#0x04
		LDR 	r5,[r1],r3		
		STR		r4,[r0],#0x04
		STR		r5,[r0],lr
		LDR 	r4,[r1],#0x04
		LDR 	r5,[r1],r3		
		STR		r4,[r0],#0x04
		STR		r5,[r0],lr
		SUBS	r12,r12,#4
		BGT		Lc1
		BEQ		Lc7

Lc2		LDR		r4,[sp,#40]			;uFwdRatio		
		LDR		r5,[sp,#44]			;uBwdRatio
		MOV		r12,#0x08
		CMP		r4,r5
		BEQ		Lc5

		SUB		lr,lr,#0x08			;uPitch - 8
		SUB		r3,r3,#0x08			;uPredPitch - 8
		MOV		r10,#0x01
		MOV		r10,r10,LSL #13
Lc3		MOV		r6,#0x08
Lc4		LDRB	r7,[r2],#0x01		;lpFutr[i]
		LDRB	r8,[r1],#0x01		;lpPrev[i]
		SUBS	r6,r6,#0x04
		MLA		r7,r4,r7,r10
		MLA		r8,r5,r8,r7

		LDRB	r7,[r2],#0x01		;lpFutr[i]
		MOV		r9,r8,ASR #14		
		LDRB	r8,[r1],#0x01		;lpPrev[i]		
		MLA		r7,r4,r7,r10
		STRB	r9,[r0],#0x01
		MLA		r8,r5,r8,r7	
				
		LDRB	r7,[r2],#0x01		;lpFutr[i]
		MOV		r9,r8,ASR #14			
		LDRB	r8,[r1],#0x01		;lpPrev[i]		
		MLA		r7,r4,r7,r10
		STRB	r9,[r0],#0x01
		MLA		r8,r5,r8,r7	
				
		LDRB	r7,[r2],#0x01		;lpFutr[i]
		MOV		r9,r8,ASR #14			
		LDRB	r8,[r1],#0x01		;lpPrev[i]		
		MLA		r7,r4,r7,r10
		STRB	r9,[r0],#0x01
		MLA		r8,r5,r8,r7
		MOV		r9,r8,ASR #14			
		STRB	r9,[r0],#0x01		
		BGT		Lc4
		ADD		r0,r0,lr
		ADD		r1,r1,r3
		ADD		r2,r2,r3
		SUBS	r12,r12,#1
		BGT		Lc3
		BEQ		Lc7

Lc5		MOV		r4,#0x80
		ORR		r4,r4,r4,LSL #0x08
		ORR		r4,r4,r4,LSL #0x10		;0x80808080
		MOV		r12,#0x08
Lc6		LDR		r5,[r1,#0x00]
		LDR		r6,[r2,#0x00]
		LDR		r7,[r1,#0x04]
		LDR		r8,[r2,#0x04]
		ADDS	r9,r5,r6
		EOR		r10,r5,r6
		ANDS	r10,r4,r10,RRX
		ADC		r9,r10,r9,LSR #1
		STR		r9,[r0,#0x00]
		ADDS	r9,r7,r8
		EOR		r10,r7,r8
		ANDS	r10,r4,r10,RRX
		ADC		r9,r10,r9,LSR #1
		STR		r9,[r0,#0x04]
		ADD		r0,r0,lr
		ADD		r1,r1,r3
		ADD		r2,r2,r3

		LDR		r5,[r1,#0x00]
		LDR		r6,[r2,#0x00]
		LDR		r7,[r1,#0x04]
		LDR		r8,[r2,#0x04]
		ADDS	r9,r5,r6
		EOR		r10,r5,r6
		ANDS	r10,r4,r10,RRX
		ADC		r9,r10,r9,LSR #1
		STR		r9,[r0,#0x00]
		ADDS	r9,r7,r8
		EOR		r10,r7,r8
		ANDS	r10,r4,r10,RRX
		ADC		r9,r10,r9,LSR #1
		STR		r9,[r0,#0x04]

		ADD		r0,r0,lr
		ADD		r1,r1,r3
		ADD		r2,r2,r3
		SUBS	r12,r12,#2
		BGT		Lc6			
Lc7		LDMFD	sp!,{r4-r10,pc}
;}//C_DirectB_InterpolMB_Cr
		ENDP
		EXPORT	C_DirectB_InterpolMB_Cr