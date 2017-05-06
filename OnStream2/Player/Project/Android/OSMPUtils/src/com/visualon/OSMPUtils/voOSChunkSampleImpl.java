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
 
 package com.visualon.OSMPUtils;

public class voOSChunkSampleImpl implements voOSChunkSample{
	
	int 		mFlag;
	long		mChunkStartTime;
    int         mPeriodSequenceNumber;
    long        mPeriodFirstChunkStartTime;
	long		mSampleTime;
	int         mSampleTimePointer;

	public voOSChunkSampleImpl(int mFlag, long mChunkStartTime, int mPeriodSequenceNumber, long mPeriodFirstChunkStartTime, long mSampleTime, int mSampleTimePointer) {
		super();
		this.mFlag = mFlag;
		this.mChunkStartTime = mChunkStartTime;
        this.mPeriodSequenceNumber = mPeriodSequenceNumber;
        this.mPeriodFirstChunkStartTime = mPeriodFirstChunkStartTime;
		this.mSampleTime = mSampleTime;
		this.mSampleTimePointer = mSampleTimePointer;
	}


	public int getFlag() {
		// TODO Auto-generated method stub
		return mFlag;
	}


	public long getChunkStartTime() {
		// TODO Auto-generated method stub
		return mChunkStartTime;
	}

    public int getPeriodSequenceNumber() {
        return mPeriodSequenceNumber;
    }
    
    public long getPeriodFirstChunkStartTime() {
        return mPeriodFirstChunkStartTime;
    }

	public long getSampleTime() {
		// TODO Auto-generated method stub
		return mSampleTime;
	}

	public int getSampleTimePointer() {
		// TODO Auto-generated method stub
		return mSampleTimePointer;
	}
	
}