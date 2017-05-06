@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************

#include "../../voASMPort.h"

    @AREA    |.text|, CODE, READONLY
    .text
	   .align 4
	.globl	VOASMFUNCNAME(Fill_Edge_Y_Ctx)
	.globl	VOASMFUNCNAME(Fill_Edge_UV_Ctx)	
@------------------------------------------------------------------------------------------------------------------------------------
@void RV_FASTCALL VOASMFUNCNAME(Fill_Edge_Y_Ctx)(U8 *Y,const U32 ExWidth,const U32 ExHeight,U32 width, U32 height)
@-------------------------------------------------------------------------------------------------------------------------------------
   .align 4
VOASMFUNCNAME(Fill_Edge_Y_Ctx):	@PROC
	stmfd       sp!,{r4-r12,lr}
	
	ldr	        r4,[sp,#40]       @height
	mov         r8,  #16   
    
    mov         r12, r0
	mla         r0,  r4, r1, r0   @dst = image->m_pYPlane + height * ExWidth
	sub         r10, r0, r1       @src = image->m_pYPlane + (height - 1) * ExWidth@
	mov         r11, r0
	
	mov         r9,  r3, lsr #4   @width >> 4
FillYLoopOne:
	vld1.64		{q0},  [r10,:64], r8
	
	vst1.64     {q0},  [r0,:64], r1        @0
	vst1.64     {q0},  [r0,:64], r1        @1
	vst1.64     {q0},  [r0,:64], r1        @2
	vst1.64     {q0},  [r0,:64], r1        @3
	vst1.64     {q0},  [r0,:64], r1        @4
	vst1.64     {q0},  [r0,:64], r1        @5
	vst1.64     {q0},  [r0,:64], r1        @6
	vst1.64     {q0},  [r0,:64], r1        @7
	
	vst1.64     {q0},  [r0,:64], r1        @8
	vst1.64     {q0},  [r0,:64], r1        @9
	vst1.64     {q0},  [r0,:64], r1        @10
	vst1.64     {q0},  [r0,:64], r1        @11
	vst1.64     {q0},  [r0,:64], r1        @12
	vst1.64     {q0},  [r0,:64], r1        @13
	vst1.64     {q0},  [r0,:64], r1        @14
	vst1.64     {q0},  [r0,:64]		     @15
	
	add         r11,   r11,   #16
	mov         r0,    r11
	
	subs        r9, r9, #1 
	bgt         FillYLoopOne
	
	mov         r0, r12                      @ src = image->m_pYPlane
	sub         r11,r0, #16                  @ dst  = image->m_pYPlane - YUV_Y_PADDING
	add         r9, r0, r3                   @ src + width
	sub         r10,r9, #1                   @ src + width - 1
	add         r7, r4, #16					 @ height + YUV_Y_PADDING 
	lsr         r7, r7, #3					 @ width >> 3    
FillYLoopTwo:
	vld1.8      {d0[]}, [r0],  r1
	vmov        d1,  d0
	vld1.8      {d16[]},[r10], r1
	vmov        d17, d16
	vld1.8      {d2[]}, [r0],  r1
	vmov        d3,  d2
	vld1.8      {d18[]},[r10], r1
	vmov        d19, d18
	vld1.8      {d4[]}, [r0],  r1
	vmov        d5,  d4
	vld1.8      {d20[]},[r10], r1
	vmov        d21, d20
	vld1.8      {d6[]}, [r0],  r1
	vmov        d7,  d6
	vld1.8      {d22[]},[r10], r1
	vmov        d23, d22
	vld1.8      {d8[]}, [r0],  r1
	vmov        d9,  d8
	vld1.8      {d24[]},[r10], r1
	vmov        d25, d24
	vld1.8      {d10[]},[r0],  r1
	vmov        d11, d10
	vld1.8      {d26[]},[r10], r1
	vmov        d27, d26
	vld1.8      {d12[]},[r0],  r1
	vmov        d13, d12
	vld1.8      {d28[]},[r10], r1
	vmov        d29, d28
	vld1.8      {d14[]},[r0],  r1
	vmov        d15, d14
	vld1.8      {d30[]},[r10], r1
	vmov        d31, d30
	
	vst1.64     {q0},   [r11,:64], r1
	vst1.64     {q8},   [r9, :64], r1 
	vst1.64     {q1},   [r11,:64], r1
	vst1.64     {q9},   [r9, :64], r1
	vst1.64     {q2},   [r11,:64], r1
	vst1.64     {q10},  [r9, :64], r1 
	vst1.64     {q3},   [r11,:64], r1
	vst1.64     {q11},  [r9, :64], r1
	vst1.64     {q4},   [r11,:64], r1
	vst1.64     {q12},  [r9, :64], r1 
	vst1.64     {q5},   [r11,:64], r1
	vst1.64     {q13},  [r9, :64], r1
	vst1.64     {q6},   [r11,:64], r1
	vst1.64     {q14},  [r9, :64], r1
	vst1.64     {q7},   [r11,:64], r1
	vst1.64     {q15},  [r9, :64], r1 
	
	subs        r7, r7, #1 
	bgt         FillYLoopTwo

	mov         r0, r12                      
	sub         r11,r0, #16                   @ src  = image->m_pYPlane - YUV_Y_PADDING
	lsl         r9, r1, #4					  @ ExWidth*16
	sub         r10,r11,r9                    @ dst  = src - ExWidth*16
	mov         r9, r1, lsr #4				  @ ExWidth >> 4
	mov         r0, r10	
FillYLoopThree:
	vld1.64		{q0},  [r11,:64], r8
	
	vst1.64     {q0},  [r10,:64], r1        @0
	vst1.64     {q0},  [r10,:64], r1        @1
	vst1.64     {q0},  [r10,:64], r1        @2
	vst1.64     {q0},  [r10,:64], r1        @3
	vst1.64     {q0},  [r10,:64], r1        @4
	vst1.64     {q0},  [r10,:64], r1        @5
	vst1.64     {q0},  [r10,:64], r1        @6
	vst1.64     {q0},  [r10,:64], r1        @7
	
	vst1.64     {q0},  [r10,:64], r1        @8
	vst1.64     {q0},  [r10,:64], r1        @9
	vst1.64     {q0},  [r10,:64], r1        @10
	vst1.64     {q0},  [r10,:64], r1        @11
	vst1.64     {q0},  [r10,:64], r1        @12
	vst1.64     {q0},  [r10,:64], r1        @13
	vst1.64     {q0},  [r10,:64], r1        @14
	vst1.64     {q0},  [r10,:64]		      @15
	
	add         r0,    r0,   #16
	mov         r10,   r0
	
	subs        r9, r9, #1 
	bgt         FillYLoopThree		
	
	ldmfd       sp!,{r4-r12,pc}	
    @ENDP
@------------------------------------------------------------------------------------------------------------------------------------
@void RV_FASTCALL VOASMFUNCNAME(Fill_Edge_UV_Ctx)(U8 *Y,const U32 ExWidth,const U32 ExHeight,U32 width, U32 height)
@-------------------------------------------------------------------------------------------------------------------------------------
   .align 4
VOASMFUNCNAME(Fill_Edge_UV_Ctx):	@PROC

	stmfd       sp!,{r4-r12,lr}
	
	ldr	        r4,[sp,#40]       @height
	mov         r8,  #8   
    
    mov         r12, r0
	mla         r0,  r4, r1, r0   @dst = image->m_pUPlane + height * ExWidth
	sub         r10, r0, r1       @src = image->m_pUPlane + (height - 1) * ExWidth@
	mov         r11, r0
	
	mov         r9,  r3, lsr #3   @width >> 3
FillUVLoopOne:
	vld1.64		{d0},  [r10,:64], r8
	
	vst1.64     {d0},  [r0,:64], r1        @0
	vst1.64     {d0},  [r0,:64], r1        @1
	vst1.64     {d0},  [r0,:64], r1        @2
	vst1.64     {d0},  [r0,:64], r1        @3
	vst1.64     {d0},  [r0,:64], r1        @4
	vst1.64     {d0},  [r0,:64], r1        @5
	vst1.64     {d0},  [r0,:64], r1        @6
	vst1.64     {d0},  [r0,:64]	        @7
	
	add         r11,   r11,   #8
	mov         r0,    r11
	
	subs        r9, r9, #1 
	bgt         FillUVLoopOne
	
	mov         r0, r12                      @ src = image->m_pUPlane
	sub         r11,r0, #8                  @ dst  = image->m_pUPlane - YUV_UV_PADDING
	add         r9, r0, r3                   @ src + width
	sub         r10,r9, #1                   @ src + width - 1
	add         r7, r4, #8					 @ height + YUV_UV_PADDING 
	lsr         r7, r7, #3					 @ width >> 3                  
FillUVLoopTwo:
	vld1.8      {d0[]}, [r0],  r1
	vld1.8      {d8[]}, [r10], r1
	vld1.8      {d1[]}, [r0],  r1
	vld1.8      {d9[]}, [r10], r1 
	vld1.8      {d2[]}, [r0],  r1
	vld1.8      {d10[]},[r10], r1
	vld1.8      {d3[]}, [r0],  r1
	vld1.8      {d11[]},[r10], r1 
	vld1.8      {d4[]}, [r0],  r1
	vld1.8      {d12[]},[r10], r1
	vld1.8      {d5[]}, [r0],  r1
	vld1.8      {d13[]},[r10], r1 
	vld1.8      {d6[]}, [r0],  r1
	vld1.8      {d14[]},[r10], r1
	vld1.8      {d7[]}, [r0],  r1
	vld1.8      {d15[]},[r10], r1 
		    
	vst1.64     {d0},   [r11,:64], r1
	vst1.64     {d8},   [r9, :64], r1
	vst1.64     {d1},   [r11,:64], r1
	vst1.64     {d9},   [r9, :64], r1
	vst1.64     {d2},   [r11,:64], r1
	vst1.64     {d10},  [r9, :64], r1
	vst1.64     {d3},   [r11,:64], r1
	vst1.64     {d11},  [r9, :64], r1 
	vst1.64     {d4},   [r11,:64], r1
	vst1.64     {d12},  [r9, :64], r1
	vst1.64     {d5},   [r11,:64], r1
	vst1.64     {d13},  [r9, :64], r1
	vst1.64     {d6},   [r11,:64], r1
	vst1.64     {d14},  [r9, :64], r1
	vst1.64     {d7},   [r11,:64], r1
	vst1.64     {d15},  [r9, :64], r1 
	
	subs        r7, r7, #1 
	bgt         FillUVLoopTwo

	mov         r0, r12                      
	sub         r11,r0, #8                    @ src  = image->m_pUPlane - YUV_UV_PADDING
	lsl         r9, r1, #3					  @ ExWidth*8
	sub         r10,r11,r9                    @ dst  = src - ExWidth*8
	ldr	        r9,[sp,#44]       			  @ edged_width2
	lsr         r9, r9, #3                    @ edged_width2>>3
	mov         r0, r10	
FillUVLoopThree:
	vld1.64		{d0},  [r11,:64], r8
	
	vst1.64     {d0},  [r10,:64], r1        @0
	vst1.64     {d0},  [r10,:64], r1        @1
	vst1.64     {d0},  [r10,:64], r1        @2
	vst1.64     {d0},  [r10,:64], r1        @3
	vst1.64     {d0},  [r10,:64], r1        @4
	vst1.64     {d0},  [r10,:64], r1        @5
	vst1.64     {d0},  [r10,:64], r1        @6
	vst1.64     {d0},  [r10,:64]		     @7
	
	add         r0,    r0,   #8
	mov         r10,   r0
	
	subs        r9, r9, #1 
	bgt         FillUVLoopThree		
	
	ldmfd       sp!,{r4-r12,pc}	
    @ENDP
    @END