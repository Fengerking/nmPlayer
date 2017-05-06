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

import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_MODULE_TYPE;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_PLAYER_ENGINE;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_SCREEN_BRIGHTNESS_MODE;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_SEI_INFO_FLAG;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_STATUS;

public interface VOCommonPlayerStatus {
	
	/**
	 * Get player engine type
	 *
	 * @return Player engine type, refer to {@link VO_OSMP_PLAYER_ENGINE}.
	 */
	VO_OSMP_PLAYER_ENGINE getPlayerType();

    /**
     * Get player status
     *
     * @return    Player status. Please refer to {@link VO_OSMP_STATUS}.
     */
    VO_OSMP_STATUS getPlayerStatus();
    
    /**
     * Check if the stream can be paused
     *
     * @return true for a VOD, false for a live stream with no DVR-like features enabled.
     */
    boolean canBePaused();
    
    /**
     * Check if an opened data source is live streaming
     *
     * @return true if the data source is opened, is a streaming source and is live streaming.
     */
    boolean isLiveStreaming();
    
    /**
	 * Get current playback position.
	 * 
	 * <p><ul>
	 * <li> For VOD. returns current playback position.
	 * <li> For Live streaming. Depend on {@link VOCommonPlayerConfiguration#enableLiveStreamingDVRPosition(boolean enable)}
	 *  <ol>
	 *  <li>If enable. returns 0 when playing at the live head. A negative position represents the offset into the DVR window with respect to live head
	 *  <li>If disable. returns the offset with respect to the beginning of the playback
	 * </ul><p>
	 * 
		 * @return  the position <ms>; -1 if unsuccessful
		 */
	long getPosition();
	

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
	long getMinPosition();
	   
	
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
	long getMaxPosition();
	
    /**
     * Get duration of the stream
     *
     * @return      Current stream duration (ms); 0 if source is a live stream.
     */
    long getDuration();
    
    /**
	 * Get screen display brightness mode
	 *
	 * @return Current display brightness mode. For Android version 2.1 or below, it returns VO_OSMP_SCREEN_BRIGHTNESS_MODE_MANUAL
	 */
	VO_OSMP_SCREEN_BRIGHTNESS_MODE getScreenBrightnessMode();    


    /**
	 * Get screen display brightness
	 *
	 * @return Current display brightness (percent)
	 */
	int getScreenBrightness();
	
	/**
	 * Get unique identifier of the device
	 *
	 * @return unique identifier is successful, null if failed
	 * 
	 * @deprecated This method is replaced by {@link com.visualon.OSMPPlayer.VOCommonPlayerConfiguration#getDRMUniqueIdentifier} and will be removed in the future.
	 */
	String getDRMUniqueIndentifier();

	/**
	 * Get unique identifier of the device
	 *
	 * @return unique identifier is successful, null if failed
	 */
	String getDRMUniqueIdentifier();
		
		
	/**
	 * Get version information of a module
	 * <p>
	 * The OSMP+ SDK is currently built using the same version for all modules.
	 * Instead of passing a module as an argument, simply pass VO_OSMP_MODULE_TYPE_SDK
	 * </p>
	 *
	 * @return version information of the given module
	 */
	String getVersion(VO_OSMP_MODULE_TYPE module);
	
	
    /**
	 * get the SEI info
	 *
	 * @param   time [in] the time stamp of SEI that want to get
	 * @param   flag [in] the type flag of SEI info {@link VO_OSMP_SEI_INFO_FLAG}
	 *
	 * @return  The object according to flag if successful.
	 */
	VOOSMPSEIPicTiming getSEIInfo(long time, VO_OSMP_SEI_INFO_FLAG flag);
	
	/**
	 * Get the value from specified ID. This method should only be used on recommendation from VisualOn.
	 *
	 * @param id    [in] Parameter id.
	 * @param obj   [out] Returned parameter value; depends on parameter id.
	 * @return      {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
	 */
	VO_OSMP_RETURN_CODE getParameter(int id, Object obj);
   

   

}
