#include "COSBasePlayer.h"
#include "voOSFunc.h"

#define LOG_TAG "COSBasePlayer"
#include "voLog.h"

COSBasePlayer::COSBasePlayer()
	:m_pOSLibFunc(NULL)	
	,m_pOSReadBufFunc(NULL)
	,m_pVideoSubtile(NULL)
	,m_bSeeking(VO_FALSE)
	,m_bCCParser(VO_FALSE)
	,m_hVideoParser(NULL)
#if defined(_LINUX) && !defined(__ODM__)
	,m_JavaVM(NULL)
	,m_jOSmpEng(0)
	,m_JniUtil(NULL)
#endif
	,m_nPrefixEnable(0)
	,m_nSuffixEnable(0)
	//,m_nLocalFile(0)
	,m_nChanging(0)
	,m_nImageUpdated(0)
	,m_nH264SEIInfo(0)
	,m_nLastSendtime(0)
	,m_nSEICallBackTime(-1)
	,m_pCSEIInfo(NULL)
	,m_nSEICount(100)
	,m_nSubLangGot(0)
	,m_nVideoChanged(0)
	,m_nSaveVideoBuffer(0)
	,m_nSaveAudioBuffer(0)
	,m_nLogAudioLevel(0)
	,m_nLogVideoLevel(0)
	,m_pAudioDumpFile(0)
	,m_pVideoDumpFile(0)
	,m_nDumpAudioLevel(0)
	,m_nDumpVideoLevel(0)
	,m_nStartSystemTime(0)
	,m_hAudioDumpFile(0)
	,m_hVideoDumpFile(0)
    ,m_pModuleVersion(NULL)
    ,m_nAudioVideoOnly(0)
{
	memset(&m_cbVOLOG, 0, sizeof(VO_LOG_PRINT_CB));
	memset(&m_sInVideoFormat, 0, sizeof(VOOSMP_BUFFER_FORMAT));
	memset(&m_sInAudioFormat, 0, sizeof(VOOSMP_BUFFER_FORMAT));
	memset(&m_sVideoParserAPI, 0, sizeof(VO_VIDEO_PARSERAPI));
	memset(&m_sSubLangInfo, 0, sizeof(VOOSMP_SUBTITLE_LANGUAGE_INFO));

	memset(m_cPrefix, 0, 32);
	memset(m_cSuffix, 0, 32);
	//memset(m_cLocalFilePath, 0, 2*1024);
	memset(m_szPathLib, 0, sizeof(VO_TCHAR)*1024);

	m_sInAudioFormat.nCodec = VOOSMP_AUDIO_CodingAAC;
	m_sInAudioFormat.sFormat.audio.SampleRate = 44100;
	m_sInAudioFormat.sFormat.audio.Channels = 2;
	m_sInAudioFormat.sFormat.audio.SampleBits = 16;

	m_sInVideoFormat.nCodec = VOOSMP_VIDEO_CodingH264;
	m_sInVideoFormat.sFormat.video.Width = 320;
	m_sInVideoFormat.sFormat.video.Height = 240;

	memset(&m_sH264SPS, 0, sizeof(parser_out_sps_t));
	memset(&m_sBufferVideoSaved, 0, sizeof(VOOSMP_BUFFERTYPE));
	memset(&m_sBufferAudioSaved, 0, sizeof(VOOSMP_BUFFERTYPE));

	OpenDebugConfig();

	if (g_fileOP.Open == NULL)
		cmnFileFillPointer ();
	m_pFileOP = &g_fileOP;
}

COSBasePlayer::~COSBasePlayer()
{
	if(m_pVideoSubtile)
	{
		delete m_pVideoSubtile;
	}
#if defined(_LINUX) && !defined(__ODM__)
	if(m_JniUtil)
		delete m_JniUtil;
#endif

	Uninit();

	if(m_hAudioDumpFile && m_pFileOP)
	{
		m_pFileOP->Close(m_hAudioDumpFile);
	}

	if(m_hVideoDumpFile && m_pFileOP)
	{
		m_pFileOP->Close (m_hVideoDumpFile);
	}
}

int	COSBasePlayer::Init()
{
	VO_FILE_SOURCE filSource;
	memset (&filSource, 0, sizeof (VO_FILE_SOURCE));
	filSource.nFlag = VO_FILE_TYPE_NAME;
	filSource.nMode = VO_FILE_WRITE_ONLY;

	if(m_pAudioDumpFile && m_pFileOP)
	{
#ifdef _WIN32			
		VO_TCHAR szFile[1024];
		memset (szFile, 0, sizeof (szFile));
		MultiByteToWideChar (CP_ACP, 0, m_pAudioDumpFile, -1, szFile, sizeof (szFile)); 
		filSource.pSource = szFile;		
#elif defined _IOS
        char szFile[1024];
        voOS_GetAppFolder(szFile, 1024);
        vostrcat(szFile, m_pAudioDumpFile);
		filSource.pSource = szFile;
#else
		filSource.pSource = m_pAudioDumpFile;
#endif
		m_hAudioDumpFile = m_pFileOP->Open (&filSource);
	}

	if(m_pVideoDumpFile && m_pFileOP)
	{
#ifdef _WIN32
		VO_TCHAR szFile[1024];
		memset (szFile, 0, sizeof (szFile));
		MultiByteToWideChar (CP_ACP, 0, m_pVideoDumpFile, -1, szFile, sizeof (szFile)); 
		filSource.pSource = szFile;	
#elif defined _IOS
        char szFile[1024];
        voOS_GetAppFolder(szFile, 1024);
        vostrcat(szFile, m_pVideoDumpFile);
		filSource.pSource = szFile;
#else
		filSource.pSource = m_pVideoDumpFile;
#endif
		m_hVideoDumpFile = m_pFileOP->Open (&filSource);
	}

	return 0;
}

int	COSBasePlayer::Uninit()
{
	if(m_pVideoSubtile)
	{
		delete m_pVideoSubtile;
		m_pVideoSubtile = NULL;
	}

	if(m_pCSEIInfo)
	{
		delete m_pCSEIInfo;
		m_pCSEIInfo = NULL;
	}

	if(m_hVideoParser != NULL && m_sVideoParserAPI.Uninit != NULL)
	{
		m_sVideoParserAPI.Uninit(m_hVideoParser);
		m_hVideoParser = NULL;
		memset(&m_sVideoParserAPI, 0, sizeof(VO_VIDEO_PARSERAPI));
	}

	if(m_sInVideoFormat.pHeadData)
	{
		delete []m_sInVideoFormat.pHeadData;
		m_sInVideoFormat.pHeadData = NULL;
		m_sInVideoFormat.nHeadDataLen = 0;
	}

	if(m_sBufferVideoSaved.pBuffer)
	{
		delete []m_sBufferVideoSaved.pBuffer;
		m_sBufferVideoSaved.pBuffer = NULL;
	}

	if(m_sBufferAudioSaved.pBuffer)
	{
		delete []m_sBufferAudioSaved.pBuffer;
		m_sBufferAudioSaved.pBuffer = NULL;
	}

	m_nPrefixEnable = 0;
	m_nSuffixEnable = 0;
	//m_nLocalFile = 0;

	ClearLanguage();
    
    if(m_pModuleVersion)
    {
        delete  m_pModuleVersion;
        m_pModuleVersion = NULL;
    }

	return 0;
}

