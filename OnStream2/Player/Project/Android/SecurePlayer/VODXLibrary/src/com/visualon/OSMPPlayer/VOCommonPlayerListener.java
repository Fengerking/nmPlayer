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

import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_ASPECT_RATIO;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_SRC_PROGRAM_TYPE;
import com.visualon.OSMPUtils.voLog;

/**
 * Interface for event process. The client who wants to receive event from Player shall implement the interface.
 */
public interface VOCommonPlayerListener
{
	public static final String TAG = "@@@VOCommonPlayerListener";
	
    /**
     * Enumeration for callback EventID
     */
   
    public enum VO_OSMP_CB_EVENT_ID
    {
        /** Unknown error */
        VO_OSMP_CB_ERROR                              (0X8000000C),
        /** Source playback complete */
        VO_OSMP_CB_PLAY_COMPLETE                      (0X00000001),
        /** Video stream started buffering */
        VO_OSMP_CB_VIDEO_START_BUFFER                 (0X00000003),
        /** Video stream stopped buffering */
        VO_OSMP_CB_VIDEO_STOP_BUFFER                  (0X00000004),
        /** Audio stream started buffering */
        VO_OSMP_CB_AUDIO_START_BUFFER                 (0X00000005),
        /** Audio stream stopped buffering */
        VO_OSMP_CB_AUDIO_STOP_BUFFER                  (0X00000006),
        /** Buffer time in source; param1 is (int)audio (ms) value, param2 is (int)video (ms) */
        VO_OSMP_CB_SRC_BUFFER_TIME                    (0X00000007),
        /** Seek is finished */
        VO_OSMP_CB_SEEK_COMPLETE                      (0X0000000D),
        /** Video aspect ratio changed, param1 is {@link VO_OSMP_ASPECT_RATIO}. Refer to
         * <ul>
         * <li>0 for {@link VO_OSMP_ASPECT_RATIO#VO_OSMP_RATIO_00}
         * <li>1 for {@link VO_OSMP_ASPECT_RATIO#VO_OSMP_RATIO_11}
         * <li>2 for {@link VO_OSMP_ASPECT_RATIO#VO_OSMP_RATIO_43}
         * <li>3 for {@link VO_OSMP_ASPECT_RATIO#VO_OSMP_RATIO_169}
         * <li>4 for {@link VO_OSMP_ASPECT_RATIO#VO_OSMP_RATIO_21}
         * <li>5 for {@link VO_OSMP_ASPECT_RATIO#VO_OSMP_RATIO_2331}
         * </ul>
         * */
        VO_OSMP_CB_VIDEO_ASPECT_RATIO                 (0X0000000E),
        /** Video size changed; param1 is (int) video width (px), param2 is (int)video height (px)*/
        VO_OSMP_CB_VIDEO_SIZE_CHANGED                 (0X0000000F),
        /** Codec not supported */
        VO_OSMP_CB_CODEC_NOT_SUPPORT                  (0X80000010),
        /** Video codec deblock event; param1 is 0:disable, 1:enable */
        VO_OSMP_CB_DEBLOCK                            (0X00000011),
        /** Hardware decoder status available event; param1 is 0: error, 1: available */
        VO_OSMP_CB_HW_DECODER_STATUS                  (0X00000013),  
        /** Authentication response information; e.g. report information from server;
         *  Third parameter obj is String: (detailed information).*/
        VO_OSMP_CB_AUTHENTICATION_RESPONSE            (0X00000014),
        /** Subtile language info is parsed and available */
        VO_OSMP_CB_LANGUAGE_INFO_AVAILABLE            (0X00000015),
        /**video render started*/
        VO_OSMP_CB_VIDEO_RENDER_START                 (0X00000016),
        /** Open Source complete */
        VO_OSMP_CB_OPEN_SRC_COMPLETE                  (0X00000017),
        /** Notify SEI info, param1 is a {@link VO_OSMP_SEI_INFO_FLAG}, pObj is an object depending on param1 */
        VO_OSMP_CB_SEI_INFO                           (0X00000019),
        /** Audio render failed */
        VO_OSMP_CB_AUDIO_RENDER_FAIL                  (0X8000001A),
        /** This event will be issued only when PCM data output is enabled. pObj should be VOOSMPPCMBuffer */
        VO_OSMP_CB_PCM_OUTPUT                         (0X0000001C),
        /** Notify license check failed */
        VO_OSMP_CB_LICENSE_FAIL                       (0X8000001D),
   
