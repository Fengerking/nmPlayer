;**************************************************************
;* Copyright 2003~2010 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;***************************************************************
;void Pred_lt4(
;		  Word16 exc[],                         /* in/out: excitation buffer */
;		  Word16 T0,                            /* input : integer pitch lag */
;		  Word16 frac,                          /* input : fraction of lag   */
;		  Word16 L_subfr                        /* input : subframe size     */
;	      )
;******************************
;       ARM Register
;******************************
; r0  ---  exc[]
; r1  ---  T0
; r2  ---  frac
; r3  ---  L_subfr

         AREA     |.text|, CODE, READONLY
	 EXPORT   pred_lt4_asm
	 IMPORT   voAMRWBEncinter4_2

pred_lt4_asm     FUNCTION
         STMFD     r13!, {r4 - r12, r14}
   
         RSB       r4, r1, #0                         ;-T0
         RSB       r2, r2, #0                         ;frac = -frac
         ADD       r5, r0, r4, LSL #1                 ;x = exc - T0
         CMP       r2, #0
         ADDLT     r2, r2, #4                         ;frac += UP_SAMP
         SUBLT     r5, r5, #2                         ;x--
         SUB       r5, r5, #30                        ;x -= 15
         RSB       r4, r2, #3                         ;k = 3 - frac
         LDR       r6, Table
	 MOV       r8, r4, LSL #6                    
         MOV       r1, r5
	 MOV       r14, #21
         ADD       r8, r6, r8                         ;ptr2 = &(inter4_2[k][0])

; used register
         ;r0 --- exc[]  r1 --- x  r7 --- j  r8 --- ptr2  r5 --- 0x8000