int COSBasePlayer::OpenDebugConfig()
{
	VO_TCHAR	szCfgFile[1024];	

	if(!voOS_EnableDebugMode(0))
	{
		return 0;
	}

#ifdef LINUX
	vostrcpy (szCfgFile, _T("/data/local/tmp/OnStreamPlayer/osmpEngine.cfg"));
#else
	voOS_GetAppFolder (szCfgFile, 1024);
	vostrcat (szCfgFile, _T("osmpEngine.cfg"));
#endif // LINUX

	if (!m_cfgSource.Open (szCfgFile))
	{
		return 0;
	}

	m_nLogAudioLevel = m_cfgSource.GetItemValue("OSMP_ENGINE", "LogAudioLevel", m_nLogAudioLevel);
	m_nLogVideoLevel = m_cfgSource.GetItemValue("OSMP_ENGINE", "LogVideoLevel", m_nLogVideoLevel);

	m_pAudioDumpFile = m_cfgSource.GetItemText("OSMP_ENGINE", "AudioDumpFile", m_pAudioDumpFile);
	m_pVideoDumpFile = m_cfgSource.GetItemText("OSMP_ENGINE", "VideoDumpFile", m_pVideoDumpFile);
	
	m_nDumpAudioLevel = m_cfgSource.GetItemValue("OSMP_ENGINE", "DumpAudioLevel", m_nDumpAudioLevel);
	m_nDumpVideoLevel = m_cfgSource.GetItemValue("OSMP_ENGINE", "DumpVideoLevel", m_nDumpVideoLevel);
	
	return 0;
}


int	COSBasePlayer::SetDataSource (void * pSource, int nFlag)
{
	CheckingStatus(100);

	if(m_nChanging)
		return VOOSMP_ERR_Status;
	return 0;
}

int	COSBasePlayer::Run (void)
{
	CheckingStatus(100);

	if(m_nChanging)
		return VOOSMP_ERR_Status;

	return 0;
}

int	COSBasePlayer::Pause (void)
{
	CheckingStatus(100);

	if(m_nChanging)
		return VOOSMP_ERR_Status;

	return 0;
}
	
int	COSBasePlayer::Stop (void)
{
	CheckingStatus(500);

	if(m_nChanging)
		return VOOSMP_ERR_Status;

	if(m_pVideoSubtile)
	{
		delete m_pVideoSubtile;
		m_pVideoSubtile = NULL;
	}

	if(m_hVideoParser != NULL && m_sVideoParserAPI.Uninit != NULL)
	{
		m_sVideoParserAPI.Uninit(m_hVideoParser);
		m_hVideoParser = NULL;
		memset(&m_sVideoParserAPI, 0, sizeof(VO_VIDEO_PARSERAPI));
	}

	m_bSeeking = VO_FALSE;

	return 0;
}

int	COSBasePlayer::Close (void)
{
    return Stop();
}

int	COSBasePlayer::Flush (void)
{
	if(m_nChanging)
		return VOOSMP_ERR_Status;

	if(m_pVideoSubtile)
		m_pVideoSubtile->Flush ();

	return 0;
}

int	COSBasePlayer::GetStatus (int * pStatus)
{
	return 0;
}
	
int	COSBasePlayer::GetDuration (int * pDuration)
{
	return 0;
}

int	COSBasePlayer::SetView(void* pView)
{
	return 0;
}


int	COSBasePlayer::SetJavaVM(void *pJavaVM, void* obj)
{
	return 0;
}

void*	COSBasePlayer::GetJavaObj()
{
	return 0;
}

int	COSBasePlayer::GetPos (int * pCurPos)
{
	return 0;
}

int	COSBasePlayer::SetPos (int nCurPos)
{
	if(m_pVideoSubtile && m_bCCParser)
	{
		m_pVideoSubtile->Flush ();
		m_nImageUpdated = 0;
	}

	if(m_pCSEIInfo)
	{
		m_pCSEIInfo->SetPos(nCurPos);
	}

	m_bSeeking = VO_TRUE;

	if(m_nSaveVideoBuffer)
		m_nSaveVideoBuffer = 0;

	if(m_nSaveAudioBuffer)
		m_nSaveAudioBuffer = 0;

	return 0;
}
 
int	COSBasePlayer::GetParam (int nID, void * pValue)
{
    if(nID == VOOSMP_PID_MODULE_VERSION)
    {        
        if(!m_pModuleVersion)
        {
            m_pModuleVersion = new CModuleVersion;
            m_pModuleVersion->SetLibOperator((VO_LIB_OPERATOR *)m_pOSLibFunc);
            m_pModuleVersion->SetWorkPath((TCHAR *)m_szPathLib);
            m_pModuleVersion->LoadLib(NULL);
        }

        return m_pModuleVersion->GetModuleVersion((VOOSMP_MODULE_VERSION*)pValue);
    }
	else if (nID == VOOSMP_PID_GET_ONSTREAMSRC_HANDLE)
	{
		return VOOSMP_ERR_ParamID;
	}
    
	return 0;
}
	
int	COSBasePlayer::SetParam (int nID, void * pValue)
{
	if(nID == VOOSMP_PID_COMMON_CCPARSER)
	{
		if(pValue)
			m_bCCParser = *((VO_BOOL *)pValue);
		
		if(m_pVideoSubtile)
		{
			if(m_bCCParser == VO_TRUE && m_sInVideoFormat.nHeadDataLen > 0 && m_sInVideoFormat.pHeadData != NULL)
				m_pVideoSubtile->Init((unsigned char *)m_sInVideoFormat.pHeadData, m_sInVideoFormat.nHeadDataLen, voSubtitleType_ClosedCaption);

			m_pVideoSubtile->Enable(m_bCCParser);
		}

		return VOOSMP_ERR_None;
	}
	else if(nID == VOOSMP_PID_LIB_NAME_PREFIX)
	{
		if(pValue == NULL)
			return VOOSMP_ERR_Pointer;

		m_nPrefixEnable = 1;
		strcpy(m_cPrefix, (char *)pValue);
		return VOOSMP_ERR_None;
	}
	else if(nID == VOOSMP_PID_LIB_NAME_SUFFIX)
	{
		if(pValue == NULL)
			return VOOSMP_ERR_Pointer;

		m_nSuffixEnable = 1;
		strcpy(m_cSuffix, (char *)pValue);
		return VOOSMP_ERR_None;
	}
	else if(nID == VOOSMP_PID_RETRIEVE_SEI_INFO)
	{
		if(pValue == NULL)
			return VOOSMP_ERR_Pointer;

		if(*((int *)pValue) & VOOSMP_FLAG_SEI_PIC_TIMING)
			m_nH264SEIInfo |= VO_SEI_PIC_TIMING;
		else if(*((int *)pValue) & VOOSMP_FLAG_SEI_USER_DATA_UNREGISTERED)
			m_nH264SEIInfo |= VO_SEI_USER_DATA_UNREGISTERED;

		VOLOGI("@@@### VOOSMP_PID_RETRIEVE_SEI_INFO m_nH264SEIInfo = %d", m_nH264SEIInfo);
		if(m_pCSEIInfo)
		{
			m_pCSEIInfo->Enable(m_nH264SEIInfo);
		}

		return VOOSMP_ERR_None;
	}
	else if(nID == VOOSMP_PID_SEI_EVENT_INTERVAL)
	{
		if(pValue == NULL)
			return VOOSMP_ERR_Pointer;

		m_nSEICallBackTime = *((int *)pValue);
		VOLOGI("@@@### set m_nSEICallBackTime = %d", m_nSEICallBackTime);
		return VOOSMP_ERR_None;
	}
	//else if(nID == VOOSMP_PID_SOURCE_FILENAME)
	//{
	//	if(pValue == NULL)
	//		return VOOSMP_ERR_Pointer;

	//	char* szURL = (char*)pValue;

	//	if (!strncmp (szURL, "RTSP://", 6) || !strncmp (szURL, "rtsp://", 6) || 
	//		!strncmp (szURL, "MMS://", 5) || !strncmp (szURL, "mms://", 5))
	//	{
	//		return VOOSMP_ERR_None;
	//	}
	//	else if (!strncmp (szURL, "HTTP://", 6) || !strncmp (szURL, "http://", 6) 
	//			|| !strncmp (szURL, "HTTPS://", 7) || !strncmp (szURL, "https://", 7))
	//	{
	//		return VOOSMP_ERR_None;
	//	}

	//	m_nLocalFile = 1;
	//	strcpy(m_cLocalFilePath, (char *)szURL);
	//	return VOOSMP_ERR_None;
	//}

	return VOOSMP_ERR_Implement;
}
	
