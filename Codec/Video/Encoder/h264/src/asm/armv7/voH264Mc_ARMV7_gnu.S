@;*****************************************************************************
@;*																			*
@;*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@;*																			*
@;*****************************************************************************
#include "../../voH264EncID.h"
    @AREA	|.text|, CODE
    
  .section .text

	.global	 MCCopy16x16_ARMV7
	.global  MCCopy8x8_ARMV7
	.global  MCChroma8x8_ARMV7
	
	.align 8
	 
	.macro MCChroma8x8_ROW q0, d0, d1, d2, d3
		vld1.64		{\q0},  [r2], r3
		vext.8      \d1, \d0, \d1, #1
		vmull.u8   q7, \d2, d24
		vmull.u8   q8, \d3, d25
		vmull.u8   q9, \d0, d26
		vmull.u8   q10, \d1, d27
		vadd.u16	q7, q8
		vadd.u16	q7, q9
		vadd.u16	q7, q10
		vqrshrun.s16 	d16,  q7,  #6	@;0-7 solution
		vst1.64     {d16}, [r0],  r1
	.endm
	
MCCopy16x16_ARMV7:
	
	vld1.64		{q0},  [r0], r1
	vld1.64		{q1},  [r0], r1
	vst1.64     {q0}, [r2],  r3
	vst1.64     {q1}, [r2],  r3
	vld1.64		{q0},  [r0], r1
	vld1.64		{q1},  [r0], r1
	vst1.64     {q0}, [r2],  r3
	vst1.64     {q1}, [r2],  r3
	vld1.64		{q0},  [r0], r1
	vld1.64		{q1},  [r0], r1
	vst1.64     {q0}, [r2],  r3
	vst1.64     {q1}, [r2],  r3
	vld1.64		{q0},  [r0], r1
	vld1.64		{q1},  [r0], r1
	vst1.64     {q0}, [r2],  r3
	vst1.64     {q1}, [r2],  r3
	vld1.64		{q0},  [r0], r1
	vld1.64		{q1},  [r0], r1
	vst1.64     {q0}, [r2],  r3
	vst1.64     {q1}, [r2],  r3
	vld1.64		{q0},  [r0], r1
	vld1.64		{q1},  [r0], r1
	vst1.64     {q0}, [r2],  r3
	vst1.64     {q1}, [r2],  r3
	vld1.64		{q0},  [r0], r1
	vld1.64		{q1},  [r0], r1
	vst1.64     {q0}, [r2],  r3
	vst1.64     {q1}, [r2],  r3
	vld1.64		{q0},  [r0], r1
	vld1.64		{q1},  [r0], r1
	vst1.64     {q0}, [r2],  r3
	vst1.64     {q1}, [r2],  r3
	bx          lr
	
MCCopy8x8_ARMV7:
	
	vld1.64		{d0},  [r0], r1
	vld1.64		{d1},  [r0], r1
	vst1.64     {d0}, [r2],  r3
	vst1.64     {d1}, [r2],  r3
	vld1.64		{d0},  [r0], r1
	vld1.64		{d1},  [r0], r1
	vst1.64     {d0}, [r2],  r3
	vst1.64     {d1}, [r2],  r3
	vld1.64		{d0},  [r0], r1
	vld1.64		{d1},  [r0], r1
	vst1.64     {d0}, [r2],  r3
	vst1.64     {d1}, [r2],  r3
	vld1.64		{d0},  [r0], r1
	vld1.64		{d1},  [r0], r1
	vst1.64     {d0}, [r2],  r3
	vst1.64     {d1}, [r2],  r3
	bx          lr
	
MCChroma8x8_ARMV7:
 .equ	 FENC_STRIDE, 16
 .equ		MC_OffsetRegSaving, 32
 .equ		MC_Offset_dmvx, MC_OffsetRegSaving + 0
 .equ		MC_Offset_dmvy, MC_OffsetRegSaving + 4

	push     	{r4 - r10, r14}
	ldr  		r4, [sp, #MC_Offset_dmvx]
	ldr  		r5, [sp, #MC_Offset_dmvy]
	mov			r6, #8
	mov			r7, #8
	sub         r6, r6, r4
	sub         r7, r7, r5
	mul			r8, r6, r7
	mul			r9, r4, r7
	mul         r10, r6, r5
	mul 		r12, r4, r5
	vdup.8     d24,  r8	
	vdup.8     d25,  r9
	vdup.8     d26,  r10
	vdup.8     d27,  r12
	vld1.64		{q0},  [r2], r3
	vld1.64		{q1},  [r2], r3
	vext.8      d1, d0, d1, #1
	vext.8      d3, d2, d3, #1
	vmull.u8   q7, d0, d24
	vmull.u8   q8, d1, d25
	vmull.u8   q9, d2, d26
	vmull.u8   q10, d3, d27
	vadd.u16	q7, q8
	vadd.u16	q7, q9
	vadd.u16	q7, q10
	vqrshrn.u16 	d16,  q7,  #6	@;0-7 solution
	vst1.64     {d16}, [r0],  r1
@;row 2
	MCChroma8x8_ROW q0, d0, d1, d2, d3
@;row 3
	MCChroma8x8_ROW q1, d2, d3, d0, d1
@;row 4
	MCChroma8x8_ROW q0, d0, d1, d2, d3
@;row 5
	MCChroma8x8_ROW q1, d2, d3, d0, d1
@;row 6
	MCChroma8x8_ROW q0, d0, d1, d2, d3
@;row 7
	MCChroma8x8_ROW q1, d2, d3, d0, d1
@;row 8
	MCChroma8x8_ROW q0, d0, d1, d2, d3
	
	pop      	{r4 - r10, pc}
	
	
	.end
