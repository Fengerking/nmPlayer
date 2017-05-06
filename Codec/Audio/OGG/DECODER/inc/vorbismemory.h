//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    vorbismemory.h

Abstract:

    Ogg Vorbis Memory Operation function declare header file.

Author:

    Witten Wen 07-September-2009

Revision History:

*************************************************************************/

#ifndef __VORBIS_MEMORY_H_
#define __VORBIS_MEMORY_H_

#include "voOGG.h"
#include "voOGGDecID.h"

#define DECODER_TYPE_ID	VO_INDEX_DEC_OGG
#if defined(WINCE) && !defined(ARM) && !defined(_IOS)
#define ARM_OPT
#endif
#ifdef ARM_OPT
#define OPT_VOMEMORY_ARM		1	//1 enable arm memory
#define OPT_IMDCT_ARM			1
#endif

extern void *voMalloc(VO_MEM_OPERATOR *vopMemOP, const VO_U32 iSize);
extern void *voRealloc(VO_MEM_OPERATOR *vopMemOP, void *ptr, const VO_U32 iSize);
extern void	 voFree(VO_MEM_OPERATOR *vopMemOP, void *pFree);
extern void *voCalloc(VO_MEM_OPERATOR *vopMemOP, const VO_U32 Count, const VO_U32 iSize);
extern long voMemcmp(VO_MEM_OPERATOR *vopMemOP, void *ptr1, void *ptr2, VO_U32 count);

#define voOGGMalloc(iSize)				voMalloc(pvorbisdec->vopMemOP, iSize)
#define voOGGRealloc(ptr, iSize)		voRealloc(pvorbisdec->vopMemOP, ptr, iSize)
#define voOGGFree(pFree)				voFree(pvorbisdec->vopMemOP, pFree)
#define voOGGCalloc(Count, iSize)		voCalloc(pvorbisdec->vopMemOP, Count, iSize)
#define voOGGMemcmp(ptr1, ptr2, count)	voMemcmp(pvorbisdec->vopMemOP, ptr1, ptr2, count)

extern void *voMemset(void *pDest, int c, VO_U32 size);
extern void *voMemcpy (void *pD, const void *pS, VO_U32 size);
extern void *voMemchr(void *pSrc, int Value, int size);

#define	voOGGMemset		voMemset
#define	voOGGMemcpy		voMemcpy
#define voOGGMemchr		voMemchr
#endif //__VORBIS_MEMORY_H_