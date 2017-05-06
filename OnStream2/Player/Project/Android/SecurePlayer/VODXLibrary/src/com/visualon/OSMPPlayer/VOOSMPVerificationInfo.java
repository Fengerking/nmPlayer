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

public class VOOSMPVerificationInfo {
   
    private String  VerificationString;       /*!<[in] The data that need to do verification*/
    private int     DataFlag;               /*!<[in] The data flag, if it is 0, it is the request string, 1 is password,other value to not support currently */
    private String  ResponseString;           /*!<[out] Output the authentication response string*/
   
    /**
     * The Get the verification string
     * @return the VerificationString
     */
    public String getVerificationString() {
        return VerificationString;
    }
   
    /**
     * Set the verification string
     * @param str the VerificationData verification string to set
     */
    public VO_OSMP_RETURN_CODE setVerificationString(String str) {
        VerificationString = str;
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }
   
    /**
     * Get the data flag. If the flag is 0, the data is the request string; if it is 1, the data is the password; other values are not currently supported.
     *
     * @return the dataFlag
     */
    public int getDataFlag() {
        return DataFlag;
    }
    
    /**
     * Set the data flag. If the flag is 0, the data is the request string; if it is 1, the data is the password; other values are not currently supported. 
     *
     * @param dataFlag the dataFlag to set
     */
    public VO_OSMP_RETURN_CODE setDataFlag(int dataFlag) {
        DataFlag = dataFlag;
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }
   
    /**
     * Get the authentication response string
     * @return the szResponse
     */
    public  String getResponseString() {
        return ResponseString;
    }
    
    /**
     * Set the authentication response string
     * @param response [in] the authentication response string
     */
    public VO_OSMP_RETURN_CODE setResponseString( String response) {
        ResponseString = response;
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }
   
   
    /**
     * @param data verification data
     * @param dataFlag data flag
     * @param response authentication response string
     */
    public VOOSMPVerificationInfo(String data, int dataFlag, String response) {
        super();
        VerificationString = data;
        DataFlag = dataFlag;
        ResponseString = response;
    }
    
    /**
     *
     */
    public VOOSMPVerificationInfo() {
        super();

        VerificationString = null;
        DataFlag = 0;
        ResponseString = null;

    }



}
