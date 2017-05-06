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
#ifndef _WIN32
#include <dlfcn.h>
#include <string.h>
#endif

#include "vompCEngine.h"
#include "voLog.h"

vompCEngine::vompCEngine()
	: m_hPlay (NULL)
	, m_hDll (NULL)
{
}

vompCEngine::~vompCEngine ()
{
	if (m_hPlay != NULL)
	{
		m_fUninit (m_hPlay);
		m_hPlay = NULL;
	}

#ifdef _WIN32
	if (m_hDll != NULL)
		FreeLibrary (m_hDll);
#else
	if (m_hDll != NULL)
	{
		dlclose (m_hDll);
	}
#endif
}

int vompCEngine::init() {
	LoadDll();
	return initVomp();
}

int vompCEngine::initVomp() {
	if (m_fInit == NULL)
		return -1;
	
	if (m_hPlay != NULL)
	{
		m_fUninit (m_hPlay);
		m_hPlay = NULL;
	}
	
	m_fInit (&m_hPlay, VOMP_PLAYER_MEDIA, OnListener, this);
	if (m_hPlay == NULL)
		return -1;
	
	return 1;
}

int vompCEngine::SetDataSource (void * pSource, int nFlag, int nMode)
{
	if (m_hPlay == NULL || m_fSetSource == NULL)
		return VOMP_ERR_Pointer;

	return m_fSetSource (m_hPlay, pSource, nFlag);
}

int vompCEngine::SendBuffer (VO_BUFFER_INPUT_TYPE eInputType, VOMP_BUFFERTYPE * pBuffer)
{
	if (m_hPlay == NULL || m_fSendBuffer == NULL)
		return VOMP_ERR_Pointer;

	int iType = VOMP_SS_Video;
	
	if (VO_BUFFER_INPUT_VIDEO == eInputType) {
		iType = VOMP_SS_Video;
	}
	else if (VO_BUFFER_INPUT_AUDIO == eInputType) {
		iType = VOMP_SS_Audio;
	}
	else {
		iType = VOMP_SS_TS;
	}

	return m_fSendBuffer (m_hPlay, iType, pBuffer);
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
	if (m_hPlay == NULL)
		init();
	
	if (m_hPlay == NULL || m_fSetParam == NULL)
		return VOMP_ERR_Pointer;

	return m_fSetParam (m_hPlay, nID, pValue);
}

int vompCEngine::LoadDll (void)
{
#ifndef _IOS
	
#ifdef _WIN32
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
	
#elif defined _MAC_OS
#else // _MAC_OS
	if (m_hDll != NULL)
		dlclose (m_hDll);
	
	char szDllFile[256];
	int nOk = access("/data/local/voOMXPlayer/lib/debugvolib.txt", F_OK);
	if (nOk == 0)
	{
		strcpy(szDllFile, "/data/local/voOMXPlayer/lib/libvompEngn.so");
		m_hDll = dlopen (szDllFile, RTLD_NOW);
	}
	
	if (m_hDll == NULL && strlen (m_szWorkPath) > 0)
	{
		sprintf(szDllFile, "%slib/libvompEngn.so", m_szWorkPath);
		m_hDll = dlopen (szDllFile, RTLD_NOW);
	}
	
	if (m_hDll == NULL)
	{
		strcpy(szDllFile, "libvompEngn.so");
		m_hDll = dlopen (szDllFile, RTLD_NOW);
	}
	
	if (m_hDll == NULL)
	{
		VOLOGE ("It could not load the module libvompEngn.so!");
		return -1;
	}
	else
	{
		VOLOGI ("Load module %s", szDllFile);
	}
	
	m_fInit			= (VOMPINIT) dlsym (m_hDll, ("vompInit"));
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
#endif
	

#else // IOS
	//VOMP_FUNCSET funcSet;
	//vompGetFuncSet(&funcSet);
	m_fInit			= vompInit;
	m_fSetSource	= vompSetDataSource;
	m_fSendBuffer	= vompSendBuffer;
	m_fRun			= vompRun;
	m_fPause		= vompPause;
	m_fStop			= vompStop;
	m_fFlush		= vompFlush;
	m_fGetStatus	= vompGetStatus;
	m_fGetDuration	= vompGetDuration;
	m_fGetPos		= vompGetCurPos;
	m_fSetPos		= vompSetCurPos;
	m_fGetParam		= vompGetParam;
	m_fSetParam		= vompSetParam;
	m_fUninit		= vompUninit;
#endif
	
	return 1;
}

int	vompCEngine::OnListener (void * pUserData, int nID, void * pParam1, void * pParam2)
{
	vompCEngine * pPlayer = (vompCEngine *)pUserData;

	return pPlayer->HandleEvent (nID, pParam1, pParam2);
}

int	vompCEngine::HandleEvent (int nID, void * pParam1, void * pParam2)
{
	return 0;
}

int	vompCEngine::SetDrawRect(VOMP_RECT* pRect)
{
	return 0;
}