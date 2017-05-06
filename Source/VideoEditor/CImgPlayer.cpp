/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
*																		*
************************************************************************/
/*******************************************************************************
File:		CImgPlayer.cpp

Contains:	CImgPlayer class file

Modify by:   Leon Huang

Change History (most recent first):
2011-03-03		Leon        Create the file from vompCPlayer.cpp

*******************************************************************************/
#include "CImgPlayer.h"

#include "voOSFunc.h"

#include "EffectionCtrl.h"
#include "voLog.h"



CImgPlayer::CImgPlayer(VO_LIB_OPERATOR* pLibOp)
	:CPlayerBaseCtrl(pLibOp)
	,m_llLastVideoMediaTime (0)
	,m_bEffectAction(VO_FALSE)
	,m_bExitPlayer(false)
	,m_llTime_previous(-1)
	
{	//,m_fVideoTransitionStep(0)
	
	m_bDropBlackFrame = 1;

	m_pImgEngine = new ImgEngine(NULL,NULL);
	m_pImgEngine->SetLibOperator(m_pLibOp);
	m_pVideoBuffer = NULL;
	m_pVideoBuffer = new VOMP_BUFFERTYPE;
	m_pVideoBuffer->pBuffer = NULL;

	memset (&m_sVideoFormat, 0, sizeof (m_sVideoFormat));
	memset (&m_sVideoFormat_Thumbnail, 0, sizeof (m_sVideoFormat_Thumbnail));

	m_sClipParam.bloop = 0;
	m_sClipParam.point_in  = 0;
	m_sClipParam.point_out = 0;
	m_sClipParam.videoTransition = NULL;
	m_sClipParam.audioTransition = NULL;


	m_Previous_VideoBuffer.pBuffer = NULL;
	m_pPresentBuffer = NULL;

	m_nClipType = VOEDT_PID_CLIP_TYPE_IMAGE;
}	

