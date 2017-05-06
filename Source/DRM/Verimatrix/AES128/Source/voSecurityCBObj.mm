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
 * @file voSecurityCBObj.cpp
 * Objective-c object for transform Callback
 *
 * Objective-c object for transform Callback. Can used by c language
 *
 * @author  Jeff Huang
 *
 * @date    2013-2013 (yyyy: year of creation, zzzz: year of last update)
 ************************************************************************/

#import "voSecurityCBObj.h"

@implementation voSecurityCBObj

- (id) init
{
    if (nil != (self = [super init]))
    {
        memset(&m_cInfoCB, 0, sizeof(m_cInfoCB));
    }
    
    return self;
}

- (void) setDelegateCB:(voSecurityListenerInfo *)pInfo;
{
    if (NULL == pInfo) {
        return;
    }
    
    memcpy(&m_cInfoCB, pInfo, sizeof(m_cInfoCB));
}

- (void) securityCallback
{
    if (NULL != m_cInfoCB.pListener && NULL != m_cInfoCB.pUserData) {
        m_cInfoCB.pListener(m_cInfoCB.pUserData, 0, 0, 0);
    }
}

- (void) ConfigureOutputControlSettings:(struct OutputControls) settings;
{
	//TODO
}

@end
