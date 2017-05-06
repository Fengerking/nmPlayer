#include "CDemultiplexer.h"
#include "dmxbasicapi.h"
#include "GZipHelper.h"
#include "CDumper.h"

#include "CEsgParseMng.h"


#if defined _LINUX
#include <unistd.h>
#include <sys/time.h>
#endif // LINUX


CDemultiplexer::CDemultiplexer(void)
: m_pReciever(NULL)
, m_dwCurrVideoTimeStamp(0)
, m_dwCurrAudioTimeStamp(0)
, m_dwLastAudioTimeStamp(0)
, m_nAudioAlgorithm(AUDIO_AAC)
, m_nVideoAlgorithm(VIDEO_H264)
, m_bStopForceParse(false)
, m_nEmmMfId(-1)
{
	::BuildCrcTable();

	// must init
	// 20090827

#ifdef _OLD_ESG_PARSE
	for(BYTE n=0; n<TOTAL_ESG_DATA_TYPE+1; n++)
	{
		m_EsgDataContainer[n].seg				= NULL;
		m_EsgDataContainer[n].seg_count			= 0;
		m_EsgDataContainer[n].curr_seg_count	= 0;
		m_EsgDataContainer[n].total_payload_len = 0;
		m_EsgDataContainer[n].next_seg_sn		= 0;
	}
#else
	for(BYTE n=0; n<TOTAL_ESG_DATA_TYPE+1; n++)
	{
		//m_EsgDataContainer[n].seg				= NULL;
		m_EsgDataContainer[n].seg_count			= 0;
		m_EsgDataContainer[n].curr_seg_count	= 0;
		m_EsgDataContainer[n].total_payload_len = 0;
		m_EsgDataContainer[n].next_seg_sn		= 0;
	}
#endif

	for(BYTE n=0; n<TOTAL_ESG_DATA_TYPE+1; n++)
	{
		m_EsgParseState[n].finished = false;
		m_EsgParseState[n].curr_data_fragment_count = 0;
		m_EsgParseState[n].max_fragment_id = 0;

		for(int j=0; j<MAX_DATA_FRAGMENT_DESC_COUNT; j++)
		{
			m_EsgParseState[n].data_fragment_id[j] = 255;
		}
	}

	//m_RawData.pRawData	= new BYTE[MAX_MF_LEN];
	m_RawData.nCurrLen	= 0;
	m_RawData.nLeftLen	= 0;
}


CDemultiplexer::~CDemultiplexer(void)
{
	if(!m_bStopForceParse)
		Close();

	//CDmxDump::CloseAllAction();
}

void CDemultiplexer::Open()
{
	m_bStopForceParse = false;
}

void CDemultiplexer::Close()
{
	StopForceParse();

#ifdef _WIN32
	Sleep (100);
#elif defined _LINUX
	usleep (1000 * 100);
#endif // _WIN32
	//release
	//OutputDebugString(_T("Begin to release esg buffer\n"));
	for(BYTE n=0; n<TOTAL_ESG_DATA_TYPE+1; n++)
	{
		// 20100331 removed tmp
		//m_EsgDataContainer[n].reset();
	}
	//OutputDebugString(_T("End to release esg buffer\n"));


	//OutputDebugString(_T("Begin to release rawdata buffer\n"));
	if(m_RawData.pRawData)
	{
		//20090827
		//delete m_RawData.pRawData;
		//m_RawData.pRawData = NULL;
		m_RawData.nCurrLen = 0;
		m_RawData.nLeftLen = 0;
	}
	//OutputDebugString(_T("end to release rawdata buffer\n"));
}

void CDemultiplexer::SetReciever(CDmxResultReciever* pRecv)
{
	m_pReciever = pRecv;
}

void CDemultiplexer::StopForceParse()
{
	m_bStopForceParse = true;
}

int CDemultiplexer::CheckFrameLength(MultiplexFrameHeader* pHeader, int nLen)
{
	int nRet = DEMUX_OK;

	// check mf length
	WORD wMuxLen = pHeader->header_len + MF_HEADER_CRC32_LEN;

	for(BYTE n=0; n<pHeader->multi_sub_frame_num; n++)
	{
		wMuxLen += pHeader->multi_sub_frame_len[n];
	}

	nRet = nLen<wMuxLen?DEMUX_MF_LENTH_ERROR:DEMUX_OK;

	if(nRet != DEMUX_OK && m_pReciever)
		m_pReciever->WhenParsedFail(nRet, NULL);

	//check msf length
	if(pHeader->multi_sub_frame_num > 0)
	{
		WORD wSubFrmLen = wMuxLen - pHeader->header_len - MF_HEADER_CRC32_LEN;
		nLen			= nLen - pHeader->header_len - MF_HEADER_CRC32_LEN;

		if(nLen<wSubFrmLen || 0>nLen)
		{
			nRet = DEMUX_MSF_LENTH_ERROR;
			if(m_pReciever)
				m_pReciever->WhenParsedFail(nRet, NULL);
		}
	}

	return nRet;
}

bool CDemultiplexer::FindStartCode(LPBYTE& pData, int& nLen)
{
	DWORD dwSync = 0X01000000;
	int maxval = nLen - (MF_START_CODE_LEN - 1);
	for(int i=0; i<maxval; i++)
	{
		if (!memcmp(pData+i, &dwSync, MF_START_CODE_LEN))
		{
			pData += i;
			nLen  -= i;

			BYTE* pKey = (pData+MF_START_CODE_LEN+1);
			WORD wKeyBits = (pKey[0]<<8) + pKey[1];

			// 20090313
			// checek CMMB version flag
			//if(1==(wKeyBits>>11) && 1==((wKeyBits>>6)&0x1f) )
				//return true;
			return true;

			//CDmxDump::DumpLog(_T("++++++FOUND falsehood start code.+++\n"));
		}
	}

	return false;
}

bool CDemultiplexer::GetMfLen(LPBYTE pData, int nDataLen, int& nMfLen)
{
	// mf header len
	pData += MF_START_CODE_LEN;
	nMfLen = *pData;

	// msf count
	pData += 7;
	BYTE nMsfCount = (*pData) & 0x0f;
	
	// total msf len
	pData++;
	
	for(BYTE n=0; n<nMsfCount; n++)
	{
		nMfLen += STEP24(pData);	
	}

	//crc
	nMfLen += MF_HEADER_CRC32_LEN;

	return nMfLen <= nDataLen;
}

void CDemultiplexer::CopyRawData(MfRawData* pRawDataBuf, BYTE* pData, int nLen)
{
	if(MAX_MF_LEN < pRawDataBuf->nCurrLen+nLen)
	{
		//CDmxDump::DumpLog(_T("++++++++++++++CopyRawData return.Left Len=%d.++++++++++++++\n"), m_RawData.nLeftLen);

		//skip this frame
		m_RawData.nLeftLen = 0;
		m_RawData.nCurrLen = 0;
		return;
	}
		

	memcpy(pRawDataBuf->pRawData+pRawDataBuf->nCurrLen , pData, nLen);

	pRawDataBuf->nCurrLen	+= nLen;
	pRawDataBuf->nLeftLen	-= nLen;	
}

int count = 0;
int CDemultiplexer::DemuxEx(BYTE* pData, int nLen, WORD wServiceId)
{
	int nRet		= DEMUX_OK;
	int nCopyLen	= 0;

	//CDmxDump::DumpLogEx("Demux data: count = %04d, len = %d", count, nLen);
	//CDmxDump::DumpMFRawData(pData, nLen);
 	//CDmxDump::DumpLog(_T("Demux data: count = %04d, len = %d\n"), count, nLen);
 	count++;

	BYTE* pNewData		= pData;
	int nNewLen			= nLen;
	m_RawData.nCurrLen	= 0;
	m_RawData.nLeftLen	= 0;

	//CDmxDump::StartParsePerformance();

	//
	if(DEMUX_OK != ParseMFHeader(pNewData, &m_MF.frame_header))
	{
		ForceParse(pNewData, nNewLen, UNKNOWN_UNIT, DEMUX_MF_HEADER_FAILED, m_dwCurrAudioTimeStamp);
		return DEMUX_MF_HEADER_FAILED;
	}

// 	nRet = CheckFrameLength(&m_MF.frame_header, nNewLen);
// 	if(DEMUX_OK != nRet)
// 	{
// 		ForceParse(pNewData, nNewLen, UNKNOWN_UNIT, DEMUX_MSF_LENTH_ERROR, m_dwCurrAudioTimeStamp);
// 		return nRet;
// 	}

	//skip header
	pNewData += (m_MF.frame_header.header_len + MF_HEADER_CRC32_LEN);
	nNewLen -= (m_MF.frame_header.header_len + MF_HEADER_CRC32_LEN);


	if(TS0_MF_ID == m_MF.frame_header.mf_id)
	{
		// parse ts0
		nRet = ParseTS0(pNewData, &m_MF.frame_header);
	}
	else
	{
		//CDmxDump::DumpMFRawData(pNewData, nNewLen);

		// parse msf
		nRet = ParseMSF(pNewData, nNewLen);
	}

	if(DEMUX_OK == nRet)
	{
		if(m_pReciever)
			m_pReciever->WhenParseWholeMFFinished(&m_MF);
	}

// 	if(nLen > 0)
// 	{
// 		Demux(pData, nLen);
// 	}

	//CDmxDump::DumpLog(_T("+++Demux a MF finished.+++\n"));
	//CDmxDump::EndParsePerformance();

	return nRet;
}

int CDemultiplexer::Demux(BYTE* pData, int nLen, WORD wServiceId)
{
	return DemuxEx(pData, nLen, wServiceId);

	//CDmxDump::DumpMFRawData(pData, nLen);

	int nRet		= DEMUX_OK;
	int nCopyLen	= 0;

	// not start with start code
	if(m_RawData.nLeftLen != 0)
	{
		if(nLen >= m_RawData.nLeftLen)
		{
			nCopyLen = m_RawData.nLeftLen;
			CopyRawData(&m_RawData, pData, nCopyLen);
			pData += nCopyLen;
			nLen -= nCopyLen;
		}
		else
		{
			CopyRawData(&m_RawData, pData, nLen);
			return DEMUX_OK;
		}
	}
	else
	{
		// found start code
		if(FindStartCode(pData, nLen))
		{
			// get mf len by bits
			GetMfLen(pData, nLen, m_RawData.nLeftLen);

			if(nLen >= m_RawData.nLeftLen)
			{
				nCopyLen = m_RawData.nLeftLen;
				CopyRawData(&m_RawData, pData, nCopyLen);
				pData += nCopyLen;
				nLen  -= nCopyLen;
			}
			else
			{
				CopyRawData(&m_RawData, pData, nLen);
				return DEMUX_OK;
			}
		}
		else// no found start code
		{
			return DEMUX_OK;	
		}
	}

	BYTE* pNewData		= m_RawData.pRawData;
	int nNewLen			= m_RawData.nCurrLen;
	m_RawData.nCurrLen	= 0;
	m_RawData.nLeftLen	= 0;

	//CDmxDump::StartParsePerformance();

	//
	if(DEMUX_OK != ParseMFHeader(pNewData, &m_MF.frame_header))
	{
		ForceParse(pNewData, nNewLen, UNKNOWN_UNIT, DEMUX_MF_HEADER_FAILED, m_dwCurrAudioTimeStamp);
		return DEMUX_MF_HEADER_FAILED;
	}
		
	nRet = CheckFrameLength(&m_MF.frame_header, nNewLen);
	if(DEMUX_OK != nRet)
	{
		ForceParse(pNewData, nNewLen, UNKNOWN_UNIT, DEMUX_MSF_LENTH_ERROR, m_dwCurrAudioTimeStamp);
		return nRet;
	}

//  CDmxDump::DumpLog(_T("MF id = %d\n"), m_MF.frame_header.mf_id);
// 	if (m_MF.frame_header.mf_id == 4)// EMM:11
// 	{
// 		return nRet;
// 	}


	//skip header
	pNewData += (m_MF.frame_header.header_len + MF_HEADER_CRC32_LEN);
	nNewLen -= (m_MF.frame_header.header_len + MF_HEADER_CRC32_LEN);


	if(TS0_MF_ID == m_MF.frame_header.mf_id)
	{
		// parse ts0
		nRet = ParseTS0(pNewData, &m_MF.frame_header);
	}
	else
	{
		//CDmxDump::DumpMFRawData(pNewData, nNewLen);

		// parse msf
		nRet = ParseMSF(pNewData, nNewLen);
	}

	if(DEMUX_OK == nRet)
	{
		if(m_pReciever)
			m_pReciever->WhenParseWholeMFFinished(&m_MF);
	}


	if(nLen > 0)
	{
		Demux(pData, nLen);
	}

	//CDmxDump::DumpLog(_T("+++Demux a MF finished.+++\n"));
	//CDmxDump::EndParsePerformance();

	return nRet;
}

