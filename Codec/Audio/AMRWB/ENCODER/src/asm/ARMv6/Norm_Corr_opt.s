;**************************************************************
;* Copyright 2003-2010 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************

;static void Norm_Corr (Word16 exc[], 
;                       Word16 xn[], 
;                       Word16 h[],
;                       Word16 L_subfr,
;                       Word16 t_min, 
;                       Word16 t_max,
;                       Word16 corr_norm[])
;

	AREA	|.text|, CODE, READONLY
        EXPORT    voAMRWBEncNorm_corr_asm 
        IMPORT    voAMRWBEncConvolve_asm
        IMPORT    Isqrt_n
                  
voAMRWBEncNorm_corr_asm     FUNCTION

        STMFD      r13!, {r4 - r12, r14}  
        SUB        r13, r13, #172
  
        ADD        r8, r13, #20                 ;get the excf[L_SUBFR]
        LDR        r4, [r13, #212]            ;get t_min
        RSB        r11, r4, #0                  ;k = -t_min
        ADD        r5, r0, r11, LSL #1          ;get the &exc[k]   
        
        ;transfer Convolve function
        STMFD       sp!, {r0 - r3}
        MOV         r0, r5
        MOV         r1, r2
        MOV         r2, r8                       ;r2 --- excf[]
        BL          voAMRWBEncConvolve_asm
        LDMFD       sp!, {r0 - r3}

        ; r8 --- excf[]

	MOV         r14, r1                       ;copy xn[] address                      
        MOV         r5, #64
        MOV         r6, #0                       ;L_tmp = 0
        MOV         r7, #1	
LOOP1
        LDR         r9,  [r14], #4
	LDR         r10, [r14], #4
	LDR         r11, [r14], #4
	LDR         r12, [r14], #4
	SMLAD       r6, r9, r9, r6
	SMLAD       r6, r10, r10, r6
	SMLAD       r6, r11, r11, r6
	SMLAD       r6, r12, r12, r6
        SUBS        r5, r5, #8
        BNE         LOOP1

	ADD         r9, r7, r6, LSL #1           ;L_tmp = (L_tmp << 1) + 1
	CLZ         r7, r9
	SUB         r6, r7, #1                   ;exp = norm_l(L_tmp)
        RSB         r7, r6, #32                  ;exp = 32 - exp
	MOV         r6, r7, ASR #1         
	RSB         r7, r6, #0                   ;scale = -(exp >> 1)
	
        ;loop for every possible period
	;for(t = t_min; t <= t_max; t++)
	;r7 --- scale r4 --- t_min r8 --- excf[]
LOOPFOR	
        MOV         r5, #0                       ;L_tmp  = 0
	MOV         r6, #0                       ;L_tmp1 = 0
	MOV         r9, #64  
	MOV         r12, r1                      ;copy of xn[]
	ADD         r14, r13, #20                ;copy of excf[]
	MOV         r8, #0x8000
        	
LOOPi
	LDR         r11, [r14], #4               ;load excf[i], excf[i+1]
        LDR         r10, [r12], #4               ;load xn[i], xn[i+1]
	SMLAD       r6, r11, r11, r6
	SMLAD       r5, r10, r11, r5
	LDR         r11, [r14], #4               ;load excf[i+2], excf[i+3]
	LDR         r10, [r12], #4               ;load xn[i+2], xn[i+3]
	SMLAD       r6, r11, r11, r6
	SMLAD       r5, r10, r11, r5

	SUBS        r9, r9, #4
        BNE         LOOPi

	;r5 --- L_tmp, r6 --- L_tmp1
	MOV         r10, #1
	ADD         r5, r10, r5, LSL #1          ;L_tmp = (L_tmp << 1) + 1
	ADD         r6, r10, r6, LSL #1          ;L_tmp1 = (L_tmp1 << 1) + 1
 
	CLZ         r10, r5        
	CMP         r5, #0
	RSBLT       r11, r5, #0
	CLZLT       r10, r11
	SUB         r10, r10, #1                 ;exp = norm_l(L_tmp)
     
	MOV         r5, r5, LSL r10              ;L_tmp = (L_tmp << exp)
	RSB         r10, r10, #30                ;exp_corr = 30 - exp
	MOV         r11, r5, ASR #16             ;corr = extract_h(L_tmp)

	CLZ         r5, r6
	SUB         r5, r5, #1
	MOV         r6, r6, LSL r5               ;L_tmp = (L_tmp1 << exp)
	RSB         r5, r5, #30                  ;exp_norm = 30 - exp

	;r10 --- exp_corr, r11 --- corr
	;r6  --- L_tmp, r5 --- exp_norm

	;Isqrt_n(&L_tmp, &exp_norm)

	MOV         r14, r0
	MOV         r12, r1 

        STMFD       sp!, {r0 - r4, r7 - r12, r14}
	ADD         r1, sp, #4
	ADD         r0, sp, #0
	STR         r6, [sp]
	STRH        r5, [sp, #4]
	BL          Isqrt_n
	LDR         r6, [sp]
	LDRSH       r5, [sp, #4]
        LDMFD       sp!, {r0 - r4, r7 - r12, r14}
	MOV         r0, r14
	MOV         r1, r12


	MOV         r6, r6, ASR #16              ;norm = extract_h(L_tmp)
	MUL         r12, r6, r11
	ADD         r12, r12, r12                ;L_tmp = vo_L_mult(corr, norm)
  
	ADD         r6, r10, r5
	ADD         r6, r6, r7                   ;exp_corr + exp_norm + scale

        CMP         r6, #0
        RSBLT       r6, r6, #0
	MOVLT       r12, r12, ASR r6
        MOVGT       r12, r12, LSL r6             ;L_tmp = L_shl(L_tmp, exp_corr + exp_norm + scale)

        ADD         r12, r12, r8
        MOV         r12, r12, ASR #16            ;vo_round(L_tmp)

        LDR         r5, [r13, #220]        ; get corr_norm address
	LDR         r6, [r13, #216]            ; get t_max
	ADD         r10, r5, r4, LSL #1          ; get corr_norm[t] address
	STRH        r12, [r10]                   ; corr_norm[t] = vo_round(L_tmp)

	CMP         r4, r6
	BEQ         Norm_corr_asm_end
 
	ADD         r4, r4, #1                   ; t_min ++
        
	RSB         r5, r4, #0                   ; k

	MOV         r6, #63                      ; i = 63
	MOV         r8, r0                       ; exc[]
	MOV         r9, r2                       ; h[]
	ADD         r10, r13, #20                ; excf[]

	ADD         r8, r8, r5, LSL #1           ; exc[k] address
	ADD         r9, r9, r6, LSL #1           ; h[i] address
	ADD         r10, r10, r6, LSL #1         ; excf[i] address
	LDRSH       r11, [r8]                    ; tmp = exc[k]
LOOPK
        LDRSH       r8, [r9], #-2                ; load h[i]
	LDRSH       r12, [r10, #-2]              ; load excf[i - 1]
	MUL         r14, r11, r8
	MOV         r8, r14, ASR #15
	ADD         r14, r8, r12
	STRH        r14, [r10], #-2
	SUBS        r6, r6, #1
	BGT         LOOPK

	LDRSH       r8, [r9]                     ; load h[0]
	MUL         r14, r11, r8
        LDR         r6, [r13, #216]            ; get t_max
	MOV         r8, r14, ASR #15
	STRH        r8, [r10]                    

	CMP         r4, r6
	BLE         LOOPFOR

Norm_corr_asm_end 
        
        ADD            r13, r13, #172     
        LDMFD          r13!, {r4 - r12, r15}
    
        ENDFUNC
        END


