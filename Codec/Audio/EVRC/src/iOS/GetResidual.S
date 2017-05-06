@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@* File Name: 
@*            GetResidual.s
@* Description: 
@*            This module implements the GetResidual().
@* Functions Included:
@*            1. void GetResidual
@*
@***************************** Change History**************************
@* 
@*    DD/MMM/YYYY     Code Ver     Description             Author
@*    -----------     --------     -----------             ------
@*    04-28-2009        1.0        File imported from      Huaping Liu
@*                                             
@**********************************************************************
@void GetResidual(
@		short *res,
@		short *input,
@		short *coef,
@		short *FIRmemory,
@		short order,
@		short length)
@********************
@ ARM Register
@********************
@r0   ---  *res
@r1   ---  *input
@r2   ---  *coef
@r3   ---  *FIRmemory
@r4   ---  order
@r5   ---  length
#include "voEvrcDecID.h"
        .text
        .align 4
	.globl    _GetResidual_asm

_GetResidual_asm:

        stmfd       sp!, {r4-r12,lr}
        ldr         r5, [sp, #44]               @get length
        ldr         r4, [r2]                    @coef[0], coef[1]
        ldr         r6, [r2, #4]                @coef[2], coef[3]
        ldr         r7, [r2, #8]                @coef[4], coef[5]
        ldr         r8, [r2, #12]               @coef[6], coef[7]
        ldr         r9, [r2, #16]               @coef[8], coef[9]
        mov         r2, #0                      @i = 0

LOOP:
        ldrsh       r10, [r3, #18]              @FIRmemory[9]
        ldrsh       r11, [r3, #16]              @FIRmemory[8]
        smulbt      r14, r10, r9                @LSUM = coef[9] * FIRmemory[9]
        strh        r11, [r3, #18]              @FIRmemory[9] = FIRmemory[8]
        ldrsh       r10, [r3, #14]              @FIRmemory[7]
        smlabb      r14, r11, r9, r14           @LSUM += coef[8] * FIRmemory[8]
        ldrsh       r11, [r3, #12]              @FIRmemory[6]
        strh        r10, [r3, #16]              @FIRmemory[8] = FIRmemory[7]
        smlabt      r14, r10, r8, r14           @LSUM += coef[7] * FIRmemory[7]
        strh        r11, [r3, #14]              @FIRmemory[7] = FIRmemory[6]
        ldrsh       r10, [r3, #10]              @FIRmemory[5]
        smlabb      r14, r11, r8, r14           @LSUM += coef[6] * FIRmemory[6]
        ldrsh       r11, [r3, #8]               @FIRmemory[4]
        strh        r10, [r3, #12]              @FIRmemory[6] = FIRmemory[5]
        smlabt      r14, r10, r7, r14           @LSUM += coef[5] * FIRmemory[5]
        ldrsh       r10, [r3, #6]               @FIRmemory[3]
        strh        r11, [r3, #10]              @FIRmemory[5] = FIRmemory[4]
        smlabb      r14, r11, r7, r14           @LSUM += coef[4] * FIRmemory[4]
        ldrsh       r11, [r3, #4]               @FIRmemory[2]
        strh        r10, [r3, #8]               @FIRmemory[4] = FIRmemory[3]
        smlabt      r14, r10, r6, r14           @LSUM += coef[3] * FIRmemory[3]
        ldrsh       r10, [r3, #2]               @FIRmemory[1]
        strh        r11, [r3, #6]               @FIRmemory[3] = FIRmemory[2]
        smlabb      r14, r11, r6, r14           @LSUM += coef[2] * FIRmemory[2]
        ldrsh       r11, [r3]                   @FIRmemory[0]
        strh        r10, [r3, #4]               @FIRmemory[2] = FIRmemory[1]
        smlabt      r14, r10, r4, r14           @LSUM += coef[1] * FIRmemory[1]
        ldrsh       r12, [r1], #2               @input[i]
        strh        r11, [r3, #2]               @FIRmemory[1] = FIRmemory[0]
        smlabb      r14, r11, r4, r14           @LSUM += coef[0] * FIRmemory[0]
        mov         r11, #0x800
        mov         r10, r12, LSL #12           @input[i] << 12
        strh        r12, [r3]                   @FIRmemory[0] = input[i]
        add         r14, r14, r10               @LSUM += (input[i] << 12)
        add         r14, r14, r11
        add         r2, r2, #1
        mov         r10, r14, ASR #12
        strh        r10, [r0], #2               @res[i] = (LSUM + 0x800) >> 12
        cmp         r2,  r5
        blt         LOOP    
        

        ldmfd       sp!, {r4 - r12,pc} 
        @.END

