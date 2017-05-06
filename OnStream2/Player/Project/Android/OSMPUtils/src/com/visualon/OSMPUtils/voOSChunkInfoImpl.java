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
 * @file voOSChunkInfoImpl.java
 * implementation for voOSChunkInfo interface.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

public class voOSChunkInfoImpl implements voOSChunkInfo{
	
	int 		mType;
	String  	mRootUrl;
	String 		mUrl;
	long		mStartTime;
	int         mPeriodSequenceNumber;
	long		mDuration;
	long		mTimeScale;
	int			mReserved1;
	int			mReserved2;


	/**
	 * @param mType
	 * @param mRootUrl
	 * @param mUrl
	 * @param mStartTime
	 * @param mDuration
	 * @param mTimeScale
	 * @param mReserved1
	 * @param mReserved2
	 */
	public voOSChunkInfoImpl(int mType, String mRootUrl, String mUrl,
			long mStartTime, int mPeriodSequenceNumber, long mDuration, long mTimeScale, int mReserved1,
			int mReserved2) {
		super();
		this.mType = mType;
		this.mRootUrl = mRootUrl;
		this.mUrl = mUrl;
		this.mStartTime = mStartTime;
		this.mPeriodSequenceNumber = mPeriodSequenceNumber;
		this.mDuration = mDuration;
		this.mTimeScale = mTimeScale;
		this.mReserved1 = mReserved1;
		this.mReserved2 = mReserved2;
	}


	public int Type() {
		// TODO Auto-generated method stub
		return mType;
	}


	public String RootUrl() {
		// TODO Auto-generated method stub
		return mRootUrl;
	}


	public String Url() {
		// TODO Auto-generated method stub
		return mUrl;
	}


	public long StartTime() {
		// TODO Auto-generated method stub
		return mStartTime;
	}

	public int PeriodSequenceNumber() {
        return mPeriodSequenceNumber;
    }

	public long Duration() {
		// TODO Auto-generated method stub
		return mDuration;
	}

	
	public long TimeScale() {
		// TODO Auto-generated method stub
		return mTimeScale;
	}


	public int Reserved1() {
		// TODO Auto-generated method stub
		return mReserved1;
	}


	public int Reserved2() {
		// TODO Auto-generated method stub
		return mReserved2;
	}

}
