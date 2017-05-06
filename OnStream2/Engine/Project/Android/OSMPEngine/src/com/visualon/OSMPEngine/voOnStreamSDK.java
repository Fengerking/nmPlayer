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
 * @file voOnStreamSDK.java
 * interface of sdk engine.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/

package com.visualon.OSMPEngine;


import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Locale;
import java.util.Timer;
import java.util.TimerTask;

import android.app.Activity;
import android.content.Context;
import android.content.res.Configuration;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.Parcel;
import android.util.DisplayMetrics;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.view.WindowManager;
import android.widget.RelativeLayout;

import com.visualon.OSMPUtils.*;
import com.visualon.widget.ClosedCaptionManager;
import com.visualon.OSMPSubTitle.*;


/**
 * OSMP+ SDK Engine.
 * 
 * 
 * 
 */
public final class voOnStreamSDK
{
	private static final String TAG = "@@@voOnStreamSDK.java";
	public static final int KEY_START_TIME 				= 1;
	
	private voAudioRender			mAudioRender;
	private voVideoRender			mVideoRender;
	private Surface 				mSurface; 				// accessed by native methods
	private SurfaceHolder  			mSurfaceHolder;
	private SurfaceView				mSurfaceView;
	private int 					mNativeContext; 		// accessed by native methods	
	private onEventListener 		mEventListener;
	private	boolean					mEventFinish;
	private Context					mContext;
	
    private EventHandler 			mEventHandler;
    private	boolean					mSurfaceChangedFinish;
    private int						mSurfaceChangedCount;
    
	private int 	       			mWidthVideo;
	private int 					mHeightVideo;	
	
	private int 	       			mOldWidthVideo;
	private int 					mOldHeightVideo;	
	
	private voOSRect				mZoomInRect;
	
	private int						mScreenWidth;
	private int						mScreenHeight;
	
	private int						mAspectRatio;
	private boolean					mbKeepAspectRatio;
	private float                   mfAspectRatio;
	
	private int 	       			mSampleRate;
	private int 					mChannels;		
	private int 					mAudioRenderFormat;
	private int 					mSampleBit;
	
	Thread							mThreadEvent;

	private	int						mEventMsg;
	private int						mEventParam1;
	private int						mEventParam2;
	
	private int						mThirdLibOp;
	private boolean					mbOMXAL;
	private boolean					mbIOMX;
	private int						mZoomMode;
	private boolean					mbVideoRenderStopping;
	
	private boolean					closeCaptionOutput = false;
	private boolean					enableInnerCloseCaption = false;
	private	Timer 					timer = null;
	private TimerTask 				timerTask= null;//new TimerTask()
	private ClosedCaptionManager 	ccMan = null;
	private	boolean					mSubtitleInEngine = true;
	//private static final int 		messageCreateCC_UI		= 0xf0f00001;
	private static final int 		messageDestroyCC_UI		= 0xf0f00002;
	private static final int 		messageShowCC_UI		= 0xf0f00003;
	private static final int 		messageCustomTag		= 0xf0f00004;
	private static final int 		messageCCEnable			= 0xf0f00005;
	private static final int 		messageCCSetXYRate				= 0xf0f00007;
	
	private static boolean 			mJNILoaded = false;

	// TASK 21051: setFixedSize heuristic
	private int	mFixedSizeWidthThreshold = -1;
	private int	mFixedSizeHeightThreshold = -1;
	private int	mFixedSizeWidthCurrent = -1;
	private int	mFixedSizeHeightCurrent = -1;

	/**
	 * Interface for event process. The client who wants to receive event from voOnStreamSDK shall implement the interface.
	 * 
	 * @author zhang_yi
	 *
	 */
	public interface onEventListener
	{
		/**
		 * Called to indicate an event.
		 * 
		 * 
		 * @param nID		type of the event, refer to
		 * <ul>
         * <li>{@link voOSType#VOOSMP_CB_PlayComplete}
         * <li>{@link voOSType#VOOSMP_CB_BufferStatus}
         * <li>{@link voOSType#VOOSMP_CB_VideoStartBuff}
         * <li>{@link voOSType#VOOSMP_CB_VideoStopBuff}
         * <li>{@link voOSType#VOOSMP_CB_AudioStartBuff}
         * <li>{@link voOSType#VOOSMP_CB_AudioStopBuff}
         * <li>{@link voOSType#VOOSMP_CB_SourceBuffTime}
         * <li>{@link voOSType#VOOSMP_CB_VideoDelayTime}
		 * <li>{@link voOSType#VOOSMP_CB_VideoLastTime}
		 * <li>{@link voOSType#VOOSMP_CB_VideoDropFrames}
		 * <li>{@link voOSType#VOOSMP_CB_VideoFrameRate}
		 * <li>{@link voOSType#VOOSMP_CB_Error}
		 * <li>{@link voOSType#VOOSMP_CB_SeekComplete}
		 * <li>{@link voOSType#VOOSMP_CB_VideoAspectRatio}
		 * <li>{@link voOSType#VOOSMP_CB_VideoSizeChanged}
		 * <li>{@link voOSType#VOOSMP_CB_CodecNotSupport}
		 * <li>{@link voOSType#VOOSMP_CB_Deblock}
		 * <li>{@link voOSType#VOOSMP_CB_MediaTypeChanged}
		 * <li>{@link voOSType#VOOSMP_CB_HWDecoderStatus}
		 * <li>{@link voOSType#VOOSMP_CB_AUTHENTICATION_RESPONSE}
		 * <li>{@link voOSType#VOOSMP_CB_LanguageInfoAvailable}
		 * <li>{@link voOSType#VOOSMP_CB_VideoRenderStart}
		 * <li>{@link voOSType#VOOSMP_CB_OpenSource}
		 * <li>{@link voOSType#VOOSMP_CB_Metadata_Arrive}
		 * <li>{@link voOSType#VOOSMP_CB_SEI_INFO}
		 * </ul>
		 * <ul>
		 * <li>{@link voOSType#VOOSMP_CB_BLUETOOTHHANDSET}
		 * <li>{@link voOSType#VOOSMP_CB_ClosedCaptionData}
         * </ul>
		 * @param nParam1 	first param code, specific to the event if needs. Typically implementation dependent.
		 * @param nParam2 	second param code, specific to the event if needs. Typically implementation dependent.
		 * @param obj 		third param code, specific to the event if needs. Typically implementation dependent.
		 * @return {@link voOSType#VOOSMP_ERR_None} if no error.
		 */
		public int onEvent(int nID, int nParam1, int nParam2, Object obj);
	}
			
	
    /**
     * Register a listener for processing event.
     *
     * @param listener for processing event callback
     */
	public void setEventListener(onEventListener listener)
	{
		mEventListener = listener;
	}
	
	
    public class EventHandler extends Handler
    {
        private voOnStreamSDK mMediaPlayer;

        public EventHandler(voOnStreamSDK mp, Looper looper) 
        {
            super(looper);
            mMediaPlayer = mp;
        }

