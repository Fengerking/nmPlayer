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
 * @file voSecurityCBObj.h
 * Objective-c object for transform Callback
 *
 * Objective-c object for transform Callback. Can used by c language
 *
 * @author  Jeff Huang
 *
 * @date    2013-2013 (yyyy: year of creation, zzzz: year of last update)
 ************************************************************************/

#import <Foundation/Foundation.h>
#import "ViewRightWebiOS.h"

typedef void (* VO_SECURITY_LISTENER) (void* pUserData, int nID, void *pParam1, void *pParam2);
typedef struct
{
    VO_SECURITY_LISTENER  pListener;
    void*			                 pUserData;
}voSecurityListenerInfo;


@interface voSecurityCBObj : NSObject <VRSecurityCallback>
{
    voSecurityListenerInfo m_cInfoCB;
}

- (id) init;
- (void) setDelegateCB:(voSecurityListenerInfo *)pInfo;
- (void) ConfigureOutputControlSettings:(struct OutputControls) settings; 

@end
