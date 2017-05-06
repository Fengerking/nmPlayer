	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CFileMediaExtractor.cpp

	Contains:	CFileMediaExtractor class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-18		JBF			Create file

*******************************************************************************/
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "voAudio.h"
#include "voVideo.h"
#include "voFile.h"
#include "cmnMemory.h"
#include "voOSFunc.h"
#include "voMetaData.h"

#include "OMX_Video.h"

#include "CFileFormatCheck.h"

#include "CDataSourceOP.h"

#include "CFileMediaExtractor.h"
#include "CFileMediaSource.h"

#include "CRTSPSource.h"
#include "CHTTPPDSource.h"
#include "CLiveSource.h"

#include "ParseSPS.h"
#include "cConstructCodecInfo.h"

#if defined(_LINUX_ANDROID)
#    include <cutils/properties.h>
#endif
#define LOG_TAG "CFileMediaExtractor"
#include "voLog.h"

#define DEFAULT_AUDIO_SAMPLE_RATE	8000

CFileMediaExtractor::CFileMediaExtractor(void * pDataSource, int nFlag,
										 VOSF_MEDIABUFFERTYPE * pMediaBufferOP, VOSF_METADATATYPE * pMetaDataOP)
	: CBaseMediaExtractor (pMediaBufferOP, pMetaDataOP)
	, m_pDataSource ((VOSF_DATASOURCETYPE *)pDataSource)
	, m_pAudioSource (NULL)
	, m_pVideoSource (NULL)
	, m_pNotifyFunc (NULL)
	, m_pUserData (NULL)
	, m_pFileOP (NULL)
	, m_pSource (NULL)
	, m_nTrackCount (0)
	, m_nFF (VO_FILE_FFUNKNOWN)
	, m_llLastSeekPos (0)
	, m_nAudioTrack (-1)
	, m_nAudioTrackIndex (-1)
	, m_nAudioMaxSize (12000)
	, m_nAudioReadSamples (0)
	, mpAudioHead(NULL)
	, mnAudioHeadSize(0)
	, m_nVideoTrack (-1)
	, m_nVideoTrackIndex (-1)
	, m_nVideoMaxSize (102400)
	, m_nVideoReadSamples (0)
	, m_pAudioDec (NULL)
	, m_pLastAudioBuff (NULL)
	, m_nAudioFrameSize (0)
	, m_nAudioBuffTime (162)
	, m_pVideoDec (NULL)
	, m_pLastVideoBuff (NULL)
	, m_bVideoYUVBuffer (VO_TRUE)
	, m_bVideoYUVCopy (VO_TRUE)
	, m_pVideoYUVBuffer (NULL)
	, m_bMoreVideoBuff (VO_FALSE)
	, m_bFileDropFrame (VO_FALSE)
	, m_nNotFrameBnum (0)
	, m_bDropFrame (VO_FALSE)
	, m_bFrameDropped (VO_FALSE)
	, m_nAudioBytesPerSec (44100)
	, m_nAudioPcmDataTime (0)
	, m_nAudioSysStartTime (0)
	, m_nLastVideoBuffTime(0)
	, m_nBufferPercent (0)
	, m_bStop (VO_FALSE)
	, mllClockTime(0)
	, mbInnerClock(VO_TRUE)
	, mbUseDecThread(VO_TRUE)
	, mhDecThread(NULL)
	, mnDecThreadID(-1)
	, mnBufferCount(15)
	, mnReadIndex(0)
	, mnWriteIndex(0)
	, mbStop(VO_FALSE)
	, mbSeeking(VO_FALSE)
	, mbEOS(VO_FALSE)
	, mbDumpVideo(VO_FALSE)
	, mbDumpAudio(VO_FALSE)
	, mhVideoFile(0)
	, mhAudioFile(0)
	, mpVideoSpecData(NULL)
	, mpTempHD(NULL)
	, mppMediaBufList(NULL)
	, mpBufferList(NULL)
{
	VOLOGI("");
	cmnMemFillPointer('SFPS');

	m_ppAudioBuffer = new void *[VOSF_MEDIABUFFER_NUM];
	m_ppVideoBuffer = new void *[VOSF_MEDIABUFFER_NUM];

	for (int i = 0; i < VOSF_MEDIABUFFER_NUM; i++)
	{
		m_ppAudioBuffer[i] = NULL;
		m_ppVideoBuffer[i] = NULL;
	}

	memset (&m_sFilInfo, 0, sizeof (m_sFilInfo));
	memset (&m_sAudioTrackInfo, 0, sizeof (m_sAudioTrackInfo));
	memset (&m_sAudioFormat, 0, sizeof (m_sAudioFormat));
	memset (&m_sAudioSample, 0, sizeof (m_sAudioSample));

	memset (&m_sVideoTrackInfo, 0, sizeof (m_sVideoTrackInfo));
	memset (&m_sVideoFormat, 0, sizeof (m_sVideoFormat));
	memset (&m_sVideoSample, 0, sizeof (m_sVideoSample));

	memset (&m_sAudioDecInBuf, 0, sizeof (m_sAudioDecInBuf));
	memset (&m_sAudioDecOutBuf, 0, sizeof (m_sAudioDecOutBuf));

	memset (&m_sVideoDecInBuf, 0, sizeof (m_sVideoDecInBuf));
	memset (&m_sVideoDecOutBuf, 0, sizeof (m_sVideoDecOutBuf));

	strcpy (m_szAudioMIME, "audio/raw");
	strcpy (m_szAudioSect, "");
	strcpy (m_szVideoMIME, "video/raw");
	strcpy (m_szVideoSect, "");

	strcpy (m_szURL, "");
	if (nFlag == VOSF_DATA_URL)
	{
		strcpy (m_szURL, (char *)pDataSource);
		VOLOGI ("The URL is %s", m_szURL);
	}
	
	VO_TCHAR szPlayCfg[64];
	VO_TCHAR szCodeCfg[64];
	vostrcpy(szPlayCfg,_T("vosfPlay.cfg"));
	vostrcpy(szCodeCfg,_T("vommcodec.cfg"));

	m_cfgPlay.Open (szPlayCfg);
	m_nAudioBuffTime = m_cfgPlay.GetItemValue ("Playback", "AudioSinkBuffTime", 162);
	m_bVideoYUVBuffer = m_cfgPlay.GetItemValue ("Playback", "YUV420Buffer", 1) == 1 ? VO_TRUE : VO_FALSE;
	mbUseDecThread	= m_cfgPlay.GetItemValue("Playback" , "UseDecThread" , 1) == 1 ? VO_TRUE : VO_FALSE;
	mnBufferCount	= m_cfgPlay.GetItemValue("Playback" , "BufferCount" , 15);

	m_cfgCodec.Open(szCodeCfg);

	if(mbUseDecThread == VO_TRUE)
	{
		mpBufferList = new VO_VIDEO_BUFFER[mnBufferCount];
		memset (mpBufferList , 0 , sizeof(VO_VIDEO_BUFFER ) * mnBufferCount);

		mppMediaBufList = new void *[mnBufferCount];
		memset (mppMediaBufList , 0 , sizeof(void *) * mnBufferCount);

	}

	VOLOGI("m_bVideoYUVBuffer = %d " , m_bVideoYUVBuffer);

	if(mbDumpAudio == VO_TRUE)
	{
		mhAudioFile = fopen("/sdcard/audio.dat" , "wb");
		if(mhAudioFile == 0)
		{
			VOLOGI("Create Audio dump /sdcard/audio.dat Failed !");
		}
	}

	if(mbDumpVideo == VO_TRUE)
	{
		mhVideoFile = fopen("/sdcard/video.dat" , "wb");
		if(mhVideoFile == 0)
		{
			VOLOGI("Create video dump file Failed !");
		}
	}

	LoadSource ();
}

CFileMediaExtractor::~CFileMediaExtractor(void)
{
	VOLOGI("");
	mbStop = VO_TRUE;

	int nTimes = 0;
	while (mhDecThread != NULL && nTimes < 200)
	{
		//VOLOGI("mhDecThread : %p %d " , mhDecThread , nTimes);
		voOS_Sleep(5);
		nTimes ++;
	}
	
	if(mpTempHD != NULL)
	{
		delete mpTempHD;
		mpTempHD = NULL;
	}

	if(mpVideoSpecData != NULL)
	{
		delete mpVideoSpecData;
		mpVideoSpecData = NULL;
	}

	if (m_pVideoDec != NULL)
	{
		m_pVideoDec->Uninit ();
		delete m_pVideoDec;
	}
	if (m_pAudioDec != NULL)
	{
		m_pAudioDec->Uninit ();
		delete m_pAudioDec;
	}

	if (m_pSource != NULL)
	{
		m_pSource->CloseSource ();
		delete m_pSource;
	}

	if (m_sAudioDecOutBuf.Buffer != NULL)
		delete []m_sAudioDecOutBuf.Buffer;

	int i = 0;
	for (i = 0; i < VOSF_MEDIABUFFER_NUM; i++)
	{
		if (m_ppAudioBuffer[i] != NULL)
		{
			m_pMediaBufferOP->setObserver (m_ppAudioBuffer[i], NULL);
			m_pMediaBufferOP->release (m_ppAudioBuffer[i]);
		}
	}
	delete []m_ppAudioBuffer;

	for (i = 0; i < VOSF_MEDIABUFFER_NUM; i++)
	{
		if (m_ppVideoBuffer[i] != NULL)
		{
			m_pMediaBufferOP->setObserver (m_ppVideoBuffer[i], NULL);
			m_pMediaBufferOP->release (m_ppVideoBuffer[i]);
		}
	}
	delete []m_ppVideoBuffer;

	if (m_pVideoYUVBuffer != NULL)
		delete []m_pVideoYUVBuffer;

	ReleaseMediaBuf();
	ReleaseBufList();

	if(mpAudioHead != NULL)
	{
		delete mpAudioHead;
		mpAudioHead = NULL;
	}

	if(mhAudioFile != 0)
	{
		fclose(mhAudioFile);
		mhAudioFile = 0;
	}

	if(mhVideoFile != 0)
	{
		fclose(mhVideoFile);
		mhVideoFile = 0;
	}
}

void CFileMediaExtractor::signalBufferReturned (void * pBuffer)
{
	//VOLOGI ("Buffer is %p", pBuffer);
}

int CFileMediaExtractor::GetTrackNum (void)
{
	
	//VOLOGI ("m_nTrackCount is %d", m_nTrackCount);
	return m_nTrackCount;
}

void * CFileMediaExtractor::GetTrackSource (int nTrack)
{
	
	if (nTrack < 0 || nTrack >= m_nTrackCount)
		return NULL;

	//VOLOGI ("nTrack is %d", nTrack);

	CBaseMediaSource * pSource = NULL;

	if (nTrack == m_nAudioTrackIndex)
	{
		if ((m_cfgPlay.GetItemValue (m_szAudioSect, "voDec", 0) > 0) && m_pAudioDec == NULL)
			CreateAudioDec ();
		else
		{
			//Eric:if not using VO audio codec, we need to skip the first frame, which is head data info
			if(m_sAudioTrackInfo.Codec== VO_AUDIO_CodingAAC || m_sAudioTrackInfo.Codec== VO_AUDIO_CodingMP3)
			{
				m_pSource->GetTrackData(m_nAudioTrack , &m_sAudioSample);
			}
		}			
		m_pAudioSource = pSource = (CBaseMediaSource *) new CFileMediaSource (this, nTrack, true, m_pMediaBufferOP, m_pMetaDataOP);		
	}
	else 
	{
		if ((m_cfgPlay.GetItemValue (m_szVideoSect, "voDec", 0) > 1) && m_pVideoDec == NULL)
		{
#ifndef _SE_
			 CreateVideoDec ();
#endif 
		}

		m_pVideoSource = pSource = (CBaseMediaSource *) new CFileMediaSource (this, nTrack, false, m_pMediaBufferOP, m_pMetaDataOP);
	}

	return pSource;
}

