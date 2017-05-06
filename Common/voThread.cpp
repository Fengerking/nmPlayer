	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voThread.c

	Contains:	memory operator implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-15		JBF			Create file

*******************************************************************************/
#ifdef _METRO
#include <windows.h>
#include <process.h>
#elif defined _WIN32
#include <windows.h>
#elif defined LINUX
#include <sys/mman.h>
#include <pthread.h>
#include <sys/prctl.h>
#elif defined __SYMBIAN32__
#include <e32std.h>
#elif defined _IOS || defined _MAC_OS
#include "voLog.h"
#include <pthread.h>
#include "voOSFunc.h"
#endif

#include "voThread.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#define VO_DEFAULT_STACKSIZE (128 * 1024)

#ifdef _WIN32
#define MS_VC_EXCEPTION 0x406D1388
#pragma pack(push,8)
typedef struct tagVOTHREADNAME_INFO
{
   VO_U32 dwType; // Must be 0x1000.
   char * szName; // Pointer to name (in user addr space).
   VO_U32 dwThreadID; // Thread ID (-1=caller thread).
   VO_U32 dwFlags; // Reserved for future use, must be zero.
} VOTHREADNAME_INFO;
#pragma pack(pop)
#endif // _WIN32

void voThreadSetName(VO_U32 uThreadID, const char* threadName)
{
#ifdef _WIN32
   //VOTHREADNAME_INFO info;
   //info.dwType = 0x1000;
   //info.szName = (char *)threadName;
   //info.dwThreadID = uThreadID;
   //info.dwFlags = 0;

   //__try
   //{
   //   RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
   //}
   //__except(EXCEPTION_EXECUTE_HANDLER)
   //{
   //}
#elif defined _LINUX_ANDROID
	prctl(PR_SET_NAME, (unsigned long)threadName , 0 , 0 , 0);
#endif // _WIN32
}

VO_U32 voThreadCreate (voThreadHandle * pHandle, VO_U32 * pID,
								 voThreadProc fProc, VO_PTR pParam, VO_U32 uFlag)
{
	if (pHandle == NULL || pID == NULL)
		return VO_ERR_INVALID_ARG;

	*pHandle = NULL;
	*pID = 0;
#ifdef _METRO
	*pHandle = (void *) _beginthreadex( NULL , 0 , (VOTHREAD_START_ROUTINE)fProc , pParam , 0 , 0);
	if (*pHandle == NULL)
		return VO_ERR_OUTOF_MEMORY;
#elif defined _WIN32
	*pHandle = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE) fProc, pParam, 0, pID);
	if (*pHandle == NULL)
		return VO_ERR_OUTOF_MEMORY;
#elif defined LINUX
	pthread_t 		tt;

	pthread_attr_t	attr;
	pthread_attr_init(&attr);
#if !defined NNJ
	pthread_attr_setstacksize(&attr, VO_DEFAULT_STACKSIZE);
	pthread_attr_setguardsize(&attr, PAGE_SIZE);
	/*attr.flags = 0;
	attr.stack_base = NULL;
	attr.stack_size = VO_DEFAULT_STACKSIZE; //DEFAULT_STACKSIZE;
	attr.guard_size = PAGE_SIZE;*/

	if (uFlag == 0)
	{
		pthread_attr_setschedpolicy(&attr, SCHED_NORMAL);
		attr.sched_priority = 0;
	}
	else
	{
		pthread_attr_setschedpolicy(&attr, SCHED_RR);
		attr.sched_priority = uFlag;
	}
#else 

#define PAGE_SIZE   (1UL << 12)
	pthread_attr_setstacksize(&attr, VO_DEFAULT_STACKSIZE);
	pthread_attr_setguardsize(&attr, PAGE_SIZE);

	struct sched_param param;
	if (uFlag == 0) {
		pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
		param.sched_priority = 0;
	} else {
		pthread_attr_setschedpolicy(&attr, SCHED_RR);
		param.sched_priority = uFlag;
	}
	pthread_attr_setschedparam(&attr, &param);
#endif

	int rs = pthread_create(&tt, &attr, (void*(*)(void*))fProc ,pParam);
	pthread_attr_destroy(&attr);

	if (rs != 0)
		return VO_ERR_OUTOF_MEMORY;

	rs = pthread_detach(tt);

	*pHandle = (voThreadHandle)tt;
	*pID = (VO_U32)tt;
