//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************

#ifndef NO_TIMER

#ifdef macintosh
#   include "voWmvPort.h"
    typedef unsigned long DWORD;
#endif
#if defined(jan_x86) || defined(_CONSOLE)
#   include "voWmvPort.h"
#   include <tchar.h>
    typedef unsigned long DWORD;
#   define _stprintf sprintf
    typedef char TCHAR;
#endif

void WMVInitFrameProfile();

void WMVStartFrameProfile(void);
void WMVMidFrameProfile(void);
void WMVStartGetOutputFrameProfile(void);
void WMVEndFrameProfile(void);

void WMVTypeFrameProfile( int iType );
void WMVFlagFrameProfile( unsigned int uiFlags );
void WMVShowBuffersFrameProfile(void* pCurr, void* pRef0, void* pRef1);
void WMVShowOutputFrameProfile(void* pCurr);
void WMVShowInputFrameProfile(void* pWMVDec, unsigned int uiLen, void* pCurr);
void WMVShowCopyBufferFrameProfile(void* pDst, void* pSrc);

void WMVFunctionCountFrameProfile(void *pFunction);

void WMVFrameProfilePrint(void* msg);
void WMVOutputFrameProfile();

#endif
