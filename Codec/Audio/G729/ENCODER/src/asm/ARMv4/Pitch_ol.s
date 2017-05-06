;**************************************************************
;* Copyright 2003~2009 by VisualOn Corporation, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Corporation, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;Word16 Pitch_ol_fast(  /* output: open loop pitch lag                        */
;   Word16 signal[],    /* input : signal used to compute the open loop pitch */
;                       /*     signal[-pit_max] to signal[-1] should be known */
;   Word16   pit_max,   /* input : maximum pitch lag                          */
;   Word16   L_frame    /* input : length of frame to compute pitch           */
;)
; r0 --- signal[]     Input signal
; r1 --- pit_max      input : maximum pitch lag
; r2 --- L_frame      input : length of frame to compute pitch 


       AREA    |.text|, CODE, READONLY       
       EXPORT Pitch_ol_fast_asm
       IMPORT Inv_sqrt_asm 

Pitch_ol_fast_asm    PROC

       STMFD      r13!, {r4 - r12, r14}
       SUB        r13, r13, #488                 ;scaled_signal[L_FRAME+PIT_MAX];
       MOV        r10, r0                        ;store the input address
       RSB        r8, r1, #0                     ;i = - pit_max 
       ADD        r6, r13, r1, LSL #1            ;scale_sig address
       ADD        r11, r10, r8, LSL #1           ;get the signal[-pit_max] address
       MOV        r9, r11                        ;hold on signal[-pit_max] address
       MOV        r12, #0                        ;sum = 0

LOOP1
       LDRSH      r3, [r9], #4                   ;signal[i]
       ADD        r8, r8, #2

       MUL        r4, r3, r3                     ;signal[i] * signal[i]
       ADDS       r12, r12, r4, LSL #1           ;sum = L_add(sum, L_temp)
       BVS        Label1                         ;Overflow = 1
       CMP        r8, r2
       BLT        LOOP1   
       BL         Label2                         ;Overflow = 0
                        
Label1
       MOV        r9, r11
       MOV        r7, r6
       RSB        r8, r1, #0
       ADD        r7, r7, r8, LSL #1             ; scal_sig[-pit_max]
LOOP2
       LDRSH      r3, [r9], #2                   ; signal[-pit_max]
       ADD        r8, r8, #1
       MOV        r4, r3, ASR #3
       STRH       r4, [r7], #2
       CMP        r8, r2
       BLT        LOOP2
       BL         Label3
                  
Label2
       CMP        r12, #0x100000
       BLT        LOOP3
       BL         LOOP5

LOOP3
       MOV        r9, r11
       MOV        r7, r6
       RSB        r8, r1, #0
       ADD        r7, r7, r8, LSL #1             ; scal_sig[-pit_max]   
LOOP4
       LDRSH      r3, [r9], #2                   ; signal[-pit_max]
       ADD        r8, r8, #1
       MOV        r4, r3, ASL #3
       STRH       r4, [r7], #2
       CMP        r8, r2
       BLT        LOOP4
       BL         Label3    

LOOP5
       MOV        r9, r11
       MOV        r7, r6
       RSB        r8, r1, #0
       ADD        r7, r7, r8, LSL #1             ; scal_sig[-pit_max]   
LOOP6
       LDRSH      r3, [r9], #2                   ; signal[-pit_max]
       ADD        r8, r8, #1
       STRH       r3, [r7], #2
       CMP        r8, r2
       BLT        LOOP6     
 

Label3                                           ; start three time section
      ; r6 --- scal_sig address
       MOV        r4, #0x80000000                ; max = MIN_32
       MOV        r3, #20                        ; T = 20
       MOV        r8, #20                        ; i = 20
LOOP7                                            ; for(i=20; i<40; i++)
       MOV        r9, r6                         ; r9---p = scal_sig
       RSB        r10, r8, #0                    ; -i
       ADD        r10, r9, r10, LSL #1           ; r10---p1 = scal_sig[-i]
       MOV        r12, #0                        ; sum = 0
       MOV        r14, #0                        ; j = 0

