@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************

    @TTL spatialpredictor.cpp
    #include "../c/voWMVDecID.h"
    .include "wmvdec_member_arm.inc"
    .include "xplatform_arm_asm.h" 


    .if WMV_OPT_X8_ARM == 1

    @@AREA    .bss, NOINIT
		.section .bss

    @ extern "C" Void_WMV FilterHorzEdgeX8 (PixelC* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iStepSize)@
    @ extern "C" Void_WMV FilterVertEdgeX8 (PixelC* ppxlcCenter, I32_WMV iPixelIncrement, I32_WMV iStepSize)@
    .global FilterHorzEdgeX8
    .global FilterVertEdgeX8
    
 @   .if DEBUG==1
@ we only need the following when we are debugging
@    IMPORT  WMVAsmBreak
  @  .endif


@*****************************************************************************************************
@ FilterHorzEdgeX8
@*****************************************************************************************************

    @AREA    .text, CODE
		.section .text

@ extern "C" Void_WMV FilterHorzEdgeX8 (PixelC* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iStepSize)@
    WMV_LEAF_ENTRY FilterHorzEdgeX8 @FilterHorzEdgeX8

@ r0 == ppclcCenter
@ r1 == iPixelDistance
@ r2 == iStepSize

.equ oiThr1          , 0x18
.equ oiStepSize      , 0x1C
.equ ov1             , 0x20
.equ oi              , 0x24
.equ oiPixelInc      , 0x28

.equ StackTempSizeH  , 0x30


r4pCntr .req  r4
r5v2    .req  r5
r6v7    .req  r6
r7v3    .req  r7
r8v6    .req  r8
r9PD    .req  r9
r10v4   .req  r10
r11v5   .req  r11
r12v8   .req  r12

