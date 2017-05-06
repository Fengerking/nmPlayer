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

package com.visualon.OSMPPlayer;

import com.visualon.OSMPPlayer.VOOSMPType.*;

import android.content.Context;
import android.view.SurfaceView;

public interface VOCommonPlayerControl {

    //
    // Player initialization
    //

    /**
     * Initialize a player instance.
     * 
     * <pre>
     * This function must be called first to start a session.
     * </pre>
     * 
     * @param playEngineType
     *            [in] Refer to {@link VO_OSMP_PLAYER_ENGINE}.
     * @param initParam
     *            [in] Refer to {@link VO_OSMP_INIT_PARAM}.
     * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE init(VO_OSMP_PLAYER_ENGINE playEngineType,
            VOOSMPInitParam initParam);

    /**
     * Initialize a player instance.
     * 
     * <pre>
     * This function must be called first to start a session.
     * </pre>
     * 
     * @param context
     *            [in] Current context.
     * @param libraryPath
     *            [in] Full path name of package, such as
     *            /data/data/packagename/lib/.
     * @param playEngineType
     *            [in] Refer to {@link VO_OSMP_PLAYER_ENGINE}.
     * @param initParam
     *            [in] Currently unused, should be set to The value is NULL now.
     * @param initParamFlag
     *            [in] Currently unused, The value is should be set to 0 now.
     * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     * 
     * @deprecated This method is replaced by {@link com.visualon.OSMPPlayer.VOCommonPlayerControl#init(VO_OSMP_PLAYER_ENGINE playEngineType, VOOSMPInitParam initParam)} and will be removed in the future. 
     * 
     */
    @Deprecated
    VO_OSMP_RETURN_CODE init(Context context, String libraryPath,
            VO_OSMP_PLAYER_ENGINE playEngineType, Object initParam,
            int initParamFlag);

    /**
     * Destroy a player instance.
     * 
     * <pre>
     * This function must be called last to close a session.
     * </pre>
     * 
     * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful .
     */
    VO_OSMP_RETURN_CODE destroy();

   
    /**
     * Set surface view for video playback
     * 
     * @param sv  [in] Current SurfaceView.
     * 
     * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE setView(SurfaceView sv);

   
    /**
     * Set surface view size
     *
     * @param width     [in] Surface view width.
     * @param height    [in] Surface view height.
     *
     * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE setViewSize(int width, int height);


    //
    // Data source initialization
    //

    /**
     * Open media source
     * 
     * @param url
     *            [in] Source file description (e.g. an URL or a file
     *            descriptor, etc.).
     * @param flag
     *            [in] The flag for opening media source. Refer to
     *            {@link VO_OSMP_SRC_FLAG}.
     * @param type
     *            [in] Indicates the source format. Default is
     *            {@link VO_OSMP_SRC_FORMAT#VO_OSMP_SRC_AUTO_DETECT}.
     * @param openParam
     *            [in] Initial parameters. Refer to {@link VOOSMPOpenParam}.
     * 
     * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     * 
     */
    VO_OSMP_RETURN_CODE open(String url, VO_OSMP_SRC_FLAG flag,
            VO_OSMP_SRC_FORMAT type, VOOSMPOpenParam openParam);

    /**
     * Open media source
     * 
     * @param url
     *            [in] Source file description (e.g. an URL or a file
     *            descriptor, etc.).
     * @param flag
     *            [in] The flag for opening media source. Refer to
     *            {@link VO_OSMP_SRC_FLAG}.
     * @param type
     *            [in] Indicates the source format.;, default Default is
     *            {@link VO_OSMP_SRC_FORMAT#VO_OSMP_SRC_AUTO_DETECT}.
     * @param initParam
     *            [in] Initial parameters. Refer to {@link VOOSMPInitParam}.
     *            Valid fields depend on type, according to initParamFlag,
     *            indicate which field is valid. If (initParamFlag &
     *            {@link VO_OSMP_INIT_PARAM_FLAG#VO_OSMP_FLAG_INIT_ACTUAL_FILE_SIZE}
     *            ) > 0, {@link VOOSMPInitParam#getFileSize()} should get a File
     *            size.
     * @param initParamFlag
     *            [in] InitParam flags. refer Refer to
     *            {@link VO_OSMP_INIT_PARAM_FLAG}, }. you You can use "|" (or)
     *            to input several parameters at same time.
     * 
     * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     * 
     * @deprecated This function is replaced by {@link com.visualon.OSMPPlayer.VOCommonPlayerControl#open(String url,VO_OSMP_SRC_FLAG flag,VO_OSMP_SRC_FORMAT type, VOOSMPOpenParam openParam)} and will be removed in the future
     */
    @Deprecated
    VO_OSMP_RETURN_CODE open(String url, VO_OSMP_SRC_FLAG flag,
            VO_OSMP_SRC_FORMAT type, VOOSMPInitParam initParam,
            int initParamFlag);

