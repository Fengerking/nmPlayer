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

#ifndef __vomeAPI_H__
#define __vomeAPI_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "OMX_Types.h"
#include "OMX_Core.h"
#include "OMX_Component.h"
#include "OMX_ContentPipe.h"
#include "OMX_Video.h"

#include "voOMX_Types.h"
//#include "vomeFilePipe.h"
#include "voOMX_FilePipe.h"	
#include "voOMX_Drm.h"	

/**
 * Definition call back ID
 */
#define	VOME_CID_STATUS_CHANGED			0X01000000	/*!<The statusw was changed. nParam1 is new status */
#define	VOME_CID_PLAY_FINISHED			0X01000001	/*!<Play file was finished */
#define	VOME_CID_PLAY_BUFFERING			0X01000002	/*!<The buffer status 0 - 100.the pParam1 is OMX_U32 * */
#define	VOME_CID_PLAY_ERROR				0X01000003	/*!<Play file was error */
#define	VOME_CID_PLAY_BUFFERSTART		0X01000004	/*!<The buffering start  * */
#define	VOME_CID_PLAY_BUFFERSTOP		0X01000005	/*!<The buffering stop * */
#define	VOME_CID_PLAY_DOWNLOADPOS		0X01000006	/*!<The downloading position * */
#define VOME_CID_PLAY_PACKETLOST		0X01000007	/*!<packet lost detected * */
#define VOME_CID_PLAY_CODECPASSABLEERR	0X01000008	/*!<codec detect error data, but not fatal error * */
#define VOME_CID_PLAY_MEDIACONTENTTYPE	0X01000009	/*!<media content type * */
#define VOME_CID_PLAY_MEDIAFOURCC		0X0100000A	/*!<media fourcc, pParam1 0 - audio, 1 - video; pParam2 fourcc */
#define VOME_CID_PLAY_UNSUPPORTEDFORMAT	0X0100000B	/*!<Unsupported format */
#define VOME_CID_PLAY_UNSUPPORTEDCODEC	0X0100000C	/*!<Unsupported codec */
#define VOME_CID_PLAY_FRAMEDROPPED		0X0100000D	/*!<Frame Dropped */
#define VOME_CID_PLAY_SERVERGOODBYE		0X0100000E	/*!<Server Goodbye */
#define VOME_CID_PLAY_NOTSEEKABLE		0X0100000F  /*!<Not Seekable */
#define VOME_CID_PLAY_STREAMERROREVENT	0X01000010	/*!<Streaming Error Event */
#define VOME_CID_PLAY_STREAMBITRATETHROUGHPUT	0x01000011	/*!<Streaming Bitrate Throughput Event */
#define VOME_CID_PLAY_STREAMRTCPRR		0X01000012	/*!<Streaming RTCP RR */
#define VOME_CID_PLAY_NATIVEWINDOWCOMMAND	0x01000013	/*!<VOME_NATIVEWINDOWCOMMAND* */
#define VOME_CID_PLAY_UNSUPPORTEDAUDIO	0X01000014	/*!<Unsupported audio */
#define VOME_CID_PLAY_UNSUPPORTEDVIDEO	0X01000015	/*!<Unsupported video */
#define VOME_CID_PLAY_3DVIDEOTYPE		0X01000016	/*!<3D video Type,int, depends on h/w deocder and render */


#define VOME_CID_REC_COMPLETE			0X01000021	/*!<reach end of source * */
#define VOME_CID_REC_MAXFILESIZE		0X01000022	/*!<reach the max file size * */
#define VOME_CID_REC_MAXDURATION		0X01000023	/*!<reach the max duration * */

//Rogine add for MFW Self-test
#define VOME_CID_PLAY_ARTIMESTAMP		0X01000101	/*!<audio render time stamp * */
#define VOME_CID_PLAY_VRTIMESTAMP		0X01000102	/*!<video render time stamp * */
#define VOME_CID_PLAY_AUDIOPLAYTIME		0X01000103	/*!<audio play time, should be equal to mediatime * */

