@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@// Filename msaudio_arm_v6.s
@
@// Copyright (c) VisualOn SoftWare Co., Ltd. All rights reserved.
@
@//*@@@---@@@@******************************************************************
@//
@// Abstract:
@// 
@//     ARM Arch-7 specific multiplications
@//
@//      Custom build with 
@//          armasm -cpu arm1136 $(InputPath) "$(IntDir)/$(InputName).obj"
@//      and
@//          $(OutDir)\$(InputName).obj
@// 
@// Author:
@// 
@//     Witten Wen (Shanghai, China) September 15, 2008
@//
@// Revision History:
@//
@@//*************************************************************************


@  OPT         2       @ disable listing 
  #include "../../../inc/audio/v10/include/voWMADecID.h"
  .include     "kxarm.h"
  .include     "wma_member_arm.inc"
  .include		"wma_arm_version.h"
@  OPT         1       @ enable listing	
 
@  AREA    |.text|, CODE, READONLY
  .text .align 4
 		 
	.if	ARMVERSION	>= 7
  .if WMA_OPT_AURECON_ARM == 1
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


  .globl  _auReconSample16_ARM
  .globl  _auReconSample24_ARM
  
@******************************
@ Neon register 
@******************************
@A0			.req	D0
@A1			.req	D1
@A2			.req	D2
@A3			.req	D3
@A4			.req	D4
@A5			.req	D5
@A6			.req	D6
@A7			.req	D7
@A8			.req	D8
@A9			.req	D9
@A10			.req	D10
@A11			.req	D11
@A12			.req	D16
@A13			.req	D17
@A14			.req	D18
@A15			.req	D19

@B0			.req	Q0
@B1			.req	Q1
@B2			.req	Q2
@B3			.req	Q3
@B4			.req	Q4
@B5			.req	Q5
@B6			.req	Q8
@B7			.req	Q9

@C0			.req	D12
@C1			.req	D13
@C2			.req	D14
@C3			.req	D15
@C4			.req	D20
@C5			.req	D21
@C6			.req	D22
@C7			.req	D23

@C0_0		.req	S24
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Constants for auReconSample16_ARM
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.set	next_PerChannelInfo_m_rgiPCMBuffer	,	PerChannelInfo_size+PerChannelInfo_m_rgiPCMBuffer
.set	next2_PerChannelInfo_m_rgiPCMBuffer	,	2*PerChannelInfo_size+PerChannelInfo_m_rgiPCMBuffer
.set	double_PerChannelInfo_size			, 2*PerChannelInfo_size
@//*************************************************************************************
@//
@// WMARESULT auReconSample16_ARM(CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples)
@//
@//*************************************************************************************

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@  AREA    |.text|, CODE
_auReconSample16_ARM:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pau
@ r1 = piOutput
@ r2 = cSamples

