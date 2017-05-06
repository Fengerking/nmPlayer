
#include "voLog.h"
#include "voLogDll.h"
#include "voIndex.h"

#ifdef _WIN32
#include "windows.h"
#endif // _WIN32

#ifdef _LINUX
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#endif // _LINUX

#ifdef _LINUX_ANDROID
#include <pthread.h>
#if !defined __VO_NDK__
#include <utils/Log.h>
#else
#include <android/log.h>
#endif
#endif // _LINUX_ANDROID

#if defined _IOS || defined _MAC_OS
#include <dlfcn.h>
#else
#include "malloc.h"
#endif

#ifdef _LINUX
#ifndef HAVE_SNPRINTF
int snprintf ( char *str, size_t n, const char *format, ... );
#endif
#ifndef HAVE_VSNPRINTF
int vsnprintf (char *str, size_t size, const char *format, va_list ap);
#endif
#endif

typedef VO_U32 (VO_API * VOLOGDLLINIT) ( VO_U32 uMoudleID,  VO_CHAR* pCfgPath, VO_S32* pMaxLogLevel);

typedef VO_U32 (VO_API * VOLOGGETMAXLEVEL) ( VO_U32 uMoudleID, VO_S32* pMaxLogLevel);

typedef VO_U32 (VO_API * VOLOGPRINT) ( int nLevel, VO_CHAR* pLogText);

typedef VO_U32 (VO_API * VOLOGDLLUNINIT) ();


/**
* The volog info
*/
typedef struct
{
	VO_HANDLE				hDll;				/*!< Volog system library handle  */
	VO_S32					nMaxLogLevel;		/*!< Volog max level from config file  */
	VO_S32					nLoadCounts;
	VOLOGDLLINIT			fInit;				/*!< Init volog system  */
	VOLOGGETMAXLEVEL		fGetLogMaxLevel;	/*!< Get module corresponding log max level from config file*/
	VOLOGPRINT				fLogPrint;			/*!< Log print function  */
	VOLOGDLLUNINIT			fUninit;			/*!< Uninit volog system */
} VO_LOG_INFO;

#define LOGINFORENAME(ID) g_LogInfo##ID
#define g_LogInfo(ID) LOGINFORENAME(ID)
static VO_LOG_INFO		 g_LogInfo(_VOMODULEID);

#define voLoadLibRENAME(name) voLoadLib##name
#define voLoadLib(name) voLoadLibRENAME(name)
#define voGetAddressRENAME(name) voGetAddress##name
#define voGetAddress(name) voGetAddressRENAME(name)
#define voFreeLibRENAME(name) voFreeLib##name
#define voFreeLib(name) voFreeLibRENAME(name)
#define voLoadModuleRENAME(name) voLoadModule##name
#define voLoadModule(name) voLoadModuleRENAME(name)

VO_PTR voLoadLib(_VOMODULEID)(VO_TCHAR* pLibName);

VO_PTR voGetAddress(_VOMODULEID)(VO_PTR hLib, VO_TCHAR* pFuncName);

VO_S32 voFreeLib(_VOMODULEID)(VO_PTR hLib);

int voLoadModule(_VOMODULEID)(VO_TCHAR* pWorkPath);


int vologInit(_VOMODULEID)( VO_TCHAR* pWorkPath )
{
#ifndef _WIN32
	__D("@#@#log vologInit(0x%08x) ===>",_VOMODULEID );
#endif


	int nRC = VO_ERR_FAILED;
	/*config file path*/
	VO_CHAR cfgPath[256] = {0};
#if defined (_LINUX)
	vostrcpy(cfgPath, "/data/local/tmp/");
#elif defined (_WIN32)
	WideCharToMultiByte(CP_ACP, 0, pWorkPath, -1, cfgPath, 256, NULL, NULL);
#else
	;//if path is null, the path will be get in volog module
#endif

	if(NULL != g_LogInfo(_VOMODULEID).hDll )
		return VO_ERR_NONE;

#ifndef _WIN32
	__D("@#@#log pWorkPath: %s", pWorkPath);
#endif

	nRC = voLoadModule(_VOMODULEID) (pWorkPath);
	if(VO_ERR_NONE != nRC)
	{
		voFreeLib(_VOMODULEID)(g_LogInfo(_VOMODULEID).hDll);
		g_LogInfo(_VOMODULEID).hDll = NULL;

		return nRC;
	}
	nRC = g_LogInfo(_VOMODULEID).fInit(_VOMODULEID, cfgPath,&g_LogInfo(_VOMODULEID).nMaxLogLevel);
	if(VO_ERR_NONE != nRC)
	{
		voFreeLib(_VOMODULEID)(g_LogInfo(_VOMODULEID).hDll);
		g_LogInfo(_VOMODULEID).hDll = NULL;

		return nRC;
	}
	g_LogInfo(_VOMODULEID).nLoadCounts ++;
#ifndef _WIN32
	__D("@#@#log maxlevel: %d", (int)g_LogInfo(_VOMODULEID).nMaxLogLevel);
#endif
#if 0
	VOLOGE("VOLOGE  You will see this log if maxlevel >= 0");
	VOLOGW("VOLOGW  You will see this log if maxlevel >= 1");
	VOLOGI("VOLOGI  You will see this log if maxlevel >= 2");
	VOLOGS("VOLOGS  You will see this log if maxlevel >= 3");
	VOLOGR("VOLOGR  You will see this log if maxlevel >= 4");
#endif

	return nRC;
}

