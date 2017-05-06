#ifndef __VOLOCKEDOBJECT_H__

#define __VOLOCKEDOBJECT_H__

#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

template < typename T >
class voLockedObject
{
public:
	voLockedObject()
	{
		;
	}

	voLockedObject( T val )
		:m_object(val)
	{
		;
	}

	~voLockedObject()
	{
		;
	}

	voLockedObject<T>& operator=( T obj )
	{
		voCAutoLock lock( &m_lock );

		m_object = obj;
		return *this;
	}

	operator T()
	{
		voCAutoLock lock( &m_lock );
		return m_object;
	}

	T& Lock()
	{
		m_lock.Lock();
		return m_object;
	}

	void Unlock()
	{
		m_lock.Unlock();
	}

protected:
	T m_object;
	
	voCMutex m_lock;
};

#ifdef _VONAMESPACE
}
#endif

#endif