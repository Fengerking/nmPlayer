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

import com.visualon.OSMPPlayer.VOCommonPlayerListener.*;
import com.visualon.OSMPUtils.voLog;

public class VOOSMPType {
   
	public static final String TAG = "@@@VOOSMPType";
   
    /**
    * Enumeration of return codes
    */
    public enum VO_OSMP_RETURN_CODE
    {
        /** No error */
        VO_OSMP_ERR_NONE                              (0X00000000),
        /** Reached the end of source */
        VO_OSMP_ERR_EOS                               (0X00000001),
        /** The buffer was empty, retry */
        VO_OSMP_ERR_RETRY                             (0X00000002),
        /** Format was changed */
        VO_OSMP_ERR_FORMAT_CHANGE                     (0X00000003),
        /** Audio is not currently available */
    	VO_OSMP_ERR_AUDIO_NO_NOW     				  (0X00000010),
    	/** Video is not currently available */
    	VO_OSMP_ERR_VIDEO_NO_NOW     				  (0X00000011),
    	/** Buffer needs to be flushed */
    	VO_OSMP_ERR_FLUSH_BUFFER     				  (0X00000012),
        /** Error from video codec */
        VO_OSMP_ERR_VIDEO                             (0X80000004),
        /** Error from audio codec */
        VO_OSMP_ERR_AUDIO                             (0X80000005),
        /** Out of memory */
        VO_OSMP_ERR_OUTMEMORY                         (0X80000006),
        /** The pointer is invalid */
        VO_OSMP_ERR_POINTER                           (0X80000007),
        /** The parameter id is not supported */
        VO_OSMP_ERR_PARAMID                           (0X80000008),
        /** The status is wrong */
        VO_OSMP_ERR_STATUS                            (0X80000009),
        /** The function is not implemented */
        VO_OSMP_ERR_IMPLEMENT                         (0X8000000A),
        /** The input buffer is too small */
        VO_OSMP_ERR_SMALLSIZE                         (0X8000000B),
        /** It is out of time (late). */
        VO_OSMP_ERR_OUT_OF_TIME                       (0X8000000C),
        /** Waiting reference time (early). */
        VO_OSMP_ERR_WAIT_TIME                         (0X8000000D),
        /** Unknown error */
        VO_OSMP_ERR_UNKNOWN                           (0X8000000E),
        /** JNI error. Not used for iOS platforms */
        VO_OSMP_ERR_JNI                               (0X8000000F),
        /** License check fail */
        VO_OSMP_ERR_LICENSE_FAIL                      (0X80000011),
        /** HTTPS Certification Authority error */
    	VO_OSMP_ERR_HTTPS_CA_FAIL					  (0X80000012),
    	/** Invalid argument */
    	VO_OSMP_ERR_ARGUMENT             			  (0X80000013),
    	/** Multiple player instances are not supported */
        VO_OSMP_ERR_MULTIPLE_INSTANCES_NOT_SUPPORTED  (0X80000014),
        /** Object not initialized */
        VO_OSMP_ERR_UNINITIALIZE                      (0X80000101),
       
        /** Open source failed */
        VO_OSMP_SRC_ERR_OPEN_SRC_FAIL                 (0X81000001),
        /** Content is encrypted, further operations required  */
        VO_OSMP_SRC_ERR_CONTENT_ENCRYPT               (0X81000002),
        /** Play mode not supported */
        VO_OSMP_SRC_ERR_PLAYMODE_UNSUPPORT            (0X81000003),
        /** Source file data error */
        VO_OSMP_SRC_ERR_ERROR_DATA                    (0X81000004),
        /** Seek failed/not supported */
        VO_OSMP_SRC_ERR_SEEK_FAIL                     (0X81000005),
        /** Source file format not supported */
        VO_OSMP_SRC_ERR_FORMAT_UNSUPPORT              (0X81000006),
        /** Track cannot be found */
        VO_OSMP_SRC_ERR_TRACK_NOTFOUND                (0X81000007),
        /** No download pointer set */
        VO_OSMP_SRC_ERR_NO_DOWNLOAD_OP                (0X81000008),
        /** No lib function pointer set */
        VO_OSMP_SRC_ERR_NO_LIB_OP                     (0X81000009),
        /** Output type not found */
        VO_OSMP_SRC_ERR_OUTPUT_NOTFOUND               (0X8100000A),
        /** Chunk must be dropped/skipped */
        VO_OSMP_SRC_ERR_CHUNK_SKIP                    (0X8100000B),
        /** Source object not initialized */
        VO_OSMP_SRC_ERR_SRC_UNINITIALIZE              (0X80001001),
       
