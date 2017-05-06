	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		COMXLoader.cpp

	Contains:	COMXLoader class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-02-16		JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "voString.h"

#ifdef _LINUX
#  include <dlfcn.h>
#endif // _LINUX

#include "COMXLoader.h"

#define LOG_TAG "CBaseComp"
#include "voLog.h"

#define	VO_OMX_MAX_LOADER_NUM	8

static VO_COMPWRAP_OMX_LOADER	g_OmxLoader[VO_OMX_MAX_LOADER_NUM];
static VO_U32					g_OmxLoaderNum = 0;

COMXLoader::COMXLoader()
	: m_pComponent (NULL)
	, m_hCoreFile (NULL)
	, m_fInit (NULL)
	, m_fDeinit (NULL)
	, m_fNameEnum (NULL)
	, m_fGetHandle (NULL)
	, m_fFreeHandle (NULL)
	, m_fSetupTunnel (NULL)
	, m_fGetPipe (NULL)
	, m_fGetCompRole (NULL)
	, m_fGetRoleComp (NULL)
{
	strcpy (m_szCoreFile, "");
	strcpy (m_szCompName, "");

	if (g_OmxLoaderNum == 0)
	{
		for (OMX_U32 i = 0; i < VO_OMX_MAX_LOADER_NUM; i++)
		{
			strcpy (g_OmxLoader[i].CoreName, "");
			g_OmxLoader[i].Loader  = NULL;
		}
	}
}

COMXLoader::~COMXLoader()
{
	if (strlen (m_szCoreFile) > 0)
	{
		for (OMX_U32 i = 0; i < VO_OMX_MAX_LOADER_NUM; i++)
		{
			if (!strcmp (m_szCoreFile, g_OmxLoader[i].CoreName))
			{
				if (m_pComponent != NULL)
					g_OmxLoader[i].Loader->OMX_FreeHandle (m_pComponent);

				break;
			}
		}
	}
	else
	{
		for (OMX_U32 i = 0; i < VO_OMX_MAX_LOADER_NUM; i++)
		{
			if (g_OmxLoader[i].Loader == this)
			{
				strcpy (g_OmxLoader[i].CoreName, "");
				g_OmxLoader[i].Loader  = NULL;
				break;
			}
		}

		if (m_pComponent != NULL)
			OMX_FreeHandle (m_pComponent);

		OMX_Deinit ();

		if (m_hCoreFile != NULL)
#ifdef _WIN32
			FreeLibrary (m_hCoreFile);
#elif defined LINUX
			dlclose (m_hCoreFile);
#endif // _WIN_32
	}
}

OMX_COMPONENTTYPE * COMXLoader::CreateComponent (OMX_STRING pCoreFile, OMX_STRING	pCompName, OMX_PTR pAppData, OMX_CALLBACKTYPE* pCallBack)
{
	if (m_pComponent != NULL)
		return m_pComponent;

	for (OMX_U32 i = 0; i < VO_OMX_MAX_LOADER_NUM; i++)
	{
		if (!strcmp (pCoreFile, g_OmxLoader[i].CoreName))
		{
			strcpy (m_szCoreFile, pCoreFile);
			strcpy (m_szCompName, pCompName);

			OMX_HANDLETYPE hComp = NULL;
			OMX_ERRORTYPE errType = g_OmxLoader[i].Loader->OMX_GetHandle (&hComp, pCompName, pAppData, pCallBack);
			if (errType != OMX_ErrorNone)
				return NULL;

			m_pComponent = (OMX_COMPONENTTYPE *)hComp;

			return m_pComponent;
		}
	}

	//added by david 2011/12/15
	strcpy(m_szCompName, pCompName);
	if (LoadLib (pCoreFile) != OMX_ErrorNone)
		return NULL;

	if (OMX_Init () != OMX_ErrorNone)
		return NULL;

	for (OMX_U32 i = 0; i < VO_OMX_MAX_LOADER_NUM; i++)
	{
		if (g_OmxLoader[i].Loader == NULL)
		{
			strcpy (g_OmxLoader[i].CoreName, pCoreFile);
			g_OmxLoader[i].Loader = this;
			g_OmxLoaderNum++;
			break;
		}
	}

	OMX_HANDLETYPE hComp = NULL;
	OMX_ERRORTYPE errType = OMX_GetHandle (&hComp, pCompName, pAppData, pCallBack);
	if (errType != OMX_ErrorNone)
		return NULL;
	m_pComponent = (OMX_COMPONENTTYPE *)hComp;

	return m_pComponent;
}

OMX_ERRORTYPE COMXLoader::OMX_Init(void)
{
	if (m_fInit == NULL)
		return OMX_ErrorComponentNotFound;

	return m_fInit ();
}

