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
*		voMalloc.c
* 
* Abstact:	
*
*		Memory allocation and processing functions for Resample module.
*
* Author:
*
*		Witten Wen 2010-7-27
*
* Revision History:
*
******************************************************/

#include "resample.h"

//#define MEMORYSIZE
//#define TESTMEMORYLEAK
#ifdef TESTMEMORYLEAK
int MallocNumber = 0;
#endif

#ifdef MEMORYSIZE
size_t memorysize = 0;
#endif

VO_S32 LOG2(VO_U32 i)
{   // returns n where n = log2(2^n) = log2(2^(n+1)-1)
    VO_U32 iLog2 = 0;

    while ((i >> iLog2) > 1)
        iLog2++;

    return iLog2;
}

void *voauMalloc(VO_MEM_OPERATOR *vopMemOP, VO_S32 iSize)
{
	VO_MEM_INFO mem_info;
	VO_U32 hr;
#ifdef TESTMEMORYLEAK
	MallocNumber++;
	printf("malloc number: %d,\n", MallocNumber);
#endif	//TESTMEMORYLEAK

#ifdef MEMORYSIZE
	memorysize += iSize;
#endif
	mem_info.Flag = 0;
	mem_info.Size = iSize;
	hr = vopMemOP->Alloc(VO_INDEX_DEC_WMA, &mem_info);
	return (void *)mem_info.VBuffer;
}

void voauFree(VO_MEM_OPERATOR *vopMemOP, void *pFree)
{
	VO_U32 hr;
#ifdef TESTMEMORYLEAK
	MallocNumber--;
	printf("malloc number: %d,\n", MallocNumber);
#endif
	hr = vopMemOP->Free(VO_INDEX_DEC_WMA, pFree);
	pFree = NULL;
}

//*****************************************************************************************
//
// voMallocAligned & voFreeAligned
// allocates a buffer of size bytes aligned to iAlignToBytes bytes.
//
//*****************************************************************************************
void *voMallocAligned(VO_MEM_OPERATOR *vopMemOP, VO_S32 size,VO_S32 iAlignToBytes)
{
    VO_S32 mask = -1;                                    //Initally set mask to 0xFFFFFFFF
    void *retBuffer;// = voauMalloc(vopMemOP, size);
    void *buffer = voauMalloc(vopMemOP, size+iAlignToBytes);        //allocate buffer + alignment bytes
    if(buffer == NULL || iAlignToBytes < 4) // 4 here is an overconstraint. 2 should be fine.
    {
        if (buffer)
        {
            voauFree(vopMemOP, buffer);
            buffer = NULL;
        }
        return NULL;
    }
    mask <<= LOG2(iAlignToBytes);                     //Generate mask to clear lsb's
    retBuffer = (void*)(((VO_S32)((VO_U8*)buffer+iAlignToBytes))&mask);//Generate aligned pointer
    ((VO_U8*)retBuffer)[-1] = (VO_U8)((VO_U8*)retBuffer-(VO_U8*)buffer);//Write offset to newPtr-1
    return retBuffer;
} // voMallocAligned

void voFreeAligned(VO_MEM_OPERATOR *vopMemOP, void *ptr)
{
    VO_U8* realBuffer = (VO_U8*)ptr;
    VO_U8 bytesBack;
    if (realBuffer == NULL) return; 
    bytesBack = ((VO_U8*)ptr)[-1];      //Get offset to real pointer from -1 possition
    realBuffer -= bytesBack;    //Get original pointer address
    voauFree(vopMemOP, realBuffer);
} // voFreeAligned

#if !OPT_VOMEMORY_ARM
void *voMemset(void *pDest, int c, unsigned int size)
{
	char j = c & 0xff;
	unsigned int i = 0;
	char *pD = (char *)pDest;
	for(i =0; i< size; i++)
		pD[i] = j;
	return pDest;
}

void *voMemcpy (void *pDest, const void *pSrc, unsigned int size)
{
	char *pD = (char *)pDest;
	char *pS = (char *)pSrc;
	while(size--)
		*pD++ = *pS++;
	return pDest;
}
#endif	//OPT_VOMEMORY_ARM


void* voauCalloc(VO_MEM_OPERATOR *vopMemOP, VO_S32 iSize)
{
    void *pv = voauMalloc(vopMemOP, iSize);
    if (pv)
        pv = voMemset(pv, 0, iSize);
    return pv;
}

void* vocallocAligned(VO_MEM_OPERATOR *vopMemOP, VO_S32 iSize, VO_S32 iAlignToBytes)
{
    void *pv = voMallocAligned(vopMemOP, iSize, iAlignToBytes);
    if (pv)
        pv = voMemset(pv, 0, iSize);
    return pv;
}
