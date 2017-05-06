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
@//	    idctARM.s
@//
@//	Abstract:
@//	
@//	    ARM specific transforms
@//		Optimized assembly routines to implement 8x8, 4x8, 8x4 IDCT
@//
@//     Custom build with 
@//          armasm \(InputDir)\\(InputName).s \(OutDir)\\(InputName).obj
@//     and
@//          \(OutDir)\\(InputName).obj
@//	
@//	Author:
@//	
@//	    Chuang Gu (chuanggu@microsoft.com) Feb. 8, 2001
@//
@//	Revision History:
@//
@//*************************************************************************
@//
@// r0 : x0, r1 : x1, ..., r8 : x8@
@// r14: blk[]
@// r9, r10, r11, r12 -> temporal registers
@//
@//*************************************************************************

    #include "../c/voWMVDecID.h"
    .include "wmvdec_member_arm.inc"
    .include "xplatform_arm_asm.h" 

	.section .text
	@AREA IDCT_8x8_4x8_8x4, CODE, READONLY
	
	.if WMV_OPT_IDCT_ARM==1

    .global g_IDCTDec_WMV2_Intra            @ intr8x8
    .global g_IDCTDec16_WMV2                @ intra8x8X8    
    .global  g_IDCTPass1_WMV2                @ interHor
    .global  g_IDCTPass2_WMV2                @ interVer
    .global  g_IDCTDec_WMV2_16bit            @ inter8x8
    .global  g_8x4IDCTDec_WMV2_16bit         @ inter8x4
    .global  g_4x8IDCTDec_WMV2_16bit         @ inter4x8

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   .macros
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    .if ARCH_V3 == 1
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        .macro  STORETWO16bits dstRN, offset, srcRN1, srcRN2 @ Hi: RN2@ Lo: RN1
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        MOV     \srcRN1, \srcRN1, LSL #16
        MOV     \srcRN1, \srcRN1, LSR #16
        ORR     \srcRN1, \srcRN1, \srcRN2, LSL #16
        STR     \srcRN1, [\dstRN, \offset]
        .endm

        .macro STORETOne16bits dstRN, srcRN, offsetLo, offsetHi 
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        STRB    \srcRN, [\dstRN, \offsetLo]
        MOV     \srcRN, \srcRN, ASR #8
        STRB    \srcRN, [\dstRN, \offsetHi]
        .endm

        .macro  LOADTWO16bits srcRN, offset, dstRN1, dstRN2 @ Hi: RN2@ Lo: RN1
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        LDR     \dstRN1, [\srcRN, \offset]
        MOV     \dstRN2, \dstRN1, ASR #16
        MOV     \dstRN1, \dstRN1, LSL #16
        MOV     \dstRN1, \dstRN1, ASR #16
        .endm

        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        .macro  LOADONE16bitsHi srcRN, offset, dstRN
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        LDR     \dstRN, [\srcRN, \offset]
        MOV     \dstRN, \dstRN, ASR #16
        .endm

        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        .macro  LOADONE16bitsLo srcRN, offset, dstRN
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        LDR     \dstRN, [\srcRN, \offset]
        MOV     \dstRN, \dstRN, LSL #16
        MOV     \dstRN, \dstRN, ASR #16
        .endm
    .endif @ //ARCH_V3

    @@@@@@@@@@@@@@@@@@
    .macro  SATURATION8 srcRN
    @@@@@@@@@@@@@@@@@@
    CMP     \srcRN, #0
	MOVMI   \srcRN, #0
	CMPPL   \srcRN, #0xFF  @ 0xFF = 255
	MOVGT   \srcRN, #0xFF  @ 0xFF = 255
    .endm

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro  Hor8x1IDCT32BitLoad srcRN @ result: r0 - r7       
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    LDR     r0, [\srcRN]
    LDR     r4, [\srcRN, #4]
    LDR     r3, [\srcRN, #8]
    LDR     r7, [\srcRN, #12]
    LDR     r1, [\srcRN, #16]
    LDR     r6, [\srcRN, #20]
    MOV     r1, r1, LSL #11
    LDR     r2, [\srcRN, #24]
    LDR     r5, [\srcRN, #28]
    MOV     r10, r0, LSL #11
    ADD     r0, r10, #128 
    .endm

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro  Hor8x1IDCT16BitLoad srcRN @ result: r0 - r7       
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .if ARCH_V3 == 1
        LOADTWO16bits \srcRN, #0, r0, r4
        LOADTWO16bits \srcRN, #4, r3, r7
        LOADTWO16bits \srcRN, #8, r1, r6
        LOADTWO16bits \srcRN, #12, r2, r5
    .else
        LDRSH   r0, [\srcRN]    
        LDRSH   r4, [\srcRN, #2]   
        LDRSH   r3, [\srcRN, #4]
        LDRSH   r7, [\srcRN, #6]
        LDRSH   r1, [\srcRN, #8]
        LDRSH   r6, [\srcRN, #10]
        LDRSH   r2, [\srcRN, #12]
        LDRSH   r5, [\srcRN, #14]
    .endif @// ARCH_V3
    MOV     r1, r1, LSL #11
    MOV     r10, r0, LSL #11
    ADD     r0, r10, #128 
    .endm

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro  Hor8x1IDCT_stage1234            @Input: r0 - r7@ Ouput: r5 - r12
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @@@@@@@@@@@ first stage
    MOV     r8, #0x8D, 30           @ 0x234 = 564
    ORR     r8, r8, #1              @ W7 -> r8  
    ADD     r9, r4, r5              @ x4 + x5    
    MUL     r8, r9, r8              @ W7 * (x4 + x5)

	MOV     r10, #0x96, 28          @ 0x960 = 2400
	ORR     r10, r10, #8            @ W3 -> r10               
    ADD     r11, r6, r7             @ x6 + x7 
    MUL     r10, r11, r10           @ W3 * (x6 + x7)

	MOV     r9, #0x8E, 28           @ 0x8E0 = 2272
	ORR     r9, r9, #4              @ W1_W7 -> r9            
    MLA     r4, r9, r4, r8          @ x4 = x8 + W1_W7 * x4@

    MOV     r11, #0xC7, 30          @ 0x31C = 796
	ORR     r11, r11, #3            @ W3_W5 -> r11       
    MUL     r6, r11, r6             @ x6: W3_W5 * x6@

    MOV     r9, #0x35, 26           @ 0xD40 = 3392
	ORR     r9, r9, #0xE            @ 0xE = 14
    MUL     r5, r9, r5              @ x5 = W1pW7 * x5@

    SUB     r6, r10, r6             @ x6 = x8 - W3_W5 * x6@

	MOV     r11, #0xFB, 28          @ 0xFB0 = 4016
	ORR     r11, r11, #1            @ W3pW5 -> r11
    MUL     r7, r11, r7             @ x7 = x8 - W3pW5 * x7@
    
    SUB     r5, r8, r5              @ x5 = x8 - W1pW7 * x5@
    @SUB     r7, r10, r7            @ x7 = x8 - W3pW5 * x7@ // should move down

    @@@@@@@@@@@@@ second stage
    ADD     r8, r0, r1              @ x8 = x0 + x1@
    SUB     r0, r0, r1              @ x0 -= x1@     
    SUB     r7, r10, r7             @ x7 = x8 - W3pW5 * x7@ // should move down
    ADD     r9, r3, r2              @ x3 + x2 -> r9
    
    MOV     r10, #0x45, 28          @ 0x450 = 1104
	ORR     r10, r10, #4            @ W6 -> r10
    MUL     r1, r10, r9             @ W6 * (x3 + x2) -> r1

    MOV     r11, #0x3B, 26          @ 0xEC0 = 3776
	ORR     r11, r11, #8            @ W2pW6 -> r11
    MUL     r2, r11, r2             @ x2 = W2pW6 * x2@

    MOV     r10, #0x62, 28          @ r10 : W2_W6 -> 0x620 = 1568
    MLA     r3, r10, r3, r1         @ x3 = x1 + W2_W6 * x3 

    SUB     r2, r1, r2              @ x2 = x1 - W2pW6 * x2@    
    ADD     r1, r4, r6              @ x1 = x4 + x6@
    SUB     r4, r4, r6              @ x4 -= x6@
    ADD     r6, r5, r7              @ x6 = x5 + x7@
    SUB     r5, r5, r7              @ x5 -= x7@
 
    @@@@@@@@@@@@@@ third stage
    ADD     r7, r8, r3              @ x7 = x8 + x3@
    SUB     r8, r8, r3              @ x8 -= x3@
    ADD     r3, r0, r2              @ x3 = x0 + x2@
    SUB     r0, r0, r2              @ x0 -= x2@
    MOV     r12, #0xB5              @ 0xB5 = 181 -> r12
    MOV     r11, #128               @ 128 -> r11
    ADD     r9, r4, r5              @ x4 + x5 -> r9
    MLA     r2, r12, r9, r11        @ 181L * (x4 + x5) + 128L

    SUB     r10, r4, r5             @ x4 - x5 -> r11
    MLA     r4, r12, r10, r11

    @@@@@@@@@@@@@@@ fourth stage
    SUB     r12, r7, r1
    MOV     r12, r12, ASR #8        @ blk [7] = (PixelI32) ((x7 - x1) >> 8)@
    SUB     r11, r3, r2, ASR #8
    MOV     r11, r11, ASR #8        @ blk [6] = (PixelI32) ((x3 - x2) >> 8)@
    SUB     r10, r0, r4, ASR #8
    MOV     r10, r10, ASR #8        @ blk [5] = (PixelI32) ((x0 - x4) >> 8)@
    SUB     r9, r8, r6
    MOV     r9, r9, ASR #8          @ blk [4] = (PixelI32) ((x8 - x6) >> 8)@
    ADD     r8, r8, r6
    MOV     r8, r8, ASR #8          @ blk [3] = (PixelI32) ((x8 + x6) >> 8)@
    ADD     r5, r7, r1
    MOV     r5, r5, ASR #8          @ blk [0] = (PixelI32) ((x7 + x1) >> 8)@
    ADD     r6, r3, r2, ASR #8
    MOV     r6, r6, ASR #8          @ blk [1] = (PixelI32) ((x3 + x2) >> 8)@
    ADD     r7, r0, r4, ASR #8
    MOV     r7, r7, ASR #8          @ blk [2] = (PixelI32) ((x0 + x4) >> 8)@         
    .endm

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro  Ver1x8IDCT16BitLoad srcRN @ result: r0 - r7       
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .if ARCH_V3 == 1
        AND     r0, \srcRN, #3
        CMP     r0, #0
        BNE     LoadHi
        LOADONE16bitsLo \srcRN, #0, r0
        LOADONE16bitsLo \srcRN, #64, r1
        LOADONE16bitsLo \srcRN, #96, r2
        LOADONE16bitsLo \srcRN, #32, r3
        LOADONE16bitsLo \srcRN, #16, r4
        LOADONE16bitsLo \srcRN, #112, r5
        LOADONE16bitsLo \srcRN, #80, r6
        LOADONE16bitsLo \srcRN, #48, r7
        B       OutOfVerLoad
LoadHi:
        SUB     \srcRN, \srcRN, #2
        LOADONE16bitsHi \srcRN, #0, r0
        LOADONE16bitsHi \srcRN, #64, r1
        LOADONE16bitsHi \srcRN, #96, r2
        LOADONE16bitsHi \srcRN, #32, r3
        LOADONE16bitsHi \srcRN, #16, r4
        LOADONE16bitsHi \srcRN, #112, r5
        LOADONE16bitsHi \srcRN, #80, r6
        LOADONE16bitsHi \srcRN, #48, r7
        ADD     \srcRN, \srcRN, #2
    .else    
        LDRSH   r0, [\srcRN]
        LDRSH   r1, [\srcRN, #64]
        LDRSH   r2, [\srcRN, #96]
        LDRSH   r3, [\srcRN, #32]
        LDRSH   r4, [\srcRN, #16]
        LDRSH   r5, [\srcRN, #112]
        LDRSH   r6, [\srcRN, #80]
        LDRSH   r7, [\srcRN, #48] 
    .endif
OutOfVerLoad:
    MOV     r0, r0, LSL #8          @ x0 = (Int)((Int)blk0[i] << 8) + 8192L@
    ADD     r0, r0, #8192
    MOV     r1, r1, LSL #8          @ x1 = (Int)blk4[i] << 8@
    .endm

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro  Ver1x8IDCT32BitLoad srcRN, iOffsetToNextRowForDCT @result: r0 - r7       
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    LDR     r0, [\srcRN], \iOffsetToNextRowForDCT
    LDR     r4, [\srcRN], \iOffsetToNextRowForDCT
    LDR     r3, [\srcRN], \iOffsetToNextRowForDCT
    LDR     r7, [\srcRN], \iOffsetToNextRowForDCT  
    LDR     r1, [\srcRN], \iOffsetToNextRowForDCT
    LDR     r6, [\srcRN], \iOffsetToNextRowForDCT
    LDR     r2, [\srcRN], \iOffsetToNextRowForDCT
    LDR     r5, [\srcRN], \iOffsetToNextRowForDCT
    MOV     r0, r0, LSL #8          @ x0 = (Int)((Int)blk0[i] << 8) + 8192L@
    ADD     r0, r0, #8192
    MOV     r1, r1, LSL #8          @ x1 = (Int)blk4[i] << 8@
    .endm

    @@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro Ver1x8IDCT_stage123
    @@@@@@@@@@@@@@@@@@@@@@@@@@
    @@@@@@@@ first stage
    MOV     r9, #4                  @ 4 --> r9
    MOV     r10, #0x8D, 30          @ 0x234 = 564
	ORR     r10, r10, #1            @ r10: W7
    ADD     r11, r4, r5             @ x4 + x5 -> r9
    MLA     r8, r10, r11, r9        @ x8 = W7 * (x4 + x5) + 4@
    
	MOV     r10, #0x96, 28          @ 0x960 = 2400
	ORR     r10, r10, #8            @ r10: W3
    ADD     r11, r6, r7             @ x6 + x7 -> r11
    MLA     r11, r10, r11, r9       @ r11 -> W3 * (x6 + x7) + 4@

    MOV     r10, #0x8E, 28          @ 0x8E0 = 2272
	ORR     r10, r10, #4            @ W1_W7 -> r11
    MLA     r4, r10, r4, r8         @ x4 = (x8 + W1_W7 * x4) @     
    
	MOV     r10, #0x35, 26          @ 0xD40 = 3392
	ORR     r10, r10, #0xE          @ W1pW7 -> r10 0xE = 14
    MUL     r5, r10, r5             @ W1pW7 * x5

    MOV     r10, #0xC7, 30          @ 0x31C = 796
	ORR     r10, r10, #3            @ W3_W5 -> r10
    MUL     r6, r10, r6             @ r6 : W3_W5 * x6 

    MOV     r10, #0xFB, 28          @ 0xFB0 = 4016
	ORR     r10, r10, #1            @ W3pW5 -> r10
    MUL     r7, r10, r7             @ r7 : W3pW5 * x7

    SUB     r5, r8, r5              @ x5 = (x8 - W1pW7 * x5) >> 3@
    MOV     r5, r5, ASR #3          @ r8 is free
    SUB     r6, r11, r6             @ x8 - W3_W5 * x6
    SUB     r7, r11, r7             @ r7 : x8 - W3pW5 * x7       @ 
    
    @@@@@@@@@@ second stage
    ADD     r8, r0, r1              @ x8 = x0 + x1@r9 is still 4
    SUB     r0, r0, r1              @ x0 -= x1@
    ADD     r1, r3, r2              @ x3 + x2 -> r1

	MOV     r10, #0x45, 28          @ 0x450 = 1104
    ORR     r10, r10, #4            @ W6 : 1108
    MLA     r9, r10, r1, r9         @ r9 <-- W6 * (x3 + x2) + 4@ 
     
    MOV     r11, #0x3B, 26          @ 0xEC0 = 3776
	ORR     r11, r11, #8            @ W2pW6 -> r11
    MUL     r2, r11, r2             @ W2pW6 * x2 -> r2

    MOV     r10, #0x62, 28          @ W2_W6 -> r10: 0x620 = 1568
    MLA     r3, r10, r3, r9         @ x3 = (x1 + W2_W6 * x3)@

    MOV     r4, r4, ASR #3          @ x4 = (x8 + W1_W7 * x4) >> 3
    ADD     r1, r4, r6, ASR #3      @ x1 = x4 + x6@
    SUB     r4, r4, r6, ASR #3      @ x4 -= x6@
    ADD     r6, r5, r7, ASR #3      @ x6 = x5 + x7@
    SUB     r5, r5, r7, ASR #3      @ x5 -= x7@
    SUB     r2, r9, r2              @ x2 = (x1 - W2pW6 * x2)

    @@@@@@@@@@@ third stage
    ADD     r7, r8, r3, ASR #3      @ x7 = x8 + x3@
    SUB     r8, r8, r3, ASR #3      @ x8 -= x3@
    ADD     r3, r0, r2, ASR #3      @ x3 = x0 + x2@
    SUB     r0, r0, r2, ASR #3      @ x0 -= x2@
    MOV     r10, #181               @ 181 -> r10
    MOV     r11, #128               @ 128 -> r11
    ADD     r2, r4, r5              @ x4 + x5 -> r2
    MLA     r2, r10, r2, r11        @ x2 = (Int) (181L * (x4 + x5) + 128L)
 
    SUB     r4, r4, r5              @ x4 - x5 -> r4
    MLA     r4, r10, r4, r11        @ x4 = (Int) (181L * (x4 - x5) + 128L)
    
    MOV     r2, r2, ASR #8          @ x2 = (Int) (181L * (x4 + x5) + 128L) >> 8@
    MOV     r4, r4, ASR #8          @ x4 = (Int) (181L * (x4 - x5) + 128L) >> 8@
    .endm

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro Ver1x8IDCT_stage4_Store destRN @
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @@@@@@@@@@@@ fourth stage
    ADD     r9, r7, r1              @ x7 + x1 -> r9
    ADD     r10, r3, r2             @ x3 + x2 -> r10
    MOV     r9, r9, ASR #14         @ (PixelI32) ((x7 + x1) >> 14)@
    .if ARCH_V3 == 1
        STORETOne16bits \destRN, r9, #0, #1
    .else
        STRH    r9, [\destRN]               @ blk0[i] = (PixelI32) ((x7 + x1) >> 14)@
    .endif
    MOV     r10, r10, ASR #14       @ r10: (x3 + x2) >> 14
    .if ARCH_V3 == 1
        STORETOne16bits \destRN, r10, #16, #17
    .else
        STRH    r10, [\destRN, #16]         @ blk1[i] = (PixelI32) ((x3 + x2) >> 14)@
    .endif
    ADD     r9, r0, r4              @ r9 : x0 + x4
    MOV     r9, r9, ASR #14         @ (x0 + x4) >> 14
    .if ARCH_V3 == 1
        STORETOne16bits \destRN, r9, #32, #33
    .else
        STRH    r9, [\destRN, #32]          @ blk2[i] = (PixelI32) ((x0 + x4) >> 14)@
    .endif
    ADD     r10, r8, r6             @ r10: x8 + x6
    MOV     r10, r10, ASR #14       @ (x8 + x6) >> 14
    .if ARCH_V3 == 1
        STORETOne16bits \destRN, r10, #48, #49
    .else
        STRH    r10, [\destRN, #48]         @ blk3[i] = (PixelI32) ((x8 + x6) >> 14)@
    .endif
    SUB     r9, r8, r6              @ r9: x8 - x6
    MOV     r9, r9, ASR #14         @ ((x8 - x6) >> 14)@
    .if ARCH_V3 == 1
        STORETOne16bits \destRN, r9, #64, #65
    .else
        STRH    r9, [\destRN, #64]         @ blk4[i] = (PixelI32) ((x8 - x6) >> 14)@
    .endif
    SUB     r10, r0, r4             @ r10: x0 - x4
    MOV     r10, r10, ASR #14       @ (x0 - x4) >> 14
    .if ARCH_V3 == 1
        STORETOne16bits \destRN, r10, #80, #81
    .else
        STRH    r10, [\destRN, #80]        @ blk5[i] = (PixelI32) ((x0 - x4) >> 14)@             
    .endif
    SUB     r9, r3, r2              @ r9: x3 - x2
    MOV     r9, r9, ASR #14         @ (x3 - x2) >> 14
    .if ARCH_V3 == 1
        STORETOne16bits \destRN, r9, #96, #97
    .else
        STRH    r9, [\destRN, #96]         @ blk6[i] = (PixelI32) ((x3 - x2) >> 14)@
    .endif
    SUB     r10, r7, r1             @ r10: x7 - x1
    MOV     r10, r10, ASR #14       @ (x7 - x1) >> 14
    .if ARCH_V3 == 1
        STORETOne16bits \destRN, r10, #112, #113
    .else
        STRH    r10, [\destRN, #112]        @ blk7[i] = (PixelI32) ((x7 - x1) >> 14)@
    .endif
    .endm
        
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro HorVer4x1IDCT     @ input: r4 - r7@ output: r0 - r3@ r10 - r12L: coefs
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    ADD     r0, r4, r6              @ x4 + x6
    MUL     r0, r10, r0             @ x0 = (x4 + x6)*W2a@
    SUB     r1, r4, r6              @ x4 - x6
    MUL     r2, r5, r11             @ x5*W1a -> r2: 
    MUL     r3, r7, r11             @ x7*W1a -> r3
    MUL     r4, r5, r12             @ x5*W3a -> r4: 
    MUL     r1, r10, r1             @ x1 = (x4 - x6)*W2a@ r4, r6 free
    MLA     r2, r7, r12, r2         @ x2 = x5*W1a + x7*W3a@
    SUB     r3, r4, r3              @ x3 = x5*W3a - x7*W1a@
    .endm

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro Ver8x4IDCT
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    LDR     r9, [sp, #8]            @ get Half&HorFlag
    LDR     r14, [sp, #4]           @ get src
    LDR     r8, [sp]                @ get dst
    MOV     r9, r9, LSR #31         @ iHalf=((U32)iHalfAndiDCTHorzFlags)>>31@
    ADD     r8, r8, r9, LSL #6      @ piDst = piDstBuf->i16 + (iHalf*32)@  
    MOV     r10, #0x5A, 28          @ 0x5A0 = 1440
    ORR     r10, r10, #8            @ W2a -> r10
    MOV     r11, #0x76, 28          @ 0x760 = 1888 
    ORR     r11, r11, #4            @ W1a -> r11
    MOV     r12, #0x31, 28          @ W3a -> r12: 0x310 = 784
    MOV     r9, #0                  @ 0 -> i
Ver8x4Loop:
    .if ARCH_V3 == 1
        LOADONE16bitsLo r14, #0, r4
        LOADONE16bitsLo r14, #32, r5
        LOADONE16bitsLo r14, #64, r6
        LOADONE16bitsLo r14, #96, r7
    .else
        LDRSH   r4, [r14]               @ x4
        LDRSH   r5, [r14, #32]          @ x5
        LDRSH   r6, [r14, #64]          @ x6
        LDRSH   r7, [r14, #96]          @ x7
    .endif
    HorVer4x1IDCT         
    ADD     r0, r0, #2, 18          @ x0 + 32768L
    ADD     r1, r1, #2, 18          @ x1 + 32768L
    ADD     r4, r0, r2              @ r4: x0 + x2 + 32768L
    ADD     r5, r1, r3              @ r5: x1 + x3 + 32768L
    SUB     r6, r1, r3              @ r6: x1 - x3 + 32768L
    SUB     r7, r0, r2              @ r7: x0 - x2 + 32768L
    AND     r0, r9, #1
	MOV     r0, r0, lsl #6
    ADD     r0, r0, r9, ASR #1      @ idex=(i>>1)+((i&1)<<6)@
    ADD     r0, r8, r0, LSL #1      @ get blk0
    MOV     r4, r4, ASR #16
    .if ARCH_V3 == 1
        STORETOne16bits r0, r4, #0, #1
    .else
        STRH    r4, [r0]                @ blk0[i] = (PixelI32)((x0 + x2 + 32768L)>>16)@
    .endif
    MOV     r5, r5, ASR #16
    .if ARCH_V3 == 1
        STORETOne16bits r0, r5, #16, #17
    .else
        STRH    r5, [r0, #16]           @ blk1[i] = (PixelI32)((x1 + x3 + 32768L)>>16)@
    .endif
    MOV     r6, r6, ASR #16
    .if ARCH_V3 == 1
        STORETOne16bits r0, r6, #32, #33
    .else
        STRH    r6, [r0, #32]           @ blk2[i] = (PixelI32)((x1 - x3 + 32768L)>>16)@
    .endif
    MOV     r7, r7, ASR #16
    .if ARCH_V3 == 1
        STORETOne16bits r0, r7, #48, #49
    .else
        STRH    r7, [r0, #48]           @ blk3[i] = (PixelI32)((x0 - x2 + 32768L)>>16)@
    .endif
    ADD     r14, r14, #4            @ next column
    ADD     r9, r9, #1              @ i+1 -> i
    CMP     r9, #8
    BNE     Ver8x4Loop            
    .endm

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA	|.text|, CODE
		@.section .text

    WMV_LEAF_ENTRY g_IDCTDec_WMV2_Intra
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    STMFD	sp!, {r0, r1, r4 - r12, r14}    @  r0: dest (sp+4)
                                        @  r1: iOffsetToNextRowForIDCT (sp+8)
                                        @  r2: src    
    FRAME_PROFILE_COUNT
    MOV     r14, r2                 @ r2 -> r14: blk[]
    MOV		r9, #0                  @ 0 -> r9	             				
    ADD     sp, sp, #-4             @ save i
    STR		r9, [sp]                @ 0 -> i            (sp)

LoopIntra8x8Hor:
    Hor8x1IDCT32BitLoad r14
    Hor8x1IDCT_stage1234            @ r14 -> src                  
    STMIA   r14, {r5 - r12}         @ output
    LDR     r9, [sp]                @ next row
    ADD     r14, r14, #32
    ADD     r9, r9, #1
    STR     r9, [sp]
    CMP     r9, #8
    BLT     LoopIntra8x8Hor

    MOV     r12, #8                 @ 8 -> i
    SUB     r14, r14, #256          @ restore r14: blk[] src
LoopIntra8x8Ver: 
    MOV     r10, r14  
    MOV     r11, #32
    Ver1x8IDCT32BitLoad r10, r11
    Ver1x8IDCT_stage123
    @@@@@@@@@@@@ fourth stage
    ADD     r5, r7, r1             
    MOV     r5, r5, ASR #14         @ x5 = (x7 + x1) >> 14@
    SUB     r1, r7, r1
    MOV     r1, r1, ASR #14         @ x1 = (x7 - x1) >> 14@
    ADD     r7, r3, r2
    MOV     r7, r7, ASR #14         @ x7 = (x3 + x2) >> 14@     
    SUB     r2, r3, r2          
    MOV     r2, r2, ASR #14         @ x2 = (x3 - x2) >> 14@
    ADD     r3, r0, r4
    MOV     r3, r3, ASR #14         @ x3 = (x0 + x4) >> 14@
    SUB     r4, r0, r4
    MOV     r4, r4, ASR #14         @ x4 = (x0 - x4) >> 14@
    ADD     r0, r8, r6
    MOV     r0, r0, ASR #14         @ x0 = (x8 + x6) >> 14@
    SUB     r6, r8, r6              
    MOV     r6, r6, ASR #14         @ x6 = (x8 - x6) >> 14@
    ORR     r9, r1, r5              @ iTest: r9
    ORR     r9, r9, r7  
    ORR     r9, r9, r2    
    ORR     r9, r9, r3
    ORR     r9, r9, r4
    ORR     r9, r9, r0
    ORR     r9, r9, r6
    BICS    r9, r9, #0xFF           @ 0xFF = 255
    BNE     Saturate8Pixels
Output8Bytes:
    LDR     r10, [sp, #4]           @ get src
    LDR     r8, [sp, #8]            @ get iOffsetToNextRowForIDCT
    MOV     r9, r10                 @ src -> r9
    
    STRB    r5, [r9], +r8           @ *blk0++ = x5@
    STRB    r7, [r9], +r8           @ *blk1++ = x7@
    STRB    r3, [r9], +r8           @ *blk2++ = x3@
    STRB    r0, [r9], +r8           @ *blk3++ = x0@
    STRB    r6, [r9], +r8           @ *blk4++ = x6@
    STRB    r4, [r9], +r8           @ *blk5++ = x4@
    STRB    r2, [r9], +r8           @ *blk6++ = x2@
    STRB    r1, [r9], +r8           @ *blk7++ = x1@

    ADD     r10, r10, #1            @ next column
    STR     r10, [sp, #4]           @ save src     
    ADD     r14, r14, #4
    SUB     r12, r12, #1            @ i-1 -> i
    CMP     r12, #0
    BNE     LoopIntra8x8Ver

    ADD     sp, sp, #12             
    LDMFD   sp!, {r4 - r12, PC}
    WMV_ENTRY_END

Saturate8Pixels:
    SATURATION8 r0
    SATURATION8 r1
    SATURATION8 r2
    SATURATION8 r3
    SATURATION8 r4
    SATURATION8 r5
    SATURATION8 r6
    SATURATION8 r7

    B Output8Bytes

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    WMV_LEAF_ENTRY g_IDCTDec16_WMV2
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	STMFD	sp!, {r0 - r1, r3, r4 - r12, r14} @ sp+8: src@ sp+12: dst@ sp+16: HorzFlag@
    FRAME_PROFILE_COUNT
    ADD     sp, sp, #-4             @ store VertFlag, i 
    MOV		r0, #0                  @ 0 -> r0	          
	STR		r0, [sp], #-4			@ 0 -> VertFlag     (sp+4)   				
    STR		r0, [sp]                @ 0 -> i            (sp)
    MOV     r9, r0                  @ r9 -> i
    MOV     r14, r1                 @ src -> r14
LoopHorX8:
	LDR		r12, [sp, #16]			@ r12: HorzFlag@ r9: i
    MOV		r11, #1                 @ 1 -> r11
    ANDS    r12, r12, r11, LSL r9   @ iDCTHorzFlags & (1 << i)     
	BNE     DoHorX8
	@@@@@@@@@@@@@@ Horz short cut
    .if ARCH_V3 == 1
        LOADONE16bitsLo r14, #0, r0
    .else
        LDRSH   r0, [r14]
    .endif
    CMP     r0, #0
    BEQ     NextHorX8
    @MOV     r0, r0, LSL #3          @ [0] - [7] <-- rgiCoefRecon [0] << 3
    MOV     r0, r0, LSL #19          @(3 + 16 = 19)
    ORR     r0, r0, r0, LSR #16
    STR     r0, [r14]
    STR     r0, [r14, #4]
    STR     r0, [r14, #8]
    STR     r0, [r14, #12]
    STR     r9, [sp, #4]            @ i -> iDCRVertFlag
    B       NextHorX8
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
DoHorX8:
    STR     r9, [sp, #4]            @ i -> iDCRVertFlag
    Hor8x1IDCT16BitLoad r14
    Hor8x1IDCT_stage1234            @ r14 -> src
    .if ARCH_V3 == 1
        STORETWO16bits r14, #0, r5, r6
        STORETWO16bits r14, #4, r7, r8
        STORETWO16bits r14, #8, r9, r10
        STORETWO16bits r14, #12, r11, r12
    .else
        STRH    r5, [r14]               @ output
        STRH    r6, [r14, #2]
        STRH    r7, [r14, #4]
        STRH    r8, [r14, #6]
        STRH    r9, [r14, #8]
        STRH    r10,[r14, #10]
        STRH    r11,[r14, #12]
        STRH    r12,[r14, #14]
    .endif @//ARCH_V3
NextHorX8:
    LDR     r9, [sp]
    ADD     r14, r14, #16
    ADD     r9, r9, #1
    STR     r9, [sp]
    CMP     r9, #8
    BLT     LoopHorX8

@@@@@@@@@@@@ Vertical transform
    LDR     r10, [sp, #4]           @ load VertFlag
    LDR     r14, [sp, #12]          @ src -> r14
    CMP     r10, #0                 @ if (iDCTVertFlag == 0)
    BEQ     VerShortCutX8           @ jump to vertical short cut
    LDR     r12, [sp, #8]           @ dst -> r12              
    MOV     r9, #0                  @ 0 -> i
    STR     r9, [sp]
LoopVerX8:      
    Ver1x8IDCT16BitLoad r14
    Ver1x8IDCT_stage123 
    Ver1x8IDCT_stage4_Store r12            
    LDR     r9, [sp]
    ADD     r14, r14, #2
    ADD     r12, r12, #2
    ADD     r9, r9, #1
    STR     r9, [sp]
    CMP     r9, #8
    BNE     LoopVerX8
    
EndVerX8:
    ADD     sp, sp, #20 
    LDMFD   sp!, {r4 - r12, PC}
    WMV_ENTRY_END

    @@@@@@@@@@@@ Vertical short cut 
VerShortCutX8:
    .if ARCH_V3 == 1
        LOADTWO16bits r14, #0, r0, r1
        LOADTWO16bits r14, #4, r2, r3
        LOADTWO16bits r14, #8, r4, r5
        LOADTWO16bits r14, #12, r6, r7
    .else
        LDRSH   r0, [r14]               @ (blk0[i] + 32) >> 6@
        LDRSH   r1, [r14, #2]
        LDRSH   r2, [r14, #4]
        LDRSH   r3, [r14, #6]
        LDRSH   r4, [r14, #8]
        LDRSH   r5, [r14, #10]
        LDRSH   r6, [r14, #12]
        LDRSH   r7, [r14, #14]
    .endif @//ARCH_V3
    ADD     r0, r0, #32
    MOV     r0, r0, ASR #6          
    ADD     r1, r1, #32
    MOV     r1, r1, ASR #6 
    ADD     r2, r2, #32
    MOV     r2, r2, ASR #6
    ADD     r3, r3, #32
    MOV     r3, r3, ASR #6
    ADD     r4, r4, #32
    MOV     r4, r4, ASR #6
    ADD     r5, r5, #32
    MOV     r5, r5, ASR #6
    ADD     r6, r6, #32
    MOV     r6, r6, ASR #6
    ADD     r7, r7, #32
    MOV     r7, r7, ASR #6
    LDR     r12, =0xffff            @ mask
    LDR     r14, [sp, #8]           @ dst -> r14
    AND     r0, r0, r12
    ORR     r8, r0, r1, LSL #16     @ r8: r1r0
    AND     r2, r2, r12
    ORR     r9, r2, r3, LSL #16     @ r9: r3r2
    AND     r4, r4, r12
    ORR     r10, r4, r5, LSL #16    @ r10: r5r4
    AND     r6, r6, r12
    ORR     r11, r6, r7, LSL #16    @ r11: r7r6 
    STMIA   r14!, {r8 - r11}        @ output one row
    STMIA   r14!, {r8 - r11}        @ output one row
    STMIA   r14!, {r8 - r11}        @ output one row
    STMIA   r14!, {r8 - r11}        @ output one row
    STMIA   r14!, {r8 - r11}        @ output one row
    STMIA   r14!, {r8 - r11}        @ output one row
    STMIA   r14!, {r8 - r11}        @ output one row
    STMIA   r14!, {r8 - r11}        @ output one row
    ADD     sp, sp, #20 
    LDMFD   sp!, {r4 - r12, PC}

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

		@AREA	|.text|, CODE
		@.section .text
    WMV_LEAF_ENTRY g_IDCTPass1_WMV2
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    STMFD	sp!, {r0 - r2, r14} @ sp+8: src/dst@ sp+12: num@ sp+16: HorzFlag@
    FRAME_PROFILE_COUNT
    MOV     r14, r0                 @ r14 -> src/dst blk[]
    ADD     sp, sp, #-4             @ store VertFlag, i 
    MOV		r0, #0                  @ 0 -> r0	          
	STR		r0, [sp], #-4			@ 0 -> VertFlag     (sp+4)   				
	MOV     r9, r0                  @ r9 -> i
    STR		r9, [sp]                @ 0 -> i            (sp)
LoopPass1:
	LDR		r12, [sp, #16]			@ r12: HorzFlag 				@ r9: i
    MOV		r11, #1                 @ 1 -> r11
    ANDS    r12, r12, r11, LSL r9   @ iDCTHorzFlags & (1 << i)     
	BNE     DoPass1
	@@@@@@@@@@@@@@ Horz short cut
    LDR     r0, [r14]
    CMP     r0, #0
    BEQ     NextPass1
    MOV     r0, r0, LSL #3          @ [0] - [7] <-- rgiCoefRecon [0] << 3
    STR     r0, [r14]
    STR     r0, [r14, #4]
    STR     r0, [r14, #8]
    STR     r0, [r14, #12]
    STR     r0, [r14, #16]
    STR     r0, [r14, #20]
    STR     r0, [r14, #24]
    STR     r0, [r14, #28]
    STR     r9, [sp, #4]            @ i -> iDCRVertFlag
    B       NextPass1
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
DoPass1:
    STR     r9, [sp, #4]            @ i -> iDCRVertFlag
    Hor8x1IDCT32BitLoad r14
    Hor8x1IDCT_stage1234            @ r14 -> src                  
    STMIA   r14, {r5 - r12}         @ output
NextPass1:
    LDR     r9, [sp]
    ADD     r14, r14, #32
    ADD     r9, r9, #1
    STR     r9, [sp]
    LDR     r8, [sp, #12]           @ get number
    CMP     r9, r8
    BLT     LoopPass1

    LDR     r0, [sp, #4]
    ADD     sp, sp, #20
    LDMFD   sp!, {PC}
    WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA	|.text|, CODE
		@.section .text
    WMV_LEAF_ENTRY g_IDCTPass2_WMV2
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    STMFD	sp!, {r0 - r2} @ sp+8: dst@ sp+12: src@ sp+16: number*4@
    FRAME_PROFILE_COUNT_SAVE_LR

    ADD     sp, sp, #-4
    MOV     r8, #0                  @ 0 -> i, k  
    STR     r8, [sp], #-4           @ 0 -> k    [sp+4]
DoPass2:
    LDR     r14, [sp, #12]          @ get src
    LDR     r12, [sp, #8]           @ get dst
    ADD     r14, r14, r8, LSL #2    @ rgiCoefRecon = rgiCoefReconOri + k@
    ADD     r12, r12, r8, LSL #7    @ PixelI16 __huge *blk0 = blk + k*64@
    MOV     r9, #0                  @ 0 -> i
    STR     r9, [sp]                @ 0 -> i    [sp]  
LoopPass2:
    MOV     r10, r14    
    LDR     r11, [sp, #16]
    Ver1x8IDCT32BitLoad r10, r11
    Ver1x8IDCT_stage123
    Ver1x8IDCT_stage4_Store r12

    ADD     r14, r14, #8            @ next src column
    ADD     r12, r12, #2            @ next dst column
    LDR     r9, [sp]                @ get i
    ADD     r9, r9, #1              @ i+1 -> i
    STR     r9, [sp]                @ save i
    LDR     r10, [sp, #16]          @ number -> r10
    CMP     r9, r10, ASR #3         @ i =? number/2
    BNE     LoopPass2               @ next column

    LDR     r8, [sp, #4]            @ get k
    ADD     r8, r8, #1              @ k+1 -> k
    STR     r8, [sp, #4]            @ save k
    CMP     r8, #2
    BNE     DoPass2                 @ odd column

    ADD     sp, sp, #32             @ all the way out of g_IDCTDec_WMV2_16bit
    LDMFD   sp!, {r4 - r12, PC}
    WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA	|.text|, CODE
		@.section .text
    WMV_LEAF_ENTRY g_IDCTDec_WMV2_16bit
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    STMFD	sp!, {r0 - r1, r3, r4 - r12, r14} @ r0: dst [sp], r1: src [sp+4], r3: HorFlag [sp+8]
    FRAME_PROFILE_COUNT

    MOV     r0, r1
    MOV     r1, #8
    MOV     r2, r3
    BL      g_IDCTPass1_WMV2

    @ r0 <= VerFlag
    CMP     r0, #0
    BEQ     VerShortCut8x8     

    LDMIA   sp, {r0 - r1}
    MOV     r2, #32                 @ number * 4 = 32
    BL      g_IDCTPass2_WMV2

VerShortCut8x8:
    LDMIA   sp, {r12, r14}          @ get dst and src
    LDMIA   r14, {r0 - r7}          @ get rgiCoefRecon[0] - [7]
    ADD     r0, r0, #32
    ADD     r1, r1, #32
    ADD     r2, r2, #32
    ADD     r3, r3, #32
    ADD     r4, r4, #32
    ADD     r5, r5, #32
    ADD     r6, r6, #32
    ADD     r7, r7, #32
    MOV     r0, r0, ASR #6          
    MOV     r1, r1, ASR #6 
    MOV     r2, r2, ASR #6
    MOV     r3, r3, ASR #6
    MOV     r4, r4, ASR #6
    MOV     r5, r5, ASR #6
    MOV     r6, r6, ASR #6
    MOV     r7, r7, ASR #6
    LDR     r11, =0xffff            @ mask
    AND     r0, r0, r11
    ORR     r8, r0, r2, LSL #16     @ r8: r0r2
    AND     r4, r4, r11
    ORR     r9, r4, r6, LSL #16     @ r9: r4r6
    STMIA   r12, {r8, r9}           @ output one even row
    ADD     r12, r12, #16
    STMIA   r12, {r8, r9}  
    ADD     r12, r12, #16
    STMIA   r12, {r8, r9}  
    ADD     r12, r12, #16
    STMIA   r12, {r8, r9}  
    ADD     r12, r12, #16
    STMIA   r12, {r8, r9}  
    ADD     r12, r12, #16
    STMIA   r12, {r8, r9}  
    ADD     r12, r12, #16
    STMIA   r12, {r8, r9}  
    ADD     r12, r12, #16
    STMIA   r12, {r8, r9} 
    ADD     r12, r12, #16
    AND     r1, r1, r11
    ORR     r8, r1, r3, LSL #16     @ r8: r1r3
    AND     r5, r5, r11
    ORR     r9, r5, r7, LSL #16     @ r9: r5r7
    STMIA   r12, {r8, r9}           @ output one odd row
    ADD     r12, r12, #16
    STMIA   r12, {r8, r9}  
    ADD     r12, r12, #16
    STMIA   r12, {r8, r9}  
    ADD     r12, r12, #16
    STMIA   r12, {r8, r9}  
    ADD     r12, r12, #16
    STMIA   r12, {r8, r9}  
    ADD     r12, r12, #16
    STMIA   r12, {r8, r9}  
    ADD     r12, r12, #16
    STMIA   r12, {r8, r9}  
    ADD     r12, r12, #16
    STMIA   r12, {r8, r9}         

    ADD     sp, sp, #12             @ all the way out of g_IDCTDec_WMV2_16bit
    LDMFD   sp!, {r4 - r12, PC}
    WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA	|.text|, CODE
		@.section .text
    WMV_LEAF_ENTRY g_8x4IDCTDec_WMV2_16bit
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    STMFD	sp!, {r0, r2 - r3, r4 - r12, r14} @ r0: dst [sp], r2: src [sp+4], r3: Half&HorFlag [sp+8]
    FRAME_PROFILE_COUNT
    
    MOV     r0, r2
    MOV     r1, #4
    LDR     r2, =0x7fffffff
    AND     r2, r3, r2              @ iDCTHorzFlags=iHalfAndiDCTHorzFlags&0x7fffffff@
    BL      g_IDCTPass1_WMV2

    @ r0 <= VerFlag
    CMP     r0, #0
    BEQ     VerShortCut8x4     

    Ver8x4IDCT    
    ADD     sp, sp, #12             @ all the way out of g_8x4IDCTDec_WMV2_16bit
    LDMFD   sp!, {r4 - r12, PC}

VerShortCut8x4:
    LDMIA   sp, {r12, r14}          @ get dst and src
    LDR     r9, [sp, #8]            @ get Half&HorFlag
    MOV     r9, r9, LSR #31         @ iHalf=((U32)iHalfAndiDCTHorzFlags)>>31@
    ADD     r12, r12, r9, LSL #6    @ piDst = piDstBuf->i16 + (iHalf*32)@  
    LDMIA   r14, {r0 - r7}          @ get rgiCoefRecon[0] - [7]
    MOV     r8, #0x5A, 28           @ 0x5A0 = 1440
    ORR     r8, r8, #8              @ W2a -> r8
    MOV     r9, #2, 18              @ 32768L -> r9
    @rgiCoefRecon[i]*W2a + 32768L) >> 16 )
    MLA     r0, r8, r0, r9
    MLA     r1, r8, r1, r9
    MLA     r2, r8, r2, r9
    MLA     r3, r8, r3, r9
    MLA     r4, r8, r4, r9
    MLA     r5, r8, r5, r9
    MLA     r6, r8, r6, r9
    MLA     r7, r8, r7, r9
    MOV     r0, r0, ASR #16          
    MOV     r1, r1, ASR #16 
    MOV     r2, r2, ASR #16
    MOV     r3, r3, ASR #16
    MOV     r4, r4, ASR #16
    MOV     r5, r5, ASR #16
    MOV     r6, r6, ASR #16
    MOV     r7, r7, ASR #16
    LDR     r11, =0xffff            @ mask
    AND     r0, r0, r11
    ORR     r8, r0, r2, LSL #16     @ r8: r0r2
    AND     r4, r4, r11
    ORR     r9, r4, r6, LSL #16     @ r9: r4r6
    STMIA   r12, {r8, r9}           @ output one even row
    ADD     r12, r12, #16
    STMIA   r12, {r8, r9}  
    ADD     r12, r12, #16
    STMIA   r12, {r8, r9}  
    ADD     r12, r12, #16
    STMIA   r12, {r8, r9}  
    AND     r1, r1, r11
    ORR     r8, r1, r3, LSL #16     @ r8: r1r3
    AND     r5, r5, r11
    ORR     r9, r5, r7, LSL #16     @ r9: r5r7
    ADD     r12, r12, #80           @ to odd field
    STMIA   r12, {r8, r9}           @ output one odd row
    ADD     r12, r12, #16
    STMIA   r12, {r8, r9}  
    ADD     r12, r12, #16
    STMIA   r12, {r8, r9}  
    ADD     r12, r12, #16
    STMIA   r12, {r8, r9}     
    ADD     sp, sp, #12             @ all the way out of g_8x4IDCTDec_WMV2_16bit
    LDMFD   sp!, {r4 - r12, PC}
    WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA	|.text|, CODE
		@.section .text
g_4x8IDCTDec_WMV2_16bit:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    STMFD	sp!, {r0, r2 - r3, r4 - r12, r14} @ r0: dst [sp], r2: src [sp+4], r3: Half&HorFlag [sp+8]
    FRAME_PROFILE_COUNT
    
    MOV     r10, #0x5A, 28          @ 0x5A0 = 1440
    ORR     r10, r10, #8            @ W2a -> r10
    MOV     r11, #0x76, 28          @ 0x760 = 1888 
    ORR     r11, r11, #4            @ W1a -> r11
    MOV     r12, #0x31, 28          @ W3a -> r12: 0x310 = 784
    MOV     r14, r2
    MOV     r9, #0                  @ 0->i
Hor4x8Loop:
    LDMIA   r14, {r4 - r7}          @ load x4, x5, x6, x7
    HorVer4x1IDCT         
    ADD     r4, r0, #64             @ x0 + 64
    ADD     r5, r1, #64             @ x1 + 64
    SUB     r6, r5, r3              @ x1 - x3 + 64
    SUB     r7, r4, r2              @ x0 - x2 + 64
    ADD     r4, r4, r2              @ x0 + x2 + 64
    ADD     r5, r5, r3              @ x1 + x3 + 64
    MOV     r4, r4, ASR #7
    MOV     r5, r5, ASR #7
    MOV     r6, r6, ASR #7
    MOV     r7, r7, ASR #7
    STMIA   r14!, {r4 - r7}         @ output one row
    ADD     r9, r9, #1              @ i+1 -> i
    CMP     r9, #8                  @ i == 8?
    BNE     Hor4x8Loop

    SUB     r1, r14, #128           @ rgiCoefRecon-=32@
    LDR     r0, [sp]                @ get dst
    LDR     r3, [sp, #8]            @ get iHalfAndiDCTHorzFlags
    MOV     r3, r3, LSR #31         @ iHalf=((U32)iHalfAndiDCTHorzFlags)>>31@
    ADD     r0, r0, r3, LSL #2      @ piDst = piDstBuf->i16 + (iHalf*2)@
    MOV     r2, #16                 @ number * 4 = 16
    BL      g_IDCTPass2_WMV2

	.endif @ WMV_OPT_IDCT_ARM

   .end 

