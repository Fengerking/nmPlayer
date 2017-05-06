//////////////////////////////////////////////////////////////////////////
//																																			//
//                   vosyncmutex.h																			//
//     This is designed just only for multithread sync on Linux.        // 
//																																			//
//						Copyright: ViusalOn 																			//
//						Author:    DavidON																				//
//						Date Created:2007/01/24																		//
//////////////////////////////////////////////////////////////////////////

#pragma once
#if !defined(__VO_SYNC_MUTEX_H__)
#define __VO_SYNC_MUTEX_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#if defined(__cplusplus)
extern "C"{
#endif

#define CRITSECT_NOERROR   0
#define CRITSECT_TIMEDOUT  1
#define CRITSECT_INVALID   2
#define CRITSECT_NOMEMORY  3


    /**** Multithreaded Pthreads Version ****/

#define CRITSECT_ENTER(cs)   pthread_mutex_lock((cs)->pm)
#define CRITSECT_LEAVE(cs)   pthread_mutex_unlock((cs)->pm)
#define CRITSECT_SIGNAL(cs)  pthread_cond_signal(&cs->cv)

#define CRITSECT_TIMEDWAIT_ENTER(cs, wait, timedout, predicate)         \
{                                                                       \
  struct timespec abstime;                                              \
  Int    retval;                                                        \
  pthread_get_expiration_np(wait, &abstime);                            \
  pthread_mutex_lock(cs->pm);                                     \
  while (predicate && !timedout)                                        \
    timedout = pthread_cond_timedwait(&cs->cv, cs->pm, &abstime); \
  if (timedout) {                                                       \
    if      (timedout == ETIMEDOUT) timedout = CRITSECT_TIMEDOUT;       \
    else if (timedout == ENOMEM)    timedout = CRITSECT_NOMEMORY;       \
    else                            timedout = CRITSECT_INVALID;        \
  }                                                                     \
}

#define CRITSECT_WAIT_ENTER(cs, predicate) {                            \
  pthread_mutex_lock(cs->pm);                                     \
  while (predicate)                                                     \
    pthread_cond_wait(&cs->cv, cs->pm);                           \
}

typedef struct _critical_section{
  pthread_mutex_t   pm;
  pthread_cond_t    cv;
} critical_section, *pcritical_section;
typedef critical_section CRITICAL_SECTION;
typedef pcritical_section PCRITICAL_SECTION;

int InitializeCriticalSection(pcritical_section pcs);
int EnterCriticalSection (pcritical_section pcs);
int LeaveCriticalSection(pcritical_section pcs);
int DeleteCriticalSection(pcritical_section pcs);

//pcritical_section vocs_create(IMemory* memory, pcritical_section parent);
//void  vocs_destroy(pcritical_section cs);

#define CRITSECT_DECLARE(C)                pcritical_section  C;
#define CRITSECT_CREATE(C,M,P)             ((C) = vocs_create(M, P))
#define CRITSECT_DESTROY(C)                if (C) vocs_destroy(C)
#define CRITSECT_EXISTS(C)                 (C)
#define CRITSECT_ASSIGN(C,CS)              (C) = (CS)
#define TIMESPEC                           struct timespec
#define CRITSECT_SET_TIMESPEC(T,USEC)      (T).tv_sec = (USEC)/1000000, (T).tv_nsec = ((USEC)%1000000)*1000

#define MUTEX_DECLARE(M)                   pthread_mutex_t M;
#define MUTEX_INIT(MP, ATTR)               !pthread_mutex_init(MP, ATTR)
#define MUTEX_DESTROY(MP)                  ((MP) && !pthread_mutex_destroy(MP))
#define MUTEX_LOCK(MP)                     pthread_mutex_lock(MP)
#define MUTEX_UNLOCK(MP)                   pthread_mutex_unlock(MP)


#if defined(__cplusplus)
}
#endif

#endif