#elif defined __SYMBIAN32__
	RThread thread;
	int nRC = thread.Create(_L("VOThreadName"), (TThreadFunction)fProc, 40960, KMinHeapSize, 256
			* KMinHeapSize, NULL);
	if (nRC == KErrNone)
		thread.Resume();
	
#elif defined _IOS || defined _MAC_OS
	pthread_attr_t  attr;
    pthread_t       posixThreadID;
    int             returnVal;
	/*
	attr.flags = 0;
	attr.stack_base = NULL;
	attr.stack_size = VO_DEFAULT_STACKSIZE; //DEFAULT_STACKSIZE;
	attr.guard_size = PAGE_SIZE;
	
	if (uFlag == 0)
	{
		attr.sched_policy = SCHED_NORMAL;
		attr.sched_priority = 0;
	}
	else
	{
		attr.sched_policy = SCHED_RR;
		attr.sched_priority = uFlag;
	}
 */
	
    returnVal = pthread_attr_init(&attr);
    returnVal = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    //int     threadError = 
    pthread_create(&posixThreadID, &attr, (void*(*)(void*))fProc, pParam);
	*pHandle	= (voThreadHandle)posixThreadID;
	*pID		= (VO_U32)posixThreadID;
	//pthread_detach(posixThreadID);
#endif

	return VO_ERR_NONE;
}

VO_U32 voThreadClose (voThreadHandle hHandle, VO_U32 uExitCode)
{
	if (hHandle == NULL)
		return VO_ERR_INVALID_ARG;

#ifdef _WIN32
	CloseHandle (hHandle);
#endif //_WIN32

	return VO_ERR_NONE;
}

VO_U32 voThreadGetPriority (voThreadHandle hHandle, voThreadPriority * pPriority)
{
	if (hHandle == NULL)
		return VO_ERR_INVALID_ARG;

#ifdef _METRO
	return VO_ERR_NOT_IMPLEMENT;
#elif defined _WIN32
	VO_S32 nPriority = 0;
	nPriority = GetThreadPriority (hHandle);
	voThreadConvertPriority (pPriority, &nPriority, VO_FALSE);
#endif //_WIN32

#ifdef _IOS
	int policy = 0;
	struct sched_param param;
    VO_S32 nPriority = 0;
	
	int iRet = pthread_getschedparam((pthread_t)hHandle, &policy, &param);
	if (0 != iRet) {
		VOLOGE("pthread_getschedparam error :%d", iRet);
		return VO_ERR_FAILED;
	}
	nPriority = param.sched_priority;
	VOLOGI("get succ hHandle:%ld, policy:%d, param.sched_priority:%d", (VO_U32)hHandle, policy, param.sched_priority);
	voThreadConvertPriority (pPriority, &nPriority, VO_FALSE);
#endif

	return VO_ERR_NONE;
}


VO_U32 voThreadSetPriority (voThreadHandle hHandle, voThreadPriority uPriority)
{
	if (hHandle == NULL)
		return VO_ERR_INVALID_ARG;

#ifdef _METRO
	return VO_ERR_NOT_IMPLEMENT;
#elif defined _WIN32
	VO_S32 nPriority = 0;
	voThreadConvertPriority (&uPriority, &nPriority, VO_TRUE);
	SetThreadPriority (hHandle, nPriority);
#endif //_WIN32

#ifdef _IOS
	int policy = 0;
	struct sched_param param;
    VO_S32 nPriority = 0;
	
	int iRet = pthread_getschedparam((pthread_t)hHandle, &policy, &param);
	if (0 != iRet) {
		VOLOGE("pthread_getschedparam hHandle:%ld, error :%d", (VO_U32)hHandle, iRet);
		return VO_ERR_FAILED;
	}

	VOLOGI("get succ hHandle:%ld, policy:%d, param.sched_priority:%d", (VO_U32)hHandle, policy, param.sched_priority);
	
	voThreadConvertPriority (&uPriority, &nPriority, VO_TRUE);
	param.sched_priority = nPriority;
	iRet = pthread_setschedparam((pthread_t)hHandle, policy, &param);
	
	if (0 != iRet) {
		VOLOGE("pthread_attr_setschedparam hHandle:%ld, error :%d, param.sched_priority:%d", (VO_U32)hHandle, iRet, param.sched_priority);
		return VO_ERR_FAILED;
	}

	VOLOGI("set succ hHandle:%ld, policy:%d, param.sched_priority:%d", (VO_U32)hHandle, policy, param.sched_priority);
#endif
	
	return VO_ERR_NONE;
}

