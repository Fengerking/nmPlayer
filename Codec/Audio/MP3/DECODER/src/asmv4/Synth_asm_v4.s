	MACRO	
	SSAT15	$Rn, $sign, $maxPos

	mov		$Rn, $Rn, ASR #15
	mov		$sign, $Rn, ASR #31
	cmp		$sign, $Rn, ASR #15
	eorne	$Rn, $sign, $maxPos
	
	MEND	; SSAT15

	EXPORT	|voMP3DecSynthMono|
	AREA	|.text|, CODE, READONLY
|voMP3DecSynthMono| PROC
	stmdb   sp!, {r4 - r11, lr}

	mov		r14, r0	
	mov		r0, r2	
	;	MC0M(0)          
	;	MC0M(1)
	;	MC0M(2)          
	;	MC0M(3)          
    ldr     r10, [r0, #0]		; c1 = *coef;		coef++;		c2 = *coef;		coef++;
	ldr     r11, [r0, #4]		; c1 = *coef;		coef++;		c2 = *coef;		coef++;
	ldr     r2,  [r1, #0]		; vLo = *(vb1+(x));
	ldr     r3,  [r1, #4]		; vLo = *(vb1+(x+1));
	ldr     r4,  [r1, #8]		; vLo = *(vb1+(x+2));
	ldr     r5,  [r1, #12]		; vLo = *(vb1+(x+3));
	ldr     r6, [r1, #80]  	    ; vHi = *(vb1+(23-(x+3)));
	ldr     r7, [r1, #84]  	    ; vHi = *(vb1+(23-(x+2)));
	ldr     r8, [r1, #88]  	    ; vHi = *(vb1+(23-(x+1))); 
	ldr     r9, [r1, #92]  	    ; vHi = *(vb1+(23-(x))); 
	mul     r2, r10, r2			; sum1L += vLo * c1;
	mul     r9, r11, r9			; sum1L -= vHi * c2;  
	ldr     r10, [r0, #8] 
	add     r2, r2, #16384
	ldr     r11, [r0, #12] 
	sub		r12, r2, r9
	mul		r2, r8, r11
	mla		r12, r3, r10, r12
	ldr     r10, [r0, #16]
	sub		r12, r12, r2	 
	ldr     r11, [r0, #20] 
	mla		r12, r4, r10, r12
	mul		r2, r7, r11 
	ldr     r10, [r0, #24]
	sub		r12, r12, r2
	ldr     r11, [r0, #28] 
	mla		r12, r5, r10, r12
	mul		r8, r6, r11 

	;	MC0M(4)          
	;	MC0M(5)
	;	MC0M(6)          
	;	MC0M(7) 
    ldr     r10, [r0, #32]
	ldr     r11, [r0, #36]         
	sub		r12, r12, r8
	ldr     r2,  [r1, #16]   
	ldr     r3,  [r1, #20]   
	ldr     r4,  [r1, #24] 
	ldr     r5,  [r1, #28] 
	ldr     r6,  [r1, #64]
	ldr     r7,  [r1, #68]
	ldr     r8,  [r1, #72]  	     
	ldr     r9,  [r1, #76]  	     
	mla     r12, r2, r10, r12   
	mul     r9, r11, r9
	ldr     r10, [r0, #40] 
	sub		r12, r12, r9
	ldr     r11, [r0, #44]
	mla		r12, r3, r10, r12
	mul		r2, r8, r11
	ldr     r10, [r0, #48]
	sub		r12, r12, r2
	ldr     r11, [r0, #52] 
	mla		r12, r4, r10, r12
	mul		r2, r7, r11
	ldr     r10, [r0, #56] 
	ldr     r11, [r0, #60] 
	sub		r12, r12, r2
	mov		r7, #0x7f00
	mul		r2, r6, r11
	mla		r12, r5, r10, r12
	orr		r7, r7, #0xff	
		
	sub		r12, r12, r2
	add		r0, r0, #1024

	;*(pcm + 0) = ClipToShort(sum1L, (21-CSHIFT + DEF_NFRACBITS));
	;*(pcm + 1) = ClipToShort(sum1R, (21-CSHIFT + DEF_NFRACBITS));
	SSAT15	r12, r9, r7

	add		r1, r1, #4096
	strh    r12, [r14, #0] 	

	ldr     r10, [r0, #0]         
	ldr     r11, [r0, #4]         
	ldr     r2,  [r1, #0]  
	ldr     r3,  [r1, #4]  
	ldr     r4,  [r1, #8]
	ldr     r5,  [r1, #12]  
	ldr     r8,  [r1, #16]  	  
	ldr     r9,  [r1, #20]  	  
	mul     r2, r10, r2   
	mul     r3, r11, r3
	ldr     r10, [r0, #8]
	add     r12, r2, #16384
	ldr     r11, [r0, #12]   
	add		r12, r12, r3
	mul		r2, r10, r4
	mla		r12, r5, r11, r12
	ldr     r10, [r0, #16] 
	ldr     r11, [r0, #20] 
	add		r12, r12, r2
	ldr     r4,  [r1, #24]
	ldr     r5,  [r1, #28]  
	mul		r3, r10, r8
	mla		r12, r9, r11, r12
	ldr     r10, [r0, #24] 
	ldr     r11, [r0, #28]
	add		r12, r12, r3
	mul		r2, r10, r4
	mla		r12, r5, r11, r12
	sub     r0, r0, #960
	add		r12, r12, r2
	mov     r4, #15
		
	;*(pcm + 0) = ClipToShort(sum1L, (21-CSHIFT + DEF_NFRACBITS));
	SSAT15	r12, r9, r7
	sub     r1, r1, #3840   
	strh    r12, [r14, #32] 
	add     r14, r14, #2         

|$M493|
	;	MC2M(0)
	;	MC2M(1)
	ldr     r10, [r0, #0]               
	ldr     r11, [r0, #4]               
	ldr     r2, [r1, #0]    ; vLo = *(vb1+(x));            
	ldr     r3, [r1, #4]    ; vLo = *(vb1+(x));            
	ldr     r8, [r1, #88]   ; vHi = *(vb1+(23-(x)));       
	ldr     r9, [r1, #92]   ; vHi = *(vb1+(23-(x)));         
	mul     r12, r10, r2 
	mul		r6, r11, r9	                
	mul     r5, r10, r9  
	add     r12, r12, #16384
	mla		r5, r11, r2, r5 
	ldr     r10, [r0, #8]
	ldr     r11, [r0, #12]
	add		r5, r5, #16384
	sub		r12, r12, r6  
	mla		r5, r10, r8, r5
	mla		r12, r10, r3, r12
	mla		r5, r11, r3, r5
	mul		r6, r11, r8

	;	MC2M(2)
	;	MC2M(3)
	ldr     r10, [r0, #16]               
	ldr     r11, [r0, #20]  
	sub		r12, r12, r6             
	ldr     r2, [r1, #8]    ; vLo = *(vb1+(x));            
	ldr     r3, [r1, #12]   ; vLo = *(vb1+(x));            
	ldr     r8, [r1, #80]   ; vHi = *(vb1+(23-(x)));         
	ldr     r9, [r1, #84]   ; vHi = *(vb1+(23-(x)));         
	mla     r12, r10, r2, r12                 
	mla     r5, r10, r9, r5  
	mul		r6, r11, r9	
	mla		r5, r11, r2, r5 
	ldr     r10, [r0, #24]
	ldr     r11, [r0, #28]
	sub     r12, r12, r6
	mla		r5, r10, r8, r5
	mla		r12, r10, r3, r12
	mla		r5, r11, r3, r5
	mul		r6, r11, r8

	;	MC2M(4)
	;	MC2M(5)
	ldr     r10, [r0, #32]               
	ldr     r11, [r0, #36] 
	sub		r12, r12, r6
	ldr     r2, [r1, #16]    ; vLo = *(vb1+(x));            
	ldr     r3, [r1, #20]    ; vLo = *(vb1+(x));            
	ldr     r8, [r1, #72]    ; vHi = *(vb1+(23-(x)));         
	ldr     r9, [r1, #76]    ; vHi = *(vb1+(23-(x)));         
	mla     r12, r10, r2, r12                 
	mla     r5, r10, r9, r5  
	mul		r6, r11, r9	
	mla		r5, r11, r2, r5 
	ldr     r10, [r0, #40]
	ldr     r11, [r0, #44]
	sub     r12, r12, r6
	mla		r5, r10, r8, r5
	mla		r12, r10, r3, r12
	mla		r5, r11, r3, r5
	mul		r6, r11, r8

	;	MC2M(6)
	;	MC2M(7)
	ldr     r10, [r0, #48]               
	ldr     r11, [r0, #52]
	sub		r12, r12, r6
	ldr     r2, [r1, #24]    ; vLo = *(vb1+(x));            
	ldr     r3, [r1, #28]    ; vLo = *(vb1+(x));            
	ldr     r8, [r1, #64]    ; vHi = *(vb1+(23-(x)));         
	ldr     r9, [r1, #68]    ; vHi = *(vb1+(23-(x)));         
	mla     r12, r10, r2, r12                 
	mla     r5, r10, r9, r5  
	mul		r6, r11, r9	
	mla		r5, r11, r2, r5 
	ldr     r10, [r0, #56]
	ldr     r11, [r0, #60]
	sub     r12, r12, r6
	mla		r5, r10, r8, r5
	add     r0, r0, #64
	mla		r12, r10, r3, r12
	mla		r5, r11, r3, r5
	mul		r6, r11, r8  
	add     r1, r1, #256	
	sub		r12, r12, r6
				       
	SSAT15	r5, r9, r7    
	SSAT15	r12, r9, r7    
	mov     r6, r4, lsl #2  
	strh    r12, [r14, #0]            
	strh    r5, [r14, r6] 
		   
	sub     r4, r4, #1 
	add     r14, r14, #2 
	cmp     r4, #0  
	bgt     |$M493|

|$M494|	
	ldmia   sp!, {r4 - r11, pc}

	ENDFUNC

	EXPORT	|voMP3DecSynthStereo|
	AREA	|.text|, CODE, READONLY
|voMP3DecSynthStereo| PROC
	stmdb     sp!, {r4 - r11, lr}
	sub       sp, sp, #0x8	
	
	str		  r0, [sp, #4]	
	mov		  r0, r2	
	;	MC0S(0)          
	;	MC0S(1)          
    ldr     r10, [r0, #0]
	ldr     r11, [r0, #4]
	ldr     r2,  [r1, #0]
	ldr     r3,  [r1, #4]    
	ldr     r4, [r1, #88]
	ldr     r5, [r1, #92]  	     
	ldr     r6, [r1, #128]  
	ldr     r7, [r1, #132]  
	mul     r2, r10, r2   
	ldr     r8, [r1, #216]
	ldr     r9, [r1, #220] 	
	mul     r5, r11, r5 
	add     r2, r2, #16384  
	mul     r6, r10, r6	
	mul	    r9, r11, r9        
	add     r6, r6, #16384 
	sub		r12, r2, r5
	ldr     r10, [r0, #8]
	ldr     r11, [r0, #12] 
	sub		r14, r6, r9	  
	mla		r12, r3, r10, r12
	mul		r2, r4, r11
	mla		r14, r7, r10, r14
	mul		r5, r8, r11
	sub		r12, r12, r2
	sub		r14, r14, r5 	 

	;	MC0S(2)          
	;	MC0S(3) 
    ldr     r10, [r0, #16]         
	ldr     r11, [r0, #20]         
	ldr     r2,  [r1, #8]    
	ldr     r3,  [r1, #12]    
	ldr     r4, [r1, #80]  	 
	ldr     r5, [r1, #84]  	     
	ldr     r6, [r1, #136]  
	ldr     r7, [r1, #140]  
	mla     r12, r2, r10, r12 
	ldr     r8, [r1, #208]	
	ldr     r9, [r1, #212]	  
	mul     r5, r11, r5
	mla     r14, r6, r10, r14         
	mul	    r9, r11, r9
	sub		r12, r12, r5
	ldr     r10, [r0, #24]  
	ldr     r11, [r0, #28]  
	sub		r14, r14, r9	 
	mla		r12, r3, r10, r12
	mul		r2, r4, r11
	mla		r14, r7, r10, r14
	mul		r5, r8, r11
	sub		r12, r12, r2
	sub		r14, r14, r5 
	
	;	MC0S(4)          
	;	MC0S(5) 
    ldr     r10, [r0, #32]         
	ldr     r11, [r0, #36]         
	ldr     r2,  [r1, #16]    
	ldr     r3,  [r1, #20]    
	ldr     r4, [r1, #72]  	  
	ldr     r5, [r1, #76]  	     
	ldr     r6, [r1, #144]  
	ldr     r7, [r1, #148]  
	mla     r12, r2, r10, r12   
	ldr     r8, [r1, #200] 
	ldr     r9, [r1, #204] 
	mul     r5, r11, r5
	mla     r14, r6, r10, r14         
	mul	    r9, r11, r9
	sub		r12, r12, r5
	ldr     r10, [r0, #40] 
	ldr     r11, [r0, #44] 
	sub		r14, r14, r9	  
	mla		r12, r3, r10, r12
	mul		r2, r4, r11
	mla		r14, r7, r10, r14
	mul		r5, r8, r11
	sub		r12, r12, r2
	sub		r14, r14, r5 
	
	;	MC0S(6)          
	;	MC0S(7) 
    ldr     r10, [r0, #48]         
	ldr     r11, [r0, #52]         
	ldr     r2,  [r1, #24]    
	ldr     r3,  [r1, #28]    
	ldr     r4, [r1, #64]  	  
	ldr     r5, [r1, #68]  	     
	ldr     r6, [r1, #152]  
	ldr     r7, [r1, #156]  
	mla     r12, r2, r10, r12   
	ldr     r8, [r1, #192] 
	ldr     r9, [r1, #196] 
	mul     r5, r11, r5 
	mla     r14, r6, r10, r14    
	mul	    r9, r11, r9
	ldr		r6, [sp, #4]
	sub		r12, r12, r5
	ldr     r10, [r0, #56]  
	ldr     r11, [r0, #60]  
	sub		r14, r14, r9 
	mla		r12, r3, r10, r12
	mul		r2, r4, r11
	mla		r14, r7, r10, r14
	mul		r5, r8, r11
	mov		r7, #0x7f00
	sub		r12, r12, r2
	orr		r7, r7, #0xff
	sub		r14, r14, r5 
	 

	;*(pcm + 0) = ClipToShort(sum1L, (21-CSHIFT + DEF_NFRACBITS));
	;*(pcm + 1) = ClipToShort(sum1R, (21-CSHIFT + DEF_NFRACBITS));
	SSAT15	r12, r9, r7
	SSAT15	r14, r9, r7	

	add		r4, r0, #1024
	add		r5, r1, #4096		
	strh    r12, [r6, #0] 	
	strh    r14, [r6, #2] 	

	;MC1S(0)
	;MC1S(1)
	ldr     r10, [r4, #0]         
	ldr     r11, [r4, #4]         
	ldr     r2,  [r5, #0]    
	ldr     r3,  [r5, #4]    
	ldr     r8,  [r5, #128]  
	ldr     r12,  [r5, #132]  
	mul     r2, r10, r2   
	mul     r8, r10, r8
	add     r12, r2, #16384  
	add     r14, r8, #16384 
	mla		r12, r3, r11, r12
	mla		r14, r9, r11, r14
	
	;MC1S(2)
	;MC1S(3)	   	    
	ldr     r10, [r4, #8]         
	ldr     r11, [r4, #12]         
	ldr     r2,  [r5, #8]    
	ldr     r3,  [r5, #12]    
	ldr     r8,  [r5, #136]  
	ldr     r9,  [r5, #140]  
	mla     r12, r2, r10, r12 
	mla     r14, r8, r10, r14
	mla		r12, r3, r11, r12	
	mla		r14, r9, r11, r14
	
	;MC1S(4)
	;MC1S(5)	   	    
	ldr     r10, [r4, #16]         
	ldr     r11, [r4, #20]         
	ldr     r2,  [r5, #16]    
	ldr     r3,  [r5, #20]    
	ldr     r8,  [r5, #144]  
	ldr     r9,  [r5, #148]  
	mla     r12, r2, r10, r12 
	mla     r14, r8, r10, r14
	mla		r12, r3, r11, r12	
	mla		r14, r9, r11, r14
	
	;MC1S(6)
	;MC1S(7)	   	    
	ldr     r10, [r4, #24]         
	ldr     r11, [r4, #28]         
	ldr     r2,  [r5, #24]    
	ldr     r3,  [r5, #28]    
	ldr     r8,  [r5, #152]  
	ldr     r9,  [r5, #156]  
	mla     r12, r2, r10, r12  
	mla     r14, r8, r10, r14
	mla		r12, r3, r11, r12	  
	mla		r14, r9, r11, r14   
	
	;*(pcm + 0) = ClipToShort(sum1L, (21-CSHIFT + DEF_NFRACBITS));
	;*(pcm + 1) = ClipToShort(sum1R, (21-CSHIFT + DEF_NFRACBITS));
	SSAT15	r12, r8, r7
	SSAT15	r14, r8, r7	

	mov     r4, #15 
	add     r7, r0, #64  
	strh    r12, [r6, #64] 
	strh    r14, [r6, #66] 
	add     r5, r6, #4       	
	add     r6, r1, #256    
|$M1493|
	;	MC2S(0)
	ldr     r10, [r7, #0]               
	ldr     r11, [r7, #4]               
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
	ldr     r10, [r7, #8]               
	ldr     r11, [r7, #12]               
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
	ldr     r10, [r7, #16]               
	ldr     r11, [r7, #20]               
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
	ldr     r10, [r7, #24] 
	ldr     r11, [r7, #28]               
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
	ldr     r10, [r7, #32] 
	ldr     r11, [r7, #36]               
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
	ldr     r10, [r7, #40] 
	ldr     r11, [r7, #44]               
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
	ldr     r10, [r7, #48]  
	ldr     r11, [r7, #52]               
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
	ldr     r10, [r7, #56]               
	ldr     r11, [r7, #64]  
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
	mov		r1, #0x7f00
	mla		r9, r3, r10, r9
	orr		r1, r1, #0xff
	sub		r8, r8, r0 

	SSAT15	r12, r2, r1
	SSAT15	r8, r2, r1
	SSAT15	r14, r2, r1
	SSAT15	r9, r2, r1	
	mov     r3, r4, lsl #3	    
	strh    r12, [r5, #0] 
	add     r0, r5, r3	
	strh    r8, [r5, #2]            
	strh    r14, [r5, r3]
	strh    r9,  [r0, #2] 
		   
	sub     r4, r4, #1 
	add     r5, r5, #4 
	cmp     r4, #0  
	bgt     |$M1493|

|$M1494|	
	add       sp, sp, #0x8
	ldmia     sp!, {r4 - r11, pc}

	ENDFUNC

	END
        