void * CFileMediaExtractor::GetTrackMetaData (int nTrack, int nFlag)
{
	
	VOLOGI("In GetTrackMetadata nTrack : %d  nFlag : %d " , nTrack , nFlag);

	if (nTrack == m_nAudioTrackIndex)
	{
		void * pMetaData = m_pMetaDataOP->create ();
		m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , m_szAudioMIME);
		m_pMetaDataOP->setInt64(pMetaData, kKeyDuration , ((VO_U64)m_sAudioTrackInfo.Duration) * 1000);
		m_pMetaDataOP->setInt32(pMetaData, kKeySampleRate , m_sAudioFormat.SampleRate);
		
		// delete by gtxia 2012-9-20
		/*
		if(m_pAudioDec != NULL && m_sAudioFormat.Channels > 2)
		{
				m_sAudioFormat.Channels = 2;
				m_nAudioBytesPerSec = m_sAudioFormat.SampleRate * m_sAudioFormat.Channels * m_sAudioFormat.SampleBits / 8;
		}
		*/
		m_pMetaDataOP->setInt32(pMetaData, kKeyChannelCount , m_sAudioFormat.Channels);
		m_pMetaDataOP->setInt32(pMetaData, kKeyMaxInputSize, m_nAudioMaxSize);
		if (m_pAudioDec != NULL)
		{
			m_pMetaDataOP->setCString(pMetaData, kKeyDecoderComponent, "VO.AudioDec");
		}
		else if(m_sAudioTrackInfo.Codec == VO_AUDIO_CodingAAC )
		{	// added by Eric, to using stagefright default aac decoder		
			VO_SOURCE_TRACKINFO	trkInfo;
			m_pSource->GetTrackInfo(m_nAudioTrack, &trkInfo);

			const int sample_rates[] =
			{
				96000, 88200, 64000, 48000, 
				44100, 32000,24000, 22050, 
				16000, 12000, 11025, 8000,
				0, 0, 0, 0
			};

			int nRateIndex = 0;

			for( nRateIndex = 0; nRateIndex < 16 ; nRateIndex++ )
			{
				if(m_sAudioFormat.SampleRate == sample_rates[nRateIndex])
					break;
			}

			if(nRateIndex > 12)
			{
				if(trkInfo.HeadSize == 2)
				{
					nRateIndex = ((trkInfo.HeadData[0] & 0x7) << 1)  + (trkInfo.HeadData[1] >> 7);
					if(nRateIndex > 12)
						 nRateIndex = 4;
				}
				else
				{
					nRateIndex = 4;
				}
			}

			//DecoderSpecificInfo
			VO_BYTE aacInfo[2];

			aacInfo[0] = 0x10;
			aacInfo[0] = aacInfo[0] + (nRateIndex >> 1);
			aacInfo[1] = (nRateIndex << 7);
			aacInfo[1] += (m_sAudioFormat.Channels << 3);

			addESDSFromAudioSpecificInfo(pMetaData, aacInfo, 2);			
		}
		else if(m_sAudioTrackInfo.Codec == VO_AUDIO_CodingOGG)
		{
			//VOLOGI("In ogg meta info");
			VO_SOURCE_TRACKINFO	trkInfo;

			m_pSource->GetTrackInfo(m_nAudioTrack, &trkInfo);
			//VOLOGI("trkInfo headsize = %d " , trkInfo.HeadSize);
			if(trkInfo.HeadSize == 0)
			{
				if(mpAudioHead == NULL)
				{
					m_pSource->GetTrackData(m_nAudioTrack , &m_sAudioSample);
					mnAudioHeadSize = m_sAudioSample.Size& 0x7FFFFFFF;
					mpAudioHead = new VO_BYTE[mnAudioHeadSize];

					//VOLOGI("mnAudioHeadSize = %d   mpAudioHead = %p " , mnAudioHeadSize , mpAudioHead);
					memcpy(mpAudioHead , m_sAudioSample.Buffer, mnAudioHeadSize);
				}

				//VOLOGI("mnAudioHeadSize = %d " , mnAudioHeadSize);

				AddOggCodecInfo(pMetaData , mpAudioHead , mnAudioHeadSize);
			}
			else
			{
				AddOggCodecInfo(pMetaData , trkInfo.HeadData, trkInfo.HeadSize);
			}

		}
		//VOLOGI ("Audio MIME %s, %d, %d, %d", m_szAudioMIME, m_sAudioTrackInfo.Duration, m_sAudioFormat.SampleRate, m_sAudioFormat.Channels);

		return pMetaData;
	}
	else if (nTrack == m_nVideoTrackIndex)
	{
		if (nFlag != 0 && m_pVideoDec == NULL)
		{
			bool bRet = true;
			if (m_cfgPlay.GetItemValue (m_szVideoSect, "voDec", 0) > 0){
				bRet = CreateVideoDec ();
			}
			
			VOLOGI("vodec = %d " , m_cfgPlay.GetItemValue (m_szVideoSect, "voDec", 0));
			if(!bRet)	return NULL;	
		}
		//VOLOGI ("GetTrackMetaData Video nFlag is %d MIME %s ", nFlag, m_szVideoMIME);
		//VOLOGI ("GetTrackMetaData m_pVideoDec %p, m_pAudioSource is %p", m_pVideoDec, m_pAudioSource);

		void * pMetaData = m_pMetaDataOP->create ();
		m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , m_szVideoMIME);
		m_pMetaDataOP->setInt64(pMetaData, kKeyDuration , ((VO_U64)m_sVideoTrackInfo.Duration) * 1000);
		m_pMetaDataOP->setInt32(pMetaData, kKeyWidth , m_sVideoFormat.Width);
		m_pMetaDataOP->setInt32(pMetaData, kKeyHeight , m_sVideoFormat.Height);
		m_pMetaDataOP->setInt32(pMetaData, kKeyMaxInputSize, m_nVideoMaxSize);
		
		VO_U64 nThumbnailTime = 0;
		m_pMetaDataOP->setInt64(pMetaData, kKeyThumbnailTime , nThumbnailTime * 1000);

		if (m_pVideoDec != NULL)
		{
			if(mMode == MedataMode)
				m_bVideoYUVBuffer = VO_TRUE;
			
			m_pMetaDataOP->setCString(pMetaData, kKeyDecoderComponent, "VO.VideoDec");
			//m_pMetaDataOP->setInt32(kKeyColorFormat , OMX_VO_COLOR_FormatYUV420StridePlanar);
			if (m_bVideoYUVBuffer)
			{
				VOLOGI("OMX_COLOR_FormatYUV420Planar");
				m_pMetaDataOP->setInt32(pMetaData, kKeyColorFormat , OMX_COLOR_FormatYUV420Planar);
			}
			else
			{
				VOLOGI("OMX_VO_COLOR_FormatYUV420StridePlanar");
				m_pMetaDataOP->setInt32(pMetaData, kKeyColorFormat , OMX_VO_COLOR_FormatYUV420StridePlanar);
			}
		}
		else 
		{
			if(m_sVideoTrackInfo.Codec == VO_VIDEO_CodingH264)
			{
				if(m_sVideoTrackInfo.HeadSize > 0)
				{
					int nBufSize = 512;
					mpVideoSpecData = new VO_BYTE[nBufSize];
					if(mpVideoSpecData != NULL)
					{
						memset(mpVideoSpecData , 0 , nBufSize);
						H264Stream2AVCConfig((char *)m_sVideoTrackInfo.HeadData , m_sVideoTrackInfo.HeadSize , (char *)mpVideoSpecData , &nBufSize);
						
						m_pMetaDataOP->setData(pMetaData, kKeyAVCC , TYPE_POINTER, mpVideoSpecData, nBufSize);
					}
				}
			}
			else if(m_sVideoTrackInfo.Codec == VO_VIDEO_CodingMPEG4||m_sVideoTrackInfo.Codec == VO_VIDEO_CodingMPEG2)
			{
				if(m_sVideoTrackInfo.HeadSize > 0)
				{
					int nBufSize = 512;
					mpVideoSpecData = new VO_BYTE[nBufSize];
					if(mpVideoSpecData != NULL)
					{
						memset(mpVideoSpecData , 0 , nBufSize);
						ConstructESDS((unsigned char *)m_sVideoTrackInfo.HeadData , m_sVideoTrackInfo.HeadSize , (unsigned char *)mpVideoSpecData , nBufSize);
						m_pMetaDataOP->setData(pMetaData, kKeyESDS , TYPE_POINTER, mpVideoSpecData, nBufSize);
					}
				}
			}
			else if(m_sVideoTrackInfo.Codec == VO_VIDEO_CodingWMV || m_sVideoTrackInfo.Codec == VO_VIDEO_CodingVC1)
			{
				if( m_sVideoTrackInfo.HeadSize > 0 && m_cfgPlay.GetItemValue (m_szVideoSect, "voDec", 0) < 2)
					m_pMetaDataOP->setData(pMetaData, kKeyRawCodecSpecificData , TYPE_POINTER, m_sVideoTrackInfo.HeadData,  m_sVideoTrackInfo.HeadSize);
				// make QCOM VC1 use the frame by frame mode
				m_pMetaDataOP->setInt32(pMetaData, kKeyUseArbitraryMode, 0);
			}
		}
		//VOLOGI ("Video MIME %s, %d, %d, %d", m_szVideoMIME, m_sVideoTrackInfo.Duration, m_sVideoFormat.Width, m_sVideoFormat.Height);
		return pMetaData;
	}
	return NULL;
}

void * CFileMediaExtractor::GetMetaData (void)
{
	
	void * pMetaData = m_pMetaDataOP->create ();

	if (m_nVideoTrack >= 0)
	{
		if (m_nFF == VO_FILE_FFMOVIE_MP4)
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "video/mp4");
		else if (m_nFF == VO_FILE_FFMOVIE_ASF)
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "video/x-ms-wmv");
		else if (m_nFF == VO_FILE_FFMOVIE_AVI)
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "video/avi");
		else if (m_nFF == VO_FILE_FFMOVIE_MOV)
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "video/mov");
		else if (m_nFF == VO_FILE_FFMOVIE_REAL)
		{
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType ,"video/x-pn-realvideo");
			//m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "video/rmvb");
		}
		else if (m_nFF == VO_FILE_FFMOVIE_MPG)
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "video/mpeg");
		else if (m_nFF == VO_FILE_FFMOVIE_TS)
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "video/mp2ts");
		else if (m_nFF == VO_FILE_FFMOVIE_MKV)
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "video/x-matroska");
		else if (m_nFF == VO_FILE_FFAUDIO_FLAC)
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "video/flac");
		else if (m_nFF == VO_FILE_FFMOVIE_FLV)
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "video/flv");
		else
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "video/mp4");
	}
	else if (m_nAudioTrack >= 0)
	{
		if (m_nFF == VO_FILE_FFMOVIE_MP4)
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "audio/mpeg4");
		else if (m_nFF == VO_FILE_FFMOVIE_ASF)
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "audio/x-ms-wma");
		else if (m_nFF == VO_FILE_FFMOVIE_AVI)
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "audio/avi");
		else if (m_nFF == VO_FILE_FFMOVIE_MOV)
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "audio/mov");
		else if (m_nFF == VO_FILE_FFMOVIE_REAL)
		{
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType ,"audio/x-pn-realaudio");
			//m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "audio/ra");
		}
		else if (m_nFF == VO_FILE_FFMOVIE_MPG)
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "audio/mpeg");
		else if (m_nFF == VO_FILE_FFMOVIE_TS)
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "audio/ts");
		else if (m_nFF == VO_FILE_FFMOVIE_MKV)
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "audio/x-matroska");
		else if (m_nFF == VO_FILE_FFAUDIO_FLAC)
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "audio/flac");
		else if (m_nFF == VO_FILE_FFMOVIE_FLV)
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "audio/flv");
		else if (m_nFF == VO_FILE_FFAUDIO_APE)
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "audio/ape");
		else if (m_nFF == VO_FILE_FFAUDIO_ALAC)
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "audio/alac");
		else if (m_nFF == VO_FILE_FFAUDIO_QCP)
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "audio/qcp");
		else
			m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "audio/aac");
	}

	GetMetaData (pMetaData, VO_PID_METADATA_TRACK, kKeyCDTrackNumber);
	GetMetaData (pMetaData, VO_PID_METADATA_DISK, kKeyDiscNumber);
	GetMetaData (pMetaData, VO_PID_METADATA_ALBUM, kKeyAlbum);
	GetMetaData (pMetaData, VO_PID_METADATA_ARTIST, kKeyArtist);
	GetMetaData (pMetaData, VO_PID_METADATA_ARTIST, kKeyAlbumArtist);
	GetMetaData (pMetaData, VO_PID_METADATA_OWNER, kKeyAuthor);
	GetMetaData (pMetaData, VO_PID_METADATA_COMPOSER, kKeyComposer);
	GetMetaData (pMetaData, VO_PID_METADATA_YEAR, kKeyDate);
	GetMetaData (pMetaData, VO_PID_METADATA_GENRE, kKeyGenre);
	GetMetaData (pMetaData, VO_PID_METADATA_TITLE, kKeyTitle);
	GetMetaData (pMetaData, VO_PID_METADATA_YEAR, kKeyYear);
	GetMetaData (pMetaData, VO_PID_METADATA_PUBLISHER, kKeyWriter);
	getAlbumArt(pMetaData);

	return pMetaData;
}