//add by leon for test
VO_VOID CImgPlayer::ReflashLib()
{

}
CImgPlayer::~CImgPlayer ()
{
	m_bExitPlayer = true;
	
//	if(m_Previous_VideoBuffer.pBuffer)delete []m_Previous_VideoBuffer.pBuffer;
	
	VO_VIDEO_BUFFER *vf = (VO_VIDEO_BUFFER*)m_Previous_VideoBuffer.pBuffer;
	if(vf)
	{
		if(vf->Buffer[0]) delete vf->Buffer[0];
		delete vf;
	}
	if(m_pVideoBuffer )
	{
		VO_VIDEO_BUFFER *vf = (VO_VIDEO_BUFFER*)m_pVideoBuffer->pBuffer;
		if(vf)
		{
			if(vf->Buffer[0]) delete vf->Buffer[0]; 
			delete vf;
		}
		delete m_pVideoBuffer;
	}
	//if(m_pDataBuffer.Buffer[0]) delete []m_pDataBuffer.Buffer[0]; 
}
VO_S32 CImgPlayer::SetClipParam(VO_S32 nPid,VO_VOID *pData)
{
	VO_S32 nRc = VOEDT_ERR_NONE;
	switch (nPid)
	{
	case VOEDT_PID_CLIP_PARAM:
		{
			VOEDT_CLIP_PARAM  *pParam = (VOEDT_CLIP_PARAM*)(pData);
			memcpy(&m_sClipParam,pParam,sizeof(VOEDT_CLIP_PARAM));
			break;
		}
	case VOEDT_PID_THUMBNAIL_FORMAT:
		{
			VOEDT_VIDEO_FORMAT *tf = (VOEDT_VIDEO_FORMAT*)pData;
			VOMP_VIDEO_FORMAT vf;
			m_sVideoFormat_Thumbnail.ColorType = vf.Type = tf->ColorType;
			CheckWidthHeight(tf->nWidth, tf->nHeight)
			m_sVideoFormat_Thumbnail.nHeight = vf.Height = tf->nHeight;
			//width 16µÄ±¶Êý
			m_sVideoFormat_Thumbnail.nWidth = vf.Width = tf->nWidth;
			break;
		}
	case VOEDT_PID_DROP_BLACK_FRAME:
		{
			m_bDropBlackFrame = *((VO_U32*) pData);
			break;
		}
	case VOEDT_PID_PLAYER_OUTPUT_VIDEO_FORMAT:
		{
			if(!(m_nClipType & VOEDT_PID_CLIP_TYPE_VIDEO)) break; 

			VOMP_VIDEO_FORMAT *vf = (VOMP_VIDEO_FORMAT*) pData;
			VOLOGI("Set output videoFormat:W:%d,H:%d,ColorType:%d",vf->Width,vf->Height,vf->Type);
			if(m_sVideoOutFormat.Height == vf->Height && m_sVideoOutFormat.Width == vf->Width && m_sVideoOutFormat.Type == vf->Type)
				break;

			memcpy(&m_sVideoOutFormat, (VOMP_VIDEO_FORMAT*) pData, sizeof(VOMP_VIDEO_FORMAT));
			m_nColorType = m_sVideoOutFormat.Type;
			
			m_pImgEngine->SetParam (VOEDT_PID_VIDEO_FORMAT, &m_sVideoOutFormat);

// 			EX_INIT_BUFFER3(m_sVideoOutFormat, &m_CurrentVideoBuffer);
// 			m_nVideoStride = m_sVideoOutFormat.Width  * 4;
// 			m_CurrentVideoBuffer.pData = (void*)&m_nVideoStride;
// 
// 			EX_INIT_BUFFER3(m_sVideoOutFormat, &m_Previous_VideoBuffer);
// 			m_Previous_VideoBuffer.pData = (void*)&m_nVideoStride;
			break;
		}
	default:
		nRc = VOEDT_ERR_PID_NOTFOUND;
		break;
	}
	return nRc;
}
VO_S32 CImgPlayer::GetClipParam(VO_S32 nPid,VO_VOID *pData)
{
	VO_S32 nRc = VOEDT_ERR_NONE;

	nRc = CPlayerBaseCtrl::GetClipParam(nPid,pData);
	if(nRc == VOEDT_ERR_NONE)
		return nRc;
	nRc = VOEDT_ERR_NONE;
	switch (nPid)
	{
	case VOEDT_PID_PLAYER_PLAY_MODE:
		{
			VO_U32 *ppp = (VO_U32*)(pData);

			*ppp = m_sClipParam.nPlayMode;
			break;
		}
	case VOEDT_PID_VIDEO_FORMAT:
		{
			VOEDT_VIDEO_FORMAT *vf = (VOEDT_VIDEO_FORMAT*)pData;
			vf->ColorType = m_sVideoFormat.Type;
			vf->nHeight = m_sVideoFormat.Height;
			vf->nWidth = m_sVideoFormat.Width;
			break;
		}
	case VOEDT_PID_CLIP_PARAM:
		{
			VOEDT_CLIP_PARAM  *ppp = (VOEDT_CLIP_PARAM*)(pData);
			//*ppp = &m_sClipParam;
			memcpy(ppp,&m_sClipParam,sizeof(VOEDT_CLIP_PARAM));
			break;
		}
	case VOEDT_PID_THUMBNAIL_FORMAT:
		{
			VOEDT_VIDEO_FORMAT *tf = (VOEDT_VIDEO_FORMAT*)pData;
			tf->ColorType = m_sVideoFormat_Thumbnail.ColorType ;
			tf->nHeight = m_sVideoFormat_Thumbnail.nHeight ;
			tf->nWidth = m_sVideoFormat_Thumbnail.nWidth ;
			break;
		}
	default:
		nRc = VOEDT_ERR_PID_NOTFOUND;
		break;
	}
	return nRc;
}


