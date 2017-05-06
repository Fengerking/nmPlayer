    /************************************************************************
    *                                                                        *
    *        VisualOn, Inc. Confidential and Proprietary, 2003                *
    *                                                                        *
    ************************************************************************/
/*******************************************************************************
    File:        voOSMediaPlayer.cpp

    Contains:    voOSMediaPlayer class file

    Written by:  Jeff
 
    Change History (most recent first):
    2011-11-15        Jeff            Create file

*******************************************************************************/

#include "voOSMediaPlayer.h"
#include <stdio.h>
#include <string.h>
#include "voLog.h"
#include "voCapXMLParser.h"
#include "voStreamDownload.h"
#include "voCheckUseTime.h"
#include "voOSFunc.h"

#ifdef _IOS
#include <UIKit/UIKit.h>

#include "voLoadLibControl.h"
#include "voDRM2.h"
typedef VO_S32 (VO_API * VO_GET_DRM2_API_UUID)(VO_DRM2_API * pDRMHandle, VO_U32 uFlag);
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voOSLog voOSMediaPlayer::m_cLog;

const static int VO_INVALID_SELECT = -2;
const static int VO_OVER_TIME = 10;


voOSMediaPlayer::voOSMediaPlayer()
:m_bSeeking(false)
,m_bNativeRecoveryBlockEvent(false)
,m_bSuspendPlayAudio(false)
,m_bIsAppActive(true)
,m_nIsLive(-1)
,m_nOpenFlag(0)
,m_nSelectedAudio(VO_INVALID_SELECT)
,m_nSelectedSubtitle(VO_INVALID_SELECT)
,m_nlastStatus(VOOSMP_STATUS_INIT)
,m_nPosition(0)
,m_pOSEng(NULL)
,m_pSource(NULL)
,m_pDownloadFile(NULL)
,m_pVerificationInfo(NULL)
,m_pRTSP_Port(NULL)
,m_pProxy(NULL)
,m_pUserAgent(NULL)
,m_pDrmName(NULL)
,m_pDrmAPI(NULL)
,m_pDRMAdapter(NULL)
,m_pLicensePath(NULL)
,m_pLicenseText(NULL)
,m_pLicenseContent(NULL)
,m_pPathLib(NULL)
{
    memset(&m_cSendListener, 0, sizeof(m_cSendListener));
    m_cThisListener.pUserData = this;
	m_cThisListener.pListener = OnListener;
    
    memset(&m_cRequest_SendListener, 0, sizeof(m_cRequest_SendListener));
    m_cRequest_ThisListener.pUserData = this;
	m_cRequest_ThisListener.pListener = OnRequestListener;
    
    m_pCabXml = [[voCapXMLParser alloc] init];
    m_pStreamDownload = [[voStreamDownload alloc] init];
    
    memset(m_szCap, 0, sizeof(m_szCap));
    
    memset(&m_cStartCap, 0, sizeof(m_cStartCap));
}

voOSMediaPlayer::~voOSMediaPlayer()
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    UnInit();
    
    if (nil != m_pCabXml) {
        [(voCapXMLParser *)m_pCabXml release];
        m_pCabXml = NULL;
    }
    
    if (nil != m_pStreamDownload) {
        [(voStreamDownload *)m_pStreamDownload release];
        m_pStreamDownload = NULL;
    }
    
    if (NULL != m_pDownloadFile) {
        [[NSFileManager defaultManager] removeItemAtPath:(NSString *)m_pDownloadFile error:nil];
        [(NSString *)m_pDownloadFile release];
        m_pDownloadFile = NULL;
    }
    
    if (NULL != m_pVerificationInfo) {
        if (NULL != m_pVerificationInfo->pData) {
            delete [](char *)(m_pVerificationInfo->pData);
        }
        
        if (NULL != m_pVerificationInfo->pszResponse) {
            delete []m_pVerificationInfo->pszResponse;
        }
        
        delete m_pVerificationInfo;
        m_pVerificationInfo = NULL;
    }
    
    if (NULL != m_pRTSP_Port) {
        delete m_pRTSP_Port;
        m_pRTSP_Port = NULL;
    }
    
    if (NULL != m_pProxy) {
        if (NULL != m_pProxy->pszProxyHost) {
            delete [](m_pProxy->pszProxyHost);
            m_pProxy->pszProxyHost = NULL;
        }
        
        delete m_pProxy;
        m_pProxy = NULL;
    }
    
    if (NULL != m_pUserAgent) {
        delete []m_pUserAgent;
        m_pUserAgent = NULL;
    }
    
    if (NULL != m_pDrmName) {
        delete []m_pDrmName;
        m_pDrmName = NULL;
    }
    
    if (NULL != m_pDrmAPI) {
        delete []m_pDrmAPI;
        m_pDrmAPI = NULL;
    }
    
    if (NULL != m_pLicensePath) {
        delete []m_pLicensePath;
        m_pLicensePath = NULL;
    }
    
    if (NULL != m_pLicenseText) {
        delete []m_pLicenseText;
        m_pLicenseText = NULL;
    }
    
    if (NULL != m_pLicenseContent) {
        delete []m_pLicenseContent;
        m_pLicenseContent = NULL;
    }
    
    if (NULL != m_pPathLib) {
        delete []m_pPathLib;
        m_pPathLib = NULL;
    }
    
    m_cLog.flush();
}

int voOSMediaPlayer::Init(int nPlayerType, void* pInitParam, int nInitParamFlag)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if((nInitParamFlag & VOOSMP_FLAG_INIT_WORKING_PATH) && pInitParam)
	{
		VO_TCHAR* pWorkpath = (VO_TCHAR *)((VOOSMP_INIT_PARAM*)pInitParam)->pWorkingPath;
        
		if (pWorkpath)
		{
            ResetStr(&m_pPathLib, (const char *)pWorkpath);
            m_cLog.setPath(m_pPathLib);
		}
	}
    
    UnInit();
	
    VOLOGINIT(m_pPathLib);
    
    VOLOGI("Init with player type:%d", nPlayerType);
    
	int nRet = VOOSMP_ERR_Pointer;
	
	m_pOSEng = new voOSEngWrapper();
	m_pSource = new voOSSourceWrapper();
    
    if ((NULL == m_pSource) || (NULL == m_pOSEng)) {
		return VOOSMP_ERR_Pointer;
    }
	
    if (m_pPathLib)
    {
        m_pSource->SetWorkPath(m_pPathLib);
        m_pOSEng->SetWorkPath(m_pPathLib);
    }
    
    m_pSource->Create();
    
    m_pOSEng->SetParam (VOOSMP_PID_COMMON_LOGFUNC, m_cLog.getCB());
    m_pSource->SetParam (VOOSMP_PID_COMMON_LOGFUNC, m_cLog.getCB());
    
    if (m_pPathLib)
    {
        m_pSource->SetParam(VOOSMP_PID_PLAYER_PATH, m_pPathLib);
    }
    
    nRet = m_pOSEng->Init(nPlayerType, pInitParam, nInitParamFlag); //(VOOSMP_VOME2_PLAYER, NULL, 0);
	
    m_pOSEng->SetParam(VOOSMP_PID_LISTENER, &m_cThisListener);
    m_pOSEng->SetParam(VOOSMP_PID_ONREQUEST_LISTENER, &m_cRequest_ThisListener);
    
	return nRet;
}

int voOSMediaPlayer::UnInit()
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
	if (m_pOSEng)
	{
		m_pOSEng->Close();
        m_pOSEng->Uninit();
		delete m_pOSEng;
		m_pOSEng = NULL;
	}
    
    if (m_pSource != NULL)
	{
		m_pSource->Close();
		m_pSource->Uninit();
        m_pSource->Destroy();
		delete m_pSource;
        m_pSource = NULL;
	}
    
    VOLOGUNINIT();
    
    return VOOSMP_ERR_None;
}

int voOSMediaPlayer::SetView(void* hDrawWnd)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if (NULL == m_pOSEng || NULL == hDrawWnd) {
        return VOOSMP_ERR_Pointer;
    }
	
	return m_pOSEng->SetView(hDrawWnd);
}

