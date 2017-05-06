	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		OMX_VO_Index.h

	Contains:	memory operator function define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

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

	OMX_VO_IndexURL					= 0x7F000001,
	OMX_VO_IndexSourceHandle,						/**< reference: OMX_PTR				*/
	OMX_VO_IndexSourceID,							/**< reference: OMX_U32				*/
	OMX_VO_IndexSourcePipe,							/**< reference: OMX_VO_FILEOP *		*/
	OMX_VO_IndexSourceOffset,						/**< reference: OMX_S64				*/
	OMX_VO_IndexSourceLength,						/**< reference: OMX_S64				*/
	OMX_VO_IndexRenderBufferCallBack,				/**< reference: OMX_VO_CHECKRENDERBUFFERTYPE *   */
	OMX_VO_IndexConfigTimeDuration,					/**< reference: OMX_TIME_CONFIG_TIMESTAMPTYPE *   */
	OMX_VO_IndexConfigDisplayArea,					/**< reference: VOME_DISPLAYAREATYPE *   */
	OMX_VO_IndexVideoBufferType,					/**< reference: OMX_VO_VIDEOBUFFERTYPE *   */
	OMX_VO_IndexSendBuffer,							/**< reference: OMX_BUFFERHEADERTYPE *   */
	OMX_VO_IndexFileFormat,							/**< reference: OMX_VO_FILE_FORMATTYPE *   */
	OMX_VO_IndexFileMaxSize,						/**< reference: OMX_S64 *   */
	OMX_VO_IndexFileMaxDuration,					/**< reference: OMX_S64 *   */
	OMX_VO_IndexVideoDataBuffer,					/**< reference: OMX_VO_VIDEOBUFFERTYPE *   */
	OMX_VO_IndexVideoMemOperator,					/**< reference: OMX_VO_VIDEOBUFFERTYPE *   */
	OMX_VO_IndexStartWallClock,						/**< reference: OMX__U32 1, start, 0 pause   */
	OMX_VO_IndexSourceDrmCallBack,					/**< reference: OMX_VO_SOURCEDRMCALLBACK *   */
	OMX_VO_IndexSourceDrmCommit,					/**< reference: OMX_U32(Reserved) *   */
	OMX_VO_IndexConfigAudioChanConfig,				/**< reference: AUDIO_CHANNEL_CONFIG   */

	OMX_VO_IndexMax = 0x7FFFFFFF

} OMX_VO_INDEXTYPE;

/** @ingroup comp */
typedef enum OMX_VO_EVENTTYPE
{
	OMX_VO_EventStartUnused = 0x7F000000,

	OMX_EventComponentLog					= 0x7F000001,  /**< component show log message */
	OMX_VO_Event_Bufferstatus				= 0x7F000002,  /**< component has detected buffer status 0 - 100 nData1 */
	OMX_VO_Event_StreamError				= 0x7F000003,  /**< component has detected stream error occured */
	OMX_VO_Event_BufferStart				= 0x7F000004,  /**< component has detected stream error occured */
	OMX_VO_Event_BufferStop					= 0x7F000005,  /**< component has detected stream error occured */
	OMX_VO_Event_DownloadPos				= 0x7F000006,  /**< component has detected stream error occured */

	OMX_VO_EventMax = 0x7FFFFFFF
} OMX_VO_EVENTTYPE;

typedef enum OMX_VO_AUDIO_CODINGTYPE {
	OMX_VO_AUDIO_CodingVendorStartUnused	= 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
	OMX_VO_AUDIO_CodingAC3					= 0x7F000001,
	OMX_VO_AUDIO_CodingAMRWBP				= 0x7F000002,
	OMX_VO_AUDIO_CodingDRA					= 0x7F000003,
	OMX_VO_AUDIO_CodingMax					= 0x7FFFFFFF
} OMX_VO_AUDIO_CODINGTYPE;

typedef enum OMX_VO_VIDEO_CODINGTYPE {
	OMX_VO_VIDEO_CodingVendorStartUnused	= 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
	OMX_VO_VIDEO_CodingS263					= 0x7F000001,
	OMX_VO_VIDEO_CodingVP6					= 0x7F000002,
	OMX_VO_VIDEO_CodingDIV3					= 0x7F000003,
	OMX_VO_VIDEO_CodingMax					= 0x7FFFFFFF
} OMX_VO_VIDEO_CODINGTYPE;

typedef enum OMX_VO_FILE_FORMATTYPE {
	OMX_VO_FILE_FormatUnused				= 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
	OMX_VO_FILE_FormatMP4					= 0x7F000001,
	OMX_VO_FILE_FormatASF					= 0x7F000002,
	OMX_VO_FILE_FormatAVI					= 0x7F000003,
	OMX_VO_FILE_FormatReal					= 0x7F000004,
	OMX_VO_FILE_FormatOGG					= 0x7F000005,
	OMX_VO_FILE_FormatMKV					= 0x7F000006,
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

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __OMX_VO_Index_H__
