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

#include "windows.h"
#include "voOnStreamMP.h"
#include "COnStreamMPImpl.h"

int voOnStreamMPInit(void** phHandle, int nPlayerType, void* pInitParam, int nInitParamFlag)
{
	if(nPlayerType != VOOSMP_VOME2_PLAYER)
		return VOOSMP_ERR_ParamID;

	COnStreamMPImpl *pImpl = new COnStreamMPImpl(nPlayerType , pInitParam , nInitParamFlag);
	if(pImpl != NULL)
	{
		*phHandle = pImpl;
		return VOOSMP_ERR_None;
	}

	return VOOSMP_ERR_OutMemory;
}

int voOnStreamMPUninit(void* pHandle)
{
	if(pHandle == NULL)
		return VOOSMP_ERR_Pointer;

	COnStreamMPImpl *pImpl = (COnStreamMPImpl *)pHandle;

	delete pImpl;
	pImpl = NULL;

	return VOOSMP_ERR_None;
}

int voOnStreamMPSetView(void* pHandle, void* pView)
{
	if(pHandle == NULL)
		return VOOSMP_ERR_Pointer;

	COnStreamMPImpl *pImpl = (COnStreamMPImpl *)pHandle;

	return pImpl->SetView(pView);
}

int voOnStreamMPOpen(void* pHandle, void* pSource, int nFlag, int nSourceType)
{
	if(pHandle == NULL)
		return VOOSMP_ERR_Pointer;

	COnStreamMPImpl *pImpl = (COnStreamMPImpl *)pHandle;

	return pImpl->Open(pSource , nFlag , nSourceType);
}

int voOnStreamMPGetProgramCount(void* pHandle, int* pProgramCount)
{
	if(pHandle == NULL)
		return VOOSMP_ERR_Pointer;

	COnStreamMPImpl *pImpl = (COnStreamMPImpl *)pHandle;
	return pImpl->GetProgramCount(pProgramCount);
}

int voOnStreamMPGetProgramInfo(void* pHandle, int nProgramIndex, VOOSMP_SRC_PROGRAM_INFO** ppProgramInfo)
{
	if(pHandle == NULL)
		return VOOSMP_ERR_Pointer;

	COnStreamMPImpl *pImpl = (COnStreamMPImpl *)pHandle;
	return pImpl->GetProgramInfo(nProgramIndex , ppProgramInfo);
}

int voOnStreamMPGetCurTrackInfo(void* pHandle, int nTrackType, VOOSMP_SRC_TRACK_INFO** ppTrackInfo )
{
	if(pHandle == NULL)
		return VOOSMP_ERR_Pointer;

	COnStreamMPImpl *pImpl = (COnStreamMPImpl *)pHandle;
	return pImpl->GetCurTrackInfo(nTrackType , ppTrackInfo);
}

int voOnStreamMPGetSample(void* pHandle, int nTrackType, void* pSample)
{
	if(pHandle == NULL)
		return VOOSMP_ERR_Pointer;

	COnStreamMPImpl *pImpl = (COnStreamMPImpl *)pHandle;
	return pImpl->GetSample(nTrackType , pSample);
}

int voOnStreamMPSelectProgram(void* pHandle, int nProgram)
{
	if(pHandle == NULL)
		return VOOSMP_ERR_Pointer;

	COnStreamMPImpl *pImpl = (COnStreamMPImpl *)pHandle;
	return pImpl->SelectProgram(nProgram);
}

int voOnStreamMPSelectStream(void* pHandle, int nStream)
{
	if(pHandle == NULL)
		return VOOSMP_ERR_Pointer;

	COnStreamMPImpl *pImpl = (COnStreamMPImpl *)pHandle;
	return pImpl->SelectStream(nStream);
}

int voOnStreamMPSelectTrack(void* pHandle, int nTrack)
{
	if(pHandle == NULL)
		return VOOSMP_ERR_Pointer;

	COnStreamMPImpl *pImpl = (COnStreamMPImpl *)pHandle;
	return pImpl->SelectTrack(nTrack);
}


int voOnStreamMPSelectLanguage(void* pHandle, int nIndex)
{
	if(pHandle == NULL)
		return VOOSMP_ERR_Pointer;

	COnStreamMPImpl *pImpl = (COnStreamMPImpl *)pHandle;
	return pImpl->SelectLanguage(nIndex);
}

int voOnStreamMPGetLanguage(void* pHandle, VOOSMP_SUBTITLE_LANGUAGE_INFO** ppLangInfo)
{
	if(pHandle == NULL)
		return VOOSMP_ERR_Pointer;

	COnStreamMPImpl *pImpl = (COnStreamMPImpl *)pHandle;
	return pImpl->GetLanguage(ppLangInfo);
}