int voOSMediaPlayer::Open(void* pSource, int nFlag, int nSourceType, void* pInitParam, int nInitParamFlag)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    // support inactive now
//    if (!m_bIsAppActive) {
//        return VOOSMP_ERR_Status;
//    }
    
    if (NULL == pSource) {
        return VOOSMP_ERR_Pointer;
    }
    
    VO_U64 nTime = voOS_GetSysTime();
    
    Stop();
    
    voMoreUNIXIgnoreSIGPIPE();
    
    if ((NULL == m_pSource) || (NULL == m_pOSEng))
		return VOOSMP_ERR_Pointer;
    
    void *pSourceSet = pSource;
    
    if (VOOSMP_FLAG_SOURCE_URL & nFlag) {
        NSString *path = [NSString stringWithFormat:@"%s", (char *)pSource];
        if ((nil != path) && [path hasPrefix:@"file://localhost/"]) {
            path = [path substringFromIndex:strlen("file://localhost")];
            pSourceSet = (void *)[path UTF8String];
        }
        
        if ((nil != path) && [path hasPrefix:@"file:///"]) {
            path = [path substringFromIndex:strlen("file://")];
            pSourceSet = (void *)[path UTF8String];
        }
        
        
        
        VOLOGI("Open url:%s", pSourceSet);
    }
    
    VO_U64 nTime1 = voOS_GetSysTime();
    
    if((nInitParamFlag & VOOSMP_FLAG_INIT_WORKING_PATH) && pInitParam)
	{
		VO_TCHAR* pWorkpath = (VO_TCHAR *)((VOOSMP_INIT_PARAM*)pInitParam)->pWorkingPath;
        
		if (pWorkpath)
		{
            ResetStr(&m_pPathLib, (const char *)pWorkpath);
		}
	}
    
    VOOSMP_INIT_PARAM cParam;
    memset(&cParam, 0, sizeof(cParam));
    
    if (NULL != m_pPathLib) {
        
        if ((nInitParamFlag & VOOSMP_FLAG_INIT_WORKING_PATH) && pInitParam && ((VOOSMP_INIT_PARAM*)pInitParam)->pWorkingPath) {
            // do nothine
        }
        else {
            
            nInitParamFlag |= VOOSMP_FLAG_INIT_WORKING_PATH;
            
            if (NULL == pInitParam) {
                pInitParam = &cParam;
            }
            
            if (NULL == ((VOOSMP_INIT_PARAM*)pInitParam)->pWorkingPath) {
                ((VOOSMP_INIT_PARAM*)pInitParam)->pWorkingPath = m_pPathLib;
            }
        }
    }
    
	int nRet = m_pSource->Init(pSourceSet, nFlag, nSourceType, pInitParam, nInitParamFlag);
	if (VOOSMP_ERR_None != nRet) {
        return nRet;
    }
    
    VO_U64 nTime2 = voOS_GetSysTime();
    
    map<VO_U32, int>::iterator iter = m_cParamMap.begin();
    while (iter != m_cParamMap.end())
    {
        m_pSource->SetParam(iter->first, &(iter->second));
        ++iter;
    }
    
    m_pSource->SetParam (VOOSMP_PID_COMMON_LOGFUNC, m_cLog.getCB());
    
	m_pSource->SetParam(VOOSMP_PID_LISTENER, &m_cThisListener);
    m_pSource->SetParam(VOOSMP_PID_ONREQUEST_LISTENER, &m_cRequest_ThisListener);
    
    if (NULL != m_pVerificationInfo) {
        m_pSource->SetParam(VOOSMP_SRC_PID_DOHTTPVERIFICATION, m_pVerificationInfo);
    }
    
    if (NULL != m_pProxy) {
        m_pSource->SetParam(VOOSMP_SRC_PID_HTTP_PROXY_INFO, m_pProxy);
    }
            
    if (NULL != m_pRTSP_Port) {
        m_pSource->SetParam(VOOSMP_SRC_PID_RTSP_CONNECTION_PORT, m_pRTSP_Port);
    }
    
    if (0 < m_cStartCap.nBitRate) {
        m_pSource->SetParam(VOOSMP_SRC_PID_BA_STARTCAP, &m_cStartCap);
    }
    
    if (NULL != m_pLicensePath) {
        m_pSource->SetParam(VOOSMP_PID_LICENSE_FILE_PATH, m_pLicensePath);
    }
    
    if (NULL != m_pLicenseText) {
        m_pSource->SetParam(VOOSMP_PID_LICENSE_TEXT, m_pLicenseText);
    }
    
    if (NULL != m_pLicenseContent) {
        m_pSource->SetParam(VOOSMP_PID_LICENSE_CONTENT, m_pLicenseContent);
    }
    
    if (NULL != m_pUserAgent) {
        VOOSMP_SRC_HTTP_HEADER cHeader;
        memset(&cHeader, 0, sizeof(cHeader));
        
        cHeader.pszHeaderName = (unsigned char* )"User-Agent";
        cHeader.pszHeaderValue = (unsigned char* )m_pUserAgent;
        
        m_pSource->SetParam(VOOSMP_SRC_PID_HTTP_HEADER, &cHeader);
    }
    
    if (NULL != m_pDRMAdapter) {
        m_pSource->SetParam(VOOSMP_SRC_PID_DRM_ADAPTER_OBJECT, m_pDRMAdapter);
    }
    
    // Don't need keep since OSSource lifecycle sync
//    if (NULL != m_pDrmName) {
//        m_pSource->SetParam(VOOSMP_SRC_PID_DRM_FILE_NAME, m_pDrmName);
//    }
//    else {
//        m_pSource->SetParam(VOOSMP_SRC_PID_DRM_FILE_NAME, (void *)"voDRMLibName");
//    }
//    
//    if (NULL != m_pDrmAPI) {
//        m_pSource->SetParam(VOOSMP_SRC_PID_DRM_API_NAME, m_pDrmAPI);
//    }
//    else {
//        m_pSource->SetParam(VOOSMP_SRC_PID_DRM_API_NAME, (void *)"voGetDRMAPI");
//    }
    
    int nType = VOOSMP_VOME2_PLAYER;
    if (VOOSMP_ERR_None == m_pOSEng->GetParam(VOOSMP_PID_PLAYER_TYPE, &nType)) {
        
        if (VOOSMP_AV_PLAYER == nType) {
            int nDisalbe = 1;
            m_pSource->SetParam(VOOSMP_SRC_PID_DISABLE_CPU_ADAPTION, &nDisalbe);
        }
    }
    
    if (VOOSMP_FLAG_SOURCE_READBUFFER & nFlag) {
        void *ReadBufPtr = NULL;
        m_pSource->GetParam(VOOSMP_SRC_PID_FUNC_READ_BUF, &ReadBufPtr);
        
        if (VOOSMP_ERR_None == ParserCap()) {
            
            VOOSMP_PERF_DATA *pData = [(voCapXMLParser *)m_pCabXml getPerfData];
            if (NULL != pData) {
                SetParam(VOOSMP_PID_PERFORMANCE_DATA, pData);
            }
        }
        
        VO_U64 nTime3 = voOS_GetSysTime();
        
        nRet = m_pSource->Open();
        
        VO_U64 nTime4 = voOS_GetSysTime();
        
        if (VOOSMP_ERR_None != nRet) {
            return nRet;
        }
        
        if (ReadBufPtr)
        {
            pSourceSet = ReadBufPtr;
            m_nOpenFlag = VOOSMP_FLAG_SOURCE_READBUFFER;
            
            VOOSMP_READ_SUBTITLE_FUNC cSubtitleCB;
            cSubtitleCB.pUserData = this;
            cSubtitleCB.ReadSubtitle = OnReadSubtitle;
            m_pOSEng->SetParam(VOOSMP_PID_READ_SUBTITLE_CALLBACK, &cSubtitleCB);
            
            nRet = m_pOSEng->Open(pSourceSet, m_nOpenFlag);
        }
        else {
            nRet = VOOSMP_ERR_Unknown;
        }
        
        VOLOGI("Open pre:%lld, init source:%lld, set property:%lld, source:%lld, engine:%lld", nTime1 - nTime, nTime2 - nTime1, nTime3 - nTime2, nTime4 - nTime3, voOS_GetSysTime() - nTime4);
    }
    
	return nRet;
}

int voOSMediaPlayer::GetProgramCount (int* pStreamCount)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if ((NULL == m_pSource) || (NULL == pStreamCount)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pSource->GetProgramCount(pStreamCount);
}