        public void handleMessage(Message msg) 
        {      	
        	voLog.i(TAG, "handleMessage id is %d", msg.what);
            if (mMediaPlayer.mNativeContext == 0)
            {
                voLog.w(TAG, "vomeplayer went away with unhandled events");
                return;
            }
            if(msg.what == messageCCEnable){
            	setCommonCCParser(msg.arg1, msg.obj);
            	return;
            }
            if(msg.what == messageCCSetXYRate){
            	if(ccMan != null)
            		ccMan.setXYRate((Float)msg.obj);
            	return;
            }
            
            if(msg.what == messageDestroyCC_UI){
        		if(ccMan != null)
        		{
        			ccMan.show(false);
        			ccMan.clearWidget();
        			ccMan = null;
        		}
                return;
            }
            
            if(msg.what == messageShowCC_UI){
        		if(ccMan != null)
        		{
        			ccMan.show(msg.arg1==1?true:false);
        		}
                return;
            }
            
            if(msg.what == voOSType.VOOSMP_CB_ClosedCaptionData)
            {
            	if(!mSubtitleInEngine)
            		return;
	    		int nCurr = GetPos();
	    		Parcel parc = GetSubTitleSample(nCurr);
            	if(closeCaptionOutput)
            	{
            		Object arg0 = null;
            		if(parc!=null)
            		{
            			if(ccMan != null)
            				arg0 = ccMan.parcelToSubtitleInfo( parc);
            			else
            			{
            				ClosedCaptionManager cc = new ClosedCaptionManager();
            				arg0 = cc.parcelToSubtitleInfo( parc);
            				
            			}
            			msg = mEventHandler.obtainMessage(voOSType.VOOSMP_CB_ClosedCaptionData, 0, 0, arg0);
            			//voLog.v(TAG,"VOOSMP_CB_CloseCaptionData CloseCaption = nCurr = %d", nCurr); 
            		}
            		else
            			return ;
          		
            	}
            	else
            	{
	    		  if(ccMan == null || !enableInnerCloseCaption) return;
	    		  
	    		  if(parc!=null)
	    		  {
	    			  ccMan.setData(parc, true);
	    			  //voLog.v(TAG," 2 CloseCaption = nCurr = %d", nCurr); 
	    		  }
	    		  else
	    		  {
	    			  //voLog.v(TAG,"CloseCaption parc==null 3 CloseCaption = nCurr = %d", nCurr); 
	    		  }
	    		  ccMan.checkViewShowStatus(nCurr);
	    		  return;
            		
            	}
            	
            }else if (msg.what == voOSType.VOOSMP_CB_VideoAspectRatio)
    		{
            	boolean bNeedResize = false;
            	if(mAspectRatio!= msg.arg1)
            		bNeedResize = true;
    			mAspectRatio = msg.arg1;
				voLog.v(TAG, "OnEvent message VOOSMP_CB_VideoAspectRatio, mAspectRatio is %d,  ", mAspectRatio);
    			if ((bNeedResize || mOldWidthVideo != 0 && mOldHeightVideo != 0) && (mZoomMode == 0)  && (mbKeepAspectRatio) )
    			{
    				
    					
					switch(mAspectRatio)
					{
					case voOSType.VOOSMP_RATIO_00:
					{
						if(mWidthVideo!=0 && mHeightVideo != 0)
							updateVideoAspectRatio(mWidthVideo, mHeightVideo);
						
						mfAspectRatio = 0;
						break;
					}
					case voOSType.VOOSMP_RATIO_11:
					{
						updateVideoAspectRatio(1, 1);
						mfAspectRatio = 1;
						break;
					}
					case voOSType.VOOSMP_RATIO_43:
					{
						updateVideoAspectRatio(4, 3);
						mfAspectRatio = ((float)4)/((float)3);
						break;
					}
					case voOSType.VOOSMP_RATIO_169:
					{
						updateVideoAspectRatio(16, 9);
						mfAspectRatio = ((float)16)/((float)9);
						break;
					}
					case voOSType.VOOSMP_RATIO_21:
					{
						updateVideoAspectRatio(2, 1);
						mfAspectRatio = 2;
						break;
					}
					case voOSType.VOOSMP_RATIO_2331:
					{
						updateVideoAspectRatio(2, 1);
						mfAspectRatio = (float) 2.33;
						break;
					}
					default:
					{
						if(mAspectRatio>0)
							updateVideoAspectRatio((mAspectRatio >> 16), (mAspectRatio&0xFFFF));
						
						if ((mAspectRatio&0xFFFF) != 0)
						{
							mfAspectRatio = ((float)(mAspectRatio >> 16))/((float)(mAspectRatio&0xFFFF));
						}else
						{
							mfAspectRatio = 0;
						}
						break;					
					}
					
					}
    				voLog.v(TAG, "OnEvent message VOOSMP_CB_VideoAspectRatio, but get later for Video Width height, mAspectRatio is %d,  ", mAspectRatio);
    				
    			}
    		}else if (msg.what == voOSType.VOOSMP_CB_VideoSizeChanged)
        	{
    			voLog.i(TAG, "handleMessage id is VOOSMP_CB_VideoSizeChanged step1");
        		int 	width = msg.arg1;
        		int		height = msg.arg2;
        		
        		if ((width == 0) || (height == 0))
				{
        			voLog.e(TAG, "Error!, Video Width or height is 0, width is %d, height is %d", width, height);
        			return;
				}
        		
        		if ((mScreenWidth == 0) || (mScreenHeight == 0))
				{
        			voLog.e(TAG, "Error!, Screen Size is 0, please first call SetScreenSize, input screen Width or height, width is %d, height is %d", mScreenWidth, mScreenHeight);
        			return;
				}
        		
        		checkOrientationAndScreenSize();
 
        		voLog.i(TAG, "Old video width is %d, old video height is %d. \nNew video width is %d, new video height is %d. mAspectRatio is %d ",
        				mOldWidthVideo, mOldHeightVideo, width, height, mAspectRatio );	

				if (mAspectRatio !=0 && (mZoomMode == 0) && (mbKeepAspectRatio) )
				{
					
					switch(mAspectRatio)
					{
					case voOSType.VOOSMP_RATIO_11:
					{
						updateVideoAspectRatio(1, 1);
						break;
					}
					case voOSType.VOOSMP_RATIO_43:
					{
						updateVideoAspectRatio(4, 3);
						break;
					}
					case voOSType.VOOSMP_RATIO_169:
					{
						updateVideoAspectRatio(16, 9);
						break;
					}
					case voOSType.VOOSMP_RATIO_21:
					{
						updateVideoAspectRatio(2, 1);
						break;
					}
					default:
					{
						updateVideoAspectRatio((mAspectRatio >> 16), (mAspectRatio&0xFFFF));
						break;					
					}
					
					}
	        		voLog.i(TAG, "call updateVideoAspectRatio only with real aspect ratio" );	
				
				}else if ((mZoomMode == 0)&&((mOldWidthVideo == 0 && width > 0) 
						|| mOldWidthVideo * height != width * mOldHeightVideo))
				{
					updateVideoAspectRatio(width, height);
	        		voLog.i(TAG, "call updateVideoAspectRatio by width*height" );	
				}else if (mZoomMode != 0)
				{
					if (mZoomMode == voOSType.VOOSMP_ZM_PANSCAN || mZoomMode == voOSType.VOOSMP_ZM_ZOOMIN || mZoomMode == voOSType.VOOSMP_ZM_FITWINDOW)
					{
						startPanScan();
					}else if(mZoomMode == voOSType.VOOSMP_ZM_ORIGINAL)
					{
						endPanScan();
					}
				}

				mOldWidthVideo = width;
				mOldHeightVideo = height;

				if (width != 0 && height != 0) 
				{
					if(mZoomMode == 0)
					{
						// TASK 21051: setFixedSize heuristic
						if (mFixedSizeWidthThreshold == -1 || mFixedSizeWidthCurrent == -1 || (width >= mFixedSizeWidthThreshold && height >= mFixedSizeHeightThreshold))
						{

							if (mFixedSizeWidthCurrent == -1 && mFixedSizeWidthThreshold != -1) 
							{
								mFixedSizeWidthCurrent = mFixedSizeWidthThreshold;
								mFixedSizeHeightCurrent = mFixedSizeHeightThreshold;
							}
							else 
							{
								mFixedSizeWidthCurrent = width;
								mFixedSizeHeightCurrent = height;
							}
	
							if(mSurfaceView != null)
							{
								mSurfaceHolder.setFixedSize(mFixedSizeWidthCurrent, mFixedSizeHeightCurrent);
								mSurfaceView.invalidate();
								mSurfaceView.setVisibility(View.VISIBLE);
								String sr = String.format("SurfaceHolder.setFixedSize video Width is %d , Height is %d. PanScan is %d", mFixedSizeWidthCurrent, mFixedSizeHeightCurrent, mZoomMode);
								voLog.v(TAG, sr);
							}
						}
						else 
						{
						    voLog.v(TAG, "%dx%d, skip calling setFixedSize()", width, height);
						}
					}
				}   	
				if (mMediaPlayer.mEventListener != null)
	        		mMediaPlayer.mEventListener.onEvent (msg.what, msg.arg1, msg.arg2, msg.obj);
	        	
	        	mEventFinish = true;
	        	return;
        	}
//    		else if (msg.what == eventTtmlLocalFileOK)
//        	{
//				if(mNativeContext!=0)
//				{
//					voLog.i(TAG,"TCC download file valid" );
//					nativeSetParam(mNativeContext, voOSType.VOOSMP_PID_SUBTITLE_FILE_NAME, (String)msg.obj);
//				}
//				voLog.i(TAG,"TCC download file ok event" );
//
//				return ;
//        		
//        	}
        	
        	if (mMediaPlayer.mEventListener != null)
        		mMediaPlayer.mEventListener.onEvent (msg.what, msg.arg1, msg.arg2, msg.obj);
        	
        }
    }	
	
    /**
    * Construct function
    */
	public voOnStreamSDK() 
	{
        Looper looper;
        if ((looper = Looper.myLooper()) != null) {
            mEventHandler = new EventHandler(this, looper);
            voLog.v(TAG, "Looper.myLooper()");
        } else if ((looper = Looper.getMainLooper()) != null) {
            mEventHandler = new EventHandler(this, looper);
            voLog.v(TAG, "getMainLooper()");
        } else {
            mEventHandler = null;
        }
        
        mAudioRender = null;		
		mVideoRender = null;		
		
		mSurface = null;
		mSurfaceHolder = null;
		mSurfaceView = null;
		mNativeContext = 0;
		mEventListener = null;
		mContext = null;
		mWidthVideo = 0;
		mHeightVideo = 0;
		mOldWidthVideo = 0;
		mOldHeightVideo = 0;
		mScreenWidth = 0;
		mScreenHeight = 0;
		mZoomInRect = null;
		
		mAspectRatio = 0;
		mbKeepAspectRatio = true;
		
		mSampleRate = 0;
		mChannels = 0;
	    mAudioRenderFormat = voAudioRender.RENDER_FORMAT_PCM; // default to PCM 
	    mSampleBit = 16;    // default to 16-bit		
		
		mEventMsg = -1;
		mEventParam1 = 0;
		mEventParam2 = 0;
		
		mSurfaceChangedFinish = true;
		mEventFinish = true;
		mSurfaceChangedCount = 0;
		mThirdLibOp = 0;
		mbOMXAL = false;
		mbIOMX = false;
		mZoomMode = 0;
		mbVideoRenderStopping = false;
	}

