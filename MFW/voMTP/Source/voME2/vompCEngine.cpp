	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		vompCEngine.cpp

	Contains:	vompCEngine class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include "vompCEngine.h"
#include "voLog.h"

#ifdef _LINUX
#include <dlfcn.h>
#endif // _LINUX


vompCEngine::vompCEngine(void* pUserData, VOMPListener pListener)
	: m_hPlay (NULL)
	, m_pUserData(pUserData)
	, m_pListener(pListener)
{
	LoadLib (NULL);
}

vompCEngine::~vompCEngine ()
{
	if (m_hPlay != NULL)
	{
		m_fUninit (m_hPlay);
		m_hPlay = NULL;
	}

	FreeLib();
	//if (m_hDll != NULL)
		//FreeLibrary (m_hDll);
}

int vompCEngine::SetDataSource (void * pSource, int nFlag)
{
	if (m_hPlay == NULL || m_fSetSource == NULL)
		return VOMP_ERR_Pointer;

	return m_fSetSource (m_hPlay, pSource, nFlag);
}

int vompCEngine::SendBuffer (bool bVideo, VOMP_BUFFERTYPE * pBuffer)
{
	if (m_hPlay == NULL || m_fSendBuffer == NULL)
		return VOMP_ERR_Pointer;

	return m_fSendBuffer (m_hPlay, bVideo, pBuffer);
}

int vompCEngine::Run (void)
{
	if (m_hPlay == NULL || m_fRun == NULL)
		return VOMP_ERR_Pointer;

	return m_fRun (m_hPlay);
}

int vompCEngine::Pause (void)
{
	if (m_hPlay == NULL || m_fPause == NULL)
		return VOMP_ERR_Pointer;

	return m_fPause (m_hPlay);
}

int vompCEngine::Stop (void)
{
	if (m_hPlay == NULL || m_fStop == NULL)
		return VOMP_ERR_Pointer;

	return m_fStop (m_hPlay);
}

int vompCEngine::Flush (void)
{
	if (m_hPlay == NULL || m_fFlush == NULL)
		return VOMP_ERR_Pointer;

	return m_fFlush (m_hPlay);
}

int vompCEngine::GetStatus (VOMP_STATUS * pStatus)
{
	if (m_hPlay == NULL || m_fGetStatus == NULL)
		return VOMP_ERR_Pointer;

	return m_fGetStatus (m_hPlay, pStatus);
}

int vompCEngine::GetDuration (int * pDuration)
{
	if (m_hPlay == NULL || m_fGetDuration == NULL)
		return VOMP_ERR_Pointer;

	return m_fGetDuration (m_hPlay, pDuration);
}

int vompCEngine::GetCurPos (int * pCurPos)
{
	if (m_hPlay == NULL || m_fGetPos == NULL)
		return VOMP_ERR_Pointer;

	return m_fGetPos (m_hPlay, pCurPos);
}

int vompCEngine::SetCurPos (int nCurPos)
{
	if (m_hPlay == NULL || m_fSetPos == NULL)
		return VOMP_ERR_Pointer;

	return m_fSetPos (m_hPlay, nCurPos);
}

int vompCEngine::GetParam (int nID, void * pValue)
{
	if (m_hPlay == NULL || m_fGetParam == NULL)
		return VOMP_ERR_Pointer;

	return m_fGetParam (m_hPlay, nID, pValue);
}

int vompCEngine::SetParam (int nID, void * pValue)
{
	if (m_hPlay == NULL || m_fSetParam == NULL)
		return VOMP_ERR_Pointer;

	return m_fSetParam (m_hPlay, nID, pValue);
}


