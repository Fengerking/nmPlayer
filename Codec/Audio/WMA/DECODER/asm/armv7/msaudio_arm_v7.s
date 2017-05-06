;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;// Filename msaudio_arm_v6.s
;
;// Copyright (c) VisualOn SoftWare Co., Ltd. All rights reserved.
;
;//*@@@---@@@@******************************************************************
;//
;// Abstract:
;// 
;//     ARM Arch-7 specific multiplications
;//
;//      Custom build with 
;//          armasm -cpu arm1136 $(InputPath) "$(IntDir)/$(InputName).obj"
;//      and
;//          $(OutDir)\$(InputName).obj
;// 
;// Author:
;// 
;//     Witten Wen (Shanghai, China) September 15, 2008
;//
;// Revision History:
;//
;;//*************************************************************************


  OPT         2       ; disable listing 
  INCLUDE     kxarm.h
  INCLUDE     wma_member_arm.inc
  INCLUDE	wma_arm_version.h
  OPT         1       ; enable listing	
 
  AREA    |.text|, CODE, READONLY
  
	IF	ARMVERSION	>= 7
  IF WMA_OPT_AURECON_ARM = 1
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  EXPORT  auReconSample16_ARM
  EXPORT  auReconSample24_ARM
  
;******************************
; Neon register 
;******************************
A0			DN	D0
A1			DN	D1
A2			DN	D2
A3			DN	D3
A4			DN	D4
A5			DN	D5
A6			DN	D6
A7			DN	D7
A8			DN	D8
A9			DN	D9
A10			DN	D10
A11			DN	D11
A12			DN	D16
A13			DN	D17
A14			DN	D18
A15			DN	D19

B0			QN	Q0
B1			QN	Q1
B2			QN	Q2
B3			QN	Q3
B4			QN	Q4
B5			QN	Q5
B6			QN	Q8
B7			QN	Q9

C0			DN	D12
C1			DN	D13
C2			DN	D14
C3			DN	D15
C4			DN	D20
C5			DN	D21
C6			DN	D22
C7			DN	D23

C0_0		SN	S24
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Constants for auReconSample16_ARM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

next_PerChannelInfo_m_rgiPCMBuffer	EQU	PerChannelInfo_size+PerChannelInfo_m_rgiPCMBuffer
next2_PerChannelInfo_m_rgiPCMBuffer	EQU	2*PerChannelInfo_size+PerChannelInfo_m_rgiPCMBuffer
double_PerChannelInfo_size			EQU 2*PerChannelInfo_size
;//*************************************************************************************
;//
;// WMARESULT auReconSample16_ARM(CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples)
;//
;//*************************************************************************************

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  AREA    |.text|, CODE
  LEAF_ENTRY auReconSample16_ARM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Input parameters
; r0 = pau
; r1 = piOutput
; r2 = cSamples

