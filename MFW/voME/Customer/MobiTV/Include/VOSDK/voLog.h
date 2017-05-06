	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voLog.h

	Contains:	voLog define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-10		JBF			Create file

*******************************************************************************/

#ifndef __voLog_H__
#define __voLog_H__

#include "voString.h"

#ifdef _LINUX_ANDROID
#include <pthread.h>
#include <utils/Log.h>
#endif // _LINUX_ANDROID

#ifdef _WIN32
#include "windows.h"

#pragma warning (disable : 4996)
#pragma warning (disable : 4003)
#endif // _WIN32

/********************************************************************************
There were four types logs
1. VOLOGE (...)		Error.	 This type log will always show in debug window.
2. VOLOGW (...)		Warning. This type log will show if define _VOLOG_WARNING
3. VOLOGI (...)		Info	 This type log will show if define _VOLOG_INFO
4. VOLOGS (...)		Status.	 This type log will show if define _VOLOG_STATUS
5. VOLOGR (...)		Run.	 This type log will show if define _VOLOG_RUN.
							 This should be define in run loop. Performance.
6. VOLOGF (...)		Status.	 This type log will show if define _VOLOG_FUNC

********************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct CVOLOGFunc
{
public:
	CVOLOGFunc (char * pText)
	{
		m_pLogText = (char *)malloc (strlen(pText) + 12);
		strcpy (m_pLogText, pText);
	}

	~CVOLOGFunc (void)
	{
		strcat (m_pLogText, "    Exit\r\n");
#ifdef _WIN32
		VO_TCHAR	wzLog[256];
		MultiByteToWideChar (CP_ACP, 0, m_pLogText, -1, wzLog, sizeof (wzLog));
		OutputDebugString (wzLog);
#elif defined _LINUX_ANDROID
		LOGE (m_pLogText);
#endif // _WIN32
		free (m_pLogText);
	}

protected:
	char *		m_pLogText;
};

#ifdef _WIN32

#define VOLOGE(fmt, ...) \
{ \
	char		szLog[256]; \
	VO_TCHAR	wzLog[256]; \
	const char * pName = strrchr (__FILE__, '\\') + 1; \
	sprintf(szLog, "@@@VOLOG Error THD %08X:   %s  %s  %d    " fmt "\r\n", GetCurrentThreadId(), pName, __FUNCTION__, __LINE__, __VA_ARGS__); \
	MultiByteToWideChar (CP_ACP, 0, szLog, -1, wzLog, sizeof (wzLog)); \
	OutputDebugString (wzLog); \
}

#ifdef _VOLOG_WARNING
#define VOLOGW(fmt, ...) \
{ \
	char		szLog[256]; \
	VO_TCHAR	wzLog[256]; \
	const char * pName = strrchr (__FILE__, '\\') + 1; \
	sprintf(szLog, "@@@VOLOG Warning THD %08X: %s  %s  %d    " fmt "\r\n", GetCurrentThreadId(), pName, __FUNCTION__, __LINE__, __VA_ARGS__); \
	MultiByteToWideChar (CP_ACP, 0, szLog, -1, wzLog, sizeof (wzLog)); \
	OutputDebugString (wzLog); \
}
#else
#define VOLOGW(fmt, ...)
#endif // _VOLOG_WARNING

#ifdef _VOLOG_INFO
#define VOLOGI(fmt, ...) \
{ \
	char		szLog[256]; \
	VO_TCHAR	wzLog[256]; \
	const char * pName = strrchr (__FILE__, '\\') + 1; \
	sprintf(szLog, "@@@VOLOG Info THD %08X:    %s  %s  %d    " fmt "\r\n", GetCurrentThreadId(), pName, __FUNCTION__, __LINE__, __VA_ARGS__); \
	MultiByteToWideChar (CP_ACP, 0, szLog, -1, wzLog, sizeof (wzLog)); \
	OutputDebugString (wzLog); \
}
#else
#define VOLOGI(fmt, ...)
#endif // _VOLOG_INFO

#ifdef _VOLOG_STATUS
#define VOLOGS(fmt, ...) \
{ \
	char		szLog[256]; \
	VO_TCHAR	wzLog[256]; \
	const char * pName = strrchr (__FILE__, '\\') + 1; \
	sprintf(szLog, "@@@VOLOG Status THD %08X:  %s  %s  %d    " fmt "\r\n", GetCurrentThreadId(), pName, __FUNCTION__, __LINE__, __VA_ARGS__); \
	MultiByteToWideChar (CP_ACP, 0, szLog, -1, wzLog, sizeof (wzLog)); \
	OutputDebugString (wzLog); \
}
#else
#define VOLOGS(fmt, ...)
#endif // _VOLOG_STATUS

#ifdef _VOLOG_RUN
#define VOLOGR(fmt, ...) \
{ \
	char		szLog[256]; \
	VO_TCHAR	wzLog[256]; \
	const char * pName = strrchr (__FILE__, '\\') + 1; \
	sprintf(szLog, "@@@VOLOG Run THD %08X  :   %s  %s  %d    " fmt "\r\n", GetCurrentThreadId(), pName, __FUNCTION__, __LINE__, __VA_ARGS__); \
	MultiByteToWideChar (CP_ACP, 0, szLog, -1, wzLog, sizeof (wzLog)); \
	OutputDebugString (wzLog); \
}
#else
#define VOLOGR(fmt, ...)
#endif // _VOLOG_RUN

#ifdef _VOLOG_FUNC
#define VOLOGF(fmt, ...) \
	char *		szVOLog = (char *)malloc (256); \
	VO_TCHAR *	wzVOLog = (VO_TCHAR *)malloc (512); \
	memset (wzVOLog, 0, 512); \
	const char * pVOLOGName = strrchr (__FILE__, '\\') + 1; \
	sprintf(szVOLog, "@@@VOLOG Func THD %08X :   %s  %s  %d  %d  " fmt, GetCurrentThreadId(), pVOLOGName, __FUNCTION__, __LINE__, GetTickCount(), __VA_ARGS__); \
	CVOLOGFunc logFunc (szVOLog); \
	MultiByteToWideChar (CP_ACP, 0, szVOLog, -1, wzVOLog, 256); \
	_tcscat (wzVOLog, _T("\r\n")); \
	OutputDebugString (wzVOLog); \
	free (szVOLog); \
	free (wzVOLog);
#else
#define VOLOGF(fmt, ...)
#endif // _VOLOG_RUN

#elif defined _LINUX_ANDROID

#ifdef _VOLOG_ERROR
#define VOLOGE(fmt, args...) \
{ \
	const char * pName = strrchr (__FILE__, '/') + 1; \
	LOGE ("@@@VOLOG Error THD %08X:   %s  %s  %d    " fmt "\n", (int)pthread_self (), pName, __FUNCTION__, __LINE__, ## args); \
}
#else
#define VOLOGE(fmt, ...)
#endif // _VOLOG_ERROR

#ifdef _VOLOG_WARNING
#define VOLOGW(fmt, args...) \
{ \
	const char * pName = strrchr (__FILE__, '/') + 1; \
	LOGE ("@@@VOLOG Warning THD %08X: %s  %s  %d    " fmt "\n", (int)pthread_self (), pName, __FUNCTION__, __LINE__, ## args); \
}
#else
#define VOLOGW(fmt, ...)
#endif // _VOLOG_WARNING

#ifdef _VOLOG_INFO
#define VOLOGI(fmt, args...) \
{ \
	const char * pName = strrchr (__FILE__, '/') + 1; \
	LOGE ("@@@VOLOG Info THD %08X:    %s  %s  %d    " fmt "\n", (int)pthread_self (), pName, __FUNCTION__, __LINE__, ## args); \
}
#else
#define VOLOGI(fmt, ...)
#endif // _VOLOG_INFO

#ifdef _VOLOG_STATUS
#define VOLOGS(fmt, args...) \
{ \
	const char * pName = strrchr (__FILE__, '/') + 1; \
	LOGE ("@@@VOLOG Status THD %08X:  %s  %s  %d    " fmt "\n", (int)pthread_self (), pName, __FUNCTION__, __LINE__, ## args); \
}
#else
#define VOLOGS(fmt, ...)
#endif // _VOLOG_STATUS

#ifdef _VOLOG_RUN
#define VOLOGR(fmt, args...) \
{ \
	const char * pName = strrchr (__FILE__, '/') + 1; \
	LOGE ("@@@VOLOG Run THD %08X:     %s  %s  %d    " fmt "\n", (int)pthread_self (), pName, __FUNCTION__, __LINE__, ## args); \
}
#else
#define VOLOGR(fmt, ...)
#endif // _VOLOG_RUN

#ifdef _VOLOG_FUNC
#define VOLOGF(fmt, args...) \
	char * szVOLog = (char *)malloc (256); \
	struct timeval tmvologVal; \
	gettimeofday(&tmvologVal, NULL); \
	int nVOLOGTime = tmvologVal.tv_sec*1000 + tmvologVal.tv_usec/1000; \
	const char * pVOLOGName = strrchr (__FILE__, '/') + 1; \
	sprintf (szVOLog, "@@@VOLOG Func THD %08X     %s  %s  %d  %d  " fmt, (int)pthread_self (), pVOLOGName, __FUNCTION__, __LINE__, nVOLOGTime, ## args); \
	CVOLOGFunc logFunc (szVOLog); \
	strcat (szVOLog, "\n"); \
	LOGE (szVOLog); \
	free (szVOLog);
#else
#define VOLOGF(fmt, ...)
#endif // _VOLOG_FUNC

#else

#define VOLOGE(fmt, ...)
#define VOLOGW(fmt, ...)
#define VOLOGI(fmt, ...)
#define VOLOGS(fmt, ...)
#define VOLOGR(fmt, ...)
#define VOLOGF(fmt, ...)

#endif // _WIN32

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __voLog_H__
