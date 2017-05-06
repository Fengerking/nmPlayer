//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    wmatypes_sh4.h

Abstract:

    SH4 processor-specific data types.

Author:

    Kazuhito Koishida (kazukoi)       Jan 31, 2003

Revision History:


*************************************************************************/

#ifndef __WMATYPES_SH4_H
#define __WMATYPES_SH4_H

#ifdef NOINT64 

#include "int64.h"
#define PLATFORM_SPECIFIC_U64
typedef UINT64 U64;
#define PLATFORM_SPECIFIC_I64
typedef  INT64 I64;

#else // #ifdef NOINT64

#define PLATFORM_SPECIFIC_U64
typedef long long U64;
#define PLATFORM_SPECIFIC_I64
typedef long long I64;

#endif // #ifdef NOINT64

#ifdef HITACHI
#define PLATFORM_SPECIFIC_U32
typedef unsigned long U32;
#define PLATFORM_SPECIFIC_I32
typedef long I32;
#else

#define PLATFORM_SPECIFIC_INLINE
#ifdef HITACHI
#pragma inline(quickRand, RandStateClear, LOG2, NormUInt, Align2FracBits, ROUNDF, ROUNDD, bitCpy)
#define INLINE static
#else
#define INLINE __forceinline
#endif

#endif //HITACHI

#endif  //_WMATYPES_SH4_H
