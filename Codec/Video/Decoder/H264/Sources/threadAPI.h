
#ifndef _THREADAPI_H_
#define _THREADAPI_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voVideo.h>
typedef struct{
	VO_HANDLE syncObject;
	int		  waiting;
}TThreadSync;
VO_U32 VO_API ConstructSyncObject(VO_HANDLE* psyncObject);
VO_U32 VO_API WaitSyncObject(TThreadSync* threadSync,int timeWait); 
VO_U32 VO_API NotifySyncObject(TThreadSync* threadSync);
VO_U32 VO_API ResetSyncObject(TThreadSync* threadSync);
VO_U32 VO_API DestructSyncObject (VO_HANDLE syncObject);

VO_U32 VO_API ConstructMutexObject(VO_HANDLE* psyncObject);
VO_U32 VO_API DestructMutexObject(VO_HANDLE syncObject);
VO_U32 VO_API LockMutexObject(TThreadSync* threadSync);
VO_U32 VO_API UnLockMutexObject(TThreadSync* threadSync);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

