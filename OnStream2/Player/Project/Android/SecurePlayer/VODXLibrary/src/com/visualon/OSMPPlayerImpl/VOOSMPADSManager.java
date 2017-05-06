/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.visualon.OSMPPlayerImpl;

import java.io.File;
import java.util.ArrayList;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.Parcel;

import com.visualon.OSMPPlayer.VOOSMPAdInfo;
import com.visualon.OSMPPlayer.VOOSMPAdPeriod;
import com.visualon.OSMPPlayerImpl.VOOSMPTrackingAction;
import com.visualon.OSMPPlayer.VOOSMPTrackingEvent;
import com.visualon.OSMPPlayerImpl.VOOSMPTrackingPercentage;
import com.visualon.OSMPPlayer.VOOSMPTrackingTimePassedInfo;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_ADS_TRACKING_EVENT_TYPE;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_LAYOUT_TYPE;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;
import com.visualon.OSMPPlayerImpl.VOOSMPADSeekInfo;
import com.visualon.OSMPUtils.voLog;
import com.visualon.OSMPUtils.voOSType;
import com.visualon.OSMPPlayer.VOOSMPAdTracking;

class VOOSMPADSManager
{
    private String TAG = "@@@VOOSMPADSManager";
    public static final int VO_ADSMANAGER_OPENFLAG_PID         = 0x00000001;
    public static final int VO_ADSMANAGER_OPENFLAG_ASYNCOPEN   = 0x00001000; //Indicate open should be async mode
    
    
    public static final int VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT   = 0;
    public static final int VO_ADSMANAGER_PERIODTYPE_ADS   = 1;
    
    public static final int VO_ADSMANAGER_PID_EVENTCALLBACK             = 0x1230adcb;   //Indicate that the Param will be VO_ADSMANAGER_EVENTCALLBACK*
    public static final int VO_ADSMANAGER_PID_DISCONTINUECHUNK          = 0x1230adcc;   //Indicate that the Param will be VO_ADSMANAGER_CHUNKSAMPLEINFO*
    public static final int VO_ADSMANAGER_PID_DISCONTINUESAMPLE         = 0x1230adcd;   //Indicate that the Param will be VO_ADSMANAGER_CHUNKSAMPLEINFO*
    public static final int VO_ADSMANAGER_PID_STARTSEEKADS              = 0x1230adce;
    public static final int VO_ADSMANAGER_PID_STARTSEEKCONTENT          = 0x1230adcf;
    
    public static final int VO_ADSMANAGER_EVENT_BASE            = 0xecb00000;
    public static final int VO_ADSMANAGER_EVENT_CONTENT_START   = ( VO_ADSMANAGER_EVENT_BASE | 0x00000001 );
    public static final int VO_ADSMANAGER_EVENT_CONTENT_END     = ( VO_ADSMANAGER_EVENT_BASE | 0x00000002 );
    public static final int VO_ADSMANAGER_EVENT_AD_START        = ( VO_ADSMANAGER_EVENT_BASE | 0x00000003 );
    public static final int VO_ADSMANAGER_EVENT_AD_END          = ( VO_ADSMANAGER_EVENT_BASE | 0x00000004 );
    public static final int VO_ADSMANAGER_EVENT_OPEN_COMPLETE   = ( VO_ADSMANAGER_EVENT_BASE | 0x00000005 );               //nParam1 is Open return value VO_U32 */
    public static final int VO_ADSMANAGER_EVENT_WHOLECONTENT_START      = ( VO_ADSMANAGER_EVENT_BASE | 0x00000006 );               //nParam1 is VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO*
    public static final int VO_ADSMANAGER_EVENT_WHOLECONTENT_END        = ( VO_ADSMANAGER_EVENT_BASE | 0x00000007 );               //nParam1 is VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO*

    public static final int VO_ADSMANAGER_EVENT_TRACKING_BASE           = ( VO_ADSMANAGER_EVENT_BASE | 0x00010000 );
    //  public static final int VO_ADSMANAGER_EVENT_TRACKING_PRECENTAGE     = ( VO_ADSMANAGER_EVENT_TRACKING_BASE | 0x00000001 );       //this event show there is a percentage event, the nParam1 is VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO*
    public static final int VO_ADSMANAGER_EVENT_TRACKING_ACTION         = ( VO_ADSMANAGER_EVENT_TRACKING_BASE | 0x00000002 );       //this event show there is an action event, the nParam1 is VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO*
    public static final int VO_ADSMANAGER_EVENT_TRACKING_PERIODPRECENTAGE           =( VO_ADSMANAGER_EVENT_TRACKING_BASE | 0x00000003 );        //this event show there is a period percentage event, the nParam1 is VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO*
    public static final int VO_ADSMANAGER_EVENT_TRACKING_WHOLECONTENTPRECENTAGE     =( VO_ADSMANAGER_EVENT_TRACKING_BASE | 0x00000004 );        //this event show there is a percentage event, the nParam1 is VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO*
    public static final int VO_ADSMANAGER_EVENT_TRACKING_TIMEPASSED                 =( VO_ADSMANAGER_EVENT_TRACKING_BASE | 0x00000005 );        //this event show how much time passed since last same event, the nParam1 is VO_ADSMANAGER_EVENT_TRAKCING_TIMEPASSEDINFO*
    
