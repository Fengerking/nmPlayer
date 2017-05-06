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
 * @file voOSAudioFormat.java
 * implementation of Verification Information.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

public class voOSVerificationInfo {
	
	private int		UserData;				/*!<[in] The userdata recved in VOOSMP_SRC_HTTP_VERIFYCALLBACK*/
	private String	VerificationData;					/*!<[in] The data that need to do verification*/
	private int		DataSize;				/*!<[in] The data size*/
	private int		DataFlag;				/*!<[in] The data flag, if it is 0, it is the request string, 1 is password,other value to not support currently */
	private byte[] 	ResponseData;			/*!<[out] Output the authentication response string*/
	private int		ResponseDataSize;			/*!<[out] Output the response string size*/
	
	/**
	 * @return the userData
	 */
	public int getUserData() {
		return UserData;
	}
	/**
	 * 
	 * @param userData the userData to set
	 */
	public void setUserData(int userData) {
		UserData = userData;
	}
	
	/**
	 * The data that need to do verification
	 * @return the VerificationData
	 */
	public String getVerificationData() {
		return VerificationData;
	}
	
	/**
	 * The data that need to do verification
	 * @param str the VerificationData to set
	 */
	public void setVerificationData(String str) {
		VerificationData = str;
	}

	
	/**
	 * The data flag, if it is 0, it is the request string, 1 is password,other value to not support currently
	 * 
	 * @return the dataFlag
	 */
	public int getDataFlag() {
		return DataFlag;
	}
	/**
	 * The data flag, if it is 0, it is the request string, 1 is password,other value to not support currently
	 * 
	 * @param dataFlag the dataFlag to set
	 */
	public void setDataFlag(int dataFlag) {
		DataFlag = dataFlag;
	}
	
	/**
	 * [out] Output the authentication response string
	 * @return the szResponse
	 */
	public  byte[] getResponseData() {
		return ResponseData;
	}
	/**
	 * [out] Output the authentication response string
	 * @param szResponse the szResponse to set
	 */
	public void setResponseData( byte[] szResponse) {
		ResponseData = szResponse;
	}
	
	
	/**
	 * @param userData
	 * @param data
	 * @param dataFlag
	 * @param szResponse
	 */
	public voOSVerificationInfo(int userData, String data,
			int dataFlag,  byte[] szResponse) {
		super();
		UserData = userData;
		VerificationData = data;
		DataFlag = dataFlag;
		ResponseData = szResponse;
	}
	/**
	 * 
	 */
	public voOSVerificationInfo() {
		super(); 
		UserData = 0;
		VerificationData = null;
		DataSize = 0;
		DataFlag = 0;
		ResponseData = null;
		ResponseDataSize = 0;
	}


}
