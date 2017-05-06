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

import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_ADS_TRACKING_EVENT_TYPE;

public interface VOOSMPTrackingEvent {
	
	/**
	 * Get tracking event type
	 * 
	 * @return {@link VO_OSMP_ADS_TRACKING_EVENT_TYPE}
	 */
	public VO_OSMP_ADS_TRACKING_EVENT_TYPE getEventType();
	
	/**
	 * Get current period id
	 * 
	 * @return current PeriodID
	 */
	public int getPeriodID();
	
	/**
	 * Get Elapsed Time
	 * 
	 * @return elapsed time
	 */
	public long getElapsedTime();
	
	/**
	 * Get event relational value
	 * If event type is {@link VO_OSMP_ADS_TRACKING_EVENT_TYPE#VO_OSMP_ADS_TRACKING_EVENT_PERCENTAGE}, get percentage value, fg: 15%, 50% 75%,<br>
	 * if event type is {@link VO_OSMP_ADS_TRACKING_EVENT_TYPE#VO_OSMP_ADS_TRACKING_EVENT_TIMEPASSED}, get current playing passed time,<br>
	 * if event type is {@link VO_OSMP_ADS_TRACKING_EVENT_TYPE#VO_OSMP_ADS_TRACKING_EVENT_SEEKS}, get seek to position
	 * if event type is {@link VO_OSMP_ADS_TRACKING_EVENT_TYPE#VO_OSMP_ADS_TRACKING_EVENT_PAUSE}, get 1 for pause, 0 for unpause.
	 * if event type is {@link VO_OSMP_ADS_TRACKING_EVENT_TYPE#VO_OSMP_ADS_TRACKING_EVENT_PLAYER_FULLSCREEN}, get 1 for full screen , 0 for not full screen
	 * if event type is {@link VO_OSMP_ADS_TRACKING_EVENT_TYPE#VO_OSMP_ADS_TRACKING_EVENT_PLAYER_CLOSEDCAPTION}, get 1 for Closed Caption start , 0 for Closed Caption end
	 * if other event type, return -1;
	 * 
	 * @return percentage value, current AD/content playing time stamp or seek to position. 
	 */
	public long getEventValue();
	
	/**
	 * Get current playing time
	 * 
	 * 
	 * @return current playing time
	 */
	public long getPlayingTime();
	
	/**
	 * Get current event text<br>
	 * When type equal to  VO_OSMP_ADS_TRACKING_EVENT_CLICK_THROUGH, text is a http request url from VAST file.<br>
	 * Other event type, text is null
	 * 
	 * @return current event text
	 */
	public String[] getEventText();
	
	
}
