package com.visualon.VOOSMPStreamingDownloaderImpl;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;

import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_LANGUAGE_TYPE;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;
import com.visualon.OSMPUtils.voLog;
import com.visualon.OSMPUtils.voOSType;
import com.visualon.VOOSMPStreamingDownloader.VOOSMPStreamingDownloader;
import com.visualon.VOOSMPStreamingDownloader.VOOSMPStreamingDownloaderInitParam;
import com.visualon.VOOSMPStreamingDownloader.VOOSMPStreamingDownloaderListener;
import com.visualon.VOOSMPStreamingDownloader.VOOSMPStreamingDownloaderListener.VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID;
import com.visualon.OSMPPlayerImpl.*;

public class VOOSMPStreamingDownloaderImpl implements VOOSMPStreamingDownloader {
	
	private static final String TAG = "@@@VOOSMPStreamingDownloaderImpl";
	
	private VOOSMPStreamingDownloaderInitParam mInitParam;
	private VOOSMPStreamingDownloaderListener mListener;
	private int mNativeContext;
	
	private static boolean 			mJNILoaded = false;
	private EventHandler 			mEventHandler;
	
	private class EventHandler extends Handler
    {
        public EventHandler( Looper looper) 
        {
            super(looper);
        }
        
        public void handleMessage(Message msg) 
        {   
        	if (mListener != null)
        	{
        		mListener.onVOStreamingDownloaderEvent(
        				VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID.valueOf(msg.what), 
        				msg.arg1, msg.arg2, msg.obj);
        	}
        		
        }
    }
	
	public VOOSMPStreamingDownloaderImpl() {
		super();
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
        
        mNativeContext = 0;
		mInitParam = null;
		mListener = null;
	}

	@Override
	public VO_OSMP_RETURN_CODE init(VOOSMPStreamingDownloaderListener listener,
			VOOSMPStreamingDownloaderInitParam initParam) {
		// TODO Auto-generated method stub
		mNativeContext = 0;
		
		mInitParam = initParam;
		mListener = listener;
		
		if (mInitParam == null || mInitParam.getContext() == null 
				|| mInitParam.getLibraryPath() == null || mListener == null)
		{
			voLog.e(TAG, "init parameter is invalid, init failed!");
			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_PARAMID;
		}
		
		voLog.i(TAG, "VOOSMPStreamingDownloader init, libraryPath is %s", mInitParam.getLibraryPath());
		
		if(!mJNILoaded)
		{
			loadJNI(mInitParam.getLibraryPath());
			if(!mJNILoaded)
			{
				voLog.e(TAG, "Fail to load JNI library from Init()");
                return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_JNI;
			}
		}
		
		Integer nativeContext = new Integer (0);
		int ret = nativeInit(nativeContext, mInitParam);
		if (ret < 0 || nativeContext == 0)
		{
			voLog.e(TAG, "Init failed.");
			return VO_OSMP_RETURN_CODE.valueOf(ret);
		}else
		{
			mNativeContext = nativeContext;
			voLog.i(TAG, "VOOSMPStreamDownloader module init success! mNaiveContext is " + mNativeContext);
			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
		}
	}
	
	@Override
	public VO_OSMP_RETURN_CODE destroy() {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
		}
		
		voLog.i(TAG, "VOOSMPStreamingDownloader destroy -.");
		
