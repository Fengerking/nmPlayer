/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

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

/**
 *Protocol of Picture timing SEI message ISO/IEC 14496-10:2005 (E) Annex D 2.2
 */  
public interface VOOSMPSEIPicTiming {
	
	/**
	 * getCpbDpbDelaysPresentFlag
	 *
	 * @return cpbDpbDelaysPresentFlag
	 **/
	public int getCpbDpbDelaysPresentFlag();
	
	/**
	 * getCpbRemovalDelay
	 *
	 * @return cpbRemovalDelay
	 **/
	public int getCpbRemovalDelay();
	
	/**
	 * getDpbOutputDelay
	 *
	 * @return dpbOutputDelay
	 **/
	public int getDpbOutputDelay();
	
	/**
	 * getPictureStructurePresentFlag
	 *
	 * @return pictureStructurePresentFlag
	 **/
	public int getPictureStructurePresentFlag();
	
	/**
	 * getPictureStructure
	 *
	 * @return pictureStructure
	 **/
	public int getPictureStructure();
	
	/**
	 * getNumClockTs
	 *
	 * @return numClockTs
	 **/
	public int getNumClockTs();
	
	/**
	 * getClock
	 *
	 * @return an array {@link VOOSMPSEIClockTimestamp} of SEI clock timestamps
	 **/
	public VOOSMPSEIClockTimestamp[] getClock();

}
