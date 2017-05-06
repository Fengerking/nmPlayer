;//*@@@+++@@@@******************************************************************
;//
;// Microsoft Windows Media
;// Copyright (C) Microsoft Corporation. All rights reserved.
;//
;//*@@@---@@@@******************************************************************

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;
;  THIS IS ASSEMBLY VERSION OF ROUTINES IN POSTFILTER_WMV.CPP WHEN 
;  SUPPORT_POSTFILTERS AND OPT_POSTFILTER_DEBLOCK ARE DEFINED
;
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;




    INCLUDE wmvdec_member_arm.inc
    INCLUDE xplatform_arm_asm.h 
    IF UNDER_CE != 0
    INCLUDE kxarm.h
    ENDIF

    AREA DEBLOCK, CODE, READONLY

    IF WMV_OPT_PPL_ARM = 1
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

 

    EXPORT  FilterEdge_Short
    EXPORT  FilterEdge

;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        AREA |.FilterEdge_Short|, CODE, READONLY

        WMV_LEAF_ENTRY FilterEdge_Short
;
        STMFD    sp!,{r4-r11,lr}
        FRAME_PROFILE_COUNT
;
;Reg Usage:
;  r0: pWMVDec
;  r1: ppxlcCenter
;  r2: iPixelDistance
;  r3: iPixelIncrement
;
;Stack Usage:
;  0x00: pV4
;  0x04: iPixelDistance
;  0x08: i
;
        SUB      sp, sp, #0x0c
        SUB      r1, r1, r2, LSL #1
        STR      r3, [sp, #0x04]    ; Store iPixelIncrement; r3 is free
        MOV      r0, #0             ; r0 = i = 0
        B        |forLoop|          
;
;The for loop Begins
;
|loopBegin|
;
;Reg Usage: r4-r11 = v1-v8
;           r1 = pWMVDec->m_rgiClapTabDec, r2 = iPixelDistance
;           Available: r12, lr(r14), r3, r0
;           Reserved: r13(sp), r15(pc)
;
;Reg Status: r6 = pV3
;
        TST      r0, #3             ; if (i&3)==0
        BNE      |skipCalcThreshold|
;
;Calc Theshold eq_cnt
;
        LDRB     r4,  [r1, -r2, ASL #1]      ; pV1 = pV3 - 2 * iPixelDistance
        ADD      lr,  r1, r2, LSL #2         ; pV7 = pV3 + 4 * iPixelDistance
        LDRB     r6,  [r1]                   ; pV3
        LDRB     r7,  [r1, r2]               ; pV4 = pV3 + iPixelDistance
        LDRB     r5,  [r1, -r2]              ; pV2 = pV3 - iPixelDistance
;
        SUB      r12, r7, r6                 ; v4-v3
        LDRB     r8,  [lr, -r2, ASL #1]      ; pV5 = pV7 - 2*iPixelDistance
        ADD      r12, r12, #2                ; v4-v3+2
        LDRB     r9,  [lr, -r2]              ; pV6 = pV7 - iPixelDistance
        CMP      r12, #4              ; 
        LDRB     r11, [lr, r2]               ; pV8 = pV7 + iPixelDistance
        MOVHI    r12, #0              ; v4-v3+2 <= 4
        LDRB     r10, [lr]                   ; pV7
        MOVLS    r12, #1              ; r12 = phi(v4-v3)
;
        SUB      lr,  r8, r7          ; v5-v4
        ADD      lr,  lr, #2          ; v5-v4+2
        CMP      lr,  #4              ;
        ADDLS    r12, r12, #1
;
        SUB      lr, r9, r8           ; v6-v5
        ADD      lr, lr, #2           ; v6-v5+2
        CMP      lr, #4               ; 
        ADDLS    r12, r12, #1
;
        SUB      lr, r10, r9          ; v7-v6
        ADD      lr, lr, #2           ; v7-v6+2
        CMP      lr, #4               ; 
        ADDLS    r12, r12, #1
;
        CMP      r12,#2               ; if (eq_cnt<2)
        BLT      |easyDeblock|
;
;otherwise, continue to calc eq_cnt
;
        SUB      lr, r6, r5           ; v3-v2
        ADD      lr, lr, #2           ; v3-v2+2
        CMP      lr, #4               ; 
        ADDLS    r12, r12, #1
;
        SUB      lr, r5, r4           ; v2-v1
        ADD      lr, lr, #2           ; v2-v1 + 2
        CMP      lr, #4               ; 
        ADDLS    r12, r12, #1
;
        SUB      lr, r11, r10         ; v8-v7
        ADD      lr, lr, #2           ; v8-v7 + 2
        CMP      lr, #4               ; 
        ADDLS    r12, r12, #1
        CMP      r12, #5
        BLT      |easyDeblock|        ; if (eq_cnt < THR2-1)
        B        |fullDeblock|
;        
|skipCalcThreshold|
        CMP      r12, #5
        BLT      |beforeEasyDeblock|        ; if (eq_cnt < THR2-1)
;
;Load the rest registers
        LDRB     r4,  [r1, -r2, ASL #1]      ; pV1 = pV3 - 2 * iPixelDistance
        ADD      lr,  r1, r2, LSL #2         ; pV7 = pV3 + 4 * iPixelDistance
        LDRB     r5,  [r1, -r2]              ; pV2 = pV3 - iPixelDistance
        LDRB     r7,  [r1, r2]               ; pV4 = pV3 + iPixelDistance
        LDRB     r6,  [r1]                   ; pV3
;
        LDRB     r8,  [lr, -r2, ASL #1]      ; pV5 = pV7 - 2*iPixelDistance
        LDRB     r9,  [lr, -r2]              ; pV6 = pV7 - iPixelDistance
        LDRB     r11, [lr, r2]               ; pV8 = pV7 + iPixelDistance
        LDRB     r10, [lr]                   ; pV7
;
|fullDeblock|
;
;otherwise execute full deblock
;
;lr = max, r3=min
;
  IF _XSC_=1
        PLD      [r1, #32]            ; Cheap preload gets 16 of 21 lines needed in the future at low cost
  ENDIF

        CMP      r5, r10              ; if (v2>=v7)
        MOVLT    lr, r10
        MOVLT    r3, r5
        MOVGE    lr, r5
        MOVGE    r3, r10
;
        CMP      r3, r7                ; if (min > v4)
        MOVGT    r3, r7
        BGT      |bgt4to6|
        CMP      r7, lr                ; if (v4 > max)
        MOVGT    lr, r7
|bgt4to6|
        CMP      r3, r9                ; if (min > v6)
        MOVGT    r3, r9
        BGT      |bgt6to3|
        CMP      r9, lr                ; if (v6 > max)
        MOVGT    lr, r9
|bgt6to3|
        CMP      r3, r6                ; if (min > v3)
        MOVGT    r3, r6
        BGT      |bgt3to5|
        CMP      r6, lr                ; if (v3 > max)
        MOVGT    lr, r6
|bgt3to5|
        CMP      r3, r8                ; if (min > v5)
        MOVGT    r3, r8
        BGT      |bgt5|
        CMP      r8, lr                ; if (v5 > max)
        MOVGT    lr, r8
|bgt5|
        SUB      r3, lr, r3            ; max - min
        LDR      lr, [sp,#0x34]        ; iStepSize
        CMP      r3,lr,LSL #1          ; if (max-min<2*iStepSize)
        BGE      |doneDeblock|         ; Don't do anything if large than threshold
;
;otherwise Modify neighborhood points.
;
;
        SUBS     r3, r5, r4             ; v2-v1
        RSBLT    r3, r3, #0             ; abs(v2-v1)
        CMP      r3, lr                 ; if (abs(v2-v1)>=iStepSize)
        MOVGE    r4, r5                 ; v1=v2
;
        SUBS     r3, r11, r10           ; v8-v7
        RSBLT    r3, r3, #0
        CMP      r3, lr
        MOVGE    r11, r10               ; v8=v7
;
;Register Contention: Save r0.
;
;        STR      r0, [sp, #0x08]        ; Save i, register contention later
;
;r0, r2, lr is free
;
        ADD      r3, r4, r4, LSL #1     ; 3*v1
        ADD      r3, r3, r5, LSL #1     ; 3*v1+2*v2
        ADD      r3, r3, r6             ; 3*v1+2*v2+v3
        ADD      r3, r3, r7             ; 3*v1+2*v2+v3+v4
        ADD      r3, r3, r8             ; 3*v1+2*v2+v3+v4+v5
        ADD      r3, r3, #0x04          ; 3*v1+2*v2+v3+v4+v5+4
;
        MOV      lr, r3, LSR #3
;
        STRB     lr, [r1, -r2]          ; *pV2
;!
;
        ADD      r3, r3, r6             ; v3
        ADD      r3, r3, r9             ; v3+v6
        SUB      r3, r3, r4             ; v3+v6-v1
        SUB      r3, r3, r5             ; v3+v6-v1-v2
;
        MOV      lr, r3, LSR #3
        STRB     lr, [r1]               ; *pV3
;
        ADD      r3, r3, r7             ; v4
        ADD      r3, r3, r10            ; v4+v7
        SUB      r3, r3, r4             ; v4+v7-v1
        SUB      r3, r3, r6             ; v4+v7-v1-v3
;
        MOV      lr, r3, LSR #3
        STRB     lr, [r1, r2]           ; *pV4
;
        ADD      r3, r3, r8             ; v5
        ADD      r3, r3, r11            ; v5+v8
        SUB      r3, r3, r4             ; v5+v8-v1
        SUB      r3, r3, r7             ; v5+v8-v1-v4
;
; r4 is free at this point
;
        MOV      lr, r1
        MOV      r4, r3, LSR #3
        STRB     r4, [lr, r2, LSL #1]!  ; *pV5
;
        ADD      r3, r3, r9             ; v6
        ADD      r3, r3, r11            ; v6+v8
        SUB      r3, r3, r5             ; v6+v8-v2
        SUB      r3, r3, r8             ; v6+v8-v2-v5
;
        MOV      r4, r3, LSR #3
        STRB     r4, [lr, r2]!          ; *pV6
;
        ADD      r3, r3, r10            ; v7
        ADD      r3, r3, r11            ; v7+v8
        SUB      r3, r3, r6             ; v7+v8-v3
        SUB      r3, r3, r9             ; v7+v8-v3-v6
;
        MOV      r4, r3, LSR #3
        STRB     r4, [lr, r2]           ; *pV7
        B        |doneDeblock|
;
|beforeEasyDeblock|
;Reg Usage:
; r6 = pV3
; r2 = iPixelDistance
;
        LDRB     r7, [r1, r2]           ; *pV4
        LDRB     r8, [r1, r2, LSL #1]   ; *pV5
;
|easyDeblock|
;
;free registers: r3, r4, r5, r6, r9, r10, r11
;

  IF _XSC_=1
        PLD      [r1, #32]               ; Cheap preload gets 16 of 21 lines needed in the future at low cost
  ENDIF

        SUBS     r3, r8, r7              ; dx=v5-v4
        BEQ      |doneDeblock|           ; if (dx==0)
        LDR      r4, [sp, #0x34]         ; iStepSize
        RSBLT    r3, r3, #0              ; abs(v5-v4)
        CMP      r3, r4                  ; if (abs(dx)<iStepSize)
        BGE      |doneDeblock|           
;
;   -3 / 4 = 0  -5 / 4 = -1
;   -1 + 3 = 1111 + 11 = 11 >> 2 = 0
;   -3 + 3 = 1101 + 11 = 0 >> 2 = 0
;   -5 + 3 = 1011 + 11 = 1111 >> 2 = 1111 = -1
;
        SUBS     r5, r8, r7
        SUBLT    r7, r7, r3, LSR #2
        ADDLT    r8, r8, r3, LSR #2
        ADDGE    r7, r7, r3, LSR #2
        SUBGE    r8, r8, r3, LSR #2
;
        BICS     r4, r7, #0xFF
        MVNNE    r7, r4, ASR #31
        STRB     r7, [r1, r2]           ; *pV4
;
        BICS     r5, r8, #0xFF
        MVNNE    r8, r5, ASR #31
        STRB     r8, [r1, r2, LSL #1]    ; *pV5
;
|doneDeblock|
;
        LDR      r3, [sp, #0x04]        ; iPixelIncrement
        ADD      r0, r0, #0x01          ; i++
        ADD      r1, r1, r3             ; pV3 + iPixelIncrement
;
|forLoop|
        LDR     lr, [sp, #0x30]         ; EdgeLength
        CMP     r0, lr                  ; if (i<EdgeLength)
        BLT     |loopBegin|             ; restart loop again.
;
;otherwise the program ends here
;
        ADD      sp, sp, #0x0c
        LDMFD    sp!,{r4-r11,pc}
        WMV_ENTRY_END
        ENDP
;
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; FilterEdge Optimization
;
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        WMV_LEAF_ENTRY FilterEdge
;
        STMFD    sp!,{r4-r11,lr}
        FRAME_PROFILE_COUNT
;
;Reg Usage:
;  r0: pWMVDec
;  r1: ppxlcCenter
;  r2: iPixelDistance
;  r3: iPixelIncrement
;
;Stack Usage:
;  0x00: eq_cnt
;  0x04: iPixelDistance
;  0x08: i
;
        SUB      sp, sp, #0x0c
        SUB      r1, r1, r2, LSL #2 ; pV1
        STR      r3, [sp, #0x04]    ; Store iPixelIncrement; r3 is free
        MOV      r0, #0             ; r0 = i = 0
        B        |FilterEdge.forLoop|          
;
;The for loop Begins
;
|FilterEdge.loopBegin|
;
;Reg Usage: r4-r11 = v1-v8
;           r0: i, r1 = pV1, r2 = iPixelDistance
;           Available: r12, lr(r14), r3
;           Reserved: r13(sp), r15(pc)
;
;
        LDRB     r4,  [r1]                   ; pV1
        LDRB     r5,  [r1, r2]               ; pV2 = pV1 + iPixelDistance
        LDRB     r6,  [r1, r2, LSL #1]       ; pV3 = pV1 + 2*iPixelDistance
;
        ADD      lr,  r1, r2, LSL #2         ;       pV1 + 4 * iPixelDistance
        LDRB     r7,  [lr, -r2]              ; pV4 = pV5 - iPixelDistance
        LDRB     r8,  [lr]                   ; pV5 = pV1 + 4*iPixelDistance
        LDRB     r9,  [lr, r2]!              ; pV6 = pV5 + iPixelDistance
        LDRB     r10, [lr, r2]               ; pV7 = pV6 + iPixelDistance
        LDRB     r11, [lr, r2, LSL #1]       ; pV8 = pV6 + 2*iPixelDistance
;
; r0 is free at this use
;
        TST      r0, #3             ; if (i&3)==0
        BNE      |FilterEdge.skipCalcThreshold|
;
;Calc Theshold eq_cnt
;
        SUB      r12, r6, r5                 ; v3-v2
        ADD      r12, r12, #2                 ; v3-v2+2
        CMP      r12, #4                     ; 
        MOVHI    r12, #0                     ; v3-v2+2 <= 4
        MOVLS    r12, #1                     ; r12 = phi(v3-v2)
;
        SUB      lr, r7, r6                ; v4-v3
        ADD      lr, lr, #2               ; v4-v3+2
        CMP      lr, #4                    ; 
        ADDLS    r12, r12, #1
;
        SUB      lr,  r8, r7                ; v5-v4
        ADD      lr,  lr, #2                ; v5-v4+2
        CMP      lr,  #4                    ;
        ADDLS    r12, r12, #1
;
        SUB      lr, r9, r8                 ; v6-v5
        ADD      lr, lr, #2                 ; v6-v5+2
        CMP      lr, #4                     ; 
        ADDLS    r12, r12, #1
;
        SUB      lr, r10, r9                ; v7-v6
        ADD      lr, lr, #2                 ; v7-v6+2
        CMP      lr, #4                     ; 
        ADDLS    r12, r12, #1
;
        SUB      lr, r11, r10               ; v8-v7
        ADD      lr, lr, #2                 ; v8-v7 + 2
        CMP      lr, #4                     ; 
        ADDLS   r12, r12, #1
;
        CMP      r12, #3               ; if (eq_cnt<3)
        BLT      |FilterEdge.easyDeblock|
;
;otherwise, continue to calc eq_cnt
;
;Load v0, v9
;
        LDRB     r3,  [r1, -r2]             ; pV0 = pV1 - iPixelDistance
        SUB      lr, r5, r4                 ; v2-v1
        LDRB     r0,  [r1, r2, LSL #3]      ; pV9 = pV1 + 8*iPixelDistance
;
        ADD      lr, lr, #2                 ; v2-v1 + 2
        CMP      lr, #4                     ; 
        ADDLS    r12, r12, #1
;
        SUB      lr, r4, r3                 ; v1-v0
        ADD      lr, lr, #2                 ; v1-v0 + 2
        CMP      lr, #4                     ; 
        ADDLS    r12, r12, #1
;
        SUB      lr, r0, r11                ; v9-v8
        ADD      lr, lr, #2                 ; v2-v1 + 2
        CMP      lr, #4                     ; 
        ADDLS    r12, r12, #1
;
|FilterEdge.skipCalcThreshold|
        CMP      r12, #6
        BLT      |FilterEdge.easyDeblock|   ; if (eq_cnt < THR2)
;
;Reg Usage: r4-r11: v0-v8, r12 = eq_cnt
;Reg free: r0, r3, lr
;lr = max, r3=min
;
  IF _XSC_=1
        PLD      [r1, #32]                  ; Cheap preload gets 16 of 21 lines needed in the future
  ENDIF

        CMP      r5, r10              ; if (v2>=v7)
        MOVLT    lr, r10
        MOVLT    r3, r5
        MOVGE    lr, r5
        MOVGE    r3, r10
;
        CMP      r3, r7                ; if (min > v4)
        MOVGT    r3, r7
        BGT      |FilterEdge.bgt4to6|
        CMP      r7, lr                ; if (v4 > max)
        MOVGT    lr, r7
|FilterEdge.bgt4to6|
        CMP      r3, r9                ; if (min > v6)
        MOVGT    r3, r9
        BGT      |FilterEdge.bgt6to3|
        CMP      r9, lr                ; if (v6 > max)
        MOVGT    lr, r9
|FilterEdge.bgt6to3|
        CMP      r3, r6                ; if (min > v3)
        MOVGT    r3, r6
        BGT      |FilterEdge.bgt3to5|
        CMP      r6, lr                ; if (v3 > max)
        MOVGT    lr, r6
|FilterEdge.bgt3to5|
        CMP      r3, r8                ; if (min > v5)
        MOVGT    r3, r8
        BGT      |FilterEdge.bgt5to1|
        CMP      r8, lr                ; if (v5 > max)
        MOVGT    lr, r8
;
|FilterEdge.bgt5to1|
        CMP      r3, r4                ; if (min > v1)
        MOVGT    r3, r4
        BGT      |FilterEdge.bgt1to8|
        CMP      r4, lr                ; if (v1 > max)
        MOVGT    lr, r4
;
|FilterEdge.bgt1to8|
;
        CMP      r3, r11               ; if (min > v8)
        MOVGT    r3, r11
        BGT      |FilterEdge.bgt8|
        CMP      r11, lr                ; if (v8 > max)
        MOVGT    lr, r11
;
|FilterEdge.bgt8|
;
        SUB      r3, lr, r3            ; max - min
        LDR      lr, [sp,#0x34]        ; iStepSize
        CMP      r3,lr,LSL #1          ; if (max-min<2*iStepSize)
        BGE      |FilterEdge.doneDeblock|         ; Don't do anything if large than threshold
;
;otherwise Modify neighborhood points.
;
; r0: v9, r3: v0, 
; Load v0, v9
;
        LDRB     r3,  [r1, -r2]             ; pV0 = pV1 - iPixelDistance
        LDRB     r0,  [r1, r2, LSL #3]      ; pV9 = pV1 + 8*iPixelDistance
;
;Register Contention.
;Save eq_cnt, free r12
;
        STR      r12, [sp]                ; Save eq_cnt
;
        SUBS     r12, r4, r3              ; v1-v0
;       CMP      r12, #0
        RSBLT    r12, r12, #0             ; abs(v2-v1)
        CMP      r12, lr                  ; if (abs(v2-v1)>=iStepSize)
        MOVGE    r3, r4                   ; v0=v1
;
        SUBS      r12, r0, r11            ; v9-v8
;        CMP      r12, #0
        RSBLT    r12, r12, #0
        CMP      r12, lr
        MOVGE    r0, r11                  ; v9=v8
;
        ADD      r12, r3, r3, LSL #1      ; 3*v0
        ADD      r12, r7, r12, LSL #1     ; 6*v0+v4
        ADD      r12, r12, r4, LSL #2     ; +4*v1
        ADD      r12, r12, r5, LSL #1     ; +2*v2          
        ADD      r12, r12, r6, LSL #1     ; +2*v3          
        ADD      r12, r12, r8             ; +v5          
        ADD      r12, r12, #0x08          ; +8
;
;Does all results in ClipResult.
;
        MOV      lr, r12, LSR #4          ; >>4
        STRB     lr, [r1]                 ; *pV1
;
        ADD      r12, r12, r5, LSL #1     ; +2*v2
        ADD      r12, r12, r7             ; +v4
        ADD      r12, r12, r9             ; +v6
        SUB      r12, r12, r3, LSL #1     ; -2*v0
        SUB      r12, r12, r4, LSL #1     ; -2*v1
;
        MOV      lr, r12, LSR #4
        STRB     lr, [r1, r2]             ; *pV2
;
        ADD      r12, r12, r6, LSL #1     ; +2*v3
        ADD      r12, r12, r8             ; +v5
        ADD      r12, r12, r10            ; +v7
        SUB      r12, r12, r3, LSL #1     ; -2*v0
        SUB      r12, r12, r5, LSL #1     ; -2*v2
;
        MOV      lr, r12, LSR #4
        STRB     lr, [r1, r2, LSL #1]!    ; *pV3
;
        ADD      r12, r12, r7, LSL #1     ; +2*v4
        ADD      r12, r12, r9             ; +v6
        ADD      r12, r12, r11            ; +v8
        SUB      r12, r12, r3             ; -v0
        SUB      r12, r12, r4             ; -v1
        SUB      r12, r12, r6, LSL #1     ; -2*v3
;
        MOV      lr, r12, LSR #4
        STRB     lr, [r1, r2]             ; *pV4
;
        ADD      r12, r12, r8, LSL #1     ; +2*v5
        ADD      r12, r12, r10            ; +v7
        ADD      r12, r12, r0             ; +v9
        SUB      r12, r12, r3             ; -v0
;
; r3 is free at this point
;
        SUB      r12, r12, r5             ; -v2
        SUB      r12, r12, r7, LSL #1     ; -2*v4
;
        MOV      lr, r12, LSR #4
        STRB     lr, [r1, r2, LSL #1]!    ; *pV5
;
        ADD      r12, r12, r9, LSL #1     ; +2*v6
        ADD      r12, r12, r11            ; +v8
        ADD      r12, r12, r0             ; +v9
        SUB      r12, r12, r4             ; -v1
        SUB      r12, r12, r6             ; -v3
        SUB      r12, r12, r8, LSL #1     ; -2*v5
;
        MOV      lr, r12, LSR #4
        STRB     lr, [r1, r2]             ; *pV6
;
        ADD      r12, r12, r10, LSL #1    ; +2*v7
        ADD      r12, r12, r0, LSL #1     ; +2*v9
        SUB      r12, r12, r5             ; -v2
        SUB      r12, r12, r7             ; -v4
        SUB      r12, r12, r9, LSL #1     ; -2*v6
;
        MOV      lr, r12, LSR #4
        STRB     lr, [r1, r2, LSL #1]!    ; *pV7
;
        ADD      r12, r12, r11, LSL #1    ; +2*v8
        ADD      r12, r12, r0, LSL #1     ; +2*v9
        SUB      r12, r12, r6             ; -v3
        SUB      r12, r12, r8             ; -v5
        SUB      r12, r12, r10, LSL #1    ; -2*v7
;
        MOV      lr, r12, LSR #4
        STRB     lr, [r1, r2]             ; *pV8
;
;Load r12
;
        LDR      r12, [sp]                ; Load eq_cnt
        SUB      r1, r1, r2, LSL #2       ; Recover r1
        SUB      r1, r1, r2, LSL #1       ; 
;
        B        |FilterEdge.doneDeblock|
;
|FilterEdge.easyDeblock|
;
;free registers: r0, r3, lr
;
;
  IF _XSC_=1
        PLD      [r1, #32]                  ; Cheap preload gets 16 of 21 lines needed in the future
  ENDIF

        SUBS     r3, r7, r8              ; v4-v5
        BEQ      |FilterEdge.doneDeblock|
;
        SUB      lr, r6, r9              ; v3-v6
        RSB      lr, r3, lr, ASL #1      ; 2*(v3-v6) - (v4-v5)
        SUB      lr, lr, r3, ASL #2          ; 2*(v3-v6) - 5*(v4-v5)
        ADDS     lr, lr, #4              ; +4
        MOV      r3, lr                  ; a30
        RSBLT    lr, lr, #0             ; abs(a30)
        MOV      lr, lr, LSR #3         ; absa30
;
        LDR      r0, [sp, #0x34]         ; iStepSize
        CMP      lr, r0                  ; if (absa30 < iStepSize)
        BGE      |FilterEdge.doneDeblock|
;
        SUB      r5, r5, r6              ; v2-v3
        SUB      r9, r9, r10             ; v6-v7
;
; r6, r10 is free
;
        SUB      r4, r4, r7              ; v1-v4
        RSB      r4, r5, r4, ASL #1      ; 2*(v1-v4) - (v2-v3)
        SUB      r4, r4, r5, ASL #2      ; 2*(v1-v4) - 5*(v2-v3)
        ADDS     r4, r4, #4              ; a31
        RSBLT    r4, r4, #0              ; abs(a31)
;
        SUB      r11, r8, r11            ; v5-v8
        RSB      r11, r9, r11, ASL #1    ; 2*(v5-v8) - (v6-v7)
        SUB      r11, r11, r9, ASL #2    ; 2*(v5-v8) - 5*(v6-v7)
        ADDS     r11, r11, #4            ; a32
        RSBLT    r11, r11, #0            ; abs(a32)
;
;
        CMP      r4, r11                 ; cmp(abs(a31), abs(a32))
        MOVGT    r4, r11                  ; iMina31_a32
;
        SUBS     r6, lr, r4, LSR #3       ; idx = absa30 - iMina31_a32/8
        BLE      |FilterEdge.doneDeblock|
;
        SUBS     r10, r7, r8
        RSBLT    r10, r10, #0             ; abs(v4-v5)
        MOVS     r10, r10, LSR #1         ; abs(v4-v5) >> 1
        BEQ      |FilterEdge.doneDeblock|
;
        ADD      r4, r6, r6, LSL #2       ; 5*(absa30 - iMina31_a32)
        MOV      r4, r4, LSR #3           ; 5*(absa30 - iMina31_a32)/8
;
;
; clip abs(dA30) in [0, abs(v4_5/2)]
;
        CMP      r4, r10                  ; abs(dA30)  abs(v4_5/2)
        MOVGT    r4, r10                  ; min(abs(dA30), abs(v4_5/2))
;
;  CLIP dA30 in [0, v4_5/2]
;
;   a30  dA30    v4_5/2     RLT
;    -    +       +        CLIP
;    +    -       -        SIGN(v4_5) * CLIP
;    -    +       -        0
;    +    -       +        0
;
        SUBS     r5, r7, r8
        RSBLT    r4, r4, #0               ; SIGN(v4_v5) * min(dA30, abs(v4_v5/2))
        MOV      r5, r5, ASR #31
        CMP      r5, r3, ASR #31
        BEQ      |FilterEdge.doneDeblock|
;        
;
        ADD      r9, r1, r2, LSL #2
        SUB      r7, r7, r4             ; v4 - d
;
        BICS     r6, r7, #0xFF          ; Clip(v4-d) in [0, 255]
        MVNNE    r7, r6, ASR #31
        STRB     r7, [r9, -r2]          ; *pV4
;
        ADD      r8, r8, r4
        BICS     r5, r8, #0xFF
        MVNNE    r8, r5, ASR #31
        STRB     r8, [r9]               ; *pV5
;
;
;
|FilterEdge.doneDeblock|
;
        LDR      r0, [sp, #0x08]        ; Load i
        LDR      r3, [sp, #0x04]        ; iPixelIncrement
        ADD      r0, r0, #0x01          ; i++
        ADD      r1, r1, r3             ; pV1 + iPixelIncrement
;
|FilterEdge.forLoop|
;
;Save r0(i)
;
        LDR     lr, [sp, #0x30]         ; EdgeLength
        STR     r0, [sp, #0x08]        ; Save i, register contention later
        CMP     r0, lr                  ; if (i<EdgeLength)
        BLT     |FilterEdge.loopBegin|             ; restart loop again.
;
;otherwise the program ends here
;
        ADD      sp, sp, #0x0c
        LDMFD    sp!,{r4-r11,pc}
        WMV_ENTRY_END
        ENDP
;

    EXPORT end_postfilter_arm
end_postfilter_arm
        nop

  ENDIF ; WMV_OPT_PPL_ARM

  END