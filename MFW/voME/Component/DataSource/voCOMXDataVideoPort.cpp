	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXDataVideoPort.cpp

	Contains:	voCOMXDataVideoPort class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include "voOMXOSFun.h"
#include "voOMXFile.h"
#include "voCOMXPortClock.h"
#include "voCOMXDataVideoPort.h"

#define LOG_TAG "voCOMXDataVideoPort"
#include "voLog.h"

voCOMXDataVideoPort::voCOMXDataVideoPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex)
	: voCOMXDataOutputPort (pParent, nIndex)
	, m_bDropFrame (false)
{
	strcpy (m_pObjName, __FILE__);
	m_sType.eDomain = OMX_PortDomainVideo;

	m_sType.nBufferSize = 409600;
	m_sType.nBufferCountActual = 1;
	m_sType.nBufferCountMin = 1;
	m_sType.bBuffersContiguous = OMX_FALSE;
	m_sType.nBufferAlignment = 1;

	m_sType.format.video.pNativeRender = NULL;
	m_sType.format.video.bFlagErrorConcealment = OMX_FALSE;

	m_sType.format.video.nFrameWidth = 320;
	m_sType.format.video.nFrameHeight = 240;
	m_sType.format.video.nStride = m_sType.format.video.nFrameWidth;
	m_sType.format.video.nSliceHeight = 16;
	m_sType.format.video.nBitrate = 0;
	m_sType.format.video.xFramerate = (25 << 16) + 1;
	m_sType.format.video.eColorFormat = OMX_COLOR_FormatUnused;
	m_sType.format.video.pNativeWindow = NULL;

	strcpy (m_pMIMEType, "H264");
	m_sType.format.video.cMIMEType = m_pMIMEType;
	m_sType.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
}

voCOMXDataVideoPort::~voCOMXDataVideoPort(void)
{
}

void voCOMXDataVideoPort::SetConfigFile (CBaseConfig * pCfgFile)
{
	voCOMXDataOutputPort::SetConfigFile (pCfgFile);

	if (m_pCfgComponent != NULL)
	{
		char * pFile = m_pCfgComponent->GetItemText (m_pParent->GetName (), (char*)"VideoSourceFile");
		if (pFile != NULL)
		{
#ifdef _WIN32
			TCHAR szFile[256];
			memset (szFile, 0, sizeof (szFile));
			MultiByteToWideChar (CP_ACP, 0, pFile, -1, szFile, sizeof (szFile)); 
			m_hSourceFile = voOMXFileOpen ((OMX_STRING)szFile, VOOMX_FILE_READ_ONLY);		
#else
			m_hSourceFile = voOMXFileOpen (pFile, VOOMX_FILE_READ_ONLY);
#endif // _WIN32
			VOLOGI ("Open source file %s, The handle is 0X%08X.", pFile, (int)m_hSourceFile);
		}
		m_nSourceFormat = m_pCfgComponent->GetItemValue (m_pParent->GetName (), (char*)"VideoSourceFormat", 0);
		if (m_hSourceFile != NULL)
		{
			if (m_nSourceFormat == 0)
			{
				m_nSourceSize = (OMX_S32)voOMXFileSize (m_hSourceFile);
				m_pSourceBuffer = new OMX_U8[m_nSourceSize];
				m_pCurrentBuffer = m_pSourceBuffer;

				voOMXFileRead (m_hSourceFile, m_pSourceBuffer, m_nSourceSize);
			}
			else
			{
				m_nSourceSize = 102400;
				m_pSourceBuffer = new OMX_U8[m_nSourceSize];
				m_pCurrentBuffer = m_pSourceBuffer;
			}
		}

		pFile = m_pCfgComponent->GetItemText (m_pParent->GetName (), (char*)"VideoDumpFile");
		if (pFile != NULL)
		{
#ifdef _WIN32
			TCHAR szFile[256];
			memset (szFile, 0, sizeof (szFile));
			MultiByteToWideChar (CP_ACP, 0, pFile, -1, szFile, sizeof (szFile)); \
			m_hDumpFile = voOMXFileOpen ((OMX_STRING)szFile, VOOMX_FILE_WRITE_ONLY);		
#else
			m_hDumpFile = voOMXFileOpen (pFile, VOOMX_FILE_WRITE_ONLY);
#endif // _WIN32
			VOLOGI ("Create dump file %s, The handle is 0X%08X.", pFile, (int)m_hDumpFile);
		}
		m_nDumpFormat = m_pCfgComponent->GetItemValue (m_pParent->GetName (), (char*)"VideoDumpFormat", 0);

		m_nSyncWord = 0X01000000;

		m_bDropFrame = m_pCfgComponent->GetItemValue (m_pParent->GetName (), (char*)"VideoDropFrame", 0) > 0;
	}
}

