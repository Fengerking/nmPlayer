@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************

	@AREA    |.text|, CODE, READONLY
	#include "../../../voRVDecID.h"
	.section .text

	.global	CopyBlock16x16_Ctx
	.global	AddBlock16x16_Ctx	
	.global	CopyBlock8x8_Ctx
	.global	AddBlock8x8_Ctx
	
	.global	InterpolateDirectLuma_Ctx
	.global	InterpolateDirectChroma_Ctx	
@-----------------------------------------------------------------------------------------------------
@void RV_FASTCALL CopyBlock16x16(const U8 *Src, U8 *Dst,  U32 SrcPitch, U32 DstPitch)
@-----------------------------------------------------------------------------------------------------
   .ALIGN 4
CopyBlock16x16_Ctx:	@PROC
		stmdb        sp!,{r4,lr}
    
    vld1.64		{q0},  [r0,:64], r2
    vld1.64		{q1},  [r0,:64], r2
    vld1.64		{q2},  [r0,:64], r2
    vld1.64		{q3},  [r0,:64], r2
    vld1.64		{q4},  [r0,:64], r2
    vld1.64		{q5},  [r0,:64], r2
    vld1.64		{q6},  [r0,:64], r2
    vld1.64		{q7},  [r0,:64], r2
    vld1.64		{q8},  [r0,:64], r2
    vld1.64		{q9},  [r0,:64], r2
    vld1.64		{q10}, [r0,:64], r2
    vld1.64		{q11}, [r0,:64], r2
    vld1.64		{q12}, [r0,:64], r2
    vld1.64		{q13}, [r0,:64], r2
    vld1.64		{q14}, [r0,:64], r2
    vld1.64		{q15}, [r0,:64]
    
    vst1.64     {q0},  [r1,:64], r3
    vst1.64     {q1},  [r1,:64], r3
    vst1.64     {q2},  [r1,:64], r3
    vst1.64     {q3},  [r1,:64], r3
    vst1.64     {q4},  [r1,:64], r3
    vst1.64     {q5},  [r1,:64], r3
    vst1.64     {q6},  [r1,:64], r3
    vst1.64     {q7},  [r1,:64], r3
    vst1.64     {q8},  [r1,:64], r3
    vst1.64     {q9},  [r1,:64], r3
    vst1.64     {q10}, [r1,:64], r3
    vst1.64     {q11}, [r1,:64], r3
    vst1.64     {q12}, [r1,:64], r3
    vst1.64     {q13}, [r1,:64], r3
    vst1.64     {q14}, [r1,:64], r3
    vst1.64     {q15}, [r1,:64]
    
    ldmia       sp!,{r4,pc}
	@ENDP
	
@-----------------------------------------------------------------------------------------------------
@void RV_FASTCALL AddBlock16x16(const U8 *Src, U8 * Src1, U8 *Dst,  U32 Pitch)
@-----------------------------------------------------------------------------------------------------
   .ALIGN 4
AddBlock16x16_Ctx:	@PROC
	stmdb        sp!,{r4,lr}
	
	mov          r4,    #2
AddBlock16x16Loop_Ctx:
    vld1.64		{q0},  [r0,:64], r3
    vld1.64		{q1},  [r0,:64], r3
    vld1.64		{q2},  [r0,:64], r3
    vld1.64		{q3},  [r0,:64], r3
    vld1.64		{q4},  [r0,:64], r3
    vld1.64		{q5},  [r0,:64], r3
    vld1.64		{q6},  [r0,:64], r3
    vld1.64		{q7},  [r0,:64], r3
    
    vld1.64		{q8},  [r1,:64], r3  
    vld1.64		{q9},  [r1,:64], r3
    vld1.64		{q10}, [r1,:64], r3
    vld1.64		{q11}, [r1,:64], r3
    vld1.64		{q12}, [r1,:64], r3
    vld1.64		{q13}, [r1,:64], r3
    vld1.64		{q14}, [r1,:64], r3
    vld1.64		{q15}, [r1,:64], r3
    
    vrhadd.u8   q0,  q0,  q8
    vrhadd.u8   q1,  q1,  q9
    vrhadd.u8   q2,  q2,  q10
    vrhadd.u8   q3,  q3,  q11
    vrhadd.u8   q4,  q4,  q12
    vrhadd.u8   q5,  q5,  q13
    vrhadd.u8   q6,  q6,  q14
    vrhadd.u8   q7,  q7,  q15
    
    vst1.64     {q0},  [r2,:64], r3
    vst1.64     {q1},  [r2,:64], r3
    vst1.64     {q2},  [r2,:64], r3
    vst1.64     {q3},  [r2,:64], r3
    vst1.64     {q4},  [r2,:64], r3
    vst1.64     {q5},  [r2,:64], r3
    vst1.64     {q6},  [r2,:64], r3
    vst1.64     {q7},  [r2,:64], r3
    
    subs        r4, r4, #1 
	bgt         AddBlock16x16Loop_Ctx
    
    ldmia       sp!,{r4,pc}
	@ENDP

