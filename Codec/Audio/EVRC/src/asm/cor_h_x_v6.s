;**************************************************************
;* Copyright 2009 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;void cor_h_x (
;	      Word16 h[],    /* (i): impulse response of weighted synthesis filter */
;	      Word16 x[],    /* (i): target                                        */
;	      Word16 dn[],   /* (o): correlation between target and h[]            */
;	      )
;******************
;   ARM Register
;******************
; r0 --- h[]
; r1 --- x[]
; r2 --- dn[]
        AREA     |.text|,  CODE, READONLY
	EXPORT   cor_h_x2_asm

cor_h_x2_asm FUNCTION

        STMFD          r13!, {r4 - r12, r14} 
        MOV            r4, #5                             ; tot = 5
        SUB            r13, r13, #280                     ; y32[L_CODE]
	MOV            r5, #4                             ; k = NB_TRACK - 1
	STR            r2, [r13, #4]                      ; push dn[]
	ADD            r2, r13, #12                       ; y32[] addr

LOOPK
	MOV            r7, r5                             ; i = k
        MOV            r6, #0                             ; max = 0

LOOPi
        MOV            r9, r7                             ; j = i
	ADD            r3, r2, r7, LSL #2                 ; y32[i] addr
        MOV            r10, r0
        ADD            r11, r1, r9, LSL #1                ; x[j] addr
        MOV            r8, #0                             ; s = 0

LOOPj
        LDRSH          r12, [r11], #2                     ; load x[j]
        LDRSH          r14, [r10], #2                     ; load h[j - i]
	CMP            r12, #0
	BEQ            Lable
	MLA            r8, r12, r14, r8

Lable
	ADD            r9, r9, #1
	CMP            r9, #55
	BLT            LOOPj
        ADD            r12, r8, r8                        ; s <<= 1;
	STR            r12, [r3]
        CMP            r12, #0
	RSBLT          r12, r12, #0                        ; s = (s < 0) ? -s: s
	CMP            r12, r6
	MOVGT          r6, r12                             ; if(s > max) max = s
	ADD            r7, r7, #5
	CMP            r7, #55
	BLT            LOOPi

        ADD            r4, r4, r6, ASR #1                 ; tot += (max >> 1)
        
        SUBS           r5, r5, #1
	BGE            LOOPK
        ; r4 --- tot

	LDR            r5, [r13, #4]                      ; pull dn[]
        CLZ            r6, r4
        MOV            r7, #0                             ; i = 0
        SUB            r6, r6, #3                         ; j = norm_l(tot) - sf

	; r2 --- y32[], r7 --- i, r6 --- j, r5 --- dn[]
	MOV            r12, #0x8000
        CMP            r6, #0
        BGE            LOOPL2
        RSB            r6, r6, #0    

LOOPL1	
        LDR            r8,  [r2], #4
        LDR            r9,  [r2], #4
        LDR            r10, [r2], #4
        LDR            r11, [r2], #4
	LDR            r3,  [r2], #4

        ADD            r8, r12, r8, ASR r6
        ADD            r9, r12, r9, ASR r6	
        ADD            r10, r12, r10, ASR r6
	ADD            r11, r12, r11, ASR r6
	ADD            r3,  r12, r3,  ASR r6

	MOV            r8, r8, ASR #16
	MOV            r9, r9, ASR #16
	MOV            r10, r10, ASR #16
	MOV            r11, r11, ASR #16
	MOV            r3, r3, ASR #16

	STRH           r8,  [r5], #2
	STRH           r9,  [r5], #2
	STRH           r10, [r5], #2
        STRH           r11, [r5], #2
	STRH           r3,  [r5], #2

	ADD            r7, r7, #5
	CMP            r7, #55
	BLT            LOOPL1
	BL             cor_h_x2_asm_end 

LOOPL2
        LDR            r8,  [r2], #4
        LDR            r9,  [r2], #4
        LDR            r10, [r2], #4
        LDR            r11, [r2], #4
	LDR            r3,  [r2], #4

        ADD            r8, r12, r8, LSL r6
        ADD            r9, r12, r9, LSL r6	
        ADD            r10, r12, r10, LSL r6
	ADD            r11, r12, r11, LSL r6
	ADD            r3,  r12, r3, LSL r6

	MOV            r8, r8, ASR #16
	MOV            r9, r9, ASR #16
	MOV            r10, r10, ASR #16
	MOV            r11, r11, ASR #16
	MOV            r3, r3, ASR #16

	STRH           r8,  [r5], #2
	STRH           r9,  [r5], #2
	STRH           r10, [r5], #2
        STRH           r11, [r5], #2
	STRH           r3, [r5], #2

	ADD            r7, r7, #5
	CMP            r7, #55
	BLT            LOOPL2
	
cor_h_x2_asm_end

        ADD            r13, r13, #280    
        LDMFD          r13!, {r4 - r12, r15}  

	ENDFUNC
        END
