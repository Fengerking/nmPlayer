/************************************************************************
 VisualOn Proprietary
 Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved
 
VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA
 
All data and information contained in or disclosed by this document are
 confidential and proprietary information of VisualOn, and all rights
 therein are expressly reserved. By accepting this material, the
 recipient agrees that this material and the information contained
 therein are held in confidence and in trust. The material may only be
 used and/or disclosed as authorized in a license agreement controlling
 such use and disclosure.
 ************************************************************************/

/************************************************************************
 * @file voOSType.java
 * define all global IDs.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

public class voOSType {
	
	private static final String TAG = "@@@voOSType";
	/**
	 * Enumeration for select media framework type, used in Init() API
	 */
	
	/**Indicate using VOME2 media framework */
	public static final int VOOSMP_VOME2_PLAYER 			= 0x00000000;	
	/**Indicate using OpenMAX AL media framework*/
	public static final int VOOSMP_OMXAL_PLAYER				= 0x00000001;	
	
	
	/**
	 * Defination of return error code
	 */
	
	/**No Error */
	public static final int VOOSMP_ERR_None					= 0X00000000;
	/**Reach the end of source */
	public static final int VOOSMP_ERR_EOS					= 0X00000001;
	/**The buffer was empty, try it again */
	public static final int VOOSMP_ERR_Retry				= 0X00000002;	
	/**Format was changed */
	public static final int VOOSMP_ERR_FormatChange			= 0X00000003;
	/** Audio is not currently available */
	public static final int VOOSMP_ERR_Audio_No_Now     	= 0X00000010;
	/** Video is not currently available */
	public static final int VOOSMP_ERR_Video_No_Now     	= 0X00000011;
	/** Buffer needs to be flushed */
	public static final int VOOSMP_ERR_FLush_Buffer     	= 0X00000012;
	/**Error from video codec */
	public static final int VOOSMP_ERR_Video				= 0X80000004;
	/**Error from audio codec */
	public static final int VOOSMP_ERR_Audio				= 0X80000005;
	/**Error is out of memory */
	public static final int VOOSMP_ERR_OutMemory			= 0X80000006;
	/**The pointer is invalid */
	public static final int VOOSMP_ERR_Pointer				= 0X80000007;
	/**Parameter ID is not supported or parameter value is invalid*/
	public static final int VOOSMP_ERR_ParamID				= 0X80000008;	
	/**The status is wrong */
	public static final int VOOSMP_ERR_Status				= 0X80000009;
	/**This function is not implemented */
	public static final int VOOSMP_ERR_Implement			= 0X8000000A;
	/**The input buffer was too small */
	public static final int VOOSMP_ERR_SmallSize			= 0X8000000B;
	/**It is out of time (late)  */
	public static final int VOOSMP_ERR_OutOfTime			= 0X8000000C;
	/**It is waiting reference time (elear)  */
	public static final int VOOSMP_ERR_WaitTime				= 0X8000000D;
	/**Unknow error*/
	public static final int VOOSMP_ERR_Unknown				= 0X8000000E;
	/**JNI error*/
	public static final int VOOSMP_ERR_JNI					= 0X8000000F;
	/**Seek failed*/
	public static final int VOOSMP_ERR_SeekFailed           = 0X80000010;
	/**License check failed */
	public static final int VOOSMP_ERR_LicenseFailed		= 0X80000011;
	/** HTTPS Certification Authority error */
	public static final int VOOSMP_ERR_HTTPS_CA_FAIL		= 0X80000012;
	/** Invalid argument */
	public static final int VOOSMP_ERR_Args             	= 0X80000013;
	/** Multiple player instances are not supported */
	public static final int VOOSMP_ERR_MultipleInstancesNotSupport 	= 0X80000014;
	
	/**Object not initialize*/
	public static final int VOOSMP_ERR_Uninitialize			= 0X80000101;

	
	/**
	 * Definition for call back ID
	 */
	
	/**Play source was finished */
	public static final int VOOSMP_CB_PlayComplete			= 0X00000001;
	/**The buffer status. the param1 is (0 - 100)  */
	public static final int VOOSMP_CB_BufferStatus			= 0X00000002;
	/**The video stream start buffering  */
	public static final int VOOSMP_CB_VideoStartBuff		= 0X00000003;
	/**The video stream stop buffering   */
	public static final int VOOSMP_CB_VideoStopBuff			= 0X00000004;
	/**The audio stream start buffering  */
	public static final int VOOSMP_CB_AudioStartBuff		= 0X00000005;
	/**The audio stream stop buffering  */
	public static final int VOOSMP_CB_AudioStopBuff			= 0X00000006;
	/**The buffer time in source  param1 is audio value, param2 is video value.*/
	public static final int VOOSMP_CB_SourceBuffTime		= 0X00000007;
	/**
	 * Video delay time. param1 is ms value
	 * @deprecated this API remove for task 27762.
	 */
	public static final int VOOSMP_CB_VideoDelayTime		= 0X00000008;
	/**
	 * Video last render time, param1 is ms value
	 * @deprecated this API remove for task 27762.
	 */
	public static final int VOOSMP_CB_VideoLastTime			= 0X00000009;
	/**
	 * Video dropped frames. param1 is value
	 * @deprecated this API remove for task 27762.
	 */
	public static final int VOOSMP_CB_VideoDropFrames		= 0X0000000A;
	/**Video playback frame rate last 5 seconds, param1 is value  */
	public static final int VOOSMP_CB_VideoFrameRate		= 0X0000000B;
	/**Unknown error.  */
	public static final int VOOSMP_CB_Error					= 0X8000000C;
	/**Seek was finished. param 1 should be the seek return timestamp long long* , param 2 should be the pointor of VOOSMP_ERR_XXX value, VOOSMP_ERR_SeekFailed or VOOSMP_ERR_None*/
	public static final int VOOSMP_CB_SeekComplete			= 0X0000000D;
	/**Video aspect ratio changed, param1 value refer to
	 * <ul>
     * <li>{@link #VOOSMP_RATIO_00}
     * <li>{@link #VOOSMP_RATIO_11}
     * <li>{@link #VOOSMP_RATIO_43}
     * <li>{@link #VOOSMP_RATIO_169}
     * <li>{@link #VOOSMP_RATIO_21}
     * <li>{@link #VOOSMP_RATIO_2331}
	 * </ul>
	 * 
	 * If param1 do not match above value, then parameter obj is Float type, value is width/height.
	 * */
	public static final int VOOSMP_CB_VideoAspectRatio		= 0X0000000E;	
	/**Video size changed, param1 is video width, param2 is video height. */
	public static final int VOOSMP_CB_VideoSizeChanged		= 0X0000000F;
	/**Can not support the codec */
	public static final int VOOSMP_CB_CodecNotSupport		= 0X80000010;
	/**Video codec deblock event, param1 is 0:disable, 1:enable */
	public static final int VOOSMP_CB_Deblock				= 0X00000011;
	/**Notify if video/audio is available, param1 value refer to 
	 * <ul>
     * <li>{@link #VOOSMP_AVAILABLE_PUREAUDIO}
     * <li>{@link #VOOSMP_AVAILABLE_PUREVIDEO}
     * <li>{@link #VOOSMP_AVAILABLE_AUDIOVIDEO}
	 * </ul>
	 */
	public static final int VOOSMP_CB_MediaTypeChanged		= 0X00000012;  
	/**Notify hardware decoder available event, param1 is 0: error, 1: available */
	public static final int VOOSMP_CB_HWDecoderStatus		= 0X00000013;  
	/**Notify authentication response information,for instance report the information from server. Third parameter obj is String: detail info */
	public static final int VOOSMP_CB_AUTHENTICATION_RESPONSE		= 0X00000014; 
	/**Notify subtile language info is parsed */
	public static final int VOOSMP_CB_LanguageInfoAvailable = 0X00000015;
	/**Notify video render starts*/
	public static final int VOOSMP_CB_VideoRenderStart      = 0X00000016;
	/**Notify Open Source completely*/
	public static final int VOOSMP_CB_OpenSource            = 0X00000017;
	/**Meta data found,pass the type of metadata, param1 is Integer, value refer to {@link VOOSMP_SOURCE_STREAMTYPE}*/
	public static final int VOOSMP_CB_Metadata_Arrive       = 0X00000018;
	/**Notify SEI info, param1 is SEI type, value refer to {@link VOOSMP_SEI_EVENT_FLAG}, other param depend on param1*/
	public static final int VOOSMP_CB_SEI_INFO              = 0X00000019;
	/** Notify audio render initialization failed */
	public static final int VOOSMP_CB_Audio_Render_Failed   = 0X8000001A;
	/** This event will be issued only when PCM data output is enabled. Parameter 1 should be VOOSMP_PCMBUFFER */
	public static final int VOOSMP_CB_PCM_OUTPUT            = 0X0000001C;
	/** Notify license check failed */
	public static final int VOOSMP_CB_LicenseFailed         = 0X8000001D;  

	
	/**Callback Bluetooth handset status, param1 is 0: bluetooth off , 1: bluetooth on */
	public static final int VOOSMP_CB_BLUETOOTHHANDSET		= 0x00001011;
	/**Close caption data arrived.  param3: Object param refer to {@link com.visualon.OSMPSubTitle.voSubTitleManager.voSubtitleInfo} class */
	public static final int VOOSMP_CB_ClosedCaptionData		= 0X00002001;
	
	/**
	 * Enumeration for render type
	 * VOOSMP_RENDER_TYPE
	 */
	/**Indicate using Java render */
	public static final int VOOSMP_RENDER_TYPE_JAVA			= 0x00000000;
	/**Indicate using native window render*/
	public static final int VOOSMP_RENDER_TYPE_NATIVE		= 0x00000001;
	/**Indicate using bitmap render*/
	public static final int VOOSMP_RENDER_TYPE_BITMAP		= 0x00000002;
	/**Indicate using OpenGL ES*/
	public static final int VOOSMP_RENDER_TYPE_OPENGLES		= 0x00000003;
	/**Indicate using native C render*/
	public static final int VOOSMP_RENDER_TYPE_NATIVE_C     = 0x00000004;
	/**Indicate using native HW render*/
	public static final int VOOSMP_RENDER_TYPE_HW_RENDER	= 0x00000005;	
	/**Indicate using Java MediaCodec render*/
	public static final int VOOSMP_RENDER_TYPE_JMHW_RENDER	= 0x00000006;	
	/**Indicate using OpenMAX AL media framework and render*/
	public static final int VOOSMP_RENDER_TYPE_OpenMAXAL	= 0x00000009;
	
	
	/**
	 * Defination parameter ID
	 */
	/**Set the audio volume, param is int type, value is 0 - 100, 0 is mute and 100 is max*/
	public static final int VOOSMP_PID_AUDIO_VOLUME			= 0X00000001;	
	/**Set the draw area, param is {@link voOSRect} It will support next build. */
	public static final int VOOSMP_PID_DRAW_RECT			= 0X00000002;	
	/**Set the draw color type, param is int type, value is refer to 
	 * <ul>
     * <li>{@link #VOOSMP_COLOR_RGB565_PACKED}
     * <li>{@link #VOOSMP_COLOR_RGB32_PACKED}
     * <li>{@link #VOOSMP_COLOR_ARGB32_PACKED}
	 * </ul>
	 */
	public static final int VOOSMP_PID_DRAW_COLOR				= 0X00000003;
	/**Set Lib func set,  param is int type, value is VOOSMP_LIB_FUNC's pointer, It will support next build. */	
	public static final int VOOSMP_PID_FUNC_LIB					= 0X00000004;
	/**Set data source max buffer time,  Param is int type value. */
	public static final int VOOSMP_PID_MAX_BUFFTIME				= 0X00000005;
	/**Set data source min buffer time,  Param is int type value.  */
	public static final int VOOSMP_PID_MIN_BUFFTIME				= 0X00000006;
	/**Set Redraw the video in render .  */
	public static final int VOOSMP_PID_VIDEO_REDRAW				= 0X00000007;
	/**Set audio effect enable, Param is int type, value >0: enable, 0: disable .*/
	public static final int VOOSMP_PID_AUDIO_EFFECT_ENABLE		= 0X00000008;
	/**Set check license text, Param is String type.   */
	public static final int VOOSMP_PID_LICENSE_TEXT				= 0X00000009;
	/**Set license file path, Param is String type .*/
	public static final int VOOSMP_PID_LICENSE_FILE_PATH		= 0X0000000A;
	/**Set player work path. Param is String type   */
	public static final int VOOSMP_PID_PLAYER_PATH				= 0X0000000B;	
	/**Set DRM func set,  Param is int type,  value is VOOSMP_DRM_CALLBACK's pointer.   */
	public static final int VOOSMP_PID_FUNC_DRM					= 0X0000000C;	
	/**Set audio render adjust time,  Param is int type .   */
	public static final int VOOSMP_PID_AUDIO_REND_ADJUST_TIME	= 0X0000000D;	
	/**Get engine status, Return type is int, value is refer to 
	 * <ul>
     * <li>{@link #VOOSMP_STATUS_INIT}
     * <li>{@link #VOOSMP_STATUS_LOADING}
     * <li>{@link #VOOSMP_STATUS_RUNNING}
     * <li>{@link #VOOSMP_STATUS_PAUSED}
     * <li>{@link #VOOSMP_STATUS_STOPPED}
     * </ul
	 */
	public static final int VOOSMP_PID_STATUS					= 0X0000000E;
	/**Override automatic screen width and height adjustment according to zoom mode. Parameter is int type. Please refer to the following:
	 * <ul>
     * <li>{@link #VOOSMP_ZM_LETTERBOX}
     * <li>{@link #VOOSMP_ZM_PANSCAN}
     * <li>{@link #VOOSMP_ZM_FITWINDOW}
     * <li>{@link #VOOSMP_ZM_ORIGINAL}
     * </ul
	 */
	public static final int VOOSMP_PID_ZOOM_MODE				= 0X0000000F;  
	/**Set device CPU number , Param is int type.   */
	public static final int VOOSMP_PID_CPU_NUMBER				= 0X00000010;
	/**Set event callback func, Param is int type, value is VOOSMP_LISTENERINFO's pointer   */
	public static final int VOOSMP_PID_LISTENER					= 0X00000011;
	/**Set audio render buffer time, Param is int type.    */
	public static final int VOOSMP_PID_AUDIO_REND_BUFFER_TIME	= 0X00000012;
	/**Get send buffer callback func, return is int type,  value is VOOSMP_SENDBUFFER_FUNC's pointer   */
	public static final int VOOSMP_PID_SENDBUFFERFUNC			= 0X00000013;
	/**Get audio format, return is int type, value is {@link voOSAudioFormat} */
	public static final int VOOSMP_PID_AUDIO_FORMAT				= 0X00000014;
	/**Get video format, return is int type, value is {@link voOSVideoFormat} */
	public static final int VOOSMP_PID_VIDEO_FORMAT				= 0X00000015;
	/**Set video render type, param is int type, value is refer to
	 * <ul>
     * <li>{@link #VOOSMP_RENDER_TYPE_JAVA}<br>
     * <li>{@link #VOOSMP_RENDER_TYPE_NATIVE}
     * <li>{@link #VOOSMP_RENDER_TYPE_BITMAP}
     * <li>{@link #VOOSMP_RENDER_TYPE_OPENGLES}
     * <li>{@link #VOOSMP_RENDER_TYPE_NATIVE_C}
     * <li>{@link #VOOSMP_RENDER_TYPE_HW_RENDER}
     * <li>{@link #VOOSMP_RENDER_TYPE_JMHW_RENDER}	
     * </ul>
     */
	public static final int VOOSMP_PID_VIDEO_RENDER_TYPE		= 0X00000016;
	/**Get media freamework type, return is int type*/
	public static final int VOOSMP_PID_PLAYER_TYPE				= 0X00000017;
	/**Set audio decoder module file name, param is String type ,
	 * VOOSMP_PID_DECODER_FILE_NAME is the base ID value,
	 * decoder module ID should be 
	 * (VOOSMP_PID_DECODER_FILE_NAME + {@link VOOSMP_AUDIO_CODINGTYPE}) value,
	 * for instance,if you want to set AAC decoder module name,
	 * it should be (VOOSMP_PID_AUDIO_DECODER_FILE_NAME + {@link VOOSMP_AUDIO_CODINGTYPE#VOOSMP_AUDIO_CodingAAC})*/
	public static final int VOOSMP_PID_AUDIO_DECODER_FILE_NAME	= 0X00180000;  														
	/**Set video decoder module file name, param is String type, same as {@link #VOOSMP_PID_AUDIO_DECODER_FILE_NAME} */
	public static final int VOOSMP_PID_VIDEO_DECODER_FILE_NAME	= 0X00190000; 
	/**Set codec performance data, param is {@link voOSPerformanceData}*/
	public static final int VOOSMP_PID_PERFORMANCE_DATA			= 0X0000001A;  
	/**Set the function address of read data, param is int type, value is VOOSMP_READDATA_FUNC's pointer */
	public static final int VOOSMP_PID_FUNC_READ_IO				= 0X0000001B;	
	/**Set audio sink step time, Param is int type, value is  ms */
	public static final int VOOSMP_PID_AUDIO_STEP_BUFFTIME		= 0X0000001C;	
	/**Set audio sink buffer time, Param is int type, value is  ms */
	public static final int VOOSMP_PID_AUDIO_SINK_BUFFTIME		= 0X0000001D;	
	/**Set enable/disable playback audio frame, param is int type, value is 0:disable,1:enable */
	public static final int VOOSMP_PID_AUDIO_STREAM_ONOFF		= 0X0000001E;  
	/**Set enable/disable playback video frame, Param is int type, value is 0:disable,1:enable */
	public static final int VOOSMP_PID_VIDEO_STREAM_ONOFF		= 0X0000001F;
	/**Get bitrate of audio codec, return is int array */
	public static final int VOOSMP_PID_AUDIO_DEC_BITRATE		= 0X00000020;	
	/**Get bitrate of video codec, return is int array*/
	public static final int VOOSMP_PID_VIDEO_DEC_BITRATE		= 0X00000021;
	/**Set media framework configuration file, param is String type*/
	public static final int VOOSMP_PID_MFW_CFG_FILE		= 0X00000022;

	/**Set audio decoder module API name, param is String type,
	 * VOOSMP_PID_AUDIO_DECODER_API_NAME is the base ID value,
	 * decoder API ID should be (VOOSMP_PID_AUDIO_DECODER_API_NAME + {@link VOOSMP_AUDIO_CODINGTYPE}) value,
	 * for instance,if you want to set AAC decoder API name,
	 * it should be (VOOSMP_PID_AUDIO_DECODER_API_NAME + {@link VOOSMP_AUDIO_CODINGTYPE#VOOSMP_AUDIO_CodingAAC}).*/
	public static final int VOOSMP_PID_AUDIO_DECODER_API_NAME	= 0X00230000;	
	/**Set video decoder API name, param is String type, same as {@link #VOOSMP_PID_AUDIO_DECODER_API_NAME}.*/
	public static final int VOOSMP_PID_VIDEO_DECODER_API_NAME	= 0X00240000;	
	/**Set enable/disable deblock, param is int type, value is 0:disable,1:enable .*/
	public static final int VOOSMP_PID_DEBLOCK_ONOFF			= 0x00000025;	
	/**Get CPU information, return is {@link voOSCPUInfo} */
	public static final int VOOSMP_PID_CPU_INFO					= 0x00000026;
	/**Set enable/disable load audio effect library, param is int type, value is 0:disable, 1:enable,default is 0*/
	public static final int VOOSMP_PID_LOAD_AUDIO_EFFECT_MODULE	= 0X00000027; 
	/**Set authentication information,for instance user name, param is String type. */
	public static final int VOOSMP_PID_AUTHENTICATION_INFO		= 0X00000028; 
