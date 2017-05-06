/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CEditor.h

Contains:	CEditor header file

Written by:	Leon Huang

Change History (most recent first):
2011-01-05		Leon			Create file
*******************************************************************************/

#ifndef __CEditor_H__
#define __CEditor_H__

#include "CVideoEditorEngine.h"
#include "voedtType.h"

class CEditor
{
public:
	CEditor();
	~CEditor();
	
	CVideoEditorEngine *m_pVideoEditorEngine;
public:
	

	VO_S32 Open(VOEDT_INIT_INFO* pParam);
	VO_S32 Close();

	/*return value is a index of this clip ,greater than 0*/ 
	VO_S32 AddAClip(VO_S32 nPid, VO_VOID* pSource,VO_U32 nFlag);
	VO_S32 DeleteClip(VOEDT_CLIP_HANDLE *pClipHandle);
	VO_S32 GetClipDuration(VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 *duration);
	VO_S32 GetClipPosition(VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 *position);
	VO_S32 GetEditorDuration(VO_U32 *duration);
	VO_S32 GetEditorPosition(VO_U32 *position);
	VO_S32 SetClipPosition(VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 position);
	VO_S32 SetEditorPosition(VO_U32 position);
	VO_S32 GetClipThumbNail(VOEDT_CLIP_HANDLE *pClipHandle,VO_S32 pos);
	VO_S32 GetEditorThumbNail(VO_S32 pos);
	VO_S32 GetEditorAudioSample(VO_S32 pos);
	VO_S32 JumpClipTo(VOEDT_CLIP_HANDLE *pClipHandle,VO_S32 nTargetPos);
	//VO_S32 SetOutputParam(VOEDT_OUTPUT_PARAM *pParam);

	VO_S32 SetClipParam(VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 nPid,VO_VOID *pValue);
	VO_S32 GetClipParam(VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 nPid,VO_VOID *pValue);

	VO_S32 GetTextFormat(VOEDT_CLIP_HANDLE *pClipHandle,VOEDT_TEXT_FORMAT *pFormat);
	VO_S32 SetTextFormat(VOEDT_CLIP_HANDLE *pClipHandle,VOEDT_TEXT_FORMAT *pFormat);
	VO_S32 SetTextContent(VOEDT_CLIP_HANDLE *pClipHandle,VO_PTR pValue,VO_U32 nFlag);

	VO_S32 GetEditorParam(VO_S32 nID,VO_VOID *pValue);
	VO_S32 SetEditorParam(VO_S32 nID,VO_VOID *pValue );
	
	VO_S32 GetCallbackData(VOEDT_CALLBACK_BUFFER *pBuffer);

	/*To Start Play EDITOR, The mode of How to playing(audio/video preview,encoder,filesink) is setted by Function "SetEDITORParam" ,"nActionMode" parameter*/
	VO_S32 PlayEditor();
	//To Stop play EDITOR
	VO_S32 StopEditor();
	VO_S32  LoadConfig(char* pSource);
	VO_S32  SaveConfig(char* pDest);
private:
	VOEDTCALLBACK	m_pCallback;
	VO_VOID*		m_pUserData;


	VO_U32 ReadOPListThread();
	voCMutex m_voMemMutex;
	bool m_bClosed;
	bool m_bFlush;/* Flush Operation List */
	voThreadHandle m_thread;

	int m_nGetThumbNailOP_DefineCounts;
	int m_nSetPositionOP_DefineCounts;
	int m_nGetThumbNailOP_Counts;
	int m_nSetPositionOP_Counts;
public:
	static VO_U32 threadfunc( VO_PTR pParam );
};
#endif
