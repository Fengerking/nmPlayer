@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************
@// Module Name:
@//
@//     msaudio_arm.s
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
@//     Jerry He (yamihe) Dec 1, 2003
@//
@// Revision History:
@//
@//     For more information on ARM assembler directives, use
@//        http://msdn.microsoft.com/library/default.asp?url=/library/en-us/wcechp40/html/ccconarmassemblerdirectives.asp
@//*************************************************************************


  .include     "kxarm.h"
  .include     "wma_member_arm.inc"
  .include	  "wma_arm_version.h"
 
  .section .text

  .IF WMA_OPT_AURECON_ARM == 1
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


  .global  auReconSample16_ARM
  .global  auReconSample24_ARM
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Constants for auReconSample16_ARM
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.equ next_PerChannelInfo_m_rgiPCMBuffer	,	PerChannelInfo_size+PerChannelInfo_m_rgiPCMBuffer

@//*************************************************************************************
@//
@// WMARESULT auReconSample16_ARM(CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples)
@//
@//*************************************************************************************

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  @AREA    |.text|, CODE
  LEAF_ENTRY auReconSample16_ARM
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pau
@ r1 = piOutput
@ r2 = cSamples

@ r4 = pau->m_cChannel
@ r5 = pau->m_rgpcinfo


  STMFD sp!, {r4 - r8, lr}

	CMP		r2, #0
	BEQ		EndLoop
	
  LDRH  r4, [r0, #CAudioObject_m_cChannel]
  LDR   r5, [r0, #CAudioObject_m_rgpcinfo]

@ lr  = 0x00007FFF
@ r12 = 0xFFFF8000

@ r3 = -1
  MVN   r3, #0

@ lr = 0x00007FFF
  MOV   lr, r3, LSR #17

@ r12 = 0xFFFF8000
  MOV   r12,r3, LSL #15
	
@ r3 = i
  MOV   r3, #0

	CMP		r4, #2
	BLT		LoopOneChannel
	BGT		LoopMultiChannel
	
LoopTwoChannel:	
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer@
	LDR		r7, [r5, #next_PerChannelInfo_m_rgiPCMBuffer]
		
	LDR		r6, [r0, r3, LSL #2]								@ iPCMData = (PCMSAMPLE)pcfPCMBuf[i]@
	LDR		r8, [r7, r3, LSL #2]	
	ADD		r3, r3, #1
	CMP		r6, lr												@ if (iPCMData > (I32)0X00007FFF)
	MOVGT	r6, lr												@    iPCMData = (I16)0X7FFF@
	CMPLE	r6, r12												@ else if (iPCMData < (I32)0XFFFF8000)
	MOVLT	r6, r12												@    iPCMData = (I16)0X8000@
	CMP		r8, lr												@ if (iPCMData > (I32)0X00007FFF)
	MOVGT	r8, lr												@    iPCMData = (I16)0X7FFF@
	CMPLE	r8, r12												@ else if (iPCMData < (I32)0XFFFF8000)
	MOVLT	r8, r12												@    iPCMData = (I16)0X8000@
	STRH	r6, [r1], #2
	STRH	r8, [r1], #2										@armv4	

	CMP		r3, r2
	BLT		LoopTwoChannel
	BGE		EndLoop
	
LoopOneChannel:
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer@
	
	
	LDR		r6, [r0, r3, LSL #2]								@ iPCMData = (PCMSAMPLE)pcfPCMBuf[i]@
	ADD		r3, r3, #1
	
	CMP		r6, lr												@ if (iPCMData > (I32)0X00007FFF)
	MOVGT	r6, lr												@    iPCMData = (I16)0X7FFF@
	CMPLE	r6, r12												@ else if (iPCMData < (I32)0XFFFF8000)
	MOVLT	r6, r12												@    iPCMData = (I16)0X8000@	
	STRH	r6, [r1], #2
	CMP		r3, r2
	BLT		LoopOneChannel
	BGE		EndLoop
	
LoopMultiChannel:
	MOV		r7, r4
	MOV		r8, r5
gInLoop16:
	LDR		r0, [r8, #PerChannelInfo_m_rgiPCMBuffer]
	ADD		r8, r8, #PerChannelInfo_size
	LDR		r6, [r0, r3, LSL #2]
	CMP		r6, lr
	MOVGT	r6, lr
	CMPLE	r6, r12
	MOVLT	r6, r12
	STRH	r6, [r1], #2
	SUBS	r7, r7, #1
	BNE		gInLoop16
	ADD		r3, r3, #1
	CMP		r3, r2
	BLT		LoopMultiChannel
	
EndLoop:
	MOV r0, #0
	LDMFD sp!, {r4 - r8, PC} @auReconSample16_ARM
	
	ENTRY_END auReconSample16_ARM


@//*************************************************************************************
@//
@// WMARESULT auReconSample24_ARM(CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples)
@//
@//*************************************************************************************

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  @AREA    |.text|, CODE
  LEAF_ENTRY auReconSample24_ARM
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	.IF		DEVICEPCMOUT16	==	1
	
@ Input parameters
@ r0 = pau
@ r1 = piOutput
@ r2 = cSamples

@ r4 = pau->m_cChannel
@ r5 = pau->m_rgpcinfo


  STMFD sp!, {r4 - r8, lr}
	CMP		r2, #0
	BEQ		EndLoop24
  LDRH  r4, [r0, #CAudioObject_m_cChannel]
  LDR   r5, [r0, #CAudioObject_m_rgpcinfo]

@ lr  = 0X007FFFFF
@ r12 = 0XFF800000

@ r3 = -1
  MVN   r3, #0

@ lr = 0X007FFFFF
  MOV   lr, r3, LSR #9

@ r12 = 0XFF800000
  MOV   r12,r3, LSL #23

@ r3 = i
  MOV   r3, #0
  
	CMP		r4, #2
	BLT		LoopOneChannel24
	BGT		LoopMultiChannel24

LoopTwoChannel24:	
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer@
	LDR		r7, [r5, #next_PerChannelInfo_m_rgiPCMBuffer]
	
	LDR		r6, [r0, r3, LSL #2]								@ iPCMData = (PCMSAMPLE)pcfPCMBuf[i]@
	LDR		r8, [r7, r3, LSL #2]	
	ADD		r3, r3, #1
	CMP		r6, lr												@ if (iPCMData > (I32)0X007FFFFF)
	MOVGT	r6, lr												@    iPCMData = (I16)0X7FFFFF@
	CMPLE	r6, r12												@ else if (iPCMData < (I32)0XFF800000)
	MOVLT	r6, r12												@    iPCMData = (I16)0X800000@
	MOV		r6, r6, ASR #8
	CMP		r8, lr												@ if (iPCMData > (I32)0X007FFFFF)
	MOVGT	r8, lr												@    iPCMData = (I16)0X7FFFFF@
	CMPLE	r8, r12												@ else if (iPCMData < (I32)0XFF800000)
	MOVLT	r8, r12												@    iPCMData = (I16)0X800000@
	MOV		r8, r8, ASR #8
	STRH	r6, [r1], #2
	STRH	r8, [r1], #2										@armv4	
	CMP		r3, r2
	BLT		LoopTwoChannel24
	BGE		EndLoop24
	
LoopOneChannel24:
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer@	
	
	LDR		r6, [r0, r3, LSL #2]								@ iPCMData = (PCMSAMPLE)pcfPCMBuf[i]@
	ADD		r3, r3, #1	
	CMP		r6, lr												@ if (iPCMData > (I32)0X007FFFFF)
	MOVGT	r6, lr												@    iPCMData = (I16)0X7FFFFF@
	CMPLE	r6, r12												@ else if (iPCMData < (I32)0XFF800000)
	MOVLT	r6, r12												@    iPCMData = (I16)0X800000@	
	MOV		r6, r6, ASR #8
	STRH	r6, [r1], #2
	CMP		r3, r2
	BLT		LoopOneChannel24
	BGE		EndLoop24
	
LoopMultiChannel24:
	MOV		r7, r4
	MOV		r8, r5
gInLoop24:
	LDR		r0, [r8, #PerChannelInfo_m_rgiPCMBuffer]
	ADD		r8, r8, #PerChannelInfo_size
	LDR		r6, [r0, r3, LSL #2]
	CMP		r6, lr
	MOVGT	r6, lr
	CMPLE	r6, r12
	MOVLT	r6, r12
	MOV		r6, r6, ASR #8
	STRH	r6, [r1], #2
	SUBS	r7, r7, #1
	BNE		gInLoop24
	ADD		r3, r3, #1
	CMP		r3, r2
	BLT		LoopMultiChannel24
	
EndLoop24:
	MOV r0, #0
	LDMFD sp!, {r4 - r8, PC} @auReconSample24_ARM
	
	ENTRY_END auReconSample24_ARM
	
	.ELSE	@DEVICEPCMOUT16
	
@ Input parameters
@ r0 = pau
@ r1 = piOutput
@ r2 = cSamples

@ r4 = pau->m_cChannel
@ r5 = pau->m_rgpcinfo


  STMFD sp!, {r4 - r8, lr}
  SUB   sp, sp, #4  @reserve address for iPCMData

  LDRH  r4, [r0, #CAudioObject_m_cChannel]
  LDR   r5, [r0, #CAudioObject_m_rgpcinfo]

@ lr  = 0X007FFFFF
@ r12 = 0XFF800000

@ r3 = -1
  MVN   r3, #0

@ lr = 0X007FFFFF
  MOV   lr, r3, LSR #9

@ r12 = 0XFF800000
  MOV   r12,r3, LSL #23

@ r3 = i
  MOV   r3, #0

gOutLoop24:
@ for( i = 0@ i < cSamples@ i++ )

@ r7 = loop counter
  MOV   r7, r4

@ r6 = iPCMData
@ r8 = ppcinfo
  MOV   r8, r5

gInLoop24:
@ for( iCh = 0@ iCh < pau->m_cChannel@ iCh++ )

@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer@
  LDR  r0, [r8, #PerChannelInfo_m_rgiPCMBuffer]

@ PerChannelInfo* ppcinfo = pau->m_rgpcinfo + iCh@
  ADD  r8, r8, #PerChannelInfo_size
  
@ iPCMData = (PCMSAMPLE)pcfPCMBuf[i]@
  LDR  r6, [r0, r3, LSL #2]

@ if (iPCMData > (I32)0X007FFFFF)
@    iPCMData = 0X007FFFFF@
  CMP   r6, lr
  STR   r6, [sp]
  STRGT lr, [sp]

@ else if (iPCMData < (I32)0XFF800000)
@    iPCMData = 0XFF800000@    
  CMPLE r6, r12
  STRLT r12, [sp]
  

@ *(((I8*)piOutput)++) = *(((I8*)(&iPCMData)))@
  LDRB  r0, [sp]
  STRB  r0, [r1], #1

@ *(((I8*)piOutput)++) = *(((I8*)(&iPCMData)+1))@
  LDRB  r0, [sp, #1]
  STRB  r0, [r1], #1

@ *(((I8*)piOutput)++) = *(((I8*)(&iPCMData)+2))@
  LDRB  r0, [sp, #2]
  STRB  r0, [r1], #1

  SUBS  r7, r7, #1
  BNE   gInLoop24

  ADD   r3, r3, #1

  CMP   r3, r2
  BLT   gOutLoop24

  MOV   r0, #0

  ADD   sp, sp, #4  @release address for iPCMData
  LDMFD sp!, {r4 - r8, PC} @auReconSample24_ARM
  ENTRY_END auReconSample24_ARM

	.ENDIF	@DEVICEPCMOUT16
  .ENDIF @ WMA_OPT_AURECON_ARM
    
  .END