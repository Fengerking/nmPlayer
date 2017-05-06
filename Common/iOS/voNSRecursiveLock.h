/************************************************************************
 VisualOn Proprietary
 Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved
 
 VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA
 
 All data and information contained in or disclosed by this document are
 confidential and proprietary information of VisualOn, and all rights
 therein are expressly reserved. By accepting this material, the
 recipient agrees that this material and the information contained
 therein are held in confidence and in trust. The material may only be
 used and/or disclosed as authorized in a license agreement controlling
 such use and disclosure.
 ************************************************************************/
/************************************************************************
 * @file voNSRecursiveLock.h
 * objective-c locker
 *
 * objective-c locker
 *
 * @author  Jeff Huang
 *
 * @date    2013-2013 (yyyy: year of creation, zzzz: year of last update)
 ************************************************************************/

#ifndef __voNSRecursiveLock_H__
#define __voNSRecursiveLock_H__

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class voNSRecursiveLock
{
public:
    voNSRecursiveLock(void);
    virtual ~voNSRecursiveLock(void);
    
    bool Init();

    void Lock();
    void Unlock();

    bool TryLock();
    
private:
    void* m_pLock;
};

// locks a voNSRecursiveLock, and unlocks it automatically
// when the lock goes out of scope
class voNSAutoLock
{
public:
    voNSAutoLock(voNSRecursiveLock * plock)
    {
        m_pLock = plock;
        if (m_pLock) {
            m_pLock->Lock();
        }
    };

    ~voNSAutoLock()
	{
        if (m_pLock) {
            m_pLock->Unlock();
        }
    };
protected:
    voNSRecursiveLock * m_pLock;
};
    
#ifdef _VONAMESPACE
}
#endif

#endif //__voNSRecursiveLock_H__
