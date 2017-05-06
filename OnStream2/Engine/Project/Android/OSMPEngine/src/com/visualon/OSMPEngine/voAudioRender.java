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
 * @file voAudioRender.java
 * Render audio stream.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/

package com.visualon.OSMPEngine;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;

import java.nio.ByteBuffer;
import java.util.concurrent.locks.ReentrantLock;

import com.visualon.OSMPUtils.*;
import com.visualon.OSMPEngine.voOnStreamSDK.*;

public class voAudioRender 
{	
    public static final int RENDER_FORMAT_PCM    =  voOSType.VOOSMP_AUDIO_CODINGTYPE.VOOSMP_AUDIO_CodingPCM.ordinal();
    public static final int RENDER_FORMAT_AC3    =  voOSType.VOOSMP_AUDIO_CODINGTYPE.VOOSMP_AUDIO_CodingAC3.ordinal();
    public static final int RENDER_FORMAT_EAC3   =  voOSType.VOOSMP_AUDIO_CODINGTYPE.VOOSMP_AUDIO_CodingEAC3.ordinal();

	private voOnStreamSDK	mPlayer;
	private AudioTrack 		mAudioTrack;  

	private int 			mSampleRate;
	private int 			mChannels;
	private int 			mRenderFormat;
	private int 			mSampleBit;

	private ByteBuffer 	 	mByteBuffer;

	Thread					mThreadPlayback;
	runPlayback				mrunPlayback;
	private	int				mStatus;	// 0 stop, 1 play, 2 pause
	private boolean			mbWrite;
	private ReentrantLock   mLock;
	private float 			mLeftVolume = -1.0f;
	private float 			mRightVolume = -1.0f;

	private static String	TAG = "@@@voAudioRender";
    private class runPlayback implements Runnable
    {
        private voAudioRender mAudioRender;

        public runPlayback (voAudioRender ar) 
        {
        	mAudioRender = ar;
        }
        
        public void run ()
        { 
        	mAudioRender.playback();
        }
    }	
	
	public voAudioRender(voOnStreamSDK pPlayer)
	{
		mPlayer = pPlayer;
		mAudioTrack = null;

		mSampleRate = 0;
		mChannels = 0;
		mRenderFormat = 0;
                mSampleBit = 0;

		mByteBuffer = null;
		
		mStatus = 0;
		
		mbWrite = false;
		
		mLock = new ReentrantLock();

	    voLog.i(TAG, "voAudioRender Construct");
	}	
	
	public void run () 
	{	
	    voLog.v(TAG, "run-  mStatus is " + mStatus);	
	    
		if (mStatus == 1)
		{
			return;
		}
		
		setStatus(1);
		if (mrunPlayback == null)
			mrunPlayback = new runPlayback (this);
		
		if (mThreadPlayback == null)
		{
			mThreadPlayback = new Thread (mrunPlayback, "vomeAudio Playback");
			mThreadPlayback.setPriority(Thread.MAX_PRIORITY - 1);
			mThreadPlayback.start();
		}
		voLog.v(TAG, "run+  mStatus is " + mStatus);	
	}
	
