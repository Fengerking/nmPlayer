/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
*																		*
************************************************************************/
/*******************************************************************************
File:		CClipPlayer.cpp

Contains:	CClipPlayer class file

Modify by:   Leon Huang

Change History (most recent first):
2011-03-03		Leon        Create the file from vompCPlayer.cpp

*******************************************************************************/
#include "CClipPlayer.h"

#include "voOSFunc.h"

#include "EffectionCtrl.h"
#include "voLog.h"

#define VOMP_PID_CHECK_PERFORMANCE 0X09000001
#define  VIDEO_EDITOR

CClipPlayer::CClipPlayer(VO_LIB_OPERATOR* pLibOp)
	:CPlayerBaseCtrl(pLibOp)
	,m_bOpenSync (true)
	, m_nPlayMode (VOMP_PULL_MODE)
	, m_nColorType (VOMP_COLOR_ARGB32_PACKED)
//	, m_nColorType (VOMP_COLOR_RGB565_PACKED)
//	, m_nColorType (VOMP_COLOR_YUV_PLANAR420)
	, m_llLastVideoMediaTime (0)
	, m_nLastVideoSystemTime (0)
	, m_bStopPlay (true)
	, m_bSeeking (false)
	, m_bRenderOut (false)
	,m_bEffectAction(VO_FALSE)
	,m_bExitPlayer(false)
	,m_llTime_previous(-1)
	,m_fVideoTransitionStep(0)
	,m_nAudioTransitionStep(0)
	,m_syncStartTimestamp(-1)
	,m_nCurVolume(0)
	,m_pTrimCtrl(NULL)
	,m_pVideoBuffer(NULL)
	,m_nVideoMaxSize(0)
{
	m_pVompCEngine = new vompCEngine(NULL,NULL);
	m_pVompCEngine->SetLibOperator(m_pLibOp);
	//m_pVompCEngine->SetParam(,void*)m_pcWorkingPath);
	m_pVompCEngine->Init();

	m_pVideoBuffer = new VOMP_BUFFERTYPE;
	m_pVideoBuffer->pBuffer = NULL;

	memset (&m_sOrignalAudioFormat, 0, sizeof (m_sOrignalAudioFormat));
	memset (&m_sVideoFormat, 0, sizeof (m_sVideoFormat));
	memset (&m_sVideoFormat_Thumbnail, 0, sizeof (m_sVideoFormat_Thumbnail));
	
	memset(&m_CurrentVideoBuffer, 0 ,sizeof(VOMP_BUFFERTYPE));
	memset(&m_CurrentAudioBuffer, 0 ,sizeof(VOMP_BUFFERTYPE));

	m_sClipParam.bloop = 0;
	m_sClipParam.point_in  = 0;
	m_sClipParam.point_out = 0;
	m_sClipParam.videoTransition = NULL;
	m_sClipParam.audioTransition = NULL;

	m_Previous_VideoBuffer.pBuffer = NULL;
	m_pPresentBuffer = NULL;

	NULL_AUDIO.llTime = 0;
	NULL_AUDIO.nSize = 2*2*44100/100;
	NULL_AUDIO.pBuffer = new VO_BYTE[NULL_AUDIO.nSize];
	memset(NULL_AUDIO.pBuffer,-0x8000,sizeof(VO_BYTE) * NULL_AUDIO.nSize /2);


	m_audioReSampleEngine.SetLibOperator(pLibOp);
	m_audioReSampleEngine.Init();
}	

