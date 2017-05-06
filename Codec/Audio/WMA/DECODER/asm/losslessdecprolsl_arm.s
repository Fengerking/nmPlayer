;//*@@@+++@@@@******************************************************************
;//
;// Microsoft Windows Media
;// Copyright (C) Microsoft Corporation. All rights reserved.
;//
;//*@@@---@@@@******************************************************************
;// Module Name:
;//
;//     losslessdecprolsl_arm.s
;//
;// Abstract:
;// 
;//     ARM Arch-4 specific multiplications
;//
;//      Custom build with 
;//          armasm $(InputDir)\$(InputName).s -o=$(OutDir)\$(InputName).obj 
;//      and
;//          $(OutDir)\$(InputName).obj
;// 
;// Author:
;// 
;//     Jerry He (yamihe) Aug 6, 2003
;//
;// Revision History:
;//
;//     For more information on ARM assembler directives, use
;//        http://msdn.microsoft.com/library/default.asp?url=/library/en-us/wcechp40/html/ccconarmassemblerdirectives.asp
;//*************************************************************************


  OPT         2       ; disable listing 
  INCLUDE     kxarm.h
  INCLUDE     wma_member_arm.inc
  OPT         1       ; enable listing
 
  AREA    |.text|, CODE, READONLY

  IF WMA_OPT_LOSSLESSDECPROLSL_ARM = 1
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  EXPORT  prvDecLMSPredictor_I16_C
  EXPORT  prvDecLMSPredictor_I32_C


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  AREA    |.text|, CODE
  LEAF_ENTRY prvDecLMSPredictor_I16_C
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Input parameters
; r0 = pau
; r1 = pLMSPredictor
; r2 = rgiResidue
; r3 = iLen

; r4 = pLMSPredictor
; r5 = rgiResidue
; r6 = iLen

; r7 = rgiPrevValRdBuf
; r8 = rgiUpdateRdBuf

