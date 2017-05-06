
/* david 2009-11-28 */

#include <pthread.h>
#include "vomutex.h"

#if !defined EINTR
#define EINTR 4
#endif

Mutex::Mutex()
{
    _init();
}

Mutex::Mutex(const char* name)
{
    // XXX: name not used for now
    _init();
}

void Mutex::_init()
{
    pthread_mutex_t* pMutex = new pthread_mutex_t;
    pthread_mutex_init(pMutex, NULL);
    mState = pMutex;
}

Mutex::~Mutex()
{
    delete (pthread_mutex_t*) mState;
}

int Mutex::lock()
{
    int res;
    while ((res=pthread_mutex_lock((pthread_mutex_t*) mState)) == EINTR) ;
    return -res;
}

void Mutex::unlock()
{
    pthread_mutex_unlock((pthread_mutex_t*) mState);
}

int Mutex::tryLock()
{
    int res;
    while ((res=pthread_mutex_trylock((pthread_mutex_t*) mState)) == EINTR) ;
    return -res;
}

