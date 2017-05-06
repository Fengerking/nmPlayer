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

/**protocol of Picture timing ISO/IEC 14496-10:2005 (E) Annex D 2.2*/
public interface VOOSMPSEIClockTimestamp {
	
	/**
	 * getClockTimestampFlag
	 *
	 * @return clockTimestampFlag
	 **/
	public int getClockTimestampFlag();
	
	/**
	 * getCtType
	 *
	 * @return ctType
	 **/
	public int getCtType();
	
	/**
	 * getNuitFieldBasedFlag
	 *
	 * @return nuitFieldBasedFlag
	 **/
	public int getNuitFieldBasedFlag();
	
	/**
	 * getCountingType
	 *
	 * @return countingType
	 **/
	public int getCountingType();
	
	/**
	 * getFullTimestampFlag
	 *
	 * @return fullTimestampFlag
	 **/
	public int getFullTimestampFlag();
	
	/**
	 * getDiscontinuityFlag
	 *
	 * @return discontinuityFlag
	 **/
	public int getDiscontinuityFlag();
	
	/**
	 * getCntDroppedFlag
	 *
	 * @return countDroppedFlag
	 **/
	public int getCntDroppedFlag();
	
	/**
	 * getFrames
	 *
	 * @return frames
	 **/
	public int getFrames();
	
	/**
	 * getSecondsValue
	 *
	 * @return secondsValue
	 **/
	public int getSecondsValue();
	
	/**
	 * getMinutesValue
	 *
	 * @return minutesValue
	 **/
	public int getMinutesValue();
	
	/**
	 * getHoursValue
	 *
	 * @return hoursValue
	 **/
	public int getHoursValue();
	
	/**
	 * getSecondsFlag
	 *
	 * @return secondsFlag
	 **/
	public int getSecondsFlag();
	
	/**
	 * getMinutesFlag
	 *
	 * @return minutesFlag
	 **/
	public int getMinutesFlag();
	
	/**
	 * getHoursFlag
	 *
	 * @return hoursFlag
	 **/
	public int getHoursFlag();

	/**
	 * getTimeOffset
	 *
	 * @return timeOffset
	 **/
	public int getTimeOffset();


}
