
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
 * @file voOSDataSource.java
 * interface of data source.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/

package com.visualon.OSMPDataSource;


import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.Parcel;

import com.visualon.OSMPUtils.voOSType.VOOSMP_SOURCE_STREAMTYPE;
import com.visualon.OSMPUtils.*;

/**
 * OSMP+ Data Source class
 * 
 * 
 * @author zhang_yi
 *
 */
public final class voOSDataSource {
	private static final String TAG = "@@@OSMPDataSource.java";



	private int 					mNativeContext; 		// accessed by native methods	
	private Context					mContext;
	private EventHandler 			mEventHandler;
	private onEventListener 		mEventListener;
	private onRequestListener 		mRequestListener;
	private static int 				eventHtmlLocalFileOK = 0x06008501;
	private static boolean  		mJNILoaded = false;
	
	private boolean					mOpenFinished;
	private boolean					mSeeking;
	//for connect control
	private static final int 		VO_PID_ADAPTIVESTREAMING_STREAMTYPE		= 0;
	
//	private static final int 		VOOSMP_SRC_PROGRAM_TYPE_LIVE		= 0;
//	private static final int 		VOOSMP_SRC_PROGRAM_TYPE_VOD		= 1;
	
	private int 					m_sProgramType = 1;
	private static final int 		VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PLAYLIST_DOWNLOADOK	= 7;
	private static final int 		VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_DOWNLOADFAIL		= 4;

	private static final int 		VO_SOURCE2_ADAPTIVESTREAMING_AUDIO		= 0;
	private static final int 		VO_SOURCE2_ADAPTIVESTREAMING_VIDEO		= 1;
	private static final int 		VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO		= 2;
	private static final int 		VO_SOURCE2_ADAPTIVESTREAMING_HEADDATA		= 3;
	private static final int 		VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE		= 4;

	private static final int 		VO_RET_SOURCE2_BASE		= 0x86000000;
	private static final int 		VO_RET_SOURCE2_NEEDRETRY					= VO_RET_SOURCE2_BASE | 0x0002;		/*!< can not finish operation, but maybe you can finish it next time */
	private static final int 		VO_RET_SOURCE2_ONECHUNKFAIL					= VO_RET_SOURCE2_BASE | 0x0012;		/*!< One chunk retry count is more than IO_CONTINUE_TOLERATE_COUNTS in IOError.h */
	private static final int 		VO_RET_SOURCE2_LINKFAIL					= VO_RET_SOURCE2_BASE | 0x0013;		/*!< The link causes failed, maybe download(checked by ErrorHandler) or other situation. */
	private boolean 				isLinkError					= false;

	
	public interface onEventListener
	{
		/**
		 * Called to indicate an event.
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
	     * <li>{@link voOSType#VOOSMP_SRC_CB_Authentication_Response}
	     * <li>{@link voOSType#VOOSMP_SRC_CB_IO_HTTP_Start_Download}
	     * <li>{@link voOSType#VOOSMP_SRC_CB_IO_HTTP_Download_Failed}
         * </ul>
		 * @param param1 first parameter code, specific to the event. Typically implementation dependant.
		 * @param param2 second parameter code, specific to the event. Typically implementation dependant.
		 * @param obj third parameter code, specific to the event. Typically implementation dependant.
		 * @return True if the method handled the info, false if it didn't.
         * Returning false,
		 */
		public int onDataSourceEvent(int id, int param1, int param2, Object obj);
	}
	
	
	/**
	 * Interface definition of a callback to be invoked to 
	 * communicate some event about the media or its playback.
	 * 
	 * 
	 * @author zhang_yi
	 *
	 */
	public interface onRequestListener
	{
		/**
		 * Called to indicate an event.
		 * 
		 * 
		 * @param id		type of the event, refer to
		 * <ul>
         * </ul>
		 * @param param1 	first parameter code, specific to the event. Typically implementation dependant.
		 * @param param2 	second parameter code, specific to the event. Typically implementation dependant.
		 * @param obj 		third parameter code, specific to the event. Typically implementation dependant.
		 * @return True if the method handled the info, false if it didn't.
         * Returning false,
		 */
		public int onRequest(int id, int param1, int param2, Object obj);
	}

		
	
    /**
     * Register a callback to be invoked when an event is available.
     *
     * @param listener the callback that will be run
     */
	public void setEventListener(onEventListener listener)
	{
		mEventListener = listener;
	}
	
	
	/**
     * Register a callback to be invoked when an event is available.
     *
     * @param listener the callback that will be run
     */
	public void setRequestListener(onRequestListener listener)
	{
		mRequestListener = listener;
	}
	
	
    private class EventHandler extends Handler
    {
        private voOSDataSource mSource;
 
        public EventHandler(voOSDataSource obj, Looper looper) 
        {
            super(looper);
            mSource = obj;
        }

