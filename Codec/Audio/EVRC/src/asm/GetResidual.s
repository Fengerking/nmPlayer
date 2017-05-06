;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;* File Name: 
;*            GetResidual.s
;* Description: 
;*            This module implements the GetResidual().
;* Functions Included:
;*            1. void GetResidual
;*
;***************************** Change History**************************
;* 
;*    DD/MMM/YYYY     Code Ver     Description             Author
;*    -----------     --------     -----------             ------
;*    04-28-2009        1.0        File imported from      Huaping Liu
;*                                             
;**********************************************************************
;void GetResidual(
;		short *res,
;		short *input,
;		short *coef,
;		short *FIRmemory,
;		short order,
;		short length)
;********************
; ARM Register
;********************
;r0   ---  *res
;r1   ---  *input
;r2   ---  *coef
;r3   ---  *FIRmemory
;r4   ---  order
;r5   ---  length
        AREA    |.text|, CODE, READONLY
        EXPORT  GetResidual_asm  

GetResidual_asm FUNCTION
        STMFD       sp!, {r4-r12,lr}
        LDR         r5, [sp, #44]               ;get length
        LDR         r4, [r2]                    ;coef[0], coef[1]
        LDR         r6, [r2, #4]                ;coef[2], coef[3]
        LDR         r7, [r2, #8]                ;coef[4], coef[5]
        LDR         r8, [r2, #12]               ;coef[6], coef[7]
        LDR         r9, [r2, #16]               ;coef[8], coef[9]
        MOV         r2, #0                      ;i = 0

LOOP
        LDRSH       r10, [r3, #18]              ;FIRmemory[9]
        LDRSH       r11, [r3, #16]              ;FIRmemory[8]
        SMULBT      r14, r10, r9                ;LSUM = coef[9] * FIRmemory[9]
        STRH        r11, [r3, #18]              ;FIRmemory[9] = FIRmemory[8]
        LDRSH       r10, [r3, #14]              ;FIRmemory[7]
        SMLABB      r14, r11, r9, r14           ;LSUM += coef[8] * FIRmemory[8]
        LDRSH       r11, [r3, #12]              ;FIRmemory[6]
        STRH        r10, [r3, #16]              ;FIRmemory[8] = FIRmemory[7]
        SMLABT      r14, r10, r8, r14           ;LSUM += coef[7] * FIRmemory[7]
        STRH        r11, [r3, #14]              ;FIRmemory[7] = FIRmemory[6]
        LDRSH       r10, [r3, #10]              ;FIRmemory[5]
        SMLABB      r14, r11, r8, r14           ;LSUM += coef[6] * FIRmemory[6]
        LDRSH       r11, [r3, #8]               ;FIRmemory[4]
        STRH        r10, [r3, #12]              ;FIRmemory[6] = FIRmemory[5]
        SMLABT      r14, r10, r7, r14           ;LSUM += coef[5] * FIRmemory[5]
        LDRSH       r10, [r3, #6]               ;FIRmemory[3]
        STRH        r11, [r3, #10]              ;FIRmemory[5] = FIRmemory[4]
        SMLABB      r14, r11, r7, r14           ;LSUM += coef[4] * FIRmemory[4]
        LDRSH       r11, [r3, #4]               ;FIRmemory[2]
        STRH        r10, [r3, #8]               ;FIRmemory[4] = FIRmemory[3]
        SMLABT      r14, r10, r6, r14           ;LSUM += coef[3] * FIRmemory[3]
        LDRSH       r10, [r3, #2]               ;FIRmemory[1]
        STRH        r11, [r3, #6]               ;FIRmemory[3] = FIRmemory[2]
        SMLABB      r14, r11, r6, r14           ;LSUM += coef[2] * FIRmemory[2]
        LDRSH       r11, [r3]                   ;FIRmemory[0]
        STRH        r10, [r3, #4]               ;FIRmemory[2] = FIRmemory[1]
        SMLABT      r14, r10, r4, r14           ;LSUM += coef[1] * FIRmemory[1]
        LDRSH       r12, [r1], #2               ;input[i]
        STRH        r11, [r3, #2]               ;FIRmemory[1] = FIRmemory[0]
        SMLABB      r14, r11, r4, r14           ;LSUM += coef[0] * FIRmemory[0]
        MOV         r11, #0x800
        MOV         r10, r12, LSL #12           ;input[i] << 12
        STRH        r12, [r3]                   ;FIRmemory[0] = input[i]
        ADD         r14, r14, r10               ;LSUM += (input[i] << 12)
        ADD         r14, r14, r11
        ADD         r2, r2, #1
        MOV         r10, r14, ASR #12
        STRH        r10, [r0], #2               ;res[i] = (LSUM + 0x800) >> 12
        CMP         r2,  r5
        BLT         LOOP    
        

        LDMFD       sp!, {r4 - r12,pc} 
        ENDFUNC
        END

