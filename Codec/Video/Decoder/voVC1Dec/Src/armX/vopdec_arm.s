;//*@@@+++@@@@******************************************************************
;//
;// Microsoft Windows Media
;// Copyright (C) Microsoft Corporation. All rights reserved.
;//
;//*@@@---@@@@******************************************************************

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;
;  THIS IS ASSEMBLY VERSION OF ROUTINES IN VOPDEC9_WMV9_EMB.C WHEN 
;  WMV_OPT_DQUANT_ARM ARE DEFINED
;
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



    INCLUDE wmvdec_member_arm.inc
    INCLUDE xplatform_arm_asm.h 
    IF UNDER_CE != 0
    INCLUDE kxarm.h
    ENDIF

    AREA MOTIONCOMP, CODE, READONLY

    IF WMV_OPT_DQUANT_ARM = 1
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    EXPORT  Pack32To16_ARMV4
    EXPORT  ResetCoefACPred1_ARMV4
    EXPORT  ResetCoefACPred2_ARMV4
    EXPORT  ResetCoefBuffer8x8_ARMV4
    EXPORT  ResetCoefBuffer8x4_ARMV4
    EXPORT  ResetCoefBuffer4x8_ARMV4
    EXPORT  ResetCoefBuffer4x4_ARMV4

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    AREA    |.embsec_PMainLoopLvl1|, CODE
ResetCoefACPred1_ARMV4
;   r0:     piQuanCoefACPred
;   r1:     pRef
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    
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

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    AREA    |.embsec_PMainLoopLvl1|, CODE
ResetCoefACPred2_ARMV4
;   r0:     piQuanCoefACPred
;   r1:     pRef
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

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

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    AREA    |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY ResetCoefBuffer8x8_ARMV4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  FRAME_PROFILE_COUNT

;  MOV   r8, #8
    
   MOV   r1, #0
   MOV   r2, #0
   MOV   r3, #0
   MOV   r12,#0

;gReset8x8Loop
;  SUBS  r8, r8, #1

  STMIA r0!, {r1 - r3, r12}
  STMIA r0!, {r1 - r3, r12}
  STMIA r0!, {r1 - r3, r12}
  STMIA r0!, {r1 - r3, r12}
  STMIA r0!, {r1 - r3, r12}
  STMIA r0!, {r1 - r3, r12}
  STMIA r0!, {r1 - r3, r12}
  STMIA r0!, {r1 - r3, r12}

;  BNE   gReset8x8Loop

   MOV   PC, lr  
   WMV_ENTRY_END

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    AREA    |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY ResetCoefBuffer8x4_ARMV4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  
  FRAME_PROFILE_COUNT

  MOV   r1, #0
  MOV   r2, #8

gReset8x4Loop
  STR   r1, [r0], #4
  SUBS  r2, r2, #1
  STR   r1, [r0], #12
 
  BNE   gReset8x4Loop

  MOV   PC, lr
  WMV_ENTRY_END

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    AREA    |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY ResetCoefBuffer4x8_ARMV4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  FRAME_PROFILE_COUNT

  MOV   r1, #0
  MOV   r2, #0
  MOV   r3, #0
  MOV   r12,#0

  STMIA r0!, {r1 - r3, r12}
  STMIA r0!, {r1 - r3, r12}
  STMIA r0!, {r1 - r3, r12}
  STMIA r0!, {r1 - r3, r12}

  MOV   PC, lr
  WMV_ENTRY_END

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    AREA    |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY ResetCoefBuffer4x4_ARMV4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  FRAME_PROFILE_COUNT

  MOV   r1, #0
  MOV   r2, #4

gReset4x4Loop
  STR   r1, [r0], #4
  SUBS  r2, r2, #1
  STR   r1, [r0], #12
 
  BNE   gReset4x4Loop

  MOV   PC, lr
  WMV_ENTRY_END




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	AREA	|.text|, CODE
    WMV_LEAF_ENTRY Pack32To16_ARMV4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   r0:     ErrorBuffer            
;   r1:     CoefReconBuffer
;   
;    for ( ii = 0; ii < 64; ii++)
;                    pWMVDec->m_rgErrorBlock->i16[ii] = (I16_WMV) pWMVDec->m_rgiCoefRecon[ii];

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

  ENDIF ; WMV_OPT_DQUANT_ARM
    
  END