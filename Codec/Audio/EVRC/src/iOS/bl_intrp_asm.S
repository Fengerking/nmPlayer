@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@* File Name: 
@*            bl_intrp.s
@* Description: 
@*            This module implements the bl_intrp().
@* Functions Included:
@*            1. void bl_intrp
@*
@***************************** Change History**************************
@* 
@*    DD/MMM/YYYY     Code Ver     Description             Author
@*    -----------     --------     -----------             ------
@*    04-14-2009        1.0        File imported from      Huaping Liu
@*                                             
@**********************************************************************
@void bl_intrp(
@	        short *output,   	/* int ptr */
@	        short *input,	        /* int ptr */
@		long delay,	        /* sign bit + 7bits int  + 24bits frac */
@		short factor,	        /* fractional value */
@		short fl		/* int value */
@)
@********************
@ ARM Register
@********************
@r0   ---  *output
@r1   ---  *input
@r2   ---  delay
@r3   ---  factor
@r4   ---  f1
#include "voEvrcDecID.h"
        .text
        .align 4
	.globl  _bl_intrp_asm
	.globl  _Table1
	.globl  _Table

_bl_intrp_asm:

        stmfd       sp!, {r4-r12,lr}
        ldr         r4, [sp, #40]               @get f1
        mov         r7, r2, asr #23             @delay >> 23
        mov         r6, #0x4000                 @loutput = 0x4000
        add         r8, r7, #0x01               @(delay>>23) + 0x01
        ldr         r5, Tab2                    @factor2 = 29491
        mov         r9, r8, asr #1              @offset
        mov         r7, r9, lsl #24             @offset << 24
        sub         r8, r7, r2                  @(offset << 24) - delay
        mov         r7, r8, asr #15             @((offset << 24) - delay)>>15
        add         r8, r7, #288                
        mov         r7, r8, asr #6              @t = ((((offset << 24) - delay) >> 15) + 288) >> 6

        cmp         r7, #8
        moveq       r7, #0                      @t = 0
        subeq       r9, r9, #1                  @offset--
        add         r8, r9, r4                  @offset + f1
        add         r10, r4, r4
        add         r10, r10, #1
        mul         r11, r7, r10                @t = (t * ((f1 << 1) + 1))
        sub         r10, r1, r8, lsl #1         @f = input - offset - f1
        ldr         r7, Tab1                    @get Table1 address
        ldr         r12, Tab                    @get Table address
        cmp         r3, r5
        addeq       r12, r7, r11, lsl #1        @coef_ptr = Table1 + t
        addne       r12, r12, r11, lsl #1       @coef_ptr = Table + t
        ldrsh       r5, [r12], #2               
        ldrsh       r7, [r10], #2
        mla         r6, r5, r7, r6

LOOP:        
        ldrsh       r8, [r12], #2
        ldrsh       r9, [r10], #2

        ldrsh       r5, [r12], #2
        ldrsh       r7, [r10], #2
        mla         r6, r8, r9, r6
        mla         r6, r5, r7, r6
        subs        r4, r4, #1
        bgt         LOOP
        mov         r8, r6, asr #15
        strh        r8, [r0]
        
        ldmfd       sp!, {r4 - r12,pc} 

Tab1:
        .word       _Table1
Tab:
        .word       _Table
Tab2:
        .word       0x7333
 
        @.END

