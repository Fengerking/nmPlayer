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

import android.os.Parcel;

public class voOSPCMBufferImpl extends voOSBufferImpl implements voOSPCMBuffer {
	
	private long   mTimestamp;
	private int    mBufferSize;
	private byte[] mBuffer;
	
	public voOSPCMBufferImpl()
	{
		super();
	}
	public voOSPCMBufferImpl(long timestamp,int buffersize,byte[] buffer)
	{
		this.mTimestamp = timestamp;
		this.mBufferSize = buffersize;
		this.mBuffer = buffer;
	}
	public boolean parse(Parcel parc){
		if(parc == null)
			return false;
		
		parc.setDataPosition(0);
		mTimestamp = parc.readLong();
		mBufferSize = parc.readInt();
		if(mBufferSize > 0){
			mBuffer = new byte[mBufferSize];
			parc.readByteArray(mBuffer);
		}
		
		return true;
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
