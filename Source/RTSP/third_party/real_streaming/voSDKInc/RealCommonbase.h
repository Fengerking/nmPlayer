/** 
 * \file RealCommonbase.h
 * \brief VisualOn RealAudio ID & sturcture, for SetParameter/GetParameter
 * \version 0.1
 * \date 10/31/2006 created
 * \author VisualOn
 */


#ifndef __VO_RealCommon_BASE_H_
#define __VO_RealCommon_BASE_H_

#include "sdkbase.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#else
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif
#pragma pack(push, 4)



typedef struct  
{
	unsigned long ulSampleRate;
	unsigned long ulActualRate;
	unsigned short usBitsPerSample;
	unsigned short usNumChannels;
	unsigned short usAudioQuality;
	unsigned short usFlavorIndex;
	unsigned long ulBitsPerFrame;
	unsigned long ulGranularity;
	unsigned long ulOpaqueDataSize;
	unsigned char*  pOpaqueData;
}VORA_FORMAT_INFO;
typedef struct
{
	VORA_FORMAT_INFO *format;
	unsigned long ulVOFOURCC;
	void*  otherParams;//reserved field
	unsigned long otherParamSize;//reserved field
} VORA_INIT_PARAM;

typedef struct
{
	unsigned long   ulLength;
	unsigned long   ulMOFTag;
	unsigned long   ulSubMOFTag;
	unsigned short   usWidth;
	unsigned short   usHeight;
	unsigned short   usBitCount;
	unsigned short   usPadWidth;
	unsigned short   usPadHeight;
	unsigned long  ufFramesPerSecond;
	unsigned long   ulOpaqueDataSize;
	unsigned char*    pOpaqueData;
} VORV_FORMAT_INFO;

typedef struct
{
  VORV_FORMAT_INFO* format;		
} VORV_INIT_PARAM;

#pragma pack(pop)

#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif

#endif /* __VO_RealCommon_BASE_H_ */