        VO_OSMP_RETURN_CODE_MAX                       (0xFFFFFFFF);
       
        private int value;
        VO_OSMP_RETURN_CODE(int value)
        {
            this.value = value;
        }
       
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_RETURN_CODE valueOf(int value)
        {
        	for (int i = 0; i < VO_OSMP_RETURN_CODE.values().length; i ++)
    		{
    			if (VO_OSMP_RETURN_CODE.values()[i].getValue() == value)
    				return VO_OSMP_RETURN_CODE.values()[i];
    		}
    		
    		voLog.e(TAG, "VO_OSMP_RETURN_CODE isn't match. id = " + Integer.toHexString(value));
    		return VO_OSMP_RETURN_CODE_MAX;
        }
    }
   
   
    /**
     * Enumeration for render type
     * VO_OSMP_RENDER_TYPE
     */
    public enum VO_OSMP_RENDER_TYPE
    {
        /** Canvas render using rgb data */
        VO_OSMP_RENDER_TYPE_CANVAS_DATA               (0x00000000),
        /** Native window render */
        VO_OSMP_RENDER_TYPE_NATIVE_WINDOW             (0x00000001),
        /** Canvas render using bitmap handle */
        VO_OSMP_RENDER_TYPE_CANVAS_BITMAP             (0x00000002),
        /** Render using OpenGL ES. Not currently supported */
        VO_OSMP_RENDER_TYPE_OPENGLES                  (0x00000003),
        /** Native Surface render */
        VO_OSMP_RENDER_TYPE_NATIVE_SURFACE            (0x00000004),

        /** Render using Native HW. Not currently supported */
        VO_OSMP_RENDER_TYPE_HW_RENDER                 (0x00000005),
        
        /**Max value definition */
        VO_OSMP_RENDER_TYPE_MAX                       (0xFFFFFFFF);
       
        private int value;
        VO_OSMP_RENDER_TYPE(int value)
        {
            this.value = value;
        }
       
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_RENDER_TYPE valueOf(int value)
        {
        	for (int i = 0; i < VO_OSMP_RENDER_TYPE.values().length; i ++)
    		{
    			if (VO_OSMP_RENDER_TYPE.values()[i].getValue() == value)
    				return VO_OSMP_RENDER_TYPE.values()[i];
    		}
    		
    		voLog.e(TAG, "VO_OSMP_RENDER_TYPE isn't match. id = " + Integer.toHexString(value));
    		return VO_OSMP_RENDER_TYPE_MAX;
        }
    }

   
    /**
    * Enumeration of open source flags. Used in Open() function.
    * VO_OSMP_OPEN_SRC_FLAG
    *
    */
    public enum VO_OSMP_SRC_FLAG
    {
        /** Open source in synchronous mode.
         *  The open function will not return until the operation is complete.
         */
        VO_OSMP_FLAG_SRC_OPEN_SYNC                 (0X00000010),
       
         /** Open source in asynchronous mode.
         *  The open function will return immediately after the call.
         *  The completion of the open will be indicated by the {@link VO_OSMP_SRC_CB_OPEN_FINISHED} event.
         */
        VO_OSMP_FLAG_SRC_OPEN_ASYNC                (0X00000020),
        
        /**Max value definition */
        VO_OSMP_FLAG_SRC_MAX                       (0xFFFFFFFF);
       
        private int value;
       
        VO_OSMP_SRC_FLAG(int value)
        {
            this.value = value;
        }
           
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_SRC_FLAG valueOf(int value)
        {
        	for (int i = 0; i < VO_OSMP_SRC_FLAG.values().length; i ++)
    		{
    			if (VO_OSMP_SRC_FLAG.values()[i].getValue() == value)
    				return VO_OSMP_SRC_FLAG.values()[i];
    		}
    		
    		voLog.e(TAG, "VO_OSMP_SRC_FLAG isn't match. id = " + Integer.toHexString(value));
    		return VO_OSMP_FLAG_SRC_MAX;
        }
    }
   

    /**
     * Enumeration of color formats
     * VO_OSMP_COLORTYPE
     */
    public enum VO_OSMP_COLORTYPE
    {
        /** RGB packed mode, data: B:5 G:6 R:5. */
        VO_OSMP_COLOR_RGB565_PACKED                   (0x00000014),
        /** RGB packed mode, data: B G R A. */
        VO_OSMP_COLOR_RGB32_PACKED                    (0x00000017),
        /** ARGB packed mode, data: B G R A. */
        VO_OSMP_COLOR_ARGB32_PACKED                   (0x0000001A),
        
