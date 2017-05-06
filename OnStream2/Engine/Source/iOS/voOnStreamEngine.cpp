	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voOnStreamSDK.cpp

	Contains:	voOnStreamSDK class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-18		JBF			Create file

*******************************************************************************/

#include "voOnStreamEngine.h"
#include "voOSPlayerAdapter.h"
#include "stdio.h"
#include "voLog.h"

VO_LOG_PRINT_CB		g_OnStreamEngineVOLOGCB;

int voOSEngUninit(void* pHandle)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
    int nRC = ((voOSPlayerAdapter *)pHandle)->Uninit();
	delete (voOSPlayerAdapter *)pHandle;
    
    VOLOGUNINIT();
    
	return nRC;
}

int voOSEngInit(void** phHandle, int nPlayerType, void* pInitParam, int nInitParamFlag)
{
	voOSPlayerAdapter* pPlayer = NULL;
    
    pPlayer = new voOSPlayerAdapter();
    
	if (pPlayer == NULL)
	{
		return VOOSMP_ERR_OutMemory;
	}
    
    char *Path = NULL;
    if ((0 != (VOOSMP_FLAG_INIT_WORKING_PATH & nInitParamFlag))
         && (NULL != pInitParam)) {
        VOOSMP_INIT_PARAM* pInit = (VOOSMP_INIT_PARAM *)pInitParam;
        
        if (NULL != pInit->pWorkingPath) {
            Path = (char *)(pInit->pWorkingPath);
            pPlayer->SetParam(VOOSMP_PID_PLAYER_PATH, pInit->pWorkingPath);
        }
    }

    VOLOGINIT(Path);
    
    pPlayer->SetParam (VOOSMP_PID_COMMON_LOGFUNC, &g_OnStreamEngineVOLOGCB);
    
    int nRet = pPlayer->Init(nPlayerType);
    
    if (VOOSMP_ERR_None != nRet) {
        voOSEngUninit(pPlayer);
        return nRet;
    }
    
	*phHandle = pPlayer;

	return VOOSMP_ERR_None;
}

int voOSEngSetView(void* pHandle, void* pView)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSPlayerAdapter *)pHandle)->SetView(pView);
}

int voOSEngOpen(void* pHandle, void* pSource, int nFlag)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSPlayerAdapter *)pHandle)->SetDataSource(pSource, nFlag);
}

int voOSEngClose(void* pHandle)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSPlayerAdapter *)pHandle)->Close();
}

int voOSEngRun(void* pHandle)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSPlayerAdapter *)pHandle)->Run();
}

int voOSEngPause(void* pHandle)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSPlayerAdapter *)pHandle)->Pause();
}

int voOSEngStop(void* pHandle)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSPlayerAdapter *)pHandle)->Stop();
}

int voOSEngGetPos(void* pHandle)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
    int nPos = 0;
	int nRet = ((voOSPlayerAdapter *)pHandle)->GetPos(&nPos);
    
    return nRet;
}

int voOSEngSetPos(void* pHandle, int nPos)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSPlayerAdapter *)pHandle)->SetPos(nPos);
}

int voOSEngGetSubtitleSample(void* pHandle, voSubtitleInfo* pSample)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSPlayerAdapter *)pHandle)->GetSubtileSample(pSample);
}

int voOSEngSelectLanguage(void* pHandle, int nIndex)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSPlayerAdapter *)pHandle)->SelectLanguage(nIndex);
}

int voOSEngGetLanguage(void* pHandle, VOOSMP_SUBTITLE_LANGUAGE_INFO** ppLangInfo)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSPlayerAdapter *)pHandle)->GetSubLangInfo(ppLangInfo);
}

int voOSEngGetParam(void* pHandle, int nParamID, void* pValue)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSPlayerAdapter *)pHandle)->GetParam(nParamID, pValue);
}

int voOSGetSEI(void* pHandle, VOOSMP_SEI_INFO* pValue)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSPlayerAdapter *)pHandle)->GetSEISample(pValue);
}

int voOSEngSetParam(void* pHandle, int nParamID, void* pValue)
{
    if (nParamID == VOOSMP_PID_COMMON_LOGFUNC && pValue)
	{
		memcpy (&g_OnStreamEngineVOLOGCB, pValue, sizeof (g_OnStreamEngineVOLOGCB));
		
		VO_LOG_PRINT_CB * pVologCB = (VO_LOG_PRINT_CB *)pValue;
	}
    
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSPlayerAdapter *)pHandle)->SetParam(nParamID, pValue);
}

int voGetOnStreamEngnAPI(voOnStreamEngnAPI * pFuncSet)
{
	if (pFuncSet == NULL)
		return VOOSMP_ERR_Pointer;
    
    pFuncSet->Init = voOSEngInit;
    pFuncSet->Uninit = voOSEngUninit;
    pFuncSet->SetView = voOSEngSetView;
    pFuncSet->Open = voOSEngOpen;
    pFuncSet->Close = voOSEngClose;
    pFuncSet->Run = voOSEngRun;
    pFuncSet->Pause = voOSEngPause;
    pFuncSet->Stop = voOSEngStop;
    pFuncSet->GetPos = voOSEngGetPos;
    pFuncSet->SetPos = voOSEngSetPos;
    pFuncSet->GetSubtitleSample = voOSEngGetSubtitleSample;
    pFuncSet->SelectLanguage = voOSEngSelectLanguage;
    pFuncSet->GetLanguage = voOSEngGetLanguage;
    pFuncSet->GetSEI = voOSGetSEI;
    pFuncSet->GetParam = voOSEngGetParam;
    pFuncSet->SetParam = voOSEngSetParam;

	return VOOSMP_ERR_None;
}
