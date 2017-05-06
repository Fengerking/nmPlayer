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

public class VOOSMPTrackingAction{
	private int mAction;
	private int mPeriodID;
	private long mElapsedTime;
	
	public boolean parse(Parcel parc)
	{
		mAction = parc.readInt();
		mPeriodID  = parc.readInt();
		mElapsedTime  = parc.readLong();
//		parc.recycle();
		return true;
	}
	
	public int getAction()
	{
		return mAction;
	}

	public int getPeriodID()
	{
		return mPeriodID;
	}
	
	public long getElapsedTime()
	{
		return mElapsedTime;
	}
}