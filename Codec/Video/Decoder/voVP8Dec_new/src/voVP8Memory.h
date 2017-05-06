/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
 
#ifndef __VP8_DEC_MEMORY_H
#define __VP8_DEC_MEMORY_H
#include "voVideo.h"

#define CACHE_LINE 64
typedef struct 
{
	unsigned int nPictureWidth;					/*!< picture nWidth offered outside*/
	unsigned int nPictureHeight;				/*!< picture nHeight offered outside*/
	unsigned int nNumRefFrames;					/*!< number of reference and reconstruction  frames */
	unsigned int nPictureEXWidth;				/*!< including padding */
	unsigned int nPictureEXHeight;				/*!< including padding */
	unsigned int nPrivateMemSize;				/*!< size in byte */
	unsigned char *frameMem[4][3];
	unsigned char *pPrivateMem;
}VO_VIDEO_INNER_MEM;

void *MallocMem(VO_CODEC_INIT_USERDATA * pUserData, VO_U32 nCodecIdx, VO_U32 nSize, VO_U32 nAlignment);
void FreeMem(VO_CODEC_INIT_USERDATA * pUserData, VO_U32 nCodecIdx, VO_PTR pDst);
void SetMem(VO_CODEC_INIT_USERDATA * pUserData, VO_U32 nCodecIdx, VO_U8 *pDst, VO_U8 nValue, VO_U32 nSize);
void CopyMem(VO_CODEC_INIT_USERDATA * pUserData, VO_U32 nCodecIdx, VO_U8 *pDst, VO_U8* pSrc, VO_U32 nSize);


#endif