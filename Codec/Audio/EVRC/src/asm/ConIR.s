;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;* File Name: 
;*            ConIR.s
;* Description: 
;*            This module implements the ConvolveImpulseR().
;* Functions Included:
;*            1. void ConvolveImpulseR
;*
;***************************** Change History**************************
;* 
;*    DD/MMM/YYYY     Code Ver     Description             Author
;*    -----------     --------     -----------             ------
;*    04-21-2009        1.0        File imported from      Huaping Liu
;*                                             
;**********************************************************************
;void ConvolveImpulseR(
;		short *out,
;		short *in,
;		short *H,
;		short hlength,
;		short length)
;********************
; ARM Register
;********************
;r0   ---  *out
;r1   ---  *in
;r2   ---  *H
;r3   ---  length
        AREA    |.text|, CODE, READONLY
        EXPORT  ConIR_asm  

ConIR_asm FUNCTION
        STMFD       sp!, {r4-r12,lr}
        MOV         r5, #0                          ;i=0
        MOV         r4, #0x8000                       

LOOP
        ADD         r7, r1, r5, LSL #1              ;sp1 = &in[i]
        MOV         r8, r2                          ;sp2 = H
        MOV         r6, #0                          ;ltemp = 0
        MOV         r9, #0                          ;j = 0
LOOP1
        LDRSH       r10, [r7], #-2                  ;*(sp1--)
        LDRSH       r11, [r8], #2                   ;*(sp2++)
        ADD         r9, r9, #1
        CMP         r9, r5
        MLA         r6, r10, r11, r6                ;ltemp += *(sp1--) * *(sp2++)
        BLS         LOOP1
        MOV         r12, r6, LSL #5                 ;ltemp <<= 5
        ADD         r10, r12, r4
        ADD         r5, r5, #1                      ;i++
        MOV         r11, r10, ASR #16
        CMP         r5, r3
        STRH        r11, [r0], #2
        BLT         LOOP
        

        LDMFD       sp!, {r4 - r12,pc} 
        ENDFUNC
              
        END

