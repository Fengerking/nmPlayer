;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;*************************************************************** 
;void Pred_lt4(
;     Word16 exc[],                         /* in/out: excitation buffer */
;     Word16 T0,                            /* input : integer pitch lag */
;     Word16 frac,                          /* input : fraction of lag   */
;     Word16 L_subfr                        /* input : subframe size     */
;)
;***************************************************************
; r0    ---  exc[]
; r1    ---  T0
; r2    ---  frac
; r3    ---  L_subfr
 
          AREA    |.text|, CODE, READONLY
          EXPORT  Pred_lt4_asm
          IMPORT  inter4_2

Pred_lt4_asm FUNCTION
          STMFD   	r13!, {r4 - r12, r14} 
          SUB           r4, r0, r1, LSL #1                        ; x = exc - T0
          RSB           r2, r2, #0                                ; frac = - frac
          SUB           r4, r4, #30                               ; x -= L_INTERPOL2 - 1
          CMP           r2, #0
          ADDLT         r2, r2, #4                                ; frac += UP_SAMP
          SUBLT         r4, r4, #2                                ; x--

          LDR           r11, Lable1
          RSB           r2, r2, #3                                ; k = UP_SAMP - 1 - frac
          MOV           r8, #0                                    ; j = 0
          ADD           r11, r11, r2, LSL #1                      ; get inter4_2[] address

LOOP
          MOV           r12, r4                                   ; copy x[] address
          MOV           r9, #0                                    ; i = 0
          MOV           r14, #0                                   ; L_sum = 0L
          MOV           r10, r11

          ; used registers : r0 -- exc[], r2 -- k, r3 -- L_subfr, r4 -- x, r8 -- j
          ; r11 -- inter4_2[], r14 -- 0

LOOP1
          LDRSH         r1, [r12], #2                             ; x[i]
          LDRSH         r5, [r10], #8                             ; inter4_2[k]

          LDRSH         r6, [r12], #2                             ; x[i+1]
          LDRSH         r7, [r10], #8                             ; inter4_2[k + UP_SAMP]
          MLA           r14, r1, r5, r14

          LDRSH         r1, [r12], #2                             ; x[i+2]
          LDRSH         r5, [r10],#8                              ; inter4_2[]
          MLA           r14, r6, r7, r14

          LDRSH         r6, [r12], #2                             ; x[i+3]
          LDRSH         r7, [r10], #8
          MLA           r14, r1, r5, r14

          LDRSH         r1, [r12], #2                             ; x[i+4]
          LDRSH         r5, [r10], #8                       
          MLA           r14, r6, r7, r14

          LDRSH         r6, [r12], #2                             ; x[i+5]
          LDRSH         r7, [r10], #8
          MLA           r14, r1, r5, r14

          LDRSH         r1, [r12], #2                             ; x[i+6]
          LDRSH         r5, [r10], #8
          MLA           r14, r6, r7, r14                          
 
          LDRSH         r6, [r12], #2                             ; x[i+7]
          LDRSH         r7, [r10], #8
          MLA           r14, r1, r5, r14

          LDRSH         r1, [r12], #2                             ; x[i]
          LDRSH         r5, [r10], #8                             ; inter4_2[k]
          MLA           r14, r6, r7, r14

          LDRSH         r6, [r12], #2                             ; x[i+1]
          LDRSH         r7, [r10], #8                             ; inter4_2[k + UP_SAMP]
          MLA           r14, r1, r5, r14

          LDRSH         r1, [r12], #2                             ; x[i+2]
          LDRSH         r5, [r10],#8                              ; inter4_2[]
          MLA           r14, r6, r7, r14

          LDRSH         r6, [r12], #2                             ; x[i+3]
          LDRSH         r7, [r10], #8
          MLA           r14, r1, r5, r14

          LDRSH         r1, [r12], #2                             ; x[i+4]
          LDRSH         r5, [r10], #8                       
          MLA           r14, r6, r7, r14

          LDRSH         r6, [r12], #2                             ; x[i+5]
          LDRSH         r7, [r10], #8
          MLA           r14, r1, r5, r14

          LDRSH         r1, [r12], #2                             ; x[i+6]
          LDRSH         r5, [r10], #8
          MLA           r14, r6, r7, r14                          
 
          LDRSH         r6, [r12], #2                             ; x[i+7]
          LDRSH         r7, [r10], #8
          MLA           r14, r1, r5, r14
         
          ADD           r9, r9, #16                                ; i += 8
          MLA           r14, r6, r7, r14

          CMP           r9, #32                                   ; L_INTERPOL2 << 1
          BLT           LOOP1

          MOV           r1, r14, LSL #2                           ; L_sum = (L_sum << 2)
          ADD           r4, r4, #2                                ; x++
          ADD           r5, r1, #0x8000                           
          ADD           r8, r8, #1                                ; j++
          MOV           r1, r5, ASR #16
          CMP           r8, r3
          STRH          r1, [r0], #2                              ; exc[j] = (L_sum + 0x8000) >> 16
          BLT           LOOP
                    
pred_lt4_end
		     
          LDMFD   	r13!, {r4 - r12, r15} 
 
Lable1
          DCD   	inter4_2
          ENDFUNC
          END
