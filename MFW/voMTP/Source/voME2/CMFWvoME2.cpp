#include "voLog.h"
#include "CMFWvoME2.h"
#include "cmnMemory.h"
#include "voOSFunc.h"

CMFWvoME2::CMFWvoME2(void)
:m_pvoME2(NULL)
{
}

CMFWvoME2::~CMFWvoME2(void)
{
}

int	CMFWvoME2::HandleEvent (int nID, void * pParam1, void * pParam2)
{
	if (VOMP_CB_RenderAudio == nID)
	{
		VOMP_AUDIO_FORMAT*	pAudioFormat = (VOMP_AUDIO_FORMAT *)pParam2;
		VOMP_BUFFERTYPE*	pAudioBuffer = (VOMP_BUFFERTYPE *)pParam1;

		MFW_OUTPUT_BUFFER sample;
		sample.bVideo		= VO_FALSE;
		sample.pBuffer[0]	= pAudioBuffer->pBuffer;
		sample.nSize		= pAudioBuffer->nSize;
		sample.llTime		= pAudioBuffer->llTime;
		
		VO_AUDIO_FORMAT fmt;
		fmt.Channels	= pAudioFormat->Channels;
		fmt.SampleBits	= pAudioFormat->SampleBits;
		fmt.SampleRate	= pAudioFormat->SampleRate;

		sample.nReserved1   = &fmt;

		OutputData(&sample);
	}
	else if (VOMP_CB_RenderVideo == nID)
	{
		VOMP_VIDEO_BUFFER* pVideoBuffer = (VOMP_VIDEO_BUFFER *)pParam1;
		VOMP_VIDEO_FORMAT* pVideoFormat = (VOMP_VIDEO_FORMAT *)pParam2;

		MFW_OUTPUT_BUFFER sample;
		sample.bVideo		= VO_TRUE;
		sample.pBuffer[0]	= (VO_BYTE*)pVideoBuffer->Buffer[0];
		sample.pBuffer[1]	= (VO_BYTE*)pVideoBuffer->Buffer[1];
		sample.pBuffer[2]	= (VO_BYTE*)pVideoBuffer->Buffer[2];
		sample.nStride[0]	= pVideoBuffer->Stride[0];
		sample.nStride[1]	= pVideoBuffer->Stride[1];
		sample.nStride[2]	= pVideoBuffer->Stride[2];
		sample.llTime		= pVideoBuffer->Time;
		sample.nColorType	= pVideoBuffer->ColorType;

		VO_VIDEO_FORMAT fmt;
		fmt.Width	= pVideoFormat->Width;
		fmt.Height	= pVideoFormat->Height;
		fmt.Type	= (VO_VIDEO_FRAMETYPE)pVideoFormat->Type;

		sample.nReserved1	= &fmt;

		OutputData(&sample);
	}
	else if (nID == VOMP_CB_OpenSource)
	{
	}
	else if (nID == VOMP_CB_PlayComplete)
	{
	}
	else if (nID == VOMP_CB_Error)
	{
	}
	else if (nID == VOMP_CB_VideoStartBuff)
	{
	}
	else if (nID == VOMP_CB_VideoStopBuff)
	{
	}
	else if (nID == VOMP_CB_SourceBuffTime)
	{
	}
	else if (nID == VOMP_CB_BufferStatus)
	{
	}
	else if (nID == VOMP_CB_DownloadPos)
	{

	}
	else if (nID == VOMP_CB_SeekComplete)
	{
		VOLOGI ("VOMP_CB_SeekComplete  222");
	}
	else if (nID == VOMP_CB_VideoDelayTime)
	{
	}
	else if (nID == VOMP_CB_VideoLastTime)
	{
	}
	else if (nID == VOMP_CB_VideoDropFrames)
	{
		VOLOGI ("VOMP_CB_VideoDropFrames");
	}
	else if (nID == VOMP_CB_Error)
	{
	}


	return 0;
}

int HandleEvent_CB (void* pUserData, int nID, void * pParam1, void * pParam2)
{
	CMFWvoME2* pvoME2 = (CMFWvoME2*)pUserData;
	return pvoME2->HandleEvent(nID, pParam1, pParam2);
}


VO_U32 CMFWvoME2::Open(MFW_OPEN_PARAM* pParam)
{
	Close();

	cmnMemCopy(0, &m_OpenParam, pParam, sizeof(MFW_OPEN_PARAM));

	m_pvoME2	= new vompCEngine(this, HandleEvent_CB);
	VO_U32 rc	= m_pvoME2->SetParam(VOMP_PID_DRAW_VIEW, pParam->hDrawWnd);

	VO_U32 mode	= pParam->nTrasferMode==MTP_PUSH_MODE?VOMP_PUSH_MODE:VOMP_PULL_MODE;
	rc			= m_pvoME2->SetParam (VOMP_PID_AUDIO_PLAYMODE, &mode);
	rc			= m_pvoME2->SetParam (VOMP_PID_VIDEO_PLAYMODE, &mode);

// 	RECT rcDraw;
// 	GetClientRect ((HWND)pParam->hDrawWnd, (LPRECT)&rcDraw);
// 	m_pvoME2->SetParam(VOMP_PID_DRAW_RECT, &rcDraw);

	return VO_ERR_MTP_OK;
}

VO_U32 CMFWvoME2::Close()
{
	if(m_pvoME2)
	{
		m_pvoME2->Stop();
		delete m_pvoME2;
		m_pvoME2 = NULL;
	}
	return VO_ERR_MTP_OK;
}

