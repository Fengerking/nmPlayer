;**************************************************************
;* Copyright 2003~2009 by VisualOn Corporation, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Corporation, Inc. ALL RIGHTS RESERVED.
;****************************************************************

        AREA    |.text|, CODE, READONLY
        EXPORT  Inv_sqrt_asm
        IMPORT  tabsqr

Inv_sqrt_asm  PROC

        STMDB      sp!, {r1 - r12, lr}

        LDR     r11, Tab     
        MOV     r1, r0
        MOV     r2, #0
        CMP     r0, #0x40000000
        MOVGE   r2, #16           
        MOVGE   r3, r0, asr #1
        BGE     L2
L1
        MOV     r1, r1, lsl #1
        ADD     r2, r2, #1
        CMP     r1, #0x40000000
        BLT     L1
        MOV     r3, r0, lsl r2
        RSB     r2, r2, #30
        ANDS    r0, r2, #1
        MOVEQ   r3, r3, asr #1
        MOV     r0, r2, asr #1
        ADD     r2, r0, #1           ; exp---r2
L2
        MOV     r4, r3, asr #25      ; i = L_x >>25--r4
        LDR     r7, L10              ; Inv_sqrt_table addess
        MOV     r5, r3, asr #10      ; a = L_x >>10--r5
        SUB     r4, r4, #16          ; i = (i -16)
        AND     r5, r5, r11          ; a = a &(Word16) 0x7fff
        MOV     r10, r4, lsl #1    
        LDRSH   r8, [r7,r10]         ; Inv_sqrt_table
        ADD     r1, r10, #2          ; i+1
        LDRSH   r11, [r7,r1]         ; r10 = Inv_sqrt_table[i+1]
        MOV     r0, r8, lsl #16      ; L_y--r0 = Inv_sqrt_table[i] <<16
        SUB     r4, r8,r11           ; tmp = r4
        MUL     r4, r4,r5            ; tmp *a
        MOV     r4, r4, lsl #1      
        SUB     r0, r0, r4
        MOV     r0, r0, asr r2                          
       
        LDMIA   sp!, {r1 - r12, pc}   
        
        ENDP 
L10
        DCD   tabsqr

Tab     DCD   0x7fff 
       
        END
      