int	COSBasePlayer::doReadAudio(VOOSMP_BUFFERTYPE * pBuffer)
{
	if (!pBuffer)
		return VOOSMP_ERR_Pointer;

	int nRC = VOOSMP_ERR_None;

	if(m_pOSReadBufFunc == NULL|| m_pOSReadBufFunc->ReadAudio == NULL)
		return VOOSMP_ERR_Implement;
	
	if(m_nSaveAudioBuffer)
	{
		m_nSaveAudioBuffer = 0;
		memcpy(pBuffer, &m_sBufferAudioSaved, sizeof(VOOSMP_BUFFERTYPE));
	}
	else
	{
		nRC = m_pOSReadBufFunc->ReadAudio(m_pOSReadBufFunc->pUserData, pBuffer);
		if (nRC == VOOSMP_ERR_Audio_No_Now)
			m_nAudioVideoOnly = 2;
		else if (nRC == VOOSMP_ERR_Video_No_Now)
			m_nAudioVideoOnly = 1;

		DumpDebugInfo(VOOSMP_SS_AUDIO, nRC, pBuffer);
	}

	if(m_nVideoChanged)
	{
		m_nVideoChanged = 0;
		if(!(pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_PROGRAM) && !(pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_FORMAT))
			pBuffer->nFlag |= VOOSMP_FLAG_BUFFER_NEW_FORMAT;
	}

	if((pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_PROGRAM) || (pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_FORMAT))
	{
		if(pBuffer->pData)
		{
			VOOSMP_BUFFER_FORMAT* pFormat = (VOOSMP_BUFFER_FORMAT *)pBuffer->pData;
			if(pFormat->nStreamtype == VOOSMP_SS_AUDIO)
			{
				memcpy(&m_sInAudioFormat, pBuffer->pData, sizeof(VOOSMP_BUFFER_FORMAT));
			}
			else if(pFormat->nStreamtype == VOOSMP_SS_SUBTITLE)
			{
				if(pFormat->nFourCC == VOOSMP_SS_IMAGE)
				{
					HandleImageSub(pBuffer, voSubtitleType_ExternImage, nRC);			

					return VOOSMP_ERR_Retry;
				}
			}
		}
		else
		{
			pBuffer->pData = &m_sInAudioFormat;
		}

		if(pBuffer->pBuffer && pBuffer->nSize > 0)
		{
			m_sBufferAudioSaved.llTime = pBuffer->llTime;
			m_sBufferAudioSaved.nDuration = pBuffer->nDuration;
			m_sBufferAudioSaved.pData = NULL;
			m_sBufferAudioSaved.nFlag = pBuffer->nFlag & ~(VOOSMP_FLAG_BUFFER_UNKNOWN | VOOSMP_FLAG_BUFFER_NEW_FORMAT | VOOSMP_FLAG_BUFFER_NEW_PROGRAM);
			m_sBufferAudioSaved.nSize = pBuffer->nSize;

			if(m_sBufferAudioSaved.pBuffer)
			{
				delete []m_sBufferAudioSaved.pBuffer;
				m_sBufferAudioSaved.pBuffer = NULL;
			}

			m_sBufferAudioSaved.pBuffer = new unsigned char[pBuffer->nSize];
			memcpy(m_sBufferAudioSaved.pBuffer, pBuffer->pBuffer, pBuffer->nSize);

			if(pBuffer->nSize > 7 && m_sInAudioFormat.nCodec == VOOSMP_AUDIO_CodingAAC)
			{
				unsigned char *buf = pBuffer->pBuffer;
				int	sampleIndex = (buf[2] >> 2) &0xF;
				int	channel = ((buf[2]&0x01) << 2) | (buf[3] >> 6);
				
				const int AACsampRateTab[12] = {
					96000, 88200, 64000, 48000, 44100, 32000, 
					24000, 22050, 16000, 12000, 11025,  8000
				};

				m_sInAudioFormat.sFormat.audio.Channels = channel;
				m_sInAudioFormat.sFormat.audio.SampleRate = AACsampRateTab[sampleIndex];
			}

			pBuffer->nSize = 0;
			pBuffer->pBuffer = NULL;
			pBuffer->nFlag &= (VOOSMP_FLAG_BUFFER_NEW_FORMAT | VOOSMP_FLAG_BUFFER_NEW_PROGRAM);

			//VOLOGI("pBuffer->nSize %d, pBuffer->pBuffer %x, pBuffer->nFlag %x", pBuffer->nSize, pBuffer->pBuffer, pBuffer->nFlag);
			//VOLOGI("m_sBufferAudioSaved.nSize %d, m_sBufferAudioSaved.pBuffer %x, m_sBufferAudioSaved.nFlag %x", m_sBufferAudioSaved.nSize, m_sBufferAudioSaved.pBuffer, m_sBufferAudioSaved.nFlag);

			m_nSaveAudioBuffer = 1;	 
		}
	}

	CallBackImage();

	return nRC;	
}

int	COSBasePlayer::doSendBuffer (int nSSType, VOOSMP_BUFFERTYPE * pBuffer)
{
	return 0;
}

