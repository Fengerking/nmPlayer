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

#import <Foundation/Foundation.h>
#import "VOOSMPType.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
    /**
     * Enumeration of callback event IDs.
     * VO_OSMP_CB_EVENT_ID
     */
    typedef enum {
        /** Unknown error */
        VO_OSMP_CB_ERROR                                   = 0X8000000C,
        /** Source playback complete */
        VO_OSMP_CB_PLAY_COMPLETE                           = 0X00000001,
        /** Video stream started buffering */
        VO_OSMP_CB_VIDEO_START_BUFFER                      = 0X00000003,
        /** Video stream stopped buffering */
        VO_OSMP_CB_VIDEO_STOP_BUFFER                       = 0X00000004,
        /** Audio stream started buffering */
        VO_OSMP_CB_AUDIO_START_BUFFER                      = 0X00000005,
        /** Audio stream stopped buffering */
        VO_OSMP_CB_AUDIO_STOP_BUFFER                       = 0X00000006,
        /** Buffer time in source; param1 is (int)audio <ms> value, param2 is (int)video <ms> */
        VO_OSMP_CB_SRC_BUFFER_TIME                         = 0X00000007,
        /** Seek action complete */
        VO_OSMP_CB_SEEK_COMPLETE                           = 0X0000000D,
        /** Video aspect ratio changed; param1 is {@link VO_OSMP_ASPECT_RATIO}.*/
        VO_OSMP_CB_VIDEO_ASPECT_RATIO                      = 0X0000000E,
        /** Video size changed; param1 is video width (px), param2 is video height (px). */
        VO_OSMP_CB_VIDEO_SIZE_CHANGED                      = 0X0000000F,
        /** Codec not supported */
        VO_OSMP_CB_CODEC_NOT_SUPPORT                       = 0X80000010,
        /** Video codec deblock event; param1 is <0:disable|1:enable>. */
        VO_OSMP_CB_DEBLOCK                                 = 0X00000011,
        /** Hardware decoder status available event; param1 is <0: error|1: available>. */
        VO_OSMP_CB_HW_DECODER_STATUS                       = 0X00000013,
        /** Authentication response information, e.g. report information from server;
         *  param obj is String: (detailed information). */
        VO_OSMP_CB_AUTHENTICATION_RESPONSE                 = 0X00000014,
        /** Subtitle language info is parsed and available */
        VO_OSMP_CB_LANGUAGE_INFO_AVAILABLE                 = 0X00000015,
        /** video render started */
		VO_OSMP_CB_VIDEO_RENDER_START                      = 0X00000016,
        /** Open Source complete */
        VO_OSMP_CB_OPEN_SRC_COMPLETE                       = 0X00000017,
        /** Notify SEI info, param1 is a {link @VO_OSMP_SEI_INFO_FLAG}, pObj is an object depending on param1 */
        VO_OSMP_CB_SEI_INFO                                = 0X00000019,
        /** Audio render failed */
        VO_OSMP_CB_AUDIO_RENDER_FAIL                       = 0X8000001A,
        /** This event will be issued only when PCM data output is enabled. pObj should be VOOSMPPCMBuffer */
        VO_OSMP_CB_PCM_OUTPUT                              = 0X0000001C,
        /** Notify license check fail */
        VO_OSMP_CB_LICENSE_FAIL                            = 0X8000001D,
        /** Bluetooth handset status; param1 is <0: bluetooth off|1: bluetooth on>.
         *  Not currently implemented on iOS.*/
        VO_OSMP_CB_BLUETOOTHHANDSET_CONNECTION             = 0x00001011,
        /** Playback performance analytics info available; param obj is {@link VOOSMPAnalyticsInfo}. */
        VO_OSMP_CB_ANALYTICS_INFO                          = 0x00001021,
        /**
         * Enumeration of source callback event IDs.
         */
        /** Source is connecting */
        VO_OSMP_SRC_CB_CONNECTING                          = 0X02000001,
        /** Source connection is finished */
        VO_OSMP_SRC_CB_CONNECTION_FINISHED                 = 0X02000002,
        /** Source connection timeout */
        VO_OSMP_SRC_CB_CONNECTION_TIMEOUT                  = 0X82000003,
        /** Source connection lost */
        VO_OSMP_SRC_CB_CONNECTION_LOSS                     = 0X82000004,
        /** HTTP download status, param 1 is int (0 - 100) */
        VO_OSMP_SRC_CB_DOWNLOAD_STATUS                     = 0X02000005,
        /** Source connection failed */
        VO_OSMP_SRC_CB_CONNECTION_FAIL                     = 0X82000006,
        /** Source download failed */
        VO_OSMP_SRC_CB_DOWNLOAD_FAIL                       = 0X82000007,
        /** DRM engine error */
        VO_OSMP_SRC_CB_DRM_FAIL                            = 0X82000008,
        /** Playlist parse error */
        VO_OSMP_SRC_CB_PLAYLIST_PARSE_ERR                  = 0X82000009,
        /** Maximum number of connections was reached. Currently used for RTSP only. */
        VO_OSMP_SRC_CB_CONNECTION_REJECTED                 = 0X8200000A,
        /** Bitrate is changed; param1 is new bitrate <bps> */
        VO_OSMP_SRC_CB_BA_HAPPENED                         = 0X0200000B,
        /** Device is rooted/jailbroken, DRM not secure. Not currently implemented on iOS */
        VO_OSMP_SRC_CB_DRM_NOT_SECURE                      = 0X0200000C,
        /** Device uses A/V output device but the license doesn't support it */
        VO_OSMP_SRC_CB_DRM_AV_OUT_FAIL                     = 0X8200000D,
        /** Download failed, and is waiting to recover */
        VO_OSMP_SRC_CB_DOWNLOAD_FAIL_WAITING_RECOVER       = 0X8200000E,
        /** Download recovery success */
        VO_OSMP_SRC_CB_DOWNLOAD_FAIL_RECOVER_SUCCESS       = 0X0200000F,
        /** Source open finished, param1 is {@link VO_OSMP_RETURN_CODE} */
        VO_OSMP_SRC_CB_OPEN_FINISHED                       = 0X02000010,
        /** Customer tag information available inside source; param1 is  {@link VO_OSMP_SRC_CUSTOMERTAGID},
         *  and other params will depend on param1
         */
        VO_OSMP_SRC_CB_CUSTOMER_TAG                        = 0X02000020,
        /** Streaming information; param1 is {@link VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT} and
         *  other params will depend on param1
         */
        VO_OSMP_SRC_CB_ADAPTIVE_STREAMING_INFO             = 0X02000030,
        /** Adaptive streaming error, param1 is {@link VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT} and
         *  other params will depend on param1
         */
        VO_OSMP_SRC_CB_ADAPTIVE_STREAMING_ERROR            = 0X02000040,
        /** Adaptive streaming error warning, param1 is {@link VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT} and
         *  other params will depend on param1
         */
        VO_OSMP_SRC_CB_ADAPTIVE_STREAM_WARNING             = 0X02000050,
        /** Notify RTSP error, param1 will be defined to {@link VO_OSMP_SRC_RTSP_ERROR} */
        VO_OSMP_SRC_CB_RTSP_ERROR                          = 0X02000060,
        /** Notify seek complete, param1 is the seek return timestamp (ms),
         * param2 is the seek result return code,
         * {@link VO_OSMP_SRC_ERR_SEEK_FAIL} or {@link VO_OSMP_ERR_NONE}*/
        VO_OSMP_SRC_CB_SEEK_COMPLETE                       = 0X02000070,
        /** Notify the program info has been changed in source */
        VO_OSMP_SRC_CB_PROGRAM_CHANGED                     = 0X02000071,
        /** Notify the program info has been reset in source */
        VO_OSMP_SRC_CB_PROGRAM_RESET                       = 0X02000072,
        /** Notify when seek to the last chunk of play list (NTS link without END tag need change to live mode) */
        VO_OSMP_SRC_CB_ADAPTIVE_STREAMING_SEEK2LASTCHUNK   = 0X02000073,
        /** Network is not available now, but it still has some buffering data which can be played back */
        VO_OSMP_SRC_CB_NOT_APPLICABLE_MEDIA                = 0X02000074,
        /** Notify the current media position downloaded by the progressive download module, param1 is the position */
        VO_OSMP_SRC_CB_PD_DOWNLOAD_POSITION                = 0X02000075,
        /** Notify the current buffering percent of the progressive download module, param1 is the percent */
        VO_OSMP_SRC_CB_PD_BUFFERING_PERCENT                = 0X02000076,
        /** Update source URL complete, param1 is {@link VO_OSMP_CB_EVENT_ID} */
        VO_OSMP_SRC_CB_UPDATE_URL_COMPLETE                 = 0X02000077,

        
        /** Notify once when the player starts displaying the first ad or content video */
        VO_OSMP_AD_CB_PLAYLIST_START                       = 0X03000001,
        /** Notify once when the player finishes displaying the last ad or content video */
        VO_OSMP_AD_CB_PLAYLIST_END                         = 0X03000002,
        /** Notify each time a content segment is started  */
        VO_OSMP_AD_CB_CONTENT_START                        = 0X03000003,
        /** Notify each time a content segment has ended */
        VO_OSMP_AD_CB_CONTENT_END                          = 0X03000004,
        /** Notify each time a video ad is started  */
        VO_OSMP_AD_CB_AD_START                             = 0X03000005,
        /** Notify each time a video ad has ended  */
        VO_OSMP_AD_CB_AD_END                               = 0X03000006,
        /** Notify current playing time, param1 is the playing time timestamp(ms) */
        VO_OSMP_AD_CB_VIDEO_PROGRESS                       = 0X03000007,
        /** Notify ad and content video hass ended */
        VO_OSMP_AD_CB_VIDEO_DONE                           = 0X03000008,
        /** Notify player's playback state changes, param1 is {@link VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT} */
        VO_OSMP_AD_CB_STATE_CHANGE                         = 0X03000009,
        /** Notify when an ad has no content or did not load as expected */
        VO_OSMP_AD_CB_NO_AD_CONTENT                        = 0X8300000A,
        /** Notify when the data for an ad request cannot be loaded */
        VO_OSMP_AD_CB_AD_LOAD_ERROR                        = 0X8300000B,
        /** Notify when the content video has expired or is unavailable */
        VO_OSMP_AD_CB_VIDEO_NOT_AVAILABLE                  = 0X8300000C,
        /** Notify when the content video experiences a fatal error while loading or during playback */
        VO_OSMP_AD_CB_DATA_LOAD_ERROR                      = 0X8300000D,
        
        /** Max value definition */
        VO_OSMP_CB_EVENT_ID_MAX                            = 0xFFFFFFFF
    } VO_OSMP_CB_EVENT_ID;
    
    
    /**
     * Enumeration of callback sync event IDs.
     * VO_OSMP_CB_SYNC_EVENT_ID
     * <p>
     * This event be sent from sub-thread, please don't update UI or call other visualon's api in callback (except API in comments).
     * </p>
     */
    typedef enum {
        /** Request DRM authentication information {@link VOCommonPlayerConfiguration#setDRMVerificationInfo:}
         *  Set authentication information when receiving this event
         **/
        VO_OSMP_SRC_CB_SYNC_AUTHENTICATION_DRM_SERVER_INFO  = 0X03000001,
        
        /** Start HTTP download. Parameter obj is the URL (NSString *)
         *  Can set HTTP information by API in VOCommonPlayerHTTPConfiguration.
         */
        VO_OSMP_SRC_CB_SYNC_IO_HTTP_START_DOWNLOAD          = 0X03000002,
        
        /** HTTP download failed. Parameter obj is {@link VOOSMPHTTPDownloadFailure}
         *  Can set HTTP information by API in VOCommonPlayerHTTPConfiguration.
         */
        VO_OSMP_SRC_CB_SYNC_IO_HTTP_DOWNLOAD_FAIL           = 0X03000003,
        
        /** Max value definition */
        VO_OSMP_CB_SYNC_EVENT_ID_MAX                        = 0XFFFFFFFF
        
    } VO_OSMP_CB_SYNC_EVENT_ID;

    
    /**
     * Enumeration of source customer tag IDs. Used in {@link VO_OSMP_SRC_CB_CUSTOMER_TAG} callback.
     * This enumeration is param1.
     */
    typedef enum {
        /** Param2 is time stamp (ms), obj is NSData for TimedTag content.*/
        VO_OSMP_SRC_CUSTOMERTAGID_TIMEDTAG              = 0X00000001,
        
        /** Max value definition */
        VO_OSMP_SRC_CUSTOMERTAGID_MAX                   = 0xFFFFFFFF
    } VO_OSMP_SRC_CUSTOMERTAGID;
    
    
    /**
     * Enumeration of available stream info events.
     * Used in {@link VO_OSMP_SRC_CB_ADAPTIVE_STREAMING_INFO} callback.
     */
    typedef enum {
        /** Bitrate changed. {@link VOCommonPlayerDelegate} callback param2 is new bitrate (bps). */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_BITRATE_CHANGE                   = 0x00000001,
        /** Media type changed. {@link VOCommonPlayerDelegate} callback param2 is {@link VO_OSMP_AVAILABLE_TRACK_TYPE}. */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_MEDIATYPE_CHANGE                 = 0x00000002,
        /** Program type. {@link VOCommonPlayerDelegate} callback param2 is{@link VO_OSMP_SRC_PROGRAM_TYPE}. */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PROGRAM_TYPE                     = 0x00000003,
        /** Chunk download begins. {@link VOCommonPlayerDelegate} callback param obj is {@link VOOSMPChunkInfo},
         *  If the return value of this callback is {@link VO_OSMP_SRC_ERR_CHUNK_SKIP}, source must drop this chunk. */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_BEGINDOWNLOAD              = 0x00000004,
        /** Chunk download dropped. {@link VOCommonPlayerDelegate} callback param obj will be {@link VOOSMPChunkInfo}. */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DROPPED                    = 0x00000005,
        /** Chunk download OK. {@link VOCommonPlayerDelegate} callback param obj will be {@link VOOSMPChunkInfo}. */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DOWNLOADOK                 = 0x00000006,
        /** Indicate Playlist download OK. */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PLAYLIST_DOWNLOADOK              = 0x00000007,
        /** Indicates that Program has been changed in source. When this event is received, you should get program info again. */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PROGRAM_CHANGE                   = 0x00000008,
        /** Indicates chunk is supported. */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_FILE_FORMATSUPPORTED             = 0x00000009,
        /** Indicates live clip can be seeked now. */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_LIVESEEKABLE                     = 0x0000000A,
        
        /** Max value definition */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_INFOEVENT_MAX                               = 0xFFFFFFFF
    } VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT;
    
    
    /**
     * Enumeration of available streaming error codes.
     * Used in {@link VO_OSMP_SRC_CB_ADAPTIVE_STREAMING_ERROR} callback
     */
    typedef enum {
        /** Playlist parsing failed */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_PLAYLIST_PARSEFAIL              = 0x00000001,
        /** Playlist not supported */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_PLAYLIST_UNSUPPORTED            = 0x00000002,
        /** Stream not supported */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_UNSUPPORTED           = 0x00000003,
        /** Download failed */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_DOWNLOADFAIL          = 0x00000004,
        /** DRM error */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_DRMLICENSEERROR       = 0x00000005,
        
        /** Max value definition */
        VO_OSMP_SRC_ADAPTIVESTREAMING_ERROR_EVENT_MAX                              = 0xFFFFFFFF
    } VO_OSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT;
    
    /**
     * Enumeration of available streaming warning events.
     * Used in {@link VO_OSMP_SRC_CB_ADAPTIVE_STREAM_WARNING} callback.
     */
    typedef enum {
        /** Chunk download failed */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_DOWNLOADERROR           = 0x00000001,
        /** Chunk format not supported */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_FILEFORMATUNSUPPORTED   = 0x00000002,
        /** DRM error */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_DRMERROR                = 0x00000003,
        /** Playlist download failed */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_PLAYLIST_DOWNLOADERROR        = 0x00000004,
        
        /** Max value definition */
        VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_MAX                           = 0xFFFFFFFF
    } VO_OSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT;
    
    /**
     * Enumeration of available track types.
     * Used in {@link VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_MEDIATYPE_CHANGE} callback.
     *
     */
    typedef enum {
        /** Only audio is available */
        VO_OSMP_AVAILABLE_PUREAUDIO                   = 0x00000000,
        /** Only video is available */
        VO_OSMP_AVAILABLE_PUREVIDEO                   = 0x00000001,
        /** Both audio and video are available */
        VO_OSMP_AVAILABLE_AUDIOVIDEO                  = 0x00000002,
        /** Max value definition    */
        VO_OSMP_AVAILABLE_MAX                         = 0xFFFFFFFF
    } VO_OSMP_AVAILABLE_TRACK_TYPE;
    
    /**
     * Enumeration of RTSP error
     */
    typedef enum
    {
        /** Notify connect failed in RTSP */
        VO_OSMP_SRC_RTSP_ERROR_CONNECT_FAIL           = 0x00000001,
        /** Notify describe failed in RTSP */
        VO_OSMP_SRC_RTSP_ERROR_DESCRIBE_FAIL          = 0x00000002,
        /** Notify setup failed in RTSP */
        VO_OSMP_SRC_RTSP_ERROR_SETUP_FAIL             = 0x00000003,
        /** Notify play failed in RTSP */
        VO_OSMP_SRC_RTSP_ERROR_PLAY_FAIL              = 0x00000004,
        /** Notify pause failed in RTSP */
        VO_OSMP_SRC_RTSP_ERROR_PAUSE_FAIL             = 0x00000005,
        /** Notify option failed in RTSP */
        VO_OSMP_SRC_RTSP_ERROR_OPTION_FAIL            = 0x00000006,
        /** Notify socket error in RTP channel of RTSP */
        VO_OSMP_SRC_RTSP_ERROR_SOCKET_ERROR           = 0x00000007,
        /** Max value definition    */
        VO_OSMP_SRC_RTSP_ERROR_MAX                    = 0xFFFFFFFF,
    } VO_OSMP_SRC_RTSP_ERROR;
    
    /**
     * Enumeration of player's playback status.
     * Used in {@link VO_OSMP_AD_CB_STATE_CHANGE} callback.
     */
    typedef enum {
        /** When a video is first loaded, player will broadcast this event  */
        VO_OSMP_AD_STATUS_UNSTARTED                 = -1,
        /** Video stopped */
        VO_OSMP_AD_STATUS_STOPPED                   = 0,
        /** Video is playing */
        VO_OSMP_AD_STATUS_PLAYING                   = 1,
        /** Video is paused */
        VO_OSMP_AD_STATUS_PAUSED                    = 2,
        /** Playback buffering */
        VO_OSMP_AD_STATUS_BUFFERING                 = 3,
        /** Domain blacklisted */
        VO_OSMP_AD_STATUS_DOMAIN_BLACKLISTED        = 100,
        /** Content video has expired or is unavailable */
        VO_OSMP_AD_STATUS_VIDEO_NOT_AVAILABLE       = 110,
        /** Content video experienced a fatal error while loading or during playback */
        VO_OSMP_AD_STATUS_DATA_LOAD_ERROR           = 130,
        /** Data for an ad request cannot be loaded */
        VO_OSMP_AD_STATUS_AD_LOAD_ERROR             = 140,
        
        /** Max value definition */
        VO_OSMP_AD_STATUS_MAX                       = 0xFFFFFFFF
    } VO_OSMP_AD_STATUS;


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


