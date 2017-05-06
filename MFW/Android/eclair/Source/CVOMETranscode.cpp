	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoWindow.cpp

	Contains:	CVideoWindow class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/

#include "voOMXOSFun.h"

#include "CVOMETranscode.h"

#define LOG_TAG "CVOMETranscode"
#include "voLog.h"

//#pragma warning (disable : 4996)

#define OMX_COLOR_Format_QcomHW		0X7FA30C00

CVOMETranscode::CVOMETranscode(void)
	: CVOMERecorder ()
	, m_hInputFile (NULL)
	, m_nDuration (0)
	, m_pVideoResize (NULL)
	, m_pVideoBuffer (NULL)
	, m_nVideoBufSize (0)
	, m_llVideoTime (0)
	, m_nResizeWidth (0)
	, m_nResizeHeight (0)
	, m_llFirstTime (0)
	, m_nEncFrames (0)
	, m_pAudioResample (NULL)
	, m_pAudioBuffer (NULL)
	, m_nAudioBufSize (0)
	, m_llAudioTime (0)
	, m_pFirstItem (NULL)
	, m_pCurrentItem (NULL)
	, m_bTranscoding (OMX_FALSE)
	, m_bStopTrans (OMX_FALSE)
{
	m_bTranscode = true;

	m_vomeSource.Init (vomeSourceCallBack, (OMX_PTR)this);

	OMX_VO_CHECKRENDERBUFFERTYPE	videoRender;
	videoRender.pUserData = (OMX_PTR)this;
	videoRender.pCallBack = vomeSourceVideoBuffer;
	m_vomeSource.SetParam (VOME_PID_VideoRenderBuffer, &videoRender);

	OMX_VO_CHECKRENDERBUFFERTYPE	audioRender;
	audioRender.pUserData = (OMX_PTR)this;
	audioRender.pCallBack = vomeSourceAudioBuffer;
	m_vomeSource.SetParam (VOME_PID_AudioRenderBuffer, &audioRender);

	memset (&m_videoSize, 0, sizeof (m_videoSize));
	memset (&m_audioFormat, 0, sizeof (m_audioFormat));

//	AddVideoSource (_T("/sdcard/1_03.mp4"), 0, 0);
}

CVOMETranscode::~CVOMETranscode ()
{
	if (m_pVideoBuffer != NULL)
		delete []m_pVideoBuffer;
	if (m_pAudioBuffer != NULL)
		delete []m_pAudioBuffer;

	if (m_pVideoResize != NULL)
		delete m_pVideoResize;
	if (m_pAudioResample != NULL)
		delete m_pAudioResample;

	COMXTranscodeItem * pItem = m_pFirstItem;
	COMXTranscodeItem * pCurItem = m_pFirstItem;
	while (pItem != NULL)
	{
		pCurItem = pItem;
		pItem = pItem->m_pNext;

		delete pCurItem;
	}

	if (m_hInputFile != NULL)
		fclose (m_hInputFile);
}

OMX_ERRORTYPE CVOMETranscode::AddVideoSource (VO_TCHAR * pVideoFile, OMX_S32 nFD, OMX_S32 nBeginPos, OMX_S32 nEndPos)
{
	COMXTranscodeItem * pNewItem = new COMXTranscodeItem ();
	if (pNewItem == NULL)
		return OMX_ErrorInsufficientResources;

	if (pVideoFile != NULL)
		vostrcpy (pNewItem->m_szVideoFile, pVideoFile);
	pNewItem->m_nFD = nFD;
	pNewItem->m_nBeginPos = nBeginPos;
	pNewItem->m_nEndPos = nEndPos;

	if (m_pFirstItem == NULL)
	{
		m_pFirstItem = pNewItem;
	}
	else
	{
		COMXTranscodeItem * pItem = m_pFirstItem;
		COMXTranscodeItem * pCurItem = m_pFirstItem;
		while (pItem != NULL)
		{
			pCurItem = pItem;
			pItem = pItem->m_pNext;
		}
		pCurItem->m_pNext = pNewItem;
	}
	
	return OMX_ErrorNone;
}

