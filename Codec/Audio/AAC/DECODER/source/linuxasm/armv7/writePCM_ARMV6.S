
	@AREA	.text, CODE, READONLY
	.section .text

	.global	writePCM_V6

writePCM_V6: @PROC
	stmdb   sp!, {r4 - r11, lr}
	mov     r7, #0x7F, 24
	orr     r10, r7, #0xFF       
	mov     r11, #0            
    mov     r12, r2, lsl #1            
T3213:
    ldrd    r2, [r0]  
	ldrd    r4, [r0, #8]     
	ldrd	r6, [r0, #16]
	ldrd	r8,	[r0, #24]
	add     r11, r11, #8      
    add     r2, r2, #4      
	add     r3, r3, #4            
    ssat	r2, #16, r2, asr #3
	ssat	r3, #16, r3, asr #3
	strh    r2, [r1] 
	add     r4, r4, #4      
	add     r5, r5, #4    
	strh    r3, [r1, r12] 
	ssat	r4, #16, r4, asr #3
	add		r1, r1, r12, lsl #1
	ssat	r5, #16, r5, asr #3
	strh    r4, [r1] 
	add     r6, r6, #4      
	add     r7, r7, #4    
	strh    r5, [r1, r12]
	ssat	r6, #16, r6, asr #3
	add		r1, r1, r12, lsl #1
	ssat	r7, #16, r7, asr #3
	strh    r6, [r1] 
	add     r8, r8, #4      
	add     r9, r9, #4    
	strh    r7, [r1, r12]
	ssat	r8, #16, r8, asr #3
	add		r1, r1, r12, lsl #1
	ssat	r9, #16, r9, asr #3
	strh    r8, [r1] 
	strh    r9, [r1, r12]

	cmp     r11, #1024 
	add		r0, r0, #32 
	add     r1, r1, r12, lsl #1 
    blt     T3213        
T3214:
	ldmia     sp!, {r4 - r11, pc}

	@ENDP  @ writePCM_V6

	.END

