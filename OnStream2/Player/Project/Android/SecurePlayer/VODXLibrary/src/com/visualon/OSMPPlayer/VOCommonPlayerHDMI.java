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

import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_SCREEN_BRIGHTNESS_MODE;
import com.visualon.OSMPUtils.voLog;

import android.content.Context;


/**
 *
 *
 *
 *
 */
public interface VOCommonPlayerHDMI {    
   
    /**
     * Enable/disable HDMI connection detection
     *
     * @param context [in] Context.
     * @param value   [in] Enable/disable; true to enable HDMI detection, false to disable.
     *
     * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
         // NOTE: can we disable this at run time?
    VO_OSMP_RETURN_CODE enableHDMIDetection(Context context, boolean value);
   
   
    /**
     * Check if HDMI connection detection is supported
     *
     * @return  true if HDMI connection detection is supported, false if not supported.
     */
    boolean isHDMIDetectionSupported();
   
    /**
     * Check if HDMI is connected
     *
     *
     * @return  HDMI connection state, refer to {@link VO_OSMP_HDMI_CONNECTION_STATUS}.
     */
    VO_OSMP_HDMI_CONNECTION_STATUS getHDMIStatus ();

    /**
     * Set a listener for get HDMI Connection change callback
     *
     *
     * @param listener [in] Listener, an onHDMIConnectionChangeListener instance.
     *
     * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE setOnHDMIConnectionChangeListener(onHDMIConnectionChangeListener listener);
   
   
    public interface onHDMIConnectionChangeListener {
        /**
         * HDMI state change event listener
         *
         * @param status [in] HDMI connection status, refer to
         * <ul>
         * <li> {@link VO_OSMP_HDMI_CONNECTION_STATUS}
         * </ul>
         *
         */
        public VO_OSMP_RETURN_CODE onHDMIStateChangeEvent(VO_OSMP_HDMI_CONNECTION_STATUS status);
    }

/**
 * Enumeration of HDMI connection statuses
 */

    public enum VO_OSMP_HDMI_CONNECTION_STATUS
    {
        /**
         * HDMI state change to disconnected
         */
        VO_OSMP_HDMISTATE_DISCONNECT                       (0x00000000),
       
        /**
         * HDMI state change to connected
         */
        VO_OSMP_HDMISTATE_CONNECT                          (0x00000001),
       
        /**
         * Couldn't detect HDMI connection state
         */
        VO_OSMP_HDMISTATE_UNKNOWN                          (0x00000002),
        
        VO_OSMP_HDMISTATE_MAX                              (0X7FFFFFFF);
        
        private int value;
        
        VO_OSMP_HDMI_CONNECTION_STATUS(int value)
        {
            this.value = value;
        }
       
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_HDMI_CONNECTION_STATUS valueOf(int value)
    	{
    		for (int i = 0; i < VO_OSMP_HDMI_CONNECTION_STATUS.values().length; i ++)
    		{
    			if (VO_OSMP_SCREEN_BRIGHTNESS_MODE.values()[i].getValue() == value)
    				return VO_OSMP_HDMI_CONNECTION_STATUS.values()[i];
    		}
    		
    		voLog.e("VOCommonPlayerHDMI", "VO_OSMP_HDMI_CONNECTION_STATUS isn't match. id = " + Integer.toHexString(value));
    		return VO_OSMP_HDMISTATE_MAX;
    	}

    }
}

