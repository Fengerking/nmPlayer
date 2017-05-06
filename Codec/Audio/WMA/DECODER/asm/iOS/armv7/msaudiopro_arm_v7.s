@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@// Filename msaudiopro_arm_v6.s
@
@// Copyright (c) VisualOn SoftWare Co., Ltd. All rights reserved.
@//
@//*@@@---@@@@******************************************************************
@//
@// Abstract:
@// 
@//     ARM Arch-6 specific multiplications
@//
@//      Custom build with 
@//          armasm -cpu arm1136 $(InputPath) "$(IntDir)/$(InputName).obj"
@//      and
@//          $(OutDir)\$(InputName).obj
@// 
@// Author:
@// 
@//     Witten Wen (Shanghai, China) September 28, 2008
@//
@// Revision History:
@//
@//*************************************************************************
	

@  OPT         2       @ disable listing 
  #include "../../../inc/audio/v10/include/voWMADecID.h"
  .include     "kxarm.h"
  .include     "wma_member_arm.inc"
  .include	  "wma_arm_version.h"
@  OPT         1       @ enable listing
 
@  AREA    |.text|, CODE, READONLY
  .text .align 4
	 
	.if	ARMVERSION	>=7
  .if WMA_OPT_SCALE_COEFFS_V3_ARM == 1
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  .globl  _prvWeightedModifiedQuantizationV3

  .globl  _auPreScaleCoeffsV3
  .globl  _auPostScaleCoeffsV3
  .globl  _auInvWeightSpectrumV3


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
@valA               .req r8
@valA_1		       .req r9
@valB               .req r10
@valB_1             .req r11

iFraction          .req r12
iFracBits          .req r8
ctMaxValLow        .req r10

cLeftShiftBits     .req r1
	
iCh2               .req r2
rgiChInTile2       .req r1 

T1                 .req r10
T2                 .req r9

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@  NEON Registers for auPreScaleCoeffsV3
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
A0					.req	D0
AA1					.req	D1
AA2					.req	D2
AA3					.req	D3

B0					.req	Q0
B1					.req	Q1

QLeftCount			.req	Q6
QMaxVal				.req	Q7
DMaxVal0			.req	D14
DMaxVal1			.req	D15
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Constants for auPreScaleCoeffsV3
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
.set	iStackSpaceRev				, 4*4				 
.set	iOffset_cChInTile			, iStackSpaceRev-4
.set	iOffset_rgiCoefRecon		, iStackSpaceRev-8
.set	iOffset_iRecon				, iStackSpaceRev-12
.set	iOffset_iMaxOutputAllowed	,	iStackSpaceRev-16
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.if LINUX_RVDS == 1	
	PRESERVE8
	.endif
@  AREA    |.text|, CODE
_auPreScaleCoeffsV3:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pau
@ r1 = iMaxOutputAllowed
@ r2 = fUseQuantStep
@ r3 = fUseGlobalScale


  stmfd sp!, {r4 - r12, lr}
  sub   sp, sp, #iStackSpaceRev      @ rev stack space

