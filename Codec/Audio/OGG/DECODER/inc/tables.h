//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    tables.h

Abstract:

    OGG tables declaration header file. 

Author:

    Witten Wen 10-October-2009

Revision History:

*************************************************************************/

#ifndef __TABLES_H_
#define __TABLES_H_

#include "macros.h"

extern LOOKUP_T sincos_lookup1[1024];
extern LOOKUP_T sincos_lookup0[1026];

extern LOOKUP_T voWin64[32];
extern LOOKUP_T voWin128[64];
extern LOOKUP_T voWin256[128];
extern LOOKUP_T voWin512[256];
extern LOOKUP_T voWin1024[512];
extern LOOKUP_T voWin2048[1024];
extern LOOKUP_T voWin4096[2048];
extern LOOKUP_T voWin8192[4096];

#endif	//__TABLES_H_