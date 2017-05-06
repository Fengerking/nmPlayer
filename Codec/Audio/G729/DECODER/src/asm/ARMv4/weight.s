;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
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
       STRH       r5, [r3], #2         ; ap[0] = a[0]

LOOP
       LDRSH      r6, [r0], #2         ; a[1]
       ADD        r8, r8, #1
       MUL        r7, r4, r6
       CMP        r7, #0x40000000    
       MOVLT      r7, r7, LSL #1
       MOVGE      r7, #0x7fffffff 
       ADD        r7, r7, #0x00008000
       MOV        r7, r7, ASR #16 
       MUL        r4, r4, r1
       CMP        r4, #0x40000000
       MOVLT      r4, r4, LSL #1
       MOVGE      r4, #0x7fffffff
       ADD        r4, r4, #0x00008000
       MOV        r4, r4, ASR #16 
       STRH       r7, [r3], #2
       
       CMP        r8, r2
       BLT        LOOP


       LDRSH      r6, [r0]               ; a[m]
       MUL        r7, r6, r4
       CMP        r7, #0x40000000
       MOVLT      r7, r7, LSL #1
       MOVGE      r7, #0x7fffffff 
       ADD        r7, r7, #0x00008000
       MOV        r7, r7, ASR #16  
       STRH       r7, [r3]                ; ap[m]       

       

       LDMIA      sp!, {r4 - r12, pc}
       ENDP
       END
