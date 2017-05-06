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

import com.visualon.OSMPPlayer.VOCommonPlayerListener.VO_OSMP_CB_SYNC_EVENT_ID;
import com.visualon.OSMPPlayer.VOCommonPlayerListener.VO_OSMP_SRC_RTSP_ERROR;
import com.visualon.OSMPUtils.voLog;

public interface VOOSMPHTTPDownloadFailure {
    
    public enum VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON
    {
        /** HTTP connect failed */
        VO_OSMP_HTTP_CONNECT_FAILED               (0x00000000),

        /** HTTP failed to get response or response cannot be parsed or too large */
        VO_OSMP_HTTP_INVALID_RESPONSE            (0x00000001),

        /** HTTP 4xx error */
        VO_OSMP_HTTP_CLIENT_ERROR                (0x00000002),

        /** HTTP 5xx error */
        VO_OSMP_HTTP_SERVER_ERROR                (0x00000003),
   
        /** Max value */
        VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON_MAX  (0xFFFFFFFF);
        
        private int value;
        VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON (int event)
        {
            value = event;
        }
       
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON valueOf(int value)
        {
            for (int i = 0; i < VO_OSMP_CB_SYNC_EVENT_ID.values().length; i ++)
            {
                if (VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON.values()[i].getValue() == value)
                    return VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON.values()[i];
            }
            voLog.e("VOCommonPlayerListener", "VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON_MAX isn't match. id = " + Integer.toHexString(value));
            return VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON_MAX;
        }
    }

    /**
     * Get URL of the HTTP download failure
     *
     * @return  URL
     **/
    String getURL();

    /**
     * Get reason of the HTTP download failure
     *
     * @return  reason of HTTP download failure
     **/
    VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON getReason();

    /**
     * Get response of the HTTP download failure when the download failure reason is VO_OSMP_HTTP_CLIENT_ERROR or VO_OSMP_HTTP_SERVER_ERROR. It is null for other failure reasons
     *
     * @return  response of HTTP download failure
     **/
    String getResponse();

}
