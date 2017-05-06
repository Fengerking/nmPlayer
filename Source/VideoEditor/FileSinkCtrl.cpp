/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		FileSinkCtrl.cpp

Contains:	FileSinkCtrl class file

Written by:	Leon Huang

Change History (most recent first):
2011-01-06		Leon			Create file
*******************************************************************************/
#include "FileSinkCtrl.h"

CFileSinkCtrl::CFileSinkCtrl(void)
:m_pFileSink(NULL)
{

}

CFileSinkCtrl::~CFileSinkCtrl(void)
{
	if(m_pFileSink)
	{ m_pFileSink->Stop(); m_pFileSink->Uninit(); delete m_pFileSink;m_pFileSink = NULL;}
}

VO_S32 CFileSinkCtrl::SetParam(VO_S32 nPid,void* pValue)
{
	VO_S32 nRc; 
		if(m_pFileSink)
			nRc = m_pFileSink->SetParam(VO_PID_SINK_EXTDATA, pValue);
	return nRc;
}

VO_U32 CFileSinkCtrl::AddASample(VOEDT_ENCODER_SAMPLE *sample)
{
	VO_SINK_SAMPLE ss;
	ss.Buffer = sample->buffer->Buffer;
	ss.Duration = sample->Duration;
	sample->nSampleType == VOMP_PID_AUDIO_SAMPLE ? ss.nAV =0 : ss.nAV = 1;
	ss.Size = sample->buffer->Length;
	ss.Time = sample->buffer->Time;
	if(m_pFileSink) return m_pFileSink->AddSample(&ss);
	return 0;
}
VO_S32 CFileSinkCtrl::Start()
{
	if(m_pFileSink) return m_pFileSink->Start();

	return VO_ERR_FAILED;
}
VO_U32 CFileSinkCtrl::Stop()
{
	if(m_pFileSink){m_pFileSink->Stop(); m_pFileSink->Uninit(); delete m_pFileSink; m_pFileSink = NULL; }
	return 0;
}
VO_S32 CFileSinkCtrl::SetFileSink(VO_TCHAR *dumpFilePath,VO_TCHAR *dumpTmpDir
								  ,VO_U32 nPacketType
								  ,VO_U32 nAudioCodec,VO_AUDIO_FORMAT *fmtAudio
								  ,VO_U32 nVideoCodec,VO_VIDEO_FORMAT *fmtVideo
								  ,VO_LIB_OPERATOR* pLibop)
{
	memset(&m_Source,0, sizeof(m_Source));
	//ZeroMemory(&m_Source , sizeof(VO_FILE_SOURCE));
	m_Source.pSource = (VO_PTCHAR) dumpFilePath;
	m_Source.nFlag   = VO_FILE_TYPE_NAME;
	m_Source.nOffset  = 0;
//	m_Source.nLength = 20*1024*1024;
	m_Source.nMode    = VO_FILE_WRITE_ONLY;
	m_Source.nReserve = (VO_U32)dumpTmpDir;

	Stop();

	m_pFileSink = new CFileSink (NULL, (VO_FILE_FORMAT)nPacketType, NULL, NULL);
	m_pFileSink->SetLibOperator(pLibop);
	VO_U32 nRC = m_pFileSink->Init (&m_Source, nAudioCodec, nVideoCodec);
	if (nRC != VO_ERR_NONE) 
	{
		VOLOGE ("Failed to init File Sink. 0X%08X", nRC);
		delete m_pFileSink; m_pFileSink = NULL;
		return VO_ERR_FAILED;
	}
	nRC |= m_pFileSink->SetParam (VO_PID_AUDIO_FORMAT, fmtAudio);
	nRC |= m_pFileSink->SetParam (VO_PID_VIDEO_FORMAT, fmtVideo);
	if (nRC != VO_ERR_NONE) 
	{
		VOLOGE ("Failed to init File Sink. 0X%08X",nRC);
		delete m_pFileSink; m_pFileSink = NULL;
		return VO_ERR_FAILED;
	}
	return Start();
}