VO_U32 CMFWvoME2::SetDataSource(VO_PTR pSource)
{
	if(!m_pvoME2)
		return VO_ERR_MTP_NULLPOINTER;

// 	int nCodec = VOMP_VIDEO_CodingH264;
// 	m_pvoME2->SetParam (VOMP_PID_VIDEO_TRACK, &nCodec);
// 	nCodec = VOMP_AUDIO_CodingAAC;
// 	m_pvoME2->SetParam (VOMP_PID_AUDIO_TRACK, &nCodec);


	VO_U32 rc = m_pvoME2->SetDataSource(pSource, 1);

	return rc;
}

VO_U32 CMFWvoME2::SendBuffer(MFW_SAMPLE* pSample)
{
	if(!m_pvoME2)
		return VO_ERR_MTP_NULLPOINTER;

	VO_U32 rc;
	VOMP_BUFFERTYPE sample;
	cmnMemSet(0, &sample, 0, sizeof(VOMP_BUFFERTYPE));

	sample.llTime	= pSample->llTime;
	sample.pBuffer	= pSample->pBuffer;
	sample.nSize	= pSample->nSize&0x7FFFFFFF;
	sample.nFlag	= pSample->nFlag;

	if (pSample->bVideo)
	{
		//sample.nFlag = (pSample->nSize & 0x80000000) ? VOMP_FLAG_BUFFER_KEYFRAME:0;
		
		if(pSample->nSize & 0x80000000)
		{
			//sample.nSize = pSample->nSize;
			//VOLOGI("[MTP] video key frame, TS = %d", pSample->llTime);
		}
	}
	else
	{

	}

	do
	{
		rc = m_pvoME2->SendBuffer(pSample->bVideo, &sample);

		if(rc != VOMP_ERR_None)
			voOS_Sleep(2);

	}while(rc != VOMP_ERR_None);
	

	return rc;
}

VO_U32 CMFWvoME2::SetDrawArea(VO_U16 nLeft, VO_U16 nTop, VO_U16 nRight, VO_U16 nBottom)
{
	return VO_ERR_MTP_NOT_IMPLEMENT;
}

VO_U32 CMFWvoME2::Seek(VO_U64 nPosition)
{
	if(!m_pvoME2)
		return VO_ERR_MTP_NULLPOINTER;

	VO_U32 rc = m_pvoME2->SetCurPos(nPosition);

	return rc;
}

VO_U32 CMFWvoME2::GetPosition(VO_U64* pPosition)
{
	if(!m_pvoME2)
		return VO_ERR_MTP_NULLPOINTER;

	int pos		= 0;
	VO_U32 rc	= m_pvoME2->GetCurPos(&pos);
	*pPosition	= pos;

	return rc;
}

VO_U32 CMFWvoME2::Start()
{
	if(!m_pvoME2)
		return VO_ERR_MTP_NULLPOINTER;

	VO_U32 rc = m_pvoME2->Run();

	return rc;
}

VO_U32 CMFWvoME2::Stop()
{
	if(!m_pvoME2)
		return VO_ERR_MTP_NULLPOINTER;

	VO_U32 rc = m_pvoME2->Stop();

	return rc;
}

VO_U32 CMFWvoME2::Pause()
{
	if(!m_pvoME2)
		return VO_ERR_MTP_NULLPOINTER;

	VO_U32 rc = m_pvoME2->Pause();

	return rc;
}


VO_U32 CMFWvoME2::GetParam(VO_U32 nParamID, VO_PTR pParam)
{
	return m_pvoME2->GetParam(nParamID, pParam);
}

VO_U32 CMFWvoME2::SetParam(VO_U32 nParamID, VO_PTR pParam)
{
	if(VO_MTP_PID_COLOR_TYPE == nParamID)
		nParamID = VOMP_PID_DRAW_COLOR;
	else if(VO_MTP_PID_AUDIO_SINK_BUFFTIME == nParamID)
		nParamID = VOMP_PID_AUDIO_SINK_BUFFTIME;

	return m_pvoME2->SetParam(nParamID, pParam);
}


VO_VOID	CMFWvoME2::Flush()
{
	if (m_pvoME2)
	{
		m_pvoME2->Flush();
	}
}

VO_U32 CMFWvoME2::OnTrackInfo(VO_LIVESRC_TRACK_INFO* pTrackInfo)
{
	VO_U32 nCodec = VOMP_VIDEO_CodingUnused;

	for (VO_U32 n=0; n<pTrackInfo->nTrackCount; n++)
	{
		VOLOGI("[MTP] Live Track ID = %d", (pTrackInfo->pTrackID[n]));

		if(pTrackInfo->pInfo[n].Codec == VO_VIDEO_CodingH264)
		{
			nCodec = VOMP_VIDEO_CodingH264;
			SetParam (VOMP_PID_VIDEO_TRACK, &nCodec);
		}
		else if (pTrackInfo->pInfo[n].Codec == VO_VIDEO_CodingMPEG2)
		{
			nCodec = VOMP_VIDEO_CodingMPEG2;
			SetParam (VOMP_PID_VIDEO_TRACK, &nCodec);
		}
		else if (pTrackInfo->pInfo[n].Codec == VO_AUDIO_CodingAAC)
		{
			nCodec = VOMP_AUDIO_CodingAAC;
			SetParam (VOMP_PID_AUDIO_TRACK, &nCodec);
		}
		else if (pTrackInfo->pInfo[n].Codec == VO_AUDIO_CodingMP3)
		{
			nCodec = VOMP_AUDIO_CodingMP3;
			SetParam (VOMP_PID_AUDIO_TRACK, &nCodec);
		}
	}

	return VO_ERR_MTP_OK;
}




