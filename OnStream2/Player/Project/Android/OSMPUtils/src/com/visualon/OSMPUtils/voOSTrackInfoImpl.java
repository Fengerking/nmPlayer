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
 * @file voOSTrackInfoImpl.java
 * implementation for voOSTrackInfo interface.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

import java.io.UnsupportedEncodingException;

import android.os.Parcel;

import com.visualon.OSMPUtils.voOSType.VOOSMP_SOURCE_STREAMTYPE;

public class voOSTrackInfoImpl implements voOSTrackInfo {

	/**
	 * @param trackID
	 * @param selectInfo
	 * @param fourCC
	 * @param trackType
	 * @param codec
	 * @param duration
	 * @param audioInfo
	 * @param videoInfo
	 * @param chunkCounts
	 * @param bitrate
	 * @param headSize
	 * @param headData
	 */
	public voOSTrackInfoImpl(int trackID, int selectInfo, char[] fourCC,
			VOOSMP_SOURCE_STREAMTYPE trackType, int codec, long duration,
			voOSAudioInfo audioInfo, voOSVideoInfo videoInfo, int chunkCounts,
			int bitrate, int headSize, char[] headData) {
		super();
		TrackID = trackID;
		SelectInfo = selectInfo;
		FourCC = fourCC;
		TrackType = trackType;
		Codec = codec;
		Duration = duration;
		AudioInfo = audioInfo;
		VideoInfo = videoInfo;
		ChunkCounts = chunkCounts;
		Bitrate = bitrate;
		HeadSize = headSize;
		HeadData = headData;
	}
	/**
	 * 
	 */
	public voOSTrackInfoImpl() {
		super();
	}
	
	/**
	 * @param parc  the Parcel object to convert data into this object
	 * @return true if parse OK,
	 */
	public boolean parse(Parcel parc)
	{
    	byte[] dt = new byte[8];
		TrackID = parc.readInt();
		SelectInfo = parc.readInt();
    	try {
        	parc.readByteArray(dt);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    	try {
    		FourCC = (new String(dt,0,8,"utf-8")).toCharArray();
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		int n = parc.readInt();
//		TrackType = voOSType.VOOSMP_SOURCE_STREAMTYPE.values()[n];
		TrackType = voOSType.VOOSMP_SOURCE_STREAMTYPE.valueOf(n);
		Codec = parc.readInt();
		Duration = parc.readInt();
		ChunkCounts = parc.readInt();
		Bitrate = parc.readInt();
		
		if((TrackType.compareTo( voOSType.VOOSMP_SOURCE_STREAMTYPE.VOOSMP_SS_AUDIO)==0) ||
			(TrackType.compareTo( voOSType.VOOSMP_SOURCE_STREAMTYPE.VOOSMP_SS_AUDIO_GROUP)==0) )
		{
			AudioInfo = new voOSAudioInfoImpl();
			AudioInfo.parse(parc);
		}else if((TrackType.compareTo( voOSType.VOOSMP_SOURCE_STREAMTYPE.VOOSMP_SS_VIDEO)==0) || 
			(TrackType.compareTo( voOSType.VOOSMP_SOURCE_STREAMTYPE.VOOSMP_SS_VIDEO_GROUP)==0 ))
		{
			VideoInfo = new voOSVideoInfoImpl();
			VideoInfo.parse(parc);
		}else if((TrackType.compareTo( voOSType.VOOSMP_SOURCE_STREAMTYPE.VOOSMP_SS_SUBTITLE)==0 ) ||
		  	(TrackType.compareTo( voOSType.VOOSMP_SOURCE_STREAMTYPE.VOOSMP_SS_SUBTITLE_GROUP)==0 ))
		{
			voOSSubtitleInfoImpl impl = new voOSSubtitleInfoImpl();
			impl.parse(parc);
			SubtitleInfo = impl;
		}
		
		HeadSize = parc.readInt();
		if(HeadSize>0)
		{
			dt = new byte[HeadSize];
	    	try {
		    	parc.readByteArray(dt);
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
	    	try {
	    		HeadData = (new String(dt,0,HeadSize,"utf-8")).toCharArray();
			} catch (UnsupportedEncodingException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		return true;
	}
	
	
	private int					TrackID;		/*!< the track id created by our parser, it is unique in this source session */
	private int                 SelectInfo;	/*!< Indicated if the track is selected and recommend or default,refer to VOOSMP_SRC_TRACK_SELECT */
	private char[]				FourCC;
	private voOSType.VOOSMP_SOURCE_STREAMTYPE	TrackType;	/*!< video/audio/subtitle */
	
	private int					Codec;			/*!< codec type,refer to VOOSMP_AUDIO_CODINGTYPE and VOOSMP_VIDEO_CODINGTYPE  */	
	private long 				Duration;		/*!< duration of this track */		
	
	private int					ChunkCounts;	/*!< chunks in the track */
	private int					Bitrate;		/*!< bitrate of this track */
	
	
	private voOSAudioInfo		AudioInfo;		/*!< audio info */
	private voOSVideoInfo		VideoInfo;		/*!< video info */
	private voOSSubtitleInfo    SubtitleInfo;   /*!< subtitle info */
	
	private int					HeadSize;		/*!< sequence head data size*/
	private char[]				HeadData;		/*!< sequence head data */
	/**
	 * @return the trackID
	 */
	public int getTrackID() {
		return TrackID;
	}
	/**
	 * @return the fourCC
	 */
	public char[] getFourCC() {
		return FourCC;
	}
	/**
	 * @return the trackType
	 */
	public voOSType.VOOSMP_SOURCE_STREAMTYPE getTrackType() {
		return TrackType;
	}
	/**
	 * @return the codec
	 */
	public int getCodec() {
		return Codec;
	}
	/**
	 * @return the duration
	 */
	public long getDuration() {
		return Duration;
	}
	/**
	 * @return the audioInfo
	 */
	public voOSAudioInfo getAudioInfo() {
		return AudioInfo;
	}
	/**
	 * @return the videoInfo
	 */
	public voOSVideoInfo getVideoInfo() {
		return VideoInfo;
	}
	/**
	 * @return the headSize
	 */
	public int getHeadSize() {
		return HeadSize;
	}
	/**
	 * @return the headData
	 */
	public char[] getHeadData() {
		return HeadData;
	}
	/**
	 * @return the selectInfo
	 */
	public int getSelectInfo() {
		return SelectInfo;
	}
	/**
	 * @return the chunkCounts
	 */
	public int getChunkCounts() {
		return ChunkCounts;
	}
	/**
	 * @return the bitrate
	 */
	public int getBitrate() {
		return Bitrate;
	}
	@Override
	public voOSSubtitleInfo getSubtitleInfo() {
		// TODO Auto-generated method stub
		return SubtitleInfo;
	}

}
