
#include "CCmmbParser.h"
#include "voVideo.h"
#include "voAudio.h"
#include "CBaseTools.h"

CCmmbParser::CCmmbParser(VO_MEM_OPERATOR* pMemOp)
: CBaseParser(pMemOp)
, m_nFrameCurrLen(0)
{
	m_Dmx.SetReciever(this);

	m_pVideoFrameBuf	= (VO_PBYTE)MemAlloc(128*1024);
}

CCmmbParser::~CCmmbParser(void)
{
	if (m_pVideoFrameBuf)
	{
		MemFree(m_pVideoFrameBuf);
		m_pVideoFrameBuf = VO_NULL;
	}
}

VO_U32 CCmmbParser::Open(VO_PARSER_INIT_INFO* pParam)
{
	CBaseParser::Open(pParam);

	return VO_ERR_PARSER_OK;
}

VO_U32 CCmmbParser::Close()
{
	m_Dmx.Close();

	CBaseParser::Close();

	return VO_ERR_PARSER_OK;
}

VO_U32 CCmmbParser::Process(VO_PARSER_INPUT_BUFFER* pBuffer)
{
	m_Dmx.Demux(pBuffer->pBuf, pBuffer->nBufLen);

	return VO_ERR_PARSER_NOT_IMPLEMENT;
}

VO_U32 CCmmbParser::doSetParam(VO_U32 uID, VO_PTR pParam)
{
	return VO_ERR_PARSER_FAIL;
}

VO_U32 CCmmbParser::doGetParam(VO_U32 uID, VO_PTR pParam)
{
	return VO_ERR_PARSER_FAIL;
}



void CCmmbParser::WhenParsedVideoFrame(VideoUnitBuf* pVideo)
{
	for(int n=0; n<pVideo->buf_count; n++)
	{
		if(FRAME_WHOLE == pVideo->frame[n].pos)
		{
			m_nFrameCurrLen = pVideo->frame[n].dwLen;
			MemCopy(m_pVideoFrameBuf, pVideo->frame[n].pData, pVideo->frame[n].dwLen);

			if(!CheckHeadData(VO_VIDEO_CodingH264, m_pVideoFrameBuf, m_nFrameCurrLen))
				return;

			VO_MTV_FRAME_BUFFER buf;
			buf.nCodecType	= VO_VIDEO_CodingH264;
			buf.pData		= m_pVideoFrameBuf;
			buf.nSize		= m_nFrameCurrLen;
			buf.nStartTime	= pVideo->frame[n].dwTimeStamp;
			
			VO_BYTE*	pHeadData	= NULL;
			VO_U32		nHeadSize	= 0;
			VO_BOOL		bVideo		= VO_TRUE;
			VO_BOOL		bKeyFrame	= VO_FALSE;
			bKeyFrame = pVideo->frame[n].type==FRAME_I?VO_TRUE:VO_FALSE;
			buf.nFrameType	= bKeyFrame?VO_VIDEO_FRAME_I:VO_VIDEO_FRAME_B;
			buf.nFrameType	= CBaseTools::IsKeyFrame(VO_VIDEO_CodingH264, buf.pData, buf.nSize)?VO_VIDEO_FRAME_I:VO_VIDEO_FRAME_B;

			VO_U32 ts = ((pVideo->frame[n].dwTimeStamp*1000) / 22500);
			//VOLOGI("parse %s sample-> %02x %02x %02x %02x, ts:%u", "VIDEO", m_pVideoFrameBuf[0], m_pVideoFrameBuf[1], m_pVideoFrameBuf[2], m_pVideoFrameBuf[3], (pVideo->frame[n].dwTimeStamp*1000)/22500);

			IssueParseResult(VO_PARSER_OT_VIDEO, &buf);
		}
		else if(FRAME_BEGIN == pVideo->frame[n].pos)
		{
			MemCopy(m_pVideoFrameBuf, pVideo->frame[n].pData, pVideo->frame[n].dwLen);
			m_nFrameCurrLen = pVideo->frame[n].dwLen;
		}
		else if(FRAME_MID == pVideo->frame[n].pos)
		{
			MemCopy(m_pVideoFrameBuf+m_nFrameCurrLen, pVideo->frame[n].pData, pVideo->frame[n].dwLen);
			m_nFrameCurrLen += pVideo->frame[n].dwLen;
		}
		else if(FRAME_END == pVideo->frame[n].pos)
		{
			MemCopy(m_pVideoFrameBuf+m_nFrameCurrLen, pVideo->frame[n].pData, pVideo->frame[n].dwLen);
			m_nFrameCurrLen += pVideo->frame[n].dwLen;

			if(!CheckHeadData(VO_VIDEO_CodingH264, m_pVideoFrameBuf, m_nFrameCurrLen))
				return;

			VO_MTV_FRAME_BUFFER buf;
			buf.nCodecType	= VO_VIDEO_CodingH264;
			buf.pData		= m_pVideoFrameBuf;
			buf.nSize		= m_nFrameCurrLen;
			buf.nStartTime	= pVideo->frame[n].dwTimeStamp;

			VO_BYTE*	pHeadData	= NULL;
			VO_U32		nHeadSize	= 0;
			VO_BOOL		bVideo		= VO_TRUE;
			VO_BOOL		bKeyFrame	= VO_FALSE;
			bKeyFrame = pVideo->frame[n].type==FRAME_I?VO_TRUE:VO_FALSE;

			buf.nFrameType	= bKeyFrame?VO_VIDEO_FRAME_I:VO_VIDEO_FRAME_B;
			buf.nFrameType	= CBaseTools::IsKeyFrame(VO_VIDEO_CodingH264, buf.pData, buf.nSize)?VO_VIDEO_FRAME_I:VO_VIDEO_FRAME_B;

			//VO_U32 ts = ((pVideo->frame[n].dwTimeStamp*1000) / 22500);
			//VOLOGI("parse %s sample-> %02x %02x %02x %02x, ts:%u", "VIDEO", m_pVideoFrameBuf[0], m_pVideoFrameBuf[1], m_pVideoFrameBuf[2], m_pVideoFrameBuf[3], (pVideo->frame[n].dwTimeStamp*1000)/22500);

			IssueParseResult(VO_PARSER_OT_VIDEO, &buf);
		}		
	}
	
}

