//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    sbccommon.h

Abstract:

    Subband Codec Common function and define head file.

Author:

    Witten Wen 11-January-2010

Revision History:

*************************************************************************/

#ifndef __SBC_COMMON_H_
#define __SBC_COMMON_H_

#include "sbctypes.h"

void sbc_calculate_bits(const struct sbc_frame *frame, int (*bits)[8], uint8_t sf);
uint8_t sbc_crc8(const uint8_t *data, VO_U32 len);

#endif	//__SBC_COMMON_H_