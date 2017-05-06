//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    vosbcmemory.cpp

Abstract:

    VisualOn SBC memory opration file.

Author:

    Witten Wen 04-September-2009

Revision History:

*************************************************************************/

#include "voSBCmemory.h"

#define MEMORYSIZE
//#define TESTMEMORY
#ifdef TESTMEMORY
int memory_number = 0;
#endif
#ifdef MEMORYSIZE
int memorysize = 0;
#endif
void *voMalloc(VO_MEM_OPERATOR *vopMemOP, const VO_U32 iSize)
{
	VO_MEM_INFO mem_info;
#ifdef TESTMEMORY
	if(iSize == 6922)
		mem_info.Size = iSize;
	memory_number ++;
	printf("malloc number: %d;\n", memory_number);
#endif
#ifdef MEMORYSIZE
	memorysize += iSize;
#endif
	mem_info.Flag = 0;
	mem_info.Size = iSize;
	vopMemOP->Alloc(DECODER_TYPE_ID, &mem_info);
	return (void *)mem_info.VBuffer;
}

void *voCalloc(VO_MEM_OPERATOR *vopMemOP, const VO_U32 Count, const VO_U32 iSize)
{
	VO_U32 size = Count * iSize;
	void *ptr = voMalloc(vopMemOP, size);
	if(!ptr)
		return NULL;
	voMemset(ptr, 0, size);
	return ptr;
}

void voFree(VO_MEM_OPERATOR *vopMemOP, void *pFree)
{
	vopMemOP->Free(DECODER_TYPE_ID, pFree);
#ifdef TESTMEMORY
	memory_number--;
	printf("malloc number: %d;\n", memory_number);
#endif
}

void *voRealloc(VO_MEM_OPERATOR *vopMemOP, void *ptr, const VO_U32 iSize)
{
	if(ptr)
		voFree(vopMemOP, ptr);
	return voMalloc(vopMemOP, iSize);
}

long voMemcmp(VO_MEM_OPERATOR *vopMemOP, void *ptr1, void *ptr2, VO_U32 count)
{
	return vopMemOP->Compare(DECODER_TYPE_ID, ptr1, ptr2, count);
}

void *voMemmove(VO_MEM_OPERATOR *vopMemOP, void *pDest, void *pSrc, VO_U32 size)
{
	vopMemOP->Move(DECODER_TYPE_ID, pDest, pSrc, size);
	return pDest;
}

void *voMemchr(void *pSrc, int Value, int size)
{
	char *pS = (char *)pSrc;
	if(pSrc == NULL)
		return NULL;
	while(size--)
		if(*pS++ == (char)Value)
			return pS-1;
	return NULL;
}

#if !OGG_OPT_VOMEMORY_ARM
void *voMemset(void *pDest, int c, VO_U32 size)
{
	char j = c & 0xff;
	VO_U32 i = 0;
	char *pD = (char *)pDest;
	for(i =0; i< size; i++)
		pD[i] = j;
	return pDest;
//	return memset(pDest, c, size);
}
void *voMemcpy (void *pD, const void *pS, VO_U32 size)
{
	char *pDest = (char *)pD;
	char *pSrc = (char *)pS;
	while(size--)
		*pDest++ = *pSrc++;
	return pDest;
}

#endif




