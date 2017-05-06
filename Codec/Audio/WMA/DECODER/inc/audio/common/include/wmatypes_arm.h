//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    wmatypes_arm.h

Abstract:

    ARM processor-specific data types.

Author:

    Kazuhito Koishida (kazukoi)       Jan 31, 2003

Revision History:


*************************************************************************/

#ifndef __WMATYPES_ARM_H
#define __WMATYPES_ARM_H

#define PLATFORM_SPECIFIC_U64
typedef unsigned long long U64;
#define PLATFORM_SPECIFIC_I64
typedef long long I64;

typedef unsigned long       DWORD;
typedef unsigned short      WORD;

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#define PLATFORM_SPECIFIC_INLINE
#if defined(LINUX) || defined(_IOS)
#define INLINE __inline static
#else
#define INLINE __forceinline
#endif
#endif  //_WMATYPES_ARM_H
