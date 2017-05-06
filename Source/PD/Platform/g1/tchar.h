/**
 * VisualOn Base Types 
 * @created 4/29/2006 by david
 * modified 2007/01/24
 */

#if !defined __VO_CRS_TYPES_H__
#define __VO_CRS_TYPES_H__

#if defined(LINUX)
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <ctype.h>
#else
#include <tchar.h>
#endif

#if defined __VOPRJ_LO__  || defined __VOPRJ_MTK__ || defined __VORPJ_TOM__ 
#define VOSCREEN_WIDTH		480
#define VOSCREEN_HEIGHT		272
#elif defined __VOPRJ_GP__// for greenphone
#define VOSCREEN_WIDTH		240
#define VOSCREEN_HEIGHT		320
#elif defined __VOPRJ_SS__ // for spansion
#define VOSCREEN_WIDTH		480
#define VOSCREEN_HEIGHT		640
#elif defined __VOPRJ_JF__ || defined __VOPRJ_GI2__
#define VOSCREEN_WIDTH		320
#define VOSCREEN_HEIGHT		240
#elif defined __VOPRJ_TI__ // for Texas Instruments
#define VOSCREEN_WIDTH		640
#define VOSCREEN_HEIGHT		480	
#elif defined __VOPRJ_GI__ // for Giant
#define VOSCREEN_WIDTH		800
#define VOSCREEN_HEIGHT		480	
#else
#define VOSCREEN_WIDTH		480
#define VOSCREEN_HEIGHT		272
#endif

#if !defined __VO_BASE_TYPES_DEFINED__
#define __VO_BASE_TYPES_DEFINED__
typedef signed char int8, INT8;
typedef signed short int16, INT16;
typedef signed long int32, INT32;
typedef signed long LONG; 
typedef unsigned char uint8, UINT8;
typedef unsigned short uint16, UINT16;
typedef unsigned long uint32, UINT32;
typedef unsigned char BYTE;
#endif

typedef unsigned char bit1;
typedef unsigned char bit2;
typedef unsigned char bit3;
typedef unsigned char bit4;
typedef unsigned char bit5;
typedef unsigned char bit6;
typedef unsigned char bit7;
typedef unsigned char bit8;
typedef unsigned short bit9;
typedef unsigned short bit10;
typedef unsigned short bit11;
typedef unsigned short bit12;
typedef unsigned short bit13;
typedef unsigned short bit14;
typedef unsigned short bit15;
typedef unsigned short bit16;
typedef unsigned long bit17;
typedef unsigned long bit18;
typedef unsigned long bit19;
typedef unsigned long bit20;
typedef unsigned long bit21;
typedef unsigned long bit22;
typedef unsigned long bit23;
typedef unsigned long bit24;
typedef unsigned long bit25;
typedef unsigned long bit26;
typedef unsigned long bit27;
typedef unsigned long bit28;
typedef unsigned long bit29;
typedef unsigned long bit30;
typedef unsigned long bit31;
typedef unsigned long bit32;
typedef unsigned long ULONG;
typedef long LONG_PTR;
#if defined(_WIN32_WCE) || defined(_WIN32)
////////////////////////////////////////////////////////////////////
// for windows
typedef __int64 int64;
typedef unsigned __int64 uint64;
typedef unsigned __int64 bit33;
typedef unsigned __int64 bit34;
typedef unsigned __int64 bit35;
typedef unsigned __int64 bit36;
typedef unsigned __int64 bit64;
//end for windows
////////////////////////////////////////////////////////////////////
#elif defined(LINUX)
////////////////////////////////////////////////////////////////////
// for linux

#define far
#define near

typedef void* HBITMAP;
typedef void* HDC;
typedef void* HWND;
typedef void* HFONT;
typedef void* HANDLE;
typedef HANDLE HINSTANCE;
typedef HANDLE HMODULE;
typedef int BOOL;
typedef unsigned int UINT,UINT_PTR, *PUINT;
typedef unsigned char UCHAR, *PBYTE, *LPBYTE;
typedef unsigned short USHORT, WORD,*PWORD;
typedef unsigned long DWORD,DWORD_PTR,*PDWORD,*PDWORD_PTR;
typedef long long int64;
typedef int64 LONGLONG;
typedef DWORD FOURCC;
typedef DWORD far *LPDWORD;
typedef long long __int64;
typedef unsigned long long uint64, ULONGLONG;
typedef ULONGLONG* PULONGLONG;
typedef unsigned long long bit33;
typedef unsigned long long bit34;
typedef unsigned long long bit35;
typedef unsigned long long bit36;
typedef unsigned long long bit64;
typedef DWORD_PTR SIZE_T, *PSIZE_T;
typedef void *PVOID;
typedef void far 	*LPVOID;
typedef void near *HOBJ;

