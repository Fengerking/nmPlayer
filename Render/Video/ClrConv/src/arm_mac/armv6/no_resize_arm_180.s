
	@AREA	 .rdata , DATA, READONLY
	@.section .rdata

	@AREA	 .text , CODE, READONLY
	@.section .text
	#include "voYYDef_CC.h"
	.text
	
	.align 2
	
	.globl	 _cc_mb_16x16_180_arm 
	.globl	 _ccClip63 

  .macro cc_mb_16x16_180_arm_num_count
		@ldr	r9, =0x645a1			@r9 = ConstU2
		ldr   r9, ConstU2
		@ldr	r8, =0x2045a1			@r8 = ConstU1
		ldr   r8, ConstU1	
		ldrb	r6, [r1, #($0)]			@r6 = psrc_u[num]		
		ldrb	r7, [r2, #($0)]			@r7 = psrc_v[num]					
		ldrb	r10, [r0, #($0*2)]			@r10 = psrc_y[2*num]
		@ldr	r12, =0x129fbe			@r12 = ConstY	
		ldr   r12, ConstY					
		sub	r6, r6, #128			@r6 = a0
		mul	r9, r6, r9			@r9 = a0 *ConstU2
		sub	r10, r10, #16
		mul	r10, r12, r10			@*********r10 = a3 = (psrc_y[2*nu] - 16)*ConstY@
		@ldr	r11, =0xD020C			@r11 = ConstV2
		ldr   r11, ConstV2					
		sub	r7, r7, #128			@r7 = a1			
		mul	r6, r8, r6			@*********r6 = a2 = (a0 * ConstU1)@		
		@ldr	r8, =0x198937			@r8 = ConstV1
		ldr   r8, ConstV1
		mla	r11, r7, r11, r9		@*********r11 = a0 = (a1 * ConstV2 + a0 *ConstU2)@
		add	r9, r10, r6			@*********r9 = (a3 + a2)
		mul	r7, r8, r7			@*********r7 = a1 = (a1 * ConstV1)@

		ldrb	r9, [r14, +r9, asr #22]		@r9 = ccClip63[((a3 + a2)>>22)]
		add	r8, r10, r7
		sub	r10, r10, r11
		ldrb	r10, [r14, +r10, asr #22]	@r10 = a5  = (ccClip63[((a3 - a0)>>22)])@
		ldrb	r8, [r14, +r8, asr #22]		@r8 = ccClip63[((a3 + a1)>>22)]						
		mov	r9, r9, lsr #1			@r9 = a3  = (ccClip63[((a3 + a2)>>22)])>>1@
        	orr     r10, r9, r10, lsl #5            @r10 = (a5<<5) a3
		ldrb	r9, [r0, #(($0*2) + 1)]			@r7 = psrc_y[(2*num + 1)]        	
		mov	r8, r8, lsr #1			@r8 = a4  = (ccClip63[((a3 + a1)>>22)])>>1@		
        	orr     r10, r10, r8, lsl #11           @********r10 = a4 = (a4<<11) (a5<<5) a3@
        	sub	r9, r9, #16
		mul	r9, r12, r9			@r9 = a3 = (psrc_y[(2*nu + 1)] - 16)*ConstY@
		@nop	r12 can used
		
		add	r8, r9, r6	
		ldrb	r8, [r14, +r8, asr #22]		@r8 = ccClip63[((a3 + a2)>>22)]			
		add	r12, r9, r7
		ldrb	r12, [r14, +r12, asr #22]	@r12 = ccClip63[((a3 + a1)>>22)]
		sub	r9, r9, r11
		ldrb	r9, [r14, +r9, asr #22]		@r9 = a3  = (ccClip63[((a3 - a0)>>22)])@
		mov	r8, r8, lsr #1			@r8 = a5  = (ccClip63[((a3 + a2)>>22)])>>1@		
		mov	r12, r12, lsr #1		@r12 = a5  = (ccClip63[((a3 + a1)>>22)])>>1@			
        	orr     r10, r10, r8, lsl #16           @********r10 = a4  = (a5 << 16)@
        	orr     r9, r9, r12, lsl #6           	@********r9 = a3  = (a5<<6)@
        	orr     r8, r10, r9, lsl #21           @********r10 = a4  = (a3 << 21)@

			
		add	r10, r0, r3			@r8 = psrc_y+in_stride
		ldrb	r10, [r10, #($0*2 )]			@r10 = psrc_y[in_stride+2*num]
		
		mov	r12, r8, lsr #16
		orr	r8, r12, r8, lsl #16
				
		@ldr	r12, =0x129fbe			@r12 = ConstY		
		ldr  r12, ConstY
			
        	str	r8, [r4, #(28 - $0*4)]			@*((int32_t*)(out_buf+(4*num))) = (int32_t)a4@
        	sub	r10, r10, #16	
		mul	r10, r12, r10			@*********r10 = a3 = (psrc_y[in_stride+2*nu] - 16)*ConstY@		
		add	r9, r10, r6			@*********r9 = (a3 + a2)	
		
		ldrb	r9, [r14, +r9, asr #22]		@r9 = ccClip63[((a3 + a2)>>22)]
		add	r8, r10, r7
		sub	r10, r10, r11
		ldrb	r10, [r14, +r10, asr #22]	@r10 = a5  = (ccClip63[((a3 - a0)>>22)])@
		ldrb	r8, [r14, +r8, asr #22]		@r8 = ccClip63[((a3 + a1)>>22)]						
		mov	r9, r9, lsr #1			@r9 = a3  = (ccClip63[((a3 + a2)>>22)])>>1@
        	orr     r10, r9, r10, lsl #5            @r10 = (a5<<5) a3
		add	r9, r0, r3			@r8 = psrc_y+in_stride        	
		ldrb	r9, [r9, #(($0*2) + 1)]			@r7 = psrc_y[(in_stride+2*num + 1)]        	
		mov	r8, r8, lsr #1			@r8 = a4  = (ccClip63[((a3 + a1)>>22)])>>1@		
        	orr     r10, r10, r8, lsl #11           @********r10 = a4 = (a4<<11) (a5<<5) a3@
        	sub	r9, r9, #16
		mul	r9, r12, r9			@r9 = a3 = (psrc_y[(2*nu + 1)] - 16)*ConstY@
		@nop	r12 can used
		
		add	r8, r9, r6	
		ldrb	r8, [r14, +r8, asr #22]		@r8 = ccClip63[((a3 + a2)>>22)]			
		add	r12, r9, r7
		ldrb	r12, [r14, +r12, asr #22]	@r12 = ccClip63[((a3 + a1)>>22)]
		sub	r9, r9, r11
		ldrb	r9, [r14, +r9, asr #22]		@r9 = a3  = (ccClip63[((a3 - a0)>>22)])@
		mov	r8, r8, lsr #1			@r8 = a5  = (ccClip63[((a3 + a2)>>22)])>>1@		
		mov	r12, r12, lsr #1		@r12 = a5  = (ccClip63[((a3 + a1)>>22)])>>1@			
        	orr     r10, r10, r8, lsl #16           @********r10 = a4  = (a5 << 16)@
        	orr     r9, r9, r12, lsl #6           	@********r9 = a3  = (a5<<6)@
        	orr     r10, r10, r9, lsl #21           @********r10 = a4  = (a3 << 21)@
        	sub	r8, r4, r5			@r8 = out_buf+out_stride

		mov	r12, r10, lsr #16
		orr	r10, r12, r10, lsl #16
							        	
        	str	r10, [r8, #(28 - $0*4)]			@*((int32_t*)(out_buf+(out_stride+4*num))) = (int32_t)a4@						
  .endm  	
						
	
_cc_mb_16x16_180_arm: @  PROC
		stmdb	sp!, {r4-r11, lr}		@ save regs used
		sub	sp, sp, #32			@ sp - 32
		ldr	r4, [sp, #68]			@r4 = out_buf
		ldr	r5, [sp, #72]			@r5 = out_stride				
		@ldr	r14, ccClip63_DATA_D_180
		ldr	r14, ccClip63_DATA_D_180
		ldr	r14, [r14]			@r14 = ccClip63		
		mov	r12, #8				@ a6 = 8
		add		r4, r4, r5, asl	#4
		sub		r4, r4, r5
		str	r12, [sp, #4]	
BIG_WHILE_dis:

@	do{
@		int32_t a0, a1, a2, a3, a4, a5, a7@
	cc_mb_16x16_180_arm_num_count 0
	cc_mb_16x16_180_arm_num_count 1
	cc_mb_16x16_180_arm_num_count 2
	cc_mb_16x16_180_arm_num_count 3
	cc_mb_16x16_180_arm_num_count 4
	cc_mb_16x16_180_arm_num_count 5
	cc_mb_16x16_180_arm_num_count 6
	cc_mb_16x16_180_arm_num_count 7
@			psrc_y += (in_stride<<1)@
@			psrc_u += (in_stride>>1)@
@			psrc_v += (in_stride>>1)@
@			out_buf += (out_stride<<1)@
@
@	}while(--a6 != 0)@
@}
		ldr		r6, [sp, #84]@		uin_stride
		ldr		r7, [sp, #88]@		vin_stride
		ldr		r12, [sp, #4]
		sub		r4, r4, r5, asl	#1
		subs	r12, r12, #1
		strne		r12, [sp, #4]
		add		r0, r0, r3, asl	#1
		add		r1, r1, r6
		add		r2, r2, r7		

		bne		BIG_WHILE_dis

        	add     sp, sp, #32
		ldmia	sp!, {r4-r11, pc}		@ restore and return 

	@ENDP  @
		
@ccClip63_DATA_D_180
	@DCD        ccClip63 
ConstU2:	.word	0x645a1	  @ConstU2
ConstU1:	.word	0x2045a1	@ConstU1
ConstY:	  .word	0x129fbe	@ConstY
ConstV2:  .word	0xD020C	  @ConstV2
ConstV1:  .word	0x198937	@ConstV1	
ccClip63_DATA_D_180: .word _ccClip63 

