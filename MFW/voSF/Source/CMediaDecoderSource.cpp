	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CMediaDecoderSource.cpp

	Contains:	CMediaDecoderSource class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-18		JBF			Create file

*******************************************************************************/
#include <malloc.h>
#include <string.h>
#include <stdio.h>

#include "voAudio.h"
#include "voVideo.h"
#include "voFile.h"
#include "cmnMemory.h"
#include "voOSFunc.h"
#include "voH264.h"

#include "OMX_Video.h"

#include "CFileFormatCheck.h"

#include "CDataSourceOP.h"

#include "CMediaDecoderSource.h"
#include "CFileMediaSource.h"

#define LOG_TAG "CMediaDecoderSource"

#include "voLog.h"

CMediaDecoderSource::CMediaDecoderSource(void * pMediaSource, int nFlag,
										 VOSF_MEDIABUFFERTYPE * pMediaBufferOP, VOSF_METADATATYPE * pMetaDataOP)
	: CBaseMediaSource (pMediaBufferOP, pMetaDataOP)
	, m_bIsVideo (false)
	, m_pMediaSource ((VOSF_MEDIASOURCETYPE *)pMediaSource)
	, m_pMeidaBuffer (NULL)
	, m_pInputBuffer (NULL)
	, m_llDuration (0)
	, m_llMediaTime (0)
	, m_nMediaFrameCount (0)
	, m_pAudioDec (NULL)
	, m_pLastAudioBuff (NULL)
	, m_nAudioFrameSize (0)
	, m_pVideoDec (NULL)
	, m_pLastVideoBuff (NULL)
	, m_bVideoYUVBuffer (VO_TRUE)
	, m_bVideoYUVCopy (VO_TRUE)
	, m_pVideoYUVBuffer (NULL)
	, m_nCoreNumber(1)
	, mnSeqDataLen(0)
	, mpSeqData(NULL)
	, mnDecType(VO_VIDEO_CodingUnused)
	, m_pBufferList(NULL)
	, m_llReferenceTime(0)
	, m_llSystemTime(0)
	, mbUseBuffer(VO_FALSE)
	, mnDrawFrames(0)
	, mnDrawBFrames(0)
	, mnWaterMark(1)
	, mnDropAllBFrame(80000)
	, mbDumpData(VO_FALSE)
	, mhDumpDataFile(NULL)
	, mbDumpOutData(VO_FALSE)
	, mhDumpOutData(NULL)
	, mnLogLevel(0)
	, mbUseDecThread(VO_TRUE)
	, mhDecThread(NULL)
	, mnDecThreadID(0)
	, mnReadIndex(0)
	, mnWriteIndex(0)
	, mpBufferList(NULL)
	, mppMediaBufList(NULL)
	, mbStop(VO_FALSE)
	, mbSeeking(VO_FALSE)
	, mbEOS(VO_FALSE)
	, mllLastTime(0)
	, mllLastOrgTime(0)
	, mbUseVideoMemOP(VO_FALSE)
	, mnDeblockBufCount(5)
	, mnDeblockEdge(10)
	, mnDeInterlace(3)	
{
	cmnMemFillPointer('SFPS');

	strcpy (m_szMIME, "");

	memset (&m_sAudioFormat, 0, sizeof (m_sAudioFormat));
	m_sAudioFormat.SampleBits = 16;
	memset (&m_sAudioDecInBuf, 0, sizeof (m_sAudioDecInBuf));
	memset (&m_sAudioDecOutBuf, 0, sizeof (m_sAudioDecOutBuf));

	memset (&m_sVideoFormat, 0, sizeof (m_sVideoFormat));
	memset (&m_sVideoDecInBuf, 0, sizeof (m_sVideoDecInBuf));
	memset (&m_sVideoDecOutBuf, 0, sizeof (m_sVideoDecOutBuf));

	memset (mBufferCount , 0 , 10 * sizeof(VO_S32 ));
	
	VO_TCHAR szPlayCfg[64];
	vostrcpy(szPlayCfg,_T("vosfPlay.cfg"));
	m_cfgPlay.Open (szPlayCfg);
	m_bVideoYUVBuffer = m_cfgPlay.GetItemValue ("Playback", "YUV420Buffer", 1) == 1 ? VO_TRUE : VO_FALSE;
	mbDumpData		= m_cfgPlay.GetItemValue ("Playback", "DumpData", 0) == 1 ? VO_TRUE : VO_FALSE;
	mbDumpOutData	= m_cfgPlay.GetItemValue ("Playback", "DumpOutData", 0) == 1 ? VO_TRUE : VO_FALSE;
	mnLogLevel		= m_cfgPlay.GetItemValue("Playback" , "LogLevel" , 1);
	m_nCoreNumber	= m_cfgPlay.GetItemValue("Playback" , "CoreNumber" , 1);
	mbUseBuffer		=  m_cfgPlay.GetItemValue ("Playback", "UseBuffer", 1) == 1 ? VO_TRUE : VO_FALSE;
	mbUseDecThread	= m_cfgPlay.GetItemValue("Playback" , "UseDecThread" , 1) == 1 ? VO_TRUE : VO_FALSE;
	mbDropFrames	= m_cfgPlay.GetItemValue("Playback" , "DropFrames" , 1) == 1 ? VO_TRUE : VO_FALSE;
	mnBufferCount	= m_cfgPlay.GetItemValue("Playback" , "BufferCount" , 12);
	mnWaterMark		= m_cfgPlay.GetItemValue("Playback" , "WaterMark" , 4);
	mnDropAllBFrame = m_cfgPlay.GetItemValue("Playback", "DropAllBFrames" , 80000);
	mnDeblockBufCount  = m_cfgPlay.GetItemValue("Playback", "Deblock" , 5);
	mnDeblockEdge	= m_cfgPlay.GetItemValue("Playback", "DeblockEdge" , 10);
	mnDeInterlace	= m_cfgPlay.GetItemValue("Playback", "Deinterlace" , 3);
	
	if(mnWaterMark > mnBufferCount)
	{
		mnWaterMark = mnBufferCount / 3;
	}
	
	if(mnLogLevel > 0)
		VOLOGI("mnLogLevel = %d  mbDumpData = %d  m_nCoreNumber = %d  mbUseBuffer = %d  mbDropFrames = %d  mnBufferCount = %d mnWaterMark = %d mnDropAllBFrame = %d  mnDeblockBufCount = %d  mnDeblockEdge = %d " ,
		mnLogLevel ,mbDumpData , m_nCoreNumber , mbUseBuffer , mbDropFrames , mnBufferCount , mnWaterMark , mnDropAllBFrame , mnDeblockBufCount , mnDeblockEdge);

	if(mbDumpData == VO_TRUE)
	{
		char *pFile = m_cfgPlay.GetItemText("Playback" , "DumpInputDataFile");

		if(mnLogLevel > 0)
			VOLOGI("dump file : %s " , pFile);
		mhDumpDataFile = fopen(pFile , "wb");
		if(mnLogLevel > 0)
			VOLOGI("file %d" , (int)mhDumpDataFile);
	}

	if(mbDumpOutData == VO_TRUE)
	{
		char *pFile = m_cfgPlay.GetItemText("Playback" , "DumpOutputDataFile");

		if(mnLogLevel > 0)
			VOLOGI("dump file : %s " , pFile);
		mhDumpOutData = fopen(pFile , "wb");
		if(mnLogLevel > 0)
			VOLOGI("file %d" , (int)mhDumpOutData);
	}

	mpBufferList = new VO_VIDEO_BUFFER[mnBufferCount];
	memset (mpBufferList , 0 , sizeof(VO_VIDEO_BUFFER ) * mnBufferCount);

	mppMediaBufList = new void *[mnBufferCount];
	memset (mppMediaBufList , 0 , sizeof(void *) * mnBufferCount);

	void * pMetaData = m_pMediaSource->getFormat (m_pMediaSource->pMediaSource);
	if (pMetaData != NULL)
	{
		const char * pMIME = NULL;

		m_pMetaDataOP->findInt64 (pMetaData, kKeyDuration , &m_llDuration);
		m_pMetaDataOP->findCString (pMetaData, kKeyMIMEType, &pMIME);
		if (pMIME != NULL)
		{
			strcpy (m_szMIME, pMIME);
			if(mnLogLevel > 0)
				VOLOGI ("The MIME is %s", pMIME);
			if (strstr (m_szMIME, "audio/") == NULL)
			{
				m_bIsVideo = true;
				m_pMetaDataOP->findInt32 (pMetaData, kKeyWidth, (int *)&m_sVideoFormat.Width);	
				m_pMetaDataOP->findInt32 (pMetaData, kKeyHeight, (int *)&m_sVideoFormat.Height);
				if(mnLogLevel > 0)
					VOLOGI ("Video Size %d, %d", m_sVideoFormat.Width, m_sVideoFormat.Height);

				int nType = 0;
				if(!m_pMetaDataOP->findData(pMetaData , kKeyAVCC , &nType , (const void **)&mpSeqData , &mnSeqDataLen))
				{
					if(mnLogLevel > 0)
						VOLOGI("Can't get kKeyAVCC data");
					mnSeqDataLen = 0;
					mpSeqData = NULL;
				}

				CreateVideoDec ();
			}
			else
			{
				m_bIsVideo = false;
				m_pMetaDataOP->findInt32 (pMetaData, kKeySampleRate, (int *)&m_sAudioFormat.SampleRate);	
				m_pMetaDataOP->findInt32 (pMetaData, kKeyChannelCount, (int *)&m_sAudioFormat.Channels);
				
				if(mnLogLevel > 0)
					VOLOGI ("Audio Info %d, %d", m_sAudioFormat.SampleRate, m_sAudioFormat.Channels);
				CreateAudioDec ();
			}
		}
		else
		{
			VOLOGE ("m_pMetaDataOP->findCString (pMetaData, kKeyMIMEType, &pMIME); return NULL");
		}
	}
	else
	{
		VOLOGE ("m_pMediaSource->getFormat return NULL");
	}
}

