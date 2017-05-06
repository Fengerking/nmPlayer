;//*@@@+++@@@@******************************************************************
;//
;// Microsoft Windows Media
;// Copyright (C) Microsoft Corporation. All rights reserved.
;//
;//*@@@---@@@@******************************************************************
;// Module Name:
;//
;//     strmdec_arm.s
;//
;// Abstract:
;// 
;//     ARM Arch-4 specific multiplications
;//
;//      Custom build with 
;//          armasm $(InputDir)\$(InputName).s -o=$(OutDir)\$(InputName).obj 
;//      and
;//          $(OutDir)\$(InputName).obj
;// 
;// Author:
;// 
;//     Jerry He (yamihe) Sep 2, 2003
;//
;// Revision History:
;//
;//     For more information on ARM assembler directives, use
;//        http://msdn.microsoft.com/library/default.asp?url=/library/en-us/wcechp40/html/ccconarmassemblerdirectives.asp
;//*************************************************************************


  OPT         2       ; disable listing 
  INCLUDE     kxarm.h
  INCLUDE     wma_member_arm.inc
  INCLUDE	  wma_arm_version.h
  OPT         1       ; enable listing
  
  AREA    |.text|, CODE, READONLY

  IF WMA_OPT_STRMDEC_ARM = 1
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  IMPORT	ibstrmGetMoreData

  EXPORT	ibstrmGetBits	
  EXPORT	ibstrmLookForBits	
  EXPORT	ibstrmLoadBits	
  EXPORT	ibstrmCountSerial1Bits	
  EXPORT	ibstrmPeekBits	
  EXPORT	ibstrmPeekBits_Naked
  EXPORT	ibstrmFlushBits	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	IF LINUX_RVDS = 1
  	PRESERVE8
	ENDIF
  	AREA    |.text|, CODE
  	LEAF_ENTRY ibstrmGetBits
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Input parameters
; r0 = pibstrm
; r1 = dwNumBits
; r2 = piResult

; r4 = pibstrm->m_dwBitsLeft
; r5 = pibstrm->m_dwDot


  STMFD sp!, {r4 - r5, r12, lr}

