@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************

@//************************************************************************
@//
@// Module Name:
@//
@//     effects_arm.s
@//
@// Abstract:
@// 
@//     ARM specific optimization for photo story v2 effects routines
@//
@//     Custom build with 
@//          armasm $(InputDir)\$(InputName).s $(OutDir)\$(InputName).obj
@//     and
@//          $(OutDir)\$(InputName).obj
@// 
@// Author:
@// 
@//     Peter X. Zuo (peterzuo@microsoft.com) May. 10, 2004
@//
@// Revision History:
@//
@//************************************************************************
   #include "../c/voWMVDecID.h"
    .include "wmvdec_member_arm.inc"
    .include "xplatform_arm_asm.h" 

    @AREA COMMON, CODE, READONLY
		.section .text

    .if WMV_OPT_WVP2_ARM==1
    
    .global AffineAddCrossFadeYFrame
    .global AffineAddCrossFadeUVFrame
    .global WMVImageFadeColorQuickFillY
    .global WMVImageFadeColorQuickFillUV
    .global ImageRoll_LineQuickFill
    .global WMVImageGenerateFlipOutputYLine

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    WMV_LEAF_ENTRY AffineAddCrossFadeYFrame
@
@r0: pTmp1
@r1: pTmp2
@r2: pTmp3
@r3: iSize
@
    stmdb   sp!, {r4 - r11, lr}

    mov     r9, #0xff
    mov     r10, r9, lsl #24
    mov     r11, r9, lsl #16
    add     r9, r9, r9, lsl #16


    b       AACFYF_LoopEnd

AACFYF_Start:

    ldr     r4, [r0], #4
    ldr     r5, [r1], #4

    and     r7, r4, r9
    and     r8, r5, r9
    add     r6, r7, r8
    tst     r6, #0x100      @test overflow
    orrne   r6, r6, #0xff   @overflow => set "FF"
    tst     r6, r10         @#0xFF000000    @test overflow high bits
    orrne   r6, r6, r11     @overflow => set "FF"
    and     r6, r6, r9

    and     r4, r9, r4, lsr #8
    and     r5, r9, r5, lsr #8
    add     r7, r4, r5
    tst     r7, #0x100          @test overflow
    orrne   r7, r7, #0xff       @overflow => set "FF"
    tst     r7, r10             @test overflow high bits
    orrne   r7, r7, r11         @overflow => set "FF"
    and     r7, r7, r9

    orr     r6, r6, r7, lsl #8
    str     r6, [r2], #4

AACFYF_LoopEnd:
    subs    r3, r3, #4
    bge     AACFYF_Start

    adds    r3, r3, #4
    ble     AACFYF_End

AACFYF_Loop2Start:
    ldrb    r4, [r0], #1
    ldrb    r5, [r1], #1
    add     r6, r4, r5
    tst     r6, #0x100          @test overflow
    orrne   r6, r6, #0xff       @overflow => set "FF"
    strb    r6, [r2], #1
    subs    r3, r3, #1
    bgt     AACFYF_Loop2Start

AACFYF_End:
    ldmia   sp!, {r4 - r11, pc}

    WMV_ENTRY_END
    @  @ AffineAddCrossFadeYFrame


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    WMV_LEAF_ENTRY AffineAddCrossFadeUVFrame
@
@r0: pTmp1
@r1: pTmp2
@r2: pTmp3
@r3: iSize
@
    stmdb   sp!, {r4 - r12, lr}

    mov     r9, #0xff

    orr     r10, r9, #0x100         @ r10 = 0x1ff

    mov     lr, #3
    add     lr, lr, r9, LSL #2      @ lr = 0x3ff

    add     r9, r9, r9, lsl #16     @ r9  = 0xff00ff

    mov     r12, #1
    mov     r12, r12, LSL #24       @ r12 = 0x1000000

    mov     r11, #0x80              @ 128
    add     r11, r11, r11, lsl #16  @ r11 = 0x800080

    b       AACFUVF_LoopEnd

AACFUVF_Start:

    ldr     r4, [r0], #4
    ldr     r5, [r1], #4
@
@calc the low 8 bits
@
    and     r7, r4, r9
    and     r8, r5, r9
    add     r6, r7, r8

    add     r6, r6, r11         @x-128 = (x+128) - 256
    tst     r6, r12, lsl #1     @overflow => set "FF"
    orrne   r6, r6, r10, lsl #16
    tst     r6, r12             @negative => set high bits to 0
    andeq   r6, r6, lr          @

    tst     r6, #0x200          @overflow => set low bits to ff
    orrne   r6, r6, r10
    tst     r6, #0x100          @negative => set low bits to 0
    biceq   r6, r6, #0xff
    and     r6, r6, r9