@-----------------------------------------------------------------------------------------------------
@void RV_FASTCALL CopyBlock8x8(const U8 *Src, U8 *Dst,  U32 SrcPitch, U32 DstPitch)
@-----------------------------------------------------------------------------------------------------
   .ALIGN 4
CopyBlock8x8_Ctx:	@PROC
		stmdb        sp!,{r4,lr}
    vld1.64		{d0},  [r0,:64], r2
    vld1.64		{d1},  [r0,:64], r2
    vld1.64		{d2},  [r0,:64], r2
    vld1.64		{d3},  [r0,:64], r2
    vld1.64		{d4},  [r0,:64], r2
    vld1.64		{d5},  [r0,:64], r2
    vld1.64		{d6},  [r0,:64], r2
    vld1.64		{d7},  [r0,:64]
   
    vst1.64     {d0},  [r1,:64], r3
    vst1.64     {d1},  [r1,:64], r3
    vst1.64     {d2},  [r1,:64], r3
    vst1.64     {d3},  [r1,:64], r3
    vst1.64     {d4},  [r1,:64], r3
    vst1.64     {d5},  [r1,:64], r3
    vst1.64     {d6},  [r1,:64], r3
    vst1.64     {d7},  [r1,:64]

    ldmia       sp!,{r4,pc}
	@ENDP
	
@-----------------------------------------------------------------------------------------------------
@void RV_FASTCALL AddBlock8x8(const U8 *Src, U8 * Src1, U8 *Dst,  U32 Pitch)
@-----------------------------------------------------------------------------------------------------
   .ALIGN 4