        /**Max value definition */
        VO_OSMP_COLOR_MAX                             (0xFFFFFFFF);
       
        private int value;
       
        VO_OSMP_COLORTYPE(int value)
        {
            this.value = value;
        }
           
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_COLORTYPE valueOf(int value)
        {
        	for (int i = 0; i < VO_OSMP_COLORTYPE.values().length; i ++)
    		{
    			if (VO_OSMP_COLORTYPE.values()[i].getValue() == value)
    				return VO_OSMP_COLORTYPE.values()[i];
    		}
    		
    		voLog.e(TAG, "VO_OSMP_COLORTYPE isn't match. id = " + Integer.toHexString(value));
    		return VO_OSMP_COLOR_MAX;
        }
    }
   
       
    /**
     * Enumeration of the status of the player
     * VO_OSMP_STATUS
     */
    public enum VO_OSMP_STATUS
    {
        /** Currently initializing */
        VO_OSMP_STATUS_INITIALIZING                   (0X00000000),
        /** Currently loading */
        VO_OSMP_STATUS_LOADING                        (0X00000001),
        /** Currently running */
        VO_OSMP_STATUS_PLAYING                        (0X00000002),
        /** Currently paused */
        VO_OSMP_STATUS_PAUSED                         (0X00000003),
        /** Currently stopped */
        VO_OSMP_STATUS_STOPPED                        (0X00000004),
        
        /**Max value definition */
        VO_OSMP_STATUS_MAX                            (0xFFFFFFFF);
      
       
        private int value;
       
        VO_OSMP_STATUS(int value)
        {
            this.value = value;
        }
           
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_STATUS valueOf(int value)
        {
        	for (int i = 0; i < VO_OSMP_STATUS.values().length; i ++)
    		{
    			if (VO_OSMP_STATUS.values()[i].getValue() == value)
    				return VO_OSMP_STATUS.values()[i];
    		}
    		
    		voLog.e(TAG, "VO_OSMP_STATUS isn't match. id = " + Integer.toHexString(value));
    		return VO_OSMP_STATUS_MAX;
        }
    }
   
   
    /**
     * Enumeration of zoom modes
     * VO_OSMP_ZOOM_MODE
     */
    public enum VO_OSMP_ZOOM_MODE
    {
        /** Letter box zoom mode*/
        VO_OSMP_ZOOM_LETTERBOX                          (0X00000001),
        /** Pan scan zoom mode */
        VO_OSMP_ZOOM_PANSCAN                            (0X00000002),
        /** Fit to window zoom mode */
        VO_OSMP_ZOOM_FITWINDOW                          (0X00000003),
        /** Original size zoom mode */
        VO_OSMP_ZOOM_ORIGINAL                           (0X00000004),
        /** Zoom in with the window zoom mode */
        VO_OSMP_ZOOM_ZOOMIN                             (0x00000005),
        
        /**Max value definition */
        VO_OSMP_ZOOM_MAX                                (0xFFFFFFFF);
       
        private int value;
       
        VO_OSMP_ZOOM_MODE(int value)
        {
            this.value = value;
        }
           
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_ZOOM_MODE valueOf(int value)
        {
        	for (int i = 0; i < VO_OSMP_ZOOM_MODE.values().length; i ++)
    		{
    			if (VO_OSMP_ZOOM_MODE.values()[i].getValue() == value)
    				return VO_OSMP_ZOOM_MODE.values()[i];
    		}
    		
    		voLog.e(TAG, "VO_OSMP_ZOOM_MODE isn't match. id = " + Integer.toHexString(value));
    		return VO_OSMP_ZOOM_MAX;
        }
    }
   


    /**
     * Enumeration of source formats.
     * VO_OSMP_SRC_SRC_FORMAT
     */
    public enum VO_OSMP_SRC_FORMAT
    {
        /** Detect the source format automatically (default).*/
        VO_OSMP_SRC_AUTO_DETECT                       (0X00000000),  
        /** Local MP4 file */
        VO_OSMP_SRC_FFLOCAL_MP4                       (0X00000001),  
        /** RTSP Streaming */
        VO_OSMP_SRC_FFSTREAMING_RTSP                  (0X08000100),  
        /** Session description */
        VO_OSMP_SRC_FFSTREAMING_SDP                   (0X08000200),  
        /** HTTP Live Streaming */
        VO_OSMP_SRC_FFSTREAMING_HLS                   (0X08000400),
        /** Silverlight Smooth Streaming */
        VO_OSMP_SRC_FFSTREAMING_SSSTR                 (0X08000800),   
        /** DASH Streaming */
        VO_OSMP_SRC_FFSTREAMING_DASH                  (0X08001000),
        /** PD Streaming */
        VO_OSMP_SRC_FFSTREAMING_HTTPPD                (0X08002000),
        
