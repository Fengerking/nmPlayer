@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************
@// Module Name:
@//
@//     msaudiostdpro_arm.s
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
@//     Jerry He (yamihe) Jan 16, 2004
@//
@// Revision History:
@//
@//     For more information on ARM assembler directives, use
@//        http://msdn.microsoft.com/library/default.asp?url=/library/en-us/wcechp40/html/ccconarmassemblerdirectives.asp
@//*************************************************************************


  @OPT         2       @ disable listing 
  #include "../../../inc/audio/v10/include/voWMADecID.h"
  .include     "kxarm.h"
  .include     "wma_member_arm.inc"
@  INCLUDE	  wma_arm_version.h
  @OPT         1       @ enable listing
 
  @AREA    |.text|, CODE, READONLY
	.text   .align 4
  .if WMA_OPT_SUBFRAMERECON_ARM == 1
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


  .globl  _auSubframeRecon_Std
  .globl  _auSubframeRecon_MonoPro


@//*************************************************************************************
@//
@// WMARESULT ausubframeRecon_Std ( CAudioObject* pau, PerChannelInfo* ppcinfo, PerChannelInfo* ppcinfo2)
@//
@//*************************************************************************************

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Registers for ausubframeRecon_Std
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

piCurrCoef        .req  r4
piPrevCoef        .req  r5

piCurrCoef2       .req  r12
piPrevCoef2       .req  r14

i                 .req  r3
iOverlapSize      .req  r6
iShift            .req  r3
bp2Step           .req  r7

iSizeCurr         .req  r8
iSizePrev         .req  r9

temp1             .req  r6
temp2             .req  r7
temp3             .req  r1

bp2Sin            .req  r8
bp2Cos            .req  r9
bp2Sin1           .req  r2
bp2Cos1           .req  r0

bp2SinT           .req  r10
bp2CosT           .req  r11

cfPrevData        .req  r10
cfCurrData        .req  r11

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Constants for ausubframeRecon_Std
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
.set iStackSpaceRevStd		, 4*4    
.set iOffset_coefShift		, iStackSpaceRevStd-4
.set iOffset_bp2Step			, iStackSpaceRevStd-8
.set iOffset_iOverlapSize	, iStackSpaceRevStd-12
.set iOffset_iSizeCurr		, iStackSpaceRevStd-16
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  @AREA    |.text|, CODE
_auSubframeRecon_Std:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pau
@ r1 = ppcinfo
@ r2 = ppcinfo2


  stmfd sp!, {r4 - r11, lr}
  sub   sp, sp, #iStackSpaceRevStd @ rev stack space

