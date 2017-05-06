@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************

@//************************************************************************
@//
@//	Module Name:
@//
@//	    repeatpadARM.s
@//
@//	Abstract:
@//	
@//	    ARM specific transforms
@//		Optimized assembly routines to implement repeatPad
@//
@//     Custom build with 
@//          armasm $(InputDir)\$(InputName).s $(OutDir)\$(InputName).obj
@//     and
@//          $(OutDir)\$(InputName).obj
@//	
@//	Author:
@//	
@//	    Chuang Gu (chuanggu@microsoft.com) April 12, 2001
@//
@//	Revision History:
@//
@//*************************************************************************
    #include "../../Src/c/voVC1DID.h"
    .include "wmvdec_member_arm.h"
    .include "xplatform_arm_asm.h" 

	@AREA REPEATPAD, CODE, READONLY
	 .text
	 .align 4

	.if WMV_OPT_REPEATPAD_ARM == 1
    .globl  _g_memcpy_ARMV4
    .globl  _g_RepeatRef0Y_LeftRight_ARMV4 
    .globl  _g_RepeatRef0UV_LeftRight_ARMV4 
			
@Void g_RepeatRef0Y_32 (
@	PixelC* ppxlcRef0Y,
@	CoordI  iStart, 
@   CoordI  iEnd,
@	Int     iOldLeftOffet,
@	Bool    fTop, 
@   Bool    fBottom,
@   Int     iWidthY,
@   Int     iWidthYPlusExp,
@   Int     iWidthPrevY
@)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Registers g_RepeatRef0Y_32
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ppxlcRef0Y              .req  r0
iStart                  .req  r1
iEnd                    .req  r2
iWidthYPlusExp          .req  r3
ppxliLeft               .req  r4
ppxliRight              .req  r5
iYCount                 .req  r6
uiLeftValue             .req  r7
uiRightValue            .req  r8
offsetToNextRow         .req  r9

ppxliTopSrc             .req  r0
ppxliBottomSrc          .req  r1
iXCount                 .req  r2
TopSrc0                 .req  r3
TopSrc1                 .req  r4
TopSrc2                 .req  r5
TopSrc3                 .req  r6
BotSrc0                 .req  r7
BotSrc1                 .req  r8
BotSrc2                 .req  r10
BotSrc3                 .req  r11
k                       .req  r12

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Registers g_RepeatRef0UV_16
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ppxlcRef0U              .req  r0
ppxlcRef0V              .req  r1
iStartUV                .req  r2
iEndUV                  .req  r3
ppxliLeftU              .req  r4
ppxliRightU             .req  r5
ppxliLeftV              .req  r6
ppxliRightV             .req  r7
iYYCount                .req  r8
uiLeftValueU            .req  r10
uiRightValueU           .req  r11
uiLeftValueV            .req  r12
uiRightValueV           .req  r14

iWidthUVPlusExp         .req  r8    

ppxliTopSrcU            .req  r0
ppxliTopSrcV            .req  r1
ppxliBottomSrcU         .req  r2
ppxliBottomSrcV         .req  r3
kk                      .req  r4
TopSrc0U                .req  r5
TopSrc1U                .req  r6
BotSrc0U                .req  r7
BotSrc1U                .req  r8
TopSrc0V                .req  r10
TopSrc1V                .req  r11
BotSrc0V                .req  r12
BotSrc1V                .req  r14
iXXCount                .req  r14


    
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@Void_WMV g_RepeatRef0Y_LeftRight ( 
@	const U8_WMV* pLeft, 
@	const U8_WMV* pRight, 
@	U8_WMV* pDst, 
@	I32_WMV iWidthPrevY,	
@	I32_WMV iWidthYPlusExp,  
@	I32_WMV iRowNum )    
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	WMV_LEAF_ENTRY g_RepeatRef0Y_LeftRight_ARMV4

@r0: pLeft, 
@r1: pRight, 
@r2: pDst, 
@r3: iWidthPrevY,	
@r4: pDstPlusWidth
@r5: iRowNum
@r6,r7: uipadValueLeft
@r8,r9: uipadValueRight
@r14: temp

