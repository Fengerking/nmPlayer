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

/************************************************************************
 * @file voOSDVRInfoImpl.java
 * implementation for voOSDVRInfo interface.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

public class voOSDVRInfoImpl implements voOSDVRInfo {
	
	long mStartTime;
    long mEndTime;
    long mPlayingTime;
    long mLiveTime;

	/**
	 * 
	 */
	public voOSDVRInfoImpl() {
		super();
	}

	/**
	 * @param mStartTime
	 * @param mEndTime
	 * @param mPlayingTime
	 * @param mLiveTime
	 */
	public voOSDVRInfoImpl(long mStartTime, long mEndTime, long mPlayingTime,
			long mLiveTime) {
		super();
		this.mStartTime = mStartTime;
		this.mEndTime = mEndTime;
		this.mPlayingTime = mPlayingTime;
		this.mLiveTime = mLiveTime;
	}

	@Override
	public long getStartTime() {
		// TODO Auto-generated method stub
		return mStartTime;
	}

	@Override
	public long getEndTime() {
		// TODO Auto-generated method stub
		return mEndTime;
	}

	@Override
	public long getPlayingTime() {
		// TODO Auto-generated method stub
		return mPlayingTime;
	}

	@Override
	public long getLiveTime() {
		// TODO Auto-generated method stub
		return mLiveTime;
	}

}
