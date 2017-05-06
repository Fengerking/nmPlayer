
/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
 
#ifndef __VP8_DEC_DECMC_H
#define __VP8_DEC_DECMC_H
#include "voVP8DecMBlock.h"
//#include "voVP8DecBoolCoder.h"

#define SUBMVREF_COUNT 5


int vp8_build_uvmvs(MACROBLOCKD *x,MODE_INFO *pModeInfo);
int vp8_build_uvmvs_fullpixel(MACROBLOCKD *x,MODE_INFO *pModeInfo);

#endif
