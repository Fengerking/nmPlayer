//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    wmatypes_mips.h

Abstract:

    MIPS processor-specific data types.

Author:

    Kazuhito Koishida (kazukoi)       Jan 31, 2003

Revision History:


*************************************************************************/

#ifndef __WMATYPES_MIPS_H
#define __WMATYPES_MIPS_H

#define PLATFORM_SPECIFIC_U64
typedef unsigned long long U64;
#define PLATFORM_SPECIFIC_I64
typedef long long I64;

#define PLATFORM_SPECIFIC_INLINE
#define INLINE __forceinline

#endif  //_WMATYPES_MIPS_H