typedef UINT_PTR  WPARAM;
typedef LONG_PTR	LPARAM;

typedef LONG_PTR HRESULT;
typedef LONG_PTR LRESULT;
typedef float FLOAT;

#if !defined MAXWORD 
#define MAXWORD 65535
#endif

#if !defined MAXDWORD
#define MAXDWORD 0xFFFFFFFF
#endif

#if defined(MIDL_PASS)
typedef struct _LARGE_INTEGER{
#else
typedef union _LARGE_INTEGER{
	struct{
		DWORD LowPart;
		LONG HighPart;
	};
	struct{
		DWORD LowPart;
		LONG HighPart;
	}u;
#endif
	LONGLONG QuadPart;
} LARGE_INTEGER;

typedef LARGE_INTEGER *PLARGE_INTEGER;

typedef struct _VOPOS{
	int x; int y;
}VOPOS, *PVOPOS;

typedef struct _VORECT{
	VOPOS vp;
	int 	w;
	int		h;
}VORECT, *PVORECT;

#define __cdecl 
#define MAX_PATH 256

#define GENERIC_READ		(0x80000000L)
#define GENERIC_WRITE		(0x40000000L)
#define GENERIC_EXECUTE	(0x20000000L)
#define GENERIC_ALL			(0x10000000L)

#define FILE_SHARE_READ		0x00000001
#define FILE_SHARE_WRITE  0x00000002
#define FILE_SHARE_DELETE 0x00000004

#define CREATE_NEW				1
#define CREATE_ALWAYS 		2
#define OPEN_EXISTING 		3
#define OPEN_ALWAYS				4
#define TRUNCATE_EXISTING 5

#if !defined(SAFE_DELETE)
#define SAFE_DELETE(x)\
	if (x){\
		delete x;\
		x = NULL;\
	}
#endif // SAFE_DELETE

#if !defined(SAFE_ARRAYDELETE)
#define SAFE_ARRAYDELETE(x)\
	if (x){\
		delete []x;\
		x = NULL;\
	}
#endif // SAFE_ARRAYDELETE

#if !defined(min)
#define min(x,y) ((x)>=(y)) ? (y):(x)
#endif

#if !defined(max)
#define max(x,y) ((x)>=(y)) ? (x):(y)
#endif

#define VOINFO(x) { \
	printf("[David] %s->%d: %s\n", __FILE__,  __LINE__, (char*)x); }

#define VOINFON(x) { \
	printf("[David] %s->%d: %d\n", __FILE__,  __LINE__, (int)x); }

#define VOINFOX(x) { \
	printf("[David] %s->%d: 0x%08x\n", __FILE__,  __LINE__, (unsigned long)x); }

#define VOINFOF(x) { \
	printf("[David] %s->%d: %2.3f\n", __FILE__,  __LINE__, (double)x); }

#define TRUE 1
#define FALSE 0
#define FAR far
#define NEAR near
#define IN 
#define OUT
#define INFINITE 0xFFFFFFFF

// added by gtxia 2007-4-20
#define CALLBACK
#define PATH_SEPARATOR '/'
#define PATH_SEPARATORS "/"

//#define NULL 0

typedef struct _GUID{
  unsigned long   Data1;
  unsigned short  Data2;
  unsigned short  Data3;
  unsigned char   Data4[8];
} GUID;

#define REFGUID const GUID &

