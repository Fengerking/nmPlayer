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
#ifndef _DEF_DSTHREAD_H
#define _DEF_DSTHREAD_H

// The module is for changing thread priority from registry
// daihu - Jan 7 2001

#define CUSTOM_THREADPRIORITY

HANDLE _internal_CreateThread (
    LPSECURITY_ATTRIBUTES lpsa,
    DWORD   cbStack,
    LPTHREAD_START_ROUTINE lpStartAddr,
    LPVOID lpvThreadParam,
    DWORD fdwCreate,
    LPDWORD lpIDThread
    );

int _internal_GetThreadPriority(HANDLE hThread);

BOOL _internal_SetThreadPriority(HANDLE hThread, int nPriority);

#ifndef CUSTOM_THREADPRIORITY

inline HANDLE _internal_CreateThread (
    LPSECURITY_ATTRIBUTES lpsa,
    DWORD   cbStack,
    LPTHREAD_START_ROUTINE lpStartAddr,
    LPVOID lpvThreadParam,
    DWORD fdwCreate,
    LPDWORD lpIDThread
    )
{
	return CreateThread (lpsa, cbStack, lpStartAddr, lpvThreadParam, fdwCreate, lpIDThread);
}

inline int _internal_GetThreadPriority(HANDLE hThread)
{
	return CeGetThreadPriority (hThread);
}

inline BOOL _internal_SetThreadPriority(HANDLE hThread, int nPriority)
{
	return SetThreadPriority (hThread, nPriority);
}

#endif // CUSTOM_THREADPRIORITY

#endif // _DEF_DSTHREAD_H
