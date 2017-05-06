/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CPlayerBaseCtrl.cpp

Contains:	CPlayerBaseCtrl class file

Written by:	Leon Huang

Change History (most recent first):
2011-01-05		Leon			Create file
*******************************************************************************/
#include "CPlayerBaseCtrl.h"

CPlayerBaseCtrl::CPlayerBaseCtrl(VO_LIB_OPERATOR* pLibOp)
:m_nClipType(0)
,m_pLibOp(pLibOp)
,m_bDropBlackFrame(0)
,m_nCurPosNow(0)
{
	m_pDataBuffer.Buffer[0]= NULL;
	memset(m_cTitle,0x00,sizeof(m_cTitle));
	m_clip_info.cTitle = m_cTitle;
	InitParam();
}

 CPlayerBaseCtrl::~CPlayerBaseCtrl()
{
	ReleaseBuffer();
}
 VO_S32 CPlayerBaseCtrl::ReleaseBuffer()
 {
	 if(m_pDataBuffer.Buffer[0] ) delete []m_pDataBuffer.Buffer[0];
	 m_pDataBuffer.Buffer[0] = NULL;

	 return VO_ERR_NONE;
 }
VO_S32 CPlayerBaseCtrl::InitParam()
{
	m_bStopPlay = VO_FALSE;
	m_sClipParam.nPlayMode = 0;
	m_sClipParam.bloop = false;
	m_sClipParam.point_in = 0;
	m_sClipParam.point_out = 0;
	m_sClipParam.beginAtTimeline = -1;
	m_sClipParam.positionFormat.nTop =0 ;
	m_sClipParam.positionFormat.nLeft = 0;
	m_sClipParam.positionFormat.nHeight = -1;
	m_sClipParam.positionFormat.nWidth = -1;
	m_sClipParam.videoTransition = NULL;
	m_sClipParam.audioTransition = NULL;
	m_sClipParam.nVolume = VOEDT_AUDIO_DEFAULT_VOLUME;

	return VO_ERR_NONE;
}

VO_S32 CPlayerBaseCtrl::Stop()
{
	m_bStopPlay = VO_FALSE;
	return VO_ERR_NONE;
}
VO_S32 CPlayerBaseCtrl::SetClipParam(VO_S32 nPid,VO_VOID *pData)
{

	return VO_ERR_NONE;
}
VO_S32 CPlayerBaseCtrl::GetClipParam(VO_S32 nPid,VO_VOID *pData)
 {
	 VO_S32 nRc = VO_ERR_NONE;
	 switch(nPid)
	 {
	 case VOEDT_PID_CLIP_NAME:
		 {
			 VO_CHAR *ppp = (VO_CHAR*)(pData);
			 memcpy(ppp,m_cTitle,sizeof(m_cTitle));
			 break;
		 }
	case VOEDT_PID_CLIP_TYPE:
		{
			VO_U32 *ppp = (VO_U32*)(pData);

			*ppp = m_nClipType;
			break;
		}
	case VOEDT_PID_PLAYER_CLIP_PARAM://it is only for inside used
		{
			VOEDT_CLIP_PARAM  **ppp = (VOEDT_CLIP_PARAM**)(pData);
			*ppp = &m_sClipParam;

			break;
		}
	case VOEDT_PID_DROP_BLACK_FRAME:
		{
			*((VO_U32*)pData) = m_bDropBlackFrame;
			break;
		}
	case VOEDT_PID_PLAYER_OUTPUT_VIDEO_FORMAT:
		{
			VOEDT_VIDEO_FORMAT* vf =  (VOEDT_VIDEO_FORMAT*)pData;
			vf->ColorType = m_sVideoOutFormat.Type;
			vf->nWidth = m_sVideoOutFormat.Width;
			vf->nHeight = m_sVideoOutFormat.Height;
			VOLOGI("Get VOEDT_PID_PLAYER_OUTPUT_VIDEO_FORMAT (%dx%d)",m_sVideoOutFormat.Width,m_sVideoOutFormat.Height);
			break;
		}
	default:
		nRc = VOEDT_ERR_PID_NOTFOUND;
		break;
	 }
	return nRc;
 }
VO_S32 CPlayerBaseCtrl::GetDuration(VO_U32* pDuration)
{
	*pDuration = m_sClipParam.point_out - m_sClipParam.point_in;
	return VO_ERR_NONE;
}
VO_S32 CPlayerBaseCtrl::GetPosition(VO_U32* pPos)
{
	*pPos = 0;
	return VO_ERR_NONE;
}
VO_S32 CPlayerBaseCtrl::SetPosition(VO_U32* pPos)
{
	return VO_ERR_NONE;
}
VO_S32 CPlayerBaseCtrl::SetPosition(VOMP_BUFFERTYPE **pOutBuffer,VO_U32* pPos,VO_U32 nSampleType)
{
	return VO_ERR_NONE;
}
VO_S32 CPlayerBaseCtrl::SetDataSource(VO_VOID *pSource,VO_S32 nFlag)
{
	return VO_ERR_NONE;
}
VO_S32 CPlayerBaseCtrl::GetClipThumbNail(VO_U32 pos, VO_VIDEO_BUFFER **vBuffer )
{
	return VO_ERR_NONE;
}
VO_S32 CPlayerBaseCtrl::Flush()
{
	return VO_ERR_NONE;
}
VO_S32 CPlayerBaseCtrl::ResetParam()
{
	m_nCurPosNow = 0;
	return VO_ERR_NONE;
}

float CPlayerBaseCtrl::GetVideoTransitionValue()
{
	return 0;
}
VO_S32 CPlayerBaseCtrl::GetSample(VOMP_BUFFERTYPE **pOutBuffer,VO_U64 syncTimestmp,VO_S32 *pReserve,VO_U32 nSampleType)
{
	return VO_ERR_NONE;
}