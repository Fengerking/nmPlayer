	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		COverlayUI.cpp

	Contains:	COverlayUI class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/

#ifdef WIN32_MEMLEAK
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif //WIN32_MEMLEAK

#include "COverlayUI.h"
#include "voOSFunc.h"

COverlayUI::COverlayUI(void)
	: m_hDll (NULL)
	, m_hUI (NULL)
{
	memset (&m_OverlayAPI, 0, sizeof (VOPUI_OVERLAY_API));
}

COverlayUI::~COverlayUI ()
{
	Release();
}

int COverlayUI::Init (VO_PTCHAR pLibPath, VOPUI_INIT_PARAM * pParam)
{	
	if (LoadModule (pLibPath) < 0)
		return -1;

	if (m_OverlayAPI.Init == NULL)
		return -1;

	m_OverlayAPI.Init(pParam, &m_hUI);
	if (m_hUI != NULL)
		return 0;

	return -1;
}

void * COverlayUI::GetView (void)
{
	if (m_OverlayAPI.GetView == NULL || m_hUI == NULL)
		return NULL;

	return m_OverlayAPI.GetView (m_hUI);
}

int COverlayUI::ShowFullScreen (void)
{
	if (m_OverlayAPI.ShowFullScreen == NULL || m_hUI == NULL)
		return NULL;

	return m_OverlayAPI.ShowFullScreen (m_hUI);
}

int COverlayUI::ShowControls (bool bShow)
{
	if (m_OverlayAPI.ShowControls == NULL || m_hUI == NULL)
		return -1;

	return m_OverlayAPI.ShowControls (m_hUI, bShow);
}

int COverlayUI::SetParam (int nID, void * pParam)
{
	if (m_OverlayAPI.SetParam == NULL || m_hUI == NULL)
		return -1;

	return m_OverlayAPI.SetParam (m_hUI, nID, pParam);
}

int COverlayUI::GetParam (int nID, void * pParam)
{
	if (m_OverlayAPI.GetParam == NULL || m_hUI == NULL)
		return -1;

	return m_OverlayAPI.GetParam (m_hUI, nID, pParam);
}

void COverlayUI::Release()
{
	if (m_OverlayAPI.Uninit != NULL && m_hUI != NULL)
	{
		m_OverlayAPI.Uninit (m_hUI);
		m_hUI = NULL;
	}

	if (m_hDll != NULL)
	{
#ifdef _WIN32
		FreeLibrary ((HMODULE)m_hDll);
#else
		dlclose (m_hDll);
#endif		
		m_hDll = NULL;
	}
}

int COverlayUI::LoadModule (VO_PTCHAR path)
{
	Release();

#ifndef _WIN32
	char szDllFile[256];
	int nOk = access("/data/local/voOMXPlayer/lib/debugvolib.txt", F_OK);
	if (nOk == 0)
	{
		strcpy(szDllFile, "/data/local/voOMXPlayer/lib/libvoNPPluginUI.so");
		m_hDll = dlopen (szDllFile, RTLD_NOW);
	}

	if (m_hDll == NULL)
	{
#ifdef _MAC_OS
		strcpy(szDllFile, "voNPPluginUI.dylib");
#else
		strcpy(szDllFile, "libvoNPPluginUI.so");
#endif
		m_hDll = dlopen (szDllFile, RTLD_NOW);
	}

	if (m_hDll == NULL)
	{
#ifdef _MAC_OS
		voOS_GetAppFolder(szDllFile, sizeof(szDllFile));
		strcat(szDllFile, "voNPPluginUI.dylib");
#else		
		strcat(szDllFile, "libvoNPPluginUI.so");
#endif
		m_hDll = dlopen (szDllFile, RTLD_NOW);
	}
	
#ifdef _MAC_OS
	if (m_hDll == NULL)
	{
		voOS_GetPluginModuleFolder(szDllFile, sizeof(szDllFile));
		strcat(szDllFile, "voNPPluginUI.dylib");
		m_hDll = dlopen (szDllFile, RTLD_NOW);
	}
	
	if (m_hDll == NULL)
	{
		strcpy(szDllFile, "/Library/Internet Plug-Ins/");
		strcat(szDllFile, "voNPPluginUI.dylib");
		m_hDll = dlopen (szDllFile, RTLD_NOW);
	}
#endif	

	if (m_hDll == NULL)
	{
		printf("It could not load the module %s!", szDllFile);
		return -1;
	}
	else
	{
		printf("Load module %s", szDllFile);
	}

	m_fGetAPI = (VOGETPLUGINUIAPI) dlsym (m_hDll, ("voGetPlugInUIAPI"));
#else
	TCHAR szDllFile[256];
#ifdef _WIN32
	_tcscpy (szDllFile, _T("voPluginUI.Dll"));
#else// _WIN32
	_tcscpy (szDllFile, _T("libvoPluginUI.Dll"));
#endif// _WIN32

	VO_TCHAR szPath[MAX_PATH];
	vostrcpy(szPath, path);
	VO_TCHAR * pPos = vostrrchr (szPath, _T('\\'));
	if (pPos != NULL)
		*(pPos + 1) = 0;

	vostrcat (szPath, szDllFile);
	m_hDll = LoadLibrary (szPath);
	
	m_fGetAPI = (VOGETPLUGINUIAPI) GetProcAddress (m_hDll, ("voGetPlugInUIAPI"));
#endif

	if (m_fGetAPI == NULL)
		return -1;

	m_fGetAPI (&m_OverlayAPI, 0);

	return 1;
}