	/**
	 * Initialize ENGINE instance.
	 * This function must be called at first for a session.
	 * 
	 * @param	context		Current context	
     * @param   packageName	Full path name of package such as /data/data/packagename/lib/
     * @param	options		List of Player options. This parameter is deprecated.
	 * @param	lPlayerType	Type of media framework, refer to {@link voOSType#VOOSMP_VOME2_PLAYER}, {@link voOSType#VOOSMP_OMXAL_PLAYER}.
	 * @param	initParam	Initialization param, depending on lFlag,
	 * <ul>
     * <li>if lFlag == {@link voOSType#VOOSMP_FLAG_INIT_NOUSE} then initParam should be 0;
     * <li>if lFlag == {@link voOSType#VOOSMP_FLAG_INIT_LIBOP} then initParam should be pointer of thirdly Library OP;
     * </ul> 
	 * @param	lFlag		{@link voOSType#VOOSMP_FLAG_INIT_NOUSE}, {@link voOSType#VOOSMP_FLAG_INIT_LIBOP}
	 * @return	{@link voOSType#VOOSMP_ERR_None} if succeeded 
	 * 
	 */
	public int Init (Context context, String packageName,  List<voOSOption> options, long lPlayerType, long initParam, long lFlag)  throws IllegalStateException
	{	
		int nRet;
		mContext = context;
		
		if(!mJNILoaded)
		{
			loadJNI(packageName);
			if(!mJNILoaded)
			{
				voLog.e(TAG, "Fail to load JNI library from Init()");
                return voOSType.VOOSMP_ERR_JNI;
			}
		}
		  
		
		 Date dateNow = new Date(System.currentTimeMillis());
         
        
		
//		String strAPK = "/data/data/" + mContext.getPackageName() + "/";
		nRet = (int)nativeCreate(this, packageName, lPlayerType, initParam, lFlag);
		
		if (lPlayerType == voOSType.VOOSMP_OMXAL_PLAYER)
		{
			String ver = android.os.Build.VERSION.RELEASE;
			if (ver.startsWith("4.") || ver.startsWith("5."))
				mbOMXAL = true;
			else
			{
				voLog.e(TAG, "Device OS version is low, OpenMax AL need Android4.0 or above.");
				return voOSType.VOOSMP_ERR_Unknown;
			}
		}else
		{
//			mAudioRender = new voAudioRender(this);		
			mVideoRender = new voVideoRender(this);		
			
		    if (mVideoRender != null)
		    	mVideoRender.setSurface ( mSurface);	
		       
		    mbOMXAL = false;
		} 
		mAudioRender = new voAudioRender(this);		
                mAudioRender.init();
	/*	
	    String strModel = android.os.Build.MODEL;
	    strModel = strModel.toLowerCase();
		voLog.v(TAG, strModel);
		if (strModel.contains("kindle"))
		{
			int nAuAdjustTime = 300;
			SetParam(voOSType.VOOSMP_PID_AUDIO_REND_ADJUST_TIME, nAuAdjustTime);
	
			int nAudioBufferTime = 500;
			SetParam(voOSType.VOOSMP_PID_AUDIO_SINK_BUFFTIME, nAudioBufferTime);
	
			int nAuStepTime = 300;
			SetParam (voOSType.VOOSMP_PID_AUDIO_STEP_BUFFTIME, nAuStepTime);
			voLog.v(TAG, "This device is kindle fire, setup audo param!");
		}
	*/	
		if(options!=null)
		{
			for(int i=0; i< options.size(); i++)
			{
				voOSOption opt = options.get(i);
				if(opt.getType()== voOSOption.eVoOption.eoVideoRender)
				{
					int iParam = (int) opt.getValue();
					SetParam (voOSType.VOOSMP_PID_VIDEO_RENDER_TYPE, iParam);
					voLog.v(TAG, "SetParam voOSType.VOOSMP_PID_VIDEO_RENDER_TYPE is %d .", iParam);
				}
			}
			
		}
		
		Date dateEnd = new Date(System.currentTimeMillis());
		Long l = dateEnd.getTime() - dateNow.getTime();
		voLog.i(TAG, "init spend time is %d", l.intValue());
		DisplayMetrics dm  = new DisplayMetrics();
        final WindowManager window =
        (WindowManager)mContext.getSystemService(Context.WINDOW_SERVICE);
        window.getDefaultDisplay().getMetrics(dm);
        voOSVideoFormat fmt = new voOSVideoFormatImpl(dm.widthPixels, dm.heightPixels,0);
        SetParam(voOSType.VOOSMP_PID_FIXEDSIZE_THRESHOLD, fmt);  
		return nRet;
	}
	
	
    
	/**
	 * open a video source
	 * 	
	 * @param source 	
	 * <ul>
	 * <li>if flag == {@link voOSType#VOOSMP_FLAG_SOURCE_URL}, type is String, source should be video file or link path<br>
	 * <li>if flag == {@link voOSType#VOOSMP_FLAG_SOURCE_READBUFFER}, type is Integer, source should be pointer of VOOSMP_READBUFFER_FUNC, <br>
	 * <li>					you can get the point form osmpSource.getReadBufPtr() .<br>
	 * <li>if flag == {@link voOSType#VOOSMP_FLAG_SOURCE_SENDBUFFER}, type is Integer, source should be pointer of VOOSMP_SENDBUFFER_FUNC, <br>
	 * </ul>
	 * @param flag		direction one of {{@link voOSType#VOOSMP_FLAG_SOURCE_URL}, <br>
	 * 					{@link voOSType#VOOSMP_FLAG_SOURCE_READBUFFER}, <br>
	 * 					{@link voOSType#VOOSMP_FLAG_SOURCE_SENDBUFFER}.}<br>
	 * 
	 * @return {@link voOSType#VOOSMP_ERR_None} if succeeded 
	 * 
	 */
    public int Open (Object source, long flag) throws IllegalStateException 
    {
	    mAspectRatio = 0;
	    mfAspectRatio = 0;
	    mZoomMode = 0;
		mWidthVideo = 0;
		mHeightVideo = 0;
		mOldWidthVideo = 0;
		mOldHeightVideo = 0;
		mZoomInRect = null;
    	voLog.v(TAG, "Open mAspectRatio=%d", mAspectRatio);
	    long nRC = nativeOpen (mNativeContext, source, flag);
    	return (int)nRC;
    }
    
    /**
	 * Close source
	 *
	 * @return {@link voOSType#VOOSMP_ERR_None} if succeeded 
	 */
    public int Close()
    {
    	voLog.v(TAG, "Close -");

    	int nRC = (int) nativeClose(mNativeContext);
    	voLog.v(TAG, "Close +, return value is " + nRC);

    	mAspectRatio = 0;
	    mfAspectRatio = 0;
        mWidthVideo = 0;
        mHeightVideo = 0;
        
    	return nRC;
    }

    /**
     * Start to play
     * 
     * @return    {@link voOSType#VOOSMP_ERR_None} if succeeded 
     */ 
	public int Run () 
	{
	    voLog.v(TAG, "run");
	    
	    int nRet = (int)nativeRun (mNativeContext);
	    if (nRet != voOSType.VOOSMP_ERR_None){
		    voLog.v(TAG, "run return error");
	    	return nRet;
	    }
	    
		if (mVideoRender != null)
			mVideoRender.run ();
		if (mAudioRender != null)
			mAudioRender.run ();
		
		return nRet;
	}

    /**
     * Pause 
     * 
     * @return    {@link voOSType#VOOSMP_ERR_None} if succeeded 
	 * 
     */ 
	public int Pause ()
	{	
	    int nRet = (int)nativePause (mNativeContext);
	    if (nRet == voOSType.VOOSMP_ERR_None)
	    {
	    	if (mAudioRender != null)
				mAudioRender.pause ();
	    	if (mVideoRender != null)
				mVideoRender.pause ();
			
			voLog.i(TAG, "Engine pause successful, audio and video Render puase.");
	    }
	    
		return nRet;
	}
	
    /**
     * Stop play
     * 
     * @return   {@link voOSType#VOOSMP_ERR_None} if succeeded 
	 * 
     */ 
	public int Stop()
	{
	    voLog.v(TAG, "stop-");
	    
		if (mVideoRender != null)
		{
			mbVideoRenderStopping = true;
			mVideoRender.stop ();
			mbVideoRenderStopping = false;
		}
		
		if (mAudioRender != null)
			mAudioRender.stop ();
		Integer param = new Integer(0);
		nativeSetParam(mNativeContext, voOSType.VOOSMP_PID_COMMON_CCPARSER, param);
		enableInnerCloseCaption = false;
		Message m = mEventHandler.obtainMessage(messageDestroyCC_UI, 0, 0, null);//local file
		mEventHandler.sendMessage(m);
		
		voLog.v(TAG, "nativeStop-");
		int nRC = (int)nativeStop (mNativeContext);
		voLog.v(TAG, "stop+ nativeStop+, return value is " + nRC);

	    return nRC;
	}
	
    /**
     * Seek the current playback point to a new location
     * 
     * @param     pos	Time stamp to seek to
     * @return    The correct position after seek operation, -1 if seek failed
     */ 
	public int SetPos (long pos)
	{
	    voLog.v(TAG, "setPos" + "pos is " + pos);
	   
	    int nRet = (int)nativeSetPos (mNativeContext, pos);
    	if (nRet != voOSType.VOOSMP_ERR_None)
	    	return nRet;
	    
		if (mAudioRender != null)
			mAudioRender.flush();	    
	 	    
		if (mVideoRender != null)
			mVideoRender.setPos (pos);
		
		return 0;
	}

    /**
     * Get the current playback point
     * 
     * @return   current position,-1 if fialed 
     */ 
	public int GetPos ()
	{
		int nPos = (int)nativeGetPos (mNativeContext);
		
		return nPos;
	}
 


