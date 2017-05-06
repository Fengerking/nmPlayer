
#include "CDemultiplexerTp.h"
#include "dmxbasicapi.h"


CDemultiplexerTp::CDemultiplexerTp(void)
{
}

CDemultiplexerTp::~CDemultiplexerTp(void)
{
}

bool CDemultiplexerTp::FindStartCode(LPBYTE& pData, int& nLen)
{
	//DWORD dwSync = 0X01000000;
	DWORD dwSync = 0X55dbaabd;

	int maxval = nLen - (MF_START_CODE_LEN - 1);
	for(int i=0; i<maxval; i++)
	{
		if (!memcmp(pData+i, &dwSync, MF_START_CODE_LEN))
		{
			pData += i;
			nLen  -= i;
			return true;
		}
	}

	return false;
}

bool CDemultiplexerTp::GetMfLen(LPBYTE pData, int nDataLen, int& nMfLen)
{
	pData += (1+MF_START_CODE_LEN);

	// get MSF len
	nMfLen = *(pData+5);
	nMfLen = (nMfLen<<8) | *(pData+6);
	nMfLen = (nMfLen<<8) | *(pData+7);

	// 32 is MF header len
	nMfLen += 32;

	return nMfLen <= nDataLen;
}

int CDemultiplexerTp::Demux(BYTE* pData, int nLen, WORD wServiceId/*=0xffff*/)
{
	//OutputDebugString(_T("tp demux begin.\n"));
	
	int nCopyLen = 0;
	// not start with start code
	if(m_RawData.nLeftLen != 0)
	{
		if(nLen >= m_RawData.nLeftLen)
		{
			nCopyLen = m_RawData.nLeftLen;
			CopyRawData(&m_RawData, pData, nCopyLen);
			pData += nCopyLen;
			nLen -= nCopyLen;
			//CDmxDump::DumpLog(_T("copy end of frame, copy = %d, data left = %d....\n"), nCopyLen, nLen);
		}
		else
		{
			CopyRawData(&m_RawData, pData, nLen);
			//CDmxDump::DumpLog(_T("1 - Return, copy = %d, left to copy = %d....\n"), nLen, m_RawData.nLeftLen);
			return DEMUX_OK;
		}
	}
	else
	{
		// found start code
		if(FindStartCode(pData, nLen))
		{
			BYTE frm_type = *(pData+MF_START_CODE_LEN);

			if(frm_type!=FRAME_TYPE_ESG_DATA_FRAME && frm_type!=FRAME_TYPE_CONTROL_INFO && frm_type!=FRAME_TYPE_MULTIPLEX_SUBFRAME)
				return DEMUX_OK;

			// get mf len by bits, m_RawData.nLeftLen is result
			GetMfLen(pData, nLen, m_RawData.nLeftLen);

			if(nLen >= m_RawData.nLeftLen)
			{
				nCopyLen = m_RawData.nLeftLen;
				CopyRawData(&m_RawData, pData, nCopyLen);
				pData += nCopyLen;
				nLen  -= nCopyLen;
				//CDmxDump::DumpLog(_T("copy whole frame, copy = %d, data left = %d....\n"), nCopyLen, nLen);
			}
			else
			{
				CopyRawData(&m_RawData, pData, nLen);
				//CDmxDump::DumpLog(_T("2 - Return.copy = %d, left to copy = %d...\n"), nLen, m_RawData.nLeftLen);

				return DEMUX_OK;
			}
		}
		else// no found start code
		{
			if(m_RawData.nLeftLen != 0)
			{
				CopyRawData(&m_RawData, pData, nLen);
			}

			//CDmxDump::DumpLog(_T("no found start code.\n"));
			return DEMUX_OK;	
		}
	}

		
	int nRet = DEMUX_OK;
	m_MF.frame_header.mf_id					= 10;
	m_MF.frame_header.header_len			= 32;
	m_MF.frame_header.multi_sub_frame_num	= 1;

	BYTE* pNewData	= m_RawData.pRawData;
	int nNewLen		= m_RawData.nCurrLen;
	//BYTE* pTmp = pData;
	BYTE* pTmp = pNewData;
	
	// skip start code
	STEP32(pTmp);

	BYTE frametype = STEP8(pTmp);
	FRAME_CTRL_INFO_SUB_TYPE_E nSubType;
	
	m_MF.frame_header.multi_sub_frame_len[0] = *(pTmp+5);
	m_MF.frame_header.multi_sub_frame_len[0] = (m_MF.frame_header.multi_sub_frame_len[0]<<8) | *(pTmp+6);
	m_MF.frame_header.multi_sub_frame_len[0] = (m_MF.frame_header.multi_sub_frame_len[0]<<8) | *(pTmp+7);
	
	WORD serviceID = STEP16(pTmp);

	if(frametype == FRAME_TYPE_ESG_DATA_FRAME)
		m_MF.frame_header.mf_id = 1;
	else if(frametype == FRAME_TYPE_CONTROL_INFO)
		m_MF.frame_header.mf_id = 0;
	else if(frametype == FRAME_TYPE_MULTIPLEX_SUBFRAME)
	{
		if(601 == serviceID)
			m_MF.frame_header.mf_id = 2;
		else if(602 == serviceID)
			m_MF.frame_header.mf_id = 3;
		else if(603 == serviceID)
			m_MF.frame_header.mf_id = 4;
		else if(604 == serviceID)
			m_MF.frame_header.mf_id = 5;
		else if(605 == serviceID)
			m_MF.frame_header.mf_id = 6;
		else if(606 == serviceID)
			m_MF.frame_header.mf_id = 7;
		else if(701 == serviceID)
			m_MF.frame_header.mf_id = 8;
		else if(702 == serviceID)
			m_MF.frame_header.mf_id = 9;
		else if(2001 == serviceID)
			m_MF.frame_header.mf_id = 10;
		else if(691 == serviceID)
			m_MF.frame_header.mf_id = 11;
		else if(607 == serviceID)
			m_MF.frame_header.mf_id = 12;
		else if(608 == serviceID)
			m_MF.frame_header.mf_id = 13;
	}
	else
	{
		m_RawData.nCurrLen	= 0;
		m_RawData.nLeftLen	= 0;
		//CDmxDump::DumpLog(_T("Ignore telepath frame, type = %d\n"), frametype);
		return DEMUX_OK;
	}
		


	// skip MF header
	pNewData += 32;
	nNewLen -= (4 + 28);

	if(TS0_MF_ID == m_MF.frame_header.mf_id)
	{
		// parse ts0
		nRet = ParseTS0(pNewData, &m_MF.frame_header);
	}
	else
	{
		// parse msf
		//CDmxDump::DumpLog(_T("Parse telepath MSF, start code = %08x\n"), *((DWORD*)(pNewData-32)));
		nRet = ParseMSF(pNewData, nNewLen);
	}

	if(DEMUX_OK == nRet)
	{
		if(m_pReciever)
			m_pReciever->WhenParseWholeMFFinished(&m_MF);
	}

	m_RawData.nCurrLen	= 0;
	m_RawData.nLeftLen	= 0;

	if(nLen > 0)
	{
		//CopyRawData(&m_RawData, pData, nLen);
		//CDmxDump::DumpLog(_T("left data : %d.\n"), nLen);
		Demux(pData, nLen);
	}
		

	return nRet;
}


