;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;// Filename msaudiopro_arm_v6.s
;
;// Copyright (c) VisualOn SoftWare Co., Ltd. All rights reserved.
;//
;//*@@@---@@@@******************************************************************
;//
;// Abstract:
;// 
;//     ARM Arch-6 specific multiplications
;//
;//      Custom build with 
;//          armasm -cpu arm1136 $(InputPath) "$(IntDir)/$(InputName).obj"
;//      and
;//          $(OutDir)\$(InputName).obj
;// 
;// Author:
;// 
;//     Witten Wen (Shanghai, China) September 28, 2008
;//
;// Revision History:
;//
;//*************************************************************************
	

  OPT         2       ; disable listing 
  INCLUDE     kxarm.h
  INCLUDE     wma_member_arm.inc
  INCLUDE	  wma_arm_version.h
  OPT         1       ; enable listing
 
  AREA    |.text|, CODE, READONLY
  
	IF	ARMVERSION	>=7
  IF WMA_OPT_SCALE_COEFFS_V3_ARM = 1
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  IMPORT  prvWeightedModifiedQuantizationV3

  EXPORT  auPreScaleCoeffsV3
  EXPORT  auPostScaleCoeffsV3
  EXPORT  auInvWeightSpectrumV3


;//*************************************************************************************
;//
;// WMARESULT auPreScaleCoeffsV3(CAudioObject *pau,
;//                          CoefType iLog2MaxOutputAllowed,
;//                          Bool fUseQuantStep,
;//                          Bool fUseGlobalScale,
;//                          CoefType *iMaxGain)
;//
;//*************************************************************************************

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Registers for auPreScaleCoeffsV3
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

pau                RN 0
iMaxOutputAllowed  RN 1
fUseQuantStep      RN 2
fUseGlobalScale    RN 3

iHi                RN 11
cLeftShiftBitsMin  RN 7
	
rgiChInTile        RN 7
rgpcinfo           RN 4
cChInTile          RN 14

iCh                RN 4
iRecon             RN 5
ppcinfo            RN 6

rgiCoefRecon       RN 12
ctMaxVal           RN 14
;valA               RN 8
;valA_1		       RN 9
;valB               RN 10
;valB_1             RN 11

iFraction          RN 12
iFracBits          RN 8
ctMaxValLow        RN 10

cLeftShiftBits     RN 1
	
iCh2               RN 2
rgiChInTile2       RN 1 

T1                 RN 10
T2                 RN 9

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  NEON Registers for auPreScaleCoeffsV3
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
A0					DN	D0
A1					DN	D1
A2					DN	D2
A3					DN	D3

B0					QN	Q0
B1					QN	Q1

QLeftCount			QN	Q6
QMaxVal				QN	Q7
DMaxVal0			DN	D14
DMaxVal1			DN	D15
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Constants for auPreScaleCoeffsV3
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
iStackSpaceRev				EQU 4*4				 
iOffset_cChInTile			EQU iStackSpaceRev-4
iOffset_rgiCoefRecon		EQU iStackSpaceRev-8
iOffset_iRecon				EQU iStackSpaceRev-12
iOffset_iMaxOutputAllowed	EQU	iStackSpaceRev-16
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	IF LINUX_RVDS = 1	
	PRESERVE8
	ENDIF
  AREA    |.text|, CODE
  LEAF_ENTRY auPreScaleCoeffsV3
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Input parameters
; r0 = pau
; r1 = iMaxOutputAllowed
; r2 = fUseQuantStep
; r3 = fUseGlobalScale


  STMFD sp!, {r4 - r12, lr}
  SUB   sp, sp, #iStackSpaceRev      ; rev stack space

