;**************************************************************
;* Copyright 2008 by Visualon Corporation, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Corporation, Inc. ALL RIGHTS RESERVED.
;****************************************************************     
;void Pred_lt_3(
;  Word16   exc[],       /* in/out: excitation buffer */
;  Word16   T0,          /* input : integer pitch lag */
;  Word16   frac,        /* input : fraction of lag   */
;  Word16   L_subfr      /* input : subframe size     */
;)
;
;*****************************************************************
; r0  --- exc[]
; r1  --- T0
; r2  --- frac
; r3  --- L_subfr

      AREA   |.text|, CODE, READONLY
      EXPORT Pred_lt_3_asm
      IMPORT inter_3l
      
Pred_lt_3_asm  PROC

      STMDB    sp!, {r4 - r12, lr}
      SUB      r3, r0, r1, LSL #1            ; get &exc[-T0] --- r3
      MOV      r8, #40            
      LDR      r4, Table                     ; get the table address
      ; if(frac <= 0)
      CMP      r2, #0
      BLE      NEG                           ;frac <= 0
      RSB      r2, r2, #3                    ;frac = UP_SAMP - frac 
      SUB      r3, r3, #2                    ;x0--
      RSB      r5, r2, #3                    ; frac2 = UP_SAMP - frac (r5)
      ADD      r6, r4, r2, LSL #1            ; c1 = &inter_3l[frac]
      ADD      r7, r4, r5, LSL #1            ; c2 = &inter_3l[frac2]
      B        LOOP
NEG   
      ADD      r5, r2, #3                    ; frac2 = UP_SAMP + frac
      RSB      r7, r2, #0
      ADD      r6, r4, r7, LSL #1            ; c1           
      ADD      r7, r4, r5, LSL #1            ; c2
 
LOOP
      MOV      r4, r3                         ; x1 = x0++
      ADD      r3, r3, #2
      MOV      r9,  #0                        ; s = 0
      MOV      r5, r3                         ; x2 = x0


      LDRSH      r1, [r6]                     ; c1[k]
      LDRSH      r2, [r4]                  ; x1[-i]
      LDRSH      r10, [r7]                  ; c2[k]
      LDRSH      r11, [r5]                  ; x2[i]
      MLA        r9, r1, r2, r9                    ; s+=(x1[-i] * c1[k]<<1)

      LDRSH      r2, [r4, #-2]                  ; x1[-i]
      LDRSH      r1, [r6, #6]                   ; c1[k]
      MLA        r9, r10, r11, r9                   ; s +=(x2[i] * c2[k]<<1)

 
      LDRSH      r11, [r5, #2]                  ; x2[i]
      LDRSH      r10, [r7, #6]                  ; c2[k]
      MLA        r9, r1, r2, r9                     ; s+=(x1[-i] * c1[k]<<1)

      LDRSH      r2, [r4, #-4]                  ; x1[-i]
      LDRSH      r1, [r6, #12]                   ; c1[k]
      MLA        r9, r10, r11, r9                   ; s +=(x2[i] * c2[k]<<1)


      LDRSH      r11, [r5, #4]                  ; x2[i]
      LDRSH      r10, [r7, #12]                  ; c2[k]
      MLA        r9, r1, r2, r9                     ; s+=(x1[-i] * c1[k]<<1)

      LDRSH      r2, [r4, #-6]                  ; x1[-i]
      LDRSH      r1, [r6, #18]                   ; c1[k]
      MLA        r9, r10, r11, r9                  ; s +=(x2[i] * c2[k]<<1)


      LDRSH      r11, [r5, #6]                  ; x2[i]
      LDRSH      r10, [r7, #18]                  ; c2[k]
      MLA        r9, r1, r2, r9                 ; s+=(x1[-i] * c1[k]<<1)

      LDRSH      r2, [r4, #-8]                  ; x1[-i]
      LDRSH      r1, [r6, #24]                   ; c1[k]
      MLA        r9, r10, r11, r9                   ; s +=(x2[i] * c2[k]<<1)


      LDRSH      r11, [r5, #8]                  ; x2[i]
      LDRSH      r10, [r7, #24]                  ; c2[k]
      MLA        r9, r1, r2, r9                     ; s+=(x1[-i] * c1[k]<<1)


      LDRSH      r1, [r6, #30]                   ; c1[k]
      MLA        r9, r10, r11, r9                   ; s +=(x2[i] * c2[k]<<1)
      LDRSH      r2, [r4, #-10]                  ; x1[-i]
      LDRSH      r11, [r5, #10]                  ; x2[i]
      LDRSH      r10, [r7, #30]                  ; c2[k]
      MLA        r9, r1, r2, r9                     ; s+=(x1[-i] * c1[k]<<1)

      LDRSH      r2, [r4, #-12]                  ; x1[-i]
      LDRSH      r1, [r6, #36]                   ; c1[k]
      MLA        r9, r10, r11, r9                   ; s +=(x2[i] * c2[k]<<1)
 

      LDRSH      r11, [r5, #12]                  ; x2[i]
      LDRSH      r10, [r7, #36]                  ; c2[k]
      MLA        r9, r1, r2, r9             	      ; s+=(x1[-i] * c1[k]<<1)

      LDRSH      r2, [r4, #-14]                  ; x1[-i]
      LDRSH      r1, [r6, #42]                   ; c1[k]
      MLA        r9, r10, r11, r9                   ; s +=(x2[i] * c2[k]<<1)


      LDRSH      r11, [r5, #14]                  ; x2[i]
      LDRSH      r10, [r7, #42]                  ; c2[k]
      MLA        r9, r1, r2, r9                     ; s+=(x1[-i] * c1[k]<<1)

      LDRSH      r2, [r4, #-16]                  ; x1[-i]
      LDRSH      r1, [r6, #48]                   ; c1[k]
      MLA        r9, r10, r11, r9                   ; s +=(x2[i] * c2[k]<<1)
  


      LDRSH      r10, [r7, #48]                  ; c2[k]
      MLA        r9, r1, r2, r9                     ; s+=(x1[-i] * c1[k]<<1)

      LDRSH      r11, [r5, #16]                  ; x2[i]
      LDRSH      r1, [r6, #54]                   ; c1[k]
      LDRSH      r2, [r4, #-18]                  ; x1[-i]
      MLA        r9, r10, r11, r9                   ; s +=(x2[i] * c2[k]<<1)



      LDRSH      r10, [r7, #54]                  ; c2[k]


      LDRSH      r11, [r5, #18]                  ; x2[i]
      
      SUBS     r8, r8, #1
      MLA        r9, r1, r2, r9                     ; s+=(x1[-i] * c1[k]<<1)
      MLA        r9, r10, r11, r9                   ; s +=(x2[i] * c2[k]<<1)
   
      MOV      r9, r9, LSL #1
      ADD      r9, r9, #0x00008000
      MOV      r9, r9, ASR #16
      STRH     r9, [r0], #2

      BGT      LOOP
      LDMIA    sp!, {r4 - r12, pc}   
      ENDP

Table
      DCD      inter_3l
      
      END
