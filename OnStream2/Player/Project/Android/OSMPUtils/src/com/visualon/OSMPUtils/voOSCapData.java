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
 * get information about current playing media.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

public interface voOSCapData {
	
	/**The codec you want to query performance data */
	public int		getCodecType();	
	
	/**The bitrate */
	public int		getBitRate();		
	
	/**The width of video */
	public int		getVideoWidth();	
	
	/**The height of video */
	public int		getVideoHeight();	
	
	/**The profile's level, 0:baseline,1:main profile,2:high profile*/
	public int		getProfileLevel();	
	
	/**The frequency of video  */
	public int		getFPS();

}
