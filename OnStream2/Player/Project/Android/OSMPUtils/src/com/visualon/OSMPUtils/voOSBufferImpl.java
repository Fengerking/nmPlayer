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

public class voOSBufferImpl implements voOSBuffer {

	private long   mTimestamp;
	private int    mBufferSize;
	private byte[] mBuffer;
	
	public voOSBufferImpl()
	{
		super();
	}
	public voOSBufferImpl(long timestamp,int buffersize,byte[] buffer)
	{
		super();
		this.mBuffer = buffer;
		this.mBufferSize = buffersize;
		this.mTimestamp = timestamp;
		
	}
	
	@Override
	public long getTimestamp() {
		// TODO Auto-generated method stub
		return mTimestamp;
	}

	@Override
	public int getBufferSize() {
		// TODO Auto-generated method stub
		return mBufferSize;
	}

	@Override
	public byte[] getBuffer() {
		// TODO Auto-generated method stub
		return mBuffer;
	}

}