bool CFileMediaExtractor::GetMetaData (void * pMetaData, int nMediaID, int nKey)
{
	
	char szMetaText[1024];
	memset (szMetaText, 0, sizeof (szMetaText));
	if (m_pSource->GetSourceParam (nMediaID, &szMetaText) == VO_ERR_NONE)
	{
		if (strlen (szMetaText) > 0)
		{
			//VOLOGI ("MediaID %d, nKey %d, text: %s", nMediaID, nKey, szMetaText);
			m_pMetaDataOP->setCString(pMetaData, nKey , szMetaText);
		}
	}
	return true;
}

bool CFileMediaExtractor::getAlbumArt(void* pMedaData)
{
	
	MetaDataImage metaData;
	memset(&metaData, 0, sizeof(metaData));
	m_pSource->GetSourceParam (VO_PID_METADATA_FRONTCOVER, &metaData);
	if(metaData.pBuffer != NULL)
	{
		m_pMetaDataOP->setData(pMedaData, kKeyAlbumArt, TYPE_NONE, (char*)metaData.pBuffer, metaData.dwBufferSize);
		const char* pMime;
		switch (metaData.nImageType)
		{
		case VO_METADATA_IT_JPEG:
			pMime ="image/jpeg" ;
			break;
		case VO_METADATA_IT_BMP:
			pMime = "image/bmp" ;
			break;
		case VO_METADATA_IT_PNG:
			pMime = "image/png" ;
			break;
		case VO_METADATA_IT_GIF:
			pMime = "image/gif" ;
			break;
		default:
			pMime = "text/plain";
			break;	
		}
		//VOLOGI("pMime = %s\n", pMime);
		m_pMetaDataOP->setCString(pMedaData, kKeyAlbumArtMIME, pMime);
		return true;
	}
	return false;
}

int CFileMediaExtractor::GetFlag (void)
{
	
//	VOLOGI ();
	int nFlag = 0;
	if(m_sAudioTrackInfo.Duration > 0 || m_sVideoTrackInfo.Duration > 0)
		nFlag = 15;//MediaExtractor::CAN_SEEK_BACKWARD | MediaExtractor::CAN_SEEK_FORWARD | MediaExtractor::CAN_PAUSE; | MediaExtractor::CAN_SEEK

	return nFlag;
}

int CFileMediaExtractor::GetParam (int nParamID, void * pValue1, void * pValue2)
{
	
	return CBaseMediaExtractor::GetParam (nParamID, pValue1, pValue2);;
}

int CFileMediaExtractor::SetParam (int nParamID, void * pValue1, void * pValue2)
{
	
	if (nParamID == VOSF_PID_NotifyFunc)
	{
		//VOLOGI ("VOSF_PID_NotifyFunc %p, %p", pValue1, pValue2);

		m_pNotifyFunc = (VOSFNOFITY) pValue1;
		m_pUserData = pValue2;

		return VOSF_ERR_NONE;
	}

	return CBaseMediaExtractor::SetParam (nParamID, pValue1, pValue2);;
}

int CFileMediaExtractor::Start (int nTrack, void * pParam)
{
	

	mbStop = VO_FALSE;

	if (nTrack == m_nVideoTrackIndex && m_nAudioTrackIndex >= 0)
		return 0;

	m_llLastSeekPos = 0;
	m_nStartSysTime = 0;

	if (m_pSource != NULL)
	{
		m_nBufferPercent = 0;
		m_bStop = VO_FALSE;
		m_pSource->Start ();

		if (m_nFF == VO_FILE_FFSTREAMING_RTSP)
		{
			while (m_nBufferPercent < 100)
			{
				voOS_Sleep (10);
				if (m_bStop)
					break;
			}
		}
	}

	return 0;
}

int CFileMediaExtractor::Stop (int nTrack)
{
	

	mbStop = VO_TRUE;

	int nTimes = 0;
	while (mhDecThread != NULL && nTimes < 200)
	{
		//VOLOGI("mhDecThread : %p %d " , mhDecThread , nTimes);
		voOS_Sleep(5);
		nTimes ++;
	}

	if (nTrack == m_nVideoTrackIndex && m_nAudioTrackIndex >= 0)
		return 0;

	m_bStop = VO_TRUE;

	if (m_pSource != NULL)
		m_pSource->Stop ();

	return 0;
}