//add by leon for test
VO_VOID CClipPlayer::ReflashLib()
{

}
CClipPlayer::~CClipPlayer ()
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

	delete []NULL_AUDIO.pBuffer;
	if(m_pTrimCtrl)delete m_pTrimCtrl;

	VOLOGI("exit clip player");
}
VO_S32 CClipPlayer::SetClipParam(VO_S32 nPid,VO_VOID *pData)
{	
	VOLOGI("in SetclipParam nID :%08x",nPid);
	VO_S32 nRc = VO_ERR_NONE;
	switch (nPid)
	{
	case VOEDT_PID_CLIP_PARAM:
		{
			VOEDT_CLIP_PARAM  *pParam = (VOEDT_CLIP_PARAM*)(pData);
			VO_U32 duration;
			GetDuration(&duration);
			pParam->point_in = (pParam->point_in >=  duration&& pParam->point_in <=0 )? 0 : pParam->point_in;
			pParam->point_out = (pParam->point_out >= duration && pParam->point_out <=0 ) ? duration: pParam->point_out;
			//	pParam->beginAtTimeline = pParam->beginAtTimeline >=0 ? pParam->beginAtTimeline : -1;
			/*m_sClipParam.targetVFormat.nHeight = param->targetVFormat.nHeight;
			m_sClipParam.targetVFormat.nWidth = param->targetVFormat.nWidth;
			m_sClipParam.targetVFormat.nLeft = param->targetVFormat.nLeft;
			m_sClipParam.targetVFormat.nTop = param->targetVFormat.nTop;*/
			pParam->nVolume = pParam->nVolume <= 0 ? 0 : pParam->nVolume;
			VOLOGI("Before check PlayMode:%d", pParam->nPlayMode);
			if(m_nClipType == VOEDT_PID_CLIP_TYPE_VIDEO| VOEDT_PID_CLIP_TYPE_AUDIO) ;
			else if(m_nClipType == VOEDT_PID_CLIP_TYPE_VIDEO) pParam->nPlayMode = VOEDT_PID_PLAYMODE_VIDEO;
			else if(m_nClipType == VOEDT_PID_CLIP_TYPE_AUDIO) pParam->nPlayMode = VOEDT_PID_PLAYMODE_AUDIO;
			VOLOGI("After check PlayMode:%d", pParam->nPlayMode);

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
			m_pVompCEngine->SetParam (VOMP_PID_DRAW_COLOR, &m_nColorType);
			VO_RECT rcDraw;
			rcDraw.left = 0; rcDraw.top = 0;
			rcDraw.right = m_sVideoOutFormat.Width, rcDraw.bottom = m_sVideoOutFormat.Height;
			m_pVompCEngine->SetParam (VOMP_PID_DRAW_RECT, &rcDraw);
			
			m_nVideoMaxSize = EX_INIT_BUFFER3(m_sVideoOutFormat, &m_CurrentVideoBuffer);
			m_nVideoStride = m_sVideoOutFormat.Width  * 4;
			m_CurrentVideoBuffer.pData = (void*)&m_nVideoStride;

			EX_INIT_BUFFER3(m_sVideoOutFormat, &m_Previous_VideoBuffer);
			m_Previous_VideoBuffer.pData = (void*)&m_nVideoStride;
			VOLOGI("BufferSize. CurVideoBuffer:%d, PreviousVideoBuffer:%d", m_CurrentVideoBuffer.nSize, m_Previous_VideoBuffer.nSize);

			break;
		}
	case VOEDT_PID_PLAYER_AUDIO_FORMAT_TARGENT:
		{
			VOEDT_AUDIO_FORMAT *af = (VOEDT_AUDIO_FORMAT *)pData;
			memcpy( &m_sTargetAudioFormat, af, sizeof(VOEDT_AUDIO_FORMAT));
			m_audioReSampleEngine.SetParam(nPid, pData);
		}
		break;
	default:
		nRc = VOEDT_ERR_PID_NOTFOUND;
		break;
	}
	return nRc;
}
VO_S32 CClipPlayer::GetClipParam(VO_S32 nPid,VO_VOID *pData)
{
	VO_S32 nRc = VO_ERR_NONE;
	nRc = CPlayerBaseCtrl::GetClipParam(nPid,pData);
	if(nRc == VO_ERR_NONE)
		return nRc;
	nRc = VO_ERR_NONE;
	switch (nPid)
	{
	case VOEDT_PID_AUDIO_CODEC:
		{
			VO_U32 *ppp = (VO_U32*)(pData);

			*ppp = m_nAudioCodec;
			break;
		}
	case VOEDT_PID_VIDEO_CODEC:
		{
			VO_U32 *ppp = (VO_U32*)(pData);

			*ppp = m_nVideoCodec;
			break;
		}
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
			VOLOGI("Get Video Format (%dx%d)",vf->nWidth,vf->nWidth);
			break;
		}

	case VOEDT_PID_AUDIO_FORMAT:
		{
			VOEDT_AUDIO_FORMAT *af = (VOEDT_AUDIO_FORMAT*)pData;
			af->Channels = m_sOrignalAudioFormat.Channels;
			af->SampleBits = m_sOrignalAudioFormat.SampleBits;
			af->SampleRate = m_sOrignalAudioFormat.SampleRate;
		
			break;
		}
	/*case VOEDT_PID_PLAYER_THUMBNAIL_BUFFER:
		{
			VO_VIDEO_BUFFER  **ppp = (VO_VIDEO_BUFFER**)(pData);
			*ppp = &m_pDataBuffer;
			break;
		}*/
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
	case VOEDT_PID_DROP_BLACK_FRAME:
		{
			*((VO_U32*)pData) = m_bDropBlackFrame;
			break;
		}
	case VOEDT_PID_AUDIO_SAMPLE:
		{
			VOMP_BUFFERTYPE **abuffer = (VOMP_BUFFERTYPE**)pData;
			nRc = GetASample(VOMP_PID_AUDIO_SAMPLE,abuffer);
			break;
		}
	case VOEDT_PID_VIDEO_SAMPLE:
		{
			VOMP_BUFFERTYPE **vbuffer = (VOMP_BUFFERTYPE**)pData;
			if(VOMP_COLOR_ARGB32_PACKED != m_nColorType)
			{
				m_nColorType = VOMP_COLOR_ARGB32_PACKED;
				m_pVompCEngine->SetParam (VOMP_PID_DRAW_COLOR, &m_nColorType);
				VO_RECT rcDraw;
				rcDraw.left = 0; rcDraw.top = 0;
				rcDraw.right = m_sVideoFormat.Width, rcDraw.bottom = m_sVideoFormat.Height;
				m_pVompCEngine->SetParam (VOMP_PID_DRAW_RECT, &rcDraw);
				int nCurPos = 0;
				m_pVompCEngine->GetCurPos(&nCurPos);
				m_pVompCEngine->SetCurPos(nCurPos);
			}
			nRc = GetASample(VOMP_PID_VIDEO_SAMPLE,vbuffer);
			//{VOLOGI("SampleStamp:%lld",((VO_VIDEO_BUFFER*)((VOMP_BUFFERTYPE*)*vbuffer)->pBuffer)->Time);}
			break;
		}
	default:
		nRc = VOEDT_ERR_PID_NOTFOUND;
		break;
	}
	return nRc;
}


