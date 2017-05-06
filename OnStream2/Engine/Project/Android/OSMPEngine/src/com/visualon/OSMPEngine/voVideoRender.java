
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
 * @file voVideoRender.java
 * Render video stream.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/

package com.visualon.OSMPEngine;

import java.nio.ByteBuffer;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.view.Surface;
import android.view.Surface.OutOfResourcesException;
import android.view.SurfaceHolder;

import com.visualon.OSMPUtils.*;

public class voVideoRender 
{
	private final String	TAG = "@@@voVideoRender";
	private voOnStreamSDK	mPlayer;
	private Surface       	mSurface;
	private SurfaceHolder	mSurfaceHolder;
	private Bitmap        	mBitmap;
	private ByteBuffer 	 	mByteBuffer;

	private int 	       	mWidthBitmap;
	private int 			mHeightBitmap;
	private	int				mPixelBytes;
	
	Thread					mThreadPlayback;
	runPlayback				mrunPlayback;
	private	int				mStatus;	// 0 stop, 1 play, 2 pause
	
	private Paint			mPaint;
	private boolean			mSeeking;
	
	private boolean			mUseOptimisedBitmapRendering;
	private long 			mlJavaRenderFrames;
	private boolean 		mbNativeRender;

	
    private class runPlayback implements Runnable
    {
        private voVideoRender mVideoRender;

        public runPlayback (voVideoRender vr) 
        {
        	mVideoRender = vr;
        }
        
        public void run ()
        { 
        	mVideoRender.playback();
        }
    }	
    
