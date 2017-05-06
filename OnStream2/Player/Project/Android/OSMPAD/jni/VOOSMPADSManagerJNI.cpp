
#include "VOOSMPADSManagerJNI.h"

#ifdef _LINUX_ANDROID
#include <dlfcn.h>
#include "CJniEnvUtil.h"
#include "CJavaParcelWrap.h"
#include "CJavaParcelWrapOSMP.h"
#include "vodlfcn.h"
#endif

#include "voLog.h"
#include "voOnStreamType.h"

typedef VO_S32 (VO_API *pvoGetAdsManagerAPI)(VO_ADSMANAGER_API * pAPI);

VOOSMPADSManagerJNI::VOOSMPADSManagerJNI()
:m_pDll(NULL)
,handle(NULL)
#ifdef _LINUX_ANDROID
,m_JavaVM(NULL)
,m_jObj(NULL)
#endif
{
    memset(&g_api, 0, sizeof(g_api));
}

VOOSMPADSManagerJNI::~VOOSMPADSManagerJNI()
{
    if (NULL != m_pDll) {
        vomtFreeLib(0,m_pDll,0);
        m_pDll = NULL;
    }
    
#ifdef _LINUX_ANDROID
    if ((NULL != m_JavaVM) && (NULL != m_jObj)) {
        CJniEnvUtil	env(m_JavaVM);
        
        if (env.getEnv() != NULL) {
            env.getEnv()->DeleteGlobalRef(m_jObj);
            m_jObj = NULL;
        }
    }
#endif
}

#ifdef _LINUX_ANDROID
int VOOSMPADSManagerJNI::SetJavaVM(JNIEnv *pJNIEnv, jobject jobj)
{
    if ((NULL == pJNIEnv) || (NULL == jobj)) {
        return -1;
    }
    
	pJNIEnv->GetJavaVM(&m_JavaVM);
	m_jObj = pJNIEnv->NewGlobalRef(jobj);
    
	CJniEnvUtil	env(m_JavaVM);
    
	if (env.getEnv() == NULL)
		return -1;
    
    VOLOGI("ADS wrapper SetJavaVM m_JavaVM = %x m_jObj = %x", m_JavaVM, m_jObj);

	return 0;
}
#endif

void * VOOSMPADSManagerJNI::vomtLoadLib (void * pUserData, VO_PCHAR pLibName, VO_S32 nFlag)
{
	void * hDll = NULL;
    
	hDll = voLoadLibrary (pLibName, RTLD_NOW);
    
	if (hDll == NULL)
	{
		VOLOGE("ADS wrapper ADS wrapper Load Library File %s, failed!", pLibName);
	}
    else
	{
		VOLOGI("ADS wrapper Load Library File %s, Handle %p", pLibName, hDll);
	}
    
	return hDll;
}

void * VOOSMPADSManagerJNI::vomtGetAddress (void * pUserData, void * hLib, VO_PCHAR pFuncName, VO_S32 nFlag)
{
	void * ptr = voGetAddressSymbol (hLib, pFuncName);
    
	VOLOGI( "Func Name: %s  %p" , pFuncName , ptr );
	return ptr;
}

VO_S32 VOOSMPADSManagerJNI::vomtFreeLib (void * pUserData, void * hLib, VO_S32 nFlag)
{
	VOLOGI ("Free Lib: %0xd", (unsigned int) hLib);
	voFreeLibrary (hLib);
	return 0;
}

VO_S32 VOOSMPADSManagerJNI::AdsEventCallback(VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
    VOOSMPADSManagerJNI *pThis = (VOOSMPADSManagerJNI*)pUserData;
    
    if (NULL == pThis) {
        return VO_RET_SOURCE2_FAIL;
    }
    
    return pThis->onAdsEvent(nID, nParam1, nParam2);
}

