//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    MsAudio.h

Abstract:

    Implementation of public member functions for CAudioObject.

Author:

    Wei-ge Chen (wchen) 11-March-1998

Revision History:


*************************************************************************/

#ifndef __WMAMALLOC_H_
#define __WMAMALLOC_H_

// ***********************************************

#include <stdio.h>
#include <limits.h>
#ifndef BUILD_WITHOUT_C_LIB
//#include <stdlib.h>
//#include "wmatyps.h"
#endif	//BUILD_WITHOUT_C_LIB
#ifndef _WMA_USED_EXTERNALLY_
#include "wmatypes.h"
#endif //_WMA_USED_EXTERNALLY_
#include "voType.h"
#include "voMem.h"
#include "commontables.h"


#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#ifdef BUILD_WITHOUT_C_LIB

//#define USE_DIVIDE_FUNC 1

#define longjmp(...) 
#define setjmp(...) 0

#if defined (_DEBUG)
#define sprintf(...)
#define printf(...)
#define fprintf(...)
#define fopen(...)
#define fflush(...)
#define fclose(...)
#endif

#define jmp_buf int
#ifndef RVDS
#define size_t unsigned int
#endif
#define clock_t int

//#define CLOCKS_PER_SEC 1000
//#define DBL_EPSILON 	2.2204460492503131e-016 /* smallest such that 1.0+DBL_EPSILON != 1.0 */
#define INT_MAX		0x7fffffff		/* maximum (signed) int value */
#if !defined(ARM) || defined(LINUX) || defined(_IOS) || defined(_MAC_OS)
#define voFLT_MAX         3.402823466e+38F        /* max value */	
#define voFLT_MIN         1.175494351e-38F        /* min positive value */
#endif
#if defined(__arm) || defined(LINUX) || defined(_IOS) || defined(_MAC_OS)
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a, b)  (((a) < (b)) ? (a) : (b))
#define __max max
#endif //__arm

#endif//BUILD_WITHOUT_C_LIB

#if defined(LINUX)
#define TEXT(x)	x
#define __max max
#endif
	
#if defined (_IOS) || defined(_MAC_OS)
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define __max max
#endif

#ifndef __WIN32
//#define cos(x) (1 - x)
//#define sqrt(x) (x)			
//#define sin(x) (x)			
//#define exp(x) (0)	//x¡Ö£­¡Þ	
//#define log(x)	(1)
#endif //__WIN32

#include "macros.h"

// This defines the Heap category if want to trace specific Heap allocated for specific component
#define TOTAL_HEAP_CATEGORY     1

// ----- Memory Allocation Functions -----
typedef enum {
	MAS_ALLOCATE, 
	MAS_LOCKED, 
	MAS_DELETE, 
	MAX_MEMALLOCSTATE_VALUE = MAX_VOENUM_VALUE
} MEMALLOCSTATE;

WMA_I32 auMallocGetCount(void);
void auMallocSetState(const MEMALLOCSTATE maState, void *pBuf, const WMA_U32 iBufSize);

//
// Turn on WANT_HEAP_MEASURE and WANT_STACK_MEASURE for Debug Build Only.
// No available for tracing RT Heap
//
#if !defined(AUMALLOC_RT_HEAP) && (defined(DEBUG)||defined(_DEBUG)) && defined(UNDER_CE)
//#define WANT_HEAP_MEASURE
//#define WANT_STACK_MEASURE
#endif

//#define WMAMALLOC_PRTMEM

#if defined(WMAMALLOC_PRTMEM)

extern Int g_size;
#define WMAMALLOC_NUMSIZE (10)
#define WMAMALLOC_COUNT_DECL Int wmaMallocSize[WMAMALLOC_NUMSIZE]={0};
#define WMAMALLOC_COUNT_STORE(i) \
{ \
  assert(i < WMAMALLOC_NUMSIZE); \
  wmaMallocSize[i] = g_size; \
}
#define WMAMALLOC_COUNT_PRT(str, start, end) \
  printf("%s: %d\n", str, \
         (end>=0 ? wmaMallocSize[end] : g_size) - \
         (start>=0 ? wmaMallocSize[start] : 0) )

#else // defined(WMAMALLOC_PRTMEM)

#define WMAMALLOC_COUNT_DECL
#define WMAMALLOC_COUNT_STORE(i)
#define WMAMALLOC_COUNT_PRT(str, start, end)

#endif // defined(WMAMALLOC_PRTMEM)

#ifndef WANT_HEAP_MEASURE

//define empty macros for the sake of implementing Heap/Stack Tracing
#define START_MEASURE_MEM(log_file)
#define WRITE_MEM_LOG()
#define GET_MEM_STATS(PeakHeapSize, MemLeakSize)

//mallocAligned allocates a buffer of size (+iAlignToBytes for allignment padding) bytes 
//which is alligned on an iAllignToBytes byte boundary
//the buffer must be deallocated using freeAligned
//and it is only safe to use size bytes as the padding could be at the begining or ending
void *voMallocAligned(VO_MEM_OPERATOR *vopMemOP, size_t size,Int iAlignToBytes);
void voFreeAligned(VO_MEM_OPERATOR *vopMemOP, void *ptr);

