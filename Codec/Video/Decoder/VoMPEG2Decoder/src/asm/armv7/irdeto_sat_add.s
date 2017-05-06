;************************************************************************
;									                                    *
;	VisualOn, Inc Confidential and Proprietary, 2011		            *
;								 	                                    *
;***********************************************************************/

	AREA	|.text|, CODE

	EXPORT Bit16Armv7SatAdd
	
	ALIGN 4
Bit16Armv7SatAdd PROC
   ;r0 = buf, r1 = src, r2 = dst, r3 = buf_stride
    stmdb   sp!, {r4 - r12, lr}
    ldr		r4, [sp, #40]      ;src_stride
    ldr     r5, [sp, #44]      ;dst_stride
    
    mov     r3,  r3, lsl #1
    vld1.64	{q0}, [r0], r3
	vld1.64	{q1}, [r0], r3
	vld1.64	{q2}, [r0], r3
	vld1.64	{q3}, [r0], r3	
	vld1.64	{q4}, [r0], r3
	vld1.64	{q5}, [r0], r3	
	vld1.64	{q6}, [r0], r3	
	vld1.64	{q7}, [r0]	
	
	cmp		r1, #0
	beq		SAT_ADD_NOSRC
	vld1.64 {d16}, [r1],r4 
	vld1.64 {d17}, [r1],r4 
	vld1.64 {d18}, [r1],r4 
	vld1.64 {d19}, [r1],r4 
	vld1.64 {d20}, [r1],r4 
	vld1.64 {d21}, [r1],r4 
	vld1.64 {d22}, [r1],r4 
	vld1.64 {d23}, [r1] 
	
	vaddw.u8	q0, q0, d16
	vaddw.u8	q1, q1, d17
	vaddw.u8	q2, q2, d18
	vaddw.u8	q3, q3, d19
	vaddw.u8	q4, q4, d20
	vaddw.u8	q5, q5, d21
	vaddw.u8	q6, q6, d22
	vaddw.u8	q7, q7, d23	
SAT_ADD_NOSRC 
	vqmovun.s16	d16, q0
	vqmovun.s16	d17, q1
	vqmovun.s16	d18, q2
	vqmovun.s16	d19, q3
	vqmovun.s16	d20, q4
	vqmovun.s16	d21, q5
	vqmovun.s16	d22, q6
	vqmovun.s16	d23, q7		
						
	vst1.64	{d16}, [r2], r5
	vst1.64	{d17}, [r2], r5		
	vst1.64	{d18}, [r2], r5
	vst1.64	{d19}, [r2], r5	
	vst1.64	{d20}, [r2], r5
	vst1.64	{d21}, [r2], r5	
	vst1.64	{d22}, [r2], r5
	vst1.64	{d23}, [r2]	 
    
    ldmia   sp!, {r4 - r12, pc} 

	ENDP