bool CFileMediaExtractor::LoadSource (void)
{
	
	if (m_pFileOP == NULL)
	{
		voFileFillPointer ();
		m_pFileOP = (VO_FILE_OPERATOR *)&g_vosfFileOP;
	}

	char *			pURL = NULL;

	VO_FILE_SOURCE filSource;
	memset (&filSource, 0, sizeof (VO_FILE_SOURCE));
	filSource.nMode = VO_FILE_READ_ONLY;

	if (strlen (m_szURL) > 0)
	{
		filSource.nFlag = VO_FILE_TYPE_NAME;
		pURL = strstr (m_szURL, "http://");
		if (pURL == NULL)
			pURL = strstr ((char *)m_szURL, "rtsp://");
		filSource.pSource = m_szURL;

		VO_TCHAR szURL[1024];
		vostrcpy (szURL, (VO_PTCHAR)m_szURL);

		if (!vostrncmp (szURL, _T("RTSP://"), 6) || !vostrncmp (szURL, _T("rtsp://"), 6) || 
			!vostrncmp (szURL, _T("MMS://"), 5) || !vostrncmp (szURL, _T("mms://"), 5))
			m_nFF = VO_FILE_FFSTREAMING_RTSP;
		else if (!vostrncmp (szURL, _T("HTTP://"), 6) || !vostrncmp (szURL, _T("http://"), 6))
		{
			VO_TCHAR* p = vostrstr(szURL, _T(".sdp"));
			if(!p)
				p = vostrstr(szURL, _T(".SDP"));

			if(p && ((size_t)(p + 4 - szURL) == vostrlen(szURL) || p[4] == _T('?')))
				m_nFF = VO_FILE_FFSTREAMING_RTSP;
			else
			{
				if( vostrstr(szURL , _T(".m3u")) || vostrstr(szURL , _T(".M3U")) )
				{
					VOLOGE( "It is HTTP Live Streaming!" );
					m_nFF = VO_FILE_FFSTREAMING_HLS;
				}
				else if( vostrstr(szURL , _T(".manifest")) || vostrstr( szURL , _T(".MANIFEST") ) )
				{
					VOLOGE( "It is Silverlight Smooth Streaming!" );
					m_nFF = VO_FILE_FFSTREAMING_SSS;
				}
				else
				{
					VOLOGE( "It is HTTP Streaming!" );
					m_nFF = VO_FILE_FFSTREAMING_HTTPPD;
				}
			}
		}
	}
	else
	{
		filSource.nFlag = VO_FILE_TYPE_HANDLE;
		filSource.pSource = m_pDataSource;
	}

	if(pURL == NULL)
	{
		CFileFormatCheck filCheck (m_pFileOP , &g_memOP);
		m_nFF = filCheck.GetFileFormat(&filSource , ~FLAG_CHECK_AUDIOREADER);
	}
//	VOLOGI ("File Format is %d", m_nFF);

	if (m_nFF == VO_FILE_FFSTREAMING_RTSP || m_nFF == VO_FILE_FFAPPLICATION_SDP)
		m_pSource = new CRTSPSource (NULL, m_nFF, &g_memOP, m_pFileOP, NULL);
	else if (m_nFF == VO_FILE_FFSTREAMING_HTTPPD)
		m_pSource = new CHTTPPDSource (NULL, m_nFF, &g_memOP, m_pFileOP, NULL);
	else if (m_nFF == VO_FILE_FFSTREAMING_HLS || m_nFF == VO_FILE_FFSTREAMING_SSS)
		m_pSource = new CLiveSource(NULL, m_nFF, &g_memOP, m_pFileOP, NULL);
	else
		m_pSource = new CFileSource (NULL, m_nFF, &g_memOP, m_pFileOP, NULL);
//	m_pSource = new CFileSource(NULL, m_nFF, &g_memOP , m_pFileOP , NULL);
	if(m_pSource == NULL)
	{
		VOLOGE ("It could not create CFileSource");
		return false;
	}

	// added by gtxia 
	m_pSource->SetConfig(&m_cfgCodec);

	m_pSource->SetCallBack ((VO_PTR)vosfFileSourceStreamCallBack, (VO_PTR)this);

	VO_U32 nRC = m_pSource->LoadSource(filSource.pSource, VO_FILE_TYPE_HANDLE , 0 , 0);
	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("The file could not load!");
		return false;
	}

	m_pSource->GetSourceInfo (&m_sFilInfo);
	if (m_sFilInfo.Tracks == 0)
	{
		VOLOGE ("The track number is 0");
		return false;
	}
	m_nTrackCount = 0;

	VOLOGI("The track number is %d " , m_sFilInfo.Tracks);

	VO_SOURCE_TRACKINFO	trkInfo;
	VO_U32				i = 0;
	for (i = 0; i < m_sFilInfo.Tracks; i++)
	{
		m_pSource->GetTrackInfo (i, &trkInfo);

		if ((trkInfo.Type == VO_SOURCE_TT_AUDIO || trkInfo.Type == VO_SOURCE_TT_RTSP_AUDIO))
		{
			
			if(m_nAudioTrack!= -1)
				continue;

			m_nAudioTrack = i;
			m_nTrackCount++;
			m_nAudioTrackIndex = m_nTrackCount-1;
			m_pSource->GetTrackInfo (m_nAudioTrack, &m_sAudioTrackInfo);
			m_pSource->GetTrackParam  (m_nAudioTrack, VO_PID_SOURCE_MAXSAMPLESIZE, &m_nAudioMaxSize);
			m_pSource->GetAudioFormat (m_nAudioTrack, &m_sAudioFormat);
			
			//if (m_sAudioFormat.Channels > 2)
			//	m_sAudioFormat.Channels = 2;
			m_nAudioBytesPerSec = m_sAudioFormat.SampleRate * m_sAudioFormat.Channels * m_sAudioFormat.SampleBits / 8;
		
			VOLOGI("m_sAudioTrackInfo.Codec = %d " , m_sAudioTrackInfo.Codec);
			if (m_sAudioTrackInfo.Codec == VO_AUDIO_CodingMP3 || m_sAudioTrackInfo.Codec == VO_AUDIO_CodingMP1)
			{
				strcpy (m_szAudioSect, "Audio_Dec_MP3");
				strcpy (m_szAudioMIME , "audio/mpeg");
			}
			else if (m_sAudioTrackInfo.Codec== VO_AUDIO_CodingAAC)
			{
				strcpy (m_szAudioSect, "Audio_Dec_AAC");
				strcpy (m_szAudioMIME , "audio/mp4a-latm");
			}
			else if (m_sAudioTrackInfo.Codec== VO_AUDIO_CodingAMRNB)
			{
				strcpy (m_szAudioSect, "Audio_Dec_AMRNB");
				strcpy (m_szAudioMIME , "audio/3gpp");
				m_sAudioFormat.Channels = 1;
				m_sAudioFormat.SampleRate = 8000;
			}
			else if (m_sAudioTrackInfo.Codec== VO_AUDIO_CodingAMRWB)
			{
				strcpy (m_szAudioSect, "Audio_Dec_AMRWB");
				strcpy (m_szAudioMIME , "audio/amr-wb");
			}
			else if (m_sAudioTrackInfo.Codec== VO_AUDIO_CodingAMRWBP)
			{
				strcpy (m_szAudioSect, "Audio_Dec_AMRWBP");
				strcpy (m_szAudioMIME , "audio/amr-wbp");
			}
			else if (m_sAudioTrackInfo.Codec== VO_AUDIO_CodingWMA)
			{
				strcpy (m_szAudioSect, "Audio_Dec_WMA");
				strcpy (m_szAudioMIME , "audio/wma");
			}
			else if (m_sAudioTrackInfo.Codec== VO_AUDIO_CodingADPCM)
			{
				strcpy (m_szAudioSect, "Audio_Dec_ADPCM");
				strcpy (m_szAudioMIME , "audio/adpcm");
			}
			else if (m_sAudioTrackInfo.Codec== VO_AUDIO_CodingQCELP13)
			{
				strcpy (m_szAudioSect, "Audio_Dec_QCELP");
				strcpy (m_szAudioMIME , "audio/qcelp");
				m_sAudioFormat.Channels = 1;
				m_sAudioFormat.SampleRate = 8000;
			}
			else if (m_sAudioTrackInfo.Codec== VO_AUDIO_CodingEVRC)
			{
				strcpy (m_szAudioSect, "Audio_Dec_EVRC");
				strcpy (m_szAudioMIME , "audio/evrc");
				m_sAudioFormat.Channels = 1;
				m_sAudioFormat.SampleRate = 8000;
			}
			else if (m_sAudioTrackInfo.Codec== VO_AUDIO_CodingAC3)
			{
				strcpy (m_szAudioSect, "Audio_Dec_AC3");
				strcpy (m_szAudioMIME , "audio/ac3");
			}
			else if (m_sAudioTrackInfo.Codec== VO_AUDIO_CodingEAC3)
			{
				strcpy (m_szAudioSect, "Audio_Dec_EAC3");
				strcpy (m_szAudioMIME , "audio/eac3");
			}
			else if (m_sAudioTrackInfo.Codec== VO_AUDIO_CodingFLAC)
			{
				strcpy (m_szAudioSect, "Audio_Dec_FLAC");
				strcpy (m_szAudioMIME , "audio/flac");
			}
			else if (m_sAudioTrackInfo.Codec== VO_AUDIO_CodingOGG)
			{
				strcpy (m_szAudioSect, "Audio_Dec_OGG");
				strcpy (m_szAudioMIME , "audio/vorbis");
			}
			else if (m_sAudioTrackInfo.Codec == VO_AUDIO_CodingRA)
			{
				strcpy (m_szAudioSect, "Audio_Dec_RA");
				strcpy (m_szAudioMIME , "audio/x-pn-realaudio");
				//strcpy (m_szAudioMIME , "audio/real");
			}
			else if( m_sAudioTrackInfo.Codec == VO_AUDIO_CodingAPE )
			{
				strcpy (m_szAudioSect, "Audio_Dec_MAC ");
				strcpy (m_szAudioMIME , "audio/ape");
			}
			else if( m_sAudioTrackInfo.Codec == VO_AUDIO_CodingALAC )
			{
				strcpy (m_szAudioSect, "Audio_Dec_ALAC ");
				strcpy (m_szAudioMIME , "audio/alac");
			}
			else if( m_sAudioTrackInfo.Codec == VO_AUDIO_CodingALAC )
			{
				strcpy (m_szAudioSect, "Audio_Dec_DTS ");
				strcpy (m_szAudioMIME , "audio/dts");
			}

			char * pAudioMIME = m_cfgPlay.GetItemText (m_szAudioSect, "MIME");
			if (pAudioMIME != NULL)
				strcpy (m_szAudioMIME , pAudioMIME);

			VOLOGI("m_szAudioSect %s   m_szAudioMIME %s " , m_szAudioSect , m_szAudioMIME);
		}
		else if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO))
		{
			if(m_nVideoTrack != -1)
				continue;

			m_nVideoTrack = i;
			m_nTrackCount++;
			m_nVideoTrackIndex = m_nTrackCount-1;
			m_pSource->GetTrackInfo (m_nVideoTrack, &m_sVideoTrackInfo);
			m_pSource->GetTrackParam  (m_nVideoTrack, VO_PID_SOURCE_MAXSAMPLESIZE, &m_nVideoMaxSize);
			m_pSource->GetVideoFormat (m_nVideoTrack, &m_sVideoFormat);
			
			//VOLOGI("m_sVideoTrackInfo.Codec = %d " , m_sVideoTrackInfo.Codec);
			strcpy (m_szVideoSect, "Video_Dec_Unknown");
			if (m_sVideoTrackInfo.Codec == VO_VIDEO_CodingMPEG4)
			{
				strcpy (m_szVideoSect, "Video_Dec_MPEG4");
				strcpy (m_szVideoMIME , "video/mp4v-es");
			}
			else if (m_sVideoTrackInfo.Codec == VO_VIDEO_CodingH263)
			{
				strcpy (m_szVideoSect, "Video_Dec_H263");
				strcpy (m_szVideoMIME , "video/3gpp");
			}
			else if (m_sVideoTrackInfo.Codec == VO_VIDEO_CodingS263)
			{
				strcpy (m_szVideoSect, "Video_Dec_S263");
				strcpy (m_szVideoMIME , "video/s263");
			}
			else if (m_sVideoTrackInfo.Codec == VO_VIDEO_CodingH264)
			{
				strcpy (m_szVideoSect, "Video_Dec_H264");
				strcpy (m_szVideoMIME, "video/avc");
			}
			else if (m_sVideoTrackInfo.Codec == VO_VIDEO_CodingWMV)
			{
				strcpy (m_szVideoSect, "Video_Dec_WMV");
				//strcpy (m_szVideoMIME , "video/wmv");
				strcpy (m_szVideoMIME , "video/x-ms-wmv");
			}
			else if (m_sVideoTrackInfo.Codec == VO_VIDEO_CodingVC1)
			{
				strcpy (m_szVideoSect, "Video_Dec_VC1");
				//strcpy (m_szVideoMIME , "video/vc1");
				strcpy (m_szVideoMIME , "video/x-ms-wmv");
			}
			else if (m_sVideoTrackInfo.Codec == VO_VIDEO_CodingRV)
			{
				strcpy (m_szVideoSect, "Video_Dec_REAL");
				//strcpy (m_szVideoMIME , "video/rmvb");
				strcpy (m_szVideoMIME , "video/x-pn-realvideo");
			}
			else if (m_sVideoTrackInfo.Codec == VO_VIDEO_CodingMPEG2)
			{
				strcpy (m_szVideoSect, "Video_Dec_MPEG2");
				strcpy (m_szVideoMIME , "video/mpeg2");
			}
			else if (m_sVideoTrackInfo.Codec == VO_VIDEO_CodingMJPEG)
			{
				strcpy (m_szVideoSect, "Video_Dec_MJPEG");
				strcpy (m_szVideoMIME , "video/mjpeg");
			}
			else if (m_sVideoTrackInfo.Codec == VO_VIDEO_CodingDIVX)
			{
				strcpy (m_szVideoSect, "Video_Dec_DIVX");
				strcpy (m_szVideoMIME , "video/divx3");
			}
			else if (m_sVideoTrackInfo.Codec == VO_VIDEO_CodingVP6)
			{
				strcpy (m_szVideoSect, "Video_Dec_VP6");
				strcpy (m_szVideoMIME , "video/vp6");
			}
			else if(m_sVideoTrackInfo.Codec == VO_VIDEO_CodingVP8)
			{
				strcpy (m_szVideoSect, "Video_Dec_VP8");
				strcpy (m_szVideoMIME , "video/x-vnd.on2.vp8");
			}


			char * pVideoMIME = m_cfgPlay.GetItemText (m_szVideoSect, "MIME");
			if (pVideoMIME != NULL)
				strcpy (m_szVideoMIME , pVideoMIME);

			VOLOGI("m_szVideoSect %s   m_szVideoMIME %s " , m_szVideoSect , m_szVideoMIME);
		}
	}

	m_nStartSysTime = 0;
	m_nAudioReadSamples = 0;
	m_nVideoReadSamples = 0;

	return true;
}

bool CFileMediaExtractor::CreateAudioDec (void)
{
	
	//VOLOGI("Create Audio Decoder !!!");
	voCAutoLock lock (&m_mtSource);
	if (m_pSource == NULL || m_nAudioTrack < 0)
		return false;

	if (m_pAudioDec != NULL)
		delete m_pAudioDec;

	bool bRet = false;
	VO_SOURCE_TRACKINFO	trkInfo;
	VO_S32	nRC = 0;
	VO_U32	nFourCC = 0;

	m_pSource->GetTrackInfo (m_nAudioTrack, &trkInfo);	
	m_pAudioDec = new CAudioDecoder (NULL , trkInfo.Codec, &g_memOP);
	if (m_pAudioDec == NULL)
	{
		VOLOGE ("It could not create audio dec class!");
		goto _EXIT_;
	}

	nRC = m_pSource->GetTrackParam (m_nAudioTrack, VO_PID_SOURCE_CODECCC, &nFourCC);
	m_pAudioDec->SetFourCC (nFourCC);

	// the head data for WMA is different
	if (trkInfo.Codec == VO_AUDIO_CodingWMA || trkInfo.Codec == VO_AUDIO_CodingADPCM)
	{	
		nRC = m_pSource->GetTrackParam (m_nAudioTrack, VO_PID_SOURCE_WAVEFORMATEX, &trkInfo.HeadData);
		if (nRC != VO_ERR_NONE)
		{
			VOLOGE ("It could not get VO_PID_SOURCE_WAVEFORMATEX!");
			goto _EXIT_;
		}

		VO_WAVEFORMATEX * pWaveFormat = (VO_WAVEFORMATEX *)trkInfo.HeadData;
		trkInfo.HeadSize = VO_WAVEFORMATEX_STRUCTLEN + pWaveFormat->cbSize;
	}
	

	if (trkInfo.HeadData != NULL && trkInfo.HeadSize > 0)
	{
		nRC = m_pAudioDec->Init (trkInfo.HeadData, trkInfo.HeadSize, &m_sAudioFormat);
		if(nRC != VOSF_ERR_NONE)
			goto _EXIT_;
	}
	else
	{
		VO_SOURCE_SAMPLE	sample;
		nRC = m_pSource->GetTrackParam (m_nAudioTrack, VO_PID_SOURCE_FIRSTFRAME, &sample);
		if (nRC != VO_ERR_NONE)
		{
			sample.Buffer = NULL;
			sample.Size = 0;
		}
		else
		{
			sample.Size = sample.Size & 0X7FFFFFFF;
		}
		nRC = m_pAudioDec->Init (sample.Buffer, sample.Size, &m_sAudioFormat);
	}

	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("Init audio dec error. %08X", nRC);
		goto _EXIT_;
	}		
	
	bRet = true;
	strcpy (m_szAudioMIME , "audio/raw");
	if(m_sAudioFormat.SampleRate == 0)
		m_sAudioFormat.SampleRate = DEFAULT_AUDIO_SAMPLE_RATE;
	if(m_sAudioFormat.Channels == 0)
		m_sAudioFormat.Channels = 2;
	m_nAudioFrameSize = m_sAudioFormat.SampleRate * m_sAudioFormat.Channels * 2;
	if (m_sAudioDecOutBuf.Buffer == NULL)
		m_sAudioDecOutBuf.Buffer = new VO_BYTE[m_nAudioFrameSize];

	/*ReadAudioSample ();
	long long llSeekPos = 0;
	nRC = m_pSource->SetTrackPos(m_nAudioTrack , &llSeekPos);
	m_pAudioDec->Flush();
	m_pLastAudioBuff = NULL;
	m_sAudioSample.Time = 0;*/

