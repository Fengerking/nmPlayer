;//*@@@+++@@@@******************************************************************
;//
;// Microsoft Windows Media
;// Copyright (C) Microsoft Corporation. All rights reserved.
;//
;//*@@@---@@@@******************************************************************
;// Module Name:
;//
;//     msaudiostdpro_arm.s
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
;//     Jerry He (yamihe) Jan 16, 2004
;//
;// Revision History:
;//
;//     For more information on ARM assembler directives, use
;//        http://msdn.microsoft.com/library/default.asp?url=/library/en-us/wcechp40/html/ccconarmassemblerdirectives.asp
;//*************************************************************************


  OPT         2       ; disable listing 
  INCLUDE     kxarm.h
  INCLUDE     ../wma_member_arm.inc
  INCLUDE	  ../wma_arm_version.h
  OPT         1       ; enable listing
 
  AREA    |.text|, CODE, READONLY

  IF WMA_OPT_SUBFRAMERECON_ARM = 1
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  EXPORT  auSubframeRecon_Std
  EXPORT  auSubframeRecon_MonoPro


;//*************************************************************************************
;//
;// WMARESULT auSubframeRecon_Std ( CAudioObject* pau, PerChannelInfo* ppcinfo, PerChannelInfo* ppcinfo2)
;//
;//*************************************************************************************

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Registers for auSubframeRecon_Std
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

piCurrCoef        RN  4
piPrevCoef        RN  5

piCurrCoef2       RN  12
piPrevCoef2       RN  14

i                 RN  3
iOverlapSize      RN  6
iShift            RN  3
bp2Step           RN  7

iSizeCurr         RN  8
iSizePrev         RN  9

temp1             RN  6
temp2             RN  7
temp3             RN  1

bp2Sin            RN  8
bp2Cos            RN  9
bp2Sin1           RN  2
bp2Cos1           RN  0

bp2SinT           RN  10
bp2CosT           RN  11

cfPrevData        RN  10
cfCurrData        RN  11

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Constants for auSubframeRecon_Std
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
iStackSpaceRevStd		EQU 4*4    
iOffset_coefShift		EQU iStackSpaceRevStd-4
iOffset_bp2Step			EQU iStackSpaceRevStd-8
iOffset_iOverlapSize	EQU iStackSpaceRevStd-12
iOffset_iSizeCurr		EQU iStackSpaceRevStd-16
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  AREA    |.text|, CODE
  LEAF_ENTRY auSubframeRecon_Std
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Input parameters
; r0 = pau
; r1 = ppcinfo
; r2 = ppcinfo2


  STMFD sp!, {r4 - r11, lr}
  SUB   sp, sp, #iStackSpaceRevStd ; rev stack space

