;**************************************************************
;* Copyright 2003~2010 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;void Weight_Ai (
;    Word16 a[],         /* (i)     : a[M+1]  LPC coefficients   (M=10)    */
;    const Word16 fac[], /* (i)     : Spectral expansion factors.          */
;    Word16 a_exp[]      /* (o)     : Spectral expanded LPC coefficients   */
;)

    AREA    |.text|, CODE, READONLY 
  	EXPORT   Vo_weight_ai 

Vo_weight_ai PROC
        STMFD          r13!, {r4 - r12, r14} 
        MOV            r12,  #0x4000 
        LDRSH          r3, [r0], #2                    ;r3---a[0]

        LDRSH          r4, [r1], #2                    ;r4---fac[0]
        LDRSH          r5, [r0], #2                    ;r5---a[1]

        LDRSH          r6, [r1], #2                    ;r6---fac[1]
        LDRSH          r7, [r0], #2                    ;r7---a[2]

        LDRSH          r8, [r1], #2                    ;r8---fac[2]
	LDRSH          r9, [r0], #2                    ;r9---a[3]

	MLA            r10, r4, r5, r12                ;fac[0] * a[1]
	MLA            r11, r6, r7, r12                ;fac[1] * a[2]

	MOV            r4, r10, ASR #15
	MOV            r5, r11, ASR #15

	MLA            r14, r8, r9, r12                ;fac[2] * a[3]
	
	LDRSH          r6, [r1], #2                    ;fac[3]
	MOV            r14, r14, ASR #15

	LDRSH          r7, [r0], #2                    ;a[4]

	STRH           r3, [r2], #2
	STRH           r4, [r2], #2
	STRH           r5, [r2], #2
	STRH           r14, [r2], #2

	;PKHBT          r8, r3, r4, LSL #16
	;PKHBT          r9, r5, r14, LSL #16

	LDRSH          r4, [r1], #2                    ;fac[4]
	LDRSH          r5, [r0], #2                    ;a[5]
        ;STR            r8, [r2], #4
	;STR            r9, [r2], #4
	LDRSH          r8, [r1], #2                    ;fac[5]
	LDRSH          r9, [r0], #2                    ;a[6]

	MLA            r3, r6, r7, r12                 ;fac[3] * a[4]
	MLA            r6, r4, r5, r12                 ;fac[4] * a[5]
	MLA            r7, r8, r9, r12                 ;fac[5] * a[6]

	MOV            r3, r3, ASR #15
	MOV            r6, r6, ASR #15
	MOV            r7, r7, ASR #15

	LDRSH          r4, [r1], #2                    ;fac[6]
	LDRSH          r5, [r0], #2                    ;a[7]
	;PKHBT          r14, r3, r6, LSL #16
	STRH           r3, [r2], #2
	STRH           r6, [r2], #2
	LDRSH          r8, [r1], #2                    ;fac[7]
	LDRSH          r9, [r0], #2                    ;a[8]
        ;STR            r14, [r2], #4
	MLA            r14, r4, r5, r12                ;fac[6] * a[7]
	MLA            r3, r8, r9, r12                 ;fac[7] * a[8]
	MOV            r14, r14, ASR #15

	LDRSH          r4, [r1], #2                    ;fac[8]
	LDRSH          r5, [r0], #2                    ;a[9]
        ;PKHBT          r8, r7, r14, LSL #16
        STRH           r7, [r2], #2
	STRH           r14, [r2], #2
	MLA            r9, r4, r5, r12                 ;fac[8] * a[9]
	MOV            r3, r3, ASR #15
	MOV            r9, r9, ASR #15
	;STR            r8, [r2], #4

	LDRSH          r4, [r1], #2                    ;fac[9]
	LDRSH          r5, [r0], #2                    ;a[10]
        ;PKHBT          r7, r3, r9, LSL #16
	STRH           r3, [r2], #2
	STRH           r9, [r2], #2
	MLA            r8, r4, r5, r12                 ;fac[9] * a[10]
	;STR            r7, [r2], #4
	MOV            r4, r8, ASR #15
	STRH           r4, [r2]
  
weight_asm_end 
 
        LDMFD          r13!, {r4 - r12, r15}  
        ENDP
        END

