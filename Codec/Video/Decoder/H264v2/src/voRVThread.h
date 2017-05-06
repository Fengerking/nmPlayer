/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003-2011			*
*																		*
************************************************************************/
/*******************************************************************************
	File:		voRVThread.h

	Contains:	Thread type define header file

	Written by:	Rock Hu

	Change History (most recent first):
	2011-10-20		HarryYang	Add atomic functions in WIN32 & Android platform
	2011-08-26		HarryYang	Modify and make most WIN32 define to correct
								Add mutex, condition variable define in WIN32
								Delete Tls function
	2011-01-26		HW			Create file

*******************************************************************************/
#ifndef VORV_THREAD_H
#define VORV_THREAD_H

//#define VPXINFINITE 10000       //10second.

#ifdef WIN32
//#define VPXINFINITE 10000       //10second.
#define VPXINFINITE INFINITE

#include <windows.h>
#include <winbase.h>
#include <process.h>

#define THREAD_FUNCTION DWORD WINAPI
#define THREAD_FUNCTION_RETURN DWORD
#define THREAD_SPECIFIC_INDEX DWORD

typedef HANDLE sem_t;
typedef sem_t VO_SEM;
//#define  
//#define VO_SEM sem_t

#define pthread_t HANDLE
#define pthread_attr_t DWORD 
#define pthread_create(thhandle,attr,thfunc,tharg)   (int)((*(thhandle)=(HANDLE)_beginthreadex(NULL,0,thfunc,tharg,0,NULL))==NULL)
#define pthread_join(thread, result) (int)(WAIT_OBJECT_0!=WaitForSingleObject(thread,VPXINFINITE) || \
	(result && !GetExitCodeThread(thread, result)) || \
	!CloseHandle(thread))
#define pthread_detach(thread) (0) //_beginthreadex should have created the detach thread
#define pthread_exit(retval) _endthreadex(retval)
#define pthread_cancel(thread) (int)(!TerminateThread(thread, -1))	//not suggest

#define thread_sleep(nms) Sleep(nms)
// #define pthread_key_create(ts_key, destructor) {ts_key = TlsAlloc();};
// #define pthread_getspecific(ts_key) TlsGetValue(ts_key)
// #define pthread_setspecific(ts_key, value) TlsSetValue(ts_key, (void *)value)
#define pthread_self() GetCurrentThread()	//notice: return pseudo handle

#define USE_CONDITION_VARIABLE 1
// #if (_WIN32_WINNT >= 0x0600) //vista+
// //use CRITICAL_SECTION will be faster
// #define pthread_mutex_t CRITICAL_SECTION
// #define pthread_mutex_init(mutex,attr) (int)(!InitializeCriticalSectionEx(mutex, 4000, CRITICAL_SECTION_NO_DEBUG_INFO))
// #define pthread_mutex_destroy(mutex) (int)(DeleteCriticalSection(mutex),0)
// #define pthread_mutex_lock(mutex) (int)(EnterCriticalSection(mutex),0)
// #define pthread_mutex_trylock(mutex) (int)(!TryEnterCriticalSection(mutex))
// #define pthread_mutex_unlock(mutex) (int)(LeaveCriticalSection(mutex),0)
//  
// #if USE_CONDITION_VARIABLE
// //In vista+ system, we can use CONDITION_VARIABLE
// #define pthread_cond_t CONDITION_VARIABLE
// #define pthread_cond_init(cond,attr) (int)(InitializeConditionVariable(cond), 0)
// #define pthread_cond_destroy(cond) (0) //no define 
// #define pthread_cond_signal(cond) (int)(WakeConditionVariable(cond), 0)
// #define pthread_cond_broadcast(cond) (int)(WakeAllConditionVariable(cond), 0)	//new support in vista+
// #define pthread_cond_wait(cond, mutex) (int)(!SleepConditionVariableCS(cond, mutex, VPXINFINITE))
// #endif
// #elif (_WIN32_WINNT >= 0x0400)
#if (_WIN32_WINNT >= 0x0400)
#if USE_CONDITION_VARIABLE
//to use Event as condition variable, we must use Mutex(it's slower) 
#define pthread_mutex_t HANDLE
#define pthread_mutex_init(mutex,attr) (int)((*(mutex) = CreateMutex(NULL,FALSE,NULL))==NULL)
#define pthread_mutex_destroy(mutex) (int)((*(mutex)) ? (CloseHandle(*(mutex)) ? (*(mutex) = NULL, 0) : 1) : 0)
#define pthread_mutex_lock(mutex) (int)(WAIT_OBJECT_0 != WaitForSingleObject(*(mutex),VPXINFINITE))
#define pthread_mutex_trylock(mutex) (int)(WAIT_OBJECT_0 != WaitForSingleObject(*(mutex),0))
#define pthread_mutex_unlock(mutex) (int)(!ReleaseMutex(*(mutex)))

