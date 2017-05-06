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

import java.util.Timer;
import java.util.TimerTask;

import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.graphics.Rect;
import android.graphics.Typeface;
import android.view.SurfaceView;
import android.view.View;
import android.media.AudioManager;
import android.os.*;

import com.visualon.OSMPBasePlayer.voOSBasePlayer;
import com.visualon.OSMPDataSource.voOSDataSource;
import com.visualon.OSMPUtils.voOSType.VOOSMP_SEI_EVENT_FLAG;
import com.visualon.OSMPUtils.voOSType.VOOSMP_SRC_AUTHENTICATION_REQUEST_TYPE;
import com.visualon.OSMPUtils.voOSType.VOOSMP_SRC_PROGRAM_TYPE;
import com.visualon.OSMPUtils.*;
import com.visualon.OSMPPlayer.*;
import com.visualon.OSMPPlayer.VOCommonPlayerListener.VO_OSMP_AD_STATUS;
import com.visualon.OSMPPlayer.VOCommonPlayerListener.VO_OSMP_CB_EVENT_ID;
import com.visualon.OSMPPlayer.VOCommonPlayerListener.VO_OSMP_CB_SYNC_EVENT_ID;
import com.visualon.OSMPPlayer.VOCommonPlayerListener.VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT;
import com.visualon.OSMPPlayer.VOCommonPlayerListener.VO_OSMP_SRC_CUSTOMERTAGID;
import com.visualon.OSMPPlayer.VOOSMPHTTPDownloadFailure.VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_ADS_TRACKING_EVENT_TYPE;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_LANGUAGE_TYPE;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;
import com.visualon.OSMPPlayer.VOOSMPType.*;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_STATUS;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_SCREEN_BRIGHTNESS_MODE;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_PLAYER_ENGINE;
import com.visualon.OSMPPlayer.VOOSMPSEIPicTiming;
import com.visualon.OSMPPlayer.VOCommonPlayerDeviceInfo.VO_OSMP_DEVICE_INFO_OS_TYPE;
import com.visualon.OSMPPlayer.VOCommonPlayerAssetSelection.VOOSMPAssetIndex;
import com.visualon.OSMPPlayer.VOCommonPlayerAssetSelection.VOOSMPAssetProperty;
import com.visualon.OSMPPlayerImpl.VOOSMPADSManager.VO_OSMP_ADSMANAGER_ACTION;
import com.visualon.OSMPPlayerImpl.VOOSMPADSManager.adsListener;
import com.visualon.OSMPSubTitle.*;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_LAYOUT_TYPE;

