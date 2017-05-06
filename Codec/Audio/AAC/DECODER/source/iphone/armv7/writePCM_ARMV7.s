	.text
	.align 4

	.globl  _writePCM_V7

_writePCM_V7: @PROC
	stmdb   sp!, {r4 - r11, lr}     
	mov     r11, #0            
	cmp     r3, #2
	bne     T4214
          
T4213:
	vld1.i32		{d0 , d1 , d2 , d3 }, [r0]!
	vld1.i32		{d4 , d5 , d6 , d7 }, [r0]!
	vld1.i32		{d8 , d9 , d10, d11}, [r1]!
	vld1.i32		{d12, d13, d14, d15}, [r1]!
	
	vqrshrn.s32		d16, q0, #3
	vqrshrn.s32		d17, q1, #3
	vqrshrn.s32		d18, q4, #3
	vqrshrn.s32		d19, q5, #3
	vqrshrn.s32		d20, q2, #3
	vqrshrn.s32		d21, q3, #3
	vqrshrn.s32		d22, q6, #3
	vqrshrn.s32		d23, q7, #3
	
	add     		r11, r11, #16
	

	vst2.i16		{d16, d17, d18, d19}, [r2]!
	vst2.i16		{d20, d21, d22, d23}, [r2]!
		
	cmp     r11, #1024 
  	blt     T4213
    
 	b		T4215       
T4214:
	
	vld1.i32		{d0 , d1 , d2 , d3 }, [r0]!
	vld1.i32		{d4 , d5 , d6 , d7 }, [r0]!
	vld1.i32		{d8 , d9 , d10, d11}, [r0]!
	vld1.i32		{d12, d13, d14, d15}, [r0]!
	
	vqrshrn.s32		d16, q0, #3
	vqrshrn.s32		d17, q1, #3
	vqrshrn.s32		d18, q2, #3
	vqrshrn.s32		d19, q3, #3
	vqrshrn.s32		d20, q4, #3
	vqrshrn.s32		d21, q5, #3
	vqrshrn.s32		d22, q6, #3
	vqrshrn.s32		d23, q7, #3
	
	add     		r11, r11, #32 
	
	vst1.i16		{d16, d17, d18, d19}, [r2]!
	vst1.i16		{d20, d21, d22, d23}, [r2]!	
		
	cmp     r11, #1024 
  	blt     T4214	                 
T4215:
	ldmia     sp!, {r4 - r11, pc}