int CDemultiplexerKTouch::ParseUnitMode2(BYTE* pData, BYTE nUnitType, WORD wUnitLen, void* pUnitParam, DWORD dwPrevUnitTimeStamp)
{
	int nRet		= DEMUX_OK;
	BYTE* pStart	= pData;
	BYTE* pEnd		= pData + wUnitLen;

	// '00' no encrypt; '01'decrypt; '10' encrypt; '11' reserved 
	bool bEncrypt = (m_MF.multiplex_sub_frame.sub_frame_header->encrypt_flag == ENCRYPT_SCRAMBLE);


	//每个单元的复用块个数无法知道，故只能通过起始指针来判断
	while(pEnd > pStart)
	{
		Mode2FragmentHeader header;
		int nRet = ::ParseFragmentMode2Header(pStart, &header);

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

		pStart += header.header_len + MODE2_FRAGMENT_CRC8_LEN;

		//switch(header.fragment_type)
		switch(nUnitType)
		{
		case VIDEO_UNIT:
			{
				m_VideoBuf.buf_count = 0;

				// Tianyu CA卡：对于video， payload需要跳过前面8字节的解扰数据
				if(bEncrypt)
				{
					m_VideoBuf.frame[m_VideoBuf.buf_count].dwLen	= header.payload_len - 8;
					m_VideoBuf.frame[m_VideoBuf.buf_count].pData	= pStart + 8;
					m_VideoBuf.frame[m_VideoBuf.buf_count].pos		= CalFramePos(header.start_flag, header.end_flag);
					m_VideoBuf.buf_count = 1;

					nRet = DEMUX_OK;
				}
				else
					nRet = ParseFragmentMode2(pStart, &header, &m_VideoBuf);

				if(DEMUX_OK==nRet)
				{
					VideoSegmentHeader::VideoUnitParam* param = (VideoSegmentHeader::VideoUnitParam*)pUnitParam;

					for(BYTE n=0; n<m_VideoBuf.buf_count; n++)
					{
						m_VideoBuf.frame[n].nCodecType	= m_nVideoAlgorithm;

						// 20100616
						m_VideoBuf.frame[0].type		= (FRAMETYPE)m_nVideoFrameType;
						//m_VideoBuf.frame[0].type		= m_KeyFrameChecker.AnalyseData(m_VideoBuf.frame[0].pData, m_VideoBuf.frame[0].dwLen)?FRAME_I:FRAME_P;

						//m_VideoBuf.frame[n].pos			= CalFramePos(header.start_flag, header.end_flag);
						m_VideoBuf.frame[n].dwTimeStamp = m_dwCurrVideoTimeStamp;
					}

					if(m_pReciever)
						m_pReciever->WhenParsedVideoFrame(&m_VideoBuf);
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

				// Tianyu CA卡：对于audio， payload需要跳过前面9字节的解扰数据
				if(bEncrypt)
				{
					// to do: 这里需要注意区分DRA和AAC
					m_AudioBuf.frame[m_AudioBuf.buf_count].dwLen	= header.payload_len - 9;
					m_AudioBuf.frame[m_AudioBuf.buf_count].pData	= pStart + 9;
					m_AudioBuf.buf_count = 1;

					nRet = DEMUX_OK;
				}
				else
				{
					if((m_nAudioAlgorithm==AUDIO_AAC) || m_nAudioAlgorithm==AUDIO_HE_AAC)
						nRet = ParseFragmentMode2(pStart, &header, &m_AudioBuf);
					else
						nRet = ParseDraFragmentMode2(pStart, &header, &m_AudioBuf);
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

	return DEMUX_OK;
}

int CDemultiplexerKTouch::ParseEcm(BYTE* pData, int nLen, BYTE nEcmType)
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

		// post mux segment
		if(m_pReciever)
		{
			//CDmxDump::DumpLog(_T("Send ECM : Len = %d, %02x %02x\n"), header.payload_len+5, pStart[0], pStart[1]);

			// 以0x55开始
			CaPacket packet;
			Decrypt(&packet, pStart, header.payload_len+header.header_len+MODE2_FRAGMENT_CRC8_LEN, MUX_SEGMENT_ECM, nEcmType);
		}

		pStart += header.header_len + MODE2_FRAGMENT_CRC8_LEN;


		// 20090313
		if(header.end_flag == 1)
			break;

		pStart += header.payload_len;
	}

	return DEMUX_OK;	
}

int CDemultiplexerKTouch::ParseEmm(BYTE* pData, int nLen, BYTE nEmmType)
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

		// post payload
		if(m_pReciever)
		{
			// 以0x55开始
			CaPacket packet;
			Decrypt(&packet, pStart, header.payload_len+header.header_len+MODE2_FRAGMENT_CRC8_LEN, MUX_SEGMENT_EMM, nEmmType);
		}

		pStart += header.header_len + MODE2_FRAGMENT_CRC8_LEN;


		pStart += header.payload_len;
	}

	return DEMUX_OK;	
}

int CDemultiplexerKTouch::ParseVideo(BYTE* pData, MultiSubFrameHeader* pHeader)
{
	int nRet = DEMUX_OK;
	bool bEncrypt = (m_MF.multiplex_sub_frame.sub_frame_header->encrypt_flag == ENCRYPT_SCRAMBLE);

	VideoSegmentHeader header;
	nRet = ParseVideoSegmentHeader(pData, &header);	

	if(DEMUX_OK != nRet && !bEncrypt)
	{
		//20090313 modify timestamp
		//ForceParse(pData, pHeader->video_seg_len, VIDEO_UNIT, nRet, pHeader->start_time_stamp);
		ForceParse(pData, pHeader->video_seg_len, VIDEO_UNIT, nRet, m_dwCurrVideoTimeStamp);
		return nRet;
	}

	// skip header
	pData += header.header_len + VIDEO_SEGMENT_HEADER_CRC32_LEN;


	// Tianyu CA卡:将整个视频单元（包含多个复用块，以0x55起始）送入解扰，输出亦为整个视频单元。
	if (bEncrypt)
	{
		//CDmxDump::DumpLog(_T("[DMX] Issue video encrypt data = %02x %02x\n"), pData[0], pData[1]);
		CaPacket packet;
		Decrypt(&packet, pData, pHeader->video_seg_len - header.header_len - VIDEO_SEGMENT_HEADER_CRC32_LEN, MUX_SEGMENT_VIDEO);

		if (!packet.encrypt)
			return DEMUX_DESCRAMBLE_FAILED;

		pData = packet.decrypt_data_ptr;
	}

	// NOTE: currently each unit has a whole h264 frame, but this  not desc
	for(int i=0; i<header.video_unit_count; i++)
	{
		//if(header.video_unit_param[i].frame_type == FRAME_I)
		//int n = 0;
		m_nVideoAlgorithm = m_MF.multiplex_sub_frame.sub_frame_header->video_stream_param[i].algorithm_type;

		if(header.video_unit_param[i].play_time_flag == 1)
			m_dwCurrVideoTimeStamp = header.video_unit_param[i].relative_play_time + pHeader->start_time_stamp;
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
				m_VideoBuf.buf_count			= 1;
				m_VideoBuf.frame[0].dwLen		= header.video_unit_param[i].unit_len;
				m_VideoBuf.frame[0].dwTimeStamp = m_dwCurrVideoTimeStamp;
				m_VideoBuf.frame[0].pData		= pData;
				m_VideoBuf.frame[0].pos			= FRAME_WHOLE;
				

				// 20100616
				m_VideoBuf.frame[0].type		= FRAME_I;
				//m_VideoBuf.frame[0].type		= m_KeyFrameChecker.AnalyseData(m_VideoBuf.frame[0].pData, m_VideoBuf.frame[0].dwLen)?FRAME_I:FRAME_P;

				m_VideoBuf.frame[0].nCodecType	= m_nVideoAlgorithm;

				if(m_pReciever)
					m_pReciever->WhenParsedVideoFrame(&m_VideoBuf);
			}
			break;
		}

		pData += header.video_unit_param[i].unit_len;
	}

	return DEMUX_OK;
}

