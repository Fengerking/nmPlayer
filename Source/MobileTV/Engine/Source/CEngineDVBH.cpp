#include "CEngineDVBH.h"
// #include "ConfigFile.h"
#include "cmnFile.h"

#define CONFIG_FILE _T("voEngDVBH.cfg")

const int sampRateTab[12] = {
	96000, 88200, 64000, 48000, 44100, 32000, 
	24000, 22050, 16000, 12000, 11025,  8000
};

int Config2ADTSHeader(/*in*/	unsigned char *config, 
					  /*in*/	unsigned int  conlen,
					  /*in*/	unsigned int  framelen,
					  /*in&out*/unsigned char *adtsbuf,
					  /*in&out*/unsigned int  *adtslen)
{
	int object, sampIdx, sampFreq, chanNum;
	unsigned char tbuf;

	if(config == NULL || conlen < 2 || adtsbuf == NULL || *adtslen < 7)
		return -1;

	object = config[0] >> 3;
	if(object > 5 && object != 29)
	{
		return -2;
	}
	else if(object == 5 || object == 29 )
	{
		object = 2;
	}
	if(object == 0) object += 1;

	object--;

	sampIdx = ((config[0] & 3) << 1) | (config[1] >> 7);
	if(sampIdx == 0x0f)
	{
		int idx;

		if(conlen < 5)
			return -3;

		sampFreq = ((config[1]&0x7f) << 17) | (config[2] << 9) | ((config[3] << 1)) | (config[4] >> 7);

		for (idx = 0; idx < 12; idx++) {
			if (sampFreq == sampRateTab[idx]) {
				sampIdx = idx;
				break;
			}
		}

		if (idx == 12)
			return -4;

		chanNum = (config[4]&0x78) >> 3;
	}
	else
	{
		chanNum = (config[1]&0x78) >> 3;
	}

	if(chanNum > 7)
		return -5;

	adtsbuf[0] = 0xFF; adtsbuf[1] = 0xF9;

	adtsbuf[2] = (unsigned char)((object << 6) | (sampIdx << 2) | ((chanNum&4)>>2));

	framelen += 7;

	if(framelen > 0x1FFF)
		return -6;

	adtsbuf[3] = (chanNum << 6) | (framelen >> 11);
	adtsbuf[4] = (framelen & 0x7FF) >> 3;
	adtsbuf[5] = ((framelen & 7) << 5) | 0x1F;
	adtsbuf[6] = 0xFC;	

	*adtslen = 7;

	return 0;
}

int CEngineDVBH::LoadConfig()
{
	return 0;
}

int CEngineDVBH::SaveConfig()
{
	return 0;
}


CEngineDVBH::CEngineDVBH(void)
:  m_pSDPData (NULL)
, m_lastVideoFrameType (VO_MTV_FRAME_POS_END)
{
}

CEngineDVBH::~CEngineDVBH(void)
{
}

int CEngineDVBH::InitParser()
{
	m_pParser = new CCMMBDemux;

	VO_PARSER_INIT_INFO	dmxInitInfo;
	memset (&dmxInitInfo, 0, sizeof (VO_PARSER_INIT_INFO));
	dmxInitInfo.pProc = OnParsedDataProc;
	dmxInitInfo.pUserData = this;
	int nRC = m_pParser->Open(&dmxInitInfo);
//	m_pParser->SetCASType(CAS_TYPE);

	return VO_ERR_LIVESRC_OK;
}

int CEngineDVBH::ReleaseParser()
{
	// 	CloseHandle(m_hESGParsed);

	// 	OutputDebugString(_T("[ENG] To delete m_pParser.\n"));
	delete m_pParser;
	//	OutputDebugString(_T("[ENG] after delete m_pParser.\n"));

	return VO_ERR_LIVESRC_OK;
}

int CEngineDVBH::OpenDevice()
{
	memset(m_AudioExData, 0, 2);

	InitParser();

	return 0;
}

int CEngineDVBH::CloseDevice()
{
	Stop();

	m_Source.Close();

	ReleaseParser();

	return 0;
}