AddBlock8x8_Ctx:	@PROC
		stmdb        sp!,{r4,lr}
    vld1.64		{d0},  [r0,:64], r3
    vld1.64		{d1},  [r0,:64], r3
    vld1.64		{d2},  [r0,:64], r3
    vld1.64		{d3},  [r0,:64], r3
    vld1.64		{d4},  [r0,:64], r3
    vld1.64		{d5},  [r0,:64], r3
    vld1.64		{d6},  [r0,:64], r3
    vld1.64		{d7},  [r0,:64]
    
    vld1.64		{d8},  [r1,:64], r3
    vld1.64		{d9},  [r1,:64], r3
    vld1.64		{d10}, [r1,:64], r3
    vld1.64		{d11}, [r1,:64], r3
    vld1.64		{d12}, [r1,:64], r3
    vld1.64		{d13}, [r1,:64], r3
    vld1.64		{d14}, [r1,:64], r3
    vld1.64		{d15}, [r1,:64]
    
    vrhadd.u8   q0,  q0,  q4
    vrhadd.u8   q1,  q1,  q5
    vrhadd.u8   q2,  q2,  q6
    vrhadd.u8   q3,  q3,  q7
   
    vst1.64     {d0},  [r2,:64], r3
    vst1.64     {d1},  [r2,:64], r3
    vst1.64     {d2},  [r2,:64], r3
    vst1.64     {d3},  [r2,:64], r3
    vst1.64     {d4},  [r2,:64], r3
    vst1.64     {d5},  [r2,:64], r3
    vst1.64     {d6},  [r2,:64], r3
    vst1.64     {d7},  [r2,:64]

    ldmia       sp!,{r4,pc}
	@ENDP
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	void InterpolateDirectLuma_Ctx(U8 *pPrev,U8 *pFutr,I32 uPitch,U8 *dst, U32 uDstPitch, I32 iRatio0, I32 iRatio1)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .ALIGN 4
InterpolateDirectLuma_Ctx:	@PROC
    stmfd       sp!,{r4-r7,lr}
    
    ldr	        r4,[sp,#20]  @uDstPitch
    ldr	        r5,[sp,#24]  @iRatio0
    ldr	        r6,[sp,#28]  @iRatio1
    
    vdup.16     q10,  r5     @iRatio0
    vdup.16     q11,  r6     @iRatio1
    mov         r7,   #8
InterpolateDirectLumaLoop_Ctx:
    vld1.8	    {d0}, [r0], r2
    vld1.8      {d2}, [r1], r2
    vshll.u8     q0,  d0,   #3
    vshll.u8     q1,  d2,   #3
    vqdmulh.s16  q0,  q0,   q11
    vqdmulh.s16  q1,  q1,   q10
    vadd.u16     q0,  q0,   q1
    vqrshrn.u16  d0,  q0,   #2  
    
    vst1.64     {d0},  [r3,:64], r4  
    
    subs        r7, r7, #1 
	bgt         InterpolateDirectLumaLoop_Ctx
       
    ldmfd       sp!,{r4-r7,pc}	
    @ENDP
    
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	void InterpolateDirectChroma_Ctx(U8 *pPrev,U8 *pFutr,I32 uPitch,U8 *dst, U32 uDstPitch, I32 iRatio0, I32 iRatio1)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .ALIGN 4 
InterpolateDirectChroma_Ctx:	@PROC
    stmfd       sp!,{r4-r7,lr}
 
    ldr	        r4,[sp,#20]  @uDstPitch
    ldr	        r5,[sp,#24]  @iRatio0
    ldr	        r6,[sp,#28]  @iRatio1
    
    vdup.16     q10,  r5     @iRatio0
    vdup.16     q11,  r6     @iRatio1
    
    vld1.8	    {d0}, [r0], r2
    vld1.8      {d2}, [r1], r2
    vld1.8	    {d4}, [r0], r2
    vld1.8      {d6}, [r1], r2
    vld1.8	    {d8}, [r0], r2
    vld1.8      {d10}, [r1],r2
    vld1.8	    {d12}, [r0]
    vld1.8      {d14}, [r1]
    vshll.u8     q0,  d0,   #3
    vshll.u8     q1,  d2,   #3
    vshll.u8     q2,  d4,   #3
    vshll.u8     q3,  d6,   #3
    vshll.u8     q4,  d8,   #3
    vshll.u8     q5,  d10,  #3
    vshll.u8     q6,  d12,  #3
    vshll.u8     q7,  d14,  #3
    
    vqdmulh.s16  q0,  q0,   q11
    vqdmulh.s16  q1,  q1,   q10
    vqdmulh.s16  q2,  q2,   q11
    vqdmulh.s16  q3,  q3,   q10
    vqdmulh.s16  q4,  q4,   q11
    vqdmulh.s16  q5,  q5,   q10
    vqdmulh.s16  q6,  q6,   q11
    vqdmulh.s16  q7,  q7,   q10
    
    vadd.u16     q0,  q0,   q1
    vqrshrn.u16  d0,  q0,   #2 
    vadd.u16     q2,  q2,   q3
    vqrshrn.u16  d1,  q2,   #2 
    vadd.u16     q4,  q4,   q5
    vqrshrn.u16  d2,  q4,   #2 
    vadd.u16     q6,  q6,   q7
    vqrshrn.u16  d3,  q6,   #2 
    
    vst1.32      {d0[0]}, [r3,:32], r4 
    vst1.32      {d1[0]}, [r3,:32], r4 
    vst1.32      {d2[0]}, [r3,:32], r4 
    vst1.32      {d3[0]}, [r3,:32]
    
    
    ldmfd       sp!,{r4-r7,pc}	
    @ENDP
    .END
