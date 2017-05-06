	AREA	|_rdata|, DATA, READONLY
	EXPORT	|jpeg_dctquantfwd_ARMv4|
	AREA	|_text|, CODE, READONLY

jpeg_dctquantfwd_ARMv4
        stmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r14}
        sub     r13, r13, #68   
        mov     r12, #0								; r12 = bc  
        str     r1, [r13, #8]                     
        str     r0, [r13, #4] 

				                                               
_BIG_WHILE
        add     r5, r0, r12, lsl #2  				; r4 = r3 = dataptr = outblock->WorkBlock[bc];              
        ldr     r3, [r5, #4]						                    
        mov     r2, #8                            
        str     r12, [r13, #12]						; [r13, #12] = bc                           
        str     r3, [r13, #16]  	

		mov		r4, #181
		mov		r9, #139
        mov     r14, #78                          
        orr     r14, r14, #256		; r14 = 334  

_RPOCESS_ROWS
        ldrsh   r5, [r3, #0]		; r5 =  dataptr[0]                  
        ldrsh   r7, [r3, #14]		; r7 =  dataptr[7]                     
        ldrsh   r8, [r3, #2] 		; r8 =  dataptr[1]                    
        ldrsh   r6, [r3, #12] 		; r6 =  dataptr[6]          
        add     r0, r5, r7			; r0 = tmp0                       
        sub     r11, r5, r7 		; r11 = tmp7                      
        add     r5, r8, r6  		; r5 = tmp1                     
        sub     r8, r8, r6  		; r8 = tmp6

        ldrsh   r1, [r3, #4]  		; r1 =  dataptr[2] 		                    
        ldrsh   r10, [r3, #10]  	; r10 =  dataptr[5]                  
        ldrsh   r6, [r3, #6]  		; r6 =  dataptr[3]                    
        ldrsh   r7, [r3, #8]    	; r7 =  dataptr[4] 
		          
        add     r12, r1, r10    	; r12 = tmp2                    
        sub     r10, r1, r10    	; r10 = tmp5                     
        add     r1, r6, r7   		; r1 = tmp3                        
        sub     r7, r6, r7    		; r7 = tmp4 
		                      
        add     r6, r0, r1			; r6 = tmp10 = tmp0 + tmp3;                      
        sub     r1, r0, r1 			; r1 = tmp13 = tmp0 - tmp3;                    
        add     r0, r5, r12			; r0 = tmp11 = tmp0 + tmp3;                        
        sub     r12, r5, r12 		; r12 = tmp12 = tmp0 + tmp3;  
		                    
        add     r5, r6, r0   
        sub     r6, r6, r0  
        add     r0, r1, r12         ; r0 = tmp12 + tmp13     
        strh    r5, [r3, #0]		; dataptr[0] = tmp10 + tmp11;  		        
        mul		r12, r0, r4 		  		                                                    
        strh    r6, [r3, #8]		; dataptr[4] = tmp10 - tmp11;   
        add     r5, r10, r7			; tmp10 = tmp4 + tmp5;

        add     r6, r10, r8         ; tmp11 = tmp5 + tmp6;
		           
        sub     r0, r1, r12, asr #8                
        add     r1, r1, r12, asr #8 
		mov		r7, #98		               
        strh    r1, [r3, #4]		; dataptr[2] = tmp13 + z1;                      


		mul		r1, r5, r9

        add     r8, r8, r11   		; r8 = tmp12 = tmp6 + tmp7;
									; r8 = tmp12
        sub     r12, r5, r8			; r12 = tmp10 - tmp12
        strh    r0, [r3, #12]		; dataptr[2] = tmp13 - z1; 			                   
        mul		r0, r12, r7
        mul     r5, r8, r14         ; (tmp12 * 334)						 	               
        mov     r1, r1, asr #8      ; r1 = ((tmp10 * 139) >> 8) 
        mul		r12, r6, r4				             
        add     r1, r1, r0, asr #8  ;; r1 = z2 = ((tmp10 * 139) >> 8) + z5;  z5 = (((tmp10 - tmp12) * 98) >> 8); 
		               
        mov     r5, r5, asr #8      ; r5 = ((tmp12 * 334) >> 8)		                 
        add     r0, r5, r0, asr #8  ;; r0 = z4 = ((tmp12 * 334) >> 8) + z5;
		             
        sub     r5, r11, r12, asr #8  ;; r5 = z13 = tmp7 - z3;  z3 = ((tmp11 * 181) >> 8);                             
        add     r12, r11, r12, asr #8 ;; r12 = z11 = tmp7 + z3;  z3 = ((tmp11 * 181) >> 8); 
		  		
        add     r6, r1, r5 
        sub     r5, r5, r1  
        add     r7, r0, r12  				  
        sub     r8, r12, r0  		          
        strh    r6, [r3, #10]		  ; dataptr[5] = z13 + z2;                                    
        strh    r5, [r3, #6]		  ; dataptr[3] = z13 - z2;                                        
        strh    r7, [r3, #2]		  ; dataptr[1] = z11 + z4;                                        
        strh    r8, [r3, #14]		  ; dataptr[7] = z11 - z4;                     
        add     r3, r3, #16  
        subs     r2, r2, #1		                                            
        bne     _RPOCESS_ROWS                     

        ldr     r14, [r13, #12]
        ldr     r0, [r13, #4]		                            
        ldr     r1, [r13, #8]   
        ldr     r12, [r13, #16] 
        add     r2, r0, r14, lsl #1
;stall
        ldrsh   r2, [r2, #28]		     
        mov     r4, #0          	      
        mov     r5, #8  
        str     r5, [r13, #48]		        
        ldr     r2, [r1, +r2, lsl #2]
;stall ;stall		           
        add     r3, r2, #64           
        add     r2, r2, #192
		          
_COL_QUANT                          
        ldrsh   r0, [r12, #0]   
        ldrsh   r1, [r12, #112] 
        ldrsh   r5, [r12, #16]  
        ldrsh   r8, [r12, #96]
        add     r6, r0, r1            
        ldrsh   r7, [r12, #32]        
        ldrsh   r9, [r12, #80]
		sub     r0, r0, r1
		add     r14, r5, r8           
        sub     r1, r5, r8
        ldrsh   r8, [r12, #48]
        add     r5, r7, r9                
        sub     r7, r7, r9                
        ldrsh   r11, [r12, #64]            
        str     r7, [r13, #32]                    
        sub     r9, r14, r5
        add     r7, r14, r5 
		add     r5, r8, r11                      
        sub     r14, r8, r11                     
        ldrh    r10, [r3, #0]                  
		str     r14, [r13, #24]                   
		add     r14, r6, r5                       
        sub     r11, r6, r5                                          
        add     r8, r14, r7 
        ldrh    r5, [r2, #0] 
        eor     r6, r8, r8, asr #31               
        sub     r6, r6, r8, asr #31
        cmp     r6, r10  
        mulge     r5, r8, r5		                          
        strlth  r4, [r12, #0]                     
;        blt     _L1_9                             
;_L1_8                                           
;_L1_9                         
        sub     r14, r14, r7
        ldrh    r6, [r2, #64] 
        movge     r5, r5, asr #16                   
        strgeh    r5, [r12, #0] 		                    
		ldrh    r5, [r3, #64]                                          
        eor     r7, r14, r14, asr #31  
        sub     r7, r7, r14, asr #31              
        cmp     r7, r5                                                 
;        blt     _L1_12                            
;_L1_11                                            
;_L1_12                        
        add     r7, r11, r9                       
		mov		r5, #181
        mulge     r14, r14, r6
        mul		r7, r5, r7
        strlth  r4, [r12, #64]
        ldrh    r5, [r2, #32]
        movge     r14, r14, asr #16   
        add     r6, r11, r7, asr #8		             
        strgeh    r14, [r12, #64]
        ldrh    r14, [r3, #32]		               
        eor     r8, r6, r6, asr #31          
        sub     r8, r8, r6, asr #31               
        cmp     r8, r14   
        mulge     r14, r6, r5 		                        
        strlth  r4, [r12, #32]                     
;        blt     _L1_15                           
;_L1_14                                        
;_L1_15   
        sub     r5, r11, r7, asr #8                       
        ldrh    r6, [r3, #96]  
        eor     r7, r5, r5, asr #31 		                  
        movge     r14, r14, asr #16 
        ldrh    r8, [r2, #96]		              
        strgeh    r14, [r12, #32]		                                
        sub     r7, r7, r5, asr #31
        cmp     r7, r6  
        mulge     r14, r5, r8		                          
        strlth  r4, [r12, #96]                     
;        blt     _L1_18                           
;_L1_17                                       
;_L1_18                                            
        ldr     r6, [r13, #32]                    
        ldrh    r10, [r3, #80]  
        movge     r8, r14, asr #16                 		          
        ldr     r14, [r13, #24]		        
        strgeh    r8, [r12, #96]
        ldrh    r11, [r2, #80]		                                       
        add     r8, r6, r1                       
        add     r14, r6, r14		 
        add     r6, r1, r0                       
        mov		r7, #98
		mov		r5, #139
        sub     r9, r14, r6
		mul		r14, r5, r14
		mul		r9, r7, r9
        mov     r7, #78
        orr     r7, r7, #256
        mov     r14, r14, asr #8
        mul     r6, r6, r7
        add     r5, r14, r9, asr #8 
		mov		r14, #181
        ldrh    r1, [r2, #48]   
        mul		r7, r8, r14
		mov     r6, r6, asr #8	
        add     r6, r6, r9, asr #8 
		ldrh    r9, [r3, #48]  
        sub     r14, r0, r7, asr #8              
        add     r0, r0, r7, asr #8               
        add     r7, r5, r14
        sub     r14, r14, r5 		                  
        eor     r8, r7, r7, asr #31 
		sub     r8, r8, r7, asr #31              
        cmp     r8, r10 
        mulge     r7, r7, r11		        
        strlth  r4, [r12, #80]                     
;        blt     _L1_21                          
;_L1_20                                                                   
;_L1_21      
;        sub     r14, r14, r5                    
;        ldrh    r1, [r2, #48]                                                          
        eor     r5, r14, r14, asr #31            
        sub     r5, r5, r14, asr #31 
        movge     r7, r7, asr #16
        ldrh    r8, [r3, #16]                    
        strgeh    r7, [r12, #80] 		             
        cmp     r5, r9   
        mulge     r1, r14, r1		                 
;        blt     _L1_24                            
;_L1_23                                       
;_L1_24    
        add     r5, r6, r0                                    
        strlth  r4, [r12, #48]                     

        eor     r7, r5, r5, asr #31          
        movge     r14, r1, asr #16
        ldrh    r1, [r2, #16]                                  
        sub     r7, r7, r5, asr #31           

        strgeh    r14, [r12, #48]
        cmp     r7, r8    
        mulge     r1, r5, r1		            
        strlth  r4, [r12, #16]                    
;        blt     _L1_27                         
;_L1_26                                         
;_L1_27    
        sub     r0, r0, r6                      
        ldrh    r14, [r3, #112] 
        movge     r5, r1, asr #16		           
        ldrh    r1, [r2, #112]   
        strgeh    r5, [r12, #16] 		                            
        eor     r5, r0, r0, asr #31           
        sub     r5, r5, r0, asr #31           
        cmp     r5, r14
        mulge     r5, r0, r1 
        strlth  r4, [r12, #112]               
;        blt     _L1_30                      
;_L1_29                                          
;_L1_30                         
        ldr     r0, [r13, #48]                
        add     r3, r3, #2                  
        movge     r5, r5, asr #16
        add     r2, r2, #2
        strgeh    r5, [r12, #112]
        add     r12, r12, #2              		                   
        subs     r0, r0, #1                     
        str     r0, [r13, #48]                                           
        bne     _COL_QUANT                           

        ldr     r0, [r13, #4]                      
        ldr     r12, [r13, #12]
;stall	 		               
        ldrh    r2, [r0, #0] 
;stall	                  
        add     r12, r12, #1  
;stall		                        
        cmp     r12, r2                           
        blt     _BIG_WHILE  
                       
        add     r13, r13, #68                     
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15}
       END
       