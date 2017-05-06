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
#include "stdlib.h"
#include "COMXEngine.h"
#include "voLog.h"

#pragma warning (disable : 4996)

COMXEngine::COMXEngine(void)
	: m_hDll (NULL)
	, m_hEngine (NULL)
{
	VOLOGI("<<<Eng>>> enter");
}

COMXEngine::~COMXEngine ()
{
	VOLOGI("<<<Eng>>> enter");
	Release ();
}

OMX_ERRORTYPE COMXEngine::Init (VOMECallBack pCallBack, OMX_PTR pUserData)
{
  VOLOGI("<<<Eng>>> enter");
  Release ();
  
  if (LoadDll () == 0)
    {
      VOLOGE("load dll failed");
      return OMX_ErrorUndefined;
    }
  m_OMXEng.Init (&m_hEngine, 0, pCallBack, pUserData);
  
  if (m_hEngine != NULL)
    {
      
      VOLOGI(" pCallBack = %x", pCallBack);
#if !defined(_WIN32_XP)
      //m_OMXEng.SetParam (m_hEngine,VOME_PID_WorkingPath, (void*)(VO_APPLICATION_LOCATION));
      m_OMXEng.SetParam (m_hEngine,VOME_PID_WorkingPath, (void*)(vo_application_location));
#endif//_WIN32_XP
      return OMX_ErrorNone;
    }
  else
    {
      VOLOGE("Init failed");
      return OMX_ErrorUndefined;
    }
}

