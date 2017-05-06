
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
 * @file voOSBasePlayer.java
 * interface of player for playing media.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/

package com.visualon.OSMPBasePlayer;


import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Timer;
import java.util.TimerTask;
import java.util.TreeMap;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.apache.http.util.ByteArrayBuffer;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import android.app.Activity;
import android.content.Context;
import android.content.res.Configuration;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.Parcel;
import android.provider.Settings;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.RelativeLayout;

import com.visualon.OSMPDataSource.voOSDataSource;
import com.visualon.OSMPEngine.voOnStreamSDK;
import com.visualon.OSMPUtils.voLog;
import com.visualon.OSMPUtils.voOSCPUInfo;
import com.visualon.OSMPUtils.voOSChunkInfo;
import com.visualon.OSMPUtils.voOSChunkSample;
import com.visualon.OSMPUtils.voOSDVRInfo;
import com.visualon.OSMPUtils.voOSHTTPHeader;
import com.visualon.OSMPUtils.voOSInitParam;
import com.visualon.OSMPUtils.voOSOption;
import com.visualon.OSMPUtils.voOSPerformanceData;
import com.visualon.OSMPUtils.voOSPerformanceDataImpl;
import com.visualon.OSMPUtils.voOSPreviewSubtitleInfo;
import com.visualon.OSMPUtils.voOSProgramInfo;
import com.visualon.OSMPUtils.voOSRTSPStatus;
import com.visualon.OSMPUtils.voOSSubtitleLanguage;
import com.visualon.OSMPUtils.voOSTimedTag;
import com.visualon.OSMPUtils.voOSTrackInfo;
import com.visualon.OSMPUtils.voOSType;
import com.visualon.OSMPUtils.voOSType.VOOSMP_SOURCE_STREAMTYPE;
import com.visualon.OSMPUtils.voOSVerificationInfo;
import com.visualon.widget.ClosedCaptionManager;



/**
 * OSMP+ BasePlayer class
 * 
 * 
 * @author zhang_yi
 *
 */