    public static final int VO_ADSMANAGER_EVENT_ERROR_BASE              =( VO_ADSMANAGER_EVENT_BASE | 0x000e0000 );
    public static final int VO_ADSMANAGER_EVENT_ERROR_GEOBLOCK          =( VO_ADSMANAGER_EVENT_ERROR_BASE | 0x00000001 );         //indicate the PID is GEO blocked
    public static final int VO_ADSMANAGER_EVENT_ERROR_PID_INVALID       =( VO_ADSMANAGER_EVENT_ERROR_BASE | 0x00000002 );         //indicate the PID is invalid, it can not be downloaded
    public static final int VO_ADSMANAGER_EVENT_ERROR_PID_EXPIRED       =( VO_ADSMANAGER_EVENT_ERROR_BASE | 0x00000003 );         //indicate the PID has expired
    public static final int VO_ADSMANAGER_EVENT_ERROR_PID_NOTAVALIBLE   =( VO_ADSMANAGER_EVENT_ERROR_BASE | 0x00000004 );         //indicate the PID is not avalible right now, it will be avalible in the future
    public static final int VO_ADSMANAGER_EVENT_ERROR_SMIL_DOWNLOADFAIL =( VO_ADSMANAGER_EVENT_ERROR_BASE | 0x00000005 );         //indicate the smil download fail
    public static final int VO_ADSMANAGER_EVENT_ERROR_SMIL_PARSEFAIL    =( VO_ADSMANAGER_EVENT_ERROR_BASE | 0x00000006 );         //indicate the smil parse fail
    public static final int VO_ADSMANAGER_EVENT_ERROR_VMAP_AUTHFAIL     =( VO_ADSMANAGER_EVENT_ERROR_BASE | 0x00000007 );         //indicate the vmap authentication fail
    public static final int VO_ADSMANAGER_EVENT_ERROR_VMAP_DOWNLOADFAIL =( VO_ADSMANAGER_EVENT_ERROR_BASE | 0x00000008 );         //indicate the vmap download fail
    public static final int VO_ADSMANAGER_EVENT_ERROR_VMAP_PARSEERROR   =( VO_ADSMANAGER_EVENT_ERROR_BASE | 0x00000009 );         //indicate the vmap parse fail
    
    public static final int  VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE                =( VO_ADSMANAGER_EVENT_TRACKING_BASE | 0x00001000 );
    public static final int  VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_AD_START            =( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x00000001 );      //this event show there is an ad start event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
    public static final int  VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_FIRST_QUARTILE      =( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x00000002 );       //this event show there is a first quartile event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
    public static final int  VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_MID_POINT           =( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x00000003 );       //this event show there is a mid point event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
    public static final int  VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_THIRD_QUARTILE      =( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x00000004 );       //this event show there is a third quartile event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
    public static final int  VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_COMPLETE            =( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x00000005 );       //this event show there is a complete event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
    public static final int  VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_CREATIVE_VIEW       =( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x00000006 );       //this event show there is a creative view event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
    public static final int  VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_MUTE                =( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x00000007 );       //this event show there is a mute event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
    public static final int  VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_UNMUTE              =( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x00000008 );       //this event show there is a unmute event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
    public static final int  VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_PAUSE               =( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x00000009 );       //this event show there is a pause event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
    public static final int  VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_RESUME              =( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x0000000a );       //this event show there is a resume event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
    public static final int  VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_FULLSCREEN          =( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x0000000b );       //this event show there is a fullscreen event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
    public static final int  VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_IMPRESSION          =( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x0000000c );      //this event show there is an impression event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*

    
    public enum VO_OSMP_ADSMANAGER_ACTION
    {
        /**AD Manager action click */
        VO_ADSMANAGER_ACTION_CLICK                 (0X00000000),
        /**AD Manager action playback start */
        VO_ADSMANAGER_ACTION_PLAYBACKSTART         (0X00000001),
        /**AD Manager action playback complete */
        VO_ADSMANAGER_ACTION_PLAYBACKCOMPLETE      (0X00000002),
        /**AD Manager action pause */
        VO_ADSMANAGER_ACTION_PAUSE                 (0X00000003),
        /**AD Manager action seek */
        VO_ADSMANAGER_ACTION_SEEKS                 (0X00000004),
        /**AD Manager action dragstart */
        VO_ADSMANAGER_ACTION_DRAGSTART             (0X00000005),
        /**AD Manager action resume */
        VO_ADSMANAGER_ACTION_RESUME                (0X00000006),
        /**AD Manager action forcestop */
        VO_ADSMANAGER_ACTION_FORCESTOP             (0X00000007),
        /**AD Manager action full screen on */
        VO_ADSMANAGER_ACTION_FULLSCREENON          (0X00000008),
        /**AD Manager action full screen off */
        VO_ADSMANAGER_ACTION_FULLSCREENOFF         (0X00000009),
        /**AD Manager action subtitle on*/
        VO_ADSMANAGER_ACTION_SUBTITLEON            (0X0000000A),
        /**AD Manager action subtitle off */
        VO_ADSMANAGER_ACTION_SUBTITLEOFF           (0X0000000B),
        /**AD Manager action mute on */
        VO_ADSMANAGER_ACTION_MUTEON                (0X0000000C),
        /**AD Manager action mute off */
        VO_ADSMANAGER_ACTION_MUTEOFF               (0X0000000D),
        /**Max value definition */
        VO_OSMP_ADSMANAGER_ACTION_MAX              (0X7FFFFFFF);
        
        private int value;
        
        VO_OSMP_ADSMANAGER_ACTION(int event)
        {
            value = event;
        }
        
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_ADSMANAGER_ACTION valueOf(int value)
        {
            for (int i = 0; i < VO_OSMP_ADSMANAGER_ACTION.values().length; i ++)
            {
                if (VO_OSMP_ADSMANAGER_ACTION.values()[i].getValue() == value)
                    return VO_OSMP_ADSMANAGER_ACTION.values()[i];
            }
            
            return VO_OSMP_ADSMANAGER_ACTION_MAX;
        }
        
    }
    
    private int mNativeContext = 0; // accessed by native methods
    private adsListener mEventListener = null;
    private VOOSMPAdTracking mTracking = null;
    VOOSMPAdInfoImpl mInfo = null;
    private boolean isSendInfo = false;
    private boolean mAdsManagerEnable = false;
    private int mPause = 0;
    private boolean isInit = false;
    private long mCurrPos = 0;
    private int mPauseStatus = 0;
    private int mFullScreen = 0;
    private EventHandler mEventHandler = null;
    
