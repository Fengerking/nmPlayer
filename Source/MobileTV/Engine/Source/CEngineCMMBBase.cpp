#include <stdio.h>
#include <stdlib.h>

#include "CEngineCMMBBase.h"
#include "voOSFunc.h"
#include "cmnFile.h"
#include "voLog.h"

#pragma warning (disable : 4996)

#ifdef _CA_BIGCARD
extern TCHAR g_sCardName[];
#endif

VO_U32		g_nTick;
VO_TCHAR	g_szTextOut[128];

#ifdef LINUX
#define _ttoi	atoi
#endif

/*
#define DEBUG_ENG_LOG(strName)	\
	g_nTick = GetTickCount(); \
	_stprintf(g_szTextOut, _T("[ENG] [%d] "), g_nTick); \
	vostrcat(g_szTextOut, strName); \
	vostrcat(g_szTextOut, _T(" \n")); \
	OutputDebugString(g_szTextOut); \
*/
void OnParsedDataProc(VO_PARSER_OUTPUT_BUFFER* pData)
{
	((CEngineCMMBBase*)(pData->pUserData))->OnParsedData(pData);
}

CEngineCMMBBase::CEngineCMMBBase(void)
: m_bCancelScan (false)
, m_bStop (true)
, m_bServiceSelected (false)
, m_bPause (false)
, m_bPauseConfirmed (true)
, m_bWaitForGetServiceDone (false)
, m_pChannelInfo (NULL)
, m_nChannelCount (0)
, m_nLastChannel (-1)
, m_nCurChannel (-1)
, m_pChannelESGNum (NULL)
, m_ppESGInfo (NULL)
, m_bScanning (false)
, m_nFreqNum (23)
, m_nFreqStart (32)
, m_nFreqEnd (48)
, m_nCurStreamType (STREAM_TYPE_NULL)
, m_bGotServiceInfo (false)
, m_pESGTemp(NULL)
, m_nESGEntryNum(0)
, m_fESGReceived(false)
, m_nContServiceCount (0)
, m_pContService (NULL)
, m_nServiceInfoCount (0)
, m_pServiceInfo (NULL)
, m_nEMMService (0)
, m_pCAS (NULL)
, m_nCATSize (0)
, m_pDecryptBuffer (NULL)
, m_nDecryptBufferSize (0)
//, m_fSendDataEx (NULL)
{
	m_CmmbEsgInfo.nCount	= 0;
	m_CmmbEsgInfo.pItem		= NULL;

	m_TotalEsgInfo.tEngine	= VO_LIVESRC_FT_CMMB;
	m_TotalEsgInfo.pEsgInfo	= (VO_PTR)&m_CmmbEsgInfo;

	vostrcpy(m_szCAID, _T("N/A"));

#ifdef _WIN32
	// Get the full path of stored channel info file
	GetModuleFileName (NULL, m_szAppPath, MAX_PATH);
	TCHAR * pPos = _tcsrchr (m_szAppPath, _T('\\'));
	*(pPos + 1) = 0;
	_tcscpy (m_szChannelFilePath, m_szAppPath);
	_tcscat (m_szChannelFilePath, _T("voChannelInfo.Dat"));
#endif
#if defined LINUX
	voOS_GetModuleFileName(NULL, m_szAppPath, sizeof(m_szAppPath));
	VO_TCHAR * pPos = vostrrchr (m_szAppPath, _T('/'));
	*(pPos + 1) = 0;
	vostrcpy (m_szChannelFilePath, m_szAppPath);
	vostrcat(m_szChannelFilePath, "/voChannelInfo.dat");
#endif

	VOLOGI ("  <<< -----  %s ----- >>>\n", m_szChannelFilePath);

// 	ReadData ();

// 	OpenDevice();

	InitParser();

	m_pDecryptBuffer = new VO_BYTE[DECRYPT_BUFFER_MAXSIZE];

	/////////

#ifdef _CA_BIGCARD
	TCHAR szCAPath[MAX_PATH];
	_tcscpy(szCAPath, m_szAppPath);
	_tcscat(szCAPath, _T("CAPath.cfg"));
	HANDLE hCAFile = CreateFile (szCAPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hCAFile != INVALID_HANDLE_VALUE)
	{
		bool	bRead = true;
		VO_U32	dwRead = 0;

		char	szLine[1028];
		int nCount = ReadLine((LPBYTE)szLine, 1028, hCAFile);
		if (nCount >= 4)
		{
			memset (g_sCardName, 0, sizeof (TCHAR) * MAX_PATH);
			MultiByteToWideChar (CP_ACP, 0, szLine, -1, g_sCardName, MAX_PATH);
		}
		CloseHandle(hCAFile);
	}
#endif

}

int CEngineCMMBBase::InitParser()
{
	m_pParser = new CCMMBDemux;

	VO_PARSER_INIT_INFO	dmxInitInfo;
	memset (&dmxInitInfo, 0, sizeof (VO_PARSER_INIT_INFO));
	dmxInitInfo.pProc = OnParsedDataProc;
	dmxInitInfo.pUserData = this;
	int nRC = m_pParser->Open(&dmxInitInfo);
	m_pParser->SetCASType(CAS_TYPE);

	m_hESGParsed.Reset();

	return VO_ERR_LIVESRC_OK;
}

int CEngineCMMBBase::ReleaseParser()
{
// 	CloseHandle(m_hESGParsed);

// 	OutputDebugString(_T("[ENG] To delete m_pParser.\n"));
	delete m_pParser;
//	OutputDebugString(_T("[ENG] after delete m_pParser.\n"));

	return VO_ERR_LIVESRC_OK;
}

int CEngineCMMBBase::ReadLine(VO_PBYTE pData, int nDataLen, VO_PTR hFile)
{
	memset (pData, 0, nDataLen);

	VO_PBYTE	pPos = pData;
	VO_U32	dwRead = 0;

	while (pPos - pData < nDataLen)
	{
		dwRead = cmnFileRead(hFile, pPos, 1);

		if (dwRead != 1)
			return -1;

		if (*pPos == '\r')
		{
			pPos++;
			dwRead = cmnFileRead(hFile, pPos, 1);

			if (dwRead != 1)
				return -1;

			if (*pPos == '\n')
			{
				// remove \r\n
				*(pPos - 1) = 0;
				break;
			}

			pPos++;
		}

		pPos++;
	}

	return pPos - pData;
}

