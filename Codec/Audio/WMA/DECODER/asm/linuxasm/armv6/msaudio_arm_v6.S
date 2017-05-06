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
@//     Witten Wen (Shanghai)  2008
@//
@// Revision History:
@//
@//     For more information on ARM assembler directives, use
@//        http://msdn.microsoft.com/library/default.asp?url=/library/en-us/wcechp40/html/ccconarmassemblerdirectives.asp
@//*************************************************************************

  #include "../../../../../inc/audio/v10/include/voWMADecID.h"
  .include     "kxarm.h"
  .include     "wma_member_arm.inc"
  .include	  "wma_arm_version.h"
 
  @AREA    |.text|, CODE, READONLY
  .section .text

  .if WMA_OPT_AURECON_ARM == 1
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
@	.extern		printf

  .global  auReconSample16_ARM
  .global  auReconSample24_ARM
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Constants for auReconSample16_ARM
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.equ next_PerChannelInfo_m_rgiPCMBuffer	,	PerChannelInfo_size+PerChannelInfo_m_rgiPCMBuffer
.equ double_PerChannelInfo_size			, 2*PerChannelInfo_size

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

	CMP		r4, #2
	BLT		OneChannel
	BGT		MultiChannel
	
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer@
	LDR		r12, [r5, #next_PerChannelInfo_m_rgiPCMBuffer]
	MOV		lr, r2, lsr #1										@lr = cSamples/2
LoopTwoChannel:	
	LDRD	r4, [r0], #8										@ iPCMData = (PCMSAMPLE)pcfPCMBuf[i]@
	LDRD	r6, [r12], #8
	SUBS	lr, lr, #1
	SSAT	r4, #16, r4
	SSAT	r6, #16, r6
	SSAT	r5, #16, r5	
	SSAT	r7, #16, r7	
	PKHBT	r4,	r4,	r6, LSL #16									@ armv6
	PKHBT	r5,	r5,	r7, LSL #16
	STRD	r4, [r1], #8	
	BNE		LoopTwoChannel
	ANDS	r3, r2, #1
	BEQ		EndLoop
	LDR		r4, [r0], #4	
	LDR		r6, [r12], #4
	SSAT	r4, #16, r4
	SSAT	r6, #16, r6
	PKHBT	r4,	r4,	r6, LSL #16
	STR		r4, [r1], #4
	B		EndLoop
	
OneChannel:
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer@
	MOV		lr, r2, lsr #2										@lr = cSamples/4	
LoopOneChannel:	
	LDRD	r4, [r0], #8										@ iPCMData = (PCMSAMPLE)pcfPCMBuf[i]@	
	LDRD	r6, [r0], #8
	SUBS	lr, lr, #1	
	SSAT	r4, #16, r4	
	SSAT	r5, #16, r5
	SSAT	r6, #16, r6
	SSAT	r7, #16, r7
	PKHBT	r4,	r4,	r5, LSL #16									@armv6
	PKHBT	r5,	r6,	r7, LSL #16
	STRD	r4, [r1], #8
	BNE		LoopOneChannel
	ANDS	lr, r2, #3	
	BEQ		EndLoop
LoopOdd:															@max odd samples is 3
	LDR		r4, [r0], #4
	SUBS	lr, lr, #1
	SSAT	r4, #16, r4	
	STRH	r4, [r1], #2
	BNE		LoopOdd
	BEQ		EndLoop
	
MultiChannel:
	MOV		r3, #0
LoopMultiChannel:
	MOV		r7, r4, lsr #1										@r7 = channel/2
	MOV		r8, r5
gInLoop16:
	LDR		r0, [r8, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer@
	LDR		r12, [r8, #next_PerChannelInfo_m_rgiPCMBuffer]
	ADD		r8, r8, #double_PerChannelInfo_size
	LDR		r6, [r0, r3, LSL #2]
	LDR		lr, [r12, r3, LSL #2]
	
	SSAT	r6, #16, r6	
	SSAT	lr, #16, lr
	SUBS	r7, r7, #1
	
	PKHBT	r6, r6, lr, LSL #16
	STR		r6, [r1], #4	
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
	.if		DEVICEPCMOUT16	==	1
@ Input parameters
@ r0 = pau
@ r1 = piOutput
@ r2 = cSamples

@ r4 = pau->m_cChannel
@ r5 = pau->m_rgpcinfo

	STMFD sp!, {r4 - r8, lr}
	CMP		r2, #0
	BEQ		EndLoop_24
	LDRH  r4, [r0, #CAudioObject_m_cChannel]
	LDR   r5, [r0, #CAudioObject_m_rgpcinfo]
	
	CMP		r4, #2
	BLT		OneChannel_24
	BGT		MultiChannel_24
	
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer@
	LDR		r12, [r5, #next_PerChannelInfo_m_rgiPCMBuffer]
	MOV		lr, r2, lsr #1										@lr = cSamples/2
LoopTwoChannel_24:		
	LDRD	r4, [r0], #8										@ iPCMData = (PCMSAMPLE)pcfPCMBuf[i]@
	LDRD	r6, [r12], #8
	SUBS	lr, lr, #1
	SSAT	r4, #16, r4, ASR #8
	SSAT	r6, #16, r6, ASR #8
	SSAT	r5, #16, r5, ASR #8
	SSAT	r7, #16, r7, ASR #8
	PKHBT	r4,	r4,	r6, LSL #16									@armv6
	PKHBT	r5,	r5,	r7, LSL #16
	STRD	r4, [r1], #8	
	BNE		LoopTwoChannel_24
	ANDS	r3, r2, #1
	BEQ		EndLoop_24
	LDR		r4, [r0], #4	
	LDR		r6, [r12], #4
	SSAT	r4, #16, r4, ASR #8
	SSAT	r6, #16, r6, ASR #8
	PKHBT	r4,	r4,	r6, LSL #16
	STR		r4, [r1], #4
	B		EndLoop_24
	
OneChannel_24:	
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer@
	MOV		lr, r2, lsr #2										@lr = cSamples/4	
LoopOneChannel_24:	
	LDRD	r4, [r0], #8										@ iPCMData = (PCMSAMPLE)pcfPCMBuf[i]@	
	LDRD	r6, [r0], #8
	SUBS	lr, lr, #1	
	SSAT	r4, #16, r4, ASR #8
	SSAT	r5, #16, r5, ASR #8
	SSAT	r6, #16, r6, ASR #8
	SSAT	r7, #16, r7, ASR #8
	PKHBT	r4,	r4,	r5, LSL #16									@armv6
	PKHBT	r5,	r6,	r7, LSL #16
	STRD	r4, [r1], #8
	BNE		LoopOneChannel_24
	ANDS	lr, r2, #3
	BEQ		EndLoop_24
LoopOdd_24:															@max odd samples is 3
	LDR		r4, [r0], #4
	SUBS	lr, lr, #1
	SSAT	r4, #16, r4, ASR #8
	STRH	r4, [r1], #2
	BNE		LoopOdd
	BEQ		EndLoop
	
MultiChannel_24:
	MOV		r3, #0
LoopMultiChannel_24:
	MOV		r7, r4, lsr #1										@r7 = channel/2
	MOV		r8, r5
gInLoop24:
	LDR		r0, [r8, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer@
	LDR		r12, [r8, #next_PerChannelInfo_m_rgiPCMBuffer]
	ADD		r8, r8, #double_PerChannelInfo_size
	LDR		r6, [r0, r3, LSL #2]
	LDR		lr, [r12, r3, LSL #2]
	SUBS	r7, r7, #1
	SSAT	r6, #16, r6, ASR #8
	SSAT	lr, #16, lr, ASR #8
	PKHBT	r6,	r6,	lr, LSL #16
	STR		r6, [r1], #4
	BNE		gInLoop24
	ADD		r3, r3, #1
	CMP		r3, r2
	BLT		LoopMultiChannel_24
	
EndLoop_24:
	MOV r0, #0
  	LDMFD sp!, {r4 - r8, PC} @auReconSample24_ARM
  	ENTRY_END auReconSample24_ARM

	.else	@DEVICEPCMOUT16
	
@ Input parameters
@ r0 = pau
@ r1 = piOutput
@ r2 = cSamples

@ r4 = pau->m_cChannel
@ r5 = pau->m_rgpcinfo


  STMFD sp!, {r4 - r8, lr}
  CMP		r2, #0
	BEQ		EndLoop_24_1
  LDRH  r4, [r0, #CAudioObject_m_cChannel]
  LDR   r5, [r0, #CAudioObject_m_rgpcinfo]
  
	CMP		r4, #2
	BLT		OneChannel_24
	BGT		MultiChannel_24
	
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer@
	LDR		r12, [r5, #next_PerChannelInfo_m_rgiPCMBuffer]
	MOV		lr, r2, lsr #1										@lr = cSamples/2
LoopTwoChannel_24_1:		
	LDRD	r4, [r0], #8										@ iPCMData = (PCMSAMPLE)pcfPCMBuf[i]@
	LDRD	r6, [r12], #8
	SUBS	lr, lr, #1
	SSAT	r4, #24, r4
	SSAT	r6, #24, r6
	SSAT	r5, #24, r5	
	SSAT	r7, #24, r7	
	MOV		r4, r4, LSL #8
	MOV		r6, r6, LSL #8
	MOV		r5, r5, LSL #8
	MOV		r7, r7, LSL #8
	MOV		r4, r4, LSR #8	
	ORR		r4, r4, r6, LSL #16
	ORR		r7, r7, r5, LSR #24
	MOV		r5, r5, LSL #8
	ORR		r5, r5, r6, LSR #16
	STRD	r4, [r1], #8
	STR		r7, [r1], #4	
	BNE		LoopTwoChannel_24_1
	ANDS	r3, r2, #1
	BEQ		EndLoop_24_1
	LDR		r4, [r0], #4	
	LDR		r6, [r12], #4
	SSAT	r4, #24, r4
	SSAT	r6, #24, r6
	MOV		r4, r4, LSL #8
	MOV		r4, r4, LSR #8
	ORR		r4, r4, r6, LSL #24
	MOV		r6, r6, LSR #8
	STR		r4, [r1], #4
	STRH	r6, [r1], #2
	B		EndLoop_24_1
	
OneChannel_24_1:	
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer@
	MOV		lr, r2, lsr #2										@lr = cSamples/4	
LoopOneChannel_24_1:	
	LDRD	r4, [r0], #8										@ iPCMData = (PCMSAMPLE)pcfPCMBuf[i]@	
	LDRD	r6, [r0], #8
	SUBS	lr, lr, #1	
	SSAT	r4, #24, r4	
	SSAT	r5, #24, r5
	SSAT	r6, #24, r6
	SSAT	r7, #24, r7
	MOV		r4, r4, LSL #8
	MOV		r5, r5, LSL #8
	MOV		r6, r6, LSL #8
	MOV		r7, r7, LSL #8
	MOV		r4, r4, LSR #8
	ORR		r4, r4, r5, LSL #16
	ORR		r7, r7, r6, LSR #24
	MOV		r5, r5, LSR #16
	ORR		r5, r5, r6, LSL #8
	STRD	r4, [r1], #8
	STR		r7, [r1], #4
	BNE		LoopOneChannel_24_1
	ANDS	lr, r2, #3	
	BEQ		EndLoop_24_1
	MOV		r6, #0xff
LoopOdd_24_1:															@max odd samples is 3
	LDR		r4, [r0], #4
	SUBS	lr, lr, #1
	SSAT	r4, #24, r4	
	ORR		r5, r4, r6
	STRB	r5, [r1], #1
	ORR		r5, r6, r4, LSR #8
	STRB	r5, [r1], #1
	ORR		r5, r6, r4, LSR #16
	STRB	r5, [r1], #1
	BNE		LoopOdd_24
	BEQ		EndLoop_24
	
MultiChannel_24_1:
	MOV		r3, #0
LoopMultiChannel_24_1:
	MOV		r7, r4, lsr #1										@r7 = channel/2
	MOV		r8, r5
gInLoop24_1:
	LDR		r0, [r8, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer@
	LDR		r12, [r8, #next_PerChannelInfo_m_rgiPCMBuffer]
	ADD		r8, r8, #double_PerChannelInfo_size
	LDR		r6, [r0, r3, LSL #2]
	LDR		lr, [r12, r3, LSL #2]
	SUBS	r7, r7, #1
	SSAT	r6, #24, r6	
	SSAT	lr, #24, lr
	MOV		r6, r6, LSL #8
	MOV		r6, r6, LSR #8
	ORR		r6, r6, lr, LSL #24
	MOV		lr, lr, LSR #8	
	STR		r6, [r1], #4
	STRH	lr, [r1], #2	
	BNE		gInLoop24_1
	ADD		r3, r3, #1
	CMP		r3, r2
	BLT		LoopMultiChannel_24_1
			
EndLoop_24_1:
	MOV r0, #0
  	LDMFD sp!, {r4 - r8, PC} @auReconSample24_ARM
  	ENTRY_END auReconSample24_ARM

	.endif	@DEVICEPCMOUT16
  	.endif @ WMA_OPT_AURECON_ARM
    
  	.end
  	
  	