    public interface adsListener
        {
            public VO_OSMP_RETURN_CODE onAdsEvent(int id, int param1, int param2, Object obj);
        }
    
    public void setOnEventListener(adsListener event) {
        // TODO Auto-generated method stub
        mEventListener = event;
    }
    
    private class EventHandler extends Handler
    {
        private VOOSMPADSManager mADSManager = null;
        
        public EventHandler(VOOSMPADSManager obj, Looper looper) 
        {
            super(looper);
            mADSManager = obj;
        }
        
        public void handleMessage(Message msg) 
        {
            if (mADSManager.mNativeContext == 0)
            {
                voLog.w(TAG, "VOOSMPADSManager went away with unhandled events");
                return;
            }
            
            if (mADSManager.mEventListener != null )
            {
                mADSManager.mEventListener.onAdsEvent(msg.what, msg.arg1, msg.arg2, msg.obj);
            }
        }
    }
    
    public VOOSMPADSManager() {
        Looper looper;
        if ((looper = Looper.myLooper()) != null) {
            mEventHandler = new EventHandler(this, looper);
            voLog.v(TAG, "myLooper()" + looper.getThread().getId());
        } else if ((looper = Looper.getMainLooper()) != null) {
            mEventHandler = new EventHandler(this, looper);
            voLog.v(TAG, "getMainLooper()" + looper.getThread().getId());
        } else {
            mEventHandler = null;
        }
        
        voLog.i(TAG, "VOOSMPADSManager construct.");
    }
    
    /**
     * Open the session
     * \param phHandle [out] handle.
     * \param pSource [in] input param, it depends on the nFlag.
     * \param nSize [in] the size of pSource.
     * \param nFlag [in] this flag indicate the type of the pSource.
     * @param type 
     */
    public int Init(String pSource , int nSize , int nFlag , String pPartnerString, String workingPath, String libName, VO_OSMP_LAYOUT_TYPE type)
    {
        File fPathName = new File(workingPath + "libvoOSMPADSManagerJni_OSMP.so");
        
        if (fPathName.exists())
        {
            try {
                System.loadLibrary("vodl");
                System.loadLibrary("voOSMPADSManagerJni_OSMP");
            } catch (UnsatisfiedLinkError e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            int deviceType = type.getValue();
            Integer nativeContext = new Integer(0);
            int nRet = nativeInit(nativeContext, pSource, nSize, nFlag, pPartnerString, workingPath, libName, deviceType);
            
            mNativeContext = nativeContext.intValue();
            return nRet;
        }
        else
        {
            return voOSType.VOOSMP_SRC_ERR_OPEN_SRC_FAIL;
        }
    }
    
    public int Open()
    {
        if(mNativeContext == 0)
        {
            return -1;
        }
        return nativeOpen(mNativeContext);
    }
    
    /**
     * Close the session
     * \param phHandle [in] handle.
     */
    public int Close()
    {
        mEventHandler.removeCallbacksAndMessages(null);
        
        if (mNativeContext == 0)
        {
            return -1;
        }
        
        mInfo = null;
        return nativeClose(mNativeContext);
    }
    
    public int Uninit()
    {
        if(mNativeContext == 0)
        {
            return -1;
        }
        int nRet = nativeUninit(mNativeContext);
        if (voOSType.VOOSMP_ERR_None == nRet) {
            mNativeContext = 0;
        }
        return nRet;
    }
    
    /**
     * Get the playback period info
     * \param phHandle [in] handle.
     * \param ppInfo [out] it will contains all the playback period info, the memory will be maintained in AdsManager side, it will not be destroyed until next GetPlaybackInfo call
     */
    public Object getPlaybackInfo()
    {
        if(mNativeContext == 0)
        {
            return null;
        }
        Object obj = nativeGetPlaybackInfo(mNativeContext);
        if(obj!=null)
        {
            Parcel parc = (Parcel)obj;
            parc.setDataPosition(0);
            mInfo = new VOOSMPAdInfoImpl();
            mInfo.parse(parc);
            parc.recycle();
            obj = mInfo;
        }
        
        return mInfo;
    }
    
    public int getCount()
    {
        if (null == mInfo) {
            return -1;
        }
        
        return mInfo.getCount();
    }
    
    public String getUrl()
    {
        if (null == mInfo) {
            return null;
        }
        
        int count = mInfo.getCount();
        ArrayList<VOOSMPAdPeriod> cList = mInfo.getPeriodList();
        
        if ((null == cList) || (0 >= count)){
            return null;
        }
        
        for (int i = 0; (i < count) && (i < cList.size()); ++i) {
            VOOSMPAdPeriod period = cList.get(i);
            
            if (null != period) {
                if (VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT == period.getPeriodType()) {
                    return period.getPeriodURL();
                }
            }
        }
        
        return null;
    }
    
    public String getSubtitleUrl()
    {
        if (null == mInfo) {
            return null;
        }
        
        int count = mInfo.getCount();
        ArrayList<VOOSMPAdPeriod> cList = mInfo.getPeriodList();
        
        if ((null == cList) || (0 >= count)){
            return null;
        }
        
        for (int i = 0; (i < count) && (i < cList.size()); ++i) {
            VOOSMPAdPeriod period = cList.get(i);
            
            if (null != period) {
                if (VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT == period.getPeriodType()) {
                    return period.getCaptionURL();
                }
            }
        }
        
        return null;
    }
    
    public int getTimeStamp( int timeStamp)
    {
        if(mNativeContext == 0)
        {
            return -1;
        }
        return nativeGetTimeStamp(mNativeContext, timeStamp);
    }
    
    public int setPlayingTime( int currTime)
    {
        if(mNativeContext == 0)
        {
            return -1;
        }
        return nativeSetPlayingTime(mNativeContext, currTime);
    }
    private void adsCallBack(int nID, int param1, int param2, Object arg0)
    {
        if(mInfo != null && mTracking != null && !isSendInfo)
        {
            voLog.v(TAG, "[TRACKING] setPlaybackInfo");
            mTracking.setPlaybackInfo(mInfo);
            isSendInfo = true;
        }
        
        if(mTracking != null)
        {
            VOOSMPTrackingEvent trackingEvent = null;
            switch(nID)
            {
                case VO_ADSMANAGER_EVENT_AD_START:
                {
                	voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_EVENT_AD_START");
                    Parcel parc = (Parcel)arg0;
                    parc.setDataPosition(0);
                    VOOSMPTrackingAction ta = new VOOSMPTrackingAction();
                    ta.parse(parc);
                    parc.recycle();
                    
                    trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKSTART, ta.getPeriodID()
                                                                ,ta.getElapsedTime(), -1 , param2, null);
                    arg0 = ta;
                    mTracking.sendTrackingEvent(trackingEvent);
                    //                  mTracking.sendOpenEvent(ta,param2);
                    param1 = ta.getPeriodID();
                    voLog.v(TAG, "[TRACKING]  ta.mPeriodID = "
                          + ta.getPeriodID() + " ta.mElapsedTime = " + ta.getElapsedTime());
                }
                    break;
                case VO_ADSMANAGER_EVENT_AD_END:
                {
                	voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_EVENT_AD_END");
                    Parcel parc = (Parcel)arg0;
                    parc.setDataPosition(0);
                    VOOSMPTrackingAction ta = new VOOSMPTrackingAction();
                    ta.parse(parc);
                    parc.recycle();
                    
                    trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKCOMPLETE, ta.getPeriodID()
                                                                ,ta.getElapsedTime(), -1 , param2, null);
                    arg0 = ta;
                    mTracking.sendTrackingEvent(trackingEvent);
                    //                  mTracking.sendPlayCompleteEvent(ta,param2);
                    param1 = ta.getPeriodID();
                    voLog.v(TAG, "[TRACKING]  ta.mPeriodID = "
                          + ta.getPeriodID() + " ta.mElapsedTime = " + ta.getElapsedTime());
                }
                    break;
                case VO_ADSMANAGER_EVENT_CONTENT_START:
                {
                	voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_EVENT_CONTENT_START");
                    Parcel parc = (Parcel)arg0;
                    parc.setDataPosition(0);
                    VOOSMPTrackingAction ta = new VOOSMPTrackingAction();
                    ta.parse(parc);
                    parc.recycle();
                    
                    trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKSTART, ta.getPeriodID()
                                                                ,ta.getElapsedTime(), -1 , param2, null);
                    arg0 = ta;
                    mTracking.sendTrackingEvent(trackingEvent);
                    param1 = ta.getPeriodID();
                    //              mTracking.sendOpenEvent(ta,param2);
                    voLog.v(TAG, "[TRACKING]  ta.mPeriodID = "
                          + ta.getPeriodID() + " ta.mElapsedTime = " + ta.getElapsedTime());
                }
                    break;
                case VO_ADSMANAGER_EVENT_CONTENT_END:
                {
                	voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_EVENT_CONTENT_END");
                    Parcel parc = (Parcel)arg0;
                    parc.setDataPosition(0);
                    VOOSMPTrackingAction ta = new VOOSMPTrackingAction();
                    ta.parse(parc);
                    parc.recycle();
                    
                    trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKCOMPLETE, ta.getPeriodID()
                                                                ,ta.getElapsedTime(), -1 , param2, null);
                    arg0 = ta;
                    mTracking.sendTrackingEvent(trackingEvent);
                    //                  mTracking.sendPlayCompleteEvent(ta,param2);
                    param1 = ta.getPeriodID();
                    voLog.v(TAG, "[TRACKING]  ta.mPeriodID = "
                          + ta.getPeriodID() + " ta.mElapsedTime = " + ta.getElapsedTime());
                }
                    break;
           
