@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************
@// Module Name:
@//
@//     losslessdecprolsl_arm.s
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
@//     Jerry He (yamihe) Aug 6, 2003
@//
@// Revision History:
@//
@//     For more information on ARM assembler directives, use
@//        http://msdn.microsoft.com/library/default.asp?url=/library/en-us/wcechp40/html/ccconarmassemblerdirectives.asp
@//*************************************************************************

  #include "../../../inc/audio/v10/include/voWMADecID.h"
  .include     "kxarm.h"
  .include     "wma_member_arm.inc"
 
  @AREA    |.text|, CODE, READONLY
  .text .align 4

  .if WMA_OPT_LOSSLESSDECPROLSL_ARM == 1
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  .globl  _prvDecLMSPredictor_I16_C
  .globl  _prvDecLMSPredictor_I32_C


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

_prvDecLMSPredictor_I16_C:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pau
@ r1 = pLMSPredictor
@ r2 = rgiResidue
@ r3 = iLen

@ r4 = pLMSPredictor
@ r5 = rgiResidue
@ r6 = iLen

@ r7 = rgiPrevValRdBuf
@ r8 = rgiUpdateRdBuf

@ r10= iInput
@ r11= iRecent

  stmfd sp!, {r4 - r11, lr}


  ldr   r11,[r1, #LMSPredictor_m_iRecent]
  mov   r5, r2

  ldr   r7, [r1, #LMSPredictor_m_rgiPrevVal]
  mov   r6, r3

  ldr   r8, [r1, #LMSPredictor_m_rgiUpdate]
  mov   r4, r1

gOutLoop16:

@ for (i = 0; i < iLen; i++) {
@ r0 = iPred
@ iPred = iScalingOffset;
  ldr   r0, [r4, #LMSPredictor_m_iScalingOffset]

@ rgiFilter = rgiFilterBuf;
  ldr   r9, [r4, #LMSPredictor_m_rgiFilter]

@ r3 = iOrder_Div_8
  ldr   r3, [r4, #LMSPredictor_m_iOrder_Div_8]

@ iPredErr = (I32)*rgiResidue;
@ r2 = iPredErr
  ldr   r2, [r5]

@ rgiPrevVal = rgiPrevValRdBuf + iRecent;
@ r12 = rgiPrevVal
  add   r12, r7, r11, LSL #1

@ rgiUpdate = rgiUpdateRdBuf + iRecent;
@ r10 = rgiUpdate
  add   r10, r8, r11, LSL #1

  cmp   r2, #0
  bgt   gInLoop16GT
  blt   gInLoop16LT

gInLoop16EQ:
@ for (j = 0; j < iOrder_Div_8; j++) {
@ iPred += (I32)(rgiFilter[0]) * (I32)(rgiPrevVal[0]) + (I32)(rgiFilter[1]) * (I32)(rgiPrevVal[1])@
@ iPred += (I32)(rgiFilter[2]) * (I32)(rgiPrevVal[2]) + (I32)(rgiFilter[3]) * (I32)(rgiPrevVal[3])@
@ iPred += (I32)(rgiFilter[4]) * (I32)(rgiPrevVal[4]) + (I32)(rgiFilter[5]) * (I32)(rgiPrevVal[5])@
@ iPred += (I32)(rgiFilter[6]) * (I32)(rgiPrevVal[6]) + (I32)(rgiFilter[7]) * (I32)(rgiPrevVal[7])@
@ rgiFilter += 8;
@ rgiPrevVal += 8;

@ r9  = rgiFilter
@ r12 = rgiPrevVal

  ldrsh r1, [r9], #2
  ldrsh r2, [r12], #2
  mla   r0, r2, r1, r0

  ldrsh r1, [r9], #2
  ldrsh r2, [r12], #2
  mla   r0, r2, r1, r0

  ldrsh r1, [r9], #2
  ldrsh r2, [r12], #2
  mla   r0, r2, r1, r0

  ldrsh r1, [r9], #2
  ldrsh r2, [r12], #2
  mla   r0, r2, r1, r0

  ldrsh r1, [r9], #2
  ldrsh r2, [r12], #2
  mla   r0, r2, r1, r0

  ldrsh r1, [r9], #2
  ldrsh r2, [r12], #2
  mla   r0, r2, r1, r0

  ldrsh r1, [r9], #2
  ldrsh r2, [r12], #2
  mla   r0, r2, r1, r0

  ldrsh r1, [r9], #2
  ldrsh r2, [r12], #2
  mla   r0, r2, r1, r0

  subs  r3, r3, #1
  bne   gInLoop16EQ

  b     gInLoopEnd

gInLoop16GT:
@ for (j = 0; j < iOrder_Div_8; j++) {
@ iPred += (I32)(rgiFilter[0]) * (I32)(rgiPrevVal[0]) + (I32)(rgiFilter[1]) * (I32)(rgiPrevVal[1])@
@ iPred += (I32)(rgiFilter[2]) * (I32)(rgiPrevVal[2]) + (I32)(rgiFilter[3]) * (I32)(rgiPrevVal[3])@
@ iPred += (I32)(rgiFilter[4]) * (I32)(rgiPrevVal[4]) + (I32)(rgiFilter[5]) * (I32)(rgiPrevVal[5])@
@ iPred += (I32)(rgiFilter[6]) * (I32)(rgiPrevVal[6]) + (I32)(rgiFilter[7]) * (I32)(rgiPrevVal[7])@
@ rgiFilter += 8;
@ rgiPrevVal += 8;

@ if (iPredErr > 0) 
@ for (j = 0; j < iOrder; j++) {
@ rgiFilter[j] +=  rgiUpdate[j]@

@ r9  = rgiFilter
@ r12 = rgiPrevVal
@ r10 = rgiUpdate

  ldrsh r2, [r12], #2
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  add   r1, r1, lr
  strh  r1, [r9], #2

  ldrsh r2, [r12], #2
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  add   r1, r1, lr
  strh  r1, [r9], #2

  ldrsh r2, [r12], #2
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  add   r1, r1, lr
  strh  r1, [r9], #2

  ldrsh r2, [r12], #2
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  add   r1, r1, lr
  strh  r1, [r9], #2

  ldrsh r2, [r12], #2
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  add   r1, r1, lr
  strh  r1, [r9], #2

  ldrsh r2, [r12], #2
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  add   r1, r1, lr
  strh  r1, [r9], #2

  ldrsh r2, [r12], #2
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  add   r1, r1, lr
  strh  r1, [r9], #2

  ldrsh r2, [r12], #2
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  add   r1, r1, lr
  strh  r1, [r9], #2

  subs  r3, r3, #1
  bne   gInLoop16GT

  b     gInLoopEnd

gInLoop16LT:
@ for (j = 0; j < iOrder_Div_8; j++) {
@ iPred += (I32)(rgiFilter[0]) * (I32)(rgiPrevVal[0]) + (I32)(rgiFilter[1]) * (I32)(rgiPrevVal[1])@
@ iPred += (I32)(rgiFilter[2]) * (I32)(rgiPrevVal[2]) + (I32)(rgiFilter[3]) * (I32)(rgiPrevVal[3])@
@ iPred += (I32)(rgiFilter[4]) * (I32)(rgiPrevVal[4]) + (I32)(rgiFilter[5]) * (I32)(rgiPrevVal[5])@
@ iPred += (I32)(rgiFilter[6]) * (I32)(rgiPrevVal[6]) + (I32)(rgiFilter[7]) * (I32)(rgiPrevVal[7])@
@ rgiFilter += 8;
@ rgiPrevVal += 8;

@ if (iPredErr < 0) 
@ for (j = 0; j < iOrder; j++) {
@ rgiFilter[j] -=  rgiUpdate[j]@

@ r9  = rgiFilter
@ r12 = rgiPrevVal
@ r10 = rgiUpdate

  ldrsh r2, [r12], #2
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  sub   r1, r1, lr
  strh  r1, [r9], #2

  ldrsh r2, [r12], #2
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  sub   r1, r1, lr
  strh  r1, [r9], #2

  ldrsh r2, [r12], #2
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  sub   r1, r1, lr
  strh  r1, [r9], #2

  ldrsh r2, [r12], #2
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  sub   r1, r1, lr
  strh  r1, [r9], #2

  ldrsh r2, [r12], #2
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  sub   r1, r1, lr
  strh  r1, [r9], #2

  ldrsh r2, [r12], #2
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  sub   r1, r1, lr
  strh  r1, [r9], #2

  ldrsh r2, [r12], #2
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  sub   r1, r1, lr
  strh  r1, [r9], #2

  ldrsh r2, [r12], #2
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  sub   r1, r1, lr
  strh  r1, [r9], #2

  subs  r3, r3, #1
  bne   gInLoop16LT

gInLoopEnd:
@ iPredErr = (I32)*rgiResidue;
@ iPred = (iPred >> iScaling)@
  ldr   r1, [r4, #LMSPredictor_m_iScaling]
  ldr   r2, [r5]
  mov   r0, r0, ASR r1

@ iInput = iPredErr + iPred;
  add   r10,r2, r0

@ if (iRecent == 0) {
  cmp   r11, #0
  bne   gReNE16

@ iRecent = iOrder - 1;
@ r11 = iOrder
  ldr   r11, [r4, #LMSPredictor_m_iOrder]
  
  mov   r9, r11
  mov   r1, r7
  mov   r12,r8

  add   r2, r7, r9, LSL #1
  add   lr, r8, r9, LSL #1
  
gMemcpy16:
  ldr   r0, [r1], #4
  ldr   r3, [r12],#4
  str   r0, [r2], #4
  str   r3, [lr], #4

  subs  r9, r9, #2
  bne   gMemcpy16

gReNE16:
@ iRecent = iRecent - 1;
  sub   r11, r11, #1

@ * rgiResidue = iInput;
@ rgiResidue = rgiResidue + 1;
  str   r10, [r5], #4

@ rgiPrevVal = rgiPrevValRdBuf + iRecent;
  add   r1, r7, r11, LSL #1

@ rgiUpdate = rgiUpdateRdBuf + iRecent;
  add   r2, r8, r11, LSL #1

@ * rgiUpdate = iUpdStepSize_X_8;
@ r0 = iUpdStepSize_X_8
  ldrsh r0, [r4, #LMSPredictor_m_iUpdStepSize_X_8]

@ r3 = -1
  mvn   r3, #0

@ lr = 0x7FFF
  mov   lr, r3, LSR #17

@ r12 = 0x8000
  mov   r12,r3, LSL #15

@ if (iInput > 0) {
  cmp   r10, #0

@ * rgiUpdate = 0;
  moveq r0, #0

@ * rgiUpdate = - iUpdStepSize_X_8;
  rsblt r0, r0, #0

@ if (iInput > (I32)0X00007FFF) {
@ * rgiPrevVal = (I16)0X7FFF;
  cmp   r10, lr
  movgt r10, lr

@ if (iInput < (I32)0XFFFF8000) {
@ * rgiPrevVal = (I16)0X8000;        
  cmp   r10, r12
  movlt r10, r12

@ store rgiUpdate
  strh  r0, [r2]

@ * rgiPrevVal = ((I16)iInput)@
  strh  r10,[r1]

@ can not guarantee iOrder_Div_16*2 == iOrder_Div_8
@ therefore need load both iOrder_Div_16 and iOrder_Div_8

@ rgiUpdate[iOrder_Div_16] >>= 2;
@ rgiUpdate[iOrder_Div_8] >>= 1;
  ldr   r3, [r4, #LMSPredictor_m_iOrder_Div_16]
  ldr   r12,[r4, #LMSPredictor_m_iOrder_Div_8]

  mov   r3, r3, LSL #1
  mov   r12,r12,LSL #1

  ldrsh r0, [r2, r3]
  ldrsh lr, [r2, r12]

  mov   r0, r0, ASR #2
  mov   lr, lr, ASR #1

  strh  r0, [r2, r3]
  strh  lr, [r2, r12]

@ r6 = iLen
  subs  r6, r6, #1
  bne   gOutLoop16

@ pLMSPredictor->m_iRecent = iRecent;
  str   r11,[r4, #LMSPredictor_m_iRecent]

  ldmfd sp!, {r4 - r11, PC} @prvDecLMSPredictor_I16_C
  @ENTRY_END prvDecLMSPredictor_I16_C 

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

_prvDecLMSPredictor_I32_C:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pau
@ r1 = pLMSPredictor
@ r2 = rgiResidue
@ r3 = iLen


@ r4 = pLMSPredictor
@ r5 = rgiResidue
@ r6 = iLen

@ r7 = rgiPrevValRdBuf
@ r8 = rgiUpdateRdBuf

@ r10= iInput
@ r11= iRecent


  stmfd sp!, {r4 - r11, lr}

  ldr   r11,[r1, #LMSPredictor_m_iRecent]
  mov   r5, r2

  ldr   r7, [r1, #LMSPredictor_m_rgiPrevVal]
  mov   r6, r3

  ldr   r8, [r1, #LMSPredictor_m_rgiUpdate]
  mov   r4, r1

gOutLoop32:
@ for (i = 0; i < iLen; i++) {
@ r0 = iPred
@ iPred = iScalingOffset;
  ldr   r0, [r4, #LMSPredictor_m_iScalingOffset]

@ rgiFilter = rgiFilterBuf;
  ldr   r9, [r4, #LMSPredictor_m_rgiFilter]

@ r3 = iOrder_Div_8
  ldr   r3, [r4, #LMSPredictor_m_iOrder_Div_8]

@ iPredErr = (I32)*rgiResidue;
@ r2 = iPredErr
  ldr   r2, [r5]

@ rgiPrevVal = rgiPrevValRdBuf + iRecent;
@ r12 = rgiPrevVal
  add   r12,  r7, r11, LSL #2

@ rgiUpdate = rgiUpdateRdBuf + iRecent;
@ r10 = rgiUpdate
  add   r10, r8, r11, LSL #1

  cmp   r2, #0
  bgt   gInLoop32GT
  blt   gInLoop32LT

gInLoop32EQ:
@ for (j = 0; j < iOrder_Div_8; j++) {
@ iPred += (I32)rgiFilter[0] * rgiPrevVal[0]@
@ iPred += (I32)rgiFilter[1] * rgiPrevVal[1]@
@ iPred += (I32)rgiFilter[2] * rgiPrevVal[2]@
@ iPred += (I32)rgiFilter[3] * rgiPrevVal[3]@
@ iPred += (I32)rgiFilter[4] * rgiPrevVal[4]@
@ iPred += (I32)rgiFilter[5] * rgiPrevVal[5]@
@ iPred += (I32)rgiFilter[6] * rgiPrevVal[6]@
@ iPred += (I32)rgiFilter[7] * rgiPrevVal[7]@
@ rgiFilter += 8;
@ rgiPrevVal += 8;

@ r9  = rgiFilter
@ r12 = rgiPrevVal

  ldrsh r1, [r9], #2
  ldr   r2, [r12], #4
  mla   r0, r2, r1, r0

  ldrsh r1, [r9], #2
  ldr   r2, [r12], #4
  mla   r0, r2, r1, r0

  ldrsh r1, [r9], #2
  ldr   r2, [r12], #4
  mla   r0, r2, r1, r0

  ldrsh r1, [r9], #2
  ldr   r2, [r12], #4
  mla   r0, r2, r1, r0

  ldrsh r1, [r9], #2
  ldr   r2, [r12], #4
  mla   r0, r2, r1, r0

  ldrsh r1, [r9], #2
  ldr   r2, [r12], #4
  mla   r0, r2, r1, r0

  ldrsh r1, [r9], #2
  ldr   r2, [r12], #4
  mla   r0, r2, r1, r0

  ldrsh r1, [r9], #2
  ldr   r2, [r12], #4
  mla   r0, r2, r1, r0

  subs  r3, r3, #1
  bne   gInLoop32EQ

  b     gInLoopEnd32

gInLoop32GT:
@ for (j = 0; j < iOrder_Div_8; j++) {
@ iPred += (I32)rgiFilter[0] * rgiPrevVal[0]@
@ iPred += (I32)rgiFilter[1] * rgiPrevVal[1]@
@ iPred += (I32)rgiFilter[2] * rgiPrevVal[2]@
@ iPred += (I32)rgiFilter[3] * rgiPrevVal[3]@
@ iPred += (I32)rgiFilter[4] * rgiPrevVal[4]@
@ iPred += (I32)rgiFilter[5] * rgiPrevVal[5]@
@ iPred += (I32)rgiFilter[6] * rgiPrevVal[6]@
@ iPred += (I32)rgiFilter[7] * rgiPrevVal[7]@
@ rgiFilter += 8;
@ rgiPrevVal += 8;

@ if (iPredErr > 0) 
@ for (j = 0; j < iOrder; j++) {
@ rgiFilter[j] +=  rgiUpdate[j]@

@ r9  = rgiFilter
@ r12 = rgiPrevVal
@ r10 = rgiUpdate

  ldr   r2, [r12], #4
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  add   r1, r1, lr
  strh  r1, [r9], #2

  ldr   r2, [r12], #4
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  add   r1, r1, lr
  strh  r1, [r9], #2

  ldr   r2, [r12], #4
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  add   r1, r1, lr
  strh  r1, [r9], #2

  ldr   r2, [r12], #4
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  add   r1, r1, lr
  strh  r1, [r9], #2

  ldr   r2, [r12], #4
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  add   r1, r1, lr
  strh  r1, [r9], #2

  ldr   r2, [r12], #4
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  add   r1, r1, lr
  strh  r1, [r9], #2

  ldr   r2, [r12], #4
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  add   r1, r1, lr
  strh  r1, [r9], #2

  ldr   r2, [r12], #4
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  add   r1, r1, lr
  strh  r1, [r9], #2

  subs  r3, r3, #1
  bne   gInLoop32GT

  b     gInLoopEnd32

gInLoop32LT:
@ for (j = 0; j < iOrder_Div_8; j++) {
@ iPred += (I32)rgiFilter[0] * rgiPrevVal[0]@
@ iPred += (I32)rgiFilter[1] * rgiPrevVal[1]@
@ iPred += (I32)rgiFilter[2] * rgiPrevVal[2]@
@ iPred += (I32)rgiFilter[3] * rgiPrevVal[3]@
@ iPred += (I32)rgiFilter[4] * rgiPrevVal[4]@
@ iPred += (I32)rgiFilter[5] * rgiPrevVal[5]@
@ iPred += (I32)rgiFilter[6] * rgiPrevVal[6]@
@ iPred += (I32)rgiFilter[7] * rgiPrevVal[7]@
@ rgiFilter += 8;
@ rgiPrevVal += 8;

@ if (iPredErr > 0) 
@ for (j = 0; j < iOrder; j++) {
@ rgiFilter[j] -=  rgiUpdate[j]@

@ r9  = rgiFilter
@ r12 = rgiPrevVal
@ r10 = rgiUpdate

  ldr   r2, [r12], #4
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  sub   r1, r1, lr
  strh  r1, [r9], #2

  ldr   r2, [r12], #4
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  sub   r1, r1, lr
  strh  r1, [r9], #2

  ldr   r2, [r12], #4
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  sub   r1, r1, lr
  strh  r1, [r9], #2

  ldr   r2, [r12], #4
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  sub   r1, r1, lr
  strh  r1, [r9], #2

  ldr   r2, [r12], #4
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  sub   r1, r1, lr
  strh  r1, [r9], #2

  ldr   r2, [r12], #4
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  sub   r1, r1, lr
  strh  r1, [r9], #2

  ldr   r2, [r12], #4
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  sub   r1, r1, lr
  strh  r1, [r9], #2

  ldr   r2, [r12], #4
  ldrsh r1, [r9]
  ldrsh lr, [r10], #2
  mla   r0, r2, r1, r0
  sub   r1, r1, lr
  strh  r1, [r9], #2

  subs  r3, r3, #1
  bne   gInLoop32LT

gInLoopEnd32:
@ iPredErr = (I32)*rgiResidue;
@ iPred = (iPred >> iScaling)@
  ldr   r1, [r4, #LMSPredictor_m_iScaling]
  ldr   r2, [r5]
  mov   r0, r0, ASR r1

@ iInput = iPredErr + iPred;
  add   r10,r2, r0

@ if (iRecent == 0) {
  cmp   r11, #0
  bne   gReNE32

@ iRecent = iOrder - 1;
@ r11 = iOrder
  ldr   r11, [r4, #LMSPredictor_m_iOrder]

  mov   r9, r11
  mov   r1, r7
  mov   r12,r8

  add   r2, r7, r9, LSL #2
  add   lr, r8, r9, LSL #1
  
gMemcpy32:
  ldr   r0, [r1], #4
  ldr   r3, [r12],#4

  str   r0, [r2], #4
  ldr   r0, [r1], #4
  str   r3, [lr], #4
  str   r0, [r2], #4

  subs  r9, r9, #2
  bne   gMemcpy32


gReNE32:
@ iRecent = iRecent - 1;
  sub   r11, r11, #1

@ * rgiResidue = iInput;
@ rgiResidue = rgiResidue + 1;
  str   r10, [r5], #4
 
@ rgiPrevVal = rgiPrevValRdBuf + iRecent;
  add   r1, r7, r11, LSL #2

@ rgiUpdate = rgiUpdateRdBuf + iRecent;
  add   r2, r8, r11, LSL #1

@ * rgiUpdate = iUpdStepSize_X_8;
@ r0 = iUpdStepSize_X_8
  ldrsh r0, [r4, #LMSPredictor_m_iUpdStepSize_X_8]

@ r3 = -1
  mvn   r3, #0

@ lr = 0X007FFFFF
  mov   lr, r3, LSR #9

@ r12 = 0XFF800000
  mov   r12,r3, LSL #23

@ if (iInput > 0) {
  cmp   r10, #0

@ * rgiUpdate = 0;
  moveq r0, #0

@ * rgiUpdate = - iUpdStepSize_X_8;
  rsblt r0, r0, #0

@ if (iInput > (I32)0X007FFFFF)
@ * rgiPrevVal = (I32)0X007FFFFF;
  cmp   r10, lr
  movgt r10, lr

@ if (iInput < (I32)0XFF800000)
@ * rgiPrevVal = (I32)0XFF800000;
  cmp   r10, r12
  movlt r10, r12
            
@ store rgiUpdate
  strh  r0, [r2]

@ * rgiPrevVal = ((I32)iInput)@ 
  str   r10,[r1]

@ can not guarantee iOrder_Div_16*2 == iOrder_Div_8
@ therefore need load both iOrder_Div_16 and iOrder_Div_8

@ rgiUpdate[iOrder_Div_16] >>= 2;
@ rgiUpdate[iOrder_Div_8] >>= 1;
  ldr   r3, [r4, #LMSPredictor_m_iOrder_Div_16]
  ldr   r12,[r4, #LMSPredictor_m_iOrder_Div_8]

  mov   r3, r3, LSL #1
  mov   r12,r12,LSL #1

  ldrsh r0, [r2, r3]
  ldrsh lr, [r2, r12]

  mov   r0, r0, ASR #2
  mov   lr, lr, ASR #1

  strh  r0, [r2, r3]
  strh  lr, [r2, r12]

@ r6 = iLen
  subs  r6, r6, #1
  bne   gOutLoop32

@ pLMSPredictor->m_iRecent = iRecent;
  str   r11,[r4, #LMSPredictor_m_iRecent]

  ldmfd sp!, {r4 - r11, PC} @prvDecLMSPredictor_I32_C
  @ENTRY_END prvDecLMSPredictor_I32_C

  .endif @ WMA_OPT_LOSSLESSDECPROLSL_ARM

  
  
