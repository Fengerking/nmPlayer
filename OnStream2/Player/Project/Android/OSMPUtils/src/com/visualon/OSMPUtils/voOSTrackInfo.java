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
 * @file voOSTrackInfo.java
 * interface for getting Track Info information.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;



import android.os.Parcel;

public interface voOSTrackInfo {

	/**
	 * @param parc  the Parcel object to convert data into this object
	 * @return true if parse OK,
	 */
	public boolean parse(Parcel parc);
	
	/**
	 *  the track id created by our parser, it is unique in this source session 
	 *  
	 * @return the trackID
	 */
	public int getTrackID();
	
	/**
	 * Indicated if the track is selected and recommend or default,refer to 
 	 * <ul>
 	 * <li> {@link voOSType#VOOSMP_SRC_TRACK_SELECT_SELECTABLE}
 	 * <li> {@link voOSType#VOOSMP_SRC_TRACK_SELECT_RECOMMEND}
 	 * <li> {@link voOSType#VOOSMP_SRC_TRACK_SELECT_SELECTED}
 	 * <li> {@link voOSType#VOOSMP_SRC_TRACK_SELECT_DISABLE}
 	 * <li> {@link voOSType#VOOSMP_SRC_TRACK_SELECT_DEFAULT}
 	 * <li> {@link voOSType#VOOSMP_SRC_TRACK_SELECT_FORCE}
	 * </ul>
	 * @return the selectInfo
	 */
	public int getSelectInfo();
	
	/**
	 * @return the fourCC
	 */
	public char[] getFourCC();
	
	/**
	 * video/audio/subtitle
	 * 
	 * @return the trackType
	 */
	public voOSType.VOOSMP_SOURCE_STREAMTYPE getTrackType();
	
	/**
	 * codec type,refer to {@link voOSType.VOOSMP_AUDIO_CODINGTYPE} and {@link voOSType.VOOSMP_VIDEO_CODINGTYPE}
	 *  
	 * @return the codec
	 */
	public int getCodec();
	
	/**
	 * duration of this track
	 * 
	 * @return the duration
	 */
	public long getDuration();
	
	/**
	 * chunks in the track
	 *  
	 * @return the chunkCounts
	 */
	public int getChunkCounts();
	
	/**
	 * bitrate of this track
	 * 
	 * @return the bitrate
	 */
	public int getBitrate();
	
	/**
	 * audio info
	 * 
	 * @return the audioInfo
	 */
	public voOSAudioInfo getAudioInfo();
	
	/**
	 * video info
	 * 
	 * @return the videoInfo
	 */
	public voOSVideoInfo getVideoInfo();
	
	/**
	 * subtitle info
	 * 
	 * @return the subtitleInfo
	 */
	public voOSSubtitleInfo getSubtitleInfo();
	
	/**
	 * sequence head data size
	 * @return the headSize
	 */
	public int getHeadSize();
	
	/**
	 * sequence head data 
	 *  
	 * @return the headData
	 */
	public char[] getHeadData();
	
	

}