        @Override 
        public void handleMessage(Message msg) 
        {    
        	//voLog.i(TAG, "handleMessage id=%d", msg.what);
            if (mSource.mNativeContext == 0)
            {
                voLog.w(TAG, "vomeplayer went away with unhandled events");
                return;
            }
    		if (msg.what == eventHtmlLocalFileOK)
        	{
				if(mNativeContext!=0)
				{
					voLog.i(TAG,"TCC download file valid" );
					String str = (String)msg.obj;
					nativeSetParam(mNativeContext, voOSType.VOOSMP_PID_SUBTITLE_FILE_NAME, str );
					if(str.contains("vo_subtitle_downloaded") && msg.arg1 == 0){
						File fl = new File(str);
						fl.deleteOnExit();
					}
				}
				voLog.i(TAG,"TCC download file ok event" );

				return ;
        		
        	}
    		if (msg.what == voOSType.VOOSMP_SRC_CB_Connection_Fail && msg.arg2 == 0x00109901){
		    	//to pause, because we need to notify ap and the engine need not to try to connect, pause it firstly to enable qp quit quickly.
		    	Pause();
    		}
            
        	if (mSource.mEventListener != null)
        		mSource.mEventListener.onDataSourceEvent (msg.what, msg.arg1, msg.arg2, msg.obj);  
        }
    }	
    
    
    /**
     * Construct
     * 
     * 
     */
	public voOSDataSource() 
	{
		Looper looper;
        if ((looper = Looper.myLooper()) != null) {
            mEventHandler = new EventHandler(this, looper);
        }else  if ((looper = Looper.getMainLooper()) != null) {
            mEventHandler = new EventHandler(this, looper);
        } else {
            mEventHandler = null;
        }
        
		mNativeContext = 0;
		mContext = null;
		
		mEventListener = null;
		mRequestListener = null;
		mOpenFinished = false;
		mSeeking = false;
		Integer nativeContext = new Integer(0);
		nativeCreate(nativeContext);
		mNativeContext = nativeContext.intValue();
	}

	/**
	 * Initial a source session and Set the Source( url etc. ) into the module
	 * For this function please only copy the pSource param, and cache the callback, do not do anything else
	 * This function should be called first for a session.
	 * @param context			current context
	 * @param packageName		where the package, hence its associated JNI libraries, resides for loading later
     * @param options			a list of Player options. 
	 * @param pSource			source file description, should be an url or fd etc.
	 * @param nSourceFlag		The flag for open source,refer to 
	 * <ul>
	 * <li>{@link voOSType#VOOSMP_FLAG_SOURCE_URL}
	 * <li>{@link voOSType#VOOSMP_FLAG_SOURCE_READBUFFER}
	 * <li>{@link voOSType#VOOSMP_FLAG_SOURCE_SENDBUFFER}
	 * <li>{@link voOSType#VOOSMP_FLAG_SOURCE_HANDLE}
	 * <li>{@link voOSType#VOOSMP_FLAG_SOURCE_OPEN_SYNC}
	 * <li>{@link voOSType#VOOSMP_FLAG_SOURCE_OPEN_ASYNC}
	 * </ul>
	 * @param nSourceType		indicate the source format, usually is {@link voOSType#VOOSMP_SRC_AUTO_DETECT}
	 * @param pInitParam		voOSInitParam type, nInitParamFlag can indicate which field is valid.
	 * @param nInitParamFlag	Initial param flag, refer to 
	 * <ul>
	 * <li>{@link voOSType#VOOSMP_FLAG_INIT_NOUSE}
	 * <li>{@link voOSType#VOOSMP_FLAG_INIT_LIBOP}
	 * <li>{@link voOSType#VOOSMP_FLAG_INIT_IO_FILE_NAME}
	 * <li>{@link voOSType#VOOSMP_FLAG_INIT_IO_API_NAME}
	 * <li>{@link voOSType#VOOSMP_FLAG_INIT_ACTUAL_FILE_SIZE}
	 * </ul>
	 * you use "|" to input several pamameter at same time.
	 * 
	 * @return {@link voOSType#VOOSMP_ERR_None} if Succeeded
	 */
	public int Init (Context context, String packageName,  List<voOSOption> options, Object pSource, int nSourceFlag, int nSourceType, voOSInitParam pInitParam, int nInitParamFlag)
	{
		mContext = context;
//		String strAPK = "/data/data/" + mContext.getPackageName() + "/";
		if(!mJNILoaded)
		{
			loadJNI(packageName);
			if(!mJNILoaded)
			{
				voLog.e(TAG, "Fail to load JNI library from Init()");
                return voOSType.VOOSMP_ERR_JNI;
			}
		}
		
		Integer nativeContext = new Integer(0);
		int nRet;
		if(pSource.getClass().getName().equalsIgnoreCase("java.lang.String"))
		{
			String s = (String)pSource;
			if(s.toLowerCase().startsWith("file:///"))
				pSource = s.substring(7);
		}

		long sourceType = nSourceType;
		long initParamFlag = nInitParamFlag;
		voLog.i(TAG, "sourceType is %d, initParamFlag is %d", nSourceType, nInitParamFlag);
		
		nRet =  nativeInit(mNativeContext, packageName, (String)pSource, nSourceFlag, sourceType, pInitParam, initParamFlag);
//		mNativeContext = nativeContext.intValue();
//		voLog.v(TAG, "osmpSource nativecontext is " + mNativeContext);
		
		return nRet;
	}
	
    /**
     * Uninitial a source session
     * 
     * @return {@link voOSType#VOOSMP_ERR_None} if Succeeded
     */
    public int Uninit()
    {
        voLog.v(TAG, "Uninit -");
//      Stop();
        int nRC = nativeUninit(mNativeContext);
        if (voOSType.VOOSMP_ERR_None == nRC) {
//            mNativeContext = 0;
        }
        
        mOpenFinished = false;
        
        voLog.v(TAG, "Uninit + , return value is " + nRC);
        return nRC;
    }

	/**
     * Destroy a source session
     * 
     * @return {@link voOSType#VOOSMP_ERR_None} if Succeeded
     */
    public int Destroy()
    {
        voLog.v(TAG, "Destroy -");
//      Stop();
        int nRC = nativeDestroy(mNativeContext);
        if (voOSType.VOOSMP_ERR_None == nRC) {
            mNativeContext = 0;
        }
        mEventListener = null;
        mRequestListener = null;    
        voLog.v(TAG, "Destroy + , return value is " + nRC);
        return nRC;
    }
    
