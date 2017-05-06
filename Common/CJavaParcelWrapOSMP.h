/************************************************************************
 VisualOn Proprietary
 Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved
 
VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA
 
All data and information contained in or disclosed by this document are
 confidential and proprietary information of VisualOn, and all rights
 therein are expressly reserved. By accepting this material, the
 recipient agrees that this material and the information contained
 therein are held in confidence and in trust. The material may only be
 used and/or disclosed as authorized in a license agreement controlling
 such use and disclosure.
 ************************************************************************/

/************************************************************************
 * @file CJavaParcelWrapOSMP.h
 * Convert some C struct data to Java Parcel object.
 *
 * This class base on CJavaParcelWrap
 *
 *
 * @author  Li Mingbo
 * @date    2012-2012 
 ************************************************************************/

#ifndef __CJavaParcelWrapOSMP_H__
#define __CJavaParcelWrapOSMP_H__

#include "voYYDef_Common.h"
#include	<string.h>
#include	<jni.h>
#include	<stdio.h>
#include    <stdlib.h>
#include	<dlfcn.h>
#include "voOnStreamType.h"
#include "voOnStreamSource.h"
#include "CJavaParcelWrap.h"
#include "voAdsManager.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CJavaParcelWrapOSMP: public CJavaParcelWrap
{
public:
	CJavaParcelWrapOSMP(JNIEnv* env):CJavaParcelWrap(env)
	{
	}
	virtual ~CJavaParcelWrapOSMP()
	{
	}
public:
	int fillParcelData(VOOSMP_SRC_PROGRAM_INFO* pInfo);
	int fillParcelData(VOOSMP_SRC_STREAM_INFO* pInfo);
	int fillParcelData(VOOSMP_SRC_TRACK_INFO* pInfo);
	int fillParcelData(VOOSMP_SRC_VIDEO_INFO* pInfo);
	int fillParcelData(VOOSMP_SRC_AUDIO_INFO* pInfo);
	int fillParcelData(VOOSMP_VIDEO_FORMAT* pInfo);
	int fillParcelData(VOOSMP_AUDIO_FORMAT* pInfo);
	int fillParcelData(VOOSMP_SRC_CUSTOMERTAG_TIMEDTAG* pInfo);
	int fillParcelData(VOOSMP_SRC_CHUNK_INFO* pInfo);
	int fillParcelData(VOOSMP_CLOCK_TIME_STAMP* pTimeStamp);
	int fillParcelData(VOOSMP_SEI_PIC_TIMING* pPicTiming);
	int fillParcelData(VOOSMP_SRC_SUBTITLE_INFO* pInfo);
	int fillParcelData(VOOSMP_SRC_IO_FAILED_REASON_DESCRIPTION* pIOFailedDesc, const char* url);
	int fillParcelData(VOOSMP_PCMBUFFER* pInfo);
	int fillParcelData(VOOSMP_MODULE_VERSION* pInfo);
	int fillParcelData(VOOSMP_SRC_DRM_INIT_DATA_RESPONSE* pInfo);
	int fillParcelData(VO_ADSMANAGER_PLAYBACKINFO* pInfo);
	int fillParcelData(VO_ADSMANAGER_PLAYBACKPERIOD* pInfo);
	int fillParcelData(VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO* pInfo);
	int fillParcelData(VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO* pInfo);
	int fillParcelData(VOOSMP_SRC_CHUNK_SAMPLE* pInfo);
	int fillParcelData(VO_ADSMANAGER_SEEKINFO* pInfo);
    int fillParcelData(VO_ADSMANAGER_EVENT_TRAKCING_TIMEPASSEDINFO* pInfo);
	int fillParcelData(VO_ADSMANAGER_TRACKINGEVENT_INFO* pInfo);
private:

};

#ifdef _VONAMESPACE
}
#endif

#endif
