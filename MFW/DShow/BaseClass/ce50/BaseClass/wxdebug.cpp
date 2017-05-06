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

// ActiveX system debugging facilities

#define _WINDLL

#include <streams.h>
#include <stdarg.h>
#ifndef UNDER_CE
#include <stdio.h>
#endif // UNDER_CE

#ifdef DEBUG
#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE
#endif // _UNICODE
#endif // UNICODE
#endif // DEBUG

#include <tchar.h>

#ifdef DEBUG

const INT iDEBUGINFO = 512;                 // Used to format strings

HINSTANCE m_hInst;                          // Module instance handle
TCHAR m_ModuleName[iDEBUGINFO];             // Cut down module name

CRITICAL_SECTION m_CSDebug;                 // Controls access to list
DWORD m_dwNextCookie;                       // Next active object ID
ObjectDesc *pListHead = NULL;               // First active object
DWORD m_dwObjectCount;                      // Active object count
BOOL m_bInit = FALSE;                       // Have we been initialised
DWORD dwWaitTimeout = INFINITE;             // Default timeout value
DWORD dwTimeOffset;             // Time of first DbgLog call
bool g_fUseKASSERT = false;                 // don't create messagebox
bool g_fDbgInDllEntryPoint = false;


const TCHAR *pBaseKey = TEXT("SOFTWARE\\Debug");
const TCHAR *pGlobalKey = TEXT("GLOBAL");
static TCHAR *pUnknownName = TEXT("UNKNOWN");

TCHAR *TimeoutName = TEXT("TIMEOUT");

/* This sets the instance handle that the debug library uses to find
the module's file name from the Win32 GetModuleFileName function */


void WINAPI DbgInitialise(HINSTANCE hInst)
{
    InitializeCriticalSection(&m_CSDebug);
    m_bInit = TRUE;
    m_hInst = hInst;
    DbgInitModuleName();

	/*
	 * We can get to this point from (at least) two different code paths:
	 * Either DbgInitialise() is called by the base class DLL entry point,
	 * in which case hInst is a DLL instance handle, or we're called by
	 * DbgRegisterObjectCreation, in which case hInst is a proc handle.
	 * We don't want pass our handle to DEBUGREGISTER in the proc case
	 * since RegisterDbgZones only takes a DLL handle or NULL. We don't
	 * want to pass NULL either since that could cause a clash with
	 * debug zones defined by the application.
	 *
	 * If users want to take advantage of the debug zone implementation
	 * in the DirectShow base classes, then they either need to:
	 * 1. Implement their own DllMain() which calls DbgInitialise, or
	 * 2. Set their DLLENTRY=DllEntryPoint to use the base class default.
	 *
	 * NOTE: Option #2 above will not call static ctors/dtors, so if you
	 * have global C++ objects in your module you need to use option #1.
	 * You may also want to define your own dpCurSettings.
	 *
	 */

	if (hInst != (HINSTANCE)GetModuleHandle(NULL))
	{
		// only set up zones if hInst is a *DLL* instance handle
		DEBUGREGISTER((HINSTANCE)hInst);
	}
}


/* This is called to clear up any resources the debug library uses - at the
moment we delete our critical section and the object list. The values we
retrieve from the registry are all done during initialisation but we don't
go looking for update notifications while we are running, if the values
are changed then the application has to be restarted to pick them up */

void WINAPI DbgTerminate()
{
    DeleteCriticalSection(&m_CSDebug);
    m_bInit = FALSE;
}

/* This function has been deprecated, use DebugZones instead of registry keys */

void WINAPI DbgInitKeyLevels(HKEY hKey)
{
    DEBUGMSG(ZONE_ERROR,(TEXT("DbgInitKeyLevels() deprecated, use DebugZones instead\n")));
}


void WINAPI DbgOutString(LPCTSTR psz)
{
    OutputDebugString (psz);
}

/* This function has been deprecated, use DebugZones instead of registry keys */
void WINAPI DbgInitLogTo (
                          HKEY hKey)
{
    DEBUGMSG(ZONE_ERROR,(TEXT("DbgInitLogTo() deprecated, use DebugZones instead\n")));
}




/* Initialise the module file name */

