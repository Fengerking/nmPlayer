/************************************************************************
 *																		*
 *		VisualOn, Inc. Confidential and Proprietary, 2003				*
 *																		*
 ************************************************************************/
/*******************************************************************************
 File:		voOnStreamMP.cpp
 
 Contains:	voOnStreamMP class file
 
 Written by:	Jeff Huang
 
 Change History (most recent first):
 2012-05-08		Jeff Huang			Create file
 
 *******************************************************************************/

#include "voOnStreamMP.h"
#include "voOSMediaPlayer.h"
#include "stdio.h"

int voOSMPInit(void** phHandle, int nPlayerType, void* pInitParam, int nInitParamFlag)
{
	voOSMediaPlayer* pPlayer = new voOSMediaPlayer();
    
	if (pPlayer == NULL)
	{
		return VOOSMP_ERR_OutMemory;
	}
    
    pPlayer->Init(nPlayerType, pInitParam, nInitParamFlag);
	*phHandle = pPlayer;
    
	return VOOSMP_ERR_None;
}

int voOSMPUninit(void* pHandle)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
    int nRC = ((voOSMediaPlayer *)pHandle)->UnInit();
	delete (voOSMediaPlayer *)pHandle;
    
	return nRC;
}

int voOSMPSetView(void* pHandle, void* pView)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSMediaPlayer *)pHandle)->SetView(pView);
}

int voOSMPOpen(void* pHandle, void* pSource, int nFlag, int nSourceType)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSMediaPlayer *)pHandle)->Open(pSource, nFlag, nSourceType, 0, 0);
}

int voOSMPGetProgramCount(void* pHandle, int* pProgramCount)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSMediaPlayer *)pHandle)->GetProgramCount(pProgramCount);
}

int voOSMPGetProgramInfo(void* pHandle, int nProgramIndex, VOOSMP_SRC_PROGRAM_INFO** ppProgramInfo)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSMediaPlayer *)pHandle)->GetProgramInfo(nProgramIndex, ppProgramInfo);
}

int voOSMPGetCurTrackInfo(void* pHandle, int nTrackType, VOOSMP_SRC_TRACK_INFO** ppTrackInfo )
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSMediaPlayer *)pHandle)->GetCurTrackInfo(nTrackType, ppTrackInfo);
}

int voOSMPGetSample(void* pHandle, int nTrackType, void* pSample)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSMediaPlayer *)pHandle)->GetSample(nTrackType, pSample);
}

int voOSMPSelectProgram(void* pHandle, int nProgram)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSMediaPlayer *)pHandle)->SelectProgram(nProgram);
}

int voOSMPSelectStream(void* pHandle, int nStream)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSMediaPlayer *)pHandle)->SelectStream(nStream);
}

int voOSMPSelectTrack(void* pHandle, int nTrack)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSMediaPlayer *)pHandle)->SelectTrack(nTrack);
}

int voOSMPSelectLanguage(void* pHandle, int nIndex)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSMediaPlayer *)pHandle)->SelectLanguage(nIndex);
}

int voOSMPGetLanguage(void* pHandle, VOOSMP_SUBTITLE_LANGUAGE_INFO** ppLangInfo)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSMediaPlayer *)pHandle)->GetLanguage(ppLangInfo);
}

int voOSMPClose(void* pHandle)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSMediaPlayer *)pHandle)->Close();
}

int voOSMPRun(void* pHandle)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSMediaPlayer *)pHandle)->Run();
}

int voOSMPPause(void* pHandle)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSMediaPlayer *)pHandle)->Pause();
}

int voOSMPStop(void* pHandle)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSMediaPlayer *)pHandle)->Stop();
}

int voOSMPGetPos(void* pHandle)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSMediaPlayer *)pHandle)->GetPos();
}

int voOSMPSetPos(void* pHandle, int nPos)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSMediaPlayer *)pHandle)->SetPos(nPos);
}

int voOSMPGetDuration(void* pHandle, long long* pDuration)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSMediaPlayer *)pHandle)->GetDuration(pDuration);
}

int voOSMPGetParam(void* pHandle, int nParamID, void* pValue)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSMediaPlayer *)pHandle)->GetParam(nParamID, pValue);
}

int voOSMPSetParam(void* pHandle, int nParamID, void* pValue)
{
    if (pHandle == NULL)
		return VOOSMP_ERR_Pointer;
    
	return ((voOSMediaPlayer *)pHandle)->SetParam(nParamID, pValue);
}

int voGetOnStreamMediaPlayerAPI(voOnStreamMediaPlayerAPI * pFuncSet)
{
	if (pFuncSet == NULL)
		return VOOSMP_ERR_Pointer;
    
    pFuncSet->Init = voOSMPInit;
    pFuncSet->Uninit = voOSMPUninit;
    pFuncSet->SetView = voOSMPSetView;
    pFuncSet->Open = voOSMPOpen;
    pFuncSet->GetProgramCount = voOSMPGetProgramCount;
    pFuncSet->GetProgramInfo = voOSMPGetProgramInfo;
    pFuncSet->GetCurTrackInfo = voOSMPGetCurTrackInfo;
    pFuncSet->GetSample = voOSMPGetSample;
    pFuncSet->SelectProgram = voOSMPSelectProgram;
    pFuncSet->SelectStream = voOSMPSelectStream;
    pFuncSet->SelectTrack = voOSMPSelectTrack;
    pFuncSet->SelectLanguage = voOSMPSelectLanguage;
    pFuncSet->GetLanguage = voOSMPGetLanguage;
    pFuncSet->Close = voOSMPClose;
    pFuncSet->Run = voOSMPRun;
    pFuncSet->Pause = voOSMPPause;
    pFuncSet->Stop = voOSMPStop;
    pFuncSet->GetPos = voOSMPGetPos;
    pFuncSet->SetPos = voOSMPSetPos;
    pFuncSet->GetDuration = voOSMPGetDuration;
    pFuncSet->GetParam = voOSMPGetParam;
    pFuncSet->SetParam = voOSMPSetParam;
    
	return VOOSMP_ERR_None;
}