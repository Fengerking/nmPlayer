@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@* File Name: 
@*            Impulse_asm.s
@* Description: 
@*            This module implements the ImpulseRzp().
@* Functions Included:
@*            1. void ImpulseRzp
@*
@***************************** Change History**************************
@* 
@*    DD/MMM/YYYY     Code Ver     Description             Author
@*    -----------     --------     -----------             ------
@*    04-21-2009        1.0        File imported from      Huaping Liu
@*                                             
@**********************************************************************
@void ImpulseRzp1(
@		short *output,
@		short *coef_uq,
@		short *coef,
@		short length)
@********************
@ ARM Register
@********************
@r0   ---  *output
@r1   ---  *coef_uq
@r2   ---  *coef
@r3   ---  length
#include "voEvrcDecID.h"
        .text
        .align 4
	.globl   _ImpulseRzp1_asm
	.globl   _GAMMA11

_ImpulseRzp1_asm:
        stmfd       sp!, {r4-r12,lr}
        sub         sp, sp, #60 
        add         r4, sp, #40                     @r4 --- &memory[0]
        str         r0, [sp, #20]                    @store r0

        mov         r6, #0x0800
        mov         r7, #0x0    
        strh        r6, [r0], #2                     @output[0] = 0x0800 
        strh        r6, [r4]                         @memory[0] = 0x0800
        strh        r7, [r4, #2]                     @memory[1] = 0
        str         r7, [r4, #4]                     @memory[2],memory[3] = 0
        str         r7, [r4, #8]                     @memory[4],memory[5] = 0
        str         r7, [r4, #12]                    @memory[6],memory[7] = 0
        str         r7, [r4, #16]                    @memory[8],memory[9] = 0

        mov         r12, #0x1                        @i = 1
        ldr         r6,  [r2]                        @coef[0], coef[1]
        ldr         r7,  [r2, #4]                    @coef[2], coef[3]
        ldr         r8,  [r2, #8]                    @coef[4], coef[5]
        ldr         r9,  [r2, #12]                   @coef[6], coef[7]
        ldr         r10, [r2, #16]                   @coef[8], coef[9] 

LOOP:
        ldrsh       r11, [r4, #18]                   @memory[9]
        ldrsh       r5,  [r4, #16]                   @memory[8]
        smulbt      r14, r11, r10                    @LSUM = memory[9] * coef[9]
        ldrsh       r11, [r4, #14]                   @memory[7]
        strh        r5,  [r4, #18]                   @memory[9] = memory[8]
        smlabb      r14, r5, r10, r14                @LSUM += memory[8] * coef[8]
        ldrsh       r5,  [r4, #12]                   @memory[6]
        strh        r11, [r4, #16]                   @memory[8] = memory[7]
        smlabt      r14, r11, r9, r14                @LSUM += memory[7] * coef[7]
        ldrsh       r11, [r4, #10]                   @memory[5]
        strh        r5,  [r4, #14]                   @memory[7] = memory[6]
        smlabb      r14, r5, r9, r14                 @LSUM += memory[6] * coef[6]
        ldrsh       r5,  [r4, #8]                    @memory[4]
        strh        r11, [r4, #12]                   @memory[6] = memory[5]
        smlabt      r14, r11, r8, r14                @LSUM += memory[5] * coef[5]
        ldrsh       r11, [r4, #6]                    @memory[3]
        strh        r5,  [r4, #10]                   @memory[5] = memory[4]
        smlabb      r14, r5, r8, r14                 @LSUM += memory[4] * coef[4]
        ldrsh       r5,  [r4, #4]                    @memory[2]
        strh        r11, [r4, #8]                    @memory[4] = memoey[3]
        smlabt      r14, r11, r7, r14                @LSUM += memory[3] * coef[3]
        ldrsh       r11, [r4, #2]                    @memory[1]
        strh        r5,  [r4, #6]                    @memory[3] = memory[2]
        smlabb      r14, r5, r7, r14                 @LSUM += memory[2] * coef[2]
        ldrsh       r5,  [r4]                        @memory[0]
        strh        r11, [r4, #4]                    @memory[2] = memory[1]
        smlabt      r14, r11, r6, r14                @LSUM += memory[1] * coef[1]
        strh        r5,  [r4, #2]                    @memory[1] = memory[0]
        smlabb      r14, r5, r6, r14                 @LSUM += memory[0] * coef[0]
        
        rsb         r14, r14, #0                     @L_temp = (-LSUM) << 4             
        mov         r5, r14, LSL #4
        add         r12, r12, #1
        add         r5, r5, #0x8000                            
        mov         r5, r5, ASR #16
        strh        r5, [r4]                         @*memory = (L_temp + 0x8000) >> 16
        cmp         r12,  r3
        strh        r5, [r0], #2
        blt         LOOP  

          
        
        ldr         r0, Tab                         @sp = GAMMA11                  
        mov         r12, #0x0800
        mov         r7, #0x0    
        strh        r12, [r4]                        @memory[0] = 0x0800
        strh        r7, [r4, #2]                     @memory[1] = 0
        str         r7, [r4, #4]                     @memory[2],memory[3] = 0
        str         r7, [r4, #8]                     @memory[4],memory[5] = 0
        str         r7, [r4, #12]                    @memory[6],memory[7] = 0
        str         r7, [r4, #16]                    @memory[8],memory[9] = 0
  
        mov         r14, #0x4000
        ldr         r11, [r0]                        @load GAMMA11
        ldr         r12, [r1]                        @load coef_uq[0], coef_uq[1]
        smlabb      r10, r11, r12, r14
        mov         r6, r10, ASR #15                 @wcoef[0] = ((*sp++) * (*sp1) + 0x4000) >> 15
        smlatt      r10, r11, r12, r14
        mov         r7, r10, ASR #15
        pkhbt       r6, r6, r7, LSL #16
        ldr         r11, [r0, #4]
        ldr         r12, [r1, #4]
        smlabb      r10, r11, r12, r14
        mov         r7, r10, ASR #15
        smlatt      r10, r11, r12, r14
        mov         r8, r10, ASR #15
        pkhbt       r7, r7, r8, LSL #16           
        ldr         r11, [r0, #8]
        ldr         r12, [r1, #8]
        smlabb      r10, r11, r12, r14
        mov         r8, r10, ASR #15
        smlatt      r10, r11, r12, r14
        mov         r9, r10, ASR #15
        pkhbt       r8, r8, r9, LSL #16
        ldr         r11, [r0, #12]
        ldr         r12, [r1, #12]
        smlabb      r10, r11, r12, r14
        mov         r9, r10, ASR #15
        smlatt      r10, r11, r12, r14
        mov         r10, r10, ASR #15
        pkhbt       r9, r9, r10, LSL #16
 
        ldr         r11, [r0, #16]
        ldr         r12, [r1, #16]
        smlabb      r10, r11, r12, r14
        mov         r10, r10, ASR #15
        smlatt      r0, r11, r12, r14
        mov         r0, r0, ASR #15
        pkhbt       r10, r10, r0, LSL #16

        mov         r12, #0x1                        @i=1
        ldr         r0, [sp, #20]
        add         r0, r0, #2                       @point output[1]

LOOP1:
        ldrsh       r5, [r0]                         @load output[1]
        mov         r14, r5, LSL #12                 @LSUM = output[i] << 12@
        ldrsh       r11, [r4, #18]                   @memory[9]
        ldrsh       r5,  [r4, #16]                   @memory[8]
        smlabt      r14, r11, r10, r14               @L_SUM += memory[9] * wcoef[9]
        ldrsh       r11, [r4, #14]                   @memory[7]
        strh        r5,  [r4, #18]                   @memory[9] = memory[8]
        smlabb      r14, r5, r10, r14                @L_SUM += memory[8] * wcoef[8]
        ldrsh       r5,  [r4, #12]                   @memory[6]
        strh        r11, [r4, #16]                   @memory[8] = memory[7]
        smlabt      r14, r11, r9, r14                @L_SUM += memory[7] * wcoef[7]
        ldrsh       r11, [r4, #10]                   @memory[5]
        strh        r5,  [r4, #14]                   @memory[7] = memory[6]
        smlabb      r14, r5, r9, r14                 @L_SUM += memory[6] * wcoef[6]
        ldrsh       r5,  [r4, #8]                    @memory[4]
        strh        r11, [r4, #12]                   @memory[6] = memory[5]
        smlabt      r14, r11, r8, r14                @L_SUM += memory[5] * wcoef[5]
        ldrsh       r11, [r4, #6]                    @memory[3]
        strh        r5,  [r4, #10]                   @memory[5] = memory[4]
        smlabb      r14, r5, r8, r14                 @L_SUM += memory[4] * wcoef[4]
        ldrsh       r5,  [r4, #4]                    @memory[2]
        strh        r11, [r4, #8]                    @memory[4] = memory[3]
        smlabt      r14, r11, r7, r14                @L_SUM += memory[3] * wcoef[3]
        ldrsh       r11, [r4, #2]                    @memory[1]
        strh        r5, [r4, #6]                     @memory[3] = memory[2]
        smlabb      r14, r5, r7, r14                 @L_SUM += memory[2] * wcoef[2]
        ldrsh       r5, [r4]                         @memory[0]
        strh        r11, [r4, #4]                    @memory[2] = memory[1]
        smlabt      r14, r11, r6, r14                @L_SUM += memory[1] * wcoef[1]
        strh        r5, [r4, #2]                     @memory[1] = memory[0]
        smlabb      r14, r5, r6, r14                 @L_SUM += memory[0] * wcoef[0]

        ldrsh       r5, [r0]                         @output[i] 
        mov         r11, r14, LSL #4                 @L_temp = LSUM << 4                                      
        add         r12, r12, #1                     @i++
        add         r14, r11, #0x8000
        mov         r11, r14, ASR #16
        strh        r5, [r4]                         @*memory = output[i]
        strh        r11, [r0], #2                    @output[i] = (L_temp + 0x8000) >> 16
        cmp         r12, r3
        blt         LOOP1

        mov         r12, #0x0800
        mov         r7, #0x0    
        strh        r12, [r4]                        @memory[0] = 0x0800
        strh        r7, [r4, #2]                     @memory[1] = 0
        str         r7, [r4, #4]                     @memory[2],memory[3] = 0
        str         r7, [r4, #8]                     @memory[4],memory[5] = 0
        str         r7, [r4, #12]                    @memory[6],memory[7] = 0
        str         r7, [r4, #16]                    @memory[8],memory[9] = 0 

        mov         r14, #0x4000 
        ldrsh       r6,  [r1]                        @coef_uq[0]
        ldrsh       r7,  [r1, #2]                    @coef_uq[1]
        ldrsh       r8,  [r1, #4]                    @coef_uq[2]
        ldrsh       r9,  [r1, #6]                    @coef_uq[3]
        add         r10, r14, r6, LSL #14
        add         r11, r14, r7, LSL #13
        mov         r6, r10, ASR #15
        mov         r7, r11, ASR #15
        pkhbt       r6, r6, r7, LSL #16            
        ldrsh       r5,  [r1, #8]                    @coef_uq[4]
        ldrsh       r12, [r1, #10]                   @coef_uq[5]
        add         r7, r14, r8, LSL #12
        add         r10, r14, r9, LSL #11
        mov         r7, r7, ASR #15
        mov         r8, r10, ASR #15
        pkhbt       r7, r7, r8, LSL #16
        ldrsh       r10, [r1, #12]                   @coef_uq[6]
        ldrsh       r11, [r1, #14]                   @coef_uq[7]
        add         r8, r14, r5, LSL #10
        add         r9, r14, r12, LSL #9
        mov         r8, r8, ASR #15
        mov         r9, r9, ASR #15
        pkhbt       r8, r8, r9, LSL #16
        ldrsh       r5, [r1, #16]                    @coef_uq[8]
        ldrsh       r12, [r1, #18]                   @coef_uq[9]
        add         r9, r14, r10, LSL #8
        add         r10, r14, r11, LSL #7
        mov         r9, r9, ASR #15
        mov         r10, r10, ASR #15
        pkhbt       r9, r9, r10, LSL #16
        add         r10, r14, r5, LSL #6
        add         r5, r14, r12, LSL #5
        mov         r10, r10, ASR #15
        mov         r5, r5, ASR #15
        pkhbt       r10, r10, r5, LSL #16 

        mov         r12, #0x1                        @i=1
        ldr         r0, [sp, #20]
        add         r0, r0, #2                       @point output[1]
LOOP2:
        ldrsh       r5, [r4, #18]                    @memory[9]
        ldrsh       r11, [r4, #16]                   @memory[8]
        smulbt      r14, r5, r10                     @tmp = memory[9] * wcoef[9]             
        ldrsh       r11, [r4, #14]                   @memory[7]
        strh        r5,  [r4, #18]                   @memory[9] = memory[8]
        smlabb      r14, r5, r10, r14                @tmp += memory[8] * coef[8]
        ldrsh       r5,  [r4, #12]                   @memory[6]
        strh        r11, [r4, #16]                   @memory[8] = memory[7]
        smlabt      r14, r11, r9, r14                @tmp += memory[7] * coef[7]
        ldrsh       r11, [r4, #10]                   @memory[5]
        strh        r5,  [r4, #14]                   @memory[7] = memory[6]
        smlabb      r14, r5, r9, r14                 @tmp += memory[6] * coef[6]
        ldrsh       r5,  [r4, #8]                    @memory[4]
        strh        r11, [r4, #12]                   @memory[6] = memory[5]
        smlabt      r14, r11, r8, r14                @tmp += memory[5] * coef[5]
        ldrsh       r11, [r4, #6]                    @memory[3]
        strh        r5,  [r4, #10]                   @memory[5] = memory[4]
        smlabb      r14, r5, r8, r14                 @tmp += memory[4] * coef[4]
        ldrsh       r5,  [r4, #4]                    @memory[2]
        strh        r11, [r4, #8]                    @memory[4] = memoey[3]
        smlabt      r14, r11, r7, r14                @tmp += memory[3] * coef[3]
        ldrsh       r11, [r4, #2]                    @memory[1]
        strh        r5,  [r4, #6]                    @memory[3] = memory[2]
        smlabb      r14, r5, r7, r14                 @tmp += memory[2] * coef[2]
        ldrsh       r5,  [r4]                        @memory[0]
        strh        r11, [r4, #4]                    @memory[2] = memory[1]
        smlabt      r14, r11, r6, r14                @tmp += memory[1] * coef[1]
        strh        r5,  [r4, #2]                    @memory[1] = memory[0]
        smlabb      r14, r5, r6, r14                 @tmp += memory[0] * coef[0]   
        ldrsh       r5,  [r0] 
        mov         r11, r5, LSL #12                 @output[i] << 12
        sub         r10, r11, r14                    @LSUM -= tmp
        mov         r14, r10, LSL #4                 @L_temp = LSUM << 4
        add         r5, r14, #0x8000
        mov         r10, r5, ASR #16                 @s_temp = (L_temp + 0x8000) >> 16
        add         r12, r12, #1
        strh        r10, [r4]                        @*memory = s_temp
        strh        r10, [r0], #2                    @output[i] = s_temp
        cmp         r12, r3
        blt         LOOP2
  
        add         sp, sp, #60
        ldmfd       sp!, {r4 - r12,pc} 

Tab:     
        .word         _GAMMA11
        @.END

