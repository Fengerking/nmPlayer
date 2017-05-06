/*******************************************************************************
 *	File:		voOSEngWrapper.cpp
 *	
 *	Contains:	voOSEngWrapper class file
 *	
 *	Written by:	Jeff huang
 *	
 *	Change History (most recent first):
 *	2012-03-20		Jeff			Create file
 *	
 ******************************************************************************/


#include <string.h>

#include "voOSEngWrapper.h"
#include "voOnStreamType.h"
#include "voLog.h"

voOSEngWrapper::voOSEngWrapper()
:CDllLoad()
,m_pOSHandle(NULL)
,m_pLogCB(NULL)
{
	memset(&m_cOSFuncSet, 0, sizeof(voOnStreamEngnAPI));
}

voOSEngWrapper::~voOSEngWrapper(void)
{
	Close();
    Uninit();
}

int voOSEngWrapper::Init(int nPlayerType, void* pInitParam, int nInitParamFlag)
{
    memset(&m_cOSFuncSet, 0, sizeof(voOnStreamEngnAPI));
    
#if defined (_IOS)
	voGetOnStreamEngnAPI(&m_cOSFuncSet);
#elif defined (_MAC_OS)
    vostrcpy (m_szDllFile, _T("voOSEng"));
    vostrcpy (m_szAPIName, _T("voGetOnStreamEngnAPI"));
    
    if (LoadLib (NULL) == 0)
	{
		VOLOGE ("voOSEngWrapper::LoadLib File %s, API %s Failed.", m_szDllFile, m_szAPIName);
		return VOOSMP_ERR_Unknown;
	}
    
    if (NULL == m_pAPIEntry) {
        return VOOSMP_ERR_Unknown;
    }
    
    ((VOGETONSTREAMENGNAPI)m_pAPIEntry)(&m_cOSFuncSet);
#endif
    
	if (NULL == m_cOSFuncSet.Init) {
        return VOOSMP_ERR_Pointer;
    }
    
    if (m_pLogCB && m_cOSFuncSet.SetParam) {
        m_cOSFuncSet.SetParam(NULL, VOOSMP_PID_COMMON_LOGFUNC, m_pLogCB);
    }
    
    return m_cOSFuncSet.Init(&m_pOSHandle, nPlayerType, pInitParam, nInitParamFlag);
}

int voOSEngWrapper::Uninit()
{
	if (NULL == m_cOSFuncSet.Uninit) {
        return VOOSMP_ERR_Pointer;
    }
    
    if (NULL == m_pOSHandle) {
        return VOOSMP_ERR_None;
    }
    
    int nRet = m_cOSFuncSet.Uninit(m_pOSHandle);
    
    if (VOOSMP_ERR_None == nRet) {
        memset(&m_cOSFuncSet, 0, sizeof(voOnStreamEngnAPI));
        m_pOSHandle = NULL;
    }
    
    return nRet;
}

int voOSEngWrapper::SetView(void* pView)
{
	if ((NULL == m_cOSFuncSet.SetView) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSFuncSet.SetView(m_pOSHandle, pView);
}

int voOSEngWrapper::Open(void * pSource, int nFlag)
{
	if ((NULL == m_cOSFuncSet.Open) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSFuncSet.Open(m_pOSHandle, pSource, nFlag);
}

int voOSEngWrapper::SelectLanguage (int nIndex)
{
    if ((NULL == m_cOSFuncSet.SelectLanguage) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSFuncSet.SelectLanguage(m_pOSHandle, nIndex);
}

int voOSEngWrapper::GetLanguage (VOOSMP_SUBTITLE_LANGUAGE_INFO** ppLangInfo)
{
    if ((NULL == m_cOSFuncSet.GetLanguage) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSFuncSet.GetLanguage(m_pOSHandle, ppLangInfo);
}

int voOSEngWrapper::Close()
{
	if ((NULL == m_cOSFuncSet.Close) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSFuncSet.Close(m_pOSHandle);
}

int voOSEngWrapper::Run()
{
	if ((NULL == m_cOSFuncSet.Run) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSFuncSet.Run(m_pOSHandle);
}

int voOSEngWrapper::Pause()
{
	if ((NULL == m_cOSFuncSet.Pause) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSFuncSet.Pause(m_pOSHandle);
}

int voOSEngWrapper::Stop()
{
	if ((NULL == m_cOSFuncSet.Stop) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSFuncSet.Stop(m_pOSHandle);
}

int voOSEngWrapper::GetPos()
{
	if ((NULL == m_cOSFuncSet.GetPos) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSFuncSet.GetPos(m_pOSHandle);
}

int voOSEngWrapper::SetPos(int nCurPos)
{
	if ((NULL == m_cOSFuncSet.SetPos) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSFuncSet.SetPos(m_pOSHandle, nCurPos);
}

int voOSEngWrapper::GetSubtitleSample(voSubtitleInfo* pSample)
{
    if ((NULL == m_cOSFuncSet.GetSubtitleSample) || (NULL == m_pOSHandle) || (NULL == pSample)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSFuncSet.GetSubtitleSample(m_pOSHandle, pSample);
}

int voOSEngWrapper::GetParam(int nParamID, void* pParam)
{
	if ((NULL == m_cOSFuncSet.GetParam) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSFuncSet.GetParam(m_pOSHandle, nParamID, pParam);
}

int voOSEngWrapper::SetParam(int nParamID, void* pParam)
{
    if (NULL == m_cOSFuncSet.SetParam) {
        if (nParamID == VOOSMP_PID_COMMON_LOGFUNC)
        {
            m_pLogCB = pParam;
            return VOOSMP_ERR_None;
        }
        
        return VOOSMP_ERR_Pointer;
    }
    
    if (VOOSMP_PID_COMMON_LOGFUNC == nParamID) {
        return m_cOSFuncSet.SetParam(m_pOSHandle, nParamID, pParam);
    }
    else if (NULL == m_pOSHandle) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSFuncSet.SetParam(m_pOSHandle, nParamID, pParam);
}

int	voOSEngWrapper::GetSEISample (VOOSMP_SEI_INFO * pSample)
{
    if ((NULL == m_cOSFuncSet.GetSEI) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_cOSFuncSet.GetSEI(m_pOSHandle, pSample);
}
