@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************
@// Module Name:
@//
@//     msaudiodec_arm.s
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
@//     Jerry He (yamihe) Jan 27, 2004
@//
@// Revision History:
@//
@//     For more information on ARM assembler directives, use
@//        http://msdn.microsoft.com/library/default.asp?url=/library/en-us/wcechp40/html/ccconarmassemblerdirectives.asp
@//*************************************************************************

  #include "../../../inc/audio/v10/include/voWMADecID.h"
  .include     "kxarm.h"
  .include     "wma_member_arm.inc"
  .include	  "wma_arm_version.h"
 
  @AREA    |.text|, CODE, READONLY
  .text .align 4

  .if WMA_OPT_CHANNEL_DOWNMIX_ARM == 1
  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  .globl  _prvChannelUpMix
  .globl  _prvChannelDownMix


@//*************************************************************************************
@//
@// WMARESULT prvChannelDownMix (CAudioObjectDecoder *paudec, U8 *piSourceBuffer, Int nBlocks, U8 *piDestBuffer)
@//
@//*************************************************************************************

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Registers for prvChannelDownMix
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

paudec            .req  r0
piSourceBuffer    .req  r1
nBlocks           .req  r2
piDestBuffer      .req  r3

iChDst            .req  r4
iChSrc            .req  r5
pcmsDst           .req  r6
tmpSrc            .req  r7
pcmsMixMtx        .req  r8

cMaxValue         .req  r9  
cMinValue         .req  r10

cSrcChannel       .req  r14
cDstChannel       .req  r11

pau               .req  r7
	.if	DEVICEPCMOUT16	==	0
BitDepth          .req  r8
	.endif
temp1             .req  r12
temp2             .req  r14
temp3             .req  r0


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Constants for prvChannelDownMix
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
.set iStackSpaceRev              , 2*4    
.set iOffset_cSrcChannel         , iStackSpaceRev-4
.set iOffset_rgrgfltChDnMixMtx   , iStackSpaceRev-8

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.if LINUX_RVDS == 1
  	PRESERVE8
	.endif
  	@AREA    |.text|, CODE
_prvChannelDownMix:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = paudec
@ r1 = piSourceBuffer
@ r2 = nBlocks
@ r3 = piDestBuffer


  stmfd sp!, {r4 - r12, lr}
  sub   sp, sp, #iStackSpaceRev @ rev stack space

