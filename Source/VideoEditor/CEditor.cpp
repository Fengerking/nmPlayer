/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CEditor.cpp

Contains:	CEditor class file

Written by:	Leon Huang

Change History (most recent first):
2011-01-05		Leon			Create file
*******************************************************************************/

#include "CEditor.h"

CEditor::CEditor()
:m_pVideoEditorEngine(NULL)
,m_pCallback(NULL)
,m_pUserData(NULL)
,m_bClosed(false)
,m_nGetThumbNailOP_DefineCounts(20)
,m_nSetPositionOP_DefineCounts(2)
,m_nGetThumbNailOP_Counts(0)
,m_nSetPositionOP_Counts(0)
{
	
}

CEditor::~CEditor()
{
	Close();
}

VO_S32 CEditor::Open(VOEDT_INIT_INFO* pParam)
{
	m_pVideoEditorEngine = new CVideoEditorEngine;
	if(!m_pVideoEditorEngine ) return VO_ERR_OUTOF_MEMORY;
	m_pUserData = pParam->pUserData;
	m_pCallback = pParam->pCallback;
	m_pVideoEditorEngine->InitVideoEditorEngine(pParam );

	VO_U32 thread_id;
	voThreadHandle hthread;
	voThreadCreate( &hthread , &thread_id , threadfunc , this , 0 );
}

VO_S32 CEditor::Close()
{
	m_bClosed = true;
	while (m_thread != NULL )
	{
		VOLOGI("waiting for stop");
		voOS_Sleep (20);
	}		

	if(!m_pVideoEditorEngine ) delete m_pVideoEditorEngine;
	return VO_ERR_NONE;
}
enum VOEDT_OPERATION_TYPE{
	VOEDT_OP_NULL =0,
	VOEDT_OP_ADD_CLIP,									/* pParam1 indicates nPid(VO_S32*), pParam2 indicates pSource(VO_CHAR* ), pParam3 indicates nFlag (VO_U32*)*/
	VOEDT_OP_GET_CLIP_THUMBNAIL,				/*pParam1 indicates pClipHandle(VOEDT_CLIP_HANDLE*), pParam2 indicates nPos(VO_S32*) */
	VOEDT_OP_GET_EDITOR_THUMBNAIL,		/*pParam1 indicates nPos(VO_S32*) */
	VOEDT_OP_SET_CLIP_POSITION,					/*pParam1 indicates pClipHandle(VOEDT_CLIP_HANDLE*), pParam2 indicates nPos(VO_S32*) */
	VOEDT_OP_SET_EDITOR_POSITION,				/*pParam1 indicates nPos(VO_S32*)  */
	VOEDT_OP_GET_EDITOR_AUDIO_SAMPLE  /*pParam1 indicates nPos(VO_S32*) */

};

typedef struct VOEDT_OPERATION
{
	VO_U32 _type;//indicate VOEDT_OPERATION_TYPE
	VO_PTR pParam1;
	VO_PTR pParam2;
	VO_PTR pParam3;
	VO_PTR pParam4;
};


CObjectList<VOEDT_OPERATION>	m_OperationList;

