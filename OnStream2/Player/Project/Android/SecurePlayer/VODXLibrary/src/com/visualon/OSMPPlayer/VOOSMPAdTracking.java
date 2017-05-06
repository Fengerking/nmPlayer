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

import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;

public interface VOOSMPAdTracking {

    /** init Tracking
    *
    * @return {@link VO_OSMP_ERR_NONE} if successful.
    * 
    * 
    */
	public VO_OSMP_RETURN_CODE initTracking();
	
    /** uninit Tracking
    *
    * @return {@link VO_OSMP_ERR_NONE} if successful.
    * 
    * 
    */
	public VO_OSMP_RETURN_CODE uninitTracking();
	   
    /** Set Playback info
    *
    * @param {@link VOOSMPADSInfo}
    * 
    * @return {@link VO_OSMP_ERR_NONE} if successful.
    * 
    */
	public VO_OSMP_RETURN_CODE setPlaybackInfo(VOOSMPAdInfo adsInfo);
	  	
	
	/** Send Tracking event
	 * 
	 * @param event {@link VOOSMPTrackingEvent}
	 * 
	 * @return  {@link VO_OSMP_ERR_NONE} if successful.
	 * 
	 */
	public VO_OSMP_RETURN_CODE sendTrackingEvent(VOOSMPTrackingEvent event);
	
	/** Notify if an new link be played
	 * 
	 * @return  {@link VO_OSMP_ERR_NONE} if successful.
	 * 
	 */
	public VO_OSMP_RETURN_CODE notifyPlayNewVideo();
}
  
