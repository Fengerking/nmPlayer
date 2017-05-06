/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

package com.visualon.OSMPPlayer;


public interface VOOSMPAnalyticsInfo {
   
    /**
     * Get how far back to report
     *
     * @return How far back to report (seconds).
     */
    int getLastTime();      
   
    /**
    * Get number of source dropped frames.
    *
    * This function returns the total count of dropped frames by the module which fetches assets from the source. 
    * If the playing time is ahead of next or second next key frame, 
    * we will drop the frames before this key frame to catch up to the video playing time. *
	*
    * @return number of source dropped frames
    */
    int getSourceDropNum();
   
    /**
     * Get number of codec dropped frames.
     * <p>
     * This function returns the total count of dropped frames at the input of the video decoder module.
     * Before decoding the current frame, if the playing time is ahead of current frame and this frame is not a reference frame, we will drop it to catch up to the video playing time.
     * </p>
     *
     * @return number of codec dropped frames
     */
    int getCodecDropNum();
   
    /**
     * Get number of render dropped frames.
     * <p>
     * This function returns the total count of dropped frames at the input of the rendering module.
     * Before rendering the current frame, if the playing time is ahead of current frame and there are more frames ready to be rendered, we will drop it to catch up to the video playing time.
     * <p>
     *
     * @return number of render dropped frames
     */
    int getRenderDropNum();
   
    /**
     * Get number of decoded frames.
     *
     * @return Number of decoded frames.
     */
    int getDecodedNum();
   
    /**
     * Get number of rendered frames
     *
     * @return Number of rendered frames
     */
    int getRenderNum();
   
    /**
     * Get number of instances that the source exceeds the time (I / ms)
     *
     * @return Number of instances that the source exceeds the time.
     */
    int getSourceTimeNum();            
   
    /**
     * Get number of instances that the  codec exceeds the time  (I / ms)
     *
     * @return Number of instances that the codec exceeds the time.
     */
    int getCodecTimeNum();
   
    /**
     * Get number of instances that the render exceeds the time (I / ms)
     *
     * @return Number of instances that the render exceeds the time.
     */
    int getRenderTimeNum();  
   
    /**
     * Get number of instances that the  jitter exceeds the time (I / ms)
     *
     * @return Number of instances that the jitter exceeds the time (I / ms).
     *
     */
    int getJitterNum();            
   
    /**
     * Get number of codec frames dropped after encountering errors
     *
     * @return number codec frames dropped after encountering errors.
     */
    int getCodecErrorsNum();
   
    /**
     * Get codec errors
     *
     * @return List of codec error codes.
     * */
    int[] getCodecErrors();              
   
    /**
     * Get current CPU load.
     *
     * @return Current CPU load (percent).
     *
     */
    int getCPULoad();  
   
    /**
     * Get current CPU frequency 
     *
     * @return Current CPU frequency.
     */
    int getFrequency();    
   
    /**
     * Get maximum CPU frequency
     *
     * @return Maximum CPU frequency.
     */
    int getMaxFrequency();  
   
    /**
     * Get worst codec decode time (ms)
     *
     * @return Worst codec decode time.
     */
    int getWorstDecodeTime();
   
    /**
     * Get worst render time (ms)
     *
     * @return Worst render time (ms).
     */
    int getWorstRenderTime();  
   
    /**
     * Get average codec decode time (ms)
     *
     * @return Average codec decode time (ms)
     */
    int getAverageDecodeTime();    
   
    /**
     * Get average render time (ms)
     *
     * @return Average render time (ms).
     */
    int getAverageRenderTime();        
   
    /**
     * Get current total CPU load
     *
     * @return Current total CPU load (percent).
     */
    int getTotalCPULoad();     

}
