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
 * @file voOSInitParam.java
 * Enumeration for init param, used in Open() API
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

/**
 * Enumeration for init param, used in Open() API
 */
public class voOSInitParam {
	
	/**Indicate init PARAM is load library operation API when {@link VOOSMP_FLAG_INIT_LIBOP} is available */
	int       	LibOP;         
	
	/**Indicate init PARAM is IO module file name when {@link VOOSMP_FLAG_INIT_IO_FILE_NAME} is available */
	String		IOFileName;
	
	/**Indicate init PARAM is IO API name when {@link VOOSMP_FLAG_INIT_IO_API_NAME} is available*/
	String 		IOApiName;
	
	/**Indicate init PARAM is actual file size when {@link VOOSMP_FLAG_INIT_IO_API_NAME} is available*/
	long  	FileSize;

	/**
	 * @return the libOP
	 */
	public int getLibOP() {
		return LibOP;
	}

	/**
	 * @return the iOFileName
	 */
	public String getIOFileName() {
		return IOFileName;
	}

	/**
	 * @return the iOApiName
	 */
	public String getIOApiName() {
		return IOApiName;
	}

	/**
	 * @return the fileSize
	 */
	public long getFileSize() {
		return FileSize;
	}

	/**
	 * @param libOP the libOP to set
	 */
	public void setLibOP(int libOP) {
		LibOP = libOP;
	}

	/**
	 * @param iOFileName the iOFileName to set
	 */
	public void setIOFileName(String iOFileName) {
		IOFileName = iOFileName;
	}

	/**
	 * @param iOApiName the iOApiName to set
	 */
	public void setIOApiName(String iOApiName) {
		IOApiName = iOApiName;
	}

	/**
	 * @param fileSize the fileSize to set
	 */
	public void setFileSize(long fileSize) {
		FileSize = fileSize;
	}

	/**
	 * 
	 */
	public voOSInitParam() {
		super();
		
		LibOP = 0;;         
		
		IOFileName = null;
	
		IOApiName = null;

		FileSize = 0;
	}  
	
	
	
	
}