int CImgPlayer::SetDataSource (void * pSource, int nFlag)
{	
	VOLOGI("1");
	VO_U32 nRc = VOEDT_ERR_NONE;
	memcpy(m_cTitle, (VO_CHAR*)pSource, sizeof(m_cTitle));
	nRc = m_pImgEngine->SetDataSource(pSource,nFlag);
	CHECK_FAIL(nRc);
	VO_U32 duration = 9999999;
	m_sClipParam.point_out = 0;
	m_clip_info.nClipType = m_nClipType;
	m_clip_info.nDuration = duration;
	m_sClipParam.nPlayMode = VOEDT_PID_PLAYMODE_IMG;
	m_pImgEngine->GetParam(VOMP_PID_VIDEO_FORMAT, &m_sVideoFormat);
	m_sClipParam.positionFormat.nHeight = m_sVideoFormat.Height;
	m_sClipParam.positionFormat.nWidth = m_sVideoFormat.Width;
	return VOEDT_ERR_NONE;
}
// int CImgPlayer::Run (void)
// {
// 	int nRC = m_pVompCEngine->Run ();
// 	
// 	return nRC;
// }

// int CImgPlayer::Pause (void)
// {
// 	int nRC = m_pVompCEngine->Pause ();
// 	return nRC;
// }

// VO_S32 CImgPlayer::Stop (void)
// {
// 	m_bStopPlay = true;
// 
// 	VO_S32 nRC = m_pVompCEngine->Stop ();
// 
// 	return nRC;
// }

// int CImgPlayer::SetCurPos (int nCurPos)
// {
// //	if (m_bSeeking)
// //		return VOMP_ERR_Status;
// 
// 	m_bSeeking = true;
// 
// 	return m_pVompCEngine->SetCurPos (nCurPos);
// }

// int	CImgPlayer::HandleEvent (int nID, void * pParam1, void * pParam2)
// {
// 	return VOMP_ERR_Implement;
// }


VO_S32 CImgPlayer::SetAParam(void* pValue)
{
	VOEDT_CLIP_PARAM *param = (VOEDT_CLIP_PARAM*)pValue;
	m_sClipParam.bloop = param->bloop;
	m_sClipParam.nPlayMode = param->nPlayMode;
	VO_U32 pos;
	GetDuration(&pos);
	m_sClipParam.point_in = 0;
	m_sClipParam.point_out =  param->point_out;
	m_sClipParam.beginAtTimeline = param->beginAtTimeline >=0 ? param->beginAtTimeline : -1;
	/*m_sClipParam.targetVFormat.nHeight = param->targetVFormat.nHeight;
	m_sClipParam.targetVFormat.nWidth = param->targetVFormat.nWidth;
	m_sClipParam.targetVFormat.nLeft = param->targetVFormat.nLeft;
	m_sClipParam.targetVFormat.nTop = param->targetVFormat.nTop;*/
	m_sClipParam.nVolume = 0;
	if(param->videoTransition )
	{	
		m_sClipParam.videoTransition = param->videoTransition;
		m_sClipParam.videoTransition->nDuration = param->videoTransition->nDuration;
		m_sClipParam.videoTransition->nType = param->videoTransition->nType;
	}
	return VOEDT_ERR_NONE;
}
VO_S32 CImgPlayer::GetASample(VO_S32 nSampletype,VOMP_BUFFERTYPE **pBufferIn)
{
	VO_U32 nRC = VOEDT_ERR_NONE;
	VOMP_BUFFERTYPE *pBuffer = m_pPresentBuffer;

	nRC = m_pImgEngine->GetParam(VOEDT_PID_VIDEO_SAMPLE, (VO_VIDEO_BUFFER*)&pBuffer);
	
	*pBufferIn = pBuffer;
	return nRC;
}