#define pthread_cond_t HANDLE
#define pthread_cond_init(cond,attr) (int)((*(cond) = CreateEvent(NULL,FALSE,FALSE,NULL))==NULL)
#define pthread_cond_destroy(cond) (int)((*(cond)) ? (CloseHandle(*(cond)) ? (*(cond) = NULL, 0) : 1) : 0)
#define pthread_cond_signal(cond) (int)(!SetEvent(*(cond)))		//(int)(!PulseEvent(*(cond)))
#define pthread_cond_broadcast(cond) (int)(0)	//not support in XP
// #define pthread_cond_wait(cond, mutex) (int)(WAIT_OBJECT_0 == SignalObjectAndWait(*(mutex), *(cond),VPXINFINITE, FALSE) ? \
// 	(WAIT_OBJECT_0 != WaitForSingleObject(*(mutex),VPXINFINITE)) : 1)
#define pthread_cond_wait(cond, mutex) (int)(SignalObjectAndWait(*(mutex), *(cond),1000, FALSE) ,\
	(WAIT_OBJECT_0 != WaitForSingleObject(*(mutex),VPXINFINITE)) )

#else //USE_CONDITION_VARIABLE
//not use condition variable
//use CRITICAL_SECTION will be faster
#define pthread_mutex_t CRITICAL_SECTION
#define pthread_mutex_init(mutex,attr) (int)(InitializeCriticalSection(mutex),0)
#define pthread_mutex_destroy(mutex) (int)(DeleteCriticalSection(mutex),0)
#define pthread_mutex_lock(mutex) (int)(EnterCriticalSection(mutex),0)
#define pthread_mutex_trylock(mutex) (int)(!TryEnterCriticalSection(mutex))
#define pthread_mutex_unlock(mutex) (int)(LeaveCriticalSection(mutex),0)
#endif //USE_CONDITION_VARIABLE

#endif //_WIN32_WINNT

#define pause(voidpara) __asm PAUSE

#define sem_t HANDLE
#define vo_sem_init(name, sem, sem_attr1, sem_init_value) (int)((*(sem) = CreateSemaphore(NULL,(sem_init_value),LONG_MAX,NULL))==NULL)
#define vo_sem_wait(sem) (int)(WAIT_OBJECT_0 != WaitForSingleObject(*(sem),VPXINFINITE))
#define vo_sem_post(sem) (int)(!ReleaseSemaphore(*(sem), 1, NULL))
#define vo_sem_destroy(name, sem) (int)((*(sem)) ? (CloseHandle(*(sem)) ? *(sem) = (HANDLE)NULL : (HANDLE)1) : 0)

//Atomic functions
// #define VO_ATOMIC_Inc(addr) (InterlockedIncrement(addr)-1)	//return the old value
// #define VO_ATOMIC_Dec(addr) (InterlockedDecrement(addr)+1)	//return the old value
// #define VO_ATOMIC_Cmpxchg(addr, newV, cmpV) (InterlockedCompareExchange(addr, newV, cmpV),0)	//return 0

#elif defined(VOWINCE)
#define VPXINFINITE 200000       //10second.
#include <windows.h>
#include <winbase.h>
#define THREAD_FUNCTION DWORD WINAPI
#define THREAD_FUNCTION_RETURN DWORD
#define THREAD_SPECIFIC_INDEX DWORD


#define pthread_t HANDLE
#define pthread_attr_t DWORD 
#define pthread_create(thhandle,attr,thfunc,tharg)   (*thhandle=(HANDLE)CreateThread(attr,0,thfunc,tharg,0,0))
#define pthread_join(thread, result) (int)(WAIT_OBJECT_0!=WaitForSingleObject(thread,VPXINFINITE) || \
	(result && !GetExitCodeThread(thread, result)) || \
	!CloseHandle(thread))
#define pthread_detach(thread) if(thread!=NULL)CloseHandle(thread)
#define pthread_exit(retval) ExitThread(retval)
#define pthread_cancel(thread) (int)(!TerminateThread(thread, -1))	//not suggest
#define thread_sleep(nms) Sleep(nms)
#define pthread_self() GetCurrentThreadId()