int voOnStreamMPClose(void* pHandle)
{
	if(pHandle == NULL)
		return VOOSMP_ERR_Pointer;

	COnStreamMPImpl *pImpl = (COnStreamMPImpl *)pHandle;
	return pImpl->Close();
}

int voOnStreamMPRun(void* pHandle)
{
	if(pHandle == NULL)
		return VOOSMP_ERR_Pointer;

	COnStreamMPImpl *pImpl = (COnStreamMPImpl *)pHandle;
	return pImpl->Run();
}

int voOnStreamMPPause(void* pHandle)
{
	if(pHandle == NULL)
		return VOOSMP_ERR_Pointer;

	COnStreamMPImpl *pImpl = (COnStreamMPImpl *)pHandle;
	return pImpl->Pause();
}


int voOnStreamMPStop(void* pHandle)
{
	if(pHandle == NULL)
		return VOOSMP_ERR_Pointer;

	COnStreamMPImpl *pImpl = (COnStreamMPImpl *)pHandle;
	return pImpl->Stop();
}

int voOnStreamMPGetPos(void* pHandle)
{
	if(pHandle == NULL)
		return VOOSMP_ERR_Pointer;

	COnStreamMPImpl *pImpl = (COnStreamMPImpl *)pHandle;
	return pImpl->GetPos();
}

int voOnStreamMPSetPos(void* pHandle, int nPos)
{
	if(pHandle == NULL)
		return VOOSMP_ERR_Pointer;

	COnStreamMPImpl *pImpl = (COnStreamMPImpl *)pHandle;
	return pImpl->SetPos(nPos);
}

int voOnStreamMPGetDuration(void* pHandle, long long* pDuration)
{
	if(pHandle == NULL)
		return VOOSMP_ERR_Pointer;

	COnStreamMPImpl *pImpl = (COnStreamMPImpl *)pHandle;
	return pImpl->GetDuration(pDuration);
}

int voOnStreamMPGetParam(void* pHandle, int nParamID, void* pValue)
{
	if(pHandle == NULL)
		return VOOSMP_ERR_Pointer;

	COnStreamMPImpl *pImpl = (COnStreamMPImpl *)pHandle;
	return pImpl->GetParam(nParamID , pValue);
}

int voOnStreamMPSetParam(void* pHandle, int nParamID, void* pValue)
{
	if(pHandle == NULL)
		return VOOSMP_ERR_Pointer;

	COnStreamMPImpl *pImpl = (COnStreamMPImpl *)pHandle;
	return pImpl->SetParam(nParamID , pValue);
}


int voGetOnStreamMediaPlayerAPI(voOnStreamMediaPlayerAPI* pFuncSet)
{
	if(pFuncSet == NULL)
		return VOOSMP_ERR_Pointer;

	pFuncSet->Init				= voOnStreamMPInit;
	pFuncSet->Uninit			= voOnStreamMPUninit;
	pFuncSet->SetView			= voOnStreamMPSetView;
	pFuncSet->Open				= voOnStreamMPOpen;
	pFuncSet->GetProgramCount	= voOnStreamMPGetProgramCount;
	pFuncSet->GetProgramInfo	= voOnStreamMPGetProgramInfo;
	pFuncSet->GetCurTrackInfo	= voOnStreamMPGetCurTrackInfo;
	pFuncSet->GetSample			= voOnStreamMPGetSample;
	pFuncSet->SelectProgram		= voOnStreamMPSelectProgram;
	pFuncSet->SelectStream		= voOnStreamMPSelectStream;
	pFuncSet->SelectTrack		= voOnStreamMPSelectTrack;
	pFuncSet->SelectLanguage	= voOnStreamMPSelectLanguage;
	pFuncSet->GetLanguage		= voOnStreamMPGetLanguage;
	pFuncSet->Close				= voOnStreamMPClose;
	pFuncSet->Run				= voOnStreamMPRun;
	pFuncSet->Pause				= voOnStreamMPPause;
	pFuncSet->Stop				= voOnStreamMPStop;
	pFuncSet->GetPos			= voOnStreamMPGetPos;
	pFuncSet->SetPos			= voOnStreamMPSetPos;
	pFuncSet->GetDuration		= voOnStreamMPGetDuration;
	pFuncSet->GetParam			= voOnStreamMPGetParam;
	pFuncSet->SetParam			= voOnStreamMPSetParam;


	return VOOSMP_ERR_None;
}