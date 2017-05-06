@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************
    #include "../c/voWMVDecID.h"
    .include "wmvdec_member_arm.h"
    .include "xplatform_arm_asm.h" 

    .if WMV_OPT_HUFFMAN_GET_ARM == 1

@	AREA	|.rdata|, DATA, READONLY
	@ .rdata	

    .globl _BS_GetMoreData
    .globl _BS_flush16_2
    .globl _BS_flush16
    .globl _getHuffman	
    
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  
@PRESERVE8
	@AREA	|.text|, CODE, READONLY
	 .text
	 .align 4


    WMV_LEAF_ENTRY BS_flush16_2
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ r0 = pThis

    stmfd  sp!, {r4, lr}
    FRAME_PROFILE_COUNT


@   if (pThis->m_pCurr == pThis->m_pLast)
    ldr    r2, [r0, #tagCInputBitStream_WMV_m_pCurr]  @ pThis->m_pCurr
    ldr    r3, [r0, #tagCInputBitStream_WMV_m_pLast]  @ pThis->m_pLast
    mov    r4, r0

    cmp    r2, r3
    bne    gBS_GetData_2

@   pThis->m_uBitMask += pThis->m_pCurr[0] << (8 - pThis->m_iBitsLeft)@
    ldr    r1, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]
    ldrb   r12,[r2]
    ldr    lr, [r0]
    rsb    r3, r1, #8
    add    r3, lr, r12, lsl r3
    str    r3, [r0]

@   pThis->m_pCurr += 1@
    add    r3, r2, #1
    str    r3, [r0, #tagCInputBitStream_WMV_m_pCurr]

@   pThis->m_iBitsLeft += 8@
    add    r3, r1, #8
    str    r3, [r4, #tagCInputBitStream_WMV_m_iBitsLeft]
    b      gBS_Return_2

gBS_GetData_2:
@   if(pThis->m_bNotEndOfFrame == TRUE_WMV)
    ldr    r3, [r0, #tagCInputBitStream_WMV_m_bNotEndOfFrame]!
    cmp    r3, #1
    bne    gBS_Error_2

@   BS_GetMoreData(pThis)@
    mov    r0, r4
    bl     _BS_GetMoreData

    mov    r1, #1
    b      gBS_End_2

gBS_Error_2:
@   else if (pThis->m_iBitsLeft < -16)
    ldr    r3, [r4, #tagCInputBitStream_WMV_m_iBitsLeft]
    mvn    r2, #0xF  @ -16
    cmp    r3, r2
    bge    gBS_Return_3

@   if (pThis->m_iStatus == 0)
    ldr    r3, [r4, #tagCInputBitStream_WMV_m_iStatus]
    cmp    r3, #0

@   pThis->m_iStatus = 2@
    moveq  r3, #2
    streq  r3, [r4, #tagCInputBitStream_WMV_m_iStatus]

@   pThis->m_iBitsLeft = 127@
    mov    r3, #127

gBS_Return_2:
    str    r3, [r4, #tagCInputBitStream_WMV_m_iBitsLeft]

gBS_Return_3:
    mov    r1, #0

gBS_End_2:
    mov    r0, r4

    ldmfd  sp!, {r4, pc}
    WMV_ENTRY_END    
    
    
    WMV_LEAF_ENTRY BS_flush16
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

   stmfd sp!, {r4,lr}
@    str   lr,  [sp, #-4]!

    FRAME_PROFILE_COUNT

@   pThis->m_uBitMask <<= iNumBits@
@   ldr   r3, [r0, #tagCInputBitStream_WMV_m_uBitMask]
    ldr   r3, [r0]
    ldr   r2, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]

    mov   r3, r3, lsl r1
    subs  r2, r2, r1

@   str   r3, [r0, #tagCInputBitStream_WMV_m_uBitMask]
    str   r3, [r0]

@   if ((pThis->m_iBitsLeft -= iNumBits) < 0)
    str   r2, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]
    bpl   gBS_End2

@   U8_WMV *p = pThis->m_pCurr@
    ldr   r1, [r0, #tagCInputBitStream_WMV_m_pCurr]

@   if (p < pThis->m_pLast)
    ldr   r2, [r0, #tagCInputBitStream_WMV_m_pLast]
    cmp   r1, r2
    bcc   g_BSUpdate2
    bne   g_BSGetData2

g_BSLastBit2:
@   pThis->m_uBitMask += p[0] << (8 - pThis->m_iBitsLeft)@
    ldrb  r2, [r1]
    ldr   r3, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]

@   p += 1@
    add   r12,r1, #1
@   ldr   lr, [r0, #tagCInputBitStream_WMV_m_uBitMask]
    ldr   lr, [r0]
    str   r12,[r0, #tagCInputBitStream_WMV_m_pCurr]

    rsb   r1, r3, #8
    add   r2, lr, r2, lsl r1
@   str   r2, [r0, #tagCInputBitStream_WMV_m_uBitMask]
    str   r2, [r0]

@   pThis->m_iBitsLeft += 8@
    add   r3, r3, #8
    str   r3, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]
    b     gBS_End2

g_BSGetData2:
@   if(pThis->m_bNotEndOfFrame == TRUE_WMV)
    ldr   r3, [r0, #tagCInputBitStream_WMV_m_bNotEndOfFrame]
    cmp   r3, #1
    bne   g_BSError2

    stmfd sp!, {r4,r5}       @ save r4
    mov   r4, r0

@   BS_GetMoreData(pThis)@
    bl    _BS_GetMoreData

@   p = pThis->m_pCurr@
    ldr   r1, [r4, #tagCInputBitStream_WMV_m_pCurr]
    ldr   r2, [r4, #tagCInputBitStream_WMV_m_pLast]

    mov   r0, r4
    ldmfd sp!, {r4,r5}     @ restore r4

    cmp   r2, r1
    beq   g_BSLastBit2
    bcc   g_BSSetError2

g_BSUpdate2:

@   pThis->m_uBitMask += ((p[0] << 8) + p[1]) << (-pThis->m_iBitsLeft)@
    ldrb  r2, [r1, #1]
    ldrb  r3, [r1]

@   p += 2@
    add   r1, r1, #2
    str   r1, [r0, #tagCInputBitStream_WMV_m_pCurr]
    add   r3, r2, r3, lsl #8

    ldr   r1, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]
@   ldr   lr, [r0, #tagCInputBitStream_WMV_m_uBitMask]
    ldr   lr, [r0]
    rsb   r2, r1, #0
    add   r3, lr, r3, lsl r2
@   str   r3, [r0, #tagCInputBitStream_WMV_m_uBitMask]
    str   r3, [r0]

@   pThis->m_iBitsLeft += 16@
    add   r3, r1, #16
    str   r3, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]
    b     gBS_End2

g_BSError2:

@   else if (pThis->m_iBitsLeft < -16)
    ldr   r3, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]
    mvn   r2, #15      @ -16
    cmp   r3, r2
    bge   gBS_End2

g_BSSetError2:
@   if (pThis->m_iStatus == 0)
    ldr   r3, [r0, #tagCInputBitStream_WMV_m_iStatus]
    cmp   r3, #0

@   pThis->m_iStatus = 2@
    moveq r3, #2
    streq r3, [r0, #tagCInputBitStream_WMV_m_iStatus]

@   pThis->m_iBitsLeft = 127@
    mov   r3, #0x7F  @ 0x7F = 127
    str   r3, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]

gBS_End2:

   ldmfd  sp!, {r4,PC}
@    ldr    PC, [sp], #4
    WMV_ENTRY_END
    
    
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    
    WMV_LEAF_ENTRY getHuffman
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ r0 = bs
@ r1 = pDecodeTable
@ r2 = iRootBits

@ r4 = bs.m_uBitMask
@ r5 = iSymbol

    stmfd sp!, {r4 - r6, r14}

    FRAME_PROFILE_COUNT

@   iSymbol = pDecodeTable[BS_peek16(bs, iRootBits)]@
@   pThis->m_uBitMask >> (32 - iNumBits)
    ldr   r4, [r0]
    rsb   r3, r2, #0x20  @ 0x20 = 32
    mov   r3, r4, lsr r3

    add   r3, r1, r3, lsl #1
    ldrsh r3, [r3]

@   if (iSymbol >= 0)
    movs  r5, r3
    bmi   gOverTable2

@   BS_flush16(bs, (iSymbol & ((1 << HUFFMAN_DECODE_ROOT_BITS_LOG) - 1)))@
@   iSymbol >>= HUFFMAN_DECODE_ROOT_BITS_LOG@

    and   r3, r5, #0xF   @ 0xF = 15 

@   pThis->m_uBitMask <<= iNumBits@
    mov   r4, r4, lsl r3
    ldr   r2, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]
    str   r4, [r0]

@   if ((pThis->m_iBitsLeft -= iNumBits) < 0)
    subs  r2, r2, r3
    str   r2, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]

    bpl   gNoFlush2

@   U8_WMV *p = pThis->m_pCurr@
    ldr   r1, [r0, #tagCInputBitStream_WMV_m_pCurr]

@   if (p < pThis->m_pLast)
    ldr   r2, [r0, #tagCInputBitStream_WMV_m_pLast]
    cmp   r1, r2
    bcc   gUpdate2
    bne   gGetData2

gLastBit2:
@   pThis->m_uBitMask += p[0] << (8 - pThis->m_iBitsLeft)@
    ldrb  r2, [r1]
    ldr   r3, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]

@   p += 1@
    add   r1, r1, #1
    ldr   lr, [r0]
    str   r1, [r0, #tagCInputBitStream_WMV_m_pCurr]

    rsb   r1, r3, #8
    add   r2, lr, r2, lsl r1
    str   r2, [r0]

@   pThis->m_iBitsLeft += 8@
    add   r3, r3, #8
    str   r3, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]
    b     gNoFlush2

gGetData2:
    mov   r4, r0

@   if(pThis->m_bNotEndOfFrame == TRUE_WMV)
    ldr   r3, [r0, #tagCInputBitStream_WMV_m_bNotEndOfFrame]!
    cmp   r3, #1
    bne   gError2

@   BS_GetMoreData(bs)@
    mov   r0, r4
    bl    _BS_GetMoreData

    ldr   r1, [r4, #tagCInputBitStream_WMV_m_pCurr]
    ldr   r2, [r4, #tagCInputBitStream_WMV_m_pLast]

    mov   r0, r4
    cmp   r2, r1
    beq   gLastBit2
    bcc   gSetError2

gUpdate2:
@   pThis->m_uBitMask += ((p[0] << 8) + p[1]) << (-pThis->m_iBitsLeft)@
    ldrb  r2, [r1, #1]
    ldrb  r3, [r1]

@   p += 2@
    add   r1, r1, #2
    add   r3, r2, r3, lsl #8
    str   r1, [r0, #tagCInputBitStream_WMV_m_pCurr]

    ldr   r12,[r0, #tagCInputBitStream_WMV_m_iBitsLeft]
    ldr   lr, [r0]
    rsb   r2, r12,#0
    add   r3, lr, r3, lsl r2
    add   r12,r12, #16
    str   r3, [r0]

@   pThis->m_iBitsLeft += 16@
    str   r12,[r0, #tagCInputBitStream_WMV_m_iBitsLeft]
    b     gNoFlush2

gError2:
@   else if (pThis->m_iBitsLeft < -16)
    ldr   r3, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]
    mvn   r2, #15      @ -16
    cmp   r3, r2
    bge   gNoFlush2

@   if (pThis->m_iStatus == 0)
gSetError2:
    ldr   r3, [r0, #tagCInputBitStream_WMV_m_iStatus]
    cmp   r3, #0

@   pThis->m_iStatus = 2@
    moveq r3, #2
    streq r3, [r0, #tagCInputBitStream_WMV_m_iStatus]

@   pThis->m_iBitsLeft = 127@
    mov   r3, #0x7F  @ 0x7F = 127
    str   r3, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]

gNoFlush2:
    mov   r0, r5, asr #4
    b     gEnd2

gOverTable2:
@   BS_flush16(bs, iRootBits)@

    stmfd sp!, {r6,r7}

    mov   r4, r0
    mov   r6, r1
    mov   r1, r2
    bl    _BS_flush16

@   do
@   {
@      iSymbol += BS_peekBit(bs)@
@      BS_flush16(bs, 1)@
@      iSymbol = pDecodeTable[iSymbol + 0x8000]@
@   }
@   while (iSymbol < 0)@

gOverLoop2:
    ldr   r2, [r4]
    mov   r0, r4
    mov   r1, #1
    add   r5, r5, r2, lsr #31
    bl    _BS_flush16

    add   r3, r6, #0x10000
    add   r3, r3, r5, lsl #1
    ldrsh r3, [r3]
    movs  r5, r3

    bmi   gOverLoop2

    ldmfd sp!, {r6,r7}
    mov   r0, r5

gEnd2:

    ldmfd sp!, {r4 - r6, PC}
    WMV_ENTRY_END

    .endif @WMV_OPT_HUFFMAN_GET_ARM

    @@.end
