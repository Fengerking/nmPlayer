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

#include "voOSFunc.h"
#include "CDllLoad.h"
#include "IReaderLoader.h"
#ifdef _WIN32
#pragma warning (disable : 4996)
#endif //_WIN32

CDllLoad::CDllLoad(void)
	: m_hDll (NULL)
	, m_pAPIEntry (NULL)
{
	vostrcpy (m_szDllFile, _T(""));
	vostrcpy (m_szAPIName, _T(""));
}

CDllLoad::~CDllLoad ()
{
	FreeLib ();
}

VO_U32 CDllLoad::LoadLib (VO_HANDLE hInst)
{
	FreeLib ();
#ifdef _WIN32
	VO_TCHAR szDll[MAX_PATH];
	VO_TCHAR szPath[MAX_PATH];
	GetModuleFileName ((HMODULE)hInst, szPath, sizeof (szPath));

	VO_TCHAR * pPos = vostrrchr (szPath, _T('\\'));
	*(pPos + 1) = 0;

	vostrcpy (szDll, szPath);
	vostrcat (szDll, m_szDllFile);
	m_hDll = LoadLibrary (szDll);

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
#elif defined _LINUX
	VO_TCHAR libname[128];
	vostrcpy(libname, m_szDllFile);

    voOS_GetAppFolder(m_szDllFile, sizeof(m_szDllFile));
	vostrcat(m_szDllFile, libname);
	m_hDll = dlopen (m_szDllFile, RTLD_NOW);

	if (m_hDll == NULL)
	{
		voOS_GetAppFolder(m_szDllFile, sizeof(m_szDllFile));
		vostrcat(m_szDllFile, "lib/");
		vostrcat(m_szDllFile, libname);
		m_hDll = dlopen (m_szDllFile, RTLD_NOW);
	}

	if (m_hDll == NULL)
	{
		m_hDll = dlopen (libname, RTLD_NOW);
	}
	if (m_hDll == NULL)
	{
		vostrcpy(m_szDllFile, "/data/local/voOMXPlayer/lib/");
		vostrcat(m_szDllFile, libname);
		m_hDll = dlopen (m_szDllFile, RTLD_NOW);
	}
#endif // _WIN32

	if (m_hDll == NULL)
	{
#ifdef LINUX
		printf("%s->%d: load %s failed at: %s\n", __FILE__, __LINE__, m_szDllFile, dlerror());
#endif // _LINUX
		return 0;
	}

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

	if (m_pAPIEntry == NULL)
	{
#ifdef LINUX
		printf("%s->%d: get function %s failed at: %s\n", __FILE__, __LINE__, m_szAPIName, dlerror());
#endif // _LINUX
		return 0;
	}

	return 1;
}

VO_U32 CDllLoad::FreeLib (void)
{
	if (m_hDll != NULL)
#ifdef _WIN32
		FreeLibrary (m_hDll);
#elif defined _LINUX
		dlclose (m_hDll);
#endif // _WIN32
	m_hDll = NULL;

	return VO_ERR_NONE;
}


class CDllLoad2:public CDllLoad,public IReaderLoader
{
public:
	CDllLoad2(TCHAR* readerName,TCHAR* apiName,VO_PTR hInst)
	{
		vostrcpy (m_szDllFile, readerName);
		vostrcpy (m_szAPIName, apiName);
		LoadLib(hInst);
	}
	voReadAPI GetAPIEntry()
	{
		voReadAPI result = NULL;
		if(m_hDll)
		{
			result = (voReadAPI)m_pAPIEntry;
		}
		return result;
	}
}; 
IReaderLoader*  ReaderLoaderFactory::CreateOneReaderLoader(TCHAR* dllName,TCHAR* readerAPI,VO_PTR hInst)
{
	return new CDllLoad2(dllName,readerAPI,hInst);
}
void					  ReaderLoaderFactory::DestroyOneReaderLoader(IReaderLoader* loader)
{
	delete(loader);
}