int CDemultiplexer::ParseMFHeader(BYTE* pData, MultiplexFrameHeader* pHeader)
{
	int nRet = ::ParseMFHeader(pData, pHeader);

	if(DEMUX_OK == nRet)
	{
		if(m_pReciever)
			m_pReciever->WhenParsedMultiFrameHeader(pHeader);
	}	
	else
	{
		if(m_pReciever)
			m_pReciever->WhenParsedFail(nRet, NULL);
	}

	return nRet;
}

int CDemultiplexer::ParseTS0(BYTE* pData, MultiplexFrameHeader* pHeader)
{
	int nRet = DEMUX_OK;
	BYTE table_id = 0x00;

	memset(&m_MF.ts0_info.emegency_broadcast, 0, sizeof(EmegencyBroadcast));

	for(BYTE i=0; i<pHeader->multi_sub_frame_num; i++)
	{
		if(pHeader->multi_sub_frame_len[i] <= 0)
			continue;

		table_id = *pData;

		switch(table_id)
		{
		case CTROL_TABLE_NIT:
			nRet = ParseNIT(pData, pHeader->multi_sub_frame_len[i]);
			break;
		case CTROL_TABLE_CMCT:
			nRet = ParseCMCT(pData, pHeader->multi_sub_frame_len[i]);
			break;
		case CTROL_TABLE_SMCT:
			nRet = ParseSMCT(pData, pHeader->multi_sub_frame_len[i]);
			break;
		case CTROL_TABLE_CSCT:
			nRet = ParseCSCT(pData, pHeader->multi_sub_frame_len[i]);
			break;
		case CTROL_TABLE_SSCT:
			nRet = ParseSSCT(pData, pHeader->multi_sub_frame_len[i]);
			break;
		case CTROL_TABLE_ESG:
			nRet = ParseEsgDesc(pData, pHeader->multi_sub_frame_len[i]);
			break;
		case CTROL_TABLE_CA:
			nRet = ParseEADesc(pData, pHeader->multi_sub_frame_len[i]);
			break;
		case CTROL_TABLE_EMERGENCY:
			nRet = ParseEB(pData, pHeader->multi_sub_frame_len[i]);
			break;
		default:
			break;
		}
		
		pData += pHeader->multi_sub_frame_len[i];
	}

	if(m_pReciever)
	{	
		m_pReciever->WhenParsedTS0(&m_MF.ts0_info);
	}
		
	
	return nRet;
}

int CDemultiplexer::ParseNIT(BYTE* pData, int nLen)
{
	int nRet = ::ParseNIT(pData, nLen, &m_MF.ts0_info.nit);
	
	if(DEMUX_OK != nRet)
	{
		if(m_pReciever)
			m_pReciever->WhenParsedFail(nRet, 0);
	}
	else
	{
		if(m_pReciever)
			m_pReciever->WhenParsedNIT(&m_MF.ts0_info.nit);
	}

	return nRet;
}

int CDemultiplexer::ParseCMCT(BYTE* pData, int nLen)
{
	int nRet = ::ParseCMCT(pData, nLen, &m_MF.ts0_info.cmct);

	if(nRet == DEMUX_OK)
	{
		if(m_pReciever)
			m_pReciever->WhenParsedCMCT(&m_MF.ts0_info.cmct);
	}
	else
	{
		if(m_pReciever)
			m_pReciever->WhenParsedFail(nRet, 0);
	}

	return nRet;
}

int CDemultiplexer::ParseSMCT(BYTE* pData, int nLen)
{
	int nRet = ::ParseCMCT(pData, nLen, &m_MF.ts0_info.smct);

	if(nRet == DEMUX_OK)
	{
		if(m_pReciever)
			m_pReciever->WhenParsedSMCT(&m_MF.ts0_info.smct);
	}
	else
	{
		if(m_pReciever)
			m_pReciever->WhenParsedFail(nRet, 0);
	}

	return nRet;
}


int CDemultiplexer::ParseCSCT(BYTE* pData, int nLen)
{
	int nRet = ::ParseCSCT(pData, nLen, &m_MF.ts0_info.csct);

	if(nRet == DEMUX_OK)
	{
		if(m_pReciever)
			m_pReciever->WhenParsedCSCT(&m_MF.ts0_info.csct);
	}
	else
	{
		if(m_pReciever)
			m_pReciever->WhenParsedFail(nRet, 0);
	}

	return nRet;
}

int CDemultiplexer::ParseSSCT(BYTE* pData, int nLen)
{
	int nRet = ::ParseCSCT(pData, nLen, &m_MF.ts0_info.ssct);

	if(nRet == DEMUX_OK)
	{
		if(m_pReciever)
			m_pReciever->WhenParsedSSCT(&m_MF.ts0_info.ssct);
	}
	else
	{
		if(m_pReciever)
			m_pReciever->WhenParsedFail(nRet, 0);
	}

	return nRet;
}


int CDemultiplexer::ParseEsgDesc(BYTE* pData, int nLen)
{
	int nRet = ::ParseEBDT(pData, nLen, &m_MF.ts0_info.esg_desc);

	if(DEMUX_OK == nRet)
	{
		if(m_pReciever)
			m_pReciever->WhenParsedEsgBasicDescription(&m_MF.ts0_info.esg_desc);
	}
	else
	{
		if(m_pReciever)
			m_pReciever->WhenParsedFail(nRet, 0);
	}
		
	return nRet;
}

int CDemultiplexer::ParseEADesc(BYTE* pData, int nLen)
{
	int nRet = ::ParseEADesc(pData, nLen, &m_MF.ts0_info.ea_desc);

	if(DEMUX_OK == nRet)
	{
		if(m_pReciever)
			m_pReciever->WhenParsedEADesc(&m_MF.ts0_info.ea_desc);
	}
	else
	{
		if(m_pReciever)
			m_pReciever->WhenParsedFail(nRet, 0);
	}

	return nRet;

}

int CDemultiplexer::ParseEB(BYTE* pData, int nLen)
{
	memset(&m_MF.ts0_info.emegency_broadcast, 0, sizeof(EmegencyBroadcast));
	int nRet = ::ParseEB(pData, nLen, &m_MF.ts0_info.emegency_broadcast);

	if(DEMUX_OK == nRet)
	{
		if(m_pReciever)
			m_pReciever->WhenParsedEB(&m_MF.ts0_info.emegency_broadcast);
	}
	else
	{
		if(m_pReciever)
			m_pReciever->WhenParsedFail(nRet, 0);
	}

	return nRet;

}

int CDemultiplexer::ParseMSFHeader(BYTE* pData, MultiSubFrameHeader* pHeader)
{
	int nRet = ::ParseMSFHeader(pData, pHeader);

	if(DEMUX_OK == nRet)
	{
		//CDmxDump::DumpAacFramerate(pHeader);

		if(m_pReciever)
			m_pReciever->WhenParsedMultiSubFrameHeader(pHeader);
	}
	else
	{
		if(m_pReciever)
			m_pReciever->WhenParsedFail(nRet, 0);
	}

	return nRet;
}

int CDemultiplexer::ParseMSF(BYTE* pData, int nLen)
{
	int nRet = DEMUX_OK;
	//CDmxDump::DumpLog(_T("MSF count = %d.\n"), m_MF.frame_header.multi_sub_frame_num);

	for(int n=0; n<m_MF.frame_header.multi_sub_frame_num; n++)
	{
		//parse multiplex sub frame header
		if(DEMUX_OK != ParseMSFHeader(pData, &m_MF.multiplex_sub_frame.sub_frame_header[n]))
		{
			// 20090729
			ForceParse(pData, m_MF.frame_header.multi_sub_frame_len[n], UNKNOWN_UNIT, DEMUX_MSF_HEADER_FAILED, 0);
				
			// NOTE: Don't return, continue to parse the left. skip sub frame
			pData += m_MF.frame_header.multi_sub_frame_len[n];
			continue;
		}

		//skip header length
		pData += m_MF.multiplex_sub_frame.sub_frame_header[n].header_len + MSF_HEADER_CRC32_LEN;

		// first parse data
		if(m_MF.multiplex_sub_frame.sub_frame_header[n].data_seg_len > 0)
		{
			BYTE* pStart = pData;
			if(m_MF.multiplex_sub_frame.sub_frame_header[n].video_seg_len > 0)
				pStart += m_MF.multiplex_sub_frame.sub_frame_header[n].video_seg_len;
			if(m_MF.multiplex_sub_frame.sub_frame_header[n].audio_seg_len > 0)
				pStart += m_MF.multiplex_sub_frame.sub_frame_header[n].audio_seg_len;

			nRet = ParseData(pStart, &m_MF.multiplex_sub_frame.sub_frame_header[n]);
		}

		if (IsEmmFrame())
		{
			return nRet;
		}

		// 20090827
		if(m_bStopForceParse)
			break;

		if(m_MF.multiplex_sub_frame.sub_frame_header[n].audio_seg_len > 0)
		{
			BYTE* pStart = pData;
			if(m_MF.multiplex_sub_frame.sub_frame_header[n].video_seg_len > 0)
				pStart += m_MF.multiplex_sub_frame.sub_frame_header[n].video_seg_len;

			nRet = ParseAudio(pStart, &m_MF.multiplex_sub_frame.sub_frame_header[n]);
			//pData += m_MF.multiplex_sub_frame.sub_frame_header[n].audio_seg_len;
		}

		// 20090827
		if(m_bStopForceParse)
			break;

		if(m_MF.multiplex_sub_frame.sub_frame_header[n].video_seg_len > 0)
		{
			nRet = ParseVideo(pData, &m_MF.multiplex_sub_frame.sub_frame_header[n]);
			pData += m_MF.multiplex_sub_frame.sub_frame_header[n].video_seg_len;
		}

		if(m_MF.multiplex_sub_frame.sub_frame_header[n].audio_seg_len > 0)
		{
			pData += m_MF.multiplex_sub_frame.sub_frame_header[n].audio_seg_len;
		}

		if(m_MF.multiplex_sub_frame.sub_frame_header[n].data_seg_len > 0)
		{
			pData += m_MF.multiplex_sub_frame.sub_frame_header[n].data_seg_len;
		}

		//pData += m_MF.frame_header.multi_sub_frame_len[n] - m_MF.multiplex_sub_frame.sub_frame_header[n].header_len - MSF_HEADER_CRC32_LEN;
	}

	return nRet;
}


