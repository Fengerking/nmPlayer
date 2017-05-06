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
 * @file voOSTimedTagImpl.java
 * implementation for voOSTimedTag interface.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

public class voOSTimedTagImpl implements voOSTimedTag {
	
	private long		mTimeStamp;				/*!<Time stamp*/
	
	private int			mSize;					/*!<Size */
	private byte[]		mData;					/*!<Data */
		
	private int			mFlag;					/*!<For further use */
	private Object		mReserve;				/*!<Data pointor */

	/**
	 * 
	 */
	public voOSTimedTagImpl() {
		super();
	}

	/**
	 * @param mTimeStamp
	 * @param mSize
	 * @param mData
	 * @param mFlag
	 * @param mReserve
	 */
	public voOSTimedTagImpl(long mTimeStamp, int mSize, byte[] mData,
			int mFlag, Object mReserve) {
		super();
		this.mTimeStamp = mTimeStamp;
		this.mSize = mSize;
		this.mData = mData;
		this.mFlag = mFlag;
		this.mReserve = mReserve;
	}

	@Override
	public long TimeStamp() {
		// TODO Auto-generated method stub
		return mTimeStamp;
	}

	@Override
	public int Size() {
		// TODO Auto-generated method stub
		return mSize;
	}

	@Override
	public byte[] Data() {
		// TODO Auto-generated method stub
		return mData;
	}

	@Override
	public int Flag() {
		// TODO Auto-generated method stub
		return mFlag;
	}

	@Override
	public Object Reserve() {
		// TODO Auto-generated method stub
		return mReserve;
	}

}
