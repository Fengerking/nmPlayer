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
#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#endif // _WIN32


#include <OMX_Core.h>
#include <OMX_Component.h>

#include "voOMXMemory.h"
#include "voOMXBase.h"
#include "voCOMXCoreOne.h"

#include "voLog.h"

OMX_PTR			g_hInst = NULL;
voCOMXCoreOne * g_pCore = NULL;

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
	VOLOGF ();

	if (g_pCore != NULL)
		return OMX_ErrorNone;

	g_pCore = new voCOMXCoreOne ();
	if (g_pCore == NULL)
		return OMX_ErrorInsufficientResources;

	return g_pCore->Init (g_hInst);
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_Deinit(void)
{
	VOLOGF ();

	if (g_pCore == NULL)
		return OMX_ErrorUndefined;

	delete g_pCore;
	g_pCore = NULL;

	voOMXMemShowStatus ();

	return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_ComponentNameEnum(OMX_OUT OMX_STRING cComponentName,
															OMX_IN  OMX_U32 nNameLength,
															OMX_IN  OMX_U32 nIndex)
{
	VOLOGF ();

	if (g_pCore == NULL)
		return OMX_ErrorUndefined;

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
	VOLOGF ();

	if (g_pCore == NULL)
		return OMX_ErrorUndefined;

	OMX_COMPONENTTYPE * pComp = (OMX_COMPONENTTYPE*)voOMXMemAlloc (sizeof (OMX_COMPONENTTYPE));
	if (pComp == NULL)
		return OMX_ErrorInsufficientResources;

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
	VOLOGF ();

	if (g_pCore == NULL)
		return OMX_ErrorUndefined;

	OMX_COMPONENTTYPE * pComp = (OMX_COMPONENTTYPE*)hComponent;
	if (pComp == NULL)
		return OMX_ErrorBadParameter;

	OMX_ERRORTYPE errType = g_pCore->FreeComponent (pComp);
	if (errType != OMX_ErrorNone)
	{
		voOMXMemFree (pComp);
		return errType;
	}

	voOMXMemFree (pComp);

	return OMX_ErrorNone;
}


OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_SetupTunnel(OMX_IN  OMX_HANDLETYPE hOutput,
													OMX_IN  OMX_U32 nPortOutput,
													OMX_IN  OMX_HANDLETYPE hInput,
													OMX_IN  OMX_U32 nPortInput)
{
	VOLOGF ();

	if (hOutput == NULL && hInput == NULL)
		return OMX_ErrorBadParameter;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	OMX_COMPONENTTYPE * pComponent = (OMX_COMPONENTTYPE*)hOutput;

	OMX_TUNNELSETUPTYPE * pTunnelSetup = (OMX_TUNNELSETUPTYPE *)voOMXMemAlloc (sizeof (OMX_TUNNELSETUPTYPE));

	if (hOutput != NULL)
	{
		errType = pComponent->ComponentTunnelRequest (hOutput, nPortOutput, hInput, nPortInput, pTunnelSetup);
		if (errType != OMX_ErrorNone)
		{
			voOMXMemFree (pTunnelSetup);
			return errType;
		}
	}

	pComponent = (OMX_COMPONENTTYPE*)hInput;
	if (hInput != NULL)
	{
		errType = pComponent->ComponentTunnelRequest (hInput, nPortInput, hOutput, nPortOutput, pTunnelSetup);
		if (errType != OMX_ErrorNone)
		{
			errType = pComponent->ComponentTunnelRequest (hOutput, nPortOutput, NULL, 0, pTunnelSetup);
			if (errType != OMX_ErrorNone)
			{
				voOMXMemFree (pTunnelSetup);
				return OMX_ErrorUndefined;
			}

			voOMXMemFree (pTunnelSetup);
			 return OMX_ErrorPortsNotCompatible;
		}
	}

	voOMXMemFree (pTunnelSetup);
	return OMX_ErrorNone;
}


OMX_API OMX_ERRORTYPE   OMX_GetContentPipe(OMX_OUT OMX_HANDLETYPE *hPipe,
											OMX_IN OMX_STRING szURI)
{
	VOLOGF ();

	if (hPipe == NULL)
		return OMX_ErrorUndefined;

	return OMX_ErrorUndefined;
}


OMX_API OMX_ERRORTYPE OMX_GetComponentsOfRole (OMX_IN      OMX_STRING role,
												OMX_INOUT   OMX_U32 *pNumComps,
												OMX_INOUT   OMX_U8  **compNames)
{
	VOLOGF ();

	if (role == NULL)
		return OMX_ErrorUndefined;

	if (g_pCore == NULL)
		return OMX_ErrorUndefined;

	return g_pCore->GetComponentsOfRole (role, pNumComps, compNames);
}

OMX_API OMX_ERRORTYPE OMX_GetRolesOfComponent (OMX_IN      OMX_STRING compName,
												OMX_INOUT   OMX_U32 *pNumRoles,
												OMX_OUT     OMX_U8 **roles)
{
	VOLOGF ();

	if (compName == NULL)
		return OMX_ErrorUndefined;

	if (compName == NULL)
		return OMX_ErrorUndefined;

	return g_pCore->GetRolesOfComponent (compName, pNumRoles, roles);
}

