@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************
@// Module Name:
@//
@//     strmdec_arm.s
@//
@// Abstract:
@// 
@//     ARM Arch-4 specific multiplications
@//
@//      Custom build with 
@//          armasm $(InputDir)\$(InputName).s -o=$(OutDir)\$(InputName).obj 
@//      and
@//          $(OutDir)\$(InputName).obj
@// 
@// Author:
@// 
@//     Jerry He (yamihe) Sep 2, 2003
@//
@// Revision History:
@//
@//     For more information on ARM assembler directives, use
@//        http://msdn.microsoft.com/library/default.asp?url=/library/en-us/wcechp40/html/ccconarmassemblerdirectives.asp
@//*************************************************************************


@  OPT         2       @ disable listing 
  #include "../../../inc/audio/v10/include/voWMADecID.h"
  .include     "kxarm.h"
  .include     "wma_member_arm.inc"
  .include	  "wma_arm_version.h"
@  OPT         1       @ enable listing
  
@  AREA    |.text|, CODE, READONLY
  .text .align 4
  
  .globl	_ibstrmGetMoreData
  .globl	_ibstrmPeekBits_Naked
  
  .if WMA_OPT_STRMDEC_ARM == 1
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


  .globl	_ibstrmGetMoreData

  .globl	_ibstrmGetBits	
  .globl	_ibstrmLookForBits	
  .globl	_ibstrmLoadBits	
  .globl	_ibstrmCountSerial1Bits	
  .globl	_ibstrmPeekBits	  
  .globl	_ibstrmFlushBits	

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.if LINUX_RVDS == 1
  	PRESERVE8
	.endif
@  	AREA    |.text|, CODE
_ibstrmGetBits:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pibstrm
@ r1 = dwNumBits
@ r2 = piResult

@ r4 = pibstrm->m_dwBitsLeft
@ r5 = pibstrm->m_dwDot


  stmfd sp!, {r4 - r5, r12, lr}

