;**************************************************************
;* Copyright 2008 by VisualOn Corporation, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Corporation, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;void Cor_h_X(
;     Word16 h[],        /* (i) Q12 :Impulse response of filters      */
;     Word16 X[],        /* (i)     :Target vector                    */
;     Word16 D[]         /* (o)     :Correlations between h[] and D[] */
;                        /*          Normalized to 13 bits            */
;)
; r0 --- h[]         Impulse response of filters
; r1 --- X[]         Target vector 
; r2 --- D[]         Correlations between h[] and D[]

       AREA    |.text|, CODE, READONLY
       EXPORT Cor_h_X_asm

Cor_h_X_asm    PROC

       STMFD      r13!, {r4 - r12, r14}
       SUB        r13, r13, #168               ; y32[L_SUBFR]
       MOV        r8, #0                       ; max = 0
       MOV        r6, r13
       MOV        r9, #0                       ; i = 0
L1                                             ;for(i=0; i < L_SUBFR; i++)

       MOV        r10, #0                      ; s = 0
       MOV        r7, r9                       ; j = i
       MOV        r11, r9, LSL #1              
       ADD        r12, r1, r11                 ; &X[j]
       MOV        r14, r0   
           

L2                                             ;for(j=i; j < L_SUBFR; i++)   
       LDRSH      r3, [r12], #2
       LDRSH      r4, [r14],  #2
       ADD        r7, r7, #1
       MUL        r5, r3, r4
       ADD        r10, r10, r5, LSL #1        
      ; MLA        r10, r3, r4, r10             ; s +=(X[j] * h[j-i])<<1
       CMP        r7, #40
       BLT        L2
       STR        r10, [r6], #4                ; y32[i]= s
       CMP        r10, #0
       RSBLT      r10, r10, #0
       CMP        r10, r8
       MOVGT      r8, r10
       ADD        r9, r9, #1
       CMP        r9, #40
       BLT        L1
      
       MOV        r9, #0 
       CMP        r8, #0
       BEQ        L5
       MOV        r9, #1 
L3                                             ; j = norm_l(max)
       MOV        r8, r8, LSL #1
       CMP        r8, #0x40000000
       ADDLT      r9, r9, #1
       BLT        L3
L5
     
       CMP        r9, #16
       MOVGT      r9, #16
       RSB        r9, r9, #18                   ; j = 18-j

       MOV        r8, #0                        ;i = 0
       MOV        r6, r13                       ; &y32[]                

L4
       LDR        r3, [r6], #4
       LDR        r4, [r6], #4
       LDR        r5, [r6], #4
       
       MOV        r3, r3, LSR r9     
       MOV        r4, r4, LSR r9
       STRH       r3, [r2], #2
       MOV        r5, r5, LSR r9      
       LDR        r7, [r6], #4
       STRH       r4, [r2], #2 
       STRH       r5, [r2], #2
       MOV        r7, r7, LSR r9
       ADD        r8, r8, #4
       STRH       r7, [r2], #2
       CMP        r8, #40
       BLT        L4
      
       ADD        r13, r13, #168
      
       LDMFD      r13!, {r4 - r12, r15}
       ENDP
       END
