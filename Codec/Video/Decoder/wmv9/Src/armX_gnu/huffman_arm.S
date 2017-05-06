@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************
    #include "../c/voWMVDecID.h"
    .include "wmvdec_member_arm.inc"
    .include "xplatform_arm_asm.h" 

    .if WMV_OPT_HUFFMAN_GET_ARM == 1

@	AREA	|.rdata|, DATA, READONLY
	.section .rdata	

    @IMPORT BS_GetMoreData
    .extern BS_GetMoreData
    .global BS_flush16_2
    .global BS_flush16
    .global getHuffman	
    
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  
@PRESERVE8
	@AREA	|.text|, CODE, READONLY
	.section .text


    WMV_LEAF_ENTRY BS_flush16_2
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ r0 = pThis

    STMFD  sp!, {r4, lr}
    FRAME_PROFILE_COUNT


@   if (pThis->m_pCurr == pThis->m_pLast)
    LDR    r2, [r0, #tagCInputBitStream_WMV_m_pCurr]  @ pThis->m_pCurr
    LDR    r3, [r0, #tagCInputBitStream_WMV_m_pLast]  @ pThis->m_pLast
    MOV    r4, r0

    CMP    r2, r3
    BNE    gBS_GetData_2

@   pThis->m_uBitMask += pThis->m_pCurr[0] << (8 - pThis->m_iBitsLeft)@
    LDR    r1, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]
    LDRB   r12,[r2]
    LDR    lr, [r0]
    RSB    r3, r1, #8
    ADD    r3, lr, r12, lsl r3
    STR    r3, [r0]

@   pThis->m_pCurr += 1@
    ADD    r3, r2, #1
    STR    r3, [r0, #tagCInputBitStream_WMV_m_pCurr]

@   pThis->m_iBitsLeft += 8@
    ADD    r3, r1, #8
    STR    r3, [r4, #tagCInputBitStream_WMV_m_iBitsLeft]
    B      gBS_Return_2

gBS_GetData_2:
@   if(pThis->m_bNotEndOfFrame == TRUE_WMV)
    LDR    r3, [r0, #tagCInputBitStream_WMV_m_bNotEndOfFrame]!
    CMP    r3, #1
    BNE    gBS_Error_2

@   BS_GetMoreData(pThis)@
    MOV    r0, r4
    BL     BS_GetMoreData

    MOV    r1, #1
    B      gBS_End_2

gBS_Error_2:
@   else if (pThis->m_iBitsLeft < -16)
    LDR    r3, [r4, #tagCInputBitStream_WMV_m_iBitsLeft]
    MVN    r2, #0xF  @ -16
    CMP    r3, r2
    BGE    gBS_Return_3

@   if (pThis->m_iStatus == 0)
    LDR    r3, [r4, #tagCInputBitStream_WMV_m_iStatus]
    CMP    r3, #0

@   pThis->m_iStatus = 2@
    MOVEQ  r3, #2
    STREQ  r3, [r4, #tagCInputBitStream_WMV_m_iStatus]

@   pThis->m_iBitsLeft = 127@
    MOV    r3, #127

gBS_Return_2:
    STR    r3, [r4, #tagCInputBitStream_WMV_m_iBitsLeft]

gBS_Return_3:
    MOV    r1, #0

gBS_End_2:
    MOV    r0, r4

    LDMFD  sp!, {r4, pc}
    WMV_ENTRY_END    
    
    
    WMV_LEAF_ENTRY BS_flush16
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

   STMFD sp!, {r4,lr}
@    STR   lr,  [sp, #-4]!

    FRAME_PROFILE_COUNT

@   pThis->m_uBitMask <<= iNumBits@
@   LDR   r3, [r0, #tagCInputBitStream_WMV_m_uBitMask]
    LDR   r3, [r0]
    LDR   r2, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]

    MOV   r3, r3, LSL r1
    SUBS  r2, r2, r1

@   STR   r3, [r0, #tagCInputBitStream_WMV_m_uBitMask]
    STR   r3, [r0]

@   if ((pThis->m_iBitsLeft -= iNumBits) < 0)
    STR   r2, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]
    BPL   gBS_End2

@   U8_WMV *p = pThis->m_pCurr@
    LDR   r1, [r0, #tagCInputBitStream_WMV_m_pCurr]

@   if (p < pThis->m_pLast)
    LDR   r2, [r0, #tagCInputBitStream_WMV_m_pLast]
    CMP   r1, r2
    BCC   g_BSUpdate2
    BNE   g_BSGetData2

g_BSLastBit2:
@   pThis->m_uBitMask += p[0] << (8 - pThis->m_iBitsLeft)@
    LDRB  r2, [r1]
    LDR   r3, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]

@   p += 1@
    ADD   r12,r1, #1
@   LDR   lr, [r0, #tagCInputBitStream_WMV_m_uBitMask]
    LDR   lr, [r0]
    STR   r12,[r0, #tagCInputBitStream_WMV_m_pCurr]

    RSB   r1, r3, #8
    ADD   r2, lr, r2, LSL r1
@   STR   r2, [r0, #tagCInputBitStream_WMV_m_uBitMask]
    STR   r2, [r0]

@   pThis->m_iBitsLeft += 8@
    ADD   r3, r3, #8
    STR   r3, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]
    B     gBS_End2

g_BSGetData2:
@   if(pThis->m_bNotEndOfFrame == TRUE_WMV)
    LDR   r3, [r0, #tagCInputBitStream_WMV_m_bNotEndOfFrame]
    CMP   r3, #1
    BNE   g_BSError2

    STMFD sp!, {r4,r5}       @ save r4
    MOV   r4, r0

@   BS_GetMoreData(pThis)@
    BL    BS_GetMoreData

@   p = pThis->m_pCurr@
    LDR   r1, [r4, #tagCInputBitStream_WMV_m_pCurr]
    LDR   r2, [r4, #tagCInputBitStream_WMV_m_pLast]

    MOV   r0, r4
    LDMFD sp!, {r4,r5}     @ restore r4

    CMP   r2, r1
    BEQ   g_BSLastBit2
    BCC   g_BSSetError2

g_BSUpdate2:

@   pThis->m_uBitMask += ((p[0] << 8) + p[1]) << (-pThis->m_iBitsLeft)@
    LDRB  r2, [r1, #1]
    LDRB  r3, [r1]

@   p += 2@
    ADD   r1, r1, #2
    STR   r1, [r0, #tagCInputBitStream_WMV_m_pCurr]
    ADD   r3, r2, r3, LSL #8

    LDR   r1, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]
@   LDR   lr, [r0, #tagCInputBitStream_WMV_m_uBitMask]
    LDR   lr, [r0]
    RSB   r2, r1, #0
    ADD   r3, lr, r3, LSL r2
@   STR   r3, [r0, #tagCInputBitStream_WMV_m_uBitMask]
    STR   r3, [r0]

@   pThis->m_iBitsLeft += 16@
    ADD   r3, r1, #16
    STR   r3, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]
    B     gBS_End2

g_BSError2:

@   else if (pThis->m_iBitsLeft < -16)
    LDR   r3, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]
    MVN   r2, #15      @ -16
    CMP   r3, r2
    BGE   gBS_End2

g_BSSetError2:
@   if (pThis->m_iStatus == 0)
    LDR   r3, [r0, #tagCInputBitStream_WMV_m_iStatus]
    CMP   r3, #0

@   pThis->m_iStatus = 2@
    MOVEQ r3, #2
    STREQ r3, [r0, #tagCInputBitStream_WMV_m_iStatus]

@   pThis->m_iBitsLeft = 127@
    MOV   r3, #0x7F  @ 0x7F = 127
    STR   r3, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]

gBS_End2:

   LDMFD  sp!, {r4,PC}
@    LDR    PC, [sp], #4
    WMV_ENTRY_END
    
    
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    
    WMV_LEAF_ENTRY getHuffman
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ r0 = bs
@ r1 = pDecodeTable
@ r2 = iRootBits

@ r4 = bs.m_uBitMask
@ r5 = iSymbol

    STMFD sp!, {r4 - r6, r14}

    FRAME_PROFILE_COUNT

@   iSymbol = pDecodeTable[BS_peek16(bs, iRootBits)]@
@   pThis->m_uBitMask >> (32 - iNumBits)
    LDR   r4, [r0]
    RSB   r3, r2, #0x20  @ 0x20 = 32
    MOV   r3, r4, LSR r3

    ADD   r3, r1, r3, LSL #1
    LDRSH r3, [r3]

@   if (iSymbol >= 0)
    MOVS  r5, r3
    BMI   gOverTable2

@   BS_flush16(bs, (iSymbol & ((1 << HUFFMAN_DECODE_ROOT_BITS_LOG) - 1)))@
@   iSymbol >>= HUFFMAN_DECODE_ROOT_BITS_LOG@

    AND   r3, r5, #0xF   @ 0xF = 15 

@   pThis->m_uBitMask <<= iNumBits@
    MOV   r4, r4, LSL r3
    LDR   r2, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]
    STR   r4, [r0]

@   if ((pThis->m_iBitsLeft -= iNumBits) < 0)
    SUBS  r2, r2, r3
    STR   r2, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]

    BPL   gNoFlush2

@   U8_WMV *p = pThis->m_pCurr@
    LDR   r1, [r0, #tagCInputBitStream_WMV_m_pCurr]

@   if (p < pThis->m_pLast)
    LDR   r2, [r0, #tagCInputBitStream_WMV_m_pLast]
    CMP   r1, r2
    BCC   gUpdate2
    BNE   gGetData2

gLastBit2:
@   pThis->m_uBitMask += p[0] << (8 - pThis->m_iBitsLeft)@
    LDRB  r2, [r1]
    LDR   r3, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]

@   p += 1@
    ADD   r1, r1, #1
    LDR   lr, [r0]
    STR   r1, [r0, #tagCInputBitStream_WMV_m_pCurr]

    RSB   r1, r3, #8
    ADD   r2, lr, r2, LSL r1
    STR   r2, [r0]

@   pThis->m_iBitsLeft += 8@
    ADD   r3, r3, #8
    STR   r3, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]
    B     gNoFlush2

gGetData2:
    MOV   r4, r0

@   if(pThis->m_bNotEndOfFrame == TRUE_WMV)
    LDR   r3, [r0, #tagCInputBitStream_WMV_m_bNotEndOfFrame]!
    CMP   r3, #1
    BNE   gError2

@   BS_GetMoreData(bs)@
    MOV   r0, r4
    BL    BS_GetMoreData

    LDR   r1, [r4, #tagCInputBitStream_WMV_m_pCurr]
    LDR   r2, [r4, #tagCInputBitStream_WMV_m_pLast]

    MOV   r0, r4
    CMP   r2, r1
    BEQ   gLastBit2
    BCC   gSetError2

gUpdate2:
@   pThis->m_uBitMask += ((p[0] << 8) + p[1]) << (-pThis->m_iBitsLeft)@
    LDRB  r2, [r1, #1]
    LDRB  r3, [r1]

@   p += 2@
    ADD   r1, r1, #2
    ADD   r3, r2, r3, LSL #8
    STR   r1, [r0, #tagCInputBitStream_WMV_m_pCurr]

    LDR   r12,[r0, #tagCInputBitStream_WMV_m_iBitsLeft]
    LDR   lr, [r0]
    RSB   r2, r12,#0
    ADD   r3, lr, r3, LSL r2
    ADD   r12,r12, #16
    STR   r3, [r0]

@   pThis->m_iBitsLeft += 16@
    STR   r12,[r0, #tagCInputBitStream_WMV_m_iBitsLeft]
    B     gNoFlush2

gError2:
@   else if (pThis->m_iBitsLeft < -16)
    LDR   r3, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]
    MVN   r2, #15      @ -16
    CMP   r3, r2
    BGE   gNoFlush2

@   if (pThis->m_iStatus == 0)
gSetError2:
    LDR   r3, [r0, #tagCInputBitStream_WMV_m_iStatus]
    CMP   r3, #0

@   pThis->m_iStatus = 2@
    MOVEQ r3, #2
    STREQ r3, [r0, #tagCInputBitStream_WMV_m_iStatus]

@   pThis->m_iBitsLeft = 127@
    MOV   r3, #0x7F  @ 0x7F = 127
    STR   r3, [r0, #tagCInputBitStream_WMV_m_iBitsLeft]

gNoFlush2:
    MOV   r0, r5, ASR #4
    B     gEnd2

gOverTable2:
@   BS_flush16(bs, iRootBits)@

    STMFD sp!, {r6,r7}

    MOV   r4, r0
    MOV   r6, r1
    MOV   r1, r2
    BL    BS_flush16

@   do
@   {
@      iSymbol += BS_peekBit(bs)@
@      BS_flush16(bs, 1)@
@      iSymbol = pDecodeTable[iSymbol + 0x8000]@
@   }
@   while (iSymbol < 0)@

gOverLoop2:
    LDR   r2, [r4]
    MOV   r0, r4
    MOV   r1, #1
    ADD   r5, r5, r2, LSR #31
    BL    BS_flush16

    ADD   r3, r6, #0x10000
    ADD   r3, r3, r5, LSL #1
    LDRSH r3, [r3]
    MOVS  r5, r3

    BMI   gOverLoop2

    LDMFD sp!, {r6,r7}
    MOV   r0, r5

gEnd2:

    LDMFD sp!, {r4 - r6, PC}
    WMV_ENTRY_END

    .endif @WMV_OPT_HUFFMAN_GET_ARM

    .END