//	public static final int VOOSMP_PID_COMMON_LOGFUNC			= 0X00000029;	/*!< Set Log function pointer */
	/** Set ClosedCaption parser enable or disable*, param is int type, 1 is enable, 0 is disable, default is 0.*/
	public static final int VOOSMP_PID_COMMON_CCPARSER			= 0X0000002A;
	/**Set license content. param is ByteArray type.   */
	public static final int VOOSMP_PID_LICENSE_CONTENT			= 0X0000002B;
	/**Set source filename. Find relational smi, srt subtitle file.  param is String type.   */
	public static final int VOOSMP_PID_SOURCE_FILENAME			= 0X0000002C;
	/**Set prefix of library name. param is String type.   */
	public static final int VOOSMP_PID_LIB_NAME_PREFIX			= 0X0000002D;
	/**Set suffix of library name. param is String type.   */
	public static final int VOOSMP_PID_LIB_NAME_SUFFIX			= 0X0000002E;
	/*!<Get/Set command string,module need parse this string to get the exact command. param(return) is String type*/
	public static final int VOOSMP_PID_COMMAND_STRING			= 0X0000002F;
	/**Set aspect ratio. param is int type, refer to VOOSMP_ASPECT_RATIO 
	 * 
	 * <ul>
     * <li>{@link #VOOSMP_RATIO_00}
     * <li>{@link #VOOSMP_RATIO_11}
     * <li>{@link #VOOSMP_RATIO_43}
     * <li>{@link #VOOSMP_RATIO_169}
	 * <li>{@link #VOOSMP_RATIO_21}
     * <li>{@link #VOOSMP_RATIO_2331}
	 * </ul>
	 *  <A HREF="../../../com/visualon/OSMPBasePlayer/voOSBasePlayer.html#updateVideoAspectRatio"><CODE>updateVideoAspectRatio</CODE></A> instead of the function. )
	 * */
	public static final int VOOSMP_PID_VIDEO_ASPECT_RATIO       = 0X00000030;	
	/**Enable/disable audio/video output immediately for rendering, Param is Integer type, 1:enable,0:disable */
	public static final int VOOSMP_PID_LOW_LATENCY  			= 0X00000032;
	/**Video view go to forground, set view handle, param type is SurfaceView.  */  
	public static final int VOOSMP_PID_VIEW_ACTIVE              = 0X00000033;
	/**
	 * Set the performance data buffer time, in second, param type is Integer
	 * @deprecated this API remove for task 27762.
	 */
	public static final int VOOSMP_PID_VIDEO_PERFORMANCE_CACHE  = 0X00000034;
	/**
	 * Set/Get the performance data Statistics, param data tyep is {@link voOSVideoPerformanceImpl}, return data type is {@link voOSVideoPerformance}
	 * @deprecated this API remove for task 27762.
	 */
	public static final int VOOSMP_PID_VIDEO_PERFORMANCE_OPTION = 0X00000035; 
	/**Get/Set Subtitle settings, the function isn't implement. */
	public static final int VOOSMP_PID_SUBTITLE_SETTINGS        = 0X00000036;
	/**Set Subtitle file full path */
	public static final int VOOSMP_PID_SUBTITLE_FILE_NAME       = 0X00000037;
	/**
	 * Set enable/disable the performance data statistics, param is Integer type. default disable, 0 is disable, 1 is enable
	 * @deprecated this API remove for task 27762.
	 */
	public static final int VOOSMP_PID_VIDEO_PERFORMANCE_ONOFF  = 0X00000038;
	/**Enable/disable subtitle settings, param is Integer type, 1:enable, 0:disable, default is 0 */
	public static final int VOOSMP_PID_SUBTITLE_SETTINGS_ONOFF  = 0X00000039;
	/**Set application suspend,such as go backgroud */
	public static final int VOOSMP_PID_APPLICATION_SUSPEND      = 0X0000003A;
	/**Set application resume,such as go foreground, it need set new view handle, param is SurfaceView*/    
	public static final int VOOSMP_PID_APPLICATION_RESUME       = 0X0000003B;
	/**Set read close caption or subtile buffer callback,refer to VOOSMP_READ_SUBTITLE_FUNC*/
