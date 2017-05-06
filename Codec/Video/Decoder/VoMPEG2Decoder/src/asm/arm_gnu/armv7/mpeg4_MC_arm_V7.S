@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2005		            *
@								 	                                    *
@***********************************************************************/

	@AREA	|.text|, CODE
	.section .text

	.global MPEG2DEC_VO_Armv7Copy8x8 
MPEG2DEC_VO_Armv7Copy8x8:  @PROC
	ldr		r12, [sp]
MPEG2DEC_VO_Armv7Copy8x8_loop_again:	
	VLD1.64  {d0},[r0],r2  
	VLD1.64  {d1},[r0],r2  
	VLD1.64  {d2},[r0],r2  
	VLD1.64  {d3},[r0],r2   	 
	subs r12,r12,#4   
	VST1.64   {d0},[r1],r3 
	VST1.64   {d1},[r1],r3 
	VST1.64   {d2},[r1],r3 
	VST1.64   {d3},[r1],r3
	bne	MPEG2DEC_VO_Armv7Copy8x8_loop_again	
	
	mov	pc, lr
	@ENDP
	
@------------------------------------------
@ArmCopy16x16 no aligment!, only used in Copy()
@------------------------------------------
	.global MPEG2DEC_VO_Armv7Copy16x16 
MPEG2DEC_VO_Armv7Copy16x16:  @PROC
	ldr		r12, [sp]
@	mov		r12, #16
MPEG2DEC_VO_Armv7Copy16x16_loop_again:
	VLD1.64  {q2},[r0],r2
	VLD1.64  {q4},[r0],r2
	VLD1.64  {q6},[r0],r2
	VLD1.64  {q8},[r0],r2
	subs r12,r12,#4   
	VST1.64   {q2},[r1],r3
	VST1.64   {q4},[r1],r3
	VST1.64   {q6},[r1],r3
	VST1.64   {q8},[r1],r3
	bne	MPEG2DEC_VO_Armv7Copy16x16_loop_again		
	mov	pc, lr
	@ENDP	
	
	
@------------------------------------------
@ArmInter8x8
@------------------------------------------
	.global MPEG2DEC_VO_Armv7Inter8x8 
MPEG2DEC_VO_Armv7Inter8x8:  @PROC
	ldr		r12, [sp]
@	mov		r12, #8
MPEG2DEC_VO_Armv7Inter8x8_loop_again:
	VLD1.8  {d0},[r0],r2  
	VLD1.8  {d1},[r0],r2  
	VLD1.8  {d2},[r0],r2  
	VLD1.8  {d3},[r0],r2  
@	VLD1.8  {d4},[r0],r2  
@	VLD1.8  {d5},[r0],r2  
@	VLD1.8  {d6},[r0],r2  
@	VLD1.8  {d7},[r0],r2 
		
	subs	r12, r12, #4   
	VST1.64   {d0},[r1],r3 
	VST1.64   {d1},[r1],r3 
	VST1.64   {d2},[r1],r3 
	VST1.64   {d3},[r1],r3	
@	VST1.64   {d4},[r1],r3 
@	VST1.64   {d5},[r1],r3 
@	VST1.64   {d6},[r1],r3 
@	VST1.64   {d7},[r1],r3	 
	bne	MPEG2DEC_VO_Armv7Inter8x8_loop_again		
	mov	pc, lr
	@ENDP	

@------------------------------------------
@ArmInter2_8x8 
@------------------------------------------
	.global MPEG2DEC_VO_Armv7Inter2_8x8 