        /** Max value definition */
        VO_OSMP_SRC_FFMOVIE_MAX                       (0XFFFFFFFF);
       
        private int value;
       
        VO_OSMP_SRC_FORMAT(int value)
        {
            this.value = value;
        }
           
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_SRC_FORMAT valueOf(int value)
        {
        	for (int i = 0; i < VO_OSMP_SRC_FORMAT.values().length; i ++)
    		{
    			if (VO_OSMP_SRC_FORMAT.values()[i].getValue() == value)
    				return VO_OSMP_SRC_FORMAT.values()[i];
    		}
    		
    		voLog.e(TAG, "VO_OSMP_SRC_FORMAT isn't match. id = " + Integer.toHexString(value));
    		return VO_OSMP_SRC_FFMOVIE_MAX;
        }
    }
   

    /**
     * Enumeration of aspect ratios
     * VO_OSMP_ASPECT_RATIO
     *
     */
    public enum VO_OSMP_ASPECT_RATIO
    {
        /** Width and height ratio is original */
        VO_OSMP_RATIO_00                              (0X00000000),        
        /** Width and Height ratio is 1 : 1    */
        VO_OSMP_RATIO_11                              (0X00000001),        
        /** Width and Height ratio is 4 : 3    */
        VO_OSMP_RATIO_43                              (0X00000002),        
        /** Width and Height ratio is 16 : 9   */
        VO_OSMP_RATIO_169                             (0X00000003),        
        /** Width and Height ratio is 2 : 1    */
        VO_OSMP_RATIO_21                              (0X00000004),        
        /** Width and Height ratio is 2.33 : 1 */
        VO_OSMP_RATIO_2331                            (0X00000005),  
        /** Use aspect ratio value from video frame.
	         *  If frame does not contain this value, behavior is same as VO_OSMP_RATIO_ORIGINAL
	      */
        VO_OSMP_RATIO_AUTO                            (0X00000006), 
        /** Use video original ratio of yuv, width: height */
        VO_OSMP_RATIO_ORIGINAL                        (0X00000000), 
        /** Max value definition */
        VO_OSMP_RATIO_MAX                             (0xFFFFFFFF);
       
        private int value;
       
        VO_OSMP_ASPECT_RATIO(int value)
        {
            this.value = value;
        }
           
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_ASPECT_RATIO valueOf(int value)
        {
        	for (int i = 0; i < VO_OSMP_ASPECT_RATIO.values().length; i ++)
    		{
    			if (VO_OSMP_ASPECT_RATIO.values()[i].getValue() == value)
    				return VO_OSMP_ASPECT_RATIO.values()[i];
    		}
    		
    		voLog.e(TAG, "VO_OSMP_SRC_FORMAT isn't match. id = " + Integer.toHexString(value));
    		return VO_OSMP_RATIO_MAX;
        }
    }
   
    /**
     * Enumeration of player engine types
     * VO_OSMP_PLAYER_ENGINE
     *
     */
    public enum VO_OSMP_PLAYER_ENGINE
    {
        /** VisualOn media framework engine */
        VO_OSMP_VOME2_PLAYER                          (0X00000000),
        /** OpenMAX AL engine, for Android platforms only */
        VO_OSMP_OMXAL_PLAYER                          (0X00000001),
        /** AVPlayer engine, for iOS platforms only. Supports H.264 video decoder and AAC audio decoder  */
        VO_OSMP_AV_PLAYER                             (0X00000002),
        
        /** Max value definition */
        VO_OSMP_PLAYER_ENGINE_MAX                     (0xFFFFFFFF);
        
        private int value;
       
        VO_OSMP_PLAYER_ENGINE(int value)
        {
            this.value = value;
        }
           
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_PLAYER_ENGINE valueOf(int value)
        {
        	for (int i = 0; i < VO_OSMP_PLAYER_ENGINE.values().length; i ++)
    		{
    			if (VO_OSMP_PLAYER_ENGINE.values()[i].getValue() == value)
    				return VO_OSMP_PLAYER_ENGINE.values()[i];
    		}
    		
    		voLog.e(TAG, "VO_OSMP_PLAYER_ENGINE isn't match. id = " + Integer.toHexString(value));
    		return VO_OSMP_PLAYER_ENGINE_MAX;
        }
    }
   
