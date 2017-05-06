@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2005		            *
@								 	                                    *
@***********************************************************************/

	@AREA	|.text|, CODE
	 .text
	.align 4

	.globl _MPEG2DEC_VO_Armv7Copy8x8 
	
_MPEG2DEC_VO_Armv7Copy8x8:  @PROC
	ldr		r12, [sp]
MPEG2DEC_VO_Armv7Copy8x8_loop_again:	
	vld1.64  {d0},[r0],r2  
	vld1.64  {d1},[r0],r2  
	vld1.64  {d2},[r0],r2  
	vld1.64  {d3},[r0],r2   	 
	subs r12,r12,#4   
	vst1.64   {d0},[r1],r3 
	vst1.64   {d1},[r1],r3 
	vst1.64   {d2},[r1],r3 
	vst1.64   {d3},[r1],r3
	bne	MPEG2DEC_VO_Armv7Copy8x8_loop_again	
	
	mov	pc, lr
	@ENDP
	
@------------------------------------------
@ArmCopy16x16 no aligment!, only used in Copy()
@------------------------------------------
	.globl _MPEG2DEC_VO_Armv7Copy16x16 
_MPEG2DEC_VO_Armv7Copy16x16:  @PROC
	ldr		r12, [sp]
@	mov		r12, #16
MPEG2DEC_VO_Armv7Copy16x16_loop_again:
	vld1.64  {q2},[r0],r2
	vld1.64  {q4},[r0],r2
	vld1.64  {q6},[r0],r2
	vld1.64  {q8},[r0],r2
	subs r12,r12,#4   
	vst1.64   {q2},[r1],r3
	vst1.64   {q4},[r1],r3
	vst1.64   {q6},[r1],r3
	vst1.64   {q8},[r1],r3
	bne	MPEG2DEC_VO_Armv7Copy16x16_loop_again		
	mov	pc, lr
	@ENDP	
	
	
@------------------------------------------
@ArmInter8x8
@------------------------------------------
	.globl _MPEG2DEC_VO_Armv7Inter8x8 
_MPEG2DEC_VO_Armv7Inter8x8:  @PROC
	ldr		r12, [sp]
@	mov		r12, #8
MPEG2DEC_VO_Armv7Inter8x8_loop_again:
	vld1.8  {d0},[r0],r2  
	vld1.8  {d1},[r0],r2  
	vld1.8  {d2},[r0],r2  
	vld1.8  {d3},[r0],r2  
@	vld1.8  {d4},[r0],r2  
@	vld1.8  {d5},[r0],r2  
@	vld1.8  {d6},[r0],r2  
@	vld1.8  {d7},[r0],r2 
		
	subs	r12, r12, #4   
	vst1.64   {d0},[r1],r3 
	vst1.64   {d1},[r1],r3 
	vst1.64   {d2},[r1],r3 
	vst1.64   {d3},[r1],r3	
@	vst1.64   {d4},[r1],r3 
@	vst1.64   {d5},[r1],r3 
@	vst1.64   {d6},[r1],r3 
@	vst1.64   {d7},[r1],r3	 
	bne	MPEG2DEC_VO_Armv7Inter8x8_loop_again		
	mov	pc, lr
	@ENDP	

@------------------------------------------
@ArmInter2_8x8 
@------------------------------------------
	.globl _MPEG2DEC_VO_Armv7Inter2_8x8 
