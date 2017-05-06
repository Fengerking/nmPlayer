    #include "../c/voWMVDecID.h"
    .include "wmvdec_member_arm.inc"
    .include "xplatform_arm_asm.h" 
    @.if UNDER_CE != 0
    @.include kxarm.h
    @.endif
	@AREA	.rdata, DATA, READONLY    
	.section .text
    .extern  BS_flush16_2
    .extern  BS_flush16
    .extern  DecodeInverseInterBlockQuantize16_level2_EMB 
	.global	DecodeInverseInterBlockQuantize16_EMB_Naked

@PRESERVE8
	@AREA	.text, CODE, READONLY
	
    WMV_LEAF_ENTRY DecodeInverseInterBlockQuantize16_EMB_Naked
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ r0 == pPMainLoop 
@ r1 == ppInterDCTTableInfo_Dec
@ r2 == XFormType
@ r3 == pDQ

@ r4  == signBit
@ r5  == lIndex
@ r6  == uiNumCoefs
@ r7  == rgiCoefRecn
@ r8  == iDCTHorzFlags
@ r9  == uiCoefCounter
@ r10 == uiTCOEF_ESCAPE
@ r11 == uiStartIndxOfLastRun

.equ ST_iStepMinusStepIsEven  , 32
.equ ST_iDoubleStepSize       , 28
.equ ST_hufDCTACDec           , 24
.equ ST_pPMainLoop            , 20
.equ ST_pZigzagInv            , 16
.equ ST_XFormType             , 12
.equ ST_InterDCTTableInfo_Dec , 8
.equ ST_rgLevelRunAtIndx      , 4
.equ ST_value                 , 0
.equ ST_SIZE                  , 40

 STMFD sp!, {r4, lr}
