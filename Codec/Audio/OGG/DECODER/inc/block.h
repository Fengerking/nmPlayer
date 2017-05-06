//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    block.h

Abstract:

    block functions.

Author:

    Witten Wen 14-September-2009

Revision History:

*************************************************************************/

#ifndef __BLOCK_H_
#define __BLOCK_H_

extern int voOGGDecSynthesisBlockin(VorbisDSPState *v, VorbisBlock *vb);//vorbis_synthesis_blockin
extern int voOGGDecSynthesisRead(VorbisDSPState *v, int bytes);	//vorbis_synthesis_read
extern int voOGGDecBlockInit(VorbisDSPState *v, VorbisBlock *vb);

#endif	//__BLOCK_H_