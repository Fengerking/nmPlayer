	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CCMMCLiveSource.cpp

	Contains:	CCMMCLiveSource source file

	Written by: 	Thomas Liang

	Change History (most recent first):
	2011-03-09		Thomas Liang			Create file

*******************************************************************************/


#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include "CCMCCLiveSource.h"
#include "CCMCCRTPSrc.h"
#include "CCMCCRTPLiveSrc.h"

#define LOG_TAG 		"CCMCCLiveSource"

void * g_hInst = NULL;

#if defined __cplusplus
extern "C" {
#endif

#ifdef _WIN32
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	g_hInst = hModule;
	return TRUE;
}
#endif // _WIN32
	
VO_U32 voCMCCLive_Open(VO_HANDLE * phLiveSrc, VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData)
{
	CCMCCRTPSrc *pRTPSrc = new CCMCCRTPLiveSrc();

	if(pRTPSrc->Open(pUserData,fStatus,fSendData))
	{
		VOLOGE("%s: open failed", __FUNCTION__);
		*phLiveSrc = NULL;
		delete pRTPSrc;
		
		return -1;
	}
	
	*phLiveSrc = pRTPSrc;
	
	return 0;
}  

VO_U32 voCMCCLive_Close(VO_HANDLE hLiveSrc)
{
	if(!hLiveSrc)
		return -1;
	
	CCMCCRTPSrc *pRTPSrc = (CCMCCRTPSrc *)hLiveSrc;
	
	return pRTPSrc->Close();
}

VO_U32 voCMCCLive_Scan(VO_HANDLE hLiveSrc, VO_LIVESRC_SCAN_PARAMEPTERS* pParam)
{
	return 0;
}

VO_U32 voCMCCLive_Cancel(VO_HANDLE hLiveSrc, VO_U32 nID)
{
	return 0;
}

VO_U32 voCMCCLive_Start(VO_HANDLE hLiveSrc)
{
	if(!hLiveSrc)
		return -1;
	
	CCMCCRTPSrc *pRTPSrc = (CCMCCRTPSrc *)hLiveSrc;	

	return pRTPSrc->Start();
}

VO_U32 voCMCCLive_Pause(VO_HANDLE hLiveSrc)
{
	return 0;
}

VO_U32 voCMCCLive_Stop(VO_HANDLE hLiveSrc)
{
	if(!hLiveSrc)
		return -1;
	
	CCMCCRTPSrc *pRTPSrc = (CCMCCRTPSrc *)hLiveSrc;	
	
	return pRTPSrc->Stop();
}

VO_U32 voCMCCLive_SetDataSource(VO_HANDLE hLiveSrc, VO_PTR pSource, VO_U32 nFlag)
{
	return 0;
}

VO_U32 voCMCCLive_Seek(VO_HANDLE hLiveSrc, VO_U64 nTimeStamp)
{
	return 0;
}

VO_U32 voCMCCLive_GetChannel(VO_HANDLE hLiveSrc, VO_U32* pCount, VO_LIVESRC_CHANNELINFO** ppChannelInfo)
{
	return 0;
}

VO_U32 voCMCCLive_SetChannel(VO_HANDLE hLiveSrc, VO_S32 nChannelID)
{
	return 0;
}

VO_U32 voCMCCLive_GetESG(VO_HANDLE hLiveSrc, VO_S32 nChannelID, VO_LIVESRC_ESG_INFO** ppESGInfo)
{
	return 0;
}

VO_U32 voCMCCLive_GetParam(VO_HANDLE hLiveSrc, VO_U32 nParamID, VO_PTR pParam)
{
	return 0;
}

VO_U32 voCMCCLive_SetParam(VO_HANDLE hLiveSrc, VO_U32 nParamID, VO_PTR pParam)
{
	return 0;
}

VO_S32 VO_API voGetLiveSrcAPI(VO_LIVESRC_API* pAPI)
{
	if (pAPI == NULL)
		return -1;

	pAPI->Open			= voCMCCLive_Open;
	pAPI->Close			= voCMCCLive_Close;
	pAPI->Scan			= voCMCCLive_Scan;
	pAPI->Cancel		= voCMCCLive_Cancel;
	pAPI->Start			= voCMCCLive_Start;
	pAPI->Pause 		= voCMCCLive_Pause;
	pAPI->Stop			= voCMCCLive_Stop;
	pAPI->SetDataSource	= voCMCCLive_SetDataSource;
	pAPI->Seek			= voCMCCLive_Seek;
	pAPI->GetChannel	= voCMCCLive_GetChannel;
	pAPI->SetChannel 	= voCMCCLive_SetChannel;
	pAPI->GetESG 		= voCMCCLive_GetESG;
	pAPI->SetParam		= voCMCCLive_SetParam;
	pAPI->GetParam		= voCMCCLive_GetParam;

	return 0;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