void CEngineDVBH::ReleaseMemory()
{
	delete[] m_pSDPData;
	m_pSDPData = NULL;
}

void CEngineDVBH::OnSendData(int nChannelID, unsigned char * pData, int nDataLen)
{
// 	int accepted;
// 	m_pParser->Process(nChannelID, pData, nDataLen, &accepted);

}

void CEngineDVBH::OnSourceData(VO_RTPSRC_DATA * pData)
{
	if (m_pParser == NULL)
		return;

	VOLOGI("[ENG] Received data size = %d\n", pData->nDataSize);

	VO_PARSER_INPUT_BUFFER inputBuf;
	inputBuf.pBuf = (VO_PBYTE)pData->pData;
	inputBuf.nBufLen = pData->nDataSize;
	inputBuf.nStreamID = pData->nChannelID;
	m_pParser->Process(&inputBuf);
}

int CEngineDVBH::BeginScan(VO_PTCHAR szParseModule)
{
/*
#ifdef _DEBUG
	OutputDebugString(_T("CEngineDVBH::BeginScan\n"));
#endif //_DEBUG
	ReleaseMemory();
	VORETURNCODE rc = m_parser.Init(szParseModule);
	if (VORC_FAILED(rc))
		return rc;
	rc = m_parser.Open(this);
	if (VORC_FAILED(rc))
		return rc;
	m_hFinishEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_fOrigSendData = m_fSendData;
	m_pUserData = this;
	m_fSendData = OnSendDataEntry;
*/
	return 0;
}

int CEngineDVBH::EndScan()
{
/*
#ifdef _DEBUG
	OutputDebugString(_T("CEngineDVBH::EndScan\n"));
#endif //_DEBUG
	m_pUserData = m_pOrigUserData;
	m_fSendData = m_fOrigSendData;
	CloseHandle(m_hFinishEvent);
	m_parser.Close();
	VORETURNCODE rc = m_parser.Uninit();
*/
	return 0;
}

void CEngineDVBH::ResetParser()
{
// 	m_parser.Close();
// 	m_parser.Open(this);
}

int CEngineDVBH::Scan(VO_LIVESRC_SCAN_PARAMEPTERS * pParam)
{
	return 0;
/*
	#ifdef _DEBUG
		OutputDebugString(_T("CEngineDVBH::Scan\n"));
	#endif //_DEBUG
		int rc = 0;
		rc = BeginScan(MODULE_PARSE_DVBH);
		if (rc)
			return rc;
		EnableScan(TRUE);
		VODVBHSCANPARAM* param = pParam ? (VODVBHSCANPARAM*) pParam : &DVBH_SCAN_PARAM;
	#if 0 //def _FILEMODE
		param = &ISDBT_SCAN_PARAM;
	#endif //_FILEMODE
		int nCount = (param->nEndFrequency - param->nStartFrequency) / ISDBT_USER_SCAN_GAP + 1;
		if (nCount <= 0)
			return 0;
		for (int i = 0; i < nCount; i++)
		{
			int nFrequency = param->nStartFrequency + ISDBT_USER_SCAN_GAP * i;
			rc = OpenChannel(nFrequency, ISDBT_USER_SCAN_GAP);
			if (rc == VORC_OK)	// Frequency locked
			{
				rc = Start();
				if (rc)
					continue;
	
				m_nCurrentFrequency = nFrequency;
				m_nCurrentBandwidth = ISDBT_USER_SCAN_GAP;
				VO_U32 dw = WaitForSingleObject(m_hFinishEvent, SCAN_TIMEOUT);
				if (dw != WAIT_OBJECT_0)
				{
	#ifdef _DEBUG
					OutputDebugString(_T("Query scan result\n"));
	#endif //_DEBUG
					m_parser.Query();
					dw = WaitForSingleObject(m_hFinishEvent, SCAN_TIMEOUT);
	#ifdef _DEBUG
					OutputDebugString(_T("WaitForSingleObject done\n"));
	#endif //_DEBUG
				}
			}
			Stop();	
			CloseChannel();
			m_nCurrentFrequency = 0;
			m_nCurrentBandwidth = 0;
			SendScanProgress(i + 1, nCount);
			if (!ScanEnabled())
				break;
			ResetParser();
		}
		EndScan();
	
	return 0;
	*/
}

