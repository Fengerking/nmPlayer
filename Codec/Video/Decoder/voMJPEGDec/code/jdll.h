#ifndef __JDLL_H__
#define __JDLL_H__

#ifdef __cplusplus
extern "C" 
{
#endif

#ifdef VOJPEGFLAGE
#include "voJPEG.h"
#else
#include "voMJPEG.h"
#endif

extern VO_HANDLE g_hMJPEGDecInst;
extern VO_HANDLE g_hJPEGDecInst;

#ifdef __cplusplus
}
#endif

#endif