@
@calc the high 8 bits
@
    and     r4, r9, r4, LSR #8
    and     r5, r9, r5, LSR #8
    add     r7, r4, r5

    add     r7, r7, r11         @x-128 = (x+128) - 256
    tst     r7, r12, lsl #1     @overflow => set "FF"
    orrne   r7, r7, r10, lsl #16
    tst     r7, r12             @negative => set high bits to 0
    andeq   r7, r7, lr          @

    tst     r7, #0x200          @overflow => set low bits to ff
    orrne   r7, r6, r10
    tst     r7, #0x100          @negative => set low bits to 0
    biceq   r7, r7, #0xff
    and     r7, r7, r9

    orr     r6, r6, r7, LSL #8
    str     r6, [r2], #4

AACFUVF_LoopEnd:
    subs    r3, r3, #4
    bge     AACFUVF_Start

    adds    r3, r3, #4
    ble     AACFUVF_End

AACFUVF_Loop2Start:
    ldrb    r4, [r0], #1
    ldrb    r5, [r1], #1
    add     r6, r4, r5
    subs    r6, r6, #0x80
    submi   r6, r6, r6          @ r6=0
    tst     r6, #0x100
    orrne   r6, r6, #0xff       @ overflow => set FF
    strb    r6, [r2], #1
    subs    r3, r3, #1
    bgt     AACFUVF_Loop2Start