//	public static final int VOOSMP_PID_READ_SUBTITLE_CALLBACK   = 0X0000003C;
	/**Set Audio sink  */     
	public static final int VOOSMP_PID_AUDIO_SINK               = 0X0000003D;
	/**Set Optimised bitmap object, param is Bitmap object  */
	public static final int VOOSMP_PID_BITMAP_HANDLE  			= 0x0000003E;
	/**Set the flag which you like to retrieve SEI, param is Integer type, param is combination of {@link VOOSMP_SEI_EVENT_FLAG},
	 * {@link VOOSMP_SEI_EVENT_FLAG#VOOSMP_FLAG_SEI_EVENT_NONE} indicate disable it,
	 * {@link VOOSMP_SEI_EVENT_FLAG#VOOSMP_FLAG_SEI_EVENT_PIC_TIMING} Indicate picture info,
	 * {@link VOOSMP_SEI_EVENT_FLAG#VOOSMP_FLAG_SEI_EVENT_MAX} indicate retrieve all SEI info */
	public static final int VOOSMP_PID_RETRIEVE_SEI_INFO        = 0x0000003F;
	/** Set SEI event interval. If want to disable event notification, then the corresponding int value is set to -1. param is int type, in ms */
	public static final int VOOSMP_PID_SEI_EVENT_INTERVAL       = 0x00000040;
	public static final int VOOSMP_PID_IOMX_PROBE               = 0x00000045; 
	 /**Set enable/disable the audio pcm output, param is int type. 1 to enable, 0 to disable. Default is 0. */
	public static final int VOOSMP_PID_AUDIO_PCM_OUTPUT        	= 0x00000047;
	 /**< Set enable/disable use mediacodec audio decoder, int *. 1 to enable, 0 to disable. Default is 0. Only implemented on Android*/ 
	public static final int VOOSMP_PID_MEDIACODEC_AUDIO_DECODER = 0x00000048;
	/** Get module version, refer to VOOSMP_MODULE_VERSION */
	public static final int  VOOSMP_PID_MODULE_VERSION   		= 0x00000049;  
	/** Get/Set audio playback speed, float*, range is 0.5 ~ 2.0, default is 1.0 */
	public static final int VOOSMP_PID_AUDIO_PLAYBACK_SPEED     = 0x00000050;
	/** Redraw video area using black color */
	public static final int VOOSMP_PID_CLEAR_VIDEO_AREA      	= 0x00000052;
	/** Set DRM unique identifier */
	public static final int VOOSMP_PID_DRM_UNIQUE_IDENTIFIER    = 0x00000054;
	/** Set time offset of subtitle rendering */
	public static final int VOOSMP_PID_SUBTITLE_TIME_OFFSET     = 0X00000056;  

        /* INTERNAL: To set handle for getting DSP audio clock */
        public static final int VOOSMP_PID_AUDIO_DSP_CLOCK		= 0X00000059;  
        
    /** Set pause reference clock or not. 1 for enable, 0 for disable*/
    public static final int VOOSMP_PID_PAUSE_REFERENCE_COLOCK	= 0x0000005A;
	/** Get audio render latency from hardware. Type is Integer, ms */
    public static final int VOOSMP_PID_AUDIO_RENDER_LATENCY     = 0x0000005B;
    /** Set audio render format, {@link voOSAudioFormat} */
    public static final int VOOSMP_PID_AUDIO_RENDER_FORMAT      = 0x0000005C;
    /** Set enable/disable loading Dolby decoder library, 0:disable, 1:enable, default is 1 */
    public static final int VOOSMP_PID_LOAD_DOLBY_DECODER_MODULE= 0x0000005D;
    /** Preview subtitle by sending some sample text to be rendered, {@link voOSPreviewSubtitleInfo} * */
    public static final int VOOSMP_PID_PREVIEW_SUBTITLE         = 0x0000005E;

	/**Get Bluetooth handset status, return is int type, value is 0: bluetooth off , 1: bluetooth on */
	public static final int VOOSMP_PID_BLUETOOTH_HANDSET		= 0x00001026;
	/**Set Surface changed finish, param is int type, value is 1.*/
	public static final int VOOSMP_PID_SURFACE_CHANGED			= 0x00001027;
	/**Set if HDMI connected, display or don't display video, param is int type, 0 is don't display, 1 is display*/
	public static final int VOOSMP_PID_HDMI_DISPLAY				= 0x00001028;
	/**Set ClosedCaption widget create in engine or outside.  */
	public static final int VOOSMP_PID_SUBTITLE_IN_ENGINE       = 0x00001029; 

	/* TASK 21051 INTERNAL: threshold to call setFixedSize() */
	public static final int VOOSMP_PID_FIXEDSIZE_THRESHOLD 		= 0x0000102A; 
	
	/**Set Output or not output close caption data to upper program , param is int type, value is 0:disable,1:enable , default is 0,
	 * If output, upper program can receive ClosedCaption data by OnEvent({@link #VOOSMP_CB_ClosedCaptionData}, 0, 0, 
	 * {@link com.visualon.OSMPSubTitle.voSubTitleManager.voSubtitleInfo})
	 * upper program should be responsible for deal with ClosedCaption. voOSStreamSDK don't show ClosedCation.
	 * If don't output, voOSStreamSDK can show ClosedCation.
	 * */
	public static final int VOOSMP_PID_CLOSED_CAPTION_OUTPUT  	= 0X00002900;  
	/**Get close caption settings object, return is CCSettings */
	public static final int VOOSMP_PID_CLOSED_CAPTION_SETTINGS 	= 0X00002901;  
	/**Set keep SurfaceView layout aspect ratio fit to video aspect ratio, 1 for keep, 0 for not keep, default is 1 */
	public static final int VOOSMP_PID_KEEP_ASPECT_RATIO        = 0x00002902;
	
	/**Set enable/disable playback audio frame, param is int type, value is 0:disable,1:enable */
	public static final int VOOMXAL_PID_AUDIO_STREAM_ONOFF  	= 0X00000009;  
	/**Set enable/disable playback video frame, param is int type, value is 0:disable,1:enable */
	public static final int VOOMXAL_PID_VIDEO_STREAM_ONOFF  	= 0X0000000A;


	
	
	/**
	* Enumeration for open source flag, used in Open() function
	* VOOSMP_OPEN_SOURCE_FLAG
	*/
	
	/**Indicate the source is URL */
	public static final int VOOSMP_FLAG_SOURCE_URL			= 0X00000001;
	/**Indicate the source is out of buffer. ReadBuffer */
	public static final int VOOSMP_FLAG_SOURCE_READBUFFER	= 0X00000002;
	/**Indicate the source is out of buffer. SendBuffer */
	public static final int VOOSMP_FLAG_SOURCE_SENDBUFFER	= 0X00000004;
	/**Indicate the source is Handle */
	public static final int VOOSMP_FLAG_SOURCE_HANDLE		= 0X00000008;	
	/** Indicates the source was opened in synchronous mode.
	* The function will not return until the open operation is complete.
	* */
	public static final int VOOSMP_FLAG_SOURCE_OPEN_SYNC	= 0X00000010;
	/** Indicates the source was opened in asynchronous mode.
	* The function will return immediately after the call.
	* The completion of the open will be indicated by the {@link #VOOSMP_SRC_CB_Open_Finished} event.
	* */
	public static final int VOOSMP_FLAG_SOURCE_OPEN_ASYNC	= 0X00000020;
	

		
	
	/**
	 * Definition of color format
	 * VOOSMP_COLORTYPE
	 */
	
	/** RGB packed mode, data: B:5 G:6 R:5 */
	public static final int VOOSMP_COLOR_RGB565_PACKED		= 20;		
	/**< RGB packed mode, data: B G R A	*/
	public static final int VOOSMP_COLOR_RGB32_PACKED		= 23;		
	/**ARGB packed mode, data: B G R A*/
	public static final int VOOSMP_COLOR_ARGB32_PACKED		= 26;	
	
	
	/**
	 * Definition of aspect ratio type
	 * VOOSMP_ASPECT_RATIO
	 */
	/** Width and height is width: height */
	public static final int VOOSMP_RATIO_00					= 0x00;	 
	/** Width and Height is 1 : 1		*/
	public static final int VOOSMP_RATIO_11					= 0X01; 
	/** Width and Height is 4 : 3		*/
	public static final int VOOSMP_RATIO_43					= 0X02; 
	/** Width and Height is 16 : 9	*/
	public static final int VOOSMP_RATIO_169				= 0X03;  
	/** Width and Height is 2 : 1		*/
	public static final int VOOSMP_RATIO_21					= 0X04; 
	/** Width and Height is 2.33 : 1 */
	public static final int VOOSMP_RATIO_2331       		= 0X05; 
	/** Use aspect ratio value from video frame.
	*  If frame does not contain this value, behavior is same as VO_OSMP_RATIO_ORIGINAL
	*/
	public static final int VOOSMP_RATIO_AUTO       		= 0X06;
	/** Use video original ratio of yuv, width: height */
	public static final int VOOSMP_RATIO_ORIGINAL       	= 0X00;	
	
	
	/*
	 *the status of the engine	
	 */
	/**The status is init */
	public static final int VOOSMP_STATUS_INIT				= 0;	
	/**The status is loading */
	public static final int VOOSMP_STATUS_LOADING			= 1;		
	/**The status is running */
	public static final int VOOSMP_STATUS_RUNNING			= 2;	
	/**The status is paused */
	public static final int VOOSMP_STATUS_PAUSED			= 3;		
	/**The status is stopped */
	public static final int VOOSMP_STATUS_STOPPED			= 4;			
	
	
	/**
	 * Definition of zoom mode
	 * VOOSMP_ZOOM_MODE
	 */
	/** the zoom mode is letter box		*/
	public static final int VOOSMP_ZM_LETTERBOX				= 0X01;
	/** the zoom mode is pan scan			*/
	public static final int VOOSMP_ZM_PANSCAN				= 0X02;
	/** the zoom mode is fit to window	*/
	public static final int VOOSMP_ZM_FITWINDOW				= 0X03;
	/** the zoom mode is original size	*/
	public static final int VOOSMP_ZM_ORIGINAL				= 0X04;
	/** the xoom mode is zoom in with the window */
	public static final int VOOSMP_ZM_ZOOMIN        		= 0x05;
	
	
	/**
	 * Enumeration for init param flag, used in Init() API
	 * VOOSMP_INIT_PARAM_FLAG
	 */
	/**Indicate init PARAM is NULL */
	public static final int VOOSMP_FLAG_INIT_NOUSE			= 0X00000000;
	/**Indicate init PARAM is load library function address */
	public static final int VOOSMP_FLAG_INIT_LIBOP			= 0X00000001;
	/**Indicate init PARAM is IO module file name */
	public static final int VOOSMP_FLAG_INIT_IO_FILE_NAME   = 0X00000002;
	/**Indicate init PARAM is IO function name */
	public static final int VOOSMP_FLAG_INIT_IO_API_NAME    = 0X00000004;
	/**Indicate init PARAM is actual file size */
	public static final int VOOSMP_FLAG_INIT_ACTUAL_FILE_SIZE   = 0X00000008;
	
	
	/**
	 * Enumeration for available track, used in VOOSMP_CB_MediaTypeChanged callback
	 * VOOSMP_AVAILABLE_TRACK_TYPE
	 */	
	/**Only audio is available */
	public static final int VOOSMP_AVAILABLE_PUREAUDIO		= 0x00;		
	/**Only video is available */
	public static final int VOOSMP_AVAILABLE_PUREVIDEO		= 0x01;		
	/**Both audio and video are available */
	public static final int VOOSMP_AVAILABLE_AUDIOVIDEO		= 0x02;		
	
	/**
	 * Enumeration for screen brightness mode track, 
	 * used in voOSBasePlayer.setScreenBrightnessMode and voOSBasePlayer.getScreenBrightnessMode function
	 * 
	 * VO_OSMP_SCREEN_BRIGHTNESS_MODE
	 */	
	/**Indicate current screen brightness mode value is manual mode  */
	public static final int VOOSMP_SCREEN_BRIGHTNESS_MODE_MANUAL     = 0x00000000;
	/**Indicate current screen brightness mode value is automatic mode  */
	public static final int VOOSMP_SCREEN_BRIGHTNESS_MODE_AUTOMATIC  = 0x00000001;
    
	/**
	 * Video Coding type
	 */
	public enum VOOSMP_VIDEO_CODINGTYPE
	{
		/** Value when coding is N/A */
		VOOSMP_VIDEO_CodingUnused                           (0x00000000),
		/** AKA: H.262 */
		VOOSMP_VIDEO_CodingMPEG2                            (0x00000001),
		/** H.263 */
		VOOSMP_VIDEO_CodingH263                             (0x00000002),
		/** H.263 */
		VOOSMP_VIDEO_CodingS263                             (0x00000003),
		/** MPEG-4 */
		VOOSMP_VIDEO_CodingMPEG4                            (0x00000004),
		/** H.264/AVC */
		VOOSMP_VIDEO_CodingH264                             (0x00000005),
		/** all versions of Windows Media Video */
		VOOSMP_VIDEO_CodingWMV                              (0x00000006),
		/** all versions of Real Video */
		VOOSMP_VIDEO_CodingRV                               (0x00000007),
		/** Motion JPEG */
		VOOSMP_VIDEO_CodingMJPEG                            (0x00000008),
		/** DIV3 */
		VOOSMP_VIDEO_CodingDIVX                             (0x00000009),
		/** VP6 */
		VOOSMP_VIDEO_CodingVP6                              (0x0000000A),
		/** VP8 */
		VOOSMP_VIDEO_CodingVP8                              (0x0000000B),
		/** VP7 */
		VOOSMP_VIDEO_CodingVP7                              (0x0000000C),
		/** VC1:WMV3,WMVA,WVC1*/
		VOOSMP_VIDEO_CodingVC1                              (0x0000000D),
		/*!< H265 */
		VOOSMP_VIDEO_CodingH265                             (0x0000000E),   
		 
		VOOSMP_VIDEO_Coding_Max                             (0X7FFFFFFF);
		
		private int value;
		
		VOOSMP_VIDEO_CODINGTYPE(int event)
		{
			value = event;
		}
		
		public int getValue()
		{
			return value;
		}
		
		public static VOOSMP_VIDEO_CODINGTYPE valueOf(int value)
		{
			if (value < VOOSMP_VIDEO_CODINGTYPE.values().length)
				return VOOSMP_VIDEO_CODINGTYPE.values()[value];
			
			return VOOSMP_VIDEO_Coding_Max;
		}
	}
	
	/**
	 *Enumeration used to define the possible audio codings.
	 */
	public enum VOOSMP_AUDIO_CODINGTYPE
	{
		/**< Placeholder value when coding is N/A  */
		VOOSMP_AUDIO_CodingUnused                           (0x00000000),
		/** Any variant of PCM coding */
		VOOSMP_AUDIO_CodingPCM                              (0x00000001),
		/** Any variant of ADPCM encoded data */
		VOOSMP_AUDIO_CodingADPCM                            (0x00000002),  
		/** Any variant of AMR encoded data */
		VOOSMP_AUDIO_CodingAMRNB                            (0x00000003),
		/** Any variant of AMR encoded data */
		VOOSMP_AUDIO_CodingAMRWB                            (0x00000004),
		/** Any variant of AMR encoded data */
		VOOSMP_AUDIO_CodingAMRWBP                           (0x00000005),
		/** Any variant of QCELP 13kbps encoded data */
		VOOSMP_AUDIO_CodingQCELP13                          (0x00000006),
		/** Any variant of EVRC encoded data */
		VOOSMP_AUDIO_CodingEVRC                             (0x00000007),
		/** Any variant of AAC encoded data, 0xA106 - ISO/MPEG-4 AAC, 0xFF - AAC */
		VOOSMP_AUDIO_CodingAAC                              (0x00000008),
		/** Any variant of AC3 encoded data */
		VOOSMP_AUDIO_CodingAC3                              (0x00000009), 
		/** Any variant of FLAC encoded data */
		VOOSMP_AUDIO_CodingFLAC                             (0x0000000A),
		/** Any variant of MP1 encoded data */
		VOOSMP_AUDIO_CodingMP1                              (0x0000000B),
		/** Any variant of MP3 encoded data */
		VOOSMP_AUDIO_CodingMP3                              (0x0000000C),
		/** Any variant of OGG encoded data */
		VOOSMP_AUDIO_CodingOGG                              (0x0000000D),
		/** Any variant of WMA encoded data */
		VOOSMP_AUDIO_CodingWMA                              (0x0000000E),
		/** Any variant of RA encoded data */
		VOOSMP_AUDIO_CodingRA                               (0x0000000F),
		/** Any variant of MIDI encoded data */
		VOOSMP_AUDIO_CodingMIDI                             (0x00000010),
		/** Any variant of dra encoded data */
		VOOSMP_AUDIO_CodingDRA                              (0x00000011),
		/** Any variant of dra encoded data */
		VOOSMP_AUDIO_CodingG729                             (0x00000012),
		/** Any variant of Enhanced AC3 encoded data */
		VOOSMP_AUDIO_CodingEAC3                             (0x00000013),
		/** Any variant of APE encoded data */
		VOOSMP_AUDIO_CodingAPE                              (0x00000014),
		/** Any variant of ALAC encoded data */
		VOOSMP_AUDIO_CodingALAC                             (0x00000015),
		/** Any variant of DTS encoded data */
		VOOSMP_AUDIO_CodingDTS                              (0x00000016),
		
		VOOSMP_AUDIO_Coding_MAX                             (0X7FFFFFFF);
	
		private int value;
		
		VOOSMP_AUDIO_CODINGTYPE(int event)
		{
			value = event;
		}
		
		public int getValue()
		{
			return value;
		}
		
		public static VOOSMP_AUDIO_CODINGTYPE valueOf(int value)
		{
			if (value < VOOSMP_AUDIO_CODINGTYPE.values().length)
				return VOOSMP_AUDIO_CODINGTYPE.values()[value];
			
			return VOOSMP_AUDIO_Coding_MAX;
		}
	}


	/**
	 * Data source ID
	 * 
	 */
	/**
	 * Defination of return error code
	 */
	/**Open source fail */
	public static final int VOOSMP_SRC_ERR_OPEN_SRC_FAIL			= 0X81000001;
	/**Content is encrypt, only can playback after some operations */
	public static final int VOOSMP_SRC_ERR_CONTENT_ENCRYPT			= 0X81000002;
	/**Play mode not support */
	public static final int VOOSMP_SRC_ERR_PLAYMODE_UNSUPPORT		= 0X81000003;
	/**File has error data */
	public static final int VOOSMP_SRC_ERR_ERROR_DATA				= 0X81000004;
	/**Seek not support */
	public static final int VOOSMP_SRC_ERR_SEEK_FAIL				= 0X81000005;
	/**File format not support */
	public static final int VOOSMP_SRC_ERR_FORMAT_UNSUPPORT			= 0X81000006; 
	/**Track can not be found */
	public static final int VOOSMP_SRC_ERR_TRACK_NOTFOUND			= 0X81000007; 
	/**No set download pointer */
	public static final int VOOSMP_SRC_ERR_NO_DOWNLOAD_OP			= 0X81000008; 
	/**No set lib op  pointer */
	public static final int VOOSMP_SRC_ERR_NO_LIB_OP				= 0X81000009;
	/**Can not find such output type in this source */
	public static final int VOOSMP_SRC_ERR_OUTPUT_NOTFOUND			= 0X8100000A;
	/**Indicate the chunk must be dropped */
	public static final int VOOSMP_SRC_ERR_CHUNK_SKIP           	= 0X8100000B;
	/**Indicate source should use the timestamp in the event callback function's parameter */
	public static final int VOOSMP_SRC_ERR_FORCETIMESTAMP           = 0X8100000C;
	/**Source Object not initialize*/
	public static final int VOOSMP_SRC_ERR_SOURCE_UNINITIALIZE		= 0X80001001;
	
	/** RequestCallBack Callback function, 
	 * return this code present user will control these warning event, */
	public static final int VOOSMP_SRC_ERR_CB_CONTROL_WARNING_EVENT = 0x80001002;

	/**
	 * Defination event call back ID
	 */
	/**It's connectting source */
	public static final int VOOSMP_SRC_CB_Connecting				= 0X02000001;	
	/**Source connection is finished */
	public static final int VOOSMP_SRC_CB_Connection_Finished		= 0X02000002;
	/**Source connect timeout */
	public static final int VOOSMP_SRC_CB_Connection_Timeout		= 0X82000003;
	/**Source connection loss */
	public static final int VOOSMP_SRC_CB_Connection_Loss			= 0X82000004;	
	/**HTTP download status the param 1 is int (0 - 100)  */
	public static final int VOOSMP_SRC_CB_Download_Status			= 0X02000005;
	/**Notify connect fail */
	public static final int VOOSMP_SRC_CB_Connection_Fail			= 0X82000006;
	/**Notify download fail */
	public static final int VOOSMP_SRC_CB_Download_Fail				= 0X82000007;
	/**Notify drm engine err */
	public static final int VOOSMP_SRC_CB_DRM_Fail					= 0X82000008;	
	/**Notify playlist parse error */
	public static final int VOOSMP_SRC_CB_Playlist_Parse_Err		= 0X82000009;	
	/**Notify maximum number of connections reached,currently used in RTSP only */
	public static final int VOOSMP_SRC_CB_Connection_Rejected		= 0X8200000A;
	/**Notify bitrate is changed, param1 is new bitrate value, Integer type */
	public static final int VOOSMP_SRC_CB_BA_Happened           	= 0X0200000B;
	/**Notify device is rooted.*/
	public static final int VOOSMP_SRC_CB_DRM_Not_Secure	    	= 0X0200000C;
	/**Notify device uses a/v output device but the license doesnt allows it.*/
	public static final int VOOSMP_SRC_CB_DRM_AV_Out_Fail       	= 0X8200000D;
	/**Notify download failed,and is waitting recover*/
	public static final int VOOSMP_SRC_CB_Download_Fail_Waiting_Recover     = 0X8200000E;
	/**Notify download recovered*/
	public static final int VOOSMP_SRC_CB_Download_Fail_Recover_Success     = 0X0200000F;
	/**Source open finished, param 1 should be VOOSMP_SRC_ERR_XXX, {@link #VOOSMP_SRC_ERR_OPEN_SRC_FAIL} or {@link #VOOSMP_ERR_None}*/
	public static final int VOOSMP_SRC_CB_Open_Finished         	= 0X02000010;  
	/**Notify customer tag information inside source, param 1 will be the value defined in {@link #VOOSMP_SRC_CUSTOMERTAGID_TIMEDTAG} and param 2 will depend on param 1 */
	public static final int VOOSMP_SRC_CB_Customer_Tag          	= 0X02000020;
	
	/**Notify streaming information , param1 will be defined to following value, other params will be define param1
	 * <ul>
	 * <li>{@link #VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_BITRATE_CHANGE}
	 * <li>{@link #VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_MEDIATYPE_CHANGE}
	 * <li>{@link #VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PROGRAM_TYPE}
	 * <li>{@link #VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_BEGINDOWNLOAD}   
	 * <li>{@link #VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DROPPED}
	 * <li>{@link #VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DOWNLOADOK}
	 * <li>{@link #VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PLAYLIST_DOWNLOADOK}
	 * <li>{@link #VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PROGRAM_CHANGE}
	 * <li>{@link #VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_FILE_FORMATSUPPORTED}
	 * <li>{@link #VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_LIVESEEKABLE}
	 * <li>{@link #VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_DISCONTINUE_SAMPLE}
	 * </ul>
	 */
	public static final int VOOSMP_SRC_CB_Adaptive_Streaming_Info 	= 0X02000030;
	 
	/**Notify adaptive streaming error, param1 will be defined to following value, other params2 will depend on param1
	 * <ul>
	 * <li>{@link #VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_PLAYLIST_PARSEFAIL}
	 * <li>{@link #VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_PLAYLIST_UNSUPPORTED}
	 * <li>{@link #VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_UNSUPPORTED}
	 * <li>{@link #VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_DOWNLOADFAIL}
	 * <li>{@link #VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_DRMLICENSEERROR}
	 * </ul>
	 */
	public static final int VOOSMP_SRC_CB_Adaptive_Streaming_Error 	= 0X02000040;  
	
	/**Notify adaptive streaming error warning, param1 will be defined to following value, other params will depend on param1
	 * <ul>
	 * <li>{@link #VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_DOWNLOADERROR}
	 * <li>{@link #VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_FILEFORMATUNSUPPORTED}
	 * <li>{@link #VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_DRMERROR}
	 * <li>{@link #VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_PLAYLIST_DOWNLOADERROR}
	 * </ul>
	 * */
	public static final int VOOSMP_SRC_CB_Adaptive_Stream_Warning 	= 0X02000050;	
	
	
	/**Notify RTSP error, param 1 will be defined to following value
	 * <ul>
	 * <li>{@link #VOOSMP_SRC_RTSP_ERROR_CONNECT_FAIL}
	 * <li>{@link #VOOSMP_SRC_RTSP_ERROR_DESCRIBE_FAIL}	
	 * <li>{@link #VOOSMP_SRC_RTSP_ERROR_SETUP_FAIL}
	 * <li>{@link #VOOSMP_SRC_RTSP_ERROR_PLAY_FAIL}
	 * <li>{@link #VOOSMP_SRC_RTSP_ERROR_PAUSE_FAIL}
	 * <li>{@link #VOOSMP_SRC_RTSP_ERROR_OPTION_FAIL}
	 * <li>{@link #VOOSMP_SRC_RTSP_ERROR_SOCKET_ERROR}
	 * </ul>
	 * */
	public static final int VOOSMP_SRC_CB_RTSP_Error              	= 0X02000060;    
	/**Notify seek complete, param 1 should be the seek return timestamp, , 
	 * param 2 should be the pointor of VOOSMP_SRC_ERR_XXX value, 
	 * {@link #VOOSMP_SRC_ERR_SEEK_FAIL} or {@link #VOOSMP_ERR_None}*/
	public static final int VOOSMP_SRC_CB_Seek_Complete             = 0X02000070;
	/**Notify the program info has been changed in source */
	public static final int VOOSMP_SRC_CB_Program_Changed			= 0X02000071;
	/**Notify the program info has been reset in source */
	public static final int VOOSMP_SRC_CB_Program_Reset				= 0x02000072;
	/** Notify when seek to the last chunk of play list (NTS link without END tag need change to live mode) */
	public static final int VOOSMP_SRC_CB_Adaptive_Stream_SEEK2LASTCHUNK	= 0x02000073;
	/**Network is not available now,but it still hase some buffering data which can playback */
	public static final int VOOSMP_SRC_CB_Not_Applicable_Media    	= 0x02000074;
	/**Notify the current media position downloaded by the progressive download module, param1 is the position, int* */
	public static final int VOOSMP_SRC_CB_PD_DOWNLOAD_POSITION    	= 0X02000075;
	/**Notify the current buffering percent of the progressive download module, param1 is the percent, int* */
	public static final int VOOSMP_SRC_CB_PD_BUFFERING_PERCENT    	= 0X02000076; 
	/** Update source URL complete, param1 is {@link VO_OSMP_CB_EVENT_ID} */
	public static final int VOOSMP_SRC_CB_UPDATE_URL_COMPLETE       = 0X02000077;
	
	/** Request authentication information, application need process it.
    param 1 is  value of {@link VOOSMP_SRC_AUTHENTICATION_REQUEST_TYPE} */
	public static final int VOOSMP_SRC_CB_Authentication_Request    = 0X03000001;
	/**HTTP IO has start download, third param obj will be the related url, type is String */
	public static final int VOOSMP_SRC_CB_IO_HTTP_Start_Download    = 0X03000002;
	/**HTTP IO failed to download, third param obj will be reason, type is {@link voOSIOFailedDesc} */
	public static final int VOOSMP_SRC_CB_IO_HTTP_Download_Failed   = 0X03000003;
	/**Event of DRM init data,param is VOOSMP_SRC_DRM_INIT_DATA */
	public static final int VOOSMP_SRC_CB_DRM_Init_Data   = 0X03000004;



	
	/**callback a TimedTag event, this is param1,  Object param is {@link voOSTimedTag} */
	public static final int VOOSMP_SRC_CUSTOMERTAGID_TIMEDTAG 		= 0X00000001;

	
	/**
	 * Defination parameter ID
	 */			
	/**Set DRM file name, param is String type.*/
	public static final int VOOSMP_SRC_PID_DRM_FILE_NAME			= 0X03000001;
	/**Set DRM API name*, param is String type.*/
	public static final int VOOSMP_SRC_PID_DRM_API_NAME				= 0X03000002;
	/**Set the io function, it can be local file or http file, param is int type, value is VODOWNLOAD_FUNC's pointer. */
	public static final int VOOSMP_SRC_PID_FUNC_IO					= 0X03000003;
	/**Get read buffer function pointer, return is int type, valude is VOOSMP_READBUFFER_FUNC's pointer. */
	public static final int VOOSMP_SRC_PID_FUNC_READ_BUF			= 0X03000004;
	/**Set the duration of close caption swith automatically between 608 and 708, param is int*/
	public static final int VOOSMP_SRC_PID_CC_AUTO_SWITCH_DURATION	= 0X03000005;  
	/**Set third party DRM function set, param is int type, value is thirdparty drm function's pointer. */
	public static final int VOOSMP_SRC_PID_DRM_THIRDPARTY_FUNC_SET	= 0X03000006;
	/**Set DRM callback function,customer follow VisualOn's DRM API, param is int type, value is drm callback function's pointer. */
	public static final int VOOSMP_SRC_PID_DRM_CALLBACK_FUNC		= 0X03000007;
	 /**Set the http verification callback, VOOSMP_SRC_HTTP_VERIFYCALLBACK* */
