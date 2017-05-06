package com.visualon.VOOSMPStreamingDownloader;

import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;
import com.visualon.OSMPUtils.voLog;

public interface VOOSMPStreamingDownloaderListener {
	
	/**
     * Enumeration of callback event IDs.
     * VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID
     * <p>
     * This event is sent from sub-thread, please do not update UI or call other OSMP+ APIs in callback (except API in comments).
     * </p>
     */
	public enum VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID
	{
		/** Open complete */
        VO_OSMP_CB_STREAMING_DOWNLOADER_OPEN_COMPLETE            		(0X10000001),
        /** Manifest file download was successful. */
        VO_OSMP_CB_STREAMING_DOWNLOADER_MANIFEST_OK              		(0X10000002),
        /** Entire content download completed */
        VO_OSMP_CB_STREAMING_DOWNLOADER_END                      		(0X10000004),
        /** Program information has changed */
        VO_OSMP_CB_STREAMING_DOWNLOADER_PROGRAM_INFO_CHANGE      		(0X10000005),
        /** Manifest file is updated, param1 is {@link VOOSMPStreamingDownloaderProgressInfo} */
        VO_OSMP_CB_STREAMING_DOWNLOADER_MANIFEST_UPDATE          		(0X10000006),
        
        /** Manifest file download has failed */
        VO_OSMP_CB_STREAMING_DOWNLOADER_DOWNLOAD_MANIFEST_FAIL  		(0X90000001),
        /** Manifest file write has failed */
        VO_OSMP_CB_STREAMING_DOWNLOADER_WRITE_MANIFEST_FAIL      		(0X90000002),
        /** Chunk file download has failed */
        VO_OSMP_CB_STREAMING_DOWNLOADER_DOWNLOAD_CHUNK_FAIL      		(0X90000003),
        /** Chunk file write has failed */
        VO_OSMP_CB_STREAMING_DOWNLOADER_WRITE_CHUNK_FAIL         		(0X90000004),
        /** Disk is full */
        VO_OSMP_CB_STREAMING_DOWNLOADER_DISK_FULL                		(0X90000005),
        /** Live stream is not supported. */
        VO_OSMP_CB_STREAMING_DOWNLOADER_LIVE_STREAM_NOT_SUPPORT			(0X90000007),
        /** Stream on local disk not supported.*/
        VO_OSMP_CB_STREAMING_DOWNLOADER_LOCAL_STREAM_NOT_SUPPORT		(0X90000008),
        
        /** Max value definition */
        VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID_UNDEFINED               (0xFFFFFFFF);

		
		private int value;
		
		VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID(int value)
		{
			this.value = value;
		}
		
		public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID valueOf(int value)
    	{
    		for (int i = 0; i < VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID.values().length; i ++)
    		{
    			if (VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID.values()[i].getValue() == value)
    				return VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID.values()[i];
    		}
    		
    		voLog.e("@@@DownloaderListener", "@@@VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID isn't match. id = " + Integer.toHexString(value));
    		return VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID_UNDEFINED;
    	}
	}
	
	public interface VOOSMPStreamingDownloaderProgressInfo
	{
		/**
		 * Duration of stream which is downloaded.The units should be in ms.
		 */
	    int getDownloadedStreamDuration();
	    
	    /**
	     * Total duration of stream.The units should be in ms.
	     */
	    int getTotalStreamDuration();
	}
	
	/**
	 * Protocol to process events. The client can implement the listener to receive/manage events.
	 *
	 * @param nID    [out] Event type. Refer to {@link VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID}.
	 * @param param1 [out] First  parameter, specific to the event if needed.
	 * @param param2 [out] Second parameter, specific to the event if needed.
	 * @param obj    [out] Third  parameter, specific to the event if needed.
	 *
	 * @return {@link VO_OSMP_ERR_NONE} if successful.
	 */
    public VO_OSMP_RETURN_CODE onVOStreamingDownloaderEvent(VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID id, int param1, int param2, Object obj);
    
}
