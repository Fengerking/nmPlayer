    #include "../c/voWMVDecID.h"
    .include "wmvdec_member_arm.h"
    .include "xplatform_arm_asm.h" 
    
    @.if UNDER_CE != 0
    @.include kxarm.h
    @.endif
	@AREA	.rdata, DATA, READONLY    
	 .text
	 .align 4
    .globl  _BS_flush16_2
    .globl  _BS_flush16
    .globl  _DecodeInverseInterBlockQuantize16_level2_EMB 
	.globl	_DecodeInverseInterBlockQuantize16_EMB_Naked

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

.set ST_iStepMinusStepIsEven  , 32
.set ST_iDoubleStepSize       , 28
.set ST_hufDCTACDec           , 24
.set ST_pPMainLoop            , 20
.set ST_pZigzagInv            , 16
.set ST_XFormType             , 12
.set ST_InterDCTTableInfo_Dec , 8
.set ST_rgLevelRunAtIndx      , 4
.set ST_value                 , 0
.set ST_SIZE                  , 40

 stmfd sp!, {r4, lr}
@ stmfd sp!, {lr}
@  str   lr,  [sp, #-4]!

  FRAME_PROFILE_COUNT
  sub   sp, sp, #ST_SIZE

  ldr   r4, [r0, #tagEMB_PBMainLoop_m_pbitstrmIn]

  cmp   r2, #0  
  ldreq r5, [r0, #tagEMB_PBMainLoop_m_pZigzagScanOrder]
  beq   gStoreZigzag

  sub   r6, r2, #1
  add   r5, r0, #tagEMB_PBMainLoop_m_pZigzag
  
  @add   r6, r5, r6, lsl #2
  ldr   r5, [r5, +r6, lsl #2] @[r6]

gStoreZigzag:

  ldr   r11,[r3]
  ldr   r12,[r3, #4]

  str   r5, [sp, #ST_pZigzagInv]

@ str   r2, [sp, #ST_pZigzagInv]

@ str   r4, [sp, #ST_m_pbitstrmIn]

@ I32_WMV iDoubleStepSize == pDQ->iDoubleStepSize@
@ I32_WMV iStepMinusStepIsEven == pDQ->iStepMinusStepIsEven@
@ ldr   r11,[lr]
@ ldr   r12,[lr, #4]
 
@ CDCTTableInfo_Dec* InterDCTTableInfo_Dec == ppInterDCTTableInfo_Dec[0]@
  ldr   lr, [r1]

@ U32_WMV uiCoefCounter == 0@
  mov   r9, #0
  
  str   r11,[sp, #ST_iDoubleStepSize]
  str   lr, [sp, #ST_InterDCTTableInfo_Dec] 

@ str   r12,[sp, #ST_iStepMinusStepIsEven]

@ Huffman_WMV* hufDCTACDec == InterDCTTableInfo_Dec -> hufDCTACDec@
  ldr   r5, [lr]
  str   r12,[sp, #ST_iStepMinusStepIsEven]
  str   r0, [sp, #ST_pPMainLoop]
  str   r5, [sp, #ST_hufDCTACDec]  

@ I16_WMV * rgLevelRunAtIndx == InterDCTTableInfo_Dec -> combined_levelrun@
  ldr   r1, [lr, #_CDCTTableInfo_Dec_combined_levelrun]
  ldr   r5, [lr, #_CDCTTableInfo_Dec_iStartIndxOfLastRunMinus1]
  str   r2, [sp, #ST_XFormType]
  str   r1, [sp, #ST_rgLevelRunAtIndx]

@ I32_WMV uiStartIndxOfLastRun == InterDCTTableInfo_Dec -> iStartIndxOfLastRunMinus1 + 1@
  add   r11,r5, #1
@ str   r2, [sp, #ST_uiStartIndxOfLastRun]

@ ldr   r12, =g_iNumCoefTbl
  add   r12, r2, #tagEMB_PBMainLoop_m_iNumCoefTbl

@ I32_WMV  uiTCOEF_ESCAPE == InterDCTTableInfo_Dec -> iTcoef_ESCAPE@
  ldr   r10, [lr, #_CDCTTableInfo_Dec_iTcoef_ESCAPE]

@ U32_WMV uiNumCoefs == g_iNumCoefTbl[XFormType]@
@ ldrb  r6, [r12, r3]
  ldrb  r6, [r0, r12]

@ I16_WMV *rgiCoefRecon == (I16_WMV*)pWMVDec->m_rgiCoefRecon
  ldr   r7, [r0, #tagEMB_PBMainLoop_m_rgiCoefReconBuf]

@ g_ResetCoefBuffer[XFormType] ((I32_WMV *)rgiCoefRecon)
@  ldr   r12,=g_ResetCoefBuffer         @ r12 address of g_ResetCoefBuffer
@  mov   r0, r7
@  ldr   r12,[r12, r3, lsl #2]
@  mov   lr, pc
@  mov   pc, r12

  rsb   r2, r2, #4
  mov   r1, #0
  mov   r2, r2, lsl #5
  mov   r0, r7
  add   pc, pc, r2

  nop

gReset4x4:
  mov   r2, #4

g4x4Loop:
  str   r1, [r0], #4
  subs  r2, r2, #1
  str   r1, [r0], #12
 
  bne   g4x4Loop
  b     gResetDone
  nop
  nop

gResetNull:
  b     gResetDone
  nop
  nop
  nop
  nop
  nop
  nop
  nop

gReset4x8:
  mov   r2, #0
  mov   r3, #0
  mov   r12,#0

  stmia r0!, {r1 - r3, r12}
  stmia r0!, {r1 - r3, r12}
  stmia r0!, {r1 - r3, r12}
  stmia r0!, {r1 - r3, r12}

  b     gResetDone

gReset8x4:
  mov   r2, #8

g8x4Loop:
  str   r1, [r0], #4
  subs  r2, r2, #1
  str   r1, [r0], #12
 
  bne   g8x4Loop

  b     gResetDone
  nop
  nop

gReset8x8:
  mov   r2, #0
  mov   r3, #0
  mov   r12,#0

  stmia r0!, {r1 - r3, r12}
  stmia r0!, {r1 - r3, r12}
  stmia r0!, {r1 - r3, r12}
  stmia r0!, {r1 - r3, r12}
  stmia r0!, {r1 - r3, r12}
  stmia r0!, {r1 - r3, r12}
  stmia r0!, {r1 - r3, r12}
  stmia r0!, {r1 - r3, r12}

gResetDone:  
@ iDCTHorzFlags == 0@
  mov   r8, #0
  mov   r0, r4

gDQLoop:
@ getHuffmanAndPeekbit(pWMVDec->m_pbitstrmIn, hufDCTACDec->m_hufDecTable, hufDCTACDec->m_tableSize, &signBit)
@  ldr   r0, [sp, #ST_m_pbitstrmIn]
  ldr   r3, [sp, #ST_hufDCTACDec]
  ldr   r12,[r0]
  ldrb  r2, [r3, #8]
  ldr   r1, [r3]

@ bl    _getHuffmanAndPeekbit

@   iSymbol == pDecodeTable[BS_peek16(bs, iRootBits)]@
@   pThis->m_uBitMask >> (32 - iNumBits)
    
    rsb   r3, r2, #0x20  @ 0x20 == 32
    mov   r3, r12,lsr r3

    add   r3, r1, r3, lsl #1
    ldrsh r3, [r3]

@   if (iSymbol >= 0)
    movs  r5, r3
    bmi   gOverTable3

@   BS_flush16(bs, (iSymbol & ((1 << HUFFMAN_DECODE_ROOT_BITS_LOG) - 1)))@
@   iSymbol >>= HUFFMAN_DECODE_ROOT_BITS_LOG@

    and   r3, r5, #0xF   @ 0xF == 15 

@   pThis->m_uBitMask <<= iNumBits@
    mov   r12, r12, lsl r3

@   *signBit == ((I32_WMV)(pThis->m_uBitMask) >> 31)@
@   pThis->m_uBitMask <<= 1@
    ldr   r2, [r0, #4]

    mov   r4, r12, asr #31
    mov   r12,r12, lsl #1
    str   r12,[r0]

@   if ((pThis->m_iBitsLeft -= (iNumBits+1)) < 0)
    
    add   r3, r3, #1
    subs  r2, r2, r3
    str   r2, [r0, #4]

    bpl   gNoFlush3

gRestart3:
@   U8_WMV *p == pThis->m_pCurr@
    ldr   r1, [r0, #8]

@   if (p < pThis->m_pLast)
    ldr   r2, [r0, #12]
    cmp   r1, r2
    bcc   gUpdate3

    bl    _BS_flush16_2
    cmp   r1, #1
@    ldr   r1, [sp, #ST_pPMainLoop]
@    ldr   r0, [r1, #tagEMB_PBMainLoop_m_pbitstrmIn]
@    ldr   r0, [sp, #ST_m_pbitstrmIn]

    beq   gRestart3
    b     gNoFlush3

gUpdate3:
@   pThis->m_uBitMask += ((p[0] << 8) + p[1]) << (-pThis->m_iBitsLeft)@
    ldrb  r2, [r1, #1]
    ldrb  r3, [r1]

@   p += 2@
    add   r1, r1, #2
    str   r1, [r0, #8]
    ldr   r1, [r0, #4]
    add   r3, r2, r3, lsl #8
    ldr   lr, [r0]
    rsb   r2, r1, #0

    add   r3, lr, r3, lsl r2
    str   r3, [r0]

@   pThis->m_iBitsLeft += 16@
    add   r3, r1, #16
    str   r3, [r0, #4]

gNoFlush3:

    mov   r5, r5, asr #4
    b     gEnd3

gOverTable3:
@   BS_flush16(bs, iRootBits)@

    stmfd sp!, {r11,r12}

    mov   r4, r0
    mov   r11,r1
    mov   r1, r2
    bl    _BS_flush16

@   do
@   {
@      iSymbol += BS_peekBit(bs)@
@      BS_flush16(bs, 1)@
@      iSymbol == pDecodeTable[iSymbol + 0x8000]@
@   }
@   while (iSymbol < 0)@

gOverLoop3:
    ldr   r2, [r4]
    mov   r0, r4
    mov   r1, #1
    add   r5, r5, r2, lsr #31
    bl    _BS_flush16

    add   r3, r11,#0x10000
    add   r3, r3, r5, lsl #1
    ldrsh r3, [r3]
    movs  r5, r3

    bmi   gOverLoop3

@   *signBit == ((I32_WMV)(bs->m_uBitMask) >> 31)@
@   BS_flush16(bs, 1)@
    ldr   r3, [r4]
    mov   r1, #1
    mov   r0, r4
    mov   r11,r4
    mov   r4, r3, asr #31
    bl    _BS_flush16

    mov   r0, r11
    ldmfd sp!, {r11,r12}

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
  ldr   lr, [sp, #ST_rgLevelRunAtIndx]
  add   lr, lr, r5, lsl #1
  ldrsh r1, [lr]
  ldr   r12,[sp, #ST_pZigzagInv]
  mov   r2, r1, asr #8
  and   r1, r1, #255

@ uiCoefCounter += uiRun@
  add   r9, r9, r1

@ iIndex ==  pZigzagInv [uiCoefCounter]
  ldrb  r3, [r12, r9]
  ldr   r12,[sp, #ST_iStepMinusStepIsEven]

@ ldr   r12,[sp, #64] @ pDQ 
  ldr   lr, [sp, #ST_iDoubleStepSize]
  
  cmp   r4, #0

@ value == ((iDoubleStepSize * iLevel + iStepMinusStepIsEven) ^ signBit) - signBit@
  mla   r1, lr, r2, r12
@ ldr   lr, =g_IDCTShortCutLUT
  ldr   lr, [sp, #ST_pPMainLoop]
@ ldrb  lr, [lr, r3]
  add   lr, lr, r3
  ldrb  lr, [lr, #tagEMB_PBMainLoop_m_IDCTShortCutLUT]

  rsbne r1, r1, #0
  orr   r8, r8, lr

  str   r1, [sp, #ST_value]

@ iDCTHorzFlags ==  g_IDCTShortCutLUT[iIndex]@

@ if((iIndex&1)== 0) {
@   *(I32_WMV *)( rgiCoefRecon + iIndex ) += (I32_WMV)value@
@ }
@ else {
@   ( *(I16_WMV *)( rgiCoefRecon + iIndex)) += (I16_WMV)value@
@ }

 .if _EMB_SSIMD32_ == 1
  ands  lr, r3, #1
  bne   gDQAdd16

  mov   r2, r7
  ldr   r3, [r2, r3, lsl #1]!
  add   r3, r3, r1
  str   r3, [r2]
  b     gDQAddDone

gDQAdd16:
  add   r3, r7, r3, lsl #1
  ldrsh lr, [r3]
  mov   r2, r1, lsl #16
  add   r2, lr, r2, asr #16
  strh  r2, [r3]

 .else

@  ( *(I16_WMV *)( rgiCoefRecon + iIndex)) == (I16_WMV)value@
  add   r3, r7, r3, lsl #1
  ldrsh lr, [r3]
  mov   r2, r1, lsl #16
  add   r2, lr, r2, asr #16
  strh  r2, [r3]

 .endif

gDQAddDone:
 

@ bDone == (uiCoefCounter >= uiNumCoefs)@
  cmp   r9, r6

@ uiCoefCounter++@
  add   r9, r9, #1

  bge   gDQOutLoop

@ bDone == ( lIndex >= uiStartIndxOfLastRun)@
  cmp   r5, r11
  bge   gDQOutLoop

  cmp   r5, r10
  beq   gDQOutLoop

@ } while (bDone == 0) @
@  cmp   r0, #0
@  beq   gDQLoop
  b     gDQLoop

gDQOutLoop:
@ uiCoefCounter--@
@  ldr   r0, [sp, #ST_m_pbitstrmIn]
  

@ if (BS_invalid(pWMVDec->m_pbitstrmIn)  (uiCoefCounter >= uiNumCoefs)) {
@     return ICERR_ERROR@
@ }
  ldr   r1, [r0, #16]
  sub   r9, r9, #1
  cmp   r1, #1
  beq   gDQError
  
  cmp   r9, r6
  bcs   gDQError

@ if(lIndex != uiTCOEF_ESCAPE) {
@     break@
@ }
  cmp   r5, r10
  bne   gDQBreak

@ if(XFormType < 8 && (pZigzagInv [uiCoefCounter]&1)== 0) {
@   *(I32_WMV *)( rgiCoefRecon + pZigzagInv [uiCoefCounter&63] ) -= (I32_WMV)value@
@ }

  ldr   r3, [sp, #ST_XFormType]
  ldr   r0, [sp, #ST_pZigzagInv]
  ldr   r1, [sp, #ST_value]

 .if _EMB_SSIMD32_ == 1
  cmp   r3, #8
  bge   gDQElse

  ldrb  r3, [r0, r9]
  ands  r3, r3, #1
  andeq r3, r9, #63
  ldreqb r3, [r0, r3]
  moveq r2, r7
  ldreq r3, [r2, r3, lsl #1]!
  subeq r3, r3, r1
  streq r3, [r2]
  beq   gDQGoon
  
@ else {
@   ( *(I16_WMV *)( rgiCoefRecon + pZigzagInv [uiCoefCounter&63] )) -= (I16_WMV)value@
@ }
gDQElse:
  and   r3, r9, #63
  ldrb  r3, [r0, r3]
  add   r2, r7, r3, lsl #1
  mov   r3, r1, lsl #16
  ldrsh r1, [r2]
  sub   r3, r1, r3, asr #16
  strh  r3, [r2]

 .else
@ ( *(I16_WMV *)( rgiCoefRecon + pZigzagInv [uiCoefCounter&63] )) -= (I16_WMV)value@
  and   r3, r9, #63
  ldrb  r3, [r0, r3]
  add   r2, r7, r3, lsl #1
  mov   r3, r1, lsl #16
  ldrsh r1, [r2]
  sub   r3, r1, r3, asr #16
  strh  r3, [r2]
 .endif

gDQGoon:
@ uiCoefCounter -= (U8_WMV)rgLevelRunAtIndx[lIndex]@

  ldr   r3, [sp, #ST_pPMainLoop]
  ldr   r2, [sp, #ST_rgLevelRunAtIndx]
  ldr   r0, [r3, #tagEMB_PBMainLoop_m_pWMVDec]
  ldr   r1, [sp, #ST_InterDCTTableInfo_Dec]
  ldrb  r3, [r2, r5, lsl #1]
  mov   r2, r6
  sub   sp, sp, #32
  sub   r9, r9, r3


@ result == DecodeInverseInterBlockQuantize16_level2_EMB(pWMVDec, 
@           InterDCTTableInfo_Dec,
@           uiNumCoefs,
@           &uiCoefCounter,
@           &iLevel,
@           &signBit,
@           &lIndex
@           )@
  
  add   r3, sp, #20     @lIndex
  add   r12,sp, #24
  str   r4, [sp, #24]
  str   r3, [sp, #8]
  str   r12,[sp, #4]    @signBit

  add   r3, sp, #16     @iLevel
  str   r3, [sp]

  str   r9, [sp, #12]   
  add   r3, sp, #12     @uiCoefCounter
  bl    _DecodeInverseInterBlockQuantize16_level2_EMB

  ldr   r9, [sp, #12]
  ldr   r2, [sp, #16]
  ldr   r5, [sp, #20]
  ldr   r4, [sp, #24]

  add   sp, sp, #32

@ if(result != ICERR_OK)
@    return result@

  cmp   r0, #0
  bne   gDQError

@ iIndex ==  pZigzagInv [uiCoefCounter]
  ldr   lr, [sp, #ST_pZigzagInv]
@  ldr   r12,[sp, #ST_iStepMinusStepIsEven]

  ldr   r0, [sp, #ST_iDoubleStepSize]
  ldr   r12,[sp, #ST_iStepMinusStepIsEven]
@ ldr   r12,[sp, #64] @ pDQ
  ldrb  r3, [lr, r9]

  

@ ldr   r0, [r12]
@ ldr   r12,[r12, #4]

  
@ iDCTHorzFlags ==  g_IDCTShortCutLUT[iIndex]@
@ ldr   lr, =g_IDCTShortCutLUT
  ldr   lr, [sp, #ST_pPMainLoop]

  mla   r2, r0, r2, r12
  add   lr, lr, r3
  ldrb  r1, [lr, #tagEMB_PBMainLoop_m_IDCTShortCutLUT]
  
@ value == ((iDoubleStepSize * iLevel + iStepMinusStepIsEven) ^ signBit) - signBit@
  eor   r0, r2, r4
  sub   r0, r0, r4

@ ldrb  r1, [lr, r3]
  orr   r8, r8, r1

@ if((iIndex&1)== 0) {
@   *(I32_WMV *)( rgiCoefRecon + iIndex ) += (I32_WMV)value@
@ }
@ else {
@   ( *(I16_WMV *)( rgiCoefRecon + iIndex)) += (I16_WMV)value@
@ }
  ands  lr, r3, #1
  moveq r2, r7
  ldreq r3, [r2, r3, lsl #1]!
  addeq r3, r3, r0
  streq r3, [r2]
  
  addne r3, r7, r3, lsl #1
  ldrnesh lr, [r3]
  movne r2, r0, lsl #16
  addne r2, lr, r2, asr #16
  strneh r2, [r3]

@ ldr   r0, [sp, #ST_uiStartIndxOfLastRun]
  
@ uiCoefCounter++@
  add   r9, r9, #1

@ } while (lIndex < uiStartIndxOfLastRun)
  cmp   r5, r11
  bge   gDQBreak

  ldr   r1, [sp, #ST_pPMainLoop]
  ldr   r0, [r1, #tagEMB_PBMainLoop_m_pbitstrmIn]
  b     gDQLoop

gDQBreak:
@ pWMVDec->m_iDCTHorzFlags == iDCTHorzFlags@
  ldr   r3, [sp, #ST_pPMainLoop]
  mov   r0, #0
  str   r8, [r3, #tagEMB_PBMainLoop_m_iDCTHorzFlags]
  
  b     gDQEnd

gDQError:
  ldr   r3, [sp, #ST_pPMainLoop]
  mvn   r0, #0x63

gDQEnd:
  add   sp, sp, #ST_SIZE        @ restore stack
 ldmfd sp!, {r4, PC}
@ ldmfd sp!, {PC}
@  ldr   pc,  [sp], #4
    
  WMV_ENTRY_END

  @@.end