VO_S32 VOOSMPADSManagerJNI::onAdsEvent(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
#ifdef _LINUX_ANDROID
    CJniEnvUtil	env(m_JavaVM);
	jclass clazz = env.getEnv()->GetObjectClass(m_jObj);
	if (clazz == NULL)
	{
		VOLOGI("ADS Wrapper Get clazz failed!");
		return VO_RET_SOURCE2_FAIL;
	}
    
	jmethodID eventCallBack = env.getEnv()->GetMethodID(clazz, "adsCallBack", "(IIILjava/lang/Object;)V");
	if (eventCallBack == NULL)
	{
		VOLOGI("ADS Wrapper Get Call back failed!");
		return VO_RET_SOURCE2_FAIL;
	}
    
    VOLOGI("ADS Wrapper onAdsEvent:%d, %d, %d", nID, nParam1, nParam2);
	
    jobject jobj = NULL;
    
    switch (nID) {
        case VO_ADSMANAGER_EVENT_TRACKING_PERIODPRECENTAGE:
        case VO_ADSMANAGER_EVENT_TRACKING_WHOLECONTENTPRECENTAGE:
        {
            if (0 != nParam1) {
                CJavaParcelWrapOSMP wrap(env.getEnv());
                wrap.fillParcelData((VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO *)nParam1);
                jobj = wrap.getParcel();
            }
        }
            break;
        case VO_ADSMANAGER_EVENT_TRACKING_ACTION:
        case VO_ADSMANAGER_EVENT_CONTENT_START:
        case VO_ADSMANAGER_EVENT_CONTENT_END:
        case VO_ADSMANAGER_EVENT_AD_START:
        case VO_ADSMANAGER_EVENT_AD_END:
		case VO_ADSMANAGER_EVENT_WHOLECONTENT_START:
		case VO_ADSMANAGER_EVENT_WHOLECONTENT_END:
        {
            if (0 != nParam1) {
                CJavaParcelWrapOSMP wrap(env.getEnv());
                wrap.fillParcelData((VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO *)nParam1);
                jobj = wrap.getParcel();
            }
        }
            break;
        case VO_ADSMANAGER_EVENT_TRACKING_TIMEPASSED:
        {
            if (0 != nParam1) {
                CJavaParcelWrapOSMP wrap(env.getEnv());
                wrap.fillParcelData((VO_ADSMANAGER_EVENT_TRAKCING_TIMEPASSEDINFO *)nParam1);
                jobj = wrap.getParcel();
            }
        }
			break;
		case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_AD_START:
		case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_FIRST_QUARTILE:
		case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_MID_POINT:
		case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_THIRD_QUARTILE:
		case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_COMPLETE:
		case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_CREATIVE_VIEW:
		case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_MUTE:
		case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_UNMUTE:
		case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_PAUSE:
		case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_RESUME:
		case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_FULLSCREEN:
		case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_IMPRESSION:
		
			if (0 != nParam1) {
                CJavaParcelWrapOSMP wrap(env.getEnv());
                wrap.fillParcelData((VO_ADSMANAGER_TRACKINGEVENT_INFO *)nParam1);
                jobj = wrap.getParcel();
            }
        default:
            break;
    }
    
	env.getEnv()->CallVoidMethod(m_jObj, eventCallBack, nID, nParam1, nParam2, jobj);
#endif
    
    return VO_RET_SOURCE2_OK;
}

