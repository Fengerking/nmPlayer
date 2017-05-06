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

public interface VOOSMPChunkInfo {
   
    /** Get the type of this chunk
     *
     * @return Chunk type 
     */
    int     getType();    
   
    /**
     * The root URL of manifest. It must be filled by parser.
     *
     * @return root URL
     */
    String  getRootURL();  
   
    /**
     * The URL of this chunk. It can be a relative URL.
     *
     * @return chunk URL
     */
    String  getURL();
   
    /** Get the start offset time of this chunk; the units of ( ullStartTime startTime / ullTimeScale timeScale * 1000 ) should be ms.
     *
     * @return Start offset time of this chunk
     */
    long    getStartTime();
   
    /** Get duration of this chunk; the units of ( ullDuration duration / ullTimeScale timeScale * 1000 ) should be ms.
     *
     * @return Duration of this chunk.
     */
    long    getDuration();
   
    /** Get the time scale of this chunk
     *
     * @return Time scale of this chunk.
     */
    long    getTimeScale();        


}
  
