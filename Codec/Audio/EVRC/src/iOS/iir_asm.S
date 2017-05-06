@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@* File Name: 
@*            iir_asm.s
@* Description: 
@*            This module implements the iir().
@* Functions Included:
@*            1. void iir
@*
@***************************** Change History**************************
@* 
@*    DD/MMM/YYYY     Code Ver     Description             Author
@*    -----------     --------     -----------             ------
@*    04-14-2009        1.0        File imported from      Huaping Liu
@*                                             
@**********************************************************************
@void iir(
@	  short *output,
@	  short *input,
@	  short *coef,
@	  short *IIRmemory,
@	  short order,
@	  short length)
@********************
@ ARM Register
@********************
@r0   ---  *output
@r1   ---  *input
@r2   ---  *coef
@r3   ---  *IIRmemory
@r4   ---  order
@r5   ---  length
#include "voEvrcDecID.h"
        .text
        .align 4
	.globl    _iir_asm

_iir_asm:
        stmfd       sp!, {r4-r12,lr}
        ldr         r5, [sp, #44]               
        ldr         r4, [r2]                    
        ldr         r6, [r2, #4]                
        ldr         r7, [r2, #8]                
        ldr         r8, [r2, #12]               
        ldr         r9, [r2, #16]               
        mov         r2, #0                      

LOOP:
        ldrsh       r10, [r3, #18]              
        ldrsh       r11, [r3, #16]              
        smulbt      r14, r10, r9                
        strh        r11, [r3, #18]              
        ldrsh       r10, [r3, #14]              
        smlabb      r14, r11, r9, r14         
        ldrsh       r11, [r3, #12]            
        strh        r10, [r3, #16]            
        smlabt      r14, r10, r8, r14         
        strh        r11, [r3, #14]            
        ldrsh       r10, [r3, #10]            
        smlabb      r14, r11, r8, r14         
        ldrsh       r11, [r3, #8]             
        strh        r10, [r3, #12]            
        smlabt      r14, r10, r7, r14         
        ldrsh       r10, [r3, #6]             
        strh        r11, [r3, #10]            
        smlabb      r14, r11, r7, r14         
        ldrsh       r11, [r3, #4]             
        strh        r10, [r3, #8]             
        smlabt      r14, r10, r6, r14         
        ldrsh       r10, [r3, #2]             
        strh        r11, [r3, #6]             
        smlabb      r14, r11, r6, r14         
        ldrsh       r11, [r3]                 
        strh        r10, [r3, #4]             
        smlabt      r14, r10, r4, r14         
        ldrsh       r12, [r1], #2             
        strh        r11, [r3, #2]             
        smlabb      r14, r11, r4, r14         

        mov         r10, r12, LSL #12         
        @strh        r12, [r3]                
        rsb         r14, r14, r10             
        mov         r11, #0x8000
        mov         r10, r14, LSL #4          
        add         r2, r2, #1
        add         r14, r10, r11
        cmp         r2,  r5
        mov         r10, r14, ASR #16
        strh        r10, [r0], #2             
        strh        r10, [r3]                 
        blt         LOOP    
        

        ldmfd       sp!, {r4 - r12,pc} 
        @.END