int CDemultiplexer::ParseData(BYTE* pData, MultiSubFrameHeader* pHeader)
{
	int nRet = DEMUX_OK;

	DataSegmentHeader header;
	nRet = ParseDataSegmentHeader(pData, &header);

	if(DEMUX_OK != nRet)
	{
		// 20100330
		//ForceParse(pData, pHeader->audio_seg_len, DATA_UNIT, nRet, m_dwCurrAudioTimeStamp);
		return nRet;
	}	

	// skip header len
	pData += header.header_len + DATA_SEGMENT_HEADER_CRC32_LEN;

	for(BYTE n=0; n<header.unit_count; n++)
	{
		// 20090827
		if(m_bStopForceParse)
			break;
		
		switch(header.data_unit_param[n].unit_type)
		{
		case DATA_PROGRAM_GUIDE:
			nRet = ParseProgramGuide(pData, header.data_unit_param[n].unit_len, pHeader->packet_mode);
			break;
		case DATA_ESG:
			nRet = ParseEsg(pData, header.data_unit_param[n].unit_len, pHeader->packet_mode);
			break;
		case DATA_ECM_1:
		case DATA_ECM_2:
		case DATA_ECM_3:
			{
				nRet = ParseEcm(pData, header.data_unit_param[n].unit_len, header.data_unit_param[n].unit_type);
			}
			break;
		case DATA_EMM_1:
		case DATA_EMM_2:
		case DATA_EMM_3:
			{
				m_nEmmMfId = m_MF.frame_header.mf_id;

				nRet = ParseEmm(pData, header.data_unit_param[n].unit_len, header.data_unit_param[n].unit_type);
			}
			break;
		case DATA_XPE:
			{
				nRet = ParseXpe(pData, header.data_unit_param[n].unit_len);
			}
			break;
		case DATA_XPE_FEC:
			{
				nRet = ParseXpe_FEC(pData, header.data_unit_param[n].unit_len);
			}
			break;
		case DATA_TEST:
			break;
		default:
#ifdef WIN32
			//CDumper::WriteLog("Maybe reserved data segment type = %d.", header.data_unit_param[n].unit_type);
#endif
			break;
		}

		pData += header.data_unit_param[n].unit_len;
	}


	return nRet;
}


int CDemultiplexer::ParseXpe(BYTE* pData, int nLen)
{
	int nRet		= DEMUX_OK;
	BYTE* pStart	= pData;
	BYTE* pEnd		= pData + nLen;


	//每个单元的复用块个数无法知道，故只能通过起始指针来判断
	while(pEnd > pStart)
	{
		Mode2FragmentHeader header;
		int nRet = ::ParseFragmentMode2Header(pStart, &header);

		if(DEMUX_OK != nRet)
		{
			//if(m_pReciever)
			//m_pReciever->WhenParsedFail(nRet, 0);
		}

		// 不是以0x55开始
		pStart += header.header_len + MODE2_FRAGMENT_CRC8_LEN;

		//
		BYTE CheckSum,CrcOn,TransNum;
		BYTE *data;
		unsigned short XpePackageLen,TotalLen;
		static BYTE guType = 0xFF;
		CheckSum = *pStart;
		XpePackageLen = ((*pStart&0x0F)<<8) + *(pStart+1);
		CrcOn = *(pStart+2) >> 7;
		TransNum = *(pStart+2) & 0x7F;
		CheckSum ^= *(pStart+1);
		CheckSum ^= *(pStart+2);
		data = pStart+3;

		if((*pStart)&0x80)
		{
			if(((*pStart)&0x40) == 0) //
			{
				TotalLen = (*(pStart+3)<<8) + *(pStart+4);
				CheckSum ^= *(pStart+3);
				CheckSum ^= *(pStart+4);
				
				// 数据包总长度
				data += 2;
			}
			CheckSum ^= *data;
			guType = (*data)>>7;

			// 扩展
			data++;
		}
		if( CheckSum != *data )
		{
			return DEMUX_XPE_ERROR;
		}

		// 校验和
		data++;

		if (DATA_SERVICE_FILE_MODE == guType)
		{
			return ParseXpeFileMode(data, nLen-(data-pData));
		}

		bool bEncrypt = (m_MF.multiplex_sub_frame.sub_frame_header->encrypt_flag == ENCRYPT_SCRAMBLE);

		//OnStreamModeXpe(data, XpePackageLen);
		if (bEncrypt)
		{
			CaPacket packet;
			Decrypt(&packet, data, XpePackageLen, MUX_SEGMENT_DATA, DATA_XPE);

			if (packet.encrypt && m_pReciever)
			{
				XpeBuf buf;
				buf.data		= packet.decrypt_data_ptr;
				buf.data_len	= packet.decrypt_data_len;
				buf.mode		= guType==0?DATA_SERVICE_STREAM_MODE:DATA_SERVICE_FILE_MODE;
				buf.type		= TYPE_XPE;
				m_pReciever->WhenParsedXpe(&buf);
			}
		}
		else
		{
			if (m_pReciever)
			{
				XpeBuf buf;
				buf.data		= data;
				buf.data_len	= XpePackageLen;
				buf.mode		= guType==0?DATA_SERVICE_STREAM_MODE:DATA_SERVICE_FILE_MODE;
				buf.type		= TYPE_XPE;
				m_pReciever->WhenParsedXpe(&buf);
			}
		}

		pStart += header.payload_len;
	}

	return DEMUX_OK;
}

int CDemultiplexer::ParseXpe_FEC(BYTE* pData, int nLen)
{
	int nRet		= DEMUX_OK;
	BYTE* pStart	= pData;
	BYTE* pEnd		= pData + nLen;


	//每个单元的复用块个数无法知道，故只能通过起始指针来判断
	while(pEnd > pStart)
	{
		Mode2FragmentHeader header;
		int nRet = ::ParseFragmentMode2Header(pStart, &header);

		if(DEMUX_OK != nRet)
		{
			//if(m_pReciever)
			//m_pReciever->WhenParsedFail(nRet, 0);
		}

		// 不是以0x55开始
		pStart += header.header_len + MODE2_FRAGMENT_CRC8_LEN;

		//
		BYTE CheckSum,CrcOn,TransNum;
		BYTE *data;
		unsigned short XpePackageLen,TotalLen;
		static BYTE guType = DATA_SERVICE_STREAM_MODE;
		CheckSum = *pStart;
		XpePackageLen = ((*pStart&0x0F)<<8) + *(pStart+1);
		CrcOn = *(pStart+2) >> 7;
		TransNum = *(pStart+2) & 0x7F;
		CheckSum ^= *(pStart+1);
		CheckSum ^= *(pStart+2);
		data = pStart+3;

		if((*pStart)&0x80)
		{
			if(((*pStart)&0x40) == 0) //
			{
				TotalLen = (*(pStart+3)<<8) + *(pStart+4);
				CheckSum ^= *(pStart+3);
				CheckSum ^= *(pStart+4);

				// 校验数据总长度
				data += 2;
			}
			CheckSum ^= *data;
			//guType = (*data)>>7;
			
			// 算法标识
			data++;

			// 参数长度
			BYTE param_len = *data;
			data++;

			data += param_len;
		}

		// here need check
		if( CheckSum != *data )
		{
			return DEMUX_XPE_ERROR;
		}

		// 校验和
		data++;

		bool bEncrypt = (m_MF.multiplex_sub_frame.sub_frame_header->encrypt_flag == ENCRYPT_SCRAMBLE);

		//OnStreamModeXpe(data, XpePackageLen);
		if (bEncrypt)
		{
			CaPacket packet;
			Decrypt(&packet, data, XpePackageLen, MUX_SEGMENT_DATA, DATA_XPE_FEC);

			if (packet.encrypt && m_pReciever)
			{
				XpeBuf buf;
				buf.data		= packet.decrypt_data_ptr;
				buf.data_len	= packet.decrypt_data_len;
				buf.mode		= guType==0?DATA_SERVICE_STREAM_MODE:DATA_SERVICE_FILE_MODE;
				buf.type		= TYPE_XPE;
				m_pReciever->WhenParsedXpe(&buf);
			}
		}
		else
		{
			if (m_pReciever)
			{
				XpeBuf buf;
				buf.data		= data;
				buf.data_len	= XpePackageLen;
				buf.mode		= guType==0?DATA_SERVICE_STREAM_MODE:DATA_SERVICE_FILE_MODE;
				buf.type		= TYPE_XPE;
				m_pReciever->WhenParsedXpe(&buf);
			}
		}

		pStart += header.payload_len;
	}

	return DEMUX_OK;
}

int CDemultiplexer::ParseXpeFileMode(BYTE* pData, int nLen)
{
	bool bEncrypt = (m_MF.multiplex_sub_frame.sub_frame_header->encrypt_flag == ENCRYPT_SCRAMBLE);

	//OnStreamModeXpe(data, XpePackageLen);
	if (bEncrypt)
	{
		CaPacket packet;
		Decrypt(&packet, pData, nLen, MUX_SEGMENT_DATA, DATA_XPE);

		if (packet.encrypt && m_pReciever)
		{
			XpeBuf buf;
			buf.data		= packet.decrypt_data_ptr;
			buf.data_len	= packet.decrypt_data_len;
			buf.mode		= DATA_SERVICE_FILE_MODE;
			buf.type		= TYPE_XPE;
			m_pReciever->WhenParsedXpe(&buf);
		}
	}
	else
	{
		if (m_pReciever)
		{
			XpeBuf buf;
			buf.data		= pData;
			buf.data_len	= nLen;
			buf.mode		= DATA_SERVICE_FILE_MODE;
			buf.type		= TYPE_XPE;
			m_pReciever->WhenParsedXpe(&buf);
		}
	}

	return DEMUX_OK;
}


int CDemultiplexer::Decrypt(CaPacket* pPacket, BYTE* pData, int nLen, BYTE nDescramblingType, BYTE nEcmEmmType)
{
	memset(pPacket, 0, sizeof(CaPacket));

	if(!m_pReciever)
	{
		return DEMUX_OK;
	}

#if 0
	TCHAR tmp[128];
	memset(tmp, 0, 128);
	if(nDescramblingType == MUX_SEGMENT_ECM)
		_stprintf(tmp, _T("ECM --- payload len = %04d, payload ptr=0x%02x%02x\n"), nLen, pData[0], pData[1]);
	else if (nDescramblingType == MUX_SEGMENT_EMM)
		_stprintf(tmp, _T("EMM --- payload len = %04d, payload ptr=0x%02x%02x\n"), nLen, pData[0], pData[1]);
	else if (nDescramblingType == MUX_SEGMENT_AUDIO)
		_stprintf(tmp, _T("AUDIO - payload len = %04d, payload ptr=0x%02x%02x\n"), nLen, pData[0], pData[1]);
	else if (nDescramblingType == MUX_SEGMENT_VIDEO)
		_stprintf(tmp, _T("VIDEO - payload len = %04d, payload ptr=0x%02x%02x\n"), nLen, pData[0], pData[1]);

#ifdef WIN32
	OutputDebugString(tmp);
#endif

#endif

	pPacket->encrypt_data_ptr	= pData;
	pPacket->encrypt_data_len	= nLen;

	pPacket->mf_id				= m_MF.frame_header.mf_id;
	pPacket->type				= nDescramblingType;
	pPacket->ecm_emm_type		= nEcmEmmType;

	m_pReciever->WhenParsedEncryptData(pPacket);

	return DEMUX_OK;
}