@ if (pibstrm->m_dwBitsLeft < dwNumBits)
  ldr   r4, [r0, #CWMAInputBitStream_m_dwBitsLeft]
  ldr   r5, [r0, #CWMAInputBitStream_m_dwDot]
  cmp   r4, r1
  bcs   gOutputBits

@ LOAD_BITS_FROM_DotT@
@ if (pibstrm->m_cBitDotT > 0)
  ldr   r3, [r0, #CWMAInputBitStream_m_cBitDotT]
  cmp   r3, #0
  bls   gPreLoadFromStream

@ I32 cBitMoved = min (32 - pibstrm->m_dwBitsLeft, pibstrm->m_cBitDotT)@
  rsb   r12, r4, #32
  cmp   r12, r3
  movcs r12, r3

@ pibstrm->m_cBitDotT -= cBitMoved@
  sub   r3, r3, r12
  ldr   lr, [r0, #CWMAInputBitStream_m_dwDotT]
  str   r3, [r0, #CWMAInputBitStream_m_cBitDotT]

@ pibstrm->m_dwBitsLeft += cBitMoved@
  add   r4, r4, r12
  mov   r3, lr, LSR r3

@ pibstrm->m_dwDot <<= cBitMoved@
@ pibstrm->m_dwDot |= (pibstrm->m_dwDotT >> pibstrm->m_cBitDotT)@
  orr   r5, r3, r5, LSL r12

@ pibstrm->m_dwDotT &= ((I32) (1 << pibstrm->m_cBitDotT)) - 1@
  ldr   r3, [r0, #CWMAInputBitStream_m_cBitDotT]
  mov   r12, #1
  mov   r12, r12, LSL r3
  sub   r12, r12, #1
  and   lr,  lr,  r12
  str   r5, [r0, #CWMAInputBitStream_m_dwDot]
  str   lr, [r0, #CWMAInputBitStream_m_dwDotT]

gPreLoadFromStream:
  ldr   r3, [r0, #CWMAInputBitStream_m_cbBuflen]
  ldr   lr, [r0, #CWMAInputBitStream_m_pBuffer]

gLoadFromStream:
@ LOAD_BITS_FROM_STREAM@
@ while (pibstrm->m_dwBitsLeft <= 24 && pibstrm->m_cbBuflen > 0)
  cmp   r4, #24
  bhi   gGetMoreData

  cmp   r3, #0
  bls   gGetMoreData

@ --(pibstrm->m_cbBuflen)@
  sub   r3, r3, #1

@ pibstrm->m_dwBitsLeft += 8@
  ldrb  r12, [lr], #1
  add   r4, r4, #8

@ pibstrm->m_dwDot <<= 8@
@ pibstrm->m_dwDot |= *(pibstrm->m_pBuffer)++@
  orr   r5, r12, r5, LSL #8

  b     gLoadFromStream

gGetMoreData:
  str   lr, [r0, #CWMAInputBitStream_m_pBuffer]
  str   r3, [r0, #CWMAInputBitStream_m_cbBuflen]
  str   r5, [r0, #CWMAInputBitStream_m_dwDot]

@ if(pibstrm->m_dwBitsLeft < dwNumBits){
  cmp   r4, r1
  bcs   gOutputBits

  str   r4, [r0, #CWMAInputBitStream_m_dwBitsLeft]

@ TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData(pibstrm, ModeGetFlush, dwNumBits))@
  
  stmfd  sp!, {r0 - r2} @save r0, r1, r2

  mov    r2, r1
  mov    r1, #2
  bl     _ibstrmGetMoreData

  movs   r3, r0
  ldmfd  sp!, {r0 - r2} @restore r0, r1, r2

  movmi  r0, r3
  bmi    gExit


gOutputBits:
@ pibstrm->m_dwBitsLeft -= dwNumBits@
  mvn   lr, #0
  sub   r4, r4, r1
    
@ *piResult = (pibstrm->m_dwDot >> pibstrm->m_dwBitsLeft) & getMask[dwNumBits]@
  mov   r12, r5, LSR r4
  bic   r3, r12, lr, LSL r1
  
@pibstrm->m_cFrmBitCnt += dwNumBits@
	ldr		r5, [r0, #CWMAInputBitStream_m_cFrmBitCnt]
	add		r5, r5, r1
	
	str	r5, [r0, #CWMAInputBitStream_m_cFrmBitCnt]
  str   r4, [r0, #CWMAInputBitStream_m_dwBitsLeft]
  str   r3, [r2]
 
  mov   r0, #0

gExit:
  ldmfd sp!, {r4 - r5, r12, PC} @ibstrmGetBits
  @ENTRY_END ibstrmGetBits

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.if LINUX_RVDS == 1
  	PRESERVE8
	.endif
 @ 	AREA    |.text|, CODE
_ibstrmLookForBits:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pibstrm
@ r1 = dwNumBits

@ r2 = pibstrm->m_dwBitsLeft
@ r3 = pibstrm->m_cBitDotT
@ lr = pibstrm->m_cbBuflen


  str   lr,  [sp, #-4]!

@ if (dwNumBits > pibstrm->m_dwBitsLeft + pibstrm->m_cBitDotT + pibstrm->m_cbBuflen * 8)
  ldr   lr, [r0, #CWMAInputBitStream_m_cbBuflen]
  ldr   r2, [r0, #CWMAInputBitStream_m_dwBitsLeft]
  ldr   r3, [r0, #CWMAInputBitStream_m_cBitDotT]

  add   r12, r2, lr, LSL #3
  add   r12, r12, r3 
  cmp   r1, r12
  movls r0, #0
  bls   gLookBitsExit

  stmfd sp!, {r4, r5}  @save r4, r5

  ldr   r4, [r0, #CWMAInputBitStream_m_pBuffer]
  ldr   r5, [r0, #CWMAInputBitStream_m_dwDot]

gLookBitsLoadFromStream:
@ LOAD_BITS_FROM_STREAM@
@ while (pibstrm->m_dwBitsLeft <= 24 && pibstrm->m_cbBuflen > 0)
  cmp   r2, #24
  bhi   gLoadBitsIntoDotT

  cmp   lr, #0
  bls   gLoadBitsIntoDotT

@ --(pibstrm->m_cbBuflen)@
  sub   lr, lr, #1

@ pibstrm->m_dwBitsLeft += 8@
  ldrb  r12, [r4], #1
  add   r2, r2, #8

@ pibstrm->m_dwDot <<= 8@
@ pibstrm->m_dwDot |= *(pibstrm->m_pBuffer)++@
  orr   r5, r12, r5, LSL #8

  b     gLookBitsLoadFromStream


@ LOAD_BITS_INTO_DotT
gLoadBitsIntoDotT:
@ pibstrm->m_dwDotT = 0@
  str   r5, [r0, #CWMAInputBitStream_m_dwDot]
  mov   r5, #0
   
@ while (pibstrm->m_cbBuflen > 0)
  cmp   lr, #0
  bls   gLookBitsGetMoreData
  
@ --(pibstrm->m_cbBuflen)@
  sub   lr, lr, #1

@ pibstrm->m_cBitDotT += 8@
  ldrb  r12, [r4], #1
  add   r3, r3, #8
     
@ pibstrm->m_dwDotT <<= 8@
@ pibstrm->m_dwDotT |= *(pibstrm->m_pBuffer)++@
  orr   r5, r12, r5, LSL #8

  b     gLoadBitsIntoDotT
  
gLookBitsGetMoreData:
  str   lr, [r0, #CWMAInputBitStream_m_cbBuflen]
  str   r2, [r0, #CWMAInputBitStream_m_dwBitsLeft]
  str   r3, [r0, #CWMAInputBitStream_m_cBitDotT]

  str   r4, [r0, #CWMAInputBitStream_m_pBuffer]
  str   r5, [r0, #CWMAInputBitStream_m_dwDotT]

  ldmfd  sp!, {r4, r5} @restore r4, r5


@ TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData(pibstrm, ModeLookFor, dwNumBits))@
  
  mov    r2, r1
  mov    r1, #0
  bl     _ibstrmGetMoreData

gLookBitsExit:
  ldr    PC, [sp], #4 @ibstrmLookForBits
  @ENTRY_END ibstrmLookForBits

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.if LINUX_RVDS == 1
  	PRESERVE8
	.endif
@  	AREA    |.text|, CODE
_ibstrmLoadBits:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pibs
@ r1 = iFirstBit
@ r2 = uByte

@ r4 = pibstrm->m_cbBuflen
@ r5 = pibs->m_pBuffer


  stmfd sp!, {r4 - r6, lr}

@ r6 = pibs->m_cBitDotT
@ r3 = pibstrm->m_dwBitsLeft

@if ( 0 == iFirstBit ) // ues
@    {
@        uByte = *(pibs->m_pBuffer)@
@    }
	cmp		r1, #0
	bne		NotNeedReadData
	ldr		r5, [r0, #CWMAInputBitStream_m_pBuffer]
	ldrb	r2, [r5]
	
NotNeedReadData:
@ if (pibs->m_cBitDotT == 0 && pibs->m_dwBitsLeft + (8 - iFirstBit) <= 32)
  ldr   r6, [r0, #CWMAInputBitStream_m_cBitDotT]
@  ldr   r5, [r0, #CWMAInputBitStream_m_pBuffer]
@  ldr   r4, [r0, #CWMAInputBitStream_m_cbBuflen]
  cmp   r6, #0
  bne   gLoadBitsElse

  ldr   r3, [r0, #CWMAInputBitStream_m_dwBitsLeft]

  sub   lr, r3, r1
  cmp   lr, #24
  bgt   gLoadBitsElse

  ldr   r6, [r0, #CWMAInputBitStream_m_dwDot]

@gLoadBitsWhile
@ while (pibs->m_cbBuflen > 0 && (pibs->m_dwBitsLeft + (8 - iFirstBit)) <= 32)
@  cmp   r4, #0
@  bls   gLoadBitsWhileEnd

@ temp = (*pibs->m_pBuffer++)@
@  ldrb  r12, [r5], #1
  
@ pibs->m_cbBuflen--@
@  sub   r4, r4, #1

@ temp = temp<<iFirstBit@
@ temp = temp>>iFirstBit@
  mov   r2, r2, LSL r1
  and   r2, r2, #0xFF

  rsb   lr, r1, #8
  mov   r2, r2, LSR r1
  
@ pibs->m_dwDot = (pibs->m_dwDot<<(8 - iFirstBit))|temp@
  orr   r6, r2, r6, LSL lr
  
@ pibs->m_dwBitsLeft += (8 - iFirstBit)@
  add   r3, r3, lr

@ iFirstBit = 0@
  mov   r1, #0

@  cmp   r3, #24
@  BLE   gLoadBitsWhile

@gLoadBitsWhileEnd
  str   r3, [r0, #CWMAInputBitStream_m_dwBitsLeft]
  str   r6, [r0, #CWMAInputBitStream_m_dwDot]
  b     gLoadBitsExit

gLoadBitsElse:
@ temp = (*pibs->m_pBuffer++)@
@  ldrb  r12, [r5], #1
  ldr   r3, [r0, #CWMAInputBitStream_m_dwDotT]

@ pibs->m_cbBuflen--@
@  sub   r4, r4, #1

@ temp = temp<<iFirstBit@
@ temp = temp>>iFirstBit@
  mov   r2, r2, LSL r1
  and   r2, r2, #0xFF

  rsb   lr, r1, #8
  mov   r2, r2, LSR r1

@ pibs->m_dwDotT = (pibs->m_dwDotT<<(8 - iFirstBit))|temp@
  orr   r3, r2, r3, LSL lr

@ pibs->m_cBitDotT += (8 - iFirstBit)@
  add   r6, r6, lr

  str   r3, [r0, #CWMAInputBitStream_m_dwDotT]
  str   r6, [r0, #CWMAInputBitStream_m_cBitDotT]

gLoadBitsExit:
@  str   r5, [r0, #CWMAInputBitStream_m_pBuffer]
@  str   r4, [r0, #CWMAInputBitStream_m_cbBuflen]

  ldmfd sp!, {r4 - r6, PC} @ibstrmLoadBits
 @ ENTRY_END ibstrmLoadBits


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.if LINUX_RVDS == 1
  	PRESERVE8
	.endif
@  	AREA    |.text|, CODE
_ibstrmCountSerial1Bits:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pibstrm
@ r1 = pCount

@ r2 = pibstrm->m_dwBitsLeft
@ r3 = pibstrm->m_dwDot
@ r4 = *pCount

  stmfd sp!, {r4, lr}

@ r2 = pibstrm->m_dwBitsLeft
  ldr   r2, [r0, #CWMAInputBitStream_m_dwBitsLeft]

@ r3 = pibstrm->m_dwDot
  ldr   r3, [r0, #CWMAInputBitStream_m_dwDot]

@ r4 = *pCount
  ldr   r4, [r1]
 
@ if (pibstrm->m_dwBitsLeft <= 0)  
  cmp   r2, #0
  bgt   gCountBits

gReload:
@ pibstrm->m_dwBitsLeft = 0@
  mov   r2, #0

@ LOAD_BITS_FROM_DotT@
@ if (pibstrm->m_cBitDotT > 0)
  ldr   lr, [r0, #CWMAInputBitStream_m_cBitDotT]
  cmp   lr, #0
  bls   gPreLoadFromStream2

@ I32 cBitMoved = min (32 - pibstrm->m_dwBitsLeft, pibstrm->m_cBitDotT)@
@ pibstrm->m_cBitDotT -= cBitMoved@
@ pibstrm->m_dwBitsLeft += cBitMoved@

@ pibstrm->m_dwBitsLeft == 0 -> cBitMoved = pibstrm->m_cBitDotT
@ then pibstrm->m_cBitDotT = 0

  ldr   r12,[r0, #CWMAInputBitStream_m_dwDotT]
  str   r2, [r0, #CWMAInputBitStream_m_cBitDotT]

@ pibstrm->m_dwDot <<= cBitMoved@
@ pibstrm->m_dwDot |= (pibstrm->m_dwDotT >> pibstrm->m_cBitDotT)@
@ pibstrm->m_dwDotT &= ((I32) (1 << pibstrm->m_cBitDotT)) - 1@
@ -> pibstrm->m_dwDot = pibstrm->m_dwDotT@
@ -> pibstrm->m_dwDotT no meaning

  str   r12,[r0, #CWMAInputBitStream_m_dwDot]
  mov   r2, lr
  mov   r3, r12

gPreLoadFromStream2:
  ldr   r12,[r0, #CWMAInputBitStream_m_cbBuflen]
  ldr   lr, [r0, #CWMAInputBitStream_m_pBuffer]

@ save *pCount, release r4
  str   r4, [r1]

gLoadFromStream2:
@ LOAD_BITS_FROM_STREAM@
@ while (pibstrm->m_dwBitsLeft <= 24 && pibstrm->m_cbBuflen > 0)
  cmp   r2, #24
  bhi   gGetMoreData2

  cmp   r12, #0
  bls   gGetMoreData2

@ --(pibstrm->m_cbBuflen)@
  sub   r12, r12, #1

@ pibstrm->m_dwBitsLeft += 8@
  ldrb  r4, [lr], #1
  add   r2, r2, #8

@ pibstrm->m_dwDot <<= 8@
@ pibstrm->m_dwDot |= *(pibstrm->m_pBuffer)++@
  orr   r3, r4, r3, LSL #8

  b     gLoadFromStream2

gGetMoreData2:
  str   lr, [r0, #CWMAInputBitStream_m_pBuffer]
  str   r12,[r0, #CWMAInputBitStream_m_cbBuflen]
  str   r3, [r0, #CWMAInputBitStream_m_dwDot]

@ load *pCount, release r4
  ldr   r4, [r1]

@ if(pibstrm->m_dwBitsLeft < 1){
  cmp   r2, #1
  bcs   gCountBits

  str   r2, [r0, #CWMAInputBitStream_m_dwBitsLeft]

@ TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData(pibstrm, ModeGetFlush, 1))@
  
  stmfd  sp!, {r0 - r1} @save r0, r1

  mov    r2, #1
  mov    r1, #2
  bl     _ibstrmGetMoreData

  movs   lr, r0
  ldmfd  sp!, {r0 - r1} @restore r0, r1

  movmi  r0, lr
  bmi    gExit2

  ldr   r2, [r0, #CWMAInputBitStream_m_dwBitsLeft]
  ldr   r3, [r0, #CWMAInputBitStream_m_dwDot]

gCountBits:
@ dwCurrent = pibstrm->m_dwDot << (32-pibstrm->m_dwBitsLeft)@
  rsb   lr, r2, #33

  movs  r12, r3, LSL lr
  bcc   gWhileEnd

@ dwCount = 1@
  mov   lr, #1
    
@ while (dwCurrent & 0x80000000)
@    dwCount ++@
@    dwCurrent <<= 1@

gWhileCount:
  movs  r12, r12, LSL #1

  addcs lr, lr, #1
  bcs   gWhileCount

@ pibstrm->m_dwBitsLeft -= (dwCount+1)@
  sub   r2, r2, lr

@ *pCount += dwCount@
  add   r4, r4, lr

gWhileEnd:
  subs  r2, r2, #1

@ if ((I32)pibstrm->m_dwBitsLeft < 0)
@	 goto reload@
  blt   gReload

  str   r2, [r0, #CWMAInputBitStream_m_dwBitsLeft]
  str   r4, [r1]
 
  mov   r0, #0

gExit2:
  ldmfd sp!, {r4, PC} @ibstrmCountSerial1Bits
  @ENTRY_END ibstrmCountSerial1Bits 


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.if LINUX_RVDS == 1
  	PRESERVE8
	.endif
@  	AREA    |.text|, CODE
_ibstrmPeekBits:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pibstrm
@ r1 = dwNumBits
@ r2 = piRetBits

@ r4 = pibstrm->m_dwBitsLeft
@ r5 = pibstrm->m_dwDot


  stmfd sp!, {r4 - r5, r12, lr}

@ if (pibstrm->m_dwBitsLeft < dwNumBits)
  ldr   r4, [r0, #CWMAInputBitStream_m_dwBitsLeft]
  ldr   r5, [r0, #CWMAInputBitStream_m_dwDot]
  cmp   r4, r1
  bcs   gPeekBits

@ LOAD_BITS_FROM_DotT@
@ if (pibstrm->m_cBitDotT > 0)
  ldr   r3, [r0, #CWMAInputBitStream_m_cBitDotT]
  cmp   r3, #0
  bls   gPeekBitsPreLoadFromStream

@ I32 cBitMoved = min (32 - pibstrm->m_dwBitsLeft, pibstrm->m_cBitDotT)@
  rsb   r12, r4, #32
  cmp   r12, r3
  movcs r12, r3

@ pibstrm->m_cBitDotT -= cBitMoved@
  sub   r3, r3, r12
  ldr   lr, [r0, #CWMAInputBitStream_m_dwDotT]
  str   r3, [r0, #CWMAInputBitStream_m_cBitDotT]

@ pibstrm->m_dwBitsLeft += cBitMoved@
  add   r4, r4, r12
  mov   r3, lr, LSR r3

@ pibstrm->m_dwDot <<= cBitMoved@
@ pibstrm->m_dwDot |= (pibstrm->m_dwDotT >> pibstrm->m_cBitDotT)@
  orr   r5, r3, r5, LSL r12

@ pibstrm->m_dwDotT &= ((I32) (1 << pibstrm->m_cBitDotT)) - 1@
  ldr   r3, [r0, #CWMAInputBitStream_m_cBitDotT]
  mov   r12, #1
  mov   r12, r12, LSL r3
  sub   r12, r12, #1
  and   lr,  lr,  r12
  str   r5, [r0, #CWMAInputBitStream_m_dwDot]
  str   lr, [r0, #CWMAInputBitStream_m_dwDotT]

gPeekBitsPreLoadFromStream:
  ldr   r3, [r0, #CWMAInputBitStream_m_cbBuflen]
  ldr   lr, [r0, #CWMAInputBitStream_m_pBuffer]

gPeekBitsLoadFromStream:
@ LOAD_BITS_FROM_STREAM@
@ while (pibstrm->m_dwBitsLeft <= 24 && pibstrm->m_cbBuflen > 0)
  cmp   r4, #24
  bhi   gPeekBitsGetMoreData

  cmp   r3, #0
  bls   gPeekBitsGetMoreData

@ --(pibstrm->m_cbBuflen)@
  sub   r3, r3, #1

@ pibstrm->m_dwBitsLeft += 8@
  ldrb  r12, [lr], #1
  add   r4, r4, #8

@ pibstrm->m_dwDot <<= 8@
@ pibstrm->m_dwDot |= *(pibstrm->m_pBuffer)++@
  orr   r5, r12, r5, LSL #8

  b     gPeekBitsLoadFromStream

gPeekBitsGetMoreData:
  str   lr, [r0, #CWMAInputBitStream_m_pBuffer]
  str   r3, [r0, #CWMAInputBitStream_m_cbBuflen]
  str   r5, [r0, #CWMAInputBitStream_m_dwDot]
  str   r4, [r0, #CWMAInputBitStream_m_dwBitsLeft]

@ if(pibstrm->m_dwBitsLeft < dwNumBits){
  cmp   r4, r1
  bcs   gPeekBits

@ TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData(pibstrm, ModePeek, dwNumBits))@
  
  stmfd  sp!, {r0 - r2} @save r0, r1, r2

  mov    r2, r1
  mov    r1, #1
  bl     _ibstrmGetMoreData

  movs   r3, r0
  ldmfd  sp!, {r0 - r2} @restore r0, r1, r2

  movmi  r0, r3
  bmi    gPeekBitsExit


@ if (pibstrm->m_dwBitsLeft < dwNumBits)
@	 dwNumBits = pibstrm->m_dwBitsLeft@
  cmp   r4, r1
  movcc r1, r4

gPeekBits:
@ cBitExtra = (I16) pibstrm->m_dwBitsLeft - (I16) dwNumBits@
  sub   r3, r4, r1
  rsb   lr, r1, #32

@ *piRetBits = (pibstrm->m_dwDot >> cBitExtra) << (32 - dwNumBits)@
  mov   r12, r5, LSR r3
  mov   r1, r12, LSL lr 

  mov   r0, #0
  str   r1, [r2]

gPeekBitsExit:
  ldmfd sp!, {r4 - r5, r12, PC} @ibstrmPeekBits
  @ENTRY_END ibstrmPeekBits


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.if LINUX_RVDS == 1
  	PRESERVE8
	.endif
@  	AREA    |.text|, CODE
_ibstrmFlushBits:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = pibstrm
@ r1 = dwNumBits

@ r4 = pibstrm->m_dwBitsLeft


  stmfd sp!, {r4, r5, lr}

@ if (pibstrm->m_dwBitsLeft < dwNumBits)
  ldr   r4, [r0, #CWMAInputBitStream_m_dwBitsLeft]
  cmp   r4, r1
  bcs   gFlushBits

@ LOAD_BITS_FROM_DotT@

@ r2 = pibstrm->m_dwDot
  ldr   r2, [r0, #CWMAInputBitStream_m_dwDot]

@ if (pibstrm->m_cBitDotT > 0)
  ldr   r3, [r0, #CWMAInputBitStream_m_cBitDotT]
  cmp   r3, #0
  bls   gFlushBitsPreLoadFromStream

@ I32 cBitMoved = min (32 - pibstrm->m_dwBitsLeft, pibstrm->m_cBitDotT)@
  rsb   r12, r4, #32
  cmp   r12, r3
  movcs r12, r3

@ pibstrm->m_cBitDotT -= cBitMoved@
  sub   r3, r3, r12
  ldr   lr, [r0, #CWMAInputBitStream_m_dwDotT]
  str   r3, [r0, #CWMAInputBitStream_m_cBitDotT]

@ pibstrm->m_dwBitsLeft += cBitMoved@
  add   r4, r4, r12
  mov   r5, lr, LSR r3

@ pibstrm->m_dwDot <<= cBitMoved@
@ pibstrm->m_dwDot |= (pibstrm->m_dwDotT >> pibstrm->m_cBitDotT)@
  orr   r2, r5, r2, LSL r12

@ pibstrm->m_dwDotT &= ((I32) (1 << pibstrm->m_cBitDotT)) - 1@
	mov		r5, lr, LSR r3
	eor		lr, lr, r5, LSL r3
	str		lr, [r0, #CWMAInputBitStream_m_dwDotT]
	
gFlushBitsPreLoadFromStream:
  ldr   r3, [r0, #CWMAInputBitStream_m_cbBuflen]
  ldr   lr, [r0, #CWMAInputBitStream_m_pBuffer]

gFlushBitsLoadFromStream:
@ LOAD_BITS_FROM_STREAM@
@ while (pibstrm->m_dwBitsLeft <= 24 && pibstrm->m_cbBuflen > 0)
  cmp   r4, #24
  bhi   gFlushBitsGetMoreData

  cmp   r3, #0
  bls   gFlushBitsGetMoreData

@ --(pibstrm->m_cbBuflen)@
  sub   r3, r3, #1

@ pibstrm->m_dwBitsLeft += 8@
  ldrb  r12, [lr], #1
  add   r4, r4, #8

@ pibstrm->m_dwDot <<= 8@
@ pibstrm->m_dwDot |= *(pibstrm->m_pBuffer)++@
  orr   r2, r12, r2, LSL #8

  b     gFlushBitsLoadFromStream

gFlushBitsGetMoreData:
  str   lr, [r0, #CWMAInputBitStream_m_pBuffer]
  str   r3, [r0, #CWMAInputBitStream_m_cbBuflen]
  str   r2, [r0, #CWMAInputBitStream_m_dwDot]

@ if(pibstrm->m_dwBitsLeft < dwNumBits){
  cmp   r4, r1
  bcs   gFlushBits

  str   r4, [r0, #CWMAInputBitStream_m_dwBitsLeft]

@ TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData(pibstrm, ModeGetFlush, dwNumBits))@
  
  stmfd  sp!, {r0 - r1} @save r0, r1

  mov    r2, r1
  mov    r1, #2
  bl     _ibstrmGetMoreData
  movs   r3, r0
  ldmfd  sp!, {r0 - r1} @restore r0, r1
  movmi  r0, r3
  bmi    gFlushBitsExit

gFlushBits:
@ pibstrm->m_dwBitsLeft -= dwNumBits@
@pibstrm->m_cFrmBitCnt += dwNumBits@
	ldr		r5, [r0, #CWMAInputBitStream_m_cFrmBitCnt]
	sub		r4, r4, r1
	str		r4, [r0, #CWMAInputBitStream_m_dwBitsLeft]  

	add		r5, r5, r1
	str		r5, [r0, #CWMAInputBitStream_m_cFrmBitCnt]
	mov		r0, #0

gFlushBitsExit:
  ldmfd sp!, {r4, r5, PC} @ibstrmFlushBits
  @ENTRY_END ibstrmFlushBits

  .endif @ WMA_OPT_STRMDEC_ARM 
 	  
 @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.if LINUX_RVDS == 1
  	PRESERVE8
	.endif
 @ 	AREA    |.text|, CODE 
_ibstrmPeekBits_Naked:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r1 = pibstrm

@ r3 = iRetBits
@ r4 = pibstrm->m_dwBitsLeft
@ r5 = pibstrm->m_dwDot


  stmfd sp!, {r4 - r5, r12, lr}
@ str   lr,  [sp, #-4]!

@ r2 = dwNumBits
  mov   r2, #23

@ if (pibstrm->m_dwBitsLeft < dwNumBits)
  ldr   r4, [r1, #CWMAInputBitStream_m_dwBitsLeft]
  ldr   r5, [r1, #CWMAInputBitStream_m_dwDot]
  cmp   r4, r2
  bcs   gPeekBitsNaked

@ LOAD_BITS_FROM_DotT@
@ if (pibstrm->m_cBitDotT > 0)
  ldr   r3, [r1, #CWMAInputBitStream_m_cBitDotT]
  cmp   r3, #0
  bls   gPeekBitsPreLoadFromStreamNaked

@ I32 cBitMoved = min (32 - pibstrm->m_dwBitsLeft, pibstrm->m_cBitDotT)@
  rsb   r12, r4, #32
  cmp   r12, r3
  movcs r12, r3

@ pibstrm->m_cBitDotT -= cBitMoved@
  sub   r3, r3, r12
  ldr   lr, [r1, #CWMAInputBitStream_m_dwDotT]
  str   r3, [r1, #CWMAInputBitStream_m_cBitDotT]

@ pibstrm->m_dwBitsLeft += cBitMoved@
  add   r4, r4, r12
  mov   r3, lr, LSR r3

@ pibstrm->m_dwDot <<= cBitMoved@
@ pibstrm->m_dwDot |= (pibstrm->m_dwDotT >> pibstrm->m_cBitDotT)@
  orr   r5, r3, r5, LSL r12

@ pibstrm->m_dwDotT &= ((I32) (1 << pibstrm->m_cBitDotT)) - 1@
  ldr   r3, [r1, #CWMAInputBitStream_m_cBitDotT]
  mov   r12, #1
  mov   r12, r12, LSL r3
  sub   r12, r12, #1
  and   lr,  lr,  r12
  str   r5, [r1, #CWMAInputBitStream_m_dwDot]
  str   lr, [r1, #CWMAInputBitStream_m_dwDotT]

gPeekBitsPreLoadFromStreamNaked:
  ldr   r3, [r1, #CWMAInputBitStream_m_cbBuflen]
  ldr   lr, [r1, #CWMAInputBitStream_m_pBuffer]

gPeekBitsLoadFromStreamNaked:
@ LOAD_BITS_FROM_STREAM@
@ while (pibstrm->m_dwBitsLeft <= 24 && pibstrm->m_cbBuflen > 0)
  cmp   r4, #24
  bhi   gPeekBitsGetMoreDataNaked

  cmp   r3, #0
  bls   gPeekBitsGetMoreDataNaked

@ --(pibstrm->m_cbBuflen)@
  sub   r3, r3, #1

@ pibstrm->m_dwBitsLeft += 8@
  ldrb  r12, [lr], #1
  add   r4, r4, #8

@ pibstrm->m_dwDot <<= 8@
@ pibstrm->m_dwDot |= *(pibstrm->m_pBuffer)++@
  orr   r5, r12, r5, LSL #8

  b     gPeekBitsLoadFromStreamNaked

gPeekBitsGetMoreDataNaked:
  str   lr, [r1, #CWMAInputBitStream_m_pBuffer]
  str   r3, [r1, #CWMAInputBitStream_m_cbBuflen]
  str   r5, [r1, #CWMAInputBitStream_m_dwDot]
  str   r4, [r1, #CWMAInputBitStream_m_dwBitsLeft]

@ if(pibstrm->m_dwBitsLeft < dwNumBits){
  cmp   r4, r2
  bcs   gPeekBitsNaked

@ TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData(pibstrm, ModePeek, dwNumBits))@

  mov    r0, r1  
  mov    r1, #1
  bl     _ibstrmGetMoreData

  cmp    r0, #0
  bmi    gPeekBitsExitNaked


@ if (pibstrm->m_dwBitsLeft < dwNumBits)
@	 dwNumBits = pibstrm->m_dwBitsLeft@
  mov   r2, #23

  cmp   r4, r2
  movcc r2, r4

gPeekBitsNaked:
@ cBitExtra = (I16) pibstrm->m_dwBitsLeft - (I16) dwNumBits@
  sub   r1, r4, r2
  rsb   lr, r2, #32

@ *piRetBits = (pibstrm->m_dwDot >> cBitExtra) << (32 - dwNumBits)@
  mov   r12, r5, LSR r1
  mov   r3, r12, LSL lr 

  mov   r0, #0

gPeekBitsExitNaked:
  ldmfd sp!, {r4 - r5, r12, PC} @ibstrmPeekBits_Naked
@ ldr   pc,  [sp], #4
  @ENTRY_END ibstrmPeekBits_Naked

  
  
  
  
