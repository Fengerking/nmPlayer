	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CDataSourcePlayer.cpp

	Contains:	CDataSourcePlayer class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-11-21		JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>

#define LOG_TAG "CDataSourcePlayer"

#include "CDataSourcePlayer.h"
#include "voOMXOSFun.h"

#include "voLog.h"

CDataSourcePlayer::CDataSourcePlayer(void)
	: CVOMEPlayer ()
	, m_pCompSrc (NULL)
	, m_pCompAD (NULL)
	, m_pCompVD (NULL)
	, m_pCompAR (NULL)
	, m_pCompVR (NULL)
	, m_pCompClock (NULL)
{
	memset (&m_bufVideoHead, 0, sizeof (OMX_BUFFERHEADERTYPE));
	m_bufVideoHead.nSize = sizeof (OMX_BUFFERHEADERTYPE);
	m_bufVideoHead.nOutputPortIndex = 1; // Video
	m_bufVideoHead.nTickCount = 1;

	memset (&m_bufAudioHead, 0, sizeof (OMX_BUFFERHEADERTYPE));
	m_bufAudioHead.nSize = sizeof (OMX_BUFFERHEADERTYPE);
	m_bufAudioHead.nOutputPortIndex = 0; // Audio
	m_bufAudioHead.nTickCount = 1;
}

CDataSourcePlayer::~CDataSourcePlayer()
{
}

