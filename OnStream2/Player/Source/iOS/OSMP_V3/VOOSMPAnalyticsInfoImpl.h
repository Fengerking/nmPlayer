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

#import "voOnStreamType.h"
#import "VOOSMPAnalyticsInfo.h"

@interface VOOSMPAnalyticsInfoImpl : NSObject <VOOSMPAnalyticsInfo>
{
    int _lastTime;
    int _sourceDropNum;
    int _codecDropNum;
    int _renderDropNum;
    int _decodedNum;
    int _renderNum;
    int _sourceTimeNum;
    int _codecTimeNum;
    int _renderTimeNum;
    int _jitterNum;
    int _codecErrorsNum;
    int* _codecErrors;
    int _CPULoad;
    int _frequency;
    int _maxFrequency;
    int _worstDecodeTime;
    int _worstRenderTime;
    int _averageDecodeTime;
    int _averageRenderTime;
    int _totalCPULoad;
    int _playbackDuration;
    int _totalSourceDropNum;
    int _totalCodecDropNum;
    int _totalRenderDropNum;
    int _totalDecodedNum;
    int _totalRenderedNum;
}

- (id) init:(VOOSMP_PERFORMANCE_DATA *)pValue;

@end
