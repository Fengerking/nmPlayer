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
 * @file voSystemStatus.cpp
 * System status
 *
 * System status
 *
 * @author  Jeff Huang
 *
 * @date    2013-2013 (yyyy: year of creation, zzzz: year of last update)
 ************************************************************************/

#import "voSystemStatus.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voSystemStatus* voSystemStatus::m_pSystemStatusManager = NULL;
voCMutex voSystemStatus::m_cMutex;

voSystemStatus* voSystemStatus::Instance() {
    voCAutoLock lock (&m_cMutex);
    
	if (NULL == m_pSystemStatusManager) {
		m_pSystemStatusManager = new voSystemStatus();
	}
	return m_pSystemStatusManager;
}


bool voSystemStatus::IsAPPActive()
{
    return m_bIsAppActive;
}

void voSystemStatus::SetAPPActive(bool bActive)
{
    m_bIsAppActive = bActive;
}

/*-------------------------------------------------------------------------------------*
 * Internal methods
 *-------------------------------------------------------------------------------------*/
voSystemStatus::voSystemStatus()
:m_bIsAppActive(true)
{
}


voSystemStatus::~voSystemStatus()
{
}