int VOOSMPADSManagerJNI::LoadDll(VO_CHAR *pPath, VO_CHAR *pLibName)
{
    if ((NULL == pPath) || (NULL == pLibName)) {
        return VOOSMP_ERR_Pointer;
    }
    
    VOLOGI("ADS wrapper LoadDll path:%s name:%s", pPath, pLibName);
    
    char dllFile[1024];
    memset(dllFile, 0, sizeof(dllFile));
    
    strcat(dllFile, pPath);
    strcat(dllFile, pLibName);
    
    if (m_pDll == NULL)
        m_pDll = vomtLoadLib (0, dllFile, RTLD_NOW);
    
    if (m_pDll == NULL)
    {
        VOLOGE("ADS wrapper Failed to load %s.", dllFile);
        return VOOSMP_ERR_Pointer;
    }
    else
    {
        VOLOGI("ADS wrapper Success to load %s.", dllFile);
    }
    
    pvoGetAdsManagerAPI pAPI = (pvoGetAdsManagerAPI) vomtGetAddress(0, m_pDll, "voGetAdsManagerAPI", 0);
    if (pAPI == NULL)
    {
        VOLOGE("ADS wrapper @@@@@@ Get function Open address Error %s \n", dlerror ());
        return VOOSMP_ERR_Pointer;
    }
    
    pAPI(&g_api);
    
    if (!g_api.Init)
        return VOOSMP_ERR_Pointer;

    return VOOSMP_ERR_None;
}

VO_U32 VOOSMPADSManagerJNI::Init(VO_PTR pSource , VO_U32 nSize , VO_U32 nFlag , VO_ADSMANAGER_INITPARAM * pParam, VO_CHAR *pLibName)
{
    if ((NULL == pSource) || (NULL == pParam)) {
        VOLOGE("ADS wrapper error pointer:%x, %x \n", pSource, pParam);
        return VOOSMP_ERR_Pointer;
    }
    
    VO_U32 nRet = LoadDll(pParam->ptr_workingpath, pLibName);
    if (VOOSMP_ERR_None != nRet) {
        return nRet;
    }

    if (!g_api.Init) {
        return VOOSMP_ERR_Pointer;
    }
    
    nRet = g_api.Init(&handle, pSource, nSize, nFlag, pParam);
    
    VOLOGI("ADS wrapper Init handle = %d", handle);
    
    if (NULL == handle) {
        return VOOSMP_ERR_Pointer;
    }

    VO_ADSMANAGER_EVENTCALLBACK callback;
    memset(&callback, 0, sizeof(VO_ADSMANAGER_EVENTCALLBACK));
    callback.pUserData  = this;
    callback.SendEvent  = AdsEventCallback;
    g_api.SetParam(handle, VO_ADSMANAGER_PID_EVENTCALLBACK, &callback);
    
    return nRet;
}

VO_U32 VOOSMPADSManagerJNI::Uninit()
{
    if (NULL == handle)
        return VOOSMP_ERR_None;
    
    if ((NULL == m_pDll) || (NULL == g_api.Uninit))
        return VOOSMP_ERR_Pointer;
    
	VO_S32 nRet = g_api.Uninit(handle);
    if (VO_RET_SOURCE2_OK != nRet) {
        VOLOGE("ADS wrapper Uninit fail:%d", nRet);
        return nRet;
    }
    
    handle = NULL;
	VOLOGI("ADS wrapper Uninit success");
    
	return nRet;
}

VO_U32 VOOSMPADSManagerJNI::Open()
{
    if ((NULL == handle) || (NULL == m_pDll) || (NULL == g_api.Open))
        return VOOSMP_ERR_Pointer;
    
	return g_api.Open(handle);
}

VO_U32 VOOSMPADSManagerJNI::Close()
{
    if ((NULL == handle) || (NULL == m_pDll) || (NULL == g_api.Close))
        return VOOSMP_ERR_Pointer;
    
	return g_api.Close(handle);
}

VO_U32 VOOSMPADSManagerJNI::GetPlaybackInfo(VO_ADSMANAGER_PLAYBACKINFO ** ppInfo)
{
    if ((NULL == handle) || (NULL == m_pDll) || (NULL == g_api.GetPlaybackInfo) || (NULL == ppInfo))
        return VOOSMP_ERR_Pointer;
    
    return g_api.GetPlaybackInfo(handle, ppInfo);
}

