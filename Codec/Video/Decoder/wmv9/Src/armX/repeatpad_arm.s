;//*@@@+++@@@@******************************************************************
;//
;// Microsoft Windows Media
;// Copyright (C) Microsoft Corporation. All rights reserved.
;//
;//*@@@---@@@@******************************************************************

;//************************************************************************
;//
;//	Module Name:
;//
;//	    repeatpadARM.s
;//
;//	Abstract:
;//	
;//	    ARM specific transforms
;//		Optimized assembly routines to implement repeatPad
;//
;//     Custom build with 
;//          armasm $(InputDir)\$(InputName).s $(OutDir)\$(InputName).obj
;//     and
;//          $(OutDir)\$(InputName).obj
;//	
;//	Author:
;//	
;//	    Chuang Gu (chuanggu@microsoft.com) April 12, 2001
;//
;//	Revision History:
;//
;//*************************************************************************


    INCLUDE wmvdec_member_arm.inc
    INCLUDE xplatform_arm_asm.h 
    IF UNDER_CE != 0
    INCLUDE kxarm.h
    ENDIF
 
	AREA REPEATPAD, CODE, READONLY

	IF WMV_OPT_REPEATPAD_ARM=1

    EXPORT  g_RepeatRef0Y_32                  
    EXPORT  g_RepeatRef0UV_16                    
    EXPORT  g_RepeatRef0Y_24                  
    EXPORT  g_RepeatRef0UV_12  
    
    EXPORT  g_memcpy  
    EXPORT  g_RepeatRef0Y_LeftRight 
    EXPORT  g_RepeatRef0UV_LeftRight 

;Void g_RepeatRef0Y_32 (
;	PixelC* ppxlcRef0Y,
;	CoordI  iStart, 
;   CoordI  iEnd,
;	Int     iOldLeftOffet,
;	Bool    fTop, 
;   Bool    fBottom,
;   Int     iWidthY,
;   Int     iWidthYPlusExp,
;   Int     iWidthPrevY
;)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Registers g_RepeatRef0Y_32
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ppxlcRef0Y              RN  0
iStart                  RN  1
iEnd                    RN  2
iWidthYPlusExp          RN  3
ppxliLeft               RN  4
ppxliRight              RN  5
iYCount                 RN  6
uiLeftValue             RN  7
uiRightValue            RN  8
offsetToNextRow         RN  9

ppxliTopSrc             RN  0
ppxliBottomSrc          RN  1
iXCount                 RN  2
TopSrc0                 RN  3
TopSrc1                 RN  4
TopSrc2                 RN  5
TopSrc3                 RN  6
BotSrc0                 RN  7
BotSrc1                 RN  8
BotSrc2                 RN  10
BotSrc3                 RN  11
k                       RN  12

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Registers g_RepeatRef0UV_16
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ppxlcRef0U              RN  0
ppxlcRef0V              RN  1
iStartUV                RN  2
iEndUV                  RN  3
ppxliLeftU              RN  4
ppxliRightU             RN  5
ppxliLeftV              RN  6
ppxliRightV             RN  7
iYYCount                RN  8
uiLeftValueU            RN  10
uiRightValueU           RN  11
uiLeftValueV            RN  12
uiRightValueV           RN  14

iWidthUVPlusExp         RN  8    

