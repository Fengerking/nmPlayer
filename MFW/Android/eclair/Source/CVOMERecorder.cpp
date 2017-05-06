	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVOMERecorder.cpp

	Contains:	CVOMERecorder class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-11-21		JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>

#ifdef _LINUX
#include <dlfcn.h>
#endif // _LINUX

#include "CVOMERecorder.h"
#include "voOMXOSFun.h"
#include "voSource.h"

#define LOG_TAG "CVOMERecorder"
#include "voLog.h"

CVOMERecorder::CVOMERecorder(void)
	: m_nAudioSrc (AUDIO_SOURCE_LIST_END)
	, m_nVideoSrc (VIDEO_SOURCE_LIST_END)
	, m_nOutFormat (OUTPUT_FORMAT_LIST_END)
	, m_nAudioEnc (AUDIO_ENCODER_LIST_END)
	, m_nVideoEnc (VIDEO_ENCODER_LIST_END)
	, m_nStatus (VOME_STATUS_Unknown)
	, m_bTranscode (false)
	, m_nSampleRate (8000)
	, m_nChannels (1)
	, m_nColorType (OMX_COLOR_FormatYUV420SemiPlanar)
	, m_nFrameRate (15)
	, m_nVideoWidth (640)
	, m_nVideoHeight (480)
	, m_pOutputFile (NULL)
	, m_nFD (0)
	, m_nOffset (0)
	, m_nLength (0)
	, m_hFile (NULL)
	, m_nMaxFileSize (0)
	, m_nMaxDuration (0)
	, m_nSysStartTime (0)
	, m_nImgStartTime (0)
	, m_pCallBack (NULL)
	, m_pUserData (NULL)
	, m_bTrackMaxAmplitude (0)
	, m_nMaxAmplitude (0)
	, m_pAudioCap (NULL)
	, m_pVideoCap (NULL)
	, m_pVideoEnc (NULL)								   
{
	memset (&m_bufAudio, 0, sizeof (OMX_BUFFERHEADERTYPE));
	memset (&m_bufVideo, 0, sizeof (OMX_BUFFERHEADERTYPE));
}

CVOMERecorder::~CVOMERecorder()
{
	Close ();
}

int CVOMERecorder::getMaxAmplitude (int *max)
{
	voCOMXAutoLock lock (&m_mutAudio);

	if (m_bTrackMaxAmplitude == false)
		m_bTrackMaxAmplitude = true;

	*max = m_nMaxAmplitude;
	m_nMaxAmplitude = 0;

	return 0;
}

int CVOMERecorder::setAudioSource (audio_source  as)
{
	m_nAudioSrc = as;
	return 0;
}

int CVOMERecorder::setVideoSource (video_source  vs)
{
	m_nVideoSrc = vs;
	return 0;
}

int CVOMERecorder::setOutputFormat (output_format of)
{
	m_nOutFormat = of;
	return 0;
}

int CVOMERecorder::setAudioEncoder (audio_encoder ae)
{
	m_nAudioEnc = ae;

	if (m_nAudioEnc == AUDIO_ENCODER_AMR_NB)
		m_omxAudioEnc = OMX_AUDIO_CodingAMR;
	else if (m_nAudioEnc == AUDIO_ENCODER_AMR_WB)
		m_omxAudioEnc = OMX_AUDIO_CodingAMR;

	return 0;
}

int CVOMERecorder::setVideoEncoder (video_encoder ve)
{
	m_nVideoEnc = ve;

	if (m_nVideoEnc == VIDEO_ENCODER_H263)
	{
		VOLOGI("MAAA Set Video encoder h263");
		m_omxVideoEnc = OMX_VIDEO_CodingH263;
	}
	else if (m_nVideoEnc == VIDEO_ENCODER_MPEG_4_SP)
	{
		VOLOGI("MAAA Set Video encoder MPEG");
		m_omxVideoEnc = OMX_VIDEO_CodingMPEG4;
	}
	else if (m_nVideoEnc == VIDEO_ENCODER_H264)
	{
		VOLOGI("MAAA Set Video encoder h264");
		m_omxVideoEnc = OMX_VIDEO_CodingAVC;
	}

	return 0;
}

int CVOMERecorder::setAudioFormat (int nSampleRate, int nChannels)
{
	m_nSampleRate = nSampleRate;
	m_nChannels = nChannels;
	return 0;
}

