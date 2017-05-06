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

#import <Foundation/Foundation.h>

@interface VOOSMPAnalyticsFilter : NSObject
{
    int _lastTime;
    int _sourceTime;
    int _codecTime;
    int _renderTime;
    int _jitterTime;
}

/**
 * How far back to report (seconds).
 */
@property (readwrite, assign, getter=getLastTime) int lastTime;

/**
 * Number of instances that will be recorded when the source exceeds this time (ms).
 */
@property (readwrite, assign, getter=getSourceTime) int sourceTime;

/**
 * Number of instances that will be recorded when the codec exceeds this time (ms).
 */
@property (readwrite, assign, getter=getCodecTime) int codecTime;

/**
 * Number of instances that will be recorded when the render exceeds this time (ms).
 */
@property (readwrite, assign, getter=getRenderTime) int renderTime;

/**
 * Number of instances that will be recorded when the jitter exceeds this time (ms).
 */
@property (readwrite, assign, getter=getJitterTime) int jitterTime;

/**
 * Initialize filter with values.
 *
 * @param lastTime   [in] How far back to report (s).
 * @param sourceTime [in] Number of instances that will be recorded when the source exceeds this time (ms).
 * @param codecTime  [in] Number of instances that will be recorded when the codec exceeds this time (ms).
 * @param renderTime [in] Number of instances that will be recorded when the render exceeds this time (ms).
 * @param jitter     [in] Number of instances that will be recorded when the jitter exceeds this time (ms).
 *
 * @return	object if successful; nil if unsuccessful
 */
- (id) init:(int)lastTime sourceTime:(int)sourceTime codecTime:(int)codecTime renderTime:(int)renderTime jitterTime:(int)jitterTime;

@end

