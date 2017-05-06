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

import android.graphics.Rect;

public interface VOCommonPlayerConfiguration {    
   
//
// Set license information
//

	/**
	 * Set contents of license file
	 *
	 * @param   data    [in] Contents of the license file in a Byte array.
	 *
	 * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
	 *
	 * <pre>This API must be called before running the player calling the {@link VOCommonPlayerControl#start} method.</pre>
	 */
	VO_OSMP_RETURN_CODE setLicenseContent(byte[] data);
   

	/**
	 * Set the location of the license file
	 *
	 * @param   path    [in] location of the license file.
	 *
	 * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
	 *
	 * <pre>This API must be called before running the player calling the {@link VOCommonPlayerControl#start} method.</pre>
	 */
	VO_OSMP_RETURN_CODE setLicenseFilePath(String path);
   

	/**
	 * Set pre-agreed license string
	 *
	 * @param   str  [in] Pre-agreed license string.
	 *
	 * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
	 *
	 * <pre>This API must be called before running the player calling the {@link VOCommonPlayerControl#start} method.</pre>
	 */
	VO_OSMP_RETURN_CODE setPreAgreedLicense(String str);
	
	
    /**
	 * Set the draw color type
	 *
	 * @param   type    [in] Color type. Please refer to {@link VO_OSMP_COLORTYPE}.
	 *
	 * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
	 *
	 * <pre>This API must be called before running the player calling the {@link VOCommonPlayerControl#start} method.</pre>
	 */
 	VO_OSMP_RETURN_CODE setColorType(VO_OSMP_COLORTYPE type);
    

    /**
     * Set video render type
     *
     * @param   type    [in] Render type. Refer to {@link VO_OSMP_RENDER_TYPE}.
     *
     * Default value is {@link VO_OSMP_RENDER_TYPE#VO_OSMP_RENDER_TYPE_NATIVE_WINDOW} for Android 2.3 or above;
     * {@link VO_OSMP_RENDER_TYPE#VO_OSMP_RENDER_TYPE_NATIVE_SURFACE} for older.
     *
     * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     *
     * <pre>This API must be called before running the player calling the {@link VOCommonPlayerControl#start} method.</pre>
     * 
     */
 	VO_OSMP_RETURN_CODE setRenderType(VO_OSMP_RENDER_TYPE type);


	/**
	 * Enable/Disable deblock. This is valid for H.264 and Real video. The default is enabled.
	 *
	 * @param  value    [in] Enable/Disable; true to enable deblock (default), false to disable deblock.
	 *
	 * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
	 *
	 */
	VO_OSMP_RETURN_CODE enableDeblock(boolean value);


	/**
	 * Set the location of device capability file. The device capability file contains device-specific display sizes and bit rates to optimize performance. An example cap.xml file is included with the SamplePlayer project.
	 *
	 * @param   filename    [in] Location of device capability file.
	 *
	 * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
	 *
	 * <pre>This API should be called before opening data source calling the {@link VOCommonPlayerControl#open} method. Returns error code if failed.</pre>  
	 *
	 */
	VO_OSMP_RETURN_CODE setDeviceCapabilityByFile(String filename);