int vologUninit(_VOMODULEID)()
{
#ifndef _WIN32
	__D("@#@#log vologUninit(0x%08x) ===>", _VOMODULEID);
#endif


	g_LogInfo(_VOMODULEID).nLoadCounts --;
	if( g_LogInfo(_VOMODULEID).nLoadCounts <= 0)
		g_LogInfo(_VOMODULEID).nLoadCounts = 0;
	if (g_LogInfo(_VOMODULEID).nLoadCounts == 0 && g_LogInfo(_VOMODULEID).hDll != NULL)
	{

		g_LogInfo(_VOMODULEID).fUninit();

#ifndef _WIN32
		__D("@#@#log voFreeLib(0x%08x)(%p)", _VOMODULEID,g_LogInfo(_VOMODULEID).hDll);
#endif	
		voFreeLib(_VOMODULEID) (g_LogInfo(_VOMODULEID).hDll);
		g_LogInfo(_VOMODULEID).hDll = NULL;
	}
#ifndef _WIN32
	__D("@#@#log g_LogInfo(0x%08x): %p. InitCounts:%d", _VOMODULEID, &g_LogInfo(_VOMODULEID), (int)g_LogInfo(_VOMODULEID).nLoadCounts);
#endif
	return 0;
}



 VO_PTR voLoadLib(_VOMODULEID)(VO_TCHAR* pLibName)
{
	VO_PTR pDll = NULL;
#if defined(_LINUX) || defined(_MAC_OS)
	pDll = dlopen(pLibName, RTLD_NOW);
	if(pDll == NULL)
	{
		//__D("can not load pLibName = %s  the reason is %s\n", pLibName, dlerror());
	}	
#elif defined(_WIN32)
	pDll = LoadLibrary (pLibName);
#endif	
	return pDll;
}

VO_PTR voGetAddress(_VOMODULEID)(VO_PTR hLib, VO_TCHAR* pFuncName)
{
	VO_PTR pFunc = NULL;
	if(hLib == NULL || pFuncName == NULL)
	{
		//__D("hLib = %p\n", hLib);
		return NULL;
	}
	if(vostrlen(pFuncName) > 0)
	{
#if defined(_LINUX) || defined(_MAC_OS)
		//__D("%s",pFuncName);

		pFunc = dlsym (hLib, pFuncName);
		if(pFunc == NULL)
		{
			__D("can not find the pFuncName  %s the reason is %s\n", pFuncName, dlerror());
		}
#elif defined(_WIN32)
		pFunc = GetProcAddress((HMODULE)hLib, (LPCSTR)pFuncName);
		if(pFunc == NULL)
		{
			//__D("can not find the pFuncName  %s\n", pFuncName);
		}
#endif
	}
	else
	{
#if !defined(_IOS)
		//__D("the pFuncName is zero\n");
#endif
	}
	return pFunc;
}

VO_S32 voFreeLib(_VOMODULEID)(VO_PTR hLib)
{

	VO_S32 sOk = -1;
	if ( NULL != hLib )
	{
#ifdef _WIN32
		sOk = FreeLibrary ((HMODULE)hLib);
#elif defined _LINUX || defined(_MAC_OS)
		sOk = dlclose(hLib);
#endif // _WIN32
	} 
	return sOk;
}

