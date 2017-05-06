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

    #include "../c/voWMVDecID.h"
    .include "wmvdec_member_arm.h"
    .include "xplatform_arm_asm.h" 

	@AREA REPEATPAD, CODE, READONLY
	 .text
	 .align 4

	.if WMV_OPT_REPEATPAD_ARM == 1

    .globl  _g_RepeatRef0Y_32                  
    .globl  _g_RepeatRef0UV_16                    
    .globl  _g_RepeatRef0Y_24                  
    .globl  _g_RepeatRef0UV_12  
    .globl  _g_memcpy  
    .globl  _g_RepeatRef0Y_LeftRight 
    .globl  _g_RepeatRef0UV_LeftRight 
			
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

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    WMV_LEAF_ENTRY g_RepeatRef0Y_32
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    stmfd	sp!, {r4 - r12, r14}
    FRAME_PROFILE_COUNT
    FUNCTION_PROFILE_START DECODEREPEATPAD_PROFILE
    
    ldr     offsetToNextRow, [sp, #56]  @ get  iWidthPrevY
    add     ppxliLeft, ppxlcRef0Y, offsetToNextRow, lsl #5
    ldr     iWidthYPlusExp, [sp, #52]  
    add     ppxliRight, ppxliLeft, iWidthYPlusExp
    sub     iYCount, iEnd, iStart
LoopLeftRightY:
        ldrb    uiLeftValue, [ppxliLeft, #32]
        ldrb    uiRightValue, [ppxliRight, #-1]
        orr     uiLeftValue, uiLeftValue, uiLeftValue, lsl #8
        orr     uiLeftValue, uiLeftValue, uiLeftValue, lsl #16
        orr     uiRightValue, uiRightValue, uiRightValue, lsl #8
        orr     uiRightValue, uiRightValue, uiRightValue, lsl #16
        str     uiLeftValue, [ppxliLeft]
        str     uiLeftValue, [ppxliLeft, #4]
        str     uiLeftValue, [ppxliLeft, #8]
        str     uiLeftValue, [ppxliLeft, #12]
        str     uiLeftValue, [ppxliLeft, #16]
        str     uiLeftValue, [ppxliLeft, #20]
        str     uiLeftValue, [ppxliLeft, #24]
        str     uiLeftValue, [ppxliLeft, #28]
        str     uiRightValue, [ppxliRight]
        str     uiRightValue, [ppxliRight, #4]
        str     uiRightValue, [ppxliRight, #8]
        str     uiRightValue, [ppxliRight, #12]
        str     uiRightValue, [ppxliRight, #16]
        str     uiRightValue, [ppxliRight, #20]
        str     uiRightValue, [ppxliRight, #24]
        str     uiRightValue, [ppxliRight, #28]
        add     ppxliLeft, ppxliLeft, offsetToNextRow
        add     ppxliRight, ppxliRight, offsetToNextRow
        sub     iYCount, iYCount, #1
        cmp     iYCount, #0
        bne     LoopLeftRightY

    add     ppxliTopSrc, ppxlcRef0Y, offsetToNextRow, lsl #5
    sub     ppxliBottomSrc, ppxliLeft, offsetToNextRow
    mov     iXCount, offsetToNextRow, asr #4
LoopTopBotHorY:
        ldmia   ppxliTopSrc, {TopSrc0, TopSrc1, TopSrc2, TopSrc3}  
        ldmia   ppxliBottomSrc, {BotSrc0, BotSrc1, BotSrc2, BotSrc3}
        sub     ppxliTopSrc, ppxliTopSrc, offsetToNextRow, lsl #5
        add     ppxliBottomSrc, ppxliBottomSrc, offsetToNextRow, lsl #5
        mov     k, #32
LoopTopBotVerY:
            stmia   ppxliTopSrc, {TopSrc0, TopSrc1, TopSrc2, TopSrc3}
            stmia   ppxliBottomSrc, {BotSrc0, BotSrc1, BotSrc2, BotSrc3}
            add     ppxliTopSrc, ppxliTopSrc, offsetToNextRow
            sub     ppxliBottomSrc, ppxliBottomSrc, offsetToNextRow
            sub     k, k, #1
            cmp     k, #0
            bne     LoopTopBotVerY
        add     ppxliTopSrc, ppxliTopSrc, #16
        add     ppxliBottomSrc, ppxliBottomSrc, #16
        sub     iXCount, iXCount, #1
        cmp     iXCount, #0
        bne     LoopTopBotHorY

    FUNCTION_PROFILE_STOP
    ldmfd   sp!, {r4 - r12, PC}
    WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@Void g_RepeatRef0UV_16 (
@	PixelC* ppxlcRef0U,
@	PixelC* ppxlcRef0V,
@	Coord I  iStart, CoordI iEnd,
@	Int     iOldLeftOffet,
@	Bool    fTop, 
@   Bool    fBottom,
@   Int     iWidthUV,
@   Int     iWidthUVPlusExp,
@   Int     iWidthPrevUV
@)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    WMV_LEAF_ENTRY g_RepeatRef0UV_16
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    stmfd	sp!, {r4 - r12, r14}
    FRAME_PROFILE_COUNT
    FUNCTION_PROFILE_START DECODEREPEATPAD_PROFILE

    ldr     offsetToNextRow, [sp, #60]  @ get  iWidthPrevUV
    add     ppxliLeftU, ppxlcRef0U, offsetToNextRow, lsl #4
    ldr     iWidthUVPlusExp, [sp, #56]
    add     ppxliRightU, ppxliLeftU, iWidthUVPlusExp  
    add     ppxliLeftV, ppxlcRef0V, offsetToNextRow, lsl #4
    add     ppxliRightV, ppxliLeftV, iWidthUVPlusExp
    sub     iYYCount, iEndUV, iStartUV@
LoopLeftRightUV:
        ldrb    uiLeftValueU, [ppxliLeftU, #16]
        ldrb    uiLeftValueV, [ppxliLeftV, #16]
        orr     uiLeftValueU, uiLeftValueU, uiLeftValueU, lsl #8
        orr     uiLeftValueU, uiLeftValueU, uiLeftValueU, lsl #16
        orr     uiLeftValueV, uiLeftValueV, uiLeftValueV, lsl #8
        orr     uiLeftValueV, uiLeftValueV, uiLeftValueV, lsl #16
        str     uiLeftValueU, [ppxliLeftU]
        str     uiLeftValueU, [ppxliLeftU, #4]
        str     uiLeftValueU, [ppxliLeftU, #8]
        str     uiLeftValueU, [ppxliLeftU, #12]
        str     uiLeftValueV, [ppxliLeftV]
        str     uiLeftValueV, [ppxliLeftV, #4]
        str     uiLeftValueV, [ppxliLeftV, #8]
        str     uiLeftValueV, [ppxliLeftV, #12]
        ldrb    uiRightValueU, [ppxliRightU, #-1]
        ldrb    uiRightValueV, [ppxliRightV, #-1]
        orr     uiRightValueU, uiRightValueU, uiRightValueU, lsl #8
        orr     uiRightValueU, uiRightValueU, uiRightValueU, lsl #16
        orr     uiRightValueV, uiRightValueV, uiRightValueV, lsl #8
        orr     uiRightValueV, uiRightValueV, uiRightValueV, lsl #16
        str     uiRightValueU, [ppxliRightU]
        str     uiRightValueU, [ppxliRightU, #4]
        str     uiRightValueU, [ppxliRightU, #8]
        str     uiRightValueU, [ppxliRightU, #12]
        str     uiRightValueV, [ppxliRightV]
        str     uiRightValueV, [ppxliRightV, #4]
        str     uiRightValueV, [ppxliRightV, #8]
        str     uiRightValueV, [ppxliRightV, #12]
        add     ppxliLeftU, ppxliLeftU, offsetToNextRow
        add     ppxliRightU, ppxliRightU, offsetToNextRow
        add     ppxliLeftV, ppxliLeftV, offsetToNextRow
        add     ppxliRightV, ppxliRightV, offsetToNextRow
        sub     iYYCount, iYYCount, #1
        cmp     iYYCount, #0
        bne     LoopLeftRightUV

    add     ppxliTopSrcU, ppxlcRef0U, offsetToNextRow, lsl #4
    add     ppxliTopSrcV, ppxlcRef0V, offsetToNextRow, lsl #4
    sub     ppxliBottomSrcU, ppxliLeftU, offsetToNextRow
    sub     ppxliBottomSrcV, ppxliLeftV, offsetToNextRow
    mov     iXXCount, offsetToNextRow, asr #3
    str     iXXCount, [sp, #40]
LoopTopBotUVHor:
        ldmia   ppxliTopSrcU, {TopSrc0U, TopSrc1U}
        ldmia   ppxliTopSrcV, {TopSrc0V, TopSrc1V}
        ldmia   ppxliBottomSrcU, {BotSrc0U, BotSrc1U}
        ldmia   ppxliBottomSrcV, {BotSrc0V, BotSrc1V}
        sub     ppxliTopSrcU, ppxliTopSrcU, offsetToNextRow, lsl #4
        sub     ppxliTopSrcV, ppxliTopSrcV, offsetToNextRow, lsl #4
        add     ppxliBottomSrcU, ppxliBottomSrcU, offsetToNextRow, lsl #4
        add     ppxliBottomSrcV, ppxliBottomSrcV, offsetToNextRow, lsl #4
        mov     kk, #16
LoopTopBotUVVer:
            stmia   ppxliTopSrcU, {TopSrc0U, TopSrc1U}
            stmia   ppxliTopSrcV, {TopSrc0V, TopSrc1V}
            stmia   ppxliBottomSrcU, {BotSrc0U, BotSrc1U}
            stmia   ppxliBottomSrcV, {BotSrc0V, BotSrc1V}
            add     ppxliTopSrcU, ppxliTopSrcU, offsetToNextRow
            add     ppxliTopSrcV, ppxliTopSrcV, offsetToNextRow
            sub     ppxliBottomSrcU, ppxliBottomSrcU, offsetToNextRow
            sub     ppxliBottomSrcV, ppxliBottomSrcV, offsetToNextRow
            sub     kk, kk, #1
            cmp     kk, #0
            bne     LoopTopBotUVVer
        ldr     iXXCount, [sp, #40]
        add     ppxliTopSrcU, ppxliTopSrcU, #8
        add     ppxliTopSrcV, ppxliTopSrcV, #8
        add     ppxliBottomSrcU, ppxliBottomSrcU, #8
        add     ppxliBottomSrcV, ppxliBottomSrcV, #8
        sub     iXXCount, iXXCount, #1
        str     iXXCount, [sp, #40]
        cmp     iXXCount, #0
        bne     LoopTopBotUVHor

    FUNCTION_PROFILE_STOP
    ldmfd   sp!, {r4 - r12, PC}
    WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    WMV_LEAF_ENTRY g_RepeatRef0Y_24
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    nop
    stmfd	sp!, {r4 - r12, r14}
    FRAME_PROFILE_COUNT
    FUNCTION_PROFILE_START DECODEREPEATPAD_PROFILE
    
    ldr     offsetToNextRow, [sp, #56]  @ get  iWidthPrevY
    ldr     iWidthYPlusExp, [sp, #52]
    add     ppxliLeft, ppxlcRef0Y, offsetToNextRow, lsl #4
    sub     iYCount, iEnd, iStart
    add     ppxliLeft, ppxliLeft, offsetToNextRow, lsl #3   @24 lines
    add     ppxliRight, ppxliLeft, iWidthYPlusExp
    
LoopLeftRightY_24:

        ldrb    uiLeftValue, [ppxliLeft, #24]
        ldrb    uiRightValue, [ppxliRight, #-1]
        subs    iYCount, iYCount, #1
        .if PLD_ENABLE == 1
        pld     [ppxliLeft, offsetToNextRow, lsl #4]
        pld     [ppxliRight, offsetToNextRow, lsl #4]
        .endif
        orr     uiLeftValue, uiLeftValue, uiLeftValue, lsl #8
        orr     uiRightValue, uiRightValue, uiRightValue, lsl #8
        orr     uiLeftValue, uiLeftValue, uiLeftValue, lsl #16
        orr     uiRightValue, uiRightValue, uiRightValue, lsl #16
        str     uiLeftValue, [ppxliLeft]
        str     uiLeftValue, [ppxliLeft, #4]
        str     uiLeftValue, [ppxliLeft, #8]
        str     uiLeftValue, [ppxliLeft, #12]
        str     uiLeftValue, [ppxliLeft, #16]
        str     uiLeftValue, [ppxliLeft, #20]
        add     ppxliLeft, ppxliLeft, offsetToNextRow
        str     uiRightValue, [ppxliRight]
        str     uiRightValue, [ppxliRight, #4]
        str     uiRightValue, [ppxliRight, #8]
        str     uiRightValue, [ppxliRight, #12]
        str     uiRightValue, [ppxliRight, #16]
        str     uiRightValue, [ppxliRight, #20]
        add     ppxliRight, ppxliRight, offsetToNextRow
        bne     LoopLeftRightY_24

    add     ppxliTopSrc, ppxlcRef0Y, offsetToNextRow, lsl #4
    mov     iXCount, offsetToNextRow, asr #4
    add     ppxliTopSrc, ppxliTopSrc, offsetToNextRow, lsl #3 @ 24 lines
    sub     ppxliBottomSrc, ppxliLeft, offsetToNextRow
LoopTopBotHorY_24:
        ldmia   ppxliTopSrc, {TopSrc0, TopSrc1, TopSrc2, TopSrc3} 
        sub     ppxliTopSrc, ppxliTopSrc, offsetToNextRow, lsl #4           @ 24 lines
        ldmia   ppxliBottomSrc, {BotSrc0, BotSrc1, BotSrc2, BotSrc3}
        add     ppxliBottomSrc, ppxliBottomSrc, offsetToNextRow, lsl #4     @ 24 lines
        sub     ppxliTopSrc, ppxliTopSrc, offsetToNextRow, lsl #3
        add     ppxliBottomSrc, ppxliBottomSrc, offsetToNextRow, lsl #3
        mov     k, #24  @24 lines
LoopTopBotVerY_24:
            stmia   ppxliTopSrc, {TopSrc0, TopSrc1, TopSrc2, TopSrc3}
            add     ppxliTopSrc, ppxliTopSrc, offsetToNextRow
            stmia   ppxliBottomSrc, {BotSrc0, BotSrc1, BotSrc2, BotSrc3}
            sub     ppxliBottomSrc, ppxliBottomSrc, offsetToNextRow
            subs    k, k, #1
            bne     LoopTopBotVerY_24
        add     ppxliTopSrc, ppxliTopSrc, #16
        add     ppxliBottomSrc, ppxliBottomSrc, #16
        subs    iXCount, iXCount, #1
        bne     LoopTopBotHorY_24

    FUNCTION_PROFILE_STOP
    ldmfd   sp!, {r4 - r12, PC}
    WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    WMV_LEAF_ENTRY g_RepeatRef0UV_12
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    stmfd	sp!, {r4 - r12, r14}
    FRAME_PROFILE_COUNT
    FUNCTION_PROFILE_START DECODEREPEATPAD_PROFILE

    ldr     offsetToNextRow, [sp, #60]  @ get  iWidthPrevUV
    ldr     iWidthUVPlusExp, [sp, #56]
    add     ppxliLeftU, ppxlcRef0U, offsetToNextRow, lsl #3     @ 12 lines
    add     ppxliLeftV, ppxlcRef0V, offsetToNextRow, lsl #3     @ 12 lines
    add     ppxliLeftU, ppxliLeftU, offsetToNextRow, lsl #2
    add     ppxliLeftV, ppxliLeftV, offsetToNextRow, lsl #2
    add     ppxliRightU, ppxliLeftU, iWidthUVPlusExp
    add     ppxliRightV, ppxliLeftV, iWidthUVPlusExp
    sub     iYYCount, iEndUV, iStartUV@
LoopLeftRightUV_12:
        ldrb    uiLeftValueU, [ppxliLeftU, #12]
        ldrb    uiLeftValueV, [ppxliLeftV, #12]
        ldrb    uiRightValueU, [ppxliRightU, #-1]
        ldrb    uiRightValueV, [ppxliRightV, #-1]
        orr     uiLeftValueU, uiLeftValueU, uiLeftValueU, lsl #8
        orr     uiLeftValueV, uiLeftValueV, uiLeftValueV, lsl #8
        orr     uiLeftValueU, uiLeftValueU, uiLeftValueU, lsl #16
        orr     uiLeftValueV, uiLeftValueV, uiLeftValueV, lsl #16
        str     uiLeftValueU, [ppxliLeftU]
        str     uiLeftValueU, [ppxliLeftU, #4]
        str     uiLeftValueU, [ppxliLeftU, #8]
        str     uiLeftValueV, [ppxliLeftV]
        str     uiLeftValueV, [ppxliLeftV, #4]
        str     uiLeftValueV, [ppxliLeftV, #8]
        orr     uiRightValueU, uiRightValueU, uiRightValueU, lsl #8
        orr     uiRightValueV, uiRightValueV, uiRightValueV, lsl #8
        orr     uiRightValueU, uiRightValueU, uiRightValueU, lsl #16
        orr     uiRightValueV, uiRightValueV, uiRightValueV, lsl #16
        str     uiRightValueU, [ppxliRightU]
        str     uiRightValueU, [ppxliRightU, #4]
        str     uiRightValueU, [ppxliRightU, #8]
        str     uiRightValueV, [ppxliRightV]
        str     uiRightValueV, [ppxliRightV, #4]
        str     uiRightValueV, [ppxliRightV, #8]
        add     ppxliLeftU, ppxliLeftU, offsetToNextRow
        add     ppxliRightU, ppxliRightU, offsetToNextRow
        add     ppxliLeftV, ppxliLeftV, offsetToNextRow
        add     ppxliRightV, ppxliRightV, offsetToNextRow
        subs    iYYCount, iYYCount, #1
        bne     LoopLeftRightUV_12

    add     ppxliTopSrcU, ppxlcRef0U, offsetToNextRow, lsl #3       @ 12 lines
    add     ppxliTopSrcV, ppxlcRef0V, offsetToNextRow, lsl #3
    add     ppxliTopSrcU, ppxliTopSrcU, offsetToNextRow, lsl #2     @ 12 lines
    add     ppxliTopSrcV, ppxliTopSrcV, offsetToNextRow, lsl #2
    sub     ppxliBottomSrcU, ppxliLeftU, offsetToNextRow
    sub     ppxliBottomSrcV, ppxliLeftV, offsetToNextRow
    mov     iXXCount, offsetToNextRow, asr #3
    str     iXXCount, [sp, #40]
LoopTopBotUVHor_12:
        ldmia   ppxliTopSrcU, {TopSrc0U, TopSrc1U}
        sub     ppxliTopSrcU, ppxliTopSrcU, offsetToNextRow, lsl #3 @ 12 lines
        ldmia   ppxliTopSrcV, {TopSrc0V, TopSrc1V}
        sub     ppxliTopSrcV, ppxliTopSrcV, offsetToNextRow, lsl #3 @ 12 lines
        ldmia   ppxliBottomSrcU, {BotSrc0U, BotSrc1U}
        sub     ppxliTopSrcU, ppxliTopSrcU, offsetToNextRow, lsl #2
        ldmia   ppxliBottomSrcV, {BotSrc0V, BotSrc1V}       
        sub     ppxliTopSrcV, ppxliTopSrcV, offsetToNextRow, lsl #2
        add     ppxliBottomSrcU, ppxliBottomSrcU, offsetToNextRow, lsl #3
        add     ppxliBottomSrcV, ppxliBottomSrcV, offsetToNextRow, lsl #3
        add     ppxliBottomSrcU, ppxliBottomSrcU, offsetToNextRow, lsl #2
        add     ppxliBottomSrcV, ppxliBottomSrcV, offsetToNextRow, lsl #2
        mov     kk, #12  @ 12 lines
LoopTopBotUVVer_12:
            stmia   ppxliTopSrcU, {TopSrc0U, TopSrc1U}
            add     ppxliTopSrcU, ppxliTopSrcU, offsetToNextRow
            stmia   ppxliTopSrcV, {TopSrc0V, TopSrc1V}
            add     ppxliTopSrcV, ppxliTopSrcV, offsetToNextRow
            stmia   ppxliBottomSrcU, {BotSrc0U, BotSrc1U}
            sub     ppxliBottomSrcU, ppxliBottomSrcU, offsetToNextRow
            stmia   ppxliBottomSrcV, {BotSrc0V, BotSrc1V}
            sub     ppxliBottomSrcV, ppxliBottomSrcV, offsetToNextRow
            subs    kk, kk, #1
            bne     LoopTopBotUVVer_12
        ldr     iXXCount, [sp, #40]
        add     ppxliTopSrcU, ppxliTopSrcU, #8
        add     ppxliTopSrcV, ppxliTopSrcV, #8
        add     ppxliBottomSrcU, ppxliBottomSrcU, #8
        add     ppxliBottomSrcV, ppxliBottomSrcV, #8
        subs    iXXCount, iXXCount, #1
        str     iXXCount, [sp, #40]
        bne     LoopTopBotUVHor_12

    FUNCTION_PROFILE_STOP
    ldmfd   sp!, {r4 - r12, PC}
    WMV_ENTRY_END
    
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@Void_WMV g_RepeatRef0Y_LeftRight ( 
@	const U8_WMV* pLeft, 
@	const U8_WMV* pRight, 
@	U8_WMV* pDst, 
@	I32_WMV iWidthPrevY,	
@	I32_WMV iWidthYPlusExp,  
@	I32_WMV iRowNum )    
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	WMV_LEAF_ENTRY g_RepeatRef0Y_LeftRight

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
	WMV_LEAF_ENTRY g_RepeatRef0UV_LeftRight

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
	WMV_LEAF_ENTRY g_memcpy

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
	
	