void WINAPI DbgInitModuleName()
{
    TCHAR FullName[iDEBUGINFO];     // Load the full path and module name
    TCHAR *pName;                   // Searches from the end for a backslash
    
    GetModuleFileName(m_hInst,FullName,iDEBUGINFO);
    pName = _tcsrchr(FullName,'\\');
    if (pName == NULL) {
        pName = FullName;
    } else {
        pName++;
    }
    lstrcpy(m_ModuleName,pName);
}


/* Displays a message box if the condition evaluated to FALSE */

void WINAPI DbgAssert(const TCHAR *pCondition,const TCHAR *pFileName,INT iLine)
{
    TCHAR szInfo[iDEBUGINFO];
    
#ifndef UNDER_CE
    wsprintf(szInfo, TEXT("%s \nAt line %d of %s\nContinue? (Cancel to debug)"),
        pCondition, iLine, pFileName);
    
    INT MsgId = MessageBox(NULL,szInfo,TEXT("ASSERT Failed"),
        MB_SYSTEMMODAL |
        MB_ICONHAND |
        MB_YESNOCANCEL |
        MB_SETFOREGROUND);
    switch (MsgId)
    {
    case IDNO:              /* Kill the application */
        
        FatalAppExit(FALSE, TEXT("Application terminated"));
        break;
        
    case IDCANCEL:          /* Break into the debugger */
        
        DebugBreak();
        break;
        
    case IDYES:             /* Ignore assertion continue execution */
        break;
    }
#else // !UNDER_CE
    wsprintf(szInfo, TEXT("%s \nAt line %d of %s\n"),
        pCondition, iLine, pFileName);
    OutputDebugString(szInfo);
    DebugBreak();
#endif // !UNDER_CE
}

/* Displays a message box at a break point */

void WINAPI DbgBreakPoint(const TCHAR *pCondition,const TCHAR *pFileName,INT iLine)
{
    TCHAR szInfo[iDEBUGINFO];
    
#ifndef UNDER_CE
    wsprintf(szInfo, TEXT("%s \nAt line %d of %s\nContinue? (Cancel to debug)"),
        pCondition, iLine, pFileName);
    
    INT MsgId = MessageBox(NULL,szInfo,TEXT("Hard coded break point"),
        MB_SYSTEMMODAL |
        MB_ICONHAND |
        MB_YESNOCANCEL |
        MB_SETFOREGROUND);
    switch (MsgId)
    {
    case IDNO:              /* Kill the application */
        
        FatalAppExit(FALSE, TEXT("Application terminated"));
        break;
        
    case IDCANCEL:          /* Break into the debugger */
        
        DebugBreak();
        break;
        
    case IDYES:             /* Ignore break point continue execution */
        break;
    }
#else // !UNDER_CE
    wsprintf(szInfo, TEXT("%s \nAt line %d of %s\n"),
        pCondition, iLine, pFileName);
    OutputDebugString(szInfo);
    DebugBreak();
#endif // !UNDER_CE
}


/* 
 * Deprecated: Place-holder to pass the debugging info onto the console output
 *
 * Please Use DEBUGMSG instead of DbgLog & DbgLogInfo
 */
void WINAPI DbgLogInfo(DWORD Type, DWORD Level, const TCHAR *pFormat,...)
{
    if (Type) 
    {
        va_list va;
        va_start(va, pFormat);

        //
        // We're just "implementing" the Level argument for the LOG_TRACE parameter.
        // 
        // For the LOG_TRACE parameter, we need to verify if ZONE_INFO_MAX is also
        // turned on. If it is, all tracing information should be sent to output.
        // Otherwise, just send to output if user wanted selective info
        // (0 means more info; 5 means less info).
        //
        //   For all other LOG_<>, send it straight to NKDbgPrintfW.
        //
        if (Type != LOG_TRACE ||             // anything different than LOG_TRACE
            (Level < 3  ||  ZONE_INFO_MAX))  // LOG_TRACE is on and user wants high level of information, so verify if ZONE_INFO_MAX is turned on...
            
        {
            TCHAR rgchBuf[MAX_DEBUG_STR];
            _vsntprintf (rgchBuf, (sizeof(rgchBuf)/sizeof(rgchBuf[0])), pFormat, va);
            rgchBuf[MAX_DEBUG_STR-1] = '\0';
            OutputDebugStringW(rgchBuf);
        }
        va_end(va);
    }
}


