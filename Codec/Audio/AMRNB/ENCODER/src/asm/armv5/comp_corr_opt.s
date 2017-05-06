;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;void comp_corr ( 
;		Word16 scal_sig[],  /* i   : scaled signal.                          */
;		Word16 lag_max,     /* i   : maximum lag                             */
;		Word16 lag_min,     /* i   : minimum lag                             */
;		Word32 corr[])      /* o   : correlation of selected lag             */

;*************************************************
; ARM register 
;*************************************************
; short *scal_sig               RN           0
; lag_max                       RN           1
; lag_min                       RN           2
; *corr                         RN           3

        EXPORT comp_corr_asm
        AREA    |.text|, CODE, READONLY

comp_corr_asm  PROC	

        STMFD         r13!, {r4 - r12, r14}       
        ;for(i = lag_max; i >= lag_min; i--)
        
        SUB           r12, r3, r1, LSL #2            ; corr[-i] address
             
LOOP1
        MOV           r4, r0                         ; p = scal_sig       
        SUB           r7, r0, r1, LSL #1             ; p1 = &scal_sig[-i]
        MOV           r11, #80
        MOV           r14, #0                        ; t0 = 0
        MOV           r3, #0
LOOP2
        LDRSH         r8,  [r4], #2
        LDRSH         r6,  [r4], #2
        LDRSH         r9,  [r7], #2
        LDRSH         r10, [r7], #2
        MLA           r14, r8, r9, r3
        LDRSH         r8,  [r4], #2
        MLA           r3,  r6, r10, r14


        LDRSH         r6,  [r4], #2
        LDRSH         r9,  [r7], #2
        LDRSH         r10, [r7], #2
        MLA           r14, r8, r9, r3
        LDRSH         r8,  [r4], #2
        MLA           r3, r6, r10, r14


        LDRSH         r6,  [r4], #2
        LDRSH         r9,  [r7], #2
        LDRSH         r10, [r7], #2
        MLA           r14, r8, r9, r3
        LDRSH         r8,  [r4], #2
        MLA           r3, r6, r10, r14

        
        LDRSH         r6,  [r4], #2
        LDRSH         r9,  [r7], #2
        LDRSH         r10, [r7], #2
        MLA           r14, r8, r9, r3
        LDRSH         r8,  [r4], #2
        MLA           r3, r6, r10, r14

        
        LDRSH         r6,  [r4], #2
        LDRSH         r9,  [r7], #2
        LDRSH         r10, [r7], #2
        MLA           r14, r8, r9, r3
        SUBS          r11, r11, #10                  ; j -= 10
        MLA           r3, r6, r10, r14

       
        BNE           LOOP2
        

        MOV           r3, r3, LSL #1

        SUB           r1, r1, #1
        STR           r3, [r12], #4
        CMP           r1,  r2
        BGE           LOOP1

comp_corr_asm_end
                        
        LDMFD         r13!, {r4 - r12, r15} 
        ENDP
        END
