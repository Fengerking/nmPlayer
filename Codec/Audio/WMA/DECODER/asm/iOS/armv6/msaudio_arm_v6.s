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

  #include "../../../inc/audio/v10/include/voWMADecID.h"
  .include     "kxarm.h"
  .include     "wma_member_arm.inc"
  .include	  "wma_arm_version.h"
 
  @AREA    |.text|, CODE, READONLY
  .text   .align 4

  .if WMA_OPT_AURECON_ARM == 1
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
@	.extern		printf

  .globl  _auReconSample16_ARM
  .globl  _auReconSample24_ARM
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Constants for auReconSample16_ARM
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.set next_PerChannelInfo_m_rgiPCMbuffer	,	PerChannelInfo_size+PerChannelInfo_m_rgiPCMBuffer
.set double_PerChannelInfo_size			, 2*PerChannelInfo_size

@//*************************************************************************************
@//
@// WMARESULT auReconSample16_ARM(CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples)
@//
@//*************************************************************************************

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  @AREA    |.text|, CODE
_auReconSample16_ARM:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pau
@ r1 = piOutput
@ r2 = cSamples

@ r4 = pau->m_cChannel
@ r5 = pau->m_rgpcinfo


  stmfd sp!, {r4 - r8, lr}
  
	cmp		r2, #0
	beq		EndLoop
	
  ldrh  r4, [r0, #CAudioObject_m_cChannel]
  ldr   r5, [r0, #CAudioObject_m_rgpcinfo]

	cmp		r4, #2
	blt		OneChannel
	bgt		MultiChannel
	
	ldr		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMbuf = (CoefType *)ppcinfo->m_rgiPCMbuffer@
	ldr		r12, [r5, #next_PerChannelInfo_m_rgiPCMbuffer]
	mov		lr, r2, lsr #1										@lr = cSamples/2
LoopTwoChannel:	
	ldrd	r4, [r0], #8										@ iPCMData = (PCMSAMPLE)pcfPCMbuf[i]@
	ldrd	r6, [r12], #8
	subs	lr, lr, #1
	ssat	r4, #16, r4
	ssat	r6, #16, r6
	ssat	r5, #16, r5	
	ssat	r7, #16, r7	
	pkhbt	r4,	r4,	r6, lsl #16									@ armv6
	pkhbt	r5,	r5,	r7, lsl #16
	strd	r4, [r1], #8	
	bne		LoopTwoChannel
	ands	r3, r2, #1
	beq		EndLoop
	ldr		r4, [r0], #4	
	ldr		r6, [r12], #4
	ssat	r4, #16, r4
	ssat	r6, #16, r6
	pkhbt	r4,	r4,	r6, lsl #16
	str		r4, [r1], #4
	b		EndLoop
	
OneChannel:
	ldr		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMbuf = (CoefType *)ppcinfo->m_rgiPCMbuffer@
	mov		lr, r2, lsr #2										@lr = cSamples/4	
LoopOneChannel:	
	ldrd	r4, [r0], #8										@ iPCMData = (PCMSAMPLE)pcfPCMbuf[i]@	
	ldrd	r6, [r0], #8
	subs	lr, lr, #1	
	ssat	r4, #16, r4	
	ssat	r5, #16, r5
	ssat	r6, #16, r6
	ssat	r7, #16, r7
	pkhbt	r4,	r4,	r5, lsl #16									@armv6
	pkhbt	r5,	r6,	r7, lsl #16
	strd	r4, [r1], #8
	bne		LoopOneChannel
	ands	lr, r2, #3	
	beq		EndLoop
LoopOdd:															@max odd samples is 3
	ldr		r4, [r0], #4
	subs	lr, lr, #1
	ssat	r4, #16, r4	
	strh	r4, [r1], #2
	bne		LoopOdd
	beq		EndLoop
	
MultiChannel:
	mov		r3, #0
LoopMultiChannel:
	mov		r7, r4, lsr #1										@r7 = channel/2
	mov		r8, r5
gInLoop16:
	ldr		r0, [r8, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMbuf = (CoefType *)ppcinfo->m_rgiPCMbuffer@
	ldr		r12, [r8, #next_PerChannelInfo_m_rgiPCMbuffer]
	add		r8, r8, #double_PerChannelInfo_size
	ldr		r6, [r0, r3, lsl #2]
	ldr		lr, [r12, r3, lsl #2]
	
	ssat	r6, #16, r6	
	ssat	lr, #16, lr
	subs	r7, r7, #1
	
	pkhbt	r6, r6, lr, lsl #16
	str		r6, [r1], #4	
	bne		gInLoop16
	add		r3, r3, #1
	cmp		r3, r2
	blt		LoopMultiChannel
	
EndLoop:
	mov r0, #0
	ldmfd sp!, {r4 - r8, PC} @auReconSample16_ARM
	
@ENTRY_END auReconSample16_ARM


@//*************************************************************************************
@//
@// WMARESULT auReconSample24_ARM(CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples)
@//
@//*************************************************************************************

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  @AREA    |.text|, CODE
_auReconSample24_ARM:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.if		DEVICEPCMOUT16	==	1
@ Input parameters
@ r0 = pau
@ r1 = piOutput
@ r2 = cSamples

@ r4 = pau->m_cChannel
@ r5 = pau->m_rgpcinfo

	stmfd sp!, {r4 - r8, lr}
	cmp		r2, #0
	beq		EndLoop_24
	ldrh  r4, [r0, #CAudioObject_m_cChannel]
	ldr   r5, [r0, #CAudioObject_m_rgpcinfo]
	
	cmp		r4, #2
	blt		OneChannel_24
	bgt		MultiChannel_24
	
	ldr		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMbuf = (CoefType *)ppcinfo->m_rgiPCMbuffer@
	ldr		r12, [r5, #next_PerChannelInfo_m_rgiPCMbuffer]
	mov		lr, r2, lsr #1										@lr = cSamples/2
LoopTwoChannel_24:		
	ldrd	r4, [r0], #8										@ iPCMData = (PCMSAMPLE)pcfPCMbuf[i]@
	ldrd	r6, [r12], #8
	subs	lr, lr, #1
	ssat	r4, #16, r4, asr #8
	ssat	r6, #16, r6, asr #8
	ssat	r5, #16, r5, asr #8
	ssat	r7, #16, r7, asr #8
	pkhbt	r4,	r4,	r6, lsl #16									@armv6
	pkhbt	r5,	r5,	r7, lsl #16
	strd	r4, [r1], #8	
	bne		LoopTwoChannel_24
	ands	r3, r2, #1
	beq		EndLoop_24
	ldr		r4, [r0], #4	
	ldr		r6, [r12], #4
	ssat	r4, #16, r4, asr #8
	ssat	r6, #16, r6, asr #8
	pkhbt	r4,	r4,	r6, lsl #16
	str		r4, [r1], #4
	b		EndLoop_24
	
OneChannel_24:	
	ldr		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMbuf = (CoefType *)ppcinfo->m_rgiPCMbuffer@
	mov		lr, r2, lsr #2										@lr = cSamples/4	
LoopOneChannel_24:	
	ldrd	r4, [r0], #8										@ iPCMData = (PCMSAMPLE)pcfPCMbuf[i]@	
	ldrd	r6, [r0], #8
	subs	lr, lr, #1	
	ssat	r4, #16, r4, asr #8
	ssat	r5, #16, r5, asr #8
	ssat	r6, #16, r6, asr #8
	ssat	r7, #16, r7, asr #8
	pkhbt	r4,	r4,	r5, lsl #16									@armv6
	pkhbt	r5,	r6,	r7, lsl #16
	strd	r4, [r1], #8
	bne		LoopOneChannel_24
	ands	lr, r2, #3
	beq		EndLoop_24
LoopOdd_24:															@max odd samples is 3
	ldr		r4, [r0], #4
	subs	lr, lr, #1
	ssat	r4, #16, r4, asr #8
	strh	r4, [r1], #2
	bne		LoopOdd
	beq		EndLoop
	
MultiChannel_24:
	mov		r3, #0
LoopMultiChannel_24:
	mov		r7, r4, lsr #1										@r7 = channel/2
	mov		r8, r5
gInLoop24:
	ldr		r0, [r8, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMbuf = (CoefType *)ppcinfo->m_rgiPCMbuffer@
	ldr		r12, [r8, #next_PerChannelInfo_m_rgiPCMbuffer]
	add		r8, r8, #double_PerChannelInfo_size
	ldr		r6, [r0, r3, lsl #2]
	ldr		lr, [r12, r3, lsl #2]
	subs	r7, r7, #1
	ssat	r6, #16, r6, asr #8
	ssat	lr, #16, lr, asr #8
	pkhbt	r6,	r6,	lr, lsl #16
	str		r6, [r1], #4
	bne		gInLoop24
	add		r3, r3, #1
	cmp		r3, r2
	blt		LoopMultiChannel_24
	
EndLoop_24:
	mov r0, #0
  ldmfd sp!, {r4 - r8, PC} @auReconSample24_ARM
  	@ENTRY_END auReconSample24_ARM

	.else	@DEVICEPCMOUT16
	
@ Input parameters
@ r0 = pau
@ r1 = piOutput
@ r2 = cSamples

@ r4 = pau->m_cChannel
@ r5 = pau->m_rgpcinfo


  stmfd sp!, {r4 - r8, lr}
  cmp		r2, #0
	beq		EndLoop_24_1
  ldrh  r4, [r0, #CAudioObject_m_cChannel]
  ldr   r5, [r0, #CAudioObject_m_rgpcinfo]
  
	cmp		r4, #2
	blt		OneChannel_24
	bgt		MultiChannel_24
	
	ldr		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMbuf = (CoefType *)ppcinfo->m_rgiPCMbuffer@
	ldr		r12, [r5, #next_PerChannelInfo_m_rgiPCMbuffer]
	mov		lr, r2, lsr #1										@lr = cSamples/2
LoopTwoChannel_24_1:		
	ldrd	r4, [r0], #8										@ iPCMData = (PCMSAMPLE)pcfPCMbuf[i]@
	ldrd	r6, [r12], #8
	subs	lr, lr, #1
	ssat	r4, #24, r4
	ssat	r6, #24, r6
	ssat	r5, #24, r5	
	ssat	r7, #24, r7	
	mov		r4, r4, lsl #8
	mov		r6, r6, lsl #8
	mov		r5, r5, lsl #8
	mov		r7, r7, lsl #8
	mov		r4, r4, lsr #8	
	orr		r4, r4, r6, lsl #16
	orr		r7, r7, r5, lsr #24
	mov		r5, r5, lsl #8
	orr		r5, r5, r6, lsr #16
	strd	r4, [r1], #8
	str		r7, [r1], #4	
	bne		LoopTwoChannel_24_1
	ands	r3, r2, #1
	beq		EndLoop_24_1
	ldr		r4, [r0], #4	
	ldr		r6, [r12], #4
	ssat	r4, #24, r4
	ssat	r6, #24, r6
	mov		r4, r4, lsl #8
	mov		r4, r4, lsr #8
	orr		r4, r4, r6, lsl #24
	mov		r6, r6, lsr #8
	str		r4, [r1], #4
	strh	r6, [r1], #2
	b		EndLoop_24_1
	
OneChannel_24_1:	
	ldr		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMbuf = (CoefType *)ppcinfo->m_rgiPCMbuffer@
	mov		lr, r2, lsr #2										@lr = cSamples/4	
LoopOneChannel_24_1:	
	ldrd	r4, [r0], #8										@ iPCMData = (PCMSAMPLE)pcfPCMbuf[i]@	
	ldrd	r6, [r0], #8
	subs	lr, lr, #1	
	ssat	r4, #24, r4	
	ssat	r5, #24, r5
	ssat	r6, #24, r6
	ssat	r7, #24, r7
	mov		r4, r4, lsl #8
	mov		r5, r5, lsl #8
	mov		r6, r6, lsl #8
	mov		r7, r7, lsl #8
	mov		r4, r4, lsr #8
	orr		r4, r4, r5, lsl #16
	orr		r7, r7, r6, lsr #24
	mov		r5, r5, lsr #16
	orr		r5, r5, r6, lsl #8
	strd	r4, [r1], #8
	str		r7, [r1], #4
	bne		LoopOneChannel_24_1
	ands	lr, r2, #3	
	beq		EndLoop_24_1
	mov		r6, #0xff
LoopOdd_24_1:															@max odd samples is 3
	ldr		r4, [r0], #4
	subs	lr, lr, #1
	ssat	r4, #24, r4	
	orr		r5, r4, r6
	strb	r5, [r1], #1
	orr		r5, r6, r4, lsr #8
	strb	r5, [r1], #1
	orr		r5, r6, r4, lsr #16
	strb	r5, [r1], #1
	bne		LoopOdd_24
	beq		EndLoop_24
	
MultiChannel_24_1:
	mov		r3, #0
LoopMultiChannel_24_1:
	mov		r7, r4, lsr #1										@r7 = channel/2
	mov		r8, r5
gInLoop24_1:
	ldr		r0, [r8, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMbuf = (CoefType *)ppcinfo->m_rgiPCMbuffer@
	ldr		r12, [r8, #next_PerChannelInfo_m_rgiPCMbuffer]
	add		r8, r8, #double_PerChannelInfo_size
	ldr		r6, [r0, r3, lsl #2]
	ldr		lr, [r12, r3, lsl #2]
	subs	r7, r7, #1
	ssat	r6, #24, r6	
	ssat	lr, #24, lr
	mov		r6, r6, lsl #8
	mov		r6, r6, lsr #8
	orr		r6, r6, lr, lsl #24
	mov		lr, lr, lsr #8	
	str		r6, [r1], #4
	strh	lr, [r1], #2	
	bne		gInLoop24_1
	add		r3, r3, #1
	cmp		r3, r2
	blt		LoopMultiChannel_24_1
			
EndLoop_24_1:
	mov r0, #0
  ldmfd sp!, {r4 - r8, PC} @auReconSample24_ARM
@  	ENTRY_END auReconSample24_ARM

	.endif	@DEVICEPCMOUT16
  .endif @ WMA_OPT_AURECON_ARM
    
  	@.end
  	
  	