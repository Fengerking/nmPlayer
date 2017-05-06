/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All rights Reserved

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

import android.os.Parcel;

class VOOSMPTrackingEventInfo{
    
    private int mPid = 0;
    private int mCount = 0;
    private String[] mUrl = null;
    
    public boolean parse(Parcel parc)
    {
        mPid = parc.readInt();
        mCount  = parc.readInt();
        if(mCount > 0)
        {
            mUrl = new String[mCount];
            for(int i=0; i< mCount; i++)
            {
                mUrl[i] = parc.readString();
            }
        }
        return true;
    }
    
    public int getPid()
    {
        return mPid;
    }

    public int getCount()
    {
        return mCount;
    }
    
    public String[] getPeriodUrlList()
    {
        return mUrl;
    }
    
}