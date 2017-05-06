/************************************************************************
  VisualOn Proprietary
  Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved.

  VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

  All data and information contained in or disclosed by this document are
  confidential and proprietary information of VisualOn, and all rights
  therein are expressly reserved. By accepting this material, the
  recipient agrees that this material and the information contained
  therein are held in confidence and in trust. The material may only be
  used and/or disclosed as authorized in a license agreement controlling
  such use and disclosure.
 ************************************************************************/

/**
 * Constant definitions
 */
var VO_OSMP_ASSET_NOT_SELECTED  = -2;
var VO_OSMP_ASSET_AUTO_SELECTED = -1;


/********************************************************************************************************************************/
//* public interface VOCommonPlayerControl {
function VOCommonPlayerControl(pluginobject) {

	this.player = pluginobject;

	/**
	 * Initialize a player instance.
	 * <pre>This function must be called first to start a session.</pre>
	 *
	 * @param   playEngineType  [in] Refer to {@link VO_OSMP_PLAYER_ENGINE}.
	 * 
	 * @param   initParam       [in] Currently unused, should be set to nil.
	 * 
 	 * @return 0 if successful.
	 */
//* VO_OSMP_RETURN_CODE	init(VO_OSMP_PLAYER_ENGINE engineType, VOOSMPInitParam* initParam);
	VOCommonPlayerControl.prototype.init = function(engineType, initParam)
	{
		var ret = this.player.init(engineType, initParam);
		return ret;
	};


	/**
	 * Open media source.
	 *
	 * @param url  [string] Source file description (e.g. a URL or a file descriptor, etc.).
	 *
	 * @param flag [in]     Flag for opening media source. Default is zero.
	 *
	 * @param sourceType      [in] Indicates the source format. Refer to {@link VO_OSMP_SRC_FORMAT}. Default value is {@link VO_OSMP_SRC_AUTO_DETECT}
	 *
 	 * @param openParam       [in] Open parameters. Refer to {@link VOOSMPOpenParam}. Valid fields depend on the value.
	 *
 	 * @return 0 if successful.
	 */
//* VO_OSMP_RETURN_CODE open(string url, VO_OSMP_SRC_FLAG flag, VO_OSMP_SRC_FORMAT sourceType, VOOSMPOpenParam* openParam);
	VOCommonPlayerControl.prototype.open = function(url, flag, sourceType, openParam)
	{
		var ret = this.player.open(url, flag, sourceType, openParam);
		return ret;
	};


	/**
	 * Close media source. Framework is still available.
	 * 
	 * @return 0 if successful.
	 */
//* VO_OSMP_RETURN_CODE close();
	VOCommonPlayerControl.prototype.close = function()
	{
		var ret = this.player.close();
		return ret;
	};


	/**
	 * Start playback.
	 * 
	 * @return 0 if successful.
	 */
//* VO_OSMP_RETURN_CODE play();
	VOCommonPlayerControl.prototype.play = function()
	{
		var ret = this.player.start();
		return ret;
	};


	/**
	 * Pause playback.
	 * 
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE pause();
	VOCommonPlayerControl.prototype.pause = function()
	{
		var ret = this.player.pause();
		return ret;
	};


	/**
	 * Stop playback.
	 * 
	 * @return VO_OSMP_ERR_NONE if successful.
	 */
  //* VO_OSMP_RETURN_CODE stop();
	VOCommonPlayerControl.prototype.stop = function()
	{
		var ret = this.player.stop();
		return ret;
	};


	/**
	 * Seek operation.
	 * 
	 * @param msec [number] Position to seek to.
         *
	 * @return Position after seek operation; -1 if seek failed.
	 */	
  //* long setPosition(long msec);
	VOCommonPlayerControl.prototype.setPosition = function(msec)
	{
		return this.player.setPosition(msec);
	};


        /**
         * Get current playback position.
         *
         * <p><ul>
         * <li> For VOD. returns current playback position.
         * <li> For Live streaming. Depend on {@link VOCommonPlayerConfiguration#enableLiveStreamingDVRPosition:}
         *  <ol>
         *  <li>If enable. returns 0 when playing at the live head. A negative position represents the offset into the DVR window with respect to live head
         *  <li>If disable. returns the offset with respect to the beginning of the playback
         * </ul><p>
         *
         * @return  the position <ms>; -1 if unsuccessful
         */
 //* long getPosition();
	VOCommonPlayerControl.prototype.getPosition = function()
	{
		return this.player.getPosition();
	};


	/**
	 * Check if an opened media source is live streaming.
	 *
	 * @return True if the media source is opened, is a streaming source, and is live streaming.
	 */
  //* boolean isLiveStreaming();
	VOCommonPlayerControl.prototype.isLiveStreaming = function()
	{
		return this.player.isLiveStreaming();
	};


	/**
	 * Get duration of the stream.
	 * 
	 * @return Current stream duration, 0 if source is a live stream.
	 */
  //* long getDuration();
	VOCommonPlayerControl.prototype.getDuration = function() 
	{
		return this.player.getDuration();
	};


	/**
	 * Get playback volume.
	 * 
	 * @return Current playback volume.
	 */
  //* float getVolume();
	VOCommonPlayerControl.prototype.getVolume = function()
	{
		return this.player.getVolume();
	};


	/**
	 * Set playback volume.
	 * 
	 * @param leftvol  [number] Left audio channel. Silence has a value of 0.0f. A value of 1.0f indicates no attenuation.
         *
	 * @param rightvol [number] Right audio channel.
	 * 
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE setVolume(float leftvol, float rightvol);
	VOCommonPlayerControl.prototype.setVolume = function(leftvol, rightvol)
	{
		var ret = this.player.setVolume(leftvol, rightvol);
		return ret;
	};


	/**
	 * Mute the audio.
	 * 
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE mute();
	VOCommonPlayerControl.prototype.mute = function()
	{
		var ret = this.player.mute();
		return ret;
	};


	/**
	 * Unmute the audio and restore the previous volume settings.
	 * 
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE unmute();
	VOCommonPlayerControl.prototype.unmute = function()
	{
		var ret = this.player.unmute();
		return ret;
	};


	/**
	 * Set video aspect ratio.
	 * 
	 * @param ar [in] Aspect ratio of the video. For more details on valid values, please refer to the VO_OSMP_ASPECT_RATIO section in VOOSMPType.js.
	 *							
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE setVideoAspectRatio(VO_OSMP_ASPECT_RATIO ar);
	VOCommonPlayerControl.prototype.setVideoAspectRatio = function(ar)
	{
		var ret = this.player.setVideoAspectRatio(ar);
		return ret;
	};


	/** 
	 * Get player status.
	 * 
	 * @return Current player status. Please refer to VO_OSMP_STATUS.
	 */
  //* VO_OSMP_STATUS getPlayerStatus();
	VOCommonPlayerControl.prototype.getPlayerStatus = function()
	{
		return this.player.getPlayerStatus();
	};


	/**
	 * Check if the stream can be paused.
	 * 
	 * @return True for VOD, false for a live stream with no DVR-like features enabled.
	 */
  //* boolean canBePaused();
	VOCommonPlayerControl.prototype.canBePaused = function()
	{
		return this.player.canBePaused();
	};


	/**
	 * Make player display in full screen.
	 * 
	 * @return 0 if successful; non-zero if failed.
	 */
  //* VO_OSMP_RETURN_CODE fullScreen();
	VOCommonPlayerControl.prototype.fullScreen = function()
	{
		var ret = this.player.fullScreen();
		return ret;
	};

	/**
	 * To check if the player is currently in full mode or not.
	 * 
	 * @return 0 for normal mode, 1 for full mode.
	 */
  //* VO_OSMP_RETURN_CODE isFullScreen();
	VOCommonPlayerControl.prototype.isFullScreen = function()
	{
		var ret = this.player.isFullScreen();
		return ret;
	};
	
	/**
	 * Start periodic analytics data notifications. With each notification, analytics data of past "interval" ms is provided.
	 *
	 * @param interval [in] Time interval (ms) between two analytics data notifications. In each notification, analytics data of past "interval" ms is returned.
	 * @param filter   [in] Filters, which is used to cache analytic data before analytics os stopped and  specified as a string in JSON format as below.
	 *
	 *                   lastTime:   A value in seconds, which indicates that how far backwards to report it.
	 *                   sourceTime: A value in ms-seconds, which results in a Number that would be recorded once the source exceeds it.
	 *                   codecTime:  A value in ms-seconds, which results in a Number of instances that would be recorded once codec exceeds it.
	 *                   renderTime: A value in ms-seconds, which results in a Number of instances that will be .recorded when the render exceeds it.
	 *                   jitter:     A value in ms-seconds, which results in a Number of instances that will be recorded when the jitter exceeds it.
	 *
	 * @return VO_OSMP_ERR_NONE if successful.
	 */
  //* VO_OSMP_RETURN_CODE startAnalyticsNotification(int interval, VOOSMPAnalyticsFilter filter);
	VOCommonPlayerControl.prototype.startAnalyticsNotification = function(interval, filter)
	{
		var ret = this.player.startAnalyticsNotification(interval, filter);
		return ret;
	};


	/**
	 * Stop periodic analytics data notifications.
	 *
	 * @return VO_OSMP_ERR_NONE if successful.
	 */
  //* VO_OSMP_RETURN_CODE stopAnalyticsNotification();
	VOCommonPlayerControl.prototype.stopAnalyticsNotification = function()
	{
		var ret = this.player.stopAnalyticsNotification();
		return ret;
	};


	/**
	 * Register callback function to plugin.
	 * 
	 * @param eventHandler Object of the callback function which should be defined as below.
	 *
	 *		function onEventListner(nID, nParam1, nParam2, object)
     *
     *		object is a string of JSON format
	 *
	 * @return 0 if successful; non-zero if failed.
	 */
  //* VO_OSMP_RETURN_CODE registerEventHandler(VOCommonPlayerListener eventHandler);
	VOCommonPlayerControl.prototype.registerEventHandler = function(eventHandler)
	{
		var ret = this.player.registerEventHandler(eventHandler);
		return ret;
	};


	/**
	 * Register callback function to plugin.
	 * 
	 * @param eventHandler Object of the callback function which should be defined as below.
	 *
	 *		function onEventListner(nID, nParam1, nParam2, object)	 
     *
     *		object is a string of JSON format
     *
	 * @return 0 if successful; non-zero if failed.
	 */
  //* VO_OSMP_RETURN_CODE setOnEventHandler(VOCommonPlayerListener eventHandler);
	VOCommonPlayerControl.prototype.setOnEventHandler = function(eventHandler)
	{
		var ret = this.player.registerEventHandler(eventHandler);
		return ret;
	};
	
	/**
	 * Improve video performance when overlay UI has been hidden by the application.
	 * This function should be called (with false) after the UI has been hidden.
	 * It should be called (with true) before the UI is displayed again.
	 * 
	 * @param shown true to show overlay UI; false to hide overlay UI.
	 *
	 * @return 0 if successful; non-zero if failed.
	 */
  //* VO_OSMP_RETURN_CODE toggleOverlayUI(boolean shown);
	VOCommonPlayerControl.prototype.toggleOverlayUI = function(shown)
	{
		var ret = this.player.toggleOverlayUI(shown);
		return ret;
	}

	/**
	 * Start periodic SEI data notifications.
	 *
	 * @param   interval [in] Time interval <ms> between two SEI data notifications.
	 *
	 * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful
	 */
  //* VO_OSMP_RETURN_CODE startSEINotification(int interval);
	VOCommonPlayerControl.prototype.startSEINotification = function(interval)
	{
		var ret = this.player.startSEINotification(interval);
		return ret;
	}

	/**
	 * Stop periodic SEI data notifications.
	 *
	 * @return  {@link VO_OSMP_ERR_NONE} if successful
	 */
  //* VO_OSMP_RETURN_CODE stopSEINotification();
	VOCommonPlayerControl.prototype.stopSEINotification = function()
	{
		var ret = this.player.stopSEINotification();
		return ret;
	}

	/**
	 * Get the SEI info.
	 *
	 * @param   time [in] the time stamp of SEI that want to get
	 * @param   flag [in] the type flag of SEI info {@link VO_OSMP_SEI_INFO_FLAG}
	 *
	 * @return  the object according to flag if successful
	 */
  //* JSON getSEIInfo(long long time, VO_OSMP_SEI_INFO_FLAG flag);
	VOCommonPlayerControl.prototype.getSEIInfo = function(tm, flag)
	{
		var ret = this.player.getSEIInfo(tm, flag);
		return ret;
	}

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
  //* VO_OSMP_RETURN_CODE updateSourceURL(char *url);
	VOCommonPlayerControl.prototype.updateSourceURL = function(url)
	{
		var ret = this.player.updateSourceURL(url);
		return ret;
	}




};
//* }

