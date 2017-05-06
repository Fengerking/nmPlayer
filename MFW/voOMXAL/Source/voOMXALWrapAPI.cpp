	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voOMXALWrapAPI.cpp

	Contains:	voOMXALWrapAPI c file

	Written by:	Jim Lin

	Change History (most recent first):
	2012-03-21		Jim			Create file

*******************************************************************************/

#include "voNPWrap.h"
#include "COMXALMediaPlayer.h"
#include "voLog.h"

#define VOPLAYER ((COMXALMediaPlayer*)pHandle)

VO_LOG_PRINT_CB*			g_pLogPrintCallback = NULL;
VO_TCHAR              playerPath[1024];

int voOMXALInit(void** phHandle, void* pInitParam)
{
	COMXALMediaPlayer* pPlayer	= new COMXALMediaPlayer((VONP_LIB_FUNC*)pInitParam);
	if(!pPlayer)
		return VONP_ERR_Pointer;
	
	pPlayer->SetParam(VONP_PID_COMMON_LOGFUNC, g_pLogPrintCallback);
	pPlayer->SetParam(VONP_PID_PLAYER_PATH, playerPath);
	int nRet = pPlayer->Init();
	
	*phHandle				= (void*)pPlayer; 
	return nRet;
}

int voOMXALUninit(void* pHandle)
{
	if(!pHandle)
		return VONP_ERR_Pointer;
	
	COMXALMediaPlayer * pPlayer = (COMXALMediaPlayer *)pHandle;

	int nRet = pPlayer->Uninit();

	delete pPlayer;

	return nRet;
}

int voOMXALSetView(void* pHandle, void* pView)
{
	if(!VOPLAYER)
		return VONP_ERR_Pointer;
	
	return VOPLAYER->SetView(pView);
}

int voOMXALOpen(void* pHandle, void* pSource, int nFlag)
{
	if(!VOPLAYER)
		return VONP_ERR_Pointer;
	
	return VOPLAYER->Open(pSource, nFlag);
}

int voOMXALClose(void* pHandle)
{
	if(!VOPLAYER)
		return VONP_ERR_Pointer;
	
	return VOPLAYER->Close();
}

int voOMXALRun(void* pHandle)
{
	if(!VOPLAYER)
		return VONP_ERR_Pointer;
		
	return VOPLAYER->Run();
}

int voOMXALPause(void* pHandle)
{
	if(!VOPLAYER)
		return VONP_ERR_Pointer;
	
	return VOPLAYER->Pause();
}

int voOMXALStop(void* pHandle)
{
	if(!VOPLAYER)
		return VONP_ERR_Pointer;
	
	return VOPLAYER->Stop();
}

int voOMXALGetPos(void* pHandle)
{
	if(!VOPLAYER)
		return VONP_ERR_Pointer;
	
	return VOPLAYER->GetPos();
}

int voOMXALSetPos(void* pHandle, int nPos)
{
	if(!VOPLAYER)
		return VONP_ERR_Pointer;

	return VOPLAYER->SetPos(nPos);
}

int voOMXALGetParam(void* pHandle, int nParamID, void* pValue)
{
	if(!VOPLAYER)
		return VONP_ERR_Pointer;
	
	return VOPLAYER->GetParam(nParamID, pValue);
}

int voOMXALSetParam(void* pHandle, int nParamID, void* pValue)
{
	if(nParamID == VONP_PID_COMMON_LOGFUNC)
	{
		g_pLogPrintCallback = (VO_LOG_PRINT_CB*)pValue;
		return VONP_ERR_None;
	}
  else if(VONP_PID_PLAYER_PATH == nParamID)
  {
    memset(playerPath, 0, sizeof(VO_TCHAR) * 1024);
    strcpy(playerPath, (VO_TCHAR*)pValue);
		return VONP_ERR_None;
  }
			
	if(!VOPLAYER)
		return VONP_ERR_Pointer;
	
	return VOPLAYER->SetParam(nParamID, pValue);
}

int voOMXALWrapperAPI(VO_NP_WRAPPER_API* pFuncSet)
{
	if (pFuncSet == 0)
		return VONP_ERR_Pointer;
    
    pFuncSet->Init		= voOMXALInit;
    pFuncSet->Uninit	= voOMXALUninit;
    pFuncSet->SetView	= voOMXALSetView;
    pFuncSet->Open		= voOMXALOpen;
    pFuncSet->Close		= voOMXALClose;
    pFuncSet->Run		= voOMXALRun;
    pFuncSet->Pause		= voOMXALPause;
    pFuncSet->Stop		= voOMXALStop;
    pFuncSet->GetPos	= voOMXALGetPos;
    pFuncSet->SetPos	= voOMXALSetPos;
    pFuncSet->GetParam	= voOMXALGetParam;
    pFuncSet->SetParam	= voOMXALSetParam;

	return VONP_ERR_None;
}