#define pthread_mutex_t HANDLE
#define pthread_mutex_init(mutex,attr) (int)((*(mutex) = CreateMutex(NULL,FALSE,NULL))==NULL)
#define pthread_mutex_destroy(mutex) (int)((*(mutex)) ? (CloseHandle(*(mutex)) ? (*(mutex) = NULL, 0) : 1) : 0)
#define pthread_mutex_lock(mutex) (int)(WAIT_OBJECT_0 != WaitForSingleObject(*(mutex),VPXINFINITE))
#define pthread_mutex_trylock(mutex) (int)(WAIT_OBJECT_0 != WaitForSingleObject(*(mutex),0))
#define pthread_mutex_unlock(mutex) (int)(!ReleaseMutex(*(mutex)))

#define pthread_cond_t HANDLE
#define pthread_cond_init(cond,attr) (int)((*(cond) = CreateEvent(NULL,FALSE,FALSE,NULL))==NULL)
#define pthread_cond_destroy(cond) (int)((*(cond)) ? (CloseHandle(*(cond)) ? (*(cond) = NULL, 0) : 1) : 0)
#define pthread_cond_signal(cond) (int)(!SetEvent(*(cond)))		//(int)(!PulseEvent(*(cond)))
#define pthread_cond_broadcast(cond) (int)(0)	//not support in XP
#define pthread_cond_wait(cond, mutex) \
		SetEvent(*(mutex)),((int)(WAIT_OBJECT_0 == WaitForSingleObject(*(cond),VPXINFINITE) ? \
			(WAIT_OBJECT_0 != WaitForSingleObject(*(mutex),VPXINFINITE)) : 1))

#define sem_t HANDLE
#define pause(voidpara) __asm PAUSE
#define vo_sem_init(name, sem, sem_attr1, sem_init_value) (int)((*sem = CreateEvent(NULL,FALSE,FALSE,NULL))==NULL)
#define vo_sem_wait(sem) (int)(WAIT_OBJECT_0 != WaitForSingleObject(*sem,VPXINFINITE))
#define vo_sem_post(sem) SetEvent(*sem)
#define vo_sem_destroy(name, sem) if(*sem)((int)(CloseHandle(*sem))==TRUE)

#define VO_SEM sem_t
#elif defined(VOANDROID)
#define VPXINFINITE 200000       //10second.
#include<pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
// #include <cutils/atomic.h>

#define VO_SEM sem_t

#define THREAD_FUNCTION  void*
#define THREAD_FUNCTION_RETURN void*

#define thread_sleep(nms) usleep(nms*1000)
#define pthread_cancel(thread)	{}		//Android not support this function in it's "pthread" lib

#define vo_sem_init(name, sem, sem_attr1, sem_init_value)  sem_init(sem, sem_attr1, sem_init_value)
#define vo_sem_wait(sem) sem_wait(sem)
#define vo_sem_post(sem) sem_post(sem)
#define vo_sem_destroy(name, sem) sem_destroy(sem)

// Atomic functions
// #define VO_ATOMIC_Inc(addr) android_atomic_inc(addr)
// #define VO_ATOMIC_Dec(addr) android_atomic_dec(addr)
// #define VO_ATOMIC_Cmpxchg(addr, newV, cmpV) android_atomic_cmpxchg(addr, newV, cmpV)

#elif defined(_IOS)
#define VPXINFINITE 200000       //10second.
#include<pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define THREAD_FUNCTION  void*
#define THREAD_FUNCTION_RETURN void*
#define thread_sleep(nms) usleep(nms*1000)

#define VO_SEM sem_t*

#define vo_sem_init(name, sem, sem_attr1, sem_init_value) ((*sem) = sem_open( name,O_CREAT|O_EXCL,0644,sem_init_value ))
#define vo_sem_wait(sem) sem_wait(*sem)
#define vo_sem_post(sem) sem_post(*sem)
#define vo_sem_destroy(name, sem) sem_close(*sem); sem_unlink(name);

#elif defined(_MAC_OS)

#define VPXINFINITE 200000       //10second.
#include<pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define THREAD_FUNCTION  void*
#define THREAD_FUNCTION_RETURN void*
#define thread_sleep(nms) usleep(nms*1000)

#define VO_SEM sem_t*

#define vo_sem_init(name, sem, sem_attr1, sem_init_value) ((*sem) = sem_open( name,O_CREAT|O_EXCL,0644,sem_init_value ))
#define vo_sem_wait(sem) sem_wait(*sem)
#define vo_sem_post(sem) sem_post(*sem)
#define vo_sem_destroy(name, sem) sem_close(*sem); sem_unlink(name);

#endif

#if ARCH_X86 || ARCH_X86_64
#include "vpx_ports/x86.h"
#else
#define x86_pause_hint()
#endif

#endif
