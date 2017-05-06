@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@
@  THIS IS ASSEMBLY VERSION OF ROUTINES IN VOPDEC9_WMV9_EMB.C WHEN 
@  WMV_OPT_DQUANT_ARM ARE DEFINED
@
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    #include "../c/voWMVDecID.h"
    .include "wmvdec_member_arm.h"
    .include "xplatform_arm_asm.h" 

    @AREA MOTIONCOMP, CODE, READONLY
	 .text
	 .align 4

    .if WMV_OPT_DQUANT_ARM == 1
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

 	
    .globl  _Pack32To16
    .globl  _ResetCoefACPred1
    .globl  _ResetCoefACPred2
    .globl  _ResetCoefBuffer8x8
    .globl  _ResetCoefBuffer8x4
    .globl  _ResetCoefBuffer4x8
    .globl  _ResetCoefBuffer4x4

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA    |.embsec_PMainLoopLvl1|, CODE
	WMV_LEAF_ENTRY ResetCoefACPred1
@   r0:     piQuanCoefACPred
@   r1:     pRef
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
    mov     r12, #0
    ldrsh   r2, [r1, #2]
    ldrsh   r3, [r1, #4]
    strh    r12, [r0, #18]
    strh    r12, [r0, #20]
    strh    r2, [r0, #2]
    strh    r3, [r0, #4]
    ldrsh   r2, [r1, #6]
    ldrsh   r3, [r1, #8]
    strh    r12, [r0, #22]
    strh    r12, [r0, #24]
    strh    r2, [r0, #6]
    strh    r3, [r0, #8]
    ldrsh   r2, [r1, #10]
    ldrsh   r3, [r1, #12]
    strh    r12, [r0, #26]
    strh    r12, [r0, #28]
    strh    r2, [r0, #10]
    ldrsh   r2, [r1, #14]
    strh    r3, [r0, #12]
    strh    r12, [r0, #30]
    strh    r2, [r0, #14]
    mov     PC, r14

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA    |.embsec_PMainLoopLvl1|, CODE
	WMV_LEAF_ENTRY ResetCoefACPred2
@   r0:     piQuanCoefACPred
@   r1:     pRef
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    mov     r12, #0
    ldrsh   r2, [r1, #18]
    ldrsh   r3, [r1, #20]
    strh    r12, [r0, #2]
    strh    r12, [r0, #4]
    strh    r2, [r0, #18]
    strh    r3, [r0, #20]
    ldrsh   r2, [r1, #22]
    ldrsh   r3, [r1, #24]
    strh    r12, [r0, #6]
    strh    r12, [r0, #8]
    strh    r2, [r0, #22]
    strh    r3, [r0, #24]
    ldrsh   r2, [r1, #26]
    ldrsh   r3, [r1, #28]
    strh    r12, [r0, #10]
    strh    r12, [r0, #12]
    strh    r2, [r0, #26]
    ldrsh   r2, [r1, #30]
    strh    r3, [r0, #28]
    strh    r12, [r0, #14]
    strh    r2, [r0, #30]
    mov     PC, r14

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA    |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY ResetCoefBuffer8x8
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  FRAME_PROFILE_COUNT

@  mov   r8, #8
    
   mov   r1, #0
   mov   r2, #0
   mov   r3, #0
   mov   r12,#0

@gReset8x8Loop
@  subs  r8, r8, #1

  stmia r0!, {r1 - r3, r12}
  stmia r0!, {r1 - r3, r12}
  stmia r0!, {r1 - r3, r12}
  stmia r0!, {r1 - r3, r12}
  stmia r0!, {r1 - r3, r12}
  stmia r0!, {r1 - r3, r12}
  stmia r0!, {r1 - r3, r12}
  stmia r0!, {r1 - r3, r12}

@  bne   gReset8x8Loop

   mov   PC, lr  
   WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA    |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY ResetCoefBuffer8x4
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  
  FRAME_PROFILE_COUNT

  mov   r1, #0
  mov   r2, #8

gReset8x4Loop:
  str   r1, [r0], #4
  subs  r2, r2, #1
  str   r1, [r0], #12
 
  bne   gReset8x4Loop

  mov   PC, lr
  WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA    |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY ResetCoefBuffer4x8
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  FRAME_PROFILE_COUNT

  mov   r1, #0
  mov   r2, #0
  mov   r3, #0
  mov   r12,#0

  stmia r0!, {r1 - r3, r12}
  stmia r0!, {r1 - r3, r12}
  stmia r0!, {r1 - r3, r12}
  stmia r0!, {r1 - r3, r12}

  mov   PC, lr
  WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA    |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY ResetCoefBuffer4x4
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  FRAME_PROFILE_COUNT

  mov   r1, #0
  mov   r2, #4

gReset4x4Loop:
  str   r1, [r0], #4
  subs  r2, r2, #1
  str   r1, [r0], #12
 
  bne   gReset4x4Loop

  mov   PC, lr
  WMV_ENTRY_END




@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@AREA	|.text|, CODE
    WMV_LEAF_ENTRY Pack32To16
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   r0:     ErrorBuffer            
@   r1:     CoefReconBuffer
@   
@    for ( ii == 0@ ii < 64@ ii++)
@                    pWMVDec->m_rgErrorBlock->i16[ii] == (I16_WMV) pWMVDec->m_rgiCoefRecon[ii]@

    ldr     r2, [r1], #4
    ldr     r3, [r1], #4
    ldr     r12, [r1], #4
    strh    r2, [r0], #2
    strh    r3, [r0], #2
    strh    r12, [r0], #2
    ldr     r2, [r1], #4
    ldr     r3, [r1], #4
    ldr     r12, [r1], #4
    strh    r2, [r0], #2
    strh    r3, [r0], #2
    strh    r12, [r0], #2

    ldr     r2, [r1], #4
    ldr     r3, [r1], #4
    ldr     r12, [r1], #4
    strh    r2, [r0], #2
    strh    r3, [r0], #2
    strh    r12, [r0], #2
    ldr     r2, [r1], #4
    ldr     r3, [r1], #4
    ldr     r12, [r1], #4
    strh    r2, [r0], #2
    strh    r3, [r0], #2
    strh    r12, [r0], #2

    ldr     r2, [r1], #4
    ldr     r3, [r1], #4
    ldr     r12, [r1], #4
    strh    r2, [r0], #2
    strh    r3, [r0], #2
    strh    r12, [r0], #2
    ldr     r2, [r1], #4
    ldr     r3, [r1], #4
    ldr     r12, [r1], #4
    strh    r2, [r0], #2
    strh    r3, [r0], #2
    strh    r12, [r0], #2

    ldr     r2, [r1], #4
    ldr     r3, [r1], #4
    ldr     r12, [r1], #4
    strh    r2, [r0], #2
    strh    r3, [r0], #2
    strh    r12, [r0], #2
    ldr     r2, [r1], #4
    ldr     r3, [r1], #4
    ldr     r12, [r1], #4
    strh    r2, [r0], #2
    strh    r3, [r0], #2
    strh    r12, [r0], #2

    ldr     r2, [r1], #4
    ldr     r3, [r1], #4
    ldr     r12, [r1], #4
    strh    r2, [r0], #2
    strh    r3, [r0], #2
    strh    r12, [r0], #2
    ldr     r2, [r1], #4
    ldr     r3, [r1], #4
    ldr     r12, [r1], #4
    strh    r2, [r0], #2
    strh    r3, [r0], #2
    strh    r12, [r0], #2

    ldr     r2, [r1], #4
    ldr     r3, [r1], #4
    ldr     r12, [r1], #4
    strh    r2, [r0], #2
    strh    r3, [r0], #2
    strh    r12, [r0], #2
    ldr     r2, [r1], #4
    ldr     r3, [r1], #4
    ldr     r12, [r1], #4
    strh    r2, [r0], #2
    strh    r3, [r0], #2
    strh    r12, [r0], #2

    ldr     r2, [r1], #4
    ldr     r3, [r1], #4
    ldr     r12, [r1], #4
    strh    r2, [r0], #2
    strh    r3, [r0], #2
    strh    r12, [r0], #2
    ldr     r2, [r1], #4
    ldr     r3, [r1], #4
    ldr     r12, [r1], #4
    strh    r2, [r0], #2
    strh    r3, [r0], #2
    strh    r12, [r0], #2

    ldr     r2, [r1], #4
    ldr     r3, [r1], #4
    ldr     r12, [r1], #4
    strh    r2, [r0], #2
    strh    r3, [r0], #2
    strh    r12, [r0], #2
    ldr     r2, [r1], #4
    ldr     r3, [r1], #4
    ldr     r12, [r1], #4
    strh    r2, [r0], #2
    strh    r3, [r0], #2
    strh    r12, [r0], #2

    ldr     r2, [r1], #4
    ldr     r3, [r1], #4
    ldr     r12, [r1], #4
    strh    r2, [r0], #2
    strh    r3, [r0], #2
    strh    r12, [r0], #2
    ldr     r2, [r1], #4
    ldr     r3, [r1], #4
    ldr     r12, [r1], #4
    strh    r2, [r0], #2
    strh    r3, [r0], #2
    strh    r12, [r0], #2

    ldr     r2, [r1], #4
    ldr     r3, [r1], #4
    ldr     r12, [r1], #4
    strh    r2, [r0], #2
    strh    r3, [r0], #2
    strh    r12, [r0], #2
    ldr     r2, [r1], #4
    ldr     r3, [r1], #4
    ldr     r12, [r1], #4
    strh    r2, [r0], #2
    strh    r3, [r0], #2
    strh    r12, [r0], #2

    ldr     r2, [r1], #4
    ldr     r3, [r1], #4
    ldr     r12, [r1], #4
    strh    r2, [r0], #2
    ldr     r2, [r1], #4
    strh    r3, [r0], #2
    strh    r12, [r0], #2
    strh    r2, [r0], #2

    mov     PC, r14
    WMV_ENTRY_END

  .endif @ WMV_OPT_DQUANT_ARM
    
  @@.end