@ iSizeCurr = ppcinfo->m_iSizeCurr;
  ldrsh iSizeCurr, [r1, #PerChannelInfo_m_iSizeCurr]

@ piCurrCoef = (CoefType *)ppcinfo->m_rgiCoefRecon;   // now reverse the coef buffer
  ldr   piCurrCoef, [r1, #PerChannelInfo_m_rgiCoefRecon]
  
@ piCurrCoef2 = (CoefType *)ppcinfo2->m_rgiCoefRecon;   // now reverse the coef buffer
  ldr   piCurrCoef2, [r2, #PerChannelInfo_m_rgiCoefRecon]

  mov   temp1, piCurrCoef
  mov   temp2, piCurrCoef2

  str	iSizeCurr, [sp, #iOffset_iSizeCurr]
  
@ for(i = 0; i < iSizeCurr >> 1; i++) {
  mov   i, iSizeCurr, asr #2                        @ i = iSizeCurr/4
  add   piPrevCoef, piCurrCoef, iSizeCurr, lsl #2
  add   piPrevCoef2, piCurrCoef2, iSizeCurr, lsl #2
  sub   piPrevCoef, piPrevCoef, #8                 @ piPrevCoef = &piCurrCoef[iSizeCurr - 2]
  sub   piPrevCoef2, piPrevCoef2, #8               @ piPrevCoef2 = &piCurrCoef2[iSizeCurr - 2]

LoopMemmove:
@ cfCurrData = piCurrCoef[i]@
@ piCurrCoef[i] = piCurrCoef[iSizeCurr - 1 - i]@
@ piCurrCoef[iSizeCurr - 1 - i] = cfCurrData;

@ cfCurrData = piCurrCoef2[i]@
@ piCurrCoef2[i] = piCurrCoef2[iSizeCurr - 1 - i]@
@ piCurrCoef2[iSizeCurr - 1 - i] = cfCurrData;

	ldrd	r8, [piPrevCoef]
	ldrd	r10, [piCurrCoef]
	subs	i, i, #1
	str		r9, [piCurrCoef], #4
	str		r8, [piCurrCoef], #4
	ldrd	r8, [piPrevCoef2]
	str		r10, [piPrevCoef, #4]
	str		r11, [piPrevCoef], #-8
	ldrd	r10, [piCurrCoef2]
	str		r9, [piCurrCoef2], #4
	str		r8, [piCurrCoef2], #4
	str		r10, [piPrevCoef2, #4]
	str		r11, [piPrevCoef2], #-8
  
	bne   LoopMemmove

@ // ---- Setup the coef buffer pointer ----
@ piPrevCoef = (CoefType *)ppcinfo->m_rgiCoefRecon - iSizePrev / 2;       // go forward
@ piCurrCoef = (CoefType *)ppcinfo->m_rgiCoefRecon + iSizeCurr / 2 - 1;   // go backward
  
  ldrsh iSizePrev, [r1, #PerChannelInfo_m_iSizePrev]
  ldr	iSizeCurr, [sp, #iOffset_iSizeCurr]
  
  
  mov   i, iSizeCurr, asr #1                        @ i = iSizeCurr/2	
  ands	i, i, #1
  beq	i_is_even
@ the last odd reverse
  add	piPrevCoef, piPrevCoef, #4
  ldr   cfPrevData, [piPrevCoef]
  ldr   cfCurrData, [piCurrCoef]
  str   cfPrevData, [piCurrCoef], #4
  str   cfCurrData, [piPrevCoef], #-4

@ cfCurrData = piCurrCoef2[i]@
@ piCurrCoef2[i] = piCurrCoef2[iSizeCurr - 1 - i]@
@ piCurrCoef2[iSizeCurr - 1 - i] = cfCurrData;
  add	piPrevCoef2, piPrevCoef2, #4
  ldr   cfPrevData, [piPrevCoef2]
  ldr   cfCurrData, [piCurrCoef2]
  str   cfPrevData, [piCurrCoef2], #4
  str   cfCurrData, [piPrevCoef2], #-4
  
  mov   piCurrCoef, piPrevCoef
  sub   piPrevCoef, temp1, iSizePrev, lsl #1    
  mov   piCurrCoef2, piPrevCoef2  
  bne	i_is_odd
i_is_even:   
@ piPrevCoef2 = (CoefType *)ppcinfo2->m_rgiCoefRecon - iSizePrev / 2;       // go forward
@ piCurrCoef2 = (CoefType *)ppcinfo2->m_rgiCoefRecon + iSizeCurr / 2 - 1;   // go backward
  add   piCurrCoef, piPrevCoef, #4
  sub   piPrevCoef, temp1, iSizePrev, lsl #1
  add   piCurrCoef2, piPrevCoef2, #4  
  
i_is_odd:
@ piCurrCoef2 = (CoefType *)ppcinfo2->m_rgiCoefRecon + iSizeCurr / 2 - 1;   // go backward
  sub   piPrevCoef2, temp2, iSizePrev, lsl #1
  
@ if ((pau->m_bUnifiedPureLLMCurrFrm == WMAb_TRUE) && (pau->m_bFirstUnifiedPureLLMFrm == WMAb_FALSE))
@       return WMA_OK; 
  ldr   temp1, [r0, #CAudioObject_m_bUnifiedPureLLMCurrFrm]
  ldr   temp2, [r0, #CAudioObject_m_bFirstUnifiedPureLLMFrm]
  cmp   temp1, #1
  bne   Resetbuffer

  cmp   temp2, #0
  beq   ausubframeRecon_StdExit

Resetbuffer:
@ if( iSizeCurr > iSizePrev ){
  cmp   iSizeCurr, iSizePrev
  ble   ResetbufferLE

@ piCurrCoef -= (iSizeCurr - iSizePrev) >> 1;
@ piCurrCoef2 -= (iSizeCurr - iSizePrev) >> 1;
  sub   temp1, iSizeCurr, iSizePrev
  sub   piCurrCoef, piCurrCoef, temp1, lsl #1
  sub   piCurrCoef2, piCurrCoef2, temp1, lsl #1

@ iOverlapSize = iSizePrev >> 1;
  mov   iOverlapSize, iSizePrev, asr #1
  b     Overlap

ResetbufferLE:
@ if (iSizeCurr < iSizePrev)
  beq   ResetbufferEQ

@ piPrevCoef += ((iSizePrev - iSizeCurr) >> 1)@
@ piPrevCoef2 += ((iSizePrev - iSizeCurr) >> 1)@
  sub   temp1, iSizePrev, iSizeCurr
  add   piPrevCoef, piPrevCoef, temp1, lsl #1
  add   piPrevCoef2, piPrevCoef2, temp1, lsl #1

ResetbufferEQ:
@ iOverlapSize = iSizeCurr >> 1;
  mov   iOverlapSize, iSizeCurr, asr #1

Overlap:
	ldr   temp2, [r0, #CAudioObject_m_cLeftShiftBitsFixedPost] @ coefShift = pau->m_cLeftShiftbitsFixedPost

	ldrd   bp2Sin, [r1, #PerChannelInfo_m_fiSinRampUpStart]	@ bp2Sin  = ppcinfo->m_fiSinRampUpStart;
  															@ bp2Cos  = ppcinfo->m_fiCosRampUpStart;
	str   temp2, [sp, #iOffset_coefShift]

@ bp2Step = ppcinfo->m_fiSinRampUpStep;
	ldr   bp2Step,  [r1, #PerChannelInfo_m_fiSinRampUpStep]

@ bp2Sin1 = ppcinfo->m_fiSinRampUpPrior;
	ldr   bp2Sin1, [r1, #PerChannelInfo_m_fiSinRampUpPrior]
	ldr   cfPrevData, [piPrevCoef]
	mov   bp2Step, bp2Step, lsl #2
	str   bp2Step, [sp, #iOffset_bp2Step]
	ldr   cfCurrData, [piCurrCoef]				@ cfCurrData = *piCurrCoef;
@ bp2Cos1 = ppcinfo->m_fiCosRampUpPrior;
	ldr   bp2Cos1, [r1, #PerChannelInfo_m_fiCosRampUpPrior]
	mov   cfPrevData, cfPrevData, lsl #2
LOOPOverlap:
@ for(i = 0; i < iOverlapSize; i++ ){

@ cfPrevData = *piPrevCoef;
@ cfCurrData = *piCurrCoef;
@ *piPrevCoef++ = INT_FROM_COEF( MULT_bP2(-bp2Sin, cfCurrData) + MULT_bP2(bp2Cos, cfPrevData) )@
@ *piCurrCoef-- = INT_FROM_COEF( MULT_bP2(bp2Sin, cfPrevData) + MULT_bP2(bp2Cos, cfCurrData) )@
	smmul temp2, bp2Cos, cfPrevData				@ MULT_bP2(bp2Cos, cfPrevData)
	mov   cfCurrData, cfCurrData, lsl #2

	ldr   iShift, [sp, #iOffset_coefShift]
	smmls temp2, bp2Sin, cfCurrData, temp2 

	smmul temp3, bp2Cos, cfCurrData    @ MULT_bP2(bp2Cos, cfCurrData)
	mov   temp2, temp2, asr iShift            @ temp2 = INT_FROM_COEF( MULT_bP2(-bp2Sin, cfCurrData) + MULT_bP2(bp2Cos, cfPrevData) )

	smmla temp3, bp2Sin, cfPrevData, temp3
	ldr   cfPrevData, [piPrevCoef2]
	ldr   cfCurrData, [piCurrCoef2]           @ cfCurrData = *piCurrCoef2;	
	str   temp2, [piPrevCoef], #4  
  
	mov   cfPrevData, cfPrevData, lsl #2
	mov   cfCurrData, cfCurrData, lsl #2
@ cfPrevData = *piPrevCoef2;
@ cfCurrData = *piCurrCoef2;
    
@ *piPrevCoef2++ = INT_FROM_COEF( MULT_bP2(-bp2Sin, cfCurrData) + MULT_bP2(bp2Cos, cfPrevData) )@
@ *piCurrCoef2-- = INT_FROM_COEF( MULT_bP2(bp2Sin, cfPrevData) + MULT_bP2(bp2Cos, cfCurrData) )@

	smmul temp2, bp2Cos, cfPrevData    @ MULT_bP2(bp2Cos, cfPrevData)
  
	mov   temp3, temp3, asr iShift            @ temp3 = INT_FROM_COEF( MULT_bP2(bp2Sin, cfPrevData) + MULT_bP2(bp2Cos, cfCurrData) )@

	str   temp3, [piCurrCoef], #-4

	smmls temp2, bp2Sin, cfCurrData, temp2
  
	smmul temp3, bp2Cos, cfCurrData    @ MULT_bP2(bp2Cos, cfCurrData)
	mov   temp2, temp2, asr iShift            @ temp2 = INT_FROM_COEF( MULT_bP2(-bp2Sin, cfCurrData) + MULT_bP2(bp2Cos, cfPrevData) )


	smmla temp3, bp2Sin, cfPrevData, temp3
  
	str   temp2, [piPrevCoef2], #4  
	ldr   bp2Step, [sp, #iOffset_bp2Step]
@ bp2SinT = bp2Sin1 + MULT_bP2(bp2Step,bp2Cos)@
@ bp2CosT = bp2Cos1 - MULT_bP2(bp2Step,bp2Sin)@
@ bp2Sin1 = bp2Sin;  bp2Sin = bp2SinT;
@ bp2Cos1 = bp2Cos;  bp2Cos = bp2CosT;
	mov   temp3, temp3, asr iShift            @ temp3 = INT_FROM_COEF( MULT_bP2(bp2Sin, cfPrevData) + MULT_bP2(bp2Cos, cfCurrData) )
	str   temp3, [piCurrCoef2], #-4  
  
	smmla bp2SinT, bp2Step, bp2Cos, bp2Sin1
	subs  iOverlapSize, iOverlapSize, #1
	smmlsr bp2CosT, bp2Step, bp2Sin, bp2Cos1
	mov   bp2Sin1, bp2Sin
	mov   bp2Sin, bp2SinT

	ldr   cfPrevData, [piPrevCoef]
	mov   bp2Cos1, bp2Cos
	mov   bp2Cos, bp2CosT   
	ldr   cfCurrData, [piCurrCoef]				@ cfCurrData = *piCurrCoef;
	mov   cfPrevData, cfPrevData, lsl #2
	bne   LOOPOverlap

ausubframeRecon_StdExit:
  add   sp, sp, #iStackSpaceRevStd     @ give back rev stack space  
  ldmfd sp!, {r4 - r11, PC} @ ausubframeRecon_Std
  @ENTRY_END ausubframeRecon_Std



@//*************************************************************************************
@//
@// WMARESULT WMARESULT ausubframeRecon_MonoPro ( CAudioObject* pau )
@//
@//*************************************************************************************

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Registers for ausubframeRecon_MonoPro
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
@   Constants for ausubframeRecon_MonoPro
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
.set iStackSpaceRevPro        , 4*4    
.set iOffset_coefShiftPro     , iStackSpaceRevPro-4
.set iOffset_cChInTilePro     , iStackSpaceRevPro-8
.set iOffset_iChPro           , iStackSpaceRevPro-12
.set iOffset_pauPro           , iStackSpaceRevPro-16

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  @AREA    |.text|, CODE
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
  ldr   t2, [pau, #CAudioObject_m_cLeftShiftBitsFixedPost] @ coefShift = pau->m_cLeftShiftbitsFixedPost

  str   cChInTile_pro, [sp, #iOffset_cChInTilePro]
  str   t2, [sp, #iOffset_coefShiftPro]
  str   pau, [sp, #iOffset_pauPro]


@ for (iCh = 0; iCh < pau->m_cChInTile; iCh++) {
LoopChannel:

@ iChSrc = pau->m_rgiChInTile [iCh]@
  ldr   t1, [pau, #CAudioObject_m_rgiChInTile]
  add   t1, t1, iCh_pro, lsl #1
  ldrsh iChSrc, [t1]

@ ppcinfo = pau->m_rgpcinfo + iChSrc;
  ldr   ppcinfo, [pau, #CAudioObject_m_rgpcinfo]

  mov   t1, #PerChannelInfo_size
  mla   ppcinfo, t1, iChSrc, ppcinfo

  add   iCh_pro, iCh_pro, #1
  str   iCh_pro, [sp, #iOffset_iChPro]

@ iSizePrev = ppcinfo->m_iSizePrev;
  ldrsh iSizePrev_pro, [ppcinfo, #PerChannelInfo_m_iSizePrev]

@ iSizeCurr = ppcinfo->m_iSizeCurr;
  ldrsh iSizeCurr_pro, [ppcinfo, #PerChannelInfo_m_iSizeCurr]

@ piCurrCoef = (CoefType *)ppcinfo->m_rgiCoefRecon;   // now reverse the coef buffer
  ldr   piCurrCoef_pro, [ppcinfo, #PerChannelInfo_m_rgiCoefRecon]
  
  mov   t1, piCurrCoef_pro
  
  @ for(i = 0; i < iSizeCurr >> 2; i++) {
  mov   i_pro, iSizeCurr_pro, asr #2                            @ i = iSizeCurr/4
  add   piPrevCoef_pro, piCurrCoef_pro, iSizeCurr_pro, lsl #2
  sub   piPrevCoef_pro, piPrevCoef_pro, #8                      @ piPrevCoef = &piCurrCoef[iSizeCurr - 2]
  
LoopMemmove_pro:
  ldrd	cfCurrData_pro, [piCurrCoef_pro]
  ldrd	cfPrevData_pro, [piPrevCoef_pro]  
  subs  i_pro, i_pro, #1
  str	cfCurrData_pro, [piPrevCoef_pro, #4]
  str	cfCurrData_pro_next, [piPrevCoef_pro], #-8
  str	cfPrevData_pro_next, [piCurrCoef_pro], #4
  str	cfPrevData_pro, [piCurrCoef_pro], #4  
  bne   LoopMemmove_pro
  
  mov   i_pro, iSizeCurr_pro, asr #1                            @ i = iSizeCurr/2
  ands	i_pro, i_pro, #1
  beq	i_pro_is_even
  
  @ cfCurrData = piCurrCoef[i]@
@ piCurrCoef[i] = piCurrCoef[iSizeCurr - 1 - i]@
@ piCurrCoef[iSizeCurr - 1 - i] = cfCurrData;
  add	piPrevCoef_pro, piPrevCoef_pro, #4
  ldr   cfPrevData_pro, [piPrevCoef_pro]
  ldr   cfCurrData_pro, [piCurrCoef_pro]
  str   cfPrevData_pro, [piCurrCoef_pro], #4
  str   cfCurrData_pro, [piPrevCoef_pro], #-4
  
  mov   piCurrCoef_pro, piPrevCoef_pro
  sub   piPrevCoef_pro, t1, iSizePrev_pro, lsl #1  
  b		i_pro_is_odd
  
i_pro_is_even: 
  add	piCurrCoef_pro, piPrevCoef_pro, #4
  sub   piPrevCoef_pro, t1, iSizePrev_pro, lsl #1  
i_pro_is_odd:
   
@ if ((pau->m_bUnifiedPureLLMCurrFrm == WMAb_TRUE) && (pau->m_bFirstUnifiedPureLLMFrm == WMAb_FALSE))
@       return WMA_OK; 
  ldr   t1, [pau, #CAudioObject_m_bUnifiedPureLLMCurrFrm]
  ldr   t2, [pau, #CAudioObject_m_bFirstUnifiedPureLLMFrm]
  cmp   t1, #1
  bne   Resetbuffer_pro

  cmp   t2, #0
  beq   LoopChannelContinue

Resetbuffer_pro:
@ if( iSizeCurr > iSizePrev ){
  cmp   iSizeCurr_pro, iSizePrev_pro
  ble   ResetbufferLE_pro

@ piCurrCoef -= (iSizeCurr - iSizePrev) >> 1;
  sub   t1, iSizeCurr_pro, iSizePrev_pro
  sub   piCurrCoef_pro, piCurrCoef_pro, t1, lsl #1

@ iOverlapSize = iSizePrev >> 1;
  mov   iOverlapSize_pro, iSizePrev_pro, asr #1
  b     Overlap_pro

ResetbufferLE_pro:
@ if (iSizeCurr < iSizePrev)
  beq   ResetbufferEQ_pro

@ piPrevCoef += ((iSizePrev - iSizeCurr) >> 1)@
  sub   t1, iSizePrev_pro, iSizeCurr_pro
  add   piPrevCoef_pro, piPrevCoef_pro, t1, lsl #1

ResetbufferEQ_pro:
@ iOverlapSize = iSizeCurr >> 1;
  mov   iOverlapSize_pro, iSizeCurr_pro, asr #1

Overlap_pro:

  ldrd   bp2Sin_pro, [ppcinfo, #PerChannelInfo_m_fiSinRampUpStart]	@ bp2Sin  = ppcinfo->m_fiSinRampUpStart;
  																	@ bp2Cos  = ppcinfo->m_fiCosRampUpStart;
	ldr   cfPrevData_pro, [piPrevCoef_pro]

  ldr   bp2Step_pro, [ppcinfo, #PerChannelInfo_m_fiSinRampUpStep]	@ bp2Step = ppcinfo->m_fiSinRampUpStep;
	

  ldrd   bp2Sin1_pro, [ppcinfo, #PerChannelInfo_m_fiSinRampUpPrior]	@ bp2Sin1 = ppcinfo->m_fiSinRampUpPrior;
																	@ bp2Cos1 = ppcinfo->m_fiCosRampUpPrior;
	ldr   cfCurrData_pro, [piCurrCoef_pro]
	mov   cfPrevData_pro, cfPrevData_pro, lsl #2 
  mov   bp2Step_pro, bp2Step_pro, lsl #2


LOOPOverlap_pro:
@ for(i = 0; i < iOverlapSize; i++ ){


@ cfCurrData = *piCurrCoef;
@ *piPrevCoef++ = INT_FROM_COEF( MULT_bP2(-bp2Sin, cfCurrData) + MULT_bP2(bp2Cos, cfPrevData) )@
@ *piCurrCoef-- = INT_FROM_COEF( MULT_bP2(bp2Sin, cfPrevData) + MULT_bP2(bp2Cos, cfCurrData) )@  
	 	
	smmul	t2, bp2Cos_pro, cfPrevData_pro  		@ MULT_bP2(bp2Cos, cfPrevData)	
	mov   cfCurrData_pro, cfCurrData_pro, lsl #2
	ldr   iShift_pro, [sp, #iOffset_coefShiftPro]
	smmls	t2, bp2Sin_pro, cfCurrData_pro, t2  

	smmul	t3, bp2Cos_pro, cfCurrData_pro  @ MULT_bP2(bp2Cos, cfCurrData)
	subs  iOverlapSize_pro, iOverlapSize_pro, #1
	mov   t2, t2, asr iShift_pro              @ t2 = INT_FROM_COEF( MULT_bP2(-bp2Sin, cfCurrData) + MULT_bP2(bp2Cos, cfPrevData) )

	smmla	t3, bp2Sin_pro, cfPrevData_pro, t3
	str   t2, [piPrevCoef_pro], #4  	

@ bp2SinT = bp2Sin1 + MULT_bP2(bp2Step,bp2Cos)@
@ bp2CosT = bp2Cos1 - MULT_bP2(bp2Step,bp2Sin)@
@ bp2Sin1 = bp2Sin;  bp2Sin = bp2SinT;
@ bp2Cos1 = bp2Cos;  bp2Cos = bp2CosT;
  
	smmla	bp2SinT_pro, bp2Step_pro, bp2Cos_pro, bp2Sin1_pro
	mov   t3, t3, asr iShift_pro              @ t3 = INT_FROM_COEF( MULT_bP2(bp2Sin, cfPrevData) + MULT_bP2(bp2Cos, cfCurrData) )
	str   t3, [piCurrCoef_pro], #-4  

	smmls	bp2CosT_pro, bp2Step_pro, bp2Sin_pro, bp2Cos1_pro
@	add   bp2SinT_pro, bp2Sin1_pro, t2
	mov   bp2Sin1_pro, bp2Sin_pro
	mov   bp2Sin_pro, bp2SinT_pro

	ldr   cfPrevData_pro, [piPrevCoef_pro]	@ cfPrevData = *piPrevCoef;
@  sub   bp2CosT_pro, bp2Cos1_pro, t3
	mov   bp2Cos1_pro, bp2Cos_pro
	mov   bp2Cos_pro, bp2CosT_pro   
	ldr   cfCurrData_pro, [piCurrCoef_pro]	@ cfPrevData = *piPrevCoef;
	mov   cfPrevData_pro, cfPrevData_pro, lsl #2 
	bne   LOOPOverlap_pro

LoopChannelContinue:
  ldr   cChInTile_pro, [sp, #iOffset_cChInTilePro]
  ldr   iCh_pro, [sp, #iOffset_iChPro]
  ldr   pau, [sp, #iOffset_pauPro]

  cmp   iCh_pro, cChInTile_pro
  blt   LoopChannel

ausubframeRecon_ProExit:
  add   sp, sp, #iStackSpaceRevPro     @ give back rev stack space  
  ldmfd sp!, {r4 - r11, PC} @ausubframeRecon_MonoPro
  @ENTRY_END ausubframeRecon_MonoPro


  .endif @ WMA_OPT_subFRAMERECON_ARM
    
  @.end
  
  