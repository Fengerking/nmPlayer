/************************************************************************
 *                                                                      *
 *        VisualOn, Inc. Confidential and Proprietary, 2003-            *
 *                                                                      *
 ************************************************************************/
/*******************************************************************************
 File:        voRunRequestOnMain
 
 Contains:    VisualOn request to run on main thread header file
 
 Written by:  Jeff
 
 Change History (most recent first):
 2012-09-19   Jeff            Create file
 *******************************************************************************/

#import <Foundation/Foundation.h>
#import "voRunRequestOnMain.h"

@interface voRunRequestOnMainObjC : NSObject
{
    voNSThreadListenerInfo    m_cInfoCB;
}

-(void) setDelegateCB:(voNSThreadListenerInfo *)pInfo;
-(void) postRunOnMainRequest:(bool)bWaitUntilDone nID:(int)nID pParam1:(void *)pParam1 pParam2:(void *)pParam2;
-(void) runOnMain:(NSArray *) pValue;

@end