.set StackOffset_RegSaving		, 28
.set StackOffset_iWidthYPlusExp	, StackOffset_RegSaving + 0
.set StackOffset_iRowNum			, StackOffset_RegSaving + 4

	stmdb     sp!, {r4-r9, r14}

    .if PLD_ENABLE == 1
	pld		[r0, r3]
	pld		[r1, r3]
    .endif
	ldr		r4, [sp, #StackOffset_iWidthYPlusExp]
	ldr		r5, [sp, #StackOffset_iRowNum]
    .if PLD_ENABLE == 1
	pld		[r0, r3, lsl #1]
	pld		[r1, r3, lsl #1]
    .endif
	add		r4, r2, r4
	
Y_LR_Loop:

	ldrb	r6, [r0], r3
	ldrb	r8, [r1], r3
    .if PLD_ENABLE == 1
	pld		[r0, r3, lsl #1]
	pld		[r1, r3, lsl #1]
    .endif
	
	orr		r6, r6, r6, lsl #8
	orr		r8, r8, r8, lsl #8
	orr		r6, r6, r6, lsl #16
	orr		r8, r8, r8, lsl #16
		
	str		r6, [r2, #4]
	str		r6, [r2, #8]
	str		r6, [r2, #12]
	str		r6, [r2, #16]
	str		r6, [r2, #20]
	str		r6, [r2, #24]
	str		r6, [r2, #28]
	str		r6, [r2], r3
	str		r8, [r4, #4]
	str		r8, [r4, #8]
	str		r8, [r4, #12]
	str		r8, [r4, #16]
	str		r8, [r4, #20]
	str		r8, [r4, #24]
	str		r8, [r4, #28]
	str		r8, [r4], r3
	
	subs	r5, r5, #1
	bne		Y_LR_Loop

	ldmia     sp!, {r4-r9,pc}    
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@Void_WMV g_RepeatRef0UV_LeftRight ( 
@	const U8_WMV* pLeftU, 
@	const U8_WMV* pRightU, 
@	const U8_WMV* pLeftV, 
@	const U8_WMV* pRightV, 
@	U8_WMV* pDstU, 
@	U8_WMV* pDstV, 
@	I32_WMV iWidthPrevUV,	
@	I32_WMV iWidthUVPlusExp,  
@	I32_WMV iRowNum)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	WMV_LEAF_ENTRY g_RepeatRef0UV_LeftRight_ARMV4

@r0: pLeftU
@r1: pRightU
@r2: pLeftV
@r3: pRightV	
@r4: pDstU
@r5: pDstV
@r6,r7: uipadValueLeft
@r8,r9: uipadValueRight
@r10: pDstPlusWidth
@r11: iWidthPrevUV
@r12: iWidthUVPlusExp
@r14: iRowNum

.set StackOffset_RegSaving_UV	,		36
.set StackOffset_pDstU			,		StackOffset_RegSaving_UV + 0
.set StackOffset_pDstV			,		StackOffset_RegSaving_UV + 4
.set StackOffset_iWidthPrevUV	,		StackOffset_RegSaving_UV + 8
.set StackOffset_iWidthUVPlusExp	,		StackOffset_RegSaving_UV + 12
.set StackOffset_iRowNum_UV 		,		StackOffset_RegSaving_UV + 16
.set StackOffset_iRowNum_Back	,		-4

	stmdb     sp!, {r4-r11, r14}
	
	ldr		r12, [sp, #StackOffset_iWidthUVPlusExp]
	ldr		r11, [sp, #StackOffset_iWidthPrevUV]
	ldr		r14, [sp, #StackOffset_iRowNum_UV]
	ldr		r4 , [sp, #StackOffset_pDstU]
	ldr		r5 , [sp, #StackOffset_pDstV]
	
    .if PLD_ENABLE == 1
	pld		[r0, r11]
	pld		[r1, r11]
	pld		[r0, r11, lsl #1]
	pld		[r1, r11, lsl #1]
    .endif
	
	add		r10, r4, r12   @ pDstUPlusWidth
	str		r14, [sp, #StackOffset_iRowNum_Back]
U_LR_Loop:

	ldrb	r6, [r0], r11
	ldrb	r8, [r1], r11
    .if PLD_ENABLE == 1
	pld		[r0, r11, lsl #1]
	pld		[r1, r11, lsl #1]
    .endif
	
	orr		r6, r6, r6, lsl #8
	orr		r8, r8, r8, lsl #8
	orr		r6, r6, r6, lsl #16
	orr		r8, r8, r8, lsl #16
	
	str		r6, [r4, #4]
	str		r6, [r4, #8]
	str		r6, [r4, #12]
	str		r6, [r4], r11
	str		r8, [r10, #4]
	str		r8, [r10, #8]
	str		r8, [r10, #12]
	str		r8, [r10], r11
	
	subs	r14, r14, #1
	bne		U_LR_Loop

    .if PLD_ENABLE == 1
	pld		[r2, r11]
	pld		[r3, r11]
	pld		[r2, r11, lsl #1]
	pld		[r3, r11, lsl #1]
    .endif
	ldr		r14, [sp, #StackOffset_iRowNum_Back]
	add		r10, r5, r12   @ pDstVPlusWidth
V_LR_Loop:

	ldrb	r6, [r2], r11
	ldrb	r8, [r3], r11
    .if PLD_ENABLE == 1
	pld		[r2, r11, lsl #1]
	pld		[r3, r11, lsl #1]
    .endif
	
	orr		r6, r6, r6, lsl #8
	orr		r8, r8, r8, lsl #8
	orr		r6, r6, r6, lsl #16
	orr		r8, r8, r8, lsl #16
	
	str		r6, [r5, #4]
	str		r6, [r5, #8]
	str		r6, [r5, #12]
	str		r6, [r5], r11
	str		r8, [r10, #4]
	str		r8, [r10, #8]
	str		r8, [r10, #12]
	str		r8, [r10], r11
		
	subs	r14, r14, #1
	bne		V_LR_Loop
	
	ldmia     sp!, {r4-r11,pc}    


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@Void_WMV g_memcpy(U8_WMV* ppxliLeftTop, U8_WMV* ppxliSrc, I32_WMV iWidthPrevY, I32_WMV iTrueWidth, I32_WMV iVertPad)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	WMV_LEAF_ENTRY g_memcpy_ARMV4

.set	iVertPad_offset,	24

    .if PLD_ENABLE == 1
     pld	[r1, #16]
     .endif
     stmdb  sp!, {r4-r8, r14}

     ldr	r8, [sp, #iVertPad_offset]
     sub	r2, r2, #16
     mov	r14, r0

memcpy_load:
     ldr	r4, [r1], #4
     ldr	r5, [r1], #4
     ldr	r6, [r1], #4
     ldr	r7, [r1], #4
    .if PLD_ENABLE == 1
     pld	[r1, #16]
     .endif
     mov	r12, r8
    
memcpy_store:
     str	r4, [r0], #4
     str	r5, [r0], #4
     str	r6, [r0], #4
     str	r7, [r0], #4
     subs	r12, r12, #1
     add	r0, r0, r2      
     bne	memcpy_store
     
     add	r14, r14, #16
     sub	r3, r3, #16
     mov	r0, r14     
     cmp	r3, #16
     bge	memcpy_load         
     
     cmp	r3, #0
     ldmeqia sp!, {r4-r8,pc}    
                       
     ldr	r4, [r1]
     ldr	r5, [r1, #4]
     mov	r12, r8
     add	r2, r2, #16
     
memcpy_store_last:
     subs	r12, r12, #1
     str	r5, [r0, #4]
     str	r4, [r0], r2
     bne	memcpy_store_last
                     
     ldmia sp!, {r4-r8,pc}    

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	.endif @WMV_OPT_REPEATPAD_ARM

    .globl _end_repeatpad_arm
end_repeatpad_arm:
    nop             @ mark location where previous function ends for cache analysis


	@@.end	
	
	
