;//*@@@+++@@@@******************************************************************
;//
;// Microsoft Windows Media
;// Copyright (C) Microsoft Corporation. All rights reserved.
;//
;//*@@@---@@@@******************************************************************
;// Module Name:
;//
;//     msaudiodec_arm.s
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
;//     Jerry He (yamihe) Jan 27, 2004
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

  IF WMA_OPT_CHANNEL_DOWNMIX_ARM = 1
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  IMPORT  prvChannelUpMix
  EXPORT  prvChannelDownMix


;//*************************************************************************************
;//
;// WMARESULT prvChannelDownMix (CAudioObjectDecoder *paudec, U8 *piSourceBuffer, Int nBlocks, U8 *piDestBuffer)
;//
;//*************************************************************************************

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Registers for prvChannelDownMix
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

paudec            RN  0
piSourceBuffer    RN  1
nBlocks           RN  2
piDestBuffer      RN  3

iChDst            RN  4
iChSrc            RN  5
pcmsDst           RN  6
tmpSrc            RN  7
pcmsMixMtx        RN  8

cMaxValue         RN  9  
cMinValue         RN  10

cSrcChannel       RN  14
cDstChannel       RN  11

pau               RN  7
	IF	DEVICEPCMOUT16	=	0
BitDepth          RN  8
	ENDIF
temp1             RN  12
temp2             RN  14
temp3             RN  0


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Constants for prvChannelDownMix
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
iStackSpaceRev              EQU 2*4    
iOffset_cSrcChannel         EQU iStackSpaceRev-4
iOffset_rgrgfltChDnMixMtx   EQU iStackSpaceRev-8

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	IF LINUX_RVDS = 1
  	PRESERVE8
	ENDIF
  	AREA    |.text|, CODE
  	LEAF_ENTRY prvChannelDownMix
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Input parameters
; r0 = paudec
; r1 = piSourceBuffer
; r2 = nBlocks
; r3 = piDestBuffer


  STMFD sp!, {r4 - r12, lr}
  SUB   sp, sp, #iStackSpaceRev ; rev stack space

