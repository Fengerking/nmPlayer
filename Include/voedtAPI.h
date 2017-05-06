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
 

#ifndef __voedtAPI_H__
#define __voedtAPI_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voedtType.h"


/**
* VIDEO EDITOR function set
*/
typedef struct
{

	VO_S32 (VO_API * Init)(VO_PTR * ppHandle,VOEDT_INIT_INFO* pParam);
	VO_S32 (VO_API * Uninit) (VO_PTR pHandle);
	
	VO_S32 (VO_API *SaveConfig)(VO_PTR pHandle,VO_CHAR* pDest );
	VO_S32 (VO_API *LoadConfig)(VO_PTR pHandle,VO_CHAR* pSource);
	/*add a clip to list .
	/* if AddAClip success,a VOEDT_CLIP_INFO pointer will be pushed out by VOEDTCALLBACK function
	*\and if clip type is VOEDT_CLIP_TYPE_VIDEO, engine will callback a unBlack-ThumbNail. So ,it is necessary to set ThumbNail-Fromat( VOEDT_VIDEO_FORMAT)
	*\using SetEditorParam with PID (VOEDT_PID_THUMBNAIL_FORMAT) before AddAClip operation.
	* \param nPid [in] ,Clip type PID, indicate the clip type(VOEDT_CLIP_TYPE_AUDIO / VOEDT_CLIP_TYPE_VIDEO / VOEDT_CLIP_TYPE_TEXT/VOEDT_PID_CLIP_TYPE_IMAGE)
	*  \param pSource [in] source pointer 
	*  \param nFlag[in] Indate the source type and load mode .Now it is only VOEDT_FLAG_SOURCE_URL /VOEDT_FLAG_SOURCE_NULL
	*/
	VO_S32 (VO_API * AddAClip) (VO_PTR pHandle,VO_S32 nPid, VO_VOID* pSource,VO_U32 nFlag);
	/*DELETE a clip
	* \param pClip [in] the clip handle 
	*/
	VO_S32 (VO_API * DeleteClip) (VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle);

	/*Jump clip's StartTime from Origin Position to target Position
	* \param pClip [in] the clip handle
	* \param nOriginPos [in] the clip's Origin position in whole Editor timeline
	* \param nTargetPos [in] the clip's Target position in whole Editor timeline
	*/
	VO_S32 (VO_API * JumpClipTo) (VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle,int nTargetPos);
	
	/*Get Clip Duration 
	* \param pClip [in] the clip handle
	* \param duration [in/out] the duration pointer
	* if the duration pointer is set NULL ,the value will be callback with VOEDT_CB_CLIP_DURATION id
	*/
	VO_S32 (VO_API * GetClipDuration) (VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 *duration);
	/*Get Editor Duration 
	* \param duration [in/out] the duration pointer
	* if the duration pointer is set NULL ,the value will be callback with VOEDT_CB_EDITOR_DURATION id
	*/
	VO_S32 (VO_API * GetEditorDuration) (VO_PTR pHandle,VO_U32 *duration);

	/*Get Clip Position 
	* \param pClip [in] the clip handle
	* \param position [in/out] the position pointer
	* if the position pointer is set NULL ,the value will be callback with VOEDT_CB_CLIP_POSITION id
	*/
	VO_S32 (VO_API * GetClipPosition) (VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 *position);
	/*Get Editor Position 
	* \param position [in/out] the position pointer
	* if the position pointer is set NULL ,the value will be callback with VOEDT_CB_EDITOR_POSITION id
	*/
	VO_S32 (VO_API * GetEditorPosition) (VO_PTR pHandle,VO_U32 *position);

	/*Set Clip Position
	* \param pClip [in] the clip handle 
	* \param position [in] the position 
	*/
	VO_S32 (VO_API *SetClipPosition) (VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 position);
	/*Set Editor Position
	* \param position [in] the position 
	*/
	VO_S32 (VO_API *SetEditorPosition) (VO_PTR pHandle,VO_U32 position);
	/*set text clip string
	* \param pClip [in] the clip handle 
	*\param pValue[in] the Content data
	*\param nFlag[in]  flag type,to indicate the pValue type (VOEDT_FLAG_SOURCE_URL or VOEDT_FLAG_SOURCE_BUFFER)
	* VOEDT_FLAG_SOURCE_BUFFER: param pValue is a VOEDT_IMG_BUFFER pointer
	* VOEDT_FLAG_SOURCE_URL: param pValue is a VO_CHAR pointer 
	* now only support VOEDT_FLAG_SOURCE_URL
	*/
// 	VO_S32 (VO_API *SetTextContent)  (VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle, VO_PTR pValue,VO_U32 nFlag);
// 	VO_S32 (VO_API *SetTextFormat)    (VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle, VOEDT_TEXT_FORMAT *pParam);
// 	VO_S32 (VO_API *GetTextFormat)    (VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle, VOEDT_TEXT_FORMAT *pParam);
	/*Set the clip param 
	* \param pClipInfo [in] the pointer indicate one clip 
	*\param pParam[in] the pointer of parameter will be set in
	*for video/audio ,the pParam is VOEDT_CLIP_PARAM*
	*for text				the pParam is VOEDT_CLIP_PARAM*
	*/
	VO_S32 (VO_API *SetClipParam) (VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 nPID, VO_VOID *pParam);

	/*Get the clip data 
	* \param pClip [in] the clip handle 
	*\param pParam[out] 
	*/
	//VO_S32 (VO_API *GetClipParam) (VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle, VOEDT_CLIP_PARAM *pParam);
	VO_S32 (VO_API *GetClipParam) (VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 nPID, VO_VOID *pParam);

	/*Get ThumbNail ,ASYNC motion
	* \param pClip [in] the clip handle 
	* \param pos [in] the position you want to get
	* if the ThumbNail has been prepared, the engine will callback a  VOEDT_CB_VIDEO_THUMBNAIL ID.
	* the thumbNail you want to get will be callbacked as VOEDT_VIDEO_BUFFER* with  VOEDT_CB_VIDEO_THUMBNAIL
	* if pParam is NULL ,Then this function will set the position in whole Editor timeline
	*at this event ,the value of nReserve in VOEDT_CALLBACK_BUFFER is the buffersize
	*/
	VO_S32 (VO_API *GetClipThumbNail) (VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 pos);

	VO_S32 (VO_API *GetEditorThumbNail) (VO_PTR pHandle,VO_U32 pos);

	VO_S32 (VO_API *GetEditorAudioSample) (VO_PTR pHandle,VO_U32 pos);
	/*Get Parameter from EditorEngine
	* \param nPID [in] to indicate Parameter's type
	* \param pValue [IN] the special data pointer depend on nPID
	*/
	VO_S32 (VO_API *GetEditorParam) (VO_PTR pHandle,int nPID,VO_VOID* pValue);

	/*Set Parameter from EditorEngine
	* \param nPID [in] to indicate Parameter's type
	* \param pValue [IN] the special data pointer depend on nPID
	*/
	VO_S32 (VO_API *SetEditorParam) (VO_PTR pHandle,int nPID,VO_VOID* pValue);

	/*To Start Play Editor*/
	VO_S32 (VO_API *PlayEditor) (VO_PTR pHandle);

	/*To Stop Play Editor*/
	VO_S32 (VO_API *StopEditor) (VO_PTR pHandle);

	///*Set Output parameter for transcode
	//* \param pParam [in] the VOEDT_OUTPUT_PARAM pointer
	//*/
	//VO_S32 (VO_API *SetOutputParam) (VO_PTR pHandle,int nPID,VO_VOID* pParam);

} VO_VIDEOEDITOR_API;


/**
* Get EDITOR API interface
* \param Editor [out] Return the Editor handle.
* \retval VOEDT_ERR_OK Succeeded.
*/
VO_S32 VO_API voGetEditorAPI(VO_VIDEOEDITOR_API * pEditor);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif


