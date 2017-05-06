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
 * @file voOSModuleVersionImpl.java
 * implementation for voOSModuleVersion interface.
 ************************************************************************/
package com.visualon.OSMPUtils;

import android.os.Parcel;

public class voOSModuleVersionImpl implements voOSModuleVersion {

    private int mModuleType = -1;
    private String  mVersion = null;
    
    /**
     * @param mModuleType
     * @param mVersion
     */
    public voOSModuleVersionImpl(int type, String ver)
    {
        mModuleType = type;
        mVersion = ver;
        
    }
	
	 public voOSModuleVersionImpl()
    {
        mModuleType = -1;
        mVersion = null;
        
    }
	
    @Override
    public String GetVersion() {
        // TODO Auto-generated method stub
        return mVersion;
    }
	
	@Override
    public int GetModuleType() {
        // TODO Auto-generated method stub
        return mModuleType;
    }

	public boolean parse(Parcel parc){
		if(parc == null)
			return false;
		
		parc.setDataPosition(0);
		mModuleType = parc.readInt();
		mVersion = parc.readString();
		return true;
	}

}