@ STMFD sp!, {lr}
@  STR   lr,  [sp, #-4]!

  FRAME_PROFILE_COUNT
  SUB   sp, sp, #ST_SIZE

  LDR   r4, [r0, #tagEMB_PBMainLoop_m_pbitstrmIn]

  CMP   r2, #0  
  LDREQ r5, [r0, #tagEMB_PBMainLoop_m_pZigzagScanOrder]
  BEQ   gStoreZigzag

  SUB   r6, r2, #1
  ADD   r5, r0, #tagEMB_PBMainLoop_m_pZigzag
  
  @ADD   r6, r5, r6, LSL #2
  LDR   r5, [r5, +r6, LSL #2] @[r6]

gStoreZigzag:

  LDR   r11,[r3]
  LDR   r12,[r3, #4]

  STR   r5, [sp, #ST_pZigzagInv]

@ STR   r2, [sp, #ST_pZigzagInv]

@ STR   r4, [sp, #ST_m_pbitstrmIn]

@ I32_WMV iDoubleStepSize == pDQ->iDoubleStepSize@
@ I32_WMV iStepMinusStepIsEven == pDQ->iStepMinusStepIsEven@
@ LDR   r11,[lr]
@ LDR   r12,[lr, #4]
 
@ CDCTTableInfo_Dec* InterDCTTableInfo_Dec == ppInterDCTTableInfo_Dec[0]@
  LDR   lr, [r1]

@ U32_WMV uiCoefCounter == 0@
  MOV   r9, #0
  
  STR   r11,[sp, #ST_iDoubleStepSize]
  STR   lr, [sp, #ST_InterDCTTableInfo_Dec] 

@ STR   r12,[sp, #ST_iStepMinusStepIsEven]

@ Huffman_WMV* hufDCTACDec == InterDCTTableInfo_Dec -> hufDCTACDec@
  LDR   r5, [lr]
  STR   r12,[sp, #ST_iStepMinusStepIsEven]
  STR   r0, [sp, #ST_pPMainLoop]
  STR   r5, [sp, #ST_hufDCTACDec]  

@ I16_WMV * rgLevelRunAtIndx == InterDCTTableInfo_Dec -> combined_levelrun@
  LDR   r1, [lr, #_CDCTTableInfo_Dec_combined_levelrun]
  LDR   r5, [lr, #_CDCTTableInfo_Dec_iStartIndxOfLastRunMinus1]
  STR   r2, [sp, #ST_XFormType]
  STR   r1, [sp, #ST_rgLevelRunAtIndx]

@ I32_WMV uiStartIndxOfLastRun == InterDCTTableInfo_Dec -> iStartIndxOfLastRunMinus1 + 1@
  ADD   r11,r5, #1
@ STR   r2, [sp, #ST_uiStartIndxOfLastRun]

@ LDR   r12, =g_iNumCoefTbl
  ADD   r12, r2, #tagEMB_PBMainLoop_m_iNumCoefTbl

@ I32_WMV  uiTCOEF_ESCAPE == InterDCTTableInfo_Dec -> iTcoef_ESCAPE@
  LDR   r10, [lr, #_CDCTTableInfo_Dec_iTcoef_ESCAPE]

@ U32_WMV uiNumCoefs == g_iNumCoefTbl[XFormType]@
@ LDRB  r6, [r12, r3]
  LDRB  r6, [r0, r12]

@ I16_WMV *rgiCoefRecon == (I16_WMV*)pWMVDec->m_rgiCoefRecon
  LDR   r7, [r0, #tagEMB_PBMainLoop_m_rgiCoefReconBuf]

@ g_ResetCoefBuffer[XFormType] ((I32_WMV *)rgiCoefRecon)
@  LDR   r12,=g_ResetCoefBuffer         @ r12 address of g_ResetCoefBuffer
@  MOV   r0, r7
@  LDR   r12,[r12, r3, LSL #2]
@  MOV   lr, pc
@  MOV   pc, r12

  RSB   r2, r2, #4
  MOV   r1, #0
  MOV   r2, r2, LSL #5
  MOV   r0, r7
  ADD   pc, pc, r2

  NOP

gReset4x4:
  MOV   r2, #4

g4x4Loop:
  STR   r1, [r0], #4
  SUBS  r2, r2, #1
  STR   r1, [r0], #12
 
  BNE   g4x4Loop
  B     gResetDone
  NOP
  NOP

gResetNull:
  B     gResetDone
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP

gReset4x8:
  MOV   r2, #0
  MOV   r3, #0
  MOV   r12,#0

  STMIA r0!, {r1 - r3, r12}
  STMIA r0!, {r1 - r3, r12}
  STMIA r0!, {r1 - r3, r12}
  STMIA r0!, {r1 - r3, r12}

  B     gResetDone

gReset8x4:
  MOV   r2, #8

g8x4Loop:
  STR   r1, [r0], #4
  SUBS  r2, r2, #1
  STR   r1, [r0], #12
 
  BNE   g8x4Loop

  B     gResetDone
  NOP
  NOP

gReset8x8:
  MOV   r2, #0
  MOV   r3, #0
  MOV   r12,#0

  STMIA r0!, {r1 - r3, r12}
  STMIA r0!, {r1 - r3, r12}
  STMIA r0!, {r1 - r3, r12}
  STMIA r0!, {r1 - r3, r12}
  STMIA r0!, {r1 - r3, r12}
  STMIA r0!, {r1 - r3, r12}
  STMIA r0!, {r1 - r3, r12}
  STMIA r0!, {r1 - r3, r12}

gResetDone:  
@ iDCTHorzFlags == 0@
  MOV   r8, #0
  MOV   r0, r4

gDQLoop:
@ getHuffmanAndPeekbit(pWMVDec->m_pbitstrmIn, hufDCTACDec->m_hufDecTable, hufDCTACDec->m_tableSize, &signBit)
@  LDR   r0, [sp, #ST_m_pbitstrmIn]
  LDR   r3, [sp, #ST_hufDCTACDec]
  LDR   r12,[r0]
  LDRB  r2, [r3, #8]
  LDR   r1, [r3]

@ BL    getHuffmanAndPeekbit

@   iSymbol == pDecodeTable[BS_peek16(bs, iRootBits)]@
@   pThis->m_uBitMask >> (32 - iNumBits)
    
    RSB   r3, r2, #0x20  @ 0x20 == 32
    MOV   r3, r12,LSR r3

    ADD   r3, r1, r3, LSL #1
    LDRSH r3, [r3]

@   if (iSymbol >= 0)
    MOVS  r5, r3
    BMI   gOverTable3

@   BS_flush16(bs, (iSymbol & ((1 << HUFFMAN_DECODE_ROOT_BITS_LOG) - 1)))@
@   iSymbol >>= HUFFMAN_DECODE_ROOT_BITS_LOG@

    AND   r3, r5, #0xF   @ 0xF == 15 

@   pThis->m_uBitMask <<= iNumBits@
    MOV   r12, r12, LSL r3

@   *signBit == ((I32_WMV)(pThis->m_uBitMask) >> 31)@
@   pThis->m_uBitMask <<= 1@
    LDR   r2, [r0, #4]

    MOV   r4, r12, ASR #31
    MOV   r12,r12, LSL #1
    STR   r12,[r0]

@   if ((pThis->m_iBitsLeft -= (iNumBits+1)) < 0)
    
    ADD   r3, r3, #1
    SUBS  r2, r2, r3
    STR   r2, [r0, #4]

    BPL   gNoFlush3

gRestart3:
@   U8_WMV *p == pThis->m_pCurr@
    LDR   r1, [r0, #8]

@   if (p < pThis->m_pLast)
    LDR   r2, [r0, #12]
    CMP   r1, r2
    BCC   gUpdate3

    BL    BS_flush16_2
    CMP   r1, #1
@    LDR   r1, [sp, #ST_pPMainLoop]
@    LDR   r0, [r1, #tagEMB_PBMainLoop_m_pbitstrmIn]
@    LDR   r0, [sp, #ST_m_pbitstrmIn]

    BEQ   gRestart3
    B     gNoFlush3

gUpdate3:
@   pThis->m_uBitMask += ((p[0] << 8) + p[1]) << (-pThis->m_iBitsLeft)@
    LDRB  r2, [r1, #1]
    LDRB  r3, [r1]

@   p += 2@
    ADD   r1, r1, #2
    STR   r1, [r0, #8]
    LDR   r1, [r0, #4]
    ADD   r3, r2, r3, LSL #8
    LDR   lr, [r0]
    RSB   r2, r1, #0

    ADD   r3, lr, r3, LSL r2
    STR   r3, [r0]

@   pThis->m_iBitsLeft += 16@
    ADD   r3, r1, #16
    STR   r3, [r0, #4]

gNoFlush3:

    MOV   r5, r5, ASR #4
    B     gEnd3

gOverTable3:
@   BS_flush16(bs, iRootBits)@

    STMFD sp!, {r11,r12}

    MOV   r4, r0
    MOV   r11,r1
    MOV   r1, r2
    BL    BS_flush16

@   do
@   {
@      iSymbol += BS_peekBit(bs)@
@      BS_flush16(bs, 1)@
@      iSymbol == pDecodeTable[iSymbol + 0x8000]@
@   }
@   while (iSymbol < 0)@

gOverLoop3:
    LDR   r2, [r4]
    MOV   r0, r4
    MOV   r1, #1
    ADD   r5, r5, r2, LSR #31
    BL    BS_flush16

    ADD   r3, r11,#0x10000
    ADD   r3, r3, r5, LSL #1
    LDRSH r3, [r3]
    MOVS  r5, r3

    BMI   gOverLoop3

@   *signBit == ((I32_WMV)(bs->m_uBitMask) >> 31)@
@   BS_flush16(bs, 1)@
    LDR   r3, [r4]
    MOV   r1, #1
    MOV   r0, r4
    MOV   r11,r4
    MOV   r4, r3, ASR #31
    BL    BS_flush16

    MOV   r0, r11
    LDMFD sp!, {r11,r12}

gEnd3:

@ r0 == bDone
@ r1 == uiRun
@ r2 == iLevel
@ r3 == iIndex

@ r4  == signBit
@ r5  == lIndex
@ r6  == uiNumCoefs
@ r7  == rgiCoefRecn
@ r8  == iDCTHorzFlags
@ r9  == uiCoefCounter
@ r10 == uiTCOEF_ESCAPE
@ r11 == iDoubleStepSize

@ uiRun= (U8_WMV)rgLevelRunAtIndx[lIndex]@
@ iLevel= rgLevelRunAtIndx[lIndex] >> 8@
  LDR   lr, [sp, #ST_rgLevelRunAtIndx]
  ADD   lr, lr, r5, LSL #1
  LDRSH r1, [lr]
  LDR   r12,[sp, #ST_pZigzagInv]
  MOV   r2, r1, ASR #8
  AND   r1, r1, #255

@ uiCoefCounter += uiRun@
  ADD   r9, r9, r1

@ iIndex ==  pZigzagInv [uiCoefCounter]
  LDRB  r3, [r12, r9]
  LDR   r12,[sp, #ST_iStepMinusStepIsEven]

@ LDR   r12,[sp, #64] @ pDQ 
  LDR   lr, [sp, #ST_iDoubleStepSize]
  
  CMP   r4, #0

@ value == ((iDoubleStepSize * iLevel + iStepMinusStepIsEven) ^ signBit) - signBit@
  MLA   r1, lr, r2, r12
@ LDR   lr, =g_IDCTShortCutLUT
  LDR   lr, [sp, #ST_pPMainLoop]
@ LDRB  lr, [lr, r3]
  ADD   lr, lr, r3
  LDRB  lr, [lr, #tagEMB_PBMainLoop_m_IDCTShortCutLUT]

  RSBNE r1, r1, #0
  ORR   r8, r8, lr

  STR   r1, [sp, #ST_value]

@ iDCTHorzFlags ==  g_IDCTShortCutLUT[iIndex]@

@ if((iIndex&1)== 0) {
@   *(I32_WMV *)( rgiCoefRecon + iIndex ) += (I32_WMV)value@
@ }
@ else {
@   ( *(I16_WMV *)( rgiCoefRecon + iIndex)) += (I16_WMV)value@
@ }

 .if _EMB_SSIMD32_ == 1
  ANDS  lr, r3, #1
  BNE   gDQAdd16

  MOV   r2, r7
  LDR   r3, [r2, r3, LSL #1]!
  ADD   r3, r3, r1
  STR   r3, [r2]
  B     gDQAddDone

gDQAdd16:
  ADD   r3, r7, r3, LSL #1
  LDRSH lr, [r3]
  MOV   r2, r1, LSL #16
  ADD   r2, lr, r2, ASR #16
  STRH  r2, [r3]

 .else

@  ( *(I16_WMV *)( rgiCoefRecon + iIndex)) == (I16_WMV)value@
  ADD   r3, r7, r3, LSL #1
  LDRSH lr, [r3]
  MOV   r2, r1, LSL #16
  ADD   r2, lr, r2, ASR #16
  STRH  r2, [r3]

 .endif

gDQAddDone:
 

@ bDone == (uiCoefCounter >= uiNumCoefs)@
  CMP   r9, r6

@ uiCoefCounter++@
  ADD   r9, r9, #1

  BGE   gDQOutLoop

@ bDone == ( lIndex >= uiStartIndxOfLastRun)@
  CMP   r5, r11
  BGE   gDQOutLoop

  CMP   r5, r10
  BEQ   gDQOutLoop

@ } while (bDone == 0) @
@  CMP   r0, #0
@  BEQ   gDQLoop
  B     gDQLoop

gDQOutLoop:
@ uiCoefCounter--@
@  LDR   r0, [sp, #ST_m_pbitstrmIn]
  

@ if (BS_invalid(pWMVDec->m_pbitstrmIn)  (uiCoefCounter >= uiNumCoefs)) {
@     return ICERR_ERROR@
@ }
  LDR   r1, [r0, #16]
  SUB   r9, r9, #1
  CMP   r1, #1
  BEQ   gDQError
  
  CMP   r9, r6
  BCS   gDQError

@ if(lIndex != uiTCOEF_ESCAPE) {
@     break@
@ }
  CMP   r5, r10
  BNE   gDQBreak

@ if(XFormType < 8 && (pZigzagInv [uiCoefCounter]&1)== 0) {
@   *(I32_WMV *)( rgiCoefRecon + pZigzagInv [uiCoefCounter&63] ) -= (I32_WMV)value@
@ }

  LDR   r3, [sp, #ST_XFormType]
  LDR   r0, [sp, #ST_pZigzagInv]
  LDR   r1, [sp, #ST_value]

 .if _EMB_SSIMD32_ == 1
  CMP   r3, #8
  BGE   gDQElse

  LDRB  r3, [r0, r9]
  ANDS  r3, r3, #1
  ANDEQ r3, r9, #63
  LDREQB r3, [r0, r3]
  MOVEQ r2, r7
  LDREQ r3, [r2, r3, LSL #1]!
  SUBEQ r3, r3, r1
  STREQ r3, [r2]
  BEQ   gDQGoon
  
@ else {
@   ( *(I16_WMV *)( rgiCoefRecon + pZigzagInv [uiCoefCounter&63] )) -= (I16_WMV)value@
@ }
gDQElse:
  AND   r3, r9, #63
  LDRB  r3, [r0, r3]
  ADD   r2, r7, r3, LSL #1
  MOV   r3, r1, LSL #16
  LDRSH r1, [r2]
  SUB   r3, r1, r3, ASR #16
  STRH  r3, [r2]

 .else
@ ( *(I16_WMV *)( rgiCoefRecon + pZigzagInv [uiCoefCounter&63] )) -= (I16_WMV)value@
  AND   r3, r9, #63
  LDRB  r3, [r0, r3]
  ADD   r2, r7, r3, LSL #1
  MOV   r3, r1, LSL #16
  LDRSH r1, [r2]
  SUB   r3, r1, r3, ASR #16
  STRH  r3, [r2]
 .endif

gDQGoon:
@ uiCoefCounter -= (U8_WMV)rgLevelRunAtIndx[lIndex]@

  LDR   r3, [sp, #ST_pPMainLoop]
  LDR   r2, [sp, #ST_rgLevelRunAtIndx]
  LDR   r0, [r3, #tagEMB_PBMainLoop_m_pWMVDec]
  LDR   r1, [sp, #ST_InterDCTTableInfo_Dec]
  LDRB  r3, [r2, r5, LSL #1]
  MOV   r2, r6
  SUB   sp, sp, #32
  SUB   r9, r9, r3


@ result == DecodeInverseInterBlockQuantize16_level2_EMB(pWMVDec, 
@           InterDCTTableInfo_Dec,
@           uiNumCoefs,
@           &uiCoefCounter,
@           &iLevel,
@           &signBit,
@           &lIndex
@           )@
  
  ADD   r3, sp, #20     @lIndex
  ADD   r12,sp, #24
  STR   r4, [sp, #24]
  STR   r3, [sp, #8]
  STR   r12,[sp, #4]    @signBit

  ADD   r3, sp, #16     @iLevel
  STR   r3, [sp]

  STR   r9, [sp, #12]   
  ADD   r3, sp, #12     @uiCoefCounter
  BL    DecodeInverseInterBlockQuantize16_level2_EMB

  LDR   r9, [sp, #12]
  LDR   r2, [sp, #16]
  LDR   r5, [sp, #20]
  LDR   r4, [sp, #24]

  ADD   sp, sp, #32

@ if(result != ICERR_OK)
@    return result@

  CMP   r0, #0
  BNE   gDQError

@ iIndex ==  pZigzagInv [uiCoefCounter]
  LDR   lr, [sp, #ST_pZigzagInv]
@  LDR   r12,[sp, #ST_iStepMinusStepIsEven]

  LDR   r0, [sp, #ST_iDoubleStepSize]
  LDR   r12,[sp, #ST_iStepMinusStepIsEven]
@ LDR   r12,[sp, #64] @ pDQ
  LDRB  r3, [lr, r9]

  

@ LDR   r0, [r12]
@ LDR   r12,[r12, #4]

  
@ iDCTHorzFlags ==  g_IDCTShortCutLUT[iIndex]@
@ LDR   lr, =g_IDCTShortCutLUT
  LDR   lr, [sp, #ST_pPMainLoop]

  MLA   r2, r0, r2, r12
  ADD   lr, lr, r3
  LDRB  r1, [lr, #tagEMB_PBMainLoop_m_IDCTShortCutLUT]
  
@ value == ((iDoubleStepSize * iLevel + iStepMinusStepIsEven) ^ signBit) - signBit@
  EOR   r0, r2, r4
  SUB   r0, r0, r4

@ LDRB  r1, [lr, r3]
  ORR   r8, r8, r1

@ if((iIndex&1)== 0) {
@   *(I32_WMV *)( rgiCoefRecon + iIndex ) += (I32_WMV)value@
@ }
@ else {
@   ( *(I16_WMV *)( rgiCoefRecon + iIndex)) += (I16_WMV)value@
@ }
  ANDS  lr, r3, #1
  MOVEQ r2, r7
  LDREQ r3, [r2, r3, LSL #1]!
  ADDEQ r3, r3, r0
  STREQ r3, [r2]
  
  ADDNE r3, r7, r3, LSL #1
  LDRNESH lr, [r3]
  MOVNE r2, r0, LSL #16
  ADDNE r2, lr, r2, ASR #16
  STRNEH r2, [r3]

@ LDR   r0, [sp, #ST_uiStartIndxOfLastRun]
  
@ uiCoefCounter++@
  ADD   r9, r9, #1

@ } while (lIndex < uiStartIndxOfLastRun)
  CMP   r5, r11
  BGE   gDQBreak

  LDR   r1, [sp, #ST_pPMainLoop]
  LDR   r0, [r1, #tagEMB_PBMainLoop_m_pbitstrmIn]
  B     gDQLoop

gDQBreak:
@ pWMVDec->m_iDCTHorzFlags == iDCTHorzFlags@
  LDR   r3, [sp, #ST_pPMainLoop]
  MOV   r0, #0
  STR   r8, [r3, #tagEMB_PBMainLoop_m_iDCTHorzFlags]
  
  B     gDQEnd

gDQError:
  LDR   r3, [sp, #ST_pPMainLoop]
  MVN   r0, #0x63

gDQEnd:
  ADD   sp, sp, #ST_SIZE        @ restore stack
 LDMFD sp!, {r4, PC}
@ LDMFD sp!, {PC}
@  LDR   pc,  [sp], #4
    
  WMV_ENTRY_END

  .END

