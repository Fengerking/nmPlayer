/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

package com.visualon.OSMPPlayerImpl;

import com.visualon.OSMPPlayer.VOOSMPTrackingEvent;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_ADS_TRACKING_EVENT_TYPE;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;

class VOOSMPTrackingEventImpl implements  VOOSMPTrackingEvent {
	
	private VO_OSMP_ADS_TRACKING_EVENT_TYPE mTrackingEventType;
	private int mPeriodID;
	private long mElapsedTime;
	private long mEventValue;
	private long mPlayingTime;
	private String[] mEventText;
	
	public VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE trackingEventType, 
			int periodID, long elapsedTime, long eventValue, long playingTime, String[] eventText){
		mTrackingEventType = trackingEventType;
		mPeriodID = periodID;
		mElapsedTime = elapsedTime;
		mEventValue = eventValue;
		mPlayingTime = playingTime;
		mEventText = eventText;
	}
	
	public VO_OSMP_RETURN_CODE setTrackingEventType(VO_OSMP_ADS_TRACKING_EVENT_TYPE trackingEventType){
		mTrackingEventType = trackingEventType;
		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}
	
	public VO_OSMP_RETURN_CODE setPeriodID(int periodID){
		mPeriodID = periodID;
		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}
	
	public VO_OSMP_RETURN_CODE setElapsedTime(long elapsedTime){
		mElapsedTime = elapsedTime;
		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}
	
	public VO_OSMP_RETURN_CODE setEventValue(long eventValue){
		mEventValue = eventValue;
		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}
	
	public VO_OSMP_RETURN_CODE setPlayingTime(long playingTime){
		mPlayingTime = playingTime;
		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}
	
	public VO_OSMP_ADS_TRACKING_EVENT_TYPE getEventType(){
		return mTrackingEventType;
	}
	
	public int getPeriodID(){
		return mPeriodID;
	}
	
	public long getElapsedTime(){
		return mElapsedTime;
	}
	
	public long getEventValue(){
		return mEventValue;
	}
	
	public long getPlayingTime(){
		return mPlayingTime;
	}

	@Override
	public String[] getEventText() {
		// TODO Auto-generated method stub
		return mEventText;
	}
}