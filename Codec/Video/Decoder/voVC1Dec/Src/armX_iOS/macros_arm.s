@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************

@//************************************************************************
@//
@// Module Name:
@//
@//     macros_arm.s
@//
@// Abstract:
@// 
@//     ARM specific optimization for some common routines
@//
@//     Custom build with 
@//          armasm $(InputDir)\$(InputName).s $(OutDir)\$(InputName).obj
@//     and
@//          $(OutDir)\$(InputName).obj
@// 
@// Author:
@// 
@//     Chuang Gu (chuanggu@microsoft.com) Sept. 26, 2002
@//
@// Revision History:
@//
@//************************************************************************
		#include "../../Src/c/voVC1DID.h"
    .include "wmvdec_member_arm.h"
    .include "xplatform_arm_asm.h" 

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @@AREA COMMON, CODE, READONLY
		 .text
		.align 8

    .if WMV_OPT_COMMON_ARM==1
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
    .globl _memcpyOddEven_ARMV4

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA    .text, CODE
    WMV_LEAF_ENTRY memcpyOddEven_ARMV4
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    stmfd   sp!, {r4 - r5, r14} 
    
    FRAME_PROFILE_COUNT
    mov       r12, #8

LoopMemcpyOddEven_arm:
    ldr     r3, [r1]
    ldr     r5, [r1, #4]
    ldr     r4, [r1, #64]
    ldr     r14, [r1, #68]

    strh    r3, [r0]
    strh    r4, [r0, #2]
    strh    r5, [r0, #8]
    strh    r14, [r0, #10]

    mov     r3, r3, lsr #16
    mov     r4, r4, lsr #16
    mov     r5, r5, lsr #16  
    mov     r14, r14, lsr #16   
    
    strh    r3, [r0, #4] 
    strh    r4, [r0, #6] 
    strh    r5, [r0, #12]   
    strh    r14, [r0, #14]  

    add     r1, r1, #8
    add     r0, r0, r2, lsl #1
    subs    r12, r12, #1
    bne     LoopMemcpyOddEven_arm

    ldmfd   sp!, {r4 - r5, PC}
    WMV_ENTRY_END

@ *********************************************************************
@   prvWMMemClrAligned( void* pDest, int cBytes2Clear )
@   This is a fast memory clear function when clearing large blocks of memory
@   cBytes2Clear mod 4 ==== 0 and pDest must be aligned

    .globl _prvWMMemClrAligned_ARMV4

    @AREA    .text, CODE
    WMV_LEAF_ENTRY prvWMMemClrAligned_ARMV4
    cmp       r1, #0
    movle     pc, lr                @ do nothing for cBytes2Clear <== 0
    stmdb     sp!, {r4 - r7, lr}    @ stmfd
    FRAME_PROFILE_COUNT

    mov       r2,  #0
    mov       r3,  #0
    mov       r4,  #0
    mov       r5,  #0
    mov       r6,  #0
    mov       r7,  #0
    mov       r12, #0
    mov       lr,  #0

    @ now clear 32 bytes at a time
mcaLoop32:
    subs      r1, r1, #8*4
    stmplia   r0!, {r2-r7, r12, lr}
    bpl       mcaLoop32

    @ now clear the remainder trying 16, 8, 4 at a time
    adds      r1, r1, #4*4      @ r1 == remaing count -32: add 32 and subtract 16 ==== add 16
    stmplia   r0!, {r2-r5}      
    addmi     r1, r1, #4*4      @ add 16 back in if we did not store
    subs      r1, r1, #2*4      @ sub 8 and set flags
    stmplia   r0!, {r2-r3}
    addmi     r1, r1, #2*4      @ add 8 back in if we did not store
    subs      r1, r1, #4        @ set for last store
    strpl     r2, [r0]

    ldmia     sp!, {r4 - r7, pc}  @ ldmfd
    WMV_ENTRY_END
@
@   prvWMMemCpyAligned(void *dest, void *src, int nbytes)
@   This is a fast memory copy function when copying large blocks of memory
@   dest and src must both aligned and nbytes mod 4 == 0
@
    .globl _prvWMMemCpyAligned_ARMV4

    @AREA    .text, CODE
    WMV_LEAF_ENTRY prvWMMemCpyAligned_ARMV4
    cmp       r2, #0
    movle     pc, lr                @ do nothing for cBytes2Clear <== 0
    stmdb     sp!, {r4-r8, r12, lr}    @ stmfd
    FRAME_PROFILE_COUNT

    @ now copy 32 bytes at a time
mcpyLoop32:
    subs      r2, r2, #8*4
    ldmplia   r1!, {r3-r8, r12, lr} @ read from src
    stmplia   r0!, {r3-r8, r12, lr} @ write to dst
    bpl       mcpyLoop32

    @ now cpy the remainder trying 16, 8, 4 at a time
    adds      r2, r2, #4*4      @ r1 == remaing count -32: add 32 and subtract 16 ==== add 16
    ldmplia   r1!, {r3-r6}      @ read from src
    stmplia   r0!, {r3-r6}      
    addmi     r2, r2, #4*4      @ add 16 back in if we did not store
    subs      r2, r2, #2*4      @ sub 8 and set flags
    ldmplia   r1!, {r3-r4}      @ read from src
    stmplia   r0!, {r3-r4}
    addmi     r2, r2, #2*4      @ add 8 back in if we did not store
    subs      r2, r2, #4        @ set for last store
    ldrpl     r3, [r1]
    strpl     r3, [r0]

    ldmia     sp!, {r4-r8, r12, pc}  @ ldmfd
    WMV_ENTRY_END
@
@   FASTCPY_8x16(void *dest, void *src, iDstStride, iSrcStride)
@   This is a fast memory copy function when copying large blocks of memory
@   dest and src must both aligned
@
    .globl _FASTCPY_8x16_ARMV4

    .if _XSC_==1 @ XSCALE version
    WMV_LEAF_ENTRY FASTCPY_8x16_ARMV4
    stmdb   sp!, {r4-r5, lr}    @ stmfd
    FRAME_PROFILE_COUNT

    @ldmia   r1, {r4-r5, r12, lr} @ read from src
    @stmia   r0, {r4-r5, r12, lr} @ write to dst

    ldr     r4, [r1]
    ldr     r5, [r1, #4]
    ldr     r12, [r1, #8]
    ldr     lr, [r1, #12]
    @stmia   r0, {r4-r5, r12, lr} @ write to dst
	str     r4, [r0]
    str     r5, [r0, #4]
    str     r12, [r0, #8]
    str     lr, [r0, #12]

    add     r1, r1, r3
    @ldmia   r1, {r4-r5, r12, lr} @ read from src
    ldr     r4, [r1]
    ldr     r5, [r1, #4]
    ldr     r12, [r1, #8]
    ldr     lr, [r1, #12]
    add     r0, r0, r2
    @stmia   r0, {r4-r5, r12, lr} @ write to dst
    str     r4, [r0]
    str     r5, [r0, #4]
    str     r12, [r0, #8]
    str     lr, [r0, #12]

    add     r1, r1, r3
    @ldmia   r1, {r4-r5, r12, lr} @ read from src
    ldr     r4, [r1]
    ldr     r5, [r1, #4]
    ldr     r12, [r1, #8]
    ldr     lr, [r1, #12]
    add     r0, r0, r2
    @stmia   r0, {r4-r5, r12, lr} @ write to dst
    str     r4, [r0]
    str     r5, [r0, #4]
    str     r12, [r0, #8]
    str     lr, [r0, #12]

    add     r1, r1, r3
    @ldmia   r1, {r4-r5, r12, lr} @ read from src
    ldr     r4, [r1]
    ldr     r5, [r1, #4]
    ldr     r12, [r1, #8]
    ldr     lr, [r1, #12]
    add     r0, r0, r2
    @stmia   r0, {r4-r5, r12, lr} @ write to dst
    str     r4, [r0]
    str     r5, [r0, #4]
    str     r12, [r0, #8]
    str     lr, [r0, #12]

    add     r1, r1, r3
    @ldmia   r1, {r4-r5, r12, lr} @ read from src
    ldr     r4, [r1]
    ldr     r5, [r1, #4]
    ldr     r12, [r1, #8]
    ldr     lr, [r1, #12]
    add     r0, r0, r2
    @stmia   r0, {r4-r5, r12, lr} @ write to dst
    str     r4, [r0]
    str     r5, [r0, #4]
    str     r12, [r0, #8]
    str     lr, [r0, #12]

    add     r1, r1, r3
    @ldmia   r1, {r4-r5, r12, lr} @ read from src
    ldr     r4, [r1]
    ldr     r5, [r1, #4]
    ldr     r12, [r1, #8]
    ldr     lr, [r1, #12]
    add     r0, r0, r2
    @stmia   r0, {r4-r5, r12, lr} @ write to dst
    str     r4, [r0]
    str     r5, [r0, #4]
    str     r12, [r0, #8]
    str     lr, [r0, #12]

    add     r1, r1, r3
    @ldmia   r1, {r4-r5, r12, lr} @ read from src
    ldr     r4, [r1]
    ldr     r5, [r1, #4]
    ldr     r12, [r1, #8]
    ldr     lr, [r1, #12]
    add     r0, r0, r2
    @stmia   r0, {r4-r5, r12, lr} @ write to dst
    str     r4, [r0]
    str     r5, [r0, #4]
    str     r12, [r0, #8]
    str     lr, [r0, #12]

    add     r1, r1, r3
    @ldmia   r1, {r4-r5, r12, lr} @ read from src
    ldr     r4, [r1]
    ldr     r5, [r1, #4]
    ldr     r12, [r1, #8]
    ldr     lr, [r1, #12]
    add     r0, r0, r2
    @stmia   r0, {r4-r5, r12, lr} @ write to dst
    str     r4, [r0]
    str     r5, [r0, #4]
    str     r12, [r0, #8]
    str     lr, [r0, #12]

    ldmia   sp!, {r4-r5, pc}
    WMV_ENTRY_END
    .endif

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .globl _medianof3_ARMV4

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    
    @AREA    .embsec_PMainLoopLvl1, CODE
    WMV_LEAF_ENTRY medianof3_ARMV4
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ r0 == a0
@ r1 == a1
@ r2 == a2

    FRAME_PROFILE_COUNT

    cmp   r0, r1
    movgt r3, r1
    movgt r1, r0
    movgt r0, r3

    cmp   r2, r1
    movge r0, r1
    bge   gMedianof3End

    cmp   r0, r2
    movlt r0, r2

gMedianof3End:
    mov   PC, lr
    WMV_ENTRY_END

    .globl _medianof4_ARMV4

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    
    @AREA    .embsec_PMainLoopLvl1, CODE
    WMV_LEAF_ENTRY medianof4_ARMV4
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ r0 == a0
@ r1 == a1
@ r2 == a2
@ r3 == a3

@ lr == min
@ r12== max

@   STMDB  sp!, {lr}  @ stmfd
    str    lr,  [sp, #-4]!

    FRAME_PROFILE_COUNT

    cmp    r1, r0
    mov    r12,r0
    mov    lr, r0

    movgt  r12,r1
    movlt  lr, r1

    cmp    r2, r12
    movgt  r12,r2
    cmple  r2, lr
    movlt  lr, r2

    cmp    r3, r12
    movgt  r12,r3
    cmple  r3, lr
    movlt  lr, r3

    add    r0, r0, r1
    add    r2, r2, r3
    add    lr, lr, r12
    add    r0, r0, r2
    subs   r0, r0, lr
    addmi  r0, r0, #1
    mov    r0, r0, asr #1

@   ldmia  sp!, {pc}
    ldr    pc, [sp], #4
    
    WMV_ENTRY_END
    .endif @ WMV_OPT_COMMON_ARM== 1


    @ this export must be at the end of the .s file
    @@.end 