int	COSBasePlayer::doReadVideo(VOOSMP_BUFFERTYPE * pBuffer)
{
	int nRC = VOOSMP_ERR_None;

	if(m_pOSReadBufFunc == NULL|| m_pOSReadBufFunc->ReadVideo == NULL)
		return VOOSMP_ERR_Implement;

	if(m_nSaveVideoBuffer)
	{
		m_nSaveVideoBuffer = 0;
		memcpy(pBuffer, &m_sBufferVideoSaved, sizeof(VOOSMP_BUFFERTYPE));
	}
	else
	{
		nRC = m_pOSReadBufFunc->ReadVideo(m_pOSReadBufFunc->pUserData, pBuffer);
		if (nRC == VOOSMP_ERR_Audio_No_Now)
			m_nAudioVideoOnly = 2;
		else if (nRC == VOOSMP_ERR_Video_No_Now)
			m_nAudioVideoOnly = 1;

		DumpDebugInfo(VOOSMP_SS_VIDEO, nRC, pBuffer);
	}

	if((pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_PROGRAM) || (pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_FORMAT))
	{
		if(pBuffer->pData)
		{
			VOOSMP_BUFFER_FORMAT* pFormat = (VOOSMP_BUFFER_FORMAT *)pBuffer->pData;
			if(pFormat->nStreamtype == VOOSMP_SS_VIDEO)
			{
				m_nAudioVideoOnly = 0;

			        if(NULL != m_hVideoParser && NULL != m_sVideoParserAPI.Uninit)
			        {
			          m_sVideoParserAPI.Uninit(m_hVideoParser);
			          m_hVideoParser = NULL;         
			          memset(&m_sVideoParserAPI, 0, sizeof(VO_VIDEO_PARSERAPI));
			        }

				m_sInVideoFormat.nCodec = pFormat->nCodec;
				m_sInVideoFormat.nFourCC = pFormat->nFourCC;
				m_sInVideoFormat.nStreamtype = pFormat->nStreamtype;
				memcpy(&m_sInVideoFormat.sFormat.video, &pFormat->sFormat.video, sizeof(VOOSMP_VIDEO_FORMAT));
				
				if(m_sInVideoFormat.pHeadData)
				{
					delete []m_sInVideoFormat.pHeadData;
					m_sInVideoFormat.pHeadData = NULL;
					m_sInVideoFormat.nHeadDataLen = 0;
				}

				if(pBuffer->nFlag & VOOSMP_FLAG_BUFFER_UNKNOWN || m_nH264SEIInfo)
				{
					int nRRC;

					if(m_hVideoParser == NULL || m_sVideoParserAPI.Init == NULL)
					{
						nRRC = voGetVideoParserAPI (&m_sVideoParserAPI, VO_VIDEO_CodingH264);
						if(nRRC == 0)
						{
							nRRC = m_sVideoParserAPI.Init(&m_hVideoParser);
						}
					}

					if(m_hVideoParser && m_sVideoParserAPI.Init)
					{
						VO_CODECBUFFER sCodeBuffer;

						memset(&sCodeBuffer, 0, sizeof(VO_CODECBUFFER));

						sCodeBuffer.Buffer = (VO_PBYTE)pFormat->pHeadData;
						sCodeBuffer.Length = pFormat->nHeadDataLen;
						sCodeBuffer.Time = pBuffer->llTime;

						nRRC = m_sVideoParserAPI.Process(m_hVideoParser, &sCodeBuffer);

						if(nRRC >= 0 && (pBuffer->nFlag & VOOSMP_FLAG_BUFFER_UNKNOWN))
						{
							parser_out_sps_t		sH264SPS;
							memset(&sH264SPS, 0, sizeof(parser_out_sps_t));

							nRRC = m_sVideoParserAPI.GetParam(m_hVideoParser, VO_PID_VIDEOPARSER_SPS, &sH264SPS);

							if(nRRC == 0)
							{
								VO_S32 nWidth=0,nHeight =0;
								nRRC = m_sVideoParserAPI.GetParam(m_hVideoParser, VO_PID_VIDEOPARSER_WIDTH, &nWidth);
								nRRC |= m_sVideoParserAPI.GetParam(m_hVideoParser, VO_PID_VIDEOPARSER_HEIGHT, &nHeight);

								m_sInVideoFormat.sFormat.video.Width = nWidth;
								m_sInVideoFormat.sFormat.video.Height = nHeight;

								CopySPS(&sH264SPS, &m_sH264SPS);
							}
						}	
					}
				}

				m_sInVideoFormat.pHeadData = new char[pFormat->nHeadDataLen];
				memcpy(m_sInVideoFormat.pHeadData, pFormat->pHeadData, pFormat->nHeadDataLen);
				m_sInVideoFormat.nHeadDataLen = pFormat->nHeadDataLen;
			}
			else if(pFormat->nStreamtype == VOOSMP_SS_SUBTITLE)
			{
				if(pFormat->nFourCC == VOOSMP_SS_IMAGE)
				{
					HandleImageSub(pBuffer, voSubtitleType_ExternImage, nRC);

					return VOOSMP_ERR_Retry;
				}
				else 
				{
					HandleSubtitle(pBuffer, voSubtitleType_ExternText, nRC);
					return VOOSMP_ERR_Retry;
				}
			}
		}

		//if(m_nLocalFile)
		//	HandleSubtitle(pBuffer, voSubtitleType_ExternFile, nRC);
	}

	if(m_sInVideoFormat.nCodec == VOOSMP_VIDEO_CodingH264)
	{
		if(nRC == VOOSMP_ERR_None && ((pBuffer->nFlag & VOOSMP_FLAG_BUFFER_UNKNOWN) || m_nH264SEIInfo))
		{
			HandleH264(pBuffer);
		}
	}

	if(m_bCCParser && m_sInVideoFormat.nCodec == VOOSMP_VIDEO_CodingH264 && (nRC == VOOSMP_ERR_None || nRC == VOOSMP_ERR_EOS))
	{
		HandleSubtitle(pBuffer, voSubtitleType_ClosedCaption, nRC);
	}
		
	//if(pBuffer->nFlag&VOOSMP_FLAG_BUFFER_DELAY_TO_DROP)
	//	nRC = VOOSMP_ERR_Retry;

	return nRC;	
}

int	COSBasePlayer::CallBackImage()
{
	if(m_pVideoSubtile == NULL)	
		return 0;
	
	int nImage = 0;
	int nRC = m_pVideoSubtile->GetParam(VOSUB_GETIMAGE_NUMBER, &nImage);

	if(m_nImageUpdated > 0)
		m_nImageUpdated--;

	if(nImage > 0 && m_nImageUpdated == 0)
	{
		int PlayTime = 0;

		nRC = GetPos(&PlayTime);

		voSubtitleInfo nImageSub;
		memset(&nImageSub, 0, sizeof(voSubtitleInfo));

		if(PlayTime > 10)
			nImageSub.nTimeStamp = PlayTime - 10;
		else
			nImageSub.nTimeStamp = PlayTime;
		nRC = m_pVideoSubtile->GetOutputImage(&nImageSub);
		if(nRC == 0 && m_nAudioVideoOnly == 1)
		{
			voCAutoLock lock( &m_EventLock );
			int MetaType = VOOSMP_SS_IMAGE;
			voSubtitleInfo* pImageSub = &nImageSub;
			VOLOGI("@@@### Send ID3 picture event to APP");
			HandleEvent(VOOSMP_CB_Metadata_Arrive, &MetaType, &pImageSub);
			m_nImageUpdated = 20;
		}
	}

	return 0;
}

int	COSBasePlayer::ChangEventIDEvent (unsigned int nID, void * pParam1, void * pParam2)
{
	return 0;
}

int	COSBasePlayer::HandleEvent (int nID, void * pParam1, void * pParam2)
{
	return 0;
}

int	COSBasePlayer::GetSubtileSample (voSubtitleInfo * pSample)
{
	//if(m_bCCParser == VO_FALSE)
	//	return VOOSMP_ERR_Retry;

	voCAutoLock lock( &m_Lock );

	if(m_pVideoSubtile && !m_bSeeking)
	{
		int nRC = m_pVideoSubtile->GetOutputData(pSample);

		if(nRC == VO_RET_SOURCE2_OK)
		{
			return VOOSMP_ERR_None;
		}
		else 
			return VOOSMP_ERR_Retry; 
	}
	else 
	{
		return VOOSMP_ERR_Retry;
	}
}

int	COSBasePlayer::ClearLanguage()
{
	if(m_sSubLangInfo.nLanguageCount > 0 && m_sSubLangInfo.ppLanguage)
	{
		int n;

		for(n = 0; n < m_sSubLangInfo.nLanguageCount; n++)
		{
			if(m_sSubLangInfo.ppLanguage[n])
			{
				delete m_sSubLangInfo.ppLanguage[n];
				m_sSubLangInfo.ppLanguage[n] = NULL;
			}
		}

		delete []m_sSubLangInfo.ppLanguage;
		m_sSubLangInfo.ppLanguage = NULL;

		m_sSubLangInfo.nLanguageCount = 0;
		m_nSubLangGot = 0;
	}

	return 0;
}

int	COSBasePlayer::GetSubLangNum(int *pNum)
{
	int nRC = VOOSMP_ERR_Retry;
	if(m_pVideoSubtile)
	{
		nRC = m_pVideoSubtile->GetLanguageNum((VO_U32 *)pNum);
		if(nRC)
			return VOOSMP_ERR_Retry;

		ClearLanguage();

		if(*pNum > 0)
		{
			m_sSubLangInfo.nLanguageCount = *pNum;

			m_sSubLangInfo.ppLanguage = new VOOSMP_SUBTITLE_LANGUAGE*[m_sSubLangInfo.nLanguageCount];

			memset(m_sSubLangInfo.ppLanguage, 0, sizeof(VOOSMP_SUBTITLE_LANGUAGE*) * m_sSubLangInfo.nLanguageCount);

			int i = 0;				
			VO_CAPTION_LANGUAGE *pVoCC = NULL;

			for(i = 0; i < m_sSubLangInfo.nLanguageCount; i++)
			{
				nRC = m_pVideoSubtile->GetLanguageItem(i, &pVoCC);
				if(nRC)
					return VOOSMP_ERR_Retry;

				m_sSubLangInfo.ppLanguage[i] = new VOOSMP_SUBTITLE_LANGUAGE();
				memset(m_sSubLangInfo.ppLanguage[i], 0, sizeof(VOOSMP_SUBTITLE_LANGUAGE));

				strcpy((char *)m_sSubLangInfo.ppLanguage[i]->szLangName, pVoCC->chLangName);
				m_sSubLangInfo.ppLanguage[i]->nReserved2 = (int)pVoCC->pReserved;
			}

			m_nSubLangGot = 1;
		}

		return nRC;
	}

	return VOOSMP_ERR_Retry;
}