#define	VOME_CID_PLAY_DEBUGTEXT			0X01100001	/*!<Output debug string. OMX_STRING	*/

/**
 * Definition parameter ID
 */
#define	VOME_PID_VideoRenderBuffer		0X40000001	/*!<Set the video render buffer callback function. OMX_VO_CHECKRENDERBUFFERTYPE *	*/
#define	VOME_PID_AudioRenderBuffer		0X40000002	/*!<Set the Audio render buffer callback function. OMX_VO_CHECKRENDERBUFFERTYPE *	*/
#define	VOME_PID_VideoDataBuffer		0X40000003	/*!<Set the video data buffer point. OMX_VO_VIDEOBUFFERTYPE *	*/
#define	VOME_PID_VideoMemOperator		0X40000004	/*!<Set the video data buffer point. VO_MEM_VIDEO_OPERATOR *	*/
#define	VOME_PID_WorkingPath			0X40000005	/*!<Set the working path. OMX_STRING	*/
#define	VOME_PID_Flush					0X40000006	/*!<Set Flush all components	*/
#define	VOME_PID_CompConfigFile			0X40000007	/*!<Set the working path. OMX_STRING	*/
#define VOME_PID_CoreConfigFile			0X40000008	/*!<Set the core config. OMX_STRING	*/
#define VOME_PID_ChangeSurface			0X40000009	/*!<Process before surface change. VOME_UPDATESURFACECALLBACK *	*/

#define	VOME_PID_AudioFormat			0X40000011	/*!<Get the Audio format VOME_AUDIOFORMATTYPE *	*/
#define	VOME_PID_VideoSize				0X40000012	/*!<Get the Video Size VOME_VIDEOSIZETYPE * */
#define VOME_PID_VideoRotation			0X40000013	/*!<Get the Video Rotation int * */

//Ferry add for select track
#define VOME_PID_GetTrackCount			0X40000200	/*!<Get the file track number* */
#define VOME_PID_GetTrackInfo			0X40000201	/*!<Get the file track information * */
#define VOME_PID_SelectTrack			0X40000202	/*!<Set the file current playback track * */

/**
*the status of the player
*/
typedef enum{
	VOME_STATUS_Unknown				= 0,			/*!<The status is unknown */
	VOME_STATUS_Loaded				= 1,			/*!<The status is running */
	VOME_STATUS_Running				= 2,			/*!<The status is running */
	VOME_STATUS_Paused				= 3,			/*!<The status is paused */
	VOME_STATUS_Stopped				= 4,			/*!<The status is stopped*/
	VOME_STATUS_MAX					= 0X7FFFFFFF,
}VOMESTATUS;

/**
* The source type
*/
typedef enum{
	VOME_SOURCE_Unknown				= 0,			/*!<The source type is unknown */
	VOME_SOURCE_File				= 1,			/*!<The source type is Local file */
	VOME_SOURCE_Handle				= 2,			/*!<The source type is Handle */
	VOME_SOURCE_ID					= 3,			/*!<The source type is Handle */
	VOME_SOURCE_RTSP				= 4,			/*!<The source type is RTSP */
	VOME_SOURCE_HLS                 = 5,            /*!<The source type is HTTP Live streaming */
	VOME_SOURCE_ISS                 = 6,            /*!<The source type is Smooth streaming */
	VOME_SOURCE_MAX					= 0X7FFFFFFF,
}VOME_SOURCETYPE;

typedef struct VOME_SOURCECONTENTTYPE {
    OMX_U32						nSize;
    OMX_VERSIONTYPE				nVersion;
	OMX_PTR						pSource;
	VOME_SOURCETYPE				nType;
	OMX_S64						nOffset;
	OMX_S64						nLength;
	OMX_VO_FILE_OPERATOR *		pPipe;
	OMX_VO_SOURCEDRM_CALLBACK *	pDrmCB;
	OMX_VO_LIB_OPERATOR *		pLibOP;
	// XXXXXXXX XXXXXXXX XXXXXXXX XXXSPPPP
	// P: play flags(1 - video only; 2 - audio only)
	// S: source component only
    OMX_U32						nFlag;
	OMX_PTR						pHeaders;
    OMX_U32						nReserved;
} VOME_SOURCECONTENTTYPE;

