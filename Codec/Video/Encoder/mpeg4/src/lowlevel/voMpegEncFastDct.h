/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _FDCT_H_
#define _FDCT_H_
#include "../voMpegProtect.h"
#include "voType.h"

#if defined (VOWMMX)
#include "ippVC.h"
#define FastDct ippiDCT8x8Fwd_Video_16s_C1I 
#elif defined (VOARMV4)
extern VO_VOID FastDct_ARMV4(VO_S16 *const block);
#define FastDct FastDct_ARMV4 
#elif defined (VOARMV6)
extern VO_VOID FastDct_ARMV6(VO_S16 *const block);
#define FastDct FastDct_ARMV6 
#elif defined (VOARMV7)
extern VO_VOID FastDct_ARMV7(VO_S16 *const block);
#define FastDct FastDct_ARMV7
#else
VO_VOID FastDct_C(VO_S16 *const block);
#define FastDct  FastDct_C
#endif

#endif   // _FDCT_H_ 
