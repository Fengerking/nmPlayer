#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <assert.h>

#include "Lin2WinThread.h"
#include "Lin2WinBase.h"


pthread_attr_t *make_priority(int priority) 
{   
   int error; 
   pthread_attr_t* attr;
   struct sched_param param; 
   if ((attr = (pthread_attr_t *)malloc(sizeof(pthread_attr_t))) == NULL) 
      return NULL; 
   if (!(error = pthread_attr_init(attr)) && 
       !(error = pthread_attr_getschedparam(attr, &param))) 
   { 
       param.sched_priority = priority; 
       error = pthread_attr_setschedparam(attr, &param); 
   } 
   if (error) 
   {          
	  // if failure, be sure to free memory 
      free(attr); 
      errno = error; 
      return NULL; 
   } 
   return attr; 
} 

HANDLE CreateThread(LPSECURITY_ATTRIBUTES  lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId, int nPriority)
{
	typedef void* (*P_ST_RT)(void* lpThreadParameter);	

	pthread_t pt;	
	if(nPriority != THREAD_PRIORITY_NORMAL)
	{
#if defined(_DEBUG)
		printf("at present the priority is not supported\n");
#endif 		
	}
	if(pthread_create(&pt, NULL, (P_ST_RT)(lpStartAddress), lpParameter) != 0)
	{
#if defined(_DEBUG)
		printf("can not create the thread the reason is %s\n", strerror(errno));
		return NULL;
#endif
	}
	
	
	*lpThreadId = (unsigned long)pt;
	lh_info* pInfo = (lh_info*)malloc(sizeof(lh_info));

	memset(pInfo, 0, sizeof(lh_info));

	pInfo->mType = LH_THREAD;
	
	pInfo->mData.mThrd.mThId = pt;
	
	return pInfo;
}

DWORD GetThreadId(HANDLE hThread)
{
	if(hThread)
	{
		lh_info* pInfo = (lh_info*)(hThread);
		assert(pInfo->mType == LH_THREAD);
		return (DWORD)pInfo->mData.mThrd.mThId;
	}
	return -1;
}
	
DWORD GetCurrentThreadId(void)
{
	return (DWORD)pthread_self();
}
