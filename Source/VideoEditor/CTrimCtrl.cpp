/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CTrimCtrl.cpp

Contains:	CTrimCtrl class file

Written by:	Leon Huang

Change History (most recent first):
2011-01-06		Leon			Create file
*******************************************************************************/

#include "CTrimCtrl.h"

CTrimCtrl::CTrimCtrl(void* pUserData,VOEDTENCODERCALLBACKPROC proc)
:m_pUserData(pUserData)
,m_proc(proc)
,m_pFileSource(NULL)
,m_nAudioTrack(-1)
,m_nVideoTrack(-1)
,m_bStopPlay(VO_FALSE)
{

	m_videoSourceSample.Buffer = NULL;
	m_videoSourceSample.Size = 0;
	m_audioSourceSample.Buffer = NULL;
	m_audioSourceSample.Size = 0;
	m_nCurPosition = 0;
}

CTrimCtrl::~CTrimCtrl()
{
	m_bStopPlay  = VO_TRUE;
	if(m_pFileSource) { m_pFileSource->CloseSource(); delete m_pFileSource;}
}
VO_S32 CTrimCtrl::CloseFile()
{
	if(!m_pFileSource) return VOEDT_ERR_OPEN_FILESOURCE_FAIL;
	VO_S32 nRc = m_pFileSource->CloseSource();
	delete m_pFileSource;
	m_pFileSource = NULL;
	return nRc;
}
VO_S32 CTrimCtrl::OpenFile(VO_PTR pSource)
{
	if(m_pFileSource) CloseFile();

	VO_S32 nRC = av_Open_Input_File(pSource);
	CHECK_FAIL(nRC);

	nRC = av_find_stream_info(m_FileInfo);
	CHECK_FAIL(nRC);
	
	nRC = av_find_track_info(&m_FileInfo);
	CHECK_FAIL(nRC);

	nRC = m_pFileSource->GetTrackParam(m_nVideoTrack, VO_PID_SOURCE_FRAMETIME,&m_nVideoFrameTime);
	m_nVideoFrameTime /=100;

	return VO_ERR_NONE;
}

VO_S32 CTrimCtrl::av_Open_Input_File(VO_PTR pSource)
{
	m_VO_FileFormat = VO_FILE_FFUNKNOWN;
	if ( m_VO_FileFormat == VO_FILE_FFUNKNOWN)
	{
		CFileFormatCheck filCheck(NULL, NULL);

		VO_FILE_SOURCE filSource;
		memset (&filSource, 0, sizeof (VO_FILE_SOURCE));
		filSource.nMode = VO_FILE_READ_ONLY;
		filSource.nFlag = VO_FILE_TYPE_NAME;
		filSource.pSource = pSource;
		filSource.nOffset = 0;
		filSource.nLength = 0;

		m_VO_FileFormat = filCheck.GetFileFormat (&filSource,0xffffffff, 0xffffffff );
	}
	if(m_VO_FileFormat == VO_FILE_FFUNKNOWN)
		return VOEDT_ERR_OPEN_FILESOURCE_FAIL;

	m_pFileSource = new CFileSource(NULL,m_VO_FileFormat,NULL,NULL,NULL);
	m_pFileSource->SetVOSDK (VO_TRUE);
	VO_U32 nRC = m_pFileSource->LoadSource (pSource, m_VO_FileFormat, 0, 0);
	if (nRC != VO_ERR_NONE)
	{
		return VOEDT_ERR_OPEN_FILESOURCE_FAIL;
	}
	return VO_ERR_NONE;
}
VO_S32 CTrimCtrl::av_find_stream_info(VO_SOURCE_INFO &fileInfo)
{
	m_pFileSource->GetSourceInfo (&fileInfo);
	if (fileInfo.Tracks == 0)
		return VOEDT_ERR_INVLAID_FILE;
	return VO_ERR_NONE;
}

VO_S32 CTrimCtrl::av_find_track_info(VO_SOURCE_INFO *fileInfo)
{
	if(!m_pFileSource) return VOEDT_ERR_INVLAID_FILE;
	VO_SOURCE_TRACKINFO	trkInfo;

	for(int k = 0;k<m_FileInfo.Tracks;k++)
	{
		m_pFileSource->GetTrackInfo (k, &trkInfo);

		if  (trkInfo.Type == VO_SOURCE_TT_AUDIO  )
		{	
			m_pFileSource->GetTrackInfo(k,&m_AudioTracktype);
			m_pFileSource->GetAudioFormat (k, &m_fmtAudio);
			m_nAudioCodecType = (VO_AUDIO_CODINGTYPE)trkInfo.Codec;
			m_nAudioTrack = k;
		}
		else if(trkInfo.Type == VO_SOURCE_TT_VIDEO)
		{
			m_pFileSource->GetTrackInfo(k,&m_VideoTracktype);
			m_pFileSource->GetVideoFormat(k,&m_fmtVideo);
			m_nVideoCodecType = (VO_VIDEO_CODINGTYPE)trkInfo.Codec;
			m_nVideoTrack = k;
		}
	}

	return VO_ERR_NONE;
}

