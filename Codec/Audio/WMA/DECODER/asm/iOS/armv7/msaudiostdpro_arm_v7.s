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
@//     Witten Wen (Shanghai, China) October 10, 2008
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
  .if WMA_OPT_SUBFRAMERECON_ARM == 1
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


  .globl  _auSubframeRecon_Std
  .globl  _auSubframeRecon_MonoPro


@//*************************************************************************************
@//
@// WMARESULT auSubframeRecon_Std ( CAudioObject* pau, PerChannelInfo* ppcinfo, PerChannelInfo* ppcinfo2)
@//
@//*************************************************************************************

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Registers for auSubframeRecon_Std
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

piCurrCoef          .req  r4
piPrevCoef          .req  r5

piCurrCoef2         .req  r12
piPrevCoef2			.req  r14

i					.req  r3
iOverlapSize		.req  r6
iShift				.req  r3
bp2Step				.req  r7

iSizeCurr			.req  r8
iSizePrev			.req  r9

temp1             	.req  r6
temp2             	.req  r7
temp3             	.req	r1
temp4				.req	r8

cfPrevData          .req  r10
cfCurrData          .req  r11

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Neon Registers for auSubframeRecon_Std
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
A0					.req	D0
AA1					.req	D1
AA2					.req	D2
AA3					.req	D3

B0					.req	D4
B1					.req	D5
B2					.req	D6
B3					.req	D7

CC0					.req	D8
CC1					.req	D9
CC2					.req	D10
CC3					.req	D11

DD0					.req	D12
DD1					.req	D13
DD2					.req	D14
DD3					.req	D15

QA0					.req	Q0
QA1					.req	Q1
QB0					.req	Q2
QB1					.req	Q3
QC0					.req	Q4
QC1					.req	Q5
QD0					.req	Q6
QD1					.req	Q7

DcfPrevData			.req	D0
DcfCurrData			.req	D1
Dbp2Sin_Cos			.req	D2
DBp2Sin1_Cos1		.req	D3
Dtemp1_0			.req	D4
Dtemp1_1			.req	D5
Dtemp2_0			.req	D6
Dtemp2_1			.req	D7
DiShift				.req	D8
Dbp2step			.req	D10

QData				.req	Q0
Qtemp1				.req	Q2
Qtemp2				.req	Q3
QiShift				.req	Q4
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@  AREA    |.text|, CODE
_auSubframeRecon_Std:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pau
@ r1 = ppcinfo
@ r2 = ppcinfo2


  stmfd sp!, {r4 - r11, lr}
