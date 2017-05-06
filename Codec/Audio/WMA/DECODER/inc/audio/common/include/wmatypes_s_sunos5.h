//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    wmatypes_s_sunos5.h

Abstract:

    SUN OS5 processor-specific data types.

Author:

    Kazuhito Koishida (kazukoi)       Jan 31, 2003

Revision History:


*************************************************************************/

#ifndef __WMATYPES_S_SUNOS5_H
#define __WMATYPES_S_SUNOS5_H

#define PLATFORM_SPECIFIC_U64
typedef  unsigned long long int  U64 ;
#define PLATFORM_SPECIFIC_I64
typedef  long long int  I64 ;

#define  PLATFORM_SPECIFIC_U32
typedef unsigned int U32;
#define  PLATFORM_SPECIFIC_I32
typedef int I32;

#define PLATFORM_SPECIFIC_INLINE
#define INLINE inline  

#endif  //_WMATYPES_S_SUNOS5_H