VO_S32 CTrimCtrl::SeekPos(VO_U32 *pos)
{
	VO_S32 nRc ;
	VO_S64 poss1= *pos;
	VO_S64 poss2 = *pos;
	nRc = m_pFileSource->SetTrackPos(m_nVideoTrack, &poss1);
	nRc |= m_pFileSource->SetTrackPos(m_nAudioTrack,&poss1);
	m_nCurPosition = m_nStartPosition = poss1;
	return nRc;
}
//VO_BOOL gFirst = VO_FALSE;
VO_S32 CTrimCtrl::GetVideoSample(VO_S32 *nTime)
{
	int track = -1;
	VOEDT_ENCODER_SAMPLE	esample;
	VO_CODECBUFFER		codecbuffer;
	VO_SOURCE_SAMPLE	*sample;
	track = m_nVideoTrack;
	sample = &m_videoSourceSample;
	if(track == -1) return VOEDT_ERR_INVLAID_TRACK;

// 	if(!gFirst)
// 	{
		VO_SINK_EXTENSION sinkExt;
		sinkExt.Buffer = m_VideoTracktype.HeadData ;
		sinkExt.Size = m_VideoTracktype.HeadSize;
		sinkExt.nAV = 1;
		VOEDT_PLAYER_CALLBACK_BUFFER pcb;
		pcb.pUserData = m_pUserData;
		pcb.pData = &sinkExt;
		m_proc(VO_PID_SINK_EXTDATA,&pcb);
// 		gFirst = VO_TRUE;
// 	}

	while(!m_bStopPlay )
	{
		VO_U32 nRC = m_pFileSource->GetTrackData (track, sample);

		if (nRC == VO_ERR_SOURCE_END)
		{
			return VOEDT_ERR_END_OF_FILE;
		}
		else if (nRC == VO_ERR_SOURCE_NEEDRETRY)
		{
			voOS_Sleep(2);
		}
		else if (nRC == VO_ERR_SOURCE_OK)
		{	//skip head-data, if it is?
			if((sample->Buffer[0]== 0x00)
				&&(sample->Buffer[1]== 0x00)
				&&(sample->Buffer[2]== 0x01)
				&&(sample->Buffer[3]== 0xb0)) continue;

			codecbuffer.Buffer = sample->Buffer;
			codecbuffer.Length = sample->Size ;//& 0X7FFFFFFF;
			codecbuffer.Time = *nTime += m_nVideoFrameTime;
			esample.nSampleType = VOEDT_PID_VIDEO_SAMPLE;
			esample.buffer = &codecbuffer;
			//esample.buffer->Time = *nTime + m_nVideoFrameTime;
			esample.pType = sample->Size & 0x80000000 ? VO_VIDEO_FRAME_I :VO_VIDEO_FRAME_P;
			esample.Duration =sample->Duration ;
			//	sample.Duration = 10;
			VOEDT_PLAYER_CALLBACK_BUFFER pcb;
			pcb.pUserData = m_pUserData;
			pcb.pData = &esample;
			if(codecbuffer.Length > 0)	m_proc(-1,&pcb);
			break;
		}
	}
	return VO_ERR_NONE;

}
VO_S32 CTrimCtrl::GetAudioSample(VO_S32 *nTime)
{
	int track = -1;
	VOEDT_ENCODER_SAMPLE	esample;
	VO_CODECBUFFER		codecbuffer;
	VO_SOURCE_SAMPLE	*sample;
	track = m_nAudioTrack;
	sample = &m_audioSourceSample;
	if(track == -1) return VOEDT_ERR_INVLAID_TRACK;
	while(!m_bStopPlay )
	{
		VO_U32 nRC = m_pFileSource->GetTrackData (track, sample);

		if (nRC == VO_ERR_SOURCE_END)
		{
			return VOEDT_ERR_END_OF_FILE;
		}
		else if (nRC == VO_ERR_SOURCE_NEEDRETRY)
		{
			voOS_Sleep(2);
		}
		else if (nRC == VO_ERR_SOURCE_OK)
		{
			codecbuffer.Buffer = &sample->Buffer[7];
			codecbuffer.Length = (sample->Size ) - 7;
			VO_S32 diff_time = sample->Time - m_nCurPosition;// - m_nStartPosition;
			if(diff_time <=0 ) diff_time=0;
			m_nCurPosition = sample->Time;
			codecbuffer.Time =  *nTime += diff_time;
			esample.nSampleType = VOEDT_PID_AUDIO_SAMPLE;
			esample.buffer = &codecbuffer;
		//	esample.buffer->Time = sample->Time;//*nTime ;
	
			VOEDT_PLAYER_CALLBACK_BUFFER pcb;
			pcb.pUserData = m_pUserData;
			pcb.pData = &esample;
			if(codecbuffer.Length > 0)	m_proc(-1,&pcb);
			break;
		}
	}
	return VO_ERR_NONE;

}
VO_S32 CTrimCtrl::GetSample(VO_U32 nSampleType,VO_U32 *nTime)
{
	if(!m_pFileSource) return VOEDT_ERR_INVLAID_FILE;

	if(nSampleType == VOEDT_PID_AUDIO_SAMPLE)
		GetAudioSample((VO_S32*)nTime);
	if(nSampleType == VOEDT_PID_VIDEO_SAMPLE)
		GetVideoSample((VO_S32*)nTime);
	return VO_ERR_NONE;
}