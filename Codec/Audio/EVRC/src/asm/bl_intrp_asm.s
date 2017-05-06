;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;* File Name: 
;*            bl_intrp.s
;* Description: 
;*            This module implements the bl_intrp().
;* Functions Included:
;*            1. void bl_intrp
;*
;***************************** Change History**************************
;* 
;*    DD/MMM/YYYY     Code Ver     Description             Author
;*    -----------     --------     -----------             ------
;*    04-14-2009        1.0        File imported from      Huaping Liu
;*                                             
;**********************************************************************
;void bl_intrp(
;	        short *output,   	/* int ptr */
;	        short *input,	        /* int ptr */
;		long delay,	        /* sign bit + 7bits int  + 24bits frac */
;		short factor,	        /* fractional value */
;		short fl		/* int value */
;)
;********************
; ARM Register
;********************
;r0   ---  *output
;r1   ---  *input
;r2   ---  delay
;r3   ---  factor
;r4   ---  f1

        AREA    |.text|, CODE, READONLY
        EXPORT  bl_intrp_asm  
        IMPORT  Table1
        IMPORT  Table

bl_intrp_asm FUNCTION

        STMFD       sp!, {r4-r12,lr}
        LDR         r4, [sp, #40]               ;get f1
        MOV         r7, r2, ASR #23             ;delay >> 23
        MOV         r6, #0x4000                 ;loutput = 0x4000
        ADD         r8, r7, #0x01               ;(delay>>23) + 0x01
        LDR         r5, =0x7333                 ;factor2 = 29491
        MOV         r9, r8, ASR #1              ;offset
        MOV         r7, r9, LSL #24             ;offset << 24
        SUB         r8, r7, r2                  ;(offset << 24) - delay
        MOV         r7, r8, ASR #15             ;((offset << 24) - delay)>>15
        ADD         r8, r7, #288                
        MOV         r7, r8, ASR #6              ;t = ((((offset << 24) - delay) >> 15) + 288) >> 6

        CMP         r7, #8
        MOVEQ       r7, #0                      ;t = 0
        SUBEQ       r9, r9, #1                  ;offset--
        ADD         r8, r9, r4                  ;offset + f1
        ADD         r10, r4, r4
        ADD         r10, r10, #1
        MUL         r11, r7, r10                ;t = (t * ((f1 << 1) + 1))
        SUB         r10, r1, r8, LSL #1         ;f = input - offset - f1
        LDR         r7, Tab1                    ;get Table1 address
        LDR         r12, Tab                    ;get Table address
        CMP         r3, r5
        ADDEQ       r12, r7, r11, LSL #1        ;coef_ptr = Table1 + t
        ADDNE       r12, r12, r11, LSL #1       ;coef_ptr = Table + t
        LDRSH       r5, [r12], #2               
        LDRSH       r7, [r10], #2
        MLA         r6, r5, r7, r6

LOOP        
        LDRSH       r8, [r12], #2
        LDRSH       r9, [r10], #2

        LDRSH       r5, [r12], #2
        LDRSH       r7, [r10], #2
        MLA         r6, r8, r9, r6
        MLA         r6, r5, r7, r6
        SUBS        r4, r4, #1
        BGT         LOOP
        MOV         r8, r6, ASR #15
        STRH        r8, [r0]
        
        LDMFD       sp!, {r4 - r12,pc} 
        ENDFUNC

Tab1    DCD         Table1
Tab     DCD         Table 
        END