		int ret = nativeUnInit(mNativeContext);
		mInitParam = null;
		mListener = null;
		mNativeContext = 0;;
		voLog.i(TAG, "VOOSMPStreamingDownloader destroy +.");
		return VO_OSMP_RETURN_CODE.valueOf(ret);
	}

	@Override
	public VO_OSMP_RETURN_CODE open(String source, int flag, String localDir) {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
		}
		
		voLog.i(TAG, "VOOSMPStreamingDownloader open, source is %s, type is %s, localDir is %s", source, Integer.toString(flag), localDir);
		
		int ret = nativeOpen(mNativeContext, source, flag, localDir);
		
		return VO_OSMP_RETURN_CODE.valueOf(ret);
	}

	@Override
	public VO_OSMP_RETURN_CODE close() {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
		}
		
		voLog.i(TAG, "VOOSMPStreamingDownloader close.");
		
		int ret = nativeClose(mNativeContext);
		return VO_OSMP_RETURN_CODE.valueOf(ret);
	}

	@Override
	public VO_OSMP_RETURN_CODE start() {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
		}
		
		voLog.i(TAG, "VOOSMPStreamingDownloader start.");
		
		int ret = nativeStart(mNativeContext);
		
		return VO_OSMP_RETURN_CODE.valueOf(ret);
	}

	@Override
	public VO_OSMP_RETURN_CODE stop() {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
		}
		
		voLog.i(TAG, "VOOSMPStreamingDownloader stop.");
		
		int ret = nativeStop(mNativeContext);
		
		return VO_OSMP_RETURN_CODE.valueOf(ret);
	}

	@Override
	public VO_OSMP_RETURN_CODE pause() {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
		}
		
		voLog.i(TAG, "VOOSMPStreamingDownloader pause.");
		
		int ret = nativePause(mNativeContext);
		
		return VO_OSMP_RETURN_CODE.valueOf(ret);
	}

	@Override
	public VO_OSMP_RETURN_CODE resume() {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
		}
		
		voLog.i(TAG, "VOOSMPStreamingDownloader resume.");
		
		int ret = nativeResume(mNativeContext);
		
		return VO_OSMP_RETURN_CODE.valueOf(ret);
	}


	@Override
	public VO_OSMP_RETURN_CODE clearSelection() {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
		}
		
    	int ret = nativeClearSelection(mNativeContext);
		return VO_OSMP_RETURN_CODE.valueOf(ret);
	}

	@Override
	public VO_OSMP_RETURN_CODE commitSelection() {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
		}
		
    	int ret = nativeCommitSelection(mNativeContext);
		return VO_OSMP_RETURN_CODE.valueOf(ret);
	}

	@Override
	public int getAudioCount() {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return voOSType.VOOSMP_ERR_Uninitialize;
		}
    	
    	return nativeGetAudioCount(mNativeContext);
    
	}

	@Override
	public VOOSMPAssetProperty getAudioProperty(int arg0) {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return null;
		}
		
		VOOSMPAssetProperty property = new VOOSMPAssetPropertyImpl(nativeGetAudioProperty(mNativeContext, arg0));
		return property;
	}

	@Override
	public VOOSMPAssetIndex getCurrentSelection() {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return null;
		}
		
    	VOOSMPAssetIndex index = new VOOSMPAssetIndexImpl(nativeGetCurrentSelection(mNativeContext));
    	return index;
	}

	@Override
	public VOOSMPAssetIndex getPlayingAsset() {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return null;
		}
		
		VOOSMPAssetIndex index = new VOOSMPAssetIndexImpl(nativeGetPlayingAsset(mNativeContext));
    	return index;
	}

	@Override
	public int getSubtitleCount() {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return voOSType.VOOSMP_ERR_Uninitialize;
		}
    	
    	return nativeGetSubtitleCount(mNativeContext);
	}

	@Override
	public VOOSMPAssetProperty getSubtitleProperty(int arg0) {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return null;
		}
		
		VOOSMPAssetProperty property = new VOOSMPAssetPropertyImpl(nativeGetSubtitleProperty(mNativeContext, arg0));
		return property;
	}

	@Override
	public int getVideoCount() {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return voOSType.VOOSMP_ERR_Uninitialize;
		}
		
    	return nativeGetVideoCount(mNativeContext);
	}

	@Override
	public VOOSMPAssetProperty getVideoProperty(int arg0) {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return null;
		}
	
    	VOOSMPAssetProperty property = new VOOSMPAssetPropertyImpl(nativeGetVideoProperty(mNativeContext, arg0));
		return property;
	}

	@Override
	public boolean isAudioAvailable(int arg0) {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return false;
		}
    	
    	return nativeIsAudioAvailable(mNativeContext, arg0);
	}

	@Override
	public boolean isSubtitleAvailable(int arg0) {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return false;
		}
    	
    	return nativeIsSubtitleAvailable(mNativeContext, arg0);
	}

	@Override
	public boolean isVideoAvailable(int arg0) {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return false;
		}
    	
    	return nativeIsVideoAvailable(mNativeContext, arg0);
	}

	@Override
	public VO_OSMP_RETURN_CODE selectAudio(int arg0) {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
		}
    	
		int ret = nativeSelectAudio(mNativeContext, arg0);
    	return VO_OSMP_RETURN_CODE.valueOf(ret);
	}

	@Override
	public VO_OSMP_RETURN_CODE selectSubtitle(int arg0) {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
		}
    	
		int ret = nativeSelectSubtitle(mNativeContext, arg0);
    	return VO_OSMP_RETURN_CODE.valueOf(ret);
	}

	@Override
	public VO_OSMP_RETURN_CODE selectVideo(int arg0) {
		// TODO Auto-generated method stub
		if (mNativeContext == 0)
		{
			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
		}
    	
		int ret = nativeSelectVideo(mNativeContext, arg0);
    	return VO_OSMP_RETURN_CODE.valueOf(ret);
	}
	
