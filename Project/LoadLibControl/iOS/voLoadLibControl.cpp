    /************************************************************************
    *                                                                      *
    *        VisualOn, Inc. Confidential and Proprietary, 2003 -           *
    *                                                                      *
    ************************************************************************/
/*******************************************************************************
    File:        voLoadLibControl.cpp
 
    Contains:    Control the load libs of source
 
    Written by:  Jeff
 
    Change History (most recent first):
    2012-07-24        Jeff            Create file
 
 *******************************************************************************/

#include "voLoadLibControl.h"
#include <string.h>

#define _ENABLE_SOURCE_MP4 1
#define _ENABLE_SOURCE_ASF 1
#define _ENABLE_SOURCE_AVI 1
#define _ENABLE_SOURCE_MOV 1
#define _ENABLE_SOURCE_REAL 1
#define _ENABLE_SOURCE_MPG 1
#define _ENABLE_SOURCE_TS 1
#define _ENABLE_SOURCE_FLV 1
#define _ENABLE_SOURCE_MKV 1
#define _ENABLE_SOURCE_AUDIO 1
#define _ENABLE_SOURCE_OGG 1
#define _ENABLE_SOURCE_RTSP 1
#define _ENABLE_SOURCE_HLS 1
#define _ENABLE_SOURCE_SSS 1
#define _ENABLE_SOURCE_DASH 1
#define _ENABLE_SOURCE_HTTPPD 1
#define _ENABLE_SOURCE_WMSP 1
#define _ENABLE_SOURCE_DRM 1

#define _ENABLE_VIDEO_SUBTITLE 1

#define _ENABLE_VIDEO_MPEG4 1
#define _ENABLE_VIDEO_H264 1
#define _ENABLE_VIDEO_WMV 1
#define _ENABLE_VIDEO_VC1 1
#define _ENABLE_VIDEO_RV 1
#define _ENABLE_VIDEO_MPEG2 1
#define _ENABLE_VIDEO_MJPEG 1
#define _ENABLE_VIDEO_DIVX 1
#define _ENABLE_VIDEO_VP6 1
#define _ENABLE_VIDEO_VP8 1
#define _ENABLE_AUDIO_MP3 1
#define _ENABLE_AUDIO_AAC 1
#define _ENABLE_AUDIO_AMRNB 1
#define _ENABLE_AUDIO_AMRWB 1
#define _ENABLE_AUDIO_AMRWBP 1
#define _ENABLE_AUDIO_WMA 1
#define _ENABLE_AUDIO_ADPCM 1
#define _ENABLE_AUDIO_QCELP13 1
#define _ENABLE_AUDIO_EVRC 1
#define _ENABLE_AUDIO_AC3 1
#define _ENABLE_AUDIO_EAC3 1
#define _ENABLE_AUDIO_FLAC 1
#define _ENABLE_AUDIO_OGG 1
#define _ENABLE_AUDIO_RA 1
#define _ENABLE_AUDIO_DOLBY 1