        /** Bluetooth handset status; param1 is 0: bluetooth off, 1: bluetooth on */
        VO_OSMP_CB_BLUETOOTHHANDSET_CONNECTION        (0x00001011),
        /** Playback performance analytics info available; param obj is {@link VOOSMPAnalyticsInfo} 
         * 
         */
        VO_OSMP_CB_ANALYTICS_INFO                     (0x00001021),
        
        
        /** Notify once when the player starts displaying the first ad video or content video 
         *
         * */
        VO_OSMP_AD_CB_PLAYLIST_START                  (0X03000001),
        /** Notify once when the player finishes displaying the last ad video or content video 
         * 
         * */
        VO_OSMP_AD_CB_PLAYLIST_END                    (0X03000002),
        /** Notify each time a content segment is started  
         * 
         * */
        VO_OSMP_AD_CB_CONTENT_START                   (0X03000003),
        /** Notify each time a content segment is end 
         * 
         * */
        VO_OSMP_AD_CB_CONTENT_END                     (0X03000004),
        /** Notify each time a video ad is started  
         * 
         * */
        VO_OSMP_AD_CB_AD_START                        (0X03000005),
        /** Notify each time a video ad is end  
         * 
         * */
        VO_OSMP_AD_CB_AD_END                          (0X03000006),
        /** Notify current playing time, param1 is the playing time timestamp(ms) 
         * 
         * */
        VO_OSMP_AD_CB_VIDEO_PROGRESS                  (0X03000007),
        /** Notify ad and content video is end 
         * 
         * */
        VO_OSMP_AD_CB_VIDEO_DONE                      (0X03000008),
        /** Notify player's playback state changes, param1 is {@link VO_OSMP_AD_STATUS} 
         * 
         * */
        VO_OSMP_AD_CB_STATE_CHANGE                    (0X03000009),
        /** Get playback info, Object is {@link VOOSMPADSInfo} 
         * 
         * */
        VO_OSMP_AD_CB_PLAYBACKINFO                    (0X0300000A),
        /** Notify when an ad has no content or didn't load expected 
         *
         * */
        VO_OSMP_AD_CB_NO_AD_CONTENT                   (0X8300000B),
        /** Notify when the data for an ad request can't be loaded 
         * 
         * */
        VO_OSMP_AD_CB_AD_LOAD_ERROR                   (0X8300000C),
        /** Notify when the conent video has expired or is unavailable 
         * 
         * */
        VO_OSMP_AD_CB_VIDEO_NOT_AVAILABLE             (0X8300000D),
        /** Notify when the content video experiences a fatal error while loading or playback  
         * 
         * */
        VO_OSMP_AD_CB_DATA_LOAD_ERROR                 (0X8300000E),
        /** Notify when the GEO blocked
         * 
         * */
        VO_OSMP_AD_CB_VIDEO_GEO_BLOCKED               (0X8300000F),
        

        /**
         * Enumeration source callback event ID
         */
        /** Source is connecting */
        VO_OSMP_SRC_CB_CONNECTING                     (0X02000001),
        /** Source connection is finished */
        VO_OSMP_SRC_CB_CONNECTION_FINISHED            (0X02000002),
        /** Source connection timeout */
        VO_OSMP_SRC_CB_CONNECTION_TIMEOUT             (0X82000003),
        /** Source connection lost */
        VO_OSMP_SRC_CB_CONNECTION_LOSS                (0X82000004),
        /** HTTP download status; param1 is int (0 - 100) */
        VO_OSMP_SRC_CB_DOWNLOAD_STATUS                (0X02000005),
        /** Source connection failed */
        VO_OSMP_SRC_CB_CONNECTION_FAIL                (0X82000006),
        /** Source download failed */
        VO_OSMP_SRC_CB_DOWNLOAD_FAIL                  (0X82000007),
        /** DRM engine error */
        VO_OSMP_SRC_CB_DRM_FAIL                       (0X82000008),
        /** Playlist parse error */
        VO_OSMP_SRC_CB_PLAYLIST_PARSE_ERR             (0X82000009),
        /** Maximum number of connections was reached, Currently used for RTSP only. */
        VO_OSMP_SRC_CB_CONNECTION_REJECTED            (0X8200000A),
        
