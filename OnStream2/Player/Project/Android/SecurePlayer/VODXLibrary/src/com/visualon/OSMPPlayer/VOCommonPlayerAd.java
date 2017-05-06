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

package com.visualon.OSMPPlayer;

import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_LAYOUT_TYPE;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;


public interface VOCommonPlayerAd {

/**
 *  Takes an ad call and plays the ad. Allows a business unit to play an ad call directly, typically in conjunction with the loadVideoByUrl() API call. This allows an app developer to dynamically create a playlist of content videos and ad videos
 * @param   adCall     [in] string of Ad.
 * @return  {@link VO_OSMP_ERR_NONE} if successful.
 */
VO_OSMP_RETURN_CODE loadAdCall(String adCall);

/**
 * Directly plays a video url. Can be uses with the loadAdCall() to alternate ads and videos
 * @param   adCall     [in] string of video content.
 * @return  {@link VO_OSMP_ERR_NONE} if successful.
 */
VO_OSMP_RETURN_CODE loadVideoByUrl(String videoURL);

/**
 *  Get ad click-thru url
 * @return  Return the ad click-thru url as a string for the current video ad or returns null if no click-thru url value.
 */
String getVideoAdClickThru();
	
/**
 * Load the specified video and begins playing video at the specified start time.
 *
 * @param   videoID     [in] PID from application that returns a SMIL file with video and Ad metadata.
 * @param   parentID    [in] Unknown,wait customer feedback.
 * @param   startTime   [in] The specified start time to playback.
 * @param   flag	    [in] The current player is debug mode or product mode, if debug mode, value is {@link VO_OSMP_ADS_OPENFLAG_DEBUG}
 * @param   type	    [in] The current player layout type, refer to {@link VO_OSMP_LAYOUT_TYPE}.
 * @return  {@link VO_OSMP_ERR_NONE} if successful.
 */
 VO_OSMP_RETURN_CODE loadVideoById(String videoID, String parentID, int startTime, int flag, VO_OSMP_LAYOUT_TYPE type);
 
/**
 *  Set ad tracking
 * @return  return  {@link VO_OSMP_ERR_NONE} if successful.
 */

 VO_OSMP_RETURN_CODE setAdTrackingAPI(VOOSMPAdTracking tracking);
 
/**
 *  enable is full screen
 *  
 *  @param   isFullScreen    [in] true:full screen; false: not full screen.
 *  @return  return  {@link VO_OSMP_ERR_NONE} if successful.
 */

 VO_OSMP_RETURN_CODE enableFullScreen(boolean isFullScreen);
}

