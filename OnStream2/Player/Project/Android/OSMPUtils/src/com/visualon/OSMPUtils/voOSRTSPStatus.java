package com.visualon.OSMPUtils;

import com.visualon.OSMPUtils.voOSType.VOOSMP_SOURCE_STREAMTYPE;

/** 
 * @deprecated this API remove for task 27762 .
 */
public interface voOSRTSPStatus {
	
	/** track type */
	VOOSMP_SOURCE_STREAMTYPE        getTrackType();
	
	/** The cumulative number of RTP media packets received in this media track during this session */
	int                    			getPacketRecved();		
	
	/** The cumulative number of RTP media packets previously received in this media track during this session */
	int                    			getPacketDuplicated();		
	
	/** The cumulative number of RTP media packets lost for this media track type */
	int                    			getPacketLost();		
	
	/** The cumulative number of RTP media packets sent in this media track during this session */
	int                    			getPacketSent();
	
	/** The mean relative transit time between each two RTP packets of this media track type throughout the duration of the media session */
	int                    			getAverageJitter();
	
	/** Average accumulating latency values of all RTP media packets transmitted from the server to the client, !!now only reserved!! */
	int                    			getAverageLatency();


}
