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

#ifndef __OMX_VO_Index_H__
#define __OMX_VO_Index_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "OMX_Index.h"
#include "voOMX_Types.h"

typedef enum OMX_VO_INDEXTYPE {
	// VisualOn specific area
	OMX_VO_IndexStartUnused = 0x7F000000,

	OMX_VO_IndexSourceURL,							/**< reference: OMX_STRING	*/
	OMX_VO_IndexSourceHandle,						/**< reference: OMX_PTR		*/
	OMX_VO_IndexSourceID,							/**< reference: OMX_U32		*/
	OMX_VO_IndexSourcePipe,							/**< reference: OMX_VO_FILE_OPERATOR *	*/
	OMX_VO_IndexSourceOffset,						/**< reference: OMX_S64 *	*/
	OMX_VO_IndexSourceLength,						/**< reference: OMX_S64	*	*/
	OMX_VO_IndexRenderBufferCallBack,				/**< reference: OMX_VO_CHECKRENDERBUFFERTYPE *	*/
	OMX_VO_IndexConfigTimeDuration,					/**< reference: OMX_TIME_CONFIG_TIMESTAMPTYPE *	*/
	OMX_VO_IndexConfigDisplayArea,					/**< reference: OMX_VO_DISPLAYAREATYPE *	*/
	OMX_VO_IndexVideoBufferType,					/**< reference: OMX_VO_VIDEOBUFFERTYPE *	*/
	OMX_VO_IndexSendBuffer,							/**< reference: OMX_BUFFERHEADERTYPE *	*/
	OMX_VO_IndexFileFormat,							/**< reference: OMX_VO_FILE_FORMATTYPE *	*/
	OMX_VO_IndexFileMaxSize,						/**< reference: OMX_S64 *	*/
	OMX_VO_IndexFileMaxDuration,					/**< reference: OMX_S64 *	*/
	OMX_VO_IndexVideoDataBuffer,					/**< reference: OMX_VO_VIDEOBUFFERTYPE *	*/
	OMX_VO_IndexVideoMemOperator,					/**< reference: OMX_VO_VIDEOBUFFERTYPE *	*/
	OMX_VO_IndexStartWallClock,						/**< reference: OMX_U32 1, start, 0 pause	*/
	OMX_VO_IndexConfigAudioChanConfig,				/**< reference: AUDIO_CHANNEL_CONFIG *	*/
	OMX_VO_IndexSourceDrm,							/**< reference: OMX_VO_CALLBACKTYPE *	*/
	OMX_VO_IndexLibraryOperator,					/**< reference: OMX_VO_CALLBACKTYPE *	*/
	OMX_VO_IndexWorkPath,							/**< reference: OMX_STRING	*/
	OMX_VO_IndexSetPosInPause,						/**< reference: OMX_BOOL *	*/
	OMX_VO_IndexSetTranscodeMode,					/**< reference: OMX_BOOL *	*/
	OMX_VO_IndexSourceType,							/**< reference: OMX_U32 * 0, Live, 1 File	*/
	OMX_VO_IndexTimeStampOwner,						/**< reference: OMX_U32 * 0, unspecified, 1 decoder	*/
	OMX_VO_IndexAudioStreamStop,					/**< reference: OMX_U32 1, stop, 0 running	*/
	OMX_VO_IndexAudioParamDTS,						/**< reference: OMX_VO_AUDIO_PARAM_DTSTYPE *	*/
	OMX_VO_IndexConfigAC3Effect,					/**< reference: OMX_U32 * 0: AUTO, 1: LTRT, 2: LORO	*/
	OMX_VO_IndexSourceHeaders,						/**< reference: OMX_STRING	*/
	OMX_VO_IndexSourceFileFormat,					/**< reference: OMX_U32, VO_FILE_FORMAT *	*/
	OMX_VO_IndexRenderHoldBuffer,					/**< reference: OMX_BOOL *	*/
	OMX_VO_IndexSourceNearKeyFrame,					/**< reference: OMX_VO_SOURCE_NEARKEYFRAME *	*/
	OMX_VO_IndexAudioRenderFixedSampleRate,			/**< reference: OMX_BOOL *	*/
	OMX_VO_IndexVideoRenderTime,					/**< reference: OMX_S64 *   */
	OMX_VO_IndexVideoRotation,						/**< reference: OMX_S32 *   */
	OMX_VO_IndexVideoFPS,							/**< reference: OMX_S32 * unit: f/100s for example 2997 mean 29.97f/s */
	OMX_VO_IndexVideoFrameHeader,					/**< reference: OMX_VO_BUFFER *   */		
	OMX_VO_IndexAudioRenderVideoFirstOutput,		/**< reference: OMX_BOOL *	*/
	OMX_VO_IndexAudioRenderTimeOffset,				/**< reference: OMX_S32 * unit: ms, <0 or >0 */
	OMX_VO_IndexTIHWDecodedDimension,				/**< reference: OMX_S32 * unit: ms, <0 or >0 */
	OMX_VO_IndexForceUseThisBufferNumber,			/**< reference: OMX_U32 *	*/
	OMX_VO_IndexCPUCoreNum,							/**< reference: OMX_U32 * 1 or 2	*/
	OMX_VO_IndexMultiChannelSupport,				/**< reference: OMX_BOOL *	*/
	OMX_VO_IndexAudioParamAC3,						/**< reference: OMX_VO_AUDIO_PARAM_AC3TYPE *	*/
	OMX_VO_IndexVideoUpsideDown,					/**< reference: OMX_BOOL *	*/
	OMX_VO_IndexStreamingType,						/**< reference: OMX_VO_StreamType *	*/
	OMX_VO_IndexWVC1DecType,						/**< reference: OMX_U32 * 0: VO software, 1: QCM hardware(default), 2: TI hardware	*/
	OMX_VO_IndexInterlacedVideoCheck,				/**< reference: OMX_BOOL *	*/
	OMX_VO_IndexCodecFCC,							/**< reference: OMX_U32 *	*/
	OMX_VO_IndexLiveStreamingType,					/**< reference: VO_LIVESRC_FORMATTYPE *	*/
	OMX_VO_IndexBlockPortSettingsChangedEvent,		/**< reference: OMX_BOOL *	*/
	OMX_VO_IndexBlockUnknownCodec,					/**< reference: OMX_BOOL *	<S> Source Component, 
													OMX_TRUE: not create port for unknown codec and Send OMX_VO_Event_UnknownCodecPortBlocked
													OMX_FALSE(default): create port always	*/
	OMX_VO_IndexConfigDTSChanOutMask,				/**< reference: OMX_U32 *	*/	
	OMX_VO_IndexSeekablePosition,					/**< reference: OMX_S64 *	<G> Source Component, 
													OMX_ErrorNone: Push Play and we get seekable position successfully, pComponentParameterStructure indicates seekable position
													OMX_ErrorNotImplemented: source detect non-Push Play (for example local play)
													OMX_ErrorXXX: no index or other error	*/
	OMX_VO_IndexAddRTSPField,						/**< reference: RTSPFieldParam* */
	OMX_VO_IndexVideoS3d,							/**< reference: for possible S3D format type, OMX_VO_S3D_params *	*/
	OMX_VO_IndexOutputConfigData,					/**< reference: OMX_VO_OUTPUTCONFIGDATATYPE *	*/

	OMX_VO_IndexFileMaxAVDiffTime,					/**< reference: OMX_U32 *	*/
	OMX_VO_IndexVideoQuality,						/**< reference: OMX_U32 * 0 - low; 1 - middle; 2 - high */

	// Set DRM API to source component directly
	OMX_VO_IndexOMADrmAPI = 0x7F000100,				/**< reference: OMX_VO_DRM_OMADRM_API *	*/
	OMX_VO_IndexPlayReadyAPI,						/**< reference: OMX_VO_DRM_PLAYREADY_API *	*/
	OMX_VO_IndexFileBasedDRMInterface = 0x7F600010,	/**< reference: IVOFileBasedDRM *	*/
	OMX_VO_IndexDRMWidevine,						/**< reference: IVOWidevineDRM *	*/

	//add by Ferry for select track
	OMX_VO_IndexTrackCount,							/**<reference:OMX_U32 * get track count */
	OMX_VO_IndexTrackInfo,							/**<reference:VOME_TRACKINFO * get track information */
	OMX_VO_IndexSetAudioTrack,						/**<reference:VOME_TRACKSET * set selected audio track */
	OMX_VO_IndexSetVideoTrack,						/**<reference:VOME_TRACKSET * set selected video track */
	OMX_VO_IndexAudioEncodeInfo,					/**<reference:OMX_VO_AUDIO_ENCODE_INFO* , get from source component, set into decoder component */
	OMX_VO_IndexMax = 0x7FFFFFFF

} OMX_VO_INDEXTYPE;