int CEngineDVBH::SetChannel(int nChannelID)
{
	m_Source.Close();

	ReleaseMemory();

	// tag: 20100813
	//int nRet = ReadSDPData(_T("E:\\MFSCSim\\wtcc.sdp"), &m_pSDPData);
	int nRet = ReadSDPData(_T("E:\\MFSCSim\\22.2.sdp"), &m_pSDPData);
	if (nRet != 0)
	{
		VOLOGI("[ENG] Can't read SDP data.");
		return -1;
	}

	VO_RTPSRC_INIT_INFO	srcInitInfo;
	memset (&srcInitInfo, 0, sizeof (VO_RTPSRC_INIT_INFO));
	srcInitInfo.pProc = OnSourceDataProc;
	srcInitInfo.pUserData = this;

	m_Source.Open((char *)m_pSDPData, &srcInitInfo);

	VOLOGI("[ENG] SetChannel finished.");
	
	return 0;

/*
#ifdef _DEBUG
	OutputDebugString(_T("CEngineDVBH::SetChannel\n"));
#endif //_DEBUG
	int nChannel = nChannelID < 0 ? m_nDefaultChannel : nChannelID;
	if (nChannel < 0)
		nChannel = 0;
	if (nChannel >= m_cChannel)
		return VORC_ENG_INVALID_ARG;

	VOENGCHANNELINFO* channel = m_aChannelInfo + nChannel;
	ProgramData* data = (ProgramData*)(channel->pData);
	VO_BOOL bStated = IsStatusStarted();
	if (nChannel != m_nCurrentChannel)
	{
		VO_BOOL bFreqChanged = data->frequency != m_nCurrentFrequency;
		if (m_nCurrentChannel >= 0)
		{
			if (bFreqChanged)
			{
				if (bStated)
					Stop();
				CloseChannel();
				m_nCurrentFrequency = 0;
				m_nCurrentBandwidth = 0;
			}
			m_aChannelInfo[m_nCurrentChannel].nCurrent = FALSE;
		}
#ifdef _DUMP_DATA
		if (m_fDump)
			fclose(m_fDump);
		OutputDebugString(_T("\nCEngineDVBH - SendChannelStopped\n"));
#endif //_DUMP_DATA
		SendChannelStopped(ENGINE_ISDBT, ENGINE_ISDBT);
		if (bFreqChanged)
		{
			if (VORC_OK == OpenChannel(data->frequency, data->bandwidth))
			{
				m_nCurrentFrequency = data->frequency;
				m_nCurrentBandwidth = data->bandwidth;
			}
			else
			{
				m_nCurrentFrequency = 0;
				m_nCurrentBandwidth = 0;
				return VORC_FAIL;
			}
		}
		m_nCurrentChannel =  nChannel;
	}
	channel->nCurrent = TRUE;
// 	SetFilter(data->pids, data->pid_count, true); //!!! must sync
	if (bStated) //restore status
	{
		if (!IsStatusStarted())
			Start();
		//SendChannelStarted(ENGINE_DVBT, ENGINE_DVBT);
	}
	else
	{
		if (!IsStatusStopped())
			Stop();
	}
#ifdef _DUMP_DATA
	TCHAR path[MAX_PATH];
	_stprintf(path, _T("CH%d.ts"), nChannel);
	if (m_fDump)
		fclose(m_fDump);
	m_fDump = _tfopen(path, _T("wb"));
	OutputDebugString(_T("\nCEngineDVBH - Start dump new file\n"));
	OutputDebugString(_T("\nCEngineDVBH - SendChannelStarted\n"));
#endif //_DUMP_DATAk
	SendProgramChanged(-1, data->id);
	SendChannelStarted(ENGINE_ISDBT, ENGINE_ISDBT);
	return 0;
	*/
}

