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
 * @file voOSStreamInfoImpl.java
 * implementation for voOSStreamInfo interface.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

import android.os.Parcel;

public class voOSStreamInfoImpl implements voOSStreamInfo {
	

	/**
	 * 
	 */
	public voOSStreamInfoImpl() {
		super();
	}
	/**
	 * @param streamID
	 * @param selInfo
	 * @param bitrate
	 * @param trackCount
	 * @param trackInfo
	 */
	public voOSStreamInfoImpl(int streamID, int selInfo, int bitrate,
			int trackCount, voOSTrackInfo[] trackInfo) {
		super();
		StreamID = streamID;
		SelInfo = selInfo;
		Bitrate = bitrate;
		TrackCount = trackCount;
		TrackInfo = trackInfo;
	}
	/**
	 * @param parc  the Parcel object to convert data into this object
	 * @return true if parse OK,
	 */
	public boolean parse(Parcel parc)
	{
		StreamID = parc.readInt();
		SelInfo = parc.readInt();
		Bitrate = parc.readInt();
		TrackCount = parc.readInt();
		if(TrackCount>0)
		{
			TrackInfo = new voOSTrackInfo[TrackCount];
			for(int i = 0;i<TrackCount;i++)
			{
				TrackInfo[i] = new voOSTrackInfoImpl();
				TrackInfo[i].parse(parc);
			}
		}
		return true;
	}
	
	int							StreamID;		/*!< the sub stream id created by our parser, it is unique in this source session */
	int							SelInfo;		/*!< Indicated if the Stream is selected and recommend or default */
	int							Bitrate;		/*!< the bitrate of the stream */
	int							TrackCount;		/*!< track count if this sub stream */
	private voOSTrackInfo[]		TrackInfo;		/*!< Track info */
	/**
	 * @return the streamID
	 */
	public int getStreamID() {
		return StreamID;
	}
	/**
	 * @return the selInfo
	 */
	public int getSelInfo() {
		return SelInfo;
	}
	/**
	 * @return the bitrate
	 */
	public int getBitrate() {
		return Bitrate;
	}
	/**
	 * @return the trackCount
	 */
	public int getTrackCount() {
		return TrackCount;
	}
	/**
	 * @return the trackInfo
	 */
	public voOSTrackInfo[] getTrackInfo() {
		return TrackInfo;
	}
}
