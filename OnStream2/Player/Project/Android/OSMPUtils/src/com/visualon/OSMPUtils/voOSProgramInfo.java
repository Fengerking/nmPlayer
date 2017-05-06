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
 * @file voOSProgramInfot.java
 * interface for getting Program information.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

import android.os.Parcel;

public interface voOSProgramInfo {

	/**
	 * @param parc  the Parcel object to convert data into this object
	 * @return true if parse OK,
	 */
	public boolean parse(Parcel parc);
	
	/**
	 * the stream id created by our parser, it is unique in this source session
	 * 
	 * @return the programID
	 */
	public int getProgramID();
	
	/**
	 *  Indicated if the Stream is selected and recommend or default 
	 *  
	 * @return the selInfo
	 */
	public int getSelInfo();
	
	/**
	 * Name of the program
	 * 
	 * @return the programName
	 */
	public String getProgramName();
	
	/**
	 *  Indicate if the Program is live or vod 
	 *  
	 * @return the programType
	 */
	public voOSType.VOOSMP_SRC_PROGRAM_TYPE getProgramType();
	
	/**
	 * Stream Count 
	 * 
	 * @return the streamCount
	 */
	public int getStreamCount();
	
	/**
	 * Stream info
	 * 
	 * @return the streamInfo
	 */
	public voOSStreamInfo[] getStreamInfo();
}