_MPEG2DEC_VO_Armv7Inter2_8x8:  @PROC
    push     {lr}
	ldr		r12, [sp, #4]
@	mov		r12, #8
MPEG2DEC_VO_Armv7Inter2_8x8_loop_again:
	mov	lr, r1	  
	vld1.8  {d0},[r0],r2
	vld1.8  {d1},[r0],r2
	vld1.8  {d2},[r0],r2
	vld1.8  {d3},[r0],r2

	vld1.64  {d4},[r1],r3
	vld1.64  {d5},[r1],r3
	vld1.64  {d6},[r1],r3
	vld1.64  {d7},[r1],r3  
	subs	r12, r12, #4   
	vrhadd.u8 d0, d0, d4
	vrhadd.u8 d1, d1, d5
	vrhadd.u8 d2, d2, d6
	vrhadd.u8 d3, d3, d7    
	
	vst1.64   {d0},[lr],r3
	vst1.64   {d1},[lr],r3
	vst1.64   {d2},[lr],r3
	vst1.64   {d3},[lr],r3  
	bne	MPEG2DEC_VO_Armv7Inter2_8x8_loop_again		
    pop      {pc} 
	@ENDP	
	
	
Src		.req	r0
Dst		.req	r1
Src_W		.req	r2
Dst_W		.req	r3
Count		.req	r12
@------------------------------------------
@ ArmInter8x8H
@------------------------------------------
	.globl _MPEG2DEC_VO_Armv7Inter8x8H 
_MPEG2DEC_VO_Armv7Inter8x8H:  @PROC
	push   {r14}
	add		R14, Src, #8	
	ldr		Count, [sp, #4]
@	mov		Count, #8
MPEG2DEC_VO_Armv7Inter8x8H_loop:
	pld [Src, Src_W, lsl #2]
	vld1.8 {D0}, [Src], Src_W
	vld1.8 {D1[0]}, [R14], Src_W

	pld [Src, Src_W, lsl #2]
	vld1.8 {D2}, [Src], Src_W	
	vld1.8 {D3[0]}, [R14], Src_W	

	pld [Src, Src_W, lsl #2]
	vld1.8 {D4}, [Src], Src_W
	vld1.8 {D5[0]}, [R14], Src_W

	pld [Src, Src_W, lsl #2]
	vld1.8 {D6}, [Src], Src_W
	vld1.8 {D7[0]}, [R14], Src_W	
			
	vext.u8 D11, D0, D1, #1
	vext.u8 D13, D2, D3, #1	
	vext.u8 D15, D4, D5, #1	
	vext.u8 D17, D6, D7, #1
		
	vrhadd.u8 D10, D0, D11
	vrhadd.u8 D12, D2, D13
	vrhadd.u8 D14, D4, D15	
	vrhadd.u8 D16, D6, D17	
	subs Count, Count, #4		
	vst1.64 {D10}, [Dst], Dst_W
	vst1.64 {D12}, [Dst], Dst_W
	vst1.64 {D14}, [Dst], Dst_W
	vst1.64 {D16}, [Dst], Dst_W
				
	bne	MPEG2DEC_VO_Armv7Inter8x8H_loop		 
	pop   {pc} 	
	@ENDP	
	
	
@------------------------------------------
@ MPEG2DEC_VO_Armv7Inter8x8HRD
@------------------------------------------
	.globl _MPEG2DEC_VO_Armv7Inter8x8HRD 
_MPEG2DEC_VO_Armv7Inter8x8HRD:  @PROC
	push   {r14}
	add		R14, Src, #8	
@	ldr		Count, [sp, #4]
	mov		Count, #8
MPEG2DEC_VO_Armv7Inter8x8HRD_loop:
	pld [Src, Src_W, lsl #2]
	vld1.8 {D0}, [Src], Src_W
	vld1.8 {D1[0]}, [R14], Src_W

	pld [Src, Src_W, lsl #2]
	vld1.8 {D2}, [Src], Src_W	
	vld1.8 {D3[0]}, [R14], Src_W	

	pld [Src, Src_W, lsl #2]
	vld1.8 {D4}, [Src], Src_W
	vld1.8 {D5[0]}, [R14], Src_W

	pld [Src, Src_W, lsl #2]
	vld1.8 {D6}, [Src], Src_W
	vld1.8 {D7[0]}, [R14], Src_W
			
	vext.u8 D11, D0, D1, #1
	vext.u8 D13, D2, D3, #1	
	vext.u8 D15, D4, D5, #1	
	vext.u8 D17, D6, D7, #1
		
	vhadd.u8 D10, D0, D11
	vhadd.u8 D12, D2, D13
	vhadd.u8 D14, D4, D15	
	vhadd.u8 D16, D6, D17	
		
	vst1.64 {D10}, [Dst], Dst_W
	vst1.64 {D12}, [Dst], Dst_W
	vst1.64 {D14}, [Dst], Dst_W
	vst1.64 {D16}, [Dst], Dst_W
	subs Count, Count, #4	
	bne	MPEG2DEC_VO_Armv7Inter8x8HRD_loop
	pop   {pc} 	
	@ENDP		
	
@------------------------------------------
@ MPEG2DEC_VO_Armv7Inter8x8V
@------------------------------------------
	.globl _MPEG2DEC_VO_Armv7Inter8x8V 
_MPEG2DEC_VO_Armv7Inter8x8V:  @PROC
	ldr		Count, [sp]
	vld1.8 {D0}, [Src], Src_W
@	mov		Count, #8		
MPEG2DEC_VO_Armv7Inter8x8V_loop:
	vld1.8 {D1}, [Src], Src_W
	vld1.8 {D2}, [Src], Src_W	
	vld1.8 {D3}, [Src], Src_W	
	vrhadd.u8 D11, D0, D1
	vld1.8 {D0}, [Src], Src_W	
	vrhadd.u8 D12, D1, D2
	vrhadd.u8 D13, D2, D3	
	vrhadd.u8 D14, D3, D0
	subs Count, Count, #4	
	vst1.64 {D11}, [Dst], Dst_W
	vst1.64 {D12}, [Dst], Dst_W		
	vst1.64 {D13}, [Dst], Dst_W
	vst1.64 {D14}, [Dst], Dst_W	
			
@	vld1.8 {D5}, [Src], Src_W		
@	vld1.8 {D6}, [Src], Src_W		
@	vld1.8 {D7}, [Src], Src_W				
@	vld1.8 {D0}, [Src], Src_W	
@	vrhadd.u8 D15, D4, D5		
@	vrhadd.u8 D16, D5, D6	
@	vrhadd.u8 D17, D6, D7	
@	vrhadd.u8 D10, D7, D0
@	vst1.64 {D15}, [Dst], Dst_W
@	vst1.64 {D16}, [Dst], Dst_W	
@	vst1.64 {D17}, [Dst], Dst_W	
@	vst1.64 {D10}, [Dst], Dst_W				
	bgt MPEG2DEC_VO_Armv7Inter8x8V_loop
	mov	pc, lr
	@ENDP	

@------------------------------------------
@ MPEG2DEC_VO_Armv7Inter8x8VRD
@------------------------------------------
	.globl _MPEG2DEC_VO_Armv7Inter8x8VRD 
_MPEG2DEC_VO_Armv7Inter8x8VRD:  @PROC
	ldr		Count, [sp]
	vld1.8 {D0}, [Src], Src_W
@	mov		Count, #8		
MPEG2DEC_VO_Armv7Inter8x8VRD_loop:
	vld1.8 {D1}, [Src], Src_W
	vld1.8 {D2}, [Src], Src_W	
	vld1.8 {D3}, [Src], Src_W
	vhadd.u8 D11, D0, D1
	vld1.8 {D0}, [Src], Src_W	

	vhadd.u8 D12, D1, D2
	vhadd.u8 D13, D2, D3	
	vhadd.u8 D14, D3, D0
	subs Count, Count, #4	
	vst1.64 {D11}, [Dst], Dst_W
	vst1.64 {D12}, [Dst], Dst_W		
	vst1.64 {D13}, [Dst], Dst_W
	vst1.64 {D14}, [Dst], Dst_W	
		
@	subs Count, Count, #4
	bgt MPEG2DEC_VO_Armv7Inter8x8VRD_loop		 
	mov	pc, lr
	@ENDP			
@------------------------------------------
@ MPEG2DEC_VO_Armv7Inter8x8HV
@------------------------------------------
	.globl _MPEG2DEC_VO_Armv7Inter8x8HV 
_MPEG2DEC_VO_Armv7Inter8x8HV:  @PROC
	push   {r14}
	add		R14, Src, #8	
	ldr		Count, [sp, #4]
@	mov		Count, #8
	vld1.8 {D0}, [Src], Src_W
	vld1.8 {D1[0]}, [R14], Src_W	
	vext.u8 D1, D0, D1, #1
	vaddl.u8 Q8, D0, D1
	vmov.u8 D31, #2	
MPEG2DEC_VO_Armv7Inter8x8HV_loop:
	pld [Src, Src_W, lsl #2]
	vld1.8 {D2}, [Src], Src_W	
	vld1.8 {D3[0]}, [R14], Src_W	

	pld [Src, Src_W, lsl #2]
	vld1.8 {D4}, [Src], Src_W
	vld1.8 {D5[0]}, [R14], Src_W

	pld [Src, Src_W, lsl #2]
	vld1.8 {D6}, [Src], Src_W
	vld1.8 {D7[0]}, [R14], Src_W

	pld [Src, Src_W, lsl #2]
	vld1.8 {D8}, [Src], Src_W
	vld1.8 {D9[0]}, [R14], Src_W
							
	vext.u8 D3, D2, D3, #1
	vext.u8 D5, D4, D5, #1
		
	vaddl.u8 Q9, D2, D3
	vaddl.u8 Q10, D4, D5
		
	vaddw.u8 Q9, Q9, D31	
	vadd.u16 Q8, Q8, Q9
	vadd.u16 Q9, Q9, Q10		
	vshrn.u8.u16 D2, Q8, #2
	vshrn.u8.u16 D4, Q9, #2

	vext.u8 D7, D6, D7, #1
	vext.u8 D9, D8, D9, #1
		
	vaddl.u8 Q11, D6, D7
	vaddl.u8 Q8, D8, D9
		
	vaddw.u8 Q11, Q11, D31	
	vadd.u16 Q10, Q10, Q11
	vadd.u16 Q11, Q11, Q8	
	vshrn.u8.u16 D6, Q10, #2
	vshrn.u8.u16 D8, Q11, #2
	subs Count, Count, #4
	
	vst1.64 {D2}, [Dst], Dst_W	
	vst1.64 {D4}, [Dst], Dst_W	
	vst1.64 {D6}, [Dst], Dst_W	
	vst1.64 {D8}, [Dst], Dst_W

	bgt MPEG2DEC_VO_Armv7Inter8x8HV_loop	 
	pop   {pc} 	
	@ENDP	
	

@------------------------------------------
@ MPEG2DEC_VO_Armv7Inter8x8HVRD
@------------------------------------------
	.globl _MPEG2DEC_VO_Armv7Inter8x8HVRD 
_MPEG2DEC_VO_Armv7Inter8x8HVRD:  @PROC
	push   {r14}
	add		R14, Src, #8	
	ldr		Count, [sp, #4]
@	mov		Count, #8
	vld1.8 {D0}, [Src], Src_W
	vld1.8 {D1[0]}, [R14], Src_W		
	vext.u8 D1, D0, D1, #1
	vaddl.u8 Q8, D0, D1
	vmov.u8 D31, #1
MPEG2DEC_VO_Armv7Inter8x8HVRD_loop:
	pld [Src, Src_W, lsl #2]
	vld1.8 {D2}, [Src], Src_W	
	vld1.8 {D3[0]}, [R14], Src_W	

	pld [Src, Src_W, lsl #2]
	vld1.8 {D4}, [Src], Src_W
	vld1.8 {D5[0]}, [R14], Src_W

	pld [Src, Src_W, lsl #2]
	vld1.8 {D6}, [Src], Src_W
	vld1.8 {D7[0]}, [R14], Src_W

	pld [Src, Src_W, lsl #2]
	vld1.8 {D8}, [Src], Src_W
	vld1.8 {D9[0]}, [R14], Src_W
		
	vext.u8 D3, D2, D3, #1
	vext.u8 D5, D4, D5, #1
		
	vaddl.u8 Q9, D2, D3
	vaddl.u8 Q10, D4, D5
		
	vaddw.u8 Q9, Q9, D31	
	vadd.u16 Q8, Q8, Q9
	vadd.u16 Q9, Q9, Q10	
	vshrn.u8.u16 D2, Q8, #2
	vshrn.u8.u16 D4, Q9, #2
	
	vext.u8 D7, D6, D7, #1
	vext.u8 D9, D8, D9, #1
		
	vaddl.u8 Q11, D6, D7
	vaddl.u8 Q8, D8, D9
		
	vaddw.u8 Q11, Q11, D31	
	vadd.u16 Q10, Q10, Q11
	vadd.u16 Q11, Q11, Q8	
	vshrn.u8.u16 D6, Q10, #2
	vshrn.u8.u16 D8, Q11, #2
	
	vst1.64 {D2}, [Dst], Dst_W	
	vst1.64 {D4}, [Dst], Dst_W	
	vst1.64 {D6}, [Dst], Dst_W	
	vst1.64 {D8}, [Dst], Dst_W

	subs Count, Count, #4
	bgt MPEG2DEC_VO_Armv7Inter8x8HVRD_loop	 
	pop   {pc} 
	@ENDP	

@------------------------------------------
@ MPEG2DEC_VO_Armv7Inter2_8x8H
@------------------------------------------
	.globl _MPEG2DEC_VO_Armv7Inter2_8x8H 
_MPEG2DEC_VO_Armv7Inter2_8x8H:  @PROC
	push   {r14}
	add		R14, Src, #8	
	ldr		Count, [sp, #4]
@	mov		Count, #8
	vmov.u8 D31, #1	
MPEG2DEC_VO_Armv7Inter2_8x8H_loop:
	pld [Src, Src_W]
	pld [Src, Src_W, lsl #1]
	pld [Dst, Dst_W]
	pld [Dst, Dst_W, lsl #1]

	vld1.8 {D0}, [Src], Src_W
	vld1.8  {D1[0]}, [R14], Src_W
	vld1.64 {D10}, [Dst]	
	vext.u8 D1, D0, D1, #1	
	vaddl.u8 Q8, D0, D1
	vaddw.u8 Q8, Q8, D31
	vshr.u16 Q8, Q8, #1
	vaddw.u8 Q8, Q8, D31
	vaddw.u8 Q8, Q8, D10	
	vshrn.u8.u16 D10, Q8, #1						
	vst1.64 {D10}, [Dst], Dst_W
	
	vld1.8 {D2}, [Src], Src_W
	vld1.8  {D3[0]}, [R14], Src_W
	vld1.64 {D11}, [Dst]	
	vext.u8 D3, D2, D3, #1	
	vaddl.u8 Q9, D2, D3
	vaddw.u8 Q9, Q9, D31
	vshr.u16 Q9, Q9, #1
	vaddw.u8 Q9, Q9, D31
	vaddw.u8 Q9, Q9, D11	
	vshrn.u8.u16 D11, Q9, #1
	subs Count, Count, #2							
	vst1.64 {D11}, [Dst], Dst_W

	
	bne	MPEG2DEC_VO_Armv7Inter2_8x8H_loop		 
	pop   {pc} 
	@ENDP	
	
@------------------------------------------
@ MPEG2DEC_VO_Armv7Inter2_8x8V
@------------------------------------------
	.globl _MPEG2DEC_VO_Armv7Inter2_8x8V 
_MPEG2DEC_VO_Armv7Inter2_8x8V:  @PROC
	ldr		Count, [sp]
@	mov		Count, #8
	vmov.u8 D13, #1		
	vld1.8 {D0}, [Src], Src_W	
MPEG2DEC_VO_Armv7Inter2_8x8V_loop:
	pld [Src, Src_W]
	pld [Src, Src_W, lsl #1]

	pld [Dst, Dst_W]
	pld [Dst, Dst_W, lsl #1]

	vld1.8 {D2}, [Src], Src_W
	vld1.64 {D3}, [Dst]	
	vaddl.u8 Q5, D0, D2
	vaddw.u8 Q5, Q5, D13
	vshr.u16 Q5, Q5, #1	
	vaddw.u8 Q5, Q5, D13
	vaddw.u8 Q5, Q5, D3	
	vshrn.u8.u16 D3, Q5, #1
	vst1.64 {D3}, [Dst], Dst_W	
	subs Count, Count, #2		
	vld1.8 {D0}, [Src], Src_W
	vld1.64 {D3}, [Dst]	
	vaddl.u8 Q4, D0, D2
	vaddw.u8 Q4, Q4, D13
	vshr.u16 Q4, Q4, #1	
	vaddw.u8 Q4, Q4, D13
	vaddw.u8 Q4, Q4, D3	
	vshrn.u8.u16 D3, Q4, #1
	vst1.64 {D3}, [Dst], Dst_W	
	bgt MPEG2DEC_VO_Armv7Inter2_8x8V_loop	 
	mov	pc, lr	
	@ENDP
	
				
@------------------------------------------
@ MPEG2DEC_VO_Armv7Inter2_8x8HV
@------------------------------------------
	.globl _MPEG2DEC_VO_Armv7Inter2_8x8HV 
_MPEG2DEC_VO_Armv7Inter2_8x8HV:  @PROC
	push   {r14}
	add		R14, Src, #8	
	ldr		Count, [sp, #4]
	vld1.8 {D0}, [Src], Src_W
@	mov		Count, #8	
	vld1.8 {D1[0]}, [R14], Src_W	
	vext.u8 D1, D0, D1, #1
	vaddl.u8 Q4, D0, D1
	vmov.u8 D13, #2
	vmov.u8 D14, #1	
MPEG2DEC_VO_Armv7Inter2_8x8HV_loop:
	pld [Src, Src_W]
	pld [Src, Src_W, lsl #1]

	pld [Dst, Dst_W]
	pld [Dst, Dst_W, lsl #1]
	vld1.8 {D2}, [Src], Src_W
	vld1.8 {D3[0]}, [R14], Src_W	
	vext.u8 D3, D2, D3, #1
	vaddl.u8 Q5, D2, D3
	vaddw.u8 Q5, Q5, D13
	vadd.u16 Q4, Q4, Q5
	vshr.u16 Q4, Q4, #2	
	vld1.64 {D3}, [Dst]	
	vaddw.u8 Q4, Q4, D14
	vaddw.u8 Q4, Q4, D3	
	vshrn.u8.u16 D3, Q4, #1
	vst1.64 {D3}, [Dst], Dst_W		
	vld1.8  {D0}, [Src], Src_W
	vld1.8 {D1[0]}, [R14], Src_W
	vext.u8 D1, D0, D1, #1
	vaddl.u8 Q4, D0, D1
	vadd.u16 Q5, Q5, Q4
	vshr.u16 Q5, Q5, #2	
	vld1.64 {D3}, [Dst]	
	vaddw.u8 Q5, Q5, D14
	vaddw.u8 Q5, Q5, D3	
	vshrn.u8.u16 D3, Q5, #1
	vst1.64 {D3}, [Dst], Dst_W
	subs Count, Count, #2
	bgt MPEG2DEC_VO_Armv7Inter2_8x8HV_loop
	pop   {pc} 
	@ENDP
	
@END

