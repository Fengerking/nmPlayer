	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voStageFright.cpp

	Contains:	voStageFright class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-18		JBF			Create file

*******************************************************************************/
#ifdef __SYMBIAN32__
#include <aknnotewrappers.h>
#endif // __SYMBIAN32__

#include "vompAPI.h"
#include "voCBasePlayer.h"
#include "voCMediaPlayer.h"
#include "voCEditPlayer.h"
//#include "voCLivePlayer.h"

#define LOG_TAG "voMP"
#include "voLog.h"

VO_LOG_PRINT_CB		g_vompVOLOGCB;

#ifndef _IOS
VO_PTR		g_hInst = NULL;
#endif

#ifdef _WIN32
#include <windows.h>

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	g_hInst = (VO_PTR) hModule;
    return TRUE;
}
#endif // _WIN32

VO_EXPORT_FUNC int vompInit (void ** phPlayer, int nFlag, VOMPListener pListener, void * pUserData)
{	
	VOLOGI ("nFlag = %d", nFlag);
	
	voCBasePlayer * pPlayer = NULL;
	if (nFlag == VOMP_PLAYER_MEDIA)
		pPlayer = new voCMediaPlayer (pListener, pUserData);
	else if (nFlag == VOMP_PLAYER_EDITOR)
		pPlayer = new voCEditPlayer (pListener, pUserData);
// 	else if (nFlag == VOMP_PLAYER_LIVE)
// 		pPlayer = new voCLivePlayer (pListener, pUserData);
	else
		return VOMP_ERR_ParamID;

	if (pPlayer == NULL)
	{
		VOLOGE ("It could not create player.");
		return VOMP_ERR_OutMemory;
	}
	
	pPlayer->SetParam (VO_PID_COMMON_LOGFUNC, &g_vompVOLOGCB);
	
#ifndef _IOS
	pPlayer->SetInstance (g_hInst);
#endif
	*phPlayer = pPlayer;

	return VOMP_ERR_None;
}

VO_EXPORT_FUNC int vompSetDataSource (void * hPlayer, void * pSource, int nFlag)
{
	if (hPlayer == NULL)
		return VOMP_ERR_Pointer;

	return ((voCBasePlayer *)hPlayer)->SetDataSource (pSource, nFlag);
}

VO_EXPORT_FUNC int vompSendBuffer (void * hPlayer, int nSSType, VOMP_BUFFERTYPE * pBuffer)
{
	if (hPlayer == NULL)
		return VOMP_ERR_Pointer;

	return ((voCBasePlayer *)hPlayer)->SendBuffer (nSSType, pBuffer);
}

VO_EXPORT_FUNC int vompGetVideoBuffer (void * hPlayer, VOMP_BUFFERTYPE ** ppBuffer)
{
	if (hPlayer == NULL)
		return VOMP_ERR_Pointer;

	return ((voCBasePlayer *)hPlayer)->GetVideoBuffer (ppBuffer);
}

VO_EXPORT_FUNC int vompGetAudioBuffer (void * hPlayer, VOMP_BUFFERTYPE ** ppBuffer)
{
	if (hPlayer == NULL)
		return VOMP_ERR_Pointer;

	return ((voCBasePlayer *)hPlayer)->GetAudioBuffer (ppBuffer);
}

VO_EXPORT_FUNC int vompRun (void * hPlayer)
{
	if (hPlayer == NULL)
		return VOMP_ERR_Pointer;

	return ((voCBasePlayer *)hPlayer)->Run ();
}

VO_EXPORT_FUNC int vompPause (void * hPlayer)
{
	if (hPlayer == NULL)
		return VOMP_ERR_Pointer;

	return ((voCBasePlayer *)hPlayer)->Pause ();
}

VO_EXPORT_FUNC int vompStop (void * hPlayer)
{
	if (hPlayer == NULL)
		return VOMP_ERR_Pointer;

	return ((voCBasePlayer *)hPlayer)->Stop ();
}

