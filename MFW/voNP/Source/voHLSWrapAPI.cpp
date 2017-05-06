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
#include "CHLSServerWrap.h"
#include "voLog.h"

#define VOPLAYER ((CHLSServerWrap*)pHandle)

VO_LOG_PRINT_CB*			g_pLogPrintCallback = NULL;


int voHLSWrapInit(void** phHandle, void* pInitParam)
{
	CHLSServerWrap* pPlayer	= new CHLSServerWrap((VONP_LIB_FUNC*)pInitParam);
	if(!pPlayer)
		return VONP_ERR_Pointer;
	
	pPlayer->SetParam(VONP_PID_COMMON_LOGFUNC, g_pLogPrintCallback);
	int nRet = pPlayer->Init();
	
	*phHandle				= (void*)pPlayer; 
	return nRet;
}

int voHLSWrapUninit(void* pHandle)
{
	if(!VOPLAYER)
		return VONP_ERR_Pointer;
	
	int nRet = VOPLAYER->Uninit();
    
    delete VOPLAYER;

    return nRet;
}

int voHLSWrapSetView(void* pHandle, void* pView)
{
	if(!VOPLAYER)
		return VONP_ERR_Pointer;
	
	return VOPLAYER->SetView(pView);
}

int voHLSWrapOpen(void* pHandle, void* pSource, int nFlag)
{
	if(!VOPLAYER)
		return VONP_ERR_Pointer;
	
	return VOPLAYER->Open(pSource, nFlag);
}

int voHLSWrapClose(void* pHandle)
{
	if(!VOPLAYER)
		return VONP_ERR_Pointer;
	
	return VOPLAYER->Close();
}

int voHLSWrapRun(void* pHandle)
{
	if(!VOPLAYER)
		return VONP_ERR_Pointer;
		
	return VOPLAYER->Run();
}

int voHLSWrapPause(void* pHandle)
{
	if(!VOPLAYER)
		return VONP_ERR_Pointer;
	
	return VOPLAYER->Pause();
}

int voHLSWrapStop(void* pHandle)
{
	if(!VOPLAYER)
		return VONP_ERR_Pointer;
	
	return VOPLAYER->Stop();
}

int voHLSWrapGetPos(void* pHandle)
{
	if(!VOPLAYER)
		return VONP_ERR_Pointer;
	
	return VOPLAYER->GetPos();
}

int voHLSWrapSetPos(void* pHandle, int nPos)
{
	if(!VOPLAYER)
		return VONP_ERR_Pointer;

	return VOPLAYER->SetPos(nPos);
}

int voHLSWrapGetParam(void* pHandle, int nParamID, void* pValue)
{
	if(!VOPLAYER)
		return VONP_ERR_Pointer;
	
	return VOPLAYER->GetParam(nParamID, pValue);
}

int voHLSWrapSetParam(void* pHandle, int nParamID, void* pValue)
{
	if(nParamID == VONP_PID_COMMON_LOGFUNC)
	{
		g_pLogPrintCallback = (VO_LOG_PRINT_CB*)pValue;
		return VONP_ERR_None;
	}
			
	if(!VOPLAYER)
		return VONP_ERR_Pointer;
	
	return VOPLAYER->SetParam(nParamID, pValue);
}

int voGetHLSWrapperAPI(VO_NP_WRAPPER_API* pFuncSet)
{
	if (pFuncSet == 0)
		return VONP_ERR_Pointer;
    
    pFuncSet->Init		= voHLSWrapInit;
    pFuncSet->Uninit	= voHLSWrapUninit;
    pFuncSet->SetView	= voHLSWrapSetView;
    pFuncSet->Open		= voHLSWrapOpen;
    pFuncSet->Close		= voHLSWrapClose;
    pFuncSet->Run		= voHLSWrapRun;
    pFuncSet->Pause		= voHLSWrapPause;
    pFuncSet->Stop		= voHLSWrapStop;
    pFuncSet->GetPos	= voHLSWrapGetPos;
    pFuncSet->SetPos	= voHLSWrapSetPos;
    pFuncSet->GetParam	= voHLSWrapGetParam;
    pFuncSet->SetParam	= voHLSWrapSetParam;

	return VONP_ERR_None;
}
