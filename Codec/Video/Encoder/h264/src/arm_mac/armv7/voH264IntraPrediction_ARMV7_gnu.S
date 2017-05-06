@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************  
#include "../../voH264EncID.h"
	@AREA	|.text|, CODE
	
	.text
 
	.globl _PredIntraChroma8x8H_ARMV7
	.globl _PredIntraChroma8x8V_ARMV7
	.globl _PredIntraLuma16x16H_ARMV7
	.globl _PredIntraLuma16x16V_ARMV7
	.globl _PredIntraLuma16x16Dc_ARMV7
	.globl _PredIntraLuma16x16P_ARMV7
  .align 4
_PredIntraChroma8x8H_ARMV7:
    sub         r12, r0, #1
    vld1.8      {d0[]}, [r12], r1
    vld1.8      {d1[]}, [r12], r1
    vld1.8      {d2[]}, [r12], r1
    vld1.8      {d3[]}, [r12], r1
    vld1.8      {d4[]}, [r12], r1
    vld1.8      {d5[]}, [r12], r1
    vld1.8      {d6[]}, [r12], r1
    vld1.8      {d7[]}, [r12]
    vst1.64     {d0}, [r2 ], r3
    vst1.64     {d1}, [r2 ], r3
    vst1.64     {d2}, [r2 ], r3
    vst1.64     {d3}, [r2 ], r3    
    vst1.64     {d4}, [r2 ], r3
    vst1.64     {d5}, [r2 ], r3   
    vst1.64     {d6}, [r2 ], r3
    vst1.64     {d7}, [r2 ]
    bx			lr

_PredIntraChroma8x8V_ARMV7:
    sub         r0, r0, r1
    vld1.64     {d0}, [r0 ]
    vst1.64     {d0}, [r2 ], r3
    vst1.64     {d0}, [r2 ], r3
    vst1.64     {d0}, [r2 ], r3
    vst1.64     {d0}, [r2 ], r3
    vst1.64     {d0}, [r2 ], r3
    vst1.64     {d0}, [r2 ], r3
    vst1.64     {d0}, [r2 ], r3
    vst1.64     {d0}, [r2 ]
    bx          lr

_PredIntraLuma16x16Dc_ARMV7 :
    push        {r4-r8,lr}
    
    sub         r5, r0, r1
    sub         r0, r0, #1
    vld1.64     {d0-d1}, [r5 ]
    ldrb        r8, [r0], r1
    vaddl.u8    q0, d0, d1
    ldrb        r6, [r0], r1
    vadd.u16    d0, d0, d1
    vpadd.u16   d0, d0, d0
    vpadd.u16   d0, d0, d0

    ldrb        r7, [r0], r1    
    add         r8, r8, r6
    ldrb        r5, [r0], r1
    add         r8, r8, r7
    ldrb        r6, [r0], r1
    add         r8, r8, r5
    
    ldrb        r7, [r0], r1   
    add         r8, r8, r6
    ldrb        r5, [r0], r1
    add         r8, r8, r7
    ldrb        r6, [r0], r1
    add         r8, r8, r5
    
    ldrb        r7, [r0], r1  
    add         r8, r8, r6
    ldrb        r5, [r0], r1
    add         r8, r8, r7
    ldrb        r6, [r0], r1
    add         r8, r8, r5
    
    ldrb        r7, [r0], r1   
    add         r8, r8, r6
    ldrb        r5, [r0], r1
    add         r8, r8, r7
    ldrb        r6, [r0], r1
    add         r8, r8, r5

    ldrb        r7, [r0], r1
    add         r8, r8, r6
    ldrb        r5, [r0], r1
    add         r8, r8, r7

    add         r8, r8, r5
    vdup.16     d1, r8
    vadd.u16    d0, d0, d1
    vrshr.u16   d0, d0, #5
    vdup.8      q0, d0[0]

    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ]

    pop         {r4-r8,pc}
    
_PredIntraLuma16x16H_ARMV7:
    sub         r0, r0, #1
    vld1.8      {d0[],d1[]}, [r0], r1
    vld1.8      {d2[],d3[]}, [r0], r1
    vld1.8      {d4[],d5[]}, [r0], r1
    vld1.8      {d6[],d7[]}, [r0], r1
    vld1.8      {d8[],d9[]}, [r0], r1
    vld1.8      {d10[],d11[]}, [r0], r1
    vld1.8      {d12[],d13[]}, [r0], r1
    vld1.8      {d14[],d15[]}, [r0], r1
    vld1.8      {d16[],d17[]}, [r0], r1
    vld1.8      {d18[],d19[]}, [r0], r1
    vld1.8      {d20[],d21[]}, [r0], r1
    vld1.8      {d22[],d23[]}, [r0], r1
    vld1.8      {d24[],d25[]}, [r0], r1
    vld1.8      {d26[],d27[]}, [r0], r1
    vld1.8      {d28[],d29[]}, [r0], r1
    vld1.8      {d30[],d31[]}, [r0]
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d2-d3}, [r2 ], r3  
    vst1.64     {d4-d5}, [r2 ], r3
    vst1.64     {d6-d7}, [r2 ], r3
    vst1.64     {d8-d9}, [r2 ], r3
    vst1.64     {d10-d11}, [r2 ], r3
    vst1.64     {d12-d13}, [r2 ], r3
    vst1.64     {d14-d15}, [r2 ], r3
    vst1.64     {d16-d17}, [r2 ], r3
    vst1.64     {d18-d19}, [r2 ], r3  
    vst1.64     {d20-d21}, [r2 ], r3
    vst1.64     {d22-d23}, [r2 ], r3
    vst1.64     {d24-d25}, [r2 ], r3
    vst1.64     {d26-d27}, [r2 ], r3
    vst1.64     {d28-d29}, [r2 ], r3
    vst1.64     {d30-d31}, [r2 ]
    bx          lr

