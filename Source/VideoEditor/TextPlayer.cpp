/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		TextPlayer.cpp

Contains:	TextPlayer class file

Written by:	Leon Huang

Change History (most recent first):
2011-01-18		Leon			Create file
*******************************************************************************/
#include "TextPlayer.h"
#ifdef _WIN32
#include "GDITextBlend.h"
#endif


CTextPlayer::CTextPlayer(VO_LIB_OPERATOR* pLibOp)
:CPlayerBaseCtrl(pLibOp)
,m_bChanged(VO_FALSE)
,m_pTextBlend(NULL)
{
	VO_CHAR *font =  "黑体";
	memcpy( m_sTextFormat.cFont ,font,sizeof(m_sTextFormat.cFont));
//	m_sTextFormat.nColor;
	m_sTextFormat.nSize = 20;
	
	m_nClipType = VOEDT_PID_CLIP_TYPE_TEXT;
	m_clip_info.nClipType = m_nClipType;
	m_clip_info.nDuration = 0;
	//m_clip_info.pClip = this;
// #ifdef _WIN32
// 	m_pTextBlend = new CGDITextBlend();
// #else
	m_pTextBlend = new CBaseTextBlend();
//#endif
	m_pImagePlayer = new CImgPlayer(pLibOp);

	//m_sClipParam.targetVFormat = &m_pTextBlend->targetVFormat;
}

CTextPlayer::~CTextPlayer(void)
{
	if(m_pTextBlend) delete m_pTextBlend;
}
VO_S32 CTextPlayer::Init(VO_PTR hView)
{
	return m_pTextBlend->Init(hView);
}
VO_S32 CTextPlayer::BlendText(VO_PBYTE buffer,VO_VIDEO_FORMAT vf)
{
	VOMP_BUFFERTYPE *pTextBuffer;
	int pos = 0;
	m_pImagePlayer->SetPosition(&pTextBuffer,(VO_U32*)&pos,VOEDT_PID_VIDEO_SAMPLE);
	return m_pTextBlend->BlendText((VO_PBYTE)pTextBuffer, (VO_PBYTE)buffer, vf,m_sClipParam.positionFormat);
}
VO_S32 CTextPlayer::SetDataSource(VO_VOID* pSource,VO_S32 nFlag)
{
		memcpy(m_cTitle, (VO_CHAR*)pSource, sizeof(m_cTitle));
		m_pImagePlayer->SetDataSource(pSource,nFlag);
// 	if(nFlag == VOEDT_FLAG_SOURCE_BUFFER)
// 	{
// 		m_pTextBlend->CreateTextBuffer(NULL,(VOEDT_IMG_BUFFER*)pSource);
// 	}
// 	else if(nFlag == VOEDT_FLAG_SOURCE_URL)
// 	{
// 		memcpy(m_cTitle, (VO_CHAR*)pSource, sizeof(m_cTitle));
// 
 //		m_pTextBlend->CreateTextBuffer(m_cTitle,NULL);
// 	}
	m_bChanged = VO_TRUE;
	return VO_ERR_NONE;
}

VO_S32 CTextPlayer::SetClipParam(VO_S32 nPid,VO_VOID *pData)
{

	VO_S32 nRc = VO_ERR_NONE;
	switch(nPid)
	{
	case VOEDT_PID_RELOAD:
		{
			nRc = SetDataSource((VO_VOID*)m_cTitle, 1);
			break;
		}
	case VOEDT_PID_PLAYER_DATA_BUFFER:
		{
			break;
		}
	case VOEDT_PID_CLIP_PARAM:
		{
			VOEDT_CLIP_PARAM  *ppp = (VOEDT_CLIP_PARAM*)(pData);
			memcpy(&m_sClipParam,ppp,sizeof(VOEDT_CLIP_PARAM));
			VOEDT_VIDEO_FORMAT vf;
			m_pImagePlayer->GetClipParam(VOEDT_PID_VIDEO_FORMAT, &vf);
			m_sClipParam.positionFormat.nHeight = vf.nHeight;
			m_sClipParam.positionFormat.nWidth = vf.nWidth;

			break;
		}
	case VOEDT_PID_PLAYER_TEXT_FORMAT:
		{
			memcpy(&m_sTextFormat, pData, sizeof(VOEDT_TEXT_FORMAT));
			nRc = m_pTextBlend->SetFont(&m_sTextFormat);
			break;
		}
	case VOEDT_PID_IMG_BUFFER:
		{
			nRc = m_pTextBlend->CreateTextBuffer(NULL,(VOEDT_IMG_BUFFER*)pData);
			break;
		}
	default:
		nRc = VOEDT_ERR_PID_NOTFOUND;
		break;
	}
	return nRc;
}
VO_S32 CTextPlayer::GetClipParam(VO_S32 nPid,VO_VOID *pData)
{
	VO_S32 nRc = VO_ERR_NONE;
	nRc = CPlayerBaseCtrl::GetClipParam(nPid,pData);
	if(nRc == VO_ERR_NONE)
		return nRc;
	nRc = VO_ERR_NONE;
	switch(nPid)
	{
		case VOEDT_PID_PLAYER_DATA_BUFFER:
		{
// 			if(m_bChanged)
// 			{			
// 				m_pTextBlend->CreateTextBuffer(m_cTitle,&m_pDataBuffer);
// 				m_bChanged = VO_FALSE;
// 			}
// 			VO_VIDEO_BUFFER  *ppp = (VO_VIDEO_BUFFER*)(pData);
// 			*ppp = m_pDataBuffer;
			break;
		}
		case VOEDT_PID_CLIP_PARAM:
		{
			VOEDT_CLIP_PARAM  *ppp = (VOEDT_CLIP_PARAM*)(pData);
			memcpy(ppp,&m_sClipParam,sizeof(VOEDT_CLIP_PARAM));
			break;
		}
		case VOEDT_PID_PLAYER_TEXT_FORMAT:
		{
			VOEDT_TEXT_FORMAT  **ppp = (VOEDT_TEXT_FORMAT**)(pData);
			*ppp = (VOEDT_TEXT_FORMAT*)&m_sTextFormat;
			nRc = m_pTextBlend->SetFont(&m_sTextFormat);
			break;
		}
		default:
			nRc = VOEDT_ERR_PID_NOTFOUND;
			break;
	}
	return nRc;
}
