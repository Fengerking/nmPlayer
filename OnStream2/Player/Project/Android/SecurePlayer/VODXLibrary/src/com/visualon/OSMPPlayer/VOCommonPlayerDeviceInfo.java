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


import com.visualon.OSMPUtils.voLog;

public interface VOCommonPlayerDeviceInfo {

/**
* Enumeration device OS types.
*/

    public enum VO_OSMP_DEVICE_INFO_OS_TYPE
    {
        /** iOS Platform */
        VO_OSMP_DEVICE_INFO_OS_IOS       (0x00000000),
       
        /** Mac OS X */
        VO_OSMP_DEVICE_INFO_OS_OSX       (0x00000001),
       
        /** Android Platform */
        VO_OSMP_DEVICE_INFO_OS_ANDROID   (0x00000002),
       
        /** Max value definition */
        VO_OSMP_DEVICE_INFO_OS_MAX       (0XFFFFFFFF);
       
        private int value;
       
        VO_OSMP_DEVICE_INFO_OS_TYPE(int value)
        {
            this.value = value;
        }
       
        public int getValue()
        {
            return value;
        }

        public static VO_OSMP_DEVICE_INFO_OS_TYPE valueOf(int value) {
            for (int i = 0; i < VO_OSMP_DEVICE_INFO_OS_TYPE.values().length; i++) {
                if (VO_OSMP_DEVICE_INFO_OS_TYPE.values()[i].getValue() == value)
                    return VO_OSMP_DEVICE_INFO_OS_TYPE.values()[i];
            }
            voLog.e("VOCommonPlayerDeviceInfo",
                    "VO_OSMP_DEVICE_INFO_OS_TYPE isn't match. id = "
                            + Integer.toHexString(value));

            return VO_OSMP_DEVICE_INFO_OS_MAX;
        }
    }
       

    /**
     * Get the number of CPU cores
     *
     * @return  Number of CPU cores; -1 if get failed
     */
    int getNumberOfCores();


    /**
     * Check whether neon is available
     *
     * @return  true if neon is available; false if not available
     */
    boolean hasNeon();


    /**
     * Get max frequency of the CPU 
     *
     * @return  Max frequency of the CPU; -1 if get failed
     */
    long getMaxCPUFrequency();


    /**
     * Get the OS Type
     *
     * @return  OS type {@link VO_OSMP_DEVICE_INFO_OS_TYPE}.
     */
    VO_OSMP_DEVICE_INFO_OS_TYPE getOSType();


    /**
     * Get the OS version (the system version).

    * e.g. iOS: "4.0"
 *      OS X: "10.7.5"
 *
     * @return  OS version; null if get failed
     */
    String getOSVersion();


    /**
     * Get the model of device
     * e.g. "iPhone2,1", "iPhone4,1", "iPod2,1", "iPad1,1", "iPad2,1"...
 *      "MacBookPro8,2"...
*
     * @return  Device model null if get failed
     */
    String getDeviceModel();
}
