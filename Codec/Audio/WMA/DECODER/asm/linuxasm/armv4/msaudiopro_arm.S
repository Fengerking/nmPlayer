@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************
@// Module Name:
@//
@//     msaudiopro_arm.s
@//
@// Abstract:
@// 
@//     ARM Arch-4 specific multiplications
@//
@//      Custom build with 
@//          armasm $(InputDir)\$(InputName).s -o=$(OutDir)\$(InputName).obj 
@//      and
@//          $(OutDir)\$(InputName).obj
@// 
@// Author:
@// 
@//     Jerry He (yamihe) Feb 10, 2004
@//
@// Revision History:
@//
@//     For more information on ARM assembler directives, use
@//        http://msdn.microsoft.com/library/default.asp?url=/library/en-us/wcechp40/html/ccconarmassemblerdirectives.asp
@//*************************************************************************


@  OPT         2       @ disable listing 
  .include     "kxarm.h"
  .include     "wma_member_arm.inc"
  .include	  "wma_arm_version.h"
@  OPT         1       @ enable listing
 
  .section .text

  .IF WMA_OPT_SCALE_COEFFS_V3_ARM == 1
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  .extern  prvWeightedModifiedQuantizationV3

  .global  auPreScaleCoeffsV3
  .global  auPostScaleCoeffsV3
  .global  auInvWeightSpectrumV3


@//*************************************************************************************
@//
@// WMARESULT auPreScaleCoeffsV3(CAudioObject *pau,
@//                          CoefType iLog2MaxOutputAllowed,
@//                          Bool fUseQuantStep,
@//                          Bool fUseGlobalScale,
@//                          CoefType *iMaxGain)
@//
@//*************************************************************************************

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Registers for auPreScaleCoeffsV3
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

pau                .req r0
iMaxOutputAllowed  .req r1
fUseQuantStep      .req r2
fUseGlobalScale    .req r3

iHi                .req r11
cLeftShiftBitsMin  .req r7
	
rgiChInTile        .req r7
rgpcinfo           .req r4
cChInTile          .req r14

iCh                .req r4
iRecon             .req r5
ppcinfo            .req r6

rgiCoefRecon       .req r12
ctMaxVal           .req r14
valA               .req r8
valA_1		       .req r9
valB               .req r10
valB_1             .req r11

iFraction          .req r12
iFracBits          .req r8
ctMaxValLow        .req r10
cLeftShiftBits     .req r9
	
iCh2               .req r2
rgiChInTile2       .req r1 

T1                 .req r10
T2                 .req r9

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Constants for auPreScaleCoeffsV3
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
.equ iStackSpaceRev			, 2*4   
.equ iOffset_cChInTile		, iStackSpaceRev-4
.equ iOffset_rgiCoefRecon	, iStackSpaceRev-8
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.IF LINUX_RVDS == 1	
	PRESERVE8
	.ENDIF
  @AREA    |.text|, CODE
  LEAF_ENTRY auPreScaleCoeffsV3
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pau
@ r1 = iMaxOutputAllowed
@ r2 = fUseQuantStep
@ r3 = fUseGlobalScale


  STMFD sp!, {r4 - r12, lr}
  SUB   sp, sp, #iStackSpaceRev      @ rev stack space

