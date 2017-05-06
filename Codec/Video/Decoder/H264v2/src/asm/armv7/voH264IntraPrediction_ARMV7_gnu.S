@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************  
	@AREA	|.text|, CODE
 #include "../../defineID.h"	
	.section .text
 
	.global PredIntraChroma8x8H_ARMV7
	.global PredIntraChroma8x8V_ARMV7
	.global PredIntraChroma8x8Dc_ARMV7
	.global PredIntraChroma8x8Dc128_ARMV7
	.global PredIntraChroma8x8DcTop_ARMV7
	.global PredIntraChroma8x8DcLeft_ARMV7
	.global PredIntraLuma16x16H_ARMV7
	.global PredIntraLuma16x16V_ARMV7
	.global PredIntraLuma16x16Dc_ARMV7
	.global PredIntraLuma16x16Dc128_ARMV7
	.global PredIntraLuma16x16DcTop_ARMV7
	.global PredIntraLuma16x16DcLeft_ARMV7
	.global PredIntraLuma16x16P_ARMV7
	.global Predict8x8VerRight_ARMV7
	.global Predict8x8DownRight_ARMV7
	.global Predict8x8HorDown_ARMV7
	.global Predict8x8HorUp_ARMV7
	.global Predict8x8Ver_ARMV7
	.global Predict8x8VerLeft_ARMV7
	.global Predict8x8DownLeft_ARMV7
	.global Predict8x8Hor_ARMV7
	.global Predict8x8DC_ARMV7
	.global Predict8x8DC128_ARMV7
	.global Predict8x8DCLeft_ARMV7
	.global Predict8x8DCTop_ARMV7


PredIntraChroma8x8H_ARMV7:
    sub         r12, r0, #1
    vld1.8      {d0[]}, [r12], r1
    vld1.8      {d1[]}, [r12], r1
    vld1.8      {d2[]}, [r12], r1
    vld1.8      {d3[]}, [r12], r1
    vld1.8      {d4[]}, [r12], r1
    vld1.8      {d5[]}, [r12], r1
    vld1.8      {d6[]}, [r12], r1
    vld1.8      {d7[]}, [r12]
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d1}, [r2  ], r3
    vst1.64     {d2}, [r2  ], r3
    vst1.64     {d3}, [r2  ], r3    
    vst1.64     {d4}, [r2  ], r3
    vst1.64     {d5}, [r2  ], r3   
    vst1.64     {d6}, [r2  ], r3
    vst1.64     {d7}, [r2  ]
    bx			lr

PredIntraChroma8x8V_ARMV7:
    sub         r0, r0, r1
    vld1.64     {d0}, [r0  ]
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ]
    bx          lr

PredIntraChroma8x8DcLeft_ARMV7 :
    push        {r4-r8,lr}
    sub         r0, r0, #1
    ldrb        r8, [r0], r1
    ldrb        r6, [r0], r1
    ldrb        r7, [r0], r1    
    add         r8, r8, r6
    ldrb        r5, [r0], r1
    add         r8, r8, r7
    add         r8, r8, r5
    vdup.16     q1, r8
    ldrb        r8, [r0], r1
    ldrb        r6, [r0], r1
    ldrb        r7, [r0], r1    
    add         r8, r8, r6
    ldrb        r5, [r0], r1
    add         r8, r8, r7
    add         r8, r8, r5
    vdup.16     q2, r8
    vrshrn.i16  d0, q1, #2
    vrshrn.i16  d1, q2, #2
    
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d1}, [r2  ], r3
    vst1.64     {d1}, [r2  ], r3
    vst1.64     {d1}, [r2  ], r3
    vst1.64     {d1}, [r2  ], r3

    pop         {r4-r8,pc}

PredIntraChroma8x8DcTop_ARMV7 :
    push        {r4-r8,lr}
    sub         r5, r0, r1
    vld1.64     {d0}, [r5  ]
    vpaddl.u8   d0, d0
    vpadd.u16   d0, d0, d0
    vdup.16     d2, d0[0]
    vdup.16     d3, d0[1]
    vrshrn.i16  d0, q1, #2
    
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3

    pop         {r4-r8,pc}