VO_U32 voThreadSuspend (voThreadHandle hHandle)
{
	if (hHandle == NULL)
		return VO_ERR_INVALID_ARG;

#ifdef _METRO
	return VO_ERR_NOT_IMPLEMENT;
#elif defined _WIN32
	SuspendThread (hHandle);
#endif //_WIN32

	return VO_ERR_NONE;
}

VO_U32 voThreadResume (voThreadHandle hHandle)
{
	if (hHandle == NULL)
		return VO_ERR_INVALID_ARG;

#ifdef _METRO
	return VO_ERR_NOT_IMPLEMENT;
#elif defined _WIN32
	ResumeThread (hHandle);
#endif //_WIN32

	return VO_ERR_NONE;
}

VO_U32 voThreadProtect (voThreadHandle hHandle)
{

	return VO_ERR_NONE;
}

VO_U32 voThreadConvertPriority (voThreadPriority * pPriority, VO_S32 * pPlatform, VO_BOOL bPlatform)
{
	if (bPlatform)
	{
		switch (*pPriority)
		{
#if defined _WIN32 || defined _IOS
		case VO_THREAD_PRIORITY_TIME_CRITICAL:
			*pPlatform = THREAD_PRIORITY_TIME_CRITICAL;
			break;

		case VO_THREAD_PRIORITY_HIGHEST:
			*pPlatform = THREAD_PRIORITY_HIGHEST;
			break;

		case VO_THREAD_PRIORITY_ABOVE_NORMAL:
			*pPlatform = THREAD_PRIORITY_ABOVE_NORMAL;
			break;

		case VO_THREAD_PRIORITY_NORMAL:
			*pPlatform = THREAD_PRIORITY_NORMAL;
			break;

		case VO_THREAD_PRIORITY_BELOW_NORMAL:
			*pPlatform = THREAD_PRIORITY_BELOW_NORMAL;
			break;

		case VO_THREAD_PRIORITY_LOWEST:
			*pPlatform = THREAD_PRIORITY_LOWEST;
			break;

		case VO_THREAD_PRIORITY_IDLE:
			*pPlatform = THREAD_PRIORITY_IDLE;
			break;
#endif // _WIN32
		default:
			break;
		}
	}
	else
	{
		switch (*pPlatform)
		{
#if defined _WIN32 || defined _IOS
		case THREAD_PRIORITY_TIME_CRITICAL:
			*pPriority = VO_THREAD_PRIORITY_TIME_CRITICAL;
			break;

		case THREAD_PRIORITY_HIGHEST:
			*pPriority = VO_THREAD_PRIORITY_HIGHEST;
			break;

		case THREAD_PRIORITY_ABOVE_NORMAL:
			*pPriority = VO_THREAD_PRIORITY_ABOVE_NORMAL;
			break;

		case THREAD_PRIORITY_NORMAL:
			*pPriority = VO_THREAD_PRIORITY_NORMAL;
			break;

		case THREAD_PRIORITY_BELOW_NORMAL:
			*pPriority = VO_THREAD_PRIORITY_BELOW_NORMAL;
			break;

		case THREAD_PRIORITY_LOWEST:
			*pPriority = VO_THREAD_PRIORITY_LOWEST;
			break;

		case THREAD_PRIORITY_IDLE:
			*pPriority = VO_THREAD_PRIORITY_IDLE;
			break;
#endif // _WIN32
		default:
			break;
		}
	}

	return VO_ERR_NONE;
}

VO_S32	voThreadGetCurrentID (void)
{
#ifdef _WIN32
	return (VO_S32) GetCurrentThreadId ();
#elif defined _LINUX
	return (VO_S32) pthread_self ();
#elif defined _MAC_OS || defined _IOS
	return (signed long) pthread_self ();
#endif // _WIN32
}
    
#ifdef _VONAMESPACE
}
#endif