VO_U32 CEditor::threadfunc( VO_PTR pParam )
{
	CEditor * ptr_obj = ( CEditor * )pParam;
	ptr_obj->m_thread = (voThreadHandle)1;
	ptr_obj->ReadOPListThread();//start_smoothstream();
	ptr_obj->m_thread = 0;
	return 1;
}
VO_U32 CEditor::ReadOPListThread()
{
	while(1)
	{
	

		VO_S32 nRC;
		VOEDT_OPERATION *op = NULL;

		{	
			voOS_Sleep(5);
			voCAutoLock lock (&m_voMemMutex);	
			int counts = m_OperationList.GetCount() ;
			if(m_bClosed && counts <= 0) break;
			else if(counts == 0)
			{ 
				m_bFlush = false;
				continue;
			}

			op = m_OperationList.GetHead();
			if(op->_type ==  VOEDT_OP_GET_EDITOR_THUMBNAIL)
			{
				VO_S32 pos = *((VO_S32*)op->pParam1);
				VOLOGI("GetThumbNail Position:%d",pos);
			}	
			m_OperationList.RemoveHead();
		}
		if( !m_bClosed || !m_bFlush)
		{
			switch(op->_type)
			{
			case VOEDT_OP_ADD_CLIP:
				{
					VO_S32 nPid = *((VO_S32*)op->pParam1); 
					VO_VOID *pSource = op->pParam2;
					VO_U32 nFlag = *((VO_U32*) op->pParam3);
					switch(*((VO_S32*)op->pParam1))
					{
					case VOEDT_PID_CLIP_TYPE_TEXT:
						nRC = m_pVideoEditorEngine->AddTextClip(nPid, pSource, nFlag);
						break;
					case VOEDT_PID_CLIP_TYPE_AUDIO:
					case VOEDT_PID_CLIP_TYPE_VIDEO:
					case VOEDT_PID_CLIP_TYPE_VIDEO | VOEDT_PID_CLIP_TYPE_AUDIO:
						nRC = m_pVideoEditorEngine->AddMediaClip(nPid, pSource, nFlag);
						break;
					case VOEDT_PID_CLIP_TYPE_IMAGE:
						nRC = m_pVideoEditorEngine->AddImgClip(nPid,pSource,nFlag);
						break;
					}
				}
				break;
			case VOEDT_OP_GET_CLIP_THUMBNAIL:
				{
					VOEDT_CLIP_HANDLE *pClipHandle =(VOEDT_CLIP_HANDLE*) op->pParam1;
					VO_S32 pos = *((VO_S32*)op->pParam2);
					nRC = m_pVideoEditorEngine->GetClipThumbNail(pClipHandle,pos);
					op->pParam1 = NULL;
				}
				break;
			case VOEDT_OP_GET_EDITOR_THUMBNAIL:
				{
 					VO_S32 pos = *((VO_S32*)op->pParam1);
					if(m_nGetThumbNailOP_Counts < m_nGetThumbNailOP_DefineCounts)
						nRC = m_pVideoEditorEngine->GetEditorThumbNail(pos);
					else
					{
						VOLOGI("skip a thumbnail");
						nRC = 0;
					}
					m_nGetThumbNailOP_Counts --;
				}
				break;
			case VOEDT_OP_GET_EDITOR_AUDIO_SAMPLE:	 {
					VO_S32 pos = *((VO_S32*)op->pParam1);
					nRC = m_pVideoEditorEngine->GetEditorAudioSample(pos);
				}
				break;
			case VOEDT_OP_SET_CLIP_POSITION:
				{
					VOEDT_CLIP_HANDLE *pClipHandle =(VOEDT_CLIP_HANDLE*) op->pParam1;
					VO_S32 pos = *((VO_S32*)op->pParam2);
					nRC = m_pVideoEditorEngine->SetClipPosition(pClipHandle,pos);
					op->pParam1 = NULL;
				}
				break;
			case VOEDT_OP_SET_EDITOR_POSITION:
				{
					VO_S32 pos = *((VO_S32*)op->pParam1);
					if(m_nSetPositionOP_Counts < m_nSetPositionOP_DefineCounts)
						nRC = m_pVideoEditorEngine->SetEditorPosition(pos);			
					else
						nRC = 0;
					m_nSetPositionOP_Counts --;

				}
				break;
			}//end of switch 
		}//end of if( !m_bClosed)
		if(op->pParam1) delete op->pParam1; 
		if(op->pParam2) delete op->pParam2; 
		if(op->pParam3) delete op->pParam3;
		if(op->pParam4) delete op->pParam4;
		delete op;
	}//end of while(1)
	VOLOGI("Exit Operation Thread !");
	return 1;
}
VO_S32 CEditor::AddAClip(VO_S32 nPid, VO_VOID* pSource,VO_U32 nFlag)
{
	VOLOGI("in AddAClip");
	VO_S32 nRC = VOEDT_ERR_NOT_INIT;

	if(!m_pVideoEditorEngine )
		return nRC;

	VOEDT_OPERATION *op = new VOEDT_OPERATION;
	memset(op, 0 ,sizeof(VOEDT_OPERATION));
	op->_type = VOEDT_OP_ADD_CLIP;
	op->pParam1 =(VO_PTR*) new VO_S32;
	memcpy(op->pParam1, &nPid, sizeof(VO_S32));
	op->pParam2 =(VO_PTR*) new VO_CHAR[255];
	memcpy(op->pParam2, pSource, 255);
	op->pParam3 =(VO_PTR*) new VO_U32;
	memcpy(op->pParam3, &nFlag, sizeof(VO_U32));

	voCAutoLock lock (&m_voMemMutex);
	m_OperationList.AddTail(op);

	return nRC;
}

