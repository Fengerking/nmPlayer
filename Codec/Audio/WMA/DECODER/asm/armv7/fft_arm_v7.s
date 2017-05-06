;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;// Filename fft_arm_v7.s
;
;// Copyright (c) VisualOn SoftWare Co., Ltd. All rights reserved.
;
;//*@@@---@@@@******************************************************************
;//
;// Abstract:
;// 
;//     ARM specific transforms
;//     Optimized assembly Cortex-A8 routines to implement DCTIV & FFT and other routines
;// 
;// Author:
;// 
;//     Witten Wen (Shanghai, China) September 1, 2008
;//
;//****************************************************************************
;//
;// void prvFFT4DCT(Void *ptrNotUsed, CoefType data[], Int nLog2np, FftDirection fftDirection)
;// void prvDctIV_ARM ( rgiCoef, nLog2cSB, CR, CI, CR1, CI1, STEP)
;//
;//****************************************************************************

  OPT			2       ; disable listing 
  INCLUDE		kxarm.h
  INCLUDE		wma_member_arm.inc
  INCLUDE		wma_arm_version.h
  OPT			1       ; enable listing
  
  
	AREA    |.text|, CODE, READONLY

	IF	ARMVERSION	>=7	
	IF	WMA_R4FFT = 0
	IF WMA_OPT_FFT_ARM = 1

	IMPORT  icosPIbynp
	IMPORT  isinPIbynp
	IMPORT  FFT4DCT16_STEP
	
	EXPORT  prvFFT4DCT
	EXPORT  prvDctIV_ARM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Registers for FFT
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
px					RN	1
np					RN	2
CR					RN	3
SI					RN	4
STEP				RN	5


m					RN	8
n					RN	9
l					RN	10
CR1					RN	6
SI1					RN	7

CR2					RN	0
SI2					RN	12
k					RN	14
j					RN	11

i					RN	0

pxk					RN	3
pxi					RN	4

temp    			RN	11
temp1				RN	3
temp2				RN	4
temp3				RN	12
temp4				RN	6
;******************************
; Neon register 
;******************************
CR1_SI1				DN	D0				;D0[0] = CR1, D0[1] = SI1
NSI1_CR1			DN	D1				;D1[0] = -SI1, D1[1] = CR1
CR2_SI2				DN	D2				
NSI2_CR2			DN	D3

A0					DN	D4
A1					DN	D5
A2					DN	D6
A3					DN	D7

B0					QN	Q2
B1					QN	Q3
B2					QN	Q4
B3					QN	Q5
B4					QN	Q7
B5					QN	Q6

C0					DN	D8
C1					DN	D9
C2					DN	D10
C3					DN	D11
C4					DN	D12
C5					DN	D13
C6					DN	D14
C7					DN	D15
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Constants for FFT
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
iStackSpaceRev  EQU 8    
iOffset_CR      EQU iStackSpaceRev-4
iOffset_SI      EQU iStackSpaceRev-8

;//****************************************************************************
;//
;// void prvFFT4DCT(Void *ptrNotUsed, CoefType data[], Int nLog2np, FftDirection fftDirection)
;// 
;//****************************************************************************
	IF LINUX_RVDS = 1
  	PRESERVE8
	ENDIF
  	AREA    |.text|, CODE
  	LEAF_ENTRY prvFFT4DCT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Input parameters
; r0 = ptrNotUsed
; r1 = data
; r2 = nLog2np
; r3 = fftDirection
	STMFD   sp!, {r4 - r11, r14}
    SUB     sp, sp, #iStackSpaceRev ; rev stack space
    
;   if ( nLog2np < 16 )
	CMP   r2, #16
	BLT   gLOG2NPLT16
    
;   CR = BP2_FROM_FLOAT(cos(dPI/np));
;   STEP = BP2_FROM_FLOAT(2*sin(-dPI/np));