VO_BOOL CCmmbParser::CheckHeadData(VO_U32 nCodecType, VO_BYTE* pData, VO_U32 nLen)
{
	if(m_StreamInfo.pVideoExtraData && m_StreamInfo.pAudioExtraData)
		return VO_TRUE;

	VO_BYTE*	pHeadData	= NULL;
	VO_U32		nHeadSize	= 0;
	VO_BOOL		bVideo		= VO_FALSE;
	VO_BOOL		bKeyFrame	= VO_FALSE;

	bKeyFrame = CBaseTools::GetHeadData(nCodecType, pData, nLen, bVideo, &pHeadData, nHeadSize);

	if (bKeyFrame && pHeadData && nHeadSize>0 && bVideo)
	{
		m_StreamInfo.nVideoCodecType = nCodecType;
		m_StreamInfo.pVideoExtraData = pHeadData;
		m_StreamInfo.nVideoExtraSize = nHeadSize;

	/*
	const int sample_rates[] =
	{
	96000, 88200, 64000, 48000, 
	44100, 32000,24000, 22050, 
	16000, 12000, 11025, 8000,
	0, 0, 0, 0
	};

	*/
		//AAC profile :ISO_IEC_14496-3-2005.pdf
		// here has issue
		VO_BYTE* head = new VO_BYTE[2];
		VO_BYTE channel			= 2;
		VO_BYTE sample_rate_idx	= 6;
		VO_BYTE profile			= 2; //AAC LC
		head[0] = (VO_BYTE) ((profile << 3) | (sample_rate_idx >> 1));
		head[1] = (VO_BYTE) ((sample_rate_idx << 7) | (channel << 3));

		//
		head[0] = 0x13;
		head[1] = 0x10;

		m_StreamInfo.nAudioCodecType = VO_AUDIO_CodingAAC;
		m_StreamInfo.pAudioExtraData = head;
		m_StreamInfo.nAudioExtraSize = 2;

		IssueParseResult(VO_PARSER_OT_STREAMINFO, &m_StreamInfo);
		return VO_TRUE;
	}

	return VO_FALSE;
}