int CVOMETranscode::SetDataSource (const char *source, int offset)
{
	VOLOGI ("Source %s, offset %d", source, offset);

//	offset = 10000;

    if (strncmp(source, "content://", 10) == 0)
	{
#ifdef _LINUX_ANDROID
		// get a filedescriptor for the content Uri and
		// pass it to the setDataSource(fd) method

		android::String16 url16(source);
		int fd = android::openContentProviderFile(url16);
		if (fd < 0)
		{
			VOLOGE ("Couldn't open fd for %s", source);
			return -1;
		}
		
		m_hInputFile = fdopen (fd, "r");

		AddVideoSource (NULL, fd, offset, 0);

//        close(fd);
#endif // _LINUX_ANDROID
	}
	else
	{
		AddVideoSource ((VO_TCHAR *)source, 0, offset, 0);
	}

	return 0;
}


int CVOMETranscode::Prepare (void)
{
	m_nColorType = OMX_COLOR_FormatYUV420Planar;
//	m_nVideoWidth = 176;
//	m_nVideoHeight = 144;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (m_pFirstItem == NULL)
		return -1;

	errType = TryToTrim ();
	if (errType == OMX_ErrorNone)
		return 0;

	OMX_BOOL	bTransMode = OMX_TRUE;
	VOME_SOURCECONTENTTYPE source;
	memset (&source, 0, sizeof (VOME_SOURCECONTENTTYPE));
	source.nSize = sizeof (VOME_SOURCECONTENTTYPE);
	source.nFlag = 3; // Audio and Video
	if (m_hInputFile != NULL)
	{
		source.nType = VOME_SOURCE_Handle;
		source.pSource = (OMX_PTR)m_hInputFile;
	}
	else
	{
		source.nType = VOME_SOURCE_File;
		source.pSource = m_pFirstItem->m_szVideoFile;
	}

	errType = m_vomeSource.Playback (&source);
	if (errType != OMX_ErrorNone)
		return -1;

	OMX_COMPONENTTYPE * pFileSource = NULL;
	errType = m_vomeSource.GetComponentByName ((char*)"OMX.VisualOn.FileSource", &pFileSource);
	if (pFileSource != NULL)
	{
		OMX_TIME_CONFIG_SEEKMODETYPE seekType;
		seekType.eType = OMX_TIME_SeekModeFast;
		pFileSource->SetConfig (pFileSource, OMX_IndexConfigTimeSeekMode, &seekType);

		if (m_pFirstItem->m_nBeginPos > 0)
		{
			OMX_TIME_CONFIG_TIMESTAMPTYPE timeType;
			timeType.nPortIndex = 0;
			timeType.nTimestamp = m_pFirstItem->m_nBeginPos;
			pFileSource->SetConfig (pFileSource, OMX_IndexConfigTimePosition, &timeType);
		}
	}

	m_vomeSource.GetParam (VOME_PID_VideoSize, &m_videoSize);
	m_vomeSource.GetParam (VOME_PID_AudioFormat, &m_audioFormat);
	m_vomeSource.GetDuration (&m_nDuration);
	m_vomeSource.SetParam (OMX_VO_IndexSetTranscodeMode, &bTransMode);

	// create the output components.
	int nRC = CreateOutputComponents ();
	if (nRC != 0)
		return -1;

	m_nStatus = VOME_STATUS_Loaded;

	return 0;
}

int CVOMETranscode::Start (void)
{
	if (m_nStatus == VOME_STATUS_Loaded || m_nStatus == VOME_STATUS_Stopped)
	{
		m_llFirstTime = 0;
		m_nEncFrames = 0;

		m_vomeOutput.Run ();
	}

	m_vomeSource.Run ();

	m_nStatus = VOME_STATUS_Running;

	return 0;
}

int CVOMETranscode::Pause (void)
{
	m_vomeSource.Pause ();

	m_nStatus = VOME_STATUS_Paused;

	return 0;
}

int CVOMETranscode::Stop (void)
{
	m_bStopTrans = OMX_TRUE;

	m_vomeSource.Stop ();
	m_vomeSource.Close ();

	m_bTranscoding = OMX_FALSE;

	m_vomeOutput.Stop ();
	m_vomeOutput.Close ();

	m_nStatus = VOME_STATUS_Stopped;

	return OMX_ErrorNone;
}

int CVOMETranscode::Close (void)
{ 
	Stop ();

	m_vomeSource.Close ();
	m_vomeOutput.Close ();

	if (m_hInputFile != NULL)
		fclose (m_hInputFile);
	m_hInputFile = NULL;

	return 0;
}

int CVOMETranscode::SendAudioData (VOA_DATA_BUFFERTYPE * pData, int nTime)
{
	return 0;
}

