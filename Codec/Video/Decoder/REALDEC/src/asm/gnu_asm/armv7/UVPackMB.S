@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************

#include "../../../voRVDecID.h"

    @AREA    |.text|, CODE, READONLY

    .section .text

	.global	YUV420T0YUV420PACKMB_Ctx
@-----------------------------------------------------------------------------------------------------
@YUV420T0YUV420PACKMB(VO_U8* srcu, VO_U8* srcv, VO_U8* dst, VO_U32 srcStride, VO_U32 dstStride)
@-----------------------------------------------------------------------------------------------------
   .ALIGN 4
YUV420T0YUV420PACKMB_Ctx:	@PROC
    stmfd       sp!,{r4,lr}   
    ldr	        r4,[sp,#8]  @dstStride
   
    vld1.64		{d0},   [r0,:64], r3   @u
    vld1.64		{d1},   [r0,:64], r3
    vld1.64		{d2},   [r1,:64], r3   @v
    vld1.64		{d3},   [r1,:64], r3
    
    vld1.64		{d4},   [r0,:64], r3   @u
    vld1.64		{d5},   [r0,:64], r3
    vld1.64		{d6},   [r1,:64], r3   @v
    vld1.64		{d7},   [r1,:64], r3
    
    vld1.64		{d8},   [r0,:64], r3   @u
    vld1.64		{d9},   [r0,:64], r3
    vld1.64		{d10},  [r1,:64], r3   @v
    vld1.64		{d11},  [r1,:64], r3
    
    vld1.64		{d12},  [r0,:64], r3   @u
    vld1.64		{d13},  [r0,:64]
    vld1.64		{d14},  [r1,:64], r3   @v
    vld1.64		{d15},  [r1,:64]
    
    vzip.8    q0, q1 
    vzip.8    q2, q3
    vzip.8    q4, q5
    vzip.8    q6, q7
    
    vst1.64     {q0},  [r2,:128], r4
    vst1.64     {q1},  [r2,:128], r4
    vst1.64     {q2},  [r2,:128], r4
    vst1.64     {q3},  [r2,:128], r4
    vst1.64     {q4},  [r2,:128], r4
    vst1.64     {q5},  [r2,:128], r4
    vst1.64     {q6},  [r2,:128], r4
    vst1.64     {q7},  [r2,:128]

    ldmfd       sp!,{r4,pc}	
    @ENDP