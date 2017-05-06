@************************************************************************
@									                                    *
@	VisualOn, Inc Confvtrnidential and Proprietary, 2005		            *
@								 	                                    *
@***********************************************************************/

	@AREA	|.text|, CODE
	#include "voYYDef_CC.h"
	 .text
	 
	 .align 2
	
	.globl _rgb888_to_s_rgb16_8x8_armv7
	.globl	_rgb888_to_s_rgb16_8x2n_armv7	
	.globl	_rgb888_to_rgb16_8nx2n_armv7	
	@AREA	|.text|, CODE, READONLY

  .macro macro_line_0_1
		ldr	r0, [sp, #20]	
		ldr	r10, [sp, #24]
					
		@y0	
		uxtah	r7, $0, r0
		uxtah	r2, $0, r0, ror #16		
		vld2.8 {d16[0], d17[0]}, [r7], r9
		vld2.8 {d16[1], d17[1]}, [r2], r9		
		vld2.8 {d18[0], d19[0]}, [r7]
		vld2.8 {d18[1], d19[1]}, [r2]
		
		@y1	
		uxtah	r7, $1, r0
		uxtah	r2, $1, r0, ror #16	
		vld2.8 {d14[0], d15[0]}, [r7], r9
		vld2.8 {d14[1], d15[1]}, [r2], r9		
		vld2.8 {d28[0], d29[0]}, [r7]
		vld2.8 {d28[1], d29[1]}, [r2]		
	
		@y0	
		uxtah	r7, $0, r10
		uxtah	r2, $0, r10, ror #16		
		vld2.8 {d16[2], d17[2]}, [r7], r9
		vld2.8 {d16[3], d17[3]}, [r2], r9		
		vld2.8 {d18[2], d19[2]}, [r7]
		vld2.8 {d18[3], d19[3]}, [r2]
		@y1	
		uxtah	r7, $1, r10
		uxtah	r2, $1, r10, ror #16		
		vld2.8 {d14[2], d15[2]}, [r7], r9
		vld2.8 {d14[3], d15[3]}, [r2], r9	
		vld2.8 {d28[2], d29[2]}, [r7]
		vld2.8 {d28[3], d29[3]}, [r2]
		ldr	r0, [sp, #28]	
			
		@y0		
		uxtah	r7, $0, r0
		uxtah	r2, $0, r0, ror #16		
		vld2.8 {d16[4], d17[4]}, [r7], r9
		vld2.8 {d16[5], d17[5]}, [r2], r9		
		vld2.8 {d18[4], d19[4]}, [r7]
		vld2.8 {d18[5], d19[5]}, [r2]
		
		@y1		
		uxtah	r7, $1, r0
		uxtah	r2, $1, r0, ror #16	
		vld2.8 {d14[4], d15[4]}, [r7], r9
		vld2.8 {d14[5], d15[5]}, [r2], r9	
		vld2.8 {d28[4], d29[4]}, [r7]
		vld2.8 {d28[5], d29[5]}, [r2]	

		@y0
		uxtah	r7, $0, r12
		uxtah	r2, $0, r12, ror #16			
		vld2.8 {d16[6], d17[6]}, [r7], r9
		vld2.8 {d16[7], d17[7]}, [r2], r9	
		vld2.8 {d18[6], d19[6]}, [r7]
		vld2.8 {d18[7], d19[7]}, [r2]
		
		@y1		
		uxtah	r7, $1, r12
		uxtah	r2, $1, r12, ror #16		
		vld2.8 {d14[6], d15[6]}, [r7], r9
		vld2.8 {d14[7], d15[7]}, [r2], r9	
		vld2.8 {d28[6], d29[6]}, [r7]
		vld2.8 {d28[7], d29[7]}, [r2]

		vmovl.u8 q10, d16			@y  0~7	
		vmovl.u8 q11, d17			@y  0~7
		vmovl.u8 q12, d18			@y  0~7
		vmovl.u8 q13, d19			@y  0~7

@free register q2, q3, q4, q5, q7, (q8, q9, q10, q11), q12, q13, q14 
		vmull.u16 q8, d20, D1		@y  0~3		@src_y0[a0]*bb  
		vmull.u16 q9, d21, D3		@y  4~7		@src_y0[a0]*bb		
		vmlal.u16 q8, d22, D0		@y  0~3		@src_y0[a0]*bb + src_y0[a0+1]*aa		  
		vmlal.u16 q9, d23, D2		@y  4~7		@src_y0[a0]*bb + src_y0[a0+1]*aa		
		
		vmull.u16 q10, d24, D1		@y  0~3		@src_y0[a0+src_stride]*bb
		vmull.u16 q11, d25, D3		@y  4~7		@src_y0[a0+src_stride]*bb
		vmlal.u16 q10, d26, D0		@y  0~3		@src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa
		vmlal.u16 q11, d27, D2		@y  4~7		@src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa
		
		vmul.u32  q8, q8, d12[1]
		vmla.u32  q8, q10, d12[0]	@(src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0														
		vmul.u32  q9, q9, d12[1]
		vmla.u32  q9, q11, d12[0]	@(src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0
		vshrn.u32 d16, q8, #16		@a4  = ()>>16@
		vshrn.u32 d17, q9, #16		@a4  = ()>>16@		
@free register q2, q3, q4, q5, q7, q8, q9, q10, q11, q12, q13, q14
@		yyyy0[0~7] = q8

@ line 1
@free register q2, q3, q4, q5, q7, (q10, q11, q12, q13), q14, add q7, q14
		vmovl.u8 q10, d14			@y  0~7	
		vmovl.u8 q11, d15			@y  0~7
		vmovl.u8 q12, d28			@y  0~7
		vmovl.u8 q13, d29			@y  0~7

@free register (q2, q3, q4, q5), q7, q8, q9, q10, q11, q12, q13, q14 
		vmull.u16 q7, d20, D1		@y  0~3		@src_y0[a0]*bb  
		vmull.u16 q14, d21, D3		@y  4~7		@src_y0[a0]*bb		
		vmlal.u16 q7, d22, D0		@y  0~3		@src_y0[a0]*bb + src_y0[a0+1]*aa		  
		vmlal.u16 q14, d23, D2		@y  4~7		@src_y0[a0]*bb + src_y0[a0+1]*aa		
		
		vmull.u16 q10, d24, D1		@y  0~3		@src_y0[a0+src_stride]*bb
		vmull.u16 q11, d25, D3		@y  4~7		@src_y0[a0+src_stride]*bb
		vmlal.u16 q10, d26, D0		@y  0~3		@src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa
		vmlal.u16 q11, d27, D2		@y  4~7		@src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa
		
		vmul.u32  q7, q7, d13[1]
		vmla.u32  q7, q10, d13[0]	@(src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0														
		vmul.u32  q14, q14, d13[1]
		vmla.u32  q14, q11, d13[0]	@(src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0
		vshrn.u32 d14, q7, #16		@a4  = ()>>16@
		vshrn.u32 d15, q14, #16		@a4  = ()>>16@
@free register q2, q3, q4, q5, q7, q8, q9, q10, q11, q12, q13, q14
@		yyyy1[0~7] = q7
  .endmacro
  
  .macro  yuv2yuv_8x2n_num
		ldr	r10, [sp, #116]		@r10 = y_scale_par
		ldr	r0, [sp, #120]		@r0 = uin_stride		
		ldr	r2, [sp, #124]		@r2 = vin_stride	
		vld1.32	{q6}, [r10]!		
		ldr	r1, [r10], #4		@r5 = y_scale_par[0]
		ldr	r8, [r10], #4		@r6 = y_scale_par[3]
		str	r10, [sp, #116]		@r10 = y_scale_par					
		ldr	r10, [sp, #12]		@r10 = src_y
		ldr	r3, [sp, #16]		@r3 = src_u							
		ldr	r4, [sp, #96]		@r4 = src_v			
		mul	r5, r9, r1		@y_scale_par[0] * src_stride
		mul	r6, r9, r8		@y_scale_par[3] * src_stride	
		add	r5, r10, r5		@r5 = src_y0		
		add	r6, r10, r6		@r6 = src_y1
	
		mul	r11, r0, r1		@y_scale_par[0] * uin_stride
		mul	r14, r0, r8		@y_scale_par[3] * uin_stride
		add	r11, r11, r3		@r11 = src_u0
		add	r14, r14, r3		@r14 = src_u1
		
		mul	r1, r2, r1		@y_scale_par[0] * vin_stride
		mul	r8, r2, r8		@y_scale_par[3] * vin_stride
		add	r3, r1, r4		@r3 = src_v0
		add	r4, r8, r4		@r4 = src_v1
									
		macro_line_0_1 r5, r6	@y0y1
@y out_put	
		vshrn.u16 d4, q8, #4		@a4  = ()>>20@        yyyy0[0~7]			
		vshrn.u16 d7, q7, #4		@a4  = ()>>20@        yyyy1[0~7]
				
		macro_line_0_1 r11, r14	@u0u1
@u out_put
		vshrn.u16 d5, q8, #4		@a4  = ()>>20@        uuuu0[0~7]			
		vshrn.u16 d8, q7, #4		@a4  = ()>>20@        uuuu1[0~7]
		
		macro_line_0_1 r3, r4	@v0v1				
@v out_put
		vshrn.u16 d6, q8, #4		@a4  = ()>>20@        vvvv0[0~7]			
		vshrn.u16 d9, q7, #4		@a4  = ()>>20@        vvvv1[0~7]
		
	
		ldr 	r0, [sp, #100]@		r0 = out_buf
		ldr 	r2, [sp, #108]@		r2 = dst_stride		
@q6, q15 can used		
		vmovl.u8 q6, d6
		vmovl.u8 q15, d5 
		vmovl.u8 q2, d4
		vshr.u16 q6, q6, #3
		vshr.u16 q15, q15, #2		
		vshr.u16 q2, q2, #3
		vsli.u16 q6, q15, #5
		vsli.u16 q6, q2, #11
		vst1.64 {q6}, [r0], r2
		
		vmovl.u8 q6, d9
		vmovl.u8 q15, d8 
		vmovl.u8 q2, d7
		vshr.u16 q6, q6, #3
		vshr.u16 q15, q15, #2		
		vshr.u16 q2, q2, #3
		vsli.u16 q6, q15, #5
		vsli.u16 q6, q2, #11
		vst1.64 {q6}, [r0], r2		
		str 	r0, [sp, #100]@		r0 = out_buf				
		
  .endmacro

_rgb888_to_s_rgb16_8x8_armv7: @PROC
		stmdb	sp!, {r4-r11, lr}		@save regs used
		sub	sp, sp, #60			@sp - 32
		
		str	r2,	[sp, #12]		@[sp, #12] = src_y
		str	r3,	[sp, #16]		@[sp, #16] = src_u
		@str	r4,	[sp, #96]		@[sp, #96] = src_v
								
		ldr	r12,	[sp, #112]	@[sp, #112] = x_scale_par
		ldr	r9, [sp, #104]		@r9 = src_stride
		
		ldr	r0, [r12], #4
		ldr	r2, [r12], #4
		vld1.16	{q0}, [r12]!				
		ldr	r7, [r12], #4
		ldr	r10, [r12], #4
		vld1.16	{q1}, [r12]
		str	r0, [sp, #20]
		str	r2, [sp, #24]
		str	r7, [sp, #28]
		mov	r12, r10	
		
	yuv2yuv_8x2n_num 0	
		
	yuv2yuv_8x2n_num 1	
				
	yuv2yuv_8x2n_num 2	
		
	yuv2yuv_8x2n_num 3
			

  add     sp, sp, #60
	ldmia	sp!, {r4-r11, pc}		@ restore and return 
	
	
_rgb888_to_s_rgb16_8x2n_armv7: @PROC
		stmdb	sp!, {r4-r11, lr}		@save regs used
		sub	sp, sp, #60			@sp - 32

		cmp	r1, #2
		beq	lab_2222 
		cmp	r1, #4
		beq	lab_4444 
		cmp	r1, #6
		beq	lab_6666

lab_2222:		
		str	r2,	[sp, #12]		@[sp, #12] = src_y
		str	r3,	[sp, #16]		@[sp, #16] = src_u
		@str	r4,	[sp, #96]		@[sp, #96] = src_v
								
		ldr	r12,	[sp, #112]	@[sp, #112] = x_scale_par
		ldr	r9, [sp, #104]		@r9 = src_stride
		
		ldr	r0, [r12], #4
		ldr	r2, [r12], #4
		vld1.16	{q0}, [r12]!				
		ldr	r7, [r12], #4
		ldr	r10, [r12], #4
		vld1.16	{q1}, [r12]
		str	r0, [sp, #20]
		str	r2, [sp, #24]
		str	r7, [sp, #28]
		mov	r12, r10	

	  yuv2yuv_8x2n_num 0
			
    add     sp, sp, #60
	  ldmia	sp!, {r4-r11, pc}		@ restore and return 



lab_4444:		
		str	r2,	[sp, #12]		@[sp, #12] = src_y
		str	r3,	[sp, #16]		@[sp, #16] = src_u
								
		ldr	r12,	[sp, #112]	@[sp, #112] = x_scale_par
		ldr	r9, [sp, #104]		@r9 = src_stride
		
		ldr	r0, [r12], #4
		ldr	r2, [r12], #4
		vld1.16	{q0}, [r12]!				
		ldr	r7, [r12], #4
		ldr	r10, [r12], #4
		vld1.16	{q1}, [r12]
		str	r0, [sp, #20]
		str	r2, [sp, #24]
		str	r7, [sp, #28]
		mov	r12, r10	
		
	  yuv2yuv_8x2n_num 0			
	  yuv2yuv_8x2n_num 1			

    add     sp, sp, #60
		ldmia	sp!, {r4-r11, pc}		@ restore and return 


lab_6666:		
		str	r2,	[sp, #12]		@[sp, #12] = src_y
		str	r3,	[sp, #16]		@[sp, #16] = src_u
								
		ldr	r12,	[sp, #112]	@[sp, #112] = x_scale_par
		ldr	r9, [sp, #104]		@r9 = src_stride
		
		ldr	r0, [r12], #4
		ldr	r2, [r12], #4
		vld1.16	{q0}, [r12]!				
		ldr	r7, [r12], #4
		ldr	r10, [r12], #4
		vld1.16	{q1}, [r12]
		str	r0, [sp, #20]
		str	r2, [sp, #24]
		str	r7, [sp, #28]
		mov	r12, r10	
		

	  yuv2yuv_8x2n_num 0	
		
	  yuv2yuv_8x2n_num 1	
				
	  yuv2yuv_8x2n_num 2
			
    add     sp, sp, #60
		ldmia	sp!, {r4-r11, pc}		@ restore and return 



	
_rgb888_to_rgb16_8nx2n_armv7: @PROC
		stmdb	sp!, {r4-r11, lr}		@ save regs used
		sub	sp, sp, #64			@ sp - 32		

		ldr	r6, [sp, #100]			@out_buf
		ldr	r7, [sp, #104]			@out_stride
		ldr	r8, [sp, #108]			@width
		ldr	r9, [sp, #112]			@height
		ldr	r4, [sp, #116]			@in_strideu
Height_loop:
		ldr	r5, [sp, #120]			@in_stridev			

		add	r12, r1, r4
		add	r5, r2, r5
		add	r10, r0, r3
		add	r11, r6, r7
	
		mov	r14, r8	
	
Width_loop:				
 		vld1.64 {d0}, [r0]!	@y  0~7
		vld1.64 {d1}, [r10]!	@y1
		vld1.64 {d2}, [r1]!	@u  0~7
		vld1.64 {d3}, [r12]!	@u1		 		
		vld1.64 {d4}, [r2]!	@v  0~7
		vld1.64 {d5}, [r5]!	@v1		
		
		vmovl.u8 q5, d4		@B
		vmovl.u8 q6, d2 	@G
		vmovl.u8 q8, d0		@R
		vshr.u16 q5, q5, #3
		vshr.u16 q6, q6, #2		
		vshr.u16 q8, q8, #3
		vsli.u16 q5, q6, #5
		vsli.u16 q5, q8, #11
		vst1.64 {q5}, [r6]!
		
		vmovl.u8 q5, d5
		vmovl.u8 q6, d3 
		vmovl.u8 q8, d1
		vshr.u16 q5, q5, #3
		vshr.u16 q6, q6, #2		
		vshr.u16 q8, q8, #3
		vsli.u16 q5, q6, #5
		vsli.u16 q5, q8, #11
		vst1.64 {q5}, [r11]!		

	
		subs	r14, r14, #8						
		bgt		Width_loop
				
		sub	r6, r6, r8, lsl #1								
		sub	r0, r0, r8
		sub	r1, r1, r8
		sub	r2, r2, r8				
		mov	r14, r3, lsl #1 
		add	r0, r0, r14	@y
		add	r1, r1, r14	@u
		add	r2, r2, r14	@v

		add	r6, r6, r7, lsl #1	@d
		subs	r9, r9, #2					

		bgt		Height_loop

    add     sp, sp, #64
		ldmia	sp!, {r4-r11, pc}		@ restore and return 
	@ENDP  @

