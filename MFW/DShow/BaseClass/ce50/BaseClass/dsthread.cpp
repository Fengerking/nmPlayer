//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
#if defined(CUSTOM_THREADPRIORITY)

// The module is for changing thread priority from registry
// daihu - Jan 7 2001
static DWORD s_dwHighestThreadPriority = -1;
static DWORD s_dwAboveNormalThreadPriority  = -1;
static DWORD s_dwNormalThreadPriority  = -1;
static DWORD s_dwBelowNormalThreadPriority  = -1;

HANDLE
WINAPI
_internal_CreateThread 
(
    LPSECURITY_ATTRIBUTES lpsa,
    DWORD   cbStack,
    LPTHREAD_START_ROUTINE lpStartAddr,
    LPVOID lpvThreadParam,
    DWORD fdwCreate,
    LPDWORD lpIDThread
    )
{
	HANDLE hThread = CreateThread (lpsa, cbStack, lpStartAddr, lpvThreadParam, fdwCreate, lpIDThread);

	if (NULL != hThread)
	{
		_internal_SetThreadPriority (hThread, THREAD_PRIORITY_NORMAL);
	}

	return hThread;
}

int _internal_GetThreadPriority
(
	HANDLE hThread
)
{
	int nPriority = CeGetThreadPriority (hThread);

	// Map CE thread priority back to win32 thread priority
	if (DWORD(nPriority) == s_dwHighestThreadPriority)
	{
		nPriority = THREAD_PRIORITY_HIGHEST;
	}
	else if (DWORD(nPriority) == s_dwAboveNormalThreadPriority)
	{
		nPriority = THREAD_PRIORITY_ABOVE_NORMAL;
	}
	else if (DWORD(nPriority) == s_dwBelowNormalThreadPriority)
	{
		nPriority = THREAD_PRIORITY_BELOW_NORMAL;
	}
	else
	{
		nPriority = THREAD_PRIORITY_NORMAL;
	}

	return nPriority;
}

BOOL _internal_SetThreadPriority
(
    HANDLE hThread,
    int nPriority
)
{
	// Read user defined thread priority from registry
    if (-1 == s_dwHighestThreadPriority)
	{
		s_dwHighestThreadPriority	  = 248 + THREAD_PRIORITY_HIGHEST;
		s_dwAboveNormalThreadPriority = 248 + THREAD_PRIORITY_ABOVE_NORMAL;
		s_dwNormalThreadPriority	  = 248 + THREAD_PRIORITY_NORMAL;
		s_dwBelowNormalThreadPriority = 248 + THREAD_PRIORITY_BELOW_NORMAL;

		HKEY hKey;

		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\DirectShow\\ThreadPriority"), 
										0, 0, &hKey))
		{
			DWORD dwType, dwSize;
			DWORD dwHighest, dwAboveNormal, dwNormal, dwBelowNormal;

			// Use default value if the value isn't defined, or is invalidate
			dwSize = sizeof (DWORD);
			if (ERROR_SUCCESS != RegQueryValueEx (hKey, TEXT("Highest"), NULL, &dwType, LPBYTE(&dwHighest), &dwSize) ||
				REG_DWORD != dwType || 
				dwHighest >= 256)
			{
				dwHighest = s_dwHighestThreadPriority;
			}

			// Use default value if the value isn't defined, or is invalidate
			dwSize = sizeof (DWORD);
			if (ERROR_SUCCESS != RegQueryValueEx (hKey, TEXT("AboveNormal"), NULL, &dwType, LPBYTE(&dwAboveNormal), &dwSize) ||
				REG_DWORD != dwType || 
				dwAboveNormal >= 256)
			{
				dwAboveNormal = s_dwAboveNormalThreadPriority;
			}

			// Use default value if the value isn't defined, or is invalidate
			dwSize = sizeof (DWORD);
			if (ERROR_SUCCESS != RegQueryValueEx (hKey, TEXT("Normal"), NULL, &dwType, LPBYTE(&dwNormal), &dwSize) ||
				REG_DWORD != dwType || 
				dwNormal >= 256)
			{
				dwNormal = s_dwNormalThreadPriority;
			}

			// Use default value if the value isn't defined, or is invalidate
			dwSize = sizeof (DWORD);
			if (ERROR_SUCCESS != RegQueryValueEx (hKey, TEXT("BelowNormal"), NULL, &dwType, LPBYTE(&dwBelowNormal), &dwSize) ||
				REG_DWORD != dwType || 
				dwBelowNormal >= 256)
			{
				dwBelowNormal = s_dwBelowNormalThreadPriority;
			}

			// Update the thread priorities if the following rule is followed
			if (dwHighest <= dwAboveNormal && 
				dwAboveNormal <= dwNormal  && 
				dwNormal <= dwBelowNormal)
			{
				s_dwHighestThreadPriority      = dwHighest;
				s_dwAboveNormalThreadPriority  = dwAboveNormal;
				s_dwNormalThreadPriority       = dwNormal;
				s_dwBelowNormalThreadPriority  = dwBelowNormal;
			}
			else
			{
				RETAILMSG (TRUE, (TEXT("SAPI WARNING: Invalidate thread priorities, the higher thread priority must be lesser than lower, each must be less than 256\n")));
			}

			RegCloseKey (hKey);
		}
	}

	switch (nPriority)
	{
	case THREAD_PRIORITY_HIGHEST:
		nPriority = s_dwHighestThreadPriority;
		break;

	case THREAD_PRIORITY_ABOVE_NORMAL:
		nPriority = s_dwAboveNormalThreadPriority;
		break;

	case THREAD_PRIORITY_BELOW_NORMAL:
		nPriority = s_dwBelowNormalThreadPriority;
		break;

	case THREAD_PRIORITY_NORMAL:
	default:
		nPriority = s_dwNormalThreadPriority;
		break;
	}

	return CeSetThreadPriority (hThread, nPriority);
}

#endif
