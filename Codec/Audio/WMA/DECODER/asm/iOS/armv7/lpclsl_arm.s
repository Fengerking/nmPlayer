@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************
@// Module Name:
@//
@//     lpclsl_arm.s
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
@//     Jerry He (yamihe) Sep 19th, 2003
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


  .if WMA_OPT_LPCLSL_ARM == 1
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  .globl  _prvDecMCLMSPredictorPredAndUpdate_I16
  .globl  _prvDecMCLMSPredictorPredAndUpdate_I32


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  @AREA    |.text|, CODE
_prvDecMCLMSPredictorPredAndUpdate_I16:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pau
@ r1 = pLMSPredictor
@ r2 = rgiInput
@ r3 = rgiPred

@ r4 = piFilter
@ r5 = piPrevVal
@ r6 = piUpdate
@ r7 = piFilterCurrTime

@ r8 = pau->m_cChannel
@ r9 = pMCLMSPredictor->m_iOrder_X_CH
@ r10= iPred


  stmfd sp!, {r4 - r11, lr}

@ r12= pMCLMSPredictor->m_iRecent
  ldr   r12,[r1, #MCLMSPredictor_m_iRecent]

@ I16 * piFilter = pMCLMSPredictor->m_rgiFilter;
  ldr   r4, [r1, #MCLMSPredictor_m_rgiFilter]

@ I16 * piFilterCurrTime = pMCLMSPredictor->m_rgiFilterCurrTime;
  ldr   r7, [r1, #MCLMSPredictor_m_rgiFilterCurrTime]

@ I16 * piPrevVal = (I16 *)pMCLMSPredictor->m_rgiPrevVal + pMCLMSPredictor->m_iRecent;
@ I16 * piUpdate = pMCLMSPredictor->m_rgiUpdate + pMCLMSPredictor->m_iRecent;
  ldr   r5, [r1, #MCLMSPredictor_m_rgiPrevVal]
  ldr   r6, [r1, #MCLMSPredictor_m_rgiUpdate]

@ r8 = m_cChannel
  ldrh  r8, [r0, #CAudioObject_m_cChannel]
  
  add   r5, r5, r12, LSL #1
  add   r6, r6, r12, LSL #1

@ r0 = outloop counter
  mov   r0, #0

@ for (j = 0; j < pau->m_cChannel; j++) {
gOutLoop16:

@ r9 = pMCLMSPredictor->m_iOrder_X_CH
  ldr   r9, [r1, #MCLMSPredictor_m_iOrder_X_CH]

@ if (rgiPred[j] == 0) { // decoder : this channel has zero power.
  ldr   lr, [r3]
  
  	cmp		lr, #0
  	bne   	gInLoop16
	
@ piFilter += pMCLMSPredictor->m_iOrder_X_CH;
  add   r4, r4, r9, LSL #1

@ continue;
  add   r3, r3, #4
  b     gOutLoopTest16    

gInLoop16:
@ iPred = (pMCLMSPredictor->m_iScalingOffset)@
  ldr   r10, [r1, #MCLMSPredictor_m_iScalingOffset]

@ lr = rgiInput[j]
  ldr   lr, [r2, r0, LSL #2]
  cmp   lr, #0

  bgt   gInLoopGT16
  blt   gInLoopLT16

@ r0 = pMCLMSPredictor->m_iOrder_X_CH
  mov   lr, r9

@ else { // (rgiInput[j] == 0)
gInLoopEQ16:

@ for (i = 0; i < pMCLMSPredictor->m_iOrder_X_CH; i++) {
@ iPred += ((I32)(*(piFilter + i)) * (I32)(*(piPrevVal + i)))@

@ r4 = piFilter
@ r5 = piPrevVal

  ldrsh r11, [r5], #2
  ldrsh r12, [r4], #2
  mla   r10, r11, r12, r10

  subs  lr, lr, #1
  bne   gInLoopEQ16

  mov   lr, r0

@ r5 = piPrevVal, restore point
  sub   r5, r5, r9, LSL #1 

g2ndInLoopEQ:
@ for (i = j-1; i >= 0; i--) {
  subs  lr, lr, #1
  bmi   gInLoopEnd16

@ iPred += (I32)rgiInput[i] * (I32)(*(piFilterCurrTime + i))@
  mov   r12, lr, LSL #1
  ldr   r11, [r2, lr, LSL #2]
  ldrsh r12, [r7, r12]

  mla   r10, r11, r12, r10
  b     g2ndInLoopEQ


@ if (rgiInput[j] > 0){
gInLoopGT16:

@ for (i = 0; i < pMCLMSPredictor->m_iOrder_X_CH; i++) {
@ iPred += ((I32)(*(piFilter + i)) * (I32)(*(piPrevVal + i)))@
@ (*(piFilter + i)) += (*(piUpdate + i))@

@ r4 = piFilter
@ r5 = piPrevVal
@ r6 = piUpdate

@ unloop by 2
  ldrsh r11, [r5], #2
  ldrsh r12, [r4]
  ldrsh lr,  [r6], #2
  mla   r10, r11, r12, r10
  add   r12, r12, lr
  strh  r12, [r4], #2

  ldrsh r11, [r5], #2
  ldrsh r12, [r4]
  ldrsh lr,  [r6], #2
  mla   r10, r11, r12, r10
  add   r12, r12, lr
  strh  r12, [r4], #2

  subs  r9, r9, #2
  bne   gInLoopGT16

  mov   lr, r0

@ r9 = pMCLMSPredictor->m_iUpdStepSize
  ldrsh r9,  [r1, #MCLMSPredictor_m_iUpdStepSize]

g2ndInLoopGT:
@ for (i = j-1; i >= 0; i--) {
  subs  lr, lr, #1
  bmi   gInLoopRestore16

@ iPred += (I32)rgiInput[i] * (I32)(*(piFilterCurrTime + i))@
  mov   r12, lr, LSL #1
  ldr   r11, [r2, lr, LSL #2]
  ldrsh r12, [r7, r12]

  cmp   r11, #0
  mla   r10, r11, r12, r10

@ if (rgiInput[i] > 0)
@ (*(piFilterCurrTime + i)) += pMCLMSPredictor->m_iUpdStepSize;
  addgt r12, r12, r9

  mov   r11, lr, LSL #1

@ else if (rgiInput[i] < 0)
@ (*(piFilterCurrTime + i)) -= pMCLMSPredictor->m_iUpdStepSize;
  sublt r12, r12, r9

  strh  r12, [r7, r11]
  b     g2ndInLoopGT


@ else if (rgiInput[j] < 0){
gInLoopLT16:

@ for (i = 0; i < pMCLMSPredictor->m_iOrder_X_CH; i++) {
@ iPred += ((I32)(*(piFilter + i)) * (I32)(*(piPrevVal + i)))@
@ (*(piFilter + i)) -= (*(piUpdate + i))@
@ r4 = piFilter
@ r5 = piPrevVal
@ r6 = piUpdate

@ unloop by 2
  ldrsh r11, [r5], #2
  ldrsh r12, [r4]
  ldrsh lr,  [r6], #2
  mla   r10, r11, r12, r10
  sub   r12, r12, lr
  strh  r12, [r4], #2

  ldrsh r11, [r5], #2
  ldrsh r12, [r4]
  ldrsh lr,  [r6], #2
  mla   r10, r11, r12, r10
  sub   r12, r12, lr
  strh  r12, [r4], #2

  subs  r9, r9, #2
  bne   gInLoopLT16

  mov   lr, r0

@ r9 = pMCLMSPredictor->m_iUpdStepSize
  ldrsh r9,  [r1, #MCLMSPredictor_m_iUpdStepSize]

g2ndInLoopLT:
@ for (i = j-1; i >= 0; i--) {
  subs  lr, lr, #1
  bmi   gInLoopRestore16

@ iPred += (I32)rgiInput[i] * (I32)(*(piFilterCurrTime + i))@
  mov   r12, lr, LSL #1
  ldr   r11, [r2, lr, LSL #2]
  ldrsh r12, [r7, r12]

  cmp   r11, #0
  mla   r10, r11, r12, r10

@ if (rgiInput[i] > 0)
@ (*(piFilterCurrTime + i)) -= pMCLMSPredictor->m_iUpdStepSize;
  subgt r12, r12, r9

  mov   r11, lr, LSL #1

@ else if (rgiInput[i] < 0)
@ (*(piFilterCurrTime + i)) += pMCLMSPredictor->m_iUpdStepSize;
  addlt r12, r12, r9

  strh  r12, [r7, r11]
  b     g2ndInLoopLT

gInLoopRestore16:
@ r9 = pMCLMSPredictor->m_iOrder_X_CH
  ldr   r9, [r1, #MCLMSPredictor_m_iOrder_X_CH]

@ r5 = piPrevVal, restore point
  sub   r5, r5, r9, LSL #1 

@ r6 = piUpdate, restore point
  sub   r6, r6, r9, LSL #1 

gInLoopEnd16:
@ pMCLMSPredictor->m_iScaling
  ldr   r11, [r1, #MCLMSPredictor_m_iScaling]

@ r12 = rgiInput[j]
  ldr   r12, [r2, r0, LSL #2]

@ iPred = iPred >> (pMCLMSPredictor->m_iScaling)@
  mov   r10, r10, ASR r11

@ rgiInput[j] = rgiInput[j] + iPred;
  add   r12, r12, r10

@ rgiPred[j] = iPred;
  str   r10, [r3], #4
  str   r12, [r2, r0, LSL #2]  

gOutLoopTest16:
@ piFilterCurrTime += pau->m_cChannel;
  add   r7,  r7, r8, LSL #1
 
  add   r0, r0, #1
  cmp   r0, r8
  blt   gOutLoop16

@ r0 = -1
  mvn   r0, #0

@ r11 = 0x7FFF, pau->m_iSampleMaxValue
  mov   r11, r0, LSR #17

@ r12 = 0x8000, pau->m_iSampleMinValue
  mov   r12, r0, LSL #15

@ r4 = pMCLMSPredictor->m_iRecent
  ldr   r4, [r1, #MCLMSPredictor_m_iRecent]

@ piPrevVal = (I16 *)pMCLMSPredictor->m_rgiPrevVal;
@ r5 = piPrevVal
  ldr   r5, [r1, #MCLMSPredictor_m_rgiPrevVal]

@ r6 = pMCLMSPredictor->m_rgiUpdate
  ldr   r6, [r1, #MCLMSPredictor_m_rgiUpdate]

  sub   r8, r8, #1

@ r0 = pMCLMSPredictor->m_iUpdStepSize
  ldrsh r0,  [r1, #MCLMSPredictor_m_iUpdStepSize]

@ for (j = pau->m_cChannel - 1; j >= 0; j--) {
g2ndOutLoop16:

@ pMCLMSPredictor->m_iRecent = pMCLMSPredictor->m_iRecent - 1;
  sub   r4, r4, #1

@ if (rgiInput[j] > pau->m_iSampleMaxValue)
@    piPrevVal[pMCLMSPredictor->m_iRecent] = (I16)pau->m_iSampleMaxValue;
@ else if (rgiInput[j] < pau->m_iSampleMinValue)
@    piPrevVal[pMCLMSPredictor->m_iRecent] = (I16)pau->m_iSampleMinValue;
@ else
@    piPrevVal[pMCLMSPredictor->m_iRecent] = (I16)rgiInput[j]@

@ lr = rgiInput[j]
  ldr   lr, [r2, r8, LSL #2]

  cmp   lr, r11
  movgt lr, r11

  cmp   lr, r12
  movlt lr, r12

  mov   r7, r4, LSL #1
  strh  lr, [r5, r7]

@ if (rgiInput[j] > 0)
@    pMCLMSPredictor->m_rgiUpdate[pMCLMSPredictor->m_iRecent] = pMCLMSPredictor->m_iUpdStepSize;
@ else if (rgiInput[j] < 0)
@    pMCLMSPredictor->m_rgiUpdate[pMCLMSPredictor->m_iRecent] = -pMCLMSPredictor->m_iUpdStepSize;
@ else
@    pMCLMSPredictor->m_rgiUpdate[pMCLMSPredictor->m_iRecent] = 0;

  cmp   lr, #0
  movgt lr, r0
  rsblt lr, r0, #0
  moveq lr, #0

  strh  lr, [r6, r7]

  subs  r8, r8, #1
  bge   g2ndOutLoop16

@ r7 = pLMSPredictor
  mov   r7, r1

@ if (pMCLMSPredictor->m_iRecent == 0) {
  cmp   r4, #0
  bne   gEnd16

@ pMCLMSPredictor->m_iRecent = pMCLMSPredictor->m_iOrder_X_CH;
  mov   r4, r9

  add   r2, r5, r9, LSL #1
  add   lr, r6, r9, LSL #1
  
gMemcpy16:
  ldr   r0, [r5], #4
  ldr   r3, [r6], #4
  str   r0, [r2], #4
  str   r3, [lr], #4

  subs  r9, r9, #2
  bne   gMemcpy16

gEnd16:
@ r4 = pMCLMSPredictor->m_iRecent
  str   r4, [r7, #MCLMSPredictor_m_iRecent]

  ldmfd sp!, {r4 - r11, PC} @ prvDecMCLMSPredictorPredAndUpdate_I16
  @ENTRY_END prvDecMCLMSPredictorPredAndUpdate_I16


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  @AREA    |.text|, CODE
_prvDecMCLMSPredictorPredAndUpdate_I32:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pau
@ r1 = pLMSPredictor
@ r2 = rgiInput
@ r3 = rgiPred

@ r4 = piFilter
@ r5 = piPrevVal
@ r6 = piUpdate
@ r7 = piFilterCurrTime

@ r8 = pau->m_cChannel
@ r9 = pMCLMSPredictor->m_iOrder_X_CH
@ r10= iPred


  stmfd sp!, {r4 - r11, lr}

@ r12= pMCLMSPredictor->m_iRecent
  ldr   r12,[r1, #MCLMSPredictor_m_iRecent]

@ I16 * piFilter = pMCLMSPredictor->m_rgiFilter;
  ldr   r4, [r1, #MCLMSPredictor_m_rgiFilter]

@ I16 * piFilterCurrTime = pMCLMSPredictor->m_rgiFilterCurrTime;
  ldr   r7, [r1, #MCLMSPredictor_m_rgiFilterCurrTime]

@ I16 * piPrevVal = (I16 *)pMCLMSPredictor->m_rgiPrevVal + pMCLMSPredictor->m_iRecent;
@ I16 * piUpdate = pMCLMSPredictor->m_rgiUpdate + pMCLMSPredictor->m_iRecent;
  ldr   r5, [r1, #MCLMSPredictor_m_rgiPrevVal]
  ldr   r6, [r1, #MCLMSPredictor_m_rgiUpdate]

@ r8 = m_cChannel
  ldrh  r8, [r0, #CAudioObject_m_cChannel]
  
  add   r5, r5, r12, LSL #2
  add   r6, r6, r12, LSL #1

@ r0 = outloop counter
  mov   r0, #0

@ for (j = 0; j < pau->m_cChannel; j++) {
gOutLoop32:

@ r9 = pMCLMSPredictor->m_iOrder_X_CH
  ldr   r9, [r1, #MCLMSPredictor_m_iOrder_X_CH]

@ if (rgiPred[j] == 0) { // decoder : this channel has zero power.
  ldr   lr, [r3]
  cmp   lr, #0
  bne   gInLoop32

@ piFilter += pMCLMSPredictor->m_iOrder_X_CH;
  add   r4, r4, r9, LSL #1

@ continue;
  add   r3, r3, #4
  b     gOutLoopTest32    

gInLoop32:
@ iPred = (pMCLMSPredictor->m_iScalingOffset)@
  ldr   r10, [r1, #MCLMSPredictor_m_iScalingOffset]

@ lr = rgiInput[j]
  ldr   lr, [r2, r0, LSL #2]
  cmp   lr, #0

  bgt   gInLoopGT32
  blt   gInLoopLT32

@ r0 = pMCLMSPredictor->m_iOrder_X_CH
  mov   lr, r9

@ else { // (rgiInput[j] == 0)
gInLoopEQ32:

@ for (i = 0; i < pMCLMSPredictor->m_iOrder_X_CH; i++) {
@ iPred += ((I32)(*(piFilter + i)) * (*(piPrevVal + i)))@

@ r4 = piFilter
@ r5 = piPrevVal

  ldr   r11, [r5], #4
  ldrsh r12, [r4], #2
  mla   r10, r11, r12, r10

  subs  lr, lr, #1
  bne   gInLoopEQ32

  mov   lr, r0

@ r5 = piPrevVal, restore point
  sub   r5, r5, r9, LSL #2 

g2ndInLoopEQ32:
@ for (i = j-1; i >= 0; i--) {
  subs  lr, lr, #1
  bmi   gInLoopEnd32

@ iPred += (I32)rgiInput[i] * (I32)(*(piFilterCurrTime + i))@
  mov   r12, lr, LSL #1
  ldr   r11, [r2, lr, LSL #2]
  ldrsh r12, [r7, r12]

  mla   r10, r11, r12, r10
  b     g2ndInLoopEQ32


@ if (rgiInput[j] > 0){
gInLoopGT32:

@ for (i = 0; i < pMCLMSPredictor->m_iOrder_X_CH; i++) {
@ iPred += ((I32)(*(piFilter + i)) * (*(piPrevVal + i)))@
@ (*(piFilter + i)) += (*(piUpdate + i))@

@ r4 = piFilter
@ r5 = piPrevVal
@ r6 = piUpdate

@ unloop by 2
  ldr   r11, [r5], #4
  ldrsh r12, [r4]
  ldrsh lr,  [r6], #2
  mla   r10, r11, r12, r10
  add   r12, r12, lr
  strh  r12, [r4], #2

  ldr   r11, [r5], #4
  ldrsh r12, [r4]
  ldrsh lr,  [r6], #2
  mla   r10, r11, r12, r10
  add   r12, r12, lr
  strh  r12, [r4], #2

  subs  r9, r9, #2
  bne   gInLoopGT32

  mov   lr, r0

@ r9 = pMCLMSPredictor->m_iUpdStepSize
  ldrsh r9,  [r1, #MCLMSPredictor_m_iUpdStepSize]

g2ndInLoopGT32:
@ for (i = j-1; i >= 0; i--) {
  subs  lr, lr, #1
  bmi   gInLoopRestore32

@ iPred += (I32)rgiInput[i] * (I32)(*(piFilterCurrTime + i))@
  mov   r12, lr, LSL #1
  ldr   r11, [r2, lr, LSL #2]
  ldrsh r12, [r7, r12]

  cmp   r11, #0
  mla   r10, r11, r12, r10

@ if (rgiInput[i] > 0)
@ (*(piFilterCurrTime + i)) += pMCLMSPredictor->m_iUpdStepSize;
  addgt r12, r12, r9

  mov   r11, lr, LSL #1

@ else if (rgiInput[i] < 0)
@ (*(piFilterCurrTime + i)) -= pMCLMSPredictor->m_iUpdStepSize;
  sublt r12, r12, r9

  strh  r12, [r7, r11]
  b     g2ndInLoopGT32


@ else if (rgiInput[j] < 0){
gInLoopLT32:

@ for (i = 0; i < pMCLMSPredictor->m_iOrder_X_CH; i++) {
@ iPred += ((I32)(*(piFilter + i)) * (*(piPrevVal + i)))@
@ (*(piFilter + i)) -= (*(piUpdate + i))@
@ r4 = piFilter
@ r5 = piPrevVal
@ r6 = piUpdate

@ unloop by 2
  ldr   r11, [r5], #4
  ldrsh r12, [r4]
  ldrsh lr,  [r6], #2
  mla   r10, r11, r12, r10
  sub   r12, r12, lr
  strh  r12, [r4], #2

  ldr   r11, [r5], #4
  ldrsh r12, [r4]
  ldrsh lr,  [r6], #2
  mla   r10, r11, r12, r10
  sub   r12, r12, lr
  strh  r12, [r4], #2

  subs  r9, r9, #2
  bne   gInLoopLT32

  mov   lr, r0

@ r9 = pMCLMSPredictor->m_iUpdStepSize
  ldrsh r9,  [r1, #MCLMSPredictor_m_iUpdStepSize]

g2ndInLoopLT32:
@ for (i = j-1; i >= 0; i--) {
  subs  lr, lr, #1
  bmi   gInLoopRestore32

@ iPred += (I32)rgiInput[i] * (I32)(*(piFilterCurrTime + i))@
  mov   r12, lr, LSL #1
  ldr   r11, [r2, lr, LSL #2]
  ldrsh r12, [r7, r12]

  cmp   r11, #0
  mla   r10, r11, r12, r10

@ if (rgiInput[i] > 0)
@ (*(piFilterCurrTime + i)) -= pMCLMSPredictor->m_iUpdStepSize;
  subgt r12, r12, r9

  mov   r11, lr, LSL #1

@ else if (rgiInput[i] < 0)
@ (*(piFilterCurrTime + i)) += pMCLMSPredictor->m_iUpdStepSize;
  addlt r12, r12, r9

  strh  r12, [r7, r11]
  b     g2ndInLoopLT32

gInLoopRestore32:
@ r9 = pMCLMSPredictor->m_iOrder_X_CH
  ldr   r9, [r1, #MCLMSPredictor_m_iOrder_X_CH]

@ r5 = piPrevVal, restore point
  sub   r5, r5, r9, LSL #2 

@ r6 = piUpdate, restore point
  sub   r6, r6, r9, LSL #1 

gInLoopEnd32:
@ pMCLMSPredictor->m_iScaling
  ldr   r11, [r1, #MCLMSPredictor_m_iScaling]

@ r12 = rgiInput[j]
  ldr   r12, [r2, r0, LSL #2]

@ iPred = iPred >> (pMCLMSPredictor->m_iScaling)@
  mov   r10, r10, ASR r11

@ rgiInput[j] = rgiInput[j] + iPred;
  add   r12, r12, r10

@ rgiPred[j] = iPred;
  str   r10, [r3], #4
  str   r12, [r2, r0, LSL #2]  

gOutLoopTest32: 
@ piFilterCurrTime += pau->m_cChannel;
  add   r7,  r7, r8, LSL #1
 
  add   r0, r0, #1
  cmp   r0, r8
  blt   gOutLoop32

@ r0 = -1
  mvn   r0, #0

@ r11 = 0X007FFFFF, pau->m_iSampleMaxValue
  mov   r11, r0, LSR #9

@ r12 = 0XFF800000, pau->m_iSampleMinValue
  mov   r12, r0, LSL #23

@ r4 = pMCLMSPredictor->m_iRecent
  ldr   r4, [r1, #MCLMSPredictor_m_iRecent]

@ piPrevVal = (I16 *)pMCLMSPredictor->m_rgiPrevVal;
@ r5 = piPrevVal
  ldr   r5, [r1, #MCLMSPredictor_m_rgiPrevVal]

@ r6 = pMCLMSPredictor->m_rgiUpdate
  ldr   r6, [r1, #MCLMSPredictor_m_rgiUpdate]

  sub   r8, r8, #1

@ r0 = pMCLMSPredictor->m_iUpdStepSize
  ldrsh r0,  [r1, #MCLMSPredictor_m_iUpdStepSize]

@ for (j = pau->m_cChannel - 1; j >= 0; j--) {
g2ndOutLoop32:

@ pMCLMSPredictor->m_iRecent = pMCLMSPredictor->m_iRecent - 1;
  sub   r4, r4, #1

@ if (rgiInput[j] > pau->m_iSampleMaxValue)
@    piPrevVal[pMCLMSPredictor->m_iRecent] = (I32)pau->m_iSampleMaxValue;
@ else if (rgiInput[j] < pau->m_iSampleMinValue)
@    piPrevVal[pMCLMSPredictor->m_iRecent] = (I32)pau->m_iSampleMinValue;
@ else
@    piPrevVal[pMCLMSPredictor->m_iRecent] = (I32)rgiInput[j]@

@ lr = rgiInput[j]
  ldr   lr, [r2, r8, LSL #2]

  cmp   lr, r11
  movgt lr, r11

  cmp   lr, r12
  movlt lr, r12

  str   lr, [r5, r4, LSL #2]

@ if (rgiInput[j] > 0)
@    pMCLMSPredictor->m_rgiUpdate[pMCLMSPredictor->m_iRecent] = pMCLMSPredictor->m_iUpdStepSize;
@ else if (rgiInput[j] < 0)
@    pMCLMSPredictor->m_rgiUpdate[pMCLMSPredictor->m_iRecent] = -pMCLMSPredictor->m_iUpdStepSize;
@ else
@    pMCLMSPredictor->m_rgiUpdate[pMCLMSPredictor->m_iRecent] = 0;


  mov   r7, r4, LSL #1

  cmp   lr, #0
  movgt lr, r0
  rsblt lr, r0, #0
  moveq lr, #0

  strh  lr, [r6, r7]

  subs  r8, r8, #1
  bge   g2ndOutLoop32

@ r7 = pLMSPredictor
  mov   r7, r1

@ if (pMCLMSPredictor->m_iRecent == 0) {
  cmp   r4, #0
  bne   gEnd32

@ pMCLMSPredictor->m_iRecent = pMCLMSPredictor->m_iOrder_X_CH;
  mov   r4, r9

  add   r2, r5, r9, LSL #2
  add   lr, r6, r9, LSL #1
  
gMemcpy32:
  ldr   r0, [r5], #4
  ldr   r3, [r6], #4

  str   r0, [r2], #4
  ldr   r0, [r5], #4
  str   r3, [lr], #4
  str   r0, [r2], #4

  subs  r9, r9, #2
  bne   gMemcpy32

gEnd32:
@ r4 = pMCLMSPredictor->m_iRecent
  str   r4, [r7, #MCLMSPredictor_m_iRecent]

  ldmfd sp!, {r4 - r11, PC} @ prvDecMCLMSPredictorPredAndUpdate_I32
  @ENTRY_END prvDecMCLMSPredictorPredAndUpdate_I32


  .endif @ WMA_OPT_LPCLSL_ARM

  
