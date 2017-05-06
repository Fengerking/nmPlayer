/************************************************************************
 *                                                                      *
 *        VisualOn, Inc. Confidential and Proprietary, 2003-            *
 *                                                                      *
 ************************************************************************/
/*******************************************************************************
 File:        voRunRequestOnMain
 
 Contains:    VisualOn request to run on main thread cpp file
 
 Written by:  Jeff
 
 Change History (most recent first):
 2012-09-19   Jeff            Create file
 *******************************************************************************/

#import "voRunRequestOnMainObjC.h"

@implementation voRunRequestOnMainObjC
-(void) setDelegateCB:(voNSThreadListenerInfo *)pInfo
{
    if (NULL == pInfo) {
        return;
    }
    memcpy(&m_cInfoCB, pInfo, sizeof(m_cInfoCB));
}

- (void) runOnMain:(NSArray *) pValue
{
    int iID = 0;
    void *pParam1 = NULL;
    void *pParam2 = NULL;
    
    if (nil != pValue && (3 == [pValue count])) {
        iID = [[pValue objectAtIndex:0] intValue];
        [[pValue objectAtIndex:1] getValue:&pParam1];
        [[pValue objectAtIndex:2] getValue:&pParam2];
    }
    
    if (NULL != m_cInfoCB.pListener && NULL != m_cInfoCB.pUserData) {
        m_cInfoCB.pListener(m_cInfoCB.pUserData, iID, pParam1, pParam2);
    }
}

-(void) postRunOnMainRequest:(bool)bWaitUntilDone nID:(int)nID pParam1:(void *)pParam1 pParam2:(void *)pParam2
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    NSNumber *pValueID = [NSNumber numberWithInt:nID];
    NSValue *pValueParam1 = [NSValue valueWithPointer:pParam1];
    NSValue *pValueParam2 = [NSValue valueWithPointer:pParam2];
    NSArray *pPost = [NSArray arrayWithObjects:pValueID, pValueParam1, pValueParam2, nil];
    
    BOOL bWait = NO;
    if (bWaitUntilDone) {
        bWait = YES;
    }
    
    [self performSelectorOnMainThread:@selector(runOnMain:) withObject:pPost waitUntilDone:bWait];
    
    [pool release];
}

@end
