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

#ifndef SKIP_IN_MAIN_DOCUMENTATION
#import "VOOSMPAnalyticsFilter.h"
#import "VOOSMPAnalyticsInfo.h"
#endif

#import "VOOSMPType.h"

@protocol VOCommonPlayerAnalytics <NSObject>

/**
 * Set cache time and start analytics data collection.
 *
 * @param   cacheTime  [in] Time <seconds> to cache the analytics data.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) enableAnalytics:(int)cacheTime;


/**
 * Get current analytics data.
 *
 * @param   filter   [in] Filters, specified as an {@link VOOSMPAnalyticsFilter} object,
 *                       to be applied to cached analytic data before being returned.
 *
 * @param   data     [out] Filtered analytics data. Refer to {@link VOOSMPAnalyticsInfo}.
 *
 * @return  A {@link VOOSMPAnalyticsInfo} object if successful; nil if unsuccessful
 */
- (id<VOOSMPAnalyticsInfo>) getAnalytics:(VOOSMPAnalyticsFilter *)filter;


/**
 * Get video decoding bitrate(bps) for each of the last 10 seconds
 *
 * @return an array(NSNumber) of size 10 in which elements are the video
 *         decoding bitrates of each of the last 10 seconds.
 *         The bitrate information is arranged in the way that
 *         the smaller the array index is, the closer
 *         the bitrate is to the current time. That is,
 *         element 0 is the bitrate of the closest second
 *         before the current time; element 1 is the bitrate
 *         that is 1 second before element 0; element 2 is the
 *         bitrate that is 1 second before element 1, and so on.
 */
- (NSArray *) getVideoDecodingBitrate;


/**
 * Get audio decoding bitrate(bps) for each of the last 10 seconds
 *
 * @return an array(NSNumber) of size 10 in which elements are the audio
 *         decoding bitrates of each of the last 10 seconds.
 *         The bitrate information is arranged in the way that
 *         the smaller the array index is, the closer
 *         the bitrate is to the current time. That is,
 *         element 0 is the bitrate of the closest second
 *         before the current time; element 1 is the bitrate
 *         that is 1 second before element 0; element 2 is the
 *         bitrate that is 1 second before element 1, and so on.
 */
- (NSArray *) getAudioDecodingBitrate;

@end