; r4 = pau->m_cChannel
; r5 = pau->m_rgpcinfo

	STMFD sp!, {r4 - r9, lr}

	CMP		r2, #0
	BEQ		EndLoop
	
	LDRH  r4, [r0, #CAudioObject_m_cChannel]
	LDR   r5, [r0, #CAudioObject_m_rgpcinfo]

	CMP		r4, #2
	BLT		OneChannel
	BGT		MultiChannel
	
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			; pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer;
	LDR		r12, [r5, #next_PerChannelInfo_m_rgiPCMBuffer]
	MOV		lr, r2, lsr #3							; lr = cSamples/8			
LoopTwoChannel			
	VLD1.32			{A0, A1, A2, A3}, [r0]!
	VQMOVN.S32		C0, B0	
	VLD1.32			{A4, A5, A6, A7}, [r12]!	
	VQMOVN.S32		C1, B1	
	VQMOVN.S32		C2, B2
	VQMOVN.S32		C3, B3	
	SUBS			lr, lr, #1
	VST2.16		{C0, C1, C2, C3}, [r1]!			;9 stalls
	BNE				LoopTwoChannel
	ANDS			r3, r2, #7
	BEQ			EndLoop
LoopTwoOdd
	VLD1.32		{A0[0]}, [r0@32]!
	VLD1.32		{A0[1]}, [r12@32]!
	VQMOVN.S32		C0, B0
	SUBS		r3, r3, #1
	VSTR		C0_0, [r1]
	ADD			r1, r1, #4
	
	BNE		LoopTwoOdd
	BEQ		EndLoop
	
OneChannel	
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			; pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer;
	MOV		lr, r2, lsr #3							;lr = cSamples/8 
	
LoopOneChannel
	VLD2.32		{A0, A1, A2, A3}, [r0]!
	VQMOVN.S32		C0, B0
	VQMOVN.S32		C1, B1
	SUBS			lr, lr, #1
	VST2.16		{C0, C1}, [r1]!		
	BNE				LoopOneChannel
	ANDS			r3, r2, #7
	BEQ				EndLoop	
LoopOneOdd
	LDR			r2, [r0], #4
	SUBS		r3, r3, #1
	SSAT		r2, #16, r4	
	STRH		r2, [r1], #2
	BNE			LoopOneOdd
	BEQ			EndLoop
	
MultiChannel
	CMP			r4, #6
	BEQ			SixChannel
	CMP			r4, #8
	MOV			r3, #0
	BNE			MultiNO6_8
	LDR			r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]!	;//channel 0	
	LDR			r3, [r5, #PerChannelInfo_size]!				;//channel 1
	LDR			r6, [r5, #PerChannelInfo_size]!				;//channel 2	
	LDR			r7, [r5, #PerChannelInfo_size]!				;//channel 3
	LDR			r8, [r5, #PerChannelInfo_size]!				;//channel 4
	LDR			r12, [r5, #PerChannelInfo_size]!			;//channel 5	
	LDR			r4, [r5, #PerChannelInfo_size]!				;//channel 6
	MOV			r9, r2, LSR #2								; r9 = cSamples/4 
	LDR			lr, [r5, #PerChannelInfo_size]				;//channel 7
	
LoopEightChannel
	VLD4.32		{A0[0], A1[0], A2[0], A3[0]}, [r0]!		;//0	
	VLD4.32		{A0[1], A1[1], A2[1], A3[1]}, [r3]!		;//1	
	VLD4.32		{A4[0], A5[0], A6[0], A7[0]}, [r6]!		;//2
	VQMOVN.S32		C0, B0
	VLD4.32		{A4[1], A5[1], A6[1], A7[1]}, [r7]!		;//3	
	VQMOVN.S32		C4, B1
	VLD4.32		{A8[0], A9[0], A10[0], A11[0]}, [r8]!	;//4
	VQMOVN.S32		C1, B2
	VLD4.32		{A8[1], A9[1], A10[1], A11[1]}, [r12]!	;//5
	VQMOVN.S32		C5, B3
	VLD4.32		{A12[0], A13[0], A14[0], A15[0]}, [r4]!	;//6
	VQMOVN.S32		C2, B4
	VLD4.32		{A12[1], A13[1], A14[1], A15[1]}, [lr]!	;//7		
	VQMOVN.S32		C3, B6	
	VQMOVN.S32		C6, B5
	VQMOVN.S32		C7, B7	
	VST4.32			{C0, C1, C2, C3}, [r1]!			
	SUBS			r9, r9, #1
	VST4.32			{C4, C5, C6, C7}, [r1]!					;13 stalls
	BNE				LoopEightChannel
	ANDS			r9, r2, #3
	BEQ				EndLoop
LoopEightOdd
	VLD1.32		{A0[0]}, [r0@32]!		;//0
	VLD1.32		{A0[1]}, [r6@32]!		;//2
	VLD1.32		{A1[0]}, [r8@32]!		;//4
	VLD1.32		{A1[1]}, [r4@32]!		;//6
	VLD1.32		{A2[0]}, [r3@32]!		;//1
	VQMOVN.S32		C0, B0
	VLD1.32		{A2[1]}, [r7@32]!		;//3
	VLD1.32		{A3[0]}, [r12@32]!		;//5
	VLD1.32		{A3[1]}, [lr@32]!		;//7	
	VQMOVN.S32		C1, B1
	SUBS		r9, r9, #1
	VST2.16		{C0, C1}, [r1]!
	BNE			LoopEightOdd
	BEQ			EndLoop
	
SixChannel	
	LDR			r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]!	;//channel 0	
	LDR			r3, [r5, #PerChannelInfo_size]!				;//channel 1
	LDR			r6, [r5, #PerChannelInfo_size]!				;//channel 2	
	LDR			r7, [r5, #PerChannelInfo_size]!				;//channel 3
	LDR			r8, [r5, #PerChannelInfo_size]!				;//channel 4
	MOV			r9, r2, LSR #2								; r9 = cSamples/4 
	LDR			r12, [r5, #PerChannelInfo_size]!			;//channel 5
LoopSixChannel
	VLD1.32		{A0[0]}, [r0@32]!		;//0
	VLD1.32		{A0[1]}, [r3@32]!		;//1
	VLD1.32		{A2[0]}, [r6@32]!		;//2
	VLD1.32		{A2[1]}, [r7@32]!		;//3
	VLD1.32		{A4[0]}, [r8@32]!		;//4
	VLD1.32		{A4[1]}, [r12@32]!		;//5
	VLD1.32		{A6[0]}, [r0@32]!		;//0
	VLD1.32		{A6[1]}, [r3@32]!		;//1
	VLD1.32		{A1[0]}, [r6@32]!		;//2
	VLD1.32		{A1[1]}, [r7@32]!		;//3
	VLD1.32		{A3[0]}, [r8@32]!		;//4
	VQMOVN.S32		C0, B0
	VLD1.32		{A3[1]}, [r12@32]!		;//5
	VLD1.32		{A5[0]}, [r0@32]!		;//0
	VQMOVN.S32		C1, B1
	VLD1.32		{A5[1]}, [r3@32]!		;//1
	VLD1.32		{A7[0]}, [r6@32]!		;//2
	VQMOVN.S32		C2, B2
	VLD1.32		{A7[1]}, [r7@32]!		;//3	
	VQMOVN.S32		C3, B3
	VLD1.32		{A0[0]}, [r8@32]!		;//4->0
	VLD1.32		{A0[1]}, [r0@32]!		;//0->2
	VLD1.32		{A1[0]}, [r6@32]!		;//2->4	
	VST4.32			{C0, C1, C2, C3}, [r1]!		
	VLD1.32		{A1[1]}, [r8@32]!		;//4->6
	VLD1.S32		{A2[0]}, [r12@32]!		;//5->1
	VQMOVN.S32		C0, B0
	VLD1.32		{A2[1]}, [r3@32]!		;//1->3
	VLD1.32		{A3[0]}, [r7@32]!		;//3->5
	VLD1.32		{A3[1]}, [r12@32]!		;//5->7		
	VQMOVN.S32		C1, B1
	SUBS			r9, r9, #1
	VST2.16		{C0, C1}, [r1]!			;29 stalls
	BNE				LoopSixChannel
	ANDS			r9, r2, #3
	BEQ				EndLoop	
LoopSixOdd
	VLD1.32		{A0[0]}, [r0@32]!		;//0
	VLD1.32		{A0[1]}, [r3@32]!		;//1
	VLD1.32		{A1[0]}, [r6@32]!		;//2
	VLD1.32		{A1[1]}, [r7@32]!		;//3
	VLD1.32		{A2[0]}, [r8@32]!		;//4
	VQMOVN.S32		C0, B0
	VLD1.32		{A2[1]}, [r12@32]!		;//5		
	VQMOVN.S32		C1, B1		
	VST1.32		C0, [r1@64]!
	SUBS		r9, r9, #1
	VST1.32		{C1[0]}, [r1@32]!
	BNE			LoopSixOdd
	BEQ			EndLoop
	
MultiNO6_8
	MOV		r7, r4
	MOV		r8, r5
LoopMultiNO6_8
	LDR		r0, [r8, #PerChannelInfo_m_rgiPCMBuffer]
	ADD		r8, r8, #PerChannelInfo_size
	LDR		r6, [r0, r3, LSL #2]
	SSAT	r6, #16, r6	
	STRH	r6, [r1], #2
	SUBS	r7, r7, #1
	BNE		LoopMultiNO6_8
	ADD		r3, r3, #1
	CMP		r3, r2
	BLT		MultiNO6_8
	
EndLoop
	MOV r0, #0
	LDMFD sp!, {r4 - r9, PC} ;auReconSample16_ARM
	
	ENTRY_END auReconSample16_ARM
	
;//*************************************************************************************
;//
;// WMARESULT auReconSample24_ARM(CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples)
;//
;//*************************************************************************************

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  AREA    |.text|, CODE
  LEAF_ENTRY auReconSample24_ARM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	IF		DEVICEPCMOUT16	=	1
; Input parameters
; r0 = pau
; r1 = piOutput
; r2 = cSamples

; r4 = pau->m_cChannel
; r5 = pau->m_rgpcinfo

	STMFD	sp!, {r4 - r9, lr}
	CMP		r2, #0
	BEQ		EndLoop_24
	LDRH	r4, [r0, #CAudioObject_m_cChannel]
	LDR		r5, [r0, #CAudioObject_m_rgpcinfo]
	
	CMP		r4, #2
	BLT		OneChannel_24
	BGT		MultiChannel_24
	
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			; pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer;
	LDR		r12, [r5, #next_PerChannelInfo_m_rgiPCMBuffer]
	MOV		lr, r2, lsr #3										;lr = cSamples/8
LoopTwoChannel_24	
	VLD1.32			{A0, A1, A2, A3}, [r0]!
	VSHR.S32		B0, B0, #8
	VQMOVN.S32		C0, B0	
	VLD1.32			{A4, A5, A6, A7}, [r12]!	
	VSHR.S32		B1, B1, #8
	VQMOVN.S32		C1, B1	
	VSHR.S32		B2, B2, #8
	VQMOVN.S32		C2, B2
	VSHR.S32		B3, B3, #8
	VQMOVN.S32		C3, B3	
	SUBS			lr, lr, #1
	VST2.16			{C0, C1, C2, C3}, [r1]!			;9 stalls
	BNE				LoopTwoChannel_24
	ANDS			r3, r2, #7
	BEQ				EndLoop_24
LoopTwoOdd_24
	VLD1.32			{A0[0]}, [r0@32]!
	VLD1.32			{A0[1]}, [r12@32]!
	VSHR.S32			B0, B0, #8
	VQMOVN.S32		C0, B0
	SUBS			r3, r3, #1
	VSTR			C0_0, [r1]
	ADD				r1, r1, #4
	BNE				LoopTwoOdd_24
	BEQ				EndLoop_24		
	
OneChannel_24	
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			; pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer;
	MOV		lr, r2, lsr #3										;lr = cSamples/8	
LoopOneChannel_24	
	VLD2.32		{A0, A1, A2, A3}, [r0]!
	VSHR.S32	B0, B0, #8
	VSHR.S32	B1, B1, #8
	VQMOVN.S32	C0, B0
	VQMOVN.S32	C1, B1
	SUBS		lr, lr, #1
	VST2.16		{C0, C1}, [r1]!		
	BNE			LoopOneChannel_24
	ANDS		r3, r2, #7
	BEQ			EndLoop_24	
LoopOneOdd_24
	LDR			r2, [r0], #4
	SUBS		r3, r3, #1
	SSAT		r2, #16, r4, ASR #8
	STRH		r2, [r1], #2
	BNE			LoopOneOdd_24
	BEQ			EndLoop_24
	
MultiChannel_24
	CMP			r4, #6
	BEQ			SixChannel_24
	CMP			r4, #8
	MOV			r3, #0										;//for orthers, r3 is sample number.
	BNE			MultiNO6_8_24
	LDR			r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]!	;//channel 0	
	LDR			r3, [r5, #PerChannelInfo_size]!				;//channel 1
	LDR			r6, [r5, #PerChannelInfo_size]!				;//channel 2	
	LDR			r7, [r5, #PerChannelInfo_size]!				;//channel 3
	LDR			r8, [r5, #PerChannelInfo_size]!				;//channel 4
	LDR			r12, [r5, #PerChannelInfo_size]!			;//channel 5	
	LDR			r4, [r5, #PerChannelInfo_size]!				;//channel 6
	MOV			r9, r2, LSR #2								; r9 = cSamples/2 
	LDR			lr, [r5, #PerChannelInfo_size]				;//channel 7
	
LoopEightChannel_24
	VLD4.32		{A0[0], A1[0], A2[0], A3[0]}, [r0]!		;//0	
	VLD4.32		{A0[1], A1[1], A2[1], A3[1]}, [r3]!		;//1	
	VLD4.32		{A4[0], A5[0], A6[0], A7[0]}, [r6]!		;//2
	VSHR.S32	B0, B0, #8
	VQMOVN.S32	C0, B0
	VLD4.32		{A4[1], A5[1], A6[1], A7[1]}, [r7]!		;//3
	VSHR.S32	B1, B1, #8	
	VQMOVN.S32	C4, B1
	VLD4.32		{A8[0], A9[0], A10[0], A11[0]}, [r8]!	;//4
	VSHR.S32	B2, B2, #8
	VQMOVN.S32	C1, B2
	VLD4.32		{A8[1], A9[1], A10[1], A11[1]}, [r12]!	;//5
	VSHR.S32	B3, B3, #8
	VQMOVN.S32	C5, B3
	VLD4.32		{A12[0], A13[0], A14[0], A15[0]}, [r4]!	;//6
	VSHR.S32	B4, B4, #8
	VQMOVN.S32	C2, B4
	VLD4.32		{A12[1], A13[1], A14[1], A15[1]}, [lr]!	;//7
	VSHR.S32	B6, B6, #8		
	VQMOVN.S32	C3, B6	
	VSHR.S32	B5, B5, #8		
	VQMOVN.S32	C6, B5
	VSHR.S32	B7, B7, #8		
	VQMOVN.S32	C7, B7	
	VST4.32		{C0, C1, C2, C3}, [r1]!			
	SUBS		r9, r9, #1
	VST4.32		{C4, C5, C6, C7}, [r1]!					;13 stalls
	BNE			LoopEightChannel_24
	ANDS		r9, r2, #3
	BEQ			EndLoop_24
LoopEightOdd_24
	VLD1.32		{A0[0]}, [r0@32]!		;//0
	VLD1.32		{A0[1]}, [r6@32]!		;//2
	VLD1.32		{A1[0]}, [r8@32]!		;//4
	VLD1.32		{A1[1]}, [r4@32]!		;//6
	VLD1.32		{A2[0]}, [r3@32]!		;//1
	VSHR.S32	B0, B0, #8
	VQMOVN.S32	C0, B0
	VLD1.32		{A2[1]}, [r7@32]!		;//3
	VLD1.32		{A3[0]}, [r12@32]!		;//5
	VLD1.32		{A3[1]}, [lr@32]!		;//7
	VSHR.S32	B1, B1, #8	
	VQMOVN.S32	C1, B1
	SUBS		r9, r9, #1
	VST2.16		{C0, C1}, [r1]!
	BNE			LoopEightOdd_24
	BEQ			EndLoop_24
	
SixChannel_24	
	LDR			r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]!	;//channel 0	
	LDR			r3, [r5, #PerChannelInfo_size]!				;//channel 1
	LDR			r6, [r5, #PerChannelInfo_size]!				;//channel 2	
	LDR			r7, [r5, #PerChannelInfo_size]!				;//channel 3
	LDR			r8, [r5, #PerChannelInfo_size]!				;//channel 4
	MOV			r9, r2, LSR #2								; r9 = cSamples/4 
	LDR			r12, [r5, #PerChannelInfo_size]!			;//channel 5
LoopSixChannel_24
	VLD1.32		{A0[0]}, [r0@32]!		;//0
	VLD1.32		{A0[1]}, [r3@32]!		;//1
	VLD1.32		{A2[0]}, [r6@32]!		;//2
	VLD1.32		{A2[1]}, [r7@32]!		;//3
	VLD1.32		{A4[0]}, [r8@32]!		;//4
	VLD1.32		{A4[1]}, [r12@32]!		;//5
	VLD1.32		{A6[0]}, [r0@32]!		;//0
	VLD1.32		{A6[1]}, [r3@32]!		;//1
	VLD1.32		{A1[0]}, [r6@32]!		;//2
	VLD1.32		{A1[1]}, [r7@32]!		;//3
	VLD1.32		{A3[0]}, [r8@32]!		;//4
	VSHR.S32	B0, B0, #8
	VQMOVN.S32	C0, B0
	VLD1.32		{A3[1]}, [r12@32]!		;//5
	VLD1.32		{A5[0]}, [r0@32]!		;//0
	VSHR.S32	B1, B1, #8
	VQMOVN.S32	C1, B1
	VLD1.32		{A5[1]}, [r3@32]!		;//1
	VLD1.32		{A7[0]}, [r6@32]!		;//2
	VSHR.S32	B2, B2, #8
	VQMOVN.S32	C2, B2
	VLD1.32		{A7[1]}, [r7@32]!		;//3	
	VSHR.S32	B3, B3, #8
	VQMOVN.S32	C3, B3
	VLD1.32		{A0[0]}, [r8@32]!		;//4->0
	VLD1.32		{A0[1]}, [r0@32]!		;//0->2
	VLD1.32		{A1[0]}, [r6@32]!		;//2->4	
	VST4.32		{C0, C1, C2, C3}, [r1]!		
	VLD1.32		{A1[1]}, [r8@32]!		;//4->6
	VLD1.S32	{A2[0]}, [r12@32]!		;//5->1
	VSHR.S32	B0, B0, #8
	VQMOVN.S32	C0, B0
	VLD1.32		{A2[1]}, [r3@32]!		;//1->3
	VLD1.32		{A3[0]}, [r7@32]!		;//3->5
	VLD1.32		{A3[1]}, [r12@32]!		;//5->7		
	VSHR.S32	B1, B1, #8
	VQMOVN.S32	C1, B1
	SUBS		r9, r9, #1
	VST2.16		{C0, C1}, [r1]!			;29 stalls
	BNE			LoopSixChannel_24
	ANDS		r9, r2, #3
	BEQ			EndLoop_24	
LoopSixOdd_24
	VLD1.32		{A0[0]}, [r0@32]!		;//0
	VLD1.32		{A0[1]}, [r3@32]!		;//1
	VLD1.32		{A1[0]}, [r6@32]!		;//2
	VLD1.32		{A1[1]}, [r7@32]!		;//3
	VLD1.32		{A2[0]}, [r8@32]!		;//4
	VSHR.S32	B0, B0, #8
	VQMOVN.S32	C0, B0
	VLD1.32		{A2[1]}, [r12@32]!		;//5	
	VSHR.S32	B1, B1, #8	
	VQMOVN.S32	C1, B1		
	VST1.32		C0, [r1@64]!
	SUBS		r9, r9, #1
	VST1.32		{C1[0]}, [r1@32]!
	BNE			LoopSixOdd_24
	BEQ			EndLoop_24	
		
MultiNO6_8_24		
	MOV		r7, r4
	MOV		r8, r5
LoopMultiNO6_8_24
	LDR		r0, [r8, #PerChannelInfo_m_rgiPCMBuffer]
	ADD		r8, r8, #PerChannelInfo_size
	LDR		r6, [r0, r3, LSL #2]
	SSAT	r6, #16, r6, ASR #8	
	STRH	r6, [r1], #2
	SUBS	r7, r7, #1
	BNE		LoopMultiNO6_8_24
	ADD		r3, r3, #1
	CMP		r3, r2
	BLT		MultiNO6_8_24
		
EndLoop_24
	MOV r0, #0
  	LDMFD sp!, {r4 - r9, PC} ;auReconSample24_ARM
  	ENTRY_END auReconSample24_ARM
  	
	ELSE	;DEVICEPCMOUT16
	
; Input parameters
; r0 = pau
; r1 = piOutput
; r2 = cSamples

; r4 = pau->m_cChannel
; r5 = pau->m_rgpcinfo


	STMFD sp!, {r4 - r8, lr}
	CMP		r2, #0
	BEQ		EndLoop_24
	LDRH  r4, [r0, #CAudioObject_m_cChannel]
	LDR   r5, [r0, #CAudioObject_m_rgpcinfo]
  
	CMP		r4, #2
	BLT		OneChannel_24
	BGT		MultiChannel_24
	
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			; pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer;
	LDR		r12, [r5, #next_PerChannelInfo_m_rgiPCMBuffer]
	MOV		lr, r2, lsr #1										;lr = cSamples/2
LoopTwoChannel_24			
	LDRD	r4, [r0], #8										; iPCMData = (PCMSAMPLE)pcfPCMBuf[i];
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
	BNE		LoopTwoChannel_24
	ANDS	r3, r2, #1
	BEQ		EndLoop_24
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
	B		EndLoop_24
	
OneChannel_24	
	LDR		r0, [r5, #PerChannelInfo_m_rgiPCMBuffer]			; pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer;
	MOV		lr, r2, lsr #2										;lr = cSamples/4	
LoopOneChannel_24	
	LDRD	r4, [r0], #8										; iPCMData = (PCMSAMPLE)pcfPCMBuf[i];	
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
	BNE		LoopOneChannel_24
	ANDS	lr, r2, #3	
	BEQ		EndLoop_24
	MOV		r6, #0xff
LoopOdd_24															;max odd samples is 3
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
	
MultiChannel_24
	MOV		r3, #0
LoopMultiChannel_24
	MOV		r7, r4, lsr #1										;r7 = channel/2
	MOV		r8, r5
gInLoop24
	LDR		r0, [r8, #PerChannelInfo_m_rgiPCMBuffer]			; pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer;
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
	BNE		gInLoop24
	ADD		r3, r3, #1
	CMP		r3, r2
	BLT		LoopMultiChannel_24
			
EndLoop_24
	MOV r0, #0
  	LDMFD sp!, {r4 - r8, PC} ;auReconSample24_ARM
  	ENTRY_END auReconSample24_ARM

	ENDIF	;DEVICEPCMOUT16
	ENDIF 	;//WMA_OPT_AURECON_ARM
    ENDIF	;//ARMVERSION
  	END