int	COSBasePlayer::GetSubLangItem(int Index, VOOSMP_SUBTITLE_LANGUAGE** pSubLangItem)
{
	int nRC = VOOSMP_ERR_Retry;
	if(m_pVideoSubtile && m_nSubLangGot)
	{
		*pSubLangItem = m_sSubLangInfo.ppLanguage[Index];

		return VOOSMP_ERR_None;
	}

	return nRC;
}

int	COSBasePlayer::GetSubLangInfo(VOOSMP_SUBTITLE_LANGUAGE_INFO** pSubLangInfo)
{
	int nRC = VOOSMP_ERR_Retry;
	int nNum = 0;

	nRC = GetSubLangNum(&nNum);
	if(nRC) return nRC;
	
	*pSubLangInfo = &m_sSubLangInfo;

	return nRC;
}

int	COSBasePlayer::SelectLanguage(int Index)
{
	int nRC = VOOSMP_ERR_Retry;
	if(m_pVideoSubtile && m_nSubLangGot)
	{
		nRC = m_pVideoSubtile->SelectLanguage(Index);
		if(nRC == 0)
			return 0;
		else
			return VOOSMP_ERR_Retry;	
	}

	return VOOSMP_ERR_Retry;
}

int	COSBasePlayer::HandleImageSub(VOOSMP_BUFFERTYPE* pBuffer, int nType, int RC)
{
	voCAutoLock lock( &m_Lock );
	
	if(m_pVideoSubtile == NULL)
	{
		m_pVideoSubtile = new CVideoSubtitle();

		m_pVideoSubtile->SetLibOperator((VO_LIB_OPERATOR *)m_pOSLibFunc);
		m_pVideoSubtile->SetWorkPath((TCHAR *)m_szPathLib);
	}

	int nFlag = 0;
	if(nType == voSubtitleType_ExternImage)
	{
		if(nType == voSubtitleType_ExternImage)
			nFlag |= VOSUB_FLAG_EXTERNIMAGE;
		
		if(RC == VOOSMP_ERR_EOS)
			nFlag |= VOSUB_FLAG_STREAM_EOS;

		VOOSMP_BUFFER_FORMAT* pFormat = (VOOSMP_BUFFER_FORMAT *)pBuffer->pData;

		if(pFormat)
			m_pVideoSubtile->SetInputData((unsigned char *)pFormat->pHeadData, pFormat->nHeadDataLen, (int)pBuffer->llTime, nFlag);
	}

	return 0;
}

int	COSBasePlayer::HandleSubtitle(VOOSMP_BUFFERTYPE* pBuffer, int nType, int RC)
{
	if(m_bCCParser == VO_FALSE)
		return 0;
	
	voCAutoLock lock( &m_Lock );
	
	if((pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_PROGRAM)||(pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_FORMAT))
	{
		if(m_pVideoSubtile == NULL)
		{
			m_pVideoSubtile = new CVideoSubtitle();

			m_pVideoSubtile->Enable(m_bCCParser);
			m_pVideoSubtile->SetLibOperator((VO_LIB_OPERATOR *)m_pOSLibFunc);
			m_pVideoSubtile->SetWorkPath((TCHAR *)m_szPathLib);

			if(nType == voSubtitleType_ClosedCaption)
			{
				if(m_sInVideoFormat.nHeadDataLen > 0)
					m_pVideoSubtile->Init((unsigned char *)m_sInVideoFormat.pHeadData, m_sInVideoFormat.nHeadDataLen, nType);
				else
					m_pVideoSubtile->Init(pBuffer->pBuffer, pBuffer->nSize, nType);
			}
//#ifdef _LINUX	
//			else if(nType == voSubtitleType_ExternFile)
//			{
//				m_pVideoSubtile->Init((unsigned char *)m_cLocalFilePath, strlen(m_cLocalFilePath), nType);			
//
//				//CJniEnvUtil	env(m_JavaVM);
//				if(m_JniUtil == NULL)
//					m_JniUtil = new CJniEnvUtil(m_JavaVM);
//				m_pVideoSubtile->SetParam(ID_SET_JAVA_ENV, m_JniUtil->getEnv());
//			}
//#endif
		}
		else
		{
			if(nType == voSubtitleType_ClosedCaption)
			{
				int nFlag = VOSUB_FLAG_CLOSECAPTION | VOSUB_FLAG_NEW_STREAM;

				if(m_sInVideoFormat.nHeadDataLen > 0)
					m_pVideoSubtile->SetInputData((unsigned char *)m_sInVideoFormat.pHeadData, m_sInVideoFormat.nHeadDataLen, (int)pBuffer->llTime, nFlag);
				else
					m_pVideoSubtile->SetInputData(pBuffer->pBuffer, pBuffer->nSize, (int)pBuffer->llTime, nFlag);
			}
		}
	}

	if(m_pVideoSubtile == NULL)
	{
		m_pVideoSubtile = new CVideoSubtitle();

		m_pVideoSubtile->Enable(m_bCCParser);
		m_pVideoSubtile->SetLibOperator((VO_LIB_OPERATOR *)m_pOSLibFunc);
		m_pVideoSubtile->SetWorkPath((TCHAR *)m_szPathLib);

		if(nType == voSubtitleType_ClosedCaption)
		{
			m_pVideoSubtile->Init((unsigned char *)m_sInVideoFormat.pHeadData, m_sInVideoFormat.nHeadDataLen, nType);
		}		
	}

	if((pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_PROGRAM))
	{
		m_pVideoSubtile->ReSet();

		if(nType == voSubtitleType_ClosedCaption)
		{
			if(m_sInVideoFormat.nHeadDataLen > 0)
				m_pVideoSubtile->Init((unsigned char *)m_sInVideoFormat.pHeadData, m_sInVideoFormat.nHeadDataLen, nType);
			else
				m_pVideoSubtile->Init(pBuffer->pBuffer, pBuffer->nSize, nType);
		}
//#ifdef _LINUX	
//		else if(nType == voSubtitleType_ExternFile)
//		{
//			m_pVideoSubtile->Init((unsigned char *)m_cLocalFilePath, strlen(m_cLocalFilePath), nType);			
//
//			CJniEnvUtil	env(m_JavaVM);
//			m_pVideoSubtile->SetParam(ID_SET_JAVA_ENV, env.getEnv());
//		}
//#endif
	}

	int nFlag = 0;
	if(nType == voSubtitleType_ClosedCaption)
	{
		nFlag |= VOSUB_FLAG_CLOSECAPTION;
		if(RC == VOOSMP_ERR_EOS)
			nFlag |= VOSUB_FLAG_STREAM_EOS;

		if(pBuffer->pBuffer != NULL && pBuffer->nSize > 0)
			m_pVideoSubtile->SetInputData(pBuffer->pBuffer, pBuffer->nSize, (int)pBuffer->llTime, nFlag);
	}
	else if(nType == voSubtitleType_ExternImage || nType == voSubtitleType_ExternText)
	{
		if(nType == voSubtitleType_ExternImage)
			nFlag |= VOSUB_FLAG_EXTERNIMAGE;
		else if(nType == voSubtitleType_ExternText)
			nFlag |= VOSUB_FLAG_EXTERNTEXT;
		
		if(RC == VOOSMP_ERR_EOS)
			nFlag |= VOSUB_FLAG_STREAM_EOS;

		VOOSMP_BUFFER_FORMAT* pFormat = (VOOSMP_BUFFER_FORMAT *)pBuffer->pData;

		if(pFormat && pFormat->pHeadData != NULL && pFormat->nHeadDataLen > 0)
			m_pVideoSubtile->SetInputData((unsigned char *)pFormat->pHeadData, pFormat->nHeadDataLen, (int)pBuffer->llTime, nFlag);
	}	

	return 0;
}