	/**
	 * Check and analyze the stream, get information
	 * For this function, please do not start to parser the media data
	 *
	 * @return {@link voOSType#VOOSMP_ERR_None} if Succeeded
	 */
    public int Open ()
    {
    	isLinkError = false;
	    return nativeOpen (mNativeContext);
    }
    
	/**
	 * Destroy the information of the stream
	 * 
	 * @return {@link voOSType#VOOSMP_ERR_None} if Succeeded
	 */
	public int Close ()
	{
		voLog.v(TAG, "Close - ");
		mEventHandler.removeCallbacksAndMessages(null);
		int nRC =  nativeClose (mNativeContext);
		voLog.v(TAG, "Close + , return value is " + nRC);
		return nRC;
	}

	/**
	 * Start to parse the media data
	 * 
	 * @return {@link voOSType#VOOSMP_ERR_None} if Succeeded
	 */
    public int Run()
    {
    	mOpenFinished = true;
    	return nativeRun(mNativeContext);
    }
    
	/**
	 * Pause
	 * 
	 * @return {@link voOSType#VOOSMP_ERR_None} if Succeeded
	 */
	public int Pause()
	{
		return nativePause(mNativeContext);
	}
	
	/**
     * Stop the Source object
     * 
     * 
     * @return	{@link voOSType#VOOSMP_ERR_None} if Succeeded
     */
	public int Stop()
	{
		voLog.v(TAG, "Stop - ");
		mSeeking = true;
		 
		int nRC =  nativeStop(mNativeContext);
		mSeeking = false;
		voLog.v(TAG, "Stop + , return value is " + nRC);
		return nRC;
	}
	
	
	/**
	 * Set Source current position
	 * 
	 * @param nPos		seek to position value (ms) 
	 * @return			{@link voOSType#VOOSMP_ERR_None} if Succeeded.
	 */
	public int SetPos(int nPos)
	{
		mSeeking = true;
		int nRC = nativeSetPos(mNativeContext, nPos);
		mSeeking = false;
		return nRC;
	}
	
	/**
	 * Get Duration of the stream
	 * 
	 * @return 		Current source duration, if source is live type, the value is 0
	 */
	public int GetDuration()
	{
		return nativeGetDuration(mNativeContext);
	}
	
	/**
	 * Get sample from the source
	 * 
	 * @param nTrackType	The sample type of the stream, it will be audio/video/closed caption, refer to {@link VOOSMP_SOURCE_STREAMTYPE}
	 * @param nTimeStamp 	Current time stamp
	 *
	 * @return 	return value depend on different nTrackType.
	 * 			sample of the stream, all buffer in the sample( include reserve field ) will be mantained by source, they will be available until you call GetSample again
	 *          the return type is depended on the nTrackType, for {@link VOOSMP_SOURCE_STREAMTYPE#VOOSMP_SS_AUDIO} & {@link VOOSMP_SOURCE_STREAMTYPE#VOOSMP_SS_VIDEO} please use VOOSMP_BUFFERTYPE
	 *                                                         for {@link VOOSMP_SOURCE_STREAMTYPE#VOOSMP_SS_SUBTITLE} please use voSubtitleInfo
	 */
	public Object GetSample( int nTrackType, int nTimeStamp)
	{
		return nativeGetSample(mNativeContext, nTrackType, nTimeStamp);
	}
	

	
	/**
	 * It retrieve how many program in the live stream
	 * For program, it should be defined as: all sub streams that share the same angle for video and same language for audio
	 * 
	 * @return  The number of the program
	 * 
	 */
	public int GetProgramCount ()
	{
		return nativeGetProgramCount(mNativeContext);
	}

	
	/**
	 * Enum all the program info based on stream count
	 * 
	 * @param nProgram	The program sequence based on the program counts
	 * 
	 * @return  {@link voOSProgramInfo}, return the info of the Program, source will keep the info structure available until you call close
	 * 
	 */
	public synchronized Object GetProgramInfo(int nProgram)
	{
		Object ob = nativeGetProgramInfo(mNativeContext, nProgram);
		if(ob!=null)
		{
			Parcel parc = (Parcel)ob;
			parc.setDataPosition(0);
			voOSProgramInfo info = new voOSProgramInfoImpl();
			info.parse(parc);
			parc.recycle();
			ob = info;
		}
		return ob;
	}

	
	/**
	 * Get the track info we selected
	 * 
	 * @param nTrackType 	The sample type of the stream, it will be audio/video/closed caption,
	 * 						refer to {@link VOOSMP_SOURCE_STREAMTYPE}
	 * @return 				{@link voOSTrackInfo} of the selected track. The trackinfo memory will be mantained in source until you call close
	 * 
	 */
	public Object GetCurTrackInfo(int nTrackType)
	{
		Object ob = nativeGetCurTrackInfo(mNativeContext, nTrackType);
		if(ob!=null)
		{
			Parcel parc = (Parcel)ob;
			parc.setDataPosition(0);
			voOSTrackInfo info = new voOSTrackInfoImpl();
			info.parse(parc);
			parc.recycle();
			ob = info;
		}
		return ob;
	}
	
	
	/**
	 * Select the Program
	 * 
	 * @param nProgram [in] Program ID
	 * @return {@link voOSType#VOOSMP_ERR_None} if success
	 */
	public int SelectProgram (int nProgram)
	{
		return nativeSelectProgram(mNativeContext, nProgram);
	}
	
