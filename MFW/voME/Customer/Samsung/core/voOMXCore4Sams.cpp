	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2010				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voOMXCore4Sams.cpp

	Written by:	Xia GuangTai

	Change History (most recent first):
	2010-1-18		 Create file

*******************************************************************************/
#include <OMX_Core.h>
#include <OMX_Component.h>

#include "voOMXBase.h"
#include "voOMXMemory.h"
#include "voCOMXCoreOne4Sams.h"

#include "voLog.h"

OMX_PTR			g_hInst = NULL;
voCOMXCoreOne4Sams * g_pCore = NULL;
bool            g_bVoCoreInit = false;

#define LOG_TAG "voOMXCore4Sams"



OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_Init(void)
{
	VOLOGF ();

	if (g_pCore != NULL)
		return OMX_ErrorNone;

	g_pCore = new voCOMXCoreOne4Sams ();
	if (g_pCore == NULL)
		return OMX_ErrorInsufficientResources;

	g_bVoCoreInit = true;

	return g_pCore->Init (g_hInst);
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_Deinit(void)
{
	VOLOGF ();

	if (g_pCore == NULL)
		return OMX_ErrorUndefined;

	delete g_pCore;
	g_pCore = NULL;

	g_bVoCoreInit = false;

//	voOMXMemShowStatus ();

	return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_ComponentNameEnum(OMX_OUT OMX_STRING cComponentName,
															OMX_IN  OMX_U32 nNameLength,
															OMX_IN  OMX_U32 nIndex)
{
	VOLOGF ();

	if (g_pCore == NULL && g_bVoCoreInit)
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
	{
		if(g_bVoCoreInit)
		{
			return OMX_ErrorUndefined;
		}
		else
		{
			OMX_Init();
		}
	}

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
	if (pTunnelSetup == NULL)
		return OMX_ErrorPortsNotCompatible;

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