void CCmmbParser::WhenParsedAudioFrame(AudioUnitBuf* pAudio)
{
	for (VO_U16 n=0; n<pAudio->buf_count; n++)
	{
		VO_MTV_FRAME_BUFFER buf;

		if(buf.nCodecType	= pAudio->frame[n].nCodecType == AUDIO_DRA)
			buf.nCodecType = VO_AUDIO_CodingDRA;
		else if(pAudio->frame[n].nCodecType == AUDIO_HE_AAC || pAudio->frame[n].nCodecType == AUDIO_AAC)
			buf.nCodecType = VO_AUDIO_CodingAAC;
		else
			buf.nCodecType	= pAudio->frame[n].nCodecType = VO_AUDIO_CodingUnused;

		buf.pData		= pAudio->frame[n].pData;
		buf.nSize		= pAudio->frame[n].dwLen;
		buf.nStartTime	= pAudio->frame[n].dwTimeStamp;
		buf.nFrameType	= 0;

		//VO_U32 ts = ((pAudio->frame[n].dwTimeStamp*1000) / 22500);
		//VOLOGI("parse %s sample-> %02x %02x %02x %02x, ts:%u", "VIDEO", m_pVideoFrameBuf[0], m_pVideoFrameBuf[1], m_pVideoFrameBuf[2], m_pVideoFrameBuf[3], (pVideo->frame[n].dwTimeStamp*1000)/22500);
		
		IssueParseResult(VO_PARSER_OT_AUDIO, &buf);
	}
}