int CClipPlayer::SetDataSource (void * pSource, int nFlag, int nMode,VO_U32 editorType)
{	
	VOLOGI("in SetDataSource! Type:%d", editorType);
	Stop ();
	InitParam();

	m_nPlayMode = VOMP_PULL_MODE;
	
	m_pVompCEngine->SetParam (VOMP_PID_AUDIO_PLAYMODE, &m_nPlayMode);
	m_pVompCEngine->SetParam (VOMP_PID_VIDEO_PLAYMODE, &m_nPlayMode);

	m_pVompCEngine->SetParam (VOMP_PID_DRAW_COLOR, &m_nColorType);
	//GetClientRect (m_hWnd, (LPRECT)&m_rcDraw);
	//m_rcDraw.bottom = m_rcDraw.bottom - 26;
	//SetParam (VOMP_PID_DRAW_RECT, &m_rcDraw);
	VO_S32 param = 1;
	int s = m_pVompCEngine->SetParam(VOMP_PID_DISABLE_DROPFRAME,&param);

	param = 0;//any pos
	s     = m_pVompCEngine->SetParam(VOMP_PID_SEEK_MODE,&param);
	param = 0x03;//fixwindow
	m_pVompCEngine->SetParam(VOMP_PID_ZOOM_MODE,&param);
	param = 1;
	m_pVompCEngine->SetParam(VOMP_PID_AUDIO_GETONLY, &param);

	param = 1;
//	if(editorType == VOEDT_PID_CLIP_TYPE_AUDIO)
//		m_pVompCEngine->SetParam(VOMP_PID_AUDIO_ONLY,&param);

	 m_pVompCEngine->SetParam(VOMP_PID_CHECK_PERFORMANCE,&param);

	VOLOGI("2");
	int nOpenFlag = m_bOpenSync ? VOMP_FLAG_SOURCE_SYNC : VOMP_FLAG_SOURCE_ASYNC;
	if (nMode <= 3)
	{
		VOLOGI("3");
#ifdef _WIN32
		VO_TCHAR source[255];
		MultiByteToWideChar (CP_ACP, 0,(VO_CHAR*) pSource, -1, source, sizeof (source));
#else
		VO_CHAR source[255];
		memcpy(source, (VO_CHAR*)pSource, sizeof(source));
#endif
		if (m_opFile.Open ((const char *)source) < 0)
			return VOEDT_ERR_OPEN_FAIL;

		memcpy(m_cTitle, (VO_CHAR*)pSource, sizeof(m_cTitle));
		
		nOpenFlag = nOpenFlag | VOMP_FLAG_SOURCE_URL;
		VOLOGI("URL: %s, flag: %d",source,nOpenFlag );
		int nRC = m_pVompCEngine->SetDataSource (source, nOpenFlag);
		if(nRC != VOMP_ERR_None) return nRC;
		nRC = m_pVompCEngine->GetParam (VOMP_PID_AUDIO_FORMAT, &m_sOrignalAudioFormat);
		if(nRC == VOMP_ERR_None)
		{

			m_pVompCEngine->GetParam(VOMP_PID_AUDIO_CODEC,&m_nAudioCodec);
			if(m_nAudioCodec > 0) {
				m_nAudioMaxSize = m_sOrignalAudioFormat.Channels * m_sOrignalAudioFormat.SampleBits * m_sOrignalAudioFormat.SampleRate / 8 +100;
				VOLOGI("Channels:%d, SampleBits:%d, SampleRate:%d,AudioMaxSize:%d", m_sOrignalAudioFormat.Channels, m_sOrignalAudioFormat.SampleBits, m_sOrignalAudioFormat.SampleRate, m_nAudioMaxSize);
				m_CurrentAudioBuffer.pBuffer = new unsigned char[m_nAudioMaxSize];
				//if(editorType & VOEDT_PID_CLIP_TYPE_AUDIO)
				m_sClipParam.nPlayMode |= VOEDT_PID_PLAYMODE_AUDIO;
				m_nIntervalTime = 300;
				m_nClipType |= VOEDT_PID_CLIP_TYPE_AUDIO;

				VOEDT_AUDIO_FORMAT af;
				af.Channels = m_sOrignalAudioFormat.Channels;
				af.SampleBits = m_sOrignalAudioFormat.SampleBits;
				af.SampleRate = m_sOrignalAudioFormat.SampleRate;
				m_audioReSampleEngine.SetParam(VOEDT_PID_PLAYER_AUDIO_FORMAT_ORIGINAL, &af );
			}			
		}
		nRC = m_pVompCEngine->GetParam (VOMP_PID_VIDEO_FORMAT, &m_sVideoFormat);
		if(nRC == VOMP_ERR_None)
		{
			m_pVompCEngine->GetParam(VOMP_PID_VIDEO_CODEC,&m_nVideoCodec);
			if(m_nVideoCodec > 0){
				//if(editorType & VOEDT_PID_CLIP_TYPE_VIDEO)
				m_sClipParam.nPlayMode |= VOEDT_PID_PLAYMODE_VIDEO;
				m_nClipType |= VOEDT_PID_CLIP_TYPE_VIDEO;			
			}
		}
		VOLOGI("ClipType:%d", m_nClipType);
		VO_U32 duration;
		GetDuration(&duration);
		m_sClipParam.point_out = duration;
	
		m_clip_info.nClipType = m_nClipType;
		m_clip_info.nDuration = duration;
		//m_clip_info.pClip = this;
		//if(editorType == VOEDT_CLIP_TYPE_AUDIO)
		//{
		//	GetParam (VOMP_PID_AUDIO_FORMAT, &m_sOrignalAudioFormat);
		//	//m_Previous_VideoBuffer.pBuffer = new VO_BYTE[AUDIO_BUFFER_SIZE];
		//	//m_nIntervalTime = 300;
		//}
		
	
		if (m_nPlayMode == VOMP_PULL_MODE )
		{
			VO_U32 nID = 0;
			if(m_bOpenSync) Run();
		}

	}

	return VO_ERR_NONE;
}