int CDemultiplexer::ParseEcm(BYTE* pData, int nLen, BYTE nEcmType)
{
	int nRet		= DEMUX_OK;
	BYTE* pStart	= pData;
	BYTE* pEnd		= pData + nLen;


	//每个单元的复用块个数无法知道，故只能通过起始指针来判断
	while(pEnd > pStart)
	{
		Mode2FragmentHeader header;
		int nRet = ::ParseFragmentMode2Header(pStart, &header);

		if(DEMUX_OK != nRet)
		{
			//if(m_pReciever)
				//m_pReciever->WhenParsedFail(nRet, 0);
		}

		// 不是以0x55开始
		pStart += header.header_len + MODE2_FRAGMENT_CRC8_LEN;

		// post payload
		if(m_pReciever)
		{
			CaPacket packet;

			// 20090427
			// WHY +5.      --->  5 = header.header_len + MODE2_FRAGMENT_CRC8_LEN
 			//Decrypt(&packet, pStart, header.payload_len+5, MUX_SEGMENT_ECM, nEcmType);
			Decrypt(&packet, pStart, header.payload_len, MUX_SEGMENT_ECM, nEcmType);
		}
		
		// 20090313
		if(header.end_flag == 1)
			break;
		
		pStart += header.payload_len;
	}

	return DEMUX_OK;	
}

int CDemultiplexer::ParseEmm(BYTE* pData, int nLen, BYTE nEmmType)
{
	int nRet		= DEMUX_OK;
	BYTE* pStart	= pData;
	BYTE* pEnd		= pData + nLen;


	//每个单元的复用块个数无法知道，故只能通过起始指针来判断
	while(pEnd > pStart)
	{
		Mode2FragmentHeader header;
		int nRet = ::ParseFragmentMode2Header(pStart, &header);

		if(DEMUX_OK != nRet)
		{
			//if(m_pReciever)
			//m_pReciever->WhenParsedFail(nRet, 0);
		}

		// 不是以0x55开始
		pStart += header.header_len + MODE2_FRAGMENT_CRC8_LEN;

		// post payload
		if(m_pReciever)
		{
			CaPacket packet;
			Decrypt(&packet, pStart, header.payload_len, MUX_SEGMENT_EMM, nEmmType);
		}

		pStart += header.payload_len;
	}

	return DEMUX_OK;	
}



int CDemultiplexer::ParseProgramGuide(BYTE* pData, WORD wUnitLen, BYTE nPacketMode)
{
	int nRet = DEMUX_OK;

	if(wUnitLen <= 0)
		return DEMUX_OK;

	if(PACKET_MODE_1 == nPacketMode)
	{
		//OutputDebugString(_T("ParseProgramGuide Mode 1.\n"));
		ProgramGuideInfo guide;
		nRet = ParseProgramGuide(pData, &guide);
	}
	else if (PACKET_MODE_2 == nPacketMode)
	{
		BYTE param = DATA_PROGRAM_GUIDE;
		nRet = ParseUnitMode2(pData, DATA_UNIT, wUnitLen, &param);
	}

	return nRet;
}

int CDemultiplexer::ParseProgramGuide(BYTE* pData, ProgramGuideInfo* pGuide)
{
	int nRet = ::ParseProgramGuideInfo(pData, pGuide);

	if(nRet == DEMUX_OK)
	{
		if(m_pReciever)
			m_pReciever->WhenParsedProgramGuide(pGuide);
	}
	else
	{
		if(m_pReciever)
			m_pReciever->WhenParsedFail(nRet, NULL);
	}

	return nRet;
}

int CDemultiplexer::ParseEsg(BYTE* pData, WORD wUnitLen, BYTE nPacketMode)
{
	int nRet = DEMUX_OK;

	if(wUnitLen <= 0)
		return DEMUX_OK;

	if(PACKET_MODE_1 == nPacketMode)
	{
		EsgDataSegment seg;
		nRet = ParseEsgSegment(pData, &seg);
	}
	else if (PACKET_MODE_2 == nPacketMode)
	{
		BYTE param = DATA_ESG;
		nRet = ParseUnitMode2(pData, DATA_UNIT, wUnitLen, &param);
	}

	return nRet;
}

int CDemultiplexer::ParseEsgSegment(BYTE* pData, EsgDataSegment* pSeg)
{
	int nRet = ::ParseEsgDataSegment(pData, pSeg);

	if(nRet != DEMUX_OK)
	{
		if(m_pReciever)
			m_pReciever->WhenParsedFail(nRet, NULL);

		return nRet;
	}

	//current parse segment type
	BYTE type = pSeg->header.type;
	//CDmxDump::DumpLog(_T("total=%d, type=%d, id=%d,seg_sn=%d\n"), pSeg->header.total, type, pSeg->header.id, pSeg->header.seg_sn);

	int nCheck = TotalEsgParsed(pSeg);
	if(1 == nCheck)
	{
		if(m_pReciever)
			m_pReciever->WhenParsedEsgFinished();
		return DEMUX_OK;
	}
	else if (-1 == nCheck)
	{
		return DEMUX_OK;
	}


	if(pSeg->header.total <= 0)
		return DEMUX_OK;

#ifdef _OLD_ESG_PARSE
	if(m_EsgDataContainer[type].seg == NULL)
	{
		// wait first packet
		if(pSeg->header.seg_sn != 0)
			return DEMUX_OK;

		m_EsgDataContainer[type].init(pSeg);
	}
#else
	if(m_EsgDataContainer[type].seg_count == 0)
	{
		// wait first packet
		if(pSeg->header.seg_sn != 0)
			return DEMUX_OK;

		m_EsgDataContainer[type].init(pSeg);
	}
#endif

	// 20090820
	if(m_EsgDataContainer[type].next_seg_sn != pSeg->header.seg_sn)
	{
		//CDmxDump::DumpLog(_T("return this seg_sn=%d\n"), pSeg->header.seg_sn);
		return DEMUX_OK;
	}

	m_EsgDataContainer[type].insert(pSeg);

	if (m_EsgDataContainer[type].curr_seg_count != m_EsgDataContainer[type].seg_count)
		return DEMUX_OK;
			
	//parse this whole segment
 	m_EsgParseState[type].data_fragment_id[m_EsgParseState[type].curr_data_fragment_count] = pSeg->header.id;
	m_EsgParseState[type].curr_data_fragment_count++;

	int len			= m_EsgDataContainer[type].total_payload_len;
	BYTE* payload	= new BYTE[len];
	
#ifdef _OLD_ESG_PARSE
	int copyed = 0;
	for (BYTE n=0; n<m_EsgDataContainer[type].seg_count; n++)
	{
		memcpy(payload+copyed, m_EsgDataContainer[type].seg[n]->payload, m_EsgDataContainer[type].seg[n]->header.payload_len);
		copyed += m_EsgDataContainer[type].seg[n]->header.payload_len;
	}
#else
	int copyed = 0;
	for (BYTE n=0; n<m_EsgDataContainer[type].seg_count; n++)
	{
		memcpy(payload+copyed, m_EsgDataContainer[type].seg[n].payload, m_EsgDataContainer[type].seg[n].header.payload_len);
		copyed += m_EsgDataContainer[type].seg[n].header.payload_len;
	}
#endif
	
	CGZIP2A unzip;
	char* pOut = (char*)payload;
	if(pSeg->header.encode == GZIP_ENCODE)
	{
// 		if (pSeg->header.type == SCHEDULE_INFO)
// 		{
// 			CDmxDump::WriteData("c:\\SCHEDULE_INFO.zip", (char*)payload, len);
// 		}


		// unzip
		unzip.Init(payload, len);
		pOut	= unzip.psz;
		len		= unzip.Length;
	}

	CEsgParseMng parser;

	switch(pSeg->header.type)
	{
	case SERVICE_INFO:
		{
#ifdef WIN32
			//CDmxDump::DumpData("c:\\SERVICE_INFO.xml", (BYTE*)pOut, len);
#endif
			parser.ParseServiceInfo((unsigned char*)pOut, len, m_pReciever);
		}
		break;
	case SERVICE_AUX_INFO:
		{
#ifdef WIN32
			//CDmxDump::DumpData("c:\\SERVICE_AUX_INFO.xml", (BYTE*)pOut, len);
#endif
			parser.ParseServiceAuxInfo((unsigned char*)pOut, len, m_pReciever);
		}
		break;
	case SCHEDULE_INFO:
		{
#ifdef WIN32
			//CDmxDump::DumpData("c:\\SCHEDULE_INFO.xml", (BYTE*)pOut, len);
#endif
			parser.ParseScheduleInfo((unsigned char*)pOut, len, m_pReciever);
		}
		break;
	case CONTENT_INFO:
		{
#ifdef WIN32
			//CDmxDump::DumpData("c:\\CONTENT_INFO.xml", (BYTE*)pOut, len);
#endif
			parser.ParseContentInfo((unsigned char*)pOut, len, m_pReciever);
		}
		break;
	case SERVICE_PARAM_INFO:
		{
#ifdef WIN32
			//CDmxDump::DumpData("c:\\SERVICE_PARAM_INFO.xml", (BYTE*)pOut, len);
#endif
			parser.ParseServiceParam((unsigned char*)pOut, len, m_pReciever);
		}
		break;
	}

	delete payload;

	//CDmxDump::DumpLog(_T("Esg parsed: type = %d, id = %d\n"), type, pSeg->header.id);

	ResetEsgDataContainer(&m_EsgDataContainer[type]);

	return nRet;
}

