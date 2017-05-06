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
 * @file voOnStreamSDK.cpp
 * wrap class for SDK engine.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
#include "ODMAVomePlayer.h"
#include "voOnStreamEngine.h"
#include "voSubtitleType.h"


int voOSInit(void** phHandle, int nPlayerType, void* pInitParam, int nInitParamFlag)
{
	IOSBasePlayer * pPlayer = 0;

	if(nPlayerType == VOOSMP_VOME2_PLAYER)
		pPlayer = new ODMAVomePlayer();
	else
		return VOOSMP_ERR_ParamID;

	if(pPlayer == 0)
		return VOOSMP_ERR_OutMemory;

	if(nInitParamFlag == VOOSMP_FLAG_INIT_LIBOP)
	{
		pPlayer->SetParam(VOOSMP_PID_FUNC_LIB, pInitParam);
	}

	int nRC = pPlayer->Init();
	if(nRC) 
	{
		delete pPlayer;
		return nRC;
	}

	*phHandle = pPlayer;

	return VOOSMP_ERR_None;
}

int voOSUninit(void* pHandle)
{
	IOSBasePlayer *pEngine = (IOSBasePlayer *)pHandle;
	if(pEngine == 0)
		return VOMP_ERR_Pointer;

	int nRC = pEngine->Uninit();
	delete pEngine;

	pHandle = 0;

	return nRC;
}

int voOSSetView(void* pHandle, void* pView)
{
	IOSBasePlayer *pEngine = (IOSBasePlayer *)pHandle;
	if(pEngine == 0)
		return VOMP_ERR_Pointer;

	return pEngine->SetView(pView);
}

int voOSOpen(void* pHandle, void* pSource, int nFlag)
{
	IOSBasePlayer *pEngine = (IOSBasePlayer *)pHandle;
	if(pEngine == 0)
		return VOMP_ERR_Pointer;

	return pEngine->SetDataSource(pSource, nFlag);
}

int voOSClose(void* pHandle)
{
	IOSBasePlayer *pEngine = (IOSBasePlayer *)pHandle;
	if(pEngine == 0)
		return VOMP_ERR_Pointer;

	return pEngine->Stop();
}

int voOSRun(void* pHandle)
{
	IOSBasePlayer *pEngine = (IOSBasePlayer *)pHandle;
	if(pEngine == 0)
		return VOMP_ERR_Pointer;

	return pEngine->Run();
}

int voOSPause(void* pHandle)
{
	IOSBasePlayer *pEngine = (IOSBasePlayer *)pHandle;
	if(pEngine == 0)
		return VOMP_ERR_Pointer;

	return pEngine->Pause();
}

int voOSStop (void* pHandle)
{
	IOSBasePlayer *pEngine = (IOSBasePlayer *)pHandle;
	if(pEngine == 0)
		return VOMP_ERR_Pointer;

	return pEngine->Stop();
}


int voOSGetPos(void* pHandle)
{
	IOSBasePlayer *pEngine = (IOSBasePlayer *)pHandle;
	if(pEngine == 0)
		return VOMP_ERR_Pointer;

	int nCurPos = 0;

	int nRC = pEngine->GetPos(&nCurPos);

	if(nRC) return 0;

	return nCurPos;
}

int voOSSetPos(void* pHandle, int nPos)
{
	IOSBasePlayer *pEngine = (IOSBasePlayer *)pHandle;
	if(pEngine == 0)
		return VOMP_ERR_Pointer;

	return pEngine->SetPos(nPos);
}


int voOSGetParam(void* pHandle, int nParamID, void* pValue)
{
	IOSBasePlayer *pEngine = (IOSBasePlayer *)pHandle;
	if(pEngine == 0)
		return VOMP_ERR_Pointer;

	return pEngine->GetParam(nParamID, pValue);
}


int voOSSetParam(void* pHandle, int nParamID, void* pValue)
{
	IOSBasePlayer *pEngine = (IOSBasePlayer *)pHandle;
	if(pEngine == 0)
		return VOMP_ERR_Pointer;

	return pEngine->SetParam(nParamID, pValue);
}


int GetSubtileSample(void* pHandle, voSubtitleInfo * pSample)
{
	IOSBasePlayer *pEngine = (IOSBasePlayer *)pHandle;
	if(pEngine == 0)
		return VOMP_ERR_Pointer;

	return pEngine->GetSubtileSample(pSample);
}


int voGetOnStreamEngnAPI(voOnStreamEngnAPI* pFuncSet)
{
	if(pFuncSet == 0)
		return VOOSMP_ERR_Pointer;

	pFuncSet->Init = voOSInit;
	pFuncSet->Uninit = voOSUninit;
	pFuncSet->SetView = voOSSetView;
	pFuncSet->Open = voOSOpen;
	pFuncSet->Close = voOSClose;
	pFuncSet->Run = voOSRun;
	pFuncSet->Pause = voOSPause;
	pFuncSet->Stop = voOSStop;
	pFuncSet->GetPos = voOSGetPos;
	pFuncSet->SetPos = voOSSetPos;
	pFuncSet->GetParam = voOSGetParam;
	pFuncSet->SetParam = voOSSetParam;
	pFuncSet->GetSubtitleSample = GetSubtileSample;

	return VOOSMP_ERR_None;
}