//	@Override
//	public VO_OSMP_RETURN_CODE setDefaultAudioLanguage(
//			VO_OSMP_LANGUAGE_TYPE type) {
//		// TODO Auto-generated method stub
//		
//		if (mNativeContext == 0)
//		{
//			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
//			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
//		}
//		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;
//	}
//
//	@Override
//	public VO_OSMP_RETURN_CODE setDefaultSubtitleLanguage(
//			VO_OSMP_LANGUAGE_TYPE type) {
//		// TODO Auto-generated method stub
//		if (mNativeContext == 0)
//		{
//			voLog.e(TAG, "VOOSMP_ERR_Uninitialize");
//			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;
//		}
//		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT;
//	}
	
	private int VOOSMPStreamingDownloaderCallBack(int id, int param1, int param2, Object obj)
	{
		if (mEventHandler == null)
		{
			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_IMPLEMENT.getValue();
			
		}
		  
		VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID eventID = VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID.valueOf(id);
		
        switch(eventID)
        {
        	case VO_OSMP_CB_STREAMING_DOWNLOADER_MANIFEST_OK:
        		String localUrl = (String) obj;
        		voLog.i(TAG, "Downloader event is DOWNLOADER_MANIFEST_OK, localURI is " + localUrl);
        		break;
        	case  VO_OSMP_CB_STREAMING_DOWNLOADER_MANIFEST_UPDATE:
        		voLog.i(TAG, "Downloader event is DOWNLOADER_MANIFEST_UPDATE, current is " + param1 + " , total is " + param2);
        		VOOSMPStreamingDownloaderProgressInfoImpl impl = new VOOSMPStreamingDownloaderProgressInfoImpl(param1, param2);
        		obj = impl;
        		break;
        	default:
        		voLog.i(TAG, "Downloader event is %s .", eventID.name());
        }

		
		Message msg = Message.obtain(mEventHandler, id, param1, param2, obj);
		mEventHandler.sendMessage(msg);
		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE.getValue();
	}
	
	static
	{
	    try{
	        System.loadLibrary("vodl");
	        System.loadLibrary("voSTRMDownloaderJni_OSMP");
				mJNILoaded = true;
	        }catch(UnsatisfiedLinkError e ){
			mJNILoaded = false;
			e.printStackTrace();
		}
	}
	
	private static void loadJNI(String libPath)
	{
		String[] libs = {"vodl","voSTRMDownloaderJni_OSMP"};
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

	private	native int nativeInit (Integer context, VOOSMPStreamingDownloaderInitParam initParam);
	private	native int nativeUnInit (int context);
	private	native int nativeOpen (int context, String source, int flag, String localDir);
	private	native int nativeClose (int context);
	private	native int nativeStart (int context);
	private	native int nativeStop (int context);
	private	native int nativePause (int context);
	private	native int nativeResume (int context);

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
	private native String[] nativeGetVideoProperty (int context, int nIndex);
	private native String[] nativeGetAudioProperty (int context, int nIndex);
	private native String[] nativeGetSubtitleProperty (int context, int nIndex);
	private native int[] nativeGetCurrentSelection (int context);
	private native int[] nativeGetPlayingAsset(int context);






}