// return value: 1: finished,  0: unfinished, -1:id exist but not all id parsed or other type esg not finished
int CDemultiplexer::TotalEsgParsed(EsgDataSegment* pSeg)
{
	if(m_EsgParseState[pSeg->header.type].finished)
	{
		if(m_EsgParseState[SERVICE_INFO].finished 
			&& m_EsgParseState[SCHEDULE_INFO].finished 
			&& m_EsgParseState[CONTENT_INFO].finished)
		{
			return 1;
		}

		return -1;
	}
		

	if(m_EsgParseState[pSeg->header.type].max_fragment_id < pSeg->header.id)
		m_EsgParseState[pSeg->header.type].max_fragment_id = pSeg->header.id;

	// check
	bool bFinished = false;
	bool bIdExist = false;
	for(BYTE n=0; n<m_EsgParseState[pSeg->header.type].curr_data_fragment_count; n++)
	{
		if(m_EsgParseState[pSeg->header.type].data_fragment_id[n] == pSeg->header.id)
		{
			bIdExist = true;

			BYTE nExistCount = 0;
			for (BYTE m=1; m<=m_EsgParseState[pSeg->header.type].max_fragment_id; m++)
			{
				for (BYTE i=0; i<m_EsgParseState[pSeg->header.type].curr_data_fragment_count; i++)
				{
					if(m_EsgParseState[pSeg->header.type].data_fragment_id[i] == m)
					{
						nExistCount++;
						break;
					}
				}
			}

			if(nExistCount == m_EsgParseState[pSeg->header.type].max_fragment_id)
			{
				// Indicate that this type esg has been parsed if exist this id aand all id parsed
				bFinished = true;
			}

			break;
		}
	}

	if(bFinished)
	{
		m_EsgParseState[pSeg->header.type].finished = true;

		if(m_EsgParseState[SERVICE_INFO].finished 
			&& m_EsgParseState[SCHEDULE_INFO].finished 
			&& m_EsgParseState[CONTENT_INFO].finished)
		{
			return 1;
		}

		return -1;
	}

	if (bIdExist)
	{
		return -1;
	}

	return 0;

	/*
	if(m_EsgParseState[pSeg->header.type].finished)
		return true;

	// check
	BYTE nMaxId = 0;
	bool bIdExist = false;
	for(BYTE n=0; n<m_EsgParseState[pSeg->header.type].curr_data_fragment_count; n++)
	{
		if(m_EsgParseState[pSeg->header.type].data_fragment_id[n] == pSeg->header.id)
		{
			// Indicate that this type esg has been parsed if exist this id
			bIdExist = true;
		}

		if(nMaxId < m_EsgParseState[pSeg->header.type].data_fragment_id[n])
			nMaxId = m_EsgParseState[pSeg->header.type].data_fragment_id[n];
	}

	if(bIdExist)
	{
		m_EsgParseState[pSeg->header.type].finished = true;
	}

	return m_EsgParseState[pSeg->header.type].finished;
	*/
}

void CDemultiplexer::ResetEsgSegment(EsgDataSegment* pSeg)
{
	if(pSeg->payload)
	{
		delete pSeg->payload;
		pSeg->payload = NULL;
	}

	memset(&pSeg->header, 0, sizeof(EsgDataSegmentHeader));
	pSeg->header.id = -1;
	pSeg->header.seg_sn = 0;
	pSeg->header.type = -1;
}

void CDemultiplexer::ResetEsgDataContainer(EsgDataContainer* pContainer)
{
	pContainer->reset();
}


int CDemultiplexer::ParseDataSegmentHeader(LPBYTE pData, DataSegmentHeader* pHeader)
{
	int nRet = DEMUX_OK;

	nRet = ::ParseDataSegmentHeader(pData, pHeader);

	if(nRet != DEMUX_OK)
	{
		if(m_pReciever)
			m_pReciever->WhenParsedFail(nRet, 0);
	}

	return nRet;
}


int CDemultiplexer::ParseVideo(BYTE* pData, MultiSubFrameHeader* pHeader)
{
	int nRet = DEMUX_OK;
	bool bEncrypt = (m_MF.multiplex_sub_frame.sub_frame_header->encrypt_flag == ENCRYPT_SCRAMBLE);

	VideoSegmentHeader header;
	nRet = ParseVideoSegmentHeader(pData, &header);

	if(DEMUX_OK != nRet)
	{
		ForceParse(pData, pHeader->video_seg_len, VIDEO_UNIT, nRet, m_dwCurrVideoTimeStamp);
		return nRet;
	}

	// skip header
	pData += header.header_len + VIDEO_SEGMENT_HEADER_CRC32_LEN;	

	// NOTE: currently each unit has a whole h264 frame, but this  not desc
	for(int i=0; i<header.video_unit_count; i++)
	{
		m_nVideoAlgorithm = m_MF.multiplex_sub_frame.sub_frame_header->video_stream_param[i].algorithm_type;

		if(header.video_unit_param[i].play_time_flag == 1)
			m_dwCurrVideoTimeStamp = pHeader->start_time_stamp + header.video_unit_param[i].relative_play_time;
		else
			m_dwCurrVideoTimeStamp = pHeader->start_time_stamp;
		

		switch(pHeader->packet_mode)
		{
		case PACKET_MODE_2:// mode 2
			{
				nRet = ParseUnitMode2(pData, VIDEO_UNIT, header.video_unit_param[i].unit_len, &header.video_unit_param[i]);
			}
			break;
		case PACKET_MODE_1:// mode 1
			{
				// Nagra:将payload送入解扰，输出亦为payload。若是起始帧，输出则为‘00 00 00 01’开始的数据?
				if(bEncrypt)
				{
					CaPacket packet;
					Decrypt(&packet, pData, header.video_unit_param[i].unit_len, MUX_SEGMENT_VIDEO);

					if (packet.encrypt)
					{
						m_VideoBuf.frame[0].dwLen	= packet.decrypt_data_len;
						m_VideoBuf.frame[0].pData	= packet.decrypt_data_ptr;
					}
				}
				else
				{
					m_VideoBuf.frame[0].dwLen		= header.video_unit_param[i].unit_len;
					m_VideoBuf.frame[0].pData		= pData;
				}

				m_VideoBuf.buf_count				= 1;
				m_VideoBuf.frame[0].dwTimeStamp		= m_dwCurrVideoTimeStamp;
				m_VideoBuf.frame[0].pos				= FRAME_WHOLE;
				
				// 20100616
				m_VideoBuf.frame[0].type			= FRAME_I;
				//m_VideoBuf.frame[0].type			= m_KeyFrameChecker.AnalyseData(m_VideoBuf.frame[0].pData, m_VideoBuf.frame[0].dwLen)?FRAME_I:FRAME_P;
				
				m_VideoBuf.frame[0].nCodecType		= m_nVideoAlgorithm;

				if(m_pReciever)
					m_pReciever->WhenParsedVideoFrame(&m_VideoBuf);
			}
			break;
		}
		
		pData += header.video_unit_param[i].unit_len;
	}
	
	return DEMUX_OK;
}

int CDemultiplexer::ParseVideoSegmentHeader(LPBYTE pData, VideoSegmentHeader* pHeader)
{
	int nRet = ::ParseVideoSegmentHeader(pData, pHeader);

	// notify
	if(m_pReciever && nRet!=DEMUX_OK)
		m_pReciever->WhenParsedFail(nRet, 0);

	return nRet;
}

bool CDemultiplexer::FindDraFrmFlag(LPBYTE& pData, int& nLen)
{
	DWORD dwSync = 0XFF7F0100;
	int nFlagLen = 4;
	int maxval = nLen - (nFlagLen - 1);
	for(int i=0; i<maxval; i++)
	{
		if (!memcmp(pData+i, &dwSync, nFlagLen))
		{
			pData += (i+2);
			nLen  -= (i+2);
			return true;
		}
	}

	return false;
}


// Dra Audio: Mode 1 
int CDemultiplexer::ParseDra(LPBYTE pData, int nLen)
{
	m_AudioBuf.buf_count = 0;

	LPBYTE lpData = pData;
	LPBYTE lpEnd = lpData + nLen;
	while(lpData < lpEnd)
	{
		int nLen = lpEnd - lpData;
		 
		// skip '00 01'
		//lpData += 2;
		bool bFound = FindDraFrmFlag(lpData, nLen);

		BYTE flag = (lpData[2]>>7) & 0x1;
		WORD length = ((lpData[2]<<8) | lpData[3]);

		if(flag == 1)
		{
			// 13 bits
			length = ((length & 0x7FFF) >> 2) << 2;
		}
		else
		{
			// 10 bits
			length = ((length & 0x7FFF) >> 5) << 2;
		}

		if(lpData + length > lpEnd)
			break;


		m_AudioBuf.frame[m_AudioBuf.buf_count].dwLen		= length;
		m_AudioBuf.frame[m_AudioBuf.buf_count].dwTimeStamp	= m_dwCurrAudioTimeStamp;
		m_AudioBuf.frame[m_AudioBuf.buf_count].pData		= lpData;
		m_AudioBuf.frame[m_AudioBuf.buf_count].nCodecType	= m_nAudioAlgorithm;
		m_AudioBuf.buf_count++;

		lpData += length;
	}

	AssembleAudioTimeStamp();

	if(m_pReciever)
		m_pReciever->WhenParsedAudioFrame(&m_AudioBuf);

	return DEMUX_OK;
}

int CDemultiplexer::ParseAac(LPBYTE pData, int nLen)
{
	m_AudioBuf.buf_count = 0;

	LPBYTE lpData = pData;
	LPBYTE lpEnd = lpData + nLen;
	while(lpData < lpEnd)
	{
		WORD length = lpData[0];

		if(lpData + length > lpEnd)
			break;

		switch(length)
		{
		case 0xff:
			length = 0x100 + lpData[1] - 1;
			lpData+=2;
			break;
		case 0xfe:
			length = 0x200 + lpData[1] - 1;
			lpData+=2;
			break;
		default:lpData++;break;
		}

		m_AudioBuf.frame[m_AudioBuf.buf_count].dwLen		= length;
		m_AudioBuf.frame[m_AudioBuf.buf_count].dwTimeStamp	= m_dwCurrAudioTimeStamp;
		m_AudioBuf.frame[m_AudioBuf.buf_count].pData		= lpData;
		m_AudioBuf.frame[m_AudioBuf.buf_count].nCodecType	= m_nAudioAlgorithm;
		m_AudioBuf.buf_count++;

		lpData += length;
	}

	AssembleAudioTimeStamp();

	if(m_pReciever)
		m_pReciever->WhenParsedAudioFrame(&m_AudioBuf);

	return DEMUX_OK;
}

int CDemultiplexer::ParseAacEx(LPBYTE pData, int nLen)
{
	m_AudioBuf.buf_count = 0;

	LPBYTE lpData = pData;
	LPBYTE lpEnd = lpData + nLen;
	while(lpData < lpEnd)
	{
		WORD length = lpData[0];

		if(lpData + length > lpEnd)
			break;

		switch(length)
		{
		case 0xff:
			length = 0x100 + lpData[1] - 1;
			lpData+=2;
			break;
		case 0xfe:
			length = 0x200 + lpData[1] - 1;
			lpData+=2;
			break;
		default:lpData++;break;
		}

		m_AudioBuf.frame[m_AudioBuf.buf_count].dwLen		= length;

		// 20100225
		m_AudioBuf.frame[m_AudioBuf.buf_count].dwTimeStamp	= 0;
		//m_AudioBuf.frame[m_AudioBuf.buf_count].dwTimeStamp	= m_dwCurrAudioTimeStamp;
		m_AudioBuf.frame[m_AudioBuf.buf_count].pData		= lpData;
		m_AudioBuf.frame[m_AudioBuf.buf_count].nCodecType	= m_nAudioAlgorithm;
		m_AudioBuf.buf_count++;

		lpData += length;
	}

	//AssembleAudioTimeStamp();

	if(m_pReciever)
		m_pReciever->WhenParsedAudioFrame(&m_AudioBuf);

	return DEMUX_OK;
}

