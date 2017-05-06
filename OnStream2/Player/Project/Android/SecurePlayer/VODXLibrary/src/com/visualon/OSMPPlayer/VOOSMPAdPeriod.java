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

public interface VOOSMPAdPeriod
{
    /**
     * This index indicates the type of period is normal content.
     */
	public static final int VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT = 0;
	
	/**
	 * This index indicates the type of period is ads.
	 */
	public static final int VO_ADSMANAGER_PERIODTYPE_ADS =1;

    /**
     * Get the ID.
     * 
     * @return the id.
     */
	public int getID();

    /**
     * Get the type of period.
     * 
     * @return the type of period.
     */
	public int getPeriodType();
	
    /**
     * Get the url of period.
     * 
     * @return the url of period.
     */
	public String getPeriodURL();
	
    /**
     * Get the start time of period.
     * 
     * @return the start time of period.
     */
	public long getStartTime();
	
    /**
     * Get the end time of period.
     * 
     * @return the end time of period.
     */
	public long getEndTime();
	
    /**
     * Get the url of caption.
     * 
     * @return the url of caption.
     */
	public String getCaptionURL();
	
    /**
     * Get the title of period.
     * 
     * @return the title of period.
     */
	public String getPeriodTitle();
	
    /**
     * Get the id of period.
     * 
     * @return the id of period.
     */
	public String getPeriodID();
	
    /**
     * Get the id of content.
     * 
     * @return the id of content.
     */
	public String getContentID();
	
    /**
     * Judge if the url is live.
     * 
     * @return true:live false:not live.
     */
	public boolean isLive();
	
    /**
     * Judge if the period is episode.
     * 
     * @return true:episode false:not episode.
     */
	public boolean isEpisode();
}