int	COSBasePlayer::GetSEISample (VOOSMP_SEI_INFO * pSample)
{
	if(m_pCSEIInfo == NULL || pSample == NULL || !pSample->nFlag)
		return VOOSMP_ERR_Retry;

	int nRC = VOOSMP_ERR_Retry;

	if ( m_nH264SEIInfo & VO_SEI_PIC_TIMING )
	{
		VO_PARSER_SEI_INFO nSEIInfo;
		memset(&nSEIInfo, 0, sizeof(VO_PARSER_SEI_INFO));

		nSEIInfo.time = pSample->llTime;

		nRC = m_pCSEIInfo->GetSEIData(&nSEIInfo);
		if(nRC == 0)
		{
			pSample->pInfo = m_pCSEIInfo->GetSEISample(&nSEIInfo,  VO_SEI_PIC_TIMING);
		}
	}
	else if (m_nH264SEIInfo & VO_SEI_USER_DATA_UNREGISTERED)
	{
		nRC = m_pCSEIInfo->GetSEIData( pSample );
	}

	return nRC;
}

int	COSBasePlayer::HandleH264(VOOSMP_BUFFERTYPE* pBuffer)
{
	int nRC = 0;
	VO_CODECBUFFER sCodeBuffer;
	
	if(m_sInVideoFormat.nCodec != VOOSMP_VIDEO_CodingH264 || pBuffer->pBuffer == NULL || pBuffer->nSize == 0)
		return 0;

	if(!(pBuffer->nFlag & VOOSMP_FLAG_BUFFER_UNKNOWN) && !m_nH264SEIInfo)
		return 0;

	if(m_hVideoParser == NULL || m_sVideoParserAPI.Init == NULL)
	{
		nRC = voGetVideoParserAPI (&m_sVideoParserAPI, VO_VIDEO_CodingH264);
		if(nRC) return nRC;

		nRC = m_sVideoParserAPI.Init(&m_hVideoParser);
		if(nRC) return nRC;
	}

	memset(&sCodeBuffer, 0, sizeof(VO_CODECBUFFER));

	sCodeBuffer.Buffer = pBuffer->pBuffer;
	sCodeBuffer.Length = pBuffer->nSize;
	sCodeBuffer.Time = pBuffer->llTime;

	//int nStartTime = voOS_GetSysTime();
		
	nRC = m_sVideoParserAPI.Process(m_hVideoParser, &sCodeBuffer);
	VOLOGI("@@@### m_sVideoParserAPI.Process nRC = 0x%08X", nRC);

	//VOLOGI("Process Using Time %d", voOS_GetSysTime() - nStartTime);

	if(pBuffer->nFlag & VOOSMP_FLAG_BUFFER_UNKNOWN)
	{
		if(nRC == VO_RETURN_SQHEADER || nRC == VO_RETURN_FMHEADER || nRC == VO_RETURN_SQFMHEADER)
		{
			VO_U32 nFrameType = 0;
			nRC = m_sVideoParserAPI.GetParam(m_hVideoParser, VO_PID_VIDEOPARSER_FRAMETYPE, &nFrameType);

			if(nRC == VO_ERR_NONE && nFrameType == VO_VIDEO_FRAME_I)
			{
				pBuffer->nFlag |= VOOSMP_FLAG_BUFFER_KEYFRAME;
			}

			if(nRC == VO_RETURN_SQHEADER || nRC == VO_RETURN_SQFMHEADER)
			{
				VO_S32 nWidth=0,nHeight =0;
				nRC = m_sVideoParserAPI.GetParam(m_hVideoParser, VO_PID_VIDEOPARSER_WIDTH, &nWidth);
				nRC |= m_sVideoParserAPI.GetParam(m_hVideoParser, VO_PID_VIDEOPARSER_HEIGHT, &nHeight);

				parser_out_sps_t		sH264SPS;
				memset(&sH264SPS, 0, sizeof(parser_out_sps_t));

				nRC |= m_sVideoParserAPI.GetParam(m_hVideoParser, VO_PID_VIDEOPARSER_SPS, &sH264SPS);

				if(nRC == VO_ERR_NONE && nWidth && nHeight)
				{
					if(m_sInVideoFormat.sFormat.video.Width != nWidth || m_sInVideoFormat.sFormat.video.Height != nHeight || CompareSPS(&sH264SPS, &m_sH264SPS))
					{
						VOLOGI("Resoltuion change to Width %d, Height %d, older width %d, height %d", nWidth, nHeight, m_sInVideoFormat.sFormat.video.Width, m_sInVideoFormat.sFormat.video.Height);
						m_sInVideoFormat.sFormat.video.Width = nWidth;
						m_sInVideoFormat.sFormat.video.Height = nHeight;

						CopySPS(&sH264SPS, &m_sH264SPS);

						if(!(pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_PROGRAM) && !(pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_FORMAT))
						{
							pBuffer->nFlag |= VOOSMP_FLAG_BUFFER_NEW_FORMAT;
							m_nVideoChanged = 1;
						}					
					}

					if((pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_FORMAT) || (pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_PROGRAM))
					{
						if(pBuffer->pData == NULL || ((VOOSMP_BUFFER_FORMAT *)pBuffer->pData)->nHeadDataLen == 0)
						{
							m_sInVideoFormat.sFormat.video.Width = nWidth;
							m_sInVideoFormat.sFormat.video.Height = nHeight;

							if(m_sInVideoFormat.pHeadData)
							{
								delete []m_sInVideoFormat.pHeadData;
								m_sInVideoFormat.pHeadData = NULL;
								m_sInVideoFormat.nHeadDataLen = 0;
							}

							m_sInVideoFormat.pHeadData = new char[sH264SPS.buffer_lenth];
							memcpy(m_sInVideoFormat.pHeadData, sH264SPS.buffer, sH264SPS.buffer_lenth);
							m_sInVideoFormat.nHeadDataLen = sH264SPS.buffer_lenth;

							pBuffer->pData = &m_sInVideoFormat;
						}

						if(pBuffer->pBuffer && pBuffer->nSize > 0)
						{
							m_sBufferVideoSaved.llTime = pBuffer->llTime;
							m_sBufferVideoSaved.nDuration = pBuffer->nDuration;
							m_sBufferVideoSaved.pData = NULL;
							m_sBufferVideoSaved.nFlag = pBuffer->nFlag & ~(VOOSMP_FLAG_BUFFER_UNKNOWN | VOOSMP_FLAG_BUFFER_NEW_FORMAT | VOOSMP_FLAG_BUFFER_NEW_PROGRAM);
							m_sBufferVideoSaved.nSize = pBuffer->nSize;

							if(m_sBufferVideoSaved.pBuffer)
							{
								delete []m_sBufferVideoSaved.pBuffer;
								m_sBufferVideoSaved.pBuffer = NULL;
							}

							m_sBufferVideoSaved.pBuffer = new unsigned char[pBuffer->nSize];
							memcpy(m_sBufferVideoSaved.pBuffer, pBuffer->pBuffer, pBuffer->nSize);

							pBuffer->nSize = 0;
							pBuffer->pBuffer = NULL;
							pBuffer->nFlag &= (VOOSMP_FLAG_BUFFER_NEW_FORMAT | VOOSMP_FLAG_BUFFER_NEW_PROGRAM);

							m_nSaveVideoBuffer = 1;	   	   		  
						}
					}
				}
			}
		}
	}

	if(m_nH264SEIInfo&VO_SEI_PIC_TIMING)
	{
		VO_PARSER_SEI_INFO sSEIInfo;
		memset(&sSEIInfo, 0, sizeof(VO_PARSER_SEI_INFO));

		if(m_pCSEIInfo == NULL)
		{
			m_pCSEIInfo = new CSEIInfo(m_nSEICount);
			m_pCSEIInfo->Enable(m_nH264SEIInfo);
		}

		int nRCC = m_sVideoParserAPI.GetParam(m_hVideoParser, VO_PID_VIDEOPARSER_SEI_INFO, &sSEIInfo);
		VOLOGI("@@@### m_sVideoParserAPI.GetParam nRC = 0x%08X", nRCC);

		if(nRCC == 0)
		{
			sSEIInfo.time = pBuffer->llTime;
			nRCC = m_pCSEIInfo->AddSEIData(&sSEIInfo);
			VOLOGI("@@@### Add SEIData.time = %d, retVal = 0x%08X", (int)sSEIInfo.time, nRCC);
		}

		if(m_nLastSendtime == 0)
			m_nLastSendtime = voOS_GetSysTime();

		int nTimePassed = (int)voOS_GetSysTime() - m_nLastSendtime;
		VOLOGI("@@@### nTimePassed = %d", nTimePassed);

		if( (m_nSEICallBackTime > 0) && ( nTimePassed > m_nSEICallBackTime ) ) 
		{
			int PlayTime = 0;

			nRCC = GetPos(&PlayTime);

			VOOSMP_SEI_INFO sSEIInfoSample;
			VO_PARSER_SEI_INFO nSEIInfo;
			memset(&nSEIInfo, 0, sizeof(VO_PARSER_SEI_INFO));
			memset(&sSEIInfoSample, 0, sizeof(VOOSMP_SEI_INFO));

			nSEIInfo.time = PlayTime;

			nRCC = m_pCSEIInfo->GetSEIData(&nSEIInfo);
			VOLOGI("@@@###  m_pCSEIInfo->GetSEIData() return 0x%08X, nCurrTime = %d", nRCC, PlayTime);
			if(nRCC == 0)
			{
				if(m_nH264SEIInfo&VO_SEI_PIC_TIMING)
				{
					VOOSMP_SEI_PIC_TIMING* sPicTiming = NULL;

					sPicTiming = (VOOSMP_SEI_PIC_TIMING *)m_pCSEIInfo->GetSEISample(&nSEIInfo, VO_SEI_PIC_TIMING);

					if(sPicTiming)
					{
						voCAutoLock lock( &m_EventLock );					
						int SEIType = VOOSMP_FLAG_SEI_PIC_TIMING;

						sSEIInfoSample.llTime = nSEIInfo.time;
						sSEIInfoSample.nFlag = SEIType;
						sSEIInfoSample.pInfo = sPicTiming;
						HandleEvent(VOOSMP_CB_SEI_INFO, &sSEIInfoSample, NULL);
						VOLOGI("@@@### Send Event VOOSMP_CB_SEI_INFO to APP");
					}
				}

				if(m_nH264SEIInfo&VO_SEI_USER_DATA_REGISTERED_ITU_T_T35)
				{
					;
				}

				m_nLastSendtime = voOS_GetSysTime();
			}
		}
	}
	else if(m_nH264SEIInfo&VO_SEI_USER_DATA_UNREGISTERED)
	{
		if(m_pCSEIInfo == NULL)
		{
			m_pCSEIInfo = new CSEIInfo(m_nSEICount);
			m_pCSEIInfo->Enable(m_nH264SEIInfo);
		}

		VO_H264_USERDATA_Params sUser_data;
		nRC = m_sVideoParserAPI.GetParam(m_hVideoParser, VO_PID_VIDEOPARSER_USERDATA, &sUser_data);
		VOLOGI("@@@### m_sVideoParserAPI.GetParam(VO_PID_VIDEOPARSER_USERDATA) nRC = 0x%08X", nRC);

		if (nRC == VO_ERR_NONE)
		{
			VOOSMP_SEI_INFO sSEIInfoSample;
			memset(&sSEIInfoSample, 0, sizeof(VOOSMP_SEI_INFO));
			sSEIInfoSample.llTime = pBuffer->llTime;
			sSEIInfoSample.nFlag = VO_SEI_USER_DATA_UNREGISTERED;
			sSEIInfoSample.pInfo = &sUser_data;
			m_pCSEIInfo->AddSEIData(&sSEIInfoSample);

			if(m_nLastSendtime == 0)
			{
				m_nLastSendtime = voOS_GetSysTime();
			}
			int nTimePassed = (int)voOS_GetSysTime() - m_nLastSendtime;
			VOLOGI("@@@### nTimePassed = %d, m_nSEICallBackTime = %d", nTimePassed, m_nSEICallBackTime);
			if( (m_nSEICallBackTime >= 0) && ( nTimePassed > m_nSEICallBackTime ) ) 
			{
				HandleEvent(VOOSMP_CB_SEI_INFO, &sSEIInfoSample, NULL);
				VOLOGI("@@@### Send Event VO_H264_USERDATA_Params to APP; .count = %d, size[0] = %d, size[1] = %d, size[2] = %d", sUser_data.count, sUser_data.size[0], sUser_data.size[1], sUser_data.size[2]);
			}
		}
	}

	return nRC;
}

