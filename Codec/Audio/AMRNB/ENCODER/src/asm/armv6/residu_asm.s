;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;void Residu (
;	     Word16 a[], /* (i)     : prediction coefficients */
;	     Word16 x[], /* (i)     : speech signal           */
;	     Word16 y[], /* (o)     : residual signal         */
;	     Word16 lg   /* (i)     : size of filtering       */
;	     )

        AREA     |.text|, CODE, READONLY
	EXPORT   Vo_Residu_asm

Vo_Residu_asm PROC

        STMFD      r13!, {r4 - r12, r14}
        MOV        r3, #40

	LDRSH      r4, [r0], #2                          ;r4 --- a0
	LDRSH      r6, [r0], #2                          ;a1
	LDRSH      r7, [r0], #2                          ;a2
	LDRSH      r8, [r0], #2                          ;a3
	LDRSH      r9, [r0], #2                          ;a4
	PKHBT      r5, r6, r7, LSL #16                   ;r5 --- a2, a1
	LDRSH      r7, [r0], #2                          ;a5
	PKHBT      r6, r8, r9, LSL #16                   ;r6 --- a4, a3
	LDRSH      r8, [r0], #2                          ;a6
	LDRSH      r9, [r0], #2                          ;a7
	LDRSH      r10, [r0], #2                         ;a8
	PKHBT      r7, r7, r8, LSL #16                   ;r7 --- a6, a5
	PKHBT      r8, r9, r10, LSL #16                  ;r8 --- a8, a7
	LDRSH      r9, [r0], #2                          ;a9
	LDRSH      r10, [r0], #2                         ;a10
	PKHBT      r9, r9, r10, LSL #16                  ;r9 --- a10, a9

        ;r0, r10, r11, r12, r14	

LOOP

        MOV        r0, r1
	LDRSH      r10, [r0], #-2                        ;x[0]
	LDRSH      r11, [r0], #-2                        ;x[-1]
	MUL        r14, r4, r10
	LDRSH      r12, [r0], #-2                        ;x[-2]
	LDRSH      r10, [r0], #-2                        ;x[-3]
	PKHBT      r11, r11, r12, LSL #16                ;r11 --- x[-2] x[-1]
	LDRSH      r12, [r0], #-2                        ;x[-4]
	SMLAD      r14, r11, r5, r14
	LDRSH      r11, [r0], #-2                        ;x[-5]
	PKHBT      r10, r10, r12, LSL #16                ;r10 --- x[-4] x[-3]
	LDRSH      r12, [r0], #-2                        ;x[-6]
	SMLAD      r14, r10, r6, r14
	LDRSH      r10, [r0], #-2                        ;x[-7]
	PKHBT      r11, r11, r12, LSL #16                ;r11 --- x[-6] x[-5]
	LDRSH      r12, [r0], #-2                        ;x[-8]
	SMLAD      r14, r11, r7, r14
	LDRSH      r11, [r0], #-2                        ;x[-9]
	PKHBT      r10, r10, r12, LSL #16                ;r10 --- x[-8] x[-7]
	LDRSH      r12, [r0], #-2                        ;x[-10]
	SMLAD      r14, r10, r8, r14
	ADD        r1, r1, #2
        PKHBT      r11, r11, r12, LSL #16                ;r11 --- x[-10] x[-9]	
        SMLAD      r14, r11, r9, r14

        SSAT       r14, #32, r14, LSL #4 
        MOV        r10, #0x8000
        QADD       r11, r14, r10
        SUBS       r3, r3, #1        
        MOV        r11, r11, ASR #16
	
        STRH       r11, [r2], #2                          ;y[i] = round(s)
   
	BGT        LOOP

        LDMFD      r13!, {r4 - r12, r15}
        ENDP
        END 


     
