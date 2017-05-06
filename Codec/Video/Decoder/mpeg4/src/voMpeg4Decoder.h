/**                                                                                                                             
* \file         MP4VO_INDEX_DEC_H263_API.h                                                                                         
* \brief    API definition for the VisualOn MPEG-4 and H.263 decoder    
* \version 0.1  
*/

#ifndef _VOMP4VO_INDEX_DEC_H263_H_
#define _VOMP4VO_INDEX_DEC_H263_H_

#include "voMpeg4DecGlobal.h"
#include "voMpeg4Dec.h"
  
#define DEC_FRAME		0
#define DEC_VOLHR		1
#define DEC_FRAMETYPE	2
#define DEC_HEADINFO	3

extern void GetInnerMemSize(VO_VIDEO_INNER_MEM * pInnerMem, VO_U32 nMemConfig, VO_U32 nFrameNum);	
extern VO_S32 ConfigDecoder(VO_MPEG4_DEC* pDec,  VO_S32 nWidth, VO_S32 nHeight);

extern VO_U32 voMPEGDecFrame(VO_MPEG4_DEC *pDec, VO_CODECBUFFER *pInData, 
							 VO_VIDEO_BUFFER *pOutData, VO_VIDEO_OUTPUTINFO *pOutPutInfo, 
							 const VO_U32 nHeaderFlag);
extern VO_U32 voMPEG4DecCreate(void **pHandle, VO_CODEC_INIT_USERDATA * pUserData, VO_U32 nCodecId);
extern VO_U32 voMPEG4DecFinish(VO_MPEG4_DEC* pDec);

/* */
#define INNER_MEM_DATA_PARTITION	1
#define INNER_MEM_FRAME_MEME		2
#define INNER_MEM_FRONT_HANDLE		4
#define INNER_MEM_DEC_HANDLE		8

#define INNER_MEM_MAX		(INNER_MEM_DATA_PARTITION | INNER_MEM_FRAME_MEME |\
							INNER_MEM_FRONT_HANDLE | INNER_MEM_DEC_HANDLE)

#endif