    /**
     * Enumeration of program types
     * VO_OSMP_SRC_PROGRAM_TYPE
     *
     */
    public enum VO_OSMP_SRC_PROGRAM_TYPE
    {
        /**The source is live streaming */     
        VO_OSMP_SRC_PROGRAM_TYPE_LIVE       (0x00000000),
        /**The source is VOD*/
        VO_OSMP_SRC_PROGRAM_TYPE_VOD        (0x00000001),
        /**The source is unknown*/
        VO_OSMP_SRC_PROGRAM_TYPE_UNKNOWN    (0X000000FF),

        /** Max value definition */
        VO_OSMP_SRC_PROGRAM_TYPE_MAX        (0xFFFFFFFF);
       
        private int value;
       
        VO_OSMP_SRC_PROGRAM_TYPE(int value)
        {
            this.value = value;
        }
           
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_SRC_PROGRAM_TYPE valueOf(int value)
        {
        	for (int i = 0; i < VO_OSMP_SRC_PROGRAM_TYPE.values().length; i ++)
    		{
    			if (VO_OSMP_SRC_PROGRAM_TYPE.values()[i].getValue() == value)
    				return VO_OSMP_SRC_PROGRAM_TYPE.values()[i];
    		}
    		
    		voLog.e(TAG, "VO_OSMP_SRC_PROGRAM_TYPE isn't match. id = " + Integer.toHexString(value));
    		return VO_OSMP_SRC_PROGRAM_TYPE_MAX;
        }
    }
   
    /**
     * Enumeration of initParam flags, used in open() API
     * VO_OSMP_INIT_PARAM_FLAG
     */
    public enum VO_OSMP_INIT_PARAM_FLAG
    {
        /** Indicate initParam is NULL */
        VO_OSMP_FLAG_INIT_NOUSE                        (0X00000000),
        /** Set the actual file size */
        VO_OSMP_FLAG_INIT_ACTUAL_FILE_SIZE             (0X00000008),
        
        /** Max value definition */
        VO_OSMP_FLAG_INIT_MAX                          (0xFFFFFFFF);
       
        private int value;
       
        VO_OSMP_INIT_PARAM_FLAG(int value)
        {
            this.value = value;
        }
           
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_INIT_PARAM_FLAG valueOf(int value)
        {
        	for (int i = 0; i < VO_OSMP_INIT_PARAM_FLAG.values().length; i ++)
    		{
    			if (VO_OSMP_INIT_PARAM_FLAG.values()[i].getValue() == value)
    				return VO_OSMP_INIT_PARAM_FLAG.values()[i];
    		}
    		
    		voLog.e(TAG, "VO_OSMP_INIT_PARAM_FLAG isn't match. id = " + Integer.toHexString(value));
    		return VO_OSMP_FLAG_INIT_MAX;
        }
    }
   
 
     /**
     * Enumeration used to define RTSP connection type
     * VO_OSMP_RTSP_CONNECTION_TYPE
     */
    public enum VO_OSMP_RTSP_CONNECTION_TYPE
    {
        /** Set connection type to automatic. UDP will be attempted before TCP */
        VO_OSMP_RTSP_CONNECTION_AUTOMATIC             (0X00000000),
        /** Set connection type to TCP */
        VO_OSMP_RTSP_CONNECTION_TCP                   (0X00000001),
        /** Set connection type  to UDP (disable TCP) */
        VO_OSMP_RTSP_CONNECTION_UDP                   (0X00000002),
        /** Max value definition */
        VO_OSMP_RTSP_CONNECTION_MAX                   (0xFFFFFFFF);
       
        private int value;
       
        VO_OSMP_RTSP_CONNECTION_TYPE(int event)
        {
            value = event;
        }
       
        public int getValue()
        {
            return value;
        }
       
        public static VO_OSMP_RTSP_CONNECTION_TYPE valueOf(int value)
        {
        	for (int i = 0; i < VO_OSMP_RTSP_CONNECTION_TYPE.values().length; i ++)
    		{
    			if (VO_OSMP_RTSP_CONNECTION_TYPE.values()[i].getValue() == value)
    				return VO_OSMP_RTSP_CONNECTION_TYPE.values()[i];
    		}
    		
    		voLog.e(TAG, "VO_OSMP_RTSP_CONNECTION_TYPE isn't match. id = " + Integer.toHexString(value));
    		return VO_OSMP_RTSP_CONNECTION_MAX;
        }
       
    };
   
   
    /**
     * Enumeration of the verification flag
     * VO_OSMP_SRC_VERIFICATION_FLAG
     */
    public enum VO_OSMP_SRC_VERIFICATION_FLAG
    {
        /** The verification data is the request string */
        VO_OSMP_SRC_VERIFICATION_REQUEST_STRING       (0),
        /** The verification data is username:password */
        VO_OSMP_SRC_VERIFICATION_USERNAME_PASSWORD    (1),
       
