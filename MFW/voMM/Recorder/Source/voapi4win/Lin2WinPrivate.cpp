#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <errno.h>

#include "Lin2WinBase.h"
#include "Lin2WinPrivate.h"

static lh_info* get_handle(HANDLE inHandle, LH_TYPE type)
{
	if(inHandle)
	{
		lh_info* pInfo = (lh_info*)inHandle; 
		assert(pInfo->mType == type);
		return pInfo;
	}
	return NULL;
}

BOOL _close_event_handle(HANDLE hEvent)
{
	lh_info* pInfo = get_handle(hEvent, LH_EVENT);
	
	pthread_mutex_destroy(&pInfo->mData.mEvt.lMutex);
	pthread_cond_destroy(&pInfo->mData.mEvt.lCond);
	if(pInfo)
	{
		free(pInfo);
		return TRUE;
	}
	return FALSE;
}

BOOL _close_thread_handle(HANDLE hThread)
{
	lh_info* pInfo = get_handle(hThread, LH_THREAD);
	if(pInfo)
	{
		free(pInfo);
		return TRUE;
	}
	return FALSE;
}

BOOL _close_file_handle(HANDLE hFile)
{
	lh_info* pInfo = get_handle(hFile, LH_FILE);
	close(pInfo->mData.mFd);

	if(pInfo)
	{
		free(pInfo);
		return TRUE;
	}
	return FALSE;
}

DWORD _wait_event_handle(HANDLE hEvt, DWORD  dwms)
{
	int nRet = -1;
	lh_info* pInfo = get_handle(hEvt, LH_EVENT);
	if(pInfo)
	{
		pthread_mutex_lock(&pInfo->mData.mEvt.lMutex);
		if(dwms > 0 && dwms != INFINITE)
		{
			struct timespec ts ;
			ts.tv_sec = dwms/1000;
			ts.tv_nsec = (dwms%1000)*1000000;
			nRet = pthread_cond_timedwait(&pInfo->mData.mEvt.lCond, &pInfo->mData.mEvt.lMutex, &ts);
		}
		else
		{
			nRet = pthread_cond_wait(&pInfo->mData.mEvt.lCond, &pInfo->mData.mEvt.lMutex);
		}
		pthread_mutex_unlock(&pInfo->mData.mEvt.lMutex);
	}

#if defined(_DEBUG)
	if(nRet != 0)
	{
		printf("the error is %s\n", strerror(errno));
	}
#endif 
	return nRet;
}
DWORD _wait_thread_handle(HANDLE hThread, DWORD dwMillSeconds)
{
	int nRet = -1;
	lh_info* pInfo = get_handle(hThread, LH_THREAD);
	if(pInfo)
	{
		nRet = pthread_join(pInfo->mData.mThrd.mThId, NULL);
		
#if defined(_DEBUG)
		if(nRet != 0)
			printf("the pthread can not join with the error %s\n", strerror(nRet));
#endif
	}
	return nRet;
}
