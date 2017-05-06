/**********************************************************************
*
* VisualOn voplayer
*
* CopyRight (C) VisualOn SoftWare Co., Ltd. All rights are reserved.
*
***********************************************************************/

/*********************************************************************
*
* Filename:	
*		alacstrmdec.h
* 
* Abstact:	
*
*		Apple Lossless Audio Codec for VisualOn Decoder read stream header file.
*
* Author:
*
*		Witten Wen 1-April-2010
*
* Revision History:
*
******************************************************/

#ifndef __ALAC_STRM_DEC_H_
#define __ALAC_STRM_DEC_H_
#include "vomemory.h"
#include "voalacdec.h"

VO_VOID	ReadStream(CALACObjectDecoder *palacdec, VO_U32 iSize, VO_VOID *pBuffer);
VO_U16 ReadU16(CALACObjectDecoder *palacdec);
VO_U32 ReadU32(CALACObjectDecoder *palacdec);

VO_U32	ReadBit(CALACBitStream *pstrmbits);
VO_U32	ReadBits16(CALACBitStream *pstrmbits, int bits);
VO_U32	ReadBits32(CALACBitStream *pstrmbits, int bits);

VO_VOID SkipBits(CALACBitStream *pstrmbits, int bits);
VO_VOID	UnReadBits(CALACBitStream *pstrmbits, int bits);

#define SWAPBYTE32(v)	( \
	(((v) & 0x000000FF) << 0x18) | \
	(((v) & 0x0000FF00) << 0x08) | \
	(((v) & 0x00FF0000) >> 0x08) | \
	(((v) & 0xFF000000) >> 0x18))

#define SWAPBYTE16(v)	( \
	(((v) & 0x00FF) << 0x8) | \
	(((v) & 0xFF00) >> 0x8))

#ifndef MAKEFOURC
#define MAKEFOURC(x0, x1, x2, x3) ( \
	( (VO_U32)(x0) << 24 ) | \
	( (VO_U32)(x1) << 16 ) | \
	( (VO_U32)(x2) << 8 ) | \
	( (VO_U32)(x3) ) )
#endif




#endif	//__ALAC_STRM_DEC_H_