void *voauMalloc(VO_MEM_OPERATOR *vopMemOP, const size_t iSize);
void voauFree(VO_MEM_OPERATOR *vopMemOP, void *pFree);
void *voMemmove(VO_MEM_OPERATOR *vopMemOP, void *pDest, void *pSrc, size_t size);
extern void *voWMAMemset(void *pSrc, int n, unsigned int size);
extern void *voWMAMemcpy(void *pDest, const void *pSrc, unsigned int size);

#define mallocAligned(size, iAlignToBytes) voMallocAligned(paudec->vopMemOP, size, iAlignToBytes)
#define freeAligned(ptr)	voFreeAligned(paudec->vopMemOP, ptr)
#define auMalloc(iSize)		voauMalloc(paudec->vopMemOP, iSize)
#define auFree(pFree)		voauFree(paudec->vopMemOP, pFree)
#define auMemmove(pDest, pSrc, size)	voMemmove(paudec->vopMemOP, pDest, pSrc, size)
//#define memset voWMAMemset
#ifdef RVDS
#define memcpy voWMAMemcpy
#endif
#else
//
//Heap Traceing functions override
//
#include <string.h>

void * prvMalloc(size_t pSize, char *pFile, int pLine, int category);
void   prvFree(void *pMem, char *pFile, int pLine);
void * prvMallocAligned(size_t size, Int iAlignToBytes, char *pFile, int pLine);
void   prvFreeAligned(void *ptr, char *pFile, int pLine);

void * prvRegAllocMem(void *new_ptr, size_t pSize, char *pFile, int pLine, int category);
void   prvUnregAllocMem(void *lpMem, char *pFile, int pLine);
void   prvGetMemStats(size_t *PeakHeapSize, size_t *MemLeakSize);

#define auMalloc(size)      \
    prvMalloc(size, __FILE__, __LINE__, 0)
                                
#define auFree(free_ptr)        \
    prvFree(free_ptr, __FILE__, __LINE__)

#define WMAREGALLOCMEM(lpMem, size, category)   \
    prvRegAllocMem(lpMem, size, __FILE__, __LINE__, 0, category)

#define WMVAUNREGALLOCMEM(free_ptr)     \
    prvUnregAllocMem(free_ptr, __FILE__, __LINE__)

#define mallocAligned(size, iAlignToBytes) \
    prvMallocAligned(size, iAlignToBytes, __FILE__, __LINE__);

#define freeAligned(free_ptr)       \
    prvFreeAligned(free_ptr, __FILE__, __LINE__)


int     prvWriteMemTraceLog();
void    prvHeapMeasureStart(const char *log_file);

#define     START_MEASURE_MEM(log_file) prvHeapMeasureStart(log_file);
#define     WRITE_MEM_LOG()             prvWriteMemTraceLog();
#define     GET_MEM_STATS(PeakHeapSize, MemLeakSize) prvGetMemStats(PeakHeapSize, MemLeakSize);

#endif //WANT_HEAP_MEASURE

//
// define tracing stack usage macros
//

#ifndef WANT_STACK_MEASURE

#define STACK_MEASURE_START(stack_no)
#define STACK_MEASURE_STOP(a, stack_no)
#define WRITE_STACK_LOG(logfile, stackno)

#else //WANT_STACK_MEASURE


#ifndef STACK_MEASURE_TOPS
#define STACK_MEASURE_TOPS 6000
#endif

void prvStackMeasureStart(int stack_no);
int  prvStackUsed(int stack_no);
int  prvWriteStackTraceLog(const char *log_file, int stackno, int stackSize);

#define STACK_MEASURE_START(stack_no) prvStackMeasureStart(stack_no);
#define STACK_MEASURE_STOP(a, stack_no) int a = prvStackUsed(stack_no);
#define WRITE_STACK_LOG(logfile, stackNo)   \
    { \
        STACK_MEASURE_STOP(stackSize, stackNo); \
        prvWriteStackTraceLog(logfile, stackNo, stackSize); \
    } \

#endif //WANT_STACK_MEASURE

void *voauCalloc(VO_MEM_OPERATOR *vopMemOP,const size_t iSize);
void *vocallocAligned(VO_MEM_OPERATOR *vopMemOP, size_t size, WMA_I32 iAlignToBytes);

#define auCalloc(iSize)						voauCalloc(paudec->vopMemOP, iSize)
#define callocAligned(size, iAlignToBytes)	vocallocAligned(paudec->vopMemOP, size, iAlignToBytes)
#if 0
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#ifndef NO_MF
#if !defined(NEW_NOTUSING_AUMALLOC)

#pragma warning(disable:4211)

static void * __cdecl operator new(size_t nSize)
{
    void * p = auMalloc(nSize);
    return p;
}

static void * __cdecl operator new[](size_t nSize)
{
	void * p = auMalloc(nSize);
    return p;
}

static void __cdecl operator delete(void* pv)
{
    auFree(pv);
}

static void __cdecl operator delete[](void* pv)
{
	auFree(pv);
}

#pragma warning(default:4211)

#endif // defined(AUMALLOC_RT_HEAP)
#endif // NO_MF
#endif // __cplusplus
#endif
#endif //__AUMALLOC_H_