        /** Bit rate is changed; param1 is new bit rate value (bps), integer type */
        VO_OSMP_SRC_CB_BA_HAPPENED                    (0X0200000B),
        /** Device is rooted/jailbroken, DRM not secure */
        VO_OSMP_SRC_CB_DRM_NOT_SECURE                 (0X0200000C),
        /** Device uses A/V output device but the license doesn't support it */
        VO_OSMP_SRC_CB_DRM_AV_OUT_FAIL                (0X8200000D),
        /** Download failed, waiting to recover */
        VO_OSMP_SRC_CB_DOWNLOAD_FAIL_WAITING_RECOVER  (0X8200000E),
        /** Download recovery success. */
        VO_OSMP_SRC_CB_DOWNLOAD_FAIL_RECOVER_SUCCESS  (0X0200000F),
        /** Source open finished; 0:success 1:failed */
        VO_OSMP_SRC_CB_OPEN_FINISHED                  (0X02000010),  
        /** Customer tag information available inside source; param1 is {@link VO_OSMP_SRC_CUSTOMERTAGID} and other params will depend on param1.*/
        VO_OSMP_SRC_CB_CUSTOMER_TAG                   (0X02000020),
        /** Streaming information; param1 is {@link VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT} and other params will depend on param1*/
        VO_OSMP_SRC_CB_ADAPTIVE_STREAMING_INFO        (0X02000030),
        /** Adaptive streaming error, param1 is {@link VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT} and other params will depend on param1 */
        VO_OSMP_SRC_CB_ADAPTIVE_STREAMING_ERROR       (0X02000040),  
        /** Adaptive streaming error warning, param1 is {@link VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT} and other params will depend on param1*/
        VO_OSMP_SRC_CB_ADAPTIVE_STREAM_WARNING        (0X02000050),
        /** Notify RTSP error, param1 will be defined to {@link VO_OSMP_SRC_RTSP_ERROR} */
        VO_OSMP_SRC_CB_RTSP_ERROR                     (0X02000060),    
        /**Notify seek complete, param1 is the seek return timestamp (ms), ,
         * param2 is the seek result return code,
         * {@link VO_OSMP_RETURN_CODE#VO_OSMP_SRC_ERR_SEEK_FAIL} or {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE}
         */
        VO_OSMP_SRC_CB_SEEK_COMPLETE                  (0X02000070),
        /**Notify the program info has been changed in source */
        VO_OSMP_SRC_CB_PROGRAM_CHANGED                (0X02000071),
        /**Notify the program info has been reset in source */
        VO_OSMP_SRC_CB_PROGRAM_RESET                  (0x02000072),
        /**Notify when seek to the last chunk of play list (NTS link without END tag need change to live mode) */
        VO_OSMP_SRC_CB_ADAPTIVE_STREAMING_SEEK2LASTCHUNK (0x02000073),

		/**Network is not available now,but it still hase some buffering data which can playback */
		VO_OSMP_SRC_CB_NOT_APPLICABLE_MEDIA 		  (0x02000074),
		
		/** Notify the current media position downloaded by the progressive download module, param1 is the position */
		VO_OSMP_SRC_CB_PD_DOWNLOAD_POSITION 		  (0x02000075),
		
		/** Notify the current buffering percent of the progressive download module, param1 is the percent */
		VO_OSMP_SRC_CB_PD_BUFFERING_PERCENT 		  (0x02000076),
		
		/** Update source URL complete, param1 is {@link VO_OSMP_CB_EVENT_ID} */
        VO_OSMP_SRC_CB_UPDATE_URL_COMPLETE            (0X02000077),
		
        VO_OSMP_CB_EVENT_ID_MAX                       (0xFFFFFFFF);
       
        private int value;
       