int CVOMERecorder::setVideoFrameRate (int fps)
{
	m_nFrameRate = fps;
	return 0;
}

int CVOMERecorder::setColorType (VOA_COLORTYPE nColorType)
{
	if (nColorType == VOA_COLOR_YUV420)
	{
		m_nColorType = OMX_COLOR_FormatYUV420Planar;
		VOLOGI ("The color type is YUV420Plannar  %d", OMX_COLOR_FormatYUV420Planar);
	}
	else
	{
//		m_nColorType = OMX_COLOR_FormatYUV422SemiPlanar;
		m_nColorType = OMX_COLOR_FormatYUV420SemiPlanar;
		VOLOGI ("The color type is YUV420SemiPlannar  %d", OMX_COLOR_FormatYUV420SemiPlanar);
	}

	return 0;
}

int CVOMERecorder::setVideoSize (int w, int h)
{
	m_nVideoWidth = w;
	m_nVideoHeight = h;

	return 0;
}

int CVOMERecorder::SetDataSource (const char *source, int offset)
{
	return 0;
}


int CVOMERecorder::setOutputFile (const char *path)
{
	if (m_pOutputFile == NULL)
		m_pOutputFile = new char[256];

	strcpy (m_pOutputFile, path);

	VOLOGI ("output file name : %s", m_pOutputFile);

	return 0;
}

int CVOMERecorder::setOutputFile (int fd, int64_t offset, int64_t length)
{
	m_nFD = fd;
	m_nOffset = (int)offset;
	m_nLength = (int)length;

	char szLink[128];
	if (m_pOutputFile == NULL)
		m_pOutputFile = new char[256];

	sprintf(szLink, "/proc/self/fd/%d", fd);
	memset(m_pOutputFile, 0, sizeof(m_pOutputFile));

#ifdef _LINUX_ANDROID
	readlink(szLink, m_pOutputFile, sizeof(m_pOutputFile));
#endif // _LINUX_ANDROID

	m_hFile = fdopen (fd, "w");

//	strcpy (m_pOutputFile, "/sdcard/dcim/Camera/111.3gp");
	VOLOGI ("output file name : %s", m_pOutputFile);

	return 0;
}

int CVOMERecorder::setParameters (const char * pParam)
{
	VOLOGI ("the param is %s", pParam);

	char *	pPos = strstr ((char *)pParam, "max-filesize");
	if (pPos != NULL)
	{
		pPos = pPos + strlen ("max-filesize");
		while (*pPos != 0)
		{
			if (*pPos == ' ')
				pPos++;
			else if (*pPos == '=')
				pPos++;
			else
				break;
		}

		sscanf(pPos, "%lld", &m_nMaxFileSize);

		VOLOGI ("the max output file size is %lld", m_nMaxFileSize);
	}

	pPos = strstr ((char *)pParam, "max-duration");
	if (pPos != NULL)
	{
		pPos = pPos + strlen ("max-duration");
		while (*pPos != 0)
		{
			if (*pPos == ' ')
				pPos++;
			else if (*pPos == '=')
				pPos++;
			else
				break;
		}

		sscanf(pPos, "%lld", &m_nMaxDuration);

		VOLOGI ("the max Duration is %lld", m_nMaxDuration);
	}

//	m_nMaxFileSize = 1024 * 300;

	return 0;
}