/********************************************************************************************************************************/
//* public interface VOCommonPlayerConfiguration {
function VOCommonPlayerConfiguration(pluginobject) {

	this.player = pluginobject;

	/**
	 * Set content of license file.
	 * 
	 * @param licdata [string] Content of the license file in a string.
	 * 
	 * @return 0 if successful.
	 * 
	 * NOTE: This API must be called before running the player.
	 */
  //* VO_OSMP_RETURN_CODE setLicenseContent(byte[] licdata);
	VOCommonPlayerConfiguration.prototype.setLicenseContent = function(licdata)
	{
		var ret = this.player.setLicenseContent(licdata);
		return ret;
	};


	/**
	 * Set pre-agreed license string.
	 * 
	 * @param  str  Pre-agreed license string.
	 * 
	 * @return 0 if successful.
	 * 
	 * NOTE: This API must be called before running the player.
	 */
  //* VO_OSMP_RETURN_CODE setPreAgreedLicense(string str);
	VOCommonPlayerConfiguration.prototype.setPreAgreedLicense = function(str)
	{
		var ret = this.player.setPreAgreedLicense(str);
		return ret;
	};


	/**
	 * Set the location of the license file.
	 * 
	 * @param  licpath Location of the license file.
	 * 
	 * @return 0 if successful.
	 * 
	 * NOTE: This API must be called before running the player.
	 */
  //* VO_OSMP_RETURN_CODE setLicenseFilePath(string licpath);
	VOCommonPlayerConfiguration.prototype.setLicenseFilePath = function(licpath)
	{
		var ret = this.player.setLicenseFilePath(licpath);
		return ret;
	};


      /**
	 * Set the draw color type.
	 *
	 * @param  type Color type. Please refer to VO_OSMP_COLORTYPE.
	 *
	 * @return VO_OSMP_ERR_NONE if successful.
	 *
	 * NOTE: This API must be called before running the player calling the play method.
	 */
  //* VO_OSMP_RETURN_CODE setColorType(VO_OSMP_COLORTYPE type);
	VOCommonPlayerConfiguration.prototype.setColorType = function(type)
	{
		var ret = this.player.setColorType(type);
		return ret;
	};


      /**
	 * Enable/Disable deblock. This is valid for H.264 and Real video. The default is enabled.
	 * 
	 * @param  val True to enable deblock, false to disable deblock.
	 * 
	 * @return 0 if successful.
	 * 
	 */
  //* VO_OSMP_RETURN_CODE enableDeblock(boolean val);
	VOCommonPlayerConfiguration.prototype.enableDeblock = function(val)
	{
		var ret = this.player.enableDeblock(val);
		return ret;
	};


	/**
	 * Set the location of device capability file.
	 * 
	 * @param  filename Location of device capability file.
	 * 
	 * @return 0 if successful.
	 * 
	 * NOTE: This API should be called before opening media source.
	 */
  //* VO_OSMP_RETURN_CODE setDeviceCapabilityByFile(string filename);
	VOCommonPlayerConfiguration.prototype.setDeviceCapabilityByFile = function(filename)
	{
		var ret = this.player.setDeviceCapabilityByFile(filename);
		return ret;
	};


	/**
	 * Set the initial bitrate. The player will select the closest stream at or below that number. If none is available, it will pick the closest one above. If an initial bitrate is not set, the player will follow the streaming protocol specification. If the behavior is undefined in the protocol being used, the player will pick a stream based on cap.xml, the order in the manifest, and other heuristics.
         *
	 * @param bitrate [in] Initial bitrate (bps).
         *
	 * @return 0 if successful.
	 * 
	 * NOTE: This API must be called after opening media source and before running the player.
	 * 
	 */
  //* VO_OSMP_RETURN_CODE setInitialBitrate(int bitrate);
	VOCommonPlayerConfiguration.prototype.setInitialBitrate = function(bitrate)
	{
		var ret = this.player.setInitialBitrate(bitrate);
		return ret;
	};


	/**
	 * Get player engine type.
	 *
	 * @return Player engine type. Refer to VO_OSMP_PLAYER_ENGINE.
	 */
  //* VO_OSMP_PLAYER_ENGINE getPlayerType();
	VOCommonPlayerConfiguration.prototype.getPlayerType = function()
	{
		return this.player.getPlayerType();
	};


	/**
	 * Set video display area.
	 * Override automatic screen width and height adjustment according to zoom mode.
	 *
	 * @param mode  [in] Zoom mode. Refer to VO_OSMP_ZOOM_MODE.
	 * @param rect  [in] Rectangular area to be displayed.
	 *
	 * @return VO_OSMP_ERR_NONE if successful.
	 */
  //* VO_OSMP_RETURN_CODE setZoomMode(VO_OSMP_ZOOM_MODE mode, Rect rect);
	VOCommonPlayerConfiguration.prototype.setZoomMode = function(mode, rect)
	{
		var ret = this.player.setZoomMode(mode, rect);
		return ret;
	};

    /**
     * Get minimum position.
     *
     * <p><ul>
     * <li> For VOD: returns 0
     * <li> For Live: returns (getMaxPosition - DVR window size)
     * </ul></p>
     *
     * @return The position (ms); -1 if unsuccessful
     */
  //* long getMinPosition();
	VOCommonPlayerConfiguration.prototype.getMinPosition = function()
	{
		return this.player.getMinPosition();
	};   


    /**
     * Get maximum position.
     *
     * <p><ul>
     * <li> For VOD: returns the duration
     * <li> For Live: depends on {@link VOCommonPlayerConfiguration#enableLiveStreamingDVRPosition}
     *  <ol>
     *  <li>If enabled: always returns 0, which represents the live playhead
     *  <li>If disabled: returns the offset between the live playhead and the beginning of the playback
     * </ul></p>
     *
     * @return The position (ms); -1 if unsuccessful
     */
  //* long getMaxPosition();
	VOCommonPlayerConfiguration.prototype.getMaxPosition = function()
	{
		return this.player.getMaxPosition();
	}

      //
	// Set DRM library info.
	//
	/**
	 * Set DRM Library name and API name.
	 *
	 * @param libName    [in] DRM library name.
	 * @param libApiName [in] DRM library export api name.
	 *
	 * @return VO_OSMP_ERR_NONE if successful.
	 *
	 * NOTE: This API must be called after initializing the player but before opening the media source calling the open method.
	 */
  //* VO_OSMP_RETURN_CODE setDRMLibrary(String libName, String libApiName);
	VOCommonPlayerConfiguration.prototype.setDRMLibrary = function(libName, libApiName)
	{
		var ret = this.player.setDRMLibrary(libName, libApiName);
		return ret;
	};

	/**
	 * Set the verification information to start DRM verification.
	 *
	 * @param   info   [in] Verification information. Refer to VOOSMPVerificationInfo.
	 *
	 * @return VO_OSMP_ERR_NONE if successful.
	 */
  //* VO_OSMP_RETURN_CODE setDRMVerificationInfo(VOOSMPVerificationInfo info);
	VOCommonPlayerConfiguration.prototype.setDRMVerificationInfo = function(info)
	{
		var ret = this.player.setDRMVerificationInfo(info);
		return ret;
	};


	/**
	 * Set IMEI or MAC of the device.
	 *
	 * @param   string [in] Unique identifier string.
	 *
	 * @return  VO_OSMP_ERR_NONE if successful.
	 */
  //* VO_OSMP_RETURN_CODE setDRMUniqueIdentifier(string string);
	VOCommonPlayerConfiguration.prototype.setDRMUniqueIdentifier = function(string)
	{
		var ret = this.player.setDRMUniqueIdentifier(string);
		return ret;
	};

	/**
	 * Get IMEI or MAC of the device.
	 *
	 * @return  Unique identifier string.
	 */
  //* string getDRMUniqueIdentifier();
	VOCommonPlayerConfiguration.prototype.getDRMUniqueIdentifier = function()
	{
		return this.player.getDRMUniqueIdentifier();
	};
	/**
	 * Set IMEI or MAC of the device.
	 *
	 * @param   string [in] Unique identifier string.
	 *
	 * @return  VO_OSMP_ERR_NONE if successful.
	 */
  //* VO_OSMP_RETURN_CODE setDRMUniqueIndentifier(string string);
	VOCommonPlayerConfiguration.prototype.setDRMUniqueIndentifier = function(string)
	{
		var ret = this.player.setDRMUniqueIdentifier(string);
		return ret;
	};

	/**
	 * Get IMEI or MAC of the device.
	 *
	 * @return  Unique identifier string.
	 */
  //* string getDRMUniqueIndentifier();
	VOCommonPlayerConfiguration.prototype.getDRMUniqueIndentifier = function()
	{
		return this.player.getDRMUniqueIdentifier();
	};


	/**
	 * Set the initial buffering time of for playback start.
	 *
	 * @param   time [in] buffer time (Microseconds)
	 *
	 * @return  VO_OSMP_ERR_NONE if successful.
	 */
  //* VO_OSMP_RETURN_CODE setInitialBufferTime(int time);
	VOCommonPlayerConfiguration.prototype.setInitialBufferTime = function(time)
	{
		var ret = this.player.setInitialBufferTime(time);
		return ret;
	};


	/**
	 * Enable/Disable playback of the audio stream. The default is enabled.
	 *
	 * @param   value  [in] True to enable playback, false to disable playback.
	 *
	 * @return  VO_OSMP_ERR_NONE if successful.
	 *
	 */
  //* VO_OSMP_RETURN_CODE enableAudioStream(boolean value);
	VOCommonPlayerConfiguration.prototype.enableAudioStream = function(value)
	{
		var ret = this.player.enableAudioStream(value);
		return ret;
	};


	/**
	 * Enable/Disable playback of the video stream. The default is enabled.
	 *
	 * @param   value  [in] True to enable playback, false to disable playback.
	 *
	 * @return  VO_OSMP_ERR_NONE if successful.
	 *
	 */
  //* VO_OSMP_RETURN_CODE enableVideoStream(boolean value);
	VOCommonPlayerConfiguration.prototype.enableVideoStream = function(value)
	{
		var ret = this.player.enableVideoStream(value);
		return ret;
	};


	/**
	 * Enable/Disable immediate video rendering with low latency.
	 * This function is only applicable to video-only streams. The default is disabled.
	 *
	 * @param   value  [in] True to enable playback, false to disable playback.
	 *
	 * @return  VO_OSMP_ERR_NONE if successful.
	 *
	 */
  //* VO_OSMP_RETURN_CODE enableLowLatencyVideo(boolean value);
	VOCommonPlayerConfiguration.prototype.enableLowLatencyVideo = function(value)
	{
		var ret = this.player.enableLowLatencyVideo(value);
		return ret;
	};


	/**
	 * Set audio playback speed.
	 *
	 * @param   speed  [in] Speed multiplier with respect to real-time playback. Default is 1.0.
	 *
	 * @return  VO_OSMP_ERR_NONE if successful.
	 *
	 */
  //* VO_OSMP_RETURN_CODE setAudioPlaybackSpeed(float speed);
	VOCommonPlayerConfiguration.prototype.setAudioPlaybackSpeed = function(speed)
	{
		var ret = this.player.setAudioPlaybackSpeed(speed);
		return ret;
	};

	/**
	 * Enable/Disable audio effect. The default is disabled.
	 *
	 * @param   value  [in] True to enable effect, false to disable effect.
	 *
	 * @return  VO_OSMP_ERR_NONE if successful.
	 *
	 */
  //* VO_OSMP_RETURN_CODE enableAudioEffect(boolean value);
	VOCommonPlayerConfiguration.prototype.enableAudioEffect = function(value)
	{
		var ret = this.player.enableAudioEffect(value);
		return ret;
	};
	
    /**
	* Set the presentation delay time for a live streaming source.
	*
	* @param   time [in] Presentation delay time (milliseconds) for a live streaming source with respective to current time.
	*
	* @return  VO_OSMP_ERR_NONE if successful.
	*/
  //* VO_OSMP_RETURN_CODE setPresentationDelay(int time);
	VOCommonPlayerConfiguration.prototype.setPresentationDelay = function(time)
	{
		var ret = this.player.setPresentationDelay(time);
		return ret;
	};

       /**
        * Set the buffering time to continue playback before the need to buffer again.
        *
        * @param  time [in] Buffer time (milliseconds).
        *
        * @return VO_OSMP_ERR_NONE if successful.
        */
  //* VO_OSMP_RETURN_CODE setAnewBufferingTime(int time);
        VOCommonPlayerConfiguration.prototype.setAnewBufferingTime = function(time)
        {
                var ret = this.player.setAnewBufferingTime(time);
                return ret;
        };
        
        /**
        * Show or hide cursor.
        *
        * @param bShow [in] True to display plugin cursor; false to hide it.
        *
        * @return No return value.
        */
  //* null showCursor(int time);        
        VOCommonPlayerConfiguration.prototype.showCursor = function(bShow)
        {
                var ret = this.player.showCursor(bShow);
		return ret;
        };
        


        /**
	 * Enable defining position based on live streaming DVR window. The default is enabled.
	 * <p>
	 * When enabled:
	 * <p><ul>
	 * <li> {@link VOCommonPlayerControl#getPosition} returns 0 when playing at the live head. A negative position represents
	 *      the offset into the DVR window with respect to live head
	 * <li> {@link VOCommonPlayerConfiguration#getMaxPosition} always returns 0 which represent the live head
	 * <li> {@link VOCommonPlayerConfiguration#getMinPosition} returns (getMaxPosition - DVR window size)
	 * </ul><p>
	 *
	 * When disabled:
	 * <p><ul>
	 * <li> {@link VOCommonPlayerControl#getPosition} returns the offset with respect to the beginning of the playback
	 * <li> {@link VOCommonPlayerConfiguration#getMaxPosition} returns the offset between the live head and the beginning of the playback
	 * <li> {@link VOCommonPlayerConfiguration#getMinPosition} returns (getMaxPosition - DVR window size)
	 * <ul><p>
	 * <p>
	 *
	 * @param   value  [in] Enable/Disable; true to enable(default), false to disable.
	 *
	 * @return  {@link VO_OSMP_ERR_NONE} if successful
	 */
	VOCommonPlayerConfiguration.prototype.enableLiveStreamingDVRPosition = function(enable)
        {
                var ret = this.player.enableLiveStreamingDVRPosition(enable);
                return ret;
        };

	/**
	 * Enable/Disable processing SEI information.
	 *
	 * @param   flag [in] the flag {@link VO_OSMP_SEI_INFO_FLAG}. Set to VO_OSMP_SEI_INFO_NONE to disable processing SEI information or any other flags to enable
	 *
	 * @return  {@link VO_OSMP_ERR_NONE} if successful
	 */
  //* VO_OSMP_RETURN_CODE enableSEI(VO_OSMP_SEI_INFO_FLAG flag);
	VOCommonPlayerConfiguration.prototype.enableSEI = function(flag)
        {
                var ret = this.player.enableLiveStreamingDVRPosition(flag);
                return ret;
        };

    /**
     * Get version information of a module.
     * 
     * The OSMP+ SDK is currently built using the same version for all modules.
     * Instead of passing a module as an argument, simply pass VO_OSMP_MODULE_TYPE_SDK
     *
     * @param   module [in] module type.
     *
     * @return  version information of the given module
     */
  //* string getVersion(VO_OSMP_MODULE_TYPE module);
	VOCommonPlayerConfiguration.prototype.getVersion = function(module)
        {
                var ret = this.player.getVersion(module);
                return ret;
        };

    /**
      * Enable/Disable CPU adaptation. The default is enabled.
      *
      * @param   value  [in] Enable/Disable; true to enable, false to disable.
      *
      * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful
      *
      */
   //* VO_OSMP_RETURN_CODE enableCPUAdaptation(boolean value);
        VOCommonPlayerConfiguration.prototype.enableCPUAdaptation = function(value)
        {
                var ret = this.player.enableCPUAdaptation(value);
                return ret;
        };

    /**
     * Set upper/lower bitrate threshold for adaptation
     *
     * @param upper [in] Maximum bitrate that the adaptation must not exceed (bps) in addition to the constraint defined in the device capability file {@link VOCommonPlayerControl#setDeviceCapabilityByFile}.
     * <p><ul>
     * <li> If the value is 0, the maximum bitrate is constrained by the device capability file if it is specified. Otherwise the maximum bitrate is unconstrained.
     * <li> If the value is not 0 and the device capability file is not specified, then it defines the maximum bitrate threshold
     * <li> If the value is not 0 and the device capability file is specified, then the maximum bitrate threshold is defined as min(upper, max bitrate in device capability file)
     * </ul><p>
     * @param lower [in] Mimimum bitrate that the adaptation must not drop below (bps).
     * <p><ul>
     * <li> If the value is 0, then the minimum threshold is unconstrained
     * <li> If the value is not 0, then it defined the minimum bitrate threshold that adaptation must not drop below.
     * </ul><p>
     * @return {@link VO_OSMP_ERR_NONE} if successful.
     *
     * <pre> This API should be called before calling the {@link VOCommonPlayerControl#open:flag:sourceType:openParam:} method. </pre>
     *
     */
  //* int setBitrateThreshold(int upper, int lower);
   	VOCommonPlayerConfiguration.prototype.setBitrateThreshold = function(upper, lower)
	{
		var ret = this.player.setBitrateThreshold(upper, lower);
		return ret;
	};

    /**
     * Set the maximum number of retry attempts to download a corresponding segment from a
     * different bit rate due to DRM errors, server issues, or unsupported media format.
     *
     * @param times [in] Maximum number of retry attempts. The default is 0.
     * @return {@link VO_OSMP_ERR_NONE} if successful.
     *
     * <pre>This API should be called before calling the {@link VOCommonPlayerControl#open:flag:sourceType:openParam:} method.</pre>
     *
     */
  //* int setSegmentDownloadRetryCount(int times);
   	VOCommonPlayerConfiguration.prototype.setSegmentDownloadRetryCount = function(times)
	{
		var ret = this.player.setSegmentDownloadRetryCount(times);
		return ret;
	};


    /**
     * Enable/Disable loading Dolby libraries. The default is enable.
     *
     * @param   value  [in] Enable/Disable; true to enable, false to disable.
     *
     * @return  {@link VO_OSMP_ERR_NONE} if successful
     *
     */
  //* int enableDolbyLibrary(bool value);
   	VOCommonPlayerConfiguration.prototype.enableDolbyLibrary = function(value)
	{
		var ret = this.player.enableDolbyLibrary(value);
		return ret;
	};

};
//* }