                case VO_ADSMANAGER_EVENT_WHOLECONTENT_START:
                {
                    voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_EVENT_WHOLECONTENT_START");
                    Parcel parc = (Parcel)arg0;
                    parc.setDataPosition(0);
                    VOOSMPTrackingAction ta = new VOOSMPTrackingAction();
                    ta.parse(parc);
                    parc.recycle();
                    
                    trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_WHOLECONTENT_START, ta.getPeriodID()
                                                                ,ta.getElapsedTime(), -1 , param2, null);
                    arg0 = ta;
                    mTracking.sendTrackingEvent(trackingEvent);
                    //                  mTracking.sendPlayCompleteEvent(ta,param2);
                    param1 = ta.getPeriodID();
                    voLog.v(TAG, "[TRACKING]  ta.mPeriodID = "
                          + ta.getPeriodID() + " ta.mElapsedTime = " + ta.getElapsedTime());
                }
                    break;
                    
                case VO_ADSMANAGER_EVENT_WHOLECONTENT_END:
                {
                    voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_EVENT_WHOLECONTENT_END");
                    Parcel parc = (Parcel)arg0;
                    parc.setDataPosition(0);
                    VOOSMPTrackingAction ta = new VOOSMPTrackingAction();
                    ta.parse(parc);
                    parc.recycle();
                    
                    trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_WHOLECONTENT_END, ta.getPeriodID()
                                                                ,ta.getElapsedTime(), -1 , param2, null);
                    arg0 = ta;
                    mTracking.sendTrackingEvent(trackingEvent);
                    //                  mTracking.sendPlayCompleteEvent(ta,param2);
                    param1 = ta.getPeriodID();
                    voLog.v(TAG, "[TRACKING]  ta.mPeriodID = "
                          + ta.getPeriodID() + " ta.mElapsedTime = " + ta.getElapsedTime());
                }
                    break;
                    
                    
                case VO_ADSMANAGER_EVENT_TRACKING_TIMEPASSED:
                {
                	voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_EVENT_TRACKING_TIMEPASSED");
                    Parcel parc = (Parcel)arg0;
                    parc.setDataPosition(0);
                    VOOSMPTrackingTimePassedInfoImpl ttpi = new VOOSMPTrackingTimePassedInfoImpl();
                    ttpi.parse(parc);
                    parc.recycle();
                    
                    trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_TIMEPASSED, ttpi.getPeriodID()
                                                                , 0, ttpi.getPassedTime(), param2, null);
                    arg0 = ttpi;
                    mTracking.sendTrackingEvent(trackingEvent);
                    voLog.v(TAG, "[TRACKING]  ta.mPeriodID = "
                          + ttpi.getPeriodID() + " ta.mPassedTime = " + ttpi.getPassedTime());
                }
                    break;
                    
                case VO_ADSMANAGER_EVENT_TRACKING_ACTION:
                    if(arg0 != null)
                    {
                        Parcel parc = (Parcel)arg0;
                        parc.setDataPosition(0);
                        VOOSMPTrackingAction ta = new VOOSMPTrackingAction();
                        ta.parse(parc);
                        parc.recycle();
                        voLog.v(TAG, "[TRACKING] ta.mAction = " + ta.getAction() + " ta.mPeriodID = "
                              + ta.getPeriodID() + " ta.mElapsedTime = " + ta.getElapsedTime());
                        
                        VO_OSMP_ADSMANAGER_ACTION action = VO_OSMP_ADSMANAGER_ACTION.valueOf(ta.getAction());
                        arg0 = ta;
                        
                        switch(action)
                        {
                            case VO_ADSMANAGER_ACTION_CLICK:
                            {
                            	voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_ACTION_CLICK");
                                //                      mTracking.sendClickEvent(ta,param2);
                                trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_CLICK, ta.getPeriodID()
                                                                            , ta.getElapsedTime(), -1, param2, null);
                                mTracking.sendTrackingEvent(trackingEvent);
                            }
                                break;
                            case VO_ADSMANAGER_ACTION_PLAYBACKSTART:
                            {
//                                Log.v(TAG, "[TRACKING] VO_ADSMANAGER_ACTION_PLAYBACKSTART");
//                                //                      mTracking.sendOpenEvent(ta,param2);
//                                trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKSTART, ta.getPeriodID()
//                                                                            , ta.getElapsedTime(), -1, param2);
//                                mTracking.sendTrackingEvent(trackingEvent);
                            }
                                break;
                            case VO_ADSMANAGER_ACTION_PLAYBACKCOMPLETE:
//                                Log.v(TAG, "[TRACKING] VO_ADSMANAGER_ACTION_PLAYBACKCOMPLETE");
//                                //                      mTracking.sendPlayCompleteEvent(ta,param2);
//                                trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKCOMPLETE, ta.getPeriodID()
//                                                                            , ta.getElapsedTime(), -1, param2);
//                                mTracking.sendTrackingEvent(trackingEvent);
                                break;
                            case VO_ADSMANAGER_ACTION_PAUSE:
                            	voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_ACTION_PAUSE");
                                //                      mTracking.sendPauseEvent(ta,param2);
                                mPause = 1;
                                trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PAUSE, ta.getPeriodID()
                                                                            , ta.getElapsedTime(), 1, param2, null);
                                mTracking.sendTrackingEvent(trackingEvent);
                                break;
                            case VO_ADSMANAGER_ACTION_SEEKS:
                            	voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_ACTION_SEEKS");
                                //                      mTracking.sendSeekEvent(ta,param2);
                                trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_SEEKS, ta.getPeriodID()
                                                                            , ta.getElapsedTime(), mCurrPos, param2, null);
                                mTracking.sendTrackingEvent(trackingEvent);
                                break;
                            case VO_ADSMANAGER_ACTION_DRAGSTART:
                            	voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_ACTION_DRAGSTART");
                                //                      mTracking.sendDragStartEvent(ta,param2);
                                trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_DRAGSTART, ta.getPeriodID()
                                                                            , ta.getElapsedTime(), -1, param2, null);
                                mTracking.sendTrackingEvent(trackingEvent);
                                break;
                            case VO_ADSMANAGER_ACTION_RESUME:
                            	voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_ACTION_RESUME");
                                //                      mTracking.sendResumeEvent(ta,param2);
                                if(mPause == 1)
                                {
                                	voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_ACTION_PAUSE , continue to play");
                                    mPause = 0;
                                    trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PAUSE, ta.getPeriodID()
                                                                                , ta.getElapsedTime(), 0, param2, null);
                                    mTracking.sendTrackingEvent(trackingEvent);
                                }