/** @ingroup comp */
typedef enum OMX_VO_EVENTTYPE
{
	OMX_VO_EventStartUnused = 0x7F000000,

	OMX_EventComponentLog					= 0x7F000001,  /**< component show log message */
	OMX_VO_Event_Bufferstatus				= 0x7F000002,  /**< component has detected buffer status 0 - 100 nData1 */
	OMX_VO_Event_StreamError				= 0x7F000003,  /**< component has detected stream error occurred */
	OMX_VO_Event_BufferStart				= 0x7F000004,  /**< component has detected stream error occurred */
	OMX_VO_Event_BufferStop					= 0x7F000005,  /**< component has detected stream error occurred */
	OMX_VO_Event_DownloadPos				= 0x7F000006,  /**< component has detected stream error occurred */
	OMX_VO_Event_PacketLost					= 0x7F000007,  /**< component has detected stream packet lost */
	OMX_VO_Event_CodecPassableError			= 0x7F000008,  /**< component has detected error media data, but not cause codec fatal error */
	OMX_VO_Event_MediaContentType			= 0x7F000009,  /**< component has detected media content type */
	OMX_VO_Event_VideoRenderFrames			= 0x7F000010,  /**< component video render had rend frames  */
	OMX_VO_Event_ComponentComplete			= 0x7F000011,  /**< component reach end of source  */
	OMX_VO_Event_ComponentMaxFileSize		= 0x7F000012,  /**< component reach end of source  */
	OMX_VO_Event_ComponentMaxDuration		= 0x7F000013,  /**< component reach end of source  */
	OMX_VO_Event_MediaFourcc				= 0x7F000014,  /**< component has detected media fourcc	*/
	OMX_VO_Event_FormatUnsupported			= 0x7F000015,  /**< component has detected format unsupported */
	OMX_VO_Event_CodecUnsupported			= 0x7F000016,  /**< component has detected codec unsupported */
	OMX_VO_Event_FrameDropped				= 0x7F000017,  /**< component has detected frame dropped for re-sync */
	OMX_VO_Event_NotSeekable				= 0x7F000018,  /**< component has detected file unsupport seek */
	OMX_VO_Event_StreamBitrateThroughput	= 0x7F000019,  /**< component has detected streaming clip bitrate and throughput */
	OMX_VO_Event_StreamRtcpRR				= 0x7F00001A,  /**< component has detected streaming RTCP RR */
	OMX_VO_Event_NeedLoopPlay				= 0x7F00001B,  /**< component has detected this file need loop play(mainly for OGG ANDROID_LOOP flag) */
	OMX_VO_Event_NeedProxy					= 0x7F00001C,  /**< component has detected this url need proxy setting */
	OMX_VO_Event_RTSPSessionEvent			= 0x7F00001D,  /**< component has received session event from RTSP engine, Param1: VO_RTSP_EVENT_SESSION_XXX, Param2: associated parameter of Param1 */
	OMX_VO_Event_AudioUnsupported			= 0x7F00001E,  /**< component has detected audio unsupported */
	OMX_VO_Event_VideoUnsupported			= 0x7F00001F,  /**< component has detected video unsupported */
	OMX_VO_Event_UnknownCodecPortBlocked	= 0x7F000020,  /**< component has detected unknown codec port has been blocked, Param1: 0 - audio, 1 - video */

	//Rogine add for MFW Self-test
	OMX_VO_Event_AudioRenderTimeStamp		= 0x7F000031,
	OMX_VO_Event_VideoRenderTimeStamp		= 0x7F000032,
	OMX_VO_Event_AudioPlayTime				= 0x7F000033,

	OMX_VO_EventMax = 0x7FFFFFFF
} OMX_VO_EVENTTYPE;