void CCmmbParser::WhenParsedTS0(TS0Info* pTS0)
{
	VOTS0INFO info;

	//Encription & Authotization Description Table
	info.ca_info.ca_update_sn		= pTS0->ea_desc.ea_update_sn;
	info.ca_info.data_ptr			= pTS0->ea_desc.table_data_ptr;
	info.ca_info.data_len			= pTS0->ea_desc.table_data_len;

	info.ca_info.ca_service_count	= pTS0->ea_desc.segment_count;
	info.ca_info.ca_system_count	= pTS0->ea_desc.segment_count;
	for(BYTE n=0; n<pTS0->ea_desc.segment_count; n++)
	{
		info.ca_info.ca_service_id[n]	= pTS0->ea_desc.ea_data_desc[n].service_id;
		info.ca_info.ca_system_id[n]	= pTS0->ea_desc.ea_data_desc[n].ca_id;
	}

	//NIT
	info.nit_info.net_id			= pTS0->nit.net_id;
	info.nit_info.nit_update_sn		= pTS0->nit.nit_update_sn;

	//CMCT
	info.continue_service_count = pTS0->cmct.mf_num;
	for(BYTE n=0; n<pTS0->cmct.mf_num; n++)
	{
		info.continue_service[n].mf_id				= pTS0->cmct.mf_info[n].mf_id;
		info.continue_service[n].service_id			= pTS0->cmct.mf_info[n].msf_info[0].service_id;
		info.continue_service[n].freq				= pTS0->cmct.freq;
		info.continue_service[n].time_slot_count	= pTS0->cmct.mf_info[n].ts_num;
		info.continue_service[n].time_slot_start	= pTS0->cmct.mf_info[n].ts_sn[0];
		//info.continue_service[n].demod			= ((pTS0->cmct.mf_info[n].mode<<6) + (pTS0->cmct.mf_info[n].rs<<4) + (pTS0->cmct.mf_info[n].byte_interlace<<2) + pTS0->cmct.mf_info[n].ldpc);
		info.continue_service[n].demod				= pTS0->cmct.mf_info[n].mode;
		info.continue_service[n].rs					= pTS0->cmct.mf_info[n].rs;
		info.continue_service[n].byte_interlace		= pTS0->cmct.mf_info[n].byte_interlace;
		info.continue_service[n].ldpc				= pTS0->cmct.mf_info[n].ldpc;
	}

	//esg
	info.esg_desc.esg_service_count = pTS0->esg_desc.esg_service_count;
	for(BYTE n=0; n<pTS0->esg_desc.esg_service_count; n++)
	{
		info.esg_desc.esg_service_id[n] = pTS0->esg_desc.esg_service_desc[n].esg_service_id;
	}

	//EB
	memset(&info.eb, 0, sizeof(VOEBINFO));

	if(pTS0->emegency_broadcast.broadcast_data_len > 0)
	{
		info.eb.broadcast_sn		= pTS0->emegency_broadcast.broadcast_sn;

		// the 3 items below determine exclusive EB 
		info.eb.eb_data_segment.net_id			= pTS0->emegency_broadcast.eb_data_segment.net_id;
		info.eb.eb_data_segment.net_level		= pTS0->emegency_broadcast.eb_data_segment.net_level;
		info.eb.eb_data_segment.msg_id			= pTS0->emegency_broadcast.eb_data_segment.msg_id;

		info.eb.eb_data_segment.eb_data_len		= pTS0->emegency_broadcast.eb_data_segment.eb_data_len;
		info.eb.eb_data_segment.trigger_flag	= pTS0->emegency_broadcast.eb_data_segment.trigger_flag;

		if(pTS0->emegency_broadcast.eb_data_segment.trigger_flag == 0)
		{
			info.eb.eb_data_segment.eb_msg0.trigger_type	= pTS0->emegency_broadcast.eb_data_segment.eb_msg0.trigger_type;
			info.eb.eb_data_segment.eb_msg0.eb_charset		= pTS0->emegency_broadcast.eb_data_segment.eb_msg0.eb_charset;
			info.eb.eb_data_segment.eb_msg0.eb_start_time	= pTS0->emegency_broadcast.eb_data_segment.eb_msg0.eb_start_time;
			info.eb.eb_data_segment.eb_msg0.eb_duration		= pTS0->emegency_broadcast.eb_data_segment.eb_msg0.eb_duration;
			info.eb.eb_data_segment.eb_msg0.text_data_count	= pTS0->emegency_broadcast.eb_data_segment.eb_msg0.text_data_count;

			for (BYTE n=0; n<pTS0->emegency_broadcast.eb_data_segment.eb_msg0.text_data_count; n++)
			{
				info.eb.eb_data_segment.eb_msg0.text_data[n].language_type	= pTS0->emegency_broadcast.eb_data_segment.eb_msg0.text_data[n].language_type;
				info.eb.eb_data_segment.eb_msg0.text_data[n].text_data_len	= pTS0->emegency_broadcast.eb_data_segment.eb_msg0.text_data[n].text_data_len;
				info.eb.eb_data_segment.eb_msg0.text_data[n].text_data_ptr	= pTS0->emegency_broadcast.eb_data_segment.eb_msg0.text_data[n].text_data_ptr;

				info.eb.eb_data_segment.eb_msg0.text_data[n].organization_name_len	= pTS0->emegency_broadcast.eb_data_segment.eb_msg0.text_data[n].organization_name_len;
				info.eb.eb_data_segment.eb_msg0.text_data[n].organization_name_ptr	= pTS0->emegency_broadcast.eb_data_segment.eb_msg0.text_data[n].organization_name_ptr;

				info.eb.eb_data_segment.eb_msg0.text_data[n].ref_service_id	= pTS0->emegency_broadcast.eb_data_segment.eb_msg0.text_data[n].ref_service_id;
				info.eb.eb_data_segment.eb_msg0.text_data[n].aux_data_idx	= pTS0->emegency_broadcast.eb_data_segment.eb_msg0.text_data[n].aux_data_idx;
			}

			// aux data`
			info.eb.eb_data_segment.eb_msg0.aux_data_count	= pTS0->emegency_broadcast.eb_data_segment.eb_msg0.aux_data_count;

			for (BYTE n=0; n<pTS0->emegency_broadcast.eb_data_segment.eb_msg0.aux_data_count; n++)
			{
				info.eb.eb_data_segment.eb_msg0.aux_data[n].aux_data_type	= pTS0->emegency_broadcast.eb_data_segment.eb_msg0.aux_data[n].aux_data_type;
				info.eb.eb_data_segment.eb_msg0.aux_data[n].aux_data_len	= pTS0->emegency_broadcast.eb_data_segment.eb_msg0.aux_data[n].aux_data_len;
				info.eb.eb_data_segment.eb_msg0.aux_data[n].aux_data_ptr	= pTS0->emegency_broadcast.eb_data_segment.eb_msg0.aux_data[n].aux_data_ptr;
			}
		}
		else // trigger flag == 1
		{

			info.eb.eb_data_segment.eb_msg1.trigger_type		= pTS0->emegency_broadcast.eb_data_segment.eb_msg1.trigger_type;			// 7 bits

			info.eb.eb_data_segment.eb_msg1.trigger_msg_level	= pTS0->emegency_broadcast.eb_data_segment.eb_msg1.trigger_msg_level;
			info.eb.eb_data_segment.eb_msg1.reserved1			= pTS0->emegency_broadcast.eb_data_segment.eb_msg1.reserved1;

			info.eb.eb_data_segment.eb_msg1.trigger_net_level	= pTS0->emegency_broadcast.eb_data_segment.eb_msg1.trigger_net_level;
			info.eb.eb_data_segment.eb_msg1.trigger_network_sn	= pTS0->emegency_broadcast.eb_data_segment.eb_msg1.trigger_network_sn;

			info.eb.eb_data_segment.eb_msg1.trigger_freq_point	= pTS0->emegency_broadcast.eb_data_segment.eb_msg1.trigger_freq_point;
			info.eb.eb_data_segment.eb_msg1.trigger_center_freq	= pTS0->emegency_broadcast.eb_data_segment.eb_msg1.trigger_center_freq;

			info.eb.eb_data_segment.eb_msg1.trigger_band_width	= pTS0->emegency_broadcast.eb_data_segment.eb_msg1.trigger_band_width;
			info.eb.eb_data_segment.eb_msg1.reserved2			= pTS0->emegency_broadcast.eb_data_segment.eb_msg1.reserved2;
		}

	}

	//OnTS0Info(&info);
	IssueParseResult(VO_PARSER_OT_CMMB_TS0_INFO, &info);
}

