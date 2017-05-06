#include "RTPSourceWarper.h"
#include "ipnetwork.h"
#include "cmnFile.h"


#ifdef _WIN32
#define RTPPARSER_DLL_NAME _T("voAtscmhParser.dll")
#elif defined LINUX
#define RTPPARSER_DLL_NAME _T("libvoAtscmhParser.so")
#endif

typedef VO_U32 (VO_API * VOGETPARSERAPI) (VO_PARSER_API * pFunc);


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

	sampIdx = ((config[0] & 7) << 1) | (config[1] >> 7);
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

CRTPSourceWarper::CRTPSourceWarper()
{
	m_RtpSource = NULL;
#ifdef _WIN32
	m_hDumpFile = INVALID_HANDLE_VALUE;
#endif
	memset(m_AudioExData,0x00,2);
	memset(&m_ParserFunc,0x00,sizeof(VO_PARSER_API));
	memset(&m_ParserInfo,0x00,sizeof(VO_PARSER_INIT_INFO));
	memset(&m_SrcInitInfo,0x00,sizeof(VO_RTPSRC_INIT_INFO));
	memset(&m_SrcWraperInfo,0x00,sizeof(VO_RTPSRCWRAP_INIT_INFO));
	LoadRtpParser();

}
CRTPSourceWarper::~CRTPSourceWarper()
{
}
VO_U32 CRTPSourceWarper::voRtpSrcOpen(VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData)
{
	
	ParserOpen(&m_pParser);

	m_SrcWraperInfo.pProc = fSendData;
	m_SrcWraperInfo.pUserData = pUserData;
	m_SrcWraperInfo.fStatus = fStatus;
	///<Process temporarily 
	///< Read SDP File to simulate receiving the SDP info,
	VO_PBYTE	pSDPData;
	int nRet = ReadSDPData(_T("E:\\Task\\ATSC-MH\\22.2.sdp"), &pSDPData);
//	int nRet = ReadSDPData(_T("C:\\ATSC-MH\\22.2.sdp"), &pSDPData);
	if (nRet != 0)
	{
		return nRet;
	}

	if(!InitWinsock())
		return VORC_RTPSRC_BAD_NETWORK;

	m_RtpSource = new CRTPSource();
	if(m_RtpSource == NULL)
		return VORC_RTPSRC_OUT_OF_MEMORY;

	VO_RTPSRC_INIT_INFO pInitInfo;
	pInitInfo.pProc = OnSourceDataProc;
	pInitInfo.pUserData = this;

	VORC_RTPSRC rc = m_RtpSource->Open((char *)pSDPData, &pInitInfo);
	if(rc != VORC_RTPSRC_OK)
	{
		SAFE_DELETE(m_RtpSource);
		SAFE_DELETE(pSDPData);
		return rc;
	}
	SAFE_DELETE(pSDPData);
	return VO_ERR_LIVESRC_OK;
}
VO_U32 CRTPSourceWarper::voRtpSrcClose()
{
	if(m_RtpSource == NULL)
		return VORC_RTPSRC_INVALID_ARG;

	VORC_RTPSRC rc = m_RtpSource->Close();
	SAFE_DELETE(m_RtpSource);
	
	CleanupWinsock();

	ParserClose(m_pParser);

	
	return VORC_RTPSRC_OK;
}
VO_U32 CRTPSourceWarper::voRtpSrcScan(VO_LIVESRC_SCAN_PARAMEPTERS* pParam)
{
	return 0;
}
VO_U32 CRTPSourceWarper::voRtpSrcCancel(VO_U32 nID)
{
	return 0;
}
VO_U32 CRTPSourceWarper::voRtpSrcStart()
{
	if(m_RtpSource == NULL)
		return VORC_RTPSRC_INVALID_ARG;

	return m_RtpSource->Start();
}

VO_U32 CRTPSourceWarper::voRtpSrcPause()
{
	if(m_RtpSource == NULL)
		return VORC_RTPSRC_INVALID_ARG;

	return 0;
}

VO_U32 CRTPSourceWarper::voRtpSrcStop()
{
	if(m_RtpSource == NULL)
		return VORC_RTPSRC_INVALID_ARG;

	return m_RtpSource->Stop();
}
VO_U32 CRTPSourceWarper::voRtpSrcSetDataSource(VO_PTR pSource, VO_U32 nFlag)
{
	return 0;
}
VO_U32 CRTPSourceWarper::voRtpSrcSeek(VO_U64 nTimeStamp)
{
	return 0;
}
VO_U32 CRTPSourceWarper::voRtpSrcGetChannel(VO_U32* pCount, VO_LIVESRC_CHANNELINFO** ppChannelInfo)
{
	return 0;
}
VO_U32 CRTPSourceWarper::voRtpSrcSetChannel(VO_S32 nChannelID)
{
	return 0;
}
VO_U32 CRTPSourceWarper::voRtpSrcGetESG(VO_S32 nChannelID, VO_LIVESRC_ESG_INFO** ppESGInfo)
{
	return 0;
}
VO_U32 CRTPSourceWarper::voRtpSrcGetParam(VO_U32 nParamID, VO_PTR pParam)
{
	if(m_RtpSource == NULL)
		return VORC_RTPSRC_INVALID_ARG;

	return m_RtpSource->GetParam(nParamID, pParam);
}
VO_U32 CRTPSourceWarper::voRtpSrcSetParam(VO_U32 nParamID, VO_PTR pParam)
{
	if(m_RtpSource == NULL)
		return VORC_RTPSRC_INVALID_ARG;

	return m_RtpSource->SetParam(nParamID, pParam);
}