        /** Max value definition */
        VO_OSMP_SRC_VERIFICATION_MAX                  (0XFFFFFFFF);
       
        private int value;
       
        VO_OSMP_SRC_VERIFICATION_FLAG(int event)
        {
            value = event;
        }
       
        public int getValue()
        {
            return value;
        }
       
        public static VO_OSMP_SRC_VERIFICATION_FLAG valueOf(int value)
        {
        	for (int i = 0; i < VO_OSMP_SRC_VERIFICATION_FLAG.values().length; i ++)
    		{
    			if (VO_OSMP_SRC_VERIFICATION_FLAG.values()[i].getValue() == value)
    				return VO_OSMP_SRC_VERIFICATION_FLAG.values()[i];
    		}
    		
    		voLog.e(TAG, "VO_OSMP_SRC_VERIFICATION_FLAG isn't match. id = " + Integer.toHexString(value));
    		return VO_OSMP_SRC_VERIFICATION_MAX;
        }
       
    };
    
    public enum VO_OSMP_SCREEN_BRIGHTNESS_MODE
    {
    	/**Indicate current screen brightness mode value is manual mode  */
    	VO_OSMP_SCREEN_BRIGHTNESS_MODE_MANUAL         (0x00000000),
    	/**Indicate current screen brightness mode value is automatic mode  */
    	VO_OSMP_SCREEN_BRIGHTNESS_MODE_AUTOMATIC      (0x00000001),
    	
    	/** Max value or unknown value definition */
    	VO_OSMP_SCREEN_BRIGHTNESS_MODE_MAX            (0XFFFFFFFF);
    	
        private int value;
        
        VO_OSMP_SCREEN_BRIGHTNESS_MODE(int value)
        {
            this.value = value;
        }
       
        public int getValue()
        {
            return value;
        }
        
        public static VO_OSMP_SCREEN_BRIGHTNESS_MODE valueOf(int value)
    	{
    		for (int i = 0; i < VO_OSMP_SCREEN_BRIGHTNESS_MODE.values().length; i ++)
    		{
    			if (VO_OSMP_SCREEN_BRIGHTNESS_MODE.values()[i].getValue() == value)
    				return VO_OSMP_SCREEN_BRIGHTNESS_MODE.values()[i];
    		}
    		
    		voLog.e(TAG, "VO_OSMP_SCREEN_BRIGHTNESS_MODE isn't match. id = " + Integer.toHexString(value));
    		return VO_OSMP_SCREEN_BRIGHTNESS_MODE_MAX;
    	}
    }
    
    public enum VO_OSMP_DECODER_TYPE
    {
        /** Invalid decoder type */
        VO_OSMP_DEC_NONE                              (0X00000000),

        /** Software video decoder */
        VO_OSMP_DEC_VIDEO_SW                          (0X00000001),
        /** IOMX video decoder */
        VO_OSMP_DEC_VIDEO_IOMX                        (0X00000002),
        /** MediaCodec video decoder */
        VO_OSMP_DEC_VIDEO_MEDIACODEC                  (0X00000004),
        /** Software audio decoder */
        VO_OSMP_DEC_AUDIO_SW                          (0X00100000),
        /** MediaCodec audio decoder */
        VO_OSMP_DEC_AUDIO_MEDIACODEC                  (0X00200000),

        /**Max value definition */
        VO_OSMP_DECODER_TYPE_MAX                      (0xFFFFFFFF);


        private int value;

        VO_OSMP_DECODER_TYPE(int value)
        {
            this.value = value;
        }

        public int getValue()
        {
            return value;
        }

        public static VO_OSMP_DECODER_TYPE valueOf(int value)
        {
                for (int i = 0; i < VO_OSMP_DECODER_TYPE.values().length; i ++)
                {
                        if (VO_OSMP_DECODER_TYPE.values()[i].getValue() == value)
                                return VO_OSMP_DECODER_TYPE.values()[i];
                }

                voLog.e(TAG, "VO_OSMP_DECODER_TYPE does not match. id = " + Integer.toHexString(value));
                return VO_OSMP_DEC_NONE;
        }
    }


    /**
     * Enumeration of the module type.
     * VO_OSMP_MODULE_TYPE
     */
    public enum VO_OSMP_MODULE_TYPE
    {
    	/** The whole OSMP+ SDK */
        VO_OSMP_MODULE_TYPE_SDK							(0x00000000),
        /** The version of DRM vendor A */
        VO_OSMP_MODULE_TYPE_DRM_VENDOR_A              	(0x00000001),
        