@ 1349 : {

    mov       r12, sp
    stmdb     sp!, {r1, r2}  @ stmfd
    stmdb     sp!, {r4 - r12, lr}  @ stmfd
    FRAME_PROFILE_COUNT
    sub       sp, sp, #StackTempSizeH  @ 0x44 == 68

M25444:
    str       r2, [sp, #oiStepSize]  
    mov       r3, #1
    str       r3, [sp, #oiPixelInc]

FilterEitherEdgeX8:   @ ************* target from FilterVertEdgeX8 ****************
    @ share code with vertical filter by setting up oiPixelInc and r2==iPixelDistance


@ 1350 :     const Int iThr1 == ((iStepSize + 10) >> 3)@ // + 1@@
    add       r3, r2, #0xA              @ iStepSize + 10
    mov       r4pCntr, r0
    mov       r9PD, r1                  @ iPixelDistance
    mov       lr, r3, asr #3            @ ((iStepSize + 10) >> 3)
    str       lr, [sp, #oiThr1]         @ iThr1 == ((iStepSize + 10) >> 3)
    mov       r3, #8
    str       r3, [sp, #oi]
    b         HorzX8iLoopTop1  @ 00000C00

HorzX8iLoopTop:
    ldr       lr, [sp, #oiThr1]

HorzX8iLoopTop1:
@ 1351 :     
@ 1352 :     for (Int i == 0@ i < 8@ ++i) {
@ 1353 :         Int  v1, v2, v3, v4, v5, v6, v7, v8@
@ 1354 :         PixelC* pVtmpR == ppxlcCenter@
@ 1355 :         PixelC* pVtmpL == ppxlcCenter - iPixelDistance@
@ 1356 : 
@ 1357 :         v4 == *pVtmpL@
@ 1358 :         v5 == *pVtmpR@
@ 1359 :         pVtmpL -== iPixelDistance@
@ 1360 :         pVtmpR +== iPixelDistance@
@ 1361 :         v3 == *pVtmpL@
@ 1362 :         v6 == *pVtmpR@
@ 1363 :         pVtmpL -== iPixelDistance@
@ 1364 :         pVtmpR +== iPixelDistance@
@ 1365 :         v2 == *pVtmpL@
@ 1366 :         v7 == *pVtmpR@
@ 1367 :         pVtmpL -== iPixelDistance@
@ 1368 :         pVtmpR +== iPixelDistance@
@ 1369 :         v1 == *pVtmpL@
@ 1370 :         v8 == *pVtmpR@

    add       r3,   r9PD,  r9PD, lsl #1         @ 3 * iPixelDistance
    ldrb      r0,   [r4pCntr, -r9PD, LSL #2]    @ v1 == [ppxlcCenter - 4 * iPixelDistance]
    ldrb      r5v2, [r4pCntr, -r3]              @ v2 == [ppxlcCenter - 3 * iPixelDistance]
    ldrb      r12v8,[r4pCntr, +r3]              @ v8 == [ppxlcCenter + 3 * iPixelDistance]
    str       r0,   [sp, #ov1]                  @ v1

@ 1371 : 
@ 1372 :         Int eq_cnt == phi1(v1 - v2) + phi1(v2 - v3) + phi1(v3 - v4) + phi1(v4 - v5)@        
    ldrb      r7v3, [r4pCntr, -r9PD, LSL #1]    @@@@ v3 == [ppxlcCenter - 2 * iPixelDistance]
    mov       r3, #0                            @ init count

    sub       r2, r0, r5v2                      @ v1 - v2
    add       r1, r2, lr                        @ (v1 - v2) + iThr1
    cmp       r1, lr, lsl #1                    @ (unsigned(v1 - v2 + iThr1) <== uThr2) 
    addls     r3, r3, #1                        @ count if in range

    ldrb      r10v4,[r4pCntr, -r9PD]            @@@@ v4 == [ppxlcCenter - iPixelDistance]
    sub       r2, r5v2, r7v3                    @ v2 - v3
    add       r1, r2, lr                        @ (v2 - v3) + iThr1
    cmp       r1, lr, lsl #1                    @ (unsigned(v2 - v3 + iThr1) <== uThr2) 
    addls     r3, r3, #1                        @ count if in range

    ldrb      r11v5,[r4pCntr]                   @@@@ v5 == [ppxlcCenter]
    sub       r2, r7v3, r10v4                   @ v3 - v4
    add       r1, r2, lr                        @ (v3 - v4) + iThr1
    cmp       r1, lr, lsl #1                    @ (unsigned(v3 - v4 + iThr1) <== uThr2) 
    addls     r3, r3, #1                        @ count if in range

    ldrb      r6v7, [r4pCntr, +r9PD, LSL #1]    @@@@ v7 == [ppxlcCenter + 2 * iPixelDistance]
    sub       r2, r10v4, r11v5                  @ v4 - v5
    add       r1, r2, lr                        @ (v4 - v5) + iThr1
    cmp       r1, lr, lsl #1                    @ (unsigned(v4 - v5 + iThr1) <== uThr2) 
    addls     r3, r3, #1                        @ count if in range

@ 1373 :         if (eq_cnt !== 0)
    ldrb      r8v6, [r4pCntr, +r9PD]            @@@@ v6 == [ppxlcCenter + iPixelDistance]
    cmp       r3, #0
    beq       HorzX8EqCnt1

@ 1374 :             eq_cnt +== phi1(pVtmpL[-iPixelDistance] - v1) + phi1(v5 - v6) + phi1(v6 - v7) + phi1(v7 - v8) + phi1(v8 - pVtmpR[iPixelDistance])@

    add       r2, r9PD, r9PD, lsl #2            @ 5 * iPixelDistance
    ldr       r0, [sp, #ov1]                    @ v1
    ldrb      r2, [r4pCntr, -r2]                @ [ppxlcCenter - 5 * iPixelDistance]

    sub       r2, r2, r0                        @ pVtmpL[-iPixelDistance] - v1
    add       r1, r2, lr                        @ (pVtmpL[-iPixelDistance] - v1) + iThr1
    cmp       r1, lr, lsl #1                    @ (unsigned(pVtmpL[-iPixelDistance] - v1 + iThr1) <== uThr2) 
    addls     r3, r3, #1                        @ count if in range

    sub       r2, r11v5, r8v6                   @ (v5 - v6)
    add       r1, r2, lr                        @ (v5 - v6) + iThr1
    cmp       r1, lr, lsl #1                    @ (unsigned(v5 - v6 + iThr1) <== uThr2) 
    addls     r3, r3, #1                        @ count if in range

    sub       r2, r8v6, r6v7                    @ v6 - v7
    add       r1, r2, lr                        @ (v6 - v7) + iThr1
    cmp       r1, lr, lsl #1                    @ (unsigned(v6 - v7 + iThr1) <== uThr2) 
    addls     r3, r3, #1                        @ count if in range

    ldrb      r0, [r4pCntr, +r9PD, lsl #2]      @@pVtmpR[iPixelDistance] == [ppxlcCenter + 4 * iPixelDistance]

    sub       r2, r6v7, r12v8                   @ v7 - v8
    add       r1, r2, lr                        @ (v7 - v8) + iThr1
    cmp       r1, lr, lsl #1                    @ (unsigned(v7 - v8) <== uThr2) 
    addls     r3, r3, #1                        @ count if in range

    sub       r2, r12v8, r0                     @@v8 - pVtmpR[iPixelDistance]
    add       r1, r2, lr                        @ (v8 - pVtmpR[iPixelDistance]) + iThr1
    cmp       r1, lr, lsl #1                    @ (unsigned(v8 - pVtmpR[iPixelDistance] + iThr1) <== uThr2) 
    addls     r3, r3, #1                        @ count if in range


HorzX8EqCnt1:

@ 1376 :         Bool bStrongFilter == FALSE@
@ 1377 :         if (eq_cnt >== 6) {

    cmp       r3, #6                            @ eq_cnt >== 6
    blt       UnStrongFilter

@ 1378 :             bStrongFilter == bMin_Max_LE_2QP_TMP (v1, v2, v3, v4, v5, v6, v7, v8, iStepSize*2)@        

r2min   .req r2
r3max   .req r3
@                       int min, max@
@                       if (v1 >== v8){ 
@                           min == v8@ max == v1@
@                       } else{
@                           min == v1@ max == v8@
@                       }
    ldr       r0, [sp, #ov1]
    mov       r3max, r0
    cmp       r0, r12v8                       @ v1 - v8
    mov       r2min, r12v8
    movlt     r3max, r12v8
    movlt     r2min, r0
    
@                       if (min > v3) min  == v3@
@                       else if (v3 > max) max == v3@
@                       if (min > v5) min  == v5@
@                       else if (v5 > max) max == v5@
    cmp       r2min, r7v3
    movgt     r2min, r7v3
    cmp       r7v3,  r3max
    movgt     r3max, r7v3
    cmp       r2min, r11v5
    movgt     r2min, r11v5
    cmp       r11v5, r3max
    movgt     r3max, r11v5
    
@                       if (max - min >== iStepSize*2) return FALSE@
    ldr       r1, [sp, #oiStepSize] 
    sub       r0, r3max, r2min
    cmp       r0, r1, LSL #1            @ (max - min >== iStepSize*2)
    bge       UnStrongFilter

@                       if (min > v2) min  == v2@
@                       else if (v2 > max) max == v2@
@                       if (min > v4) min  == v4@
@                       else if (v4 > max) max == v4@
@                       if (min > v6) min  == v6@
@                       else if (v6 > max) max == v6@
@                       if (min > v7) min  == v7@
@                       else if (v7 > max) max == v7@
    cmp       r2min, r5v2
    movgt     r2min, r5v2
    cmp       r5v2,  r3max
    movgt     r3max, r5v2
    cmp       r2min, r10v4
    movgt     r2min, r10v4
    cmp       r10v4, r3max
    movgt     r3max, r10v4
    cmp       r2min, r8v6
    movgt     r2min, r8v6
    cmp       r8v6,  r3max
    movgt     r3max, r8v6
    cmp       r2min, r6v7
    movgt     r2min, r6v7
    cmp       r6v7,  r3max
    movgt     r3max, r6v7

@                       return (Bool) (max - min < iStepSize*2)@
@ }
@ 1383 :         if (bStrongFilter) {
    sub       r0, r3max, r2min
    cmp       r0, r1, LSL #1            @ (max - min >== iStepSize*2)
    bge       UnStrongFilter

@ 1384 :             Int  v2plus7 == v2 + v7@            
@ 1385 :             *(ppxlcCenter - 2*iPixelDistance) == (3 * (v2 + v3) + v2plus7 + 4) >> 3@                                                        
    add       r3, r5v2, r7v3                    @ v2 + v3
    add       r3, r3, r3, LSL #1                @ 3 * (v2 + v3)
    add       r0, r6v7, r5v2                    @ v2plus7
    add       r0, r0, #4                        @ v2plusv7 + 4
    add       r3, r3, r0                        @ 3 * (v2 + v3) + v2plus7 + 4
    mov       r3, r3, asr #3                    @ (3 * (v2 + v3) + v2plus7 + 4) >> 3
    strb      r3, [r4pCntr, -r9PD, lsl #1]      @ *(ppxlcCenter - 2*iPixelDistance) == (3 * (v2 + v3) + v2plus7 + 4) >> 3

@ 1386 :             *(ppxlcCenter + iPixelDistance) == (3 * (v7 + v6) + v2plus7 + 4) >> 3@
    add       r3, r6v7, r8v6                    @ v7 + v6
    add       r3, r3, r3, lsl #1                @ 3 * (v7 + v6)
    add       r3, r3, r0                        @ 3 * (v7 + v6) + v2plus7 + 4
    mov       r3, r3, asr #3                    @ (3 * (v7 + v6) + v2plus7 + 4) >> 3
    strb      r3, [r4pCntr, r9PD]               @ *(ppxlcCenter + iPixelDistance) == (3 * (v7 + v6) + v2plus7 + 4) >> 3

@ 1387 :             v2plus7 <<== 1@
@ 1388 :             *(ppxlcCenter - iPixelDistance) == (v2 + 3 * v4 + v2plus7 + 4) >> 3@
    add       r3, r10v4, r10v4, lsl #1          @ 3 * v4
    mov       r0, r0, lsl #1                    @ v2plus7*2 + 8
    sub       r0, r0, #4                        @ v2plus7*2 + 4
    add       r3, r3, r0                        @ 3 * v4 + v2plus7 + 4
    add       r3, r3, r5v2                      @ v2 + 3 * v4 + v2plus7 + 4
    mov       r3, r3, asr #3                    @ (v2 + 3 * v4 + v2plus7 + 4) >> 3
    strb      r3, [r4pCntr, -r9PD]              @ *(ppxlcCenter - iPixelDistance) == (v2 + 3 * v4 + v2plus7 + 4) >> 3

@ 1389 :             *(ppxlcCenter) == (v7 + 3 * v5 + v2plus7 + 4) >> 3@
    add       r3, r11v5, r11v5, lsl #1          @ 3 * v5
    add       r3, r3, r0                        @ 3 * v5 + v2plus7 + 4
    add       r3, r3, r6v7                      @ v7 + 3 * v5 + v2plus7 + 4
    mov       r3, r3, asr #3                    @ (v7 + 3 * v5 + v2plus7 + 4) >> 3
    b         FilterBlockTail

@ 1390 :         } else {
UnStrongFilter:

lra30   .req  lr
r3abs   .req  r3
r7a31   .req  r7
r6a32   .req  r6
r2v45   .req  r2
r2c     .req  r2
r8a     .req  r8

@ 1393 :             Int v4_v5 == v4 - v5@
@ 1394 :             Int a30 == (2*(v3-v6) - 5*v4_v5 + 4) >> 3@
    sub       r2v45,r10v4,  r11v5               @ v4 - v5
    add       r0,   r2v45,  r2v45, LSL #2       @ 5*v4_v5
    sub       r3,   r7v3,   r8v6                @ v3 - v6
    rsb       r3,   r0,     r3, lsl #1          @ 2*(v3-v6) - 5*v4_v5
    add       r3,   r3,     #4                  @ 2*(v3-v6) - 5*v4_v5 + 4
    mov       lra30,        r3, asr #3          @ a30 == (2*(v3-v6) - 5*v4_v5 + 4) >> 3

@ 1395 :             Int absA30 == abs(a30)@
    movs      r0,   lra30,  asr #31
    eor       r3,   lra30,  r0
    ldr       r1,           [sp, #oiStepSize]
    rsb       r3abs,r0,     r3                  @ absA30 == abs(a30)

@ 1396 :             if (absA30 < iStepSize) {
    cmp       r3abs,        r1
    bge       FilterBlockEnd

@ 1398 :                 v2 -== v3@
@ 1399 :                 v6 -== v7@
@ 1400 :                 Int a31 == (2 * (v1-v4) - 5 * v2 + 4) >> 3@                                 
    ldr       r1,           [sp, #ov1]          @ v1
    sub       r5v2, r5v2,   r7v3                @ v2 -== v3                  {free: r7v3}
    sub       r1,   r1,     r10v4               @ v1 - v4
    add       r0,   r5v2,   r5v2, lsl #2        @ 5 * v2                    {free: r5v2}
    rsb       r1,   r0,     r1, lsl #1          @ 2 * (v1-v4) - 5 * v2
    add       r1,   r1,     #4                  @ 2 * (v1-v4) - 5 * v2 + 4
    mov       r7a31,        r1, asr #3          @ a31 == (2 * (v1-v4) - 5 * v2 + 4) >> 3

@ 1401 :                 Int a32 == (2 * (v5-v8) - 5 * v6 + 4) >> 3@    
    sub       r8v6, r8v6,   r6v7                @ v6 -== v7                  {free: r6v7}
    sub       r1,   r11v5,  r12v8               @ v5 - v8
    add       r6,   r8v6,   r8v6, lsl #2        @ 5 * v6                    {free: r8v6}
    rsb       r6,   r6,     r1, lsl #1          @ 2 * (v5-v8) - 5 * v6
    add       r6,   r6,     #4                  @ 2 * (v5-v8) - 5 * v6 + 4
    mov       r6a32,        r6, asr #3          @ a32 == (2 * (v5-v8) - 5 * v6 + 4) >> 3

@ 1404 :                 Int iMina31_a32 == min(abs(a31),abs(a32))@ 
    movs      r8,           r7a31, asr #31
    eor       r0,   r7a31,  r8
    rsb       r0,   r8,     r0
    movs      r8,           r6a32, asr #31
    eor       r1,   r6a32,  r8
    rsb       r1,   r8,     r1
    cmp       r0,   r1
    movge     r0,   r1                          @ iMina31_a32

@ 1408 :                 if (iMina31_a32 < absA30){
    cmp       r0,   r3abs
    bge       FilterBlockEnd 

@ 1410 :                     Int a, c == v4_v5 / 2@                
    cmp       r2v45,        #0
    addmi     r2v45,r2v45,  #1
    mov       r2c,          r2v45, asr #1

@ 1412 :                     if (0 < c) {
    cmp       r2c,          #0
    ble       FilterBlock2

@ 1413 :                         if (a30 < 0) {
    cmp       lra30,        #0
    bpl       FilterBlockEnd 

@ 1415 :                             Int dA30 == absA30 - iMina31_a32 @
@ 1416 :                             a == (5 * dA30) >> 3@
    sub       r1,   r3abs,  r0                  @ dA30 == absA30 - iMina31_a32
    add       r1,   r1,     r1, lsl #2          @ 5 * dA30
    mov       r8a,  r1,     asr #3              @ a == (5 * dA30) >> 3

@ 1417 :                             if (a > c) a == c@

    cmp       r8a,  r2c
    movgt     r8a,  r2c
    b         FilterBlockTail0

@ 1418 :                             *(ppxlcCenter - iPixelDistance) == v4 - a@                                                                                         
@ 1419 :                             *(ppxlcCenter) == v5 + a@
@ 1420 :                         } 
@ 1421 :                     } else if (c < 0) {

FilterBlock2:
    bpl       FilterBlockEnd 

@ 1422 :                         if (a30 >== 0) {                       
    cmp       lra30,        #0
    bmi       FilterBlockEnd 

@ 1424 :                             Int dA30 ==  iMina31_a32 - absA30@ // < 0
@ 1425 :                             a == (5 * dA30 + 7) >> 3@ // <== 0
    sub       r3,   r0,     r3abs               @ dA30 ==  iMina31_a32 - absA30
    add       r3,   r3,     r3, lsl #2          @ 5 * dA30
    add       r3,   r3,     #7                  @ 5 * dA30 + 7
    mov       r8a,          r3, asr #3          @ a == (5 * dA30 + 7) >> 3

@ 1426 :                             if (a < c) a == c@
    cmp       r8a,          r2c
    movlt     r8a,          r2c      

FilterBlockTail0:

@ 1427 :                             *(ppxlcCenter - iPixelDistance) == v4 - a@                                                                
@ 1428 :                             *(ppxlcCenter) == v5 + a@
    sub       r0,   r10v4,  r8a                         @ v4 - a         {free: r10v4}
    strb      r0,           [r4pCntr, -r9PD]            @ *(ppxlcCenter - iPixelDistance) == v4 - a
    add       r3,   r8a,    r11v5                       @ v5 + a          {free: r11v5}

FilterBlockTail:
    strb      r3,           [r4pCntr]                   @ *(ppxlcCenter) == whatever

FilterBlockEnd:

@ 1429 :                         }
@ 1430 :                     }
@ 1448 :                 }
@ 1449 :             }        
@ 1450 :         }        
@ 1451 :         ppxlcCenter +== 1@

    ldr       r2,       [sp, #oiPixelInc]
    ldr       r3,       [sp, #oi]
    add       r4pCntr,  r4pCntr, r2
    sub       r0, r3,   #1
    str       r0,       [sp, #oi]
    cmp       r0,       #0
    bhi       HorzX8iLoopTop

@ 1452 :     }
@ 1453 : }

    add       sp, sp, #StackTempSizeH  @ 0x44 == 68
    ldmia     sp, {r4 - r11, sp, pc}  @ ldmfd
    WMV_ENTRY_END

M25445:

      @ ?FilterHorzEdgeX8@CSpatialPredictor@@CAXPAEHH@Z, CSpatialPredictor::FilterHorzEdgeX8


@*****************************************************************************************************
@*****************************************************************************************************
@ FilterVertEdgeX8
@ extern "C" Void_WMV FilterVertEdgeX8 (PixelC* ppxlcCenter, I32_WMV iPixelIncrement, I32_WMV iStepSize)@
@*****************************************************************************************************

    @AREA    .text, CODE


@ extern "C" Void_WMV FilterVertEdgeX8 (PixelC* ppxlcCenter, I32_WMV iPixelIncrement, I32_WMV iStepSize)@
    WMV_LEAF_ENTRY FilterVertEdgeX8 @ FilterVertEdgeX8

    mov       r12, sp
    stmdb     sp!, {r1, r2}  @ stmfd
    stmdb     sp!, {r4 - r12, lr}  @ stmfd
    FRAME_PROFILE_COUNT

    @ share code with vertical filter by setting up oiPixelInc and r2==iPixelDistance
    sub       sp, sp, #StackTempSizeH
    str       r2, [sp, #oiStepSize]  
    str       r1, [sp, #oiPixelInc]
    mov       r1, #1                        @ iPixelDistance
    b         FilterEitherEdgeX8

    WMV_ENTRY_END

      @ ?FilterVertEdgeX8@CSpatialPredictor@@CAXPAEHH@Z, CSpatialPredictor::FilterVertEdgeX8


@****************************************************************************************************
@ utility functions
@****************************************************************************************************
@ #define ALIGNED32_MEMSET_U32(pDst,u32C,cbSiz) {            
@            U32* pD==(U32*)(pDst)@                           
@            for(int j==(cbSiz)>>2@ j>0@j--)                  
@                *pD++ == u32C@                               
@        }
@ void aligned32_memset_u32( void* pDst, unsigned int u32C, unsigned int cbSiz )

    .global  _aligned32_memset_u32x4

_aligned32_memset_u32x4:             @ total == 2 + n
    subs    r2, r2, #4              @ 1n/4
    str     r1, [r0], #+4           @ 1n/4
    bne     _aligned32_memset_u32x4 @ 2n/4
    mov     pc, lr                  @ 2

@
@   _aligned32_memset_u32x8             @ total == 3 + 5n/8  [n==128, t==83]
@       mov     r3, r1                  @ 1
@   msloop1
@       subs    r2, r2, #8              @ 1n/8
@       stmia   r0!, { r1, r3 }         @ 2n/8
@       bne     msloop1                 @ 2n/8
@       mov     pc, lr                  @ 2

    .global  _aligned32_memset_u32x8

_aligned32_memset_u32x8:             @ total == 2 + 5n/8  [n==128, t==82]
    subs    r2, r2, #8              @ 1n/8
    str     r1, [r0], #+4           @ 1n/8
    str     r1, [r0], #+4           @ 1n/8
    bne     _aligned32_memset_u32x8 @ 2n/8
    mov     pc, lr                  @ 2

@   _aligned32_memset_u32x32            @ total == 9 + 7n/32  [n==128, t==37]
@       stmdb     sp!, {r4, r5, pc}     @ 3
@       mov     r3, r1                  @ 1
@       mov     r4, r1                  @ 1
@       mov     r5, r1                  @ 1
@   msloop2
@       subs    r2, r2, #32             @ 1n/32
@       stmia   r0!, { r1, r3, r4, r5 } @ 4n/32
@       bne     msloop2                 @ 2n/32
@       ldmia     sp!, {r4, r5, pc}     @ 3


    .global  _aligned32_memset_u32x32

_aligned32_memset_u32x32:            @ total == 2 + 7n/32  [n==128, t==30]
    subs    r2, r2, #32             @ 1n/32
    str     r1, [r0], #+4           @ 1n/32
    str     r1, [r0], #+4           @ 1n/32
    str     r1, [r0], #+4           @ 1n/32
    str     r1, [r0], #+4           @ 1n/32
    bne    _aligned32_memset_u32x32 @ 2n/32
    mov     pc, lr                  @ 2

    .endif @ WMV_OPT_X8_ARM

    .END

