/*******************************************************************************
 *	File:		voOSSourceWrapper.cpp
 *	
 *	Contains:	voOSSourceWrapper class file
 *	
 *	Written by:	Jeff huang
 *	
 *	Change History (most recent first):
 *	2012-03-20		Jeff			Create file
 *	
 ******************************************************************************/

#include "voOSSourceWrapper.h"
#include "voOnStreamType.h"
#include "voLog.h"

#include <string.h>

voOSSourceWrapper::voOSSourceWrapper()
:CDllLoad()
,m_pOSHandle(NULL)
,m_pLogCB(NULL)
,m_bInit(false)
{
    memset(&m_cOSSourceFuncSet, 0, sizeof(voOSMPSourceAPI));
}

voOSSourceWrapper::~voOSSourceWrapper(void)
{
    Destroy();
}

int voOSSourceWrapper::Create()
{
    memset(&m_cOSSourceFuncSet, 0, sizeof(voOSMPSourceAPI));
    
#if defined (_IOS)
	voGetOnStreamSourceAPI(&m_cOSSourceFuncSet);
#elif defined (_MAC_OS)
    vostrcpy (m_szDllFile, _T("voOSSource"));
    vostrcpy (m_szAPIName, _T("voGetOnStreamSourceAPI"));
    
    if (LoadLib (NULL) == 0)
	{
		VOLOGE ("voOSEngWrapper::LoadLib File %s, API %s Failed.", m_szDllFile, m_szAPIName);
		return VOOSMP_ERR_Unknown;
	}
    
    if (NULL == m_pAPIEntry) {
        return VOOSMP_ERR_Unknown;
    }
    
    ((VOGETONSTREAMSOURCEAPI)m_pAPIEntry)(&m_cOSSourceFuncSet);
#endif
    
    if (NULL == m_cOSSourceFuncSet.Create) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.Create(&m_pOSHandle);
}

int voOSSourceWrapper::Destroy()
{
    if (NULL == m_cOSSourceFuncSet.Destroy) {
        return VOOSMP_ERR_Pointer;
    }
    
    if (NULL == m_pOSHandle) {
        return VOOSMP_ERR_None;
    }
    
    int nRet = m_cOSSourceFuncSet.Destroy(m_pOSHandle);
    
    if (VOOSMP_ERR_None == nRet) {
        memset(&m_cOSSourceFuncSet, 0, sizeof(voOSMPSourceAPI));
        m_pOSHandle = NULL;
    }
    
    return nRet;
}

