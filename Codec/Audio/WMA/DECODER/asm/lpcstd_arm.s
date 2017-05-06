;//*@@@+++@@@@******************************************************************
;//
;// Microsoft Windows Media
;// Copyright (C) Microsoft Corporation. All rights reserved.
;//
;//*@@@---@@@@******************************************************************
;// Module Name:
;//
;//     lpcstd_arm.s
;//
;// Abstract:
;// 
;//     ARM Arch-4 specific multiplications
;//
;//      Custom build with 
;//          armasm $(InputDir)\$(InputName).s -o=$(OutDir)\$(InputName).obj 
;//      and
;//          $(OutDir)\$(InputName).obj
;// 
;// Author:
;// 
;//     Sil Sanders (sils) October 9, 2001
;//
;// Revision History:
;// 
;//     Jerry He (yamihe) Dec 13th, 2003
;//
;//     For more information on ARM assembler directives, use
;//        http://msdn.microsoft.com/library/default.asp?url=/library/en-us/wcechp40/html/ccconarmassemblerdirectives.asp
;//*************************************************************************


  OPT         2       ; disable listing 
  INCLUDE     kxarm.h
  INCLUDE     wma_member_arm.inc
  INCLUDE	  wma_arm_version.h
  OPT         1       ; enable listing
 
  AREA    |.text|, CODE, READONLY


  IF WMA_OPT_LPCSTD_ARM = 1
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

 
;=====================================================================================================================
;*********************************************************************************************************************
;=====================================================================================================================
; WeightType InverseQuadRootOfSumSquares(LpSpecType F1, LpSpecType F2 );
; This optimization does not seem like much, but improves overall performance 2.2% (16_16m8) to 3.7% (22_32s4) 

    EXPORT  InverseQuadRootOfSumSquares
    EXPORT  prvDoLpc4
    
    IMPORT  g_InvQuadRootFraction
    IMPORT  g_InvQuadRootExponent
    IMPORT  gLZLTable

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  AREA    |.text|, CODE
  LEAF_ENTRY InverseQuadRootOfSumSquares
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; 588  : {

    stmdb     sp!, {r4, lr}  ; stmfd
|$M4106|


;   LpSpecPowerType fOrig = SUM_SQUARES( F1, F2 );
;   LpSpecPowerType f = fOrig >> (2 * FRACT_BITS_LP_SPEC - FRACT_BITS_LP_SPEC_POWER);

    SMULL     R12, R4, r1, r1               ; F1*F1
    ldr       lr, pgLZLTable                ; needed later
    ;;; stall - issue = 1 cycles, but SMLAL can follow the next cycle after this on StrongARM (experimentally verified)
    SMLAL     R12, R4, r0, r0               ; F2 * F2
    mov       r3, #0                        ; iExp = 0
    MOV       r0, r12, LSR #20              ; macl >> 20
    ORR       r0,  r0, r4, LSL #12          ; low32(f)  = mach << (32-20) | macl >> 20
    movs      r1,  r4, LSR #20              ; high32(f) = mach >> 20;

; 591  :     U32 uiMSF = (U32)(f>>32);
; 592  :     Int iExp = 0;
; 593  :     int iMSF8;
; 594  :     U32 uiFrac1;
; 595  :    U8 index;
; 596  :     if ( uiMSF==0 ) {
; 597  :         iExp = 32;
; 598  :         uiMSF = (U32)f;

;   r2 = uiMSF
;   r3 = iExp

    orrs      r2, r1, r1                    ; if ( uiMSF == 0 )
    moveq     r3, #32                       ;   iExp = 32
    moveqs    r2, r0                        ;   uiMSF = (U32)f

; 599  :     }
; 600  :     if (uiMSF==0) 
; 601  :         return(0xFFFFFFFF);

    mvneq     r0, #0                        ; if (uiMSF==0) return(0xFFFFFFFF)
    ldmeqia   sp!, {r4, pc}        ; ldmeqfd

; 630  : }


; 602  :     // normalize the most significant fractional part
; 603  :     while( (uiMSF & 0xFF000000)==0 ) {
; 604  :         iExp += 8;
; 605  :         uiMSF <<= 8;

;   r4  = high32(fOrig)
;   r12 = low32(fOrig)
;   r2 = uiMSF
;   r3 = iExp


    ands      r0, r2, #0xFF, 8              ; (uiMSF & 0xFF000000)==0
    bne       |$L3787|
|$L3786|
    mov       r2, r2, lsl #8                ; uiMSF <<= 8
    add       r3, r3, #8                    ; iExp += 8
    ands      r0, r2, #0xFF, 8              ; (uiMSF & 0xFF000000)==0
    beq       |$L3786|
|$L3787|

; 606  :     }