//                                trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PAUSE, ta.getPeriodID()
//                                                                            , ta.getElapsedTime(), -1, param2);
//                                mTracking.sendTrackingEvent(trackingEvent);
                                break;
                            case VO_ADSMANAGER_ACTION_FORCESTOP:
                            	voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_ACTION_FORCESTOP");
                                //                      mTracking.sendStopEvent(ta,param2);
                                trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_FORCESTOP, ta.getPeriodID()
                                                                            , ta.getElapsedTime(), -1, param2, null);
                                mTracking.sendTrackingEvent(trackingEvent);
                                break;
                                
                            case VO_ADSMANAGER_ACTION_FULLSCREENON:
                            	voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_ACTION_FULLSCREENON");
                            	trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PLAYER_FULLSCREEN, ta.getPeriodID()
                                        ,0, 1, -1, null);
                            	mTracking.sendTrackingEvent(trackingEvent);
                                break;
                                
                            case VO_ADSMANAGER_ACTION_FULLSCREENOFF:
                            	voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_ACTION_FULLSCREENOFF");
                            	trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PLAYER_FULLSCREEN, ta.getPeriodID()
                                        ,0, 0, -1, null);
                            	mTracking.sendTrackingEvent(trackingEvent);
                                break;
                                
                            case VO_ADSMANAGER_ACTION_SUBTITLEON:
                            	voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_ACTION_SUBTITLEON");
                            	trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PLAYER_CLOSEDCAPTION, ta.getPeriodID()
                                        ,0, 1, -1, null);
                            	mTracking.sendTrackingEvent(trackingEvent);
                                break;
                                
                            case VO_ADSMANAGER_ACTION_SUBTITLEOFF:
                            	voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_ACTION_SUBTITLEOFF");
                            	trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PLAYER_CLOSEDCAPTION, ta.getPeriodID()
                                        ,0, 0, -1, null);
                            	mTracking.sendTrackingEvent(trackingEvent);
                                break;
                                