CMediaDecoderSource::~CMediaDecoderSource(void)
{
	mbStop = VO_TRUE;

	int nTimes = 0;
	while (mhDecThread != NULL && nTimes < 200)
	{
		//VOLOGI("mhDecThread : %p %d " , mhDecThread , nTimes);
		voOS_Sleep(5);
		nTimes ++;
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

	if (m_sAudioDecOutBuf.Buffer != NULL)
		delete []m_sAudioDecOutBuf.Buffer;

	if (m_pMeidaBuffer != NULL)
	{
		m_pMediaBufferOP->setObserver (m_pMeidaBuffer, NULL);
		m_pMediaBufferOP->release (m_pMeidaBuffer);
	}

	if (m_pInputBuffer != NULL)
		m_pMediaBufferOP->release (m_pInputBuffer);


	ReleaseMediaBuf();
	ReleaseBufList();

	if (m_pVideoYUVBuffer != NULL)
	{
		delete []m_pVideoYUVBuffer;
		m_pVideoYUVBuffer = NULL;
	}

	if(mhDumpDataFile != NULL)
	{
		fclose(mhDumpDataFile);
		mhDumpDataFile = NULL;
	}

	if(mhDumpOutData != NULL)
	{
		fclose(mhDumpOutData);
		mhDumpOutData = NULL;
	}

	if(m_pBufferList !=NULL)
	{
		delete m_pBufferList;
		m_pBufferList = NULL;
	}
}

void CMediaDecoderSource::signalBufferReturned (void * pBuffer)
{
//	VOLOGI ("Buffer is %p", pBuffer);
}

int CMediaDecoderSource::Start (void * pParam)
{
	if(mnLogLevel > 0)
		VOLOGI ("pParam  %p", pParam);

	m_nMediaFrameCount = 0;

	mbStop = VO_FALSE;

	if (m_pInputBuffer != NULL)
	{
		m_pMediaBufferOP->release (m_pInputBuffer);
		m_pInputBuffer = NULL;
	}

	if(mpMemVideoOP != NULL)
	{
		if(mnLogLevel > 0 )
		{
			VOLOGI("Use out memory operator !!!");
		}
		ReleaseBufList();

		VO_MEM_VIDEO_INFO memInfo;
		memInfo.ColorType = (VO_IV_COLORTYPE)OMX_VO_COLOR_FormatYUV420StridePlanar;
		memInfo.FrameCount = mnBufferCount;
		memInfo.Height = m_sVideoFormat.Height;
		memInfo.Stride = m_sVideoFormat.Width;

		if(mpMemVideoOP->Init(1 , &memInfo) == 0)
		{
			mpBufferList = memInfo.VBuffer;
			mbUseVideoMemOP = VO_TRUE;

			if(mnLogLevel > 0 )
			{
				for(unsigned int i = 0 ; i < mnBufferCount ; i++)
				{
					if(mpBufferList[i].Buffer[0] == NULL)
					{
						mnBufferCount = i;
					}

					VOLOGI("Out memory operator allocate buffer : %p " , mpBufferList[i].Buffer[0]);
				}
			}
		}
		else
		{
			mpBufferList = new VO_VIDEO_BUFFER[mnBufferCount];
			memset (mpBufferList , 0 , sizeof(VO_VIDEO_BUFFER ) * mnBufferCount);
		}
	}

	int nRC = m_pMediaSource->start (m_pMediaSource->pMediaSource, pParam);
	if(mnLogLevel > 0)
	{
		VOLOGI ("start return %d", nRC);
	}

	return nRC;
}

int CMediaDecoderSource::Stop (void)
{
	if(mnLogLevel > 0)
	{
		VOLOGI("m_nMediaFrameCount : %lld  mllLastTime = %lld " , m_nMediaFrameCount , mllLastOrgTime);
		if(mllLastOrgTime / 1000000 > 0)
		{
			VOLOGI("********************Decode FPS : %lld ***********************" , m_nMediaFrameCount * 100 / (mllLastOrgTime / 1000000) );
		}
	}

	m_nMediaFrameCount = 0;

	mbStop = VO_TRUE;

	int nTimes = 0;
	while (mhDecThread != NULL && nTimes < 200)
	{
		//VOLOGI("mhDecThread : %p %d " , mhDecThread , nTimes);
		voOS_Sleep(5);
		nTimes ++;
	}

	if (m_pInputBuffer != NULL)
	{
		m_pMediaBufferOP->release (m_pInputBuffer);
		m_pInputBuffer = NULL;
	}

	int nRC =  m_pMediaSource->stop (m_pMediaSource->pMediaSource);

	if(mnLogLevel > 0)
		VOLOGI ("stop return %d", nRC);

	return nRC;
}

void * CMediaDecoderSource::GetFormat (void)
{
	void * pMetaData = m_pMetaDataOP->create ();
	if (!m_bIsVideo)
	{
		m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "audio/raw");
		m_pMetaDataOP->setCString(pMetaData, kKeyDecoderComponent, "VO.AudioDec");
		m_pMetaDataOP->setInt64(pMetaData, kKeyDuration , m_llDuration);
		m_pMetaDataOP->setInt32(pMetaData, kKeySampleRate , m_sAudioFormat.SampleRate);
		m_pMetaDataOP->setInt32(pMetaData, kKeyChannelCount , m_sAudioFormat.Channels);

		if(mnLogLevel > 0)
		{
			VOLOGI ("Audio format %d, %d", m_sAudioFormat.SampleRate, m_sAudioFormat.Channels);
		}
	}
	else
	{
		m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , "video/raw");
		m_pMetaDataOP->setCString(pMetaData, kKeyDecoderComponent, "VO.VideoDec");
		m_pMetaDataOP->setInt64(pMetaData, kKeyDuration , m_llDuration);
		m_pMetaDataOP->setInt32(pMetaData, kKeyWidth , m_sVideoFormat.Width);
		m_pMetaDataOP->setInt32(pMetaData, kKeyHeight , m_sVideoFormat.Height);

		if (m_bVideoYUVBuffer)
			m_pMetaDataOP->setInt32(pMetaData, kKeyColorFormat , OMX_COLOR_FormatYUV420Planar);
		else
			m_pMetaDataOP->setInt32(pMetaData, kKeyColorFormat , OMX_VO_COLOR_FormatYUV420StridePlanar);

		if(mnLogLevel > 0)
			VOLOGI ("Video Info, %d, %d", m_sVideoFormat.Width, m_sVideoFormat.Height);
	}

	return pMetaData;
}