        /** Max value definition */
        VO_OSMP_MODULE_TYPE_MAX                			(0xFFFFFFFF);

        private int value;

        VO_OSMP_MODULE_TYPE(int value)
        {
            this.value = value;
        }

        public int getValue()
        {
            return value;
        }

        public static VO_OSMP_MODULE_TYPE valueOf(int value)
        {
                for (int i = 0; i < VO_OSMP_MODULE_TYPE.values().length; i ++)
                {
                        if (VO_OSMP_MODULE_TYPE.values()[i].getValue() == value)
                                return VO_OSMP_MODULE_TYPE.values()[i];
                }

                voLog.e(TAG, "VO_OSMP_MODULE_TYPE does not match. id = " + Integer.toHexString(value));
                return VO_OSMP_MODULE_TYPE_MAX;
        }
    }
    
	public enum VO_OSMP_SEI_INFO_FLAG
	{
		/**Indicate not to process any SEI info */
		VO_OSMP_SEI_INFO_NONE                   (0X00000000),      
		/**Indicate to process PIC Timing info, obj param refer to {@link VOOSMPSEIPicTiming}*/
		VO_OSMP_SEI_INFO_PIC_TIMING             (0X00000001),
		/**Indicate to process User data unregistered info, obj param refer to {@link VOOSMPSEIUserDataUnregistered}*/
        VO_OSMP_SEI_INFO_USER_DATA_UNREGISTERED (0X00000002),
		/**Max value definition */
		VO_OSMP_SEI_INFO_MAX                    (0X7FFFFFFF);
		
		private int value;
		
		VO_OSMP_SEI_INFO_FLAG(int event)
		{
			value = event;
		}
		
		public int getValue()
		{
			return value;
		}
		
		public static VO_OSMP_SEI_INFO_FLAG valueOf(int value)
        {
            for (int i = 0; i < VO_OSMP_SEI_INFO_FLAG.values().length; i ++)
            {
                    if (VO_OSMP_SEI_INFO_FLAG.values()[i].getValue() == value)
                            return VO_OSMP_SEI_INFO_FLAG.values()[i];
            }

            voLog.e(TAG, "VO_OSMP_SEI_INFO_FLAG does not match. id = " + Integer.toHexString(value));
            return VO_OSMP_SEI_INFO_MAX;
        }
		
	}
	
	
	public enum VO_OSMP_ADS_TRACKING_EVENT_TYPE
	{
		/**AD Manager action click */
		VO_OSMP_ADS_TRACKING_EVENT_CLICK                      (0X00000000),      
		/**AD Manager action playback start */
		VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKSTART              (0X00000001),
		/**AD Manager action playback complete */
		VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKCOMPLETE           (0X00000002),      
		/**AD Manager action pause */
		VO_OSMP_ADS_TRACKING_EVENT_PAUSE             	      (0X00000003),
		/**AD Manager action seek */
		VO_OSMP_ADS_TRACKING_EVENT_SEEKS                      (0X00000004),      
		/**AD Manager action dragstart */
		VO_OSMP_ADS_TRACKING_EVENT_DRAGSTART                  (0X00000005),
		/**AD Manager action resume */
		VO_OSMP_ADS_TRACKING_EVENT_RESUME                     (0X00000006),      
		/**AD Manager action forcestop */
		VO_OSMP_ADS_TRACKING_EVENT_FORCESTOP                  (0X00000007),
		/**The event is fired off when playing every stream over special percentage, fg: 10%, 25%, 50% */
		VO_OSMP_ADS_TRACKING_EVENT_PERCENTAGE                 (0X00001001),
		/**The event is fired off when playing whole content over special percentage, fg: 10%, 25%, 50% */
		VO_OSMP_ADS_TRACKING_EVENT_WHOLECONTENT_PERCENTAGE    (0X00001002),
		/**The event is fired off when playing passed time, fg: 15s, 120s, 180s */
		VO_OSMP_ADS_TRACKING_EVENT_TIMEPASSED                 (0X00001003),
		/**The Player Initialization Event is fired off when a player session is initialized  */
		VO_OSMP_ADS_TRACKING_EVENT_PLAYER_INITIALIZATION      (0x00001004),
		/** The event is fired off when full screen playing or quit fullscreen playing */
		VO_OSMP_ADS_TRACKING_EVENT_PLAYER_FULLSCREEN          (0x00001005),
		/** The event is fired off when full screen playing or quit fullscreen playing */
		VO_OSMP_ADS_TRACKING_EVENT_PLAYER_CLOSEDCAPTION       (0x00001006),
		/** The event is user click through action when playing */
		VO_OSMP_ADS_TRACKING_EVENT_CLICK_THROUGH		      (0x00001007),
		/**The event is fired off when playing whole content start */
		VO_OSMP_ADS_TRACKING_EVENT_WHOLECONTENT_START   	  (0x00001008),
		/**The event is fired off when playing whole content Finished */
		VO_OSMP_ADS_TRACKING_EVENT_WHOLECONTENT_END     	  (0x00001009),
		