public class VOCommonPlayerImpl implements VOCommonPlayer,
        voOSBasePlayer.onEventListener, voOSBasePlayer.onRequestListener {
    private String TAG = "@@@VOCommonPlayerImpl";
    private String IO_FILE_NAME = "voSourceIO";
    private String IO_API_NAME = "voGetSourceIOAPI";
    private String mLibPath = null;

    private voOSBasePlayer mPlayer = null;
    private Context mContext = null;
    private VO_OSMP_PLAYER_ENGINE mPlayerType = null;
    private VOCommonPlayerListener mEventListener = null;

    private Timer timer = null;
    private TimerTask timerTask = null;// new TimerTask()
    private Timer adtimer = null;
    private TimerTask adtimerTask = null;// new TimerTask()
    private boolean subtitleFileNameSet = false;
    private voOSDataSource mSource = null;
    private boolean m_bAssetChanged = false;
    private boolean m_bEnableSubtitle = false;
    private boolean m_bAdHideSubtitle = false;
    private boolean m_bSubtitleShow = false;
	private boolean m_bSuspendPlayAudio = false;
	private boolean m_bSuspendPaused = false;
	private boolean m_bEnableDVRPosition = true;
	private boolean m_bIsSeeking = false;
	private Integer mDrmHandle = null;
    private int mCodeType = VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_VIDEO_SW.getValue() | VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_AUDIO_SW.getValue();
    private VO_OSMP_RENDER_TYPE m_RenderType;
    
    private AudioManager m_amMain;
    private voOSMPDRM mDRMWrap = null;
    private VOOSMPADSManager        mADManager 				= null;
    private EventHandler 			mEventHandler;
    private static final int 		messagePlayingTime		= 0xf0f00001;
    private static final int 		messageCustomerTag		= 0xf0f00002;
    private static final int		messageResume			= 0xf0f01003;
    
    private int 		   mSeekContentPos          = -1;
    private boolean		   mIsPaused                = false;
    private boolean        isPlaybackComplete       = false;
    private class EventHandler extends Handler
    {
        public EventHandler( Looper looper) 
        {
            super(looper);
        }
        
        public void handleMessage(Message msg) 
        {   
        	if(msg.what == messageCustomerTag){
        		mEventListener.onVOEvent(VO_OSMP_CB_EVENT_ID.VO_OSMP_SRC_CB_CUSTOMER_TAG, msg.arg1, msg.arg2, msg.obj);
                voLog.i(TAG,  "messageCustomerTag onVOEvent VO_OSMP_CB_EVENT_ID.VO_OSMP_SRC_CB_CUSTOMER_TAG");
            	return;
            }
	        if(msg.what == messagePlayingTime){
	        	if(mADManager != null && mADManager.isAdsManagerEnable() && !m_bIsSeeking)
	        	{
	        		int nCurr = mPlayer.GetPos();
	        		mEventListener.onVOEvent(VO_OSMP_CB_EVENT_ID.VO_OSMP_AD_CB_VIDEO_PROGRESS, nCurr, 0, null);
	        		int temp = mADManager.setPlayingTime(nCurr);
	        		nCurr = mADManager.getTimeStamp(nCurr);
	        		mPlayer.SetParam(voOSType.VOOSMP_PID_SUBTITLE_TIME_OFFSET, nCurr);
	        	}
	        }
	        
	        if (msg.what == messageResume){
	        	if (m_bSuspendPaused)
	        	{
	        		start();
	        		m_bSuspendPaused = false;
	        	}
	        }
        }
    }
    
    public VOCommonPlayerImpl() {
    	Looper looper;
        if ((looper = Looper.myLooper()) != null) {
            mEventHandler = new EventHandler( looper);
            voLog.v(TAG, "myLooper()" + looper.getThread().getId());
        } else if ((looper = Looper.getMainLooper()) != null) {
            mEventHandler = new EventHandler( looper);
            voLog.v(TAG, "getMainLooper()" + looper.getThread().getId());
        } else {
            mEventHandler = null;
        }
        mPlayer = null;
        mContext = null;
        mPlayerType = null;
        mEventListener = null;
        m_amMain = null;
        m_bEnableSubtitle = false;
        m_bAdHideSubtitle = false;
        m_bSubtitleShow = false;
        voLog.i(TAG, "VOCommonPlayerImpl construct.");
    }

    @Override
    public VO_OSMP_RETURN_CODE init(Context context, String libraryPath,
            VO_OSMP_PLAYER_ENGINE playEngineType, Object initParam,
            int initParamFlag) {
        // TODO Auto-generated method stub
        mContext = context;
        mPlayerType = playEngineType;
        mLibPath = libraryPath;
        m_RenderType = VO_OSMP_RENDER_TYPE.VO_OSMP_RENDER_TYPE_NATIVE_WINDOW;

        if (mPlayer != null) {
            mPlayer.Uninit();
            mPlayer = null;
        }

        mPlayer = new voOSBasePlayer();
        int type = mPlayerType.ordinal();
        int rc = mPlayer.Init(context, libraryPath, null, type, 0, 0);
        mPlayer.setEventListener(this);
        mPlayer.setRequestListener(this);
        m_amMain = (AudioManager) mContext
                .getSystemService(Context.AUDIO_SERVICE);

    	mADManager = new VOOSMPADSManager();
    	
        return VO_OSMP_RETURN_CODE.valueOf(rc);
    }

    @Override
    public VO_OSMP_RETURN_CODE init(VO_OSMP_PLAYER_ENGINE playEngineType,
            VOOSMPInitParam initParam) {
        // TODO Auto-generated method stub
        mContext = (Context) initParam.getContext();
        mPlayerType = playEngineType;
        mLibPath = initParam.getLibraryPath();
        m_RenderType = VO_OSMP_RENDER_TYPE.VO_OSMP_RENDER_TYPE_NATIVE_WINDOW;
        
        if (mPlayer != null) {
            mPlayer.Uninit();
            mPlayer = null;
        }
        mPlayer = new voOSBasePlayer();
        int type = mPlayerType.ordinal();
        int rc = mPlayer.Init((Context) initParam.getContext(),
                initParam.getLibraryPath(), null, type, 0, 0);
        mPlayer.setEventListener(this);
        mPlayer.setRequestListener(this);
        m_amMain = (AudioManager) mContext
                .getSystemService(Context.AUDIO_SERVICE);
        
    	mADManager = new VOOSMPADSManager();
    	
        mPlayer.SetParam(
                voOSType.VOOSMP_PID_LOAD_AUDIO_EFFECT_MODULE,
                1);
        mPlayer.SetParam(
                voOSType.VOOSMP_PID_AUDIO_EFFECT_ENABLE,
                new Integer(0));
    	
        return VO_OSMP_RETURN_CODE.valueOf(rc);

    }

    @Override
    public VO_OSMP_RETURN_CODE destroy() {
        // TODO Auto-generated method stub
    	if(mADManager != null)
        {
    		if(mADManager.isAdsManagerEnable()){
    			mADManager.Uninit();
    		}
        	mADManager = null;
        }
    	
        int rc = (mPlayer == null) ? VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE
                .getValue() : mPlayer.Uninit();
        mPlayer = null;
		if (mDRMWrap != null)
    	{
    		mDRMWrap.destoryDRM(mDrmHandle);
    		mDrmHandle = null;
    		mDRMWrap = null;
    	}
        closeTimer();
        
        m_bEnableSubtitle = false;
        m_bAdHideSubtitle = false;
        m_bSubtitleShow = false;
        
        voLog.i(TAG,
                "VOCommonPlayerImpl destroy, return " + Integer.toHexString(rc));
        return VO_OSMP_RETURN_CODE.valueOf(rc);
    }

    @Override
    public VO_OSMP_RETURN_CODE setView(SurfaceView sv) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        mPlayer.SetView(sv);
        voLog.i(TAG, "VOCommonPlayerImpl SetView.");
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_RETURN_CODE setViewSize(int width, int height) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        mPlayer.SetDisplaySize(width, height);
        voLog.i(TAG, "VOCommonPlayerImpl setViewSize.");
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_RETURN_CODE open(String url, VO_OSMP_SRC_FLAG flag,
            VO_OSMP_SRC_FORMAT type, VOOSMPOpenParam openParam) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
        if (url == null || type == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_PARAMID;
        
        if (openParam != null) {
            mCodeType = openParam.getDecoderType();
            mPlayer.SetParam(
                    voOSType.VOOSMP_SRC_PID_INIT_PARAM_ACTUAL_FILE_SIZE,
                    openParam.getFileSize());
            voLog.i(TAG,
                    "Init Param VOOSMP_SRC_PID_INIT_PARAM_ACTUAL_FILE_SIZE , param id is  %s, param value is  %d",
                    Integer.toHexString(voOSType.VOOSMP_SRC_PID_INIT_PARAM_ACTUAL_FILE_SIZE),
                    openParam.getFileSize());
        }
        if(mPlayerType == VO_OSMP_PLAYER_ENGINE.VO_OSMP_VOME2_PLAYER)
        {
            if(setCodeType(mCodeType) != VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE)
            {
                return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;
            }
            
        }
       
        int fmt = voOSType.VOOSMP_SRC_AUTO_DETECT;

        mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_INIT_PARAM_IO_FILE_NAME,
                IO_FILE_NAME);
        mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_INIT_PARAM_IO_API_NAME,
                IO_API_NAME);

        voLog.i(TAG,
                "VO_OSMP_SOURCE_FORMAT.VO_OSMP_SRC_FFLOCAL_ENC_MP4 , IO_FILE_NAME is %s, IO_API_NAME is  %s.",
                IO_FILE_NAME, IO_API_NAME);

        
        int rc = mPlayer.Open(url,
                voOSType.VOOSMP_FLAG_SOURCE_URL | flag.getValue(), fmt, 0, 0);

        mSource = (voOSDataSource) mPlayer
                .GetParam(voOSType.VOOSMP_SRC_PID_SOURCE_MODULE_OBJECT);

        voLog.i(TAG,
                "VOCommonPlayerImpl open, return " + Integer.toHexString(rc));
    	
        return VO_OSMP_RETURN_CODE.valueOf(rc);

    }

    private VO_OSMP_RETURN_CODE setCodeType(int mCodeType2) {
        // TODO Auto-generated method stub
        
        int videoMask = VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_VIDEO_SW.getValue() |
                VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_VIDEO_IOMX.getValue() |
                VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_VIDEO_MEDIACODEC.getValue();

        int audioMask = VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_AUDIO_SW.getValue() |
                VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_AUDIO_MEDIACODEC.getValue();

        int videoDec = mCodeType2 & videoMask;
        int audioDec = mCodeType2 & audioMask;
        int rendertype = 0;
        if(audioDec == VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_AUDIO_MEDIACODEC.getValue())
        {
            mPlayer.SetParam(voOSType.VOOSMP_PID_MEDIACODEC_AUDIO_DECODER, 1);
           
        }
        
        if(videoDec == VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_VIDEO_MEDIACODEC.getValue())
        {
            return VO_OSMP_RETURN_CODE.valueOf(mPlayer.SetParam(voOSType.VOOSMP_PID_VIDEO_RENDER_TYPE, voOSType.VOOSMP_RENDER_TYPE_JMHW_RENDER));
        }else if(videoDec == VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_VIDEO_IOMX.getValue())
        {
            SharedPreferences pre = this.mContext.getSharedPreferences("IOMX",Context.MODE_PRIVATE);
            if(pre.getString("checkIOMX", "none").equals("false")){
                return  VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
            }
            return VO_OSMP_RETURN_CODE.valueOf(mPlayer.SetParam(voOSType.VOOSMP_PID_VIDEO_RENDER_TYPE, voOSType.VOOSMP_RENDER_TYPE_HW_RENDER));
        }else if(videoDec == VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_VIDEO_SW.getValue())
        {
        	return VO_OSMP_RETURN_CODE.valueOf(mPlayer.SetParam(voOSType.VOOSMP_PID_VIDEO_RENDER_TYPE, m_RenderType.getValue()));
        }
          
        return  VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_RETURN_CODE open(String url, VO_OSMP_SRC_FLAG flag,
            VO_OSMP_SRC_FORMAT format, VOOSMPInitParam initParam,
            int initParamFlag) {
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        if (url == null || flag == null || format == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_PARAMID;

        if ((initParamFlag & VO_OSMP_INIT_PARAM_FLAG.VO_OSMP_FLAG_INIT_ACTUAL_FILE_SIZE
                .getValue()) > 0 && initParam != null) {
            mPlayer.SetParam(
                    voOSType.VOOSMP_SRC_PID_INIT_PARAM_ACTUAL_FILE_SIZE,
                    initParam.getFileSize());
            voLog.i(TAG,
                    "Init Param VOOSMP_SRC_PID_INIT_PARAM_ACTUAL_FILE_SIZE , initParamFlag is %d, param id is  %s, param value is  %d",
                    initParamFlag,
                    Integer.toHexString(voOSType.VOOSMP_SRC_PID_INIT_PARAM_ACTUAL_FILE_SIZE),
                    initParam.getFileSize());
        }

        int fmt;
        if (url.toLowerCase().startsWith("http://")
                || url.toLowerCase().startsWith("https://")) {
            fmt = voOSType.VOOSMP_SRC_AUTO_DETECT;
        } else {
            fmt = voOSType.VOOSMP_SRC_FFMOVIE_MP4;

            mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_INIT_PARAM_IO_FILE_NAME,
                    IO_FILE_NAME);
            mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_INIT_PARAM_IO_API_NAME,
                    IO_API_NAME);

            voLog.i(TAG,
                    "VO_OSMP_SOURCE_FORMAT.VO_OSMP_SRC_FFLOCAL_ENC_MP4 , IO_FILE_NAME is %s, IO_API_NAME is  %s.",
                    IO_FILE_NAME, IO_API_NAME);
        }

        int rc = mPlayer.Open(url,
                voOSType.VOOSMP_FLAG_SOURCE_URL | flag.getValue(), fmt, 0, 0);

        mSource = (voOSDataSource) mPlayer
                .GetParam(voOSType.VOOSMP_SRC_PID_SOURCE_MODULE_OBJECT);

        voLog.i(TAG,
                "VOCommonPlayerImpl open, return " + Integer.toHexString(rc));
        return VO_OSMP_RETURN_CODE.valueOf(rc);
    }

    @Override
    public VO_OSMP_RETURN_CODE close() {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        if(mADManager != null)
        {
        	if(mADManager.isAdsManagerEnable()){
        		mADManager.Close();
    		}
        }
        
        int rc = mPlayer.Close();
        m_bEnableSubtitle = false;
        m_bAdHideSubtitle = false;
        m_bSubtitleShow = false;

        voLog.i(TAG,
                "VOCommonPlayerImpl Close, return " + Integer.toHexString(rc));
        return VO_OSMP_RETURN_CODE.valueOf(rc);

    }

    @Override
    public VO_OSMP_RETURN_CODE start() {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        int rc = mPlayer.Run();
        mEventListener.onVOEvent(VO_OSMP_CB_EVENT_ID.VO_OSMP_AD_CB_STATE_CHANGE,
        		VO_OSMP_AD_STATUS.VO_OSMP_AD_STATUS_PLAYING.getValue(), 0, null);
        
        voLog.i(TAG,
                "VOCommonPlayerImpl Run, return " + Integer.toHexString(rc));
        
        if(mADManager != null && mADManager.isAdsManagerEnable())
    	{
        	if(mIsPaused)
        	{
    		int nCurr = mPlayer.GetPos();
    		mADManager.setAction(VO_OSMP_ADSMANAGER_ACTION.VO_ADSMANAGER_ACTION_RESUME.getValue(),nCurr);
    		mIsPaused = false;
        	}
    	}
        return VO_OSMP_RETURN_CODE.valueOf(rc);
    }

    @Override
    public VO_OSMP_RETURN_CODE pause() {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        int rc = mPlayer.Pause();
        mEventListener.onVOEvent(VO_OSMP_CB_EVENT_ID.VO_OSMP_AD_CB_STATE_CHANGE,
        		VO_OSMP_AD_STATUS.VO_OSMP_AD_STATUS_PAUSED.getValue(), 0, null);
        voLog.i(TAG,
                "VOCommonPlayerImpl Pause, return " + Integer.toHexString(rc));
        
    	if(mADManager != null && mADManager.isAdsManagerEnable())
    	{
    		int nCurr = mPlayer.GetPos();
    		mADManager.setAction(VO_OSMP_ADSMANAGER_ACTION.VO_ADSMANAGER_ACTION_PAUSE.getValue(),nCurr);
    		mIsPaused = true;
    	}
        
        return VO_OSMP_RETURN_CODE.valueOf(rc);
    }

    @Override
    public VO_OSMP_RETURN_CODE stop() {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

    	int nCurr = mPlayer.GetPos();

        m_bIsSeeking = false;
        
        int rc = mPlayer.Stop();
        mEventListener.onVOEvent(VO_OSMP_CB_EVENT_ID.VO_OSMP_AD_CB_STATE_CHANGE,
        		VO_OSMP_AD_STATUS.VO_OSMP_AD_STATUS_STOPPED.getValue(), 0, null);

    	if(mADManager != null && mADManager.isAdsManagerEnable())
    	{
    	    if(!isPlaybackComplete)
    	    {
        	    voLog.i(TAG, "COMPLETE OR CLOSE VO_ADSMANAGER_ACTION_FORCESTOP");
        		mADManager.setAction(VO_OSMP_ADSMANAGER_ACTION.VO_ADSMANAGER_ACTION_FORCESTOP.getValue(),nCurr);
    	    }
    	}
        voLog.i(TAG,
                "VOCommonPlayerImpl stop, return " + Integer.toHexString(rc));
        
        return VO_OSMP_RETURN_CODE.valueOf(rc);
    }
    
    private long setPositionInner(long msec) {
        if (mPlayer == null)
            return -1;
        
        int rc = -1;
        if (m_bIsSeeking) {
            rc = mPlayer.SetPos((int) msec);
        }
        else {
            m_bIsSeeking = true;
            
            rc = mPlayer.SetPos((int) msec);
            if (-1 == rc) {
                voLog.d(TAG, "SetPos fail");
                m_bIsSeeking = false;
            }
        }
        
        return rc;
    }

    @Override
    public long setPosition(long msec) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return -1;

        long rc = 0;
    	if(mADManager != null && mADManager.isAdsManagerEnable())
    	{
    		int nCurr = mPlayer.GetPos();
    		if(mADManager.isSeekable(nCurr))
    		{
    			voLog.d(TAG, "isSeekable true");
    			mADManager.setDesPos(msec);
    			VOOSMPADSeekInfo seekInfo = (VOOSMPADSeekInfo) mADManager.seek(nCurr, msec);
    			if(seekInfo == null)
    			{
    				voLog.e(TAG,"setPosition return seekInfo error!!");
    				return -1;
    			}
    			if(seekInfo.getAdsSequnceNumber() == -1)
    			{
    				rc = setPositionInner(seekInfo.getContentSeekPos());
    			}
    			else
    			{
    				voOSPeriodTime cTime = new voOSPeriodTime();
    				cTime.setPeriodSequenceNumber(seekInfo.getAdsSequnceNumber());
    				rc = mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_PERIOD2TIME, cTime);
    				
    		        if (rc != voOSType.VOOSMP_ERR_None) {
    		        	return rc;
    		        }
    		        
    				mSeekContentPos = (int)seekInfo.getContentSeekPos();
    				rc = setPositionInner(cTime.getTimeStamp());
    			}
    			mADManager.setAction(VO_OSMP_ADSMANAGER_ACTION.VO_ADSMANAGER_ACTION_SEEKS.getValue(),nCurr);
    		}
    		else
    		{
    			voLog.d(TAG, "isSeekable false");
    		}
    		
    		return rc;
    	}
        
        if (isLiveStreaming())
        {
            voOSDVRInfo info = (voOSDVRInfo) mPlayer
                    .GetParam(voOSType.VOOSMP_SRC_PID_DVRINFO);
            return setPositionInner(msec + info.getLiveTime());
        }

        rc = setPositionInner(msec);
        voLog.i(TAG, "VOCommonPlayerImpl setPosition, return new position is "
                + rc);
        
        return rc;
    }

    @Override
    public long getPosition() {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return -1;

        voLog.i(TAG, "VOCommonPlayerImpl getPosition.");
        
    	if (mADManager != null && mADManager.isAdsManagerEnable())
    	{
            int nCurr = mSeekContentPos;
            if (-1 == nCurr) {
                nCurr = mPlayer.GetPos();
            }
            
            nCurr = mADManager.getTimeStamp(nCurr);
            
            return nCurr;
    	}
    	
    	if (m_bEnableDVRPosition && isLiveStreaming()) {
            voOSDVRInfo info = (voOSDVRInfo) mPlayer
                    .GetParam(voOSType.VOOSMP_SRC_PID_DVRINFO);
            if (info != null) {
                return (mPlayer.GetPos() - info.getLiveTime());
            }
        }

        return mPlayer.GetPos();
    }

    @Override
    public long getDuration() {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE.getValue();

        if(mADManager != null && mADManager.isAdsManagerEnable())
        {
        	return mADManager.getContentDuration();
        }
        
        voLog.i(TAG, "VOCommonPlayerImpl getDuration.");
        return mPlayer.GetDuration();
    }

    @Override
    public VO_OSMP_RETURN_CODE setVolume(float volume) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        int rc = mPlayer.SetVolume(volume, volume);

        voLog.i(TAG,
                "VOCommonPlayerImpl SetVolume, return "
                        + Integer.toHexString(rc));
        return VO_OSMP_RETURN_CODE.valueOf(rc);
    }

    @Override
    public VO_OSMP_RETURN_CODE suspend(boolean keepAudio) {
        // TODO Auto-generated method stub
         voLog.i(TAG, "VOCommonPlayerImpl suspend.");
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
		
           m_bSuspendPlayAudio = keepAudio;
        if (!keepAudio){
			Integer param = Integer.valueOf(0);
        	mPlayer.SetParam(voOSType.VOOSMP_PID_APPLICATION_SUSPEND, param);
        	
        	if (getPlayerStatus() == VO_OSMP_STATUS.VO_OSMP_STATUS_PLAYING)
        	{
        		mPlayer.Pause();
        		m_bSuspendPaused = true;
        	}
        }
		else
			return enableVideoStream(false);
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_RETURN_CODE resume(SurfaceView sv) {
        // TODO Auto-generated method stub
        voLog.i(TAG, "VOCommonPlayerImpl resume.");
		if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
		
		VO_OSMP_RETURN_CODE ret = VO_OSMP_RETURN_CODE.valueOf(mPlayer.SetParam(
                voOSType.VOOSMP_PID_APPLICATION_RESUME, sv));
		
		if(m_bSuspendPlayAudio){
			m_bSuspendPlayAudio = false;
			enableVideoStream(true);
		}
		
		if(mADManager != null && mADManager.isAdsManagerEnable())
		{
			int nCurr = mPlayer.GetPos();
			mADManager.setAction(VO_OSMP_ADSMANAGER_ACTION.VO_ADSMANAGER_ACTION_RESUME.getValue(),nCurr);
		}
		
		Message msg = Message.obtain(mEventHandler, messageResume, 0, 0, null);
		mEventHandler.sendMessageDelayed(msg, 200);
		return ret;
		
    }

    @Override
    public VO_OSMP_RETURN_CODE setVideoAspectRatio(VO_OSMP_ASPECT_RATIO ar) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        int nAspect = voOSType.VOOSMP_RATIO_AUTO;
         switch (ar) {
         case VO_OSMP_RATIO_00:
         case VO_OSMP_RATIO_ORIGINAL:
        	//to restore to old aspect ratio
        	nAspect = voOSType.VOOSMP_RATIO_ORIGINAL;
            break;

        case VO_OSMP_RATIO_11:
        	nAspect = voOSType.VOOSMP_RATIO_11;
            break;

        case VO_OSMP_RATIO_43:
        	nAspect = voOSType.VOOSMP_RATIO_43;
            break;

        case VO_OSMP_RATIO_169:
        	nAspect = voOSType.VOOSMP_RATIO_169;
            break;

        case VO_OSMP_RATIO_21:
        	nAspect = voOSType.VOOSMP_RATIO_21;
            break;

        case VO_OSMP_RATIO_2331:
        	nAspect = voOSType.VOOSMP_RATIO_2331;
            break;
            
        case VO_OSMP_RATIO_AUTO:
        	nAspect = voOSType.VOOSMP_RATIO_AUTO;
            break;

        }
        Integer param = new Integer(nAspect);
        mPlayer.SetParam(voOSType.VOOSMP_PID_VIDEO_ASPECT_RATIO, param);
        voLog.i(TAG, "VOCommonPlayerImpl setVideoAspectRatio.", nAspect);
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_STATUS getPlayerStatus() {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return null;
        voLog.i(TAG,
                "VOCommonPlayerImpl getPlayerStatus "
                        + VO_OSMP_STATUS.values()[(Integer) mPlayer
                                .GetParam(voOSType.VOOSMP_PID_STATUS)].name());
        return VO_OSMP_STATUS.values()[(Integer) mPlayer
                .GetParam(voOSType.VOOSMP_PID_STATUS)];
    }

    @Override
    public boolean canBePaused() {
        // TODO Auto-generated method stub
        voLog.i(TAG, "VOCommonPlayerImpl canBePaused ");

        return true;
        // return (mPlayer != null && mPlayer.GetDuration() > 0);
    }

    @Override
    public VO_OSMP_RETURN_CODE setOnEventListener(VOCommonPlayerListener event) {
        // TODO Auto-generated method stub
        mEventListener = event;
        voLog.i(TAG, "VOCommonPlayerImpl setOnEventListener ");
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_RETURN_CODE setLicenseContent(byte[] data) {
        // TODO Auto-generated method stub
        voLog.i(TAG, "VOCommonPlayerImpl setLicenseContent ");
        return (mPlayer == null) ? VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE
                : VO_OSMP_RETURN_CODE.valueOf(mPlayer.SetParam(
                        voOSType.VOOSMP_PID_LICENSE_CONTENT, data));
    }

    @Override
    public VO_OSMP_RETURN_CODE setLicenseFilePath(String path) {
        // TODO Auto-generated method stub
        voLog.i(TAG, "VOCommonPlayerImpl setLicenseFilePath ");
        return (mPlayer == null) ? VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE
                : VO_OSMP_RETURN_CODE.valueOf(mPlayer.SetParam(
                        voOSType.VOOSMP_PID_LICENSE_FILE_PATH, path));

    }

    @Override
    public VO_OSMP_RETURN_CODE setPreAgreedLicense(String str) {
        // TODO Auto-generated method stub
        voLog.i(TAG, "VOCommonPlayerImpl setPreAgreedLicense ");
        return (mPlayer == null) ? VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE
                : VO_OSMP_RETURN_CODE.valueOf(mPlayer.SetParam(
                        voOSType.VOOSMP_PID_LICENSE_TEXT, str));
    }

    @Override
    public VO_OSMP_RETURN_CODE setColorType(VO_OSMP_COLORTYPE type) {
        // TODO Auto-generated method stub
        voLog.i(TAG, "VOCommonPlayerImpl setColorType ");
        return (mPlayer == null) ? VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE
                : VO_OSMP_RETURN_CODE.valueOf(mPlayer.SetParam(
                        voOSType.VOOSMP_PID_DRAW_COLOR, type.getValue()));
    }

    @Override
    public VO_OSMP_RETURN_CODE setRenderType(VO_OSMP_RENDER_TYPE type) {
        // TODO Auto-generated method stub
        voLog.i(TAG, "VOCommonPlayerImpl setRenderType ");
        m_RenderType = type;
        return (mPlayer == null) ? VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE
                : VO_OSMP_RETURN_CODE.valueOf(mPlayer.SetParam(
                        voOSType.VOOSMP_PID_VIDEO_RENDER_TYPE, type.ordinal()));

    }

    @Override
    public VO_OSMP_RETURN_CODE enableDeblock(boolean value) {
        // TODO Auto-generated method stub
        voLog.i(TAG, "VOCommonPlayerImpl enableDeblock ");
        return (mPlayer == null) ? VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE
                : VO_OSMP_RETURN_CODE.valueOf(mPlayer.SetParam(
                        voOSType.VOOSMP_PID_DEBLOCK_ONOFF, (value) ? 1 : 0));

    }

    @Override
    public VO_OSMP_RETURN_CODE setDeviceCapabilityByFile(String filename) {
        // TODO Auto-generated method stub
        voLog.i(TAG, "VOCommonPlayerImpl setDeviceCapabilityByFile ");
        return (mPlayer == null) ? VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE
                : VO_OSMP_RETURN_CODE.valueOf(mPlayer.SetParam(
                        voOSType.VOOSMP_SRC_PID_CAP_TABLE_PATH, filename));
    }

    @Override
    public VO_OSMP_RETURN_CODE setInitialBitrate(int bitrate) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voLog.i(TAG, "VOCommonPlayerImpl setInitialBitrate ");
        voOSPerformanceDataImpl data = new voOSPerformanceDataImpl(5, bitrate,
                0, 0, 0, 0);
        return VO_OSMP_RETURN_CODE.valueOf(mPlayer.SetParam(
                voOSType.VOOSMP_SRC_PID_BA_STARTCAP, data));
    }

    @Override
    public VO_OSMP_RETURN_CODE setSurfaceChangeFinished() {
        // TODO Auto-generated method stub
        voLog.i(TAG, "VOCommonPlayerImpl setSurfaceChangeFinished ");
        return (mPlayer == null) ? VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE
                : VO_OSMP_RETURN_CODE.valueOf(mPlayer.SetParam(
                        voOSType.VOOSMP_PID_SURFACE_CHANGED, 1));
    }

    @Override
    public VO_OSMP_PLAYER_ENGINE getPlayerType() {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return null;
        voLog.i(TAG, "VOCommonPlayerImpl getPlayerType ");
        return VO_OSMP_PLAYER_ENGINE.values()[(Integer) mPlayer
                .GetParam(voOSType.VOOSMP_PID_PLAYER_TYPE)];
    }

    @Override
    public VO_OSMP_RETURN_CODE getParameter(int id, Object obj) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voLog.i(TAG, "VOCommonPlayerImpl getParameter " + id);
        obj = mPlayer.GetParam(id);
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;

    }

    @Override
    public VO_OSMP_RETURN_CODE setParameter(int id, Object obj) {
        // TODO Auto-generated method stub
        voLog.i(TAG, "VOCommonPlayerImpl setParameter " + id);
        return (mPlayer == null) ? VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE
                : VO_OSMP_RETURN_CODE.valueOf(mPlayer.SetParam(id, obj));
    }

    @Override
    public VO_OSMP_RETURN_CODE setZoomMode(VO_OSMP_ZOOM_MODE mode, Rect rect) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voLog.i(TAG, "VOCommonPlayerImpl setVideoDisplayArea. ");
        int nRet;
        nRet = mPlayer.SetParam(voOSType.VOOSMP_PID_ZOOM_MODE, mode.getValue());
        
        if (mode == VO_OSMP_ZOOM_MODE.VO_OSMP_ZOOM_ZOOMIN)
        {
	        voOSRectImpl voRect;
	        if (rect == null)
	        {
	        	return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_PARAMID;
	        }else
	        {
	        	voRect = new voOSRectImpl(rect.left,rect.top,rect.right,rect.bottom);
	        }
	        return VO_OSMP_RETURN_CODE.valueOf(mPlayer.SetParam(
	                voOSType.VOOSMP_PID_DRAW_RECT, voRect));
        }
        
        return VO_OSMP_RETURN_CODE.valueOf(nRet);
    }

    @Override
    public VO_OSMP_RETURN_CODE enableLiveStreamingDVRPosition(boolean enable) {
        m_bEnableDVRPosition = enable;
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public long getMinPosition() {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return -1;

        voLog.i(TAG, "VOCommonPlayerImpl getMinPosition. ");
        
        if (!isLiveStreaming())
        	return 0;

        voOSDVRInfo info = (voOSDVRInfo) mPlayer
                .GetParam(voOSType.VOOSMP_SRC_PID_DVRINFO);
        if (info == null)
            return -1;
        if (m_bEnableDVRPosition && isLiveStreaming())
            return (info.getStartTime() - info.getLiveTime());

        return info.getStartTime();
    }

    @Override
    public long getMaxPosition() {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return -1;
        voLog.i(TAG, "VOCommonPlayerImpl getMaxPosition. ");
        
        if (!isLiveStreaming())
        	return getDuration();

        voOSDVRInfo info = (voOSDVRInfo) mPlayer
                .GetParam(voOSType.VOOSMP_SRC_PID_DVRINFO);
        if (info == null)
            return -1;
        
        voLog.i(TAG,
                "StartTime is %d, EndTime is %d, PlayingTime is %d, LivingTime is %d.",
                info.getStartTime(), info.getEndTime(), info.getPlayingTime(),
                info.getLiveTime());

        if (m_bEnableDVRPosition && isLiveStreaming())
            return 0;

        return info.getEndTime();
    }

    @Override
    public int onEvent(int nID, int nParam1, int nParam2, Object obj) {
        // TODO Auto-generated method stub
        VO_OSMP_CB_EVENT_ID id = VO_OSMP_CB_EVENT_ID.valueOf(nID);
        // voLog.v(TAG, "VOCommonPlayerImpl onEvent. " +
        // Integer.toHexString(id.getValue()) + " , " + id.name());
        voLog.i(TAG, "VOCommonPlayerImpl onEvent. "+Integer.toHexString(nID));
        if (mEventListener == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE.getValue();

        VO_OSMP_RETURN_CODE result = VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
        
        if (nID == voOSType.VOOSMP_CB_MediaTypeChanged) {
            result = mEventListener
                    .onVOEvent(
                            VO_OSMP_CB_EVENT_ID.VO_OSMP_SRC_CB_ADAPTIVE_STREAMING_INFO,
                            VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT.VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_MEDIATYPE_CHANGE
                                    .getValue(), nParam1, obj);
            if(result == null)
                return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT.getValue();
            return result.getValue();
                
        }
        if (nID == voOSType.VOOSMP_SRC_CB_Adaptive_Streaming_Info) {
            if(nParam1 == voOSType.VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DOWNLOADOK)
            {
                voLog.d(TAG,"VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DOWNLOADOK");
                VOOSMPChunkInfo info = null;
                if (obj != null)
                {
                    voOSChunkInfo ci = (voOSChunkInfo)obj;
                    info = new VOOSMPChunkInfoImpl(ci.Type(),ci.RootUrl(),ci.Url(),ci.StartTime(),ci.Duration(),ci.TimeScale());
                }
                result = mEventListener
                        .onVOEvent(
                                VO_OSMP_CB_EVENT_ID.VO_OSMP_SRC_CB_ADAPTIVE_STREAMING_INFO,
                                VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT.VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DOWNLOADOK
                                        .getValue(), nParam1, info);
                if(result == null)
                    return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT.getValue();
                return result.getValue();
            }
            else if(nParam1 == voOSType.VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DROPPED)
            {
                voLog.d(TAG,"VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DROPPED");
                VOOSMPChunkInfo info = null;
                if (obj != null)
                {
                    voOSChunkInfo ci = (voOSChunkInfo)obj;
                    info = new VOOSMPChunkInfoImpl(ci.Type(),ci.RootUrl(),ci.Url(),ci.StartTime(),ci.Duration(),ci.TimeScale());
                }
                result = mEventListener
                        .onVOEvent(
                                VO_OSMP_CB_EVENT_ID.VO_OSMP_SRC_CB_ADAPTIVE_STREAMING_INFO,
                                VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT.VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DROPPED
                                        .getValue(), nParam1, info);
                if(result == null)
                    return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT.getValue();
                return result.getValue();
            }
        }
        if(nID == voOSType.VOOSMP_CB_PCM_OUTPUT){
            VOOSMPPCMBufferImpl param = new VOOSMPPCMBufferImpl(nParam1, nParam2, (byte[]) obj);
            result = mEventListener.onVOEvent(VO_OSMP_CB_EVENT_ID.VO_OSMP_CB_PCM_OUTPUT, 0, 0, param);
            if(result == null)
                return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT.getValue();
            return result.getValue();
            
        }else if (nID == voOSType.VOOSMP_CB_SEI_INFO)
        {
        	if(nParam1 == voOSType.VOOSMP_SEI_EVENT_FLAG.VOOSMP_FLAG_SEI_EVENT_PIC_TIMING.getValue() )
        	{
	        	voLog.v(TAG, "VO_OSMP_CB_SEI_INFO event, SEI type, param1 is %d", nParam1);
        		VOOSMPSEIPicTimingImpl impl = null;
        		if (obj != null)
        		{
        			impl = new VOOSMPSEIPicTimingImpl((voOSSEIPicTiming)obj);
        		}
				result = mEventListener.onVOEvent(VO_OSMP_CB_EVENT_ID.VO_OSMP_CB_SEI_INFO, nParam1, 0, impl);
				if(result == null)
	                return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT.getValue();
	            return result.getValue();
        	}
        	if(nParam1 == voOSType.VOOSMP_SEI_EVENT_FLAG.VOOSMP_FLAG_SEI_USER_DATA_UNREGISTERED.getValue() )
        	{
        		VOOSMPSEIUserDataUnregisteredImpl impl = null;
        		if (obj != null)
        		{
        			impl = new VOOSMPSEIUserDataUnregisteredImpl();
        			impl.setData((voOSSEIUserDataImpl)obj);
        		}
        		if(impl == null)
    	        	voLog.v(TAG, "VO_OSMP_CB_SEI_INFO event, SEI type, param1 is %d", nParam1);
        		else
    	        	voLog.v(TAG, "VO_OSMP_CB_SEI_INFO event, SEI type, param1 = %d,FieldCount = %d, FieldLength = %d", nParam1, impl.getFieldCount(), impl.getFieldLength(0) );
				result = mEventListener.onVOEvent(VO_OSMP_CB_EVENT_ID.VO_OSMP_CB_SEI_INFO, nParam1, 0, impl);
				if(result == null)
	                return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT.getValue();
	            return result.getValue();
        	}
		} else if (nID == voOSType.VOOSMP_CB_SeekComplete) {
		    voLog.v(TAG, "java VOOSMP_CB_SeekComplete, param1 is %d, param2 is :%d vo_ads_manager", nParam1, nParam2);
            m_bIsSeeking = false;
		}

        if (nID == voOSType.VOOSMP_CB_PlayComplete) {
            
            voLog.i(TAG, "ADS wrapper java VOOSMP_CB_PlayComplete");
            
            if (mADManager != null && mADManager.isAdsManagerEnable())
            {
                if (-1 != mSeekContentPos)
                {
                    setPositionInner(mSeekContentPos);
                    mSeekContentPos = -1;
                    
                    mADManager.setParam(VOOSMPADSManager.VO_ADSMANAGER_PID_STARTSEEKCONTENT, 0);
                    
                    return voOSType.VOOSMP_ERR_None;
                }
                
            	//mEventListener.onVOEvent(VO_OSMP_CB_EVENT_ID.VO_OSMP_AD_CB_VIDEO_DONE, nParam1, nParam2, obj);
            	
                int nCurr = mPlayer.GetPos();
        		mADManager.setAction(VO_OSMP_ADSMANAGER_ACTION.VO_ADSMANAGER_ACTION_PLAYBACKCOMPLETE.getValue(),nCurr);
        		isPlaybackComplete = true;
        		//return voOSType.VOOSMP_ERR_None;
            }
//            result = mEventListener.onVOEvent(id, nParam1, nParam2, obj);
//            if(result == null)
//                return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT.getValue();
//            return result.getValue();
        }
        
        result = mEventListener.onVOEvent(id, nParam1, nParam2, obj);
        if(result == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT.getValue();
        return result.getValue();
    }

    @Override
    public VO_OSMP_RETURN_CODE enableAnalytics(int cacheTime) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voLog.i(TAG, "VOCommonPlayerImpl enableAnalytics. ");
        mPlayer.SetParam(voOSType.VOOSMP_PID_VIDEO_PERFORMANCE_CACHE, cacheTime);
        mPlayer.SetParam(voOSType.VOOSMP_PID_VIDEO_PERFORMANCE_ONOFF, 1);

        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_RETURN_CODE startAnalyticsNotification(int interval,
            VOOSMPAnalyticsFilter filter) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voLog.i(TAG, "VOCommonPlayerImpl startAnalyticsNotification. ");
        closeTimer();
        final VOOSMPAnalyticsFilter analyticsFilter = filter;
        timerTask = new TimerTask() {
            public void run() {
                VOOSMPAnalyticsInfo info = getAnalytics(analyticsFilter);

                mEventListener.onVOEvent(
                        VO_OSMP_CB_EVENT_ID.VO_OSMP_CB_ANALYTICS_INFO, 0, 0,
                        info);
            }
        };

        if (timer == null) {
            timer = new Timer();
        }

        timer.schedule(timerTask, interval, interval);

        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_RETURN_CODE stopAnalyticsNotification() {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voLog.i(TAG, "VOCommonPlayerImpl stopAnalyticsNotification. ");
        closeTimer();
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VOOSMPAnalyticsInfo getAnalytics(VOOSMPAnalyticsFilter filter) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return null;

        voLog.i(TAG, "VOCommonPlayerImpl getAnalytics. ");
        int arrInt[] = new int[1];
        voOSVideoPerformanceImpl perf = new voOSVideoPerformanceImpl(
                filter.getLastTime(), 0, 0, 0, 0, 0, filter.getSourceTime(),
                filter.getCodecTime(), filter.getRenderTime(),
                filter.getJitterTime(), 0, arrInt, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0);

        mPlayer.SetParam(voOSType.VOOSMP_PID_VIDEO_PERFORMANCE_OPTION, perf);

        perf = (voOSVideoPerformanceImpl) mPlayer
                .GetParam(voOSType.VOOSMP_PID_VIDEO_PERFORMANCE_OPTION);
        VOOSMPAnalyticsInfoImpl infoImpl = new VOOSMPAnalyticsInfoImpl();
        if (perf == null)
            return null;

        infoImpl.setLastTime(perf.LastTime());
        infoImpl.setSourceDropNum(perf.SourceDropNum());
        infoImpl.setCodecDropNum(perf.CodecDropNum());
        infoImpl.setRenderDropNum(perf.RenderDropNum());
        infoImpl.setDecodedNum(perf.DecodedNum());
        infoImpl.setRenderNum(perf.RenderNum());
        infoImpl.setSourceTimeNum(perf.SourceTimeNum());
        infoImpl.setCodecTimeNum(perf.CodecTimeNum());
        infoImpl.setRenderTimeNum(perf.RenderTimeNum());
        infoImpl.setJitterNum(perf.JitterNum());
        infoImpl.setCodecErrorsNum(perf.CodecErrorsNum());
        infoImpl.setCodecErrors(perf.CodecErrors());
        infoImpl.setCPULoad(perf.CPULoad());
        infoImpl.setFrequency(perf.Frequency());
        infoImpl.setMaxFrequency(perf.MaxFrequency());
        infoImpl.setWorstDecodeTime(perf.WorstDecodeTime());
        infoImpl.setWorstRenderTime(perf.WorstRenderTime());
        infoImpl.setAverageDecodeTime(perf.AverageDecodeTime());
        infoImpl.setAverageRenderTime(perf.AverageRenderTime());
        infoImpl.setTotalCPULoad(perf.TotalCPULoad());

        String tagMsg = String
                .format("VOOSMPAnalyticsInfoImpl LastTime() is %d, SourceDropNum is %d, CodecDropNum is %d, RenderDropNum is %d, DecodedNum() is %d, RenderNum() is %d, SourceTimeNum is %d, CodecTimeNum is %d, RenderTimeNum is %d, JitterNum is %d, CodecErrorsNum is %d, CPULoad is %d, Frequency is %d, MaxFrequency() is %d, WorstDecodeTime is %d, WorstRenderTime is %d, AverageDecodeTime is %d, AverageRenderTime is %d, TotalCPULoad is %d",
                        infoImpl.getLastTime(), infoImpl.getSourceDropNum(),
                        infoImpl.getCodecDropNum(),
                        infoImpl.getRenderDropNum(), infoImpl.getDecodedNum(),
                        infoImpl.getRenderNum(), infoImpl.getSourceTimeNum(),
                        infoImpl.getCodecTimeNum(),
                        infoImpl.getRenderTimeNum(), infoImpl.getJitterNum(),
                        infoImpl.getCodecErrorsNum(), infoImpl.getCPULoad(),
                        infoImpl.getFrequency(), infoImpl.getMaxFrequency(),
                        infoImpl.getWorstDecodeTime(),
                        infoImpl.getWorstRenderTime(),
                        infoImpl.getAverageDecodeTime(),
                        infoImpl.getAverageRenderTime(),
                        infoImpl.getTotalCPULoad());
        voLog.i(TAG, tagMsg);
        return infoImpl;
    }

    @Override
    public VO_OSMP_RETURN_CODE setDRMLibrary(String libName, String libApiName) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voLog.i(TAG, "VOCommonPlayerImpl setDRMLibrary. ");
        int rc = mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_DRM_FILE_NAME,
                libName);
        if (rc != voOSType.VOOSMP_ERR_None)
            return VO_OSMP_RETURN_CODE.valueOf(rc);

        rc = mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_DRM_API_NAME, libApiName);
        return VO_OSMP_RETURN_CODE.valueOf(rc);
    }

    private boolean closeTimer() {
        if (timerTask != null)
            timerTask.cancel();
        timerTask = null;
        if (timer != null)
            timer.cancel();
        timer = null;
        return true;
    }

    @Override
    public int getVideoCount() {
        // TODO Auto-generated method stub
        if (mSource == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE.getValue();

        return mSource.GetVideoCount();
    }

    @Override
    public int getAudioCount() {
        // TODO Auto-generated method stub
        if (mSource == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE.getValue();

        return mSource.GetAudioCount();
    }

    @Override
    public int getSubtitleCount() {
        // TODO Auto-generated method stub
        if (mSource == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE.getValue();

        return mSource.GetSubtitleCount();
    }

    @Override
    public VO_OSMP_RETURN_CODE selectVideo(int index) {
        // TODO Auto-generated method stub
        if (mSource == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        return VO_OSMP_RETURN_CODE.valueOf(mSource.SelectVideo(index));
    }

    @Override
    public VO_OSMP_RETURN_CODE selectAudio(int index) {
        // TODO Auto-generated method stub
        if (mSource == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        m_bAssetChanged = true;
        return VO_OSMP_RETURN_CODE.valueOf(mSource.SelectAudio(index));
    }

    @Override
    public VO_OSMP_RETURN_CODE selectSubtitle(int index) {
        // TODO Auto-generated method stub
        if (mSource == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        m_bAssetChanged = true;
        return VO_OSMP_RETURN_CODE.valueOf(mSource.SelectSubtitle(index));
    }

    @Override
    public boolean isVideoAvailable(int index) {
        // TODO Auto-generated method stub
        if (mSource == null)
            return false;

        return mSource.IsVideoAvailable(index);
    }

    @Override
    public boolean isAudioAvailable(int index) {
        // TODO Auto-generated method stub
        if (mSource == null)
            return false;

        return mSource.IsAudioAvailable(index);
    }

    @Override
    public boolean isSubtitleAvailable(int index) {
        // TODO Auto-generated method stub
        if (mSource == null)
            return false;

        return mSource.IsSubtitleAvailable(index);
    }

    @Override
    public VO_OSMP_RETURN_CODE commitSelection() {
        // TODO Auto-generated method stub
        if (mSource == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        int nRet = mSource.CommitSelection();
        if ((nRet == VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE.getValue())
                && m_bAssetChanged) {
            long cur = getPosition();

            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }

            setPosition(cur);
            voLog.i(TAG, "Commit selection, then seek current position." + cur);
        }
        m_bAssetChanged = false;
        return VO_OSMP_RETURN_CODE.valueOf(nRet);
    }

    @Override
    public VO_OSMP_RETURN_CODE clearSelection() {
        // TODO Auto-generated method stub
        if (mSource == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
        m_bAssetChanged = false;
        return VO_OSMP_RETURN_CODE.valueOf(mSource.ClearSelection());
    }

    @Override
    public VOOSMPAssetProperty getVideoProperty(int index) {
        // TODO Auto-generated method stub
        if (mSource == null)
            return null;

        VOOSMPAssetPropertyImpl impl = new VOOSMPAssetPropertyImpl();
        impl.fillAssetsProperty(mSource,
                voOSType.VOOSMP_SOURCE_STREAMTYPE.VOOSMP_SS_VIDEO.ordinal(),
                index);
        return impl;
    }

    @Override
    public VOOSMPAssetProperty getAudioProperty(int index) {
        // TODO Auto-generated method stub
        if (mSource == null)
            return null;

        VOOSMPAssetPropertyImpl impl = new VOOSMPAssetPropertyImpl();
        impl.fillAssetsProperty(mSource,
                voOSType.VOOSMP_SOURCE_STREAMTYPE.VOOSMP_SS_AUDIO.ordinal(),
                index);
        return impl;
    }

    @Override
    public VOOSMPAssetProperty getSubtitleProperty(int index) {
        // TODO Auto-generated method stub
        if (mSource == null)
            return null;

        VOOSMPAssetPropertyImpl impl = new VOOSMPAssetPropertyImpl();
        impl.fillAssetsProperty(mSource,
                voOSType.VOOSMP_SOURCE_STREAMTYPE.VOOSMP_SS_SUBTITLE.ordinal(),
                index);
        return impl;
    }

    @Override
    public VOOSMPAssetIndex getPlayingAsset() {
        // TODO Auto-generated method stub
        if (mSource == null)
            return null;

        VOOSMPAssetIndexImpl impl = new VOOSMPAssetIndexImpl();
        impl.fillPlayingAssetsIndex(mSource);
        return impl;
    }

    @Override
    public VOOSMPAssetIndex getCurrentSelection() {
        // TODO Auto-generated method stub
        if (mSource == null)
            return null;

        VOOSMPAssetIndexImpl impl = new VOOSMPAssetIndexImpl();
        impl.fillSelectionAssetsIndex(mSource);
        return impl;
    }


    @Override
    public VO_OSMP_RETURN_CODE setSubtitlePath(String filePath) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        subtitleFileNameSet = true;
        mPlayer.SetParam(voOSType.VOOSMP_PID_SUBTITLE_FILE_NAME, filePath);
        return null;
    }

    @Override
    public VO_OSMP_RETURN_CODE enableSubtitle(boolean value) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        if (value) {
            m_bEnableSubtitle = true;
        } else {
            m_bEnableSubtitle = false;
        }
        
        if(mADManager != null && mADManager.isAdsManagerEnable())
        {
//          mADManager.enableSubtitle(m_bSubtitleShow);
            int nCurr = mPlayer.GetPos();
            if(m_bEnableSubtitle)
            {
                mADManager.setAction(VO_OSMP_ADSMANAGER_ACTION.VO_ADSMANAGER_ACTION_SUBTITLEON.getValue(),nCurr);
            }
            else
            {
                mADManager.setAction(VO_OSMP_ADSMANAGER_ACTION.VO_ADSMANAGER_ACTION_SUBTITLEOFF.getValue(),nCurr);
            }
        }
        
        return refreshSubtitle();
    }
    
    public VO_OSMP_RETURN_CODE refreshSubtitle() {
    
    	voLog.i(TAG, "refreshSubtitle" + m_bEnableSubtitle + m_bAdHideSubtitle + m_bSubtitleShow);
    	
    	if (m_bEnableSubtitle && !m_bAdHideSubtitle) {
        	if (!m_bSubtitleShow) {
        		mPlayer.SetParam(voOSType.VOOSMP_PID_COMMON_CCPARSER, 1);
        		m_bSubtitleShow = true;
        	}
        }
    	else {
        	if (m_bSubtitleShow) {
        		mPlayer.SetParam(voOSType.VOOSMP_PID_COMMON_CCPARSER, 0);
        		m_bSubtitleShow = false;
        	}
    	}
    	
    	return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_RETURN_CODE setSubtitleFontColor(int color) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voSubTitleFormatSetting sett = (voSubTitleFormatSetting) mPlayer
                .GetParam(voOSType.VOOSMP_PID_CLOSED_CAPTION_SETTINGS);
        if (sett == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;

        int clr = Color.argb(255, (color & 0x00ff0000) / 256 / 256,
                (color & 0x0000ff00) / 256, (color & 0x000000ff));
        sett.setFontColor(clr);

        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_RETURN_CODE setSubtitleFontOpacity(int alpha) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voSubTitleFormatSetting sett = (voSubTitleFormatSetting) mPlayer
                .GetParam(voOSType.VOOSMP_PID_CLOSED_CAPTION_SETTINGS);
        if (sett == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;

        if (alpha < 0)
            alpha = 0;

        if (alpha > 100)
            alpha = 100;

        sett.setFontOpacity((int) (alpha));

        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_RETURN_CODE setSubtitleFontSizeScale(int scale) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voSubTitleFormatSetting sett = (voSubTitleFormatSetting) mPlayer
                .GetParam(voOSType.VOOSMP_PID_CLOSED_CAPTION_SETTINGS);
        if (sett == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;

        if (scale < 50)
            scale = 50;

        if (scale > 200)
            scale = 200;

        sett.setFontSizeScale(scale);

        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_RETURN_CODE setSubtitleFontBackgroundColor(int color) {
        // TODO Auto-generated method stub

        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voSubTitleFormatSetting sett = (voSubTitleFormatSetting) mPlayer
                .GetParam(voOSType.VOOSMP_PID_CLOSED_CAPTION_SETTINGS);
        if (sett == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;

        int clr = Color.argb(255, (color & 0x00ff0000) / 256 / 256,
                (color & 0x0000ff00) / 256, (color & 0x000000ff));
        sett.setBackgroundColor(clr);

        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;

    }

    @Override
    public VO_OSMP_RETURN_CODE setSubtitleFontBackgroundOpacity(int alpha) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voSubTitleFormatSetting sett = (voSubTitleFormatSetting) mPlayer
                .GetParam(voOSType.VOOSMP_PID_CLOSED_CAPTION_SETTINGS);
        if (sett == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;

        if (alpha < 0)
            alpha = 0;

        if (alpha > 100)
            alpha = 100;

        sett.setBackgroundOpacity((int) (alpha));

        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_RETURN_CODE setSubtitleWindowBackgroundColor(int color) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voSubTitleFormatSetting sett = (voSubTitleFormatSetting) mPlayer
                .GetParam(voOSType.VOOSMP_PID_CLOSED_CAPTION_SETTINGS);
        if (sett == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;

        int clr = Color.argb(255, (color & 0x00ff0000) / 256 / 256,
                (color & 0x0000ff00) / 256, (color & 0x000000ff));
        sett.setWindowColor(clr);

        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;

    }

    @Override
    public VO_OSMP_RETURN_CODE setSubtitleWindowBackgroundOpacity(int alpha) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voSubTitleFormatSetting sett = (voSubTitleFormatSetting) mPlayer
                .GetParam(voOSType.VOOSMP_PID_CLOSED_CAPTION_SETTINGS);
        if (sett == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;

        if (alpha < 0)
            alpha = 0;

        if (alpha > 100)
            alpha = 100;

        sett.setWindowOpacity((int) (alpha));

        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_RETURN_CODE setSubtitleFontItalic(boolean enable) {
        // TODO Auto-generated method stub

        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voSubTitleFormatSetting sett = (voSubTitleFormatSetting) mPlayer
                .GetParam(voOSType.VOOSMP_PID_CLOSED_CAPTION_SETTINGS);
        if (sett == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;

        sett.setFontItalic(enable);
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_RETURN_CODE setSubtitleFontBold(boolean enable) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voSubTitleFormatSetting sett = (voSubTitleFormatSetting) mPlayer
                .GetParam(voOSType.VOOSMP_PID_CLOSED_CAPTION_SETTINGS);
        if (sett == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;

        sett.setFontBold(enable);
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_RETURN_CODE setSubtitleFontUnderline(boolean enable) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voSubTitleFormatSetting sett = (voSubTitleFormatSetting) mPlayer
                .GetParam(voOSType.VOOSMP_PID_CLOSED_CAPTION_SETTINGS);
        if (sett == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;

        sett.setFontUnderline(enable);
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_RETURN_CODE setSubtitleFontName(String name) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voSubTitleFormatSetting sett = (voSubTitleFormatSetting) mPlayer
                .GetParam(voOSType.VOOSMP_PID_CLOSED_CAPTION_SETTINGS);
        if (sett == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;

        sett.setFontName(name);
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_RETURN_CODE setSubtitleFontEdgeType(int type) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voSubTitleFormatSetting sett = (voSubTitleFormatSetting) mPlayer
                .GetParam(voOSType.VOOSMP_PID_CLOSED_CAPTION_SETTINGS);
        if (sett == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;
        sett.setEdgeType(type);

        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_RETURN_CODE setSubtitleFontEdgeColor(int color) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voSubTitleFormatSetting sett = (voSubTitleFormatSetting) mPlayer
                .GetParam(voOSType.VOOSMP_PID_CLOSED_CAPTION_SETTINGS);
        if (sett == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;

        int clr = Color.argb(255, (color & 0x00ff0000) / 256 / 256,
                (color & 0x0000ff00) / 256, (color & 0x000000ff));
        sett.setEdgeColor(clr);

        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_RETURN_CODE setSubtitleFontEdgeOpacity(int alpha) {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voSubTitleFormatSetting sett = (voSubTitleFormatSetting) mPlayer
                .GetParam(voOSType.VOOSMP_PID_CLOSED_CAPTION_SETTINGS);
        if (sett == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;

        if (alpha < 0)
            alpha = 0;

        if (alpha > 100)
            alpha = 100;

        sett.setEdgeOpacity((int) (alpha));

        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_RETURN_CODE resetSubtitleParameter() {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voSubTitleFormatSetting sett = (voSubTitleFormatSetting) mPlayer
                .GetParam(voOSType.VOOSMP_PID_CLOSED_CAPTION_SETTINGS);
        if (sett == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;

        sett.reset();

        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public int getNumberOfCores() {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return -1;

        voOSCPUInfo info = (voOSCPUInfo) mPlayer
                .GetParam(voOSType.VOOSMP_PID_CPU_INFO);
        return info.CoreCount();

    }

    @Override
    public boolean hasNeon() {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return false;

        voOSCPUInfo info = (voOSCPUInfo) mPlayer
                .GetParam(voOSType.VOOSMP_PID_CPU_INFO);
        return (info.CPUType() == 1) ? true : false;
    }

    @Override
    public long getMaxCPUFrequency() {
        // TODO Auto-generated method stub
        if (mPlayer == null)
            return -1;

        voOSCPUInfo info = (voOSCPUInfo) mPlayer
                .GetParam(voOSType.VOOSMP_PID_CPU_INFO);
        return info.Frequency();
    }

    @Override
    public VO_OSMP_DEVICE_INFO_OS_TYPE getOSType() {
        // TODO Auto-generated method stub
        return VO_OSMP_DEVICE_INFO_OS_TYPE.VO_OSMP_DEVICE_INFO_OS_ANDROID;
    }

    @Override
    public String getOSVersion() {
        // TODO Auto-generated method stub
        return Build.VERSION.RELEASE;

    }

    @Override
    public String getDeviceModel() {
        // TODO Auto-generated method stub
        return Build.MODEL;
    }

    @Override
    public VO_OSMP_RETURN_CODE mute() {
        // TODO Auto-generated method stub
        m_amMain.setStreamMute(AudioManager.STREAM_MUSIC, true);
        if (mPlayer == null) {
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
        }
        
        if(mADManager != null && mADManager.isAdsManagerEnable())
        {
            int nCurr = mPlayer.GetPos();
            mADManager.setAction(VO_OSMP_ADSMANAGER_ACTION.VO_ADSMANAGER_ACTION_MUTEON.getValue(),nCurr);
        }
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_RETURN_CODE unmute() {
        // TODO Auto-generated method stub
        m_amMain.setStreamMute(AudioManager.STREAM_MUSIC, false);
        if (mPlayer == null) {
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
        }

        if(mADManager != null && mADManager.isAdsManagerEnable())
        {
            int nCurr = mPlayer.GetPos();
            mADManager.setAction(VO_OSMP_ADSMANAGER_ACTION.VO_ADSMANAGER_ACTION_MUTEOFF.getValue(),nCurr);
        }
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public VO_OSMP_RETURN_CODE redrawVideo() {
        // TODO Auto-generated method stub
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;
    }

    @Override
    public VOOSMPRTSPStatistics getRTSPStatistics() {
        // TODO Auto-generated method stub
        if (mPlayer == null) {
            return null;
        }
        voOSRTSPStatusImpl nRtspStatus = (voOSRTSPStatusImpl)mPlayer
                .GetParam(voOSType.VOOSMP_SRC_PID_RTSP_STATUS);
         

        return new VOOSMPRTSPStatisticsImpl(nRtspStatus);
    }

    @Override
    public VO_OSMP_RETURN_CODE setHTTPVerificationInfo(
            VOOSMPVerificationInfo info) {
        // TODO Auto-generated method stub
        if (mPlayer == null) {
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
        }

        voOSVerificationInfo info1 = new voOSVerificationInfo(0,
                info.getVerificationString(), info.getDataFlag(), null);
        int nRet = mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_DOHTTPVERIFICATION,
                info1);
        return VO_OSMP_RETURN_CODE.valueOf(nRet);
    }

    @Override
    public VO_OSMP_RETURN_CODE setRTSPConnectionType(
            VO_OSMP_RTSP_CONNECTION_TYPE type) {
        // TODO Auto-generated method stub
        if (mPlayer == null) {
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
        }

        int value = 0;

        switch (type) {
        case VO_OSMP_RTSP_CONNECTION_AUTOMATIC:
            value = 0;
            break;
        case VO_OSMP_RTSP_CONNECTION_TCP:
            value = 1;
            break;
        case VO_OSMP_RTSP_CONNECTION_UDP:
            value = -1;
            break;
        default:
            value = 0;
            break;
        }

        int nRet = mPlayer.SetParam(
                voOSType.VOOSMP_SRC_PID_SOCKET_CONNECTION_TYPE, value);
        return VO_OSMP_RETURN_CODE.valueOf(nRet);
    }

    @Override
    public VO_OSMP_RETURN_CODE setDRMVerificationInfo(
            VOOSMPVerificationInfo info) {
        // TODO Auto-generated method stub
        if (mPlayer == null) {
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
        }

        voOSVerificationInfo info1 = new voOSVerificationInfo(0,
                info.getVerificationString(), info.getDataFlag(), null);
        int nRet = mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_DODRMVERIFICATION,
                info1);
        return VO_OSMP_RETURN_CODE.valueOf(nRet);
    }

    @Override
    public VO_OSMP_RETURN_CODE setDRMUniqueIndentifier(String string) {
        // TODO Auto-generated method stub
        return setDRMUniqueIdentifier(string);
    }

    @Override
    public VO_OSMP_RETURN_CODE setDRMUniqueIdentifier(String string) {
        // TODO Auto-generated method stub
        if (mPlayer == null) {
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
        }

        int nRet = mPlayer.SetParam(
                voOSType.VOOSMP_SRC_PID_DRM_UNIQUE_IDENTIFIER, string);
		nRet = mPlayer.SetParam(
                voOSType.VOOSMP_PID_DRM_UNIQUE_IDENTIFIER, string);
        return VO_OSMP_RETURN_CODE.valueOf(nRet);
    }

    @Override
    public VO_OSMP_RETURN_CODE setDRMInitData(VOOSMPDRMInit initData) {
        // TODO Auto-generated method stub
        if (mPlayer == null) 
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        voOSDRMInit param = new voOSDRMInit(initData.getDRMInitDataHandle(), 
                                            initData.getDRMInitData());

        int nRet = mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_DRM_INIT_DATA_RESPONSE, param);
        return VO_OSMP_RETURN_CODE.valueOf(nRet);
    }


    @Override
    public VO_OSMP_RETURN_CODE setInitialBufferTime(int time) {
        // TODO Auto-generated method stub
        if (mPlayer == null) {
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
        }

        int nRet = mPlayer.SetParam(
                voOSType.VOOSMP_SRC_PID_BUFFER_START_BUFFERING_TIME, time);
        return VO_OSMP_RETURN_CODE.valueOf(nRet);
    }

    @Override
    public VO_OSMP_RETURN_CODE enableAudioStream(boolean value) {
        // TODO Auto-generated method stub
        if (mPlayer == null) {
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
        }

        int nRet = mPlayer.SetParam(voOSType.VOOSMP_PID_AUDIO_STREAM_ONOFF,
                value ? 1 : 0);
        return VO_OSMP_RETURN_CODE.valueOf(nRet);
    }

    @Override
    public VO_OSMP_RETURN_CODE enableVideoStream(boolean value) {
        // TODO Auto-generated method stub
        if (mPlayer == null) {
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
        }

        int nRet = mPlayer.SetParam(voOSType.VOOSMP_PID_VIDEO_STREAM_ONOFF,
                value ? 1 : 0);
        return VO_OSMP_RETURN_CODE.valueOf(nRet);
    }

    @Override
    public VO_OSMP_RETURN_CODE enableLowLatencyVideo(boolean value) {
        // TODO Auto-generated method stub
        if (mPlayer == null) {
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
        }

        int nRet = mPlayer.SetParam(voOSType.VOOSMP_PID_LOW_LATENCY,
                value ? 1 : 0);
        mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_LOW_LATENCY_MODE,
                value ? 1 : 0);
        return VO_OSMP_RETURN_CODE.valueOf(nRet);
    }

    @Override
    public VO_OSMP_RETURN_CODE enableAudioEffect(boolean value) {
        // TODO Auto-generated method stub
        if (mPlayer == null) {
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
        }
        voLog.i(TAG,
                "enableAudioEffect , VOOSMP_PID_AUDIO_EFFECT_ENABLE  "+value );
        int nRet = mPlayer.SetParam(voOSType.VOOSMP_PID_AUDIO_EFFECT_ENABLE,
                new Integer(value ? 1 : 0));
        return VO_OSMP_RETURN_CODE.valueOf(nRet);
    }

    @Override
    public int onRequest(int nID, int nParam1, int nParam2, Object obj) {
        // TODO Auto-generated method stub
        // Log.i(TAG, "VOCommonPlayerImpl onEvent. " +
        // Integer.toHexString(id.getValue()) + " , " + id.name());

        if (mEventListener == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE.getValue();

        if(nID == voOSType.VOOSMP_SRC_CB_Customer_Tag){
        	voOSTimedTag info = (voOSTimedTag)obj;
        	if( info!=null)
        	{
        		Message msg = Message.obtain(mEventHandler, messageCustomerTag, VO_OSMP_SRC_CUSTOMERTAGID.VO_OSMP_SRC_CUSTOMERTAGID_TIMEDTAG.getValue(), (int) info.TimeStamp(), info.Data());
        		mEventHandler.sendMessage(msg);
        	}
        	return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE.getValue();
        }
        
        // VO_SOURCE2_CALLBACKIDBASE_DRM
        if(nID == voOSType.VOOSMP_SRC_CB_Authentication_Request){
	        if (nParam1 == (VOOSMP_SRC_AUTHENTICATION_REQUEST_TYPE.VOOSMP_SRC_AUTHENTICATION_DRM_REQUEST_SERVER_INFO.getValue())) 
	        {
                voLog.i(TAG,
                    "onRequest arg0 is %s, VOOSMP_SRC_PID_DODRMVERIFICATION",
                    Integer.toHexString(nID));
                VO_OSMP_RETURN_CODE result = mEventListener.onVOSyncEvent(
                        VO_OSMP_CB_SYNC_EVENT_ID.VO_OSMP_SRC_CB_SYNC_AUTHENTICATION_DRM_SERVER_INFO,
                        nParam1, nParam2, obj);
                if(result == null)
                    return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT.getValue();
                return result.getValue();
            }
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_PARAMID.getValue(); 
        }
        
        if(nID == voOSType.VOOSMP_SRC_CB_IO_HTTP_Start_Download){
            VO_OSMP_RETURN_CODE result = mEventListener.onVOSyncEvent(
                    VO_OSMP_CB_SYNC_EVENT_ID.VO_OSMP_SRC_CB_SYNC_IO_HTTP_START_DOWNLOAD,
                   nParam1, nParam2, obj);
            if(result == null)
                return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT.getValue();
            return result.getValue();
        }

        if(nID == voOSType.VOOSMP_SRC_CB_IO_HTTP_Download_Failed)
        {
        	if(obj instanceof voOSIOFailedDescImpl)
        	{
        		voOSIOFailedDescImpl object = (voOSIOFailedDescImpl)obj;
        		int n1 = object.getReason().getValue();
        		//n1++;
        		VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON rs = VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON.valueOf(n1);
        		if(n1 == 2)
        			rs = VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON.VO_OSMP_HTTP_CLIENT_ERROR;
        		VOOSMPHTTPDownloadFailureImpl htp = new VOOSMPHTTPDownloadFailureImpl(rs,object.getResponse(),object.getURL());
        		VO_OSMP_RETURN_CODE result = mEventListener.onVOSyncEvent(
        				VO_OSMP_CB_SYNC_EVENT_ID.VO_OSMP_SRC_CB_SYNC_IO_HTTP_DOWNLOAD_FAIL,
                     nParam1, nParam2, htp);
        		if(result == null)
        			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT.getValue();
        		return result.getValue();
        	}
        	return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_PARAMID.getValue();
        }

        if(nID == voOSType.VOOSMP_SRC_CB_DRM_Init_Data){
            voOSDRMInit v2DrmInit = (voOSDRMInit) obj;
            VOOSMPDRMInit drmInit = new VOOSMPDRMInit(v2DrmInit.getDRMInitDataHandle(), v2DrmInit.getDRMInitData());
            VO_OSMP_RETURN_CODE result = mEventListener.onVOSyncEvent(
                    VO_OSMP_CB_SYNC_EVENT_ID.VO_OSMP_SRC_CB_DRM_INIT_DATA,
                    nParam1, nParam2, drmInit);
            if(result == null)
                return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT.getValue();
            return result.getValue();
        }

		if(nID == voOSType.VOOSMP_SRC_CB_Adaptive_Streaming_Info){
            
	        if (nParam1 == voOSType.VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_DISCONTINUE_SAMPLE) 
	        {
	        	if(mADManager != null && mADManager.isAdsManagerEnable())
	        	{
	                voLog.i(TAG,
	                    "[ADS] VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_DISCONTINUE_SAMPLE",
	                    Integer.toHexString(nID));
	                voOSChunkSample cs = (voOSChunkSample)obj;
	                
	                //inner return code should pass without transform
	                return mADManager.setParam(VOOSMPADSManager.VO_ADSMANAGER_PID_DISCONTINUESAMPLE, cs);
	        	}
	        }
			else if (nParam1 == voOSType.VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_BEGINDOWNLOAD) 
	        {
				if(mADManager != null && mADManager.isAdsManagerEnable())
				{
	                voLog.i(TAG,
	                    "[ADS] VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_BEGINDOWNLOAD",
	                    Integer.toHexString(nID));
	                voOSChunkInfo ci = (voOSChunkInfo)obj;
	                
	                //inner return code should pass without transform
	                return mADManager.setParam(VOOSMPADSManager.VO_ADSMANAGER_PID_DISCONTINUECHUNK, ci);
				}
	        }

            return voOSType.VOOSMP_ERR_None;
        }
	
		
		if (VO_OSMP_CB_SYNC_EVENT_ID.valueOf(nID) != VO_OSMP_CB_SYNC_EVENT_ID.VO_OSMP_CB_SYNC_EVENT_ID_MAX)
		{
			 VO_OSMP_RETURN_CODE result = mEventListener.onVOSyncEvent(
					 VO_OSMP_CB_SYNC_EVENT_ID.valueOf(nID),nParam1, nParam2, obj);
	         if(result == null)
	             return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT.getValue();
	         return result.getValue();
		}
		
        return 0;

    }

    @Override
    public VO_OSMP_RETURN_CODE setScreenBrightnessMode(
            VO_OSMP_SCREEN_BRIGHTNESS_MODE mode) {
        // TODO Auto-generated method stub
        if (mPlayer == null) {
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
        }

        return VO_OSMP_RETURN_CODE.valueOf(mPlayer.SetScreenBrightnessMode(mode
                .getValue()));
    }

    @Override
    public VO_OSMP_SCREEN_BRIGHTNESS_MODE getScreenBrightnessMode() {
        // TODO Auto-generated method stub
        if (mPlayer == null) {
            return VO_OSMP_SCREEN_BRIGHTNESS_MODE.VO_OSMP_SCREEN_BRIGHTNESS_MODE_MAX;
        }

        return VO_OSMP_SCREEN_BRIGHTNESS_MODE.valueOf(mPlayer
                .GetScreenBrightnessMode());
    }

    @Override
    public VO_OSMP_RETURN_CODE setScreenBrightness(int brightness) {
        // TODO Auto-generated method stub
        if (mPlayer == null) {
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
        }
        return VO_OSMP_RETURN_CODE.valueOf(mPlayer
                .SetScreenBrightness(brightness));
    }

    @Override
    public int getScreenBrightness() {
        // TODO Auto-generated method stub
        if (mPlayer == null) {
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE.getValue();
        }

        return mPlayer.GetScreenBrightness();
    }

    @Override
    public boolean isLiveStreaming() {
        // TODO Auto-generated method stub
        voLog.i(TAG, "VOCommonPlayerImpl isLive.");
        if (null == mPlayer)
            return false;
        voLog.i(TAG, "VOCommonPlayerImpl isLive.");
        voOSProgramInfo programInfo = (voOSProgramInfo) mPlayer
                .GetProgramInfo(0);
        if(null == programInfo)
            return false;

        return (programInfo.getProgramType() == VOOSMP_SRC_PROGRAM_TYPE.VOOSMP_SRC_PROGRAM_TYPE_LIVE);

    }

    @Override
    public VO_OSMP_RETURN_CODE setRTSPConnectionPort(VOOSMPRTSPPort portNum) {
        // TODO Auto-generated method stub

        if (mPlayer == null) {
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
        }

        int nAudioPort = portNum.getAudioConnectionPort();
        int nVideoPort = portNum.getVideoConnectionPort();

        int MIN = 0X00000400;
        int MAX = 0x0000FFFF;

        if (nAudioPort > MAX || nAudioPort < MIN)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;
        if (nVideoPort > MAX || nVideoPort < MIN)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;

        int nRet = mPlayer.SetParam(
                voOSType.VOOSMP_SRC_PID_RTSP_CONNECTION_PORT,
                new voOSConnectPortInfoImpl(nAudioPort, nVideoPort));
        return VO_OSMP_RETURN_CODE.valueOf(nRet);
    }

    @Override
    public VO_OSMP_RETURN_CODE setHTTPHeader(String headerName,
            String headerValue) {
        
        if (mPlayer == null) {
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
        }
        
        if(null == headerName || null == headerValue)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNKNOWN;
        
        voOSHTTPHeader httpHead = new voOSHTTPHeader(headerName,headerValue,0,null);
        return VO_OSMP_RETURN_CODE.valueOf(mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_HTTP_HEADER, httpHead));
        
        
    }

    @Override
    public VO_OSMP_RETURN_CODE setHTTPProxy(VOOSMPHTTPProxy proxy) {
        
        if (mPlayer == null) {
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
        }
            
        voOSHTTPProxy nProxy = new voOSHTTPProxy(proxy.getProxyHost(),proxy.getProxyPort(),0,null);
        // TODO Auto-generated method stub
        return VO_OSMP_RETURN_CODE.valueOf(mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_HTTP_PROXY_INFO, nProxy));
    }

    @Override
    public VO_OSMP_RETURN_CODE setMaxBufferTime(int time) {
        // TODO Auto-generated method stub
        if (mPlayer == null) {
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
        }
        return VO_OSMP_RETURN_CODE.valueOf( mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_BUFFER_MAX_SIZE, time));
   
    }

    @Override
    public VO_OSMP_RETURN_CODE setAnewBufferingTime(int time) {
        // TODO Auto-generated method stub
        if (mPlayer == null) {
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
        }
        return VO_OSMP_RETURN_CODE.valueOf( mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_BUFFER_BUFFERING_TIME, time));
   
    }
    
    @Override
    public String getDRMUniqueIndentifier() {
        // TODO Auto-generated method stub
        return getDRMUniqueIdentifier();
    }

    @Override
    public String getDRMUniqueIdentifier() {
        // TODO Auto-generated method stub
        if (mPlayer == null) {
            return null;
        }
        return (String)mPlayer.GetParam(voOSType.VOOSMP_SRC_PID_DRM_UNIQUE_IDENTIFIER);
    }


    @Override
    public VO_OSMP_RETURN_CODE enablePCMOutput(boolean value) {
        // TODO Auto-generated method stub
        if (mPlayer == null) {
            return null;
        }
        return VO_OSMP_RETURN_CODE.valueOf(mPlayer.SetParam(voOSType.VOOSMP_PID_AUDIO_PCM_OUTPUT, value ? 1 : 0));
    }

    @Override
    public VO_OSMP_RETURN_CODE setDRMAdapter(Object adapter, boolean isLibraryName) {
        // TODO Auto-generated method stub
        if (mPlayer == null) {
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
        }
        

        if(isLibraryName)
        {
            if(mDRMWrap == null)
            {
            	voLog.i(TAG, "Initlize DRM Wrap setDRMAdapter ");
            	mDRMWrap = new voOSMPDRM();
            }else
            {
            	voLog.e(TAG, "ERROR! Call setDRMAdapter twice.");
            	return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNKNOWN;
            }

            mDrmHandle = new Integer(0);
            mDRMWrap.creatDRM(mDrmHandle, mLibPath, (String)adapter);
            
            return VO_OSMP_RETURN_CODE.valueOf(mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_DRM_ADAPTER_OBJECT, mDrmHandle));
        }
        return VO_OSMP_RETURN_CODE.valueOf(mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_DRM_ADAPTER_OBJECT, adapter));
    }

   @Override
    public VO_OSMP_RETURN_CODE setAudioPlaybackSpeed(float speed) {
        // TODO Auto-generated method stub
        if (mPlayer == null) {
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
        }
        return VO_OSMP_RETURN_CODE.valueOf( mPlayer.SetParam(voOSType.VOOSMP_PID_AUDIO_PLAYBACK_SPEED, new Float(speed)));
    }

@Override
public String getVersion(VO_OSMP_MODULE_TYPE module) {
    
    if (mPlayer == null || module == null ) 
        return null;
        int pid = (module.getValue() << 16) ^ voOSType.VOOSMP_PID_MODULE_VERSION;
        voOSModuleVersionImpl mv = (voOSModuleVersionImpl)mPlayer.GetParam(pid);
        return mv.GetVersion();
        
    }

@Override
public VO_OSMP_RETURN_CODE setPresentationDelay(int time) {
    
    if (mPlayer == null) {
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
    }
    return VO_OSMP_RETURN_CODE.valueOf( mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_PRESENTATION_DELAY, new Integer(time)));
    // TODO Auto-generated method stub
    }

@Override
public VO_OSMP_RETURN_CODE loadVideoById(String pID, String parentID, int startTime, int flag, VO_OSMP_LAYOUT_TYPE type) {
	
	if (mPlayer == null || mADManager == null) {
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
    }
	stop();
	mADManager.Close();
	mADManager.Uninit();
	mADManager.setAdsManager(true);
	mADManager.setOnEventListener(mAdsEventListener);
	isPlaybackComplete = false;
	int nSize = pID.length();
	int nFlag = VOOSMPADSManager.VO_ADSMANAGER_OPENFLAG_PID | VOOSMPADSManager.VO_ADSMANAGER_OPENFLAG_ASYNCOPEN | flag;
	int nRet = 0;
	//String workPath = mLibPath + "libvoAdsManager_OSMP.so";
	nRet = mADManager.Init( pID, nSize, nFlag, "", mLibPath, "libvoAdsManager_OSMP.so",type);
    if (voOSType.VOOSMP_ERR_None != nRet)
    {
        voLog.e(TAG,"loadVideoById open error!!");
        mADManager.setAdsManager(false);
        return VO_OSMP_RETURN_CODE.VO_OSMP_SRC_ERR_OPEN_SRC_FAIL;
    }

	nRet = mADManager.Open();
	if (voOSType.VOOSMP_ERR_None != nRet)
	{
		voLog.e(TAG,"loadVideoById open error!!");
		mADManager.setAdsManager(false);
		return VO_OSMP_RETURN_CODE.VO_OSMP_SRC_ERR_OPEN_SRC_FAIL;
	}
	
	return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
}

private VO_OSMP_RETURN_CODE onLoadVideoComplete() {
	if (mPlayer == null || mADManager == null) {
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
    }
    Object playbackInfo = mADManager.getPlaybackInfo();
    if(playbackInfo == null)
    {
        voLog.e(TAG,"loadVideoById getPlayBackInfo error!!");
        mADManager.setAdsManager(false);
        return VO_OSMP_RETURN_CODE.VO_OSMP_SRC_ERR_OPEN_SRC_FAIL;
    }
    mEventListener.onVOEvent(VO_OSMP_CB_EVENT_ID.VO_OSMP_AD_CB_PLAYBACKINFO, 0, 0, playbackInfo);
    
    String videoPath = mADManager.getUrl();
    if(videoPath ==null)
    {
        voLog.e(TAG,"loadVideoById get videoPath error!!");
        mADManager.setAdsManager(false);
        return VO_OSMP_RETURN_CODE.VO_OSMP_SRC_ERR_OPEN_SRC_FAIL;
    }
    String subtitlePath = mADManager.getSubtitleUrl();

    voLog.d(TAG,"AD Manager subtitlePath = " + subtitlePath);
    voLog.d(TAG,"AD Manager videoPath = " + videoPath);
    if(subtitlePath != null && subtitlePath.length() > 0)
    {
        setSubtitlePath(subtitlePath);
    }
    VOOSMPOpenParam openParam = new VOOSMPOpenParam();
    openParam.setDecoderType(VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_VIDEO_SW.getValue() | VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_AUDIO_SW.getValue());
    open(videoPath, VO_OSMP_SRC_FLAG.VO_OSMP_FLAG_SRC_OPEN_ASYNC, VO_OSMP_SRC_FORMAT.VO_OSMP_SRC_AUTO_DETECT, openParam);
    adstartTimer(); 
    int nCurr = mPlayer.GetPos();
    mADManager.setAction(VO_OSMP_ADSMANAGER_ACTION.VO_ADSMANAGER_ACTION_PLAYBACKSTART.getValue(),nCurr);
    
    return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
}

private boolean adcloseTimer(){
	if(adtimerTask!=null)
		adtimerTask.cancel();
	adtimerTask = null;
	if(adtimer!=null)
		adtimer.cancel();
	adtimer = null;
	return true;
	}

private boolean adstartTimer() {
	adcloseTimer();
	adtimerTask= new TimerTask()
	{
		public void run()
		{
			mEventHandler.sendEmptyMessage(messagePlayingTime);
		}
	};
	
	if(adtimer == null)
	{
		adtimer = new Timer();
	}
	
	adtimer.schedule(adtimerTask, 100, 100);
	
	voLog.v(TAG, "adsmanager");
	return true;
	
	}

@Override
public VO_OSMP_RETURN_CODE loadVideoByUrl(String videoURL) {
	// TODO Auto-generated method stub
	VOOSMPOpenParam openParam = new VOOSMPOpenParam();
	openParam.setDecoderType(VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_VIDEO_SW.getValue() | VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_AUDIO_SW.getValue());
	open(videoURL, VO_OSMP_SRC_FLAG.VO_OSMP_FLAG_SRC_OPEN_ASYNC, VO_OSMP_SRC_FORMAT.VO_OSMP_SRC_AUTO_DETECT, openParam);

	return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}

@Override
public VO_OSMP_RETURN_CODE loadAdCall(String adCall) {
	// TODO Auto-generated method stub
	VOOSMPOpenParam openParam = new VOOSMPOpenParam();
	openParam.setDecoderType(VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_VIDEO_SW.getValue() | VO_OSMP_DECODER_TYPE.VO_OSMP_DEC_AUDIO_SW.getValue());
	open(adCall, VO_OSMP_SRC_FLAG.VO_OSMP_FLAG_SRC_OPEN_ASYNC, VO_OSMP_SRC_FORMAT.VO_OSMP_SRC_AUTO_DETECT, openParam);

	return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}

@Override
public String getVideoAdClickThru() {
	// TODO Auto-generated method stub
	 if(mADManager == null || !mADManager.isAdsManagerEnable())
	 {
		 return null;
	 }
	 
	 int nCurr = mPlayer.GetPos();
	 String str = null;
	 if(!mADManager.isSeekable(nCurr))
	 {
		 str = mADManager.setActionSync(VO_OSMP_ADSMANAGER_ACTION.VO_ADSMANAGER_ACTION_CLICK.getValue(),nCurr);
		 mADManager.setClickThrough(str);
	 }
	  
	 voLog.i(TAG, "Get AD url = " + str);
	return str;
	}

@Override
public VO_OSMP_RETURN_CODE setAdTrackingAPI(VOOSMPAdTracking tracking){
	if(mADManager == null)
	{
		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
	}
	
	int ret = mADManager.setTracking(tracking);
	  
	return VO_OSMP_RETURN_CODE.valueOf(ret);		
	}
	
@Override
public VO_OSMP_RETURN_CODE startSEINotification(int interval) {
	// TODO Auto-generated method stub
	if (mPlayer == null) {
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
    }
	  
	int ret = mPlayer.SetParam(voOSType.VOOSMP_PID_SEI_EVENT_INTERVAL, interval);
	  
	return VO_OSMP_RETURN_CODE.valueOf(ret);
}

@Override
public VO_OSMP_RETURN_CODE stopSEINotification() {
	// TODO Auto-generated method stub
	if (mPlayer == null) {
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
    }
	  
	int ret = mPlayer.SetParam(voOSType.VOOSMP_PID_SEI_EVENT_INTERVAL, -1);
	  
	return VO_OSMP_RETURN_CODE.valueOf(ret);
}


	private adsListener mAdsEventListener = new adsListener() {
        /* SDK event handling */
		@Override
		public VO_OSMP_RETURN_CODE onAdsEvent(int id, int param1, int param2,
				Object obj) {
			// TODO Auto-generated method stub
			if (mEventListener == null)
	            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
			
			VO_OSMP_CB_EVENT_ID adsEvent = null;
			
            switch (id) {
	            case VOOSMPADSManager.VO_ADSMANAGER_EVENT_CONTENT_START:
	            {
                    m_bAdHideSubtitle = false;
                    refreshSubtitle();
                    
	                adsEvent = VO_OSMP_CB_EVENT_ID.VO_OSMP_AD_CB_CONTENT_START;
	            }
                    break;
                    
	            case VOOSMPADSManager.VO_ADSMANAGER_EVENT_CONTENT_END:
	            {
	                adsEvent = VO_OSMP_CB_EVENT_ID.VO_OSMP_AD_CB_CONTENT_END;
	            }
                    break;
	            case VOOSMPADSManager.VO_ADSMANAGER_EVENT_AD_START:
	            {
	                m_bAdHideSubtitle = true;
	                refreshSubtitle();
	                
	                adsEvent = VO_OSMP_CB_EVENT_ID.VO_OSMP_AD_CB_AD_START;
	            }
                    break;
	            case VOOSMPADSManager.VO_ADSMANAGER_EVENT_AD_END:
	            {
	                adsEvent = VO_OSMP_CB_EVENT_ID.VO_OSMP_AD_CB_AD_END;
	            }
	                break;
	            
	            case VOOSMPADSManager.VO_ADSMANAGER_EVENT_OPEN_COMPLETE:
	            {
	                if (0 != param1) {
	                    adsEvent = VO_OSMP_CB_EVENT_ID.VO_OSMP_SRC_CB_OPEN_FINISHED;
	                }
	                else {
	                    VO_OSMP_RETURN_CODE ret = onLoadVideoComplete();
	                    
	                    if (VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE != ret) {
	                        if (null != mEventListener) {
	                            mEventListener.onVOEvent(VO_OSMP_CB_EVENT_ID.VO_OSMP_SRC_CB_OPEN_FINISHED, ret.getValue(), 0, null);
	                        }
	                    }
	                    
	                    return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	                }
	            }
	                break;
	            case VOOSMPADSManager.VO_ADSMANAGER_EVENT_ERROR_GEOBLOCK:
	            {
	                adsEvent = VO_OSMP_CB_EVENT_ID.VO_OSMP_AD_CB_VIDEO_GEO_BLOCKED;
	                break;
	            }
	            case VOOSMPADSManager.VO_ADSMANAGER_EVENT_ERROR_PID_INVALID:
	            case VOOSMPADSManager.VO_ADSMANAGER_EVENT_ERROR_PID_EXPIRED:
	            case VOOSMPADSManager.VO_ADSMANAGER_EVENT_ERROR_PID_NOTAVALIBLE:
                case VOOSMPADSManager.VO_ADSMANAGER_EVENT_ERROR_SMIL_DOWNLOADFAIL:
                case VOOSMPADSManager.VO_ADSMANAGER_EVENT_ERROR_SMIL_PARSEFAIL:
                {
	                adsEvent = VO_OSMP_CB_EVENT_ID.VO_OSMP_AD_CB_VIDEO_NOT_AVAILABLE;
                    break;
                }
	            case VOOSMPADSManager.VO_ADSMANAGER_EVENT_ERROR_VMAP_AUTHFAIL:
	            case VOOSMPADSManager.VO_ADSMANAGER_EVENT_ERROR_VMAP_DOWNLOADFAIL:
	            case VOOSMPADSManager.VO_ADSMANAGER_EVENT_ERROR_VMAP_PARSEERROR:
                {
                    adsEvent = VO_OSMP_CB_EVENT_ID.VO_OSMP_AD_CB_DATA_LOAD_ERROR;
                }
                    break;
	            case VOOSMPADSManager.VO_ADSMANAGER_EVENT_TRACKING_ACTION:
	            {
	                if (null == obj) {
	                    return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	                }
	                VOOSMPTrackingAction ta = (VOOSMPTrackingAction) obj;
	                
	                VO_OSMP_ADSMANAGER_ACTION action = VO_OSMP_ADSMANAGER_ACTION.valueOf(ta.getAction());
	                
	                if (action == VO_OSMP_ADSMANAGER_ACTION.VO_ADSMANAGER_ACTION_PLAYBACKCOMPLETE) {
	                    
	                    voLog.i(TAG, "ADS wrapper java VO_ADSMANAGER_ACTION_PLAYBACKCOMPLETE");
	                            
	                    mEventListener.onVOEvent(VO_OSMP_CB_EVENT_ID.VO_OSMP_AD_CB_VIDEO_DONE, 0, 0, null);
	                    
	                    mEventListener.onVOEvent(VO_OSMP_CB_EVENT_ID.VO_OSMP_CB_PLAY_COMPLETE, 0, 0, null);
	                }
	                
	                break;
	            }
	            default:
                    break;
            }
	        
			if(adsEvent != null)
			{
				VO_OSMP_RETURN_CODE result = mEventListener.onVOEvent(adsEvent,param1, param2, obj);
	            if(result == null)
	                return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;
			}
			
			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
		}
	};

	
@Override
public VOOSMPSEIPicTiming getSEIInfo(long time, VO_OSMP_SEI_INFO_FLAG flag) {
	// TODO Auto-generated method stub
	
	if (mPlayer == null) {
		voLog.e(TAG, "getSEIInfo failed, return null");
        return null;
    }
	  
	VOOSMP_SEI_EVENT_FLAG eventFlag = VOOSMP_SEI_EVENT_FLAG.valueOf(flag.getValue());
	voOSSEIPicTimingImpl impl = (voOSSEIPicTimingImpl) mPlayer.GetSEITimingInfo(eventFlag, time);
	
	VOOSMPSEIPicTimingImpl timeImpl = null;
	if (impl != null)
	{
		timeImpl = new VOOSMPSEIPicTimingImpl(impl);
	}

	
	return timeImpl;
}

@Override
public VO_OSMP_RETURN_CODE enableSEI(VO_OSMP_SEI_INFO_FLAG flag) {
	// TODO Auto-generated method stub
	  if (mPlayer == null) {
          return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
      }
	  
	  int ret = mPlayer.SetParam(voOSType.VOOSMP_PID_RETRIEVE_SEI_INFO, 
			  flag.getValue());
	  
	  return VO_OSMP_RETURN_CODE.valueOf(ret);
}

@Override
public VO_OSMP_RETURN_CODE enableFullScreen(boolean isFullScreen) {
	// TODO Auto-generated method stub
	if (mPlayer == null) {
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
    }
	
	if(mADManager != null && mADManager.isAdsManagerEnable())
	{
//		mADManager.enableFullScreen(isFullScreen);
		int nCurr = mPlayer.GetPos();
		if(isFullScreen)
		{
			mADManager.setAction(VO_OSMP_ADSMANAGER_ACTION.VO_ADSMANAGER_ACTION_FULLSCREENON.getValue(),nCurr);
		}
		else
		{
			mADManager.setAction(VO_OSMP_ADSMANAGER_ACTION.VO_ADSMANAGER_ACTION_FULLSCREENOFF.getValue(),nCurr);
		}
	}
	return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
}

@Override
public VO_OSMP_RETURN_CODE setPDConnectionRetryCount(int times) {
	// TODO Auto-generated method stub
	if (mPlayer == null) {
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
    }
	  
	  int ret = mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_PD_CONNECTION_RETRY_TIMES, 
			  times);
	  
	  return VO_OSMP_RETURN_CODE.valueOf(ret);
	}

@Override
public int[] getVideoDecodingBitrate() {
	// TODO Auto-generated method stub
	   if(mPlayer==null)
	   {
	       return null;
	   }
	   int[] vdb = (int[]) mPlayer.GetParam(voOSType.VOOSMP_PID_VIDEO_DEC_BITRATE);
	   int[] retVdb=new int[10];
	   for(int i=0;i<10;i++){
	       retVdb[i]=8*vdb[9-i];
	   }
	   return retVdb;
}

@Override
public int[] getAudioDecodingBitrate() {
	// TODO Auto-generated method stub
		if(mPlayer==null)
		{
		    return null;
		}
		int[] adb = (int[]) mPlayer.GetParam(voOSType.VOOSMP_PID_AUDIO_DEC_BITRATE);
		int[] retAdb=new int[10];
		for(int i = 0; i<10;i++)
		{
		   retAdb[9-i] = 8 * adb[i];
		}
		return retAdb;
}

@Override
public VO_OSMP_RETURN_CODE enableCPUAdaptation(boolean value) {
	// TODO Auto-generated method stub
	if (mPlayer == null) {
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
    }
	
	int ret = mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_DISABLE_CPU_ADAPTION, value?0:1);
	return VO_OSMP_RETURN_CODE.valueOf(ret);
}

@Override
public VO_OSMP_RETURN_CODE setPlaybackBufferingTime(int time) {
	// TODO Auto-generated method stub
	if (mPlayer == null) {
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
    }
    return VO_OSMP_RETURN_CODE.valueOf( mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_BUFFER_BUFFERING_TIME, time));

}

@Override
public VO_OSMP_RETURN_CODE setBitrateThreshold(int upper, int lower) {
	// TODO Auto-generated method stub
	if (mPlayer == null) {
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
    }
	
	voOSSrcBAThreshold hold = new voOSSrcBAThreshold();
	hold.setUpper(upper);
	hold.setLower(lower);
	
	int ret = mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_BITRATE_THRESHOLD, hold);
	
	return VO_OSMP_RETURN_CODE.valueOf(ret);
}

@Override
public VO_OSMP_RETURN_CODE setSegmentDownloadRetryCount(int count) {
	// TODO Auto-generated method stub
	
	if (mPlayer == null) {
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
    }
	
	int ret = mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_SEGMENT_DOWNLOAD_RETRY_COUNT, count);
	return VO_OSMP_RETURN_CODE.valueOf(ret);
}

@Override
public VO_OSMP_RETURN_CODE enableRTSPOverHTTP(boolean enable) {
	// TODO Auto-generated method stub
	if (mPlayer == null) {
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
    }
	
	int ret = mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_ENABLE_RTSP_HTTP_TUNNELING, enable?1:0);
	return VO_OSMP_RETURN_CODE.valueOf(ret);
}

@Override
public VO_OSMP_RETURN_CODE setRTSPOverHTTPConnectionPort(int portNum) {
	// TODO Auto-generated method stub
	if (mPlayer == null) {
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
    }
	
	int ret = mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_RTSP_OVER_HTTP_CONNECTION_PORT, portNum);
	return VO_OSMP_RETURN_CODE.valueOf(ret);
}

@Override
public VO_OSMP_RETURN_CODE updateSourceURL(String url) {
	// TODO Auto-generated method stub
	if (mPlayer == null) {
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
    }
	
	int ret = mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_UPDATE_SOURCE_URL, url);
	return VO_OSMP_RETURN_CODE.valueOf(ret);
}

@Override
public VO_OSMP_RETURN_CODE enableDolbyLibrary(boolean value) {
	// TODO Auto-generated method stub
	if (mPlayer == null) {
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
    }
	
	int nValue = value?1:0, ret;
	ret = mPlayer.SetParam(voOSType.VOOSMP_PID_LOAD_DOLBY_DECODER_MODULE, nValue);
	ret = mPlayer.SetParam(voOSType.VOOSMP_PID_LOAD_AUDIO_EFFECT_MODULE, nValue);
	return VO_OSMP_RETURN_CODE.valueOf(ret);
}

@Override
public VO_OSMP_RETURN_CODE previewSubtitle(String sampleText, View view) {
	// TODO Auto-generated method stub
	if (mPlayer == null) {
		mPlayer = new voOSBasePlayer();;//return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
    }
	
	voOSPreviewSubtitleInfo info = new voOSPreviewSubtitleInfo();
	info.setSampleText(sampleText);
	info.setView(view);
	
	int ret;
	ret = mPlayer.SetParam(voOSType.VOOSMP_PID_PREVIEW_SUBTITLE, info);
	
	return VO_OSMP_RETURN_CODE.valueOf(ret);
}

@Override
public VO_OSMP_RETURN_CODE setHTTPRetryTimeout(int time) {
	// TODO Auto-generated method stub
	if (mPlayer == null) {
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
    }
	
	int ret;
	ret = mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_HTTP_RETRY_TIMEOUT, time);
	return VO_OSMP_RETURN_CODE.valueOf(ret);
}

@Override
public VO_OSMP_RETURN_CODE setSubtitleTypeface(Typeface type) {
	// TODO Auto-generated method stub
	if (mPlayer == null) {
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
    }

	 voSubTitleFormatSetting sett = (voSubTitleFormatSetting) mPlayer
             .GetParam(voOSType.VOOSMP_PID_CLOSED_CAPTION_SETTINGS);
     if (sett == null)
         return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;

     sett.setTypeface(type);
     return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
}

//@Override
//public VO_OSMP_RETURN_CODE setDefaultAudioLanguage(VO_OSMP_LANGUAGE_TYPE type) {
//	// TODO Auto-generated method stub
//	if (mPlayer == null) {
//        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
//    }
//
//	int ret =  mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_DEFAULT_AUDIO_LANGUAGE, type.getValue() );
//    return VO_OSMP_RETURN_CODE.valueOf(ret);
//}
//
//@Override
//public VO_OSMP_RETURN_CODE setDefaultSubtitleLanguage(VO_OSMP_LANGUAGE_TYPE type) {
//	// TODO Auto-generated method stub
//	if (mPlayer == null) {
//        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
//    }
//
//	int ret =  mPlayer.SetParam(voOSType.VOOSMP_SRC_PID_DEFAULT_SUBTITLE_LANGUAGE, type.getValue() );
//    return VO_OSMP_RETURN_CODE.valueOf(ret);
//}

}
