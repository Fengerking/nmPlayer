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
 * @file voOSVideoInfoImpl.java
 * implementation for voOSVideoInfo interface.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

import android.os.Parcel;

public class voOSVideoInfoImpl implements voOSVideoInfo {
	/**
	 * 
	 */
	public voOSVideoInfoImpl() {
		super();
		mFormat = null;
		mAngle = 0;
		mVideoDesc = null;
	}
	/**
	 * @param format
	 * @param angle
	 */
	public voOSVideoInfoImpl(voOSVideoFormat format, int angle) {
		super();
		mFormat = format;
		mAngle = angle;
	}
	private voOSVideoFormat	 	mFormat;
	private int					mAngle;
	private String              mVideoDesc;

	/**
	 * @param parc  the Parcel object to convert data into this object
	 * @return true if parse OK,
	 */
	public boolean parse(Parcel parc)
	{
		mFormat = new voOSVideoFormatImpl();
		mFormat.parse(parc);
		mAngle = parc.readInt();
		return true;
	}
	
	/**
	 * @return the format
	 */
	public voOSVideoFormat Format() {
		return mFormat;
	}
	/**
	 * @return the angle
	 */
	public int Angle() {
		return mAngle;
	}
	@Override
	public String VideoDesc() {
		// TODO Auto-generated method stub
		return null;
	}
}