@ iHi = pau->m_rgpcinfo[pau->m_rgiChInTile[0]].m_cSubbandAdjusted@

  ldr   T2, [pau, #CAudioObject_m_rgiChInTile]
  ldr   rgpcinfo, [pau, #CAudioObject_m_rgpcinfo]

  ldrsh rgiChInTile, [T2]
  mov   T1, #PerChannelInfo_size
  mla   T2, rgiChInTile, T1, rgpcinfo
  
@ cLeftShiftBitsMin = 64@
	mov		cLeftShiftBitsMin, #64 
	mov		iCh, #0

  add   T1, pau, #CAudioObject_m_cChInTile
  ldrsh cChInTile, [T1] 

  ldrsh iHi, [T2, #PerChannelInfo_m_cSubbandAdjusted]
  str   cChInTile, [sp, #iOffset_cChInTile]  
	
	mov		iRecon, iHi, ASR #3
	str		iMaxOutputAllowed, [sp, #iOffset_iMaxOutputAllowed]
	str		iRecon, [sp, #iOffset_iRecon]	
	
gTileLoop:
@ for (iCh = 0@ iCh < pau->m_cChInTile@ iCh++)

@ ppcinfo = &pau->m_rgpcinfo[pau->m_rgiChInTile[iCh]]@
@ rgiCoefRecon = ppcinfo->m_rgiCoefRecon@
  
  ldr   T1, [pau, #CAudioObject_m_rgiChInTile]
  ldr   ppcinfo, [pau, #CAudioObject_m_rgpcinfo]
  add   T1, T1, iCh, LSL #1 
  ldrsh T2, [T1]

  mov   T1, #PerChannelInfo_size
  mla   ppcinfo, T2, T1, ppcinfo

@ if (ppcinfo->m_bNoDecodeForCx)
@            continue@
	ldr		r5, [ppcinfo, #PerChannelInfo_m_bNoDecodeForCx]
	cmp		r5, #0
	bne		gTileloopBreak
	
@ ctMaxVal = 0@
	vmov.i32	QMaxVal, #0
	ldr			iRecon, [sp, #iOffset_iRecon]
	ldr			rgiCoefRecon, [ppcinfo, #PerChannelInfo_m_rgiCoefRecon]
	str			rgiCoefRecon, [sp, #iOffset_rgiCoefRecon]
  
@ // find max value
@ for (iRecon=0@ iRecon < iHi@ iRecon++) {
@   val = (rgiCoefRecon[iRecon] > 0) ?
@      rgiCoefRecon[iRecon] : -rgiCoefRecon[iRecon]@
@   if (val > ctMaxVal) ctMaxVal = (U32)val@
@ }

@ unloop by 8
maxValueLoop:
	vld4.32		{A0, AA1, AA2, AA3}, [rgiCoefRecon]!
	vabs.s32	B0, B0
	vabs.s32	B1, B1
	subs		iRecon, iRecon, #1
	vmax.s32	QMaxVal, QMaxVal, B0
	vmax.s32	QMaxVal, QMaxVal, B1	
	bne			maxValueLoop
	
	vpmax.s32	DMaxVal0, DMaxVal0, DMaxVal1
	vpmax.s32	DMaxVal0, DMaxVal0, DMaxVal1
	vmov.32		ctMaxVal, DMaxVal0[0]
	cmp			ctMaxVal, #0
	beq			gTileloopBreak

@ if (fUseQuantStep == WMAB_TRUE) {
	cmp		fUseQuantStep, #1
	bne		NotUseQuantStep

@ maxQuantStep = ppcinfo->m_qfltMaxQuantStep@
  ldr    iFraction, [ppcinfo, #PerChannelInfo_m_qfltMaxQuantStep+4]
  cmp    iFraction, #0
  beq    gTileloopBreak
  
@ ctMaxVal = ctMaxVal*maxQuantStep.iFraction@
	ldr		iMaxOutputAllowed, [sp, #iOffset_iMaxOutputAllowed]
  smull  ctMaxValLow, ctMaxVal, iFraction, ctMaxVal
  ldr    iFracBits, [ppcinfo, #PerChannelInfo_m_qfltMaxQuantStep]
 
@ cLeftShiftBits = (I32)(iMaxOutputAllowed - LOG2CEIL_64(ctMaxVal) + maxQuantStep.iFracBits)
  sub    cLeftShiftBits, iMaxOutputAllowed, #32
  add    cLeftShiftBits, cLeftShiftBits, iFracBits

  cmp    ctMaxVal, #0
  bne    countHigh32

countLow32:
  movs   ctMaxValLow, ctMaxValLow, LSR #1

  sub    cLeftShiftBits, cLeftShiftBits, #1
  bne    countLow32

  add    cLeftShiftBits, cLeftShiftBits, #32
  b      UpdateLeftShiftBitsMin

countHigh32:
  movs   ctMaxVal, ctMaxVal, LSR #1

  sub    cLeftShiftBits, cLeftShiftBits, #1
  bne    countHigh32
  b      UpdateLeftShiftBitsMin

NotUseQuantStep:

@ cLeftShiftBits = (I32)(iMaxOutputAllowed - LOG2CEIL_32(ctMaxVal))@
	ldr		iMaxOutputAllowed, [sp, #iOffset_iMaxOutputAllowed]
  mov    cLeftShiftBits, iMaxOutputAllowed

count32:
  movs   ctMaxVal, ctMaxVal, LSR #1

  sub    cLeftShiftBits, cLeftShiftBits, #1
  bne    count32

UpdateLeftShiftBitsMin:
@ if (cLeftShiftBits<cLeftShiftBitsMin && cLeftShiftBits!=0)
@    cLeftShiftBitsMin = cLeftShiftBits@
  cmp    cLeftShiftBits, #0
  beq    gTileloopBreak
  
  cmp    cLeftShiftBits, cLeftShiftBitsMin
  movlt  cLeftShiftBitsMin, cLeftShiftBits
@ if (!fUseGlobalScale) {
  cmp    fUseGlobalScale, #0
  bne    gTileloopBreak

@ ppcinfo->m_cLeftShiftBitsTotal += cLeftShiftBits@
  ldr    T1, [ppcinfo, #PerChannelInfo_m_cLeftShiftBitsTotal]
  ldr    rgiCoefRecon, [sp, #iOffset_rgiCoefRecon]
  add    T1, T1, cLeftShiftBits

	ldr		iRecon, [sp, #iOffset_iRecon]
  
	str    T1, [ppcinfo, #PerChannelInfo_m_cLeftShiftBitsTotal]
  
	vdup.32		QLeftCount, cLeftShiftBits
ShiftLoop:
	vld4.32		{A0, AA1, AA2, AA3}, [rgiCoefRecon]
	vqshl.S32	B0, B0, QLeftCount
	vqshl.S32	B1, B1, QLeftCount
	subs		iRecon, iRecon, #1
	vst4.32		{A0, AA1, AA2, AA3}, [rgiCoefRecon]!	
	bne			ShiftLoop
			
gTileloopBreak:
  ldr    cChInTile, [sp, #iOffset_cChInTile]
  add    iCh, iCh, #1
  cmp    iCh, cChInTile
  blt    gTileLoop

@ if (fUseGlobalScale) {
  cmp    fUseGlobalScale, #1
  bne    auPreScaleCoeffsV3_Exit

  mov    iCh2, #0
  ldr    rgiChInTile2, [pau, #CAudioObject_m_rgiChInTile]
  ldr    rgpcinfo, [pau, #CAudioObject_m_rgpcinfo]
 
gOutTileLoop:
@ for (iCh = 0@ iCh < pau->m_cChInTile@ iCh++)

@  ppcinfo = &pau->m_rgpcinfo[pau->m_rgiChInTile[iCh]]@
@  rgiCoefRecon = ppcinfo->m_rgiCoefRecon@

  ldrsh  T2, [rgiChInTile2], #2

  mov    T1, #PerChannelInfo_size
  mla    ppcinfo, T2, T1, rgpcinfo

	ldr		iRecon, [sp, #iOffset_iRecon]
  
  
  
@ ppcinfo->m_cLeftShiftBitsTotal += cLeftShiftBits@
  ldr    rgiCoefRecon, [ppcinfo, #PerChannelInfo_m_rgiCoefRecon]
  ldr    T1, [ppcinfo, #PerChannelInfo_m_cLeftShiftBitsTotal]
  add    T1, T1, cLeftShiftBitsMin
  
	@// m_rgiCoefRecon can be NULL ( under CX_DECODE_MONO )
	@// if this is the case, skip the shift adjustment for
	@// un-allocated CoefRecon buffers.
	@if( NULL == rgiCoefRecon ) continue@
  cmp		rgiCoefRecon, #0				
  beq		gOutTileloopBreak
  
  cmp    cLeftShiftBitsMin, #0
  beq    gOutTileloopBreak
  str    T1, [ppcinfo, #PerChannelInfo_m_cLeftShiftBitsTotal]

	vdup.32		QLeftCount, cLeftShiftBitsMin
OutShiftLoop:	
	vld4.32		{A0, AA1, AA2, AA3}, [rgiCoefRecon]
	vqshl.S32	B0, B0, QLeftCount
	vqshl.S32	B1, B1, QLeftCount
	subs		iRecon, iRecon, #1
	vst4.32		{A0, AA1, AA2, AA3}, [rgiCoefRecon]!	
	bne			OutShiftLoop
	
gOutTileloopBreak:
  add    iCh2, iCh2, #1
  cmp    iCh2, cChInTile
  blt    gOutTileLoop

auPreScaleCoeffsV3_Exit:
  mov   r0, #0
  add   sp, sp, #iStackSpaceRev      @ give back rev stack space  
  ldmfd sp!, {r4 - r12, PC} @auPreScaleCoeffsV3
  @ENTRY_END auPreScaleCoeffsV3


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

ChannelInfo_size        .req r7
m_bNoDecodeForCx		.req r8

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@  NEON Registers for auPostScaleCoeffsV3
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
A0					.req	D0
AA1					.req	D1
AA2					.req	D2
AA3					.req	D3

B0					.req	Q0
B1					.req	Q1

QLeftCount			.req	Q6

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.if LINUX_RVDS == 1	
	PRESERVE8
	.endif
@	AREA    |.text|, CODE
_auPostScaleCoeffsV3:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pau
	stmfd sp!, {r4 - r8, lr}

  add   Temp1, pauPost, #CAudioObject_m_cChInTile
  mov   iChPost, #0
  ldrsh cChInTilePost, [Temp1]
  ldr   rgiChInTilePost, [pauPost, #CAudioObject_m_rgiChInTile]

  mov   ChannelInfo_size, #PerChannelInfo_size
 
gTileLoopPost:
@ for (iCh = 0@ iCh < pau->m_cChInTile@ iCh++) {

@ ppcinfo = &pau->m_rgpcinfo[pau->m_rgiChInTile[iCh]]@
@ rgiCoefRecon = ppcinfo->m_rgiCoefRecon@

  ldr   ppcinfoPost, [pauPost, #CAudioObject_m_rgpcinfo]
  ldrsh Temp1, [rgiChInTilePost], #2
  mla   ppcinfoPost, ChannelInfo_size, Temp1, ppcinfoPost
  add   iChPost, iChPost, #1

  ldrsh iReconPost, [ppcinfoPost, #PerChannelInfo_m_cSubbandAdjusted]
  ldr   rgiCoefReconPost, [ppcinfoPost, #PerChannelInfo_m_rgiCoefRecon]
  
@ if (ppcinfo->m_bNoDecodeForCx)
@            continue@
	ldr		m_bNoDecodeForCx, [ppcinfoPost, #PerChannelInfo_m_bNoDecodeForCx]
	cmp		m_bNoDecodeForCx, #0
	bne		gTileloopBreakPost
	
@ ppcinfo->m_cLeftShiftBitsTotal -= ppcinfo->m_cLeftShiftBitsQuant@
@ ppcinfo->m_cLeftShiftBitsTotal -= pau->m_cLeftShiftBitsFixedPost@

  ldr   cLeftShiftBitsQuant, [ppcinfoPost, #PerChannelInfo_m_cLeftShiftBitsQuant]
  ldr   cLeftShiftBitsTotal, [ppcinfoPost, #PerChannelInfo_m_cLeftShiftBitsTotal]
  ldr   cLeftShiftBitsFixedPost, [pauPost, #CAudioObject_m_cLeftShiftBitsFixedPost]

  sub   cLeftShiftBitsTotal, cLeftShiftBitsTotal, cLeftShiftBitsQuant
@  ldr   cLeftShiftBitsFixedPre, [pauPost, #CAudioObject_m_cLeftShiftBitsFixedPre]

  sub   cLeftShiftBitsTotal, cLeftShiftBitsTotal, cLeftShiftBitsFixedPost
@  add   cLeftShiftBitsTotal, cLeftShiftBitsTotal, cLeftShiftBitsFixedPre

@ ppcinfo->m_cLeftShiftBitsTotal = 0@
@ ppcinfo->m_cLeftShiftBitsQuant = 0@
  
  mov   Temp1, #0
  str   Temp1, [ppcinfoPost, #PerChannelInfo_m_cLeftShiftBitsTotal]
  str   Temp1, [ppcinfoPost, #PerChannelInfo_m_cLeftShiftBitsQuant]
	
	cmp		cLeftShiftBitsTotal, #0
	beq		gTileloopBreakPost
	rsb		cLeftShiftBitsTotal, cLeftShiftBitsTotal, #0
	
	vdup.32		QLeftCount, cLeftShiftBitsTotal
ShiftLoopPost:	
	vld4.32		{A0, AA1, AA2, AA3}, [rgiCoefReconPost]
	vqshl.S32	B0, B0, QLeftCount
	vqshl.S32	B1, B1, QLeftCount
	subs		iReconPost, iReconPost, #8
	vst4.32		{A0, AA1, AA2, AA3}, [rgiCoefReconPost]!	
	bgt			ShiftLoopPost
	
gTileloopBreakPost:
  cmp    iChPost, cChInTilePost
  blt    gTileLoopPost

  mov   r0, #0
	ldmfd sp!, {r4 - r8, PC} @auPostScaleCoeffsV3
  @ENTRY_END auPostScaleCoeffsV3



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

.set	iStackSpaceRevSpec            , 4
@iOffset_iFracBitsSpec         EQU iStackSpaceRevSpec-4		
@iOffset_iFractionSpec         EQU iStackSpaceRevSpec-8		
.set	iOffset_fMaskUpdateSpec       , iStackSpaceRevSpec-4

@cLastCodedIndexV3OffsetPart1  EQU CAudioObject_m_cLastCodedIndexV3-214
@cLastCodedIndexV3OffsetPart2  EQU 214

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.if LINUX_RVDS == 1	
	PRESERVE8
	.endif
@	AREA    |.text|, CODE
_auInvWeightSpectrumV3:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pau
@ r1 = ppcinfo
@ r2 = fMaskUpdate


  stmfd sp!, {r4 - r12, lr}
  sub   sp, sp, #iStackSpaceRevSpec      @ rev stack space

  str   r2, [sp, #iOffset_fMaskUpdateSpec]
  mov   pauSpec, r0
  mov   ppcinfoSpec, r1

@ if (ppcinfo->m_cSubFrameSampleHalfWithUpdate <= 0 || 
@     ppcinfo->m_cSubFrameSampleHalf <= 0)
@       wmaResult = WMA_E_BROKEN_FRAME@

  ldr   r0, [ppcinfoSpec, #PerChannelInfo_m_cSubFrameSampleHalfWithUpdate]
  ldrsh r1, [ppcinfoSpec, #PerChannelInfo_m_cSubFrameSampleHalf]

  cmp   r0, #0
  ble   auInvWeightSpectrumV3_error

  cmp   r1, #0
  ble   auInvWeightSpectrumV3_error
  
@ rgiBarkIndex = pau->m_rgiBarkIndex@
  ldr   rgiBarkIndexSpec, [pauSpec, #CAudioObject_m_rgiBarkIndex]

@ cValidBarkBand = pau->m_cValidBarkBand@
  ldr   cValidBarkBandSpec, [pauSpec, #CAudioObject_m_cValidBarkBand]

@ if (pau->m_bFreqex && pau->m_bDoFexOnCodedChannels)
@		iHighCutOff = ppcinfo->m_cSubbandAdjusted@
	ldr		r0, [pauSpec, #CAudioObject_m_bFreqex]
	cmp		r0, #0
	beq		FexDisable
	ldr		r0, [pauSpec, #CAudioObject_m_bDoFexOnCodedChannels]
	cmp		r0, #0
	beq		FexDisable	
	ldrsh	iHighCutOffSpec, [ppcinfoSpec, #PerChannelInfo_m_cSubbandAdjusted]
	b		gOutSpec
	
FexDisable:
@pau->m_cLastCodedIndex
	mov		r0, #CAudioObject_m_cLastCodedIndex
	ldrh	iHighCutOffSpec, [pauSpec, r0]
  	cmp   	iHighCutOffSpec, r1		@r1 = ppcinfo->m_cSubFrameSampleHalf
  	movgt 	iHighCutOffSpec, r1
  
gOutSpec:
	mov   	iBarkSpec, #0
  cmp   cValidBarkBandSpec, #0
  ble   auInvWeightSpectrumV3_Exit

gOutLoopSpec:
@ for (iBark = 0@ iBark < cValidBarkBand@ iBark++)

@ iLo = rgiBarkIndex [iBark]@
@ iHi = min(iHighCutOff, (rgiBarkIndex [iBark + 1]))@
	ldrd	iLoSpec, [rgiBarkIndexSpec], #4				@iLoSpec = [rgiBarkIndexSpec], #4, iHiSpec = [rgiBarkIndexSpec]
@ rgiCoefRecon = (CoefType*) ppcinfo->m_rgiCoefRecon@
	ldr   rgiCoefReconSpec, [ppcinfoSpec, #PerChannelInfo_m_rgiCoefRecon]
	
	cmp   iHiSpec, iHighCutOffSpec
	movgt iHiSpec, iHighCutOffSpec
	
	cmp   iHiSpec, iLoSpec
	ble   gOutLoopBreakSpec
	
	sub		iHi_LoSpec, iHiSpec, iLoSpec
	add   rgiCoefReconSpec, rgiCoefReconSpec, iLoSpec, LSL #2

@ qfltQuantizer = prvWeightedModifiedQuantizationV3(pau,ppcinfo,iBark,fMaskUpdate)@
	ldr   r3, [sp, #iOffset_fMaskUpdateSpec]
	mov   r0, pauSpec
	mov   r1, ppcinfoSpec
	mov   r2, iBarkSpec
	bl    _prvWeightedModifiedQuantizationV3
  
	ldrd	iFracBitsSpec, [r0]			@iFracBitsSpec = [r0], iFractionSpec = [r0 + 4]	

	
@if ((0 > qfltQuantizer.iFracBits) || (64 <= qfltQuantizer.iFracBits))
@		REPORT_BITSTREAM_CORRUPTION_AND_EXIT(wmaResult)@	
	cmp		iFracBitsSpec, #0
	blt		auInvWeightSpectrumV3_error
	cmp		iFracBitsSpec, #64
	bge		auInvWeightSpectrumV3_error	
	
  rsbs  iShiftSpec, iFracBitsSpec, #32
  ldr   iCoefSpec, [rgiCoefReconSpec]
  bpl   gInnerLoopSpecPos

	rsb		iShiftSpec, iShiftSpec, #0
	
gInnerLoopSpecNeg:
@ for (iRecon = iLo@ iRecon < iHi@ iRecon++)
@ rgiCoefRecon [iRecon] = MULT_QUANT_AND_SCALE(rgiCoefRecon [iRecon],qfltQuantizer)@
  smmul iHiSpec, iFractionSpec, iCoefSpec  
  subs  iHi_LoSpec, iHi_LoSpec, #1

  mov   iCoefSpec, iHiSpec, ASR iShiftSpec

  str   iCoefSpec, [rgiCoefReconSpec], #4
  ldr   iCoefSpec, [rgiCoefReconSpec]
  bne   gInnerLoopSpecNeg
  b     gOutLoopBreakSpec

gInnerLoopSpecPos:
@ for (iRecon = iLo@ iRecon < iHi@ iRecon++)
@ rgiCoefRecon [iRecon] = MULT_QUANT_AND_SCALE(rgiCoefRecon [iRecon],qfltQuantizer)@
  smmul iHiSpec, iFractionSpec, iCoefSpec  
  subs  iHi_LoSpec, iHi_LoSpec, #1

  mov   iCoefSpec, iHiSpec, LSL iShiftSpec

  str   iCoefSpec, [rgiCoefReconSpec], #4
  ldr   iCoefSpec, [rgiCoefReconSpec]
  bne   gInnerLoopSpecPos
  
gOutLoopBreakSpec:
  add   iBarkSpec, iBarkSpec, #1
  cmp   iBarkSpec, cValidBarkBandSpec
  blt   gOutLoopSpec

  	mov   	r0,  #0 
	b		auInvWeightSpectrumV3_Exit
  
auInvWeightSpectrumV3_error:
	@ WMA_E_BROKEN_FRAME    0x80040002
	mov   r1, #0x80000002
	orr   r0, r1, #0x40000

auInvWeightSpectrumV3_Exit:

  add   sp, sp, #iStackSpaceRevSpec      @ give back rev stack space  
  ldmfd sp!, {r4 - r12, PC} @auInvWeightSpectrumV3
  @ENTRY_END auInvWeightSpectrumV3


  .endif @ WMA_OPT_SCALE_COEFFS_V3_ARM
  .endif	@//IF	ARMVERSION	>=7
  
  
