/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _MPEG_IMAGE_H_
#define _MPEG_IMAGE_H_
#include "voMpeg4DecGlobal.h"

#define SAFETY	64
#define EDGE_SIZE  32
#define EDGE_SIZE2  (EDGE_SIZE/2)

#define CACHE_LINE 64

#define ALIGN_MEM(out_mem){\
	if((VO_U32)(out_mem) & ~(CACHE_LINE - 1)){\
		out_mem = 	(VO_U8 *) ((VO_U32) (out_mem + CACHE_LINE - 1) &\
							 (~(VO_U32) (CACHE_LINE - 1)));\
	}\
}


void SwapImage(VO_IMGYUV ** iMgDst, VO_IMGYUV ** iMgSrc);
void ColoneImage(VO_CODEC_INIT_USERDATA * pUserData,  VO_U32 nCodecIdx, VO_IMGYUV * iMgDst,
				VO_IMGYUV * iMgSrc, VO_U32 ExWidth, VO_U32 nHeight);		
void SetImageEdge(VO_CODEC_INIT_USERDATA * pUserData,
				  VO_U32 nCodecIdx, VO_IMGYUV * image,
				   const VO_U32 ExWidth, const VO_U32 ExWidthUV,
			   const VO_U32 ExHeight, VO_U32 nWidth, VO_U32 nHeight);
			   
void *MallocMem(VO_CODEC_INIT_USERDATA * pUserData, VO_U32 nCodecIdx, VO_U32 nSize, VO_U32 nAlignment);
void SetMem(VO_CODEC_INIT_USERDATA * pUserData, VO_U32 nCodecIdx, VO_U8 *pDst, VO_U8 nValue, VO_U32 nSize);
void CopyMem(VO_CODEC_INIT_USERDATA * pUserData, VO_U32 nCodecIdx, VO_U8 *pDst, VO_U8* pSrc, VO_U32 nSize);
void FreeMem(VO_CODEC_INIT_USERDATA * pUserData, VO_U32 nCodecIdx, VO_PTR pDst);

#define FIFO_WRITE	0
#define FIFO_READ	1

VO_S32		GetFrameBufIdx(VO_IMGYUV* img, VO_IMGYUV* img_seq);
VO_IMGYUV*  FrameBufCtl(FIFOTYPE *priv, VO_IMGYUV* img , const VO_U32 flag);
//void  ResetFrameBuf(VO_MPEG4_DEC *pDec);


#endif							/* _MPEG_IMAGE_H_ */