; r10= iInput
; r11= iRecent

  STMFD sp!, {r4 - r11, lr}


  LDR   r11,[r1, #LMSPredictor_m_iRecent]
  MOV   r5, r2

  LDR   r7, [r1, #LMSPredictor_m_rgiPrevVal]
  MOV   r6, r3

  LDR   r8, [r1, #LMSPredictor_m_rgiUpdate]
  MOV   r4, r1

gOutLoop16

; for (i = 0; i < iLen; i++) {
; r0 = iPred
; iPred = iScalingOffset;
  LDR   r0, [r4, #LMSPredictor_m_iScalingOffset]

; rgiFilter = rgiFilterBuf;
  LDR   r9, [r4, #LMSPredictor_m_rgiFilter]

; r3 = iOrder_Div_8
  LDR   r3, [r4, #LMSPredictor_m_iOrder_Div_8]

; iPredErr = (I32)*rgiResidue;
; r2 = iPredErr
  LDR   r2, [r5]

; rgiPrevVal = rgiPrevValRdBuf + iRecent;
; r12 = rgiPrevVal
  ADD   r12, r7, r11, LSL #1

; rgiUpdate = rgiUpdateRdBuf + iRecent;
; r10 = rgiUpdate
  ADD   r10, r8, r11, LSL #1

  CMP   r2, #0
  BGT   gInLoop16GT
  BLT   gInLoop16LT

gInLoop16EQ
; for (j = 0; j < iOrder_Div_8; j++) {
; iPred += (I32)(rgiFilter[0]) * (I32)(rgiPrevVal[0]) + (I32)(rgiFilter[1]) * (I32)(rgiPrevVal[1]);
; iPred += (I32)(rgiFilter[2]) * (I32)(rgiPrevVal[2]) + (I32)(rgiFilter[3]) * (I32)(rgiPrevVal[3]);
; iPred += (I32)(rgiFilter[4]) * (I32)(rgiPrevVal[4]) + (I32)(rgiFilter[5]) * (I32)(rgiPrevVal[5]);
; iPred += (I32)(rgiFilter[6]) * (I32)(rgiPrevVal[6]) + (I32)(rgiFilter[7]) * (I32)(rgiPrevVal[7]);
; rgiFilter += 8;
; rgiPrevVal += 8;

; r9  = rgiFilter
; r12 = rgiPrevVal

  LDRSH r1, [r9], #2
  LDRSH r2, [r12], #2
  MLA   r0, r2, r1, r0

  LDRSH r1, [r9], #2
  LDRSH r2, [r12], #2
  MLA   r0, r2, r1, r0

  LDRSH r1, [r9], #2
  LDRSH r2, [r12], #2
  MLA   r0, r2, r1, r0

  LDRSH r1, [r9], #2
  LDRSH r2, [r12], #2
  MLA   r0, r2, r1, r0

  LDRSH r1, [r9], #2
  LDRSH r2, [r12], #2
  MLA   r0, r2, r1, r0

  LDRSH r1, [r9], #2
  LDRSH r2, [r12], #2
  MLA   r0, r2, r1, r0

  LDRSH r1, [r9], #2
  LDRSH r2, [r12], #2
  MLA   r0, r2, r1, r0

  LDRSH r1, [r9], #2
  LDRSH r2, [r12], #2
  MLA   r0, r2, r1, r0

  SUBS  r3, r3, #1
  BNE   gInLoop16EQ

  B     gInLoopEnd

gInLoop16GT
; for (j = 0; j < iOrder_Div_8; j++) {
; iPred += (I32)(rgiFilter[0]) * (I32)(rgiPrevVal[0]) + (I32)(rgiFilter[1]) * (I32)(rgiPrevVal[1]);
; iPred += (I32)(rgiFilter[2]) * (I32)(rgiPrevVal[2]) + (I32)(rgiFilter[3]) * (I32)(rgiPrevVal[3]);
; iPred += (I32)(rgiFilter[4]) * (I32)(rgiPrevVal[4]) + (I32)(rgiFilter[5]) * (I32)(rgiPrevVal[5]);
; iPred += (I32)(rgiFilter[6]) * (I32)(rgiPrevVal[6]) + (I32)(rgiFilter[7]) * (I32)(rgiPrevVal[7]);
; rgiFilter += 8;
; rgiPrevVal += 8;

; if (iPredErr > 0) 
; for (j = 0; j < iOrder; j++) {
; rgiFilter[j] +=  rgiUpdate[j];

; r9  = rgiFilter
; r12 = rgiPrevVal
; r10 = rgiUpdate

  LDRSH r2, [r12], #2
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  ADD   r1, r1, lr
  STRH  r1, [r9], #2

  LDRSH r2, [r12], #2
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  ADD   r1, r1, lr
  STRH  r1, [r9], #2

  LDRSH r2, [r12], #2
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  ADD   r1, r1, lr
  STRH  r1, [r9], #2

  LDRSH r2, [r12], #2
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  ADD   r1, r1, lr
  STRH  r1, [r9], #2

  LDRSH r2, [r12], #2
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  ADD   r1, r1, lr
  STRH  r1, [r9], #2

  LDRSH r2, [r12], #2
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  ADD   r1, r1, lr
  STRH  r1, [r9], #2

  LDRSH r2, [r12], #2
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  ADD   r1, r1, lr
  STRH  r1, [r9], #2

  LDRSH r2, [r12], #2
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  ADD   r1, r1, lr
  STRH  r1, [r9], #2

  SUBS  r3, r3, #1
  BNE   gInLoop16GT

  B     gInLoopEnd

gInLoop16LT
; for (j = 0; j < iOrder_Div_8; j++) {
; iPred += (I32)(rgiFilter[0]) * (I32)(rgiPrevVal[0]) + (I32)(rgiFilter[1]) * (I32)(rgiPrevVal[1]);
; iPred += (I32)(rgiFilter[2]) * (I32)(rgiPrevVal[2]) + (I32)(rgiFilter[3]) * (I32)(rgiPrevVal[3]);
; iPred += (I32)(rgiFilter[4]) * (I32)(rgiPrevVal[4]) + (I32)(rgiFilter[5]) * (I32)(rgiPrevVal[5]);
; iPred += (I32)(rgiFilter[6]) * (I32)(rgiPrevVal[6]) + (I32)(rgiFilter[7]) * (I32)(rgiPrevVal[7]);
; rgiFilter += 8;
; rgiPrevVal += 8;

; if (iPredErr < 0) 
; for (j = 0; j < iOrder; j++) {
; rgiFilter[j] -=  rgiUpdate[j];

; r9  = rgiFilter
; r12 = rgiPrevVal
; r10 = rgiUpdate

  LDRSH r2, [r12], #2
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  SUB   r1, r1, lr
  STRH  r1, [r9], #2

  LDRSH r2, [r12], #2
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  SUB   r1, r1, lr
  STRH  r1, [r9], #2

  LDRSH r2, [r12], #2
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  SUB   r1, r1, lr
  STRH  r1, [r9], #2

  LDRSH r2, [r12], #2
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  SUB   r1, r1, lr
  STRH  r1, [r9], #2

  LDRSH r2, [r12], #2
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  SUB   r1, r1, lr
  STRH  r1, [r9], #2

  LDRSH r2, [r12], #2
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  SUB   r1, r1, lr
  STRH  r1, [r9], #2

  LDRSH r2, [r12], #2
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  SUB   r1, r1, lr
  STRH  r1, [r9], #2

  LDRSH r2, [r12], #2
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  SUB   r1, r1, lr
  STRH  r1, [r9], #2

  SUBS  r3, r3, #1
  BNE   gInLoop16LT

gInLoopEnd
; iPredErr = (I32)*rgiResidue;
; iPred = (iPred >> iScaling);
  LDR   r1, [r4, #LMSPredictor_m_iScaling]
  LDR   r2, [r5]
  MOV   r0, r0, ASR r1

; iInput = iPredErr + iPred;
  ADD   r10,r2, r0

; if (iRecent == 0) {
  CMP   r11, #0
  BNE   gReNE16

; iRecent = iOrder - 1;
; r11 = iOrder
  LDR   r11, [r4, #LMSPredictor_m_iOrder]
  
  MOV   r9, r11
  MOV   r1, r7
  MOV   r12,r8

  ADD   r2, r7, r9, LSL #1
  ADD   lr, r8, r9, LSL #1
  
gMemcpy16
  LDR   r0, [r1], #4
  LDR   r3, [r12],#4
  STR   r0, [r2], #4
  STR   r3, [lr], #4

  SUBS  r9, r9, #2
  BNE   gMemcpy16

gReNE16
; iRecent = iRecent - 1;
  SUB   r11, r11, #1

; * rgiResidue = iInput;
; rgiResidue = rgiResidue + 1;
  STR   r10, [r5], #4

; rgiPrevVal = rgiPrevValRdBuf + iRecent;
  ADD   r1, r7, r11, LSL #1

; rgiUpdate = rgiUpdateRdBuf + iRecent;
  ADD   r2, r8, r11, LSL #1

; * rgiUpdate = iUpdStepSize_X_8;
; r0 = iUpdStepSize_X_8
  LDRSH r0, [r4, #LMSPredictor_m_iUpdStepSize_X_8]

; r3 = -1
  MVN   r3, #0

; lr = 0x7FFF
  MOV   lr, r3, LSR #17

; r12 = 0x8000
  MOV   r12,r3, LSL #15

; if (iInput > 0) {
  CMP   r10, #0

; * rgiUpdate = 0;
  MOVEQ r0, #0

; * rgiUpdate = - iUpdStepSize_X_8;
  RSBLT r0, r0, #0

; if (iInput > (I32)0X00007FFF) {
; * rgiPrevVal = (I16)0X7FFF;
  CMP   r10, lr
  MOVGT r10, lr

; if (iInput < (I32)0XFFFF8000) {
; * rgiPrevVal = (I16)0X8000;        
  CMP   r10, r12
  MOVLT r10, r12

; store rgiUpdate
  STRH  r0, [r2]

; * rgiPrevVal = ((I16)iInput);
  STRH  r10,[r1]

; can not guarantee iOrder_Div_16*2 == iOrder_Div_8
; therefore need load both iOrder_Div_16 and iOrder_Div_8

; rgiUpdate[iOrder_Div_16] >>= 2;
; rgiUpdate[iOrder_Div_8] >>= 1;
  LDR   r3, [r4, #LMSPredictor_m_iOrder_Div_16]
  LDR   r12,[r4, #LMSPredictor_m_iOrder_Div_8]

  MOV   r3, r3, LSL #1
  MOV   r12,r12,LSL #1

  LDRSH r0, [r2, r3]
  LDRSH lr, [r2, r12]

  MOV   r0, r0, ASR #2
  MOV   lr, lr, ASR #1

  STRH  r0, [r2, r3]
  STRH  lr, [r2, r12]

; r6 = iLen
  SUBS  r6, r6, #1
  BNE   gOutLoop16

; pLMSPredictor->m_iRecent = iRecent;
  STR   r11,[r4, #LMSPredictor_m_iRecent]

  LDMFD sp!, {r4 - r11, PC} ;prvDecLMSPredictor_I16_C
  ENTRY_END prvDecLMSPredictor_I16_C 


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  AREA    |.text|, CODE
  LEAF_ENTRY prvDecLMSPredictor_I32_C
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Input parameters
; r0 = pau
; r1 = pLMSPredictor
; r2 = rgiResidue
; r3 = iLen


; r4 = pLMSPredictor
; r5 = rgiResidue
; r6 = iLen

; r7 = rgiPrevValRdBuf
; r8 = rgiUpdateRdBuf

; r10= iInput
; r11= iRecent


  STMFD sp!, {r4 - r11, lr}

  LDR   r11,[r1, #LMSPredictor_m_iRecent]
  MOV   r5, r2

  LDR   r7, [r1, #LMSPredictor_m_rgiPrevVal]
  MOV   r6, r3

  LDR   r8, [r1, #LMSPredictor_m_rgiUpdate]
  MOV   r4, r1

gOutLoop32
; for (i = 0; i < iLen; i++) {
; r0 = iPred
; iPred = iScalingOffset;
  LDR   r0, [r4, #LMSPredictor_m_iScalingOffset]

; rgiFilter = rgiFilterBuf;
  LDR   r9, [r4, #LMSPredictor_m_rgiFilter]

; r3 = iOrder_Div_8
  LDR   r3, [r4, #LMSPredictor_m_iOrder_Div_8]

; iPredErr = (I32)*rgiResidue;
; r2 = iPredErr
  LDR   r2, [r5]

; rgiPrevVal = rgiPrevValRdBuf + iRecent;
; r12 = rgiPrevVal
  ADD   r12,  r7, r11, LSL #2

; rgiUpdate = rgiUpdateRdBuf + iRecent;
; r10 = rgiUpdate
  ADD   r10, r8, r11, LSL #1

  CMP   r2, #0
  BGT   gInLoop32GT
  BLT   gInLoop32LT

gInLoop32EQ
; for (j = 0; j < iOrder_Div_8; j++) {
; iPred += (I32)rgiFilter[0] * rgiPrevVal[0];
; iPred += (I32)rgiFilter[1] * rgiPrevVal[1];
; iPred += (I32)rgiFilter[2] * rgiPrevVal[2];
; iPred += (I32)rgiFilter[3] * rgiPrevVal[3];
; iPred += (I32)rgiFilter[4] * rgiPrevVal[4];
; iPred += (I32)rgiFilter[5] * rgiPrevVal[5];
; iPred += (I32)rgiFilter[6] * rgiPrevVal[6];
; iPred += (I32)rgiFilter[7] * rgiPrevVal[7];
; rgiFilter += 8;
; rgiPrevVal += 8;

; r9  = rgiFilter
; r12 = rgiPrevVal

  LDRSH r1, [r9], #2
  LDR   r2, [r12], #4
  MLA   r0, r2, r1, r0

  LDRSH r1, [r9], #2
  LDR   r2, [r12], #4
  MLA   r0, r2, r1, r0

  LDRSH r1, [r9], #2
  LDR   r2, [r12], #4
  MLA   r0, r2, r1, r0

  LDRSH r1, [r9], #2
  LDR   r2, [r12], #4
  MLA   r0, r2, r1, r0

  LDRSH r1, [r9], #2
  LDR   r2, [r12], #4
  MLA   r0, r2, r1, r0

  LDRSH r1, [r9], #2
  LDR   r2, [r12], #4
  MLA   r0, r2, r1, r0

  LDRSH r1, [r9], #2
  LDR   r2, [r12], #4
  MLA   r0, r2, r1, r0

  LDRSH r1, [r9], #2
  LDR   r2, [r12], #4
  MLA   r0, r2, r1, r0

  SUBS  r3, r3, #1
  BNE   gInLoop32EQ

  B     gInLoopEnd32

gInLoop32GT
; for (j = 0; j < iOrder_Div_8; j++) {
; iPred += (I32)rgiFilter[0] * rgiPrevVal[0];
; iPred += (I32)rgiFilter[1] * rgiPrevVal[1];
; iPred += (I32)rgiFilter[2] * rgiPrevVal[2];
; iPred += (I32)rgiFilter[3] * rgiPrevVal[3];
; iPred += (I32)rgiFilter[4] * rgiPrevVal[4];
; iPred += (I32)rgiFilter[5] * rgiPrevVal[5];
; iPred += (I32)rgiFilter[6] * rgiPrevVal[6];
; iPred += (I32)rgiFilter[7] * rgiPrevVal[7];
; rgiFilter += 8;
; rgiPrevVal += 8;

; if (iPredErr > 0) 
; for (j = 0; j < iOrder; j++) {
; rgiFilter[j] +=  rgiUpdate[j];

; r9  = rgiFilter
; r12 = rgiPrevVal
; r10 = rgiUpdate

  LDR   r2, [r12], #4
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  ADD   r1, r1, lr
  STRH  r1, [r9], #2

  LDR   r2, [r12], #4
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  ADD   r1, r1, lr
  STRH  r1, [r9], #2

  LDR   r2, [r12], #4
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  ADD   r1, r1, lr
  STRH  r1, [r9], #2

  LDR   r2, [r12], #4
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  ADD   r1, r1, lr
  STRH  r1, [r9], #2

  LDR   r2, [r12], #4
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  ADD   r1, r1, lr
  STRH  r1, [r9], #2

  LDR   r2, [r12], #4
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  ADD   r1, r1, lr
  STRH  r1, [r9], #2

  LDR   r2, [r12], #4
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  ADD   r1, r1, lr
  STRH  r1, [r9], #2

  LDR   r2, [r12], #4
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  ADD   r1, r1, lr
  STRH  r1, [r9], #2

  SUBS  r3, r3, #1
  BNE   gInLoop32GT

  B     gInLoopEnd32

gInLoop32LT
; for (j = 0; j < iOrder_Div_8; j++) {
; iPred += (I32)rgiFilter[0] * rgiPrevVal[0];
; iPred += (I32)rgiFilter[1] * rgiPrevVal[1];
; iPred += (I32)rgiFilter[2] * rgiPrevVal[2];
; iPred += (I32)rgiFilter[3] * rgiPrevVal[3];
; iPred += (I32)rgiFilter[4] * rgiPrevVal[4];
; iPred += (I32)rgiFilter[5] * rgiPrevVal[5];
; iPred += (I32)rgiFilter[6] * rgiPrevVal[6];
; iPred += (I32)rgiFilter[7] * rgiPrevVal[7];
; rgiFilter += 8;
; rgiPrevVal += 8;

; if (iPredErr > 0) 
; for (j = 0; j < iOrder; j++) {
; rgiFilter[j] -=  rgiUpdate[j];

; r9  = rgiFilter
; r12 = rgiPrevVal
; r10 = rgiUpdate

  LDR   r2, [r12], #4
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  SUB   r1, r1, lr
  STRH  r1, [r9], #2

  LDR   r2, [r12], #4
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  SUB   r1, r1, lr
  STRH  r1, [r9], #2

  LDR   r2, [r12], #4
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  SUB   r1, r1, lr
  STRH  r1, [r9], #2

  LDR   r2, [r12], #4
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  SUB   r1, r1, lr
  STRH  r1, [r9], #2

  LDR   r2, [r12], #4
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  SUB   r1, r1, lr
  STRH  r1, [r9], #2

  LDR   r2, [r12], #4
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  SUB   r1, r1, lr
  STRH  r1, [r9], #2

  LDR   r2, [r12], #4
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  SUB   r1, r1, lr
  STRH  r1, [r9], #2

  LDR   r2, [r12], #4
  LDRSH r1, [r9]
  LDRSH lr, [r10], #2
  MLA   r0, r2, r1, r0
  SUB   r1, r1, lr
  STRH  r1, [r9], #2

  SUBS  r3, r3, #1
  BNE   gInLoop32LT

gInLoopEnd32
; iPredErr = (I32)*rgiResidue;
; iPred = (iPred >> iScaling);
  LDR   r1, [r4, #LMSPredictor_m_iScaling]
  LDR   r2, [r5]
  MOV   r0, r0, ASR r1

; iInput = iPredErr + iPred;
  ADD   r10,r2, r0

; if (iRecent == 0) {
  CMP   r11, #0
  BNE   gReNE32

; iRecent = iOrder - 1;
; r11 = iOrder
  LDR   r11, [r4, #LMSPredictor_m_iOrder]

  MOV   r9, r11
  MOV   r1, r7
  MOV   r12,r8

  ADD   r2, r7, r9, LSL #2
  ADD   lr, r8, r9, LSL #1
  
gMemcpy32
  LDR   r0, [r1], #4
  LDR   r3, [r12],#4

  STR   r0, [r2], #4
  LDR   r0, [r1], #4
  STR   r3, [lr], #4
  STR   r0, [r2], #4

  SUBS  r9, r9, #2
  BNE   gMemcpy32


gReNE32
; iRecent = iRecent - 1;
  SUB   r11, r11, #1

; * rgiResidue = iInput;
; rgiResidue = rgiResidue + 1;
  STR   r10, [r5], #4
 
; rgiPrevVal = rgiPrevValRdBuf + iRecent;
  ADD   r1, r7, r11, LSL #2

; rgiUpdate = rgiUpdateRdBuf + iRecent;
  ADD   r2, r8, r11, LSL #1

; * rgiUpdate = iUpdStepSize_X_8;
; r0 = iUpdStepSize_X_8
  LDRSH r0, [r4, #LMSPredictor_m_iUpdStepSize_X_8]

; r3 = -1
  MVN   r3, #0

; lr = 0X007FFFFF
  MOV   lr, r3, LSR #9

; r12 = 0XFF800000
  MOV   r12,r3, LSL #23

; if (iInput > 0) {
  CMP   r10, #0

; * rgiUpdate = 0;
  MOVEQ r0, #0

; * rgiUpdate = - iUpdStepSize_X_8;
  RSBLT r0, r0, #0

; if (iInput > (I32)0X007FFFFF)
; * rgiPrevVal = (I32)0X007FFFFF;
  CMP   r10, lr
  MOVGT r10, lr

; if (iInput < (I32)0XFF800000)
; * rgiPrevVal = (I32)0XFF800000;
  CMP   r10, r12
  MOVLT r10, r12
            
; store rgiUpdate
  STRH  r0, [r2]

; * rgiPrevVal = ((I32)iInput); 
  STR   r10,[r1]

; can not guarantee iOrder_Div_16*2 == iOrder_Div_8
; therefore need load both iOrder_Div_16 and iOrder_Div_8

; rgiUpdate[iOrder_Div_16] >>= 2;
; rgiUpdate[iOrder_Div_8] >>= 1;
  LDR   r3, [r4, #LMSPredictor_m_iOrder_Div_16]
  LDR   r12,[r4, #LMSPredictor_m_iOrder_Div_8]

  MOV   r3, r3, LSL #1
  MOV   r12,r12,LSL #1

  LDRSH r0, [r2, r3]
  LDRSH lr, [r2, r12]

  MOV   r0, r0, ASR #2
  MOV   lr, lr, ASR #1

  STRH  r0, [r2, r3]
  STRH  lr, [r2, r12]

; r6 = iLen
  SUBS  r6, r6, #1
  BNE   gOutLoop32

; pLMSPredictor->m_iRecent = iRecent;
  STR   r11,[r4, #LMSPredictor_m_iRecent]

  LDMFD sp!, {r4 - r11, PC} ;prvDecLMSPredictor_I32_C
  ENTRY_END prvDecLMSPredictor_I32_C
	
  ENDIF ; WMA_OPT_LOSSLESSDECPROLSL_ARM
    
  END