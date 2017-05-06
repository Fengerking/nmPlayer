@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2005		            *
@								 	                                    *
@***********************************************************************/

#include "../../../voMpeg4DID.h"
	@AREA	|.text|, CODE
	.text
	.align 4

@------------------------------------------
@ArmCopy8x8: no aligment!, only used in Copy()
@------------------------------------------
	.globl _MPEG4DEC_VO_Armv7Copy8x8 
_MPEG4DEC_VO_Armv7Copy8x8  : @PROC
	vld1.64  {d0},[r0],r2  
	vld1.64  {d1},[r0],r2  
	vld1.64  {d2},[r0],r2  
	vld1.64  {d3},[r0],r2  
	vld1.64  {d4},[r0],r2  
	vld1.64  {d5},[r0],r2  
	vld1.64  {d6},[r0],r2  
	vld1.64  {d7},[r0],r2   
	
	vst1.64   {d0},[r1],r3 
	vst1.64   {d1},[r1],r3 
	vst1.64   {d2},[r1],r3 
	vst1.64   {d3},[r1],r3
	vst1.64   {d4},[r1],r3 
	vst1.64   {d5},[r1],r3 
	vst1.64   {d6},[r1],r3 
	vst1.64   {d7},[r1],r3  	 	
	mov	pc, lr
	@ENDP			
@------------------------------------------
@ArmCopy16x16: no aligment!, only used in Copy()
@------------------------------------------
	.globl _MPEG4DEC_VO_Armv7Copy16x16 
_MPEG4DEC_VO_Armv7Copy16x16  : @PROC
@	ldr		r12, [sp]
	mov		r12, #16
MPEG4DEC_VO_Armv7Copy16x16_loop_again:
	vld1.64  {q2},[r0],r2
	vld1.64  {q4},[r0],r2
	vld1.64  {q6},[r0],r2
	vld1.64  {q8},[r0],r2
	subs r12,r12,#4   
	vst1.64   {q2},[r1],r3
	vst1.64   {q4},[r1],r3
	vst1.64   {q6},[r1],r3
	vst1.64   {q8},[r1],r3
	bne	MPEG4DEC_VO_Armv7Copy16x16_loop_again		
	mov	pc, lr
	@ENDP	
	
@------------------------------------------
@ArmInter8x8
@------------------------------------------
	.globl _MPEG4DEC_VO_Armv7Inter8x8 
_MPEG4DEC_VO_Armv7Inter8x8  : @PROC
	ldr		r12, [sp]
@	mov		r12, #8
MPEG4DEC_VO_Armv7Inter8x8_loop_again:
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
	bne	MPEG4DEC_VO_Armv7Inter8x8_loop_again		
	mov	pc, lr
	@ENDP	
	
@------------------------------------------
@ArmInter2_8x8 
@------------------------------------------
	.globl _MPEG4DEC_VO_Armv7Inter2_8x8 