VO_S32 CEditor::SetEditorPosition(VO_U32 position)
{
	VOLOGI("in SetEditorPosition");
	VO_S32 nRC = VOEDT_ERR_NOT_INIT;

	if(!m_pVideoEditorEngine )
		return nRC;

	VOEDT_OPERATION *op = new VOEDT_OPERATION;
	memset(op, 0 ,sizeof(VOEDT_OPERATION));
	op->_type = VOEDT_OP_SET_EDITOR_POSITION;
	op->pParam1 = (VO_PTR)new VO_S32;
	memcpy(op->pParam1, &position, sizeof(VO_S32));

	voCAutoLock lock (&m_voMemMutex);
	m_OperationList.AddTail(op);
	m_nSetPositionOP_Counts++;
	return nRC;
}

VO_S32 CEditor::SetClipPosition(VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 position)
{
	VOLOGI("in SetClipPosition");
	VO_S32 nRC = VOEDT_ERR_NOT_INIT;
	
	if(!m_pVideoEditorEngine )
		return nRC;
	nRC = m_pVideoEditorEngine->CheckClip(pClipHandle);
	if(nRC != VO_ERR_NONE) return nRC;

	VOEDT_OPERATION *op = new VOEDT_OPERATION;
	memset(op, 0 ,sizeof(VOEDT_OPERATION));
	op->_type = VOEDT_OP_SET_CLIP_POSITION;
	op->pParam1 = pClipHandle;
	op->pParam2 = (VO_PTR)new VO_S32;
	memcpy(op->pParam2, &position, sizeof(VO_S32));

	voCAutoLock lock (&m_voMemMutex);
	m_OperationList.AddTail(op);

	VOLOGI("out SetClipPosition");
	return nRC;
}