_EXIT_:
	if(!bRet && m_pAudioDec)
	{
		m_pAudioDec->Uninit();
		delete m_pAudioDec;
		m_pAudioDec = NULL;
	}
	return bRet;
}

bool CFileMediaExtractor::CreateVideoDec (void)
{
	
	//VOLOGI("Create Video Decoder !!!");
	voCAutoLock lock (&m_mtSource);
	if (m_pSource == NULL || m_nVideoTrack < 0)
		return false;

	if (m_pVideoDec != NULL)
		delete m_pVideoDec;

	bool bRet = false;
	VO_SOURCE_TRACKINFO	trkInfo;
	VO_S32	nRC = 0;
	VO_U32	nFourCC = 0;
	//int nCPUNums = 1;
	m_pSource->GetTrackInfo (m_nVideoTrack, &trkInfo);

	//VOLOGI("video codec = %d " , trkInfo.Codec);
	m_pVideoDec = new CVideoDecoder (NULL, trkInfo.Codec, &g_memOP);
	if (m_pVideoDec == NULL)
	{
		VOLOGE ("It could not create CVideoDecoder");
		goto _EXIT_;
	}

	// added by gtxia, It should only be used for debugging 
    //nCPUNums = m_cfgPlay.GetItemValue ("Playback", "CPU_Num", 1);	
	//m_pVideoDec->SetCPUNumber(nCPUNums);
    /////////////////////////////////////////////////////////////////////////////////

	nRC = m_pSource->GetTrackParam (m_nVideoTrack, VO_PID_SOURCE_CODECCC, &nFourCC);
	m_pVideoDec->SetFourCC (nFourCC);
	VOLOGI("Video codec FourCC = %x" , nFourCC);

	// the head data for WMV is different
	if (trkInfo.Codec == VO_VIDEO_CodingWMV || trkInfo.Codec == VO_VIDEO_CodingVC1)
	{
		nRC = m_pSource->GetTrackParam (m_nVideoTrack, VO_PID_SOURCE_BITMAPINFOHEADER, &trkInfo.HeadData);
		if (nRC != VO_ERR_NONE)
		{
			VOLOGE ("WMV VO_PID_SOURCE_BITMAPINFOHEADER error %08X", nRC);
			goto _EXIT_;
		}

		VO_BITMAPINFOHEADER * pBmpInfo = (VO_BITMAPINFOHEADER *)trkInfo.HeadData;
		trkInfo.HeadSize = pBmpInfo->biSize;
	}

	if (trkInfo.HeadData != NULL && trkInfo.HeadSize > 0)
	{
		nRC = m_pVideoDec->Init (trkInfo.HeadData, trkInfo.HeadSize, &m_sVideoFormat);
		if(nRC!= VO_ERR_NONE)
		{
			goto _EXIT_;
		}
	}
	else
	{
		VO_SOURCE_SAMPLE	sample;
		sample.Buffer = NULL;
		sample.Size = 0;

		nRC = m_pSource->GetTrackParam (m_nVideoTrack, VO_PID_SOURCE_FIRSTFRAME, &sample);

		if (nRC != VO_ERR_NONE)
		{
			sample.Buffer = NULL;
			sample.Size = 0;
		}
		else
		{
			sample.Size = sample.Size & 0X7FFFFFFF;
		}

		nRC = m_pVideoDec->Init (sample.Buffer, sample.Size, &m_sVideoFormat);
		if (nRC != VO_ERR_NONE)
		{
			VOLOGE ("m_pVideoDec->Init error %08X", nRC);
			goto _EXIT_;
		}
	}	
	
	bRet = true;
	strcpy (m_szVideoMIME , "video/raw");
/*
	ReadVideoSample ();
	long long llSeekPos = 0;
	VOLOGI ("llSeekPos  IS %d", (int)llSeekPos);
	nRC = m_pSource->SetTrackPos(m_nVideoTrack , &llSeekPos);
	VOLOGI ("llSeekPos  IS %d", (int)llSeekPos);
	m_pVideoDec->Flush();
	m_pLastVideoBuff = NULL;
	m_sVideoSample.Time = 0;

	if (m_bVideoYUVBuffer)
	{
		if (m_sVideoDecOutBuf.Buffer[0] < m_sVideoDecOutBuf.Buffer[1])
		{
			if (m_sVideoDecOutBuf.Buffer[1] - m_sVideoDecOutBuf.Buffer[0] == m_sVideoFormat.Width * m_sVideoFormat.Height &&
				m_sVideoDecOutBuf.Buffer[2] - m_sVideoDecOutBuf.Buffer[0] == m_sVideoFormat.Width * m_sVideoFormat.Height * 5 / 4)
			{
				m_bVideoYUVCopy = VO_FALSE;
			}
		}
	}
*/
_EXIT_:
	if(!bRet&& m_pVideoDec)
	{
		m_pVideoDec->Uninit();
		delete m_pVideoDec;
		m_pVideoDec = NULL;
	}
	return bRet;
}