	/**
	 * Select the Stream
	 * 
	 * @param nStream [in] Stream ID
	 * @return {@link voOSType#VOOSMP_ERR_None} if Succeeded
	 */
	public int SelectStream(int nStream)
	{
		return nativeSelectStream(mNativeContext, nStream);
	}
	
	/**
	 * Select the Stream Track
	 * 
	 * @param nTrackID 	Track to be selected
	 * @return 	{@link voOSType#VOOSMP_ERR_None} if Succeeded
	 */
	public int SelectTrack (int nTrackID)
	{
		return nativeSelectTrack(mNativeContext, nTrackID);
	}

	/**
	* Send the buffer into the source, if you want to use this function, you should use flag {@link voOSType#VOOSMP_FLAG_SOURCE_SENDBUFFER}
	* 
	* @param pBuffer 	The buffer pointer send to the source
	* @return {@link voOSType#VOOSMP_ERR_None} if Succeeded
	*/
	public int SendBuffer ( int pBuffer)
	{
		return nativeSendBuffer(mNativeContext, pBuffer);
	}

	
	 /**
     * Get parameter for player
     * 
     * @param      	id	type of the parameter
     * <ul>
     * <li>{@link voOSType#VOOSMP_SRC_PID_DVRINFO}
	 * <li>{@link voOSType#VOOSMP_SRC_PID_BA_WORKMODE}
	 * <li>{@link voOSType#VOOSMP_SRC_PID_RTSP_STATUS}
	 * <li>{@link voOSType#VOOSMP_SRC_PID_FUNC_READ_BUF}
	 * <li>{@link voOSType#VOOSMP_SRC_PID_COMMAND_STRING}
     * </ul>
     * @return    	return value depend on different id.
     */ 
	public Object GetParam(int id)
	{
		switch (id){
		case voOSType.VOOSMP_SRC_PID_DVRINFO:
			long[] dvrInfo =  (long[]) nativeGetParam(mNativeContext, id);
			if (dvrInfo == null)
				return null;
			
			voOSDVRInfoImpl info = new voOSDVRInfoImpl(dvrInfo[0], dvrInfo[1], dvrInfo[2], dvrInfo[3]);
			return info;
		case voOSType.VOOSMP_SRC_PID_RTSP_STATUS:
			int[] arrRTSPStatus =  (int[]) nativeGetParam(mNativeContext, id);
			if (arrRTSPStatus == null){
				voLog.v(TAG,"can not get RTSPStatus");
				return null;
			}
			
			
			VOOSMP_SOURCE_STREAMTYPE nTrackType = VOOSMP_SOURCE_STREAMTYPE.valueOf(arrRTSPStatus[0]);
			voOSRTSPStatusImpl status = new voOSRTSPStatusImpl(
					nTrackType, arrRTSPStatus[1], arrRTSPStatus[2],
					arrRTSPStatus[3],arrRTSPStatus[4],arrRTSPStatus[5],arrRTSPStatus[6]);
			
			voLog.i(TAG,"RTSPStatus-nTrackType:"+nTrackType);
			voLog.i(TAG,"RTSPStatus-uPacketRecved:"+arrRTSPStatus[1]);
			voLog.i(TAG,"RTSPStatus-uPacketDuplicated:"+arrRTSPStatus[2]);
			voLog.i(TAG,"RTSPStatus-uPacketLost:"+arrRTSPStatus[3]);
			voLog.i(TAG,"RTSPStatus-uPacketSent:"+arrRTSPStatus[4]);
			voLog.i(TAG,"RTSPStatus-uAverageJitter:"+arrRTSPStatus[5]);
			voLog.i(TAG,"RTSPStatus-uAverageLatency:"+arrRTSPStatus[6]);
			return status;
		case voOSType.VOOSMP_SRC_PID_BA_WORKMODE:
			Integer Int = (Integer)nativeGetParam(mNativeContext, id);
			if (Int == null)
			{
				voLog.e(TAG, "GetParam VOOSMP_SRC_PID_BA_WORKMODE, id = %s, occur error, return null",
						Integer.toHexString(id));
				return voOSType.VOOSMP_SRC_ADAPTIVE_STREAMING_BA_MODE_MANUAL;
			}else
			{
				return Int;
			}
		}
		
		return nativeGetParam(mNativeContext, id);
	}
	
	
	/**
     * Set parameter for player
     * 
     * @param	id	type of the parameter, refer to
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
     * <li> {@link voOSType#VOOSMP_SRC_PID_BITRATE_THRESHOLD}
     * </ul>
     * @param 	param depend on different id.
     * 
     * @return {@link voOSType#VOOSMP_ERR_None} if Succeeded
     */ 
	public int SetParam(int id, Object param)
	{
		if (id == voOSType.VOOSMP_PID_PERFORMANCE_DATA
			|| id == voOSType.VOOSMP_SRC_PID_BA_STARTCAP)
		{
			int[] perfData = new int[10];
			voOSPerformanceData PerformanceData = (voOSPerformanceData)param;
			perfData[0] = PerformanceData.CodecType();
			perfData[1] = PerformanceData.BitRate();
			perfData[2] = PerformanceData.VideoWidth();
			perfData[3] = PerformanceData.VideoHeight();
			perfData[4] = PerformanceData.ProfileLevel();
			perfData[5] = PerformanceData.FPS();
			return nativeSetParam(mNativeContext, id, perfData);
		}
		
		if (id == voOSType.VOOSMP_SRC_PID_HTTP_HEADER)
		{
			String[] arr = new String[2];
			voOSHTTPHeader header = (voOSHTTPHeader)param;
			arr[0] = header.getHeaderName();
			arr[1] = header.getHeaderValue();
			return nativeSetParam(mNativeContext, id, arr);
		}
		
		if (id == voOSType.VOOSMP_SRC_PID_RTSP_CONNECTION_PORT)
		{
			int[] arr = new int[2];
			voOSConnectPortInfoImpl portInfo = (voOSConnectPortInfoImpl)param;
			voLog.e(TAG, "SetParam VOOSMP_SRC_PID_RTSP_CONNECTION_PORT, audio is %d video is %d ", portInfo.AudioConnectPort(),portInfo.VideoConnectPort());
			arr[0] = portInfo.AudioConnectPort();
			arr[1] = portInfo.VideoConnectPort();
			return nativeSetParam(mNativeContext, id, arr);
		}
		
		if (id == voOSType.VOOSMP_SRC_PID_BITRATE_THRESHOLD)
		{
			int[] arr = new int[2];
			voOSSrcBAThreshold hold = (voOSSrcBAThreshold)param;
			arr[0] = hold.getUpper();
			arr[1] = hold.getLower();
			return nativeSetParam(mNativeContext, id, arr);
		}
		

		if (id == voOSType.VOOSMP_SRC_PID_Timed_Text_ClosedCaption || id == voOSType.VOOSMP_PID_SUBTITLE_FILE_NAME)
		{
			String url1 = (String)param;
			String urlLow = url1.toLowerCase();
			final long id1 = voOSType.VOOSMP_PID_SUBTITLE_FILE_NAME;
			
			if(url1.length()<=0)
				return 0;
			if(!urlLow.startsWith("http")){
				if(urlLow.startsWith("file:///"))
					url1 = url1.substring(7);
				Message m = mEventHandler.obtainMessage(eventHtmlLocalFileOK, 1, 0, url1);//local file
				mEventHandler.sendMessage(m);
				return 0;
			}
			//nativeSetParam(mNativeContext, voOSType.VOOSMP_PID_SUBTITLE_FILE_NAME, param);
			
			final String url = url1;
			new Thread(new Runnable(){
				
				@Override
				public void run() {
				try {
						//String url =  "http://www.cbs.com/closedcaption/CBS_BOLD_6360_ENG_CONTENT_CIAN_caption.xml";//"http://www.cbs.com/closedcaption/CBS_BOLD_6360_ENG_CONTENT_CIAN_caption.xml"
						URL urlLink = new URL(url);
						voLog.i(TAG,"TCC download begin with %s", url );
						InputStream in = new BufferedInputStream(urlLink.openStream());
						voLog.i(TAG,"TCC download after new BufferedInputStream" );
						if(in!=null)
						{
							{
								String url2 = "";///mnt/sdcard/vo_ttml_downloaded.xml";
								String urlTemp = url;
								String [] arrStr = urlTemp.split("\\.");
								String extFile = ".xml";
								if(arrStr.length>0){
									String sLow =  arrStr[arrStr.length-1].toLowerCase();
									if(!sLow.startsWith("xml"))
										extFile = "." + arrStr[arrStr.length-1];
									
								}
								voLog.i(TAG,"TCC download before createTempFile" );
								   ///data/data/com.visualon.osmpSamplePlayer/files/
								File fl =  File.createTempFile("vo_subtitle_downloaded01", extFile,mContext.getFilesDir());
								//File fl = File.createTempFile("vo_subtitle_downloaded", extFile); //new File(url2);
								voLog.i(TAG,"TCC download after createTempFile" );
								
								if(fl!=null)
								{
									url2 = fl.getAbsolutePath();
									
									voLog.i(TAG,"TCC download before new FileOutputStream" );
									FileOutputStream  fos = new FileOutputStream(url2);
									
									byte [] bt = new byte[1024];
									int all = 0;
									while(true)
									{
										int n = in.read(bt);
										if(n<=0)
											break;
										all+=n;
										voLog.i(TAG,"TCC download read=%d", all );
										fos.write(bt, 0, n);
									}
									
									fos.flush();
									fos.close();
									voLog.i(TAG,"TCC download ok; url=%s, len:=%d", url2, all );
									Message m = mEventHandler.obtainMessage(eventHtmlLocalFileOK, 0, 0, url2);
									mEventHandler.sendMessage(m);
								}
								else
									voLog.i(TAG,"TCC, Can't create ttml temp local file" );
								
		
							}
							
						}
						} catch (IOException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
							voLog.i(TAG,"TCC download error" );
						}  
					}
				
			}).start();
			return 0;
		}
		return nativeSetParam(mNativeContext, id, param);
	}
	
	
	/**
	 * Get Subtitle Language Information
	 * 
	 * @return Subtitle Language Information {@link voOSSubtitleLanguage}. 
	 * 
	 */
	
