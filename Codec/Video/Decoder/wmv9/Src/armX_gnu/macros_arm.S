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
    #include "../c/voWMVDecID.h"
    .include "wmvdec_member_arm.inc"
    .include "xplatform_arm_asm.h" 

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @@AREA COMMON, CODE, READONLY
		.section .text
    .global g_supportStrongARM
    .global g_supportXScale
    .global g_supportARMv5
    .global g_supportWMMX

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @@AREA    .text, CODE
g_supportStrongARM:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    MRC     p15,0,r0,c0,c0,0
    LDR     r1, =0xFF0FFFF0
    AND     r0, r0, r1
    LDR     r1, =0x6901B110
    CMP     r0, r1
    MOVEQ   r0, #1
    MOVNE   r0, #0
    MOV     PC, r14

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA    .text, CODE
g_supportXScale:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    MRC     p15,0,r0,c0,c0,0        @ get Coprocessor 0 Register 0 (ID)
    MOV     r12, r0, LSR #24        
    MOV     r0,  r0, LSR #16        @ Arch Version
    AND     r0,  r0, #0xFF          @ Arch Version
    CMP     r0,  #05                @ Is it >== 5
    MOVGE   r0,  #1
    MOVLT   r0,  #0
    CMP     r12, #0x69              @ implementation trademark "i"
    MOVNE   r0,  #0
    MOV     PC,  r14

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA    .text, CODE
g_supportARMv5:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    MRC     p15,0,r0,c0,c0,0        @ get Coprocessor 0 Register 0 (ID)
    MOV     r0, r0, LSR #16         @ Arch Version
    AND     r0, r0, #0xFF           @ Arch Version
    CMP     r0, #05                 @ Is it >== 5
    MOVGE   r0, #1
    MOVLT   r0, #0
    MOV     PC, r14

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA    .text, CODE
g_supportWMMX:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    MRC     p15,0,r0,c0,c0,0
    LDR     r1, =0xFFFF0FF0
    AND     r0, r0, r1
    LDR     r1, =0x69050110
    CMP     r0, r1
    MOVEQ   r0, #1
    MOVNE   r0, #0
    MOV     PC, r14

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    .if WMV_OPT_COMMON_ARM==1

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@define .macros

    .macro Copy2AlignedWords   dr,sr,di,si,b1,b0

    ldr     \b1,  [\sr, #4]
    ldr     \b0, [\sr], \si
    str     \b1,  [\dr, #4]
    str     \b0, [\dr], \di
    .endm

    .macro Cpy2Words dr, sr, di, si, b2, b1, b0, ls, rs

    ldr     \b2, [\sr, #8]
    ldr     \b1, [\sr, #4]
    mov     \b2, \b2, lsl \ls
    ldr     \b0, [\sr], \si
    orr     \b2, \b2, \b1, lsr \rs
    str     \b2, [\dr, #4]

    mov     \b1, \b1, lsl \ls
    orr     \b1, \b1, \b0, lsr \rs
    str     \b1, [\dr], \di
    .endm


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
    .global memclr256_arm
    .global memclr128_arm
    .global memcpyOddEven_arm
@   extern void memclr256_arm (void* pDst)@
@   extern void memclr128_arm (void* pDst)@
@   extern void memcpyOddEven_arm (pDst1, pSrc, iWidth1)@

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA    .text, CODE
    WMV_LEAF_ENTRY memcpyOddEven_arm
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    STMFD   sp!, {r4 - r5, r14} 
    
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

    LDMFD   sp!, {r4 - r5, PC}
    WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA    .text, CODE
    WMV_LEAF_ENTRY memclr256_arm
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    STMFD   sp!, {r4 - r8, r14} 
    FRAME_PROFILE_COUNT
    MOV     r1, #0    
    MOV     r2, #0
    MOV     r3, #0
    MOV     r4, #0
    MOV     r5, #0
    MOV     r6, #0
    MOV     r7, #0
    MOV     r8, #0
    STMIA   r0!, {r1 - r8}  
    STMIA   r0!, {r1 - r8}  
    STMIA   r0!, {r1 - r8}  
    STMIA   r0!, {r1 - r8} 
    STMIA   r0!, {r1 - r8}  
    STMIA   r0!, {r1 - r8}  
    STMIA   r0!, {r1 - r8}  
    STMIA   r0!, {r1 - r8} 
    LDMFD   sp!, {r4 - r8, PC}
    WMV_ENTRY_END
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @AREA    .text, CODE
    WMV_LEAF_ENTRY memclr128_arm
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    STMFD   sp!, {r4 - r8, r14}                   
    FRAME_PROFILE_COUNT
    MOV     r1, #0    
    MOV     r2, #0
    MOV     r3, #0
    MOV     r4, #0
    MOV     r5, #0
    MOV     r6, #0
    MOV     r7, #0
    MOV     r8, #0
    STMIA   r0!, {r1 - r8}  
    STMIA   r0!, {r1 - r8}  
    STMIA   r0!, {r1 - r8}  
    STMIA   r0!, {r1 - r8}     
    LDMFD   sp!, {r4 - r8, PC}
    WMV_ENTRY_END
@
@ *********************************************************************
@   prvWMMemClr( void* pDest, int cBytes2Clear )
@   This is a fast memory clear function when clearing large blocks of memory
@   It can handle any size cBytes2Clear and any alignment of pDest
@
    .global prvWMMemClr

    @AREA    .text, CODE
    WMV_LEAF_ENTRY prvWMMemClr
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

    @ first clear bytes or half-words until pointer is aligned
    tst       r0,  #1
    subne     r1, r1, #1
    strneb    r2, [r0], #+1
    tst       r0,  #2
    subne     r1, r1, #2
    strneh    r2, [r0], #+2

    @ now clear 32 bytes at a time
mcLoop32:
    subs      r1, r1, #8*4
    stmplia   r0!, {r2-r7, r12, lr}
    bpl       mcLoop32

    @ now clear the remainder trying 16, 8, 4, 2, 1 at a time
    adds      r1, r1, #4*4
    stmplia   r0!, {r2-r5}
    addmi     r1, r1, #4*4
    subs      r1, r1, #2*4
    stmplia   r0!, {r2-r3}
    addmi     r1, r1, #2*4
    subs      r1, r1, #4
    strpl     r2, [r0], #+4
    addmi     r1, r1, #4
    subs      r1, r1, #2
    strplh    r2, [r0], #+2
    addmi     r1, r1, #2
    subs      r1, r1, #1
    strplb    r2, [r0]

    ldmia     sp!, {r4 - r7, pc}  @ ldmfd
    WMV_ENTRY_END

@ *********************************************************************
@   prvWMMemClrAligned( void* pDest, int cBytes2Clear )
@   This is a fast memory clear function when clearing large blocks of memory
@   cBytes2Clear mod 4 ==== 0 and pDest must be aligned

    .global prvWMMemClrAligned

    @AREA    .text, CODE
    WMV_LEAF_ENTRY prvWMMemClrAligned
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
    .global prvWMMemCpyAligned

    @AREA    .text, CODE
    WMV_LEAF_ENTRY prvWMMemCpyAligned
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
@   prvWMMemCpy16Aligned(void *dest, void *src, 16)
@   This is a fast memory copy function when copying large blocks of memory
@   dest and src must both aligned
@
    .global prvWMMemCpy16Aligned

    @AREA    .text, CODE
    WMV_LEAF_ENTRY prvWMMemCpy16Aligned
@   stmdb     sp!, {lr}    @ stmfd
    str     lr, [sp, #-4]!
    FRAME_PROFILE_COUNT

    ldmia   r1!, {r2-r3, r12, lr} @ read from src
    stmia   r0!, {r2-r3, r12, lr} @ write to dst

    ldr     pc, [sp], #4
@   ldmia     sp!, {pc}  @ ldmfd
    WMV_ENTRY_END
@
@   prvWMMemCpy16DestAligned(void *dest, void *src, 16)
@   This is a fast memory copy function when copying large blocks of memory
@   dest must aligned. src can be unaligned
@
    .global prvWMMemCpy16DestAligned

    @AREA    .text, CODE
    WMV_LEAF_ENTRY prvWMMemCpy16DestAligned
@   stmdb   sp!, {lr}    @ stmfd
    str     lr, [sp, #-4]!
    FRAME_PROFILE_COUNT

    ands    r2, r1, #3
    bne     Cpy16DestAlignedFull

    ldmia   r1!, {r2-r3, r12, lr} @ read from src
    stmia   r0!, {r2-r3, r12, lr} @ write to dst

    ldr     pc, [sp], #4
@   ldmia   sp!, {pc}  @ ldmfd
    WMV_ENTRY_END

    WMV_LEAF_ENTRY Cpy16DestAlignedFull

    stmdb   sp!, {r4-r6}    @ stmfd
    bic     r1, r1, #3
    ldr     r3, [r1], #4
    ldmia   r1!, {r4-r6, r12}

    mov     lr, r2, lsl #3
    rsb     r2, lr, #32
    mov     r3, r3, lsr lr
    orr     r3, r3, r4, lsl r2
    mov     r4, r4, lsr lr
    orr     r4, r4, r5, lsl r2
    mov     r5, r5, lsr lr
    orr     r5, r5, r6, lsl r2
    stmia   r0!, {r3-r5}
    mov     r6, r6, lsr lr
    orr     r6, r6, r12, lsl r2
    str     r6, [r0]
@    stmia   r0!, {r3-r6}
    ldmia   sp!, {r4-r6, pc}  @ ldmfd
    WMV_ENTRY_END

@
@   prvWMMemCpy8Aligned(void *dest, void *src,8)
@   This is a fast memory copy function when copying large blocks of memory
@   dest and src must both aligned
@
    .global prvWMMemCpy8Aligned

    @AREA    .text, CODE
    WMV_LEAF_ENTRY prvWMMemCpy8Aligned
    .if PROFILE_FRAMES_FUNCTION_COUNT == 1
        str     lr, [sp, #-4]!
        FRAME_PROFILE_COUNT
        ldr     lr, [sp]!, #4
    .endif
    ldmia   r1!, {r2-r3} @ read from src
    stmia   r0!, {r2-r3} @ write to dst
    mov       pc,  lr
    WMV_ENTRY_END

@
@   FASTCPY_8x16(void *dest, void *src, iDstStride, iSrcStride)
@   This is a fast memory copy function when copying large blocks of memory
@   dest and src must both aligned
@
    .global FASTCPY_8x16

    @AREA    .text, CODE

	.if _XSC_==0 @ SA1110 version
    WMV_LEAF_ENTRY FASTCPY_8x16
    stmdb   sp!, {r4-r5, lr}    @ stmfd
    FRAME_PROFILE_COUNT

    ldmia   r1, {r4-r5, r12, lr} @ read from src
    stmia   r0, {r4-r5, r12, lr} @ write to dst

    add     r1, r1, r3
    ldmia   r1, {r4-r5, r12, lr} @ read from src
    add     r0, r0, r2
    stmia   r0, {r4-r5, r12, lr} @ write to dst

    add     r1, r1, r3
    ldmia   r1, {r4-r5, r12, lr} @ read from src
    add     r0, r0, r2
    stmia   r0, {r4-r5, r12, lr} @ write to dst

    add     r1, r1, r3
    ldmia   r1, {r4-r5, r12, lr} @ read from src
    add     r0, r0, r2
    stmia   r0, {r4-r5, r12, lr} @ write to dst

    add     r1, r1, r3
    ldmia   r1, {r4-r5, r12, lr} @ read from src
    add     r0, r0, r2
    stmia   r0, {r4-r5, r12, lr} @ write to dst

    add     r1, r1, r3
    ldmia   r1, {r4-r5, r12, lr} @ read from src
    add     r0, r0, r2
    stmia   r0, {r4-r5, r12, lr} @ write to dst

    add     r1, r1, r3
    ldmia   r1, {r4-r5, r12, lr} @ read from src
    add     r0, r0, r2
    stmia   r0, {r4-r5, r12, lr} @ write to dst

    add     r1, r1, r3
    ldmia   r1, {r4-r5, r12, lr} @ read from src
    add     r0, r0, r2
    stmia   r0, {r4-r5, r12, lr} @ write to dst

    ldmia   sp!, {r4-r5, pc}
    WMV_ENTRY_END
	.endif

    .if _XSC_==1 @ XSCALE version
    WMV_LEAF_ENTRY FASTCPY_8x16
    stmdb   sp!, {r4-r5, lr}    @ stmfd
    FRAME_PROFILE_COUNT

    @ldmia   r1, {r4-r5, r12, lr} @ read from src
    @stmia   r0, {r4-r5, r12, lr} @ write to dst

    LDR     r4, [r1]
    LDR     r5, [r1, #4]
    LDR     r12, [r1, #8]
    LDR     lr, [r1, #12]
    @stmia   r0, {r4-r5, r12, lr} @ write to dst
	STR     r4, [r0]
    STR     r5, [r0, #4]
    STR     r12, [r0, #8]
    STR     lr, [r0, #12]

    add     r1, r1, r3
    @ldmia   r1, {r4-r5, r12, lr} @ read from src
    LDR     r4, [r1]
    LDR     r5, [r1, #4]
    LDR     r12, [r1, #8]
    LDR     lr, [r1, #12]
    add     r0, r0, r2
    @stmia   r0, {r4-r5, r12, lr} @ write to dst
    STR     r4, [r0]
    STR     r5, [r0, #4]
    STR     r12, [r0, #8]
    STR     lr, [r0, #12]

    add     r1, r1, r3
    @ldmia   r1, {r4-r5, r12, lr} @ read from src
    LDR     r4, [r1]
    LDR     r5, [r1, #4]
    LDR     r12, [r1, #8]
    LDR     lr, [r1, #12]
    add     r0, r0, r2
    @stmia   r0, {r4-r5, r12, lr} @ write to dst
    STR     r4, [r0]
    STR     r5, [r0, #4]
    STR     r12, [r0, #8]
    STR     lr, [r0, #12]

    add     r1, r1, r3
    @ldmia   r1, {r4-r5, r12, lr} @ read from src
    LDR     r4, [r1]
    LDR     r5, [r1, #4]
    LDR     r12, [r1, #8]
    LDR     lr, [r1, #12]
    add     r0, r0, r2
    @stmia   r0, {r4-r5, r12, lr} @ write to dst
    STR     r4, [r0]
    STR     r5, [r0, #4]
    STR     r12, [r0, #8]
    STR     lr, [r0, #12]

    add     r1, r1, r3
    @ldmia   r1, {r4-r5, r12, lr} @ read from src
    LDR     r4, [r1]
    LDR     r5, [r1, #4]
    LDR     r12, [r1, #8]
    LDR     lr, [r1, #12]
    add     r0, r0, r2
    @stmia   r0, {r4-r5, r12, lr} @ write to dst
    STR     r4, [r0]
    STR     r5, [r0, #4]
    STR     r12, [r0, #8]
    STR     lr, [r0, #12]

    add     r1, r1, r3
    @ldmia   r1, {r4-r5, r12, lr} @ read from src
    LDR     r4, [r1]
    LDR     r5, [r1, #4]
    LDR     r12, [r1, #8]
    LDR     lr, [r1, #12]
    add     r0, r0, r2
    @stmia   r0, {r4-r5, r12, lr} @ write to dst
    STR     r4, [r0]
    STR     r5, [r0, #4]
    STR     r12, [r0, #8]
    STR     lr, [r0, #12]

    add     r1, r1, r3
    @ldmia   r1, {r4-r5, r12, lr} @ read from src
    LDR     r4, [r1]
    LDR     r5, [r1, #4]
    LDR     r12, [r1, #8]
    LDR     lr, [r1, #12]
    add     r0, r0, r2
    @stmia   r0, {r4-r5, r12, lr} @ write to dst
    STR     r4, [r0]
    STR     r5, [r0, #4]
    STR     r12, [r0, #8]
    STR     lr, [r0, #12]

    add     r1, r1, r3
    @ldmia   r1, {r4-r5, r12, lr} @ read from src
    LDR     r4, [r1]
    LDR     r5, [r1, #4]
    LDR     r12, [r1, #8]
    LDR     lr, [r1, #12]
    add     r0, r0, r2
    @stmia   r0, {r4-r5, r12, lr} @ write to dst
    STR     r4, [r0]
    STR     r5, [r0, #4]
    STR     r12, [r0, #8]
    STR     lr, [r0, #12]

    ldmia   sp!, {r4-r5, pc}
    WMV_ENTRY_END
    .endif

@FASTCPY_8x8(pDst2, pSrc2, iDstStride, iSrcStride)
@   This is a fast memory copy function when copying large blocks of memory
@   dest must aligned. src can be unaligned, iSrcStride, iDstStride mod 4 == 0

    .global FASTCPY_8x8

    @AREA    .text, CODE
    WMV_LEAF_ENTRY FASTCPY_8x8

@   stmdb   sp!, {lr}    @ stmfd
    str     lr, [sp, #-4]!
    FRAME_PROFILE_COUNT

    ands    r12, r1, #3
    bne     FASTCPY_8x8Full
    
    Copy2AlignedWords   r0, r1, r2, r3, lr, r12
    Copy2AlignedWords   r0, r1, r2, r3, lr, r12
    Copy2AlignedWords   r0, r1, r2, r3, lr, r12
    Copy2AlignedWords   r0, r1, r2, r3, lr, r12
    Copy2AlignedWords   r0, r1, r2, r3, lr, r12
    Copy2AlignedWords   r0, r1, r2, r3, lr, r12
    Copy2AlignedWords   r0, r1, r2, r3, lr, r12
    Copy2AlignedWords   r0, r1, r2, r3, lr, r12

@   ldmia   sp!, {pc}  @ ldmfd
    ldr     pc, [sp], #4
    WMV_ENTRY_END

    WMV_LEAF_ENTRY FASTCPY_8x8Full

    stmdb   sp!, {r4-r6}

    bic     r1, r1, #3
    mov     lr, r12, lsl #3
    rsb     r12, lr, #32

    Cpy2Words r0, r1, r2, r3, r6, r5, r4, r12, lr
    Cpy2Words r0, r1, r2, r3, r6, r5, r4, r12, lr
    Cpy2Words r0, r1, r2, r3, r6, r5, r4, r12, lr
    Cpy2Words r0, r1, r2, r3, r6, r5, r4, r12, lr
    Cpy2Words r0, r1, r2, r3, r6, r5, r4, r12, lr
    Cpy2Words r0, r1, r2, r3, r6, r5, r4, r12, lr
    Cpy2Words r0, r1, r2, r3, r6, r5, r4, r12, lr
    Cpy2Words r0, r1, r2, r3, r6, r5, r4, r12, lr

    ldmia   sp!, {r4-r6, pc}
    WMV_ENTRY_END
@
@   prvWMMemCpy(void *dest, void *src, int nbytes)
@   This is a fast memory copy function when copying large blocks of memory
@
    .global prvWMMemCpy

    @AREA    .text, CODE
    WMV_LEAF_ENTRY prvWMMemCpy
    cmp       r2, #0
    movle     pc, lr                    @ do nothing for nbytes <== 0
    stmdb     sp!, {r4-r8, r12, lr}     @ stmfd
    FRAME_PROFILE_COUNT

    and       r3, r0, #3                @ dest % 4
    and       r4, r1, #3                @ src % 4
    subs      r12, r3, r4
    bne       mcpyByte                @ copy bytes/bytes

    cmp       r3, #0                    @ dest % 4 == src % 4
    beq       mcpLoop32             @ dest % 4 == 0, aligned

    rsb       r3, r3, #4
    sub       r2, r2, r3

    tst       r3, #1
    beq       mcpySec
           
    sub       r3, r3, #1                    
    ldrb      r5, [r1], #+1             @ copy one byte
    strb      r5, [r0], #+1

mcpySec:
    subs      r3, r3, #2
    ldrplh    r5, [r1], #+2             @ copy half word
    strplh    r5, [r0], #+2
    
    @ now copy 32 bytes at a time
mcpLoop32:
    subs      r2, r2, #8*4
    ldmplia   r1!, {r3-r8, r12, lr} @ read from src
    stmplia   r0!, {r3-r8, r12, lr} @ write to dst
    bpl       mcpLoop32

    @ now cpy the remainder trying 16, 8, 4 at a time
    adds      r2, r2, #4*4      @ r1 == remaing count -32: add 32 and subtract 16 ==== add 16
    ldmplia   r1!, {r3-r6}      @ read from src
    stmplia   r0!, {r3-r6}      
    addmi     r2, r2, #4*4      @ add 16 back in if we did not store
    subs      r2, r2, #2*4      @ sub 8 and set flags
    ldmplia   r1!, {r3-r4}      @ read from src
    stmplia   r0!, {r3-r4}
    addmi     r2, r2, #2*4      @ add 8 back in if we did not store
    subs      r2, r2, #4
    ldrpl     r3, [r1], #+4     @ copy 4 bytes
    strpl     r3, [r0], #+4
    addmi     r2, r2, #4
    subs      r2, r2, #2
    ldrplh    r3, [r1], #+2     @ copy 2 bytes
    strplh    r3, [r0], #+2
    addmi     r2, r2, #2
    subs      r2, r2, #1
    ldrplb    r3, [r1]          @ copy 1 byte
    strplb    r3, [r0]

    ldmia     sp!, {r4-r8, r12, pc}  @ ldmfd
    

mcpyByte:
    tst    r12, #1             @ see if can do half word: r12 ==== 2?
    bne    mcpyOneByte

@do half words
    ands    r3, r3, #1          @ dest % 4 & 0x01
    ldrneb  r4, [r1], #+1       @ copy the odd byte
    strneb  r4, [r0], #+1
    subne   r2, r2, #1

mcpyHalfWord:
    subs    r2, r2, #2          @ copy half word
    ldrplh  r4, [r1], #+2
    strplh  r4, [r0], #+2
    bpl    mcpyHalfWord 
    addmi   r2, r2, #2

mcpyOneByte:
    subs    r2, r2, #1          @ copy byte by byte
    ldrplb  r4, [r1], #+1
    strplb  r4, [r0], #+1
    bpl     mcpyOneByte 

    ldmia     sp!, {r4-r8, r12, pc}  @ ldmfd
    WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


@ *********************************************************************
@   prvWMMemSetAligned( void* pDest, unsigned char set_value, int cBytes2Set )
@   This is a fast memory set function when setting large blocks of memory
@   cBytes2Set mod 4 ==== 0 and pDest must be aligned

    .global prvWMMemSetAligned

    @AREA    .text, CODE
    WMV_LEAF_ENTRY prvWMMemSetAligned
    cmp       r2, #0
    movle     pc, lr                @ do nothing for cBytes2Set <== 0
    stmdb     sp!, {r4 - r7, lr}    @ stmfd
    FRAME_PROFILE_COUNT

    add       r1,  r1, r1, LSL #8
    add       r1,  r1, r1, LSL #16

    mov       r3,  r1
    mov       r4,  r1
    mov       r5,  r1
    mov       r6,  r1
    mov       r7,  r1
    mov       r12, r1
    mov       lr,  r1

    @ now clear 32 bytes at a time
msetLoop32:
    subs      r2, r2, #8*4
    stmplia   r0!, {r1, r3-r7, r12, lr}
    bpl       msetLoop32

    @ now clear the remainder trying 16, 8, 4 at a time
    adds      r2, r2, #4*4      @ r1 == remaing count -32: add 32 and subtract 16 ==== add 16
    stmplia   r0!, {r3-r6}      
    addmi     r2, r2, #4*4      @ add 16 back in if we did not store
    subs      r2, r2, #2*4      @ sub 8 and set flags
    stmplia   r0!, {r3-r4}
    addmi     r2, r2, #2*4      @ add 8 back in if we did not store
    subs      r2, r2, #4        @ set for last store
    strpl     r3, [r0]

    ldmia     sp!, {r4 - r7, pc}  @ ldmfd
    WMV_ENTRY_END

    .if PROFILE_FRAMES_FUNCTION_COUNT == 1
@ *********************************************************************
@   WMVFunctionCountFrameProfileArm()
@   indirectly call WMVFunctionCountFrameProfile with a unique function address
@   callable from within ARM assembly code (saves and restores all registers
    .global  WMVFunctionCountFrameProfileArm
    IMPORT  WMVFunctionCountFrameProfile

    @AREA    .text, CODE
WMVFunctionCountFrameProfileArm
    stmdb     sp!, {r0 - r10, r11, r12, lr}    @ stmfd
    sub       r0,   lr, #8
    bl        WMVFunctionCountFrameProfile
    ldmia     sp!, {r0 - r10, r11, r12, pc} @ ldmfd

    .endif @ PROFILE_FRAMES_FUNCTION_COUNT


    .global medianof3

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    
    @AREA    .embsec_PMainLoopLvl1, CODE
    WMV_LEAF_ENTRY medianof3
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ r0 == a0
@ r1 == a1
@ r2 == a2

    FRAME_PROFILE_COUNT

    CMP   r0, r1
    MOVGT r3, r1
    MOVGT r1, r0
    MOVGT r0, r3

    CMP   r2, r1
    MOVGE r0, r1
    BGE   gMedianof3End

    CMP   r0, r2
    MOVLT r0, r2

gMedianof3End:
    MOV   PC, lr
    WMV_ENTRY_END

    .global medianof4

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    
    @AREA    .embsec_PMainLoopLvl1, CODE
    WMV_LEAF_ENTRY medianof4
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
    .END 

