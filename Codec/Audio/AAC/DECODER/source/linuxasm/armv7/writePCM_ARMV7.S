
	@AREA	.text, CODE, READONLY
	.section .text

	.global	writePCM_V7

writePCM_V7: @PROC
	stmdb   sp!, {r4 - r11, lr}     
	@VMOV		D7, r9, r10
	mov     r11, #0            
	cmp     r3, #2
	bne     T4214
          
T4213:
	VLD1.I32		{D0 , D1 , D2 , D3 }, [r0]!
	VLD1.I32		{D4 , D5 , D6 , D7 }, [r0]!
	VLD1.I32		{D8 , D9 , D10, D11}, [r1]!
	VLD1.I32		{D12, D13, D14, D15}, [r1]!
	
	@VMOV			r4, r5, D0
	@VMOV			r6, r7, D1
	@VMOV			r8, r9, D8
	@VMOV			r10, r12, D9
	
	VQRSHRN.S32		D16, Q0, #3
	VQRSHRN.S32		D17, Q1, #3
	VQRSHRN.S32		D18, Q4, #3
	VQRSHRN.S32		D19, Q5, #3
	VQRSHRN.S32		D20, Q2, #3
	VQRSHRN.S32		D21, Q3, #3
	VQRSHRN.S32		D22, Q6, #3
	VQRSHRN.S32		D23, Q7, #3
	
	add     		r11, r11, #16
	
	@VMOV			r4, r5, D16
	@VMOV			r6, r7, D17
	@VMOV			r8, r9, D18
	@VMOV			r10, r12, D19 
	
	VST2.I16		{D16, D17, D18, D19}, [r2]!
	VST2.I16		{D20, D21, D22, D23}, [r2]!
		
	cmp     r11, #1024 
  	blt     T4213
    
 	b		T4215       
T4214:
	
	VLD1.I32		{D0 , D1 , D2 , D3 }, [r0]!
	VLD1.I32		{D4 , D5 , D6 , D7 }, [r0]!
	VLD1.I32		{D8 , D9 , D10, D11}, [r0]!
	VLD1.I32		{D12, D13, D14, D15}, [r0]!
	
	VQRSHRN.S32		D16, Q0, #3
	VQRSHRN.S32		D17, Q1, #3
	VQRSHRN.S32		D18, Q2, #3
	VQRSHRN.S32		D19, Q3, #3
	VQRSHRN.S32		D20, Q4, #3
	VQRSHRN.S32		D21, Q5, #3
	VQRSHRN.S32		D22, Q6, #3
	VQRSHRN.S32		D23, Q7, #3
	
	add     		r11, r11, #32 
	
	VST1.I16		{D16, D17, D18, D19}, [r2]!
	VST1.I16		{D20, D21, D22, D23}, [r2]!	
		
	cmp     r11, #1024 
  	blt     T4214	                 
T4215:
	ldmia     sp!, {r4 - r11, pc}

	@ENDP  @ writePCM_V7

	.END

