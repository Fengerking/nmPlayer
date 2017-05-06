    /************************************************************************
    *                                                                       *
    *        VisualOn, Inc. Confidential and Proprietary, 2003              *
    *                                                                       *
    ************************************************************************/
    /************************************************************************
    VisualOn Proprietary
    Copyright (c) 2012, VisualOn Incorporated. All rights Reserved

    VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

    All data and information contained in or disclosed by this document are
    confidential and proprietary information of VisualOn, and all rights
    therein are expressly reserved. By accepting this material, the
    recipient agrees that this material and the information contained
    therein are held in confidence and in trust. The material may only be
    used and/or disclosed as authorized in a license agreement controlling
    such use and disclosure.
    ************************************************************************/
/*******************************************************************************
    File:        VOOSMPPCMBufferImpl

    Contains:    VOOSMPPCMBufferImpl interface file

    Written by:    Jeff Huang


    Change History (most recent first):
    2012-10-24        Jeff Huang        Create file

 *******************************************************************************/

#import <Foundation/Foundation.h>
#import "VOOSMPSEIPicTiming.h"
#import "VOOSMPSEIUserDataUnregistered.h"
#import "voOnStreamType.h"

@interface VOOSMPSEIPicTimingImpl : NSObject <VOOSMPSEIPicTiming>
{
    int _cpbDpbDelaysPresentFlag;
    int _cpbRemovalDelay;
    int _dpbOutputDelay;
    int _pictureStructurePresentFlag;
    int _pictureStructure;
    int _numClockTs;
    NSArray* _clock;
}

@property (readwrite, retain, getter=getClock) NSArray * clock;

- (id) init:(VOOSMP_SEI_INFO *)pValue;

@end


@interface VOOSMPSEIUserDataUnregisteredImpl : NSObject <VOOSMPSEIUserDataUnregistered>
{
    int                 _fieldCount;
    NSMutableArray*     _fieldLength;
    NSData*             _dataBuffer;
}

@property (readwrite, retain, getter=getDataBuffer) NSData* dataBuffer;

- (int) getFieldLength:(int) i;
- (id) init:(VOOSMP_SEI_USER_DATA_UNREGISTERED *)pValue;

@end