LOOP7_1                            ; for(j=0; j<L_frame; j+=2, p+=2, p1+=2)
    
       LDRSH      r5,  [r9],  #4
       LDRSH      r11, [r10], #4
       LDRSH      r0, [r9], #4
       LDRSH      r2, [r10], #4
     
       ADD        r14, r14, #4 
       MLA        r12, r5, r11, r12
       MLA        r12, r0, r2, r12
       ;LDRSH      r5,  [r9],  #4
       ;LDRSH      r11, [r10], #4
       ;LDRSH      r0, [r9], #4
       ;LDRSH      r2, [r10], #4

       ;MLA        r12, r5, r11, r12
       ;MLA        r12, r0, r2, r12
       CMP        r14, #80
       BLT        LOOP7_1

       MOV        r12, r12, LSL #1
       CMP        r12, r4
       BLE        L12
       MOV        r4, r12                        ; max = sum
       MOV        r3, r8                         ; T1 = i

L12
       ADD        r8, r8, #1
       CMP        r8, #40
       BLT        LOOP7


      ; r3--- T1   r4 --- max
   
       MOV        r12, #0                        ; sum = 0
       MOV        r9, r6                         ; r9 --- scal_sig
       RSB        r10, r3, #0                    ; -T1
       ADD        r10, r9, r10, LSL #1           ; r10 --- &scal_sig[-T1]
       MOV        r14, #0                        ; i = 0

LOOP8
       LDRSH      r5,  [r10], #4
       LDRSH      r2,  [r10], #4
       ADD        r14, r14, #4
       MLA        r12, r5, r5, r12
       MLA        r12, r2, r2, r12
       CMP        r14, #80
       BLT        LOOP8
 
       MOV        r12, r12, LSL #1                ; sum = (sum <<1) + 1         
       ADD        r12, r12, #1  

       MOV        r0, r12
       BL         Inv_sqrt_asm                    ; sum = Inv_sqrt_asm(sum)

       ; need hold on: r3, r4, r12, r6
       MOV        r5, r4, ASR #16                 ; max_h = max >>16
       SUB        r8, r4, r5, LSL #16             ; max - (max_h <<16)
       MOV        r8, r8, ASR #1                  ; max_l = (max - (max_h <<16))>>1

       MOV        r7, r0, ASR #16                 ; ener_h = sum >>16
       SUB        r9, r0, r7, LSL #16            ; ener_l =(sum - (ener_h <<16))>>1
       MOV        r9, r9, ASR #1

       MUL        r12, r5, r7                     ; max_h * ener_h
       MUL        r11, r5, r9                
       ADD        r12, r12, r11, ASR #15
       MUL        r11, r8, r7
       ADD        r12, r12, r11, ASR #15
       MOV        r12, r12, LSL #1                ; r12--- sum

       LDR        r9, TAB1 
       AND        r4, r12, r9                     ; max1 = (Word16)sum

       ; need hold on : r3 --- T1; r4 ---- max1;  r6 --- scal_sig

       MOV        r5, #40                         ; T2 = 40
       MOV        r7, #40                         ; i = 40
       MOV        r2, #0x80000000                 ; max = MIN_32

LOOP10
       MOV        r9, r6                          ; r9 --  p = scal_sig
       RSB        r10, r7, #0 
       ADD        r10, r9, r10, LSL #1            ; r10 -- p1 = &scal_sig[-i]
       MOV        r8, #0                          ; j = 0
       MOV        r14, #0                         ; sum = 0

LP10_2
       
       LDRSH      r11, [r9], #4
       LDRSH      r12, [r10],#4
       LDRSH      r0,  [r9], #4
       LDRSH      r5,  [r10],#4
       ADD        r8, r8, #4
       MLA        r14, r11, r12, r14
       MLA        r14, r0, r5, r14
       CMP        r8, #80
       BLT        LP10_2

       MOV        r14, r14, LSL #1                ; sum <<=1

       CMP        r14, r2
       BLE        LP11
       MOV        r2, r14                         ; max = sum
       MOV        r1, r7                          ; T2 = i

LP11
       ADD        r7, r7, #1
       CMP        r7, #80
       BLT        LOOP10

       ; need hold on register: r3 --- T1; r4 ---- max1;  r6 --- scal_sig
       ; need hold on register: r2 --- max; r1 --- T2

       MOV        r0, #0                          ; sum = 0
       RSB        r5, r1, #0                      ; r5 --- -T2
       ADD        r5, r6, r5, LSL #1              ; r5 --- p = &scal_sig[-T2]
       MOV        r8, #0                          ; r8 -- i = 0