/**
 * Protocol for event process. The client who wants to receive event from Player shall implement the delegate.
 */
@protocol VOCommonPlayerDelegate <NSObject>

/**
 * Protocol to process events. The client can implement the listener to receive/manage events.
 *
 * @param nID    [out] Event type. Refer to {@link VO_OSMP_CB_EVENT_ID}.
 * @param param1 [out] First  parameter, specific to the event if needed.
 * @param param2 [out] Second parameter, specific to the event if needed.
 * @param obj    [out] Third  parameter, specific to the event if needed.
 *
 * @return {@link VO_OSMP_ERR_NONE} if successful.
 */
- (VO_OSMP_RETURN_CODE) onVOEvent:(VO_OSMP_CB_EVENT_ID)nID param1:(int)param1 param2:(int)param2 pObj:(void *)pObj;

/**
 * Protocol to process synchronous events.  The client can implement the listener to receive/manage synchronous events.
 *
 * @param id     Event type. Refer to (@link VO_OSMP_CB_EVENT_ID}.
 * @param param1 First  parameter, specific to the event if needed.
 * @param param2 Second parameter, specific to the event if needed.
 * @param obj    Third  parameter, specific to the event if needed.
 * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
 */
@optional
- (VO_OSMP_RETURN_CODE) onVOSyncEvent:(VO_OSMP_CB_SYNC_EVENT_ID)nID param1:(int)param1 param2:(int)param2 pObj:(void *)pObj;

@end

