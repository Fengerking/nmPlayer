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
@//     interpolate_arm.s
@//
@// Abstract:
@//  
@//     ARM specific WMV9 interpolation
@//     Optimized assembly routines to implement motion compensation
@//
@//     Custom build with 
@//          armasm $(InputDir)\$(InputName).s $(OutDir)\$(InputName).obj
@//     and
@//          $(OutDir)\$(InputName).obj
@// 
@// Author:
@// 
@//     Chuang Gu (chuanggu@microsoft.com) Oct. 8, 2002
@//
@// Revision History:
@//
@//************************************************************************
    #include "../c/voWMVDecID.h"
    .include "wmvdec_member_arm.inc"
    .include "xplatform_arm_asm.h" 

    @AREA |.embsec_PMainLoopLvl1|, CODE, READONLY
    .section .text
       
    .if WMV_OPT_MOTIONCOMP_ARM==1

    .global  g_NewVertFilterX
    .global  g_NewHorzFilterX
    @.global  g_NewVertFilter0Long
	.global  g_NewVertFilter0LongNoGlblTbl
    .global  g_InterpolateBlock_00_SSIMD
    .global  g_AddNull_SSIMD
    .global  g_AddNullB_SSIMD
    .global  g_InterpolateBlockBilinear_SSIMD 
    .global  g_InterpolateBlockBilinear_SSIMD_11
    .global  g_InterpolateBlockBilinear_SSIMD_01
    .global  g_InterpolateBlockBilinear_SSIMD_10
    .global  g_Prefetch
    .global  IntensityComp

    .if _WMMX_==1
    
    .global  g_InterpolateBlockBicubic_WMMX_Copyy       @ not turn on yet
    .global  g_InterpolateBlockBicubic_WMMX_vertical
    .global  g_InterpolateBlockBicubic_WMMX_horizontal
    .global  g_InterpolateBlockBicubic_WMMX_VerHor
    .global  g_AddError_WMMX

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@Void_WMV g_InterpolateBlockBicubic_C_Copy (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA	|.text|, CODE
    WMV_LEAF_ENTRY g_InterpolateBlockBicubic_WMMX_Copyy

    stmdb     sp!, {r4 - r11, r14}
    FRAME_PROFILE_COUNT

pSrc                            .req r0
iSrcStride                      .req r1
pDst                            .req r2 
iDstStride                      .req r3

    and r12, pSrc, #7           @ get align offset to 64-bit position
    bic pSrc, pSrc, #7          @ pSrc 64-bit aligned

    wldrd wR0, [pSrc]           @ load 1st 8 bytes
    tmcr wCGR0, r12             @ transfer alighnment to wCGR0
    wldrd wR1, [pSrc, #8]       @ load 2nd 8 bytes
    add pSrc, pSrc, iSrcStride  @ next line
    wldrd wR3, [pSrc]           @ load 1st 8 bytes 
    walignr0 wR2, wR0, wR1      @ get 8 bytes
    wldrd wR4, [pSrc, #8]       @ load 2nd 8 bytes
    wstrd wR2, [pDst]           @ store data
    add pDst, pDst, iDstStride  @ next line
    add pSrc, pSrc, iSrcStride  @ next line
    wldrd wR0, [pSrc]           @ load 1st 8 bytes
    walignr0 wR5, wR3, wR4      @ get 8 bytes
    wldrd wR1, [pSrc, #8]       @ load 2nd 8 bytes
    add pSrc, pSrc, iSrcStride  @ next line
    wstrd wR5, [pDst]           @ store data
    wldrd wR3, [pSrc]           @ load 1st 8 bytes 
    add pDst, pDst, iDstStride  @ next line
    wldrd wR4, [pSrc, #8]       @ load 2nd 8 bytes
    add pSrc, pSrc, iSrcStride  @ next line
    walignr0 wR2, wR0, wR1      @ get 8 bytes
    wstrd wR2, [pDst]           @ store data
    wldrd wR0, [pSrc]           @ load 1st 8 bytes
    add pDst, pDst, iDstStride  @ next line
    wldrd wR1, [pSrc, #8]       @ load 2nd 8 bytes
    add pSrc, pSrc, iSrcStride  @ next line
    walignr0 wR5, wR3, wR4      @ get 8 bytes
    wstrd wR5, [pDst]           @ store data
    wldrd wR3, [pSrc]           @ load 1st 8 bytes  
    add pDst, pDst, iDstStride  @ next line
    wldrd wR4, [pSrc, #8]       @ load 2nd 8 bytes
    add pSrc, pSrc, iSrcStride  @ next line
    walignr0 wR2, wR0, wR1      @ get 8 byte
    wldrd wR0, [pSrc]           @ load 1st 8 bytes
    wstrd wR2, [pDst]           @ store data
    add pDst, pDst, iDstStride  @ next line
    wldrd wR1, [pSrc, #8]       @ load 2nd 8 bytes
    add pSrc, pSrc, iSrcStride  @ next line
    walignr0 wR5, wR3, wR4      @ get 8 bytes
    wstrd wR5, [pDst]           @ store data
    wldrd wR3, [pSrc]           @ load 1st 8 bytes 
    add pDst, pDst, iDstStride  @ next line
    wldrd wR4, [pSrc, #8]       @ load 2nd 8 bytes   
    walignr0 wR2, wR0, wR1      @ get 8 bytes
    wstrd wR2, [pDst]           @ store data
    add pDst, pDst, iDstStride  @ next line  
    walignr0 wR5, wR3, wR4      @ get 8 bytes
    @wstrd wR5, [pDst]           @ store data
    add pSrc, pSrc, iSrcStride  @ next line
    add pDst, pDst, iDstStride  @ next line

    ldmia     sp!, {r4 - r11, pc}
          
    WMV_ENTRY_END
    @ENDP  @g_InterpolateBlockBicubic_WMMX_Copyy

@@@@@@@@@@@@@@@ End of g_InterpolateBlockBicubic_C_Copy @@@@@@@@@@@@@@@@@@@@@@@



@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@Void_WMV Void_WMV g_InterpolateBlockBicubic_WMMX_VerHor (const U8_WMV *pSrc, I32_WMV iSrcStride,
@                U8_WMV *pDst, I32_WMV iDstStride, const I16_WMV *pH, const I16_WMV *pV, I32_WMV iShift, I32_WMV iRound1, I32_WMV iRound2)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA	|.text|, CODE
    WMV_LEAF_ENTRY g_InterpolateBlockBicubic_WMMX_VerHor

    stmdb     sp!, {r4 - r11, r14}
    FRAME_PROFILE_COUNT

pSrc                        .req  r0
iSrcStride                  .req  r1
pDst                        .req  r2
iDstStride                  .req  r3
pH                          .req  r4
pV                          .req  r5
iShift                      .req  r6
iRound1                     .req  r7
iRound2                     .req  r8
iCount                      .req  r14
iAlign                      .req  r9
temp                        .req  r12
.equ VHsaveRegistersOffset       , 36
@pHOffset                    , VHsaveRegistersOffset + 0
.equ VHpVOffset                    , VHsaveRegistersOffset + 4
.equ VHiShiftOffset                , VHsaveRegistersOffset + 8
.equ iRound1Offset               , VHsaveRegistersOffset + 12
.equ iRound2Offset               , VHsaveRegistersOffset + 16
@ wR0, wR1, wR2             : 1st row src data
@ wR3, wR4, wR5             : 2nd row src data
@ wR6, wR7, wR8             : 3rd row src data
@ wR9, wR10, wR11           : 4th row src data
@ wR12, wR13, wR14          @ accumulaters
@ wCGR0                     : iAlign
@ wCGR1                     : iAlign + 3
@ wCGR2                     : iShift
@ wCGR3                     : 7

    nop

    ldr pH, [sp, #pHOffset]
    ldr pV, [sp, #VHpVOffset]
	ldr iShift, [sp, #VHiShiftOffset]
    sub pSrc, pSrc, #1  
	ldr iRound1, [sp, #iRound1Offset]
    ldr iRound2, [sp, #iRound2Offset]
    sub pSrc, pSrc, iSrcStride  @ pSrc == pSrc - iSrcStride - 1
    and iAlign, pSrc, #7        @ get align offset to 64-bit position
    tmcr wCGR0, iAlign          @ transfer alighnment to wCGR0, wCGR1 is free!!!
    bic pSrc, pSrc, #7          @ pSrc 64-bit aligned
    mov temp, #7                
    tmcr wCGR3, temp            @ transfer 7 to wCGR3      
    ldrsh r10, [pV] 
    ldrsh r11, [pV, #2]
    ldrsh r12, [pV, #4]
    ldrsh r3,  [pV, #6]

    cmp iAlign, #4 @ if iAlign <== 4
    bgt Eq567_0
        @ get three src lines
        wldrd wR10, [pSrc]          @ load 1st 8 bytes
        mov iCount, #8
        wldrd wR11, [pSrc, #8]      @ load 2nd 8 bytes
        add pSrc, pSrc, iSrcStride  @ next line
        tmcr wCGR2, iShift          @ transfer iShift to wCGR2
		.if PLD_ENABLE == 1
        pld [pSrc, #32]
		.endif
        walignr0 wR12, wR10, wR11   @ get 8 bytes
        walignr0 wR13, wR11, wR10   @ get 3 + bytes
        wldrd wR10, [pSrc]          @ load 1st 8 bytes
        wunpckelub wR0, wR12        @ unpack 1st 4 bytes
        wldrd wR11, [pSrc, #8]      @ load 2nd 8 bytes
        wunpckehub wR1, wR12        @ unpack 2rd 4 bytes 
		.if PLD_ENABLE == 1
        pld [pSrc, #32]
		.endif
        wunpckelub wR2, wR13        @ unpack remaining 4 bytes      
        add pSrc, pSrc, iSrcStride  @ next line
        walignr0 wR12, wR10, wR11   @ get 8 bytes
        walignr0 wR13, wR11, wR10   @ get 3 + bytes
        wldrd wR10, [pSrc]          @ load 1st 8 bytes
        wunpckelub wR3, wR12        @ unpack 1st 4 bytes 
        wldrd wR11, [pSrc, #8]      @ load 2nd 8 bytes
        wunpckehub wR4, wR12        @ unpack 2rd 4 bytes 
		.if PLD_ENABLE == 1
        pld [pSrc, #32]
		.endif
        wunpckelub wR5, wR13        @ unpack remaining 4 bytes 
        add pSrc, pSrc, iSrcStride  @ next line
        walignr0 wR12, wR10, wR11   @ get 8 bytes
        walignr0 wR13, wR11, wR10   @ get 3 + bytes
        wunpckelub wR6, wR12        @ unpack 1st 4 bytes 
        wunpckehub wR7, wR12        @ unpack 2rd 4 bytes 
        wunpckelub wR8, wR13        @ unpack remaining 4 bytes 
        
        b LoopMCVerHor
Eq567_0:
        @ else iAlign ==== 5, 6, 7
        wldrd wR10, [pSrc]          @ load 1st 8 bytes
        mov iCount, #8
        wldrd wR11, [pSrc, #8]      @ load 2nd 8 bytes
        tmcr wCGR2, iShift          @ transfer iShift to wCGR2
        wldrd wR12, [pSrc, #16]     @ load 3rd 8 bytes
		.if PLD_ENABLE == 1
        pld [pSrc, #32]
		.endif
        add pSrc, pSrc, iSrcStride  @ next line
        walignr0 wR13, wR10, wR11   @ get 8 bytes
        wldrd wR10, [pSrc]          @ load 1st 8 bytes
        walignr0 wR14, wR11, wR12   @ get 3 + bytes
        wldrd wR11, [pSrc, #8]      @ load 2nd 8 bytes
        wunpckelub wR0, wR13        @ unpack 1st 4 bytes 
        wldrd wR12, [pSrc, #16]     @ load 3rd 8 bytes
		.if PLD_ENABLE == 1
        pld [pSrc, #32]
		.endif
        add pSrc, pSrc, iSrcStride  @ next line
        wunpckehub wR1, wR13        @ unpack 2rd 4 bytes 
        wunpckelub wR2, wR14        @ unpack remaining 4 bytes 
        walignr0 wR13, wR10, wR11   @ get 8 bytes
        wldrd wR10, [pSrc]          @ load 1st 8 bytes
        walignr0 wR14, wR11, wR12   @ get 3 + bytes
        wldrd wR11, [pSrc, #8]      @ load 2nd 8 bytes
        wunpckelub wR3, wR13        @ unpack 1st 4 bytes 
        wldrd wR12, [pSrc, #16]     @ load 3rd 8 bytes
        wunpckehub wR4, wR13        @ unpack 2rd 4 bytes 
		.if PLD_ENABLE == 1
        pld [pSrc, #32]
		.endif
        wunpckelub wR5, wR14        @ unpack remaining 4 bytes 
        walignr0 wR13, wR10, wR11   @ get 8 bytes
        walignr0 wR14, wR11, wR12   @ get 3 + bytes
        wunpckelub wR6, wR13        @ unpack 1st 4 bytes 
        wunpckehub wR7, wR13        @ unpack 2rd 4 bytes 
        wunpckelub wR8, wR14        @ unpack remaining 4 bytes 
        add pSrc, pSrc, iSrcStride  @ next line
  
LoopMCVerHor:    

    @ get the 4th row
    tbcsth wR15, r10                @ broadcast pV[0] to wR15
    cmp iAlign, #4 @ if iAlign <== 4
    bgt Eq567_1
        @ get three src lines
        wldrd wR10, [pSrc]          @ load 1st 8 bytes
        wmulsl wR0, wR15, wR0       @ wR12 == pV[0] * wR0 @ wR0 free
        wldrd wR11, [pSrc, #8]      @ load 2nd 8 bytes
		.if PLD_ENABLE == 1
        pld [pSrc, #32]
		.endif
        
        wmulsl wR1, wR15, wR1       @ wR13 == pV[0] * wR1 @ wR1 free
        walignr0 wR12, wR10, wR11   @ get 8 bytes
        wmulsl wR2, wR15, wR2       @ wR14 == pV[0] * wR2 @ wR2 free
        walignr0 wR13, wR11, wR10   @ get 3 + bytes
        wunpckelub wR9,  wR12       @ unpack 1st 4 bytes 
        wunpckehub wR10, wR12       @ unpack 2rd 4 bytes 
        wunpckelub wR11, wR13       @ unpack remaining 4 bytes       
        b LoopMCVerHorFiltering
Eq567_1:
        @ else iAlign ==== 5, 6, 7
        wldrd wR10, [pSrc]          @ load 1st 8 bytes
        wmulsl wR0, wR15, wR0       @ wR12 == pV[0] * wR0 @ wR0 free
        wldrd wR11, [pSrc, #8]      @ load 2nd 8 bytes
        wmulsl wR1, wR15, wR1       @ wR13 == pV[0] * wR1 @ wR1 free
        wldrd wR12, [pSrc, #16]     @ load 3rd 8 bytes
        wmulsl wR2, wR15, wR2       @ wR14 == pV[0] * wR2 @ wR2 free
		.if PLD_ENABLE == 1
        pld [pSrc, #32]
		.endif
        walignr0 wR13, wR10, wR11   @ get 8 bytes
        wunpckelub wR9,  wR13       @ unpack 1st 4 bytes 
        walignr0 wR14, wR11, wR12   @ get 3 + bytes
        wunpckehub wR10, wR13       @ unpack 2rd 4 bytes 
        wunpckelub wR11, wR14       @ unpack remaining 4 bytes 
    
LoopMCVerHorFiltering:
  
    @ vertical filtering

    tbcsth wR15, r11                @ broadcast pV[1] to wR15
    wmulsl wR12, wR15, wR3          @ wR0 == pV[1] * wR3
    add pSrc, pSrc, iSrcStride      @ next src line
    wmulsl wR13, wR15, wR4          @ wR1 == pV[1] * wR4
    waddhss wR12, wR12, wR0         @ +
    wmulsl wR14, wR15, wR5          @ wR2 == pV[1] * wR5  
    
    tbcsth wR15, r12                @ broadcast pV[2] to wR15
    wmulsl wR0, wR15, wR6           @ wR0 == pV[2] * wR6
    waddhss wR13, wR13, wR1         @ +
    wmulsl wR1, wR15, wR7           @ wR1 == pV[2] * wR7
    waddhss wR14, wR14, wR2         @ +
    wmulsl wR2, wR15, wR8           @ wR2 == pV[2] * wR8  
    
    tbcsth wR15, r3                 @ broadcast pV[3] to wR15
    waddhss wR12, wR12, wR0         @ +   
    wmulsl wR0, wR15, wR9           @ wR0 == pV[3] * wR9
    waddhss wR13, wR13, wR1         @ +
    wmulsl wR1, wR15, wR10          @ wR1 == pV[3] * wR10
    waddhss wR14, wR14, wR2         @ +
    wmulsl wR2, wR15, wR11          @ wR2 == pV[3] * wR11  
    
    waddhss wR12, wR12, wR0         @ +
    waddhss wR13, wR13, wR1         @ +
    waddhss wR14, wR14, wR2         @ +

    tbcsth wR15, iRound1            @ broadcast iRound1 to wR15
    waddhss wR12, wR12, wR15        @ + iRound1
    waddhss wR13, wR13, wR15        @ + iRound1
    waddhss wR14, wR14, wR15        @ + iRound1
    wldrd wR15, [pH]                @ make sure [pH] 64 bit aligned !!!
    wsrahg wR12, wR12, wCGR2        @ >>= iShift
    wsrahg wR13, wR13, wCGR2        @ >>= iShift
    wsrahg wR14, wR14, wCGR2        @ >>= iShift

    @@ horizontal filtering
    wmacsz wR0, wR12, wR15          @ pixel 0
    waligni wR1, wR12, wR13, #2     @ pixel 1
    wmacsz wR1, wR1, wR15           @ result of pixel 1
    waligni wR2, wR12, wR13, #4     @ pixel 2
    wmacsz wR2, wR2, wR15           @ result of pixel 2
    waligni wR12, wR12, wR13, #6    @ pixel 3@ wR12 is free
    wmacsz wR12, wR12, wR15         @ result of pixel 3
    wpackdss wR0, wR0, wR1          @ pack wR0, wR1 -> wR0@ wR1 free
    wpackdss wR2, wR2, wR12         @ pack wR2, wR12 -> wR2
    tbcstw wR1, iRound2             @ broadcast iRound2 to wR1
    waddwss wR0, wR0, wR1           @ + iRound2
    waddwss wR2, wR2, wR1           @ + iRound2
    wsrawg wR0, wR0, wCGR3          @ >> 7
    wsrawg wR2, wR2, wCGR3          @ >> 7
    wpackwus wR12, wR0, wR2         @ first 4 pixels ready in wR12
    
    wmacsz wR0, wR13, wR15          @ pixel 0
    waligni wR1, wR13, wR14, #2     @ pixel 1
    wmacsz wR1, wR1, wR15           @ result of pixel 1
    waligni wR2, wR13, wR14, #4     @ pixel 2
    wmacsz wR2, wR2, wR15           @ result of pixel 2
    waligni wR13, wR13, wR14, #6    @ pixel 3@ wR13 is free
    wmacsz wR13, wR13, wR15         @ result of pixel 3
    wpackdss wR0, wR0, wR1          @ pack wR0, wR1 -> wR0@ wR1 free
    wpackdss wR2, wR2, wR13         @ pack wR2, wR13 -> wR2
    tbcstw wR1, iRound2             @ broadcast iRound2 to wR1
    waddwss wR0, wR0, wR1           @ + iRound2
    waddwss wR2, wR2, wR1           @ + iRound2
    wsrawg wR0, wR0, wCGR3          @ >> 7
    wsrawg wR2, wR2, wCGR3          @ >> 7
    wpackwus wR13, wR0, wR2         @ second 4 pixels ready in wR13
    
    wpackhus wR12, wR12, wR13       @ pack pair of 4 pixels 
    subs iCount, iCount, #1
    wstrd wR12, [pDst]              @ store 8 final pixels
    add pDst, pDst, iSrcStride      @ next dst line, iSrcStride ==== iDstStride
    wmov wR0, wR3
    wmov wR1, wR4
    wmov wR2, wR5
    wmov wR3, wR6
    wmov wR4, wR7
    wmov wR5, wR8
    wmov wR6, wR9
    wmov wR7, wR10
    wmov wR8, wR11

    bne LoopMCVerHor    
    ldmia     sp!, {r4 - r11, pc}
          
    WMV_ENTRY_END
    @ENDP  @g_InterpolateBlockBicubic_WMMX_VerHor

@@@@@@@@@@@@@@@ End of g_InterpolateBlockBicubic_WMMX_VerHor @@@@@@@@@@@@@@@@@@@@@@@

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@Void_WMV g_AddError_WMMX(U8_WMV* ppxlcDst, U8_WMV* ppxlcRef, I16_WMV* ppxliError, I32_WMV iPitch, U8_WMV* pcClapTabMC)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA	|.text|, CODE
    WMV_LEAF_ENTRY g_AddError_WMMX

    FRAME_PROFILE_COUNT

    mov r12, #8
    sub r0, r0, r3
LoopAddError
    wldrd wR0, [r1]         @ load 8 bytes in Ref
    add r1, r1, r3          @ Ref + iPitch ==> Ref
    wldrd wR2, [r2, #8]     @ load 4 half words in Error
    add r0, r0, r3          @ Dst + iPitch ==> Dst
    wldrd wR1, [r2], #16    @ load 4 half words in Error
    wunpckelub wR3, wR0     @ unpack 1st 4 bytes in Ref
    wunpckehub wR4, wR0     @ unpack 2rd 4 bytes in Ref
    waddhss wR4, wR4, wR2   @ add error
    waddhss wR3, wR3, wR1   @ add error
    wpackhus wR0, wR3, wR4  @ pack wR3, wR4 ==> wR0
    subs r12, r12, #1       @ Count - 1 ==> Count
    wstrd wR0, [r0]         @ store wR0 result
    bne LoopAddError
    mov pc, r14
     
    WMV_ENTRY_END
    @ENDP  @g_AddError_WMMX

@@@@@@@@@@@@@@@ End of g_AddError_WMMX @@@@@@@@@@@@@@@@@@@@@@@

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@Void_WMV g_InterpolateBlockBicubic_WMMX_horizontal (const U8_WMV *pSrc, I32_WMV iSrcStride,
@                U8_WMV *pDst, I32_WMV iDstStride, const I16_WMV *pH, I32_WMV iShift, I32_WMV iRound)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA	|.text|, CODE
    WMV_LEAF_ENTRY g_InterpolateBlockBicubic_WMMX_horizontal

    stmdb     sp!, {r4 - r11, r14}
    FRAME_PROFILE_COUNT

@@@   Registers of g_InterpolateBlockBicubic_WMMX_horizontal
pSrc                        RN  0
iSrcStride                  RN  1
pDst                        RN  2
iDstStride                  RN  3
pH                          RN  4
pV                          RN  5
iShift                      RN  6
iRound                      RN  7
iRound1                     RN  7
iRound2                     RN  8
iCount                      RN  14
iAlign                      RN  9
iAlignNext                  RN  10
iJump                       RN  10
temp                        RN  12
saveRegistersOffset         EQU 36
pHOffset                    EQU saveRegistersOffset + 0
iShiftOffset                EQU saveRegistersOffset + 4
iRoundOffset                EQU saveRegistersOffset + 8
@ wR0                       : src, dst
@ wR1 - wR12                : WMMX registers
@ wR15                      : pH[0, 1, 2, 3]
@ wR14                      : [iShift, iShift, iShift, iShift]
@ wR13                      : [iRound, iRound, iRound, iRound]

    ldr pH, [sp, #pHOffset]
	ldr iShift, [sp, #iShiftOffset]
	ldr iRound, [sp, #iRoundOffset]

    and iAlign, pH, #7          @ get align offset to 64-bit position
    tmcr wCGR0, iAlign          @ transfer alighnment0 to wCGR0
    bic pH, pH, #7              @ pH 64-bit aligned
    wldrd wR14, [pH]
    wldrd wR15, [pH, #8]
    walignr0 wR15, wR14, wR15   @ get pH 
    
    sub pSrc, pSrc, #1          @ pSrc -== 1
    and iAlign, pSrc, #7        @ get align offset to 64-bit position
    tmcr wCGR0, iAlign          @ transfer alighnment0 to wCGR0
    add iAlignNext, iAlign, #1  @ 
    tmcr wCGR1, iAlignNext      @ transfer alighnment1 to wCGR1
    add iAlignNext, iAlign, #2  @ 
    tmcr wCGR2, iAlignNext      @ transfer alighnment2 to wCGR2
    add iAlignNext, iAlign, #3  @ 
    tmcr wCGR3, iAlignNext      @ transfer alighnment3 to wCGR3
    bic pSrc, pSrc, #7          @ pSrc 64-bit aligned
      
    sub iJump, iAlign, #5
    mov iJump, iJump, LSL #5    @ (iAlign - 4) * 32 ====> iJump
    tbcsth wR13, iRound         @ broadcast iRound to wR13
    tbcsth wR14, iShift         @ broadcast iShift to wR14
    mov iCount, #8              @ loop count
    @for (i == 0@ i < 8@ i++) {
Loop_InterpolateBlockBicubic_C_horizontal
    
    @Prepare
    @wR1:    x0x1x2x3x4x5x6x7
    @wR2:    x1x2x3x4x5x6x7x8
    @wR3:    x2x3x4x5x6x7x8x9
    @wR4:    x3x4x5x6x7x8x9x10
    cmp iAlign, #4 
        bgt Eq567
        wldrd wR0, [pSrc]           @ load 1st 8 bytes
		.if PLD_ENABLE == 1
        pld [pSrc, #32]
		.endif
        wldrd wR12, [pSrc, #8]      @ load 2nd 8 bytes
        walignr0 wR1, wR0, wR12     @ get wR1
        walignr1 wR2, wR0, wR12     @ get wR2     
        walignr2 wR3, wR0, wR12     @ get wR3
        walignr3 wR4, wR0, wR12     @ get wR4
        b Filtering
Eq567
        @cmp iAlign, #5  @else iAlign ==== 5
        add PC, PC, iJump
        nop
        wldrd wR0, [pSrc]           @ load 1st 8 bytes
		.if PLD_ENABLE == 1
        pld [pSrc, #32]
		.endif
        wldrd wR4, [pSrc, #8]       @ load 2nd 8 bytes
        nop
        waligni wR1, wR0, wR4, #5   @ get wR1
        waligni wR2, wR0, wR4, #6   @ get wR2     
        waligni wR3, wR0, wR4, #7   @ get wR3
        b Filtering

        @cmp iAlign, #6  @else iAlign ==== 6
        wldrd wR0, [pSrc]           @ load 1st 8 bytes
		.if PLD_ENABLE == 1
        pld [pSrc, #32]
		.endif
        wldrd wR3, [pSrc, #8]       @ load 2nd 8 bytes
        waligni wR1, wR0, wR3, #6   @ get wR1
        waligni wR2, wR0, wR3, #7   @ get wR2     
        wldrd wR0, [pSrc, #16]      @ get 3rd 8 bytes
        waligni wR4, wR3, wR0, #1   @ get wR4
        b Filtering

        @cmp iAlign, #7  @else iAlign ==== 7
        wldrd wR0, [pSrc]           @ load 1st 8 bytes
		.if PLD_ENABLE == 1
        pld [pSrc, #32]
		.endif
        wldrd wR2, [pSrc, #8]       @ load 2nd 8 bytes
        waligni wR1, wR0, wR2, #7   @ get wR1    
        wldrd wR0, [pSrc, #16]      @ get 3rd 8 bytes
        waligni wR3, wR2, wR0, #1   @ get wR3
        waligni wR4, wR2, wR0, #2   @ get wR4        
    @@@@ Now wR1, wR2, wR3, wR4 are ready
  
Filtering  
    @ 4-tag filtering
    wunpckelub wR5, wR1         @ unpack 1st 4 bytes
    wmacsz wR5, wR5, wR15       @ 4-tag filtering
    wunpckehub wR6, wR1         @ unpack 2rd 4 bytes
    wmacsz wR6, wR6, wR15       @ 4-tag filtering
    wunpckelub wR7, wR2         @ unpack 1st 4 bytes
    wmacsz wR7, wR7, wR15       @ 4-tag filtering
    wunpckehub wR8, wR2         @ unpack 2rd 4 bytes
    wmacsz wR8, wR8, wR15       @ 4-tag filtering
    wunpckelub wR9, wR3         @ unpack 1st 4 bytes
    wmacsz wR9, wR9, wR15       @ 4-tag filtering
    wunpckehub wR10, wR3        @ unpack 2rd 4 bytes
    wmacsz wR10, wR10, wR15     @ 4-tag filtering
    wunpckelub wR11, wR4        @ unpack 1st 4 bytes
    wmacsz wR11, wR11, wR15     @ 4-tag filtering
    wunpckehub wR12, wR4        @ unpack 2rd 4 bytes
    wmacsz wR12, wR12, wR15     @ 4-tag filtering
    @ results are in wR5 -- wR12
    @ wR5  wR6
    @ wR7  wR8
    @ wR9  wR10
    @ wR11 wR12

    @ packing 64bits
    wpackdss wR5, wR5, wR7      @ pack wR5, wR7   ==> wR5
    wpackdss wR9, wR9, wR11     @ pack wR9, wR11  ==> wR9
    wpackdss wR6, wR6, wR8      @ pack wR6, wR8   ==> wR6
    wpackdss wR10, wR10, wR12   @ pack wR10, wR12 ==> wR10
    @ packing 32bits
    wpackwss wR5, wR5, wR9      @ pack wR5, wR9   ==> wR5
    wpackwss wR6, wR6, wR10     @ pack wR6, wR10  ==> wR6
    @ round and shift
    waddhss wR5, wR5, wR13      @ wR5 == wR5 + iRound
    waddhss wR6, wR6, wR13      @ wR6 == wR6 + iRound
    wsrah wR5, wR5, wR14        @ >>= iShift
    wsrah wR6, wR6, wR14        @ >>= iShift
    @ pack 16bits
    wpackhus wR0, wR5, wR6      @ pack wR5, wR6 ==> wR0

    subs iCount, iCount, #1     @ i == i - 1
    add pSrc, pSrc, iSrcStride  @ pSrc +== iSrcStride@
    wstrd wR0, [pDst]           @ store wR0 result
    add pDst, pDst, iDstStride  @ pDst +== iDstStride@
    bne Loop_InterpolateBlockBicubic_C_horizontal
    @}

    ldmia     sp!, {r4 - r11, pc}
     
    WMV_ENTRY_END
    @ENDP  @g_InterpolateBlockBicubic_WMMX_horizontal

@@@@@@@@@@@@@@@ End of g_InterpolateBlockBicubic_WMMX_vertical @@@@@@@@@@@@@@@@@@@@@@@


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@Void_WMV g_InterpolateBlockBicubic_WMMX_vertical (const U8_WMV *pSrc, I32_WMV iSrcStride,
@                U8_WMV *pDst, I32_WMV iDstStride, const I16_WMV *pV, I32_WMV iShift, I32_WMV iRound)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA	|.text|, CODE
    WMV_LEAF_ENTRY g_InterpolateBlockBicubic_WMMX_vertical

    stmdb     sp!, {r4 - r11, r14}
    FRAME_PROFILE_COUNT

@@@   Registers of g_InterpolateBlockBicubic_WMMX_vertical
pSrc                        RN  0
iSrcStride                  RN  1
pDst                        RN  2
iDstStride                  RN  3
pH                          RN  4
pV                          RN  5
iShift                      RN  6
iRound                      RN  7
iRound1                     RN  7
iRound2                     RN  8
iCount                      RN  14
temp                        RN  12
@saveRegistersOffset         EQU 36
pVOffset                    EQU saveRegistersOffset + 0
@iShiftOffset                EQU saveRegistersOffset + 4
@iRoundOffset                EQU saveRegistersOffset + 8
@ wR0, wR15             :   dst
@ wR1, wR2, wR3, wR4    :   pV[0, 1, 2, 3]
@ wR5                   :   iRound
@ wR6, wR7, wR8, wR9    :   vertical 4 pixels
@ wR10, wR11, wR12, wR13:   next vertical 4 pixels
@ wR14                  :   temp

    @I32_WMV i, j@
    @I16_WMV wR0[4], wR15[4]@ // dst
    @I16_WMV wR1[4] == {pV[0], pV[0], pV[0], pV[0]}@
    @I16_WMV wR2[4] == {pV[1], pV[1], pV[1], pV[1]}@
    @I16_WMV wR3[4] == {pV[2], pV[2], pV[2], pV[2]}@
    @I16_WMV wR4[4] == {pV[3], pV[3], pV[3], pV[3]}@
    @I16_WMV wR5[4] == {iRound, iRound, iRound, iRound,}@

    ldr pV, [sp, #pVOffset]
	ldr iShift, [sp, #iShiftOffset]
	ldr iRound, [sp, #iRoundOffset]

    ldrsh r9,  [pV]
    ldrsh r10, [pV, #2]
    ldrsh r11, [pV, #4]
    ldrsh r12, [pV, #6]
    tbcsth wR1, r9              @ broadcast r9 to wR1
    tbcsth wR2, r10             @ broadcast r10 to wR2
    tbcsth wR3, r11             @ broadcast r11 to wR3
    tbcsth wR4, r12             @ broadcast r12 to wR4
    
    sub pSrc, pSrc, iSrcStride  @ previous line pSrc -== iSrcStride
	.if PLD_ENABLE == 1
    pld [pSrc, #32]
	.endif
    and temp, pSrc, #7          @ temp
    tmcr wCGR1, temp            @ transfer alighnment to wCGR1
    bic pSrc, pSrc, #7          @ pSrc 64-bit aligned

    wldrd wR6, [pSrc]           @ load 1st 8 bytes
    tbcsth wR5, iRound          @ broadcast iRound to wR5
    wldrd wR10, [pSrc, #8]      @ load 2rd 8 bytes
    tmcr wCGR2, iShift          @ transfer iShift to wCGR2
    add pSrc, pSrc, iSrcStride  @ pSrc to next row
	.if PLD_ENABLE == 1
    pld [pSrc, #32]
	.endif
       
    wldrd wR7, [pSrc]           @ load 1st 8 bytes
    walignr1 wR14, wR6, wR10    @ get 1st 8 bytes row   
    wldrd wR11, [pSrc, #8]      @ load 2rd 8 bytes
    wunpckelub wR6, wR14        @ unpack 1st 4 bytes
    wunpckehub wR10, wR14       @ unpack 2rd 4 bytes
    add pSrc, pSrc, iSrcStride  @ pSrc to next row
	.if PLD_ENABLE == 1
    pld [pSrc, #32]
	.endif

    wldrd wR8, [pSrc]           @ load 1st 8 bytes
    walignr1 wR14, wR7, wR11    @ get 2rd 8 bytes row 
    wldrd wR12, [pSrc, #8]      @ load 2rd 8 bytes
    wunpckelub wR7, wR14        @ unpack 1st 4 bytes
    wunpckehub wR11, wR14       @ unpack 2rd 4 bytes
    add pSrc, pSrc, iSrcStride  @ pSrc to next row
    
    walignr1 wR14, wR8, wR12    @ get 3rd 8 bytes row     
    wunpckelub wR8, wR14        @ unpack 1st 4 bytes
    wunpckehub wR12, wR14       @ unpack 2rd 4 bytes

    mov iCount, #8              @ loop count
    @for (i == 0@ i < 8@ i++) {
Loop_InterpolateBlockBicubic_C_vertical
    
	.if PLD_ENABLE == 1
    pld [pSrc, #32]
	.endif
    wldrd wR9, [pSrc]           @ load 1st 8 bytes of last row
    wmulsl wR0, wR6, wR1        @ wR0 == wR6 * wR1
    wldrd wR13, [pSrc, #8]      @ load 2rd 8 bytes of last row
    wmulsl wR6, wR7, wR2        @ wR6 == wR7 * wR2
    add pSrc, pSrc, iSrcStride  @ pSrc to next row
    waddhss wR0, wR0, wR6       @ wR0 == wR0 + wR6
    walignr1 wR14, wR9, wR13    @ get complete 8 bytes in last row
    wmulsl wR6, wR8, wR3        @ wR6 == wR8 * wR3
    wunpckelub wR9, wR14        @ unpack 1st 4 bytes of last row
    waddhss wR0, wR0, wR6       @ wR0 == wR0 + wR6
    wmulsl wR6, wR9, wR4        @ wR6 == wR9 * wR4
    wunpckehub wR13, wR14       @ unpack 2rd 4 bytes of last row
    waddhss wR0, wR0, wR6       @ wR0 == wR0 + wR6
    waddhss wR0, wR0, wR5       @ wR0 == wR0 + wR5
    wsrahg wR0, wR0, wCGR2      @ >>= iShift

    wmulsl wR15, wR10, wR1      @ wR15 == wR10 * wR1
    wmov wR6, wR7               @ wR7 ==> wR6
    wmulsl wR10, wR11, wR2      @ wR10 == wR11 * wR2
    wmov wR7, wR8               @ wR8 ==> wR7
    waddhss wR15, wR15, wR10    @ wR15 == wR15 + wR10
    wmulsl wR10, wR12, wR3      @ wR10 == wR12 * wR3
    wmov wR8, wR9               @ wR9 ==> wR8
    waddhss wR15, wR15, wR10    @ wR15 == wR15 + wR10
    wmulsl wR10, wR13, wR4      @ wR10 == wR13 * wR4
    subs iCount, iCount, #1     @ i == i - 1
    waddhss wR15, wR15, wR10    @ wR15 == wR15 + wR10
    waddhss wR15, wR15, wR5     @ wR15 == wR15 + wR5
    wsrahg wR15, wR15, wCGR2    @ >>= iShift

    wpackhus wR14, wR0, wR15    @ pack wR0, wR15 ==> wR14
    wmov wR10, wR11             @ wR11 ==> wR10
    wmov wR11, wR12             @ wR12 ==> wR11
    wmov wR12, wR13             @ wR13 ==> wR12
    wstrd wR14, [pDst]          @ store result
    add pDst, pDst, iDstStride  @ pDst +== iDstStride@
     
    bne Loop_InterpolateBlockBicubic_C_vertical
    @}

    ldmia     sp!, {r4 - r11, pc}
     
    WMV_ENTRY_END
    @ENDP  @g_InterpolateBlockBicubic_WMMX_vertical

@@@@@@@@@@@@@@@ End of g_InterpolateBlockBicubic_WMMX_vertical @@@@@@@@@@@@@@@@@@@@@@@
    .endif @@_WMMX_



     @AREA  |.embsec_PMainLoopLvl1|, CODE

g_Prefetch: @PROC

    ldrb   r2,  [r0 ], r1
    ldrb   r12, [r0 ], r1
    ldrb   r3,  [r0 ], r1
  @  ldrb   r3, [r0 ], r1
  @  ldrb   r2,  [r0 ], r1
  @  ldrb   r3,  [r0 ], r1
  @  ldrb   r12, [r0 ], r1
  @  ldrb   r2,  [r0 ], r1
 @   ldrb   r3,  [r0 ], r1

    mov pc, r14
    

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@Void_WMV  g_InterpolateBlockBilinear_SSIMD (const U8_WMV *pSrc, 
@                                           I32_WMV iSrcStride, 
@                                           U8_WMV *pDst, 
@                                           I32_WMV iXFrac, 
@                                           I32_WMV iYFrac, 
@                                           I32_WMV iRndCtrl, 
@                                           Bool_WMV b1MV)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@{  
@    I32_WMV i, j, k0,k1@
@    I32_WMV iNumLoops == 8<<b1MV@
@    U8_WMV *pD @ 
@    const U8_WMV  *pT @
@    iRndCtrl == 8 - ( iRndCtrl&0xff)@ 
@    pT == pSrc@	
@    pD == pDst@
@
@	if( 0 ==== b1MV )	{
@	I16_WMV a,b,c,d@
@	    a == (pT[0]<<2)+  ( pT[iSrcStride] - pT[0])* iYFrac@
@	    for (i == 0@ i < 8@ i++) 
@	    {
@	//0
@	        b == (pT[1]<<2)+  ( pT[iSrcStride+1] - pT[1])* iYFrac@
@	        c == (pT[2]<<2)+  ( pT[iSrcStride+2] - pT[2])* iYFrac@
@	        k0 == ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4@
@	        k1 == ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4@
@	        *(I16_WMV *)(pD+0) == (U8_WMV) k0@
@	        *(I16_WMV *)(pD+20) == (U8_WMV) k1@
@	
@	        d == (pT[3]<<2)+  ( pT[iSrcStride+3] - pT[3])* iYFrac@
@	        a == (pT[4]<<2)+  ( pT[iSrcStride+4] - pT[4])* iYFrac@
@	        k0 == ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4@
@	        k1 == ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4@
@	        *(I16_WMV *)(pD+2) == (U8_WMV) k0@
@	        *(I16_WMV *)(pD+22) == (U8_WMV) k1@
@	//1
@	        b == (pT[5]<<2)+  ( pT[iSrcStride+5] - pT[5])* iYFrac@
@	        c == (pT[6]<<2)+  ( pT[iSrcStride+6] - pT[6])* iYFrac@
@	        k0 == ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4@
@	        k1 == ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4@
@	        *(I16_WMV *)(pD+4) == (U8_WMV) k0@
@	        *(I16_WMV *)(pD+24) == (U8_WMV) k1@
@	
@	        d == (pT[7]<<2)+  ( pT[iSrcStride+7] - pT[7])* iYFrac@
@	        a == (pT[8]<<2)+  ( pT[iSrcStride+8] - pT[8])* iYFrac@
@	        k0 == ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4@
@	        k1 == ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4@
@	        *(I16_WMV *)(pD+6) == (U8_WMV) k0@
@	        *(I16_WMV *)(pD+26) == (U8_WMV) k1@
@
@	        pT +== iSrcStride@
@	        pD +== 40@
@	    } 
@	else {
@		I16_WMV a,b,c,d@
@        a == (pT[0]<<2)+  ( pT[iSrcStride] - pT[0])* iYFrac@
@	    for (i == 0@ i < 16@ i++) 
@	    {
@//0
@        b == (pT[1]<<2)+  ( pT[iSrcStride+1] - pT[1])* iYFrac@
@        c == (pT[2]<<2)+  ( pT[iSrcStride+2] - pT[2])* iYFrac@
@        k0 == ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4@
@        k1 == ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4@
@        *(I16_WMV *)(pD+0) == (U8_WMV) k0@
@        *(I16_WMV *)(pD+20) == (U8_WMV) k1@
@
@        d == (pT[3]<<2)+  ( pT[iSrcStride+3] - pT[3])* iYFrac@
@        a == (pT[4]<<2)+  ( pT[iSrcStride+4] - pT[4])* iYFrac@
@        k0 == ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4@
@        k1 == ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4@
@        *(I16_WMV *)(pD+2) == (U8_WMV) k0@
@        *(I16_WMV *)(pD+22) == (U8_WMV) k1@
@//1
@        b == (pT[5]<<2)+  ( pT[iSrcStride+5] - pT[5])* iYFrac@
@        c == (pT[6]<<2)+  ( pT[iSrcStride+6] - pT[6])* iYFrac@
@        k0 == ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4@
@        k1 == ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4@
@        *(I16_WMV *)(pD+4) == (U8_WMV) k0@
@        *(I16_WMV *)(pD+24) == (U8_WMV) k1@
@
@        d == (pT[7]<<2)+  ( pT[iSrcStride+7] - pT[7])* iYFrac@
@        a == (pT[8]<<2)+  ( pT[iSrcStride+8] - pT[8])* iYFrac@
@        k0 == ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4@
@        k1 == ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4@
@        *(I16_WMV *)(pD+6) == (U8_WMV) k0@
@        *(I16_WMV *)(pD+26) == (U8_WMV) k1@
@	//2
@	        b == (pT[9]<<2)+  ( pT[iSrcStride+9] - pT[9])* iYFrac@
@	        c == (pT[10]<<2)+  ( pT[iSrcStride+10] - pT[10])* iYFrac@
@	        k0 == ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4@
@	        k1 == ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4@
@	        *(I16_WMV *)(pD+8) == (U8_WMV) k0@
@	        *(I16_WMV *)(pD+28) == (U8_WMV) k1@
@	
@	        d == (pT[11]<<2)+  ( pT[iSrcStride+11] - pT[11])* iYFrac@
@	        a == (pT[12]<<2)+  ( pT[iSrcStride+12] - pT[12])* iYFrac@
@	        k0 == ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4@
@	        k1 == ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4@
@	        *(I16_WMV *)(pD+10) == (U8_WMV) k0@
@	        *(I16_WMV *)(pD+30) == (U8_WMV) k1@
@	//3
@	        b == (pT[13]<<2)+  ( pT[iSrcStride+13] - pT[13])* iYFrac@
@	        c == (pT[14]<<2)+  ( pT[iSrcStride+14] - pT[14])* iYFrac@
@	        k0 == ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4@
@	        k1 == ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4@
@	        *(I16_WMV *)(pD+12) == (U8_WMV) k0@
@	        *(I16_WMV *)(pD+32) == (U8_WMV) k1@
@	
@	        d == (pT[15]<<2)+  ( pT[iSrcStride+15] - pT[15])* iYFrac@
@	        a == (pT[16]<<2)+  ( pT[iSrcStride+16] - pT[16])* iYFrac@
@	        k0 == ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4@
@	        k1 == ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4@
@	        *(I16_WMV *)(pD+14) == (U8_WMV) k0@
@	        *(I16_WMV *)(pD+34) == (U8_WMV) k1@
@
@        pT +== iSrcStride@
@        pD +== 40@
@    } 
@}
@}

    @AREA  |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY g_InterpolateBlockBilinear_SSIMD

@r0 == pSrc
@r12 == pSrc + iSrcStride	
@r1 == iSrcStride
@r2 == pDst
@r3 == iXFrac
@r4 == iYFrac
@r5 == iRndCtl
@r14 == iNumLoops@
@r6-r11

@@ stack usage
.equ IBB_StackSize              , 0x24
.equ IBB_OffsetRegSaving        , 0x24

.equ IBB_Offset_iYFrac          , IBB_StackSize + IBB_OffsetRegSaving + 0
.equ IBB_Offset_iRndCtrl        , IBB_StackSize + IBB_OffsetRegSaving + 4
.equ IBB_Offset_b1MV            , IBB_StackSize + IBB_OffsetRegSaving + 8


    stmdb     sp!, {r4 - r11, r14}
    FRAME_PROFILE_COUNT

    @I32_WMV iNumLoops == 8<<b1MV@

	
    ldr r6, [sp, #44]
	.if PLD_ENABLE == 1
	pld		[r0, #32]
	.endif

    ldr r10, [sp, #40]
    mov r12, #8
    mov r14, r12, lsl r6				@r14 == iNumLoops
  
    @iRndCtrl == 8 - ( iRndCtrl&0xff)@
    and r10, r10, #0xff
    sub r5 , r12, r10					@r5 == iRndCtrl

    ldr  r4, [sp, #36]					@r4==iYFrac

	add r12, r0, r1						@r12 == pT + iSrcStride
	cmp	r6, #0
	bne lab_16_loop
loop_g_InterpolateBlockBilinear_SSIMD:

@        a == (pT[0]<<2)+  ( pT[iSrcStride] - pT[0])* iYFrac@
@//0
@        b == (pT[1]<<2)+  ( pT[iSrcStride+1] - pT[1])* iYFrac@
@        c == (pT[2]<<2)+  ( pT[iSrcStride+2] - pT[2])* iYFrac@
@        k0 == ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4@
@        k1 == ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4@
@        *(I16_WMV *)(pD+0) == (U8_WMV) k0@
@        *(I16_WMV *)(pD+20) == (U8_WMV) k1@
@
@        d == (pT[3]<<2)+  ( pT[iSrcStride+3] - pT[3])* iYFrac@
@        a == (pT[4]<<2)+  ( pT[iSrcStride+4] - pT[4])* iYFrac@
@        k0 == ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4@
@        k1 == ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4@
@        *(I16_WMV *)(pD+2) == (U8_WMV) k0@
@        *(I16_WMV *)(pD+22) == (U8_WMV) k1@

    ldrb r6, [ r0]@
    ldrb r9, [ r12]@
    ldrb r7, [ r0, #1]@
    ldrb r10, [ r12, #1]@
        
	sub	r9, r9, r6
    ldrb r8, [ r0, #2]@
	mul	r9, r4, r9
    
	sub	r10, r10, r7
    ldrb r11, [ r12, #2]@

	mul	r10, r4, r10
	sub	r11, r11, r8
	add r9, r9, r6, lsl #2		@a

	mul	r11, r4, r11
	add r10, r10, r7, lsl #2	@b
	sub	r6, r10, r9
	add r11, r11, r8, lsl #2	@c

	mul	r6, r3, r6
	sub	r7, r11, r10
	add	r9, r5, r9, lsl #2
	add	r6, r6, r9

	mul	r7, r3, r7
	mov	r6, r6, asr #4			@k0
    strh r6, [r2]
	add	r10, r5, r10, lsl #2
	add	r7, r7, r10


@@@@@@@@@@@@@@@@@@@@@
    ldrb r6, [ r0, #3]@
    ldrb r9, [ r12, #3]@
    
	mov	r7, r7, asr #4			@k1
    strh r7, [r2, #20]
    ldrb r7, [ r0, #4]@
	sub	r9, r9, r6
    ldrb r10, [ r12, #4]@
   
	mul	r9, r4, r9
	sub	r10, r10, r7 
		
	mul	r10, r4, r10
	add r6, r9, r6, lsl #2		@d
	sub	r8, r6, r11
	add r9, r10, r7, lsl #2		@a

	mul	r8, r3, r8
	add	r7, r5, r11, lsl #2
	sub	r10, r9, r6
	add	r8, r8, r7

	mul	r10, r3, r10
	mov	r8, r8, asr #4			@k0
	add	r6, r5, r6, lsl #2
	add	r10, r10, r6
    strh r8, [r2, #2]
	mov	r10, r10, asr #4			@k1

    strh r10, [r2, #22]

@//1
@        b == (pT[5]<<2)+  ( pT[iSrcStride+5] - pT[5])* iYFrac@
@        c == (pT[6]<<2)+  ( pT[iSrcStride+6] - pT[6])* iYFrac@
@        k0 == ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4@
@        k1 == ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4@
@        *(I16_WMV *)(pD+4) == (U8_WMV) k0@
@        *(I16_WMV *)(pD+24) == (U8_WMV) k1@
@
@        d == (pT[7]<<2)+  ( pT[iSrcStride+7] - pT[7])* iYFrac@
@        a == (pT[8]<<2)+  ( pT[iSrcStride+8] - pT[8])* iYFrac@
@        k0 == ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4@
@        k1 == ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4@
@        *(I16_WMV *)(pD+6) == (U8_WMV) k0@
@        *(I16_WMV *)(pD+26) == (U8_WMV) k1@
    ldrb r7, [ r0, #5]@
    ldrb r10, [ r12, #5]@
    ldrb r8, [ r0, #6]@
    ldrb r11, [ r12, #6]@

	sub	r10, r10, r7
	sub	r11, r11, r8	
	mul	r10, r4, r10

	mul	r11, r4, r11
	add r10, r10, r7, lsl #2	@b
	sub	r6, r10, r9
	
	add r11, r11, r8, lsl #2	@c
	mul	r6, r3, r6

	sub	r7, r11, r10
	add	r9, r5, r9, lsl #2

	mul	r7, r3, r7
	add	r10, r5, r10, lsl #2
	add	r6, r6, r9
	add	r7, r7, r10
	mov	r6, r6, asr #4			@k0
	mov	r7, r7, asr #4			@k1

    strh r6, [r2, #4]
    
@@@@@@@@@@@@@@@@@@@@@
    ldrb r6, [ r0, #7]@
    ldrb r9, [ r12, #7]@
    strh r7, [r2, #24]    
    ldrb r7, [ r0, #8]@
	sub	r9, r9, r6
    ldrb r10, [ r12, #8]@


	mul	r9, r4, r9
	sub	r10, r10, r7
	subs r14, r14, #1
	add r6, r9, r6, lsl #2		@d
	
	sub	r8, r6, r11
	mul	r10, r4, r10
	
	mul	r8, r3, r8
	add r9, r10, r7, lsl #2		@a
	add	r7, r5, r11, lsl #2
	add	r8, r8, r7
	sub	r10, r9, r6
	mov	r8, r8, asr #4			@k0

	mul	r10, r3, r10
    strh r8, [r2, #6]
	add	r6, r5, r6, lsl #2
	add r0, r0, r1
	
	add	r10, r10, r6
	.if PLD_ENABLE == 1
	pld	[r0, #32]
	.endif
	
	add r12, r12, r1
	mov	r10, r10, asr #4			@k1

    strh r10, [r2, #26]

@        pT +== iSrcStride@
@        pD +== 40@
	add r2, r2, #40
	bne	loop_g_InterpolateBlockBilinear_SSIMD

    ldmia     sp!, {r4 - r11, pc}


lab_16_loop:
loop_g_InterpolateBlockBilinear_SSIMD_16:

@        a == (pT[0]<<2)+  ( pT[iSrcStride] - pT[0])* iYFrac@
@//0
@        b == (pT[1]<<2)+  ( pT[iSrcStride+1] - pT[1])* iYFrac@
@        c == (pT[2]<<2)+  ( pT[iSrcStride+2] - pT[2])* iYFrac@
@        k0 == ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4@
@        k1 == ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4@
@        *(I16_WMV *)(pD+0) == (U8_WMV) k0@
@        *(I16_WMV *)(pD+20) == (U8_WMV) k1@
@
@        d == (pT[3]<<2)+  ( pT[iSrcStride+3] - pT[3])* iYFrac@
@        a == (pT[4]<<2)+  ( pT[iSrcStride+4] - pT[4])* iYFrac@
@        k0 == ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4@
@        k1 == ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4@
@        *(I16_WMV *)(pD+2) == (U8_WMV) k0@
@        *(I16_WMV *)(pD+22) == (U8_WMV) k1@


    ldrb r6, [ r0]@
    ldrb r9, [ r12]@
    ldrb r7, [ r0, #1]@
    ldrb r10, [ r12, #1]@

	sub	r9, r9, r6
    ldrb r8, [ r0, #2]@
    ldrb r11, [ r12, #2]@
	mul	r9, r4, r9
	sub	r10, r10, r7

	sub	r11, r11, r8
	mul	r10, r4, r10
	mul	r11, r4, r11
	add r9, r9, r6, lsl #2		@a
	add r10, r10, r7, lsl #2	@b

	sub	r6, r10, r9
	add r11, r11, r8, lsl #2	@c

	mul	r6, r3, r6
	add	r9, r5, r9, lsl #2
	sub	r7, r11, r10
	add	r6, r6, r9

	mul	r7, r3, r7
	mov	r6, r6, asr #4			@k0
	add	r10, r5, r10, lsl #2
	add	r7, r7, r10
    strh r6, [r2]



@@@@@@@@@@@@@@@@@@@@@
    ldrb r6, [ r0, #3]@
    ldrb r9, [ r12, #3]@
	mov	r7, r7, asr #4			@k1
    strh r7, [r2, #20]
    
    ldrb r7, [ r0, #4]@
	sub	r9, r9, r6
    ldrb r10, [ r12, #4]@

	mul	r9, r4, r9
	sub	r10, r10, r7
	sub	r8, r6, r11
	add r6, r9, r6, lsl #2		@d

	mul	r10, r4, r10

	mul	r8, r3, r8
	add r9, r10, r7, lsl #2		@a

	sub	r10, r9, r6
	add	r7, r5, r11, lsl #2
	mul	r10, r3, r10

	add	r6, r5, r6, lsl #2
	add	r8, r8, r7
	add	r10, r10, r6
	mov	r8, r8, asr #4			@k0
	mov	r10, r10, asr #4			@k1


@//1
@        b == (pT[5]<<2)+  ( pT[iSrcStride+5] - pT[5])* iYFrac@
@        c == (pT[6]<<2)+  ( pT[iSrcStride+6] - pT[6])* iYFrac@
@        k0 == ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4@
@        k1 == ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4@
@        *(I16_WMV *)(pD+4) == (U8_WMV) k0@
@        *(I16_WMV *)(pD+24) == (U8_WMV) k1@
@
@        d == (pT[7]<<2)+  ( pT[iSrcStride+7] - pT[7])* iYFrac@
@        a == (pT[8]<<2)+  ( pT[iSrcStride+8] - pT[8])* iYFrac@
@        k0 == ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4@
@        k1 == ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4@
@        *(I16_WMV *)(pD+6) == (U8_WMV) k0@
@        *(I16_WMV *)(pD+26) == (U8_WMV) k1@
    ldrb r7, [ r0, #5]@
    strh r10, [r2, #22]
    ldrb r10, [ r12, #5]@
    ldrb r11, [ r12, #6]@
    strh r8, [r2, #2]
 	sub	r10, r10, r7
    ldrb r8, [ r0, #6]@

	mul	r10, r4, r10
	sub	r11, r11, r8
	add	r9, r5, r9, lsl #2
	add r10, r10, r7, lsl #2	@b
	
	sub	r6, r10, r9
	mul	r11, r4, r11
	mul	r6, r3, r6
	
	add r11, r11, r8, lsl #2	@c
	sub	r7, r11, r10
	add	r6, r6, r9
	
	mul	r7, r3, r7
	mov	r6, r6, asr #4			@k0

	add	r10, r5, r10, lsl #2

@@@@@@@@@@@@@@@@@@@@@
    strh r6, [r2, #4]
    ldrb r6, [ r0, #7]@
	add	r7, r7, r10
    ldrb r9, [ r12, #7]@
	mov	r7, r7, asr #4			@k1

    strh r7, [r2, #24]
    ldrb r7, [ r0, #8]@
	sub	r9, r9, r6
    ldrb r10, [ r12, #8]@

	mul	r9, r4, r9
	sub	r10, r10, r7
	add	r7, r5, r11, lsl #2
	add r6, r9, r6, lsl #2		@d

	sub	r8, r6, r11
	mul	r10, r4, r10
	mul	r8, r3, r8
	
	add r9, r10, r7, lsl #2		@a
	sub	r10, r9, r6
	add	r8, r8, r7

	mul	r10, r3, r10
	mov	r8, r8, asr #4			@k0

	add	r6, r5, r6, lsl #2


@//2
@        b == (pT[9]<<2)+  ( pT[iSrcStride+9] - pT[9])* iYFrac@
@        c == (pT[10]<<2)+  ( pT[iSrcStride+10] - pT[10])* iYFrac@
@        k0 == ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4@
@        k1 == ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4@
@        *(I16_WMV *)(pD+8) == (U8_WMV) k0@
@        *(I16_WMV *)(pD+28) == (U8_WMV) k1@
@
@        d == (pT[11]<<2)+  ( pT[iSrcStride+11] - pT[11])* iYFrac@
@        a == (pT[12]<<2)+  ( pT[iSrcStride+12] - pT[12])* iYFrac@
@        k0 == ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4@
@        k1 == ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4@
@        *(I16_WMV *)(pD+10) == (U8_WMV) k0@
@        *(I16_WMV *)(pD+30) == (U8_WMV) k1@

 	add	r10, r10, r6
    ldrb r7, [ r0, #9]@
	mov	r10, r10, asr #4			@k1
    strh r10, [r2, #26]
    ldrb r10, [ r12, #9]@
    strh r8, [r2, #6]    
    ldrb r8, [ r0, #10]@
	sub	r10, r10, r7
    ldrb r11, [ r12, #10]@

	mul	r10, r4, r10
	sub	r11, r11, r8
	add	r9, r5, r9, lsl #2

	mul	r11, r4, r11
	sub	r6, r10, r9
	add r10, r10, r7, lsl #2	@b

	mul	r6, r3, r6
	add r11, r11, r8, lsl #2	@c
	sub	r7, r11, r10
	add	r6, r6, r9
	
	mul	r7, r3, r7
	mov	r6, r6, asr #4			@k0

	add	r10, r5, r10, lsl #2
	add	r7, r7, r10
    strh r6, [r2, #8]

@@@@@@@@@@@@@@@@@@@@@
    ldrb r6, [ r0, #11]@
    ldrb r9, [ r12, #11]@
	mov	 r7, r7, asr #4			@k1
    strh r7, [r2, #28]
    ldrb r7, [ r0, #12]@

	sub	r9, r9, r6
    ldrb r10, [ r12, #12]@
    
	mul	r9, r4, r9
	sub	r10, r10, r7
	add	r7, r5, r11, lsl #2

	mul	r10, r4, r10
	add r6, r9, r6, lsl #2		@d
	sub	r8, r6, r11
	add r9, r10, r7, lsl #2		@a

	mul	r8, r3, r8
	sub	r10, r9, r6
	add	r6, r5, r6, lsl #2
	
	mul	r10, r3, r10
	add	r8, r8, r7
    ldrb r7, [ r0, #13]@
	add	r10, r10, r6
	
	mov	r8, r8, asr #4			@k0
	mov	r10, r10, asr #4			@k1


@//3
@        b == (pT[13]<<2)+  ( pT[iSrcStride+13] - pT[13])* iYFrac@
@        c == (pT[14]<<2)+  ( pT[iSrcStride+14] - pT[14])* iYFrac@
@        k0 == ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4@
@        k1 == ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4@
@        *(I16_WMV *)(pD+12) == (U8_WMV) k0@
@        *(I16_WMV *)(pD+32) == (U8_WMV) k1@
@
@        d == (pT[15]<<2)+  ( pT[iSrcStride+15] - pT[15])* iYFrac@
@        a == (pT[16]<<2)+  ( pT[iSrcStride+16] - pT[16])* iYFrac@
@        k0 == ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4@
@        k1 == ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4@
@        *(I16_WMV *)(pD+14) == (U8_WMV) k0@
@        *(I16_WMV *)(pD+34) == (U8_WMV) k1@


    strh r10, [r2, #30]
    ldrb r10, [ r12, #13]@
    strh r8, [r2, #10]
    ldrb r8, [ r0, #14]@
 	sub	r10, r10, r7
    ldrb r11, [ r12, #14]@

	mul	r10, r4, r10
	sub	r11, r11, r8
	subs r14, r14, #1
	sub	r6, r10, r9

	mul	r11, r4, r11
	add r10, r10, r7, lsl #2	@b
	mul	r6, r3, r6
	add r11, r11, r8, lsl #2	@c

	add	r9, r5, r9, lsl #2
	sub	r7, r11, r10
	add	r6, r6, r9
	mul	r7, r3, r7
	mov	r6, r6, asr #4			@k0

	add	r10, r5, r10, lsl #2
	add	r7, r7, r10
    strh r6, [r2, #12]
	mov	r7, r7, asr #4			@k1


@@@@@@@@@@@@@@@@@@@@@
    ldrb r6, [ r0, #15]@
    ldrb r9, [ r12, #15]@
    strh r7, [r2, #32]
    ldrb r7, [ r0, #16]@
	sub	r9, r9, r6
    ldrb r10, [ r12, #16]@
	
	mul	r9, r4, r9
	add r0, r0, r1
	
	sub	r10, r10, r7
	.if PLD_ENABLE == 1
	pld	[r0, #32]
	.endif

	add r6, r9, r6, lsl #2		@d

	mul	r10, r4, r10
	add r12, r12, r1
	sub	r8, r6, r11
	add r9, r10, r7, lsl #2		@a

	mul	r8, r3, r8
	sub	r10, r9, r6
	add	r7, r5, r11, lsl #2
	add	r8, r8, r7
	
	mul	r10, r3, r10
	mov	r8, r8, asr #4			@k0

	add	r6, r5, r6, lsl #2
    strh r8, [r2, #14]
	add	r10, r10, r6
	add r2, r2, #40
	mov	r10, r10, asr #4			@k1

    strh r10, [r2, #34]
@        pT +== iSrcStride@
@        pD +== 40@

	bne	loop_g_InterpolateBlockBilinear_SSIMD_16

    ldmia     sp!, {r4 - r11, pc}
    	
    WMV_ENTRY_END	@g_InterpolateBlockBilinear_SSIMD


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@Void_WMV  g_InterpolateBlockBilinear_SSIMD_11 (const U8_WMV *pSrc, 
@                                           I32_WMV iSrcStride, 
@                                           U8_WMV *pDst, 
@                                           I32_WMV iXFrac, 
@                                           I32_WMV iYFrac, 
@                                           I32_WMV iRndCtrl, 
@                                           Bool_WMV b1MV)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA  |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY g_InterpolateBlockBilinear_SSIMD_11
	
@r0==pSrc
@r1==iSrcStride
@r2==pDst
@r3==iRndCtl
@r4==i@
@r5==j
@r6==PF0
@r7==PF1
@r8==PF2
@r9==PF3
@r10==PF4
@r14==iNumLoops
@r11, r12 == tmp

@ stack usage:
.equ IBB_11_OffsetRegSaving        , 36
.equ IBB_11_Offset_iYFrac          , IBB_11_OffsetRegSaving + 0
.equ IBB_11_Offset_iRndCtrl        , IBB_11_OffsetRegSaving + 4
.equ IBB_11_Offset_b1MV            , IBB_11_OffsetRegSaving + 8

    stmdb     sp!, {r4 - r11, r14}
    FRAME_PROFILE_COUNT
 
@	I32_WMV iNumLoops == 8<<b1MV@
@	iRndCtrl == 8 - ( iRndCtrl&0xff)@

	.if PLD_ENABLE == 1
	pld		[r0]
	pld		[r0, r1]
	.endif
	
	ldr		r3 , [sp, #IBB_11_Offset_iRndCtrl]
	ldr		r12, [sp, #IBB_11_Offset_b1MV]
	mov		r14, #8
	and		r3, r3, #0xff
	rsb		r3, r3, #8			@iRndCtrl
	mov		r14, r14, lsl r12
	mov		r4, r14	
	
IBB_11_OuterLoop:

 @   for (i == 0@ i < iNumLoops@ i++) 
 @   {
 @       const U8_WMV  *pT @
	@	I16_WMV PF0, PF1, PF2, PF3, PF4@
        
    @    PF0 == pSrc[0] + pSrc[iSrcStride]@
        
    ldrb	r6 , [r0]
    ldrb	r8 , [r0, r1]
	mov		r5, r14
	.if PLD_ENABLE == 1
	pld		[r0, r1, lsl #1]
	.endif
	add		r6, r6, r8
IBB_11_InnerLoop:
    @    for (j == 0@ j < iNumLoops@ j +== 4) {

    @        pT == pSrc + j@
	@		PF1 == pT[1] + pT[iSrcStride+1]@
	@		PF2 == pT[2] + pT[iSrcStride+2]@
	@		PF3 == pT[3] + pT[iSrcStride+3]@
	@		PF4 == pT[4] + pT[iSrcStride+4]@
	
	add		r12, r0, r1
    ldrb	r7 , [r0, #1]
    ldrb	r9 , [r12, #1]
    ldrb	r8, [r0, #2]
    ldrb	r10, [r12, #2]
    
	add		r7, r7, r9
    ldrb	r9, [r0, #3]
    ldrb	r11, [r12, #3]
    
	add		r8, r8, r10
    ldrb	r10, [r0, #4]!
    ldrb	r12, [r12, #4]
    
	add		r9, r9, r11

     @       k0 == (((PF0 + PF1) << 2) + iRndCtrl) >> 4@
     @       k1 == (((PF1 + PF2) << 2) + iRndCtrl) >> 4@
     @       *(I16_WMV *)(pDst + j) == (U8_WMV) k0@
     @       *(I16_WMV *)(pDst + j + 20) == (U8_WMV) k1@
     
    add		r6, r6, r7
    add		r7, r7, r8
    add		r10, r10, r12
	add		r6, r3, r6, lsl #2
	add		r7, r3, r7, lsl #2
	mov		r6, r6, asr #4
	mov		r7, r7, asr #4
    strh	r7, [r2, #20]

     @       k0 == (((PF2 + PF3) << 2) + iRndCtrl) >> 4@
     @       k1 == (((PF3 + PF4) << 2) + iRndCtrl) >> 4@
     @       *(I16_WMV *)(pDst + j + 2) == (U8_WMV) k0@
     @       *(I16_WMV *)(pDst + j + 22) == (U8_WMV) k1@
     
    add		r8, r8, r9
    add		r9, r9, r10
	add		r8, r3, r8, lsl #2
	add		r9, r3, r9, lsl #2
	mov		r8, r8, asr #4
	mov		r9, r9, asr #4
    strh	r8, [r2, #2]
    strh	r9, [r2, #22]
    strh	r6, [r2], #4
    
		@	PF0 == PF4@
	mov		r6, r10
		
	subs	r5, r5, #4
	bne		IBB_11_InnerLoop
      @  }
        
      @  pSrc +== iSrcStride@
      @  pDst +== 40@
 
	sub		r0, r0, r14
	sub		r2, r2, r14
	add		r0, r0, r1
	add		r2, r2, #40
	
	subs	r4, r4, #1	
	bne		IBB_11_OuterLoop
   @ }

	ldmia     sp!, {r4 - r11, pc}
@}	
	
	WMV_ENTRY_END	@g_InterpolateBlockBilinear_SSIMD_11



@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@Void_WMV  g_InterpolateBlockBilinear_SSIMD_10 (const U8_WMV *pSrc, 
@                                           I32_WMV iSrcStride, 
@                                           U8_WMV *pDst, 
@                                           I32_WMV iXFrac, 
@                                           I32_WMV iYFrac, 
@                                           I32_WMV iRndCtrl, 
@                                           Bool_WMV b1MV)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA  |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY g_InterpolateBlockBilinear_SSIMD_10

@r0==pSrc
@r1==iSrcStride
@r2==pDst
@r3==iRndCtl
@r4==i@
@r5==j
@r6==PF0
@r7==PF1
@r8==PF2
@r9==PF3
@r10==PF4
@r14 == iNumLoops
@r12 == tmp

@ stack usage:
.equ IBB_10_OffsetRegSaving        , 32
.equ IBB_10_Offset_iYFrac          ,IBB_10_OffsetRegSaving + 0
.equ IBB_10_Offset_iRndCtrl        ,IBB_10_OffsetRegSaving + 4
.equ IBB_10_Offset_b1MV            ,IBB_10_OffsetRegSaving + 8

    stmdb     sp!, {r4 - r10, r14}
    FRAME_PROFILE_COUNT

 @   I32_WMV iNumLoops == 8<<b1MV@
 @   iRndCtrl == 8 - ( iRndCtrl&0xff)@
 
	.if PLD_ENABLE == 1
	pld		[r0]
	.endif
	
	ldr		r3 , [sp, #IBB_10_Offset_iRndCtrl]
	ldr		r12, [sp, #IBB_10_Offset_b1MV]
	mov		r14, #8
	and		r3, r3, #0xff
	rsb		r3, r3, #8			@iRndCtrl
	mov		r14, r14, lsl r12
	mov		r4, r14	
	
IBB_10_OuterLoop:
   
 @   for (i == 0@ i < iNumLoops@ i++) 
 @   {
 @       const U8_WMV  *pT @
	@	I16_WMV PF0, PF1, PF2, PF3, PF4@

    @    PF0 == pSrc[0]@

	.if PLD_ENABLE == 1
	pld		[r0, r1]
	.endif

	mov		r5, r14
	ldrb	r6, [r0]
IBB_10_InnerLoop:

    @    for (j == 0@ j < iNumLoops@ j +== 4) {

     @       pT == pSrc + j@
     @       PF1 == pT[1]@
     @       PF2 == pT[2]@
     @       PF3 == pT[3]@
     @       PF4 == pT[4]@
     
	ldrb	r7, [r0, #1]
	ldrb	r8, [r0, #2]
	ldrb	r9, [r0, #3]
	ldrb	r10, [r0, #4]!

      @      k0 == (((PF0 + PF1 ) << 3) + iRndCtrl) >> 4@
      @      k1 == (((PF1 + PF2 ) << 3) + iRndCtrl) >> 4@
      @      *(I16_WMV *)(pDst + j) == (U8_WMV) k0@
      @      *(I16_WMV *)(pDst + j + 20) == (U8_WMV) k1@
      
    add		r6, r6, r7
    add		r7, r7, r8
    add		r6, r3, r6, lsl #3
    add		r7, r3, r7, lsl #3
    mov		r6, r6, asr #4
    mov		r7, r7, asr #4
    strh	r7, [r2, #20]

      @      k0 == (((PF2 + PF3 ) << 3) + iRndCtrl) >> 4@
      @      k1 == (((PF3 + PF4 ) << 3) + iRndCtrl) >> 4@
      @      *(I16_WMV *)(pDst + j + 2) == (U8_WMV) k0@
      @      *(I16_WMV *)(pDst + j + 22) == (U8_WMV) k1@

    add		r8, r8, r9
    add		r9, r9, r10
    add		r8, r3, r8, lsl #3
    add		r9, r3, r9, lsl #3
    mov		r8, r8, asr #4
    mov		r9, r9, asr #4
    strh	r8, [r2, #2]
    strh	r9, [r2, #22]
    strh	r6, [r2], #4

		@	PF0 == PF4@		
    mov		r6, r10
    
	subs	r5, r5, #4
	bne		IBB_10_InnerLoop
       @ }
        
      @  pSrc +== iSrcStride@
      @  pDst +== 40@

	sub		r0, r0, r14
	sub		r2, r2, r14
	add		r0, r0, r1
	add		r2, r2, #40
	
	subs	r4, r4, #1
	bne		IBB_10_OuterLoop
   @ }
    
	ldmia     sp!, {r4 - r10, pc}
	
	WMV_ENTRY_END	@g_InterpolateBlockBilinear_SSIMD_10

    
    
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@Void_WMV  g_InterpolateBlockBilinear_SSIMD_01 (const U8_WMV *pSrc, 
@                                           I32_WMV iSrcStride, 
@                                           U8_WMV *pDst, 
@                                           I32_WMV iXFrac, 
@                                           I32_WMV iYFrac, 
@                                           I32_WMV iRndCtrl, 
@                                           Bool_WMV b1MV)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA  |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY g_InterpolateBlockBilinear_SSIMD_01

@r5==j
@r6==PF0
@r7==PF1
@r8==PF2
@r9==PF3
@r10, r11, r12 == tmp

@ stack usage:
.equ IBB_01_OffsetRegSaving        ,	36
.equ IBB_01_Offset_iYFrac          ,	IBB_01_OffsetRegSaving + 0
.equ IBB_01_Offset_iRndCtrl        ,	IBB_01_OffsetRegSaving + 4
.equ IBB_01_Offset_b1MV            ,	IBB_01_OffsetRegSaving + 8

    stmdb     sp!, {r4 - r11, r14}
    FRAME_PROFILE_COUNT

  @  I32_WMV iNumLoops == 8<<b1MV@    
  @  iRndCtrl == 8 - ( iRndCtrl&0xff)@

	.if PLD_ENABLE == 1
	pld		[r0]
	pld		[r0, r1]
	.endif
	
	ldr		r3 , [sp, #IBB_01_Offset_iRndCtrl]
	ldr		r12, [sp, #IBB_01_Offset_b1MV]
	mov		r14, #8
	and		r3, r3, #0xff
	rsb		r3, r3, #8			@iRndCtrl
	mov		r14, r14, lsl r12
	mov		r4, r14	
	
IBB_01_OuterLoop:
  @  for (i == 0@ i < iNumLoops@ i++) 
  @  {

	mov		r5, r14
IBB_01_InnerLoop:

   @     for (j == 0@ j < iNumLoops@ j +== 4) {
		@	 pT == pSrc + j@
        @    PF0 == pT[0] + pT[iSrcStride+0]@
        @    PF1 == pT[1] + pT[iSrcStride+1]@
        @    PF2 == pT[2] + pT[iSrcStride+2]@
        @    PF3 == pT[3] + pT[iSrcStride+3]@
        
    add		r12, r0, r1
    ldrb	r8 , [r0, #2]
    ldrb	r9 , [r12, #2]
    ldrb	r10, [r12]
    ldrb	r7 , [r0, #1]
    ldrb	r11, [r12, #1]
    add		r8 , r8, r9
    ldrb	r9 , [r0, #3]
    ldrb	r12, [r12, #3]
    ldrb	r6 , [r0], #4

         @   k0 == ((PF0 << 3) + iRndCtrl) >> 4@
         @   k1 == ((PF1 << 3) + iRndCtrl) >> 4@
         @   *(I16_WMV *)(pDst + j) == (U8_WMV) k0@
         @   *(I16_WMV *)(pDst + j + 20) == (U8_WMV) k1@

         @   k0 == ((PF2 << 3) + iRndCtrl) >> 4@
         @   k1 == ((PF3 << 3) + iRndCtrl) >> 4@
         @   *(I16_WMV *)(pDst + j + 2) == (U8_WMV) k0@
         @   *(I16_WMV *)(pDst + j + 22) == (U8_WMV) k1@
         
    add		r8, r3, r8, lsl #3
    add		r7, r7, r11
    add		r9, r9, r12
    add		r7, r3, r7, lsl #3
    add		r6, r6, r10
    add		r9, r3, r9, lsl #3
    add		r6, r3, r6, lsl #3
    
    mov		r7, r7, asr #4
    mov		r6, r6, asr #4
    mov		r8, r8, asr #4
    mov		r9, r9, asr #4
    strh	r7, [r2, #20]
    strh	r8, [r2, #2]
    strh	r9, [r2, #22]
    strh	r6, [r2], #4

	subs	r5, r5, #4
	bne		IBB_01_InnerLoop
  @      }


 @   pSrc +== iSrcStride@
 @   pDst +== 40@
 
	sub		r0, r0, r14
	sub		r2, r2, r14
	add		r0, r0, r1
	add		r2, r2, #40
	
	subs	r4, r4, #1

	.if PLD_ENABLE == 1
	pld		[r0, r1]
	.endif
	
	bne		IBB_01_OuterLoop
	
@	}
	
    ldmia     sp!, {r4 - r11, pc}
    
    WMV_ENTRY_END	@g_InterpolateBlockBilinear_SSIMD_01


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@U32_WMV g_NewVertFilterX(const U8_WMV  *pSrc,
@                             const I32_WMV iSrcStride, 
@                             U8_WMV * pDst, 
@                             const I32_WMV iShift, 
@                             const I32_WMV iRound32, 
@                             const I8_WMV * const pV, 
@                             I32_WMV iNumHorzLoop, 
@                             const U32_WMV uiMask,
@                             Bool_WMV b1MV
@                             )
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA  |.embsec_PMainLoopLvl1|, CODE
	    
    WMV_LEAF_ENTRY g_NewVertFilterX

    stmdb     sp!, {r4 - r11, r14}
    FRAME_PROFILE_COUNT

@pSrc == r0
@iSrcStride == r1
@pDst == r2
@iShift == r3
@m==r4
@o1==r5
@o2==r6
@o3==r7
@v0 == r8
@v1 == r12@
@v3 == r9
@overflow == r10
@t0 == r11@

@r14 == tmp@

@@ stack usage:
.equ VFX_StackSize              , 0xc
.equ VFX_OffsetRegSaving        , 0x24
.equ VFX_Stack_iNumHorzLoop   , 0
.equ VFX_Stack_iNumInnerLoop  , 4
.equ VFX_Stack_v2             , 0x8
@VFX_Stack_pDstUpdate     EQU 0xc
@VFX_Stack_pSrcUpdate     EQU 0x10


.equ VFX_Offset_iRound32        , VFX_StackSize + VFX_OffsetRegSaving + 0
.equ VFX_Offset_pV              , VFX_StackSize + VFX_OffsetRegSaving + 4
.equ VFX_Offset_iNumHorzLoop    , VFX_StackSize + VFX_OffsetRegSaving + 8
.equ VFX_Offset_uiMask          , VFX_StackSize + VFX_OffsetRegSaving + 0xc
.equ VFX_Offset_b1MV            , VFX_StackSize + VFX_OffsetRegSaving + 0x10
.equ VFX_Offset_pTbl	             , VFX_StackSize + VFX_OffsetRegSaving + 0x14

@  for(k == 0@ k < (iNumHorzLoop<<1)@ k++)
    
    ldr r8, [sp, #VFX_Offset_iNumHorzLoop - VFX_StackSize ]
    ldr r4, [sp, #VFX_Offset_b1MV - VFX_StackSize]
    ldr r5, [sp, #VFX_Offset_pV - VFX_StackSize ]
    mov  r6, #1    
	mov r8, r8, lsl #3



    @ I32_WMV iNumInnerLoop == 1<<(3+b1MV)@
    add  r4, r4, #3  @ 3+b1MV
    str r8, [sp, #-VFX_StackSize]!    
    mov  r6, r6, lsl r4 
   @sub  r11, r6, #1                                    @ adjust iNumInnerLoop since it will be in iNumInnerLoop,,iShift
    
    sub  r3, r3, #0x10000
    mov r11, r6, lsl #16
    str  r11, [sp, #VFX_Stack_iNumInnerLoop]

    ldrsb r12, [ r5, #1] @relo11

    @v1@
    @v2 == pV[2]@
    ldrsb r7, [ r5, #2]
    
    @v0, v3
    ldrsb r8, [ r5, #0]
    ldrsb r9, [ r5, #3]
    str r7, [sp, #VFX_Stack_v2] @relo12

    @U32_WMV overflow == 0@
    mov r10, #0

     @ for(k == 0@ k < (iNumHorzLoop<<1)@ k++)
     @{

g_NewVertFilterX_outloop:
        
			.if PLD_ENABLE == 1
                PLD [r0, #32]
            .endif

            ldrb r14, [r0, #2]                          @o1 == pSrc[0] | (pSrc[2]<<16)@
            ldrb r5,  [r0], +r1                         @pSrc +== iSrcStride@
            
			.if PLD_ENABLE == 1
                PLD [r0, #32]
            .endif

            ldrb r11, [r0, #2]                          @o2 == pSrc[0] | (pSrc[2]<<16)@
            ldrb r6,  [r0], +r1                         @pSrc +== iSrcStride@
            
            orr r5, r5, r14, lsl #16
			.if PLD_ENABLE == 1
                PLD [r0, #32]
            .endif
           
            ldrb r14, [r0, #2]                          @o3 == pSrc[0] | (pSrc[2]<<16)@
            ldrb r7,  [r0], +r1                         @pSrc +== iSrcStride@
            orr r6, r6, r11, lsl #16                     
            
            orr r7, r7, r14, lsl #16

            ldr r14, [sp, #VFX_Stack_iNumInnerLoop]
            ldr r4,  [sp, #VFX_Offset_iRound32]
	        add  r3, r3, r14

           @ for(m == 0@ m < iNumInnerLoop@ m++)
           @ {
g_NewVertFilterX_innerloop:
                
				.if PLD_ENABLE == 1
                    PLD [r0, #32]
                .endif

                mla  r11, r5, r8, r4                    @ t0 == o1*v0 + iRound32@
                ldr  r14, [sp, #VFX_Stack_v2]           @ v2
                
                mla  r11, r6, r12, r11                  @ t0 +== o2*v1@
			    ldrb r5, [r0, #2]         @relo100             @ o1 == pSrc[0] | (pSrc[2]<<16)@
               
                mla  r11, r7, r14, r11                  @ t0 +== o3*v2@
                ldrb r14,  [r0], +r1                     @ pSrc +== iSrcStride@
                
				.if PLD_ENABLE == 1
                    PLD [r0, #32]
                .endif            

                orr  r5, r14, r5, lsl #16
                mla  r11, r5, r9, r11                   @ t0 +== o1*v3@
            
                ldr  r14, [sp, #VFX_Offset_uiMask]      @ uiMask
                
                orr  r10, r10, r11                      @ overflow |== t0@

			    and  r11, r14, r11, lsr r3              @ t0 ==(t0>>iShift)&uiMask

				mla  r14, r6, r8, r4                    @ t0 == o2*v0 + iRound32@
                str  r11, [r2], #+40                    @ *(U32_WMV *)pDst == t0@
                                                        @ pDst +== 40@
              @  bpl  g_NewVertFilterX_innerloop

								
                ldr  r11, [sp, #VFX_Stack_v2]           @ v2
                
                mla  r14, r7, r12, r14                  @ t0 +== o3*v1@

                mov  r6, r5                             @ o2 == o1@
               
                mla  r14, r5, r11, r14                  @ t0 +== o1*v2@
                
                ldrb r11, [r0, #2]                      @ o3 == pSrc[0] | (pSrc[2]<<16)@
                
				mov  r5, r7                             @ o1 == o3@
				
				ldrb r7,  [r0], +r1                     @ pSrc +== iSrcStride@

                subs r3, r3, #0x20000                   @ m++
                             
                orr  r7, r7, r11, lsl #16

                mla  r14, r7, r9, r14                   @ t0 +== o3*v3@
            
                ldr  r11, [sp, #VFX_Offset_uiMask]      @ uiMask
                
                orr  r10, r10, r14                      @ overflow |== t0@ 

		        and r14, r11, r14, lsr r3               @ t0 ==(t0>>iShift)&uiMask

                str  r14, [r2], #+40                    @ *(U32_WMV *)pDst == t0@
                                                        @ pDst +== 40@
                bpl  g_NewVertFilterX_innerloop
           @ }

@/*
@            pSrc +== pTbl[0] - 3*iSrcStride - (iSrcStride<<(3+b1MV)) @
@            pDst +== pTbl[1] - (40<<(3+b1MV))@
@            pTbl +== 2@
@*/

        @if(k&1)
        ldr  r14, [sp, #VFX_Stack_iNumHorzLoop]
		ldr  r11, [sp, #VFX_Offset_pTbl]
		and r5, r14, #4
		ldrh  r6, [r11, r5] !
		ldrh  r7, [r11, #2] 

		subs r14, r14, #4
		sub r0, r0, r6
		sub  r2, r2, r7
		
        str  r14, [sp, #VFX_Stack_iNumHorzLoop]
        bgt  g_NewVertFilterX_outloop
    @}

     @return overflow@

     add  sp, sp, #VFX_StackSize
     mov  r0, r10
     ldmia     sp!, {r4 - r11, pc}
@}

     WMV_ENTRY_END
     @ENDP  @  g_NewVertFilterX


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@    U32_WMV g_NewHorzFilterX(U8_WMV *pF, 
@                              const I32_WMV iShift, 
@                              const I32_WMV iRound2_32, 
@                              const I8_WMV * const pH, 
@                              Bool_WMV b1MV)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    
    
    @AREA  |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY g_NewHorzFilterX
    
@r0 == pF
@r1 == iShift
@r2 == iRound2_32
@r3 == i
@r4 == h0
@r5 == h1
@r6 == h2
@r7 == h3
@r8 == o0
@r9 == o1
@r10 == t0@
@r11 == t1
@r12 == overflow@
@r14 == mask

@@ stack usage:
.equ HFX_StackSize         , 0x10
.equ HFX_OffsetRegSaving   , 0x24

.equ HFX_Offset_b1MV       , HFX_StackSize + HFX_OffsetRegSaving + 0

.equ HFX_Stack_iNumLoops   , 0
.equ HFX_Stack_j           , 4
.equ HFX_Stack_pFUpdate    , 8
.equ HFX_Stack_iRound2_32  , 12

    @I32_WMV j, i@
    @U32_WMV overflow == 0@
    @ register U32_WMV t0, t1@ //, t2, t3@
    @I32_WMV  iNumLoops == 1<<(3+b1MV)@
    
    @const I16_WMV h0 == pH[0]@
    @const I16_WMV h1 == pH[1]@
    @const I16_WMV h2 == pH[2]@
    @const I16_WMV h3 == pH[3]@

    stmdb     sp!, {r4 - r11, r14}
    FRAME_PROFILE_COUNT

   
    ldr r11, [sp, #HFX_Offset_b1MV - HFX_StackSize]
   
	.if PLD_ENABLE == 1
    pld	 [r0] 
	.endif
       
    mov r10, #8    
	str r2, [sp, #HFX_Stack_iRound2_32 - HFX_StackSize] 
	    
    mov r10, r10, lsl r11
    str r10, [sp, #HFX_Stack_j-HFX_StackSize]
    sub r10, r10, #1
    str r10, [sp, #HFX_Stack_iNumLoops-HFX_StackSize]!

    mov r9, #320
    ldrsb r4, [r3, #0]
    mov r9, r9, lsl r11
    sub r9, r9, #4
    str r9, [sp, #HFX_Stack_pFUpdate]

    ldrsb r5, [r3, #1]
    ldrsb r6, [r3, #2]
    ldrsb r7, [r3, #3]

    mov r14, #0xff
    mov r12, #0
    orr r14, r14, r14, lsl #16


HFX_Outerloop:
    @for (j == 0@ j < iNumLoops@ j +== 4) 
    @{

    ldr  r10, [sp, #HFX_Stack_iNumLoops]
    and  r1, r1, #0xff                          @ mask iShift
    orr  r1, r1, r10, lsl #16

HFX_InnerLoop:
    
    @  for(i == 0@ i < iNumLoops@ i++)
    @  {
    @        register I32_WMV o0@
    @        register I32_WMV o1@
           
    ldr  r10, [r0]                              @ t0 == *(I32_WMV *)pF@
	ldr  r2, [sp, #HFX_Stack_iRound2_32]    
    ldr  r11, [r0, #20]                         @ t1 == *(I32_WMV *)(pF + 20)@ 
    ldr  r3,  [r0, #4]                          @ t1 == (*(U32_WMV *)(pF+4))@
    mla  r8,  r10, r4, r2                       @ o0 == t0 * h0 + iRound2_32@

    mla  r9,  r11, r4, r2                       @ o1 == t1 * h0 + iRound2_32@
    mla  r8,  r11, r5, r8                       @ o0 +== t1 * h1@

    mov  r10, r10, lsr #16                       @ t0 == t0>>16@
    orr  r11, r10, r3, lsl #16                   @ t1 == t0 | (t1<<16)@
    ldrh r10, [r0, #22]                         @ t0 == *(U16_WMV *)(pF + 20 + 2)@  
    ldr  r2, [r0, #24]         @ t1 == (*(U32_WMV *)(pF+20+4))@
    
    mla  r8,  r11, r6, r8                       @ o0 +== t1 * h2@
    mla  r9,  r11, r5, r9                       @ o1 +== t1 * h1@
                  
    orr  r11, r10, r2, lsl #16                  @ t1 == t0 | (t1<<16)@
    subs r1,  r1, #0x10000
    
    @ ldr r10, [r0, #4]                         @ t0 == *(I32_WMV *)(pF+4)@  still in r3
    
    mla  r8,  r11, r7, r8                       @ o0 +== t1 * h3@
    
	.if PLD_ENABLE == 1
    pld	 [r0, #40]
	.endif
    
    mla  r9,  r11, r6, r9                       @ o1 +== t1 * h2@
    orr  r12, r12, r8                           @ overflow |== o0@
    mla  r9,  r3,  r7, r9                       @ o1 +== t0 * h3@

    and  r8,  r14, r8, lsr r1                       @ o0 >>= iShift@ o0 &== 0x00ff00ff@
	orr  r12, r12, r9                           @ overflow |== o1@
    and  r9,  r14, r9, lsr r1                       @ o1 >>= iShift@ o1 &== 0x00ff00ff@

    str  r9,  [r0, #20]                         @  *(U32_WMV *)(pF+20) == o1@
    str  r8,  [r0], #40                         @  *(U32_WMV *)pF == o0@
                                                @  pF +== 40@
    bpl  HFX_InnerLoop

    @    }

    ldr r10, [sp, #HFX_Stack_j]
    ldr r11, [sp, #HFX_Stack_pFUpdate]

    @    pF +== -(40<<(3+b1MV)) + 4@
    @}
    subs r10, r10, #4
    str r10, [sp, #HFX_Stack_j]
    sub  r0, r0, r11
    
	.if PLD_ENABLE == 1
    pld	 [r0]
	.endif
    
    bgt  HFX_Outerloop

    @return overflow@

    add  sp, sp, #HFX_StackSize
    mov r0, r12
    ldmia     sp!, {r4 - r11, pc}

@}
	
    WMV_ENTRY_END	@g_NewHorzFilterX

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@Void_WMV g_InterpolateBlock_00_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, 
@										I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV) // iXFrac ==== 0@ iYFrac ==== 0
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA  |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY g_InterpolateBlock_00_SSIMD
    
@r0 == pSrc
@r1 == iSrcStride
@r2 == pDst
@r7 == i
@r14, r4-r7 == tmp@

    stmdb   sp!, {r4 - r11, r14}
  
    ldr		r3, [ sp, #44]
    tst		r0, #3
    beq		IB00_SRC_ALIGN4
    
	cmp		r3, #0
	adreq	r3, IB00_Loop8
	moveq 	r12, #8
	adrne	r3, IB00_Loop16
	movne	r12, #16
	mov		pc, r3
    
IB00_Loop16:

     @       *(I32_WMV *)(pDst + 12)       == pSrc[12   ]|(pSrc[12+2]<<16)@
     @       *(I32_WMV *)(pDst + 12 + 20)  == pSrc[12+1 ]|(pSrc[12+3]<<16)@

     ldrb r14, [r0, #12]
     ldrb r4,  [r0, #2+12]@
     ldrb r5,  [r0, #1+12]
     ldrb r6,  [r0, #3+12]@
     ldrb r7,  [r0, #8]

     orr  r4, r14, r4, lsl #16
     str  r4,  [r2, #0 +12 ]
     
     orr  r6, r5, r6, lsl #16

     @       *(I32_WMV *)(pDst + 8)       == pSrc[8   ]|(pSrc[8+2]<<16)@
     @       *(I32_WMV *)(pDst + 8 + 20)  == pSrc[8+1 ] |(pSrc[8+3]<<16)@

     ldrb r4,  [r0, #2+8]@
     str  r6,  [r2, #20 +12]
     ldrb r6,  [r0, #3+8]@
     ldrb r5,  [r0, #1+8]

     orr  r4, r7, r4, lsl #16
     str  r4,  [r2, #0 +8 ]
     
     orr  r6, r5, r6, lsl #16
     str  r6,  [r2, #20 +8]

IB00_Loop8:

     @       *(I32_WMV *)(pDst + 4)       == pSrc[4   ]|(pSrc[4+2]<<16)@
     @       *(I32_WMV *)(pDst + 4 + 20)  == pSrc[4+1 ]|(pSrc[4+3]<<16)@
     ldrb r14, [r0, #4]
     ldrb r4,  [r0, #2+4]@
     ldrb r5,  [r0, #1+4]
     ldrb r6,  [r0, #3+4]@
     
     subs  r12,r12, #1

     orr  r4, r14, r4, lsl #16
     str  r4,  [r2, #0 + 4 ]
     
     orr  r6, r5, r6, lsl #16
     str  r6,  [r2, #20 + 4]

     @       *(I32_WMV *)(pDst + 0)      == pSrc[0   ]|(pSrc[0+2]<<16)@
     @       *(I32_WMV *)(pDst + 0 + 20)  == pSrc[0+1 ]|(pSrc[0+3]<<16)@
     
     ldrb r5,  [r0, #1]
     ldrb r6,  [r0, #3]@
     ldrb r4,  [r0, #2]@

	.if PLD_ENABLE == 1
          PLD  [r0, #32]
     .endif

     ldrb r14, [r0], r1
     orr  r6, r5, r6, lsl #16
     str  r6,  [r2, #20]
     
     orr  r4, r14, r4, lsl #16
     str  r4,  [r2], #40

     movgt pc, r3
     
	mov  r0, #0
	ldmia     sp!, {r4 - r11, pc}

IB00_SRC_ALIGN4:		@src address is 4 byte alignment

	.if PLD_ENABLE == 1
	pld		[r0, #32]
	.endif
	cmp		r3, #0
	mov		r14, #0xff
	moveq 	r12, #4
	movne	r12, #16
	orr		r14, r14, r14, lsl #16	@0x00ff00ff
	bne		IB00_Loop16_SRC_ALIGN4
     
IB00_Loop8_SRC_ALIGN4:
     @   for (i == 0@ i < 4@ i++) 
     @   {
     @       *(I32_WMV *)(pDst + 0)      == pSrc[0   ]|(pSrc[0+2]<<16)@
     @       *(I32_WMV *)(pDst + 0 + 20)  == pSrc[0+1 ]|(pSrc[0+3]<<16)@
     @       *(I32_WMV *)(pDst + 4)       == pSrc[4   ]|(pSrc[4+2]<<16)@
     @       *(I32_WMV *)(pDst + 4 + 20)  == pSrc[4+1 ]|(pSrc[4+3]<<16)@
     @       pSrc +== iSrcStride@
     @       pDst +== 40@
     @       *(I32_WMV *)(pDst + 0)      == pSrc[0   ]|(pSrc[0+2]<<16)@
     @       *(I32_WMV *)(pDst + 0 + 20)  == pSrc[0+1 ]|(pSrc[0+3]<<16)@
     @       *(I32_WMV *)(pDst + 4)       == pSrc[4   ]|(pSrc[4+2]<<16)@
     @       *(I32_WMV *)(pDst + 4 + 20)  == pSrc[4+1 ]|(pSrc[4+3]<<16)@
     @       pSrc +== iSrcStride@
     @       pDst +== 40@
     
	ldmia	r0, {r4, r5}
	add		r0, r0, r1
	add		r3, r2, #20     
	.if PLD_ENABLE == 1
	pld		[r0, #32]
	.endif
	ldmia	r0, {r6, r7}
	add		r0, r0, r1    

	and		r8 , r14, r4, lsr #8
	.if PLD_ENABLE == 1
	pld		[r0, #32]
	.endif
	and		r4 , r14, r4
	and		r9 , r14, r5, lsr #8
	and		r5 , r14, r5
	stmia	r2, {r4, r5}    
	add		r2, r2, #40     
	stmia	r3, {r8, r9}    

	and		r8 , r14, r6, lsr #8
	and		r4 , r14, r6
	and		r9 , r14, r7, lsr #8
	and		r5 , r14, r7
	add		r3, r2, #20     
	stmia	r2, {r4, r5}    
	add		r2, r2, #40     
	stmia	r3, {r8, r9}    

	subs	r12, r12, #1
	bne	IB00_Loop8_SRC_ALIGN4
     @	}
     
	mov		r0, #0
	ldmia   sp!, {r4 - r11, pc}

IB00_Loop16_SRC_ALIGN4:

	.if PLD_ENABLE == 1
	pld		[r0, r1]
	pld		[r0, r1, lsl #1]
	.endif
IB00_Loop16_SRC_ALIGN4_Start:
     @   for (i == 0@ i < 16@ i++) 
     @   {

      @      *(I32_WMV *)(pDst + 0)       == pSrc[0   ]|(pSrc[0+2]<<16)@
      @      *(I32_WMV *)(pDst + 0 + 20)  == pSrc[0+1 ]|(pSrc[0+3]<<16)@
      @      *(I32_WMV *)(pDst + 4)       == pSrc[4   ]|(pSrc[4+2]<<16)@
      @      *(I32_WMV *)(pDst + 4 + 20)  == pSrc[4+1 ]|(pSrc[4+3]<<16)@
      @      *(I32_WMV *)(pDst + 8)       == pSrc[8   ]|(pSrc[8+2]<<16)@
      @      *(I32_WMV *)(pDst + 8 + 20)  == pSrc[8+1 ]|(pSrc[8+3]<<16)@
      @      *(I32_WMV *)(pDst + 12)      == pSrc[12  ]|(pSrc[12+2]<<16)@
      @      *(I32_WMV *)(pDst + 12 + 20) == pSrc[12+1]|(pSrc[12+3]<<16)@
     @       pSrc +== iSrcStride@
     @       pDst +== 40@
     
	ldmia	r0, {r4-r7}
	add		r0, r0, r1 
	subs	r12, r12, #1
	.if PLD_ENABLE == 1
	pld		[r0, r1, lsl #1]
	.endif

	and		r8 , r14, r4, lsr #8
	and		r9 , r14, r5, lsr #8
	and		r10, r14, r6, lsr #8
	and		r11, r14, r7, lsr #8
	and		r4 , r14, r4
	and		r5 , r14, r5
	and		r6 , r14, r6
	and		r7 , r14, r7

	stmia	r2, {r4-r7}    
	add		r3, r2, #20     
	add		r2, r2, #40     
	stmia	r3, {r8-r11} 

	bne	IB00_Loop16_SRC_ALIGN4_Start
     @	}
        
    mov  r0, #0
    ldmia     sp!, {r4 - r11, pc}

    WMV_ENTRY_END	@g_InterpolateBlock_00_SSIMD

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@Void_WMV g_NewVertFilter0Long(const U8_WMV  *pSrc,   
@                              I32_WMV iSrcStride, 
@                              U8_WMV * pDst, 
@                              Bool_WMV b1MV)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
@|g_NewVertFilter0Long| PROC
    WMV_LEAF_ENTRY g_NewVertFilter0LongNoGlblTbl

@r0 == pSrc
@r1 == iSrcStride
@r2 == pDst
@r7 == i
@r14, r4-r6 == tmp@

    FRAME_PROFILE_COUNT
    stmdb     sp!, {r4 - r7, r14}
	
	cmp		r3, #0
	adreq	r3, NVFL_Loop8
	moveq	r12, #8
	adrne	r3, NVFL_Loop16
	movne	r12, #16
	mov		pc, r3

NVFL_Loop16:
     @   for(i == 0@ i < 8@ i++)
     @   {

     @*(I32_WMV *)(pDst + 16)       == pSrc[16   ]|(pSrc[16+2]<<16)@
     @*(I32_WMV *)(pDst + 16 + 20)  == pSrc[16+1 ] |(pSrc[16+3]<<16)@
     ldrb r14, [r0, #16]
     ldrb r4,  [r0, #2+16]@
     ldrb r5,  [r0, #1+16]
     ldrb r6,  [r0, #3+16]@
     ldrb r7, [r0, #12]

     orr  r4, r14, r4, lsl #16
     str  r4,  [r2, #0 +16 ]

     orr  r6, r5, r6, lsl #16

     @       *(I32_WMV *)(pDst + 12)       == pSrc[12   ]|(pSrc[12+2]<<16)@
     @       *(I32_WMV *)(pDst + 12 + 20)  == pSrc[12+1 ]|(pSrc[12+3]<<16)@

     ldrb r4,  [r0, #2+12]@
     str  r6,  [r2, #20 +16]
     ldrb r6,  [r0, #3+12]@
     ldrb r5,  [r0, #1+12]

     orr  r4, r7, r4, lsl #16
     str  r4,  [r2, #0 +12 ]
     
     orr  r6, r5, r6, lsl #16
     str  r6,  [r2, #20 +12]

NVFL_Loop8:

     @       *(I32_WMV *)(pDst + 8)       == pSrc[8   ]|(pSrc[8+2]<<16)@
     @       *(I32_WMV *)(pDst + 8 + 20)  == pSrc[8+1 ] |(pSrc[8+3]<<16)@

     ldrb r14, [r0, #8]
     ldrb r4,  [r0, #2+8]@
     ldrb r5,  [r0, #1+8]
     ldrb r6,  [r0, #3+8]@
     
     subs  r12,r12, #1

     orr  r4, r14, r4, lsl #16
     str  r4,  [r2, #0 +8 ]
     
     orr  r6, r5, r6, lsl #16

     @       *(I32_WMV *)(pDst + 4)       == pSrc[4   ]|(pSrc[4+2]<<16)@
     @       *(I32_WMV *)(pDst + 4 + 20)  == pSrc[4+1 ]|(pSrc[4+3]<<16)@
     ldrb r14, [r0, #4]
     ldrb r4,  [r0, #2+4]@
     str  r6,  [r2, #20 +8]
     ldrb r6,  [r0, #3+4]@
     ldrb r5,  [r0, #1+4]
     
     orr  r4, r14, r4, lsl #16
     str  r4,  [r2, #0 + 4 ]
     
     orr  r6, r5, r6, lsl #16
     str  r6,  [r2, #20 + 4]

     @       *(I32_WMV *)(pDst + 0)      == pSrc[0   ]|(pSrc[0+2]<<16)@
     @       *(I32_WMV *)(pDst + 0 + 20)  == pSrc[0+1 ]|(pSrc[0+3]<<16)@
     
     ldrb r5,  [r0, #1]
     ldrb r6,  [r0, #3]@
     ldrb r4,  [r0, #2]@

	.if PLD_ENABLE == 1
          PLD  [r0, #32]
     .endif

     ldrb r14, [r0], r1
     orr  r6, r5, r6, lsl #16
     str  r6,  [r2, #20]
     
     orr  r4, r14, r4, lsl #16
     str  r4,  [r2], #40

     movgt pc, r3
     
    mov  r0, #0
    ldmia     sp!, {r4 - r7, pc}
    
    WMV_ENTRY_END	@g_NewVertFilter0LongNoGlblTbl


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@Void_WMV g_AddNull_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV iPitch)
@{
@    I32_WMV iy@
@    U32_WMV u0,u1,u2,u3, y0,y1@  
@    for (iy == 0@ iy < 8@ iy++) 
@    {     
@        u0 == pRef[0]@
@        u1 == pRef[0 + 5]@
@        u2 == pRef[1]@
@        u3 == pRef[1 + 5]@
@
@        pRef +== 10@
@        
@        y0 == (u0) | ((u1) << 8)@
@        y1 == (u2) | ((u3) << 8)@
@        
@        *(U32_WMV *)ppxlcDst == y0@
@        *(U32_WMV *)(ppxlcDst + 4)== y1@
@        ppxlcDst +== iPitch@
@    }
@}

    @AREA  |.embsec_PMainLoopLvl1|, CODE
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    WMV_LEAF_ENTRY g_AddNull_SSIMD
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
@r0 == ppxlcDst
@r1 == pRef
@r2 == iPitch
@r3 == iy
@r4 == u0
@r5 == u1
@r12 == u2
@r14 == u3
    stmdb     sp!, {r4 - r5, r14}
    FRAME_PROFILE_COUNT

    ldr  r3,[r1, #4]

    ldr  r14,[r1, #24]
    ldr  r5, [r1, #20]
    ldr  r4, [r1], #40
    orr r12,r3,r14, lsl #8
	ldr  r3,[r1, #4]
    orr r4, r4, r5,  lsl #8
    str r12,[r0, #4]
    str r4, [r0], r2
 
    ldr  r14,[r1, #24]
    ldr  r5, [r1, #20]
    ldr  r4, [r1], #40
    orr r12,r3,r14, lsl #8
	ldr  r3,[r1, #4]
    orr r4, r4, r5,  lsl #8
    str r12,[r0, #4]
    str r4, [r0], r2

    ldr  r14,[r1, #24]
    ldr  r5, [r1, #20]
    ldr  r4, [r1], #40
    orr r12,r3,r14, lsl #8
	ldr  r3,[r1, #4]
    orr r4, r4, r5,  lsl #8
    str r12,[r0, #4]
    str r4, [r0], r2
 
    ldr  r14,[r1, #24]
    ldr  r5, [r1, #20]
    ldr  r4, [r1], #40
    orr r12,r3,r14, lsl #8
	ldr  r3,[r1, #4]
    orr r4, r4, r5,  lsl #8
    str r12,[r0, #4]
    str r4, [r0], r2        
        
    ldr  r14,[r1, #24]
    ldr  r5, [r1, #20]
    ldr  r4, [r1], #40
    orr r12,r3,r14, lsl #8
	ldr  r3,[r1, #4]
    orr r4, r4, r5,  lsl #8
    str r12,[r0, #4]
    str r4, [r0], r2
 
    ldr  r14,[r1, #24]
    ldr  r5, [r1, #20]
    ldr  r4, [r1], #40
    orr r12,r3,r14, lsl #8
	ldr  r3,[r1, #4]
    orr r4, r4, r5,  lsl #8
    str r12,[r0, #4]
    str r4, [r0], r2

    ldr  r14,[r1, #24]
    ldr  r5, [r1, #20]
    ldr  r4, [r1], #40
    orr r12,r3,r14, lsl #8
	ldr  r3,[r1, #4]
    orr r4, r4, r5,  lsl #8
    str r12,[r0, #4]
    str r4, [r0], r2
 
    ldr  r14,[r1, #24]
    ldr  r5, [r1, #20]
    ldr  r4, [r1], #40
    orr r12,r3,r14, lsl #8
	ldr  r3,[r1, #4]
    orr r4, r4, r5,  lsl #8
    str r12,[r0, #4]
    str r4, [r0], r2        

    ldmia     sp!, {r4 - r5, pc}

@}

    WMV_ENTRY_END  @  g_AddNull_SSIMD


    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@Void_WMV g_AddNullB_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef0 , U32_WMV* pRef1, I32_WMV iOffset, I32_WMV iPitch)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA  |.embsec_BMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY g_AddNullB_SSIMD
@r0 == ppxlcDst
@r1 == pRef0
@r2 == pRef1
@r3 == iy
@r4 == u0
@r5 == u1
@r12 == u2
@r14 == u3
@r6 == v0
@r7 == v1
@r8 == v2
@r9 == v3
@r10 == 0x00010001
@r11 == iPitch

.equ ANBE_OffsetRegSaving   ,0x24
.equ ANBE_Offset_iPitch          ,ANBE_OffsetRegSaving + 0

    @I32_WMV iy@
    @U32_WMV u0,u1,u2,u3, y0,y1@

    stmdb     sp!, {r4 - r11, r14}
    FRAME_PROFILE_COUNT

    ldr r11, [sp, #ANBE_Offset_iPitch]

    @pRef0 +== iOffset@
    @pRef1 +== iOffset@

    mov r10, #1
    add r1, r1, r3, lsl #2
    orr   r10, r10, r10, lsl #16
    add r2, r2, r3, lsl #2
    mov r3, #8

ANBE_Loop:
   @for (iy == 0@ iy < BLOCK_SIZE@ iy++) 
    

        @u0 == pRef[0]@
        @u1 == pRef[0 + 5]@
        @u2 == pRef[1]@
        @u3 == pRef[1 + 5]@
        @pRef0 +== 10@

        @v0 == pRef1[0]@
        @v1 == pRef1[0 + 5]@
        @v2 == pRef1[1]@
        @v3 == pRef1[1 + 5]@
        @pRef1 +== 10@

        ldr  r12,[r1, #4]
        ldr  r8,  [r2,  #4]
        ldr  r14,[r1, #24]
        ldr  r9,  [r2,   #24]

        subs r3, r3, #1

        ldr  r5, [r1, #20]
        ldr  r7,[r2,  #20]
        ldr  r4, [r1], #40
        ldr  r6, [r2], #40

        @u2 == (u2 + v2 + 0x00010001) >>1@
        @u3 == (u3 + v3 + 0x00010001) >>1@
        @u2 == u2 & ~0x8000@
        @u3 == u3 & ~0x8000@

        add r12, r12, r8
        add r12, r12, r10
        bic  r12, r12, #0x10000
        add r14, r14, r9
        mov r12, r12, lsr #1

        add r14, r14, r10
        bic  r14, r14, #0x10000
        add r5, r5, r7
        mov r14, r14, lsr #1

        add r5, r5, r10
        bic  r5, r5, #0x10000
        add r4, r4, r6
        mov r5, r5, lsr #1

        add r4, r4, r10
        bic  r4, r4, #0x10000
        orr r12,r12,r14, lsl #8
        mov r4, r4, lsr #1
        
        @u2 == (u2) | ((u3) << 8)@
        @u0 == (u0) | ((u1) << 8)@
        
        orr r4, r4, r5,  lsl #8

        @*(U32_WMV *)(ppxlcDst + 4)== u2@
        @*(U32_WMV *)ppxlcDst == u0@
        @ppxlcDst +== iPitch@

        str r12,[r0, #4]
        str r4, [r0], r11
        
        bgt ANBE_Loop
        
    ldmia     sp!, {r4 - r11, pc}
    
    
@}
    WMV_ENTRY_END


    .endif @ WMV_OPT_MOTIONCOMP_ARM== 1

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@



@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .if WMV_OPT_INTENSITYCOMP_ARM==1
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

pSrc        .req  r0
iFrameSize  .req  r1
pLUT        .req  r2
dst0        .req  r3
dst1        .req  r4
dst2        .req  r5
dst3        .req  r6
temp0       .req  r7
temp1       .req  r8
temp2       .req  r9
temp3       .req  r10

    @AREA  |.text|, CODE
    WMV_LEAF_ENTRY IntensityComp

	stmdb     sp!, {r4 - r11, lr}

FadingLoop:    
        
    ldmia   pSrc, {dst0 - dst3}
	ldrb    temp0, [pLUT, +dst0, lsr #24]
    ldrb    temp1, [pLUT, +dst1, lsr #24]
    ldrb    temp2, [pLUT, +dst2, lsr #24]
    ldrb    temp3, [pLUT, +dst3, lsr #24]
    add     dst0, temp0, dst0, lsl #8
    add     dst1, temp1, dst1, lsl #8
    add     dst2, temp2, dst2, lsl #8
    add     dst3, temp3, dst3, lsl #8
	ldrb    temp0, [pLUT, +dst0, lsr #24]
    ldrb    temp1, [pLUT, +dst1, lsr #24]
    ldrb    temp2, [pLUT, +dst2, lsr #24]
    ldrb    temp3, [pLUT, +dst3, lsr #24]
    add     dst0, temp0, dst0, lsl #8
    add     dst1, temp1, dst1, lsl #8
	.if PLD_ENABLE == 1
        pld     [pSrc, #32]
    .endif
    add     dst2, temp2, dst2, lsl #8
    add     dst3, temp3, dst3, lsl #8
	ldrb    temp0, [pLUT, +dst0, lsr #24]
    ldrb    temp1, [pLUT, +dst1, lsr #24]
    ldrb    temp2, [pLUT, +dst2, lsr #24]
    ldrb    temp3, [pLUT, +dst3, lsr #24]
    add     dst0, temp0, dst0, lsl #8
    add     dst1, temp1, dst1, lsl #8
    add     dst2, temp2, dst2, lsl #8
    add     dst3, temp3, dst3, lsl #8
	ldrb    temp0, [pLUT, +dst0, lsr #24]
    ldrb    temp1, [pLUT, +dst1, lsr #24]
    ldrb    temp2, [pLUT, +dst2, lsr #24]
    ldrb    temp3, [pLUT, +dst3, lsr #24]
    add     dst0, temp0, dst0, lsl #8
    add     dst1, temp1, dst1, lsl #8
    add     dst2, temp2, dst2, lsl #8
    add     dst3, temp3, dst3, lsl #8
    stmia   pSrc!, {dst0 - dst3}

    subs    iFrameSize, iFrameSize, #1
    bne     FadingLoop

	ldmia   sp!, {r4 - r11, pc}
    WMV_ENTRY_END

    .endif @WMV_OPT_INTENSITYCOMP_ARM

    .END 
