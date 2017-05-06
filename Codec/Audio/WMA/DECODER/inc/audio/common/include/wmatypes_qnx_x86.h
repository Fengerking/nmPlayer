//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    wmatypes_qnx_x86.h

Abstract:

    QNX x86 processor-specific data types.

Author:

    Kazuhito Koishida (kazukoi)       Jan 31, 2003

Revision History:


*************************************************************************/

#ifndef __WMATYPES_QNX_X86_H
#define __WMATYPES_QNX_X86_H

#ifdef __QNX__
#define PLATFORM_SPECIFIC_U64
#define PLATFORM_SPECIFIC_I64
#include "../qnx/qnx.h"
#else
#define PLATFORM_SPECIFIC_U64
typedef long long U64;
#define PLATFORM_SPECIFIC_I64
typedef long long I64;
#endif

#define PLATFORM_SPECIFIC_INLINE                       
#define INLINE __inline

#endif  //_WMATYPES_QNX_X86_H
