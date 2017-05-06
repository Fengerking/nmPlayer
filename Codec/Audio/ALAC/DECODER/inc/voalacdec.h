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
*		voalacdec.h
* 
* Abstact:	
*
*		Apple Lossless Audio Codec for VisualOn Decoder define header.
*
* Author:
*
*		Witten Wen 24-March-2010
*
* Revision History:
*
******************************************************/

#ifndef __VO_ALAC_DEC_H_
#define __VO_ALAC_DEC_H_

#include "voALAC.h"

typedef		VO_U32		fourc_t;

typedef struct CALACBitStream{
	VO_U8		*m_ptr;
	VO_U32		m_UsedBits;
	VO_U32		m_Length;
}CALACBitStream;

typedef struct CALACSetInfo{
	/* Codec Data Info */
	VO_U32		m_MaxSamplesPerFrame;
	VO_U8		m_Info7a;
	VO_U8		m_SampleSize;
	VO_U8		m_RiceHistorymult;
	VO_U8		m_RiceInitHistory;
	VO_U8		m_RiceKmodifier;
	VO_U8		m_Info7f;
	VO_U16		m_Info80;
	VO_U32		m_Info82;
	VO_U32		m_Info86;
	VO_U32		m_Info8aRate;

	/* Buffer */
	VO_S32		*m_pErrorBuffA;
	VO_S32		*m_pErrorBuffB;
	VO_S32		*m_pOutBuffA;
	VO_S32		*m_pOutBuffB;
	VO_S32		*m_pUncprsseBytsBufA;
	VO_S32		*m_pUncprsseBytsBufB;
}CALACSetInfo;

typedef struct CALACObjectDecoder{
	VO_U16		m_Channels;
	VO_U16		m_SampleBits;
	VO_U32		m_SampleRate;
	VO_U32		m_DatLen;

	/* demux codec data */
	VO_U32		m_CodecDataLen;
	VO_U8		*m_pCodecData;

//	fourc_t		m_Format;
	VO_BOOL		m_Header_OK;
	
//	int			m_ReadFormat;		

	CALACSetInfo *m_pALACInfo;

	VO_U8		*m_pInBuffer;
	VO_U8		*m_pOutBuffer;
	VO_U32		m_InSize;
	VO_U32		m_OutSize;
	CALACBitStream	*m_pBitStream;

	VO_MEM_OPERATOR *	vopMemOP;

	//voCheck lib need
	VO_PTR				hCheck;
}CALACObjectDecoder;

VO_VOID	ALACSetInfo(CALACObjectDecoder *palacdec);
VO_S32 voALACDecodeFrame(CALACObjectDecoder *palacdec, VO_PBYTE outbuffer, VO_U32 *outlength);
int CountHeadZeros(int input);
#endif	//__VO_ALAC_DEC_H_