int voOSMediaPlayer::GetProgramInfo (int nStream, VOOSMP_SRC_PROGRAM_INFO** ppStreamInfo)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if ((NULL == m_pSource) || (NULL == ppStreamInfo)) {
        return VOOSMP_ERR_Pointer;
    }
    
    if ([NSRunLoop mainRunLoop] != [NSRunLoop currentRunLoop]) {
        return VOOSMP_ERR_Retry;
    }
    
    return m_pSource->GetProgramInfo(nStream, ppStreamInfo);
}

int voOSMediaPlayer::GetCurTrackInfo (int nOutPutType, VOOSMP_SRC_TRACK_INFO** ppTrackInfo)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if ((NULL == m_pSource) || (NULL == ppTrackInfo)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pSource->GetCurTrackInfo(nOutPutType, ppTrackInfo);
}

int voOSMediaPlayer::GetSample (int nOutPutType, void* pSample)
{
    if ((NULL == m_pSource) || (NULL == m_pOSEng))
		return VOOSMP_ERR_Pointer;
    
    if (NULL == pSample) {
        return VOOSMP_ERR_Pointer;
    }
    
    if (VOOSMP_SS_SUBTITLE == nOutPutType) {
        return m_pOSEng->GetSubtitleSample((voSubtitleInfo*)pSample);
    }
    
    return m_pSource->GetSample(nOutPutType, pSample);
}

int voOSMediaPlayer::SelectProgram (int nProgram)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if (NULL == m_pSource) {
        return VOOSMP_ERR_Pointer;
    }
    
    VOLOGI("Select program: %d", nProgram);
    
    return m_pSource->SelectProgram(nProgram);
}

int voOSMediaPlayer::SelectStream (int nStream)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if (NULL == m_pSource) {
        return VOOSMP_ERR_Pointer;
    }
    
    VOLOGI("Select stream: %d", nStream);
    
    return m_pSource->SelectStream(nStream);
}

int voOSMediaPlayer::SelectTrack (int nTrack)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if (NULL == m_pSource) {
        return VOOSMP_ERR_Pointer;
    }
    
    VOLOGI("Select track: %d", nTrack);
    
    // We need get pos from engine,becasue get pos from source is not correct if seek not complete while live url playback
    int nPos = 0;
    if(m_pOSEng)
        nPos = m_pOSEng->GetPos();

    VO_U64 nTime = voOS_GetSysTime();
    
    int nRet = m_pSource->SelectTrack(nTrack);

    VO_U64 nTime1 = voOS_GetSysTime();
    
    if (VOOSMP_ERR_Implement == nRet) {
        return VOOSMP_ERR_None;
    }
    
    SetPos(nPos);
    //SetPos(GetPos());
    
    VOLOGI("SelectTrack source:%lld, continue:%lld", nTime1 - nTime, voOS_GetSysTime() - nTime1);
    
    return nRet;
}

int voOSMediaPlayer::SelectLanguage (int nIndex)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if ((NULL == m_pSource) || (NULL == m_pOSEng))
		return VOOSMP_ERR_Pointer;
    
    VOLOGI("Select language: %d", nIndex);
    
    // We need get pos from engine,becasue get pos from source is not correct if seek not complete while live url playback
    int nPos = 0;
    if(m_pOSEng)
        nPos = m_pOSEng->GetPos();
    
    VOOSMP_SUBTITLE_LANGUAGE_INFO *pInfo = NULL;
    int nRet = m_pSource->GetLanguage(&pInfo);
    
    if ((VOOSMP_ERR_None == nRet)
        && (NULL != pInfo)
        && (0 < (pInfo->nLanguageCount))) {
        nRet = m_pSource->SelectLanguage(nIndex);
    }
    else {
        nRet = m_pOSEng->SelectLanguage(nIndex);
    }
    
    if (VOOSMP_ERR_Implement == nRet) {
        return VOOSMP_ERR_None;
    }

//    if (!IsLive()) {
        SetPos(nPos);
        //SetPos(GetPos());
//    }
    
    return nRet;
}

int voOSMediaPlayer::GetLanguage (VOOSMP_SUBTITLE_LANGUAGE_INFO** ppLangInfo)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if ((NULL == m_pSource) || (NULL == m_pOSEng))
		return VOOSMP_ERR_Pointer;
    
    if (NULL == ppLangInfo) {
        return VOOSMP_ERR_Pointer;
    }
    
    int nRet = m_pSource->GetLanguage(ppLangInfo);
    
    if ((VOOSMP_ERR_None == nRet)
        && (NULL != *ppLangInfo)
        && (0 < (*ppLangInfo)->nLanguageCount)) {
        return nRet;
    }
    
    return m_pOSEng->GetLanguage(ppLangInfo);
}

int voOSMediaPlayer::Close() {
    
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if (nil != m_pStreamDownload) {
        [(voStreamDownload *)m_pStreamDownload stop];
    }
    
    if (NULL != m_pDownloadFile) {
        [[NSFileManager defaultManager] removeItemAtPath:(NSString *)m_pDownloadFile error:nil];
        [(NSString *)m_pDownloadFile release];
        m_pDownloadFile = NULL;
    }
    
    m_nIsLive = -1;
    
//    if (NULL != m_pVerificationInfo) {
//        if (NULL != m_pVerificationInfo->pData) {
//            delete [](char *)(m_pVerificationInfo->pData);
//        }
//        
//        if (NULL != m_pVerificationInfo->pszResponse) {
//            delete []m_pVerificationInfo->pszResponse;
//        }
//        
//        delete m_pVerificationInfo;
//        m_pVerificationInfo = NULL;
//    }
    
    if ((NULL == m_pSource) || (NULL == m_pOSEng))
		return VOOSMP_ERR_Pointer;
    
    int nRet = m_pOSEng->Close();
    
    if (nRet != VOOSMP_ERR_None)
    {
        VOLOGE("occur error, error code:%d", nRet);
    }
    
    // don't deal return value of close source, source be uninit when stop
    m_pSource->Close();
    
    return VOOSMP_ERR_None;
}

int voOSMediaPlayer::Run(void)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    // support inactive now
//    if (!m_bIsAppActive) {
//        return VOOSMP_ERR_Status;
//    }
    
    if ((NULL == m_pSource) || (NULL == m_pOSEng))
		return VOOSMP_ERR_Pointer;

    VO_U64 nTime = voOS_GetSysTime();
    
    int nRet = m_pSource->Run();
        
    if (nRet != VOOSMP_ERR_None)
    {
        VOLOGE("occur error, error code:%d", nRet);
        return nRet;
    }

    VO_U64 nTime1 = voOS_GetSysTime();
    
    nRet = m_pOSEng->Run();
    
    VOLOGI("Run source:%lld, engine:%lld", nTime1 - nTime, voOS_GetSysTime() - nTime1);
    
	return nRet;
}

int voOSMediaPlayer::Pause(void)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if ((NULL == m_pSource) || (NULL == m_pOSEng))
		return VOOSMP_ERR_Pointer;
    
    VO_U64 nTime = voOS_GetSysTime();
    
    int nRet = m_pOSEng->Pause();
    
    VO_U64 nTime1 = voOS_GetSysTime();
    
    if (nRet != VOOSMP_ERR_None)
    {
        VOLOGE("occur error, error code:%d", nRet);
    }
    
    nRet = m_pSource->Pause();
    
    VOLOGI("Pause engine:%lld, source:%lld", nTime1 - nTime, voOS_GetSysTime() - nTime1);
    
    return nRet;
}

int voOSMediaPlayer::Stop(void)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    m_bSeeking = false;
    m_bNativeRecoveryBlockEvent = false;
    
    if ((NULL == m_pSource) || (NULL == m_pOSEng))
		return VOOSMP_ERR_Pointer;
    
    VO_U64 nTime = voOS_GetSysTime();
    
    voNSAutoLock cAuto(&m_cLock);
    
    int nRet = m_pOSEng->Stop();
    
    if (nRet != VOOSMP_ERR_None)
    {
        VOLOGE("occur error, error code:%d", nRet);
    }
    
    VO_U64 nTime1 = voOS_GetSysTime();
    
    nRet = m_pSource->Stop();
    nRet = m_pSource->Close();
    nRet = m_pSource->Uninit();
    
    voMoreUNIXRecoverySIGPIPE();
    
    m_cLog.flush();
    
    VOLOGI("Stop engine:%lld, source:%lld", nTime1 - nTime, voOS_GetSysTime() - nTime1);
    
    return nRet;
}

