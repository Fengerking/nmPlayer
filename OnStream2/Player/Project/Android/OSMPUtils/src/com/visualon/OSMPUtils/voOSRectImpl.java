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
 * @file voOSRectImpl.java
 * implementation for voOSRect interface.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

public class voOSRectImpl implements voOSRect {
	
	int			mLeft;
	int			mTop;
	int			mRight;
	int			mBottom;

	public voOSRectImpl(int nLeft, int nTop, int nRight, int nBottom) {
		super();
		this.mLeft = nLeft;
		this.mTop = nTop;
		this.mRight = nRight;
		this.mBottom = nBottom;
	}

	public int Left() {
		// TODO Auto-generated method stub
		return mLeft;
	}

	public int Top() {
		// TODO Auto-generated method stub
		return mTop;
	}

	public int Right() {
		// TODO Auto-generated method stub
		return mRight;
	}

	public int Bottom() {
		// TODO Auto-generated method stub
		return mBottom;
	}

}
