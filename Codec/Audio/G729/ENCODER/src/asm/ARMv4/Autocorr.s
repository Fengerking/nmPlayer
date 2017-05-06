;**************************************************************
;* Copyright 2003~2009 by VisualOn Corporation, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Corporation, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;void Autocorr(
;  Word16 x[],      /* (i)    : Input signal                      */
;  Word16 m,        /* (i)    : LPC order                         */
;  Word16 r_h[],    /* (o)    : Autocorrelations  (msb)           */
;  Word16 r_l[]     /* (o)    : Autocorrelations  (lsb)           */
;)
; r0 --- x[]         Input signal
; r1 --- m           LPC order
; r2 --- r_h[]       Autocorrelations  (msb) 
; r3 --- r_l[]       Autocorrelations  (lsb) 

       AREA    |.text|, CODE, READONLY     
       EXPORT Autocorr_asm 
       IMPORT hamwindow 


Autocorr_asm    PROC

       STMFD      r13!, {r4 - r12, r14}
       SUB        r13, r13, #488                 ;y[L_WINDOW]
       MOV        r6, r13                        ; store y[] ptr
       MOV        r8, #0                         ; i = 0
       LDR        r11, L_data                    ; hamwindow table address
       MOV        r12, #0x4000                   ; r12 = 16384
       
LOOP1      
       LDRSH      r4, [r0],  #2                  ; x[i]
       LDRSH      r5, [r11], #2                  ; hamwindow[i]
       LDRSH      r9, [r0],  #2
       LDRSH      r10,[r11], #2 
       MLA        r7, r4, r5, r12
       MOV        r7, r7, LSR #15
       STRH       r7, [r6], #2
       MLA        r7, r9, r10, r12
       MOV        r7, r7, LSR #15
       STRH       r7, [r6], #2

       LDRSH      r4, [r0],  #2                  ; x[i]
       LDRSH      r5, [r11], #2                  ; hamwindow[i]
       LDRSH      r9, [r0],  #2
       LDRSH      r10,[r11], #2 
       MLA        r7, r4, r5, r12
       MOV        r7, r7, LSR #15
       STRH       r7, [r6], #2
       MLA        r7, r9, r10, r12
       MOV        r7, r7, LSR #15
       STRH       r7, [r6], #2

       LDRSH      r4, [r0],  #2                  ; x[i]
       LDRSH      r5, [r11], #2                  ; hamwindow[i]
       LDRSH      r9, [r0],  #2
       LDRSH      r10,[r11], #2 
       MLA        r7, r4, r5, r12
       MOV        r7, r7, LSR #15
       STRH       r7, [r6], #2
       MLA        r7, r9, r10, r12
       MOV        r7, r7, LSR #15
       STRH       r7, [r6], #2

       LDRSH      r4, [r0],  #2                  ; x[i]
       LDRSH      r5, [r11], #2                  ; hamwindow[i]
       LDRSH      r9, [r0],  #2
       LDRSH      r10,[r11], #2 
       MLA        r7, r4, r5, r12
       MOV        r7, r7, LSR #15
       STRH       r7, [r6], #2
       MLA        r7, r9, r10, r12
       MOV        r7, r7, LSR #15
       STRH       r7, [r6], #2

       LDRSH      r4, [r0],  #2                  ; x[i]
       LDRSH      r5, [r11], #2                  ; hamwindow[i]
       LDRSH      r9, [r0],  #2
       LDRSH      r10,[r11], #2 
       MLA        r7, r4, r5, r12
       MOV        r7, r7, LSR #15
       STRH       r7, [r6], #2
       MLA        r7, r9, r10, r12
       MOV        r7, r7, LSR #15
       STRH       r7, [r6], #2

       ADD        r8, r8, #10
       CMP        r8, #240
       BLT        LOOP1


LOOP2
       MOV        r8, #1                         ; sum = 1   
       MOV        r7, #0                         ; i = 0
       MOV        r6, r13                        ; get y[0] address
       
LOOP3
       LDRSH      r4, [r6],#2                    ; y[i]
       MUL        r5, r4, r4
       MOV        r5, r5, LSL #1
       ADDS       r8, r8, r5
       BVS        Label
       ADD        r7, r7, #1
       CMP        r7, #240
       BLT        LOOP3                          ; LOOP3 can get sum
       BL         Label2

Label  
       MOV        r7, #0
       MOV        r6, r13
LOOP4
       LDRSH      r4, [r6]
       LDRSH      r5, [r6, #2]
       LDRSH      r8, [r6, #4]
       LDRSH      r9, [r6, #6]
       MOV        r4, r4, LSR #2
       MOV        r5, r5, LSR #2
       MOV        r8, r8, LSR #2
       MOV        r9, r9, LSR #2
       STRH       r4, [r6], #2
       STRH       r5, [r6], #2
       STRH       r8, [r6], #2
       STRH       r9, [r6], #2
       ADD        r7, r7, #4
       
       CMP        r7, #240
       BLT        LOOP4

       BL         LOOP2    
               
Label2                                          ; r8 --- sum

       MOV        r9, #0 
       MOV        r10, r8
L3                                              ; j = norm_l(max)
       ;MOV        r10, r10, LSL #1
       CMP        r10, #0x40000000
       ADDLT      r9, r9, #1
       MOVLT        r10, r10, LSL #1
       BLT        L3                            ; r9 =norm

       MOV        r8, r8, LSL r9                ; sum <<= norm
       ADD        r9, r9, #1                    ; norm + 1

       MOV        r10, r8, LSR #16
       STRH       r10, [r2], #2                 ; r_h[0]= sum >>16
       SUB        r11, r8, r10, LSL #16         ; sum - (r_h[0]<<16)
       MOV        r11, r11, LSR #1           
       STRH       r11, [r3], #2                 ; r_l[0] = (sum - (r_h[0]<<16))>>1


       MOV        r14, #240
       MOV        r7, #1                        ; i = 1
LOOP5                                           ; for(i=1; i <=m; i++)
       MOV        r8, #0                        ; sum = 0;
       MOV        r6, r13                       ; get the y[] address
       SUB        r10, r14, r7                  ; r10 --- L_WINDOW-i
       ADD        r12, r6, r7, LSL #1           ; get the y[i]
       MOV        r11, #0                       ; j = 0
   
LOOP6                                           ; for(j=0; j<L_WINDOW-i; j++)

       LDRSH      r4, [r6], #2                  ; y[j]
       LDRSH      r5, [r12], #2                 ; y[j+i]
       MLA        r8, r4, r5, r8                ; sum += (y[j] * y[j+i])
       ADD        r11, r11, #1
       CMP        r11, r10
       BLT        LOOP6

       MOV        r8, r8, LSL r9                ; sum <<=(norm +1)

       MOV        r4, r8, LSR #16               ; r_h[i] = sum >>16;
       STRH       r4, [r2], #2
       SUB        r5, r8, r4, LSL #16
       MOV        r5, r5, LSR #1
       STRH       r5, [r3], #2

       ADD        r7, r7, #1
       CMP        r7, #10
       BLE        LOOP5
                
       ADD        r13, r13, #488
       LDMFD      r13!, {r4 - r12, r15}
       ENDP

L_data

       DCD        hamwindow
       END