MPEG2DEC_VO_Armv7Inter2_8x8:  @PROC
    PUSH     {lr}
	ldr		r12, [sp, #4]
@	mov		r12, #8
MPEG2DEC_VO_Armv7Inter2_8x8_loop_again:
	mov	lr, r1	  
	VLD1.8  {d0},[r0],r2
	VLD1.8  {d1},[r0],r2
	VLD1.8  {d2},[r0],r2
	VLD1.8  {d3},[r0],r2

	VLD1.64  {d4},[r1],r3
	VLD1.64  {d5},[r1],r3
	VLD1.64  {d6},[r1],r3
	VLD1.64  {d7},[r1],r3  
	subs	r12, r12, #4   
	VRHADD.U8 d0, d0, d4
	VRHADD.U8 d1, d1, d5
	VRHADD.U8 d2, d2, d6
	VRHADD.U8 d3, d3, d7    
	
	VST1.64   {d0},[lr],r3
	VST1.64   {d1},[lr],r3
	VST1.64   {d2},[lr],r3
	VST1.64   {d3},[lr],r3  
	bne	MPEG2DEC_VO_Armv7Inter2_8x8_loop_again		
    POP      {pc} 
	@ENDP	
	
	
Src		.req	r0
Dst		.req	r1
Src_W		.req	r2
Dst_W		.req	r3
Count		.req	r12
@------------------------------------------
@ ArmInter8x8H
@------------------------------------------
	.global MPEG2DEC_VO_Armv7Inter8x8H 
MPEG2DEC_VO_Armv7Inter8x8H:  @PROC
	PUSH   {r14}
	ADD		R14, Src, #8	
	ldr		Count, [sp, #4]
@	mov		Count, #8
MPEG2DEC_VO_Armv7Inter8x8H_loop:
	pld [Src, Src_W, lsl #2]
	VLD1.8 {D0}, [Src], Src_W
	VLD1.8 {D1[0]}, [R14], Src_W

	pld [Src, Src_W, lsl #2]
	VLD1.8 {D2}, [Src], Src_W	
	VLD1.8 {D3[0]}, [R14], Src_W	

	pld [Src, Src_W, lsl #2]
	VLD1.8 {D4}, [Src], Src_W
	VLD1.8 {D5[0]}, [R14], Src_W

	pld [Src, Src_W, lsl #2]
	VLD1.8 {D6}, [Src], Src_W
	VLD1.8 {D7[0]}, [R14], Src_W	
			
	VEXT.U8 D11, D0, D1, #1
	VEXT.U8 D13, D2, D3, #1	
	VEXT.U8 D15, D4, D5, #1	
	VEXT.U8 D17, D6, D7, #1
		
	VRHADD.U8 D10, D0, D11
	VRHADD.U8 D12, D2, D13
	VRHADD.U8 D14, D4, D15	
	VRHADD.U8 D16, D6, D17	
	SUBS Count, Count, #4		
	VST1.64 {D10}, [Dst], Dst_W
	VST1.64 {D12}, [Dst], Dst_W
	VST1.64 {D14}, [Dst], Dst_W
	VST1.64 {D16}, [Dst], Dst_W
				
	bne	MPEG2DEC_VO_Armv7Inter8x8H_loop		 
	POP   {pc} 	
	@ENDP	
	
	
@------------------------------------------
@ MPEG2DEC_VO_Armv7Inter8x8HRD
@------------------------------------------
	.global MPEG2DEC_VO_Armv7Inter8x8HRD 
MPEG2DEC_VO_Armv7Inter8x8HRD:  @PROC
	PUSH   {r14}
	ADD		R14, Src, #8	
@	ldr		Count, [sp, #4]
	mov		Count, #8
MPEG2DEC_VO_Armv7Inter8x8HRD_loop:
	pld [Src, Src_W, lsl #2]
	VLD1.8 {D0}, [Src], Src_W
	VLD1.8 {D1[0]}, [R14], Src_W

	pld [Src, Src_W, lsl #2]
	VLD1.8 {D2}, [Src], Src_W	
	VLD1.8 {D3[0]}, [R14], Src_W	

	pld [Src, Src_W, lsl #2]
	VLD1.8 {D4}, [Src], Src_W
	VLD1.8 {D5[0]}, [R14], Src_W

	pld [Src, Src_W, lsl #2]
	VLD1.8 {D6}, [Src], Src_W
	VLD1.8 {D7[0]}, [R14], Src_W
			
	VEXT.U8 D11, D0, D1, #1
	VEXT.U8 D13, D2, D3, #1	
	VEXT.U8 D15, D4, D5, #1	
	VEXT.U8 D17, D6, D7, #1
		
	VHADD.U8 D10, D0, D11
	VHADD.U8 D12, D2, D13
	VHADD.U8 D14, D4, D15	
	VHADD.U8 D16, D6, D17	
		
	VST1.64 {D10}, [Dst], Dst_W
	VST1.64 {D12}, [Dst], Dst_W
	VST1.64 {D14}, [Dst], Dst_W
	VST1.64 {D16}, [Dst], Dst_W
	SUBS Count, Count, #4	
	bne	MPEG2DEC_VO_Armv7Inter8x8HRD_loop
	POP   {pc} 	
	@ENDP		
	
@------------------------------------------
@ MPEG2DEC_VO_Armv7Inter8x8V
@------------------------------------------
	.global MPEG2DEC_VO_Armv7Inter8x8V 
MPEG2DEC_VO_Armv7Inter8x8V:  @PROC
	ldr		Count, [sp]
	VLD1.8 {D0}, [Src], Src_W
@	mov		Count, #8		
MPEG2DEC_VO_Armv7Inter8x8V_loop:
	VLD1.8 {D1}, [Src], Src_W
	VLD1.8 {D2}, [Src], Src_W	
	VLD1.8 {D3}, [Src], Src_W	
	VRHADD.U8 D11, D0, D1
	VLD1.8 {D0}, [Src], Src_W	
	VRHADD.U8 D12, D1, D2
	VRHADD.U8 D13, D2, D3	
	VRHADD.U8 D14, D3, D0
	SUBS Count, Count, #4	
	VST1.64 {D11}, [Dst], Dst_W
	VST1.64 {D12}, [Dst], Dst_W		
	VST1.64 {D13}, [Dst], Dst_W
	VST1.64 {D14}, [Dst], Dst_W	
			
@	VLD1.8 {D5}, [Src], Src_W		
@	VLD1.8 {D6}, [Src], Src_W		
@	VLD1.8 {D7}, [Src], Src_W				
@	VLD1.8 {D0}, [Src], Src_W	
@	VRHADD.U8 D15, D4, D5		
@	VRHADD.U8 D16, D5, D6	
@	VRHADD.U8 D17, D6, D7	
@	VRHADD.U8 D10, D7, D0
@	VST1.64 {D15}, [Dst], Dst_W
@	VST1.64 {D16}, [Dst], Dst_W	
@	VST1.64 {D17}, [Dst], Dst_W	
@	VST1.64 {D10}, [Dst], Dst_W				
	BGT MPEG2DEC_VO_Armv7Inter8x8V_loop
	mov	pc, lr
	@ENDP	

@------------------------------------------
@ MPEG2DEC_VO_Armv7Inter8x8VRD
@------------------------------------------
	.global MPEG2DEC_VO_Armv7Inter8x8VRD 
MPEG2DEC_VO_Armv7Inter8x8VRD:  @PROC
	ldr		Count, [sp]
	VLD1.8 {D0}, [Src], Src_W
@	mov		Count, #8		
MPEG2DEC_VO_Armv7Inter8x8VRD_loop:
	VLD1.8 {D1}, [Src], Src_W
	VLD1.8 {D2}, [Src], Src_W	
	VLD1.8 {D3}, [Src], Src_W
	VHADD.U8 D11, D0, D1
	VLD1.8 {D0}, [Src], Src_W	

	VHADD.U8 D12, D1, D2
	VHADD.U8 D13, D2, D3	
	VHADD.U8 D14, D3, D0
	SUBS Count, Count, #4	
	VST1.64 {D11}, [Dst], Dst_W
	VST1.64 {D12}, [Dst], Dst_W		
	VST1.64 {D13}, [Dst], Dst_W
	VST1.64 {D14}, [Dst], Dst_W	
		
@	SUBS Count, Count, #4
	BGT MPEG2DEC_VO_Armv7Inter8x8VRD_loop		 
	mov	pc, lr
	@ENDP			
@------------------------------------------
@ MPEG2DEC_VO_Armv7Inter8x8HV
@------------------------------------------
	.global MPEG2DEC_VO_Armv7Inter8x8HV 
MPEG2DEC_VO_Armv7Inter8x8HV:  @PROC
	PUSH   {r14}
	ADD		R14, Src, #8	
	ldr		Count, [sp, #4]
@	mov		Count, #8
	VLD1.8 {D0}, [Src], Src_W
	VLD1.8 {D1[0]}, [R14], Src_W	
	VEXT.U8 D1, D0, D1, #1
	VADDL.U8 Q8, D0, D1
	VMOV.U8 D31, #2	
MPEG2DEC_VO_Armv7Inter8x8HV_loop:
	pld [Src, Src_W, lsl #2]
	VLD1.8 {D2}, [Src], Src_W	
	VLD1.8 {D3[0]}, [R14], Src_W	

	pld [Src, Src_W, lsl #2]
	VLD1.8 {D4}, [Src], Src_W
	VLD1.8 {D5[0]}, [R14], Src_W

	pld [Src, Src_W, lsl #2]
	VLD1.8 {D6}, [Src], Src_W
	VLD1.8 {D7[0]}, [R14], Src_W

	pld [Src, Src_W, lsl #2]
	VLD1.8 {D8}, [Src], Src_W
	VLD1.8 {D9[0]}, [R14], Src_W
							
	VEXT.U8 D3, D2, D3, #1
	VEXT.U8 D5, D4, D5, #1
		
	VADDL.U8 Q9, D2, D3
	VADDL.U8 Q10, D4, D5
		
	VADDW.U8 Q9, Q9, D31	
	VADD.U16 Q8, Q8, Q9
	VADD.U16 Q9, Q9, Q10		
	VSHRN.U8.U16 D2, Q8, #2
	VSHRN.U8.U16 D4, Q9, #2

	VEXT.U8 D7, D6, D7, #1
	VEXT.U8 D9, D8, D9, #1
		
	VADDL.U8 Q11, D6, D7
	VADDL.U8 Q8, D8, D9
		
	VADDW.U8 Q11, Q11, D31	
	VADD.U16 Q10, Q10, Q11
	VADD.U16 Q11, Q11, Q8	
	VSHRN.U8.U16 D6, Q10, #2
	VSHRN.U8.U16 D8, Q11, #2
	SUBS Count, Count, #4
	
	VST1.64 {D2}, [Dst], Dst_W	
	VST1.64 {D4}, [Dst], Dst_W	
	VST1.64 {D6}, [Dst], Dst_W	
	VST1.64 {D8}, [Dst], Dst_W

	BGT MPEG2DEC_VO_Armv7Inter8x8HV_loop	 
	POP   {pc} 	
	@ENDP	
	

@------------------------------------------
@ MPEG2DEC_VO_Armv7Inter8x8HVRD
@------------------------------------------
	.global MPEG2DEC_VO_Armv7Inter8x8HVRD 
MPEG2DEC_VO_Armv7Inter8x8HVRD:  @PROC
	PUSH   {r14}
	ADD		R14, Src, #8	
	ldr		Count, [sp, #4]
@	mov		Count, #8
	VLD1.8 {D0}, [Src], Src_W
	VLD1.8 {D1[0]}, [R14], Src_W		
	VEXT.U8 D1, D0, D1, #1
	VADDL.U8 Q8, D0, D1
	VMOV.U8 D31, #1
MPEG2DEC_VO_Armv7Inter8x8HVRD_loop:
	pld [Src, Src_W, lsl #2]
	VLD1.8 {D2}, [Src], Src_W	
	VLD1.8 {D3[0]}, [R14], Src_W	

	pld [Src, Src_W, lsl #2]
	VLD1.8 {D4}, [Src], Src_W
	VLD1.8 {D5[0]}, [R14], Src_W

	pld [Src, Src_W, lsl #2]
	VLD1.8 {D6}, [Src], Src_W
	VLD1.8 {D7[0]}, [R14], Src_W

	pld [Src, Src_W, lsl #2]
	VLD1.8 {D8}, [Src], Src_W
	VLD1.8 {D9[0]}, [R14], Src_W
		
	VEXT.U8 D3, D2, D3, #1
	VEXT.U8 D5, D4, D5, #1
		
	VADDL.U8 Q9, D2, D3
	VADDL.U8 Q10, D4, D5
		
	VADDW.U8 Q9, Q9, D31	
	VADD.U16 Q8, Q8, Q9
	VADD.U16 Q9, Q9, Q10	
	VSHRN.U8.U16 D2, Q8, #2
	VSHRN.U8.U16 D4, Q9, #2
	
	VEXT.U8 D7, D6, D7, #1
	VEXT.U8 D9, D8, D9, #1
		
	VADDL.U8 Q11, D6, D7
	VADDL.U8 Q8, D8, D9
		
	VADDW.U8 Q11, Q11, D31	
	VADD.U16 Q10, Q10, Q11
	VADD.U16 Q11, Q11, Q8	
	VSHRN.U8.U16 D6, Q10, #2
	VSHRN.U8.U16 D8, Q11, #2
	
	VST1.64 {D2}, [Dst], Dst_W	
	VST1.64 {D4}, [Dst], Dst_W	
	VST1.64 {D6}, [Dst], Dst_W	
	VST1.64 {D8}, [Dst], Dst_W

	SUBS Count, Count, #4
	BGT MPEG2DEC_VO_Armv7Inter8x8HVRD_loop	 
	POP   {pc} 
	@ENDP	

@------------------------------------------
@ MPEG2DEC_VO_Armv7Inter2_8x8H
@------------------------------------------
	.global MPEG2DEC_VO_Armv7Inter2_8x8H 
MPEG2DEC_VO_Armv7Inter2_8x8H:  @PROC
	PUSH   {r14}
	ADD		R14, Src, #8	
	ldr		Count, [sp, #4]
@	mov		Count, #8
	VMOV.U8 D31, #1	
MPEG2DEC_VO_Armv7Inter2_8x8H_loop:
	pld [Src, Src_W]
	pld [Src, Src_W, lsl #1]
	pld [Dst, Dst_W]
	pld [Dst, Dst_W, lsl #1]

	VLD1.8 {D0}, [Src], Src_W
	VLD1.8  {D1[0]}, [R14], Src_W
	VLD1.64 {D10}, [Dst]	
	VEXT.U8 D1, D0, D1, #1	
	VADDL.U8 Q8, D0, D1
	VADDW.U8 Q8, Q8, D31
	VSHR.U16 Q8, Q8, #1
	VADDW.U8 Q8, Q8, D31
	VADDW.U8 Q8, Q8, D10	
	VSHRN.U8.U16 D10, Q8, #1						
	VST1.64 {D10}, [Dst], Dst_W
	
	VLD1.8 {D2}, [Src], Src_W
	VLD1.8  {D3[0]}, [R14], Src_W
	VLD1.64 {D11}, [Dst]	
	VEXT.U8 D3, D2, D3, #1	
	VADDL.U8 Q9, D2, D3
	VADDW.U8 Q9, Q9, D31
	VSHR.U16 Q9, Q9, #1
	VADDW.U8 Q9, Q9, D31
	VADDW.U8 Q9, Q9, D11	
	VSHRN.U8.U16 D11, Q9, #1
	SUBS Count, Count, #2							
	VST1.64 {D11}, [Dst], Dst_W

	
	bne	MPEG2DEC_VO_Armv7Inter2_8x8H_loop		 
	POP   {pc} 
	@ENDP	
	
@------------------------------------------
@ MPEG2DEC_VO_Armv7Inter2_8x8V
@------------------------------------------
	.global MPEG2DEC_VO_Armv7Inter2_8x8V 
MPEG2DEC_VO_Armv7Inter2_8x8V:  @PROC
	ldr		Count, [sp]
@	mov		Count, #8
	VMOV.U8 D13, #1		
	VLD1.8 {D0}, [Src], Src_W	
MPEG2DEC_VO_Armv7Inter2_8x8V_loop:
	pld [Src, Src_W]
	pld [Src, Src_W, lsl #1]

	pld [Dst, Dst_W]
	pld [Dst, Dst_W, lsl #1]

	VLD1.8 {D2}, [Src], Src_W
	VLD1.64 {D3}, [Dst]	
	VADDL.U8 Q5, D0, D2
	VADDW.U8 Q5, Q5, D13
	VSHR.U16 Q5, Q5, #1	
	VADDW.U8 Q5, Q5, D13
	VADDW.U8 Q5, Q5, D3	
	VSHRN.U8.U16 D3, Q5, #1
	VST1.64 {D3}, [Dst], Dst_W	
	SUBS Count, Count, #2		
	VLD1.8 {D0}, [Src], Src_W
	VLD1.64 {D3}, [Dst]	
	VADDL.U8 Q4, D0, D2
	VADDW.U8 Q4, Q4, D13
	VSHR.U16 Q4, Q4, #1	
	VADDW.U8 Q4, Q4, D13
	VADDW.U8 Q4, Q4, D3	
	VSHRN.U8.U16 D3, Q4, #1
	VST1.64 {D3}, [Dst], Dst_W	
	BGT MPEG2DEC_VO_Armv7Inter2_8x8V_loop	 
	mov	pc, lr	
	@ENDP
	
				
@------------------------------------------
@ MPEG2DEC_VO_Armv7Inter2_8x8HV
@------------------------------------------
	.global MPEG2DEC_VO_Armv7Inter2_8x8HV 
MPEG2DEC_VO_Armv7Inter2_8x8HV:  @PROC
	PUSH   {r14}
	ADD		R14, Src, #8	
	ldr		Count, [sp, #4]
	VLD1.8 {D0}, [Src], Src_W
@	mov		Count, #8	
	VLD1.8 {D1[0]}, [R14], Src_W	
	VEXT.U8 D1, D0, D1, #1
	VADDL.U8 Q4, D0, D1
	VMOV.U8 D13, #2
	VMOV.U8 D14, #1	
MPEG2DEC_VO_Armv7Inter2_8x8HV_loop:
	pld [Src, Src_W]
	pld [Src, Src_W, lsl #1]

	pld [Dst, Dst_W]
	pld [Dst, Dst_W, lsl #1]
	VLD1.8 {D2}, [Src], Src_W
	VLD1.8 {D3[0]}, [R14], Src_W	
	VEXT.U8 D3, D2, D3, #1
	VADDL.U8 Q5, D2, D3
	VADDW.U8 Q5, Q5, D13
	VADD.U16 Q4, Q4, Q5
	VSHR.U16 Q4, Q4, #2	
	VLD1.64 {D3}, [Dst]	
	VADDW.U8 Q4, Q4, D14
	VADDW.U8 Q4, Q4, D3	
	VSHRN.U8.U16 D3, Q4, #1
	VST1.64 {D3}, [Dst], Dst_W		
	VLD1.8  {D0}, [Src], Src_W
	VLD1.8 {D1[0]}, [R14], Src_W
	VEXT.U8 D1, D0, D1, #1
	VADDL.U8 Q4, D0, D1
	VADD.U16 Q5, Q5, Q4
	VSHR.U16 Q5, Q5, #2	
	VLD1.64 {D3}, [Dst]	
	VADDW.U8 Q5, Q5, D14
	VADDW.U8 Q5, Q5, D3	
	VSHRN.U8.U16 D3, Q5, #1
	VST1.64 {D3}, [Dst], Dst_W
	SUBS Count, Count, #2
	BGT MPEG2DEC_VO_Armv7Inter2_8x8HV_loop
	POP   {pc} 
	@ENDP
	
@END