int CClipPlayer::Run (void)
{
	int nRC = m_pVompCEngine->Run ();
	
	return nRC;
}

int CClipPlayer::Pause (void)
{
	int nRC = m_pVompCEngine->Pause ();
	return nRC;
}

VO_S32 CClipPlayer::Stop (void)
{
	VOLOGI("+Player stop");
	m_bStopPlay = true;

	VO_S32 nRC = m_pVompCEngine->Stop ();
	VOLOGI("-Player stop");

	return nRC;
}

int CClipPlayer::SetCurPos (int nCurPos)
{
//	if (m_bSeeking)
//		return VOMP_ERR_Status;

	m_bSeeking = true;

	return m_pVompCEngine->SetCurPos (nCurPos);
}

int	CClipPlayer::HandleEvent (int nID, void * pParam1, void * pParam2)
{
	return VOMP_ERR_Implement;
}

//int CClipPlayer::vompThreadCreate (void ** pHandle, int * pID, void * fProc, void * pParam)
//{
//	*pHandle = NULL;
//	*pID = 0;
//
//	*pHandle = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE) fProc, pParam, 0, (LPDWORD)pID);
//
//	return 0;
//}



int CClipPlayer::vompSourceDrmCallBack (void * pUserData, int nFlag, void * pParam, int nReserved)
{
	return 0;
}