VO_EXPORT_FUNC int vompFlush (void * hPlayer)
{
	if (hPlayer == NULL)
		return VOMP_ERR_Pointer;

	return ((voCBasePlayer *)hPlayer)->Flush ();
}

VO_EXPORT_FUNC int vompGetStatus (void * hPlayer, VOMP_STATUS * pStatus)
{
	if (hPlayer == NULL)
		return VOMP_ERR_Pointer;

	return ((voCBasePlayer *)hPlayer)->GetStatus (pStatus);
}

VO_EXPORT_FUNC int vompGetDuration (void * hPlayer, int * pDuration)
{
	if (hPlayer == NULL)
		return VOMP_ERR_Pointer;

	return ((voCBasePlayer *)hPlayer)->GetDuration (pDuration);
}

VO_EXPORT_FUNC int vompGetCurPos (void * hPlayer, int * pCurPos)
{
	if (hPlayer == NULL)
		return VOMP_ERR_Pointer;

	return ((voCBasePlayer *)hPlayer)->GetCurPos (pCurPos);
}

VO_EXPORT_FUNC int vompSetCurPos (void * hPlayer, int nCurPos)
{
	if (hPlayer == NULL)
		return VOMP_ERR_Pointer;

	return ((voCBasePlayer *)hPlayer)->SetCurPos (nCurPos);
}

VO_EXPORT_FUNC int vompGetParam (void * hPlayer, int nID, void * pValue)
{
	if (hPlayer == NULL)
		return VOMP_ERR_Pointer;

	return ((voCBasePlayer *)hPlayer)->GetParam (nID, pValue);
}

VO_EXPORT_FUNC int vompSetParam (void * hPlayer, int nID, void * pValue)
{		
	if (nID == VO_PID_COMMON_LOGFUNC && pValue)
	{
		memcpy (&g_vompVOLOGCB, pValue, sizeof (g_vompVOLOGCB));
		
		VO_LOG_PRINT_CB * pVologCB = (VO_LOG_PRINT_CB *)pValue;
		//vologInit (pVologCB->pUserData, pVologCB->fCallBack);
	}
	else if (nID == VOMP_PID_PLAYER_PATH)
	{
		VO_TCHAR* pWorkpath = (VO_TCHAR*)pValue;
		VOLOGINIT(pWorkpath);
	}
	
	if (hPlayer == NULL)
		return VOMP_ERR_Pointer;
		
	return ((voCBasePlayer *)hPlayer)->SetParam (nID, pValue);
}

VO_EXPORT_FUNC int vompUninit (void * hPlayer)
{
	if (hPlayer == NULL)
		return VOMP_ERR_Pointer;

	delete (voCBasePlayer *)hPlayer;

	VOLOGUNINIT();
	
	return 0;
}

VO_EXPORT_FUNC int vompGetFuncSet (VOMP_FUNCSET * pFuncSet)
{
	if (pFuncSet == NULL)
		return VOMP_ERR_Pointer;

	pFuncSet->Init = vompInit;
	pFuncSet->SetDataSource = vompSetDataSource;
	pFuncSet->SendBuffer = vompSendBuffer;
	pFuncSet->GetVideoBuffer = vompGetVideoBuffer;
	pFuncSet->GetAudioBuffer = vompGetAudioBuffer;
	pFuncSet->Run = vompRun;
	pFuncSet->Pause = vompPause;
	pFuncSet->Stop = vompStop;
	pFuncSet->Flush = vompFlush;
	pFuncSet->GetStatus = vompGetStatus;
	pFuncSet->GetDuration = vompGetDuration;
	pFuncSet->GetCurPos = vompGetCurPos;
	pFuncSet->SetCurPos = vompSetCurPos;
	pFuncSet->GetParam = vompGetParam;
	pFuncSet->SetParam = vompSetParam;
	pFuncSet->Uninit = vompUninit;

	memset (&g_vompVOLOGCB, 0, sizeof (g_vompVOLOGCB));
	
	return VOMP_ERR_None;
}