; if (!paudec->m_fChannelFoldDown)
  LDR   temp1, [paudec, #CAudioObjectDecoder_m_fChannelFoldDown]
  CMP   temp1, #0
  BEQ   prvChannelDownMix_Exit

; CAudioObject* pau = paudec->pau;
  LDR   pau, [paudec, #CAudioObjectDecoder_pau]

; Int cSrcChannel   = pau->m_cChannel;
; Int cDstChannel   = paudec->m_cDstChannel;
  LDR   cDstChannel, [paudec, #CAudioObjectDecoder_m_cDstChannel]
  LDRH  cSrcChannel, [pau, #CAudioObject_m_cChannel]
	IF		DEVICEPCMOUT16	= 0
  LDR   BitDepth, [pau, #CAudioObject_m_iBitDepthSelector]
	ENDIF
; if (cSrcChannel >= cDstChannel)
  CMP   cSrcChannel, cDstChannel
  BGE   ChannelDownMix
  
; prvChannelUpMix(paudec, piSourceBuffer, nBlocks, piDestBuffer);
  BL    prvChannelUpMix
  B     prvChannelDownMix_Exit

ChannelDownMix

  CMP   nBlocks, #0
  BEQ   prvChannelDownMix_Exit

  LDR   temp1, [paudec, #CAudioObjectDecoder_m_rgrgfltChDnMixMtx]
  STR   cSrcChannel, [sp, #iOffset_cSrcChannel]
  STR   temp1, [sp, #iOffset_rgrgfltChDnMixMtx]

; pau->m_iPCMSampleMin
  LDR   cMinValue, [pau, #CAudioObject_m_iPCMSampleMin]
  LDR   cMaxValue, [pau, #CAudioObject_m_iPCMSampleMax]
	IF	DEVICEPCMOUT16	=	0
; if (pau->m_iBitDepthSelector == BITDEPTH_16)
  CMP   BitDepth, #0x3D   ; BITDEPTH_16
  BNE   ChannelDownMix24
	ENDIF
ChannelDownMix16
  MOV   iChDst, #0

DstChannelLoop16
; for (iChDst = 0; iChDst < cDstChannel; iChDst++)
; pcmsDst = 0;
  MOV   pcmsDst, #0

; tmpSrc = piSrc;
  MOV   tmpSrc, piSourceBuffer

; pcmsMixMtx = paudec->m_rgrgfltChDnMixMtx[iChDst];
  LDR   temp1, [sp, #iOffset_rgrgfltChDnMixMtx]
  ADD   temp1, temp1, iChDst, LSL #2
  LDR   pcmsMixMtx, [temp1]

  LDR   iChSrc, [sp, #iOffset_cSrcChannel]

SrcChannelLoop16
; for (iChSrc = 0; iChSrc < cSrcChannel; iChSrc++)

; pcmsTemp = *((I16*)tmpSrc);
; tmpSrc += 2;
  LDRSH temp1, [tmpSrc], #2
  LDR   temp2, [pcmsMixMtx], #4
  MOV   temp1, temp1, LSL #9
  MOV   temp3, #0
                     
; pcmsDst += (PCMSAMPLE) MULT_CHDN(pcmsTemp, *pcmsMixMtx++);
  SMLAL temp3, pcmsDst, temp2, temp1

  SUBS  iChSrc, iChSrc, #1
  BNE   SrcChannelLoop16

; PPPOSTCLIPRANGE(pcmsDst);
  CMP   pcmsDst, cMaxValue
  MOVGT pcmsDst, cMaxValue

  CMP   pcmsDst, cMinValue
  MOVLT pcmsDst, cMinValue

; *((I16*)piDst) = (I16)pcmsDst;
; piDst += 2;
  STRH  pcmsDst, [piDestBuffer], #2

  ADD   iChDst, iChDst, #1
  CMP   iChDst, cDstChannel
  BLT   DstChannelLoop16

; piSrc = tmpSrc;
  MOV   piSourceBuffer, tmpSrc

  SUBS  nBlocks, nBlocks, #1
  BNE   ChannelDownMix16
  
	IF	DEVICEPCMOUT16	=	0
  B     prvChannelDownMix_Exit

ChannelDownMix24
  MOV   iChDst, #0

DstChannelLoop24
; for (iChDst = 0; iChDst < cDstChannel; iChDst++)
; pcmsDst = 0;
  MOV   pcmsDst, #0

; tmpSrc = piSrc;
  MOV   tmpSrc, piSourceBuffer

; pcmsMixMtx = paudec->m_rgrgfltChDnMixMtx[iChDst];
  LDR   temp1, [sp, #iOffset_rgrgfltChDnMixMtx]
  ADD   temp1, temp1, iChDst, LSL #2
  LDR   pcmsMixMtx, [temp1]

  LDR   iChSrc, [sp, #iOffset_cSrcChannel]

SrcChannelLoop24
; for (iChSrc = 0; iChSrc < cSrcChannel; iChSrc++)

; pcmsTemp = (*((U8*)tmpSrc) | (*((U8*)(tmpSrc + 1)) << 8) | (*((I8*)(tmpSrc + 2)) << 16));
; tmpSrc += 3;
  
  LDRB  temp1, [tmpSrc], #1
  LDRB  temp2, [tmpSrc], #1
  LDRB  temp3, [tmpSrc], #1

  ADD   temp1, temp1, temp2, LSL #8
  ADD   temp1, temp1, temp3, LSL #16
  LDR   temp2, [pcmsMixMtx], #4
  
  MOV   temp1, temp1, LSL #8
  MOV   temp2, temp2, LSL #1
                     
; pcmsDst += (PCMSAMPLE) MULT_CHDN(pcmsTemp, *pcmsMixMtx++);
  SMLAL temp3, pcmsDst, temp2, temp1

  SUBS  iChSrc, iChSrc, #1
  BNE   SrcChannelLoop24

; PPPOSTCLIPRANGE(pcmsDst);
  CMP   pcmsDst, cMaxValue
  MOVGT pcmsDst, cMaxValue

  CMP   pcmsDst, cMinValue
  MOVLT pcmsDst, cMinValue

; tmpDst = (U8*) &pcmsDst;
; *((I8*)piDst) = *((I8*)tmpDst);
; *((I8*)(piDst + 1)) = *((I8*)(tmpDst + 1));
; *((I8*)(piDst + 2)) = *((I8*)(tmpDst + 2));
; piDst += 3;

  STRB  pcmsDst, [piDestBuffer], #1
  MOV   pcmsDst, pcmsDst, LSR #8
  STRB  pcmsDst, [piDestBuffer], #1
  MOV   pcmsDst, pcmsDst, LSR #8
  STRB  pcmsDst, [piDestBuffer], #1

  ADD   iChDst, iChDst, #1
  CMP   iChDst, cDstChannel
  BLT   DstChannelLoop24

; piSrc = tmpSrc;
  MOV   piSourceBuffer, tmpSrc

  SUBS  nBlocks, nBlocks, #1
  BNE   ChannelDownMix24
	ENDIF
prvChannelDownMix_Exit	
  MOV   r0, #0

  ADD   sp, sp, #iStackSpaceRev     ; give back rev stack space  
  LDMFD sp!, {r4 - r12, PC} ;prvChannelDownMix
  ENTRY_END prvChannelDownMix

  ENDIF ; WMA_OPT_CHANNEL_DOWNMIX_ARM


  
  IF WMA_OPT_REQUANTO16_ARM = 1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  EXPORT  prvRequantizeTo16


;//*************************************************************************************
;//
;// WMARESULT prvRequantizeTo16(CAudioObjectDecoder *paudec, U8 *piSrc, Int nBlocks)
;//
;//*************************************************************************************

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Registers for prvRequantizeTo16
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

paudecReoon       RN  0
piSrcReoon        RN  1
nBlocksRecon      RN  2

piDstRecon        RN  3
cDstChannelRecon  RN  12
fReQuantizeTo16   RN  14

T1Recon           RN  12
T2Recon           RN  14

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  AREA    |.text|, CODE
  LEAF_ENTRY prvRequantizeTo16
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Input parameters
; r0 = paudec
; r1 = piSrc
; r2 = nBlocks


  STR   lr,  [sp, #-4]!

; Int cDstChannel = paudec->m_cDstChannel;
  LDR   cDstChannelRecon, [paudecReoon, #CAudioObjectDecoder_m_cDstChannel]

; if (!paudec->m_fReQuantizeTo16)
;	 goto exit;
  LDR   fReQuantizeTo16, [paudecReoon, #CAudioObjectDecoder_m_fReQuantizeTo16]
  MUL   T1Recon, nBlocksRecon, cDstChannelRecon 

  MOV   piDstRecon, piSrcReoon
  CMP   fReQuantizeTo16, #0
  BEQ   prvRequantizeTo16_Exit

  CMP   nBlocksRecon, #0
  BEQ   prvRequantizeTo16_Exit

  MOV   nBlocksRecon, T1Recon, LSR #1

BlockReconLoop
; for (iBlock = 0; iBlock < nBlocks*cDstChannel; iBlock++)
 
;	pcmsTemp = (*((U8*)(piSrc + 1))) | (*((I8*)(piSrc + 2)) << 8);
;	*((I16*)piDst) = pcmsTemp;

;	piSrc += 3;
;	piDst += 2;
;   unloop by 2

  LDRB  T1Recon, [piSrcReoon, #1]
  LDRB  T2Recon, [piSrcReoon, #2]

  ADD   T1Recon, T1Recon, T2Recon, LSL #8
  STRH  T1Recon, [piDstRecon], #2

  LDRB  T1Recon, [piSrcReoon, #4]
  LDRB  T2Recon, [piSrcReoon, #5]

  ADD   T1Recon, T1Recon, T2Recon, LSL #8
  ADD   piSrcReoon, piSrcReoon, #6
  STRH  T1Recon, [piDstRecon], #2

  SUBS  nBlocksRecon, nBlocksRecon, #1
  BNE   BlockReconLoop
    
prvRequantizeTo16_Exit

  MOV   r0, #0

  LDR   PC, [sp], #4 ;prvRequantizeTo16
  ENTRY_END prvRequantizeTo16


  ENDIF ; WMA_OPT_REQUANTO16_ARM

    
  END