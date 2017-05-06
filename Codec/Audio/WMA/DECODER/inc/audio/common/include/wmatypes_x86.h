//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    wmatypes_x86.h

Abstract:

    x86 processor-specific data types.

Author:

    Kazuhito Koishida (kazukoi)       Jan 31, 2003

Revision History:


*************************************************************************/

#ifndef __WMATYPES_X86_H
#define __WMATYPES_X86_H

#define PLATFORM_SPECIFIC_U64
typedef unsigned long long U64;
#define PLATFORM_SPECIFIC_I64
typedef long long I64;

#ifndef INLINE
#define PLATFORM_SPECIFIC_INLINE
#if _MSC_VER >= 1200            /* VC6.0 == 1200, VC5.0 == 1100 */
#define INLINE __forceinline
#else  /* _MSC_VER */
#define INLINE __inline static
#endif /* _MSC_VER */
#endif

#endif  //_WMATYPES_X86_H