	 /**
     * Set playback volume
     * 
     * @param     fLeft	Left audio channel. A value of 0.0f is silence, a value of 1.0f is no attenuation.
     * @param	  fRight	Right audio channel
     * @return    {@link voOSType#VOOSMP_ERR_None} if succeeded 
     */ 
	public int SetVolume(float fLeft, float fRight) throws IllegalStateException 
	{
		if (mAudioRender != null)
			mAudioRender.arsetVolume(fLeft, fRight); 
		
		return 0;
	}
	
    /**
     * Uninitialize ENGINE instance.
	 * This function must be called at last for a session.
     * 
     * @return    {@link voOSType#VOOSMP_ERR_None} if succeeded 
     */ 
	public int Uninit() 
	{
	    voLog.v(TAG, "Uninit -");
	    Date dateNow = new Date(System.currentTimeMillis());
		
		Stop ();
		
		this.closeTimer();
		
                if (mAudioRender != null) 
                    mAudioRender.unInit();

		long nRC = nativeDestroy(mNativeContext);
		
        if (voOSType.VOOSMP_ERR_None == nRC) {
            mNativeContext = 0;
        }

		mEventListener = null;		

        mEventHandler.removeCallbacksAndMessages(null);
        
        mbOMXAL = false;
        mbIOMX = false;
        mZoomMode = 0;
        
        Date dateEnd = new Date(System.currentTimeMillis());
		Long l = dateEnd.getTime() - dateNow.getTime();
		voLog.i(TAG, "Uninit + , init spend time is %d", l.intValue());
		return (int)nRC;
	}

	/**
	 * Set the current SurfaceView
	 * 
	 * @param     sv	current SurfaceView object
	 */ 
	public void SetView (SurfaceView sv) 
	{
	    if (sv == null)
	    {
	    	mSurface = null;
	    	nativeSetSurface(mNativeContext);
	    	voLog.v(TAG, "Surface Destroy, value is null");
	    	return;
	    }

	    mSurfaceView = sv;
	    mSurfaceHolder = mSurfaceView.getHolder();
	    voLog.v(TAG, "setDisplay suface is " + mSurfaceHolder);
	    
	    if (mSurfaceHolder != null) 
			mSurface = mSurfaceHolder.getSurface();
		else
			mSurface = null;

	    if (mVideoRender != null)
	    {
	    	mVideoRender.setSurface ( mSurface);
	    	mVideoRender.setSurfaceHolder (mSurfaceHolder);
	    }

	    nativeSetSurface(mNativeContext);
	}
	
	/**
	* Set a surface holder for video
	* 
	* @param  sh 	Current Surface Holder
	* 
	*/
	public void SetSurfaceHolder (SurfaceHolder sh) 
	{
		if (sh == null)
	    {
	    	mSurface = null;
	    	nativeSetSurface(mNativeContext);
	    	voLog.v(TAG, "Surface Destroy, value is null");
	    	return;
	    }

	    mSurfaceView = null;
	    mSurfaceHolder = sh;
	    voLog.v(TAG, "setDisplay suface is " + mSurfaceHolder);
	    
	    if (mSurfaceHolder != null) 
			mSurface = mSurfaceHolder.getSurface();
		else
			mSurface = null;

	    if (mVideoRender != null)
	    {
	    	mVideoRender.setSurface ( mSurface);
	    	mVideoRender.setSurfaceHolder (mSurfaceHolder);
	    }

	    nativeSetSurface(mNativeContext);
	}
	
	private void createCloseCaptionUI()
	{
		if(!enableInnerCloseCaption)
			return;
		if(ccMan!=null)
			ccMan.show(true);
		if(ccMan!=null || mSurfaceView == null)
			return;
		if(mSurfaceView.getParent()!=null)
		{
	    	ccMan = new ClosedCaptionManager();
	    	ccMan.setActivity(this.mContext);
	    	ccMan.setSurfaceView(mSurfaceView);
	    	
	    	startTimer();
			
		}
	}
	