int voOSSourceWrapper::Init(void* pSource, int nSourceFlag, int nSourceType, void* pInitParam, int nInitParamFlag)
{
	if ((NULL == m_cOSSourceFuncSet.Init) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    if (m_pLogCB && m_cOSSourceFuncSet.SetParam) {
        m_cOSSourceFuncSet.SetParam(NULL, VOOSMP_PID_COMMON_LOGFUNC, m_pLogCB);
    }
    
    int nRet = m_cOSSourceFuncSet.Init(m_pOSHandle, pSource, nSourceFlag, nSourceType, pInitParam, nInitParamFlag);
    
    if (VOOSMP_ERR_None == nRet) {
        m_bInit = true;
    }
    
    return nRet;
}

int voOSSourceWrapper::Uninit()
{
	if (NULL == m_cOSSourceFuncSet.Uninit) {
        return VOOSMP_ERR_Pointer;
    }
    
    int nRet = m_cOSSourceFuncSet.Uninit(m_pOSHandle);
    
    if (VOOSMP_ERR_None == nRet) {
        m_bInit = false;
    }
    
    return nRet;
}

int voOSSourceWrapper::Open()
{
	if ((NULL == m_cOSSourceFuncSet.Open) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.Open(m_pOSHandle);
}

int voOSSourceWrapper::Close()
{
	if ((NULL == m_cOSSourceFuncSet.Close) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.Close(m_pOSHandle);
}

int voOSSourceWrapper::Run()
{
	if ((NULL == m_cOSSourceFuncSet.Run) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.Run(m_pOSHandle);
}

int voOSSourceWrapper::Pause()
{
	if ((NULL == m_cOSSourceFuncSet.Pause) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.Pause(m_pOSHandle);
}

int voOSSourceWrapper::Stop()
{
	if ((NULL == m_cOSSourceFuncSet.Stop) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.Stop(m_pOSHandle);
}

int voOSSourceWrapper::SetPos(long long* pTimeStamp)
{
	if ((NULL == m_cOSSourceFuncSet.SetPos) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.SetPos(m_pOSHandle, pTimeStamp);
}

int voOSSourceWrapper::GetDuration (long long* pDuration)
{
	if ((NULL == m_cOSSourceFuncSet.GetDuration) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.GetDuration(m_pOSHandle, pDuration);
}

int voOSSourceWrapper::GetSample (int nOutPutType, void* pSample)
{
    if ((NULL == m_cOSSourceFuncSet.GetSample) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.GetSample(m_pOSHandle, nOutPutType, pSample);
}

int voOSSourceWrapper::GetProgramCount (int* pStreamCount)
{
    if ((NULL == m_cOSSourceFuncSet.GetProgramCount) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.GetProgramCount(m_pOSHandle, pStreamCount);
}

int voOSSourceWrapper::GetProgramInfo (int nStream, VOOSMP_SRC_PROGRAM_INFO** ppStreamInfo)
{
    if ((NULL == m_cOSSourceFuncSet.GetProgramInfo) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.GetProgramInfo(m_pOSHandle, nStream, ppStreamInfo);
}

int voOSSourceWrapper::GetCurTrackInfo (int nOutPutType, VOOSMP_SRC_TRACK_INFO** ppTrackInfo)
{
    if ((NULL == m_cOSSourceFuncSet.GetCurTrackInfo) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.GetCurTrackInfo(m_pOSHandle, nOutPutType, ppTrackInfo);
}

int voOSSourceWrapper::SelectProgram (int nProgram)
{
    if ((NULL == m_cOSSourceFuncSet.SelectProgram) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.SelectProgram(m_pOSHandle, nProgram);
}

int voOSSourceWrapper::SelectStream (int nStream)
{
    if ((NULL == m_cOSSourceFuncSet.SelectStream) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.SelectStream(m_pOSHandle, nStream);
}

int voOSSourceWrapper::SelectTrack (int nTrack)
{
    if ((NULL == m_cOSSourceFuncSet.SelectTrack) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.SelectTrack(m_pOSHandle, nTrack);
}

int voOSSourceWrapper::SelectLanguage (int nIndex)
{
    if ((NULL == m_cOSSourceFuncSet.SelectLanguage) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.SelectLanguage(m_pOSHandle, nIndex);
}

int voOSSourceWrapper::GetLanguage (VOOSMP_SUBTITLE_LANGUAGE_INFO** ppLangInfo)
{
    if ((NULL == m_cOSSourceFuncSet.GetLanguage) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.GetLanguage(m_pOSHandle, ppLangInfo);
}

int voOSSourceWrapper::SendBuffer (const VOOSMP_BUFFERTYPE& buffer)
{
    if ((NULL == m_cOSSourceFuncSet.SendBuffer) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.SendBuffer(m_pOSHandle, buffer);
}

int voOSSourceWrapper::GetParam(int nParamID, void* pParam)
{
	if ((NULL == m_cOSSourceFuncSet.GetParam) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.GetParam(m_pOSHandle, nParamID, pParam);
}

int voOSSourceWrapper::SetParam(int nParamID, void* pParam)
{
    if (NULL == m_cOSSourceFuncSet.SetParam) {
        if (nParamID == VOOSMP_PID_COMMON_LOGFUNC)
        {
            m_pLogCB = pParam;
            return VOOSMP_ERR_None;
        }
        
        return VOOSMP_ERR_Pointer;
    }
    
    if (VOOSMP_PID_COMMON_LOGFUNC == nParamID) {
        return m_cOSSourceFuncSet.SetParam(m_pOSHandle, nParamID, pParam);
    }
	else if (NULL == m_pOSHandle) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.SetParam(m_pOSHandle, nParamID, pParam);
}

int voOSSourceWrapper::GetVideoCount()
{
	if ((NULL == m_cOSSourceFuncSet.GetVideoCount) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.GetVideoCount(m_pOSHandle);
}

int voOSSourceWrapper::GetAudioCount()
{
	if ((NULL == m_cOSSourceFuncSet.GetAudioCount) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.GetAudioCount(m_pOSHandle);
}

int voOSSourceWrapper::GetSubtitleCount()
{
	if ((NULL == m_cOSSourceFuncSet.GetSubtitleCount) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.GetSubtitleCount(m_pOSHandle);
}

int voOSSourceWrapper::SelectVideo(int nIndex)
{
	if ((NULL == m_cOSSourceFuncSet.SelectVideo) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.SelectVideo(m_pOSHandle, nIndex);
}

int voOSSourceWrapper::SelectAudio(int nIndex)
{
	if ((NULL == m_cOSSourceFuncSet.SelectAudio) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.SelectAudio(m_pOSHandle, nIndex);
}

int voOSSourceWrapper::SelectSubtitle(int nIndex)
{
	if ((NULL == m_cOSSourceFuncSet.SelectSubtitle) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.SelectSubtitle(m_pOSHandle, nIndex);
}

bool voOSSourceWrapper::IsVideoAvailable(int nIndex)
{
	if ((NULL == m_cOSSourceFuncSet.IsVideoAvailable) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.IsVideoAvailable(m_pOSHandle, nIndex);
}

bool voOSSourceWrapper::IsAudioAvailable(int nIndex)
{
	if ((NULL == m_cOSSourceFuncSet.IsAudioAvailable) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.IsAudioAvailable(m_pOSHandle, nIndex);
}

bool voOSSourceWrapper::IsSubtitleAvailable(int nIndex)
{
	if ((NULL == m_cOSSourceFuncSet.IsSubtitleAvailable) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.IsSubtitleAvailable(m_pOSHandle, nIndex);;
}

int voOSSourceWrapper::CommitSelection()
{
	if ((NULL == m_cOSSourceFuncSet.CommitSelection) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.CommitSelection(m_pOSHandle);
}

int voOSSourceWrapper::ClearSelection()
{
	if ((NULL == m_cOSSourceFuncSet.ClearSelection) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.ClearSelection(m_pOSHandle);
}


int voOSSourceWrapper::GetVideoProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
	if ((NULL == m_cOSSourceFuncSet.GetVideoProperty) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.GetVideoProperty(m_pOSHandle, nIndex, ppProperty);
}

int voOSSourceWrapper::GetAudioProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
	if ((NULL == m_cOSSourceFuncSet.GetAudioProperty) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.GetAudioProperty(m_pOSHandle, nIndex, ppProperty);
}

int voOSSourceWrapper::GetSubtitleProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
	if ((NULL == m_cOSSourceFuncSet.GetSubtitleProperty) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.GetSubtitleProperty(m_pOSHandle, nIndex, ppProperty);
}

int voOSSourceWrapper::GetCurrTrackSelection(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
	if ((NULL == m_cOSSourceFuncSet.GetCurrTrackSelection) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.GetCurrTrackSelection(m_pOSHandle, pCurrIndex);
}

int voOSSourceWrapper::GetCurrPlayingTrackIndex(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
	if ((NULL == m_cOSSourceFuncSet.GetCurrPlayingTrackIndex) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.GetCurrPlayingTrackIndex(m_pOSHandle, pCurrIndex);
}

int voOSSourceWrapper::GetCurrSelectedTrackIndex(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
	if ((NULL == m_cOSSourceFuncSet.GetCurrSelectedTrackIndex) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSSourceFuncSet.GetCurrSelectedTrackIndex(m_pOSHandle, pCurrIndex);
}

bool voOSSourceWrapper::IsBeInit()
{
    return true;
}