VO_S32 CImgPlayer::GetSample(VOMP_BUFFERTYPE **pOutBuffer,VO_U64 syncTimestamp,VO_S32 *pReserve,VO_U32 nSampleType)
{
	VOMP_BUFFERTYPE *pBuffer;
	m_bStopPlay = false;
	VO_U32  effecttmp;
	VOEDT_TRANSITION_EFFECT *tmppTransition;

	int nRC = VOEDT_ERR_NONE;
	if(VOEDT_PID_VIDEO_SAMPLE == nSampleType)
	{
		VOLOGI("GetSample");
		m_pPresentBuffer = &m_Previous_VideoBuffer;
		nRC = GetASample(nSampleType,&pBuffer);

		m_sClipParam.videoTransition ? effecttmp = m_sClipParam.videoTransition->nType :effecttmp = VOEDT_TRANSITION_NONE;
		tmppTransition = m_sClipParam.videoTransition;
	}
		
	*pOutBuffer = pBuffer;
	VOLOGI("Image SampleSize:%d", pBuffer->nSize);
	m_nCurPosNow += 40;
	CHECK_FAIL(nRC);
	return nRC;
}
float CImgPlayer::GetVideoTransitionValue()
{
	return  (m_nCurPosNow -m_sClipParam.point_in) * 255.0/ (m_sClipParam.videoTransition->nDuration);
}

float CImgPlayer::GetAudioTransitionValue()
{
	float flag = (m_nCurPosNow -m_sClipParam.point_in) * 1.0 / (m_sClipParam.audioTransition->nDuration);
	return  flag <= 1? flag : 1;
}

VO_BOOL CImgPlayer::CheckBlackFrame (VO_VIDEO_BUFFER * pVideoBuffer, VOMP_VIDEO_FORMAT * pVideoSize)
{
	VO_BOOL bBlack = VO_FALSE;

	return bBlack;
}


VO_S32 CImgPlayer::InitParam()
{
	m_llTime_previous = -1;
	m_bStopPlay = false;
	m_fVideoTransitionStep = 0;
	m_nAudioTransitionStep = 0;
	m_sClipParam.nPlayMode = 0;
	//m_sClipParam.audioTransition->nDuration = 0;
	//m_sClipParam.audioTransition->nType = VOEDT_TRANSITION_AUDIO_FADE_IN_FADE_OUT;
	m_sClipParam.bloop = false;
	m_sClipParam.nPlayMode = 0;
	m_sClipParam.point_in = 0;
	m_sClipParam.point_out = 0;
	m_sClipParam.beginAtTimeline = -1;
	m_sClipParam.videoTransition = NULL;
	m_sClipParam.audioTransition = NULL;
	m_sClipParam.nVolume = VOEDT_AUDIO_DEFAULT_VOLUME;
	//m_sClipParam.videoTransition->nDuration = 0;
	//m_sClipParam.videoTransition->nType = VOEDT_TRANSITION_NONE;
	
	return VOEDT_ERR_NONE;
}


VO_S32 CImgPlayer::ResetParam()
{
	m_llTime_previous = -1;
	m_bStopPlay = false;
	m_fVideoTransitionStep = 0;
	m_nAudioTransitionStep = 0;
	m_nCurPosNow = 0;
	return VOEDT_ERR_NONE;
}

VO_S32 CImgPlayer::GetClipThumbNail(VO_U32 pos, VO_VIDEO_BUFFER **vBuffer )
{
	VOMP_BUFFERTYPE *pOutBuf;
	pOutBuf->pBuffer = (VO_PBYTE)*vBuffer; 
	return VOEDT_ERR_NONE;
}
VO_S32 CImgPlayer::SetPosition(VOMP_BUFFERTYPE **pOutBuffer,VO_U32 *pPos,VO_U32 nSampleType)
{
	VO_S32 nRc = 0;
	//trim must be reset file position.
	m_nCurPosNow = *pPos;
	nRc = GetSample(pOutBuffer,0,NULL,VOEDT_PID_VIDEO_SAMPLE );
	return  nRc;
}