PredIntraChroma8x8Dc_ARMV7 :
    push        {r4-r8,lr}
    
    sub         r5, r0, r1
    sub         r0, r0, #1
    vld1.64     {d0}, [r5  ]
    ldrb        r8, [r0], r1
    vpaddl.u8   d0, d0
    ldrb        r6, [r0], r1
    vpadd.u16   d0, d0, d0        @d0[0]:sum0 d0[1]:sum1

    ldrb        r7, [r0], r1    
    add         r8, r8, r6
    ldrb        r5, [r0], r1
    add         r8, r8, r7
    ldrb        r6, [r0], r1
    add         r8, r8, r5        @r8 sum2
    
    ldrb        r7, [r0], r1   
    add         r6, r7, r6
    ldrb        r5, [r0], r1
    add         r6, r6, r5
    ldrb        r7, [r0], r1
    add         r6, r6, r7       @r6 sum3
    
    vdup.16     d2, d0[0]
    vdup.16     d4, d0[1]
    vdup.16     d3, r8
    vdup.16     d5, r6
    vadd.i16    d6, d2, d3
    vadd.i16    d7, d4, d5
    vrshrn.i16  d0, q2, #2      @dc1 dc2
    vrshrn.i16  d1, q3, #3      @dc0 dc3
    vtrn.32     d1, d0
    vrev64.32   q1, q0

    vst1.64     {d1}, [r2  ], r3
    vst1.64     {d1}, [r2  ], r3
    vst1.64     {d1}, [r2  ], r3
    vst1.64     {d1}, [r2  ], r3
    vst1.64     {d2}, [r2  ], r3
    vst1.64     {d2}, [r2  ], r3
    vst1.64     {d2}, [r2  ], r3
    vst1.64     {d2}, [r2  ], r3
   

    pop         {r4-r8,pc}

PredIntraChroma8x8Dc128_ARMV7 :
    vmov.i8      d0, #128

    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3
    vst1.64     {d0}, [r2  ], r3

    bx            lr

PredIntraLuma16x16Dc_ARMV7 :
    push        {r4-r8,lr}
    
    sub         r5, r0, r1
    sub         r0, r0, #1
    vld1.64     {d0-d1}, [r5  ]
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

    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ]

    pop         {r4-r8,pc}
    
PredIntraLuma16x16DcLeft_ARMV7 :
    push        {r4-r8,lr}
    
    sub         r0, r0, #1
    ldrb        r8, [r0], r1
    ldrb        r6, [r0], r1

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
    vdup.16     d0, r8
    vrshr.u16   d0, d0, #4
    vdup.8      q0, d0[0]

    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ]

    pop         {r4-r8,pc}
    
PredIntraLuma16x16DcTop_ARMV7 :
    push        {r4-r8,lr}
    
    sub         r5, r0, r1
    vld1.64     {d0-d1}, [r5  ]
    vaddl.u8    q0, d0, d1
    vadd.u16    d0, d0, d1
    vpadd.u16   d0, d0, d0
    vpadd.u16   d0, d0, d0

    vrshr.u16   d0, d0, #4
    vdup.8      q0, d0[0]

    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ]

    pop         {r4-r8,pc}

PredIntraLuma16x16Dc128_ARMV7 :
    vmov.i8      q0, #128

    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ]

    bx            lr
    
PredIntraLuma16x16H_ARMV7:
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
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d2-d3}, [r2  ], r3  
    vst1.64     {d4-d5}, [r2  ], r3
    vst1.64     {d6-d7}, [r2  ], r3
    vst1.64     {d8-d9}, [r2  ], r3
    vst1.64     {d10-d11}, [r2  ], r3
    vst1.64     {d12-d13}, [r2  ], r3
    vst1.64     {d14-d15}, [r2  ], r3
    vst1.64     {d16-d17}, [r2  ], r3
    vst1.64     {d18-d19}, [r2  ], r3  
    vst1.64     {d20-d21}, [r2  ], r3
    vst1.64     {d22-d23}, [r2  ], r3
    vst1.64     {d24-d25}, [r2  ], r3
    vst1.64     {d26-d27}, [r2  ], r3
    vst1.64     {d28-d29}, [r2  ], r3
    vst1.64     {d30-d31}, [r2  ]
    bx          lr

PredIntraLuma16x16V_ARMV7 :
    sub         r0, r0, r1
    vld1.64     {d0-d1}, [r0  ]
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ], r3
    vst1.64     {d0-d1}, [r2  ]
    bx          lr
    
voMultiplierTable16x16:
    .hword   7,  6,  5,  4,  3,  2,  1,  8 
    .hword   0,  1,  2,  3,  4,  5,  6,  7
    .hword   8,  9, 10, 11, 12, 13, 14, 15

p16weight:
    .short          1,2,3,4,5,6,7,8

PredIntraLuma16x16P_ARMV7:

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


