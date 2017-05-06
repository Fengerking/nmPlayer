@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************
@// Module Name:
@//
@//     losslessdeclsl_arm.s
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
@//     Jerry He (yamihe) Sep 30th, 2003
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

  .if WMA_OPT_LOSSLESSDECLSL_ARM == 1

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  .extern  ibstrmGetBits
  .extern  ibstrmLookForBits
  .extern  ibstrmCountSerial1Bits
  .extern  prvDecoderCDLMSPredictorHelper

  .global  prvDecodeSubFrameChannelResiduePureLosslessModeVerB
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.if LINUX_RVDS == 1
  	PRESERVE8
	.endif
  	LEAF_ENTRY prvDecodeSubFrameChannelResiduePureLosslessModeVerB
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@stack definition
.equ ST_uiOffset1             , 16
.equ ST_iShift1               , 12
.equ ST_iShift0               , 8
.equ ST_iResult1              , 4
.equ ST_iResult               , 0

.equ ST_SIZE                  , 20


@ Input parameters
@ r0 = pau
@ r1 = paudec
@ r2 = ppcinfo

  STMFD sp!, {r4 - r12, lr}

  SUB   sp, sp, #ST_SIZE  @setup stack

@ r4 = rgiLPCResidue
@ r5 = iLen

@ r9 = pau
@ r10= paudec
@ r11= ppcinfo


  MOV   r11, r2
  MOV   r10, r1
  MOV   r9,  r0
  
  LDR   r3, [r11, #PerChannelInfo_m_rgsubfrmconfig]
  LDR   r2, [r9, #CAudioObject_m_cFrameSampleHalf]
  LDR   r0, [r3, #SubFrameConfigInfo_m_rgiSubFrameStart]

@ iLen = ppcinfo->m_cSubFrameSampleHalf@
  LDRSH r5, [r11, #PerChannelInfo_m_cSubFrameSampleHalf]

@ r4 = ppcinfo->m_rgsubfrmconfig[0].m_rgiSubFrameStart[0]
  LDRSH r4, [r0]

@ if ((ppcinfo->m_rgsubfrmconfig[0].m_rgiSubFrameStart[0]) >= pau->m_cFrameSampleHalf)
@    REPORT_BITSTREAM_CORRUPTION_AND_EXIT(wmaResult)@
  CMP   r4, r2
  BGE   gCorruptStream


@ if ((ppcinfo->m_rgsubfrmconfig[0].m_rgiSubFrameStart[0] + iLen) > pau->m_cFrameSampleHalf)
@    REPORT_BITSTREAM_CORRUPTION_AND_EXIT(wmaResult)@
  ADD   r12, r4, r5
  CMP   r12, r2
  BGT   gCorruptStream

  
@ if (ppcInfoComm->m_cLMSPredictors > LLMB_CLMSFLT_TTL_MAX || ppcInfoComm->m_cLMSPredictors < LLMB_CLMSFLT_TTL_MIN)
@    REPORT_BITSTREAM_CORRUPTION_AND_EXIT(wmaResult)@
  LDRSH r3, [r11, #PerChannelInfo_m_cLMSPredictors]
  CMP   r3, #4
  BGT   gCorruptStream

  CMP   r3, #1
  BLT   gCorruptStream
  
@ r2 = pau->m_iResidueMemScaling 
  ADD   r0, r9, #CAudioObject_m_iResidueMemScaling
  MOV   r3, #1
  LDRSH r2, [r0]

@ uiOffset1 = 1 << pau->m_iResidueMemScaling@
  MOV   r1, r3, LSL r2

@ iShift0 = pau->m_iResidueMemScaling@
  STR   r2, [sp, #ST_iShift0]
  ADD   r3, r2, #1
  STR   r1, [sp, #ST_uiOffset1]

@ iShift1 = (pau->m_iResidueMemScaling + 1)@
  STR   r3, [sp, #ST_iShift1]

@ r6 = paudec->m_LLMdecsts
@ r7 = ppcInfoComm->m_iSum
@ r8 = pau->m_iCurrReconCoef

  LDR   r0, [r11, #PerChannelInfo_m_rgiCoefRecon]
  LDR   r6, [r10, #CAudioObjectDecoder_m_LLMdecsts]
  LDR   r7, [r11, #PerChannelInfo_m_iSum]
  LDRH  r8, [r9, #CAudioObject_m_iCurrReconCoef]

@ rgiCoefRecon = ppcinfo->m_rgiCoefRecon + ppcinfo->m_rgsubfrmconfig[0].m_rgiSubFrameStart[0]@
@ rgiLPCResidue = rgiCoefRecon@ // share the same memory.
  ADD   r4, r0, r4, LSL #2
	
@ switch (paudec->m_LLMdecsts)
  CMP   r6, #10
  BEQ   gCH_COEF

  CMP   r6, #9
  BEQ   gCH_DIVISOR

  CMP   r6, #3
  BEQ   gCH_TRANSIENT_POS

@ CMP   r6, #2
@ BEQ   gCH_TRANSIENT

@ case CH_BEGIN:
@ CMP   r6, #0
@ BNE   gCH_END

@ paudec->m_LLMdecsts = CH_TRANSIENT@
  MOV   r6, #2

@ gCH_TRANSIENT
@ case CH_TRANSIENT:

@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, 1, (U32 *)&iResult))@
  ADD   r0, r10, #CAudioObjectDecoder_m_ibstrm
  MOV   r1, #1
  ADD   r2, sp, #ST_iResult
  BL    ibstrmGetBits

  CMP   r0, #0
  BMI   gSaveStatus1

@ r0 = iResult
  LDR   r0, [sp, #ST_iResult]

@ if (iResult == WMAB_TRUE) {// There is transient.
  CMP   r0, #1
  BNE   gNO_TRANSIENT

@ paudec->m_LLMdecsts = CH_TRANSIENT_POS@
@ ppcinfo->m_iTransientPosition = 0@
  MOV   lr, #0
  MOV   r6, #3
  STR   lr, [r11, #PerChannelInfo_m_iTransientPosition]

  B     ggCH_TRANSIENT_POS_FIRST
  
	
gNO_TRANSIENT:
@ ppcinfo->m_iTransientPosition = -1@
  MOV   lr, #-1
  STR   lr, [r11, #PerChannelInfo_m_iTransientPosition]
  B     gCH_TRANSIENT_POS_SECOND

gCH_TRANSIENT_POS:
@ if (ppcinfo->m_iTransientPosition != -1) {
  LDR   lr, [r11, #PerChannelInfo_m_iTransientPosition]
  CMP   lr, #-1
  BNE   gCH_TRANSIENT_POS_SECOND

ggCH_TRANSIENT_POS_FIRST:
@ LOG2(iLen)
  MOV   r1, #-1
  MOV   r3, r5

gLog2While:
  ADD   r1, r1, #1
  MOVS  r3, r3, LSR #1
  BNE   gLog2While

@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, LOG2(iLen), (U32 *)&iResult))@
  ADD   r0, r10, #CAudioObjectDecoder_m_ibstrm
  ADD   r2, sp, #ST_iResult
  BL    ibstrmGetBits

  CMP   r0, #0
  BMI   gSaveStatus1

@ ppcinfo->m_iTransientPosition = iResult@
  LDR   lr, [sp, #ST_iResult]
  STR   lr, [r11, #PerChannelInfo_m_iTransientPosition]

gCH_TRANSIENT_POS_SECOND:
@ paudec->m_LLMdecsts = CH_DIVISOR@
  MOV   r6, #9


gCH_DIVISOR:
@ if (pau->m_bSeekable == WMAB_TRUE) {
  LDR   r0, [r9, #CAudioObject_m_bSeekable]
  CMP   r0, #1
  BNE   gCH_DIVISOR_NE

@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, pau->m_nValidBitsPerSample, (U32 *)&iResult))@
  LDRH  r1, [r9, #CAudioObject_m_nValidBitsPerSample]
  ADD   r0, r10, #CAudioObjectDecoder_m_ibstrm
  ADD   r2, sp, #ST_iResult
  BL    ibstrmGetBits

  CMP   r0, #0
  BMI   gSaveStatus1

@ iMean = iResult@
  LDR   r0, [sp, #ST_iResult]

@ ppcInfoComm->m_iSum = iMean * LLMA_RESIDUE_MEMORY_TTLWEIGHT@
@ ppcInfoComm->m_iSum = (ppcInfoComm->m_iSum << 1)@
  MOV   r7, r0, LSL #6

gCH_DIVISOR_NE:
@ paudec->m_LLMdecsts = CH_COEF@
  MOV   r6, #10
 
@ paudec->m_Dec1stSplsts = FIRSTSPL_SIGN_BIT@
@ pau->m_iCurrReconCoef = 0@
  MOV   r8, #0

gCH_COEF:
@ if (pau->m_bSeekable == WMAB_TRUE) {
  LDR   r0, [r9, #CAudioObject_m_bSeekable]
  CMP   r0, #1
  BNE   gCH_COEF_PRELOOP

@ if (pau->m_iCurrReconCoef == 0) { 
  CMP   r8, #0
  BNE   gCH_COEF_PRELOOP
  
	
@ if(pau->m_bDoInterChDecorr == DO_CHANNEL_MIXING) {
  LDR   r0, [r9, #CAudioObject_m_bDoInterChDecorr]
  CMP   r0, #1
  BNE   gNO_CHANNEL_MIXING

@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, (UInt)(pau->m_nValidBitsPerSample+1), (U32 *)&iResult))@
  LDRH  r1, [r9, #CAudioObject_m_nValidBitsPerSample]
  ADD   r0, r10, #CAudioObjectDecoder_m_ibstrm
  ADD   r2, sp, #ST_iResult
  ADD   r1, r1, #1
  BL    ibstrmGetBits

  CMP   r0, #0
  BMI   gSaveStatus1

@ r0 = iResult@
  LDR   r0, [sp, #ST_iResult]
  LDRH  r1, [r9, #CAudioObject_m_nValidBitsPerSample]

@ if (iResult & ((I32)1 << pau->m_nValidBitsPerSample))
  MOV   r2, #1
  MOV   r2, r2, LSL r1
  TST   r2, r0
  BEQ   gCH_COEF_RESULT

@ iResult = (~(((I32)1 << pau->m_nValidBitsPerSample) - 1)) | iResult@
  SUB   r2, r2, #1
  MVN   r1, r2
  ORR   r0, r1, r0

gCH_COEF_RESULT:
@ rgiLPCResidue[0] = (I32)iResult@
  STR   r0, [r4]

@ pau->m_iCurrReconCoef++@
  ADD   r8, r8, #1
  B     gCH_COEF_PRELOOP


gNO_CHANNEL_MIXING:
@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, (UInt)(pau->m_nValidBitsPerSample), (U32 *)&iResult))@
  LDRH  r1, [r9, #CAudioObject_m_nValidBitsPerSample]
  ADD   r0, r10, #CAudioObjectDecoder_m_ibstrm
  ADD   r2, sp, #ST_iResult
  BL    ibstrmGetBits

  CMP   r0, #0
  BMI   gSaveStatus1
	
@ iResult@
  LDR   r0, [sp, #ST_iResult]
  LDRH  r1, [r9, #CAudioObject_m_nValidBitsPerSample]

@ if (iResult & ((I32)1 << (pau->m_nValidBitsPerSample - 1)))
  SUB   r3, r1, #1
  MOV   r2, #1
  MOV   r2, r2, LSL r3
  TST   r2, r0
  BEQ   gNO_CHANNEL_MIXING_RESULT

@ iResult = (~(((I32)1 << (pau->m_nValidBitsPerSample - 1)) - 1)) | iResult@
  SUB   r1, r2, #1
  MVN   r3, r1
  ORR   r0, r3, r0

gNO_CHANNEL_MIXING_RESULT:
@ rgiLPCResidue[0] = (I32)iResult@
  STR   r0, [r4]

@ pau->m_iCurrReconCoef++@
  ADD   r8, r8, #1
    

gCH_COEF_PRELOOP:
  STR   r6, [r10, #CAudioObjectDecoder_m_LLMdecsts]

@ r6 = iDivPow2
@ r7 = ppcInfoComm->m_iSum
@ r8 = pau->m_iCurrReconCoef
	
  
gCH_COEF_LOOP:
@ for (@ pau->m_iCurrReconCoef < iLen@ pau->m_iCurrReconCoef++) {
  CMP   r8, r5
  BGE   gCH_END
	
@ switch (paudec->m_Colombdecsts)
  LDR   r1, [r10, #CAudioObjectDecoder_m_Colombdecsts]
  ADD   r0, pc, #4
  LDRB  r0, [r0, r1]  
  ADD   pc, pc, r0

  .byte   0x0      @gFIRST_PART
  .byte   0x28     @gFIRST_PART_B, 10 instructions
  .byte   0x5C     @gFIRST_PART_C, 10 + 13 instructions
  .byte   0xEC     @gSECOND_PART,  10 + 13 + 36 instructions 

@ case FIRST_PART:
gFIRST_PART:

@ TRACEWMA_EXIT(wmaResult, ibstrmCountSerial1Bits(&paudec->m_ibstrm, &paudec->m_iResQ))@
  ADD  r1, r10, #CAudioObjectDecoder_m_iResQ
  ADD  r0, r10, #CAudioObjectDecoder_m_ibstrm
  BL   ibstrmCountSerial1Bits

  CMP   r0, #0
  BMI   gSaveStatus2

@ RESQ_ESCAPE_VALUE   32
@ if (paudec->m_iResQ < RESQ_ESCAPE_VALUE) {
  LDR   r1, [r10, #CAudioObjectDecoder_m_iResQ] 
  CMP   r1, #32
  BCC   gFIRST_PART_CC

@ paudec->m_Colombdecsts = FIRST_PART_B@
  MOV   r2, #1
  STR   r2, [r10, #CAudioObjectDecoder_m_Colombdecsts]

gFIRST_PART_B:
@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, 5, (U32 *)&iResult))@
  ADD   r0, r10, #CAudioObjectDecoder_m_ibstrm
  MOV   r1, #5
  ADD   r2, sp, #ST_iResult
  BL    ibstrmGetBits

  CMP   r0, #0
  BMI   gSaveStatus2

@ iResult + 1
@ paudec->m_iResQBits = iResult + 1@
  LDR   r1, [sp, #ST_iResult]
  ADD   r1, r1, #1
  STR   r1, [r10, #CAudioObjectDecoder_m_iResQBits]

@ if (iResult + 1 > 32)
@    REPORT_BITSTREAM_CORRUPTION_AND_EXIT(wmaResult)@
  CMP   r1, #32
  BGT   gCorruptStream

@ paudec->m_Colombdecsts = FIRST_PART_C@
  MOV   r2, #2
  STR   r2, [r10, #CAudioObjectDecoder_m_Colombdecsts]


gFIRST_PART_C:
@ if (paudec->m_iResQBits <= 24) {
  LDR   r1, [r10, #CAudioObjectDecoder_m_iResQBits]
  CMP   r1, #24
  BLS   gFIRST_PART_C_LS24

@ TRACEWMA_EXIT(wmaResult, ibstrmLookForBits(&paudec->m_ibstrm, paudec->m_iResQBits))@
  ADD   r0, r10, #CAudioObjectDecoder_m_ibstrm
  BL    ibstrmLookForBits

  CMP   r0, #0
  BMI   gSaveStatus2
                          
@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, paudec->m_iResQBits - 24, (U32 *)&iResult))@
  LDR   r1, [r10, #CAudioObjectDecoder_m_iResQBits]
  ADD   r0, r10, #CAudioObjectDecoder_m_ibstrm
  SUB   r1, r1, #24
  ADD   r2, sp, #ST_iResult
  BL    ibstrmGetBits

  CMP   r0, #0
  BMI   gSaveStatus2

@ paudec->m_iResQ += (((U32)iResult) << 24)@
  LDR   r0, [sp, #ST_iResult]
  LDR   r1, [r10, #CAudioObjectDecoder_m_iResQ] 

  ADD   r2, r1, r0, LSL #24
  STR   r2, [r10, #CAudioObjectDecoder_m_iResQ] 

@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, 24, (U32 *)&iResult))@
  ADD   r0, r10, #CAudioObjectDecoder_m_ibstrm
  MOV   r1, #24
  ADD   r2, sp, #ST_iResult
  BL    ibstrmGetBits

  CMP   r0, #0
  BMI   gSaveStatus2

  B     gFIRST_PART_C_END

gFIRST_PART_C_LS24:
@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, paudec->m_iResQBits, (U32 *)&iResult))@
  ADD   r0, r10, #CAudioObjectDecoder_m_ibstrm
  ADD   r2, sp, #ST_iResult
  BL    ibstrmGetBits
  
  CMP   r0, #0
  BMI   gSaveStatus2

gFIRST_PART_C_END:
@ paudec->m_iResQ += iResult@
  LDR   r0, [sp, #ST_iResult]
  LDR   r1, [r10, #CAudioObjectDecoder_m_iResQ]
  ADD   r1, r1, r0
  STR   r1, [r10, #CAudioObjectDecoder_m_iResQ] 

gFIRST_PART_CC:
@ paudec->m_Colombdecsts = SECOND_PART@  
  MOV   r2, #3
  STR   r2, [r10, #CAudioObjectDecoder_m_Colombdecsts]

gSECOND_PART:
@ iSum = ppcInfoComm->m_iSum@
@ iMean = (iSum + uiOffset1) >> iShift1@
  LDR   r1, [sp, #ST_uiOffset1]
  LDR   r2, [sp, #ST_iShift1]
  ADD   r0, r7, r1
  MOV   r0, r0, LSR r2

@ CEILOFLOG2(iDivPow2, iMean)

  MOV   r6, #0
  MOV   r2, r0
  MOV   r3, #1

gCEILOFLOG2:
  MOVS  r0, r0, LSR #1
  ADDGT r6, r6, #1
  BGT   gCEILOFLOG2
  
  SUBS  r0, r2, r3, LSL r6
  ADDGT r6, r6, #1

@ if (iDivPow2 <= 24) {
  CMP   r6, #24
  BLS   gSECOND_PART_LS24

@ TRACEWMA_EXIT(wmaResult, ibstrmLookForBits(&paudec->m_ibstrm, iDivPow2))@
  ADD   r0, r10, #CAudioObjectDecoder_m_ibstrm
  MOV   r1, r6
  BL    ibstrmLookForBits

  CMP   r0, #0
  BMI   gSaveStatus2

@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, iDivPow2 - 24, (U32 *)&iResult))@
  ADD   r0, r10, #CAudioObjectDecoder_m_ibstrm
  ADD   r2, sp, #ST_iResult
  SUB   r1, r6, #24
  BL    ibstrmGetBits
  
  CMP   r0, #0
  BMI   gSaveStatus2

@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, 24, (U32 *)&iResult))@
  ADD   r0, r10, #CAudioObjectDecoder_m_ibstrm
  ADD   r2, sp, #ST_iResult1
  MOV   r1, #24
  BL    ibstrmGetBits
  
  CMP   r0, #0
  BMI   gSaveStatus2

@ iRem = (((U32)iResult) << 24)@
@ iRem += ((U32)iResult1)@
  LDR   r0, [sp, #ST_iResult]
  LDR   r1, [sp, #ST_iResult1]

@ r2 = iRem
  ADD   r2, r1, r0, LSL #24
  B     gCH_COEF_SWITCH_END

gSECOND_PART_LS24:
@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, iDivPow2, (U32 *)&iResult))@
  ADD   r0, r10, #CAudioObjectDecoder_m_ibstrm
  ADD   r2, sp, #ST_iResult
  MOV   r1, r6
  BL    ibstrmGetBits
  
  CMP   r0, #0
  BMI   gSaveStatus2

@ iRem = iResult@
@ r2 = iRem
  LDR   r2, [sp, #ST_iResult]

gCH_COEF_SWITCH_END:
@ uiResidue = (paudec->m_iResQ << iDivPow2) + iRem@
  LDR   r1, [r10, #CAudioObjectDecoder_m_iResQ] 
  ADD   r3, r2, r1, LSL r6

@ r0 = iShift0
  LDR   r0, [sp, #ST_iShift0]

@ ppcInfoComm->m_iSum  = uiResidue + ppcInfoComm->m_iSum - (ppcInfoComm->m_iSum >> iShift0)@
  SUB   r1, r3, r7, LSR r0
  MOV   r2, #-1
  ADD   r7, r7, r1
  
@ iResidue = (I32)(uiResidue >> 1)@
@ if (uiResidue & 0x1) {
@ iResidue = -iResidue - 1@
@ // iResidue = ~iResidue@
  MOVS  r0, r3, LSR #1
  EORCS r0, r0, r2

@ paudec->m_Colombdecsts = FIRST_PART@
  MOV   r2, #0
  STR   r2, [r10, #CAudioObjectDecoder_m_Colombdecsts]

@ rgiLPCResidue[pau->m_iCurrReconCoef] = iResidue@
  STR   r0, [r4, r8, LSL #2]

@ paudec->m_iResQ = 0@
@ paudec->m_iRem = 0@
  STR   r2, [r10, #CAudioObjectDecoder_m_iResQ]

  ADD   r8, r8, #1
  B     gCH_COEF_LOOP

gCH_END:
@ if (pau->m_bSeekable == WMAB_TRUE) {
@    ppcInfoComm->m_iUpdSpdUpSamples = ppcInfoComm->m_cSubFrameSampleHalf@
  LDR   r0, [r9, #CAudioObject_m_bSeekable]
  CMP   r0, #1
  STREQ r5, [r11, #PerChannelInfo_m_iUpdSpdUpSamples]


@ prvDecoderCDLMSPredictorHelper(paudec, ppcinfo, ppcinfo, rgiCoefRecon, iLen)@
  MOV   r0, r10
  MOV   r1, r11
  MOV   r2, r11
  MOV   r3, r4
  STR   r5, [sp, #ST_iResult]

  BL    prvDecoderCDLMSPredictorHelper
	MOV		r0, #0

gSaveStatus2:
  STRH  r8, [r9, #CAudioObject_m_iCurrReconCoef]
  STR   r7, [r11, #PerChannelInfo_m_iSum]

  B     gExit
  
gCorruptStream:
@ REPORT_BITSTREAM_CORRUPTION_AND_EXIT(wmaResult)@
@ WMA_E_BROKEN_FRAME = 0x80040002
  MOV   r0, #0x00000002
  MOV   r1, #0x00040000
  MOV   r2, #0x80000000

  ORR   r0, r1, r0
  ORR   r0, r0, r2
  B     gExit

gSaveStatus1:
  STR   r6, [r10, #CAudioObjectDecoder_m_LLMdecsts]
  STR   r7, [r11, #PerChannelInfo_m_iSum]
  STRH  r8, [r9, #CAudioObject_m_iCurrReconCoef]

	
gExit:
  ADD   sp,  sp, #ST_SIZE  @cleanup stack
  LDMFD sp!, {r4 - r12, PC} @prvDecodeSubFrameChannelResiduePureLosslessModeVerB
  ENTRY_END prvDecodeSubFrameChannelResiduePureLosslessModeVerB

  .endif @ WMA_OPT_LOSSLESSDECLSL_ARM
    
  .end