int CVOMERecorder::vomeOutputCallBack (void* pUserData, int nID, void* pParam1, void* pParam2)
{
	CVOMERecorder * pRecorder = (CVOMERecorder *)pUserData;

	switch(nID)
	{
	case VOME_CID_REC_COMPLETE:
	case VOME_CID_REC_MAXFILESIZE:
	case VOME_CID_REC_MAXDURATION:
	{
		VOA_NOTIFYEVENT event;
		event.msg = MEDIA_RECORDER_EVENT_INFO;
		event.ext2 = 0;
		if (nID == VOME_CID_REC_COMPLETE)
			event.ext1 = MEDIA_RECORDER_INFO_UNKNOWN;
		else if (nID == VOME_CID_REC_MAXFILESIZE)
			event.ext1 = MEDIA_RECORDER_INFO_MAX_DURATION_REACHED;
		else if (nID == VOME_CID_REC_MAXDURATION)
			event.ext1 = MEDIA_RECORDER_INFO_MAX_FILESIZE_REACHED;
		//turn on for fixing eclair(2.1) version can not displaying well 
		if (pRecorder->m_pCallBack != NULL)
			pRecorder->m_pCallBack (pRecorder->m_pUserData, VOAR_IDC_notifyEvent, &event, NULL);
	}
		break;

	case VOME_CID_PLAY_ERROR:
	{
		VOA_NOTIFYEVENT event;
		event.msg = MEDIA_RECORDER_EVENT_ERROR;
		event.ext1 = MEDIA_RECORDER_ERROR_UNKNOWN;
		event.ext2 = 0;
		//turn on for fixing eclair(2.1) version can not displaying well 
		if (pRecorder->m_pCallBack != NULL)
			pRecorder->m_pCallBack (pRecorder->m_pUserData, VOAR_IDC_notifyEvent, &event, NULL);

	}
		break;

	case VOME_CID_PLAY_DEBUGTEXT:
		VOLOGI("%s : %s", "VOME_CID_PLAY_DEBUGTEXT", (char *)pParam1);
		break;

	default:
		break;
	}

	return 0;
}

int CVOMERecorder::Prepare (void)
{
	return 0;
}

int CVOMERecorder::Start (void)
{
	if (CreateOutputComponents () != 0)
		return -1;

	if (m_vomeOutput.Run () != OMX_ErrorNone)
		return -1;

	m_nStatus = VOME_STATUS_Running;

	return 0;
}


int CVOMERecorder::Pause (void)
{
	m_nStatus = VOME_STATUS_Paused;

	return 0;
}

int CVOMERecorder::Stop (void)
{
	if (m_vomeOutput.Stop () != OMX_ErrorNone)
		return -1;

	m_nStatus = VOME_STATUS_Stopped;
	return 0;
}

int CVOMERecorder::Close (void)
{
	m_vomeOutput.Close ();

	if (m_hFile != NULL)
		fclose (m_hFile);
	m_hFile = NULL;
	return 0;
}


int CVOMERecorder::SendAudioData (VOA_DATA_BUFFERTYPE * pData, int nTime)
{
	if (m_nStatus != VOME_STATUS_Running)
		return 0;

	if (m_bTrackMaxAmplitude)
	{
		voCOMXAutoLock lock (&m_mutAudio);

		short v = 0;
		short *p = (short*) pData->pBuffer;
		for (int i = pData->nSize >> 1; i > 0; --i)
		{
			v = *p++;
			if (v < 0)
				v = -v;
			if (v > m_nMaxAmplitude)
				m_nMaxAmplitude = v;
		}
	}

	if (m_pAudioCap != NULL)
	{
		m_bufAudio.nTimeStamp    = nTime;
		m_bufAudio.pBuffer       = pData->pBuffer;
		m_bufAudio.nOffset       = 0;
		m_bufAudio.nFilledLen    = pData->nSize;

//		VOLOGI ("@@@ Audio Buffer %08X, Size %d, Time %d", pData->pBuffer, pData->nSize, (unsigned int)nTime);
		m_pAudioCap->SetParameter(m_pAudioCap, (OMX_INDEXTYPE)OMX_VO_IndexSendBuffer, &m_bufAudio);
	}

	return 0;
}

int CVOMERecorder::SendVideoData (VOA_DATA_BUFFERTYPE * pData, int nTime)
{
	if (m_nStatus != VOME_STATUS_Running)
		return 0;

	if (m_pVideoCap != NULL)
	{
		m_bufVideo.nTimeStamp    = nTime;
		m_bufVideo.pBuffer       = pData->pBuffer;
		m_bufVideo.nOffset       = 0;
		m_bufVideo.nFilledLen    = pData->nSize;

//		VOLOGI ("*** Video Buffer %08X, Size %d, Time %d", pData->pBuffer, pData->nSize, (unsigned int)nTime);
//		voOMXOS_Sleep (100);

/*
		if (m_nSysStartTime == 0)
			m_nSysStartTime = voOMXOS_GetSysTime ();
		int nSysTime = voOMXOS_GetSysTime () - m_nSysStartTime;

		if (m_nImgStartTime == 0)
			m_nImgStartTime = nTime;
		int nImgTime = nTime - m_nImgStartTime;

		if (nImgTime > nSysTime + 10)
			return 0;
*/
		m_pVideoCap->SetParameter(m_pVideoCap, (OMX_INDEXTYPE)OMX_VO_IndexSendBuffer, &m_bufVideo);
	}

	return 0;
}