VO_S32 CEditor::GetEditorDuration(VO_U32 *duration)
{

		VOLOGI("in GetEditorDuration");
	VO_S32 nRC = VOEDT_ERR_NOT_INIT;
	
	if(!m_pVideoEditorEngine )
		return nRC;

	nRC = m_pVideoEditorEngine->GetEditorDuration(duration);
	return nRC;
}
VO_S32 CEditor::GetClipDuration(VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 *duration)
{
	VOLOGI("in GetClipDuration");
	VO_S32 nRC = VOEDT_ERR_NOT_INIT;

	if(!m_pVideoEditorEngine )
		return nRC;
	nRC = m_pVideoEditorEngine->CheckClip(pClipHandle);
	if(nRC != VO_ERR_NONE) return nRC;

	nRC = m_pVideoEditorEngine->GetClipDuration(pClipHandle,duration);
	return nRC;
}
VO_S32 CEditor::GetEditorPosition(VO_U32 *position)
{
	VOLOGI("in GetEditorPosition");
	VO_S32 nRC = VOEDT_ERR_NOT_INIT;

	if(!m_pVideoEditorEngine )
		return nRC;

	nRC = m_pVideoEditorEngine->GetEditorPosition(position);
	return nRC;
}
VO_S32 CEditor::GetClipPosition(VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 *position)
{
	VOLOGI("in GetClipPosition");
	VO_S32 nRC = VOEDT_ERR_NOT_INIT;

	if(!m_pVideoEditorEngine )
		return nRC;
	nRC = m_pVideoEditorEngine->CheckClip(pClipHandle);
	if(nRC != VO_ERR_NONE) return nRC;

	nRC = m_pVideoEditorEngine->GetClipPosition(pClipHandle,position);
	return nRC;
}
VO_S32 CEditor::GetClipThumbNail(VOEDT_CLIP_HANDLE *pClipHandle,VO_S32 pos)
{
	VOLOGI("in GetClipThumbNail");
	VO_S32 nRC = VOEDT_ERR_NOT_INIT;

	if(!m_pVideoEditorEngine )
		return nRC;
	nRC = m_pVideoEditorEngine->CheckClip(pClipHandle);
	if(nRC != VO_ERR_NONE) return nRC;

	VOEDT_OPERATION *op = new VOEDT_OPERATION;
	memset(op, 0 ,sizeof(VOEDT_OPERATION));
	op->_type = VOEDT_OP_GET_CLIP_THUMBNAIL;
	op->pParam1 = pClipHandle;
	op->pParam2 = (VO_PTR)new VO_S32;
	memcpy(op->pParam2, &pos, sizeof(VO_S32));
	voCAutoLock lock (&m_voMemMutex);
	m_OperationList.AddTail(op);

	return nRC;
}
VO_S32 CEditor::GetEditorAudioSample(VO_S32 pos)
{
	VOLOGI("in GetEditorAudioSample.Position: %d", pos);
	VO_S32 nRC = VOEDT_ERR_NOT_INIT;
	if(!m_pVideoEditorEngine )
		return nRC;

	VOEDT_OPERATION *op = new VOEDT_OPERATION;
	memset(op, 0 ,sizeof(VOEDT_OPERATION));
	op->_type = VOEDT_OP_GET_EDITOR_AUDIO_SAMPLE;
	op->pParam1 = (VO_PTR)new VO_S32;
	memcpy(op->pParam1, &pos, sizeof(VO_S32));
	voCAutoLock lock (&m_voMemMutex);
	m_OperationList.AddTail(op);

	return nRC;
}
VO_S32 CEditor::GetEditorThumbNail(VO_S32 pos)
{
	VOLOGI("in GetEditorThumbNail. Position :%d,List size:%d",pos,m_OperationList.GetCount());

	VO_S32 nRC = VOEDT_ERR_NOT_INIT;

	if(!m_pVideoEditorEngine )
		return nRC;

	
	VOEDT_OPERATION *op = new VOEDT_OPERATION;
	memset(op, 0 ,sizeof(VOEDT_OPERATION));
	op->_type = VOEDT_OP_GET_EDITOR_THUMBNAIL;
	op->pParam1 = (VO_PTR)new VO_S32;
	memcpy(op->pParam1, &pos, sizeof(VO_S32));
	voCAutoLock lock (&m_voMemMutex);
	m_OperationList.AddTail(op);
	m_nGetThumbNailOP_Counts ++;
	VOLOGI("out GetEditorThumbNail. Position :%d,List size:%d, ThumbNailCounts:%d" ,pos,m_OperationList.GetCount(),m_nGetThumbNailOP_Counts );

	return nRC;
}
VO_S32 CEditor::JumpClipTo(VOEDT_CLIP_HANDLE *pClipHandle,VO_S32 nTargetPos)
{

	VOLOGI("in JumpClipTo");

	VO_S32 nRC = VOEDT_ERR_NOT_INIT;

	if(!m_pVideoEditorEngine )
		return nRC;
	nRC = m_pVideoEditorEngine->CheckClip(pClipHandle);
	if(nRC != VO_ERR_NONE) return nRC;
	nRC = m_pVideoEditorEngine->JumpClipTo(pClipHandle,nTargetPos);
	return nRC;

}
VO_S32 CEditor::DeleteClip(VOEDT_CLIP_HANDLE *pClipHandle)
{

	VOLOGI("in DeleteClip");
	VO_S32 nRC = VOEDT_ERR_NOT_INIT;

	if(!m_pVideoEditorEngine )
		return nRC;
	nRC = m_pVideoEditorEngine->CheckClip(pClipHandle);
	if(nRC != VO_ERR_NONE) return nRC;
	nRC = m_pVideoEditorEngine->DeleteClip(pClipHandle);
	return nRC;
}

VO_S32 CEditor::SetTextContent(VOEDT_CLIP_HANDLE *pClipHandle,VO_PTR pValue,VO_U32 nFlag)
{
	VOLOGI("in SetTextContent");
	VO_S32 nRC = VOEDT_ERR_NOT_INIT;

	if(!m_pVideoEditorEngine )
		return nRC;
	nRC = m_pVideoEditorEngine->CheckClip(pClipHandle);
	if(nRC != VO_ERR_NONE) return nRC;
	nRC = m_pVideoEditorEngine->SetTextContent(pClipHandle,pValue,nFlag);
	return nRC;
}
VO_S32 CEditor::SetTextFormat(VOEDT_CLIP_HANDLE *pClipHandle,VOEDT_TEXT_FORMAT *pFormat)
{
	VOLOGI("in SetTextFormat");

	VO_S32 nRC = VOEDT_ERR_NOT_INIT;

	if(!m_pVideoEditorEngine )
		return nRC;
	nRC = m_pVideoEditorEngine->CheckClip(pClipHandle);
	if(nRC != VO_ERR_NONE) return nRC;
	nRC = m_pVideoEditorEngine->SetTextFormat(pClipHandle,pFormat);
	return nRC;
}
VO_S32 CEditor::GetTextFormat(VOEDT_CLIP_HANDLE *pClipHandle,VOEDT_TEXT_FORMAT *pFormat)
{
	VOLOGI("in GetTextFormat");

	VO_S32 nRC = VOEDT_ERR_NOT_INIT;

	if(!m_pVideoEditorEngine )
		return nRC;
	nRC = m_pVideoEditorEngine->CheckClip(pClipHandle);
	if(nRC != VO_ERR_NONE) return nRC;
	nRC = m_pVideoEditorEngine->GetTextFormat(pClipHandle,pFormat);
	return nRC;
}

