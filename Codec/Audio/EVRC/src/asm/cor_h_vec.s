;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;* File Name: 
;*            cor_h_vec.s
;* Description: 
;*            This module implements the cor_h_vec().
;* Functions Included:
;*            1. void cor_h_vec
;*
;***************************** Change History**************************
;* 
;*    DD/MMM/YYYY     Code Ver     Description             Author
;*    -----------     --------     -----------             ------
;*    04-07-2009        1.0        File imported from      Huaping Liu
;*                                             
;**********************************************************************
;static void cor_h_vec123(
;		         Shortword h[],		/* (i) scaled impulse response */
;		         Shortword vec[],	        /* (i) vector to correlate with h[] */
;		         Shortword track,	        /* (i) track to use */
;		         Shortword sign[],    	/* (i) sign vector */
;		         Shortword rrixix[][NB_POS],/* (i) correlation of h[x] with h[x] */
;		         Shortword corx[],           /* (o) result of correlation (NB_POS elements) */
;                        Shortword cory[]
;)
;********************
; ARM Register
;********************
;r0   ---  h[]
;r1   ---  vec[]
;r2   ---  track
;r3   ---  sign[]
;r4   ---  rrixix[][NB_POS]
;r5   ---  corx[]
;r6   ---  cory[]
        AREA    |.text|, CODE, READONLY
        EXPORT  cor_h_vec123 

cor_h_vec123 FUNCTION
        STMFD       sp!, {r4-r12,lr}
        MOV         r5, #22
        MOV         r10, #0                     ;i = 0
        LDR         r4, [sp, #40]               ;get rrixix[][NB_POS]
        MUL         r8, r2, r5                 
        ADD         r7, r4, r8                  ;p0 = rrixix[track]
        ADD         r8, r7, #22                 ;p3 = rrixix[track + 1]

LOOP1
        MOV         r4, #0                      ;s  = 0
        MOV         r9, #0                      ;s1 = 0
        MOV         r11, r0                     ;p1 = h
        ADD         r12, r1, r2, LSL #1         ;p2 = &vec[pos]
        RSB         r14, r2, #54                ;j = L_SUBFR - pos - 1

LOOP2
        LDRSH       r5, [r11], #2               ;*p1
        LDRSH       r6, [r12], #2               ;*p2++
        MLA         r4, r5, r6, r4              ;s += (*p1) * (*p2++)
        LDRSH       r6, [r12]                   ;*p2
        SUBS        r14, r14, #1
        MLA         r9, r5, r6, r9              ;s1 += (*p1++) * (*p2)
        BGT         LOOP2


        LDRSH       r5, [r11]                   ;*p1
        LDRSH       r6, [r12]                   ;*p2
        MOV         r9, r9, LSL #1              ;s1 <<=1
        MLA         r4, r5, r6, r4              ;s += (*p1) * (*p2)
        ADD         r9, r9, #0x8000
        ADD         r14, r3, r2, LSL #1         ;sign[pos]
        MOV         r4, r4, LSL #1              ;s <<=1
        ADD         r4, r4, #0x8000
        MOV         r9, r9, ASR #16             ;s1_temp = (s1 + 0x8000) >> 16

        LDRSH       r11, [r14], #2              ;sign[pos]
        MOV         r4, r4, ASR #16             ;s_temp = (s + 0x8000) >> 16
        LDRSH       r12, [r14]                  ;sign[pos + 1]
        LDRSH       r5, [r7], #2                ;*p0++
        MUL         r14, r4, r11
        MUL         r11, r9, r12
        LDRSH       r6, [r8], #2                ;*p3++
        MOV         r14, r14, ASR #15
        ADD         r14, r14, r5
        MOV         r11, r11, ASR #15
        LDR         r5, [sp, #44]               ;get corx[] 
        ADD         r11, r11, r6
        LDR         r6, [sp, #48]               ;get cory[]    
        ADD         r4, r5, r10, LSL #1
        ADD         r2, r2, #5
        ADD         r9, r6, r10, LSL #1
        STRH        r14, [r4]
        STRH        r11, [r9]
        ADD         r10, r10, #1
        CMP         r10, #11
        BLT         LOOP1  
        
        
        LDMFD       sp!, {r4 - r12,pc} 
        ENDFUNC
        END

