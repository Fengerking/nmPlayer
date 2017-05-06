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

import com.visualon.OSMPPlayer.VOOSMPAdPeriod;
import com.visualon.OSMPUtils.voLog;

import android.os.Parcel;


class VOOSMPAdPeriodImpl implements VOOSMPAdPeriod
{     
    private int mId;
    private int mPeriodType;
    private String mPeriodURL;
    private long mStartTime;
    private long mEndTime;
    private String mCaptionURL;
    private String mPeriodTitle;
    private String mPeriodID;
    private int isLive;
    private int isEpisode;
    
    
    VOOSMPAdPeriodImpl()
    { 
        super();
    }
    
    boolean parse(Parcel parc)
    {
        mId         = parc.readInt();
        mPeriodType = parc.readInt();
        mPeriodURL  = parc.readString();
        mStartTime  = parc.readLong();
        mEndTime    = parc.readLong();
        mCaptionURL = parc.readString();
        mPeriodTitle = parc.readString();
        mPeriodID = parc.readString();
        isLive = parc.readInt();
        isEpisode = parc.readInt();
//      parc.recycle();
        voLog.d("adsCallBack", "VOOSMPADSPeriod mId = " + mId + "  mPeriodType = " + mPeriodType + "  mPeriodURL = " + mPeriodURL
                + "mStartTime = " + mStartTime + "  mEndTime = " + mEndTime + "  mCaptionURL = " + mCaptionURL
                + "mPeriodTitle = " + mPeriodTitle + "  mContentID = " + mPeriodID);
        return true;
    }
    
    public int getID()
    {
        return mId;
    }

    public int getPeriodType()
    {
        return mPeriodType;
    }
    
    public String getPeriodURL()
    {
        return mPeriodURL;
    }
    
    public long getStartTime()
    {
        return mStartTime;
    }
    
    public long getEndTime()
    {
        return mEndTime;
    }
    
    public String getCaptionURL()
    {
        return mCaptionURL;
    }
    
    public String getPeriodTitle()
    {
        return mPeriodTitle;
    }
    
    public String getPeriodID()
    {
        return mPeriodID;
    }
    
    public String getContentID()
    {
        return mPeriodID;
    }
    
    public boolean isLive()
    {
        if(isLive == 0)
        return false;
        else
        return true;
    }
    
    public boolean isEpisode()
    {
        if(isEpisode == 0)
        return false;
        else
        return true;
    }
}