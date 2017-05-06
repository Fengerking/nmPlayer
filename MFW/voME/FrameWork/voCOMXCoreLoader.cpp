	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCoreLoader.cpp

	Contains:	voCOMXCoreLoader class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-02-16		JBF			Create file

*******************************************************************************/
#include <string.h>
#include "voOMXOSFun.h"

#ifdef _LINUX
#  include <dlfcn.h>
#  include <stdio.h>
#  include "voString.h"
#endif // _LINUX

#include "voCOMXCoreLoader.h"
#include "voLog.h"
#undef LOG_TAG
#define LOG_TAG "voCOMXCoreLoader"

#ifdef _WIN32
#pragma warning (disable : 4996)
#endif

voCoreRefMgr::voCoreRefMgr()
{
}

voCoreRefMgr::~voCoreRefMgr()
{
}

int voCoreRefMgr::AddRef(const char* name)
{
	for(int i = 0; i < VO_MAX_COREREF; i++)
	{
		if(!strcmp(m_Refs[i].name, name))
			return ++(m_Refs[i].ref_num);

		if(!strcmp(m_Refs[i].name, ""))
		{
			//new Item
			strcpy(m_Refs[i].name, name);
			m_Refs[i].ref_num = 1;
			return 1;
		}
	}

	VOLOGW ("Core Ref beyond VO_MAX_COREREF!");
	return 0;
}

int voCoreRefMgr::Release(const char* name)
{
	for(int i = 0; i < VO_MAX_COREREF; i++)
	{
		if(!strcmp(m_Refs[i].name, name))
			return --(m_Refs[i].ref_num);
	}

	//No Item
	return 0;
}

voCoreRefMgr voCOMXCoreLoader::g_CoreRefMgr;

voCOMXCoreLoader::voCOMXCoreLoader()
	: m_bInit (OMX_FALSE)
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
	, m_fSetWorkingPath (NULL)
	, m_pLibOP (NULL)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
	m_nFuncCount = 9;
	m_ppFuncName = new char *[m_nFuncCount];
	for (int i = 0; i < m_nFuncCount; i++)
		m_ppFuncName[i] = new char[128];

	strcpy (m_ppFuncName[0], "OMX_Init");
	strcpy (m_ppFuncName[1], "OMX_Deinit");
	strcpy (m_ppFuncName[2], "OMX_ComponentNameEnum");
	strcpy (m_ppFuncName[3], "OMX_GetHandle");
	strcpy (m_ppFuncName[4], "OMX_FreeHandle");
	strcpy (m_ppFuncName[5], "OMX_SetupTunnel");
	strcpy (m_ppFuncName[6], "OMX_GetContentPipe");
	strcpy (m_ppFuncName[7], "OMX_GetComponentsOfRole");
	strcpy (m_ppFuncName[8], "OMX_GetRolesOfComponent");
}

voCOMXCoreLoader::~voCOMXCoreLoader()
{


	if (m_hCoreFile != NULL)
	{
		OMX_Deinit();

		if (m_pLibOP != NULL)
		{
			m_pLibOP->FreeLib (m_pLibOP->pUserData, m_hCoreFile, 0);
		}
		else
		{
#ifdef _WIN32
			FreeLibrary (m_hCoreFile);
#elif defined _LINUX
			dlclose (m_hCoreFile);
#endif // _WIN32
		}
	}

	for (int i = 0; i < m_nFuncCount; i++)
		delete []m_ppFuncName[i];
	delete []m_ppFuncName;

	m_hCoreFile = NULL;
}

void voCOMXCoreLoader::SetCallBack (VOMECallBack pCallBack, OMX_PTR pUserData)
{
	m_pCallBack = pCallBack;
	m_pUserData = pUserData;
}