    /**
     * Close data media source. Framework is still available
     *
     * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE close();


    //
    // Runtime control
    //

    /**
     * Start playback
     *
     * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE start();
   

    /**
     * Pause playback
     *
     * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE pause();
   

    /**
     * Stop playback
     *
     * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE stop();


    /**
     * Seek operation
     *
     * @param   msec [in] the position (ms) to seek to.
     * @return  Position after seek operation; -1 if seek failed.
     */
    long setPosition(long msec);
    
   
    /**
     * Set playback volume
     *
     * @param     volume    [in] audio channel volume. A value of 0.0f indicates silence; a value of 1.0f indicates no attenuation.
     *
     * @return    {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE setVolume(float volume);   


    /**
     * Mute the audio
     *
     * @return    {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE mute();


    /**
     * Unmute the audio
     *
     * @return    {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE unmute();


    /**
     * Suspend player
     * <pre>The application layer should invoke this method on receiving a notification of moving to background.</pre>
     *
     * @param keepAudio [in] Keep/pause audio. True for continuing to play audio, false to pause both video and audio.
     *
     * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful; error code if not.
     */
    VO_OSMP_RETURN_CODE suspend(boolean keepAudio);


    /**
     * Resume player
     * <pre>The application layer should invoke this method on receiving a notification of moving to foreground.</pre>
     *
     * @param  sv   [in] Current SurfaceView.
     *
     * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful; error code if not.
     */
    VO_OSMP_RETURN_CODE resume(SurfaceView sv);


    /**
     * Notify the player that surface changed event has been completed and video rendering can be started.
     * This method is called when receiving SurfaceHolder.SurfaceChanged(). When surface is changing, video rendering
     * is suspended until this API is called. It will time out after 200ms if the API is not called, and video rendering will continue.
     *
     * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE setSurfaceChangeFinished();
   

    /**
     * Set aspect ratio of video
     *
     * @param ar  [in]  Refer to {@link VO_OSMP_ASPECT_RATIO}.
     *
     * {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful; error code if not.
     */
    VO_OSMP_RETURN_CODE setVideoAspectRatio(VO_OSMP_ASPECT_RATIO ar);
   
    /**
     * Redraw the video 
     *
     * This function is not applicable on Android
     *
     * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE redrawVideo();
   
   
    /**
     * Set the onEventListener instance to handle SDK events. Events are posted by sub-thread to {@link VOCommonPlayerListener#onVOEvent}.
     *
     *
     * @param   instance [in] Event listener instance.
     * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE setOnEventListener(VOCommonPlayerListener instance);
    
    /**
     * Set screen display brightness mode. This function is not applicable to Android version 2.1 or below.
     *
     * @param mode [in] Refer to {@link VO_OSMP_SCREEN_BRIGHTNESS_MODE}
     *
     * @return    {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE setScreenBrightnessMode(VO_OSMP_SCREEN_BRIGHTNESS_MODE mode);
  

    /**
     * Set screen display brightness
     *
     * @param brightness [in] Display brightness (percent). The valid range is 0 to 100
     *
     * @return    {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE setScreenBrightness(int brightness);      
    

    /**
     * Start periodic SEI data notifications
     *
     * @param   interval [in] Time interval <ms> between two SEI data notifications.
     *
     * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE startSEINotification(int interval);


    /**
     * Stop periodic SEI data notifications
     *
     * @return  {@link VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE stopSEINotification();
    
    /**
     * Update media source.
     *
     * @param   url             [in] Source file description (e.g. an URL or a file descriptor, etc.)
     *
     * @return {@link VO_OSMP_ERR_NONE} if successful
     *
     * * <pre>This API should be called after calling the {@link VOCommonPlayerControl#open:flag:sourceType:openParam:} method.</pre>
     *
     */
    VO_OSMP_RETURN_CODE updateSourceURL(String url);
    
    /**
     * Start periodic analytics data notifications. With each notification, analytics data of past "interval" ms is provided.
     *
     * @param interval  [in] Time interval (ms), between two analytics data notifications. In each notification, analytics data of past "interval" ms is returned.
     * @param filter    [in] Filters, specified as an {@link VOOSMPAnalyticsFilter} object, to be applied to cached analytic data before being returned.
     *        
     * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     * 
     */
    VO_OSMP_RETURN_CODE startAnalyticsNotification(int interval, VOOSMPAnalyticsFilter filter);


    /**
     * Stop periodic analytics data notifications
     *
     * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     *
     */
    VO_OSMP_RETURN_CODE stopAnalyticsNotification();

}