int voOSMediaPlayer::Suspend(bool bPlayAudio)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    VO_U64 nTime = voOS_GetSysTime();
    
    int nRet = VOOSMP_ERR_None;
    
    if (m_bIsAppActive) {
        
        m_bIsAppActive = false;
        
        m_bSuspendPlayAudio = bPlayAudio;
        
        if ((NULL == m_pSource) || (NULL == m_pOSEng)) {
            return VOOSMP_ERR_Pointer;
        }
        
        m_pOSEng->GetParam(VOOSMP_PID_STATUS, &m_nlastStatus);
        
        m_nPosition = 0;
        
        // fix avplayer issue: pause before enter suspend status
        //if (VOOSMP_STATUS_RUNNING == m_nlastStatus)
        {
            m_nPosition = GetPos();
        }
        
        {
            int nType = VOOSMP_VOME2_PLAYER;
            
            if (VOOSMP_ERR_None == m_pOSEng->GetParam(VOOSMP_PID_PLAYER_TYPE, &nType)) {
                
                if (VOOSMP_VOME2_PLAYER == nType) {
                    m_pOSEng->SetParam(VOOSMP_PID_APPLICATION_SUSPEND, &m_bSuspendPlayAudio);
                    
                    VOLOGI("Suspend with audio:%lld", voOS_GetSysTime() - nTime);
                    //return VOOSMP_ERR_None;
                }
            }
        }
        
        if (!m_bSuspendPlayAudio && VOOSMP_STATUS_PAUSED != m_nlastStatus)
        {
            nRet = Pause();
        }
    }
    
    VOLOGI("Suspend with pause:%lld", voOS_GetSysTime() - nTime);
    
    return nRet;
}

int voOSMediaPlayer::Resume(void)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    VO_U64 nTime = voOS_GetSysTime();
    
    m_bIsAppActive = true;
    
    int nRet = VOOSMP_ERR_None;
    
    //if (VOOSMP_STATUS_RUNNING == m_nlastStatus)
    {
        
        if ((NULL == m_pSource) || (NULL == m_pOSEng)) {
            return VOOSMP_ERR_Pointer;
        }
        
        int nType = VOOSMP_VOME2_PLAYER;
        
        m_pOSEng->GetParam(VOOSMP_PID_PLAYER_TYPE, &nType);
        
        if (VOOSMP_VOME2_PLAYER == nType) {
            
            nRet = m_pOSEng->SetParam(VOOSMP_PID_APPLICATION_RESUME, &m_bSuspendPlayAudio);
        }
        else {
            //don't seek if use avplayer while pause
            //maybe we don't need do seeking any more
            if(VOOSMP_STATUS_RUNNING == m_nlastStatus)
                SetPos(m_nPosition);
        }

        if (!m_bSuspendPlayAudio && VOOSMP_STATUS_RUNNING == m_nlastStatus)
        {
            nRet = Run();
        }
    }
    
    m_nlastStatus = VOOSMP_STATUS_INIT;
    
    VOLOGI("Resume:%lld", voOS_GetSysTime() - nTime);
    
    return nRet;
}

int voOSMediaPlayer::SetPos(int nCurPos) {
    
#ifdef _VOLOG_INFO
    char over[512];
    snprintf(over, 512, "%s, position:%d", __FUNCTION__, nCurPos);
    voCheckUseTime cOverTime(VO_OVER_TIME, over);
#endif
    
    m_bSeeking = true;
    m_bNativeRecoveryBlockEvent = false;
    
    return SetPosInner(nCurPos);
}

int voOSMediaPlayer::SetPosInner(int nCurPos)
{
    if ((NULL == m_pSource) || (NULL == m_pOSEng))
		return VOOSMP_ERR_Pointer;
    
    VO_U64 nTime = voOS_GetSysTime();
    
    voNSAutoLock cAuto(&m_cLock);
    
    int nRet = VOOSMP_ERR_None;
    
    VOOSMP_STATUS status = VOOSMP_STATUS_INIT;
    
    nRet = m_pOSEng->GetParam(VOOSMP_PID_STATUS, &status);

    if (VOOSMP_STATUS_RUNNING == status)
    {
        Pause();
    }
    
    VO_U64 nTime1 = voOS_GetSysTime();
    
    long long lCurPos = nCurPos;
    nRet = m_pSource->SetPos(&lCurPos);
    
    if (nRet != VOOSMP_ERR_None)
    {
        VOLOGE("occur error, error code:%d", nRet);
        
        if (VOOSMP_STATUS_RUNNING == status)
        {
            m_pSource->Run();
            m_pOSEng->Run();
        }
        
        return nRet;
    }
    
    VO_U64 nTime2 = voOS_GetSysTime();
    
    nRet = m_pOSEng->SetPos(nCurPos);
    
    VO_U64 nTime3 = voOS_GetSysTime();
    
    if (VOOSMP_STATUS_RUNNING == status)
    {
        m_pSource->Run();
        m_pOSEng->Run();
    }
    
    VOLOGI("Seek step1:%lld, source:%lld, engine:%lld, continue:%lld", nTime1 - nTime, nTime2 - nTime1, nTime3 - nTime2, voOS_GetSysTime() - nTime3);
    
	return nRet;
}

int voOSMediaPlayer::GetPos()
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if (NULL == m_pOSEng) {
        return VOOSMP_ERR_Pointer;
    }
    
//    if (IsLive()) {
//        VOOSMP_SRC_DVRINFO cInfo;
//        memset(&cInfo, 0, sizeof(cInfo));
//        if (VOOSMP_ERR_None != GetParam(VOOSMP_SRC_PID_DVRINFO, &cInfo)) {
//            return -1;
//        }
//        
//        return cInfo.uPlayingTime;
//    }
    
    return m_pOSEng->GetPos();
}

int voOSMediaPlayer::GetDuration(long long* pDuration)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if (NULL == m_pSource || NULL == pDuration) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pSource->GetDuration(pDuration);
}

int voOSMediaPlayer::GetParam(int nParamID, void* pValue)
{
#ifdef _VOLOG_INFO
    char over[512];
    snprintf(over, 512, "%s, nParamID:%d", __FUNCTION__, nParamID);
    voCheckUseTime cOverTime(VO_OVER_TIME, over);
#endif
    
    if ((NULL == m_pSource) || (NULL == m_pOSEng))
		return VOOSMP_ERR_Pointer;
    
    if ((NULL == pValue)
        && (VOOSMP_PID_CAPTURE_VIDEO_IMAGE != nParamID)
        ) {
        return VOOSMP_ERR_Pointer;
    }
    
    // disable at present
    if (VOOSMP_PID_CAPTURE_VIDEO_IMAGE == nParamID) {
        return VOOSMP_ERR_Implement;
    }
    
    if (VOOSMP_PID_COMMON_LOGFUNC == nParamID) {
        *((VO_LOG_PRINT_CB**)pValue) = m_cLog.getCB();
    }
    else if (VOOSMP_PID_RETRIEVE_SEI_INFO == nParamID) {
        VOOSMP_SEI_INFO *pInfo = (VOOSMP_SEI_INFO *)pValue;
        m_pOSEng->GetSEISample(pInfo);
    }
    else if (VOOSMP_PID_SCREEN_BRIGHTNESS_VALUE == nParamID) {
        if (NULL == pValue) {
            return VOOSMP_ERR_Pointer;
        }
#ifdef _IOS
        if ([UIScreen instancesRespondToSelector:
             @selector (brightness)]) {
            
            *(int *)pValue = [UIScreen mainScreen].brightness * 100;
            
            return VOOSMP_ERR_None;
        }
#endif
        *(int *)pValue = 0;
        
        return VOOSMP_ERR_Implement;
    }
    else if (VOOSMP_PID_SCREEN_BRIGHTNESS_MODE == nParamID) {
        return VOOSMP_ERR_Implement;
    }
    else if (VOOSMP_SRC_PID_DRM_UNIQUE_IDENTIFIER == nParamID) {
        
        if (!(m_pSource->IsBeInit())) {
#ifdef _IOS
            voGetModuleRealAPI pFunction = (voGetModuleRealAPI)voGetModuleAdapterFunc((char *)"voGetDRMAPI");
            
            VO_DRM2_API m_fDRMCallBack;
            VO_GET_DRM2_API_UUID pGetDRM2API = NULL;
            if (pFunction) {
                pGetDRM2API = (VO_GET_DRM2_API_UUID)pFunction();
            }
            
            if (pGetDRM2API == NULL)
                return VOOSMP_ERR_Implement;
            
            pGetDRM2API(&m_fDRMCallBack, 0);
            
            if (m_fDRMCallBack.GetParameter == NULL)
                return VOOSMP_ERR_Implement;
            
            return m_fDRMCallBack.GetParameter(NULL, VO_PID_DRM2_UNIQUE_IDENTIFIER, pValue);
#endif
        }
        
        return m_pSource->GetParam(nParamID, pValue);
    }
    else if (VOOSMP_SRC_PID_DRM_FILE_NAME <= nParamID)
    {
        return m_pSource->GetParam(nParamID, pValue);
    }
    
    return m_pOSEng->GetParam(nParamID, pValue);
}

