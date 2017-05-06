;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
; r0 --- x[]             input vector
; r1 --- y[]             output vector
; r2 --- L               vector length

       AREA    |.text|, CODE, READONLY
       EXPORT Copy_asm
       
Copy_asm    PROC
       STMDB      sp!, {r4 - r12, lr}
       AND        r8, r2, #7
       SUBS       r2, r2, r8
       BEQ        L2

L1
       LDMIA      r0!, {r3-r6}
       SUBS       r2, r2, #8
       STMIA      r1!, {r3-r6} 
       BNE        L1
L2
       CMP        r8, #0
       BEQ        L4

L3
       LDRSH      r9, [r0], #2
       SUBS       r8, r8, #1
       STRH       r9, [r1], #2
       BNE        L3
L4
       LDMIA      sp!, {r4 - r12, pc}
       ENDP
       END