VO_U32 CRTPSourceWarper::ReadSDPData(VO_PTCHAR pPath, VO_PBYTE* ppData)
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

VO_U32	CRTPSourceWarper::LoadRtpParser(void)
{
	vostrcpy(m_szDllFile, RTPPARSER_DLL_NAME);
	vostrcpy(m_szAPIName, _T("voGetParserAPI"));

	VO_U32 rc =  CDllLoad::LoadLib(NULL);

	if(rc == 1)
	{
		VOGETPARSERAPI pAPI = (VOGETPARSERAPI) m_pAPIEntry;
		pAPI (&m_ParserFunc);
	}

	return rc;
}
#ifdef _WIN32
void CRTPSourceWarper::DumpData(PBYTE pdata,DWORD dwSize)
{
	if (INVALID_HANDLE_VALUE == m_hDumpFile)
	{
		m_hDumpFile = CreateFile(L"E:\\ATSCVideo",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,0,NULL);
	}
	
	if (m_hDumpFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL,L"Open File",L"Error",MB_OK);
	}
	DWORD dwWriteSize = 0;
	bool beSuc = WriteFile(m_hDumpFile,pdata,dwSize,&dwWriteSize,NULL);
	if (false == beSuc || dwWriteSize != dwSize)
	{
		MessageBox(NULL,L"Write File",L"Error",MB_OK);
	}

}
#endif
void CRTPSourceWarper::OnSourceData(VO_RTPSRC_DATA * pData)
{
	if (m_pParser == NULL)
		return;

	VO_PARSER_INPUT_BUFFER inputBuf;
	inputBuf.pBuf = (VO_PBYTE)pData->pData;
	inputBuf.nBufLen = pData->nDataSize;
	inputBuf.nStreamID = pData->nChannelID;
	ParserProcess(m_pParser,&inputBuf);
	
}

void CRTPSourceWarper::OnParsedData(VO_PARSER_OUTPUT_BUFFER * pData)
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

			if (pFrame->nCodecType == VO_AUDIO_CodingAAC && pFrame->nPos == VO_MTV_FRAME_POS_WHOLE/*VO_MTV_FRAME_POS_MID*/)
			{
				int nADTSheaderSize = 7;
				VO_BYTE *	pNewData = new VO_BYTE[pFrame->nSize + 7];
				Config2ADTSHeader(m_AudioExData, 2, pFrame->nSize, pNewData, (unsigned int *)&nADTSheaderSize);
				VO_BYTE *	pOldData = pFrame->pData;
				int		nOldSize = pFrame->nSize;
				memcpy(pNewData + 7, pFrame->pData, pFrame->nSize);
				pFrame->pData = pNewData;
				pFrame->nSize += 7;
				{
					VO_LIVESRC_SAMPLE sample;
					sample.nTrackID = 0;
					sample.nCodecType = pFrame->nCodecType;
					sample.Sample.Buffer = pFrame->pData;
					sample.Sample.Size = pFrame->nSize;
					sample.Sample.Time = pFrame->nStartTime;
					m_SrcWraperInfo.pProc(m_SrcWraperInfo.pUserData,VO_LIVESRC_OUTPUT_AUDIO,&sample);
					//					srcSample
				}
				pFrame->pData = pOldData;
				pFrame->nSize = nOldSize;
				delete[] pNewData;
			}
			else if (pFrame->nCodecType == VO_AUDIO_CodingDRA)
			{
				;// To be added
			}
		}
		break;
	case VO_PARSER_OT_VIDEO :
		{
			VO_MTV_FRAME_BUFFER * pFrame = (VO_MTV_FRAME_BUFFER *)(pData->pOutputData);
			{
				VO_LIVESRC_SAMPLE sample;
				sample.nTrackID = 0;
				sample.nCodecType = pFrame->nCodecType;
				sample.Sample.Buffer = pFrame->pData;
				sample.Sample.Size = pFrame->nSize;
				sample.Sample.Time = pFrame->nStartTime;
				m_SrcWraperInfo.pProc(m_SrcWraperInfo.pUserData,VO_LIVESRC_OUTPUT_VIDEO,&sample);
#if (defined _WIN32 && defined _DEBUG)
				DumpData(pFrame->pData,pFrame->nSize);
#endif
				
			}
		}
		break;
	default :
		break;
	}
}

VO_U32 CRTPSourceWarper::ParserOpen(VO_PTR * ppHandle)
{
	m_ParserInfo.pProc = OnParsedDataProc;
	m_ParserInfo.pUserData = this;
	if (m_ParserFunc.Open)
		return m_ParserFunc.Open(ppHandle,&m_ParserInfo);
	return -1;
}

VO_U32 CRTPSourceWarper::ParserProcess(VO_PTR pHandle, VO_PARSER_INPUT_BUFFER* pBuffer)
{
	if (m_ParserFunc.Process && pHandle)
		return m_ParserFunc.Process(pHandle,pBuffer);
	return -1;
}

VO_U32 CRTPSourceWarper::ParserClose(VO_PTR pHandle)
{
	if (m_ParserFunc.Close && pHandle)
		return m_ParserFunc.Close(pHandle);
	return -1;
}

VO_U32 CRTPSourceWarper::ParserGetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	if (m_ParserFunc.GetParam && pHandle)
		return m_ParserFunc.GetParam(pHandle,uID,pParam);
	return -1;
}

VO_U32 CRTPSourceWarper::ParserSetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	if (m_ParserFunc.SetParam && pHandle)
		return m_ParserFunc.SetParam(pHandle,uID,pParam);
	return -1;
}

