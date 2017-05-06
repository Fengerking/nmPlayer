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
  #include "../../../../../inc/audio/v10/include/voWMADecID.h"
  .INCLUDE     "kxarm.h"
  .INCLUDE     "wma_member_arm.inc"
  .INCLUDE		"wma_arm_version.h"
@  OPT         1       @ enable listing	
 
@  AREA    |.text|, CODE, READONLY
 		.section .text
 		 
	.IF	ARMVERSION	>= 7
  .IF WMA_OPT_AURECON_ARM == 1
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


  .global  auReconSample16_ARM
  .global  auReconSample24_ARM
  
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

.equ	next_PerChannelInfo_m_rgiPCMBuffer	,	PerChannelInfo_size+PerChannelInfo_m_rgiPCMBuffer
.equ	next2_PerChannelInfo_m_rgiPCMBuffer	,	2*PerChannelInfo_size+PerChannelInfo_m_rgiPCMBuffer
.equ	double_PerChannelInfo_size			, 2*PerChannelInfo_size
@//*************************************************************************************
@//
@// WMARESULT auReconSample16_ARM(CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples)
@//
@//*************************************************************************************

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@  AREA    |.text|, CODE
  LEAF_ENTRY auReconSample16_ARM
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pau
@ r1 = piOutput
@ r2 = cSamples