        VO_OSMP_CB_EVENT_ID(int value)
        {
            this.value = value;
        }
       
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_CB_EVENT_ID valueOf(int value)
    	{
    		for (int i = 0; i < VO_OSMP_CB_EVENT_ID.values().length; i ++)
    		{
    			if (VO_OSMP_CB_EVENT_ID.values()[i].getValue() == value)
    				return VO_OSMP_CB_EVENT_ID.values()[i];
    		}
    		
    		voLog.e("VOCommonPlayerListener", "VO_OSMP_CB_EVENT_ID isn't match. id = " + Integer.toHexString(value));
    		return VO_OSMP_CB_EVENT_ID_MAX;
    	}
    }
    
    /**
     * Enumeration of callback sync event IDs
     * VO_OSMP_CB_SYNC_EVENT_ID
     * <p>
     * This event be sent from sub-thread, please don't update UI or call other visualon's api in callback (except API in comments).
     * </p>
     */
    public enum VO_OSMP_CB_SYNC_EVENT_ID
    {
        /** Request DRM authentication information {@link VOCommonPlayerConfiguration#setDRMVerificationInfo}
         *  Set authentication information when receving this event
         */
        VO_OSMP_SRC_CB_SYNC_AUTHENTICATION_DRM_SERVER_INFO  (0X03000001),

        /** Start HTTP download. Parameter obj is the URL (String)
         *  Can set HTTP information by API in VOCommonPlayerHTTPConfiguration.
         */
        VO_OSMP_SRC_CB_SYNC_IO_HTTP_START_DOWNLOAD          (0X03000002),

        /** HTTP download failed. Parameter obj is {@link VOOSMPHTTPDownloadFailure}
         *  Can set HTTP information by API in VOCommonPlayerHTTPConfiguration.
         */
        VO_OSMP_SRC_CB_SYNC_IO_HTTP_DOWNLOAD_FAIL           (0X03000003),

		/**Event of DRM init data,param is VOOSMP_SRC_DRM_INIT_DATA */
		VO_OSMP_SRC_CB_DRM_INIT_DATA                        (0X03000004),

        VO_OSMP_CB_SYNC_EVENT_ID_MAX                        (0XFFFFFFFF);
        
        private int value;
        VO_OSMP_CB_SYNC_EVENT_ID (int event)
        {
            value = event;
        }
       
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_CB_SYNC_EVENT_ID valueOf(int value)
        {
            for (int i = 0; i < VO_OSMP_CB_SYNC_EVENT_ID.values().length; i ++)
            {
                if (VO_OSMP_SRC_RTSP_ERROR.values()[i].getValue() == value)
                    return VO_OSMP_CB_SYNC_EVENT_ID.values()[i];
            }
            voLog.e("VOCommonPlayerListener", "VO_OSMP_CB_SYNC_EVENT_ID isn't match. id = " + Integer.toHexString(value));
            return VO_OSMP_CB_SYNC_EVENT_ID_MAX;
        }
    }
   
    /** Enumeration of HLS customer tag IDs. Used in VO_OSMP_SRC_CB_Customer_Tag callback.
     * This enumeration is param1, param2 is time stamp (ms), obj is byte[] TimedTag content.
     */
    public enum VO_OSMP_SRC_CUSTOMERTAGID
    {
        VO_OSMP_SRC_CUSTOMERTAGID_TIMEDTAG           (0X00000001),
        
         /** Max value definition */
        VO_OSMP_SRC_CUSTOMERTAGID_MAX                (0XFFFFFFFF);
       
        private int value;
       
        VO_OSMP_SRC_CUSTOMERTAGID(int value)
        {
            this.value = value;
        }
       
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_SRC_CUSTOMERTAGID valueOf(int value)
    	{
    		for (int i = 0; i < VO_OSMP_SRC_CUSTOMERTAGID.values().length; i ++)
    		{
    			if (VO_OSMP_SRC_CUSTOMERTAGID.values()[i].getValue() == value)
    				return VO_OSMP_SRC_CUSTOMERTAGID.values()[i];
    		}
    		
    		voLog.e("VOCommonPlayerListener", "VO_OSMP_SRC_CUSTOMERTAGID isn't match. id = " + Integer.toHexString(value));
    		return VO_OSMP_SRC_CUSTOMERTAGID_MAX;
    	}
    }
   