    /**
     *  Set the special value according to param ID
     * 
     * @param     id	type of the parameter, refer to
		 * <ul>
         * <li>{@link voOSType#VOOSMP_PID_AUDIO_VOLUME}
         * <li>{@link voOSType#VOOSMP_PID_DRAW_RECT}
         * <li>{@link voOSType#VOOSMP_PID_DRAW_COLOR}
         * <li>{@link voOSType#VOOSMP_PID_FUNC_LIB}
         * <li>{@link voOSType#VOOSMP_PID_MAX_BUFFTIME}
         * <li>{@link voOSType#VOOSMP_PID_MIN_BUFFTIME}
         * <li>{@link voOSType#VOOSMP_PID_VIDEO_REDRAW}
         * <li>{@link voOSType#VOOSMP_PID_AUDIO_EFFECT_ENABLE}
         * <li>{@link voOSType#VOOSMP_PID_LICENSE_TEXT}
         * <li>{@link voOSType#VOOSMP_PID_LICENSE_FILE_PATH}
         * <li>{@link voOSType#VOOSMP_PID_PLAYER_PATH}
         * <li>{@link voOSType#VOOSMP_PID_FUNC_DRM}
         * <li>{@link voOSType#VOOSMP_PID_AUDIO_REND_ADJUST_TIME}
         * <li>{@link voOSType#VOOSMP_PID_ZOOM_MODE}
         * <li>{@link voOSType#VOOSMP_PID_CPU_NUMBER}
         * <li>{@link voOSType#VOOSMP_PID_LISTENER}
         * <li>{@link voOSType#VOOSMP_PID_AUDIO_REND_BUFFER_TIME}
         * <li>{@link voOSType#VOOSMP_PID_VIDEO_RENDER_TYPE}
         * <li>{@link voOSType#VOOSMP_PID_AUDIO_DECODER_FILE_NAME}										
         * <li>{@link voOSType#VOOSMP_PID_VIDEO_DECODER_FILE_NAME}
         * <li>{@link voOSType#VOOSMP_PID_PERFORMANCE_DATA}
         * <li>{@link voOSType#VOOSMP_PID_FUNC_READ_IO}
         * <li>{@link voOSType#VOOSMP_PID_AUDIO_STEP_BUFFTIME}
         * <li>{@link voOSType#VOOSMP_PID_AUDIO_SINK_BUFFTIME}
         * <li>{@link voOSType#VOOSMP_PID_AUDIO_STREAM_ONOFF}
         * <li>{@link voOSType#VOOSMP_PID_VIDEO_STREAM_ONOFF}
         * <li>{@link voOSType#VOOSMP_PID_AUDIO_DECODER_API_NAME}
         * <li>{@link voOSType#VOOSMP_PID_VIDEO_DECODER_API_NAME}
         * <li>{@link voOSType#VOOSMP_PID_DEBLOCK_ONOFF}
         * <li>{@link voOSType#VOOSMP_PID_LOAD_AUDIO_EFFECT_MODULE}
         * <li>{@link voOSType#VOOSMP_PID_AUTHENTICATION_INFO}
         * <li>{@link voOSType#VOOSMP_PID_SURFACE_CHANGED}
	     * <li>{@link voOSType#VOOSMP_PID_COMMON_CCPARSER}
	     * <li>{@link voOSType#VOOSMP_PID_LICENSE_CONTENT}
	     * <li>{@link voOSType#VOOSMP_PID_SOURCE_FILENAME}
	     * <li>{@link voOSType#VOOSMP_PID_LIB_NAME_PREFIX}
	     * <li>{@link voOSType#VOOSMP_PID_LIB_NAME_SUFFIX}
	     * <li>{@link voOSType#VOOSMP_PID_COMMAND_STRING}
         * <li>{@link voOSType#VOOSMP_PID_HDMI_DISPLAY}
         * <li>{@link voOSType#VOOSMP_PID_CLOSED_CAPTION_OUTPUT}
         * <li>{@link voOSType#VOOSMP_PID_LOW_LATENCY}
         * <li>{@link voOSType#VOOSMP_PID_VIEW_ACTIVE}
         * <li>{@link voOSType#VOOSMP_PID_VIDEO_PERFORMANCE_OPTION}
         * <li>{@link voOSType#VOOSMP_PID_SUBTITLE_SETTINGS}
         * <li>{@link voOSType#VOOSMP_PID_VIDEO_PERFORMANCE_ONOFF}
         * <li>{@link voOSType#VOOSMP_PID_SUBTITLE_SETTINGS_ONOFF}
         * <li>{@link voOSType#VOOSMP_PID_APPLICATION_SUSPEND}
         * <li>{@link voOSType#VOOSMP_PID_APPLICATION_RESUME}
         * <li>{@link voOSType#VOOSMP_PID_AUDIO_SINK}
         * <li>{@link voOSType#VOOSMP_PID_BITMAP_HANDLE}
         * <li>{@link voOSType#VOOMXAL_PID_AUDIO_STREAM_ONOFF}
         * <li>{@link voOSType#VOOMXAL_PID_VIDEO_STREAM_ONOFF}
         * </ul>
     * @param  param   depend on different id.
     */ 
	public int SetParam(long id, Object param)
	{
		if (id == voOSType.VOOSMP_PID_SURFACE_CHANGED)
		{
			mSurfaceChangedFinish = true;
			return 0;
		}else if (id == voOSType.VOOSMP_PID_ZOOM_MODE)
		{
			mZoomMode = (Integer)param;
			if (mZoomMode == voOSType.VOOSMP_ZM_PANSCAN || mZoomMode == voOSType.VOOSMP_ZM_FITWINDOW)
			{
				startPanScan();
			}else if(mZoomMode == voOSType.VOOSMP_ZM_ORIGINAL || mZoomMode == voOSType.VOOSMP_ZM_LETTERBOX)
			{
				mZoomMode = voOSType.VOOSMP_ZM_ORIGINAL;
				endPanScan();
			}
			return 0;
		}else if (id == voOSType.VOOSMP_PID_DRAW_RECT)
		{
			voOSRect rect = (voOSRect)param;
			if (mZoomMode == voOSType.VOOSMP_ZM_ZOOMIN && rect!= null)
			{
				mZoomInRect = rect;
				startPanScan();
				return 0;
			}
			
			int[] intArray = new int[4];
			intArray[0] = rect.Left();
			intArray[1] = rect.Top();
			intArray[2] = rect.Right();
			intArray[3] = rect.Bottom();
			return (int) nativeSetParam(mNativeContext, id, intArray);
		}
		else if (id == voOSType.VOOSMP_PID_VIEW_ACTIVE
				|| id == voOSType.VOOSMP_PID_APPLICATION_RESUME)
		{
			mSurfaceView = (SurfaceView)param;
			mSurfaceHolder = mSurfaceView.getHolder();
			voLog.v(TAG, "setDisplay suface is " + mSurfaceHolder + " ID = " + id + " , mbOMXAL is " + mbOMXAL);
			    
			if (mSurfaceHolder != null) 
				mSurface = mSurfaceHolder.getSurface();
			else
				mSurface = null;
		    
			if ((mVideoRender != null) && (!mbOMXAL))
		    {
		    	mVideoRender.setSurface ( mSurface);
		    	mVideoRender.setSurfaceHolder (mSurfaceHolder);
		    
		    	nativeSetSurface(mNativeContext);
		    }
			nativeSetParam(mNativeContext, id, mSurface);
			return 0;
		}else if(id == voOSType.VOOSMP_PID_IOMX_PROBE)
		{
			
			 mSurfaceView = (SurfaceView)param;
			 mSurfaceHolder = mSurfaceView.getHolder();
			 if (mSurfaceHolder != null) 
				mSurface = mSurfaceHolder.getSurface();
			else
				mSurface = null;
			voLog.v(TAG, "set VOOSMP_PID_IOMX_PROBE suface is " + mSurfaceHolder + " ID = " + id + " , mSurface is " + mSurface);
			
			  int nRet = (int)nativeSetParam(mNativeContext, id, mSurface);
			 voLog.v(TAG, "set VOOSMP_PID_IOMX_PROBE result is %d ",nRet);
			 return nRet;
		}
		else if (id == voOSType.VOOSMP_PID_VIDEO_PERFORMANCE_OPTION )
		{
			if (param == null)
			{
				voLog.i(TAG, "Param data is NULL!");
				return voOSType.VOOSMP_ERR_Unknown;
			}
				
			int[] intArray = new int[30];
			voOSVideoPerformance perf = (voOSVideoPerformance)param;

			intArray[0] = perf.LastTime();
			intArray[6] = perf.SourceTimeNum();
			intArray[7] = perf.CodecTimeNum();
			intArray[8] = perf.RenderTimeNum();
			intArray[9] = perf.JitterNum();
			intArray[12] = perf.CPULoad();
/*
			intArray[0] = perf.LastTime();
			intArray[1] = perf.SourceDropNum();
			intArray[2] = perf.CodecDropNum();
			intArray[3] = perf.RenderDropNum();
			intArray[4] = perf.DecodedNum();
			intArray[5] = perf.RenderNum();
			intArray[6] = perf.SourceTimeNum();
			intArray[7] = perf.CodecTimeNum();
			intArray[8] = perf.RenderTimeNum();
			intArray[9] = perf.JitterNum();
			intArray[10] = perf.CodecErrorsNum();
			intArray[11] = 0;
			intArray[12] = perf.CPULoad();
			intArray[13] = perf.Frequency();
			intArray[14] = perf.MaxFrequency();
			intArray[15] = perf.WorstDecodeTime();
			intArray[16] = perf.WorstRenderTime();
			intArray[17] = perf.AverageDecodeTime();
			intArray[18] = perf.AverageRenderTime();
			intArray[19] = perf.TotalCPULoad();
			intArray[20] = perf.TotalPlaybackDuration();
			intArray[21] = perf.TotalSourceDropNum();
			intArray[22] = perf.TotalCodecDropNum();
			intArray[23] = perf.TotalRenderDropNum();
			intArray[24] = perf.TotalDecodedNum();
			intArray[25] = perf.TotalRenderNum();
*/
			
			return (int)nativeSetParam(mNativeContext, id, intArray);
		}else if (id == voOSType.VOOSMP_PID_COMMON_CCPARSER)
		{
			Message m = mEventHandler.obtainMessage(messageCCEnable, (int)id, 0, param);//local file
			mEventHandler.sendMessage(m);

			return 0;//setCommonCCParser(id,param);
		}else if (id == voOSType.VOOSMP_PID_CLOSED_CAPTION_OUTPUT)
		{
			if(((Integer)param) == 1)
			{
				if(!enableInnerCloseCaption)
					nativeSetParam(mNativeContext, voOSType.VOOSMP_PID_COMMON_CCPARSER, 1);
				if(mSubtitleInEngine)
				{
					closeCaptionOutput = true;
					startTimer();
					Message m = mEventHandler.obtainMessage(messageShowCC_UI, 0, 0, null);//local file
					mEventHandler.sendMessage(m);
					//if(ccMan != null)	
					//	ccMan.show(false);
				}
			}
			else
			{
				//closeTimer();
				if(mSubtitleInEngine)
				{
					closeCaptionOutput = false;
					//if(ccMan != null)	
					//	ccMan.show(true);
					Message m = mEventHandler.obtainMessage(messageShowCC_UI, 1, 0, null);//local file
					mEventHandler.sendMessage(m);
				}
				
			}
			return 0;
		}else if (id == voOSType.VOOSMP_PID_SUBTITLE_IN_ENGINE)
		{
			mSubtitleInEngine = ((Integer)param == 0)?false:true;
			if(!mSubtitleInEngine)
			{
				this.closeTimer();
				ccMan = null;
			}
		}else if (id == voOSType.VOOSMP_PID_BLUETOOTH_HANDSET && ((Integer)param == 1))
        {
        	String strModel = android.os.Build.MODEL;
    	    strModel = strModel.toLowerCase();
    		
    	    if (strModel.contains("im-a760s") || strModel.contains("galaxy nexus"))
    		{
    			int nAuAdjustTime = 300;
    			SetParam(voOSType.VOOSMP_PID_AUDIO_REND_ADJUST_TIME, nAuAdjustTime);
    	
    			int nAudioBufferTime = 500;
    			SetParam(voOSType.VOOSMP_PID_AUDIO_SINK_BUFFTIME, nAudioBufferTime);
    	
    			int nAuStepTime = 300;
    			SetParam (voOSType.VOOSMP_PID_AUDIO_STEP_BUFFTIME, nAuStepTime);
    			voLog.v(TAG, "This device is "+strModel+", setup audo param!");
    		}
        }else if (id == voOSType.VOOSMP_PID_VIDEO_RENDER_TYPE)
        {
        	int nParam = (Integer)param;
        	if (mVideoRender != null)
        		mVideoRender.setRenderType(nParam);
        	
        	voLog.v(TAG, "Set Render type is " + nParam);
        	if (nParam == voOSType.VOOSMP_RENDER_TYPE_HW_RENDER ||
        		voOSType.VOOSMP_RENDER_TYPE_JMHW_RENDER == nParam)
        	{
        		String apiName =  null;
    			String fileName = null;
    			String ver = android.os.Build.VERSION.RELEASE;
    			
    			if(voOSType.VOOSMP_RENDER_TYPE_HW_RENDER == nParam)
    			{
    				apiName = "voGetIOMXDecAPI";
    				if (ver.startsWith("2.2"))
    					fileName = "voIOMXDec_fry";
    				else if (ver.startsWith("2.3"))
    					fileName = "voIOMXDec_gb";
    				else if (ver.startsWith("3."))
    					fileName = "voIOMXDec_hc";
    				else if (ver.startsWith("4.")){
    					if (ver.startsWith("4.0") || ver.startsWith("4.1") || ver.startsWith("4.2"))
    						fileName = "voIOMXDec_ics";
    					else if (ver.startsWith("4.3"))
        					fileName = "voIOMXDec_jb";
    					else if (ver.startsWith("4.4"))
    						fileName = "voIOMXDec_kk";
    					else
    						fileName = "voIOMXDec_kk";
    				}
    				else if (ver.startsWith("5."))
    					fileName = "voIOMXDec_kk";
    				else
    					fileName = null;
    			}
    			else if (voOSType.VOOSMP_RENDER_TYPE_JMHW_RENDER == nParam)
    			{
    				apiName = "voGetAMediaCodecAPI";
    				fileName= "voAMediaCodec";
    			}
    			if (fileName == null)
    			{
    				voLog.e(TAG, "Device Android version is too low, don't run hardware decode and render!");
    				return voOSType.VOOSMP_ERR_Unknown;
    			}
    			else
    			{
    				// modified by gtxia 2013-4-15
    				if(voOSType.VOOSMP_RENDER_TYPE_HW_RENDER == nParam ||
    					nParam == voOSType.VOOSMP_RENDER_TYPE_JMHW_RENDER)
    					mbIOMX = true;
    				
    				
    				nativeSetParam(mNativeContext,
    						voOSType.VOOSMP_PID_VIDEO_DECODER_FILE_NAME|voOSType.VOOSMP_VIDEO_CODINGTYPE.VOOSMP_VIDEO_CodingH264.ordinal(), 
    						fileName);
    				nativeSetParam(mNativeContext,
    						voOSType.VOOSMP_PID_VIDEO_DECODER_API_NAME|voOSType.VOOSMP_VIDEO_CODINGTYPE.VOOSMP_VIDEO_CodingH264.ordinal(),
    						apiName);
    				int nRet = (int) nativeSetParam(mNativeContext, voOSType.VOOSMP_PID_VIDEO_RENDER_TYPE, nParam);
    				voLog.v(TAG, "Run hardware decode and render type! Decode file name is " + fileName + ", api name is " + apiName);
    				return nRet;
    			}
        	}
    		else
        		mbIOMX = false;
        	
		} else if (id == voOSType.VOOSMP_PID_DRAW_COLOR) {
			int nParam = (Integer) param;
			if (mVideoRender != null)
				mVideoRender.setColorType(nParam);

		} else if (id == voOSType.VOOSMP_PID_KEEP_ASPECT_RATIO) {
			int nParam = (Integer) param;
			if (nParam == 1) {
				mbKeepAspectRatio = true;
			} else if (nParam == 0) {
				mbKeepAspectRatio = false;
			}
		} else if (id == voOSType.VOOSMP_PID_FIXEDSIZE_THRESHOLD) {
			// TASK 21051: setFixedSize heuristic
			String ver = Build.VERSION.RELEASE.toLowerCase();
			if (ver.startsWith("4.3") || ver.startsWith("4.2")
					|| ver.startsWith("4.1")) {
				voOSVideoFormat fmt = (voOSVideoFormat) param;
				if (fmt.Height() != -1 && fmt.Width() != -1) {
					mFixedSizeWidthThreshold = fmt.Width();
					mFixedSizeHeightThreshold = fmt.Height();
				}
			}
			return 0;
		} else if( voOSType.VOOSMP_PID_MEDIACODEC_AUDIO_DECODER == id)
		{
			// added by gtxia 2013-7-11 to make the MediaCodec take true effect with audio 
			int nParam = (Integer) param;
			if(1 == nParam)
			{
				String apiName =  "voGetAudioMCDecAPI";
    			String fileName = "voAudioMCDec";
    			
    			nativeSetParam(mNativeContext,
						voOSType.VOOSMP_PID_AUDIO_DECODER_FILE_NAME|voOSType.VOOSMP_AUDIO_CODINGTYPE.VOOSMP_AUDIO_CodingAAC.ordinal(), 
						fileName);
				nativeSetParam(mNativeContext,
						voOSType.VOOSMP_PID_AUDIO_DECODER_API_NAME|voOSType.VOOSMP_AUDIO_CODINGTYPE.VOOSMP_AUDIO_CodingAAC.ordinal(),
						apiName);
				int nRet = (int) nativeSetParam(mNativeContext, voOSType.VOOSMP_PID_MEDIACODEC_AUDIO_DECODER, nParam);
				voLog.v(TAG, "Run hardware decode and render! Decode file name is " + fileName + ", api name is " + apiName);
				return nRet;
			}
			
		} else if (id == voOSType.VOOSMP_PID_AUDIO_RENDER_FORMAT)
		{
			int[] intArray = new int[4];
			voOSAudioRenderFormat  audioRenderFormat = (voOSAudioRenderFormat) param;
			intArray[0] = audioRenderFormat.getSampleRate();
			intArray[1] = audioRenderFormat.getChannels();
			intArray[2] = audioRenderFormat.getSampleBits();
			intArray[3] = audioRenderFormat.getBufferSize();
			return (int) nativeSetParam(mNativeContext, id, intArray);
		}
		return (int) nativeSetParam(mNativeContext, id, param);
	}
	