typedef struct VOME_RECORDERFILETYPE  {
    OMX_U32					nSize;
    OMX_VERSIONTYPE			nVersion;
	OMX_VIDEO_CODINGTYPE	nVideoCoding;
	OMX_U32					nWidth;
	OMX_U32					nHeight;
	OMX_U32					nFrameRate;
	OMX_U32					nVideoQuality;
	OMX_AUDIO_CODINGTYPE	nAudioCoding;
	OMX_U32					nSampleRate;
	OMX_U32					nChannels;
	OMX_U32					nAudioQuality;
	OMX_U8					szFileName[128];
} VOME_RECORDERFILETYPE;

typedef struct VOME_RECORDERIMAGETYPE  {
    OMX_U32					nSize;
    OMX_VERSIONTYPE			nVersion;
	OMX_IMAGE_CODINGTYPE	nImageCoding;
	OMX_U32					nWidth;
	OMX_U32					nHeight;
	OMX_U32					nImageQuality;
	OMX_U8					szFileName[128];
} VOME_RECORDERIMAGETYPE;

/**
* The transcode destination type
*/
typedef enum
{
	VOME_TRANSCODE_DEST_File		= 0,			/*!<The transcode destination type is local file, pDest is file path (OMX_STRING) */
	VOME_TRANSCODE_DEST_Callback	= 1,			/*!<The transcode destination type is callback, pDest is OMX_VO_FILESINK_CALLBACK* */
} VOME_TRANSCODEDESTTYPE;

#define VOME_TRANSCODEDESTFLAG_FILESEEKDISABLED		0x00000001	// forbid seek operation when write file

typedef struct VOME_TRANSCODETYPE {
	OMX_U32						nSize;
	OMX_VERSIONTYPE				nVersion;

	// source content
	OMX_PTR						pSource;
	VOME_SOURCETYPE				nSourceType;
	OMX_S64						nSourceOffset;
	OMX_S64						nSourceLength;
	OMX_VO_FILE_OPERATOR *		pFileOP;
	OMX_VO_SOURCEDRM_CALLBACK *	pDrmCB;
	OMX_VO_LIB_OPERATOR *		pLibOP;

	// output video settings
	OMX_VIDEO_CODINGTYPE		nVideoCoding;
	OMX_U32						nVideoWidth;		// output video width, 0 means use source video width
	OMX_U32						nVideoHeight;		// output video height, 0 means use source video height
	OMX_U32						xVideoFrameRate;	// output video frame rate, 0 means use source video frame rate, Q16 format
	OMX_U32						nVideoBitrate;		// output video bitrate, 0 means use similar to source, trancode engine will compute it with resolution change
	OMX_U32						nVideoQuality;		// output video quality, 0 - low; 1 - middle; 2 - high.
	OMX_U32						nVideoReserved;

	// output audio settings
	OMX_AUDIO_CODINGTYPE		nAudioCoding;
	OMX_U32						nAudioSampleRate;	// output audio sample rate, 0 means use source audio sample rate
	OMX_U32						nAudioChannels;		// output audio channel, 0 means use source audio channel
	OMX_U32						nAudioSampleBits;	// output audio sample bit, 0 means use source audio sample bit
	OMX_U32						nAudioBitrate;		// output audio bitrate, 0 means use source audio bitrate
	OMX_U32						nAudioReserved;

	// output content
	OMX_VO_FILE_FORMATTYPE		nDestFormat;		// OMX_VO_FILE_FormatXXX
	VOME_TRANSCODEDESTTYPE		nDestType;			// VOME_TRANSCODE_DEST_XXX
	OMX_PTR						pDest;				// depend on nDestType
	OMX_U32						nFlags;				// VOME_TRANSCODEDESTFLAG_XXX
	OMX_U32						nMaxBlockSize;		// the max size file sink write file once, 0 means file sink can decide it itself
	OMX_U32						nMaxAVDiffTime;		// the max Audio/Video un-interleaved time (ms), 0 means file sink can decide it itself 

	OMX_U32						nReserved;
} VOME_TRANSCODETYPE;