/* 
 * Deprecated: Place-holder to pass the debugging info onto the console output.
 *
 * Please Use DEBUGMSG instead of DbgLog & DbgLogInfo
 */
void WINAPI DbgLogInfo(DWORD Type, const char *pFormat,...)
{
    if (Type)
    {
        char rgchBuf[MAX_DEBUG_STR];
        WCHAR wzBuf[MAX_DEBUG_STR];
        va_list va;
        va_start(va, pFormat);
        
        _vsnprintf (rgchBuf, MAX_DEBUG_STR, pFormat, va);
        rgchBuf[MAX_DEBUG_STR - 1] = '\0';      // guarantees NULL-terminated string
        MultiByteToWideChar(CP_ACP, 0, rgchBuf, MAX_DEBUG_STR, wzBuf, MAX_DEBUG_STR);
        OutputDebugString(wzBuf);
        va_end(va);
    }
}


/* If we are executing as a pure kernel filter we cannot display message
boxes to the user, this provides an alternative which puts the error
condition on the debugger output with a suitable eye catching message */

void WINAPI DbgKernelAssert(const TCHAR *pCondition,const TCHAR *pFileName,INT iLine)
{
    DbgLog((LOG_ERROR,0,TEXT("Assertion FAILED (%s) at line %d in file %s"),
        pCondition, iLine, pFileName));
    DebugBreak();
}



/* Each time we create an object derived from CBaseObject the constructor will
call us to register the creation of the new object. We are passed a string
description which we store away. We return a cookie that the constructor
uses to identify the object when it is destroyed later on. We update the
total number of active objects in the DLL mainly for debugging purposes */

DWORD WINAPI DbgRegisterObjectCreation(const TCHAR *pObjectName)
{
    /* If this fires you have a mixed DEBUG/RETAIL build */
    
    ASSERT(pObjectName);
    
    /* Create a place holder for this object description */
    
    ObjectDesc *pObject = new ObjectDesc;
    ASSERT(pObject);
    
    /* It is valid to pass a NULL object name */
    if (pObject == NULL) {
        return FALSE;
    }
    
    /* Check we have been initialised - we may not be initialised when we are
    being pulled in from an executable which has globally defined objects
    as they are created by the C++ run time before WinMain is called */
    
    if (m_bInit == FALSE) {
        DbgInitialise(GetModuleHandle(NULL));
    }
    
    /* Grab the list critical section */
    EnterCriticalSection(&m_CSDebug);
    
    /* If no name then default to UNKNOWN */
    if (pObjectName == NULL) {
        pObjectName = pUnknownName;
    }
    
    /* Put the new description at the head of the list */
    
    pObject->m_pName = pObjectName;
    pObject->m_dwCookie = ++m_dwNextCookie;
    pObject->m_pNext = pListHead;
    
    pListHead = pObject;
    m_dwObjectCount++;
    
    DWORD ObjectCookie = pObject->m_dwCookie;
    ASSERT(ObjectCookie);
    
    DbgLog((LOG_MEMORY,2,TEXT("Object created   %d (%s) %d Active"),
        pObject->m_dwCookie, pObjectName, m_dwObjectCount));
    
    LeaveCriticalSection(&m_CSDebug);
    return ObjectCookie;
}


/* This is called by the CBaseObject destructor when an object is about to be
destroyed, we are passed the cookie we returned during construction that
identifies this object. We scan the object list for a matching cookie and
remove the object if successful. We also update the active object count */

BOOL WINAPI DbgRegisterObjectDestruction(DWORD dwCookie)
{
    /* Grab the list critical section */
    EnterCriticalSection(&m_CSDebug);
    
    ObjectDesc *pObject = pListHead;
    ObjectDesc *pPrevious = NULL;
    
    /* Scan the object list looking for a cookie match */
    
    while (pObject) {
        if (pObject->m_dwCookie == dwCookie) {
            break;
        }
        pPrevious = pObject;
        pObject = pObject->m_pNext;
    }
    
    if (pObject == NULL) {
        DbgBreak("Apparently destroying a bogus object");
        LeaveCriticalSection(&m_CSDebug);
        return FALSE;
    }
    
    /* Is the object at the head of the list */
    
    if (pPrevious == NULL) {
        pListHead = pObject->m_pNext;
    } else {
        pPrevious->m_pNext = pObject->m_pNext;
    }
    
    /* Delete the object and update the housekeeping information */
    
    m_dwObjectCount--;
    DbgLog((LOG_MEMORY,2,TEXT("Object destroyed %d (%s) %d Active"),
        pObject->m_dwCookie, pObject->m_pName, m_dwObjectCount));
    
    delete pObject;
    LeaveCriticalSection(&m_CSDebug);
    return TRUE;
}


