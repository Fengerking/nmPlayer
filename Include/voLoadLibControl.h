/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

#ifndef __VO_LOADLIB_CONTROL_
#define __VO_LOADLIB_CONTROL_

void* voGetModuleAdapterFunc(char *pszApiName);

typedef void* (* voGetModuleRealAPI) ();

void* voGetHLSWrapperAPIAdapter();

void* voGetPD2ReadAPIAdapter();
void* voGetWMSP2ReadAPIAdapter();
void* voGetDRMAPIAdapter();
void* voGetMP4ReadAPIAdapter();
void* voGetMP42ReadAPIAdapter();
void* voGetASFReadAPIAdapter();
void* voGetASF2ReadAPIAdapter();
void* voGetRealReadAPIAdapter();
void* voGetReal2ReadAPIAdapter();
void* voGetMPGReadAPIAdapter();
void* voGetMPG2ReadAPIAdapter();
void* voGetMTVReadAPIAdapter();
void* voGetMTV2ReadAPIAdapter();
void* voGetMKVReadAPIAdapter();
void* voGetMKV2ReadAPIAdapter();
void* voGetH2642RawDataParserAPIAdapter();
void* voGetAudioReadAPIAdapter();
void* voGetAudio2ReadAPIAdapter();
void* voGetPCM2RawDataParserAPIAdapter();
void* voGetMidi2ReadAPIAdapter();
void* voGetRTSP2ReadAPIAdapter();
void* voGetPreHTTPAPIAdapter();
void* voGetAVI2ReadAPIAdapter();
void* voGetAVIReadAPIAdapter();
void* voGetFLACDecAPIAdapter();
void* voGetFLV2ReadAPIAdapter();

void* voGetSSLAPIAdapter();
void* voGetAdaptiveStreamControllerAPIAdapter();
void* voGetSourceIOAPIAdapter();
void* voGetAdaptiveStreamHLSAPIAdapter();
void* voGetAdaptiveStreamDASHAPIAdapter();
void* voGetAdaptiveStreamISSAPIAdapter();
void* voGetSource2AACAPIAdapter();
void* voGetPushAudioAPIAdapter();
void* voGetParserAPIAdapter();
void* voGetSMTHParserAPIAdapter();
void* voGetMp4StreamReaderAPIAdapter();
void* voGetSubTitleParserAPIAdapter();
void* voGetDVBParserAPIAdapter();

void* vompGetFuncSetAdapter();

void* voGetAudioSpeedAPIAdapter();

void* voGetMPEG4DecAPIAdapter();
void* voGetH264DecAPIAdapter();
void* voGetH265DecAPIAdapter();
void* voGetDIVX3DecAPIAdapter();
void* voGetWMV9DecAPIAdapter();
void* voGetVC1DecAPIAdapter();
void* voGetRVDecAPIAdapter();
void* voGetMPEG2DecAPIAdapter();
void* voGetVP7DecAPIAdapter();
void* voGetVP8DecAPIAdapter();

void* voGetMP3DecAPIAdapter();
void* voGetAACDecAPIAdapter();
void* voGetWMADecAPIAdapter();
void* voGetAC3DecAPIAdapter();
void* voGetEAC3DecAPIAdapter();
void* voGetRADecAPIAdapter();
void* voGetAPEDecAPIAdapter();
void* voGetALACDecAPIAdapter();
void* voGetADPCMDecAPIAdapter();

void* voGetDolbyEffectAPIAdapter();
void* voGetCaptionParserAPIAdapter();

#endif // __VO_LOADLIB_CONTROL_