	/**
	 * Set the initial bitrate.
	 * The player will select the closest stream at or below that number. If none is available, 
	 * it will pick the closest one above.
	 * If an initial bitrate is not set, the player will follow the streaming protocol specification. 
	 * If the behavior is undefined in the protocol being used, the player will pick a stream based on cap.xml, 
	 * the order in the manifest, and other heuristics.
	 * 
	 * @param bitrate [in] Bitrate (bps).
	 * @return {@link VO_OSMP_ERR_NONE} if successful.
	 * 
	 * <pre> This API should be called before calling the {@link VOCommonPlayerControl#open(String, VO_OSMP_SRC_FLAG, VO_OSMP_SRC_FORMAT, VOOSMPOpenParam)} method. </pre>
	 * 
	 */
	VO_OSMP_RETURN_CODE setInitialBitrate(int bitrate);
   
   
	/**
	 * Set video display area
	 * Override automatic screen width and height adjustment according to zoom mode.
	 *
	 * @param mode  [in] Zoom mode. Refer to {@link VO_OSMP_ZOOM_MODE}.
	 * @param rect  [in] Rectangular area to be displayed.<br>
	 *                   Only when Zoom mode is {@link VO_OSMP_ZOOM_MODE#VO_OSMP_ZOOM_ZOOMIN}, the value is needful. 
	 *
	 * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
	 *
	 *
	 */
	VO_OSMP_RETURN_CODE setZoomMode(VO_OSMP_ZOOM_MODE mode, Rect rect);
   
   
	/**
	 * Enable defining position based on live streaming DVR window. The default is enabled.
	 * <p>
	 * When enabled: 
	 * <p><ul>
	 * <li> {@link VOCommonPlayerControl#getPosition()} returns 0 when playing at the live head. A negative position represents
	 *      the offset into the DVR window with respect to live head
	 * <li> {@link VOCommonPlayerConfiguration#getMaxPosition()} always returns 0 which represent the live head
	 * <li> {@link VOCommonPlayerConfiguration#getMinPosition()} returns (getMaxPosition - DVR window size)
	 * </ul><p>
	 *
	 * When disabled: 
	 * <p><ul>
	 * <li> {@link VOCommonPlayerControl#getPosition()} returns the offset with respect to the beginning of the playback
	 * <li> {@link VOCommonPlayerConfiguration#getMaxPosition()} returns the offset between the live head and the beginning of the playback
	 * <li> {@link VOCommonPlayerConfiguration#getMinPosition()} returns (getMaxPosition - DVR window size)
	 * <ul><p>
	 * <p>
	 * 
	 * @param   value  [in] Enable/Disable; true to enable(default), false to disable.
	 *
	 * @return  {@link VO_OSMP_ERR_NONE} if successful
	 */
	VO_OSMP_RETURN_CODE enableLiveStreamingDVRPosition(boolean enable);


	/**
	 * Set DRM Library name and API name
	 *
	 * @param libName [in] DRM library name.
	 * @param libApiName [in] DRM library export api name.
	 *
	 * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful .
	 *
	 * <pre>This API must be called after init the player and before opening data source calling the {@link VOCommonPlayerControl#open} method.</pre>
	 */
	VO_OSMP_RETURN_CODE setDRMLibrary(String libName, String libApiName);

	/**
	 * Set the DRM adapter
	 *
	 * @param   adapter  [in] DRM adapter
	 *
	 * @return  {@link VO_OSMP_ERR_NONE} if successful.
	 */
	VO_OSMP_RETURN_CODE setDRMAdapter(Object adapter, boolean isLibraryName);


	/**
	 * Set the verification information to start DRM verification
	 * 
	 * @param info
	 *            [in] Verification information. {@link VOOSMPVerificationInfo}
	 * 
	 * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
	 */
	VO_OSMP_RETURN_CODE setDRMVerificationInfo(VOOSMPVerificationInfo info);
	
	/**
     * Set DRM init data
     * 
     * @param   initData [in] init data
     * 
     * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */ 
	VO_OSMP_RETURN_CODE setDRMInitData(VOOSMPDRMInit initData);


	/**
	  * Set unique indentifier of the device
	  *
	  * @param   string [in] Unique identifier string
	  *
	  * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
	  * 
	  * @deprecated This method is replaced by {@link com.visualon.OSMPPlayer.VOCommonPlayerConfiguration#getDRMUniqueIdentifier} and will be removed in the future.
	  */
	VO_OSMP_RETURN_CODE setDRMUniqueIndentifier(String string);

	/**
	  * Set unique indentifier of the device
	  *
	  * @param   string [in] Unique identifier string
	  *
	  * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
	  */
	VO_OSMP_RETURN_CODE setDRMUniqueIdentifier(String string);
 
	/**
	  * Set the initial buffering time before playback starts
	  *
	  * @param   time [in] buffer time (milliseconds) 
	  *
	  * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
	  */
	VO_OSMP_RETURN_CODE setInitialBufferTime(int time);
 
	/**
	  * Set the max buffering time for playback
	  *
	  * @param time
	  *            [in] buffer time (milliseconds)
	  *
	  * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
	  */
	VO_OSMP_RETURN_CODE setMaxBufferTime(int time);
   

	/**
	 * Set the buffering time when re-buffering is needed during playback
	 *
	 * @param   time [in] buffer time (milliseconds)
	 *
	 * @return  {@link VO_OSMP_ERR_NONE} if successful.
	 */
	VO_OSMP_RETURN_CODE setPlaybackBufferingTime(int time);


	/**
	 * Set the buffering time for continue playback when need to buffer again.
	 *
	 * @param   time [in] buffer time (milliseconds)
	 *
	 * @return  {@link VO_OSMP_ERR_NONE} if successful.
	 *
	 * @deprecated This method is replaced by {@link VOCommonPlayerConfiguration#setPlaybackBufferingTime:} and will be removed in the future.
	 */
	VO_OSMP_RETURN_CODE setAnewBufferingTime(int time);



