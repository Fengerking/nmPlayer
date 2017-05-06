@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@* File Name: 
@*            fir_asm.s
@* Description: 
@*            This module implements the fir().
@* Functions Included:
@*            1. void fir
@*
@***************************** Change History**************************
@* 
@*    DD/MMM/YYYY     Code Ver     Description             Author
@*    -----------     --------     -----------             ------
@*    07-14-2010        1.0        File imported from      Huaping Liu
@*                                             
@**********************************************************************
@void fir(
@	  short *output,
@	  short *input,
@	  short *coef,
@	  short *FIRmemory,
@	  short order,
@	  short length)
@********************
@ ARM Register
@********************
@r0   ---  *output
@r1   ---  *input
@r2   ---  *coef
@r3   ---  *FIRmemory
@r4   ---  order
@r5   ---  length
#include "voEvrcDecID.h"

        .section .text
        .global  fir_asm  

fir_asm:

        STMFD       sp!, {r4-r12,lr}
        LDR         r5, [sp, #44]             @ load length             
        LDR         r4, [r2]                    
        LDR         r6, [r2, #4]                
        LDR         r7, [r2, #8]                
        LDR         r8, [r2, #12]               
        LDR         r9, [r2, #16]               
        MOV         r2, #0                      

LOOP:
        LDRSH       r10, [r3, #18]              
        LDRSH       r11, [r3, #16]              
        SMULBT      r14, r10, r9                
        STRH        r11, [r3, #18]              
        LDRSH       r10, [r3, #14]              
        SMLABB      r14, r11, r9, r14         
        LDRSH       r11, [r3, #12]            
        STRH        r10, [r3, #16]            
        SMLABT      r14, r10, r8, r14         
        STRH        r11, [r3, #14]            
        LDRSH       r10, [r3, #10]            
        SMLABB      r14, r11, r8, r14         
        LDRSH       r11, [r3, #8]             
        STRH        r10, [r3, #12]            
        SMLABT      r14, r10, r7, r14         
        LDRSH       r10, [r3, #6]             
        STRH        r11, [r3, #10]            
        SMLABB      r14, r11, r7, r14         
        LDRSH       r11, [r3, #4]             
        STRH        r10, [r3, #8]             
        SMLABT      r14, r10, r6, r14         
        LDRSH       r10, [r3, #2]             
        STRH        r11, [r3, #6]             
        SMLABB      r14, r11, r6, r14         
        LDRSH       r11, [r3]                 
        STRH        r10, [r3, #4]             
        SMLABT      r14, r10, r4, r14         
        LDRSH       r12, [r1], #2             
        STRH        r11, [r3, #2]             
        SMLABB      r14, r11, r4, r14         

        MOV         r10, r12, LSL #12         
        @STRH        r12, [r3]                
        ADD         r14, r14, r10             
        MOV         r11, #0x8000
        MOV         r10, r14, LSL #4          
        ADD         r2, r2, #1
        ADD         r14, r10, r11
        CMP         r2,  r5
        MOV         r10, r14, ASR #16
        STRH        r10, [r0], #2             
        STRH        r12, [r3]                 
        BLT         LOOP    
        

        LDMFD       sp!, {r4 - r12,pc} 
        @ENDFUNC
        .END