; if (pibstrm->m_dwBitsLeft < dwNumBits)
  LDR   r4, [r0, #CWMAInputBitStream_m_dwBitsLeft]
  LDR   r5, [r0, #CWMAInputBitStream_m_dwDot]
  CMP   r4, r1
  BCS   gOutputBits

; LOAD_BITS_FROM_DotT;
; if (pibstrm->m_cBitDotT > 0)
  LDR   r3, [r0, #CWMAInputBitStream_m_cBitDotT]
  CMP   r3, #0
  BLS   gPreLoadFromStream

; I32 cBitMoved = min (32 - pibstrm->m_dwBitsLeft, pibstrm->m_cBitDotT);
  RSB   r12, r4, #32
  CMP   r12, r3
  MOVCS r12, r3

; pibstrm->m_cBitDotT -= cBitMoved;
  SUB   r3, r3, r12
  LDR   lr, [r0, #CWMAInputBitStream_m_dwDotT]
  STR   r3, [r0, #CWMAInputBitStream_m_cBitDotT]

; pibstrm->m_dwBitsLeft += cBitMoved;
  ADD   r4, r4, r12
  MOV   r3, lr, LSR r3

; pibstrm->m_dwDot <<= cBitMoved;
; pibstrm->m_dwDot |= (pibstrm->m_dwDotT >> pibstrm->m_cBitDotT);
  ORR   r5, r3, r5, LSL r12

; pibstrm->m_dwDotT &= ((I32) (1 << pibstrm->m_cBitDotT)) - 1;
  LDR   r3, [r0, #CWMAInputBitStream_m_cBitDotT]
  MOV   r12, #1
  MOV   r12, r12, LSL r3
  SUB   r12, r12, #1
  AND   lr,  lr,  r12
  STR   r5, [r0, #CWMAInputBitStream_m_dwDot]
  STR   lr, [r0, #CWMAInputBitStream_m_dwDotT]

gPreLoadFromStream
  LDR   r3, [r0, #CWMAInputBitStream_m_cbBuflen]
  LDR   lr, [r0, #CWMAInputBitStream_m_pBuffer]

gLoadFromStream
; LOAD_BITS_FROM_STREAM;
; while (pibstrm->m_dwBitsLeft <= 24 && pibstrm->m_cbBuflen > 0)
  CMP   r4, #24
  BHI   gGetMoreData

  CMP   r3, #0
  BLS   gGetMoreData

; --(pibstrm->m_cbBuflen);
  SUB   r3, r3, #1

; pibstrm->m_dwBitsLeft += 8;
  LDRB  r12, [lr], #1
  ADD   r4, r4, #8

; pibstrm->m_dwDot <<= 8;
; pibstrm->m_dwDot |= *(pibstrm->m_pBuffer)++;
  ORR   r5, r12, r5, LSL #8

  B     gLoadFromStream

gGetMoreData
  STR   lr, [r0, #CWMAInputBitStream_m_pBuffer]
  STR   r3, [r0, #CWMAInputBitStream_m_cbBuflen]
  STR   r5, [r0, #CWMAInputBitStream_m_dwDot]

; if(pibstrm->m_dwBitsLeft < dwNumBits){
  CMP   r4, r1
  BCS   gOutputBits

  STR   r4, [r0, #CWMAInputBitStream_m_dwBitsLeft]

; TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData(pibstrm, ModeGetFlush, dwNumBits));
  
  STMFD  sp!, {r0 - r2} ;save r0, r1, r2

  MOV    r2, r1
  MOV    r1, #2
  BL     ibstrmGetMoreData

  MOVS   r3, r0
  LDMFD  sp!, {r0 - r2} ;restore r0, r1, r2

  MOVMI  r0, r3
  BMI    gExit


gOutputBits
; pibstrm->m_dwBitsLeft -= dwNumBits;
  MVN   lr, #0
  SUB   r4, r4, r1
    
; *piResult = (pibstrm->m_dwDot >> pibstrm->m_dwBitsLeft) & getMask[dwNumBits];
  MOV   r12, r5, LSR r4
  BIC   r3, r12, lr, LSL r1
  
;pibstrm->m_cFrmBitCnt += dwNumBits;
	LDR		r5, [r0, #CWMAInputBitStream_m_cFrmBitCnt]
	ADD		r5, r5, r1
	
	STR	r5, [r0, #CWMAInputBitStream_m_cFrmBitCnt]
  STR   r4, [r0, #CWMAInputBitStream_m_dwBitsLeft]
  STR   r3, [r2]
 
  MOV   r0, #0

gExit
  LDMFD sp!, {r4 - r5, r12, PC} ;ibstrmGetBits
  ENTRY_END ibstrmGetBits

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	IF LINUX_RVDS = 1
  	PRESERVE8
	ENDIF
  	AREA    |.text|, CODE
  	LEAF_ENTRY ibstrmLookForBits
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Input parameters
; r0 = pibstrm
; r1 = dwNumBits

; r2 = pibstrm->m_dwBitsLeft
; r3 = pibstrm->m_cBitDotT
; lr = pibstrm->m_cbBuflen


  STR   lr,  [sp, #-4]!

; if (dwNumBits > pibstrm->m_dwBitsLeft + pibstrm->m_cBitDotT + pibstrm->m_cbBuflen * 8)
  LDR   lr, [r0, #CWMAInputBitStream_m_cbBuflen]
  LDR   r2, [r0, #CWMAInputBitStream_m_dwBitsLeft]
  LDR   r3, [r0, #CWMAInputBitStream_m_cBitDotT]

  ADD   r12, r2, lr, LSL #3
  ADD   r12, r12, r3 
  CMP   r1, r12
  MOVLS r0, #0
  BLS   gLookBitsExit

  STMFD sp!, {r4, r5}  ;save r4, r5

  LDR   r4, [r0, #CWMAInputBitStream_m_pBuffer]
  LDR   r5, [r0, #CWMAInputBitStream_m_dwDot]

gLookBitsLoadFromStream
; LOAD_BITS_FROM_STREAM;
; while (pibstrm->m_dwBitsLeft <= 24 && pibstrm->m_cbBuflen > 0)
  CMP   r2, #24
  BHI   gLoadBitsIntoDotT

  CMP   lr, #0
  BLS   gLoadBitsIntoDotT

; --(pibstrm->m_cbBuflen);
  SUB   lr, lr, #1

; pibstrm->m_dwBitsLeft += 8;
  LDRB  r12, [r4], #1
  ADD   r2, r2, #8

; pibstrm->m_dwDot <<= 8;
; pibstrm->m_dwDot |= *(pibstrm->m_pBuffer)++;
  ORR   r5, r12, r5, LSL #8

  B     gLookBitsLoadFromStream


; LOAD_BITS_INTO_DotT
gLoadBitsIntoDotT
; pibstrm->m_dwDotT = 0;
  STR   r5, [r0, #CWMAInputBitStream_m_dwDot]
  MOV   r5, #0
   
; while (pibstrm->m_cbBuflen > 0)
  CMP   lr, #0
  BLS   gLookBitsGetMoreData
  
; --(pibstrm->m_cbBuflen);
  SUB   lr, lr, #1

; pibstrm->m_cBitDotT += 8;
  LDRB  r12, [r4], #1
  ADD   r3, r3, #8
     
; pibstrm->m_dwDotT <<= 8;
; pibstrm->m_dwDotT |= *(pibstrm->m_pBuffer)++;
  ORR   r5, r12, r5, LSL #8

  B     gLoadBitsIntoDotT
  
gLookBitsGetMoreData
  STR   lr, [r0, #CWMAInputBitStream_m_cbBuflen]
  STR   r2, [r0, #CWMAInputBitStream_m_dwBitsLeft]
  STR   r3, [r0, #CWMAInputBitStream_m_cBitDotT]

  STR   r4, [r0, #CWMAInputBitStream_m_pBuffer]
  STR   r5, [r0, #CWMAInputBitStream_m_dwDotT]

  LDMFD  sp!, {r4, r5} ;restore r4, r5


; TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData(pibstrm, ModeLookFor, dwNumBits));
  
  MOV    r2, r1
  MOV    r1, #0
  BL     ibstrmGetMoreData

gLookBitsExit
  LDR    PC, [sp], #4 ;ibstrmLookForBits
  ENTRY_END ibstrmLookForBits

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	IF LINUX_RVDS = 1
  	PRESERVE8
	ENDIF
  	AREA    |.text|, CODE
  	LEAF_ENTRY ibstrmLoadBits
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Input parameters
; r0 = pibs
; r1 = iFirstBit
; r2 = uByte

; r4 = pibstrm->m_cbBuflen
; r5 = pibs->m_pBuffer


  STMFD sp!, {r4 - r6, lr}

; r6 = pibs->m_cBitDotT
; r3 = pibstrm->m_dwBitsLeft

;if ( 0 == iFirstBit ) // ues
;    {
;        uByte = *(pibs->m_pBuffer);
;    }
	CMP		r1, #0
	BNE		NotNeedReadData
	LDR		r5, [r0, #CWMAInputBitStream_m_pBuffer]
	LDRB	r2, [r5]
	
NotNeedReadData
; if (pibs->m_cBitDotT == 0 && pibs->m_dwBitsLeft + (8 - iFirstBit) <= 32)
  LDR   r6, [r0, #CWMAInputBitStream_m_cBitDotT]
;  LDR   r5, [r0, #CWMAInputBitStream_m_pBuffer]
;  LDR   r4, [r0, #CWMAInputBitStream_m_cbBuflen]
  CMP   r6, #0
  BNE   gLoadBitsElse

  LDR   r3, [r0, #CWMAInputBitStream_m_dwBitsLeft]

  SUB   lr, r3, r1
  CMP   lr, #24
  BGT   gLoadBitsElse

  LDR   r6, [r0, #CWMAInputBitStream_m_dwDot]

;gLoadBitsWhile
; while (pibs->m_cbBuflen > 0 && (pibs->m_dwBitsLeft + (8 - iFirstBit)) <= 32)
;  CMP   r4, #0
;  BLS   gLoadBitsWhileEnd

; temp = (*pibs->m_pBuffer++);
;  LDRB  r12, [r5], #1
  
; pibs->m_cbBuflen--;
;  SUB   r4, r4, #1

; temp = temp<<iFirstBit;
; temp = temp>>iFirstBit;
  MOV   r2, r2, LSL r1
  AND   r2, r2, #0xFF

  RSB   lr, r1, #8
  MOV   r2, r2, LSR r1
  
; pibs->m_dwDot = (pibs->m_dwDot<<(8 - iFirstBit))|temp;
  ORR   r6, r2, r6, LSL lr
  
; pibs->m_dwBitsLeft += (8 - iFirstBit);
  ADD   r3, r3, lr

; iFirstBit = 0;
  MOV   r1, #0

;  CMP   r3, #24
;  BLE   gLoadBitsWhile

;gLoadBitsWhileEnd
  STR   r3, [r0, #CWMAInputBitStream_m_dwBitsLeft]
  STR   r6, [r0, #CWMAInputBitStream_m_dwDot]
  B     gLoadBitsExit

gLoadBitsElse
; temp = (*pibs->m_pBuffer++);
;  LDRB  r12, [r5], #1
  LDR   r3, [r0, #CWMAInputBitStream_m_dwDotT]

; pibs->m_cbBuflen--;
;  SUB   r4, r4, #1

; temp = temp<<iFirstBit;
; temp = temp>>iFirstBit;
  MOV   r2, r2, LSL r1
  AND   r2, r2, #0xFF

  RSB   lr, r1, #8
  MOV   r2, r2, LSR r1

; pibs->m_dwDotT = (pibs->m_dwDotT<<(8 - iFirstBit))|temp;
  ORR   r3, r2, r3, LSL lr

; pibs->m_cBitDotT += (8 - iFirstBit);
  ADD   r6, r6, lr

  STR   r3, [r0, #CWMAInputBitStream_m_dwDotT]
  STR   r6, [r0, #CWMAInputBitStream_m_cBitDotT]

gLoadBitsExit
;  STR   r5, [r0, #CWMAInputBitStream_m_pBuffer]
;  STR   r4, [r0, #CWMAInputBitStream_m_cbBuflen]

  LDMFD sp!, {r4 - r6, PC} ;ibstrmLoadBits
  ENTRY_END ibstrmLoadBits


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	IF LINUX_RVDS = 1
  	PRESERVE8
	ENDIF
  	AREA    |.text|, CODE
  	LEAF_ENTRY ibstrmCountSerial1Bits
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Input parameters
; r0 = pibstrm
; r1 = pCount

; r2 = pibstrm->m_dwBitsLeft
; r3 = pibstrm->m_dwDot
; r4 = *pCount

  STMFD sp!, {r4, lr}

; r2 = pibstrm->m_dwBitsLeft
  LDR   r2, [r0, #CWMAInputBitStream_m_dwBitsLeft]

; r3 = pibstrm->m_dwDot
  LDR   r3, [r0, #CWMAInputBitStream_m_dwDot]

; r4 = *pCount
  LDR   r4, [r1]
 
; if (pibstrm->m_dwBitsLeft <= 0)  
  CMP   r2, #0
  BGT   gCountBits

gReload
; pibstrm->m_dwBitsLeft = 0;
  MOV   r2, #0

; LOAD_BITS_FROM_DotT;
; if (pibstrm->m_cBitDotT > 0)
  LDR   lr, [r0, #CWMAInputBitStream_m_cBitDotT]
  CMP   lr, #0
  BLS   gPreLoadFromStream2

; I32 cBitMoved = min (32 - pibstrm->m_dwBitsLeft, pibstrm->m_cBitDotT);
; pibstrm->m_cBitDotT -= cBitMoved;
; pibstrm->m_dwBitsLeft += cBitMoved;

; pibstrm->m_dwBitsLeft == 0 -> cBitMoved = pibstrm->m_cBitDotT
; then pibstrm->m_cBitDotT = 0

  LDR   r12,[r0, #CWMAInputBitStream_m_dwDotT]
  STR   r2, [r0, #CWMAInputBitStream_m_cBitDotT]

; pibstrm->m_dwDot <<= cBitMoved;
; pibstrm->m_dwDot |= (pibstrm->m_dwDotT >> pibstrm->m_cBitDotT);
; pibstrm->m_dwDotT &= ((I32) (1 << pibstrm->m_cBitDotT)) - 1;
; -> pibstrm->m_dwDot = pibstrm->m_dwDotT;
; -> pibstrm->m_dwDotT no meaning

  STR   r12,[r0, #CWMAInputBitStream_m_dwDot]
  MOV   r2, lr
  MOV   r3, r12

gPreLoadFromStream2
  LDR   r12,[r0, #CWMAInputBitStream_m_cbBuflen]
  LDR   lr, [r0, #CWMAInputBitStream_m_pBuffer]

; save *pCount, release r4
  STR   r4, [r1]

gLoadFromStream2
; LOAD_BITS_FROM_STREAM;
; while (pibstrm->m_dwBitsLeft <= 24 && pibstrm->m_cbBuflen > 0)
  CMP   r2, #24
  BHI   gGetMoreData2

  CMP   r12, #0
  BLS   gGetMoreData2

; --(pibstrm->m_cbBuflen);
  SUB   r12, r12, #1

; pibstrm->m_dwBitsLeft += 8;
  LDRB  r4, [lr], #1
  ADD   r2, r2, #8

; pibstrm->m_dwDot <<= 8;
; pibstrm->m_dwDot |= *(pibstrm->m_pBuffer)++;
  ORR   r3, r4, r3, LSL #8

  B     gLoadFromStream2

gGetMoreData2
  STR   lr, [r0, #CWMAInputBitStream_m_pBuffer]
  STR   r12,[r0, #CWMAInputBitStream_m_cbBuflen]
  STR   r3, [r0, #CWMAInputBitStream_m_dwDot]

; load *pCount, release r4
  LDR   r4, [r1]

; if(pibstrm->m_dwBitsLeft < 1){
  CMP   r2, #1
  BCS   gCountBits

  STR   r2, [r0, #CWMAInputBitStream_m_dwBitsLeft]

; TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData(pibstrm, ModeGetFlush, 1));
  
  STMFD  sp!, {r0 - r1} ;save r0, r1

  MOV    r2, #1
  MOV    r1, #2
  BL     ibstrmGetMoreData

  MOVS   lr, r0
  LDMFD  sp!, {r0 - r1} ;restore r0, r1

  MOVMI  r0, lr
  BMI    gExit2

  LDR   r2, [r0, #CWMAInputBitStream_m_dwBitsLeft]
  LDR   r3, [r0, #CWMAInputBitStream_m_dwDot]

gCountBits
; dwCurrent = pibstrm->m_dwDot << (32-pibstrm->m_dwBitsLeft);
  RSB   lr, r2, #33

  MOVS  r12, r3, LSL lr
  BCC   gWhileEnd

; dwCount = 1;
  MOV   lr, #1
    
; while (dwCurrent & 0x80000000)
;    dwCount ++;
;    dwCurrent <<= 1;

gWhileCount
  MOVS  r12, r12, LSL #1

  ADDCS lr, lr, #1
  BCS   gWhileCount

; pibstrm->m_dwBitsLeft -= (dwCount+1);
  SUB   r2, r2, lr

; *pCount += dwCount;
  ADD   r4, r4, lr

gWhileEnd
  SUBS  r2, r2, #1

; if ((I32)pibstrm->m_dwBitsLeft < 0)
;	 goto reload;
  BLT   gReload

  STR   r2, [r0, #CWMAInputBitStream_m_dwBitsLeft]
  STR   r4, [r1]
 
  MOV   r0, #0

gExit2
  LDMFD sp!, {r4, PC} ;ibstrmCountSerial1Bits
  ENTRY_END ibstrmCountSerial1Bits 


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	IF LINUX_RVDS = 1
  	PRESERVE8
	ENDIF
  	AREA    |.text|, CODE
  	LEAF_ENTRY ibstrmPeekBits
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Input parameters
; r0 = pibstrm
; r1 = dwNumBits
; r2 = piRetBits

; r4 = pibstrm->m_dwBitsLeft
; r5 = pibstrm->m_dwDot


  STMFD sp!, {r4 - r5, r12, lr}

; if (pibstrm->m_dwBitsLeft < dwNumBits)
  LDR   r4, [r0, #CWMAInputBitStream_m_dwBitsLeft]
  LDR   r5, [r0, #CWMAInputBitStream_m_dwDot]
  CMP   r4, r1
  BCS   gPeekBits

; LOAD_BITS_FROM_DotT;
; if (pibstrm->m_cBitDotT > 0)
  LDR   r3, [r0, #CWMAInputBitStream_m_cBitDotT]
  CMP   r3, #0
  BLS   gPeekBitsPreLoadFromStream

; I32 cBitMoved = min (32 - pibstrm->m_dwBitsLeft, pibstrm->m_cBitDotT);
  RSB   r12, r4, #32
  CMP   r12, r3
  MOVCS r12, r3

; pibstrm->m_cBitDotT -= cBitMoved;
  SUB   r3, r3, r12
  LDR   lr, [r0, #CWMAInputBitStream_m_dwDotT]
  STR   r3, [r0, #CWMAInputBitStream_m_cBitDotT]

; pibstrm->m_dwBitsLeft += cBitMoved;
  ADD   r4, r4, r12
  MOV   r3, lr, LSR r3

; pibstrm->m_dwDot <<= cBitMoved;
; pibstrm->m_dwDot |= (pibstrm->m_dwDotT >> pibstrm->m_cBitDotT);
  ORR   r5, r3, r5, LSL r12

; pibstrm->m_dwDotT &= ((I32) (1 << pibstrm->m_cBitDotT)) - 1;
  LDR   r3, [r0, #CWMAInputBitStream_m_cBitDotT]
  MOV   r12, #1
  MOV   r12, r12, LSL r3
  SUB   r12, r12, #1
  AND   lr,  lr,  r12
  STR   r5, [r0, #CWMAInputBitStream_m_dwDot]
  STR   lr, [r0, #CWMAInputBitStream_m_dwDotT]

gPeekBitsPreLoadFromStream
  LDR   r3, [r0, #CWMAInputBitStream_m_cbBuflen]
  LDR   lr, [r0, #CWMAInputBitStream_m_pBuffer]

gPeekBitsLoadFromStream
; LOAD_BITS_FROM_STREAM;
; while (pibstrm->m_dwBitsLeft <= 24 && pibstrm->m_cbBuflen > 0)
  CMP   r4, #24
  BHI   gPeekBitsGetMoreData

  CMP   r3, #0
  BLS   gPeekBitsGetMoreData

; --(pibstrm->m_cbBuflen);
  SUB   r3, r3, #1

; pibstrm->m_dwBitsLeft += 8;
  LDRB  r12, [lr], #1
  ADD   r4, r4, #8

; pibstrm->m_dwDot <<= 8;
; pibstrm->m_dwDot |= *(pibstrm->m_pBuffer)++;
  ORR   r5, r12, r5, LSL #8

  B     gPeekBitsLoadFromStream

gPeekBitsGetMoreData
  STR   lr, [r0, #CWMAInputBitStream_m_pBuffer]
  STR   r3, [r0, #CWMAInputBitStream_m_cbBuflen]
  STR   r5, [r0, #CWMAInputBitStream_m_dwDot]
  STR   r4, [r0, #CWMAInputBitStream_m_dwBitsLeft]

; if(pibstrm->m_dwBitsLeft < dwNumBits){
  CMP   r4, r1
  BCS   gPeekBits

; TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData(pibstrm, ModePeek, dwNumBits));
  
  STMFD  sp!, {r0 - r2} ;save r0, r1, r2

  MOV    r2, r1
  MOV    r1, #1
  BL     ibstrmGetMoreData

  MOVS   r3, r0
  LDMFD  sp!, {r0 - r2} ;restore r0, r1, r2

  MOVMI  r0, r3
  BMI    gPeekBitsExit


; if (pibstrm->m_dwBitsLeft < dwNumBits)
;	 dwNumBits = pibstrm->m_dwBitsLeft;
  CMP   r4, r1
  MOVCC r1, r4

gPeekBits
; cBitExtra = (I16) pibstrm->m_dwBitsLeft - (I16) dwNumBits;
  SUB   r3, r4, r1
  RSB   lr, r1, #32

; *piRetBits = (pibstrm->m_dwDot >> cBitExtra) << (32 - dwNumBits);
  MOV   r12, r5, LSR r3
  MOV   r1, r12, LSL lr 

  MOV   r0, #0
  STR   r1, [r2]

gPeekBitsExit
  LDMFD sp!, {r4 - r5, r12, PC} ;ibstrmPeekBits
  ENTRY_END ibstrmPeekBits


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	IF LINUX_RVDS = 1
  	PRESERVE8
	ENDIF
  	AREA    |.text|, CODE 
  	LEAF_ENTRY ibstrmPeekBits_Naked
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Input parameters
; r1 = pibstrm

; r3 = iRetBits
; r4 = pibstrm->m_dwBitsLeft
; r5 = pibstrm->m_dwDot


  STMFD sp!, {r4 - r5, r12, lr}
; STR   lr,  [sp, #-4]!

; r2 = dwNumBits
  MOV   r2, #23

; if (pibstrm->m_dwBitsLeft < dwNumBits)
  LDR   r4, [r1, #CWMAInputBitStream_m_dwBitsLeft]
  LDR   r5, [r1, #CWMAInputBitStream_m_dwDot]
  CMP   r4, r2
  BCS   gPeekBitsNaked

; LOAD_BITS_FROM_DotT;
; if (pibstrm->m_cBitDotT > 0)
  LDR   r3, [r1, #CWMAInputBitStream_m_cBitDotT]
  CMP   r3, #0
  BLS   gPeekBitsPreLoadFromStreamNaked

; I32 cBitMoved = min (32 - pibstrm->m_dwBitsLeft, pibstrm->m_cBitDotT);
  RSB   r12, r4, #32
  CMP   r12, r3
  MOVCS r12, r3

; pibstrm->m_cBitDotT -= cBitMoved;
  SUB   r3, r3, r12
  LDR   lr, [r1, #CWMAInputBitStream_m_dwDotT]
  STR   r3, [r1, #CWMAInputBitStream_m_cBitDotT]

; pibstrm->m_dwBitsLeft += cBitMoved;
  ADD   r4, r4, r12
  MOV   r3, lr, LSR r3

; pibstrm->m_dwDot <<= cBitMoved;
; pibstrm->m_dwDot |= (pibstrm->m_dwDotT >> pibstrm->m_cBitDotT);
  ORR   r5, r3, r5, LSL r12

; pibstrm->m_dwDotT &= ((I32) (1 << pibstrm->m_cBitDotT)) - 1;
  LDR   r3, [r1, #CWMAInputBitStream_m_cBitDotT]
  MOV   r12, #1
  MOV   r12, r12, LSL r3
  SUB   r12, r12, #1
  AND   lr,  lr,  r12
  STR   r5, [r1, #CWMAInputBitStream_m_dwDot]
  STR   lr, [r1, #CWMAInputBitStream_m_dwDotT]

gPeekBitsPreLoadFromStreamNaked
  LDR   r3, [r1, #CWMAInputBitStream_m_cbBuflen]
  LDR   lr, [r1, #CWMAInputBitStream_m_pBuffer]

gPeekBitsLoadFromStreamNaked
; LOAD_BITS_FROM_STREAM;
; while (pibstrm->m_dwBitsLeft <= 24 && pibstrm->m_cbBuflen > 0)
  CMP   r4, #24
  BHI   gPeekBitsGetMoreDataNaked

  CMP   r3, #0
  BLS   gPeekBitsGetMoreDataNaked

; --(pibstrm->m_cbBuflen);
  SUB   r3, r3, #1

; pibstrm->m_dwBitsLeft += 8;
  LDRB  r12, [lr], #1
  ADD   r4, r4, #8

; pibstrm->m_dwDot <<= 8;
; pibstrm->m_dwDot |= *(pibstrm->m_pBuffer)++;
  ORR   r5, r12, r5, LSL #8

  B     gPeekBitsLoadFromStreamNaked

gPeekBitsGetMoreDataNaked
  STR   lr, [r1, #CWMAInputBitStream_m_pBuffer]
  STR   r3, [r1, #CWMAInputBitStream_m_cbBuflen]
  STR   r5, [r1, #CWMAInputBitStream_m_dwDot]
  STR   r4, [r1, #CWMAInputBitStream_m_dwBitsLeft]

; if(pibstrm->m_dwBitsLeft < dwNumBits){
  CMP   r4, r2
  BCS   gPeekBitsNaked

; TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData(pibstrm, ModePeek, dwNumBits));

  MOV    r0, r1  
  MOV    r1, #1
  BL     ibstrmGetMoreData

  CMP    r0, #0
  BMI    gPeekBitsExitNaked


; if (pibstrm->m_dwBitsLeft < dwNumBits)
;	 dwNumBits = pibstrm->m_dwBitsLeft;
  MOV   r2, #23

  CMP   r4, r2
  MOVCC r2, r4

gPeekBitsNaked
; cBitExtra = (I16) pibstrm->m_dwBitsLeft - (I16) dwNumBits;
  SUB   r1, r4, r2
  RSB   lr, r2, #32

; *piRetBits = (pibstrm->m_dwDot >> cBitExtra) << (32 - dwNumBits);
  MOV   r12, r5, LSR r1
  MOV   r3, r12, LSL lr 

  MOV   r0, #0

gPeekBitsExitNaked
  LDMFD sp!, {r4 - r5, r12, PC} ;ibstrmPeekBits_Naked
; LDR   pc,  [sp], #4
  ENTRY_END ibstrmPeekBits_Naked


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	IF LINUX_RVDS = 1
  	PRESERVE8
	ENDIF
  	AREA    |.text|, CODE
  	LEAF_ENTRY ibstrmFlushBits
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Input parameters
; r0 = pibstrm
; r1 = dwNumBits

; r4 = pibstrm->m_dwBitsLeft


  STMFD sp!, {r4, r5, lr}

; if (pibstrm->m_dwBitsLeft < dwNumBits)
  LDR   r4, [r0, #CWMAInputBitStream_m_dwBitsLeft]
  CMP   r4, r1
  BCS   gFlushBits

; LOAD_BITS_FROM_DotT;

; r2 = pibstrm->m_dwDot
  LDR   r2, [r0, #CWMAInputBitStream_m_dwDot]

; if (pibstrm->m_cBitDotT > 0)
  LDR   r3, [r0, #CWMAInputBitStream_m_cBitDotT]
  CMP   r3, #0
  BLS   gFlushBitsPreLoadFromStream

; I32 cBitMoved = min (32 - pibstrm->m_dwBitsLeft, pibstrm->m_cBitDotT);
  RSB   r12, r4, #32
  CMP   r12, r3
  MOVCS r12, r3

; pibstrm->m_cBitDotT -= cBitMoved;
  SUB   r3, r3, r12
  LDR   lr, [r0, #CWMAInputBitStream_m_dwDotT]
  STR   r3, [r0, #CWMAInputBitStream_m_cBitDotT]

; pibstrm->m_dwBitsLeft += cBitMoved;
  ADD   r4, r4, r12
  MOV   r5, lr, LSR r3

; pibstrm->m_dwDot <<= cBitMoved;
; pibstrm->m_dwDot |= (pibstrm->m_dwDotT >> pibstrm->m_cBitDotT);
  ORR   r2, r5, r2, LSL r12

; pibstrm->m_dwDotT &= ((I32) (1 << pibstrm->m_cBitDotT)) - 1;
	MOV		r5, lr, LSR r3
	EOR		lr, lr, r5, LSL r3
	STR		lr, [r0, #CWMAInputBitStream_m_dwDotT]
	
gFlushBitsPreLoadFromStream
  LDR   r3, [r0, #CWMAInputBitStream_m_cbBuflen]
  LDR   lr, [r0, #CWMAInputBitStream_m_pBuffer]

gFlushBitsLoadFromStream
; LOAD_BITS_FROM_STREAM;
; while (pibstrm->m_dwBitsLeft <= 24 && pibstrm->m_cbBuflen > 0)
  CMP   r4, #24
  BHI   gFlushBitsGetMoreData

  CMP   r3, #0
  BLS   gFlushBitsGetMoreData

; --(pibstrm->m_cbBuflen);
  SUB   r3, r3, #1

; pibstrm->m_dwBitsLeft += 8;
  LDRB  r12, [lr], #1
  ADD   r4, r4, #8

; pibstrm->m_dwDot <<= 8;
; pibstrm->m_dwDot |= *(pibstrm->m_pBuffer)++;
  ORR   r2, r12, r2, LSL #8

  B     gFlushBitsLoadFromStream

gFlushBitsGetMoreData
  STR   lr, [r0, #CWMAInputBitStream_m_pBuffer]
  STR   r3, [r0, #CWMAInputBitStream_m_cbBuflen]
  STR   r2, [r0, #CWMAInputBitStream_m_dwDot]

; if(pibstrm->m_dwBitsLeft < dwNumBits){
  CMP   r4, r1
  BCS   gFlushBits

  STR   r4, [r0, #CWMAInputBitStream_m_dwBitsLeft]

; TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData(pibstrm, ModeGetFlush, dwNumBits));
  
  STMFD  sp!, {r0 - r1} ;save r0, r1

  MOV    r2, r1
  MOV    r1, #2
  BL     ibstrmGetMoreData
  MOVS   r3, r0
  LDMFD  sp!, {r0 - r1} ;restore r0, r1
  MOVMI  r0, r3
  BMI    gFlushBitsExit

gFlushBits
; pibstrm->m_dwBitsLeft -= dwNumBits;
;pibstrm->m_cFrmBitCnt += dwNumBits;
	LDR		r5, [r0, #CWMAInputBitStream_m_cFrmBitCnt]
	SUB		r4, r4, r1
	STR		r4, [r0, #CWMAInputBitStream_m_dwBitsLeft]  

	ADD		r5, r5, r1
	STR		r5, [r0, #CWMAInputBitStream_m_cFrmBitCnt]
	MOV		r0, #0

gFlushBitsExit
  LDMFD sp!, {r4, r5, PC} ;ibstrmFlushBits
  ENTRY_END ibstrmFlushBits


  ENDIF ; WMA_OPT_STRMDEC_ARM   
 
  END