	public void pause()
	{
	    voLog.v(TAG, "pause-  mStatus is " + mStatus);	
	    setStatus(2);
	    
	    while (mbWrite)
	    {
	    	try {
				Thread.sleep(2);
				voLog.w(TAG, "Audio Pause wait for write completed.");
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
	    }

	    mLock.lock();
		if (mAudioTrack != null && mAudioTrack.getPlayState() == AudioTrack.PLAYSTATE_PLAYING)
		   mAudioTrack.pause();		
		mLock.unlock();
		
		voLog.v(TAG, "pause+  mStatus is " + mStatus);	
	}
	
	public void stop()
	{
	    voLog.v(TAG, "stop-  mStatus is " + mStatus);	
	    
	    setStatus(0);
	    
	    try {
	        mLock.lock();
		    if (mAudioTrack != null)
			{
				mAudioTrack.stop();		
			}
		    mLock.unlock();
	    }catch(Exception e)
	    {
	    	voLog.e(TAG, "AudioTracker stop exception , %s", e.getMessage());
	    }
		
	    voLog.v(TAG, "stop  call audio track stop");
		while (mThreadPlayback != null)
		{
			try {
				Thread.sleep(2);
				voLog.v(TAG, "Stop, waiting for audio thread quit. sleep 2ms status is " + mStatus);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}		
		}
		voLog.v(TAG, "stop+  mStatus is " + mStatus);	
	}
	
	public void flush()
	{
	    voLog.v(TAG, "flush");	

	    mLock.lock();
		if (mAudioTrack != null)
		   mAudioTrack.flush();
		mLock.unlock();
	}
	
	private void playback ()
	{
	    voLog.v(TAG, "playbackVideo started!");	
	    long lRC = 0;
	
		mThreadPlayback.setPriority(Thread.MAX_PRIORITY - 1);
		
		int oldStatus = 0;
		while (mStatus == 1 || mStatus == 2)
		{
			if (mStatus == 1)
			{
				mbWrite = true;
				if (mByteBuffer == null)
					lRC = mPlayer.GetAudioData (null);
				else
					lRC = mPlayer.GetAudioData (mByteBuffer.array());	
	
//				if (lRC == 0 || mAudioTrack == null)
				if (lRC == 0)
				{
					if (mPlayer.GetAudioSampleRate() != mSampleRate || mPlayer.GetAudioChannels() != mChannels || mPlayer.GetAudioRenderFormat() != mRenderFormat || mPlayer.GetAudioBitPerSample() != mSampleBit)
					{
						openTrack (mPlayer.GetAudioSampleRate(), mPlayer.GetAudioChannels(), mPlayer.GetAudioRenderFormat(), mPlayer.GetAudioBitPerSample());
						voLog.i(TAG, "openTrack finished.");
						if (mAudioTrack == null || mAudioTrack.getState() == AudioTrack.STATE_UNINITIALIZED )
						{
						    mLock.lock();
							mAudioTrack = null;
							mLock.unlock();
							
							EventHandler handler = mPlayer.GetEventHandler();
							if (handler != null)
							{
								handler.sendEmptyMessage(voOSType.VOOSMP_CB_Audio_Render_Failed);
							}
							voLog.e(TAG, "VOOSMP_CB_Audio_Render_Failed");
						}
                        if (nativeSetAudioTrack(mNativeContext, mAudioTrack) == true) {
                            //android.util.Log.i(TAG, "nativeSetAudioTrack returns true");
                                mPlayer.SetParam(voOSType.VOOSMP_PID_AUDIO_DSP_CLOCK, (int)(mNativeContext));
                    //mPlayer.SetParam(voOSType.VOOSMP_PID_AUDIO_REND_BUFFER_TIME, 500);
                        }
                        else {
                            //android.util.Log.i(TAG, "nativeSetAudioTrack returns false");
                                mPlayer.SetParam(voOSType.VOOSMP_PID_AUDIO_DSP_CLOCK, 0);
                        }
                        voLog.i(TAG, "Ready to write.");
					}
				}

				if (lRC > 0)
				{
					if(oldStatus>1)
					{
						oldStatus--;
						voLog.i(TAG, "The audio sample is discard.");
					}
					else
						writeData (mByteBuffer.array(), lRC);
				}
				mbWrite = false;
			}
			else
			{
				oldStatus = 2;
				try {
					Thread.sleep(2);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}		
		}
		
		mThreadPlayback = null;
		
		mLock.lock();
	    try {
		    if ((mAudioTrack != null) && (mAudioTrack.getPlayState() != AudioTrack.PLAYSTATE_STOPPED))
			{
				mAudioTrack.stop();		
			}
		    closeTrack ();	
	    }catch(Exception e)
	    {
	    	voLog.e(TAG, "AudioTracker stop or close exception , %s", e.getMessage());
	    }
	    mLock.unlock();
	    
	    voLog.v(TAG, "playbackaudio stopped!");	
	}

	private long writeData (byte[] audioData, long nSize)
	{
		if (mAudioTrack != null && nSize > 0)
		{
			if (mAudioTrack.getPlayState() != AudioTrack.PLAYSTATE_PLAYING)
			{
				voLog.v(TAG, "mAudioTrack.getPlayState() is %d, threadid is %d ",
						mAudioTrack.getPlayState(), mThreadPlayback.getId());
				mAudioTrack.play();	
				voLog.v(TAG, "mAudioTrack.play(), mAudioTrack.getPlayState is " + mAudioTrack.getPlayState());
			}

			mAudioTrack.write(audioData, 0, (int)nSize);
//			voLog.i(TAG, "mAudioTrack.write is %d, thread id is %d", nSize, (int)mThreadPlayback.getId());
		}
		
		return 0;
	}


	private int openTrack (int sampleRate, int channelCount, int renderFormat, int sampleBit)
	{
	    mLock.lock();
		try {
		    if (mAudioTrack != null)
			{
				mAudioTrack.stop();		
			}
		    closeTrack ();	
	    }catch(Exception e)
	    {
	    	voLog.e(TAG, "AudioTracker stop or close exception before openTrack , %s", e.getMessage());
	    }
		mLock.unlock();
				
        int nFormat = nativeGetAudioFormat(mNativeContext, renderFormat, sampleBit);
        if (nFormat == -1) // can only happen when native context could not be created
            nFormat = AudioFormat.ENCODING_PCM_16BIT;
		int nChannelConfig =  (channelCount == 1)? AudioFormat.CHANNEL_CONFIGURATION_MONO : AudioFormat.CHANNEL_CONFIGURATION_STEREO;
		int nMinBufSize = AudioTrack.getMinBufferSize(sampleRate, nChannelConfig, nFormat);                  
		
		if ( nMinBufSize == AudioTrack.ERROR_BAD_VALUE || nMinBufSize == AudioTrack.ERROR )   
		     return -1; 
/*		
		int param = nMinBufSize*1000/(sampleRate*channelCount*2)+100;
		if(mPlayer!=null){
			mPlayer.SetParam(voOSType.VOOSMP_PID_AUDIO_REND_BUFFER_TIME, param);
		}
*/

		nMinBufSize = nMinBufSize * 2;
		
		if (nMinBufSize < 2048)
			nMinBufSize = 2048;
		
		mLock.lock();
		mAudioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, sampleRate, nChannelConfig,   
	    								 nFormat, nMinBufSize, AudioTrack.MODE_STREAM);
		voLog.i(TAG , "Create AudioTrack, SampleRate " + sampleRate + "ChannelCount " + channelCount + " nMinBufSize = " + nMinBufSize);
		mLock.unlock();
		
		voOSAudioRenderFormat audioRenderFormat = new voOSAudioRenderFormat(sampleRate, channelCount, sampleBit, nMinBufSize);
		int renderBufferTime = 0;
		if(mPlayer!=null){
			mPlayer.SetParam(voOSType.VOOSMP_PID_AUDIO_RENDER_FORMAT, audioRenderFormat);
			renderBufferTime = (Integer) mPlayer.GetParam(voOSType.VOOSMP_PID_AUDIO_RENDER_LATENCY);
			mPlayer.SetParam(voOSType.VOOSMP_PID_AUDIO_REND_BUFFER_TIME, renderBufferTime);
			voLog.i(TAG, "VOOSMP_PID_AUDIO_RENDER_LATENCY VOOSMP_PID_AUDIO_REND_BUFFER_TIME is %d", renderBufferTime);
		}
		
		if(mLeftVolume>=0)
			arsetVolume(mLeftVolume, mRightVolume);

		if(mPlayer!=null)
		{
			String strModel = android.os.Build.MODEL;
		    strModel = strModel.toLowerCase();
			if (strModel.contains("kindle"))
			{
				int nAuAdjustTime = 300;
				mPlayer.SetParam(voOSType.VOOSMP_PID_AUDIO_REND_ADJUST_TIME, nAuAdjustTime);
		
				int nAudioBufferTime = 500;
				mPlayer.SetParam(voOSType.VOOSMP_PID_AUDIO_SINK_BUFFTIME, nAudioBufferTime);
		
				int nAuStepTime = 300;
				mPlayer.SetParam (voOSType.VOOSMP_PID_AUDIO_STEP_BUFFTIME, nAuStepTime);
				voLog.v(TAG, "This device is kindle fire, setup audo param!");
			}
		}
		
		mSampleRate = sampleRate;
		mChannels = channelCount;
		mRenderFormat = renderFormat;
        mSampleBit = sampleBit;
		
        voLog.i(TAG, "opentracker before allocate buffer");
		mByteBuffer = ByteBuffer.allocate (mSampleRate * mChannels * 3);
		if (mByteBuffer == null) 
		{
			voLog.e(TAG, "Failed to allocate buffer");
			return -1;
		}		
		voLog.i(TAG, "opentracker before allocate end");
		return 0;
	}
	
	private void closeTrack ()
	{
		if (mAudioTrack != null)
		{
			voLog.i(TAG, "closeTrack");
			nativeSetAudioTrack(mNativeContext, null);
            mPlayer.SetParam(voOSType.VOOSMP_PID_AUDIO_DSP_CLOCK, 0);
	//		mAudioTrack.stop();		 
			mAudioTrack.release();
			mAudioTrack = null;
			mSampleRate = 0;
            mChannels = 0;
            mByteBuffer = null;
		}
	}

    public void arsetVolume(float left, float right)
    {
		voLog.i(TAG, "arsetVolume left%.3f; right:%.3f .", left, right);
        mLock.lock();
        if (mAudioTrack != null) 
            mAudioTrack.setStereoVolume(left, right); 
    	mLeftVolume = left;
    	mRightVolume = right;

        mLock.unlock();
    }
	
	private  synchronized  void setStatus (int status)
	{
		voLog.i(TAG, "setStatus - , status is %d .", status);
		if (status == 0 || status == 1 || status == 2)
		{
			mStatus = status;
		}else
		{
			voLog.e(TAG, "Error: wrong status value is %d .", status);
		}
		voLog.i(TAG, "setStatus + , mStatus is %d .", mStatus);
	}


    public void init() {
        nativeInit();
    }

    public void unInit() {
        nativeUninit();
    }

    private long mNativeContext = 0;

    private native void    nativeInit();
    private native void    nativeUninit();
    private native boolean nativeSetAudioTrack(long context, AudioTrack track);
    private native int     nativeGetAudioFormat(long context, int renderFormat, int sampleBit);
}
