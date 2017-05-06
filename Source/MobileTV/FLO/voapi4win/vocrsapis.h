////////////////////////////////////////////////////////////////////////////
// vocrsapis.h
//   designed for transforming some system apis from windwos to Linux
//   copyright visualon
//   author daivdoNe
//	 time 2007/02/26
///////////////////////////////////////////////////////////////////////////

#if !defined __VO_CRS_APIS_H__
#define __VO_CRS_APIS_H__
#include "vocrstypes.h"
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>

#if 0// !defined(Sleep)
#define Sleep(x) usleep(1000*x)
#endif

#if defined(__cplusplus)
extern "C" {
#endif
	
DWORD timeGetTime();
DWORD GetTickCount();

int get_thread_policy(const pthread_attr_t &attr);
void set_thread_policy(pthread_attr_t &attr,const int policy);
void show_thread_priority(const pthread_attr_t &attr,const int policy);
int get_thread_priority(const pthread_attr_t &attr);
void set_thread_priority(const int priority);

void* LoadLibrary(LPCSTR);
void FreeLibrary(HMODULE);
void* GetProcAddress(HMODULE, LPCSTR);

DWORD GetModuleFileName(HMODULE, LPSTR, DWORD);
BOOL 	IsRectEmpty(const RECT&);
BOOL 	SetRectEmpty(PRECT);
BOOL	SetRect(PRECT, int, int, int, int);
BOOL	OffsetRect(PRECT, int, int);
BOOL	PtInRect(IN CONST RECT *lprect, IN POINT pt);
BOOL	DeleteObject(HOBJ);
void	ZeroMemory(void*, SIZE_T);

#define CP_ACP		0
#define CP_OEMCP	1
#define CP_MACCP	2
#define CP_THREAD_ACP	3
#define CP_SYMBOL	42

// added by gtxia 2007-12-29 no effect	
#define CP_UTF8     65001	
int	MultiByteToWideChar(
	IN	UINT CodePage,
	IN	DWORD dwFlags,
	IN	const char* lpMultiByteStr,
	IN 	int	cbMultiByte,
	OUT	LPWSTR  lpWideCharStr,
	IN	int	cchWideChar);

bool IsEqualGUID(REFGUID, REFGUID);

typedef struct _MEMORYSTATUS {
    DWORD dwLength;
    DWORD dwMemoryLoad;
    SIZE_T dwTotalPhys;
    SIZE_T dwAvailPhys;
    SIZE_T dwTotalPageFile;
    SIZE_T dwAvailPageFile;
    SIZE_T dwTotalVirtual;
    SIZE_T dwAvailVirtual;
} MEMORYSTATUS, *LPMEMORYSTATUS;
void GlobalMemoryStatus(IN OUT LPMEMORYSTATUS);


// added by gtxia 2007-8-15
#define _tfopen      fopen
#define closesocket  close
#define _strnicmp    strncasecmp
#define _stricmp     strcasecmp
#define ioctlsocket  ioctl	

// added by gtxia 2007-12-12
void OutputDebugString(const char* inBuffer);

// added by gtxia 2007-12-29
int WideCharToMultiByte( UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr,int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte,LPCSTR lpDefaultChar,LPBOOL lpUsedDefaultChar);


// added by gtxia 2008-1-11
DWORD WaitForSingleObject(
  IN  HANDLE hHandle,
  IN  DWORD dwMilliseconds
);
BOOL CloseHandle(
  IN  HANDLE hObject
);	
	
#if defined(__cplusplus)
}
#endif

#endif //__VO_CRS_APIS_H__


