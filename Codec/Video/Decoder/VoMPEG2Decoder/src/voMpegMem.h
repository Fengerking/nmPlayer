/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _MEM_ALIGN_H_
#define _MEM_ALIGN_H_

#include "voMpegPort.h"
#include "voMpeg2Decoder.h"

#define CACHE_LINE 32
#define SAFETY	32

VO_VOID *MallocMem(VO_S32 size, VO_U8 alignment,VO_MEM_OPERATOR *pMemOperator);
VO_VOID FreeMem(VO_VOID *mem_ptr,VO_MEM_OPERATOR *pMemOperator);
VO_VOID SetMem(VO_U8 *pDst, VO_U8 nValue, VO_U32 nSize, VO_MEM_OPERATOR *pMemOperator);
VO_VOID MoveMem(VO_U8 *pDst, VO_U8* pSrc, VO_U32 nSize, VO_MEM_OPERATOR *pMemOperator);
VO_VOID CopyMem(VO_U8 *pDst, VO_U8* pSrc, VO_U32 nSize, VO_MEM_OPERATOR *pMemOperator);

#endif							/* _MEM_ALIGN_H_ */
