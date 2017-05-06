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
 * @file voOSSubtitleLanguageImpl.java
 * implementation for voOSSubtitleLanguage interface.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

public class voOSSubtitleLanguageImpl implements voOSSubtitleLanguage{
	
    String			mLangName;
    int             mLanguageType;
    int             mFlag;
    int             mReserved1;
    int             mReserved2;



	/**
	 * @param mLangName
	 * @param mLanguageType
	 * @param mFlag
	 * @param mReserved1
	 * @param mReserved2
	 */
	public voOSSubtitleLanguageImpl(String mLangName, int mLanguageType,
			int mFlag, int mReserved1, int mReserved2) {
		super();
		this.mLangName = mLangName;
		this.mLanguageType = mLanguageType;
		this.mFlag = mFlag;
		this.mReserved1 = mReserved1;
		this.mReserved2 = mReserved2;
	}


	public String LangName() {
		// TODO Auto-generated method stub
		return mLangName;
	}

	public int LanguageType() {
		// TODO Auto-generated method stub
		return mLanguageType;
	}


	public int Flag() {
		// TODO Auto-generated method stub
		return mFlag;
	}


	public int Reserved1() {
		// TODO Auto-generated method stub
		return mReserved1;
	}


	public int Reserved2() {
		// TODO Auto-generated method stub
		return mReserved2;
	}

}