THREE_LOOP
         ;MOV       r1, r5                             ;ptr1 = x
	 MOV       r2, r8                             ;ptr = ptr2
	 LDRSH     r4, [r1], #2                       ;x[0]
	 MOV       r10, #0
	 LDRSH     r6, [r1], #2                       ;x[1]
         LDR       r3, [r2], #4                       ;h[0], h[1]
	 LDRSH     r9, [r1], #2                       ;x[2]

	 MOV       r11, #0
	 MOV       r12, #0

	 PKHBT     r5, r4, r6, LSL #16
         LDRSH     r4, [r1], #2                       ;x[3]
	 PKHBT     r7, r6, r9, LSL #16

	 SMLAD     r10, r5, r3, r10
	 SMLAD     r11, r7, r3, r11
	 PKHBT     r5, r9, r4, LSL #16
	 LDRSH     r6, [r1], #2                       ;x[4]
	 SMLAD     r12, r5, r3, r12

	 LDR       r3, [r2], #4                       ;h[2], h[3]
	 PKHBT     r5, r9, r4, LSL #16
         LDRSH     r9, [r1], #2                       ;x[5] 
	 PKHBT     r7, r4, r6, LSL #16
	 SMLAD     r10, r5, r3, r10
	 SMLAD     r11, r7, r3, r11
	 PKHBT     r5, r6, r9, LSL #16
         LDRSH     r4, [r1], #2                       ;x[6]
	 SMLAD     r12, r5, r3, r12

         LDR       r3, [r2], #4                       ;h[4], h[5]
	 PKHBT     r5, r6, r9, LSL #16
	 LDRSH     r6, [r1], #2                       ;x[7]
	 PKHBT     r7, r9, r4, LSL #16
	 SMLAD     r10, r5, r3, r10
	 SMLAD     r11, r7, r3, r11
	 PKHBT     r5, r4, r6, LSL #16
	 LDRSH     r9, [r1], #2                       ;x[8]
	 SMLAD     r12, r5, r3, r12


         LDR       r3, [r2], #4                       ;h[6], h[7]
	 PKHBT     r5, r4, r6, LSL #16
	 LDRSH     r4, [r1], #2                       ;x[9]
	 PKHBT     r7, r6, r9, LSL #16
	 SMLAD     r10, r5, r3, r10
	 SMLAD     r11, r7, r3, r11
	 PKHBT     r5, r9, r4, LSL #16
	 LDRSH     r6, [r1], #2                       ;x[10]
	 SMLAD     r12, r5, r3, r12


	 LDR       r3, [r2], #4                       ;h[8], h[9]
	 PKHBT     r5, r9, r4, LSL #16
	 LDRSH     r9, [r1], #2                       ;x[11]
	 PKHBT     r7, r4, r6, LSL #16
	 SMLAD     r10, r5, r3, r10
	 SMLAD     r11, r7, r3, r11
	 PKHBT     r5, r6, r9, LSL #16
	 LDRSH     r4, [r1], #2                       ;x[12]
	 SMLAD     r12, r5, r3, r12


         LDR       r3, [r2], #4                       ;h[10], h[11]
	 PKHBT     r5, r6, r9, LSL #16
	 LDRSH     r6, [r1], #2                       ;x[13]
	 PKHBT     r7, r9, r4, LSL #16
	 SMLAD     r10, r5, r3, r10
	 SMLAD     r11, r7, r3, r11
	 PKHBT     r5, r4, r6, LSL #16
	 LDRSH     r9, [r1], #2                       ;x[14]
	 SMLAD     r12, r5, r3, r12


	 LDR       r3, [r2], #4                       ;h[12], h[13]
	 PKHBT     r5, r4, r6, LSL #16
	 LDRSH     r4, [r1], #2                       ;x[15]
	 PKHBT     r7, r6, r9, LSL #16
	 SMLAD     r10, r5, r3, r10
	 SMLAD     r11, r7, r3, r11
	 PKHBT     r5, r9, r4, LSL #16
	 LDRSH     r6, [r1], #2                       ;x[16]
	 SMLAD     r12, r5, r3, r12


	 LDR       r3, [r2], #4                       ;h[14], h[15]
	 PKHBT     r5, r9, r4, LSL #16
	 LDRSH     r9, [r1], #2                       ;x[17]
	 PKHBT     r7, r4, r6, LSL #16
	 SMLAD     r10, r5, r3, r10
	 SMLAD     r11, r7, r3, r11
	 PKHBT     r5, r6, r9, LSL #16
	 LDRSH     r4, [r1], #2                       ;x[18]
	 SMLAD     r12, r5, r3, r12


	 LDR       r3, [r2], #4                       ;h[16], h[17]
	 PKHBT     r5, r6, r9, LSL #16
         LDRSH     r6, [r1], #2                       ;x[19]
	 PKHBT     r7, r9, r4, LSL #16
	 SMLAD     r10, r5, r3, r10
	 SMLAD     r11, r7, r3, r11
	 PKHBT     r5, r4, r6, LSL #16
         LDRSH     r9, [r1], #2                       ;x[20]
	 SMLAD     r12, r5, r3, r12


	 LDR       r3, [r2], #4                       ;h[18], h[19]
	 PKHBT     r5, r4, r6, LSL #16
	 LDRSH     r4, [r1], #2                       ;x[21]
	 PKHBT     r7, r6, r9, LSL #16
	 SMLAD     r10, r5, r3, r10
	 SMLAD     r11, r7, r3, r11
	 PKHBT     r5, r9, r4, LSL #16
	 LDRSH     r6, [r1], #2                       ;x[22]
	 SMLAD     r12, r5, r3, r12


	 LDR       r3, [r2], #4                       ;h[20], h[21]
	 PKHBT     r5, r9, r4, LSL #16
	 LDRSH     r9, [r1], #2                       ;x[23]
	 PKHBT     r7, r4, r6, LSL #16
	 SMLAD     r10, r5, r3, r10
	 SMLAD     r11, r7, r3, r11
	 PKHBT     r5, r6, r9, LSL #16
	 LDRSH     r4, [r1], #2                       ;x[24]
	 SMLAD     r12, r5, r3, r12


	 LDR       r3, [r2], #4                       ;h[22], h[23]
	 PKHBT     r5, r6, r9, LSL #16
         LDRSH     r6, [r1], #2                       ;x[25]
	 PKHBT     r7, r9, r4, LSL #16
	 SMLAD     r10, r5, r3, r10
	 SMLAD     r11, r7, r3, r11
	 PKHBT     r5, r4, r6, LSL #16
         LDRSH     r9, [r1], #2                       ;x[26]	 
	 SMLAD     r12, r5, r3, r12



	 LDR       r3, [r2], #4                       ;h[24], h[25]
	 PKHBT     r5, r4, r6, LSL #16
	 LDRSH     r4, [r1], #2                       ;x[27]
	 PKHBT     r7, r6, r9, LSL #16
	 SMLAD     r10, r5, r3, r10
	 SMLAD     r11, r7, r3, r11
	 PKHBT     r5, r9, r4, LSL #16
	 LDRSH     r6, [r1], #2                       ;x[28]
	 SMLAD     r12, r5, r3, r12


	 LDR       r3, [r2], #4                       ;h[26], h[27]
	 PKHBT     r5, r9, r4, LSL #16
	 LDRSH     r9, [r1], #2                       ;x[29]
	 PKHBT     r7, r4, r6, LSL #16
	 SMLAD     r10, r5, r3, r10
	 SMLAD     r11, r7, r3, r11
	 PKHBT     r5, r6, r9, LSL #16
	 LDRSH     r4, [r1], #2                       ;x[30]
	 SMLAD     r12, r5, r3, r12


	 LDR       r3, [r2], #4                       ;h[28], h[29]
	 PKHBT     r5, r6, r9, LSL #16
         LDRSH     r6, [r1], #2                       ;x[31]
	 PKHBT     r7, r9, r4, LSL #16
	 SMLAD     r10, r5, r3, r10
	 SMLAD     r11, r7, r3, r11
	 PKHBT     r5, r4, r6, LSL #16
         LDRSH     r9, [r1], #2                       ;x[32]
	 SMLAD     r12, r5, r3, r12


	 LDR       r3, [r2], #4                       ;h[30], h[31]
	 PKHBT     r5, r4, r6, LSL #16
	 LDRSH     r4, [r1], #-60                     ;x[33]
	 PKHBT     r7, r6, r9, LSL #16
	 SMLAD     r10, r5, r3, r10
	 SMLAD     r11, r7, r3, r11
	 PKHBT     r9, r9, r4, LSL #16
	 MOV       r10, r10, LSL #1

	 SMLAD     r12, r9, r3, r12

	 ;SSAT      r10, #32, r10, LSL #2
	 ;SSAT      r11, #32, r11, LSL #2
	 ;SSAT      r12, #32, r12, LSL #2

	 MOV       r11, r11, LSL #1
	 MOV       r12, r12, LSL #1
	 QADD      r10, r10, r10
	 MOV       r5, #0x8000
	 QADD      r11, r11, r11
	 SUBS      r14, r14, #1
	 QADD      r12, r12, r12

	 QADD      r10, r10, r5
	 QADD      r11, r11, r5
	 MOV       r10, r10, ASR #16
	 QADD      r12, r12, r5


	 MOV       r11, r11, ASR #16
	 MOV       r12, r12, ASR #16

	 STRH      r10, [r0], #2
	 STRH      r11, [r0], #2
	 STRH      r12, [r0], #2
	 BNE       THREE_LOOP

	 MOV       r2, r8                             ;ptr = ptr2

