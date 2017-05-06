
#include "threadAPI.h"
#include "voCSemaphore.h"
#include "voCMutex.h"

VO_U32 VO_API ConstructSyncObject(VO_HANDLE* psyncObject)
{
	voCSemaphore* sem = new voCSemaphore();
	*psyncObject = sem;
	return 0;
}
VO_U32 VO_API WaitSyncObject(TThreadSync* threadSync,int timeWait)
{
	voCSemaphore* sem = (voCSemaphore*)threadSync->syncObject;
	if(timeWait==-1)
		timeWait=1;//0000*1000;//10s
	
	threadSync->waiting=1;
	sem->Wait(timeWait);
	return 0;
}
VO_U32 VO_API NotifySyncObject(TThreadSync* threadSync)
{
	voCSemaphore* sem = (voCSemaphore*)threadSync->syncObject;
	threadSync->waiting=0;
	sem->Signal();
	return 0;
}
VO_U32 VO_API ResetSyncObject(TThreadSync* threadSync)
{
	voCSemaphore* sem = (voCSemaphore*)threadSync->syncObject;
	threadSync->waiting=0;
	sem->Reset();
	return 0;
}
VO_U32 VO_API DestructSyncObject(VO_HANDLE syncObject)
{
	voCSemaphore* sem = (voCSemaphore*)syncObject;
//	delete sem;
	return 0;
}


VO_U32 VO_API ConstructMutexObject(VO_HANDLE* psyncObject)
{
	voCMutex* mutex = new voCMutex();
	*psyncObject = mutex;
	return 0;
}

VO_U32 VO_API DestructMutexObject(VO_HANDLE syncObject)
{
	voCMutex* mutex = (voCMutex*)syncObject;
//	delete mutex;
	return 0;
}


VO_U32 VO_API LockMutexObject(TThreadSync* threadSync)
{
	voCMutex* mutex = (voCMutex*)threadSync->syncObject;
	mutex->Lock();
	return 0;
}
VO_U32 VO_API UnLockMutexObject(TThreadSync* threadSync)
{
	voCMutex* mutex = (voCMutex*)threadSync->syncObject;
	mutex->Unlock();
	return 0;
}



