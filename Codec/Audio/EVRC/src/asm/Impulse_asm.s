;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;* File Name: 
;*            Impulse_asm.s
;* Description: 
;*            This module implements the ImpulseRzp().
;* Functions Included:
;*            1. void ImpulseRzp
;*
;***************************** Change History**************************
;* 
;*    DD/MMM/YYYY     Code Ver     Description             Author
;*    -----------     --------     -----------             ------
;*    04-21-2009        1.0        File imported from      Huaping Liu
;*                                             
;**********************************************************************
;void ImpulseRzp1(
;		short *output,
;		short *coef_uq,
;		short *coef,
;		short length)
;********************
; ARM Register
;********************
;r0   ---  *output
;r1   ---  *coef_uq
;r2   ---  *coef
;r3   ---  length
        AREA    |.text|, CODE, READONLY
        EXPORT  ImpulseRzp1_asm  
        IMPORT  GAMMA11

ImpulseRzp1_asm FUNCTION
        STMFD       sp!, {r4-r12,lr}
        SUB         sp, sp, #60 
        ADD         r4, sp, #40                     ;r4 --- &memory[0]
        STR         r0, [sp, #20]                    ;store r0

        MOV         r6, #0x0800
        MOV         r7, #0x0    
        STRH        r6, [r0], #2                     ;output[0] = 0x0800 
        STRH        r6, [r4]                         ;memory[0] = 0x0800
        STRH        r7, [r4, #2]                     ;memory[1] = 0
        STR         r7, [r4, #4]                     ;memory[2],memory[3] = 0
        STR         r7, [r4, #8]                     ;memory[4],memory[5] = 0
        STR         r7, [r4, #12]                    ;memory[6],memory[7] = 0
        STR         r7, [r4, #16]                    ;memory[8],memory[9] = 0

        MOV         r12, #0x1                        ;i = 1
        LDR         r6,  [r2]                        ;coef[0], coef[1]
        LDR         r7,  [r2, #4]                    ;coef[2], coef[3]
        LDR         r8,  [r2, #8]                    ;coef[4], coef[5]
        LDR         r9,  [r2, #12]                   ;coef[6], coef[7]
        LDR         r10, [r2, #16]                   ;coef[8], coef[9] 

LOOP
        LDRSH       r11, [r4, #18]                   ;memory[9]
        LDRSH       r5,  [r4, #16]                   ;memory[8]
        SMULBT      r14, r11, r10                    ;LSUM = memory[9] * coef[9]
        LDRSH       r11, [r4, #14]                   ;memory[7]
        STRH        r5,  [r4, #18]                   ;memory[9] = memory[8]
        SMLABB      r14, r5, r10, r14                ;LSUM += memory[8] * coef[8]
        LDRSH       r5,  [r4, #12]                   ;memory[6]
        STRH        r11, [r4, #16]                   ;memory[8] = memory[7]
        SMLABT      r14, r11, r9, r14                ;LSUM += memory[7] * coef[7]
        LDRSH       r11, [r4, #10]                   ;memory[5]
        STRH        r5,  [r4, #14]                   ;memory[7] = memory[6]
        SMLABB      r14, r5, r9, r14                 ;LSUM += memory[6] * coef[6]
        LDRSH       r5,  [r4, #8]                    ;memory[4]
        STRH        r11, [r4, #12]                   ;memory[6] = memory[5]
        SMLABT      r14, r11, r8, r14                ;LSUM += memory[5] * coef[5]
        LDRSH       r11, [r4, #6]                    ;memory[3]
        STRH        r5,  [r4, #10]                   ;memory[5] = memory[4]
        SMLABB      r14, r5, r8, r14                 ;LSUM += memory[4] * coef[4]
        LDRSH       r5,  [r4, #4]                    ;memory[2]
        STRH        r11, [r4, #8]                    ;memory[4] = memoey[3]
        SMLABT      r14, r11, r7, r14                ;LSUM += memory[3] * coef[3]
        LDRSH       r11, [r4, #2]                    ;memory[1]
        STRH        r5,  [r4, #6]                    ;memory[3] = memory[2]
        SMLABB      r14, r5, r7, r14                 ;LSUM += memory[2] * coef[2]
        LDRSH       r5,  [r4]                        ;memory[0]
        STRH        r11, [r4, #4]                    ;memory[2] = memory[1]
        SMLABT      r14, r11, r6, r14                ;LSUM += memory[1] * coef[1]
        STRH        r5,  [r4, #2]                    ;memory[1] = memory[0]
        SMLABB      r14, r5, r6, r14                 ;LSUM += memory[0] * coef[0]
        
        RSB         r14, r14, #0                     ;L_temp = (-LSUM) << 4             
        MOV         r5, r14, LSL #4
        ADD         r12, r12, #1
        ADD         r5, r5, #0x8000                            
        MOV         r5, r5, ASR #16
        STRH        r5, [r4]                         ;*memory = (L_temp + 0x8000) >> 16
        CMP         r12,  r3
        STRH        r5, [r0], #2
        BLT         LOOP  

          
        
        LDR         r0, Tab                         ;sp = GAMMA11                  
        MOV         r12, #0x0800
        MOV         r7, #0x0    
        STRH        r12, [r4]                        ;memory[0] = 0x0800
        STRH        r7, [r4, #2]                     ;memory[1] = 0
        STR         r7, [r4, #4]                     ;memory[2],memory[3] = 0
        STR         r7, [r4, #8]                     ;memory[4],memory[5] = 0
        STR         r7, [r4, #12]                    ;memory[6],memory[7] = 0
        STR         r7, [r4, #16]                    ;memory[8],memory[9] = 0
  
        MOV         r14, #0x4000
        LDR         r11, [r0]                        ;load GAMMA11
        LDR         r12, [r1]                        ;load coef_uq[0], coef_uq[1]
        SMLABB      r10, r11, r12, r14
        MOV         r6, r10, ASR #15                 ;wcoef[0] = ((*sp++) * (*sp1) + 0x4000) >> 15
        SMLATT      r10, r11, r12, r14
        MOV         r7, r10, ASR #15
        PKHBT       r6, r6, r7, LSL #16
        LDR         r11, [r0, #4]
        LDR         r12, [r1, #4]
        SMLABB      r10, r11, r12, r14
        MOV         r7, r10, ASR #15
        SMLATT      r10, r11, r12, r14
        MOV         r8, r10, ASR #15
        PKHBT       r7, r7, r8, LSL #16           
        LDR         r11, [r0, #8]
        LDR         r12, [r1, #8]
        SMLABB      r10, r11, r12, r14
        MOV         r8, r10, ASR #15
        SMLATT      r10, r11, r12, r14
        MOV         r9, r10, ASR #15
        PKHBT       r8, r8, r9, LSL #16
        LDR         r11, [r0, #12]
        LDR         r12, [r1, #12]
        SMLABB      r10, r11, r12, r14
        MOV         r9, r10, ASR #15
        SMLATT      r10, r11, r12, r14
        MOV         r10, r10, ASR #15
        PKHBT       r9, r9, r10, LSL #16
 
        LDR         r11, [r0, #16]
        LDR         r12, [r1, #16]
        SMLABB      r10, r11, r12, r14
        MOV         r10, r10, ASR #15
        SMLATT      r0, r11, r12, r14
        MOV         r0, r0, ASR #15
        PKHBT       r10, r10, r0, LSL #16

        MOV         r12, #0x1                        ;i=1
        LDR         r0, [sp, #20]
        ADD         r0, r0, #2                       ;point output[1]

LOOP1
        LDRSH       r5, [r0]                         ;load output[1]
        MOV         r14, r5, LSL #12                 ;LSUM = output[i] << 12;
        LDRSH       r11, [r4, #18]                   ;memory[9]
        LDRSH       r5,  [r4, #16]                   ;memory[8]
        SMLABT      r14, r11, r10, r14               ;L_SUM += memory[9] * wcoef[9]
        LDRSH       r11, [r4, #14]                   ;memory[7]
        STRH        r5,  [r4, #18]                   ;memory[9] = memory[8]
        SMLABB      r14, r5, r10, r14                ;L_SUM += memory[8] * wcoef[8]
        LDRSH       r5,  [r4, #12]                   ;memory[6]
        STRH        r11, [r4, #16]                   ;memory[8] = memory[7]
        SMLABT      r14, r11, r9, r14                ;L_SUM += memory[7] * wcoef[7]
        LDRSH       r11, [r4, #10]                   ;memory[5]
        STRH        r5,  [r4, #14]                   ;memory[7] = memory[6]
        SMLABB      r14, r5, r9, r14                 ;L_SUM += memory[6] * wcoef[6]
        LDRSH       r5,  [r4, #8]                    ;memory[4]
        STRH        r11, [r4, #12]                   ;memory[6] = memory[5]
        SMLABT      r14, r11, r8, r14                ;L_SUM += memory[5] * wcoef[5]
        LDRSH       r11, [r4, #6]                    ;memory[3]
        STRH        r5,  [r4, #10]                   ;memory[5] = memory[4]
        SMLABB      r14, r5, r8, r14                 ;L_SUM += memory[4] * wcoef[4]
        LDRSH       r5,  [r4, #4]                    ;memory[2]
        STRH        r11, [r4, #8]                    ;memory[4] = memory[3]
        SMLABT      r14, r11, r7, r14                ;L_SUM += memory[3] * wcoef[3]
        LDRSH       r11, [r4, #2]                    ;memory[1]
        STRH        r5, [r4, #6]                     ;memory[3] = memory[2]
        SMLABB      r14, r5, r7, r14                 ;L_SUM += memory[2] * wcoef[2]
        LDRSH       r5, [r4]                         ;memory[0]
        STRH        r11, [r4, #4]                    ;memory[2] = memory[1]
        SMLABT      r14, r11, r6, r14                ;L_SUM += memory[1] * wcoef[1]
        STRH        r5, [r4, #2]                     ;memory[1] = memory[0]
        SMLABB      r14, r5, r6, r14                 ;L_SUM += memory[0] * wcoef[0]

        LDRSH       r5, [r0]                         ;output[i] 
        MOV         r11, r14, LSL #4                 ;L_temp = LSUM << 4                                      
        ADD         r12, r12, #1                     ;i++
        ADD         r14, r11, #0x8000
        MOV         r11, r14, ASR #16
        STRH        r5, [r4]                         ;*memory = output[i]
        STRH        r11, [r0], #2                    ;output[i] = (L_temp + 0x8000) >> 16
        CMP         r12, r3
        BLT         LOOP1

        MOV         r12, #0x0800
        MOV         r7, #0x0    
        STRH        r12, [r4]                        ;memory[0] = 0x0800
        STRH        r7, [r4, #2]                     ;memory[1] = 0
        STR         r7, [r4, #4]                     ;memory[2],memory[3] = 0
        STR         r7, [r4, #8]                     ;memory[4],memory[5] = 0
        STR         r7, [r4, #12]                    ;memory[6],memory[7] = 0
        STR         r7, [r4, #16]                    ;memory[8],memory[9] = 0 

        MOV         r14, #0x4000 
        LDRSH       r6,  [r1]                        ;coef_uq[0]
        LDRSH       r7,  [r1, #2]                    ;coef_uq[1]
        LDRSH       r8,  [r1, #4]                    ;coef_uq[2]
        LDRSH       r9,  [r1, #6]                    ;coef_uq[3]
        ADD         r10, r14, r6, LSL #14
        ADD         r11, r14, r7, LSL #13
        MOV         r6, r10, ASR #15
        MOV         r7, r11, ASR #15
        PKHBT       r6, r6, r7, LSL #16            
        LDRSH       r5,  [r1, #8]                    ;coef_uq[4]
        LDRSH       r12, [r1, #10]                   ;coef_uq[5]
        ADD         r7, r14, r8, LSL #12
        ADD         r10, r14, r9, LSL #11
        MOV         r7, r7, ASR #15
        MOV         r8, r10, ASR #15
        PKHBT       r7, r7, r8, LSL #16
        LDRSH       r10, [r1, #12]                   ;coef_uq[6]
        LDRSH       r11, [r1, #14]                   ;coef_uq[7]
        ADD         r8, r14, r5, LSL #10
        ADD         r9, r14, r12, LSL #9
        MOV         r8, r8, ASR #15
        MOV         r9, r9, ASR #15
        PKHBT       r8, r8, r9, LSL #16
        LDRSH       r5, [r1, #16]                    ;coef_uq[8]
        LDRSH       r12, [r1, #18]                   ;coef_uq[9]
        ADD         r9, r14, r10, LSL #8
        ADD         r10, r14, r11, LSL #7
        MOV         r9, r9, ASR #15
        MOV         r10, r10, ASR #15
        PKHBT       r9, r9, r10, LSL #16
        ADD         r10, r14, r5, LSL #6
        ADD         r5, r14, r12, LSL #5
        MOV         r10, r10, ASR #15
        MOV         r5, r5, ASR #15
        PKHBT       r10, r10, r5, LSL #16 

        MOV         r12, #0x1                        ;i=1
        LDR         r0, [sp, #20]
        ADD         r0, r0, #2                       ;point output[1]
LOOP2
        LDRSH       r5, [r4, #18]                    ;memory[9]
        LDRSH       r11, [r4, #16]                   ;memory[8]
        SMULBT      r14, r5, r10                     ;tmp = memory[9] * wcoef[9]             
        LDRSH       r11, [r4, #14]                   ;memory[7]
        STRH        r5,  [r4, #18]                   ;memory[9] = memory[8]
        SMLABB      r14, r5, r10, r14                ;tmp += memory[8] * coef[8]
        LDRSH       r5,  [r4, #12]                   ;memory[6]
        STRH        r11, [r4, #16]                   ;memory[8] = memory[7]
        SMLABT      r14, r11, r9, r14                ;tmp += memory[7] * coef[7]
        LDRSH       r11, [r4, #10]                   ;memory[5]
        STRH        r5,  [r4, #14]                   ;memory[7] = memory[6]
        SMLABB      r14, r5, r9, r14                 ;tmp += memory[6] * coef[6]
        LDRSH       r5,  [r4, #8]                    ;memory[4]
        STRH        r11, [r4, #12]                   ;memory[6] = memory[5]
        SMLABT      r14, r11, r8, r14                ;tmp += memory[5] * coef[5]
        LDRSH       r11, [r4, #6]                    ;memory[3]
        STRH        r5,  [r4, #10]                   ;memory[5] = memory[4]
        SMLABB      r14, r5, r8, r14                 ;tmp += memory[4] * coef[4]
        LDRSH       r5,  [r4, #4]                    ;memory[2]
        STRH        r11, [r4, #8]                    ;memory[4] = memoey[3]
        SMLABT      r14, r11, r7, r14                ;tmp += memory[3] * coef[3]
        LDRSH       r11, [r4, #2]                    ;memory[1]
        STRH        r5,  [r4, #6]                    ;memory[3] = memory[2]
        SMLABB      r14, r5, r7, r14                 ;tmp += memory[2] * coef[2]
        LDRSH       r5,  [r4]                        ;memory[0]
        STRH        r11, [r4, #4]                    ;memory[2] = memory[1]
        SMLABT      r14, r11, r6, r14                ;tmp += memory[1] * coef[1]
        STRH        r5,  [r4, #2]                    ;memory[1] = memory[0]
        SMLABB      r14, r5, r6, r14                 ;tmp += memory[0] * coef[0]   
        LDRSH       r5,  [r0] 
        MOV         r11, r5, LSL #12                 ;output[i] << 12
        SUB         r10, r11, r14                    ;LSUM -= tmp
        MOV         r14, r10, LSL #4                 ;L_temp = LSUM << 4
        ADD         r5, r14, #0x8000
        MOV         r10, r5, ASR #16                 ;s_temp = (L_temp + 0x8000) >> 16
        ADD         r12, r12, #1
        STRH        r10, [r4]                        ;*memory = s_temp
        STRH        r10, [r0], #2                    ;output[i] = s_temp
        CMP         r12, r3
        BLT         LOOP2
   
  
        ADD         sp, sp, #60
        LDMFD       sp!, {r4 - r12,pc} 
        ENDFUNC

Tab     DCD         GAMMA11



        END

