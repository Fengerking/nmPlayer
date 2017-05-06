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
    .include "wmvdec_member_arm.h"
    .include "xplatform_arm_asm.h" 

	 .text
	 .align 4
	@AREA IDCT_8x8_4x8_8x4, CODE, READONLY
	
	.if WMV_OPT_IDCT_ARM==1

    .globl  _g_IDCTDec_WMV2_Intra            @ intr8x8
    .globl  _g_IDCTDec16_WMV2                @ intra8x8X8    
    .globl  _g_IDCTPass1_WMV2                @ interHor
    .globl  _g_IDCTPass2_WMV2                @ interVer
    .globl  _g_IDCTDec_WMV2_16bit            @ inter8x8
    .globl  _g_8x4IDCTDec_WMV2_16bit         @ inter8x4
    .globl  _g_4x8IDCTDec_WMV2_16bit         @ inter4x8

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   .macros
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    .if ARCH_V3 == 1
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        .macro  STORETWO16bits	@dstRN, offset, srcRN1, srcRN2 @ Hi: RN2@ Lo: RN1
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        mov     $2, $2, lsl #16
        mov     $2, $2, lsr #16
        orr     $2, $2, $3, lsl #16
        str     $2, [$0, $1]
        .endmacro

        .macro STORETOne16bits	@dstRN, srcRN, offsetLo, offsetHi 
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        mvnne    $1, [$0, $2]
        mov      $1, $1, asr #8
        mvnne    $1, [$0, $3]
        .endmacro

        .macro  LOADTWO16bits	@srcRN, offset, dstRN1, dstRN2 @ Hi: RN2@ Lo: RN1
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        ldr     $2, [$0, $1]
        mov     $3, $2, asr #16
        mov     $2, $2, lsl #16
        mov     $2, $2, asr #16
        .endmacro

        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        .macro  LOADONE16bitsHi		@srcRN, offset, dstRN
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        ldr     $2, [$0, $1]
        mov     $2, $2, asr #16
        .endmacro

        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        .macro  LOADONE16bitsLo		@srcRN, offset, dstRN
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        ldr     $2, [$0, $1]
        mov     $2, $2, lsl #16
        mov     $2, $2, asr #16
        .endmacro
    .endif @ //ARCH_V3

    @@@@@@@@@@@@@@@@@@
    .macro  SATURATION8		@srcRN
    @@@@@@@@@@@@@@@@@@
    cmp     $0, #0
	movmi   $0, #0
	cmppl   $0, #0xFF  @ 0xFF = 255
	movgt   $0, #0xFF  @ 0xFF = 255
    .endmacro

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro  Hor8x1IDCT32BitLoad		@srcRN @ result: r0 - r7       
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    ldr     r0, [$0]
    ldr     r4, [$0, #4]
    ldr     r3, [$0, #8]
    ldr     r7, [$0, #12]
    ldr     r1, [$0, #16]
    ldr     r6, [$0, #20]
    mov     r1, r1, lsl #11
    ldr     r2, [$0, #24]
    ldr     r5, [$0, #28]
    mov     r10, r0, lsl #11
    add     r0, r10, #128 
    .endmacro

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro  Hor8x1IDCT16BitLoad		@srcRN @ result: r0 - r7       
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .if ARCH_V3 == 1
        LOADTWO16bits $0, #0, r0, r4
        LOADTWO16bits $0, #4, r3, r7
        LOADTWO16bits $0, #8, r1, r6
        LOADTWO16bits $0, #12, r2, r5
    .else
        ldrsh   r0, [$0]    
        ldrsh   r4, [$0, #2]   
        ldrsh   r3, [$0, #4]
        ldrsh   r7, [$0, #6]
        ldrsh   r1, [$0, #8]
        ldrsh   r6, [$0, #10]
        ldrsh   r2, [$0, #12]
        ldrsh   r5, [$0, #14]
    .endif @// ARCH_V3
    mov     r1, r1, lsl #11
    mov     r10, r0, lsl #11
    add     r0, r10, #128 
    .endmacro

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro  Hor8x1IDCT_stage1234            @Input: r0 - r7@ Ouput: r5 - r12
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @@@@@@@@@@@ first stage
    mov     r8, #0x8D, 30           @ 0x234 = 564
    orr     r8, r8, #1              @ W7 -> r8  
    add     r9, r4, r5              @ x4 + x5    
    mul     r8, r9, r8              @ W7 * (x4 + x5)

	mov     r10, #0x96, 28          @ 0x960 = 2400
	orr     r10, r10, #8            @ W3 -> r10               
    add     r11, r6, r7             @ x6 + x7 
    mul     r10, r11, r10           @ W3 * (x6 + x7)

	mov     r9, #0x8E, 28           @ 0x8E0 = 2272
	orr     r9, r9, #4              @ W1_W7 -> r9            
    mla     r4, r9, r4, r8          @ x4 = x8 + W1_W7 * x4@

    mov     r11, #0xC7, 30          @ 0x31C = 796
	orr     r11, r11, #3            @ W3_W5 -> r11       
    mul     r6, r11, r6             @ x6: W3_W5 * x6@

    mov     r9, #0x35, 26           @ 0xD40 = 3392
	orr     r9, r9, #0xE            @ 0xE = 14
    mul     r5, r9, r5              @ x5 = W1pW7 * x5@

    sub     r6, r10, r6             @ x6 = x8 - W3_W5 * x6@

	mov     r11, #0xFB, 28          @ 0xFB0 = 4016
	orr     r11, r11, #1            @ W3pW5 -> r11
    mul     r7, r11, r7             @ x7 = x8 - W3pW5 * x7@
    
    sub     r5, r8, r5              @ x5 = x8 - W1pW7 * x5@
    @sub     r7, r10, r7            @ x7 = x8 - W3pW5 * x7@ // should move down

    @@@@@@@@@@@@@ second stage
    add     r8, r0, r1              @ x8 = x0 + x1@
    sub     r0, r0, r1              @ x0 -= x1@     
    sub     r7, r10, r7             @ x7 = x8 - W3pW5 * x7@ // should move down
    add     r9, r3, r2              @ x3 + x2 -> r9
    
    mov     r10, #0x45, 28          @ 0x450 = 1104
	orr     r10, r10, #4            @ W6 -> r10
    mul     r1, r10, r9             @ W6 * (x3 + x2) -> r1

    mov     r11, #0x3B, 26          @ 0xEC0 = 3776
	orr     r11, r11, #8            @ W2pW6 -> r11
    mul     r2, r11, r2             @ x2 = W2pW6 * x2@

    mov     r10, #0x62, 28          @ r10 : W2_W6 -> 0x620 = 1568
    mla     r3, r10, r3, r1         @ x3 = x1 + W2_W6 * x3 

    sub     r2, r1, r2              @ x2 = x1 - W2pW6 * x2@    
    add     r1, r4, r6              @ x1 = x4 + x6@
    sub     r4, r4, r6              @ x4 -= x6@
    add     r6, r5, r7              @ x6 = x5 + x7@
    sub     r5, r5, r7              @ x5 -= x7@
 
    @@@@@@@@@@@@@@ third stage
    add     r7, r8, r3              @ x7 = x8 + x3@
    sub     r8, r8, r3              @ x8 -= x3@
    add     r3, r0, r2              @ x3 = x0 + x2@
    sub     r0, r0, r2              @ x0 -= x2@
    mov     r12, #0xB5              @ 0xB5 = 181 -> r12
    mov     r11, #128               @ 128 -> r11
    add     r9, r4, r5              @ x4 + x5 -> r9
    mla     r2, r12, r9, r11        @ 181L * (x4 + x5) + 128L

    sub     r10, r4, r5             @ x4 - x5 -> r11
    mla     r4, r12, r10, r11

    @@@@@@@@@@@@@@@ fourth stage
    sub     r12, r7, r1
    mov     r12, r12, asr #8        @ blk [7] = (PixelI32) ((x7 - x1) >> 8)@
    sub     r11, r3, r2, asr #8
    mov     r11, r11, asr #8        @ blk [6] = (PixelI32) ((x3 - x2) >> 8)@
    sub     r10, r0, r4, asr #8
    mov     r10, r10, asr #8        @ blk [5] = (PixelI32) ((x0 - x4) >> 8)@
    sub     r9, r8, r6
    mov     r9, r9, asr #8          @ blk [4] = (PixelI32) ((x8 - x6) >> 8)@
    add     r8, r8, r6
    mov     r8, r8, asr #8          @ blk [3] = (PixelI32) ((x8 + x6) >> 8)@
    add     r5, r7, r1
    mov     r5, r5, asr #8          @ blk [0] = (PixelI32) ((x7 + x1) >> 8)@
    add     r6, r3, r2, asr #8
    mov     r6, r6, asr #8          @ blk [1] = (PixelI32) ((x3 + x2) >> 8)@
    add     r7, r0, r4, asr #8
    mov     r7, r7, asr #8          @ blk [2] = (PixelI32) ((x0 + x4) >> 8)@         
    .endmacro

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro  Ver1x8IDCT16BitLoad		@srcRN @ result: r0 - r7       
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .if ARCH_V3 == 1
        and     r0, $0, #3
        cmp     r0, #0
        bne     LoadHi
        LOADONE16bitsLo $0, #0, r0
        LOADONE16bitsLo $0, #64, r1
        LOADONE16bitsLo $0, #96, r2
        LOADONE16bitsLo $0, #32, r3
        LOADONE16bitsLo $0, #16, r4
        LOADONE16bitsLo $0, #112, r5
        LOADONE16bitsLo $0, #80, r6
        LOADONE16bitsLo $0, #48, r7
        b       OutOfVerLoad
LoadHi:
        sub     $0, $0, #2
        LOADONE16bitsHi $0, #0, r0
        LOADONE16bitsHi $0, #64, r1
        LOADONE16bitsHi $0, #96, r2
        LOADONE16bitsHi $0, #32, r3
        LOADONE16bitsHi $0, #16, r4
        LOADONE16bitsHi $0, #112, r5
        LOADONE16bitsHi $0, #80, r6
        LOADONE16bitsHi $0, #48, r7
        add     $0, $0, #2
    .else    
        ldrsh   r0, [$0]
        ldrsh   r1, [$0, #64]
        ldrsh   r2, [$0, #96]
        ldrsh   r3, [$0, #32]
        ldrsh   r4, [$0, #16]
        ldrsh   r5, [$0, #112]
        ldrsh   r6, [$0, #80]
        ldrsh   r7, [$0, #48] 
    .endif
OutOfVerLoad:
    mov     r0, r0, lsl #8          @ x0 = (Int)((Int)blk0[i] << 8) + 8192L@
    add     r0, r0, #8192
    mov     r1, r1, lsl #8          @ x1 = (Int)blk4[i] << 8@
    .endmacro

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro  Ver1x8IDCT32BitLoad		@srcRN, iOffsetToNextRowForDCT @result: r0 - r7       
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    ldr     r0, [$0], $1
    ldr     r4, [$0], $1
    ldr     r3, [$0], $1
    ldr     r7, [$0], $1  
    ldr     r1, [$0], $1
    ldr     r6, [$0], $1
    ldr     r2, [$0], $1
    ldr     r5, [$0], $1
    mov     r0, r0, lsl #8          @ x0 = (Int)((Int)blk0[i] << 8) + 8192L@
    add     r0, r0, #8192
    mov     r1, r1, lsl #8          @ x1 = (Int)blk4[i] << 8@
    .endmacro

    @@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro Ver1x8IDCT_stage123
    @@@@@@@@@@@@@@@@@@@@@@@@@@
    @@@@@@@@ first stage
    mov     r9, #4                  @ 4 --> r9
    mov     r10, #0x8D, 30          @ 0x234 = 564
	orr     r10, r10, #1            @ r10: W7
    add     r11, r4, r5             @ x4 + x5 -> r9
    mla     r8, r10, r11, r9        @ x8 = W7 * (x4 + x5) + 4@
    
	mov     r10, #0x96, 28          @ 0x960 = 2400
	orr     r10, r10, #8            @ r10: W3
    add     r11, r6, r7             @ x6 + x7 -> r11
    mla     r11, r10, r11, r9       @ r11 -> W3 * (x6 + x7) + 4@

    mov     r10, #0x8E, 28          @ 0x8E0 = 2272
	orr     r10, r10, #4            @ W1_W7 -> r11
    mla     r4, r10, r4, r8         @ x4 = (x8 + W1_W7 * x4) @     
    
	mov     r10, #0x35, 26          @ 0xD40 = 3392
	orr     r10, r10, #0xE          @ W1pW7 -> r10 0xE = 14
    mul     r5, r10, r5             @ W1pW7 * x5

    mov     r10, #0xC7, 30          @ 0x31C = 796
	orr     r10, r10, #3            @ W3_W5 -> r10
    mul     r6, r10, r6             @ r6 : W3_W5 * x6 

    mov     r10, #0xFB, 28          @ 0xFB0 = 4016
	orr     r10, r10, #1            @ W3pW5 -> r10
    mul     r7, r10, r7             @ r7 : W3pW5 * x7

    sub     r5, r8, r5              @ x5 = (x8 - W1pW7 * x5) >> 3@
    mov     r5, r5, asr #3          @ r8 is free
    sub     r6, r11, r6             @ x8 - W3_W5 * x6
    sub     r7, r11, r7             @ r7 : x8 - W3pW5 * x7       @ 
    
    @@@@@@@@@@ second stage
    add     r8, r0, r1              @ x8 = x0 + x1@r9 is still 4
    sub     r0, r0, r1              @ x0 -= x1@
    add     r1, r3, r2              @ x3 + x2 -> r1

	mov     r10, #0x45, 28          @ 0x450 = 1104
    orr     r10, r10, #4            @ W6 : 1108
    mla     r9, r10, r1, r9         @ r9 <-- W6 * (x3 + x2) + 4@ 
     
    mov     r11, #0x3B, 26          @ 0xEC0 = 3776
	orr     r11, r11, #8            @ W2pW6 -> r11
    mul     r2, r11, r2             @ W2pW6 * x2 -> r2

    mov     r10, #0x62, 28          @ W2_W6 -> r10: 0x620 = 1568
    mla     r3, r10, r3, r9         @ x3 = (x1 + W2_W6 * x3)@

    mov     r4, r4, asr #3          @ x4 = (x8 + W1_W7 * x4) >> 3
    add     r1, r4, r6, asr #3      @ x1 = x4 + x6@
    sub     r4, r4, r6, asr #3      @ x4 -= x6@
    add     r6, r5, r7, asr #3      @ x6 = x5 + x7@
    sub     r5, r5, r7, asr #3      @ x5 -= x7@
    sub     r2, r9, r2              @ x2 = (x1 - W2pW6 * x2)

    @@@@@@@@@@@ third stage
    add     r7, r8, r3, asr #3      @ x7 = x8 + x3@
    sub     r8, r8, r3, asr #3      @ x8 -= x3@
    add     r3, r0, r2, asr #3      @ x3 = x0 + x2@
    sub     r0, r0, r2, asr #3      @ x0 -= x2@
    mov     r10, #181               @ 181 -> r10
    mov     r11, #128               @ 128 -> r11
    add     r2, r4, r5              @ x4 + x5 -> r2
    mla     r2, r10, r2, r11        @ x2 = (Int) (181L * (x4 + x5) + 128L)
 
    sub     r4, r4, r5              @ x4 - x5 -> r4
    mla     r4, r10, r4, r11        @ x4 = (Int) (181L * (x4 - x5) + 128L)
    
    mov     r2, r2, asr #8          @ x2 = (Int) (181L * (x4 + x5) + 128L) >> 8@
    mov     r4, r4, asr #8          @ x4 = (Int) (181L * (x4 - x5) + 128L) >> 8@
    .endmacro

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro Ver1x8IDCT_stage4_Store		@destRN @
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @@@@@@@@@@@@ fourth stage
    add     r9, r7, r1              @ x7 + x1 -> r9
    add     r10, r3, r2             @ x3 + x2 -> r10
    mov     r9, r9, asr #14         @ (PixelI32) ((x7 + x1) >> 14)@
    .if ARCH_V3 == 1
        STORETOne16bits $0, r9, #0, #1
    .else
        strh    r9, [$0]               @ blk0[i] = (PixelI32) ((x7 + x1) >> 14)@
    .endif
    mov     r10, r10, asr #14       @ r10: (x3 + x2) >> 14
    .if ARCH_V3 == 1
        STORETOne16bits $0, r10, #16, #17
    .else
        strh    r10, [$0, #16]         @ blk1[i] = (PixelI32) ((x3 + x2) >> 14)@
    .endif
    add     r9, r0, r4              @ r9 : x0 + x4
    mov     r9, r9, asr #14         @ (x0 + x4) >> 14
    .if ARCH_V3 == 1
        STORETOne16bits $0, r9, #32, #33
    .else
        strh    r9, [$0, #32]          @ blk2[i] = (PixelI32) ((x0 + x4) >> 14)@
    .endif
    add     r10, r8, r6             @ r10: x8 + x6
    mov     r10, r10, asr #14       @ (x8 + x6) >> 14
    .if ARCH_V3 == 1
        STORETOne16bits $0, r10, #48, #49
    .else
        strh    r10, [$0, #48]         @ blk3[i] = (PixelI32) ((x8 + x6) >> 14)@
    .endif
    sub     r9, r8, r6              @ r9: x8 - x6
    mov     r9, r9, asr #14         @ ((x8 - x6) >> 14)@
    .if ARCH_V3 == 1
        STORETOne16bits $0, r9, #64, #65
    .else
        strh    r9, [$0, #64]         @ blk4[i] = (PixelI32) ((x8 - x6) >> 14)@
    .endif
    sub     r10, r0, r4             @ r10: x0 - x4
    mov     r10, r10, asr #14       @ (x0 - x4) >> 14
    .if ARCH_V3 == 1
        STORETOne16bits $0, r10, #80, #81
    .else
        strh    r10, [$0, #80]        @ blk5[i] = (PixelI32) ((x0 - x4) >> 14)@             
    .endif
    sub     r9, r3, r2              @ r9: x3 - x2
    mov     r9, r9, asr #14         @ (x3 - x2) >> 14
    .if ARCH_V3 == 1
        STORETOne16bits $0, r9, #96, #97
    .else
        strh    r9, [$0, #96]         @ blk6[i] = (PixelI32) ((x3 - x2) >> 14)@
    .endif
    sub     r10, r7, r1             @ r10: x7 - x1
    mov     r10, r10, asr #14       @ (x7 - x1) >> 14
    .if ARCH_V3 == 1
        STORETOne16bits $0, r10, #112, #113
    .else
        strh    r10, [$0, #112]        @ blk7[i] = (PixelI32) ((x7 - x1) >> 14)@
    .endif
    .endmacro
        
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro HorVer4x1IDCT     @ input: r4 - r7@ output: r0 - r3@ r10 - r12L: coefs
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    add     r0, r4, r6              @ x4 + x6
    mul     r0, r10, r0             @ x0 = (x4 + x6)*W2a@
    sub     r1, r4, r6              @ x4 - x6
    mul     r2, r5, r11             @ x5*W1a -> r2: 
    mul     r3, r7, r11             @ x7*W1a -> r3
    mul     r4, r5, r12             @ x5*W3a -> r4: 
    mul     r1, r10, r1             @ x1 = (x4 - x6)*W2a@ r4, r6 free
    mla     r2, r7, r12, r2         @ x2 = x5*W1a + x7*W3a@
    sub     r3, r4, r3              @ x3 = x5*W3a - x7*W1a@
    .endmacro

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro Ver8x4IDCT
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    ldr     r9, [sp, #8]            @ get Half&HorFlag
    ldr     r14, [sp, #4]           @ get src
    ldr     r8, [sp]                @ get dst
    mov     r9, r9, lsr #31         @ iHalf=((U32)iHalfAndiDCTHorzFlags)>>31@
    add     r8, r8, r9, lsl #6      @ piDst = piDstBuf->i16 + (iHalf*32)@  
    mov     r10, #0x5A, 28          @ 0x5A0 = 1440
    orr     r10, r10, #8            @ W2a -> r10
    mov     r11, #0x76, 28          @ 0x760 = 1888 
    orr     r11, r11, #4            @ W1a -> r11
    mov     r12, #0x31, 28          @ W3a -> r12: 0x310 = 784
    mov     r9, #0                  @ 0 -> i
Ver8x4Loop:
    .if ARCH_V3 == 1
        LOADONE16bitsLo r14, #0, r4
        LOADONE16bitsLo r14, #32, r5
        LOADONE16bitsLo r14, #64, r6
        LOADONE16bitsLo r14, #96, r7
    .else
        ldrsh   r4, [r14]               @ x4
        ldrsh   r5, [r14, #32]          @ x5
        ldrsh   r6, [r14, #64]          @ x6
        ldrsh   r7, [r14, #96]          @ x7
    .endif
    HorVer4x1IDCT         
    add     r0, r0, #2, 18          @ x0 + 32768L
    add     r1, r1, #2, 18          @ x1 + 32768L
    add     r4, r0, r2              @ r4: x0 + x2 + 32768L
    add     r5, r1, r3              @ r5: x1 + x3 + 32768L
    sub     r6, r1, r3              @ r6: x1 - x3 + 32768L
    sub     r7, r0, r2              @ r7: x0 - x2 + 32768L
    and     r0, r9, #1
	mov     r0, r0, lsl #6
    add     r0, r0, r9, asr #1      @ idex=(i>>1)+((i&1)<<6)@
    add     r0, r8, r0, lsl #1      @ get blk0
    mov     r4, r4, asr #16
    .if ARCH_V3 == 1
        STORETOne16bits r0, r4, #0, #1
    .else
        strh    r4, [r0]                @ blk0[i] = (PixelI32)((x0 + x2 + 32768L)>>16)@
    .endif
    mov     r5, r5, asr #16
    .if ARCH_V3 == 1
        STORETOne16bits r0, r5, #16, #17
    .else
        strh    r5, [r0, #16]           @ blk1[i] = (PixelI32)((x1 + x3 + 32768L)>>16)@
    .endif
    mov     r6, r6, asr #16
    .if ARCH_V3 == 1
        STORETOne16bits r0, r6, #32, #33
    .else
        strh    r6, [r0, #32]           @ blk2[i] = (PixelI32)((x1 - x3 + 32768L)>>16)@
    .endif
    mov     r7, r7, asr #16
    .if ARCH_V3 == 1
        STORETOne16bits r0, r7, #48, #49
    .else
        strh    r7, [r0, #48]           @ blk3[i] = (PixelI32)((x0 - x2 + 32768L)>>16)@
    .endif
    add     r14, r14, #4            @ next column
    add     r9, r9, #1              @ i+1 -> i
    cmp     r9, #8
    bne     Ver8x4Loop            
    .endmacro

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA	|.text|, CODE
		@ .text

    WMV_LEAF_ENTRY g_IDCTDec_WMV2_Intra
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    stmfd	sp!, {r0, r1, r4 - r12, r14}    @  r0: dest (sp+4)
                                        @  r1: iOffsetToNextRowForIDCT (sp+8)
                                        @  r2: src    
    FRAME_PROFILE_COUNT
    mov     r14, r2                 @ r2 -> r14: blk[]
    mov		r9, #0                  @ 0 -> r9	             				
    add     sp, sp, #-4             @ save i
    str		r9, [sp]                @ 0 -> i            (sp)

LoopIntra8x8Hor:
    Hor8x1IDCT32BitLoad r14
    Hor8x1IDCT_stage1234            @ r14 -> src                  
    stmia   r14, {r5 - r12}         @ output
    ldr     r9, [sp]                @ next row
    add     r14, r14, #32
    add     r9, r9, #1
    str     r9, [sp]
    cmp     r9, #8
    blt     LoopIntra8x8Hor

    mov     r12, #8                 @ 8 -> i
    sub     r14, r14, #256          @ restore r14: blk[] src
LoopIntra8x8Ver: 
    mov     r10, r14  
    mov     r11, #32
    Ver1x8IDCT32BitLoad r10, r11
    Ver1x8IDCT_stage123
    @@@@@@@@@@@@ fourth stage
    add     r5, r7, r1             
    mov     r5, r5, asr #14         @ x5 = (x7 + x1) >> 14@
    sub     r1, r7, r1
    mov     r1, r1, asr #14         @ x1 = (x7 - x1) >> 14@
    add     r7, r3, r2
    mov     r7, r7, asr #14         @ x7 = (x3 + x2) >> 14@     
    sub     r2, r3, r2          
    mov     r2, r2, asr #14         @ x2 = (x3 - x2) >> 14@
    add     r3, r0, r4
    mov     r3, r3, asr #14         @ x3 = (x0 + x4) >> 14@
    sub     r4, r0, r4
    mov     r4, r4, asr #14         @ x4 = (x0 - x4) >> 14@
    add     r0, r8, r6
    mov     r0, r0, asr #14         @ x0 = (x8 + x6) >> 14@
    sub     r6, r8, r6              
    mov     r6, r6, asr #14         @ x6 = (x8 - x6) >> 14@
    orr     r9, r1, r5              @ iTest: r9
    orr     r9, r9, r7  
    orr     r9, r9, r2    
    orr     r9, r9, r3
    orr     r9, r9, r4
    orr     r9, r9, r0
    orr     r9, r9, r6
    bics    r9, r9, #0xFF           @ 0xFF = 255
    bne     Saturate8Pixels
Output8Bytes:
    ldr     r10, [sp, #4]           @ get src
    ldr     r8, [sp, #8]            @ get iOffsetToNextRowForIDCT
    mov     r9, r10                 @ src -> r9
    
	strb    r5, [r9], r8           @ *blk0++ = x5@
    strb    r7, [r9], r8           @ *blk1++ = x7@
    strb    r3, [r9], r8           @ *blk2++ = x3@
    strb    r0, [r9], r8           @ *blk3++ = x0@
    strb    r6, [r9], r8           @ *blk4++ = x6@
    strb    r4, [r9], r8           @ *blk5++ = x4@
    strb    r2, [r9], r8           @ *blk6++ = x2@
    strb    r1, [r9], r8           @ *blk7++ = x1@
	
    add     r10, r10, #1            @ next column
    str     r10, [sp, #4]           @ save src     
    add     r14, r14, #4
    sub     r12, r12, #1            @ i-1 -> i
    cmp     r12, #0
    bne     LoopIntra8x8Ver

    add     sp, sp, #12             
    ldmfd   sp!, {r4 - r12, PC}
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

    b Output8Bytes

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    WMV_LEAF_ENTRY g_IDCTDec16_WMV2
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	stmfd	sp!, {r0 - r1, r3, r4 - r12, r14} @ sp+8: src@ sp+12: dst@ sp+16: HorzFlag@
    FRAME_PROFILE_COUNT
    add     sp, sp, #-4             @ store VertFlag, i 
    mov		r0, #0                  @ 0 -> r0	          
	str		r0, [sp], #-4			@ 0 -> VertFlag     (sp+4)   				
    str		r0, [sp]                @ 0 -> i            (sp)
    mov     r9, r0                  @ r9 -> i
    mov     r14, r1                 @ src -> r14
LoopHorX8:
	ldr		r12, [sp, #16]			@ r12: HorzFlag@ r9: i
    mov		r11, #1                 @ 1 -> r11
    ands    r12, r12, r11, lsl r9   @ iDCTHorzFlags & (1 << i)     
	bne     DoHorX8
	@@@@@@@@@@@@@@ Horz short cut
    .if ARCH_V3 == 1
        LOADONE16bitsLo r14, #0, r0
    .else
        ldrsh   r0, [r14]
    .endif
    cmp     r0, #0
    beq     NextHorX8
    @mov     r0, r0, lsl #3          @ [0] - [7] <-- rgiCoefRecon [0] << 3
    mov     r0, r0, lsl #19          @(3 + 16 = 19)
    orr     r0, r0, r0, lsr #16
    str     r0, [r14]
    str     r0, [r14, #4]
    str     r0, [r14, #8]
    str     r0, [r14, #12]
    str     r9, [sp, #4]            @ i -> iDCRVertFlag
    b       NextHorX8
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
DoHorX8:
    str     r9, [sp, #4]            @ i -> iDCRVertFlag
    Hor8x1IDCT16BitLoad r14
    Hor8x1IDCT_stage1234            @ r14 -> src
    .if ARCH_V3 == 1
        STORETWO16bits r14, #0, r5, r6
        STORETWO16bits r14, #4, r7, r8
        STORETWO16bits r14, #8, r9, r10
        STORETWO16bits r14, #12, r11, r12
    .else
        strh    r5, [r14]               @ output
        strh    r6, [r14, #2]
        strh    r7, [r14, #4]
        strh    r8, [r14, #6]
        strh    r9, [r14, #8]
        strh    r10,[r14, #10]
        strh    r11,[r14, #12]
        strh    r12,[r14, #14]
    .endif @//ARCH_V3
NextHorX8:
    ldr     r9, [sp]
    add     r14, r14, #16
    add     r9, r9, #1
    str     r9, [sp]
    cmp     r9, #8
    blt     LoopHorX8

@@@@@@@@@@@@ Vertical transform
    ldr     r10, [sp, #4]           @ load VertFlag
    ldr     r14, [sp, #12]          @ src -> r14
    cmp     r10, #0                 @ if (iDCTVertFlag == 0)
    beq     VerShortCutX8           @ jump to vertical short cut
    ldr     r12, [sp, #8]           @ dst -> r12              
    mov     r9, #0                  @ 0 -> i
    str     r9, [sp]
LoopVerX8:      
    Ver1x8IDCT16BitLoad r14
    Ver1x8IDCT_stage123 
    Ver1x8IDCT_stage4_Store r12            
    ldr     r9, [sp]
    add     r14, r14, #2
    add     r12, r12, #2
    add     r9, r9, #1
    str     r9, [sp]
    cmp     r9, #8
    bne     LoopVerX8
    
EndVerX8:
    add     sp, sp, #20 
    ldmfd   sp!, {r4 - r12, PC}
    WMV_ENTRY_END

    @@@@@@@@@@@@ Vertical short cut 
VerShortCutX8:
    .if ARCH_V3 == 1
        LOADTWO16bits r14, #0, r0, r1
        LOADTWO16bits r14, #4, r2, r3
        LOADTWO16bits r14, #8, r4, r5
        LOADTWO16bits r14, #12, r6, r7
    .else
        ldrsh   r0, [r14]               @ (blk0[i] + 32) >> 6@
        ldrsh   r1, [r14, #2]
        ldrsh   r2, [r14, #4]
        ldrsh   r3, [r14, #6]
        ldrsh   r4, [r14, #8]
        ldrsh   r5, [r14, #10]
        ldrsh   r6, [r14, #12]
        ldrsh   r7, [r14, #14]
    .endif @//ARCH_V3
    add     r0, r0, #32
    mov     r0, r0, asr #6          
    add     r1, r1, #32
    mov     r1, r1, asr #6 
    add     r2, r2, #32
    mov     r2, r2, asr #6
    add     r3, r3, #32
    mov     r3, r3, asr #6
    add     r4, r4, #32
    mov     r4, r4, asr #6
    add     r5, r5, #32
    mov     r5, r5, asr #6
    add     r6, r6, #32
    mov     r6, r6, asr #6
    add     r7, r7, #32
    mov     r7, r7, asr #6
  @  ldr     r12, =0xffff            @ mask
	mov		r11, #0xff
	orr		r11, r11, lsl #8
    ldr     r14, [sp, #8]           @ dst -> r14
    and     r0, r0, r12
    orr     r8, r0, r1, lsl #16     @ r8: r1r0
    and     r2, r2, r12
    orr     r9, r2, r3, lsl #16     @ r9: r3r2
    and     r4, r4, r12
    orr     r10, r4, r5, lsl #16    @ r10: r5r4
    and     r6, r6, r12
    orr     r11, r6, r7, lsl #16    @ r11: r7r6 
    stmia   r14!, {r8 - r11}        @ output one row
    stmia   r14!, {r8 - r11}        @ output one row
    stmia   r14!, {r8 - r11}        @ output one row
    stmia   r14!, {r8 - r11}        @ output one row
    stmia   r14!, {r8 - r11}        @ output one row
    stmia   r14!, {r8 - r11}        @ output one row
    stmia   r14!, {r8 - r11}        @ output one row
    stmia   r14!, {r8 - r11}        @ output one row
    add     sp, sp, #20 
    ldmfd   sp!, {r4 - r12, PC}

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

		@AREA	|.text|, CODE
		@ .text
    WMV_LEAF_ENTRY g_IDCTPass1_WMV2
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    stmfd	sp!, {r0 - r2, r14} @ sp+8: src/dst@ sp+12: num@ sp+16: HorzFlag@
    FRAME_PROFILE_COUNT
    mov     r14, r0                 @ r14 -> src/dst blk[]
    add     sp, sp, #-4             @ store VertFlag, i 
    mov		r0, #0                  @ 0 -> r0	          
	str		r0, [sp], #-4			@ 0 -> VertFlag     (sp+4)   				
	mov     r9, r0                  @ r9 -> i
    str		r9, [sp]                @ 0 -> i            (sp)
LoopPass1:
	ldr		r12, [sp, #16]			@ r12: HorzFlag 				@ r9: i
    mov		r11, #1                 @ 1 -> r11
    ands    r12, r12, r11, lsl r9   @ iDCTHorzFlags & (1 << i)     
	bne     DoPass1
	@@@@@@@@@@@@@@ Horz short cut
    ldr     r0, [r14]
    cmp     r0, #0
    beq     NextPass1
    mov     r0, r0, lsl #3          @ [0] - [7] <-- rgiCoefRecon [0] << 3
    str     r0, [r14]
    str     r0, [r14, #4]
    str     r0, [r14, #8]
    str     r0, [r14, #12]
    str     r0, [r14, #16]
    str     r0, [r14, #20]
    str     r0, [r14, #24]
    str     r0, [r14, #28]
    str     r9, [sp, #4]            @ i -> iDCRVertFlag
    b       NextPass1
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
DoPass1:
    str     r9, [sp, #4]            @ i -> iDCRVertFlag
    Hor8x1IDCT32BitLoad r14
    Hor8x1IDCT_stage1234            @ r14 -> src                  
    stmia   r14, {r5 - r12}         @ output
NextPass1:
    ldr     r9, [sp]
    add     r14, r14, #32
    add     r9, r9, #1
    str     r9, [sp]
    ldr     r8, [sp, #12]           @ get number
    cmp     r9, r8
    blt     LoopPass1

    ldr     r0, [sp, #4]
    add     sp, sp, #20
    ldmfd   sp!, {PC}
    WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA	|.text|, CODE
		@ .text
    WMV_LEAF_ENTRY g_IDCTPass2_WMV2
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    stmfd	sp!, {r0 - r2} @ sp+8: dst@ sp+12: src@ sp+16: number*4@
    FRAME_PROFILE_COUNT_SAVE_LR

    add     sp, sp, #-4
    mov     r8, #0                  @ 0 -> i, k  
    str     r8, [sp], #-4           @ 0 -> k    [sp+4]
DoPass2:
    ldr     r14, [sp, #12]          @ get src
    ldr     r12, [sp, #8]           @ get dst
    add     r14, r14, r8, lsl #2    @ rgiCoefRecon = rgiCoefReconOri + k@
    add     r12, r12, r8, lsl #7    @ PixelI16 __huge *blk0 = blk + k*64@
    mov     r9, #0                  @ 0 -> i
    str     r9, [sp]                @ 0 -> i    [sp]  
LoopPass2:
    mov     r10, r14    
    ldr     r11, [sp, #16]
    Ver1x8IDCT32BitLoad r10, r11
    Ver1x8IDCT_stage123
    Ver1x8IDCT_stage4_Store r12

    add     r14, r14, #8            @ next src column
    add     r12, r12, #2            @ next dst column
    ldr     r9, [sp]                @ get i
    add     r9, r9, #1              @ i+1 -> i
    str     r9, [sp]                @ save i
    ldr     r10, [sp, #16]          @ number -> r10
    cmp     r9, r10, asr #3         @ i =? number/2
    bne     LoopPass2               @ next column

    ldr     r8, [sp, #4]            @ get k
    add     r8, r8, #1              @ k+1 -> k
    str     r8, [sp, #4]            @ save k
    cmp     r8, #2
    bne     DoPass2                 @ odd column

    add     sp, sp, #32             @ all the way out of g_IDCTDec_WMV2_16bit
    ldmfd   sp!, {r4 - r12, PC}
    WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA	|.text|, CODE
		@ .text
    WMV_LEAF_ENTRY g_IDCTDec_WMV2_16bit
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    stmfd	sp!, {r0 - r1, r3, r4 - r12, r14} @ r0: dst [sp], r1: src [sp+4], r3: HorFlag [sp+8]
    FRAME_PROFILE_COUNT

    mov     r0, r1
    mov     r1, #8
    mov     r2, r3
    bl      _g_IDCTPass1_WMV2

    @ r0 <= VerFlag
    cmp     r0, #0
    beq     VerShortCut8x8     

    ldmia   sp, {r0 - r1}
    mov     r2, #32                 @ number * 4 = 32
    bl      _g_IDCTPass2_WMV2

VerShortCut8x8:
    ldmia   sp, {r12, r14}          @ get dst and src
    ldmia   r14, {r0 - r7}          @ get rgiCoefRecon[0] - [7]
    add     r0, r0, #32
    add     r1, r1, #32
    add     r2, r2, #32
    add     r3, r3, #32
    add     r4, r4, #32
    add     r5, r5, #32
    add     r6, r6, #32
    add     r7, r7, #32
    mov     r0, r0, asr #6          
    mov     r1, r1, asr #6 
    mov     r2, r2, asr #6
    mov     r3, r3, asr #6
    mov     r4, r4, asr #6
    mov     r5, r5, asr #6
    mov     r6, r6, asr #6
    mov     r7, r7, asr #6
  @  ldr     r11, =0xffff            @ mask
	mov		r11, #0xff
	orr		r11, r11, lsl #8
    and     r0, r0, r11
    orr     r8, r0, r2, lsl #16     @ r8: r0r2
    and     r4, r4, r11
    orr     r9, r4, r6, lsl #16     @ r9: r4r6
    stmia   r12, {r8, r9}           @ output one even row
    add     r12, r12, #16
    stmia   r12, {r8, r9}  
    add     r12, r12, #16
    stmia   r12, {r8, r9}  
    add     r12, r12, #16
    stmia   r12, {r8, r9}  
    add     r12, r12, #16
    stmia   r12, {r8, r9}  
    add     r12, r12, #16
    stmia   r12, {r8, r9}  
    add     r12, r12, #16
    stmia   r12, {r8, r9}  
    add     r12, r12, #16
    stmia   r12, {r8, r9} 
    add     r12, r12, #16
    and     r1, r1, r11
    orr     r8, r1, r3, lsl #16     @ r8: r1r3
    and     r5, r5, r11
    orr     r9, r5, r7, lsl #16     @ r9: r5r7
    stmia   r12, {r8, r9}           @ output one odd row
    add     r12, r12, #16
    stmia   r12, {r8, r9}  
    add     r12, r12, #16
    stmia   r12, {r8, r9}  
    add     r12, r12, #16
    stmia   r12, {r8, r9}  
    add     r12, r12, #16
    stmia   r12, {r8, r9}  
    add     r12, r12, #16
    stmia   r12, {r8, r9}  
    add     r12, r12, #16
    stmia   r12, {r8, r9}  
    add     r12, r12, #16
    stmia   r12, {r8, r9}         

    add     sp, sp, #12             @ all the way out of g_IDCTDec_WMV2_16bit
    ldmfd   sp!, {r4 - r12, PC}
    WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA	|.text|, CODE
		@ .text
    WMV_LEAF_ENTRY g_8x4IDCTDec_WMV2_16bit
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    stmfd	sp!, {r0, r2 - r3, r4 - r12, r14} @ r0: dst [sp], r2: src [sp+4], r3: Half&HorFlag [sp+8]
    FRAME_PROFILE_COUNT
    
    mov     r0, r2
    mov     r1, #4
    ldr     r2, =0x7fffffff
    and     r2, r3, r2              @ iDCTHorzFlags=iHalfAndiDCTHorzFlags&0x7fffffff@
    bl      _g_IDCTPass1_WMV2

    @ r0 <= VerFlag
    cmp     r0, #0
    beq     VerShortCut8x4     

    Ver8x4IDCT    
    add     sp, sp, #12             @ all the way out of g_8x4IDCTDec_WMV2_16bit
    ldmfd   sp!, {r4 - r12, PC}

VerShortCut8x4:
    ldmia   sp, {r12, r14}          @ get dst and src
    ldr     r9, [sp, #8]            @ get Half&HorFlag
    mov     r9, r9, lsr #31         @ iHalf=((U32)iHalfAndiDCTHorzFlags)>>31@
    add     r12, r12, r9, lsl #6    @ piDst = piDstBuf->i16 + (iHalf*32)@  
    ldmia   r14, {r0 - r7}          @ get rgiCoefRecon[0] - [7]
    mov     r8, #0x5A, 28           @ 0x5A0 = 1440
    orr     r8, r8, #8              @ W2a -> r8
    mov     r9, #2, 18              @ 32768L -> r9
    @rgiCoefRecon[i]*W2a + 32768L) >> 16 )
    mla     r0, r8, r0, r9
    mla     r1, r8, r1, r9
    mla     r2, r8, r2, r9
    mla     r3, r8, r3, r9
    mla     r4, r8, r4, r9
    mla     r5, r8, r5, r9
    mla     r6, r8, r6, r9
    mla     r7, r8, r7, r9
    mov     r0, r0, asr #16          
    mov     r1, r1, asr #16 
    mov     r2, r2, asr #16
    mov     r3, r3, asr #16
    mov     r4, r4, asr #16
    mov     r5, r5, asr #16
    mov     r6, r6, asr #16
    mov     r7, r7, asr #16
 @   ldr     r11, =0xffff            @ mask
	mov		r11, #0xff
	orr		r11, r11, lsl #8
    and     r0, r0, r11
    orr     r8, r0, r2, lsl #16     @ r8: r0r2
    and     r4, r4, r11
    orr     r9, r4, r6, lsl #16     @ r9: r4r6
    stmia   r12, {r8, r9}           @ output one even row
    add     r12, r12, #16
    stmia   r12, {r8, r9}  
    add     r12, r12, #16
    stmia   r12, {r8, r9}  
    add     r12, r12, #16
    stmia   r12, {r8, r9}  
    and     r1, r1, r11
    orr     r8, r1, r3, lsl #16     @ r8: r1r3
    and     r5, r5, r11
    orr     r9, r5, r7, lsl #16     @ r9: r5r7
    add     r12, r12, #80           @ to odd field
    stmia   r12, {r8, r9}           @ output one odd row
    add     r12, r12, #16
    stmia   r12, {r8, r9}  
    add     r12, r12, #16
    stmia   r12, {r8, r9}  
    add     r12, r12, #16
    stmia   r12, {r8, r9}     
    add     sp, sp, #12             @ all the way out of g_8x4IDCTDec_WMV2_16bit
    ldmfd   sp!, {r4 - r12, PC}
    WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA	|.text|, CODE
		@ .text
	WMV_LEAF_ENTRY g_4x8IDCTDec_WMV2_16bit
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    stmfd	sp!, {r0, r2 - r3, r4 - r12, r14} @ r0: dst [sp], r2: src [sp+4], r3: Half&HorFlag [sp+8]
    FRAME_PROFILE_COUNT
    
    mov     r10, #0x5A, 28          @ 0x5A0 = 1440
    orr     r10, r10, #8            @ W2a -> r10
    mov     r11, #0x76, 28          @ 0x760 = 1888 
    orr     r11, r11, #4            @ W1a -> r11
    mov     r12, #0x31, 28          @ W3a -> r12: 0x310 = 784
    mov     r14, r2
    mov     r9, #0                  @ 0->i
Hor4x8Loop:
    ldmia   r14, {r4 - r7}          @ load x4, x5, x6, x7
    HorVer4x1IDCT         
    add     r4, r0, #64             @ x0 + 64
    add     r5, r1, #64             @ x1 + 64
    sub     r6, r5, r3              @ x1 - x3 + 64
    sub     r7, r4, r2              @ x0 - x2 + 64
    add     r4, r4, r2              @ x0 + x2 + 64
    add     r5, r5, r3              @ x1 + x3 + 64
    mov     r4, r4, asr #7
    mov     r5, r5, asr #7
    mov     r6, r6, asr #7
    mov     r7, r7, asr #7
    stmia   r14!, {r4 - r7}         @ output one row
    add     r9, r9, #1              @ i+1 -> i
    cmp     r9, #8                  @ i == 8?
    bne     Hor4x8Loop

    sub     r1, r14, #128           @ rgiCoefRecon-=32@
    ldr     r0, [sp]                @ get dst
    ldr     r3, [sp, #8]            @ get iHalfAndiDCTHorzFlags
    mov     r3, r3, lsr #31         @ iHalf=((U32)iHalfAndiDCTHorzFlags)>>31@
    add     r0, r0, r3, lsl #2      @ piDst = piDstBuf->i16 + (iHalf*2)@
    mov     r2, #16                 @ number * 4 = 16
    bl      _g_IDCTPass2_WMV2

	.endif @ WMV_OPT_IDCT_ARM

   @.end 