int CVOMETranscode::SendVideoData (VOA_DATA_BUFFERTYPE * pData, int nTime)
{
	return 0;
}

int CVOMETranscode::CreateOutputComponents (void)
{
	m_nVideoBufSize = m_nVideoWidth * m_nVideoHeight * 2;
	if (m_pVideoBuffer == NULL)
		m_pVideoBuffer = new OMX_U8[m_nVideoBufSize];
	if (m_pVideoResize == NULL)
		m_pVideoResize = new CVideoResize ();
	memset (&m_resizeData, 0, sizeof (m_resizeData));
	m_resizeData.nInType = VO_COLOR_YUV_PLANAR420;
	m_resizeData.nOutType = VO_COLOR_YUV_PLANAR420;
	m_resizeData.nOutWidth = m_nVideoWidth;
	m_resizeData.nOutHeight = m_nVideoHeight;
	m_resizeData.nOutStride = m_nVideoWidth;
	m_resizeData.nOutUVStride = m_nVideoWidth / 2;
	m_resizeData.pOutBuf[0] = m_pVideoBuffer;
	m_resizeData.pOutBuf[1] = m_pVideoBuffer + m_nVideoWidth * m_nVideoHeight;
	m_resizeData.pOutBuf[2] = m_pVideoBuffer + m_nVideoWidth * m_nVideoHeight * 5 / 4;

	int nRC = CVOMERecorder::CreateOutputComponents ();

	if (m_pVideoCap != NULL)
	{
		OMX_U32 nSourceType = 1;
		m_pVideoCap->SetParameter(m_pVideoCap, (OMX_INDEXTYPE)OMX_VO_IndexSourceType, &nSourceType);
	}

	if (m_pVideoEnc != NULL)
	{
		OMX_VIDEO_CONFIG_BITRATETYPE cfgBitrate;
		cfgBitrate.nPortIndex = 1;
		cfgBitrate.nEncodeBitrate = 1024 * 64;
		m_pVideoEnc->SetConfig (m_pVideoEnc, OMX_IndexConfigVideoBitrate, &cfgBitrate);
	}

	if (m_nColorType == OMX_COLOR_FormatYUV420SemiPlanar)
		m_resizeData.nOutType = VO_COLOR_YUV_420_PACK_2;

	return nRC;
}

