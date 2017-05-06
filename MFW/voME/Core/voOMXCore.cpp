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
#include "OMX_Core.h"
#include "voOMXBase.h"
#include "voCOMXCoreOne.h"

#define LOG_TAG "voOMXCore"
#include "voLog.h"

OMX_PTR			g_hInst = NULL;
voCOMXCoreOne * g_pCore = NULL;
bool            g_bVoCoreInit = false;
VO_TCHAR *		g_pvoOneWorkingPath = NULL;

#ifdef _WIN32
#include <windows.h>

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	g_hInst = (OMX_PTR) hModule;
    return TRUE;
}
#endif // _WIN32

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_Init(void)
{
	if (g_pCore != NULL)
		return OMX_ErrorNone;

	g_pCore = new voCOMXCoreOne ();
	if (g_pCore == NULL)
	{
		VOLOGE ("g_pCore == NULL");
		return OMX_ErrorInsufficientResources;
	}

	g_bVoCoreInit = true;

	if (g_pvoOneWorkingPath != NULL)
		g_pCore->SetWorkingPath ((OMX_STRING)g_pvoOneWorkingPath);

	return g_pCore->Init (g_hInst);
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_Deinit(void)
{
	if (g_pCore == NULL)
	{
		VOLOGE ("g_pCore == NULL");
		return OMX_ErrorUndefined;
	}

	delete g_pCore;
	g_pCore = NULL;

	if (g_pvoOneWorkingPath != NULL)
	{
		delete []g_pvoOneWorkingPath;
		g_pvoOneWorkingPath = NULL;
	}

	g_bVoCoreInit = false;

	voOMXMemShowStatus ();

	return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_ComponentNameEnum(OMX_OUT OMX_STRING cComponentName,
															OMX_IN  OMX_U32 nNameLength,
															OMX_IN  OMX_U32 nIndex)
{
	if (g_pCore == NULL)
	{
		VOLOGE ("g_pCore == NULL");
		return OMX_ErrorUndefined;
	}

	OMX_STRING pName = g_pCore->GetName (nIndex);
	if (pName == NULL)
		return OMX_ErrorNoMore;

	if (nNameLength < strlen (pName))
		return OMX_ErrorBadParameter;

	if (cComponentName != NULL)
		strcpy (cComponentName, pName);

	return OMX_ErrorNone;
}


OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_GetHandle(OMX_OUT OMX_HANDLETYPE* pHandle,
													OMX_IN  OMX_STRING cComponentName,
													OMX_IN  OMX_PTR pAppData,
													OMX_IN  OMX_CALLBACKTYPE* pCallBacks)
{
	if (g_pCore == NULL)
	{
		if(g_bVoCoreInit)
		{
			VOLOGE ("g_pCore == NULL, g_bVoCoreInit");
			return OMX_ErrorUndefined;
		}

		OMX_Init();
	}

	OMX_COMPONENTTYPE * pComp = (OMX_COMPONENTTYPE*)voOMXMemAlloc (sizeof (OMX_COMPONENTTYPE));
	if (pComp == NULL)
	{
		VOLOGE ("pComp == NULL");
		return OMX_ErrorInsufficientResources;
	}

	voOMXMemSet (pComp, 0, sizeof (OMX_COMPONENTTYPE));
	voOMXBase_SetHeader (pComp, sizeof (OMX_COMPONENTTYPE));
	pComp->pApplicationPrivate = pAppData;

	OMX_ERRORTYPE errType = g_pCore->LoadComponent (pComp, cComponentName);
	if (errType != OMX_ErrorNone)
	{
		voOMXMemFree (pComp);
		return errType;
	}

	pComp->SetCallbacks (pComp, pCallBacks, pAppData);
	*pHandle = pComp;

	return OMX_ErrorNone;
}


OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_FreeHandle(OMX_IN  OMX_HANDLETYPE hComponent)
{
	if (g_pCore == NULL)
	{
		VOLOGE ("g_pCore == NULL");
		return OMX_ErrorUndefined;
	}

	OMX_COMPONENTTYPE * pComp = (OMX_COMPONENTTYPE*)hComponent;
	if (pComp == NULL)
		return OMX_ErrorBadParameter;

	OMX_ERRORTYPE errType = g_pCore->FreeComponent (pComp);
	voOMXMemFree (pComp);
	return errType;
}


OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_SetupTunnel(OMX_IN  OMX_HANDLETYPE hOutput,
													OMX_IN  OMX_U32 nPortOutput,
													OMX_IN  OMX_HANDLETYPE hInput,
													OMX_IN  OMX_U32 nPortInput)
{
	if (hOutput == NULL && hInput == NULL)
		return OMX_ErrorBadParameter;

	OMX_TUNNELSETUPTYPE * pTunnelSetup = (OMX_TUNNELSETUPTYPE *)voOMXMemAlloc (sizeof (OMX_TUNNELSETUPTYPE));
	if (pTunnelSetup == NULL)
		return OMX_ErrorInsufficientResources;
	voOMXMemSet (pTunnelSetup, 0, sizeof (OMX_TUNNELSETUPTYPE));

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (hOutput != NULL)
	{
		errType = ((OMX_COMPONENTTYPE*)hOutput)->ComponentTunnelRequest (hOutput, nPortOutput, hInput, nPortInput, pTunnelSetup);
		if (errType != OMX_ErrorNone)
		{
			voOMXMemFree (pTunnelSetup);
			return errType;
		}
	}

	if (hInput != NULL)
	{
		errType = ((OMX_COMPONENTTYPE*)hInput)->ComponentTunnelRequest (hInput, nPortInput, hOutput, nPortOutput, pTunnelSetup);
		if (errType != OMX_ErrorNone)
		{
			errType = ((OMX_COMPONENTTYPE*)hOutput)->ComponentTunnelRequest (hOutput, nPortOutput, NULL, 0, pTunnelSetup);
			voOMXMemFree (pTunnelSetup);
			return (errType != OMX_ErrorNone) ? OMX_ErrorUndefined : OMX_ErrorPortsNotCompatible;
		}
	}

	voOMXMemFree (pTunnelSetup);
	return OMX_ErrorNone;
}


OMX_API OMX_ERRORTYPE   OMX_GetContentPipe(OMX_OUT OMX_HANDLETYPE *hPipe,
											OMX_IN OMX_STRING szURI)
{
	if (hPipe == NULL)
		return OMX_ErrorBadParameter;

	return OMX_ErrorNotImplemented;
}


OMX_API OMX_ERRORTYPE OMX_GetComponentsOfRole (OMX_IN      OMX_STRING role,
												OMX_INOUT   OMX_U32 *pNumComps,
												OMX_INOUT   OMX_U8  **compNames)
{
	if (role == NULL)
		return OMX_ErrorBadParameter;

	if (g_pCore == NULL)
	{
		VOLOGE ("g_pCore == NULL");
		return OMX_ErrorUndefined;
	}

	return g_pCore->GetComponentsOfRole (role, pNumComps, compNames);
}

OMX_API OMX_ERRORTYPE OMX_GetRolesOfComponent (OMX_IN      OMX_STRING compName,
												OMX_INOUT   OMX_U32 *pNumRoles,
												OMX_OUT     OMX_U8 **roles)
{
	if (compName == NULL)
		return OMX_ErrorBadParameter;

	if (g_pCore == NULL)
	{
		VOLOGE ("g_pCore == NULL");
		return OMX_ErrorUndefined;
	}

	return g_pCore->GetRolesOfComponent (compName, pNumRoles, roles);
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_SetWorkingPath (OMX_IN  OMX_STRING cWorkingPath)
{
	if (g_pvoOneWorkingPath == NULL)
		g_pvoOneWorkingPath = new VO_TCHAR[256];

	vostrcpy (g_pvoOneWorkingPath, (VO_TCHAR *)cWorkingPath);

	VOLOGI ("OMX_SetWorkingPath is %s", g_pvoOneWorkingPath);

	return OMX_ErrorNone;
}
