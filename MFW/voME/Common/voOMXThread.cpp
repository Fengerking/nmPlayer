	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voOMXThread.c

	Contains:	memory operator implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-15		JBF			Create file

*******************************************************************************/
#ifdef _WIN32
#include <windows.h>
#elif defined LINUX
#include <sys/mman.h>
#include <pthread.h>
#include <sys/prctl.h>
#endif // _WIN32

#include "voOMXThread.h"

#define OMXVO_DEFAULT_STACKSIZE (128 * 1024)

#include "voLog.h"


OMX_ERRORTYPE voOMXThreadCreate (voOMXThreadHandle * pHandle, OMX_U32 * pID,
								 voOMX_ThreadProc fProc, OMX_PTR pParam, OMX_U32 uFlag)
{
	if (pHandle == NULL || pID == NULL)
		return OMX_ErrorBadParameter;

#ifdef _WIN32
	*pHandle = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE) fProc, pParam, 0, pID);
	if (*pHandle == NULL)
		return OMX_ErrorInsufficientResources;
#elif defined LINUX
	pthread_t tt;

	int rs = -1;
#if defined(__VOTT_ARM__)

	pthread_attr_t	attr;

	pthread_attr_init(&attr);
	/*
	attr.flags = 0;
	attr.stack_base = NULL;
	attr.stack_size = PAGE_SIZE * 2;//DEFAULT_STACKSIZE;
	attr.guard_size = PAGE_SIZE;
	attr.sched_policy = SCHED_NORMAL;
	attr.sched_priority = 0;
	*/
	pthread_attr_setstacksize(&attr, OMXVO_DEFAULT_STACKSIZE);
	rs = pthread_create(&tt, &attr, (void*(*)(void*))fProc ,pParam);
	pthread_attr_destroy(&attr);
#else
	rs = pthread_create(&tt, NULL, (void*(*)(void*))fProc ,pParam);
#endif

	if (rs != 0)
		return OMX_ErrorInsufficientResources;

	// detach so that free resource automatically
	// but can not use pthread_join to wait thread end
	rs = pthread_detach(tt);

	*pHandle = (voOMXThreadHandle)tt;
	*pID = tt;
#endif //_WIN32

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voOMXThreadClose (voOMXThreadHandle hHandle, OMX_U32 uExitCode)
{
	if (hHandle == NULL)
		return OMX_ErrorBadParameter;

#ifdef _WIN32
	CloseHandle (hHandle);
#endif //_WIN32

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voOMXThreadGetPriority (voOMXThreadHandle hHandle, voOMX_ThreadPriority * pPriority)
{
	OMX_S32 nPriority = 0;
	nPriority = nPriority;
	if (hHandle == NULL)
		return OMX_ErrorBadParameter;

#ifdef _WIN32
	nPriority = GetThreadPriority (hHandle);
	voOMXThreadConvertPriority (pPriority, &nPriority, OMX_FALSE);
#endif //_WIN32

	return OMX_ErrorNone;
}


OMX_ERRORTYPE voOMXThreadSetPriority (voOMXThreadHandle hHandle, voOMX_ThreadPriority uPriority)
{
	OMX_S32 nPriority = 0;
	nPriority = nPriority;
	if (hHandle == NULL)
		return OMX_ErrorBadParameter;

#ifdef _WIN32
	voOMXThreadConvertPriority (&uPriority, &nPriority, OMX_TRUE);
	SetThreadPriority (hHandle, nPriority);
#endif //_WIN32

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voOMXThreadSuspend (voOMXThreadHandle hHandle)
{
	if (hHandle == NULL)
		return OMX_ErrorBadParameter;

#ifdef _WIN32
	SuspendThread (hHandle);
#endif //_WIN32

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voOMXThreadResume (voOMXThreadHandle hHandle)
{
	if (hHandle == NULL)
		return OMX_ErrorBadParameter;

#ifdef _WIN32
	ResumeThread (hHandle);
#endif //_WIN32

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voOMXThreadProtect (voOMXThreadHandle hHandle)
{

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voOMXThreadConvertPriority (voOMX_ThreadPriority * pPriority, OMX_S32 * pPlatform, OMX_BOOL bPlatform)
{
	if (bPlatform)
	{
		switch (*pPriority)
		{
#ifdef _WIN32
		case VOOMX_THREAD_PRIORITY_TIME_CRITICAL:
			*pPlatform = THREAD_PRIORITY_TIME_CRITICAL;
			break;

		case VOOMX_THREAD_PRIORITY_HIGHEST:
			*pPlatform = THREAD_PRIORITY_HIGHEST;
			break;

		case VOOMX_THREAD_PRIORITY_ABOVE_NORMAL:
			*pPlatform = THREAD_PRIORITY_ABOVE_NORMAL;
			break;

		case VOOMX_THREAD_PRIORITY_NORMAL:
			*pPlatform = THREAD_PRIORITY_NORMAL;
			break;

		case VOOMX_THREAD_PRIORITY_BELOW_NORMAL:
			*pPlatform = THREAD_PRIORITY_BELOW_NORMAL;
			break;

		case VOOMX_THREAD_PRIORITY_LOWEST:
			*pPlatform = THREAD_PRIORITY_LOWEST;
			break;

		case VOOMX_THREAD_PRIORITY_IDLE:
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
#ifdef _WIN32
		case THREAD_PRIORITY_TIME_CRITICAL:
			*pPriority = VOOMX_THREAD_PRIORITY_TIME_CRITICAL;
			break;

		case THREAD_PRIORITY_HIGHEST:
			*pPriority = VOOMX_THREAD_PRIORITY_HIGHEST;
			break;

		case THREAD_PRIORITY_ABOVE_NORMAL:
			*pPriority = VOOMX_THREAD_PRIORITY_ABOVE_NORMAL;
			break;

		case THREAD_PRIORITY_NORMAL:
			*pPriority = VOOMX_THREAD_PRIORITY_NORMAL;
			break;

		case THREAD_PRIORITY_BELOW_NORMAL:
			*pPriority = VOOMX_THREAD_PRIORITY_BELOW_NORMAL;
			break;

		case THREAD_PRIORITY_LOWEST:
			*pPriority = VOOMX_THREAD_PRIORITY_LOWEST;
			break;

		case THREAD_PRIORITY_IDLE:
			*pPriority = VOOMX_THREAD_PRIORITY_IDLE;
			break;
#endif // _WIN32
		default:
			break;
		}
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE	voSetThreadName( const char * str_name )
{
#ifdef _LINUX
	prctl(PR_SET_NAME, (unsigned long)str_name , 0 , 0 , 0);
#endif
	return OMX_ErrorNone;
}