/* This runs through the active object list displaying their details */

void WINAPI DbgDumpObjectRegister()
{
    TCHAR szInfo[iDEBUGINFO];
    
    /* Grab the list critical section */
    
    EnterCriticalSection(&m_CSDebug);
    ObjectDesc *pObject = pListHead;
    
    /* Scan the object list displaying the name and cookie */
    
    DbgLog((LOG_MEMORY,2,TEXT("")));
    DbgLog((LOG_MEMORY,2,TEXT("   ID             Object Description")));
    DbgLog((LOG_MEMORY,2,TEXT("")));
    
    while (pObject) {
        wsprintf(szInfo,TEXT("%5d (%p) %30s"),pObject->m_dwCookie, &pObject, pObject->m_pName);
        DbgLog((LOG_MEMORY,2,szInfo));
        pObject = pObject->m_pNext;
    }
    
    wsprintf(szInfo,TEXT("Total object count %5d"),m_dwObjectCount);
    DbgLog((LOG_MEMORY,2,TEXT("")));
    DbgLog((LOG_MEMORY,1,szInfo));
    LeaveCriticalSection(&m_CSDebug);
}


/*  Debug infinite wait stuff */
DWORD WINAPI DbgWaitForSingleObject(HANDLE h)
{
    DWORD dwWaitResult;
    do {
        dwWaitResult = WaitForSingleObject(h, dwWaitTimeout);
        ASSERT(dwWaitResult == WAIT_OBJECT_0);
    } while (dwWaitResult == WAIT_TIMEOUT);
    return dwWaitResult;
}


DWORD WINAPI DbgWaitForMultipleObjects(DWORD nCount,
                                       CONST HANDLE *lpHandles,
                                       BOOL bWaitAll)
{
    DWORD dwWaitResult;
    do {
        dwWaitResult = WaitForMultipleObjects(nCount,
            lpHandles,
            bWaitAll,
            dwWaitTimeout);
        ASSERT((DWORD)(dwWaitResult - WAIT_OBJECT_0) < MAXIMUM_WAIT_OBJECTS);
    } while (dwWaitResult == WAIT_TIMEOUT);
    return dwWaitResult;
}

void WINAPI DbgSetWaitTimeout(DWORD dwTimeout)
{
    dwWaitTimeout = dwTimeout;
}

#endif /* DEBUG */

#ifdef _OBJBASE_H_

/*  Stuff for printing out our GUID names */

GUID_STRING_ENTRY g_GuidNames[] = {
#define OUR_GUID_ENTRY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    { TEXT(#name), { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } } },
#include <uuids.h>
};

CGuidNameList GuidNames;
int g_cGuidNames = sizeof(g_GuidNames) / sizeof(g_GuidNames[0]);

TCHAR *CGuidNameList::operator [] (const GUID &guid)
{
    for (int i = 0; i < g_cGuidNames; i++) {
        if (g_GuidNames[i].guid == guid) {
            return g_GuidNames[i].szName;
        }
    }
    if (guid == GUID_NULL) {
        return TEXT("GUID_NULL");
    }
    
    // !!! add something to print FOURCC guids?
    
    // shouldn't this print the hex CLSID?
    return TEXT("Unknown GUID Name");
}

#endif /* _OBJBASE_H_ */

/*  CDisp class - display our data types */

