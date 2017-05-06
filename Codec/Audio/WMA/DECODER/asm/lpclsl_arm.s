;//*@@@+++@@@@******************************************************************
;//
;// Microsoft Windows Media
;// Copyright (C) Microsoft Corporation. All rights reserved.
;//
;//*@@@---@@@@******************************************************************
;// Module Name:
;//
;//     lpclsl_arm.s
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
;//     Jerry He (yamihe) Sep 19th, 2003
;//
;// Revision History:
;//
;//     For more information on ARM assembler directives, use
;//        http://msdn.microsoft.com/library/default.asp?url=/library/en-us/wcechp40/html/ccconarmassemblerdirectives.asp
;//*************************************************************************


  OPT         2       ; disable listing 
  INCLUDE     kxarm.h
  INCLUDE     wma_member_arm.inc
  INCLUDE	  wma_arm_version.h
  OPT         1       ; enable listing

 
  AREA    |.text|, CODE, READONLY


  IF WMA_OPT_LPCLSL_ARM = 1
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  EXPORT  prvDecMCLMSPredictorPredAndUpdate_I16
  EXPORT  prvDecMCLMSPredictorPredAndUpdate_I32


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  AREA    |.text|, CODE
  LEAF_ENTRY prvDecMCLMSPredictorPredAndUpdate_I16
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Input parameters
; r0 = pau
; r1 = pLMSPredictor
; r2 = rgiInput
; r3 = rgiPred

; r4 = piFilter
; r5 = piPrevVal
; r6 = piUpdate
; r7 = piFilterCurrTime

; r8 = pau->m_cChannel
; r9 = pMCLMSPredictor->m_iOrder_X_CH
; r10= iPred


  STMFD sp!, {r4 - r11, lr}