typedef struct VOME_AUDIOFORMATTYPE  {
    OMX_U32					nSize;
    OMX_VERSIONTYPE			nVersion;
	OMX_U32					nSampleRate;
	OMX_U32					nChannels;
	OMX_U32					nSampleBits;
} VOME_AUDIOFORMATTYPE;

typedef struct VOME_VIDEOSIZETYPE  {
    OMX_U32					nSize;
    OMX_VERSIONTYPE			nVersion;
	OMX_U32					nWidth;
	OMX_U32					nHeight;
} VOME_VIDEOSIZETYPE;

typedef enum
{
	// Allocate native buffers and set them to Nvidia Component by VOMEUSEGRAPHICBUFFER callback
	// pParam1: VOME_NATIVEWINDOWALLOCBUFFER*
	// pParam2: not specified
	VOME_NATIVEWINDOW_AllocBuffers		= 1, 

	// Dequeue native buffer from native window
	// pParam1: native buffer pointer pointer, OMX_PTR*
	// pParam2: VOME_NATIVEWINDOWBUFFERHEADERS*
	VOME_NATIVEWINDOW_DequeueBuffer		= 2, 

	// Cancel native buffer to native window, must thread sync with Dequeue
	// We will do nothing if the native buffer is not dequeued
	// pParam1: native buffer pointer, OMX_PTR
	// pParam2: not specified
	VOME_NATIVEWINDOW_CancelBuffer		= 3, 

	// Queue native buffer to native window
	// Actually we do not call it because VOAP_IDC_renderVideo will do it in video sink callback
	// pParam1: native buffer pointer, OMX_PTR
	// pParam2: not specified
	VOME_NATIVEWINDOW_QueueBuffer		= 4, 

	// Lock native buffer so that Nvidia component can write it
	// pParam1: native buffer pointer, OMX_PTR
	// pParam2: not specified
	VOME_NATIVEWINDOW_LockBuffer		= 5, 

	// Set crop to native window when receive OMX_IndexConfigCommonOutputCrop
	// pParam1: VOME_NATIVEWINDOWCROP*
	// pParam2: not specified
	VOME_NATIVEWINDOW_SetCrop			= 6, 

	// Free resource of native buffer
	// if the native buffer dequeued, we will cancel it to native window, must thread sync with Dequeue
	// pParam1: native buffer pointer, OMX_PTR
	// pParam2: not specified
	VOME_NATIVEWINDOW_FreeBuffer		= 7, 

	// Check if native buffer dequeued from native window
	// pParam1: native buffer pointer, OMX_PTR
	// pParam2: OMX_U32*, 1 - dequeued; 0 - not dequeued
	VOME_NATIVEWINDOW_IsBufferDequeued	= 8, 

	// Get minimum number of buffers that must remain un-dequeued after a buffer has been queued
	// pParam1: OMX_U32*, minimum number of buffers that must remain un-dequeued after a buffer has been queued
	// pParam2: not specified
	VOME_NATIVEWINDOW_GetMinUndequeuedBuffers	= 9, 

	// Check if we can use native window
	// pParam1: OMX_BOOL*, OMX_FALSE means surface is NULL
	// pParam2: not specified
	VOME_NATIVEWINDOW_IsUsable = 10, 
} VOME_NATIVEWINDOW_COMMANDID;

typedef struct VOME_NATIVEWINDOWCOMMAND {
	VOME_NATIVEWINDOW_COMMANDID		nCommand;	// Command ID
	OMX_PTR							pParam1;	// Parameter 1
	OMX_PTR							pParam2;	// Parameter 2
	OMX_U32							nReserved;	// Reserved for future
} VOME_NATIVEWINDOWCOMMAND;

