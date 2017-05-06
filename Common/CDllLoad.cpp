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
#include "voOSFunc.h"
#include "CDllLoad.h"
#include "voHalInfo.h"

#define LOG_TAG "CDllLoad"
#include "voLog.h"

#ifdef _IOS
#include "voLoadLibControl.h"
#endif
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CDllLoad::CDllLoad(void)
  : m_pAPIEntry (NULL)
  , m_nCPUNum (1)
  , m_hDll (NULL)
  , m_pLibOP (NULL)
  , m_pWorkPath (NULL)
{
    vostrcpy (m_szDllFile, _T(""));
    vostrcpy (m_szAPIName, _T(""));
    vostrcpy (m_szVer, _T(""));
#if defined (_IOS) || defined (_MAC_OS)
    m_nCPUNum = voOS_GetCPUNum();
#elif defined (_LINUX_ANDROID)
    VO_CPU_Info info;
    get_cpu_info(&info);
    if (info.mType == CPU_FAMILY_ARM) 
    {
        if ((info.mFeatures & CPU_ARM_FEATURE_NEON) != 0) 
	{
	    vostrcpy (m_szVer, _T("_v7.so"));
	    VOLOGI("CPU supports NEON!" );
        }
    }

    m_nCPUNum = info.mCount;
#endif
}

CDllLoad::~CDllLoad ()
{
	FreeLib ();
}

void CDllLoad::SetCPUVersion (int nVer)
{
	if (nVer == 7)
		vostrcpy (m_szVer, _T("_v7.so"));
}

