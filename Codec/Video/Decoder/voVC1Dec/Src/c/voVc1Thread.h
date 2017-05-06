#ifndef VP8_THREAD_H
#define VP8_THREAD_H
//#define VOWINXP 1

#define VPXINFINITE 100000       //10second.
#ifdef VOWINXP
    #include <windows.h>
	#include <winbase.h>
	#define THREAD_FUNCTION DWORD WINAPI
	#define THREAD_FUNCTION_RETURN DWORD
	#define THREAD_SPECIFIC_INDEX DWORD
	#define pthread_t HANDLE
	#define pthread_attr_t DWORD 
    #define pthread_create(thhandle,attr,thfunc,tharg)   (*thhandle=(HANDLE)CreateThread(attr,0,thfunc,tharg,0,0))
	#define pthread_join(thread, result) ((WaitForSingleObject((thread),VPXINFINITE)!=WAIT_OBJECT_0) || !CloseHandle(thread))
	#define pthread_detach(thread) if(thread!=NULL)CloseHandle(thread)
	#define thread_sleep(nms) Sleep(nms)
	#define pthread_cancel(thread) ExitThread(thread)
	#define ts_key_create(ts_key, destructor) {ts_key = TlsAlloc();};
	#define pthread_getspecific(ts_key) TlsGetValue(ts_key)
	#define pthread_setspecific(ts_key, value) TlsSetValue(ts_key, (void *)value)
	#define pthread_self() GetCurrentThreadId()

#define USE_CONDITION_VARIABLE 1
#if (_WIN32_WINNT >= 0x0600) //vista+
//use CRITICAL_SECTION will be faster
#define pthread_mutex_t CRITICAL_SECTION
#define pthread_mutex_init(mutex,attr) (int)(!InitializeCriticalSectionEx(mutex, 4000, CRITICAL_SECTION_NO_DEBUG_INFO))
#define pthread_mutex_destroy(mutex) (int)((mutex) ? (DeleteCriticalSection(mutex),0) : 0)
#define pthread_mutex_lock(mutex) (int)(EnterCriticalSection(mutex),0)
#define pthread_mutex_trylock(mutex) (int)(!TryEnterCriticalSection(mutex))
#define pthread_mutex_unlock(mutex) (int)(LeaveCriticalSection(mutex),0)

#if USE_CONDITION_VARIABLE
//In vista+ system, we can use CONDITION_VARIABLE
#define pthread_cond_t CONDITION_VARIABLE
#define pthread_cond_init(cond,attr) (int)(InitializeConditionVariable(cond), (cond)==NULL)
#define pthread_cond_destroy(cond) (0) //no define 
#define pthread_cond_signal(cond) (int)(WakeConditionVariable(cond), 0)
#define pthread_cond_broadcast(cond) (int)(WakeAllConditionVariable(cond), 0)	//new support in vista+
#define pthread_cond_wait(cond, mutex) (int)(!SleepConditionVariableCS(cond, mutex, VPXINFINITE))
#endif
#elif (_WIN32_WINNT >= 0x0400)
#if USE_CONDITION_VARIABLE
//to use Event as condition variable, we must use Mutex(it's slower) 
#define pthread_mutex_t HANDLE
#define pthread_mutex_init(mutex,attr) (int)((*(mutex) = CreateMutex(NULL,FALSE,NULL))==NULL)
#define pthread_mutex_destroy(mutex) (int)((*(mutex)) ? (CloseHandle(*(mutex)) ? *(mutex) = NULL : 1) : 0)
#define pthread_mutex_lock(mutex) (int)(WAIT_OBJECT_0 != WaitForSingleObject(*(mutex),VPXINFINITE))
#define pthread_mutex_trylock(mutex) (int)(WAIT_OBJECT_0 != WaitForSingleObject(*(mutex),0))
#define pthread_mutex_unlock(mutex) (int)(!ReleaseMutex(*(mutex)))