VO_S32 CEditor::GetClipParam(VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 nPid,VO_VOID *pValue)
{
	VOLOGI("in GetClipParam.ID :%d", nPid);

	VO_S32 nRC = VOEDT_ERR_NOT_INIT;

	if(!m_pVideoEditorEngine )
		return nRC;
	nRC = m_pVideoEditorEngine->CheckClip(pClipHandle);
	if(nRC != VO_ERR_NONE) return nRC;
	nRC = m_pVideoEditorEngine->GetClipParam(pClipHandle,nPid,pValue);
	if(nPid ==VOEDT_PID_CLIP_PARAM )
	{
		VOEDT_CLIP_PARAM *param = (VOEDT_CLIP_PARAM*)pValue;
		VOLOGI("%d,%d,%d,%d",param->point_in,param->point_out,param->beginAtTimeline,param->nPlayMode);
	}
	return nRC;
}
VO_S32 CEditor::SetClipParam(VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 nPid,VO_VOID *pValue)
{
	VOLOGI("in SetClipParam");

	VO_S32 nRC = VOEDT_ERR_NOT_INIT;

	if(!m_pVideoEditorEngine )
		return nRC;
	nRC = m_pVideoEditorEngine->CheckClip(pClipHandle);
	if(nRC != VOEDT_ERR_NONE) return nRC;
	nRC = m_pVideoEditorEngine->SetClipParam(pClipHandle,nPid,pValue);
	return nRC;
}
VO_S32 CEditor::SetEditorParam(VO_S32 nPID,VO_VOID *pValue )
{
	VOLOGI("in SetEditorParam. ID: 0x%08x",nPID);
	VO_S32 nRC =VO_ERR_NONE;
	if(!m_pVideoEditorEngine ) return VOEDT_ERR_NOT_INIT;

	switch(nPID)
	{
	case VOEDT_PID_THUMBNAIL_FORMAT:
		VOLOGI("VOEDT_PID_THUMBNAIL_FORMAT");
		nRC = m_pVideoEditorEngine->SetThumbNailFormat((VOEDT_VIDEO_FORMAT *)pValue);
		break;
	case VOEDT_PID_VIDEO_FORMAT:
		nRC = m_pVideoEditorEngine->SetVideoFormat((VOEDT_VIDEO_FORMAT *)pValue);
		break;
	case VOEDT_PID_AUDIO_FORMAT:
		nRC = m_pVideoEditorEngine->SetAudioFormat((VOEDT_AUDIO_FORMAT *)pValue);
		break;
	case VOEDT_PID_ACTION_MODE:
		nRC = m_pVideoEditorEngine->SetActionMode((VO_S32*)pValue);
		break;
	case VOEDT_PID_OUTPUT_AUDIO_PARAM:
		nRC = m_pVideoEditorEngine->SetAudioOutputParam((VOEDT_OUTPUT_AUDIO_PARAM*)pValue);
		break;
	case VOEDT_PID_OUTPUT_VIDEO_PARAM:
		nRC = m_pVideoEditorEngine->SetVideoOutputParam((VOEDT_OUTPUT_VIDEO_PARAM*)pValue);
		break;
	case VOEDT_PID_OUTPUT_SINK_PARAM:
		nRC = m_pVideoEditorEngine->SetSinkOutputParam((VOEDT_OUTPUT_SINK_PARAM*)pValue);
		break;
	case VOEDT_PID_FRAMERATE:
		VOLOGI("frameRate: %d", *(int*)pValue);
		nRC = m_pVideoEditorEngine->SetFrameRate((int*)pValue);
		break;
	case VOEDT_PID_THUMBNAILCOUNTS_INLIST:
		m_nGetThumbNailOP_DefineCounts = *(int*)pValue;
		nRC = VOEDT_ERR_NONE;
		break;
	case VOEDT_PID_SETPOSCOUNTS_INLIST:
		m_nSetPositionOP_DefineCounts = *(int*)pValue;
		nRC = VOEDT_ERR_NONE;
		break;
	case VOEDT_PID_FLUSH_OP:
		VOLOGI("VOEDT_PID_FLUSH_OP");
		m_bFlush = true;
		nRC = VOEDT_ERR_NONE;
		break;
	}
	return nRC;
}
VO_S32 CEditor::GetEditorParam(VO_S32 nID,VO_VOID *pValue )
{
	VOLOGI("in GetEditorParam");

	VO_S32 nRC =0;
	if(!m_pVideoEditorEngine ) return VOEDT_ERR_NOT_INIT;
	switch(nID)
	{
	case VOEDT_PID_EDITOR_STATUS:
		nRC = m_pVideoEditorEngine->GetEditorStatus((VO_U32*)pValue);
		break;
	case VOEDT_PID_THUMBNAIL_FORMAT:
		nRC = m_pVideoEditorEngine->GetThumbNailFormat((VOEDT_VIDEO_FORMAT *)pValue);
		break;
	case VOEDT_PID_AUDIO_FORMAT:
		nRC = m_pVideoEditorEngine->GetEditorAudioFormat((VOEDT_AUDIO_FORMAT *)pValue);
		break;
	case VOEDT_PID_VIDEO_FORMAT:
		nRC = m_pVideoEditorEngine->GetEditorVideoFormat((VOEDT_VIDEO_FORMAT *)pValue);
		break;
	case VOEDT_PID_ACTION_MODE:
		nRC = m_pVideoEditorEngine->GetActionMode((VO_S32*)pValue);
		break;
	case VOEDT_PID_OUTPUT_AUDIO_PARAM:
		nRC = m_pVideoEditorEngine->GetAudioOutputParam((VOEDT_OUTPUT_AUDIO_PARAM*)pValue);
		break;
	case VOEDT_PID_OUTPUT_VIDEO_PARAM:
		nRC = m_pVideoEditorEngine->GetVideoOutputParam((VOEDT_OUTPUT_VIDEO_PARAM*)pValue);
		break;
	case VOEDT_PID_OUTPUT_SINK_PARAM:
		nRC = m_pVideoEditorEngine->GetSinkOutputParam((VOEDT_OUTPUT_SINK_PARAM*)pValue);
		break;

	}
	return nRC;
}