    /**
     * Enumeration of available stream info event.
     * Used in VO_OSMP_SRC_CB_Adaptive_Streaming_Info callback.
     */
    public enum VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT
    {
        /** Bitrate changed. {@link VOCommonPlayerListener} callback param2 is new bitrate. */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_BITRATE_CHANGE                   (0x00000001),
        /** Media type changed. {@link VOCommonPlayerListener} callback param2 is {@link VO_OSMP_AVAILABLE_TRACK_TYPE}. Refer to
         * <ul>
         * <li>0 for {@link VO_OSMP_AVAILABLE_TRACK_TYPE#VO_OSMP_AVAILABLE_PUREAUDIO}
         * <li>1 for {@link VO_OSMP_AVAILABLE_TRACK_TYPE#VO_OSMP_AVAILABLE_PUREVIDEO}
         * <li>2 for {@link VO_OSMP_AVAILABLE_TRACK_TYPE#VO_OSMP_AVAILABLE_AUDIOVIDEO}
         * </ul>
         * */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_MEDIATYPE_CHANGE                 (0x00000002),
        /** Program type. {@link VOCommonPlayerListener} callback param2 {@link VO_OSMP_SRC_PROGRAM_TYPE}. Refer to
         * <ul>
         * <li>0 for {@link VO_OSMP_SRC_PROGRAM_TYPE#VO_OSMP_SRC_PROGRAM_TYPE_LIVE}
         * <li>1 for {@link VO_OSMP_SRC_PROGRAM_TYPE#VO_OSMP_SRC_PROGRAM_TYPE_VOD}
         * </ul>
         */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PROGRAM_TYPE                     (0x00000003),
        /** Chunk download begin. {@link VOCommonPlayerListener} callback param obj will be {@link VOOSMPChunkInfo}.
         *  If the return value of this callback is {@link VO_OSMP_RETURN_CODE#VO_OSMP_SRC_ERR_CHUNK_SKIP}, source must drop this chunk;
         *  This event is synced with callback by onRequestListener, you should return a value */ 
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_BEGINDOWNLOAD              (0x00000004),    
        /** Chunk download dropped. {@link VOCommonPlayerListener} callback param obj will be {@link VOOSMPChunkInfo}. */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DROPPED                    (0x00000005),
        /** Chunk download OK. VO_OSMPListener{@link VOCommonPlayerListener} callback param obj will be {@link VOOSMPChunkInfo}. */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DOWNLOADOK                 (0x00000006),
        /**Indicate Playlist download OK */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PLAYLIST_DOWNLOADOK              (0x00000007),    
        /**Indicates that Program has been changed in source. When this event is received, you should get program info again. Parameter 2 shall be ignored */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PROGRAM_CHANGE                   (0x00000008),
        /**Indicates chunk is supported */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_FILE_FORMATSUPPORTED             (0x00000009),
        /** Indicates live clip can be seeked now */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_LIVESEEKABLE                     (0x0000000A),
        /** Indicate this is the first sample from the discontinue chunk */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_DISCONTINUE_SAMPLE			   (0x0000000B),
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_MAX                              (0XFFFFFFFF);
       
        private int value;
       
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT(int value)
        {
            this.value = value;
        }
       
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT valueOf(int value)
        {
	        for (int i = 0; i < VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT.values().length; i ++)
			{
				if (VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT.values()[i].getValue() == value)
					return VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT.values()[i];
			}
	        voLog.e("VOCommonPlayerListener", "VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT isn't match. id = " + Integer.toHexString(value));
			return VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT.VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_MAX;
        }
    }
   
    /**
    * Enumeration of available streaming error codes.
    * Used in VO_OSMP_SRC_CB_Adaptive_Streaming_Error callback.
    */
    public enum VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT
    {
        /** Playlist parsing failed */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_PLAYLIST_PARSEFAIL              (0x00000001),
        /** Playlist not supported */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_PLAYLIST_UNSUPPORTED            (0x00000002),
        /** Stream not supported */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_UNSUPPORTED           (0x00000003),
        /** Download failed */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_DOWNLOADFAIL          (0x00000004),
        /** DRM error */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_DRMLICENSEERROR       (0x00000005), 
        /**Notify License error */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_VOLIBLICENSEERROR     (0x00000006),
       
        VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_MAX                             (0xFFFFFFFF);
       
        private int value;
       
        VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT(int value)
        {
            this.value = value;
        }
       
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT valueOf(int value)
        {
	        for (int i = 0; i < VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT.values().length; i ++)
			{
				if (VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT.values()[i].getValue() == value)
					return VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT.values()[i];
			}
	        voLog.e("VOCommonPlayerListener", "VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT isn't match. id = " + Integer.toHexString(value));
			return VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_MAX;
        }
    }
   
    /**
    * Enumeration of available streaming warning events.
    * Used in VO_OSMP_SRC_CB_Adaptive_Stream_Warning callback.
    */
    public enum VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT
    {
        /** Chunk download failed */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_DOWNLOADERROR           (0x00000001),
        /** Chunk format not supported */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_FILEFORMATUNSUPPORTED   (0x00000002),
        /** DRM error */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_DRMERROR                (0x00000003),
        /** Playlist download failed */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_PLAYLIST_DOWNLOADERROR        (0x00000004),
       
        VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_MAX                           (0xFFFFFFFF);
       
        private int value;
       
        VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT(int value)
        {
            this.value = value;
        }
       
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT valueOf(int value)
        {
	        for (int i = 0; i < VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT.values().length; i ++)
			{
				if (VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT.values()[i].getValue() == value)
					return VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT.values()[i];
			}
	        voLog.e("VOCommonPlayerListener", "VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT isn't match. id = " + Integer.toHexString(value));
			return VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_MAX;
        }
    }
   
    /**
     * Enumeration of available track types.
     * Used in {@link VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT#VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_MEDIATYPE_CHANGE} callback.
     *
     */
    public enum VO_OSMP_AVAILABLE_TRACK_TYPE
    {
        /** Only audio is available */
        VO_OSMP_AVAILABLE_PUREAUDIO       (0x00000000),
        /** Only video is available */
        VO_OSMP_AVAILABLE_PUREVIDEO       (0x00000001),
        /** Both audio and video are available */
        VO_OSMP_AVAILABLE_AUDIOVIDEO      (0x00000002),
       
        VO_OSMP_AVAILABLE_MAX             (0xFFFFFFFF);
       
        private int value;
       
        VO_OSMP_AVAILABLE_TRACK_TYPE(int value)
        {
            this.value = value;
        }
       
        public int getValue()
        {
            return value;
        }
        
    	public static VO_OSMP_AVAILABLE_TRACK_TYPE valueOf(int value)
    	{
    		for (int i = 0; i < VO_OSMP_AVAILABLE_TRACK_TYPE.values().length; i ++)
    		{
    			if (VO_OSMP_AVAILABLE_TRACK_TYPE.values()[i].getValue() == value)
    				return VO_OSMP_AVAILABLE_TRACK_TYPE.values()[i];
    		}
    		voLog.e("VOCommonPlayerListener", "VO_OSMP_AVAILABLE_TRACK_TYPE isn't match. id = " + Integer.toHexString(value));
    		return VO_OSMP_AVAILABLE_MAX;
    	}
    }
   
    /**
     * Enumeration of RTSP error
     */
    public enum VO_OSMP_SRC_RTSP_ERROR
    {
        /**Notify connect failed in RTSP*/
        VO_OSMP_SRC_RTSP_ERROR_CONNECT_FAIL           (0x00000001),
       
        /**Notify describle failed in RTSP*/
        VO_OSMP_SRC_RTSP_ERROR_DESCRIBE_FAIL          (0x00000002),
       
        /**Notify setup failed in RTSP*/
        VO_OSMP_SRC_RTSP_ERROR_SETUP_FAIL             (0x00000003),
       
        /**Notify play failed in RTSP*/
        VO_OSMP_SRC_RTSP_ERROR_PLAY_FAIL              (0x00000004),
       
        /**Notify pause failed in RTSP*/
        VO_OSMP_SRC_RTSP_ERROR_PAUSE_FAIL             (0x00000005),
       
        /**Notify option failed in RTSP*/
        VO_OSMP_SRC_RTSP_ERROR_OPTION_FAIL            (0x00000006),
       
