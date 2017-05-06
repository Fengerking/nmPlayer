//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    voSBCmemory.h

Abstract:

    VisualOn SBC Memory Operation function declare header file.

Author:

    Witten Wen 8-December-2009

Revision History:
	
*************************************************************************/

#ifndef __VO_SBC_MEMORY_H_
#define __VO_SBC_MEMORY_H_

#include "voSBC.h"

#define DECODER_TYPE_ID	VO_INDEX_DEC_WMA
#ifdef ARM
#define OGG_OPT_VOMEMORY_ARM	0
#else
#define OGG_OPT_VOMEMORY_ARM	0
#endif
extern void *voMalloc(VO_MEM_OPERATOR *vopMemOP, const VO_U32 iSize);
extern void *voRealloc(VO_MEM_OPERATOR *vopMemOP, void *ptr, const VO_U32 iSize);
extern void	 voFree(VO_MEM_OPERATOR *vopMemOP, void *pFree);
extern void *voCalloc(VO_MEM_OPERATOR *vopMemOP, const VO_U32 Count, const VO_U32 iSize);
extern long voMemcmp(VO_MEM_OPERATOR *vopMemOP, void *ptr1, void *ptr2, VO_U32 count);
extern void *voMemmove(VO_MEM_OPERATOR *vopMemOP, void *pDest, void *pSrc, VO_U32 size);

#define voSBCMalloc(iSize)				voMalloc(psbcdec->vopMemOP, iSize)
#define voSBCRealloc(ptr, iSize)		voRealloc(psbcdec->vopMemOP, ptr, iSize)
#define voSBCFree(pFree)				voFree(psbcdec->vopMemOP, pFree)
#define voSBCCalloc(Count, iSize)		voCalloc(psbcdec->vopMemOP, Count, iSize)
#define voSBCMemcmp(ptr1, ptr2, count)	voMemcmp(psbcdec->vopMemOP, ptr1, ptr2, count)
#define voSBCMemmove(pDest, pSrc, size) voMemmove(psbcdec->vopMemOP, pDest, pSrc, size)

extern void *voMemset(void *pDest, int c, VO_U32 size);
extern void *voMemcpy (void *pD, const void *pS, VO_U32 size);
extern void *voMemchr(void *pSrc, int Value, int size);

#define	voSBCMemset		voMemset
#define	voSBCMemcpy		voMemcpy
#define voSBCMemchr		voMemchr

#endif //__VO_SBC_MEMORY_H_

