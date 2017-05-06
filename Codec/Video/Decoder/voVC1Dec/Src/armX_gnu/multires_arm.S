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
    .include "wmvdec_member_arm.inc"
    .include "xplatform_arm_asm.h" 
 
    @AREA MOTIONCOMP, CODE, READONLY
		.section .text

    .if WMV_OPT_MULTIRES_ARM == 1
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

 
    .global  g_DownsampleWFilterLine6_Vert_ARMV4
    .global  g_DownsampleWFilterLine6_Horiz_ARMV4
    .global  g_UpsampleWFilterLine10_Vert_ARMV4
    .global  g_UpsampleWFilterLine10_Horiz_ARMV4


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA    |.text|, CODE
    WMV_LEAF_ENTRY g_DownsampleWFilterLine6_Vert_ARMV4
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 == pDst
@ r1 == pSrc
@ r2 == x
@ r3 == size


  STMFD sp!, {r4 - r12, r14}       @ r0-r3 are preserved
  FRAME_PROFILE_COUNT

@ the temporary buffer x can be eliminated to improve cache efficiency
@ we clip and store new value to pDst after each calculation
@ we use register to save pSrc value because pDst and pSrc can point to same address
@ so after we store pDst, we can not read it back as pSrc

  LDR   r4,  [sp, #40]             @ r4 == iPitch

  LDRB  r5,  [r1]                  @ pSrc[0]
  LDRB  r6,  [r1, r4]!             @ pSrc[iPitch]
  LDRB  r7,  [r1, r4]!             @ pSrc[2*iPitch], pSrc + 2*iPitch
  LDRB  r8,  [r1, r4]!             @ pSrc[3*iPitch]

  MOV   r11, #75                   
  MOV   r12, #59
  MOV   r14, #63

@ x[0] == (I32_WMV) (((pSrc[0] + pSrc[iPitch]) * AW1 + (pSrc[2*iPitch] + pSrc[0]) * AW2 + (pSrc[3*iPitch] + pSrc[iPitch]) * AW3+ 63) >> 7)@

  MLA   r2,  r5,  r11, r14         @ pSrc[0]*75 + 63
  ADD   r9,  r7,  r7,  LSL #2      @ pSrc[2*iPitch]*5
  MVN   r11, #10                   @ -11, AW3
  MLA   r10, r6,  r12, r9          @ pSrc[iPitch]*59 + pSrc[2*iPitch]*5
  MLA   r2,  r8,  r11, r2          @ pSrc[3*iPitch]*(-11) + pSrc[0]*75 + 63

  SUB   r3,  r3,  #4
  MOV   r3,  r3,  LSR #1

  ADD   r2,  r2,  r10
  MOV   r12, #70                   @ SW1                   
  MOV   r2,  r2,  ASR #7           @ pDst[0]
  MOV   r10, #0
 
  BICS  r9,  r2,  #0xFF            @ CLIP pDst[0] 
  MVNNE r2,  r2,  ASR #31

  STRB  r2,  [r0]
  STRB  r10, [r0, r4]!

@ r11 == SW3
@ r12 == SW1
@ r14 == 63


lDV6Loop:

@ for( j == 2@ j < size -2@ j +== 2)
@     x[j] ==  (I32_WMV) (((pSrc[j*iPitch] + pSrc[(j+1)*iPitch]) * AW1 + (pSrc[(j-1)*iPitch] + pSrc[(j+2)*iPitch]) * AW2 + (pSrc[(j-2)*iPitch] + pSrc[(j+3)*iPitch]) * AW3 + 63) >> 7)@

  LDRB  r9,  [r1, r4]!             @ pSrc[(j+2)*iPitch]
  LDRB  r10, [r1, r4]!             @ pSrc[(j+3)*iPitch]

  ADD   r6,  r6,  r9               @ pSrc[(j-1)*iPitch]+pSrc[(j+2)*iPitch]
  ADD   r5,  r5,  r10              @ pSrc[(j-2)*iPitch]+pSrc[(j+3)*iPitch]
  ADD   r2,  r7,  r8               @ pSrc[j*iPitch]+pSrc[(j+1)*iPitch]
  ADD   r6,  r6,  r6,  LSL #2      @ (pSrc[(j-1)*iPitch]+pSrc[(j+2)*iPitch]) * AW2
  MLA   r14, r5,  r11, r14         @ (pSrc[(j-2)*iPitch]+pSrc[(j+3)*iPitch]) * AW3 + 63
  MLA   r6,  r2,  r12, r6          @ (pSrc[j*iPitch]+pSrc[(j+1])*iPitch) * AW1 + (pSrc[(j-1)*iPitch]+pSrc[(j+2)*iPitch]) * AW2

  ADD   r2,  r6,  r14   
  MOV   r2,  r2,  ASR  #7          @ pDst[j]

  BICS  r5,  r2,  #0xFF            @ CLIP pDst[j] 
  MVNNE r2,  r2,  ASR #31

  MOV   r6,  #0
  STRB  r2,  [r0, r4]!
  STRB  r6,  [r0, r4]!

  MOV   r5,  r7
  MOV   r6,  r8
  MOV   r7,  r9
  MOV   r8,  r10
  MOV   r14, #63

  SUBS  r3,  r3,  #1
  BNE   lDV6Loop

@ x[size-2] ==  (I32_WMV) (((pSrc[(size-2)*iPitch] + pSrc[(size-1)*iPitch]) * AW1 + (pSrc[(size-3)*iPitch] + pSrc[(size-1)*iPitch]) * AW2 + (pSrc[(size-4)*iPitch] + pSrc[(size-2)*iPitch]) * AW3 + 63) >> 7)@

  ADD   r6,  r6,  r8               @ pSrc[(size-3)*iPitch]+pSrc[(size-1)*iPitch]
  ADD   r5,  r5,  r7               @ pSrc[(size-4)*iPitch]+pSrc[(size-2)*iPitch]
  ADD   r2,  r7,  r8               @ pSrc[(size-2)*iPitch]+pSrc[(size-1)*iPitch]
  ADD   r6,  r6,  r6,  LSL #2      @ (pSrc[(size-3)*iPitch]+pSrc[(size-1)*iPitch]) * AW2
  MLA   r14, r5,  r11, r14         @ (pSrc[(size-4)*iPitch]+pSrc[(size-2)*iPitch]) * AW3 + 63
  MLA   r6,  r2,  r12, r6          @ (pSrc[(size-2)*iPitch]+pSrc[(size-1)*iPitch]) * AW1 + (pSrc[(size-3)*iPitch]+pSrc[(size-1)*iPitch]) * AW2
  ADD   r2,  r6,  r14   
  MOV   r2,  r2,  ASR  #7          @ pDst[size-2]

  BICS  r9,  r2,  #0xFF            @ CLIP pDst[size-2] 
  MVNNE r2,  r2,  ASR #31
     
  MOV   r6,  #0
  STRB  r2,  [r0, r4]!
  STRB  r6,  [r0, r4]


  LDMFD sp!, {r4 - r12, PC}
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


  STMFD sp!, {r4 - r12, r14}       @ r0-r3 are preserved
  FRAME_PROFILE_COUNT

@ the temporary buffer x can be eliminated to improve cache efficiency
@ we clip and store new value to pDst after each calculation
@ we use register to save pSrc value because pDst and pSrc can point to same address
@ so after we store pDst, we can not read it back as pSrc


  LDRB  r4,  [r1]                  @ pSrc[0]
  LDRB  r5,  [r1, #1]              @ pSrc[1]
  LDRB  r6,  [r1, #2]!             @ pSrc[2], pSrc + 2
  LDRB  r7,  [r1, #1]              @ pSrc[3]

  MOV   r10, #64
  MOV   r11, #75
  MOV   r12, #59
  MVN   r14, #10                   @ AW3

@ x[0] == (I32_WMV) (((pSrc[0] + pSrc[1]) * AW1 + (pSrc[2] + pSrc[0]) * AW2 + (pSrc[3] + pSrc[1]) * AW3+ 64) >> 7)@

  MLA   r2,  r4,  r11, r10         @ pSrc[0]*75 + 64
  ADD   r9,  r6,  r6,  LSL #2      @ pSrc[2]*5
  MLA   r10, r5,  r12, r9          @ pSrc[1]*59 + r9
  MLA   r2,  r7,  r14, r2          @ pSrc[3]*(-11) + r4
  ADD   r2,  r2,  r10
  MOV   r2,  r2,  ASR #7           @ pDst[0]

  BICS  r9,  r2,  #0xFF            @ CLIP pDst[0] 
  MVNNE r2,  r2,  ASR #31
  MOV   r10, #0
 
  STRB  r2,  [r0]
  STRB  r10, [r0, #1]!

  SUB   r3,  r3,  #4
  MOV   r3,  r3,  LSR #1

  MOV   r11, #64                   
  MOV   r12, #70                   @ AW1
  

lDH6Loop:
@ for( j == 2@ j < size -2@ j +== 2) {
@    x[j] ==  (I32_WMV) (((pSrc[j] + pSrc[j+1]) * AW1 + (pSrc[j-1] + pSrc[j+2]) * AW2 + (pSrc[j-2] + pSrc[j+3]) * AW3 + 64) >> 7)@
@ }

  LDRB  r8,  [r1, #2]!             @ pSrc[j+2], pSrc + 2
  LDRB  r9,  [r1, #1]              @ pSrc[j+3]

  ADD   r5,  r5,  r8               @ pSrc[j-1]+pSrc[j+2]
  ADD   r4,  r4,  r9               @ pSrc[j-2]+pSrc[j+3]
  ADD   r2,  r6,  r7               @ pSrc[j]+pSrc[j+1]
  ADD   r5,  r5,  r5,  LSL #2      @ (pSrc[j-1]+pSrc[j+2]) * AW2
  MLA   r11, r4,  r14, r11         @ (pSrc[j-2]+pSrc[j+3]) * AW3 + 64
  MLA   r5,  r2,  r12, r5          @ (pSrc[j]+pSrc[j+1]) * AW1 + (pSrc[j-1]+pSrc[j+2]) * AW2
  ADD   r2,  r5,  r11   
  MOV   r2,  r2,  ASR  #7          @ pDst[j]

  BICS  r4,  r2,  #0xFF            @ CLIP pDst[j] 
  MVNNE r2,  r2,  ASR #31
 
  STRB  r2,  [r0, #1]
  STRB  r10, [r0, #2]!

  MOV   r4,  r6
  MOV   r5,  r7
  MOV   r6,  r8
  MOV   r7,  r9
  MOV   r11, #64

  SUBS  r3,  r3,  #1
  BNE   lDH6Loop

@ x[size-2] ==  (I32_WMV) (((pSrc[size-2] + pSrc[size-1]) * AW1 + (pSrc[size-3] + pSrc[size-1]) * AW2 + (pSrc[size-4] + pSrc[size-2]) * AW3 + 64) >> 7)@

  ADD   r5,  r5,  r7               @ pSrc[size-3]+pSrc[size-1]
  ADD   r4,  r4,  r6               @ pSrc[size-4]+pSrc[size-2]
  ADD   r2,  r6,  r7               @ pSrc[size-2]+pSrc[size-1]
  ADD   r5,  r5,  r5,  LSL #2      @ (pSrc[size-3]+pSrc[size-1]) * AW2
  MLA   r11, r4,  r14, r11         @ (pSrc[size-4]+pSrc[size-2]) * AW3 + 64
  MLA   r5,  r2,  r12, r5          @ (pSrc[size-2]+pSrc[size-1]) * AW1 + (pSrc[size-3]+pSrc[size-1]) * AW2
  ADD   r2,  r5,  r11   
  MOV   r2,  r2,  ASR  #7          @ pDst[size-2]

  BICS  r4,  r2,  #0xFF            @ CLIP pDst[size-2] 
  MVNNE r2,  r2,  ASR #31
   
  STRB  r2,  [r0, #1]
  STRB  r10, [r0, #2]


  LDMFD sp!, {r4 - r12, PC}
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


  STMFD sp!, {r4 - r12, r14}       @ r0-r3 are preserved
  FRAME_PROFILE_COUNT

@ the temporary buffer x can be eliminated to improve cache efficiency
@ we clip and store new value to pDst after each calculation
@ we use register to save pSrc value because pDst and pSrc can point to same address
@ so after we store pDst, we can not read it back as pSrc


  LDR   r14, [sp, #40]             @ r14 == iPitch

  LDRB  r4,  [r1]                  @ pSrc[0]
  LDRB  r5,  [r1, r14, LSL #1]!    @ pSrc[2*iPitch]
  LDRB  r6,  [r1, r14, LSL #1]!    @ pSrc[4*iPitch]
  LDRB  r7,  [r1, r14, LSL #1]!    @ pSrc[6*iPitch]
  
  MOV   r10, #16
  MOV   r11, #25
  MOV   r12, #28                   @ SW1
  
@ x[0] == (I32_WMV) ((pSrc[0] * SW1 + pSrc[0] * SW2 + pSrc[2*iPitch] * SW3 + pSrc[4*iPitch]  + 16) >> 5)@
@ x[1] == (I32_WMV) ((pSrc[0] * SW1 + pSrc[2*iPitch] * SW2 + pSrc[0] * SW3 + pSrc[2*iPitch]  + 16) >> 5)@
@ x[2] == (I32_WMV) ((pSrc[2*iPitch] * SW1 + pSrc[0] * SW2 + pSrc[4*iPitch] * SW3 + pSrc[6*iPitch]  + 16) >> 5)@
@ x[3] == (I32_WMV) ((pSrc[2*iPitch] * SW1 + pSrc[4*iPitch] * SW2 + pSrc[0] * SW3 + pSrc[0]  + 16) >> 5)@

  MLA   r2,  r4,  r11, r10         @ pSrc[0]*25 + 16
  ADD   r8,  r4,  r4,  LSL #3      @ pSrc[0]*9
  RSB   r9,  r5,  r5,  LSL #3      @ pSrc[2*iPitch]*7
  SUB   r11, r5,  r5,  LSL #2      @ pSrc[2*iPitch]*(-3)
  ADD   r8,  r2,  r8               @ pSrc[0]*34 + 16
  ADD   r11, r11, r6               @ pSrc[2*iPitch]*(-3) + pSrc[4*iPitch]
  ADD   r2,  r2,  r9               @ x[1]               
  ADD   r8,  r8,  r11              @ x[0]              

  MLA   r9,  r5,  r12, r10         @ pSrc[2*iPitch]*28 + 16

  MOV   r2,  r2,  ASR  #5          @ pDst[iPitch], guarantee no need clip
  MOV   r8,  r8,  ASR  #5          @ pDst[0]

  BICS  r11, r8,  #0xFF            @ CLIP pDst[0] 
  MVNNE r8,  r8,  ASR #31


  MOV   r11, #6                    @ SW2
 
  STRB  r8,  [r0]                  @ pDst[0]
  STRB  r2,  [r0, r14]!            @ pDst[iPitch]


  MLA   r8,  r4,  r11, r7          @ pSrc[0]*6 + pSrc[6*iPitch]
  ADD   r11, r6,  r6,  LSL #1      @ pSrc[4*iPitch]*3
  MOV   r12, r11, LSL  #1          @ pSrc[4*iPitch]*6
  SUB   r2,  r9,  r11              @ x[2]
  ADD   r9,  r9,  r12              @ x[3]
  ADD   r2,  r2,  r8               
  SUB   r9,  r9,  r4,  LSL #1


  MOV   r2,  r2,  ASR  #5          @ pDst[2*iPitch]
  BICS  r11, r2,  #0xFF            @ CLIP pDst[2*iPitch] 
  MVNNE r2,  r2,  ASR #31


  MOV   r9,  r9,  ASR  #5          @ pDst[3*iPitch]

  BICS  r11, r9,  #0xFF            @ CLIP pDst[3*iPitch] 
  MVNNE r9,  r9,  ASR #31

  STRB  r2,  [r0, r14]!            @ pDst[2*iPitch]
  STRB  r9,  [r0, r14]!            @ pDst[3*iPitch]

  MOV   r11, #6                    @ SW2
  MOV   r12, #28                   @ SW1

  SUB   r3,  r3,  #8
  MOV   r3,  r3,  LSR #1

@ r11 == SW2
@ r12 == SW1
@ r10 == 16
lUV10Loop:
@ for( j == 4@ j < size - 4@ j +== 2) {
@   x[j] == (I32_WMV) ((pSrc[j*iPitch] * SW1 + pSrc[(j-2)*iPitch] * SW2 + pSrc[(j+2)*iPitch] * SW3 + pSrc[(j+4)*iPitch]  + 16) >> 5)@
@   x[j+1] == (I32_WMV) ((pSrc[j*iPitch] * SW1 + pSrc[(j+2)*iPitch] * SW2 + pSrc[(j-2)*iPitch] * SW3 + pSrc[(j-4)*iPitch]  + 16) >> 5)@
@ }

  MLA   r4,  r7,  r11, r4          @ pSrc[(j+2)*iPitch]*SW2 + pSrc[(j-4)*iPitch]

  MOV   r10, #16
  LDRB  r8,  [r1, r14, LSL #1]!    @ pSrc[(j+4)*iPitch]

  MLA   r10, r6,  r12, r10         @ pSrc[j*iPitch]*SW1 + 16
  ADD   r9,  r5,  r5,  LSL #1      @ pSrc[(j-2)*iPitch]*SW3
  ADD   r2,  r7,  r7,  LSL #1      @ pSrc[(j+2)*iPitch]*SW3
  SUB   r4,  r4,  r9               @ pSrc[(j+2)*iPitch]*SW2 + pSrc[(j-2)*iPitch]*SW3 + pSrc[(j-4)*iPitch]

  MLA   r9,  r5,  r11, r8          @ pSrc[(j-2)*iPitch]*SW2 + pSrc[(j+4)*iPitch]
  ADD   r4,  r4,  r10              @ x[j+1]
  SUB   r2,  r10, r2               @ pSrc[j*iPitch]*SW1 + pSrc[(j+2)*iPitch]*SW3 + 6

  MOVS  r4,  r4,  ASR  #5          @ pDst[(j+1)*iPitch]
  ADD   r2,  r2,  r9               @ x[j]

  BICS  r9,  r4,  #0xFF            @ CLIP pDst[(j+1)*iPitch] 
  MVNNE r4,  r4,  ASR #31

  MOV   r2,  r2,  ASR  #5          @ pDst[j*iPitch]

  BICS  r9,  r2,  #0xFF            @ CLIP pDst[j*iPitch] 
  MVNNE r2,  r2,  ASR #31

  STRB  r2,  [r0, r14]!            @ pDst[j*iPitch]
  STRB  r4,  [r0, r14]!            @ pDst[(j+1)*iPitch]
 
  MOV   r4,  r5
  MOV   r5,  r6
  MOV   r6,  r7
  MOV   r7,  r8

  SUBS  r3,  r3,  #1
  BNE   lUV10Loop
  

  MOV   r10, #16

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

  MLA   r9,  r6,  r12, r10         @ pSrc[(size-4)*iPitch]*28 + 16
  ADD   r12, r5,  r5,  LSL #1      @ pSrc[(size-6)*iPitch]*3
  MOV   r2,  r12, LSL  #1          @ pSrc[(size-6)*iPitch]*6

  MLA   r8,  r7,  r11, r4          @ pSrc[(size-2)*iPitch]*6 + pSrc[(size-8)*iPitch]
  MOV   r11, #25
  ADD   r2,  r2,  r9               @ pSrc[(size-4)*iPitch]*28 + pSrc[(size-6)*iPitch]*6 + 16
  SUB   r2,  r2,  r7,  LSL #1      @ x[size-4]

  ADD   r8,  r8,  r9
  SUB   r8,  r8,  r12              @ x[size-3]

  MLA   r4,  r7,  r11, r10         @ pSrc[(size-2)*iPitch]*25 + 16

  MOV   r2,  r2,  ASR  #5          @ pDst[(size-4)*iPitch]

  BICS  r9,  r2,  #0xFF            @ CLIP pDst[(size-4)*iPitch] 
  MVNNE r2,  r2,  ASR #31

  MOV   r8,  r8,  ASR  #5          @ pDst[(size-3)*iPitch]
  BICS  r9,  r8,  #0xFF            @ CLIP pDst[(size-3)*iPitch] 
  MVNNE r8,  r8,  ASR #31


  STRB  r2,  [r0, r14]!            @ pDst[(size-4)*iPitch]
  STRB  r8,  [r0, r14]!            @ pDst[(size-3)*iPitch]

  ADD   r11, r7,  r7,  LSL #3      @ pSrc[(size-2)*iPitch]*9
    
  RSB   r9,  r6,  r6,  LSL #3      @ pSrc[(size-4)*iPitch]*7
  ADD   r12, r6,  r6,  LSL #1      @ pSrc[(size-4)*iPitch]*3
  ADD   r11, r11, r4               @ pSrc[(size-2)*iPitch]*34 + 16
  ADD   r2,  r4,  r9               @ x[size-2]
  
  SUB   r12, r5,  r12              @ pSrc[(size-6)*iPitch] - 3*pSrc[(size-4)*iPitch]
  MOV   r2,  r2,  ASR  #5          @ pDst[(size-2)*iPitch], guarantee no need clip
  ADD   r8,  r11, r12              @ x[size-1]

  MOV   r8,  r8,  ASR  #5

  BICS  r9,  r8,  #0xFF            @ CLIP pDst[(size-1)*iPitch] 
  MVNNE r8,  r8,  ASR #31
    
  STRB  r2,  [r0, r14]!            @ pDst[(size-2)*iPitch]
  STRB  r8,  [r0, r14]             @ pDst[(size-1)*iPitch]


  LDMFD sp!, {r4 - r12, PC}
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

  STMFD sp!, {r4 - r12, r14}       @ r0-r3 are preserved
  FRAME_PROFILE_COUNT

@ the temporary buffer x can be eliminated to improve cache efficiency
@ we clip and store new value to pDst after each calculation
@ we use register to save pSrc value because pDst and pSrc can point to same address
@ so after we store pDst, we can not read it back as pSrc


  LDRB  r4,  [r1]                  @ pSrc[0]
  LDRB  r5,  [r1, #2]              @ pSrc[2]
  LDRB  r6,  [r1, #4]              @ pSrc[4]
  LDRB  r7,  [r1, #6]!             @ pSrc[6]
  
  MOV   r10, #15
  MOV   r11, #25
  MOV   r12, #28                   @ SW1

@ x[0] == (I32_WMV) ((pSrc[0] * SW1 + pSrc[0] * SW2 + pSrc[2] * SW3 + pSrc[4] + 15) >> 5)@
@ x[1] == (I32_WMV) ((pSrc[0] * SW1 + pSrc[2] * SW2 + pSrc[0] * SW3 + pSrc[2] + 15) >> 5)@
@ x[2] == (I32_WMV) ((pSrc[2] * SW1 + pSrc[0] * SW2 + pSrc[4] * SW3 + pSrc[6] + 15) >> 5)@
@ x[3] == (I32_WMV) ((pSrc[2] * SW1 + pSrc[4] * SW2 + pSrc[0] * SW3 + pSrc[0] + 15) >> 5)@
  
  MLA   r2,  r4,  r11, r10         @ pSrc[0]*25 + 15
  ADD   r8,  r4,  r4,  LSL #3      @ pSrc[0]*9
  RSB   r9,  r5,  r5,  LSL #3      @ pSrc[2]*7
  SUB   r14, r5,  r5,  LSL #2      @ pSrc[2]*(-3)
  ADD   r8,  r2,  r8               @ pSrc[0]*34 + 15
  ADD   r14, r14, r6
  ADD   r2,  r2,  r9               
  ADD   r8,  r8,  r14              

  MLA   r9,  r5,  r12, r10         @ pSrc[2]*28 + 15
  ADD   r11, r6,  r6,  LSL #1      @ pSrc[4]*3

  MOV   r2,  r2,  ASR  #5          @ pDst[1], guarantee no need clip
  MOV   r8,  r8,  ASR  #5          @ pDst[0]

  BICS  r14, r8,  #0xFF            @ CLIP pDst[0] 
  MVNNE r8,  r8,  ASR #31

  MOV   r14, #6                    @ SW2
 
  STRB  r8,  [r0]                  @ pDst[0]
  STRB  r2,  [r0, #1]              @ pDst[1]

  MLA   r8,  r4,  r14,  r7         @ pSrc[0]*6 + pSrc[6]
  MOV   r12, r11, LSL #1           @ pSrc[4]*6
  SUB   r2,  r9,  r11              @ pDst[2]
  ADD   r9,  r9,  r12              @ pDst[3]
  ADD   r2,  r2,  r8               
  SUB   r9,  r9,  r4,  LSL #1


  MOV   r2,  r2,  ASR  #5          @ pDst[2]
  BICS  r8,  r2,  #0xFF            @ CLIP pDst[2] 
  MVNNE r2,  r2,  ASR #31


  MOV   r9,  r9,  ASR  #5          @ pDst[3]
  BICS  r8,  r9,  #0xFF            @ CLIP pDst[3] 
  MVNNE r9,  r9,  ASR #31

  STRB  r2,  [r0, #2]              @ pDst[2]
  STRB  r9,  [r0, #3]!             @ pDst[3]

  MOV   r12, #28                   @ SW1

  SUB   r3,  r3,  #8
  MOV   r3,  r3,  LSR #1

lUH10Loop:
@ for( j == 4@ j < size - 4@ j +== 2) {
@    x[j] == (I32_WMV) ((pSrc[j] * SW1 + pSrc[j-2] * SW2 + pSrc[j+2] * SW3 + pSrc[j+4] + 15) >> 5)@
@    x[j+1] == (I32_WMV) ((pSrc[j] * SW1 + pSrc[j+2] * SW2 + pSrc[j-2] * SW3 + pSrc[j-4] + 15) >> 5)@
@   }

  MOV   r10, #15
  LDRB  r8,  [r1, #2]!             @ pSrc[j+4]

  MLA   r9,  r6,  r12, r10         @ pSrc[j]*SW1 + 15
  ADD   r2,  r7,  r7,  LSL #1      @ pSrc[j+2]*SW3
  ADD   r11, r5,  r5,  LSL #1      @ pSrc[j-2]*SW3
  MLA   r10, r5,  r14, r8          @ pSrc[j-2]*SW2 + pSrc[j+4]
  SUB   r2,  r9,  r2               @ pDst[j]
  SUB   r11, r9,  r11              @ pDst[j+1]
  ADD   r2,  r2,  r10
  MLA   r9,  r7,  r14, r4          @ pSrc[j+2]*SW2 + pSrc[j-4]

  MOV   r2,  r2,  ASR  #5          @ pDst[j]
  BICS  r4,  r2,  #0xFF            @ CLIP pDst[j] 
  MVNNE r2,  r2,  ASR #31

  ADD   r11, r11, r9
  MOV   r11, r11, ASR  #5          @ pDst[j+1]

  BICS  r4,  r11, #0xFF            @ CLIP pDst[j+1] 
  MVNNE r11, r11, ASR #31

  STRB  r2,  [r0, #1]              @ pDst[j]
  STRB  r11, [r0, #2]!             @ pDst[j+1]
 
  MOV   r4,  r5
  MOV   r5,  r6
  MOV   r6,  r7
  MOV   r7,  r8

  SUBS  r3,  r3,  #1
  BNE   lUH10Loop
  

  MOV   r10, #15
  MOV   r11, #25

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

  MLA   r9,  r6,  r12, r10         @ pSrc[size-4]*28 + 15
  ADD   r12, r5,  r5,  LSL #1      @ pSrc[size-6]*3
  MLA   r8,  r7,  r14, r4          @ pSrc[size-2]*6 + pSrc[size-8]
  MOV   r2,  r12, LSL  #1          @ pSrc[size-6]*6

  MLA   r4,  r7,  r11, r10         @ pSrc[size-2]*25 + 15
  ADD   r11, r7,  r7,  LSL #3      @ pSrc[size-2]*9

  ADD   r8,  r8,  r9
  ADD   r2,  r2,  r9
  SUB   r8,  r8,  r12              @ pDst[size-3]
  SUB   r2,  r2,  r7,  LSL #1      @ pDst[size-4]
  

  MOV   r2,  r2,  ASR  #5          @ pDst[size-4]
  BICS  r9,  r2,  #0xFF            @ CLIP pDst[size-4] 
  MVNNE r2,  r2,  ASR #31

  MOV   r8,  r8,  ASR  #5          @ pDst[size-3]
  BICS  r9,  r8,  #0xFF            @ CLIP pDst[size-3] 
  MVNNE r8,  r8,  ASR #31

  STRB  r2,  [r0, #1]              @ pDst[size-4]
  STRB  r8,  [r0, #2]              @ pDst[size-3]

    
  RSB   r9,  r6,  r6,  LSL #3      @ pSrc[size-4]*7
  ADD   r12, r6,  r6,  LSL #1      @ pSrc[size-4]*3
  ADD   r11, r11, r4               @ pSrc[size-2]*34 + 15
  ADD   r2,  r4,  r9               
  
  SUB   r12, r5,  r12              @ pSrc[size-6] - 3*pSrc[size-4]
  MOV   r2,  r2,  ASR  #5          @ pDst[size-2], guarantee no need clip
  ADD   r8,  r11, r12

  MOV   r8,  r8,  ASR  #5
  BICS  r9,  r8,  #0xFF            @ CLIP pDst[size-1] 
  MVNNE r8,  r8,  ASR #31
 
   
  STRB  r2,  [r0, #3]              @ pDst[size-2]
  STRB  r8,  [r0, #4]              @ pDst[size-1]


  LDMFD sp!, {r4 - r12, PC}
  
  WMV_ENTRY_END
  .endif @ WMV_OPT_MULTIRES_ARM
    
  .END