int CDemultiplexer::ParseAudio(BYTE* pData, MultiSubFrameHeader* pHeader)
{
	bool bEncrypt = (m_MF.multiplex_sub_frame.sub_frame_header->encrypt_flag == ENCRYPT_SCRAMBLE);

	AudioSegmentHeader header;
	int nRet = ParseAudioSegmentHeader(pData, &header);
	
	if(DEMUX_OK != nRet)
	{
		ForceParse(pData, pHeader->audio_seg_len, AUDIO_UNIT, nRet, m_dwCurrAudioTimeStamp);
		return nRet;
	}	

	pData += header.header_len + AUDIO_SEGMENT_HEADER_CRC32_LEN;

	m_dwCurrAudioTimeStamp = pHeader->start_time_stamp;
	m_dwLastAudioTimeStamp = pHeader->start_time_stamp;

	for(BYTE n=0; n<header.unit_count; n++)
	{
		// 20090323
		m_dwCurrAudioTimeStamp = pHeader->start_time_stamp + header.audio_unit_param[n].relative_play_time;
		//CDmxDump::DumpLog(_T("[DMX] Audio start time = %u, relative play time = %u\n"), pHeader->start_time_stamp, header.audio_unit_param[n].relative_play_time);
		m_nAudioAlgorithm = m_MF.multiplex_sub_frame.sub_frame_header->audio_stream_param[0].algorithm_type;
		
		if (pHeader->start_time_stamp < m_dwCurrAudioTimeStamp)
		{
			//CDmxDump::DumpLog(_T("Audio Timestamp Error...\n"));
		}

		switch(pHeader->packet_mode)
		{
		case PACKET_MODE_1:
			{
				BYTE* pBakData		= pData;
				WORD  nBakUnitLen	= header.audio_unit_param[n].unit_len ;

				if(bEncrypt)
				{
					CaPacket packet;
					Decrypt(&packet, pData, header.audio_unit_param[n].unit_len, MUX_SEGMENT_AUDIO);

					if (packet.encrypt)
					{
						pData = packet.decrypt_data_ptr;
						header.audio_unit_param[n].unit_len = packet.decrypt_data_len;
					}				
					else // decrypt failed
					{
						pData += header.audio_unit_param[n].unit_len;
						continue;
					}
				}
	
				// DRA
				if(AUDIO_DRA == m_nAudioAlgorithm)
				{
					ParseDra(pData, header.audio_unit_param[n].unit_len);
				}
				else // AAC
				{
					ParseAac(pData, header.audio_unit_param[n].unit_len);
				}

				pData = pBakData;
				header.audio_unit_param[n].unit_len = nBakUnitLen;				
			}
			break;
		case PACKET_MODE_2:
			nRet = ParseUnitMode2(pData, AUDIO_UNIT, header.audio_unit_param[n].unit_len, &header.audio_unit_param[n], m_dwCurrAudioTimeStamp);
			break;
		default:
			break;
		}

		pData += header.audio_unit_param[n].unit_len;
	}

	return nRet;
}

void CDemultiplexer::AssembleAudioTimeStamp()
{
#if 0
	if (m_AudioBuf.buf_count > 1)
	{
		if (m_nAudioAlgorithm == AUDIO_DRA)
		{
			CDmxDump::DumpLog(_T("[DMX] DRA(mode %d): frame count = %d...\n"), m_MF.multiplex_sub_frame.sub_frame_header[0].packet_mode==PACKET_MODE_2?2:1, m_AudioBuf.buf_count);
		}
		else
		{
			CDmxDump::DumpLog(_T("[DMX] AAC(mode %d): frame count = %d...\n"), m_MF.multiplex_sub_frame.sub_frame_header[0].packet_mode==PACKET_MODE_2?2:1, m_AudioBuf.buf_count);
		}
	}
#endif


	DWORD dwInterval = 0;

	if (m_AudioBuf.buf_count > 1)
	{

		if (m_nAudioAlgorithm == AUDIO_DRA)
		{
			dwInterval = 480;
		}
		else
		{
			//dwInterval = 945;
			dwInterval = (m_dwCurrAudioTimeStamp-m_dwLastAudioTimeStamp) / m_AudioBuf.buf_count;
			//CDmxDump::DumpLog(_T("Audio interval = %d\n"), dwInterval);
		}
	}

	if (m_AudioBuf.buf_count == 1)
	{
		m_AudioBuf.frame[0].dwTimeStamp	= m_dwCurrAudioTimeStamp;
		m_AudioBuf.frame[0].nCodecType	= m_nAudioAlgorithm;
		m_dwLastAudioTimeStamp			= m_dwCurrAudioTimeStamp;
		//CDmxDump::DumpLog(_T("[DMX] Audio TS = %u\n"), m_dwCurrAudioTimeStamp);
	}
	else
	{
		//CDmxDump::DumpLogEx("audio count = %d", m_AudioBuf.buf_count);
		//CDmxDump::DumpLog(_T("[DMX] Audio Count > 1\n"));
		for(BYTE n=0; n<m_AudioBuf.buf_count; n++)
		{
			m_dwLastAudioTimeStamp += dwInterval;

			if (n == (m_AudioBuf.buf_count-1) )
			{
				m_dwLastAudioTimeStamp = m_dwCurrAudioTimeStamp;
			}

			m_AudioBuf.frame[n].nCodecType	= m_nAudioAlgorithm;
			m_AudioBuf.frame[n].dwTimeStamp = m_dwLastAudioTimeStamp;


			if (m_AudioBuf.frame[n].dwTimeStamp > m_dwCurrAudioTimeStamp)
			{
				//CDmxDump::DumpLog(_T("[DMX] Audio Time Stamp > Current Time Stamp\n"));
			}
		}
	}
}

int CDemultiplexer::ParseAudioSegmentHeader(LPBYTE pData, AudioSegmentHeader* pHeader)
{
	int nRet = ::ParseAudioSegmentHeader(pData, pHeader);

	//notify
	if(m_pReciever && nRet!=DEMUX_OK)
		m_pReciever->WhenParsedFail(nRet, 0);

	return nRet;
}


int CDemultiplexer::ParseUnitMode2(BYTE* pData, BYTE nUnitType, WORD wUnitLen, void* pUnitParam, DWORD dwPrevUnitTimeStamp)
{
	int nRet		= DEMUX_OK;
	BYTE* pStart	= pData;
	BYTE* pEnd		= pData + wUnitLen;
	int nPayloadLenBak = 0;

	// '00' no encrypt; '01'decrypt; '10' encrypt; '11' reserved 
	bool bEncrypt = (m_MF.multiplex_sub_frame.sub_frame_header->encrypt_flag == ENCRYPT_SCRAMBLE);

	//int nCount = 0;
	//每个单元的复用块个数无法知道，故只能通过起始指针来判断
	while(pEnd > pStart)
	{

		Mode2FragmentHeader header;
		int nRet = ::ParseFragmentMode2Header(pStart, &header);

		// 20090729
		if(DEMUX_OK != nRet)
		{
			if(m_bStopForceParse)
			{
				m_bStopForceParse = false;
				return nRet;
			}

			pStart++;
			continue;
		}
		
		/*
		if(DEMUX_OK != nRet)
		{
			if(m_pReciever)
				m_pReciever->WhenParsedFail(nRet, 0);

			if(!bEncrypt)
			{
				ForceParse(pStart, header.payload_len, nUnitType, nRet, nUnitType == VIDEO_UNIT?m_dwCurrVideoTimeStamp:m_dwCurrAudioTimeStamp);
				pStart += header.header_len + MODE2_FRAGMENT_CRC8_LEN + header.payload_len;
			}
			else //DEMUX_FRAGMENT_STRAT_CODE_ERROR
				pStart += 1;

			if(m_bStopForceParse)
			{
				m_bStopForceParse = false;
				return nRet;
			}

			continue;
		}
		*/
		

		//nCount++;
		pStart += header.header_len + MODE2_FRAGMENT_CRC8_LEN;

		//switch(header.fragment_type)
		switch(nUnitType)
		{
		case VIDEO_UNIT:
			{
				VideoSegmentHeader::VideoUnitParam* param = (VideoSegmentHeader::VideoUnitParam*)pUnitParam;
				m_nVideoFrameType = (FRAMETYPE)param->frame_type;
				m_VideoBuf.buf_count = 0;

				// Nagra:将payload送入解扰（不包含复用块头, 即不是0x55开头），输出亦为payload。若是起始帧，输出则为‘00 00 00 01’开始的数据
				if(bEncrypt)
				{
					CaPacket packet;
					Decrypt(&packet, pStart, header.payload_len, MUX_SEGMENT_VIDEO);

					if(packet.encrypt)
					{
						m_VideoBuf.frame[m_VideoBuf.buf_count].dwLen	= packet.decrypt_data_len;
						m_VideoBuf.frame[m_VideoBuf.buf_count].pData	= packet.decrypt_data_ptr;
						m_VideoBuf.frame[m_VideoBuf.buf_count].pos		= CalFramePos(header.start_flag, header.end_flag);
						m_VideoBuf.buf_count = 1;

						if(m_pReciever)
						{
							m_VideoBuf.frame[0].nCodecType	= m_nVideoAlgorithm;

							// 20100616
							m_VideoBuf.frame[0].type		= (FRAMETYPE)m_nVideoFrameType;
							//m_VideoBuf.frame[0].type			= m_KeyFrameChecker.AnalyseData(m_VideoBuf.frame[0].pData, m_VideoBuf.frame[0].dwLen)?FRAME_I:FRAME_P;

							m_VideoBuf.frame[0].dwTimeStamp = m_dwCurrVideoTimeStamp;

							m_pReciever->WhenParsedVideoFrame(&m_VideoBuf);
						}

					}

					nRet = DEMUX_OK;
				}
				else
					nRet = ParseFragmentMode2(pStart, &header, &m_VideoBuf);

				if(DEMUX_OK==nRet)
				{

					// 20090730
					// Mode 2下，若NAL type为STAP_A或STAP_B，复用块可能含有多个帧
// 					for(BYTE n=0; n<m_VideoBuf.buf_count; n++)
// 					{
// 						m_VideoBuf.frame[n].nCodecType	= m_nVideoAlgorithm;
// 						m_VideoBuf.frame[n].type		= (FRAMETYPE)param->frame_type;
// 						m_VideoBuf.frame[n].dwTimeStamp = m_dwCurrVideoTimeStamp;
// 					}

// 					if(m_pReciever)
// 						m_pReciever->WhenParsedVideoFrame(&m_VideoBuf);
				}
				else
				{
					if(m_pReciever)
						m_pReciever->WhenParsedFail(nRet, 0);

					// 20090305
					if ( (pEnd-pStart) < header.payload_len)
					{
						header.payload_len = (pEnd-pStart);
					}
					ForceParse(pStart, header.payload_len, VIDEO_UNIT, nRet, m_dwCurrVideoTimeStamp);
				}
			}
			break;
		case AUDIO_UNIT:
			{
				m_AudioBuf.buf_count = 0;

				// Nagra:将payload送入解扰（不包含复用块头,即不是以0x55开头），输出亦为payload。需要根据输出的第一个字节或第二个字节来计算audio的输出长度
				if(bEncrypt)
				{
					CaPacket packet;
					Decrypt(&packet, pStart, header.payload_len, MUX_SEGMENT_AUDIO);

					if(packet.encrypt)
					{
						nPayloadLenBak = header.payload_len;
						header.payload_len = packet.decrypt_data_len;
						
						if((m_nAudioAlgorithm==AUDIO_AAC) || m_nAudioAlgorithm==AUDIO_HE_AAC)
							nRet = ParseFragmentMode2(packet.decrypt_data_ptr, &header, &m_AudioBuf);
						else
							nRet = ::ParseDraFragmentMode2(packet.decrypt_data_ptr, &header, &m_AudioBuf);

						header.payload_len = nPayloadLenBak;
					}

					nRet = DEMUX_OK;
				}
				else
				{
					if((m_nAudioAlgorithm==AUDIO_AAC) || m_nAudioAlgorithm==AUDIO_HE_AAC)
						nRet = ParseFragmentMode2(pStart, &header, &m_AudioBuf);
					else
						nRet = ::ParseDraFragmentMode2(pStart, &header, &m_AudioBuf);
				}

				if(DEMUX_OK==nRet)
				{
					AssembleAudioTimeStamp();

					if(m_pReciever)
						m_pReciever->WhenParsedAudioFrame(&m_AudioBuf);
				}
				else
				{
					if(m_pReciever)
						m_pReciever->WhenParsedFail(nRet, 0);
					
					// 20090305
					if ( (pEnd-pStart) < header.payload_len)
					{
						header.payload_len = (pEnd-pStart);
					}
					ForceParse(pStart, header.payload_len, AUDIO_UNIT, nRet, m_dwCurrAudioTimeStamp);
				}
			}
			break;
		case DATA_UNIT:
			{
				BYTE nDataType = *(BYTE*)pUnitParam;

				if(nDataType == DATA_PROGRAM_GUIDE)
				{
					ProgramGuideInfo guide;
					nRet = ParseProgramGuide(pStart, &guide);
				}
				else if(nDataType == DATA_ESG)
				{
					EsgDataSegment seg;
					nRet = ParseEsgSegment(pStart, &seg);
				}
			}
			break;
		default:
			break;
		}

		pStart += header.payload_len;
	}

	//CDmxDump::DumpLog(_T("[DMX] Fragment count in one Unit = %d, Unit type = %s\n"), nCount, nUnitType==0?_T("Video"):_T("Audio"));

	return DEMUX_OK;
}