VO_U32 CDllLoad::LoadLib (VO_HANDLE hInst)
{
#ifdef _IOS
    voGetModuleRealAPI pFunction = (voGetModuleRealAPI)voGetModuleAdapterFunc(m_szAPIName);
    
    if (pFunction) {
        m_pAPIEntry = pFunction();
    }
    
    if (m_pAPIEntry == NULL)
    {
        VOLOGE ("It could not get the function %s address from lib %s!", m_szAPIName, m_szDllFile);
        return 0;
    }
    
    return 1;
#endif
    
	if (m_hDll != NULL)
		FreeLib ();

#ifdef _METRO
	TCHAR	szDll[1024];
	memset (szDll, 0, 1024 * sizeof(TCHAR));
	if (m_hDll == NULL)
	{
		_tcscat (szDll, m_szDllFile);
		m_hDll = LoadPackagedLibrary(szDll , 0);

		if (m_hDll == NULL)
		{
			_tcscat (szDll, _T(".Dll"));
			m_hDll = LoadPackagedLibrary (szDll , 0);
		}
	}

#elif defined _WIN32
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
	// added by gtxia 
	if(!m_hDll)
	{
		m_hDll = dlopen(libname, RTLD_NOW);
		if (m_hDll == NULL) 
			printf("load: %s, dlerror: %s\n", libname, dlerror());
	}
#elif defined _MAC_OS
    
    VO_TCHAR libname[256] = {0};
    VO_TCHAR PathAndName [1024] = {0};
    
    if (0 != strncasecmp (m_szDllFile, "lib", 3))
    {
        vostrcat(libname, "lib");
    }
    vostrcat(libname, m_szDllFile);
    if (vostrstr(libname, ".dylib") == 0)
    {
        vostrcat(libname, ".dylib");
    }
    
	bool bAPP = false;
	VOLOGI ("dllfile: %s, workpath: %s!", libname, m_pWorkPath);
    
	if (m_hDll == NULL && m_pWorkPath != NULL && strcmp(m_pWorkPath, ""))
	{
		bAPP = true;
        
		vostrcpy(PathAndName, m_pWorkPath);
        
        if (m_pWorkPath[strlen(m_pWorkPath) - 1] != '/') {
            vostrcat(PathAndName, "/");
        }
        
		vostrcat(PathAndName, libname);
        
		m_hDll = dlopen (PathAndName, RTLD_NOW);
	}
    
	VOLOGI ("bAPP: %d", bAPP);
    
    if (!bAPP && m_hDll == NULL)
	{        
        voOS_GetAppFolder(PathAndName, sizeof(PathAndName));
        vostrcat(PathAndName, libname);
        m_hDll = dlopen (PathAndName, RTLD_NOW);
        if (NULL == m_hDll)
        {
            voOS_GetAppFolder(PathAndName, sizeof(PathAndName));
            vostrcat(PathAndName, "lib/");
            vostrcat(PathAndName, libname);
            m_hDll = dlopen (PathAndName, RTLD_NOW);
        }
        
        if (NULL == m_hDll)
        {
            voOS_GetPluginModuleFolder(PathAndName, sizeof(PathAndName));
            vostrcat(PathAndName, libname);
            m_hDll = dlopen (PathAndName, RTLD_NOW);
        }
        
        if (NULL == m_hDll)
        {
            vostrcpy(PathAndName, "/Library/Internet Plug-Ins/");
            vostrcat(PathAndName, libname);
            m_hDll = dlopen (PathAndName, RTLD_NOW);
        }
        
        if (NULL == m_hDll)
        {
            m_hDll = dlopen(libname, RTLD_NOW);
        }
        
        if (NULL == m_hDll)
        {
            if (strrchr(libname, '/') == NULL)
                vostrcpy(PathAndName, "/usr/lib/");
            
            vostrcat (PathAndName, libname);
            m_hDll = dlopen (PathAndName, RTLD_NOW);
        }
	}
    
	if (m_hDll)
	{
		VOLOGI("load: %s ok", PathAndName);
	}
    else {
        VOLOGI("load: %s error", libname);
    }

#elif defined _LINUX_ANDROID
	VO_TCHAR libname[256];
	vostrcpy(libname, m_szDllFile);
	if (voOS_EnableDebugMode(0))
	{
		vostrcpy(m_szDllFile, "/data/local/tmp/OnStreamPlayer/lib/");
		if (strstr(libname, "lib") == 0)
		{
			strcat(m_szDllFile, "lib");
		}
		vostrcat(m_szDllFile, libname);
		if (strstr(libname, ".so") == 0)
		{
			strcat(m_szDllFile, ".so");
		}
		m_hDll = dlopen (m_szDllFile, RTLD_NOW);
	}

	bool bAPK = false;
	VOLOGI ("dllfile: %s, workpath: %s!", m_szDllFile, m_pWorkPath);
	if (m_hDll == NULL && m_pWorkPath != NULL && strcmp(m_pWorkPath, ""))
	{
		bAPK = true;

		vostrcpy (m_szDllFile, m_pWorkPath);
		if (strstr(libname, "lib") == 0)
		{
			strcat(m_szDllFile, "lib");
		}
		vostrcat (m_szDllFile, libname);
		if (strstr(libname, ".so") == 0)
		{
			strcat(m_szDllFile, ".so");
		}
		m_hDll = LoadVerLib (m_szDllFile);
		if (m_hDll == NULL)
			m_hDll = dlopen (m_szDllFile, RTLD_NOW);
	}

	VOLOGI ("bAPK: %d", bAPK);
	if (!bAPK && m_hDll == NULL)
	{
		if(strrchr(libname, '/') == NULL)
			vostrcpy(m_szDllFile, "/system/lib/");
		vostrcat (m_szDllFile, libname);
		m_hDll = dlopen (m_szDllFile, RTLD_NOW);
		if (m_hDll == NULL)
		{
			if(strrchr(libname, '/') == NULL)
				vostrcpy(m_szDllFile, "/system/lib/lib");
			vostrcat(m_szDllFile, libname);
			m_hDll = dlopen (m_szDllFile, RTLD_NOW);
		}
	}
#elif defined __SYMBIAN32__
	TBuf8<256> strAddr;   
	strAddr.SetLength(256);
	strAddr.Copy((TUint8*)m_szDllFile);	
	TFileName szDllFileName;
	szDllFileName.Copy(strAddr); 

	TInt nRC = m_cLibrary.Load(szDllFileName);
	if (nRC == KErrNone)
		m_hDll = &m_cLibrary;
#endif // _WIN32

#ifdef _LINUX_ANDROID
	//VOLOGE ("voErrorLibrary %s!", voErrorLibrary());
#endif // _LINUX

	if (m_hDll == NULL)
	{
#ifdef LINUX
		VOLOGW ("Load module %s was failed!", m_szDllFile);
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
#elif defined _MAC_OS
		m_pAPIEntry = dlsym (m_hDll, m_szAPIName);
#elif defined __SYMBIAN32__
	m_pAPIEntry = (VO_PTR) m_cLibrary.Lookup(1);
#endif // _WIN32
	}

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
	if (m_hDll != NULL)
#ifdef _WIN32
		FreeLibrary (m_hDll);
#elif defined _LINUX
		dlclose (m_hDll);
#elif defined __SYMBIAN32__
		m_cLibrary.Close ();
#elif defined _MAC_OS
    if (m_hDll != NULL)
		dlclose (m_hDll);
#endif // _WIN32

	if (m_hDll != NULL) {
		VOLOGI ("Free module %s!", m_szDllFile);
	}
	m_hDll = NULL;

	return VO_ERR_NONE;
}

VO_PTR CDllLoad::LoadVerLib (VO_TCHAR * pFile)
{
	VO_PTR hDll = NULL;
#ifdef _LINUX_ANDROID
	if (strlen (m_szVer) <= 0)
		return NULL;

	char szLibFile[256];
	strcpy (szLibFile, pFile);
	char * pExt = strstr (szLibFile, ".so");
	if (pExt == NULL)
		return NULL;
	*pExt = 0;
	strcat (szLibFile, m_szVer);
	
//	VOLOGI ("The ver lib is %s", szLibFile);

	hDll = dlopen (szLibFile, RTLD_NOW);
	if (hDll != NULL)
		strcpy (m_szDllFile, szLibFile);

#endif // _LINUX_ANDROID
	return hDll;
}