OMX_ERRORTYPE voCOMXDataVideoPort::GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam)
{
	//OMX_ERRORTYPE errType = OMX_ErrorUnsupportedIndex;
	switch (nIndex)
	{
	case OMX_IndexParamVideoPortFormat:
		{
			OMX_VIDEO_PARAM_PORTFORMATTYPE * pVideoFormat = (OMX_VIDEO_PARAM_PORTFORMATTYPE *) pParam;
			if (pVideoFormat->nIndex > 0)
				return OMX_ErrorNoMore;

			pVideoFormat->eCompressionFormat = m_sType.format.video.eCompressionFormat;

			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXDataOutputPort::GetParameter (nIndex, pParam);
}

OMX_ERRORTYPE voCOMXDataVideoPort::SetTrackPos (OMX_S64 * pPos)
{
	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXDataVideoPort::SendBuffer (OMX_BUFFERHEADERTYPE* pBuffer)
{
	if (m_hDumpFile != NULL)
	{
		if (m_nDumpFormat >= 1)
			voOMXFileWrite (m_hDumpFile, (OMX_U8*)&pBuffer->nFilledLen, 4);
		if (m_nDumpFormat >= 2)
			voOMXFileWrite (m_hDumpFile, (OMX_U8*)&pBuffer->nTimeStamp, 8);
		voOMXFileWrite (m_hDumpFile, pBuffer->pBuffer, pBuffer->nFilledLen);
	}

	if (m_hSourceFile != NULL)
	{
		voOMXOS_Sleep (10);
		return OMX_ErrorNone;
	}

	if (m_pLstBuffer == NULL)
		m_pLstBuffer = new voCOMXDataBufferList (40960, true);

	if (m_pLstBuffer->GetBuffTime () > m_llMaxBufferTime)
	{
		// make sure the audio port have enough buffer
		if (m_pOtherPort->GetBufferTime () > m_llMinBufferTime)
		{
			return OMX_ErrorOverflow;
		}
	}

	OMX_ERRORTYPE errType = m_pLstBuffer->AddBuffer (pBuffer);

	VOLOGR ("[Video]errType 0x%08X, timestamp %d, system time %d, buffering time %d", errType, (OMX_U32)pBuffer->nTimeStamp, voOMXOS_GetSysTime(), m_pLstBuffer->GetBuffTime());

	return errType;
}

OMX_ERRORTYPE voCOMXDataVideoPort::FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	if (m_hSourceFile != NULL)
	{
		pBuffer->nAllocLen = m_nSourceSize;
		pBuffer->nFlags = 0;
		pBuffer->nFilledLen = 0;
		pBuffer->nTimeStamp = m_llSourceSampleTime;
		m_llSourceSampleTime += 40;

		if (m_nSourceFormat == 0)
		{
			OMX_U8 * pNextData = m_pCurrentBuffer + 4;
			while (true)
			{
				if (pNextData - m_pSourceBuffer >= m_nSourceSize)
				{
					m_pCurrentBuffer = m_pSourceBuffer;
					pNextData = m_pCurrentBuffer + 4;
				}


				if (!memcmp (pNextData, &m_nSyncWord, 4))
				{
					if (pNextData - m_pCurrentBuffer > 32)
						break;
				}

				pNextData++;
			}

			pBuffer->nFilledLen = pNextData - m_pCurrentBuffer;
			pBuffer->pBuffer = m_pCurrentBuffer;

			m_pCurrentBuffer = pNextData;

		}
		else if (m_nSourceFormat == 1)
		{
			int nFrameSize = 0;
			int nReadSize = voOMXFileRead (m_hSourceFile, (OMX_U8 *)&nFrameSize, 4);
			if (nReadSize != 4)
			{
				voOMXFileSeek (m_hSourceFile, 0, VOOMX_FILE_BEGIN);
				voOMXFileRead (m_hSourceFile, (OMX_U8 *)&nFrameSize, 4);
			}
			voOMXFileRead (m_hSourceFile, m_pSourceBuffer, nFrameSize);

			pBuffer->nFilledLen = nFrameSize;
			pBuffer->pBuffer = m_pSourceBuffer;
		}
		else if (m_nSourceFormat == 2)
		{
			int nFrameSize = 0;
			int nReadSize = voOMXFileRead (m_hSourceFile, (OMX_U8 *)&nFrameSize, 4);
			if (nReadSize != 4)
			{
				voOMXFileSeek (m_hSourceFile, 0, VOOMX_FILE_BEGIN);
				voOMXFileRead (m_hSourceFile, (OMX_U8 *)&nFrameSize, 4);
			}

			voOMXFileRead (m_hSourceFile, (OMX_U8*)&m_llSourceSampleTime, 8);
			voOMXFileRead (m_hSourceFile, m_pSourceBuffer, nFrameSize);

			pBuffer->nTimeStamp = m_llSourceSampleTime;
			pBuffer->nFilledLen = nFrameSize;
			pBuffer->pBuffer = m_pSourceBuffer;
		}

		return OMX_ErrorNone;
	}

	OMX_TICKS	mediaTime = 0;
	if (m_nFillBufferCount > 0 && m_pClockPort)
	{
		m_pClockPort->GetMediaTime (&mediaTime);

		if (mediaTime > 0 && m_llLastClockTime == mediaTime)
		{
			if(!(pBuffer->nFlags & OMX_BUFFERFLAG_EOS))
			{
				voOMXOS_Sleep (2);
				return OMX_ErrorUnderflow;
			}
		}

		m_llLastClockTime = mediaTime;
	}

	if (m_bDropFrame)
		pBuffer->nTimeStamp = mediaTime;
	else
		pBuffer->nTimeStamp = 0;

	errType = voCOMXDataOutputPort::FillBuffer (pBuffer);	

	if (mediaTime > 0)
		pBuffer->pOutputPortPrivate = (OMX_PTR)(mediaTime - pBuffer->nTimeStamp);

	if ((m_nLogLevel & 0X0F) > 0)
	{
		VOLOGI ("Read video used time is %d, Sample Size %d, Time: %d, Flags %08X, Result 0X%08X", (int)(voOMXOS_GetSysTime () - m_nLogSysCurrentTime), (int)pBuffer->nFilledLen, (int)pBuffer->nTimeStamp, (int)pBuffer->nFlags, (int)errType);
	}

	return errType;
}

OMX_ERRORTYPE voCOMXDataVideoPort::ReconfigPort(VO_LIVESRC_TRACK_INFOEX * pInfoEx)
{
	VO_SOURCE_TRACKINFO trkInfo;
	memset(&trkInfo, 0, sizeof(trkInfo));
	trkInfo.Type = pInfoEx->Type;
	trkInfo.Codec = pInfoEx->Codec;
	trkInfo.Start = 0;
	trkInfo.Duration = -2;	// invalid
	trkInfo.HeadData = pInfoEx->Padding;
	trkInfo.HeadSize = pInfoEx->HeadSize;
	SetTrackInfo(&trkInfo);

	OMX_PARAM_PORTDEFINITIONTYPE *pType;
	GetPortType(&pType);
	if(pInfoEx->Codec == VO_VIDEO_CodingH264)
		pType->format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
	else if(pInfoEx->Codec == VO_VIDEO_CodingH263)
		pType->format.video.eCompressionFormat = OMX_VIDEO_CodingH263;
	else if(pInfoEx->Codec == VO_VIDEO_CodingMPEG4)
		pType->format.video.eCompressionFormat = OMX_VIDEO_CodingMPEG4;
	else if(pInfoEx->Codec == VO_VIDEO_CodingWMV||pInfoEx->Codec == VO_VIDEO_CodingVC1)
		pType->format.video.eCompressionFormat = OMX_VIDEO_CodingWMV;
	pType->format.video.nFrameWidth = pInfoEx->video_info.Width;
	pType->format.video.nFrameHeight = pInfoEx->video_info.Height;

	VOLOGI ("ReconfigPort Codec %d, Width %d, Height %d", (int)pInfoEx->Codec, (int)pInfoEx->video_info.Width, (int)pInfoEx->video_info.Height);

	return OMX_ErrorNone;
}
