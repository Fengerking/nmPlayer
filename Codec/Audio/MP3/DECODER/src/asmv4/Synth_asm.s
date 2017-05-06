	EXPORT	|SynthStereo|
	AREA	|.text|, CODE, READONLY
|SynthStereo| PROC
	stmdb   sp!, {r4 - r11, lr}
	sub     sp, sp, #0x8	
	
	str		r0, [sp, #4]	
	mov		r0, r2	
	;	MC0S(0)          
	;	MC0S(1)          
    ldrd    r10, [r0, #0]		; c1 = *coef;		coef++;		c2 = *coef;		coef++;
	ldrd    r2,  [r1, #0]		; vLo = *(vb1+(x));
	ldrd    r4, [r1, #88]  	    ; vHi = *(vb1+(23-(x))); 
	ldrd    r6, [r1, #128]		; vLo = *(vb1+32+(x));
	mul     r2, r10, r2			; sum1L += vLo * c1;
	ldrd    r8, [r1, #216] 		; vHi = *(vb1+32+(23-(x)));
	mul     r5, r11, r5			; sum1L -= vHi * c2;
	add     r2, r2, #16384  
	mul     r6, r10, r6			; sum1R += vLo * c1;
	mul	    r9, r11, r9         ; sum1R -= vHi * c2;
	add     r6, r6, #16384		
	sub		r12, r2, r5
	ldrd    r10, [r0, #8] 
	sub		r14, r6, r9	  
	mla		r12, r3, r10, r12
	mul		r2, r4, r11
	mla		r14, r7, r10, r14
	mul		r5, r8, r11
	sub		r12, r12, r2
	sub		r14, r14, r5 	 

	;	MC0S(2)          
	;	MC0S(3) 
    ldrd    r10, [r0, #16]       
	ldrd    r2,  [r1, #8]		; vLo = *(vb1+(x));  
	ldrd    r4, [r1, #80]  	    ; vHi = *(vb1+(23-(x)));  
	ldrd    r6, [r1, #136]		; vLo = *(vb1+32+(x));
	mla     r12, r2, r10, r12	; sum1L += vLo * c1;
	ldrd    r8, [r1, #208]		; vHi = *(vb1+32+(23-(x)));
	mul     r5, r11, r5			; sum1L -= vHi * c2;
	mla     r14, r6, r10, r14   ; sum1R += vLo * c1;      
	mul	    r9, r11, r9			; sum1R -= vHi * c2; 
	sub		r12, r12, r5
	ldrd    r10, [r0, #24]  
	sub		r14, r14, r9	 
	mla		r12, r3, r10, r12
	mul		r2, r4, r11
	mla		r14, r7, r10, r14
	mul		r5, r8, r11
	sub		r12, r12, r2
	sub		r14, r14, r5 
	
	;	MC0S(4)          
	;	MC0S(5) 
    ldrd    r10, [r0, #32]         
	ldrd    r2,  [r1, #16]    
	ldrd    r4, [r1, #72]  	     
	ldrd    r6, [r1, #144]  
	mla     r12, r2, r10, r12   
	ldrd    r8, [r1, #200] 
	mul     r5, r11, r5
	mla     r14, r6, r10, r14         
	mul	    r9, r11, r9
	sub		r12, r12, r5
	ldrd    r10, [r0, #40] 
	sub		r14, r14, r9	  
	mla		r12, r3, r10, r12
	mul		r2, r4, r11
	mla		r14, r7, r10, r14
	mul		r5, r8, r11
	sub		r12, r12, r2
	sub		r14, r14, r5 
	
	;	MC0S(6)          
	;	MC0S(7) 
    ldrd    r10, [r0, #48]         
	ldrd    r2,  [r1, #24]    
	ldrd    r4, [r1, #64]  	     
	ldrd    r6, [r1, #152]  
	mla     r12, r2, r10, r12   
	ldrd    r8, [r1, #192] 
	mul     r5, r11, r5 
	mla     r14, r6, r10, r14    
	mul	    r9, r11, r9
	ldr		r6, [sp, #4]
	sub		r12, r12, r5
	ldrd    r10, [r0, #56]  
	sub		r14, r14, r9 
	mla		r12, r3, r10, r12
	mul		r2, r4, r11
	mla		r14, r7, r10, r14
	mul		r5, r8, r11
	sub		r12, r12, r2
	sub		r14, r14, r5 	 

	;*(pcm + 0) = ClipToShort(sum1L, (21-CSHIFT + DEF_NFRACBITS));
	;*(pcm + 1) = ClipToShort(sum1R, (21-CSHIFT + DEF_NFRACBITS));
	ssat	r8, #16, r12, asr #15 
	ssat	r9, #16, r14, asr #15 

	add		r4, r0, #1024
	pkhbt	r8, r8, r9, lsl #16   
	add		r5, r1, #4096		
	str     r8, [r6, #0] 	

	;MC1S(0)
	;MC1S(1)
	ldrd    r10, [r4, #0]         
	ldrd    r2,  [r5, #0]    
	ldrd    r8,  [r5, #128]  
	mul     r2, r10, r2   
	mul     r8, r10, r8
	add     r12, r2, #16384  
	add     r14, r8, #16384 
	mla		r12, r3, r11, r12
	mla		r14, r9, r11, r14
	
	;MC1S(2)
	;MC1S(3)	   	    
	ldrd    r10, [r4, #8]         
	ldrd    r2,  [r5, #8]    
	ldrd    r8,  [r5, #136]  
	mla     r12, r2, r10, r12 
	mla     r14, r8, r10, r14
	mla		r12, r3, r11, r12	
	mla		r14, r9, r11, r14
	
	;MC1S(4)
	;MC1S(5)	   	    
	ldrd    r10, [r4, #16]         
	ldrd    r2,  [r5, #16]    
	ldrd    r8,  [r5, #144]  
	mla     r12, r2, r10, r12 
	mla     r14, r8, r10, r14
	mla		r12, r3, r11, r12	
	mla		r14, r9, r11, r14
	
	;MC1S(6)
	;MC1S(7)	   	    
	ldrd    r10, [r4, #24]         
	ldrd    r2,  [r5, #24]    
	ldrd    r8,  [r5, #152]  
	mla     r12, r2, r10, r12  
	mla     r14, r8, r10, r14
	mla		r12, r3, r11, r12	  
	mla		r14, r9, r11, r14   
	
	;*(pcm + 0) = ClipToShort(sum1L, (21-CSHIFT + DEF_NFRACBITS));
	;*(pcm + 1) = ClipToShort(sum1R, (21-CSHIFT + DEF_NFRACBITS));
	ssat	r8, #16, r12, asr #15 
	ssat	r9, #16, r14, asr #15 

	add     r7, r0, #64 
	pkhbt	r8, r8, r9, lsl #16   
	mov     r4, #15  
	str     r8, [r6, #64] 
	add     r5, r6, #4       	
	add     r6, r1, #256    

|$M1493|
	;	MC2S(0)
	ldrd    r10, [r7, #0]               
	ldr     r0, [r6, #0]                
	ldr     r1, [r6, #92]               
	ldr     r2, [r6, #128]             
	mul     r8, r10, r0 
	mul		r12, r11, r1	                
	mul     r9, r10, r1  
	add     r8, r8, #16384	 	
	mla		r14, r11, r0, r9     	             
 	sub		r12, r8, r12    
	add     r9, r9, #16384
	mul		r8, r10, r2
	ldr     r3, [r6, #220]
	mul		r9, r11, r2
	add     r8, r8, #16384
	mul		r0, r11, r3	
	add     r9, r9, #16384 
	sub		r8, r8, r0
	mla		r9, r3, r10, r9

	;	MC2S(1)
	ldrd    r10, [r7, #8]               
	ldr     r0, [r6, #4]                
	ldr     r1, [r6, #88]               
	ldr     r2, [r6, #132]             
	mla     r12, r10, r0, r12                 
	mul		r3, r11, r1
	mla     r14, r10, r1, r14 	
	sub		r12, r12, r3    
	mla		r14, r11, r0, r14
	mla		r8, r10, r2, r8
	ldr     r3, [r6, #216]
	mla		r9, r11, r2, r9
	mul		r0, r11, r3	
	mla		r9, r3, r10, r9
	sub		r8, r8, r0

	;	MC2S(2)
	ldrd    r10, [r7, #16]               
	ldr     r0, [r6, #8]                
	ldr     r1, [r6, #84]               
	ldr     r2, [r6, #136]             
	mla     r12, r10, r0, r12  
	mul		r3, r11, r1	               
	mla     r14, r10, r1, r14  
	sub		r12, r12, r3
	mla		r14, r11, r0, r14     
	mla		r8, r10, r2, r8
	ldr     r3, [r6, #212]
	mla		r9, r11, r2, r9
	mul		r0, r11, r3	
	mla		r9, r3, r10, r9
	sub		r8, r8, r0

	;	MC2S(3)
	ldrd    r10, [r7, #24]               
	ldr     r0, [r6, #12]                
	ldr     r1, [r6, #80]               
	ldr     r2, [r6, #140]             
	mla     r12, r10, r0, r12   
	mul		r3, r11, r1	              
	mla     r14, r10, r1, r14  
	sub		r12, r12, r3
	mla		r14, r11, r0, r14     
	mla		r8, r10, r2, r8
	ldr     r3, [r6, #208]
	mla		r9, r11, r2, r9
	mul		r0, r11, r3	
	mla		r9, r3, r10, r9
	sub		r8, r8, r0

	;	MC2S(4)
	ldrd    r10, [r7, #32]               
	ldr     r0, [r6, #16]                
	ldr     r1, [r6, #76]               
	ldr     r2, [r6, #144]             
	mla     r12, r10, r0, r12   
	mul		r3, r11, r1	              
	mla     r14, r10, r1, r14  
	sub		r12, r12, r3  
	mla		r14, r11, r0, r14     
	mla		r8, r10, r2, r8
	ldr     r3, [r6, #204]
	mla		r9, r11, r2, r9
	mul		r0, r11, r3	
	mla		r9, r3, r10, r9
	sub		r8, r8, r0

	;	MC2S(5)
	ldrd    r10, [r7, #40]               
	ldr     r0, [r6, #20]                
	ldr     r1, [r6, #72]               
	ldr     r2, [r6, #148]             
	mla     r12, r10, r0, r12  
	mul		r3, r11, r1	               
	mla     r14, r10, r1, r14  
	sub		r12, r12, r3  
	mla		r14, r11, r0, r14     
	mla		r8, r10, r2, r8
	ldr     r3, [r6, #200]
	mla		r9, r11, r2, r9
	mul		r0, r11, r3	
	mla		r9, r3, r10, r9
	sub		r8, r8, r0

	;	MC2S(6)
	ldrd    r10, [r7, #48]               
	ldr     r0, [r6, #24]                
	ldr     r1, [r6, #68]               
	ldr     r2, [r6, #152]             
	mla     r12, r10, r0, r12   
	mul		r3, r11, r1	              
	mla     r14, r10, r1, r14  
	sub		r12, r12, r3    
	mla		r14, r11, r0, r14     
	mla		r8, r10, r2, r8
	ldr     r3, [r6, #196]
	mla		r9, r11, r2, r9
	mul		r0, r11, r3	
	mla		r9, r3, r10, r9
	sub		r8, r8, r0

	;	MC2S(7)
	ldrd    r10, [r7, #56]               
	ldr     r0, [r6, #28]                
	ldr     r1, [r6, #64]               
	ldr     r2, [r6, #156]             
	mla     r12, r10, r0, r12  
	mul		r3, r11, r1	 
	add     r7, r7, #64               
	mla     r14, r10, r1, r14  
	sub		r12, r12, r3    
	mla		r14, r11, r0, r14     
	mla		r8, r10, r2, r8
	ldr     r3, [r6, #192]
	mla		r9, r11, r2, r9
	add     r6, r6, #256 
	mul		r0, r11, r3	
	mla		r9, r3, r10, r9
	sub		r8, r8, r0            
    
	ssat	r0, #16, r12, asr #15  
	ssat	r1, #16, r8, asr #15  
	ssat	r2, #16, r14, asr #15
	ssat	r3, #16, r9, asr #15 		
	pkhbt	r0, r0, r1, lsl #16 
	mov     r8, r4, lsl #3  
	pkhbt	r2, r2, r3, lsl #16 
	str     r0, [r5, #0]            
	str     r2, [r5, r8] 
		   
	sub     r4, r4, #1 
	add     r5, r5, #4 
	cmp     r4, #0  
	bgt     |$M1493|

|$M1494|	
	add     sp, sp, #0x8
	ldmia   sp!, {r4 - r11, pc}

	ENDFUNC

	END
        