; 609  :    index=(U8)(uiMSF>>24);
; 610  :    iExp+=((gLZLTable[index>>1]>>((index&1)<<2)))&0xf;
; 613  :     // discard the most significant one bit (it's presence is built into g_InvQuadRootFraction)
; 614  :     iExp++;

;   r0 = index

    mov       r0, r2, lsr #24               ; index=(U8)(uiMSF>>24)    
    ldrb      lr, [lr, +r0, lsr #1]         ; gLZLTable[index>>1]
    and       r2, r0, #1                    ; index&1
    mov       r2, r2, lsl #2                ; (index&1)<<2
    mov       lr, lr, lsr r2                ; (gLZLTable[index>>1]>>((index&1)<<2))
    and       r2, lr, #0xF                  ; ((gLZLTable[index>>1]>>((index&1)<<2)))&0xf
    add       r2, r2, r3                    ; iExp+=((gLZLTable[index>>1]>>((index&1)<<2)))&0xf
    add       r3, r2, #1                    ; iExp++

;;; this is a change made on v10 code base which different as v8
;;; v8 code
; 621  :     uiMSF = (iExp>32) ? (U32)(f<<(iExp-32)) : (U32)(f>>(32-iExp));
;;; v10 code
;	iExpNt=(iExp-(2 * FRACT_BITS_LP_SPEC - FRACT_BITS_LP_SPEC_POWER));   
;   uiMSF = (iExpNt>32) ? (U32)(fOrig<<(iExpNt-32)) : (U32)(fOrig>>(32-iExpNt));

;   FRACT_BITS_LP_SPEC                               	EQU	25
;   FRACT_BITS_LP_SPEC_POWER                         	EQU	30

;	iExpNt=(iExp-(2 * FRACT_BITS_LP_SPEC - FRACT_BITS_LP_SPEC_POWER));   
;   iExpNt=iExp   - 20
;   iExp  =iExpNT + 20

; 623  :     // split into top INVQUADROOT_FRACTION_TABLE_LOG2_SIZE==8 bits for fractional lookup and bottom bits for interpolation
; 624  :     iMSF8 = uiMSF>>(32-INVQUADROOT_FRACTION_TABLE_LOG2_SIZE);
; 625  :     uiMSF <<= INVQUADROOT_FRACTION_TABLE_LOG2_SIZE;
; 626  :     // lookup and interpolate - tables are set up to return correct binary point for WeightType
; 627  :     uiFrac1  = g_InvQuadRootFraction[iMSF8++];
; 628  :     uiFrac1 -= MULT_HI_UDWORD( uiMSF, uiFrac1 - g_InvQuadRootFraction[iMSF8] );
; 629  :     return MULT_HI_UDWORD( uiFrac1, g_InvQuadRootExponent[ iExp ] );

;   r4  = high32(fOrig)
;   r12 = low32(fOrig)
;   r2 = uiMSF
;   r3 = iExp

    cmp       r3, #52                       ; iExp>52
    subgt     r1, r3, #52                   ; (iExp-52)
    movgt     r2, r12, lsl r1               ; (U32)(fOrig<<(iExp-52))
    bgt       |$L4103|
    rsb       r2, r3, #52                   ; 52-iExp
    mov       r12, r12, lsr r2              ; uiMSF>>(52-iExp)
    sub       r0, r3, #20                   ; iExp - 20
    orr       r2, r12, r4, lsl r0           ; (uiMSF>>(52-iExp)) | (uiMSF<<iExp)
|$L4103|

;   r0 = uiMSF
;   r1 = iMSF8
;   r3 = iExp
;   r4 = g_InvQuadRootFraction[iMSF8]
;   lr = uiFrac1

    ldr       r4, pg_InvQuadRootFraction    ; needed later, assume SA stalls here
    ;;; 1 cycle for armv6
    mov       r1, r2, lsr #24
    ldr       lr, [r4, +r1, lsl #2]!
    ldr       r4, [r4, #4]
    mov       r0, r2, lsl #8
    ;;; 1 cycle for armv6
    sub       r1, lr, r4
    ldr       r2, pg_InvQuadRootExponent
    UMULL     R12, R0, R1, R0    
    ldr       r1, [r2, +r3, lsl #2]
    ;;; 1cycle for armv6
    sub       r0, lr, r0
    ;;; 1cycle for armv6
    UMULL     R12, R0, R1, R0
    ldmia     sp!, {r4, pc}  ; ldmfd

pg_InvQuadRootExponent  DCD       |g_InvQuadRootExponent|
pg_InvQuadRootFraction  DCD       |g_InvQuadRootFraction|
pgLZLTable              DCD       |gLZLTable|

    ENTRY_END InverseQuadRootOfSumSquares

;=======================================================================================================================================
;***************************************************************************************************************************************
;=======================================================================================================================================
;
; extern void prvDoLpc4( const Int k, const LpSpecType* pTmp, WeightType* pWF, const Int iSizeBy2, const BP2Type S1, const BP2Type C1 );
;

r4pTmp      rn  r4
r4TA        rn  r4
r5S1        rn  r5
r5T9        rn  r5
r6k         rn  r6	;R6
r7S2        rn  r7
r7pWF       rn  r7
r8C1        rn  r8
r8T2        rn  r8
r9T8        rn  r9
r10CmS      rn  r10
r10T6       rn  r10
r11TB       rn  r11

offT7       equ 0x00
offT4       equ 0x04
offpWF      equ 0x10
offk        equ 0x14
iSizeBy2    equ 0x40
offS1       equ 0x44
offC1       equ 0x48

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  AREA    |.text|, CODE
  LEAF_ENTRY prvDoLpc4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; 778  : {

    mov       r12, sp
    stmdb     sp!, {r3}  ; stmfd
    stmdb     sp!, {r4 - r12, lr}  ; stmfd
    sub       sp, sp, #0x18  ; 0x18 = 24
|$M4111|

; 779  :     BP2Type    CmS, CpS;
; 780  :     LpSpecType T2, T4, T6, T7, T8, T9, TA, TB;
; 781  :     LpSpecType D, E;
; 783  :     BP2Type    C2, S2, C3, S3;
; 793  :     CmS = C1 - S1;                              // cnst4[i*6+4];

    ldr       r8C1, [sp, #offC1]                    ; C1
    mov       r4pTmp, r1
    ldr       r5S1, [sp, #offS1]                    ; S1
    str       r0, [sp, #offk]
    str       r3, [sp, #iSizeBy2]                   ; iSizeBy2

; 794  :     CpS = C1 + S1;                              // cnst4[i*6+5];
; 796  :     T8 = MULT_BP2X(CmS,pTmp[2]) + MULT_BP2X(CpS,pTmp[3]);     // F[2048+j]    F[3072-j]

	IF ARMVERSION >= 6
	
	ldrd		r0, [r4pTmp, #8]					; pTmp[2], pTmp[3]
	sub			r10CmS, r8C1, r5S1					; CmS = C1 - S1
	add			r11, r5S1, r8C1						; CpS = C1 + S1 
	str			r2, [sp, #offpWF]
	SMULL		R12, R3, R0, r10CmS					; MULT_BP2X(CmS,pTmp[2])
	SMMUL		r7S2, r5S1, r8C1					; MULT_BP2X(C1,S1)
	SMLAL		R12, R3, R1, R11					; + MULT_BP2X(CpS,pTmp[3])
	rsb			r2, r10CmS, #0						; -CmS
	SMULL		R12, R10, R0, R11					; MULT_BP2X(CpS,pTmp[2])
	MOV			r9T8,  r3, LSL #2					; T8 = MULT_BP2X(CmS,pTmp[2]) + MULT_BP2X(CpS,pTmp[3])
	mvn			lr, #3, 2							; BP2_FROM_FLOAT(1.0f) = 0x3FFFFFFF = ~0xC0000000
	SMLAL		R12, R10, R1, r2					; MULT_BP2X(CpS,pTmp[2]) - MULT_BP2X(CmS,pTmp[3])
	mov			r7S2, r7S2, lsl #3					; S2 = MUL2(MULT_BP2X(C1,S1))
	SMULL		R12, R3, r5S1, r5S1
	ldrd		r0, [r4pTmp]						; pTmp[0], pTmp[1]
	mov			r10T6, r10, lsl #2					; T6 = MULT_BP2X(CpS,pTmp[2]) - MULT_BP2X(CmS,pTmp[3])
	sub			lr, lr, r3, lsl #3					; C2 = BP2_FROM_FLOAT(1.0f) - MUL2(MULT_BP2X(S1,S1))
	sub			r2, lr, r7S2						; CmS = C2 - S2    
	add			r11, lr, r7S2						; CpS = C2 + S2
	SMULL		R12, R3, R0, R2						; MULT_BP2X(CmS,pTmp[0])
	rsb			r2, r2, #0							; -CmS
	SMULL		R0, R6, R11, r0						; MULT_BP2X(CpS,pTmp[0])
											;;; stall - issue = 1 cycles
	SMLAL		R12, R3, R1, R11					; MULT_BP2X(CpS,pTmp[1])
											;;; stall - issue = 1 cycles 
	SMLAL		R0, R6, R1, R2						; - MULT_BP2X(CmS,pTmp[1])
	SMULL		R12, R11, lr, r5S1					; MULT_BP2X(S1,C2)
	MOV			r0,  r3, LSL #2						; MULT_BP2X(CmS,pTmp[0]) + MULT_BP2X(CpS,pTmp[1])
	str			r0, [sp, #offT7]					; T7 = MULT_BP2X(CmS,pTmp[0]) + MULT_BP2X(CpS,pTmp[1])
	SMLAL		R12, R11, r7S2, r8C1				; MULT_BP2X(C1,S2)
	rsb			r2, r5S1, #0						; -S1
	SMULL		R1, R0, lr, r8C1					; MULT_BP2X(C1,C2)
	MOV			r6,  r6, LSL #2						; MULT_BP2X(CpS,pTmp[0]) - MULT_BP2X(CmS,pTmp[1])
	mov			r11, r11, lsl #2					; S3 = MULT_BP2X(S1,C2) + MULT_BP2X(C1,S2)
	SMLAL		R1, R0, r7S2, R2					; - MULT_BP2X(S1,S2)
	ldrd		r2, [r4pTmp, #0x10]					; pTmp[4], pTmp[5]
	rsb			r8, r11, r0, LSL #2					; CmS = C3 - S3        
	add			r11, r11, r0, LSL #2				; CpS = C3 + S3
	SMULL		R12, r5T9, R2, R8					; MULT_BP2X(CmS,pTmp[4])
	str			r6, [sp, #offT4]					; T4 = MULT_BP2X(CpS,pTmp[0]) - MULT_BP2X(CmS,pTmp[1]) 
	rsb			r8, r8, #0
	SMLAL		R12, r5T9, R3, R11					; + MULT_BP2X(CpS,pTmp[5])
	ldr			r7pWF, [sp, #offpWF]
	SMULL		R1, R0, R2, R11						; MULT_BP2X(CpS,pTmp[4])
	mov			r5T9, r5T9, lsl #2					; T9 = MULT_BP2X(CmS,pTmp[4]) + MULT_BP2X(CpS,pTmp[5])
	ldr			lr, [sp, #offT4]					; T4 
	SMLAL		R1, R0, R3, R8						; - MULT_BP2X(CmS,pTmp[5])
	ldrd		r2, [r4pTmp, #0x18]					; pTmp[6], pTmp[7]
	ldr			r1, [sp, #offT7]					; T7
	MOV			r8T2,  r0, LSL #2					; T2 = MULT_BP2X(CpS,pTmp[4]) - MULT_BP2X(CmS,pTmp[5])
	sub			r11TB, r2, r3						; TB = pTmp[6] - pTmp[7]
	add			r4TA, r2, r3						; TA = pTmp[6] + pTmp[7]
	add			r2, r11TB, r8T2						; E  = T2 + TB
	add			r2, r2, lr							; E += T4
	add			r0, r4TA, r5T9						; D  = T9 + TA
	add			r2, r2, r10T6						; E += T6
	add			r0, r0, r1							; D += T7
	mov			r1, r2, asr #1						; E  = DIV2(+ T4  + T6 + T2 + TB)
	
	ELSE
	
    ldr       r1, [r4pTmp, #8]                      ; pTmp[2]
    sub       r10CmS, r8C1, r5S1                    ; CmS = C1 - S1
    ldr       r3, [r4pTmp, #0xC]                    ; pTmp[3]
    SMULL     R12, R0, R1, r10CmS                   ; MULT_BP2X(CmS,pTmp[2])
    add       r11, r5S1, r8C1                       ; CpS = C1 + S1       
    str       r2, [sp, #offpWF]    
    SMLAL     R12, R0, R3, r11                      ; + MULT_BP2X(CpS,pTmp[3])

    rsb       r2, r10CmS, #0                        ; -CmS
    MOV       r9T8,  r0, LSL #2                     ; T8 = MULT_BP2X(CmS,pTmp[2]) + MULT_BP2X(CpS,pTmp[3])

; 797  :     T6 = MULT_BP2X(CpS,pTmp[2]) - MULT_BP2X(CmS,pTmp[3]);     // F[2048+j]    F[3072-j]


    SMULL     R12, R0, R1, R11                      ; MULT_BP2X(CpS,pTmp[2])
    SMULL     r1, r7S2, r5S1, r8C1                  ; MULT_BP2X(C1,S1)
    SMLAL     R12, R0, R3, r2                       ; MULT_BP2X(CpS,pTmp[2]) - MULT_BP2X(CmS,pTmp[3])

; 799  :     S2 = MUL2(MULT_BP2X(C1,S1));                        // sin(2x)
    
    mvn       lr, #3, 2                             ; BP2_FROM_FLOAT(1.0f) = 0x3FFFFFFF = ~0xC0000000
    mov       r7S2, r7S2, lsl #3                    ; S2 = MUL2(MULT_BP2X(C1,S1))
    mov       r10T6, r0, lsl #2                     ; T6 = MULT_BP2X(CpS,pTmp[2]) - MULT_BP2X(CmS,pTmp[3])

; 800  :     C2 = BP2_FROM_FLOAT(1.0f) - MUL2(MULT_BP2X(S1,S1)); // cos(2x)

    SMULL     R12, R0, r5S1, r5S1
    
    ldr       r3, [r4pTmp]                          ; pTmp[0]
    ldr       r1, [r4pTmp, #4]                      ; pTmp[1]
    sub       lr, lr, r0, lsl #3                    ; C2 = BP2_FROM_FLOAT(1.0f) - MUL2(MULT_BP2X(S1,S1))

; 801  :     CmS = C2 - S2;                              // cnst4[i*6+2];
; 802  :     CpS = C2 + S2;                              // cnst4[i*6+3];

    
    sub       r2, lr, r7S2                          ; CmS = C2 - S2    
    add       r11, lr, r7S2                         ; CpS = C2 + S2

; 803  :     T7 = MULT_BP2X(CmS,pTmp[0]) + MULT_BP2X(CpS,pTmp[1]);     // F[1024+j]    F[2048-j]

    SMULL     R12, R0, R3, R2                       ; MULT_BP2X(CmS,pTmp[0])
    rsb       r2, r2, #0                            ; -CmS
    SMULL     R3, R6, R11, r3                       ; MULT_BP2X(CpS,pTmp[0])
    ;;; stall - issue = 1 cycles
    SMLAL     R12, R0, R1, R11                      ; MULT_BP2X(CpS,pTmp[1])
    ;;; stall - issue = 1 cycles    

; 804  :     T4 = MULT_BP2X(CpS,pTmp[0]) - MULT_BP2X(CmS,pTmp[1]);     // F[1024+j]    F[2048-j]
    
    SMLAL     R3, R6, R1, R2                        ; - MULT_BP2X(CmS,pTmp[1])
    

; 805  : 
; 806  :     S3 = MULT_BP2X(S1,C2) + MULT_BP2X(C1,S2);   // sin(3x) = sin(x+2x)

    SMULL     R12, R11, lr, r5S1                    ; MULT_BP2X(S1,C2)
    MOV       r0,  r0, LSL #2                       ; MULT_BP2X(CmS,pTmp[0]) + MULT_BP2X(CpS,pTmp[1])
    str       r0, [sp, #offT7]                      ; T7 = MULT_BP2X(CmS,pTmp[0]) + MULT_BP2X(CpS,pTmp[1])
    SMLAL     R12, R11, r7S2, r8C1                  ; MULT_BP2X(C1,S2)
    rsb       r2, r5S1, #0                          ; -S1
    SMULL     R3, R0, lr, r8C1                      ; MULT_BP2X(C1,C2)
    MOV       r6,  r6, LSL #2                       ; MULT_BP2X(CpS,pTmp[0]) - MULT_BP2X(CmS,pTmp[1])
    mov       r11, r11, lsl #2                      ; S3 = MULT_BP2X(S1,C2) + MULT_BP2X(C1,S2)

; 807  :     C3 = MULT_BP2X(C1,C2) - MULT_BP2X(S1,S2);   // cos(2x) = cos(x+2x)    

    SMLAL     R3, R0, r7S2, R2                      ; - MULT_BP2X(S1,S2)
	
	
; 808  :     CmS = C3 - S3;                              // old cnst4[i*6];
; 809  :     CpS = C3 + S3;                              // old cnst4[i*6+1];
; 810  :     T9 = MULT_BP2X(CmS,pTmp[4]) + MULT_BP2X(CpS,pTmp[5]);     // F[3072+j]    F[4096-j]

    ldr       r1, [r4pTmp, #0x10]                   ; pTmp[4]
    ldr       r2, [r4pTmp, #0x14]                   ; pTmp[5]
    rsb       r8, r11, r0, LSL #2                   ; CmS = C3 - S3        
    add       r11, r11, r0, LSL #2                  ; CpS = C3 + S3

    SMULL     R12, r5T9, R1, R8                     ; MULT_BP2X(CmS,pTmp[4])
    str       r6, [sp, #offT4]                      ; T4 = MULT_BP2X(CpS,pTmp[0]) - MULT_BP2X(CmS,pTmp[1]) 
    rsb       r8, r8, #0
    SMLAL     R12, r5T9, R2, R11                    ; + MULT_BP2X(CpS,pTmp[5])
    
; 811  :     T2 = MULT_BP2X(CpS,pTmp[4]) - MULT_BP2X(CmS,pTmp[5]);     // F[3072+j]    F[4096-j]
    ldr       r7pWF, [sp, #offpWF]
    SMULL     R3, R0, R1, R11                       ; MULT_BP2X(CpS,pTmp[4])
    mov       r5T9, r5T9, lsl #2                    ; T9 = MULT_BP2X(CmS,pTmp[4]) + MULT_BP2X(CpS,pTmp[5])
    ldr       r1, [r4pTmp, #0x18]                   ; pTmp[6]
    SMLAL     R3, R0, R2, R8                        ; - MULT_BP2X(CmS,pTmp[5])
    ;;; stall - issue = 1 cycles
    ldr       lr, [sp, #offT4]                      ; T4    
    MOV       r8T2,  r0, LSL #2                     ; T2 = MULT_BP2X(CpS,pTmp[4]) - MULT_BP2X(CmS,pTmp[5])

; 813  :     TA = pTmp[6] + pTmp[7];                                   // F[j]      +  F[1024-j];
; 814  :     TB = pTmp[6] - pTmp[7];                                   // F[j]      -  F[1024-j];

    ldr       r0, [r4pTmp, #0x1C]                   ; pTmp[7]    
    ;;;2 cycles for armv6 and 1cycle for armv4
    sub       r11TB, r1, r0                         ; TB = pTmp[6] - pTmp[7]
    add       r4TA, r1, r0                          ; TA = pTmp[6] + pTmp[7]

; 816  :     D  = DIV2(+ T7  + T8 + T9 + TA);
; 817  :     E  = DIV2(+ T4  + T6 + T2 + TB);
; 818  :     pWF[k]           = InverseQuadRootOfSumSquares(D, E);              // F[j]

    add       r1, r11TB, r8T2                       ; E  = T2 + TB
    add       r1, r1, lr                            ; E += T4
    ldr       lr, [sp, #offT7]                      ; T7
    add       r0, r4TA, r5T9                        ; D  = T9 + TA
    add       r1, r1, r10T6                         ; E += T6
    add       r0, r0, lr                            ; D += T7
    mov       r1, r1, asr #1                        ; E  = DIV2(+ T4  + T6 + T2 + TB)
    
    ENDIF						;ARMVERSION
    
    add       r0, r0, r9T8                          ; D += T8
    mov       r0, r0, asr #1                        ; D  = DIV2(+ T7  + T8 + T9 + TA)
    bl        InverseQuadRootOfSumSquares           ; InverseQuadRootOfSumSquares(D, E)
    ldr       r6k, [sp, #offk]
    ldr       r1, [sp, #offT4]                      ; T4
    ;;;1 cycle for armv6
    str       r0, [r7pWF, +r6k, lsl #2]             ; pWF[k] = InverseQuadRootOfSumSquares(D, E)

; 820  :     D  = DIV2(- T7  + T6 - T2 + TA);
; 821  :     E  = DIV2(+ T4  + T8 - T9 - TB);
; 822  :     pWF[iSizeBy2-k]  = InverseQuadRootOfSumSquares(D, E);              // F[1024-j]

    sub       r0, r4TA, r8T2                        ; D  = - T2 + TA
    sub       r1, r1, r11TB                         ; E  = T4 - TB
    ldr       r2, [sp, #offT7]                      ; T7
    sub       r1, r1, r5T9                          ; E -= T9    
    add       r1, r1, r9T8                          ; E += T8
    sub       r0, r0, r2                            ; D -= T7
    mov       r1, r1, asr #1                        ; E  = DIV2(+ T4  + T8 - T9 - TB)
    add       r0, r0, r10T6                         ; D += T6
    mov       r0, r0, asr #1                        ; D  = DIV2(- T7  + T6 - T2 + TA)
    bl        InverseQuadRootOfSumSquares           ; InverseQuadRootOfSumSquares(D, E)
    ldr       r1, [sp, #iSizeBy2]
    ldr       lr, [sp, #offT4]                      ; T4
    ;;;1 cycle for armv6 
    sub       r1, r1, r6k
    str       r0, [r7pWF, +r1, lsl #2]              ; pWF[iSizeBy2-k] = InverseQuadRootOfSumSquares(D, E)

; 824  :     D  = DIV2(- T7  - T6 + T2 + TA);
; 825  :     E  = DIV2(- T4  + T8 - T9 + TB);
; 826  :     pWF[iSizeBy2+k]  = InverseQuadRootOfSumSquares(D, E);              // F[1024+j]

    sub       r1, r11TB, r5T9                       ; E  = - T9 + TB
    ldr       r0, [sp, #offT7]                      ; T7
    sub       r1, r1, lr                            ; E -= T4    
    add       r1, r1, r9T8                          ; E += T8
    sub       r0, r4TA, r0                          ; D  = - T7 + TA
    sub       r0, r0, r10T6                         ; D += T6
    mov       r1, r1, asr #1                        ; E  = DIV2(- T4  + T8 - T9 + TB)
    add       r0, r0, r8T2                          ; D += T2
    mov       r0, r0, asr #1                        ; D  = DIV2(- T7  - T6 + T2 + TA)
    bl        InverseQuadRootOfSumSquares           ; InverseQuadRootOfSumSquares(D, E)
    ldr       r2, [sp, #iSizeBy2]
    ldr       r3, [sp, #offT4]                      ; T4
    sub       r1, r4TA, r5T9                        ; E  = - T9 + TA
    add       lr, r6k, r2
    str       r0, [r7pWF, +lr, lsl #2]              ; pWF[iSizeBy2+k] = InverseQuadRootOfSumSquares(D, E)

; 828  :     D  = DIV2(- T4  + T6 + T2 - TB);
; 829  :     E  = DIV2(+ T7  - T8 - T9 + TA);
; 830  :     pWF[(iSizeBy2<<1)-k]  = InverseQuadRootOfSumSquares(D, E);         // F[2048-j]    

    ldr       r2, [sp, #offT7]                      ; T7
    sub       r1, r1, r9T8                          ; E -= T8    
    sub       r0, r8T2, r11TB                       ; D  = T2 - TB
    add       r1, r2, r1                            ; E += T7
    sub       r0, r0, r3                            ; D -= T4
    mov       r1, r1, asr #1                        ; E  = DIV2(+ T7  - T8 - T9 + TA)
    add       r0, r0, r10T6                         ; D += T6
    mov       r0, r0, asr #1                        ; D  = DIV2(- T4  + T6 + T2 - TB)
    bl        InverseQuadRootOfSumSquares           ; InverseQuadRootOfSumSquares(D, E)
    ldr       lr, [sp, #iSizeBy2]
    add       sp, sp, #0x18
    ;;;1 cycle for armv6
    rsb       r3, r6k, lr, lsl #1
    str       r0, [r7pWF, +r3, lsl #2]              ; pWF[(iSizeBy2<<1)-k]  = InverseQuadRootOfSumSquares(D, E)

; 834  :     //** This block uses 36 adds and 20 mults plus either 4 more mults or 4 shifts

    ldmia     sp, {r4 - r11, sp, pc}  ; ldmfd
|$M4112|
; 856  : }

    ENTRY_END prvDoLpc4
	
  ENDIF ; WMA_OPT_LPCSTD_ARM
    
  END