int voOSMediaPlayer::ParserCap()
{
    VOOSMP_CPU_INFO cInfo;
    memset(&cInfo, 0, sizeof(cInfo));
    
    int nRet = m_pOSEng->GetParam(VOOSMP_PID_CPU_INFO, &cInfo);
    if (VOOSMP_ERR_None != nRet)
    {
        return nRet;
    }
    
    bool bRet = [(voCapXMLParser *)m_pCabXml parserXML:m_szCap pInfo:&cInfo];
    
    if (!bRet) {
        return VOOSMP_ERR_Unknown;
    }
    
    return VOOSMP_ERR_None;
}

int voOSMediaPlayer::SetParam(int nParamID, void* pValue)
{
#ifdef _VOLOG_INFO
    char over[512];
    snprintf(over, 512, "%s, nParamID:%d", __FUNCTION__, nParamID);
    voCheckUseTime cOverTime(VO_OVER_TIME, over);
#endif
    
    if ((NULL == m_pSource) || (NULL == m_pOSEng))
		return VOOSMP_ERR_Pointer;
    
    if (0X0CA51E3C == nParamID)
    {
        if (NULL == pValue) {
            return VOOSMP_ERR_Pointer;
        }
        
        if (0 == *(int*)pValue) {
            m_cLog.setUseNSLog(false);
        }
        else {
            m_cLog.setUseNSLog(true);
        }
        
        return VOOSMP_ERR_None;
    }
    else if (VOOSMP_PID_PERFORMANCE_DATA == nParamID)
    {
        if (NULL == pValue) {
            return VOOSMP_ERR_Pointer;
        }
        
        if (0 == ((VOOSMP_PERF_DATA *)pValue)->nBitRate) {
            return VOOSMP_ERR_Status;
        }
        
        return m_pSource->SetParam(nParamID, pValue);
    }
    else if (VOOSMP_PID_SUBTITLE_FILE_NAME == nParamID)
    {
        return SetSubtitlePath((const char *)pValue);
    }
    else if (VOOSMP_PID_LISTENER == nParamID) {
        VOOSMP_LISTENERINFO *pInfo = (VOOSMP_LISTENERINFO *)pValue;
        if (NULL == pInfo) {
            m_cSendListener.pUserData = NULL;
            m_cSendListener.pListener = NULL;
            return VOOSMP_ERR_None;
        }
        
        m_cSendListener.pUserData = pInfo->pUserData;
        m_cSendListener.pListener = pInfo->pListener;
        
        return VOOSMP_ERR_None;
    }
    else if (VOOSMP_PID_ONREQUEST_LISTENER == nParamID) {
        VOOSMP_LISTENERINFO *pInfo = (VOOSMP_LISTENERINFO *)pValue;
        if (NULL == pInfo) {
            m_cRequest_SendListener.pUserData = NULL;
            m_cRequest_SendListener.pListener = NULL;
            return VOOSMP_ERR_None;
        }
        
        m_cRequest_SendListener.pUserData = pInfo->pUserData;
        m_cRequest_SendListener.pListener = pInfo->pListener;
        
        return VOOSMP_ERR_None;
    }
    else if (VOOSMP_PID_APPLICATION_SUSPEND == nParamID) {
        bool bAudio = false;
        if ((NULL != pValue) && (0 != *(int *)pValue)) {
            bAudio = true;
        }
        return Suspend(bAudio);
    }
    else if (VOOSMP_PID_APPLICATION_RESUME == nParamID) {
        return Resume();
    }
    else if (VOOSMP_PID_SCREEN_BRIGHTNESS_VALUE == nParamID) {
        if (NULL == pValue) {
            return VOOSMP_ERR_Pointer;
        }
#ifdef _IOS
        if ([UIScreen instancesRespondToSelector:
             @selector (setBrightness:)]) {
            
            float fValue = (float)(*(int *)pValue) / 100;
            [UIScreen mainScreen].brightness = fValue;
            
            return VOOSMP_ERR_None;
        }
#endif
        return VOOSMP_ERR_Implement;
    }
    else if (VOOSMP_PID_SCREEN_BRIGHTNESS_MODE == nParamID) {
        return VOOSMP_ERR_Implement;
    }
    else if (VOOSMP_PID_ZOOM_MODE == nParamID)
    {
        if (NULL != pValue) {
            VOLOGI("Set VOOSMP_PID_ZOOM_MODE:%d", *(int *)pValue);
        }
    }
    else if (VOOSMP_PID_LICENSE_FILE_PATH == nParamID) {
        ResetStr(&m_pLicensePath, (const char *)pValue);
    }
    else if (VOOSMP_PID_LICENSE_TEXT == nParamID) {
        ResetStr(&m_pLicenseText, (const char *)pValue);
    }
    else if (VOOSMP_PID_LICENSE_CONTENT == nParamID) {
        
        if(m_pLicenseContent)
        {
            delete []m_pLicenseContent;
            m_pLicenseContent = NULL;
        }
        
        if (NULL != pValue) {
            
			m_pLicenseContent = new char[32768+32];
			if(m_pLicenseContent == NULL)
				return VOOSMP_ERR_OutMemory;
			memset(m_pLicenseContent, 0, 32768+32);
			memcpy (m_pLicenseContent, (char *)pValue, 32768*sizeof(char));
        }
    }
    else if(VOOSMP_PID_AUDIO_PLAYBACK_SPEED == nParamID)
    {
        // It need set it into both source and engine
        SetSrcParam(nParamID, pValue);
        return m_pOSEng->SetParam(nParamID, pValue);
    }
    else if (VOOSMP_SRC_PID_DRM_FILE_NAME <= nParamID)
    {
        return SetSrcParam(nParamID, pValue);
    }
    
    
    return m_pOSEng->SetParam(nParamID, pValue);
}

