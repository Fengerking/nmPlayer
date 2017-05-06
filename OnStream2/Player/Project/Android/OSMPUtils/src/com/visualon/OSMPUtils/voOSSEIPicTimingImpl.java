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
 * @file voOSSEIPicTimingImpl.java
 * implementation for voOSSEIPicTiming interface.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

/**
 * Structure of SEI of picture timing
 */  
public class voOSSEIPicTimingImpl implements voOSSEIPicTiming {
	
	private static String TAG = "@@@voOSSEIPicTimingImpl";
	
	/**
	 * @param mCpbDpbDelaysPresentFlag
	 * @param mCpbRemovalDelay
	 * @param mDpbOutputDelay
	 * @param mPictureStructurePresentFlag
	 * @param mPictureStructure
	 * @param mNumClockTs
	 * @param mOSClockArr
	 */
	public voOSSEIPicTimingImpl(int mCpbDpbDelaysPresentFlag,
			int mCpbRemovalDelay, int mDpbOutputDelay,
			int mPictureStructurePresentFlag, int mPictureStructure,
			int mNumClockTs, voOSSEIClockTimeStampImpl[] mOSClockArr) {
		super();
		this.mCpbDpbDelaysPresentFlag = mCpbDpbDelaysPresentFlag;
		this.mCpbRemovalDelay = mCpbRemovalDelay;
		this.mDpbOutputDelay = mDpbOutputDelay;
		this.mPictureStructurePresentFlag = mPictureStructurePresentFlag;
		this.mPictureStructure = mPictureStructure;
		this.mNumClockTs = mNumClockTs;
		this.mOSClockArr = mOSClockArr;
	}

	private int mCpbDpbDelaysPresentFlag;
	private int mCpbRemovalDelay;
	private int mDpbOutputDelay;
	private int mPictureStructurePresentFlag;
	private int mPictureStructure;
	private int mNumClockTs;
	private voOSSEIClockTimeStampImpl[] mOSClockArr = null;//new voOSClockTimeStamp[3];
	 
	public int getCpbDpbDelaysPresentFlag(){
		return mCpbDpbDelaysPresentFlag;
	}
	
	public int getCpbRemovalDelay(){
		return mCpbRemovalDelay;
	}
	
	public int getDpbOutputDelay(){
		return mDpbOutputDelay;
	}
	
	public int getPictureStructurePresentFlag(){
		return mPictureStructurePresentFlag;
	}
	
	public int getPictureStructure(){
		return mPictureStructure;
	}
	
	public int getNumClockTs(){
		return mNumClockTs;
	}
	
	public voOSSEIClockTimeStamp[] getClock(){
		return mOSClockArr;
	}
	
	public static voOSSEIPicTimingImpl parse(int[] data)
	{
		if (data == null || data.length < 51)
		{
			voLog.e(TAG, "voOSSEIPicTimingImpl parse failed, data is " + data);
			return null;
		}
		voOSSEIClockTimeStampImpl clockImpl1 = new voOSSEIClockTimeStampImpl(data[6], data[7], data[8], data[9], data[10]
		                                                                    ,data[11], data[12], data[13], data[14], data[15]
		                                                                    ,data[16], data[17], data[18], data[19], data[20]);
		
		voOSSEIClockTimeStampImpl clockImpl2 = new voOSSEIClockTimeStampImpl(data[21], data[22], data[23], data[24], data[25]
  		                                                                    ,data[26], data[27], data[28], data[29], data[30]
  		                                                                    ,data[31], data[32], data[33], data[34], data[35]);
		
		voOSSEIClockTimeStampImpl clockImpl3 = new voOSSEIClockTimeStampImpl(data[36], data[37], data[38], data[39], data[40]
		                                                                    ,data[41], data[42], data[43], data[44], data[45]
		                                                                    ,data[46], data[47], data[48], data[49], data[50]);
		
		voOSSEIClockTimeStampImpl[] clockImplArr = new voOSSEIClockTimeStampImpl[]{clockImpl1, clockImpl2, clockImpl3};
		voOSSEIPicTimingImpl impl = new voOSSEIPicTimingImpl(data[0], data[1], data[2], data[3], data[4], data[5], clockImplArr);
		return impl;
	}



}
