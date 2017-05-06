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


public interface VOOSMPRTSPStatistics {
   
   /** The cumulative number of RTP media packets received in this media track during this session 

  @return Cumulative number of RTP media packets received in this media track during this session
   */
    int                             getPacketReceived();     
   
    /** The cumulative number of RTP media packets previously received in this media track during this session 
* @return Cumulative number of RTP media packets previously received in this media track during this session
*/
    int                             getPacketDuplicated();     
   
    /** The cumulative number of RTP media packets lost for this media track type 
*
* @return Cumulative number of RTP media packets lost for this media track type */
    int                             getPacketLost();       
   
    /** The cumulative number of RTP media packets sent in this media track during this session 
*
* @return Cumulative number of RTP media packets sent in this media track during this session
*/
    int                             getPacketSent();
   
    /** The mean relative transit time between each two RTP packets of this media track type throughout the duration of the media session 
*
* @return The mean relative transit time  between each two RTP packets of this media track type throughout the duration of the media session
*/
    int                             getAverageJitter();
   
    /** Average accumulating latency values of all RTP media packets transmitted from the server to the client. Currently reserved only. 
*
* @return Average accumulating latency value 
*/
    int                             getAverageLatency();


}
