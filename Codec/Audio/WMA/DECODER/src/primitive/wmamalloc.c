//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
// Contains functions needed for WMA Std, WMA Pro, and WMA Lossless.
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    wmamalloc.c

Abstract:

    Memory allocation functions

Author:

    Wei-ge Chen (wchen) 11-March-1998

Revision History:


*************************************************************************/

#include "macros.h"

#if !defined(__arm) && !defined(_XBOX) && !defined(LINUX) && !defined(_MAC_OS) && !defined(_IOS)
#include "windows.h"
#endif
#include "voWMADecID.h"
#include "wmacommon.h"
#include "wmamalloc.h"
#include "wmspecstring.h"
#include "limits.h"
#ifdef LINUX_ANDROID
#include "voLog.h"
#endif

#ifdef DEBUG_LINUX
#include <utils/Log.h>
#define LOG_TAG "wmamalloc"
#endif

#if defined(_XBOX)
#include <xtl.h>

#define WMA_XALLOC_ATTRIBS MAKE_XALLOC_ATTRIBUTES(  1,                            \
                                                    0,                            \
                                                    0,                            \
                                                    1,                            \
                                                    eXALLOCAllocatorId_XMV,       \
                                                    XALLOC_ALIGNMENT_16,          \
                                                    XALLOC_MEMPROTECT_READWRITE,  \
                                                    0,                            \
                                                    XALLOC_MEMTYPE_HEAP )

__forceinline LPVOID static malloc( size_t size ){ return XMemAlloc( size, WMA_XALLOC_ATTRIBS ); }
__forceinline   VOID static free  ( LPVOID ptr  ){		  XMemFree ( ptr , WMA_XALLOC_ATTRIBS ); }
#endif

// This macro is needed in common audio macros!
#if !defined (INLINE)
#define PLATFORM_SPECIFIC_INLINE
#if _MSC_VER >= 1200            /* VC6.0 == 1200, VC5.0 == 1100 */
#define INLINE __forceinline
#else  /* _MSC_VER */
#define INLINE __inline
#endif /* _MSC_VER */
#endif // INLINE
#if 0
INLINE I32 LOG2(U32 i)
{   // returns n where n = log2(2^n) = log2(2^(n+1)-1)
    U32 iLog2 = 0;
    assert (i != 0);

    while ((i >> iLog2) > 1)
        iLog2++;

    return iLog2;
}
#endif

//*****************************************************************************************
//
// WANT_HEAP_MEASURE defines the memory tracing functions
// It doens't work with RT memory allocation for MF.
// Only serve for testing and looking for memory leak purposes.
//
//*****************************************************************************************


#if !defined(WANT_HEAP_MEASURE)

//*****************************************************************************************
//
// mallocAligned & freeAligned
// allocates a buffer of size bytes aligned to iAlignToBytes bytes.
//
//*****************************************************************************************
void *voMallocAligned(VO_MEM_OPERATOR *vopMemOP, size_t size,Int iAlignToBytes)
{
    I32UP mask = -1;                                    //Initally set mask to 0xFFFFFFFF
    void *retBuffer;// = voauMalloc(vopMemOP, size);
    void *buffer = voauMalloc(vopMemOP, size+iAlignToBytes);        //allocate buffer + alignment bytes
    if(buffer == NULL || iAlignToBytes < 4) // 4 here is an overconstraint. 2 should be fine.
    {
        if (buffer)
        {
            voauFree(vopMemOP, buffer);
            buffer = NULL;
        }
//        TraceResult(WMA_E_OUTOFMEMORY);
        return NULL;
    }
    assert(iAlignToBytes > 0);
    assert(iAlignToBytes < 256);
    mask <<= LOG2(iAlignToBytes);                     //Generate mask to clear lsb's
    retBuffer = (void*)(((I32UP)((U8*)buffer+iAlignToBytes))&mask);//Generate aligned pointer
    ((U8*)retBuffer)[-1] = (U8)((U8*)retBuffer-(U8*)buffer);//Write offset to newPtr-1
    return retBuffer;
} // mallocAligned

void voFreeAligned(VO_MEM_OPERATOR *vopMemOP, void *ptr)
{
    U8* realBuffer = (U8*)ptr;
    U8 bytesBack;
    if (realBuffer == NULL) return; 
    bytesBack = ((U8*)ptr)[-1];      //Get offset to real pointer from -1 possition
    realBuffer -= bytesBack;    //Get original pointer address
    voauFree(vopMemOP, realBuffer);
} // freeAligned


// ----- Memory Allocation Functions -----
#ifdef STATIC_MEMALLOC
#include "wmartmacros.h"
// Global vars
AVRT_DATA static MEMALLOCSTATE    g_maState = MAS_DELETE;
AVRT_DATA static U8              *g_pBuffer = NULL;
AVRT_DATA static U32              g_nBufRemaining = 0;
AVRT_DATA static I32              g_iAllocCount = 0;

I32 auMallocGetCount(void)
{
    return g_iAllocCount;
}

