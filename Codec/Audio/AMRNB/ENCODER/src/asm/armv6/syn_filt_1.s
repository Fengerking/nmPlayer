;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;void Syn_filt (
;			   Word16 a[],     /* (i)     : a[M+1] prediction coefficients   (M=10)  */
;			   Word16 x[],     /* (i)     : input signal                             */
;			   Word16 y[],     /* (o)     : output signal                            */
;			   Word16 lg,      /* (i)     : size of filtering                        */
;			   Word16 mem[],   /* (i/o)   : memory associated with this filtering.   */
;			   Word16 update   /* (i)     : 0=no update, 1=update of memory.         */
;			   )
; update = 0

       AREA     |.text|, CODE, READONLY
       EXPORT   Syn_filt_1
       IMPORT   myMemCopy

Syn_filt_1 PROC

        STMFD   r13!, {r4 - r11, r14} 
        SUB     r13, r13, #116                   
        MOV     r12, #40
	STR     r3, [r13, #8]                      ;mem[] addr
	STR     r2, [r13, #12]                     ;y[] addr
;myMemCopy
        ADD     r4, r13, #16                       ;tmp addr    
        STMFD   r13!, {r0, r1, r2, r12}
        MOV     r2, #20
        MOV     r0, r4
        MOV     r1, r3
        BL      myMemCopy
        LDMFD   r13!, {r0, r1, r2, r12}

    	LDRSH   r3, [r0], #2                         ;r3 --- X, a0
        LDRSH   r5, [r0], #2                         ;a1
	LDRSH   r6, [r0], #2                         ;a2
	LDRSH   r7, [r0], #2                         ;a3
        STR     r3, [r13, #4]
	LDRSH   r8, [r0], #2                         ;a4
	PKHBT   r4, r5, r6, LSL #16                  ;r4 --- a2, a1
	LDRSH   r6, [r0], #2                         ;a5
	PKHBT   r5, r7, r8, LSL #16                  ;r5 --- a4, a3

	LDRSH   r7, [r0], #2                         ;a6
	LDRSH   r8, [r0], #2                         ;a7
        LDRSH   r9, [r0], #2                         ;a8
	PKHBT   r6, r6, r7, LSL #16                  ;r6 --- a6, a5
        LDRSH   r10, [r0], #2                         ;a9
	LDRSH   r11, [r0], #2                         ;a10
	PKHBT   r7, r8, r9, LSL #16                  ;r7 --- a8, a7
        ADD     r9, r13, #36                         ;yy = tmp +10
	PKHBT   r8, r10, r11, LSL #16                  ;r8 --- a10, a9

L10_4                      
        LDRSH   r10, [r1], #2                     ; get x[i]
        LDRSH   r11, [r9, #-2]                    ; yy[-1]
	; r0, r3, r12, r10
	LDRSH   r0,  [r9, #-4]                    ; yy[-2]
	MUL     r14, r3, r10                      ; s = x[i] * (*(tmpA++));
	LDRSH   r3,  [r9, #-6]                    ; yy[-3]
	PKHBT   r0, r11, r0, LSL #16              ; r0 --- yy[-2], yy[-1]
	LDRSH   r11, [r9, #-8]                    ; yy[-4]
	RSB     r14, r14, #0
	SMLAD   r14, r0, r4, r14                  ; a[1] * yy[-1] + a[2] * yy[-2]
	LDRSH   r0, [r9, #-10]                    ; yy[-5]
	PKHBT   r3, r3, r11, LSL #16              ; r3 --- yy[-4], yy[-3]
	LDRSH   r10, [r9, #-14]                   ; yy[-7]
	SMLAD   r14, r3, r5, r14                  ; a[3] * yy[-3] + a[4] * yy[-4]

	LDRSH   r3, [r9, #-12]                    ; yy[-6]
	LDRSH   r11, [r9, #-16]                   ; yy[-8]

	PKHBT   r0, r0, r3, LSL #16               ; r0 --- yy[-6], yy[-5]
	PKHBT   r3, r10, r11, LSL #16             ; r3 --- yy[-8], yy[-7]

	SMLAD   r14, r0, r6, r14                  ; a[6] * yy[-6] + a[5] * yy[-5]
	SMLAD   r14, r3, r7, r14                  ; a[8] * yy[-8] + a[7] * yy[-7]
        
	LDRSH   r10, [r9, #-18]                   ; yy[-9]
	LDRSH   r11, [r9, #-20]                   ; yy[-10]
	LDR     r3, [r13, #4]
	PKHBT   r10, r10, r11, LSL #16            ; r10 --- yy[-10], yy[-9]

	SMLAD   r14, r10, r8, r14                 ; a[10] * yy[-10] + a[9] * y[-9]
        RSB     r14, r14, #0	
        
        ;s = ASM_L_shl(s, 4)
        SSAT    r14, #32, r14, LSL #4 

        MOV     r10, #0x8000
        QADD    r11, r14, r10
        SUBS    r12, r12, #1        
        MOV     r11, r11, ASR #16

        STRH    r11, [r9], #2
        STRH    r11, [r2], #2
        BGT     L10_4

	LDR     r4, [r13, #12]
        LDR     r0, [r13, #8]
        ADD     r1, r4, #60
        MOV     r2, #20
        BL      myMemCopy 	

        ADD     r13, r13, #116        
        LDMFD   r13!, {r4 - r11, r15} 
 
        ENDP
        END