int CEngineDVBH::StartReceive()
{
	m_Source.Start();
	return VO_ERR_LIVESRC_OK;
}

int CEngineDVBH::StopReceive()
{
	m_Source.Stop();
	return VO_ERR_LIVESRC_OK;
}

int	CEngineDVBH::GetESG (int nChannelID, VO_LIVESRC_ESG_INFO* ppESGInfo)
{
	return -1;
/*
#ifdef _DEBUG
	OutputDebugString(_T("CEngineDVBH::GetESG\n"));
#endif //_DEBUG

	UpdateESG(MODULE_PARSE_ISDBT);

	for (int i = 0; i < m_cChannel; i++)
	{
		if (nChannelID == m_aChannelInfo[i].nChannelID)
		{
			ProgramData* data = (ProgramData*) m_aChannelInfo[i].pData;
			*pCount = data->ESG_count;
			*ppESGInfo = data->ESG_items;
			return 0;
		}
	}
	return -1;
*/
}


void CEngineDVBH::OnParsedData(VO_PARSER_OUTPUT_BUFFER * pData)
{
	if (pData == NULL) 
		return;

	VO_PARSER_OUTPUT_TYPE nInfoType = (VO_PARSER_OUTPUT_TYPE)pData->nType;
	VO_VOID * pInfoBuf = pData->pOutputData;

	switch (nInfoType)
	{
	case VO_PARSER_OT_STREAMINFO :
		{
			VO_PARSER_STREAMINFO * pStreamInfo = (VO_PARSER_STREAMINFO *)pData->pOutputData;
			if (pStreamInfo->pAudioExtraData != NULL && pStreamInfo->nAudioExtraSize > 0)
				memcpy(m_AudioExData, pStreamInfo->pAudioExtraData, 2);
		}
		break;
	case VO_PARSER_OT_AUDIO :
		{
			VO_MTV_FRAME_BUFFER * pFrame = (VO_MTV_FRAME_BUFFER *)(pData->pOutputData);
// 			pFrame->nStartTime = pFrame->nStartTime * 10 / 225;
// 			_stprintf(m_szDebugString, _T("Audio position %d \n"), pFrame->nPos);
// 			OutputDebugString(m_szDebugString);
// 			if (pFrame->nPos == VO_MTV_FRAME_POS_BEGIN)
// 			{
// 				m_nAudioTimeStamp = pFrame->nStartTime;
//  			_stprintf(m_szDebugString, _T("------------------- > Audio timestamp %d \n"), pFrame->nStartTime);
// 			OutputDebugString(m_szDebugString);
// 			}

			if (pFrame->nCodecType == VO_AUDIO_CodingAAC && pFrame->nPos == VO_MTV_FRAME_POS_WHOLE/*VO_MTV_FRAME_POS_MID*/)
			{
// 				SendData(VO_LIVESRC_AUDIOFRAME, (VO_BYTE *)(pData->pOutputData), sizeof(VO_MTV_FRAME_BUFFER));

				int nADTSheaderSize = 7;
				VO_BYTE *	pNewData = new VO_BYTE[pFrame->nSize + 7];
				Config2ADTSHeader(m_AudioExData, 2, pFrame->nSize, pNewData, (unsigned int *)&nADTSheaderSize);
				VO_BYTE *	pOldData = pFrame->pData;
				int		nOldSize = pFrame->nSize;
				memcpy(pNewData + 7, pFrame->pData, pFrame->nSize);
				pFrame->pData = pNewData;
				pFrame->nSize += 7;
// 				pFrame->nStartTime = m_nAudioTimeStamp;
				//SendData(VO_LIVESRC_AUDIOFRAME, (VO_BYTE *)pFrame, sizeof(VO_MTV_FRAME_BUFFER));
				SendData(VO_LIVESRC_OUTPUT_AUDIO, pNewData, pFrame->nSize, pFrame->nStartTime);
				
				pFrame->pData = pOldData;
				pFrame->nSize = nOldSize;
				delete[] pNewData;

// 				VO_BYTE *	pNewData = new VO_BYTE[pFrame->nSize + 7];
// 				int		nSampleRate = 6;
// 				int		adtsHeaderSize = 7;
// 				pNewData[0] = 0xFF;	
// 				pNewData[1] = 0xF9;	
// 				pNewData[2] = (0x1 << 6) | (nSampleRate & 0xF) << 2 ;	
// 				pNewData[3] = 0x80;	
// 				pNewData[4] = ((pFrame->nSize + adtsHeaderSize) >> 3) & 0xFF;
// 				pNewData[5] = ((pFrame->nSize + adtsHeaderSize) & 0x7) << 5 | 0x1F;
// 				pNewData[6] = 0xFC;
// 
// 				VO_BYTE *	pOldData = pFrame->pData;
// 				int		nOldSize = pFrame->nSize;
// 				memcpy(pNewData + 7, pFrame->pData, pFrame->nSize);
// 				pFrame->pData = pNewData;
// 				pFrame->nSize += adtsHeaderSize;
// 				SendData(VO_LIVESRC_AUDIOFRAME, (VO_BYTE *)pFrame, sizeof(VO_MTV_FRAME_BUFFER));
// 				pFrame->pData = pOldData;
// 				pFrame->nSize = nOldSize;
// 				delete pNewData;
			}
			else if (pFrame->nCodecType == VO_AUDIO_CodingDRA)
			{
				;// To be added
			}

			// 			SendData(VO_LIVESRC_AUDIOFRAME, (VO_BYTE *)(pData->pOutputData), sizeof(VO_MTV_FRAME_BUFFER));
		}
		break;
	case VO_PARSER_OT_VIDEO :
		{
			VO_MTV_FRAME_BUFFER * pFrame = (VO_MTV_FRAME_BUFFER *)(pData->pOutputData);
// 			pFrame->nStartTime = pFrame->nStartTime * 10 / 225;

// 			if (pFrame->nPos != VO_MTV_FRAME_POS_WHOLE)
// 				VideoFrameBuffering(pFrame);
// 			else
				//SendData(VO_LIVESRC_VIDEOFRAME, (VO_BYTE *)pFrame, sizeof(VO_MTV_FRAME_BUFFER));
			SendData(VO_LIVESRC_OUTPUT_VIDEO, pFrame->pData, pFrame->nSize, pFrame->nStartTime);
		}
		break;
	default :
		break;
	}
}