LP12
       LDRSH      r9, [r5], #4
       LDRSH      r10, [r5], #4

       ADD        r8, r8, #4
       MLA        r0, r9, r9, r0
       MLA        r0, r10, r10, r0
       CMP        r8, #80
       BLT        LP12
 
       MOV        r0, r0, LSL #1
       ADD        r0, r0, #1                      ; sum = (sum <<1) + 1

       BL         Inv_sqrt_asm

       MOV        r5, r2, ASR #16                 ; max_h = max >>16
       MOV        r7, r5, LSL #16
       SUB        r7, r2, r7
       MOV        r7, r7, ASR #1                  ; max_l = (max - (max_h<<16))>>1

       MOV        r8, r0, ASR #16                 ; ener_h = sum >>16
       MOV        r9, r8, LSL #16
       SUB        r9, r0, r9
       MOV        r9, r9, ASR #1                  ; ener_l = (sum - (ener_h<<16))>>1

       MUL        r10, r5, r8
       MUL        r11, r5, r9
       ADD        r0, r10, r11, ASR #15
       MUL        r10, r7, r8
       ADD        r0, r0, r10, ASR #15
       MOV        r0, r0, LSL #1                  ; sum <<=1
      
       LDR        r9, TAB1 
       AND        r2, r0, r9                     ; max2 = (Word16)(sum)

       ; need hold on register: r3 --- T1; r4 ---- max1;  r6 --- scal_sig
       ; need hold on register: r2 --- max2;  r1 --- T2

       MOV        r5, #0x80000000                  ; max = MIN_32
       MOV        r7, #80                          ; T3 = 80
       MOV        r8, #80                          ; i = 80

LPT3
       MOV        r9, r6                           ; r9 -- p = scal_sig
       RSB        r10, r8, #0                   
       ADD        r10, r6, r10, LSL #1             ; r10 -- p1 = &scal_sig[-i]
       MOV        r12, #0                          ; sum = 0
       MOV        r14, #0                          ; j = 0

LPT3_1
        
       LDRSH      r0,  [r9],  #4
       LDRSH      r11, [r10], #4
       ADD        r14, r14, #2
       MLA        r12, r0, r11, r12
       CMP        r14, #80
       BLT        LPT3_1

       MOV        r12, r12, LSL #1                  ; sum <<=1
  
       CMP        r12, r5
       BLE        LT3
       MOV        r5, r12                           ; max = sum_asm 
       MOV        r7, r8                            ; T3 = i

LT3
       ADD        r8, r8, #2
       CMP        r8, #143
       BLT        LPT3

       ; need hold on register: r3 --- T1; r4 ---- max1;  r6 --- scal_sig
       ; need hold on register: r2 --- max2;  r1 --- T2; r7 -- T3; r5 --- max_asm 

       ;MOV        r8, r7                            ; i = T3
       ;MOV        r9, r6                            ; r9 -- p = scal_sig
       ;ADD        r10, r8, #1                       ; i + 1
       ;RSB        r10, r10, #0
       ;ADD        r10, r6, r10, LSL #1              ; r10 -- &scal_sig[-(i+1)]
       ;MOV        r12, #0                           ; sum = 0
       ;MOV        r14, #0                           ; j =0

;LT4
       ;LDRSH      r0,  [r9], #4
       ;LDRSH      r11, [r10],#4
       ;MLA        r12, r0, r11, r12
       ;ADD        r14, r14,#2
       ;CMP        r14, #80
       ;BLT        LT4

       ;MOV        r12, r12, LSL #1
  
       ;CMP        r12, r5                            ; if(sum > max)
       ;BLE        LT5
       ;MOV        r5, r12                            ; max = sum 
       ;ADD        r7, r7, #1                         ; T3 = i + 1
    

       MOV         r12, #0                            ; sum = 0
       RSB         r10, r7, #0                        ; -T3
       ADD         r10, r6, r10, LSL #1               ; r10 -- &scal_sig[-T3]
       MOV         r14, #0                            ; i = 0