@ iHi = pau->m_rgpcinfo[pau->m_rgiChInTile[0]].m_cSubbandAdjusted@

  LDR   T2, [pau, #CAudioObject_m_rgiChInTile]
  LDR   rgpcinfo, [pau, #CAudioObject_m_rgpcinfo]

  LDRSH rgiChInTile, [T2]
  MOV   T1, #PerChannelInfo_size
  MLA   T2, rgiChInTile, T1, rgpcinfo
  
@ cLeftShiftBitsMin = 64@
	MOV		cLeftShiftBitsMin, #64 
	MOV		iCh, #0

  ADD   T1, pau, #CAudioObject_m_cChInTile
  LDRSH cChInTile, [T1] 

  LDRSH iHi, [T2, #PerChannelInfo_m_cSubbandAdjusted]
  STR   cChInTile, [sp, #iOffset_cChInTile]
	
gTileLoop:
@ for (iCh = 0@ iCh < pau->m_cChInTile@ iCh++)

@ ppcinfo = &pau->m_rgpcinfo[pau->m_rgiChInTile[iCh]]@
@ rgiCoefRecon = ppcinfo->m_rgiCoefRecon@
  
  LDR   T1, [pau, #CAudioObject_m_rgiChInTile]
  LDR   ppcinfo, [pau, #CAudioObject_m_rgpcinfo]
  ADD   T1, T1, iCh, LSL #1 
  LDRSH T2, [T1]

  MOV   T1, #PerChannelInfo_size
  MLA   ppcinfo, T2, T1, ppcinfo
  
@ if (ppcinfo->m_bNoDecodeForCx)
@            continue@
	LDR		r5, [ppcinfo, #PerChannelInfo_m_bNoDecodeForCx]
	CMP		r5, #0
	BNE		gTileloopBreak
	
@ ctMaxVal = 0@
  MOV   ctMaxVal, #0
	MOV		iRecon, iHi, ASR #3

  LDR   rgiCoefRecon, [ppcinfo, #PerChannelInfo_m_rgiCoefRecon]
  STR   rgiCoefRecon, [sp, #iOffset_rgiCoefRecon]
  
@ // find max value
@ for (iRecon=0@ iRecon < iHi@ iRecon++) {
@   val = (rgiCoefRecon[iRecon] > 0) ?
@      rgiCoefRecon[iRecon] : -rgiCoefRecon[iRecon]@
@   if (val > ctMaxVal) ctMaxVal = (U32)val@
@ }

@ unloop by 8
maxValueLoop:
  LDR    valA, [rgiCoefRecon], #4
  LDR    valB, [rgiCoefRecon], #4

  CMP    valA, #0
  RSBLT  valA, valA, #0
  CMP    valA, ctMaxVal
  MOVGT  ctMaxVal, valA

  CMP    valB, #0
  RSBLT  valB, valB, #0
  CMP    valB, ctMaxVal
  MOVGT  ctMaxVal, valB

  LDR    valA, [rgiCoefRecon], #4
  LDR    valB, [rgiCoefRecon], #4

  CMP    valA, #0
  RSBLT  valA, valA, #0
  CMP    valA, ctMaxVal
  MOVGT  ctMaxVal, valA

  CMP    valB, #0
  RSBLT  valB, valB, #0
  CMP    valB, ctMaxVal
  MOVGT  ctMaxVal, valB

  LDR    valA, [rgiCoefRecon], #4
  LDR    valB, [rgiCoefRecon], #4

  CMP    valA, #0
  RSBLT  valA, valA, #0
  CMP    valA, ctMaxVal
  MOVGT  ctMaxVal, valA

  CMP    valB, #0
  RSBLT  valB, valB, #0
  CMP    valB, ctMaxVal
  MOVGT  ctMaxVal, valB

  LDR    valA, [rgiCoefRecon], #4
  LDR    valB, [rgiCoefRecon], #4

  CMP    valA, #0
  RSBLT  valA, valA, #0
  CMP    valA, ctMaxVal
  MOVGT  ctMaxVal, valA

  CMP    valB, #0
  RSBLT  valB, valB, #0
  CMP    valB, ctMaxVal
  MOVGT  ctMaxVal, valB
	
	SUBS	iRecon, iRecon, #1
	BNE		maxValueLoop
	
	CMP		ctMaxVal, #0
	BEQ		gTileloopBreak

@ if (fUseQuantStep == WMAB_TRUE) {
	CMP		fUseQuantStep, #1
	BNE		NotUseQuantStep

@ maxQuantStep = ppcinfo->m_qfltMaxQuantStep@
  LDR    iFraction, [ppcinfo, #PerChannelInfo_m_qfltMaxQuantStep+4]
  CMP    iFraction, #0
  BEQ    gTileloopBreak
  
@ ctMaxVal = ctMaxVal*maxQuantStep.iFraction@
  SMULL  ctMaxValLow, ctMaxVal, iFraction, ctMaxVal
  LDR    iFracBits, [ppcinfo, #PerChannelInfo_m_qfltMaxQuantStep]
 
@ cLeftShiftBits = (I32)(iMaxOutputAllowed - LOG2CEIL_64(ctMaxVal) + maxQuantStep.iFracBits)
  SUB    cLeftShiftBits, iMaxOutputAllowed, #32
  ADD    cLeftShiftBits, cLeftShiftBits, iFracBits

  CMP    ctMaxVal, #0
  BNE    countHigh32

countLow32:
  MOVS   ctMaxValLow, ctMaxValLow, LSR #1

  SUB    cLeftShiftBits, cLeftShiftBits, #1
  BNE    countLow32

  ADD    cLeftShiftBits, cLeftShiftBits, #32
  B      UpdateLeftShiftBitsMin

countHigh32:
  MOVS   ctMaxVal, ctMaxVal, LSR #1

  SUB    cLeftShiftBits, cLeftShiftBits, #1
  BNE    countHigh32
  B      UpdateLeftShiftBitsMin

NotUseQuantStep:

@ cLeftShiftBits = (I32)(iMaxOutputAllowed - LOG2CEIL_32(ctMaxVal))@
  MOV    cLeftShiftBits, iMaxOutputAllowed

count32:
  MOVS   ctMaxVal, ctMaxVal, LSR #1

  SUB    cLeftShiftBits, cLeftShiftBits, #1
  BNE    count32

UpdateLeftShiftBitsMin:
@ if (cLeftShiftBits<cLeftShiftBitsMin && cLeftShiftBits!=0)
@    cLeftShiftBitsMin = cLeftShiftBits@
  CMP    cLeftShiftBits, #0
  BEQ    gTileloopBreak
  
  CMP    cLeftShiftBits, cLeftShiftBitsMin
  MOVLT  cLeftShiftBitsMin, cLeftShiftBits
@ if (!fUseGlobalScale) {
  CMP    fUseGlobalScale, #0
  BNE    gTileloopBreak

@ ppcinfo->m_cLeftShiftBitsTotal += cLeftShiftBits@
  LDR    T1, [ppcinfo, #PerChannelInfo_m_cLeftShiftBitsTotal]
  LDR    rgiCoefRecon, [sp, #iOffset_rgiCoefRecon]
  ADD    T1, T1, cLeftShiftBits

	MOV		iRecon, iHi, ASR #3
  
  STR    T1, [ppcinfo, #PerChannelInfo_m_cLeftShiftBitsTotal]
  
  CMP    cLeftShiftBits, #0
  BGT    LeftShiftLoop
  RSB    cLeftShiftBits, cLeftShiftBits, #0

RightShiftLoop:
@ for (iRecon=0@ iRecon < iHi@ iRecon++) {
@     rgiCoefRecon[iRecon] >>= -cLeftShiftBits@
@ unloop by 8

  LDR    valA, [rgiCoefRecon]
  LDR    valB, [rgiCoefRecon, #4]

  MOV    valA, valA, ASR cLeftShiftBits
  MOV    valB, valB, ASR cLeftShiftBits

  STR    valA, [rgiCoefRecon], #4
  STR    valB, [rgiCoefRecon], #4

  LDR    valA, [rgiCoefRecon]
  LDR    valB, [rgiCoefRecon, #4]

  MOV    valA, valA, ASR cLeftShiftBits
  MOV    valB, valB, ASR cLeftShiftBits

  STR    valA, [rgiCoefRecon], #4
  STR    valB, [rgiCoefRecon], #4

  LDR    valA, [rgiCoefRecon]
  LDR    valB, [rgiCoefRecon, #4]

  MOV    valA, valA, ASR cLeftShiftBits
  MOV    valB, valB, ASR cLeftShiftBits

  STR    valA, [rgiCoefRecon], #4
  STR    valB, [rgiCoefRecon], #4

  LDR    valA, [rgiCoefRecon]
  LDR    valB, [rgiCoefRecon, #4]

  MOV    valA, valA, ASR cLeftShiftBits
  MOV    valB, valB, ASR cLeftShiftBits

  STR    valA, [rgiCoefRecon], #4
  STR    valB, [rgiCoefRecon], #4
	
  SUBS   iRecon, iRecon, #1
  BNE    RightShiftLoop

  B      gTileloopBreak

LeftShiftLoop:
@ for (iRecon=0@ iRecon < iHi@ iRecon++)
@     rgiCoefRecon[iRecon] <<= cLeftShiftBits@
@ unloop by 8
  LDR    valA, [rgiCoefRecon]
  LDR    valB, [rgiCoefRecon, #4]

  MOV    valA, valA, LSL cLeftShiftBits
  MOV    valB, valB, LSL cLeftShiftBits

  STR    valA, [rgiCoefRecon], #4
  STR    valB, [rgiCoefRecon], #4

  LDR    valA, [rgiCoefRecon]
  LDR    valB, [rgiCoefRecon, #4]

  MOV    valA, valA, LSL cLeftShiftBits
  MOV    valB, valB, LSL cLeftShiftBits

  STR    valA, [rgiCoefRecon], #4
  STR    valB, [rgiCoefRecon], #4

  LDR    valA, [rgiCoefRecon]
  LDR    valB, [rgiCoefRecon, #4]

  MOV    valA, valA, LSL cLeftShiftBits
  MOV    valB, valB, LSL cLeftShiftBits

  STR    valA, [rgiCoefRecon], #4
  STR    valB, [rgiCoefRecon], #4

  LDR    valA, [rgiCoefRecon]
  LDR    valB, [rgiCoefRecon, #4]

  MOV    valA, valA, LSL cLeftShiftBits
  MOV    valB, valB, LSL cLeftShiftBits

  STR    valA, [rgiCoefRecon], #4
  STR    valB, [rgiCoefRecon], #4
	
  SUBS   iRecon, iRecon, #1
  BNE    LeftShiftLoop

gTileloopBreak:
  LDR    cChInTile, [sp, #iOffset_cChInTile]
  ADD    iCh, iCh, #1
  CMP    iCh, cChInTile
  BLT    gTileLoop

@ if (fUseGlobalScale) {
  CMP    fUseGlobalScale, #1
  BNE    auPreScaleCoeffsV3_Exit

  MOV    iCh2, #0
  LDR    rgiChInTile2, [pau, #CAudioObject_m_rgiChInTile]
  LDR    rgpcinfo, [pau, #CAudioObject_m_rgpcinfo]
 
gOutTileLoop:
@ for (iCh = 0@ iCh < pau->m_cChInTile@ iCh++)

@  ppcinfo = &pau->m_rgpcinfo[pau->m_rgiChInTile[iCh]]@
@  rgiCoefRecon = ppcinfo->m_rgiCoefRecon@

  LDRSH  T2, [rgiChInTile2], #2

  MOV    T1, #PerChannelInfo_size
  MLA    ppcinfo, T2, T1, rgpcinfo
	MOV		iRecon, iHi, ASR #3
  
  
  
@ ppcinfo->m_cLeftShiftBitsTotal += cLeftShiftBits@
  LDR    rgiCoefRecon, [ppcinfo, #PerChannelInfo_m_rgiCoefRecon]
  LDR    T1, [ppcinfo, #PerChannelInfo_m_cLeftShiftBitsTotal]
  ADD    T1, T1, cLeftShiftBitsMin
  
	@// m_rgiCoefRecon can be NULL ( under CX_DECODE_MONO )
	@// if this is the case, skip the shift adjustment for
	@// un-allocated CoefRecon buffers.
	@if( NULL == rgiCoefRecon ) continue@
  CMP		rgiCoefRecon, #0				
  BEQ		gOutTileloopBreak
  
  CMP    cLeftShiftBitsMin, #0
  BEQ    gOutTileloopBreak
  STR    T1, [ppcinfo, #PerChannelInfo_m_cLeftShiftBitsTotal]

  BGT    OutLeftShiftLoop
  RSB    cLeftShiftBitsMin, cLeftShiftBitsMin, #0

OutRightShiftLoop:
@ for (iRecon=0@ iRecon < iHi@ iRecon++) {
@     rgiCoefRecon[iRecon] >>= -cLeftShiftBits@
@ unloop by 8
  LDR    valA, [rgiCoefRecon]
  LDR    valB, [rgiCoefRecon, #4]

  MOV    valA, valA, ASR cLeftShiftBitsMin
  MOV    valB, valB, ASR cLeftShiftBitsMin

  STR    valA, [rgiCoefRecon], #4
  STR    valB, [rgiCoefRecon], #4

  LDR    valA, [rgiCoefRecon]
  LDR    valB, [rgiCoefRecon, #4]

  MOV    valA, valA, ASR cLeftShiftBitsMin
  MOV    valB, valB, ASR cLeftShiftBitsMin

  STR    valA, [rgiCoefRecon], #4
  STR    valB, [rgiCoefRecon], #4

  LDR    valA, [rgiCoefRecon]
  LDR    valB, [rgiCoefRecon, #4]

  MOV    valA, valA, ASR cLeftShiftBitsMin
  MOV    valB, valB, ASR cLeftShiftBitsMin

  STR    valA, [rgiCoefRecon], #4
  STR    valB, [rgiCoefRecon], #4

  LDR    valA, [rgiCoefRecon]
  LDR    valB, [rgiCoefRecon, #4]

  MOV    valA, valA, ASR cLeftShiftBitsMin
  MOV    valB, valB, ASR cLeftShiftBitsMin

  STR    valA, [rgiCoefRecon], #4
  STR    valB, [rgiCoefRecon], #4

  SUBS   iRecon, iRecon, #1
  BNE    OutRightShiftLoop

  RSB    cLeftShiftBitsMin, cLeftShiftBitsMin, #0
  B      gOutTileloopBreak

OutLeftShiftLoop:
@ for (iRecon=0@ iRecon < iHi@ iRecon++)
@     rgiCoefRecon[iRecon] <<= cLeftShiftBits@
@ unloop by 8
  LDR    valA, [rgiCoefRecon]
  LDR    valB, [rgiCoefRecon, #4]

  MOV    valA, valA, LSL cLeftShiftBitsMin
  MOV    valB, valB, LSL cLeftShiftBitsMin

  STR    valA, [rgiCoefRecon], #4
  STR    valB, [rgiCoefRecon], #4

  LDR    valA, [rgiCoefRecon]
  LDR    valB, [rgiCoefRecon, #4]

  MOV    valA, valA, LSL cLeftShiftBitsMin
  MOV    valB, valB, LSL cLeftShiftBitsMin

  STR    valA, [rgiCoefRecon], #4
  STR    valB, [rgiCoefRecon], #4

  LDR    valA, [rgiCoefRecon]
  LDR    valB, [rgiCoefRecon, #4]

  MOV    valA, valA, LSL cLeftShiftBitsMin
  MOV    valB, valB, LSL cLeftShiftBitsMin

  STR    valA, [rgiCoefRecon], #4
  STR    valB, [rgiCoefRecon], #4

  LDR    valA, [rgiCoefRecon]
  LDR    valB, [rgiCoefRecon, #4]

  MOV    valA, valA, LSL cLeftShiftBitsMin
  MOV    valB, valB, LSL cLeftShiftBitsMin

  STR    valA, [rgiCoefRecon], #4
  STR    valB, [rgiCoefRecon], #4
  SUBS   iRecon, iRecon, #1
  BNE    OutLeftShiftLoop

gOutTileloopBreak:
  ADD    iCh2, iCh2, #1
  CMP    iCh2, cChInTile
  BLT    gOutTileLoop

auPreScaleCoeffsV3_Exit:
  MOV   r0, #0
  ADD   sp, sp, #iStackSpaceRev      @ give back rev stack space  
  LDMFD sp!, {r4 - r12, PC} @auPreScaleCoeffsV3
  ENTRY_END auPreScaleCoeffsV3



@//*************************************************************************************
@//
@// WMARESULT auPostScaleCoeffsV3(CAudioObject *pau)
@//
@//*************************************************************************************

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Registers for auPostScaleCoeffsV3
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

pauPost                 .req r0
cChInTilePost           .req r1
rgiChInTilePost         .req r2
iChPost                 .req r3
rgiCoefReconPost        .req r4
ppcinfoPost             .req r5

cLeftShiftBitsTotal     .req r12
iReconPost              .req r14

cLeftShiftBitsQuant     .req r6
cLeftShiftBitsFixedPost .req r8
@cLeftShiftBitsFixedPre  .req r6

Temp1                   .req r6
Temp2                   .req r8

ChannelInfo_size        .req r7
m_bNoDecodeForCx		.req r8

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	PRESERVE8
  @AREA    |.text|, CODE
  LEAF_ENTRY auPostScaleCoeffsV3
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pau
	STMFD sp!, {r4 - r8, lr}

  ADD   Temp1, pauPost, #CAudioObject_m_cChInTile
  MOV   iChPost, #0
  LDRSH cChInTilePost, [Temp1]
  LDR   rgiChInTilePost, [pauPost, #CAudioObject_m_rgiChInTile]

  MOV   ChannelInfo_size, #PerChannelInfo_size
 
gTileLoopPost:
@ for (iCh = 0@ iCh < pau->m_cChInTile@ iCh++) {

@ ppcinfo = &pau->m_rgpcinfo[pau->m_rgiChInTile[iCh]]@
@ rgiCoefRecon = ppcinfo->m_rgiCoefRecon@

  LDR   ppcinfoPost, [pauPost, #CAudioObject_m_rgpcinfo]
  LDRSH Temp1, [rgiChInTilePost], #2
  MLA   ppcinfoPost, ChannelInfo_size, Temp1, ppcinfoPost
  ADD   iChPost, iChPost, #1

  LDRSH iReconPost, [ppcinfoPost, #PerChannelInfo_m_cSubbandAdjusted]
  LDR   rgiCoefReconPost, [ppcinfoPost, #PerChannelInfo_m_rgiCoefRecon]

@ if (ppcinfo->m_bNoDecodeForCx)
@            continue@
	LDR		m_bNoDecodeForCx, [ppcinfoPost, #PerChannelInfo_m_bNoDecodeForCx]
	CMP		m_bNoDecodeForCx, #0
	BNE		gTileloopBreakPost
	
@ ppcinfo->m_cLeftShiftBitsTotal -= ppcinfo->m_cLeftShiftBitsQuant@
@ ppcinfo->m_cLeftShiftBitsTotal -= pau->m_cLeftShiftBitsFixedPost@

  LDR   cLeftShiftBitsQuant, [ppcinfoPost, #PerChannelInfo_m_cLeftShiftBitsQuant]
  LDR   cLeftShiftBitsTotal, [ppcinfoPost, #PerChannelInfo_m_cLeftShiftBitsTotal]
  LDR   cLeftShiftBitsFixedPost, [pauPost, #CAudioObject_m_cLeftShiftBitsFixedPost]

  SUB   cLeftShiftBitsTotal, cLeftShiftBitsTotal, cLeftShiftBitsQuant
@  LDR   cLeftShiftBitsFixedPre, [pauPost, #CAudioObject_m_cLeftShiftBitsFixedPre]

  SUB   cLeftShiftBitsTotal, cLeftShiftBitsTotal, cLeftShiftBitsFixedPost
@  ADD   cLeftShiftBitsTotal, cLeftShiftBitsTotal, cLeftShiftBitsFixedPre

@ ppcinfo->m_cLeftShiftBitsTotal = 0@
@ ppcinfo->m_cLeftShiftBitsQuant = 0@
  
  MOV   Temp1, #0
  STR   Temp1, [ppcinfoPost, #PerChannelInfo_m_cLeftShiftBitsTotal]
  STR   Temp1, [ppcinfoPost, #PerChannelInfo_m_cLeftShiftBitsQuant]

  CMP   cLeftShiftBitsTotal, #0
  BGT   RightShiftLoopPost
  BEQ   gTileloopBreakPost

  RSB   cLeftShiftBitsTotal, cLeftShiftBitsTotal, #0

LeftShiftLoopPost:
@ else if (ppcinfo->m_cLeftShiftBitsTotal < 0)
@ for (iRecon = 0@ iRecon < ppcinfo->m_cSubbandAdjusted@ iRecon++)
@      rgiCoefRecon[iRecon] <<= -ppcinfo->m_cLeftShiftBitsTotal@
@ unloop by 8
  LDR    Temp1, [rgiCoefReconPost]
  LDR    Temp2, [rgiCoefReconPost, #4]

  MOV    Temp1, Temp1, LSL cLeftShiftBitsTotal
  MOV    Temp2, Temp2, LSL cLeftShiftBitsTotal

  STR    Temp1, [rgiCoefReconPost], #4
  STR    Temp2, [rgiCoefReconPost], #4  

  LDR    Temp1, [rgiCoefReconPost]
  LDR    Temp2, [rgiCoefReconPost, #4]

  MOV    Temp1, Temp1, LSL cLeftShiftBitsTotal
  MOV    Temp2, Temp2, LSL cLeftShiftBitsTotal

  STR    Temp1, [rgiCoefReconPost], #4
  STR    Temp2, [rgiCoefReconPost], #4  

  LDR    Temp1, [rgiCoefReconPost]
  LDR    Temp2, [rgiCoefReconPost, #4]

  MOV    Temp1, Temp1, LSL cLeftShiftBitsTotal
  MOV    Temp2, Temp2, LSL cLeftShiftBitsTotal

  STR    Temp1, [rgiCoefReconPost], #4
  STR    Temp2, [rgiCoefReconPost], #4  

  LDR    Temp1, [rgiCoefReconPost]
  LDR    Temp2, [rgiCoefReconPost, #4]

  MOV    Temp1, Temp1, LSL cLeftShiftBitsTotal
  MOV    Temp2, Temp2, LSL cLeftShiftBitsTotal

  STR    Temp1, [rgiCoefReconPost], #4
  STR    Temp2, [rgiCoefReconPost], #4  
  SUBS   iReconPost, iReconPost, #8
  BGT    LeftShiftLoopPost
  B      gTileloopBreakPost


RightShiftLoopPost:
@ if (ppcinfo->m_cLeftShiftBitsTotal > 0)
@ for (iRecon = 0@ iRecon < ppcinfo->m_cSubbandAdjusted@ iRecon++) {
@      rgiCoefRecon[iRecon] >>= ppcinfo->m_cLeftShiftBitsTotal@
@ unloop by 8
	
  LDR    Temp1, [rgiCoefReconPost]
  LDR    Temp2, [rgiCoefReconPost, #4]

  MOV    Temp1, Temp1, ASR cLeftShiftBitsTotal
  MOV    Temp2, Temp2, ASR cLeftShiftBitsTotal

  STR    Temp1, [rgiCoefReconPost], #4
  STR    Temp2, [rgiCoefReconPost], #4  

  LDR    Temp1, [rgiCoefReconPost]
  LDR    Temp2, [rgiCoefReconPost, #4]

  MOV    Temp1, Temp1, ASR cLeftShiftBitsTotal
  MOV    Temp2, Temp2, ASR cLeftShiftBitsTotal

  STR    Temp1, [rgiCoefReconPost], #4
  STR    Temp2, [rgiCoefReconPost], #4  

  LDR    Temp1, [rgiCoefReconPost]
  LDR    Temp2, [rgiCoefReconPost, #4]

  MOV    Temp1, Temp1, ASR cLeftShiftBitsTotal
  MOV    Temp2, Temp2, ASR cLeftShiftBitsTotal

  STR    Temp1, [rgiCoefReconPost], #4
  STR    Temp2, [rgiCoefReconPost], #4  

  LDR    Temp1, [rgiCoefReconPost]
  LDR    Temp2, [rgiCoefReconPost, #4]

  MOV    Temp1, Temp1, ASR cLeftShiftBitsTotal
  MOV    Temp2, Temp2, ASR cLeftShiftBitsTotal

  STR    Temp1, [rgiCoefReconPost], #4
  STR    Temp2, [rgiCoefReconPost], #4  
	
  SUBS   iReconPost, iReconPost, #8
  BGT    RightShiftLoopPost

gTileloopBreakPost:
  CMP    iChPost, cChInTilePost
  BLT    gTileLoopPost

  MOV   r0, #0
	LDMFD sp!, {r4 - r8, PC} @auPostScaleCoeffsV3
  ENTRY_END auPostScaleCoeffsV3



@//*************************************************************************************
@//
@// WMARESULT auInvWeightSpectrumV3 (CAudioObject* pau, 
@//                                  PerChannelInfo* ppcinfo,
@//                                  U8 fMaskUpdate)
@//
@//*************************************************************************************

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Registers for auInvWeightSpectrumV3
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

iLoSpec							.req	r0
iHiSpec							.req	r1
iHi_LoSpec						.req	r11

iFracBitsSpec					.req	r2
iFractionSpec					.req	r3	
iShiftSpec						.req	r12	
	
pauSpec							.req	r4
ppcinfoSpec						.req	r5

rgiBarkIndexSpec				.req	r6			@12
cValidBarkBandSpec				.req	r7			@14			@@@@@@@@@@@@@@@
iHighCutOffSpec					.req	r8
iBarkSpec						.req	r9
rgiCoefReconSpec				.req	r10

iCoefSpec						.req	r14			@6


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Constants for auInvWeightSpectrumV3
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.equ iStackSpaceRevSpec				, 4
@iOffset_iFracBitsSpec			EQU iStackSpaceRevSpec-4		
@iOffset_iFractionSpec			EQU iStackSpaceRevSpec-8		
.equ iOffset_fMaskUpdate				, iStackSpaceRevSpec-4

@cLastCodedIndexV3OffsetPart1  EQU CAudioObject_m_cLastCodedIndex-214
@cLastCodedIndexV3OffsetPart2  EQU 214

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  @AREA    |.text|, CODE
  LEAF_ENTRY auInvWeightSpectrumV3
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pau
@ r1 = ppcinfo
@ r2 = fMaskUpdate


  STMFD sp!, {r4 - r12, lr}
  SUB   sp, sp, #iStackSpaceRevSpec      @ rev stack space

  MOV   pauSpec, r0
  MOV   ppcinfoSpec, r1
	STR		r2, [sp, #iOffset_fMaskUpdate]
	
@ if (ppcinfo->m_cSubFrameSampleHalfWithUpdate <= 0 || 
@     ppcinfo->m_cSubFrameSampleHalf <= 0)
@       wmaResult = WMA_E_BROKEN_FRAME@

  LDR   r0, [ppcinfoSpec, #PerChannelInfo_m_cSubFrameSampleHalfWithUpdate]
  LDRSH r1, [ppcinfoSpec, #PerChannelInfo_m_cSubFrameSampleHalf]

  CMP   r0, #0
  BLE   auInvWeightSpectrumV3_error

  CMP   r1, #0
  BLE   auInvWeightSpectrumV3_error

@ rgiBarkIndex = pau->m_rgiBarkIndex@
  LDR   rgiBarkIndexSpec, [pauSpec, #CAudioObject_m_rgiBarkIndex]

@ cValidBarkBand = pau->m_cValidBarkBand@
  LDR   cValidBarkBandSpec, [pauSpec, #CAudioObject_m_cValidBarkBand]

@ if (pau->m_bFreqex && pau->m_bDoFexOnCodedChannels)
@		iHighCutOff = ppcinfo->m_cSubbandAdjusted@
	LDR		r0, [pauSpec, #CAudioObject_m_bFreqex]
	CMP		r0, #0
	BEQ		FexDisable
	LDR		r0, [pauSpec, #CAudioObject_m_bDoFexOnCodedChannels]
	CMP		r0, #0
	BEQ		FexDisable	
	LDRSH	iHighCutOffSpec, [ppcinfoSpec, #PerChannelInfo_m_cSubbandAdjusted]
	B		gOutSpec
	
FexDisable:
@pau->m_cLastCodedIndex
	MOV		r0, #CAudioObject_m_cLastCodedIndex
	LDRH	iHighCutOffSpec, [pauSpec, r0]
	CMP		iHighCutOffSpec, r1		@r1 = ppcinfo->m_cSubFrameSampleHalf
	MOVGT	iHighCutOffSpec, r1
  
gOutSpec:
	MOV		iBarkSpec, #0
  CMP   cValidBarkBandSpec, #0
  BLE   auInvWeightSpectrumV3_Exit

gOutLoopSpec:
@ for (iBark = 0@ iBark < cValidBarkBand@ iBark++)

@ iLo = rgiBarkIndex [iBark]@
@ iHi = min(iHighCutOff, (rgiBarkIndex [iBark + 1]))@
  LDR   iLoSpec, [rgiBarkIndexSpec], #4
  LDR   iHiSpec, [rgiBarkIndexSpec]
	
  CMP   iHiSpec, iHighCutOffSpec
  MOVGT iHiSpec, iHighCutOffSpec

  CMP   iHiSpec, iLoSpec
  BLE   gOutLoopBreakSpec

@ rgiCoefRecon = (CoefType*) ppcinfo->m_rgiCoefRecon@
  LDR   rgiCoefReconSpec, [ppcinfoSpec, #PerChannelInfo_m_rgiCoefRecon]
	
	SUB		iHi_LoSpec, iHiSpec, iLoSpec
  ADD   rgiCoefReconSpec, rgiCoefReconSpec, iLoSpec, LSL #2

@ qfltQuantizer = prvWeightedModifiedQuantizationV3(pau,ppcinfo,iBark,fMaskUpdate)@
	LDR   r3, [sp, #iOffset_fMaskUpdate]
	MOV   r0, pauSpec
	MOV   r1, ppcinfoSpec
	MOV   r2, iBarkSpec
	BL    prvWeightedModifiedQuantizationV3
  
	LDR		iFracBitsSpec, [r0]
	LDR		iFractionSpec, [r0, #4]
	
@if ((0 > qfltQuantizer.iFracBits) || (64 <= qfltQuantizer.iFracBits))
@		REPORT_BITSTREAM_CORRUPTION_AND_EXIT(wmaResult)@	
	CMP		iFracBitsSpec, #0
	BLT		auInvWeightSpectrumV3_error
	CMP		iFracBitsSpec, #64
	BGE		auInvWeightSpectrumV3_error		
	
  RSBS  iShiftSpec, iFracBitsSpec, #32
  BPL   gInnerLoopSpecPos

@  SUB   iShiftSpec, iFracBitsSpec, #32
	RSB		iShiftSpec, iShiftSpec, #0
	
gInnerLoopSpecNeg:
@ for (iRecon = iLo@ iRecon < iHi@ iRecon++)
@ rgiCoefRecon [iRecon] = MULT_QUANT_AND_SCALE(rgiCoefRecon [iRecon],qfltQuantizer)@
  LDR   iCoefSpec, [rgiCoefReconSpec]

  SMULL iLoSpec, iHiSpec, iFractionSpec, iCoefSpec  
  SUBS  iHi_LoSpec, iHi_LoSpec, #1

  MOV   iCoefSpec, iHiSpec, ASR iShiftSpec

  STR   iCoefSpec, [rgiCoefReconSpec], #4
  BNE   gInnerLoopSpecNeg
  B     gOutLoopBreakSpec

gInnerLoopSpecPos:
@ for (iRecon = iLo@ iRecon < iHi@ iRecon++)
@ rgiCoefRecon [iRecon] = MULT_QUANT_AND_SCALE(rgiCoefRecon [iRecon],qfltQuantizer)@	
  LDR   iCoefSpec, [rgiCoefReconSpec]

  SMULL iLoSpec, iHiSpec, iFractionSpec, iCoefSpec  
  SUBS  iHi_LoSpec, iHi_LoSpec, #1

  MOV   iCoefSpec, iHiSpec, LSL iShiftSpec

  STR   iCoefSpec, [rgiCoefReconSpec], #4
  BNE   gInnerLoopSpecPos
  
gOutLoopBreakSpec:
  ADD   iBarkSpec, iBarkSpec, #1
  CMP   iBarkSpec, cValidBarkBandSpec
  BLT   gOutLoopSpec

  MOV   r0,  #0 
  B		auInvWeightSpectrumV3_Exit
  
auInvWeightSpectrumV3_error:
	@ WMA_E_BROKEN_FRAME    0x80040002
	MOV   r1, #0x80000002
	ORR   r0, r1, #0x40000

auInvWeightSpectrumV3_Exit:

  ADD   sp, sp, #iStackSpaceRevSpec      @ give back rev stack space  
  LDMFD sp!, {r4 - r12, PC} @auInvWeightSpectrumV3
  ENTRY_END auInvWeightSpectrumV3


  .ENDIF @ WMA_OPT_SCALE_COEFFS_V3_ARM
    
  .END