/******************************************************************************************************************/
//* public interface VOCommonPlayerHTTPConfiguration {
function VOCommonPlayerHTTPConfiguration(pluginobject) {

	this.player = pluginobject;

	/**
	 * Set the verification information to start HTTP verification.
	 *
	 * @param verificationData [in] Data that is needed to do verification.
	 *
	 * @param dataFlag         [in] Data flag. If it is 0, it is the request string, 1 is the password, other values are not currently supported.
	 *
	 * @return Authentication response string.
	 *
	 */
  //* String setHTTPVerificationInfo(String verificationData, int dataFlag);
	VOCommonPlayerHTTPConfiguration.prototype.setHTTPVerificationInfo = function(verificationData, dataFlag)
	{
		return this.player.setHTTPVerificationInfo(verificationData, dataFlag);
	};


	/**
	 * set HTTP header.
	 *
	 * @param headerName  [in] Set-Cookie: to set cookie;  User-Agent: to set user agent.
	 *
	 * @param headerValue [in] Corresponding value of 'headername'.
	 *
	 *						If 'Set-Cookie', it should be a string in JSON format as below.
	 *
	 *							cookie name: Cookie value.
	 *							domain:      Domain value.
	 *							path:        Path value.
	 *							expires:     Expiration value.
	 *							secure:      True or false.
	 *
	 *
	 *						If setting 'User-Agent', it should be a string value of the agent info as below.
	 *
	 *							Android: VisualOn OSMP+ Player(Android)
	 *							iOS:     VisualOn OSMP+ Player(iOS)
	 *							Windows: VisualOn OSMP+ Player(Windows)
	 *
	 * @return  VO_OSMP_ERR_NONE if successful.
	 */
  //* VO_OSMP_RETURN_CODE setHTTPHeader(string headerName, string headerValue);
	VOCommonPlayerHTTPConfiguration.prototype.setHTTPHeader = function(headerName, headerValue)
	{
		var ret = this.player.setHTTPHeader(headerName, headerValue);
		return ret;
	};


	/**
	 * Set HTTP proxy.
	 *
	 * @param   proxy   [in]  A string in JSON format, i.e.:
	 *
	 *						pszProxyHost: Proxy server host name or IP address, must not be null.
	 *						nProxyPort:   Proxy server port number, must not be null.
	 *
	 * @return  VO_OSMP_ERR_NONE if successful.
	 */
  //* VO_OSMP_RETURN_CODE setHTTPProxy(string proxy);
	VOCommonPlayerHTTPConfiguration.prototype.setHTTPProxy = function(proxy)
	{
		var ret = this.player.setHTTPProxy(proxy);
		return ret;
	};

    /*
     * Set HTTP connection retry timeout.
     *
     * The setting is applicable only after data source has been opened successfully.
     *
     * @param time [in] interval, in seconds, in which the player will attempt to re-establish the HTTP connection. The default is 120 seconds. Setting the value to -1 disables the timeout so that the player will keep retrying, until the connection is established again
     *
     * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful
     */
  //* VO_OSMP_RETURN_CODE setHTTPRetryTimeout(int time);
	VOCommonPlayerHTTPConfiguration.prototype.setHTTPRetryTimeout = function(time)
	{
		var ret = this.player.setHTTPRetryTimeout(time);
		return ret;
	};

};
//* }