VO_U32 VOOSMPADSManagerJNI::ConvertTimeStamp(VO_U64 * pOrgTime , VO_U64 ullPlayingTime)
{
    if ((NULL == handle) || (NULL == m_pDll) || (NULL == g_api.ConvertTimeStamp) || (NULL == pOrgTime))
        return VOOSMP_ERR_Pointer;
    
    VO_U32 nRet = g_api.ConvertTimeStamp(handle, pOrgTime, ullPlayingTime);
    
    if (VO_RET_SOURCE2_INVALIDPARAM == nRet) {
        if (0 < *pOrgTime) {
            *pOrgTime = ullPlayingTime;
        }
        
        return VOOSMP_ERR_ParamID;
    }
    
    return nRet;
}

VO_U32 VOOSMPADSManagerJNI::SetPlayingTime(VO_U64 ullPlayingTime)
{
    if ((NULL == handle) || (NULL == m_pDll) || (NULL == g_api.SetPlayingTime))
        return VOOSMP_ERR_Pointer;

	return g_api.SetPlayingTime(handle, ullPlayingTime);
}

VO_U32 VOOSMPADSManagerJNI::SetAction(VO_ADSMANAGER_ACTION nAction, VO_U64 ullPlayingTime)
{
    if ((NULL == handle) || (NULL == m_pDll) || (NULL == g_api.SetAction))
        return VOOSMP_ERR_Pointer;

	return g_api.SetAction(handle, nAction, ullPlayingTime);
}

VO_U32 VOOSMPADSManagerJNI::SetActionSync(VO_ADSMANAGER_ACTION nAction, VO_U64 ullPlayingTime, VO_VOID* pParam)
{
    if ((NULL == handle) || (NULL == m_pDll) || (NULL == g_api.SetActionSync))
        return VOOSMP_ERR_Pointer;
    
	return g_api.SetActionSync(handle, nAction, ullPlayingTime, pParam);
}

VO_U32 VOOSMPADSManagerJNI::GetContentDuration(VO_U64 * pullDuration)
{
    if ((NULL == handle) || (NULL == m_pDll) || (NULL == g_api.GetContentDuration) || (NULL == pullDuration))
        return VOOSMP_ERR_Pointer;
    
    return g_api.GetContentDuration(handle, pullDuration);
}

VO_BOOL VOOSMPADSManagerJNI::IsSeekable(VO_U64 ullPlayingTime)
{
    if ((NULL == handle) || (NULL == m_pDll) || (NULL == g_api.IsSeekable))
        return VO_FALSE;
    
    return g_api.IsSeekable(handle, ullPlayingTime);
}

VO_U32 VOOSMPADSManagerJNI::Seek(VO_U64 ullPlayingtTime, VO_U64 ullSeekPos, VO_ADSMANAGER_SEEKINFO* pInfo)
{
    if ((NULL == handle) || (NULL == m_pDll) || (NULL == g_api.GetContentDuration) || (NULL == pInfo))
        return VOOSMP_ERR_Pointer;
    
    return g_api.Seek(handle, ullPlayingtTime, ullSeekPos, pInfo);
}

VO_U32 VOOSMPADSManagerJNI::SetParam(VO_U32 nParamID, VO_PTR pParam)
{
    if ((NULL == handle) || (NULL == m_pDll) || (NULL == g_api.SetParam))
        return VOOSMP_ERR_Pointer;
    
    VO_U32 nRet = g_api.SetParam(handle, nParamID, pParam);
    
    if (nRet == VO_RET_SOURCE2_ADAPTIVESTREAMING_CHUNK_SKIP)
    {
        VOLOGI("ADS Wrapper nativeSetParam nRet == VO_RET_SOURCE2_ADAPTIVESTREAMING_CHUNK_SKIP");
        return VOOSMP_SRC_ERR_CHUNK_SKIP;
    }
    else if (nRet == VO_RET_SOURCE2_ADAPTIVESTREAMING_FORCETIMESTAMP)
    {
        VOLOGI("ADS Wrapper nativeSetParam nRet == VO_RET_SOURCE2_ADAPTIVESTREAMING_FORCETIMESTAMP");
        return VOOSMP_SRC_ERR_FORCETIMESTAMP;
    }
    
    return nRet;
}