int CMediaDecoderSource::Read (void ** ppBuffer, void * pOptions)
{
	int nRC = VOSF_ERR_NONE;

	VO_U32 uSystemTime = 0;
	if(mnLogLevel > 2)
	{
		uSystemTime = voOS_GetSysTime();
		VOLOGI("Render used time : %d   systemtime : %d " , uSystemTime - mnSystemTime , uSystemTime );
	}

	if (!m_bIsVideo)
	{
		if (m_pAudioDec == NULL)
			return VOSF_ERR_WRONG_STATUS;

		nRC = ReadAudioSample (pOptions);
		if (nRC == VOSF_ERR_NONE)
		{
			if (m_pMeidaBuffer == NULL)
			{
				m_pMeidaBuffer = m_pMediaBufferOP->create (m_sAudioDecOutBuf.Buffer, m_nAudioFrameSize);
				if (m_pMeidaBuffer == NULL)
					return VOSF_ERR_OUTOF_MEMORY;
				m_pMediaBufferOP->setObserver(m_pMeidaBuffer, this);
			}

			m_pMediaBufferOP->set_range (m_pMeidaBuffer, 0, (m_sAudioDecOutBuf.Length));
		}
	}
	else
	{
		if(mhDecThread == NULL && mbUseDecThread == VO_TRUE && mbStop == VO_FALSE)
		{
			voThreadCreate(&mhDecThread , &mnDecThreadID , (voThreadProc )voDecodeProc , this , 1);
		}

		VOSF_READOPTIONTYPE* pReadOption = (VOSF_READOPTIONTYPE*)pOptions;

		int Mode;
		long long llClockTime = 0;
		if(pReadOption&&pReadOption->getSeekTo(pReadOption->pOptions, &llClockTime, &Mode))
		{
			UpdateClockTime(llClockTime);

			if( Mode == 4)
			{
				pReadOption->clearSeekTo(pReadOption->pOptions);
			}
			else
			{
				mbSeeking = VO_TRUE;
				mbEOS = VO_FALSE;

				voCAutoLock lock(&m_mtDecoder);
				mnReadIndex = mnWriteIndex = 0;
				m_pVideoDec->Flush();

				mllLastTime = 0;
				memset (mBufferCount , 0 , 10 * sizeof(VO_S32 ));

				if(m_pBufferList)
					m_pBufferList->releaseList();
			}
		}

		if(mbUseDecThread == VO_TRUE &&  mbSeeking != VO_TRUE)
		{
			while(mnWriteIndex <= mnReadIndex && mbStop == VO_FALSE && mbEOS == VO_FALSE)
			{
				voOS_Sleep(2);
			}

			memmove(mBufferCount , mBufferCount + 1 , 9 * sizeof(VO_S32));
			mBufferCount[9] = mnWriteIndex - mnReadIndex;

			//VOLOGI(" mnReadIndex : %04d  mnWriteIndex : %04d  diff : %d " , mnReadIndex , mnWriteIndex , mnWriteIndex - mnReadIndex);

			if(mbEOS == VO_TRUE && mnWriteIndex - mnReadIndex <= 1)
				return VOSF_ERR_EOS;

			if(mbStop == VO_TRUE)
				return VOSF_ERR_WRONG_STATUS;

			
			int nIndex = mnReadIndex % mnBufferCount;
			VO_VIDEO_BUFFER *pVideoBuffer = &(mpBufferList[nIndex]);

			if( mhDumpOutData != NULL && pVideoBuffer->Buffer[0] != NULL )
			{
				for(int i = 0 ; i <  m_sVideoFormat.Height ; i++)
				{
					fwrite(pVideoBuffer->Buffer[0] + i * pVideoBuffer->Stride[0] , 1 ,m_sVideoFormat.Width , mhDumpOutData);
				}

				for(int i = 0 ; i < m_sVideoFormat.Height / 2 ; i++)
				{
					fwrite(pVideoBuffer->Buffer[1] + i * pVideoBuffer->Stride[1] , 1 , m_sVideoFormat.Width / 2 , mhDumpOutData);
				}

				for(int i = 0 ; i < m_sVideoFormat.Height / 2 ; i++)
				{
					fwrite(pVideoBuffer->Buffer[2] + i * pVideoBuffer->Stride[2] , 1 ,m_sVideoFormat.Width / 2 , mhDumpOutData);
				}
			}

			if(mnLogLevel > 2)
			{
				VOLOGI("mnWriteIndex : %04d   mnReadIndex : %04d   clocktime : %08lld   timestamp : %08lld " , mnWriteIndex , mnReadIndex , GetClockTime() , pVideoBuffer->Time);
			}
			
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
			m_pMetaDataOP->setInt64 (m_pMediaBufferOP->meta_data (pMediaBuf), kKeyTime,pVideoBuffer->Time);
			
			m_pMediaBufferOP->add_ref (pMediaBuf);
			*ppBuffer = pMediaBuf;
			mnReadIndex += 1;
			//VOLOGI(" Read index : %02d  %04d  %lld " , nIndex , mnReadIndex , pVideoBuffer->Time);


			if(mnLogLevel > 2)
			{
				mnSystemTime = voOS_GetSysTime();
				VOLOGI("Decoder use time : %d   systemtime : %d  timestamp : %lld " , mnSystemTime - uSystemTime , mnSystemTime , pVideoBuffer->Time);
			}
			
			return VOSF_ERR_NONE;
		
		}
		else
		{
			{
				 voCAutoLock lock(&m_mtDecoder);
				nRC = ReadVideoSample (pOptions);
			}

			if (nRC == VOSF_ERR_NONE)
			{
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

				if (m_pMeidaBuffer == NULL)
				{
					if (m_bVideoYUVBuffer)
					{
						if (m_bVideoYUVCopy)
							m_pMeidaBuffer = m_pMediaBufferOP->create (m_pVideoYUVBuffer,  m_sVideoFormat.Width * m_sVideoFormat.Height * 3 / 2);
						else
							m_pMeidaBuffer = m_pMediaBufferOP->create (m_sVideoDecOutBuf.Buffer[0],  m_sVideoFormat.Width * m_sVideoFormat.Height * 3 / 2);
					}
					else
					{
						m_pMeidaBuffer = m_pMediaBufferOP->create (&m_sVideoDecOutBuf, sizeof (m_sVideoDecOutBuf));
					}

					if (m_pMeidaBuffer == NULL)
						return VOSF_ERR_OUTOF_MEMORY;
					m_pMediaBufferOP->setObserver(m_pMeidaBuffer, this);
				}

				if (m_bVideoYUVBuffer)
					m_pMediaBufferOP->set_range (m_pMeidaBuffer, 0, m_sVideoFormat.Width * m_sVideoFormat.Height * 3 / 2);
				else
					m_pMediaBufferOP->set_range (m_pMeidaBuffer, 0, sizeof (m_sVideoDecOutBuf));	
			}
		}
	}

	if (nRC == VOSF_ERR_NONE)
	{
		m_pMetaDataOP->clear (m_pMediaBufferOP->meta_data (m_pMeidaBuffer));
		m_pMetaDataOP->setInt64 (m_pMediaBufferOP->meta_data (m_pMeidaBuffer), kKeyTime, m_llMediaTime);

		m_pMediaBufferOP->add_ref (m_pMeidaBuffer);
		*ppBuffer = m_pMeidaBuffer;

		mbSeeking = VO_FALSE;
	}

	if(mnLogLevel > 2)
	{
		mnSystemTime = voOS_GetSysTime();
		VOLOGI("Decoder use time : %d   systemtime : %d   sample timestamp : %lld " , mnSystemTime - uSystemTime , mnSystemTime , m_llMediaTime);
	}

	return nRC;

//	void * pMediaBuffer = NULL;
//	m_pMediaSource->read (m_pMediaSource->pMediaSource, &pMediaBuffer, pOptions);
}