OMX_ERRORTYPE COMXEngine::SetDisplayArea (OMX_VO_DISPLAYAREATYPE * pDisplay)
{
	VOLOGI("<<<Eng>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.SetDisplayArea (m_hEngine, pDisplay);
}

OMX_ERRORTYPE COMXEngine::GetParam (OMX_S32 nID, OMX_PTR pValue)
{
	VOLOGI("<<<Eng>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.GetParam (m_hEngine, nID, pValue);
}

OMX_ERRORTYPE COMXEngine::SetParam (OMX_S32 nID, OMX_PTR pValue)
{
	VOLOGI("<<<Eng>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.SetParam (m_hEngine, nID, pValue);
}


OMX_ERRORTYPE COMXEngine::Playback (VOME_SOURCECONTENTTYPE * pSource)
{
	VOLOGI("<<<Eng>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.Playback (m_hEngine, pSource);
}

OMX_ERRORTYPE COMXEngine::Recorder (VOME_RECORDERFILETYPE * pRecFile)
{
	VOLOGI("<<<Eng>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.Recorder (m_hEngine, pRecFile);
}

OMX_ERRORTYPE COMXEngine::Snapshot (VOME_RECORDERIMAGETYPE * pRecImage)
{
	VOLOGI("<<<Eng>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.Snapshot (m_hEngine, pRecImage);
}

OMX_ERRORTYPE COMXEngine::Close (void)
{
	VOLOGI("<<<Eng>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.Close (m_hEngine);
}


OMX_ERRORTYPE COMXEngine::Run (void)
{
	VOLOGI("<<<Eng>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.Run (m_hEngine);
}

OMX_ERRORTYPE COMXEngine::Pause (void)
{
	VOLOGI("<<<Eng>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.Pause (m_hEngine);
}

OMX_ERRORTYPE COMXEngine::Stop (void)
{
	VOLOGI("<<<Eng>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.Stop (m_hEngine);
}


OMX_ERRORTYPE COMXEngine::GetStatus (VOMESTATUS * pStatus)
{
	
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	OMX_ERRORTYPE result= m_OMXEng.GetStatus (m_hEngine, pStatus);
	//VOLOGI("<<<Eng>>> leave %d",*pStatus);
	return result;
}

OMX_ERRORTYPE COMXEngine::GetDuration (OMX_S32 * pDuration)
{
	VOLOGI("<<<Eng>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.GetDuration (m_hEngine, pDuration);
}

OMX_ERRORTYPE COMXEngine::GetCurPos (OMX_S32 * pCurPos)
{
	VOLOGI("<<<Eng>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.GetCurPos (m_hEngine, pCurPos);
}

OMX_ERRORTYPE COMXEngine::SetCurPos (OMX_S32 nCurPos)
{
	VOLOGI("<<<Eng>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.SetCurPos(m_hEngine, nCurPos);
}

OMX_ERRORTYPE COMXEngine::SetCompCallBack (OMX_CALLBACKTYPE * pCompCallBack)
{
	VOLOGI("<<<Eng>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXComp.SetCompCallBack (m_hEngine, pCompCallBack);
}

OMX_ERRORTYPE COMXEngine::EnumComponentName (OMX_STRING pCompName, OMX_U32 nIndex)
{
	VOLOGI("<<<Eng>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXComp.EnumComponentName (m_hEngine, pCompName, nIndex);
}

OMX_ERRORTYPE COMXEngine::GetRolesOfComponent (OMX_STRING pCompName, OMX_U32 * pNumRoles, OMX_U8 ** ppRoles)
{
	VOLOGI("<<<Eng>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXComp.GetRolesOfComponent (m_hEngine, pCompName, pNumRoles, ppRoles);
}

OMX_ERRORTYPE COMXEngine::GetComponentsOfRole (OMX_STRING pRole, OMX_U32 * pNumComps, OMX_U8 ** ppCompNames)
{
	VOLOGI("<<<Eng>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXComp.GetComponentsOfRole (m_hEngine, pRole, pNumComps, ppCompNames);
}

OMX_ERRORTYPE COMXEngine::AddComponent (OMX_STRING pName, OMX_COMPONENTTYPE ** ppComponent)
{
	VOLOGI("<<<Eng>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXComp.AddComponent (m_hEngine, pName, ppComponent);
}

OMX_ERRORTYPE COMXEngine::RemoveComponent (OMX_COMPONENTTYPE * pComponent)
{
	VOLOGI("<<<Eng>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXComp.RemoveComponent (m_hEngine, pComponent);
}

OMX_ERRORTYPE COMXEngine::ConnectPorts (OMX_COMPONENTTYPE * pOutputComp, OMX_U32 nOutputPort,
										OMX_COMPONENTTYPE * pInputComp, OMX_U32 nInputPort, OMX_BOOL bTunnel)
{
	VOLOGI("<<<Eng>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXComp.ConnectPorts (m_hEngine, pOutputComp, nOutputPort, pInputComp, nInputPort, bTunnel);
}

OMX_ERRORTYPE COMXEngine::SaveGraph (OMX_STRING pFile)
{
	VOLOGI("<<<Eng>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXComp.SaveGraph (m_hEngine, pFile);
}

OMX_ERRORTYPE COMXEngine::OpenGraph (OMX_STRING pFile)
{
	VOLOGI("<<<Eng>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXComp.OpenGraph (m_hEngine, pFile);
}

OMX_U32 COMXEngine::LoadDll (void)
{
	VOLOGI("<<<Eng>>> enter");
#ifdef _WIN32
	if (m_hDll != NULL)
		FreeLibrary (m_hDll);

	vostrcpy (m_szDllFile, _T("voOMXME.Dll"));

	VO_TCHAR szDll[MAX_PATH];
	VO_TCHAR szPath[MAX_PATH];
	GetModuleFileName (NULL, szPath, sizeof (szPath));
	VO_TCHAR * pPos = vostrrchr (szPath, _T('\\'));
	*(pPos + 1) = 0;

	vostrcpy (szDll, szPath);
	vostrcat (szDll, m_szDllFile);
	m_hDll = LoadLibrary (szDll);

	if (m_hDll == NULL)
	{
		DWORD dwError = GetLastError ();

		vostrcpy (szDll, szPath);
		vostrcat (szDll, _T("Dll\\"));
		vostrcat (szDll, m_szDllFile);
		m_hDll = LoadLibrary (szDll);
	}

	if (m_hDll == NULL)
	{
		DWORD dwError = GetLastError ();

		m_hDll = LoadLibrary (m_szDllFile);
	}

	if (m_hDll == NULL)
		return 0;

#ifdef _WIN32_WCE
	VOOMXGETENGINEAPI		pEngAPI = (VOOMXGETENGINEAPI) GetProcAddress (m_hDll, _T("voOMXGetEngineAPI"));
	VOOMXGETCOMPONENTAPI	pCompAPI = (VOOMXGETCOMPONENTAPI) GetProcAddress (m_hDll, _T("voOMXGetComponentAPI"));
#else
	VOOMXGETENGINEAPI		pEngAPI = (VOOMXGETENGINEAPI) GetProcAddress (m_hDll, ("voOMXGetEngineAPI"));
	VOOMXGETCOMPONENTAPI	pCompAPI = (VOOMXGETCOMPONENTAPI) GetProcAddress (m_hDll, ("voOMXGetComponentAPI"));
#endif //_WIN32_WCE
	pEngAPI (&m_OMXEng, 0);
	pCompAPI (&m_OMXComp, 0);
#endif // _WIN32

#if defined LINUX
	
	if (m_hDll != NULL)
		dlclose(m_hDll);
	int pathIdx=0;

	char tmpPath[256];
	strcpy(tmpPath,vo_application_lib_location);strcat(tmpPath,"libvoOMXME.so");
	m_hDll = dlopen (tmpPath, RTLD_NOW);

	if (m_hDll == NULL)
	{
		strcpy (tmpPath, "libvoOMXME.so");
		m_hDll = dlopen (tmpPath, RTLD_NOW);
	}
	
	

	if (m_hDll == NULL)
	{
		VOLOGE ("@@@@@@ Load libvoOMXME.so Error %s ", dlerror ());
		return -1;
	}
	else
	{
		VOLOGE ("@@@@@@ Load libvoOMXME.so done %d ", pathIdx);
	}

	VOOMXGETENGINEAPI pAPI = (VOOMXGETENGINEAPI) dlsym(m_hDll, "voOMXGetEngineAPI");
	pAPI (&m_OMXEng, 0);

	VOOMXGETCOMPONENTAPI	pCompAPI = (VOOMXGETCOMPONENTAPI) dlsym(m_hDll, "voOMXGetComponentAPI");
	pCompAPI (&m_OMXComp, 0);
#endif

	return 1;
}

OMX_U32 COMXEngine::Release (void)
{
	VOLOGI("<<<Eng>>> enter");

	//int nStart = GetTickCount ();
	VOLOGI ("begin");
	if (m_hEngine != NULL)
	{
		m_OMXEng.Uninit (m_hEngine);
		m_hEngine = NULL;
		VOLOGI ("engine is closed");
	}

	

#ifdef _WIN32
	if (m_hDll != NULL)
		FreeLibrary (m_hDll);
#endif // _WIN32
#if defined LINUX
	if (m_hDll != NULL)
		dlclose(m_hDll);
#endif
	m_hDll = NULL;
	VOLOGI ("done");
	return 1;
}
