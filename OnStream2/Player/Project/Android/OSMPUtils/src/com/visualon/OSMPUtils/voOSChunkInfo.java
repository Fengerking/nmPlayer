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
 * @file voOSChunkInfo.java
 * interface for getting Chunk information.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

public interface voOSChunkInfo {
	
	/**The type of this chunk */
	int		Type();    
	
	/**The URL of manifest. It must be filled by parser. */
	String	RootUrl();	
	
	/**URL of this chunk , maybe relative URL */
	String	Url();
	
	/**The start offset time of this chunk , the unit of ( ullStartTime / ullTimeScale * 1000 ) should be ms */
	long	StartTime();
	
	/** The sequence number of this chunk */
	int     PeriodSequenceNumber();
	
	/**Duration of this chunk , the unit of ( ullDuration / ullTimeScale * 1000 ) should be ms */
	long	Duration();
	
	/**Time scale of this chunk */
	long	TimeScale();         
	
	/**Reserved 1 */
	int		Reserved1();
	
	/**Reserved 1 */
	int		Reserved2();


}