/******************************************************************************************************************/
//* public interface VOCommonPlayerRTSPConfiguration {
function VOCommonPlayerRTSPConfiguration(pluginobject) {

	this.player = pluginobject;

	/**
	 * Set RTSP connection type. The default is VO_OSMP_RTSP_CONNECTION_AUTOMATIC.
	 *
	 * @param   type   [in] Connection type. Refer to VO_OSMP_RTSP_CONNECTION_TYPE.
	 *
	 * @return  VO_OSMP_ERR_NONE if successful.
	 */
  //* VO_OSMP_RETURN_CODE setRTSPConnectionType(String type);
	VOCommonPlayerRTSPConfiguration.prototype.setRTSPConnectionType = function(type)
	{
		var ret = this.player.setRTSPConnectionType(type);
		return ret;
	};

	/**
	 * Set port number for RTSP connection
	 *
	 * @param   portNum   [in] port number as a string type in JSON format as below.
	 *
	 *						    audioConnectionPort: [int] audio port number.
	 *						    videoConnectionPort: [int] video port number.
	 *
	 *
	 * @return  VO_OSMP_ERR_NONE if successful.
	 */
  //* VO_OSMP_RETURN_CODE setRTSPConnectionPort(string portNum);
	VOCommonPlayerRTSPConfiguration.prototype.setRTSPConnectionPort = function(portNum)
	{
		var ret = this.player.setRTSPConnectionPort(portNum);
		return ret;
	};

    /**
     * Get the rtsp RTSP module status value
     *
     * @return  A {@link VOOSMPRTSPStatistics} object if successful or null if failed.
     *
     */
  //* RTSPStatistics getRTSPStatistics();
	VOCommonPlayerRTSPConfiguration.prototype.getRTSPStatistics = function()
	{
		return this.player.getRTSPStatistics();
	};

    /**
     * Enable RTSP over HTTP tunneling. The default is disable.
     *
     * @param   enable  [in] Enable/Disable; true to enable, false to disable(default).
     *
     * @return  {@link VO_OSMP_ERR_NONE} if successful
     */
  //* int enableRTSPOverHTTP(bool enable);
	VOCommonPlayerRTSPConfiguration.prototype.enableRTSPOverHTTP = function(enable)
	{
		return this.player.enableRTSPOverHTTP(enable);
	};

    /**
     * Set port number for RTSP over HTTP tunneling.
     *
     * @param   portNum   [in] port number
     *
     * @return  {@link VO_OSMP_ERR_NONE} if successful
     */
  //* int setRTSPOverHTTPConnectionPort(int portNum);
	VOCommonPlayerRTSPConfiguration.prototype.setRTSPOverHTTPConnectionPort = function(portNum)
	{
		return this.player.setRTSPOverHTTPConnectionPort(portNum);
	};


};
//* }