//	public static final int VOOSMP_SRC_PID_HTTPVERIFICATIONCALLBACK	= 0X03000008;
	/**Set the verification information to start HTTP verification, param is voOSVerificationnfo class type. */
	public static final int VOOSMP_SRC_PID_DOHTTPVERIFICATION		= 0X03000009; 
	/**Set RTSP connection type,param is int type, 0:automatically try UDP first then TCP,1:force to use TCP,-1:disable TCP, default 0, int* */
	public static final int VOOSMP_SRC_PID_SOCKET_CONNECTION_TYPE	= 0X0300000A;
	/**Get/Set command string,module need parse this string to get the exact command. param(return) is String type*/
	public static final int VOOSMP_SRC_PID_COMMAND_STRING			= 0X0300000B;
	/**Set cap information table path, param i String type, indicate cap table file path. */
	public static final int VOOSMP_SRC_PID_CAP_TABLE_PATH			= 0X0300000C;
	/**Set start cap information, param is {@link voOSPerformanceData} , you can Set the parameter after Player.Open()*/
	public static final int VOOSMP_SRC_PID_BA_STARTCAP				= 0X0300000D;
	/**Set the verification information to start DRM verification, param is voOSVerificationnfo class type.  */
	public static final int VOOSMP_SRC_PID_DODRMVERIFICATION		= 0X0300000E;
	/**Set IMEI or MAC of the device*/
	public static final int VOOSMP_SRC_PID_DRM_UNIQUE_IDENTIFIER    = 0X0300000F;
	/**Set max tolerant count if download fail*/
	public static final int VOOSMP_SRC_PID_DOWNLOAD_FAIL_MAX_TOLERANT_COUNT   = 0X03000010;  
	/**Enable/disable CPU adaption when chose BA, param is Integer type, 1:disable,0:enable*/
	public static final int VOOSMP_SRC_PID_DISABLE_CPU_ADAPTION               = 0X03000011;
	/**Set DRM function set */
	public static final int VOOSMP_SRC_PID_DRM_FUNC_SET						  = 0X03000012;
	/**Set the buffering time of playback start, param type is Integer, unit is ms */
	public static final int VOOSMP_SRC_PID_BUFFER_START_BUFFERING_TIME		  = 0X03000013;
	/**Set the buffering time after playing,for example after seeking, param type is Integer, unit is ms */
	public static final int VOOSMP_SRC_PID_BUFFER_BUFFERING_TIME			  = 0X03000014;
	/**Get the DVR Info, {@link voOSDVRInfo} */
	public static final int VOOSMP_SRC_PID_DVRINFO                            = 0X03000015;
	
	/**Set Timed Text ClosedCaption file path, param is String type.  */
	public static final int VOOSMP_SRC_PID_Timed_Text_ClosedCaption 		  = 0x03010010; 
	/**Get get Source module object, the ID is reserve */
	public static final int VOOSMP_SRC_PID_SOURCE_MODULE_OBJECT     		  = 0x03010011; 
	/**Set Source Init param {@link #VOOSMP_FLAG_INIT_ACTUAL_FILE_SIZE}, param is Long type, value is filesize */
	public static final int VOOSMP_SRC_PID_INIT_PARAM_ACTUAL_FILE_SIZE        = 0x03010012;
	/**Set Source Init param {@link #VOOSMP_SRC_PID_INIT_PARAM_INIT_LIBOP}, param is Integer type. */
	public static final int VOOSMP_SRC_PID_INIT_PARAM_INIT_LIBOP		      = 0x03010013;
	/**Set Source Init param {@link #VOOSMP_SRC_PID_INIT_PARAM_IO_FILE_NAME}, param is String type. */
	public static final int VOOSMP_SRC_PID_INIT_PARAM_IO_FILE_NAME  		  = 0x03010014;
	/**Set Source Init param {@link #VOOSMP_SRC_PID_INIT_PARAM_IO_API_NAME}, param is String type. */
	public static final int VOOSMP_SRC_PID_INIT_PARAM_IO_API_NAME   		  = 0x03010015;
	/**Get the BA work mode, return value, please refer to 
	 * <ul>
	 * <li> {@link #VOOSMP_SRC_ADAPTIVE_STREAMING_BA_MODE_AUTO}
	 * <li> {@link #VOOSMP_SRC_ADAPTIVE_STREAMING_BA_MODE_MANUAL}
	 * </ul>
	 * */
	public static final int VOOSMP_SRC_PID_BA_WORKMODE				= 0X03000016;
	/**
	 * Get the rtsp module status value, return value is {@link voOSRTSPStatus}
	 * @deprecated this API remove for task 27762.
	 */
	public static final int VOOSMP_SRC_PID_RTSP_STATUS				= 0X03000017;
	/**Set source module into low latency mode */
	public static final int VOOSMP_SRC_PID_LOW_LATENCY_MODE			= 0X03000018;
	/**Set source module max buffer size, type is Integer, unit is ms */
	public static final int VOOSMP_SRC_PID_BUFFER_MAX_SIZE          = 0X03000019;
	/**Set the HTTP header, VOOSMP_SRC_HTTP_HEADER */
	public static final int VOOSMP_SRC_PID_HTTP_HEADER				= 0X0300001A;
	/**Set RTSP connection port, VOOSMP_SRC_RTSP_CONNECTION_PORT */
	public static final int VOOSMP_SRC_PID_RTSP_CONNECTION_PORT     = 0X0300001B;
	/**Set proxy server information, VOOSMP_SRC_HTTP_PROXY */
    public static final int VOOSMP_SRC_PID_HTTP_PROXY_INFO          = 0X0300001C; 
    /**Set DRM adapter function set. void */
    public static final int VOOSMP_SRC_PID_DRM_ADAPTER_OBJECT       = 0X0300001F;
	/**Set presentation delay time (milliseconds) for a live streaming source with respective to current time */
    public static final int VOOSMP_SRC_PID_PRESENTATION_DELAY       = 0X03000020;
	/**Set retry times when socket connect failed, the parameter is Integer type, the default value is 2, and if negative value is set, it will retry endless, until the Stop() be called.*/
	public static final int VOOSMP_SRC_PID_PD_CONNECTION_RETRY_TIMES		= 0X03000021;
	/**Convert from period sequence number to timestamp, {@link voOSPeriodTime} */
	public static final int VOOSMP_SRC_PID_PERIOD2TIME                      = 0X03000022;
	/**Set DRM init data response to DRM module */
	public static final int VOOSMP_SRC_PID_DRM_INIT_DATA_RESPONSE           = 0X03000023;
	/** Set application suspend */
	public static final int VOOSMP_SRC_PID_APPLICATION_SUSPEND              = 0X03000024;
	/** Set application resume */
	public static final int VOOSMP_SRC_PID_APPLICATION_RESUME               = 0X03000025;
	/** Set upper/lower bitrate threshold for adaptation, {@link voOSSrcBAThreshold} */
	public static final int VOOSMP_SRC_PID_BITRATE_THRESHOLD                = 0X03000026;
	/** Set the maximum number of retry attempts to download a corresponding segment from a different bit rate due, default is 0, the parameter is Integer type */
	public static final int VOOSMP_SRC_PID_SEGMENT_DOWNLOAD_RETRY_COUNT     = 0X03000027;
	/** Enable/Disable RTSP over HTTP tunneling. The default is disable(0). The parameter is Integer type. */
	public static final int VOOSMP_SRC_PID_ENABLE_RTSP_HTTP_TUNNELING       = 0X03000028;
	/** Set port number for RTSP over HTTP tunneling. The parameter is Integer type. */
	public static final int VOOSMP_SRC_PID_RTSP_OVER_HTTP_CONNECTION_PORT   = 0X03000029;
	/** Set new source URL. The parameter is String type. */
	public static final int VOOSMP_SRC_PID_UPDATE_SOURCE_URL                = 0X0300002A;  
	/** Set HTTP connection retry timeout, param type is Integer,  The default is 120 seconds. Setting the value to -1 disables the timeout so that the player will keep retrying, until the connection is established again. */
	public static final int VOOSMP_SRC_PID_HTTP_RETRY_TIMEOUT               = 0X0300002B;
