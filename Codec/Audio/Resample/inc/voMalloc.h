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
*		voMalloc.h
* 
* Abstact:	
*
*		Memory allocation and processing functions head file for Resample module.
*
* Author:
*
*		Witten Wen 2010-7-27
*
* Revision History:
*
******************************************************/

#ifndef __VO_MALLOC_H_
#define __VO_MALLOC_H_

#include "resample.h"

#ifdef ARM
#define OPT_VOMEMORY_ARM		1	//1 for enable voMemory.s
#endif

void *voauMalloc(VO_MEM_OPERATOR *vopMemOP, VO_S32 iSize);
void *voauCalloc(VO_MEM_OPERATOR *vopMemOP, VO_S32 iSize);

//*****************************************************************************************
//
// voMallocAligned & vocallocAligned
// allocates a buffer of size bytes aligned to iAlignToBytes bytes.
//
//*****************************************************************************************
void *voMallocAligned(VO_MEM_OPERATOR *vopMemOP, VO_S32 size,VO_S32 iAlignToBytes);
void *vocallocAligned(VO_MEM_OPERATOR *vopMemOP, VO_S32 iSize, VO_S32 iAlignToBytes);

void  voauFree(VO_MEM_OPERATOR *vopMemOP, void *pFree);
void  voFreeAligned(VO_MEM_OPERATOR *vopMemOP, void *ptr);

void *voMemset(void *pDest, int c, unsigned int size);
void *voMemcpy (void *pDest, const void *pSrc, unsigned int size);


//define the memory function
#define voRESMalloc(iSize)	voauMalloc(pRes->pvoMemop, iSize)
#define voRESCalloc(iSize)	voauCalloc(pRes->pvoMemop, iSize)

#define voRESMallocAligned(iSize, iAlignToBytes)	voMallocAligned(pRes->pvoMemop, iSize, iAlignToBytes)
#define voRESCallocAligned(iSize, iAlignToBytes)	vocallocAligned(pRes->pvoMemop, iSize, iAlignToBytes)

#define voRESFree(pFree)		voauFree(pRes->pvoMemop, pFree)
#define voRESFreeAligned(pFree)	voFreeAligned(pRes->pvoMemop, pFree)

#define voRESMemset		voMemset
#define voRESMemcpy		voMemcpy

#endif	//__VO_MALLOC_H_