bool CMediaDecoderSource::CreateAudioDec (void)
{
	VO_AUDIO_CODINGTYPE nDecType = VO_AUDIO_CodingUnused;
	if (strstr (m_szMIME, "audio/3gpp") != NULL)
	{
		nDecType = VO_AUDIO_CodingAMRNB;
		m_sAudioFormat.Channels = 1;
	}
	else if (strstr (m_szMIME, "audio/amr-wb") != NULL)
		nDecType = VO_AUDIO_CodingAMRWB;
	else if (strstr (m_szMIME, "audio/amr-wbp") != NULL)
		nDecType = VO_AUDIO_CodingAMRWBP;
	else if (strstr (m_szMIME, "audio/mpeg") != NULL)
		nDecType = VO_AUDIO_CodingMP3;
	else if (strstr (m_szMIME, "audio/mp4a-latm") != NULL)
		nDecType = VO_AUDIO_CodingAAC;
	else if (strstr (m_szMIME, "audio/qcelp") != NULL)
	{
		nDecType = VO_AUDIO_CodingQCELP13;
		m_sAudioFormat.Channels = 1;
	}
	else if (strstr (m_szMIME, "audio/evrc") != NULL)
	{
		nDecType = VO_AUDIO_CodingEVRC;
		m_sAudioFormat.Channels = 1;
	}
	else if (strstr (m_szMIME, "audio/flac") != NULL)
		nDecType = VO_AUDIO_CodingFLAC;
	else if (strstr (m_szMIME, "audio/ac3") != NULL)
		nDecType = VO_AUDIO_CodingAC3;
	else if (strstr (m_szMIME, "audio/adpcm") != NULL)
		nDecType = VO_AUDIO_CodingADPCM;
	else if (strstr (m_szMIME, "audio/ogg") != NULL)
		nDecType = VO_AUDIO_CodingOGG;
	else if (strstr (m_szMIME, "audio/wma") != NULL)
		nDecType = VO_AUDIO_CodingWMA;
	else if (strstr (m_szMIME, "audio/real") != NULL)
		nDecType = VO_AUDIO_CodingRA;
	else if (strstr (m_szMIME, "audio/midi") != NULL)
		nDecType = VO_AUDIO_CodingMIDI;
	else if (strstr (m_szMIME, "audio/dra") != NULL)
		nDecType = VO_AUDIO_CodingDRA;
	else if (strstr (m_szMIME, "audio/g729") != NULL)
		nDecType = VO_AUDIO_CodingG729;
	else if (strstr (m_szMIME, "audio/eac3") != NULL)
		nDecType = VO_AUDIO_CodingEAC3;
	else if (strstr (m_szMIME, "audio/ape") != NULL)
		nDecType = VO_AUDIO_CodingAPE;
	else if (strstr (m_szMIME, "audio/flac") != NULL)
		nDecType = VO_AUDIO_CodingALAC;
	else if (strstr (m_szMIME, "audio/dts") != NULL)
		nDecType = VO_AUDIO_CodingDTS;

	if (nDecType == VO_AUDIO_CodingUnused)
	{
		VOLOGE ("It could find the audio codec type.");
		return false;
	}

	if (m_pAudioDec != NULL)
		delete m_pAudioDec;

	m_pAudioDec = new CAudioDecoder (NULL, nDecType, &g_memOP);
	//m_pAudioDec = new CAudioDecoder (NULL , nDecType, &g_memOP);
	if (m_pAudioDec == NULL)
	{
		VOLOGE ("It could not create audio dec class!");
		return false;
	}
	
	VO_U32 nFourCC = getFourCC();
	if(nDecType == VO_AUDIO_CodingAAC&&nFourCC!=0)
		m_pAudioDec->SetFourCC(nFourCC);
	
	int nRC = m_pAudioDec->Init (NULL, 0, &m_sAudioFormat);
	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("Init audio dec error. %08X", nRC);
		return false;
	}
	//read codec config data from meta
	{
		int nType;
		void* meta = m_pMediaSource->getFormat(m_pMediaSource->pMediaSource);
		if(m_pMetaDataOP->findData(meta,kKeyTrkInfo, &nType, 
			(const void**)&m_sAudioDecInBuf.Buffer, (int*)&m_sAudioDecInBuf.Length))
			nRC = m_pAudioDec->SetInputData(&m_sAudioDecInBuf);
	}

	m_nAudioFrameSize = m_sAudioFormat.SampleRate * m_sAudioFormat.Channels * 2;
	if (m_sAudioDecOutBuf.Buffer == NULL)
		m_sAudioDecOutBuf.Buffer = new VO_BYTE[m_nAudioFrameSize];

	return true;
}