; r12= pMCLMSPredictor->m_iRecent
  LDR   r12,[r1, #MCLMSPredictor_m_iRecent]

; I16 * piFilter = pMCLMSPredictor->m_rgiFilter;
  LDR   r4, [r1, #MCLMSPredictor_m_rgiFilter]

; I16 * piFilterCurrTime = pMCLMSPredictor->m_rgiFilterCurrTime;
  LDR   r7, [r1, #MCLMSPredictor_m_rgiFilterCurrTime]

; I16 * piPrevVal = (I16 *)pMCLMSPredictor->m_rgiPrevVal + pMCLMSPredictor->m_iRecent;
; I16 * piUpdate = pMCLMSPredictor->m_rgiUpdate + pMCLMSPredictor->m_iRecent;
  LDR   r5, [r1, #MCLMSPredictor_m_rgiPrevVal]
  LDR   r6, [r1, #MCLMSPredictor_m_rgiUpdate]

; r8 = m_cChannel
  LDRH  r8, [r0, #CAudioObject_m_cChannel]
  
  ADD   r5, r5, r12, LSL #1
  ADD   r6, r6, r12, LSL #1

; r0 = outloop counter
  MOV   r0, #0

; for (j = 0; j < pau->m_cChannel; j++) {
gOutLoop16

; r9 = pMCLMSPredictor->m_iOrder_X_CH
  LDR   r9, [r1, #MCLMSPredictor_m_iOrder_X_CH]

; if (rgiPred[j] == 0) { // decoder : this channel has zero power.
  LDR   lr, [r3]
  
  	CMP		lr, #0
  	BNE   	gInLoop16
	
; piFilter += pMCLMSPredictor->m_iOrder_X_CH;
  ADD   r4, r4, r9, LSL #1

; continue;
  ADD   r3, r3, #4
  B     gOutLoopTest16    

gInLoop16
; iPred = (pMCLMSPredictor->m_iScalingOffset);
  LDR   r10, [r1, #MCLMSPredictor_m_iScalingOffset]

; lr = rgiInput[j]
  LDR   lr, [r2, r0, LSL #2]
  CMP   lr, #0

  BGT   gInLoopGT16
  BLT   gInLoopLT16

; r0 = pMCLMSPredictor->m_iOrder_X_CH
  MOV   lr, r9

; else { // (rgiInput[j] == 0)
gInLoopEQ16

; for (i = 0; i < pMCLMSPredictor->m_iOrder_X_CH; i++) {
; iPred += ((I32)(*(piFilter + i)) * (I32)(*(piPrevVal + i)));

; r4 = piFilter
; r5 = piPrevVal

  LDRSH r11, [r5], #2
  LDRSH r12, [r4], #2
  MLA   r10, r11, r12, r10

  SUBS  lr, lr, #1
  BNE   gInLoopEQ16

  MOV   lr, r0

; r5 = piPrevVal, restore point
  SUB   r5, r5, r9, LSL #1 

g2ndInLoopEQ
; for (i = j-1; i >= 0; i--) {
  SUBS  lr, lr, #1
  BMI   gInLoopEnd16

; iPred += (I32)rgiInput[i] * (I32)(*(piFilterCurrTime + i));
  MOV   r12, lr, LSL #1
  LDR   r11, [r2, lr, LSL #2]
  LDRSH r12, [r7, r12]

  MLA   r10, r11, r12, r10
  B     g2ndInLoopEQ


; if (rgiInput[j] > 0){
gInLoopGT16

; for (i = 0; i < pMCLMSPredictor->m_iOrder_X_CH; i++) {
; iPred += ((I32)(*(piFilter + i)) * (I32)(*(piPrevVal + i)));
; (*(piFilter + i)) += (*(piUpdate + i));

; r4 = piFilter
; r5 = piPrevVal
; r6 = piUpdate

; unloop by 2
  LDRSH r11, [r5], #2
  LDRSH r12, [r4]
  LDRSH lr,  [r6], #2
  MLA   r10, r11, r12, r10
  ADD   r12, r12, lr
  STRH  r12, [r4], #2

  LDRSH r11, [r5], #2
  LDRSH r12, [r4]
  LDRSH lr,  [r6], #2
  MLA   r10, r11, r12, r10
  ADD   r12, r12, lr
  STRH  r12, [r4], #2

  SUBS  r9, r9, #2
  BNE   gInLoopGT16

  MOV   lr, r0

; r9 = pMCLMSPredictor->m_iUpdStepSize
  LDRSH r9,  [r1, #MCLMSPredictor_m_iUpdStepSize]

g2ndInLoopGT
; for (i = j-1; i >= 0; i--) {
  SUBS  lr, lr, #1
  BMI   gInLoopRestore16

; iPred += (I32)rgiInput[i] * (I32)(*(piFilterCurrTime + i));
  MOV   r12, lr, LSL #1
  LDR   r11, [r2, lr, LSL #2]
  LDRSH r12, [r7, r12]

  CMP   r11, #0
  MLA   r10, r11, r12, r10

; if (rgiInput[i] > 0)
; (*(piFilterCurrTime + i)) += pMCLMSPredictor->m_iUpdStepSize;
  ADDGT r12, r12, r9

  MOV   r11, lr, LSL #1

; else if (rgiInput[i] < 0)
; (*(piFilterCurrTime + i)) -= pMCLMSPredictor->m_iUpdStepSize;
  SUBLT r12, r12, r9

  STRH  r12, [r7, r11]
  B     g2ndInLoopGT


; else if (rgiInput[j] < 0){
gInLoopLT16

; for (i = 0; i < pMCLMSPredictor->m_iOrder_X_CH; i++) {
; iPred += ((I32)(*(piFilter + i)) * (I32)(*(piPrevVal + i)));
; (*(piFilter + i)) -= (*(piUpdate + i));
; r4 = piFilter
; r5 = piPrevVal
; r6 = piUpdate

; unloop by 2
  LDRSH r11, [r5], #2
  LDRSH r12, [r4]
  LDRSH lr,  [r6], #2
  MLA   r10, r11, r12, r10
  SUB   r12, r12, lr
  STRH  r12, [r4], #2

  LDRSH r11, [r5], #2
  LDRSH r12, [r4]
  LDRSH lr,  [r6], #2
  MLA   r10, r11, r12, r10
  SUB   r12, r12, lr
  STRH  r12, [r4], #2

  SUBS  r9, r9, #2
  BNE   gInLoopLT16

  MOV   lr, r0

; r9 = pMCLMSPredictor->m_iUpdStepSize
  LDRSH r9,  [r1, #MCLMSPredictor_m_iUpdStepSize]

g2ndInLoopLT
; for (i = j-1; i >= 0; i--) {
  SUBS  lr, lr, #1
  BMI   gInLoopRestore16

; iPred += (I32)rgiInput[i] * (I32)(*(piFilterCurrTime + i));
  MOV   r12, lr, LSL #1
  LDR   r11, [r2, lr, LSL #2]
  LDRSH r12, [r7, r12]

  CMP   r11, #0
  MLA   r10, r11, r12, r10

; if (rgiInput[i] > 0)
; (*(piFilterCurrTime + i)) -= pMCLMSPredictor->m_iUpdStepSize;
  SUBGT r12, r12, r9

  MOV   r11, lr, LSL #1

; else if (rgiInput[i] < 0)
; (*(piFilterCurrTime + i)) += pMCLMSPredictor->m_iUpdStepSize;
  ADDLT r12, r12, r9

  STRH  r12, [r7, r11]
  B     g2ndInLoopLT

gInLoopRestore16
; r9 = pMCLMSPredictor->m_iOrder_X_CH
  LDR   r9, [r1, #MCLMSPredictor_m_iOrder_X_CH]

; r5 = piPrevVal, restore point
  SUB   r5, r5, r9, LSL #1 

; r6 = piUpdate, restore point
  SUB   r6, r6, r9, LSL #1 

gInLoopEnd16
; pMCLMSPredictor->m_iScaling
  LDR   r11, [r1, #MCLMSPredictor_m_iScaling]

; r12 = rgiInput[j]
  LDR   r12, [r2, r0, LSL #2]

; iPred = iPred >> (pMCLMSPredictor->m_iScaling);
  MOV   r10, r10, ASR r11

; rgiInput[j] = rgiInput[j] + iPred;
  ADD   r12, r12, r10

; rgiPred[j] = iPred;
  STR   r10, [r3], #4
  STR   r12, [r2, r0, LSL #2]  

gOutLoopTest16  
; piFilterCurrTime += pau->m_cChannel;
  ADD   r7,  r7, r8, LSL #1
 
  ADD   r0, r0, #1
  CMP   r0, r8
  BLT   gOutLoop16

; r0 = -1
  MVN   r0, #0

; r11 = 0x7FFF, pau->m_iSampleMaxValue
  MOV   r11, r0, LSR #17

; r12 = 0x8000, pau->m_iSampleMinValue
  MOV   r12, r0, LSL #15

; r4 = pMCLMSPredictor->m_iRecent
  LDR   r4, [r1, #MCLMSPredictor_m_iRecent]

; piPrevVal = (I16 *)pMCLMSPredictor->m_rgiPrevVal;
; r5 = piPrevVal
  LDR   r5, [r1, #MCLMSPredictor_m_rgiPrevVal]

; r6 = pMCLMSPredictor->m_rgiUpdate
  LDR   r6, [r1, #MCLMSPredictor_m_rgiUpdate]

  SUB   r8, r8, #1

; r0 = pMCLMSPredictor->m_iUpdStepSize
  LDRSH r0,  [r1, #MCLMSPredictor_m_iUpdStepSize]

; for (j = pau->m_cChannel - 1; j >= 0; j--) {
g2ndOutLoop16

; pMCLMSPredictor->m_iRecent = pMCLMSPredictor->m_iRecent - 1;
  SUB   r4, r4, #1

; if (rgiInput[j] > pau->m_iSampleMaxValue)
;    piPrevVal[pMCLMSPredictor->m_iRecent] = (I16)pau->m_iSampleMaxValue;
; else if (rgiInput[j] < pau->m_iSampleMinValue)
;    piPrevVal[pMCLMSPredictor->m_iRecent] = (I16)pau->m_iSampleMinValue;
; else
;    piPrevVal[pMCLMSPredictor->m_iRecent] = (I16)rgiInput[j];

; lr = rgiInput[j]
  LDR   lr, [r2, r8, LSL #2]

  CMP   lr, r11
  MOVGT lr, r11

  CMP   lr, r12
  MOVLT lr, r12

  MOV   r7, r4, LSL #1
  STRH  lr, [r5, r7]

; if (rgiInput[j] > 0)
;    pMCLMSPredictor->m_rgiUpdate[pMCLMSPredictor->m_iRecent] = pMCLMSPredictor->m_iUpdStepSize;
; else if (rgiInput[j] < 0)
;    pMCLMSPredictor->m_rgiUpdate[pMCLMSPredictor->m_iRecent] = -pMCLMSPredictor->m_iUpdStepSize;
; else
;    pMCLMSPredictor->m_rgiUpdate[pMCLMSPredictor->m_iRecent] = 0;

  CMP   lr, #0
  MOVGT lr, r0
  RSBLT lr, r0, #0
  MOVEQ lr, #0

  STRH  lr, [r6, r7]

  SUBS  r8, r8, #1
  BGE   g2ndOutLoop16

; r7 = pLMSPredictor
  MOV   r7, r1

; if (pMCLMSPredictor->m_iRecent == 0) {
  CMP   r4, #0
  BNE   gEnd16

; pMCLMSPredictor->m_iRecent = pMCLMSPredictor->m_iOrder_X_CH;
  MOV   r4, r9

  ADD   r2, r5, r9, LSL #1
  ADD   lr, r6, r9, LSL #1
  
gMemcpy16
  LDR   r0, [r5], #4
  LDR   r3, [r6], #4
  STR   r0, [r2], #4
  STR   r3, [lr], #4

  SUBS  r9, r9, #2
  BNE   gMemcpy16

gEnd16
; r4 = pMCLMSPredictor->m_iRecent
  STR   r4, [r7, #MCLMSPredictor_m_iRecent]

  LDMFD sp!, {r4 - r11, PC} ;prvDecMCLMSPredictorPredAndUpdate_I16
  ENTRY_END prvDecMCLMSPredictorPredAndUpdate_I16


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  AREA    |.text|, CODE
  LEAF_ENTRY prvDecMCLMSPredictorPredAndUpdate_I32
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Input parameters
; r0 = pau
; r1 = pLMSPredictor
; r2 = rgiInput
; r3 = rgiPred

; r4 = piFilter
; r5 = piPrevVal
; r6 = piUpdate
; r7 = piFilterCurrTime

; r8 = pau->m_cChannel
; r9 = pMCLMSPredictor->m_iOrder_X_CH
; r10= iPred


  STMFD sp!, {r4 - r11, lr}

; r12= pMCLMSPredictor->m_iRecent
  LDR   r12,[r1, #MCLMSPredictor_m_iRecent]

; I16 * piFilter = pMCLMSPredictor->m_rgiFilter;
  LDR   r4, [r1, #MCLMSPredictor_m_rgiFilter]

; I16 * piFilterCurrTime = pMCLMSPredictor->m_rgiFilterCurrTime;
  LDR   r7, [r1, #MCLMSPredictor_m_rgiFilterCurrTime]

; I16 * piPrevVal = (I16 *)pMCLMSPredictor->m_rgiPrevVal + pMCLMSPredictor->m_iRecent;
; I16 * piUpdate = pMCLMSPredictor->m_rgiUpdate + pMCLMSPredictor->m_iRecent;
  LDR   r5, [r1, #MCLMSPredictor_m_rgiPrevVal]
  LDR   r6, [r1, #MCLMSPredictor_m_rgiUpdate]

; r8 = m_cChannel
  LDRH  r8, [r0, #CAudioObject_m_cChannel]
  
  ADD   r5, r5, r12, LSL #2
  ADD   r6, r6, r12, LSL #1

; r0 = outloop counter
  MOV   r0, #0

; for (j = 0; j < pau->m_cChannel; j++) {
gOutLoop32

; r9 = pMCLMSPredictor->m_iOrder_X_CH
  LDR   r9, [r1, #MCLMSPredictor_m_iOrder_X_CH]

; if (rgiPred[j] == 0) { // decoder : this channel has zero power.
  LDR   lr, [r3]
  CMP   lr, #0
  BNE   gInLoop32

; piFilter += pMCLMSPredictor->m_iOrder_X_CH;
  ADD   r4, r4, r9, LSL #1

; continue;
  ADD   r3, r3, #4
  B     gOutLoopTest32    

gInLoop32
; iPred = (pMCLMSPredictor->m_iScalingOffset);
  LDR   r10, [r1, #MCLMSPredictor_m_iScalingOffset]

; lr = rgiInput[j]
  LDR   lr, [r2, r0, LSL #2]
  CMP   lr, #0

  BGT   gInLoopGT32
  BLT   gInLoopLT32

; r0 = pMCLMSPredictor->m_iOrder_X_CH
  MOV   lr, r9

; else { // (rgiInput[j] == 0)
gInLoopEQ32

; for (i = 0; i < pMCLMSPredictor->m_iOrder_X_CH; i++) {
; iPred += ((I32)(*(piFilter + i)) * (*(piPrevVal + i)));

; r4 = piFilter
; r5 = piPrevVal

  LDR   r11, [r5], #4
  LDRSH r12, [r4], #2
  MLA   r10, r11, r12, r10

  SUBS  lr, lr, #1
  BNE   gInLoopEQ32

  MOV   lr, r0

; r5 = piPrevVal, restore point
  SUB   r5, r5, r9, LSL #2 

g2ndInLoopEQ32
; for (i = j-1; i >= 0; i--) {
  SUBS  lr, lr, #1
  BMI   gInLoopEnd32

; iPred += (I32)rgiInput[i] * (I32)(*(piFilterCurrTime + i));
  MOV   r12, lr, LSL #1
  LDR   r11, [r2, lr, LSL #2]
  LDRSH r12, [r7, r12]

  MLA   r10, r11, r12, r10
  B     g2ndInLoopEQ32


; if (rgiInput[j] > 0){
gInLoopGT32

; for (i = 0; i < pMCLMSPredictor->m_iOrder_X_CH; i++) {
; iPred += ((I32)(*(piFilter + i)) * (*(piPrevVal + i)));
; (*(piFilter + i)) += (*(piUpdate + i));

; r4 = piFilter
; r5 = piPrevVal
; r6 = piUpdate

; unloop by 2
  LDR   r11, [r5], #4
  LDRSH r12, [r4]
  LDRSH lr,  [r6], #2
  MLA   r10, r11, r12, r10
  ADD   r12, r12, lr
  STRH  r12, [r4], #2

  LDR   r11, [r5], #4
  LDRSH r12, [r4]
  LDRSH lr,  [r6], #2
  MLA   r10, r11, r12, r10
  ADD   r12, r12, lr
  STRH  r12, [r4], #2

  SUBS  r9, r9, #2
  BNE   gInLoopGT32

  MOV   lr, r0

; r9 = pMCLMSPredictor->m_iUpdStepSize
  LDRSH r9,  [r1, #MCLMSPredictor_m_iUpdStepSize]

g2ndInLoopGT32
; for (i = j-1; i >= 0; i--) {
  SUBS  lr, lr, #1
  BMI   gInLoopRestore32

; iPred += (I32)rgiInput[i] * (I32)(*(piFilterCurrTime + i));
  MOV   r12, lr, LSL #1
  LDR   r11, [r2, lr, LSL #2]
  LDRSH r12, [r7, r12]

  CMP   r11, #0
  MLA   r10, r11, r12, r10

; if (rgiInput[i] > 0)
; (*(piFilterCurrTime + i)) += pMCLMSPredictor->m_iUpdStepSize;
  ADDGT r12, r12, r9

  MOV   r11, lr, LSL #1

; else if (rgiInput[i] < 0)
; (*(piFilterCurrTime + i)) -= pMCLMSPredictor->m_iUpdStepSize;
  SUBLT r12, r12, r9

  STRH  r12, [r7, r11]
  B     g2ndInLoopGT32


; else if (rgiInput[j] < 0){
gInLoopLT32

; for (i = 0; i < pMCLMSPredictor->m_iOrder_X_CH; i++) {
; iPred += ((I32)(*(piFilter + i)) * (*(piPrevVal + i)));
; (*(piFilter + i)) -= (*(piUpdate + i));
; r4 = piFilter
; r5 = piPrevVal
; r6 = piUpdate

; unloop by 2
  LDR   r11, [r5], #4
  LDRSH r12, [r4]
  LDRSH lr,  [r6], #2
  MLA   r10, r11, r12, r10
  SUB   r12, r12, lr
  STRH  r12, [r4], #2

  LDR   r11, [r5], #4
  LDRSH r12, [r4]
  LDRSH lr,  [r6], #2
  MLA   r10, r11, r12, r10
  SUB   r12, r12, lr
  STRH  r12, [r4], #2

  SUBS  r9, r9, #2
  BNE   gInLoopLT32

  MOV   lr, r0

; r9 = pMCLMSPredictor->m_iUpdStepSize
  LDRSH r9,  [r1, #MCLMSPredictor_m_iUpdStepSize]

g2ndInLoopLT32
; for (i = j-1; i >= 0; i--) {
  SUBS  lr, lr, #1
  BMI   gInLoopRestore32

; iPred += (I32)rgiInput[i] * (I32)(*(piFilterCurrTime + i));
  MOV   r12, lr, LSL #1
  LDR   r11, [r2, lr, LSL #2]
  LDRSH r12, [r7, r12]

  CMP   r11, #0
  MLA   r10, r11, r12, r10

; if (rgiInput[i] > 0)
; (*(piFilterCurrTime + i)) -= pMCLMSPredictor->m_iUpdStepSize;
  SUBGT r12, r12, r9

  MOV   r11, lr, LSL #1

; else if (rgiInput[i] < 0)
; (*(piFilterCurrTime + i)) += pMCLMSPredictor->m_iUpdStepSize;
  ADDLT r12, r12, r9

  STRH  r12, [r7, r11]
  B     g2ndInLoopLT32

gInLoopRestore32
; r9 = pMCLMSPredictor->m_iOrder_X_CH
  LDR   r9, [r1, #MCLMSPredictor_m_iOrder_X_CH]

; r5 = piPrevVal, restore point
  SUB   r5, r5, r9, LSL #2 

; r6 = piUpdate, restore point
  SUB   r6, r6, r9, LSL #1 

gInLoopEnd32
; pMCLMSPredictor->m_iScaling
  LDR   r11, [r1, #MCLMSPredictor_m_iScaling]

; r12 = rgiInput[j]
  LDR   r12, [r2, r0, LSL #2]

; iPred = iPred >> (pMCLMSPredictor->m_iScaling);
  MOV   r10, r10, ASR r11

; rgiInput[j] = rgiInput[j] + iPred;
  ADD   r12, r12, r10

; rgiPred[j] = iPred;
  STR   r10, [r3], #4
  STR   r12, [r2, r0, LSL #2]  

gOutLoopTest32  
; piFilterCurrTime += pau->m_cChannel;
  ADD   r7,  r7, r8, LSL #1
 
  ADD   r0, r0, #1
  CMP   r0, r8
  BLT   gOutLoop32

; r0 = -1
  MVN   r0, #0

; r11 = 0X007FFFFF, pau->m_iSampleMaxValue
  MOV   r11, r0, LSR #9

; r12 = 0XFF800000, pau->m_iSampleMinValue
  MOV   r12, r0, LSL #23

; r4 = pMCLMSPredictor->m_iRecent
  LDR   r4, [r1, #MCLMSPredictor_m_iRecent]

; piPrevVal = (I16 *)pMCLMSPredictor->m_rgiPrevVal;
; r5 = piPrevVal
  LDR   r5, [r1, #MCLMSPredictor_m_rgiPrevVal]

; r6 = pMCLMSPredictor->m_rgiUpdate
  LDR   r6, [r1, #MCLMSPredictor_m_rgiUpdate]

  SUB   r8, r8, #1

; r0 = pMCLMSPredictor->m_iUpdStepSize
  LDRSH r0,  [r1, #MCLMSPredictor_m_iUpdStepSize]

; for (j = pau->m_cChannel - 1; j >= 0; j--) {
g2ndOutLoop32

; pMCLMSPredictor->m_iRecent = pMCLMSPredictor->m_iRecent - 1;
  SUB   r4, r4, #1

; if (rgiInput[j] > pau->m_iSampleMaxValue)
;    piPrevVal[pMCLMSPredictor->m_iRecent] = (I32)pau->m_iSampleMaxValue;
; else if (rgiInput[j] < pau->m_iSampleMinValue)
;    piPrevVal[pMCLMSPredictor->m_iRecent] = (I32)pau->m_iSampleMinValue;
; else
;    piPrevVal[pMCLMSPredictor->m_iRecent] = (I32)rgiInput[j];

; lr = rgiInput[j]
  LDR   lr, [r2, r8, LSL #2]

  CMP   lr, r11
  MOVGT lr, r11

  CMP   lr, r12
  MOVLT lr, r12

  STR   lr, [r5, r4, LSL #2]

; if (rgiInput[j] > 0)
;    pMCLMSPredictor->m_rgiUpdate[pMCLMSPredictor->m_iRecent] = pMCLMSPredictor->m_iUpdStepSize;
; else if (rgiInput[j] < 0)
;    pMCLMSPredictor->m_rgiUpdate[pMCLMSPredictor->m_iRecent] = -pMCLMSPredictor->m_iUpdStepSize;
; else
;    pMCLMSPredictor->m_rgiUpdate[pMCLMSPredictor->m_iRecent] = 0;


  MOV   r7, r4, LSL #1

  CMP   lr, #0
  MOVGT lr, r0
  RSBLT lr, r0, #0
  MOVEQ lr, #0

  STRH  lr, [r6, r7]

  SUBS  r8, r8, #1
  BGE   g2ndOutLoop32

; r7 = pLMSPredictor
  MOV   r7, r1

; if (pMCLMSPredictor->m_iRecent == 0) {
  CMP   r4, #0
  BNE   gEnd32

; pMCLMSPredictor->m_iRecent = pMCLMSPredictor->m_iOrder_X_CH;
  MOV   r4, r9

  ADD   r2, r5, r9, LSL #2
  ADD   lr, r6, r9, LSL #1
  
gMemcpy32
  LDR   r0, [r5], #4
  LDR   r3, [r6], #4

  STR   r0, [r2], #4
  LDR   r0, [r5], #4
  STR   r3, [lr], #4
  STR   r0, [r2], #4

  SUBS  r9, r9, #2
  BNE   gMemcpy32

gEnd32
; r4 = pMCLMSPredictor->m_iRecent
  STR   r4, [r7, #MCLMSPredictor_m_iRecent]

  LDMFD sp!, {r4 - r11, PC} ;prvDecMCLMSPredictorPredAndUpdate_I32
  ENTRY_END prvDecMCLMSPredictorPredAndUpdate_I32


  ENDIF ; WMA_OPT_LPCLSL_ARM
    
  END