typedef enum OMX_VO_AUDIO_CODINGTYPE {
	OMX_VO_AUDIO_CodingVendorStartUnused	= 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
	OMX_VO_AUDIO_CodingAC3					= 0x7F000001,
	OMX_VO_AUDIO_CodingAMRWBP				= 0x7F000002,
	OMX_VO_AUDIO_CodingDRA					= 0x7F000003,
	OMX_VO_AUDIO_CodingEAC3					= 0x7F000004,
	OMX_VO_AUDIO_CodingAPE					= 0x7F000005,
	OMX_VO_AUDIO_CodingALAC					= 0x7F000006,
	OMX_VO_AUDIO_CodingDTS					= 0x7F000007,
	OMX_VO_AUDIO_CodingFlac					= 0x7F000008,
	OMX_VO_AUDIO_CodingMax					= 0x7FFFFFFF
} OMX_VO_AUDIO_CODINGTYPE;

typedef enum OMX_VO_VIDEO_CODINGTYPE {
	OMX_VO_VIDEO_CodingVendorStartUnused	= 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
	OMX_VO_VIDEO_CodingS263					= 0x7F000001,
	OMX_VO_VIDEO_CodingVP6					= 0x7F000002,
	OMX_VO_VIDEO_CodingDIV3					= 0x7F000003,
	OMX_VO_VIDEO_CodingVP8					= 0x7F000004, 
	OMX_VO_VIDEO_CodingVP7					= 0x7F000005,
	OMX_VO_VIDEO_CodingMax					= 0x7FFFFFFF
} OMX_VO_VIDEO_CODINGTYPE;