bool CMediaDecoderSource::CreateVideoDec (void)
{
	if (strstr (m_szMIME, "video/avc") != NULL)
		mnDecType = VO_VIDEO_CodingH264;
	else if (strstr (m_szMIME, "video/mp4v-es") != NULL)
		mnDecType = VO_VIDEO_CodingMPEG4;
	else if (strstr (m_szMIME, "video/3gpp") != NULL)
		mnDecType = VO_VIDEO_CodingH263;
	else if (strstr (m_szMIME, "video/wmv") != NULL)
		mnDecType = VO_VIDEO_CodingWMV;
	else if (strstr (m_szMIME, "video/rmvb") != NULL)
		mnDecType = VO_VIDEO_CodingRV;
	else if (strstr (m_szMIME, "video/mpeg2") != NULL)
		mnDecType = VO_VIDEO_CodingMPEG2;
	else if (strstr (m_szMIME, "video/s263") != NULL)
		mnDecType = VO_VIDEO_CodingS263;
	else if (strstr (m_szMIME, "video/mjpeg") != NULL)
		mnDecType = VO_VIDEO_CodingMJPEG;
	else if (strstr (m_szMIME, "video/vp6") != NULL)
		mnDecType = VO_VIDEO_CodingVP6;

	if (mnDecType == VO_VIDEO_CodingUnused)
		return false;

	if (m_pVideoDec != NULL)
		delete m_pVideoDec;
	m_pVideoDec = new CVideoDecoder (NULL, mnDecType, &g_memOP);
	if (m_pVideoDec == NULL)
		return false;

	int nRC = m_pVideoDec->Init (NULL , 0 , &m_sVideoFormat);
	if (nRC != VO_ERR_NONE)
		return false;

	UpdateParamete();
	return true;
}

 int	CMediaDecoderSource::UpdateParamete()
 {
	 if(m_nCoreNumber > 1)
	 {
		 VO_U32 nRC = m_pVideoDec->SetParam(VO_PID_COMMON_CPUNUM , &m_nCoreNumber);
		 VOLOGI("Set VO_PID_COMMON_CPUNUM %d return : 0x%x" , m_nCoreNumber , nRC);
	 }


	if(mnDecType == VO_VIDEO_CodingH264)
	{
		//the follow code depend on the data struct is AVCDecoderConfigurationRecord 
		if(mnSeqDataLen < 10)
			return 0;

		VO_BYTE pTempBuf[100];
		memset(pTempBuf , 0 , 100);
		pTempBuf[3] = 1;
		int nDataLen = mnSeqDataLen;
		VO_PBYTE pData = mpSeqData;

		VO_VIDEO_FORMAT  fmtVideo;
		VO_CODECBUFFER inputBuf;
		memset(&inputBuf , 0 , sizeof(VO_CODECBUFFER));

		if(pData[0] != 1)
			return 0;

		nDataLen -= 5;
		pData += 5;

		int nSPS = pData[0] & 31;
		nDataLen -= 1;
		pData += 1;

		for (int i = 0 ; i < nSPS ; i++)
		{
			if(nDataLen < 2)
				return 0;

			int nSPSLen = pData[0];
			nSPSLen = (nSPSLen << 8) + pData[1];
			nDataLen -= 2;
			pData += 2;

			if(nSPSLen > nDataLen)
				return 0;

			memcpy(pTempBuf + 4 , pData , nSPSLen);
			inputBuf.Buffer = pTempBuf;
			inputBuf.Length = nSPSLen + 4;
			inputBuf.Time = 0;

			if(mbDumpData && mhDumpDataFile != NULL)
			{
				fwrite(inputBuf.Buffer , 1 , inputBuf.Length , mhDumpDataFile);
			}

			m_pVideoDec->SetInputData(&inputBuf);
			m_pVideoDec->GetOutputData (&m_sVideoDecOutBuf , &fmtVideo , &m_bMoreVideoBuff);

			nDataLen += nSPSLen;
			pData += nSPSLen;
		}

		if(nDataLen <= 0)
			return 0;

		int nPPS = pData[0];
		nDataLen += 1;
		pData += 1;

		for(int i = 0 ; i < nPPS ; i++)
		{
			if(nDataLen < 2)
				return 0;

			int nPPSLen = pData[0];
			nPPSLen = (nPPSLen << 8) + pData[1];
			nDataLen -= 2;
			pData += 2;

			if(nPPSLen > nDataLen)
				return 0;

			memcpy(pTempBuf + 4 , pData , nPPSLen);
			inputBuf.Buffer = pTempBuf;
			inputBuf.Length = nPPSLen + 4;
			inputBuf.Time = 0;

			if(mbDumpData && mhDumpDataFile != NULL)
			{
				fwrite(inputBuf.Buffer , 1 , inputBuf.Length , mhDumpDataFile);
			}
			m_pVideoDec->SetInputData(&inputBuf);
			m_pVideoDec->GetOutputData (&m_sVideoDecOutBuf , &fmtVideo , &m_bMoreVideoBuff);

			nDataLen += nPPSLen;
			pData += nPPSLen;
		}

#ifdef _SE_
		int param = OPT_ENABLE_DEINTERLACE;
		m_pVideoDec->SetParam(VO_ID_H264_OPT_FLAG,&param);
#endif //_SE_
	}

	return 0;
 }

