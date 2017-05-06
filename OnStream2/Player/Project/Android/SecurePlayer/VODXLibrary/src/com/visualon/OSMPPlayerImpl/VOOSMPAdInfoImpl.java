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


import java.util.ArrayList;

import android.os.Parcel;

import com.visualon.OSMPPlayer.VOOSMPAdInfo;
import com.visualon.OSMPPlayer.VOOSMPAdPeriod;

class VOOSMPAdInfoImpl implements VOOSMPAdInfo
{
    private int mCount = 0;
    private ArrayList<VOOSMPAdPeriod>  mAdPeriodList = null;
    
    VOOSMPAdInfoImpl()
    { 
        super();
        mAdPeriodList = new ArrayList<VOOSMPAdPeriod>();
    }
    
    boolean parse(Parcel parc)
    {
        mCount = parc.readInt();
        for(int i = 0; i < mCount; i++)
        {
            VOOSMPAdPeriodImpl mAdPeriod = new VOOSMPAdPeriodImpl();
            mAdPeriod.parse(parc);
            mAdPeriodList.add(i, mAdPeriod);
        }
        return true;
    }
    
    public int getCount()
    {
        return mCount;
    }
    
    public ArrayList<VOOSMPAdPeriod> getPeriodList()
    {
        return mAdPeriodList;
    }
}