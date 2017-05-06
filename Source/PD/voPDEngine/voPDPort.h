/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2007				*
*																		*
************************************************************************
*******************************************************************************
File:		voPDPort.h


Written by:	Number Huang

Change History (most recent first):
2007-07-25		JBF			Create file

*******************************************************************************/
#ifndef  __VOPDPORT_H__
#define __VOPDPORT_H__
#include "stdlib.h"
#include "stdio.h"
#include "memory.h"
#include "string.h"
#include "tchar.h"
#include "voRTSPCrossPlatform.h"
#include "VOUtility.h"
#define INVALID_SOCKET NULL
#define SOCKET_ERROR  VEC_SOCKET_ERROR
#define INLINE __inline
#ifdef LINUX
#define __cdecl 
#endif//LINUX
using namespace vo_cross_platform_streaming;
using namespace VOUtility;
#define FILE_BEGIN           0
#define FILE_CURRENT         1
#define FILE_END             2
#define MAX_FNAME_LEN    256    // Max size of field names

#if defined(WINDOWS)||defined(_WINDOWS)
#ifdef _WIN32_WCE
#include <windows.h>
#else//_WIN32_WCE
typedef __int64 LONGLONG;
typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef BOOL            *PBOOL;
typedef BOOL             *LPBOOL;
typedef BYTE            *PBYTE;
typedef BYTE             *LPBYTE;
typedef int             *PINT;
typedef int              *LPINT;
typedef WORD            *PWORD;
typedef WORD             *LPWORD;
typedef long             *LPLONG;
typedef DWORD           *PDWORD;
typedef DWORD            *LPDWORD;
typedef void             *LPVOID;
typedef const void      *LPCVOID;
typedef DWORD           FOURCC;         /* a four character code */
typedef long HRESULT;
typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;
typedef char*	LPCSTR;
#define MINCHAR     0x80        
#define MAXCHAR     0x7f        
#define MINSHORT    0x8000      
#define MAXSHORT    0x7fff      
#define MINLONG     0x80000000  
#define MAXLONG     0x7fffffff  
#define MAXBYTE     0xff        
#define MAXWORD     0xffff      
#define MAXDWORD    0xffffffff  
#ifndef VOID
#define VOID void
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
#endif

typedef signed char         INT8, *PINT8;
typedef signed short        INT16, *PINT16;
typedef signed int          INT32, *PINT32;

typedef unsigned char       UINT8, *PUINT8;
typedef unsigned short      UINT16, *PUINT16;
typedef unsigned int        UINT32, *PUINT32;
#ifdef LINUX
typedef signed long long       INT64, *PINT64;
typedef unsigned long long    UINT64, *PUINT64;
#else//LINUX
typedef signed __int64      INT64, *PINT64;
typedef unsigned __int64    UINT64, *PUINT64;
#endif//LINUX
#ifndef _MAC
typedef wchar_t WCHAR;    // wc,   16-bit UNICODE character
#else
// some Macintosh compilers don't define wchar_t in a convenient location, or define it as a char
typedef unsigned short WCHAR;    // wc,   16-bit UNICODE character
#endif
#define CONST const

typedef WCHAR *PWCHAR, *LPWCH, *PWCH;
typedef CONST WCHAR *LPCWCH, *PCWCH;
typedef WCHAR *NWPSTR, *LPWSTR, *PWSTR;
typedef PWSTR *PZPWSTR;
typedef CONST PWSTR *PCZPWSTR;
typedef WCHAR UNALIGNED *LPUWSTR, *PUWSTR;
typedef CONST WCHAR *LPCWSTR, *PCWSTR;
typedef PCWSTR *PZPCWSTR;
typedef CONST WCHAR UNALIGNED *LPCUWSTR, *PCUWSTR;

#ifdef  UNICODE                     // r_winnt

#ifndef _TCHAR_DEFINED
typedef WCHAR TCHAR, *PTCHAR;
typedef WCHAR TBYTE , *PTBYTE ;
#define _TCHAR_DEFINED
#endif /* !_TCHAR_DEFINED */

