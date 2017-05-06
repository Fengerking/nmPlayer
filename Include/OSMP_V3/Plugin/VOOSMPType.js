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
	
var voOSMPType = {
   
   /**
    * Enumeration of return codes.
    * VO_OSMP_RETURN_CODE
    */
    VO_OSMP_RETURN_CODE: {
        /** No error */
        VO_OSMP_ERR_NONE                             :     (0X00000000),
        /** Reached the end of source */
        VO_OSMP_ERR_EOS                              :     (0X00000001),
        /** Buffer was empty, retry */
        VO_OSMP_ERR_RETRY                            :     (0X00000002),
        /** Format was changed */
        VO_OSMP_ERR_FORMAT_CHANGE                    :     (0X00000003),
        /** Error from video codec */
        VO_OSMP_ERR_VIDEO                            :     (0X80000004),
        /** Error from audio codec */
        VO_OSMP_ERR_AUDIO                            :     (0X80000005),
        /** Out of memory */
        VO_OSMP_ERR_OUTMEMORY                        :     (0X80000006),
        /** Pointer is invalid */
        VO_OSMP_ERR_POINTER                          :     (0X80000007),
        /** Parameter id is not supported */
        VO_OSMP_ERR_PARAMID                          :     (0X80000008),
        /** Status is incorrect */
        VO_OSMP_ERR_STATUS                           :     (0X80000009),
        /** Function is not implemented */
        VO_OSMP_ERR_IMPLEMENT                        :     (0X8000000A),
        /** Input buffer is too small */
        VO_OSMP_ERR_SMALLSIZE                        :     (0X8000000B),
        /** It is out of time (late). */
        VO_OSMP_ERR_OUT_OF_TIME                      :     (0X8000000C),
        /** Waiting reference time (early). */
        VO_OSMP_ERR_WAIT_TIME                        :     (0X8000000D),
        /** Unknown error */
        VO_OSMP_ERR_UNKNOWN                          :     (0X8000000E),
        /** JNI error. Not used for iOS platforms */
        VO_OSMP_ERR_JNI                              :     (0X8000000F),
        /** License check fail */
        VO_OSMP_ERR_LICENSE_FAIL                     :     (0X80000011),
        /** HTTPS Certification Authority error */
        VO_OSMP_ERR_HTTPS_CA_FAIL                    :     (0X80000012),
        /** Invalid argument */
        VO_OSMP_ERR_ARGS                             :     (0X80000013),
        /** Multiple player instances are not supported */
        VO_OSMP_ERR_MULTIPLE_INSTANCES_NOT_SUPPORTED :     (0X80000014),
         /** Object not initialized */
        VO_OSMP_ERR_UNINITIALIZE                     :     (0X80000101),
       
        /** Open source failed */
        VO_OSMP_SRC_ERR_OPEN_SRC_FAIL                :     (0X81000001),
        /** Content is encrypted, further operations required  */
        VO_OSMP_SRC_ERR_CONTENT_ENCRYPT              :     (0X81000002),
        /** Play mode not supported */
        VO_OSMP_SRC_ERR_PLAYMODE_UNSUPPORT           :     (0X81000003),
        /** Source file data error */
        VO_OSMP_SRC_ERR_ERROR_DATA                   :     (0X81000004),
        /** Seek failed/not supported */
        VO_OSMP_SRC_ERR_SEEK_FAIL                    :     (0X81000005),
        /** Source file format not supported */
        VO_OSMP_SRC_ERR_FORMAT_UNSUPPORT             :     (0X81000006),
        /** Track cannot be found */
        VO_OSMP_SRC_ERR_TRACK_NOTFOUND               :     (0X81000007),
        /** No download pointer set */
        VO_OSMP_SRC_ERR_NO_DOWNLOAD_OP               :     (0X81000008),
        /** No lib function pointer set */
        VO_OSMP_SRC_ERR_NO_LIB_OP                    :     (0X81000009),
        /** Output type not found */
        VO_OSMP_SRC_ERR_OUTPUT_NOTFOUND              :     (0X8100000A),
        /** Chunk must be dropped/skipped */
        VO_OSMP_SRC_ERR_CHUNK_SKIP                   :     (0X8100000B),
        /** Source object not initialized */
        VO_OSMP_SRC_ERR_SRC_UNINITIALIZE             :     (0X80001001),
       
        VO_OSMP_RETURN_CODE_MAX                      :     (0XFFFFFFFF)
    },
   
   
    /**
     * Enumeration for render type.
     * VO_OSMP_RENDER_TYPE
     */
    VO_OSMP_RENDER_TYPE: {
        /** Canvas render using rgb data */
        VO_OSMP_RENDER_TYPE_CANVAS_DATA              :     (0X00000000),
        /** Canvas render using bitmap handle */
        VO_OSMP_RENDER_TYPE_CANVAS_BITMAP            :     (0X00000001),
        /** Native window render */
        VO_OSMP_RENDER_TYPE_NATIVE_WINDOW            :     (0X00000002),
        /** Native Surface render */
        VO_OSMP_RENDER_TYPE_NATIVE_SURFACE           :     (0X00000003),
        /** Render using OpenGL ES. Not currently supported */
        VO_OSMP_RENDER_TYPE_OPENGLES                 :     (0X00000004),
        /** Render using Native HW. Not currently supported */
        VO_OSMP_RENDER_TYPE_HW_RENDER                :     (0X00000005),
        
        /**Max value definition */
        VO_OSMP_RENDER_TYPE_MAX                      :     (0XFFFFFFFF)
    },

    /**
     * Enumeration of causes for HTTP failures.
     * VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON
     */
    VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON: {
        /** HTTP connect failed */
        VO_OSMP_HTTP_CONNECT_FAILED                  :     (0x00000000),
        
        /** HTTP failed to get response or response cannot be parsed or too large */
        VO_OSMP_HTTP_INVALID_RESPONSE                :     (0x00000001),

        /** HTTP 4xx error */
        VO_OSMP_HTTP_CLIENT_ERROR                    :     (0x00000002),

        /** HTTP 5xx error */
        VO_OSMP_HTTP_SERVER_ERROR                    :     (0x00000003),

        /** Max value */
        VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON_MAX     :     (0xFFFFFFFF)
    }, 

   
    /**
    * Enumeration of open source flags. Used in Open() function.
    * VO_OSMP_OPEN_SRC_FLAG
    *
    */
    VO_OSMP_SRC_FLAG: {
        /** Open source in synchronous mode.
         *  The open function will not return until the operation is complete.
         */
        VO_OSMP_FLAG_SRC_OPEN_SYNC                :     (0X00000010),
       
         /** Open source in asynchronous mode.
         *  The open function will return immediately after the call.
         *  The completion of the open will be indicated by the {@link VO_OSMP_SRC_CB_OPEN_FINISHED} event.
         */
        VO_OSMP_FLAG_SRC_OPEN_ASYNC               :     (0X00000020),
        
        /**Max value definition */
        VO_OSMP_FLAG_SRC_MAX                      :     (0XFFFFFFFF)
    },
   

    /**
     * Enumeration of color formats
     * VO_OSMP_COLORTYPE
     */
    VO_OSMP_COLORTYPE: {
        /** RGB packed mode, data: B:5 G:6 R:5. */
        VO_OSMP_COLOR_RGB565_PACKED                  :     (0X00000014),
        /** RGB packed mode, data: B G R A. */
        VO_OSMP_COLOR_RGB32_PACKED                   :     (0X00000017),
        /** ARGB packed mode, data: B G R A. */
        VO_OSMP_COLOR_ARGB32_PACKED                  :     (0X0000001A),
        
        /**Max value definition */
        VO_OSMP_COLOR_MAX                            :     (0XFFFFFFFF)
    },
   
       
    /**
     * Enumeration of the status of the player.
     * VO_OSMP_STATUS
     */
    VO_OSMP_STATUS: {
        /** Currently initializing */
        VO_OSMP_STATUS_INITIALIZING                  :     (0X00000000),
        /** Currently loading */
        VO_OSMP_STATUS_LOADING                       :     (0X00000001),
        /** Currently running */
        VO_OSMP_STATUS_PLAYING                       :     (0X00000002),
        /** Currently paused */
        VO_OSMP_STATUS_PAUSED                        :     (0X00000003),
        /** Currently stopped */
        VO_OSMP_STATUS_STOPPED                       :     (0X00000004),
        
        /**Max value definition */
        VO_OSMP_STATUS_MAX                           :     (0XFFFFFFFF)
    },
   
   
    /**
     * Enumeration of zoom modes.
     * VO_OSMP_ZOOM_MODE
     */
    VO_OSMP_ZOOM_MODE: {
        /** Letter box zoom mode*/
        VO_OSMP_ZOOM_LETTERBOX                         :     (0X00000001),
        /** Pan scan zoom mode */
        VO_OSMP_ZOOM_PANSCAN                           :     (0X00000002),
        /** Fit to window zoom mode */
        VO_OSMP_ZOOM_FITWINDOW                         :     (0X00000003),
        /** Original size zoom mode */
        VO_OSMP_ZOOM_ORIGINAL                          :     (0X00000004),
        /** Zoom in with the window zoom mode */
        VO_OSMP_ZOOM_ZOOMIN                            :     (0X00000005),
        
        /**Max value definition */
        VO_OSMP_ZOOM_MAX                               :     (0XFFFFFFFF)
    },
   


    /**
     * Enumeration of source formats.
     * VO_OSMP_SRC_SRC_FORMAT
     */
    VO_OSMP_SRC_FORMAT: {
        /** Detect the source format automatically (default). */
        VO_OSMP_SRC_AUTO_DETECT                      :     (0X00000000),  
        /** Local MP4 file */
        VO_OSMP_SRC_FFLOCAL_MP4                      :     (0X00000001),  
        /** RTSP Streaming */
        VO_OSMP_SRC_FFSTREAMING_RTSP                 :     (0X08000100),  
        /** Session description */
        VO_OSMP_SRC_FFSTREAMING_SDP                  :     (0X08000200),  
        /** HTTP Live Streaming */
        VO_OSMP_SRC_FFSTREAMING_HLS                  :     (0X08000400),
        /** Silverlight Smooth Streaming */
        VO_OSMP_SRC_FFSTREAMING_SSSTR                :     (0X08000800),   
        /** DASH Streaming */
        VO_OSMP_SRC_FFSTREAMING_DASH                 :     (0X08001000),
        /** PD Streaming */
        VO_OSMP_SRC_FFSTREAMING_HTTPPD               :     (0X08002000),
        
        /** Max value definition */
        VO_OSMP_SRC_FFMOVIE_MAX                      :     (0XFFFFFFFF)
    },
   

    /**
     * Enumeration of aspect ratios.
     * VO_OSMP_ASPECT_RATIO
     */
    VO_OSMP_ASPECT_RATIO: {
        /** Width and height ratio is original */
        VO_OSMP_RATIO_00                             :     (0X00000000),        
        /** Width and Height ratio is 1 : 1    */
        VO_OSMP_RATIO_11                             :     (0X00000001),        
        /** Width and Height ratio is 4 : 3    */
        VO_OSMP_RATIO_43                             :     (0X00000002),        
        /** Width and Height ratio is 16 : 9   */
        VO_OSMP_RATIO_169                            :     (0X00000003),        
        /** Width and Height ratio is 2 : 1    */
        VO_OSMP_RATIO_21                             :     (0X00000004),        
        /** Width and Height ratio is 2.33 : 1 */
        VO_OSMP_RATIO_2331                           :     (0X00000005),  
        
        /** Use aspect ratio value from video frame.
         *  If frame does not contain this value, behavior is same as VO_OSMP_RATIO_ORIGINAL
         */
        VO_OSMP_RATIO_AUTO                           :     (0X00000006),
        /** Use video original ratio of yuv, width: height */
        VO_OSMP_RATIO_ORIGINAL                       :     (0X00000000), 

        /** Max value definition */
        VO_OSMP_RATIO_MAX                            :     (0XFFFFFFFF)
    },
   
    /**
     * Enumeration of player engine types.
     * VO_OSMP_PLAYER_ENGINE
     */
    VO_OSMP_PLAYER_ENGINE: {
        /** VisualOn VOME2 engine */
        VO_OSMP_VOME2_PLAYER                         :     (0X00000000),
        /** OpenMAX AL engine, not used on iOS platforms. */
        VO_OSMP_OMXAL_PLAYER                         :     (0X00000001),
        /** AVPlayer on iOS platforms. Supports H.264 video decoder and AAC audio decoder, not used on Android platforms.  */
        VO_OSMP_AV_PLAYER                            :     (0X00000002),
        
        /** Max value definition */
        VO_OSMP_PLAYER_ENGINE_MAX                    :     (0XFFFFFFFF)
    },
   
    /**
     * Enumeration of program types.
     * VO_OSMP_SRC_PROGRAM_TYPE
     */
    VO_OSMP_SRC_PROGRAM_TYPE: {
        /**The source is live streaming */     
        VO_OSMP_SRC_PROGRAM_TYPE_LIVE      :     (0X00000000),
        /**The source is VOD*/
        VO_OSMP_SRC_PROGRAM_TYPE_VOD       :     (0X00000001),
        /**The source is unknown*/
        VO_OSMP_SRC_PROGRAM_TYPE_UNKNOWN   :     (0X000000FF),

        /** Max value definition */
        VO_OSMP_SRC_PROGRAM_TYPE_MAX       :     (0XFFFFFFFF)
    },
   
    /**
     * Enumeration of init paraminitParamParam flags, used in open() API
     * VO_OSMP_INIT_PARAM_FLAG.
     */
    VO_OSMP_INIT_PARAM_FLAG: {
        /** Indicate initParam is NULL */
        VO_OSMP_FLAG_INIT_NOUSE                       :     (0X00000000),
        /** Set the actual file size */
        VO_OSMP_FLAG_INIT_ACTUAL_FILE_SIZE            :     (0X00000008),
        
        /** Max value definition */
        VO_OSMP_FLAG_INIT_MAX                         :     (0XFFFFFFFF)
    },
   
 
    /**
     * Enumeration used to define RTSP connection type.
     * VO_OSMP_RTSP_CONNECTION_TYPE
     */
    VO_OSMP_RTSP_CONNECTION_TYPE: {
        /** Set connection type to automatic. UDP will be attempted before TCP */
        VO_OSMP_RTSP_CONNECTION_AUTOMATIC            :     (0X00000000),
        /** Set connection type to TCP */
        VO_OSMP_RTSP_CONNECTION_TCP                  :     (0X00000001),
        /** Set connection type  to UDP (disable TCP) */
        VO_OSMP_RTSP_CONNECTION_UDP                  :     (0X00000002),
        /** Max value definition */
        VO_OSMP_RTSP_CONNECTION_MAX                  :     (0XFFFFFFFF)
    },
   
   
    /**
     * Enumeration of the verification flag.
     * VO_OSMP_SRC_VERIFICATION_FLAG
     */
    VO_OSMP_SRC_VERIFICATION_FLAG: {
        /** Verification data is the request string */
        VO_OSMP_SRC_VERIFICATION_REQUEST_STRING      :	   (0),
        /** Verification data is username:password */
        VO_OSMP_SRC_VERIFICATION_USERNAME_PASSWORD   :	   (1),
       
        /** Max value definition */
        VO_OSMP_SRC_VERIFICATION_MAX                 :     (0XFFFFFFFF)
    },
    
    VO_OSMP_SCREEN_BRIGHTNESS_MODE: {
    
        /** Current screen brightness mode value is manual mode */
        VO_OSMP_SCREEN_BRIGHTNESS_MODE_MANUAL        :     (0X00000000),
        /** Current screen brightness mode value is automatic mode */
        VO_OSMP_SCREEN_BRIGHTNESS_MODE_AUTOMATIC     :     (0X00000001),
        
        /** Max value or unknown value definition */
        VO_OSMP_SCREEN_BRIGHTNESS_MODE_MAX           :     (0XFFFFFFFF)
    },


    VO_OSMP_CB_EVENT_ID: {
        /** Unknown error */
        VO_OSMP_CB_ERROR                                   : 0X8000000C,
        /** Source playback complete */
        VO_OSMP_CB_PLAY_COMPLETE                           : 0X00000001,
        /** Video stream started buffering */
        VO_OSMP_CB_VIDEO_START_BUFFER                      : 0X00000003,
        /** Video stream stopped buffering */
        VO_OSMP_CB_VIDEO_STOP_BUFFER                       : 0X00000004,
        /** Audio stream started buffering */
        VO_OSMP_CB_AUDIO_START_BUFFER                      : 0X00000005,
        /** Audio stream stopped buffering */
        VO_OSMP_CB_AUDIO_STOP_BUFFER                       : 0X00000006,
        /** Buffer time in source; param1 is (int) audio <ms> value, param2 is (int)video <ms> */
        VO_OSMP_CB_SRC_BUFFER_TIME                         : 0X00000007,
        /** Seek action complete */
        VO_OSMP_CB_SEEK_COMPLETE                           : 0X0000000D,
        /** Video aspect ratio changed; param1 is {@link VO_OSMP_ASPECT_RATIO} */
        VO_OSMP_CB_VIDEO_ASPECT_RATIO                      : 0X0000000E,
        /** Video size changed; param1 is video width (px), param2 is video height (px) */
        VO_OSMP_CB_VIDEO_SIZE_CHANGED                      : 0X0000000F,
        /** Codec not supported */
        VO_OSMP_CB_CODEC_NOT_SUPPORT                       : 0X80000010,
        /** Video codec deblock event; param1 is <0:disable|1:enable> */
        VO_OSMP_CB_DEBLOCK                                 : 0X00000011,
        /** Hardware decoder status available event; param1 is <0: error|1: available>. */
        VO_OSMP_CB_HW_DECODER_STATUS                       : 0X00000013,
        /** Authentication response information, e.g. report information from server;
         *  param obj is String: (detailed information). */
        VO_OSMP_CB_AUTHENTICATION_RESPONSE                 : 0X00000014,
        /** Subtitle language info is parsed and available */
        VO_OSMP_CB_LANGUAGE_INFO_AVAILABLE                 : 0X00000015,
        /** video render started */
        VO_OSMP_CB_VIDEO_RENDER_START                      : 0X00000016,
        /** Open Source complete */
        VO_OSMP_CB_OPEN_SRC_COMPLETE                       : 0X00000017,
	/** Notify SEI info, param1 is a {link @VO_OSMP_SEI_INFO_FLAG}, pObj is an object depending on param1 */
	VO_OSMP_CB_SEI_INFO                                : 0X00000019,
        /** This event will be issued only when PCM data output is enabled. pObj should be VOOSMPPCMBuffer */
        VO_OSMP_CB_PCM_OUTPUT                              : 0X0000001C,
        /** Notify license check fail */
        VO_OSMP_CB_LICENSE_FAIL                            : 0X8000001D,
        /** Bluetooth handset status; param1 is <0: bluetooth off|1: bluetooth on>.
         *  Not currently implemented on iOS. */
        VO_OSMP_CB_BLUETOOTHHANDSET_CONNECTION             : 0x00001011,
        /** Playback performance analytics info available; param obj is {@link VOOSMPAnalyticsInfo}. */
        VO_OSMP_CB_ANALYTICS_INFO                          : 0x00001021,
        /** Notify if the player is currently in full mode or not by checking param1: 1 for fullmode; 0 for normal mode */
        VO_OSMP_CB_FULLSCREEN_INDICATOR                    : 0x00001022,
        /**
         * Enumeration of source callback event IDs.
         */
        /** Source is connecting */
        VO_OSMP_SRC_CB_CONNECTING                          : 0X02000001,
        /** Source connection is finished */
        VO_OSMP_SRC_CB_CONNECTION_FINISHED                 : 0X02000002,
        /** Source connection timeout */
        VO_OSMP_SRC_CB_CONNECTION_TIMEOUT                  : 0X82000003,
        /** Source connection lost */
        VO_OSMP_SRC_CB_CONNECTION_LOSS                     : 0X82000004,
        /** HTTP download status, param 1 is int (0 - 100) */
        VO_OSMP_SRC_CB_DOWNLOAD_STATUS                     : 0X02000005,
        /** Source connection failed */
        VO_OSMP_SRC_CB_CONNECTION_FAIL                     : 0X82000006,
        /** Source download failed */
        VO_OSMP_SRC_CB_DOWNLOAD_FAIL                       : 0X82000007,
        /** DRM engine error */
        VO_OSMP_SRC_CB_DRM_FAIL                            : 0X82000008,
        /** Playlist parse error */
        VO_OSMP_SRC_CB_PLAYLIST_PARSE_ERR                  : 0X82000009,
        /** Maximum number of connections was reached. Currently used for RTSP only. */
        VO_OSMP_SRC_CB_CONNECTION_REJECTED                 : 0X8200000A,
        /** Bitrate is changed; param1 is new bitrate <bps> */
        VO_OSMP_SRC_CB_BA_HAPPENED                         : 0X0200000B,
        /** Device is rooted/jailbroken, DRM not secure. Not currently implemented on iOS.*/
        VO_OSMP_SRC_CB_DRM_NOT_SECURE                      : 0X0200000C,
        /** Device uses A/V output device but the license does not support it */
        VO_OSMP_SRC_CB_DRM_AV_OUT_FAIL                     : 0X8200000D,
        /** Download failed, and is waiting to recover */
        VO_OSMP_SRC_CB_DOWNLOAD_FAIL_WAITING_RECOVER       : 0X8200000E,
        /** Download recovery success */
        VO_OSMP_SRC_CB_DOWNLOAD_FAIL_RECOVER_SUCCESS       : 0X0200000F,
        /** Source open finished, param1 is {@link VO_OSMP_RETURN_CODE} */
        VO_OSMP_SRC_CB_OPEN_FINISHED                       : 0X02000010,
        /** Customer tag information available inside source; param1 is  {@link VO_OSMP_SRC_CUSTOMERTAGID},
         *  other params will depend on param1.
         */
        VO_OSMP_SRC_CB_CUSTOMER_TAG                        : 0X02000020,
        /** Streaming information; param1 is {@link VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT} and
         *  other params will depend on param1.
         */
        VO_OSMP_SRC_CB_ADAPTIVE_STREAMING_INFO             : 0X02000030,
        /** Adaptive streaming error, param1 is {@link VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT},
         *  other params will depend on param1.
         */
        VO_OSMP_SRC_CB_ADAPTIVE_STREAMING_ERROR            : 0X02000040,
        /** Adaptive streaming error warning, param1 is {@link VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT,
         *  other params will depend on param1.
         */
        VO_OSMP_SRC_CB_ADAPTIVE_STREAM_WARNING             : 0X02000050,
        /** RTSP error, param1 will be defined to {@link VO_OSMP_SRC_RTSP_ERROR} */
        VO_OSMP_SRC_CB_RTSP_ERROR                          : 0X02000060,
        /** Seek is complete, param1 is the seek return timestamp (ms),
         * param2 is the seek result return code,
         * {@link VO_OSMP_SRC_ERR_SEEK_FAIL} or {@link VO_OSMP_ERR_NONE}*/
        VO_OSMP_SRC_CB_SEEK_COMPLETE                       : 0X02000070,
        /** Program info has been changed in source */
        VO_OSMP_SRC_CB_PROGRAM_CHANGED                     : 0X02000071,
        /** Program info has been reset in source */
        VO_OSMP_SRC_CB_PROGRAM_RESET                       : 0x02000072,
        /** Seek to the last chunk of play list (NTS link without END tag need change to live mode) */
        VO_OSMP_SRC_CB_ADAPTIVE_STREAMING_SEEK2LASTCHUNK   : 0x02000073,
        /*!<Notify the current media position downloaded by the progressive download module, param1 is the position, int* */                           
        VO_OSMP_SRC_CB_PD_DOWNLOAD_POSITION                : 0X02000075, 
        /*!<Notify the current buffering percent of the progressive download module, param1 is the percent, int* */
        VO_OSMP_SRC_CB_PD_BUFFERING_PERCENT                : 0X02000076,
        /** Update source URL complete, param1 is {@link VO_OSMP_CB_EVENT_ID}
         *  param 1 will be event error ID(VO_OSMP_SRC_CB_CONNECTION_FAIL,VO_OSMP_SRC_CB_DOWNLOAD_FAIL,VO_OSMP_SRC_CB_DRM_FAIL,VO_OSMP_SRC_CB_PLAYLIST_PARSE_ERR)
         *  param 1 is equal to 0 if successfully
         */
        VO_OSMP_SRC_CB_UPDATE_URL_COMPLETE                 : 0X02000077,
        /** Need DRM information for verification, please use {@link VOCommonPlayerConfiguration#setDRMVerificationInfo} to set the verification info */
        VO_OSMP_SRC_CB_DRM_VERIFICATION                    : 0x20000000,
		
        VO_OSMP_CB_WINDOWED_UI_COMMAND                     : 0x01000001,

        /** Network is not currently available, but player still has some buffering data which can be played back */
        VO_OSMP_CB_NOT_APPLICABLE_MEDIA                    : 0x02000074,

        VO_OSMP_CB_EVENT_ID_MAX                            : 0xFFFFFFFF
    },

    /**
     * Enumeration of callback sync event IDs.
     * VO_OSMP_CB_SYNC_EVENT_ID
     */
    VO_OSMP_CB_SYNC_EVENT_ID: {
        /** Request DRM authentication information {@link VOCommonPlayerConfiguration#setDRMVerificationInfo:}
         *  Set authentication information when receiving this event
         **/
        VO_OSMP_SRC_CB_SYNC_AUTHENTICATION_DRM_SERVER_INFO  : 0X03000001,
        
        /** Start HTTP download. Parameter obj is the URL (NSString *) */
        VO_OSMP_SRC_CB_SYNC_IO_HTTP_START_DOWNLOAD          : 0X03000002,
        
        /** HTTP download failed. Parameter obj is {@link VOOSMPHTTPDownloadFailure} */
        VO_OSMP_SRC_CB_SYNC_IO_HTTP_DOWNLOAD_FAIL           : 0X03000003,
        
        /** Max value definition */
        VO_OSMP_CB_SYNC_EVENT_ID_MAX                        : 0XFFFFFFFF

    }, 
    
    
    /**
     * Enumeration of source customer tag IDs. Used in {@link VO_OSMP_SRC_CB_CUSTOMER_TAG} callback.
     * This enumeration is param1.
     */
    VO_OSMP_SRC_CUSTOMERTAGID: {
        /** Param2 is time stamp (ms), obj is NSData for TimedTag content.*/
        VO_OSMP_SRC_CUSTOMERTAGID_TIMEDTAG              : 0X00000001,
        
        /** Max value definition */
        VO_OSMP_SRC_CUSTOMERTAGID_MAX                   : 0xFFFFFFFF
    }, 
    
    
    /**
     * Enumeration of available stream info events.
     * Used in {@link VO_OSMP_SRC_CB_ADAPTIVE_STREAMING_INFO} callback.
     */
    VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT: {
        /** Bitrate changed. {@link VOCommonPlayerDelegate} callback param2 is new bitrate (bps). */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_BITRATE_CHANGE                   : 0x00000001,
        /** Media type changed. {@link VOCommonPlayerDelegate} callback param2 is {@link VO_OSMP_AVAILABLE_TRACK_TYPE}. */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_MEDIATYPE_CHANGE                 : 0x00000002,
        /** Program type. {@link VOCommonPlayerDelegate} callback param2 is{@link VO_OSMP_SRC_PROGRAM_TYPE}. */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PROGRAM_TYPE                     : 0x00000003,
        /** Chunk download begin. {@link VOCommonPlayerDelegate} callback param obj is {@link VOOSMPChunkInfo},
         *  If the return value of this callback is {@link VO_OSMP_SRC_ERR_CHUNK_SKIP}, source must drop this chunk. */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_BEGINDOWNLOAD              : 0x00000004,
        /** Chunk download dropped. {@link VOCommonPlayerDelegate} callback param obj will be {@link VOOSMPChunkInfo}. */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DROPPED                    : 0x00000005,
        /** Chunk download OK. {@link VOCommonPlayerDelegate} callback param obj will be {@link VOOSMPChunkInfo}. */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DOWNLOADOK                 : 0x00000006,
        /** Indicate Playlist download OK. */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PLAYLIST_DOWNLOADOK              : 0x00000007,
        /** Indicates that Program has been changed in source. When this event is received, you should get program info again. */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PROGRAM_CHANGE                   : 0x00000008,
        /** Indicates chunk is supported* */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_FILE_FORMATSUPPORTED             : 0x00000009,
        /** Indicates seeking-live supported* */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_LIVESEEKABLE			   : 0x0000000A,
        /** Max value definition */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFOEVENT_MAX                               : 0xFFFFFFFF
    },
    
    
    /**
     * Enumeration of available streaming error codes.
     * Used in {@link VO_OSMP_SRC_CB_ADAPTIVE_STREAMING_ERROR} callback
     */
     VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT: {
        /** Playlist parsing failed */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_PLAYLIST_PARSEFAIL              : 0x00000001,
        /** Playlist not supported */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_PLAYLIST_UNSUPPORTED            : 0x00000002,
        /** Stream not supported */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_UNSUPPORTED           : 0x00000003,
        /** Download failed */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_DOWNLOADFAIL          : 0x00000004,
        /** DRM error */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_DRMLICENSEERROR       : 0x00000005,
        
        /** Max value definition */
        VO_OSMP_SRC_ADAPTIVESTREAMING_ERROR_EVENT_MAX                              : 0xFFFFFFFF
    },
    
    /**
     * Enumeration of available streaming warning events.
     * Used in {@link VO_OSMP_SRC_CB_ADAPTIVE_STREAM_WARNING} callback.
     */
     VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT: {
        /** Chunk download failed */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_DOWNLOADERROR           : 0x00000001,
        /** Chunk format not supported */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_FILEFORMATUNSUPPORTED   : 0x00000002,
        /** DRM error */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_DRMERROR                : 0x00000003,
        /** Playlist download failed */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_PLAYLIST_DOWNLOADERROR        : 0x00000004,
        
        /** Max value definition */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_MAX                           : 0xFFFFFFFF
    },
    
    /**
     * Enumeration of available track types.
     * Used in {@link VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_MEDIATYPE_CHANGE} callback.
     *
     */
     VO_OSMP_AVAILABLE_TRACK_TYPE: {
        /** Only audio is available */
        VO_OSMP_AVAILABLE_PUREAUDIO                   : 0x00000000,
        /** Only video is available */
        VO_OSMP_AVAILABLE_PUREVIDEO                   : 0x00000001,
        /** Both audio and video are available */
        VO_OSMP_AVAILABLE_AUDIOVIDEO                  : 0x00000002,
        /** Max value definition    */
        VO_OSMP_AVAILABLE_MAX                         : 0xFFFFFFFF
    },
    
    /**
     * Enumeration of RTSP error.
     * VO_OSMP_SRC_RTSP_ERROR
     */
    VO_OSMP_SRC_RTSP_ERROR: {
        /** Connection failed in RTSP */
        VO_OSMP_SRC_RTSP_ERROR_CONNECT_FAIL           : 0x00000001,
        /** Describe failed in RTSP */
        VO_OSMP_SRC_RTSP_ERROR_DESCRIBE_FAIL          : 0x00000002,
        /** Setup failed in RTSP */
        VO_OSMP_SRC_RTSP_ERROR_SETUP_FAIL             : 0x00000003,
        /** Play failed in RTSP */
        VO_OSMP_SRC_RTSP_ERROR_PLAY_FAIL              : 0x00000004,
        /** Pause failed in RTSP */
        VO_OSMP_SRC_RTSP_ERROR_PAUSE_FAIL             : 0x00000005,
        /** Option failed in RTSP */
        VO_OSMP_SRC_RTSP_ERROR_OPTION_FAIL            : 0x00000006,
        /** Socket error in RTP channel of RTSP */
        VO_OSMP_SRC_RTSP_ERROR_SOCKET_ERROR           : 0x00000007,
        /** Max value definition    */
        VO_OSMP_SRC_RTSP_ERROR_MAX                    : 0xFFFFFFFF
    },
    
    VO_OSMP_UI_CMD: {
	/** Callback command ID */	
	/* param1 is the exact ctrl from fullscreen UI,  param2 is invalid */
	VOUI_CMD_PLAY		:	(0X01),
	
	/* param1 is the exact ctrl from fullscreen UI,  param2 is invalid */
	VOUI_CMD_PAUSE	:		(0X02),
	
	/* param1 is the exact ctrl from fullscreen UI,  param2 is invalid */
	VOUI_CMD_STOP		:	(0X03),	
	
	/* param1 is the exact ctrl from fullscreen UI, param2 is invalid */
	VOUI_CMD_FULLSCREEN	:	(0X04),
				
	/* param1 is the exact ctrl from fullscreen UI, param2 is the pos in msec */
	VOUI_CMD_SETPOS		:	(0X05),	
		    
	/* param1 is the exact ctrl from fullscreen UI, param2 is the volume value */
	VOUI_CMD_SETVOLUME	:	(0X06)
    }, 

    /**
     * Enumeration of SEI info
     * VO_OSMP_SEI_INFO_FLAG
     */
    VO_OSMP_SEI_INFO_FLAG: {
        /** Indicate not to process any SEI info */
        VO_OSMP_FLAG_SEI_NONE               : 0x00000000,
        
        /**Indicate to process PIC Timing info, obj param refer to {@link VOOSMPSEIPicTiming}*/
        VO_OSMP_FLAG_SEI_PIC_TIMING         : 0x00000001,

        /**Indicate to process User data unregistered info, obj param refer to {@link VOOSMPSEIUserDataUnregistered}*/
        VO_OSMP_SEI_INFO_USER_DATA_UNREGISTERED : 0X00000002,
        
        /** Max value definition */
        VO_OSMP_FLAG_SEI_MAX                : 0xFFFFFFFF
        
    },

    /**
     * Enumeration of callback event IDs.
     * VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID
     * <p>
     * This event is sent from sub-thread, please do not update UI or call other OSMP+ APIs in callback (except API in comments).
     * </p>
     */
    VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID: {
        /** Open complete */
        VO_OSMP_CB_STREAMING_DOWNLOADER_OPEN_COMPLETE            :  0X10000001,
        /** Manifest file download was successful */
        VO_OSMP_CB_STREAMING_DOWNLOADER_MANIFEST_OK              :  0X10000002,
        /** Entire content download completed */
        VO_OSMP_CB_STREAMING_DOWNLOADER_END                      :  0X10000004,
        /** Program information has changed */
        VO_OSMP_CB_STREAMING_DOWNLOADER_PROGRAM_INFO_CHANGE      :  0X10000005,
        /** Manifest file is updated, param1 is {@link VOOSMPStreamingDownloaderProgressInfo} */
        VO_OSMP_CB_STREAMING_DOWNLOADER_MANIFEST_UPDATE          :  0X10000006,

        /** Manifest file download has failed */
        VO_OSMP_CB_STREAMING_DOWNLOADER_DOWNLOAD_MANIFEST_FAIL   :  0X90000001,
        /** Manifest file write has failed */
        VO_OSMP_CB_STREAMING_DOWNLOADER_WRITE_MANIFEST_FAIL      :  0X90000002,
        /** Chunk file download has failed */
        VO_OSMP_CB_STREAMING_DOWNLOADER_DOWNLOAD_CHUNK_FAIL      :  0X90000003,
        /** Chunk file write has failed */
        VO_OSMP_CB_STREAMING_DOWNLOADER_WRITE_CHUNK_FAIL         :  0X90000004,
        /** Disk is full */
        VO_OSMP_CB_STREAMING_DOWNLOADER_DISK_FULL                :  0X90000005,
        /** Live stream is not supported */
        VO_OSMP_CB_STREAMING_DOWNLOADER_LIVE_STREAM_NOT_SUPPORT  :  0X90000007,
        /** Stream on local disk not supported */
        VO_OSMP_CB_STREAMING_DOWNLOADER_LOCAL_STREAM_NOT_SUPPORT :  0X90000008,

        /** Max value definition */
        VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID_MAX             :  0xFFFFFFFF
    }, 
    
    /**
     * Enumeration of the module type.
     * VO_OSMP_MODULE_TYPE
     */
    VO_OSMP_MODULE_TYPE: {
        /** The whole OSMP+ SDK */
        VO_OSMP_MODULE_TYPE_SDK                       : 0x00000000,
        /** The version of DRM vendor A */
        VO_OSMP_MODULE_TYPE_DRM_VENDOR_A              : 0x00000001,
       
        /** Max value definition */
        VO_OSMP_MODULE_TYPE_MAX                       : 0xFFFFFFFF
    }



    /**		
     *  VOOSMPChunkInfo:
     *	    type: The type of this chunk
     *		rootURL: The root URL of manifest. It must be filled by parser.
     *		URL: The URL of this chunk. It can be a relative URL.
     *	    startTime: The start offset time of this chunk. The units of ( startTime / timeScale * 1000 ) should be in ms
     *		duration: The duration of this chunk. The units of ( duration / timeScale * 1000 ) should be in ms.
     *		timeScale: The time scale of this chunk.
     */

    /**
     *  VOOSMPHTTPDownloadFailure:
     *      URL: Get URL of the HTTP download failure.
     *      reason: Get cause of the HTTP download failure.
     *      response: Get response of the HTTP download failure when the download failure cause is VO_OSMP_HTTP_CLIENT_ERROR or VO_OSMP_HTTP_SERVER_ERROR.
     */
 

}; // end of namespace voOSMPType
