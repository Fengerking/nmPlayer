	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voOMXOSFun.cpp

	Contains:	component api cpp file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#endif // _WIN32


#if defined _LINUX
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#endif // LINUX

#include "voOMXOSFun.h"
#include "voOMXMemory.h"

#include "voLog.h"

void voOMXOS_Sleep (OMX_U32 nTime)
{
#ifdef _WIN32
	Sleep (nTime);
#elif defined _LINUX
	usleep (1000 * nTime);
#endif // _WIN32
}

OMX_U32 voOMXOS_GetSysTime (void)
{
	OMX_U32	nTime = 0;

#ifdef _WIN32
	nTime = GetTickCount ();

	//East try QueryPerformanceXXX and it is better than GetTickCount
	//But only test in PC, so block these codes
/*	LARGE_INTEGER li;
	::QueryPerformanceFrequency(&li);
	OMX_S64 llFreq = li.QuadPart;

	::QueryPerformanceCounter(&li);
	nTime = li.QuadPart * 1000 / llFreq;
*/
#elif defined _LINUX
	timespec tv;
	clock_gettime(CLOCK_MONOTONIC, &tv);

	static timespec stv = {0, 0};
	if ((0 == stv.tv_sec) && (0 == stv.tv_nsec))
	{
		stv.tv_sec = tv.tv_sec;
		stv.tv_nsec = tv.tv_nsec;
	}

	nTime = (VO_U32)((tv.tv_sec - stv.tv_sec) * 1000 + (tv.tv_nsec - stv.tv_nsec) / 1000000);
/*
	struct timeval tval;
	gettimeofday(&tval, NULL);
	nTime = tval.tv_sec*1000 + tval.tv_usec/1000;
*/
#endif // _WIN32

	return nTime;
}

OMX_U32 voOMXOS_GetThreadTime (OMX_PTR hThread)
{
	OMX_U32 nTime = 0;

#ifdef _WIN32
	if(hThread == NULL)
		hThread = GetCurrentThread();

	if(!hThread)
		return 0;

	FILETIME ftCreationTime;
	FILETIME ftExitTime;
	FILETIME ftKernelTime;
	FILETIME ftUserTime;

	BOOL bRC = GetThreadTimes(hThread, &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime);
	if (!bRC)
		return 0;

	LONGLONG llKernelTime = ftKernelTime.dwHighDateTime;
	llKernelTime = llKernelTime << 32;
	llKernelTime += ftKernelTime.dwLowDateTime;

	LONGLONG llUserTime = ftUserTime.dwHighDateTime;
	llUserTime = llUserTime << 32;
	llUserTime += ftUserTime.dwLowDateTime;

	nTime = int((llKernelTime + llUserTime) / 10000);
#elif defined _LINUX
	nTime = voOMXOS_GetSysTime ();
#endif // _WIN32

	return nTime;
}

void voOMXOS_Printf (OMX_STRING pString)
{
#ifdef _WIN32
	TCHAR szDebug[256];
	memset (szDebug, 0, 256 * sizeof (TCHAR));
	MultiByteToWideChar (CP_ACP, 0, pString, -1, szDebug, 256);
	OutputDebugString (szDebug);
#elif defined _LINUX
	printf (pString);
#endif // _WIN32

}

OMX_U32 voOMXOS_GetAppFolder (OMX_PTR hModule, OMX_STRING pFolder, OMX_U32 nSize)
{
#ifdef _WIN32
	TCHAR * pAppFolder = (TCHAR *)pFolder;

	GetModuleFileName (NULL, pAppFolder, nSize);

   TCHAR * pPos = _tcsrchr (pAppFolder, _T('/'));
	if (pPos == NULL)
		pPos = _tcsrchr (pAppFolder, _T('\\'));
    OMX_S32 nPos = pPos - pAppFolder;
    pAppFolder[nPos+1] = _T('\0');
#elif defined _LINUX
	OMX_S32 r = readlink("/proc/self/exe", pFolder, nSize);
	if (r < 0 || r >= (OMX_S32)nSize)
		return r;
	pFolder[r] = '\0';

    char * pPos = strrchr (pFolder, ('/'));
	if (pPos == NULL)
		pPos = strrchr (pFolder, ('\\'));
    OMX_S32 nPos = pPos - pFolder;
    pFolder[nPos+1] = ('\0');
#endif // _LINUX

	return 0;
}