OMX_ERRORTYPE COMXLoader::OMX_Deinit(void)
{
	if (m_fDeinit == NULL)
		return OMX_ErrorComponentNotFound;

	return m_fDeinit ();
}

OMX_ERRORTYPE COMXLoader::OMX_ComponentNameEnum(OMX_STRING cComponentName, OMX_U32 nNameLength, OMX_U32 nIndex)
{
	if (m_fNameEnum == NULL)
		return OMX_ErrorComponentNotFound;

	return m_fNameEnum (cComponentName, nNameLength, nIndex);
}

OMX_ERRORTYPE COMXLoader::OMX_GetHandle(OMX_HANDLETYPE* pHandle, OMX_STRING cComponentName, OMX_PTR pAppData, OMX_CALLBACKTYPE* pCallBacks)
{
	if (m_fGetHandle == NULL)
		return OMX_ErrorComponentNotFound;

	return m_fGetHandle (pHandle, cComponentName, pAppData, pCallBacks);
}

OMX_ERRORTYPE COMXLoader::OMX_FreeHandle(OMX_HANDLETYPE hComponent)
{
	if (m_fFreeHandle == NULL)
		return OMX_ErrorComponentNotFound;

	return m_fFreeHandle (hComponent);
}

OMX_ERRORTYPE COMXLoader::OMX_SetupTunnel(OMX_HANDLETYPE hOutput, OMX_U32 nPortOutput, OMX_HANDLETYPE hInput, OMX_U32 nPortInput)
{
	if (m_fSetupTunnel == NULL)
		return OMX_ErrorComponentNotFound;

	return m_fSetupTunnel (hOutput, nPortOutput, hInput, nPortInput);
}

OMX_ERRORTYPE COMXLoader::OMX_GetContentPipe(OMX_HANDLETYPE *hPipe,OMX_STRING szURI)
{
	if (m_fGetPipe == NULL)
		return OMX_ErrorComponentNotFound;

	return m_fGetPipe (hPipe, szURI);
}

OMX_ERRORTYPE COMXLoader::OMX_GetComponentsOfRole (OMX_STRING role, OMX_U32 *pNumComps, OMX_U8 **compNames)
{
	if (m_fGetCompRole == NULL)
		return OMX_ErrorComponentNotFound;

	return m_fGetCompRole (role, pNumComps, compNames);
}

OMX_ERRORTYPE COMXLoader::OMX_GetRolesOfComponent (OMX_STRING compName, OMX_U32 *pNumRoles, OMX_U8 **roles)
{
	if (m_fGetRoleComp == NULL)
		return OMX_ErrorComponentNotFound;

	return m_fGetRoleComp (compName, pNumRoles, roles);
}

