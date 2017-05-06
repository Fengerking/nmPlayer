

/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		voedtAPI.cpp

Contains:	voedtAPI class file

Written by:   Leon Huang

Change History (most recent first):
2011-01-13		Leon		 Create file
*******************************************************************************/

#include "voedtAPI.h"
#include "CEditor.h"
#ifdef WIN32
#if defined __cplusplus
extern "C" {
#endif
#endif


	VO_S32 VO_API voedtInit(VO_PTR * ppHandle,VOEDT_INIT_INFO* pParam)
	{
		CEditor* pEditor = NULL;
		pEditor = new CEditor;

		if (!pEditor)
		{
			return VO_ERR_FAILED;
		}

		*ppHandle = pEditor;
		pEditor->Open(pParam);
		return VO_ERR_NONE;
	}
	VO_S32 VO_API voedtUninit(VO_PTR pHandle)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;

		CEditor* pEditor = (CEditor*)pHandle;
		pEditor->Close();
		delete pEditor;
		pEditor = NULL;
		return VO_ERR_NONE;
	}
	VO_S32 VO_API voedtSaveConfig(VO_PTR pHandle, char* pDest)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;

		CEditor* pEditor = (CEditor*)pHandle;
		return pEditor->SaveConfig(pDest);
	}
	VO_S32 VO_API voedtLoadConfig(VO_PTR pHandle, char* pSource)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;

		CEditor* pEditor = (CEditor*)pHandle;
		return pEditor->LoadConfig(pSource);
	}
	/*return value is a index of this clip ,greater than 0*/ 
	VO_S32 VO_API voedtAddAClip (VO_PTR pHandle,VO_S32 nPid, VO_VOID* pSource,VO_U32 nFlag)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;

		CEditor* pEditor = (CEditor*)pHandle;
		return pEditor->AddAClip(nPid, pSource, nFlag);
	}

	/*DELETE a clip from list by nIndex returned by AddAClip*/
	VO_S32 VO_API voedtDeleteClip (VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;

		CEditor* pEditor = (CEditor*)pHandle;
		return pEditor->DeleteClip(pClipHandle);
	}
	VO_S32 VO_API  voedtJumpClipTo (VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle,int nTargetPos)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;
		if(!pClipHandle) return VOEDT_ERR_CLIP_NOTFOUND;

		CEditor* pEditor = (CEditor*)pHandle;
		return pEditor->JumpClipTo(pClipHandle,nTargetPos);
	}
	/*Get Duration 
	* \param pParam [in] the clip param pointer
	* if NULL ,Get Editor Duration
	*/
	VO_S32 VO_API voedtGetClipDuration (VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 *duration)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;
		CEditor* pEditor = (CEditor*)pHandle;
		return pEditor->GetClipDuration(pClipHandle,duration);
	}
	VO_S32 VO_API voedtGetClipPosition (VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 *position)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;
		CEditor* pEditor = (CEditor*)pHandle;
		return pEditor->GetClipPosition(pClipHandle,position);
	}
	VO_S32 VO_API voedtGetEditorDuration (VO_PTR pHandle,VO_U32 *duration)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;
		CEditor* pEditor = (CEditor*)pHandle;
		return pEditor->GetEditorDuration(duration);
	}
	VO_S32 VO_API voedtGetEditorPosition (VO_PTR pHandle,VO_U32 *position)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;
		CEditor* pEditor = (CEditor*)pHandle;
		return pEditor->GetEditorPosition(position);
	}
	VO_S32 VO_API voedtSetEditorPosition (VO_PTR pHandle,VO_U32 position)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;
		CEditor* pEditor = (CEditor*)pHandle;
		return pEditor->SetEditorPosition(position);
	}
	VO_S32 VO_API voedtSetClipPosition (VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 position)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;
		CEditor* pEditor = (CEditor*)pHandle;
		return pEditor->SetClipPosition(pClipHandle,position);
	}
	//VO_S32 VO_API voedtSetOutputParam (VO_PTR pHandle,int nPID,void *pParam)
	//{
	//	if(!pHandle)
	//		return VO_ERR_INVALID_ARG;
	//	CEditor* pEditor = (CEditor*)pHandle;
	//	return 0;//pEditor->SetOutputParam(nPID,pParam);
	//}
	VO_S32 VO_API voedtGetClipThumbNail (VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 pos)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;
		CEditor* pEditor = (CEditor*)pHandle;
		return pEditor->GetClipThumbNail(pClipHandle,pos);
	}
	VO_S32 VO_API voedtGetEditorThumbNail (VO_PTR pHandle,VO_U32 pos)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;
		CEditor* pEditor = (CEditor*)pHandle;
		return pEditor->GetEditorThumbNail(pos);
	}
	VO_S32 VO_API voedtGetEditorAudioSample (VO_PTR pHandle,VO_U32 pos)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;
		CEditor* pEditor = (CEditor*)pHandle;
		return pEditor->GetEditorAudioSample(pos);
	}
	VO_S32 VO_API voedtGetEditorParam (VO_PTR pHandle,int nPID,VO_VOID* pValue)
	{
		VOLOGI("In voedtGetEditorParam");
		if(!pHandle)
			return VO_ERR_INVALID_ARG;
		CEditor* pEditor = (CEditor*)pHandle;
		return pEditor->GetEditorParam(nPID,pValue);
	}
	VO_S32 VO_API voedtSetEditorParam (VO_PTR pHandle,int nPID,VO_VOID* pValue)
	{
		VOLOGI("In voedtSetEditorParam");
		if(!pHandle)
			return VO_ERR_INVALID_ARG;
		CEditor* pEditor = (CEditor*)pHandle;
		return pEditor->SetEditorParam(nPID,pValue);
	}
	VO_S32 VO_API voedtSetTextContent(VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle, VO_PTR pValue,VO_U32 nFlag)
	{
		VOLOGI("In voedtSetTextContent");
		if(!pHandle)
			return VO_ERR_INVALID_ARG;
		CEditor* pEditor = (CEditor*)pHandle;
		return pEditor->SetTextContent(pClipHandle,pValue,nFlag);
	}
	VO_S32 VO_API voedtSetTextFormat(VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle, VOEDT_TEXT_FORMAT *pParam)
	{
		VOLOGI("In voedtSetTextFormat");
			if(!pHandle)
				return VO_ERR_INVALID_ARG;
			CEditor* pEditor = (CEditor*)pHandle;
			return pEditor->SetTextFormat(pClipHandle,pParam);
	}
	VO_S32 VO_API voedtGetTextFormat(VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle, VOEDT_TEXT_FORMAT *pParam)
	{
		VOLOGI("In voedtGetTextFormat");
			if(!pHandle)
				return VO_ERR_INVALID_ARG;
			CEditor* pEditor = (CEditor*)pHandle;
			return pEditor->GetTextFormat(pClipHandle,pParam);
	}

	VO_S32 VO_API voedtSetClipParam (VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 nPid,VO_VOID *pParam)
	{
		VOLOGI("In voedtSetClipParam");
		if(!pHandle)
			return VO_ERR_INVALID_ARG;
		CEditor* pEditor = (CEditor*)pHandle;
		return pEditor->SetClipParam(pClipHandle,nPid,pParam);
	}
	VO_S32 VO_API voedtGetClipParam (VO_PTR pHandle,VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 nPid,VO_VOID *pParam)
	{
		VOLOGI("In voedtGetClipParam");
		if(!pHandle)
			return VO_ERR_INVALID_ARG;
		CEditor* pEditor = (CEditor*)pHandle;
		return pEditor->GetClipParam(pClipHandle,nPid,pParam);
	}
	VO_S32 VO_API voedtPlayEditor (VO_PTR pHandle)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;

		CEditor* pEditor = (CEditor*)pHandle;

		return pEditor->PlayEditor();
	}
	/*To Stop Play EDITOR*/
	VO_S32 VO_API voedtStopEditor (VO_PTR pHandle)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;

		CEditor* pEditor = (CEditor*)pHandle;

		return pEditor->StopEditor();
	}
	VO_S32 VO_API voGetEditorAPI(VO_VIDEOEDITOR_API* pHandle)
	{
		pHandle->Init					= voedtInit;
		pHandle->Uninit					= voedtUninit;

		pHandle->AddAClip				= voedtAddAClip;
		pHandle->DeleteClip				= voedtDeleteClip;
		pHandle->JumpClipTo				= voedtJumpClipTo;
		pHandle->GetClipDuration			= voedtGetClipDuration;
		pHandle->GetClipPosition			= voedtGetClipPosition;
		pHandle->SetClipPosition			= voedtSetClipPosition;
		pHandle->GetEditorDuration			= voedtGetEditorDuration;
		pHandle->GetEditorPosition			= voedtGetEditorPosition;
		pHandle->SetEditorPosition			= voedtSetEditorPosition;
		pHandle->GetClipThumbNail			= voedtGetClipThumbNail;
		pHandle->GetEditorThumbNail			= voedtGetEditorThumbNail;
		pHandle->GetEditorAudioSample			= voedtGetEditorAudioSample;

	//	pHandle->SetOutputParam			= voedtSetOutputParam;
		pHandle->GetClipParam			= voedtGetClipParam;
		pHandle->SetClipParam			= voedtSetClipParam;
// 		pHandle->SetTextContent		= voedtSetTextContent;
// 		pHandle->SetTextFormat			= voedtSetTextFormat;
// 		pHandle->GetTextFormat			= voedtGetTextFormat;

		pHandle->GetEditorParam				= voedtGetEditorParam;
		pHandle->SetEditorParam				= voedtSetEditorParam;

	//	pHandle->UpdateData				= voedtUpdateData;
		pHandle->PlayEditor				= voedtPlayEditor;
		pHandle->StopEditor				= voedtStopEditor;
		pHandle->SaveConfig				= voedtSaveConfig;
		pHandle->LoadConfig				= voedtLoadConfig;

		return VO_ERR_NONE;
	}
#ifdef WIN32
#if defined __cplusplus
}
#endif
#endif
