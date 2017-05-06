	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoWindow.cpp

	Contains:	CVideoWindow class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#if defined LINUX
#include <dlfcn.h>
#include "voOSFunc.h"
#endif

#include "CVOMEEngine.h"

#define LOG_TAG "CVOMEEngine"
#include "voLog.h"


typedef OMX_ERRORTYPE (* VOOMXGetEngineAPI) (VOOMX_ENGINEAPI * pEngine, OMX_U32 uFlag);
typedef OMX_ERRORTYPE (* VOOMXGetComponentAPI) (VOOMX_COMPONENTAPI * pComponent, OMX_U32 uFlag);

CVOMEEngine::CVOMEEngine(void)
	: m_hEngine (NULL)
{
}

CVOMEEngine::~CVOMEEngine ()
{
	if (m_hEngine != NULL)
	{
		m_OMXEng.Uninit (m_hEngine);
		m_hEngine = NULL;
	}
}

OMX_ERRORTYPE CVOMEEngine::Init (VOMECallBack pCallBack, OMX_PTR pUserData)
{
	if (m_hEngine != NULL)
		return OMX_ErrorNone;

	if (LoadModule () < 0)
		return OMX_ErrorUndefined;

	m_OMXEng.Init (&m_hEngine, 0, pCallBack, pUserData);
	if (m_hEngine == NULL)
    {
		VOLOGE ("It was failed when init vome!");
		return OMX_ErrorUndefined;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE CVOMEEngine::SetDisplayArea (OMX_VO_DISPLAYAREATYPE * pDisplay)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.SetDisplayArea (m_hEngine, pDisplay);
}

OMX_ERRORTYPE CVOMEEngine::GetParam (OMX_S32 nID, OMX_PTR pValue)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.GetParam (m_hEngine, nID, pValue);
}

OMX_ERRORTYPE CVOMEEngine::SetParam (OMX_S32 nID, OMX_PTR pValue)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.SetParam (m_hEngine, nID, pValue);
}


OMX_ERRORTYPE CVOMEEngine::Playback (VOME_SOURCECONTENTTYPE * pSource)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.Playback (m_hEngine, pSource);
}

OMX_ERRORTYPE CVOMEEngine::Recorder (VOME_RECORDERFILETYPE * pRecFile)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.Recorder (m_hEngine, pRecFile);
}

OMX_ERRORTYPE CVOMEEngine::Snapshot (VOME_RECORDERIMAGETYPE * pRecImage)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.Snapshot (m_hEngine, pRecImage);
}

OMX_ERRORTYPE CVOMEEngine::Close (void)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.Close (m_hEngine);
}


OMX_ERRORTYPE CVOMEEngine::Run (void)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.Run (m_hEngine);
}

OMX_ERRORTYPE CVOMEEngine::Pause (void)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.Pause (m_hEngine);
}

OMX_ERRORTYPE CVOMEEngine::Stop (void)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.Stop (m_hEngine);
}


OMX_ERRORTYPE CVOMEEngine::GetStatus (VOMESTATUS * pStatus)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.GetStatus (m_hEngine, pStatus);
}

OMX_ERRORTYPE CVOMEEngine::GetDuration (OMX_S32 * pDuration)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.GetDuration (m_hEngine, pDuration);
}

OMX_ERRORTYPE CVOMEEngine::GetCurPos (OMX_S32 * pCurPos)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.GetCurPos (m_hEngine, pCurPos);
}

OMX_ERRORTYPE CVOMEEngine::SetCurPos (OMX_S32 nCurPos)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.SetCurPos (m_hEngine, nCurPos);
}

OMX_ERRORTYPE CVOMEEngine::SetCompCallBack (OMX_CALLBACKTYPE * pCompCallBack)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXComp.SetCompCallBack (m_hEngine, pCompCallBack);
}