/********************************************************************************************************************************/
//* public interface VOCommonPlayerSubtitle {
function VOCommonPlayerSubtitle(pluginobject) {

	this.player = pluginobject;

	/**
	 * Set full path for external subtitle file. e.g.: smi, srt, etc.
	 *
	 * @param  filePath Location of the subtitle file.
	 * 
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE setSubtitlePath(string filePath);
	VOCommonPlayerSubtitle.prototype.setSubtitlePath = function(filePath)
	{
		var ret = this.player.setSubtitlePath(filePath);
		return ret;
	};


	/**
	 * To display/hide subtitle. Default is false.
	 *
	 * @param  val	True to display subtitle, false to hide subtitle.
	 * 
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE enableSubtitle(boolean val);
	VOCommonPlayerSubtitle.prototype.enableSubtitle = function(val)
	{
		var ret = this.player.enableSubtitle(val);
		return ret;
	};


	/**
	 * Set subtitle font color.
	 *
	 * @param color Font color (such as 0x00RRGGBB) of subtitle text.
	 * 
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE setSubtitleFontColor(int color);
	VOCommonPlayerSubtitle.prototype.setSubtitleFontColor = function(color)
	{
		var ret = this.player.setSubtitleFontColor(color);
		return ret;
	};


	/**
	 * Set subtitle font color opacity rate.
	 *
	 * @param alpha Font color opacity rate. The valid range is 0 to 100, from transparent to opaque.
	 * 
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE setSubtitleFontOpacity(int alpha);
	VOCommonPlayerSubtitle.prototype.setSubtitleFontOpacity = function(alpha)
	{
		var ret = this.player.setSubtitleFontOpacity(alpha);
		return ret;
	};


	/**
	 * Set subtitle font size scale.
	 *
	 * @param scale Font size scale for subtitle text. The valid range is 50 to 200, 
	 * where 50 is the smallest and is half of the default size, and 200 is the largest and is twice the default size.
	 * 
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE setSubtitleFontSizeScale(int scale);
	VOCommonPlayerSubtitle.prototype.setSubtitleFontSizeScale = function(scale)
	{
		var ret = this.player.setSubtitleFontSizeScale(scale);
		return ret;
	};


	/**
	 * Set subtitle font background color.
	 *
	 * @param color Subtitle font background color (such as 0x00RRGGBB).
	 * 
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE setSubtitleFontBackgroundColor(int color);
	VOCommonPlayerSubtitle.prototype.setSubtitleFontBackgroundColor = function(color)
	{
		var ret = this.player.setSubtitleFontBackgroundColor(color);
		return ret;
	};


	/**
	 * Set subtitle font background color opacity rate.
	 *
	 * @param alpha Subtitle font background color opacity rate. The valid range is 0 to 100, from transparent to opaque.
	 * 
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE setSubtitleFontBackgroundOpacity(int alpha);
	VOCommonPlayerSubtitle.prototype.setSubtitleFontBackgroundOpacity = function(alpha)
	{
		var ret = this.player.setSubtitleFontBackgroundOpacity(alpha);
		return ret;
	};


	/**
	 * Set window background color.
	 *
	 * @param color Subtitle window background color (such as 0x00RRGGBB).
	 * 
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE setSubtitleWindowBackgroundColor(int color);
	VOCommonPlayerSubtitle.prototype.setSubtitleWindowBackgroundColor = function(color)
	{
		var ret = this.player.setSubtitleWindowBackgroundColor(color);
		return ret;
	};


	/**
	 * Set window background color opacity rate.
	 *
	 * @param alpha Subtitle window background color opacity rate. The valid range is 0 to 100, from transparent to opaque.
	 * 
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE setSubtitleWindowBackgroundOpacity(int alpha);
	VOCommonPlayerSubtitle.prototype.setSubtitleWindowBackgroundOpacity = function(alpha)               
	{
		var ret = this.player.setSubtitleWindowBackgroundOpacity(alpha);
		return ret;
	};


	/**
	 * Set subtitle font italic.
	 *
	 * @param  enable  True to set subtitle font italic.
	 * 
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE setSubtitleFontItalic(boolean enable);
	VOCommonPlayerSubtitle.prototype.setSubtitleFontItalic = function(enable)
	{
		var ret = this.player.setSubtitleFontItalic(enable);
		return ret;
	};


	/**
	 * Set subtitle font bold.
	 *
	 * @param  enable  True to set subtitle font bold.
	 * 
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE setSubtitleFontBold(boolean enable);
	VOCommonPlayerSubtitle.prototype.setSubtitleFontBold = function(enable)
	{
		var ret = this.player.setSubtitleFontBold(enable);
		return ret;
	};


	/**
	 * Set subtitle font underlined.
	 *
	 * @param  enable  True to set subtitle font underlined.
	 * 
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE setSubtitleFontUnderline(boolean enable);
	VOCommonPlayerSubtitle.prototype.setSubtitleFontUnderline = function(enable)
	{
		var ret = this.player.setSubtitleFontUnderline(enable);
		return ret;
	};


	/**
	 * Set subtitle font name.
	 *
	 * @param fontname Font name for subtitle text.
	 * 
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE setSubtitleFontName(string fontname);
	VOCommonPlayerSubtitle.prototype.setSubtitleFontName = function(fontname)
	{
		var ret = this.player.setSubtitleFontName(fontname);
		return ret;
	};


	/**
	 * Set subtitle font edge type.
	 *
	 * @param edgetype Edge type of subtitle font. For more details, please refer to the relative values defined in VOOSMPType.js.
	 * 
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE setSubtitleFontEdgeType(int edgetype);
	VOCommonPlayerSubtitle.prototype.setSubtitleFontEdgeType = function(edgetype)
	{
		var ret = this.player.setSubtitleFontEdgeType(edgetype);
		return ret;
	};


	/**
	 * Set subtitle font edge color.
	 *
	 * @param color  Font edge color (such as 0x00RRGGBB) of subtitle text.
	 * 
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE setSubtitleFontEdgeColor(int color);
	VOCommonPlayerSubtitle.prototype.setSubtitleFontEdgeColor = function(color)
	{
		var ret = this.player.setSubtitleFontEdgeColor(color);
		return ret;
	};


	/**
	 * Set subtitle font edge color opacity rate.
	 *
	 * @param alpha Edge color opacity rate of subtitle font. The valid range is 0 to 100, from transparent to opaque.
	 * 
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE setSubtitleFontEdgeOpacity(int alpha);
	VOCommonPlayerSubtitle.prototype.setSubtitleFontEdgeOpacity = function(alpha)
	{
		var ret = this.player.setSubtitleFontEdgeOpacity(alpha);
		return ret;
	};


	/**
	 * Reset all parameters to their default values. Subtitles will be presented as specified in the subtitle stream.
	 *
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE resetSubtitleParameter();
	VOCommonPlayerSubtitle.prototype.resetSubtitleParameter = function()
	{
		var ret = this.player.resetSubtitleParameter();
		return ret;
	}
};
//* }

/********************************************************************************************************************************/
//* public interface VOCommonPlayerAssetSelection {
function VOCommonPlayerAssetSelection(pluginobject) {

	this.player = pluginobject;

	/** 
	 * Get number of available video streams.
	 *
	 * <pre>
	 * A video stream is defined as a combination of 
	 * bitrate and camera angle (merge internal stream 
	 * and video track).
	 * </pre>
	 * 
	 * @return Number of available video streams, -1 if failed.
	 */           
  //* int getVideoCount();
	VOCommonPlayerAssetSelection.prototype.getVideoCount = function()
	{
		return this.player.getVideoCount();
	};


	/**
	 * Get number of available audio streams.
	 *
	 * <pre>
	 * An audio stream is defined as a combination of 
	 * language and codec type. An audio stream may have
	 * multiple audio bitrates. The final playback bitrate 
	 * depends on selected video stream.
	 * </pre>
	 *  
	 * @return Number of available audio streams, -1 if failed.

	 */
  //* int getAudioCount();
	VOCommonPlayerAssetSelection.prototype.getAudioCount = function()
	{
		return this.player.getAudioCount();
	};


	/** 
	 * Get number of available subtitle streams.
	 *
	 * @return Number of available subtitle streams, -1 if failed.
	 */
  //* int getSubtitleCount();
	VOCommonPlayerAssetSelection.prototype.getSubtitleCount = function()
	{
		return this.player.getSubtitleCount();
	};


	/**
	 * Select video stream by its index.
	 *
	 * @param     videoindex  Index of video stream, starting from 0. 
	 *            The availability of video streams depends on current selection of
	 *            uncommitted audio and subtitle streams.
	 *
	 * @return   0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE selectVideo(int videoindex);
	VOCommonPlayerAssetSelection.prototype.selectVideo = function(videoindex)
	{
		var ret = this.player.selectVideo(videoindex);
		return ret;
	};


	/**
	 * Select audio stream by its index.
	 *
	 * @param     audioindex Index of audio stream, starting from 0.
	 *            The availability of audio streams depends on current selection of
	 *            uncommitted video and subtitle streams.
	 *
	 * @return    VO_OSMP_ERR_NONE if successful.
	 */
  //* VO_OSMP_RETURN_CODE selectAudio(int audioindex);
	VOCommonPlayerAssetSelection.prototype.selectAudio = function(audioindex)
	{
		var ret = this.player.selectAudio(audioindex);
		return ret;
	};


	/**
	 * Select subtitle stream by its index.
	 *
	 * @param     subtitleindex Index of subtitle stream, starting from 0.
	 *            The availability of subtitle selections depends on current
	 *            selection of uncommitted video and audio selections.
	 *
	 * @return    VO_OSMP_ERR_NONE if successful.
	 */
  //* VO_OSMP_RETURN_CODE selectSubtitle(int subtitleindex);
	VOCommonPlayerAssetSelection.prototype.selectSubtitle = function(subtitleindex)
	{
		var ret = this.player.selectSubtitle(subtitleindex);
		return ret;
	};


	/**
	 * Check if specified video stream is available for selection.
	 *
	 * @param     videoindex Index of video stream, starting from 0.
	 *            The availability of video streams depends on current selection of
	 *            uncommitted audio and subtitle streams.
	 *
	 * @return    True if specified video stream is available, false if not.
	 */
  //* boolean isVideoAvailable(int videoindex);
	VOCommonPlayerAssetSelection.prototype.isVideoAvailable = function(videoindex)
	{
		return this.player.isVideoAvailable(videoindex);
	};


	/**
	 * Check if specified audio stream is available for selection.
	 *
	 * @param     audioindex Index of audio stream, starting from 0.
	 *            The availability of audio streams depends on current selection of
	 *            uncommitted video and subtitle streams.
	 *
	 * @return    True if specified video stream is available, false if not.
	 */
  //* boolean isAudioAvailable(int audioindex);
	VOCommonPlayerAssetSelection.prototype.isAudioAvailable = function(audioindex)
	{
		return this.player.isAudioAvailable(audioindex);
	};


	/**
	 * Check if specified subtitle stream is available for selection.
	 *
	 * @param     subtitleindex Index of subtitle stream, starting from 0.
	 *            The availability of subtitle streams depends on current selection of
	 *            uncommitted video and audio streams.
	 *
	 * @return    True if specified video stream is available, false if not.
	 */
  //* boolean isSubtitleAvailable(int subtitleindex);
	VOCommonPlayerAssetSelection.prototype.isSubtitleAvailable = function(subtitleindex)
	{
		return this.player.isSubtitleAvailable(subtitleindex);
	};


	/**
	 * Commit all current asset selections. If any asset type is not selected, current playing asset of that type is used.
	 * This operation will remove all current selections after commit.
	 *
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE commitSelection();
	VOCommonPlayerAssetSelection.prototype.commitSelection = function()
	{
		var ret = this.player.commitSelection();
		return ret;
	};


	/**
	 * Remove all current uncommitted selections.
	 *
	 * @return 0 if successful.
	 */
  //* VO_OSMP_RETURN_CODE clearSelection();
	VOCommonPlayerAssetSelection.prototype.clearSelection = function()
	{
		var ret = this.player.clearSelection();
		return ret;
	};


	/**
	 * Retrieve the properties of specified video stream.
	 *
	 * @param  videoindex Index of video stream, starting from 0.
	 *
	 * @return Detailed video property values. Key and value are string type.
	 *                   Format is in JSON if successful.
	 *
	 *		      description: Description value.
	 *                    codec:       Codec value.
	 *                    bitrate:     Bitrate value.
	 *                    width:       Width value.
	 *                    height:      Height value.
	 */
  //* VOOSMPAssetProperty getVideoProperty(int videoindex);
	VOCommonPlayerAssetSelection.prototype.getVideoProperty = function(videoindex)
	{
		var fvp = this.player.getVideoProperty(videoindex);
		if (fvp.length == 0)
			return "";

		var vp = eval ("(" + fvp + ")");
		return vp;
	};


    /**
     * Get the properties of specified audio track.
     *
     * @param    index  [in] Index of audio track property. Valid from 0 to {@link VOOSMPAssetProperty#getPropertyCount} - 1.
     *
     * @return   a {@link VOOSMPAssetProperty} object if successful; nil if unsuccessful
     *           Returned object holds a list of key-value pairs where both key and value are
     *           of NSString* type.
     *           Supported property keys are:
     *                   @"description"
     *                   @"language"
     *                   @"codec"
     *                   @"bitrate"
     *                   @"channelcount"
     */
  //* VOOSMPAssetProperty getAudioProperty(int audioindex);
	VOCommonPlayerAssetSelection.prototype.getAudioProperty = function(audioindex)
	{
		var fap = this.player.getAudioProperty(audioindex);
		if (fap.length == 0)
			return "";

		var ap = eval ("(" + fap + ")");
		return ap;
	};


	/**
	 * Retrieve the properties of specified subtitle stream.
	 *
	 * @param     subtitleindex Index of subtitle stream, starting from 0
	 *
	 * @return    Detailed subtitle properties. Key and value are string type.
	 *                   Format is in JSON if successful.
	 * 
	 *                   description:        Description value.
	 *                   language: 		 Language value.
	 *                   codec:              Codec value.
	 */
  //* VOOSMPAssetProperty getSubtitleProperty(int subtitleindex);
	VOCommonPlayerAssetSelection.prototype.getSubtitleProperty = function(subtitleindex)
	{
		var fsp = this.player.getSubtitleProperty(subtitleindex);
		if (fsp.length == 0)
			return "";

		var sp = eval ("(" + fsp + ")");
		return sp;
	};


	/**
	 * Retrieve the assets which are currently being played or default assets before playing.
	 *
	 * @return    A string of index set of current IDs of selected video, audio, and subtitle.
	 * If no track of a specific type (mostly likely subtitle) is being played, then the corresponding index is set to VO_OSMP_ASSET_NOT_SELECTED. 
	 *					Format is in JSON if successful.
	 *
	 *					audio:    Audio track index.
	 *					video:    Video track index.
	 *					subtitle: Subtitle track index. 
	 */
  //* VOOSMPAssetIndex getPlayingAsset();
	VOCommonPlayerAssetSelection.prototype.getPlayingAsset = function()
	{
		var fpa = this.player.getPlayingAsset();
		if (fpa.length == 0)
			return "";

		var pa = eval ("(" + fpa + ")");
		return pa;
	};

	/**
	 * Retrieve the current track selection of each type.
	 *
	 * @return    A string of index set of current IDs of selected video, audio, and subtitle track.
	 * If a track is selected automatically, then the corresponding index is set to VO_OSMP_ASSET_AUTO_SELECTED.
	 * If no track of a specific type is being selected automatically/manually, then the corresponding index is set to VO_OSMP_ASSET_NOT_SELECTED. 
	 *
	 *					Format is in JSON if successful.
	 *
	 *					audio:    Audio track index.
	 *					video:    Video track index.
	 *					subtitle: Subtitle track index.
	 */
  //* VOOSMPAssetIndex getCurrentSelection();
	VOCommonPlayerAssetSelection.prototype.getCurrentSelection = function()
	{
		var fpa = this.player.getCurrentSelection();
		if (fpa.length == 0)
			return "";

		var pa = eval ("(" + fpa + ")");
		return pa;
	}
};
//* }


/********************************************************************************************************************************/
//* public interface VOOSMPAnalyticsInfo {
function VOOSMPAnalyticsInfo(pluginobject) {

	this.player = pluginobject;

	/**
	 * Get video decoding bitrate for each of the last 10 seconds
	 *
	 * @return an array of size 10 in which elements are the video 
	 *         decoding bitrates of each of the last 10 seconds.  
	 *         The bitrate information is arranged in the way that 
	 *         the smaller the array index is, the closer 
	 *         the bitrate is to the current time. That is, 
	 *         element 0 is the bitrate of the closest second 
	 *         before the current time; element 1 is the bitrate 
	 *         that is 1 second before element 0; element 2 is the 
	 *         bitrate that is 1 second before element 1, and so on.
	 */
	VOOSMPAnalyticsInfo.prototype.getVideoDecodingBitrate = function()
	{
		var ret = this.player.getVideoDecodingBitrate();
		return ret;
	};


	/**
	 * Get audio decoding bitrate for each of the last 10 seconds
	 *
	 * @return an array of size 10 in which elements are the audio 
	 *         decoding bitrates of each of the last 10 seconds.  
	 *         The bitrate information is arranged in the way that 
	 *         the smaller the array index is, the closer 
	 *         the bitrate is to the current time. That is, 
	 *         element 0 is the bitrate of the closest second 
	 *         before the current time; element 1 is the bitrate 
	 *         that is 1 second before element 0; element 2 is the 
	 *         bitrate that is 1 second before element 1, and so on.
	 */
	VOOSMPAnalyticsInfo.prototype.getAudioDecodingBitrate = function()
	{
		var ret = this.player.getAudioDecodingBitrate();
		return ret;
	};

	//
	// Analytics.
	//
	/**
	 * Set cache time and start analytics data collection.
	 *
	 * @param cacheTime [in] Time (seconds) to cache the analytics data.
	 *
	 * @return VO_OSMP_ERR_NONE if successful.
	 */
  //* VO_OSMP_RETURN_CODE enableAnalytics(int cacheTime);
	VOOSMPAnalyticsInfo.prototype.enableAnalytics = function(cacheTime)
	{
		var ret = this.player.enableAnalytics(cacheTime);
		return ret;
	};

	/**
	 * Get current analytics data.
	 *
	 * @param filter    [in] Specified as a string in JSON format as below, which is used to cache analytic data before analytics is stopped.
	 *         
	 *                   lastTime:   A value in seconds, which indicates that how far backwards to report it.
	 *                   sourceTime: A value in ms-seconds, which results in a Number that would be recorded once the source exceeds it.
	 *                   codecTime:  A value in ms-seconds, which results in a Number of instances that would be recorded once codec exceeds it.
	 *                   renderTime: A value in ms-seconds, which results in a Number of instances that will be recorded when the render exceeds it.
	 *                   jitter:     A value in ms-seconds, which results in a Number of instances that will be recorded when the jitter exceeds it.
	 *
	 * @return JSON object if successful; null if failed.
	 *
	 *			lastTime:          How far back to report (seconds).
	 *			sourceDropNum:     Number of source dropped frames.
	 *			codecDropNum:      Number of codec dropped frames.
	 *			renderDropNum:     Number of render dropped frames.
	 *			decodedNum:        Number of decoded frames.
	 *			renderNum:         Number of rendered frames.
	 *			sourceTimeNum:     Number of instances that the source exceeds the time (I / ms).
	 *			codecTimeNum:      Number of instances that the codec exceeds the time  (I / ms).
	 *			renderTimeNum:     Number of instances that the render exceeds the time (I / ms).
	 *			jitterNum:         Number of instances that the jitter exceeds the time (I / ms).
	 *			codecErrorsNum:    Number of codec frames dropped after encountering errors.
	 *			codecErrors:       Codec errors kept as a string in JSON format.
	 *			CPULoad:           Current CPU load (percent).
	 *			frequency:         Current CPU frequency.
	 *			maxFrequency:      Maximum CPU frequency.
	 *			worstDecodeTime:   Worst codec decode time (ms).
	 *			worstRenderTime:   Worst render time (ms).
	 *			averageDecodeTime: Average codec decode time (ms).
	 *			averageRenderTime: Average render time (ms).
	 *			totalCPULoad:      Current total CPU load (percent).
     *			playbackDuration:  Get current playback duration since playback begun.(ms)
     *			totalSourceDropNum:Get current count of frames dropped by source module since playback begun.
     *			totalCodecDropNum: Get current count of frame dropped by codec since playback begun.
     *			totalRenderDropNum:Get current count of frames dropped by renderer since playback begun.
     *			totalDecodedNum:   Get current count of decoded frames since playback begun.
     *			totalRenderedNum:  Get current count of rendered frames since playback begun.
	 */
  //* VOOSMPAnalyticsInfo getAnalytics(VOOSMPAnalyticsFilter filter);
	VOOSMPAnalyticsInfo.prototype.getAnalytics = function(filter)
	{
		var fa = "";
		fa = this.player.getAnalytics(filter);
		if (fa.length == 0)
			return "";

		var a = eval ("(" + fa + ")");
		return a;
	};
};
//* }


/********************************************************************************************************************************/

/* Function for multiple inheritance */
//* public void voPrototypeExtend();
function voPrototypeExtend() {

	for(var i=0; i<arguments.length; i++){
		var base = new arguments[i]();
		for(var j in base){
			this.prototype[j] = base[j];
		}
	}
};

//* public interface VOCommonPlayer extends VOCommonPlayerControl, VOCommonPlayerConfiguration, VOCommonPlayerHTTPConfiguration, VOCommonPlayerRTSPConfiguration, VOCommonPlayerSubtitle, VOCommonPlayerAssetSelection {
function VOCommonPlayer(pluginobject){
	VOCommonPlayerControl.call(this,pluginobject);
	VOCommonPlayerConfiguration.call(this,pluginobject);
	VOCommonPlayerHTTPConfiguration.call(this,pluginobject);
	VOCommonPlayerRTSPConfiguration.call(this,pluginobject);
	VOCommonPlayerSubtitle.call(this,pluginobject);
	VOCommonPlayerAssetSelection.call(this,pluginobject);
	VOOSMPAnalyticsInfo.call(this, pluginobject);
};

voPrototypeExtend.apply(VOCommonPlayer,[VOCommonPlayerControl,VOCommonPlayerConfiguration,VOCommonPlayerHTTPConfiguration,VOCommonPlayerRTSPConfiguration,VOCommonPlayerSubtitle,VOCommonPlayerAssetSelection, VOOSMPAnalyticsInfo]);
//* }