void CCmmbParser::WhenParsedEsgServiceInfo(EsgServiceInfo* pInfo)
{
	VOSERVICEINFO info;

	info.service_class		= (VOSERVICECLASS)pInfo->service_class;
	info.service_free		= (VO_BOOL)pInfo->for_free;
	info.service_id			= pInfo->service_id;

	info.service_name		= pInfo->service_name_str;
	info.service_param_id	= pInfo->service_param_id;

	IssueParseResult(VO_PARSER_OT_CMMB_SERVICE_INFO, &info);
}

void CCmmbParser::WhenParsedProgramGuide(ProgramGuideInfo* pInfo)
{
	if(pInfo->record_count <= 0)
		return;

	VOPROGRAMGUIDE pg;
	pg.count = pInfo->record_count;

	for(BYTE n=0; n<pInfo->record_count; n++)
	{
		int year	= (pInfo->record_info[n].date-15078.2)/365.25;
		int month	= (int)((pInfo->record_info[n].date-14956.1-(int)(year*365.25))/30.6001);
		int day		= pInfo->record_info[n].date-14956-(int)(year*365.25)-(int)(month*30.6001);

		int k = 0;
		if(month==14 || month==15)
			k = 1;
		year = year + k + 1900;
		month = month - 1 - k*12;

		pg.guide[n].start_time.wYear	= year;
		pg.guide[n].start_time.wMonth	= month;
		pg.guide[n].start_time.wDay		= day;

		// 20090729
		// 		BYTE tmp						= (pInfo->record_info[n].time >> 16) & 0xff;
		// 		pg.guide[n].start_time.wHour	= (tmp>>4)*10 + tmp&0xf;
		// 		tmp								= (pInfo->record_info[n].time >> 8) & 0xff;
		// 		pg.guide[n].start_time.wMinute	= (tmp>>4)*10 + tmp&0xf;
		// 		tmp								= pInfo->record_info[n].time & 0xff;
		// 		pg.guide[n].start_time.wSecond	= (tmp>>4)*10 + tmp&0xf;

		pg.guide[n].start_time.wHour	= pInfo->record_info[n].hour;
		pg.guide[n].start_time.wMinute	= pInfo->record_info[n].minute;
		pg.guide[n].start_time.wSecond	= pInfo->record_info[n].second;


		pg.guide[n].duration			= pInfo->record_info[n].duration;
		pg.guide[n].theme_len			= pInfo->record_info[n].theme_len;

		memset(pg.guide[n].theme, 0, MAX_PROGRAM_THEME_LEN);

#ifdef _WIN32
		pg.guide[n].theme_len = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pInfo->record_info[n].theme_ptr, pInfo->record_info[n].theme_len, (LPWSTR)pg.guide[n].theme, MAX_PROGRAM_THEME_LEN);
#else
		pg.guide[n].theme_len = pInfo->record_info[n].theme_len;
		memcpy(pg.guide[n].theme, pInfo->record_info[n].theme_ptr, pInfo->record_info[n].theme_len);
#endif
	}

	IssueParseResult(VO_PARSER_OT_CMMB_PROGRAM_GUIDE, &pg);
}