@ r4 = pau->m_cChannel
@ r5 = pau->m_rgpcinfo

	STMFD           sp!, {r4 - r9, lr}

	CMP		r2, #0
	BEQ		EndLoop
	
	LDRH            r4, [r0, #CAudioObject_m_cChannel]
	LDR             r5, [r0, #CAudioObject_m_rgpcinfo]

	CMP		r4, #2
	BLT		OneChannel
	BGT		MultiChannel
	
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer@
	LDR		r12, [r5, #next_PerChannelInfo_m_rgiPCMBuffer]
	MOV		lr, r2, lsr #3							@ lr = cSamples/8			
LoopTwoChannel:			
	VLD1.32		{D0, D1, D2, D3}, [r0]!
	VQMOVN.S32	D12, Q0	
	VLD1.32		{D4, D5, D6, D7}, [r12]!	
	VQMOVN.S32	D13, Q1	
	VQMOVN.S32	D14, Q2
	VQMOVN.S32	D15, Q3	
	SUBS		lr, lr, #1
	VST2.16		{D12, D13, D14, D15}, [r1]!			@9 stalls
	BNE		LoopTwoChannel
	ANDS		r3, r2, #7
	BEQ		EndLoop
LoopTwoOdd:
	VLD1.32		{D0[0]}, [r0]!
	VLD1.32		{D0[1]}, [r12]!
	VQMOVN.S32		D12, Q0
	SUBS		r3, r3, #1
	VSTR		S24, [r1]
	ADD			r1, r1, #4
	
	BNE		LoopTwoOdd
	BEQ		EndLoop
	
OneChannel:	
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer@
	MOV		lr, r2, lsr #3							@lr = cSamples/8 
	
LoopOneChannel:
	VLD2.32		{D0, D1, D2, D3}, [r0]!
	VQMOVN.S32	D12, Q0
	VQMOVN.S32	D13, Q1
	SUBS		lr, lr, #1
	VST2.16		{D12, D13}, [r1]!		
	BNE		LoopOneChannel
	ANDS		r3, r2, #7
	BEQ		EndLoop	
LoopOneOdd:
	LDR		r2, [r0], #4
	SUBS		r3, r3, #1
	SSAT		r2, #16, r4	
	STRH		r2, [r1], #2
	BNE		LoopOneOdd
	BEQ		EndLoop
	
MultiChannel:
        CMP             r4, #3
        BEQ             MultiNo6_8_16
        CMP             r4, #4
        BEQ             MultiNo6_8_16
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]!	@//channel 0	
	LDR		r3, [r5, #PerChannelInfo_size]!				@//channel 1
	LDR		r6, [r5, #PerChannelInfo_size]!				@//channel 2	
	LDR		r7, [r5, #PerChannelInfo_size]!				@//channel 3
	LDR		r8, [r5, #PerChannelInfo_size]!				@//channel 4
	CMP		r4, #6
	LDR		r12, [r5, #PerChannelInfo_size]!			@//channel 5
	BEQ		SixChannel
	LDR		r4, [r5, #PerChannelInfo_size]!				@//channel 6
	MOV		r9, r2, LSR #2								@ r9 = cSamples/2 
	LDR		lr, [r5, #PerChannelInfo_size]				@//channel 7
	
LoopEightChannel:
	VLD4.32		{D0[0], D1[0], D2[0], D3[0]}, [r0]!		@//0	
	VLD4.32		{D0[1], D1[1], D2[1], D3[1]}, [r3]!		@//1	
	VLD4.32		{D4[0], D5[0], D6[0], D7[0]}, [r6]!		@//2
	VQMOVN.S32	D12, Q0
	VLD4.32		{D4[1], D5[1], D6[1], D7[1]}, [r7]!		@//3	
	VQMOVN.S32	D20, Q1
	VLD4.32		{D8[0], D9[0], D10[0], D11[0]}, [r8]!	@//4
	VQMOVN.S32	D13, Q2
	VLD4.32		{D8[1], D9[1], D10[1], D11[1]}, [r12]!	@//5
	VQMOVN.S32	D21, Q3
	VLD4.32		{D16[0], D17[0], D18[0], D19[0]}, [r4]!	@//6
	VQMOVN.S32	D14, Q4
	VLD4.32		{D16[1], D17[1], D18[1], D19[1]}, [lr]!	@//7		
	VQMOVN.S32	D15, Q8	
	VQMOVN.S32	D22, Q5
	VQMOVN.S32	D23, Q9	
	VST4.32		{D12, D13, D14, D15}, [r1]!			
	SUBS		r9, r9, #1
	VST4.32		{D20, D21, D22, D23}, [r1]!					@13 stalls
	BNE		LoopEightChannel
	ANDS		r9, r2, #3
	BEQ		EndLoop

LoopEightOdd:
	VLD1.32		{D0[0]}, [r0]!		@//0
	VLD1.32		{D0[1]}, [r6]!		@//2
	VLD1.32		{D1[0]}, [r8]!		@//4
	VLD1.32		{D1[1]}, [r4]!		@//6
	VLD1.32		{D2[0]}, [r3]!		@//1
	VQMOVN.S32	D12, Q0
	VLD1.32		{D2[1]}, [r7]!		@//3
	VLD1.32		{D3[0]}, [r12]!		@//5
	VLD1.32		{D3[1]}, [lr]!		@//7	
	VQMOVN.S32	D13, Q1
	SUBS		r9, r9, #1
	VST2.16		{D12, D13}, [r1]!
	BNE		LoopEightOdd
	BEQ		EndLoop
	
SixChannel:	
	MOV		r9, r2, LSR #2								@ r9 = cSamples/4 
LoopSixChannel:
	VLD1.32		{D0[0]}, [r0]!		@//0
	VLD1.32		{D0[1]}, [r3]!		@//1
	VLD1.32		{D2[0]}, [r6]!		@//2
	VLD1.32		{D2[1]}, [r7]!		@//3
	VLD1.32		{D4[0]}, [r8]!		@//4
	VLD1.32		{D4[1]}, [r12]!		@//5
	VLD1.32		{D6[0]}, [r0]!		@//0
	VLD1.32		{D6[1]}, [r3]!		@//1
	VLD1.32		{D1[0]}, [r6]!		@//2
	VLD1.32		{D1[1]}, [r7]!		@//3
	VLD1.32		{D3[0]}, [r8]!		@//4
	VQMOVN.S32	D12, Q0
	VLD1.32		{D3[1]}, [r12]!		@//5
	VLD1.32		{D5[0]}, [r0]!		@//0
	VQMOVN.S32	D13, Q1
	VLD1.32		{D5[1]}, [r3]!		@//1
	VLD1.32		{D7[0]}, [r6]!		@//2
	VQMOVN.S32	D14, Q2
	VLD1.32		{D7[1]}, [r7]!		@//3	
	VQMOVN.S32	D15, Q3
	VLD1.32		{D0[0]}, [r8]!		@//4->0
	VLD1.32		{D0[1]}, [r0]!		@//0->2
	VLD1.32		{D1[0]}, [r6]!		@//2->4	
	VST4.32		{D12, D13, D14, D15}, [r1]!		
	VLD1.32		{D1[1]}, [r8]!		@//4->6
	VLD1.S32	{D2[0]}, [r12]!		@//5->1
	VQMOVN.S32	D12, Q0
	VLD1.32		{D2[1]}, [r3]!		@//1->3
	VLD1.32		{D3[0]}, [r7]!		@//3->5
	VLD1.32		{D3[1]}, [r12]!		@//5->7		
	VQMOVN.S32	D13, Q1
	SUBS		r9, r9, #1
	VST2.16		{D12, D13}, [r1]!			@29 stalls
	BNE		LoopSixChannel
	ANDS		r9, r2, #3
	BEQ		EndLoop	
LoopSixOdd:
	VLD1.32		{D0[0]}, [r0]!		@//0
	VLD1.32		{D0[1]}, [r3]!		@//1
	VLD1.32		{D1[0]}, [r6]!		@//2
	VLD1.32		{D1[1]}, [r7]!		@//3
	VLD1.32		{D2[0]}, [r8]!		@//4
	VQMOVN.S32	D12, Q0
	VLD1.32		{D2[1]}, [r12]!		@//5		
	VQMOVN.S32	D13, Q1		
	VST1.32		D12, [r1]!
	SUBS		r9, r9, #1
	VST1.32		{D13[0]}, [r1]!
	BNE		LoopSixOdd
        BEQ             EndLoop

MultiNo6_8_16:
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
	
	SSAT	        r6, #16, r6	
	SSAT	        lr, #16, lr
	SUBS	        r7, r7, #1
	
	PKHBT	        r6, r6, lr, LSL #16
	STR		r6, [r1], #4	
	BNE		gInLoop16
	ADD		r3, r3, #1
	CMP		r3, r2
	BLT		LoopMultiChannel

	
EndLoop:
	MOV             r0, #0
	LDMFD           sp!, {r4 - r9, PC} @auReconSample16_ARM
	
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

	STMFD	        sp!, {r4 - r9, lr}
	CMP		r2, #0
	BEQ		EndLoop_24
	LDRH	        r4, [r0, #CAudioObject_m_cChannel]
	LDR		r5, [r0, #CAudioObject_m_rgpcinfo]
	
	CMP		r4, #2
	BLT		OneChannel_24
	BGT		MultiChannel_24
	
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer;
	LDR		r12, [r5, #next_PerChannelInfo_m_rgiPCMBuffer]
	MOV		lr, r2, lsr #3										@lr = cSamples/8
LoopTwoChannel_24:	
	VLD1.32		{D0, D1, D2, D3}, [r0]!
	VSHR.S32	Q0, Q0, #8
	VQMOVN.S32	D12, Q0	
	VLD1.32		{D4, D5, D6, D7}, [r12]!	
	VSHR.S32	Q1, Q1, #8
	VQMOVN.S32	D13, Q1	
	VSHR.S32	Q2, Q2, #8
	VQMOVN.S32	D14, Q2
	VSHR.S32	Q3, Q3, #8
	VQMOVN.S32	D15, Q3	
	SUBS		lr, lr, #1
	VST2.16		{D12, D13, D14, D15}, [r1]!			@9 stalls
	BNE		LoopTwoChannel_24
	ANDS		r3, r2, #7
	BEQ		EndLoop_24
LoopTwoOdd_24:
	VLD1.32		{D0[0]}, [r0]!
	VLD1.32		{D0[1]}, [r12]!
	VSHR.S32	Q0, Q0, #8
	VQMOVN.S32	D12, Q0
	SUBS		r3, r3, #1
	VSTR		S24, [r1]
	ADD		r1, r1, #4
	BNE		LoopTwoOdd_24
	BEQ		EndLoop_24		
	
OneChannel_24:	
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			@ pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer;
	MOV		lr, r2, lsr #3										@lr = cSamples/8	
LoopOneChannel_24:	
	VLD2.32		{D0, D1, D2, D3}, [r0]!
	VSHR.S32	Q0, Q0, #8
	VSHR.S32	Q1, Q1, #8
	VQMOVN.S32	D12, Q0
	VQMOVN.S32	D13, Q1
	SUBS		lr, lr, #1
	VST2.16		{D12, D13}, [r1]!		
	BNE		LoopOneChannel_24
	ANDS		r3, r2, #7
	BEQ		EndLoop_24	
LoopOneOdd_24:
	LDR		r2, [r0], #4
	SUBS		r3, r3, #1
	SSAT		r2, #16, r4, ASR #8
	STRH		r2, [r1], #2
	BNE		LoopOneOdd_24
	BEQ		EndLoop_24
	
MultiChannel_24:
	MOV		r3, #0										@//for orthers, r3 is sample number.
	CMP		r4, #6
	BEQ		MultiNO6_8_24
	CMP		r4, #8

	BNE		MultiNO6_8_24
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]!	@//channel 0	
	LDR		r3, [r5, #PerChannelInfo_size]!				@//channel 1
	LDR		r6, [r5, #PerChannelInfo_size]!				@//channel 2	
	LDR		r7, [r5, #PerChannelInfo_size]!				@//channel 3
	LDR		r8, [r5, #PerChannelInfo_size]!				@//channel 4
	LDR		r12, [r5, #PerChannelInfo_size]!			@//channel 5	
	LDR		r4, [r5, #PerChannelInfo_size]!				@//channel 6
	MOV		r9, r2, LSR #2								@ r9 = cSamples/2 
	LDR		lr, [r5, #PerChannelInfo_size]				@//channel 7
	
LoopEightChannel_24:
	VLD4.32		{D0[0], D1[0], D2[0], D3[0]}, [r0]!		@//0	
	VLD4.32		{D0[1], D1[1], D2[1], D3[1]}, [r3]!		@//1	
	VLD4.32		{D4[0], D5[0], D6[0], D7[0]}, [r6]!		@//2
	VSHR.S32	Q0, Q0, #8
	VQMOVN.S32	D12, Q0
	VLD4.32		{D4[1], D5[1], D6[1], D7[1]}, [r7]!		@//3
	VSHR.S32	Q1, Q1, #8	
	VQMOVN.S32	D20, Q1
	VLD4.32		{D8[0], D9[0], D10[0], D11[0]}, [r8]!	@//4
	VSHR.S32	Q2, Q2, #8
	VQMOVN.S32	D13, Q2
	VLD4.32		{D8[1], D9[1], D10[1], D11[1]}, [r12]!	@//5
	VSHR.S32	Q3, Q3, #8
	VQMOVN.S32	D21, Q3
	VLD4.32		{D16[0], D17[0], D18[0], D19[0]}, [r4]!	@//6
	VSHR.S32	Q4, Q4, #8
	VQMOVN.S32	D14, Q4
	VLD4.32		{D16[1], D17[1], D18[1], D19[1]}, [lr]!	@//7
	VSHR.S32	Q8, Q8, #8		
	VQMOVN.S32	D15, Q8	
	VSHR.S32	Q5, Q5, #8		
	VQMOVN.S32	D22, Q5
	VSHR.S32	Q9, Q9, #8		
	VQMOVN.S32	D23, Q9	
	VST4.32		{D12, D13, D14, D15}, [r1]!			
	SUBS		r9, r9, #1
	VST4.32		{D20, D21, D22, D23}, [r1]!					@13 stalls
	BNE		LoopEightChannel_24
	ANDS		r9, r2, #3
	BEQ		EndLoop_24
LoopEightOdd_24:
	VLD1.32		{D0[0]}, [r0]!		@//0
	VLD1.32		{D0[1]}, [r6]!		@//2
	VLD1.32		{D1[0]}, [r8]!		@//4
	VLD1.32		{D1[1]}, [r4]!		@//6
	VLD1.32		{D2[0]}, [r3]!		@//1
	VSHR.S32	Q0, Q0, #8
	VQMOVN.S32	D12, Q0
	VLD1.32		{D2[1]}, [r7]!		@//3
	VLD1.32		{D3[0]}, [r12]!		@//5
	VLD1.32		{D3[1]}, [lr]!		@//7
	VSHR.S32	Q1, Q1, #8	
	VQMOVN.S32	D13, Q1
	SUBS		r9, r9, #1
	VST2.16		{D12, D13}, [r1]!
	BNE		LoopEightOdd_24
	BEQ		EndLoop_24
	
SixChannel_24:	
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]!	@//channel 0	
	LDR		r3, [r5, #PerChannelInfo_size]!			@//channel 1
	LDR		r6, [r5, #PerChannelInfo_size]!			@//channel 2	
	LDR		r7, [r5, #PerChannelInfo_size]!			@//channel 3
	LDR		r8, [r5, #PerChannelInfo_size]!			@//channel 4
	MOV		r9, r2, LSR #2					@ r9 = cSamples/4 
	LDR		r12, [r5, #PerChannelInfo_size]!		@//channel 5
LoopSixChannel_24:
	VLD1.32		{D0[0]}, [r0]!		@//0
	VLD1.32		{D0[1]}, [r3]!		@//1
	VLD1.32		{D2[0]}, [r6]!		@//2
	VLD1.32		{D2[1]}, [r7]!		@//3
	VLD1.32		{D4[0]}, [r8]!		@//4
	VLD1.32		{D4[1]}, [r12]!		@//5
	VLD1.32		{D6[0]}, [r0]!		@//0
	VLD1.32		{D6[1]}, [r3]!		@//1
	VLD1.32		{D1[0]}, [r6]!		@//2
	VLD1.32		{D1[1]}, [r7]!		@//3
	VLD1.32		{D3[0]}, [r8]!		@//4
	VSHR.S32	Q0, Q0, #8
	VQMOVN.S32	D12, Q0
	VLD1.32		{D3[1]}, [r12]!		@//5
	VLD1.32		{D5[0]}, [r0]!		@//0
	VSHR.S32	Q1, Q1, #8
	VQMOVN.S32	D13, Q1
	VLD1.32		{D5[1]}, [r3]!		@//1
	VLD1.32		{D7[0]}, [r6]!		@//2
	VSHR.S32	Q2, Q2, #8
	VQMOVN.S32	D14, Q2
	VLD1.32		{D7[1]}, [r7]!		@//3	
	VSHR.S32	Q3, Q3, #8
	VQMOVN.S32	D15, Q3
	VLD1.32		{D0[0]}, [r8]!		@//4->0
	VLD1.32		{D0[1]}, [r0]!		@//0->2
	VLD1.32		{D1[0]}, [r6]!		@//2->4	
	VST4.32		{D12, D13, D14, D15}, [r1]!		
	VLD1.32		{D1[1]}, [r8]!		@//4->6
	VLD1.S32	{D2[0]}, [r12]!		@//5->1
	VSHR.S32	Q0, Q0, #8
	VQMOVN.S32	D12, Q0
	VLD1.32		{D2[1]}, [r3]!		@//1->3
	VLD1.32		{D3[0]}, [r7]!		@//3->5
	VLD1.32		{D3[1]}, [r12]!		@//5->7		
	VSHR.S32	Q1, Q1, #8
	VQMOVN.S32	D13, Q1
	SUBS		r9, r9, #1
	VST2.16		{D12, D13}, [r1]!			@29 stalls
	BNE		LoopSixChannel_24
	ANDS		r9, r2, #3
	BEQ		EndLoop_24	
LoopSixOdd_24:
	VLD1.32		{D0[0]}, [r0]!		@//0
	VLD1.32		{D0[1]}, [r3]!		@//1
	VLD1.32		{D1[0]}, [r6]!		@//2
	VLD1.32		{D1[1]}, [r7]!		@//3
	VLD1.32		{D2[0]}, [r8]!		@//4
	VSHR.S32	Q0, Q0, #8
	VQMOVN.S32	D12, Q0
	VLD1.32		{D2[1]}, [r12]!		@//5	
	VSHR.S32	Q1, Q1, #8	
	VQMOVN.S32	D13, Q1		
	VST1.32		D12, [r1]!
	SUBS		r9, r9, #1
	VST1.32		{D13[0]}, [r1]!
	BNE		LoopSixOdd_24
	BEQ		EndLoop_24	
		
MultiNO6_8_24:		
	MOV		r7, r4
	MOV		r8, r5
LoopMultiNO6_8_24:
	LDR		r0, [r8, #PerChannelInfo_m_rgiPCMBuffer]
	ADD		r8, r8, #PerChannelInfo_size
	LDR		r6, [r0, r3, LSL #2]
	SSAT		r6, #16, r6, ASR #8	
	STRH		r6, [r1], #2
	SUBS		r7, r7, #1
	BNE		LoopMultiNO6_8_24
	ADD		r3, r3, #1
	CMP		r3, r2
	BLT		MultiNO6_8_24
		
EndLoop_24:
	MOV             r0, #0
  	LDMFD           sp!, {r4 - r9, PC} @auReconSample24_ARM
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
	BNE		LoopOdd_24_1
	BEQ		EndLoop_24_1
	
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
	.ENDIF 	@//WMA_OPT_AURECON_ARM
    .ENDIF	@//ARMVERSION
  	.END
  	
  	
