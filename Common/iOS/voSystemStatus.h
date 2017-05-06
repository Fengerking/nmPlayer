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
 * @file voSystemStatus.h
 * System status
 *
 * System status
 *
 * @author  Jeff Huang
 *
 * @date    2013-2013 (yyyy: year of creation, zzzz: year of last update)
 ************************************************************************/

#ifndef __VO_SYSTEM_STATUS__
#define __VO_SYSTEM_STATUS__

#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class voSystemStatus
{
public:
	static voSystemStatus* Instance();
    
    bool IsAPPActive();
    
    void SetAPPActive(bool bActive);
    
private:
	voSystemStatus();
	voSystemStatus& operator=(const voSystemStatus&);
	virtual ~voSystemStatus();
    
private:
	
	static voSystemStatus *m_pSystemStatusManager;
    
	static voCMutex m_cMutex;
	
    bool m_bIsAppActive;
};
    
#ifdef _VONAMESPACE
}
#endif

#endif