@ if (!paudec->m_fChannelFoldDown)
  ldr   temp1, [paudec, #CAudioObjectDecoder_m_fChannelFoldDown]
  cmp   temp1, #0
  beq   prvChannelDownMix_Exit

@ CAudioObject* pau = paudec->pau;
  ldr   pau, [paudec, #CAudioObjectDecoder_pau]

@ Int cSrcChannel   = pau->m_cChannel;
@ Int cDstChannel   = paudec->m_cDstChannel;
  ldr   cDstChannel, [paudec, #CAudioObjectDecoder_m_cDstChannel]
  ldrh  cSrcChannel, [pau, #CAudioObject_m_cChannel]
	.if		DEVICEPCMOUT16	== 0
  ldr   BitDepth, [pau, #CAudioObject_m_iBitDepthSelector]
	.endif
@ if (cSrcChannel >= cDstChannel)
  cmp   cSrcChannel, cDstChannel
  bge   ChannelDownMix
  
@ prvChannelUpMix(paudec, piSourceBuffer, nBlocks, piDestBuffer)@
  bl    _prvChannelUpMix
  b     prvChannelDownMix_Exit

ChannelDownMix:

  cmp   nBlocks, #0
  beq   prvChannelDownMix_Exit

  ldr   temp1, [paudec, #CAudioObjectDecoder_m_rgrgfltChDnMixMtx]
  str   cSrcChannel, [sp, #iOffset_cSrcChannel]
  str   temp1, [sp, #iOffset_rgrgfltChDnMixMtx]

@ pau->m_iPCMSampleMin
  ldr   cMinValue, [pau, #CAudioObject_m_iPCMSampleMin]
  ldr   cMaxValue, [pau, #CAudioObject_m_iPCMSampleMax]
	.if	DEVICEPCMOUT16	==	0
@ if (pau->m_iBitDepthSelector == BITDEPTH_16)
  cmp   BitDepth, #0x3D   @ BITDEPTH_16
  bne   ChannelDownMix24
	.endif
ChannelDownMix16:
  mov   iChDst, #0

DstChannelLoop16:
@ for (iChDst = 0; iChDst < cDstChannel; iChDst++)
@ pcmsDst = 0;
  mov   pcmsDst, #0

@ tmpSrc = piSrc;
  mov   tmpSrc, piSourceBuffer

@ pcmsMixMtx = paudec->m_rgrgfltChDnMixMtx[iChDst]@
  ldr   temp1, [sp, #iOffset_rgrgfltChDnMixMtx]
  add   temp1, temp1, iChDst, LSL #2
  ldr   pcmsMixMtx, [temp1]

  ldr   iChSrc, [sp, #iOffset_cSrcChannel]

SrcChannelLoop16:
@ for (iChSrc = 0; iChSrc < cSrcChannel; iChSrc++)

@ pcmsTemp = *((I16*)tmpSrc)@
@ tmpSrc += 2;
  ldrsh temp1, [tmpSrc], #2
  ldr   temp2, [pcmsMixMtx], #4
  mov   temp1, temp1, LSL #9
  mov   temp3, #0
                     
@ pcmsDst += (PCMSAMPLE) MULT_CHDN(pcmsTemp, *pcmsMixMtx++)@
  smlal temp3, pcmsDst, temp2, temp1

  subs  iChSrc, iChSrc, #1
  bne   SrcChannelLoop16

@ PPPOSTCLIPRANGE(pcmsDst)@
  cmp   pcmsDst, cMaxValue
  movgt pcmsDst, cMaxValue

  cmp   pcmsDst, cMinValue
  movlt pcmsDst, cMinValue

@ *((I16*)piDst) = (I16)pcmsDst;
@ piDst += 2;
  strh  pcmsDst, [piDestBuffer], #2

  add   iChDst, iChDst, #1
  cmp   iChDst, cDstChannel
  blt   DstChannelLoop16

@ piSrc = tmpSrc;
  mov   piSourceBuffer, tmpSrc

  subs  nBlocks, nBlocks, #1
  bne   ChannelDownMix16
  
	.if	DEVICEPCMOUT16	==	0
  b     prvChannelDownMix_Exit

ChannelDownMix24:
  mov   iChDst, #0

DstChannelLoop24:
@ for (iChDst = 0; iChDst < cDstChannel; iChDst++)
@ pcmsDst = 0;
  mov   pcmsDst, #0

@ tmpSrc = piSrc;
  mov   tmpSrc, piSourceBuffer

@ pcmsMixMtx = paudec->m_rgrgfltChDnMixMtx[iChDst]@
  ldr   temp1, [sp, #iOffset_rgrgfltChDnMixMtx]
  add   temp1, temp1, iChDst, LSL #2
  ldr   pcmsMixMtx, [temp1]

  ldr   iChSrc, [sp, #iOffset_cSrcChannel]

SrcChannelLoop24:
@ for (iChSrc = 0; iChSrc < cSrcChannel; iChSrc++)

@ pcmsTemp = (*((U8*)tmpSrc) | (*((U8*)(tmpSrc + 1)) << 8) | (*((I8*)(tmpSrc + 2)) << 16))@
@ tmpSrc += 3;
  
  ldrb  temp1, [tmpSrc], #1
  ldrb  temp2, [tmpSrc], #1
  ldrb  temp3, [tmpSrc], #1

  add   temp1, temp1, temp2, LSL #8
  add   temp1, temp1, temp3, LSL #16
  ldr   temp2, [pcmsMixMtx], #4
  
  mov   temp1, temp1, LSL #8
  mov   temp2, temp2, LSL #1
                     
@ pcmsDst += (PCMSAMPLE) MULT_CHDN(pcmsTemp, *pcmsMixMtx++)@
  smlal temp3, pcmsDst, temp2, temp1

  subs  iChSrc, iChSrc, #1
  bne   SrcChannelLoop24

@ PPPOSTCLIPRANGE(pcmsDst)@
  cmp   pcmsDst, cMaxValue
  movgt pcmsDst, cMaxValue

  cmp   pcmsDst, cMinValue
  movlt pcmsDst, cMinValue

@ tmpDst = (U8*) &pcmsDst;
@ *((I8*)piDst) = *((I8*)tmpDst)@
@ *((I8*)(piDst + 1)) = *((I8*)(tmpDst + 1))@
@ *((I8*)(piDst + 2)) = *((I8*)(tmpDst + 2))@
@ piDst += 3;

  strb  pcmsDst, [piDestBuffer], #1
  mov   pcmsDst, pcmsDst, LSR #8
  strb  pcmsDst, [piDestBuffer], #1
  mov   pcmsDst, pcmsDst, LSR #8
  strb  pcmsDst, [piDestBuffer], #1

  add   iChDst, iChDst, #1
  cmp   iChDst, cDstChannel
  blt   DstChannelLoop24

@ piSrc = tmpSrc;
  mov   piSourceBuffer, tmpSrc

  subs  nBlocks, nBlocks, #1
  bne   ChannelDownMix24
	.endif
prvChannelDownMix_Exit:	
  mov   r0, #0

  add   sp, sp, #iStackSpaceRev     @ give back rev stack space  
  ldmfd sp!, {r4 - r12, PC} @ prvChannelDownMix
  @ENTRY_END prvChannelDownMix

  .endif @ WMA_OPT_CHANNEL_DOWNMIX_ARM


  
  .if WMA_OPT_REQUANTO16_ARM == 1

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  .globl  _prvRequantizeTo16


@//*************************************************************************************
@//
@// WMARESULT prvRequantizeTo16(CAudioObjectDecoder *paudec, U8 *piSrc, Int nBlocks)
@//
@//*************************************************************************************

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Registers for prvRequantizeTo16
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

paudecReoon       .req  r0
piSrcReoon        .req  r1
nBlocksRecon      .req  r2

piDstRecon        .req  r3
cDstChannelRecon  .req  r12
fReQuantizeTo16   .req  r14

T1Recon           .req  r12
T2Recon           .req  r14

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  @AREA    |.text|, CODE
_prvRequantizeTo16:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = paudec
@ r1 = piSrc
@ r2 = nBlocks


  str   lr,  [sp, #-4]!

@ Int cDstChannel = paudec->m_cDstChannel;
  ldr   cDstChannelRecon, [paudecReoon, #CAudioObjectDecoder_m_cDstChannel]

@ if (!paudec->m_fReQuantizeTo16)
@	 goto exit;
  ldr   fReQuantizeTo16, [paudecReoon, #CAudioObjectDecoder_m_fReQuantizeTo16]
  mul   T1Recon, nBlocksRecon, cDstChannelRecon 

  mov   piDstRecon, piSrcReoon
  cmp   fReQuantizeTo16, #0
  beq   prvRequantizeTo16_Exit

  cmp   nBlocksRecon, #0
  beq   prvRequantizeTo16_Exit

  mov   nBlocksRecon, T1Recon, LSR #1

BlockReconLoop:
@ for (iBlock = 0; iBlock < nBlocks*cDstChannel; iBlock++)
 
@	pcmsTemp = (*((U8*)(piSrc + 1))) | (*((I8*)(piSrc + 2)) << 8)@
@	*((I16*)piDst) = pcmsTemp;

@	piSrc += 3;
@	piDst += 2;
@   unloop by 2

  ldrb  T1Recon, [piSrcReoon, #1]
  ldrb  T2Recon, [piSrcReoon, #2]

  add   T1Recon, T1Recon, T2Recon, LSL #8
  strh  T1Recon, [piDstRecon], #2

  ldrb  T1Recon, [piSrcReoon, #4]
  ldrb  T2Recon, [piSrcReoon, #5]

  add   T1Recon, T1Recon, T2Recon, LSL #8
  add   piSrcReoon, piSrcReoon, #6
  strh  T1Recon, [piDstRecon], #2

  subs  nBlocksRecon, nBlocksRecon, #1
  bne   BlockReconLoop
    
prvRequantizeTo16_Exit:

  mov   r0, #0

  ldr   PC, [sp], #4 @ prvRequantizeTo16
  @ENTRY_END prvRequantizeTo16


  .endif @ WMA_OPT_REQUANTO16_ARM
  
  
