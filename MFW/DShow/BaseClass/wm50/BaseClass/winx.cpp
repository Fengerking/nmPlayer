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
#include <streams.h>
#ifdef USE_PERF_COUNTER
#include <wdm.h>
#endif // USE_PERF_COUNTER
#include "winx.h"

#ifdef UNDER_CE

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

LPSTR lstrcpynA(LPSTR lpString1, LPCSTR lpString2, int iMaxLength)
{
    LPSTR src, dst;
    //try
    //{
        src = (LPSTR)lpString2;
        dst = lpString1;
        if (iMaxLength)
        {
            while (iMaxLength && *src)
            {
                *dst++ = *src++;
                iMaxLength--;
            }
            if (iMaxLength)
            {
                *dst = '\0';            
            }
            else
            {
                dst--;
                *dst = '\0';
            }
        }
    //}
    //except (EXCEPTION_EXECUTE_HANDLER)
    //{
    //    lpString1 = (LPSTR) NULL;
    //}
    return lpString1;    // return lpString1
}

BOOL FileTimeToDosDateTime(CONST FILETIME *lpFileTime, LPWORD lpFatDate, LPWORD lpFatTime)
{
    BOOL       bRet;
    SYSTEMTIME st;

    //
    // Convert to SystemTime first
    //
    bRet = FileTimeToSystemTime(lpFileTime, &st);

    if (bRet)
    {
        //
        // lpFatDate's description from MSDN:
        //
        // Points to a variable to receive the MS-DOS date. The date is a
        // packed 16-bit value with the following format:
        //
        // Bits  Contents
        // ----  --------
        // 0-4   Day of the month (1-31)
        // 5-8   Month (1 = January, 2 = February, etc.)
        // 9-15  Year offset from 1980 (add 1980 to get actual year)
        //
        *lpFatDate = st.wDay | (st.wMonth << 5) | ((st.wYear - 1980) << 9);

        //
        // lpFatTime's description from MSDN:
        //
        // Points to a variable to receive the MS-DOS time. The time is a
        // packed 16-bit value with the following format:
        //
        // Bits  Contents
        // ----  --------
        // 0-4   Second divided by 2
        // 5-10  Minute (0-59)
        // 11-15 Hour (0-23 on a 24-hour clock)
        //
        *lpFatTime = (st.wSecond / 2) | (st.wMinute << 5) | (st.wHour << 11);
    }

    return bRet;
}

long RegCreateKey(HKEY hkey, LPCWSTR lpszSubKey, PHKEY phkResult)
{
        DWORD   dwDisposition;
        return RegCreateKeyEx(hkey, lpszSubKey, 0, NULL, 0, 0, NULL, phkResult, &dwDisposition);
}

long RegSetValue(HKEY hKey, LPCWSTR lpSubKey, DWORD dwType, LPCTSTR lpData, DWORD cbData)
{
        return RegSetValueEx(hKey, lpSubKey, 0, dwType, (LPBYTE)lpData, cbData);
}

long RegOpenKey(HKEY hKey, LPCWSTR lpszSubKey, PHKEY phkResult)
{
        return RegOpenKeyEx(hKey, lpszSubKey, 0, 0, phkResult);
}

long RegQueryValue(  HKEY hKey,  LPTSTR lpSubKey,  LPTSTR lpValue,  PLONG lpcbValue  )
{
    DWORD   dwType;
    return RegQueryValueEx(hKey, lpSubKey, NULL, &dwType, (BYTE *)lpValue, (PULONG)lpcbValue);
}


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // UNDER_CE


#ifdef USE_PERF_COUNTER

#define MmNonCached 0

PerfCounter::PerfCounter() {
    unsigned long * pCntRegion;
    PHYSICAL_ADDRESS sBusAddress;

    sBusAddress.HighPart = 0;
    sBusAddress.LowPart = 0x1FD80000L;
    pCntRegion = (DWORD *)MmMapIoSpace(sBusAddress, 0x1000, MmNonCached);

    m_pTimer = &pCntRegion[9];
}

DWORD 
PerfCounter::GetTime() {
    static DWORD dwPrev = 0xFFFFFFFF;
    static DWORD dwWraps = 0;
    DWORD dwCurr = (*m_pTimer);
    LONGLONG llCurr;
    if (dwCurr > dwPrev) {
        dwWraps++; 
    }
    llCurr = dwWraps * 0x100000000 + (0xFFFFFFFF - dwCurr);
    dwPrev =  dwCurr;
     
    return (DWORD)(llCurr / 12500);
}

PerfCounter * g_pTimer = NULL;

#endif // USE_PERF_COUNTER