int CMediaDecoderSource::ReadAudioSample (void * pOptions)
{
	VO_S32 nRC = 0;
	VOSF_READOPTIONTYPE* pReadOption = (VOSF_READOPTIONTYPE*)pOptions;
	if (m_nMediaFrameCount > 0)
	{
		nRC = DecAudioSample ();
		if (nRC >= 0)
		{
			m_llMediaTime += 20000;
			return nRC;
		}
	}

	while(true)
	{
		if (m_pInputBuffer != NULL)
		{
			m_pMediaBufferOP->release (m_pInputBuffer);
			m_pInputBuffer = NULL;
		}
	
		if(pReadOption)
		{
			nRC = m_pMediaSource->read (m_pMediaSource->pMediaSource, &m_pInputBuffer, pReadOption->pOptions);
			pReadOption->clearSeekTo(pReadOption->pOptions);
		}
		else
			nRC = m_pMediaSource->read (m_pMediaSource->pMediaSource, &m_pInputBuffer, NULL);
		
		if(nRC != VO_ERR_NONE)
		{
			if(nRC == ERROR_END_OF_STREAM)
			{
				if(mnLogLevel > 0)
				{
					VOLOGI ("Read at end of track!");
				}

				mbEOS = VO_TRUE;
				return VOSF_ERR_EOS;
			}
			else if(nRC == INFO_FORMAT_CHANGED)
			{
				if(mnLogLevel > 0)
					VOLOGI ("Format is changed!");
			}
			else
			{
				VOLOGE ("Read data from track error %08X!", nRC);
				return VOSF_ERR_FAILED;
			}
		}
		if (m_pInputBuffer == NULL)
		{
			VOLOGE ("Read media source was wrong!");
			return VOSF_ERR_FAILED;
		}

		m_pMetaDataOP->findInt64 (m_pMediaBufferOP->meta_data (m_pInputBuffer), kKeyTime, &m_llMediaTime);

		m_sAudioDecInBuf.Buffer = (VO_PBYTE)m_pMediaBufferOP->data (m_pInputBuffer) + m_pMediaBufferOP->range_offset (m_pInputBuffer);
		m_sAudioDecInBuf.Length = m_pMediaBufferOP->range_length (m_pInputBuffer);
		m_sAudioDecInBuf.Time   = m_llMediaTime;
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

int CMediaDecoderSource::DecAudioSample (void)
{
	if (m_pAudioDec == NULL)
		return VOSF_ERR_WRONG_STATUS;

	VO_AUDIO_FORMAT	fmtAudio;
	m_sAudioDecOutBuf.Length = m_nAudioFrameSize;
	int nRC = m_pAudioDec->GetOutputData(&m_sAudioDecOutBuf , &fmtAudio);

	if(nRC == VOSF_ERR_NONE)
	{
		if(fmtAudio.Channels != m_sAudioFormat.Channels || fmtAudio.SampleBits != m_sAudioFormat.SampleBits || fmtAudio.SampleRate != m_sAudioFormat.SampleRate)
		{
			m_pLastAudioBuff = &m_sAudioDecOutBuf;
			memcpy(&m_sAudioFormat , &fmtAudio , sizeof(m_sAudioFormat));
			if(mnLogLevel > 0)
				VOLOGI ("The audio format was changed to SampleRate %d, Channels %d, Bits %d",  fmtAudio.SampleRate, fmtAudio.Channels,  fmtAudio.SampleBits);
			return VOSF_ERR_FORMAT_CHANGED;
		}

		m_nMediaFrameCount++;

		return VOSF_ERR_NONE;
	}

	return VOSF_ERR_FAILED;
}

int CMediaDecoderSource::ReadVideoSample (void * pOptions)
{

	VO_S32 nRC = VO_ERR_NONE;
	VOSF_READOPTIONTYPE* pReadOption = (VOSF_READOPTIONTYPE*)pOptions;
	
	if (m_nMediaFrameCount && m_bMoreVideoBuff && mbSeeking == VO_FALSE)
	{
		nRC = DecVideoSample ();
		if (nRC >= 0 && nRC != VOSF_ERR_NEED_MOREBUFF)
		{
			m_llMediaTime += 30000;
			return nRC;
		}
	}

	while (mbStop == VO_FALSE)
	{
		voCAutoLock lock(&m_mtSource);
		if (m_pInputBuffer != NULL)
		{
			m_pMediaBufferOP->release (m_pInputBuffer);
			m_pInputBuffer = NULL;
		}

		VO_U32 nStart = 0;
		if(mnLogLevel > 2)
		{
			nStart = voOS_GetSysTime();
		}
		
		if(mbUseBuffer == VO_TRUE && mbSeeking == VO_FALSE)
		{
			if(m_pBufferList == NULL)
				m_pBufferList = new CMediaBufferList(m_pMediaBufferOP, m_pMetaDataOP);
	
			m_pBufferList->arrangeList();

			if(mnLogLevel > 2)
			{
				int nCount = m_pBufferList->GetDropFrameCount();
				if(nCount > 2)
					VOLOGI("Drop Frames : %d " , m_pBufferList->GetDropFrameCount());
			}

			bool bRet;	
			int nTimes = 0;
			do
			{
				if(pReadOption == NULL)
					nRC = m_pMediaSource->read (m_pMediaSource->pMediaSource, &m_pInputBuffer, NULL);
				else
					nRC = m_pMediaSource->read (m_pMediaSource->pMediaSource, &m_pInputBuffer, pReadOption->pOptions);
				if(nRC != VO_ERR_NONE && nRC != INFO_FORMAT_CHANGED)
				{
					//VOLOGI("Set end of stream !!!");
					m_pBufferList->setEOS(true);
					break;
				}

				if(m_pInputBuffer == NULL)
					break;

				bRet = m_pBufferList->addBuffer(m_pInputBuffer);
				m_pMediaBufferOP->release (m_pInputBuffer);
				m_pInputBuffer = NULL;

				if(!bRet)
					break;	

				nTimes ++;

			}while (!m_pBufferList->isBufferReady() && nTimes < 3  && mbStop == VO_FALSE);

			long long llClockTime = GetClockTime();
			if(mbDropFrames == VO_FALSE)
				llClockTime = 0;

			bRet = m_pBufferList->getSample(llClockTime , &m_sVideoDecInBuf);
			if(!bRet)
			{
				mbEOS = VO_TRUE;
				return VOSF_ERR_EOS;
			}

			m_llMediaTime = m_sVideoDecInBuf.Time;
		}
		else
		{
			if(pReadOption == NULL)
			{
				nRC = m_pMediaSource->read (m_pMediaSource->pMediaSource, &m_pInputBuffer, NULL);
			}
			else
			{
				nRC = m_pMediaSource->read (m_pMediaSource->pMediaSource, &m_pInputBuffer, pReadOption->pOptions);
				pReadOption->clearSeekTo(pReadOption->pOptions);
			}

			if(nRC != VO_ERR_NONE)
			{
				if(nRC == ERROR_END_OF_STREAM)
				{
					if(mnLogLevel > 0)
						VOLOGI ("Read at end of track!");

					mbEOS = VO_TRUE;
					return VOSF_ERR_EOS;
				}
				else if(nRC == INFO_FORMAT_CHANGED)
				{
					if(mnLogLevel > 0)
						VOLOGI ("Format is changed!");
				}
				else
				{
					VOLOGE ("Read data from track error %08X!", nRC);
					return VOSF_ERR_FAILED;
				}
			}

			if (m_pInputBuffer == NULL)
				return VOSF_ERR_FAILED;

			m_pMetaDataOP->findInt64 (m_pMediaBufferOP->meta_data (m_pInputBuffer), kKeyTime, &m_llMediaTime);
			
			m_sVideoDecInBuf.Buffer = (VO_PBYTE)m_pMediaBufferOP->data (m_pInputBuffer) + m_pMediaBufferOP->range_offset (m_pInputBuffer);
			m_sVideoDecInBuf.Length = m_pMediaBufferOP->range_length (m_pInputBuffer);
			m_sVideoDecInBuf.Time   = m_llMediaTime;
		}

		if(mbDumpData && mhDumpDataFile != NULL)
		{
			fwrite(m_sVideoDecInBuf.Buffer , 1 , m_sVideoDecInBuf.Length , mhDumpDataFile);
		}

		if(mnLogLevel > 2)
		{
			VOLOGI("Read a sample used time : %04d  samplesize : %06d   timestamp : %08lld" , voOS_GetSysTime() - nStart , m_sVideoDecInBuf.Length , m_sVideoDecInBuf.Time);
		}

		m_sVideoDecInBuf.Time /= 1000;
		nRC = m_pVideoDec->SetInputData(&m_sVideoDecInBuf);
		if(nRC != VO_ERR_NONE)
			continue;

		nRC = DecVideoSample ();
		if ((nRC >= 0  && nRC != VOSF_ERR_NEED_MOREBUFF) || (VO_U32)nRC == VOSF_ERR_FAILED || mbStop == VO_TRUE )
		{
			return nRC;
		}
	}

	return VOSF_ERR_FAILED;
}

int CMediaDecoderSource::DecVideoSample (void)
{
	if (m_pVideoDec == NULL)
		return VOSF_ERR_WRONG_STATUS;

	VO_U32 nStart = 0;
	if(mnLogLevel >  2 )
	{
		nStart = voOS_GetSysTime();
		long long  llClockTime = GetClockTime();
		VOLOGI("m_llMediaTime = %08lld   m_llClockTime = %08lld diff = %lld   systemtime %d " , m_llMediaTime , llClockTime , (llClockTime - m_llMediaTime) / 1000 ,  voOS_GetSysTime());
	}

	if (mbDropFrames == VO_TRUE  && (m_pVideoDec->GetFrameType (NULL) == VO_VIDEO_FRAME_B) && 	!m_pVideoDec->IsRefFrame(NULL)){
	
		if(mnLogLevel > 2)
			VOLOGI("mnWriteIndex - mnReadIndex = %d  GetClockTime() - m_llMediaTime = %lld  mnDrawBFrames = %d m_nDrawFrames = %d " , mnWriteIndex - mnReadIndex , GetClockTime() - m_llMediaTime , mnDrawBFrames , mnDrawFrames);
		if(GetClockTime() > (m_llMediaTime + mnDropAllBFrame) || ( GetClockTime() >  (m_llMediaTime  + 25000) && mnDrawBFrames > 0) || (mnWriteIndex - mnReadIndex <= mnWaterMark && mnDrawBFrames >= 2))
		{
			if(mnLogLevel >  2 )
				VOLOGI("Drop Frame  B frame !!!");

			mnDrawBFrames = 0;
			mnDrawFrames = 0;
			return VO_ERR_NOT_IMPLEMENT;
		}

		mnDrawBFrames ++;
	}

	if(mnLogLevel > 5)
	{
		if(m_pVideoDec->IsRefFrame(NULL))
		{
			VOLOGI("GetFrameType = %d  IsRefFrame : TRUE " , m_pVideoDec->GetFrameType (NULL) );
		}
		else
		{
			VOLOGI("GetFrameType = %d  IsRefFrame : FALSE " , m_pVideoDec->GetFrameType (NULL) );
		}
	}

	//enable or disable deblock
	if(mnDecType == VO_VIDEO_CodingH264)
	{
		VO_S32 nDelayTime = 0;

		if((mBufferCount[0] - mBufferCount[9] > 0 || mBufferCount[9] == 1) && mnWriteIndex > 30)
		{
			if(mnDeInterlace != 0 && mnWriteIndex - mnReadIndex < mnDeInterlace)
			{
				if(mnLogLevel > 3)
				{
					VOLOGI("Disable deinterlace  mnWriteIndex %d   mnReadIndex : %d Diff %d   mnDeInterlace  %d " , mnWriteIndex , mnReadIndex , mnWriteIndex - mnReadIndex ,  mnDeInterlace );
				}
				nDelayTime = 150;
			}
			else if(mnDeblockBufCount != 0 && mnWriteIndex - mnReadIndex < mnDeblockBufCount)
			{
				if(mnLogLevel > 3)
				{
					VOLOGI("Disable deblock mnWriteIndex %d   mnReadIndex : %d Diff %d   mnDeblockBufCount  %d " , mnWriteIndex , mnReadIndex , mnWriteIndex - mnReadIndex ,  mnDeblockBufCount );
				}
				nDelayTime = 110;
			}
			else if(mnDeblockEdge != 0 &&  mnWriteIndex - mnReadIndex < mnDeblockEdge)
			{
				if(mnLogLevel > 3)
					VOLOGI("Disable deblock Edge mnWriteIndex %d   mnReadIndex : %d Diff %d   mnDeblockEdge  %d " , mnWriteIndex , mnReadIndex , mnWriteIndex - mnReadIndex ,  mnDeblockEdge );
				nDelayTime = 90;
			}
		}
	
		m_pVideoDec->SetDelayTime (nDelayTime);
	}

	
	mnDrawFrames ++;

	VO_VIDEO_FORMAT  fmtVideo;
	m_bMoreVideoBuff = VO_FALSE;

	int nRC = m_pVideoDec->GetOutputData (&m_sVideoDecOutBuf , &fmtVideo , &m_bMoreVideoBuff);
	
	if(mnLogLevel > 2)
	{
		VOLOGI("return 0x%08x decode Use %02d  Frame Type : %d   , Cur Frame : %08lld  last timestamp : %08lld  diff : %08lld" , nRC , voOS_GetSysTime() - nStart ,  fmtVideo.Type ,  m_sVideoDecOutBuf.Time , mllLastTime , m_sVideoDecOutBuf.Time - mllLastTime)
	}
	if(nRC == VO_ERR_NONE && m_sVideoDecOutBuf.Buffer[0] != NULL)
	{
		m_sVideoDecOutBuf.Time *= 1000;
		mllLastOrgTime = m_sVideoDecOutBuf.Time ;
		if(fmtVideo.Type == VO_VIDEO_FRAME_B)
		{
			if(m_sVideoDecOutBuf.Time - mllLastTime > 50000)
			{
				m_sVideoDecOutBuf.Time = mllLastTime + 33366;
			}
		}
		else if(fmtVideo.Type == VO_VIDEO_FRAME_P)
		{
			if(m_sVideoDecOutBuf.Time < mllLastTime)
			{
				m_sVideoDecOutBuf.Time = mllLastTime + 33366;
			}
		}

		mllLastTime = m_sVideoDecOutBuf.Time;

		if(fmtVideo.Height != m_sVideoFormat.Height || fmtVideo.Width != m_sVideoFormat.Width)
		{
			m_pLastVideoBuff = &m_sVideoDecOutBuf;

			m_sVideoFormat.Height = fmtVideo.Height;
			m_sVideoFormat.Width  = fmtVideo.Width;
			m_sVideoFormat.Type   = fmtVideo.Type;

			if(mnLogLevel > 0)
			{
				VOLOGI("Video change format : %d %d " , fmtVideo.Width , fmtVideo.Height);
			}

			return VOSF_ERR_FORMAT_CHANGED;
		}

		if (m_nMediaFrameCount == 0 && m_bVideoYUVBuffer)
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

		m_nMediaFrameCount++;

		return VOSF_ERR_NONE;
	}
	else
	{
		if(mnDecType == VO_VIDEO_CodingH264 && (VO_U32)nRC < 0x8000FF39 && (VO_U32)nRC > 0x8000FC18)
		{
			return VOSF_ERR_FAILED;
		}
		else if(nRC < 0 && (VO_U32)nRC != VO_ERR_INPUT_BUFFER_SMALL)
			return VOSF_ERR_FAILED;
		else
			return VOSF_ERR_NEED_MOREBUFF;
	}

	return VOSF_ERR_FAILED;
}

 VO_S32	CMediaDecoderSource::voDecodeProc (VO_PTR pParam)
 {
	 CMediaDecoderSource *pSource = (CMediaDecoderSource *)pParam;
	if(pSource == NULL)
		return -1;

	return pSource->voDecodeLoop();
 }
 
 VO_S32	CMediaDecoderSource::voDecodeLoop()
 {
	 while(mbStop == VO_FALSE)
	 {
		 while(mbSeeking == VO_TRUE && mbStop == VO_FALSE)
		 {
			 voOS_Sleep(2);
		 }

		 VO_S32 nRC = 0;
		 {
			 voCAutoLock lock(&m_mtDecoder);
			 nRC = ReadVideoSample(NULL);
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
			break;

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
			for (int i = 0; i < m_sVideoFormat.Height / 2 ; i ++)
			{
				memcpy (pYUV + m_sVideoFormat.Width * 2 * i, m_sVideoDecOutBuf.Buffer[0] + m_sVideoDecOutBuf.Stride[0] *  2 * i, m_sVideoFormat.Width);
				memcpy (pYUV + m_sVideoFormat.Width * (2 *i + 1), m_sVideoDecOutBuf.Buffer[0] + m_sVideoDecOutBuf.Stride[0] * ( 2 * i + 1), m_sVideoFormat.Width);
			}

			pYUV = mpBufferList[nIndex].Buffer[1];
			for (int i = 0; i < m_sVideoFormat.Height / 4; i++)
			{
				memcpy (pYUV + m_sVideoFormat.Width / 2 *  2 * i, m_sVideoDecOutBuf.Buffer[1] + m_sVideoDecOutBuf.Stride[1] *  2 * i, m_sVideoFormat.Width / 2);
				memcpy (pYUV + m_sVideoFormat.Width / 2 * ( 2 * i + 1), m_sVideoDecOutBuf.Buffer[1] + m_sVideoDecOutBuf.Stride[1] * ( 2 * i + 1), m_sVideoFormat.Width / 2);
			}
			pYUV = mpBufferList[nIndex].Buffer[2];
			for (int i = 0; i < m_sVideoFormat.Height / 4; i++)
			{
				memcpy (pYUV + m_sVideoFormat.Width / 2 *  2 * i, m_sVideoDecOutBuf.Buffer[2] + m_sVideoDecOutBuf.Stride[2] *  2 * i, m_sVideoFormat.Width / 2);
				memcpy (pYUV + m_sVideoFormat.Width / 2 * ( 2 * i + 1), m_sVideoDecOutBuf.Buffer[2] + m_sVideoDecOutBuf.Stride[2] * ( 2 * i + 1), m_sVideoFormat.Width / 2);
			}
			
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

 void *	 CMediaDecoderSource::FindMediaBuffer(void *pBuf)
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

 bool	CMediaDecoderSource::AddMediaBuffer(void *pBuf)
 {
	 for(unsigned int i = 0 ; i < mnBufferCount ; i++)
	 {
		 if(mppMediaBufList[i] == NULL)
		 {
			 mppMediaBufList[i] = pBuf;
			 return true;
		 }
	 }

	 VOLOGE("Add MediaBuffer count more then mnBufferCount");
	 return false;
 }

 bool	CMediaDecoderSource::ReleaseMediaBuf()
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

 void		CMediaDecoderSource::ReleaseBufList()
 {
	 if(m_bVideoYUVBuffer == VO_FALSE || mpBufferList == NULL)
		 return;

	 if(mpMemVideoOP != NULL && mbUseVideoMemOP == VO_TRUE && mpBufferList != NULL)
	 {
		 if(mnLogLevel > 0)
			 VOLOGI("Release out memory operator allocate buffer !!!");
		 mpMemVideoOP->Uninit(0);
		 mpBufferList = NULL;
		 mbUseVideoMemOP = VO_FALSE;
		 return ;
	 }

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

 VO_U32 CMediaDecoderSource::getFourCC()
 {
	 VO_U32 nObjType=0;
	 void* meta = m_pMediaSource->getFormat(m_pMediaSource->pMediaSource);

	 if(m_pMetaDataOP->findInt32(meta, kKeyObjTypeId, (int*)&nObjType))
	 {
		 if((0x40 == nObjType) || (0x67 == nObjType))
			 return 0xA106;// AudioFlag_ISOMPEG4_AAC;		 
	 }
	 return 0;	 	 
 }

 long long	CMediaDecoderSource::GetClockTime()
 {
	return m_llReferenceTime + (voOS_GetSysTime() - m_llSystemTime) * 1000;
 }

 void	CMediaDecoderSource::UpdateClockTime(long long llClockTime)
 {
	m_llReferenceTime = llClockTime;
	m_llSystemTime = voOS_GetSysTime();
 }