typedef LPWSTR LPTCH, PTCH;
typedef LPWSTR PTSTR, LPTSTR;
typedef LPCWSTR PCTSTR, LPCTSTR;
typedef LPUWSTR PUTSTR, LPUTSTR;
typedef LPCUWSTR PCUTSTR, LPCUTSTR;
typedef LPWSTR LP;
#define __TEXT(quote) L##quote      // r_winnt

#else   /* UNICODE */               // r_winnt

#ifndef _TCHAR_DEFINED
typedef char TCHAR, *PTCHAR;
typedef unsigned char TBYTE , *PTBYTE ;
#define _TCHAR_DEFINED
#endif /* !_TCHAR_DEFINED */


typedef LPSTR LPTCH, PTCH;
typedef LPSTR PTSTR, LPTSTR, PUTSTR, LPUTSTR;
typedef LPCSTR PCTSTR, LPCTSTR, PCUTSTR, LPCUTSTR;

#define __TEXT(quote) quote         // r_winnt

#endif /* UNICODE */                // r_winnt

typedef struct _GUID {
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char  Data4[ 8 ];
} GUID;
#endif//_WIN32_WCE
#endif//WINDOWS
int GetLastError2();
#ifndef _WIN32_WCE




typedef struct _SYSTEMTIME
{
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
} 	SYSTEMTIME;
typedef struct _FILETIME
{
	DWORD dwLowDateTime;
	DWORD dwHighDateTime;
} 	FILETIME;
void GetSystemTime(SYSTEMTIME* time);
BOOL
SystemTimeToFileTime(
					 CONST SYSTEMTIME *lpSystemTime,
					 FILETIME* lpFileTime
					 );


#endif//_WIN32_WCE
#define strdup _strdup
#define  _HRESULT_TYPEDEF_
#define E_FAIL                           _HRESULT_TYPEDEF_(0x80000008L)
#define E_POINTER                        _HRESULT_TYPEDEF_(0x80000005L)
#define VFW_E_INVALID_FILE_FORMAT        ((HRESULT)0x8004022FL)
#define S_OK                                   ((HRESULT)0x00000000L)
#define S_FALSE                                ((HRESULT)0x00000001L)
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
	((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
	((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#define mmioFOURCC(ch0, ch1, ch2, ch3)  MAKEFOURCC(ch0, ch1, ch2, ch3)
#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif
#define DUMMY_SIZE 10
#define  CREATE_MUTEX(a) (a)=VOCPFactory::CreateOneMutex();\
	voTraceNew(a,DUMMY_SIZE,MEM_CHECK);
#define  DELETE_MUTEX(a) VOCPFactory::DeleteOneMutex(a);voTraceDelete(a);a=NULL;
	

#define  CREATE_THREAD(a) (a)=VOCPFactory::CreateOneThread();\
	voTraceNew(a,DUMMY_SIZE,MEM_CHECK);
#define  DELETE_THREAD(a) VOCPFactory::DeleteOneThread(a);voTraceDelete(a);a=NULL;
	

#define  CREATE_SOCKET(a,p1,p2,p3) (a)=VOCPFactory::CreateOneSocket(p1,p2,p3);\
	voTraceNew(a,DUMMY_SIZE,MEM_CHECK);
#define  DELETE_SOCKET(a) VOCPFactory::DeleteOneSocket(a);voTraceDelete(a);(a)=NULL;
	

#ifndef MAX_PATH
#define MAX_PATH	260
#endif//MAX_PATH
int  __cdecl voGetCurrentTime();
int  INLINE GetSocketError()
{
	return IVOSocket::GetSockLastError();
}

class  CTest
{
	int test;
public:
	virtual  int next(){ return 0;};
protected:
private:
};
#ifndef _HTC 
#define MULTI_PROTOCOL 1
#endif//

#endif//__VOPDPORT_H__