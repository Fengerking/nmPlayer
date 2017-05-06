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
 * @file voOSPerformanceData.java
 * interface for getting Performance Data information.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

public interface voOSPerformanceData {
	
	/**The codec you want to query performance data */
	public int		CodecType();	
	
	/**The bitrate */
	public int		BitRate();		
	
	/**The width of video */
	public int		VideoWidth();	
	
	/**The height of video */
	public int		VideoHeight();	
	
	/**The profile's level, 0:baseline,1:main profile,2:high profile*/
	public int		ProfileLevel();	
	
	/**The frequency of video  */
	public int		FPS();

}