ppxliTopSrcU            RN  0
ppxliTopSrcV            RN  1
ppxliBottomSrcU         RN  2
ppxliBottomSrcV         RN  3
kk                      RN  4
TopSrc0U                RN  5
TopSrc1U                RN  6
BotSrc0U                RN  7
BotSrc1U                RN  8
TopSrc0V                RN  10
TopSrc1V                RN  11
BotSrc0V                RN  12
BotSrc1V                RN  14
iXXCount                RN  14

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    WMV_LEAF_ENTRY g_RepeatRef0Y_32
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    STMFD	sp!, {r4 - r12, r14}
    FRAME_PROFILE_COUNT
    FUNCTION_PROFILE_START DECODEREPEATPAD_PROFILE
    
    LDR     offsetToNextRow, [sp, #56]  ; get  iWidthPrevY
    ADD     ppxliLeft, ppxlcRef0Y, offsetToNextRow, LSL #5
    LDR     iWidthYPlusExp, [sp, #52]  
    ADD     ppxliRight, ppxliLeft, iWidthYPlusExp
    SUB     iYCount, iEnd, iStart
LoopLeftRightY
        LDRB    uiLeftValue, [ppxliLeft, #32]
        LDRB    uiRightValue, [ppxliRight, #-1]
        ORR     uiLeftValue, uiLeftValue, uiLeftValue, LSL #8
        ORR     uiLeftValue, uiLeftValue, uiLeftValue, LSL #16
        ORR     uiRightValue, uiRightValue, uiRightValue, LSL #8
        ORR     uiRightValue, uiRightValue, uiRightValue, LSL #16
        STR     uiLeftValue, [ppxliLeft]
        STR     uiLeftValue, [ppxliLeft, #4]
        STR     uiLeftValue, [ppxliLeft, #8]
        STR     uiLeftValue, [ppxliLeft, #12]
        STR     uiLeftValue, [ppxliLeft, #16]
        STR     uiLeftValue, [ppxliLeft, #20]
        STR     uiLeftValue, [ppxliLeft, #24]
        STR     uiLeftValue, [ppxliLeft, #28]
        STR     uiRightValue, [ppxliRight]
        STR     uiRightValue, [ppxliRight, #4]
        STR     uiRightValue, [ppxliRight, #8]
        STR     uiRightValue, [ppxliRight, #12]
        STR     uiRightValue, [ppxliRight, #16]
        STR     uiRightValue, [ppxliRight, #20]
        STR     uiRightValue, [ppxliRight, #24]
        STR     uiRightValue, [ppxliRight, #28]
        ADD     ppxliLeft, ppxliLeft, offsetToNextRow
        ADD     ppxliRight, ppxliRight, offsetToNextRow
        SUB     iYCount, iYCount, #1
        CMP     iYCount, #0
        BNE     LoopLeftRightY

    ADD     ppxliTopSrc, ppxlcRef0Y, offsetToNextRow, LSL #5
    SUB     ppxliBottomSrc, ppxliLeft, offsetToNextRow
    MOV     iXCount, offsetToNextRow, ASR #4
LoopTopBotHorY
        LDMIA   ppxliTopSrc, {TopSrc0, TopSrc1, TopSrc2, TopSrc3}  
        LDMIA   ppxliBottomSrc, {BotSrc0, BotSrc1, BotSrc2, BotSrc3}
        SUB     ppxliTopSrc, ppxliTopSrc, offsetToNextRow, LSL #5
        ADD     ppxliBottomSrc, ppxliBottomSrc, offsetToNextRow, LSL #5
        MOV     k, #32
LoopTopBotVerY
            STMIA   ppxliTopSrc, {TopSrc0, TopSrc1, TopSrc2, TopSrc3}
            STMIA   ppxliBottomSrc, {BotSrc0, BotSrc1, BotSrc2, BotSrc3}
            ADD     ppxliTopSrc, ppxliTopSrc, offsetToNextRow
            SUB     ppxliBottomSrc, ppxliBottomSrc, offsetToNextRow
            SUB     k, k, #1
            CMP     k, #0
            BNE     LoopTopBotVerY
        ADD     ppxliTopSrc, ppxliTopSrc, #16
        ADD     ppxliBottomSrc, ppxliBottomSrc, #16
        SUB     iXCount, iXCount, #1
        CMP     iXCount, #0
        BNE     LoopTopBotHorY

    FUNCTION_PROFILE_STOP
    LDMFD   sp!, {r4 - r12, PC}
    WMV_ENTRY_END

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Void g_RepeatRef0UV_16 (
;	PixelC* ppxlcRef0U,
;	PixelC* ppxlcRef0V,
;	Coord I  iStart, CoordI iEnd,
;	Int     iOldLeftOffet,
;	Bool    fTop, 
;   Bool    fBottom,
;   Int     iWidthUV,
;   Int     iWidthUVPlusExp,
;   Int     iWidthPrevUV
;)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    WMV_LEAF_ENTRY g_RepeatRef0UV_16
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    STMFD	sp!, {r4 - r12, r14}
    FRAME_PROFILE_COUNT
    FUNCTION_PROFILE_START DECODEREPEATPAD_PROFILE

    LDR     offsetToNextRow, [sp, #60]  ; get  iWidthPrevUV
    ADD     ppxliLeftU, ppxlcRef0U, offsetToNextRow, LSL #4
    LDR     iWidthUVPlusExp, [sp, #56]
    ADD     ppxliRightU, ppxliLeftU, iWidthUVPlusExp  
    ADD     ppxliLeftV, ppxlcRef0V, offsetToNextRow, LSL #4
    ADD     ppxliRightV, ppxliLeftV, iWidthUVPlusExp
    SUB     iYYCount, iEndUV, iStartUV;
LoopLeftRightUV
        LDRB    uiLeftValueU, [ppxliLeftU, #16]
        LDRB    uiLeftValueV, [ppxliLeftV, #16]
        ORR     uiLeftValueU, uiLeftValueU, uiLeftValueU, LSL #8
        ORR     uiLeftValueU, uiLeftValueU, uiLeftValueU, LSL #16
        ORR     uiLeftValueV, uiLeftValueV, uiLeftValueV, LSL #8
        ORR     uiLeftValueV, uiLeftValueV, uiLeftValueV, LSL #16
        STR     uiLeftValueU, [ppxliLeftU]
        STR     uiLeftValueU, [ppxliLeftU, #4]
        STR     uiLeftValueU, [ppxliLeftU, #8]
        STR     uiLeftValueU, [ppxliLeftU, #12]
        STR     uiLeftValueV, [ppxliLeftV]
        STR     uiLeftValueV, [ppxliLeftV, #4]
        STR     uiLeftValueV, [ppxliLeftV, #8]
        STR     uiLeftValueV, [ppxliLeftV, #12]
        LDRB    uiRightValueU, [ppxliRightU, #-1]
        LDRB    uiRightValueV, [ppxliRightV, #-1]
        ORR     uiRightValueU, uiRightValueU, uiRightValueU, LSL #8
        ORR     uiRightValueU, uiRightValueU, uiRightValueU, LSL #16
        ORR     uiRightValueV, uiRightValueV, uiRightValueV, LSL #8
        ORR     uiRightValueV, uiRightValueV, uiRightValueV, LSL #16
        STR     uiRightValueU, [ppxliRightU]
        STR     uiRightValueU, [ppxliRightU, #4]
        STR     uiRightValueU, [ppxliRightU, #8]
        STR     uiRightValueU, [ppxliRightU, #12]
        STR     uiRightValueV, [ppxliRightV]
        STR     uiRightValueV, [ppxliRightV, #4]
        STR     uiRightValueV, [ppxliRightV, #8]
        STR     uiRightValueV, [ppxliRightV, #12]
        ADD     ppxliLeftU, ppxliLeftU, offsetToNextRow
        ADD     ppxliRightU, ppxliRightU, offsetToNextRow
        ADD     ppxliLeftV, ppxliLeftV, offsetToNextRow
        ADD     ppxliRightV, ppxliRightV, offsetToNextRow
        SUB     iYYCount, iYYCount, #1
        CMP     iYYCount, #0
        BNE     LoopLeftRightUV

    ADD     ppxliTopSrcU, ppxlcRef0U, offsetToNextRow, LSL #4
    ADD     ppxliTopSrcV, ppxlcRef0V, offsetToNextRow, LSL #4
    SUB     ppxliBottomSrcU, ppxliLeftU, offsetToNextRow
    SUB     ppxliBottomSrcV, ppxliLeftV, offsetToNextRow
    MOV     iXXCount, offsetToNextRow, ASR #3
    STR     iXXCount, [sp, #40]
LoopTopBotUVHor
        LDMIA   ppxliTopSrcU, {TopSrc0U, TopSrc1U}
        LDMIA   ppxliTopSrcV, {TopSrc0V, TopSrc1V}
        LDMIA   ppxliBottomSrcU, {BotSrc0U, BotSrc1U}
        LDMIA   ppxliBottomSrcV, {BotSrc0V, BotSrc1V}
        SUB     ppxliTopSrcU, ppxliTopSrcU, offsetToNextRow, LSL #4
        SUB     ppxliTopSrcV, ppxliTopSrcV, offsetToNextRow, LSL #4
        ADD     ppxliBottomSrcU, ppxliBottomSrcU, offsetToNextRow, LSL #4
        ADD     ppxliBottomSrcV, ppxliBottomSrcV, offsetToNextRow, LSL #4
        MOV     kk, #16
LoopTopBotUVVer
            STMIA   ppxliTopSrcU, {TopSrc0U, TopSrc1U}
            STMIA   ppxliTopSrcV, {TopSrc0V, TopSrc1V}
            STMIA   ppxliBottomSrcU, {BotSrc0U, BotSrc1U}
            STMIA   ppxliBottomSrcV, {BotSrc0V, BotSrc1V}
            ADD     ppxliTopSrcU, ppxliTopSrcU, offsetToNextRow
            ADD     ppxliTopSrcV, ppxliTopSrcV, offsetToNextRow
            SUB     ppxliBottomSrcU, ppxliBottomSrcU, offsetToNextRow
            SUB     ppxliBottomSrcV, ppxliBottomSrcV, offsetToNextRow
            SUB     kk, kk, #1
            CMP     kk, #0
            BNE     LoopTopBotUVVer
        LDR     iXXCount, [sp, #40]
        ADD     ppxliTopSrcU, ppxliTopSrcU, #8
        ADD     ppxliTopSrcV, ppxliTopSrcV, #8
        ADD     ppxliBottomSrcU, ppxliBottomSrcU, #8
        ADD     ppxliBottomSrcV, ppxliBottomSrcV, #8
        SUB     iXXCount, iXXCount, #1
        STR     iXXCount, [sp, #40]
        CMP     iXXCount, #0
        BNE     LoopTopBotUVHor

    FUNCTION_PROFILE_STOP
    LDMFD   sp!, {r4 - r12, PC}
    WMV_ENTRY_END

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    WMV_LEAF_ENTRY g_RepeatRef0Y_24
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    nop
    STMFD	sp!, {r4 - r12, r14}
    FRAME_PROFILE_COUNT
    FUNCTION_PROFILE_START DECODEREPEATPAD_PROFILE
    
    LDR     offsetToNextRow, [sp, #56]  ; get  iWidthPrevY
    LDR     iWidthYPlusExp, [sp, #52]
    ADD     ppxliLeft, ppxlcRef0Y, offsetToNextRow, LSL #4
    SUB     iYCount, iEnd, iStart
    ADD     ppxliLeft, ppxliLeft, offsetToNextRow, LSL #3   ;24 lines
    ADD     ppxliRight, ppxliLeft, iWidthYPlusExp
    
LoopLeftRightY_24

        LDRB    uiLeftValue, [ppxliLeft, #24]
        LDRB    uiRightValue, [ppxliRight, #-1]
        SUBS    iYCount, iYCount, #1
        PLD     [ppxliLeft, offsetToNextRow, lsl #4]
        PLD     [ppxliRight, offsetToNextRow, lsl #4]
        ORR     uiLeftValue, uiLeftValue, uiLeftValue, LSL #8
        ORR     uiRightValue, uiRightValue, uiRightValue, LSL #8
        ORR     uiLeftValue, uiLeftValue, uiLeftValue, LSL #16
        ORR     uiRightValue, uiRightValue, uiRightValue, LSL #16
        STR     uiLeftValue, [ppxliLeft]
        STR     uiLeftValue, [ppxliLeft, #4]
        STR     uiLeftValue, [ppxliLeft, #8]
        STR     uiLeftValue, [ppxliLeft, #12]
        STR     uiLeftValue, [ppxliLeft, #16]
        STR     uiLeftValue, [ppxliLeft, #20]
        ADD     ppxliLeft, ppxliLeft, offsetToNextRow
        STR     uiRightValue, [ppxliRight]
        STR     uiRightValue, [ppxliRight, #4]
        STR     uiRightValue, [ppxliRight, #8]
        STR     uiRightValue, [ppxliRight, #12]
        STR     uiRightValue, [ppxliRight, #16]
        STR     uiRightValue, [ppxliRight, #20]
        ADD     ppxliRight, ppxliRight, offsetToNextRow
        BNE     LoopLeftRightY_24

    ADD     ppxliTopSrc, ppxlcRef0Y, offsetToNextRow, LSL #4
    MOV     iXCount, offsetToNextRow, ASR #4
    ADD     ppxliTopSrc, ppxliTopSrc, offsetToNextRow, LSL #3 ; 24 lines
    SUB     ppxliBottomSrc, ppxliLeft, offsetToNextRow
LoopTopBotHorY_24
        LDMIA   ppxliTopSrc, {TopSrc0, TopSrc1, TopSrc2, TopSrc3} 
        SUB     ppxliTopSrc, ppxliTopSrc, offsetToNextRow, LSL #4           ; 24 lines
        LDMIA   ppxliBottomSrc, {BotSrc0, BotSrc1, BotSrc2, BotSrc3}
        ADD     ppxliBottomSrc, ppxliBottomSrc, offsetToNextRow, LSL #4     ; 24 lines
        SUB     ppxliTopSrc, ppxliTopSrc, offsetToNextRow, LSL #3
        ADD     ppxliBottomSrc, ppxliBottomSrc, offsetToNextRow, LSL #3
        MOV     k, #24  ;24 lines
LoopTopBotVerY_24
            STMIA   ppxliTopSrc, {TopSrc0, TopSrc1, TopSrc2, TopSrc3}
            ADD     ppxliTopSrc, ppxliTopSrc, offsetToNextRow
            STMIA   ppxliBottomSrc, {BotSrc0, BotSrc1, BotSrc2, BotSrc3}
            SUB     ppxliBottomSrc, ppxliBottomSrc, offsetToNextRow
            SUBS    k, k, #1
            BNE     LoopTopBotVerY_24
        ADD     ppxliTopSrc, ppxliTopSrc, #16
        ADD     ppxliBottomSrc, ppxliBottomSrc, #16
        SUBS    iXCount, iXCount, #1
        BNE     LoopTopBotHorY_24

    FUNCTION_PROFILE_STOP
    LDMFD   sp!, {r4 - r12, PC}
    WMV_ENTRY_END

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    WMV_LEAF_ENTRY g_RepeatRef0UV_12
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    STMFD	sp!, {r4 - r12, r14}
    FRAME_PROFILE_COUNT
    FUNCTION_PROFILE_START DECODEREPEATPAD_PROFILE

    LDR     offsetToNextRow, [sp, #60]  ; get  iWidthPrevUV
    LDR     iWidthUVPlusExp, [sp, #56]
    ADD     ppxliLeftU, ppxlcRef0U, offsetToNextRow, LSL #3     ; 12 lines
    ADD     ppxliLeftV, ppxlcRef0V, offsetToNextRow, LSL #3     ; 12 lines
    ADD     ppxliLeftU, ppxliLeftU, offsetToNextRow, LSL #2
    ADD     ppxliLeftV, ppxliLeftV, offsetToNextRow, LSL #2
    ADD     ppxliRightU, ppxliLeftU, iWidthUVPlusExp
    ADD     ppxliRightV, ppxliLeftV, iWidthUVPlusExp
    SUB     iYYCount, iEndUV, iStartUV;
LoopLeftRightUV_12
        LDRB    uiLeftValueU, [ppxliLeftU, #12]
        LDRB    uiLeftValueV, [ppxliLeftV, #12]
        LDRB    uiRightValueU, [ppxliRightU, #-1]
        LDRB    uiRightValueV, [ppxliRightV, #-1]
        ORR     uiLeftValueU, uiLeftValueU, uiLeftValueU, LSL #8
        ORR     uiLeftValueV, uiLeftValueV, uiLeftValueV, LSL #8
        ORR     uiLeftValueU, uiLeftValueU, uiLeftValueU, LSL #16
        ORR     uiLeftValueV, uiLeftValueV, uiLeftValueV, LSL #16
        STR     uiLeftValueU, [ppxliLeftU]
        STR     uiLeftValueU, [ppxliLeftU, #4]
        STR     uiLeftValueU, [ppxliLeftU, #8]
        STR     uiLeftValueV, [ppxliLeftV]
        STR     uiLeftValueV, [ppxliLeftV, #4]
        STR     uiLeftValueV, [ppxliLeftV, #8]
        ORR     uiRightValueU, uiRightValueU, uiRightValueU, LSL #8
        ORR     uiRightValueV, uiRightValueV, uiRightValueV, LSL #8
        ORR     uiRightValueU, uiRightValueU, uiRightValueU, LSL #16
        ORR     uiRightValueV, uiRightValueV, uiRightValueV, LSL #16
        STR     uiRightValueU, [ppxliRightU]
        STR     uiRightValueU, [ppxliRightU, #4]
        STR     uiRightValueU, [ppxliRightU, #8]
        STR     uiRightValueV, [ppxliRightV]
        STR     uiRightValueV, [ppxliRightV, #4]
        STR     uiRightValueV, [ppxliRightV, #8]
        ADD     ppxliLeftU, ppxliLeftU, offsetToNextRow
        ADD     ppxliRightU, ppxliRightU, offsetToNextRow
        ADD     ppxliLeftV, ppxliLeftV, offsetToNextRow
        ADD     ppxliRightV, ppxliRightV, offsetToNextRow
        SUBS    iYYCount, iYYCount, #1
        BNE     LoopLeftRightUV_12

    ADD     ppxliTopSrcU, ppxlcRef0U, offsetToNextRow, LSL #3       ; 12 lines
    ADD     ppxliTopSrcV, ppxlcRef0V, offsetToNextRow, LSL #3
    ADD     ppxliTopSrcU, ppxliTopSrcU, offsetToNextRow, LSL #2     ; 12 lines
    ADD     ppxliTopSrcV, ppxliTopSrcV, offsetToNextRow, LSL #2
    SUB     ppxliBottomSrcU, ppxliLeftU, offsetToNextRow
    SUB     ppxliBottomSrcV, ppxliLeftV, offsetToNextRow
    MOV     iXXCount, offsetToNextRow, ASR #3
    STR     iXXCount, [sp, #40]
LoopTopBotUVHor_12
        LDMIA   ppxliTopSrcU, {TopSrc0U, TopSrc1U}
        SUB     ppxliTopSrcU, ppxliTopSrcU, offsetToNextRow, LSL #3 ; 12 lines
        LDMIA   ppxliTopSrcV, {TopSrc0V, TopSrc1V}
        SUB     ppxliTopSrcV, ppxliTopSrcV, offsetToNextRow, LSL #3 ; 12 lines
        LDMIA   ppxliBottomSrcU, {BotSrc0U, BotSrc1U}
        SUB     ppxliTopSrcU, ppxliTopSrcU, offsetToNextRow, LSL #2
        LDMIA   ppxliBottomSrcV, {BotSrc0V, BotSrc1V}       
        SUB     ppxliTopSrcV, ppxliTopSrcV, offsetToNextRow, LSL #2
        ADD     ppxliBottomSrcU, ppxliBottomSrcU, offsetToNextRow, LSL #3
        ADD     ppxliBottomSrcV, ppxliBottomSrcV, offsetToNextRow, LSL #3
        ADD     ppxliBottomSrcU, ppxliBottomSrcU, offsetToNextRow, LSL #2
        ADD     ppxliBottomSrcV, ppxliBottomSrcV, offsetToNextRow, LSL #2
        MOV     kk, #12  ; 12 lines
LoopTopBotUVVer_12
            STMIA   ppxliTopSrcU, {TopSrc0U, TopSrc1U}
            ADD     ppxliTopSrcU, ppxliTopSrcU, offsetToNextRow
            STMIA   ppxliTopSrcV, {TopSrc0V, TopSrc1V}
            ADD     ppxliTopSrcV, ppxliTopSrcV, offsetToNextRow
            STMIA   ppxliBottomSrcU, {BotSrc0U, BotSrc1U}
            SUB     ppxliBottomSrcU, ppxliBottomSrcU, offsetToNextRow
            STMIA   ppxliBottomSrcV, {BotSrc0V, BotSrc1V}
            SUB     ppxliBottomSrcV, ppxliBottomSrcV, offsetToNextRow
            SUBS    kk, kk, #1
            BNE     LoopTopBotUVVer_12
        LDR     iXXCount, [sp, #40]
        ADD     ppxliTopSrcU, ppxliTopSrcU, #8
        ADD     ppxliTopSrcV, ppxliTopSrcV, #8
        ADD     ppxliBottomSrcU, ppxliBottomSrcU, #8
        ADD     ppxliBottomSrcV, ppxliBottomSrcV, #8
        SUBS    iXXCount, iXXCount, #1
        STR     iXXCount, [sp, #40]
        BNE     LoopTopBotUVHor_12

    FUNCTION_PROFILE_STOP
    LDMFD   sp!, {r4 - r12, PC}
    WMV_ENTRY_END
    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Void_WMV g_RepeatRef0Y_LeftRight ( 
;	const U8_WMV* pLeft, 
;	const U8_WMV* pRight, 
;	U8_WMV* pDst, 
;	I32_WMV iWidthPrevY,	
;	I32_WMV iWidthYPlusExp,  
;	I32_WMV iRowNum )    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	WMV_LEAF_ENTRY g_RepeatRef0Y_LeftRight

;r0: pLeft, 
;r1: pRight, 
;r2: pDst, 
;r3: iWidthPrevY,	
;r4: pDstPlusWidth
;r5: iRowNum
;r6,r7: uipadValueLeft
;r8,r9: uipadValueRight
;r14: temp

StackOffset_RegSaving		EQU		28
StackOffset_iWidthYPlusExp	EQU		StackOffset_RegSaving + 0
StackOffset_iRowNum			EQU		StackOffset_RegSaving + 4

	stmdb     sp!, {r4-r9, r14}

	pld		[r0, r3]
	pld		[r1, r3]
	ldr		r4, [sp, #StackOffset_iWidthYPlusExp]
	ldr		r5, [sp, #StackOffset_iRowNum]
	pld		[r0, r3, lsl #1]
	pld		[r1, r3, lsl #1]
	add		r4, r2, r4
	
Y_LR_Loop

	ldrb	r6, [r0], r3
	ldrb	r8, [r1], r3
	pld		[r0, r3, lsl #1]
	pld		[r1, r3, lsl #1]
	
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
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Void_WMV g_RepeatRef0UV_LeftRight ( 
;	const U8_WMV* pLeftU, 
;	const U8_WMV* pRightU, 
;	const U8_WMV* pLeftV, 
;	const U8_WMV* pRightV, 
;	U8_WMV* pDstU, 
;	U8_WMV* pDstV, 
;	I32_WMV iWidthPrevUV,	
;	I32_WMV iWidthUVPlusExp,  
;	I32_WMV iRowNum)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	WMV_LEAF_ENTRY g_RepeatRef0UV_LeftRight

;r0: pLeftU
;r1: pRightU
;r2: pLeftV
;r3: pRightV	
;r4: pDstU
;r5: pDstV
;r6,r7: uipadValueLeft
;r8,r9: uipadValueRight
;r10: pDstPlusWidth
;r11: iWidthPrevUV
;r12: iWidthUVPlusExp
;r14: iRowNum

StackOffset_RegSaving_UV	EQU		36
StackOffset_pDstU			EQU		StackOffset_RegSaving_UV + 0
StackOffset_pDstV			EQU		StackOffset_RegSaving_UV + 4
StackOffset_iWidthPrevUV	EQU		StackOffset_RegSaving_UV + 8
StackOffset_iWidthUVPlusExp	EQU		StackOffset_RegSaving_UV + 12
StackOffset_iRowNum_UV 		EQU		StackOffset_RegSaving_UV + 16
StackOffset_iRowNum_Back	EQU		-4

	stmdb     sp!, {r4-r11, r14}
	
	ldr		r12, [sp, #StackOffset_iWidthUVPlusExp]
	ldr		r11, [sp, #StackOffset_iWidthPrevUV]
	ldr		r14, [sp, #StackOffset_iRowNum_UV]
	ldr		r4 , [sp, #StackOffset_pDstU]
	ldr		r5 , [sp, #StackOffset_pDstV]
	
	pld		[r0, r11]
	pld		[r1, r11]
	pld		[r0, r11, lsl #1]
	pld		[r1, r11, lsl #1]
	
	add		r10, r4, r12   ; pDstUPlusWidth
	str		r14, [sp, #StackOffset_iRowNum_Back]
U_LR_Loop

	ldrb	r6, [r0], r11
	ldrb	r8, [r1], r11
	pld		[r0, r11, lsl #1]
	pld		[r1, r11, lsl #1]
	
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

	pld		[r2, r11]
	pld		[r3, r11]
	pld		[r2, r11, lsl #1]
	pld		[r3, r11, lsl #1]
	ldr		r14, [sp, #StackOffset_iRowNum_Back]
	add		r10, r5, r12   ; pDstVPlusWidth
V_LR_Loop

	ldrb	r6, [r2], r11
	ldrb	r8, [r3], r11
	pld		[r2, r11, lsl #1]
	pld		[r3, r11, lsl #1]
	
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


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Void_WMV g_memcpy(U8_WMV* ppxliLeftTop, U8_WMV* ppxliSrc, I32_WMV iWidthPrevY, I32_WMV iTrueWidth, I32_WMV iVertPad)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	WMV_LEAF_ENTRY g_memcpy
	
iVertPad_offset		EQU		24

     pld	[r1, #16]
     stmdb  sp!, {r4-r8, r14}

     ldr	r8, [sp, #iVertPad_offset]
     sub	r2, r2, #16
     mov	r14, r0

memcpy_load
     ldr	r4, [r1], #4
     ldr	r5, [r1], #4
     ldr	r6, [r1], #4
     ldr	r7, [r1], #4
     pld	[r1, #16]
     mov	r12, r8
    
memcpy_store
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
     
memcpy_store_last
     subs	r12, r12, #1
     str	r5, [r0, #4]
     str	r4, [r0], r2
     bne	memcpy_store_last
                     
     ldmia sp!, {r4-r8,pc}    
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	ENDIF ;WMV_OPT_REPEATPAD_ARM

    EXPORT end_repeatpad_arm
end_repeatpad_arm
    nop             ; mark location where previous function ends for cache analysis


	END	
	
	