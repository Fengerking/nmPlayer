;**************************************************************
;* Copyright 2003~2009 by Visualon Corporation, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Corporation, Inc. ALL RIGHTS RESERVED.
;****************************************************************
; r0  ---  a[]
; r1  ---  x[]
; r2  ---  y[]

       AREA    |.text|, CODE, READONLY
       EXPORT  Residu_asm
       
Residu_asm   PROC

       STMDB    sp!, {r4 - r12, lr}
       MOV      r8, #40   
            
LOOP
      
       LDRSH    r4, [r0]               ; a[0]
       LDRSH    r5, [r1]               ; x[i]
       LDRSH    r9,  [r0, #2]          ; a[1]
       LDRSH    r10, [r1, #-2]         ; x[i-1]
       MUL      r3, r4, r5             ; s = x[i] *a[0]
       LDRSH    r6, [r0, #4]           ; a[2]
       LDRSH    r7, [r1, #-4]          ; x[i-2]
       MLA      r3, r9, r10, r3        ; s += a[1] * x[i-1]
       LDRSH    r4, [r0, #6]           ; a[3]
       LDRSH    r5, [r1, #-6]          ; x[i-3]
       MLA      r3, r6, r7, r3         ; s += a[2] * x[i-2]
       LDRSH    r9, [r0, #8]           ; a[4]
       LDRSH    r10, [r1, #-8]         ; x[i-4]
       MLA      r3, r4, r5, r3         ; s += a[3] * x[i-3]
       LDRSH    r6, [r0, #10]          ; a[5]
       LDRSH    r7, [r1, #-10]         ; x[i-5]
       MLA      r3, r9, r10, r3        ; s += a[4] * x[i-4]
       LDRSH    r4, [r0, #12]          ; a[6]
       LDRSH    r5, [r1, #-12]         ; x[i-6]
       MLA      r3, r6, r7, r3         ; s += a[5] * x[i-5] 
       LDRSH    r9, [r0, #14]          ; a[7]
       LDRSH    r10, [r1, #-14]        ; x[i-7]
       MLA      r3, r4, r5, r3         ; s += a[6] * x[i-6]
       LDRSH    r6, [r0, #16]          ; a[8]
       LDRSH    r7, [r1, #-16]         ; x[i-8]
       MLA      r3, r9, r10, r3        ; s += a[7] * x[i-7]
       LDRSH    r4, [r0, #18]          ; a[9]
       LDRSH    r5, [r1, #-18]         ; x[i-9]
       MLA      r3, r6, r7, r3         ; s += a[8] * x[i-8]
       LDRSH    r9, [r0, #20]          ; a[10]
       LDRSH    r10, [r1, #-20]        ; x[i-10]
       MLA      r3, r4, r5, r3         ; s += a[9] * x[i-9]
       MLA      r3, r9, r10, r3        ; s += a[10] * x[i-10]

       MOV      r3, r3, LSL #4
       ADD      r3, r3, #0x00008000
       MOV      r3, r3, ASR #16
       ADD      r1, r1, #2  
       STRH     r3, [r2], #2
       SUBS     r8, r8, #1
       BGT      LOOP

       LDMIA    sp!, {r4 - r12, pc}
       ENDP

       END