VO_U32 vompCEngine::LoadLib (VO_HANDLE hInst)
{
	/*
	if (m_hDll != NULL)
		FreeLibrary (m_hDll);

	_tcscpy (m_szDllFile, _T("vompEngn.Dll"));

	TCHAR szDll[MAX_PATH];
	TCHAR szPath[MAX_PATH];
	GetModuleFileName (NULL, szPath, sizeof (szPath));
	TCHAR * pPos = _tcsrchr (szPath, _T('\\'));
	*(pPos + 1) = 0;

	_tcscpy (szDll, szPath);
	_tcscat (szDll, m_szDllFile);
	m_hDll = LoadLibrary (szDll);

	if (m_hDll == NULL)
		m_hDll = LoadLibrary (m_szDllFile);

	if (m_hDll == NULL)
		return 0;

	m_fInit			= (VOMPINIT) GetProcAddress (m_hDll, ("vompInit"));
	m_fSetSource	= (VOMPSETDATASOURCE) GetProcAddress (m_hDll, ("vompSetDataSource"));
	m_fSendBuffer	= (VOMPSENDBUFFER) GetProcAddress (m_hDll, ("vompSendBuffer"));
	m_fRun			= (VOMPRUN) GetProcAddress (m_hDll, ("vompRun"));
	m_fPause		= (VOMPPAUSE) GetProcAddress (m_hDll, ("vompPause"));
	m_fStop			= (VOMPSTOP) GetProcAddress (m_hDll, ("vompStop"));
	m_fFlush		= (VOMPFLUSH) GetProcAddress (m_hDll, ("vompFlush"));
	m_fGetStatus	= (VOMPGETSTATUS) GetProcAddress (m_hDll, ("vompGetStatus"));
	m_fGetDuration	= (VOMPGETDURATION) GetProcAddress (m_hDll, ("vompGetDuration"));
	m_fGetPos		= (VOMPGETCURPOS) GetProcAddress (m_hDll, ("vompGetCurPos"));
	m_fSetPos		= (VOMPSETCURPOS) GetProcAddress (m_hDll, ("vompSetCurPos"));
	m_fGetParam		= (VOMPGETPARAM) GetProcAddress (m_hDll, ("vompGetParam"));
	m_fSetParam		= (VOMPSETPARAM) GetProcAddress (m_hDll, ("vompSetParam"));
	m_fUninit		= (VOMPUNINIT) GetProcAddress (m_hDll, ("vompUninit"));

	if (m_fInit == NULL)
		return -1;

	m_fInit (&m_hPlay, 0, OnListener, this);
	if (m_hPlay == NULL)
		return -1;

	return 1;
	*/

	VOLOGF ();
	FreeLib ();

	_tcscpy (m_szDllFile, _T("vompEngn.Dll"));

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

#elif defined _LINUX_X86 || defined NNJ
	VO_TCHAR libname[256];
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
		if (m_hDll == NULL) 
			printf("load: %s, dlerror: %s\n", m_szDllFile, dlerror());
	}

#elif defined _LINUX_ANDROID
	VO_TCHAR libname[256];
	vostrcpy(libname, m_szDllFile);

	FILE * hFile = fopen ("/data/local/voOMXPlayer/lib/debugvolib.txt", "rb");
	if (hFile != NULL)
	{
		fclose (hFile);

		vostrcpy(m_szDllFile, "/data/local/voOMXPlayer/lib/");
		vostrcat(m_szDllFile, libname);
		m_hDll = dlopen (m_szDllFile, RTLD_NOW);
		if (m_hDll == NULL)
		{
			vostrcpy(m_szDllFile, "/data/local/voOMXPlayer/lib/lib");
			vostrcat(m_szDllFile, libname);
			m_hDll = dlopen (m_szDllFile, RTLD_NOW);
		}
	}

	if (m_hDll == NULL)
	{
		if (m_pWorkPath != NULL)
		{
			vostrcpy (m_szDllFile, m_pWorkPath);
			vostrcat (m_szDllFile, libname);
			m_hDll = dlopen (m_szDllFile, RTLD_NOW);

			if (m_hDll == NULL)
			{
				vostrcpy (m_szDllFile, m_pWorkPath);
				vostrcat(m_szDllFile, "lib");
				vostrcat (m_szDllFile, libname);
				m_hDll = dlopen (m_szDllFile, RTLD_NOW);
			}
		}
		else
		{
			vostrcpy (m_szDllFile, libname);
			m_hDll = dlopen (m_szDllFile, RTLD_NOW);
			if (m_hDll == NULL)
			{
				vostrcpy(m_szDllFile, "lib");
				vostrcat(m_szDllFile, libname);
				m_hDll = dlopen (m_szDllFile, RTLD_NOW);
			}
		}
	}

#endif // _WIN32

#ifdef LINUX
	dlerror();
