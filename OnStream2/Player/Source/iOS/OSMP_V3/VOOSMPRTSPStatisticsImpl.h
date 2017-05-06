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
    File:        VOOSMPAnalyticsInfoImpl

    Contains:    VOOSMPAnalyticsInfoImpl interface file

    Written by:    Jeff Huang


    Change History (most recent first):
    2012-10-24        Jeff Huang        Create file

 *******************************************************************************/

#import <Foundation/Foundation.h>

#import "voOnStreamSourceType.h"
#import "VOOSMPRTSPStatistics.h"

@interface VOOSMPRTSPStatisticsImpl : NSObject <VOOSMPRTSPStatistics>
{
    int _packetReceived;
    int _packetDuplicated;
    int _packetLost;
    int _packetSent;
    int _averageJitter;
    int _averageLatency;
}

- (id) init:(VOOSMP_SRC_RTSP_STATS *)pValue;

@end