	public voVideoRender(voOnStreamSDK pPlayer) 
	{
		mPlayer = pPlayer;	
		mSurface = null;
		mSurfaceHolder = null;
		mBitmap = null;	
		mByteBuffer = null;
		
		mWidthBitmap = 0;
		mHeightBitmap = 0;
		mPixelBytes = 2;
		
		setStatus(0);
		
		mPaint = new Paint();
		mPaint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.SRC));
		mPaint.setDither(false);
		mPaint.setFilterBitmap(false);

		mUseOptimisedBitmapRendering 	= false;
		mlJavaRenderFrames 				= 0;
		mbNativeRender 					= true;
		
	    voLog.v(TAG, "Construct!");			
	}			
	
	public void run () 
	{	
	    voLog.v(TAG, "run  mStatus is " + mStatus);	
	    	    
	    if (mSeeking)
	    	mSeeking = false;
	    
//		if (mStatus == 1)
//			return;
		
		setStatus(1);

		if (mrunPlayback == null)
			mrunPlayback = new runPlayback (this);
		
		if (mThreadPlayback == null)
		{
			mThreadPlayback = new Thread (mrunPlayback, "vomeVideo Playback");
			mThreadPlayback.setPriority(Thread.MAX_PRIORITY - 2);
			mThreadPlayback.start();
		}
	}
	
	public void pause()
	{
	    voLog.v(TAG, "pause  mStatus is " + mStatus);	
	    
	    setStatus(2);
	}
	
	public void stop()
	{
	    voLog.v(TAG, "stop  mStatus is " + mStatus);	
	    
	    setStatus(0);
		
		while (mThreadPlayback != null)
		{
			try {
				Thread.sleep(2);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}		
		}
		
		if (mBitmap != null)
		{
			mBitmap.recycle();
			mBitmap = null;
		}
	}
	
	public void setPos (long pos)
	{
		if (mStatus == 2)
		{
			mSeeking = true;			
			setStatus(1);
		}
	}
				
	public void setSurface (Surface pSurface)
	{
		mSurface = pSurface;
	}
	
	public void setSurfaceHolder (SurfaceHolder pSH)
	{
		mSurfaceHolder = pSH;
		int icolor = 1;
		if (icolor ==1) 
		{
			voLog.v(TAG, "setSurfaceHolder use RGBA8888!");
			
			String model = android.os.Build.MODEL.toLowerCase();	
			if (model.compareToIgnoreCase("vtab1008") == 0) {
//				mSurfaceHolder.setFormat(PixelFormat.RGBX_8888);
				mPlayer.SetParam(voOSType.VOOSMP_PID_DRAW_COLOR, voOSType.VOOSMP_COLOR_RGB32_PACKED);
		    } else {
//				mSurfaceHolder.setFormat(PixelFormat.RGBA_8888);
				mPlayer.SetParam(voOSType.VOOSMP_PID_DRAW_COLOR, voOSType.VOOSMP_COLOR_ARGB32_PACKED);
		    }
			mPixelBytes = 4;
		}
		else if (icolor == 2)
		{
			voLog.v(TAG, "setSurfaceHolder use RGBX8888!");
			
			mSurfaceHolder.setFormat(PixelFormat.RGBX_8888);
			mPlayer.SetParam(voOSType.VOOSMP_PID_DRAW_COLOR, voOSType.VOOSMP_COLOR_RGB32_PACKED);
			mPixelBytes = 4;
		} 	
		else {
			voLog.v(TAG, "setSurfaceHolder use RGB565!");
			
			mSurfaceHolder.setFormat(PixelFormat.RGB_565);
			mPlayer.SetParam(voOSType.VOOSMP_PID_DRAW_COLOR, voOSType.VOOSMP_COLOR_RGB565_PACKED);
			mPixelBytes = 2;		
		}
	}
	
	public void setRenderType (int nRenderType)
	{
		if (nRenderType == voOSType.VOOSMP_RENDER_TYPE_JAVA)
		{
			mbNativeRender = false;
			mUseOptimisedBitmapRendering = false;
		}else if (nRenderType == voOSType.VOOSMP_RENDER_TYPE_BITMAP)
		{
			mbNativeRender = false;
			mUseOptimisedBitmapRendering = true;
		}else
		{
			mbNativeRender = true;
		}
	}
	
	public void setColorType(int nColorType)
	{
		if (nColorType == voOSType.VOOSMP_COLOR_ARGB32_PACKED) 
		{
			mPixelBytes = 4;
		}
		else if (nColorType == voOSType.VOOSMP_COLOR_RGB32_PACKED)
		{
			mPixelBytes = 4;
		} 	
		else 
		{
			mPixelBytes = 2;		
		}
	}
	
	public void playback()
	{
			
		long lRC = 0;
	
		mThreadPlayback.setPriority(Thread.MAX_PRIORITY - 2);	
		voLog.v(TAG, "playbackVideo started! Is native " + mbNativeRender);
		
		while (mStatus == 1 || mStatus == 2)
		{
			if (mStatus == 1)
			{
				if (mByteBuffer == null)
				{
					lRC = mPlayer.GetVideoData (null);
				}
				else
				{
					// added by gtxia for fixing the issue 23452
					mByteBuffer.clear();
					lRC = mPlayer.GetVideoData (mByteBuffer.array());	
				}
			
				if (mWidthBitmap != mPlayer.GetVideoWidth () || mHeightBitmap != mPlayer.GetVideoHeight())
				{	
					init (mPlayer.GetVideoWidth (), mPlayer.GetVideoHeight());
					continue;
				}
				
//				mbNativeRender = true;
				if (lRC == 0 && !mbNativeRender)
				{
		//			voLog.v(TAG, "render frame #, Optimized bitmap is %b, "+ mlJavaRenderFrames, this.mUseOptimisedBitmapRendering);
					// added by gtxia for fixing the issue 23452
					if(mByteBuffer.remaining() > 0)
						render ();
					if (mSeeking)
					{
						mSeeking = false;
						setStatus(2);
					}
				}
			}
			
			if (lRC == voOSType.VOOSMP_ERR_WaitTime || mStatus == 2)
			{
				try {
					Thread.sleep(2);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
		
//		mThreadPlayback.stop();
//		mThreadPlayback.destroy();
		mThreadPlayback = null;
		
		voLog.v(TAG, "playbackVideo stopped! mStatus is %d", mStatus);	
	} 
	
	public boolean init(int nWidth, int nHeight) 
	{
		voLog.v(TAG, "Init W X H " + nWidth + nHeight);
		
		if (nWidth == 0 || nHeight == 0)
			return false;
		
		if (mWidthBitmap == nWidth && mHeightBitmap == nHeight)
			return true;
		
		if (mBitmap != null) 
		{
			mBitmap.recycle();
			mBitmap = null;
		}	
		
		try
		{
			if ((mbNativeRender == false) && (mUseOptimisedBitmapRendering == false))
			{
				mByteBuffer = ByteBuffer.allocate (nWidth * nHeight * mPixelBytes);
		
				if (mByteBuffer == null) 
				{
					voLog.e(TAG, "Failed to allocate buffer");
					return false;
				}
				voLog.i(TAG, "mByteBuffer allocate is %d.", nWidth * nHeight * mPixelBytes);
			}else
			{
				//mByteBuffer will be not used, optional allocate some memory
				mByteBuffer = ByteBuffer.allocate (4);
			}
		
			if (mbNativeRender == false) 
			{
				if (mPixelBytes == 2)
					mBitmap = Bitmap.createBitmap(nWidth, nHeight, Bitmap.Config.RGB_565 );
				else
					mBitmap = Bitmap.createBitmap(nWidth, nHeight, Bitmap.Config.ARGB_8888);
				
				if (mBitmap == null) 
				{
					voLog.e(TAG, "Failed to Create Bitmap buffer");
					return false;
				}
				voLog.i(TAG, "mBitmap createBitmap is %d X %d. mPixelBytes is %d ", nWidth, nHeight, mPixelBytes);
			}
			
			mWidthBitmap  = nWidth;
			mHeightBitmap = nHeight;
		}catch(Exception e)
		{
			voLog.e(TAG, "Failed to Create Bitmap buffer on catch! " + e.getMessage());
			return false;
		}
	    
		if (mBitmap != null && mUseOptimisedBitmapRendering)
		{
			voLog.i(TAG, "new bitmap created! " + mPlayer.GetVideoWidth() + "x" + mPlayer.GetVideoHeight());	
			mPlayer.SetParam(voOSType.VOOSMP_PID_BITMAP_HANDLE, mBitmap);
		}
		

		mPlayer.VideoSizeChanged();
		return true;			
	}
	
	public long render() 
	{	
		if (mSurface == null || mBitmap == null)
			return -1;
	
		try 
		{		
			Canvas canvas = mSurface.lockCanvas(null);
			if (canvas == null)
			{
				mSurface.unlockCanvasAndPost(canvas);				
				return -1;
			}

			if (!mUseOptimisedBitmapRendering) 
			{ 	
//				voLog.i(TAG, "mByteBuffer size is %d, mBitmap width is %d, rowbytes is %d, height is %d",
//						mByteBuffer.array().length, mBitmap.getWidth(), mBitmap.getRowBytes(), mBitmap.getHeight());
				
				mBitmap.copyPixelsFromBuffer(mByteBuffer);
			}			
			canvas.drawBitmap(mBitmap, 0, 0, mPaint); 				
			mlJavaRenderFrames ++;
			mSurface.unlockCanvasAndPost(canvas);	
		} catch (IllegalArgumentException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (OutOfResourcesException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		return 0;
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
}