; iSizeCurr = ppcinfo->m_iSizeCurr;
  LDRSH iSizeCurr, [r1, #PerChannelInfo_m_iSizeCurr]

; piCurrCoef = (CoefType *)ppcinfo->m_rgiCoefRecon;   // now reverse the coef buffer
  LDR   piCurrCoef, [r1, #PerChannelInfo_m_rgiCoefRecon]
  
; piCurrCoef2 = (CoefType *)ppcinfo2->m_rgiCoefRecon;   // now reverse the coef buffer
  LDR   piCurrCoef2, [r2, #PerChannelInfo_m_rgiCoefRecon]

  MOV   temp1, piCurrCoef
  MOV   temp2, piCurrCoef2

  STR	iSizeCurr, [sp, #iOffset_iSizeCurr]
  
; for(i = 0; i < iSizeCurr >> 1; i++) {
  MOV   i, iSizeCurr, ASR #2                        ; i = iSizeCurr/4
  ADD   piPrevCoef, piCurrCoef, iSizeCurr, LSL #2
  ADD   piPrevCoef2, piCurrCoef2, iSizeCurr, LSL #2
  SUB   piPrevCoef, piPrevCoef, #8                 ; piPrevCoef = &piCurrCoef[iSizeCurr - 2]
  SUB   piPrevCoef2, piPrevCoef2, #8               ; piPrevCoef2 = &piCurrCoef2[iSizeCurr - 2]

LoopMemMove
; cfCurrData = piCurrCoef[i];
; piCurrCoef[i] = piCurrCoef[iSizeCurr - 1 - i];
; piCurrCoef[iSizeCurr - 1 - i] = cfCurrData;

; cfCurrData = piCurrCoef2[i];
; piCurrCoef2[i] = piCurrCoef2[iSizeCurr - 1 - i];
; piCurrCoef2[iSizeCurr - 1 - i] = cfCurrData;

	LDRD	r8, [piPrevCoef]
	LDRD	r10, [piCurrCoef]
	SUBS	i, i, #1
	STR		r9, [piCurrCoef], #4
	STR		r8, [piCurrCoef], #4
	LDRD	r8, [piPrevCoef2]
	STR		r10, [piPrevCoef, #4]
	STR		r11, [piPrevCoef], #-8
	LDRD	r10, [piCurrCoef2]
	STR		r9, [piCurrCoef2], #4
	STR		r8, [piCurrCoef2], #4
	STR		r10, [piPrevCoef2, #4]
	STR		r11, [piPrevCoef2], #-8
  
	BNE   LoopMemMove

; // ---- Setup the coef buffer pointer ----
; piPrevCoef = (CoefType *)ppcinfo->m_rgiCoefRecon - iSizePrev / 2;       // go forward
; piCurrCoef = (CoefType *)ppcinfo->m_rgiCoefRecon + iSizeCurr / 2 - 1;   // go backward
  
  LDRSH iSizePrev, [r1, #PerChannelInfo_m_iSizePrev]
  LDR	iSizeCurr, [sp, #iOffset_iSizeCurr]
  
  
  MOV   i, iSizeCurr, ASR #1                        ; i = iSizeCurr/2	
  ANDS	i, i, #1
  BEQ	i_is_even
;the last odd reverse
  ADD	piPrevCoef, piPrevCoef, #4
  LDR   cfPrevData, [piPrevCoef]
  LDR   cfCurrData, [piCurrCoef]
  STR   cfPrevData, [piCurrCoef], #4
  STR   cfCurrData, [piPrevCoef], #-4

; cfCurrData = piCurrCoef2[i];
; piCurrCoef2[i] = piCurrCoef2[iSizeCurr - 1 - i];
; piCurrCoef2[iSizeCurr - 1 - i] = cfCurrData;
  ADD	piPrevCoef2, piPrevCoef2, #4
  LDR   cfPrevData, [piPrevCoef2]
  LDR   cfCurrData, [piCurrCoef2]
  STR   cfPrevData, [piCurrCoef2], #4
  STR   cfCurrData, [piPrevCoef2], #-4
  
  MOV   piCurrCoef, piPrevCoef
  SUB   piPrevCoef, temp1, iSizePrev, LSL #1    
  MOV   piCurrCoef2, piPrevCoef2  
  BNE	i_is_odd
i_is_even   
; piPrevCoef2 = (CoefType *)ppcinfo2->m_rgiCoefRecon - iSizePrev / 2;       // go forward
; piCurrCoef2 = (CoefType *)ppcinfo2->m_rgiCoefRecon + iSizeCurr / 2 - 1;   // go backward
  ADD   piCurrCoef, piPrevCoef, #4
  SUB   piPrevCoef, temp1, iSizePrev, LSL #1
  ADD   piCurrCoef2, piPrevCoef2, #4  
  
i_is_odd
; piCurrCoef2 = (CoefType *)ppcinfo2->m_rgiCoefRecon + iSizeCurr / 2 - 1;   // go backward
  SUB   piPrevCoef2, temp2, iSizePrev, LSL #1
  
; if ((pau->m_bUnifiedPureLLMCurrFrm == WMAB_TRUE) && (pau->m_bFirstUnifiedPureLLMFrm == WMAB_FALSE))
;       return WMA_OK; 
  LDR   temp1, [r0, #CAudioObject_m_bUnifiedPureLLMCurrFrm]
  LDR   temp2, [r0, #CAudioObject_m_bFirstUnifiedPureLLMFrm]
  CMP   temp1, #1
  BNE   ResetBuffer

  CMP   temp2, #0
  BEQ   auSubframeRecon_StdExit

ResetBuffer
; if( iSizeCurr > iSizePrev ){
  CMP   iSizeCurr, iSizePrev
  BLE   ResetBufferLE

; piCurrCoef -= (iSizeCurr - iSizePrev) >> 1;
; piCurrCoef2 -= (iSizeCurr - iSizePrev) >> 1;
  SUB   temp1, iSizeCurr, iSizePrev
  SUB   piCurrCoef, piCurrCoef, temp1, LSL #1
  SUB   piCurrCoef2, piCurrCoef2, temp1, LSL #1

; iOverlapSize = iSizePrev >> 1;
  MOV   iOverlapSize, iSizePrev, ASR #1
  B     Overlap

ResetBufferLE
; if (iSizeCurr < iSizePrev)
  BEQ   ResetBufferEQ

; piPrevCoef += ((iSizePrev - iSizeCurr) >> 1);
; piPrevCoef2 += ((iSizePrev - iSizeCurr) >> 1);
  SUB   temp1, iSizePrev, iSizeCurr
  ADD   piPrevCoef, piPrevCoef, temp1, LSL #1
  ADD   piPrevCoef2, piPrevCoef2, temp1, LSL #1

ResetBufferEQ
; iOverlapSize = iSizeCurr >> 1;
  MOV   iOverlapSize, iSizeCurr, ASR #1

Overlap
	LDR   temp2, [r0, #CAudioObject_m_cLeftShiftBitsFixedPost] ; coefShift = pau->m_cLeftShiftBitsFixedPost

	LDRD   bp2Sin, [r1, #PerChannelInfo_m_fiSinRampUpStart]	; bp2Sin  = ppcinfo->m_fiSinRampUpStart;
  															; bp2Cos  = ppcinfo->m_fiCosRampUpStart;
	STR   temp2, [sp, #iOffset_coefShift]

; bp2Step = ppcinfo->m_fiSinRampUpStep;
	LDR   bp2Step,  [r1, #PerChannelInfo_m_fiSinRampUpStep]

; bp2Sin1 = ppcinfo->m_fiSinRampUpPrior;
	LDR   bp2Sin1, [r1, #PerChannelInfo_m_fiSinRampUpPrior]
	LDR   cfPrevData, [piPrevCoef]
	MOV   bp2Step, bp2Step, LSL #2
	STR   bp2Step, [sp, #iOffset_bp2Step]
	LDR   cfCurrData, [piCurrCoef]				; cfCurrData = *piCurrCoef;
; bp2Cos1 = ppcinfo->m_fiCosRampUpPrior;
	LDR   bp2Cos1, [r1, #PerChannelInfo_m_fiCosRampUpPrior]
	MOV   cfPrevData, cfPrevData, LSL #2
LOOPOverlap
; for(i = 0; i < iOverlapSize; i++ ){

; cfPrevData = *piPrevCoef;
; cfCurrData = *piCurrCoef;
; *piPrevCoef++ = INT_FROM_COEF( MULT_BP2(-bp2Sin, cfCurrData) + MULT_BP2(bp2Cos, cfPrevData) );
; *piCurrCoef-- = INT_FROM_COEF( MULT_BP2(bp2Sin, cfPrevData) + MULT_BP2(bp2Cos, cfCurrData) );
	SMMUL temp2, bp2Cos, cfPrevData				; MULT_BP2(bp2Cos, cfPrevData)
	MOV   cfCurrData, cfCurrData, LSL #2

	LDR   iShift, [sp, #iOffset_coefShift]
	SMMLS temp2, bp2Sin, cfCurrData, temp2 

	SMMUL temp3, bp2Cos, cfCurrData    ; MULT_BP2(bp2Cos, cfCurrData)
	MOV   temp2, temp2, ASR iShift            ; temp2 = INT_FROM_COEF( MULT_BP2(-bp2Sin, cfCurrData) + MULT_BP2(bp2Cos, cfPrevData) )

	SMMLA temp3, bp2Sin, cfPrevData, temp3
	LDR   cfPrevData, [piPrevCoef2]
	LDR   cfCurrData, [piCurrCoef2]           ; cfCurrData = *piCurrCoef2;	
	STR   temp2, [piPrevCoef], #4  
  
	MOV   cfPrevData, cfPrevData, LSL #2
	MOV   cfCurrData, cfCurrData, LSL #2
; cfPrevData = *piPrevCoef2;
; cfCurrData = *piCurrCoef2;
    
; *piPrevCoef2++ = INT_FROM_COEF( MULT_BP2(-bp2Sin, cfCurrData) + MULT_BP2(bp2Cos, cfPrevData) );
; *piCurrCoef2-- = INT_FROM_COEF( MULT_BP2(bp2Sin, cfPrevData) + MULT_BP2(bp2Cos, cfCurrData) );

	SMMUL temp2, bp2Cos, cfPrevData    ; MULT_BP2(bp2Cos, cfPrevData)
  
	MOV   temp3, temp3, ASR iShift            ; temp3 = INT_FROM_COEF( MULT_BP2(bp2Sin, cfPrevData) + MULT_BP2(bp2Cos, cfCurrData) );

	STR   temp3, [piCurrCoef], #-4

	SMMLS temp2, bp2Sin, cfCurrData, temp2
  
	SMMUL temp3, bp2Cos, cfCurrData    ; MULT_BP2(bp2Cos, cfCurrData)
	MOV   temp2, temp2, ASR iShift            ; temp2 = INT_FROM_COEF( MULT_BP2(-bp2Sin, cfCurrData) + MULT_BP2(bp2Cos, cfPrevData) )


	SMMLA temp3, bp2Sin, cfPrevData, temp3
  
	STR   temp2, [piPrevCoef2], #4  
	LDR   bp2Step, [sp, #iOffset_bp2Step]
; bp2SinT = bp2Sin1 + MULT_BP2(bp2Step,bp2Cos);
; bp2CosT = bp2Cos1 - MULT_BP2(bp2Step,bp2Sin);
; bp2Sin1 = bp2Sin;  bp2Sin = bp2SinT;
; bp2Cos1 = bp2Cos;  bp2Cos = bp2CosT;
	MOV   temp3, temp3, ASR iShift            ; temp3 = INT_FROM_COEF( MULT_BP2(bp2Sin, cfPrevData) + MULT_BP2(bp2Cos, cfCurrData) )
	STR   temp3, [piCurrCoef2], #-4  
  
	SMMLA bp2SinT, bp2Step, bp2Cos, bp2Sin1
	SUBS  iOverlapSize, iOverlapSize, #1
	SMMLSR bp2CosT, bp2Step, bp2Sin, bp2Cos1
	MOV   bp2Sin1, bp2Sin
	MOV   bp2Sin, bp2SinT

	LDR   cfPrevData, [piPrevCoef]
	MOV   bp2Cos1, bp2Cos
	MOV   bp2Cos, bp2CosT   
	LDR   cfCurrData, [piCurrCoef]				; cfCurrData = *piCurrCoef;
	MOV   cfPrevData, cfPrevData, LSL #2
	BNE   LOOPOverlap

auSubframeRecon_StdExit
  ADD   sp, sp, #iStackSpaceRevStd     ; give back rev stack space  
  LDMFD sp!, {r4 - r11, PC} ;auSubframeRecon_Std
  ENTRY_END auSubframeRecon_Std



;//*************************************************************************************
;//
;// WMARESULT WMARESULT auSubframeRecon_MonoPro ( CAudioObject* pau )
;//
;//*************************************************************************************

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Registers for auSubframeRecon_MonoPro
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

pau						RN  0
iShift_pro				RN  0

ppcinfo					RN  1
cfPrevData_pro			RN  2
cfPrevData_pro_next		RN	3
cfCurrData_pro			RN  10
cfCurrData_pro_next		RN	11

piCurrCoef_pro			RN  4
piPrevCoef_pro			RN  5

bp2Sin_pro				RN  6
bp2Cos_pro				RN  7
bp2Sin1_pro				RN  8
bp2Cos1_pro				RN  9
bp2Step_pro				RN  12

t1						RN  14
t2						RN  11
t3						RN  1
iOverlapSize_pro		RN  3

iChSrc					RN  6
cChInTile_pro			RN  7
iCh_pro					RN  8
i_pro					RN  8

iSizeCurr_pro			RN  9
iSizePrev_pro			RN  12

bp2SinT_pro				RN  2
bp2CosT_pro				RN  10


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Constants for auSubframeRecon_MonoPro
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
iStackSpaceRevPro        EQU 4*4    
iOffset_coefShiftPro     EQU iStackSpaceRevPro-4
iOffset_cChInTilePro     EQU iStackSpaceRevPro-8
iOffset_iChPro           EQU iStackSpaceRevPro-12
iOffset_pauPro           EQU iStackSpaceRevPro-16

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  AREA    |.text|, CODE
  LEAF_ENTRY auSubframeRecon_MonoPro
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Input parameters
; r0 = pau


  STMFD sp!, {r4 - r11, lr}
  SUB   sp, sp, #iStackSpaceRevPro ; rev stack space

; iCh = pau->m_cChInTile
  ADD   t1, pau, #CAudioObject_m_cChInTile
  LDRSH cChInTile_pro, [t1]

  MOV   iCh_pro, #0
  LDR   t2, [pau, #CAudioObject_m_cLeftShiftBitsFixedPost] ; coefShift = pau->m_cLeftShiftBitsFixedPost

  STR   cChInTile_pro, [sp, #iOffset_cChInTilePro]
  STR   t2, [sp, #iOffset_coefShiftPro]
  STR   pau, [sp, #iOffset_pauPro]


; for (iCh = 0; iCh < pau->m_cChInTile; iCh++) {
LoopChannel

; iChSrc = pau->m_rgiChInTile [iCh];
  LDR   t1, [pau, #CAudioObject_m_rgiChInTile]
  ADD   t1, t1, iCh_pro, LSL #1
  LDRSH iChSrc, [t1]

; ppcinfo = pau->m_rgpcinfo + iChSrc;
  LDR   ppcinfo, [pau, #CAudioObject_m_rgpcinfo]

  MOV   t1, #PerChannelInfo_size
  MLA   ppcinfo, t1, iChSrc, ppcinfo

  ADD   iCh_pro, iCh_pro, #1
  STR   iCh_pro, [sp, #iOffset_iChPro]

; iSizePrev = ppcinfo->m_iSizePrev;
  LDRSH iSizePrev_pro, [ppcinfo, #PerChannelInfo_m_iSizePrev]

; iSizeCurr = ppcinfo->m_iSizeCurr;
  LDRSH iSizeCurr_pro, [ppcinfo, #PerChannelInfo_m_iSizeCurr]

; piCurrCoef = (CoefType *)ppcinfo->m_rgiCoefRecon;   // now reverse the coef buffer
  LDR   piCurrCoef_pro, [ppcinfo, #PerChannelInfo_m_rgiCoefRecon]
  
  MOV   t1, piCurrCoef_pro
  
  ; for(i = 0; i < iSizeCurr >> 2; i++) {
  MOV   i_pro, iSizeCurr_pro, ASR #2                            ; i = iSizeCurr/4
  ADD   piPrevCoef_pro, piCurrCoef_pro, iSizeCurr_pro, LSL #2
  SUB   piPrevCoef_pro, piPrevCoef_pro, #8                      ; piPrevCoef = &piCurrCoef[iSizeCurr - 2]
  
LoopMemMove_pro
  LDRD	cfCurrData_pro, [piCurrCoef_pro]
  LDRD	cfPrevData_pro, [piPrevCoef_pro]  
  SUBS  i_pro, i_pro, #1
  STR	cfCurrData_pro, [piPrevCoef_pro, #4]
  STR	cfCurrData_pro_next, [piPrevCoef_pro], #-8
  STR	cfPrevData_pro_next, [piCurrCoef_pro], #4
  STR	cfPrevData_pro, [piCurrCoef_pro], #4  
  BNE   LoopMemMove_pro
  
  MOV   i_pro, iSizeCurr_pro, ASR #1                            ; i = iSizeCurr/2
  ANDS	i_pro, i_pro, #1
  BEQ	i_pro_is_even
  
  ; cfCurrData = piCurrCoef[i];
; piCurrCoef[i] = piCurrCoef[iSizeCurr - 1 - i];
; piCurrCoef[iSizeCurr - 1 - i] = cfCurrData;
  ADD	piPrevCoef_pro, piPrevCoef_pro, #4
  LDR   cfPrevData_pro, [piPrevCoef_pro]
  LDR   cfCurrData_pro, [piCurrCoef_pro]
  STR   cfPrevData_pro, [piCurrCoef_pro], #4
  STR   cfCurrData_pro, [piPrevCoef_pro], #-4
  
  MOV   piCurrCoef_pro, piPrevCoef_pro
  SUB   piPrevCoef_pro, t1, iSizePrev_pro, LSL #1  
  B		i_pro_is_odd
  
i_pro_is_even  
  ADD	piCurrCoef_pro, piPrevCoef_pro, #4
  SUB   piPrevCoef_pro, t1, iSizePrev_pro, LSL #1  
i_pro_is_odd
   
; if ((pau->m_bUnifiedPureLLMCurrFrm == WMAB_TRUE) && (pau->m_bFirstUnifiedPureLLMFrm == WMAB_FALSE))
;       return WMA_OK; 
  LDR   t1, [pau, #CAudioObject_m_bUnifiedPureLLMCurrFrm]
  LDR   t2, [pau, #CAudioObject_m_bFirstUnifiedPureLLMFrm]
  CMP   t1, #1
  BNE   ResetBuffer_pro

  CMP   t2, #0
  BEQ   LoopChannelContinue

ResetBuffer_pro
; if( iSizeCurr > iSizePrev ){
  CMP   iSizeCurr_pro, iSizePrev_pro
  BLE   ResetBufferLE_pro

; piCurrCoef -= (iSizeCurr - iSizePrev) >> 1;
  SUB   t1, iSizeCurr_pro, iSizePrev_pro
  SUB   piCurrCoef_pro, piCurrCoef_pro, t1, LSL #1

; iOverlapSize = iSizePrev >> 1;
  MOV   iOverlapSize_pro, iSizePrev_pro, ASR #1
  B     Overlap_pro

ResetBufferLE_pro
; if (iSizeCurr < iSizePrev)
  BEQ   ResetBufferEQ_pro

; piPrevCoef += ((iSizePrev - iSizeCurr) >> 1);
  SUB   t1, iSizePrev_pro, iSizeCurr_pro
  ADD   piPrevCoef_pro, piPrevCoef_pro, t1, LSL #1

ResetBufferEQ_pro
; iOverlapSize = iSizeCurr >> 1;
  MOV   iOverlapSize_pro, iSizeCurr_pro, ASR #1

Overlap_pro

  LDRD   bp2Sin_pro, [ppcinfo, #PerChannelInfo_m_fiSinRampUpStart]	; bp2Sin  = ppcinfo->m_fiSinRampUpStart;
  																	; bp2Cos  = ppcinfo->m_fiCosRampUpStart;
	LDR   cfPrevData_pro, [piPrevCoef_pro]

  LDR   bp2Step_pro, [ppcinfo, #PerChannelInfo_m_fiSinRampUpStep]	; bp2Step = ppcinfo->m_fiSinRampUpStep;
	

  LDRD   bp2Sin1_pro, [ppcinfo, #PerChannelInfo_m_fiSinRampUpPrior]	; bp2Sin1 = ppcinfo->m_fiSinRampUpPrior;
																	; bp2Cos1 = ppcinfo->m_fiCosRampUpPrior;
	LDR   cfCurrData_pro, [piCurrCoef_pro]
	MOV   cfPrevData_pro, cfPrevData_pro, LSL #2 
  MOV   bp2Step_pro, bp2Step_pro, LSL #2


LOOPOverlap_pro
; for(i = 0; i < iOverlapSize; i++ ){


; cfCurrData = *piCurrCoef;
; *piPrevCoef++ = INT_FROM_COEF( MULT_BP2(-bp2Sin, cfCurrData) + MULT_BP2(bp2Cos, cfPrevData) );
; *piCurrCoef-- = INT_FROM_COEF( MULT_BP2(bp2Sin, cfPrevData) + MULT_BP2(bp2Cos, cfCurrData) );  
	 	
	SMMUL	t2, bp2Cos_pro, cfPrevData_pro  		; MULT_BP2(bp2Cos, cfPrevData)	
	MOV   cfCurrData_pro, cfCurrData_pro, LSL #2
	LDR   iShift_pro, [sp, #iOffset_coefShiftPro]
	SMMLS	t2, bp2Sin_pro, cfCurrData_pro, t2  

	SMMUL	t3, bp2Cos_pro, cfCurrData_pro  ; MULT_BP2(bp2Cos, cfCurrData)
	SUBS  iOverlapSize_pro, iOverlapSize_pro, #1
	MOV   t2, t2, ASR iShift_pro              ; t2 = INT_FROM_COEF( MULT_BP2(-bp2Sin, cfCurrData) + MULT_BP2(bp2Cos, cfPrevData) )

	SMMLA	t3, bp2Sin_pro, cfPrevData_pro, t3
	STR   t2, [piPrevCoef_pro], #4  	

; bp2SinT = bp2Sin1 + MULT_BP2(bp2Step,bp2Cos);
; bp2CosT = bp2Cos1 - MULT_BP2(bp2Step,bp2Sin);
; bp2Sin1 = bp2Sin;  bp2Sin = bp2SinT;
; bp2Cos1 = bp2Cos;  bp2Cos = bp2CosT;
  
	SMMLA	bp2SinT_pro, bp2Step_pro, bp2Cos_pro, bp2Sin1_pro
	MOV   t3, t3, ASR iShift_pro              ; t3 = INT_FROM_COEF( MULT_BP2(bp2Sin, cfPrevData) + MULT_BP2(bp2Cos, cfCurrData) )
	STR   t3, [piCurrCoef_pro], #-4  

	SMMLS	bp2CosT_pro, bp2Step_pro, bp2Sin_pro, bp2Cos1_pro
;	ADD   bp2SinT_pro, bp2Sin1_pro, t2
	MOV   bp2Sin1_pro, bp2Sin_pro
	MOV   bp2Sin_pro, bp2SinT_pro

	LDR   cfPrevData_pro, [piPrevCoef_pro]	; cfPrevData = *piPrevCoef;
;  SUB   bp2CosT_pro, bp2Cos1_pro, t3
	MOV   bp2Cos1_pro, bp2Cos_pro
	MOV   bp2Cos_pro, bp2CosT_pro   
	LDR   cfCurrData_pro, [piCurrCoef_pro]	; cfPrevData = *piPrevCoef;
	MOV   cfPrevData_pro, cfPrevData_pro, LSL #2 
	BNE   LOOPOverlap_pro

LoopChannelContinue
  LDR   cChInTile_pro, [sp, #iOffset_cChInTilePro]
  LDR   iCh_pro, [sp, #iOffset_iChPro]
  LDR   pau, [sp, #iOffset_pauPro]

  CMP   iCh_pro, cChInTile_pro
  BLT   LoopChannel

auSubframeRecon_ProExit
  ADD   sp, sp, #iStackSpaceRevPro     ; give back rev stack space  
  LDMFD sp!, {r4 - r11, PC} ;auSubframeRecon_MonoPro
  ENTRY_END auSubframeRecon_MonoPro


  ENDIF ; WMA_OPT_SUBFRAMERECON_ARM
    
  END