OMX_ERRORTYPE CVOMEEngine::EnumComponentName (OMX_STRING pCompName, OMX_U32 nIndex)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXComp.EnumComponentName (m_hEngine, pCompName, nIndex);
}

OMX_ERRORTYPE CVOMEEngine::GetRolesOfComponent (OMX_STRING pCompName, OMX_U32 * pNumRoles, OMX_U8 ** ppRoles)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXComp.GetRolesOfComponent (m_hEngine, pCompName, pNumRoles, ppRoles);
}

OMX_ERRORTYPE CVOMEEngine::GetComponentsOfRole (OMX_STRING pRole, OMX_U32 * pNumComps, OMX_U8 ** ppCompNames)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXComp.GetComponentsOfRole (m_hEngine, pRole, pNumComps, ppCompNames);
}

OMX_ERRORTYPE CVOMEEngine::AddComponent (OMX_STRING pName, OMX_COMPONENTTYPE ** ppComponent)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXComp.AddComponent (m_hEngine, pName, ppComponent);
}

OMX_ERRORTYPE CVOMEEngine::RemoveComponent (OMX_COMPONENTTYPE * pComponent)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXComp.RemoveComponent (m_hEngine, pComponent);
}

OMX_ERRORTYPE CVOMEEngine::ConnectPorts (OMX_COMPONENTTYPE * pOutputComp, OMX_U32 nOutputPort,
										OMX_COMPONENTTYPE * pInputComp, OMX_U32 nInputPort, OMX_BOOL bTunnel)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXComp.ConnectPorts (m_hEngine, pOutputComp, nOutputPort, pInputComp, nInputPort, bTunnel);
}

OMX_ERRORTYPE CVOMEEngine::GetComponentByName (OMX_STRING pCompName, OMX_COMPONENTTYPE ** ppComponent)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXComp.GetComponentByName (m_hEngine, pCompName, ppComponent);
}

OMX_ERRORTYPE CVOMEEngine::GetComponentByIndex (OMX_U32 nIndex, OMX_COMPONENTTYPE ** ppComponent)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXComp.GetComponentByIndex (m_hEngine, nIndex, ppComponent);
}

OMX_ERRORTYPE CVOMEEngine::SaveGraph (OMX_STRING pFile)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXComp.SaveGraph (m_hEngine, pFile);
}

OMX_ERRORTYPE CVOMEEngine::OpenGraph (OMX_STRING pFile)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXComp.OpenGraph (m_hEngine, pFile);
}

int CVOMEEngine::LoadModule (void)
{
	vostrcpy(m_szDllFile, _T("voOMXME"));
	vostrcpy(m_szAPIName, _T("voOMXGetEngineAPI"));

#if defined _WIN32
	vostrcat(m_szDllFile, _T(".Dll"));
#elif defined LINUX
	vostrcat(m_szDllFile, _T(".so"));
#endif

	if(CDllLoad::LoadLib(NULL) == 0)
	{
		VOLOGE ("LoadLib fail");
		return -1;
	}

	VOOMXGetEngineAPI pAPI = (VOOMXGetEngineAPI) m_pAPIEntry;
	if (pAPI == NULL)
		return -1;
	pAPI (&m_OMXEng, 0);

#ifdef _WIN32
#ifdef _WIN32_WCE
	VOOMXGetComponentAPI pComp = (VOOMXGetComponentAPI) GetProcAddress (m_hDll, _T("voOMXGetComponentAPI"));
#else
	VOOMXGetComponentAPI pComp = (VOOMXGetComponentAPI) GetProcAddress (m_hDll, "voOMXGetComponentAPI");
#endif // _WIN32_WCE
#elif defined _LINUX
	VOOMXGetComponentAPI pComp = (VOOMXGetComponentAPI) dlsym (m_hDll, "voOMXGetComponentAPI");
#endif // _WIN32

	if (pComp == NULL)
		return -1;

	pComp (&m_OMXComp, 0);

	return 0;
}