	/** set common CCParser param*/
	private int setCommonCCParser(long id, Object param){
		if(param!=null && ((Integer)param) == 1)
		{
			nativeSetParam(mNativeContext, voOSType.VOOSMP_PID_COMMON_CCPARSER, 1);
			enableInnerCloseCaption = true;
			if(mSubtitleInEngine)
			{
				createCloseCaptionUI();
				if(ccMan != null ){
					ccMan.clearWidget();
				}
			}
		}
		else
		{
			//nativeSetParam(mNativeContext, voOSType.VOOSMP_PID_COMMON_CCPARSER, param);
			enableInnerCloseCaption = false;
			if(mSubtitleInEngine)
			{
				if(ccMan != null)	
					ccMan.show(false);
			}
		}
		return 0;
	}
    /**
     * Get the special value from param ID
     * 
     * @param      id	type of the parameter
		 * <ul>
         * <li>{@link voOSType#VOOSMP_PID_STATUS}
         * <li>{@link voOSType#VOOSMP_PID_SENDBUFFERFUNC}
         * <li>{@link voOSType#VOOSMP_PID_AUDIO_FORMAT}
         * <li>{@link voOSType#VOOSMP_PID_VIDEO_FORMAT}
         * <li>{@link voOSType#VOOSMP_PID_PLAYER_TYPE}
		 * <li>{@link voOSType#VOOSMP_PID_AUDIO_DEC_BITRATE}
		 * <li>{@link voOSType#VOOSMP_PID_VIDEO_DEC_BITRATE}
		 * <li>{@link voOSType#VOOSMP_PID_CPU_INFO}
		 * <li>{@link voOSType#VOOSMP_PID_BLUETOOTH_HANDSET}
	     * <li>{@link voOSType#VOOSMP_PID_COMMAND_STRING}
	     * <li>{@link voOSType#VOOSMP_PID_CLOSED_CAPTION_SETTINGS}
	     * <li>{@link voOSType#VOOSMP_PID_VIDEO_PERFORMANCE_OPTION}
	     * <li>{@link voOSType#VOOSMP_PID_SUBTITLE_SETTINGS}
	     * <li>{@link voOSType#VOOSMP_PID_MODULE_VERSION}
         * </ul>
     * @return    Depending on different id. Please refer to {@link voOSType} for details. return null if failed.
     */ 
	public Object GetParam(long id) 
	{
		if (id == voOSType.VOOSMP_PID_CLOSED_CAPTION_SETTINGS)
		{
			if(ccMan == null)
				return null;
			else
				return ccMan.getSettings();
		}
		Object obj  = nativeGetParam(mNativeContext, id);
		
		voLog.i(TAG, "GetParam id is %s", Integer.toHexString((int) id));

        if (id ==voOSType.VOOSMP_PID_VIDEO_DEC_BITRATE)
        {
            if (obj == null)
                return null;
            int[] iArr = (int[])obj;
            return iArr;
        }
		if (id == voOSType.VOOSMP_PID_CPU_INFO)
		{
			if (obj == null)
				return null;
			int[] iArr = (int[])obj;
			voLog.i(TAG, "CPU info %d %d %d", iArr[0], iArr[1], iArr[2]);
			voOSCPUInfoImpl cpuInfo = new voOSCPUInfoImpl(iArr[0], iArr[1], iArr[2], 0);
			return cpuInfo;
		}else if (id == voOSType.VOOSMP_PID_VIDEO_FORMAT)
		{
			if (obj == null)
				return null;
			int[] iArr = (int[])obj;
			voLog.i(TAG, "VOOSMP_PID_VIDEO_FORMAT %d %d %d" , iArr[0], iArr[1], iArr[2]);
			voOSVideoFormatImpl videoFormat = new voOSVideoFormatImpl(iArr[0], iArr[1], iArr[2]);
			return videoFormat;
		}else if (id == voOSType.VOOSMP_PID_AUDIO_FORMAT)
		{
			if (obj == null)
				return null;
			int[] iArr = (int[])obj;
			voLog.i(TAG, "VOOSMP_PID_AUDIO_FORMAT %d %d %d" , iArr[0], iArr[1], iArr[2]);
			voOSAudioFormatImpl videoFormat = new voOSAudioFormatImpl(iArr[0], iArr[1], iArr[2]);
			return videoFormat;
		}else if (id == voOSType.VOOSMP_PID_VIDEO_PERFORMANCE_OPTION )
		{
			if (obj == null)
			{
				voLog.i(TAG, "Get Performace Data Error!!");
				return null;
			}
			Parcel parcel = (Parcel)obj;
			parcel.setDataPosition(0);
	        if (parcel.dataAvail() == 0) 
	        {
	            return null;
	        }
	      
			voOSVideoPerformanceImpl perf = new voOSVideoPerformanceImpl();
			perf.parser(parcel);
			
		
			
			voLog.i(TAG, "Get Performance Data, LastTime %d, SourceDropNum %d, CodecDropNum %d, RenderDropNum %d, DecodedNum %d, " + 
				"RenderNum %d, SourceTimeNum %d, CodecTimeNum %d, RenderTimeNum %d, JitterNum %d, CodecErrorsNum %d, " +
				"CPULoad %d, Frequency %d, MaxFrequency %d, WorstDecodeTime %d, WorstRenderTime %d, " + 
				"AverageDecodeTime %d, AverageRenderTime %d, TotalCPULoad %d, TotalPlaybackDuration %d, TotalSourceDropNum %d" +
				"TotalCodecDropNum %d, TotalRenderDropNum %d, TotalDecodedNum %d, TotalRendernum %d",
				perf.LastTime(), perf.SourceDropNum(), perf.CodecDropNum(), perf.RenderDropNum(), perf.DecodedNum(),
				perf.RenderNum(), perf.SourceTimeNum(), perf.CodecTimeNum(), perf.RenderTimeNum(), perf.JitterNum(), perf.CodecErrorsNum(),
				perf.CPULoad(), perf.Frequency(), perf.MaxFrequency(), perf.WorstDecodeTime(), perf.WorstRenderTime(),
				perf.AverageDecodeTime(), perf.AverageRenderTime(), perf.TotalCPULoad(),
				perf.TotalPlaybackDuration(), perf.TotalSourceDropNum(), perf.TotalCodecDropNum(),
				perf.TotalRenderDropNum(), perf.TotalDecodedNum(), perf.TotalRenderNum());
				
			for (int i = 0; i < perf.CodecErrorsNum(); i++)
			{
				voLog.i(TAG, "CodecErrors index = %d, errorcode is %d", i, perf.CodecErrors()[i]);
			}
			
			return perf;
		
		}else if((((int)id) & 0x0000FFFF) == voOSType.VOOSMP_PID_MODULE_VERSION){
			if (obj == null)
				return null;
			Parcel parcel = (Parcel)obj;
			voOSModuleVersionImpl ver = new voOSModuleVersionImpl();
			if(ver.parse(parcel))
			{
				voLog.i(TAG, " get module version %s, module type is %d, version is  %s", Integer.toHexString((int) id), ver.GetModuleType(), ver.GetVersion());
				return ver;
			}
			voLog.i(TAG, " get module version error,id is VOOSMP_PID_MODULE_VERSION ");
			
			
		}
		return obj;
	}
	