Last2LOOP

	 LDRSH     r4, [r1], #2                       ;x[0]
         LDR       r3, [r2], #4                       ;h[0], h[1]
	 LDRSH     r6, [r1], #2                       ;x[1]
	 MOV       r10, #0
	 LDRSH     r9, [r1], #2                       ;x[2]

	 MOV       r11, #0

	 PKHBT     r7, r4, r6, LSL #16
	 LDRSH     r4, [r1], #2                       ;x[3]
	 PKHBT     r6, r6, r9, LSL #16
	 SMLAD     r10, r7, r3, r10
	 SMLAD     r11, r6, r3, r11

         LDRSH     r6, [r1], #2                       ;x[4]
	 LDR       r3, [r2], #4                       ;h[2], h[3]
	 PKHBT     r7, r9, r4, LSL #16
	 LDRSH     r9, [r1], #2                       ;x[5]
	 PKHBT     r4, r4, r6, LSL #16
	 SMLAD     r10, r7, r3, r10
	 SMLAD     r11, r4, r3, r11 

	 LDRSH     r4, [r1], #2                       ;x[6]
	 LDR       r3, [r2], #4                       ;h[4], h[5]
	 PKHBT     r7, r6, r9, LSL #16
	 LDRSH     r6, [r1], #2                       ;x[7]
	 PKHBT     r9, r9, r4, LSL #16
	 SMLAD     r10, r7, r3, r10
	 SMLAD     r11, r9, r3, r11

	 LDRSH     r9, [r1], #2                       ;x[8]
	 LDR       r3, [r2], #4                       ;h[6], h[7]
	 PKHBT     r7, r4, r6, LSL #16
	 LDRSH     r4, [r1], #2                       ;x[9]
	 PKHBT     r6, r6, r9, LSL #16
	 SMLAD     r10, r7, r3, r10
	 SMLAD     r11, r6, r3, r11

	 LDRSH     r6, [r1], #2                       ;x[10]
	 LDR       r3, [r2], #4                       ;h[8], h[9]
	 PKHBT     r7, r9, r4, LSL #16
	 LDRSH     r9, [r1], #2                       ;x[11]
	 PKHBT     r4, r4, r6, LSL #16
	 SMLAD     r10, r7, r3, r10
	 SMLAD     r11, r4, r3, r11

	 LDRSH     r4, [r1], #2                       ;x[12]
	 LDR       r3, [r2], #4                       ;h[10], h[11]
	 PKHBT     r7, r6, r9, LSL #16
	 LDRSH     r6, [r1], #2                       ;x[13]
	 PKHBT     r9, r9, r4, LSL #16
	 SMLAD     r10, r7, r3, r10
	 SMLAD     r11, r9, r3, r11

	 LDRSH     r9, [r1], #2                       ;x[14]
	 LDR       r3, [r2], #4                       ;h[12], h[13]
	 PKHBT     r7, r4, r6, LSL #16
	 LDRSH     r4, [r1], #2                       ;x[15]
	 PKHBT     r6, r6, r9, LSL #16
	 SMLAD     r10, r7, r3, r10
	 SMLAD     r11, r6, r3, r11

	 LDRSH     r6, [r1], #2                       ;x[16]
	 LDR       r3, [r2], #4                       ;h[14], h[15]
	 PKHBT     r7, r9, r4, LSL #16
	 LDRSH     r9, [r1], #2                       ;x[17]
	 PKHBT     r4, r4, r6, LSL #16
	 SMLAD     r10, r7, r3, r10
	 SMLAD     r11, r4, r3, r11

	 LDRSH     r4, [r1], #2                       ;x[18]
	 LDR       r3, [r2], #4                       ;h[16], h[17]
	 PKHBT     r7, r6, r9, LSL #16
	 LDRSH     r6, [r1], #2                       ;x[19]
	 PKHBT     r9, r9, r4, LSL #16
	 SMLAD     r10, r7, r3, r10
	 SMLAD     r11, r9, r3, r11

	 LDRSH     r9, [r1], #2                       ;x[20]
	 LDR       r3, [r2], #4                       ;h[18], h[19]
	 PKHBT     r7, r4, r6, LSL #16
	 LDRSH     r4, [r1], #2                       ;x[21]
	 PKHBT     r6, r6, r9, LSL #16
	 SMLAD     r10, r7, r3, r10
	 SMLAD     r11, r6, r3, r11

	 LDRSH     r6, [r1], #2                       ;x[22]
	 LDR       r3, [r2], #4                       ;h[20], h[21]
	 PKHBT     r7, r9, r4, LSL #16
	 LDRSH     r9, [r1], #2                       ;x[23]
	 PKHBT     r4, r4, r6, LSL #16
	 SMLAD     r10, r7, r3, r10
	 SMLAD     r11, r4, r3, r11

	 LDRSH     r4, [r1], #2                       ;x[24]
	 LDR       r3, [r2], #4                       ;h[22], h[23]
	 PKHBT     r7, r6, r9, LSL #16
	 LDRSH     r6, [r1], #2                       ;x[25]
	 PKHBT     r9, r9, r4, LSL #16
	 SMLAD     r10, r7, r3, r10
	 SMLAD     r11, r9, r3, r11

	 LDRSH     r9, [r1], #2                       ;x[26]
	 LDR       r3, [r2], #4                       ;h[24], h[25]
	 PKHBT     r7, r4, r6, LSL #16
	 LDRSH     r4, [r1], #2                       ;x[27]
	 PKHBT     r6, r6, r9, LSL #16
	 SMLAD     r10, r7, r3, r10
	 SMLAD     r11, r6, r3, r11

	 LDRSH     r6, [r1], #2                       ;x[28]
	 LDR       r3, [r2], #4                       ;h[26], h[27]
	 PKHBT     r7, r9, r4, LSL #16
	 LDRSH     r9, [r1], #2                       ;x[29]
	 PKHBT     r4, r4, r6, LSL #16
	 SMLAD     r10, r7, r3, r10
	 SMLAD     r11, r4, r3, r11

	 LDRSH     r4, [r1], #2                       ;x[30]
	 LDR       r3, [r2], #4                       ;h[28], h[29]
	 PKHBT     r7, r6, r9, LSL #16
	 LDRSH     r6, [r1], #2                       ;x[31]
	 PKHBT     r9, r9, r4, LSL #16
	 SMLAD     r10, r7, r3, r10
	 SMLAD     r11, r9, r3, r11

	 LDRSH     r9, [r1], #2                       ;x[32]
	 LDR       r3, [r2], #4                       ;h[30], h[31]
	 PKHBT     r4, r4, r6, LSL #16
	 MOV       r5, #0x8000
	 PKHBT     r6, r6, r9, LSL #16
	 SMLAD     r10, r4, r3, r10
	 SMLAD     r11, r6, r3, r11


	 ;SSAT      r10, #32, r10, LSL #2
	 ;SSAT      r11, #32, r11, LSL #2
	 MOV       r10, r10, LSL #1
	 MOV       r11, r11, LSL #1

	 QADD      r10, r10, r10
	 QADD      r11, r11, r11 

	 QADD      r10, r10, r5
	 QADD      r11, r11, r5

	 MOV       r10, r10, ASR #16
	 MOV       r11, r11, ASR #16

	 STRH      r10, [r0], #2
	 STRH      r11, [r0], #2


pred_lt4_end
         LDMFD     r13!, {r4 - r12, r15}

Table
         DCD       voAMRWBEncinter4_2
	 ENDFUNC
	 END



