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
 * @file voOSVideoFormatImpl.java
 * implementation for voOSVideoFormat interface.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

import android.os.Parcel;

public class voOSVideoFormatImpl implements voOSVideoFormat {
	
	/**
	 * 
	 */
	public voOSVideoFormatImpl() {
		super();
	}
	
	/**
	 * @param width
	 * @param height
	 * @param type
	 */
	public voOSVideoFormatImpl(int width, int height, int type) {
		super();
		mWidth = width;
		mHeight = height;
		mType = type;
	}
	
	/**
	 * @param parc  the Parcel object to convert data into this object
	 * @return true if parse OK,
	 */
	public boolean parse(Parcel parc)
	{
		mWidth = parc.readInt();
		mHeight = parc.readInt();
		mType = parc.readInt();
		return true;
	}

	private int					mWidth;		 /*!< Width */
	private int					mHeight;		 /*!< Height */
	private int					mType;		 /*!< Color type  */
	
	/**
	 * @return the width
	 */
	public int Width() {
		return mWidth;
	}
	
	/**
	 * @return the height
	 */
	public int Height() {
		return mHeight;
	}
	
	/**
	 * @return the type
	 */
	public int Type() {
		return mType;
	}
	
}