		VO_OSMP_VMAP_TRACKING_EVENT_AD_IMPRESSION             (0x00002000),
		VO_OSMP_VMAP_TRACKING_EVENT_AD_START                  (0x00002001),
		VO_OSMP_VMAP_TRACKING_EVENT_AD_FIRST_QUARTILE         (0x00002002),
		VO_OSMP_VMAP_TRACKING_EVENT_AD_MID_POINT              (0x00002003),
		VO_OSMP_VMAP_TRACKING_EVENT_AD_THIRDQUARTILE          (0x00002004),
		VO_OSMP_VMAP_TRACKING_EVENT_AD_COMPLETE               (0x00002005),
		VO_OSMP_VMAP_TRACKING_EVENT_AD_CREATIVE_VIEW          (0x00002006),
		VO_OSMP_VMAP_TRACKING_EVENT_AD_MUTE                   (0x00002007),
		VO_OSMP_VMAP_TRACKING_EVENT_AD_UNMUTE                 (0x00002008),
		VO_OSMP_VMAP_TRACKING_EVENT_AD_PAUSE                  (0x00002009),
		VO_OSMP_VMAP_TRACKING_EVENT_AD_RESUME                 (0x0000200A),
		VO_OSMP_VMAP_TRACKING_EVENT_AD_FULL_SCREEN            (0x0000200B),
		
		/**Max value definition */
		VO_OSMP_ADS_TRACKING_EVENT_MAX                        (0X7FFFFFFF);
		
		private int value;
		
		VO_OSMP_ADS_TRACKING_EVENT_TYPE(int event)
		{
			value = event;
		}
		
		public int getValue()
		{
			return value;
		}
		
		public static VO_OSMP_ADS_TRACKING_EVENT_TYPE valueOf(int value)
        {
            for (int i = 0; i < VO_OSMP_ADS_TRACKING_EVENT_TYPE.values().length; i ++)
            {
                    if (VO_OSMP_ADS_TRACKING_EVENT_TYPE.values()[i].getValue() == value)
                            return VO_OSMP_ADS_TRACKING_EVENT_TYPE.values()[i];
            }

            return VO_OSMP_ADS_TRACKING_EVENT_MAX;
        }
	}

	public enum VO_OSMP_LAYOUT_TYPE
	{
		VO_OSMP_LAYOUT_PHONE         (0x00000000),
		VO_OSMP_LAYOUT_TABLET        (0x00000001);
		
		private int value;
		
		VO_OSMP_LAYOUT_TYPE(int event)
		{
			value = event;
		}
		
		public int getValue()
		{
			return value;
		}
	}
	
	/**Indicate it is debug mode, we shoule use debug server */
	public static final int VO_OSMP_ADS_OPENFLAG_DEBUG		= 0x80000000;
	
    /**
     * Enumeration of language type
     * VO_OSMP_LANGUAGE_TYPE
     */
    public enum VO_OSMP_LANGUAGE_TYPE {
        /** Chinese */
        VO_OSMP_LANGUAGE_CHI                      	(0x00000000),
        /** English */
        VO_OSMP_LANGUAGE_ENG                       	(0x00000001),
        /** French */
        VO_OSMP_LANGUAGE_FRA                       	(0x00000002),
        /** Swedish */
        VO_OSMP_LANGUAGE_SWE                       	(0x00000003),
        
        /** Max value definition */
        VO_OSMP_LANGUAGE_MAX  						(0xFFFFFFFF);
        
        private int value;
		
        VO_OSMP_LANGUAGE_TYPE(int event)
		{
			value = event;
		}
		
		public int getValue()
		{
			return value;
		}
        
		public static VO_OSMP_LANGUAGE_TYPE valueOf(int value)
        {
            for (int i = 0; i < VO_OSMP_LANGUAGE_TYPE.values().length; i ++)
            {
                    if (VO_OSMP_LANGUAGE_TYPE.values()[i].getValue() == value)
                            return VO_OSMP_LANGUAGE_TYPE.values()[i];
            }

            voLog.e(TAG, "VO_OSMP_LANGUAGE_TYPE does not match. id = " + Integer.toHexString(value));
            return VO_OSMP_LANGUAGE_MAX;
        }
    }
}
