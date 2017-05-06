@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@
@  THIS IS ASSEMBLY VERSION OF ROUTINES IN MULIRES_WMV9.C WHEN 
@  WMV_OPT_MULTIRES_ARM ARE DEFINED
@
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		#include "../../Src/c/voVC1DID.h"
    .include "wmvdec_member_arm.h"
    .include "xplatform_arm_asm.h" 
 
    @AREA MOTIONCOMP, CODE, READONLY
		 .text
		 .align 4

    .if WMV_OPT_MULTIRES_ARM == 1
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

 
    .globl  _g_DownsampleWFilterLine6_Vert_ARMV4
    .globl  _g_DownsampleWFilterLine6_Horiz_ARMV4
    .globl  _g_UpsampleWFilterLine10_Vert_ARMV4
    .globl  _g_UpsampleWFilterLine10_Horiz_ARMV4


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA    |.text|, CODE
    WMV_LEAF_ENTRY g_DownsampleWFilterLine6_Vert_ARMV4
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 == pDst
@ r1 == pSrc
@ r2 == x
@ r3 == size


  stmfd sp!, {r4 - r12, r14}       @ r0-r3 are preserved
  FRAME_PROFILE_COUNT

@ the temporary buffer x can be eliminated to improve cache efficiency
@ we clip and store new value to pDst after each calculation
@ we use register to save pSrc value because pDst and pSrc can point to same address
@ so after we store pDst, we can not read it back as pSrc

  ldr   r4,  [sp, #40]             @ r4 == iPitch

  ldrb  r5,  [r1]                  @ pSrc[0]
  ldrb  r6,  [r1, r4]!             @ pSrc[iPitch]
  ldrb  r7,  [r1, r4]!             @ pSrc[2*iPitch], pSrc + 2*iPitch
  ldrb  r8,  [r1, r4]!             @ pSrc[3*iPitch]

  mov   r11, #75                   
  mov   r12, #59
  mov   r14, #63

@ x[0] == (I32_WMV) (((pSrc[0] + pSrc[iPitch]) * AW1 + (pSrc[2*iPitch] + pSrc[0]) * AW2 + (pSrc[3*iPitch] + pSrc[iPitch]) * AW3+ 63) >> 7)@

  mla   r2,  r5,  r11, r14         @ pSrc[0]*75 + 63
  add   r9,  r7,  r7,  lsl #2      @ pSrc[2*iPitch]*5
  mvn   r11, #10                   @ -11, AW3
  mla   r10, r6,  r12, r9          @ pSrc[iPitch]*59 + pSrc[2*iPitch]*5
  mla   r2,  r8,  r11, r2          @ pSrc[3*iPitch]*(-11) + pSrc[0]*75 + 63

  sub   r3,  r3,  #4
  mov   r3,  r3,  lsr #1

  add   r2,  r2,  r10
  mov   r12, #70                   @ SW1                   
  mov   r2,  r2,  asr #7           @ pDst[0]
  mov   r10, #0
 
  sub  r9,  r2,  #0xFF            @ CLIP pDst[0] 
  
  mvnne r2,  r2,  asr #31

  strb  r2,  [r0]
  strb  r10, [r0, r4]!
  

@ r11 == SW3
@ r12 == SW1
@ r14 == 63


lDV6Loop:

@ for( j == 2@ j < size -2@ j +== 2)
@     x[j] ==  (I32_WMV) (((pSrc[j*iPitch] + pSrc[(j+1)*iPitch]) * AW1 + (pSrc[(j-1)*iPitch] + pSrc[(j+2)*iPitch]) * AW2 + (pSrc[(j-2)*iPitch] + pSrc[(j+3)*iPitch]) * AW3 + 63) >> 7)@

  ldrb  r9,  [r1, r4]!             @ pSrc[(j+2)*iPitch]
  ldrb  r10, [r1, r4]!             @ pSrc[(j+3)*iPitch]

  add   r6,  r6,  r9               @ pSrc[(j-1)*iPitch]+pSrc[(j+2)*iPitch]
  add   r5,  r5,  r10              @ pSrc[(j-2)*iPitch]+pSrc[(j+3)*iPitch]
  add   r2,  r7,  r8               @ pSrc[j*iPitch]+pSrc[(j+1)*iPitch]
  add   r6,  r6,  r6,  lsl #2      @ (pSrc[(j-1)*iPitch]+pSrc[(j+2)*iPitch]) * AW2
  mla   r14, r5,  r11, r14         @ (pSrc[(j-2)*iPitch]+pSrc[(j+3)*iPitch]) * AW3 + 63
  mla   r6,  r2,  r12, r6          @ (pSrc[j*iPitch]+pSrc[(j+1])*iPitch) * AW1 + (pSrc[(j-1)*iPitch]+pSrc[(j+2)*iPitch]) * AW2

  add   r2,  r6,  r14   
  mov   r2,  r2,  asr  #7          @ pDst[j]

  sub  r5,  r2,  #0xFF            @ CLIP pDst[j] 
  mvnne r2,  r2,  asr #31

  mov   r6,  #0
  strb  r2,  [r0, r4]!
  strb  r6,  [r0, r4]!

  mov   r5,  r7
  mov   r6,  r8
  mov   r7,  r9
  mov   r8,  r10
  mov   r14, #63

  subs  r3,  r3,  #1
  bne   lDV6Loop

@ x[size-2] ==  (I32_WMV) (((pSrc[(size-2)*iPitch] + pSrc[(size-1)*iPitch]) * AW1 + (pSrc[(size-3)*iPitch] + pSrc[(size-1)*iPitch]) * AW2 + (pSrc[(size-4)*iPitch] + pSrc[(size-2)*iPitch]) * AW3 + 63) >> 7)@

  add   r6,  r6,  r8               @ pSrc[(size-3)*iPitch]+pSrc[(size-1)*iPitch]
  add   r5,  r5,  r7               @ pSrc[(size-4)*iPitch]+pSrc[(size-2)*iPitch]
  add   r2,  r7,  r8               @ pSrc[(size-2)*iPitch]+pSrc[(size-1)*iPitch]
  add   r6,  r6,  r6,  lsl #2      @ (pSrc[(size-3)*iPitch]+pSrc[(size-1)*iPitch]) * AW2
  mla   r14, r5,  r11, r14         @ (pSrc[(size-4)*iPitch]+pSrc[(size-2)*iPitch]) * AW3 + 63
  mla   r6,  r2,  r12, r6          @ (pSrc[(size-2)*iPitch]+pSrc[(size-1)*iPitch]) * AW1 + (pSrc[(size-3)*iPitch]+pSrc[(size-1)*iPitch]) * AW2
  add   r2,  r6,  r14   
  mov   r2,  r2,  asr  #7          @ pDst[size-2]

  sub  r9,  r2,  #0xFF            @ CLIP pDst[size-2] 
  mvnne r2,  r2,  asr #31
     
  mov   r6,  #0
  strb  r2,  [r0, r4]!
  strb  r6,  [r0, r4]


  ldmfd sp!, {r4 - r12, PC}
  WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA    |.text|, CODE
    WMV_LEAF_ENTRY g_DownsampleWFilterLine6_Horiz_ARMV4
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 == pDst
@ r1 == pSrc
@ r2 == x
@ r3 == size


  stmfd sp!, {r4 - r12, r14}       @ r0-r3 are preserved
  FRAME_PROFILE_COUNT

@ the temporary buffer x can be eliminated to improve cache efficiency
@ we clip and store new value to pDst after each calculation
@ we use register to save pSrc value because pDst and pSrc can point to same address
@ so after we store pDst, we can not read it back as pSrc


  ldrb  r4,  [r1]                  @ pSrc[0]
  ldrb  r5,  [r1, #1]              @ pSrc[1]
  ldrb  r6,  [r1, #2]!             @ pSrc[2], pSrc + 2
  ldrb  r7,  [r1, #1]              @ pSrc[3]

  mov   r10, #64
  mov   r11, #75
  mov   r12, #59
  mvn   r14, #10                   @ AW3

@ x[0] == (I32_WMV) (((pSrc[0] + pSrc[1]) * AW1 + (pSrc[2] + pSrc[0]) * AW2 + (pSrc[3] + pSrc[1]) * AW3+ 64) >> 7)@

  mla   r2,  r4,  r11, r10         @ pSrc[0]*75 + 64
  add   r9,  r6,  r6,  lsl #2      @ pSrc[2]*5
  mla   r10, r5,  r12, r9          @ pSrc[1]*59 + r9
  mla   r2,  r7,  r14, r2          @ pSrc[3]*(-11) + r4
  add   r2,  r2,  r10
  mov   r2,  r2,  asr #7           @ pDst[0]

  sub  r9,  r2,  #0xFF            @ CLIP pDst[0] 
  mvnne r2,  r2,  asr #31
  mov   r10, #0
 
  strb  r2,  [r0]
  strb  r10, [r0, #1]!

  sub   r3,  r3,  #4
  mov   r3,  r3,  lsr #1

  mov   r11, #64                   
  mov   r12, #70                   @ AW1
  

lDH6Loop:
@ for( j == 2@ j < size -2@ j +== 2) {
@    x[j] ==  (I32_WMV) (((pSrc[j] + pSrc[j+1]) * AW1 + (pSrc[j-1] + pSrc[j+2]) * AW2 + (pSrc[j-2] + pSrc[j+3]) * AW3 + 64) >> 7)@
@ }

  ldrb  r8,  [r1, #2]!             @ pSrc[j+2], pSrc + 2
  ldrb  r9,  [r1, #1]              @ pSrc[j+3]

  add   r5,  r5,  r8               @ pSrc[j-1]+pSrc[j+2]
  add   r4,  r4,  r9               @ pSrc[j-2]+pSrc[j+3]
  add   r2,  r6,  r7               @ pSrc[j]+pSrc[j+1]
  add   r5,  r5,  r5,  lsl #2      @ (pSrc[j-1]+pSrc[j+2]) * AW2
  mla   r11, r4,  r14, r11         @ (pSrc[j-2]+pSrc[j+3]) * AW3 + 64
  mla   r5,  r2,  r12, r5          @ (pSrc[j]+pSrc[j+1]) * AW1 + (pSrc[j-1]+pSrc[j+2]) * AW2
  add   r2,  r5,  r11   
  mov   r2,  r2,  asr  #7          @ pDst[j]

  sub  r4,  r2,  #0xFF            @ CLIP pDst[j] 
  mvnne r2,  r2,  asr #31
 
  strb  r2,  [r0, #1]
  strb  r10, [r0, #2]!

  mov   r4,  r6
  mov   r5,  r7
  mov   r6,  r8
  mov   r7,  r9
  mov   r11, #64

  subs  r3,  r3,  #1
  bne   lDH6Loop

@ x[size-2] ==  (I32_WMV) (((pSrc[size-2] + pSrc[size-1]) * AW1 + (pSrc[size-3] + pSrc[size-1]) * AW2 + (pSrc[size-4] + pSrc[size-2]) * AW3 + 64) >> 7)@

  add   r5,  r5,  r7               @ pSrc[size-3]+pSrc[size-1]
  add   r4,  r4,  r6               @ pSrc[size-4]+pSrc[size-2]
  add   r2,  r6,  r7               @ pSrc[size-2]+pSrc[size-1]
  add   r5,  r5,  r5,  lsl #2      @ (pSrc[size-3]+pSrc[size-1]) * AW2
  mla   r11, r4,  r14, r11         @ (pSrc[size-4]+pSrc[size-2]) * AW3 + 64
  mla   r5,  r2,  r12, r5          @ (pSrc[size-2]+pSrc[size-1]) * AW1 + (pSrc[size-3]+pSrc[size-1]) * AW2
  add   r2,  r5,  r11   
  mov   r2,  r2,  asr  #7          @ pDst[size-2]

  sub  r4,  r2,  #0xFF            @ CLIP pDst[size-2] 
  mvnne r2,  r2,  asr #31
   
  strb  r2,  [r0, #1]
  strb  r10, [r0, #2]

  ldmfd sp!, {r4 - r12, PC}
  WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA    |.text|, CODE
    WMV_LEAF_ENTRY g_UpsampleWFilterLine10_Vert_ARMV4
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 == pDst
@ r1 == pSrc
@ r2 == x
@ r3 == size


  stmfd sp!, {r4 - r12, r14}       @ r0-r3 are preserved
  FRAME_PROFILE_COUNT

@ the temporary buffer x can be eliminated to improve cache efficiency
@ we clip and store new value to pDst after each calculation
@ we use register to save pSrc value because pDst and pSrc can point to same address
@ so after we store pDst, we can not read it back as pSrc


  ldr   r14, [sp, #40]             @ r14 == iPitch

  ldrb  r4,  [r1]                  @ pSrc[0]
  ldrb  r5,  [r1, r14, lsl #1]!    @ pSrc[2*iPitch]
  ldrb  r6,  [r1, r14, lsl #1]!    @ pSrc[4*iPitch]
  ldrb  r7,  [r1, r14, lsl #1]!    @ pSrc[6*iPitch]
  
  mov   r10, #16
  mov   r11, #25
  mov   r12, #28                   @ SW1
  
@ x[0] == (I32_WMV) ((pSrc[0] * SW1 + pSrc[0] * SW2 + pSrc[2*iPitch] * SW3 + pSrc[4*iPitch]  + 16) >> 5)@
@ x[1] == (I32_WMV) ((pSrc[0] * SW1 + pSrc[2*iPitch] * SW2 + pSrc[0] * SW3 + pSrc[2*iPitch]  + 16) >> 5)@
@ x[2] == (I32_WMV) ((pSrc[2*iPitch] * SW1 + pSrc[0] * SW2 + pSrc[4*iPitch] * SW3 + pSrc[6*iPitch]  + 16) >> 5)@
@ x[3] == (I32_WMV) ((pSrc[2*iPitch] * SW1 + pSrc[4*iPitch] * SW2 + pSrc[0] * SW3 + pSrc[0]  + 16) >> 5)@

  mla   r2,  r4,  r11, r10         @ pSrc[0]*25 + 16
  add   r8,  r4,  r4,  lsl #3      @ pSrc[0]*9
  rsb   r9,  r5,  r5,  lsl #3      @ pSrc[2*iPitch]*7
  sub   r11, r5,  r5,  lsl #2      @ pSrc[2*iPitch]*(-3)
  add   r8,  r2,  r8               @ pSrc[0]*34 + 16
  add   r11, r11, r6               @ pSrc[2*iPitch]*(-3) + pSrc[4*iPitch]
  add   r2,  r2,  r9               @ x[1]               
  add   r8,  r8,  r11              @ x[0]              

  mla   r9,  r5,  r12, r10         @ pSrc[2*iPitch]*28 + 16

  mov   r2,  r2,  asr  #5          @ pDst[iPitch], guarantee no need clip
  mov   r8,  r8,  asr  #5          @ pDst[0]

  sub  r11, r8,  #0xFF            @ CLIP pDst[0] 
  mvnne r8,  r8,  asr #31

  mov   r11, #6                    @ SW2
 
  strb  r8,  [r0]                  @ pDst[0]
  strb  r2,  [r0, r14]!            @ pDst[iPitch]


  mla   r8,  r4,  r11, r7          @ pSrc[0]*6 + pSrc[6*iPitch]
  add   r11, r6,  r6,  lsl #1      @ pSrc[4*iPitch]*3
  mov   r12, r11, lsl  #1          @ pSrc[4*iPitch]*6
  sub   r2,  r9,  r11              @ x[2]
  add   r9,  r9,  r12              @ x[3]
  add   r2,  r2,  r8               
  sub   r9,  r9,  r4,  lsl #1


  mov   r2,  r2,  asr  #5          @ pDst[2*iPitch]
  sub  r11, r2,  #0xFF            @ CLIP pDst[2*iPitch] 
  mvnne r2,  r2,  asr #31

  mov   r9,  r9,  asr  #5          @ pDst[3*iPitch]

  sub  r11, r9,  #0xFF            @ CLIP pDst[3*iPitch] 
  mvnne r9,  r9,  asr #31

  strb  r2,  [r0, r14]!            @ pDst[2*iPitch]
  strb  r9,  [r0, r14]!            @ pDst[3*iPitch]

  mov   r11, #6                    @ SW2
  mov   r12, #28                   @ SW1

  sub   r3,  r3,  #8
  mov   r3,  r3,  lsr #1

@ r11 == SW2
@ r12 == SW1
@ r10 == 16
lUV10Loop:
@ for( j == 4@ j < size - 4@ j +== 2) {
@   x[j] == (I32_WMV) ((pSrc[j*iPitch] * SW1 + pSrc[(j-2)*iPitch] * SW2 + pSrc[(j+2)*iPitch] * SW3 + pSrc[(j+4)*iPitch]  + 16) >> 5)@
@   x[j+1] == (I32_WMV) ((pSrc[j*iPitch] * SW1 + pSrc[(j+2)*iPitch] * SW2 + pSrc[(j-2)*iPitch] * SW3 + pSrc[(j-4)*iPitch]  + 16) >> 5)@
@ }

  mla   r4,  r7,  r11, r4          @ pSrc[(j+2)*iPitch]*SW2 + pSrc[(j-4)*iPitch]

  mov   r10, #16
  ldrb  r8,  [r1, r14, lsl #1]!    @ pSrc[(j+4)*iPitch]

  mla   r10, r6,  r12, r10         @ pSrc[j*iPitch]*SW1 + 16
  add   r9,  r5,  r5,  lsl #1      @ pSrc[(j-2)*iPitch]*SW3
  add   r2,  r7,  r7,  lsl #1      @ pSrc[(j+2)*iPitch]*SW3
  sub   r4,  r4,  r9               @ pSrc[(j+2)*iPitch]*SW2 + pSrc[(j-2)*iPitch]*SW3 + pSrc[(j-4)*iPitch]

  mla   r9,  r5,  r11, r8          @ pSrc[(j-2)*iPitch]*SW2 + pSrc[(j+4)*iPitch]
  add   r4,  r4,  r10              @ x[j+1]
  sub   r2,  r10, r2               @ pSrc[j*iPitch]*SW1 + pSrc[(j+2)*iPitch]*SW3 + 6

  movs  r4,  r4,  asr  #5          @ pDst[(j+1)*iPitch]
  add   r2,  r2,  r9               @ x[j]

  sub  r9,  r4,  #0xFF            @ CLIP pDst[(j+1)*iPitch] 
  mvnne r4,  r4,  asr #31

  mov   r2,  r2,  asr  #5          @ pDst[j*iPitch]

  sub  r9,  r2,  #0xFF            @ CLIP pDst[j*iPitch] 
  mvnne r2,  r2,  asr #31

  strb  r2,  [r0, r14]!            @ pDst[j*iPitch]
  strb  r4,  [r0, r14]!            @ pDst[(j+1)*iPitch]
 
  mov   r4,  r5
  mov   r5,  r6
  mov   r6,  r7
  mov   r7,  r8

  subs  r3,  r3,  #1
  bne   lUV10Loop
  

  mov   r10, #16

@ r4 == pSrc[(size-8)*iPitch]
@ r5 == pSrc[(size-6)*iPitch]
@ r6 == pSrc[(size-4)*iPitch]
@ r7 == pSrc[(size-2)*iPitch]
@ r10 == 16
@ r11 == SW2 (6)
@ r12 == SW1 (28)

@ x[size-4] == (I32_WMV) ((pSrc[(size-4)*iPitch] * SW1 + pSrc[(size-6)*iPitch] * SW2 + pSrc[(size-2)*iPitch] * SW3 + pSrc[(size-2)*iPitch]  + 16) >> 5)@
@ x[size-3] == (I32_WMV) ((pSrc[(size-4)*iPitch] * SW1 + pSrc[(size-2)*iPitch] * SW2 + pSrc[(size-6)*iPitch] * SW3 + pSrc[(size-8)*iPitch]  + 16) >> 5)@
@ x[size-2] == (I32_WMV) ((pSrc[(size-2)*iPitch] * SW1 + pSrc[(size-4)*iPitch] * SW2 + pSrc[(size-2)*iPitch] * SW3 + pSrc[(size-4)*iPitch]  + 16) >> 5)@
@ x[size-1] == (I32_WMV) ((pSrc[(size-2)*iPitch] * SW1 + pSrc[(size-2)*iPitch] * SW2 + pSrc[(size-4)*iPitch] * SW3 + pSrc[(size-6)*iPitch]  + 16) >> 5)@

  mla   r9,  r6,  r12, r10         @ pSrc[(size-4)*iPitch]*28 + 16
  add   r12, r5,  r5,  lsl #1      @ pSrc[(size-6)*iPitch]*3
  mov   r2,  r12, lsl  #1          @ pSrc[(size-6)*iPitch]*6

  mla   r8,  r7,  r11, r4          @ pSrc[(size-2)*iPitch]*6 + pSrc[(size-8)*iPitch]
  mov   r11, #25
  add   r2,  r2,  r9               @ pSrc[(size-4)*iPitch]*28 + pSrc[(size-6)*iPitch]*6 + 16
  sub   r2,  r2,  r7,  lsl #1      @ x[size-4]

  add   r8,  r8,  r9
  sub   r8,  r8,  r12              @ x[size-3]

  mla   r4,  r7,  r11, r10         @ pSrc[(size-2)*iPitch]*25 + 16

  mov   r2,  r2,  asr  #5          @ pDst[(size-4)*iPitch]

  sub  r9,  r2,  #0xFF            @ CLIP pDst[(size-4)*iPitch] 
  mvnne r2,  r2,  asr #31

  mov   r8,  r8,  asr  #5          @ pDst[(size-3)*iPitch]
  sub  r9,  r8,  #0xFF            @ CLIP pDst[(size-3)*iPitch] 
  mvnne r8,  r8,  asr #31

  strb  r2,  [r0, r14]!            @ pDst[(size-4)*iPitch]
  strb  r8,  [r0, r14]!            @ pDst[(size-3)*iPitch]

  add   r11, r7,  r7,  lsl #3      @ pSrc[(size-2)*iPitch]*9
    
  rsb   r9,  r6,  r6,  lsl #3      @ pSrc[(size-4)*iPitch]*7
  add   r12, r6,  r6,  lsl #1      @ pSrc[(size-4)*iPitch]*3
  add   r11, r11, r4               @ pSrc[(size-2)*iPitch]*34 + 16
  add   r2,  r4,  r9               @ x[size-2]
  
  sub   r12, r5,  r12              @ pSrc[(size-6)*iPitch] - 3*pSrc[(size-4)*iPitch]
  mov   r2,  r2,  asr  #5          @ pDst[(size-2)*iPitch], guarantee no need clip
  add   r8,  r11, r12              @ x[size-1]

  mov   r8,  r8,  asr  #5

  sub  r9,  r8,  #0xFF            @ CLIP pDst[(size-1)*iPitch] 
  mvnne r8,  r8,  asr #31
    
  strb  r2,  [r0, r14]!            @ pDst[(size-2)*iPitch]
  strb  r8,  [r0, r14]             @ pDst[(size-1)*iPitch]


  ldmfd sp!, {r4 - r12, PC}
  WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA    |.text|, CODE
    WMV_LEAF_ENTRY g_UpsampleWFilterLine10_Horiz_ARMV4
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 == pDst
@ r1 == pSrc
@ r2 == x
@ r3 == size

  stmfd sp!, {r4 - r12, r14}       @ r0-r3 are preserved
  FRAME_PROFILE_COUNT

@ the temporary buffer x can be eliminated to improve cache efficiency
@ we clip and store new value to pDst after each calculation
@ we use register to save pSrc value because pDst and pSrc can point to same address
@ so after we store pDst, we can not read it back as pSrc


  ldrb  r4,  [r1]                  @ pSrc[0]
  ldrb  r5,  [r1, #2]              @ pSrc[2]
  ldrb  r6,  [r1, #4]              @ pSrc[4]
  ldrb  r7,  [r1, #6]!             @ pSrc[6]
  
  mov   r10, #15
  mov   r11, #25
  mov   r12, #28                   @ SW1

@ x[0] == (I32_WMV) ((pSrc[0] * SW1 + pSrc[0] * SW2 + pSrc[2] * SW3 + pSrc[4] + 15) >> 5)@
@ x[1] == (I32_WMV) ((pSrc[0] * SW1 + pSrc[2] * SW2 + pSrc[0] * SW3 + pSrc[2] + 15) >> 5)@
@ x[2] == (I32_WMV) ((pSrc[2] * SW1 + pSrc[0] * SW2 + pSrc[4] * SW3 + pSrc[6] + 15) >> 5)@
@ x[3] == (I32_WMV) ((pSrc[2] * SW1 + pSrc[4] * SW2 + pSrc[0] * SW3 + pSrc[0] + 15) >> 5)@
  
  mla   r2,  r4,  r11, r10         @ pSrc[0]*25 + 15
  add   r8,  r4,  r4,  lsl #3      @ pSrc[0]*9
  rsb   r9,  r5,  r5,  lsl #3      @ pSrc[2]*7
  sub   r14, r5,  r5,  lsl #2      @ pSrc[2]*(-3)
  add   r8,  r2,  r8               @ pSrc[0]*34 + 15
  add   r14, r14, r6
  add   r2,  r2,  r9               
  add   r8,  r8,  r14              

  mla   r9,  r5,  r12, r10         @ pSrc[2]*28 + 15
  add   r11, r6,  r6,  lsl #1      @ pSrc[4]*3

  mov   r2,  r2,  asr  #5          @ pDst[1], guarantee no need clip
  mov   r8,  r8,  asr  #5          @ pDst[0]

  sub  r14, r8,  #0xFF            @ CLIP pDst[0] 
  mvnne r8,  r8,  asr #31

  mov   r14, #6                    @ SW2
 
  strb  r8,  [r0]                  @ pDst[0]
  strb  r2,  [r0, #1]              @ pDst[1]

  mla   r8,  r4,  r14,  r7         @ pSrc[0]*6 + pSrc[6]
  mov   r12, r11, lsl #1           @ pSrc[4]*6
  sub   r2,  r9,  r11              @ pDst[2]
  add   r9,  r9,  r12              @ pDst[3]
  add   r2,  r2,  r8               
  sub   r9,  r9,  r4,  lsl #1


  mov   r2,  r2,  asr  #5          @ pDst[2]
  sub  r8,  r2,  #0xFF            @ CLIP pDst[2] 
  mvnne r2,  r2,  asr #31


  mov   r9,  r9,  asr  #5          @ pDst[3]
  sub  r8,  r9,  #0xFF            @ CLIP pDst[3] 
  mvnne r9,  r9,  asr #31

  strb  r2,  [r0, #2]              @ pDst[2]
  strb  r9,  [r0, #3]!             @ pDst[3]

  mov   r12, #28                   @ SW1

  sub   r3,  r3,  #8
  mov   r3,  r3,  lsr #1

lUH10Loop:
@ for( j == 4@ j < size - 4@ j +== 2) {
@    x[j] == (I32_WMV) ((pSrc[j] * SW1 + pSrc[j-2] * SW2 + pSrc[j+2] * SW3 + pSrc[j+4] + 15) >> 5)@
@    x[j+1] == (I32_WMV) ((pSrc[j] * SW1 + pSrc[j+2] * SW2 + pSrc[j-2] * SW3 + pSrc[j-4] + 15) >> 5)@
@   }

  mov   r10, #15
  ldrb  r8,  [r1, #2]!             @ pSrc[j+4]

  mla   r9,  r6,  r12, r10         @ pSrc[j]*SW1 + 15
  add   r2,  r7,  r7,  lsl #1      @ pSrc[j+2]*SW3
  add   r11, r5,  r5,  lsl #1      @ pSrc[j-2]*SW3
  mla   r10, r5,  r14, r8          @ pSrc[j-2]*SW2 + pSrc[j+4]
  sub   r2,  r9,  r2               @ pDst[j]
  sub   r11, r9,  r11              @ pDst[j+1]
  add   r2,  r2,  r10
  mla   r9,  r7,  r14, r4          @ pSrc[j+2]*SW2 + pSrc[j-4]

  mov   r2,  r2,  asr  #5          @ pDst[j]
  sub  r4,  r2,  #0xFF            @ CLIP pDst[j] 
  mvnne r2,  r2,  asr #31

  add   r11, r11, r9
  mov   r11, r11, asr  #5          @ pDst[j+1]

  sub  r4,  r11, #0xFF            @ CLIP pDst[j+1] 
  mvnne r11, r11, asr #31

  strb  r2,  [r0, #1]              @ pDst[j]
  strb  r11, [r0, #2]!             @ pDst[j+1]
 
  mov   r4,  r5
  mov   r5,  r6
  mov   r6,  r7
  mov   r7,  r8

  subs  r3,  r3,  #1
  bne   lUH10Loop
  

  mov   r10, #15
  mov   r11, #25

@ r4 == pSrc[size-8]
@ r5 == pSrc[size-6]
@ r6 == pSrc[size-4]
@ r7 == pSrc[size-2]
@ r10 == 15
@ r11 == 25
@ r12 == 28 (SW1)
@ r14 == 6  (SW2)

@ x[size-4] == (I32_WMV) ((pSrc[size-4] * SW1 + pSrc[size-6] * SW2 + pSrc[size-2] * SW3 + pSrc[size-2] + 15) >> 5)@
@ x[size-3] == (I32_WMV) ((pSrc[size-4] * SW1 + pSrc[size-2] * SW2 + pSrc[size-6] * SW3 + pSrc[size-8] + 15) >> 5)@
@ x[size-2] == (I32_WMV) ((pSrc[size-2] * SW1 + pSrc[size-4] * SW2 + pSrc[size-2] * SW3 + pSrc[size-4] + 15) >> 5)@
@ x[size-1] == (I32_WMV) ((pSrc[size-2] * SW1 + pSrc[size-2] * SW2 + pSrc[size-4] * SW3 + pSrc[size-6] + 15) >> 5)@

  mla   r9,  r6,  r12, r10         @ pSrc[size-4]*28 + 15
  add   r12, r5,  r5,  lsl #1      @ pSrc[size-6]*3
  mla   r8,  r7,  r14, r4          @ pSrc[size-2]*6 + pSrc[size-8]
  mov   r2,  r12, lsl  #1          @ pSrc[size-6]*6

  mla   r4,  r7,  r11, r10         @ pSrc[size-2]*25 + 15
  add   r11, r7,  r7,  lsl #3      @ pSrc[size-2]*9

  add   r8,  r8,  r9
  add   r2,  r2,  r9
  sub   r8,  r8,  r12              @ pDst[size-3]
  sub   r2,  r2,  r7,  lsl #1      @ pDst[size-4]
  

  mov   r2,  r2,  asr  #5          @ pDst[size-4]
  sub  r9,  r2,  #0xFF            @ CLIP pDst[size-4] 
  mvnne r2,  r2,  asr #31

  mov   r8,  r8,  asr  #5          @ pDst[size-3]
  sub  r9,  r8,  #0xFF            @ CLIP pDst[size-3] 
  mvnne r8,  r8,  asr #31

  strb  r2,  [r0, #1]              @ pDst[size-4]
  strb  r8,  [r0, #2]              @ pDst[size-3]

    
  rsb   r9,  r6,  r6,  lsl #3      @ pSrc[size-4]*7
  add   r12, r6,  r6,  lsl #1      @ pSrc[size-4]*3
  add   r11, r11, r4               @ pSrc[size-2]*34 + 15
  add   r2,  r4,  r9               
  
  sub   r12, r5,  r12              @ pSrc[size-6] - 3*pSrc[size-4]
  mov   r2,  r2,  asr  #5          @ pDst[size-2], guarantee no need clip
  add   r8,  r11, r12

  mov   r8,  r8,  asr  #5
  sub  r9,  r8,  #0xFF            @ CLIP pDst[size-1] 
  mvnne r8,  r8,  asr #31
   
  strb  r2,  [r0, #3]              @ pDst[size-2]
  strb  r8,  [r0, #4]              @ pDst[size-1]


  ldmfd sp!, {r4 - r12, PC}
  
  WMV_ENTRY_END
  .endif @ WMV_OPT_MULTIRES_ARM
    
  @@.end
