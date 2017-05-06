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
//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//
//--------------------------------------------------------------------------;
#ifndef __WINX__
#define __WINX__

// this file tells us which version of the wince os we are using
#include <bldver.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef UNDER_CE

#define _strinc(_pc)    ((_pc)+1)

#define lstrcpyA    strcpy
LPSTR lstrcpynA(LPSTR lpString1, LPCSTR lpString2, int iMaxLength);
BOOL FileTimeToDosDateTime(CONST FILETIME *lpFileTime, LPWORD lpFatDate, LPWORD lpFatTime);

#define CoInitialize(x) CoInitializeEx(x, COINIT_MULTITHREADED)

long RegCreateKey(HKEY hkey, LPCWSTR lpszSubKey, PHKEY phkResult);
long RegSetValue(HKEY hKey, LPCWSTR lpSubKey, DWORD dwType, LPCTSTR lpData, DWORD cbData);
long RegOpenKey(HKEY hKey, LPCWSTR lpszSubKey, PHKEY phkResult);
long RegQueryValue(  HKEY hKey,  LPTSTR lpSubKey,  LPTSTR lpValue,  PLONG lpcbValue  );

#endif // UNDER_CE

// should be in wingdi.h
#define BI_RLE8       1L

// time slice: 25 ms
#define TIME_SLICE    25

#define MulDiv(x, y, z) (DWORD)(((LONGLONG)((LONG)(x)) * (LONGLONG)((LONG)(y)))\
                        / (LONGLONG)((LONG)(z)))

#ifndef USE_PERF_COUNTER

#define timeGetTime() GetTickCount()

#else

class PerfCounter {
public:
    PerfCounter();
    DWORD GetTime();
private:
    DWORD *m_pTimer;
};

extern PerfCounter * g_pTimer;

#define timeGetTime() (g_pTimer->GetTime())

#endif // USE_PERF_COUNTER

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // __WINX__
