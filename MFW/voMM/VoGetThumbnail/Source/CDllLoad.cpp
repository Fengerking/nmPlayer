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
#ifdef _LINUX
#include <dlfcn.h>
#endif // _LINUX

#define LOG_TAG "CDllLoad"

#include "windows.h"
#include "voOSFunc.h"
#include "CDllLoad.h"
#include "CVoDllLoader.h"

#include "voLog.h"

CDllLoad::CDllLoad(void)
	: m_pAPIEntry (NULL)
	, m_hDll (NULL)
	, m_pLibOP (NULL)
	, m_pWorkPath (NULL)
{
	VOLOGF ();

	vostrcpy (m_szDllFile, _T(""));
	vostrcpy (m_szAPIName, _T(""));
}

CDllLoad::~CDllLoad ()
{
	VOLOGF ();

	FreeLib ();
}

VO_U32 CDllLoad::LoadLib (VO_HANDLE hInst)
{
	VOLOGF ();
	FreeLib ();
	bool bNeedSave = false;
	if(CVoDllLoader::g_dllLoader)
	{
		if(m_hDll == NULL)
		{
			m_hDll = CVoDllLoader::g_dllLoader->GetDllHandle(m_szDllFile);
		//return VO_ERR_NONE;
			if(m_hDll != NULL)
				bNeedSave = false;
			else
				bNeedSave = true;
		}

	}


#ifdef _WIN32
	VO_TCHAR szDll[MAX_PATH];
	VO_TCHAR szPath[MAX_PATH];

	if (m_pWorkPath != NULL)
	{
		vostrcpy (szDll, m_pWorkPath);
		vostrcat (szDll, m_szDllFile);
		m_hDll = LoadLibrary (szDll);
	}

	if (m_hDll == NULL)
	{
		GetModuleFileName ((HMODULE)hInst, szPath, sizeof (szPath));

		VO_TCHAR * pPos = vostrrchr (szPath, _T('\\'));
		*(pPos + 1) = 0;

		vostrcpy (szDll, szPath);
		vostrcat (szDll, m_szDllFile);
		m_hDll = LoadLibrary (szDll);
	}

	if (m_hDll == NULL)
	{
		::GetModuleFileName (NULL, szPath, sizeof (szPath));
		VO_TCHAR * pPos = vostrrchr (szPath, _T('\\'));
		*(pPos + 1) = 0;

		vostrcpy (szDll, szPath);
		vostrcat (szDll, m_szDllFile);
		m_hDll = LoadLibrary (szDll);
	}

	if (m_hDll == NULL)
	{
		vostrcpy (szDll, szPath);
		vostrcat (szDll, _T("Dll\\"));
		vostrcat (szDll, m_szDllFile);
		m_hDll = LoadLibrary (szDll);
	}

	if (m_hDll == NULL)
		m_hDll = LoadLibrary (m_szDllFile);

	if(bNeedSave && m_hDll)
	{
		CVoDllLoader::g_dllLoader->SetDllHandle(m_szDllFile, m_hDll);
	}

#endif // _WIN32

	if (m_hDll == NULL)
	{
#ifdef LINUX
		VOLOGE ("Load module %s was failed. Error: %s", m_szDllFile, dlerror());
#endif // _LINUX
		return 0;
	}
	VOLOGI ("Load module %s!", m_szDllFile);

	if (vostrlen (m_szAPIName) > 0)
	{
#ifdef _WIN32
#ifdef _WIN32_WCE
		m_pAPIEntry = GetProcAddress (m_hDll, m_szAPIName);
#else
		char szAPI[128];
		memset (szAPI, 0, 128);
		WideCharToMultiByte (CP_ACP, 0, m_szAPIName, -1, szAPI, 128, NULL, NULL);
		m_pAPIEntry = GetProcAddress (m_hDll, szAPI);
#endif // _WIN32_WCE
#elif defined _LINUX
		m_pAPIEntry = dlsym (m_hDll, m_szAPIName);
#endif // _WIN32
	}

	int n1 = GetLastError();
	if (m_pAPIEntry == NULL)
	{
#ifdef LINUX
		VOLOGE ("Get function %s was failed. Error: %s", m_szAPIName, dlerror());
#endif // _LINUX
		return 0;
	}

	return 1;
}

VO_U32 CDllLoad::FreeLib (void)
{
	VOLOGF ();
	if(CVoDllLoader::g_dllLoader)
	{
		return VO_ERR_NONE;
	}

	if (m_pLibOP != NULL)
	{
		if (m_hDll != NULL)
			m_pLibOP->FreeLib (m_pLibOP->pUserData, (VO_PTR)m_hDll, 0);
		m_hDll = NULL;
		return VO_ERR_NONE;
	}

	if (m_hDll != NULL)
#ifdef _WIN32
		FreeLibrary (m_hDll);
#elif defined _LINUX
		dlclose (m_hDll);
#endif // _WIN32

	if (m_hDll != NULL)
		VOLOGI ("Free module %s!", m_szDllFile);
	m_hDll = NULL;

	return VO_ERR_NONE;
}