; iHi = pau->m_rgpcinfo[pau->m_rgiChInTile[0]].m_cSubbandAdjusted;

  LDR   T2, [pau, #CAudioObject_m_rgiChInTile]
  LDR   rgpcinfo, [pau, #CAudioObject_m_rgpcinfo]

  LDRSH rgiChInTile, [T2]
  MOV   T1, #PerChannelInfo_size
  MLA   T2, rgiChInTile, T1, rgpcinfo
  
; cLeftShiftBitsMin = 64;
	MOV		cLeftShiftBitsMin, #64 
	MOV		iCh, #0

  ADD   T1, pau, #CAudioObject_m_cChInTile
  LDRSH cChInTile, [T1] 

  LDRSH iHi, [T2, #PerChannelInfo_m_cSubbandAdjusted]
  STR   cChInTile, [sp, #iOffset_cChInTile]  
	
	MOV		iRecon, iHi, ASR #3
	STR		iMaxOutputAllowed, [sp, #iOffset_iMaxOutputAllowed]
	STR		iRecon, [sp, #iOffset_iRecon]	
	
gTileLoop
; for (iCh = 0; iCh < pau->m_cChInTile; iCh++)

; ppcinfo = &pau->m_rgpcinfo[pau->m_rgiChInTile[iCh]];
; rgiCoefRecon = ppcinfo->m_rgiCoefRecon;
  
  LDR   T1, [pau, #CAudioObject_m_rgiChInTile]
  LDR   ppcinfo, [pau, #CAudioObject_m_rgpcinfo]
  ADD   T1, T1, iCh, LSL #1 
  LDRSH T2, [T1]

  MOV   T1, #PerChannelInfo_size
  MLA   ppcinfo, T2, T1, ppcinfo

; if (ppcinfo->m_bNoDecodeForCx)
;            continue;
	LDR		r5, [ppcinfo, #PerChannelInfo_m_bNoDecodeForCx]
	CMP		r5, #0
	BNE		gTileloopBreak
	
; ctMaxVal = 0;
	VMOV.I32	QMaxVal, #0
	LDR			iRecon, [sp, #iOffset_iRecon]
	LDR			rgiCoefRecon, [ppcinfo, #PerChannelInfo_m_rgiCoefRecon]
	STR			rgiCoefRecon, [sp, #iOffset_rgiCoefRecon]
  
; // find max value
; for (iRecon=0; iRecon < iHi; iRecon++) {
;   val = (rgiCoefRecon[iRecon] > 0) ?
;      rgiCoefRecon[iRecon] : -rgiCoefRecon[iRecon];
;   if (val > ctMaxVal) ctMaxVal = (U32)val;
; }

; unloop by 8
maxValueLoop
	VLD4.32		{A0, A1, A2, A3}, [rgiCoefRecon]!
	VABS.S32	B0, B0
	VABS.S32	B1, B1
	SUBS		iRecon, iRecon, #1
	VMAX.S32	QMaxVal, QMaxVal, B0
	VMAX.S32	QMaxVal, QMaxVal, B1	
	BNE			maxValueLoop
	
	VPMAX.S32	DMaxVal0, DMaxVal0, DMaxVal1
	VPMAX.S32	DMaxVal0, DMaxVal0, DMaxVal1
	VMOV		ctMaxVal, DMaxVal0[0]
	CMP			ctMaxVal, #0
	BEQ			gTileloopBreak

; if (fUseQuantStep == WMAB_TRUE) {
	CMP		fUseQuantStep, #1
	BNE		NotUseQuantStep

; maxQuantStep = ppcinfo->m_qfltMaxQuantStep;
  LDR    iFraction, [ppcinfo, #PerChannelInfo_m_qfltMaxQuantStep+4]
  CMP    iFraction, #0
  BEQ    gTileloopBreak
  
; ctMaxVal = ctMaxVal*maxQuantStep.iFraction;
	LDR		iMaxOutputAllowed, [sp, #iOffset_iMaxOutputAllowed]
  SMULL  ctMaxValLow, ctMaxVal, iFraction, ctMaxVal
  LDR    iFracBits, [ppcinfo, #PerChannelInfo_m_qfltMaxQuantStep]
 
; cLeftShiftBits = (I32)(iMaxOutputAllowed - LOG2CEIL_64(ctMaxVal) + maxQuantStep.iFracBits)
  SUB    cLeftShiftBits, iMaxOutputAllowed, #32
  ADD    cLeftShiftBits, cLeftShiftBits, iFracBits

  CMP    ctMaxVal, #0
  BNE    countHigh32

countLow32
  MOVS   ctMaxValLow, ctMaxValLow, LSR #1

  SUB    cLeftShiftBits, cLeftShiftBits, #1
  BNE    countLow32

  ADD    cLeftShiftBits, cLeftShiftBits, #32
  B      UpdateLeftShiftBitsMin

countHigh32
  MOVS   ctMaxVal, ctMaxVal, LSR #1

  SUB    cLeftShiftBits, cLeftShiftBits, #1
  BNE    countHigh32
  B      UpdateLeftShiftBitsMin

NotUseQuantStep

; cLeftShiftBits = (I32)(iMaxOutputAllowed - LOG2CEIL_32(ctMaxVal));
	LDR		iMaxOutputAllowed, [sp, #iOffset_iMaxOutputAllowed]
  MOV    cLeftShiftBits, iMaxOutputAllowed

count32
  MOVS   ctMaxVal, ctMaxVal, LSR #1

  SUB    cLeftShiftBits, cLeftShiftBits, #1
  BNE    count32

UpdateLeftShiftBitsMin
; if (cLeftShiftBits<cLeftShiftBitsMin && cLeftShiftBits!=0)
;    cLeftShiftBitsMin = cLeftShiftBits;
  CMP    cLeftShiftBits, #0
  BEQ    gTileloopBreak
  
  CMP    cLeftShiftBits, cLeftShiftBitsMin
  MOVLT  cLeftShiftBitsMin, cLeftShiftBits
; if (!fUseGlobalScale) {
  CMP    fUseGlobalScale, #0
  BNE    gTileloopBreak

; ppcinfo->m_cLeftShiftBitsTotal += cLeftShiftBits;
  LDR    T1, [ppcinfo, #PerChannelInfo_m_cLeftShiftBitsTotal]
  LDR    rgiCoefRecon, [sp, #iOffset_rgiCoefRecon]
  ADD    T1, T1, cLeftShiftBits

	LDR		iRecon, [sp, #iOffset_iRecon]
  
	STR    T1, [ppcinfo, #PerChannelInfo_m_cLeftShiftBitsTotal]
  
	VDUP.32		QLeftCount, cLeftShiftBits
ShiftLoop
	VLD4.32		{A0, A1, A2, A3}, [rgiCoefRecon]
	VQSHL.S32	B0, B0, QLeftCount
	VQSHL.S32	B1, B1, QLeftCount
	SUBS		iRecon, iRecon, #1
	VST4.32		{A0, A1, A2, A3}, [rgiCoefRecon]!	
	BNE			ShiftLoop
			
gTileloopBreak
  LDR    cChInTile, [sp, #iOffset_cChInTile]
  ADD    iCh, iCh, #1
  CMP    iCh, cChInTile
  BLT    gTileLoop

; if (fUseGlobalScale) {
  CMP    fUseGlobalScale, #1
  BNE    auPreScaleCoeffsV3_Exit

  MOV    iCh2, #0
  LDR    rgiChInTile2, [pau, #CAudioObject_m_rgiChInTile]
  LDR    rgpcinfo, [pau, #CAudioObject_m_rgpcinfo]
 
gOutTileLoop
; for (iCh = 0; iCh < pau->m_cChInTile; iCh++)

;  ppcinfo = &pau->m_rgpcinfo[pau->m_rgiChInTile[iCh]];
;  rgiCoefRecon = ppcinfo->m_rgiCoefRecon;

  LDRSH  T2, [rgiChInTile2], #2

  MOV    T1, #PerChannelInfo_size
  MLA    ppcinfo, T2, T1, rgpcinfo

	LDR		iRecon, [sp, #iOffset_iRecon]
  
  
  
; ppcinfo->m_cLeftShiftBitsTotal += cLeftShiftBits;
  LDR    rgiCoefRecon, [ppcinfo, #PerChannelInfo_m_rgiCoefRecon]
  LDR    T1, [ppcinfo, #PerChannelInfo_m_cLeftShiftBitsTotal]
  ADD    T1, T1, cLeftShiftBitsMin
  
	;// m_rgiCoefRecon can be NULL ( under CX_DECODE_MONO )
	;// if this is the case, skip the shift adjustment for
	;// un-allocated CoefRecon buffers.
	;if( NULL == rgiCoefRecon ) continue;
  CMP		rgiCoefRecon, #0				
  BEQ		gOutTileloopBreak
  
  CMP    cLeftShiftBitsMin, #0
  BEQ    gOutTileloopBreak
  STR    T1, [ppcinfo, #PerChannelInfo_m_cLeftShiftBitsTotal]

	VDUP.32		QLeftCount, cLeftShiftBitsMin
OutShiftLoop	
	VLD4.32		{A0, A1, A2, A3}, [rgiCoefRecon]
	VQSHL.S32	B0, B0, QLeftCount
	VQSHL.S32	B1, B1, QLeftCount
	SUBS		iRecon, iRecon, #1
	VST4.32		{A0, A1, A2, A3}, [rgiCoefRecon]!	
	BNE			OutShiftLoop
	
gOutTileloopBreak
  ADD    iCh2, iCh2, #1
  CMP    iCh2, cChInTile
  BLT    gOutTileLoop

auPreScaleCoeffsV3_Exit
  MOV   r0, #0
  ADD   sp, sp, #iStackSpaceRev      ; give back rev stack space  
  LDMFD sp!, {r4 - r12, PC} ;auPreScaleCoeffsV3
  ENTRY_END auPreScaleCoeffsV3


;//*************************************************************************************
;//
;// WMARESULT auPostScaleCoeffsV3(CAudioObject *pau)
;//
;//*************************************************************************************

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Registers for auPostScaleCoeffsV3
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

pauPost                 RN 0
cChInTilePost           RN 1
rgiChInTilePost         RN 2
iChPost                 RN 3
rgiCoefReconPost        RN 4
ppcinfoPost             RN 5

cLeftShiftBitsTotal     RN 12
iReconPost              RN 14

cLeftShiftBitsQuant     RN 6
cLeftShiftBitsFixedPost RN 8
;cLeftShiftBitsFixedPre  RN 6

Temp1                   RN 6

ChannelInfo_size        RN 7
m_bNoDecodeForCx		RN 8

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  NEON Registers for auPostScaleCoeffsV3
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
A0					DN	D0
A1					DN	D1
A2					DN	D2
A3					DN	D3

B0					QN	Q0
B1					QN	Q1

QLeftCount			QN	Q6

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	IF LINUX_RVDS = 1	
	PRESERVE8
	ENDIF
	AREA    |.text|, CODE
	LEAF_ENTRY auPostScaleCoeffsV3
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Input parameters
; r0 = pau
	STMFD sp!, {r4 - r8, lr}

  ADD   Temp1, pauPost, #CAudioObject_m_cChInTile
  MOV   iChPost, #0
  LDRSH cChInTilePost, [Temp1]
  LDR   rgiChInTilePost, [pauPost, #CAudioObject_m_rgiChInTile]

  MOV   ChannelInfo_size, #PerChannelInfo_size
 
gTileLoopPost
; for (iCh = 0; iCh < pau->m_cChInTile; iCh++) {

; ppcinfo = &pau->m_rgpcinfo[pau->m_rgiChInTile[iCh]];
; rgiCoefRecon = ppcinfo->m_rgiCoefRecon;

  LDR   ppcinfoPost, [pauPost, #CAudioObject_m_rgpcinfo]
  LDRSH Temp1, [rgiChInTilePost], #2
  MLA   ppcinfoPost, ChannelInfo_size, Temp1, ppcinfoPost
  ADD   iChPost, iChPost, #1

  LDRSH iReconPost, [ppcinfoPost, #PerChannelInfo_m_cSubbandAdjusted]
  LDR   rgiCoefReconPost, [ppcinfoPost, #PerChannelInfo_m_rgiCoefRecon]
  
; if (ppcinfo->m_bNoDecodeForCx)
;            continue;
	LDR		m_bNoDecodeForCx, [ppcinfoPost, #PerChannelInfo_m_bNoDecodeForCx]
	CMP		m_bNoDecodeForCx, #0
	BNE		gTileloopBreakPost
	
; ppcinfo->m_cLeftShiftBitsTotal -= ppcinfo->m_cLeftShiftBitsQuant;
; ppcinfo->m_cLeftShiftBitsTotal -= pau->m_cLeftShiftBitsFixedPost;

  LDR   cLeftShiftBitsQuant, [ppcinfoPost, #PerChannelInfo_m_cLeftShiftBitsQuant]
  LDR   cLeftShiftBitsTotal, [ppcinfoPost, #PerChannelInfo_m_cLeftShiftBitsTotal]
  LDR   cLeftShiftBitsFixedPost, [pauPost, #CAudioObject_m_cLeftShiftBitsFixedPost]

  SUB   cLeftShiftBitsTotal, cLeftShiftBitsTotal, cLeftShiftBitsQuant
;  LDR   cLeftShiftBitsFixedPre, [pauPost, #CAudioObject_m_cLeftShiftBitsFixedPre]

  SUB   cLeftShiftBitsTotal, cLeftShiftBitsTotal, cLeftShiftBitsFixedPost
;  ADD   cLeftShiftBitsTotal, cLeftShiftBitsTotal, cLeftShiftBitsFixedPre

; ppcinfo->m_cLeftShiftBitsTotal = 0;
; ppcinfo->m_cLeftShiftBitsQuant = 0;
  
  MOV   Temp1, #0
  STR   Temp1, [ppcinfoPost, #PerChannelInfo_m_cLeftShiftBitsTotal]
  STR   Temp1, [ppcinfoPost, #PerChannelInfo_m_cLeftShiftBitsQuant]
	
	CMP		cLeftShiftBitsTotal, #0
	BEQ		gTileloopBreakPost
	RSB		cLeftShiftBitsTotal, cLeftShiftBitsTotal, #0
	
	VDUP.32		QLeftCount, cLeftShiftBitsTotal
ShiftLoopPost	
	VLD4.32		{A0, A1, A2, A3}, [rgiCoefReconPost]
	VQSHL.S32	B0, B0, QLeftCount
	VQSHL.S32	B1, B1, QLeftCount
	SUBS		iReconPost, iReconPost, #8
	VST4.32		{A0, A1, A2, A3}, [rgiCoefReconPost]!	
	BGT			ShiftLoopPost
	
gTileloopBreakPost
  CMP    iChPost, cChInTilePost
  BLT    gTileLoopPost

  MOV   r0, #0
	LDMFD sp!, {r4 - r8, PC} ;auPostScaleCoeffsV3
  ENTRY_END auPostScaleCoeffsV3



;//*************************************************************************************
;//
;// WMARESULT auInvWeightSpectrumV3 (CAudioObject* pau, 
;//                                  PerChannelInfo* ppcinfo,
;//                                  U8 fMaskUpdate)
;//
;//*************************************************************************************

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Registers for auInvWeightSpectrumV3
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

iLoSpec							RN	0
iHiSpec							RN	1
iHi_LoSpec						RN	11

iFracBitsSpec					RN	2
iFractionSpec					RN	3	
iShiftSpec						RN	12	
	
pauSpec							RN	4
ppcinfoSpec						RN	5

rgiBarkIndexSpec				RN	6			;12
cValidBarkBandSpec				RN	7			;14			;;;;;;;;;;;;;;;
iHighCutOffSpec					RN	8
iBarkSpec						RN	9
rgiCoefReconSpec				RN	10

iCoefSpec						RN	14			;6


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Constants for auInvWeightSpectrumV3
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

iStackSpaceRevSpec            EQU 4
;iOffset_iFracBitsSpec         EQU iStackSpaceRevSpec-4		
;iOffset_iFractionSpec         EQU iStackSpaceRevSpec-8		
iOffset_fMaskUpdateSpec       EQU iStackSpaceRevSpec-4

;cLastCodedIndexV3OffsetPart1  EQU CAudioObject_m_cLastCodedIndexV3-214
;cLastCodedIndexV3OffsetPart2  EQU 214

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	IF LINUX_RVDS = 1	
	PRESERVE8
	ENDIF
	AREA    |.text|, CODE
	LEAF_ENTRY auInvWeightSpectrumV3
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Input parameters
; r0 = pau
; r1 = ppcinfo
; r2 = fMaskUpdate


  STMFD sp!, {r4 - r12, lr}
  SUB   sp, sp, #iStackSpaceRevSpec      ; rev stack space

  STR   r2, [sp, #iOffset_fMaskUpdateSpec]
  MOV   pauSpec, r0
  MOV   ppcinfoSpec, r1

; if (ppcinfo->m_cSubFrameSampleHalfWithUpdate <= 0 || 
;     ppcinfo->m_cSubFrameSampleHalf <= 0)
;       wmaResult = WMA_E_BROKEN_FRAME;

  LDR   r0, [ppcinfoSpec, #PerChannelInfo_m_cSubFrameSampleHalfWithUpdate]
  LDRSH r1, [ppcinfoSpec, #PerChannelInfo_m_cSubFrameSampleHalf]

  CMP   r0, #0
  BLE   auInvWeightSpectrumV3_error

  CMP   r1, #0
  BLE   auInvWeightSpectrumV3_error
  
; rgiBarkIndex = pau->m_rgiBarkIndex;
  LDR   rgiBarkIndexSpec, [pauSpec, #CAudioObject_m_rgiBarkIndex]

; cValidBarkBand = pau->m_cValidBarkBand;
  LDR   cValidBarkBandSpec, [pauSpec, #CAudioObject_m_cValidBarkBand]

; if (pau->m_bFreqex && pau->m_bDoFexOnCodedChannels)
;		iHighCutOff = ppcinfo->m_cSubbandAdjusted;
	LDR		r0, [pauSpec, #CAudioObject_m_bFreqex]
	CMP		r0, #0
	BEQ		FexDisable
	LDR		r0, [pauSpec, #CAudioObject_m_bDoFexOnCodedChannels]
	CMP		r0, #0
	BEQ		FexDisable	
	LDRSH	iHighCutOffSpec, [ppcinfoSpec, #PerChannelInfo_m_cSubbandAdjusted]
	B		gOutSpec
	
FexDisable
;pau->m_cLastCodedIndex
	MOV		r0, #CAudioObject_m_cLastCodedIndex
	LDRH	iHighCutOffSpec, [pauSpec, r0]
  	CMP   	iHighCutOffSpec, r1		;r1 = ppcinfo->m_cSubFrameSampleHalf
  	MOVGT 	iHighCutOffSpec, r1
  
gOutSpec
	MOV   	iBarkSpec, #0
  CMP   cValidBarkBandSpec, #0
  BLE   auInvWeightSpectrumV3_Exit

gOutLoopSpec
; for (iBark = 0; iBark < cValidBarkBand; iBark++)

; iLo = rgiBarkIndex [iBark];
; iHi = min(iHighCutOff, (rgiBarkIndex [iBark + 1]));
	LDRD	iLoSpec, [rgiBarkIndexSpec], #4				;iLoSpec = [rgiBarkIndexSpec], #4, iHiSpec = [rgiBarkIndexSpec]
; rgiCoefRecon = (CoefType*) ppcinfo->m_rgiCoefRecon;
	LDR   rgiCoefReconSpec, [ppcinfoSpec, #PerChannelInfo_m_rgiCoefRecon]
	
	CMP   iHiSpec, iHighCutOffSpec
	MOVGT iHiSpec, iHighCutOffSpec
	
	CMP   iHiSpec, iLoSpec
	BLE   gOutLoopBreakSpec
	
	SUB		iHi_LoSpec, iHiSpec, iLoSpec
	ADD   rgiCoefReconSpec, rgiCoefReconSpec, iLoSpec, LSL #2

; qfltQuantizer = prvWeightedModifiedQuantizationV3(pau,ppcinfo,iBark,fMaskUpdate);
	LDR   r3, [sp, #iOffset_fMaskUpdateSpec]
	MOV   r0, pauSpec
	MOV   r1, ppcinfoSpec
	MOV   r2, iBarkSpec
	BL    prvWeightedModifiedQuantizationV3
  
	LDRD	iFracBitsSpec, [r0]			;iFracBitsSpec = [r0], iFractionSpec = [r0 + 4]	

	
;if ((0 > qfltQuantizer.iFracBits) || (64 <= qfltQuantizer.iFracBits))
;		REPORT_BITSTREAM_CORRUPTION_AND_EXIT(wmaResult);	
	CMP		iFracBitsSpec, #0
	BLT		auInvWeightSpectrumV3_error
	CMP		iFracBitsSpec, #64
	BGE		auInvWeightSpectrumV3_error	
	
  RSBS  iShiftSpec, iFracBitsSpec, #32
  LDR   iCoefSpec, [rgiCoefReconSpec]
  BPL   gInnerLoopSpecPos

	RSB		iShiftSpec, iShiftSpec, #0
	
gInnerLoopSpecNeg
; for (iRecon = iLo; iRecon < iHi; iRecon++)
; rgiCoefRecon [iRecon] = MULT_QUANT_AND_SCALE(rgiCoefRecon [iRecon],qfltQuantizer);
  SMMUL iHiSpec, iFractionSpec, iCoefSpec  
  SUBS  iHi_LoSpec, iHi_LoSpec, #1

  MOV   iCoefSpec, iHiSpec, ASR iShiftSpec

  STR   iCoefSpec, [rgiCoefReconSpec], #4
  LDR   iCoefSpec, [rgiCoefReconSpec]
  BNE   gInnerLoopSpecNeg
  B     gOutLoopBreakSpec

gInnerLoopSpecPos
; for (iRecon = iLo; iRecon < iHi; iRecon++)
; rgiCoefRecon [iRecon] = MULT_QUANT_AND_SCALE(rgiCoefRecon [iRecon],qfltQuantizer);
  SMMUL iHiSpec, iFractionSpec, iCoefSpec  
  SUBS  iHi_LoSpec, iHi_LoSpec, #1

  MOV   iCoefSpec, iHiSpec, LSL iShiftSpec

  STR   iCoefSpec, [rgiCoefReconSpec], #4
  LDR   iCoefSpec, [rgiCoefReconSpec]
  BNE   gInnerLoopSpecPos
  
gOutLoopBreakSpec
  ADD   iBarkSpec, iBarkSpec, #1
  CMP   iBarkSpec, cValidBarkBandSpec
  BLT   gOutLoopSpec

  	MOV   	r0,  #0 
	B		auInvWeightSpectrumV3_Exit
  
auInvWeightSpectrumV3_error
	; WMA_E_BROKEN_FRAME    0x80040002
	MOV   r1, #0x80000002
	ORR   r0, r1, #0x40000

auInvWeightSpectrumV3_Exit

  ADD   sp, sp, #iStackSpaceRevSpec      ; give back rev stack space  
  LDMFD sp!, {r4 - r12, PC} ;auInvWeightSpectrumV3
  ENTRY_END auInvWeightSpectrumV3


  ENDIF ; WMA_OPT_SCALE_COEFFS_V3_ARM
    ENDIF	;//IF	ARMVERSION	>=7
  END