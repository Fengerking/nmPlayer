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
#import "VOOSMPSEIClockTimestamp.h"
#import "voOnStreamType.h"

@interface VOOSMPSEIClockTimeStampImpl : NSObject <VOOSMPSEIClockTimestamp>
{
    int _clockTimestampFlag;
    int _ctType;
    int _nuitFieldBasedFlag;
    int _countingType;
    int _fullTimestampFlag;
    int _discontinuityFlag;
    int _countDroppedFlag;
    int _frames;
    int _secondsValue;
    int _minutesValue;
    int _hoursValue;
    int _secondsFlag;
    int _minutesFlag;
    int _hoursFlag;
    int _timeOffset;
}

- (id) init:(VOOSMP_CLOCK_TIME_STAMP *)pValue;

@end