//                            case VO_ADSMANAGER_ACTION_MUTEON:
//                                voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_ACTION_MUTEON");
//                                trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_VMAP_TRACKING_EVENT_AD_SOUND_OFF, ta.getPeriodID()
//                                        ,0, 1, -1, null);
//                                mTracking.sendTrackingEvent(trackingEvent);
//                                break;
//                                
//                            case VO_ADSMANAGER_ACTION_MUTEOFF:
//                                voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_ACTION_MUTEOFF");
//                                trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_VMAP_TRACKING_EVENT_AD_SOUND_ON, ta.getPeriodID()
//                                        ,0, 0, -1, null);
//                                mTracking.sendTrackingEvent(trackingEvent);
//                                break;
                        }
                        
                    }
                    break;
                    
                case VO_ADSMANAGER_EVENT_TRACKING_PERIODPRECENTAGE:
                    if(arg0 != null)
                    {
                        Parcel parc = (Parcel)arg0;
                        parc.setDataPosition(0);
                        VOOSMPTrackingPercentage tp = new VOOSMPTrackingPercentage();
                        tp.parse(parc);
                        parc.recycle();
                        arg0 = tp;
                        
                        voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_EVENT_TRACKING_PERIODPRECENTAGE tp.mPercentage = " + tp.getPercentage() + " tp.mPeriodID = "
                              + tp.getPeriodID() + " tp.mElapsedTime = " + tp.getElapsedTime());
                        
                        //                  mTracking.sendSegmentEvent(tp,param2);
                        trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PERCENTAGE, tp.getPeriodID()
                                                                    ,tp.getElapsedTime(), tp.getPercentage(), param2, null);
                        mTracking.sendTrackingEvent(trackingEvent);
                    }
                    break;
                    
                case VO_ADSMANAGER_EVENT_TRACKING_WHOLECONTENTPRECENTAGE:
                    if(arg0 != null)
                    {
                        Parcel parc = (Parcel)arg0;
                        parc.setDataPosition(0);
                        VOOSMPTrackingPercentage tp = new VOOSMPTrackingPercentage();
                        tp.parse(parc);
                        parc.recycle();
                        arg0 = tp;
                        
                        voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_EVENT_TRACKING_WHOLECONTENTPRECENTAGE tp.mPercentage = " + tp.getPercentage() + " tp.mPeriodID = "
                              + tp.getPeriodID() + " tp.mElapsedTime = " + tp.getElapsedTime());
                        //                  mTracking.sendSegmentEvent(tp,param2);
                        trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_WHOLECONTENT_PERCENTAGE, tp.getPeriodID()
                                                                    ,tp.getElapsedTime(), tp.getPercentage(), param2, null);
                        mTracking.sendTrackingEvent(trackingEvent);
                    }
                    break;
                    
                case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_AD_START:
                    {
                        Parcel parc = (Parcel)arg0;
                        parc.setDataPosition(0);
                        VOOSMPTrackingEventInfo te = new VOOSMPTrackingEventInfo();
                        te.parse(parc);
                        parc.recycle();
                        arg0 = te;
                        
                        voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_AD_START te.mPid = " + te.getPid() + " te.getPeriodUrlList = "
                              + te.getPeriodUrlList());
                        trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_VMAP_TRACKING_EVENT_AD_START, te.getPid()
                                                                    , 0, -1, -1, te.getPeriodUrlList());
                        mTracking.sendTrackingEvent(trackingEvent);
                    }
                    break;
                    
                case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_FIRST_QUARTILE:
                {
                    Parcel parc = (Parcel)arg0;
                    parc.setDataPosition(0);
                    VOOSMPTrackingEventInfo te = new VOOSMPTrackingEventInfo();
                    te.parse(parc);
                    parc.recycle();
                    arg0 = te;
                    
                    voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_FIRST_QUARTILE te.mPid = " + te.getPid() + " te.getPeriodUrlList = "
                          + te.getPeriodUrlList());
                    trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_VMAP_TRACKING_EVENT_AD_FIRST_QUARTILE, te.getPid()
                                                                , 0, -1, -1, te.getPeriodUrlList());
                    mTracking.sendTrackingEvent(trackingEvent);
                }
                break;
                
                case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_MID_POINT:
                {
                    Parcel parc = (Parcel)arg0;
                    parc.setDataPosition(0);
                    VOOSMPTrackingEventInfo te = new VOOSMPTrackingEventInfo();
                    te.parse(parc);
                    parc.recycle();
                    arg0 = te;
                    
                    voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_MID_POINT te.mPid = " + te.getPid() + " te.getPeriodUrlList = "
                          + te.getPeriodUrlList());
                    trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_VMAP_TRACKING_EVENT_AD_MID_POINT, te.getPid()
                                                                , 0, -1, -1, te.getPeriodUrlList());
                    mTracking.sendTrackingEvent(trackingEvent);
                }
                break;
                
                case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_THIRD_QUARTILE:
                {
                    Parcel parc = (Parcel)arg0;
                    parc.setDataPosition(0);
                    VOOSMPTrackingEventInfo te = new VOOSMPTrackingEventInfo();
                    te.parse(parc);
                    parc.recycle();
                    arg0 = te;
                    
                    voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_THIRD_QUARTILE te.mPid = " + te.getPid() + " te.getPeriodUrlList = "
                          + te.getPeriodUrlList());
                    trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_VMAP_TRACKING_EVENT_AD_THIRDQUARTILE, te.getPid()
                                                                , 0, -1, -1, te.getPeriodUrlList());
                    mTracking.sendTrackingEvent(trackingEvent);
                }
                break;
                
                case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_COMPLETE:
                {
                    Parcel parc = (Parcel)arg0;
                    parc.setDataPosition(0);
                    VOOSMPTrackingEventInfo te = new VOOSMPTrackingEventInfo();
                    te.parse(parc);
                    parc.recycle();
                    arg0 = te;
                    
                    voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_COMPLETE te.mPid = " + te.getPid() + " te.getPeriodUrlList = "
                          + te.getPeriodUrlList());
                    trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_VMAP_TRACKING_EVENT_AD_COMPLETE, te.getPid()
                                                                , 0, -1, -1, te.getPeriodUrlList());
                    mTracking.sendTrackingEvent(trackingEvent);
                }
                break;
                
                case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_CREATIVE_VIEW:
                {
                    Parcel parc = (Parcel)arg0;
                    parc.setDataPosition(0);
                    VOOSMPTrackingEventInfo te = new VOOSMPTrackingEventInfo();
                    te.parse(parc);
                    parc.recycle();
                    arg0 = te;
                    
                    voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_CREATIVE_VIEW te.mPid = " + te.getPid() + " te.getPeriodUrlList = "
                          + te.getPeriodUrlList());
                    trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_VMAP_TRACKING_EVENT_AD_CREATIVE_VIEW, te.getPid()
                                                                , 0, -1, -1, te.getPeriodUrlList());
                    mTracking.sendTrackingEvent(trackingEvent);
                }
                break;
                
                case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_MUTE:
                {
                    Parcel parc = (Parcel)arg0;
                    parc.setDataPosition(0);
                    VOOSMPTrackingEventInfo te = new VOOSMPTrackingEventInfo();
                    te.parse(parc);
                    parc.recycle();
                    arg0 = te;
                    
                    voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_MUTE te.mPid = " + te.getPid() + " te.getPeriodUrlList = "
                          + te.getPeriodUrlList());
                    trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_VMAP_TRACKING_EVENT_AD_MUTE, te.getPid()
                                                                , 0, -1, -1, te.getPeriodUrlList());
                    mTracking.sendTrackingEvent(trackingEvent);
                }
                break;
                
                case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_UNMUTE:
                {
                    Parcel parc = (Parcel)arg0;
                    parc.setDataPosition(0);
                    VOOSMPTrackingEventInfo te = new VOOSMPTrackingEventInfo();
                    te.parse(parc);
                    parc.recycle();
                    arg0 = te;
                    
                    voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_UNMUTE te.mPid = " + te.getPid() + " te.getPeriodUrlList = "
                          + te.getPeriodUrlList());
                    trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_VMAP_TRACKING_EVENT_AD_UNMUTE, te.getPid()
                                                                , 0, -1, -1, te.getPeriodUrlList());
                    mTracking.sendTrackingEvent(trackingEvent);
                }
                break;
                
                case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_PAUSE:
                {
                    Parcel parc = (Parcel)arg0;
                    parc.setDataPosition(0);
                    VOOSMPTrackingEventInfo te = new VOOSMPTrackingEventInfo();
                    te.parse(parc);
                    parc.recycle();
                    arg0 = te;
                    
                    voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_PAUSE te.mPid = " + te.getPid() + " te.getPeriodUrlList = "
                          + te.getPeriodUrlList());
                    trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_VMAP_TRACKING_EVENT_AD_PAUSE, te.getPid()
                                                                , 0, -1, -1, te.getPeriodUrlList());
                    mTracking.sendTrackingEvent(trackingEvent);
                }
                break;
                
                case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_RESUME:
                {
                    Parcel parc = (Parcel)arg0;
                    parc.setDataPosition(0);
                    VOOSMPTrackingEventInfo te = new VOOSMPTrackingEventInfo();
                    te.parse(parc);
                    parc.recycle();
                    arg0 = te;
                    
                    voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_RESUME te.mPid = " + te.getPid() + " te.getPeriodUrlList = "
                          + te.getPeriodUrlList());
                    trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_VMAP_TRACKING_EVENT_AD_RESUME, te.getPid()
                                                                , 0, -1, -1, te.getPeriodUrlList());
                    mTracking.sendTrackingEvent(trackingEvent);
                }
                break;
                
                case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_FULLSCREEN:
                {
                    Parcel parc = (Parcel)arg0;
                    parc.setDataPosition(0);
                    VOOSMPTrackingEventInfo te = new VOOSMPTrackingEventInfo();
                    te.parse(parc);
                    parc.recycle();
                    arg0 = te;
                    
                    voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_FULLSCREEN te.mPid = " + te.getPid() + " te.getPeriodUrlList = "
                          + te.getPeriodUrlList());
                    trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_VMAP_TRACKING_EVENT_AD_FULL_SCREEN, te.getPid()
                                                                , 0, -1, -1, te.getPeriodUrlList());
                    mTracking.sendTrackingEvent(trackingEvent);
                }
                break;
                
                case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_IMPRESSION:
                {
                    Parcel parc = (Parcel)arg0;
                    parc.setDataPosition(0);
                    VOOSMPTrackingEventInfo te = new VOOSMPTrackingEventInfo();
                    te.parse(parc);
                    parc.recycle();
                    arg0 = te;
                    
                    voLog.v(TAG, "[TRACKING] VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_IMPRESSION te.mPid = " + te.getPid() + " te.getPeriodUrlList = "
                          + te.getPeriodUrlList());
                    trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_VMAP_TRACKING_EVENT_AD_IMPRESSION, te.getPid()
                                                                , 0, -1, -1, te.getPeriodUrlList());
                    mTracking.sendTrackingEvent(trackingEvent);
                }
                break;
            }
        }
        
        if (null != mEventHandler) {
            Message m = mEventHandler.obtainMessage(nID, param1, param2, arg0);
            mEventHandler.sendMessage(m);
        }
    }
    
    public int setAction(int action, int currTime)
    {
        if(mNativeContext == 0)
        {
            return -1;
        }
        return nativeSetAction(mNativeContext, action, currTime);
        
    }
    
    public int setTracking(VOOSMPAdTracking tracking)
    {
        mTracking = tracking;
        if(!isInit)
        {
            VOOSMPTrackingEventImpl trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PLAYER_INITIALIZATION, -1
                                                                                ,0, -1, -1, null);
            mTracking.sendTrackingEvent(trackingEvent);
            isInit = true;
        }
        return 0;
    }
    
    public boolean isAdsManagerEnable()
    {
        return mAdsManagerEnable;
    }
    
    public void setAdsManager(boolean isEnable)
    {
        mAdsManagerEnable = isEnable;
    }
    
    public boolean isSeekable(int currTime)
    {
        if(mNativeContext == 0)
        {
            return false;
        }
        return nativeIsSeekable(mNativeContext, currTime);
    }
    
    public Object seek(int currTime, long seekPos)
    {
        if(mNativeContext == 0)
        {
            return null;
        }
        Object obj = nativeSeek(mNativeContext, currTime,seekPos);
        if(obj!=null)
        {
            Parcel parc = (Parcel)obj;
            parc.setDataPosition(0);
            VOOSMPADSeekInfo seekInfo = new VOOSMPADSeekInfo();
            seekInfo.parse(parc);
            parc.recycle();
            obj = seekInfo;
        }
        return obj;
    }
    
    public int getContentDuration()
    {
        if(mNativeContext == 0)
        {
            return -1;
        }
        return nativeGetContentDuration(mNativeContext);
    }
    
    public String setActionSync(int action, int currTime)
    {
        if(mNativeContext == 0)
        {
            return null;
        }
        byte[] b = new byte[1024 *4];
        b = nativeSetActionSync(mNativeContext, action, currTime);
        String str = new String(b);
        return str;
    }
    
    public int setParam(int paramID, Object obj)
    {
        if(mNativeContext == 0)
        {
            return -1;
        }
        return nativeSetParam(mNativeContext, paramID, obj);
    }
    
    public void setDesPos(long pos){
        mCurrPos = pos;
    }
    
    /*
    public void enableFullScreen(boolean isFullScreen){
        if(mTracking != null)
        {
        	voLog.v(TAG, "[TRACKING] VO_OSMP_ADS_TRACKING_EVENT_PLAYER_FULLSCREEN isFullScreen = " + isFullScreen);
            if(isFullScreen)
            {
                VOOSMPTrackingEventImpl trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PLAYER_FULLSCREEN, -1
                                                                                    ,-1, 1, -1);
                mTracking.sendTrackingEvent(trackingEvent);
                
            }
            else
            {
                VOOSMPTrackingEventImpl trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PLAYER_FULLSCREEN, -1
                                                                                    ,-1, 0, -1);
                mTracking.sendTrackingEvent(trackingEvent);
            }
        }
    }
    
    public void enableSubtitle(boolean isShowSubtitle){
        if(mTracking != null)
        {
        	voLog.v(TAG, "[TRACKING] VO_OSMP_ADS_TRACKING_EVENT_PLAYER_CLOSEDCAPTION isShowSubtitle = " + isShowSubtitle);
            if(isShowSubtitle)
            {
                VOOSMPTrackingEventImpl trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PLAYER_CLOSEDCAPTION, -1
                                                                                    ,-1, 1, -1);
                mTracking.sendTrackingEvent(trackingEvent);
            }
            else
            {
                VOOSMPTrackingEventImpl trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PLAYER_CLOSEDCAPTION, -1
                                                                                    ,-1, 0, -1);
                mTracking.sendTrackingEvent(trackingEvent);
            }
        }
    }
    */
    
    public void setClickThrough(String url){
        if(mTracking != null)
        {
        	String[] str = new String[]{url};
            voLog.v(TAG, "[TRACKING] VO_OSMP_ADS_TRACKING_EVENT_CLICK_THROUGH url = " + url);
            VOOSMPTrackingEventImpl trackingEvent = new VOOSMPTrackingEventImpl(VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_CLICK_THROUGH, -1
                    ,0, -1, -1, str);
            mTracking.sendTrackingEvent(trackingEvent);
        }
    }
    
    public native int nativeInit(Integer context, String pSource , int nSize , int nFlag , String pPartnerString, String workingPath, String init, int deviceType);
    public native int nativeOpen(int context);
    public native int nativeClose(int context);
    public native int nativeUninit(int context);
    public native Object nativeGetPlaybackInfo(int context);
    public native int nativeGetTimeStamp(int context, int timeStamp);
    public native int nativeSetPlayingTime(int context, int currTime);
    public native int nativeSetAction(int context, int action,int currTime);
    public native boolean nativeIsSeekable(int context, int currTime);
    public native Object nativeSeek(int context, int currTime, long seekPos);
    public native int nativeGetContentDuration(int context);
    public native byte[] nativeSetActionSync(int context, int action, int currTime);
    public native int nativeSetParam(int context, int paramID, Object obj);
}