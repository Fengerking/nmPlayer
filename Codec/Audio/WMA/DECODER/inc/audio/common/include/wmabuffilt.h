//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
#ifndef _WMA_BUFFILT_H
#define _WMA_BUFFILT_H

//#include "msaudiodec.h" 
#include "wmspecstring.h"

// conditional include xmmintrin.h
#include "wmatypes.h"
//#include "msaudiodec.h"

#if defined (WMA_TARGET_X86) && !defined(WMA_DISABLE_SSE1)
#if defined(UNDER_CE) && (UNDER_CE<500)
#else
#include <xmmintrin.h>
#endif // UNDER_CE && UNDER_CE<500
#endif // WMA_TARGET_X86 && !WMA_DISABLE_SSE1

// rename all functions to avoid duplicates
#if defined(UNDER_CE) || defined(LINUX)
#define TOMATIN_BUILD
#endif

#ifdef TOMATIN_BUILD
#define bufFiltZero bufFltZero
#define bufFiltFree bufFltFree
#define bufFiltInit bufFltInit
#define bufFilt bufFlt
#define bufFilt_SSE bufFlt_SSE
#endif

#ifdef _XBOX
    //__in_bound has not been defined in Xenon yet
    #ifndef __in_bound
        #define __in_bound
    #endif //__in_bound
#endif //_XBOX

typedef Float CompType;
typedef Float DataType;

// for DataType == Int
//#define BF_ROUNDCD(x) ROUNDF(x)
// for DataType == Float
#define BF_ROUNDCD(x) (x)

// In place filter
typedef struct BufFilt
{
    float *m_filtCoeffs;
    Int m_iStartPos;
    Int m_iLength;
    float *m_dataBuf;
    Int m_curBufLen;
} BufFilt;

void bufFiltZero(BufFilt *pBufFilt);


// In place delay (no scaling)
typedef struct BufDelay
{
    Int m_iDelay;
    float *m_dataBuf;
    float *m_endDataBuf;
    Int m_curBufLen;
} BufDelay;

void bufDelayZero(BufDelay *pBufDelay);

#endif // _WMA_BUFFILT_H