// clashes with REFERENCE_TIME
CDisp::CDisp(LONGLONG ll, int Format)
{
    // note: this could be combined with CDisp(LONGLONG) by
    // introducing a default format of CDISP_REFTIME
    LARGE_INTEGER li;
    li.QuadPart = ll;
    switch (Format) {
    case CDISP_DEC:
        {
            TCHAR  temp[20];
            int pos=20;
            temp[--pos] = 0;
            int digit;
            // always output at least one digit
            do {
                // Get the rightmost digit - we only need the low word
                digit = li.LowPart % 10;
                li.QuadPart /= 10;
                temp[--pos] = (TCHAR) digit+L'0';
            } while (li.QuadPart);
            wsprintf(m_String, TEXT("%s"), temp+pos);
            break;
        }
    case CDISP_HEX:
    default:
        wsprintf(m_String, TEXT("0x%X%8.8X"), li.HighPart, li.LowPart);
    }
};

CDisp::CDisp(REFCLSID clsid)
{
    WCHAR strClass[CHARS_IN_GUID+1];
    StringFromGUID2(clsid, strClass, sizeof(strClass) / sizeof(strClass[0]));
    ASSERT(sizeof(m_String)/sizeof(m_String[0]) >= CHARS_IN_GUID+1);
    wsprintf(m_String, TEXT("%ls"), strClass);
};

/*  Display stuff */
CDisp::CDisp(CRefTime llTime)
{
    LPTSTR lpsz = m_String;
    LONGLONG llDiv;
    if (llTime < 0) {
        llTime = -llTime;
        lpsz += wsprintf(lpsz, TEXT("-"));
    }
    llDiv = (LONGLONG)24 * 3600 * 10000000;
    if (llTime >= llDiv) {
        lpsz += wsprintf(lpsz, TEXT("%d days "), (LONG)(llTime / llDiv));
        llTime = llTime % llDiv;
    }
    llDiv = (LONGLONG)3600 * 10000000;
    if (llTime >= llDiv) {
        lpsz += wsprintf(lpsz, TEXT("%d hrs "), (LONG)(llTime / llDiv));
        llTime = llTime % llDiv;
    }
    llDiv = (LONGLONG)60 * 10000000;
    if (llTime >= llDiv) {
        lpsz += wsprintf(lpsz, TEXT("%d mins "), (LONG)(llTime / llDiv));
        llTime = llTime % llDiv;
    }
    wsprintf(lpsz, TEXT("%d.%3.3d sec"),
        (LONG)llTime / 10000000,
        (LONG)((llTime % 10000000) / 10000));
};

/*  Display pin */
CDisp::CDisp(IPin *pPin)
{
    PIN_INFO pi;
    TCHAR str[MAX_PIN_NAME];
    if (pPin) {
        pPin->QueryPinInfo(&pi);
        QueryPinInfoReleaseFilter(pi);
#ifndef UNICODE
        WideCharToMultiByte(GetACP(), 0, pi.achName, lstrlenW(pi.achName) + 1,
            str, MAX_PIN_NAME, NULL, NULL);
#else
        lstrcpy(str, pi.achName);
#endif
    } else {
        lstrcpy(str, TEXT("NULL IPin"));
    }
    
    m_pString = (PTCHAR) new TCHAR[lstrlen(str)+64];
    if (!m_pString) {
        return;
    }
    
    if (pPin) {
        CLSID clsid;
        pi.pFilter->GetClassID(&clsid);
        wsprintf(m_pString, TEXT("%s(%s)"), GuidNames[clsid], str);
    }
}

CDisp::~CDisp()
{
}

CDispBasic::~CDispBasic()
{
    if (m_pString != m_String) {
        delete [] m_pString;
    }
}

CDisp::CDisp(double d)
{
#ifdef DEBUG
#ifndef UNDER_CE
    _stprintf(m_String, TEXT("%.16g"), d);
#else
    wsprintf(m_String, TEXT("%.16g"), d);
#endif // UNDER_CE
#else
    wsprintf(m_String, TEXT("%d.%03d"), (int) d, (int) ((d - (int) d) * 1000));
#endif
}


/* If built for debug this will display the media type details. We convert the
major and subtypes into strings and also ask the base classes for a string
description of the subtype, so MEDIASUBTYPE_RGB565 becomes RGB 565 16 bit
We also display the fields in the BITMAPINFOHEADER structure, this should
succeed as we do not accept input types unless the format is big enough */