	/**
	 * Get Video Data
	 * 
	 * @param data 		video data buffer
	 * @return			Success return {@link voOSType#VOOSMP_ERR_None}, error return error code. 
	 */
	public 	long GetVideoData (byte[] data)	
	{
		return nativeGetVideoData (mNativeContext, data);	
	}
	
	/**
	 * Get Audio Data
	 * 
	 * @param data 		audio data buffer
	 * @return			Success return {@link voOSType#VOOSMP_ERR_None}, error return error code. 
	 */
	public 	long GetAudioData ( byte[] data)
	{
		return nativeGetAudioData (mNativeContext, data);
	}
	
	/**
	 * Send video size changed event to upper player
	 * 
	 */
	public void VideoSizeChanged ()
	{
		mSurfaceChangedFinish = false;
		mEventFinish = false;
		mSurfaceChangedCount++;
		voLog.i(TAG,"VideoSizeChanged, before send message");
        Message m = mEventHandler.obtainMessage(voOSType.VOOSMP_CB_VideoSizeChanged, mWidthVideo, mHeightVideo, this);
        mEventHandler.sendMessage(m);

        int i = 0;
        int j = 0;
        
		while ((!mEventFinish) && (!mbVideoRenderStopping))
		{
			i++;
			try {
				Thread.sleep(1);
				 voLog.i(TAG,"waiting VideoSizeChanged");
				
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}

		voLog.i(TAG, "VideoSizeChanged, spend %d milliseconds. mSurfaceChangedCount is %d", i, mSurfaceChangedCount);
		mSurfaceChangedFinish = false;
		while ((!mSurfaceChangedFinish) && (mSurfaceChangedCount ==1))
		{
			j++;
			if (j > 200)
				break;
			
			try {
				Thread.sleep(1);
				
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}

		voLog.i(TAG, "VideoSizeChanged after surfacechanged, spend %d milliseconds, mSurfaceChangedFinish is %b, mSurfaceChangedCount is %d",
				j, mSurfaceChangedFinish, mSurfaceChangedCount);
	}
	
	/**
     * Get width for video
     * 
     * @return    video width
     */ 
	public int GetVideoWidth ()
	{
		return mWidthVideo;
	}
	
	/**
     * Get height for video
     * 
     * @return    video height
     */ 
	public int GetVideoHeight ()
	{
		return mHeightVideo;
	}
	
	/**
     * Get audio sample rate
     * 
     * @return    audio sample rate
     */ 
	public int GetAudioSampleRate ()
	{
		return mSampleRate;
	}
	
	/**
     * Get audio channels number
     * 
     * @return    audio channels number
     */
	public int GetAudioChannels ()
	{
		return mChannels;
	}	

        /**
         *@deprecated hide this API from document for now
         */
	public int GetAudioBitPerSample()
	{
		return mSampleBit;
	}	

        /**
         *@deprecated hide this API from document for now
         */
	public int GetAudioRenderFormat()
	{
		return mAudioRenderFormat;
	}	
	
	/**
	 * Get Sub title information, include closed caption and subtitle(srt,smi)
	 * 
	 * @param nTimeStamp 	Current time stamp
	 *
	 * @return 	Parcel of {@link com.visualon.OSMPSubTitle.voSubTitleManager.voSubtitleInfo} .
	 */
	public Parcel GetSubTitleSample( int nTimeStamp)
	{
		return (Parcel)nativeGetSubTitleSample(mNativeContext,  nTimeStamp);
	}
	
	private void osmpCallBack(int nID, int param1, int param2, Object arg0)
	{
//		voLog.v(TAG, "osmpCallBack, nID = " + nID);
		if(nID == voOSType.VOOSMP_CB_SEI_INFO){
			if(arg0 != null){
				try {
					if(param1 == voOSType.VOOSMP_SEI_EVENT_FLAG.VOOSMP_FLAG_SEI_EVENT_PIC_TIMING.getValue() ){
						voOSSEIPicTimingImpl impl = voOSSEIPicTimingImpl.parse((int[])arg0);
						arg0 = impl;
						voLog.v(TAG, "osmpCallBack, VOOSMP_CB_SEI_INFO, VOOSMP_FLAG_SEI_EVENT_PIC_TIMING ok");
					}
					if(param1 == voOSType.VOOSMP_SEI_EVENT_FLAG.VOOSMP_FLAG_SEI_USER_DATA_UNREGISTERED.getValue() ){
						voOSSEIUserDataImpl impl = new voOSSEIUserDataImpl();
						impl.parse((Parcel)arg0);
						arg0 = impl;
						voLog.v(TAG, "osmpCallBack, VOOSMP_CB_SEI_INFO, VOOSMP_FLAG_SEI_USER_DATA_UNREGISTERED ok");
					}
				} catch (Exception e) {
					// TODO Auto-generated catch block
					//e.printStackTrace();
					voLog.v(TAG, "osmpCallBack, VOOSMP_CB_SEI_INFO arg0 is not Parcel object");
				}
			}
		}else if(nID == voOSType.VOOSMP_CB_PCM_OUTPUT)
		{
			voLog.v(TAG, "osmpCallBack, VOOSMP_CB_PCM_OUTPUT   " + arg0);
			if(arg0 != null){
				try {
				Parcel parc = (Parcel)arg0;
			    parc.setDataPosition(0);
			    voOSPCMBufferImpl impl = new voOSPCMBufferImpl();
			    impl.parse(parc);
			    param1= (int)impl.getTimestamp();
			    param2 = impl.getBufferSize();
			    arg0 = impl.getBuffer();
				} catch (Exception e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
					voLog.v(TAG, "osmpCallBack, VOOSMP_CB_PCM_OUTPUT arg0 is not Parcel object");
				}
			    voLog.v(TAG, "osmpCallBack, VOOSMP_CB_PCM_OUTPUT ok");
			    }
		}
			
		Message m = mEventHandler.obtainMessage(nID, param1, param2, arg0);
		mEventHandler.sendMessage(m);
		
	}
	
	
	/**
     * Update aspect ratio of video display
     * 
     * @param arWidth   width of Aspect Ratio
     * @param arHeight  height of Aspect Ratio
     */
	public void updateVideoAspectRatio(int arWidth, int arHeight)
	{
		if(arWidth == 0 || arHeight == 0 || (!mbKeepAspectRatio) )
			return;
		
		if(mSurfaceView == null)
			return;
		
		LayoutParams lp = mSurfaceView.getLayoutParams();

		int nMaxOutW = mScreenWidth;
		int nMaxOutH = mScreenHeight;
		int w = 0, h = 0;

		//nMaxOutW &= ~0x7;
		//nMaxOutH &= ~0x1;

		if (nMaxOutW * arHeight > arWidth * nMaxOutH)
		{
			h = nMaxOutH;
			w = nMaxOutH * arWidth / arHeight;
		}
		else
		{
			w = nMaxOutW;
			h = nMaxOutW * arHeight / arWidth;
		}
		//w &= ~0x7;
		//h &= ~0x3;

		lp.width = w;
		lp.height = h;
		
		if(ccMan!=null){
			//ccMan.setXYRate(lp.width/(float)lp.height);
			Float fl = new Float(lp.width/(float)lp.height);
			Message m = mEventHandler.obtainMessage(messageCCSetXYRate, 0, 0, fl);//local file
			mEventHandler.sendMessage(m);
		}

		mSurfaceView.setLayoutParams(lp);
		
		voLog.v(TAG, String.format("VOOSMP_ASPECT_RATIO on video size changed, width=%d, height=%d", lp.width , lp.height));
	}
	
	/**
	 * Get Subtitle Language Information
	 * 
	 * @return Subtitle Language Information. 
	 * 
	 */
	
	public List<voOSSubtitleLanguage> GetSubtitleLanguageInfo()
	{
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return null;
		}
		int nCount;
		
		nCount = nativeGetSubtitleLanguageCount(mNativeContext);
		
		voLog.v(TAG, "Language count is %d.", nCount);
		List<voOSSubtitleLanguage> language = new ArrayList<voOSSubtitleLanguage>();
		for (int i = 0; i< nCount; i++)
		{
			language.add((voOSSubtitleLanguageImpl)nativeGetSubtitleLanguageInfo(mNativeContext, i));
		}
		
		return language;
	}
	
	/**
	 * Select Subtitle Language
	 * 
	 * @param nIndex
	 * @return Success return {@link voOSType#VOOSMP_ERR_None}, error return error code.
	 */
	public int SelectSubtitleLanguage(int nIndex)
	{
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return voOSType.VOOSMP_ERR_Uninitialize;
		}
		
		return nativeSelectSubtitleLanguage(mNativeContext, nIndex);
	}
	
