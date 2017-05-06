	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		source.cpp

	Contains:	get source api cpp file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include "vomeAPI.h"
#include "voCOMXBaseGraph.h"
#include "voCOMXFLOGraph.h"

#undef LOG_TAG
#define LOG_TAG "vomeAPI"
#include "voLog.h"

#define VOME_CHECK_HANDLE if (Handle == NULL)\
	return OMX_ErrorBadParameter;\
voCOMXBaseGraph * pGraph = (voCOMXBaseGraph *)Handle;

OMX_PTR				g_hInst = NULL;

#ifdef _WIN32
#include <windows.h>

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	g_hInst = (OMX_PTR) hModule;
    return TRUE;
}
#endif // _WIN32

OMX_ERRORTYPE vomeInit (OMX_PTR * pHandle, OMX_U32 uFlag, VOMECallBack pCallBack, OMX_PTR pUserData)
{
	voCOMXBaseGraph * pGraph = new voCOMXBaseGraph ();
//	voCOMXBaseGraph * pGraph = new voCOMXFLOGraph ();
	if (pGraph == NULL)
		return OMX_ErrorInsufficientResources;

	pGraph->SetCallBack (pCallBack, pUserData);

/*
	if (pGraph->Init (g_hInst) != OMX_ErrorNone)
	{
		delete pGraph;
		return OMX_ErrorUndefined;
	}
*/
	*pHandle = pGraph;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE vomeUninit (OMX_PTR Handle)
{
	VOME_CHECK_HANDLE


	pGraph->Close ();
	delete pGraph;

	voOMXMemShowStatus ();

	return OMX_ErrorNone;
}

OMX_ERRORTYPE vomeSetDisplayArea (OMX_PTR Handle, OMX_VO_DISPLAYAREATYPE * pDisplay)
{
	VOME_CHECK_HANDLE

	return pGraph->SetDisplayArea (pDisplay);
}

OMX_ERRORTYPE vomeGetParam (OMX_PTR Handle, OMX_S32 nID, OMX_PTR pValue)
{
	VOME_CHECK_HANDLE

	return pGraph->GetParam (nID, pValue);
}

OMX_ERRORTYPE vomeSetParam (OMX_PTR Handle, OMX_S32 nID, OMX_PTR pValue)
{
	VOME_CHECK_HANDLE

	return pGraph->SetParam (nID, pValue);
}

OMX_ERRORTYPE vomePlayback (OMX_PTR Handle, VOME_SOURCECONTENTTYPE * pSource)
{
	VOME_CHECK_HANDLE

	return pGraph->Playback (pSource);
}

OMX_ERRORTYPE vomeRecorder (OMX_PTR Handle, VOME_RECORDERFILETYPE * pRecFile)
{
	VOME_CHECK_HANDLE

	return pGraph->Record (pRecFile);
}

OMX_ERRORTYPE vomeSnapshot (OMX_PTR Handle, VOME_RECORDERIMAGETYPE * pRecImage)
{
	VOME_CHECK_HANDLE

	return pGraph->Snapshot (pRecImage);
}

OMX_ERRORTYPE vomeTranscode (OMX_PTR Handle, VOME_TRANSCODETYPE * pTranscode)
{
	VOME_CHECK_HANDLE

	return pGraph->Transcode (pTranscode);
}

OMX_ERRORTYPE vomeClose (OMX_PTR Handle)
{
	VOME_CHECK_HANDLE

	return pGraph->Close ();
}

OMX_ERRORTYPE vomeRun (OMX_PTR Handle)
{
	VOME_CHECK_HANDLE

	return pGraph->Run ();
}

OMX_ERRORTYPE vomePause (OMX_PTR Handle)
{
	VOME_CHECK_HANDLE

	return pGraph->Pause ();
}

OMX_ERRORTYPE vomeStop (OMX_PTR Handle)
{
	VOME_CHECK_HANDLE

	return pGraph->Stop ();
}

OMX_ERRORTYPE vomeGetStatus (OMX_PTR Handle, VOMESTATUS * pStatus)
{
	VOME_CHECK_HANDLE

	return pGraph->GetStatus (pStatus);
}

OMX_ERRORTYPE vomeGetDuration (OMX_PTR Handle, OMX_S32 * pDuration)
{
	VOME_CHECK_HANDLE

	return pGraph->GetDuration (pDuration);
}

OMX_ERRORTYPE vomeGetCurPos (OMX_PTR Handle, OMX_S32 * pCurPos)
{
	VOME_CHECK_HANDLE

	return pGraph->GetCurPos (pCurPos);
}

OMX_ERRORTYPE vomeSetCurPos (OMX_PTR Handle, OMX_S32 nCurPos)
{
	VOME_CHECK_HANDLE

	return pGraph->SetCurPos (nCurPos);
}

OMX_ERRORTYPE vomeSetCompCallBack (OMX_PTR Handle, OMX_CALLBACKTYPE * pCompCallBack)
{
	VOME_CHECK_HANDLE

	return pGraph->SetCompCallBack (pCompCallBack);
}

OMX_ERRORTYPE vomeEnumComponentName (OMX_PTR Handle, OMX_STRING pCompName, OMX_U32 nIndex)
{
	VOME_CHECK_HANDLE

	return pGraph->EnumComponentName (pCompName, nIndex);
}

OMX_ERRORTYPE vomeGetRolesOfComponent (OMX_PTR Handle, OMX_STRING pCompName, OMX_U32 * pNumRoles, OMX_U8 ** ppRoles)
{
	VOME_CHECK_HANDLE

	return pGraph->GetRolesOfComponent (pCompName, pNumRoles, ppRoles);
}

OMX_ERRORTYPE vomeGetComponentsOfRole (OMX_PTR Handle, OMX_STRING pRole, OMX_U32 * pNumComps, OMX_U8 ** ppCompNames)
{
	VOME_CHECK_HANDLE

	return pGraph->GetComponentsOfRole (pRole, pNumComps, ppCompNames);
}

OMX_ERRORTYPE vomeAddComponent (OMX_PTR Handle, OMX_STRING pName, OMX_COMPONENTTYPE ** ppComponent)
{
	VOME_CHECK_HANDLE

	return pGraph->AddComponent (pName, ppComponent);
}

OMX_ERRORTYPE vomeRemoveComponent (OMX_PTR Handle, OMX_COMPONENTTYPE * pComponent)
{
	VOME_CHECK_HANDLE

	return pGraph->RemoveComponent (pComponent);
}

OMX_ERRORTYPE vomeConnectPorts (OMX_PTR Handle, OMX_COMPONENTTYPE * pOutputComp, OMX_U32 nOutputPort,
								OMX_COMPONENTTYPE * pInputComp, OMX_U32 nInputPort, OMX_BOOL bTunnel)
{
	VOME_CHECK_HANDLE

	return pGraph->ConnectPorts (pOutputComp, nOutputPort, pInputComp, nInputPort, bTunnel);
}

OMX_ERRORTYPE vomeGetComponentByName (OMX_PTR Handle, OMX_STRING pName, OMX_COMPONENTTYPE ** ppComponent)
{
	VOME_CHECK_HANDLE

	return pGraph->GetComponentByName (pName, ppComponent);
}

OMX_ERRORTYPE vomeGetComponentByIndex (OMX_PTR Handle, OMX_U32 nIndex, OMX_COMPONENTTYPE ** ppComponent)
{
	VOME_CHECK_HANDLE

	return pGraph->GetComponentByIndex (nIndex, ppComponent);
}

OMX_ERRORTYPE vomeSaveGraph (OMX_PTR Handle, OMX_STRING pFile)
{
	VOME_CHECK_HANDLE

	return pGraph->SaveGraph (pFile);
}

OMX_ERRORTYPE vomeOpenGraph (OMX_PTR Handle, OMX_STRING pFile)
{
	VOME_CHECK_HANDLE

	return pGraph->OpenGraph (pFile);
}

OMX_ERRORTYPE voOMXGetEngineAPI (VOOMX_ENGINEAPI * pEngine, OMX_U32 uFlag)
{
	pEngine->Init = vomeInit;
	pEngine->Uninit = vomeUninit;
	pEngine->SetDisplayArea = vomeSetDisplayArea;
	pEngine->GetParam = vomeGetParam;
	pEngine->SetParam = vomeSetParam;

	pEngine->Playback = vomePlayback;
	pEngine->Recorder = vomeRecorder;
	pEngine->Snapshot = vomeSnapshot;
	pEngine->Close = vomeClose;

	pEngine->Run = vomeRun;
	pEngine->Pause = vomePause;
	pEngine->Stop = vomeStop;
	pEngine->GetStatus = vomeGetStatus;

	pEngine->GetDuration = vomeGetDuration;
	pEngine->GetCurPos = vomeGetCurPos;
	pEngine->SetCurPos = vomeSetCurPos;

	pEngine->Transcode = vomeTranscode;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voOMXGetComponentAPI (VOOMX_COMPONENTAPI * pCompFunc, OMX_U32 uFlag)
{
	pCompFunc->SetCompCallBack = vomeSetCompCallBack;
	pCompFunc->EnumComponentName = vomeEnumComponentName;
	pCompFunc->GetRolesOfComponent = vomeGetRolesOfComponent;
	pCompFunc->GetComponentsOfRole = vomeGetComponentsOfRole;
	pCompFunc->AddComponent = vomeAddComponent;
	pCompFunc->RemoveComponent = vomeRemoveComponent;
	pCompFunc->ConnectPorts = vomeConnectPorts;
	pCompFunc->GetComponentByName = vomeGetComponentByName;
	pCompFunc->GetComponentByIndex = vomeGetComponentByIndex;
	pCompFunc->SaveGraph = vomeSaveGraph;
	pCompFunc->OpenGraph = vomeOpenGraph;

	return OMX_ErrorNone;
}