int voOSMediaPlayer::SetSrcParam(int nParamID, void* pValue)
{
    if (NULL == m_pSource)
		return VOOSMP_ERR_Pointer;
    
    if (VOOSMP_SRC_PID_CAP_TABLE_PATH == nParamID) {
        
        if (NULL == pValue) {
            return VOOSMP_ERR_Pointer;
        }
        
        strncpy(m_szCap, (const char *)pValue, 1024);
        
        if (VOOSMP_ERR_None == ParserCap()) {
            
            VOOSMP_PERF_DATA *pData = [(voCapXMLParser *)m_pCabXml getPerfData];
            SetParam(VOOSMP_PID_PERFORMANCE_DATA, pData);
            
            return VOOSMP_ERR_None;
        }
        
        return VOOSMP_ERR_Unknown;
    }
    else if (VOOSMP_SRC_PID_DOHTTPVERIFICATION == nParamID) {
        
        if (NULL == pValue) {
            return VOOSMP_ERR_Pointer;
        }
        
        VOOSMP_SRC_VERIFICATIONINFO *pInfo = (VOOSMP_SRC_VERIFICATIONINFO *)pValue;
        
        if (NULL != m_pVerificationInfo) {
            if (NULL != m_pVerificationInfo->pData) {
                delete [](char *)(m_pVerificationInfo->pData);
            }
            
            if (NULL != m_pVerificationInfo->pszResponse) {
                delete []m_pVerificationInfo->pszResponse;
            }
        }
        else {
            m_pVerificationInfo = new VOOSMP_SRC_VERIFICATIONINFO;
        }
        
        memset(m_pVerificationInfo, 0, sizeof(VOOSMP_SRC_VERIFICATIONINFO));
        
        m_pVerificationInfo->pUserData = pInfo->pData;
        
        if ((0 < pInfo->nDataSize) && (NULL != pInfo->pData)) {
            m_pVerificationInfo->pData = new char[pInfo->nDataSize];
            memcpy(m_pVerificationInfo->pData, pInfo->pData, pInfo->nDataSize);
            m_pVerificationInfo->nDataSize = pInfo->nDataSize;
            m_pVerificationInfo->nDataFlag = pInfo->nDataFlag;
        }
        
        if ((0 < pInfo->nResponseSize) && (NULL != pInfo->pszResponse)) {
            m_pVerificationInfo->pszResponse = new char[pInfo->nResponseSize];
            memcpy(m_pVerificationInfo->pszResponse, pInfo->pszResponse, pInfo->nResponseSize);
            m_pVerificationInfo->nResponseSize = pInfo->nResponseSize;
        }
        
        if (!(m_pSource->IsBeInit())) {
            return VOOSMP_ERR_None;
        }
    }
    else if (VOOSMP_SRC_PID_HTTP_PROXY_INFO == nParamID) {
        if (NULL == pValue) {
            return VOOSMP_ERR_Pointer;
        }
        
        VOOSMP_SRC_HTTP_PROXY *pInfo = (VOOSMP_SRC_HTTP_PROXY *)pValue;
        
        if (NULL != m_pProxy) {
            if (NULL != m_pProxy->pszProxyHost) {
                delete [](char *)(m_pProxy->pszProxyHost);
            }
        }
        else {
            m_pProxy = new VOOSMP_SRC_HTTP_PROXY;
        }
        
        memset(m_pProxy, 0, sizeof(VOOSMP_SRC_HTTP_PROXY));
        
        ResetStr((char **)&(m_pProxy->pszProxyHost), (const char *)pInfo->pszProxyHost);
        m_pProxy->nProxyPort = pInfo->nProxyPort;
        
        if (!(m_pSource->IsBeInit())) {
            return VOOSMP_ERR_None;
        }
    }
    else if (VOOSMP_SRC_PID_BA_STARTCAP == nParamID) {
        if (NULL == pValue) {
            return VOOSMP_ERR_Pointer;
        }
        
        memcpy(&m_cStartCap, pValue, sizeof(m_cStartCap));
        
        if (!(m_pSource->IsBeInit())) {
            return VOOSMP_ERR_None;
        }
    }
    else if (VOOSMP_SRC_PID_RTSP_CONNECTION_PORT == nParamID) {
        
        if (NULL == pValue) {
            return VOOSMP_ERR_Pointer;
        }
        
        VOOSMP_SRC_RTSP_CONNECTION_PORT *pInfo = (VOOSMP_SRC_RTSP_CONNECTION_PORT *)pValue;
        
        if (NULL == m_pRTSP_Port) {
            m_pRTSP_Port = new VOOSMP_SRC_RTSP_CONNECTION_PORT;
        }
        
        memcpy(m_pRTSP_Port, pInfo, sizeof(VOOSMP_SRC_RTSP_CONNECTION_PORT));
        
        if (!(m_pSource->IsBeInit())) {
            return VOOSMP_ERR_None;
        }
    }
    else if (VOOSMP_SRC_PID_HTTP_HEADER == nParamID) {
        
        VOOSMP_SRC_HTTP_HEADER *pHeader = (VOOSMP_SRC_HTTP_HEADER *)pValue;
        if ((NULL == pHeader) || (NULL == pHeader->pszHeaderName) || (NULL == pHeader->pszHeaderValue)) {
            return VOOSMP_ERR_Pointer;
        }
        
        if (0 == vostrcmp("User-Agent", (const char *)(pHeader->pszHeaderName))) {
            ResetStr(&m_pUserAgent, (const char *)(pHeader->pszHeaderValue));
            
            if (!(m_pSource->IsBeInit())) {
                return VOOSMP_ERR_None;
            }
        }
    }
    // Don't need keep since OSSource lifecycle sync
//    else if (VOOSMP_SRC_PID_DRM_FILE_NAME == nParamID) {
//        if (NULL == pValue) {
//            return VOOSMP_ERR_Pointer;
//        }
//        
//        ResetStr(&m_pDrmName, (const char *)pValue);
//        
//        if (!(m_pSource->IsBeInit())) {
//            return VOOSMP_ERR_None;
//        }
//    }
//    else if (VOOSMP_SRC_PID_DRM_API_NAME == nParamID) {
//        if (NULL == pValue) {
//            return VOOSMP_ERR_Pointer;
//        }
//        
//        ResetStr(&m_pDrmAPI, (const char *)pValue);
//        
//        if (!(m_pSource->IsBeInit())) {
//            return VOOSMP_ERR_None;
//        }
//    }
    else if ((VOOSMP_SRC_PID_CC_AUTO_SWITCH_DURATION == nParamID)
             || (VOOSMP_SRC_PID_SOCKET_CONNECTION_TYPE == nParamID)
             || (VOOSMP_SRC_PID_DOWNLOAD_FAIL_MAX_TOLERANT_COUNT == nParamID)
             || (VOOSMP_SRC_PID_DISABLE_CPU_ADAPTION == nParamID)
             || (VOOSMP_SRC_PID_BUFFER_START_BUFFERING_TIME == nParamID)
             || (VOOSMP_SRC_PID_BUFFER_BUFFERING_TIME == nParamID)
             || (VOOSMP_SRC_PID_BA_WORKMODE == nParamID)
             || (VOOSMP_SRC_PID_LOW_LATENCY_MODE == nParamID)
             || (VOOSMP_SRC_PID_BUFFER_MAX_SIZE == nParamID)
             || (VOOSMP_SRC_PID_PRESENTATION_DELAY == nParamID)
             || (VOOSMP_SRC_PID_PD_CONNECTION_RETRY_TIMES == nParamID)
             )
    {
        if (NULL == pValue) {
            return VOOSMP_ERR_Pointer;
        }
        
        map<VO_U32, int>::iterator iter = m_cParamMap.find(nParamID);
        
        if (iter != m_cParamMap.end()) {
            iter->second = *(int *)pValue;
        }
        else {
            m_cParamMap.insert(pair<VO_U32, int>(nParamID, *(int *)pValue));
        }
        
        if (!(m_pSource->IsBeInit())) {
            return VOOSMP_ERR_None;
        }
    }
    else if (VOOSMP_SRC_PID_DRM_ADAPTER_OBJECT == nParamID)
    {
        m_pDRMAdapter = pValue;
        
        if (!(m_pSource->IsBeInit())) {
            return VOOSMP_ERR_None;
        }
    }
    
    return m_pSource->SetParam(nParamID, pValue);
}

int voOSMediaPlayer::ResetStr(char **pTo, const char *pFrom)
{
    if (NULL == pFrom || NULL == pTo) {
        return VOOSMP_ERR_Pointer;
    }
    
    if (NULL != *pTo) {
        if (0 == vostrcmp(*pTo, pFrom)) {
            return VOOSMP_ERR_None;
        }
        
        delete [](*pTo);
        *pTo = NULL;
    }
    
    *pTo = new char[(strlen(pFrom) + 1)];
    
    if (NULL == *pTo) {
        return VOOSMP_ERR_Pointer;
    }
    
    memset(*pTo, 0, (strlen(pFrom) + 1));
    vostrcpy(*pTo, pFrom);
    
    return VOOSMP_ERR_None;
}

void voOSMediaPlayer::voStreamDownloadRespondCB(void * pUserData, int nCode, const char * strError)
{
    voOSMediaPlayer* pPlayer = (voOSMediaPlayer*)pUserData;
    
    if ((NULL == pPlayer) || (NULL == strError) || (NULL == pPlayer->m_pSource)) {
        return;
    }
    
    if (VO_DOWNLOAD_SUCCESS == nCode) {
        pPlayer->m_pSource->SetParam(VOOSMP_PID_SUBTITLE_FILE_NAME, (void *)strError);
    }
    
    return;
}

