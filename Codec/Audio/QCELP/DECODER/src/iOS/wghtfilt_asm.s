@;**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@* File Name: 
@*            wghtfilt_asm.s
@* Description: 
@*            This module implements the wghtfilt().
@* Functions Included:
@*            1. void wghtfilt
@*
@***************************** Change History**************************
@* 
@*    DD/MMM/YYYY     Code Ver     Description             Author
@*    -----------     --------     -----------             ------
@*    03-24-2012        1.0        File imported from      Huaping Liu
@*                                             
@**********************************************************************
@**********************************************************************
@void  wghtfilt(Int16 *data,
@               Int16 * pcoeffs,
@               Int16 * zcoeffs,
@               Int16 * hist, 
@               Int16 order, 
@               Int16 len)
@**********************
@ ARM registers
@**********************
@ *data    --- R0
@ *pcoeffs --- R1
@ *zcoeffs --- R2
@ *hist    --- R3
@ order    --- R4
@ len      --- R5
#include "voQcelpDecID.h"

        .text
        .align 4
        .globl  _wghtfilt_asm  

_wghtfilt_asm: @FUNCTION

        stmfd       sp!, {r4-r12,lr}
        mov         r6, #0x0                 @extra_shift = 0
        mov         r7, #0x0                 @i = 0 
        ldr         r4, [sp, #44]            @get len


LOOP:
        ldrsh       r8, [r0]                 @data[i]
        rsb         r10, r6, #9              @9 - extra_shift
        ldr         r5,  [r1, #16]           @load pcoeffs[9],pcoeffs[8]
        ldrsh       r9,  [r3, #18]           @load hist[9]
        mov         r8, r8, LSL r10          @arA = (data[i])<<(9 - extra_shift)

        ldr         r10, [r2, #16]           @load zcoeffs[9],zcoeffs[8]
        ldrsh       r11, [r3, #16]           @load hist[8]
        smlabt      r12, r9, r5, r8          @arA += pcoeffs[9] * hist[9]
        smulbt      r14, r9, r10             @arB = hist[9] * zcoeffs[9]
        
        smlabb      r12, r11, r5, r12        @arA += pcoeffs[8] * hist[8]
        smlabb      r14, r11, r10, r14       @arB += hist[8] * zcoeffs[8]
        strh        r11, [r3, #18]           @hist[9] = hist[8]

        ldr         r5,  [r1, #12]           @load pcoeffs[7], pcoeffs[6]
        ldrsh       r9,  [r3, #14]           @load hist[7]
        ldr         r10, [r2, #12]           @load zcoeffs[7], zcoeffs[6]
        ldrsh       r11, [r3, #12]           @load hist[6]

        smlabt      r12, r9, r5, r12         @arA += pcoeffs[7] * hist[7]
        smlabt      r14, r9, r10, r14        @arB += hist[7] * zcoeffs[7]

        smlabb      r12, r11, r5, r12        @arA += pcoeffs[6] * hist[6]
        smlabb      r14, r11, r10, r14       @arB += hist[6] * zcoeffs[6]
        strh        r9, [r3, #16]            @hist[8] = hist[7]
        strh        r11,[r3, #14]            @hist[7] = hist[6]

        ldr         r5,  [r1, #8]            @load pcoeffs[5], pcoeffs[4]
        ldrsh       r9,  [r3, #10]           @load hist[5]
        ldr         r10, [r2, #8]            @load zcoeffs[5], zcoeffs[4]
        ldrsh       r11, [r3, #8]            @load hist[4]

        smlabt      r12, r9, r5, r12         @arA += pcoeffs[5] * hist[5]
        smlabt      r14, r9, r10, r14        @arB += hist[5] * zcoeffs[5]
  
        smlabb      r12, r11, r5, r12        @arA += pcoeffs[4] * hist[4]
        smlabb      r14, r11, r10, r14       @arB += hist[4] * zcoeffs[4]
        strh        r9, [r3, #12]            @hist[6] = hist[5]
        strh        r11,[r3, #10]            @hist[5] = hist[4]

        ldr         r5,  [r1, #4]            @load pcoeffs[3], pcoeffs[2]
        ldrsh       r9,  [r3, #6]            @load hist[3]
        ldr         r10, [r2, #4]            @load zcoeffs[3], zcoeffs[2]
        ldrsh       r11, [r3, #4]            @load hist[2]

        smlabt      r12, r9, r5, r12         @arA += pcoeffs[3] * hist[3]
        smlabt      r14, r9, r10, r14        @arB += hist[3] * zcoeffs[3]

        smlabb      r12, r11, r5, r12        @arA += pcoeffs[2] * hist[2]
        smlabb      r14, r11, r10, r14       @arB += hist[2] * zcoeffs[2]
        strh        r9,  [r3, #8]            @hist[4] = hist[3]
        strh        r11, [r3, #6]            @hist[3] = hist[2]

        ldr         r5, [r1]                 @load pcoeffs[1], pcoeffs[0] 
        ldrsh       r9, [r3, #2]             @load hist[1]
        ldr         r10, [r2]                @load zcoeffs[1], zcoeffs[0]
        ldrsh       r11, [r3]                @load hist[0]

        smlabt      r12, r9, r5, r12         @arA += pcoeffs[1] * hist[1]
        smlabt      r14, r9, r10, r14        @arB += hist[1] * zcoeffs[1]
  
        smlabb      r12, r11, r5, r12        @arA += pcoeffs[0] * hist[0]
        smlabb      r14, r11, r10, r14       @arB += hist[0] * zcoeffs[0]
        strh        r9, [r3, #4]             @hist[2] = hist[1]
        strh        r11,[r3, #2]             @hist[1] = hist[0]

        @r12 --- arA  r14 --- arB
        mov         r8, #0x7FFFFFFF
        rsb         r9, r14, r12             @arA - arB
        add         r10, r6, #7              @7 + extra_shift

        mov         r5, r12, LSL #4
        teq         r12, r5, ASR #4
        eorne       r5,  r8, r12, ASR #31

        @SSAT        r5, #32, r12, LSL #4    ;arA = L_shl2(arA, 4)
        mov         r11, r9, LSL r10
        teq         r9,  r11, ASR r10
        eorne       r11, r8, r9, ASR #31

        mov         r10, #0x8000
        qadd        r12, r5, r10            @arA += 0x00008000L
        qadd        r9,  r11, r10             @arB += 0x00008000L
        mov         r11, r12, ASR #16        
        strh        r11, [r3]                @hist[0] = (int16)(arA >> 16)
        cmp         r12, #0
        rsblt       r12, r12, #0             @arA = (arA >=0) ? arA : -arA
        mov         r11, r9, ASR #16
        ldr         r5, =0x70000000
        strh        r11, [r0], #2            @data[i] = (Int16)(arB >> 16)

        cmp         r12, r5
        blt         Lable
        add         r6, r6, #2
        cmp         r6, #6
        movgt       r6, #6

Lable:      
        add         r7, r7, #1
        cmp         r7, r4
        blt         LOOP 

        ldmfd       sp!, {r4 - r12,pc} 
        @ENDFUNC
        @END