typedef OMX_S32 (* VOMEUSEGRAPHICBUFFER) (OMX_PTR pUserData, OMX_PTR pUseAndroidNativeBufferParams);
typedef struct VOME_NATIVEWINDOWALLOCBUFFER {
	OMX_U32					nPortIndex;
	OMX_U32					nWidth;
	OMX_U32					nHeight;
	OMX_U32					nColor;
	OMX_PTR*				ppBufferHeader;
	OMX_U32					nBufferCount;
	VOMEUSEGRAPHICBUFFER	fUseGraphicBuffer;
	OMX_PTR					pUserData;
	OMX_U32					nBufferSize;
	OMX_U32                 mUsages;                // get this value from the OMX component 
	OMX_U32                 mUANBVer;               // OMX.google.android.index.useAndroidNativeBuffer's version
} VOME_NATIVEWINDOWALLOCBUFFER;

typedef struct VOME_NATIVEWINDOWCROP {
	OMX_S32					nLeft;
	OMX_S32					nTop;
	OMX_S32					nRight;
	OMX_S32					nBottom;
} VOME_NATIVEWINDOWCROP;

typedef struct VOME_NATIVEWINDOWBUFFERHEADERS {
	OMX_U32					nBufferCount;		// buffer header count
	OMX_PTR*				ppBufferHeader;		// buffer headers' pointer
} VOME_NATIVEWINDOWBUFFERHEADERS;

typedef void (* VOMEUpdateSurfaceCallBack) (OMX_PTR pUserData);

typedef struct VOME_UPDATESURFACECALLBACK {
	VOMEUpdateSurfaceCallBack	fCallBack;
	OMX_PTR						pUserData;
} VOME_UPDATESURFACECALLBACK;

typedef OMX_S32 (* VOMECallBack) (OMX_PTR pUserData, OMX_S32 nID, OMX_PTR pParam1, OMX_PTR pParam2);

/**
 * VisualOn Meidia Engine function set
 */
typedef struct
{
	OMX_ERRORTYPE (OMX_APIENTRY * Init) (OMX_PTR * pHandle, OMX_U32 uFlag, VOMECallBack pCallBack, OMX_PTR pUserData);
	OMX_ERRORTYPE (OMX_APIENTRY * Uninit) (OMX_PTR Handle);
	OMX_ERRORTYPE (OMX_APIENTRY * SetDisplayArea) (OMX_PTR Handle, OMX_VO_DISPLAYAREATYPE * pDisplay);
	OMX_ERRORTYPE (OMX_APIENTRY * GetParam) (OMX_PTR Handle, OMX_S32 nID, OMX_PTR pValue);
	OMX_ERRORTYPE (OMX_APIENTRY * SetParam) (OMX_PTR Handle, OMX_S32 nID, OMX_PTR pValue);

	OMX_ERRORTYPE (OMX_APIENTRY * Playback) (OMX_PTR Handle, VOME_SOURCECONTENTTYPE * pSource);
	OMX_ERRORTYPE (OMX_APIENTRY * Recorder) (OMX_PTR Handle, VOME_RECORDERFILETYPE * pRecFile);
	OMX_ERRORTYPE (OMX_APIENTRY * Snapshot) (OMX_PTR Handle, VOME_RECORDERIMAGETYPE * pRecImage);
	OMX_ERRORTYPE (OMX_APIENTRY * Close) (OMX_PTR Handle);

	OMX_ERRORTYPE (OMX_APIENTRY * Run) (OMX_PTR Handle);
	OMX_ERRORTYPE (OMX_APIENTRY * Pause) (OMX_PTR Handle);
	OMX_ERRORTYPE (OMX_APIENTRY * Stop) (OMX_PTR Handle);
	OMX_ERRORTYPE (OMX_APIENTRY * GetStatus) (OMX_PTR Handle, VOMESTATUS * pStatus);

	OMX_ERRORTYPE (OMX_APIENTRY * GetDuration) (OMX_PTR Handle, OMX_S32 * pDuration);
	OMX_ERRORTYPE (OMX_APIENTRY * GetCurPos) (OMX_PTR Handle, OMX_S32 * pCurPos);
	OMX_ERRORTYPE (OMX_APIENTRY * SetCurPos) (OMX_PTR Handle, OMX_S32 nCurPos);

	OMX_ERRORTYPE (OMX_APIENTRY * Transcode) (OMX_PTR Handle, VOME_TRANSCODETYPE * pTranscode);
}VOOMX_ENGINEAPI;

