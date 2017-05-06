//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    vomemory.h

Abstract:

    VisualOn Memory Operation function declare header file.

Author:

    Witten Wen 07-September-2009

Revision History:

*************************************************************************/
#ifndef __VORBIS_MEMORY_H_
#define __VORBIS_MEMORY_H_

#include "voALAC.h"

#define DECODER_TYPE_ID	VO_AUDIO_CodingALAC

#ifdef ARM
#define OPT_VOMEMORY_ARM		0	//1 enable arm memory 
#endif

extern void *voMalloc(VO_MEM_OPERATOR *vopMemOP, const VO_U32 iSize);
extern void *voRealloc(VO_MEM_OPERATOR *vopMemOP, void *ptr, const VO_U32 iSize);
extern void	 voFree(VO_MEM_OPERATOR *vopMemOP, void *pFree);
extern void *voCalloc(VO_MEM_OPERATOR *vopMemOP, const VO_U32 Count, const VO_U32 iSize);
extern long voMemcmp(VO_MEM_OPERATOR *vopMemOP, void *ptr1, void *ptr2, VO_U32 count);

#define voALACMalloc(iSize)				voMalloc(palacdec->vopMemOP, iSize)
#define voALACRealloc(ptr, iSize)		voRealloc(palacdec->vopMemOP, ptr, iSize)
#define voALACFree(pFree)				voFree(palacdec->vopMemOP, pFree)
#define voALACCalloc(Count, iSize)		voCalloc(palacdec->vopMemOP, Count, iSize)
#define voALACMemcmp(ptr1, ptr2, count)	voMemcmp(palacdec->vopMemOP, ptr1, ptr2, count)

extern void *voMemset(void *pDest, int c, VO_U32 size);
extern void *voMemcpy (void *pD, const void *pS, VO_U32 size);
extern void *voMemchr(void *pSrc, int Value, int size);

#define	voALACMemset		voMemset
#define	voALACMemcpy		voMemcpy
#define voALACMemchr		voMemchr
#endif //__VORBIS_MEMORY_H_

