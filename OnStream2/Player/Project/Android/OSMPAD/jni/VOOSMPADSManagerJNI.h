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
/************************************************************************
 * @file VOOSMPADSManagerJNI.h
 * VOOSMPADSManagerJNI
 *
 * VOOSMPADSManagerJNI
 *
 * @author  Jeff Huang
 *
 * @date    2013-2013 (yyyy: year of creation, zzzz: year of last update)
 ************************************************************************/

#ifndef __VOOSMP_ADS_MANAGER_JNI_H__
#define __VOOSMP_ADS_MANAGER_JNI_H__

#ifdef _LINUX_ANDROID
#include <jni.h>
#endif

#include "voAdsManager.h"
#include "voType.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

class VOOSMPADSManagerJNI
{
public:
    VOOSMPADSManagerJNI(void);
    virtual ~VOOSMPADSManagerJNI(void);
    
#ifdef _LINUX_ANDROID
    virtual int SetJavaVM(JNIEnv *pJNIEnv, jobject jobj);
    virtual void* GetJavaObj() {return m_jObj;};
#endif
    
    VO_U32 Init(VO_PTR pSource , VO_U32 nSize , VO_U32 nFlag , VO_ADSMANAGER_INITPARAM * pParam, VO_CHAR *pLibName);
    VO_U32 Uninit();
    VO_U32 Open();
    VO_U32 Close();
    
    VO_U32 GetPlaybackInfo(VO_ADSMANAGER_PLAYBACKINFO ** ppInfo);
    VO_U32 ConvertTimeStamp(VO_U64 * pOrgTime , VO_U64 ullPlayingTime);
    VO_U32 SetPlayingTime(VO_U64 ullPlayingTime);
    VO_U32 SetAction(VO_ADSMANAGER_ACTION nAction, VO_U64 ullPlayingTime);
    VO_U32 SetActionSync(VO_ADSMANAGER_ACTION nAction, VO_U64 ullPlayingTime, VO_VOID* pParam);
    VO_U32 GetContentDuration(VO_U64 * pullDuration);
    VO_BOOL IsSeekable(VO_U64 ullPlayingTime);
    VO_U32 Seek(VO_U64 ullPlayingtTime, VO_U64 ullSeekPos, VO_ADSMANAGER_SEEKINFO* pInfo);
    VO_U32 SetParam(VO_U32 nParamID, VO_PTR pParam);
    
protected:
    virtual int LoadDll(VO_CHAR *pPath, VO_CHAR *pLibName);
    
    static void * vomtLoadLib (void * pUserData, VO_PCHAR pLibName, VO_S32 nFlag);
    static void * vomtGetAddress (void * pUserData, void * hLib, VO_PCHAR pFuncName, VO_S32 nFlag);
    static VO_S32 vomtFreeLib (void * pUserData, void * hLib, VO_S32 nFlag);
    
    static VO_S32 AdsEventCallback(VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);
    VO_S32 onAdsEvent(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);
    
private:
    VO_PTR m_pDll;
    VO_HANDLE handle;
    
#ifdef _LINUX_ANDROID
    JavaVM*  m_JavaVM;
    jobject  m_jObj;
#endif
    
    VO_ADSMANAGER_API g_api;
};

#endif //__VOOSMP_ADS_MANAGER_JNI_H__