// force parse MF according the error
int CDemultiplexer::ForceParse(BYTE* pData, int nLen, BYTE nType, int nErrorType, DWORD dwTimeStamp)
{
	if(m_MF.multiplex_sub_frame.sub_frame_header->encrypt_flag == ENCRYPT_SCRAMBLE)
		return DEMUX_OK;

	// 20090729
	dwTimeStamp = 0;
	nType = UNKNOWN_UNIT;

	int nRet = DEMUX_OK;

	switch (nErrorType)
	{
	case DEMUX_MF_START_CODE_ERROR:
	case DEMUX_MF_HEADER_CRC_ERROR:
	case DEMUX_MF_HEADER_FAILED:

	case DEMUX_MSF_HEADER_FAILED:

	default:
		{
			if(nType==AUDIO_UNIT && m_nAudioAlgorithm==AUDIO_DRA)
			{
				return ParseDra(pData, nLen);
			}
	
			//check version
			BYTE* pStart = pData;
			BYTE* pEnd = pData + nLen;

			BYTE nOffset = 4;
			//while((pEnd-nOffset) > pStart)
			while(pStart + nOffset < pEnd)
			{
				if(m_bStopForceParse)
				{
					m_bStopForceParse = false;
					return DEMUX_OK;
				}

				BYTE startByte = STEP8(pStart);

				if(startByte == MODE2_FRAGMENT_START_CODE)
				{
					int start, end;
					WORD temp			= STEP16(pStart);
					WORD pkLen			= temp&0xfff;
					BYTE crc8			= STEP8(pStart);
					FRAMEPOS startendInd    = FRAME_MID;
					BYTE pkType			= (temp>>12) & 3;

					int type = nType;
					if(nType == UNKNOWN_UNIT)
						type = pkType;

					start = (temp>>15) & 1;
					end   = (temp>>14) & 1;
					if (start == 0)
					{
						if (end == 1)
							startendInd = FRAME_END;
						else
							startendInd = FRAME_MID;
					}
					else if (start == 1)
					{
						if (end != 1)
						{
							startendInd = FRAME_BEGIN;
						}
						else
						{
							startendInd = FRAME_WHOLE;
						}
					}
					
					// 20090217
					int nCurrLen = pEnd - pStart + nOffset;
					if(pkLen > nCurrLen)
						pkLen = nCurrLen;
					// end

					switch(type)
					{
					case VIDEO_UNIT:
						{
							// need add 00 00 01? FRAME_I? 20100610
							m_VideoBuf.buf_count			= 1;
							m_VideoBuf.frame[0].dwLen		= pkLen;
							m_VideoBuf.frame[0].dwTimeStamp = dwTimeStamp;
							m_VideoBuf.frame[0].pData		= pStart;
							m_VideoBuf.frame[0].pos			= startendInd;
							
							// 20100616
							m_VideoBuf.frame[0].type		= FRAME_I;
							//m_VideoBuf.frame[0].type		= m_KeyFrameChecker.AnalyseData(m_VideoBuf.frame[0].pData, m_VideoBuf.frame[0].dwLen)?FRAME_I:FRAME_P;


							if(m_pReciever)
								m_pReciever->WhenParsedVideoFrame(&m_VideoBuf);
						}
						break;
					case AUDIO_UNIT:
						{
							// 20090729
							ParseAacEx(pStart, pkLen);
							/*
							m_AudioBuf.buf_count			= 1;
							m_AudioBuf.frame[0].dwLen		= pkLen;
							m_AudioBuf.frame[0].dwTimeStamp	= dwTimeStamp;
							m_AudioBuf.frame[0].pData		= pStart;

							if(m_pReciever)
							{
								m_pReciever->WhenParsedAudioFrame(&m_AudioBuf);
							}
							*/
						}
						break;
					case DATA_UNIT:
						{

						}
						break;
					}

					pStart += pkLen;
				}
			}
		}
		break;
	}

	return nRet;
}

