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

// Debugging facilities, January 1995

#ifndef __WXDEBUG__
#define __WXDEBUG__

#define MAX_DEBUG_STR 384

//
// The original debugging mechanism (explained below) has been superseeded to
// use winCE Debug Zones mechanism. The DShow code still references the original
// macros, that in turn use Debug Zones ones.
//
// The DbgLog & DbgLogInfo functionality is maintained for compatibility. For
// new code, please use DebugZones' DEBUGMSG instead.
//

#define DBG_BIT(bitno) (1<<(bitno))

#define ZONE_ERROR		DEBUGZONE(0)
#define ZONE_WARNING	DEBUGZONE(1)
#define ZONE_INFO_MIN	DEBUGZONE(2)
#define ZONE_INFO_MAX	DEBUGZONE(3)
#define ZONE_INIT		DEBUGZONE(4)
#define ZONE_ENTER		DEBUGZONE(5)
#define ZONE_TIMING     DEBUGZONE(6)
#define ZONE_MEMORY     DEBUGZONE(7)
#define ZONE_LOCKING    DEBUGZONE(8)
#define ZONE_MTYPES     DEBUGZONE(9)
#define ZONE_REFCOUNT	DEBUGZONE(11)
#define ZONE_PERF     	DEBUGZONE(12)
#define ZONE_CELOG		DEBUGZONE(13)
#define ZONE_TEMP		DEBUGZONE(14)

#define ZONE_NAMES              \
		TEXT("Errors"),			\
		TEXT("Warnings"),		\
		TEXT("Info (min)"),		\
		TEXT("Info (max)"),		\
		TEXT("Initialize"),		\
		TEXT("Enter,Exit"),		\
		TEXT("Timing"),		    \
		TEXT("Memory"), 		\
		TEXT("Locking"), 		\
		TEXT("Media Types"),	\
		TEXT("Undefined"),		\
		TEXT("RefCount"),		\
		TEXT("Performance"),	\
		TEXT("CELOG"),	        \
		TEXT("Temporary tests"),\
		TEXT("Undefined")

#include <perflog.h>

// This library provides fairly straight forward debugging functionality, this
// is split into two main sections. The first is assertion handling, there are
// three types of assertions provided here. The most commonly used one is the
// ASSERT(condition) macro which will pop up a message box including the file
// and line number if the condition evaluates to FALSE. Then there is the
// EXECUTE_ASSERT macro which is the same as ASSERT except the condition will
// still be executed in NON debug builds. The final type of assertion is the
// KASSERT macro which is more suitable for pure (perhaps kernel) filters as
// the condition is printed onto the debugger rather than in a message box.
//
// The other part of the debug module facilties is general purpose logging.
// This is accessed by calling DbgLog(). The function takes a type and level
// field which define the type of informational string you are presenting and
// it's relative importance. The type field can be a combination (one or more)
// of LOG_TIMING, LOG_TRACE, LOG_MEMORY, LOG_LOCKING and LOG_ERROR. The level
// is a DWORD value where zero defines highest important. Use of zero as the
// debug logging level is to be encouraged ONLY for major errors or events as
// they will ALWAYS be displayed on the debugger. Other debug output has it's
// level matched against the current debug output level stored in the registry
// for this module and if less than the current setting it will be displayed.
//
// Each module or executable has it's own debug output level for each of the
// five types. These are read in when the DbgInitialise function is called
// for DLLs linking to STRMBASE.LIB this is done automatically when the DLL
// is loaded, executables must call it explicitely with the module instance
// handle given to them through the WINMAIN entry point. An executable must
// also call DbgTerminate when they have finished to clean up the resources
// the debug library uses, once again this is done automatically for DLLs

// These are the five different categories of logging information