Void prvUESGetGlobalBuf(U8 **pbuf, I32 *pBufSize)
{
    *pbuf = g_pBuffer;
    *pBufSize = g_nBufRemaining;
    return;
}
void auMallocSetState(const MEMALLOCSTATE maState, void *pBuf, const U32 nBufSize)
{
    switch (maState)
    {
        case MAS_ALLOCATE:
            assert(MAS_DELETE == g_maState);
            g_pBuffer = (U8 *) pBuf;
            g_nBufRemaining = nBufSize;
            g_maState = MAS_ALLOCATE;
            break;

        case MAS_LOCKED:
            assert(MAS_ALLOCATE == g_maState);
            g_maState = MAS_LOCKED;
            break;

        case MAS_DELETE:
            g_maState = MAS_DELETE;
            break;

        default:
            assert(0);
            break;
    }
} // auMallocSetState


void * __stdcall auMalloc(const size_t iSize)
{
    WMARESULT   wmaResult = WMA_OK;
    void       *pResult = NULL;

    switch (g_maState)
    {
        case MAS_ALLOCATE:

            if (NULL == g_pBuffer)
            {
                pResult = malloc(iSize);
                if (NULL == pResult)
                    wmaResult = TraceResult(WMA_E_OUTOFMEMORY);
            }
            else
            {
                if (iSize <= (U32) g_nBufRemaining)
                {
                    pResult = g_pBuffer;
                    g_pBuffer += iSize;
                    g_nBufRemaining -= iSize;
                }
                else
                {
                    wmaResult = TraceResult(WMA_E_OUTOFMEMORY);
                    assert(0);
                }
            }

            g_iAllocCount += iSize;
            break;

        default:
            wmaResult = TraceResult(WMA_E_FAIL);
            assert(0); // We should only be called during allocation
            break;
    }

    return pResult;
} // auMalloc

void __stdcall auFree(void *pFree)
{
    assert(MAS_DELETE == g_maState);
    if (NULL == g_pBuffer)
        free(pFree);
}
#else   // STATIC_MEMALLOC

I32 auMallocGetCount(void)
{
    return 0;
}
Void prvUESGetGlobalBuf(U8 **pbuf, I32 *pBufSize)
{
}
void auMallocSetState(const MEMALLOCSTATE maState, void *pBuf, const U32 nBufSize)
{
}

#ifndef AUMALLOC_RT_HEAP

//#define MEMORYSIZE
//#define TESTMEMORYLEAK
#ifdef TESTMEMORYLEAK
int MallocNumber = 0;
#endif

#ifdef MEMORYSIZE
size_t memorysize = 0;
#endif

void *voauMalloc(VO_MEM_OPERATOR *vopMemOP, const size_t iSize)
{
	VO_MEM_INFO mem_info;
	VO_U32 hr;

#ifdef MEMORYSIZE
	memorysize += iSize;
#endif
	mem_info.Flag = 0;
	mem_info.Size = iSize;
	hr = vopMemOP->Alloc(VO_INDEX_DEC_WMA, &mem_info);
#ifdef TESTMEMORYLEAK
	MallocNumber++;
#ifdef DEBUG_LINUX
//	LOGW("malloc number: %d,\n", MallocNumber);
//	LOGW("Malloc addr:0x%x", (VO_S32)mem_info.VBuffer);
#else
	printf("malloc number: %d,\n", MallocNumber);
#endif	//DEBUG_LINUX
#endif	//TESTMEMORYLEAK
	return (void *)mem_info.VBuffer;
}


void voauFree(VO_MEM_OPERATOR *vopMemOP, void *pFree)
{
	VO_U32 hr;
#ifdef TESTMEMORYLEAK
	MallocNumber--;
#ifdef DEBUG_LINUX
//	LOGW("malloc number: %d,\n", MallocNumber);
//	LOGW("Free addr:0x%x", (VO_S32)pFree);
#else
	printf("malloc number: %d,\n", MallocNumber);
#endif	//DEBUG_LINUX
#endif	//TESTMEMORYLEAK
	hr = vopMemOP->Free(VO_INDEX_DEC_WMA, pFree);
}

void *voMemmove(VO_MEM_OPERATOR *vopMemOP, void *pDest, void *pSrc, size_t size)
{
	VO_U32 hr;
	hr = vopMemOP->Move(VO_INDEX_DEC_WMA, pDest, pSrc, size);
	return pDest;
}

#endif//AUMALLOC_RT_HEAP

#endif  // STATIC_MEMALLOC


#else //Implement Heap/Stack Tracing Functions, 
I32 auMallocGetCount(void)
{
    return 0;
}
Void prvUESGetGlobalBuf(U8 **pbuf, I32 *pBufSize)
{
}
void auMallocSetState(const MEMALLOCSTATE maState, void *pBuf, const U32 nBufSize)
{
}


#endif //WANT_STACK_MEASURE

void* voauCalloc(VO_MEM_OPERATOR *vopMemOP, const size_t iSize)
{
    void *pv = voauMalloc(vopMemOP, iSize);
    if (pv)
        pv = memset(pv, 0, iSize);
    return pv;
}

void* vocallocAligned(VO_MEM_OPERATOR *vopMemOP, size_t iSize, WMA_I32 iAlignToBytes)
{
    void *pv = voMallocAligned(vopMemOP, iSize, iAlignToBytes);
    if (pv)
        pv = memset(pv, 0, iSize);
    return pv;
}