int	COSBasePlayer::CheckingStatus(int nCount)
{
	int n = 0;

	while(m_nChanging)
	{
		voOS_Sleep(2);
		n++;

		if(n > nCount)
			break;
	}

	return 0;
}


int COSBasePlayer::DumpDebugInfo(int ssType, int nRC, VOOSMP_BUFFERTYPE * pBuffer)
{
	if(ssType == VOOSMP_SS_AUDIO)
	{
		if(m_nLogAudioLevel&1)
		{
			if(m_nStartSystemTime == 0)
				m_nStartSystemTime = voOS_GetSysTime();
			
			int Diff = 0;
			Diff = (int)pBuffer->llTime - (voOS_GetSysTime() - m_nStartSystemTime);

			VOLOGI("GetAudio Sample return %x, TimeStamp %d, SystemTime %d, Diff %d, BufferSize %d, nFlag %d", nRC, (int)pBuffer->llTime, (int)voOS_GetSysTime() - m_nStartSystemTime, Diff, pBuffer->nSize, pBuffer->nFlag);
		}

		if(nRC == VOOSMP_ERR_None)
		{
			if((m_nDumpAudioLevel & 8) && m_hAudioDumpFile)
			{
				int nFlag = (int)(pBuffer->nFlag);
				m_pFileOP->Write(m_hAudioDumpFile,(VO_PTR)(&nFlag), 4);
			}
			
			if((m_nDumpAudioLevel & 4) && m_hAudioDumpFile)
			{
				int nTime = (int)(pBuffer->llTime);
				m_pFileOP->Write(m_hAudioDumpFile,(VO_PTR)(&nTime), 4);
			}

			if((m_nDumpAudioLevel & 2) && m_hAudioDumpFile)
			{
				int nSize = (int)(pBuffer->nSize);
				if((pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_FORMAT) || (pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_PROGRAM))
				{
					VOOSMP_BUFFER_FORMAT* pFormat = (VOOSMP_BUFFER_FORMAT *)pBuffer->pData;
				
					if(pFormat)
						m_pFileOP->Write(m_hAudioDumpFile, (VO_PTR)(&pFormat->nHeadDataLen) , 4);
				}
				else
				{
					m_pFileOP->Write(m_hAudioDumpFile, (VO_PTR)(&nSize), 4);
				}

			}

			if((m_nDumpAudioLevel & 1) && m_hAudioDumpFile)
			{
				if(pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_FORMAT)
				{
					VOOSMP_BUFFER_FORMAT* pFormat = (VOOSMP_BUFFER_FORMAT *)pBuffer->pData;
					if(pFormat && pFormat->pHeadData && pFormat->nHeadDataLen > 0)
						m_pFileOP->Write(m_hAudioDumpFile, pFormat->pHeadData , pFormat->nHeadDataLen);

					if(pBuffer->pBuffer && pBuffer->nSize > 0)
					{
						if(m_nDumpAudioLevel & 2)
							m_pFileOP->Write(m_hAudioDumpFile, (VO_PTR)(&(pBuffer->nSize)), 4);

						m_pFileOP->Write(m_hAudioDumpFile, pBuffer->pBuffer , pBuffer->nSize);
					}
				}
				else
				{
					if(pBuffer->pBuffer && pBuffer->nSize > 0)
						m_pFileOP->Write(m_hAudioDumpFile, pBuffer->pBuffer , pBuffer->nSize);
				}
			}
		}
	}
	else if(ssType == VOOSMP_SS_VIDEO)
	{
		if(m_nLogVideoLevel&1)
		{
			if(m_nStartSystemTime == 0)
				m_nStartSystemTime = voOS_GetSysTime();
			
			int Diff = 0;
			Diff = (int)pBuffer->llTime - (voOS_GetSysTime() - m_nStartSystemTime);

			VOLOGI("GetVideo Sample return %x, TimeStamp %d, SystemTime %d, Diff %d, BufferSize %d, nFlag %d", nRC, (int)pBuffer->llTime, (int)voOS_GetSysTime() - m_nStartSystemTime, Diff, pBuffer->nSize, pBuffer->nFlag);
		}

		if(nRC == VOOSMP_ERR_None)
		{		
			if((m_nDumpVideoLevel & 8) && m_hVideoDumpFile)
			{
				int nFlag = (int)(pBuffer->nFlag);
				m_pFileOP->Write(m_hVideoDumpFile,(VO_PTR)(&nFlag), 4);
			}
			
			if((m_nDumpVideoLevel & 4) && m_hVideoDumpFile)
			{
				int nTime = (int)(pBuffer->llTime);
				m_pFileOP->Write(m_hVideoDumpFile,(VO_PTR)(&nTime), 4);
			}

			if((m_nDumpVideoLevel & 2) && m_hVideoDumpFile)
			{
				int nSize = (int)(pBuffer->nSize);
				if((pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_FORMAT) || (pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_PROGRAM))
				{
					VOOSMP_BUFFER_FORMAT* pFormat = (VOOSMP_BUFFER_FORMAT *)pBuffer->pData;
				
					if(pFormat)
						m_pFileOP->Write(m_hVideoDumpFile, (VO_PTR)(&pFormat->nHeadDataLen) , 4);
				}
				else
				{
					m_pFileOP->Write(m_hVideoDumpFile, (VO_PTR)(&nSize), 4);
				}
			}

			if((m_nDumpVideoLevel & 1) && m_hVideoDumpFile)
			{
				if((pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_FORMAT) || (pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_PROGRAM))
				{
					VOOSMP_BUFFER_FORMAT* pFormat = (VOOSMP_BUFFER_FORMAT *)pBuffer->pData;
					if(pFormat && pFormat->pHeadData && pFormat->nHeadDataLen > 0)
						m_pFileOP->Write(m_hVideoDumpFile, pFormat->pHeadData , pFormat->nHeadDataLen);

					if(pBuffer->pBuffer && pBuffer->nSize > 0)
					{
						if(m_nDumpVideoLevel & 2)
							m_pFileOP->Write(m_hVideoDumpFile, (VO_PTR)(&(pBuffer->nSize)) , 4);

						m_pFileOP->Write(m_hVideoDumpFile, pBuffer->pBuffer , pBuffer->nSize);
					}
				}
				else
				{
					if(pBuffer->pBuffer && pBuffer->nSize > 0)
						m_pFileOP->Write(m_hVideoDumpFile, pBuffer->pBuffer , pBuffer->nSize);
				}
			}
		}
	}

	return 0;

}


