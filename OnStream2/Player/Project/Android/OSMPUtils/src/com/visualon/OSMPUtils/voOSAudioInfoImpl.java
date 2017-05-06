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
 * @file voOSAudioInfoImpl.java
 * implementation for voOSAudioInfo interface.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/

package com.visualon.OSMPUtils;

import java.io.UnsupportedEncodingException;

import android.os.Parcel;

public class voOSAudioInfoImpl implements voOSAudioInfo {
	/**
	 * 
	 */
	public voOSAudioInfoImpl() {
		super();
	}
	/**
	 * @param format
	 * @param language
	 */
	public voOSAudioInfoImpl(voOSAudioFormat format, String language) {
		super();
		mFormat = format;
		mLanguage = language;
	}
	
	/**
	 * @param parc  the Parcel object to convert data into this object
	 * @return true if parse OK,
	 */
	public boolean parse(Parcel parc)
	{
		mFormat = new voOSAudioFormatImpl();
		mFormat.parse(parc);
    	byte[] dt = new byte[256];
    	parc.readByteArray(dt);
    	try {
			mLanguage = new String(dt,0,256,"utf-8");
			mLanguage = mLanguage.trim();
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
//		voLog.i(this.getClass().getSimpleName(), "SubtitleInfo audio language is %s .",	mLanguage);
		return true;
	}
	
	
	private voOSAudioFormat 	mFormat;
	private String				mLanguage;
	/**
	 * @return the format
	 */
	public voOSAudioFormat Format() {
		return mFormat;
	}
	/**
	 * @return the language
	 */
	public String Language() {
		return mLanguage;
	}
}
