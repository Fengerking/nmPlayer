////////////////////////////////////////////////////////////////////////////
// vocrsapis.cpp
//   designed for transforming some system apis from windwos to Linux
//   copyright visualon
//   author daivdoNe
//	 time 2007/02/26
///////////////////////////////////////////////////////////////////////////

#include <errno.h>
//#include <iostream>
#include <sched.h>
#include <sys/time.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <semaphore.h>
#include <assert.h>

#include "vocrsapis.h"
#include "Lin2WinBase.h"
#include "Lin2WinPrivate.h"

#define SUBTHREAD_POLICY SCHED_RR

DWORD timeGetTime(){
	struct timeval tval;
	gettimeofday(&tval, NULL);
	return tval.tv_sec*1000 + tval.tv_usec/1000;
}

DWORD GetTickCount(){
	return timeGetTime();
}

int get_thread_policy(const pthread_attr_t &attr){
	int policy;
	int rs = pthread_attr_getschedpolicy(&attr, &policy);
	if (rs != 0) return -1;
#if 1//ndef MAC
	switch (policy)
	{
		case SCHED_FIFO:
			break;
		case SCHED_RR:
			break;
		case SCHED_OTHER:
			break;
		default:
			break;
	}

	return policy;
#else
	return 0;
#endif
}

void show_thread_priority(const pthread_attr_t &attr,const int policy){
	int rs = sched_get_priority_max(policy);
	if (rs == -1) return;
	//cout << "max_priority = " << priority << endl;

	rs = sched_get_priority_min(policy);
	if (rs == -1) return;
	//cout << "min_priority = " << priority << endl;

	get_thread_priority(attr);
}

int get_thread_priority(const pthread_attr_t &attr){
	struct sched_param param;

	int rs = pthread_attr_getschedparam(&attr, &param);
	if (rs != 0) return -1;
#ifdef MAC
	return param.sched_priority;
#else//MAC
	return param.__sched_priority;
#endif//MAC
}

void set_thread_policy(pthread_attr_t &attr,const int policy){
	int rs = pthread_attr_setschedpolicy(&attr, policy);
	if (rs != 0) return ;
	get_thread_policy(attr);
}

void set_thread_priority(const int priority){

	int rs;
	pthread_attr_t attr;

	rs = pthread_attr_init(&attr);
	if (rs != 0)
		return ;

	/*size_t size = 0;
	pthread_attr_getstacksize(&attr, &size);
	VOINFON(size);
	size = 8388608;//4MB
	pthread_attr_setstacksize(&attr, size);
	pthread_attr_getstacksize(&attr, &size);
	VOINFON(size);*/

	int policy;
	rs = pthread_attr_getschedpolicy(&attr, &policy);
	if (rs != 0) return ;

	rs= pthread_attr_setschedpolicy(&attr, SUBTHREAD_POLICY);
	if (rs != 0) return ;

	get_thread_policy(attr);
	get_thread_priority(attr);

	struct sched_param param;
#ifdef MAC
	param.sched_priority= priority;
#else//MAC
	
	param.__sched_priority = priority;
#endif//MAC
	//rs = pthread_setschedparam(pthread_self(), SCHED_FIFO, 30); 
	rs = pthread_attr_setschedparam(&attr, &param);
	if (rs != 0) return ;

	get_thread_priority(attr);
	pthread_attr_destroy(&attr);
}


void* GetProcAddress(HMODULE hm, LPCSTR proc){

	if (hm==NULL || proc == NULL) return NULL;

	return dlsym(hm, proc);
}

void* LoadLibrary(LPCSTR lib){
	
	void* rc = dlopen(lib, RTLD_LAZY);
#if defined(_DEBUG)
	if (rc == NULL)
		printf("%s\n", dlerror());
#endif
	return rc;
}

void FreeLibrary(HMODULE hm){

	dlclose(hm);
}

DWORD GetModuleFileName(HMODULE h, LPSTR buf, DWORD size){

	DWORD rs = readlink("/proc/self/exe", buf, size);
	if (rs<0 || rs>=size)
		return rs;

	buf[rs] = '\0';
	return rs;
}

