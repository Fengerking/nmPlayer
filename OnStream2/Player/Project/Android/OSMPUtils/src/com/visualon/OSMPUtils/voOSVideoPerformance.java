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
 * @file  voOSVideoPerformance.java
 * interface for getting  Video Performance information.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

public interface voOSVideoPerformance {

	/**
	 * Time to look back in 
	 */
	public int LastTime();       
	
	/**
	 * Source Drop frame number
	 */
	public int SourceDropNum();
	
	/**
	 * Codec dropped frame number 
	 */
	public int CodecDropNum();
	
	/**
	 * Render dropped frame number
	 */
	public int RenderDropNum();
	
	/**
	 * Decoded frame number
	 */
	public int DecodedNum(); 
	
	/**
	 * Rendered frame number
	 */
	public int RenderNum();
	
	/**
	 * Number source exceeds the time (I / ms) 
	 */
	public int SourceTimeNum();             
	
	/**
	 * Number codec exceeds the time  (I /ms) 
	 */
	public int CodecTimeNum();
	
	/**
	 * Number render exceeds the time (I /ms) 
	 */
	public int RenderTimeNum();   
	
	/**
	 * Number jitter exceeds the time (I /ms) 
	 */
	public int JitterNum();            
	
	/**
	 * Number codec dropped with encounter errors 
	 */
	public int CodecErrorsNum();
	
	/**
	 * 
	 * Codec errors 
	 * */
	public int[] CodecErrors();              
	
	/**
	 * Current CPU load in percent
	 */
	public int CPULoad();   
	
	/**
	 * Current frequency CPU is scaled to 
	 */
	public int Frequency();    
	
	/**
	 * Maximum frequency CPU 
	 */
	public int MaxFrequency();   
	
	/**
	 * Worst codec decode time (ms) 
	 */
	public int WorstDecodeTime();
	
	/**
	 * Worst render time (ms) 
	 */
	public int WorstRenderTime();   
	
	/**
	 * Average codec decode time (ms) 
	 */
	public int AverageDecodeTime();     
	
	/**
	 * Average render time (ms) 
	 */
	public int AverageRenderTime();         
	
	/**
	 * Current total CPU load in percent
	 * @deprecated this API remove for task 27762.
	 */
	public int TotalCPULoad();				

	/**
	 * Total duration from beginning of playback to  the time analytics is queried. This number is reset after seek
	 * @deprecated this API remove for task 27762.
	 */
	public int TotalPlaybackDuration();				

	/**
	 * Total number of dropped frames by source from beginning of playback to the time analytics is queried. This number is reset after seek
	 * @deprecated this API remove for task 27762.
	 */
	public int TotalSourceDropNum();				

	/**
	 * Total number of dropped frames by codec from beginning of playback to the time analytics is queried. This number is reset after seek
	 * @deprecated this API remove for task 27762.
	 */
	public int TotalCodecDropNum();				

	/**
	 * Total number of dropped frames by render from beginning of playback to the time analytics is queried. This number is reset after seek
	 * @deprecated this API remove for task 27762.
	 */
	public int TotalRenderDropNum();				

	/**
	 * Total number of decoded frames from beginning of playback to the time analytics is queried. This number is reset after seek
	 * @deprecated this API remove for task 27762.
	 */
	public int TotalDecodedNum();				

	/**
	 * Total number of rendered frames from beginning of playback to the time analytics is queried. This number is reset after seek
	 * @deprecated this API remove for task 27762.
	 */
	public int TotalRenderNum();				
}