VO_VOID CClipPlayer::NotifyStatus (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{

}

VO_S32 CClipPlayer::SetAParam(void* pValue)
{
	VOEDT_CLIP_PARAM *param = (VOEDT_CLIP_PARAM*)pValue;
	m_sClipParam.bloop = param->bloop;
	m_sClipParam.nPlayMode = param->nPlayMode;
	VO_U32 pos;
	GetDuration(&pos);
	m_sClipParam.point_in = (param->point_in >=  pos&& param->point_in <=0 )? 0 : param->point_in;
	m_sClipParam.point_out = (param->point_out >= pos && param->point_out <=0 ) ? pos: param->point_out;
	m_sClipParam.beginAtTimeline = param->beginAtTimeline >=0 ? param->beginAtTimeline : -1;
	/*m_sClipParam.targetVFormat.nHeight = param->targetVFormat.nHeight;
	m_sClipParam.targetVFormat.nWidth = param->targetVFormat.nWidth;
	m_sClipParam.targetVFormat.nLeft = param->targetVFormat.nLeft;
	m_sClipParam.targetVFormat.nTop = param->targetVFormat.nTop;*/
	m_sClipParam.nVolume = param->nVolume <= 0 ? 0 : param->nVolume;
	if(param->videoTransition )
	{	
		m_sClipParam.videoTransition = param->videoTransition;
		m_sClipParam.videoTransition->nDuration = param->videoTransition->nDuration;
		m_sClipParam.videoTransition->nType = param->videoTransition->nType;
	}
	if(param->audioTransition )
	{	
		m_sClipParam.audioTransition = param->audioTransition;
		m_sClipParam.audioTransition->nDuration = param->audioTransition->nDuration;
		m_sClipParam.audioTransition->nType = param->audioTransition->nType;
	}
	return VO_ERR_NONE;
}
VO_S32 CClipPlayer::GetASample(VO_S32 nSampletype,VOMP_BUFFERTYPE **pBufferIn)
{
	int nRC = VOMP_ERR_EOS;
	VOMP_BUFFERTYPE **pBuffer = pBufferIn;
	
	while (!m_bStopPlay)
	{
	//	pBuffer = NULL;
		if (*pBuffer != NULL)		(*pBuffer)->llTime = 0;
		nRC = m_pVompCEngine->GetParam (nSampletype, pBuffer);
		switch(nRC)
		{
		case VOMP_ERR_Retry: { voOS_Sleep(1); continue;}
		case VOMP_ERR_None: { break; }
		case VOMP_ERR_EOS:{ VOLOGI("clip eos"); break;}
		case VOMP_ERR_FormatChange:	{	break;	}
		default: { if(*pBuffer == NULL) {voOS_Sleep(1);continue;} }
		}
		if(nRC == VOMP_ERR_None || nRC == VOMP_ERR_EOS )
			break;
		//if(*pBuffer) break;
	}
	VOLOGI("nRC :0x%08x. GetSample Size:%d", nRC, (*pBuffer)->nSize);

	return nRC;
}
VO_S32 CClipPlayer::GetPosition(VO_U32 *pos)
{
	int p = 0;
	VO_S32 nRc = m_pVompCEngine->GetCurPos(&p);
	*pos = (VO_U32)p;

	return nRc;
}
VO_S32 CClipPlayer::Flush()
{
	VO_S32 nRc = m_pVompCEngine->Flush();

	return nRc;
}
VO_S32 CClipPlayer::GetSample(VOMP_BUFFERTYPE **pOutBuffer,VO_U64 syncTimestamp,VO_S32 *pReserve,VO_U32 nSampleType)
{
	VOMP_BUFFERTYPE *pBuffer = NULL;
	m_bStopPlay = false;
	VO_U32  effecttmp;
	VOEDT_TRANSITION_EFFECT *tmppTransition;
	if(m_syncStartTimestamp < 0) m_syncStartTimestamp = m_sClipParam.point_in;//syncTimestamp;

	int nRC = -1;
	int sType = VOMP_PID_VIDEO_SAMPLE;
	if(VOEDT_PID_VIDEO_SAMPLE == nSampleType)
	{
		sType = VOMP_PID_VIDEO_SAMPLE;
		VOLOGI("PresentTime:%lld, PointIn:%d, SyncCurTime:%llu, SyncStartTime:%lld", m_pPresentBuffer->llTime, m_sClipParam.point_in, syncTimestamp, m_syncStartTimestamp);
		while(!m_bStopPlay && ((m_pPresentBuffer->llTime - m_sClipParam.point_in) < VO_S64(syncTimestamp - m_syncStartTimestamp)))
		{
			m_CurrentVideoBuffer.nSize = m_nVideoMaxSize;
			EX_COPY_BUFFER1(0, &m_Previous_VideoBuffer, &m_CurrentVideoBuffer,1);
		
			pBuffer = &m_CurrentVideoBuffer;
			VOLOGI("In GetSample. GetSample Size:%d",  pBuffer->nSize);
			nRC = GetASample(sType,&pBuffer);
			VOLOGI("nRC :0x%08x. GetSample Size:%d", nRC, pBuffer->nSize);
			if(nRC == VOMP_ERR_EOS) {EX_COPY_BUFFER1(0, &m_CurrentVideoBuffer, &m_Previous_VideoBuffer,1); m_CurrentVideoBuffer.nSize =0; break;}
			m_pPresentBuffer = pBuffer;
			m_CurrentVideoBuffer.llTime -= GetIntervalTime();
			continue;
		}
		SampleStamp_MIN(syncTimestamp,m_Previous_VideoBuffer.llTime,m_CurrentVideoBuffer.llTime) ?
			pBuffer = &m_CurrentVideoBuffer :pBuffer = &m_Previous_VideoBuffer;
		m_sClipParam.videoTransition ? effecttmp = m_sClipParam.videoTransition->nType :effecttmp = VOEDT_TRANSITION_NONE;
		tmppTransition = m_sClipParam.videoTransition;
		VOLOGI("2");
	}
	else  
	{
		sType = VOMP_PID_AUDIO_SAMPLE;
		m_CurrentAudioBuffer.nSize = m_nAudioMaxSize;
		pBuffer = &m_CurrentAudioBuffer;
		nRC = GetASample(sType,&pBuffer);
		
		if(nRC == VOMP_ERR_EOS) {
			pBuffer = &NULL_AUDIO;}
		
		VOLOGI("nRC: 0x%08x. Audio SampleTimestamp:%lld, Size:%d", nRC, pBuffer->llTime,pBuffer->nSize);
        if(pBuffer->nSize >0)
		{
			for(int i = 0; i< pBuffer->nSize; i+=2)
			{
				short buf1 = pBuffer->pBuffer[i+1] <<8 |pBuffer->pBuffer[i];
				int volume = 0;
				(*pReserve >m_sClipParam.nVolume || *pReserve == VOEDT_AUDIO_MAX_VOLUME)
					? volume = m_sClipParam.nVolume : volume = *pReserve;
				if(volume <= 0) volume = 0;
				m_nCurVolume = volume;
				//*pReserve <0 ? m_nCurvolume = m_sClipParam.nVolume : volume = *pReserve;
				int buf = buf1 * volume * 1.0 / VOEDT_AUDIO_DEFAULT_VOLUME;
				//if(buf> 0xffff) buf = 0xffff;
				if(buf<-0x8000)buf = -0x8000;
				if(buf>0x7fFF)buf= 0x7fFF;
				pBuffer->pBuffer[i] = buf ;
				pBuffer->pBuffer[i+1] = buf>>8;
			}
			m_sClipParam.audioTransition ? effecttmp = m_sClipParam.audioTransition->nType :effecttmp = VOEDT_TRANSITION_NONE;
			tmppTransition = m_sClipParam.audioTransition;
			if(m_sTargetAudioFormat.Channels != m_sOrignalAudioFormat.Channels 
				|| m_sTargetAudioFormat.SampleBits != m_sOrignalAudioFormat.SampleBits
				|| m_sTargetAudioFormat.SampleRate != m_sOrignalAudioFormat.SampleRate)
				m_audioReSampleEngine.Process(pBuffer);
		}
	}
	
	*pOutBuffer = pBuffer;
	VOLOGI("SampleSize:%d", (*pOutBuffer)->nSize);

	m_nCurPosNow = pBuffer->llTime;
	
	return nRC;
	//return (void*)&output;
}
float CClipPlayer::GetVideoTransitionValue()
{
	return  (m_nCurPosNow -m_sClipParam.point_in) * 255.0/ (m_sClipParam.videoTransition->nDuration);
}

float CClipPlayer::GetAudioTransitionValue()
{
	float flag = (m_nCurPosNow -m_sClipParam.point_in) * 1.0 / (m_sClipParam.audioTransition->nDuration);
	return  flag <= 1? flag : 1;
}

VO_BOOL CClipPlayer::CheckBlackFrame (VO_VIDEO_BUFFER * pVideoBuffer, VOMP_VIDEO_FORMAT * pVideoSize)
{
	VO_BOOL bBlack = VO_FALSE;
	if (pVideoBuffer == NULL || pVideoSize == NULL)
		return VO_FALSE;

	if (pVideoBuffer->ColorType != VO_COLOR_YUV_PLANAR420)
		return VO_FALSE;

	int nLightPixs = 0;
	int nTotalPixs = 0;

	int	nX = 0;
	int nY = 0;
	int nW = pVideoSize->Width;
	int nH = pVideoSize->Height;

	if (nW > 176)
	{
		nW = 176;
		nX =  (pVideoSize->Width - nW) / 2;
	}
	if (nH > 144)
	{
		nH = 144;
		nY = (pVideoSize->Height - nH) / 2;
	}

	int i = 0;
	int j = 0;
	unsigned char * pVideo = NULL;
	for (i = nY; i < nH + nY ; i++)
	{
		pVideo = pVideoBuffer->Buffer[0] + i * pVideoBuffer->Stride[0];
		for (j = nX; j < nW + nX ; j++)
		{
			if (pVideo[j] > 50)
				nLightPixs++;

			nTotalPixs++;
		}
	}

	if (nLightPixs > nTotalPixs / 10)
		bBlack = VO_FALSE;
	else
		bBlack = VO_TRUE;

	if (bBlack && (nW < pVideoSize->Width || nH < pVideoSize->Height))
	{
		nLightPixs = 0;
		nTotalPixs = 0;

		for (i = 0; i < pVideoSize->Height; i++)
		{
			pVideo = pVideoBuffer->Buffer[0] + i * pVideoBuffer->Stride[0];
			for (j = 0; j <pVideoSize->Width; j++)
			{
				if (pVideo[j] > 50)
					nLightPixs++;

				nTotalPixs++;
			}
		}

		if (nLightPixs > nTotalPixs / 10)
			bBlack = VO_FALSE;
		else
			bBlack = VO_TRUE;		
	}

	// VOLOGE ("Result L: %d, A: %d, R: %d", nLightPixs, nTotalPixs, m_bBlackFrame);

	return bBlack;
}

VO_U32 CClipPlayer::GetIntervalTime()
{
	return m_pTrimCtrl? m_pTrimCtrl->m_nVideoFrameTime : 0;//m_nIntervalTime;
}
VO_S32 CClipPlayer::Seek(int pos)
{
		return SetCurPos(pos); 
}


VO_S32 CClipPlayer::InitParam()
{
	m_llTime_previous = -1;
	m_bStopPlay = false;
	m_syncStartTimestamp = -1;
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
	
	return VO_ERR_NONE;
}


VO_S32 CClipPlayer::ResetParam()
{
	m_llTime_previous = -1;
	m_bStopPlay = false;
	m_syncStartTimestamp = -1;
	m_fVideoTransitionStep = 0;
	m_nAudioTransitionStep = 0;
	return 0;
}
VO_S32 CClipPlayer::GetDuration(VO_U32 *duration)
{
	int d = 0;
	VO_S32 nRc = m_pVompCEngine->GetDuration(&d );
	*duration =(VO_U32) d;
	return nRc;
}
VO_S32 CClipPlayer::GetClipThumbNail(VO_U32 pos, VO_VIDEO_BUFFER **vBuffer )
{
	VOMP_BUFFERTYPE *pOutBuf;
	pOutBuf->pBuffer = (VO_PBYTE)*vBuffer; 
	return SetPosition(&pOutBuf,&pos, VOEDT_PID_VIDEO_SAMPLE);
}
VO_S32 CClipPlayer::SetPosition(VOMP_BUFFERTYPE **pOutBuffer,VO_U32 *nDestSampe_Time,VO_U32 nSampleType)
{
	VOLOGI("+ SetPosition.Pos:%d", *nDestSampe_Time);
	VO_S32 nRc = 0;

	//trim must be reset file position.
	if(m_pTrimCtrl)m_pTrimCtrl->SeekPos(nDestSampe_Time);

	VO_U32 duration ;
	GetDuration(&duration);
	if(*nDestSampe_Time >= duration)
		*nDestSampe_Time = duration - GetIntervalTime();//- 200;
	nRc = SetCurPos(*nDestSampe_Time);
	CHECK_FAIL(nRc);
	//m_pPresentBuffer = NULL;
	m_llTime_previous = -1;
	m_bStopPlay = false;

	VOMP_BUFFERTYPE *pSampleBuffer;// = &m_CurrentVideoBuffer;
	int sType = 0;
	if(nSampleType == VOEDT_PID_VIDEO_SAMPLE) {
		sType = VOMP_PID_VIDEO_SAMPLE ;
		pSampleBuffer = &m_CurrentVideoBuffer;
	}
	else {
		sType = VOMP_PID_AUDIO_SAMPLE;
		m_CurrentAudioBuffer.nSize = m_nAudioMaxSize;
		pSampleBuffer = &m_CurrentAudioBuffer;
	}

	while(!m_bStopPlay)
	{
		 nRc = GetASample(sType,&pSampleBuffer);
		if(nRc == VOMP_ERR_EOS)
		{ 
			VOLOGI("Meet EOS");
			break;
		}
		//pSampleBuffer = &pSampleBuffer;
		m_nCurPosNow = pSampleBuffer->llTime;
		if(m_nCurPosNow >=*nDestSampe_Time )
		{
			if(sType == VOMP_PID_VIDEO_SAMPLE)
			{
				/*EX_COPY_BUFFER(m_sVideoFormat.Height,(&m_Previous_VideoBuffer),m_pPresentBuffer,1);*/
				m_pPresentBuffer = pSampleBuffer;
				m_pPresentBuffer->llTime -= GetIntervalTime();
				if(m_pPresentBuffer ->llTime <= 0) m_pPresentBuffer->llTime = 0;
				if(m_bDropBlackFrame == VOEDT_PID_DROP_BLACK_FRAME)
					if(CheckBlackFrame((VO_VIDEO_BUFFER*)pSampleBuffer->pBuffer,&m_sVideoFormat) == VO_TRUE) continue;

			}
			else if(sType == VOMP_PID_AUDIO_SAMPLE)
			{

				break;
			}
			break;
		}
	}
	*pOutBuffer = pSampleBuffer;
	*nDestSampe_Time = pSampleBuffer->llTime;

	VOLOGI("- SetPosition.Pos:%d", *nDestSampe_Time);
	return  nRc;
}

VO_S32 CClipPlayer::GetTrimSample(VO_S32 nSampleType, VO_U32 *nTime)
{
	VO_U32 nRc = VO_ERR_NONE;
	 if(m_pTrimCtrl)	nRc = m_pTrimCtrl->GetSample(nSampleType, nTime);	
	 else
	 {
		 nRc = VO_ERR_FAILED;
	 }
	 return nRc;
}
VO_S32 CClipPlayer::InitTrimCtrl(VO_VOID* pUserData, VOEDTENCODERCALLBACKPROC pProc)
{
	VO_U32 nRc = VO_ERR_NONE;
	m_pTrimCtrl = new CTrimCtrl(pUserData, pProc);
#ifdef _WIN32
	VO_TCHAR file[255];
	MultiByteToWideChar (CP_ACP, 0, m_cTitle, -1, file, sizeof (file));
	nRc =  m_pTrimCtrl->OpenFile((VO_VOID*)file);
#else
	nRc =  m_pTrimCtrl->OpenFile((VO_VOID*)m_cTitle);
#endif
	if(nRc != VO_ERR_NONE) {delete m_pTrimCtrl; m_pTrimCtrl = NULL;}
	return nRc;
}