OMX_ERRORTYPE CVOMETranscode::TryToTrim (void)
{
	if (m_pFirstItem == NULL)
		return OMX_ErrorBadParameter;
	if (m_pFirstItem->m_pNext != NULL)
		return OMX_ErrorBadParameter;

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	OMX_COMPONENTTYPE * pFileSource = NULL;
	errType = m_vomeSource.AddComponent ((char*)"OMX.VisualOn.FileSource", &pFileSource);
	if (pFileSource == NULL)
	{
		VOLOGW ("It could not add the file source component!");
		return OMX_ErrorInvalidComponentName;
	}

	if (m_hInputFile != NULL)
	{
		errType = pFileSource->SetParameter (pFileSource, (OMX_INDEXTYPE)OMX_VO_IndexSourceHandle, m_hInputFile);
	}
	else
	{
#ifdef _WIN32
		OMX_U32 uSize = (wcslen ((wchar_t *)m_pFirstItem->m_szVideoFile) + 1) * 2;
#elif defined _LINUX
		OMX_U32 uSize = (strlen ((OMX_STRING)m_pFirstItem->m_szVideoFile) + 1);
#endif // _WIN32

		OMX_PARAM_CONTENTURITYPE * pContent = (OMX_PARAM_CONTENTURITYPE *)malloc (uSize + 8);
		if (pContent == NULL)
		{
			VOLOGE ("It could not alloc the memory!");
			return OMX_ErrorInvalidState;
		}

		pContent->nSize = uSize + 8;
		memcpy (pContent->contentURI, m_pFirstItem->m_szVideoFile, uSize);
		errType = pFileSource->SetParameter (pFileSource, OMX_IndexParamContentURI, pContent);
		free (pContent);
	}

	OMX_PARAM_PORTDEFINITIONTYPE   ptOutput;
	ptOutput.nPortIndex = 0;

	OMX_S32		nAudioPort = 0;
	OMX_S32		nVideoPort = 0;
	OMX_PORT_PARAM_TYPE	typePort;
	errType = pFileSource->GetParameter (pFileSource, OMX_IndexParamAudioInit, &typePort);
	if (errType == OMX_ErrorNone && typePort.nPorts > 0)
	{
		nAudioPort = typePort.nStartPortNumber;
		ptOutput.nPortIndex = typePort.nStartPortNumber;
		errType = pFileSource->GetParameter(pFileSource, OMX_IndexParamPortDefinition, &ptOutput);

		if (ptOutput.format.audio.eEncoding != m_omxAudioEnc)
		{
			VOLOGI ("The audio encoder is different. Source %d, Dest %d", ptOutput.format.audio.eEncoding, m_omxAudioEnc);
			return OMX_ErrorNotImplemented;
		}
	}

	errType = pFileSource->GetParameter (pFileSource, OMX_IndexParamVideoInit, &typePort);
	if (errType == OMX_ErrorNone && typePort.nPorts > 0)
	{
		nVideoPort = typePort.nStartPortNumber;

		ptOutput.nPortIndex = typePort.nStartPortNumber;
		errType = pFileSource->GetParameter(pFileSource, OMX_IndexParamPortDefinition, &ptOutput);

		if (ptOutput.format.video.eCompressionFormat != m_omxVideoEnc)
		{
			VOLOGI ("The video encoder is different. Source %d, Dest %d", ptOutput.format.video.eCompressionFormat, m_omxVideoEnc);
			return OMX_ErrorNotImplemented;
		}

		if (ptOutput.format.video.nFrameWidth !=(OMX_U32)m_nVideoWidth || ptOutput.format.video.nFrameHeight != (OMX_U32)m_nVideoHeight)
		{
			VOLOGI ("The video size is different. Source %d X %d, Dest %d X %d", (int)ptOutput.format.video.nFrameWidth, 
						(int)ptOutput.format.video.nFrameHeight, m_nVideoWidth, m_nVideoHeight);
			return OMX_ErrorNotImplemented;
		}
	}

	OMX_TIME_CONFIG_SEEKMODETYPE seekType;
	seekType.eType = OMX_TIME_SeekModeFast;
	pFileSource->SetConfig (pFileSource, OMX_IndexConfigTimeSeekMode, &seekType);

	if (m_pFirstItem->m_nBeginPos > 0)
	{
		OMX_TIME_CONFIG_TIMESTAMPTYPE timeType;
		timeType.nPortIndex = 0;
		timeType.nTimestamp = m_pFirstItem->m_nBeginPos;
		pFileSource->SetConfig (pFileSource, OMX_IndexConfigTimePosition, &timeType);
	}

	OMX_COMPONENTTYPE * pFileSink = NULL;
	if (m_vomeSource.AddComponent((char*)"OMX.VisualOn.FileSink",   &pFileSink) != OMX_ErrorNone)
	{
		VOLOGW ("It could not add the file sink component!");
		return OMX_ErrorComponentNotFound;
	}

	if (m_vomeSource.ConnectPorts(pFileSource, nAudioPort, pFileSink, 0, OMX_FALSE) != OMX_ErrorNone)
	{
		VOLOGW ("It could not connnect the audio port!");
		return OMX_ErrorUndefined;
	}

	if (m_vomeSource.ConnectPorts(pFileSource, nVideoPort, pFileSink, 1, OMX_FALSE) != OMX_ErrorNone)
	{
		VOLOGW ("It could not connect the video port!");
		return OMX_ErrorUndefined;
	}

	// setup the file sink components
	if (m_pOutputFile != NULL)
	{
//		VOLOGI ("output file name : %s", m_pOutputFile);

		OMX_PARAM_CONTENTURITYPE pContent[256];
		strcpy((char *)pContent->contentURI, m_pOutputFile);
		pContent->nSize = sizeof(pContent[0]) + strlen((char *)pContent->contentURI) + 1;
		pFileSink->SetParameter(pFileSink, OMX_IndexParamContentURI, pContent);
	}

//	pFileSink->SetParameter(pFileSink, (OMX_INDEXTYPE)OMX_VO_IndexSourcePipe, &m_FilePipe);
	pFileSink->SetParameter(pFileSink, (OMX_INDEXTYPE)OMX_VO_IndexSourceHandle, m_hFile);
	pFileSink->SetParameter(pFileSink, (OMX_INDEXTYPE)OMX_VO_IndexSourceOffset, &m_nOffset);
	pFileSink->SetParameter(pFileSink, (OMX_INDEXTYPE)OMX_VO_IndexSourceLength, &m_nLength);

	if (m_nMaxFileSize > 0)
		pFileSink->SetParameter(pFileSink, (OMX_INDEXTYPE)OMX_VO_IndexFileMaxSize, &m_nMaxFileSize);

	if (m_nMaxDuration > 0)
		pFileSink->SetParameter(pFileSink, (OMX_INDEXTYPE)OMX_VO_IndexFileMaxDuration, &m_nMaxDuration);

	OMX_BOOL	bTransMode = OMX_TRUE;
	m_vomeSource.SetParam (OMX_VO_IndexSetTranscodeMode, &bTransMode);

//	VOLOGI ("Start to trim media file.");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE CVOMETranscode::TransVideoBuffer (OMX_BUFFERHEADERTYPE * pHeadBuffer, OMX_VO_VIDEOBUFFERTYPE * pVideoBuffer)
{
	//OMX_ERRORTYPE errType = OMX_ErrorNone;

	if (pVideoBuffer->Color == OMX_COLOR_Format_QcomHW || pVideoBuffer->Color == OMX_COLOR_FormatYUV420SemiPlanar)
		m_resizeData.nInType = VO_COLOR_YUV_420_PACK_2;

	if (m_nFrameRate > 0)
	{
		OMX_S64 nEncFrameTime = 1000 * m_nEncFrames / m_nFrameRate;

		if (m_llFirstTime == 0)
			m_llFirstTime = pHeadBuffer->nTimeStamp;

//		VOLOGI ("FrameRate  %d Enc %d, video %d ", m_nFrameRate, (int)nEncFrameTime, (int)(pHeadBuffer->nTimeStamp - m_llFirstTime));

		if (pHeadBuffer->nTimeStamp - m_llFirstTime < nEncFrameTime)
			return OMX_ErrorNone;

		m_nEncFrames++;
		pHeadBuffer->nTimeStamp = nEncFrameTime;
	}

	if (pVideoBuffer->Width == (OMX_U32)m_nVideoWidth && pVideoBuffer->Height == (OMX_U32)m_nVideoHeight &&
		pVideoBuffer->Color == m_nColorType)
	{
		OMX_U32 i = 0;
		OMX_U8 * pOutBuff = m_pVideoBuffer;
		if (pVideoBuffer->Color == OMX_COLOR_FormatYUV420Planar)
		{
			for (i = 0; i < pVideoBuffer->Height; i++)
				memcpy (pOutBuff + pVideoBuffer->Width * i, pVideoBuffer->Buffer[0] + pVideoBuffer->Stride[0] * i, pVideoBuffer->Width);

			pOutBuff = m_pVideoBuffer + pVideoBuffer->Width * pVideoBuffer->Height;
			for (i = 0; i < pVideoBuffer->Height / 2; i++)
				memcpy (pOutBuff + (pVideoBuffer->Width / 2) * i, pVideoBuffer->Buffer[1] + pVideoBuffer->Stride[1] * i, pVideoBuffer->Width / 2);

			pOutBuff = m_pVideoBuffer + pVideoBuffer->Width * pVideoBuffer->Height * 5 / 4;
			for (i = 0; i < pVideoBuffer->Height / 2; i++)
				memcpy (pOutBuff + (pVideoBuffer->Width / 2) * i, pVideoBuffer->Buffer[2] + pVideoBuffer->Stride[2] * i, pVideoBuffer->Width / 2);
		}
	}
	else
	{
		if (m_pVideoResize == NULL)
			m_pVideoResize = new CVideoResize ();

		if ((OMX_U32)(m_resizeData.nInWidth) != pVideoBuffer->Width || (OMX_U32)(m_resizeData.nInHeight) != pVideoBuffer->Height)
		{
			m_nResizeWidth = m_nVideoWidth;
			m_nResizeHeight = m_nVideoHeight;

			if (pVideoBuffer->Width * m_nResizeHeight >= pVideoBuffer->Height * m_nResizeWidth)
				m_nResizeHeight = m_nResizeWidth * pVideoBuffer->Height / pVideoBuffer->Width;
			else if (pVideoBuffer->Width * m_nResizeHeight < pVideoBuffer->Height * m_nResizeWidth)
				m_nResizeWidth = m_nResizeHeight * pVideoBuffer->Width / pVideoBuffer->Height;
				
			m_nResizeWidth = m_nResizeWidth & ~0X03;
			m_nResizeHeight = m_nResizeHeight & ~0X01;

			m_resizeData.nInWidth = pVideoBuffer->Width;
			m_resizeData.nInHeight = pVideoBuffer->Height;

			m_resizeData.nOutWidth = m_nResizeWidth;
			m_resizeData.nOutHeight = m_nResizeHeight;

			m_resizeData.pOutBuf[0] = m_pVideoBuffer + ((m_nVideoHeight - m_nResizeHeight) / 2) * m_nVideoWidth + (m_nVideoWidth - m_nResizeWidth) / 2;
			if (m_resizeData.nOutType == VO_COLOR_YUV_420_PACK_2)
			{
				m_resizeData.pOutBuf[1] = m_pVideoBuffer + m_nVideoWidth * m_nVideoHeight + ((m_nVideoHeight - m_nResizeHeight) / 4) * m_nVideoWidth + (m_nVideoWidth - m_nResizeWidth) / 2;
			}
			else
			{
				m_resizeData.pOutBuf[1] = m_pVideoBuffer + m_nVideoWidth * m_nVideoHeight + ((m_nVideoHeight - m_nResizeHeight) / 4) * (m_nVideoWidth / 2) + (m_nVideoWidth - m_nResizeWidth) / 4;
				m_resizeData.pOutBuf[2] = m_pVideoBuffer + m_nVideoWidth * m_nVideoHeight * 5 / 4 +  + ((m_nVideoHeight - m_nResizeHeight) / 4) * (m_nVideoWidth / 2) + (m_nVideoWidth - m_nResizeWidth) / 4;
			}

			memset (m_pVideoBuffer, 0, m_nVideoWidth * m_nVideoHeight);
			memset (m_pVideoBuffer + m_nVideoWidth * m_nVideoHeight, 127, m_nVideoWidth * m_nVideoHeight);

			VOLOGI ("Video Size: %d X %d,   Resize: %d X %d", m_nVideoWidth, m_nVideoHeight, (int)m_nResizeWidth, (int)m_nResizeHeight);
		}

		m_resizeData.pInBuf[0] = pVideoBuffer->Buffer[0];
		m_resizeData.pInBuf[1] = pVideoBuffer->Buffer[1];
		m_resizeData.pInBuf[2] = pVideoBuffer->Buffer[2];
		m_resizeData.nInStride = pVideoBuffer->Stride[0];
		m_resizeData.nInUVStride = pVideoBuffer->Stride[1];

		m_resizeData.nIsResize = 1;

		m_pVideoResize->ResizeBuffer (&m_resizeData);
	}

	if (m_pVideoCap != NULL)
	{
		m_bufVideo.nTimeStamp    = pHeadBuffer->nTimeStamp;
		m_bufVideo.pBuffer       = m_pVideoBuffer;
		m_bufVideo.nOffset       = 0;
		m_bufVideo.nFilledLen    = m_nVideoWidth * m_nVideoHeight * 3 / 2;

		// VOLOGI ("Time: %d", m_bufVideo.nTimeStamp - m_llVideoTime);


		while (m_pVideoCap->SetParameter(m_pVideoCap, (OMX_INDEXTYPE)OMX_VO_IndexSendBuffer, &m_bufVideo) != OMX_ErrorNone)
		{
			voOMXOS_Sleep (1);
			if (m_bStopTrans)
				return OMX_ErrorNone;
		}

		m_llVideoTime = m_bufVideo.nTimeStamp;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE CVOMETranscode::TransAudioBuffer (OMX_BUFFERHEADERTYPE * pAudioBuffer, OMX_AUDIO_PARAM_PCMMODETYPE * pAudioFormat)
{
	if (m_pAudioResample == NULL && (pAudioFormat->nSamplingRate !=(OMX_U32) m_nSampleRate || pAudioFormat->nChannels != (OMX_U32)m_nChannels))
	{
		if (m_pAudioResample == NULL)
			m_pAudioResample = new CAudioResample ();

		if (m_pAudioBuffer == NULL)
		{
			m_nAudioBufSize = m_nSampleRate * m_nChannels * 2;
			m_pAudioBuffer = new OMX_U8[m_nAudioBufSize];
		}	

		m_pAudioResample->SetFormat (m_audioFormat.nSampleRate, m_audioFormat.nChannels, m_nSampleRate, m_nChannels);
	}

	if (pAudioFormat->nSamplingRate != m_audioFormat.nSampleRate || pAudioFormat->nChannels != m_audioFormat.nChannels)
	{
		m_audioFormat.nSampleRate = pAudioFormat->nSamplingRate;
		m_audioFormat.nChannels = pAudioFormat->nChannels;
		m_pAudioResample->SetFormat (m_audioFormat.nSampleRate, m_audioFormat.nChannels, m_nSampleRate, m_nChannels);
	}

	m_bufAudio.nTimeStamp = pAudioBuffer->nTimeStamp;
	if (m_pAudioResample == NULL)
	{
		m_bufAudio.pBuffer = pAudioBuffer->pBuffer;
		m_bufAudio.nFilledLen = pAudioBuffer->nFilledLen;
	}
	else
	{
		m_audioInBuff.Buffer = pAudioBuffer->pBuffer;
		m_audioInBuff.Length = pAudioBuffer->nFilledLen;

		m_audioOutBuff.Buffer = m_pAudioBuffer;
		m_audioOutBuff.Length = m_nAudioBufSize;
		
		m_pAudioResample->ResampleAudio (&m_audioInBuff, &m_audioOutBuff);

		m_bufAudio.pBuffer = m_pAudioBuffer;
		m_bufAudio.nFilledLen = m_audioOutBuff.Length;
	}
	
//	VOLOGI ("Audio Input Time: %d, Step %d, Data: Size %d, Time %d", (int)pAudioBuffer->nTimeStamp, (int)(pAudioBuffer->nTimeStamp - m_llAudioTime), pAudioBuffer->nFilledLen, (pAudioBuffer->nFilledLen * 1000) / (pAudioFormat->nSamplingRate * pAudioFormat->nChannels * 2));
//	VOLOGI ("Audio Ouput Time: %d, Size %d", m_bufAudio.nFilledLen / 16, m_bufAudio.nFilledLen);

	m_llAudioTime = pAudioBuffer->nTimeStamp;

	while (m_pAudioCap->SetParameter(m_pAudioCap, (OMX_INDEXTYPE)OMX_VO_IndexSendBuffer, &m_bufAudio) != OMX_ErrorNone)
	{
		voOMXOS_Sleep (1);
		if (m_bStopTrans)
			return OMX_ErrorNone;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE CVOMETranscode::vomeSourceVideoBuffer (OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer,
														OMX_IN OMX_INDEXTYPE nType, OMX_IN OMX_PTR pFormat)
{
	CVOMETranscode * pTrans = (CVOMETranscode *)pAppData;

	return pTrans->TransVideoBuffer (pBuffer, (OMX_VO_VIDEOBUFFERTYPE *)pFormat);
}

OMX_ERRORTYPE CVOMETranscode::vomeSourceAudioBuffer (OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer,
														OMX_IN OMX_INDEXTYPE nType, OMX_IN OMX_PTR pFormat)
{
	CVOMETranscode * pTrans = (CVOMETranscode *)pAppData;

	return pTrans->TransAudioBuffer (pBuffer, (OMX_AUDIO_PARAM_PCMMODETYPE *) pFormat);
}

OMX_S32 CVOMETranscode::vomeSourceCallBack (OMX_PTR pUserData, OMX_S32 nID, OMX_PTR pParam1, OMX_PTR pParam2)
{
	CVOMETranscode * pTransCode = (CVOMETranscode *)pUserData;

	if (nID == VOME_CID_PLAY_FINISHED)
	{
		VOA_NOTIFYEVENT event;
		event.msg = MEDIA_RECORDER_EVENT_INFO;
		event.ext1 = MEDIA_RECORDER_INFO_UNKNOWN;
		event.ext2 = 0;
		//turn on for fixing eclair(2.1) version can not displaying well 
		if (pTransCode->m_pCallBack != NULL)
			pTransCode->m_pCallBack (pTransCode->m_pUserData, VOAR_IDC_notifyEvent, &event, NULL);
	}
	else if (nID == VOME_CID_PLAY_ERROR)
	{
		VOA_NOTIFYEVENT event;
		event.msg = MEDIA_RECORDER_EVENT_ERROR;
		event.ext1 = MEDIA_RECORDER_ERROR_UNKNOWN;
		event.ext2 = 0;
		//turn on for fixing eclair(2.1) version can not displaying well 
		if (pTransCode->m_pCallBack != NULL)
			pTransCode->m_pCallBack (pTransCode->m_pUserData, VOAR_IDC_notifyEvent, &event, NULL);
	}

	return 0;
}
