	@AREA	|_rdata|, DATA, READONLY
	@EXPORT	|get420_data_asm|
	@EXPORT	|get422pblockdata_asm|
	@EXPORT	|get422iYUYVblockdata_asm|
	@EXPORT	|get422iYVYUblockdata_asm|
	@EXPORT	|get422iUYVYblockdata_asm|
	@EXPORT	|get422iVYUYblockdata_asm|
	#include "../../inc/voMJPEGEncID.h"
	
	.section .text
	.global	get420_data_asm
	.global	get422pblockdata_asm
	.global	get422iYUYVblockdata_asm
	.global	get422iYVYUblockdata_asm
	.global	get422iUYVYblockdata_asm
	.global	get422iVYUYblockdata_asm

_cc0X80808080:
        	.word   0x80808080

  .macro  get420_data_asm_loop
        ldrd	r4, [r2], r0  
        ldrd	r6, [r2], r0
@stall
		eor		r4, r4, r12
		eor		r5, r5, r12
		sxtb16	r10, r4
		sxtb16	r3, r4, ror #8
		sxtb16	r8, r5
		sxtb16	r11, r5, ror #8
		pkhtb	r5, r3, r10, asr #16
		pkhbt	r4, r10, r3, lsl #16
		pkhtb	r9, r11, r8, asr #16
		pkhbt	r8, r8, r11, lsl #16
		strd	r4, [r14], #8
		strd	r8, [r14], #8 
		eor		r6, r6, r12
		eor		r7, r7, r12
		sxtb16	r10, r6
		sxtb16	r3, r6, ror #8
		sxtb16	r8, r7
		pkhtb	r11, r3, r10, asr #16
		pkhbt	r10, r10, r3, lsl #16
		sxtb16	r3, r7, ror #8
        ldrd	r4, [r2], r0  
        ldrd	r6, [r2], r0
		pkhtb	r9, r3, r8, asr #16
		pkhbt	r8, r8, r3, lsl #16
		strd	r10, [r14], #8
		strd	r8, [r14], #8 

@2
		eor		r4, r4, r12
		eor		r5, r5, r12
		sxtb16	r10, r4
		sxtb16	r3, r4, ror #8
		sxtb16	r8, r5
		sxtb16	r11, r5, ror #8
		pkhtb	r5, r3, r10, asr #16
		pkhbt	r4, r10, r3, lsl #16
		pkhtb	r9, r11, r8, asr #16
		pkhbt	r8, r8, r11, lsl #16
		strd	r4, [r14], #8
		strd	r8, [r14], #8 
		eor		r6, r6, r12
		eor		r7, r7, r12
		sxtb16	r10, r6
		sxtb16	r3, r6, ror #8
		sxtb16	r8, r7
		pkhtb	r11, r3, r10, asr #16
		pkhbt	r10, r10, r3, lsl #16
		sxtb16	r3, r7, ror #8
        ldrd	r4, [r2], r0  
        ldrd	r6, [r2], r0
		pkhtb	r9, r3, r8, asr #16
		pkhbt	r8, r8, r3, lsl #16
		strd	r10, [r14], #8
		strd	r8, [r14], #8 

@3
		eor		r4, r4, r12
		eor		r5, r5, r12
		sxtb16	r10, r4
		sxtb16	r3, r4, ror #8
		sxtb16	r8, r5
		sxtb16	r11, r5, ror #8
		pkhtb	r5, r3, r10, asr #16
		pkhbt	r4, r10, r3, lsl #16
		pkhtb	r9, r11, r8, asr #16
		pkhbt	r8, r8, r11, lsl #16
		strd	r4, [r14], #8
		strd	r8, [r14], #8 
		eor		r6, r6, r12
		eor		r7, r7, r12
		sxtb16	r10, r6
		sxtb16	r3, r6, ror #8
		sxtb16	r8, r7
		pkhtb	r11, r3, r10, asr #16
		pkhbt	r10, r10, r3, lsl #16
		sxtb16	r3, r7, ror #8
        ldrd	r4, [r2], r0  
        ldrd	r6, [r2], r0
		pkhtb	r9, r3, r8, asr #16
		pkhbt	r8, r8, r3, lsl #16
		strd	r10, [r14], #8
		strd	r8, [r14], #8 

@4
		eor		r4, r4, r12
		eor		r5, r5, r12
		sxtb16	r10, r4
		sxtb16	r3, r4, ror #8
		sxtb16	r8, r5
		sxtb16	r11, r5, ror #8
		pkhtb	r5, r3, r10, asr #16
		pkhbt	r4, r10, r3, lsl #16
		pkhtb	r9, r11, r8, asr #16
		pkhbt	r8, r8, r11, lsl #16
		strd	r4, [r14], #8
		strd	r8, [r14], #8 
		eor		r6, r6, r12
		eor		r7, r7, r12
		sxtb16	r10, r6
		sxtb16	r3, r6, ror #8
		sxtb16	r8, r7
		pkhtb	r11, r3, r10, asr #16
		pkhbt	r10, r10, r3, lsl #16
		sxtb16	r3, r7, ror #8
		pkhtb	r9, r3, r8, asr #16
		pkhbt	r8, r8, r3, lsl #16
		strd	r10, [r14], #8
		strd	r8, [r14], #8 
  .endm
  
  
get420_data_asm:
		stmdb	sp!, {r4-r11, lr}				@ save regs used
