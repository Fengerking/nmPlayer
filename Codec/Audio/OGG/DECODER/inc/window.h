//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    window.h

Abstract:

    window functions header file.

Author:

    Witten Wen 10-October-2009

Revision History:

*************************************************************************/

#ifndef __WINDOW_H_
#define __WINDOW_H_

#include "macros.h"

extern void VorbisApplyWindow(OGG_S32 *d, const void *window_p[2], long *blocksizes,
			  int lW, int W, int nW); //_vorbis_apply_window
extern const void *VorbisWindow(int type, int left);		//_vorbis_window
#endif		//__WINDOW_H_