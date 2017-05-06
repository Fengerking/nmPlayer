
	@AREA	.text, CODE, READONLY
	.section .text
	.global	AAD_srtidata510

AAD_srtidata510: @PROC
	stmdb    sp!, {r4 - r11, lr}

	cmp     r1, #0                            
	beq     M2859                            
M2854:
	mov     r5, r2, lsl #1  
	@VMOV		D10, r10, r11
	mov     r8, r0          
	mov     r7, r1  
	mov     r5, r5, lsl #2   
	cmp     r1, #0          
	rsbeq   r12, r5, r5, lsl #2 
	beq     M2858              
                         
	rsb     r12, r5, r5, lsl #2    
M2855:
	mov     r6, r3 
	mov     r4, r2  
	cmp     r2, #0        
	beq     M2857         
  
M2856:
	@ar = xptr[0]@
	@ai = xptr[1]@
	VLD2.I32		{D0, D1, D2, D3}, [r8]
	
	VLD2.I32		{D28, D29, D30, D31}, [r6]!
	add				r8, r8, r5
	VLD2.I32		{D4, D5, D6,D7}, [r8]					@ br = xptr[0]@ bi = xptr[1]@
	VSHL.S32		Q13, Q15, #1
	VADD.S32		Q12, Q2, Q3								@ br + bi
	VADD.S32		Q13, Q14, Q13							@ wd = ws + 2*wi@
	VQDMULH.S32		Q12, Q15, Q12							@ tr = MULHIGH(wi, br + bi)@
	VQDMULH.S32		Q10, Q13, Q2							@ MULHIGH(wd, br)
	VQDMULH.S32		Q11, Q14, Q3							@ MULHIGH(ws, bi)
	VSUB.S32		Q2, Q10, Q12							@ br = MULHIGH(wd, br) - tr@
	VADD.S32		Q3, Q11, Q12							@ bi = MULHIGH(ws, bi) + tr@
	
	add				r8, r8, r5	
	VSHR.S32		Q0, Q0, #2	
	
	VLD2.I32		{D28, D29, D30, D31}, [r6]!
	VLD2.I32		{D8, D9, D10, D11}, [r8]
	VSHL.S32		Q13, Q15, #1
	VADD.S32		Q12, Q4, Q5
	VADD.S32		Q13, Q14, Q13
	VQDMULH.S32		Q12, Q15, Q12
	VQDMULH.S32		Q10, Q13, Q4
	VQDMULH.S32		Q11, Q14, Q5
	VSUB.S32		Q4, Q10, Q12
	VADD.S32		Q5, Q11, Q12
	
	add				r8, r8, r5
	VSHR.S32		Q1, Q1, #2
	
	VLD2.I32		{D28, D29, D30, D31}, [r6]!
	VLD2.I32		{D12, D13, D14, D15}, [r8]
	VSHL.S32		Q13, Q15, #1
	VADD.S32		Q12, Q6, Q7
	VADD.S32		Q13, Q14, Q13
	VQDMULH.S32		Q12, Q15, Q12
	VQDMULH.S32		Q10, Q13, Q6
	VQDMULH.S32		Q11, Q14, Q7
	VSUB.S32		Q6, Q10, Q12
	VADD.S32		Q7, Q11, Q12
	
	VSUB.S32		Q8, Q0, Q2									@ ar = (tr >> 2) - br@
	VSUB.S32		Q9, Q1, Q3									@ ai = (ti >> 2) - bi@
	VADD.S32		Q10, Q0, Q2									@ br = (tr >> 2) + br@
	VADD.S32		Q11, Q1, Q3									@ bi = (ti >> 2) + bi@
	
	VADD.S32		Q12, Q4, Q6									@ cr = tr + dr@
	VSUB.S32		Q13, Q7, Q5									@ ci = di - ti@
	VSUB.S32		Q14, Q4, Q6									@ dr = tr - dr@
	VADD.S32		Q15, Q5, Q7									@ di = di + ti@
	
	VADD.S32		Q0, Q8, Q13									@ xptr[0] = ar + ci@
	VADD.S32		Q1, Q9, Q14									@ xptr[1] = ai + dr@
	VST2.I32		{D0, D1, D2, D3}, [r8]
	
	VSUB.S32		Q2, Q10, Q12								@ xptr[0] = br - cr@
	sub				r8, r8, r5									@ xptr -= step@
	VSUB.S32		Q3, Q11, Q15								@ xptr[1] = bi - di@
	VST2.I32		{D4, D5, D6, D7}, [r8]
		
	VSUB.S32		Q0, Q8, Q13									@ xptr[0] = ar - ci@
	sub				r8, r8, r5									@ xptr -= step@
	VSUB.S32		Q1, Q9, Q14									@ xptr[1] = ai - dr@
	VST2.I32		{D0, D1, D2, D3}, [r8]
		
	VADD.S32		Q2, Q10, Q12								@ xptr[0] = br + cr@
	sub				r8, r8, r5									@ xptr -= step@
	VADD.S32		Q3, Q11, Q15								@ xptr[1] = bi + di@
	VST2.I32		{D4, D5, D6, D7}, [r8]!
		
	subs    r4, r4, #4 
	bne     M2856 
	                         
M2857:
	add     r8, r8, r12    
	subs    r7, r7, #1     
	bne     M2855           
                        
M2858:
	add     r3, r12, r3    
	mov     r2, r2, lsl #2 
	movs    r1, r1, asr #2 
	bne     M2854          
                        
M2859:
        
	ldmia   sp!, {r4 - r11, pc}
		
	@ENDP  @ AAD_srtidata510
	.END
