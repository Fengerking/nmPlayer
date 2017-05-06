/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved

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

import android.os.Parcel;

import com.visualon.OSMPPlayer.VOOSMPTrackingTimePassedInfo;

class VOOSMPTrackingTimePassedInfoImpl implements VOOSMPTrackingTimePassedInfo{
	private int mPeriodID;
	private long mPassedTime;

	public boolean parse(Parcel parc)
	{
		mPeriodID = parc.readInt();
		mPassedTime  = parc.readLong();
		return true;
	}
	
	@Override
	public int getPeriodID() {
		// TODO Auto-generated method stub
		return mPeriodID;
	}

	@Override
	public long getPassedTime() {
		// TODO Auto-generated method stub
		return mPassedTime;
	}

}