int CFileMediaExtractor::ReadSample (void ** ppBuffer, void * pOptions, int nTrack)
{
	
	if(m_pSource == NULL)
	{
		VOLOGE ("m_pSource == NULL");
		return -1;
	}

	//VO_U32	uStart = voOS_GetSysTime();

	if(m_bStop)
		return VOSF_ERR_EOS;
	
	long long llSeekPos = -1;
	if(pOptions != NULL)
	{
		voCAutoLock lock (&m_mtRead);

		int nMode = 0;
		VOSF_READOPTIONTYPE * pReadOption = (VOSF_READOPTIONTYPE *)pOptions;
#ifdef _GINGERBREAD
		if(!pReadOption->getSeekTo(pReadOption->pOptions, &llSeekPos , &nMode))
#else  //_GINGERBREAD
		if(!pReadOption->getSeekTo(pReadOption->pOptions, &llSeekPos))
#endif //_GINGERBREAD
		{	
			llSeekPos = -1;
		}
		else
		{
			if(nMode == 4)
			{
				mllClockTime =  llSeekPos / 1000;

				mbInnerClock = VO_FALSE;

				//VOLOGI("mllClocktime : %lld " , mllClockTime);
				if(mllClockTime < 0)
					mllClockTime = 0;

				llSeekPos = -1;
			}
			else
			{
				llSeekPos = llSeekPos / 1000;

				if(m_nVideoTrackIndex == nTrack)
				{
					mnReadIndex = mnWriteIndex = 0;
					mbSeeking = VO_TRUE;
					mbEOS = VO_FALSE;
				}
			}


		}

		if(llSeekPos != -1)
		{
			if (m_nFF == VO_FILE_FFSTREAMING_RTSP || m_nFF == VO_FILE_FFSTREAMING_HTTPPD)
			{
				if (m_llLastSeekPos < llSeekPos + 500 && m_llLastSeekPos > llSeekPos - 500)
				{
					llSeekPos = -1;
				}
			}
		}
	}

	VO_S32 nRC = VO_ERR_NONE;
	if(llSeekPos != -1)
	{
		voCAutoLock lock(&m_mtDecoder);
		if (m_nVideoTrackIndex == nTrack)
		{
			
			m_llLastSeekPos = llSeekPos;
			mllClockTime = llSeekPos;
			nRC = m_pSource->SetTrackPos(m_nVideoTrack , &llSeekPos);
			if(nRC == VO_ERR_NONE)
			{
				if (m_pVideoDec != NULL)
					m_pVideoDec->Flush();
				m_sVideoSample.Time = llSeekPos;
			}

			if (m_nAudioTrack >= 0 && m_nFF != VO_FILE_FFSTREAMING_RTSP)
			{
				m_llLastSeekPos = llSeekPos;
				nRC = m_pSource->SetTrackPos(m_nAudioTrack , &llSeekPos);
				if(nRC == VO_ERR_NONE)
				{
					if (m_pAudioDec != NULL)
						m_pAudioDec->Flush();
					m_sAudioSample.Time = llSeekPos;
				}
			}
		}

		if (m_nAudioTrackIndex == nTrack && m_nVideoTrack < 0)
		{
			m_llLastSeekPos = llSeekPos;
			nRC = m_pSource->SetTrackPos(m_nAudioTrack , &llSeekPos);
			if(nRC == VO_ERR_NONE)
			{
				if (m_pAudioDec != NULL)
					m_pAudioDec->Flush();
				m_sAudioSample.Time = llSeekPos;
			}
		}

		m_nAudioPcmDataTime = 0;
		m_nAudioSysStartTime = 0;
	}

	void * pMediaBuffer = NULL;
	if (nTrack == m_nAudioTrackIndex)
	{
		nRC = ReadAudioSample ();
		if (nRC == VOSF_ERR_NONE)
		{
			if (m_pAudioDec == NULL)
			{
				pMediaBuffer = FillMediaBuffer (m_sAudioSample.Buffer, m_nAudioMaxSize, true);
				if (pMediaBuffer == NULL)
				{
					VOLOGE ("Read Audio Sample. pMediaBuffer == NULL");
					return VOSF_ERR_OUTOF_MEMORY;
				}
				m_pMediaBufferOP->set_range (pMediaBuffer, 0, (m_sAudioSample.Size & 0X7FFFFFFF));
				m_pMetaDataOP->clear (m_pMediaBufferOP->meta_data (pMediaBuffer));
				m_pMetaDataOP->setInt64 (m_pMediaBufferOP->meta_data (pMediaBuffer), kKeyTime, m_sAudioSample.Time * 1000);

				//VOLOGI("Audio info :timestamp: %08lld  systemtime: %08d  size : %d " , m_sAudioSample.Time , voOS_GetSysTime() , (m_sAudioSample.Size & 0X7FFFFFFF));
			}
			else
			{
				pMediaBuffer = FillMediaBuffer (m_sAudioDecOutBuf.Buffer, m_nAudioFrameSize, true);
				if (pMediaBuffer == NULL)
				{
					VOLOGE ("Read Audio Sample. pMediaBuffer == NULL");
					return VOSF_ERR_OUTOF_MEMORY;
				}
				m_pMediaBufferOP->set_range (pMediaBuffer, 0, (m_sAudioDecOutBuf.Length));
				m_pMetaDataOP->clear (m_pMediaBufferOP->meta_data (pMediaBuffer));

				m_pMetaDataOP->setInt64 (m_pMediaBufferOP->meta_data (pMediaBuffer), kKeyTime, m_sAudioDecOutBuf.Time * 1000);

				voCAutoLock lock (&m_mtRead);
				m_nAudioPcmDataTime = m_nAudioPcmDataTime + m_sAudioDecOutBuf.Length * 1000 / m_nAudioBytesPerSec;
				if (m_nAudioSysStartTime == 0)
					m_nAudioSysStartTime = voOS_GetSysTime ();
			}

			m_pMediaBufferOP->add_ref(pMediaBuffer);
			*ppBuffer = pMediaBuffer;

			m_nAudioReadSamples++;
		}

		return nRC;
	}
	else if (nTrack == m_nVideoTrackIndex)
	{
		if (voOS_GetSysTime () - m_nAudioSysStartTime - m_nAudioPcmDataTime > 100)
		{
			voCAutoLock lock (&m_mtRead);
			m_nAudioSysStartTime = m_nAudioSysStartTime + 2;
		}

		if(mhDecThread == NULL && mbUseDecThread == VO_TRUE && mbStop == VO_FALSE && m_pVideoDec != NULL)
		{
			int nFlag = 1;
#if defined(_LINUX_ANDROID)
			// added by gtxia for fix the C10 issue
			char cName[512];
			property_get("ro.build.product", cName, "unknown");
			if (strncasecmp(cName, "C10", 3) == 0)
				nFlag = 0;
#endif	
			voThreadCreate(&mhDecThread , &mnDecThreadID , (voThreadProc )voDecodeProc , this , nFlag);
		}


		if(mbUseDecThread == VO_TRUE && mbSeeking == VO_FALSE && m_pVideoDec != NULL )
		{
			while(mnWriteIndex <= mnReadIndex && mbStop == VO_FALSE && mbEOS == VO_FALSE)
			{
				voOS_Sleep(2);
			}

			//VOLOGI(" mnReadIndex : %04d  mnWriteIndex : %04d  diff : %d " , mnReadIndex , mnWriteIndex , mnWriteIndex - mnReadIndex);

			if(mbEOS == VO_TRUE && mnWriteIndex - mnReadIndex <= 1)
				return VOSF_ERR_EOS;

			if(mbStop == VO_TRUE)
				return VOSF_ERR_WRONG_STATUS;


			int nIndex = mnReadIndex % mnBufferCount;
			VO_VIDEO_BUFFER *pVideoBuffer = &(mpBufferList[nIndex]);


			void *pMediaBuf = NULL;
			if(m_bVideoYUVBuffer)
				pMediaBuf = FindMediaBuffer(pVideoBuffer->Buffer[0]);
			else 
				pMediaBuf = FindMediaBuffer(pVideoBuffer);

			if(pMediaBuf == NULL)
			{
				if (m_bVideoYUVBuffer)
				{
					pMediaBuf = m_pMediaBufferOP->create (pVideoBuffer->Buffer[0],  m_sVideoFormat.Width * m_sVideoFormat.Height * 3 / 2);
				}
				else
				{
					pMediaBuf = m_pMediaBufferOP->create (pVideoBuffer , sizeof (VO_VIDEO_BUFFER));
				}

				if (pMediaBuf == NULL)
					return VOSF_ERR_OUTOF_MEMORY;
				m_pMediaBufferOP->setObserver(pMediaBuf, this);

				AddMediaBuffer(pMediaBuf);
			}

			if (m_bVideoYUVBuffer)
				m_pMediaBufferOP->set_range (pMediaBuf, 0, m_sVideoFormat.Width * m_sVideoFormat.Height * 3 / 2);
			else
				m_pMediaBufferOP->set_range (pMediaBuf, 0, sizeof (m_sVideoDecOutBuf));	

			m_pMetaDataOP->clear (m_pMediaBufferOP->meta_data (pMediaBuf));
			m_pMetaDataOP->setInt64 (m_pMediaBufferOP->meta_data (pMediaBuf), kKeyTime,pVideoBuffer->Time * 1000);

			m_pMediaBufferOP->add_ref (pMediaBuf);
			*ppBuffer = pMediaBuf;
			mnReadIndex += 1;
			
			return VOSF_ERR_NONE;
		}
		else
		{
			{
				 voCAutoLock lock(&m_mtDecoder);
				 nRC = ReadVideoSample ();
			}
			
			if (nRC == VOSF_ERR_NONE)
			{
				if (m_pVideoDec == NULL)
				{
					pMediaBuffer = FillMediaBuffer (m_sVideoSample.Buffer, m_nVideoMaxSize, false);
					if (pMediaBuffer == NULL)
					{
						VOLOGE ("Read Video Sample. pMediaBuffer == NULL");
						return VOSF_ERR_OUTOF_MEMORY;
					}
					m_pMediaBufferOP->set_range (pMediaBuffer, 0, (m_sVideoSample.Size & 0X7FFFFFFF));
					m_pMetaDataOP->clear (m_pMediaBufferOP->meta_data (pMediaBuffer));
					m_pMetaDataOP->setInt64 (m_pMediaBufferOP->meta_data (pMediaBuffer), kKeyTime, m_sVideoSample.Time * 1000);

					//long long llTime = 0 ;
					//m_pMetaDataOP->findInt64 (m_pMediaBufferOP->meta_data (pMediaBuffer), kKeyTime, &llTime);

					//VOLOGI("Video info :timestamp: %08lld  systemtime : %08d  size : %05d  read used time : %d" , m_sVideoSample.Time , voOS_GetSysTime(),  (m_sVideoSample.Size & 0X7FFFFFFF) , voOS_GetSysTime() - uStart);
				}
				else
				{
					if (m_nVideoReadSamples == 0 && m_bVideoYUVBuffer)
					{
						if (m_sVideoDecOutBuf.Buffer[0] < m_sVideoDecOutBuf.Buffer[1])
						{
							if (m_sVideoDecOutBuf.Buffer[1] - m_sVideoDecOutBuf.Buffer[0] == m_sVideoFormat.Width * m_sVideoFormat.Height &&
								m_sVideoDecOutBuf.Buffer[2] - m_sVideoDecOutBuf.Buffer[0] == m_sVideoFormat.Width * m_sVideoFormat.Height * 5 / 4)
							{
								m_bVideoYUVCopy = VO_FALSE;
							}
						}
					}

					if (m_bVideoYUVBuffer && m_bVideoYUVCopy)
					{
						int i = 0;
						if (m_pVideoYUVBuffer == NULL)
							m_pVideoYUVBuffer = new VO_BYTE[ m_sVideoFormat.Width * m_sVideoFormat.Height * 3 / 2];
						VO_PBYTE pYUV = m_pVideoYUVBuffer;
						for (i = 0; i < m_sVideoFormat.Height; i++)
							memcpy (pYUV + m_sVideoFormat.Width * i, m_sVideoDecOutBuf.Buffer[0] + m_sVideoDecOutBuf.Stride[0] * i, m_sVideoFormat.Width);
						pYUV = m_pVideoYUVBuffer + m_sVideoFormat.Width * m_sVideoFormat.Height;
						for (i = 0; i < m_sVideoFormat.Height / 2; i++)
							memcpy (pYUV + m_sVideoFormat.Width / 2 * i, m_sVideoDecOutBuf.Buffer[1] + m_sVideoDecOutBuf.Stride[1] * i, m_sVideoFormat.Width / 2);
						pYUV = m_pVideoYUVBuffer + m_sVideoFormat.Width * m_sVideoFormat.Height * 5 / 4;
						for (i = 0; i < m_sVideoFormat.Height / 2; i++)
							memcpy (pYUV + m_sVideoFormat.Width / 2 * i, m_sVideoDecOutBuf.Buffer[2] + m_sVideoDecOutBuf.Stride[2] * i, m_sVideoFormat.Width / 2);
					}

					if (m_bVideoYUVBuffer)
					{
						if (m_bVideoYUVCopy)
							pMediaBuffer = FillMediaBuffer (m_pVideoYUVBuffer,  m_sVideoFormat.Width * m_sVideoFormat.Height * 3 / 2, false);
						else
							pMediaBuffer = FillMediaBuffer (m_sVideoDecOutBuf.Buffer[0],  m_sVideoFormat.Width * m_sVideoFormat.Height * 3 / 2, false);
					}
					else
					{
						pMediaBuffer = FillMediaBuffer (&m_sVideoDecOutBuf, sizeof (m_sVideoDecOutBuf), false);
					}

					if (pMediaBuffer == NULL)
					{
						VOLOGE ("Read Video Sample. pMediaBuffer == NULL");
						return VOSF_ERR_OUTOF_MEMORY;
					}

					if (m_bVideoYUVBuffer)
						m_pMediaBufferOP->set_range (pMediaBuffer, 0, m_sVideoFormat.Width * m_sVideoFormat.Height * 3 / 2);
					else
						m_pMediaBufferOP->set_range (pMediaBuffer, 0, sizeof (m_sVideoDecOutBuf));

					m_pMetaDataOP->clear (m_pMediaBufferOP->meta_data (pMediaBuffer));
					m_pMetaDataOP->setInt64 (m_pMediaBufferOP->meta_data (pMediaBuffer), kKeyTime, m_sVideoDecOutBuf.Time * 1000);
				}
			}

			m_pMediaBufferOP->add_ref(pMediaBuffer);
			*ppBuffer = pMediaBuffer;

			mbSeeking = VO_FALSE; 

			m_nVideoReadSamples++;
		}

		return nRC;
	}
	else
	{
		VOLOGE ("It could not find the track %d", nTrack);
	}
	return nRC;
}

void * CFileMediaExtractor::FillMediaBuffer (void * pData, int nSize, bool bAudio)
{
	void * pMediaBuffer = NULL;
	void ** ppBuffer = m_ppAudioBuffer;
	if (!bAudio)
		ppBuffer = m_ppVideoBuffer;

	int i = 0;
	for (i = 0; i < VOSF_MEDIABUFFER_NUM; i++)
	{
		if (ppBuffer[i] != NULL)
		{
			if (m_pMediaBufferOP->data (ppBuffer[i]) == pData)
			{
				pMediaBuffer = ppBuffer[i];
				break;
			}
		}
	}

	if (pMediaBuffer != NULL)
	{
		return pMediaBuffer;
	}

	pMediaBuffer = m_pMediaBufferOP->create (pData, nSize);
	if (pMediaBuffer == NULL)
	{
		return NULL;
	}

	m_pMediaBufferOP->setObserver(pMediaBuffer, this);
	for (i = 0; i < VOSF_MEDIABUFFER_NUM; i++)
	{
		if (ppBuffer[i] == NULL)
		{
			ppBuffer[i] = pMediaBuffer;
			break;
		}
	}

	if (i == VOSF_MEDIABUFFER_NUM)
	{
		m_pMediaBufferOP->setObserver (ppBuffer[0], NULL);
		m_pMediaBufferOP->release (ppBuffer[0]);
		ppBuffer[0] = pMediaBuffer;
	}

	return pMediaBuffer;
}