        /**Notify socket error in RTP channel of RTSP */
        VO_OSMP_SRC_RTSP_ERROR_SOCKET_ERROR           (0x00000007),
        
        /** Max value definition    */
        VO_OSMP_SRC_RTSP_ERROR_MAX                    (0XFFFFFFFF);
       
        private int value;
       
        VO_OSMP_SRC_RTSP_ERROR (int event)
        {
            value = event;
        }
       
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_SRC_RTSP_ERROR valueOf(int value)
    	{
    		for (int i = 0; i < VO_OSMP_SRC_RTSP_ERROR.values().length; i ++)
    		{
    			if (VO_OSMP_SRC_RTSP_ERROR.values()[i].getValue() == value)
    				return VO_OSMP_SRC_RTSP_ERROR.values()[i];
    		}
    		voLog.e("VOCommonPlayerListener", "VO_OSMP_SRC_RTSP_ERROR isn't match. id = " + Integer.toHexString(value));
    		return VO_OSMP_SRC_RTSP_ERROR_MAX;
    	}
    }
   
    /**
     * Enumeration of player's playback status.
     * Used in {@link VO_OSMP_AD_CB_STATE_CHANGE} callback.
     * 
     */
    public enum VO_OSMP_AD_STATUS
    {
        /** When a video is first loaded,player will broadcast this event  */
        VO_OSMP_AD_STATUS_UNSTARTED                 (-1),
        /** Video stopped */
        VO_OSMP_AD_STATUS_STOPPED                   (0),
        /** Video is playing */
        VO_OSMP_AD_STATUS_PLAYING                   (1),
        /** Video paused */
        VO_OSMP_AD_STATUS_PAUSED                    (2),
        /** Playback buffering */
        VO_OSMP_AD_STATUS_BUFFERING                 (3),
        /** Domain blacklisted */
        VO_OSMP_AD_STATUS_DOMAIN_BLACKLISTED        (100),
        /** Conent video has expired or is unavailable */
        VO_OSMP_AD_STATUS_VIDEO_NOT_AVAILABLE       (110),
        /** Content video experiences a fatal error while loading or playback */
        VO_OSMP_AD_STATUS_DATA_LOAD_ERROR           (130),
        /** Data for an ad request can't be loaded */
        VO_OSMP_AD_STATUS_AD_LOAD_ERROR             (140),
        
        /** Max value definition */
        VO_OSMP_AD_STATUS_MAX                       (0xFFFFFFFF);
       
        private int value;
       
        VO_OSMP_AD_STATUS (int event)
        {
            value = event;
        }
       
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_AD_STATUS valueOf(int value)
    	{
    		for (int i = 0; i < VO_OSMP_AD_STATUS.values().length; i ++)
    		{
    			if (VO_OSMP_AD_STATUS.values()[i].getValue() == value)
    				return VO_OSMP_AD_STATUS.values()[i];
    		}
    		voLog.e("VOCommonPlayerListener", "VO_OSMP_AD_STATUS isn't match. id = " + Integer.toHexString(value));
    		return VO_OSMP_AD_STATUS_MAX;
    	}
    }
    
    /**
     * Interface to process events. The client can implement the listener to receive/manage events. 
     *
     *
     * @param id     Event type. Refer to (@link VO_OSMP_CB_EVENT_ID}.
     * @param param1 First  parameter, specific to the event if needed.
     * @param param2 Second parameter, specific to the event if needed.
     * @param obj    Third  parameter, specific to the event if needed.
     * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    public VO_OSMP_RETURN_CODE onVOEvent(VO_OSMP_CB_EVENT_ID id, int param1, int param2, Object obj);
    
    
    /**
     * Interface to process synchronous events.  The client can implement the listener to receive/manage synchronous events.
     *
     * @param id     Event type. Refer to (@link VO_OSMP_CB_EVENT_ID}.
     * @param param1 First  parameter, specific to the event if needed.
     * @param param2 Second parameter, specific to the event if needed.
     * @param obj    Third  parameter, specific to the event if needed.
     * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    public VO_OSMP_RETURN_CODE onVOSyncEvent(VO_OSMP_CB_SYNC_EVENT_ID id, int param1, int param2, Object obj);
}
 