	public void SetDisplaySize(int width, int height)
	{
		mScreenWidth = width;
		mScreenHeight = height;
		
		if (mWidthVideo != 0 && mHeightVideo != 0)
		{
			if (mZoomMode == 0)
			{
				if (mfAspectRatio > 0.01)
		        {
		        	updateVideoAspectRatio((int)(mfAspectRatio*100), 100);
		        }else
		        {
		        	updateVideoAspectRatio(mWidthVideo, mHeightVideo);
		        }
			}else
			{
				if (mZoomMode == voOSType.VOOSMP_ZM_PANSCAN || mZoomMode == voOSType.VOOSMP_ZM_ZOOMIN || mZoomMode == voOSType.VOOSMP_ZM_FITWINDOW)
				{
					startPanScan();
				}else if(mZoomMode == voOSType.VOOSMP_ZM_ORIGINAL)
				{
					endPanScan();
				}
			}
		}
	}
	private boolean closeTimer()
	{
		if(timerTask!=null)
			timerTask.cancel();
		timerTask = null;
		if(timer!=null)
			timer.cancel();
		timer = null;
		return true;
	}
	private boolean startTimer()
	{
		closeTimer();
		timerTask= new TimerTask()
		{
			public void run()
			{
				mEventHandler.sendEmptyMessage(voOSType.VOOSMP_CB_ClosedCaptionData);
			}
		};
		
		if(timer == null)
		{
			timer = new Timer();
		}
		
		timer.schedule(timerTask, 300, 100);
		
		voLog.v(TAG, "Start ClosedCaption!");
		return true;
		
	}
	
	public EventHandler GetEventHandler()
	{
		return mEventHandler;
	}
	
	public boolean IsHardwareRender()
	{
		if (mbOMXAL||mbIOMX)
			return true;
		else
			return false;
	}
	
	/**
	 * Get SEI Timing Information
	 * 
	 * @param subID refer to {@link com.visualon.OSMPUtils.voOSType.VOOSMP_SEI_EVENT_FLAG}, the type for getting SEI information, if subID is {@link com.visualon.OSMPUtils.voOSType.VOOSMP_SEI_EVENT_FLAG#VOOSMP_FLAG_SEI_EVENT_PIC_TIMING}, it will return {@link com.visualon.OSMPUtils.voOSSEIPicTiming}
	 *
	 * @param timeStamp  the time stamp for getting SEI information
	 * 
	 * @return if subID is {@link com.visualon.OSMPUtils.voOSType.VOOSMP_SEI_EVENT_FLAG#VOOSMP_FLAG_SEI_EVENT_PIC_TIMING}, SEI Timing Information object {@link com.visualon.OSMPUtils.voOSSEIPicTiming}, else return null.
	 */
	public Object GetSEITimingInfo(voOSType.VOOSMP_SEI_EVENT_FLAG subID, long timeStamp)
	{
		if(subID == voOSType.VOOSMP_SEI_EVENT_FLAG.VOOSMP_FLAG_SEI_EVENT_PIC_TIMING)
		{
			Object obj = nativeGetSEITimingInfo(mNativeContext, subID.getValue(), timeStamp);
			
			voOSSEIPicTimingImpl impl = voOSSEIPicTimingImpl.parse((int[])obj);
			return impl;
		}
		
		return null;
	}
	
	private void startPanScan()
	{
		int w = 0, h= 0;
		int width = mWidthVideo, height= mHeightVideo;
		voOSRect rect = null;
		
		if (mZoomMode == voOSType.VOOSMP_ZM_PANSCAN)
		{
			if (mfAspectRatio > 0.01)
				width = (int) (height * mfAspectRatio);
			
			if (mScreenWidth * height > mScreenHeight * width)
			{
				w = mWidthVideo;
			  	h = width * mScreenHeight /mScreenWidth;
			  	h &= ~0x3;
			}else
			{
			  	h = mHeightVideo;
			  	w = h * mScreenWidth / mScreenHeight;
			  	if (mfAspectRatio > 0.01)
			  		w = w * mWidthVideo/ width;
			  	
			  	w &= ~0x7;
			}
			rect = new voOSRectImpl(0, 0, w, h);
		}else if (mZoomMode == voOSType.VOOSMP_ZM_ZOOMIN)
		{
			rect = mZoomInRect;
		}else if (mZoomMode == voOSType.VOOSMP_ZM_FITWINDOW)
		{
			ViewGroup.LayoutParams lp = mSurfaceView.getLayoutParams();
			lp.width = mScreenWidth;
			lp.height = mScreenHeight;
			mSurfaceView.setLayoutParams(lp);
			voLog.i(TAG, "Pan & Scan, Fit Window. screen window width is %d, height is %d", w, h);
			return;
		}
		
		nativeSetParam(mNativeContext, voOSType.VOOSMP_PID_ZOOM_MODE, mZoomMode);
		
		int[] intArray = new int[4];
		intArray[0] = rect.Left();
		intArray[1] = rect.Top();
		intArray[2] = rect.Right();
		intArray[3] = rect.Bottom();
		nativeSetParam(mNativeContext, voOSType.VOOSMP_PID_DRAW_RECT, intArray);
		mSurfaceHolder.setFixedSize(w, h);
		
		ViewGroup.LayoutParams lp = mSurfaceView.getLayoutParams();
		lp.width = mScreenWidth;
		lp.height = mScreenHeight;
		mSurfaceView.setLayoutParams(lp);
		voLog.i(TAG, "Pan & Scan, width is %d, height is %d", w, h);
	}
	
	private void endPanScan()
	{
		nativeSetParam(mNativeContext, voOSType.VOOSMP_PID_ZOOM_MODE, mZoomMode);
        voOSRect rect = new voOSRectImpl(0, 0, mWidthVideo, mHeightVideo);
        
        int[] intArray = new int[4];
		intArray[0] = rect.Left();
		intArray[1] = rect.Top();
		intArray[2] = rect.Right();
		intArray[3] = rect.Bottom();
        nativeSetParam(mNativeContext, voOSType.VOOSMP_PID_DRAW_RECT, intArray);
        mSurfaceHolder.setFixedSize(mWidthVideo, mHeightVideo);
        
        if (mfAspectRatio > 0.01)
        {
        	updateVideoAspectRatio((int)(mfAspectRatio*100), 100);
        }else
        {
        	updateVideoAspectRatio(mWidthVideo, mHeightVideo);
        }

        voLog.i(TAG, "Pan & Scan END, width is %d, height is %d", mWidthVideo, mHeightVideo);
   
	}
	
	private void checkOrientationAndScreenSize()
	{
		if ((mContext.getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE)
				&& (mScreenWidth < mScreenHeight))
		{
			int temp;
			temp = mScreenWidth;
			mScreenWidth = mScreenHeight;
			mScreenHeight = temp;
			voLog.e(TAG, "Exchange screen width and height, new width is %d, new height is %d .",
					mScreenWidth, mScreenHeight);
			return;
		}
	}
	
	static
	{
	    try{
	        System.loadLibrary("vodl");
	        System.loadLibrary("voOSEng_OSMP");
				mJNILoaded = true;
	        }catch(UnsatisfiedLinkError e ){
			mJNILoaded = false;
			e.printStackTrace();
		}
	}
	
	private static void loadJNI(String libPath)
	{
		String[] libs = {"vodl","voOSEng_OSMP"};
		try {
			for (int i = 0; i < libs.length; i++) {
				System.load(libPath + libs[i]);
			}
			mJNILoaded = true; 
		} catch (Exception   e) {
			
			mJNILoaded = false;
			e.printStackTrace();
			
			// TODO: handle exception
		}
	}
	
	private	native long nativeCreate(Object player, String apkPath, long lPlayerType, long initParam,long lFlag);
	private native long nativeDestroy(int context);
	private native long nativeOpen(int context, Object source, long flag);
	private native long nativeRun(int context);
	private native long nativePause(int context);
	private native long nativeStop(int context);
	private native long nativeClose(int context);
	private native long nativeSetPos(int context, long pos);	
	private native long nativeGetPos(int context);
	private native long nativeGetVideoData (int context, byte[] data);	
	private native long nativeGetAudioData (int context, byte[] data);	
	private native long nativeSetParam(int context, long id, Object param);
	private native Object nativeGetParam(int context, long id);
	private	native long nativeSetSurface(int context);
	private native Object nativeGetSubTitleSample(int context, int nTimeStamp);
	private native int nativeGetSubtitleLanguageCount(int context);
	private native Object nativeGetSubtitleLanguageInfo(int context, int nIndex);
	private native int nativeSelectSubtitleLanguage(int context, int nIndex);
	private native Object nativeGetSEITimingInfo(int context, int subID, long timeStamp);
}
