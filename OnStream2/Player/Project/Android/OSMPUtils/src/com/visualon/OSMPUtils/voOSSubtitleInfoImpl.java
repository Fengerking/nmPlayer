package com.visualon.OSMPUtils;

import java.io.UnsupportedEncodingException;

import android.os.Parcel;

public class voOSSubtitleInfoImpl implements voOSSubtitleInfo {

	private String mLanguage;
	private int mCodingType;
	private String mAlign;
	
	public boolean parse(Parcel parc)
	{
		byte[] dt = new byte[256];
    	parc.readByteArray(dt);
    	try {
    		mLanguage = (new String(dt,0,256,"utf-8"));
    		mLanguage = mLanguage.trim();
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	
		mCodingType = parc.readInt();
		
		dt = new byte[8];
		parc.readByteArray(dt);
    	try {
    		mAlign = (new String(dt,0,8,"utf-8"));
    		mAlign = mAlign.trim();
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
//		voLog.i(this.getClass().getSimpleName(), "SubtitleInfo language is %s, codingtype is %d, mAlign is %s, size is %d",
//				mLanguage, mCodingType, mAlign, mLanguage.length());
		return true;
	}
	
	public String Language() {
		// TODO Auto-generated method stub
		return mLanguage;
	}

	public int CodingType() {
		// TODO Auto-generated method stub
		return mCodingType;
	}

	public String Align() {
		// TODO Auto-generated method stub
		return mAlign;
	}

}
