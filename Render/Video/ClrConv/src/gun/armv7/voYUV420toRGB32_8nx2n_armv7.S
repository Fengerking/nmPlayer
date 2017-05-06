@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2010		            *
@	written by Rock							 	                        *
@***********************************************************************/
	@AREA    |.text|, CODE, READONLY
	#include "voYYDef_CC.h"
	.section .text
	
	.global voyuv420torgb32_8nx2n_armv7
	.global voyuv420toargb32_8nx2n_armv7
	.global voyuv420torgb32_8nx2n_armv7_new
	.global voyuv420toargb32_8nx2n_armv7_new
	
voyuv420torgb32_8nx2n_armv7_new: @PROC
@(psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride, width, height, uin_stride, vin_stride)
	stmdb	sp!, {r4-r11, lr}	  @ save regs used
	
	@sub	sp, sp, #64				  @ sp - 32

	ldr	r6, [sp, #36]			@out_buf
	ldr	r7, [sp, #40]			@out_stride
	ldr	r8, [sp, #44]			@width
	ldr	r5, [sp, #48]			@height
	ldr	r4, [sp, #52]			@in_strideu
	ldr	r9, [sp, #56]			@in_stridev  no use
	
	adr   lr, coefficients
	vld1.s16        {d6[],  d7[]}, [lr]!
	vld1.s16        {d8[],  d9[]}, [lr]!
	vld1.s16        {d10[], d11[]}, [lr]!
	
  vmov.u8        	d0, #74 
  vmov.u8			d1, #102   
  vmov.u8        	d2, #25 
  vmov.u8         	d3, #52 
  vmov.u8			d4, #129
RGB32_Height_loop_new: 
  add             ip, r0, r3 @ip y_b_ptr
  add             fp, r6, r7 @fp out_b_ptr
  mov             r10, r8     @s1 = with
	
  ands            r9, r8, #15 @count
  moveq           r9, #16
RGB32_Width_loop_new:
	@ Load u and v
  vld1.u8         d17, [r2]      @v
  add             r2, r9, asr #1 @ v_ptr + 8
  vld1.u8         d16, [r1]      @u
  add             r1, r9, asr #1 @ u_ptr + 8

	@Calculate contribution from chrominance signals.
	@R = CLAMP((Y-16)*1.164 + 1.596*V)
	@G = CLAMP((Y-16)*1.164 - 0.391*U - 0.813*V) 
	@B = CLAMP((Y-16)*1.164 + 2.018*U )
  vmull.u8        q6, d17, d1   @1.596*V
  vmull.u8        q7, d16, d2   @0.391*U
  vmlal.u8        q7, d17, d3   @0.813*V
  vmull.u8        q8, d16, d4   @2.018*U
  
  @Add bias
  vadd.s16        q6, q6, q3
  vsub.s16        q7, q4, q7
  vadd.s16        q8, q8, q5
  
  pld             [r2] @pre load V
	pld             [r1] @pre load U
	
	@-----------Y TOP ROW even
	vld2.u8         {d24, d26}, [r0], r9  @d24: y even | d26: y odd
	vmull.u8        q15, d24, d0          @1.164*y  
	   
  vqadd.s16       q9,  q15, q6     @r
  vqadd.s16       q10, q15, q7     @g
  vqadd.s16       q11, q15, q8     @b
  @alpha 
  vmov.u8         d29, #0x00         @a
  
  vqrshrun.s16    d24, q9,  #6
  vqrshrun.s16    d28, q10, #6
  vqrshrun.s16    d25, q11, #6	
  pld             [r0]	
  vzip.u8         d25, d29   @
  vzip.u8         d24, d28   @

@Y TOP ROW odd
  vmull.u8        q15, d26, d0
  vqadd.s16       q9 , q15, q6
  vqadd.s16       q10, q15, q7
  vqadd.s16       q11, q15, q8
  vmov.u8         d31, #0x00
  
  vqrshrun.s16    d26, q9,  #6
  vqrshrun.s16    d27, q11, #6
  vqrshrun.s16    d30, q10, #6
  
  vzip.u8         d27, d31
  vzip.u8         d26, d30
	
	@ Top row: Store 16 pixels of ARGB32, interleaving even and odd
  vst4.u16        {d24, d25, d26, d27}, [r6]  @rgb_t_ptr
  add             r6, r9, lsl #1
  vst4.u16        {d28, d29, d30, d31}, [r6]
  add             r6, r9, lsl #1  
  
  @----------Y BOTTOM ROW
  vld2.u8        {d24, d26}, [ip], r9

  @ Bottom row, even
  vmull.u8        q15, d24, d0        
  vqadd.s16       q9,  q15, q6
  vqadd.s16       q10, q15, q7
  vqadd.s16       q11, q15, q8
  vmov.u8         d29, #0x00
  vqrshrun.s16    d24, q9,  #6
  vqrshrun.s16    d25, q11, #6
  vqrshrun.s16    d28, q10, #6
  pld             [ip]
  vzip.u8         d25, d29
	vzip.u8         d24, d28
	
	@ Bottom row, odd
  vmull.u8        q15, d26, d0        
  vqadd.s16       q9 , q15, q6
  vqadd.s16       q10, q15, q7
  vqadd.s16       q11, q15, q8
  vmov.u8         d31, #0x00
  
  vqrshrun.s16    d26, q9 , #6
  vqrshrun.s16    d27, q11, #6
  vqrshrun.s16    d30, q10, #6  
  
  vzip.u8         d27, d31
  vzip.u8         d26, d30
  
  subs            r10, r10, r9
  vst4.u16        {d24, d25, d26, d27}, [fp]   @rgb_b_ptr
  add             fp, r9, lsl #1                      
	vst4.u16       {d28, d29, d30, d31}, [fp] 
	add             fp, r9, lsl #1  
	
	mov             r9, #16
	bgt             RGB32_Width_loop_new
	
	sub             r6, r8, lsl #2  		@rgb_t_ptr
  sub             r0, r8             @
  sub             r1, r8, asr #1
  sub             r2, r8, asr #1
  add             r6, r7, lsl #1
  add             r0, r3, lsl #1
  add             r1, r4
  add             r2, r4
  
  @Have we reached the bottom row yet?
  subs            r5, r5, #2
  bgt             RGB32_Height_loop_new
	
  ldmia	sp!, {r4-r11, pc}		@ restore and return 
  
voyuv420toargb32_8nx2n_armv7_new: @PROC
@(psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride, width, height, uin_stride, vin_stride)
	stmdb	sp!, {r4-r11, lr}	  @ save regs used
	
	@sub	sp, sp, #64				  @ sp - 32

	ldr	r6, [sp, #36]			@out_buf
	ldr	r7, [sp, #40]			@out_stride
	ldr	r8, [sp, #44]			@width
	ldr	r5, [sp, #48]			@height
	ldr	r4, [sp, #52]			@in_strideu
	ldr	r9, [sp, #56]			@in_stridev  no use
	
	adr   lr, coefficients
	vld1.s16        {d6[],  d7[]}, [lr]!
	vld1.s16        {d8[],  d9[]}, [lr]!
	vld1.s16        {d10[], d11[]}, [lr]!
	
 vmov.u8        	d0, #74 
  vmov.u8			d1, #102   
  vmov.u8        	d2, #25 
  vmov.u8         	d3, #52 
  vmov.u8			d4, #129
ARGB32_Height_loop_new: 
  add             ip, r0, r3 @ip y_b_ptr
  add             fp, r6, r7 @fp out_b_ptr
  mov             r10, r8     @s1 = with
	
  ands            r9, r8, #15 @count
  moveq           r9, #16
ARGB32_Width_loop_new:
	@ Load u and v
  vld1.u8         d17, [r2]      @v
  add             r2, r9, asr #1 @ v_ptr + 8
  vld1.u8         d16, [r1]      @u
  add             r1, r9, asr #1 @ u_ptr + 8

	@Calculate contribution from chrominance signals.
	@R = CLAMP((Y-16)*1.164 + 1.596*V)
	@G = CLAMP((Y-16)*1.164 - 0.391*U - 0.813*V) 
	@B = CLAMP((Y-16)*1.164 + 2.018*U )
  vmull.u8        q6, d17, d1   @1.596*V
  vmull.u8        q7, d16, d2   @0.391*U
  vmlal.u8        q7, d17, d3   @0.813*V
  vmull.u8        q8, d16, d4   @2.018*U
  
  @Add bias
  vadd.s16        q6, q6, q3
  vsub.s16        q7, q4, q7
  vadd.s16        q8, q8, q5
  
  pld             [r2] @pre load V
	pld             [r1] @pre load U
	
	@-----------Y TOP ROW even
	vld2.u8         {d24, d26}, [r0], r9  @d24: y even | d26: y odd
	vmull.u8        q15, d24, d0          @1.164*y  
	   
  vqadd.s16       q9,  q15, q6     @r
  vqadd.s16       q10, q15, q7     @g
  vqadd.s16       q11, q15, q8     @b
  @alpha 
  vmov.u8         d29, #0xff         @a
  
  vqrshrun.s16    d24, q9,  #6
  vqrshrun.s16    d28, q10, #6
  vqrshrun.s16    d25, q11, #6	
  pld             [r0]	
  vzip.u8         d25, d29   @
  vzip.u8         d24, d28   @

@Y TOP ROW odd
  vmull.u8        q15, d26, d0
  vqadd.s16       q9 , q15, q6
  vqadd.s16       q10, q15, q7
  vqadd.s16       q11, q15, q8
  vmov.u8         d31, #0xff
  
  vqrshrun.s16    d26, q9,  #6
  vqrshrun.s16    d27, q11, #6
  vqrshrun.s16    d30, q10, #6
  
  vzip.u8         d27, d31
  vzip.u8         d26, d30
	
	@ Top row: Store 16 pixels of ARGB32, interleaving even and odd
  vst4.u16        {d24, d25, d26, d27}, [r6]  @rgb_t_ptr
  add             r6, r9, lsl #1
  vst4.u16        {d28, d29, d30, d31}, [r6]
  add             r6, r9, lsl #1  
  
  @----------Y BOTTOM ROW
  vld2.u8        {d24, d26}, [ip], r9

  @ Bottom row, even
  vmull.u8        q15, d24, d0        
  vqadd.s16       q9,  q15, q6
  vqadd.s16       q10, q15, q7
  vqadd.s16       q11, q15, q8
  vmov.u8         d29, #0xff
  vqrshrun.s16    d24, q9,  #6
  vqrshrun.s16    d25, q11, #6
  vqrshrun.s16    d28, q10, #6
  pld             [ip]
  vzip.u8         d25, d29
	vzip.u8         d24, d28
	
	@ Bottom row, odd
  vmull.u8        q15, d26, d0        
  vqadd.s16       q9 , q15, q6
  vqadd.s16       q10, q15, q7
  vqadd.s16       q11, q15, q8
  vmov.u8         d31, #0xff
  
  vqrshrun.s16    d26, q9 , #6
  vqrshrun.s16    d27, q11, #6
  vqrshrun.s16    d30, q10, #6  
  
  vzip.u8         d27, d31
  vzip.u8         d26, d30
  
  subs            r10, r10, r9
  vst4.u16        {d24, d25, d26, d27}, [fp]   @rgb_b_ptr
  add             fp, r9, lsl #1                      
	vst4.u16       {d28, d29, d30, d31}, [fp] 
	add             fp, r9, lsl #1  
	
	mov             r9, #16
	bgt             ARGB32_Width_loop_new
	
	sub             r6, r8, lsl #2  		@rgb_t_ptr
  sub             r0, r8             @
  sub             r1, r8, asr #1
  sub             r2, r8, asr #1
  add             r6, r7, lsl #1
  add             r0, r3, lsl #1
  add             r1, r4
  add             r2, r4
  
  @Have we reached the bottom row yet?
  subs            r5, r5, #2
  bgt             ARGB32_Height_loop_new
	
  ldmia	sp!, {r4-r11, pc}		@ restore and return 
  
  	.ALIGN 8
coefficients:
    .short  -14240  @/* bias_r = 74 * (-16)                + (102 * -128) */
    .short    8672  @/* bias_g = 74 * (-16) -  25 * (-128) - ( 52 * -128) */
    .short  -17696  @/* bias_b = 74 * (-16) + 129 * (-128)                */
    .short       0
	
  .macro	  OneYUVTORGB32Transpose
  		vld1.32 {d10[0]}, [r1]!		@u  0~3 		
		vld1.32 {d10[1]}, [r2]!		@v  0~3	
 		vld1.64 {d6}, [r0]!			@y  0~7
		vld1.32 {d7}, [r10]!		@y1
									@input:y0y1 = q3, uv = d10
		 		
		vmovl.u8 q5, d10			@uv0~3	will extern to 0~3
		vsub.s16 q5, q5, q2			@uv[0~3] - 128@					
									@first 0~3 uv
		vmull.s16 q9, d10, D0[2]	@a2[0~3] = (a0 * ConstU1)
		vmull.s16 q10, d10, D0[0]	@a0 = (a0 * ConstU2)		
		vmlal.s16 q10, d11, D0[1]	@a0[0~3] = (a1 * ConstV2 + a0 *ConstU2)
		vmull.s16 q11, d11, D0[3]	@a1[0~3] = (a1 * ConstV1)@
		vmov	    q12, q9
		vmov	    q13, q10
		vmov	    q14, q11					
		vtrn.32		q9, q12
		vtrn.32		q10, q13
		vtrn.32		q11, q14
						
		vswp.64		d19, d24		@q9=a2[0011] q12=a2[2233]
		vswp.64		d21, d26		@q10=a0[0011] q13=a0[2233]
		vswp.64		d23, d28		@q11=a1[0011] q14=a1[2233]
									@q0~3, q9~14 used q4~8, q15 can use	
									@line y0  0~7		
		vmovl.u8 q7, d6				@y  0~7		
		vsub.s16 q7, q7, q1			@y[0~7]- 16@
		vmull.s16 q8, d14, D1[0]	@a3[0~3] = (psrc_y[0] - 16)*ConstY@
		vmull.s16 q7, d15, D1[0]	@a3[4~7] = (psrc_y[0] - 16)*ConstY@
									@y[0~3]
		vadd.s32 q4, q8, q11		@a4  = a3 + a1@		
		vsub.s32 q15, q8, q10		@a5  = a3 - a0@
		vadd.s32 q8, q8, q9			@a3  = a3 + a2@
		vqshrun.s32 d10, q4, #12	@a4  = SAT((a3 + a1)>>12)@		
		vqshrun.s32 d12, q15,#12	@a5  = SAT((a3 - a0)>>12)@
		vqshrun.s32 d16, q8, #12	@a3  = SAT((a3 + a2)>>12)@
									@y[4~7]
		vadd.s32 q4, q7, q14		@a4  = a3 + a1@		
		vsub.s32 q15, q7, q13		@a5  = a3 - a0@
		vadd.s32 q7, q7, q12		@a3  = a3 + a2@
		vqshrun.s32 d11, q4, #12	@a4  = SAT((a3 + a1)>>12)@		
		vqshrun.s32 d13, q15,#12	@a5  = SAT((a3 - a0)>>12)@
		vqshrun.s32 d17, q7, #12	@a3  = SAT((a3 + a2)>>12)@
		
		vqmovn.u16 d14, q5
		vqmovn.u16 d15, q6
		vqmovn.u16 d10, q8
		
		vmovl.u8 q5, d10
		vmovl.u8 q6, d15 
		vmovl.u8 q8, d14
		
		vmovl.u16 q4, d10
		vmovl.u16 q5, d11
		vmovl.u16 q7, d12
		vmovl.u16 q6, d13
		vmovl.u16 q15,d16
		vmovl.u16 q8, d17
  
  .endm
  
  .macro	  TwoYUVTORGB32Transpose
									@line y1  0~7	
		vmovl.u8 q7, d7				@y  0~7		
		vsub.s16 q7, q7, q1			@y[0~7]- 16@
		vmull.s16 q8, d14, D1[0]	@a3[0~3] = (psrc_y[0] - 16)*ConstY@
		vmull.s16 q7, d15, D1[0]	@a3[4~7] = (psrc_y[0] - 16)*ConstY@
									@y[0~3]
		vadd.s32 q4, q8, q11		@a4  = a3 + a1@		
		vsub.s32 q15, q8, q10		@a5  = a3 - a0@
		vadd.s32 q8, q8, q9			@a3  = a3 + a2@
		vqshrun.s32 d10, q4, #12	@a4  = SAT((a3 + a1)>>12)@		
		vqshrun.s32 d12, q15,#12	@a5  = SAT((a3 - a0)>>12)@
		vqshrun.s32 d16, q8, #12	@a3  = SAT((a3 + a2)>>12)@
		@y[4~7]
		vadd.s32 q4, q7, q14		@a4  = a3 + a1@		
		vsub.s32 q15, q7, q13		@a5  = a3 - a0@
		vadd.s32 q7, q7, q12		@a3  = a3 + a2@
		vqshrun.s32 d11, q4, #12	@a4  = SAT((a3 + a1)>>12)@		
		vqshrun.s32 d13, q15,#12	@a5  = SAT((a3 - a0)>>12)@
		vqshrun.s32 d17, q7, #12	@a3  = SAT((a3 + a2)>>12)@
		
		vqmovn.u16 d14, q5
		vqmovn.u16 d15, q6
		vqmovn.u16 d10, q8
		
		vmovl.u8 q5, d10
		vmovl.u8 q6, d15 
		vmovl.u8 q8, d14
		
		vmovl.u16 q3, d10
		vmovl.u16 q4, d11
		vmovl.u16 q5, d12
		vmovl.u16 q6, d13
		vmovl.u16 q7, d16
		vmovl.u16 q8, d17
  .endm
				 
voyuv420torgb32_8nx2n_armv7: @PROC
		stmdb	sp!, {r4-r11, lr}	@ save regs used
		sub	sp, sp, #64				@ sp - 32				
		adr	r10, ConstYUV

		ldr	r6, [sp, #100]			@out_buf
		ldr	r7, [sp, #104]			@out_stride
		ldr	r8, [sp, #108]			@width
		ldr	r9, [sp, #112]			@height
		ldr	r4, [sp, #116]			@in_strideu
		ldr	r5, [sp, #120]			@in_stridev

		vld1.s32 {q0}, [r10]		@U2 = D0.S16[0]	V2 = D0.S16[1]
									@U1 = D0.S16[2] V1 = D0.S16[3]
									@Y = D1.S16[0]  Y = D1.S16[1]
									@Y = D1.S16[2]  Y = D1.S16[3]
		vmov.s16 q1, #16		
		vmov.s16 q2, #128						
								
				
RGB32_Height_loop:
		add	r10, r0, r3
		add	r11, r6, r7
		mov	r14, r8		
RGB32_Width_loop:		
		OneYUVTORGB32Transpose
		vsli.u32  q4, q7,  #8
		vsli.u32  q4, q15, #16
		vsli.u32  q5, q6,  #8
		vsli.u32  q5, q8,  #16
		vst1.64   {q4}, [r6]!
		vst1.64   {q5}, [r6]!
												
		TwoYUVTORGB32Transpose
		vsli.u32  q3, q5, #8
		vsli.u32  q3, q7, #16
		vsli.u32  q4, q6, #8
		vsli.u32  q4, q8, #16
		vst1.64   {q3}, [r11]!		
		vst1.64   {q4}, [r11]!
	
		subs	r14, r14, #8						
		bgt		RGB32_Width_loop
								
		sub	r0, r0, r8
		mov	r12, r8,  lsr #1
		sub	r1, r1, r12
		sub	r2, r2, r12
		sub	r6, r6, r8, lsl #2
		
		add	r0, r0, r3, lsl #1			@y
		add	r1, r1, r4					@u
		add	r2, r2, r5					@v
		add	r6, r6, r7, lsl #1			@d
		subs	r9, r9, #2				
		bgt		RGB32_Height_loop

        add     sp, sp, #64
		ldmia	sp!, {r4-r11, pc}		@ restore and return 
	@ENDP 
	 
voyuv420toargb32_8nx2n_armv7: @PROC
		stmdb	sp!, {r4-r11, lr}	@ save regs used
		sub	sp, sp, #64				@ sp - 32				
		adr	r10, ConstYUV

		ldr	r6, [sp, #100]			@out_buf
		ldr	r7, [sp, #104]			@out_stride
		ldr	r8, [sp, #108]			@width
		ldr	r9, [sp, #112]			@height
		ldr	r4, [sp, #116]			@in_strideu
		ldr	r5, [sp, #120]			@in_stridev

		vld1.s32 {q0}, [r10]		@U2 = D0.S16[0]	V2 = D0.S16[1]
									@U1 = D0.S16[2] V1 = D0.S16[3]
									@Y = D1.S16[0]  Y = D1.S16[1]
									@Y = D1.S16[2]  Y = D1.S16[3]
		vmov.s16 q1, #16		
		vmov.s16 q2, #128						
								
				
ARGB32_Height_loop:
		add	r10, r0, r3
		add	r11, r6, r7
		mov	r14, r8		
ARGB32_Width_loop:		
		OneYUVTORGB32Transpose
		vsli.u32  q15, q7,  #8
		mov       r12, #0xff
		vdup.32   q7,  r12
		vsli.u32  q15, q4, #16
		vsli.u32  q15, q7, #24
		
		
		vsli.u32  q8, q6,  #8
		vsli.u32  q8, q5,  #16
		vsli.u32  q8, q7,  #24
		vst1.64   {q15}, [r6]!
		vst1.64   {q8},  [r6]!
												
		TwoYUVTORGB32Transpose
		vsli.u32  q7, q5, #8
		mov       r12, #0xff
		vdup.32   q5,  r12
		vsli.u32  q7, q3, #16
		vsli.u32  q7, q5, #24
		
		
		vsli.u32  q8, q6, #8
		vsli.u32  q8, q4, #16
		vsli.u32  q8, q5, #24
		vst1.64   {q7}, [r11]!	
		vst1.64   {q8}, [r11]!	
	
		subs	r14, r14, #8						
		bgt		ARGB32_Width_loop
								
		sub	r0, r0, r8
		mov	r12, r8,  lsr #1
		sub	r1, r1, r12
		sub	r2, r2, r12
		sub	r6, r6, r8, lsl #2
		
		add	r0, r0, r3, lsl #1			@y
		add	r1, r1, r4					@u
		add	r2, r2, r5					@v
		add	r6, r6, r7, lsl #1			@d
		subs	r9, r9, #2				
		bgt		ARGB32_Height_loop

        add     sp, sp, #64
		ldmia	sp!, {r4-r11, pc}		@ restore and return 
	@ENDP  

	.ALIGN 8
ConstYUV:		.word	0x0D020645	@V2U2
ConstV1U1:	.word	0x19892045	@V1U1
ConstY:			.word	0x0000129F	@Y
ConstY1:		.word	0x0000129F	@Y	
.END