AACFUVF_End:
    ldmia   sp!, {r4 - r12, pc}

    WMV_ENTRY_END
    @  @ AffineAddCrossFadeUVFrame

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    WMV_LEAF_ENTRY WMVImageFadeColorQuickFillY
@
@r0: pOutY
@r1: pIn1Y
@r2: pIn2Y
@r3: iSize
@r12: iBackgroundY
@
    stmdb   sp!, {r4 - r12, lr}

    mov     r9, #0xff
    mov     r10, r9, lsl #24
    mov     r11, r9, lsl #16
    add     r9, r9, r9, lsl #16

    ldr     r12, [sp, #40]
    orr     lr, r12, r12, lsl #16

    b       IFCQFY_LoopEnd

IFCQFY_Start:

    ldr     r4, [r2], #4
    ldr     r5, [r1], #4

    and     r7, r4, r9
    and     r8, r5, r9
    add     r6, r7, r8
    add     r6, r6, lr
    tst     r6, #0xF00      @test overflow
    orrne   r6, r6, #0xff   @overflow => set "FF"
    tst     r6, r10         @#0xFF000000    @test overflow high bits
    orrne   r6, r6, r11     @overflow => set "FF"
    and     r6, r6, r9

    and     r4, r9, r4, lsr #8
    and     r5, r9, r5, lsr #8
    add     r7, r4, r5
    add     r7, r7, lr
    tst     r7, #0xF00          @test overflow
    orrne   r7, r7, #0xff       @overflow => set "FF"
    tst     r7, r10             @test overflow high bits
    orrne   r7, r7, r11         @overflow => set "FF"
    and     r7, r7, r9

    orr     r6, r6, r7, lsl #8
    str     r6, [r0], #4

IFCQFY_LoopEnd:
    subs    r3, r3, #4
    bge     IFCQFY_Start

    adds    r3, r3, #4
    ble     IFCQFY_End

IFCQFY_Loop2Start:
    ldrb    r4, [r2], #1
    ldrb    r5, [r1], #1
    add     r6, r4, r5
    add     r6, r6, r12
    tst     r6, #0xF00          @test overflow
    orrne   r6, r6, #0xff       @overflow => set "FF"
    strb    r6, [r0], #1
    subs    r3, r3, #1
    bgt     IFCQFY_Loop2Start

IFCQFY_End:
    
    ldmia   sp!, {r4 - r12, pc}

    WMV_ENTRY_END
    @  @ WMVImageFadeColorQuickFillY


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    WMV_LEAF_ENTRY WMVImageFadeColorQuickFillUV
@
@                   (U8_WMV *pOutU, U8_WMV *pIn1U, U8_WMV *pIn2U, 
@                   I32_WMV iSize, I32_WMV iBackgroundU)
@
@
@r0: pOutU
@r1: pIn1U
@r2: pIn2U
@r3: iSize
@r12: iBackgroundU
@
    stmdb   sp!, {r4 - r12, lr}

    mov     r9, #0xff

    ldr     r12, [sp, #40]          @ load iBackgroundU
    mov     r11, #0x280             @ 640 + iBackgroundU
    add     r11, r11, r12
    add     r11, r11, r11, lsl #16  @ r11 = 0x28000280

    orr     r10, r9, #0x300         @ r10 = 0x3ff

    mov     lr, #7
    add     lr, lr, r9, LSL #3      @ lr = 0x7ff

    add     r9, r9, r9, lsl #16     @ r9  = 0xff00ff

    mov     r12, #2
    mov     r12, r12, LSL #24       @ r12 = 0x2000000

    b       IFCQFUV_LoopEnd

IFCQFUV_Start:

    ldr     r4, [r2], #4
    ldr     r5, [r1], #4
@
@calc the low 8 bits
@
    and     r7, r4, r9
    and     r8, r5, r9
    add     r6, r7, r8

    add     r6, r6, r11         @x-128 = (x+640) - 768
    tst     r6, r12, lsl #1     @overflow => set "FF"
    orrne   r6, r6, r10, lsl #16
    subs    r8, r6, #0x3000000  @negative => set high bits to 0
    andmi   r6, r6, lr      @

    tst     r6, #0x400          @overflow => set low bits to ff
    orrne   r6, r6, r10
    and     r8, r6, #0x300          @negative => set low bits to 0
    teq     r8, #0x300
    bicne   r6, r6, #0xff
    and     r6, r6, r9
@
@calc the high 8 bits
@
    and     r4, r9, r4, LSR #8
    and     r5, r9, r5, LSR #8
    add     r7, r4, r5

    add     r7, r7, r11         @x-128 = (x+640) - 768
    tst     r7, r12, lsl #1     @overflow => set "FF"
    orrne   r7, r7, r10, lsl #16
    subs    r8, r7, #0x3000000  @negative => set high bits to 0
    andmi   r7, r7, lr      @

    tst     r7, #0x400          @overflow => set low bits to ff
    orrne   r7, r7, r10
    and     r8, r7, #0x300          @negative => set low bits to 0
    teq     r8, #0x300
    bicne   r7, r7, #0xff
    and     r7, r7, r9

    orr     r6, r6, r7, LSL #8
    str     r6, [r0], #4

IFCQFUV_LoopEnd:
    subs    r3, r3, #4
    bge     IFCQFUV_Start

    adds    r3, r3, #4
    ble     IFCQFUV_End

IFCQFUV_Loop2Start:
    ldrb    r4, [r2], #1
    ldrb    r5, [r1], #1
    add     r6, r4, r5
    ldr     r12, [sp, #40]          @ load iBackgroundU
    add     r6, r6, r12
    subs    r6, r6, #0x80
    submi   r6, r6, r6          @ r6=0
    tst     r6, #0x100
    orrne   r6, r6, #0xff       @ overflow => set FF
    strb    r6, [r0], #1
    subs    r3, r3, #1
    bgt     IFCQFUV_Loop2Start

IFCQFUV_End:
    ldmia   sp!, {r4 - r12, pc}

    WMV_ENTRY_END
    @  @ WMVImageFadeColorQuickFillUV

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    WMV_LEAF_ENTRY ImageRoll_LineQuickFill
@
@r0: pOutY
@r1: pInY
@r2: iDOffset_new
@r3: loops
@
    stmdb   sp!, {r4-r8, r10-r11, lr}

	mul		r3,  r2, r3
	add		r10, r0, r2
	add		r11, r1, r2
	mov		lr, r2, lsl #1
	mov		r8,  r3

    b       IRILQF_LoopEnd

IRILQF_Start:
@
@first 4
@
    ldrb    r4, [r1],  lr
    ldrb    r5, [r11], lr
    ldrb    r6, [r1],  lr
    ldrb    r7, [r11], lr

    strb    r4, [r0],  lr
    strb    r5, [r10], lr
    strb    r6, [r0],  lr
    strb    r7, [r10], lr
@
@ next 4
@
    ldrb    r4, [r1],  lr
    ldrb    r5, [r11], lr
    ldrb    r6, [r1],  lr
    ldrb    r7, [r11], lr

    strb    r4, [r0],  lr
    strb    r5, [r10], lr
    strb    r6, [r0],  lr
    strb    r7, [r10], lr

IRILQF_LoopEnd:
    subs    r3, r3, r2, lsl #3
    bge     IRILQF_Start

    adds    r3, r3, r2, lsl #3
    ble     IRILQF_End

IRILQF_Loop2Start:
    ldrb    r4, [r1], r2
    subs    r3, r3, r2
    strb    r4, [r0], r2
    bgt     IRILQF_Loop2Start

IRILQF_End:
	mov		r0, r8
    ldmia   sp!, {r4-r8, r10-r11, pc}

    WMV_ENTRY_END
    @  @ ImageRoll_LineQuickFill

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    WMV_LEAF_ENTRY WMVImageGenerateFlipOutputYLine
@
@r0: pOutY
@r1: pInY
@r2: pEffects
@r3: y
@
    stmdb   sp!, {r4 - r11, lr}

    ldr     r4, [r2, #_WMVIMAGE_EFFECTS_m_iCenterY]
    sub     r3, r3, r4              @yc

    ldr     r5, [r2, #_WMVIMAGE_EFFECTS_m_iImageWidth]
    ldr     r6, [r2, #_WMVIMAGE_EFFECTS_m_pFlipX]
    ldr     r7, [r2, #_WMVIMAGE_EFFECTS_m_pFlipFlag]
    ldr     r8, [r2, #_WMVIMAGE_EFFECTS_m_pFlipYRatio]
    ldr     r9, [r2, #_WMVIMAGE_EFFECTS_m_iImageHeight]

    eor     lr, lr, lr              @lr=0
    eor     r2, r2, r2              @r2=0

    cmp     lr, r5
    bge     WMVIGFOYL_End

WMVIGFOYL_Start:

@
@r4 = pEffects->m_pFlipFlag[x]
@
    ldr     r10, [r7, lr, lsl #2]   @pEffects->m_pFlipFlag[x]
    cmp     r10, #0
    beq     WMVIGFOYL_ShortPath
@
@yy = ((yc * pEffects->m_pFlipYRatio[x]) >> 20) + pEffects->m_iCenterY@
@
WMVIGFOYL_FullPath:
    ldr     r10, [r8, lr, lsl #2]   @pEffects->m_pFlipYRatio[x]
    mul     r11, r10, r3            @yc * pEffects->m_pFlipYRatio[x]
    add     r11, r4, r11, asr #20
    cmp     r11, r2
    blt     WMVIGFOYL_ShortPath
    cmp     r11, r9
    bge     WMVIGFOYL_ShortPath
    mul     r10, r11, r5
    ldr     r11, [r6, lr, lsl #2]
    add     r11, r10, r11
    ldrb    r10, [r1, r11]
    strb    r10, [r0], #1

    b       WMVIGFOYL_LoopEnd
@
@*pOutY++ = 0@
@
WMVIGFOYL_ShortPath:
    strb    r2, [r0], #1

WMVIGFOYL_LoopEnd:
    add     lr, lr, #1
    cmp     lr, r5
    blt     WMVIGFOYL_Start

WMVIGFOYL_End:

    ldmia   sp!, {r4 - r11, pc}

    WMV_ENTRY_END
    @  @ WMVImageGenerateFlipOutputYLine


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    WMV_LEAF_ENTRY WMVImageGenerateFlipOutputUVLine
@
@r0: pOutU
@r1: pInU
@r2: pOutV
@r3: pInV
@r12:pEffects
@r13:y
@
    stmdb   sp!, {r4 - r12, lr}

    ldr     r12, [sp, #40]  @pEffects
    ldr     r4, [r12, #_WMVIMAGE_EFFECTS_m_iCenterY]
    ldr     r5, [r12, #_WMVIMAGE_EFFECTS_m_iImageWidth]
    ldr     r6, [r12, #_WMVIMAGE_EFFECTS_m_pFlipX]
    ldr     r7, [r12, #_WMVIMAGE_EFFECTS_m_pFlipFlag]
    ldr     r8, [r12, #_WMVIMAGE_EFFECTS_m_pFlipYRatio]
    ldr     r9, [r12, #_WMVIMAGE_EFFECTS_m_iImageHeight]

    mov     r5, r5, asr #1          @iImageWidthUV

    ldr     r12, [sp, #44]          @y
    sub     r12, r12, r4            @yc
    eor     lr, lr, lr              @lr=0

    cmp     lr, r5
    bge     WMVIGFOUVL_End

WMVIGFOUVL_Start:

@
@r4 = pEffects->m_pFlipFlag[x]
@
    ldr     r10, [r7, lr, lsl #3]   @pEffects->m_pFlipFlag[x]
    cmp     r10, #0
    beq     WMVIGFOUVL_ShortPath
@
@yy = ((yc * pEffects->m_pFlipYRatio[x]) >> 20) + pEffects->m_iCenterY@
@
WMVIGFOUVL_FullPath:
    ldr     r10, [r8, lr, lsl #3]   @pEffects->m_pFlipYRatio[x]
    mul     r11, r10, r12           @yc * pEffects->m_pFlipYRatio[x]
    add     r11, r4, r11, asr #20
    cmp     r11, #0
    blt     WMVIGFOUVL_ShortPath
    cmp     r11, r9
    bge     WMVIGFOUVL_ShortPath
    mov     r11, r11, lsr #1
    mul     r10, r5, r11            @(yy >> 1) * iImageWidthUV
    ldr     r11, [r6, lr, lsl #3]
    add     r11, r10, r11, asr #1   @n = (yy >> 1) * iImageWidthUV + (xx >> 1)@

    ldrb    r10, [r1, r11]
    strb    r10, [r0], #1
    ldrb    r10, [r3, r11]
    strb    r10, [r2], #1

    b       WMVIGFOUVL_LoopEnd
@
@*pOutU++ = 0@
@*pOutV++ = 0@
@
WMVIGFOUVL_ShortPath:
    mov     r10, #128
    strb    r10, [r0], #1
    strb    r10, [r2], #1

WMVIGFOUVL_LoopEnd:
    add     lr, lr, #1
    cmp     lr, r5
    blt     WMVIGFOUVL_Start

WMVIGFOUVL_End:

    ldmia   sp!, {r4 - r12, pc}

    WMV_ENTRY_END
    @  @ WMVImageGenerateFlipOutputUVLine

@
@   WMVImageMemCpy(void *dest, void *src, int nbytes)
@   This is a fast memory copy function when copying large blocks of memory
@
    .global WMVImageMemCpy

    @AREA    .text, CODE
    WMV_LEAF_ENTRY WMVImageMemCpy
    cmp       r2, #0
    movle     pc, lr                    @ do nothing for nbytes <= 0
    stmdb     sp!, {r4-r8, r12, lr}     @ stmfd
    @FRAME_PROFILE_COUNT

    ands      r3, r0, #3               @ dest % 4
    beq       mcpLoop32start             @ dest % 4 = 0, aligned

    rsb       r3, r3, #4
    sub       r2, r2, r3

    tst       r3, #1
    beq       mcpySec				@ r3=2
           
    subs      r3, r3, #1                    
    ldrb      r5, [r1], #+1             @ copy one byte
    strb      r5, [r0], #+1
    beq       mcpLoop32start              @ r3==1

mcpySec:
    subs      r3, r3, #2
    ldrplh    r5, [r1], #+2             @ copy half word
    strplh    r5, [r0], #+2
    
    @ now copy 32 bytes at a time
mcpLoop32start:

    subs      r2, r2, #8*4
    blt       $mcp32Remain

$mcpLoop32:

    ldmia     r1!, {r3-r8, r12, lr} @ read from src
    subs      r2, r2, #8*4

	.if PLD_ENABLE == 1
        PLD   [r1]
        PLD   [r1, #32]
        PLD   [r1, #64]
        PLD   [r1, #96]
        PLD   [r1, #128]
    .endif

    stmia     r0!, {r3-r8, r12, lr} @ write to dst
    bpl       $mcpLoop32

$mcp32Remain:
    @ now cpy the remainder trying 16, 8, 4 at a time
    adds      r2, r2, #4*4      @ r1 = remaing count -32: add 32 and subtract 16 == add 16
    ldmplia   r1!, {r3-r6}      @ read from src
@	.if PLD_ENABLE == 1
@        PLD   [r1]
@        PLD   [r1, #32]
@    .endif

    stmplia   r0!, {r3-r6}      

    addmi     r2, r2, #4*4      @ add 16 back in if we did not store
    subs      r2, r2, #2*4      @ sub 8 and set flags
    ldmplia   r1!, {r3-r4}      @ read from src
    stmplia   r0!, {r3-r4}
    addmi     r2, r2, #2*4      @ add 8 back in if we did not store
    subs      r2, r2, #4
    ldrpl     r3, [r1], #+4     @ copy 4 bytes
    strpl     r3, [r0], #+4
    addmi     r2, r2, #4
    subs      r2, r2, #2
    ldrplh    r3, [r1], #+2     @ copy 2 bytes
    strplh    r3, [r0], #+2
    addmi     r2, r2, #2
    subs      r2, r2, #1
    ldrplb    r3, [r1]          @ copy 1 byte
    strplb    r3, [r0]

    ldmia     sp!, {r4-r8, r12, pc}  @ ldmfd
    
    WMV_ENTRY_END


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    .endif @ WMV_OPT_WVP2_ARM= 1

    .END 