int voOSMediaPlayer::SetSubtitlePath(const char *pPath)
{
    if (NULL == pPath) {
        return VOOSMP_ERR_Pointer;
    }
    
    char szURL[1024*2];
    strncpy(szURL, (const char*)pPath, 1024*2);
    
    if (!strncmp (szURL, "HTTP://", 6) || !strncmp (szURL, "http://", 6)
        || !strncmp (szURL, "HTTPS://", 7) || !strncmp (szURL, "https://", 7)) {
        
        if (nil == m_pStreamDownload) {
            return VOOSMP_ERR_Pointer;
        }
        
        NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
        
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        
        NSString *pStr = [NSString stringWithFormat:@"%s", pPath];//@"http://www.cbs.com/closedcaption/CBS_BOLD_6360_ENG_CONTENT_CIAN_caption.xml";
        
        NSString *strFile = [[paths objectAtIndex:0] stringByAppendingPathComponent:@"/TmpFileTT."];
        NSArray *arraySub = [pStr componentsSeparatedByString:@"/"];
        if ([arraySub count] > 0) {
            NSString *strEXT = [arraySub objectAtIndex:([arraySub count] - 1)];
            
            NSArray *arraySub2 = [strEXT componentsSeparatedByString:@"."];
            if ([arraySub2 count] > 0) {
                strEXT = [arraySub2 objectAtIndex:([arraySub2 count] - 1)];
            }
            
            NSString *strNotLetter = [strEXT stringByTrimmingCharactersInSet:[NSCharacterSet letterCharacterSet]];
            
            if (0 < [strNotLetter length]) {
                NSArray *arraySub3 = [strEXT componentsSeparatedByCharactersInSet:[NSCharacterSet characterSetWithCharactersInString:strNotLetter]];
                
                if ([arraySub3 count] > 0) {
                    strEXT = [arraySub3 objectAtIndex:0];
                }
            }
            
            strFile = [strFile stringByAppendingString:strEXT];
        }
        else {
            strFile = [strFile stringByAppendingString:@"xml"];
        }
        
        if (NULL != m_pDownloadFile) {
             [[NSFileManager defaultManager] removeItemAtPath:(NSString *)m_pDownloadFile error:nil];
            [(NSString *)m_pDownloadFile release];
            m_pDownloadFile = NULL;
        }
        
        m_pDownloadFile = strFile;
        [(NSString *)m_pDownloadFile retain];
        
        CFStringRef pUrl = (CFStringRef)pStr;
        CFURLRef myURL = CFURLCreateWithString(kCFAllocatorDefault, pUrl, NULL);
        CFStringRef requestMethod = CFSTR("GET");
        CFHTTPMessageRef myRequest = CFHTTPMessageCreateRequest(kCFAllocatorDefault,
                                                                requestMethod, myURL, kCFHTTPVersion1_1);
        
        CFReadStreamRef myReadStream = CFReadStreamCreateForHTTPRequest(kCFAllocatorDefault, myRequest);
        
        voStreamDownloadListenerInfo cInfo;
        cInfo.pUserData = this;
        cInfo.pListener = voStreamDownloadRespondCB;
        [(voStreamDownload *)m_pStreamDownload setDelegateCB:&cInfo];
        [(voStreamDownload *)m_pStreamDownload requestDownLoad:(NSInputStream *)myReadStream strOutFilePath:strFile];
        
        CFRelease(myReadStream);
        CFRelease(myRequest);
        CFRelease(myURL);
        
        [pool release];
    }
    else {
        return m_pSource->SetParam(VOOSMP_PID_SUBTITLE_FILE_NAME, (void *)pPath);
    }
    
    return VOOSMP_ERR_None;
}

int	voOSMediaPlayer::OnListener (void * pUserData, int nID, void * pParam1, void * pParam2)
{
	voOSMediaPlayer * pPlayer = (voOSMediaPlayer *)pUserData;
    
    if (NULL == pPlayer) {
        return VOOSMP_ERR_None;
    }
    
	return pPlayer->HandleEvent (nID, pParam1, pParam2);
}

int	voOSMediaPlayer::HandleEvent (int nID, void * pParam1, void * pParam2)
{
    int nRet = VOOSMP_ERR_None;
    
    bool bNeedToLock = false;
    
    if ((VOOSMP_SRC_CB_Adaptive_Streaming_Info == nID)
        || (VOOSMP_SRC_CB_Adaptive_Streaming_Error == nID)
        || (VOOSMP_SRC_CB_Adaptive_Stream_Warning == nID)) {
        bNeedToLock = true;
        
        if (0 != pParam1) {
            VOLOGI("HandleEvent + nID:%d, Param1:%d", nID, *(int *)pParam1);
        }
        else {
            VOLOGI("HandleEvent + nID:%d", nID);
        }
    }
    else if (VOOSMP_CB_NativePlayerFailed == nID)
    {
        PostRunOnMainRequest(false, nID, NULL, NULL);

        return nRet;
    }
    else {
        VOLOGI("HandleEvent + nID:%d", nID);
    }
    
    if (bNeedToLock) {
        if (NO == m_cLock.TryLock()) {
            VOLOGI("HandleEvent - Lock don't be done:%d", nID);
            return VOOSMP_ERR_None;
        }
    }
    
    if ((VOOSMP_SRC_CB_BA_Happened == nID) && (NULL != pParam1)) {
        VOLOGI("Receive VOOSMP_SRC_CB_BA_Happened:%d", *(int *)pParam1);
    }
    else if (VOOSMP_SRC_CB_Program_Changed == nID) {
        VOLOGI("Receive VOOSMP_SRC_CB_Program_Changed");
    }
    else if (VOOSMP_SRC_CB_Program_Reset == nID) {
        VOLOGI("Receive VOOSMP_SRC_CB_Program_Reset");
    }
    
    if (nID == VOOSMP_SRC_CB_Seek_Complete) {
        
        if (m_bNativeRecoveryBlockEvent) {
            return VOOSMP_ERR_None;
        }
    }
    
    if (nID == VOOSMP_CB_SeekComplete)
	{
		m_bSeeking = false;
        
        if (m_bNativeRecoveryBlockEvent) {
            m_bNativeRecoveryBlockEvent = false;
            return VOOSMP_ERR_None;
        }
	}
    
    if (VOOSMP_SRC_CB_Program_Changed == nID
        || (VOOSMP_SRC_CB_Program_Reset == nID)) {
        m_nIsLive = -1;
    }
    
    if ([NSRunLoop mainRunLoop] != [NSRunLoop currentRunLoop]) {
        if (VOOSMP_SRC_CB_Program_Changed == nID
            || (VOOSMP_SRC_CB_Program_Reset == nID)) {
            
            PostRunOnMainRequest(false, nID, NULL, NULL);
            
            if (bNeedToLock) {
                m_cLock.Unlock();
            }
            
            VOLOGI("HandleEvent - Post to main:%d", nID);
            
            return VOOSMP_ERR_None;
        }
    }
    
    if ((NULL != m_cSendListener.pUserData) && (NULL != m_cSendListener.pListener)) {
        NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
        nRet = m_cSendListener.pListener(m_cSendListener.pUserData, nID, pParam1, pParam2);
        [pool release];
    }
    
    if (bNeedToLock) {
        m_cLock.Unlock();
    }
    
    VOLOGI("HandleEvent - end:%d, nRet:%d", nID, nRet);
    
    return nRet;
}

int	voOSMediaPlayer::OnRequestListener (void * pUserData, int nID, void * pParam1, void * pParam2)
{
	voOSMediaPlayer * pPlayer = (voOSMediaPlayer *)pUserData;
    
    if (NULL == pPlayer) {
        return VOOSMP_ERR_None;
    }
    
	return pPlayer->HandleRequestEvent (nID, pParam1, pParam2);
}

int	voOSMediaPlayer::HandleRequestEvent (int nID, void * pParam1, void * pParam2)
{
    int nRet = VOOSMP_ERR_None;
    
    if ((NULL != m_cRequest_SendListener.pUserData) && (NULL != m_cRequest_SendListener.pListener)) {
        NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
        nRet = m_cRequest_SendListener.pListener(m_cRequest_SendListener.pUserData, nID, pParam1, pParam2);
        [pool release];
    }
    
    return nRet;
}

int	voOSMediaPlayer::DoReadSubtitle(voSubtitleInfo* pSubtitle)
{
    if (NULL == m_pSource || NULL == pSubtitle)
		return VOOSMP_ERR_Pointer;
    
#ifdef _Discretix_TEMP
    if (0 >= m_pSource->GetSubtitleCount())
    {
        return VOOSMP_ERR_EOS;
    }
#endif

    return m_pSource->GetSample(VOOSMP_SS_SUBTITLE, pSubtitle);
}

