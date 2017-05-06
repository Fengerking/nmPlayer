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

package com.visualon.OSMPPlayerImpl;

import com.visualon.OSMPPlayer.VOOSMPChunkInfo;

class VOOSMPChunkInfoImpl implements VOOSMPChunkInfo{
	
	int 		mType;
	String  	mRootUrl;
	String 		mUrl;
	long		mStartTime;
	long		mDuration;
	long		mTimeScale;


	/**
	 * @param mType
	 * @param mRootUrl
	 * @param mUrl
	 * @param mStartTime
	 * @param mDuration
	 * @param mTimeScale
	 */
	public VOOSMPChunkInfoImpl(int mType, String mRootUrl, String mUrl,
			long mStartTime,long mDuration, long mTimeScale) {
		super();
		this.mType = mType;
		this.mRootUrl = mRootUrl;
		this.mUrl = mUrl;
		this.mStartTime = mStartTime;
		this.mDuration = mDuration;
		this.mTimeScale = mTimeScale;
	}

	@Override
	public int getType() {
		// TODO Auto-generated method stub
		return mType;
	}


	@Override
	public String getRootURL() {
		// TODO Auto-generated method stub
		return mRootUrl;
	}


	@Override
	public String getURL() {
		// TODO Auto-generated method stub
		return mUrl;
	}


	@Override
	public long getStartTime() {
		// TODO Auto-generated method stub
		return mStartTime;
	}


	@Override
	public long getDuration() {
		// TODO Auto-generated method stub
		return mDuration;
	}


	@Override
	public long getTimeScale() {
		// TODO Auto-generated method stub
		return mTimeScale;
	}

}