BOOL	SetRect(LPRECT pr, int l, int t, int r, int b){
	if (pr == NULL) return FALSE;
	pr->left = l;
	pr->top = t;
	pr->right = r;
	pr->bottom = b;

	return TRUE;
}

BOOL 	IsRectEmpty(const RECT &rc){
	if (rc.left == rc.right && 
			rc.top == rc.bottom)
		return TRUE; 
		
	return FALSE;
}

BOOL 	SetRectEmpty(LPRECT r){
	if (r == NULL)
		return FALSE;

	r->left = r->top = r->right = r->bottom = 0;
	return TRUE;
}	

BOOL	DeleteObject(HOBJ p){

	if (NULL == p) return FALSE;
	free(p);
	p = NULL;
	//SAFE_DELETE(p);

	return TRUE;
}

void ZeroMemory(void* pm, SIZE_T s){
	memset(pm, 0, s);
}

int	MultiByteToWideChar(
		IN	UINT CodePage,
		IN	DWORD dwFlags,
		IN	const char* lpMultiByteStr,
		IN 	int	cbMultiByte,
		OUT	LPWSTR  lpWideCharStr,
		IN	int	cchWideChar){
#if 1//defined(_ADI_DSP)
	return -1;
#else
	return mbstowcs(lpWideCharStr, (char*)lpMultiByteStr, cbMultiByte);
#endif
}

bool IsEqualGUID(REFGUID g1, REFGUID g2){

	if (g1.Data1 != g2.Data1 ||
			g1.Data2 != g2.Data2 ||
			g1.Data3 != g2.Data3 ||
			memcmp(g1.Data4, g2.Data4, 8))
		return false;

	return true;
}


BOOL	OffsetRect(LPRECT r, int dx, int dy){

	if (NULL == r) return FALSE;

	r->left += dx;
	r->right += dx;
	r->top += dy;
	r->bottom  += dy;
	return TRUE;
}

BOOL	PtInRect(IN CONST RECT *pr, IN POINT pt){

	if (pt.x>=pr->left && pt.x<=pr->right && pt.y>=pr->top && pt.y<=pr->bottom)
		return TRUE;

	return FALSE;
}


void GlobalMemoryStatus(IN OUT LPMEMORYSTATUS lpBuffer){
	FILE* pf = fopen("/proc/meminfo", "r");
	if (pf == NULL) return;

	char buf[100];
	memset(buf, 0, 100);
	if (NULL == fgets(buf, 100, pf)) return;
	if (NULL == fgets(buf, 100, pf)) return;
	
	char num[100], *psz;
	memset(num, 0, 100);
	psz = strstr(buf+8, "kB"); // 8: length of MemFree:
	strncpy(num, buf+8, psz-buf-8); //
	lpBuffer->dwAvailPhys = atoi(num) * 1024;
}



void OutputDebugString(const char* inBuf)
{
	printf("%s", inBuf);
}

int WideCharToMultiByte( UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr,int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte,LPCSTR lpDefaultChar,LPBOOL lpUsedDefaultChar)
{
#if 1//defined(_ADI_DSP)
	return -1;
#else
	return wcstombs(lpMultiByteStr, lpWideCharStr, cbMultiByte);
#endif
}

DWORD WaitForSingleObject(HANDLE hHandle,DWORD dwMilliseconds)
{
	DWORD nRet = -1;
	if(hHandle)
	{
		lh_info* pInfo = (lh_info*)hHandle;
		switch(pInfo->mType)
		{
		case LH_EVENT:
			nRet = _wait_event_handle(hHandle, dwMilliseconds);
			break;
		case LH_THREAD:
			nRet = _wait_thread_handle(hHandle, dwMilliseconds);
			break;
		default:
			assert(0);
			break;
		}
	}
    return nRet;
}


BOOL CloseHandle(HANDLE hObject)
{
	BOOL bOk = FALSE;
	if(hObject)
	{
		lh_info* pInfo = (lh_info*)hObject;
		switch(pInfo->mType)
		{
		case LH_EVENT:
			bOk = _close_event_handle(hObject);
			break;
		case LH_THREAD:
			bOk = _close_thread_handle(hObject);
			break;
		case LH_FILE:
			bOk = _close_file_handle(hObject);
			break;
		default:
			assert(0);
			break;
		}
	}
	return bOk;
}