LT5
       LDRSH       r0, [r10], #4
       LDRSH       r9, [r10], #4
       ADD         r14, r14, #4   
       MLA         r12, r0, r0, r12
       MLA         r12, r9, r9, r12
       CMP         r14, #80
       BLT         LT5
_asm 
       MOV         r12, r12, LSL #1
       ADD         r12, r12, #1                       ; sum = (sum <<1) + 1;

       MOV         r0, r12
       BL          Inv_sqrt_asm                       ; sum = Inv_sqrt_asm(sum)

       
       ; need hold on register: r3 --- T1; r4 ---- max1;  r6 --- scal_sig; r0 -- sum
       ; need hold on register: r2 --- max2;  r1 --- T2; r7 -- T3; r5 --- max       
    
       MOV         r8, r5, ASR #16                    ; r8 -- max_h = max >>16
       MOV         r9, r8, LSL #16
       SUB         r9, r5, r9
       MOV         r9, r9, ASR #1                     ; r9 --- max_l 

       MOV         r10, r0, ASR #16                   ; r10 --- ener_h = sum >>16
       MOV         r11, r10, LSL #16
       SUB         r11, r0, r11
       MOV         r11, r11, ASR #1                   ; r11 -- ener_l

       MUL         r0, r8, r10
       MUL         r12, r8, r11
       ADD         r0, r0, r12, ASR #15
       MUL         r12, r9, r10
       ADD         r0, r0, r12, ASR #15
       MOV         r0, r0, LSL #1                     ; sum <<=1
       
       LDR         r9, TAB1 
       AND         r8, r0, r9                        ; max3 = (Word16)sum


       ; need hold on register: r3 --- T1; r4 ---- max1;  r6 --- scal_sig; r0 -- sum
       ; need hold on register: r2 --- max2;  r1 --- T2; r7 -- T3; r8 --- max3 

       RSB         r5, r7, r1, LSL #1                 ; i = (T2<<1) - T3
       CMP         r5, #0
       RSBLT       r9, r5, #0
       MOVGE       r9, r5                             ; j = (i<0)?-i:i

       CMP         r9, #5
       ADDLT       r2, r2, r8, ASR #2                 ; max2 += (max3 >>2);

       ADD         r5, r5, r1                        ; i +=T2
        
       CMP         r5, #0
       RSBLT       r9, r5, #0
       MOVGE       r9, r5                             ; j = (i<0)?-i:i
  
       CMP         r9, #7
       ADDLT       r2, r2, r8, ASR #2                 ; max2 += (max3 >>2);

       RSB         r5, r1, r3, LSL #1                 ; i = (T1 <<1) - T2
       CMP         r5, #0
       RSBLT       r9, r5, #0
       MOVGE       r9, r5                             ; j = (i<0)?-i:i

       LDR         r0, TAB                            ; r0 = 6554  
       CMP         r9, #5
       BGE         LE1
       MOV         r14, r2, LSL #16
       MOV         r2, r14, ASR #16
       MUL         r10, r2, r0 
       ADD         r4, r4, r10, ASR #15

LE1

       ADD         r5, r5, r3                         ; i +=T1
       CMP         r5, #0
       RSBLT       r9, r5, #0
       MOVGE       r9, r5                             ; j = (i<0)?-i:i
       CMP         r9, #7
       BGE         LE2
       MOV         r14, r2, LSL #16
       MOV         r2, r14, ASR #16
       MUL         r10, r2, r0 
       ADD         r4, r4, r10, LSR #15

LE2
       MOV         r9, r4, LSL #16
       MOV         r4, r9, ASR #16
       MOV         r9, r2, LSL #16
       MOV         r2, r9, ASR #16
       MOV         r9, r8, LSL #16
       MOV         r8, r9, ASR #16

       CMP         r4, r2
       MOVLT       r4, r2
       MOVLT       r3, r1

       CMP         r4, r8
       MOVLT       r3, r7

       MOV         r0, r3                             ; return T1

                     
       ADD        r13, r13, #488
       LDMFD      r13!, {r4 - r12, r15}
       ENDP

TAB 
       DCD        0x199a
TAB1
       DCD        0xffff

       END