int	voOSMediaPlayer::OnReadSubtitle(void* pUserData, voSubtitleInfo* pSubtitle)
{
    voOSMediaPlayer * pPlayer = (voOSMediaPlayer *)pUserData;
    
    if (NULL == pPlayer) {
        return VOOSMP_ERR_None;
    }
    
	return pPlayer->DoReadSubtitle(pSubtitle);
}


int voOSMediaPlayer::GetVideoCount()
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if (NULL == m_pSource)
		return VOOSMP_ERR_Pointer;
    
    return m_pSource->GetVideoCount();
}

int voOSMediaPlayer::GetAudioCount()
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if (NULL == m_pSource)
		return VOOSMP_ERR_Pointer;
    
    return m_pSource->GetAudioCount();
}

int voOSMediaPlayer::GetSubtitleCount()
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if (NULL == m_pSource)
		return VOOSMP_ERR_Pointer;
    
    return m_pSource->GetSubtitleCount();
}

int voOSMediaPlayer::SelectVideo(int nIndex)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if (NULL == m_pSource)
		return VOOSMP_ERR_Pointer;

    return m_pSource->SelectVideo(nIndex);
}

int voOSMediaPlayer::SelectAudio(int nIndex)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if (NULL == m_pSource)
		return VOOSMP_ERR_Pointer;
    
    m_nSelectedAudio = nIndex;
    
    return m_pSource->SelectAudio(nIndex);
}

int voOSMediaPlayer::SelectSubtitle(int nIndex)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if (NULL == m_pSource)
		return VOOSMP_ERR_Pointer;
    
    m_nSelectedSubtitle = nIndex;
    
    return m_pSource->SelectSubtitle(nIndex);
}

bool voOSMediaPlayer::IsVideoAvailable(int nIndex)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if (NULL == m_pSource)
		return VOOSMP_ERR_Pointer;
    
    return m_pSource->IsVideoAvailable(nIndex);
}

bool voOSMediaPlayer::IsAudioAvailable(int nIndex)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if (NULL == m_pSource)
		return VOOSMP_ERR_Pointer;
    
    return m_pSource->IsAudioAvailable(nIndex);
}

bool voOSMediaPlayer::IsSubtitleAvailable(int nIndex)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if (NULL == m_pSource)
		return VOOSMP_ERR_Pointer;
    
    return m_pSource->IsSubtitleAvailable(nIndex);;
}

int voOSMediaPlayer::CommitSelection()
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if (NULL == m_pSource)
		return VOOSMP_ERR_Pointer;
    
    bool bNeedSeek = false;
    
//    if (!IsLive()) {
        VOOSMP_SRC_CURR_TRACK_INDEX currIndex;
        memset(&currIndex, 0, sizeof(currIndex));
        if (VOOSMP_ERR_None == GetCurrSelectedTrackIndex(&currIndex)) {
            if (((VO_INVALID_SELECT != m_nSelectedAudio) && (m_nSelectedAudio != currIndex.nCurrAudioIdx) && (1 < GetAudioCount()))
                || ((VO_INVALID_SELECT != m_nSelectedSubtitle) && (m_nSelectedSubtitle != currIndex.nCurrSubtitleIdx) && (1 < GetSubtitleCount()))
                ) {
                bNeedSeek = true;
            }
        }
//    }
    
    int nRet = m_pSource->CommitSelection();
    
    m_nSelectedAudio = VO_INVALID_SELECT;
    m_nSelectedSubtitle = VO_INVALID_SELECT;
    
    if (bNeedSeek) {
        SetPos(GetPos());
    }
    
    return nRet;
}

int voOSMediaPlayer::ClearSelection()
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if (NULL == m_pSource)
		return VOOSMP_ERR_Pointer;
    
    m_nSelectedAudio = VO_INVALID_SELECT;
    m_nSelectedSubtitle = VO_INVALID_SELECT;
    
    return m_pSource->ClearSelection();
}


int voOSMediaPlayer::GetVideoProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if ((NULL == m_pSource) || (NULL == ppProperty)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pSource->GetVideoProperty(nIndex, ppProperty);
}

int voOSMediaPlayer::GetAudioProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if ((NULL == m_pSource) || (NULL == ppProperty)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pSource->GetAudioProperty(nIndex, ppProperty);
}

int voOSMediaPlayer::GetSubtitleProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if ((NULL == m_pSource) || (NULL == ppProperty)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pSource->GetSubtitleProperty(nIndex, ppProperty);
}

int voOSMediaPlayer::GetCurrTrackSelection(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if (NULL == m_pSource || NULL == pCurrIndex)
		return VOOSMP_ERR_Pointer;
    
    return m_pSource->GetCurrTrackSelection(pCurrIndex);
}

int voOSMediaPlayer::GetCurrPlayingTrackIndex(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if (NULL == m_pSource || NULL == pCurrIndex)
		return VOOSMP_ERR_Pointer;
    
    return m_pSource->GetCurrPlayingTrackIndex(pCurrIndex);
}

int voOSMediaPlayer::GetCurrSelectedTrackIndex(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
    if (NULL == m_pSource || NULL == pCurrIndex)
		return VOOSMP_ERR_Pointer;
    
    return m_pSource->GetCurrSelectedTrackIndex(pCurrIndex);
}

int voOSMediaPlayer::GetSelectStream(int *pStream)
{
//    if (NULL == m_pSource) {
//        return VOOSMP_ERR_Pointer;
//    }
//    
//    VOOSMP_SRC_PROGRAM_INFO* pProgramInfo = NULL;
//    GetProgramInfo(0, &pProgramInfo);
//    if ((NULL == pProgramInfo) || (NULL == (pProgramInfo->pStreamInfo))){
//        return VOOSMP_ERR_Pointer;
//    }
//    
//    int count = pProgramInfo->nStreamCount;
//    for (int streamIndex = 0; streamIndex < count; streamIndex++)
//    {
//        int nSel = 0;
//        if (pProgramInfo->pStreamInfo[streamIndex] != nil)
//        {
//            nSel = (*(pProgramInfo->pStreamInfo[streamIndex])).nSelInfo;
//        }
//        
//        if (0 != (nSel & VOOSMP_SRC_TRACK_SELECT_SELECTED)) {
//            *pStream = streamIndex;
//            return VOOSMP_ERR_None;
//        }
//    }
    
    return VOOSMP_ERR_Unknown;
}

void voOSMediaPlayer::RunningRequestOnMain(int nID, void *pParam1, void *pParam2)
{
    if (VOOSMP_CB_NativePlayerFailed == nID) {
        
        if (!m_bSeeking) {
            m_bNativeRecoveryBlockEvent = true;
        }
        
        SetPosInner(GetPos());
        
        return;
    }
    
    if ((NULL != m_cSendListener.pUserData) && (NULL != m_cSendListener.pListener)) {
        NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
        m_cSendListener.pListener(m_cSendListener.pUserData, nID, pParam1, pParam2);
        [pool release];
    }
}

bool voOSMediaPlayer::IsLive()
{
#ifdef _VOLOG_INFO
    voCheckUseTime cOverTime(VO_OVER_TIME, __FUNCTION__);
#endif
    
#ifdef _Discretix_TEMP
    long long duration = 0;
    int retDuration = GetDuration(&duration);
    
    if ((VOOSMP_ERR_None == retDuration) && (0 < duration)) {
        return false;
    }
    
    return true;
#endif
    
    if (NULL == m_pSource) {
        return false;
    }
    
    if (-1 == m_nIsLive) {
        VOOSMP_SRC_PROGRAM_INFO* pProgramInfo = NULL;
        int nRet = GetProgramInfo(0, &pProgramInfo);
        if ((VOOSMP_ERR_None != nRet) || (NULL == pProgramInfo)){
            return false;
        }
        
        if (VOOSMP_SRC_PROGRAM_TYPE_LIVE == pProgramInfo->nProgramType) {
            m_nIsLive = 1;
        }
        else if (VOOSMP_SRC_PROGRAM_TYPE_VOD == pProgramInfo->nProgramType) {
            m_nIsLive = 0;
        }
        else {
            m_nIsLive = -1;
            
            return false;
        }
    }
    
    return m_nIsLive;
}