int CFileMediaExtractor::ReadAudioSample (void)
{
	
	if(m_pSource == NULL)
		return VOSF_ERR_WRONG_STATUS;

	VO_S32 nRC = 0;
	if (m_pLastAudioBuff != NULL)
	{
		m_pLastAudioBuff = NULL;
		return VOSF_ERR_NONE;
	}

	if (m_sAudioDecInBuf.Length > 0 && m_pAudioDec != NULL)
	{
		nRC = DecAudioSample ();
		if (nRC >= 0)
			return nRC;
	}

	while(true)
	{
		{
			voCAutoLock lock (&m_mtSource);
			m_nStartSysTime = voOS_GetSysTime ();
			nRC = m_pSource->GetTrackData(m_nAudioTrack , &m_sAudioSample);

			//if(mhAudioFile != 0)
			//{
			//	fwrite(m_sAudioSample.Buffer , 1 , m_sAudioSample.Size  & 0x7FFFFFFF , mhAudioFile);
			//}
		}

		if(nRC != VO_ERR_NONE)
		{
			if((VO_U32)nRC == VO_ERR_SOURCE_END)
			{
				VOLOGI ("Read at end of track!");
				return VOSF_ERR_EOS;
			}
			else if ((VO_U32)nRC == VO_ERR_SOURCE_NEEDRETRY)
			{
				voOS_Sleep (10);
				if (m_bStop)
					return VOSF_ERR_EOS;
				continue;
			}
			else
			{
				VOLOGE ("Read data from track error %08X!", nRC);
				return VOSF_ERR_FAILED;
			}
		}

		if (m_pAudioDec == NULL)
		{
			//VOLOGI ("Data Buffer %p, Size %d, Time %lld", m_sAudioSample.pBuf, m_sAudioSample.nBufLen, m_sAudioSample.Time);

			return VOSF_ERR_NONE;
		}

		m_sAudioDecInBuf.Buffer = m_sAudioSample.Buffer;
		m_sAudioDecInBuf.Length = m_sAudioSample.Size & 0x7FFFFFFF;
		m_sAudioDecInBuf.Time   = m_sAudioSample.Time;
		nRC = m_pAudioDec->SetInputData(&m_sAudioDecInBuf);
		if(nRC != VO_ERR_NONE)
		{
			VOLOGW ("m_pAudioDec->SetInputData result is %08X", nRC);
			continue;
		}

		nRC = DecAudioSample ();
		if (nRC >= 0)
			return nRC;
	}

	return VOSF_ERR_FAILED;
}

int CFileMediaExtractor::DecAudioSample (void)
{
	
	if (m_pAudioDec == NULL)
		return VOSF_ERR_WRONG_STATUS;

	m_sAudioDecOutBuf.Length = m_nAudioFrameSize;
	m_sAudioDecOutBuf.Time = m_sAudioSample.Time;

	VO_AUDIO_FORMAT	fmtAudio;
	int nRC = m_pAudioDec->GetOutputData(&m_sAudioDecOutBuf , &fmtAudio);

	if(nRC == VOSF_ERR_NONE)
	{
		if(mhAudioFile != 0)
		{
			fwrite(m_sAudioDecOutBuf.Buffer , 1 , m_sAudioDecOutBuf.Length , mhAudioFile);
		}

		m_sAudioSample.Time += m_sAudioDecOutBuf.Length * 1000 / m_nAudioBytesPerSec;
		if(fmtAudio.Channels != m_sAudioFormat.Channels || fmtAudio.SampleBits != m_sAudioFormat.SampleBits || fmtAudio.SampleRate != m_sAudioFormat.SampleRate)
		{
			m_pLastAudioBuff = &m_sAudioDecOutBuf;
			memcpy(&m_sAudioFormat , &fmtAudio , sizeof(m_sAudioFormat));

			m_nAudioBytesPerSec = m_sAudioFormat.SampleRate * m_sAudioFormat.Channels * m_sAudioFormat.SampleBits / 8;

			VOLOGI ("The audio format was changed to SampleRate %d, Channels %d, Bits %d",  fmtAudio.SampleRate, fmtAudio.Channels,  fmtAudio.SampleBits);
			return VOSF_ERR_FORMAT_CHANGED;
		}

		return VOSF_ERR_NONE;
	}

	return VOSF_ERR_FAILED;
}

int CFileMediaExtractor::ReadVideoSample (void)
{
	
	if(m_pSource == NULL)
		return VOSF_ERR_FAILED;

	VO_S32 nRC = VO_ERR_NONE;
	if (m_pLastVideoBuff != NULL)
	{
		m_pLastVideoBuff = NULL;
		return nRC;
	}

	if (m_pVideoDec != NULL && m_bMoreVideoBuff && m_sVideoSample.Size > 0)
	{
		nRC = DecVideoSample ();
		if (nRC >= 0)
			return nRC;
	}

	while (true)
	{
		{
			voCAutoLock lock (&m_mtRead);
			if (GetPlayingTime () > 1000)
				m_sVideoSample.Time = GetPlayingTime() + 50;
			else
				m_sVideoSample.Time = 0;

			nRC = m_pSource->GetTrackData(m_nVideoTrack , &m_sVideoSample);
			if( m_nVideoReadSamples == 0)
			{
				if((m_sVideoSample.Size & 0x7FFFFFFF) == m_sVideoTrackInfo.HeadSize)
				{
					if(memcmp(m_sVideoTrackInfo.HeadData , m_sVideoSample.Buffer , m_sVideoTrackInfo.HeadSize) == 0)
					{
						nRC = m_pSource->GetTrackData(m_nVideoTrack , &m_sVideoSample);
					}
				}
			}

			//VOLOGI("GetTrackData : %d  %lld  %p" , m_sVideoSample.Size & 0x7FFFFFFF , m_sVideoSample.Time , m_sVideoSample.Buffer);

			if(mhVideoFile != 0)
			{
				fwrite(m_sVideoSample.Buffer , 1 , m_sVideoSample.Size  & 0x7FFFFFFF , mhVideoFile);
			}
		}

		if(nRC != VO_ERR_NONE)
		{
			if((VO_U32)nRC == VO_ERR_SOURCE_END)
			{
				mbEOS = VO_TRUE;
				//VOLOGI ("Read at end of track!");
				return VOSF_ERR_EOS;
			}
			else if ((VO_U32)nRC == VO_ERR_SOURCE_NEEDRETRY)
			{
				voOS_Sleep (10);
				if (m_bStop)
					return VOSF_ERR_EOS;

				continue;
			}
			else
			{
				VOLOGE ("Read data from track error %08X!", nRC);
				return VOSF_ERR_FAILED;
			}
		}

		if (m_pVideoDec == NULL)
		{
			return VOSF_ERR_NONE;
		}

		if (m_sVideoSample.Flag & VO_SOURCE_SAMPLE_FLAG_FRAMEDROPPED)
		{
			m_bFileDropFrame = VO_TRUE;
			m_nNotFrameBnum = 0;

			// VOLOGI ("Video Frame is dropped in file!");
		}

		//m_pVideoDec->SetDelayTime ((VO_S32)(m_nAudioTimeStamp - m_nAudioBuffTime - m_sVideoSample.Time));

		m_sVideoDecInBuf.Buffer = m_sVideoSample.Buffer;
		m_sVideoDecInBuf.Length = m_sVideoSample.Size & 0x7FFFFFFF;
		m_sVideoDecInBuf.Time   = m_sVideoSample.Time;
	
		//Eric, 2010.12.14, fix crash, error occurs when get wrong sample,13165276_AVI_AVC_AC3_5481K'3m7s'1280x720'30f'2c'48KHz.avi
		if(m_sVideoDecInBuf.Length<=0)
			continue;

		nRC = m_pVideoDec->SetInputData(&m_sVideoDecInBuf);
		if(nRC != VO_ERR_NONE)
			continue;

		nRC = DecVideoSample ();
		if (nRC >= 0)
		{
			return nRC;
		}
	}
	
	return VOSF_ERR_FAILED;
}

int CFileMediaExtractor::DecVideoSample (void)
{
	
	if (m_pVideoDec == NULL)
		return VOSF_ERR_WRONG_STATUS;

	if (m_bFileDropFrame)
	{
		VO_VIDEO_FRAMETYPE nFrameType = m_pVideoDec->GetFrameType (NULL);
		if (nFrameType != VO_VIDEO_FRAME_B)
			m_nNotFrameBnum++;
		else
			return VOSF_ERR_NOT_IMPLEMENT;

		if (m_nNotFrameBnum >= 2)
		{
			if ((m_sVideoSample.Flag & VO_SOURCE_SAMPLE_FLAG_FRAMEDROPPED) == 0)
				m_bFileDropFrame = VO_FALSE;
		}
	}

	if (GetPlayingTime () > m_sVideoSample.Time + 100)
	{
		if (m_pVideoDec->GetFrameType (NULL) == VO_VIDEO_FRAME_B)
			return VOSF_ERR_NOT_IMPLEMENT;
	}

	VO_VIDEO_FORMAT  fmtVideo;
	m_bMoreVideoBuff = VO_FALSE;
	int nRC = m_pVideoDec->GetOutputData (&m_sVideoDecOutBuf , &fmtVideo , &m_bMoreVideoBuff);

	
	if(nRC == VO_ERR_NONE && m_sVideoDecOutBuf.Buffer[0] != NULL)
	{
		//VOLOGI("Decoder return frame type %d " , fmtVideo.Type);
		m_sVideoDecOutBuf.Time = m_sVideoSample.Time;
		m_nLastVideoBuffTime   = m_sVideoSample.Time;
		if(fmtVideo.Height != m_sVideoFormat.Height || fmtVideo.Width != m_sVideoFormat.Width)
		{
			m_pLastVideoBuff = &m_sVideoDecOutBuf;

			m_sVideoFormat.Height = fmtVideo.Height;
			m_sVideoFormat.Width  = fmtVideo.Width;
			m_sVideoFormat.Type   = fmtVideo.Type;
			return VOSF_ERR_FORMAT_CHANGED;
		}
		
		VO_S32	nPlayTime = GetPlayingTime ();
		if (nPlayTime > 0)
		{
			if (nPlayTime > m_sVideoDecOutBuf.Time + 200)
			{
				if (m_bDropFrame)
				{
					if (!m_bFrameDropped)
					{
						m_bFrameDropped = VO_TRUE;
						return VOSF_ERR_FAILED;
					}
				}
			}
		}

		//VOLOGI ("Buffer: %p, U %p, V, %p  stride %d, Color %d", m_sVideoDecOutBuf.Buffer[0], m_sVideoDecOutBuf.Buffer[1], m_sVideoDecOutBuf.Buffer[2], m_sVideoDecOutBuf.Stride[0], m_sVideoDecOutBuf.ColorType);
		m_bFrameDropped = VO_FALSE;
		return VOSF_ERR_NONE;
	}

	return VOSF_ERR_FAILED;
}

int CFileMediaExtractor::GetPlayingTime (void)
{
#ifdef _GINGERBREAD
	if(mbInnerClock == VO_FALSE)
		return mllClockTime;
	else
		return 0;
#endif

	int nPlayTime;
	if (m_nAudioTrack >= 0)
	{
		if (m_sAudioSample.Time <= m_nAudioBuffTime)
			return 0;

		if (m_nStartSysTime == 0)
			m_nStartSysTime = voOS_GetSysTime ();
		nPlayTime = (int)(m_sAudioSample.Time - m_nAudioBuffTime +  voOS_GetSysTime () - m_nStartSysTime);
	}
	else
	{
		if (m_nStartSysTime == 0)
			m_nStartSysTime = (int)(voOS_GetSysTime () - m_sVideoSample.Time);
		nPlayTime = voOS_GetSysTime () - m_nStartSysTime;
	}
	//it will be consider as pause action if the time between two video frame exceeds a constant time, here is 500 ms
	// m_sStartSysTime needs to be synchronized again
	if(nPlayTime - m_nLastVideoBuffTime >500)
	{
		m_nStartSysTime = voOS_GetSysTime();
		m_nLastVideoBuffTime = nPlayTime;
		nPlayTime = GetPlayingTime();
	}
	return nPlayTime;
}

