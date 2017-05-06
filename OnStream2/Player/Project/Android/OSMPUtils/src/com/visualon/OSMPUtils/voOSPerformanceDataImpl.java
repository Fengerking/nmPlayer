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
 * @file voOSAudioFormat.java
 * implementation for voOSAudioFormat interface.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

public class voOSPerformanceDataImpl implements voOSPerformanceData {
	
	int		mCodecType;
	int		mBitRate;
	int		mVideoWidth;
	int		mVideoHeight;
	int		mProfileLevel;
	int		mFPS;
	
	/**
     * @return please refer to {@link voOSType.VOOSMP_VIDEO_CODINGTYPE}
     */ 
	public int		CodecType()
	{
		return mCodecType;
	}
	

	public int		BitRate()
	{
		return mBitRate;
	}
	

	public int		VideoWidth()
	{
		return mVideoWidth;
	}
	

	public int		VideoHeight()
	{
		return mVideoHeight;
	}
	

	public int		ProfileLevel()
	{
		return mProfileLevel;
	}
	
	public int		FPS()
	{
		return mFPS;
	}


	/**
	 * @param nCodecType
	 * @param nBitRate
	 * @param nVideoWidth
	 * @param nVideoHeight
	 * @param nProfileLevel
	 * @param nFPS
	 */
	public voOSPerformanceDataImpl(int nCodecType, int nBitRate,
			int nVideoWidth, int nVideoHeight, int nProfileLevel, int nFPS) {
		super();
		this.mCodecType = nCodecType;
		this.mBitRate = nBitRate;
		this.mVideoWidth = nVideoWidth;
		this.mVideoHeight = nVideoHeight;
		this.mProfileLevel = nProfileLevel;
		this.mFPS = nFPS;
	}

}