OMX_ERRORTYPE voCOMXCoreLoader::SetCoreFile (OMX_STRING pFile, OMX_VO_LIB_OPERATOR * pLibOP, OMX_STRING pPrefix, OMX_STRING pWorkPath)
{
	

	if (pWorkPath != NULL)
	{
		VOLOGI ("WorkPath is %s, OMX Core file is %s", pWorkPath, pFile);
	}
	else
	{
		VOLOGI ("OMX Core file is %s", pFile);
	}

	m_pLibOP = pLibOP;
	strcpy (m_szCoreFile, pFile);

	if (m_pLibOP != NULL && pPrefix == NULL)
	{
#ifdef _WIN32
		m_hCoreFile = (HMODULE)m_pLibOP->LoadLib (m_pLibOP->pUserData, pFile, 0);
#else
		m_hCoreFile = m_pLibOP->LoadLib (m_pLibOP->pUserData, pFile, RTLD_NOW);
#endif //_WIN32

		if (m_hCoreFile == NULL)
		{
			VOLOGE ("Load library %s via Library Operator was failed!", pFile);
			return OMX_ErrorComponentNotFound;
		}

		if (pPrefix != NULL)
		{
			char szFuncName[128];
			for (int i = 0; i < m_nFuncCount; i++)
			{
				strcpy (szFuncName, pPrefix);
				strcat (szFuncName, m_ppFuncName[i]);
				strcpy (m_ppFuncName[i], szFuncName);
			}
		}

		m_fInit = (OMX_INIT)m_pLibOP->GetAddress (m_pLibOP->pUserData, m_hCoreFile, m_ppFuncName[0], 0);
		m_fDeinit = (OMX_DEINIT)m_pLibOP->GetAddress (m_pLibOP->pUserData, m_hCoreFile, m_ppFuncName[1], 0);
		m_fNameEnum = (OMX_COMPONENTNAMEENUM)m_pLibOP->GetAddress (m_pLibOP->pUserData, m_hCoreFile, m_ppFuncName[2], 0);
		m_fGetHandle = (OMX_GETHANDLE)m_pLibOP->GetAddress (m_pLibOP->pUserData, m_hCoreFile, m_ppFuncName[3], 0);
		m_fFreeHandle = (OMX_FREEHANDLE)m_pLibOP->GetAddress (m_pLibOP->pUserData, m_hCoreFile, m_ppFuncName[4], 0);
		m_fSetupTunnel = (OMX_SETUPTUNNEL)m_pLibOP->GetAddress (m_pLibOP->pUserData, m_hCoreFile, m_ppFuncName[5], 0);
		m_fGetPipe = (OMX_GETCONTENTPIPE)m_pLibOP->GetAddress (m_pLibOP->pUserData, m_hCoreFile, m_ppFuncName[6], 0);
		m_fGetCompRole = (OMX_GETCOMPONENTSOFROLE) m_pLibOP->GetAddress(m_pLibOP->pUserData, m_hCoreFile, m_ppFuncName[7], 0);
		m_fGetRoleComp = (OMX_GETROLESOFCOMPONENT)m_pLibOP->GetAddress(m_pLibOP->pUserData, m_hCoreFile, m_ppFuncName[8], 0);
		m_fSetWorkingPath = (OMX_SETWORKINGPATH)m_pLibOP->GetAddress(m_pLibOP->pUserData, m_hCoreFile, (OMX_STRING)"OMX_SetWorkingPath", 0);

		if (m_fInit == NULL || m_fDeinit == NULL || m_fNameEnum == NULL || m_fGetHandle == NULL || m_fFreeHandle == NULL ||
			m_fSetupTunnel == NULL || m_fGetPipe == NULL || m_fGetCompRole == NULL || m_fGetRoleComp == NULL)
		{
			VOLOGE ("It could not get the function address from %s", pFile);
			return OMX_ErrorComponentNotFound;
		}

		VOLOGW ("m_fSetWorkingPath is 0X%08X", (unsigned int)m_fSetWorkingPath);

		if (pWorkPath != NULL)
		{
			if (m_fSetWorkingPath != NULL)
				m_fSetWorkingPath (pWorkPath);
		}

		return OMX_ErrorNone;
	}

#ifdef _WIN32
	TCHAR szDll[MAX_PATH];
	memset (szDll, 0, sizeof (szDll));
	::MultiByteToWideChar (CP_ACP, 0, pFile, -1, szDll, MAX_PATH);
	m_hCoreFile = LoadLibrary (szDll);
	if (m_hCoreFile == NULL && pWorkPath != NULL)
	{
		TCHAR szCoreDll[MAX_PATH];
		_tcscpy (szCoreDll, (TCHAR *)pWorkPath);
		_tcscat (szCoreDll, szDll);
		m_hCoreFile = LoadLibrary (szCoreDll);
	}

	if (m_hCoreFile == NULL)
	{
		VOLOGE ("The Core File %s could not be loaded", pFile);
		return OMX_ErrorComponentNotFound;
	}

#ifdef _WIN32_WCE
	m_fInit = (OMX_INIT)::GetProcAddress (m_hCoreFile, _T("OMX_Init"));
	m_fDeinit = (OMX_DEINIT)::GetProcAddress (m_hCoreFile, _T("OMX_Deinit"));
	m_fNameEnum = (OMX_COMPONENTNAMEENUM)::GetProcAddress (m_hCoreFile, _T("OMX_ComponentNameEnum"));
	m_fGetHandle = (OMX_GETHANDLE)::GetProcAddress (m_hCoreFile, _T("OMX_GetHandle"));
	m_fFreeHandle = (OMX_FREEHANDLE)::GetProcAddress (m_hCoreFile, _T("OMX_FreeHandle"));
	m_fSetupTunnel = (OMX_SETUPTUNNEL)::GetProcAddress (m_hCoreFile, _T("OMX_SetupTunnel"));
	m_fGetPipe = (OMX_GETCONTENTPIPE)::GetProcAddress (m_hCoreFile, _T("OMX_GetContentPipe"));
	m_fGetCompRole = (OMX_GETCOMPONENTSOFROLE)::GetProcAddress (m_hCoreFile, _T("OMX_GetComponentsOfRole"));
	m_fSetWorkingPath = (OMX_SETWORKINGPATH)::GetProcAddress (m_hCoreFile, _T("OMX_SetWorkingPath"));
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
	m_fSetWorkingPath = (OMX_SETWORKINGPATH)::GetProcAddress (m_hCoreFile, "OMX_SetWorkingPath");
#endif // _WIN32_WCE

#elif defined _LINUX
	char szDll[256];

//	FILE * hFile = fopen ("/data/local/voOMXPlayer/lib/debugvolib.txt", "rb");
//	if (hFile != NULL)
	int nRC = access("/data/local/voOMXPlayer/lib/debugvolib.txt", F_OK);
	if (nRC == 0)
	{
//		fclose (hFile);
		strcpy (szDll, "/data/local/voOMXPlayer/lib/");
		strcat (szDll, pFile);
		m_hCoreFile = dlopen (szDll, RTLD_NOW);
		if (m_hCoreFile == NULL)
		{
			strcpy (szDll, "/data/local/voOMXPlayer/lib/lib");
			strcat (szDll, pFile);
			m_hCoreFile = dlopen (szDll, RTLD_NOW);
		}
	}

	// try working path
    if (m_hCoreFile == NULL)
    {
		if (pWorkPath != NULL)
		{
			strcpy (szDll, pWorkPath);
    		strcat (szDll, pFile);
			m_hCoreFile = dlopen (szDll, RTLD_NOW);
			if (m_hCoreFile == NULL)
			{
				strcpy (szDll, pWorkPath);
    			strcat (szDll, "lib");
    			strcat (szDll, pFile);
				m_hCoreFile = dlopen (szDll, RTLD_NOW);
			}

			if (m_hCoreFile == NULL)
			{
				strcpy (szDll, pWorkPath);
    			strcat (szDll, "lib/");
    			strcat (szDll, pFile);
				m_hCoreFile = dlopen (szDll, RTLD_NOW);
			}

			if (m_hCoreFile == NULL)
			{
				strcpy (szDll, pWorkPath);
				strcat (szDll, "lib/lib");
				strcat (szDll, pFile);
				m_hCoreFile = dlopen (szDll, RTLD_NOW);
			}

			if (m_hCoreFile == NULL && strncmp (pFile, "libvo", 5))
			{
				vostrcpy (szDll, pFile);
				m_hCoreFile = dlopen (szDll, RTLD_NOW);
			}
		}
		else
		{
    		m_hCoreFile = dlopen (pFile, RTLD_NOW);
			if (m_hCoreFile == NULL)
			{
				strcpy (szDll, "lib");
				strcat (szDll, pFile);
				m_hCoreFile = dlopen (szDll, RTLD_NOW);	
			}
		}
	}

	dlerror ();

	if (m_hCoreFile == NULL)
    {
		VOLOGW ("The Core File %s could not be loaded", pFile);
		return OMX_ErrorComponentNotFound;
  	}

	if (pPrefix == NULL)
	{
		m_fInit = (OMX_INIT)dlsym (m_hCoreFile, "OMX_Init");
		m_fDeinit = (OMX_DEINIT)dlsym (m_hCoreFile, "OMX_Deinit");
		m_fNameEnum = (OMX_COMPONENTNAMEENUM)dlsym (m_hCoreFile, "OMX_ComponentNameEnum");
		m_fGetHandle = (OMX_GETHANDLE)dlsym (m_hCoreFile, "OMX_GetHandle");
		m_fFreeHandle = (OMX_FREEHANDLE)dlsym (m_hCoreFile, "OMX_FreeHandle");
		m_fSetupTunnel = (OMX_SETUPTUNNEL)dlsym (m_hCoreFile, "OMX_SetupTunnel");
		m_fGetPipe = (OMX_GETCONTENTPIPE)dlsym (m_hCoreFile, "OMX_GetContentPipe");
		m_fGetCompRole = (OMX_GETCOMPONENTSOFROLE) dlsym(m_hCoreFile, "OMX_GetComponentsOfRole");
		m_fGetRoleComp = (OMX_GETROLESOFCOMPONENT)dlsym(m_hCoreFile, "OMX_GetRolesOfComponent");
		m_fSetWorkingPath = (OMX_SETWORKINGPATH)dlsym(m_hCoreFile, "OMX_SetWorkingPath");
	}
	else
	{
		char szFunName[128];

		strcpy (szFunName, pPrefix);
		strcat (szFunName, "OMX_Init");
		m_fInit = (OMX_INIT)dlsym (m_hCoreFile, szFunName);

		strcpy (szFunName, pPrefix);
		strcat (szFunName, "OMX_Deinit");
		m_fDeinit = (OMX_DEINIT)dlsym (m_hCoreFile, szFunName);

		strcpy (szFunName, pPrefix);
		strcat (szFunName, "OMX_ComponentNameEnum");
		m_fNameEnum = (OMX_COMPONENTNAMEENUM)dlsym (m_hCoreFile, szFunName);

		strcpy (szFunName, pPrefix);
		strcat (szFunName, "OMX_GetHandle");
		m_fGetHandle = (OMX_GETHANDLE)dlsym (m_hCoreFile, szFunName);

		strcpy (szFunName, pPrefix);
		strcat (szFunName, "OMX_FreeHandle");
		m_fFreeHandle = (OMX_FREEHANDLE)dlsym (m_hCoreFile, szFunName);

		strcpy (szFunName, pPrefix);
		strcat (szFunName, "OMX_SetupTunnel");
		m_fSetupTunnel = (OMX_SETUPTUNNEL)dlsym (m_hCoreFile, szFunName);

		strcpy (szFunName, pPrefix);
		strcat (szFunName, "OMX_GetContentPipe");
		m_fGetPipe = (OMX_GETCONTENTPIPE)dlsym (m_hCoreFile, szFunName);

		strcpy (szFunName, pPrefix);
		strcat (szFunName, "OMX_GetComponentsOfRole");
		m_fGetCompRole = (OMX_GETCOMPONENTSOFROLE)dlsym (m_hCoreFile, szFunName);

		strcpy (szFunName, pPrefix);
		strcat (szFunName, "OMX_GetRolesOfComponent");
		m_fGetRoleComp = (OMX_GETROLESOFCOMPONENT)dlsym (m_hCoreFile, szFunName);

/*
		m_fInit = (OMX_INIT)dlsym (m_hCoreFile, "TIOMX_Init");
		m_fDeinit = (OMX_DEINIT)dlsym (m_hCoreFile, "TIOMX_Deinit");
		m_fNameEnum = (OMX_COMPONENTNAMEENUM)dlsym (m_hCoreFile, "TIOMX_ComponentNameEnum");
		m_fGetHandle = (OMX_GETHANDLE)dlsym (m_hCoreFile, "TIOMX_GetHandle");
		m_fFreeHandle = (OMX_FREEHANDLE)dlsym (m_hCoreFile, "TIOMX_FreeHandle");
		m_fSetupTunnel = (OMX_SETUPTUNNEL)dlsym (m_hCoreFile, "TIOMX_SetupTunnel");
		m_fGetPipe = (OMX_GETCONTENTPIPE)dlsym (m_hCoreFile, "TIOMX_GetContentPipe");
		m_fGetCompRole = (OMX_GETCOMPONENTSOFROLE)dlsym (m_hCoreFile, "TIOMX_GetComponentsOfRole");
		m_fGetRoleComp = (OMX_GETROLESOFCOMPONENT)dlsym (m_hCoreFile, "TIOMX_GetRolesOfComponent");
*/
	}

#endif // _WIN32

	VOLOGI ("Loaded module %s", szDll);

	if (m_fInit == NULL || m_fDeinit == NULL || m_fNameEnum == NULL || m_fGetHandle == NULL || m_fFreeHandle == NULL ||
		m_fSetupTunnel == NULL)// || m_fGetPipe == NULL || m_fGetCompRole == NULL || m_fGetRoleComp == NULL)
    {
		VOLOGE ("It could not get the function address from %s", pFile);
		return OMX_ErrorComponentNotFound;
    }

	VOLOGW ("m_fSetWorkingPath is 0X%08X", (unsigned int)m_fSetWorkingPath);

	if (pWorkPath != NULL)
	{
		if (m_fSetWorkingPath != NULL)
			m_fSetWorkingPath (pWorkPath);
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCoreLoader::OMX_Init(void)
{
	if (m_fInit == NULL)
		return OMX_ErrorComponentNotFound;

	if (m_bInit)
		return OMX_ErrorNone;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (g_CoreRefMgr.AddRef (m_szCoreFile) <= 1)
		errType = m_fInit ();

	if (errType != OMX_ErrorNone)
		VOLOGE ("The result of m_fInit is 0X%08X", errType);

	m_bInit = OMX_TRUE;

	return errType;
}

OMX_ERRORTYPE voCOMXCoreLoader::OMX_Deinit(void)
{
	if (m_fDeinit == NULL)
		return OMX_ErrorComponentNotFound;

	if (!m_bInit)
		return OMX_ErrorNone;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (g_CoreRefMgr.Release (m_szCoreFile) <= 0)
		errType = m_fDeinit ();

	if (errType != OMX_ErrorNone)
		VOLOGE ("The result of m_fDeinit is 0X%08X", errType);

	m_bInit = OMX_FALSE;

	return errType;
}

OMX_ERRORTYPE voCOMXCoreLoader::OMX_ComponentNameEnum(OMX_STRING cComponentName, OMX_U32 nNameLength, OMX_U32 nIndex)
{


	if (m_fNameEnum == NULL)
	{
		VOLOGE ("m_fNameEnum == NULL");
		return OMX_ErrorComponentNotFound;
	}

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (!m_bInit)
	{
		errType = OMX_Init ();
		if (errType != OMX_ErrorNone)
			return errType;
	}

	errType = m_fNameEnum (cComponentName, nNameLength, nIndex);

	if (errType == OMX_ErrorNone)
		VOLOGI ("The %d Component Name is %s", (int)nIndex, cComponentName);

	return errType;
}

OMX_ERRORTYPE voCOMXCoreLoader::OMX_GetHandle(OMX_HANDLETYPE* pHandle, OMX_STRING cComponentName, OMX_PTR pAppData, OMX_CALLBACKTYPE* pCallBacks)
{

	if (m_fGetHandle == NULL)
	{
		VOLOGE ("m_fGetHandle == NULL");
		return OMX_ErrorComponentNotFound;
	}

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (!m_bInit)
	{
		errType = OMX_Init ();
		if (errType != OMX_ErrorNone)
			return errType;
	}

	errType = m_fGetHandle (pHandle, cComponentName, pAppData, pCallBacks);
	if (errType != OMX_ErrorNone)
		VOLOGE ("The result of OMX_GetHandle is 0X%08X. The Component name is %s", errType, cComponentName);

	return errType;
}

OMX_ERRORTYPE voCOMXCoreLoader::OMX_FreeHandle(OMX_HANDLETYPE hComponent)
{


	if (m_fFreeHandle == NULL)
		return OMX_ErrorComponentNotFound;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (!m_bInit)
	{
		errType = OMX_Init ();
		if (errType != OMX_ErrorNone)
			return errType;
	}

	errType = m_fFreeHandle (hComponent);
	if (errType != OMX_ErrorNone)
		VOLOGE ("The result of OMX_FreeHandle is 0X%08X. The Component handle is 0X%08X", errType, (int)hComponent);

	return errType;
}

OMX_ERRORTYPE voCOMXCoreLoader::OMX_SetupTunnel(OMX_HANDLETYPE hOutput, OMX_U32 nPortOutput, OMX_HANDLETYPE hInput, OMX_U32 nPortInput)
{


	if (m_fSetupTunnel == NULL)
		return OMX_ErrorComponentNotFound;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (!m_bInit)
	{
		errType = OMX_Init ();
		if (errType != OMX_ErrorNone)
			return errType;
	}

	errType = m_fSetupTunnel (hOutput, nPortOutput, hInput, nPortInput);
	if (errType != OMX_ErrorNone)
		VOLOGE ("The result of OMX_SetupTunnel is 0X%08X", errType);

	return errType;
}

OMX_ERRORTYPE voCOMXCoreLoader::OMX_GetContentPipe(OMX_HANDLETYPE *hPipe,OMX_STRING szURI)
{


	if (m_fGetPipe == NULL)
		return OMX_ErrorComponentNotFound;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (!m_bInit)
	{
		errType = OMX_Init ();
		if (errType != OMX_ErrorNone)
			return errType;
	}

	errType = m_fGetPipe (hPipe, szURI);
	if (errType != OMX_ErrorNone)
		VOLOGE ("The result of OMX_GetContentPipe is 0X%08X", errType);

	return errType;
}

OMX_ERRORTYPE voCOMXCoreLoader::OMX_GetComponentsOfRole (OMX_STRING role, OMX_U32 *pNumComps, OMX_U8 **compNames)
{


	if (m_fGetCompRole == NULL)
		return OMX_ErrorComponentNotFound;

	OMX_ERRORTYPE errType = OMX_Init ();
	if (errType != OMX_ErrorNone)
		return errType;

	errType = m_fGetCompRole (role, pNumComps, compNames);
	if (errType != OMX_ErrorNone)
		VOLOGE ("The result of m_fGetCompRole is 0X%08X.The role is %s", errType, role);

	return errType;
}

OMX_ERRORTYPE voCOMXCoreLoader::OMX_GetRolesOfComponent (OMX_STRING compName, OMX_U32 *pNumRoles, OMX_U8 **roles)
{


	if (m_fGetRoleComp == NULL)
		return OMX_ErrorComponentNotFound;

	OMX_ERRORTYPE errType = OMX_Init ();
	if (errType != OMX_ErrorNone)
		return errType;

	errType = m_fGetRoleComp (compName, pNumRoles, roles);
	if (errType != OMX_ErrorNone)
		VOLOGE ("The result of m_fGetCompRole is 0X%08X.The comp is %s", errType, compName);

	return errType;
}