int CDemultiplexerKTouch::ParseAudio(BYTE* pData, MultiSubFrameHeader* pHeader)
{
	bool bEncrypt = (m_MF.multiplex_sub_frame.sub_frame_header->encrypt_flag == ENCRYPT_SCRAMBLE);

	AudioSegmentHeader header;
	int nRet = ParseAudioSegmentHeader(pData, &header);

	if(DEMUX_OK != nRet && !bEncrypt)
	{
		//20090313 modify timestamp
		//ForceParse(pData, pHeader->audio_seg_len, AUDIO_UNIT, nRet, pHeader->start_time_stamp);
		ForceParse(pData, pHeader->audio_seg_len, AUDIO_UNIT, nRet, m_dwCurrAudioTimeStamp);
		return nRet;
	}	

	pData += header.header_len + AUDIO_SEGMENT_HEADER_CRC32_LEN;
	m_dwCurrAudioTimeStamp = pHeader->start_time_stamp;
	m_dwLastAudioTimeStamp = pHeader->start_time_stamp;

	// Tianyu CA卡:将整个音频单元（包含多个复用块，以0x55起始）送入解扰，输出亦为整个单元。
	if (bEncrypt)
	{
		//CDmxDump::DumpLog(_T("[DMX] Issue audio encrypt data = %02x %02x\n"), pData[0], pData[1]);
		CaPacket packet;
		Decrypt(&packet, pData, pHeader->audio_seg_len-header.header_len - AUDIO_SEGMENT_HEADER_CRC32_LEN, MUX_SEGMENT_AUDIO);

		if (!packet.encrypt)
			return DEMUX_DESCRAMBLE_FAILED;

		pData = packet.decrypt_data_ptr;
	}

	m_dwCurrAudioTimeStamp = pHeader->start_time_stamp;

	for(BYTE n=0; n<header.unit_count; n++)
	{	
		// 20090323
		m_dwCurrAudioTimeStamp = pHeader->start_time_stamp + header.audio_unit_param[n].relative_play_time;
		//CDmxDump::DumpLog(_T("Start time = %u, relative_time = %u\n"), pHeader->start_time_stamp, header.audio_unit_param[n].relative_play_time);
		m_nAudioAlgorithm = m_MF.multiplex_sub_frame.sub_frame_header->audio_stream_param[0].algorithm_type;

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

				// 20090808 here maybe has iisue. need skip 9 bytes
				//DRA
				if(AUDIO_DRA == m_MF.multiplex_sub_frame.sub_frame_header->audio_stream_param[0].algorithm_type)
				{
					ParseDra(pData, header.audio_unit_param[n].unit_len);
				}
				else
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