#endif // _LINUX

	if (m_hDll == NULL)
	{
#ifdef LINUX
		VOLOGE ("Load module %s was failed!", m_szDllFile);
#endif // _LINUX
		return 0;
	}

	VOLOGI ("Load module %s!", m_szDllFile);

	//if (vostrlen (m_szAPIName) > 0)
	{
#ifdef _WIN32
#ifdef _WIN32_WCE
		//m_pAPIEntry = GetProcAddress (m_hDll, m_szAPIName);
		m_fInit			= (VOMPINIT) GetProcAddress (m_hDll, ("vompInit"));
		m_fSetSource	= (VOMPSETDATASOURCE) GetProcAddress (m_hDll, ("vompSetDataSource"));
		m_fSendBuffer	= (VOMPSENDBUFFER) GetProcAddress (m_hDll, ("vompSendBuffer"));
		m_fRun			= (VOMPRUN) GetProcAddress (m_hDll, ("vompRun"));
		m_fPause		= (VOMPPAUSE) GetProcAddress (m_hDll, ("vompPause"));
		m_fStop			= (VOMPSTOP) GetProcAddress (m_hDll, ("vompStop"));
		m_fFlush		= (VOMPFLUSH) GetProcAddress (m_hDll, ("vompFlush"));
		m_fGetStatus	= (VOMPGETSTATUS) GetProcAddress (m_hDll, ("vompGetStatus"));
		m_fGetDuration	= (VOMPGETDURATION) GetProcAddress (m_hDll, ("vompGetDuration"));
		m_fGetPos		= (VOMPGETCURPOS) GetProcAddress (m_hDll, ("vompGetCurPos"));
		m_fSetPos		= (VOMPSETCURPOS) GetProcAddress (m_hDll, ("vompSetCurPos"));
		m_fGetParam		= (VOMPGETPARAM) GetProcAddress (m_hDll, ("vompGetParam"));
		m_fSetParam		= (VOMPSETPARAM) GetProcAddress (m_hDll, ("vompSetParam"));
		m_fUninit		= (VOMPUNINIT) GetProcAddress (m_hDll, ("vompUninit"));
#else
		/*char szAPI[128];
		memset (szAPI, 0, 128);
		WideCharToMultiByte (CP_ACP, 0, m_szAPIName, -1, szAPI, 128, NULL, NULL);
		m_pAPIEntry = GetProcAddress (m_hDll, szAPI);
		*/
		m_fInit			= (VOMPINIT) GetProcAddress (m_hDll, ("vompInit"));
		m_fSetSource	= (VOMPSETDATASOURCE) GetProcAddress (m_hDll, ("vompSetDataSource"));
		m_fSendBuffer	= (VOMPSENDBUFFER) GetProcAddress (m_hDll, ("vompSendBuffer"));
		m_fRun			= (VOMPRUN) GetProcAddress (m_hDll, ("vompRun"));
		m_fPause		= (VOMPPAUSE) GetProcAddress (m_hDll, ("vompPause"));
		m_fStop			= (VOMPSTOP) GetProcAddress (m_hDll, ("vompStop"));
		m_fFlush		= (VOMPFLUSH) GetProcAddress (m_hDll, ("vompFlush"));
		m_fGetStatus	= (VOMPGETSTATUS) GetProcAddress (m_hDll, ("vompGetStatus"));
		m_fGetDuration	= (VOMPGETDURATION) GetProcAddress (m_hDll, ("vompGetDuration"));
		m_fGetPos		= (VOMPGETCURPOS) GetProcAddress (m_hDll, ("vompGetCurPos"));
		m_fSetPos		= (VOMPSETCURPOS) GetProcAddress (m_hDll, ("vompSetCurPos"));
		m_fGetParam		= (VOMPGETPARAM) GetProcAddress (m_hDll, ("vompGetParam"));
		m_fSetParam		= (VOMPSETPARAM) GetProcAddress (m_hDll, ("vompSetParam"));
		m_fUninit		= (VOMPUNINIT) GetProcAddress (m_hDll, ("vompUninit"));

#endif // _WIN32_WCE
#elif defined _LINUX
		//m_pAPIEntry = dlsym (m_hDll, m_szAPIName);

		m_fInit			= (VOMPINIT) dlsym (m_hDll, "vompInit");
		m_fSetSource	= (VOMPSETDATASOURCE) dlsym (m_hDll, ("vompSetDataSource"));
		m_fSendBuffer	= (VOMPSENDBUFFER) dlsym (m_hDll, ("vompSendBuffer"));
		m_fRun			= (VOMPRUN) dlsym (m_hDll, ("vompRun"));
		m_fPause		= (VOMPPAUSE) dlsym (m_hDll, ("vompPause"));
		m_fStop			= (VOMPSTOP) dlsym (m_hDll, ("vompStop"));
		m_fFlush		= (VOMPFLUSH) dlsym (m_hDll, ("vompFlush"));
		m_fGetStatus	= (VOMPGETSTATUS) dlsym (m_hDll, ("vompGetStatus"));
		m_fGetDuration	= (VOMPGETDURATION) dlsym (m_hDll, ("vompGetDuration"));
		m_fGetPos		= (VOMPGETCURPOS) dlsym (m_hDll, ("vompGetCurPos"));
		m_fSetPos		= (VOMPSETCURPOS) dlsym (m_hDll, ("vompSetCurPos"));
		m_fGetParam		= (VOMPGETPARAM) dlsym (m_hDll, ("vompGetParam"));
		m_fSetParam		= (VOMPSETPARAM) dlsym (m_hDll, ("vompSetParam"));
		m_fUninit		= (VOMPUNINIT) dlsym (m_hDll, ("vompUninit"));

#endif // _WIN32
	}

	if (m_fInit == NULL)
	{
		VOLOGI (" +++++++ Load module vompEngn.Dll failed!   ++++++");
		return -1;
	}	
		

	m_fInit (&m_hPlay, 0, OnListener, this);
	if (m_hPlay == NULL)
		return -1;

	return 1;
}

int	vompCEngine::OnListener (void * pUserData, int nID, void * pParam1, void * pParam2)
{
	vompCEngine * pPlayer = (vompCEngine *)pUserData;

	return pPlayer->HandleEvent (nID, pParam1, pParam2);
}

int	vompCEngine::HandleEvent (int nID, void * pParam1, void * pParam2)
{
	if (m_pUserData && m_pListener)
	{
		return m_pListener(m_pUserData, nID, pParam1, pParam2);
	}
	return 0;
}
