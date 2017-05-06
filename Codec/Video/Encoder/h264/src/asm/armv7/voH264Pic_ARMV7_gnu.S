@;*****************************************************************************
@;*																			*
@;*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@;*																			*
@;*****************************************************************************  
#include "../../voH264EncID.h"
	@AREA	|.text|, CODE
	.section .text
 
	.global CopyPlaneMod16_ARMV7
	.global UV420pack_to_UV420_mod16_ARMV7
	
CopyPlaneMod16_ARMV7:
@;r0	dst
@;r1 i_dst
@;r2 src
@;r3 i_src
@;r4 w
@;r5 h

	.equ	CP16_OffsetRegSaving, 36
	.equ	CP16_Offset_w, CP16_OffsetRegSaving + 0
	.equ	CP16_Offset_h, CP16_OffsetRegSaving + 4
	push     	{r4 - r11, r14}
	ldr			r4, [sp, #CP16_Offset_w]
	ldr			r5, [sp, #CP16_Offset_h]
	add			r5, r5, #16
	sub			r9, r0, r1, lsl #4		@;dst for h
	sub			r10, r2, r3, lsl #4		@;src for h
CP16_LoopH:
	subs		r5, r5, #16
	ble			CP16_Done
	add			r9, r9, r1, lsl #4		@;dst for h
	add			r10, r10, r3, lsl #4		@;src for h
	mov			r11, r4
	mov			r7, r9
	mov			r8, r10
CP16_LoopW:
	vld1.64		{q0},  [r8], r3
	vld1.64		{q1},  [r8], r3
	vld1.64		{q2},  [r8], r3
	vld1.64		{q3},  [r8], r3
	vld1.64		{q4},  [r8], r3
	vld1.64		{q5},  [r8], r3
	vld1.64		{q6},  [r8], r3
	vld1.64		{q7},  [r8], r3
	vld1.64		{q8},  [r8], r3
	vld1.64		{q9},  [r8], r3
	vld1.64		{q10},  [r8], r3
	vld1.64		{q11},  [r8], r3
	vld1.64		{q12},  [r8], r3
	vld1.64		{q13},  [r8], r3
	vld1.64		{q14},  [r8], r3
	vld1.64		{q15},  [r8], r3
	vst1.64		{q0},  [r7], r1
	vst1.64		{q1},  [r7], r1
	vst1.64		{q2},  [r7], r1
	vst1.64		{q3},  [r7], r1
	vst1.64		{q4},  [r7], r1
	vst1.64		{q5},  [r7], r1
	vst1.64		{q6},  [r7], r1
	vst1.64		{q7},  [r7], r1
	vst1.64		{q8},  [r7], r1
	vst1.64		{q9},  [r7], r1
	vst1.64		{q10},  [r7], r1
	vst1.64		{q11},  [r7], r1
	vst1.64		{q12},  [r7], r1
	vst1.64		{q13},  [r7], r1
	vst1.64		{q14},  [r7], r1
	vst1.64		{q15},  [r7], r1
	
	subs		r11, #16
	ble			CP16_LoopH
	sub			r8, r8, r3, lsl #4
	add			r8, r8, #16
	sub			r7, r7, r1, lsl #4
	add			r7, r7, #16
	b			CP16_LoopW
	
CP16_Done:
	pop      	{r4 - r11, pc}	
	
	
	
UV420pack_to_UV420_mod16_ARMV7:	
@;r0 udst
@;r1 vdst
@;r2 i_dst
@;r3 uv_src	
@;r6	i_src
@;r4 w
@;r5 h
	.equ	UV16_OffsetRegSaving, 36
	.equ	UV16_Offset_isrc, UV16_OffsetRegSaving + 0
	.equ	UV16_Offset_w, UV16_OffsetRegSaving + 4
	.equ	UV16_Offset_h, UV16_OffsetRegSaving + 8
	push     	{r4 - r11, r14}
	ldr			r4, [sp, #UV16_Offset_w]
	ldr			r5, [sp, #UV16_Offset_h]
	ldr			r6,	[sp, #UV16_Offset_isrc]
	lsr			r5, r5, #1
	add			r5, r5, #8				@;h+8
	sub			r7, r0, r2, lsl #3		@;udst-8stride for h
	sub			r8, r1, r2, lsl #3		@;vdst-8stride for h
	sub			r9, r3, r6, lsl #3		@;src-8stride for h
UV16_LoopH:
	subs		r5, r5, #8				@;h-8
	ble			UV16_Done
	add			r7, r7, r2, lsl #3		@;udst+8stride for h
	add			r8, r8, r2, lsl #3		@;vdst+8stride for h
	add			r9, r9, r6, lsl #3		@;src+8stride for h
	mov			r14, r4
	mov			r10, r7
	mov			r11, r8	
	mov			r12, r9
UV16_LoopW:	
	vld1.64		{q0},  [r12], r6
	vld1.64		{q1},  [r12], r6
	vld1.64		{q2},  [r12], r6
	vld1.64		{q3},  [r12], r6
	vld1.64		{q4},  [r12], r6
	vld1.64		{q5},  [r12], r6
	vld1.64		{q6},  [r12], r6
	vld1.64		{q7},  [r12], r6
	vuzp.8		q0, q1					@;q0 v01 q1 u01
	vuzp.8		q2, q3					@;q2 v23 q3 u23
	vuzp.8		q4, q5					@;q4 v45 q5 u45
	vuzp.8		q6, q7					@;q6 v67 q7 u67
	vst1.64		{d0},  [r10], r2
	vst1.64		{d1},  [r10], r2
	vst1.64		{d4},  [r10], r2
	vst1.64		{d5},  [r10], r2
	vst1.64		{d8},  [r10], r2
	vst1.64		{d9},  [r10], r2
	vst1.64		{d12},  [r10], r2
	vst1.64		{d13},  [r10], r2
	vst1.64		{d2},  [r11], r2
	vst1.64		{d3},  [r11], r2
	vst1.64		{d6},  [r11], r2
	vst1.64		{d7},  [r11], r2
	vst1.64		{d10},  [r11], r2
	vst1.64		{d11},  [r11], r2
	vst1.64		{d14},  [r11], r2
	vst1.64		{d15},  [r11], r2	
	
	subs		r14, #16
	ble			UV16_LoopH
	sub			r10, r10, r2, lsl #3
	add			r10, r10, #8
	sub			r11, r11, r2, lsl #3
	add			r11, r11, #8
	sub			r12, r12, r6, lsl #3
	add			r12, r12, #16
	b			UV16_LoopW
	
UV16_Done:
	pop      	{r4 - r11, pc}	
	
	.end
		