void* voGetModuleAdapterFunc(char *pszApiName)
{    
    void *pRet = NULL;
    
    if (NULL == pszApiName) {
        return pRet;
    }
    
    if (false) {
        ;
    }
#if (_ENABLE_SOURCE_MP4)
    else if (0 == strcmp(pszApiName, "voGetMP42ReadAPI")) {
		pRet = (void *)voGetMP42ReadAPIAdapter;
	}
#endif
#if (_ENABLE_SOURCE_ASF)
    else if (0 == strcmp(pszApiName, "voGetASF2ReadAPI")) {
		pRet = (void *)voGetASF2ReadAPIAdapter;
	}
#endif
#if (_ENABLE_SOURCE_AVI)
    else if (0 == strcmp(pszApiName, "voGetAVI2ReadAPI")) {
		pRet = (void *)voGetAVI2ReadAPIAdapter;
	}
#endif
#if (_ENABLE_SOURCE_MOV)
#endif
#if (_ENABLE_SOURCE_REAL)
    else if (0 == strcmp(pszApiName, "voGetReal2ReadAPI")) {
		pRet = (void *)voGetReal2ReadAPIAdapter;
	}
#endif
#if (_ENABLE_SOURCE_MPG)
    else if (0 == strcmp(pszApiName, "voGetMPG2ReadAPI")) {
		pRet = (void *)voGetMPG2ReadAPIAdapter;
	}
#endif
#if (_ENABLE_SOURCE_TS)
    else if (0 == strcmp(pszApiName, "voGetMTV2ReadAPI")) {
		pRet = (void *)voGetMTV2ReadAPIAdapter;
	}
#endif
//#if (_ENABLE_SOURCE_DV)
//#endif
#if (_ENABLE_SOURCE_FLV)
    else if (0 == strcmp(pszApiName, "voGetFLV2ReadAPI")) {
		pRet = (void *)voGetFLV2ReadAPIAdapter;
	}
#endif
//#if (_ENABLE_SOURCE_CMMB)
//#endif
#if (_ENABLE_SOURCE_MKV)
    else if (0 == strcmp(pszApiName, "voGetMKV2ReadAPI")) {
		pRet = (void *)voGetMKV2ReadAPIAdapter;
	}
#endif
//#if (_ENABLE_SOURCE_H264)
//    else if (0 == strcmp(pszApiName, "voGetH2642RawDataParserAPI")) {
//		pRet = (void *)voGetH2642RawDataParserAPIAdapter;
//	}
//#endif
#if (_ENABLE_SOURCE_AUDIO)
    else if (0 == strcmp(pszApiName, "voGetAudio2ReadAPI")) {
		pRet = (void *)voGetAudio2ReadAPIAdapter;
	}
#endif
//#if (_ENABLE_SOURCE_PCM)
//    else if (0 == strcmp(pszApiName, "voGetPCM2RawDataParserAPI")) {
//		pRet = (void *)voGetPCM2RawDataParserAPIAdapter;
//	}
//#endif
//#if (_ENABLE_SOURCE_MIDI)
//    else if (0 == strcmp(pszApiName, "voGetMidi2ReadAPI")) {
//		pRet = (void *)voGetMidi2ReadAPIAdapter;
//	}
//#endif
#if (_ENABLE_SOURCE_OGG)
    else if (0 == strcmp(pszApiName, "voGetOGG2ReadAPI")) {
		pRet = (void *)voGetOGG2ReadAPIAdapter;
	}
#endif
#if (_ENABLE_SOURCE_RTSP)
    else if (0 == strcmp(pszApiName, "voGetRTSP2ReadAPI")) {
		pRet = (void *)voGetRTSP2ReadAPIAdapter;
	}
#endif
#if (_ENABLE_SOURCE_HLS)
    else if (0 == strcmp(pszApiName, "voGetSrcHLSAPI")) {
		pRet = (void *)voGetSrcHLSAPIAdapter;
	}
#endif
#if (_ENABLE_SOURCE_SSS)
    else if (0 == strcmp(pszApiName, "voGetASCISSAPI")) {
		pRet = (void *)voGetASCISSAPIAdapter;
	}
#endif
#if (_ENABLE_SOURCE_DASH)
    else if (0 == strcmp(pszApiName, "voGetASCDASHAPI")) {
		pRet = (void *)voGetASCDASHAPIAdapter;
	}
#endif
#if (_ENABLE_SOURCE_HTTPPD) || (_ENABLE_SOURCE_WMSP)
    else if (0 == strcmp(pszApiName, "voGetPreHTTPAPI")) {
		pRet = (void *)voGetPreHTTPAPIAdapter;
	}
#endif
#if (_ENABLE_SOURCE_HTTPPD)
    else if (0 == strcmp(pszApiName, "voGetPD2ReadAPI")) {
		pRet = (void *)voGetPD2ReadAPIAdapter;
	}
#endif
#if (_ENABLE_SOURCE_WMSP)
    else if (0 == strcmp(pszApiName, "voGetWMSP2ReadAPI")) {
		pRet = (void *)voGetWMSP2ReadAPIAdapter;
	}
#endif
#if (_ENABLE_SOURCE_DRM)
    else if (0 == strcmp(pszApiName, "voGetDRMAPI")) {
		pRet = (void *)voGetDRMAPIAdapter;
	}
#endif

    else if (0 == strcmp(pszApiName, "vompGetFuncSet")) {
		pRet = (void *)vompGetFuncSetAdapter;
	}
    
#if (_ENABLE_VIDEO_MPEG4)
    else if (0 == strcmp(pszApiName, "voGetMPEG4DecAPI")) {
		pRet = (void *)voGetMPEG4DecAPIAdapter;
	}
#endif
#if (_ENABLE_VIDEO_H264)
    else if (0 == strcmp(pszApiName, "voGetH264DecAPI")) {
		pRet = (void *)voGetH264DecAPIAdapter;
	}
#endif
#if (_ENABLE_VIDEO_WMV)
    else if (0 == strcmp(pszApiName, "voGetWMV9DecAPI")) {
		pRet = (void *)voGetWMV9DecAPIAdapter;
	}
#endif
#if (_ENABLE_VIDEO_VC1)
    else if (0 == strcmp(pszApiName, "voGetVC1DecAPI")) {
		pRet = (void *)voGetVC1DecAPIAdapter;
	}
#endif
#if (_ENABLE_VIDEO_RV)
    else if (0 == strcmp(pszApiName, "voGetRVDecAPI")) {
		pRet = (void *)voGetRVDecAPIAdapter;
	}
#endif
#if (_ENABLE_VIDEO_MPEG2)
    else if (0 == strcmp(pszApiName, "voGetMPEG2DecAPI")) {
		pRet = (void *)voGetMPEG2DecAPIAdapter;
	}
#endif
#if (_ENABLE_VIDEO_MJPEG)
    else if (0 == strcmp(pszApiName, "voGetMJPEGDecAPI")) {
		pRet = (void *)voGetMJPEGDecAPIAdapter;
	}
#endif
#if (_ENABLE_VIDEO_DIVX)
    else if (0 == strcmp(pszApiName, "voGetDIVX3DecAPI")) {
		pRet = (void *)voGetDIVX3DecAPIAdapter;
	}
#endif
#if (_ENABLE_VIDEO_VP6)
    else if (0 == strcmp(pszApiName, "voGetVP6DecAPI")) {
		pRet = (void *)voGetVP6DecAPIAdapter;
	}
#endif
//#if (_ENABLE_VIDEO_VP7)
//    else if (0 == strcmp(pszApiName, "voGetVP7DecAPI")) {
//		pRet = (void *)voGetVP7DecAPIAdapter;
//	}
//#endif
#if (_ENABLE_VIDEO_VP8)
    else if (0 == strcmp(pszApiName, "voGetVP8DecAPI")) {
		pRet = (void *)voGetVP8DecAPIAdapter;
	}
#endif
    
#if (_ENABLE_AUDIO_MP3)
    else if (0 == strcmp(pszApiName, "voGetMP3DecAPI")) {
		pRet = (void *)voGetMP3DecAPIAdapter;
	}
#endif
#if (_ENABLE_AUDIO_AAC)
    else if (0 == strcmp(pszApiName, "voGetAACDecAPI")) {
		pRet = (void *)voGetAACDecAPIAdapter;
	}
#endif
#if (_ENABLE_AUDIO_AMRNB)
    else if (0 == strcmp(pszApiName, "voGetAMRNBDecAPI")) {
		pRet = (void *)voGetAMRNBDecAPIAdapter;
	}
#endif
#if (_ENABLE_AUDIO_AMRWB)
    else if (0 == strcmp(pszApiName, "voGetAMRWBDecAPI")) {
		pRet = (void *)voGetAMRWBDecAPIAdapter;
	}
#endif
#if (_ENABLE_AUDIO_AMRWBP)
    else if (0 == strcmp(pszApiName, "voGetAMRWBPDecAPI")) {
		pRet = (void *)voGetAMRWBPDecAPIAdapter;
	}
#endif
#if (_ENABLE_AUDIO_WMA)
    else if (0 == strcmp(pszApiName, "voGetWMADecAPI")) {
		pRet = (void *)voGetWMADecAPIAdapter;
	}
#endif
#if (_ENABLE_AUDIO_ADPCM)
    else if (0 == strcmp(pszApiName, "voGetADPCMDecAPI")) {
		pRet = (void *)voGetADPCMDecAPIAdapter;
	}
#endif
#if (_ENABLE_AUDIO_QCELP13)
    else if (0 == strcmp(pszApiName, "voGetQCELPDecAPI")) {
		pRet = (void *)voGetQCELPDecAPIAdapter;
	}
#endif
#if (_ENABLE_AUDIO_EVRC)
    else if (0 == strcmp(pszApiName, "voGetEVRCDecAPI")) {
		pRet = (void *)voGetEVRCDecAPIAdapter;
	}
#endif
#if (_ENABLE_AUDIO_AC3)
    else if (0 == strcmp(pszApiName, "voGetAC3DecAPI")) {
		pRet = (void *)voGetAC3DecAPIAdapter;
	}
#endif
#if (_ENABLE_AUDIO_EAC3)
    else if (0 == strcmp(pszApiName, "voGetEAC3DecAPI")) {
		pRet = (void *)voGetEAC3DecAPIAdapter;
	}
#endif
#if (_ENABLE_AUDIO_FLAC)
    else if (0 == strcmp(pszApiName, "voGetFLACDecAPI")) {
		pRet = (void *)voGetFLACDecAPIAdapter;
	}
#endif
#if (_ENABLE_AUDIO_OGG)
    else if (0 == strcmp(pszApiName, "voGetOGGDecAPI")) {
		pRet = (void *)voGetOGGDecAPIAdapter;
	}
#endif
#if (_ENABLE_AUDIO_RA)
    else if (0 == strcmp(pszApiName, "voGetRADecAPI")) {
		pRet = (void *)voGetRADecAPIAdapter;
	}
#endif
//#if (_ENABLE_AUDIO_APE)
//    else if (0 == strcmp(pszApiName, "voGetAPEDecAPI")) {
//		pRet = (void *)voGetAPEDecAPIAdapter;
//	}
//#endif
//#if (_ENABLE_AUDIO_ALAC)
//    else if (0 == strcmp(pszApiName, "voGetALACDecAPI")) {
//		pRet = (void *)voGetALACDecAPIAdapter;
//	}
//#endif
    
#if (_ENABLE_AUDIO_DOLBY)
    else if (0 == strcmp(pszApiName, "voGetDolbyEffectAPI")) {
		pRet = (void *)voGetDolbyEffectAPIAdapter;
	}
#endif
    
#if (_ENABLE_VIDEO_SUBTITLE)
    else if (0 == strcmp(pszApiName, "voGetCaptionParserAPI")) {
		pRet = (void *)voGetCaptionParserAPIAdapter;
	}
#endif
    
    else {}
    
    return pRet;
}