#ifdef DEBUG
void WINAPI DisplayType(LPSTR label, const AM_MEDIA_TYPE *pmtIn)
{
    
    /* Dump the GUID types and a short description */
    
    DbgLog((LOG_TRACE,5,TEXT("")));
    DbgLog((LOG_TRACE,2,TEXT("%hs  M type %s  S type %s"), label,
        GuidNames[pmtIn->majortype],
        GuidNames[pmtIn->subtype]));
    DbgLog((LOG_TRACE,5,TEXT("Subtype description %s"),GetSubtypeName(&pmtIn->subtype)));
    
    /* Dump the generic media types */
    
    if (pmtIn->bTemporalCompression) {
        DbgLog((LOG_TRACE,5,TEXT("Temporally compressed")));
    } else {
        DbgLog((LOG_TRACE,5,TEXT("Not temporally compressed")));
    }
    
    if (pmtIn->bFixedSizeSamples) {
        DbgLog((LOG_TRACE,5,TEXT("Sample size %d"),pmtIn->lSampleSize));
    } else {
        DbgLog((LOG_TRACE,5,TEXT("Variable size samples")));
    }
    
    if (pmtIn->formattype == FORMAT_VideoInfo) {
        /* Dump the contents of the BITMAPINFOHEADER structure */
        BITMAPINFOHEADER *pbmi = HEADER(pmtIn->pbFormat);
        VIDEOINFOHEADER *pVideoInfo = (VIDEOINFOHEADER *)pmtIn->pbFormat;
        
        DbgLog((LOG_TRACE,5,TEXT("Source rectangle (Left %d Top %d Right %d Bottom %d)"),
            pVideoInfo->rcSource.left,
            pVideoInfo->rcSource.top,
            pVideoInfo->rcSource.right,
            pVideoInfo->rcSource.bottom));
        
        DbgLog((LOG_TRACE,5,TEXT("Target rectangle (Left %d Top %d Right %d Bottom %d)"),
            pVideoInfo->rcTarget.left,
            pVideoInfo->rcTarget.top,
            pVideoInfo->rcTarget.right,
            pVideoInfo->rcTarget.bottom));
        
        DbgLog((LOG_TRACE,5,TEXT("Size of BITMAPINFO structure %d"),pbmi->biSize));
        if (pbmi->biCompression < 256) {
            DbgLog((LOG_TRACE,2,TEXT("%dx%dx%d bit  (%d)"),
                pbmi->biWidth, pbmi->biHeight,
                pbmi->biBitCount, pbmi->biCompression));
        } else {
            DbgLog((LOG_TRACE,2,TEXT("%dx%dx%d bit '%4.4hs'"),
                pbmi->biWidth, pbmi->biHeight,
                pbmi->biBitCount, &pbmi->biCompression));
        }
        
        DbgLog((LOG_TRACE,2,TEXT("Image size %d"),pbmi->biSizeImage));
        DbgLog((LOG_TRACE,5,TEXT("Planes %d"),pbmi->biPlanes));
        DbgLog((LOG_TRACE,5,TEXT("X Pels per metre %d"),pbmi->biXPelsPerMeter));
        DbgLog((LOG_TRACE,5,TEXT("Y Pels per metre %d"),pbmi->biYPelsPerMeter));
        DbgLog((LOG_TRACE,5,TEXT("Colours used %d"),pbmi->biClrUsed));
        
    } else if (pmtIn->majortype == MEDIATYPE_Audio) {
        DbgLog((LOG_TRACE,2,TEXT("     Format type %s"),
            GuidNames[pmtIn->formattype]));
        DbgLog((LOG_TRACE,2,TEXT("     Subtype %s"),
            GuidNames[pmtIn->subtype]));
        
        if ((pmtIn->subtype != MEDIASUBTYPE_MPEG1Packet)
            && (pmtIn->cbFormat >= sizeof(PCMWAVEFORMAT)))
        {
            /* Dump the contents of the WAVEFORMATEX type-specific format structure */
            
            WAVEFORMATEX *pwfx = (WAVEFORMATEX *) pmtIn->pbFormat;
            DbgLog((LOG_TRACE,2,TEXT("wFormatTag %u"), pwfx->wFormatTag));
            DbgLog((LOG_TRACE,2,TEXT("nChannels %u"), pwfx->nChannels));
            DbgLog((LOG_TRACE,2,TEXT("nSamplesPerSec %lu"), pwfx->nSamplesPerSec));
            DbgLog((LOG_TRACE,2,TEXT("nAvgBytesPerSec %lu"), pwfx->nAvgBytesPerSec));
            DbgLog((LOG_TRACE,2,TEXT("nBlockAlign %u"), pwfx->nBlockAlign));
            DbgLog((LOG_TRACE,2,TEXT("wBitsPerSample %u"), pwfx->wBitsPerSample));
            
            /* PCM uses a WAVEFORMAT and does not have the extra size field */
            
            if (pmtIn->cbFormat >= sizeof(WAVEFORMATEX)) {
                DbgLog((LOG_TRACE,2,TEXT("cbSize %u"), pwfx->cbSize));
            }
        } else {
        }
        
    } else {
        DbgLog((LOG_TRACE,2,TEXT("     Format type %s"),
            GuidNames[pmtIn->formattype]));
        // !!!! should add code to dump wave format, others
    }
}