void CCmmbParser::WhenParsedMultiFrameHeader(MultiplexFrameHeader* pHeader)
{
	VOMFHEADERINFO info;
	info.mf_id		= pHeader->mf_id;
	info.eb_flag	= pHeader->urgency_broadcast_flag;

	m_nStreamID		= pHeader->mf_id;

	IssueParseResult(VO_PARSER_OT_CMMB_MF_HEADER_INFO, &info);
}

void CCmmbParser::WhenParsedMultiSubFrameHeader(MultiSubFrameHeader* pHeader)
{
	VOMSFHEADERINFO info;

	info.encrypt_flag	= pHeader->encrypt_flag;

	IssueParseResult(VO_PARSER_OT_CMMB_MSF_HEADER_INFO, &info);
}


void CCmmbParser::WhenParsedFail(int nErrorType, void* pParam)
{
	// NOTE: need convert error type here
	nErrorType = VO_ERR_PARSER_FAIL;
	IssueParseResult(VO_PARSER_OT_CMMB_FAILED, (void*)nErrorType, pParam);


	return;


#ifdef DMX_DUMP
	switch(nErrorType)
	{
	case DEMUX_MF_START_CODE_ERROR:
		CDmxDump::DumpLog(_T("Demux Error: %s\n"), _T("DEMUX_MF_START_CODE_ERROR"));
		break;
	case DEMUX_MF_HEADER_FAILED:
		CDmxDump::DumpLog(_T("Demux Error: %s\n"), _T("DEMUX_MF_HEADER_FAILED"));
		break;
	case DEMUX_MSF_HEADER_FAILED:
		CDmxDump::DumpLog(_T("Demux Error: %s\n"), _T("DEMUX_MSF_HEADER_FAILED"));
		break;
	case DEMUX_TS0_FAILED:
		CDmxDump::DumpLog(_T("Demux Error: %s\n"), _T("DEMUX_TS0_FAILED"));
		break;
	case DEMUX_FRAGMENT_STRAT_CODE_ERROR:
#ifndef _STRICT_DUMP_
		return;
#endif
		CDmxDump::DumpLog(_T("Demux Error: %s\n"), _T("DEMUX_FRAGMENT_STRAT_CODE_ERROR"));
		break;
	case DEMUX_VIDEO_FRAGMENT_ERROR:
#ifndef _STRICT_DUMP_
		return;
#endif
		CDmxDump::DumpLog(_T("Demux Error: %s\n"), _T("DEMUX_VIDEO_FRAGMENT_ERROR"));
		break;
	case DEMUX_UNSUPORT_H264_NAL_TYPE:
		CDmxDump::DumpLog(_T("Demux Error: %s\n"), _T("DEMUX_UNSUPORT_H264_NAL_TYPE"));
		break;
	case DEMUX_NIT_CRC_ERROR:
		CDmxDump::DumpLog(_T("Demux Error: %s\n"), _T("DEMUX_NIT_CRC_ERROR"));
		break;
	case DEMUX_CMCT_CRC_ERROR:
		CDmxDump::DumpLog(_T("Demux Error: %s\n"), _T("DEMUX_CMCT_CRC_ERROR"));
		break;
	case DEMUX_EBDT_CRC_ERROR:
		CDmxDump::DumpLog(_T("Demux Error: %s\n"), _T("DEMUX_EBDT_CRC_ERROR"));
		break;
	case DEMUX_MF_HEADER_CRC_ERROR:
		CDmxDump::DumpLog(_T("Demux Error: %s\n"), _T("DEMUX_MF_HEADER_CRC_ERROR"));
		break;
	case DEMUX_MSF_HEADER_CRC_ERROR:
		CDmxDump::DumpLog(_T("Demux Error: %s\n"), _T("DEMUX_MSF_HEADER_CRC_ERROR"));
		break;
	case DEMUX_FRAGMENT_HEADER_CRC_ERROR:
#ifndef _STRICT_DUMP_
		return;
#endif
		CDmxDump::DumpLog(_T("Demux Error: %s\n"), _T("DEMUX_FRAGMENT_HEADER_CRC_ERROR"));
		break;
	case DEMUX_VIDEO_SEG_HEADER_CRC_ERROR:
		CDmxDump::DumpLog(_T("Demux Error: %s\n"), _T("DEMUX_VIDEO_SEG_HEADER_CRC_ERROR"));
		break;
	case DEMUX_AUDIO_SEG_HEADER_CRC_ERROR:
		CDmxDump::DumpLog(_T("Demux Error: %s\n"), _T("DEMUX_AUDIO_SEG_HEADER_CRC_ERROR"));
		break;
	case DEMUX_DATA_SEG_HEADER_CRC_ERROR:
		CDmxDump::DumpLog(_T("Demux Error: %s\n"), _T("DEMUX_DATA_SEG_HEADERCRC_ERROR"));
		break;
	case DEMUX_PROGRAME_GUIDE_CRC_ERROR:
		CDmxDump::DumpLog(_T("Demux Error: %s\n"), _T("DEMUX_PROGRAME_GUIDE_CRC_ERROR"));
		break;
	case DEMUX_ESG_SEG_HEADERCRC_ERROR:
		CDmxDump::DumpLog(_T("Demux Error: %s\n"), _T("DEMUX_ESG_SEG_HEADERCRC_ERROR"));
		break;
	case DEMUX_EA_DESC_CRC_ERROR:
		CDmxDump::DumpLog(_T("Demux Error: %s\n"), _T("DEMUX_EA_DESC_CRC_ERROR"));
		break;
	case DEMUX_EMEGENCY_BROADCAST_CRC_ERROR:
		CDmxDump::DumpLog(_T("%s\r\n"), _T("DEMUX_EMEGENCY_BROADCAST_CRC_ERROR"));
		break;
	case DEMUX_CSCT_CRC_ERROR:
		CDmxDump::DumpLog(_T("%s\r\n"), _T("DEMUX_CSCT_CRC_ERROR"));
		break;
	case DEMUX_MSF_LENTH_ERROR:
		CDmxDump::DumpLog(_T("%s\r\n"), _T("DEMUX_MSF_LENTH_ERROR"));
		break;
	case DEMUX_MF_LENTH_ERROR:
		CDmxDump::DumpLog(_T("%s\r\n"), _T("DEMUX_MF_LENTH_ERROR"));
		break;
	default:
		CDmxDump::DumpLog(_T("Demux Error:%d %s\n"), _T("DEMUX_UNKNOW_ERROR"), nErrorType);
		break;
	}
#endif
}