OMX_ERRORTYPE COMXLoader::LoadLib (OMX_STRING pCoreFile)
{
#ifdef _WIN32
	TCHAR szDll[MAX_PATH];
	memset (szDll, 0, sizeof (szDll));
	::MultiByteToWideChar (CP_ACP, 0, pCoreFile, -1, szDll, MAX_PATH);
	m_hCoreFile = LoadLibrary (szDll);
	if (m_hCoreFile == NULL)
		return OMX_ErrorComponentNotFound;

#ifdef _WIN32_WCE
	m_fInit = (OMX_INIT)::GetProcAddress (m_hCoreFile, _T("OMX_Init"));
	m_fDeinit = (OMX_DEINIT)::GetProcAddress (m_hCoreFile, _T("OMX_Deinit"));
	m_fNameEnum = (OMX_COMPONENTNAMEENUM)::GetProcAddress (m_hCoreFile, _T("OMX_ComponentNameEnum"));
	m_fGetHandle = (OMX_GETHANDLE)::GetProcAddress (m_hCoreFile, _T("OMX_GetHandle"));
	m_fFreeHandle = (OMX_FREEHANDLE)::GetProcAddress (m_hCoreFile, _T("OMX_FreeHandle"));
	m_fSetupTunnel = (OMX_SETUPTUNNEL)::GetProcAddress (m_hCoreFile, _T("OMX_SetupTunnel"));
	m_fGetPipe = (OMX_GETCONTENTPIPE)::GetProcAddress (m_hCoreFile, _T("OMX_GetContentPipe"));
	m_fGetCompRole = (OMX_GETCOMPONENTSOFROLE)::GetProcAddress (m_hCoreFile, _T("OMX_GetComponentsOfRole"));
	m_fGetRoleComp = (OMX_GETROLESOFCOMPONENT)::GetProcAddress (m_hCoreFile, _T("OMX_GetRolesOfComponent"));
#else
	m_fInit = (OMX_INIT)::GetProcAddress (m_hCoreFile, "OMX_Init");
	m_fDeinit = (OMX_DEINIT)::GetProcAddress (m_hCoreFile, "OMX_Deinit");
	m_fNameEnum = (OMX_COMPONENTNAMEENUM)::GetProcAddress (m_hCoreFile, "OMX_ComponentNameEnum");
	m_fGetHandle = (OMX_GETHANDLE)::GetProcAddress (m_hCoreFile, "OMX_GetHandle");
	m_fFreeHandle = (OMX_FREEHANDLE)::GetProcAddress (m_hCoreFile, "OMX_FreeHandle");
	m_fSetupTunnel = (OMX_SETUPTUNNEL)::GetProcAddress (m_hCoreFile, "OMX_SetupTunnel");
	m_fGetPipe = (OMX_GETCONTENTPIPE)::GetProcAddress (m_hCoreFile, "OMX_GetContentPipe");
	m_fGetCompRole = (OMX_GETCOMPONENTSOFROLE)::GetProcAddress (m_hCoreFile, "OMX_GetComponentsOfRole");
	m_fGetRoleComp = (OMX_GETROLESOFCOMPONENT)::GetProcAddress (m_hCoreFile, "OMX_GetRolesOfComponent");
#endif // _WIN32_WCE

#elif defined _LINUX
	// first to load the current directory
	OMX_PTR m_hCoreFile = dlopen (pCoreFile, RTLD_NOW);

	if (m_hCoreFile == NULL)
	{
		printf("%s->%d: load %s failed at: %s\n", __FILE__, __LINE__, pCoreFile, dlerror());
#if defined _LINUX_ANDROID
	    LOGE("%s->%d: load %s failed at: %s\n", __FILE__, __LINE__, pCoreFile, dlerror());	
#endif		
		return OMX_ErrorComponentNotFound;
	}
	char cOutName[256];
	char* pPrex = NULL;

	// please add your vendor prefix here
	if(strncmp(m_szCompName, "OMX.TI", 6) == 0)
		pPrex = (char*)"TI";

	getFuncName(cOutName,  "OMX_Init", pPrex);
	m_fInit = (OMX_INIT)dlsym (m_hCoreFile, cOutName);
	
	getFuncName(cOutName,  "OMX_Deinit", pPrex);
	m_fDeinit = (OMX_DEINIT)dlsym (m_hCoreFile, cOutName);
	
	getFuncName(cOutName,  "OMX_ComponentNameEnum", pPrex);
	m_fNameEnum = (OMX_COMPONENTNAMEENUM)dlsym (m_hCoreFile, cOutName);
	
	getFuncName(cOutName,  "OMX_GetHandle", pPrex);
	m_fGetHandle = (OMX_GETHANDLE)dlsym (m_hCoreFile, cOutName);
	
	getFuncName(cOutName,  "OMX_FreeHandle", pPrex);
	m_fFreeHandle = (OMX_FREEHANDLE)dlsym (m_hCoreFile,cOutName);
	
	getFuncName(cOutName,  "OMX_SetupTunnel", pPrex);
	m_fSetupTunnel = (OMX_SETUPTUNNEL)dlsym (m_hCoreFile, cOutName);

	getFuncName(cOutName,  "OMX_GetContentPipe", pPrex);
	m_fGetPipe = (OMX_GETCONTENTPIPE)dlsym (m_hCoreFile, cOutName);
	
	getFuncName(cOutName,  "OMX_GetComponentsOfRole", pPrex);
	m_fGetCompRole = (OMX_GETCOMPONENTSOFROLE) dlsym(m_hCoreFile, cOutName);

	getFuncName(cOutName,  "OMX_GetRolesOfComponent", pPrex);
	m_fGetRoleComp = (OMX_GETROLESOFCOMPONENT)dlsym(m_hCoreFile, cOutName);

#endif // _WIN32

	if (m_fInit == NULL || m_fDeinit == NULL || m_fNameEnum == NULL || m_fGetHandle == NULL || m_fFreeHandle == NULL ||
		m_fSetupTunnel == NULL || m_fGetPipe == NULL || m_fGetCompRole == NULL || m_fGetRoleComp == NULL)
    {
		return OMX_ErrorComponentNotFound;
    }
	return OMX_ErrorNone;
}

void COMXLoader::getFuncName(char* pOutName, const char* pInName, const char* inPrex,const int nBuffLen)
{
	memset(pOutName, 0, nBuffLen);
	if(inPrex)
		strcpy(pOutName, inPrex);
	assert((strlen(pOutName) + strlen(pInName)) <= nBuffLen);
	strcat(pOutName, pInName);
}

