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

  #include "../../../inc/audio/v10/include/voWMADecID.h"
  .include     "kxarm.h"
  .include     "wma_member_arm.inc"
  .include	  "wma_arm_version.h"

 
  @AREA    |.text|, CODE, READONLY
  .text .align 4

  .if WMA_OPT_LOSSLESSDECLSL_ARM == 1

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  .globl  _ibstrmGetBits
  .globl  _ibstrmLookForBits
  .globl  _ibstrmCountSerial1Bits
  .globl  _prvDecoderCDLMSPredictorHelper

  .globl  _prvDecodeSubFrameChannelResiduePureLosslessModeVerB
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.if LINUX_RVDS == 1
  	PRESERVE8
	.endif
_prvDecodeSubFrameChannelResiduePureLosslessModeVerB:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@stack definition
.set ST_uiOffset1             , 16
.set ST_iShift1               , 12
.set ST_iShift0               , 8
.set ST_iResult1              , 4
.set ST_iResult               , 0

.set ST_SIZE                  , 20


@ Input parameters
@ r0 = pau
@ r1 = paudec
@ r2 = ppcinfo

  stmfd sp!, {r4 - r12, lr}

  sub   sp, sp, #ST_SIZE  @setup stack

@ r4 = rgiLPCResidue
@ r5 = iLen

