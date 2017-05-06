/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
#ifndef __MPEG_IDCT_H__
#define __MPEG_IDCT_H__


#include "voMpegPort.h"
#include "voMpeg2MB.h"
#include "voMpegMC.h"


VO_VOID IdctColunm4(VO_S16 *block);
VO_VOID IdctColunm8(VO_S16 *block);
VO_VOID IdctLine8(VO_S16 *block, VO_U8 *dst, const VO_U8 *src);
VO_VOID IdctLine4(VO_S16 *block, VO_U8 *dst, const VO_U8 *src);



#endif//__MPEG_IDCT_H__