@ r4 = pau->m_cChannel
@ r5 = pau->m_rgpcinfo

	stmfd sp!, {r4 - r9, lr}

	cmp		r2, #0
	beq		EndLoop
	
	ldrh  r4, [r0, #CAudioObject_m_cChannel]
	ldr   r5, [r0, #CAudioObject_m_rgpcinfo]

	cmp		r4, #2
	blt		OneChannel
	bgt		MultiChannel
	
	ldr		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer@
	ldr		r12, [r5, #next_PerChannelInfo_m_rgiPCMBuffer]
	mov		lr, r2, lsr #3							@ lr = cSamples/8			
LoopTwoChannel:			
	vld1.32			{D0, D1, D2, D3}, [r0]!
	vqmovn.S32		D12, Q0	
	vld1.32			{D4, D5, D6, D7}, [r12]!	
	vqmovn.S32		D13, Q1	
	vqmovn.S32		D14, Q2
	vqmovn.S32		D15, Q3	
	subs			lr, lr, #1
	vst2.16		{D12, D13, D14, D15}, [r1]!			@9 stalls
	bne				LoopTwoChannel
	ands			r3, r2, #7
	beq			EndLoop
LoopTwoOdd:
	vld1.32		{D0[0]}, [r0]!
	vld1.32		{D0[1]}, [r12]!
	vqmovn.S32		D12, Q0
	subs		r3, r3, #1
	vstr		S24, [r1]
	add			r1, r1, #4
	
	bne		LoopTwoOdd
	beq		EndLoop
	
OneChannel:	
	ldr		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer@
	mov		lr, r2, lsr #3							@lr = cSamples/8 
	
LoopOneChannel:
	vld2.32		{D0, D1, D2, D3}, [r0]!
	vqmovn.S32		D12, Q0
	vqmovn.S32		D13, Q1
	subs			lr, lr, #1
	vst2.16		{D12, D13}, [r1]!		
	bne				LoopOneChannel
	ands			r3, r2, #7
	beq				EndLoop	
LoopOneOdd:
	ldr			r2, [r0], #4
	subs		r3, r3, #1
	ssat		r2, #16, r4	
	strh		r2, [r1], #2
	bne			LoopOneOdd
	beq			EndLoop
	
MultiChannel:
  cmp            r4, #3
  beq             MultiNo6_8_16
  cmp             r4, #4
  beq             MultiNo6_8_16
	ldr			r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]!	@//channel 0	
	ldr			r3, [r5, #PerChannelInfo_size]!				@//channel 1
	ldr			r6, [r5, #PerChannelInfo_size]!				@//channel 2	
	ldr			r7, [r5, #PerChannelInfo_size]!				@//channel 3
	ldr			r8, [r5, #PerChannelInfo_size]!				@//channel 4
	cmp			r4, #6
	ldr			r12, [r5, #PerChannelInfo_size]!			@//channel 5
	beq			SixChannel
	
	ldr			r4, [r5, #PerChannelInfo_size]!				@//channel 6
	mov			r9, r2, LSR #2								@ r9 = cSamples/2 
	ldr			lr, [r5, #PerChannelInfo_size]				@//channel 7
	
LoopEightChannel:
	vld4.32		{D0[0], D1[0], D2[0], D3[0]}, [r0]!		@//0	
	vld4.32		{D0[1], D1[1], D2[1], D3[1]}, [r3]!		@//1	
	vld4.32		{D4[0], D5[0], D6[0], D7[0]}, [r6]!		@//2
	vqmovn.S32		D12, Q0
	vld4.32		{D4[1], D5[1], D6[1], D7[1]}, [r7]!		@//3	
	vqmovn.S32		D20, Q1
	vld4.32		{D8[0], D9[0], D10[0], D11[0]}, [r8]!	@//4
	vqmovn.S32		D13, Q2
	vld4.32		{D8[1], D9[1], D10[1], D11[1]}, [r12]!	@//5
	vqmovn.S32		D21, Q3
	vld4.32		{D16[0], D17[0], D18[0], D19[0]}, [r4]!	@//6
	vqmovn.S32		D14, Q4
	vld4.32		{D16[1], D17[1], D18[1], D19[1]}, [lr]!	@//7		
	vqmovn.S32		D15, Q8	
	vqmovn.S32		D22, Q5
	vqmovn.S32		D23, Q9	
	vst4.32			{D12, D13, D14, D15}, [r1]!			
	subs			r9, r9, #1
	vst4.32			{D20, D21, D22, D23}, [r1]!					@13 stalls
	bne				LoopEightChannel
	ands			r9, r2, #3
	beq				EndLoop
LoopEightOdd:
	vld1.32		{D0[0]}, [r0]!		@//0
	vld1.32		{D0[1]}, [r6]!		@//2
	vld1.32		{D1[0]}, [r8]!		@//4
	vld1.32		{D1[1]}, [r4]!		@//6
	vld1.32		{D2[0]}, [r3]!		@//1
	vqmovn.S32		D12, Q0
	vld1.32		{D2[1]}, [r7]!		@//3
	vld1.32		{D3[0]}, [r12]!		@//5
	vld1.32		{D3[1]}, [lr]!		@//7	
	vqmovn.S32		D13, Q1
	subs		r9, r9, #1
	vst2.16		{D12, D13}, [r1]!
	bne			LoopEightOdd
	beq			EndLoop
	
SixChannel:	
	mov			r9, r2, LSR #2								@ r9 = cSamples/4 
LoopSixChannel:
	vld1.32		{D0[0]}, [r0]!		@//0
	vld1.32		{D0[1]}, [r3]!		@//1
	vld1.32		{D2[0]}, [r6]!		@//2
	vld1.32		{D2[1]}, [r7]!		@//3
	vld1.32		{D4[0]}, [r8]!		@//4
	vld1.32		{D4[1]}, [r12]!		@//5
	vld1.32		{D6[0]}, [r0]!		@//0
	vld1.32		{D6[1]}, [r3]!		@//1
	vld1.32		{D1[0]}, [r6]!		@//2
	vld1.32		{D1[1]}, [r7]!		@//3
	vld1.32		{D3[0]}, [r8]!		@//4
	vqmovn.S32		D12, Q0
	vld1.32		{D3[1]}, [r12]!		@//5
	vld1.32		{D5[0]}, [r0]!		@//0
	vqmovn.S32		D13, Q1
	vld1.32		{D5[1]}, [r3]!		@//1
	vld1.32		{D7[0]}, [r6]!		@//2
	vqmovn.S32		D14, Q2
	vld1.32		{D7[1]}, [r7]!		@//3	
	vqmovn.S32		D15, Q3
	vld1.32		{D0[0]}, [r8]!		@//4->0
	vld1.32		{D0[1]}, [r0]!		@//0->2
	vld1.32		{D1[0]}, [r6]!		@//2->4	
	vst4.32			{D12, D13, D14, D15}, [r1]!		
	vld1.32		{D1[1]}, [r8]!		@//4->6
	vld1.S32		{D2[0]}, [r12]!		@//5->1
	vqmovn.S32		D12, Q0
	vld1.32		{D2[1]}, [r3]!		@//1->3
	vld1.32		{D3[0]}, [r7]!		@//3->5
	vld1.32		{D3[1]}, [r12]!		@//5->7		
	vqmovn.S32		D13, Q1
	subs			r9, r9, #1
	vst2.16		{D12, D13}, [r1]!			@29 stalls
	bne				LoopSixChannel
	ands			r9, r2, #3
	beq				EndLoop	
LoopSixOdd:
	vld1.32		{D0[0]}, [r0]!		@//0
	vld1.32		{D0[1]}, [r3]!		@//1
	vld1.32		{D1[0]}, [r6]!		@//2
	vld1.32		{D1[1]}, [r7]!		@//3
	vld1.32		{D2[0]}, [r8]!		@//4
	vqmovn.S32		D12, Q0
	vld1.32		{D2[1]}, [r12]!		@//5		
	vqmovn.S32		D13, Q1		
	vst1.32		D12, [r1]!
	subs		r9, r9, #1
	vst1.32		{D13[0]}, [r1]!
	bne			LoopSixOdd
  beq            EndLoop
	
MultiNo6_8_16:
	mov		r3, #0
LoopMultiChannel:
	mov		r7, r4, lsr #1										@r7 = channel/2
	mov		r8, r5
gInLoop16:
	ldr		r0, [r8, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer@
	ldr		r12, [r8, #next_PerChannelInfo_m_rgiPCMBuffer]
	add		r8, r8, #double_PerChannelInfo_size
	ldr		r6, [r0, r3, LSL #2]
	ldr		lr, [r12, r3, LSL #2]
	ssat	        r6, #16, r6	
	ssat	        lr, #16, lr
	subs	        r7, r7, #1
	pkhbt	        r6, r6, lr, LSL #16
	str		r6, [r1], #4	
	bne		gInLoop16
	add		r3, r3, #1
	cmp		r3, r2
	blt		LoopMultiChannel
EndLoop:
	mov r0, #0
	ldmfd sp!, {r4 - r9, PC} @auReconSample16_ARM
	
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

	stmfd	sp!, {r4 - r9, lr}
	cmp		r2, #0
	beq		EndLoop_24
	ldrh	r4, [r0, #CAudioObject_m_cChannel]
	ldr		r5, [r0, #CAudioObject_m_rgpcinfo]
	
	cmp		r4, #2
	blt		OneChannel_24
	bgt		MultiChannel_24
	
	ldr		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer;
	ldr		r12, [r5, #next_PerChannelInfo_m_rgiPCMBuffer]
	mov		lr, r2, lsr #3										@lr = cSamples/8
LoopTwoChannel_24:	
	vld1.32			{D0, D1, D2, D3}, [r0]!
	vshr.S32		Q0, Q0, #8
	vqmovn.S32		D12, Q0	
	vld1.32			{D4, D5, D6, D7}, [r12]!	
	vshr.S32		Q1, Q1, #8
	vqmovn.S32	D13, Q1	
	vshr.S32		Q2, Q2, #8
	vqmovn.S32		D14, Q2
	vshr.S32		Q3, Q3, #8
	vqmovn.S32		D15, Q3	
	subs			lr, lr, #1
	vst2.16			{D12, D13, D14, D15}, [r1]!			@9 stalls
	bne				LoopTwoChannel_24
	ands			r3, r2, #7
	beq				EndLoop_24
LoopTwoOdd_24:
	vld1.32			{D0[0]}, [r0]!
	vld1.32			{D0[1]}, [r12]!
	vshr.S32			Q0, Q0, #8
	vqmovn.S32		D12, Q0
	subs			r3, r3, #1
	vstr			S24, [r1]
	add				r1, r1, #4
	bne				LoopTwoOdd_24
	beq				EndLoop_24		
	
OneChannel_24:	
	ldr		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer;
	mov		lr, r2, lsr #3										@lr = cSamples/8	
LoopOneChannel_24:	
	vld2.32		{D0, D1, D2, D3}, [r0]!
	vshr.S32	Q0, Q0, #8
	vshr.S32	Q1, Q1, #8
	vqmovn.S32	D12, Q0
	vqmovn.S32	D13, Q1
	subs		lr, lr, #1
	vst2.16		{D12, D13}, [r1]!		
	bne			LoopOneChannel_24
	ands		r3, r2, #7
	beq			EndLoop_24	
LoopOneOdd_24:
	ldr			r2, [r0], #4
	subs		r3, r3, #1
	ssat		r2, #16, r4, ASR #8
	strh		r2, [r1], #2
	bne			LoopOneOdd_24
	beq			EndLoop_24
	
MultiChannel_24:
	mov		  r3, #0										@//for orthers, r3 is sample number.
	cmp			r4, #6
	beq		  MultiNO6_8_24
	cmp			r4, #8

	bne			MultiNO6_8_24
	ldr			r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]!	@//channel 0	
	ldr			r3, [r5, #PerChannelInfo_size]!				@//channel 1
	ldr			r6, [r5, #PerChannelInfo_size]!				@//channel 2	
	ldr			r7, [r5, #PerChannelInfo_size]!				@//channel 3
	ldr			r8, [r5, #PerChannelInfo_size]!				@//channel 4
	ldr			r12, [r5, #PerChannelInfo_size]!			@//channel 5	
	ldr			r4, [r5, #PerChannelInfo_size]!				@//channel 6
	mov			r9, r2, LSR #2								@ r9 = cSamples/2 
	ldr			lr, [r5, #PerChannelInfo_size]				@//channel 7
	
LoopEightChannel_24:
	vld4.32		{D0[0], D1[0], D2[0], D3[0]}, [r0]!		@//0	
	vld4.32		{D0[1], D1[1], D2[1], D3[1]}, [r3]!		@//1	
	vld4.32		{D4[0], D5[0], D6[0], D7[0]}, [r6]!		@//2
	vshr.S32	Q0, Q0, #8
	vqmovn.S32	D12, Q0
	vld4.32		{D4[1], D5[1], D6[1], D7[1]}, [r7]!		@//3
	vshr.S32	Q1, Q1, #8	
	vqmovn.S32	D20, Q1
	vld4.32		{D8[0], D9[0], D10[0], D11[0]}, [r8]!	@//4
	vshr.S32	Q2, Q2, #8
	vqmovn.S32	D13, Q2
	vld4.32		{D8[1], D9[1], D10[1], D11[1]}, [r12]!	@//5
	vshr.S32	Q3, Q3, #8
	vqmovn.S32	D21, Q3
	vld4.32		{D16[0], D17[0], D18[0], D19[0]}, [r4]!	@//6
	vshr.S32	Q4, Q4, #8
	vqmovn.S32	D14, Q4
	vld4.32		{D16[1], D17[1], D18[1], D19[1]}, [lr]!	@//7
	vshr.S32	Q8, Q8, #8		
	vqmovn.S32	D15, Q8	
	vshr.S32	Q5, Q5, #8		
	vqmovn.S32	D22, Q5
	vshr.S32	Q9, Q9, #8		
	vqmovn.S32	D23, Q9	
	vst4.32		{D12, D13, D14, D15}, [r1]!			
	subs		r9, r9, #1
	vst4.32		{D20, D21, D22, D23}, [r1]!					@13 stalls
	bne			LoopEightChannel_24
	ands		r9, r2, #3
	beq			EndLoop_24
LoopEightOdd_24:
	vld1.32		{D0[0]}, [r0]!		@//0
	vld1.32		{D0[1]}, [r6]!		@//2
	vld1.32		{D1[0]}, [r8]!		@//4
	vld1.32		{D1[1]}, [r4]!		@//6
	vld1.32		{D2[0]}, [r3]!		@//1
	vshr.S32	Q0, Q0, #8
	vqmovn.S32	D12, Q0
	vld1.32		{D2[1]}, [r7]!		@//3
	vld1.32		{D3[0]}, [r12]!		@//5
	vld1.32		{D3[1]}, [lr]!		@//7
	vshr.S32	Q1, Q1, #8	
	vqmovn.S32	D13, Q1
	subs		r9, r9, #1
	vst2.16		{D12, D13}, [r1]!
	bne			LoopEightOdd_24
	beq			EndLoop_24
	
SixChannel_24:	
	ldr			r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]!	@//channel 0	
	ldr			r3, [r5, #PerChannelInfo_size]!				@//channel 1
	ldr			r6, [r5, #PerChannelInfo_size]!				@//channel 2	
	ldr			r7, [r5, #PerChannelInfo_size]!				@//channel 3
	ldr			r8, [r5, #PerChannelInfo_size]!				@//channel 4
	mov			r9, r2, LSR #2								@ r9 = cSamples/4 
	ldr			r12, [r5, #PerChannelInfo_size]!			@//channel 5
LoopSixChannel_24:
	vld1.32		{D0[0]}, [r0]!		@//0
	vld1.32		{D0[1]}, [r3]!		@//1
	vld1.32		{D2[0]}, [r6]!		@//2
	vld1.32		{D2[1]}, [r7]!		@//3
	vld1.32		{D4[0]}, [r8]!		@//4
	vld1.32		{D4[1]}, [r12]!		@//5
	vld1.32		{D6[0]}, [r0]!		@//0
	vld1.32		{D6[1]}, [r3]!		@//1
	vld1.32		{D1[0]}, [r6]!		@//2
	vld1.32		{D1[1]}, [r7]!		@//3
	vld1.32		{D3[0]}, [r8]!		@//4
	vshr.S32	Q0, Q0, #8
	vqmovn.S32	D12, Q0
	vld1.32		{D3[1]}, [r12]!		@//5
	vld1.32		{D5[0]}, [r0]!		@//0
	vshr.S32	Q1, Q1, #8
	vqmovn.S32	D13, Q1
	vld1.32		{D5[1]}, [r3]!		@//1
	vld1.32		{D7[0]}, [r6]!		@//2
	vshr.S32	Q2, Q2, #8
	vqmovn.S32	D14, Q2
	vld1.32		{D7[1]}, [r7]!		@//3	
	vshr.S32	Q3, Q3, #8
	vqmovn.S32	D15, Q3
	vld1.32		{D0[0]}, [r8]!		@//4->0
	vld1.32		{D0[1]}, [r0]!		@//0->2
	vld1.32		{D1[0]}, [r6]!		@//2->4	
	vst4.32		{D12, D13, D14, D15}, [r1]!		
	vld1.32		{D1[1]}, [r8]!		@//4->6
	vld1.S32	{D2[0]}, [r12]!		@//5->1
	vshr.S32	Q0, Q0, #8
	vqmovn.S32	D12, Q0
	vld1.32		{D2[1]}, [r3]!		@//1->3
	vld1.32		{D3[0]}, [r7]!		@//3->5
	vld1.32		{D3[1]}, [r12]!		@//5->7		
	vshr.S32	Q1, Q1, #8
	vqmovn.S32	D13, Q1
	subs		r9, r9, #1
	vst2.16		{D12, D13}, [r1]!			@29 stalls
	bne			LoopSixChannel_24
	ands		r9, r2, #3
	beq			EndLoop_24	
LoopSixOdd_24:
	vld1.32		{D0[0]}, [r0]!		@//0
	vld1.32		{D0[1]}, [r3]!		@//1
	vld1.32		{D1[0]}, [r6]!		@//2
	vld1.32		{D1[1]}, [r7]!		@//3
	vld1.32		{D2[0]}, [r8]!		@//4
	vshr.S32	Q0, Q0, #8
	vqmovn.S32	D12, Q0
	vld1.32		{D2[1]}, [r12]!		@//5	
	vshr.S32	Q1, Q1, #8	
	vqmovn.S32	D13, Q1		
	vst1.32		D12, [r1]!
	subs		r9, r9, #1
	vst1.32		{D13[0]}, [r1]!
	bne			LoopSixOdd_24
	beq			EndLoop_24	
		
MultiNO6_8_24:		
	mov		r7, r4
	mov		r8, r5
LoopMultiNO6_8_24:
	ldr		r0, [r8, #PerChannelInfo_m_rgiPCMBuffer]
	add		r8, r8, #PerChannelInfo_size
	ldr		r6, [r0, r3, LSL #2]
	ssat	r6, #16, r6, ASR #8	
	strh	r6, [r1], #2
	subs	r7, r7, #1
	bne		LoopMultiNO6_8_24
	add		r3, r3, #1
	cmp		r3, r2
	blt		MultiNO6_8_24
		
EndLoop_24:
	mov r0, #0
  ldmfd sp!, {r4 - r9, PC} @auReconSample24_ARM
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
	
	ldr		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer@
	ldr		r12, [r5, #next_PerChannelInfo_m_rgiPCMBuffer]
	mov		lr, r2, lsr #1										@lr = cSamples/2
LoopTwoChannel_24_1:		
	ldrd	r4, [r0], #8										@ iPCMData = (PCMSAMPLE)pcfPCMBuf[i]@
	ldrd	r6, [r12], #8
	subs	lr, lr, #1
	ssat	r4, #24, r4
	ssat	r6, #24, r6
	ssat	r5, #24, r5	
	ssat	r7, #24, r7	
	mov		r4, r4, LSL #8
	mov		r6, r6, LSL #8
	mov		r5, r5, LSL #8
	mov		r7, r7, LSL #8
	mov		r4, r4, LSR #8	
	orr		r4, r4, r6, LSL #16
	orr		r7, r7, r5, LSR #24
	mov		r5, r5, LSL #8
	orr		r5, r5, r6, LSR #16
	strd	r4, [r1], #8
	str		r7, [r1], #4	
	bne		LoopTwoChannel_24_1
	ands	r3, r2, #1
	beq		EndLoop_24_1
	ldr		r4, [r0], #4	
	ldr		r6, [r12], #4
	ssat	r4, #24, r4
	ssat	r6, #24, r6
	mov		r4, r4, LSL #8
	mov		r4, r4, LSR #8
	orr		r4, r4, r6, LSL #24
	mov		r6, r6, LSR #8
	str		r4, [r1], #4
	strh	r6, [r1], #2
	b		EndLoop_24_1
	
OneChannel_24_1:	
	ldr		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer@
	mov		lr, r2, lsr #2										@lr = cSamples/4	
LoopOneChannel_24_1:	
	ldrd	r4, [r0], #8										@ iPCMData = (PCMSAMPLE)pcfPCMBuf[i]@	
	ldrd	r6, [r0], #8
	subs	lr, lr, #1	
	ssat	r4, #24, r4	
	ssat	r5, #24, r5
	ssat	r6, #24, r6
	ssat	r7, #24, r7
	mov		r4, r4, LSL #8
	mov		r5, r5, LSL #8
	mov		r6, r6, LSL #8
	mov		r7, r7, LSL #8
	mov		r4, r4, LSR #8
	orr		r4, r4, r5, LSL #16
	orr		r7, r7, r6, LSR #24
	mov		r5, r5, LSR #16
	orr		r5, r5, r6, LSL #8
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
	orr		r5, r6, r4, LSR #8
	strb	r5, [r1], #1
	orr		r5, r6, r4, LSR #16
	strb	r5, [r1], #1
	bne		LoopOdd_24_1
	beq		EndLoop_24_1
	
MultiChannel_24_1:
	mov		r3, #0
LoopMultiChannel_24_1:
	mov		r7, r4, lsr #1										@r7 = channel/2
	mov		r8, r5
gInLoop24_1:
	ldr		r0, [r8, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer@
	ldr		r12, [r8, #next_PerChannelInfo_m_rgiPCMBuffer]
	add		r8, r8, #double_PerChannelInfo_size
	ldr		r6, [r0, r3, LSL #2]
	ldr		lr, [r12, r3, LSL #2]
	subs	r7, r7, #1
	ssat	r6, #24, r6	
	ssat	lr, #24, lr
	mov		r6, r6, LSL #8
	mov		r6, r6, LSR #8
	orr		r6, r6, lr, LSL #24
	mov		lr, lr, LSR #8	
	str		r6, [r1], #4
	strh	lr, [r1], #2	
	bne		gInLoop24_1
	add		r3, r3, #1
	cmp		r3, r2
	blt		LoopMultiChannel_24_1
			
EndLoop_24_1:
	mov r0, #0
  ldmfd sp!, {r4 - r8, PC} @auReconSample24_ARM
  @ENTRY_END auReconSample24_ARM

	.endif	@DEVICEPCMOUT16
	.endif 	@//WMA_OPT_AURECON_ARM
  .endif	@//ARMVERSION
  	
  	