int CEngineCMMBBase::Open(void * pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData)
{
	m_pUserData = pUserData;
	m_fStatus = fStatus;
	m_fSendData = fSendData;

	ReadData();

	return VO_ERR_LIVESRC_OK;
}

CEngineCMMBBase::~CEngineCMMBBase(void)
{
// 	Stop();
// 
	WriteCurChannel ();
// 
	ReleaseParser();
// 
// 	CloseDevice();
// 
	ReleaseData();

	delete m_pDecryptBuffer;

	VOLOGI("[ENG] CMMB engine stopped.");
}

int CEngineCMMBBase::OpenDevice()
{
	if (m_pCAS == NULL)
	{
		m_pCAS = (CCASBase *)(new CAS_CLASS());
		m_pCAS->InitializeCAS(NULL);
		m_pCAS->GetParam(CAS_PARAM_GETKDAVERSION, (int *)m_szKDAversion);
		voOS_Sleep(100);
		m_pCAS->GetCASInfo(m_szCAID);
	}

	return VO_ERR_LIVESRC_OK;
}

int CEngineCMMBBase::CloseDevice()
{
	if (m_pCAS != NULL)
	{
		m_pCAS->TerminateCAS();
		delete m_pCAS;
		m_pCAS = NULL;
	}

	return VO_ERR_LIVESRC_OK;
}

int CEngineCMMBBase::PreScan()
{
	return VO_ERR_LIVESRC_OK;
}

int CEngineCMMBBase::PostScan()
{
	return VO_ERR_LIVESRC_OK;
}

int CEngineCMMBBase::DeviceScan()
{
	return VO_ERR_LIVESRC_FAIL;
}

int CEngineCMMBBase::Scan(void * pParam, bool nQuickScan)
{
	if (PreScan() != VO_ERR_LIVESRC_OK)
		return VO_ERR_LIVESRC_FAIL;

	InitData();
//	WriteData();	// MOdify later

	//Init the freq range
	m_nFreqStart = pParam ? ((VO_LIVESRC_CMMB_SCANPARAM *)pParam)->nStartFreq : FREQSTART_DEFAULT;
	m_nFreqEnd = pParam? ((VO_LIVESRC_CMMB_SCANPARAM *)pParam)->nEndFreq : FREQEND_DEFAULT;
	VOLOGI("[ENG] Scan frequency point from %d to %d.", m_nFreqStart, m_nFreqEnd);

	m_bCancelScan = false;
	m_nCurStreamType = STREAM_TYPE_TS0;
	DeviceScan();
	m_nCurStreamType = STREAM_TYPE_NULL;
	if (m_bCancelScan)
	{
		ReleaseData();
		ReadData();
		return m_nChannelCount;
	}

	PostScan();
	ReceiveESGInfo();
	UpdateChannel();
	UpdateSchedule();

	WriteData();

	return m_nChannelCount;
}

int	CEngineCMMBBase::ReceiveESGInfo()
{
	if (m_nContServiceCount <= 0)
		return VO_ERR_LIVESRC_FAIL;

	int nIndex;
	for ( nIndex = 0; nIndex < m_nContServiceCount; nIndex++)
	{
		if (m_pContService[nIndex].service_id != 254)
			continue;

		if (m_fStatus != NULL)
			m_fStatus (m_pUserData, VO_LIVESRC_STATUS_RECEIVING_ESG, 0, 0);

		m_hESGParsed.Reset();
		m_nCurStreamType = STREAM_TYPE_ESG;
		SetDeviceChannel(MAKE_CHANNELID(m_pContService[nIndex].freq, m_pContService[nIndex].service_id));
		m_nFreqNum = m_pContService[nIndex].freq;
		m_bStop = false;
		VO_U32 dwRC = m_hESGParsed.Wait(8000);
		m_bStop = true;
		m_nCurChannel = -1;
		m_nCurStreamType = STREAM_TYPE_NULL;
	}

	if (nIndex == m_nContServiceCount)
		return VO_ERR_LIVESRC_FAIL;

	if (/*dwRC == WAIT_TIMEOUT &&*/ m_nESGEntryNum == 0)
		return VO_ERR_LIVESRC_FAIL;

	return VO_ERR_LIVESRC_OK;
}

int CEngineCMMBBase::UpdateSchedule()
{
	VOLOGI("[ENG] ESG entry count : %d <-------------------------", m_nESGEntryNum);

	if (m_nESGEntryNum <= 0)
		return VO_ERR_LIVESRC_FAIL;

	m_ppESGInfo = new VO_CMMB_ESG_ITEM * [m_nChannelCount];
	memset (m_ppESGInfo, 0, sizeof (int) * m_nChannelCount);

	// Count ESG entry number for each service
	memset(m_pChannelESGNum, 0, sizeof(int) * m_nChannelCount);
	for (int i = 0; i < m_nESGEntryNum; i++)
	{
		int nIndex = FindChannelIndex(m_pESGTemp[i]->nChannelID);
		if (nIndex >= 0)
			m_pChannelESGNum[nIndex] ++;
	}
	// Create the 
	for (int i = 0; i < m_nChannelCount; i ++)
	{
		VOLOGI("[ESG] Service %d ESG entry count : %d ", GET_SERVICEID(m_pChannelInfo[i].nChannelID), m_pChannelESGNum[i]);

		if (m_pChannelESGNum[i] > 0)
			m_ppESGInfo[i] = new VO_CMMB_ESG_ITEM [m_pChannelESGNum[i]];
	}

	memset(m_pChannelESGNum, 0, sizeof(int) * m_nChannelCount);
	for (int esg = 0; esg < m_nESGEntryNum; esg ++)
	{
		int ch = FindChannelIndex(m_pESGTemp[esg]->nChannelID);
		if (ch >= 0)
		{
			VO_CMMB_ESG_ITEM * pESG = &m_ppESGInfo[ch][m_pChannelESGNum[ch]];
			pESG->nChannelID	= m_pESGTemp[esg]->nChannelID;
			pESG->nIndex		= m_pESGTemp[esg]->nIndex;
			memcpy(&(pESG->nStartTime), &(m_pESGTemp[esg]->nStartTime), sizeof(VODATETIME));
			pESG->nDuration		= m_pESGTemp[esg]->nDuration;
			pESG->pData			= m_pESGTemp[esg]->pData;
			_tcscpy(pESG->szName, m_pESGTemp[esg]->szName);

			m_pChannelESGNum[ch] ++;
		}
	}

	return VO_ERR_LIVESRC_OK;
}