void CCmmbParser::WhenParseWholeMFFinished(MultiplexFrame* pMF)
{

}


void CCmmbParser::WhenParsedEsgServiceAuxInfo(EsgServiceAuxInfo* pInfo)
{
	VOSERVICEAUDIXINFO info;

	info.media_type.content_class	= pInfo->media_type.content_class;
	info.media_type.media_data		= pInfo->media_type.media_data;
	info.media_type.media_uri		= pInfo->media_type.media_uri;
	info.media_type.usage			= pInfo->media_type.usage;

	info.lang						= pInfo->service_lang;
	info.service_description		= pInfo->service_desc;
	info.service_id				= pInfo->service_id;
	info.service_provider			= pInfo->service_provider;
	info.lang_special				= pInfo->service_lang_special;

	IssueParseResult(VO_PARSER_OT_CMMB_SERVICE_AUX_INFO, &info);
}

void CCmmbParser::WhenParsedEsgScheduleInfo(EsgScheduleInfo* pInfo)
{
	VOSCHEDULEINFO info;

	info.content_id		= pInfo->content_id;
	info.date			= pInfo->date;
	info.time			= pInfo->time;
	info.free			= pInfo->for_free?VO_TRUE:VO_FALSE;
	info.live			= pInfo->live?VO_TRUE:VO_FALSE;
	info.repeat			= pInfo->repeat?VO_TRUE:VO_FALSE;
	info.schedule_id	= pInfo->schedule_id;
	info.service_id		= pInfo->service_id;
	info.title			= pInfo->title;

	IssueParseResult(VO_PARSER_OT_CMMB_SCHEDULE_INFO, &info);
}