int CFileMediaExtractor::vosfFileSourceStreamCallBack (long EventCode, long * EventParam1, long * userData)
{
   
	CFileMediaExtractor *	pSource = (CFileMediaExtractor *)userData;
	vosfEventType			msg;

	if (EventCode == VO_STREAM_BUFFERSTART)
	{
		//VOLOGI ("VO_STREAM_BUFFERSTART");
		pSource->m_nBufferPercent = 0;

		msg.nMsg = MEDIA_BUFFERING_UPDATE;
		msg.nExt1 = pSource->m_nBufferPercent;
	}
	else if (EventCode == VO_STREAM_BUFFERSTATUS)
	{
		//VOLOGI ("VO_STREAM_BUFFERSTATUS  %d ", *EventParam1);
		pSource->m_nBufferPercent = (VO_U32)*EventParam1;

		msg.nMsg = MEDIA_BUFFERING_UPDATE;
		msg.nExt1 = pSource->m_nBufferPercent;
	}
	else if (EventCode == VO_STREAM_BUFFERSTOP)
	{
		//VOLOGI ("VO_STREAM_BUFFERSTOP");
		pSource->m_nBufferPercent = 100;

		msg.nMsg = MEDIA_BUFFERING_UPDATE;
		msg.nExt1 = pSource->m_nBufferPercent;
	}
	else if (EventCode == VO_STREAM_DOWNLODPOS)
	{
		//VOLOGI ("VO_STREAM_DOWNLODPOS  %d ", *EventParam1);

		msg.nMsg = MEDIA_BUFFERING_UPDATE;
		msg.nExt1 =  *EventParam1;
	}
	else if (EventCode == VO_STREAM_ERROR)
	{
		//VOLOGI ("VO_STREAM_ERROR  %d ", *EventParam1);

		pSource->m_bStop = VO_TRUE;

		msg.nMsg = MEDIA_ERROR;
		msg.nExt1 =  MEDIA_ERROR_UNKNOWN;
	}
	else if (EventCode == VO_STREAM_RUNTIMEERROR)
	{
		//VOLOGI ("VO_STREAM_RUNTIMEERROR  %d ", *EventParam1);

		pSource->m_bStop = VO_TRUE;

		msg.nMsg = MEDIA_ERROR;
		msg.nExt1 =  MEDIA_ERROR_UNKNOWN;
	}
	else if (EventCode == VO_STREAM_PACKETLOST)
	{
		//VOLOGI ("VO_STREAM_PACKETLOST");

		msg.nMsg = MEDIA_INFO;
		msg.nExt1 =  MEDIA_INFO_VIDEO_TRACK_LAGGING;
	}

	if (pSource->m_pNotifyFunc != NULL)
		pSource->m_pNotifyFunc (pSource->m_pUserData, VOSF_CID_NotifyEvent, &msg, NULL);

	return 0;
}

void CFileMediaExtractor::addESDSFromAudioSpecificInfo(void* pMeta, const void *pHeadData, VO_S32 nHeadSize) 
{
		static const VO_BYTE kStaticESDS[] = {
			0x03, 22,
			0x00, 0x00,     // ES_ID
			0x00,           // streamDependenceFlag, URL_Flag, OCRstreamFlag
			0x04, 17,
			0x40,                       // Audio ISO/IEC 14496-3
			0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00,
			0x05,			// AudioSpecificInfo (with size prefix) follows
		};

		VO_S32 esdsSize = sizeof(kStaticESDS) + nHeadSize + 1;
		VO_BYTE *esds = new VO_BYTE[esdsSize];
		memcpy(esds, kStaticESDS, sizeof(kStaticESDS));
		VO_BYTE *ptr = esds + sizeof(kStaticESDS);
		*ptr++ = nHeadSize;
		memcpy(ptr, pHeadData, nHeadSize);

		m_pMetaDataOP->setData(pMeta, kKeyESDS, 0, esds, esdsSize);
		delete[] esds;
		esds = NULL;
}

void CFileMediaExtractor::AddOggCodecInfo(void* pMeta, VO_BYTE *pHeadData, VO_S32 nHeadSize)
{
	VO_PBYTE pData = pHeadData;
	int	 nDataSize = nHeadSize;

	//VOLOGI("AddOggCodecInfo");

	VO_PBYTE  pBP = NULL;
	int nFlag = 0;
	int nPacketSize = 0;
	while(nDataSize > 8)
	{
		if(memcmp(pData, "vorbis", 6) ==0 )
		{
			if(pBP != NULL)
			{
				if(nFlag == 1)
				{
					nPacketSize = pData - pBP - 1;
					m_pMetaDataOP->setData(pMeta, kKeyVorbisInfo, TYPE_POINTER, pBP, nPacketSize);

					//VOLOGI("kKeyVorbisInfo  %d  %x " , nPacketSize , pBP[0]);
					pBP = NULL;
					nFlag = 0;
				}
				else if(nFlag == 3)
				{
					nPacketSize = pData - pBP - 1;
					pBP = NULL;
					nFlag = 0;
				}
				else if(nFlag == 5)
				{
					nPacketSize = pData - pBP - 1;
					m_pMetaDataOP->setData(pMeta, kKeyVorbisBooks, TYPE_POINTER, pBP, nPacketSize);

					//VOLOGI("kKeyVorbisBooks  %d  %x " , nPacketSize , pBP[0]);
					pBP = NULL;
					nFlag = 0;
				}
			}


			VO_PBYTE pTemp = pData -1;
			nFlag = pTemp[0];
			pBP = pData - 1;

			pData += 6;
			nDataSize -= 6;
		}

		pData += 1;
		nDataSize -= 1;
	}

	if(nFlag == 5 && pBP != NULL)
	{
		nPacketSize = nHeadSize - (pBP - pHeadData);
		m_pMetaDataOP->setData(pMeta, kKeyVorbisBooks, TYPE_POINTER , pBP, nPacketSize);

		//VOLOGI("kKeyVorbisBooks  %d  %x " , nPacketSize , pBP[0]);
	}
}

VO_S32	CFileMediaExtractor::voDecodeProc (VO_PTR pParam)
{
	CFileMediaExtractor *pSource = (CFileMediaExtractor *)pParam;
	if(pSource == NULL)
		return -1;

	return pSource->voDecodeLoop();
}

VO_S32	CFileMediaExtractor::voDecodeLoop()
{
	while(mbStop == VO_FALSE)
	{
		VOLOGI("");
		while(mbSeeking == VO_TRUE && mbStop == VO_FALSE)
		{
			voOS_Sleep(2);
		}

		VO_S32 nRC = 0;
		{
			voCAutoLock lock(&m_mtDecoder);
			
			nRC = ReadVideoSample();
		}

		if (nRC < 0  || mbSeeking == VO_TRUE || mbEOS == VO_TRUE)
		{
			if(mbEOS == VO_TRUE)
			{
				voOS_Sleep(5);
			}

			continue;
		}


		while(mnWriteIndex - mnReadIndex >= (mnBufferCount -1)  && mbStop == VO_FALSE)
		{
			voOS_Sleep(2);
		}

		if(mbStop == VO_TRUE)
		{
			break;
		}

		if(mbSeeking == VO_TRUE)
			continue;

		voCAutoLock lock(&m_mtDecoder);
		int nIndex = mnWriteIndex % mnBufferCount;

		if (m_bVideoYUVBuffer)
		{
			if(mpBufferList[nIndex].Buffer[0] == NULL)
			{
				mpBufferList[nIndex].Buffer[0] = new VO_BYTE[ m_sVideoFormat.Width * m_sVideoFormat.Height * 3 / 2];
				if(mpBufferList[nIndex].Buffer[0] == NULL)
				{
					mnBufferCount = nIndex;
					continue;
				}

				mpBufferList[nIndex].Buffer[1] = mpBufferList[nIndex].Buffer[0] + m_sVideoFormat.Width * m_sVideoFormat.Height;
				mpBufferList[nIndex].Buffer[2] = mpBufferList[nIndex].Buffer[0] + m_sVideoFormat.Width * m_sVideoFormat.Height * 5 / 4;
				mpBufferList[nIndex].Stride[0] = m_sVideoFormat.Width;
				mpBufferList[nIndex].Stride[1] = m_sVideoFormat.Width / 2;
				mpBufferList[nIndex].Stride[2] = m_sVideoFormat.Width / 2;
			}

			VO_PBYTE pYUV = mpBufferList[nIndex].Buffer[0];
			
			copyYUVFrame(&pYUV);
			mpBufferList[nIndex].Time = m_sVideoDecOutBuf.Time;
		}
		else
		{
			memcpy(&mpBufferList[nIndex] , &m_sVideoDecOutBuf , sizeof(m_sVideoDecOutBuf));
			mpBufferList[nIndex].Time = m_sVideoDecOutBuf.Time;
		}

		mnWriteIndex += 1;

		//VOLOGI(" Writer index : %03d    %04d   %lld " , nIndex , mnWriteIndex ,  m_sVideoDecOutBuf.Time);


	}

	mhDecThread = NULL;
	return 0;
}

void *	 CFileMediaExtractor::FindMediaBuffer(void *pBuf)
{
	if(m_pMediaBufferOP == NULL)
		return NULL;

	for(unsigned int i = 0 ; i < mnBufferCount ; i++)
	{
		if(mppMediaBufList[i] != NULL)
		{
			if (pBuf == m_pMediaBufferOP->data(mppMediaBufList[i]))
				return mppMediaBufList[i];
		}
	}

	return NULL;
}

bool	CFileMediaExtractor::AddMediaBuffer(void *pBuf)
{
	for(unsigned int i = 0 ; i < mnBufferCount ; i++)
	{
		if(mppMediaBufList[i] == NULL)
		{
			mppMediaBufList[i] = pBuf;
			return true;
		}
	}

	VOLOGI("Add MediaBuffer count more then mnBufferCount");
	return false;
}

bool	CFileMediaExtractor::ReleaseMediaBuf()
{
	if(mppMediaBufList == NULL)
		return true;

	for(unsigned int i = 0 ; i < mnBufferCount ; i++)
	{
		if(mppMediaBufList[i] != NULL)
		{
			m_pMediaBufferOP->setObserver (mppMediaBufList[i] , NULL);
			m_pMediaBufferOP->release (mppMediaBufList[i]);
			mppMediaBufList[i] = NULL;
		}
	}

	delete [] mppMediaBufList;
	mppMediaBufList = NULL;

	return true;
}

void		CFileMediaExtractor::ReleaseBufList()
{
	if(m_bVideoYUVBuffer == VO_FALSE || mpBufferList == NULL)
		return;

	for(unsigned int i = 0 ; i < mnBufferCount ; i++)
	{
		if(mpBufferList[i].Buffer[0] != NULL)
		{
			delete [] mpBufferList[i].Buffer[0];
			mpBufferList[i].Buffer[0] = NULL;
			mpBufferList[i].Buffer[1] = NULL;
			mpBufferList[i].Buffer[2] = NULL;
		}
	}

	delete mpBufferList;
	mpBufferList = NULL;
}

void CFileMediaExtractor::copyYUVFrame(VO_PBYTE* ppBuffer)
{
	unsigned char* pYUV = *ppBuffer;
	assert(pYUV != NULL);
	unsigned char *dest_y, *src_y, *dest_u, *src_u, *dest_v, *src_v;
	int  line;

	int inWidth = m_sVideoFormat.Width ;
	int inHeight = m_sVideoFormat.Height;

	int       nHalfWidth, nHalfHeight;

	nHalfWidth = inWidth / 2;
	nHalfHeight = inHeight / 2;
	
	src_y = (unsigned char*)m_sVideoDecOutBuf.Buffer[0];
	src_u = (unsigned char*)m_sVideoDecOutBuf.Buffer[1];
	src_v = (unsigned char*)m_sVideoDecOutBuf.Buffer[2];
			

	dest_y = pYUV;
	dest_u = pYUV + inWidth*inHeight;
	dest_v = pYUV + inWidth*inHeight * 5 / 4 ;
	

	if(src_y && src_u && src_v)
	{
		for(line = 0; line < inHeight; line++)
		{
			memcpy(dest_y, src_y, inWidth);
			dest_y +=  inWidth;
		  
			src_y += m_sVideoDecOutBuf.Stride[0];
			if(line < nHalfHeight)
			{
				memcpy(dest_u, src_u, nHalfWidth);
				memcpy(dest_v, src_v, nHalfWidth);

				dest_u += nHalfWidth;
				dest_v += nHalfWidth;			 
			 
				src_u += m_sVideoDecOutBuf.Stride[1];
				src_v += m_sVideoDecOutBuf.Stride[2];
			}
		}
	}
}
