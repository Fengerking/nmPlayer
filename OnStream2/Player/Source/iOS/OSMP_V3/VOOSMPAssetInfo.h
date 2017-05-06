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
#import "VOCommonPlayerAssetSelection.h"

@interface VOOSMPAssetIndexImpl : NSObject <VOOSMPAssetIndex>
{
    int _videoIndex;
    int _audioIndex;
    int _subtitleIndex;
}
-(void) setVideoIndex:(int)videoIndex;
-(void) setAudioIndex:(int)audioIndex;
-(void) setSubtitleIndex:(int)subtitleIndex;
@end


@interface VOOSMPAssetInfo : NSObject <VOOSMPAssetProperty>
{
    NSMutableArray* _assetProperty;
}

@property (retain) NSMutableArray* assetProperty;

- (id) init:(VOOSMP_SRC_TRACK_PROPERTY *)pValue;
@end