typedef enum OMX_VO_VIDEO_WMVFORMATTYPE {
	OMX_VO_VIDEO_WMFFormatVendorStartUnused	= 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
	OMX_VO_VIDEO_WMVFormatWVC1				= 0x7F000001,
	OMX_VO_VIDEO_WMVFormatMax				= 0x7FFFFFFF
} OMX_VO_VIDEO_WMVFORMATTYPE;

typedef enum OMX_VO_VIDEO_DivXFORMATTYPE {
	OMX_VO_VIDEO_DivXVendorStartUnused	= 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
	OMX_VO_VIDEO_DivX311				= 0x7F000001,
	OMX_VO_VIDEO_DivX4					= 0x7F000002,
	OMX_VO_VIDEO_DivX5					= 0x7F000003,
	OMX_VO_VIDEO_DivX6					= 0x7F000004,
	OMX_VO_VIDEO_DivXMax				= 0x7FFFFFFF
} OMX_VO_VIDEO_DivXFORMATTYPE;

typedef enum OMX_VO_FILE_FORMATTYPE {
	OMX_VO_FILE_FormatUnused				= 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
	OMX_VO_FILE_FormatMP4					= 0x7F000001,
	OMX_VO_FILE_FormatASF					= 0x7F000002,
	OMX_VO_FILE_FormatAVI					= 0x7F000003,
	OMX_VO_FILE_FormatReal					= 0x7F000004,
	OMX_VO_FILE_FormatOGG					= 0x7F000005,
	OMX_VO_FILE_FormatMKV					= 0x7F000006,
	OMX_VO_FILE_FormatTS					= 0x7F000007,
	OMX_VO_FILE_FormatMP3					= 0x7F000101,
	OMX_VO_FILE_FormatAAC					= 0x7F000102,
	OMX_VO_FILE_FormatAMRNB					= 0x7F000103,
	OMX_VO_FILE_FormatAMRWB					= 0x7F000104,
	OMX_VO_FILE_FormatQCP					= 0x7F000105,
	OMX_VO_FILE_FormatWAV					= 0x7F000206,
	OMX_VO_FILE_FormatJPEG					= 0x7F000201,
	OMX_VO_FILE_FormatPNG					= 0x7F000202,
	OMX_VO_FILE_FormatBMP					= 0x7F000203,
	OMX_VO_FILE_FormatGIF					= 0x7F000204,
	OMX_VO_FILE_FormatTIF					= 0x7F000205,
	OMX_VO_FILE_FormatMax					= 0x7FFFFFFF
} OMX_VO_FILE_FORMATTYPE;

#define OMX_VO_BUFFERFLAG_NEWSTREAM			0x7F100000

typedef struct 
{
  OMX_BOOL active;
  OMX_U32 mode;
  OMX_U32 fmt;
  OMX_U32 order;
  OMX_U32 subsampling;
} OMX_VO_S3D_params;

typedef struct
{
	OMX_S64					Time;					// <I> specified time stamp, can > end but must > start, otherwise return OMX_ErrorBadParameter
	OMX_S64					PreviousKeyframeTime;	// <O> time stamp of previous key frame, <= Time, 0x7fffffffffffffffll mean no key frame(commonly no such case)
	OMX_S64					NextKeyframeTime;		// <O> time stamp of next key frame, > Time, 0x7fffffffffffffffll mean no key frame(commonly reach end)
} OMX_VO_SOURCE_NEARKEYFRAME;

typedef struct
{
	OMX_U8*					Buffer;
	OMX_U32					Size;
} OMX_VO_BUFFER;

typedef struct
{
	OMX_U32					nDecodedWidth;
	OMX_U32					nDecodedHeight;
	OMX_U32					nRotationDegree;
} OMX_VO_TIHWDecodedDimension;

typedef struct
{
	OMX_STRING				strURL;				// streaming URL
	OMX_U32					nStreamingType;     // streaming type
}OMX_VO_StreamType;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __OMX_VO_Index_H__