void CCmmbParser::WhenParsedEsgServiceParamInfo(EsgServiceParamInfo* pInfo)
{
	VOSERVICEPARAMINFO info;

	info.service_param_id = pInfo->service_param_id;

	IssueParseResult(VO_PARSER_OT_CMMB_SERVICE_PARAM_INFO, &info);
}

void CCmmbParser::WhenParsedEsgContentInfo(EsgContentInfo* pInfo)
{
	VOCONTENTINFO info;

	info.title			= pInfo->title;
	info.content_id		= pInfo->content_id;
	info.content_class	= pInfo->content_class;

	IssueParseResult(VO_PARSER_OT_CMMB_CONTENT_INFO, &info);
}

void CCmmbParser::WhenParsedEsgFinished()
{
	IssueParseResult(VO_PARSER_OT_CMMB_ESG_PARSE_FINISHED, NULL);
}

void CCmmbParser::WhenParsedEncryptData(CaPacket* pPacket)
{
	VODESCRAMBLING desc;

	// in
	desc.type				= pPacket->type;
	desc.mf_id				= m_nStreamID;
	desc.encrypt_data_ptr	= pPacket->encrypt_data_ptr;
	desc.encrypt_data_len	= pPacket->encrypt_data_len;
	desc.ecm_emm_type		= pPacket->ecm_emm_type;

	// out
	desc.decrypt_data_len	= 0;
	desc.decrypt_data_ptr	= NULL;
	desc.encrypt			= VO_FALSE;

	// to descramble
	IssueParseResult(VO_PARSER_OT_CMMB_DESCRAMBLING, &desc);

	// if descramble successed
	if(desc.encrypt)
	{
		pPacket->decrypt_data_ptr	= desc.decrypt_data_ptr;
		pPacket->decrypt_data_len	= desc.decrypt_data_len;
		pPacket->encrypt			= true;
	}
}


void CCmmbParser::WhenParsedXpe(XpeBuf* buf)
{
	VODATABUFFER buffer;
	buffer.data	= buf->data;
	buffer.data_len= buf->data_len;

	if (buf->mode == DATA_SERVICE_STREAM_MODE)
	{
		if(buf->type == TYPE_XPE)
			buffer.data_type = XPE_STREAM;
		else
			buffer.data_type = XPE_FEC_STREAM;
	}
	else
	{
		if(buf->type == TYPE_XPE_FEC)
			buffer.data_type = XPE_FEC_FILE;
		else
			buffer.data_type = XPE_FILE;
	}

	IssueParseResult(VO_PARSER_OT_CMMB_PACKET_DATA, &buffer);
}


