	EXPORT	|writePCM_V5E|

	AREA	|.pdata|, PDATA
|$T3212| DCD	|writePCM_V5E|
	DCD	0x40003101

	AREA	|.text|, CODE, ARM

|writePCM_V5E| PROC
	stmdb   sp!, {r4 - r11, lr}
	mov     r7, #0x7F, 24
	orr     r10, r7, #0xFF       
	mov     r11, #0            
    mov     r12, r2, lsl #1            
|$T3213|           
    ldrd    r2, [r0]  
	ldrd    r8, [r0, #8]     
	add     r11, r11, #8      
    add     r2, r2, #4      
	add     r3, r3, #4            
    mov     r5, r2, asr #3   
	mov		r7, r3, asr #3         
    mov     r4, r5, asr #31
	mov     r6, r7, asr #31           
    cmp     r4, r2, asr #18           
    eorne   r5, r4, r10
	
	cmp		r6, r3, asr #18
    strh    r5, [r1]    
	eorne   r7, r6, r10
    ;ldrd    r2, [r0, #8]          
    add     r1, r12, r1	 
	add     r2, r8, #4     
	strh    r7, [r1]  
	add     r3, r9, #4            
    mov     r5, r2, asr #3   
	mov		r7, r3, asr #3    
    add     r1, r12, r1		     
    mov     r4, r5, asr #31
	mov     r6, r7, asr #31           
    cmp     r4, r2, asr #18           
    eorne   r5, r4, r10

	cmp		r6, r3, asr #18
    strh    r5, [r1]    
	eorne   r7, r6, r10
    ldrd    r2, [r0, #16]
	ldrd    r8, [r0, #24]          
    add     r1, r12, r1	 
	add     r2, r2, #4     
	strh    r7, [r1]  
	add     r3, r3, #4            
    mov     r5, r2, asr #3   
	mov		r7, r3, asr #3    
    add     r1, r12, r1		     
    mov     r4, r5, asr #31
	mov     r6, r7, asr #31           
    cmp     r4, r2, asr #18           
    eorne   r5, r4, r10

	cmp		r6, r3, asr #18
    strh    r5, [r1]    
	eorne   r7, r6, r10
    ;ldrd    r2, [r0, #24]          
    add     r1, r12, r1	 
	add     r2, r8, #4     
	strh    r7, [r1]  
	add     r3, r9, #4            
    mov     r5, r2, asr #3   
	mov		r7, r3, asr #3    
    add     r1, r12, r1		     
    mov     r4, r5, asr #31
	mov     r6, r7, asr #31           
    cmp     r4, r2, asr #18           
    eorne   r5, r4, r10
	cmp		r6, r3, asr #18
	    
	strh    r5, [r1]  
	eorne   r7, r6, r10
	add     r0, r0, #32   
	strh    r7, [r1, r12]    

	cmp     r11, #1024  
	add     r1, r1, r12 lsl #1 
    blt     |$T3213|        
|$T3214|                        

	ldmia     sp!, {r4 - r11, pc}
|$M3211|
	ENDP  ; |writePCM_V5E|
