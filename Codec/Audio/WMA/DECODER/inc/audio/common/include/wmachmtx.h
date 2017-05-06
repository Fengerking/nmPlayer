//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************

#ifndef __WMACHMTX_H__
#define __WMACHMTX_H__

#include "msaudiodec.h"

#ifdef __cplusplus
extern "C" {
#endif
WMARESULT SetMixFromChannelMaskF(CAudioObjectDecoder *paudec, I32UP cSrcChannels, I32UP cDstChannels,
                                U32 srcMask, U32 dstMask, float *fltMixMtx);
WMARESULT SetMixFromChannelMaskD(CAudioObjectDecoder *paudec, I32UP cSrcChannels, I32UP cDstChannels,
                                U32 srcMask, U32 dstMask, double *fltMixMtx);
WMARESULT SetMixFromChannelMaskI(I32UP cSrcChannels, I32UP cDstChannels,
                                U32 srcMask, U32 dstMask, int *rgiMixMtx);
#ifdef __cplusplus
}
#endif

#endif//__WMACHMTX_H__