#define LOG_TIMING  ZONE_TIMING   // Timing and performance measurements
#define LOG_TRACE   ZONE_INFO_MIN // General step point call tracing
#define LOG_MEMORY  ZONE_MEMORY   // Memory and object allocation/destruction
#define LOG_LOCKING ZONE_LOCKING  // Locking/unlocking of critical sections
#define LOG_ERROR   ZONE_ERROR    // Debug error notification
#define LOG_WARNING ZONE_WARNING
#define LOG_MTYPES  ZONE_MTYPES
#define LOG_CUSTOM1 ZONE_TEMP
#define LOG_CUSTOM2 ZONE_TEMP
#define LOG_CUSTOM3 ZONE_TEMP
#define LOG_CUSTOM4 ZONE_TEMP
#define LOG_CUSTOM5 ZONE_TEMP

enum {  CDISP_HEX = 0x01,
        CDISP_DEC = 0x02};

// For each object created derived from CBaseObject (in debug builds) we
// create a descriptor that holds it's name (statically allocated memory)
// and a cookie we assign it. We keep a list of all the active objects
// we have registered so that we can dump a list of remaining objects

typedef struct tag_ObjectDesc {
    const TCHAR *m_pName;
    DWORD m_dwCookie;
    tag_ObjectDesc *m_pNext;
} ObjectDesc;

#define DLLIMPORT __declspec(dllimport)
#define DLLEXPORT __declspec(dllexport)

#ifdef DEBUG

    #define NAME(x) TEXT(x)

    // These are used internally by the debug library (PRIVATE)

    void WINAPI DbgInitModuleName();
    DWORD WINAPI DbgRegisterObjectCreation(const TCHAR *pObjectName);
    BOOL WINAPI DbgRegisterObjectDestruction(DWORD dwCookie);

    // These functions have been deprecated, are maintained here for compatibility reasons
    // registry settings not used anymore for debugging, instead use Debug Zones
    void WINAPI DbgInitKeyLevels(HKEY hKey);
    void WINAPI DbgInitGlobalSettings();
    void WINAPI DbgInitModuleSettings();

    // Initialise the library with the module handle

    void WINAPI DbgInitialise(HINSTANCE hInst);
    void WINAPI DbgTerminate();

    void WINAPI DbgDumpObjectRegister();


    /* deprecated function, Level ignored */
    #define DbgCheckModuleLevel(Type,Level) ((Type) ? TRUE : FALSE) // Type is in fact a DEBUGZONE(x), so it is true if the debug zone is turned on...

    /* deprecated function, Level ignored */
    #define DbgSetModuleLevel(Type,Level) ((void) 0)

    // Display error and logging to the user

    void WINAPI DbgAssert(const TCHAR *pCondition,const TCHAR *pFileName,INT iLine);
    void WINAPI DbgBreakPoint(const TCHAR *pCondition,const TCHAR *pFileName,INT iLine);
    void WINAPI DbgKernelAssert(const TCHAR *pCondition,const TCHAR *pFileName,INT iLine);
    void WINAPI DbgLogInfo(DWORD Type,DWORD Level,const TCHAR *pFormat,...);
    void WINAPI DbgLogInfo(DWORD Type,const char *pFormat,...);  // same DbgLogInfo, no Level parameter
    void WINAPI DbgOutString(LPCTSTR psz);

    //  Debug infinite wait stuff
    DWORD WINAPI DbgWaitForSingleObject(HANDLE h);
    DWORD WINAPI DbgWaitForMultipleObjects(DWORD nCount,
                                    CONST HANDLE *lpHandles,
                                    BOOL bWaitAll);
    void WINAPI DbgSetWaitTimeout(DWORD dwTimeout);

#ifdef __strmif_h__
    // Display a media type: Terse at level 2, verbose at level 5
    void WINAPI DisplayType(LPSTR label, const AM_MEDIA_TYPE *pmtIn);

    // Dump lots of information about a filter graph
    void WINAPI DumpGraph(IFilterGraph *pGraph, DWORD dwLevel);