public class voOSBasePlayer implements voOnStreamSDK.onEventListener, voOSDataSource.onEventListener, 
								voOSDataSource.onRequestListener {
	private static final String TAG = "@@@OSMPBasePlayer";

	private Context 			mContext;
	private voOnStreamSDK		mEngine;
	private voOSDataSource    	mSource;
	private onEventListener		mEventListener;
	public  onRequestListener 	mRequestListener;
	
	private String				mPackageName;
	private List<voOSOption> 	mOptions;
	private Map<Integer, Object> 	mParamDict;
	private boolean 			mSetViewFlag = false;
	private boolean 			mSetDisplaySizeFlag = false;
	private boolean				mbOMXAL = false;
	private boolean				mbAsyncOpen = false;
	private int					mCheckSubtitleTimes = 0;
	//private float				mRate;
	
	//for subttile display
	private boolean					closeCaptionOutput = false;
	private boolean					enableInnerCloseCaption = false;
	private	Timer 					timer = null;
	private TimerTask 				timerTask= null;//new TimerTask()
	private ClosedCaptionManager 	ccMan = null;
	private	boolean					mSubtitleInEngine = false;
	private SurfaceView				mSurfaceView;
    private EventHandler 			mEventHandler;
	private String					mSourceUrl;
	private boolean 				subtitleFileNameSet = false;

	private static final int 		messageDestroyCC_UI		= 0xf0f00002;
	private static final int 		messageShowCC_UI		= 0xf0f00003;
	private static final int 		messageCustomTag		= 0xf0f00004;

	private static final int 		messageCCEnable			= 0xf0f00005;
	private static final int 		messageCCMetadataArrive	= 0xf0f00006;
	private static final int 		messageCCSetXYRate		= 0xf0f00007;
	private static final int 		messageCCClearWidget	= 0xf0f00008;
	
	private boolean 				metadata_Arrive = false;
	private int						openedTimes = 0;
	private int                     mTrackType;
	
	private boolean                 mRunning = false;
	
	//members to restore to old aspect ratio
	private int 				 	m_nWidthOfAspectRatio = 0;
	private int 				 	m_nHeightOfAspectRatio = 0;
	private int					 	m_nVideoWidth        = 0;      // Video width
	private int						m_nVideoHeight       = 0;		// video height
	private Integer 				m_nAspectRationType = null;
	
	//for Customer Tag 
	private voCustomerTagWrap		customerTagWrap			= null;
	private voFrameScrubbing		frameScrubbing			= null;
	private voOSDRM					mDRM					= null;
	private int                     mRenderType             = voOSType.VOOSMP_RENDER_TYPE_JAVA;                    
	private int 					mOffsetTime				= 0;
	private boolean                 isSourceInit           = false;
	/**
     * Interface for event process. The client who wants to receive event from voOSBasePlayer shall implement the interface. 
     */
	
	public interface onEventListener
	{

		/**
		 * Interface for event process. The client who wants to use voOSBasePlayer shall implement the interface.
		 * 
		 * 
		 * @param id		type of the event
		 * <ul>
         * <li>{@link voOSType#VOOSMP_SRC_CB_Connecting}
         * <li>{@link voOSType#VOOSMP_SRC_CB_Connection_Finished}
         * <li>{@link voOSType#VOOSMP_SRC_CB_Connection_Timeout}
         * <li>{@link voOSType#VOOSMP_SRC_CB_Connection_Loss}
         * <li>{@link voOSType#VOOSMP_SRC_CB_Download_Status}
         * <li>{@link voOSType#VOOSMP_SRC_CB_Connection_Fail}
         * <li>{@link voOSType#VOOSMP_SRC_CB_Download_Fail}
         * <li>{@link voOSType#VOOSMP_SRC_CB_DRM_Fail}
         * <li>{@link voOSType#VOOSMP_SRC_CB_Playlist_Parse_Err}
         * <li>{@link voOSType#VOOSMP_SRC_CB_Connection_Rejected}
         * <li>{@link voOSType#VOOSMP_SRC_CB_BA_Happened}
		 * <li>{@link voOSType#VOOSMP_SRC_CB_DRM_Not_Secure}
		 * <li>{@link voOSType#VOOSMP_SRC_CB_DRM_AV_Out_Fail}
		 * <li>{@link voOSType#VOOSMP_SRC_CB_Download_Fail_Waiting_Recover}
		 * <li>{@link voOSType#VOOSMP_SRC_CB_Download_Fail_Recover_Success}
		 * <li>{@link voOSType#VOOSMP_SRC_CB_Open_Finished}
		 * <li>{@link voOSType#VOOSMP_SRC_CB_Customer_Tag}
		 * <li>{@link voOSType#VOOSMP_SRC_CB_Adaptive_Streaming_Info}
		 * <li>{@link voOSType#VOOSMP_SRC_CB_Adaptive_Streaming_Error}
		 * <li>{@link voOSType#VOOSMP_SRC_CB_Adaptive_Stream_Warning}
		 * <li>{@link voOSType#VOOSMP_SRC_CB_RTSP_Error}
		 * <li>{@link voOSType#VOOSMP_SRC_CB_Seek_Complete}
	     * <li>{@link voOSType#VOOSMP_SRC_CB_Program_Changed}
	     * <li>{@link voOSType#VOOSMP_SRC_CB_Program_Reset}
	     * <li>{@link voOSType#VOOSMP_SRC_CB_UPDATE_URL_COMPLETE}
         * </ul>
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
		 * </ul>
		 * <ul>
		 * <li>{@link voOSType#VOOSMP_CB_BLUETOOTHHANDSET}
		 * <li>{@link voOSType#VOOSMP_CB_ClosedCaptionData}
         * </ul>
		 * @param param1 first parameter code, specific to the event if needs. Typically implementation dependent.
		 * @param param2 second parameter code, specific to the event if needs. Typically implementation dependent.
		 * @param obj third parameter code, specific to the event if needs. Typically implementation dependent.
		 * @return {@link voOSType#VOOSMP_ERR_None} if no error.
		 */
		public int onEvent(int id, int param1, int param2, Object obj);
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
	
	
	/**
	 * Interface for request process. It is for HTTP verification only now. 
	 */
	public interface onRequestListener
	{
		/**
		 * Interface for request process. It is for HTTP verification only now.
		 * 
		 * 
		 * @param id		type of the event, refer to
		 * <ul> 
		 * <li>{@link voOSType#VOOSMP_SRC_CB_Customer_Tag}
		 * <li>{@link voOSType#VOOSMP_SRC_CB_Adaptive_Streaming_Info}
		 * <li>{@link voOSType#VOOSMP_SRC_CB_Adaptive_Stream_Warning}
		 * <li>{@link voOSType#VOOSMP_SRC_CB_Authentication_Request}
		 * <li>{@link voOSType#VOOSMP_SRC_CB_IO_HTTP_Download_Failed}
         * <li>{@link voOSType#VOOSMP_SRC_CB_IO_HTTP_Start_Download}
         * </ul>
		 * @param param1 	first parameter code, specific to the event if needs. Typically implementation dependent.
		 * @param param2 	second parameter code, specific to the event if needs. Typically implementation dependent.
		 * @param obj 		third parameter code, specific to the event if needs. Typically implementation dependent.
		 * @return {@link voOSType#VOOSMP_ERR_None} if no error.
		 */
		public int onRequest(int id, int param1, int param2, Object obj);
	}

	
	/**
     * Register a listener for processing request.
     *
     * @param listener for processing request callback
     */
	public void setRequestListener(onRequestListener listener)
	{
		mRequestListener = listener;
	}
	
    private class EventHandler extends Handler
    {
        public EventHandler( Looper looper) 
        {
            super(looper);
        }

        public void handleMessage(Message msg) 
        {      	
            if (mEngine == null)
            {
                voLog.w(TAG, "voOSBasePlayer went away with unhandled events");
                return;
            }
            if(msg.what == messageCCEnable){
            	setCommonCCParser(msg.arg1, msg.obj);
            	return;
            }
            if(msg.what == messageCCMetadataArrive){
	        	if(ccMan != null){
	        		ccMan.setData((Parcel)msg.obj, false);
	        	}
            	return;
            }
            if(msg.what == messageCCSetXYRate){
 				ccMan.setXYRate((Float)msg.obj);
            	return;
            }
            if(msg.what == messageCCClearWidget){
        		if(ccMan != null)
        			ccMan.clearWidget();
            	return;
            }
            
            if(msg.what == messageDestroyCC_UI)
            {
        		if(ccMan != null)
        		{
        			ccMan.show(false);
        			ccMan.clearWidget();
        			ccMan = null;
        		}
                return;
            }
            
            if(msg.what == messageCustomTag)
            {
            	if(customerTagWrap == null)
                    return;
	    		int nCurr = 0;
				nCurr = GetPos();
	    		
	    		voOSTimedTag info = customerTagWrap.check(nCurr);
	    		if(info!=null){
	    			
		    		voLog.v(TAG, "mTimedTagList onRequest sent, size is %d, timestamp is %d, current pos is %d", info.Size(), info.TimeStamp(), nCurr);
	    			
	    			if (mRequestListener != null)
	    				mRequestListener.onRequest(voOSType.VOOSMP_SRC_CB_Customer_Tag, voOSType.VOOSMP_SRC_CUSTOMERTAGID_TIMEDTAG, 0, info);
	    		}
                return;
            }else if(msg.what == messageShowCC_UI)
            {
        		if(ccMan != null)
        		{
	                Integer status = (Integer)mEngine.GetParam(voOSType.VOOSMP_PID_STATUS);
	                if (status.intValue() == voOSType.VOOSMP_STATUS_RUNNING){
        				ccMan.clearWidget();
        			}
        			ccMan.show(msg.arg1==1?true:false);
        		}
                return;
            }else if(msg.what == voOSType.VOOSMP_CB_ClosedCaptionData)
            {
            	if(mSubtitleInEngine || openedTimes <= 0)
            		return;
            	if(mCheckSubtitleTimes>0)
            		mCheckSubtitleTimes--;
	    		int nCurr = GetPos();
	    		if(mOffsetTime != 0)
	    		{
//	    			voLog.d(TAG, "AD Manager mOffsetTime = %d", mOffsetTime);
	    			if(mOffsetTime == -1)
	    			{
	    				return;
	    			}
	    			nCurr = mOffsetTime;
	    		}
	    		Parcel parc = (Parcel) GetSample(voOSType.VOOSMP_SOURCE_STREAMTYPE.VOOSMP_SS_SUBTITLE.ordinal(),nCurr);
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
	    		  if(ccMan == null || !IsInnerCloseCaptionEnable()) return;
	    		  
	    		  if(parc!=null)
	    		  {
	                  Integer status = (Integer)mEngine.GetParam(voOSType.VOOSMP_PID_STATUS);
	                  if (status.intValue() != voOSType.VOOSMP_STATUS_RUNNING && ccMan.isEmptySubtitleInfo() && mCheckSubtitleTimes==0){
	          			//voLog.v(TAG,"VOOSMP_CB_CloseCaptionData CloseCaption = nCurr = %d, parc = empty", nCurr); 
	                  	return;
	                  }

	    			  ccMan.setData(parc, true);
//	    			  if(mSourceOutputCCData)
//		    			  if(ccMan.isEmptySubtitleInfo())
//		    				  mSourceOutputCCData = false;
//	    			  voLog.v(TAG, "CloseCaption available, nCurr = %d", nCurr); 
	    		  }
	    		  else
	    		  {
//	    			  voLog.v(TAG, "CloseCaption parc==null 3 nCurr = %d", nCurr); 
	    		  }
	    		  ccMan.checkViewShowStatus(nCurr);
	    		  return;
            		
            	}
            	
            }
            
			if (mEventListener != null)
        		mEventListener.onEvent (msg.what, msg.arg1, msg.arg2, msg.obj);
        }
    }
	/**
	 * Construct
	 * 
	 * 
	 */
	public voOSBasePlayer() 
	{
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
        
		mEngine = null;
		mSource = null;
		mContext = null;
		mEventListener = null;
		mRequestListener = null;
		mParamDict = null;
		mTrackType = -1;
	}
	
	

	/**
	* Initialize PLAYER instance.
	* This function must be called at first for a session.
	*
	* @param	context		Current context	
    * @param    packageName	Full path name of package such as /data/data/packagename/lib/
    * @param	options		List of Player options. This parameter is deprecated.
	* @param	nPlayerType	Type of media framework, refer to {@link voOSType#VOOSMP_VOME2_PLAYER}, {@link voOSType#VOOSMP_OMXAL_PLAYER}.
	* @param	nInitParam	Initialization param, depending on nInitParamFlag,
	* <ul>
    * <li>if nInitParamFlag == {@link voOSType#VOOSMP_FLAG_INIT_NOUSE} then nInitParam should be 0;
    * <li>if nInitParamFlag == {@link voOSType#VOOSMP_FLAG_INIT_LIBOP} then nInitParam should be pointer of thirdly Library OP;
    *</ul> 
	* @param	nInitParamFlag		{@link voOSType#VOOSMP_FLAG_INIT_NOUSE} or {@link voOSType#VOOSMP_FLAG_INIT_LIBOP}
	* @return	{@link voOSType#VOOSMP_ERR_None} if succeeded 
	*/
	public int Init (Context context, String packageName,  List<voOSOption> options, int nPlayerType, int nInitParam, int nInitParamFlag)
	{
		voLog.v(TAG, "voOSBasePlayer Init -");
		if(ccMan!=null){
			ccMan.clearWidget();
			ccMan = null;
		}
		openedTimes = 0;
		if(mEngine!=null || (context == null)|| (packageName == null))
			return voOSType.VOOSMP_ERR_Status;
		
		mContext = context;
		/*
		try {
			InputStream is = context.getAssets().open("version.txt");
			byte b[] = new byte[1024];
			int length = is.read(b);
			String str = new String(b,0, length);
			
			voLog.i(TAG, "****************************************\n   OSMP+V2 SDK version is %s\n****************************************", str);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			voLog.e(TAG, "Don't find version.txt file.");
		}
		*/
		int nRet = 0;
		mPackageName = packageName;
		mOptions = options;
		mEngine = new voOnStreamSDK();
		mSource = new voOSDataSource();
		nRet = mEngine.Init(mContext, packageName, options, nPlayerType, nInitParam, nInitParamFlag);
		if (nPlayerType == voOSType.VOOSMP_VOME2_PLAYER)
		{
			mbOMXAL = false;
		}else
		{
			mbOMXAL = true;
		}
			
		mEngine.setEventListener(this);
		Integer param = new Integer(0);
		mEngine.SetParam(voOSType.VOOSMP_PID_SUBTITLE_IN_ENGINE, param);
		
		voLog.v(TAG, "voOSBasePlayer Init +, return is %d", nRet);
		
		File fPathName = new File(mPackageName + "libvoDRMJNI_OSMP.so");
				   
		if (fPathName.exists()) 
		{
			voLog.i(TAG, mPackageName + "libvoDRMJNI.so" + " is exiest, ready to init voDRM");
			if (mDRM == null)
			{

				try {
					mDRM = new voOSDRM(this, mPackageName, "libvoDRM.so");
					mDRM.init(0);
				} catch (FileNotFoundException e) {
					// TODO Auto-generated catch block
					mDRM = null;
					voLog.e(TAG, "voOSDRM init error!");
				}
				
			}
		}else
			voLog.i(TAG, mPackageName + "libvoDRMJNI.so" + " is not exiest.");
			
		return nRet;
	}
	
	/**
	* Uninitialize PLAYER instance.
	* This function must be called at last for a session.
	* 
	* {@link voOSType#VOOSMP_ERR_None} if succeeded 
	*/
	public int Uninit()
	{
		voLog.v(TAG, "voOSBasePlayer Uninit -");
		mEventListener = null;
		mRequestListener = null;
		mEventHandler.removeCallbacksAndMessages(null);
		openedTimes = 0;
		closeTimer();
		if(customerTagWrap != null)
			customerTagWrap.closeTimer();
		customerTagWrap = null;
		
		if(mEngine!=null)
			mEngine.Uninit();
		if(mSource!=null) {
			mSource.Uninit();
			mSource.Destroy();
		}
		if(mDRM!=null)
			mDRM.uninit();
		
		mEngine = null;
		mSource = null;
		mDRM = null;
		
		mParamDict = null;
		mSetViewFlag = false;
		mSetDisplaySizeFlag = false;
		subtitleFileNameSet = false;
		
		voLog.v(TAG, "voOSBasePlayer Uninit +");
		return 0;
	}
	
	/**
	* Set a surface view for video
	* 
	* @param  sv 	Current SurfaceView
	* 
	*/
	public void SetView (SurfaceView sv) 
	{
	    voLog.v(TAG, "setDisplay suface is " + sv);
		if(mEngine!=null)
		{
			mSetViewFlag = true;
			mSurfaceView = sv;
			mEngine.SetView(sv);
		}
	}
	
	/**
	* Set a surface holder for video
	* 
	* This function only used for LiveWallPaper, if you call the function, you shouldn't call SetView function. 
	* 
	* @param  sh 	Current Surface Holder
	* 
	*/
	public void SetSurfaceHolder (SurfaceHolder sh) 
	{
	    voLog.v(TAG, "setSurfaceHolder is " + sh);
		if(mEngine!=null)
		{
			mSetViewFlag = true;
			mSurfaceView = null;
			mEngine.SetSurfaceHolder(sh);
		}
	}
	
	/**
	 * Set display dimension for video
	 * Display area is center horizontally.
	 * 
	 * @param width 	display area width
	 * @param height 	display area height
	 */
	public void SetDisplaySize(int width, int height)
	{
		if(mEngine!=null)
		{
			mSetDisplaySizeFlag = true;
			//width &= ~0x7;
			//if(width<8)
			//	width = 8;
			mEngine.SetDisplaySize(width, height);
		}
	}
	
	/**
	 * Update aspect ratio of video display
	    This method will be removed, please use SetParam {@link voOSType#VOOSMP_PID_VIDEO_ASPECT_RATIO}
	 * 
	 * @param arWidth 	width of Aspect Ratio
	 * @param arHeight	height of Aspect Ratio
	 */
	public void updateVideoAspectRatio(int arWidth, int arHeight)
	{
		if(mEngine!=null)
		{
			int [] widthHeight = new int[2];
			widthHeight[0]=arWidth;
			widthHeight[1]=arHeight;
	        float f = GetAspectWidthHeight(arWidth, arHeight, widthHeight);
//	        if(arWidth == 0 && arHeight ==0){
//				if(m_nVideoWidth!=0 && m_nVideoHeight!=0){
//					nWidth = m_nVideoWidth;
//					nHeight = m_nVideoHeight;
//				}
//		        if(m_nWidthOfAspectRatio !=0 && this.m_nHeightOfAspectRatio !=0){
//		        	nWidth = m_nWidthOfAspectRatio;
//		        	nHeight = m_nHeightOfAspectRatio;
//		        }
//	        }
//			if(arWidth == 0 && arHeight == 0 && m_nAspectRationType!=null){
//				switch(m_nAspectRationType){
//				case voOSType.VOOSMP_RATIO_ORIGINAL:
//					if(m_nVideoWidth!=0 && m_nVideoHeight!=0){
//						nWidth = m_nVideoWidth;
//						nHeight = m_nVideoHeight;
//					}
//					break;
//				case voOSType.VOOSMP_RATIO_AUTO:
//					break;
//				case voOSType.VOOSMP_RATIO_11:
//					nWidth = 1;
//					nHeight = 1;
//					break;
//				case voOSType.VOOSMP_RATIO_43:
//					nWidth = 4;
//					nHeight = 3;
//					break;
//				case voOSType.VOOSMP_RATIO_169:
//					nWidth = 16;
//					nHeight = 9;
//					break;
//				case voOSType.VOOSMP_RATIO_21:
//					nWidth = 2;
//					nHeight = 1;
//					break;
//				case voOSType.VOOSMP_RATIO_2331:
//					nWidth = 233;
//					nHeight = 100;
//					break;
//				}
//				
//			}
			if(mSurfaceView!=null)
			{
				if((ccMan!=null) )
				{
					Message m = mEventHandler.obtainMessage(messageCCSetXYRate, 0, 0, f);//local file
					mEventHandler.sendMessage(m);
					voLog.v(TAG,"setXYRate updateVideoAspectRatio a1= %d; a2=%d ",arWidth,arWidth);         //this.mSubtitleInfo = subtitleInfo;
				}
			}
	        mEngine.updateVideoAspectRatio(widthHeight[0], widthHeight[1]);
		}
	}
	
	private float GetAspectWidthHeight(int arWidth, int arHeight, int [] widthHeight){
		float f = 1.333f;
		if(mEngine!=null)
		{
	        int nWidth = arWidth;
	        int nHeight = arHeight;
	        if(arWidth == 0 && arHeight ==0){
				if(m_nVideoWidth!=0 && m_nVideoHeight!=0){
					nWidth = m_nVideoWidth;
					nHeight = m_nVideoHeight;
				}
		        if(m_nWidthOfAspectRatio !=0 && this.m_nHeightOfAspectRatio !=0){
		        	nWidth = m_nWidthOfAspectRatio;
		        	nHeight = m_nHeightOfAspectRatio;
		        }
	        }
			if(arWidth == 0 && arHeight == 0 && m_nAspectRationType!=null){
				switch(m_nAspectRationType){
				case voOSType.VOOSMP_RATIO_ORIGINAL:
					if(m_nVideoWidth!=0 && m_nVideoHeight!=0){
						nWidth = m_nVideoWidth;
						nHeight = m_nVideoHeight;
					}
					break;
				case voOSType.VOOSMP_RATIO_AUTO:
					break;
				case voOSType.VOOSMP_RATIO_11:
					nWidth = 1;
					nHeight = 1;
					break;
				case voOSType.VOOSMP_RATIO_43:
					nWidth = 4;
					nHeight = 3;
					break;
				case voOSType.VOOSMP_RATIO_169:
					nWidth = 16;
					nHeight = 9;
					break;
				case voOSType.VOOSMP_RATIO_21:
					nWidth = 2;
					nHeight = 1;
					break;
				case voOSType.VOOSMP_RATIO_2331:
					nWidth = 233;
					nHeight = 100;
					break;
				}
				
			}
			if(widthHeight!=null){
				widthHeight[0] = nWidth;
				widthHeight[1] = nHeight;
			}
			if(nHeight>0)
				f = nWidth/(float)nHeight;
		}
		return f;
	}
    
	/**
	 * Open media source
	 * 
	 * @param pSource			Source file description, should be an url or fd etc.
	 * @param nSourceFlag		The flag for open source,refer to 
	 * <ul>
	 * <li>{@link voOSType#VOOSMP_FLAG_SOURCE_URL}
	 * <li>{@link voOSType#VOOSMP_FLAG_SOURCE_READBUFFER}
	 * <li>{@link voOSType#VOOSMP_FLAG_SOURCE_SENDBUFFER}
	 * <li>{@link voOSType#VOOSMP_FLAG_SOURCE_HANDLE}
	 * <li>{@link voOSType#VOOSMP_FLAG_SOURCE_OPEN_SYNC}
	 * <li>{@link voOSType#VOOSMP_FLAG_SOURCE_OPEN_ASYNC}
	 * </ul>
	 * @param nSourceType		Indicate the source format, usually is {@link voOSType#VOOSMP_SRC_AUTO_DETECT}
	 * @param nInitParam		Initialization param, depending on nInitParamFlag,
	 * <ul>
     * <li>if nInitParamFlag == {@link voOSType#VOOSMP_FLAG_INIT_NOUSE} then nInitParam should be 0;
     * <li>if nInitParamFlag == {@link voOSType#VOOSMP_FLAG_INIT_LIBOP} then nInitParam should be pointer of thirdly Library OP;
     *</ul> 
	 * @param nInitParamFlag	{@link voOSType#VOOSMP_FLAG_INIT_NOUSE} or {@link voOSType#VOOSMP_FLAG_INIT_LIBOP}
	 * @return {@link voOSType#VOOSMP_ERR_None} if Succeeded
	 */
    public int Open ( Object pSource, int nSourceFlag, int nSourceType, int nInitParam, int nInitParamFlag)
    {
    	voLog.v(TAG, "voOSBasePlayer Open -");
		if(mEngine==null)
			return voOSType.VOOSMP_ERR_Uninitialize;
		if(mSetViewFlag==false || mSetDisplaySizeFlag == false)//before open should call SetView, SetDisplaySize firstly
		{
			voLog.e(TAG, "before open should call SetView, SetDisplaySize firstly");
			return voOSType.VOOSMP_ERR_Unknown;
		}
			
		
		if(openedTimes>0){
			voLog.e(TAG, "Do not open again before player is closed.");
			return voOSType.VOOSMP_ERR_Status;
		}
		openedTimes++;
		
		if (pSource == null
				||(!String.class.isInstance(pSource) && ((nSourceFlag & voOSType.VOOSMP_FLAG_SOURCE_URL)>0))
				||(!Integer.class.isInstance(pSource) && ((nSourceFlag & voOSType.VOOSMP_FLAG_SOURCE_URL)==0)))
		{
			voLog.e(TAG, "Param pSource and param nSourceFlag is not match.");
			return voOSType.VOOSMP_ERR_Unknown;
		}

		if(pSource!=null){
			mSourceUrl = pSource.toString();
		}
		
		mTrackType = -1;
		m_nWidthOfAspectRatio = 0;
		m_nHeightOfAspectRatio = 0;
		
		int initParamFlag = 0;
		voOSInitParam initParam = null;
		
		long fileSize = 0;
		int LibOP = 0;
		String IOFileName = null;
		String IOApiName = null;
		
		if(mParamDict != null)
		{
			if(mParamDict.entrySet()!=null)
			{
				Iterator<Entry<Integer,Object>> it = mParamDict.entrySet().iterator();
				while(it!=null && it.hasNext())
				{
					Entry<Integer,Object> ent = it.next();
					if(ent!=null)
					{
						if(ent.getKey() == voOSType.VOOSMP_SRC_PID_INIT_PARAM_ACTUAL_FILE_SIZE)
						{
							fileSize = (Long) ent.getValue();
						}
						
						if(ent.getKey() == voOSType.VOOSMP_SRC_PID_INIT_PARAM_IO_FILE_NAME)
						{
							IOFileName = (String) ent.getValue();
						}
						
						if(ent.getKey() == voOSType.VOOSMP_SRC_PID_INIT_PARAM_IO_API_NAME)
						{
							IOApiName = (String) ent.getValue();
						}
						
					}
				}
			}
		}
		
		if (fileSize != 0)
		{
			initParamFlag = initParamFlag| voOSType.VOOSMP_FLAG_INIT_ACTUAL_FILE_SIZE;
			if (initParam == null)
				initParam = new voOSInitParam();
			initParam.setFileSize(fileSize);
			voLog.i(TAG, "init param is %d, %d", initParamFlag, initParam.getFileSize());
		}
		
		if (IOFileName != null)
		{
			initParamFlag = initParamFlag| voOSType.VOOSMP_FLAG_INIT_IO_FILE_NAME;
			if (initParam == null)
				initParam = new voOSInitParam();
			initParam.setIOFileName(IOFileName);
			voLog.i(TAG, "init param is %d, VOOSMP_SRC_PID_INIT_PARAM_IO_FILE_NAME %s", 
					initParamFlag, initParam.getIOFileName());
		}
		
		if (IOApiName != null)
		{
			initParamFlag = initParamFlag| voOSType.VOOSMP_FLAG_INIT_IO_API_NAME;
			if (initParam == null)
				initParam = new voOSInitParam();
			initParam.setIOApiName(IOApiName);
			voLog.i(TAG, "init param is %d, VOOSMP_SRC_PID_INIT_PARAM_IO_API_NAME %s",
					initParamFlag, initParam.getIOApiName());
		}
		
		
    	int nRet;
		isSourceInit = true;
    	nRet = mSource.Init(mContext, mPackageName, mOptions, pSource, nSourceFlag, nSourceType, initParam, initParamFlag);

    	if (nRet != voOSType.VOOSMP_ERR_None)
    	{
    		voLog.v(TAG, "mSource.Init failed, Error message is 0x" + Integer.toHexString(nRet));
    		return nRet;
    	}
    	
		mSource.setEventListener(this);
		mSource.setRequestListener(this);
		
		if ((nSourceFlag&voOSType.VOOSMP_FLAG_SOURCE_OPEN_ASYNC) > 0)
		{
			mbAsyncOpen = true;
			voLog.i(TAG, "Async open!");
    		if(this.ccMan!=null){
    			ccMan.show(false);
    		}

		}
    	
    	setParamInDict();
    	
    	if (mDRM != null)
    	{
	    	Integer iParam = new Integer(0);
	    	mDRM.getInternalAPI(iParam);
	    	mSource.SetParam(voOSType.VOOSMP_SRC_PID_DRM_CALLBACK_FUNC, iParam);
	    	voLog.i(TAG, "voDRM.getInternalAPI is %d", iParam.intValue());
    	}
    	
    	nRet = mSource.Open();
    	if (nRet != voOSType.VOOSMP_ERR_None)
    	{
    		voLog.v(TAG, "mSource.Open failed, Error message is 0x" + Integer.toHexString(nRet));
    		return nRet;
    	}
    	
    	Object obj = mSource.GetParam(voOSType.VOOSMP_SRC_PID_FUNC_READ_BUF);
    	if (obj == null)
    	{
    		voLog.v(TAG, "GetParam(voOSType.VOOSMP_SRC_PID_FUNC_READ_BUF) Error!" );
    		return voOSType.VOOSMP_SRC_ERR_OPEN_SRC_FAIL;
    	}
    	
    	Integer source = (Integer)obj;
		nRet = mEngine.Open(source, voOSType.VOOSMP_FLAG_SOURCE_READBUFFER);
		if (nRet != voOSType.VOOSMP_ERR_None)
    	{
    		voLog.v(TAG, "mEngine.Open failed, Error message is 0x" + Integer.toHexString(nRet));
    		return nRet;
    	}
		
		voLog.v(TAG, "voOSBasePlayer Open +, return %d", nRet);
	    return nRet;
    }
    
	
    /**
	 * Get sample from the source, only uese to get subtitle and ClosedCaption data.
	 * 
	 * @param nTrackType	The sample type of the stream, it will be audio/video/closed caption, refer to {@link VOOSMP_SOURCE_STREAMTYPE}
	 * @param nTimeStamp 	Current time stamp
	 *
	 * @return 	return value depend on different nTrackType.
	 * 			sample of the stream, all buffer in the sample( include reserve field ) will be mantained by source, they will be available until you call GetSample again
	 *          the return type is depended on the nTrackType, for {@link VOOSMP_SOURCE_STREAMTYPE#VOOSMP_SS_AUDIO} & {@link VOOSMP_SOURCE_STREAMTYPE#VOOSMP_SS_VIDEO} please use VOOSMP_BUFFERTYPE
	 *                                                         for {@link VOOSMP_SOURCE_STREAMTYPE#VOOSMP_SS_SUBTITLE} please use voSubtitleInfo
	 */
	public Object GetSample(int nTrackType, int nTimeStamp)
	{
		if(mSource == null)
			return null;
		
		Object obj = null;
		if (nTrackType == voOSType.VOOSMP_SOURCE_STREAMTYPE.VOOSMP_SS_SUBTITLE.ordinal())
		{
			if(mEngine==null)
			{
				return null;
			}
			obj = mSource.GetSample(nTrackType, nTimeStamp);
			return obj;
			/*
			if(subtitleFileNameSet)
				return obj;
			if(obj == null)
			{
				obj = mEngine.GetSubTitleSample(nTimeStamp);
				return obj;
			}
			else
				return obj;
			*/
		}
	
		return obj;
	}
	

	/**
	 * It retrieve how many program in the live stream
	 * For program, it should be defined as: all streams that share the same angle for video and same language for audio
	 * 
	 * @return  The number of the program
	 * 
	 */
	public int GetProgramCount ()
	{
		if (mSource == null)
			return -1;
		return mSource.GetProgramCount();
	}
	
	/**
	 * Enum the select function name which be called when change bitrate. 
	 * 
	 * 
	 * 
	 * 
	 */
	private enum EnumSelectFunc{
		SelectProgram,
		SelectStream,
		SelectTrack,
		SelectSubtitleLanguage
	}
	
	/**
	 * For fixed the bug of delay when change bitrate.
	 * 
	 * @param enumSelectFunc	Point out which function call this. 
	 * @param param				The param which mSource.SelectXXXX() function need to use.
	 * 
	 * @return  The number of the program
	 * 
	 */
	private int ChangeBitrate(EnumSelectFunc enumSelectFunc,Object param){
		int nCurrentPos = GetPos();
		
		int nRet = 0;
		
		switch(enumSelectFunc){
		case SelectTrack:
			nRet =  mSource.SelectTrack((Integer)param);
			break;
		/*
		case SelectSubtitleLanguage:
			if( subtitleFileNameSet){
				nRet =  mSource.SelectSubtitleLanguage((Integer)param);
			}else{			
				nRet =  mEngine.SelectSubtitleLanguage((Integer)param);
			}
			
			break;
		*/
		}
		
		if(nRet != voOSType.VOOSMP_ERR_None)
		{
			voLog.e(TAG, "Change audio or subtitle, return err %s", Integer.toHexString(nRet));
			return nRet;
		}

		voLog.i(TAG, "Change audio or subtitle, then Seek to %s, audiotracker or subtitle id %d", nCurrentPos, (Integer)param);
		SetPos(nCurrentPos);
		
		return nRet;
	}

	
	/**
	 * Enum all the program info based on stream count
	 * 
	 * @param nProgram	 The program sequence based on the program counts
	 * 
	 * @return {@link voOSProgramInfo}, source will keep the info structure available until you call close
	 * 
	 */
	public Object GetProgramInfo(int nProgram)
	{
		if (mSource == null)
			return null;
		
		return mSource.GetProgramInfo(nProgram);
	}

	
	/**
	 * Get the track info we selected
	 * 
	 * @param nTrackType [in] The sample type of the stream, it will be audio/video/closed caption, refer to {@link VOOSMP_SOURCE_STREAMTYPE}
	 * @return {@link voOSTrackInfo} of the selected track. The trackinfo memory will be mantained in source until you call close
	 * 
	 */
	public Object GetCurTrackInfo(int nTrackType)
	{
		if (mSource == null)
			return null;
		
		return mSource.GetCurTrackInfo(nTrackType);
	}
	
	/**
	 * Select the Program
	 * 
	 * @param 	nProgram [in] Program ID
	 * @return 	{@link voOSType#VOOSMP_ERR_None} if Succeeded
	 */
	public int SelectProgram (int nProgram)
	{
		if (mSource == null)
			return voOSType.VOOSMP_SRC_ERR_SOURCE_UNINITIALIZE;
		
		return mSource.SelectProgram(nProgram);
	}
	
	/**
	 * Select the Stream
	 * 
	 * @param 	nStream [in] Stream ID
	 * @return 	{@link voOSType#VOOSMP_ERR_None} if Succeeded
	 */
	public int SelectStream(int nStream)
	{
		if (mSource == null)
			return voOSType.VOOSMP_SRC_ERR_SOURCE_UNINITIALIZE;
		
		return mSource.SelectStream(nStream);
	}
	
	/**
	 * Select the Stream Track
	 * 
	 * @param 	nTrackID [in] track to be selected
	 * @return 	{@link voOSType#VOOSMP_ERR_None} if Succeeded
	 */
	public int SelectTrack (int nTrackID)
	{
		if (mSource == null)
			return voOSType.VOOSMP_SRC_ERR_SOURCE_UNINITIALIZE;
		
		return ChangeBitrate(EnumSelectFunc.SelectTrack,nTrackID);
	}
		
	/**
	 * Close media source
	 * 
	 * @return 	{@link voOSType#VOOSMP_ERR_None} if Succeeded
	 */
	public synchronized int Close ()
	{
		openedTimes = 0;
		if(mEngine==null || mSource == null)
		{
			return voOSType.VOOSMP_ERR_Status;
		}
		voLog.v(TAG, "close - ");
		
	//	enableInnerCloseCaption = false;
	//	closeTimer();
		
		int nRet; 
        Integer status = (Integer)mEngine.GetParam(voOSType.VOOSMP_PID_STATUS);
        if (status != null && status.intValue() == voOSType.VOOSMP_STATUS_RUNNING){
        	this.Stop();
        }
		
		nRet = mEngine.Close ();
		if (nRet != voOSType.VOOSMP_ERR_None)
		{
			voLog.e(TAG, "mEngine.Close() occur error, error code " + Integer.toHexString(nRet));
		}
		
		nRet = mSource.Close();
		if (nRet != voOSType.VOOSMP_ERR_None)
		{
			voLog.e(TAG, "mSource.Close() occur error, error code " + Integer.toHexString(nRet));
		}
		


//		mRequestListener = null;
		//closeTimer();
		if(customerTagWrap != null)
			customerTagWrap.closeTimer();
		customerTagWrap = null;
		

		if(mSource!=null)
			mSource.Uninit();
		//mSource = null;
		isSourceInit = false;
		
		
		m_nWidthOfAspectRatio = 0;
		m_nHeightOfAspectRatio = 0;
		//mDRMFileName = null;
		//mDRMApiName = null;
		//mCapTablePath = null;
		//mPerformanceData = null;
		//mParamDict = null;
		subtitleFileNameSet = false;
		metadata_Arrive = false;
		voLog.v(TAG, "close + ");
		return nRet;
	}


	/**
	* Run the whole pipe line
	* 
	* @return 	{@link voOSType#VOOSMP_ERR_None} if Succeeded
	*/
    public int Run()
    {
		if(mEngine==null || mSource == null)
		{
			return voOSType.VOOSMP_ERR_Status;
		}
		
		voLog.v(TAG, "Run - ");
		
		if (mbOMXAL)
			mSource.SetParam(voOSType.VOOSMP_SRC_PID_DISABLE_CPU_ADAPTION, 1);
		
    	int nRet; 
		nRet = mSource.Run();
		if (nRet != voOSType.VOOSMP_ERR_None)
		{
			voLog.e(TAG, "mSource.Run() occur error, error code " + Integer.toHexString(nRet));
		}
		
		{
			nRet = mEngine.Run ();
			if (nRet != voOSType.VOOSMP_ERR_None)
			{
				voLog.e(TAG, "mEngine.Run() occur error, error code " + Integer.toHexString(nRet));
			}
		}
		
		voLog.v(TAG, "Run + ");
		return nRet;
    }
    
	/**
	 * Pause the whole pipeline
	 * 
	 * @return 	{@link voOSType#VOOSMP_ERR_None} if Succeeded
	 */
	public int Pause()
	{
		if(mEngine==null || mSource == null)
		{
			return voOSType.VOOSMP_ERR_Status;
		}
		
		voLog.v(TAG, "Pause - ");
		
		int nRet; 
		
		nRet = mEngine.Pause ();
		if (nRet != voOSType.VOOSMP_ERR_None)
		{
			voLog.e(TAG, "mEngine.Pause() occur error, error code " + Integer.toHexString(nRet));
		}
		
		nRet = mSource.Pause();
		if (nRet != voOSType.VOOSMP_ERR_None)
		{
			voLog.e(TAG, "mSource.Pause() occur error, error code " + Integer.toHexString(nRet));
		}
		
		voLog.v(TAG, "Pause + ");
		return nRet;
	}
	
	/**
	* Stop the whole pipeline
	* 
	* @return 	{@link voOSType#VOOSMP_ERR_None} if Succeeded
	*/
	public int Stop()
	{
		if(mEngine==null || mSource == null)
		{
			return voOSType.VOOSMP_ERR_Status;
		}
		
		voLog.v(TAG, "Stop - ");
		
		if (ccMan != null){
			ccMan.show(false);
			ccMan.clearWidget();
			
		}
		
		int nRet; 
		
		nRet = mEngine.Stop ();
		if (nRet != voOSType.VOOSMP_ERR_None)
		{
			voLog.e(TAG, "mEngine.Stop() occur error, error code " + Integer.toHexString(nRet));
		}
		
		nRet = mSource.Stop();
		if (nRet != voOSType.VOOSMP_ERR_None)
		{
			voLog.e(TAG, "mSource.Stop() occur error, error code " + Integer.toHexString(nRet));
		}
		
		try {
			if (ccMan != null)
				ccMan.clearWidget();
		} catch (Exception e) {
			Message m = mEventHandler.obtainMessage(messageCCClearWidget, 0, 0, null);//local file
			mEventHandler.sendMessage(m);
		}
//		Integer param = new Integer(0);
//		mEngine.SetParam(voOSType.VOOSMP_PID_COMMON_CCPARSER, param);
		
//		enableInnerCloseCaption = false;
//		closeTimer();
//		Message m = mEventHandler.obtainMessage(messageDestroyCC_UI, 0, 0, null);//local file
//		mEventHandler.sendMessage(m);
		voLog.v(TAG, "Stop + ");
		return nRet;
	}
	

	
	/**
	 * Get duration of the stream
	 * 
	 * @return 		Current stream duration, if source is live type, the value is 0
	 */
	public int GetDuration()
	{
		if(mSource == null)
		{
			return voOSType.VOOSMP_ERR_Status;
		}
		return mSource.GetDuration();
	}

	
	/**
	 * Seek operation
	 * 
	 * @param 	nPos The position of seeking.
	 * @return 	correct position after seek operation, -1 if seek failed
	 */
	public int SetPos(int nPos)
	{
		if(mEngine==null || mSource == null)
		{
			return -1;
		}
		voLog.v(TAG, "SetPos - ");
		Integer status = 0;
		
		Object obj = mEngine.GetParam(voOSType.VOOSMP_PID_STATUS);
		if (obj == null)
		{
			mRunning = false;
		}else
		{
			status = (Integer)obj;
		}
			
        
		if (status == voOSType.VOOSMP_STATUS_RUNNING)
		{
			mRunning = true;
		}else
			mRunning = false;
		  
		if(mRunning == true)
		{
		   mEngine.Pause();
		   mSource.Pause();
		}
		
		mEventHandler.sendEmptyMessage(messageCCClearWidget);
//		if (ccMan != null)
//			ccMan.clearWidget();
		
		int nNewPos = mSource.SetPos(nPos);
		if (nNewPos == 0)
		{
			nNewPos = nPos;
			voLog.i(TAG, "seek, mSource.SetPos(%d), return is 0.", nPos);
		}
		
		
		if (nNewPos >= 0)
		{
			mEngine.SetPos(nNewPos);
			voLog.i(TAG, "seek, Setup pos is %d, new pos is %d.", nPos, nNewPos);
		}
		
		if(mRunning == true)
		{
			mSource.Run();
			mEngine.Run();
		}else
			mCheckSubtitleTimes = 20;
		voLog.v(TAG, "SetPos +  ");
		return nNewPos;
	}
	
	
	/**
	 * Get current playing position
	 * 
	 * @return 	current position,-1 if fialed 
	 */ 
	public int GetPos ()
	{	
		if(mEngine==null)
		{
			return 0;
		}
		return mEngine.GetPos();
	}
 


	 /**
     * Set playback volume
     * 
     * @param     fLeft	Left audio channel. A value of 0.0f is silence, a value of 1.0f is no attenuation.
     * @param	  fRight	Right audio channel
     * @return    {@link voOSType#VOOSMP_ERR_None} if Succeeded
     */ 
	public int SetVolume(float fLeft, float fRight) throws IllegalStateException 
	{
		if(mEngine==null)
		{
			return voOSType.VOOSMP_ERR_Status;
		}
		return mEngine.SetVolume(fLeft, fRight);
	}

	
	 /**
     * Get the value from specified ID
     * 
     * @param  id 	type of the parameter, refer to
     * <ul>
     * <li>{@link voOSType#VOOSMP_PID_STATUS}
     * <li>{@link voOSType#VOOSMP_PID_SENDBUFFERFUNC}
     * <li>{@link voOSType#VOOSMP_PID_AUDIO_FORMAT}
     * <li>{@link voOSType#VOOSMP_PID_VIDEO_FORMAT}
     * <li>{@link voOSType#VOOSMP_PID_PLAYER_TYPE}
	 * <li>{@link voOSType#VOOSMP_PID_AUDIO_DEC_BITRATE}
	 * <li>{@link voOSType#VOOSMP_PID_VIDEO_DEC_BITRATE}
	 * <li>{@link voOSType#VOOSMP_PID_CPU_INFO}
	 * <li>{@link voOSType#VOOSMP_PID_COMMAND_STRING}
	 * <li>{@link voOSType#VOOSMP_PID_BLUETOOTH_HANDSET}
	 * <li>{@link voOSType#VOOSMP_PID_SUBTITLE_SETTINGS}
	 * <li>{@link voOSType#VOOSMP_SRC_PID_DVRINFO}
	 * <li>{@link voOSType#VOOSMP_SRC_PID_BA_WORKMODE}
	 * <li>{@link voOSType#VOOSMP_SRC_PID_RTSP_STATUS}
	 * <li>{@link voOSType#VOOSMP_SRC_PID_FUNC_READ_BUF}
	 * <li>{@link voOSType#VOOSMP_SRC_PID_COMMAND_STRING}
	 * <li>{@link voOSType#VOOSMP_PID_MODULE_VERSION}
     * </ul>
     * @return    Depending on different id. please refer to {@link voOSType} for details. return null if failed.
     */ 
	public Object GetParam(int id)
	{
	    if (id==voOSType.VOOSMP_PID_VIDEO_DEC_BITRATE)
        {
	        if (mEngine == null)
            {
                voLog.e(TAG, "mEngine is null!");
                return null;
            }
            return mEngine.GetParam(id);
        }
		if (id == voOSType.VOOSMP_PID_CLOSED_CAPTION_SETTINGS)
		{
			if(ccMan == null)
			{
				if (mEngine == null)
				{
					voLog.e(TAG, "mSource is null!");
					return null;
				}
				return mEngine.GetParam(id);
			}
			else
				return ccMan.getSettings();
		}
		if((id & 0x0000FFFF) == voOSType.VOOSMP_PID_MODULE_VERSION &&
				id < voOSType.VOOSMP_SRC_PID_DRM_FILE_NAME)
			return mEngine.GetParam(id);
		
		if (id == voOSType.VOOSMP_SRC_PID_SOURCE_MODULE_OBJECT)
		{
			return mSource;
		}
		
		if ( id >= voOSType.VOOSMP_SRC_PID_DRM_FILE_NAME)
		{
			if (mSource == null)
			{
				voLog.e(TAG, "mSource is null!");
				return null;
			}
			return mSource.GetParam(id);
		}else
		{
			if (mEngine == null)
			{
				voLog.e(TAG, "mSource is null!");
				return null;
			}
			return mEngine.GetParam(id);
		}
	}
	
	/** Save params to dict*/
	private void SaveParameters(int id, Object param){
		if(mParamDict == null)
			mParamDict = new HashMap<Integer, Object>();
		if(mParamDict.containsKey(id))
			mParamDict.remove(id);
		mParamDict.put(id, param);

	}
	
	/** Check is inner closecaption enable*/
	private boolean IsInnerCloseCaptionEnable(){
		return enableInnerCloseCaption;
	}
	
	/** set common CCParser param*/
	private int setCommonCCParser(int id, Object param){
		if(mEngine==null) 
			return 0;
		if(metadata_Arrive && ((Integer)param == 0))
			return 0;

		if(param!=null && (Integer)param == 1)
		{
			enableInnerCloseCaption = true;
			createCloseCaptionUI();
			if(ccMan != null && !metadata_Arrive ){
				ccMan.clearWidget();
			}
		}
		else
		{
			enableInnerCloseCaption = false;
			if(ccMan != null)	
				ccMan.show(false);
			//this.closeTimer();
		}
		return 0;
	}
	
	/** set closecaption output param*/
	private int setCloseCaptionOutput(int id, Object param){
		if(mEngine==null) 
			return 0;
		if(metadata_Arrive)
			return 0;
		
		/*
		int n = mEngine.SetParam(id, param);
    	if(mSubtitleInEngine)
    	{
				return n;
    	}
		*/
		if(((Integer)param) == 1)
		{
	//		if(!IsInnerCloseCaptionEnable())
	//			mEngine.SetParam( voOSType.VOOSMP_PID_COMMON_CCPARSER, 1);
			
			closeCaptionOutput = true;
			startTimer();
			Message m = mEventHandler.obtainMessage(messageShowCC_UI, 0, 0, null);//local file
			mEventHandler.sendMessage(m);

			//if(ccMan != null)	
			//	ccMan.show(false);
		}
		else
		{
			//closeTimer();
			closeCaptionOutput = false;
			Message m = mEventHandler.obtainMessage(messageShowCC_UI, 1, 0, null);//local file
			mEventHandler.sendMessage(m);
			//if(ccMan != null)	
			//	ccMan.show(true);
			
		}
		return 0;
	}
	
	/** get param from dict and set to source*/
	private void setParamInDict(){
		if(mParamDict == null) return;

		if(mParamDict.entrySet() == null) return;

		Iterator<Entry<Integer,Object>> it = mParamDict.entrySet().iterator();
		while(it!=null && it.hasNext())
		{
			Entry<Integer,Object> ent = it.next();
			if(ent == null) continue;
			
			voLog.i(TAG, "ent.getKey()  id is %s", Integer.toHexString(ent.getKey()));

			if(ent.getKey() == voOSType.VOOSMP_SRC_PID_SOCKET_CONNECTION_TYPE)
				voLog.v(TAG, "SetParam(voOSType.VOOSMP_SRC_PID_SOCKET_CONNECTION_TYPE, %s) ", ent.getValue().toString());
			if(ent.getKey() == voOSType.VOOSMP_SRC_PID_DOHTTPVERIFICATION)
				voLog.v(TAG, "SetParam(voOSType.VOOSMP_SRC_PID_DOHTTPVERIFICATION, %s) ", ((voOSVerificationInfo)ent.getValue()).getVerificationData());
			if(ent.getKey() == voOSType.VOOSMP_PID_MIN_BUFFTIME)
				voLog.v(TAG, "SetParam(voOSType.VOOSMP_PID_MIN_BUFFTIME, %s) ", ent.getValue().toString());
			
			if(ent.getKey() == voOSType.VOOSMP_SRC_PID_CAP_TABLE_PATH){
				voOSPerformanceData perfData =  ReadCapTableFromFile((String)ent.getValue());
				if (perfData == null){
					voLog.e(TAG, "ReadCapTableFromFile error, don't setup cap data!");
				}else{
					SetParam(voOSType.VOOSMP_PID_PERFORMANCE_DATA, perfData);
				}
				
				continue;
			}
			
			if(ent.getKey() == voOSType.VOOSMP_SRC_PID_INIT_PARAM_ACTUAL_FILE_SIZE
					|| ent.getKey() == voOSType.VOOSMP_SRC_PID_INIT_PARAM_IO_FILE_NAME
					|| ent.getKey() == voOSType.VOOSMP_SRC_PID_INIT_PARAM_IO_API_NAME)
				continue;
			
			mSource.SetParam(ent.getKey(),ent.getValue());
		}
	}
	
	/**
     * Set parameter for BasePlayer
     * 
     * @param id		type of the parameter, refer to
     * <ul>
     * <li> {@link voOSType#VOOSMP_PID_PERFORMANCE_DATA}
     * <li> {@link voOSType#VOOSMP_SRC_PID_DRM_FILE_NAME}
     * <li> {@link voOSType#VOOSMP_SRC_PID_DRM_API_NAME}
     * <li> {@link voOSType#VOOSMP_SRC_PID_FUNC_IO}	
     * <li> {@link voOSType#VOOSMP_SRC_PID_CC_AUTO_SWITCH_DURATION}
     * <li> {@link voOSType#VOOSMP_SRC_PID_DRM_THIRDPARTY_FUNC_SET}
     * <li> {@link voOSType#VOOSMP_SRC_PID_DRM_CALLBACK_FUNC}
	 * <li> {@link voOSType#VOOSMP_SRC_PID_DOHTTPVERIFICATION}
	 * <li> {@link voOSType#VOOSMP_SRC_PID_SOCKET_CONNECTION_TYPE}
	 * <li> {@link voOSType#VOOSMP_SRC_PID_COMMAND_STRING}
	 * <li> {@link voOSType#VOOSMP_SRC_PID_CAP_TABLE_PATH}
	 * <li> {@link voOSType#VOOSMP_SRC_PID_BA_STARTCAP}
	 * <li> {@link voOSType#VOOSMP_SRC_PID_DODRMVERIFICATION}
	 * <li> {@link voOSType#VOOSMP_SRC_PID_DRM_UNIQUE_IDENTIFIER}
	 * <li> {@link voOSType#VOOSMP_SRC_PID_Timed_Text_ClosedCaption} 
	 * <li> {@link voOSType#VOOSMP_SRC_PID_DOWNLOAD_FAIL_MAX_TOLERANT_COUNT} 
	 * <li> {@link voOSType#VOOSMP_SRC_PID_DISABLE_CPU_ADAPTION}
	 * <li> {@link voOSType#VOOSMP_PID_SUBTITLE_FILE_NAME}
	 * <li> {@link voOSType#VOOSMP_SRC_PID_DRM_FUNC_SET} 
	 * <li> {@link voOSType#VOOSMP_SRC_PID_BUFFER_START_BUFFERING_TIME} 
	 * <li> {@link voOSType#VOOSMP_SRC_PID_BUFFER_BUFFERING_TIME}
	 * <li> {@link voOSType#VOOSMP_SRC_PID_LOW_LATENCY_MODE}
	 * <li> {@link voOSType#VOOSMP_SRC_PID_BUFFER_MAX_SIZE}
	 * <li> {@link voOSType#VOOSMP_SRC_PID_HTTP_HEADER}
	 * <li> {@link voOSType#VOOSMP_SRC_PID_RTSP_CONNECTION_PORT}
     * <li> {@link voOSType#VOOSMP_SRC_PID_HTTP_PROXY_INFO}
     * <li> {@link voOSType#VOOSMP_SRC_PID_BITRATE_THRESHOLD}
     * <li> {@link voOSType#VOOSMP_SRC_PID_SEGMENT_DOWNLOAD_RETRY_COUNT}
     * <li> {@link voOSType#VOOSMP_SRC_PID_ENABLE_RTSP_HTTP_TUNNELING}
     * <li> {@link voOSType#VOOSMP_SRC_PID_RTSP_OVER_HTTP_CONNECTION_PORT}
     * <li> {@link voOSType#VOOSMP_SRC_PID_HTTP_RETRY_TIMEOUT}
     * </ul>
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
     * <li>{@link voOSType#VOOSMP_PID_SUBTITLE_SETTINGS}
     * <li>{@link voOSType#VOOSMP_PID_VIDEO_PERFORMANCE_ONOFF}
     * <li>{@link voOSType#VOOSMP_PID_SUBTITLE_SETTINGS_ONOFF}
     * <li>{@link voOSType#VOOSMP_PID_APPLICATION_SUSPEND}
     * <li>{@link voOSType#VOOSMP_PID_APPLICATION_RESUME}
     * <li>{@link voOSType#VOOSMP_PID_AUDIO_SINK}
     * <li>{@link voOSType#VOOSMP_PID_BITMAP_HANDLE}
     * <li>{@link voOSType#VOOSMP_PID_PREVIEW_SUBTITLE}
     * </ul>
     * @param param		the param depend on different id.
     * @return			{@link voOSType#VOOSMP_ERR_None} if Succeeded
     */ 
	public int SetParam(int id, Object param)
	{
		if (id == voOSType.VOOSMP_PID_PREVIEW_SUBTITLE){
			voOSPreviewSubtitleInfo info = (voOSPreviewSubtitleInfo)param;
			if(info == null)
				return 0;
			if(ccMan!=null){
				ccMan.clearWidget();
			}
			if(ccMan == null)
				ccMan = new ClosedCaptionManager();
	    	ccMan.previewSubtitle(info);
	    	//ccMan.setSurfaceView(mSurfaceView);

			return 0;
		}
		if (id == voOSType.VOOSMP_PID_SUBTITLE_TIME_OFFSET){
			mOffsetTime = (Integer)param;
			return 0;
		}
		if (id == voOSType.VOOSMP_PID_VIDEO_ASPECT_RATIO){
			m_nAspectRationType = (Integer)param;
			this.updateVideoAspectRatio(0, 0);
			return 0;
		}
			
		if (id == voOSType.VOOSMP_PID_VIEW_ACTIVE
				|| id == voOSType.VOOSMP_PID_APPLICATION_RESUME)
		{
			mSurfaceView = (SurfaceView)param;
		}
		if (id == voOSType.VOOSMP_PID_COMMON_CCPARSER)
		{
			return setCommonCCParser(id,param);
	//		Message m = mEventHandler.obtainMessage(messageCCEnable, id, 0, param);//local file
	//		mEventHandler.sendMessage(m);

	//		return 0;//setCommonCCParser(id,param);
		}
		if (id == voOSType.VOOSMP_PID_CLOSED_CAPTION_OUTPUT)
		{
			return setCloseCaptionOutput(id,param);
		}
		if (id == voOSType.VOOSMP_PID_SUBTITLE_IN_ENGINE)
		{
			return 0;
		}
		if (id == voOSType.VOOSMP_PID_PERFORMANCE_DATA || 
			id == voOSType.VOOSMP_SRC_PID_BA_STARTCAP  ||
			id == voOSType.VOOSMP_PID_LICENSE_CONTENT  ||
			id == voOSType.VOOSMP_PID_LICENSE_FILE_PATH ||
			id == voOSType.VOOSMP_SRC_PID_LOW_LATENCY_MODE ||
			id == voOSType.VOOSMP_PID_LICENSE_TEXT ||
			id == voOSType.VOOSMP_SRC_PID_PD_CONNECTION_RETRY_TIMES ||
			id == voOSType.VOOSMP_PID_AUDIO_PLAYBACK_SPEED)
		{
			if(isSourceInit == false)
			{
				if (id == voOSType.VOOSMP_PID_PERFORMANCE_DATA)
					voLog.v(TAG, "to set parameter VOOSMP_PID_PERFORMANCE_DATA to HashMap!");
				else
					voLog.v(TAG, "to set parameter VOOSMP_SRC_PID_BA_STARTCAP to HashMap!");
				SaveParameters(id, param);
				if(id != voOSType.VOOSMP_PID_AUDIO_PLAYBACK_SPEED)
					return voOSType.VOOSMP_ERR_None;
				//return voOSType.VOOSMP_ERR_Status;
			}
			voLog.i(TAG, "to set parameter VOOSMP_PID_PERFORMANCE_DATA!");
			int nRtn = mSource.SetParam(id, param);
			if(id != voOSType.VOOSMP_PID_AUDIO_PLAYBACK_SPEED)
				return nRtn;
		}
		
		if (id == voOSType.VOOSMP_PID_MIN_BUFFTIME 
				|| id == voOSType.VOOSMP_PID_SUBTITLE_FILE_NAME  
				|| id == voOSType.VOOSMP_SRC_PID_Timed_Text_ClosedCaption)
		{
			if (id == voOSType.VOOSMP_PID_SUBTITLE_FILE_NAME  || id == voOSType.VOOSMP_SRC_PID_Timed_Text_ClosedCaption){
				if(((String)param).length()>0)
					subtitleFileNameSet = true;

			}
			if(isSourceInit == false)
			{				
				if (id == voOSType.VOOSMP_PID_MIN_BUFFTIME)
					voLog.e(TAG, "to set VOOSMP_PID_MIN_BUFFTIME parameter to HashMap!");
				if(id == voOSType.VOOSMP_SRC_PID_HTTP_HEADER)
				if(((voOSHTTPHeader)param).getHeaderName().equals("Set-Cookie"))
				  return voOSType.VOOSMP_ERR_None;
				SaveParameters(id, param);
				return voOSType.VOOSMP_ERR_None;
			}
			else
			{
				return mSource.SetParam(id, param);
			}
		}
		
		if (id == voOSType.VOOSMP_SRC_PID_DRM_UNIQUE_IDENTIFIER
				|| id == voOSType.VOOSMP_SRC_PID_DODRMVERIFICATION)
		{
			if (mDRM != null)
			{
				mDRM.setParameter(id, param);
				voLog.i(TAG, "mDRM.setParameter(%s, %s);", 
						Integer.toHexString(voOSType.VOOSMP_SRC_PID_DRM_UNIQUE_IDENTIFIER), param);
				return 0;
			}
			voLog.i(TAG, "SetParam ID is %d", id);
		}
		
		if ( id >= voOSType.VOOSMP_SRC_PID_DRM_FILE_NAME)
		{
			if (isSourceInit == false)
			{
				voLog.e(TAG, "to set parameter to HashMap! id = %s", Integer.toHexString(id));
				SaveParameters(id, param);
				return voOSType.VOOSMP_ERR_None;
			}else
			{
				if(id == voOSType.VOOSMP_SRC_PID_Timed_Text_ClosedCaption 
				|| id == voOSType.VOOSMP_SRC_PID_SOURCE_MODULE_OBJECT 
				|| id == voOSType.VOOSMP_SRC_PID_INIT_PARAM_ACTUAL_FILE_SIZE 
				|| id == voOSType.VOOSMP_SRC_PID_INIT_PARAM_INIT_LIBOP 
				|| id == voOSType.VOOSMP_SRC_PID_INIT_PARAM_IO_FILE_NAME 
				|| id == voOSType.VOOSMP_SRC_PID_INIT_PARAM_IO_API_NAME)
				{
					return voOSType.VOOSMP_ERR_None;
				}
				if (id == voOSType.VOOSMP_SRC_PID_CAP_TABLE_PATH)
				{
					SaveParameters(id, param);
					voOSPerformanceData perfData =  ReadCapTableFromFile((String)param);//mCapTablePath
					if (perfData == null)
					{
						voLog.e(TAG, "ReadCapTableFromFile error, don't setup cap data!");
						return voOSType.VOOSMP_SRC_ERR_OPEN_SRC_FAIL;
					}else
					{
						return mSource.SetParam(voOSType.VOOSMP_PID_PERFORMANCE_DATA, perfData);
					}
				}
			}
			voLog.i(TAG, "mSource SetParam ID is %d", id);
			return mSource.SetParam(id, param);
		}else
		{
			if (mEngine == null)
			{
				voLog.e(TAG, "mEngine is null!");
				return voOSType.VOOSMP_SRC_ERR_SOURCE_UNINITIALIZE;
			}
			
			if (id == voOSType.VOOSMP_PID_VIDEO_RENDER_TYPE)
			{
				int type = (Integer)param;
				if (type == voOSType.VOOSMP_RENDER_TYPE_HW_RENDER || voOSType.VOOSMP_RENDER_TYPE_JMHW_RENDER == type)
				{
					mRenderType = type;
					mbOMXAL = true;
				}
			}

			return mEngine.SetParam(id, param);
		}
		
	}
	
	
	/**
	 * Get Subtitle Language Information
	 * 
	 * @return Subtitle Language Information. 
	 * 
	 */
	
	public List<voOSSubtitleLanguage> GetSubtitleLanguageInfo()
	{
		if (mSource == null || mEngine == null)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return null;
		}
		if( subtitleFileNameSet){
			return mSource.GetSubtitleLanguageInfo();
		}
		
		List<voOSSubtitleLanguage> lsInfo = mSource.GetSubtitleLanguageInfo();
		if((lsInfo != null)&&(lsInfo.size() != 0))
			return lsInfo;

		return mEngine.GetSubtitleLanguageInfo();
	}
	
	/**
	 * Select Subtitle Language
	 * 
	 * @param nIndex
	 * @return Success return {@link voOSType#VOOSMP_ERR_None}, error return error code.
	 */
	public int SelectSubtitleLanguage(int nIndex)
	{
		if (mSource == null || mEngine == null)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return voOSType.VOOSMP_ERR_Uninitialize;
		}
		
		return ChangeBitrate(EnumSelectFunc.SelectSubtitleLanguage,nIndex);
		
		/*
		if( subtitleFileNameSet){
			return mSource.SelectSubtitleLanguage(nIndex);
		}
		
		return mEngine.SelectSubtitleLanguage(nIndex);
		*/
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
		if (mEngine == null)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return voOSType.VOOSMP_ERR_Uninitialize;
		}
		return mEngine.GetSEITimingInfo(subID, timeStamp);
	}

	public int onEvent(int id, int param1, int param2, Object obj) {
		// TODO Auto-generated method stub
		switch(id){
		    case voOSType.VOOSMP_CB_VideoSizeChanged : {  // Video size changed
		        
		        m_nVideoWidth = param1;
		        m_nVideoHeight = param2;
		        break;
		    }
		    case voOSType.VOOSMP_CB_VideoRenderStart:{
		    	{
		    		if(this.ccMan!=null){
		    			ccMan.show(true);
		    		}
		    	}
		    	break;
		    }
		    case voOSType.VOOSMP_CB_VideoAspectRatio: {
	 			switch(param1)
				{
					case voOSType.VOOSMP_RATIO_00:
					{
						break;
					}
					case voOSType.VOOSMP_RATIO_11:
					{
						m_nWidthOfAspectRatio = 1;
						m_nHeightOfAspectRatio = 1;
						break;
					}
					case voOSType.VOOSMP_RATIO_43:
					{
						m_nWidthOfAspectRatio = 4;
						m_nHeightOfAspectRatio = 3;
						break;
					}
					case voOSType.VOOSMP_RATIO_169:
					{
						m_nWidthOfAspectRatio = 16;
						m_nHeightOfAspectRatio = 9;
						break;
					}
					case voOSType.VOOSMP_RATIO_21:
					{
						m_nWidthOfAspectRatio = 2;
						m_nHeightOfAspectRatio = 1;
						break;
					}
					default:
					{
						if(param1>0){
							m_nWidthOfAspectRatio = (param1 >> 16);
							m_nHeightOfAspectRatio = (param1&0xFFFF);
						}
						break;
					}

				}
	 			float fRate = 4/3.0f;
	 			if(m_nHeightOfAspectRatio>0)
	 				fRate = m_nWidthOfAspectRatio/(float)m_nHeightOfAspectRatio;
				if(ccMan!=null && fRate>0.1)
				{
					ccMan.setXYRate(fRate);
				}
		    	break;
		    }
		    case voOSType.VOOSMP_CB_MediaTypeChanged:{
	        	mTrackType = param1;
	        	if ((mTrackType == voOSType.VOOSMP_AVAILABLE_AUDIOVIDEO) && metadata_Arrive)
	        	{
					Message m = mEventHandler.obtainMessage(messageCCClearWidget, 0, 0, null);//local file
					mEventHandler.sendMessage(m);
	        		
	        		metadata_Arrive = false;
	        	}
	        	voLog.v(TAG, "VOOSMP_CB_MediaTypeChanged type is %d. VOOSMP_CB_Metadata_Arrive is %b",mTrackType, metadata_Arrive);
		    	break;
		    }
			case voOSType.VOOSMP_CB_VideoStartBuff:
			{
//				if(ccMan!=null && enableInnerCloseCaption)
//				{
//					ccMan.show(false);
//				}
				break;
			}
			case voOSType.VOOSMP_CB_VideoStopBuff:
				{
//					if(ccMan!=null && enableInnerCloseCaption)
//					{
//						ccMan.show(true);
//					}
				}
				break;
		}
		
		checkEventVOOSMP_CB_Metadata_Arrive( id,  param1,  param2,  obj);

		{
//			voLog.i(TAG, "onEvent ID = %s, nParam1 = %d", Integer.toHexString(id), param1);
			Message msg = mEventHandler.obtainMessage(id, param1, param2, obj);
			mEventHandler.sendMessage(msg);
		}
		return voOSType.VOOSMP_ERR_None;
	}


	public int onRequest(int id, int param1, int param2, Object obj) {
		// TODO Auto-generated method stub

		if (id == voOSType.VOOSMP_SRC_CB_Customer_Tag)
	    {
	    	if (param1 == voOSType.VOOSMP_SRC_CUSTOMERTAGID_TIMEDTAG)
	    	{
	    		final voOSTimedTag tag = (voOSTimedTag) obj;
	    		if(tag == null)
	    		{
	    			voLog.i(TAG, "VOOSMP_SRC_CB_Customer_Tag = null" );
	    			return voOSType.VOOSMP_ERR_None;
	    		}
	    			
	    		if(tag.Flag() == 0){
			    		//VO_SOURCE2_CUSTOMERTAG_TIMEDTAG_FLAG_STRINGTAG==0
		    			new Thread(new Runnable(){
		    				@Override
		    				public void run() {
		    				try {
						      		String s = new String(tag.Data());
						      		voLog.v(TAG, "VOOSMP_SRC_CB_Customer_Tag = %s" ,s);
						      		if(frameScrubbing == null)
						      			frameScrubbing = new voFrameScrubbing();
		
						      		frameScrubbing.setFrameScrubbingUrl(s);
							} 
		    				catch(Exception e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
								voLog.i(TAG,"download error" );
							}  
		    				}
		    			}).start();
			    		//return voOSType.VOOSMP_ERR_None;
	    		}
	    		else{
		    		if(customerTagWrap == null)
		    			customerTagWrap = new voCustomerTagWrap();
		    		customerTagWrap.customerTagArray.add(tag);
		    		Long time = tag.TimeStamp();
		    		String s = new String(tag.Data());
		    		voLog.v(TAG, "mTimedTagList customerTagWrap saving, Flag is %d, size is %d, timestamp is %d, %s", tag.Flag(), tag.Size(), time.intValue(), s);
		    		//according to current time to send tag later
		    		return voOSType.VOOSMP_ERR_None;
	    		}
	    	}
	    }
		
		if (mRequestListener != null)
		{
			int ret = mRequestListener.onRequest(id, param1, param2, obj);
			voLog.i(TAG, "onRequest id = " + Integer.toHexString(id) 
					+ ", param1 = " + param1 + ", return is " + Integer.toHexString(ret));
			return ret;
		}

		return voOSType.VOOSMP_ERR_None;
	}
	private void checkEventVOOSMP_CB_Metadata_Arrive(int arg0, int arg1, int arg2, Object arg3)
	{
		if(arg0 == voOSType.VOOSMP_CB_Metadata_Arrive )
	    {
	        if((arg1 == 3))//voOSType.VOOSMP_SS_IMAGE
	        {//to enable subtitle
	        	if(this.ccMan == null)
	        		SetParam(voOSType.VOOSMP_PID_COMMON_CCPARSER, 1);
	        	voLog.v(TAG, "event VOOSMP_CB_Metadata_Arrive in onDataSourceEvent" );
	        	metadata_Arrive = true;
	        	
				Message m = mEventHandler.obtainMessage(messageCCMetadataArrive, 0, 0, arg3);//local file
				mEventHandler.sendMessage(m);

//	        	if(this.ccMan != null){
//	        		ccMan.setData((Parcel)arg3, false);
//	        	}
	        }
	    }
		
	}
	public int onDataSourceEvent(int id, int param1, int param2, Object obj) {
		// TODO Auto-generated method stub

		checkEventVOOSMP_CB_Metadata_Arrive( id, param1,  param2,  obj);
		
	//	if (mEventListener != null)
	//		mEventListener.onEvent(arg0, arg1, arg2, arg3);
		
	//	if (mEventHandler != null)
		{
	//		voLog.i(TAG, "onDataSourceEvent ID = %s, nParam1 = %d", Integer.toHexString(id), param1);
			Message msg = mEventHandler.obtainMessage( id, param1,  param2,  obj);
			mEventHandler.sendMessage(msg);
		}
		return voOSType.VOOSMP_ERR_None;
	}

	private voOSPerformanceData ReadCapTableFromFile(String fileName)
	{
		voOSCPUInfo info  = (voOSCPUInfo)mEngine.GetParam(voOSType.VOOSMP_PID_CPU_INFO);
		if (info == null)
		{
			voLog.e(TAG, "mEngine.GetParam(voOSType.VOOSMP_PID_CPU_INFO) Failed!");
			return null;
		}
		
        DocumentBuilderFactory factory=null;
        DocumentBuilder builder=null;
        Document document=null;
        InputStream inputStream=null;
        HashMap<String, Object> map; 
        
        final int MAXFrequency = 1000000000;
        
        ArrayList<HashMap<String, Object>> lstCapItem = new ArrayList<HashMap<String, Object>>(); 
        
        factory=DocumentBuilderFactory.newInstance();
        try {
            builder=factory.newDocumentBuilder();
            inputStream= new FileInputStream(fileName);
            document=builder.parse(inputStream);
            //root element   
	         Element root = document.getDocumentElement();   
	         //Do something here   
	         //get a NodeList by tagname   
	         Node node  = root.getFirstChild();
	         
	         NodeList nodeList = root.getElementsByTagName("item");  
	         for (int i = 0; i < nodeList.getLength(); i++)
	         {
		          node = nodeList.item(i);
		         
		          map = new HashMap<String, Object>();   
		          NodeList nodeList2;
	        	  nodeList2 = node.getChildNodes();
	        	  for (int j = 0; j<nodeList2.getLength(); j++)
	        	  {
	        		  Node childNode = nodeList2.item(j);
	        		  
			          if (childNode.getNodeName().compareToIgnoreCase("Core") == 0)
			    	  {
			    		  map.put("Core", Integer.parseInt(childNode.getFirstChild().getNodeValue()));
			    	  }else if (childNode.getNodeName().compareToIgnoreCase("Neon") == 0)
			    	  {
			    		  map.put("Neon", Integer.parseInt(childNode.getFirstChild().getNodeValue()));
			    	  }else if (childNode.getNodeName().compareToIgnoreCase("Frequency") == 0)
			    	  {
			    		  map.put("Frequency", Integer.parseInt(childNode.getFirstChild().getNodeValue()));
			    	  }else if (childNode.getNodeName().compareToIgnoreCase("CodecType") == 0)
			    	  {
			    		  map.put("CodecType", Integer.parseInt(childNode.getFirstChild().getNodeValue()));
			    	  }else if (childNode.getNodeName().compareToIgnoreCase("BitRate") == 0)
			    	  {
			    		  map.put("BitRate", Integer.parseInt(childNode.getFirstChild().getNodeValue()));
			    	  }else if (childNode.getNodeName().compareToIgnoreCase("VideoWidth") == 0)
			    	  {
			    		  map.put("VideoWidth", Integer.parseInt(childNode.getFirstChild().getNodeValue()));
			    	  }else if (childNode.getNodeName().compareToIgnoreCase("VideoHeight") == 0)
			    	  {
			    		  map.put("VideoHeight", Integer.parseInt(childNode.getFirstChild().getNodeValue()));
			    	  }else if (childNode.getNodeName().compareToIgnoreCase("ProfileLevel") == 0)
			    	  {
			    		  map.put("ProfileLevel", Integer.parseInt(childNode.getFirstChild().getNodeValue()));
			    	  }else if (childNode.getNodeName().compareToIgnoreCase("FPS") == 0)
			    	  {
			    		  map.put("FPS", Integer.parseInt(childNode.getFirstChild().getNodeValue()));
			    	  }
	        	  }
		 //		  map.put("ItemImage", R.drawable.tudou);
	        	  lstCapItem.add(map);
	        }
        }catch (IOException e){
            e.printStackTrace();
        } catch (SAXException e) {
            e.printStackTrace();
        }
         catch (ParserConfigurationException e) {
            e.printStackTrace();
        }finally{
            try {
            	if (inputStream != null)
                inputStream.close();
            } catch (IOException e) {    
                e.printStackTrace();
            }
        }
        
        if (lstCapItem.size() == 0)
        	return null;
        
		Comparator comp = new Mycomparator();
		Collections.sort(lstCapItem,comp);
        
        
        int Core = info.CoreCount();
        int Neon = info.CPUType();
        int Frequency = info.Frequency();
        
        int iIndex = -1;
        int lowFrequencyIndex = -1;
        
        for( int i = 0; i < lstCapItem.size(); i ++)
        {
        	map = lstCapItem.get(i);
        	if (Core > (Integer)map.get("Core")) 
        	{
        		iIndex = i;
        		break;
        	}
        	
        	if (Core == (Integer)map.get("Core")
        		&& Neon > ((Integer)map.get("Neon")))
        	{
        		iIndex = i;
        		break;
        	}
        	
        	if ((Core >= (Integer)map.get("Core")) 
        		&& Neon >= ((Integer)map.get("Neon"))
        		&& Frequency > ((Integer)map.get("Frequency")))
        	{
        		iIndex = i;
        		break;
        	}
        	
        	if ((Core >= (Integer)map.get("Core")) 
            		&& Neon >= ((Integer)map.get("Neon"))
            		&& Frequency <= ((Integer)map.get("Frequency")))
        	{
        		lowFrequencyIndex = i;
        	}
        	
    /*    	        	
        	String str = String.format("Core is %d, Neon is %d, Frequency is %d, CodecType is %d, BitRate is %d, VideoWidth is %d, VideoHeight is %d ProfileLevel is %d, FPS is %d", 
        			map.get("Core"), map.get("Neon"), map.get("Frequency"), 
        			map.get("CodecType"), map.get("BitRate"), map.get("VideoWidth"), 
        			map.get("VideoHeight"), map.get("ProfileLevel"), map.get("FPS"));
        	
        	voLog.v(TAG, str);
*/
        	
        	
        }
        
    	if (lowFrequencyIndex != -1)
    	{
    		map = lstCapItem.get(lowFrequencyIndex);
    	}else if (lowFrequencyIndex == -1 && iIndex != -1)
    	{
    		map = lstCapItem.get(iIndex);
    	}else
    	{
    		return null;
    	}
    	
    	String str = String.format("Cap File is %s. \nCore is %d, Neon is %d, Frequency is %d, CodecType is %d, BitRate is %d, VideoWidth is %d, VideoHeight is %d ProfileLevel is %d, FPS is %d", 
    			fileName, map.get("Core"), map.get("Neon"), map.get("Frequency"), 
    			map.get("CodecType"), map.get("BitRate"), map.get("VideoWidth"), 
    			map.get("VideoHeight"), map.get("ProfileLevel"), map.get("FPS"));
    	
    	str = str + "\n" + String.format("lowFrequencyIndex is %d, iIndex is %d .", lowFrequencyIndex, iIndex);
    	
    	voLog.v(TAG, str);
		
		return new voOSPerformanceDataImpl((Integer)map.get("CodecType"), (Integer)map.get("BitRate"),  (Integer)map.get("VideoWidth"), (Integer)map.get("VideoHeight"), (Integer)map.get("ProfileLevel"),  (Integer)map.get("FPS"));

	}
	
	private class Mycomparator implements Comparator<Object>
	{
		@SuppressWarnings("unchecked")
		public int compare(Object o1,Object o2) 
		{
			HashMap<String, Object> p1=(HashMap<String, Object>) o1;
			HashMap<String, Object> p2=(HashMap<String, Object>) o2;  


			if ((Integer)p1.get("Core") > (Integer)p2.get("Core"))
			{
				return -1;
			}else if ((Integer)p1.get("Core") == (Integer)p2.get("Core"))
			{
				if ((Integer)p1.get("Neon") > (Integer)p2.get("Neon"))
				{
					return -1;
				}else if ((Integer)p1.get("Neon") == (Integer)p2.get("Neon"))
				{
					if ((Integer)p1.get("Frequency") >= (Integer)p2.get("Frequency"))
					{
						return -1;
					}else
					{
						return 1;
					}
				}else
				{
					return 1;
				}
			}else 
			{
				return 1;
			}

			
		}
	}

	private void createCloseCaptionUI()
	{
		if(!IsInnerCloseCaptionEnable())
			return;
		if(ccMan!=null){
	    	//startTimer();
			ccMan.show(true);
			ccMan.setActivity(this.mContext);
	    	ccMan.setSurfaceView(mSurfaceView);
		}
		if(ccMan!=null || mSurfaceView == null)
			return;
		if(mSurfaceView.getParent()!=null)
		{
	    	ccMan = new ClosedCaptionManager();
	    	ccMan.setActivity(this.mContext);
	    	ccMan.setSurfaceView(mSurfaceView);
    		ccMan.setXYRate(GetAspectWidthHeight(0, 0,  null));
	    	
	    	startTimer();
			
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
	
	/**
     * Get Frame Scrubbing Thumbnail url.
     *
     * @param timeStamp use to get Frame Scrubbing Thumbnail at the time.
     * 
     * @return the Frame Scrubbing Thumbnail url
     */
	public String GetFrameScrubbingThumbnail(int timeStamp){
		if(frameScrubbing!=null){
			return frameScrubbing.GetThumbnailByTime(timeStamp);
		}
		return null;
	}
	
    /**
     * Set screen display brightness mode. This function is not applicable to Android version 2.1 or below.
     *
     * @param mode [in] Refer to 
     * <ul>
     * <li>{@link voOSType#VOOSMP_SCREEN_BRIGHTNESS_MODE_MANUAL}
     * <li>{@link voOSType#VOOSMP_SCREEN_BRIGHTNESS_MODE_AUTOMATIC}
     * </ul>
     * @return    {@link voOSType#VOOSMP_ERR_None} if successful.
     */
    public int SetScreenBrightnessMode(int mode)
    {
    	if (!isFroyoOrAbove())
    		return voOSType.VOOSMP_ERR_Implement;
    	
    	int screenMode=0;  
    	
    	if (mode == voOSType.VOOSMP_SCREEN_BRIGHTNESS_MODE_MANUAL)
    		screenMode = mode;
    	else if (mode == voOSType.VOOSMP_SCREEN_BRIGHTNESS_MODE_AUTOMATIC)
    		screenMode = mode;
    		
    	  
        try{  
            Settings.System.putInt(mContext.getContentResolver(), 
           		Settings.System.SCREEN_BRIGHTNESS_MODE, screenMode); 
        }  
        catch (Exception localException){  
           return voOSType.VOOSMP_ERR_Unknown;
        }  
         
    	return voOSType.VOOSMP_ERR_None;
    }

    /**
     * Get screen display brightness mode
     *
     * @return Current display brightness mode. 
     * <ul>
     * <li>{@link voOSType#VOOSMP_SCREEN_BRIGHTNESS_MODE_MANUAL}
     * <li>{@link voOSType#VOOSMP_SCREEN_BRIGHTNESS_MODE_AUTOMATIC}
     * </ul>
     * For Android version 2.1 or below, it returns {@link voOSType#VOOSMP_SCREEN_BRIGHTNESS_MODE_MANUAL}
     */
    public int GetScreenBrightnessMode()
    {

    	if (!isFroyoOrAbove())
    		return voOSType.VOOSMP_SCREEN_BRIGHTNESS_MODE_MANUAL;
    	
    	 int screenMode=0;  
         try{  
             screenMode = Settings.System.getInt(mContext.getContentResolver(), 
            		 Settings.System.SCREEN_BRIGHTNESS_MODE);  
             return screenMode;
         }  
         catch (Exception localException){  
        	 return voOSType.VOOSMP_SCREEN_BRIGHTNESS_MODE_MANUAL;
         }  
    }
	
    /**
     * Set screen display brightness
     *
     * @param     brightness [in] Display brightness (percent). The valid range is 0 to 100.
     *
     * @return    {@link voOSType#VOOSMP_ERR_None} if successful.
     */
    public int SetScreenBrightness(int brightness)
    {
        WindowManager.LayoutParams lp = ((Activity) mContext).getWindow().getAttributes();
        float nBrightness = (float) (brightness * 0.01);
        if (nBrightness == 0)
            nBrightness = (float) 0.1;
        
        lp.screenBrightness = nBrightness;
        ((Activity) mContext).getWindow().setAttributes(lp);
        
        
        brightness = (int) ((float)brightness * 2.55);
        
            if(Settings.System.putInt(mContext.getContentResolver(), Settings.System.SCREEN_BRIGHTNESS, brightness))
                return voOSType.VOOSMP_ERR_None;
            return voOSType.VOOSMP_ERR_Unknown;
    }

    /**
     * Get screen display brightness
     *
     * @return Current display brightness (percent).
     */
    public int GetScreenBrightness()
    {
    	int screenBrightness=255;  
        try{  
            screenBrightness = Settings.System.getInt(mContext.getContentResolver(), Settings.System.SCREEN_BRIGHTNESS);  
        }  
        catch (Exception localException){  
            
        }  
        screenBrightness = (int) ((float)screenBrightness/2.55);
        return screenBrightness;  
    }
    
    private boolean isFroyoOrAbove()
    {
    	if (android.os.Build.VERSION.RELEASE.startsWith("1.")
    			||android.os.Build.VERSION.RELEASE.startsWith("2.0")
    			||android.os.Build.VERSION.RELEASE.startsWith("2.1"))
		{
    		return false;
		}else
			return true;
    }
	
	private class voFrameScrubbing{
		//for show pictures
		private String 			urlPath = "";
		TreeMap<Integer, String> treeMap=new TreeMap<Integer, String>(new Comparator<Integer>(){ 
		      public int compare(Integer obj1,Integer obj2){
		          if (obj2>obj1)
		          {
		        	  return 1; 
		          }else
		        	  return -1;
		         } 
		        });
		private synchronized void setFrameScrubbingUrl(Object obj)
		{
			
      		String str = obj.toString();
      		String [] strArr = str.split("\n");
      		str = "";
      		if(strArr!=null){
      			for(int i=0;i<strArr.length;i++){
      				strArr[i] = strArr[i].trim();
      				strArr[i] = strArr[i].replace('\r', '\0');
      				String str2 = strArr[i].toLowerCase();
      				if(str2.endsWith(".m3u8")){
      					str = strArr[i];
      					break;
      				}
      			}
      		}
      		if(str.length()>0){
      			String sLow = str.toLowerCase();
      			if(!sLow.startsWith("http") && mSourceUrl!=null)
      			{
      				int nDiv = mSourceUrl.lastIndexOf('/');
      				if(nDiv>=0){
          				str = mSourceUrl.substring(0,nDiv+1)+str;
          				voLog.v(TAG, "VOOSMP_SRC_CB_Customer_Tag url = %s" , str);
      				}
      			}
      			setThumbnailURL(str);
      		}
			
		}
		private void setThumbnailURL(String PlayListURL)
		{
			//callEnabled = false;
			//thumbnailInfoArray.clear();
			URL urlLink;
			try {
				urlLink = new URL(PlayListURL);
				voLog.i(TAG,"Thumbnail PlayList download begin" );
				if(PlayListURL.lastIndexOf("/")>=0)
					urlPath = PlayListURL.substring(0,PlayListURL.lastIndexOf("/"));
				BufferedInputStream in = new BufferedInputStream(urlLink.openStream());
				
				ByteArrayBuffer ba = new ByteArrayBuffer(0);
				if(in!=null)
				{
					byte [] buffer = new byte[1024];
					String s = "";
					while(true)
					{
						int nRead = in.read(buffer);
						if(nRead<=0)
							break;
						ba.append(buffer, 0, nRead);
					}
				}

				byte [] buff = ba.buffer();
				String s = "";
				
				int beginTime = 0;
				if(buff!=null)
				{
					s = new String(buff );
					String [] strArr = s.split("\n");
					if(strArr!=null)
					{
						for(int i = 0;i<strArr.length;i++)
						{
							if(strArr[i].length()>8 && strArr[i].substring(0, 8).compareToIgnoreCase("#EXTINF:") == 0 && i<strArr.length-1)
							{
								String tmp = strArr[i].substring(8);
								tmp = tmp.replace("\r", "");
								tmp = tmp.replace(",", "");
								tmp = tmp.trim();
								int duration = Integer.parseInt(tmp)*1000;
								treeMap.put(beginTime, strArr[i+1]);
								voLog.i(TAG,"Thumbnail PlayList item begin time = %d, url = %s",beginTime, strArr[i+1]);
								beginTime+=duration;
								i++;

							}
						}
						
						//add last record for not displaying last thumb nail always
						if(treeMap.size()>0)
							treeMap.put(beginTime,"");
					}
				}
			} catch (Exception e) {
				voLog.i(TAG,"Thumbnail PlayList download Exception" );
				e.printStackTrace();
			}
			//callEnabled = true;
			
		}
		//get picture url
		public synchronized String GetThumbnailByTime( int time )
		{
			String url = "";
	        Iterator<Entry<Integer, String>> tit = treeMap.entrySet().iterator();
	        while (tit.hasNext()) {
	        	Entry<Integer, String> e = tit.next();
	        	if(e.getKey()<time)
	        	{//found
	        		url = e.getValue();
	        		String pictureURL = url.toLowerCase();
					if(pictureURL.length()>0 && !(pictureURL.startsWith("http://") || pictureURL.startsWith("https://")))
					{
						url = urlPath+"/"+url;
					}
	        		break;
	        	}
	        }
	        
	        return url;
		}
	}
	/**
	 * Customer_Tag interface.
	 * 
	 * 
	 * 
	 */
	private class voCustomerTagWrap{
		private ArrayList<voOSTimedTag>	customerTagArray = new ArrayList<voOSTimedTag>();
		private	Timer 					timer = null;
		private TimerTask 				timerTask= null;//new TimerTask()
		private voOSTimedTag 			oldInfo = null;
		private int 					oldInfoIndex = -1;
		
		private voCustomerTagWrap(){
			startTimer();
		}
		private voOSTimedTag check(int timePos){
			voOSTimedTag info = null;
			int n = customerTagArray.size();
			int i = 0;
			if(oldInfo!=null && oldInfo.TimeStamp()<timePos)
				i = oldInfoIndex;
			for(;i<n;i++){
				voOSTimedTag info2 = customerTagArray.get(i);
				if(info2.TimeStamp()>timePos)
					break;
				if(i<n-1){
					if(customerTagArray.get(i+1).TimeStamp()>timePos)//found
					{
						info = info2;
						break;
					}
				}else if(i == n-1){
					info = info2;
					break;
				}
			}
			if(info !=null){
				if(info.equals(oldInfo)){
					info = null;
				}
				else{
					oldInfo = info;
					oldInfoIndex = i;
				}
			}
			return info;
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
					mEventHandler.sendEmptyMessage(messageCustomTag);
				}
			};
			
			if(timer == null)
			{
				timer = new Timer();
			}
			
			timer.schedule(timerTask, 300, 1000);
			
			voLog.v(TAG, "Start CustomerTag!");
			return true;
			
		}
		
	}
}