int CEngineDVBH::VideoFrameBuffering(VO_MTV_FRAME_BUFFER * pFrame)
{
	if (pFrame->nPos == VO_MTV_FRAME_POS_BEGIN)
	{
		memset(&m_BufferedVideoFrame, 0, sizeof(VO_MTV_FRAME_BUFFER));
	}

	return 0;
}

int CEngineDVBH::ReadSDPData(VO_PTCHAR pPath, VO_PBYTE* ppData)
{
	VO_FILE_SOURCE	fileSource;
	memset(&fileSource, 0, sizeof(VO_FILE_SOURCE));
	fileSource.nFlag = VO_FILE_TYPE_NAME;
	fileSource.nOffset = 0;
	fileSource.nLength = 0xFFFFFFFF;
	fileSource.pSource = (VO_PTR)pPath;
	fileSource.nMode = VO_FILE_READ_ONLY;

	VO_PTR hFile = cmnFileOpen(&fileSource);
	if (hFile == NULL)
		return -1;

	VO_S64 nFileSize = cmnFileSize(hFile);
	if (nFileSize <= 0 || nFileSize > 10000)
	{
		cmnFileClose(hFile);
		return -1;
	}
	VO_BYTE * pSDPdata = new VO_BYTE[nFileSize + 4];
	VO_U32 dwRead = cmnFileRead (hFile, pSDPdata, nFileSize);
	if (dwRead != nFileSize)
		return -1;

	pSDPdata[nFileSize] = '\0';
	*ppData = pSDPdata;

	cmnFileClose(hFile);

	return 0;
}

