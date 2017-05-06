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
  #include "../../../../../inc/audio/v10/include/voWMADecID.h"
  .INCLUDE     "kxarm.h"
  .INCLUDE     "wma_member_arm.inc"
  .INCLUDE	  "wma_arm_version.h"
@  OPT         1       @ enable listing
 
@  AREA    |.text|, CODE, READONLY
 		.section .text
 		 
	.IF	ARMVERSION	>=7
  .IF WMA_OPT_SUBFRAMERECON_ARM == 1
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


  .global  auSubframeRecon_Std
  .global  auSubframeRecon_MonoPro


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
  LEAF_ENTRY auSubframeRecon_Std
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pau
@ r1 = ppcinfo
@ r2 = ppcinfo2


  STMFD sp!, {r4 - r11, lr}
@ iSizeCurr = ppcinfo->m_iSizeCurr@
  LDRSH iSizeCurr, [r1, #PerChannelInfo_m_iSizeCurr]

@ piCurrCoef = (CoefType *)ppcinfo->m_rgiCoefRecon@   // now reverse the coef buffer
  LDR   piCurrCoef, [r1, #PerChannelInfo_m_rgiCoefRecon]
  
@ piCurrCoef2 = (CoefType *)ppcinfo2->m_rgiCoefRecon@   // now reverse the coef buffer
  LDR   piCurrCoef2, [r2, #PerChannelInfo_m_rgiCoefRecon]

  MOV   temp1, piCurrCoef
  MOV   temp2, piCurrCoef2

@ for(i = 0@ i < iSizeCurr >> 1@ i++) {
@  MOV   i, iSizeCurr, ASR #2                        @ i = iSizeCurr/4
	MOV   i, iSizeCurr, ASR #4                        @ i = iSizeCurr/16
  ADD   piPrevCoef, piCurrCoef, iSizeCurr, LSL #2
  ADD   piPrevCoef2, piCurrCoef2, iSizeCurr, LSL #2
  SUB   piPrevCoef, piPrevCoef, #32                 @ piPrevCoef = &piCurrCoef[iSizeCurr - 8]
  SUB   piPrevCoef2, piPrevCoef2, #32               @ piPrevCoef2 = &piCurrCoef2[iSizeCurr - 8]

LoopMemMove:
@ cfCurrData = piCurrCoef[i]@
@ piCurrCoef[i] = piCurrCoef[iSizeCurr - 1 - i]@
@ piCurrCoef[iSizeCurr - 1 - i] = cfCurrData@

@ cfCurrData = piCurrCoef2[i]@
@ piCurrCoef2[i] = piCurrCoef2[iSizeCurr - 1 - i]@
@ piCurrCoef2[iSizeCurr - 1 - i] = cfCurrData@
	VLD1.32		{B0, B1, B2, B3}, [piCurrCoef]
	VLD1.32		{A0, AA1, AA2, AA3}, [piPrevCoef]
	VREV64.32	QB0, QB0
	VLD1.32		{DD0, DD1, DD2, DD3}, [piCurrCoef2]
	VREV64.32	QB1, QB1
	VLD1.32		{CC0, CC1, CC2, CC3}, [piPrevCoef2]
	VREV64.32	QA0, QA0
	VREV64.32	QA1, QA1
	VSWP.S32	B0, B3
	VREV64.32	QD0, QD0
	VSWP.S32	B1, B2
	VREV64.32	QD1, QD1
	VSWP.S32	A0, AA3
	VREV64.32	QC0, QC0
	VSWP.S32	AA1, AA2
	VREV64.32	QC1, QC1
	VSWP.S32	DD0, DD3
	VST1.32		{B0, B1, B2, B3}, [piPrevCoef]		
	VSWP.S32	DD1, DD2
	SUB			piPrevCoef, piPrevCoef, #32
	VSWP.S32	CC0, CC3
	VST1.32		{A0, AA1, AA2, AA3}, [piCurrCoef]!	
	VSWP.S32	CC1, CC2
	SUBS		i, i, #1	
	VST1.32		{DD0, DD1, DD2, DD3}, [piPrevCoef2]
	SUB			piPrevCoef2, piPrevCoef2, #32
	VST1.32		{CC0, CC1, CC2, CC3}, [piCurrCoef2]!
	@// 23 stalls
	BNE 		LoopMemMove
	
@ // ---- Setup the coef buffer pointer ----
@ piPrevCoef = (CoefType *)ppcinfo->m_rgiCoefRecon - iSizePrev / 2@       // go forward
@ piCurrCoef = (CoefType *)ppcinfo->m_rgiCoefRecon + iSizeCurr / 2 - 1@   // go backward
  
  LDRSH iSizePrev, [r1, #PerChannelInfo_m_iSizePrev]
  MOV   i, iSizeCurr, ASR #1                        @ i = iSizeCurr/2	
  ANDS	i, i, #7
  BEQ	i_is_even
@the odd reverse
	ADD	piPrevCoef, piPrevCoef, #28
	ADD	piPrevCoef2, piPrevCoef2, #28
OddRevease:	
	LDR   cfPrevData, [piPrevCoef]
	LDR   cfCurrData, [piCurrCoef]
	STR   cfPrevData, [piCurrCoef], #4
	STR   cfCurrData, [piPrevCoef], #-4

@ cfCurrData = piCurrCoef2[i]@
@ piCurrCoef2[i] = piCurrCoef2[iSizeCurr - 1 - i]@
@ piCurrCoef2[iSizeCurr - 1 - i] = cfCurrData@	
	LDR   cfPrevData, [piPrevCoef2]
	LDR   cfCurrData, [piCurrCoef2]
	STR   cfPrevData, [piCurrCoef2], #4
	STR   cfCurrData, [piPrevCoef2], #-4
	SUBS	i, i, #1
	BNE		OddRevease

  MOV   piCurrCoef, piPrevCoef
  SUB   piPrevCoef, temp1, iSizePrev, LSL #1    
  MOV   piCurrCoef2, piPrevCoef2  
  BNE	i_is_odd
i_is_even:   
@ piPrevCoef2 = (CoefType *)ppcinfo2->m_rgiCoefRecon - iSizePrev / 2@       // go forward
@ piCurrCoef2 = (CoefType *)ppcinfo2->m_rgiCoefRecon + iSizeCurr / 2 - 1@   // go backward
  ADD   piCurrCoef, piPrevCoef, #28
  SUB   piPrevCoef, temp1, iSizePrev, LSL #1
  ADD   piCurrCoef2, piPrevCoef2, #28  
  
i_is_odd:
@ piCurrCoef2 = (CoefType *)ppcinfo2->m_rgiCoefRecon + iSizeCurr / 2 - 1@   // go backward
  SUB   piPrevCoef2, temp2, iSizePrev, LSL #1
  
@ if ((pau->m_bUnifiedPureLLMCurrFrm == WMAB_TRUE) && (pau->m_bFirstUnifiedPureLLMFrm == WMAB_FALSE))
@       return WMA_OK@ 
  LDR   temp1, [r0, #CAudioObject_m_bUnifiedPureLLMCurrFrm]
  LDR   temp2, [r0, #CAudioObject_m_bFirstUnifiedPureLLMFrm]
  CMP   temp1, #1
  BNE   ResetBuffer

  CMP   temp2, #0
  BEQ   auSubframeRecon_StdExit

ResetBuffer:
@ if( iSizeCurr > iSizePrev ){
  CMP   iSizeCurr, iSizePrev
  BLE   ResetBufferLE

@ piCurrCoef -= (iSizeCurr - iSizePrev) >> 1@
@ piCurrCoef2 -= (iSizeCurr - iSizePrev) >> 1@
  SUB   temp1, iSizeCurr, iSizePrev
  SUB   piCurrCoef, piCurrCoef, temp1, LSL #1
  SUB   piCurrCoef2, piCurrCoef2, temp1, LSL #1

@ iOverlapSize = iSizePrev >> 1@
  MOV   iOverlapSize, iSizePrev, ASR #1
  B     Overlap

ResetBufferLE:
@ if (iSizeCurr < iSizePrev)
  BEQ   ResetBufferEQ

@ piPrevCoef += ((iSizePrev - iSizeCurr) >> 1)@
@ piPrevCoef2 += ((iSizePrev - iSizeCurr) >> 1)@
  SUB   temp1, iSizePrev, iSizeCurr
  ADD   piPrevCoef, piPrevCoef, temp1, LSL #1
  ADD   piPrevCoef2, piPrevCoef2, temp1, LSL #1

ResetBufferEQ:
@ iOverlapSize = iSizeCurr >> 1@
  MOV   iOverlapSize, iSizeCurr, ASR #1

Overlap:
	LDR   iShift, [r0, #CAudioObject_m_cLeftShiftBitsFixedPost] @ coefShift = pau->m_cLeftShiftBitsFixedPost
	VLDR	Dbp2Sin_Cos, [r1, #PerChannelInfo_m_fiSinRampUpStart]	@ bp2Sin_Cos[0]  = ppcinfo->m_fiSinRampUpStart@
																	@ bp2Sin_Cos[1]  = ppcinfo->m_fiCosRampUpStart@
@ bp2Step = ppcinfo->m_fiSinRampUpStep@
	LDR   bp2Step,  [r1, #PerChannelInfo_m_fiSinRampUpStep]
	RSB		temp4, bp2Step, #0
	VMOV	Dbp2step, temp4, bp2Step						@Dbp2step[0] = -bp2Step@ Dbp2step[1] = bp2Step

	VLD1.32		{DcfPrevData[0]}, [piPrevCoef]
	VLD1.32		{DcfPrevData[1]}, [piPrevCoef2]
	VSHL.S32	Dbp2step, #1						@use double result multiply, so left shift (2-1)bit
	VLD1.32		{DcfCurrData[0]}, [piCurrCoef]
	VLD1.32		{DcfCurrData[1]}, [piCurrCoef2]
	VLDR		DBp2Sin1_Cos1, [r1, #PerChannelInfo_m_fiSinRampUpPrior]	@ bp2Sin1_Cos1[0]  = ppcinfo->m_fiSinRampUpPrior@
																	@ bp2Sin1_Cos1[1]  = ppcinfo->m_fiCosRampUpPrior@
	RSB			iShift, iShift, #1
	VDUP.32		DiShift, iShift
	VMOVL.S32	QiShift, DiShift
LOOPOverlap:
@ for(i = 0@ i < iOverlapSize@ i++ ){

@ cfPrevData = *piPrevCoef@
@ cfCurrData = *piCurrCoef@
@ *piPrevCoef++ = INT_FROM_COEF( MULT_BP2(-bp2Sin, cfCurrData) + MULT_BP2(bp2Cos, cfPrevData) )@
@ *piCurrCoef-- = INT_FROM_COEF( MULT_BP2(bp2Sin, cfPrevData) + MULT_BP2(bp2Cos, cfCurrData) )@
	VQDMULL.S32		Qtemp1, DcfPrevData, Dbp2Sin_Cos[1]
	SUBS			iOverlapSize, iOverlapSize, #1
	VQDMULL.S32		Qtemp2, DcfCurrData, Dbp2Sin_Cos[1]
	VQDMLSL.S32		Qtemp1, DcfCurrData, Dbp2Sin_Cos[0]		
	VQDMLAL.S32		Qtemp2, DcfPrevData, Dbp2Sin_Cos[0]
	VSHL.S64		Qtemp1, QiShift
	VSHL.S64		Qtemp2, QiShift
	VST1.32			{Dtemp1_0[1]}, [piPrevCoef]!
	VST1.32			{Dtemp2_0[1]}, [piCurrCoef]
	SUB				piCurrCoef, piCurrCoef, #4
	VST1.32			{Dtemp1_1[1]}, [piPrevCoef2]!	
	VST1.32			{Dtemp2_1[1]}, [piCurrCoef2]
	SUB				piCurrCoef2, piCurrCoef2, #4
	VLD1.32			{DcfPrevData[0]}, [piPrevCoef]
	VQDMULH.S32		Dtemp1_0, Dbp2step, Dbp2Sin_Cos
	VLD1.32			{DcfPrevData[1]}, [piPrevCoef2]
	VLD1.32			{DcfCurrData[0]}, [piCurrCoef]
	VLD1.32			{DcfCurrData[1]}, [piCurrCoef2]	
	VREV64.32		Dtemp1_0, Dtemp1_0
	VMOV.S32		Dtemp1_1, Dbp2Sin_Cos
	VADD.S32		Dbp2Sin_Cos, Dtemp1_0, DBp2Sin1_Cos1
	VMOV.S32		DBp2Sin1_Cos1, Dtemp1_1	
	@//25 stalls
	BNE				LOOPOverlap
auSubframeRecon_StdExit:
  LDMFD sp!, {r4 - r11, PC} @auSubframeRecon_Std
  ENTRY_END auSubframeRecon_Std



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
.equ	iStackSpaceRevPro        , 4*4
.equ	iOffset_coefShiftPro     , iStackSpaceRevPro-4
.equ	iOffset_cChInTilePro     , iStackSpaceRevPro-8
.equ	iOffset_iChPro           , iStackSpaceRevPro-12
.equ	iOffset_pauPro           , iStackSpaceRevPro-16

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@  AREA    |.text|, CODE
  LEAF_ENTRY auSubframeRecon_MonoPro
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pau


  STMFD sp!, {r4 - r11, lr}
  SUB   sp, sp, #iStackSpaceRevPro @ rev stack space

@ iCh = pau->m_cChInTile
  ADD   t1, pau, #CAudioObject_m_cChInTile
  LDRSH cChInTile_pro, [t1]

  MOV   iCh_pro, #0
  LDR   t2, [pau, #CAudioObject_m_cLeftShiftBitsFixedPost] @ coefShift = pau->m_cLeftShiftBitsFixedPost

  STR   cChInTile_pro, [sp, #iOffset_cChInTilePro]
  STR   t2, [sp, #iOffset_coefShiftPro]
  STR   pau, [sp, #iOffset_pauPro]


@ for (iCh = 0@ iCh < pau->m_cChInTile@ iCh++) {
LoopChannel:

@ iChSrc = pau->m_rgiChInTile [iCh]@
  LDR   t1, [pau, #CAudioObject_m_rgiChInTile]
  ADD   t1, t1, iCh_pro, LSL #1
  LDRSH iChSrc, [t1]

@ ppcinfo = pau->m_rgpcinfo + iChSrc@
  LDR   ppcinfo, [pau, #CAudioObject_m_rgpcinfo]

  MOV   t1, #PerChannelInfo_size
  MLA   ppcinfo, t1, iChSrc, ppcinfo

  ADD   iCh_pro, iCh_pro, #1
  STR   iCh_pro, [sp, #iOffset_iChPro]

@ iSizePrev = ppcinfo->m_iSizePrev@
  LDRSH iSizePrev_pro, [ppcinfo, #PerChannelInfo_m_iSizePrev]

@ iSizeCurr = ppcinfo->m_iSizeCurr@
  LDRSH iSizeCurr_pro, [ppcinfo, #PerChannelInfo_m_iSizeCurr]

@ piCurrCoef = (CoefType *)ppcinfo->m_rgiCoefRecon@   // now reverse the coef buffer
  LDR   piCurrCoef_pro, [ppcinfo, #PerChannelInfo_m_rgiCoefRecon]
  
  MOV   t1, piCurrCoef_pro
  
  @ for(i = 0@ i < iSizeCurr >> 2@ i++) {
@  MOV   i_pro, iSizeCurr_pro, ASR #2                            @ i = iSizeCurr/4
	MOV   i_pro, iSizeCurr_pro, ASR #4                            @ i = iSizeCurr/16
  ADD   piPrevCoef_pro, piCurrCoef_pro, iSizeCurr_pro, LSL #2
  SUB   piPrevCoef_pro, piPrevCoef_pro, #32                      @ piPrevCoef = &piCurrCoef[iSizeCurr - 8]
  
LoopMemMove_pro: 
	VLD1.32		{B0, B1, B2, B3}, [piCurrCoef_pro]
	VLD1.32		{A0, AA1, AA2, AA3}, [piPrevCoef_pro]
	SUBS		i_pro, i_pro, #1
	VREV64.32	QB0, QB0
	VREV64.32	QB1, QB1
	VREV64.32	QA0, QA0
	VREV64.32	QA1, QA1
	VSWP.S32	B0, B3
	VSWP.S32	B1, B2
	VSWP.S32	A0, AA3
	VSWP.S32	AA1, AA2
	VST1.32		{B0, B1, B2, B3}, [piPrevCoef_pro]
	SUB			piPrevCoef_pro, piPrevCoef_pro, #32
	VST1.32		{A0, AA1, AA2, AA3}, [piCurrCoef_pro]!	
	@//11 stalls
	BNE		LoopMemMove_pro
	
@  LDRD	cfCurrData_pro, [piCurrCoef_pro]
@  LDRD	cfPrevData_pro, [piPrevCoef_pro]  
@  SUBS  i_pro, i_pro, #1
@  STR	cfCurrData_pro, [piPrevCoef_pro, #4]
@  STR	cfCurrData_pro_next, [piPrevCoef_pro], #-8
@  STR	cfPrevData_pro_next, [piCurrCoef_pro], #4
@  STR	cfPrevData_pro, [piCurrCoef_pro], #4  
@  BNE   LoopMemMove_pro
  
  MOV   i_pro, iSizeCurr_pro, ASR #1                            @ i = iSizeCurr/2
  ANDS	i_pro, i_pro, #7
  BEQ	i_pro_no_odd
  
@ cfCurrData = piCurrCoef[i]@
@ piCurrCoef[i] = piCurrCoef[iSizeCurr - 1 - i]@
@ piCurrCoef[iSizeCurr - 1 - i] = cfCurrData@
	ADD	piPrevCoef_pro, piPrevCoef_pro, #28
OddReverse_pro:
	LDR		cfPrevData_pro, [piPrevCoef_pro]
	LDR		cfCurrData_pro, [piCurrCoef_pro]
	SUBS	i_pro, i_pro, #1
	STR		cfPrevData_pro, [piCurrCoef_pro], #4
	STR		cfCurrData_pro, [piPrevCoef_pro], #-4
	BNE		OddReverse_pro
  
  MOV   piCurrCoef_pro, piPrevCoef_pro
  SUB   piPrevCoef_pro, t1, iSizePrev_pro, LSL #1  
  B		i_pro_odd
  
i_pro_no_odd:  
  ADD	piCurrCoef_pro, piPrevCoef_pro, #28
  SUB   piPrevCoef_pro, t1, iSizePrev_pro, LSL #1  
i_pro_odd:
   
@ if ((pau->m_bUnifiedPureLLMCurrFrm == WMAB_TRUE) && (pau->m_bFirstUnifiedPureLLMFrm == WMAB_FALSE))
@       return WMA_OK@ 
  LDR   t1, [pau, #CAudioObject_m_bUnifiedPureLLMCurrFrm]
  LDR   t2, [pau, #CAudioObject_m_bFirstUnifiedPureLLMFrm]
  CMP   t1, #1
  BNE   ResetBuffer_pro

  CMP   t2, #0
  BEQ   LoopChannelContinue

ResetBuffer_pro:
@ if( iSizeCurr > iSizePrev ){
  CMP   iSizeCurr_pro, iSizePrev_pro
  BLE   ResetBufferLE_pro

@ piCurrCoef -= (iSizeCurr - iSizePrev) >> 1@
  SUB   t1, iSizeCurr_pro, iSizePrev_pro
  SUB   piCurrCoef_pro, piCurrCoef_pro, t1, LSL #1

@ iOverlapSize = iSizePrev >> 1@
  MOV   iOverlapSize_pro, iSizePrev_pro, ASR #1
  B     Overlap_pro

ResetBufferLE_pro:
@ if (iSizeCurr < iSizePrev)
  BEQ   ResetBufferEQ_pro

@ piPrevCoef += ((iSizePrev - iSizeCurr) >> 1)@
  SUB   t1, iSizePrev_pro, iSizeCurr_pro
  ADD   piPrevCoef_pro, piPrevCoef_pro, t1, LSL #1

ResetBufferEQ_pro:
@ iOverlapSize = iSizeCurr >> 1@
  MOV   iOverlapSize_pro, iSizeCurr_pro, ASR #1

Overlap_pro:

  LDRD   bp2Sin_pro, [ppcinfo, #PerChannelInfo_m_fiSinRampUpStart]	@ bp2Sin  = ppcinfo->m_fiSinRampUpStart@
  																	@ bp2Cos  = ppcinfo->m_fiCosRampUpStart@
	LDR   cfPrevData_pro, [piPrevCoef_pro]

  LDR   bp2Step_pro, [ppcinfo, #PerChannelInfo_m_fiSinRampUpStep]	@ bp2Step = ppcinfo->m_fiSinRampUpStep@
	

  LDRD   bp2Sin1_pro, [ppcinfo, #PerChannelInfo_m_fiSinRampUpPrior]	@ bp2Sin1 = ppcinfo->m_fiSinRampUpPrior@
																	@ bp2Cos1 = ppcinfo->m_fiCosRampUpPrior@
	LDR   cfCurrData_pro, [piCurrCoef_pro]
	MOV   cfPrevData_pro, cfPrevData_pro, LSL #2 
  MOV   bp2Step_pro, bp2Step_pro, LSL #2


LOOPOverlap_pro:
@ for(i = 0@ i < iOverlapSize@ i++ ){


@ cfCurrData = *piCurrCoef@
@ *piPrevCoef++ = INT_FROM_COEF( MULT_BP2(-bp2Sin, cfCurrData) + MULT_BP2(bp2Cos, cfPrevData) )@
@ *piCurrCoef-- = INT_FROM_COEF( MULT_BP2(bp2Sin, cfPrevData) + MULT_BP2(bp2Cos, cfCurrData) )@  
	 	
	SMMUL	t2, bp2Cos_pro, cfPrevData_pro  		@ MULT_BP2(bp2Cos, cfPrevData)	
	MOV   cfCurrData_pro, cfCurrData_pro, LSL #2
	LDR   iShift_pro, [sp, #iOffset_coefShiftPro]
	SMMLS	t2, bp2Sin_pro, cfCurrData_pro, t2  

	SMMUL	t3, bp2Cos_pro, cfCurrData_pro  @ MULT_BP2(bp2Cos, cfCurrData)
	SUBS  iOverlapSize_pro, iOverlapSize_pro, #1
	MOV   t2, t2, ASR iShift_pro              @ t2 = INT_FROM_COEF( MULT_BP2(-bp2Sin, cfCurrData) + MULT_BP2(bp2Cos, cfPrevData) )

	SMMLA	t3, bp2Sin_pro, cfPrevData_pro, t3
	STR   t2, [piPrevCoef_pro], #4  	

@ bp2SinT = bp2Sin1 + MULT_BP2(bp2Step,bp2Cos)@
@ bp2CosT = bp2Cos1 - MULT_BP2(bp2Step,bp2Sin)@
@ bp2Sin1 = bp2Sin@  bp2Sin = bp2SinT@
@ bp2Cos1 = bp2Cos@  bp2Cos = bp2CosT@
  
	SMMLA	bp2SinT_pro, bp2Step_pro, bp2Cos_pro, bp2Sin1_pro
	MOV   t3, t3, ASR iShift_pro              @ t3 = INT_FROM_COEF( MULT_BP2(bp2Sin, cfPrevData) + MULT_BP2(bp2Cos, cfCurrData) )
	STR   t3, [piCurrCoef_pro], #-4  

	SMMLS	bp2CosT_pro, bp2Step_pro, bp2Sin_pro, bp2Cos1_pro
@	ADD   bp2SinT_pro, bp2Sin1_pro, t2
	MOV   bp2Sin1_pro, bp2Sin_pro
	MOV   bp2Sin_pro, bp2SinT_pro

	LDR   cfPrevData_pro, [piPrevCoef_pro]	@ cfPrevData = *piPrevCoef@
@  SUB   bp2CosT_pro, bp2Cos1_pro, t3
	MOV   bp2Cos1_pro, bp2Cos_pro
	MOV   bp2Cos_pro, bp2CosT_pro   
	LDR   cfCurrData_pro, [piCurrCoef_pro]	@ cfPrevData = *piPrevCoef@
	MOV   cfPrevData_pro, cfPrevData_pro, LSL #2 
	BNE   LOOPOverlap_pro

LoopChannelContinue:
  LDR   cChInTile_pro, [sp, #iOffset_cChInTilePro]
  LDR   iCh_pro, [sp, #iOffset_iChPro]
  LDR   pau, [sp, #iOffset_pauPro]

  CMP   iCh_pro, cChInTile_pro
  BLT   LoopChannel

auSubframeRecon_ProExit:
  ADD   sp, sp, #iStackSpaceRevPro     @ give back rev stack space  
  LDMFD sp!, {r4 - r11, PC} @auSubframeRecon_MonoPro
  ENTRY_END auSubframeRecon_MonoPro


	.ENDIF	@//WMA_OPT_SUBFRAMERECON_ARM
    .ENDIF	@//IF	ARMVERSION	>=7
	.END