#endif

    #define KASSERT(_x_) if (!(_x_))         \
        DbgKernelAssert(TEXT(#_x_),TEXT(__FILE__),__LINE__)

    //  Break on the debugger without putting up a message box
    //  message goes to debugger instead

    #define KDbgBreak(_x_)                   \
        DbgKernelAssert(TEXT(#_x_),TEXT(__FILE__),__LINE__)

    // We chose a common name for our ASSERT macro, MFC also uses this name
    // So long as the implementation evaluates the condition and handles it
    // then we will be ok. Rather than override the behaviour expected we
    // will leave whatever first defines ASSERT as the handler (i.e. MFC)
    #ifndef ASSERT
        #define ASSERT(_x_) if (!(_x_))         \
            DbgAssert(TEXT(#_x_),TEXT(__FILE__),__LINE__)
    #endif

    #define DbgAssertAligned( _ptr_, _alignment_ ) ASSERT( ((DWORD_PTR) (_ptr_)) % (_alignment_) == 0)

    //  Put up a message box informing the user of a halt
    //  condition in the program

    #define DbgBreak(_x_)                   \
        DbgBreakPoint(TEXT(#_x_),TEXT(__FILE__),__LINE__)

    #define EXECUTE_ASSERT(_x_) ASSERT(_x_)
    #define DbgLog(_x_) DbgLogInfo _x_

    // MFC style trace macros

    #define NOTE(_x_)             DbgLog((LOG_TRACE,5,TEXT(_x_)))
    #define NOTE1(_x_,a)          DbgLog((LOG_TRACE,5,TEXT(_x_),a))
    #define NOTE2(_x_,a,b)        DbgLog((LOG_TRACE,5,TEXT(_x_),a,b))
    #define NOTE3(_x_,a,b,c)      DbgLog((LOG_TRACE,5,TEXT(_x_),a,b,c))
    #define NOTE4(_x_,a,b,c,d)    DbgLog((LOG_TRACE,5,TEXT(_x_),a,b,c,d))
    #define NOTE5(_x_,a,b,c,d,e)  DbgLog((LOG_TRACE,5,TEXT(_x_),a,b,c,d,e))

#else

    // Retail builds make public debug functions inert  - WARNING the source
    // files do not define or build any of the entry points in debug builds
    // (public entry points compile to nothing) so if you go trying to call
    // any of the private entry points in your source they won't compile

    #define NAME(_x_) NULL

#ifdef SHIP_BUILD
    #define DbgInitialise(hInst) 
#else
    #define DbgInitialise(hInst) RegisterDbgZones(hInst,&dpCurSettings);
#endif
    #define DbgTerminate()
    #define DbgLog(_x_) 0
    #define DbgOutString(psz)
    #define DbgAssertAligned( _ptr_, _alignment_ ) 0


    #define DbgRegisterObjectCreation(pObjectName)
    #define DbgRegisterObjectDestruction(dwCookie)
    #define DbgDumpObjectRegister()

    #define DbgCheckModuleLevel(Type,Level)
    #define DbgSetModuleLevel(Type,Level)

    #define DbgWaitForSingleObject(h)  WaitForSingleObject(h, INFINITE)
    #define DbgWaitForMultipleObjects(nCount, lpHandles, bWaitAll)     \
               WaitForMultipleObjects(nCount, lpHandles, bWaitAll, INFINITE)
    #define DbgSetWaitTimeout(dwTimeout)

    #define KDbgBreak(_x_)
    #define DbgBreak(_x_)

    #define KASSERT(_x_) ((void)0)
    #ifndef ASSERT
	#define ASSERT(_x_) ((void)0)
    #endif
    #define EXECUTE_ASSERT(_x_) ((void)(_x_))

    // MFC style trace macros

    #define NOTE(_x_) ((void)0)
    #define NOTE1(_x_,a) ((void)0)
    #define NOTE2(_x_,a,b) ((void)0)
    #define NOTE3(_x_,a,b,c) ((void)0)
    #define NOTE4(_x_,a,b,c,d) ((void)0)
    #define NOTE5(_x_,a,b,c,d,e) ((void)0)

    #define DisplayType(label, pmtIn) ((void)0)
    #define DumpGraph(pGraph, label) ((void)0)
#endif


// Checks a pointer which should be non NULL - can be used as follows.

#define CheckPointer(p,ret) {if((p)==NULL) return (ret);}

//   HRESULT Foo(VOID *pBar)
//   {
//       CheckPointer(pBar,E_INVALIDARG)
//   }
//
//   Or if the function returns a boolean
//
//   BOOL Foo(VOID *pBar)
//   {
//       CheckPointer(pBar,FALSE)
//   }

// These validate pointers when symbol VFWROBUST is defined
// This will normally be defined in debug not retail builds

#ifdef DEBUG
    #define VFWROBUST
#endif

#ifdef VFWROBUST

    #define ValidateReadPtr(p,cb) \
        {if(IsBadReadPtr((PVOID)p,cb) == TRUE) \
            DbgBreak("Invalid read pointer");}

    #define ValidateWritePtr(p,cb) \
        {if(IsBadWritePtr((PVOID)p,cb) == TRUE) \
            DbgBreak("Invalid write pointer");}

    #define ValidateReadWritePtr(p,cb) \
        {ValidateReadPtr(p,cb) ValidateWritePtr(p,cb)}

    #define ValidateStringPtr(p) \
        {if(IsBadStringPtr((LPCTSTR)p,INFINITE) == TRUE) \
            DbgBreak("Invalid string pointer");}

    #define ValidateStringPtrA(p) \
        {if(IsBadStringPtrA((LPCSTR)p,INFINITE) == TRUE) \
            DbgBreak("Invalid ANSII string pointer");}

    #define ValidateStringPtrW(p) \
        {if(IsBadStringPtrW((LPCWSTR)p,INFINITE) == TRUE) \
            DbgBreak("Invalid UNICODE string pointer");}

#else
    #define ValidateReadPtr(p,cb) 0
    #define ValidateWritePtr(p,cb) 0
    #define ValidateReadWritePtr(p,cb) 0
    #define ValidateStringPtr(p) 0
    #define ValidateStringPtrA(p) 0
    #define ValidateStringPtrW(p) 0
#endif


#ifdef _OBJBASE_H_

    //  Outputting GUID names.  If you want to include the name
    //  associated with a GUID (eg CLSID_...) then
    //
    //      GuidNames[yourGUID]
    //
    //  Returns the name defined in uuids.h as a string

    typedef struct {
        TCHAR   *szName;
        GUID    guid;
    } GUID_STRING_ENTRY;

    class CGuidNameList {
    public:
        TCHAR *operator [] (const GUID& guid);
    };

    extern CGuidNameList GuidNames;

#endif

#ifndef REMIND
    //  REMIND macro - generates warning as reminder to complete coding
    //  (eg) usage:
    //
    //  #pragma message (REMIND("Add automation support"))


    #define QUOTE(x) #x
    #define QQUOTE(y) QUOTE(y)
    #define REMIND(str) __FILE__ "(" QQUOTE(__LINE__) ") :  " str
#endif

//  How to display objects in a useful format
//
//  eg If you want to display a LONGLONG ll in a debug string do (eg)
//
//  DbgLog((LOG_TRACE, n, TEXT("Value is %s"), (LPCTSTR)CDisp(ll, CDISP_HEX)));


class CDispBasic
{
public:
    CDispBasic() { m_pString = m_String; };
    ~CDispBasic();
protected:
    PTCHAR m_pString;  // normally points to m_String... unless too much data
    TCHAR m_String[50];
};
class CDisp : public CDispBasic
{
public:
    CDisp(LONGLONG ll, int Format = CDISP_HEX); // Display a LONGLONG in CDISP_HEX or CDISP_DEC form
    CDisp(REFCLSID clsid);      // Display a GUID
    CDisp(double d);            // Display a floating point number
#ifdef __strmif_h__
#ifdef __STREAMS__
    CDisp(CRefTime t);          // Display a Reference Time
#endif
    CDisp(IPin *pPin);          // Display a pin as {filter clsid}(pin name)
#endif // __strmif_h__
    ~CDisp();

    //  Implement cast to (LPCTSTR) as parameter to logger
    operator LPCTSTR()
    {
        return (LPCTSTR)m_pString;
    };
};

#endif // __WXDEBUG__


