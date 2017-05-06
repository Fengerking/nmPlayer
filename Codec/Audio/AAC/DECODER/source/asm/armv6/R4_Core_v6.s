	INCLUDE LDRDATA.H

	EXPORT	|AAD_srtidata510|

	AREA	|.text|, CODE, READONLY

|AAD_srtidata510| PROC
	stmdb     sp!, {r4 - r11, lr}
	sub       sp, sp, #60                     

	cmp     r1, #0                            
	beq     |$M2859|                            
|$M2853|                         
|$M2854|                          
	mov     r5, r2, lsl #1  
	mov     r8, r0          
	mov     r7, r1  
	mov     r5, r5, lsl #2   
	cmp     r1, #0          
	rsbeq   r12, r5, r5, lsl #2 
	beq     |$M2858|              
                         
	rsb     r12, r5, r5, lsl #2    
	str     r1, [sp, #8]         
	str     r0, [sp, #4]          
|$M2855|                        
	mov     r6, r3        
	mov     r0, r2        
	cmp     r2, #0        
	beq     |$M2857|         
                        
	str     r7, [sp, #24]   
	str     r0, [sp, #28]   
	str     r3, [sp, #20]   
	str     r2, [sp, #16]   
	str     r12, [sp, #12]   
|$M2856|                          
	ldrd    r0, [r8, #0]    
	;ldrd    r2, [r6, #0]   
	LOAD_DATA	r2, r3, r6, #0
	add     r7, r8, r5 	 
 	str     r0, [sp, #32]  
	str     r1, [sp, #36]  
	ldrd    r10, [r7, #0] 
 	add     r8, r2, r3, lsl #1   	 
	add     r9, r10, r11
	add     r7, r7, r5 	           
	smmul   r12, r3, r9      
	ldrd	r0, [r7, #0] 
	smmls   r10, r8, r10, r12      
    smmla	r11, r2, r11, r12 
	;ldrd	r2, [r6, #8]
	LOAD_DATA	r2, r3, r6, #8
	str 	r10, [sp, #40]
	str 	r11, [sp, #44]
	add     r9, r0, r1 
	add     r8, r2, r3, lsl #1 
	smmul   r12, r3, r9 
	add     r7, r7, r5 
	smmls   r10, r8, r0, r12
	smmla	r11, r2, r1, r12 	
	;ldrd	r8, [r6, #16]
	LOAD_DATA	r8, r9, r6, #16
	ldrd	r0, [r7, #0]	
	add     r2, r8, r9, lsl #1 
	add     r3, r0, r1 
	smmul   r12, r3, r9
	smmls   r0, r2, r0, r12
	add     r6, r6, #24
	smmla	r1, r8, r1, r12
	ldr 	r2, [sp, #32]
	ldr 	r3, [sp, #36]
	ldr 	r8, [sp, #40]
	ldr 	r9, [sp, #44]
	mov		r4, r2, asr #2
	mov     r12, r3, asr #2
	add		r2, r4, r8
	sub		r8, r4, r8	
	sub	    r3, r12, r9
	add     r9, r12, r9
	rsb		r4, r10, #0
	sub		r12, r4, r0
	add		r0, r4, r0
	sub		r4, r1, r11
	add     r1, r1, r11 		
	add		r10, r2, r4
	add		r11, r3, r0
	strd	r10, [r7, #0]	
	sub		r2, r2, r4
	sub		r3, r3, r0	
	sub		r7, r7, r5
	sub		r10, r8, r12
	sub		r11, r9, r1
	strd	r10, [r7, #0]	
	add     r0, r8, r12
	sub		r7, r7, r5
	add     r1, r9, r1
	strd	r2, [r7, #0]
	mvn     r10, #0   
	sub		r7, r7, r5
	ldr     r4, [sp, #28] 
	strd	r0, [r7, #0]
	adds    r4, r4, r10  
	add		r8, r7, #8   
	str     r4, [sp, #28] 
	bne     |$M2856| 
	                         
	ldr     r7, [sp, #24] 
	ldr     r3, [sp, #20] 
	ldr     r2, [sp, #16] 
	ldr     r12, [sp, #12] 
|$M2857|                         
	add     r8, r8, r12    
	mvn     r0, #0         
	adds    r7, r7, r0     
	bne     |$M2855|           
                        
	ldr     r1, [sp, #8] 
	ldr     r0, [sp, #4]  
|$M2858|                        
	add     r3, r12, r3    
	mov     r2, r2, lsl #2 
	movs    r1, r1, asr #2 
	bne     |$M2854|          
                        
|$M2859|                        
	add     sp, sp, #60                  
	ldmia   sp!, {r4 - r11, pc}
		
	ENDP  ; |AAD_srtidata510|
	END