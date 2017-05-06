///////////////////////////////////////////////////////////////////
// vosyncmutex.cpp  designed for simulating following  APIs 
//of Windows
// David  2007/01/24

#include "vocrstypes.h"
#include "vosyncmutex.h"


int InitializeCriticalSection(pcritical_section pcs){
	if (NULL == pcs) return -1;
/*
	if (NULL == pcs->pm){
		pcs->pm = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
		if (pcs->pm == NULL){
			printf("[david]: no enough memory\n");
			return -1;
		}
	}
*/
	return pthread_mutex_init(&pcs->pm, NULL);
}

int EnterCriticalSection (pcritical_section pcs){
	if (NULL == pcs) return -1;

	//VOINFO("enter critical section");
	//if (pcs->pm)
		pthread_mutex_lock(&pcs->pm);

	return 0;
}

int LeaveCriticalSection(pcritical_section pcs){
	if (NULL == pcs)
		return -1;

	//VOINFO("leave critical section");
	//if (pcs->pm)
		pthread_mutex_unlock(&pcs->pm);

	return 0;
}

int DeleteCriticalSection(pcritical_section pcs){
	if (NULL == pcs) return -1;

	//if (pcs->pm){
		pthread_mutex_destroy(&pcs->pm);
	//	free(pcs->pm);
	//}

	//pcs->pm = NULL;

	return 0;
}