VO_S32  CEditor::PlayEditor()
{

	VOLOGI("in PlayEditor");

	VO_S32 nRC = VOEDT_ERR_NOT_INIT;

	if(!m_pVideoEditorEngine )
		return nRC;
	nRC = m_pVideoEditorEngine->PlayEditor();
	VOLOGI("nRC: 0x%08x",nRC);
	return nRC;
}
VO_S32  CEditor::StopEditor()
{
	VOLOGI("in StopEditor");

	VO_S32 nRC = VOEDT_ERR_NOT_INIT;

	if(!m_pVideoEditorEngine )
		return nRC;
	nRC = m_pVideoEditorEngine->StopEditor();
	return nRC;
}

VO_S32  CEditor::SaveConfig(char* pDest)
{
	VOLOGI("in SaveConfig");

	VO_S32 nRC = VOEDT_ERR_NOT_INIT;

	if(!m_pVideoEditorEngine )
		return nRC;
	nRC = m_pVideoEditorEngine->SaveConfig(pDest);
	return nRC;
}
VO_S32  CEditor::LoadConfig(char* pSource)
{
	VOLOGI("in LoadConfig");

	VO_S32 nRC = VOEDT_ERR_NOT_INIT;

	if(!m_pVideoEditorEngine )
		return nRC;
	nRC = m_pVideoEditorEngine->LoadConfig(pSource);
	return nRC;
}

VO_S32 CEditor::GetCallbackData(VOEDT_CALLBACK_BUFFER *pBuffer)
{
	VOLOGI("in GetCallbackData");

	VO_S32 nRC = VOEDT_ERR_NOT_INIT;

	if(!m_pVideoEditorEngine )
		return nRC;
	nRC = m_pVideoEditorEngine->GetCallbackData(pBuffer);
	return nRC;
}
