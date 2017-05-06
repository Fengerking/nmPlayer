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
 * @file voOSCPUInfo.java
 * interface for getting CPU Info information.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

public interface voOSCPUInfo {
	/**
	 * CPU info
	 */

	/**The codec you want to query performance data */
	public int			CoreCount();		
	
	/**The CPU type,0:neon not supoort, 1:support neon */
	public int			CPUType();	

	/**The frequency of CPU */
	public int			Frequency();		
	
	/**The reserved value */
	public long			ReservedField();		
		

}