_PredIntraLuma16x16V_ARMV7 :
    sub         r0, r0, r1
    vld1.64     {d0-d1}, [r0 ]
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ], r3
    vst1.64     {d0-d1}, [r2 ]
    bx          lr
    
    .align 8
voMultiplierTable16x16:
    .short   7,  6,  5,  4,  3,  2,  1,  8 
    .short   0,  1,  2,  3,  4,  5,  6,  7
    .short   8,  9, 10, 11, 12, 13, 14, 15

_PredIntraLuma16x16P_ARMV7:

	push     	{r4 - r10, r14}
	sub			r4, r0, r1
	sub			r5, r4, #1
	mov			r12, #16
	
	adr			r6, voMultiplierTable16x16
	vld1.64		{q0}, [r4]
	vld1.8		d4[0], [r5], r1
	vld1.8		{d2[0]}, [r5], r1
	vld1.8		{d2[1]}, [r5], r1
	vld1.8		{d2[2]}, [r5], r1
	vld1.8		{d2[3]}, [r5], r1
	vld1.8		{d2[4]}, [r5], r1
	vld1.8		{d2[5]}, [r5], r1
	vld1.8		{d2[6]}, [r5], r1
	vld1.8		{d2[7]}, [r5], r1
	vld1.8		{d3[0]}, [r5], r1
	vld1.8		{d3[1]}, [r5], r1
	vld1.8		{d3[2]}, [r5], r1
	vld1.8		{d3[3]}, [r5], r1
	vld1.8		{d3[4]}, [r5], r1
	vld1.8		{d3[5]}, [r5], r1
	vld1.8		{d3[6]}, [r5], r1
	vld1.8		{d3[7]}, [r5], r1
	
	vrev64		d5.u8,	d1.u8							
	vsubl.u8		q3,	d5, d4						
    vshr    	d5.u64, d5.u64, #8 							
    vsubl.u8 		q4, d5, d0
    
    vshl		d9.s64, d9.s64, #16
    vext		d9.s16, d9.s16, d6.s16, #1
    
    vrev64		d12.u8, d3.u8								
	vsubl.u8		q7, d12, d4						
	vshr		d12.u64, d12.u64, #8							
	vsubl.u8		q8, d12, d2  
   
    vld1.16		{q10}, [r6]!
    
    vshl		d17.s64, d17.s64, #16
    vext		d17.s16, d17.s16, d14.s16, #1
    
    vmull		q11.s32, d8.s16, d20.s16
    vmull		q12.s32, d16.s16, d20.s16
    vmlal		q11.s32, d9.s16, d21.s16
    vmlal		q12.s32, d17.s16, d21.s16
    
    vpadd		d22.s32, d23.s32, d22.s32
    vpadd		d23.s32, d25.s32, d24.s32
    vpaddl.s32		q11, q11
    vshl		q12.s64, q11.s64, #2
    vadd		q11.s64, q11.s64, q12.s64
    
    
    vrshr		q11.s64, q11.s64, #6
    
   
    vshl		q12.s64, q11.s64, #3
    vsub		q12.s64, q12.s64, q11.s64
    
    
    vld1.16		{q10}, [r6]!
    vdup		q6.s16, d22.s16[0]
    vdup		q7.s16, d23.s16[0]
    
    vaddl.u8		q11, d1, d3
    vshl		q11.s16, q11.s16, #4
    vdup		q11.s16, d23.s16[3]
    vadd		d1.s64, d24.s64, d25.s64
    
    
    vld1.16		{q12}, [r6]
    
    vdup		q13.s16, d1.s16[0]
    vsub		q13.s16, q11.s16, q13.s16
    
    
  	vmul		q5.s16, q6.s16, q10.s16
  	
  	vmul		q6.s16, q6.s16, q12.s16
  	
  	vadd		q0.s16, q5.s16, q13.s16
  	vadd		q1.s16, q6.s16, q13.s16
  	

LoopPlane:       
	vqrshrun	d6.u8, q0.s16, #5
	vqrshrun	d7.u8, q1.s16, #5
	subs		r12, r12, #1 
	vst1.64		{q3}, [r2], r3
	vadd		q0.s16, q0.s16, q7.s16
	vadd		q1.s16, q1.s16, q7.s16
	bne			LoopPlane
        
	pop      	{r4 - r10, pc}  
    
   @ .end
