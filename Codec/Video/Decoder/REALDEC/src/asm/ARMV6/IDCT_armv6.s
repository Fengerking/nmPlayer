
	AREA ITRFRM, CODE, READONLY
ARMV6_Transform4x4_Add		PROC
       
        STMFD    sp!,{r4-r12,lr}
		MOV		 lr,#13
        MOV      r4,r0      	 ;pin = in
        MOV      r9,#4      	 ;r9 loop var
Ln16
		LDRSH    r5,[r4,#0x00]     ;r5 = a0
		LDRSH    r7,[r4,#0x20]     ;r7 = a2
		LDRSH    r6,[r4,#0x10]     ;r6 = a1
		LDRSH    r8,[r4,#0x30]     ;r8 = a3
		
		PKHBT	 r5,r5,r7,LSL #16
		PKHBT    r6,r6,r8,LSL #16
		
		LDR      r7,W13_13
		LDR      r8,W7_17
		
		SMUAD    r10,r5,r7
		SMUAD    r11,r6,r8
		ADD      r12,r10,r11
		STR		 r12,[r4],#4 
		
		SUB      r10,r10,r11
		STR      r10,[r4,#44] 
		
		SMUSD    r10,r5,r7
		SMUSDX   r11,r6,r8
		ADD      r12,r10,r11
		STR      r12,[r4,#12]
		
		SUB      r10,r10,r11
		STR      r10,[r4,#28]      
        
        SUBS    r9,r9,#1	 ;decrement loop var  
        BNE      Ln16
        
        MOV      r9,#4
        MOV      r4,r0      	   ;pin = in
Ln116
		LDMIA	 r4!,{r5-r8}	   ;a0,a1,a2,a3
		;ADD		 r4,r4,#16     ;pin += 4			

        ADD		 r10,r5,r7 	       ;b0 = a0 + a2		
		MUL      r10,lr,r10
        SUB		 r11,r5,r7 	       ;b1 = a0 - a2		
		MUL      r11,lr,r11
		
        RSB      r7,r6,r6,LSL #3    ;b2 = a1 * 7
        ADD      r12,r8,r8,LSL #4   ;Temp(r12) = a3*17
        SUB		 r7,r7,r12			;b2(r9) = b2 - Temp
        ADD      r5,r6,r6,LSL #4    ;b3 = a1*17
        RSB      r6,r8,r8,LSL #3    ;Temp= a3 * 7
        ADD		 r12,r5,r6			;b3(r12) = b3 + Temp		
        
        ADD		 r5,r10,r12	        ;pin[0] = (b0 + b3 + 0x200) >> 10
        LDRB	 r8,[r3]
        ADD      r5,r5,#0x200
        ;MOV      r5,r5,ASR #10
        ADD		 r5,r8,r5,ASR #10      
              
		ADD		 r6,r11,r7			 ;pin[1] = (b1 + b2 + 0x200) >> 10
		LDRB	 r8,[r3, #1]
        ADD      r6,r6,#0x200
        ;MOV     r6,r6,ASR #10 
        ADD		 r6,r8,r6,ASR #10
                               
        SUB		 r7,r11,r7			 ;pin[2] = (b1 - b2 + 0x200) >> 10
        LDRB	 r8,[r3, #2]
        ADD      r7,r7,#0x200
        ;MOV      r7,r7,ASR #10
        ADD		 r7,r8,r7,ASR #10
        
        SUB      r8,r10,r12		     ;pin[3] = (b0 - b3 + 0x200) >> 10  
        LDRB	 r10,[r3, #3] 	        
        ADD      r8,r8,#0x200		
        ;MOV     r8,r8,ASR #10
        ADD		 r8,r10,r8,ASR #10
        LDR      r10,[sp,#40]
        ADD		 r3,r3,r10           ;pPredSrc += uPredPitch
        
        USAT     r5, #8, r5
        USAT     r6, #8, r6
        USAT     r7, #8, r7
        USAT     r8, #8, r8
              
        ORR		  r5, r5, r8, LSL #24
        ORR		  r5, r5, r6, LSL #8
        ORR		  r5, r5, r7, LSL #16
        STR		  r5, [r1]
        ADD       r1, r1, r2	    ;pDst += uDestStride
        SUBS      r9,r9,#1	        ;decrement loop var    
        BNE       Ln116

        LDMFD    sp!,{r4-r12,pc}
		ENDP
		EXPORT	ARMV6_Transform4x4_Add		
		
	AREA ITRFRM, CODE, READONLY
ARMV6_ITransform4x4_DCOnly_Add		PROC
        stmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r14}
        
		ldr	   r4, [r0]
		mov	   lr, #13
		mul	   r4, lr, r4
		mul	   r4, lr, r4
		add	   r4, r4, #0x200
		mov	   r4, r4,asr #10

		ldr	   r9, [sp,#36]	;uPredPitch
        cmp    r4, #0       ;if (v==0)                       
        bne    outCopyBlock4x4_asm_ARMv6     
		             
CopyBlock4x4_asm_ARMv6
		ldr     r4, [r3], r9        
 		ldr     r6, [r3], r9 
 		ldr     r8, [r3], r9        
 		ldr     r10,[r3]	
        str     r4, [r1], r2 
        str     r6, [r1], r2 
        str     r8, [r1], r2 
        str     r10,[r1] 
		    		  				                                       
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15}

outCopyBlock4x4_asm_ARMv6

		blt     little_begin_ARMv6 

big_begin_ARMv6_ARMv6	                                       
        usat    r4, #8, r4;real_TCK	                                       
        orr     r12, r4, r4, lsl #8            
        orr     r5, r12, r12, lsl #16    
		        
 		ldr     r4, [r3], r9        
 		ldr     r6, [r3], r9   			             
		uqadd8	r8, r4, r5
		uqadd8	r10,r6, r5
        str     r8, [r1], r2 
        str     r10,[r1], r2 

 		ldr     r4, [r3], r9        
 		ldr     r6, [r3]   			             
		uqadd8	r8, r4, r5
		uqadd8	r10,r6, r5
        str     r8, [r1], r2 
        str     r10,[r1]											                                             
   
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15}   
	
little_begin_ARMv6
                      
        rsb     r12, r4, #0                       
        usat    r12, #8, r12;real_TCK                               
        orr     r12, r12, r12, lsl #8            
        orr     r5, r12, r12, lsl #16            

 		ldr     r4, [r3], r9        
 		ldr     r6, [r3], r9   			             
		uqsub8	 r8, r4, r5
		uqsub8	 r10, r6, r5
        str     r8, [r1], r2 
        str     r10, [r1], r2 

 		ldr     r4, [r3], r9        
 		ldr     r6, [r3]   			             
		uqsub8	 r8, r4, r5
		uqsub8	 r10, r6, r5
        str     r8, [r1], r2 
        str     r10, [r1] 

        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} ;C_ITransform4x4_DCOnly_Add
        ENDP
		EXPORT	ARMV6_ITransform4x4_DCOnly_Add 
		
ARMV6_Intra16x16ITransform4x4	PROC     
        ;STMFD    sp!,{r4-r8}
        STMFD    sp!,{r4-r12,lr}
		MOV		 r8,#13
        MOV      r1,r0      	   ;pin = in
        MOV      r2,#4      	   ;r2 loop var
Ln272
		LDR      r5,[r1, #0]       ;r5 = a0
		LDR      r6,[r1, #8]       ;r6 = a2

        SUBS     r2,r2,#1		   ;decrement loop var        			

        ADD		 r3,r5,r6 	       ;b0 = a0 + a2
		MUL      r3,r8,r3
        SUB		 r4,r5,r6 	       ;b1 = a0 - a2
		MUL      r4,r8,r4
		
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
		MUL      r3,r8,r3
        SUB		 r4,r5,r6 	       ;b1 = a0 - a2
		MUL      r4,r8,r4
		
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

        ;LDMFD    sp!,{r4-r8}
        LDMFD    sp!,{r4-r12,pc}
        ;MOV 	 pc,lr	

		ENDP
		EXPORT	ARMV6_Intra16x16ITransform4x4		

ALIGN 
W13_13		dcd	0x000D000D
W7_17		dcd	0x00070011	;huwei 20080917 16bit idct
		
	END