Predict8x8VerRight_ARMV7:
        STMFD   sp!, {r4-r6, lr}
        
        sub         r0, r0, #1
        cmp         r2, #1
        subeq       r0, r0, r1
        ldreqb      r4, [r0], r1
        ldrneb      r4, [r0]
        moveq       r6, r4
        vld1.8      {d6[0]}, [r0]
        vld1.8      {d0[0]}, [r0], r1
        vld1.8      {d0[1]}, [r0], r1
        vld1.8      {d0[2]}, [r0], r1
        vld1.8      {d0[3]}, [r0], r1
        vld1.8      {d0[4]}, [r0], r1
        vld1.8      {d0[5]}, [r0], r1
        vld1.8      {d0[6]}, [r0], r1
        vld1.8      {d0[7]}, [r0]
        
        sub         r0, r0, r1, lsl #3
        vld1.8      {d7[0]}, [r0]
        add         r0, r0, #1
        vld1.8      {d8[0]}, [r0]
        ldrneb      r6, [r0]           
        vld1.64     {d1}, [r0]
        cmp         r3,   #1
        ldreqb      r5,   [r0, #8]
        ldrne       r5,   [r0, #7]
        
        vmov         s4, r4
        vmov         s6, r6        
        vsli.64     q1, q0, #8
        
        vmov        d4, d0
        LSL         r5, r5, #24
        vmov        s11, r5
        vsri.64     q2, q0, #8
        vhadd.u8    q2, q2, q1
        vhsub.u8    q2, q0, q2
        vsub.u8     q0, q0, q2
        
        vhadd.u8    d8, d8, d6
        vhsub.u8    d8, d7, d8
        vsub.u8     d6, d7, d8  
        
        
        @q0 l0 l1 l2 l3 l4 l5 l6 l7  u0 u1 u2 u3 u4 u5 u6
        @d6 lt
        vmov d2, d6
        vmov d3, d6
        vsli.64  q1, q0, #8
        vmov d5, d0
        vsli.64  q2, q1, #8
        
        vhadd.u8  q2, q2, q0
        vhsub.u8  q2, q1, q2 
        vsub.u8   q2, q1, q2
        
        vshr.u64 d4, d4, #8
        vrev64.8 d4, d4
        vshr.u64 d4, d4, #16
        vshl.u64 d4, d4, #16
        vshl.u16 d6, d4, #8
        vmov   d7, d4
        vqshrn.u16 d4, q3, #8
        
        @q2 0  l5 l3 l1 0 l4 l2 l0 u0 u1 u2 u3 u4 u5 u6 u7
        vhsub.u8 d1, d3, d1
        vsub.u8  d1, d3, d1
        @d1 a0 a1 a2 a3 a4 a5 a6 a7  
@           
        add r0, r0, r1
@ 
        vst1.8  d1, [r0], r1
        vst1.8  d5, [r0], r1
@                  
        vshr.u64 d2, d4, #56
        vsli.64  d2, d1, #8
        vshr.u64 d3, d4, #24
        vsli.64  d3, d5, #8
        
        vst1.8  d2, [r0], r1
        vst1.8  d3, [r0], r1    
    
        vshr.u64 d2, d4, #48
        vsli.64  d2, d1, #16
        vshr.u64 d3, d4, #16
        vsli.64  d3, d5, #16
        
        vst1.8  d2, [r0], r1
        vst1.8  d3, [r0], r1    
      
        vshr.u64 d2, d4, #40
        vsli.64  d2, d1, #24
        vshr.u64 d3, d4, #8
        vsli.64  d3, d5, #24
        
        vst1.8  d2, [r0], r1
        vst1.8  d3, [r0]           
                   
        LDMFD   sp!, {r4-r6, pc}	 
 
Predict8x8DownRight_ARMV7:
        STMFD   sp!, {r4-r6, lr}
        
        sub         r0, r0, #1
        cmp         r2, #1
        subeq       r0, r0, r1
        ldreqb      r4, [r0], r1
        ldrneb      r4, [r0]
        moveq       r6, r4
        vld1.8      {d6[0]}, [r0]
        vld1.8      {d0[0]}, [r0], r1
        vld1.8      {d0[1]}, [r0], r1
        vld1.8      {d0[2]}, [r0], r1
        vld1.8      {d0[3]}, [r0], r1
        vld1.8      {d0[4]}, [r0], r1
        vld1.8      {d0[5]}, [r0], r1
        vld1.8      {d0[6]}, [r0], r1
        vld1.8      {d0[7]}, [r0]
        
        sub         r0, r0, r1, lsl #3
        vld1.8      {d7[0]}, [r0]
        add         r0, r0, #1
        vld1.8      {d8[0]}, [r0]
        ldrneb      r6, [r0]           
        vld1.64     {d1}, [r0]
        cmp         r3,   #1
        ldreqb      r5,   [r0, #8]
        ldrne       r5,   [r0, #7]
        
        vmov         s4, r4
        vmov         s6, r6        
        vsli.64     q1, q0, #8
        
        vmov        d4, d0
        LSL         r5, r5, #24
        vmov        s11, r5
        vsri.64     q2, q0, #8
        vhadd.u8    q2, q2, q1
        vhsub.u8    q2, q0, q2
        vsub.u8     q0, q0, q2
        
        vhadd.u8    d8, d8, d6
        vhsub.u8    d8, d7, d8
        vsub.u8     d6, d7, d8  
        
        
        @q0 l0 l1 l2 l3 l4 l5 l6 l7  u0 u1 u2 u3 u4 u5 u6 u7
        @d6 lt
        vmov d2, d6
        vmov d3, d6
        vsli.64  q1, q0, #8
        vmov d5, d0
        vswp d2, d0
        vshr.u64 d4, d2, #8
        vsli.64  d5, d3, #8
        
        vhadd.u8  q2, q2, q0
        vhsub.u8  q2, q1, q2 
        vsub.u8   q2, q1, q2
        
        vrev64.8 d4, d4
        vshr.u64 d4, d4, #8
        vshl.u64 d4, d4, #8
        
        @q2 0  l6 l5 l4 l3 l2 l1 l0 u0 u1 u2 u3 u4 u5 u6 u7
            
        add r0, r0, r1
@         
        vst1.8  d5, [r0], r1
        
        vshr.u64 d3, d4, #56
        vsli.64  d3, d5, #8      
        vst1.8  d3, [r0], r1
  
        vshr.u64 d3, d4, #48
        vsli.64  d3, d5, #16      
        vst1.8  d3, [r0], r1

        vshr.u64 d3, d4, #40
        vsli.64  d3, d5, #24      
        vst1.8  d3, [r0], r1
 
        vshr.u64 d3, d4, #32
        vsli.64  d3, d5, #32      
        vst1.8  d3, [r0], r1
        
        vshr.u64 d3, d4, #24
        vsli.64  d3, d5, #40      
        vst1.8  d3, [r0], r1
   
        vshr.u64 d3, d4, #16
        vsli.64  d3, d5, #48      
        vst1.8  d3, [r0], r1
 
        vshr.u64 d3, d4, #8
        vsli.64  d3, d5, #56      
        vst1.8  d3, [r0]                    
                                                         
        LDMFD   sp!, {r4-r6, pc}
        
Predict8x8HorDown_ARMV7:
        STMFD   sp!, {r4-r6, lr}
        
        sub         r0, r0, #1
        cmp         r2, #1
        subeq       r0, r0, r1
        ldreqb      r4, [r0], r1
        ldrneb      r4, [r0]
        moveq       r6, r4
        vld1.8      {d6[0]}, [r0]
        vld1.8      {d0[0]}, [r0], r1
        vld1.8      {d0[1]}, [r0], r1
        vld1.8      {d0[2]}, [r0], r1
        vld1.8      {d0[3]}, [r0], r1
        vld1.8      {d0[4]}, [r0], r1
        vld1.8      {d0[5]}, [r0], r1
        vld1.8      {d0[6]}, [r0], r1
        vld1.8      {d0[7]}, [r0]
        
        sub         r0, r0, r1, lsl #3
        vld1.8      {d7[0]}, [r0]
        add         r0, r0, #1
        vld1.8      {d8[0]}, [r0]
        ldrneb      r6, [r0]           
        vld1.64     {d1}, [r0]
        cmp         r3,   #1
        ldreqb      r5,   [r0, #8]
        ldrne       r5,   [r0, #7]
        
        vmov         s4, r4
        vmov         s6, r6        
        vsli.64     q1, q0, #8
        
        vmov        d4, d0
        LSL         r5, r5, #24
        vmov        s11, r5
        vsri.64     q2, q0, #8
        vhadd.u8    q2, q2, q1
        vhsub.u8    q2, q0, q2
        vsub.u8     q0, q0, q2
        
        vhadd.u8    d8, d8, d6
        vhsub.u8    d8, d7, d8
        vsub.u8     d6, d7, d8  
        
        
        @q0 l0 l1 l2 l3 l4 l5 l6 l7  u0 u1 u2 u3 u4 u5 u6
        @d6 lt
        vmov d2, d6
        vmov d3, d6
        vsli.64  q1, q0, #8
        vmov d5, d0
        vswp d2, d0
        vshr.u64 d4, d2, #8
        vsli.64  d5, d3, #8
        
        vhadd.u8  q2, q2, q0
        vhsub.u8  q2, q1, q2 
        vsub.u8   q2, q1, q2
        
        vrev64.8 d4, d4
        vshr.u64 d4, d4, #8
        vshl.u64 d4, d4, #8
        
        @q2 0 l6 l5 l4 l3 l2 l1 l0 u0 u1 u2 u3 u4 u5 u6 u7
        vhsub.u8 d2, d0, d2
        vsub.u8  d2, d0, d2
        vrev64.8 d2, d2
  @      vshr.u64 d2, d2, #8
  @      vshl.u64 d2, d2, #8        
        @d2 0 a6 a5 a4 a3 a2 a1 a0  
        vtrn.8   d4, d2
        vtrn.16  d4, d2
        vtrn.32  d4, d2
        @ d4 0 a7 l6 a6 l5 a5 l4 a4  d2 l3 a3 l2 a2 l1 a1 l0 a0        
@           
        add r0, r0, r1
        
  @      vshr.u64 d3, d2, #56
  @      vsli.64  d3, d5, #8
        vext.8 d3, d2, d5, #7           
        vst1.8  d3, [r0], r1         
     
        vext.8 d3, d2, d5, #5           
        vst1.8  d3, [r0], r1 
        
        vext.8 d3, d2, d5, #3           
        vst1.8  d3, [r0], r1 
        
        vext.8 d3, d2, d5, #1           
        vst1.8  d3, [r0], r1
        
        vext.8 d3, d4, d2, #7           
        vst1.8  d3, [r0], r1 
        
        vext.8 d3, d4, d2, #5           
        vst1.8  d3, [r0], r1
        
        vext.8 d3, d4, d2, #3           
        vst1.8  d3, [r0], r1
        
        vext.8 d3, d4, d2, #1           
        vst1.8  d3, [r0], r1            
        
        LDMFD   sp!, {r4-r6, pc}  
 
Predict8x8HorUp_ARMV7:
        STMFD   sp!, {r4-r6, lr}
        
        sub         r0, r0, #1
        cmp         r2, #1
        subeq       r0, r0, r1
        ldreqb      r4, [r0], r1
        ldrneb      r4, [r0]
        vld1.8      {d0[0]}, [r0], r1
        vld1.8      {d0[1]}, [r0], r1
        vld1.8      {d0[2]}, [r0], r1
        vld1.8      {d0[3]}, [r0], r1
        vld1.8      {d0[4]}, [r0], r1
        vld1.8      {d0[5]}, [r0], r1
        vld1.8      {d0[6]}, [r0], r1
        vld1.8      {d0[7]}, [r0], r1
        
        vmov    s2, r4
        vsli.64 d1, d0, #8 @a
        vmov    d2, d0 
        vsri.64 d2, d0, #8 @c
        
        vhadd.u8 d2, d2, d1
        vhsub.u8 d2, d0, d2
        vsub.u8  d2, d0, d2
           
        @d2 l0 ... l7
        vmov  d1, d2
        vsri.64  d1, d2, #8 @b
        vmov  d0, d1
        vsri.64  d0, d1, #8 @a
        
        vhadd.u8 d0, d2, d0
        vhsub.u8 d0, d1, d0
        vsub.u8  d0, d1, d0
        vhsub.u8  d1, d2, d1
        vsub.u8  d1, d2, d1
        @d0 l0 ... l7
        @d1 a0 ... a7
                
        sub     r0, r0, r1, lsl #3
        add   r0, r0, #1
        
        vtrn.8  d1, d0
        vtrn.16 d1, d0
        vtrn.32 d1, d0
        
        @d1 d0 a0 l0 a1 l1 ... a7 l7             
        vst1.8  d1, [r0], r1
        
        vext.8 d3, d1, d0, #2 
        vst1.8  d3, [r0], r1  
        
        vext.8 d3, d1, d0, #4 
        vst1.8  d3, [r0], r1
        
        vext.8 d3, d1, d0, #6 
        vst1.8  d3, [r0], r1
        
        vmov    d3, d0
        vst1.8  d0, [r0], r1
        
        vsri.64 d3, d3, #16
        vst1.8  d3, [r0], r1
        
        vsri.64 d3, d3, #16
        vst1.8  d3, [r0], r1
        
        vsri.64 d3, d3, #16
        vst1.8  d3, [r0]
        
        LDMFD   sp!, {r4-r6, pc}

Predict8x8Hor_ARMV7:
        STMFD   sp!, {r4-r6, lr}
        
        sub         r0, r0, #1
        cmp         r2, #1
        subeq       r0, r0, r1
        ldreqb      r4, [r0], r1
        ldrneb      r4, [r0]
        vld1.8      {d0[0]}, [r0], r1
        vld1.8      {d0[1]}, [r0], r1
        vld1.8      {d0[2]}, [r0], r1
        vld1.8      {d0[3]}, [r0], r1
        vld1.8      {d0[4]}, [r0], r1
        vld1.8      {d0[5]}, [r0], r1
        vld1.8      {d0[6]}, [r0], r1
        vld1.8      {d0[7]}, [r0], r1
        
        vmov    s2, r4
        vsli.64 d1, d0, #8 @a
        vmov    d2, d0 
        vsri.64 d2, d0, #8 @c
        
        vhadd.u8 d2, d2, d1
        vhsub.u8 d2, d0, d2
        vsub.u8  d2, d0, d2
           
        @d2 l0 ... l7
        
                
        sub   r0, r0, r1, lsl #3
        add   r0, r0, #1
        
        vdup.8  d0, d2[0]        
        vst1.8  d0, [r0], r1
        
        vdup.8  d1, d2[1]        
        vst1.8  d1, [r0], r1
       
        vdup.8  d0, d2[2]        
        vst1.8  d0, [r0], r1
        
        vdup.8  d1, d2[3]        
        vst1.8  d1, [r0], r1
        
        vdup.8  d0, d2[4]        
        vst1.8  d0, [r0], r1
        
        vdup.8  d1, d2[5]        
        vst1.8  d1, [r0], r1
        
        vdup.8  d0, d2[6]        
        vst1.8  d0, [r0], r1
        
        vdup.8  d1, d2[7]        
        vst1.8  d1, [r0] 
       
        LDMFD   sp!, {r4-r6, pc}


Predict8x8Ver_ARMV7:
        STMFD   sp!, {r4-r6, lr}
        
        sub      r0, r0, r1
        vld1.64     {d0}, [r0] 
        cmp         r2, #1
        ldreqb      r4,   [r0, #-1]
        ldrneb       r4,   [r0]     
        cmp         r3,   #1
        ldreqb      r6,   [r0, #8]
        ldrne       r6,   [r0, #7]
        
        vmov    s2, r4
        vsli.64 d1, d0, #8 @a
        lsl     r6, r6, #24
        vmov    s5, r6 
        vsri.64 d2, d0, #8 @c
        
        add      r0, r0, r1
        vhadd.u8 d2, d2, d1
        vhsub.u8 d2, d0, d2
        vsub.u8  d2, d0, d2
           
        @d2 l0 ... l7    
        vst1.8  d2, [r0], r1
        vst1.8  d2, [r0], r1
        vst1.8  d2, [r0], r1
        vst1.8  d2, [r0], r1
        vst1.8  d2, [r0], r1
        vst1.8  d2, [r0], r1
        vst1.8  d2, [r0], r1
        vst1.8  d2, [r0]
        
        LDMFD   sp!, {r4-r6, pc}
 
Predict8x8VerLeft_ARMV7:
        STMFD   sp!, {r4-r6, lr}

        mov     r5, #1
        sub      r0, r0, r1
        vld1.64     {d0}, [r0] 
        cmp         r2, #1
        ldreqb      r4,   [r0, #-1]
        ldrne       r4,   [r0]     
        cmp         r3,   #1
        ldreqb      r6,   [r0, #8]
        ldrneb       r6,   [r0, #7]
        
        vmov    s2, r4
        vsli.64 d1, d0, #8 @a
        lsl     r6, r6, #24
        vmov    s5, r6 
        vsri.64 d2, d0, #8 @c
        
       
        vhadd.u8 d2, d2, d1
        vhsub.u8 d2, d0, d2
        vsub.u8  d0, d0, d2
           
        @d0 l0 ... l7
        cmp r3, #1
        add   r0, r0, #7
        bne no_right       
        vld1.8   {d1[0]}, [r0], r5
        vld1.64  {d2}, [r0]
        vsli.64  d1, d2, #8
        vmov d3, d2
        vsri.64 d3, d2, #8 
        vhadd.u8 d3, d3, d1
        vhsub.u8 d1, d2, d3 
        vsub.u8  d1, d2, d1         
        b   no_right_end
no_right:
        vld1.8   {d1[]}, [r0], r5
no_right_end:
        sub      r0, r0, #8
        add      r0, r0, r1
        @q0 u0 ... u15
        vmov     q1, q0
        vshr.u64 q1, q1, #8
        vsli.64  d2, d1, #56
        vmov     q2, q1
        vshr.u64 q2, q2, #8
        vsli.64  d4, d3, #56
        @q0 u0 ... u13 u14 u15
        @q1 u1 ... u14 u15
        @q2 u2 ... u15
        vhadd.u8 q2, q2, q0
        vhsub.u8 q2, q1, q2
        vsub.u8  q2, q1, q2
        vhsub.u8 q1, q0, q1
        vsub.u8  q0, q0, q1
        @q2 l0 ... l13
        @q0 a0 ... a13
        @vtrn.8  q0, q2
        @vtrn.16 q0, q2
        @vtrn.32 q0, q2
        @q0 a0 l0 ... a3 l3 a8  l8 ... a11 l11 
        @q2 a4 l4 ... a7 l7 a12 l12 a13 l13 
                             
        vst1.8  d0, [r0], r1
        vst1.8  d4, [r0], r1
        vext.8  d2, d0, d1, #1
        vst1.8  d2, [r0], r1
        vext.8  d3, d4, d5, #1
        vst1.8  d3, [r0], r1
        vext.8  d2, d0, d1, #2
        vst1.8  d2, [r0], r1
        vext.8  d3, d4, d5, #2
        vst1.8  d3, [r0], r1
        vext.8  d2, d0, d1, #3
        vst1.8  d2, [r0], r1
        vext.8  d3, d4, d5, #3
        vst1.8  d3, [r0]
        
        LDMFD   sp!, {r4-r6, pc}    
        
Predict8x8DownLeft_ARMV7:
        STMFD   sp!, {r4-r6, lr}

        mov     r5, #1
        sub      r0, r0, r1
        vld1.64     {d0}, [r0] 
        cmp         r2, #1
        ldreqb      r4,   [r0, #-1]
        ldrne       r4,   [r0]     
        cmp         r3,   #1
        ldreqb      r6,   [r0, #8]
        ldrneb       r6,   [r0, #7]
        
        vmov    s2, r4
        vsli.64 d1, d0, #8 @a
        lsl     r6, r6, #24
        vmov    s5, r6 
        vsri.64 d2, d0, #8 @c
        
       
        vhadd.u8 d2, d2, d1
        vhsub.u8 d2, d0, d2
        vsub.u8  d0, d0, d2
           
        @d0 l0 ... l7
        cmp r3, #1
        add   r0, r0, #7
        bne no_right2       
        vld1.8   {d1[0]}, [r0], r5
        vld1.64  {d2}, [r0]
        vsli.64  d1, d2, #8
        vmov d3, d2
        vsri.64 d3, d2, #8 
        vhadd.u8 d3, d3, d1
        vhsub.u8 d1, d2, d3 
        vsub.u8  d1, d2, d1         
        b   no_right_end2
no_right2:
        vld1.8   {d1[]}, [r0], r5
no_right_end2:
        sub      r0, r0, #8
        add      r0, r0, r1
        @q0 u0 ... u15
        vmov     q1, q0
        vsri.64 q1, q1, #8
        vsli.64  d2, d1, #56
        vmov     q2, q1
        vsri.64 q2, q2, #8
        vsli.64  d4, d3, #56
        @q0 u0 ... u13 u14 u15
        @q1 u1 ... u14 u15
        @q2 u2 ... u15 u15
        vhadd.u8 q2, q2, q0
        vhsub.u8 q2, q1, q2
        vsub.u8  q2, q1, q2
        @vhsub.u8 q1, q0, q1
        @vsub.u8  q0, q0, q1
        @q2 l0 ... l13
        @q0 a0 ... a13
        @vtrn.8  q0, q2
        @vtrn.16 q0, q2
        @vtrn.32 q0, q2
        @q0 a0 l0 ... a3 l3 a8  l8 ... a11 l11 
        @q2 a4 l4 ... a7 l7 a12 l12 a13 l13 
                             
        vst1.8  d4, [r0], r1
        vext.8  d2, d4, d5, #1
        vst1.8  d2, [r0], r1
        vext.8  d3, d4, d5, #2
        vst1.8  d3, [r0], r1
        vext.8  d2, d4, d5, #3
        vst1.8  d2, [r0], r1
        vext.8  d3, d4, d5, #4
        vst1.8  d3, [r0], r1
        vext.8  d2, d4, d5, #5
        vst1.8  d2, [r0], r1
        vext.8  d3, d4, d5, #6
        vst1.8  d3, [r0], r1
        vext.8  d2, d4, d5, #7
        vst1.8  d2, [r0]
        
        LDMFD   sp!, {r4-r6, pc}  

Predict8x8DC_ARMV7:
        STMFD   sp!, {r4-r6, lr}
        
        sub         r0, r0, #1
        cmp         r2, #1
        subeq       r0, r0, r1
        ldreqb      r4, [r0], r1
        ldrneb      r4, [r0]
        moveq       r6, r4
        vld1.8      {d0[0]}, [r0], r1
        vld1.8      {d0[1]}, [r0], r1
        vld1.8      {d0[2]}, [r0], r1
        vld1.8      {d0[3]}, [r0], r1
        vld1.8      {d0[4]}, [r0], r1
        vld1.8      {d0[5]}, [r0], r1
        vld1.8      {d0[6]}, [r0], r1
        vld1.8      {d0[7]}, [r0]
        
        sub         r0, r0, r1, lsl #3
        add         r0, r0, #1
        ldrneb      r6, [r0]           
        vld1.64     {d1}, [r0]
        cmp         r3,   #1
        ldreqb      r5,   [r0, #8]
        ldrne       r5,   [r0, #7]
        
        vmov         s4, r4
        vmov         s6, r6        
        vsli.64     q1, q0, #8
        
        vmov        d4, d0
        LSL         r5, r5, #24
        vmov        s11, r5
        vsri.64     q2, q0, #8
        vhadd.u8    q2, q2, q1
        vhsub.u8    q2, q0, q2
        vsub.u8     q0, q0, q2
        
        @q0 l0 l1 l2 l3 l4 l5 l6 l7  u0 u1 u2 u3 u4 u5 u6
        vmov.i16    q1, #1
        vpadal.u8   q1, q0
        vpaddl.u16  q0, q1
        vpaddl.u32  q1, q0
        vadd.i64  d0, d2, d3
        vshr.u64  d1, d0, #4
        add r0, r0, r1
        vdup.8 d4, d1[0]
        vmov d5,d4
        
  @      vshr.u64 d3, d2, #56
  @      vsli.64  d3, d5, #8        
        vst1.8  d4, [r0], r1         
        vst1.8  d5, [r0], r1
        vst1.8  d4, [r0], r1         
        vst1.8  d5, [r0], r1
        vst1.8  d4, [r0], r1         
        vst1.8  d5, [r0], r1
        vst1.8  d4, [r0], r1         
        vst1.8  d5, [r0]
        LDMFD   sp!, {r4-r6, pc}   
        
Predict8x8DCLeft_ARMV7:
        STMFD   sp!, {r4-r6, lr}
        
        sub         r0, r0, #1
        cmp         r2, #1
        subeq       r0, r0, r1
        ldreqb      r4, [r0], r1
        ldrneb      r4, [r0]
        vld1.8      {d0[0]}, [r0], r1
        vld1.8      {d0[1]}, [r0], r1
        vld1.8      {d0[2]}, [r0], r1
        vld1.8      {d0[3]}, [r0], r1
        vld1.8      {d0[4]}, [r0], r1
        vld1.8      {d0[5]}, [r0], r1
        vld1.8      {d0[6]}, [r0], r1
        vld1.8      {d0[7]}, [r0], r1
        
        vmov        s2, r4       
        vsli.64     d1, d0, #8
        
        vmov        d2, d0
        vsri.64     d2, d0, #8
        vhadd.u8    d2, d2, d1
        vhsub.u8    d2, d0, d2
        vsub.u8     d0, d0, d2
        
        @q0 l0 l1 l2 l3 l4 l5 l6 l7  u0 u1 u2 u3 u4 u5 u6
        vmov.i16    d1, #1
        vpadal.u8   d1, d0
        vpaddl.u16  d0, d1
        vpaddl.u32  d1, d0
        vshr.u64  d1, d1, #3
        sub r0, r0, r1, lsl #3
        add r0, r0, #1
        vdup.8 d4, d1[0]
        vmov d5,d4
        
  @      vshr.u64 d3, d2, #56
  @      vsli.64  d3, d5, #8        
        vst1.8  d4, [r0], r1         
        vst1.8  d5, [r0], r1
        vst1.8  d4, [r0], r1         
        vst1.8  d5, [r0], r1
        vst1.8  d4, [r0], r1         
        vst1.8  d5, [r0], r1
        vst1.8  d4, [r0], r1         
        vst1.8  d5, [r0]
        LDMFD   sp!, {r4-r6, pc}         

Predict8x8DCTop_ARMV7:
        STMFD   sp!, {r4-r6, lr}
        
        sub       r0, r0, r1
        cmp         r2, #1
        ldreqb      r4, [r0, #-1]
        ldrneb      r4, [r0]
        
        vld1.64     {d0}, [r0]
        cmp         r3,   #1
        ldreqb      r5,   [r0, #8]
        ldrne       r5,   [r0, #7]
        
        vmov        s2, r4       
        vsli.64     d1, d0, #8
        
        lsl      r5, r5, #24
        vmov        s5, r5
        vsri.64     d2, d0, #8
        vhadd.u8    d2, d2, d1
        vhsub.u8    d2, d0, d2
        vsub.u8     d0, d0, d2
        
        @q0 l0 l1 l2 l3 l4 l5 l6 l7  u0 u1 u2 u3 u4 u5 u6
        vmov.i16    d1, #1
        vpadal.u8   d1, d0
        vpaddl.u16  d0, d1
        vpaddl.u32  d1, d0
        vshr.u64  d1, d1, #3
        add r0, r0, r1
        vdup.8 d4, d1[0]
        vmov d5,d4
        
  @      vshr.u64 d3, d2, #56
  @      vsli.64  d3, d5, #8        
        vst1.8  d4, [r0], r1         
        vst1.8  d5, [r0], r1
        vst1.8  d4, [r0], r1         
        vst1.8  d5, [r0], r1
        vst1.8  d4, [r0], r1         
        vst1.8  d5, [r0], r1
        vst1.8  d4, [r0], r1         
        vst1.8  d5, [r0]
        LDMFD   sp!, {r4-r6, pc} 

Predict8x8DC128_ARMV7:
        vmov.i8  d4, #128
        vmov d5, d4 
        
        vst1.8  d4, [r0], r1         
        vst1.8  d5, [r0], r1
        vst1.8  d4, [r0], r1         
        vst1.8  d5, [r0], r1
        vst1.8  d4, [r0], r1         
        vst1.8  d5, [r0], r1
        vst1.8  d4, [r0], r1         
        vst1.8  d5, [r0]                                	                         	       
        
        bx lr           
    .end
