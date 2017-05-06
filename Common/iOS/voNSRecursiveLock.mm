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
 * @file voNSRecursiveLock.cpp
 * objective-c locker
 *
 * objective-c locker
 *
 * @author  Jeff Huang
 *
 * @date    2013-2013 (yyyy: year of creation, zzzz: year of last update)
 ************************************************************************/

#import <Foundation/Foundation.h>

#include "voNSRecursiveLock.h"

#define LOG_TAG "voNSRecursiveLock"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voNSRecursiveLock::voNSRecursiveLock()
{
    m_pLock = [[NSRecursiveLock alloc] init];
}

bool voNSRecursiveLock::Init()
{
    if (nil == m_pLock) {
        return false;
    }
    
    return true;
}

voNSRecursiveLock::~voNSRecursiveLock()
{
    if (nil != m_pLock) {
        [(NSRecursiveLock *)m_pLock release];
        m_pLock = nil;
    }
}

void voNSRecursiveLock::Lock()
{
    if (nil == m_pLock) {
        return;
    }
    
    [(NSRecursiveLock *)m_pLock lock];
}

bool voNSRecursiveLock::TryLock()
{
    if (nil == m_pLock) {
        return false;
    }
    
    if (YES == [(NSRecursiveLock *)m_pLock tryLock]) {
        return true;
    }
    
    return false;
}

void voNSRecursiveLock::Unlock()
{
    if (nil == m_pLock) {
        return;
    }
    
    [(NSRecursiveLock *)m_pLock unlock];
}
