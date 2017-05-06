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

import android.os.Parcel;

public class voOSAudioFormatImpl implements voOSAudioFormat {

	/**
	 * @param sampleRate
	 * @param channels
	 * @param sampleBits
	 */
	public voOSAudioFormatImpl(int sampleRate, int channels, int sampleBits) {
		super();
		mSampleRate = sampleRate;
		mChannels = channels;
		mSampleBits = sampleBits;
	}
	
	/**
	 * 
	 */
	public voOSAudioFormatImpl() {
		super();
	}
	
	/**
	 * @param parc  the Parcel object to convert data into this object
	 * @return true if parse OK,
	 */
	public boolean parse(Parcel parc)
	{
		mSampleRate = parc.readInt();
		mChannels = parc.readInt();
		mSampleBits = parc.readInt();
		return true;
	}
	
	private int		mSampleRate;  /*!< Sample rate */
	private int		mChannels;    /*!< Channel count */
	private int		mSampleBits;  /*!< Bits per sample */
	
	/**
	 * @return the sampleRate
	 */
	public int SampleRate() {
		return mSampleRate;
	}
	
	/**
	 * @return the channels
	 */
	public int Channels() {
		return mChannels;
	}
		
	/**
	 * @return the sampleBits
	 */
	public int SampleBits() {
		return mSampleBits;
	}
}