;   call C stub function
	STMFD sp!, {r1 - r2}  ;save r1, r2
    SUB   sp, sp, #4      ;allocate pCR

    MOV   r0, r2
    MOV   r1, sp
    BL    FFT4DCT16_STEP

    MOV   r4, r0
    LDR   r3, [sp]

    ADD   sp, sp, #4      ;release pCR
    LDMFD sp!, {r1 - r2}  ;restore r1, r2
    B     gPrvFFT_ARM
	 
gLOG2NPLT16
;   CR = BP2_FROM_BP1(icosPIbynp[nLog2np]);         // CR = (I32)(cos(PI/np) * NF2BP2)
	LDR   r12, =icosPIbynp
    
;   STEP = isinPIbynp[nLog2np];                     // STEP = (I32)(2*sin(-PI/np) * NF2BP2)
	LDR   r11, =isinPIbynp
	
	LDR   r4, [r12, r2, LSL #2]	
	LDR   STEP, [r11, r2, LSL #2]	
	MOV   r3, r4, ASR #1
; fftDirection is always FFT_FORWARD at decoder side, so ignore it 
; if (fftDirection == FFT_INVERSE) STEP *= -1;

gPrvFFT_ARM
	MOV     temp, #1	
	MOV		r4, STEP, ASR #1          ; SI = STEP/2;
	MOV     np, temp, LSL r2        ; np (r2) = 1<<nLog2np;
	
	STR     r4, [sp, #iOffset_SI]   ;
	STR     r3, [sp, #iOffset_CR]   ;
	MOV     m, np, LSL #1           ; m = n = 2 * np
	MOV		n, m
	
LoopFirstStage
	CMP     m, #4                   ; m > 4?
	BLE     SecondStage
	
	LDR     SI, [sp, #iOffset_SI]   ; get SI
	LDR     CR, [sp, #iOffset_CR]   ; get CR 
    
	SMMUL   CR1, STEP, SI     		; 32*32, MULT_BP2(STEP,SI)
	MOV     l, m, ASR #1            ; l = m >> 1;
	MVN     CR2, #3, 2              ; CR2 = BP2_FROM_FLOAT(1)
	
	SMMUL   SI1, STEP, CR     		; 32*32, MULT_BP2(STEP,CR)
	SUB     CR1, CR2, CR1, LSL #2   ; CR1 = BP2_FROM_FLOAT(1) - MULT_BP2(STEP,SI);
	MOV     SI2, #0                 ; SI2 = 0
	STR     CR1, [sp, #iOffset_CR]  ; save CR
	
	MOV     SI1, SI1, LSL #2        ; shift arithmetic left 2
 	MOV     STEP, SI1, LSL #1       ; STEP = MUL2(SI1); 
 	STR     SI1, [sp, #iOffset_SI]  ; save SI
 	
TrivialButterfly   
	MOV     k, #0                   ; init k = 0
	VMOV	CR1_SI1, CR1, SI1
	RSB		temp1, SI1, #0
	VMOV	NSI1_CR1, temp1, CR1
	
LoopTrivalButterfly
		ADD     		pxk, px, k, LSL #2      ; pxk = &px[k];
		ADD     		pxi, pxk, m, LSL #1     ; pxi = &px[k+l];
		VLD1.32			{A0, A1}, [pxk]	
		VLD1.32			{A2, A3}, [pxi]
		VHSUB.S32		B2, B0, B1				;C0, C1	
		VHADD.S32		B3, B0, B1
		ADD     		k, k, m                 ; k += m
		CMP     		k, n
		VQDMULL.S32		B4, CR1_SI1, C1[0]
		VQDMLAL.S32		B4, NSI1_CR1, C1[1]
		VST1.32			{C2, C3}, [pxk]!
		VQSHL.S64		B4, #1
		VST1.32			{C0}, [pxi]!
		VST2.32			{C6[1], C7[1]}, [pxi]!	;15 stalls
		BLT     		LoopTrivalButterfly

NontrivialButterfly
	MOV     j, #4
	CMP     j, l
	BGE     OutOfJ
	
LoopNontrivialButterfly_j
		MOV     k, j                    ; k = j
		SMMUL   temp1, STEP, SI1  		; 32*32, MULT_BP2(STEP,SI1)        
        ADD     j, j, #4
        ; stall      
       
        SMMUL   temp2, STEP, CR1 		; 32*32, MULT_BP2(STEP,CR1)
        SUB     CR2, CR2, temp1, LSL #2 ; CR2 -= MULT_BP2(STEP,SI1)
        ; stall
         
        SMMUL   temp1, STEP, CR2 		; 32*32, MULT_BP2(STEP,CR2)
        ADD     SI2, SI2, temp2, LSL #2 ; SI2 += MULT_BP2(STEP,CR1)
        ; stall

        SMMUL   temp2, STEP, SI2  		; 32*32, MULT_BP2(STEP,SI2)
        ADD     SI1, SI1, temp1, LSL #2 ; SI1 += MULT_BP2(STEP,CR2);
        ; stall

        SUB     CR1, CR1, temp2, LSL #2 ; CR1 -= MULT_BP2(STEP,SI2);
		
		VMOV	CR1_SI1, CR1, SI1
		RSB		temp1, SI1, #0
		VMOV	NSI1_CR1, temp1, CR1
		VMOV	CR2_SI2, CR2, SI2
		RSB		temp1, SI2, #0
		VMOV	NSI2_CR2, temp1, CR2
		
LoopCoreButterfly_k
			ADD     		pxk, px, k, LSL #2      ; pxk = &px[k];
			ADD     		pxi, pxk, m, LSL #1     ; pxi = &px[k+l];
			VLD1.32			{A0, A1}, [pxk]			
			VLD1.32			{A2, A3}, [pxi]
			VSUB.S32		B2, B0, B1
			VHADD.S32		B3, B0, B1							
			VQDMULL.S32		B5, CR2_SI2, C0[0]
			VQDMULL.S32		B4, CR1_SI1, C1[0]
			VST1.32			{C2, C3}, [pxk]!			
			VQDMLAL.S32		B5, NSI2_CR2, C0[1]	
			ADD     		k, k, m             ; k = k + m
			CMP     		k, n                ; k <= n?					
			VQDMLAL.S32		B4, NSI1_CR1, C1[1]
			VST4.32			{C4[1], C5[1], C6[1], C7[1]}, [pxi]!
;			15 stalls			
			BLE     LoopCoreButterfly_k
			
		CMP     j, l
		BLT     LoopNontrivialButterfly_j
		
OutOfJ
	MOV     m, l                    ; m = l
	B       LoopFirstStage
	
SecondStage
	CMP     m, #2                       ; Now m is available, m > 2?
	BLE     ThirdStage
	MOV     j, #0
	
LoopSecondStage
	ADD     	pxk, px, j, LSL #2         ; pxk = px+j
	ADD     	pxi, pxk, #8                ; pxi = pxk + 2;	
	VLD1.32		{A0}, [pxk]              ; tk = *pxk
	VLD1.32		{A1}, [pxi]              ; ti = *pxi	
	VQADD.S32	C0, A0, A1
	VQSUB.S32	C1, A0, A1
	ADD     	j, j, #4
	CMP     	j, n
	VST1.S32	C0, [pxk]!
	VST1.S32	C1, [pxi]!
	;6 stalls
	BLT			LoopSecondStage
	
ThirdStage
	CMP     n, #4                      ; Now n is available, n > 4?
	BLE     EndOfprvFFT_ARM
	
	MOV     j, #0
	MOV     i, #0
	
LoopThirdStage
		CMP     i, j
		BGE     ThirdStageEscape
		;5 stalls
		ADD     	pxk, px, j, LSL #2 ; pxk = &px[j];
		ADD     	pxi, px, i, LSL #2 ; pxi = &px[i];
		VLD1.32		{A0}, [pxk]
		VLD1.32		{A1}, [pxi]
		VST1.32		{A0}, [pxi]!
		ADD     	pxi, pxi, np, LSL #2 ; pxi  += np1;
		VST1.32		{A1}, [pxk]!
		ADD     	pxk, pxk, np, LSL #2 ; pxk  += np1;
		VLD1.32		{A0}, [pxi]
		VLD1.32		{A1}, [pxk]
		VST1.32		{A0}, [pxk]
		VST1.32		{A1}, [pxi]
		
ThirdStageEscape
		ADD			temp3, i, #2
		ADD			temp4, j, np                
		ADD			pxi, px, temp3, LSL #2     ; pxi = &px[i+2];
		ADD			pxk, px, temp4, LSL #2     ; pxk = &px[j+np];
		VLD1.32		{A0}, [pxi]						
		VLD1.32		{A1}, [pxk]
		MOV			k, np, ASR #1               ; k = n2;
		VST1.32		{A0}, [pxk]		
		VST1.32		{A1}, [pxi]
		;3 stalls
Cmp_k_j
			CMP     k, j                        ; k <= j?
    		BGT     Out
			SUB     j, j, k                 ; j -= k;
			MOV     k, k, ASR #1            ; k = k / 2
			B       Cmp_k_j
Out
		ADD     j, j, k                     ; j += k
		ADD     i, i, #4                    ; i = i + 4
		CMP     i, np                       ; i < np? 
		BLT     LoopThirdStage
			
; fftDirection is always FFT_FORWARD at decoder side, so ignore it 
; if (fftDirection == FFT_INVERSE) // Normalization to match Intel library
;   for (i = 0; i < 2 * np; i++) data[i] /= np;
			
EndOfprvFFT_ARM
	ADD     sp, sp, #iStackSpaceRev     ; give back rev stack space
	LDMFD   sp!, {r4 - r11, PC}         ; prvFFT4DCT
	ENTRY_END   prvFFT4DCT


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Registers for DCTIV
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

piCoefTop       RN  0
piCoefBottom    RN  1
CR_DCT          RN  2
CI_DCT          RN  3
CR1_DCT         RN  4
CI1_DCT         RN  5
STEP_DCT        RN  6
CR2_DCT         RN  4
CI2_DCT         RN  5
iTi             RN  9  
iTr             RN  8
iBi             RN  9
iBr             RN  8
i_DCT           RN  7
temp0_DCT       RN  12
temp1_DCT       RN  10
temp2_DCT       RN  11
temp3_DCT       RN  14
temp4_DCT       RN  1
temp5_DCT		RN	9


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Constants for DCTIV
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

iRegSpaceDCT        EQU 9*4   ; {r4 - r11, r14}

iStackSpaceRevDCT   EQU 3*4    
iOffset_FFTSize     EQU iStackSpaceRevDCT-4
iOffset_nLog2cSB    EQU iStackSpaceRevDCT-8 
iOffset_rgiCoef     EQU iStackSpaceRevDCT-12

iOffset_CR1_DCT     EQU iRegSpaceDCT+iStackSpaceRevDCT
iOffset_CI1_DCT     EQU iRegSpaceDCT+iStackSpaceRevDCT+4
iOffset_STEP_DCT    EQU iRegSpaceDCT+iStackSpaceRevDCT+8
iOffset_CR2_DCT     EQU iRegSpaceDCT+iStackSpaceRevDCT+12


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	PRESERVE8
    AREA    |.text|, CODE
    LEAF_ENTRY prvDctIV_ARM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    STMFD   sp!, {r4 - r11, r14}
    SUB     sp, sp, #iStackSpaceRevDCT  ;setup stack

    MOV     temp0_DCT, #1
    SUB     temp1_DCT, r1, #2
    MOV     i_DCT, temp0_DCT, LSL temp1_DCT             ; iFFTSize/2 = 1<<(nLog2cSB-2);

    LDR     CR1_DCT, [sp, #iOffset_CR1_DCT]             ; get CR1
    LDR     CI1_DCT, [sp, #iOffset_CI1_DCT]             ; get CI1
    LDR     STEP_DCT, [sp, #iOffset_STEP_DCT]           ; get STEP

    MOV     temp0_DCT, temp0_DCT, LSL r1                ; temp = 1<<nLog2cSB
    STR     r1, [sp, #iOffset_nLog2cSB]

    SUB     temp0_DCT, temp0_DCT, #1                    ; temp = (1<<nLog2cSB) - 1
    
    LDRD	iTr, [piCoefTop]							; iTr = piCoefTop[0]; temp5_DCT = piCoefTop[1];
    ADD     piCoefBottom, piCoefTop, temp0_DCT, LSL #2  ; piCoefBottom = rgiCoef + (1<<nLog2cSB) - 1;
    
    STR     i_DCT, [sp, #iOffset_FFTSize]
    STR     piCoefTop, [sp, #iOffset_rgiCoef]

FirstDCTStageLoop
	LDR     r14, [piCoefBottom]                     ; iBi = piCoefBottom[0]; r14 is iBi here
    SMMULR	temp1_DCT, CR_DCT, iTr       			; MULT_BP1(CR,iTr)
    STR		temp5_DCT, [piCoefBottom], #-8          ; piCoefBottom[0] = piCoefTop[1];
    SMMULR	temp2_DCT, CR_DCT, r14       			; MULT_BP1(CR,iBi)
    SUBS    i_DCT, i_DCT, #1                        ; i --;	
    SMMLSR	temp1_DCT, CI_DCT, r14, temp1_DCT		; MULT_BP1(CI,iBi)
    
    MOV     temp3_DCT, CR_DCT
    SMMLAR	temp2_DCT, CI_DCT, iTr, temp2_DCT		; temp2 = MULT_BP1(CR,iBi) + MULT_BP1(CI,iTr)
    MOV     temp1_DCT, temp1_DCT, LSL #1            ; temp1 = MULT_BP1(CR,iTr) - MULT_BP1(CI,iBi)

    SMMUL   temp5_DCT, STEP_DCT, CI_DCT  			; MULT_BP1(STEP,CI)
    MOV     temp2_DCT, temp2_DCT, LSL #1            ; temp2 = MULT_BP1(CR,iBi) + MULT_BP1(CI,iTr);
    STRD	temp1_DCT, [piCoefTop], #8              ; piCoefTop[0] = MULT_BP1(CR,iTr) - MULT_BP1(CI,iBi);piCoefTop[1] = MULT_BP1(CR,iBi) + MULT_BP1(CI,iTr);
    
    SMMUL   temp2_DCT, STEP_DCT, CR_DCT  			; MULT_BP1(STEP,CR)
    SUB     CR_DCT, CR1_DCT, temp5_DCT, LSL #1      ; CR = CR1 - MULT_BP1(STEP,CI);
    MOV     CR1_DCT, temp3_DCT                      ; CR1 = CR;
	LDRD	iTr, [piCoefTop]						; iTr = piCoefTop[0]; r9 = piCoefTop[1];
	
    MOV     temp3_DCT, CI_DCT
    ADD     CI_DCT, CI1_DCT, temp2_DCT, LSL #1      ; CI = CI1 + MULT_BP1(STEP,CR);  
    MOV     CI1_DCT, temp3_DCT                      ; CI1 = CI;

    BNE     FirstDCTStageLoop


    LDR     i_DCT, [sp, #iOffset_FFTSize]

SecondDCTStageLoop                                  
    SMMULR   temp1_DCT, CR_DCT, iTr       			; MULT_BP1(CR,iTr)
    MOV     temp3_DCT, CR_DCT						;	

    SMMULR   temp2_DCT, CR_DCT, iTi       			; MULT_BP1(CR,iTi)
    SUBS    i_DCT, i_DCT, #1                        ; i --;
    
    SMMLSR  temp1_DCT, CI_DCT, iTi, temp1_DCT		; MULT_BP1(CR,iTr) + MULT_BP1(-CI,iTi)

    SMMUL   temp0_DCT, STEP_DCT, CR_DCT  			; MULT_BP1(STEP,CR)
    
    SMMLAR	temp2_DCT, CI_DCT, iTr, temp2_DCT		; MULT_BP1(CR,iTi) + MULT_BP1(CI,iTr)    
    MOV     temp1_DCT, temp1_DCT, LSL #1            ; temp1 = MULT_BP1(CR,iTr) - MULT_BP1(CI,iTi);
    
	SMMUL   temp5_DCT, STEP_DCT, CI_DCT  			; MULT_BP1(STEP,CI)		
	MOV     temp2_DCT, temp2_DCT, LSL #1            ; temp2 = MULT_BP1(CI,iTr) + MULT_BP1(CR,iTi);
	STRD	temp1_DCT, [piCoefTop], #8              ; piCoefTop[0] = MULT_BP1(CR,iTr) - MULT_BP1(CI,iTi);piCoefTop[1] = MULT_BP1(CR,iTi) + MULT_BP1(CI,iTr);
    
    MOV     temp4_DCT, CI_DCT							
    
    SUB     CR_DCT, CR1_DCT, temp5_DCT, LSL #1      ; CR = CR1 - MULT_BP1(STEP,CI);
    MOV     CR1_DCT, temp3_DCT                      ; CR1 = CR;
    
	LDRD	iTr, [piCoefTop]						; iTr = piCoefTop[0]; iTi = piCoefTop[1];
    ADD     CI_DCT, CI1_DCT, temp0_DCT, LSL #1      ; CI = CI1 + MULT_BP1(STEP,CR);
    
    MOV     CI1_DCT, temp4_DCT                      ; CI1 = CI;

    BNE     SecondDCTStageLoop

CallFFT
;   prvFFT4DCT(NULL, rgiCoef, nLog2cSB - 1, FFT_FORWARD);
	LDR     r2, [sp, #iOffset_nLog2cSB]
    LDR     r1, [sp, #iOffset_rgiCoef]    
    SUB     r2, r2, #1
;   MOV     r3, #0
    bl      prvFFT4DCT

    LDR     temp1_DCT, [sp, #iOffset_nLog2cSB]
    LDR     piCoefTop, [sp, #iOffset_rgiCoef]

    MOV     temp0_DCT, #1
    MOV     temp0_DCT, temp0_DCT, LSL temp1_DCT         ; temp = 1<<nLog2cSB
    SUB     temp0_DCT, temp0_DCT, #2                    ; temp = (1<<nLog2cSB) - 2
    ADD     piCoefBottom, piCoefTop, temp0_DCT, LSL #2  ; piCoefBottom = rgiCoef + (1<<nLog2cSB) - 2;
	
    MVN     CR_DCT, #2, 2                               ; CR = BP1_FROM_FLOAT(1);
    MOV     CI_DCT, #0   								; CI = 0
                                   
	LDRD	iTr, [piCoefTop]							;iTr = piCoefTop[0]; iTi = piCoefTop[1];
    LDR     i_DCT, [sp, #iOffset_FFTSize]
    LDR     CR2_DCT, [sp, #iOffset_CR2_DCT]             ; get CR2

    MOV     CI2_DCT, STEP_DCT, ASR #1                   ; DIV2 of STEP
    RSB     CI2_DCT, CI2_DCT, #0                        ; CI2  = -DIV2(STEP); 


ThirdDCTStageLoop
    SMMULR	temp1_DCT, CR_DCT, iTr       			; MULT_BP1(CR,iTr)    
    SMMULR	temp2_DCT, CI_DCT, iTr       			; MULT_BP1(CI,iTr)    
    SMMLSR	temp1_DCT, CI_DCT, iTi, temp1_DCT       ; MULT_BP1(CR,iTr) -  MULT_BP1(CI,iTi);    
    SMMLAR	temp2_DCT, CR_DCT, iTi, temp2_DCT       ; MULT_BP1(CI,iTr) + MULT_BP1(CR,iTi);
    MOV     temp1_DCT, temp1_DCT, LSL #1            ; temp1 = MULT_BP1(CR,iTr) -  MULT_BP1(CI,iTi);
    STR     temp1_DCT, [piCoefTop], #4              ; piCoefTop[0] =  MULT_BP1(CR,iTr) -  MULT_BP1(CI,iTi);
    RSB		temp2_DCT, temp2_DCT, #0				; MULT_BP1(-CI,iTr) - MULT_BP1(CR,iTi);
    SUBS    i_DCT, i_DCT, #1                        ; i --;
    SMMULR	temp1_DCT, STEP_DCT, CI_DCT  			; MULT_BP1(STEP,CI);
    MOV     temp2_DCT, temp2_DCT, LSL #1            ; temp2 = MULT_BP1(-CI,iTr) - MULT_BP1(CR,iTi);
    LDRD	iBr, [piCoefBottom]						; iBr; iBi
    STR     temp2_DCT, [piCoefBottom, #4]           ; piCoefBottom[1] =  MULT_BP1(-CI,iTr) - MULT_BP1(CR,iTi);
    
    SMMULR	temp2_DCT, STEP_DCT, CR_DCT  			; MULT_BP1(STEP,CR);
    MOV     temp3_DCT, CR_DCT	 
    SUB     CR_DCT, CR2_DCT, temp1_DCT, LSL #1      ; CR = CR2 - MULT_BP1(STEP,CI);
    MOV     CR2_DCT, temp3_DCT                      ; CR2 = CR;  
    MOV     temp3_DCT, CI_DCT 

    SMMULR	temp1_DCT, CR_DCT, iBr       			; MULT_BP1(CR,iBr)
    ADD     CI_DCT, CI2_DCT, temp2_DCT, LSL #1      ; CI = CI2 + MULT_BP1(STEP,CR);    
	SMMULR	temp2_DCT, CR_DCT, iBi					; MULT_BP1(CR,iBi);
	
    SMMLAR	temp1_DCT, CI_DCT, iBi, temp1_DCT		; MULT_BP1(CR,iBr) + MULT_BP1(CI,iBi);
    MOV     CI2_DCT, temp3_DCT                      ; CI2 = CI;
    
    SMMLSR	temp2_DCT, CI_DCT, iBr, temp2_DCT		; MULT_BP1(-CI,iBr) +  MULT_BP1(CR,iBi);
    MOV     temp1_DCT, temp1_DCT, LSL #1            ; temp1 = MULT_BP1(CR,iBr) + MULT_BP1(CI,iBi);
    STR     temp1_DCT, [piCoefTop], #4              ; piCoefTop[1] = MULT_BP1(CR,iBr) + MULT_BP1(CI,iBi);
   
	MOV     temp2_DCT, temp2_DCT, LSL #1            ; temp2 = MULT_BP1(-CI,iBr) +  MULT_BP1(CR,iBi);
	LDRD	iTr, [piCoefTop]						;iTr = piCoefTop[0]; iTi = piCoefTop[1];
    STR     temp2_DCT, [piCoefBottom], #-8          ; piCoefBottom[0] = MULT_BP1(-CI,iBr) +  MULT_BP1(CR,iBi);
        
    BNE     ThirdDCTStageLoop

EndOfprvDctIV_ARM
    ADD     sp, sp, #iStackSpaceRevDCT  ; give back rev stack space
    LDMFD   sp!, {r4 - r11, PC}         ; prvDctIV_ARM
    ENTRY_END   prvDctIV_ARM
    		
	ENDIF		;//WMA_OPT_FFT_ARM
	ENDIF		;//WMA_R4FFT
	ENDIF		;//ARMVERSION
	END