int CDataSourcePlayer::BuildGraph (void)
{
	voCOMXAutoLock lock (&m_mutDataSource);

	if (m_pCompSrc != NULL)
		return 0;

	if (m_hPlay == NULL)
		return OMX_ErrorInvalidState;

	OMX_VO_CHECKRENDERBUFFERTYPE	audioRender;
	audioRender.pUserData = this;
	audioRender.pCallBack = vomeAudioRenderBufferProc;
	m_fAPI.SetParam (m_hPlay, VOME_PID_AudioRenderBuffer, &audioRender);

	OMX_VO_CHECKRENDERBUFFERTYPE	videoRender;
	videoRender.pUserData = this;
	videoRender.pCallBack = vomeVideoRenderBufferProc;
	m_fAPI.SetParam (m_hPlay, VOME_PID_VideoRenderBuffer, &videoRender);

	m_bSourceLoading = true;
	VOLOGI ("start to build graph!");

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	errType = AddComponent ((char*)"OMX.VisualOn.DataSource", &m_pCompSrc);
	if (errType != OMX_ErrorNone)
		return errType;

	// set the Audio codec
	OMX_PARAM_PORTDEFINITIONTYPE portType;
	memset (&portType, 0, sizeof (OMX_PARAM_PORTDEFINITIONTYPE));
	portType.nPortIndex = 0;
	errType = m_pCompSrc->GetParameter (m_pCompSrc, OMX_IndexParamPortDefinition, &portType);
	if (errType != OMX_ErrorNone)
		return errType;
	portType.format.audio.eEncoding = m_nAudioCoding;
	errType = m_pCompSrc->SetParameter (m_pCompSrc, OMX_IndexParamPortDefinition, &portType);
	if (errType != OMX_ErrorNone)
		return errType;

	// set the Audio format
	OMX_AUDIO_PARAM_PCMMODETYPE tpAudioFormat;
	memset (&tpAudioFormat, 0, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
	tpAudioFormat.nPortIndex = 0;
	errType = m_pCompSrc->GetParameter (m_pCompSrc, OMX_IndexParamAudioPcm, &tpAudioFormat);
	if (errType != OMX_ErrorNone)
		return errType;

	tpAudioFormat.nSamplingRate = 44100;
	tpAudioFormat.nChannels = 2;
	tpAudioFormat.nBitPerSample = 16;
	errType = m_pCompSrc->SetParameter (m_pCompSrc, OMX_IndexParamAudioPcm, &tpAudioFormat);
	if (errType != OMX_ErrorNone)
		return errType;

	// set the video codec and size
	memset (&portType, 0, sizeof (OMX_PARAM_PORTDEFINITIONTYPE));
	portType.nPortIndex = 1;
	errType = m_pCompSrc->GetParameter (m_pCompSrc, OMX_IndexParamPortDefinition, &portType);
	if (errType != OMX_ErrorNone)
		return errType;
	portType.format.video.eCompressionFormat = m_nVideoCoding;
	portType.format.video.nFrameWidth = 320;
	portType.format.video.nFrameHeight = 240;
	errType = m_pCompSrc->SetParameter (m_pCompSrc, OMX_IndexParamPortDefinition, &portType);
	if (errType != OMX_ErrorNone)
		return errType;


	errType = AddComponent ((char*)"OMX.VisualOn.Clock.Time", &m_pCompClock);

	errType = AddComponent ((char*)"OMX.VisualOn.Audio.Decoder.XXX", &m_pCompAD);
	errType = AddComponent ((char*)"OMX.VisualOn.Audio.Sink", &m_pCompAR);

	errType = AddComponent ((char*)"OMX.VisualOn.Video.Decoder.XXX", &m_pCompVD);
	errType = AddComponent ((char*)"OMX.VisualOn.Video.Sink", &m_pCompVR);


	errType = ConnectPorts (m_pCompSrc, 0, m_pCompAD, 0, OMX_TRUE);
	errType = ConnectPorts (m_pCompAD, 1, m_pCompAR, 0, OMX_TRUE);

	errType = ConnectPorts (m_pCompSrc, 1, m_pCompVD, 0, OMX_TRUE);
	errType = ConnectPorts (m_pCompVD, 1, m_pCompVR, 0, OMX_TRUE);

	errType = ConnectPorts (m_pCompClock, 0, m_pCompSrc, 2, OMX_TRUE);
	errType = ConnectPorts (m_pCompClock, 1, m_pCompVR, 1, OMX_TRUE);
	errType = ConnectPorts (m_pCompClock, 2, m_pCompAR, 1, OMX_TRUE);

	OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE clkRef;
	clkRef.eClock = OMX_TIME_RefClockAudio;
	m_pCompClock->SetConfig (m_pCompClock, OMX_IndexConfigTimeActiveRefClock, &clkRef);

	m_nStatus = 0;
	m_bSourceLoading = false;

	return 0;
}

int CDataSourcePlayer::SetPos (int nPos)
{
	return 0;
}

int CDataSourcePlayer::GetPos (int * pPos)
{
	*pPos = 0;

	return 0;
}

int CDataSourcePlayer::GetDuration (int * pPos)
{
	*pPos = 0;

	return 0;
}

int CDataSourcePlayer::SendAudioData (VOA_DATA_BUFFERTYPE * pData, int nTime)
{
	voCOMXAutoLock lock (&m_mutDataSource);

	if (m_pCompSrc == NULL)
		return OMX_ErrorInvalidState;

	m_bufAudioHead.nFilledLen = pData->nSize;
	m_bufAudioHead.pBuffer = pData->pBuffer;
	m_bufAudioHead.nTimeStamp = nTime;
	m_bufAudioHead.nFlags = 0;

	OMX_ERRORTYPE errType = m_pCompSrc->SetParameter (m_pCompSrc, (OMX_INDEXTYPE)OMX_VO_IndexSendBuffer, &m_bufAudioHead);

	return errType;
}

int CDataSourcePlayer::SendVideoData (VOA_DATA_BUFFERTYPE * pData, int nTime)
{
	voCOMXAutoLock lock (&m_mutDataSource);

	if (m_pCompSrc == NULL)
		return OMX_ErrorInvalidState;

	m_bufVideoHead.nFilledLen = pData->nSize;
	m_bufVideoHead.pBuffer = pData->pBuffer;
	m_bufVideoHead.nTimeStamp = nTime;
	m_bufVideoHead.nFlags = 0;

	if ((pData->nFlag & 0X1) == 1)
		m_bufVideoHead.nFlags = OMX_BUFFERFLAG_SYNCFRAME;

	OMX_ERRORTYPE errType = m_pCompSrc->SetParameter (m_pCompSrc, (OMX_INDEXTYPE)OMX_VO_IndexSendBuffer, &m_bufVideoHead);

	return errType;
}

int CDataSourcePlayer::Uninit (bool bSuspend /* = false */)
{
	voCOMXAutoLock lock (&m_mutDataSource);

	int nRC = CVOMEPlayer::Uninit (bSuspend);

	m_pCompSrc = NULL;

	return nRC;
}

OMX_ERRORTYPE CDataSourcePlayer::AddComponent (OMX_STRING pName, OMX_COMPONENTTYPE ** ppComponent)
{
	if (m_hPlay == NULL)
		return OMX_ErrorInvalidState;

	return m_fComp.AddComponent (m_hPlay, pName, ppComponent);
}

OMX_ERRORTYPE CDataSourcePlayer::RemoveComponent (OMX_COMPONENTTYPE * pComponent)
{
	if (m_hPlay == NULL)
		return OMX_ErrorInvalidState;

	return m_fComp.RemoveComponent (m_hPlay, pComponent);
}

OMX_ERRORTYPE CDataSourcePlayer::ConnectPorts (OMX_COMPONENTTYPE * pOutputComp, OMX_U32 nOutputPort,
										OMX_COMPONENTTYPE * pInputComp, OMX_U32 nInputPort, OMX_BOOL bTunnel)
{
	if (m_hPlay == NULL)
		return OMX_ErrorInvalidState;

	return m_fComp.ConnectPorts (m_hPlay, pOutputComp, nOutputPort, pInputComp, nInputPort, bTunnel);
}