/**
 * VisualOn Meidia Engine OMX handle component function set
 */
typedef struct
{
	OMX_ERRORTYPE (OMX_APIENTRY * SetCompCallBack) (OMX_PTR Handle, OMX_CALLBACKTYPE * pCompCallBack);
	OMX_ERRORTYPE (OMX_APIENTRY * EnumComponentName) (OMX_PTR Handle, OMX_STRING pCompName, OMX_U32 nIndex);
	OMX_ERRORTYPE (OMX_APIENTRY * GetRolesOfComponent) (OMX_PTR Handle, OMX_STRING pCompName, OMX_U32 * pNumRoles, OMX_U8 ** ppRoles);
	OMX_ERRORTYPE (OMX_APIENTRY * GetComponentsOfRole) (OMX_PTR Handle, OMX_STRING pRole, OMX_U32 * pNumComps, OMX_U8 ** ppCompNames);
	OMX_ERRORTYPE (OMX_APIENTRY * AddComponent) (OMX_PTR Handle, OMX_STRING pName, OMX_COMPONENTTYPE ** ppComponent);
	OMX_ERRORTYPE (OMX_APIENTRY * RemoveComponent) (OMX_PTR Handle, OMX_COMPONENTTYPE * pComponent);
	OMX_ERRORTYPE (OMX_APIENTRY * ConnectPorts) (OMX_PTR Handle, OMX_COMPONENTTYPE * pOutputComp, OMX_U32 nOutputPort,
												OMX_COMPONENTTYPE * pInputComp, OMX_U32 nInputPort, OMX_BOOL bTunnel);
	OMX_ERRORTYPE (OMX_APIENTRY * GetComponentByName) (OMX_PTR Handle, OMX_STRING pName, OMX_COMPONENTTYPE ** ppComponent);
	OMX_ERRORTYPE (OMX_APIENTRY * GetComponentByIndex) (OMX_PTR Handle, OMX_U32 nIndex, OMX_COMPONENTTYPE ** ppComponent);

	OMX_ERRORTYPE (OMX_APIENTRY * SaveGraph) (OMX_PTR Handle, OMX_STRING pFile);
	OMX_ERRORTYPE (OMX_APIENTRY * OpenGraph) (OMX_PTR Handle, OMX_STRING pFile);
}VOOMX_COMPONENTAPI;


/**
 * Get voOMX Engine API interface
 * \param pEngine [IN/OUT] Return the Engine API handle.
 * \param uFlag,reserved
 * \retval OMX_ErrorNone Succeeded.
 */
OMX_ERRORTYPE OMX_APIENTRY voOMXGetEngineAPI (VOOMX_ENGINEAPI * pEngine, OMX_U32 uFlag);

typedef OMX_ERRORTYPE (OMX_APIENTRY * VOOMXGETENGINEAPI) (VOOMX_ENGINEAPI * pFunc, OMX_U32 uFlag);

/**
 * Get voOMX component API interface
 * \param pCompFunc [IN/OUT] Return the component API handle.
 * \param uFlag,reserved
 * \retval OMX_ErrorNone Succeeded.
 */
OMX_ERRORTYPE OMX_APIENTRY voOMXGetComponentAPI (VOOMX_COMPONENTAPI * pCompFunc, OMX_U32 uFlag);

typedef OMX_ERRORTYPE (OMX_APIENTRY * VOOMXGETCOMPONENTAPI) (VOOMX_COMPONENTAPI * pFunc, OMX_U32 uFlag);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __vomeAPI_H__