_MPEG4DEC_VO_Armv7Inter2_8x8  : @PROC
    push     {lr}
	ldr		r12, [sp, #4]
@	mov		r12, #8
MPEG4DEC_VO_Armv7Inter2_8x8_loop_again:
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
	bne	MPEG4DEC_VO_Armv7Inter2_8x8_loop_again		
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
	.globl _MPEG4DEC_VO_Armv7Inter8x8H 
_MPEG4DEC_VO_Armv7Inter8x8H  : @PROC
	push   {r14}
	add		r14, Src, #8	
	ldr		Count, [sp, #4]
@	mov		Count, #8
MPEG4DEC_VO_Armv7Inter8x8H_loop:
	pld [Src, Src_W, lsl #2]
	vld1.8 {d0}, [Src], Src_W
	vld1.8 {d1[0]}, [r14], Src_W

	pld [Src, Src_W, lsl #2]
	vld1.8 {d2}, [Src], Src_W	
	vld1.8 {d3[0]}, [r14], Src_W	

	pld [Src, Src_W, lsl #2]
	vld1.8 {d4}, [Src], Src_W
	vld1.8 {d5[0]}, [r14], Src_W

	pld [Src, Src_W, lsl #2]
	vld1.8 {d6}, [Src], Src_W
	vld1.8 {d7[0]}, [r14], Src_W	
			
	vext.u8 d11, d0, d1, #1
	vext.u8 d13, d2, d3, #1	
	vext.u8 d15, d4, d5, #1	
	vext.u8 d17, d6, d7, #1
		
	vrhadd.u8 d10, d0, d11
	vrhadd.u8 d12, d2, d13
	vrhadd.u8 d14, d4, d15	
	vrhadd.u8 d16, d6, d17	
		
	vst1.64 {d10}, [Dst], Dst_W
	vst1.64 {d12}, [Dst], Dst_W
	vst1.64 {d14}, [Dst], Dst_W
	vst1.64 {d16}, [Dst], Dst_W
	subs Count, Count, #4				
	bne	MPEG4DEC_VO_Armv7Inter8x8H_loop		 
	pop   {pc} 	
	@ENDP	
	
@------------------------------------------
@ MPEG4DEC_VO_Armv7Inter8x8HRD
@------------------------------------------
	.globl _MPEG4DEC_VO_Armv7Inter8x8HRD 
_MPEG4DEC_VO_Armv7Inter8x8HRD  : @PROC
	push   {r14}
	add		r14, Src, #8	
	ldr		Count, [sp, #4]
@	mov		Count, #8
MPEG4DEC_VO_Armv7Inter8x8HRD_loop:
	pld [Src, Src_W, lsl #2]
	vld1.8 {d0}, [Src], Src_W
	vld1.8 {d1[0]}, [r14], Src_W

	pld [Src, Src_W, lsl #2]
	vld1.8 {d2}, [Src], Src_W	
	vld1.8 {d3[0]}, [r14], Src_W	

	pld [Src, Src_W, lsl #2]
	vld1.8 {d4}, [Src], Src_W
	vld1.8 {d5[0]}, [r14], Src_W

	pld [Src, Src_W, lsl #2]
	vld1.8 {d6}, [Src], Src_W
	vld1.8 {d7[0]}, [r14], Src_W
			
	vext.u8 d11, d0, d1, #1
	vext.u8 d13, d2, d3, #1	
	vext.u8 d15, d4, d5, #1	
	vext.u8 d17, d6, d7, #1
		
	vhadd.u8 d10, d0, d11
	vhadd.u8 d12, d2, d13
	vhadd.u8 d14, d4, d15	
	vhadd.u8 d16, d6, d17	
		
	vst1.64 {d10}, [Dst], Dst_W
	vst1.64 {d12}, [Dst], Dst_W
	vst1.64 {d14}, [Dst], Dst_W
	vst1.64 {d16}, [Dst], Dst_W
	subs Count, Count, #4	
	bne	MPEG4DEC_VO_Armv7Inter8x8HRD_loop
	pop   {pc} 	
	@ENDP		
	
@------------------------------------------
@ MPEG4DEC_VO_Armv7Inter8x8V
@------------------------------------------
	.globl _MPEG4DEC_VO_Armv7Inter8x8V 
_MPEG4DEC_VO_Armv7Inter8x8V  : @PROC
	ldr		Count, [sp]
@	mov		Count, #8		
	vld1.8 {d0}, [Src], Src_W
	vld1.8 {d1}, [Src], Src_W
	vld1.8 {d2}, [Src], Src_W	
	vld1.8 {d3}, [Src], Src_W
	vld1.8 {d4}, [Src], Src_W	
	vrhadd.u8 d11, d0, d1
	vrhadd.u8 d12, d1, d2
	vrhadd.u8 d13, d2, d3	
	vrhadd.u8 d14, d3, d4
	vst1.64 {d11}, [Dst], Dst_W
	vst1.64 {d12}, [Dst], Dst_W		
	vst1.64 {d13}, [Dst], Dst_W
	vst1.64 {d14}, [Dst], Dst_W	
	subs Count, Count, #4				
	ble MPEG4DEC_VO_Armv7Inter8x8V_end
		
	vld1.8 {d5}, [Src], Src_W		
	vld1.8 {d6}, [Src], Src_W		
	vld1.8 {d7}, [Src], Src_W				
	vld1.8 {d0}, [Src], Src_W	
	vrhadd.u8 d15, d4, d5		
	vrhadd.u8 d16, d5, d6	
	vrhadd.u8 d17, d6, d7	
	vrhadd.u8 d10, d7, d0
	vst1.64 {d15}, [Dst], Dst_W
	vst1.64 {d16}, [Dst], Dst_W	
	vst1.64 {d17}, [Dst], Dst_W	
	vst1.64 {d10}, [Dst], Dst_W
	
MPEG4DEC_VO_Armv7Inter8x8V_end:
	mov	pc, lr
	@ENDP	

@------------------------------------------
@ MPEG4DEC_VO_Armv7Inter8x8VRD
@------------------------------------------
	.globl _MPEG4DEC_VO_Armv7Inter8x8VRD 
_MPEG4DEC_VO_Armv7Inter8x8VRD  : @PROC
	ldr		Count, [sp]
@	mov		Count, #8		
	vld1.8 {d0}, [Src], Src_W
	vld1.8 {d1}, [Src], Src_W
	vld1.8 {d2}, [Src], Src_W	
	vld1.8 {d3}, [Src], Src_W
	vld1.8 {d4}, [Src], Src_W	
	vhadd.u8 d11, d0, d1
	vhadd.u8 d12, d1, d2
	vhadd.u8 d13, d2, d3	
	vhadd.u8 d14, d3, d4
	vst1.64 {d11}, [Dst], Dst_W
	vst1.64 {d12}, [Dst], Dst_W		
	vst1.64 {d13}, [Dst], Dst_W
	vst1.64 {d14}, [Dst], Dst_W	
	subs Count, Count, #4
	ble MPEG4DEC_VO_Armv7Inter8x8VRD_end		 
		
	vld1.8 {d5}, [Src], Src_W		
	vld1.8 {d6}, [Src], Src_W		
	vld1.8 {d7}, [Src], Src_W				
	vld1.8 {d0}, [Src], Src_W	
	vhadd.u8 d15, d4, d5		
	vhadd.u8 d16, d5, d6	
	vhadd.u8 d17, d6, d7	
	vhadd.u8 d10, d7, d0
	vst1.64 {d15}, [Dst], Dst_W
	vst1.64 {d16}, [Dst], Dst_W	
	vst1.64 {d17}, [Dst], Dst_W	
	vst1.64 {d10}, [Dst], Dst_W
MPEG4DEC_VO_Armv7Inter8x8VRD_end:
	mov	pc, lr
	@ENDP			
@------------------------------------------
@ MPEG4DEC_VO_Armv7Inter8x8HV
@------------------------------------------
	.globl _MPEG4DEC_VO_Armv7Inter8x8HV 
_MPEG4DEC_VO_Armv7Inter8x8HV  : @PROC
	push   {r14}
	add		r14, Src, #8	
	ldr		Count, [sp, #4]
@	mov		Count, #8
	vld1.8 {d0}, [Src], Src_W
	vld1.8 {d1[0]}, [r14], Src_W	
	vext.u8 d1, d0, d1, #1
	vaddl.u8 q8, d0, d1
	vmov.u8 d31, #2	
MPEG4DEC_VO_Armv7Inter8x8HV_loop:
	pld [Src, Src_W, lsl #2]
	vld1.8 {d2}, [Src], Src_W	
	vld1.8 {d3[0]}, [r14], Src_W	

	pld [Src, Src_W, lsl #2]
	vld1.8 {d4}, [Src], Src_W
	vld1.8 {d5[0]}, [r14], Src_W

	pld [Src, Src_W, lsl #2]
	vld1.8 {d6}, [Src], Src_W
	vld1.8 {d7[0]}, [r14], Src_W

	pld [Src, Src_W, lsl #2]
	vld1.8 {d8}, [Src], Src_W
	vld1.8 {d9[0]}, [r14], Src_W
							
	vext.u8 d3, d2, d3, #1
	vext.u8 d5, d4, d5, #1
		
	vaddl.u8 q9, d2, d3
	vaddl.u8 q10, d4, d5
		
	vaddw.u8 q9, q9, d31	
	vadd.u16 q8, q8, q9
	vadd.u16 q9, q9, q10		
	vshrn.u8.u16 d2, q8, #2
	vshrn.u8.u16 d4, q9, #2

	vext.u8 d7, d6, d7, #1
	vext.u8 d9, d8, d9, #1
		
	vaddl.u8 q11, d6, d7
	vaddl.u8 q8, d8, d9
		
	vaddw.u8 q11, q11, d31	
	vadd.u16 q10, q10, q11
	vadd.u16 q11, q11, q8	
	vshrn.u8.u16 d6, q10, #2
	vshrn.u8.u16 d8, q11, #2
	
	vst1.64 {d2}, [Dst], Dst_W	
	vst1.64 {d4}, [Dst], Dst_W	
	vst1.64 {d6}, [Dst], Dst_W	
	vst1.64 {d8}, [Dst], Dst_W

	subs Count, Count, #4
	bgt MPEG4DEC_VO_Armv7Inter8x8HV_loop	 
	pop   {pc} 	
	@ENDP	

@------------------------------------------
@ MPEG4DEC_VO_Armv7Inter8x8HVRD
@------------------------------------------
	.globl _MPEG4DEC_VO_Armv7Inter8x8HVRD 
_MPEG4DEC_VO_Armv7Inter8x8HVRD  : @PROC
	push   {r14}
	add		r14, Src, #8	
	ldr		Count, [sp, #4]
@	mov		Count, #8
	vld1.8 {d0}, [Src], Src_W
	vld1.8 {d1[0]}, [r14], Src_W		
	vext.u8 d1, d0, d1, #1
	vaddl.u8 q8, d0, d1
	vmov.u8 d31, #1
MPEG4DEC_VO_Armv7Inter8x8HVRD_loop:
	pld [Src, Src_W, lsl #2]
	vld1.8 {d2}, [Src], Src_W	
	vld1.8 {d3[0]}, [r14], Src_W	

	pld [Src, Src_W, lsl #2]
	vld1.8 {d4}, [Src], Src_W
	vld1.8 {d5[0]}, [r14], Src_W

	pld [Src, Src_W, lsl #2]
	vld1.8 {d6}, [Src], Src_W
	vld1.8 {d7[0]}, [r14], Src_W

	pld [Src, Src_W, lsl #2]
	vld1.8 {d8}, [Src], Src_W
	vld1.8 {d9[0]}, [r14], Src_W
		
	vext.u8 d3, d2, d3, #1
	vext.u8 d5, d4, d5, #1
		
	vaddl.u8 q9, d2, d3
	vaddl.u8 q10, d4, d5
		
	vaddw.u8 q9, q9, d31	
	vadd.u16 q8, q8, q9
	vadd.u16 q9, q9, q10	
	vshrn.u8.u16 d2, q8, #2
	vshrn.u8.u16 d4, q9, #2
	
	vext.u8 d7, d6, d7, #1
	vext.u8 d9, d8, d9, #1
		
	vaddl.u8 q11, d6, d7
	vaddl.u8 q8, d8, d9
		
	vaddw.u8 q11, q11, d31	
	vadd.u16 q10, q10, q11
	vadd.u16 q11, q11, q8	
	vshrn.u8.u16 d6, q10, #2
	vshrn.u8.u16 d8, q11, #2
	
	vst1.64 {d2}, [Dst], Dst_W	
	vst1.64 {d4}, [Dst], Dst_W	
	vst1.64 {d6}, [Dst], Dst_W	
	vst1.64 {d8}, [Dst], Dst_W

	subs Count, Count, #4
	bgt MPEG4DEC_VO_Armv7Inter8x8HVRD_loop	 
	pop   {pc} 
	@ENDP	

@------------------------------------------
@ MPEG4DEC_VO_Armv7Inter2_8x8H
@------------------------------------------
	.globl _MPEG4DEC_VO_Armv7Inter2_8x8H 
_MPEG4DEC_VO_Armv7Inter2_8x8H  : @PROC
	push   {r14}
	add		r14, Src, #8	
	ldr		Count, [sp, #4]
@	mov		Count, #8
	vmov.u8 d31, #1	
MPEG4DEC_VO_Armv7Inter2_8x8H_loop:
	pld [Src, Src_W]
	pld [Src, Src_W, lsl #1]
	pld [Dst, Dst_W]
	pld [Dst, Dst_W, lsl #1]

	vld1.8 {d0}, [Src], Src_W
	vld1.8  {d1[0]}, [r14], Src_W
	vld1.64 {d10}, [Dst]	
	vext.u8 d1, d0, d1, #1	
	vaddl.u8 q8, d0, d1
	vaddw.u8 q8, q8, d31
	vshr.u16 q8, q8, #1
	vaddw.u8 q8, q8, d31
	vaddw.u8 q8, q8, d10	
	vshrn.u8.u16 d10, q8, #1						
	vst1.64 {d10}, [Dst], Dst_W
	
	vld1.8 {d2}, [Src], Src_W
	vld1.8  {d3[0]}, [r14], Src_W
	vld1.64 {d11}, [Dst]	
	vext.u8 d3, d2, d3, #1	
	vaddl.u8 q9, d2, d3
	vaddw.u8 q9, q9, d31
	vshr.u16 q9, q9, #1
	vaddw.u8 q9, q9, d31
	vaddw.u8 q9, q9, d11	
	vshrn.u8.u16 d11, q9, #1						
	vst1.64 {d11}, [Dst], Dst_W

	subs Count, Count, #2	
	bne	MPEG4DEC_VO_Armv7Inter2_8x8H_loop		 
	pop   {pc} 
	@ENDP	
@------------------------------------------
@ MPEG4DEC_VO_Armv7Inter2_8x8V
@------------------------------------------
	.globl _MPEG4DEC_VO_Armv7Inter2_8x8V 
_MPEG4DEC_VO_Armv7Inter2_8x8V  : @PROC
	ldr		Count, [sp]
@	mov		Count, #8
	vmov.u8 d13, #1		
	vld1.8 {d0}, [Src], Src_W	
MPEG4DEC_VO_Armv7Inter2_8x8V_loop:
	pld [Src, Src_W]
	pld [Src, Src_W, lsl #1]

	pld [Dst, Dst_W]
	pld [Dst, Dst_W, lsl #1]

	vld1.8 {d2}, [Src], Src_W
	vld1.64 {d3}, [Dst]	
	vaddl.u8 q5, d0, d2
	vaddw.u8 q5, q5, d13
	vshr.u16 q5, q5, #1	
	vaddw.u8 q5, q5, d13
	vaddw.u8 q5, q5, d3	
	vshrn.u8.u16 d3, q5, #1
	vst1.64 {d3}, [Dst], Dst_W	
	subs Count, Count, #2		
	vld1.8 {d0}, [Src], Src_W
	vld1.64 {d3}, [Dst]	
	vaddl.u8 q4, d0, d2
	vaddw.u8 q4, q4, d13
	vshr.u16 q4, q4, #1	
	vaddw.u8 q4, q4, d13
	vaddw.u8 q4, q4, d3	
	vshrn.u8.u16 d3, q4, #1
	vst1.64 {d3}, [Dst], Dst_W	
	bgt MPEG4DEC_VO_Armv7Inter2_8x8V_loop	 
	mov	pc, lr	
	@ENDP		
@------------------------------------------
@ MPEG4DEC_VO_Armv7Inter2_8x8HV
@------------------------------------------
	.globl _MPEG4DEC_VO_Armv7Inter2_8x8HV 
_MPEG4DEC_VO_Armv7Inter2_8x8HV  : @PROC
	push   {r14}
	add		r14, Src, #8	
	ldr		Count, [sp, #4]
	vld1.8 {d0}, [Src], Src_W
@	mov		Count, #8	
	vld1.8 {d1[0]}, [r14], Src_W	
	vext.u8 d1, d0, d1, #1
	vaddl.u8 q4, d0, d1
	vmov.u8 d13, #2
	vmov.u8 d14, #1	
MPEG4DEC_VO_Armv7Inter2_8x8HV_loop:
	pld [Src, Src_W]
	pld [Src, Src_W, lsl #1]

	pld [Dst, Dst_W]
	pld [Dst, Dst_W, lsl #1]
	vld1.8 {d2}, [Src], Src_W
	vld1.8 {d3[0]}, [r14], Src_W	
	vext.u8 d3, d2, d3, #1
	vaddl.u8 q5, d2, d3
	vaddw.u8 q5, q5, d13
	vadd.u16 q4, q4, q5
	vshr.u16 q4, q4, #2	
	vld1.64 {d3}, [Dst]	
	vaddw.u8 q4, q4, d14
	vaddw.u8 q4, q4, d3	
	vshrn.u8.u16 d3, q4, #1
	vst1.64 {d3}, [Dst], Dst_W		
	vld1.8  {d0}, [Src], Src_W
	vld1.8 {d1[0]}, [r14], Src_W
	vext.u8 d1, d0, d1, #1
	vaddl.u8 q4, d0, d1
	vadd.u16 q5, q5, q4
	vshr.u16 q5, q5, #2	
	vld1.64 {d3}, [Dst]	
	vaddw.u8 q5, q5, d14
	vaddw.u8 q5, q5, d3	
	vshrn.u8.u16 d3, q5, #1
	vst1.64 {d3}, [Dst], Dst_W
	subs Count, Count, #2
	bgt MPEG4DEC_VO_Armv7Inter2_8x8HV_loop
	pop   {pc} 
	@ENDP				
	@.END