int voLoadModule(_VOMODULEID)(VO_TCHAR* pWorkPath)
{
	VO_TCHAR	szDll[1024];
	VO_TCHAR	pLast = '\0';
#ifndef _IOS 
	if(NULL == pWorkPath)
		return VO_ERR_FAILED;
#endif

#ifdef _WIN32
	vostrcpy(szDll, pWorkPath);
	pLast = szDll[vostrlen(pWorkPath) -1];
	if(pLast != _T('\\'))
		vostrcat(szDll, _T("\\"));
	vostrcat (szDll, 	_T("voLogSys.Dll"));
	g_LogInfo(_VOMODULEID).hDll = voLoadLib(_VOMODULEID) (szDll);
#elif defined _IOS
	g_LogInfo(_VOMODULEID).hDll = 1;
#elif defined _MAC_OS
	vostrcpy(szDll, pWorkPath);
	pLast = szDll[vostrlen(pWorkPath) -1];
	if(pLast != '/')
		strcat(szDll, "/");
	vostrcat (szDll, 	"libvoLogSys.dylib");
	g_LogInfo(_VOMODULEID).hDll = voLoadLib(_VOMODULEID) (szDll);
#else
	vostrcpy(szDll, pWorkPath);
	pLast = szDll[vostrlen(pWorkPath) -1];
	if(pLast != '/')
		vostrcat(szDll, "/");

	vostrcat (szDll, "libvoLogSys.so");
#ifndef _WIN32
	__D("@#@#log szDll: %s", szDll);
#endif

	g_LogInfo(_VOMODULEID).hDll = voLoadLib(_VOMODULEID) (szDll);
#endif // _WIN32
	if (NULL == g_LogInfo(_VOMODULEID).hDll )
		return VO_ERR_FAILED;

#ifdef _IOS
	g_LogInfo(_VOMODULEID).fInit			= voLogDllInit;
	g_LogInfo(_VOMODULEID).fGetLogMaxLevel  = voLogDllGetMaxLevel;
	g_LogInfo(_VOMODULEID).fLogPrint		= voLogDllLogPrint;
	g_LogInfo(_VOMODULEID).fUninit			= voLogDllUninit;
#else
	g_LogInfo(_VOMODULEID).fInit			= (VOLOGDLLINIT)voGetAddress(_VOMODULEID) (g_LogInfo(_VOMODULEID).hDll, (VO_TCHAR *)"voLogDllInit");
	g_LogInfo(_VOMODULEID).fGetLogMaxLevel  = (VOLOGGETMAXLEVEL)voGetAddress(_VOMODULEID) (g_LogInfo(_VOMODULEID).hDll, (VO_TCHAR *)"voLogDllGetMaxLevel");
	g_LogInfo(_VOMODULEID).fLogPrint		= (VOLOGPRINT)voGetAddress(_VOMODULEID) (g_LogInfo(_VOMODULEID).hDll, (VO_TCHAR *)"voLogDllLogPrint");
	g_LogInfo(_VOMODULEID).fUninit			= (VOLOGDLLUNINIT)voGetAddress(_VOMODULEID) (g_LogInfo(_VOMODULEID).hDll, (VO_TCHAR *)"voLogDllUninit");
#endif
#ifndef _WIN32
	__D("@#@#log voLoadModule(0x%08x)(%p, %p, %p, %p, %p)", _VOMODULEID,g_LogInfo(_VOMODULEID).hDll
		, g_LogInfo(_VOMODULEID).fInit, g_LogInfo(_VOMODULEID).fGetLogMaxLevel, g_LogInfo(_VOMODULEID).fLogPrint,g_LogInfo(_VOMODULEID).fUninit	);
#endif
	if( g_LogInfo(_VOMODULEID).fInit  
		&& g_LogInfo(_VOMODULEID).fGetLogMaxLevel 
		&& g_LogInfo(_VOMODULEID).fLogPrint
		&& g_LogInfo(_VOMODULEID).fUninit)
		return VO_ERR_NONE;
	else
		return VO_ERR_FAILED;
}


void  vologPrint(_VOMODULEID)(VO_S32 level, const char *__fmt,...)
{
#if defined _IOS || defined _MAC_OS
#define LOG_LENGTH 1024
#else
#define LOG_LENGTH 2048
#endif

    
	if( NULL != g_LogInfo(_VOMODULEID).hDll && level <= g_LogInfo(_VOMODULEID).nMaxLogLevel)
	{
		char		szvoLog[LOG_LENGTH] = {0};
		char 		szVOLOGTempString[LOG_LENGTH] = {0};

		va_list args; 
		va_start(args,__fmt); 
		vsnprintf (szVOLOGTempString , LOG_LENGTH, __fmt , args );
		va_end(args); 
		voLogSprintf(szvoLog, LOG_LENGTH,  "%s\n", szVOLOGTempString);
		g_LogInfo(_VOMODULEID).fLogPrint( level, szvoLog);
	}

}