bool CDemultiplexer::IsEmmFrame()
{
	return m_MF.frame_header.mf_id == m_nEmmMfId;

	for (int n=0; n<m_MF.ts0_info.ea_desc.segment_count; n++)
	{
		for (int m=0; m<m_MF.ts0_info.cmct.mf_num; m++)
		{
			for (int i=0; i<m_MF.ts0_info.cmct.mf_info[m].msf_num; i++)
			{
				if (m_MF.ts0_info.cmct.mf_info[m].msf_info[i].service_id == m_MF.ts0_info.ea_desc.ea_data_desc[n].service_id)
				{
					if (m_MF.frame_header.mf_id == m_MF.ts0_info.cmct.mf_info[m].mf_id)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}


int CDemultiplexer::ParseFragmentMode2(BYTE* pData, Mode2FragmentHeader* pHeader, void* pOut)
{
	int nRet = DEMUX_OK;

	switch(pHeader->fragment_type)
	{
	case VIDEO_UNIT:
		{
			nRet = CDemultiplexer::ParseMode2VideoFragment(pData, pHeader, CalFramePos(pHeader->start_flag, pHeader->end_flag), (VideoUnitBuf*)pOut);
		}
		break;
	case AUDIO_UNIT:
		{
			nRet = ::ParseMode2AudioFragment(pData, pHeader, (AudioUnitBuf*)pOut);
		}
		break;
	case DATA_UNIT:
		{

		}
		break;
	}

	return nRet;
}


//DWORD ts, BYTE * buf, int len,NAL_FRAGMENT_POS_IND StartEndInd
int CDemultiplexer::ParseMode2VideoFragment(BYTE* pData, Mode2FragmentHeader* pHeader, FRAMEPOS nPos, VideoUnitBuf* pFrame)
{
	WORD len = pHeader->payload_len;
	BYTE nal_type = pData[0] & 0x1f;

	/*
	Neither an aggregation packet nor a fragmentation unit can be used within a
	single NAL unit packet.  A NAL unit stream composed by decapsulating
	single NAL unit packets in RTP sequence number order MUST conform to
	the NAL unit decoding order.  The structure of the single NAL unit
	packet is shown in Figure.

	Informative note: The first byte of a NAL unit co-serves as the
	RTP payload header.

	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|F|NRI|  type   |                                               |
	+-+-+-+-+-+-+-+-+                                               |
	|                                                               |
	|               Bytes 2..n of a Single NAL unit                 |
	|                                                               |
	|                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                               :...OPTIONAL RTP padding        |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	*/

	if(nal_type == 0)
	{
		pFrame->frame[0].pData = pData;
		pFrame->frame[0].dwLen = pHeader->payload_len;
		pFrame->frame[0].pos = nPos;

		if(m_pReciever)
		{
			pFrame->buf_count = 1;
			pFrame->frame[0].nCodecType	= m_nVideoAlgorithm;


			// 20100616
			pFrame->frame[0].type		= (FRAMETYPE)m_nVideoFrameType;
			//m_VideoBuf.frame[0].type	= m_KeyFrameChecker.AnalyseData(m_VideoBuf.frame[0].pData, m_VideoBuf.frame[0].dwLen)?FRAME_I:FRAME_P;

			pFrame->frame[0].dwTimeStamp = m_dwCurrVideoTimeStamp;
			m_pReciever->WhenParsedVideoFrame(pFrame);
		}

		return DEMUX_OK;
	}

	if(nal_type == 12)
	{
		// 20090731
// 		pFrame->frame[0].pData = NULL;
// 		pFrame->frame[0].dwLen = 0;
// 		pFrame->frame[0].pos = FRAME_END;

		pFrame->frame[0].pData = pData;
		pFrame->frame[0].dwLen = pHeader->payload_len;
		pFrame->frame[0].pos = nPos;
		
		if(m_pReciever)
		{
			pFrame->buf_count = 1;
			pFrame->frame[0].nCodecType	= m_nVideoAlgorithm;
			// 20100616
			pFrame->frame[0].type		= (FRAMETYPE)m_nVideoFrameType;
			//m_VideoBuf.frame[0].type		= m_KeyFrameChecker.AnalyseData(m_VideoBuf.frame[0].pData, m_VideoBuf.frame[0].dwLen)?FRAME_I:FRAME_P;

			pFrame->frame[0].dwTimeStamp = m_dwCurrVideoTimeStamp;
			m_pReciever->WhenParsedVideoFrame(pFrame);
		}

		return DEMUX_OK;
	}

	if (nal_type >= 1 && nal_type <= SINGLE_NAL)// 23
	{
		pData[-3] = 0;
		pData[-2] = 0;
		pData[-1] = 1;

		pFrame->frame[0].pData = pData-3;
		pFrame->frame[0].dwLen = pHeader->payload_len+3;
		pFrame->frame[0].pos = nPos;
		
		if(m_pReciever)
		{
			pFrame->buf_count = 1;
			pFrame->frame[0].nCodecType	= m_nVideoAlgorithm;
			
			// 20100616
			pFrame->frame[0].type		= (FRAMETYPE)m_nVideoFrameType;
			//pFrame->frame[0].type		= m_KeyFrameChecker.AnalyseData(pFrame->frame[0].pData, pFrame->frame[0].dwLen)?FRAME_I:FRAME_P;

			pFrame->frame[0].dwTimeStamp = m_dwCurrVideoTimeStamp;
			m_pReciever->WhenParsedVideoFrame(pFrame);
		}

		return DEMUX_OK;
	}

	/*
	STAP		- Single-time Aggregation Packet
	STAP-A		- Single-time Aggregation Packet without DON(Decoder Order Number)
	STAP-B		- Single-time Aggregation Packet including DON(Decoder Order Number)

	Single-time aggregation packet (STAP): aggregates NAL units with
	identical NALU-time.  Two types of STAPs are defined, one without
	DON (STAP-A) and another including DON (STAP-B).

	Single-time aggregation packet (STAP) SHOULD be used whenever NAL
	units are aggregated that all share the same NALU-time.  The payload
	of an STAP-A does not include DON and consists of at least one
	single-time aggregation unit, as presented in Figure 4.  The payload
	of an STAP-B consists of a 16-bit unsigned decoding order number
	(DON) (in network byte order) followed by at least one single-time
	aggregation unit, as presented in Figure 5.

	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	:                                               |
	+-+-+-+-+-+-+-+-+                                               |
	|                                                               |
	|                single-time aggregation units                  |
	|                                                               |
	|                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                               :
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	Figure 4.  Payload format for STAP-A

	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	:  decoding order number (DON)  |               |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+               |
	|                                                               |
	|                single-time aggregation units                  |
	|                                                               |
	|                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                               :
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	Figure 5.  Payload format for STAP-B

	//
	Figure 7 presents an example of an RTP packet that contains an STAP-
	A.  The STAP contains two single-time aggregation units, labeled as 1
	and 2 in the figure.

	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                          RTP Header                           |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|STAP-A NAL HDR |         NALU 1 Size           | NALU 1 HDR    |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                         NALU 1 Data                           |
	:                                                               :
	+               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|               | NALU 2 Size                   | NALU 2 HDR    |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                         NALU 2 Data                           |
	:                                                               :
	|                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                               :...OPTIONAL RTP padding        |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	Figure 7.  An example of an RTP packet including an STAP-A and two
	single-time aggregation units


	Figure 8 presents an example of an RTP packet that contains an STAP-
	B.  The STAP contains two single-time aggregation units, labeled as 1
	and 2 in the figure.

	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                          RTP Header                           |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|STAP-B NAL HDR | DON                           | NALU 1 Size   |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	| NALU 1 Size   | NALU 1 HDR    | NALU 1 Data                   |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
	:                                                               :
	+               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|               | NALU 2 Size                   | NALU 2 HDR    |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                       NALU 2 Data                             |
	:                                                               :
	|                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                               :...OPTIONAL RTP padding        |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	Figure 8.  An example of an RTP packet including an STAP-B and two
	single-time aggregation units

	*/

	if(nal_type==STAP_A || nal_type==STAP_B)                   // STAP (one packet, multiple nals)
	{
		// skip type
		pData++;
		len--;

		if (nal_type == STAP_B)
		{
			// skip DON, 16 bits
			pData	+= 2;
			len		-= 2;
			//OutputDebugString(_T("[DMX]STAP_B \n"));
		}
		else
		{
			//OutputDebugString(_T("[DMX]STAP_A \n"));
		}

		BYTE *src= pData;

		BYTE	start	= FRAME_BEGIN;
		BYTE	end		= FRAME_END;
		BYTE	lastnal = FRAME_MID;

		BYTE count = 0;
		FRAMEPOS pos = nPos;

		while(src < pData + len - 2)
		{
			WORD nal_size = GET16(src);

			if(src + nal_size + 2 > pData + len)
			{
				break;
			}

			src[-1] = 0;
			src[0] = 0;
			src[1] = 1;

			count++;
			lastnal =( ((src+nal_size + 2)>=(pData + len - 2))?FRAME_END:FRAME_MID);

			if(nPos == FRAME_WHOLE)
			{
				if(lastnal == FRAME_END)
					pos = count==1?FRAME_WHOLE:FRAME_END;
				else
					pos = count==1?FRAME_BEGIN:FRAME_MID;
			}
			else if(nPos == FRAME_BEGIN)
			{
				if(lastnal == FRAME_END)
					pos = count==1?FRAME_BEGIN:FRAME_MID;
				else if(lastnal == FRAME_MID)
					pos = count==1?FRAME_BEGIN:FRAME_MID;
			}
			else if(nPos == FRAME_END)
			{
				if(lastnal == FRAME_END)
					pos = FRAME_END;
				else if(lastnal == FRAME_MID)
					pos = FRAME_MID;
			}


			pFrame->frame[0].pData = src-1;
			pFrame->frame[0].dwLen = nal_size+3;
			pFrame->frame[0].pos = pos;
			
			if(m_pReciever)
			{
				pFrame->buf_count = 1;
				pFrame->frame[0].nCodecType	= m_nVideoAlgorithm;


				// 20100616
				pFrame->frame[0].type		= (FRAMETYPE)m_nVideoFrameType;
				//pFrame->frame[0].type		= m_KeyFrameChecker.AnalyseData(pFrame->frame[0].pData, pFrame->frame[0].dwLen)?FRAME_I:FRAME_P;

				pFrame->frame[0].dwTimeStamp = m_dwCurrVideoTimeStamp;
				m_pReciever->WhenParsedVideoFrame(pFrame);
			}


			start = FRAME_MID;
			src += nal_size + 2;
		}
		return DEMUX_OK;
	}

	/*
	Multi-time aggregation packet (MTAP): aggregates NAL units with
	potentially differing NALU-time.  Two different MTAPs are defined,
	differing in the length of the NAL unit timestamp offset.
	*/


	/*
	Figure 12 presents an example of an RTP packet that contains a
	multi-time aggregation packet of type MTAP16 that contains two
	multi-time aggregation units, labeled as 1 and 2 in the figure.

	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                          RTP Header                           |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|MTAP16 NAL HDR |  decoding order number base   | NALU 1 Size   |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|  NALU 1 Size  |  NALU 1 DOND  |       NALU 1 TS offset        |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|  NALU 1 HDR   |  NALU 1 DATA                                  |
	+-+-+-+-+-+-+-+-+                                               +
	:                                                               :
	+               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|               | NALU 2 SIZE                   |  NALU 2 DOND  |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|       NALU 2 TS offset        |  NALU 2 HDR   |  NALU 2 DATA  |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+               |
	:                                                               :
	|                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                               :...OPTIONAL RTP padding        |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	Figure 12.  An RTP packet including a multi-time aggregation
	packet of type MTAP16 and two multi-time aggregation
	units
	*/

	if(nal_type == MTAP_16|| nal_type == MTAP_24)
	{
		//OutputDebugString(_T("nal type = 26,27\n"));
		return DEMUX_UNSUPORT_H264_NAL_TYPE;
	}

	/*
	Figure 14 presents the RTP payload format for FU-As.  An FU-A
	consists of a fragmentation unit indicator of one octet, a
	fragmentation unit header of one octet, and a fragmentation unit
	payload.

	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	| FU indicator  |   FU header   |                               |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               |
	|                                                               |
	|                         FU payload                            |
	|                                                               |
	|                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                               :...OPTIONAL RTP padding        |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	Figure 14.  RTP payload format for FU-A


	Figure 15 presents the RTP payload format for FU-Bs.  An FU-B
	consists of a fragmentation unit indicator of one octet, a
	fragmentation unit header of one octet, a decoding order number (DON)
	(in network byte order), and a fragmentation unit payload.  In other
	words, the structure of FU-B is the same as the structure of FU-A,
	except for the additional DON field.

	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	| FU indicator  |   FU header   |               DON             |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-|
	|                                                               |
	|                         FU payload                            |
	|                                                               |
	|                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                               :...OPTIONAL RTP padding        |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	Figure 15.  RTP payload format for FU-B


	/////////////////////

	The FU indicator octet has the following format:

	+---------------+
	|0|1|2|3|4|5|6|7|
	+-+-+-+-+-+-+-+-+
	|F|NRI|  Type   |
	+---------------+


	The FU header has the following format:

	+---------------+
	|0|1|2|3|4|5|6|7|
	+-+-+-+-+-+-+-+-+
	|S|E|R|  Type   |
	+---------------+

	S: 1 bit
	When set to one, the Start bit indicates the start of a fragmented
	NAL unit.  When the following FU payload is not the start of a
	fragmented NAL unit payload, the Start bit is set to zero.

	E: 1 bit
	When set to one, the End bit indicates the end of a fragmented NAL
	unit, i.e., the last byte of the payload is also the last byte of
	the fragmented NAL unit.  When the following FU payload is not the
	last fragment of a fragmented NAL unit, the End bit is set to
	zero.

	R: 1 bit
	The Reserved bit MUST be equal to 0 and MUST be ignored by the
	receiver.

	Type: 5 bits
	The NAL unit payload type as defined in table 7-1 of [1].
	*/
	if(nal_type==FU_A || nal_type==FU_B)                   // FU-A (fragmented nal)
	{
		BYTE fu_indicator = pData[0];

		// skip type
		pData++;
		len--;

		BYTE fu_header	= *pData;   // read the fu_header.
		BYTE start_bit	= fu_header >> 7;
		BYTE end_bit	= (fu_header >> 6) & 1;
		BYTE reconstructed_nal;

		reconstructed_nal = fu_indicator & 0xe0;	// 11100000 the original nal forbidden bit and NRI are stored in this packet's nal;
		reconstructed_nal |= fu_header&0x1f;		// 00011111

		// skip the fu_header...
		pData++;
		len--;

		if (nal_type == FU_B)
		{
			//skip DON, 16 bits
			pData	+= 2;
			len		-= 2;
			//OutputDebugString(_T("[DMX]FU_B \n"));
		}

		if(start_bit)
		{
			pData[-4] = 0;
			pData[-3] = 0;
			pData[-2] = 1;
			pData[-1] = reconstructed_nal;

			pFrame->frame[0].pData = pData - 4;
			pFrame->frame[0].dwLen = len   + 4;
			pFrame->frame[0].pos = nPos;

			if(m_pReciever)
			{
				pFrame->buf_count = 1;
				pFrame->frame[0].nCodecType	= m_nVideoAlgorithm;

				// 20100616
				pFrame->frame[0].type		= (FRAMETYPE)m_nVideoFrameType;
				//pFrame->frame[0].type		= m_KeyFrameChecker.AnalyseData(pFrame->frame[0].pData, pFrame->frame[0].dwLen)?FRAME_I:FRAME_P;

				pFrame->frame[0].dwTimeStamp = m_dwCurrVideoTimeStamp;
				m_pReciever->WhenParsedVideoFrame(pFrame);
			}
		}
		else
		{
			pFrame->frame[0].pData = pData;
			pFrame->frame[0].dwLen = len;
			pFrame->frame[0].pos = nPos;
			if(m_pReciever)
			{
				pFrame->buf_count = 1;
				pFrame->frame[0].nCodecType	= m_nVideoAlgorithm;

				// 20100616
				pFrame->frame[0].type		= (FRAMETYPE)m_nVideoFrameType;
				//pFrame->frame[0].type		= m_KeyFrameChecker.AnalyseData(pFrame->frame[0].pData, pFrame->frame[0].dwLen)?FRAME_I:FRAME_P;

				pFrame->frame[0].dwTimeStamp = m_dwCurrVideoTimeStamp;
				m_pReciever->WhenParsedVideoFrame(pFrame);
			}
		}

		return DEMUX_OK;
	}

#ifdef WIN32
	OutputDebugString(_T("nal type = DEMUX_UNSUPORT_H264_NAL_TYPE\n"));
#endif

	return DEMUX_UNSUPORT_H264_NAL_TYPE;
}

int  CDemultiplexer::CalMfsLen(BYTE* pData)
{
	if(DEMUX_OK != ParseMFHeader(pData, &m_MF.frame_header))
		return 0;

	int nLen = m_MF.frame_header.header_len + MF_HEADER_CRC32_LEN;

	for (BYTE n=0; n<m_MF.frame_header.multi_sub_frame_num; n++)
	{
		nLen += m_MF.frame_header.multi_sub_frame_len[n];
	}

	return nLen;
}