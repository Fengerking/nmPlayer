	@AREA	 .rdata , DATA, READONLY
	@.globl .rdata

	@AREA	 .text , CODE, READONLY
	#include "voYYDef_CC.h"
	.text 
	
	.align 2
	
	.globl	 _ccClip63 	
	.globl	 _cc_yuv420_mb_s_180_arm 

_cc_yuv420_mb_s_180_arm:  @PROC


		stmdb	sp!, {r4-r11, lr}		@ save regs used
		sub	sp, sp, #60			@ sp - 32
			
		ldr		r7, [sp, #108]			@dst_stride	
		ldr		r5, [sp, #100]			@dst				
		ldr	r14, ccClip63_DATA_180
		ldr	r4, [sp, #104]			@ r4 = src_stride
		str	r0,	[sp, #4]			@ [sp, #4] = width
		str	r1,	[sp, #8]			@ [sp, #8] = height						
		str	r2,	[sp, #12]			@ [sp, #12] = src_y
		sub	r1, r1, #1
		mul	r12, r7, r1
		add	r5, r5, r12
		str	r5, [sp, #100]

		ldr	r14, [r14]			@r14 = ccClip63		
		str	r3,	[sp, #16]			@ [sp, #16] = src_u
		str	r0,	[sp, #48]					
		str	r14, [sp, #20]			@ [sp, #20] = ccClip63		
		str	r5, [sp, #44]				
				
@	do{
@		int32_t *x_scale_par0 = x_scale_par@
@		
@		uint8_t *src_y0 = src_y + y_scale_par[0] * src_stride@ // TBD
@		uint8_t *src_u0 = src_u + ((y_scale_par[0]>>1) * (src_stride>>1))@
@		uint8_t *src_v0 = src_v + ((y_scale_par[0]>>1) * (src_stride>>1))@
@		uint8_t *src_y1 = src_y + y_scale_par[3] * src_stride@
@		 y_scale_par += 6@		
@		i = width@
BIG_WHILE_D_180:
		ldr	r3, [sp, #116]			@ r3 = y_scale_par	
		ldr	r12, [sp, #112]			@ r12 = x_scale_par		
		ldr	r0, [sp, #12]			@ r0 = src_y
		ldr	r1, [sp, #16]			@ r1 = src_u		
		ldr	r6, [r3, #0]				
		ldr	r2, [sp, #96]			@ r2 = src_v
		ldr	r7, [r3, #12]
		ldr	r14, [sp, #44]
		ldr	r11, [sp, #48]			@ [sp, #48] = width		
		ldr	r9, [sp, #108]		
		mul	r8, r4, r6			@y_scale_par[0] * src_stride				
		add	r5, r3, #24
		mul	r7, r4, r7		
		str	r5, [sp, #116]
		add r11, r14, r11, lsl #1
		sub	r11, r11, #4
		add	r5, r0, r7			@r5 = src_y1		
		add	r0, r0, r8			@r0 = src_y0
		mov	r6, r6, asr #1
		ldr		r7, [sp, #120]	@		uin_stride	
@		mov	r7, r4, asr #1		
		str	r11, [sp, #100]			
		sub	r14, r14, r9, asl #1
		mul	r7, r6, r7			@((y_scale_par[0]>>1) * (src_stride>>1))
		str	r14, [sp, #44]			
		add	r1, r1, r7			@r1 = src_u0
		ldr		r7, [sp, #124]	@		uin_stride
		str	r1, [sp, #24]			@ [sp, #24] = src_u0			
		mul	r7, r6, r7			@((y_scale_par[0]>>1) * (src_stride>>1))				
		add	r2, r2, r7			@r2 = src_v0
		
		str	r2, [sp, #28]			@ [sp, #28] = src_v0	
				
@ @r0 = src_y0, @r3 = y_scale_par, @ r4 = src_stride, @r5 = src_y1, @ r12 = x_scale_par
@ temp register r1, r2, r6~r11, r14
LIT_WHILE_D_180:
						
@		do{
@			int32_t a0, a1, a2, a3, a4, a5, a6, a7, a8@
@			int32_t aa, bb@
@			// u, v
@			a0 = x_scale_par0[0]@	
@			a6 = x_scale_par0[3]@
@			a1 = (a0 + a6 + 1)>>2@
@			a2 = src_u0[a1]@
@			a3 = src_v0[a1]@
@			a2 -= 128@
@			a3 -= 128@
@			a0 = (a3 * ConstV1)@
@			a1 = (a3 * ConstV2 + a2 *ConstU2)@
@			a2 = (a2 * ConstU1)@

		ldr	r7, [r12, #12]
		ldr	r6, [r12, #0]
		@ldr	r9, =0x645a1			@r8 = ConstU2	
		ldr r9, ConstU2
		add	r7, r7, #1
		add	r7, r6, r7

		mov	r7, r7, asr #2		@a1 = (a0 + a6 + 1)>>2@		
		ldrb	r8, [r2, r7]		@a3 = src_v0[a1]@	
		ldrb	r7, [r1, r7]		@a2 = src_u0[a1]@
		
		@ldr	r1, =0x198937			@r8 = ConstV1
		ldr r1, ConstV1
		@ldr	r2, =0xD020C			@r9 = ConstV2
		ldr r2, ConstV2
				
		sub	r8, r8, #128			@a3 -= 128@	
		mul	r1, r8, r1			@r1 = a0 = (a3 * ConstV1)@
		sub	r7, r7, #128			@a2 -= 128@		
		mul	r2, r8, r2			@r2 = a3 * ConstV2@
		
		@ldr	r8, =0x2045a1			@r9 = ConstU1
		ldr r8, ConstU1		
		mul	r9, r7, r9			@a2 *ConstU2
		str	r1, [sp, #32]			@a0 = [sp, #32]			
		add	r2, r2, r9			@r2 = a1 = (a3 * ConstV2 + a2 *ConstU2)@
		mul	r8, r7, r8			@r9 = a2 = (a2 * ConstU1)@	
		str	r2, [sp, #36]			@a1 = [sp, #36]			
		str	r8, [sp, #40]			@a2 = [sp, #40]
			
@ r1, r2, r6, r7, r8, r9 can't used r10, r11, r14		
@ @r0 = src_y0, @r3 = y_scale_par, @ r4 = src_stride, @r5 = src_y1, @ r12 = x_scale_par, r6 = x_scale_par0[0]
@ temp register r1, r2, r7~r11, r14				
					
@		 int32_t aa0 = y_scale_par[1]@
@		 int32_t bb0 = y_scale_par[2]@
@		 int32_t aa1 = y_scale_par[4]@
@		 int32_t bb1 = y_scale_par[5]@	
@
@			a6 = x_scale_par0[0]@
@			aa = x_scale_par0[1]@
@			bb = x_scale_par0[2]@
@
@			a4 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20@
@			a5 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20@
@	
@			a4 = (a4 - 16) * ConstY@
@			a5 = (a5 - 16) * ConstY@

@		ldr	r6, [r12, #0]		@r6 = a6		
		ldr	r10, [r12, #4]		@r10 = aa				
		ldr	r11, [r12, #8]		@r11 = bb
		
		add	r8, r0, r6		
		ldrb	r1, [r8]		@src_y0[a6]
		ldrb	r7, [r8, r4]		@src_y0[a6+src_stride]			
		ldrb	r2, [r8,#1]!		@src_y0[a6+1]	
		ldrb	r9, [r8, r4]		@src_y0[a6+src_stride+1]
		
		mul	r1, r11, r1		@src_y0[a6]*bb
		add	r8, r5, r6		
		mla	r1, r2, r10, r1		@r1 = (src_y0[a6]*bb + src_y0[a6+1]*aa)
		ldrb	r2, [r8]		@src_y1[a6]		
		mul	r6, r10, r9		@src_y0[a6+src_stride+1]*aa		
		ldrb	r9, [r8, r4]		@src_y1[a6+src_stride]		
		ldrb	r14, [r8,#1]!		@src_y1[a6+1]		
		mla	r6, r7, r11, r6		@r6 = (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)
		ldrb	r7, [r8,r4]		@src_y1[a6+src_stride+1]		
		
		mul	r2, r11, r2		@src_y1[a6]*bb	
		ldr	r8, [r3, #4]		@aa0		
		mla	r2, r14, r10, r2	@r2 = (src_y1[a6]*bb + src_y1[a6+1]*aa)	
		ldr	r14, [r3, #8]		@bb0		
		mul	r7, r10, r7		@src_y1[a6+src_stride+1]*aa
		ldr	r10, [r3, #20]		@bb1		
		mla	r7, r9, r11, r7		@r7 = (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)
		ldr	r11, [r3, #16]		@aa1
		
		mul	r1, r14, r1	
		@ldr	r9, =0x129fbe			@r8 = ConstY	
		ldr r9, ConstY		
		mla	r1, r8, r6, r1		@((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)
		
		ldr	r14, [sp, #20]			@ [sp, #20] = ccClip63		
		mul	r2, r10, r2
		mov	r1, r1, asr #20		
		mla	r2, r7, r11, r2		@((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)

		sub	r1, r1, #16
		mul	r1, r9, r1
		
		mov	r2, r2, asr #20		
		sub	r2, r2, #16
		mul	r2, r9, r2
		
@ @r0 = src_y0, @r3 = y_scale_par, @ r4 = src_stride, @r5 = src_y1, @ r12 = x_scale_par
@ r1 = a4, r2 = a5
@ temp register r6~r11, r14		
		
@			a4_temp = ((ccClip63[(a4 + a0)>>22]>>1)<<11) ((ccClip63[(a4 - a1)>>22])<<5) (ccClip63[(a4 + a2)>>22]>>1)@	
@			a7_temp = ((ccClip63[(a5 + a0)>>22]>>1)<<11) ((ccClip63[(a5 - a1)>>22])<<5) (ccClip63[(a5 + a2)>>22]>>1)@

		ldr	r6, [sp, #32]			@a0 = [sp, #32]
		ldr	r7, [sp, #36]			@a1 = [sp, #36]			
		ldr	r8, [sp, #40]			@a2 = [sp, #40]	
		
		add	r9, r1, r6		
		sub	r10, r1, r7
		add	r11, r1, r8
		
		ldrb	r9, [r14, r9, asr #22]		
		ldrb	r10, [r14, r10, asr #22]	
		ldrb	r11, [r14, r11, asr #22]
		
		mov	r9, r9, lsr #1							
		
		mov	r9, r9, lsl #11
		orr	r9, r9, r10, lsl #5
		orr	r1, r9, r11, lsr #1		@a4 = ((ccClip63[(a4 + a0)>>22]>>1)<<11) ((ccClip63[(a4 - a1)>>22])<<5) (ccClip63[(a4 + a2)>>22]>>1)@
		
		add	r9, r2, r6		
		sub	r10, r2, r7
		add	r11, r2, r8
		
		ldrb	r9, [r14, r9, asr #22]		
		ldrb	r10, [r14, r10, asr #22]	
		ldrb	r11, [r14, r11, asr #22]	
		
		mov	r9, r9, lsr #1	
				
		mov	r9, r9, lsl #11
		orr	r9, r9, r10, lsl #5
		orr	r2, r9, r11, lsr #1		@a7 = ((ccClip63[(a5 + a0)>>22]>>1)<<11) ((ccClip63[(a5 - a1)>>22])<<5) (ccClip63[(a5 + a2)>>22]>>1)@											
								
		str	r1, [sp, #52]			@a1 = [sp, #52]			
		str	r2, [sp, #56]			@a2 = [sp, #56]

@			/////////////////////////////////////////////////////////////////////
@			/////////////////////////////////////////////////////////////////////
@ @r0 = src_y0, @r3 = y_scale_par, @ r4 = src_stride, @r5 = src_y1, @ r12 = x_scale_par
@ temp register r1, r2, r6~r11, r14				
					
@		 int32_t aa0 = y_scale_par[1]@
@		 int32_t bb0 = y_scale_par[2]@
@		 int32_t aa1 = y_scale_par[4]@
@		 int32_t bb1 = y_scale_par[5]@	
@
@			a6 = x_scale_par0[3]@
@			aa = x_scale_par0[4]@
@			bb = x_scale_par0[5]@
@
@			a4 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20@
@			a5 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20@
@	
@			a4 = (a4 - 16) * ConstY@
@			a5 = (a5 - 16) * ConstY@

		ldr	r6, [r12, #12]		@r9 = a6		
		ldr	r10, [r12, #16]		@r10 = aa				
		ldr	r11, [r12, #20]		@r11 = bb
		
		add	r8, r0, r6		
		ldrb	r1, [r8]		@src_y0[a6]
		ldrb	r7, [r8, r4]		@src_y0[a6+src_stride]			
		ldrb	r2, [r8,#1]!		@src_y0[a6+1]	
		ldrb	r9, [r8, r4]		@src_y0[a6+src_stride+1]
		
		mul	r1, r11, r1		@src_y0[a6]*bb
		add	r8, r5, r6		
		mla	r1, r2, r10, r1		@r1 = (src_y0[a6]*bb + src_y0[a6+1]*aa)
		ldrb	r2, [r8]		@src_y1[a6]		
		mul	r6, r10, r9		@src_y0[a6+src_stride+1]*aa		
		ldrb	r9, [r8, r4]		@src_y1[a6+src_stride]		
		ldrb	r14, [r8,#1]!		@src_y1[a6+1]		
		mla	r6, r7, r11, r6		@r6 = (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)
		ldrb	r7, [r8,r4]		@src_y1[a6+src_stride+1]		
		
		mul	r2, r11, r2		@src_y1[a6]*bb	
		ldr	r8, [r3, #4]		@aa0		
		mla	r2, r14, r10, r2	@r2 = (src_y1[a6]*bb + src_y1[a6+1]*aa)	
		ldr	r14, [r3, #8]		@bb0		
		mul	r7, r10, r7		@src_y1[a6+src_stride+1]*aa
		ldr	r10, [r3, #20]		@bb1		
		mla	r7, r9, r11, r7		@r7 = (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)
		ldr	r11, [r3, #16]		@aa1
		
		mul	r1, r14, r1	
		@ldr	r9, =0x129fbe			@r8 = ConstY
		ldr r9, ConstY			
		mla	r1, r8, r6, r1		@((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)
		
		ldr	r14, [sp, #20]			@ [sp, #20] = ccClip63		
		mul	r2, r10, r2
		mov	r1, r1, asr #20		
		mla	r2, r7, r11, r2		@((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)

		sub	r1, r1, #16
		mul	r1, r9, r1
		
		mov	r2, r2, asr #20		
		sub	r2, r2, #16
		mul	r2, r9, r2
		
@ @r0 = src_y0, @r3 = y_scale_par, @ r4 = src_stride, @r5 = src_y1, @ r12 = x_scale_par
@ r1 = a4, r2 = a5
@ temp register r6~r11, r14		
		
@			a4 = ((ccClip63[(a4 + a0)>>22]>>1)<<11) ((ccClip63[(a4 - a1)>>22])<<5) (ccClip63[(a4 + a2)>>22]>>1)@	
@			a7 = ((ccClip63[(a5 + a0)>>22]>>1)<<11) ((ccClip63[(a5 - a1)>>22])<<5) (ccClip63[(a5 + a2)>>22]>>1)@

		ldr	r6, [sp, #32]			@a0 = [sp, #32]
		ldr	r7, [sp, #36]			@a1 = [sp, #36]			
		ldr	r8, [sp, #40]			@a2 = [sp, #40]	
		
		add	r9, r1, r6		
		sub	r10, r1, r7
		add	r11, r1, r8
		
		ldrb	r9, [r14, r9, asr #22]		
		ldrb	r10, [r14, r10, asr #22]	
		ldrb	r11, [r14, r11, asr #22]
		
		mov	r9, r9, lsr #1							
		
		mov	r9, r9, lsl #11
		orr	r9, r9, r10, lsl #5
		orr	r1, r9, r11, lsr #1		@a4 = ((ccClip63[(a4 + a0)>>22]>>1)<<11) ((ccClip63[(a4 - a1)>>22])<<5) (ccClip63[(a4 + a2)>>22]>>1)@
		
		add	r9, r2, r6		
		sub	r10, r2, r7
		add	r11, r2, r8
		
		ldrb	r9, [r14, r9, asr #22]		
		ldrb	r10, [r14, r10, asr #22]	
		ldrb	r11, [r14, r11, asr #22]	
		
		mov	r9, r9, lsr #1	
		
		ldr	r6, [sp, #52]			@a1 = [sp, #52]			
		ldr	r7, [sp, #56]			@a2 = [sp, #56]
						
		mov	r9, r9, lsl #11
		orr	r9, r9, r10, lsl #5
		orr	r2, r9, r11, lsr #1		@a7 = ((ccClip63[(a5 + a0)>>22]>>1)<<11) ((ccClip63[(a5 - a1)>>22])<<5) (ccClip63[(a5 + a2)>>22]>>1)@											
			
			
		ldr	r8, [sp, #100]
		ldr	r9, [sp, #108]
							
		orr	r6, r6, r1, lsl #16	
		orr	r7, r7, r2, lsl #16
											
@			a4_temp  = (a4 << 16)@
@			a7_temp  = (a7 << 16)@
@			*((uint32_t*)dst) = a4_temp@
@			*((uint32_t*)(dst+dst_stride)) = a7_temp@
@			dst += 4@

		mov	r1, r7, lsr #16
		orr	r7, r1, r7, lsl #16
		
		mov	r1, r6, lsr #16
		orr	r6, r1, r6, lsl #16
		
		ldr	r1, [sp, #4]			@ [sp, #4] = width
						
		str	r7, [r8, -r9]
		str	r6, [r8], #-4			
		str	r8, [sp, #100]
		
@			x_scale_par0 += 6@
		add	r12, r12, #24
@			

		
		subs	r1, r1, #2
		strne		r1, [sp, #4]
		ldrne	r1, [sp, #24]			@ [sp, #24] = src_u0	
		ldrne	r2, [sp, #28]			@ [sp, #28] = src_v0		

		bne		LIT_WHILE_D_180

@		}while((i-=2) != 0)@

		ldr	r2, [sp, #8]			@ [sp, #8] = height
@		ldr	r1, [sp, #44]
		ldr	r6, [sp, #48]						
		subs	r2, r2, #2
@		strne	r1, [sp, #100]		
		strne	r6, [sp, #4]					
		strne	r2, [sp, #8]
		bne	BIG_WHILE_D_180				
	
@		dst -= (width<<1)@
@		dst += (dst_stride<<1)@
@	}while((height-=2) != 0)@
@}
        	add     sp, sp, #60
		ldmia	sp!, {r4-r11, pc}		@ restore and return 

	@ENDP  @
ConstU2:	.word	0x645a1	  @ConstU2
ConstU1:	.word	0x2045a1	@ConstU1
ConstY:	  .word	0x129fbe	@ConstY
ConstV2:  .word	0xD020C	  @ConstV2
ConstV1:  .word	0x198937	@ConstV1		
ccClip63_DATA_180: .word _ccClip63
		

