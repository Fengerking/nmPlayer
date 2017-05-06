@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@* File Name: 
@*            ConIR.s
@* Description: 
@*            This module implements the ConvolveImpulseR().
@* Functions Included:
@*            1. void ConvolveImpulseR
@*
@***************************** Change History**************************
@* 
@*    DD/MMM/YYYY     Code Ver     Description             Author
@*    -----------     --------     -----------             ------
@*    04-21-2009        1.0        File imported from      Huaping Liu
@*                                             
@**********************************************************************
@void ConvolveImpulseR(
@		short *out,
@		short *in,
@		short *H,
@		short hlength,
@		short length)
@********************
@ ARM Register
@********************
@r0   ---  *out
@r1   ---  *in
@r2   ---  *H
@r3   ---  length
#include "voEvrcDecID.h"
        .text
        .align 4
	.globl   _ConIR_asm

_ConIR_asm:
        stmfd       sp!, {r4-r12,lr}
        mov         r5, #0                          @i=0
        mov         r4, #0x8000                       

LOOP:
        add         r7, r1, r5, LSL #1              @sp1 = &in[i]
        mov         r8, r2                          @sp2 = H
        mov         r6, #0                          @ltemp = 0
        mov         r9, #0                          @j = 0
LOOP1:
        ldrsh       r10, [r7], #-2                  @*(sp1--)
        ldrsh       r11, [r8], #2                   @*(sp2++)
        add         r9, r9, #1
        cmp         r9, r5
        mla         r6, r10, r11, r6                @ltemp += *(sp1--) * *(sp2++)
        bls         LOOP1
        mov         r12, r6, LSL #5                 @ltemp <<= 5
        add         r10, r12, r4
        add         r5, r5, #1                      @i++
        mov         r11, r10, ASR #16
        cmp         r5, r3
        strh        r11, [r0], #2
        blt         LOOP
        

        ldmfd       sp!, {r4 - r12,pc} 
        
        @.END

