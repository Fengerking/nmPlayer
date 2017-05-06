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

#ifndef __VO_OSMP_TYPE_H__
#define __VO_OSMP_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
    /**
     * Enumeration of return codes.
     */
    typedef enum {
        /** No error */
        VO_OSMP_ERR_NONE                              = 0X00000000,
        /** Reached the end of source */
        VO_OSMP_ERR_EOS                               = 0X00000001,
        /** The buffer was empty, retry */
        VO_OSMP_ERR_RETRY                             = 0X00000002,
        /** Format was changed */
        VO_OSMP_ERR_FORMAT_CHANGE                     = 0X00000003,
        /** Error from video codec */
        VO_OSMP_ERR_VIDEO                             = 0X80000004,
        /** Error from audio codec */
        VO_OSMP_ERR_AUDIO                             = 0X80000005,
        /** Out of memory */
        VO_OSMP_ERR_OUTMEMORY                         = 0X80000006,
        /** The pointer is invalid */
        VO_OSMP_ERR_POINTER                           = 0X80000007,
        /** The parameter id is not supported */
        VO_OSMP_ERR_PARAMID                           = 0X80000008,
        /** The status is wrong */
        VO_OSMP_ERR_STATUS                            = 0X80000009,
        /** The function is not implemented */
        VO_OSMP_ERR_IMPLEMENT                         = 0X8000000A,
        /** The input buffer is too small */
        VO_OSMP_ERR_SMALLSIZE                         = 0X8000000B,
        /** It is out of time (late) */
        VO_OSMP_ERR_OUT_OF_TIME                       = 0X8000000C,
        /** Waiting reference time (early) */
        VO_OSMP_ERR_WAIT_TIME                         = 0X8000000D,
        /** Unknown error */
        VO_OSMP_ERR_UNKNOWN                           = 0X8000000E,
        /** JNI error. Not used for iOS platforms. */
        VO_OSMP_ERR_JNI                               = 0X8000000F,
        /** License check fail */
        VO_OSMP_ERR_LICENSE_FAIL                      = 0X80000011,
        /** HTTPS Certification Authority error */
        VO_OSMP_ERR_HTTPS_CA_FAIL                     = 0X80000012,
        /** Invalid argument */
        VO_OSMP_ERR_ARGS                              = 0X80000013,
        /** Multiple player instances are not supported */
        VO_OSMP_ERR_MULTIPLE_INSTANCES_NOT_SUPPORTED  = 0X80000014,
        /** Object not initialized */
        VO_OSMP_ERR_UNINITIALIZE                      = 0X80000101,
        
        
        /** Open source failed */
        VO_OSMP_SRC_ERR_OPEN_SRC_FAIL                 = 0X81000001,
        /** Content is encrypted, further operations required */
        VO_OSMP_SRC_ERR_CONTENT_ENCRYPT               = 0X81000002,
        /** Play mode not supported */
        VO_OSMP_SRC_ERR_PLAYMODE_UNSUPPORT            = 0X81000003,
        /** Source file data error */
        VO_OSMP_SRC_ERR_ERROR_DATA                    = 0X81000004,
        /** Seek failed/not supported */
        VO_OSMP_SRC_ERR_SEEK_FAIL                     = 0X81000005,
        /** Source file format not supported */
        VO_OSMP_SRC_ERR_FORMAT_UNSUPPORT              = 0X81000006,
        /** Track cannot be found */
        VO_OSMP_SRC_ERR_TRACK_NOTFOUND                = 0X81000007,
        /** No download pointer set */
        VO_OSMP_SRC_ERR_NO_DOWNLOAD_OP                = 0X81000008,
        /** No lib function pointer set */
        VO_OSMP_SRC_ERR_NO_LIB_OP                     = 0X81000009,
        /** Output type not found */
        VO_OSMP_SRC_ERR_OUTPUT_NOTFOUND               = 0X8100000A,
        /** Chunk must be dropped/skipped */
        VO_OSMP_SRC_ERR_CHUNK_SKIP                    = 0X8100000B,
        /** Source object not initialized */
        VO_OSMP_SRC_ERR_SRC_UNINITIALIZE              = 0X80001001,
        
        /** Max value definition */
        VO_OSMP_RETURN_CODE_MAX                       = 0xFFFFFFFF,
    } VO_OSMP_RETURN_CODE;
    
    
    /**
     * Enumeration of initParam flag.
     * VO_OSMP_INIT_PARAM_FLAG
     *
     * @deprecated This Enumeration not be used when using {@link VOCommonPlayerControl#init:initParam:} and will be removed in the future.
     */
    typedef enum
    {
        /** Indicate initParam is NULL */
        VO_OSMP_FLAG_INIT_NOUSE                       = 0X00000000,
        /** Set the actual file size*/
        VO_OSMP_FLAG_INIT_ACTUAL_FILE_SIZE            = 0X00000008,
        
        /** Max value definition */
        VO_OSMP_FLAG_INIT_MAX                         = 0xFFFFFFFF,
        
    } VO_OSMP_INIT_PARAM_FLAG;
    
    
    /**
     * Enumeration of open source flags. Used in {@link VOCommonPlayerControl#open:flag:sourceType:initParam:initParamFlag:} function.
     * VO_OSMP_SRC_FLAG
     *
     */
    typedef enum {
        /** Open source in synchronous mode.
         *  The open function will not return until the operation is complete.
         */
        VO_OSMP_FLAG_SRC_OPEN_SYNC                    = 0X00000010,
        /** Open source in asynchronous mode.
         *  The open function will return immediately after the call.
         *  The completion of the open will be indicated by the {@link VO_OSMP_SRC_CB_OPEN_FINISHED} event.
         */
        VO_OSMP_FLAG_SRC_OPEN_ASYNC                   = 0X00000020,
        /** Max value definition */
        VO_OSMP_FLAG_SRC_MAX                          = 0xFFFFFFFF,
    } VO_OSMP_SRC_FLAG;
    
    
    /**
     * Enumeration of the status of the player.
     * VO_OSMP_STATUS
     */
    typedef enum {
        /** Currently initializing */
        VO_OSMP_STATUS_INITIALIZING                   = 0X00000000,
        /** Currently loading      */
        VO_OSMP_STATUS_LOADING                        = 0X00000001,
        /** Currently running      */
        VO_OSMP_STATUS_PLAYING                        = 0X00000002,
        /** Currently paused       */
        VO_OSMP_STATUS_PAUSED                         = 0X00000003,
        /** Currently stopped      */
        VO_OSMP_STATUS_STOPPED                        = 0X00000004,
        /** Max value definition   */
        VO_OSMP_STATUS_MAX                            = 0xFFFFFFFF,
    } VO_OSMP_STATUS;
    
    
    /**
     * Enumeration of zoom modes.
     * VO_OSMP_ZOOM_MODE
     */
    typedef enum {
        /** Letter box zoom mode    */
        VO_OSMP_ZOOM_LETTERBOX                        = 0X00000001,
        /** Pan scan zoom mode      */
        VO_OSMP_ZOOM_PANSCAN                          = 0X00000002,
        /** Fit to window zoom mode */
        VO_OSMP_ZOOM_FITWINDOW                        = 0X00000003,
        /** Original size zoom mode */
        VO_OSMP_ZOOM_ORIGINAL                         = 0X00000004,
        /** Zoom in with the window zoom mode */
        VO_OSMP_ZOOM_ZOOMIN                           = 0x00000005,
        /** Max value definition    */
        VO_OSMP_ZOOM_MAX                              = 0xFFFFFFFF,
    } VO_OSMP_ZOOM_MODE;
    
    
    /**
     * Enumeration of source formats.
     * VO_OSMP_SRC_SRC_FORMAT
     */
    typedef enum {
        /** Detect the source format automatically (default) */
        VO_OSMP_SRC_AUTO_DETECT                       = 0X00000000,
        /** Local MP4 file */
        VO_OSMP_SRC_FFLOCAL_MP4                       = 0X00000001,
        /** RTSP Streaming */
        VO_OSMP_SRC_FFSTREAMING_RTSP                  = 0X08000100,
        /** Session description */
        VO_OSMP_SRC_FFSTREAMING_SDP                   = 0X08000200,
        /** HTTP Live Streaming */
        VO_OSMP_SRC_FFSTREAMING_HLS                   = 0X08000400,
        /** Silverlight Smooth Streaming */
        VO_OSMP_SRC_FFSTREAMING_SSSTR                 = 0X08000800,
        /** DASH Streaming */
        VO_OSMP_SRC_FFSTREAMING_DASH                  = 0X08001000,
        /** PD Streaming */
        VO_OSMP_SRC_FFSTREAMING_HTTPPD                = 0X08002000,
        /** Max value definition */
        VO_OSMP_SRC_FFMOVIE_MAX                       = 0xFFFFFFFF,
    } VO_OSMP_SRC_FORMAT;
    
    
    /**
     * Enumeration of aspect ratios.
     * VO_OSMP_ASPECT_RATIO
     */
    typedef enum {
        /** @deprecated Same as VO_OSMP_RATIO_ORIGINAL and will be removed in the future */
        VO_OSMP_RATIO_00                              = 0X00000000,
        /** Width and Height ratio is 1 : 1    */
        VO_OSMP_RATIO_11                              = 0X00000001,
        /** Width and Height ratio is 4 : 3    */
        VO_OSMP_RATIO_43                              = 0X00000002,
        /** Width and Height ratio is 16 : 9   */
        VO_OSMP_RATIO_169                             = 0X00000003,
        /** Width and Height ratio is 2 : 1    */
        VO_OSMP_RATIO_21                              = 0X00000004,
        /** Width and Height ratio is 2.33 : 1 */
        VO_OSMP_RATIO_2331                            = 0X00000005,
        
        /** Use aspect ratio value from video frame.
         *  If frame does not contain this value, behavior is same as VO_OSMP_RATIO_ORIGINAL
         */
        VO_OSMP_RATIO_AUTO                            = 0X00000006,
        /** Use video original ratio of yuv, width: height */
        VO_OSMP_RATIO_ORIGINAL                        = 0X00000000,
        
        /** Max value definition */
        VO_OSMP_RATIO_MAX                             = 0xFFFFFFFF,
    } VO_OSMP_ASPECT_RATIO;
    
    
    /**
     * Enumeration of player engine types.
     * VO_OSMP_PLAYER_ENGINE
     */
    typedef enum {        
        /** VisualOn media framework engine */
        VO_OSMP_VOME2_PLAYER                          = 0X00000000,
        /** OpenMAX AL engine, for Android platforms only */
        VO_OSMP_OMXAL_PLAYER                          = 0X00000001,
        /** AVPlayer engine, for iOS platforms only. Supports H.264 video decoder and AAC audio decoder  */
        VO_OSMP_AV_PLAYER                             = 0X00000002,
        /** Max value definition */
        VO_OSMP_PLAYER_ENGINE_MAX                     = 0xFFFFFFFF,
    } VO_OSMP_PLAYER_ENGINE;
    
    
    /**
     * Enumeration of program types.
     * VO_OSMP_SRC_PROGRAM_TYPE
     */
    typedef enum {
        /** The source is live streaming */
        VO_OSMP_SRC_PROGRAM_TYPE_LIVE                 = 0X00000000,
        /** The source is VOD */
        VO_OSMP_SRC_PROGRAM_TYPE_VOD                  = 0X00000001,
        /** The source is unknown*/
        VO_OSMP_SRC_PROGRAM_TYPE_UNKNOWN              = 0X000000FF,
        /** Max value definition */
        VO_OSMP_SRC_PROGRAM_TYPE_MAX                  = 0xFFFFFFFF,
    } VO_OSMP_SRC_PROGRAM_TYPE;
    
    
    /**
     * Enumeration used to define RTSP connection type.
     * VO_OSMP_RTSP_CONNECTION_TYPE
     */
    typedef enum
    {
        /** Set connection type to automatic. UDP will be attempted before TCP */
        VO_OSMP_RTSP_CONNECTION_AUTOMATIC             = 0X00000000,
        /** Set connection type to TCP */
        VO_OSMP_RTSP_CONNECTION_TCP                   = 0X00000001,
        /** Set connection type to UDP (disable TCP) */
        VO_OSMP_RTSP_CONNECTION_UDP                   = 0X00000002,
        /** Max value definition */
        VO_OSMP_RTSP_CONNECTION_MAX                   = 0xFFFFFFFF,
    }VO_OSMP_RTSP_CONNECTION_TYPE;
    
    
    /**
     * Enumeration of the verification flag.
     * VO_OSMP_SRC_VERIFICATION_FLAG
     */
    typedef enum {
        /** The verification data is the request string */
        VO_OSMP_SRC_VERIFICATION_REQUEST_STRING       = 0X00000000,
        /** The verification data is username:password */
        VO_OSMP_SRC_VERIFICATION_USERNAME_PASSWORD    = 0X00000001,
        
        /** Max value definition */
        VO_OSMP_SRC_VERIFICATION_MAX                  = 0xFFFFFFFF,
    } VO_OSMP_SRC_VERIFICATION_FLAG;
    
    /**
     * Enumeration of the module type.
     * VO_OSMP_MODULE_TYPE
     */
    typedef enum {
        /** The whole OSMP+ SDK */
        VO_OSMP_MODULE_TYPE_SDK                       = 0x00000000,
        /** The version of DRM vendor A */
        VO_OSMP_MODULE_TYPE_DRM_VENDOR_A              = 0x00000001,
        
        /** Max value definition */
        VO_OSMP_MODULE_TYPE_MAX                       = 0xFFFFFFFF,
    } VO_OSMP_MODULE_TYPE;
    
    /**
     * Enumeration of SEI info
     * VO_OSMP_SEI_INFO_FLAG
     */
    typedef enum {
        /** Indicate not to process any SEI info */
        VO_OSMP_FLAG_SEI_NONE                       = 0x00000000,
        
        /**Indicate to process PIC Timing info, obj param refer to {@link VOOSMPSEIPicTiming}*/
        VO_OSMP_FLAG_SEI_PIC_TIMING                 = 0x00000001,
        
        /**Indicate to process User data unregistered info, obj param refer to {@link VOOSMPSEIUserDataUnregistered}*/
        VO_OSMP_SEI_INFO_USER_DATA_UNREGISTERED     = 0x00000002,
        
        /** Max value definition */
        VO_OSMP_FLAG_SEI_MAX  = 0xFFFFFFFF
        
    } VO_OSMP_SEI_INFO_FLAG;

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif // __VO_OSMP_TYPE_H__