//	/** Set default audio language, param is Integer type, refer to VOOSMP_LANGUAGE_TYPE */
//	public static final int VOOSMP_SRC_PID_DEFAULT_AUDIO_LANGUAGE           = 0X0300002C;
//	/** Set default subtitle language, param is Integer type, refer to VOOSMP_LANGUAGE_TYPE */
//	public static final int VOOSMP_SRC_PID_DEFAULT_SUBTITLE_LANGUAGE        = 0X0300002D;
	/**
	 * Emuneration of source fromat / VOOSMP_SRC_SOURCE_FORMAT
	 */	
	/** Detect the source format automatically*/
	public static final int VOOSMP_SRC_AUTO_DETECT					= 0X00000000;   
	/** MP4 File Format */
	public static final int VOOSMP_SRC_FFMOVIE_MP4					= 0X00000001;   
	/** MS ASF File Format */
	public static final int VOOSMP_SRC_FFMOVIE_ASF					= 0X00000002; 
	/** AVI File Format */
	public static final int VOOSMP_SRC_FFMOVIE_AVI					= 0X00000004;   
	/** QuickTime Movie File Format */
	public static final int VOOSMP_SRC_FFMOVIE_MOV					= 0X00000008;   
	/** Real File Format */
	public static final int VOOSMP_SRC_FFMOVIE_REAL					= 0X00000010;   
	/** Mpeg File Format */
	public static final int VOOSMP_SRC_FFMOVIE_MPG					= 0X00000020;  
	/** TS File Format */
	public static final int VOOSMP_SRC_FFMOVIE_TS					= 0X00000040;   
	/** DV File Format */
	public static final int VOOSMP_SRC_FFMOVIE_DV					= 0X00000080;   
	/** FLV File Format */
	public static final int VOOSMP_SRC_FFMOVIE_FLV					= 0x00000100;  
	/** CMMB dump file */	
	public static final int VOOSMP_SRC_FFMOVIE_CMMB					= 0X00000200;	
	/** mkv file format */
	public static final int VOOSMP_SRC_FFMOVIE_MKV					= 0X00000400;
	/** Video H264 File Format */
	public static final int VOOSMP_SRC_FFVIDEO_H264					= 0X00000800; 
	/** Video H263 File Format */
	public static final int VOOSMP_SRC_FFVIDEO_H263					= 0X00001000;   
	/** Video H261 File Format */
	public static final int VOOSMP_SRC_FFVIDEO_H261					= 0X00002000;   
	/** Video Mpeg4 File Format */
	public static final int VOOSMP_SRC_FFVIDEO_MPEG4				= 0X00004000;   
	/** Video Mpeg2 File Format */
	public static final int VOOSMP_SRC_FFVIDEO_MPEG2				= 0X00008000;  
	/** Video Mpeg1 File Format */
	public static final int VOOSMP_SRC_FFVIDEO_MPEG1				= 0X00010000;   
	/** Video Motion Jpeg File Format */
	public static final int VOOSMP_SRC_FFVIDEO_MJPEG				= 0X00020000;  
	/** Video raw data (RGB, YUV) Format */	
	public static final int VOOSMP_SRC_FFVIDEO_RAWDATA				= 0X00040000;   
	/** Audio AAC File Format */
	public static final int VOOSMP_SRC_FFAUDIO_AAC					= 0X00080000;   
	/** Audio AMR NB File Format */
	public static final int VOOSMP_SRC_FFAUDIO_AMR					= 0X00100000;   
	/** Audio AMR WB File Format */
	public static final int VOOSMP_SRC_FFAUDIO_AWB					= 0X00200000;   
	/** Audio MP3 File Format */
	public static final int VOOSMP_SRC_FFAUDIO_MP3					= 0X00400000;   
	/** Audio QCP (QCELP or EVRC) File Format */
	public static final int VOOSMP_SRC_FFAUDIO_QCP					= 0X00800000;   
	/** Audio WAVE File Format */
	public static final int VOOSMP_SRC_FFAUDIO_WAV					= 0X01000000;   
	/** Audio WMA File Format */
	public static final int VOOSMP_SRC_FFAUDIO_WMA					= 0X02000000;   
	/** Audio MIDI File Format */
	public static final int VOOSMP_SRC_FFAUDIO_MIDI					= 0X04000000;   
	/** Audio OGG File Format */
	public static final int VOOSMP_SRC_FFAUDIO_OGG					= 0x08000001;	
	/** Audio FLAC File Format */
	public static final int VOOSMP_SRC_FFAUDIO_FLAC					= 0x08000002;	
	/** Audio WAVE File Format */
	public static final int VOOSMP_SRC_FFAUDIO_AU					= 0X08000004;   
	/** Audio APE file format */
	public static final int VOOSMP_SRC_FFAUDIO_APE					= 0X08000008;	
	/** Audio ALAC file format */
	public static final int VOOSMP_SRC_FFAUDIO_ALAC					= 0X08000010;	
	/** Audio AC3 file format        */
	public static final int VOOSMP_SRC_FFAUDIO_AC3					= 0X08000020;   
	/** Audio PCM file format        */
	public static final int VOOSMP_SRC_FFAUDIO_PCM					= 0X08000040;   
	/** Audio DTS file format        */
	public static final int VOOSMP_SRC_FFAUDIO_DTS					= 0X08000080;   
	/** RTSP Streaming */
	public static final int VOOSMP_SRC_FFSTREAMING_RTSP				= 0X08000100;   
	/** Session description */
	public static final int VOOSMP_SRC_FFSTREMAING_SDP				= 0X08000200;   
	/** Http Live Streaming */
	public static final int VOOSMP_SRC_FFSTREAMING_HLS				= 0X08000400;	
	/** Silverlight Smooth Streaming */
	public static final int VOOSMP_SRC_FFSTREAMING_SSSTR			= 0X08000800;	
	/** DASH Streaming */
	public static final int VOOSMP_SRC_FFSTREAMING_DASH				= 0X08001000;	
	/** PD Streaming */
	public static final int VOOSMP_SRC_FFSTREAMING_HTTPPD			= 0X08002000;
	/** CMMB Streaming */
	public static final int VOOSMP_SRC_FFSTREAMING_CMMB             = 0X08004000;
	/** Video H265 File Format */
	public static final int VOOSMP_SRC_FFVIDEO_H265			        = 0X08008000;

	
	
	/**
	 *  Emuneration of track select type
	 *  VOOSMP_SRC_TRACK_SELECT
	 * 
	 */
	/**VOOSMP_SRC_TRACK_SELECT: selectable*/
	public static final int VOOSMP_SRC_TRACK_SELECT_SELECTABLE		= 0X00000000;
	
	/**VOOSMP_SRC_TRACK_SELECT: recommend*/
	public static final int VOOSMP_SRC_TRACK_SELECT_RECOMMEND		= 0X00000001;
	
	/**VOOSMP_SRC_TRACK_SELECT: selected*/
	public static final int VOOSMP_SRC_TRACK_SELECT_SELECTED		= 0X00000002;
	
	/**VOOSMP_SRC_TRACK_SELECT: disable*/
	public static final int VOOSMP_SRC_TRACK_SELECT_DISABLE			= 0X00000004;
	
	/**VOOSMP_SRC_TRACK_SELECT: default*/
	public static final int VOOSMP_SRC_TRACK_SELECT_DEFAULT			= 0X00000008;
	
	/**VOOSMP_SRC_TRACK_SELECT: force*/
	public static final int VOOSMP_SRC_TRACK_SELECT_FORCE			= 0X00000010;
	
	
	/**
	 *definition of HTTP status, 
	 *VOOSMP_SRC_HTTP_STATUS
	 */
	public static final int VOOSMP_SRC_HTTP_BEGIN 					= 0;
	public static final int VOOSMP_SRC_HTTP_SOCKETCONNECTED			= 1;
	public static final int VOOSMP_SRC_HTTP_REQUESTPREPARED			= 2;
	public static final int VOOSMP_SRC_HTTP_REQUESTSEND				= 3;
	public static final int VOOSMP_SRC_HTTP_RESPONSERECVED			= 4;
	public static final int VOOSMP_SRC_HTTP_RESPONSEANALYSED		= 5;
	
	
	/**
	 * Emuneration of source basic in
	 */
	/**Callback param 2 will be the new bitrate, Integer type */
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_BITRATE_CHANGE       = 1;
	
	/**Callback param 2 will be the new media type defined in following value, Integer type 
	 * <ul>
	 * <li>{@link #VOOSMP_AVAILABLE_PUREAUDIO}
	 * <li>{@link #VOOSMP_AVAILABLE_PUREVIDEO}
	 * <li>{@link #VOOSMP_AVAILABLE_AUDIOVIDEO}
	 * </ul>
	 * */
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_MEDIATYPE_CHANGE     = 2;
	
	/**VOOSMPListener callabck param 2 will be the program type defined in {@link VOOSMP_SRC_PROGRAM_TYPE}, Integer type
	 */
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PROGRAM_TYPE         = 3;
	
	/**Callabck param obj will be {@link voOSChunkInfo} , 
	 * if the return value of this callback is {@link #VOOSMP_SRC_ERR_CHUNK_SKIP},  source must drop this chunk 
	 * This Event is sync callback by onRequestListener, you should return a value.*/
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_BEGINDOWNLOAD  = 4;    
	
	/**Callabck param obj will be {@link voOSChunkInfo} */
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DROPPED        = 5;
	
	/**Callabck param obj will be {@link voOSChunkInfo} */
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DOWNLOADOK     = 6;
	
	/**Indicate Playlist download OK */
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PLAYLIST_DOWNLOADOK  = 7;      
	
	/**Indicates that Program has been changed in source. When this event is received, you should get program info again. Parameter 2 shall be ignored */
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PROGRAM_CHANGE       = 8;
	
	/**Indicates chunk is supported */
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_FILE_FORMATSUPPORTED = 9;
	 /**Indicates live clip can be seeked now*/
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_LIVESEEKABLE         = 10;

	 /**indicate this is the first sample from the discontinue chunk*/
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_DISCONTINUE_SAMPLE   = 11;

	
	/**
	 * Emuneration of chunk type
	 * VOOSMP_SRC_CHUNK_TYPE
	 */
	public static final int VOOSMP_SRC_CHUNK_AUDIO					= 0;
	
	public static final int VOOSMP_SRC_CHUNK_VIDEO					= 1;
	
	public static final int VOOSMP_SRC_CHUNK_AUDIOVIDEO				= 2;
	
	public static final int VOOSMP_SRC_CHUNK_HEADDATA				= 3;
	
	public static final int VOOSMP_SRC_CHUNK_SUBTITLE				= 4;
	
	public static final int VOOSMP_SRC_CHUNK_UNKNOWN 				= 255;

	
	/**
	 * Emuneration of adapter streaming error
	 */
	/**Notify playlist parse failed */
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_PLAYLIST_PARSEFAIL        	= 1;
	
	/**Notify playlist not support */
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_PLAYLIST_UNSUPPORTED      	= 2;
	
    /**Notify stream not support */
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_UNSUPPORTED      	= 3;
	
    /**Notify download failed */
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_DOWNLOADFAIL     	= 4;
	
	/**Notify DRM error */
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_DRMLICENSEERROR     = 5;

 	/**Notify License error */
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_VOLIBLICENSEERROR   = 6;
    
	
	/**
	 * Emuneration of RTSP error
	 */
	/**Notify connect failed in RTSP*/
	public static final int VOOSMP_SRC_RTSP_ERROR_CONNECT_FAIL                                 		= 1;
	
	/**Notify describle failed in RTSP*/
	public static final int VOOSMP_SRC_RTSP_ERROR_DESCRIBE_FAIL										= 2;
	
	/**Notify setup failed in RTSP*/
	public static final int VOOSMP_SRC_RTSP_ERROR_SETUP_FAIL                                    	= 3;
	
	/**Notify play failed in RTSP*/
	public static final int VOOSMP_SRC_RTSP_ERROR_PLAY_FAIL                                     	= 4;
	
	/**Notify pause failed in RTSP*/
	public static final int VOOSMP_SRC_RTSP_ERROR_PAUSE_FAIL                                    	= 5;
	
	/**Notify option failed in RTSP*/
	public static final int VOOSMP_SRC_RTSP_ERROR_OPTION_FAIL                                   	= 6;
	
	/**Notify socket error in RTP channel of RTSP */
	public static final int VOOSMP_SRC_RTSP_ERROR_SOCKET_ERROR    									= 7;
	
	
	/**
	 * Emuneration of source warning
	 */
	/**Notify chunk download failed */
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_DOWNLOADERROR         = 1;
	
	/**Notify chunk formatnot support */
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_FILEFORMATUNSUPPORTED = 2;
	
	/**Notify DRM error */
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_DRMERROR     			= 3;
	
	/**Notify playlist download failed */
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_PLAYLIST_DOWNLOADERROR      = 4;

	/**
	 * Enum of BA mode
	 * VOOSMP_SRC_ADAPTIVESTREAMING_BA_MODE
	 */
	/**BA mode is auto adapt*/
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_BA_MODE_AUTO      = 0;
	/**BA mode is manually adapt*/
	public static final int VOOSMP_SRC_ADAPTIVE_STREAMING_BA_MODE_MANUAL    = 1;
	



	/**
	 *Enumeration used to define the possible source stream type.
	 */
	public enum VOOSMP_SOURCE_STREAMTYPE
	{
		VOOSMP_SS_NONE						(0),
		/** the source stream is Audio  */
		VOOSMP_SS_AUDIO						(0X00000001),					
		/** the source stream is Video  */
		VOOSMP_SS_VIDEO						(0X00000002),	
		/** image track*/
		VOOSMP_SS_IMAGE						(0X00000003),		
		/** stream track*/
		VOOSMP_SS_STREAM					(0X00000004),
		/** script track*/
		VOOSMP_SS_SCRIPT					(0X00000005),			
		/** hint track*/
		VOOSMP_SS_HINT						(0X00000006),		
		/** rtsp streaming video track*/
		VOOSMP_SS_RTSP_VIDEO				(0X00000007),         
		/** rtsp streaming audio track*/
		VOOSMP_SS_RTSP_AUDIO				(0X00000008),          	
		/** sub title track & closed caption*/
		VOOSMP_SS_SUBTITLE					(0X00000009),		
		/** rich media track*/
		VOOSMP_SS_RICHMEDIA					(0X0000000A),		
		/** track info, refer to {@link voOSTrackInfo} */	
		VOOSMP_SS_TRACKINFO					(0X0000000B),	
		/** the source stream is TS  */
		VOOSMP_SS_TS						(0X0000000C),	
		/** the source stream is MFS  */
		VOOSMP_SS_MFS						(0X0000000D),
		/** Source stream is audio, number of tracks is unknown */
		VOOSMP_SS_AUDIO_GROUP		        (0X0000000E),
		/** Source stream is video, number of tracks is unknown */
		VOOSMP_SS_VIDEO_GROUP		        (0X0000000F),
		/** Source stream is subtitle, number of tracks is unknown */
		VOOSMP_SS_SUBTITLE_GROUP	        (0X00000010),
		/** Source format of stream is unknown */
		VOOSMP_SS_MUX_GROUP			        (0X00000011),
		   
		/** the max value definition */
		VOOSMP_SS_MAX						(0X7FFFFFFF);
		
		private int value;
		
		VOOSMP_SOURCE_STREAMTYPE(int event)
		{
			value = event;
		}
		
		public int getValue()
		{
			return value;
		}
		
		public static VOOSMP_SOURCE_STREAMTYPE valueOf(int value)
		{
			if (value < VOOSMP_SOURCE_STREAMTYPE.values().length)
				return VOOSMP_SOURCE_STREAMTYPE.values()[value];
			
			return VOOSMP_SS_MAX;
		}
	};

	/**
	 * Emuneration of program type
	 */	
	public enum VOOSMP_SRC_PROGRAM_TYPE
	{
		VOOSMP_SRC_PROGRAM_TYPE_LIVE						(0),
		VOOSMP_SRC_PROGRAM_TYPE_VOD						(0X00000001);
		private int value;
		
		VOOSMP_SRC_PROGRAM_TYPE (int type)
		{
			value = type;	
		}
		public static VOOSMP_SRC_PROGRAM_TYPE valueOf(int value)
		{
			if (value < VOOSMP_SRC_PROGRAM_TYPE.values().length)
				return VOOSMP_SRC_PROGRAM_TYPE.values()[value];
			
			return VOOSMP_SRC_PROGRAM_TYPE_VOD;
		}
		public int getValue()
		{
			return value;
		}
	};
	
	public enum VOOSMP_SEI_EVENT_FLAG
	 {
		/**ndicate no SEI info */
		VOOSMP_FLAG_SEI_EVENT_NONE                   (0X00000000),      
		/**Indicate picture info, obj param refer to {@link voOSSEIPicTiming}*/
	    VOOSMP_FLAG_SEI_EVENT_PIC_TIMING             (0X00000001),
	    /**Indicate to process User data unregistered ,refer to {@link voOSSEIUserData}*/
	    VOOSMP_FLAG_SEI_USER_DATA_UNREGISTERED		 (0X00000002),
	    /**<Max value definition */
	    VOOSMP_FLAG_SEI_EVENT_MAX                    (0X7FFFFFFF);
		
		private int value;
		
		VOOSMP_SEI_EVENT_FLAG(int event)
		{
			value = event;
		}
		
		public int getValue()
		{
			return value;
		}
		
		public static VOOSMP_SEI_EVENT_FLAG valueOf(int value)
        {
            for (int i = 0; i < VOOSMP_SEI_EVENT_FLAG.values().length; i ++)
            {
                    if (VOOSMP_SEI_EVENT_FLAG.values()[i].getValue() == value)
                            return VOOSMP_SEI_EVENT_FLAG.values()[i];
            }

            voLog.e(TAG, "VOOSMP_SEI_EVENT_FLAG does not match. id = " + Integer.toHexString(value));
            return VOOSMP_FLAG_SEI_EVENT_MAX;
        }
	 }
	
	/**
	* Enumeration of audio decoder types
	*/
	public enum VOOSMP_AUDIO_DECODER_TYPE
	{
	    VOOSMP_AUDIO_DECODER_TYPE_SOFTWARE         (0x00000000), /*!< Using VisualOn software decoder */
	    VOOSMP_AUDIO_DECODER_TYPE_MEDIACODEC       (0x00000001), /*!< Using MediaCodec audio decoder,only available on Android */
	    VOOSMP_AUDIO_DECODER_TYPE_MAX              (0X7FFFFFFF); /*!< Max value definition */
	    private int value;
	    VOOSMP_AUDIO_DECODER_TYPE(int event)
        {
            value = event;
        }
	    public int getValue()
        {
            return value;
        }
	    public static VOOSMP_AUDIO_DECODER_TYPE valueOf(int value)
        {
            if (value < VOOSMP_AUDIO_DECODER_TYPE.values().length)
                return VOOSMP_AUDIO_DECODER_TYPE.values()[value];
            
            return VOOSMP_AUDIO_DECODER_TYPE_MAX;
        }
	};
	
	/**
	 * 
	 * voSubtitleImageType
	 *
	 */
	public enum VOOSMP_IMAGE_TYPE
	{
		VOOSMP_IMAGE_Unused                          (0x00000000),
		VOOSMP_IMAGE_RGB565                          (0x00000001),
		VOOSMP_IMAGE_RGB24                           (0x00000002),
		VOOSMP_IMAGE_RGBA32                          (0x00000003),
		VOOSMP_IMAGE_ARGB32                          (0x00000004),
		VOOSMP_IMAGE_JPEG                            (0x00000005),
		VOOSMP_IMAGE_BMP                             (0x00000006),
		VOOSMP_IMAGE_PNG                             (0x00000007),
		VOOSMP_IMAGE_MAX                             (0x7fffffff);
		
		private int value;
		
		VOOSMP_IMAGE_TYPE(int event)
		{
			value = event;
		}
		
		public int getValue()
		{
			return value;
		}
		
		public static VOOSMP_IMAGE_TYPE valueOf(int value)
		{
			if (value < VOOSMP_IMAGE_TYPE.values().length)
				return VOOSMP_IMAGE_TYPE.values()[value];
			
			return VOOSMP_IMAGE_MAX;
		}
	}
	
	public enum VOOSMP_SRC_AUTHENTICATION_REQUEST_TYPE
	{
		VOOSMP_SRC_AUTHENTICATION_HTTP_BEGIN                    (0x10000000),
		VOOSMP_SRC_AUTHENTICATION_HTTP_SOCKET_CONNECTED         (0x10000001),
		VOOSMP_SRC_AUTHENTICATION_HTTP_REQUEST_PREPARED         (0x10000002),
		VOOSMP_SRC_AUTHENTICATION_HTTP_REQUEST_SEND             (0x10000003),
		VOOSMP_SRC_AUTHENTICATION_HTTP_RESPONSE_RECVED          (0x10000004),
		VOOSMP_SRC_AUTHENTICATION_HTTP_RESPONSE_ANALYSED        (0x10000005),
	    VOOSMP_SRC_AUTHENTICATION_DRM_REQUEST_SERVER_INFO       (0x20000000),
	    
		VOOSMP_SRC_AUTHENTICATION_REQUEST_TYPE_MAX              (0x7FFFFFFF);
		
		private int value;
		
		VOOSMP_SRC_AUTHENTICATION_REQUEST_TYPE(int value)
		{
			this.value = value;
		}
		
		public int getValue()
		{
			return value;
		}
		
		public static VOOSMP_SRC_AUTHENTICATION_REQUEST_TYPE valueOf(int value)
		{
			for (int i = 0; i < VOOSMP_SRC_AUTHENTICATION_REQUEST_TYPE.values().length; i++)
			{
				if (value == VOOSMP_SRC_AUTHENTICATION_REQUEST_TYPE.values()[i].getValue())
					return VOOSMP_SRC_AUTHENTICATION_REQUEST_TYPE.values()[i];
			}
			
			return VOOSMP_SRC_AUTHENTICATION_REQUEST_TYPE_MAX;
		}
	}

	/** * Enumeration of module type */
	public enum VOOSMP_MODULE_TYPE
	{    
		VOOSMP_MODULE_TYPE_SDK          (0X00000000),        /*!< The whole OSMP+ SDK */    
		VOOSMP_MODULE_TYPE_DRM_VENDOR_A	(0X00000001),        /*!< The version of DRM vendor A */
		VOOSMP_MODULE_TYPE_MAX          (0X7FFFFFFF);        /*!< Max value definition */

		private int value;
		VOOSMP_MODULE_TYPE(int value)
		{
			this.value = value;
		}
		
		public int getValue()
		{
			return value;
		}
		
		public static VOOSMP_MODULE_TYPE valueOf(int value)
		{
			for (int i = 0; i < VOOSMP_MODULE_TYPE.values().length; i++)
			{
				if (value == VOOSMP_MODULE_TYPE.values()[i].getValue())
					return VOOSMP_MODULE_TYPE.values()[i];
			}
			
			return VOOSMP_MODULE_TYPE_MAX;
		
		}
	}
	
	/**
     * Enumeration of language type
     * VOOSMP_LANGUAGE_TYPE
     */
    public enum VOOSMP_LANGUAGE_TYPE {
        /** Chinese */
        VOOSMP_LANGUAGE_CHI                      	(0x00000000),
        /** English */
        VOOSMP_LANGUAGE_ENG                       	(0x00000001),
        /** French */
        VOOSMP_LANGUAGE_FRA                       	(0x00000002),
        /** Swedish */
        VOOSMP_LANGUAGE_SWE                       	(0x00000003),
        
        /** Max value definition */
        VOOSMP_LANGUAGE_MAX  						(0xFFFFFFFF);
        
        private int value;
		
        VOOSMP_LANGUAGE_TYPE(int event)
		{
			value = event;
		}
		
		public int getValue()
		{
			return value;
		}
        
		public static VOOSMP_LANGUAGE_TYPE valueOf(int value)
        {
            for (int i = 0; i < VOOSMP_LANGUAGE_TYPE.values().length; i ++)
            {
                    if (VOOSMP_LANGUAGE_TYPE.values()[i].getValue() == value)
                            return VOOSMP_LANGUAGE_TYPE.values()[i];
            }

            voLog.e(TAG, "VO_OSMPLANGUAGE_TYPE does not match. id = " + Integer.toHexString(value));
            return VOOSMP_LANGUAGE_MAX;
        }
    }
}