@INT32 get420_data(INT32 stride0, INT16 **block,UINT8 *Y1,UINT8 *Y2,
@				UINT8 *Y3,UINT8 *Y4,UINT8 *U,UINT8 *V,INT32 stride1, INT32 stride2)
@INT32 get420_data_t(INT32 stride0, INT16 **block,UINT8 *Y1)
@{
@	register INT32 i,k@
@	INT16 T128 = 128@
@	INT16 *blockdata@
@		for(i = 0@ i < 8@ i++)
@		{
@			k = i << 3@	
@			blockdata = block[0] + k@
@			*blockdata++ = Y1[0] - T128@
@			*blockdata++ = Y1[1] - T128@
@			*blockdata++ = Y1[2] - T128@
@			*blockdata++ = Y1[3] - T128@
@			*blockdata++ = Y1[4] - T128@
@			*blockdata++ = Y1[5] - T128@
@			*blockdata++ = Y1[6] - T128@
@			*blockdata++ = Y1[7] - T128@		
@			Y1 += stride0@
@		}
@}
		sub		sp, sp, #4
		str		r3, [sp]
        ldr     r12, _cc0X80808080	
        ldr     r14, [r1, #0]                     @  block[0] 										       
@Y1  
		get420_data_asm_loop 

@        mov		r2, r3
		ldr		r2, [sp]
		add		sp, sp, #4
        ldr     r14, [r1, #4]                     @  block[1] 												       
@Y2  
		get420_data_asm_loop 

        ldr     r2, [r13, #36]                     @  Y3 
        ldr     r14, [r1, #8]                     @  block[2] 										       
@Y3  
		get420_data_asm_loop 

        ldr     r2, [r13, #40]                     @  Y4 
        ldr     r14, [r1, #12]                     @  block[3] 										       
@Y4  
		get420_data_asm_loop 

        ldr     r2, [r13, #44]                   @  U 
        ldr     r0, [r13, #52]                   @  stride1 
        ldr     r14, [r1, #16]                     @  block[4] 										       
@U  
		get420_data_asm_loop 

        ldr     r2, [r13, #48]                   @  V 
        ldr     r0, [r13, #56]                   @  stride2 
        ldr     r14, [r1, #20]                   @  block[5] 										       
@V  
		get420_data_asm_loop 

		ldmia	sp!, {r4-r11, pc}		@ restore and return


get422pblockdata_asm:
		stmdb	sp!, {r4-r11, lr}				@ save regs used

		sub		sp, sp, #4
		str		r3, [sp]

        ldr     r12, _cc0X80808080		
        ldr     r14, [r1, #0]                     @  block[0] 										       
@Y1  
		get420_data_asm_loop 

 @       mov		r2, r3
@        mov		r2, r3
		ldr		r2, [sp]
		add		sp, sp, #4

        ldr     r14, [r1, #4]                     @  block[1] 										       
@Y2  
		get420_data_asm_loop 
@Y3  									       
@Y4  

        ldr     r2, [r13, #44]                   @  U 
        ldr     r0, [r13, #52]                   @  stride1 
        ldr     r14, [r1, #8]                     @  block[4] 										       
@U  
		get420_data_asm_loop 

        ldr     r2, [r13, #48]                   @  V 
        ldr     r0, [r13, #56]                   @  stride2 
        ldr     r14, [r1, #12]                   @  block[5] 										       
@V  
		get420_data_asm_loop 

		ldmia	sp!, {r4-r11, pc}		@ restore and return

_cc0X00ff00ff_422iYUYV:
        	.word   0x00ff00ff

_cc0X00800080_422iYUYV:
        	.word   0x00800080
get422iYUYVblockdata_asm:
		stmdb	sp!, {r4-r11, lr}				@ save regs used

        ldr     r3, _cc0X00ff00ff_422iYUYV
        ldr     r11, _cc0X00800080_422iYUYV
		mov		r12, r2
		
        ldr     r14, [r1, #0]                     @  block[0] 										       
@Y1  
		ldrd	r6, [r2, #8]
		ldrd	r4, [r2], r0
		ldrd	r8, [r2, #8]
		and		r6, r3, r6	
		and		r7, r3, r7	
		and		r4, r3, r4	
		and		r5, r3, r5	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2], r0
		and		r8, r3, r8	
		and		r9, r3, r9	
		and		r4, r3, r4	
		and		r5, r3, r5	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r8, r8, r11
		qsub16	r9, r9, r11
		strd	r4, [r14], #8 
		strd	r8, [r14], #8 
@2
		ldrd	r6, [r2, #8]
		ldrd	r4, [r2], r0
		ldrd	r8, [r2, #8]
		and		r6, r3, r6	
		and		r7, r3, r7	
		and		r4, r3, r4	
		and		r5, r3, r5	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2], r0
		and		r8, r3, r8	
		and		r9, r3, r9	
		and		r4, r3, r4	
		and		r5, r3, r5	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r8, r8, r11
		qsub16	r9, r9, r11
		strd	r4, [r14], #8 
		strd	r8, [r14], #8
@3
		ldrd	r6, [r2, #8]
		ldrd	r4, [r2], r0
		ldrd	r8, [r2, #8]
		and		r6, r3, r6	
		and		r7, r3, r7	
		and		r4, r3, r4	
		and		r5, r3, r5	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2], r0
		and		r8, r3, r8	
		and		r9, r3, r9	
		and		r4, r3, r4	
		and		r5, r3, r5	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r8, r8, r11
		qsub16	r9, r9, r11
		strd	r4, [r14], #8 
		strd	r8, [r14], #8 
@4
		ldrd	r6, [r2, #8]
		ldrd	r4, [r2], r0
		ldrd	r8, [r2, #8]
		and		r6, r3, r6	
		and		r7, r3, r7	
		and		r4, r3, r4	
		and		r5, r3, r5	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2], r0
		and		r8, r3, r8	
		and		r9, r3, r9	
		and		r4, r3, r4	
		and		r5, r3, r5	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r8, r8, r11
		qsub16	r9, r9, r11
		strd	r4, [r14], #8 
		strd	r8, [r14], #8

        mov		r2, r12
        ldr     r14, [r1, #4]                     @  block[1] 										       
@Y2  
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4	
		and		r5, r3, r5
		and		r6, r3, r6	
		and		r7, r3, r7			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4	
		and		r5, r3, r5
		and		r6, r3, r6	
		and		r7, r3, r7			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8
@2
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4	
		and		r5, r3, r5
		and		r6, r3, r6	
		and		r7, r3, r7			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4	
		and		r5, r3, r5
		and		r6, r3, r6	
		and		r7, r3, r7			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8
@3
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4	
		and		r5, r3, r5
		and		r6, r3, r6	
		and		r7, r3, r7			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4	
		and		r5, r3, r5
		and		r6, r3, r6	
		and		r7, r3, r7			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8
@4
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4	
		and		r5, r3, r5
		and		r6, r3, r6	
		and		r7, r3, r7			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4	
		and		r5, r3, r5
		and		r6, r3, r6	
		and		r7, r3, r7			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8
										       
        mov		r2, r12
        ldr     r14, [r1, #8]                     @  block[1] 										       
@Y3  
		ldrb	r3, [r2, #1]
		ldrb	r4, [r2, #5]
		ldrb	r5, [r2, #9]
		ldrb	r6, [r2, #13]
		ldrb	r7, [r2, #17]
		ldrb	r8, [r2, #21]
		ldrb	r9, [r2, #25]
		ldrb	r10, [r2, #29]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

		ldrb	r3, [r2, #1]
		ldrb	r4, [r2, #5]
		ldrb	r5, [r2, #9]
		ldrb	r6, [r2, #13]
		ldrb	r7, [r2, #17]
		ldrb	r8, [r2, #21]
		ldrb	r9, [r2, #25]
		ldrb	r10, [r2, #29]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@2
		ldrb	r3, [r2, #1]
		ldrb	r4, [r2, #5]
		ldrb	r5, [r2, #9]
		ldrb	r6, [r2, #13]
		ldrb	r7, [r2, #17]
		ldrb	r8, [r2, #21]
		ldrb	r9, [r2, #25]
		ldrb	r10, [r2, #29]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

		ldrb	r3, [r2, #1]
		ldrb	r4, [r2, #5]
		ldrb	r5, [r2, #9]
		ldrb	r6, [r2, #13]
		ldrb	r7, [r2, #17]
		ldrb	r8, [r2, #21]
		ldrb	r9, [r2, #25]
		ldrb	r10, [r2, #29]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@3
		ldrb	r3, [r2, #1]
		ldrb	r4, [r2, #5]
		ldrb	r5, [r2, #9]
		ldrb	r6, [r2, #13]
		ldrb	r7, [r2, #17]
		ldrb	r8, [r2, #21]
		ldrb	r9, [r2, #25]
		ldrb	r10, [r2, #29]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

		ldrb	r3, [r2, #1]
		ldrb	r4, [r2, #5]
		ldrb	r5, [r2, #9]
		ldrb	r6, [r2, #13]
		ldrb	r7, [r2, #17]
		ldrb	r8, [r2, #21]
		ldrb	r9, [r2, #25]
		ldrb	r10, [r2, #29]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@4
		ldrb	r3, [r2, #1]
		ldrb	r4, [r2, #5]
		ldrb	r5, [r2, #9]
		ldrb	r6, [r2, #13]
		ldrb	r7, [r2, #17]
		ldrb	r8, [r2, #21]
		ldrb	r9, [r2, #25]
		ldrb	r10, [r2, #29]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

		ldrb	r3, [r2, #1]
		ldrb	r4, [r2, #5]
		ldrb	r5, [r2, #9]
		ldrb	r6, [r2, #13]
		ldrb	r7, [r2, #17]
		ldrb	r8, [r2, #21]
		ldrb	r9, [r2, #25]
		ldrb	r10, [r2, #29]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

        mov		r2, r12
        ldr     r14, [r1, #12]                   @  block[5] 										       
@Y4  
 		ldrb	r3, [r2, #3]
		ldrb	r4, [r2, #7]
		ldrb	r5, [r2, #11]
		ldrb	r6, [r2, #15]
		ldrb	r7, [r2, #19]
		ldrb	r8, [r2, #23]
		ldrb	r9, [r2, #27]
		ldrb	r10, [r2, #31]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

 		ldrb	r3, [r2, #3]
		ldrb	r4, [r2, #7]
		ldrb	r5, [r2, #11]
		ldrb	r6, [r2, #15]
		ldrb	r7, [r2, #19]
		ldrb	r8, [r2, #23]
		ldrb	r9, [r2, #27]
		ldrb	r10, [r2, #31]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@2
 		ldrb	r3, [r2, #3]
		ldrb	r4, [r2, #7]
		ldrb	r5, [r2, #11]
		ldrb	r6, [r2, #15]
		ldrb	r7, [r2, #19]
		ldrb	r8, [r2, #23]
		ldrb	r9, [r2, #27]
		ldrb	r10, [r2, #31]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

 		ldrb	r3, [r2, #3]
		ldrb	r4, [r2, #7]
		ldrb	r5, [r2, #11]
		ldrb	r6, [r2, #15]
		ldrb	r7, [r2, #19]
		ldrb	r8, [r2, #23]
		ldrb	r9, [r2, #27]
		ldrb	r10, [r2, #31]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@3
 		ldrb	r3, [r2, #3]
		ldrb	r4, [r2, #7]
		ldrb	r5, [r2, #11]
		ldrb	r6, [r2, #15]
		ldrb	r7, [r2, #19]
		ldrb	r8, [r2, #23]
		ldrb	r9, [r2, #27]
		ldrb	r10, [r2, #31]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

 		ldrb	r3, [r2, #3]
		ldrb	r4, [r2, #7]
		ldrb	r5, [r2, #11]
		ldrb	r6, [r2, #15]
		ldrb	r7, [r2, #19]
		ldrb	r8, [r2, #23]
		ldrb	r9, [r2, #27]
		ldrb	r10, [r2, #31]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@4
 		ldrb	r3, [r2, #3]
		ldrb	r4, [r2, #7]
		ldrb	r5, [r2, #11]
		ldrb	r6, [r2, #15]
		ldrb	r7, [r2, #19]
		ldrb	r8, [r2, #23]
		ldrb	r9, [r2, #27]
		ldrb	r10, [r2, #31]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

 		ldrb	r3, [r2, #3]
		ldrb	r4, [r2, #7]
		ldrb	r5, [r2, #11]
		ldrb	r6, [r2, #15]
		ldrb	r7, [r2, #19]
		ldrb	r8, [r2, #23]
		ldrb	r9, [r2, #27]
		ldrb	r10, [r2, #31]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		
		ldmia	sp!, {r4-r11, pc}		@ restore and return

get422iYVYUblockdata_asm:
		stmdb	sp!, {r4-r11, lr}				@ save regs used


        ldr     r3, _cc0X00ff00ff_422iYUYV
        ldr     r11, _cc0X00800080_422iYUYV
		mov		r12, r2
		
        ldr     r14, [r1, #0]                     @  block[0] 										       
@Y1  
		ldrd	r6, [r2, #8]
		ldrd	r4, [r2], r0
		ldrd	r8, [r2, #8]
		and		r6, r3, r6	
		and		r7, r3, r7	
		and		r4, r3, r4	
		and		r5, r3, r5	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2], r0
		and		r8, r3, r8	
		and		r9, r3, r9	
		and		r4, r3, r4	
		and		r5, r3, r5	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r8, r8, r11
		qsub16	r9, r9, r11
		strd	r4, [r14], #8 
		strd	r8, [r14], #8 
@2
		ldrd	r6, [r2, #8]
		ldrd	r4, [r2], r0
		ldrd	r8, [r2, #8]
		and		r6, r3, r6	
		and		r7, r3, r7	
		and		r4, r3, r4	
		and		r5, r3, r5	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2], r0
		and		r8, r3, r8	
		and		r9, r3, r9	
		and		r4, r3, r4	
		and		r5, r3, r5	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r8, r8, r11
		qsub16	r9, r9, r11
		strd	r4, [r14], #8 
		strd	r8, [r14], #8
@3
		ldrd	r6, [r2, #8]
		ldrd	r4, [r2], r0
		ldrd	r8, [r2, #8]
		and		r6, r3, r6	
		and		r7, r3, r7	
		and		r4, r3, r4	
		and		r5, r3, r5	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2], r0
		and		r8, r3, r8	
		and		r9, r3, r9	
		and		r4, r3, r4	
		and		r5, r3, r5	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r8, r8, r11
		qsub16	r9, r9, r11
		strd	r4, [r14], #8 
		strd	r8, [r14], #8 
@4
		ldrd	r6, [r2, #8]
		ldrd	r4, [r2], r0
		ldrd	r8, [r2, #8]
		and		r6, r3, r6	
		and		r7, r3, r7	
		and		r4, r3, r4	
		and		r5, r3, r5	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2], r0
		and		r8, r3, r8	
		and		r9, r3, r9	
		and		r4, r3, r4	
		and		r5, r3, r5	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r8, r8, r11
		qsub16	r9, r9, r11
		strd	r4, [r14], #8 
		strd	r8, [r14], #8

        mov		r2, r12
        ldr     r14, [r1, #4]                     @  block[1] 										       
@Y2  
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4	
		and		r5, r3, r5
		and		r6, r3, r6	
		and		r7, r3, r7			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4	
		and		r5, r3, r5
		and		r6, r3, r6	
		and		r7, r3, r7			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8
@2
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4	
		and		r5, r3, r5
		and		r6, r3, r6	
		and		r7, r3, r7			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4	
		and		r5, r3, r5
		and		r6, r3, r6	
		and		r7, r3, r7			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8
@3
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4	
		and		r5, r3, r5
		and		r6, r3, r6	
		and		r7, r3, r7			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4	
		and		r5, r3, r5
		and		r6, r3, r6	
		and		r7, r3, r7			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8
@4
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4	
		and		r5, r3, r5
		and		r6, r3, r6	
		and		r7, r3, r7			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4	
		and		r5, r3, r5
		and		r6, r3, r6	
		and		r7, r3, r7			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8
										       										       
        mov		r2, r12
        ldr     r14, [r1, #12]                     @  block[1] 										       
@Y3  
		ldrb	r3, [r2, #1]
		ldrb	r4, [r2, #5]
		ldrb	r5, [r2, #9]
		ldrb	r6, [r2, #13]
		ldrb	r7, [r2, #17]
		ldrb	r8, [r2, #21]
		ldrb	r9, [r2, #25]
		ldrb	r10, [r2, #29]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

		ldrb	r3, [r2, #1]
		ldrb	r4, [r2, #5]
		ldrb	r5, [r2, #9]
		ldrb	r6, [r2, #13]
		ldrb	r7, [r2, #17]
		ldrb	r8, [r2, #21]
		ldrb	r9, [r2, #25]
		ldrb	r10, [r2, #29]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@2
		ldrb	r3, [r2, #1]
		ldrb	r4, [r2, #5]
		ldrb	r5, [r2, #9]
		ldrb	r6, [r2, #13]
		ldrb	r7, [r2, #17]
		ldrb	r8, [r2, #21]
		ldrb	r9, [r2, #25]
		ldrb	r10, [r2, #29]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

		ldrb	r3, [r2, #1]
		ldrb	r4, [r2, #5]
		ldrb	r5, [r2, #9]
		ldrb	r6, [r2, #13]
		ldrb	r7, [r2, #17]
		ldrb	r8, [r2, #21]
		ldrb	r9, [r2, #25]
		ldrb	r10, [r2, #29]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@3
		ldrb	r3, [r2, #1]
		ldrb	r4, [r2, #5]
		ldrb	r5, [r2, #9]
		ldrb	r6, [r2, #13]
		ldrb	r7, [r2, #17]
		ldrb	r8, [r2, #21]
		ldrb	r9, [r2, #25]
		ldrb	r10, [r2, #29]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

		ldrb	r3, [r2, #1]
		ldrb	r4, [r2, #5]
		ldrb	r5, [r2, #9]
		ldrb	r6, [r2, #13]
		ldrb	r7, [r2, #17]
		ldrb	r8, [r2, #21]
		ldrb	r9, [r2, #25]
		ldrb	r10, [r2, #29]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@4
		ldrb	r3, [r2, #1]
		ldrb	r4, [r2, #5]
		ldrb	r5, [r2, #9]
		ldrb	r6, [r2, #13]
		ldrb	r7, [r2, #17]
		ldrb	r8, [r2, #21]
		ldrb	r9, [r2, #25]
		ldrb	r10, [r2, #29]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

		ldrb	r3, [r2, #1]
		ldrb	r4, [r2, #5]
		ldrb	r5, [r2, #9]
		ldrb	r6, [r2, #13]
		ldrb	r7, [r2, #17]
		ldrb	r8, [r2, #21]
		ldrb	r9, [r2, #25]
		ldrb	r10, [r2, #29]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

        mov		r2, r12
        ldr     r14, [r1, #8]                   @  block[5] 										       
@Y4  
 		ldrb	r3, [r2, #3]
		ldrb	r4, [r2, #7]
		ldrb	r5, [r2, #11]
		ldrb	r6, [r2, #15]
		ldrb	r7, [r2, #19]
		ldrb	r8, [r2, #23]
		ldrb	r9, [r2, #27]
		ldrb	r10, [r2, #31]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

 		ldrb	r3, [r2, #3]
		ldrb	r4, [r2, #7]
		ldrb	r5, [r2, #11]
		ldrb	r6, [r2, #15]
		ldrb	r7, [r2, #19]
		ldrb	r8, [r2, #23]
		ldrb	r9, [r2, #27]
		ldrb	r10, [r2, #31]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@2
 		ldrb	r3, [r2, #3]
		ldrb	r4, [r2, #7]
		ldrb	r5, [r2, #11]
		ldrb	r6, [r2, #15]
		ldrb	r7, [r2, #19]
		ldrb	r8, [r2, #23]
		ldrb	r9, [r2, #27]
		ldrb	r10, [r2, #31]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

 		ldrb	r3, [r2, #3]
		ldrb	r4, [r2, #7]
		ldrb	r5, [r2, #11]
		ldrb	r6, [r2, #15]
		ldrb	r7, [r2, #19]
		ldrb	r8, [r2, #23]
		ldrb	r9, [r2, #27]
		ldrb	r10, [r2, #31]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@3
 		ldrb	r3, [r2, #3]
		ldrb	r4, [r2, #7]
		ldrb	r5, [r2, #11]
		ldrb	r6, [r2, #15]
		ldrb	r7, [r2, #19]
		ldrb	r8, [r2, #23]
		ldrb	r9, [r2, #27]
		ldrb	r10, [r2, #31]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

 		ldrb	r3, [r2, #3]
		ldrb	r4, [r2, #7]
		ldrb	r5, [r2, #11]
		ldrb	r6, [r2, #15]
		ldrb	r7, [r2, #19]
		ldrb	r8, [r2, #23]
		ldrb	r9, [r2, #27]
		ldrb	r10, [r2, #31]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@4
 		ldrb	r3, [r2, #3]
		ldrb	r4, [r2, #7]
		ldrb	r5, [r2, #11]
		ldrb	r6, [r2, #15]
		ldrb	r7, [r2, #19]
		ldrb	r8, [r2, #23]
		ldrb	r9, [r2, #27]
		ldrb	r10, [r2, #31]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

 		ldrb	r3, [r2, #3]
		ldrb	r4, [r2, #7]
		ldrb	r5, [r2, #11]
		ldrb	r6, [r2, #15]
		ldrb	r7, [r2, #19]
		ldrb	r8, [r2, #23]
		ldrb	r9, [r2, #27]
		ldrb	r10, [r2, #31]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		
		ldmia	sp!, {r4-r11, pc}		@ restore and return

_cc0X00ff00ff_422iUYVY:
        	.word   0x00ff00ff
_cc0X00800080_422iUYVY:
        	.word   0x00800080

get422iUYVYblockdata_asm:
		stmdb	sp!, {r4-r11, lr}				@ save regs used

        ldr     r3, _cc0X00ff00ff_422iUYVY
        ldr     r11, _cc0X00800080_422iUYVY
		mov		r12, r2
		
        ldr     r14, [r1, #0]                     @  block[0] 										       
@Y1  
		ldrd	r6, [r2, #8]
		ldrd	r4, [r2], r0
		ldrd	r8, [r2, #8]
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2], r0
		and		r8, r3, r8, lsr #8	
		and		r9, r3, r9, lsr #8	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r8, r8, r11
		qsub16	r9, r9, r11
		strd	r4, [r14], #8 
		strd	r8, [r14], #8 
@2
		ldrd	r6, [r2, #8]
		ldrd	r4, [r2], r0
		ldrd	r8, [r2, #8]
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2], r0
		and		r8, r3, r8, lsr #8	
		and		r9, r3, r9, lsr #8	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r8, r8, r11
		qsub16	r9, r9, r11
		strd	r4, [r14], #8 
		strd	r8, [r14], #8
@3
		ldrd	r6, [r2, #8]
		ldrd	r4, [r2], r0
		ldrd	r8, [r2, #8]
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2], r0
		and		r8, r3, r8, lsr #8	
		and		r9, r3, r9, lsr #8	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r8, r8, r11
		qsub16	r9, r9, r11
		strd	r4, [r14], #8 
		strd	r8, [r14], #8 
@4
		ldrd	r6, [r2, #8]
		ldrd	r4, [r2], r0
		ldrd	r8, [r2, #8]
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2], r0
		and		r8, r3, r8, lsr #8	
		and		r9, r3, r9, lsr #8	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r8, r8, r11
		qsub16	r9, r9, r11
		strd	r4, [r14], #8 
		strd	r8, [r14], #8

        mov		r2, r12
        ldr     r14, [r1, #4]                     @  block[1] 										       
@Y2  
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8
@2
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8		
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8
@3
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8		
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8
@4
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8		
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8		
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8
										       										       										       
        mov		r2, r12
        ldr     r14, [r1, #8]                     @  block[1] 										       
@Y3  
		ldrb	r4, [r2, #4]
		ldrb	r5, [r2, #8]
		ldrb	r6, [r2, #12]
		ldrb	r7, [r2, #16]
		ldrb	r8, [r2, #20]
		ldrb	r9, [r2, #24]
		ldrb	r10, [r2, #28]
		ldrb	r3, [r2], r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

		ldrb	r4, [r2, #4]
		ldrb	r5, [r2, #8]
		ldrb	r6, [r2, #12]
		ldrb	r7, [r2, #16]
		ldrb	r8, [r2, #20]
		ldrb	r9, [r2, #24]
		ldrb	r10, [r2, #28]
		ldrb	r3, [r2], r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@2
		ldrb	r4, [r2, #4]
		ldrb	r5, [r2, #8]
		ldrb	r6, [r2, #12]
		ldrb	r7, [r2, #16]
		ldrb	r8, [r2, #20]
		ldrb	r9, [r2, #24]
		ldrb	r10, [r2, #28]
		ldrb	r3, [r2], r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

		ldrb	r4, [r2, #4]
		ldrb	r5, [r2, #8]
		ldrb	r6, [r2, #12]
		ldrb	r7, [r2, #16]
		ldrb	r8, [r2, #20]
		ldrb	r9, [r2, #24]
		ldrb	r10, [r2, #28]
		ldrb	r3, [r2], r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@3
		ldrb	r4, [r2, #4]
		ldrb	r5, [r2, #8]
		ldrb	r6, [r2, #12]
		ldrb	r7, [r2, #16]
		ldrb	r8, [r2, #20]
		ldrb	r9, [r2, #24]
		ldrb	r10, [r2, #28]
		ldrb	r3, [r2], r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

		ldrb	r4, [r2, #4]
		ldrb	r5, [r2, #8]
		ldrb	r6, [r2, #12]
		ldrb	r7, [r2, #16]
		ldrb	r8, [r2, #20]
		ldrb	r9, [r2, #24]
		ldrb	r10, [r2, #28]
		ldrb	r3, [r2], r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@4
		ldrb	r4, [r2, #4]
		ldrb	r5, [r2, #8]
		ldrb	r6, [r2, #12]
		ldrb	r7, [r2, #16]
		ldrb	r8, [r2, #20]
		ldrb	r9, [r2, #24]
		ldrb	r10, [r2, #28]
		ldrb	r3, [r2], r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

		ldrb	r4, [r2, #4]
		ldrb	r5, [r2, #8]
		ldrb	r6, [r2, #12]
		ldrb	r7, [r2, #16]
		ldrb	r8, [r2, #20]
		ldrb	r9, [r2, #24]
		ldrb	r10, [r2, #28]
		ldrb	r3, [r2], r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

        mov		r2, r12
        ldr     r14, [r1, #12]                   @  block[5] 										       
@Y4  
 		ldrb	r3, [r2, #2]
		ldrb	r4, [r2, #6]
		ldrb	r5, [r2, #10]
		ldrb	r6, [r2, #14]
		ldrb	r7, [r2, #18]
		ldrb	r8, [r2, #22]
		ldrb	r9, [r2, #26]
		ldrb	r10, [r2, #30]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

 		ldrb	r3, [r2, #2]
		ldrb	r4, [r2, #6]
		ldrb	r5, [r2, #10]
		ldrb	r6, [r2, #14]
		ldrb	r7, [r2, #18]
		ldrb	r8, [r2, #22]
		ldrb	r9, [r2, #26]
		ldrb	r10, [r2, #30]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@2
 		ldrb	r3, [r2, #2]
		ldrb	r4, [r2, #6]
		ldrb	r5, [r2, #10]
		ldrb	r6, [r2, #14]
		ldrb	r7, [r2, #18]
		ldrb	r8, [r2, #22]
		ldrb	r9, [r2, #26]
		ldrb	r10, [r2, #30]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

 		ldrb	r3, [r2, #2]
		ldrb	r4, [r2, #6]
		ldrb	r5, [r2, #10]
		ldrb	r6, [r2, #14]
		ldrb	r7, [r2, #18]
		ldrb	r8, [r2, #22]
		ldrb	r9, [r2, #26]
		ldrb	r10, [r2, #30]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@3
 		ldrb	r3, [r2, #2]
		ldrb	r4, [r2, #6]
		ldrb	r5, [r2, #10]
		ldrb	r6, [r2, #14]
		ldrb	r7, [r2, #18]
		ldrb	r8, [r2, #22]
		ldrb	r9, [r2, #26]
		ldrb	r10, [r2, #30]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

 		ldrb	r3, [r2, #2]
		ldrb	r4, [r2, #6]
		ldrb	r5, [r2, #10]
		ldrb	r6, [r2, #14]
		ldrb	r7, [r2, #18]
		ldrb	r8, [r2, #22]
		ldrb	r9, [r2, #26]
		ldrb	r10, [r2, #30]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@4
 		ldrb	r3, [r2, #2]
		ldrb	r4, [r2, #6]
		ldrb	r5, [r2, #10]
		ldrb	r6, [r2, #14]
		ldrb	r7, [r2, #18]
		ldrb	r8, [r2, #22]
		ldrb	r9, [r2, #26]
		ldrb	r10, [r2, #30]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

 		ldrb	r3, [r2, #2]
		ldrb	r4, [r2, #6]
		ldrb	r5, [r2, #10]
		ldrb	r6, [r2, #14]
		ldrb	r7, [r2, #18]
		ldrb	r8, [r2, #22]
		ldrb	r9, [r2, #26]
		ldrb	r10, [r2, #30]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		
		ldmia	sp!, {r4-r11, pc}		@ restore and return

get422iVYUYblockdata_asm:
		stmdb	sp!, {r4-r11, lr}				@ save regs used

 
        ldr     r3, _cc0X00ff00ff_422iUYVY
        ldr     r11, _cc0X00800080_422iUYVY
		mov		r12, r2
		
        ldr     r14, [r1, #0]                     @  block[0] 										       
@Y1  
		ldrd	r6, [r2, #8]
		ldrd	r4, [r2], r0
		ldrd	r8, [r2, #8]
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2], r0
		and		r8, r3, r8, lsr #8	
		and		r9, r3, r9, lsr #8	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r8, r8, r11
		qsub16	r9, r9, r11
		strd	r4, [r14], #8 
		strd	r8, [r14], #8 
@2
		ldrd	r6, [r2, #8]
		ldrd	r4, [r2], r0
		ldrd	r8, [r2, #8]
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2], r0
		and		r8, r3, r8, lsr #8	
		and		r9, r3, r9, lsr #8	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r8, r8, r11
		qsub16	r9, r9, r11
		strd	r4, [r14], #8 
		strd	r8, [r14], #8
@3
		ldrd	r6, [r2, #8]
		ldrd	r4, [r2], r0
		ldrd	r8, [r2, #8]
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2], r0
		and		r8, r3, r8, lsr #8	
		and		r9, r3, r9, lsr #8	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r8, r8, r11
		qsub16	r9, r9, r11
		strd	r4, [r14], #8 
		strd	r8, [r14], #8 
@4
		ldrd	r6, [r2, #8]
		ldrd	r4, [r2], r0
		ldrd	r8, [r2, #8]
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2], r0
		and		r8, r3, r8, lsr #8	
		and		r9, r3, r9, lsr #8	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8	
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r8, r8, r11
		qsub16	r9, r9, r11
		strd	r4, [r14], #8 
		strd	r8, [r14], #8

        mov		r2, r12
        ldr     r14, [r1, #4]                     @  block[1] 										       
@Y2  
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8
@2
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8		
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8
@3
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8		
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8			
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8
@4
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8		
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		ldrd	r4, [r2, #16]
		ldrd	r6, [r2, #24]
		add		r2, r2, r0	
		and		r4, r3, r4, lsr #8	
		and		r5, r3, r5, lsr #8
		and		r6, r3, r6, lsr #8	
		and		r7, r3, r7, lsr #8		
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8
										       										       
										       
        mov		r2, r12
        ldr     r14, [r1, #12]                     @  block[1] 										       
@Y3  
		ldrb	r4, [r2, #4]
		ldrb	r5, [r2, #8]
		ldrb	r6, [r2, #12]
		ldrb	r7, [r2, #16]
		ldrb	r8, [r2, #20]
		ldrb	r9, [r2, #24]
		ldrb	r10, [r2, #28]
		ldrb	r3, [r2], r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

		ldrb	r4, [r2, #4]
		ldrb	r5, [r2, #8]
		ldrb	r6, [r2, #12]
		ldrb	r7, [r2, #16]
		ldrb	r8, [r2, #20]
		ldrb	r9, [r2, #24]
		ldrb	r10, [r2, #28]
		ldrb	r3, [r2], r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@2
		ldrb	r4, [r2, #4]
		ldrb	r5, [r2, #8]
		ldrb	r6, [r2, #12]
		ldrb	r7, [r2, #16]
		ldrb	r8, [r2, #20]
		ldrb	r9, [r2, #24]
		ldrb	r10, [r2, #28]
		ldrb	r3, [r2], r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

		ldrb	r4, [r2, #4]
		ldrb	r5, [r2, #8]
		ldrb	r6, [r2, #12]
		ldrb	r7, [r2, #16]
		ldrb	r8, [r2, #20]
		ldrb	r9, [r2, #24]
		ldrb	r10, [r2, #28]
		ldrb	r3, [r2], r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@3
		ldrb	r4, [r2, #4]
		ldrb	r5, [r2, #8]
		ldrb	r6, [r2, #12]
		ldrb	r7, [r2, #16]
		ldrb	r8, [r2, #20]
		ldrb	r9, [r2, #24]
		ldrb	r10, [r2, #28]
		ldrb	r3, [r2], r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

		ldrb	r4, [r2, #4]
		ldrb	r5, [r2, #8]
		ldrb	r6, [r2, #12]
		ldrb	r7, [r2, #16]
		ldrb	r8, [r2, #20]
		ldrb	r9, [r2, #24]
		ldrb	r10, [r2, #28]
		ldrb	r3, [r2], r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@4
		ldrb	r4, [r2, #4]
		ldrb	r5, [r2, #8]
		ldrb	r6, [r2, #12]
		ldrb	r7, [r2, #16]
		ldrb	r8, [r2, #20]
		ldrb	r9, [r2, #24]
		ldrb	r10, [r2, #28]
		ldrb	r3, [r2], r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

		ldrb	r4, [r2, #4]
		ldrb	r5, [r2, #8]
		ldrb	r6, [r2, #12]
		ldrb	r7, [r2, #16]
		ldrb	r8, [r2, #20]
		ldrb	r9, [r2, #24]
		ldrb	r10, [r2, #28]
		ldrb	r3, [r2], r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

        mov		r2, r12
        ldr     r14, [r1, #8]                   @  block[5] 										       
@Y4  
 		ldrb	r3, [r2, #2]
		ldrb	r4, [r2, #6]
		ldrb	r5, [r2, #10]
		ldrb	r6, [r2, #14]
		ldrb	r7, [r2, #18]
		ldrb	r8, [r2, #22]
		ldrb	r9, [r2, #26]
		ldrb	r10, [r2, #30]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

 		ldrb	r3, [r2, #2]
		ldrb	r4, [r2, #6]
		ldrb	r5, [r2, #10]
		ldrb	r6, [r2, #14]
		ldrb	r7, [r2, #18]
		ldrb	r8, [r2, #22]
		ldrb	r9, [r2, #26]
		ldrb	r10, [r2, #30]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@2
 		ldrb	r3, [r2, #2]
		ldrb	r4, [r2, #6]
		ldrb	r5, [r2, #10]
		ldrb	r6, [r2, #14]
		ldrb	r7, [r2, #18]
		ldrb	r8, [r2, #22]
		ldrb	r9, [r2, #26]
		ldrb	r10, [r2, #30]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

 		ldrb	r3, [r2, #2]
		ldrb	r4, [r2, #6]
		ldrb	r5, [r2, #10]
		ldrb	r6, [r2, #14]
		ldrb	r7, [r2, #18]
		ldrb	r8, [r2, #22]
		ldrb	r9, [r2, #26]
		ldrb	r10, [r2, #30]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@3
 		ldrb	r3, [r2, #2]
		ldrb	r4, [r2, #6]
		ldrb	r5, [r2, #10]
		ldrb	r6, [r2, #14]
		ldrb	r7, [r2, #18]
		ldrb	r8, [r2, #22]
		ldrb	r9, [r2, #26]
		ldrb	r10, [r2, #30]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

 		ldrb	r3, [r2, #2]
		ldrb	r4, [r2, #6]
		ldrb	r5, [r2, #10]
		ldrb	r6, [r2, #14]
		ldrb	r7, [r2, #18]
		ldrb	r8, [r2, #22]
		ldrb	r9, [r2, #26]
		ldrb	r10, [r2, #30]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
@4
 		ldrb	r3, [r2, #2]
		ldrb	r4, [r2, #6]
		ldrb	r5, [r2, #10]
		ldrb	r6, [r2, #14]
		ldrb	r7, [r2, #18]
		ldrb	r8, [r2, #22]
		ldrb	r9, [r2, #26]
		ldrb	r10, [r2, #30]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 

 		ldrb	r3, [r2, #2]
		ldrb	r4, [r2, #6]
		ldrb	r5, [r2, #10]
		ldrb	r6, [r2, #14]
		ldrb	r7, [r2, #18]
		ldrb	r8, [r2, #22]
		ldrb	r9, [r2, #26]
		ldrb	r10, [r2, #30]
		add		r2, r2, r0
		orr		r5, r5, r6, lsl #16
		orr		r4, r3, r4, lsl #16
		orr		r6, r7, r8, lsl #16
		orr		r7, r9, r10, lsl #16
		qsub16	r4, r4, r11
		qsub16	r5, r5, r11
		qsub16	r6, r6, r11
		qsub16	r7, r7, r11
		strd	r4, [r14], #8 
		strd	r6, [r14], #8 
		
		ldmia	sp!, {r4-r11, pc}		@ restore and return

@END