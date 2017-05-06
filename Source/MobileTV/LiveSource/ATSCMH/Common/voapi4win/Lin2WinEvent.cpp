#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#include "Lin2WinEvent.h"
#include "Lin2WinBase.h"

HANDLE CreateEvent(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState,LPCTSTR lpName)
{
    lh_info* pInfo = (lh_info*) malloc(sizeof(lh_info));
	memset(pInfo, 0, sizeof(lh_info));

	pInfo->mType = LH_EVENT;
	pthread_mutex_init(&pInfo->mData.mEvt.lMutex, NULL);
	pthread_cond_init(&pInfo->mData.mEvt.lCond, NULL);
	return pInfo;
}

BOOL SetEvent(HANDLE hEvent)
{
	if(hEvent)
	{
		lh_info* pInfo = (lh_info*) hEvent;
		assert(pInfo->mType == LH_EVENT);
		pthread_mutex_lock(&pInfo->mData.mEvt.lMutex);
		pthread_cond_signal(&pInfo->mData.mEvt.lCond);
		pthread_mutex_unlock(&pInfo->mData.mEvt.lMutex);
		return TRUE;
	}
	return FALSE;
}