int CVOMERecorder::SetCallBack (VOACallBack pCallBack, void * pUserData)
{
	m_pCallBack = pCallBack;
	m_pUserData = pUserData;
	return 0;
}

int CVOMERecorder::SetFilePipe (void * pFilePipe)
{
//	m_pFilePipe = (OMX_VO_FILE_OPERATOR *)pFilePipe;
	return 0;
}

int CVOMERecorder::CreateOutputComponents (void)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	if (m_vomeOutput.Init ((VOMECallBack)vomeOutputCallBack, (OMX_PTR)this) != OMX_ErrorNone)
	{
		VOLOGE ("It was failed when init output vome engine!");
		return -1;
	}

	OMX_AUDIO_PARAM_PORTFORMATTYPE AudioPort;
	memset(&AudioPort, 0, sizeof(AudioPort));
	OMX_VIDEO_PARAM_PORTFORMATTYPE VideoPort;
	memset(&VideoPort, 0, sizeof(VideoPort));

	// Setup Component
	OMX_COMPONENTTYPE * g_pAudioEnc = NULL;
	OMX_COMPONENTTYPE * g_pFileSink = NULL;

	// setup the file sink component
	OMX_VO_FILE_FORMATTYPE filType = OMX_VO_FILE_FormatMP4;
	switch(m_nOutFormat)
	{
	case OUTPUT_FORMAT_THREE_GPP:
	case OUTPUT_FORMAT_MPEG_4:
		filType = OMX_VO_FILE_FormatMP4;
		break;

	case OUTPUT_FORMAT_RAW_AMR:
		filType = OMX_VO_FILE_FormatAMRNB;
		break;

	case OUTPUT_FORMAT_DEFAULT:
		break;
	default:
		 break;
	}

	if (m_vomeOutput.AddComponent((char*)"OMX.VisualOn.FileSink",    &g_pFileSink) != OMX_ErrorNone)
	{
		VOLOGE ("It could not add the OMX.VisualOn.FileSink conponent!");
		return -1;
	}

	g_pFileSink->SetParameter(g_pFileSink, (OMX_INDEXTYPE)OMX_VO_IndexFileFormat, &filType);

	// setup the audio components
	if (m_nAudioSrc != AUDIO_SOURCE_LIST_END)
	{
		if (m_nAudioEnc == AUDIO_ENCODER_AMR_NB)
			AudioPort.eEncoding = OMX_AUDIO_CodingAMR;
		else if (m_nAudioEnc == AUDIO_ENCODER_AMR_WB)
			AudioPort.eEncoding = OMX_AUDIO_CodingAMR;

		if (m_vomeOutput.AddComponent((char*)"OMX.VisualOn.G1Audio.Capture",   &m_pAudioCap) != OMX_ErrorNone)
		{
			VOLOGE ("It could not add the OMX.VisualOn.G1Audio.Capture conponent!");
			return -1;
		}

		if (m_vomeOutput.AddComponent((char*)"OMX.VisualOn.Audio.Encoder.XXX", &g_pAudioEnc) != OMX_ErrorNone) 
		{
			VOLOGE ("It could not add the OMX.VisualOn.Audio.Encoder.XXX conponent!");
			return -1;
		}

		OMX_AUDIO_PARAM_PCMMODETYPE ptAudioOutput;
		memset(&ptAudioOutput, 0, sizeof(ptAudioOutput));
		ptAudioOutput.nSize      = sizeof(ptAudioOutput);
		ptAudioOutput.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
		ptAudioOutput.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
		ptAudioOutput.nVersion.s.nRevision     = OMX_VERSION_REVISION;
		ptAudioOutput.nVersion.s.nStep         = OMX_VERSION_STEP;

		ptAudioOutput.nPortIndex = 0;
		errType = m_pAudioCap->GetParameter (m_pAudioCap, OMX_IndexParamAudioPcm, &ptAudioOutput);

		ptAudioOutput.nSamplingRate = m_nSampleRate;
		ptAudioOutput.nChannels = m_nChannels;
		ptAudioOutput.nBitPerSample = 16;
		errType = m_pAudioCap->SetParameter (m_pAudioCap, OMX_IndexParamAudioPcm, &ptAudioOutput);

		AudioPort.nPortIndex = 1;
		g_pAudioEnc->SetParameter(g_pAudioEnc, OMX_IndexParamAudioPortFormat, &AudioPort);

		if (m_vomeOutput.ConnectPorts(m_pAudioCap, 0, g_pAudioEnc, 0, OMX_TRUE) != OMX_ErrorNone)
		{
			VOLOGE ("It could not connect the Audio capture and Audio encode components!");
			return -1;
		}

		AudioPort.nPortIndex = 0;
		g_pFileSink->SetParameter(g_pFileSink, OMX_IndexParamAudioPortFormat, &AudioPort);

		if (m_vomeOutput.ConnectPorts(g_pAudioEnc, 1, g_pFileSink, 0, OMX_TRUE) != OMX_ErrorNone) 
		{
			VOLOGE ("It could not connect the Audio encode and file sink components!");
			return -1;
		}
	}

	// setup the video components
	if (m_nVideoSrc != VIDEO_SOURCE_LIST_END)
	{
		if (m_nVideoEnc == VIDEO_ENCODER_H263)
			VideoPort.eCompressionFormat = OMX_VIDEO_CodingH263;
		else if (m_nVideoEnc == VIDEO_ENCODER_MPEG_4_SP)
			VideoPort.eCompressionFormat = OMX_VIDEO_CodingMPEG4;
		else if (m_nVideoEnc == VIDEO_ENCODER_H264)
			VideoPort.eCompressionFormat = OMX_VIDEO_CodingAVC;

		if (m_vomeOutput.AddComponent((char*)"OMX.VisualOn.G1Video.Capture",   &m_pVideoCap) != OMX_ErrorNone) 
		{
			VOLOGE ("It could not add the video capture components!");
			return -1;
		}

		m_nColorType = OMX_COLOR_FormatYUV420SemiPlanar;
/*
//		if (m_bTranscode)
//			errType = m_vomeOutput.AddComponent((char*)"OMX.VisualOn.Video.Encoder.XXX", &m_pVideoEnc);
//		else 
		{	
			if(m_omxVideoEnc == OMX_VIDEO_CodingMPEG4)
				errType = m_vomeOutput.AddComponent((char*)"OMX.qcom.video.encoder.mpeg4", &m_pVideoEnc);
			else if(m_omxVideoEnc == OMX_VIDEO_CodingH263)
				errType = m_vomeOutput.AddComponent((char*)"OMX.qcom.video.encoder.h263", &m_pVideoEnc);
			else if(m_omxVideoEnc == OMX_VIDEO_CodingAVC)
				errType = m_vomeOutput.AddComponent((char*)"OMX.qcom.video.encoder.avc", &m_pVideoEnc);
		}
		

		if (errType != OMX_ErrorNone)
*/		{
			m_nColorType = OMX_COLOR_FormatYUV420Planar;
			errType = m_vomeOutput.AddComponent((char*)"OMX.VisualOn.Video.Encoder.XXX", &m_pVideoEnc);

			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("It could not add the video encoder component!");
				return -1;
			}
		}

		OMX_PARAM_PORTDEFINITIONTYPE   t;
		memset(&t, 0, sizeof(t));
		t.nSize      = sizeof(t);
		t.nPortIndex = 0;
		//voOMXBase_SetHeader (&ptVideoOutput, sizeof (OMX_PARAM_PORTDEFINITIONTYPE), &verMainNext);
		t.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
		t.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
		t.nVersion.s.nRevision     = OMX_VERSION_REVISION;
		t.nVersion.s.nStep         = OMX_VERSION_STEP;

		m_pVideoCap->GetParameter(m_pVideoCap, OMX_IndexParamPortDefinition, &t);
		VOLOGI("m_pVideoCap->GetParameter(): t.format.video.eColorFormat = %u, t.format.video.nFrameWidth = %u, t.format.video.nFrameHeight = %u",
			(unsigned int)t.format.video.eColorFormat, (unsigned int)t.format.video.nFrameWidth, (unsigned int)t.format.video.nFrameHeight);

		t.format.video.eColorFormat = m_nColorType;
		t.format.video.nFrameWidth  = m_nVideoWidth;
		t.format.video.nFrameHeight = m_nVideoHeight;
		VOLOGI ("m_pVideoCap)->SetParameter(): t.format.video.eColorFormat = %u, W %d, H %d", t.format.video.eColorFormat, m_nVideoWidth, m_nVideoHeight);
		errType = m_pVideoCap->SetParameter(m_pVideoCap, OMX_IndexParamPortDefinition, &t);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("It could not SetParameter video encoder intput port! result %08X", errType);
			return -1;
		}

		if (m_vomeOutput.ConnectPorts(m_pVideoCap, 0, m_pVideoEnc, 0, OMX_TRUE) != OMX_ErrorNone)
		{
			VOLOGE ("It could not connect the video capture and encoder components!");
			return -1;
		}

		VideoPort.nPortIndex = 1;
		m_pVideoEnc->SetParameter(m_pVideoEnc, OMX_IndexParamVideoPortFormat, &VideoPort);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("It could not SetParameter video encoder output port ! result %08X", errType);
			return -1;
		}

		if (VideoPort.eCompressionFormat == OMX_VIDEO_CodingMPEG4)
		{
			OMX_VIDEO_PARAM_MPEG4TYPE mpeg4Type;
			mpeg4Type.nPortIndex = 1;
			errType = m_pVideoEnc->GetParameter (m_pVideoEnc, OMX_IndexParamVideoMpeg4, &mpeg4Type);
			mpeg4Type.nPFrames = 14;
			errType = m_pVideoEnc->SetParameter (m_pVideoEnc, OMX_IndexParamVideoMpeg4, &mpeg4Type);
		}
	
		OMX_CONFIG_FRAMERATETYPE  cfgFrameRate;
		cfgFrameRate.nPortIndex = 1;
		cfgFrameRate.xEncodeFramerate = m_nFrameRate << 16;
		errType = m_pVideoEnc->SetConfig (m_pVideoEnc, OMX_IndexConfigVideoFramerate, &cfgFrameRate);

		OMX_VIDEO_CONFIG_BITRATETYPE cfgBitrate;
		cfgBitrate.nPortIndex = 1;
		cfgBitrate.nEncodeBitrate = m_nVideoWidth * m_nVideoHeight * 20;
		errType = m_pVideoEnc->SetConfig (m_pVideoEnc, OMX_IndexConfigVideoBitrate, &cfgBitrate);

		VideoPort.nPortIndex = 1;
		g_pFileSink->SetParameter(g_pFileSink, OMX_IndexParamVideoPortFormat, &VideoPort);
		if (m_vomeOutput.ConnectPorts(m_pVideoEnc, 1, g_pFileSink, 1, OMX_TRUE) != OMX_ErrorNone)
		{
			VOLOGE ("It could not connect the video encoder and file sink components!");
			return -1;
		}
	}

	// setup the file sink components
	if (m_pOutputFile != NULL)
	{
		VOLOGI ("output file name : %s", m_pOutputFile);

		OMX_PARAM_CONTENTURITYPE pContent[256];
		strcpy((char *)pContent->contentURI, m_pOutputFile);
		pContent->nSize = sizeof(pContent[0]) + strlen((char *)pContent->contentURI) + 1;
		g_pFileSink->SetParameter(g_pFileSink, OMX_IndexParamContentURI, pContent);
	}

//	g_pFileSink->SetParameter(g_pFileSink, (OMX_INDEXTYPE)OMX_VO_IndexSourcePipe, &m_FilePipe);
	g_pFileSink->SetParameter(g_pFileSink, (OMX_INDEXTYPE)OMX_VO_IndexSourceHandle, m_hFile);
	g_pFileSink->SetParameter(g_pFileSink, (OMX_INDEXTYPE)OMX_VO_IndexSourceOffset, &m_nOffset);
	g_pFileSink->SetParameter(g_pFileSink, (OMX_INDEXTYPE)OMX_VO_IndexSourceLength, &m_nLength);

	if (m_nMaxFileSize > 0)
		g_pFileSink->SetParameter(g_pFileSink, (OMX_INDEXTYPE)OMX_VO_IndexFileMaxSize, &m_nMaxFileSize);

	if (m_nMaxDuration > 0)
		g_pFileSink->SetParameter(g_pFileSink, (OMX_INDEXTYPE)OMX_VO_IndexFileMaxDuration, &m_nMaxDuration);

//	m_vomeAPI.Run(m_hHandle);

	m_nStatus = VOME_STATUS_Loaded;

	return 0;
}