	public List<voOSSubtitleLanguage> GetSubtitleLanguageInfo()
	{
		if (mNativeContext == 0)
		{
			voLog.w(TAG, "VOOSMP_ERR_Uninitialize");
			return null;
		}
		int nCount;
		
		nCount = nativeGetSubtitleLanguageCount(mNativeContext);
		if (nCount == 0)
			return null;
		
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
			voLog.w(TAG, "VOOSMP_SRC_ERR_SOURCE_UNINITIALIZE");
			return voOSType.VOOSMP_SRC_ERR_SOURCE_UNINITIALIZE;
		}
		
		return nativeSelectSubtitleLanguage(mNativeContext, nIndex);
	}
	

	private void osmpCallBack(int nID, int param1, int param2, Object arg0)
	{
		voLog.v(TAG, "DataSource CallBack, nID = " + Integer.toHexString(nID));
		Object argRtn = CheckCallBackParam( nID, param1, param2, arg0);
		if(argRtn!=null)
			arg0 = argRtn;
		Message m = mEventHandler.obtainMessage(nID, param1, param2, arg0);
		mEventHandler.sendMessage(m);
		
	}
	private Object CheckCallBackParam(int nID, int param1, int param2, Object arg0)
	{
//		voLog.v(TAG, "CheckCallBackParam, nID = " + Integer.toHexString(nID));
		if(voOSType.VOOSMP_SRC_CB_Adaptive_Streaming_Info == nID)
		{
			if(param1 == voOSType.VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PROGRAM_TYPE)
			{
				m_sProgramType = param2;
				voLog.i(TAG, "EventHandling, VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PROGRAM_TYPE is %d", m_sProgramType);
			}
		}
		
		if(arg0 == null)
			return null;
		
		if(voOSType.VOOSMP_SRC_CB_Adaptive_Streaming_Info == nID
				||voOSType.VOOSMP_SRC_CB_Adaptive_Stream_Warning == nID){

			if(voOSType.VOOSMP_SRC_CB_Adaptive_Stream_Warning == nID || param1 == voOSType.VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_BEGINDOWNLOAD ||
					param1 == voOSType.VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DROPPED ||
					param1 == voOSType.VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DOWNLOADOK){
				Parcel parc = (Parcel)arg0;
				parc.setDataPosition(0);
				int nType = parc.readInt();
	        	byte[] dt = null;
	        	int uSize = 2048;
	        	dt = new byte[uSize];
        		parc.readByteArray(dt);
        		byte[] dt2 = new byte[uSize];
        		parc.readByteArray(dt2);
				long ullStartTime = parc.readLong();
                int mPeriodSequenceNumber = parc.readInt();
				long ullDuration = parc.readLong();
				long ullTimeScale = parc.readLong();
				int uReserved1 = parc.readInt();
				int uReserved2 = parc.readInt();
				return new voOSChunkInfoImpl(nType, new String(dt), new String(dt2),
						ullStartTime, mPeriodSequenceNumber, ullDuration, ullTimeScale, uReserved1,
						uReserved2);
			}

			if(param1 == voOSType.VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_DISCONTINUE_SAMPLE){
				Parcel parc = (Parcel)arg0;
				parc.setDataPosition(0);
				int uFlag = parc.readInt();
				long ullChunkStartTime = parc.readLong();
				int uPeriodSequenceNumber = parc.readInt();
				long ullPeriodFirstChunkStartTime = parc.readLong();
				long ullSampleTime = parc.readLong();
				int uSampleTimePointer = parc.readInt();
				return new voOSChunkSampleImpl(uFlag, ullChunkStartTime, uPeriodSequenceNumber, ullPeriodFirstChunkStartTime, ullSampleTime, uSampleTimePointer);
			}
		}
		
		if(voOSType.VOOSMP_SRC_CB_Customer_Tag == nID){
			Parcel parc = (Parcel)arg0;
			parc.setDataPosition(0);
			long ullTimeStamp = parc.readLong();
			int uSize = parc.readInt();
        	byte[] dt = null;
        	if(uSize<=0)
        		dt = new byte[1];
        	else
        		dt = new byte[uSize];
        	if(uSize>0)
        		parc.readByteArray(dt);
			int uFlag = parc.readInt();

			return new voOSTimedTagImpl(ullTimeStamp, uSize, dt,
					uFlag, null);
		}

		if(voOSType.VOOSMP_SRC_CB_DRM_Init_Data == nID){
			Parcel parc = (Parcel)arg0;
			parc.setDataPosition(0);
			int uSize = parc.readInt();
			byte[] dt = null;
            if (uSize > 0) {
                dt = new byte[uSize];
                parc.readByteArray(dt);
                voLog.d(TAG,"CheckCallBackParam  uSize = " + uSize + "  dt = " + (new String(dt)));
            }
            return new voOSDRMInit(param1, dt);
		}
		
        if (nID == voOSType.VOOSMP_SRC_CB_IO_HTTP_Download_Failed
        		|| nID == voOSType.VOOSMP_SRC_CB_IO_HTTP_Start_Download)
        {
        	Object object;
        	
        	Parcel parc = (Parcel)arg0;
        	voOSIOFailedDescImpl impl = new voOSIOFailedDescImpl();
        	if (parc != null)
        		impl.parse(parc);
        	
        	if ( nID == voOSType.VOOSMP_SRC_CB_IO_HTTP_Download_Failed)
        	{
        		object = impl;
        		voLog.i(TAG, " DataSource handleRequest, VOOSMP_SRC_CB_IO_HTTP_Download_Failed, ID is %s, reason is %s, response is %s, url is %s", Integer.toHexString(nID),
            			voOSIOFailedDesc.VOOSMP_SRC_IO_DOWNLOAD_FAIL_REASON.valueOf(impl.getReason().name()),
            			impl.getResponse(), impl.getURL());
        	}
        	else
        	{
        		object = impl.getURL();
        		voLog.i(TAG, " DataSource handleRequest, VOOSMP_SRC_CB_IO_HTTP_Start_Download, ID is %s, url is %s", Integer.toHexString(nID),
            			impl.getURL());
        	}
        	
        	return object;
        	
        }

		return null;
	}
	public int RequestCallBack(int nID, int param1, int param2, Object arg0)
	{
		voLog.v(TAG," onRequestCallBack, nID is %s, param1 is %d, param2 is %d,", Integer.toHexString(nID), param1, param2);
		Object argRtn = CheckCallBackParam( nID, param1, param2, arg0);
		if(argRtn!=null)
			arg0 = argRtn;
		
        if(voOSType.VOOSMP_SRC_CB_Adaptive_Stream_Warning == nID
				||voOSType.VOOSMP_SRC_CB_Adaptive_Streaming_Info == nID
				||voOSType.VOOSMP_SRC_CB_Adaptive_Streaming_Error == nID
				){
        	 if(mEventHandler!=null)
             {
        		 Message m = mEventHandler.obtainMessage(nID, param1, param2, arg0);
        		 mEventHandler.sendMessage(m);//to send message asyn
             }
		}

        if (mRequestListener != null)
    		return mRequestListener.onRequest(nID, param1, param2, arg0);
    	else
    		return voOSType.VOOSMP_SRC_ERR_SOURCE_UNINITIALIZE;
		
	}

	static {
	     voLog.v(TAG, "Befor loadLibrary, voOSSource_OSMP");
		try {
			 System.loadLibrary("vodl");
		     System.loadLibrary("voOSSource_OSMP");
			 mJNILoaded  = true;
			 voLog.v(TAG, "after loadLibrary, voOSSource_OSMP");
		} catch (UnsatisfiedLinkError e) {
			// TODO Auto-generated catch block
			mJNILoaded = false;
			e.printStackTrace();
		}
	}
	
	private static void loadJNI(String libPath)
	{
		String[] libs = {"vodl","voOSSource_OSMP"};
		try {
			for (int i = 0; i < libs.length; i++) {
				System.load(libPath + libs[i]);
			}
			 		mJNILoaded  = true; 
		} catch (Exception  e) {
			
			mJNILoaded = false;
			e.printStackTrace();
			
			// TODO: handle exception
		}
	}

	private native int nativeCreate(Integer context);
	private native int nativeInit(int context, String apkPath, String pSource, int nFlag, long nSourceType, voOSInitParam initParam, long jinitFlag);
	private native int nativeUninit(int context);
	private native int nativeDestroy(int context);
	private native int nativeOpen(int context);
	private native int nativeRun(int context);
	private native int nativePause(int context);
	private native int nativeStop(int context);
	private native int nativeClose(int context);
	private native int nativeFlush(int context);
	private native int nativeGetDuration(int context);
	private native int nativeSetPos(int context, int nPos);
	private native Object nativeGetSample (int context, int nTrackType, int nTimeStamp);
	private native int nativeGetProgramCount (int context);
	private native Object nativeGetProgramInfo(int context, int nStream);
	private native Object nativeGetCurTrackInfo (int context, int  nTrackType);
	private native int nativeSelectProgram ( int context, int nProgramID);
	private native int nativeSelectStream ( int context, int nStreamID );
	private native int nativeSelectTrack ( int context, int nTrackID);
	private native int nativeSendBuffer ( int context, int pBuffer);
	private native Object nativeGetParam(int context, int id);
	private native int nativeSetParam(int context, int id, Object param);
	private native int nativeGetSubtitleLanguageCount(int context);
	private native Object nativeGetSubtitleLanguageInfo(int context, int nIndex);
	private native int nativeSelectSubtitleLanguage(int context, int nIndex);
	
	private native int nativeGetVideoCount(int context);
	private native int nativeGetAudioCount(int context);
	private native int nativeGetSubtitleCount(int context);
	private native int nativeSelectVideo(int context, int nIndex);
	private native int nativeSelectAudio(int context, int nIndex);
	private native int nativeSelectSubtitle(int context, int nIndex);
	private native boolean nativeIsVideoAvailable (int context, int nIndex);
	private native boolean nativeIsAudioAvailable (int context, int nIndex);
	private native boolean nativeIsSubtitleAvailable(int context, int nIndex);
	private native int nativeCommitSelection (int context);
	private native int nativeClearSelection (int context);
	private native int nativeGetPropertyCount (int context, int nType, int nIndex);
	private native String nativeGetPropertyKeyName (int context, int nType, int nIndex, int nKeyIndex);
	private native String nativeGetPropertyValue (int context, int nType, int nIndex, int nKeyIndex);
	private native int nativeGetCurrTrackSelection(int context, Integer nCurrentVideoIndex, 
			Integer nCurrentAudioIndex, Integer nCurrentSubtitleIndex);
	private native int nativeGetCurrTrackPlaying(int context, Integer nCurrentVideoIndex, 
			Integer nCurrentAudioIndex, Integer nCurrentSubtitleIndex);	   
	
	
	/**
	 * Get video track count
	 * 
	 * @return count of video track
	 */
    public int GetVideoCount()
    {
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return voOSType.VOOSMP_ERR_Uninitialize;
		}
		
    	return nativeGetVideoCount(mNativeContext);
    }
    
 	/**
	 * Get audio track count
	 * 
	 * @return count of audio track
	 */
    public int GetAudioCount()
    {
    	if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return voOSType.VOOSMP_ERR_Uninitialize;
		}
    	
    	return nativeGetAudioCount(mNativeContext);
    	
    }
    
 	/**
	 * Get subtitle track count
	 * 
	 * @return count of subtitle track
	 */
    public int GetSubtitleCount()
    {
    	if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return voOSType.VOOSMP_ERR_Uninitialize;
		}
    	
    	return nativeGetSubtitleCount(mNativeContext);
    }
    
 	/**
	 * Select video track
	 * 
     * @param nIndex The index of track
	 * @return VOOSMP_ERR_None Succeeded
	 */
    public int SelectVideo (int nIndex)
    {
    	if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return voOSType.VOOSMP_ERR_Uninitialize;
		}
    	
    	return nativeSelectVideo(mNativeContext, nIndex);
    }
    
 	/**
	 * Select audio track
	 * 
     * @param nIndex  The index of track
	 * @return VOOSMP_ERR_None Succeeded
	 */
    public int SelectAudio(int nIndex)
    {
    	if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return voOSType.VOOSMP_ERR_Uninitialize;
		}
    	return nativeSelectAudio(mNativeContext, nIndex);
    	
    }
    
  	/**
	 * Select subtitle track
	 * 
     * @param nIndex The index of track
	 * @return VOOSMP_ERR_None Succeeded
	 */
    public int SelectSubtitle ( int nIndex)
    {
    	if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return voOSType.VOOSMP_ERR_Uninitialize;
		}
    	return nativeSelectSubtitle(mNativeContext, nIndex);
    	
    }
   
    /**
	 * Check if video track is available
	 * 
     * @param nIndex The index of track
	 * @return true Succeeded
	 */
    public boolean IsVideoAvailable (int nIndex)
    {
    	if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return false;
		}
    	
    	return nativeIsVideoAvailable(mNativeContext, nIndex);

    }
    
    /**
	 * Check if audio track is available
	 * 
     * @param nIndex The index of track
	 * @return true Succeeded
	 */
    public boolean IsAudioAvailable (int nIndex)
    {
    	if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return false;
		}
    	
    	return nativeIsAudioAvailable(mNativeContext, nIndex);
    }
    
    /**
	 * Check if subtitle track is available
	 * 
     * @param nIndex [in] The index of track
	 * @return true Succeeded
	 */
    public boolean IsSubtitleAvailable( int nIndex)
    {
    	if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return false;
		}
    	
    	return nativeIsSubtitleAvailable(mNativeContext, nIndex);
    }
    
    /**
	 * Commit all current selections
	 * 
     * @return VOOSMP_ERR_None Succeeded
	 */
    public int CommitSelection ()
    {
    	if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return voOSType.VOOSMP_ERR_Uninitialize;
		}
    	return nativeCommitSelection(mNativeContext);
    }
    
    /**
	 * Remove all current uncommitted selections
	 * 
     * @return VOOSMP_ERR_None Succeeded
	 */
    public int ClearSelection ()
    {
    	if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return voOSType.VOOSMP_ERR_Uninitialize;
		}
    	return nativeClearSelection(mNativeContext);
    }

    /**
	 * Get the properties count 0f relevant assets.
	 * 
     * @param nType Assets type, value refer to {@link VOOSMP_SOURCE_STREAMTYPE}
     *        2 for video, 1 for audio, 9 for subtitle
     * @param nIndex relevant assets index of track
     * 
     * @return count of relevant assets property. -1 for errors
	 */
    public int GetPropertyCount (int nType, int nIndex)
    {
    	if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return voOSType.VOOSMP_ERR_Uninitialize;
		}
    	int n = nativeGetPropertyCount(mNativeContext, nType, nIndex);
        voLog.i(TAG, "h265 test GetPropertyCount: %d, nType=%d, nIndex=%d", n,nType, nIndex);;
    	return n;
    }
    
    /**
	 * Get the properties key name 0f relevant assets.
	 * 
     * @param nType Assets type, value refer to {@link VOOSMP_SOURCE_STREAMTYPE}
     *        2 for video, 1 for audio, 9 for subtitle
     * @param nIndex relevant assets index of track
     * @param nKeyIndex relevant index key name
     * 
     * @return key name of relevant assets property. null for errors
	 */
    public String GetPropertyKeyName (int nType, int nIndex, int nKeyIndex)
    {
    	if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return null;
		}
    	return nativeGetPropertyKeyName(mNativeContext, nType, nIndex, nKeyIndex);
    }
    
    /**
	 * Get the properties value 0f relevant assets.
	 * 
     * @param nType Assets type, value refer to {@link VOOSMP_SOURCE_STREAMTYPE}
     *        2 for video, 1 for audio, 9 for subtitle
     * @param nIndex relevant assets index of track
     * @param nKeyIndex relevant index key name
     * 
     * @return value of relevant assets property. null for errors
	 */
    public String GetPropertyValue (int nType, int nIndex, int nKeyIndex)
    {
    	if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return null;
		}
    	return nativeGetPropertyValue(mNativeContext, nType, nIndex, nKeyIndex);
    }
    
    /**
	 * Retrieve the assets which is currently being played or default assets before playing
	 * 
	 * @param nCurrentVideoIndex Current selected video index
     * @param nCurrentAudioIndex Current selected audio index
     * @param nCurrentSubtitleIndex Current selected subtitle index
     * 
     * @return VOOSMP_ERR_None Succeeded
	 */
    public int GetCurrTrackSelection(Integer nCurrentVideoIndex, Integer nCurrentAudioIndex, Integer nCurrentSubtitleIndex)
    {
    	if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return voOSType.VOOSMP_ERR_Uninitialize;
		}
    	return nativeGetCurrTrackSelection(mNativeContext, nCurrentVideoIndex, nCurrentAudioIndex, nCurrentSubtitleIndex);
    }

	
    /**
	 * Retrieve the assets which is currently being played
	 * 
	 * @param nCurrentVideoIndex Current selected video index
     * @param nCurrentAudioIndex Current selected audio index
     * @param nCurrentSubtitleIndex Current selected subtitle index
     * 
     * @return VOOSMP_ERR_None Succeeded
	 */
    public int GetCurrTrackPlaying(Integer nCurrentVideoIndex, Integer nCurrentAudioIndex, Integer nCurrentSubtitleIndex)
    {
    	if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return voOSType.VOOSMP_ERR_Uninitialize;
		}
    	return nativeGetCurrTrackPlaying(mNativeContext, nCurrentVideoIndex, nCurrentAudioIndex, nCurrentSubtitleIndex);
    }
}