int CEngineCMMBBase::SetChannel(int nNewChannelID)
{
	if (m_nChannelCount <= 0)
		return VO_ERR_LIVESRC_FAIL;

	int nNewIndex = -1;
	if (nNewChannelID >= 0)
	{
		nNewIndex = FindChannelIndex(nNewChannelID);
		if (nNewIndex < 0 )
			return VO_ERR_LIVESRC_FAIL;	// Invalid channel ID
	}
	if (nNewChannelID < 0 && m_nCurChannel >= 0)
		return VO_ERR_LIVESRC_OK;	// Already selected a channel before. No change
	if (nNewChannelID < 0 && m_nCurChannel < 0)
	{	// First time to select channel in this session
		if ((nNewIndex = FindChannelIndex(m_nLastChannel)) < 0)
		{
			nNewIndex = 0;
			nNewChannelID = m_pChannelInfo[0].nChannelID;
		}
		else
		{
			nNewIndex = FindChannelIndex(m_nLastChannel);
			nNewChannelID = m_nLastChannel;
		}
	}

	bool bSwitch = false; // To mark if switching between TV and radio channel
	if (m_nCurChannel >= 0)
	{
		int nCurIndex = FindChannelIndex(m_nCurChannel);
		if (m_pChannelInfo[nCurIndex].nType == 1 && m_pChannelInfo[nNewIndex].nType == 2
			|| m_pChannelInfo[nCurIndex].nType == 2 && m_pChannelInfo[nNewIndex].nType == 1)
			bSwitch = true;
	}

	m_bPause = true;
	if (!m_bPauseConfirmed)
		voOS_Sleep(10);

	if (m_fStatus != NULL)
		m_fStatus (m_pUserData, VO_LIVESRC_STATUS_CHANNEL_STOP, VO_LIVESRC_FT_CMMB, bSwitch ? VO_LIVESRC_FT_FILE : VO_LIVESRC_FT_CMMB);

	VOLOGI("[ENG] Switch to channel [%d]:[%d] ...", GET_FREQINDEX(nNewChannelID), GET_SERVICEID(nNewChannelID));

	m_nCurStreamType = STREAM_TYPE_SERVICE;
	if (VO_ERR_LIVESRC_OK == SetDeviceChannel(nNewChannelID))
	{
		m_nCurChannel = nNewChannelID;
		m_nFreqNum = GET_FREQINDEX(nNewChannelID);
		for (int i=0; i<m_nChannelCount; i++)
			m_pChannelInfo[i].nCurrent = (i == nNewIndex) ? 1 : 0;

		if (m_fStatus != NULL)
			m_fStatus (m_pUserData, VO_LIVESRC_STATUS_CHANNEL_START, bSwitch ? VO_LIVESRC_FT_FILE : VO_LIVESRC_FT_CMMB, VO_LIVESRC_FT_CMMB);

		m_bPause = false;
		return VO_ERR_LIVESRC_OK;
	}
	else
		return VO_ERR_LIVESRC_FAIL;
}

// This function should be called only if the CMMB demodulator has 2 phisical channels, which means no way to receive TS0 at the same time of receiving CA content
// Application level need to decide the frequency of calling this function. And no other service will be received during this function.
int CEngineCMMBBase::EBQuery(void)
{
	int nOldStreamType = m_nCurStreamType;
	m_nCurStreamType = STREAM_TYPE_EB;
	SetTS0Channel();
	voOS_Sleep(1000);
	SetDeviceChannel(m_nCurChannel);
	m_nCurStreamType = nOldStreamType;
	return VO_ERR_LIVESRC_OK;
}

void CEngineCMMBBase::SendData(int packetType, unsigned char * pData, int nDataLen, VO_U64 nTimeStamp, int nCodecType, int nStreamId, bool bKeyFrame)
{
	if (!m_bStop)
	{
		if (m_bPause)
		{
			m_bPauseConfirmed = true;	// To guarantee no data sent out after "pause"
		}
		else
		{
			m_bPauseConfirmed = false;
			if (m_fSendData)
			{
// 				VOMTPFRAMEBUFFER * pFrame = (VOMTPFRAMEBUFFER *)pData;
// 				TCHAR szType[8] = _T("N/A");
// 				if (packetType == (PACKET_AUDIO | VOCODEC_AAC))
// 					_tcscpy(szType, _T("AAC"));
// 				else if (packetType == (PACKET_AUDIO | VOCODEC_DRA))
// 					_tcscpy(szType, _T("DRA"));
// 				else if (packetType == (PACKET_VIDEO | VOCODEC_H264))
// 					_tcscpy(szType, _T("H264"));
// 				int len = pFrame->nSize;
// 				_stprintf(m_szDebugString, _T("[ENG] %s Data [%d][%02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X]\n"),
// 					szType, len, pFrame->pData[0], pFrame->pData[1], pFrame->pData[2], pFrame->pData[3], pFrame->pData[4],
// 					pFrame->pData[len-5], pFrame->pData[len-4], pFrame->pData[len-3], pFrame->pData[len-2], pFrame->pData[len-1]);
// 				if (packetType != (PACKET_VIDEO | VOCODEC_H264))
// 					OutputDebugString(m_szDebugString);
				
				m_AVSample.Buffer			= pData;
				m_AVSample.Size				= nDataLen;
				m_AVSample.Time				= nTimeStamp;

				m_LiveSrcSample.nCodecType	= nCodecType;
				m_LiveSrcSample.nTrackID	= nStreamId;
				m_LiveSrcSample.pSample		= &m_AVSample;
				m_fSendData(m_pUserData, packetType, &m_LiveSrcSample);
			}
		}
	}
}