#ifndef mmioFOURCC
#define mmioFOURCC( ch0, ch1, ch2, ch3 )				\
		( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |	\
		( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )
#endif

#define FOURCC_RIFF     mmioFOURCC('R', 'I', 'F', 'F')
#define FOURCC_LIST     mmioFOURCC('L', 'I', 'S', 'T')
#define MINLONG		0x80000000
#define MAXLONG		0x7fffffff

#if !defined(EXTERN_C)
#if defined(__cplusplus)
#define EXTERN_C extern "C"
#else
#define EXTERN_C extern
#endif
#endif

#if defined __GNUC__
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
				static const GUID name __attribute__ ((unused)) \
							= {l, w1, w2, {b1, b2, b3, b4, b5,b6,b7,b8}}
#else
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
				static const GUID name \
							= {l, w1, w2, {b1, b2, b3, b4, b5,b6,b7,b8}}
#endif

#if !defined EXTERN_GUID
#define EXTERN_GUID(g,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
				 DEFINE_GUID(g,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) 
#endif


#if !defined(MAKEFOURCC)
    #define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif //defined(MAKEFOURCC)

#if !defined(__RECT__)
#define __RECT__
typedef struct tagRECT{
	long left;
	long top;
	long right;
	long bottom;
} RECT, *PRECT, NEAR* NPRECT, FAR* LPRECT ;
#endif //__RECT__

typedef struct tagPOINT{
	LONG x;
	LONG y;
} POINT, *PPOINT, NEAR* NPPOINT, FAR* LPPOINT;

#if !defined(CONST)
#define CONST const
#endif

#define S_OK  		((HRESULT)0x00000000L)
#define S_FALSE  	((HRESULT)0x00000001L)
#define E_FAIL		S_FALSE
#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_
typedef struct tWAVEFORMATEX{
    WORD    wFormatTag;        /* format type */
    WORD    nChannels;         /* number of channels (i.e. mono, stereo...) */
    DWORD   nSamplesPerSec;    /* sample rate */
    DWORD   nAvgBytesPerSec;   /* for buffer estimation */
    WORD    nBlockAlign;       /* block size of data */
    WORD    wBitsPerSample;    /* Number of bits per sample of mono data */
    WORD    cbSize;            /* The count in bytes of the size of
                                    extra information (after cbSize) */
} WAVEFORMATEX;
typedef WAVEFORMATEX       *PWAVEFORMATEX;
typedef WAVEFORMATEX NEAR *NPWAVEFORMATEX;
typedef WAVEFORMATEX FAR  *LPWAVEFORMATEX;
typedef const WAVEFORMATEX *LPCWAVEFORMATEX;
#endif /* _WAVEFORMATEX_ */

#define WAVE_FORMAT_PCM 1

typedef wchar_t WCHAR, *PWCHAR;
typedef wchar_t far * LPWSTR;
typedef const wchar_t far * LPCWSTR;
#if defined(__UNICODE__)
/////////////////////////////////////////////////////////////////
// for wide charactor
typedef wchar_t _TCHAR; 
typedef wchar_t TCHAR, *PTCHAR;
typedef wchar_t far * LPSTR, LPTSTR;
typedef const LPSTR LPCSTR;
typedef LPCWSTR LPCTSTR;
#define __T(x) L##x
#define _tprintf wprintf
#define _tcslen wcslen
#define _tcscpy wcscpy
#define _tcscat wcscat
#define _tcsupr _wcsupr
#define _tcscmp wcscmp
#define _tcsrchr wcsrchr
#define _tcsicmp wcsicmp
#define _stprintf swprintf
#define _tcsstr strstr
//end for UNICODE
////////////////////////////////////////////////////////////////
#else
/////////////////////////////////////////////////////////////////
// normal as  ANSI 
typedef char _TCHAR;
typedef char TCHAR, *PTCHAR;
//typedef char WCHAR;
typedef TCHAR*	LPSTR,LPTSTR;
typedef const char *LPCSTR;
typedef LPCSTR LPCTSTR;
#define __T(x) x 
#define _tprintf printf 
#define _tcslen strlen
#define _tcsclen strlen
#define _tcsrchr strrchr
#define _tcscpy strcpy
#define	 tcscpy  strcpy
#define _tcscat strcat
#define _tcscmp strcmp
#define _tcsicmp strcasecmp
#define _stprintf sprintf
#define _strnicmp strncasecmp
#define _stricmp	strcasecmp
#define _tcsnicmp strncasecmp
#define _strcmp strcmp
#define _strdup strdup
#define _tcsstr strstr
#define _tcsupr(x){\
	int i,l=strlen(x);\
	for (i=0;i<l;++i)\
		x[i]=toupper(x[i]);\
}	
//end for ANSI
/////////////////////////////////////////////////////////////////
#endif // __UNICODE__

#define _T(x) __T(x)
#define _TEXT(x) __T(x) 
#define TEXT(x) __T(x) 

#endif //defined(_WIN32_WCE) || defined(_WIN32)

#endif //__VO_CRS_TYPES_H__


