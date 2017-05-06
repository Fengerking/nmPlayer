@**************************************************************
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
@*    03-24-2009        1.0        File imported from      Huaping Liu
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

        .section .text
	.global  wghtfilt_asm

wghtfilt_asm:

        STMFD       sp!, {r4-r12,lr}
        MOV         r6, #0x0                 @extra_shift = 0
        MOV         r7, #0x0                 @i = 0 
        LDR         r4, [sp, #44]            @get len


LOOP:
        LDRSH       r8, [r0]                 @data[i]
        RSB         r10, r6, #9              @9 - extra_shift
        LDR         r5,  [r1, #16]           @load pcoeffs[9],pcoeffs[8]
        LDRSH       r9,  [r3, #18]           @load hist[9]
        MOV         r8, r8, LSL r10          @arA = (data[i])<<(9 - extra_shift)

        LDR         r10, [r2, #16]           @load zcoeffs[9],zcoeffs[8]
        LDRSH       r11, [r3, #16]           @load hist[8]
        SMLABT      r12, r9, r5, r8          @arA += pcoeffs[9] * hist[9]
        SMULBT      r14, r9, r10             @arB = hist[9] * zcoeffs[9]
        
        SMLABB      r12, r11, r5, r12        @arA += pcoeffs[8] * hist[8]
        SMLABB      r14, r11, r10, r14       @arB += hist[8] * zcoeffs[8]
        STRH        r11, [r3, #18]           @hist[9] = hist[8]

        LDR         r5,  [r1, #12]           @load pcoeffs[7], pcoeffs[6]
        LDRSH       r9,  [r3, #14]           @load hist[7]
        LDR         r10, [r2, #12]           @load zcoeffs[7], zcoeffs[6]
        LDRSH       r11, [r3, #12]           @load hist[6]

        SMLABT      r12, r9, r5, r12         @arA += pcoeffs[7] * hist[7]
        SMLABT      r14, r9, r10, r14        @arB += hist[7] * zcoeffs[7]

        SMLABB      r12, r11, r5, r12        @arA += pcoeffs[6] * hist[6]
        SMLABB      r14, r11, r10, r14       @arB += hist[6] * zcoeffs[6]
        STRH        r9, [r3, #16]            @hist[8] = hist[7]
        STRH        r11,[r3, #14]            @hist[7] = hist[6]

        LDR         r5,  [r1, #8]            @load pcoeffs[5], pcoeffs[4]
        LDRSH       r9,  [r3, #10]           @load hist[5]
        LDR         r10, [r2, #8]            @load zcoeffs[5], zcoeffs[4]
        LDRSH       r11, [r3, #8]            @load hist[4]

        SMLABT      r12, r9, r5, r12         @arA += pcoeffs[5] * hist[5]
        SMLABT      r14, r9, r10, r14        @arB += hist[5] * zcoeffs[5]
  
        SMLABB      r12, r11, r5, r12        @arA += pcoeffs[4] * hist[4]
        SMLABB      r14, r11, r10, r14       @arB += hist[4] * zcoeffs[4]
        STRH        r9, [r3, #12]            @hist[6] = hist[5]
        STRH        r11,[r3, #10]            @hist[5] = hist[4]

        LDR         r5,  [r1, #4]            @load pcoeffs[3], pcoeffs[2]
        LDRSH       r9,  [r3, #6]            @load hist[3]
        LDR         r10, [r2, #4]            @load zcoeffs[3], zcoeffs[2]
        LDRSH       r11, [r3, #4]            @load hist[2]

        SMLABT      r12, r9, r5, r12         @arA += pcoeffs[3] * hist[3]
        SMLABT      r14, r9, r10, r14        @arB += hist[3] * zcoeffs[3]

        SMLABB      r12, r11, r5, r12        @arA += pcoeffs[2] * hist[2]
        SMLABB      r14, r11, r10, r14       @arB += hist[2] * zcoeffs[2]
        STRH        r9,  [r3, #8]            @hist[4] = hist[3]
        STRH        r11, [r3, #6]            @hist[3] = hist[2]

        LDR         r5, [r1]                 @load pcoeffs[1], pcoeffs[0] 
        LDRSH       r9, [r3, #2]             @load hist[1]
        LDR         r10, [r2]                @load zcoeffs[1], zcoeffs[0]
        LDRSH       r11, [r3]                @load hist[0]

        SMLABT      r12, r9, r5, r12         @arA += pcoeffs[1] * hist[1]
        SMLABT      r14, r9, r10, r14        @arB += hist[1] * zcoeffs[1]
  
        SMLABB      r12, r11, r5, r12        @arA += pcoeffs[0] * hist[0]
        SMLABB      r14, r11, r10, r14       @arB += hist[0] * zcoeffs[0]
        STRH        r9, [r3, #4]             @hist[2] = hist[1]
        STRH        r11,[r3, #2]             @hist[1] = hist[0]

        @r12 --- arA  r14 --- arB
        MOV         r8, #0x7FFFFFFF
        RSB         r9, r14, r12             @arA - arB
        ADD         r10, r6, #7              @7 + extra_shift

        MOV         r5, r12, LSL #4
        TEQ         r12, r5, ASR #4
        EORNE       r5,  r8, r12, ASR #31

        @SSAT        r5, #32, r12, LSL #4    @arA = L_shl2(arA, 4)
        MOV         r11, r9, LSL r10
        TEQ         r9,  r11, ASR r10
        EORNE       r11, r8, r9, ASR #31

        MOV         r10, #0x8000
        QADD        r12, r5, r10             @arA += 0x00008000L
        QADD        r9,  r11, r10            @arB += 0x00008000L
        MOV         r11, r12, ASR #16        
        STRH        r11, [r3]                @hist[0] = (int16)(arA >> 16)
        CMP         r12, #0
        RSBLT       r12, r12, #0             @arA = (arA >=0) ? arA : -arA
        MOV         r11, r9, ASR #16
        LDR         r5, =0x70000000
        STRH        r11, [r0], #2            @data[i] = (Int16)(arB >> 16)

        CMP         r12, r5
        BLT         Lable
        ADD         r6, r6, #2
        CMP         r6, #6
        MOVGT       r6, #6

Lable:       
        ADD         r7, r7, #1
        CMP         r7, r4
        BLT         LOOP 

        LDMFD       sp!, {r4 - r12,pc} 
        .end