#define pthread_cond_t HANDLE
#define pthread_cond_init(cond,attr) (int)((*(cond) = CreateEvent(NULL,FALSE,FALSE,NULL))==NULL)
#define pthread_cond_destroy(cond) (int)((*(cond)) ? (CloseHandle(*(cond)) ? *(cond) = NULL : 1) : 0)
#define pthread_cond_signal(cond) (int)(!SetEvent(*(cond)))		//(int)(!PulseEvent(*(cond)))
#define pthread_cond_wait(cond, mutex) (int)(WAIT_OBJECT_0 == SignalObjectAndWait(*(mutex), *(cond),VPXINFINITE, FALSE) ? \
	(WAIT_OBJECT_0 != WaitForSingleObject(*(mutex),VPXINFINITE)) : 1)

#else //USE_CONDITION_VARIABLE
//not use condition variable
//use CRITICAL_SECTION will be faster
#define pthread_mutex_t CRITICAL_SECTION
#define pthread_mutex_init(mutex,attr) (int)(InitializeCriticalSection(mutex),(mutex)==NULL)
#define pthread_mutex_destroy(mutex) (int)((mutex) ? (DeleteCriticalSection(mutex),0) : 0)
#define pthread_mutex_lock(mutex) (int)(EnterCriticalSection(mutex),0)
#define pthread_mutex_trylock(mutex) (int)(!TryEnterCriticalSection(mutex))
#define pthread_mutex_unlock(mutex) (int)(LeaveCriticalSection(mutex),0)
#endif //USE_CONDITION_VARIABLE

#endif //_WIN32_WINNT
	#define pause(voidpara) __asm PAUSE
    #define sem_t HANDLE
    #define sem_init(sem, sem_attr1, sem_init_value) (int)((*(sem) = CreateSemaphore(NULL,(sem_init_value),LONG_MAX,NULL))==NULL)
    #define sem_wait(sem) (int)(WAIT_OBJECT_0 != WaitForSingleObject(*(sem),VPXINFINITE))
    #define sem_trywait(sem) (int)(WAIT_OBJECT_0 != WaitForSingleObject(*(sem),0))
    #define sem_post(sem) (int)(!ReleaseSemaphore(*(sem), 1, NULL))
    //#define SEM_GETVALUE(sem, pval) GetSemaphoreValue(sem, pval)
	#define sem_destroy(sem)    CloseHandle(*(sem))

	#define thread_sleep(nms) Sleep(nms)
#elif VOWINCE
	#include <windows.h>
	#include <winbase.h>
	#define THREAD_FUNCTION DWORD WINAPI
	#define THREAD_FUNCTION_RETURN DWORD
	#define THREAD_SPECIFIC_INDEX DWORD

	#define pthread_t HANDLE
	#define pthread_attr_t DWORD 
    #define pthread_create(thhandle,attr,thfunc,tharg)   (*thhandle=(HANDLE)CreateThread(attr,0,thfunc,tharg,0,0))
	#define pthread_join(thread, result) ((WaitForSingleObject((thread),VPXINFINITE)!=WAIT_OBJECT_0) || !CloseHandle(thread))
	#define pthread_detach(thread) if(thread!=NULL)CloseHandle(thread)
	#define thread_sleep(nms) Sleep(nms)
	#define pthread_cancel(thread) ExitThread(thread)
	#define ts_key_create(ts_key, destructor) {ts_key = TlsAlloc();};
	#define pthread_getspecific(ts_key) TlsGetValue(ts_key)
	#define pthread_setspecific(ts_key, value) TlsSetValue(ts_key, (void *)value)
	#define pthread_self() GetCurrentThreadId()

	#define sem_t HANDLE
	#define pause(voidpara) __asm PAUSE
	#define sem_init(sem, sem_attr1, sem_init_value) (int)((*sem = CreateEvent(NULL,FALSE,FALSE,NULL))==NULL)
	#define sem_wait(sem) (int)(WAIT_OBJECT_0 != WaitForSingleObject(*sem,VPXINFINITE))
    #define sem_trywait(sem) (int)(WAIT_OBJECT_0 != WaitForSingleObject(*(sem),0))
	#define sem_post(sem) SetEvent(*sem)
	#define sem_destroy(sem) if(*sem)((int)(CloseHandle(*sem))==TRUE)
	#define thread_sleep(nms) Sleep(nms)
#elif VOANDROID || _IOS
	#include<pthread.h>
	#include <semaphore.h>
    #include <unistd.h>
    #include<sys/time.h>
    #include<sys/resource.h>

	#define THREAD_FUNCTION  void*
	#define thread_sleep(nms) usleep(nms*1000)
#endif

#endif