int COSBasePlayer::CompareSPS(parser_out_sps_t *pSrc, parser_out_sps_t *pDes)
{
	int n = 0;

	if(pSrc->profile_idc != pDes->profile_idc ||
	   pSrc->level_idc != pDes->level_idc ||
	   pSrc->seq_parameter_set_id != pDes->seq_parameter_set_id ||
	   pSrc->log2_max_frame_num_minus4 != pDes->log2_max_frame_num_minus4 ||
	   pSrc->pic_order_cnt_type != pDes->pic_order_cnt_type ||
	   pSrc->num_ref_frames != pDes->num_ref_frames ||
	   pSrc->gaps_in_frame_num_value_allowed_flag != pDes->gaps_in_frame_num_value_allowed_flag ||
	   pSrc->pic_width_in_mbs_minus1 != pDes->pic_width_in_mbs_minus1 ||
	   pSrc->pic_height_in_map_units_minus1 != pDes->pic_height_in_map_units_minus1 ||
	   pSrc->frame_mbs_only_flag != pDes->frame_mbs_only_flag ||
	   pSrc->mb_adaptive_frame_field_flag != pDes->mb_adaptive_frame_field_flag ||
	   pSrc->direct_8x8_inference_flag != pDes->direct_8x8_inference_flag ||
	   pSrc->frame_cropping_flag != pDes->frame_cropping_flag ||
	   pSrc->frame_cropping_rect_left_offset != pDes->frame_cropping_rect_left_offset ||
	   pSrc->frame_cropping_rect_right_offset != pDes->frame_cropping_rect_right_offset ||
	   pSrc->frame_cropping_rect_top_offset != pDes->frame_cropping_rect_top_offset ||
	   pSrc->frame_cropping_rect_bottom_offset != pDes->frame_cropping_rect_bottom_offset)
	   n = 1;

	return n;

}


int COSBasePlayer::CopySPS(parser_out_sps_t *pSrc, parser_out_sps_t *pDes)
{
	pDes->profile_idc = pSrc->profile_idc;
	pDes->level_idc = pSrc->level_idc;
	pDes->seq_parameter_set_id = pSrc->seq_parameter_set_id;
	pDes->log2_max_frame_num_minus4 = pSrc->log2_max_frame_num_minus4;
	pDes->pic_order_cnt_type = pSrc->pic_order_cnt_type;
	pDes->num_ref_frames = pSrc->num_ref_frames;
	pDes->gaps_in_frame_num_value_allowed_flag = pSrc->gaps_in_frame_num_value_allowed_flag;
	pDes->pic_width_in_mbs_minus1 = pSrc->pic_width_in_mbs_minus1;
	pDes->pic_height_in_map_units_minus1 = pSrc->pic_height_in_map_units_minus1;
	pDes->frame_mbs_only_flag = pSrc->frame_mbs_only_flag;
	pDes->mb_adaptive_frame_field_flag = pSrc->mb_adaptive_frame_field_flag;
	pDes->direct_8x8_inference_flag = pSrc->direct_8x8_inference_flag;
	pDes->frame_cropping_flag =  pSrc->frame_cropping_flag;
	pDes->frame_cropping_rect_left_offset = pSrc->frame_cropping_rect_left_offset;
	pDes->frame_cropping_rect_right_offset = pSrc->frame_cropping_rect_right_offset;
	pDes->frame_cropping_rect_top_offset = pSrc->frame_cropping_rect_top_offset;
	pDes->frame_cropping_rect_bottom_offset = pSrc->frame_cropping_rect_bottom_offset;

	return 0;
}
