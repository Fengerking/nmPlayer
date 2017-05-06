@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@
@  THIS IS ASSEMBLY VERSION OF ROUTINES IN POSTFILTER_WMV.CPP WHEN 
@  SUPPORT_POSTFILTERS and OPT_POSTFILTER_DEBLOCK ARE DEFINED
@
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    #include "../c/voWMVDecID.h"
    .include "wmvdec_member_arm.h"
    .include "xplatform_arm_asm.h" 

    @AREA DEBLOCK, CODE, READONLY
		 .text
		 .align 4

    .if WMV_OPT_PPL_ARM == 1
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

 

    .globl  _FilterEdge_Short
    .globl  _FilterEdge

@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        @AREA .FilterEdge_Short, CODE, READONLY

        WMV_LEAF_ENTRY FilterEdge_Short
@
        stmfd    sp!,{r4-r11,lr}
        FRAME_PROFILE_COUNT
@
@Reg Usage:
@  r0: pWMVDec
@  r1: ppxlcCenter
@  r2: iPixelDistance
@  r3: iPixelIncrement
@
@Stack Usage:
@  0x00: pV4
@  0x04: iPixelDistance
@  0x08: i
@
        sub      sp, sp, #0x0c
        sub      r1, r1, r2, lsl #1
        str      r3, [sp, #0x04]    @ Store iPixelIncrement@ r3 is free
        mov      r0, #0             @ r0 == i == 0
        b        forLoop          
@
@The for loop Begins
@
loopBegin:
@
@Reg Usage: r4-r11 == v1-v8
@           r1 == pWMVDec->m_rgiClapTabDec, r2 == iPixelDistance
@           Available: r12, lr(r14), r3, r0
@           Reserved: r13(sp), r15(pc)
@
@Reg Status: r6 == pV3
@
        tst      r0, #3             @ if (i&3)====0
        bne      skipCalcThreshold
@
@Calc Theshold eq_cnt
@
        ldrb     r4,  [r1, -r2, ASL #1]      @ pV1 == pV3 - 2 * iPixelDistance
        add      lr,  r1, r2, lsl #2         @ pV7 == pV3 + 4 * iPixelDistance
        ldrb     r6,  [r1]                   @ pV3
        ldrb     r7,  [r1, r2]               @ pV4 == pV3 + iPixelDistance
        ldrb     r5,  [r1, -r2]              @ pV2 == pV3 - iPixelDistance
@
        sub      r12, r7, r6                 @ v4-v3
        ldrb     r8,  [lr, -r2, ASL #1]      @ pV5 == pV7 - 2*iPixelDistance
        add      r12, r12, #2                @ v4-v3+2
        ldrb     r9,  [lr, -r2]              @ pV6 == pV7 - iPixelDistance
        cmp      r12, #4              @ 
        ldrb     r11, [lr, r2]               @ pV8 == pV7 + iPixelDistance
        movhi    r12, #0              @ v4-v3+2 <== 4
        ldrb     r10, [lr]                   @ pV7
        movls    r12, #1              @ r12 == phi(v4-v3)
@
        sub      lr,  r8, r7          @ v5-v4
        add      lr,  lr, #2          @ v5-v4+2
        cmp      lr,  #4              @
        addls    r12, r12, #1
@
        sub      lr, r9, r8           @ v6-v5
        add      lr, lr, #2           @ v6-v5+2
        cmp      lr, #4               @ 
        addls    r12, r12, #1
@
        sub      lr, r10, r9          @ v7-v6
        add      lr, lr, #2           @ v7-v6+2
        cmp      lr, #4               @ 
        addls    r12, r12, #1
@
        cmp      r12,#2               @ if (eq_cnt<2)
        blt      easyDeblock
@
@otherwise, continue to calc eq_cnt
@
        sub      lr, r6, r5           @ v3-v2
        add      lr, lr, #2           @ v3-v2+2
        cmp      lr, #4               @ 
        addls    r12, r12, #1
@
        sub      lr, r5, r4           @ v2-v1
        add      lr, lr, #2           @ v2-v1 + 2
        cmp      lr, #4               @ 
        addls    r12, r12, #1
@
        sub      lr, r11, r10         @ v8-v7
        add      lr, lr, #2           @ v8-v7 + 2
        cmp      lr, #4               @ 
        addls    r12, r12, #1
        cmp      r12, #5
        blt      easyDeblock        @ if (eq_cnt < THR2-1)
        b        fullDeblock
@        
skipCalcThreshold:
        cmp      r12, #5
        blt      beforeEasyDeblock        @ if (eq_cnt < THR2-1)
@
@Load the rest registers
        ldrb     r4,  [r1, -r2, ASL #1]      @ pV1 == pV3 - 2 * iPixelDistance
        add      lr,  r1, r2, lsl #2         @ pV7 == pV3 + 4 * iPixelDistance
        ldrb     r5,  [r1, -r2]              @ pV2 == pV3 - iPixelDistance
        ldrb     r7,  [r1, r2]               @ pV4 == pV3 + iPixelDistance
        ldrb     r6,  [r1]                   @ pV3
@
        ldrb     r8,  [lr, -r2, ASL #1]      @ pV5 == pV7 - 2*iPixelDistance
        ldrb     r9,  [lr, -r2]              @ pV6 == pV7 - iPixelDistance
        ldrb     r11, [lr, r2]               @ pV8 == pV7 + iPixelDistance
        ldrb     r10, [lr]                   @ pV7
@
fullDeblock:
@
@otherwise execute full deblock
@
@lr == max, r3==min
@
  .if PLD_ENABLE==1
        pld      [r1, #32]            @ Cheap preload gets 16 of 21 lines needed in the future at low cost
  .endif

        cmp      r5, r10              @ if (v2>==v7)
        movlt    lr, r10
        movlt    r3, r5
        movge    lr, r5
        movge    r3, r10
@
        cmp      r3, r7                @ if (min > v4)
        movgt    r3, r7
        bgt      bgt4to6
        cmp      r7, lr                @ if (v4 > max)
        movgt    lr, r7
bgt4to6:
        cmp      r3, r9                @ if (min > v6)
        movgt    r3, r9
        bgt      bgt6to3
        cmp      r9, lr                @ if (v6 > max)
        movgt    lr, r9
bgt6to3:
        cmp      r3, r6                @ if (min > v3)
        movgt    r3, r6
        bgt      bgt3to5
        cmp      r6, lr                @ if (v3 > max)
        movgt    lr, r6
bgt3to5:
        cmp      r3, r8                @ if (min > v5)
        movgt    r3, r8
        bgt      bgt5
        cmp      r8, lr                @ if (v5 > max)
        movgt    lr, r8
bgt5:
        sub      r3, lr, r3            @ max - min
        ldr      lr, [sp,#0x34]        @ iStepSize
        cmp      r3,lr,lsl #1          @ if (max-min<2*iStepSize)
        bge      doneDeblock         @ Don't do anything if large than threshold
@
@otherwise Modify neighborhood points.
@
@
        subs     r3, r5, r4             @ v2-v1
        rsblt    r3, r3, #0             @ abs(v2-v1)
        cmp      r3, lr                 @ if (abs(v2-v1)>==iStepSize)
        movge    r4, r5                 @ v1==v2
@
        subs     r3, r11, r10           @ v8-v7
        rsblt    r3, r3, #0
        cmp      r3, lr
        movge    r11, r10               @ v8==v7
@
@Register Contention: Save r0.
@
@        str      r0, [sp, #0x08]        @ Save i, register contention later
@
@r0, r2, lr is free
@
        add      r3, r4, r4, lsl #1     @ 3*v1
        add      r3, r3, r5, lsl #1     @ 3*v1+2*v2
        add      r3, r3, r6             @ 3*v1+2*v2+v3
        add      r3, r3, r7             @ 3*v1+2*v2+v3+v4
        add      r3, r3, r8             @ 3*v1+2*v2+v3+v4+v5
        add      r3, r3, #0x04          @ 3*v1+2*v2+v3+v4+v5+4
@
        mov      lr, r3, lsr #3
@
  
        strb     lr, [r1, -r2]          @ *pV2
@!
@
        add      r3, r3, r6             @ v3
        add      r3, r3, r9             @ v3+v6
        sub      r3, r3, r4             @ v3+v6-v1
        sub      r3, r3, r5             @ v3+v6-v1-v2
@
        mov      lr, r3, lsr #3
        strb     lr, [r1]               @ *pV3
@
        add      r3, r3, r7             @ v4
        add      r3, r3, r10            @ v4+v7
        sub      r3, r3, r4             @ v4+v7-v1
        sub      r3, r3, r6             @ v4+v7-v1-v3
@
        mov      lr, r3, lsr #3
        strb     lr, [r1, r2]           @ *pV4
@
        add      r3, r3, r8             @ v5
        add      r3, r3, r11            @ v5+v8
        sub      r3, r3, r4             @ v5+v8-v1
        sub      r3, r3, r7             @ v5+v8-v1-v4
@
@ r4 is free at this point
@
        mov      lr, r1
        mov      r4, r3, lsr #3
        strb     r4, [lr, r2, lsl #1]!  @ *pV5
@
        add      r3, r3, r9             @ v6
        add      r3, r3, r11            @ v6+v8
        sub      r3, r3, r5             @ v6+v8-v2
        sub      r3, r3, r8             @ v6+v8-v2-v5
@
        mov      r4, r3, lsr #3
        strb     r4, [lr, r2]!          @ *pV6
@
        add      r3, r3, r10            @ v7
        add      r3, r3, r11            @ v7+v8
        sub      r3, r3, r6             @ v7+v8-v3
        sub      r3, r3, r9             @ v7+v8-v3-v6
@
        mov      r4, r3, lsr #3
        strb     r4, [lr, r2]           @ *pV7
        b        doneDeblock
		
@
beforeEasyDeblock:
@Reg Usage:
@ r6 == pV3
@ r2 == iPixelDistance
@
        ldrb     r7, [r1, r2]           @ *pV4
        ldrb     r8, [r1, r2, lsl #1]   @ *pV5
@
easyDeblock:
@
@free registers: r3, r4, r5, r6, r9, r10, r11
@

  .if PLD_ENABLE==1
        pld      [r1, #32]               @ Cheap preload gets 16 of 21 lines needed in the future at low cost
  .endif

        subs     r3, r8, r7              @ dx==v5-v4
        beq      doneDeblock           @ if (dx====0)
        ldr      r4, [sp, #0x34]         @ iStepSize
        rsblt    r3, r3, #0              @ abs(v5-v4)
        cmp      r3, r4                  @ if (abs(dx)<iStepSize)
        bge      doneDeblock           
@
@   -3 / 4 == 0  -5 / 4 == -1
@   -1 + 3 == 1111 + 11 == 11 >> 2 == 0
@   -3 + 3 == 1101 + 11 == 0 >> 2 == 0
@   -5 + 3 == 1011 + 11 == 1111 >> 2 == 1111 == -1
@
        subs     r5, r8, r7
        sublt    r7, r7, r3, lsr #2
        addlt    r8, r8, r3, lsr #2
        addge    r7, r7, r3, lsr #2
        subge    r8, r8, r3, lsr #2
@
        sub     r4, r7, #0xFF
        mvnne    r7, r4, asr #31
        strb     r7, [r1, r2]           @ *pV4
@
        sub     r5, r8, #0xFF
        mvnne    r8, r5, asr #31
        strb     r8, [r1, r2, lsl #1]    @ *pV5
		
@
doneDeblock:
@
        ldr      r3, [sp, #0x04]        @ iPixelIncrement
        add      r0, r0, #0x01          @ i++
        add      r1, r1, r3             @ pV3 + iPixelIncrement
@
forLoop:
        ldr     lr, [sp, #0x30]         @ EdgeLength
        cmp     r0, lr                  @ if (i<EdgeLength)
        blt     loopBegin             @ restart loop again.
@
@otherwise the program ends here
@
        add      sp, sp, #0x0c
        ldmfd    sp!,{r4-r11,pc}
        WMV_ENTRY_END
@
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@ FilterEdge Optimization
@
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        WMV_LEAF_ENTRY FilterEdge
@
        stmfd    sp!,{r4-r11,lr}
        FRAME_PROFILE_COUNT
@
@Reg Usage:
@  r0: pWMVDec
@  r1: ppxlcCenter
@  r2: iPixelDistance
@  r3: iPixelIncrement
@
@Stack Usage:
@  0x00: eq_cnt
@  0x04: iPixelDistance
@  0x08: i
@
        sub      sp, sp, #0x0c
        sub      r1, r1, r2, lsl #2 @ pV1
        str      r3, [sp, #0x04]    @ Store iPixelIncrement@ r3 is free
        mov      r0, #0             @ r0 == i == 0
        b        FilterEdge.forLoop          
@
@The for loop Begins
@
FilterEdge.loopBegin:
@
@Reg Usage: r4-r11 == v1-v8
@           r0: i, r1 == pV1, r2 == iPixelDistance
@           Available: r12, lr(r14), r3
@           Reserved: r13(sp), r15(pc)
@
@
        ldrb     r4,  [r1]                   @ pV1
        ldrb     r5,  [r1, r2]               @ pV2 == pV1 + iPixelDistance
        ldrb     r6,  [r1, r2, lsl #1]       @ pV3 == pV1 + 2*iPixelDistance
@
        add      lr,  r1, r2, lsl #2         @       pV1 + 4 * iPixelDistance
        ldrb     r7,  [lr, -r2]              @ pV4 == pV5 - iPixelDistance
        ldrb     r8,  [lr]                   @ pV5 == pV1 + 4*iPixelDistance
        ldrb     r9,  [lr, r2]!              @ pV6 == pV5 + iPixelDistance
        ldrb     r10, [lr, r2]               @ pV7 == pV6 + iPixelDistance
        ldrb     r11, [lr, r2, lsl #1]       @ pV8 == pV6 + 2*iPixelDistance
@
@ r0 is free at this use
@
        tst      r0, #3             @ if (i&3)====0
        bne      FilterEdge.skipCalcThreshold
@
@Calc Theshold eq_cnt
@
        sub      r12, r6, r5                 @ v3-v2
        add      r12, r12, #2                 @ v3-v2+2
        cmp      r12, #4                     @ 
        movhi    r12, #0                     @ v3-v2+2 <== 4
        movls    r12, #1                     @ r12 == phi(v3-v2)
@
        sub      lr, r7, r6                @ v4-v3
        add      lr, lr, #2               @ v4-v3+2
        cmp      lr, #4                    @ 
        addls    r12, r12, #1
@
        sub      lr,  r8, r7                @ v5-v4
        add      lr,  lr, #2                @ v5-v4+2
        cmp      lr,  #4                    @
        addls    r12, r12, #1
@
        sub      lr, r9, r8                 @ v6-v5
        add      lr, lr, #2                 @ v6-v5+2
        cmp      lr, #4                     @ 
        addls    r12, r12, #1
@
        sub      lr, r10, r9                @ v7-v6
        add      lr, lr, #2                 @ v7-v6+2
        cmp      lr, #4                     @ 
        addls    r12, r12, #1
@
        sub      lr, r11, r10               @ v8-v7
        add      lr, lr, #2                 @ v8-v7 + 2
        cmp      lr, #4                     @ 
        addls   r12, r12, #1
@
        cmp      r12, #3               @ if (eq_cnt<3)
        blt      FilterEdge.easyDeblock
@
@otherwise, continue to calc eq_cnt
@
@Load v0, v9
@
        ldrb     r3,  [r1, -r2]             @ pV0 == pV1 - iPixelDistance
        sub      lr, r5, r4                 @ v2-v1
        ldrb     r0,  [r1, r2, lsl #3]      @ pV9 == pV1 + 8*iPixelDistance
@
        add      lr, lr, #2                 @ v2-v1 + 2
        cmp      lr, #4                     @ 
        addls    r12, r12, #1
@
        sub      lr, r4, r3                 @ v1-v0
        add      lr, lr, #2                 @ v1-v0 + 2
        cmp      lr, #4                     @ 
        addls    r12, r12, #1
@
        sub      lr, r0, r11                @ v9-v8
        add      lr, lr, #2                 @ v2-v1 + 2
        cmp      lr, #4                     @ 
        addls    r12, r12, #1
@
FilterEdge.skipCalcThreshold:
        cmp      r12, #6
        blt      FilterEdge.easyDeblock   @ if (eq_cnt < THR2)
@
@Reg Usage: r4-r11: v0-v8, r12 == eq_cnt
@Reg free: r0, r3, lr
@lr == max, r3==min
@
  .if PLD_ENABLE==1
        pld      [r1, #32]                  @ Cheap preload gets 16 of 21 lines needed in the future
  .endif

        cmp      r5, r10              @ if (v2>==v7)
        movlt    lr, r10
        movlt    r3, r5
        movge    lr, r5
        movge    r3, r10
@
        cmp      r3, r7                @ if (min > v4)
        movgt    r3, r7
        bgt      FilterEdge.bgt4to6
        cmp      r7, lr                @ if (v4 > max)
        movgt    lr, r7
FilterEdge.bgt4to6:
        cmp      r3, r9                @ if (min > v6)
        movgt    r3, r9
        bgt      FilterEdge.bgt6to3
        cmp      r9, lr                @ if (v6 > max)
        movgt    lr, r9
FilterEdge.bgt6to3:
        cmp      r3, r6                @ if (min > v3)
        movgt    r3, r6
        bgt      FilterEdge.bgt3to5
        cmp      r6, lr                @ if (v3 > max)
        movgt    lr, r6
FilterEdge.bgt3to5:
        cmp      r3, r8                @ if (min > v5)
        movgt    r3, r8
        bgt      FilterEdge.bgt5to1
        cmp      r8, lr                @ if (v5 > max)
        movgt    lr, r8
@
FilterEdge.bgt5to1:
        cmp      r3, r4                @ if (min > v1)
        movgt    r3, r4
        bgt      FilterEdge.bgt1to8
        cmp      r4, lr                @ if (v1 > max)
        movgt    lr, r4
@
FilterEdge.bgt1to8:
@
        cmp      r3, r11               @ if (min > v8)
        movgt    r3, r11
        bgt      FilterEdge.bgt8
        cmp      r11, lr                @ if (v8 > max)
        movgt    lr, r11
@
FilterEdge.bgt8:
@
        sub      r3, lr, r3            @ max - min
        ldr      lr, [sp,#0x34]        @ iStepSize
        cmp      r3,lr,lsl #1          @ if (max-min<2*iStepSize)
        bge      FilterEdge.doneDeblock         @ Don't do anything if large than threshold
@
@otherwise Modify neighborhood points.
@
@ r0: v9, r3: v0, 
@ Load v0, v9
@
        ldrb     r3,  [r1, -r2]             @ pV0 == pV1 - iPixelDistance
        ldrb     r0,  [r1, r2, lsl #3]      @ pV9 == pV1 + 8*iPixelDistance
@
@Register Contention.
@Save eq_cnt, free r12
@
        str      r12, [sp]                @ Save eq_cnt
@
        subs     r12, r4, r3              @ v1-v0
@       cmp      r12, #0
        rsblt    r12, r12, #0             @ abs(v2-v1)
        cmp      r12, lr                  @ if (abs(v2-v1)>==iStepSize)
        movge    r3, r4                   @ v0==v1
@
        subs      r12, r0, r11            @ v9-v8
@        cmp      r12, #0
        rsblt    r12, r12, #0
        cmp      r12, lr
        movge    r0, r11                  @ v9==v8
@
        add      r12, r3, r3, lsl #1      @ 3*v0
        add      r12, r7, r12, lsl #1     @ 6*v0+v4
        add      r12, r12, r4, lsl #2     @ +4*v1
        add      r12, r12, r5, lsl #1     @ +2*v2          
        add      r12, r12, r6, lsl #1     @ +2*v3          
        add      r12, r12, r8             @ +v5          
        add      r12, r12, #0x08          @ +8
@
@Does all results in ClipResult.
@
        mov      lr, r12, lsr #4          @ >>4
        strb     lr, [r1]                 @ *pV1
@
        add      r12, r12, r5, lsl #1     @ +2*v2
        add      r12, r12, r7             @ +v4
        add      r12, r12, r9             @ +v6
        sub      r12, r12, r3, lsl #1     @ -2*v0
        sub      r12, r12, r4, lsl #1     @ -2*v1
@
        mov      lr, r12, lsr #4
        strb     lr, [r1, r2]             @ *pV2
@
        add      r12, r12, r6, lsl #1     @ +2*v3
        add      r12, r12, r8             @ +v5
        add      r12, r12, r10            @ +v7
        sub      r12, r12, r3, lsl #1     @ -2*v0
        sub      r12, r12, r5, lsl #1     @ -2*v2
@
        mov      lr, r12, lsr #4
        strb     lr, [r1, r2, lsl #1]!    @ *pV3
@
        add      r12, r12, r7, lsl #1     @ +2*v4
        add      r12, r12, r9             @ +v6
        add      r12, r12, r11            @ +v8
        sub      r12, r12, r3             @ -v0
        sub      r12, r12, r4             @ -v1
        sub      r12, r12, r6, lsl #1     @ -2*v3
@
        mov      lr, r12, lsr #4
        strb     lr, [r1, r2]             @ *pV4
@
        add      r12, r12, r8, lsl #1     @ +2*v5
        add      r12, r12, r10            @ +v7
        add      r12, r12, r0             @ +v9
        sub      r12, r12, r3             @ -v0
@
@ r3 is free at this point
@
        sub      r12, r12, r5             @ -v2
        sub      r12, r12, r7, lsl #1     @ -2*v4
@
        mov      lr, r12, lsr #4
        strb     lr, [r1, r2, lsl #1]!    @ *pV5
@
        add      r12, r12, r9, lsl #1     @ +2*v6
        add      r12, r12, r11            @ +v8
        add      r12, r12, r0             @ +v9
        sub      r12, r12, r4             @ -v1
        sub      r12, r12, r6             @ -v3
        sub      r12, r12, r8, lsl #1     @ -2*v5
@
        mov      lr, r12, lsr #4
        strb     lr, [r1, r2]             @ *pV6
@
        add      r12, r12, r10, lsl #1    @ +2*v7
        add      r12, r12, r0, lsl #1     @ +2*v9
        sub      r12, r12, r5             @ -v2
        sub      r12, r12, r7             @ -v4
        sub      r12, r12, r9, lsl #1     @ -2*v6
@
        mov      lr, r12, lsr #4
        strb     lr, [r1, r2, lsl #1]!    @ *pV7
@
        add      r12, r12, r11, lsl #1    @ +2*v8
        add      r12, r12, r0, lsl #1     @ +2*v9
        sub      r12, r12, r6             @ -v3
        sub      r12, r12, r8             @ -v5
        sub      r12, r12, r10, lsl #1    @ -2*v7
@
        mov      lr, r12, lsr #4
        strb     lr, [r1, r2]             @ *pV8
		
@
@Load r12
@
        ldr      r12, [sp]                @ Load eq_cnt
        sub      r1, r1, r2, lsl #2       @ Recover r1
        sub      r1, r1, r2, lsl #1       @ 
@
        b        FilterEdge.doneDeblock
@
FilterEdge.easyDeblock:
@
@free registers: r0, r3, lr
@
@
  .if PLD_ENABLE==1
        pld      [r1, #32]                  @ Cheap preload gets 16 of 21 lines needed in the future
  .endif

        subs     r3, r7, r8              @ v4-v5
        beq      FilterEdge.doneDeblock
@
        sub      lr, r6, r9              @ v3-v6
        rsb      lr, r3, lr, ASL #1      @ 2*(v3-v6) - (v4-v5)
        sub      lr, lr, r3, ASL #2          @ 2*(v3-v6) - 5*(v4-v5)
        adds     lr, lr, #4              @ +4
        mov      r3, lr                  @ a30
        rsblt    lr, lr, #0             @ abs(a30)
        mov      lr, lr, lsr #3         @ absa30
@
        ldr      r0, [sp, #0x34]         @ iStepSize
        cmp      lr, r0                  @ if (absa30 < iStepSize)
        bge      FilterEdge.doneDeblock
@
        sub      r5, r5, r6              @ v2-v3
        sub      r9, r9, r10             @ v6-v7
@
@ r6, r10 is free
@
        sub      r4, r4, r7              @ v1-v4
        rsb      r4, r5, r4, ASL #1      @ 2*(v1-v4) - (v2-v3)
        sub      r4, r4, r5, ASL #2      @ 2*(v1-v4) - 5*(v2-v3)
        adds     r4, r4, #4              @ a31
        rsblt    r4, r4, #0              @ abs(a31)
@
        sub      r11, r8, r11            @ v5-v8
        rsb      r11, r9, r11, ASL #1    @ 2*(v5-v8) - (v6-v7)
        sub      r11, r11, r9, ASL #2    @ 2*(v5-v8) - 5*(v6-v7)
        adds     r11, r11, #4            @ a32
        rsblt    r11, r11, #0            @ abs(a32)
@
@
        cmp      r4, r11                 @ cmp(abs(a31), abs(a32))
        movgt    r4, r11                  @ iMina31_a32
@
        subs     r6, lr, r4, lsr #3       @ idx == absa30 - iMina31_a32/8
        ble      FilterEdge.doneDeblock
@
        subs     r10, r7, r8
        rsblt    r10, r10, #0             @ abs(v4-v5)
        movs     r10, r10, lsr #1         @ abs(v4-v5) >> 1
        beq      FilterEdge.doneDeblock
@
        add      r4, r6, r6, lsl #2       @ 5*(absa30 - iMina31_a32)
        mov      r4, r4, lsr #3           @ 5*(absa30 - iMina31_a32)/8
@
@
@ clip abs(dA30) in [0, abs(v4_5/2)]
@
        cmp      r4, r10                  @ abs(dA30)  abs(v4_5/2)
        movgt    r4, r10                  @ min(abs(dA30), abs(v4_5/2))
@
@  CLIP dA30 in [0, v4_5/2]
@
@   a30  dA30    v4_5/2     RLT
@    -    +       +        CLIP
@    +    -       -        SIGN(v4_5) * CLIP
@    -    +       -        0
@    +    -       +        0
@
        subs     r5, r7, r8
        rsblt    r4, r4, #0               @ SIGN(v4_v5) * min(dA30, abs(v4_v5/2))
        mov      r5, r5, asr #31
        cmp      r5, r3, asr #31
        beq      FilterEdge.doneDeblock
@        
@
        add      r9, r1, r2, lsl #2
        sub      r7, r7, r4             @ v4 - d
@
        sub     r6, r7, #0xFF          @ Clip(v4-d) in [0, 255]
        mvnne    r7, r6, asr #31
        strb     r7, [r9, -r2]          @ *pV4
@
        add      r8, r8, r4
        sub     r5, r8, #0xFF
        mvnne    r8, r5, asr #31
        strb     r8, [r9]               @ *pV5	
@
@
@
FilterEdge.doneDeblock:
@
        ldr      r0, [sp, #0x08]        @ Load i
        ldr      r3, [sp, #0x04]        @ iPixelIncrement
        add      r0, r0, #0x01          @ i++
        add      r1, r1, r3             @ pV1 + iPixelIncrement
@
FilterEdge.forLoop:
@
@Save r0(i)
@
        ldr     lr, [sp, #0x30]         @ EdgeLength
        str     r0, [sp, #0x08]        @ Save i, register contention later
        cmp     r0, lr                  @ if (i<EdgeLength)
        blt     FilterEdge.loopBegin             @ restart loop again.
@
@otherwise the program ends here
@
        add      sp, sp, #0x0c
        ldmfd    sp!,{r4-r11,pc}
        WMV_ENTRY_END
@

  .endif @ WMV_OPT_PPL_ARM

  @@.end
