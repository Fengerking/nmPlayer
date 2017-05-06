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
 * @file voOSProgramInfoImpl.java
 * implementation for voOSProgramInfo interface.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

import java.io.UnsupportedEncodingException;

import com.visualon.OSMPUtils.voOSType.VOOSMP_SRC_PROGRAM_TYPE;

import android.os.Parcel;

public class voOSProgramInfoImpl implements voOSProgramInfo {


	
	/**
	 * 
	 */
	public voOSProgramInfoImpl() {
		super();
	}
	/**
	 * @param programID
	 * @param selInfo
	 * @param programName
	 * @param programType
	 * @param streamCount
	 * @param streamInfo
	 */
	public voOSProgramInfoImpl(int programID, int selInfo, String programName,
			VOOSMP_SRC_PROGRAM_TYPE programType, int streamCount,
			voOSStreamInfo[] streamInfo) {
		super();
		ProgramID = programID;
		SelInfo = selInfo;
		ProgramName = programName;
		ProgramType = programType;
		StreamCount = streamCount;
		StreamInfo = streamInfo;
	}
	/**
	 * @param parc  the Parcel object to convert data into this object
	 * @return true if parse OK,
	 */
	public boolean parse(Parcel parc)
	{
		ProgramID = parc.readInt();
		SelInfo = parc.readInt();
    	byte[] dt = new byte[256];
    	parc.readByteArray(dt);
    	try {
    		ProgramName = (new String(dt,0,256,"utf-8"));
    		ProgramName = ProgramName.trim();
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
//		voLog.i(this.getClass().getSimpleName(), "SubtitleInfo Program name is %s ", ProgramName);
		int nType = parc.readInt();
		if(nType<0)
			nType = 0;
		if(nType>1)
			nType = 0;
		ProgramType = voOSType.VOOSMP_SRC_PROGRAM_TYPE.valueOf(nType);
		StreamCount = parc.readInt();
		if(StreamCount>0)
			StreamInfo = new voOSStreamInfo[StreamCount];
		for(int i = 0;i<StreamCount;i++)
		{
			StreamInfo[i] = new voOSStreamInfoImpl();
			StreamInfo[i].parse(parc);
		}
		return true;
	}
	
	int									ProgramID;				/*!< the stream id created by our parser, it is unique in this source session */
	int									SelInfo;				/*!< Indicated if the Stream is selected and recommend or default */
	String								ProgramName;		/*!< Name of the program */
	voOSType.VOOSMP_SRC_PROGRAM_TYPE	ProgramType;			/*!< Indicate if the Program is live or vod */
	int									StreamCount;			/*!< Stream Count */
	voOSStreamInfo[]					StreamInfo;			/*!< Stream info */
	
	/**
	 * @return the programID
	 */
	public int getProgramID() {
		return ProgramID;
	}
	/**
	 * @return the selInfo
	 */
	public int getSelInfo() {
		return SelInfo;
	}
	/**
	 * @return the programName
	 */
	public String getProgramName() {
		return ProgramName;
	}
	/**
	 * @return the programType
	 */
	public voOSType.VOOSMP_SRC_PROGRAM_TYPE getProgramType() {
		return ProgramType;
	}
	/**
	 * @return the streamCount
	 */
	public int getStreamCount() {
		return StreamCount;
	}
	/**
	 * @return the streamInfo
	 */
	public voOSStreamInfo[] getStreamInfo() {
		return StreamInfo;
	}
}