	/**
	  * Enable/Disable playback of the audio stream. The default is enabled.
	  *
	  * @param   value  [in] Enable/Disable; true to enable, false to disable.
	  *
	  * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful
	  *
	  */
	VO_OSMP_RETURN_CODE enableAudioStream(boolean value);


	/**
	  * Enable/Disable playback of the video stream. The default is enabled.
	  *
	  * @param   value  [in] Enable/Disable; true to enable, false to disable.
	  *
	  * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful
	  *
	  */
	VO_OSMP_RETURN_CODE enableVideoStream(boolean value);


	/**
	  * Enable/Disable immediate video rendering with low latency.
	  * This function is only applicable to video-only streams. The default is disabled.
	  *
	  * @param   value  [in] Enable/Disable; true to enable, false to disable.
	  *
	  * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful
	  *
	  */
	VO_OSMP_RETURN_CODE enableLowLatencyVideo(boolean value);


	/**
	  * Enable/Disable audio effect. The default is disabled.
	  *
	  * @param   value  [in] Enable/Disable; true to enable, false to disable.
	  *
	  * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful
	  *
	  */
	VO_OSMP_RETURN_CODE enableAudioEffect(boolean value);
 
	/**
	  * Enable/Disable PCM data output. The default is disabled.
	  *
	  * @param  value    [in] Enable/Disable; true to enable PCM data output, false to disable (default).
	  *
	  * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
	  *
	  */
	VO_OSMP_RETURN_CODE enablePCMOutput(boolean value);


	/**
	  * Set audio playback speed
	  *
	  * @param   speed  [in] speed multiplier with respect to realtime playback. Default is 1.0
	  *
	  * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful
	  *
	  */
	VO_OSMP_RETURN_CODE setAudioPlaybackSpeed(float speed);




	/**
	  * Set the presentation delay time for a live streaming source
	  *
	  * @param   time [in] presentation delay time (milliseconds) for a live streaming source with respective to current time
	  *
	  * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
	  */
	VO_OSMP_RETURN_CODE setPresentationDelay(int time);
 

	/**
	  * Enable/Disable processing SEI information
	  *
	  * @param   flag [in] the flag {@link VO_OSMP_SEI_INFO_FLAG}. Set to {@link VO_OSMP_SEI_INFO_FLAG#VO_OSMP_SEI_INFO_NONE} to disable processing SEI information or any other flags to enable
	  *
	  * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
	  */
	VO_OSMP_RETURN_CODE enableSEI(VO_OSMP_SEI_INFO_FLAG flag);

	/**
	 * Set the number of retry attempts in progressive download module when network connection fails.
	 *
	 * @param   times [in] the number of retry attempts. Default is 2. Setting this value to -1 to continue retrying until {@link VOCommonPlayerControl#stop} be called.
	 *
	 * @return  {@link VO_OSMP_ERR_NONE} if successful
	 */
	VO_OSMP_RETURN_CODE setPDConnectionRetryCount(int times);
	
	/**
	* Enable/disable CPU adaptation; default is enabled.
	*
	* @param value [in] Enable/disable CPU adaptation; true to enable, false to disable.
	*
	* @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful
	*
	*/
    VO_OSMP_RETURN_CODE enableCPUAdaptation(boolean value);
    
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
     * <pre> This API should be called before calling the {@link VOCommonPlayerControl#open(String, VO_OSMP_SRC_FLAG, VO_OSMP_SRC_FORMAT, VOOSMPOpenParam)} method. </pre>
     *
     */
    VO_OSMP_RETURN_CODE setBitrateThreshold(int upper, int lower);
    
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
    VO_OSMP_RETURN_CODE setSegmentDownloadRetryCount(int count);
   
    /**
     * Enable/Disable loading Dolby libraries. The default is enable.
     *
     * @param   value  [in] Enable/Disable; true to enable, false to disable.
     *
     * @return  {@link VO_OSMP_ERR_NONE} if successful
     *
     */
    VO_OSMP_RETURN_CODE enableDolbyLibrary(boolean value);
    
    /**
     * Set the value for a specified ID. This method should only be used on recommendation from VisualOn.
     *
     * @param id    [in] Parameter id.
     * @param obj    [in] Parameter value; depends on parameter id.
     * @return      {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE setParameter(int id, Object obj);
    

}