void WINAPI DumpGraph(IFilterGraph *pGraph, DWORD dwLevel)
{
    IEnumFilters *pFilters;
    
    DbgLog((LOG_TRACE,dwLevel,TEXT("DumpGraph [%x]"), pGraph));
    
    if (FAILED(pGraph->EnumFilters(&pFilters))) {
        DbgLog((LOG_TRACE,dwLevel,TEXT("EnumFilters failed!")));
    }
    
    IBaseFilter *pFilter;
    ULONG   n;
    while (pFilters->Next(1, &pFilter, &n) == S_OK) {
        FILTER_INFO info;
        
        if (FAILED(pFilter->QueryFilterInfo(&info))) {
            DbgLog((LOG_TRACE,dwLevel,TEXT("    Filter [%x]  -- failed QueryFilterInfo"), pFilter));
        } else {
            QueryFilterInfoReleaseGraph(info);
            
            // !!! should QueryVendorInfo here!
            
            DbgLog((LOG_TRACE,dwLevel,TEXT("    Filter [%x]  '%ls'"), pFilter, info.achName));
            
            IEnumPins *pins;
            
            if (FAILED(pFilter->EnumPins(&pins))) {
                DbgLog((LOG_TRACE,dwLevel,TEXT("EnumPins failed!")));
            } else {
                
                IPin *pPin;
                while (pins->Next(1, &pPin, &n) == S_OK) {
                    PIN_INFO    info;
                    
                    if (FAILED(pPin->QueryPinInfo(&info))) {
                        DbgLog((LOG_TRACE,dwLevel,TEXT("          Pin [%x]  -- failed QueryPinInfo"), pPin));
                    } else {
                        QueryPinInfoReleaseFilter(info);
                        
                        IPin *pPinConnected = NULL;
                        
                        HRESULT hr = pPin->ConnectedTo(&pPinConnected);
                        
                        if (pPinConnected) {
                            DbgLog((LOG_TRACE,dwLevel,TEXT("          Pin [%x]  '%ls' [%sput]")
                                TEXT("  Connected to pin [%x]"),
                                pPin, info.achName,
                                info.dir == PINDIR_INPUT ? TEXT("In") : TEXT("Out"),
                                pPinConnected));
                            
                            pPinConnected->Release();
                            
                            // perhaps we should really dump the type both ways as a sanity
                            // check?
                            if (info.dir == PINDIR_OUTPUT) {
                                AM_MEDIA_TYPE mt;
                                
                                hr = pPin->ConnectionMediaType(&mt);
                                
                                if (SUCCEEDED(hr)) {
                                    DisplayType("Connection type", &mt);
                                    
                                    FreeMediaType(mt);
                                }
                            }
                        } else {
                            DbgLog((LOG_TRACE,dwLevel,
                                TEXT("          Pin [%x]  '%ls' [%sput]"),
                                pPin, info.achName,
                                info.dir == PINDIR_INPUT ? TEXT("In") : TEXT("Out")));
                            
                        }
                    }
                    
                    pPin->Release();
                    
                }
                
                pins->Release();
            }
            
        }
        
        pFilter->Release();
    }
    
    pFilters->Release();
    
}

#endif

