;**************************************************************
;* Copyright 2003~2009 by VisualOn Corporation, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Corporation, Inc. ALL RIGHTS RESERVED.
;****************************************************************
; r0 --- a[]     LPC coefficient
; r1 --- gamma   Spectral expansion factor
; r2 --- m       LPC order
; r3 --- ap[]    Spectral expand LPC coefficient 

       AREA    |.text|, CODE, READONLY
       EXPORT Weight_Az_asm

Weight_Az_asm    PROC

       STMDB      sp!, {r4 - r12, lr}
       LDRSH      r5, [r0], #2      
       MOV        r4, r1               ; fac = gamma
       MOV        r8, #1
       MOV        r9, #0x8000
       STRH       r5, [r3], #2         ; ap[0] = a[0]

LOOP
       LDRSH      r6, [r0], #2         ; a[1]
       ADD        r8, r8, #1
       MUL        r7, r4, r6
       ADD        r10, r9, r7, LSL #1
       MOV        r7, r10, LSR #16
       STRH       r7, [r3], #2 
       MUL        r4, r4, r1
       ADD        r10, r9, r4, LSL #1
       MOV        r4, r10, LSR #16    
       CMP        r8, r2
       BLT        LOOP


       LDRSH      r6, [r0]               ; a[m]
       MUL        r7, r6, r4
       ADD        r10, r9, r7, LSL #1
       MOV        r7, r10, LSR #16 
       STRH       r7, [r3]                ; ap[m]       

       

       LDMIA      sp!, {r4 - r12, pc}
       ENDP
       END