@ r9 = pau
@ r10= paudec
@ r11= ppcinfo


  mov   r11, r2
  mov   r10, r1
  mov   r9,  r0
  
  ldr   r3, [r11, #PerChannelInfo_m_rgsubfrmconfig]
  ldr   r2, [r9, #CAudioObject_m_cFrameSampleHalf]
  ldr   r0, [r3, #SubFrameConfigInfo_m_rgiSubFrameStart]

@ iLen = ppcinfo->m_cSubFrameSampleHalf@
  ldrsh r5, [r11, #PerChannelInfo_m_cSubFrameSampleHalf]

@ r4 = ppcinfo->m_rgsubfrmconfig[0].m_rgiSubFrameStart[0]
  ldrsh r4, [r0]

@ if ((ppcinfo->m_rgsubfrmconfig[0].m_rgiSubFrameStart[0]) >= pau->m_cFrameSampleHalf)
@    REPORT_BITSTREAM_CORRUPTION_AND_EXIT(wmaResult)@
  cmp   r4, r2
  bge   gCorruptStream


@ if ((ppcinfo->m_rgsubfrmconfig[0].m_rgiSubFrameStart[0] + iLen) > pau->m_cFrameSampleHalf)
@    REPORT_BITSTREAM_CORRUPTION_AND_EXIT(wmaResult)@
  add   r12, r4, r5
  cmp   r12, r2
  bgt   gCorruptStream

  
@ if (ppcInfoComm->m_cLMSPredictors > LLMB_CLMSFLT_TTL_MAX || ppcInfoComm->m_cLMSPredictors < LLMB_CLMSFLT_TTL_MIN)
@    REPORT_BITSTREAM_CORRUPTION_AND_EXIT(wmaResult)@
  ldrsh r3, [r11, #PerChannelInfo_m_cLMSPredictors]
  cmp   r3, #4
  bgt   gCorruptStream

  cmp   r3, #1
  blt   gCorruptStream
  
@ r2 = pau->m_iResidueMemScaling 
  add   r0, r9, #CAudioObject_m_iResidueMemScaling
  mov   r3, #1
  ldrsh r2, [r0]

@ uiOffset1 = 1 << pau->m_iResidueMemScaling@
  mov   r1, r3, LSL r2

@ iShift0 = pau->m_iResidueMemScaling@
  str   r2, [sp, #ST_iShift0]
  add   r3, r2, #1
  str   r1, [sp, #ST_uiOffset1]

@ iShift1 = (pau->m_iResidueMemScaling + 1)@
  str   r3, [sp, #ST_iShift1]

@ r6 = paudec->m_LLMdecsts
@ r7 = ppcInfoComm->m_iSum
@ r8 = pau->m_iCurrReconCoef

  ldr   r0, [r11, #PerChannelInfo_m_rgiCoefRecon]
  ldr   r6, [r10, #CAudioObjectDecoder_m_LLMdecsts]
  ldr   r7, [r11, #PerChannelInfo_m_iSum]
  ldrh  r8, [r9, #CAudioObject_m_iCurrReconCoef]

@ rgiCoefRecon = ppcinfo->m_rgiCoefRecon + ppcinfo->m_rgsubfrmconfig[0].m_rgiSubFrameStart[0]@
@ rgiLPCResidue = rgiCoefRecon@ // share the same memory.
  add   r4, r0, r4, LSL #2
	
@ switch (paudec->m_LLMdecsts)
  cmp   r6, #10
  beq   gCH_COEF

  cmp   r6, #9
  beq   gCH_DIVISOR

  cmp   r6, #3
  beq   gCH_TRANSIENT_POS

@ cmp   r6, #2
@ beq   gCH_TRANSIENT

@ case CH_BEGIN:
@ cmp   r6, #0
@ bne   gCH_END

@ paudec->m_LLMdecsts = CH_TRANSIENT@
  mov   r6, #2

@ gCH_TRANSIENT
@ case CH_TRANSIENT:

@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, 1, (U32 *)&iResult))@
  add   r0, r10, #CAudioObjectDecoder_m_ibstrm
  mov   r1, #1
  add   r2, sp, #ST_iResult
  bl    _ibstrmGetBits

  cmp   r0, #0
  bmi   gSaveStatus1

@ r0 = iResult
  ldr   r0, [sp, #ST_iResult]

@ if (iResult == WMAB_TRUE) {// There is transient.
  cmp   r0, #1
  bne   gNO_TRANSIENT

@ paudec->m_LLMdecsts = CH_TRANSIENT_POS@
@ ppcinfo->m_iTransientPosition = 0@
  mov   lr, #0
  mov   r6, #3
  str   lr, [r11, #PerChannelInfo_m_iTransientPosition]

  b     ggCH_TRANSIENT_POS_FIRST
  
	
gNO_TRANSIENT:
@ ppcinfo->m_iTransientPosition = -1@
  mov   lr, #-1
  str   lr, [r11, #PerChannelInfo_m_iTransientPosition]
  b     gCH_TRANSIENT_POS_SECOND

gCH_TRANSIENT_POS:
@ if (ppcinfo->m_iTransientPosition != -1) {
  ldr   lr, [r11, #PerChannelInfo_m_iTransientPosition]
  cmp   lr, #-1
  bne   gCH_TRANSIENT_POS_SECOND

ggCH_TRANSIENT_POS_FIRST:
@ LOG2(iLen)
  mov   r1, #-1
  mov   r3, r5

gLog2While:
  add   r1, r1, #1
  movs  r3, r3, LSR #1
  bne   gLog2While

@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, LOG2(iLen), (U32 *)&iResult))@
  add   r0, r10, #CAudioObjectDecoder_m_ibstrm
  add   r2, sp, #ST_iResult
  bl    _ibstrmGetBits

  cmp   r0, #0
  bmi   gSaveStatus1

@ ppcinfo->m_iTransientPosition = iResult@
  ldr   lr, [sp, #ST_iResult]
  str   lr, [r11, #PerChannelInfo_m_iTransientPosition]

gCH_TRANSIENT_POS_SECOND:
@ paudec->m_LLMdecsts = CH_DIVISOR@
  mov   r6, #9


gCH_DIVISOR:
@ if (pau->m_bSeekable == WMAB_TRUE) {
  ldr   r0, [r9, #CAudioObject_m_bSeekable]
  cmp   r0, #1
  bne   gCH_DIVISOR_NE

@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, pau->m_nValidBitsPerSample, (U32 *)&iResult))@
  ldrh  r1, [r9, #CAudioObject_m_nValidBitsPerSample]
  add   r0, r10, #CAudioObjectDecoder_m_ibstrm
  add   r2, sp, #ST_iResult
  bl    _ibstrmGetBits

  cmp   r0, #0
  bmi   gSaveStatus1

@ iMean = iResult@
  ldr   r0, [sp, #ST_iResult]

@ ppcInfoComm->m_iSum = iMean * LLMA_RESIDUE_MEMORY_TTLWEIGHT@
@ ppcInfoComm->m_iSum = (ppcInfoComm->m_iSum << 1)@
  mov   r7, r0, LSL #6

gCH_DIVISOR_NE:
@ paudec->m_LLMdecsts = CH_COEF@
  mov   r6, #10
 
@ paudec->m_Dec1stSplsts = FIRSTSPL_SIGN_BIT@
@ pau->m_iCurrReconCoef = 0@
  mov   r8, #0

gCH_COEF:
@ if (pau->m_bSeekable == WMAB_TRUE) {
  ldr   r0, [r9, #CAudioObject_m_bSeekable]
  cmp   r0, #1
  bne   gCH_COEF_PRELOOP

@ if (pau->m_iCurrReconCoef == 0) { 
  cmp   r8, #0
  bne   gCH_COEF_PRELOOP
  
	
@ if(pau->m_bDoInterChDecorr == DO_CHANNEL_MIXING) {
  ldr   r0, [r9, #CAudioObject_m_bDoInterChDecorr]
  cmp   r0, #1
  bne   gNO_CHANNEL_MIXING

@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, (UInt)(pau->m_nValidBitsPerSample+1), (U32 *)&iResult))@
  ldrh  r1, [r9, #CAudioObject_m_nValidBitsPerSample]
  add   r0, r10, #CAudioObjectDecoder_m_ibstrm
  add   r2, sp, #ST_iResult
  add   r1, r1, #1
  bl    _ibstrmGetBits

  cmp   r0, #0
  bmi   gSaveStatus1

@ r0 = iResult@
  ldr   r0, [sp, #ST_iResult]
  ldrh  r1, [r9, #CAudioObject_m_nValidBitsPerSample]

@ if (iResult & ((I32)1 << pau->m_nValidBitsPerSample))
  mov   r2, #1
  mov   r2, r2, LSL r1
  tst   r2, r0
  beq   gCH_COEF_RESULT

@ iResult = (~(((I32)1 << pau->m_nValidBitsPerSample) - 1)) | iResult@
  sub   r2, r2, #1
  mvn   r1, r2
  orr   r0, r1, r0

gCH_COEF_RESULT:
@ rgiLPCResidue[0] = (I32)iResult@
  str   r0, [r4]

@ pau->m_iCurrReconCoef++@
  add   r8, r8, #1
  b     gCH_COEF_PRELOOP


gNO_CHANNEL_MIXING:
@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, (UInt)(pau->m_nValidBitsPerSample), (U32 *)&iResult))@
  ldrh  r1, [r9, #CAudioObject_m_nValidBitsPerSample]
  add   r0, r10, #CAudioObjectDecoder_m_ibstrm
  add   r2, sp, #ST_iResult
  bl    _ibstrmGetBits

  cmp   r0, #0
  bmi   gSaveStatus1
	
@ iResult@
  ldr   r0, [sp, #ST_iResult]
  ldrh  r1, [r9, #CAudioObject_m_nValidBitsPerSample]

@ if (iResult & ((I32)1 << (pau->m_nValidBitsPerSample - 1)))
  sub   r3, r1, #1
  mov   r2, #1
  mov   r2, r2, LSL r3
  tst   r2, r0
  beq   gNO_CHANNEL_MIXING_RESULT

@ iResult = (~(((I32)1 << (pau->m_nValidBitsPerSample - 1)) - 1)) | iResult@
  sub   r1, r2, #1
  mvn   r3, r1
  orr   r0, r3, r0

gNO_CHANNEL_MIXING_RESULT:
@ rgiLPCResidue[0] = (I32)iResult@
  str   r0, [r4]

@ pau->m_iCurrReconCoef++@
  add   r8, r8, #1
    

gCH_COEF_PRELOOP:
  str   r6, [r10, #CAudioObjectDecoder_m_LLMdecsts]

@ r6 = iDivPow2
@ r7 = ppcInfoComm->m_iSum
@ r8 = pau->m_iCurrReconCoef
	
  
gCH_COEF_LOOP:
@ for (@ pau->m_iCurrReconCoef < iLen@ pau->m_iCurrReconCoef++) {
  cmp   r8, r5
  bge   gCH_END
	
@ switch (paudec->m_Colombdecsts)
  ldr   r1, [r10, #CAudioObjectDecoder_m_Colombdecsts]
  add   r0, pc, #4
  ldrb  r0, [r0, r1]  
  add   pc, pc, r0

  .byte   0x0      @gFIRST_PART
  .byte   0x28     @gFIRST_PART_B, 10 instructions
  .byte   0x5C     @gFIRST_PART_C, 10 + 13 instructions
  .byte   0xEC     @gSECOND_PART,  10 + 13 + 36 instructions 

@ case FIRST_PART:
gFIRST_PART:

@ TRACEWMA_EXIT(wmaResult, ibstrmCountSerial1Bits(&paudec->m_ibstrm, &paudec->m_iResQ))@
  add  r1, r10, #CAudioObjectDecoder_m_iResQ
  add  r0, r10, #CAudioObjectDecoder_m_ibstrm
  bl   _ibstrmCountSerial1Bits

  cmp   r0, #0
  bmi   gSaveStatus2

@ RESQ_ESCAPE_VALUE   32
@ if (paudec->m_iResQ < RESQ_ESCAPE_VALUE) {
  ldr   r1, [r10, #CAudioObjectDecoder_m_iResQ] 
  cmp   r1, #32
  bcc   gFIRST_PART_CC

@ paudec->m_Colombdecsts = FIRST_PART_B@
  mov   r2, #1
  str   r2, [r10, #CAudioObjectDecoder_m_Colombdecsts]

gFIRST_PART_B:
@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, 5, (U32 *)&iResult))@
  add   r0, r10, #CAudioObjectDecoder_m_ibstrm
  mov   r1, #5
  add   r2, sp, #ST_iResult
  bl    _ibstrmGetBits

  cmp   r0, #0
  bmi   gSaveStatus2

@ iResult + 1
@ paudec->m_iResQBits = iResult + 1@
  ldr   r1, [sp, #ST_iResult]
  add   r1, r1, #1
  str   r1, [r10, #CAudioObjectDecoder_m_iResQBits]

@ if (iResult + 1 > 32)
@    REPORT_BITSTREAM_CORRUPTION_AND_EXIT(wmaResult)@
  cmp   r1, #32
  bgt   gCorruptStream

@ paudec->m_Colombdecsts = FIRST_PART_C@
  mov   r2, #2
  str   r2, [r10, #CAudioObjectDecoder_m_Colombdecsts]


gFIRST_PART_C:
@ if (paudec->m_iResQBits <= 24) {
  ldr   r1, [r10, #CAudioObjectDecoder_m_iResQBits]
  cmp   r1, #24
  bls   gFIRST_PART_C_LS24

@ TRACEWMA_EXIT(wmaResult, ibstrmLookForBits(&paudec->m_ibstrm, paudec->m_iResQBits))@
  add   r0, r10, #CAudioObjectDecoder_m_ibstrm
  bl    _ibstrmLookForBits

  cmp   r0, #0
  bmi   gSaveStatus2
                          
@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, paudec->m_iResQBits - 24, (U32 *)&iResult))@
  ldr   r1, [r10, #CAudioObjectDecoder_m_iResQBits]
  add   r0, r10, #CAudioObjectDecoder_m_ibstrm
  sub   r1, r1, #24
  add   r2, sp, #ST_iResult
  bl    _ibstrmGetBits

  cmp   r0, #0
  bmi   gSaveStatus2

@ paudec->m_iResQ += (((U32)iResult) << 24)@
  ldr   r0, [sp, #ST_iResult]
  ldr   r1, [r10, #CAudioObjectDecoder_m_iResQ] 

  add   r2, r1, r0, LSL #24
  str   r2, [r10, #CAudioObjectDecoder_m_iResQ] 

@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, 24, (U32 *)&iResult))@
  add   r0, r10, #CAudioObjectDecoder_m_ibstrm
  mov   r1, #24
  add   r2, sp, #ST_iResult
  bl    _ibstrmGetBits

  cmp   r0, #0
  bmi   gSaveStatus2

  b     gFIRST_PART_C_END

gFIRST_PART_C_LS24:
@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, paudec->m_iResQBits, (U32 *)&iResult))@
  add   r0, r10, #CAudioObjectDecoder_m_ibstrm
  add   r2, sp, #ST_iResult
  bl    _ibstrmGetBits
  
  cmp   r0, #0
  bmi   gSaveStatus2

gFIRST_PART_C_END:
@ paudec->m_iResQ += iResult@
  ldr   r0, [sp, #ST_iResult]
  ldr   r1, [r10, #CAudioObjectDecoder_m_iResQ]
  add   r1, r1, r0
  str   r1, [r10, #CAudioObjectDecoder_m_iResQ] 

gFIRST_PART_CC:
@ paudec->m_Colombdecsts = SECOND_PART@  
  mov   r2, #3
  str   r2, [r10, #CAudioObjectDecoder_m_Colombdecsts]

gSECOND_PART:
@ iSum = ppcInfoComm->m_iSum@
@ iMean = (iSum + uiOffset1) >> iShift1@
  ldr   r1, [sp, #ST_uiOffset1]
  ldr   r2, [sp, #ST_iShift1]
  add   r0, r7, r1
  mov   r0, r0, LSR r2

@ CEILOFLOG2(iDivPow2, iMean)

  mov   r6, #0
  mov   r2, r0
  mov   r3, #1

gCEILOFLOG2:
  movs  r0, r0, LSR #1
  addgt r6, r6, #1
  bgt   gCEILOFLOG2
  
  subs  r0, r2, r3, LSL r6
  addgt r6, r6, #1

@ if (iDivPow2 <= 24) {
  cmp   r6, #24
  bls   gSECOND_PART_LS24

@ TRACEWMA_EXIT(wmaResult, ibstrmLookForBits(&paudec->m_ibstrm, iDivPow2))@
  add   r0, r10, #CAudioObjectDecoder_m_ibstrm
  mov   r1, r6
  bl    _ibstrmLookForBits

  cmp   r0, #0
  bmi   gSaveStatus2

@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, iDivPow2 - 24, (U32 *)&iResult))@
  add   r0, r10, #CAudioObjectDecoder_m_ibstrm
  add   r2, sp, #ST_iResult
  sub   r1, r6, #24
  bl    _ibstrmGetBits
  
  cmp   r0, #0
  bmi   gSaveStatus2

@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, 24, (U32 *)&iResult))@
  add   r0, r10, #CAudioObjectDecoder_m_ibstrm
  add   r2, sp, #ST_iResult1
  mov   r1, #24
  bl    _ibstrmGetBits
  
  cmp   r0, #0
  bmi   gSaveStatus2

@ iRem = (((U32)iResult) << 24)@
@ iRem += ((U32)iResult1)@
  ldr   r0, [sp, #ST_iResult]
  ldr   r1, [sp, #ST_iResult1]

@ r2 = iRem
  add   r2, r1, r0, LSL #24
  b     gCH_COEF_SWITCH_END

gSECOND_PART_LS24:
@ TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, iDivPow2, (U32 *)&iResult))@
  add   r0, r10, #CAudioObjectDecoder_m_ibstrm
  add   r2, sp, #ST_iResult
  mov   r1, r6
  bl    _ibstrmGetBits
  
  cmp   r0, #0
  bmi   gSaveStatus2

@ iRem = iResult@
@ r2 = iRem
  ldr   r2, [sp, #ST_iResult]

gCH_COEF_SWITCH_END:
@ uiResidue = (paudec->m_iResQ << iDivPow2) + iRem@
  ldr   r1, [r10, #CAudioObjectDecoder_m_iResQ] 
  add   r3, r2, r1, LSL r6

@ r0 = iShift0
  ldr   r0, [sp, #ST_iShift0]

@ ppcInfoComm->m_iSum  = uiResidue + ppcInfoComm->m_iSum - (ppcInfoComm->m_iSum >> iShift0)@
  sub   r1, r3, r7, LSR r0
  mov   r2, #-1
  add   r7, r7, r1
  
@ iResidue = (I32)(uiResidue >> 1)@
@ if (uiResidue & 0x1) {
@ iResidue = -iResidue - 1@
@ // iResidue = ~iResidue@
  movs  r0, r3, LSR #1
  eorcs r0, r0, r2

@ paudec->m_Colombdecsts = FIRST_PART@
  mov   r2, #0
  str   r2, [r10, #CAudioObjectDecoder_m_Colombdecsts]

@ rgiLPCResidue[pau->m_iCurrReconCoef] = iResidue@
  str   r0, [r4, r8, LSL #2]

@ paudec->m_iResQ = 0@
@ paudec->m_iRem = 0@
  str   r2, [r10, #CAudioObjectDecoder_m_iResQ]

  add   r8, r8, #1
  b     gCH_COEF_LOOP

gCH_END:
@ if (pau->m_bSeekable == WMAB_TRUE) {
@    ppcInfoComm->m_iUpdSpdUpSamples = ppcInfoComm->m_cSubFrameSampleHalf@
  ldr   r0, [r9, #CAudioObject_m_bSeekable]
  cmp   r0, #1
  streq r5, [r11, #PerChannelInfo_m_iUpdSpdUpSamples]


@ prvDecoderCDLMSPredictorHelper(paudec, ppcinfo, ppcinfo, rgiCoefRecon, iLen)@
  mov   r0, r10
  mov   r1, r11
  mov   r2, r11
  mov   r3, r4
  str   r5, [sp, #ST_iResult]

  bl    _prvDecoderCDLMSPredictorHelper
	mov		r0, #0

gSaveStatus2:
  strh  r8, [r9, #CAudioObject_m_iCurrReconCoef]
  str   r7, [r11, #PerChannelInfo_m_iSum]

  b     gExit
  
gCorruptStream:
@ REPORT_BITSTREAM_CORRUPTION_AND_EXIT(wmaResult)@
@ WMA_E_BROKEN_FRAME = 0x80040002
  mov   r0, #0x00000002
  mov   r1, #0x00040000
  mov   r2, #0x80000000

  orr   r0, r1, r0
  orr   r0, r0, r2
  b     gExit

gSaveStatus1:
  str   r6, [r10, #CAudioObjectDecoder_m_LLMdecsts]
  str   r7, [r11, #PerChannelInfo_m_iSum]
  strh  r8, [r9, #CAudioObject_m_iCurrReconCoef]

	
gExit:
  add   sp,  sp, #ST_SIZE  @cleanup stack
  ldmfd sp!, {r4 - r12, PC} @prvDecodeSubFrameChannelResiduePureLosslessModeVerB
  @ENTRY_END prvDecodeSubFrameChannelResiduePureLosslessModeVerB

  .endif @ WMA_OPT_LOSSLESSDECLSL_ARM

