;**************************************************************
;* Copyright 2003~2009 by VisualOn Corporation, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Corporation, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;Lsp_pre_select(
;  Word16 rbuf[],              /* (i) Q13 : target vetor             */
;  Word16 lspcb1[][M],         /* (i) Q13 : first stage LSP codebook */
;  Word16 *cand                /* (o)     : selected code            */
;)
;   r0 --- rbuf
;   r1 --- lspcb1[][M]
;   r2 --- *cand

       AREA    |.text|, CODE, READONLY
       EXPORT Lsp_pre_select_asm

Lsp_pre_select_asm    PROC

       STMDB      sp!, {r4 - r12, lr}
       MOV        r10, #0x7fffffff              ; L_dmin = MAX_32       
       MOV        r3,  #0                       ; *cand = 0
       STRH       r3,  [r2]
       MOV        r12, #0
      ; MOV        r12, #127

Loop1
       MOV        r11, #0                       ; L_tmp = 0
  
       LDRSH      r3, [r0]                      
       LDRSH      r4, [r1]
       LDRSH      r5, [r0, #2]
       LDRSH      r6, [r1, #2]
       SUB        r7, r3, r4
       MLA        r11, r7, r7, r11
       SUB        r8, r5, r6
       MLA        r11, r8, r8, r11

       LDRSH      r3, [r0, #4]                      
       LDRSH      r4, [r1, #4]
       LDRSH      r5, [r0, #6]
       LDRSH      r6, [r1, #6]  
       SUB        r7, r3, r4
       MLA        r11, r7, r7, r11
       SUB        r8, r5, r6
       MLA        r11, r8, r8, r11

       LDRSH      r3, [r0, #8]                      
       LDRSH      r4, [r1, #8]
       LDRSH      r5, [r0, #10]
       LDRSH      r6, [r1, #10]  
       SUB        r7, r3, r4
       MLA        r11, r7, r7, r11
       SUB        r8, r5, r6
       MLA        r11, r8, r8, r11

       LDRSH      r3, [r0, #12]                      
       LDRSH      r4, [r1, #12]
       LDRSH      r5, [r0, #14]
       LDRSH      r6, [r1, #14]  
       SUB        r7, r3, r4
       MLA        r11, r7, r7, r11
       SUB        r8, r5, r6
       MLA        r11, r8, r8, r11

       LDRSH      r3, [r0, #16]                      
       LDRSH      r4, [r1, #16]
       LDRSH      r5, [r0, #18]
       LDRSH      r6, [r1, #18]  
       SUB        r7, r3, r4
       MLA        r11, r7, r7, r11
       SUB        r8, r5, r6
       MLA        r11, r8, r8, r11

       ADD        r1, r1, #20

       RSB        r3, r10, r11, LSL #1            ; L_temp=(L_tmp<<1) - L_dmin

       CMP        r3, #0
       BGE        Loop2
       MOV        r10, r11, LSL #1                ; L_dmin = L_tmp<<1
       STRH       r12, [r2]                       ; *cand = i
Loop2   
       ADD        r12, r12, #1
       CMP        r12, #128
       BLT        Loop1

       LDMIA      sp!, {r4 - r12, pc}
       ENDP
       END