@ iSizeCurr = ppcinfo->m_iSizeCurr@
  ldrsh iSizeCurr, [r1, #PerChannelInfo_m_iSizeCurr]

@ piCurrCoef = (CoefType *)ppcinfo->m_rgiCoefRecon@   // now reverse the coef buffer
  ldr   piCurrCoef, [r1, #PerChannelInfo_m_rgiCoefRecon]
  
@ piCurrCoef2 = (CoefType *)ppcinfo2->m_rgiCoefRecon@   // now reverse the coef buffer
  ldr   piCurrCoef2, [r2, #PerChannelInfo_m_rgiCoefRecon]

  mov   temp1, piCurrCoef
  mov   temp2, piCurrCoef2

@ for(i = 0@ i < iSizeCurr >> 1@ i++) {
@  mov   i, iSizeCurr, ASR #2                        @ i = iSizeCurr/4
	mov   i, iSizeCurr, ASR #4                        @ i = iSizeCurr/16
  add   piPrevCoef, piCurrCoef, iSizeCurr, LSL #2
  add   piPrevCoef2, piCurrCoef2, iSizeCurr, LSL #2
  sub   piPrevCoef, piPrevCoef, #32                 @ piPrevCoef = &piCurrCoef[iSizeCurr - 8]
  sub   piPrevCoef2, piPrevCoef2, #32               @ piPrevCoef2 = &piCurrCoef2[iSizeCurr - 8]

LoopMemMove:
@ cfCurrData = piCurrCoef[i]@
@ piCurrCoef[i] = piCurrCoef[iSizeCurr - 1 - i]@
@ piCurrCoef[iSizeCurr - 1 - i] = cfCurrData@

@ cfCurrData = piCurrCoef2[i]@
@ piCurrCoef2[i] = piCurrCoef2[iSizeCurr - 1 - i]@
@ piCurrCoef2[iSizeCurr - 1 - i] = cfCurrData@
	vld1.32		{B0, B1, B2, B3}, [piCurrCoef]
	vld1.32		{A0, AA1, AA2, AA3}, [piPrevCoef]
	vrev64.32	QB0, QB0
	vld1.32		{DD0, DD1, DD2, DD3}, [piCurrCoef2]
	vrev64.32	QB1, QB1
	vld1.32		{CC0, CC1, CC2, CC3}, [piPrevCoef2]
	vrev64.32	QA0, QA0
	vrev64.32	QA1, QA1
	vswp.S32	B0, B3
	vrev64.32	QD0, QD0
	vswp.S32	B1, B2
	vrev64.32	QD1, QD1
	vswp.S32	A0, AA3
	vrev64.32	QC0, QC0
	vswp.S32	AA1, AA2
	vrev64.32	QC1, QC1
	vswp.S32	DD0, DD3
	vst1.32		{B0, B1, B2, B3}, [piPrevCoef]		
	vswp.s32	DD1, DD2
	sub			piPrevCoef, piPrevCoef, #32
	vswp.S32	CC0, CC3
	vst1.32		{A0, AA1, AA2, AA3}, [piCurrCoef]!	
	vswp.S32	CC1, CC2
	subs		i, i, #1	
	vst1.32		{DD0, DD1, DD2, DD3}, [piPrevCoef2]
	sub			piPrevCoef2, piPrevCoef2, #32
	vst1.32		{CC0, CC1, CC2, CC3}, [piCurrCoef2]!
	@// 23 stalls
	bne 		LoopMemMove
	
@ // ---- Setup the coef buffer pointer ----
@ piPrevCoef = (CoefType *)ppcinfo->m_rgiCoefRecon - iSizePrev / 2@       // go forward
@ piCurrCoef = (CoefType *)ppcinfo->m_rgiCoefRecon + iSizeCurr / 2 - 1@   // go backward
  
  ldrsh iSizePrev, [r1, #PerChannelInfo_m_iSizePrev]
  mov   i, iSizeCurr, ASR #1                        @ i = iSizeCurr/2	
  ands	i, i, #7
  beq	i_is_even
@the odd reverse
	add	piPrevCoef, piPrevCoef, #28
	add	piPrevCoef2, piPrevCoef2, #28
OddRevease:	
	ldr   cfPrevData, [piPrevCoef]
	ldr   cfCurrData, [piCurrCoef]
	str   cfPrevData, [piCurrCoef], #4
	str   cfCurrData, [piPrevCoef], #-4

@ cfCurrData = piCurrCoef2[i]@
@ piCurrCoef2[i] = piCurrCoef2[iSizeCurr - 1 - i]@
@ piCurrCoef2[iSizeCurr - 1 - i] = cfCurrData@	
	ldr   cfPrevData, [piPrevCoef2]
	ldr   cfCurrData, [piCurrCoef2]
	str   cfPrevData, [piCurrCoef2], #4
	str   cfCurrData, [piPrevCoef2], #-4
	subs	i, i, #1
	bne		OddRevease

  mov   piCurrCoef, piPrevCoef
  sub   piPrevCoef, temp1, iSizePrev, LSL #1    
  mov   piCurrCoef2, piPrevCoef2  
  bne	i_is_odd
i_is_even:   
@ piPrevCoef2 = (CoefType *)ppcinfo2->m_rgiCoefRecon - iSizePrev / 2@       // go forward
@ piCurrCoef2 = (CoefType *)ppcinfo2->m_rgiCoefRecon + iSizeCurr / 2 - 1@   // go backward
  add   piCurrCoef, piPrevCoef, #28
  sub   piPrevCoef, temp1, iSizePrev, LSL #1
  add   piCurrCoef2, piPrevCoef2, #28  
  
i_is_odd:
@ piCurrCoef2 = (CoefType *)ppcinfo2->m_rgiCoefRecon + iSizeCurr / 2 - 1@   // go backward
  sub   piPrevCoef2, temp2, iSizePrev, LSL #1
  
@ if ((pau->m_bUnifiedPureLLMCurrFrm == WMAB_TRUE) && (pau->m_bFirstUnifiedPureLLMFrm == WMAB_FALSE))
@       return WMA_OK@ 
  ldr   temp1, [r0, #CAudioObject_m_bUnifiedPureLLMCurrFrm]
  ldr   temp2, [r0, #CAudioObject_m_bFirstUnifiedPureLLMFrm]
  cmp   temp1, #1
  bne   ResetBuffer

  cmp   temp2, #0
  beq   auSubframeRecon_StdExit

ResetBuffer:
@ if( iSizeCurr > iSizePrev ){
  cmp   iSizeCurr, iSizePrev
  ble   ResetBufferLE

@ piCurrCoef -= (iSizeCurr - iSizePrev) >> 1@
@ piCurrCoef2 -= (iSizeCurr - iSizePrev) >> 1@
  sub   temp1, iSizeCurr, iSizePrev
  sub   piCurrCoef, piCurrCoef, temp1, LSL #1
  sub   piCurrCoef2, piCurrCoef2, temp1, LSL #1

@ iOverlapSize = iSizePrev >> 1@
  mov   iOverlapSize, iSizePrev, ASR #1
  b     Overlap

ResetBufferLE:
@ if (iSizeCurr < iSizePrev)
  beq   ResetBufferEQ

@ piPrevCoef += ((iSizePrev - iSizeCurr) >> 1)@
@ piPrevCoef2 += ((iSizePrev - iSizeCurr) >> 1)@
  sub   temp1, iSizePrev, iSizeCurr
  add   piPrevCoef, piPrevCoef, temp1, LSL #1
  add   piPrevCoef2, piPrevCoef2, temp1, LSL #1

ResetBufferEQ:
@ iOverlapSize = iSizeCurr >> 1@
  mov   iOverlapSize, iSizeCurr, ASR #1

Overlap:
	ldr   iShift, [r0, #CAudioObject_m_cLeftShiftBitsFixedPost] @ coefShift = pau->m_cLeftShiftBitsFixedPost
	vldr	Dbp2Sin_Cos, [r1, #PerChannelInfo_m_fiSinRampUpStart]	@ bp2Sin_Cos[0]  = ppcinfo->m_fiSinRampUpStart@
																	@ bp2Sin_Cos[1]  = ppcinfo->m_fiCosRampUpStart@
@ bp2Step = ppcinfo->m_fiSinRampUpStep@
	ldr   bp2Step,  [r1, #PerChannelInfo_m_fiSinRampUpStep]
	rsb		temp4, bp2Step, #0
	vmov	Dbp2step, temp4, bp2Step						@Dbp2step[0] = -bp2Step@ Dbp2step[1] = bp2Step

	vld1.32		{DcfPrevData[0]}, [piPrevCoef]
	vld1.32		{DcfPrevData[1]}, [piPrevCoef2]
	vshl.S32	Dbp2step, #1						@use double result multiply, so left shift (2-1)bit
	vld1.32		{DcfCurrData[0]}, [piCurrCoef]
	vld1.32		{DcfCurrData[1]}, [piCurrCoef2]
	vldr		DBp2Sin1_Cos1, [r1, #PerChannelInfo_m_fiSinRampUpPrior]	@ bp2Sin1_Cos1[0]  = ppcinfo->m_fiSinRampUpPrior@
																	@ bp2Sin1_Cos1[1]  = ppcinfo->m_fiCosRampUpPrior@
	rsb			iShift, iShift, #1
	vdup.32		DiShift, iShift
	vmovl.S32	QiShift, DiShift
LOOPOverlap:
@ for(i = 0@ i < iOverlapSize@ i++ ){

@ cfPrevData = *piPrevCoef@
@ cfCurrData = *piCurrCoef@
@ *piPrevCoef++ = INT_FROM_COEF( MULT_BP2(-bp2Sin, cfCurrData) + MULT_BP2(bp2Cos, cfPrevData) )@
@ *piCurrCoef-- = INT_FROM_COEF( MULT_BP2(bp2Sin, cfPrevData) + MULT_BP2(bp2Cos, cfCurrData) )@
	vqdmull.S32		Qtemp1, DcfPrevData, Dbp2Sin_Cos[1]
	subs			iOverlapSize, iOverlapSize, #1
	vqdmull.S32		Qtemp2, DcfCurrData, Dbp2Sin_Cos[1]
	vqdmlsl.S32		Qtemp1, DcfCurrData, Dbp2Sin_Cos[0]		
	vqdmlal.S32		Qtemp2, DcfPrevData, Dbp2Sin_Cos[0]
	vshl.S64		Qtemp1, QiShift
	vshl.S64		Qtemp2, QiShift
	vst1.32			{Dtemp1_0[1]}, [piPrevCoef]!
	vst1.32			{Dtemp2_0[1]}, [piCurrCoef]
	sub				piCurrCoef, piCurrCoef, #4
	vst1.32			{Dtemp1_1[1]}, [piPrevCoef2]!	
	vst1.32			{Dtemp2_1[1]}, [piCurrCoef2]
	sub				piCurrCoef2, piCurrCoef2, #4
	vld1.32			{DcfPrevData[0]}, [piPrevCoef]
	vqdmulh.S32		Dtemp1_0, Dbp2step, Dbp2Sin_Cos
	vld1.32			{DcfPrevData[1]}, [piPrevCoef2]
	vld1.32			{DcfCurrData[0]}, [piCurrCoef]
	vld1.32			{DcfCurrData[1]}, [piCurrCoef2]	
	vrev64.32		Dtemp1_0, Dtemp1_0
	vmov		Dtemp1_1, Dbp2Sin_Cos
	vadd.s32		Dbp2Sin_Cos, Dtemp1_0, DBp2Sin1_Cos1
	vshl.s32		DBp2Sin1_Cos1, Dtemp1_1, #0
	bne				LOOPOverlap
auSubframeRecon_StdExit:
  ldmfd sp!, {r4 - r11, PC} @auSubframeRecon_Std
  @ENTRY_END auSubframeRecon_Std



@//*************************************************************************************
@//
@// WMARESULT WMARESULT auSubframeRecon_MonoPro ( CAudioObject* pau )
@//
@//*************************************************************************************

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Registers for auSubframeRecon_MonoPro
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

pau						.req  r0
iShift_pro				.req  r0

ppcinfo					.req  r1
cfPrevData_pro			.req  r2
cfPrevData_pro_next		.req	r3
cfCurrData_pro			.req  r10
cfCurrData_pro_next		.req	r11

piCurrCoef_pro			.req  r4
piPrevCoef_pro			.req  r5

bp2Sin_pro				.req  r6
bp2Cos_pro				.req  r7
bp2Sin1_pro				.req  r8
bp2Cos1_pro				.req  r9
bp2Step_pro				.req  r12

t1						.req  r14
t2						.req  r11
t3						.req  r1
iOverlapSize_pro		.req  r3

iChSrc					.req  r6
cChInTile_pro			.req  r7
iCh_pro					.req  r8
i_pro					.req  r8

iSizeCurr_pro			.req  r9
iSizePrev_pro			.req  r12

bp2SinT_pro				.req  r2
bp2CosT_pro				.req  r10

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Neon Registers for auSubframeRecon_Std
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
A0					.req	D0
AA1					.req	D1
AA2					.req	D2
AA3					.req	D3

B0					.req	D4
B1					.req	D5
B2					.req	D6
B3					.req	D7

QA0					.req	Q0
QA1					.req	Q1
QB0					.req	Q2
QB1					.req	Q3

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Constants for auSubframeRecon_MonoPro
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
.set	iStackSpaceRevPro        , 4*4
.set	iOffset_coefShiftPro     , iStackSpaceRevPro-4
.set	iOffset_cChInTilePro     , iStackSpaceRevPro-8
.set	iOffset_iChPro           , iStackSpaceRevPro-12
.set	iOffset_pauPro           , iStackSpaceRevPro-16

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@  AREA    |.text|, CODE
_auSubframeRecon_MonoPro:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pau


  stmfd sp!, {r4 - r11, lr}
  sub   sp, sp, #iStackSpaceRevPro @ rev stack space

@ iCh = pau->m_cChInTile
  add   t1, pau, #CAudioObject_m_cChInTile
  ldrsh cChInTile_pro, [t1]

  mov   iCh_pro, #0
  ldr   t2, [pau, #CAudioObject_m_cLeftShiftBitsFixedPost] @ coefShift = pau->m_cLeftShiftBitsFixedPost

  str   cChInTile_pro, [sp, #iOffset_cChInTilePro]
  str   t2, [sp, #iOffset_coefShiftPro]
  str   pau, [sp, #iOffset_pauPro]


@ for (iCh = 0@ iCh < pau->m_cChInTile@ iCh++) {
LoopChannel:

@ iChSrc = pau->m_rgiChInTile [iCh]@
  ldr   t1, [pau, #CAudioObject_m_rgiChInTile]
  add   t1, t1, iCh_pro, LSL #1
  ldrsh iChSrc, [t1]

@ ppcinfo = pau->m_rgpcinfo + iChSrc@
  ldr   ppcinfo, [pau, #CAudioObject_m_rgpcinfo]

  mov   t1, #PerChannelInfo_size
  mla   ppcinfo, t1, iChSrc, ppcinfo

  add   iCh_pro, iCh_pro, #1
  str   iCh_pro, [sp, #iOffset_iChPro]

@ iSizePrev = ppcinfo->m_iSizePrev@
  ldrsh iSizePrev_pro, [ppcinfo, #PerChannelInfo_m_iSizePrev]

@ iSizeCurr = ppcinfo->m_iSizeCurr@
  ldrsh iSizeCurr_pro, [ppcinfo, #PerChannelInfo_m_iSizeCurr]

@ piCurrCoef = (CoefType *)ppcinfo->m_rgiCoefRecon@   // now reverse the coef buffer
  ldr   piCurrCoef_pro, [ppcinfo, #PerChannelInfo_m_rgiCoefRecon]
  
  mov   t1, piCurrCoef_pro
  
  @ for(i = 0@ i < iSizeCurr >> 2@ i++) {
@  mov   i_pro, iSizeCurr_pro, ASR #2                            @ i = iSizeCurr/4
	mov   i_pro, iSizeCurr_pro, ASR #4                            @ i = iSizeCurr/16
  add   piPrevCoef_pro, piCurrCoef_pro, iSizeCurr_pro, LSL #2
  sub   piPrevCoef_pro, piPrevCoef_pro, #32                      @ piPrevCoef = &piCurrCoef[iSizeCurr - 8]
  
LoopMemMove_pro: 
	vld1.32		{B0, B1, B2, B3}, [piCurrCoef_pro]
	vld1.32		{A0, AA1, AA2, AA3}, [piPrevCoef_pro]
	subs		i_pro, i_pro, #1
	vrev64.32	QB0, QB0
	vrev64.32	QB1, QB1
	vrev64.32	QA0, QA0
	vrev64.32	QA1, QA1
	vswp.S32	B0, B3
	vswp.S32	B1, B2
	vswp.S32	A0, AA3
	vswp.S32	AA1, AA2
	vst1.32		{B0, B1, B2, B3}, [piPrevCoef_pro]
	sub			piPrevCoef_pro, piPrevCoef_pro, #32
	vst1.32		{A0, AA1, AA2, AA3}, [piCurrCoef_pro]!	
	@//11 stalls
	bne		LoopMemMove_pro
	
@  ldrd	cfCurrData_pro, [piCurrCoef_pro]
@  ldrd	cfPrevData_pro, [piPrevCoef_pro]  
@  subs  i_pro, i_pro, #1
@  str	cfCurrData_pro, [piPrevCoef_pro, #4]
@  str	cfCurrData_pro_next, [piPrevCoef_pro], #-8
@  str	cfPrevData_pro_next, [piCurrCoef_pro], #4
@  str	cfPrevData_pro, [piCurrCoef_pro], #4  
@  bne   LoopMemMove_pro
  
  mov   i_pro, iSizeCurr_pro, ASR #1                            @ i = iSizeCurr/2
  ands	i_pro, i_pro, #7
  beq	i_pro_no_odd
  
@ cfCurrData = piCurrCoef[i]@
@ piCurrCoef[i] = piCurrCoef[iSizeCurr - 1 - i]@
@ piCurrCoef[iSizeCurr - 1 - i] = cfCurrData@
	add	piPrevCoef_pro, piPrevCoef_pro, #28
OddReverse_pro:
	ldr		cfPrevData_pro, [piPrevCoef_pro]
	ldr		cfCurrData_pro, [piCurrCoef_pro]
	subs	i_pro, i_pro, #1
	str		cfPrevData_pro, [piCurrCoef_pro], #4
	str		cfCurrData_pro, [piPrevCoef_pro], #-4
	bne		OddReverse_pro
  
  mov   piCurrCoef_pro, piPrevCoef_pro
  sub   piPrevCoef_pro, t1, iSizePrev_pro, LSL #1  
  b		i_pro_odd
  
i_pro_no_odd:  
  add	piCurrCoef_pro, piPrevCoef_pro, #28
  sub   piPrevCoef_pro, t1, iSizePrev_pro, LSL #1  
i_pro_odd:
   
@ if ((pau->m_bUnifiedPureLLMCurrFrm == WMAB_TRUE) && (pau->m_bFirstUnifiedPureLLMFrm == WMAB_FALSE))
@       return WMA_OK@ 
  ldr   t1, [pau, #CAudioObject_m_bUnifiedPureLLMCurrFrm]
  ldr   t2, [pau, #CAudioObject_m_bFirstUnifiedPureLLMFrm]
  cmp   t1, #1
  bne   ResetBuffer_pro

  cmp   t2, #0
  beq   LoopChannelContinue

ResetBuffer_pro:
@ if( iSizeCurr > iSizePrev ){
  cmp   iSizeCurr_pro, iSizePrev_pro
  ble   ResetBufferLE_pro

@ piCurrCoef -= (iSizeCurr - iSizePrev) >> 1@
  sub   t1, iSizeCurr_pro, iSizePrev_pro
  sub   piCurrCoef_pro, piCurrCoef_pro, t1, LSL #1

@ iOverlapSize = iSizePrev >> 1@
  mov   iOverlapSize_pro, iSizePrev_pro, ASR #1
  b     Overlap_pro

ResetBufferLE_pro:
@ if (iSizeCurr < iSizePrev)
  beq   ResetBufferEQ_pro

@ piPrevCoef += ((iSizePrev - iSizeCurr) >> 1)@
  sub   t1, iSizePrev_pro, iSizeCurr_pro
  add   piPrevCoef_pro, piPrevCoef_pro, t1, LSL #1

ResetBufferEQ_pro:
@ iOverlapSize = iSizeCurr >> 1@
  mov   iOverlapSize_pro, iSizeCurr_pro, ASR #1

Overlap_pro:

  ldrd   bp2Sin_pro, [ppcinfo, #PerChannelInfo_m_fiSinRampUpStart]	@ bp2Sin  = ppcinfo->m_fiSinRampUpStart@
  																	@ bp2Cos  = ppcinfo->m_fiCosRampUpStart@
	ldr   cfPrevData_pro, [piPrevCoef_pro]

  ldr   bp2Step_pro, [ppcinfo, #PerChannelInfo_m_fiSinRampUpStep]	@ bp2Step = ppcinfo->m_fiSinRampUpStep@
	

  ldrd   bp2Sin1_pro, [ppcinfo, #PerChannelInfo_m_fiSinRampUpPrior]	@ bp2Sin1 = ppcinfo->m_fiSinRampUpPrior@
																	@ bp2Cos1 = ppcinfo->m_fiCosRampUpPrior@
	ldr   cfCurrData_pro, [piCurrCoef_pro]
	mov   cfPrevData_pro, cfPrevData_pro, LSL #2 
  mov   bp2Step_pro, bp2Step_pro, LSL #2


LOOPOverlap_pro:
@ for(i = 0@ i < iOverlapSize@ i++ ){


@ cfCurrData = *piCurrCoef@
@ *piPrevCoef++ = INT_FROM_COEF( MULT_BP2(-bp2Sin, cfCurrData) + MULT_BP2(bp2Cos, cfPrevData) )@
@ *piCurrCoef-- = INT_FROM_COEF( MULT_BP2(bp2Sin, cfPrevData) + MULT_BP2(bp2Cos, cfCurrData) )@  
	 	
	smmul	t2, bp2Cos_pro, cfPrevData_pro  		@ MULT_BP2(bp2Cos, cfPrevData)	
	mov   cfCurrData_pro, cfCurrData_pro, LSL #2
	ldr   iShift_pro, [sp, #iOffset_coefShiftPro]
	smmls	t2, bp2Sin_pro, cfCurrData_pro, t2  

	smmul	t3, bp2Cos_pro, cfCurrData_pro  @ MULT_BP2(bp2Cos, cfCurrData)
	subs  iOverlapSize_pro, iOverlapSize_pro, #1
	mov   t2, t2, ASR iShift_pro              @ t2 = INT_FROM_COEF( MULT_BP2(-bp2Sin, cfCurrData) + MULT_BP2(bp2Cos, cfPrevData) )

	smmla	t3, bp2Sin_pro, cfPrevData_pro, t3
	str   t2, [piPrevCoef_pro], #4  	

@ bp2SinT = bp2Sin1 + MULT_BP2(bp2Step,bp2Cos)@
@ bp2CosT = bp2Cos1 - MULT_BP2(bp2Step,bp2Sin)@
@ bp2Sin1 = bp2Sin@  bp2Sin = bp2SinT@
@ bp2Cos1 = bp2Cos@  bp2Cos = bp2CosT@
  
	smmla	bp2SinT_pro, bp2Step_pro, bp2Cos_pro, bp2Sin1_pro
	mov   t3, t3, ASR iShift_pro              @ t3 = INT_FROM_COEF( MULT_BP2(bp2Sin, cfPrevData) + MULT_BP2(bp2Cos, cfCurrData) )
	str   t3, [piCurrCoef_pro], #-4  

	smmls	bp2CosT_pro, bp2Step_pro, bp2Sin_pro, bp2Cos1_pro
@	add   bp2SinT_pro, bp2Sin1_pro, t2
	mov   bp2Sin1_pro, bp2Sin_pro
	mov   bp2Sin_pro, bp2SinT_pro

	ldr   cfPrevData_pro, [piPrevCoef_pro]	@ cfPrevData = *piPrevCoef@
@  sub   bp2CosT_pro, bp2Cos1_pro, t3
	mov   bp2Cos1_pro, bp2Cos_pro
	mov   bp2Cos_pro, bp2CosT_pro   
	ldr   cfCurrData_pro, [piCurrCoef_pro]	@ cfPrevData = *piPrevCoef@
	mov   cfPrevData_pro, cfPrevData_pro, LSL #2 
	bne   LOOPOverlap_pro

LoopChannelContinue:
  ldr   cChInTile_pro, [sp, #iOffset_cChInTilePro]
  ldr   iCh_pro, [sp, #iOffset_iChPro]
  ldr   pau, [sp, #iOffset_pauPro]

  cmp   iCh_pro, cChInTile_pro
  blt   LoopChannel

auSubframeRecon_ProExit:
  add   sp, sp, #iStackSpaceRevPro     @ give back rev stack space  
  ldmfd sp!, {r4 - r11, PC} @auSubframeRecon_MonoPro
  @ENTRY_END auSubframeRecon_MonoPro


	.endif	@//WMA_OPT_SUBFRAMERECON_ARM
  .endif	@//IF	ARMVERSION	>=7