void CEngineCMMBBase::OnParsedData(VO_PARSER_OUTPUT_BUFFER * pData)
{
	if (pData == NULL) 
		return;

	VO_PARSER_OUTPUT_TYPE nInfoType = (VO_PARSER_OUTPUT_TYPE)pData->nType;
	VO_VOID * pInfoBuf = pData->pOutputData;
 	if (nInfoType != VO_PARSER_OT_CMMB_FAILED && nInfoType != VO_PARSER_OT_CMMB_ESG_PARSE_FINISHED && pInfoBuf == NULL) 
		return;

	switch (nInfoType)
	{
	case VO_PARSER_OT_CMMB_FAILED : return;
		break;
	case VO_PARSER_OT_CMMB_TS0_INFO :
		{
			VOTS0INFO * pTS0Info = (VOTS0INFO *)pInfoBuf;
			m_nCurNetworkID = pTS0Info->nit_info.net_id;

			if (m_nCurChannel <= 0)
			{
				for (int i = 0; i < pTS0Info->continue_service_count; i++)
					if (m_nCurStreamType == STREAM_TYPE_TS0)
						AddNewService(&(pTS0Info->continue_service[i]));
			}

			if (pTS0Info->eb.eb_data_segment.eb_data_len > 0 && m_nCurStreamType == STREAM_TYPE_EB)
			{
				if (m_fStatus)
					m_fStatus(m_pUserData, VO_LIVESRC_STATUS_EB_RECEIVED, 0, (int)&(pTS0Info->eb));
			}
			if (pTS0Info->ca_info.ca_service_count > 0 && m_nCurStreamType == STREAM_TYPE_TS0)
			{
				m_nEMMService = pTS0Info->ca_info.ca_service_id[0];
				DecryptData(1, &(pTS0Info->ca_info));
			}
		}
		break;
	case VO_PARSER_OT_CMMB_SERVICE_INFO :
		if (m_nCurStreamType == STREAM_TYPE_ESG )
			AddNewServiceInfo((VOSERVICEINFO *)pInfoBuf);
		break;
	case VO_PARSER_OT_CMMB_SCHEDULE_INFO :
		if (m_nCurStreamType == STREAM_TYPE_ESG )
			AddNewSchedule((VOSCHEDULEINFO *)pInfoBuf);
		break;
	case VO_PARSER_OT_CMMB_SERVICE_AUX_INFO :
		break;
	case VO_PARSER_OT_CMMB_SERVICE_PARAM_INFO :
		break;
	case VO_PARSER_OT_CMMB_CONTENT_INFO :
		break;
	case VO_PARSER_OT_CMMB_PROGRAM_GUIDE :
		{
			VOPROGRAMGUIDE * pPG = (VOPROGRAMGUIDE *)pData->pOutputData;
			for (int i=0; i<pPG->count; i++)
			{
				VOLOGI("[ENG] PG : [%s] %d.%d.%d %d:%d", pPG->guide[i].theme,
					pPG->guide[i].start_time.wYear, pPG->guide[i].start_time.wMonth, pPG->guide[i].start_time.wDay,
					pPG->guide[i].start_time.wHour, pPG->guide[i].start_time.wMinute);
			}
			if (m_fStatus)
				m_fStatus(m_pUserData, VO_LIVESRC_STATUS_PROGRAMGUIDE_RECEIVED, 0, (int)pPG->guide);
		}
		break;
	case VO_PARSER_OT_CMMB_ESG_PARSE_FINISHED :
		{
			VOLOGI("[ENG] VO_PARSER_OT_CMMB_ESG_PARSE_FINISHED received.");
			m_hESGParsed.Signal();
		}
		break;
	case VO_PARSER_OT_STREAMINFO :
		{
			int n = 0;
		}
		break;
	case VO_PARSER_OT_AUDIO :
		{
			VO_MTV_FRAME_BUFFER * pFrame = (VO_MTV_FRAME_BUFFER *)(pData->pOutputData);
			pFrame->nStartTime = pFrame->nStartTime * 10 / 225;
			if (pFrame->nCodecType == VO_AUDIO_CodingAAC)
			{
				VO_BYTE *	pNewData = new VO_BYTE[pFrame->nSize + 7];
				int		nSampleRate = 6;
				int		adtsHeaderSize = 7;
				pNewData[0] = 0xFF;	
				pNewData[1] = 0xF9;	
				pNewData[2] = (0x1 << 6) | (nSampleRate & 0xF) << 2 ;	
				pNewData[3] = 0x80;	
				pNewData[4] = ((pFrame->nSize + adtsHeaderSize) >> 3) & 0xFF;
				pNewData[5] = ((pFrame->nSize + adtsHeaderSize) & 0x7) << 5 | 0x1F;
				pNewData[6] = 0xFC;

				VO_BYTE *	pOldData = pFrame->pData;
				int		nOldSize = pFrame->nSize;
				memcpy(pNewData + 7, pFrame->pData, pFrame->nSize);
				pFrame->pData = pNewData;
				pFrame->nSize += adtsHeaderSize;
				SendData(VO_LIVESRC_OUTPUT_AUDIO, pFrame->pData, pFrame->nSize, pFrame->nStartTime, pFrame->nCodecType, 0, pFrame->nFrameType==0);
				pFrame->pData = pOldData;
				pFrame->nSize = nOldSize;
				delete pNewData;
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
			pFrame->nStartTime = pFrame->nStartTime * 10 / 225;
			SendData(VO_LIVESRC_OUTPUT_VIDEO, pFrame->pData, pFrame->nSize, pFrame->nStartTime, pFrame->nCodecType, 0, pFrame->nFrameType==0);
		}
		break;
	case VO_PARSER_OT_CMMB_PACKET_DATA :
		{
			VODATABUFFER * pDataBuffer = (VODATABUFFER *)(pData->pOutputData);

			//if (m_fSendDataEx != NULL)
				//m_fSendDataEx(m_pUserData, pDataBuffer->data, pDataBuffer->data_len, 0);
		}
		break;
	case VO_PARSER_OT_CMMB_DESCRAMBLING :
		{
			VODESCRAMBLING * pCAData = (VODESCRAMBLING *)pInfoBuf;
			DecryptData(0, pCAData);
		}
	default :
		break;
	}
}

int CEngineCMMBBase::DecryptData(int nCAT, void * pDataPtr)
{
	if (m_pCAS == NULL)
		return -1;

	int nRet = 0;
	if (nCAT == 1)
	{
		VOCAINFO * pCAInfo = (VOCAINFO *)pDataPtr;

#ifdef CAS_TIANYU
		unsigned short * pCATData = (unsigned short *)m_CATData;
		for (int m = 0; m < pCAInfo->ca_system_count; m++)
			pCATData[m] = (unsigned short)pCAInfo->ca_system_id[m];
		m_nCATSize = pCAInfo->ca_system_count * 2;
#else
		m_nCATSize = pCAInfo->data_len;
		memcpy(m_CATData, pCAInfo->data_ptr, m_nCATSize);
#endif
		m_pCAS->SetCASParam(m_CATData, m_nCATSize, m_nCurNetworkID);
		int nCASID;
		m_pCAS->GetParam(CAS_PARAM_GETCASID, &nCASID);
		for (int i=0; i< pCAInfo->ca_system_count; i++)
		{
			if (pCAInfo->ca_system_id[i] == nCASID)
			{
				m_nEMMService = pCAInfo->ca_service_id[i]; 
				VOLOGI("[ENG] EMM service ID : %d", m_nEMMService);
				break;
			}
		}
	}
	else
	{
		VODESCRAMBLING * pData = (VODESCRAMBLING *)pDataPtr;
		switch (pData->type)
		{
		case MULTIPLEX_SEGMENT_ECM :
			{
				int bRet = m_pCAS->SendExMData(pData->encrypt_data_ptr, pData->encrypt_data_len, 
					pData->ecm_emm_type, GET_SERVICEID(m_nCurChannel));
			}
			break;
		case MULTIPLEX_SEGMENT_EMM :
			{
				int bRet = m_pCAS->SendExMData(pData->encrypt_data_ptr, pData->encrypt_data_len, 
					pData->ecm_emm_type, m_nEMMService);
			}
			break;
		case MULTIPLEX_SEGMENT_DATA :
		case MULTIPLEX_SEGMENT_AUDIO :
		case MULTIPLEX_SEGMENT_VIDEO :
			{
				int nType = pData->type;
				pData->encrypt = VO_FALSE;
				m_nDecryptBufferSize = pData->encrypt_data_len;
				nRet = m_pCAS->DescrambleData(pData->encrypt_data_ptr, pData->encrypt_data_len, 
												m_pDecryptBuffer, &m_nDecryptBufferSize, nType);
				if (nRet == 0 && m_nDecryptBufferSize > 0 && m_nDecryptBufferSize < 65535)
				{
					if (nType == 2)
					{
						pData->decrypt_data_ptr = m_pDecryptBuffer + 3;
						pData->decrypt_data_len = m_nDecryptBufferSize - 3;
					}
					else if (nType == 1)
					{
						pData->decrypt_data_ptr = m_pDecryptBuffer + 3;
						pData->decrypt_data_len = m_nDecryptBufferSize - 3;
						*(pData->decrypt_data_ptr) = pData->decrypt_data_len - 1;
					}
					else 
					{
						pData->decrypt_data_ptr = m_pDecryptBuffer;
						pData->decrypt_data_len = m_nDecryptBufferSize;
					}
					pData->encrypt = VO_TRUE;
				}
			}
			break;
		case WHOLE_MSF :
			break;
		}
	}

	return 0;
}

int CEngineCMMBBase::SetDeviceChannel(int nNewChannelID)
{
	return VO_ERR_LIVESRC_FAIL;
}

int CEngineCMMBBase::Cancel (int nID)
{
	m_bCancelScan =  true;

	return VO_ERR_LIVESRC_OK;
}

int CEngineCMMBBase::Start (void)
{
	m_bStop = false;

	return VO_ERR_LIVESRC_OK;
}

int CEngineCMMBBase::Stop (void)
{
	m_bStop = true;

	return VO_ERR_LIVESRC_OK;
}

int CEngineCMMBBase::GetSignalStrength(void)
{
/*
	int ret;
	TP_INT16 SignalStrength;
	TP_UINT32 Ldpc_Total, Ldpc_Err, SignalPower,NoisePower;
	TP_UINT16 SyncFlag;

	ret = tpTvGetSignalPower(&SignalStrength, &Ldpc_Total, &Ldpc_Err, &SignalPower, &NoisePower, &SyncFlag);

	return SignalStrength;
*/
	return 0;
}

int CEngineCMMBBase::GetChannel(int * pCount, VO_LIVESRC_CHANNELINFO ** ppChannelInfo)
{
	*pCount = m_nChannelCount;
	*ppChannelInfo = m_pChannelInfo;

	if (m_nChannelCount > 0)
		return VO_ERR_LIVESRC_OK;
	else
		return VO_ERR_LIVESRC_NOCHANNEL;
}

int CEngineCMMBBase::GetESG (int nChannelID, VO_LIVESRC_ESG_INFO* pESGInfo)
{
	if (m_nChannelCount <= 0)
		return VO_ERR_LIVESRC_NOCHANNEL;

	if (m_pChannelESGNum == NULL)
		return VO_ERR_LIVESRC_NO_DATA;

	int i = FindChannelIndex(nChannelID);
	if (i < 0)
		return VO_ERR_LIVESRC_INVALIDARG;

	if (m_pChannelESGNum[i] != NULL)
		m_CmmbEsgInfo.nCount = m_pChannelESGNum[i];
	if (m_ppESGInfo != NULL)
		*m_CmmbEsgInfo.pItem = m_ppESGInfo[i];	
	
	
	pESGInfo = &m_TotalEsgInfo;

	return VO_ERR_LIVESRC_OK;
}

void CEngineCMMBBase::ReleaseData (void)
{
	m_nContServiceCount = 0;
	delete []m_pContService;
	m_pContService = NULL;

	m_nServiceInfoCount = 0;
	delete []m_pServiceInfo;
	m_pServiceInfo = NULL;

	delete []m_pChannelInfo;
	m_pChannelInfo = NULL;

	delete []m_pESGTemp;
	m_pESGTemp = NULL;
	m_nESGEntryNum = 0;

	if (m_ppESGInfo != NULL)
	{
		for (int i = 0; i < m_nChannelCount; i++)
		{
			if (m_ppESGInfo[i] != NULL)
				delete m_ppESGInfo[i];
		}

		delete []m_ppESGInfo;
		m_ppESGInfo = NULL;
	}

	if (m_pChannelESGNum != NULL)
	{
		delete []m_pChannelESGNum;
		m_pChannelESGNum = NULL;
	}

	m_nLastChannel = -1;
	m_nCurChannel = -1;
	m_nChannelCount = 0;
}

void CEngineCMMBBase::InitData(void)
{
	ReleaseData();

	m_pContService = new VOCONTINUESERVICEINFO[MAX_SERVICE_COUNT];
	m_pServiceInfo = new VOSERVICEINFO_X[MAX_SERVICE_COUNT];
	m_pChannelInfo = new VO_LIVESRC_CHANNELINFO[MAX_CHANNEL_COUNT];
	m_pChannelESGNum = new int[MAX_CHANNEL_COUNT];
	memset (m_pChannelESGNum, 0, sizeof(int) * MAX_CHANNEL_COUNT);
	m_pESGTemp = new VO_CMMB_ESG_ITEM * [MAX_ESG_ENTRY_NUM];
}

int CEngineCMMBBase::FindChannelIndex(int nChannelID)
{
	int nIndex = 0;
	for (nIndex = 0; nIndex < m_nChannelCount; nIndex ++)
		if (m_pChannelInfo[nIndex].nChannelID == nChannelID)
			break;
	if (nIndex < m_nChannelCount)
		return nIndex;
	else
		return -1;
}

int CEngineCMMBBase::FindServiceIndex(int nChannelID)
{
	int nIndex = 0;
	for (nIndex = 0; nIndex < m_nContServiceCount; nIndex ++)
		if (m_pContService[nIndex].freq == GET_FREQINDEX(nChannelID) && m_pContService[nIndex].service_id == GET_SERVICEID(nChannelID))
			break;
	if (nIndex < m_nContServiceCount)
		return nIndex;
	else
		return -1;
}

int CEngineCMMBBase::AddNewService(VOCONTINUESERVICEINFO * pContService)
{
	int nNewChannelID =  MAKE_CHANNELID(/*pContService->freq*/ m_nFreqNum, pContService->service_id);
	if (FindServiceIndex(nNewChannelID) >= 0)
		return -1;

	VOLOGI("[ENG] Service %d:%d found.", /*pContService->freq*/ m_nFreqNum, pContService->service_id);

	memcpy(&(m_pContService[m_nContServiceCount]), pContService, sizeof(VOCONTINUESERVICEINFO));
	m_pContService[m_nContServiceCount].freq = m_nFreqNum; // Freq in table may not match with real freq.
	return ++m_nContServiceCount;
}

int CEngineCMMBBase::FindServiceInfoIndex(int nChannelID)
{
	int nIndex = 0;
	for (nIndex = 0; nIndex < m_nServiceInfoCount; nIndex ++)
		if (m_pServiceInfo[nIndex].service_id == GET_SERVICEID(nChannelID))
			break;
	if (nIndex < m_nServiceInfoCount)
		return nIndex;
	else
		return -1;
}

int CEngineCMMBBase::AddNewServiceInfo(VOSERVICEINFO * pServiceInfo)
{// Only store service id, no freq point here

	if (FindServiceInfoIndex(pServiceInfo->service_id) >= 0)
		return -1;

	m_pServiceInfo[m_nServiceInfoCount].service_id = pServiceInfo->service_id;
	m_pServiceInfo[m_nServiceInfoCount].service_class = pServiceInfo->service_class;
	m_pServiceInfo[m_nServiceInfoCount].service_param_id = pServiceInfo->service_param_id;
	m_pServiceInfo[m_nServiceInfoCount].service_free = pServiceInfo->service_free;
//	m_pServiceInfo[m_nServiceInfoCount].ca_info = pServiceInfo.ca_info;
	_tcscpy(m_pServiceInfo[m_nServiceInfoCount].service_name, pServiceInfo->service_name);
	return ++m_nServiceInfoCount;
}

int CEngineCMMBBase::UpdateChannel()
{
	VOLOGI("[ENG] Filtering TV channel...");

	int nChIndex, nSerID, nChannelID, nInfoIndex;
	for (int i = 0; i < m_nContServiceCount; i ++)
	{
		nSerID = m_pContService[i].service_id;
		if (nSerID <= 0x200 || nSerID >= 0x3FFF || nSerID == 254)	
			continue;	// Invalid service
		nChannelID = MAKE_CHANNELID(m_pContService[i].freq, m_pContService[i].service_id);
		nChIndex = FindChannelIndex(nChannelID);
		if (nChIndex >= 0)	
			continue;	// Already in channel list

		nInfoIndex = FindServiceInfoIndex(nChannelID);
// 		int nClass = nInfoIndex >= 0 ? m_pServiceInfo[nInfoIndex].service_class : 0;
// 		if (m_nServiceInfoCount > 0) // Filter by service class only if the service info table is received.
// 			if (nClass != CLASS_TV && nClass != CLASS_AUDIO_BROADCAST) // Neither TV nor radio
// 				continue;	

		// Add channel
		m_pChannelInfo[m_nChannelCount].nChannelID = nChannelID;
		m_pChannelInfo[m_nChannelCount].hIcon = NULL;
		m_pChannelInfo[m_nChannelCount].nCurrent = 0;
		m_pChannelInfo[m_nChannelCount].nType = /*(VO_LIVESRC_FORMATTYPE)nClass*/VO_LIVESRC_FT_CMMB;
		m_pChannelInfo[m_nChannelCount].pData = NULL; // To be modified.
		if (nInfoIndex >= 0)
			_tcscpy (m_pChannelInfo[m_nChannelCount].szName, m_pServiceInfo[nInfoIndex].service_name);
		else
			_stprintf (m_pChannelInfo[m_nChannelCount].szName, _T("%d-%d"), GET_FREQINDEX(nChannelID), GET_SERVICEID(nChannelID));
		VOLOGI("[ENG] Channel : %d : %s", GET_SERVICEID(nChannelID), m_pChannelInfo[m_nChannelCount].szName);

		m_nChannelCount ++;
	}
	return VO_ERR_LIVESRC_OK;
}

int CEngineCMMBBase::AddNewSchedule(VOSCHEDULEINFO * pSchedule)
{
// 	_stprintf(m_szDebugString, _T("schedule #%d content #%d \n"), pSchedule->schedule_id, pSchedule->content_id);
// 	DEBUG_ENG_LOG(m_szDebugString);

	VO_CMMB_ESG_ITEM * pESG = new VO_CMMB_ESG_ITEM ();
	memset (pESG, 0, sizeof(VO_CMMB_ESG_ITEM));
	pESG->nChannelID = MAKE_CHANNELID(m_nFreqNum, pSchedule->service_id);
	pESG->nIndex = pSchedule->content_id;
	_tcscpy(pESG->szName, pSchedule->title);

// 	_stprintf(m_szDebugString, _T("[ENG] Schedule : %d %s\n"), pSchedule->service_id, pESG->szName);
// 	OutputDebugString(m_szDebugString);

	VO_TCHAR	szTemp[16];
	if (vostrlen(pSchedule->date) == 10)
	{	// Sample : "2008-09-10"
		vostrncpy(szTemp, pSchedule->date, 4); szTemp[4] = _T('\0');
		pESG->nStartTime.wYear = (VO_U16)_ttoi(szTemp);
		vostrncpy(szTemp, pSchedule->date + 5, 2); szTemp[2] = _T('\0');
		pESG->nStartTime.wMonth = (VO_U16)_ttoi(szTemp);
		vostrncpy(szTemp, pSchedule->date + 8, 2); szTemp[2] = _T('\0');
		pESG->nStartTime.wDay = (VO_U16)_ttoi(szTemp);
	}
	if (vostrlen(pSchedule->time) == 8)
	{	// Sample : "01:06:00"
		vostrncpy(szTemp, pSchedule->time, 2); szTemp[2] = _T('\0');
		pESG->nStartTime.wHour = (VO_U16)_ttoi(szTemp);
		vostrncpy(szTemp, pSchedule->time + 3, 2); szTemp[2] = _T('\0');
		pESG->nStartTime.wMinute = (VO_U16)_ttoi(szTemp);
		vostrncpy(szTemp, pSchedule->time + 6, 2); szTemp[2] = _T('\0');
		pESG->nStartTime.wSecond = (VO_U16)_ttoi(szTemp);
	}
	pESG->nDuration = 0;	// 
	pESG->pData = NULL;
	m_pESGTemp[m_nESGEntryNum] = pESG;
	m_nESGEntryNum ++;

	int i = FindChannelIndex(MAKE_CHANNELID(m_nFreqNum, pSchedule->service_id));
	if (i >= 0)
		m_pChannelESGNum[i] ++;

	pESG = NULL;

	return VO_ERR_LIVESRC_OK;
}

VO_U32 CEngineCMMBBase::GetFrequencyValue(int id)
{
	static VO_U32 freq[]=
	{
		0,   0,   0,   0, //  0 - 3
		0,   0,   0,   0, //  4 - 7
		0,   0,   0,   0, //  8 - 11
		0, 474, 482, 490, // 12 - 15
		498, 506, 514, 522, // 16 - 19
		530, 538, 546, 554, // 20 - 23
		562, 610, 618, 626, // 24 - 27
		634, 642, 650, 658, // 28 - 31
		666, 674, 682, 690, // 32 - 35
		698, 706, 714, 722, // 36 - 39
		730, 738, 746, 754, // 40 - 43
		762, 770, 778, 786, // 44 - 47
		794, 802,   0,   0, // 48 - 51
		0,   0,   0,   0,
		0,   0,   0,   0,
		0,   0,   0,   0,
		0,   0,   0,   0,
		0,   0,   0,   0,
		0,   0,   0,   0,
		0,   0,   0,   0,
		0,   0,   0,   0,
		0,   0,   0,   0,
		0,   0,   0,   0,
		0,   0,   0,   0,
		0,   0,   0,   0,
		2620,  2628,  2636 // 100 - 102
	};

	if (id > 102 || id < 0)
		return 0;
	else
		return freq[id];
}

// Read channel info from file
void CEngineCMMBBase::ReadData (void)
{
	VO_FILE_SOURCE	fileSource;
	memset(&fileSource, 0, sizeof(VO_FILE_SOURCE));
	fileSource.nFlag = VO_FILE_TYPE_NAME;
	fileSource.nOffset = 0;
	fileSource.nLength = 0xFFFFFFFF;
	fileSource.pSource = (VO_PTR)m_szChannelFilePath;
	fileSource.nMode = VO_FILE_READ_ONLY;

	VO_PTR hFile = cmnFileOpen(&fileSource);
	if (hFile == NULL)
		return;

	bool	bRead = true;
	VO_U32	dwRead = cmnFileRead (hFile, &m_nFreqNum, sizeof(int));
	if (dwRead != 4 || m_nFreqNum <= 0)
		bRead = false;
	dwRead = cmnFileRead (hFile, &m_nChannelCount, sizeof (int));
	if (dwRead != 4 || m_nChannelCount <= 0)
		bRead = false;

	int i = 0;
	int j = 0;

	if (bRead)
	{
		m_pChannelInfo = new VO_LIVESRC_CHANNELINFO[m_nChannelCount];
		for (i = 0; i < m_nChannelCount; i++)
		{
			dwRead = cmnFileRead (hFile, &m_pChannelInfo[i], sizeof (VO_LIVESRC_CHANNELINFO));
			if (dwRead != sizeof (VO_LIVESRC_CHANNELINFO))
			{
				bRead = false;
				break;
			}
		}
	}

	if (bRead)
	{
		m_pChannelESGNum = new int[m_nChannelCount];
		for (i = 0; i < m_nChannelCount; i++)
		{
			dwRead = cmnFileRead (hFile, &m_pChannelESGNum[i], sizeof (int));
			if (dwRead != sizeof (int))
			{
				bRead = false;
				break;
			}
		}
	}

	if (bRead)
	{
		m_ppESGInfo = new VO_CMMB_ESG_ITEM * [m_nChannelCount];
		for (i = 0; i < m_nChannelCount; i++)
		{
			if (m_pChannelESGNum[i] > 0)
			{
				m_ppESGInfo[i] = new VO_CMMB_ESG_ITEM[m_pChannelESGNum[i]];
				for (j = 0; j < m_pChannelESGNum[i]; j++)
				{
					dwRead = cmnFileRead (hFile, &m_ppESGInfo[i][j], sizeof (VO_CMMB_ESG_ITEM));
					if (dwRead != sizeof (VO_CMMB_ESG_ITEM))
					{
						bRead = false;
						break;
					}
				}
			}
		}
	}

	//ReadExtraData();
	dwRead = cmnFileRead (hFile, &m_nContServiceCount, sizeof (int));
	if (dwRead != 4 || m_nContServiceCount <= 0)
		bRead = false;
	if (bRead)
	{
		m_pContService = new VOCONTINUESERVICEINFO[m_nContServiceCount];
		for (i = 0; i < m_nContServiceCount; i++)
		{
			dwRead = cmnFileRead (hFile, &m_pContService[i], sizeof (VOCONTINUESERVICEINFO));
			if (dwRead != sizeof (VOCONTINUESERVICEINFO))
			{
				bRead = false;
				break;
			}
		}
	}

	if (bRead)
	{
		dwRead = cmnFileRead (hFile, &m_nCATSize, sizeof(int));
		if (dwRead != 4)
			bRead = false;
	}

	if (bRead)
	{
		dwRead = cmnFileRead (hFile, &m_CATData, 256);
		if (dwRead != 256)
			bRead = false;
	}

	if (bRead)
	{
		dwRead = cmnFileRead (hFile, &m_nCurNetworkID, sizeof(int));
		if (dwRead != 4)
			bRead = false;
	}

	if (bRead)
	{
		dwRead = cmnFileRead (hFile, &m_nEMMService, sizeof(int));
		if (dwRead != 4)
			bRead = false;
	}

	if (bRead)
	{
		dwRead = cmnFileRead (hFile, &m_nLastChannel, sizeof (int));
		if (dwRead != 4 /*|| m_nLastChannel < 0*/)
			bRead = false;
	}

	cmnFileClose(hFile);

	if (!bRead)
		ReleaseData ();
}

//Store channel info into file
void CEngineCMMBBase::WriteData (void)
{
	VO_FILE_SOURCE	fileSource;
	memset(&fileSource, 0, sizeof(VO_FILE_SOURCE));
	fileSource.nFlag = VO_FILE_TYPE_NAME;
	fileSource.nOffset = 0;
	fileSource.nLength = 0xFFFFFFFF;
	fileSource.pSource = (VO_PTR)m_szChannelFilePath;
	fileSource.nMode = VO_FILE_WRITE_ONLY;

	VO_PTR hFile = cmnFileOpen(&fileSource);
	if (hFile == NULL)
		return;

	VO_U32 dwWrite = cmnFileWrite (hFile, &m_nFreqNum, sizeof(int));
	dwWrite = cmnFileWrite (hFile, &m_nChannelCount, sizeof (int));

	int i = 0;
	int j = 0;
	for (i = 0; i < m_nChannelCount; i++)
		dwWrite = cmnFileWrite (hFile, &m_pChannelInfo[i], sizeof (VO_LIVESRC_CHANNELINFO));

	for (i = 0; i < m_nChannelCount; i++)
		dwWrite = cmnFileWrite (hFile, &m_pChannelESGNum[i], sizeof (int));

	for (i = 0; i < m_nChannelCount; i++)
	{
		for (j = 0; j < m_pChannelESGNum[i]; j++)
			dwWrite = cmnFileWrite (hFile, &m_ppESGInfo[i][j], sizeof (VO_CMMB_ESG_ITEM));
	}

	WriteExtraData(hFile);
	dwWrite = cmnFileWrite (hFile, &m_nContServiceCount, sizeof(int));
	for (i = 0; i < m_nContServiceCount; i++)
		dwWrite = cmnFileWrite (hFile, &m_pContService[i], sizeof (VOCONTINUESERVICEINFO));

	dwWrite = cmnFileWrite (hFile, &m_nCATSize, sizeof(int));
	dwWrite = cmnFileWrite (hFile, &m_CATData, 256);
	dwWrite = cmnFileWrite (hFile, &m_nCurNetworkID, sizeof(int));

	dwWrite = cmnFileWrite (hFile, &m_nEMMService, sizeof(int));

	dwWrite = cmnFileWrite (hFile, &m_nCurChannel, sizeof (int));

	cmnFileClose(hFile);
}

void CEngineCMMBBase::WriteExtraData(VO_HANDLE hFile)
{
	return;
}

// To avoid the random write error as much as possible, append the current channel information at the end of file.
void CEngineCMMBBase::WriteCurChannel()
{
	VO_FILE_SOURCE	fileSource;
	fileSource.nFlag = VO_FILE_TYPE_NAME;
	fileSource.nOffset = 0;
	fileSource.nLength = 0xFFFFFFFF;
	fileSource.pSource = (VO_PTR)m_szChannelFilePath;
	fileSource.nMode = VO_FILE_READ_WRITE;

	VO_PTR hFile = cmnFileOpen(&fileSource);
	if (hFile == NULL)
	{
		WriteData();	// Re-create the channel file
		hFile = cmnFileOpen(&fileSource);
		if (hFile == NULL)
			return;
	}

	VO_S64 dwPtr = cmnFileSeek(hFile, 0 - sizeof(int), VO_FILE_END);
	if (dwPtr < 0)
	{
		cmnFileClose(hFile);
		return;
	}

	cmnFileWrite (hFile, &m_nCurChannel, sizeof (int));
	cmnFileClose(hFile);
}

