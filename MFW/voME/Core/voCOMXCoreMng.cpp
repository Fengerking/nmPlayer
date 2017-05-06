	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCoreMng.cpp

	Contains:	voCOMXCoreMng class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#ifdef _WIN32
#  include <tchar.h>
#  include <windows.h>
#elif defined(_LINUX)
#  include "voString.h"
#  include <dlfcn.h>
#endif // _WIN32

#define VOMAX_CONFIG_FILE _T("voOMXCore.cfg")

#include "voCOMXCoreMng.h"
#include "voOMXOSFun.h"

typedef OMX_ERRORTYPE (OMX_APIENTRY * VOGETHANDLE) (OMX_INOUT OMX_COMPONENTTYPE * pHandle);

voCOMXCoreMng::voCOMXCoreMng(void)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
}

voCOMXCoreMng::~voCOMXCoreMng(void)
{
}

OMX_ERRORTYPE voCOMXCoreMng::Init (OMX_PTR hInst)
{
	OMX_ERRORTYPE	errType = OMX_ErrorNone;

#ifdef _WIN32
	TCHAR szFile[MAX_PATH];
	memset (szFile, 0, MAX_PATH * sizeof (TCHAR));
	::GetModuleFileName ((HMODULE)hInst, szFile, MAX_PATH * sizeof (TCHAR));
	TCHAR * pPos = _tcsrchr (szFile, _T('\\'));
	_tcscpy (pPos + 1, VOMAX_CONFIG_FILE);

	if (!m_Config.Open ((OMX_STRING)szFile))
		return OMX_ErrorUndefined;
#elif defined _LINUX
	if (!m_Config.Open (VOMAX_CONFIG_FILE))
		return OMX_ErrorUndefined;
#endif // _WIN32

	return errType;
}

OMX_STRING voCOMXCoreMng::GetName (OMX_U32 nIndex)
{
	return m_Config.GetCompName (nIndex);
}

OMX_ERRORTYPE voCOMXCoreMng::LoadComponent (OMX_COMPONENTTYPE * pHandle, OMX_STRING pName)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	OMX_STRING pFile = m_Config.GetCompFile (pName);
	if (pFile == NULL)
		return OMX_ErrorComponentNotFound;

	OMX_STRING pCompAPI = m_Config.GetCompAPI (pName);

#ifdef _WIN32
	TCHAR szDll[MAX_PATH];
	memset (szDll, 0, sizeof (szDll));
	::MultiByteToWideChar (CP_ACP, 0, pFile, -1, szDll, MAX_PATH);
	HMODULE hDll = LoadLibrary (szDll);
	if (hDll == NULL)
		return OMX_ErrorComponentNotFound;

#ifdef _WIN32_WCE
	TCHAR szAPIName[128];
	memset (szAPIName, 0, 256);
	MultiByteToWideChar (CP_ACP, 0, pCompAPI, -1, szAPIName, 128);
	VOGETHANDLE pGetHandle = (VOGETHANDLE)::GetProcAddress (hDll, szAPIName);
#else
	VOGETHANDLE pGetHandle = (VOGETHANDLE)::GetProcAddress (hDll, pCompAPI);
#endif // _WIN32_WCE
	if (pGetHandle == NULL)
	{
		FreeLibrary (hDll);
		return OMX_ErrorComponentNotFound;
	}
#elif defined _LINUX
	char szAppFolder[256];

	strcpy (szAppFolder, "/data/local/voOMXPlayer/lib/");
	strcat (szAppFolder, pFile);
	OMX_PTR hDll = dlopen (szAppFolder, RTLD_NOW);
	if (hDll == NULL)
	{
		voOMXOS_GetAppFolder(NULL, szAppFolder, sizeof(szAppFolder));
		strcat (szAppFolder, pFile);
		hDll = dlopen (szAppFolder, RTLD_NOW);
	}
	if (hDll == NULL)
	{
		hDll = dlopen (pFile, RTLD_NOW);
	}

	if(!hDll)
	{
		printf("%s----AT last %s can not be loaded  the reason is %s\n", __FUNCTION__, szAppFolder, dlerror());
		return OMX_ErrorComponentNotFound;
	}

	VOGETHANDLE pGetHandle = (VOGETHANDLE) dlsym (hDll, pCompAPI);
	if (pGetHandle == NULL)
	{
		printf("%s---->%s can not be found the reason is %s\n", __FUNCTION__, pCompAPI, dlerror());
		dlclose (hDll);
		return OMX_ErrorComponentNotFound;
	}
#endif // _WIN32

	errType = pGetHandle (pHandle);

	if (errType == OMX_ErrorNone)
	{
       COMXCfgSect * pSect = m_Config.FindSect (pName);
       voOMXCompInfo * pInfo = (voOMXCompInfo *)voOMXMemAlloc (sizeof (voOMXCompInfo));
       voOMXMemSet (pInfo, 0, sizeof (voOMXCompInfo));
       pInfo->Handle = pHandle;
       pInfo->File = hDll;
       pInfo->Priority = m_Config.GetCompPriority (pName);
       pInfo->GroupID = m_Config.GetCompGroupID (pName);

       if (pSect->m_pData != NULL)
          voOMXMemFree (pSect->m_pData);
       pSect->m_pData = pInfo;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXCoreMng::FreeComponent (OMX_COMPONENTTYPE * pHandle)
{
	pHandle->ComponentDeInit (pHandle);

	COMXCfgSect * pSect = m_Config.GetFirstSect ();
	while (pSect != NULL)
	{
		if (pSect->m_pData != NULL)
		{
			voOMXCompInfo * pInfo = (voOMXCompInfo*)pSect->m_pData;
			if (pInfo->Handle == pHandle)
				break;
		}
		pSect = pSect->m_pNext;
	}

	if (pSect != NULL && pSect->m_pData != NULL)
	{
		voOMXCompInfo * pInfo = (voOMXCompInfo*)pSect->m_pData;
#if defined(_WIN32)
		FreeLibrary ((HMODULE)pInfo->File);
#elif defined(_LINUX)
        dlclose((OMX_PTR)pInfo->File);
#endif
		voOMXMemFree (pSect->m_pData);
		pSect->m_pData = NULL;
	}

	return OMX_ErrorNone;
}
