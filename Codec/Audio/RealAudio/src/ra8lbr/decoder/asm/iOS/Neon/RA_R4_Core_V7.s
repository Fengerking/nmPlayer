        #include "voRADecID.h"
	@AREA	.text, CODE, READONLY
	.text
        .align  4 
	.globl	_R4Core

_R4Core: @PROC
	stmdb    sp!, {r4 - r11, lr}

	cmp     r1, #0                            
	beq     M2859                            
M2854:
	mov     r5, r2, lsl #1  
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
	vld2.i32		{d0, d1, d2, d3}, [r8]
	
	vld2.i32		{d28, d29, d30, d31}, [r6]!
	add				r8, r8, r5
	vld2.i32		{d4, d5, d6,d7}, [r8]					@ br = xptr[0]@ bi = xptr[1]@
	vshl.s32		q13, q15, #1
	vadd.s32		q12, q2, q3								@ br + bi
	vadd.s32		q13, q14, q13							@ wd = ws + 2*wi@
	vqdmulh.s32		q12, q15, q12							@ tr = MULHIGH(wi, br + bi)@
	vqdmulh.s32		q10, q13, q2							@ MULHIGH(wd, br)
	vqdmulh.s32		q11, q14, q3							@ MULHIGH(ws, bi)
	vsub.s32		q2, q10, q12							@ br = MULHIGH(wd, br) - tr@
	vadd.s32		q3, q11, q12							@ bi = MULHIGH(ws, bi) + tr@
	
	add				r8, r8, r5	
	vshr.s32		q0, q0, #2	
	
	vld2.i32		{d28, d29, d30, d31}, [r6]!
	vld2.i32		{d8, d9, d10, d11}, [r8]
	vshl.s32		q13, q15, #1
	vadd.s32		q12, q4, q5
	vadd.s32		q13, q14, q13
	vqdmulh.s32		q12, q15, q12
	vqdmulh.s32		q10, q13, q4
	vqdmulh.s32		q11, q14, q5
	vsub.s32		q4, q10, q12
	vadd.s32		q5, q11, q12
	
	add				r8, r8, r5
	vshr.s32		q1, q1, #2
	
	vld2.i32		{d28, d29, d30, d31}, [r6]!
	vld2.i32		{d12, d13, d14, d15}, [r8]
	vshl.s32		q13, q15, #1
	vadd.s32		q12, q6, q7
	vadd.s32		q13, q14, q13
	vqdmulh.s32		q12, q15, q12
	vqdmulh.s32		q10, q13, q6
	vqdmulh.s32		q11, q14, q7
	vsub.s32		q6, q10, q12
	vadd.s32		q7, q11, q12
	
	vsub.s32		q8, q0, q2									@ ar = (tr >> 2) - br@
	vsub.s32		q9, q1, q3									@ ai = (ti >> 2) - bi@
	vadd.s32		q10, q0, q2									@ br = (tr >> 2) + br@
	vadd.s32		q11, q1, q3									@ bi = (ti >> 2) + bi@
	
	vadd.s32		q12, q4, q6									@ cr = tr + dr@
	vsub.s32		q13, q7, q5									@ ci = di - ti@
	vsub.s32		q14, q4, q6									@ dr = tr - dr@
	vadd.s32		q15, q5, q7									@ di = di + ti@
	
	vadd.s32		q0, q8, q13									@ xptr[0] = ar + ci@
	vadd.s32		q1, q9, q14									@ xptr[1] = ai + dr@
	vst2.i32		{d0, d1, d2, d3}, [r8]
	
	vsub.s32		q2, q10, q12								@ xptr[0] = br - cr@
	sub				r8, r8, r5									@ xptr -= step@
	vsub.s32		q3, q11, q15								@ xptr[1] = bi - di@
	vst2.i32		{d4, d5, d6, d7}, [r8]
		
	vsub.s32		q0, q8, q13									@ xptr[0] = ar - ci@
	sub				r8, r8, r5									@ xptr -= step@
	vsub.s32		q1, q9, q14									@ xptr[1] = ai - dr@
	vst2.i32		{d0, d1, d2, d3}, [r8]
		
	vadd.s32		q2, q10, q12								@ xptr[0] = br + cr@
	sub				r8, r8, r5									@ xptr -= step@
	vadd.s32		q3, q11, q15								@ xptr[1] = bi + di@
	vst2.i32		{d4, d5, d6, d7}, [r8]!
		
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
		
	@ENDP  @ R4Core
	@.END
