#include "CMpegParser.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CMpegDataParser::CMpegDataParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp)
	: CvoFileDataParser(pFileChunk, pMemOp)
	, m_btPackLen(0xFF)
	, m_StreamIfo(NULL)
	, m_pEcmData(NULL)
	, m_DrmInfoReady(VO_FALSE)
	, m_ParserMode(VO_MPEG_PARSER_MODE_PLAYBACK)
{
}

CMpegDataParser::~CMpegDataParser()
{
	DelStreamList();
	SAFE_DELETE(m_pEcmData);
}

VO_VOID CMpegDataParser::Init(VO_PTR pParam)
{
}

VO_VOID CMpegDataParser::Reset()
{

}

VO_U32 CMpegDataParser::MoveTo( VO_S64 llTimeStamp,VO_U32 dwDuration,VO_U64 ullFileSize)
{
	SetStartFilePos(dwDuration > 0 ? (llTimeStamp*ullFileSize) / dwDuration : 0);
	return VO_ERR_SOURCE_OK;
}

VO_BOOL CMpegDataParser::StepB()
{
	use_big_endian_read

	VO_U8 btPESHeader[3];
	VO_U64 llPESFilePos = 0;
	while(1)
	{
		ptr_read_pointer(btPESHeader, 3);
		if(!MemCompare(btPESHeader, (VO_PTR)&MPEG_PES_HEADER, 3))
		{
			llPESFilePos = m_pFileChunk->FGetFilePos() - 3;

			VO_U8 btValue;
			ptr_read_byte(btValue);	// stream id
			
	//		VOLOGI("btValue == %x",btValue);
			if(PACK_START_CODE == btValue)
			{
				if(0xFF == m_btPackLen)
				{
					ptr_read_byte(btValue);
					// first 2 bit 01:		mpeg2
					// first 4 bit 0010:	mpeg1
					if(0x40 == (btValue & 0xc0))
						m_btPackLen = 10;	// mpeg2
					else if(0x20 == (btValue & 0xf0))
						m_btPackLen = 8;	// mpeg1
					else
						return VO_FALSE;

					ptr_skip(m_btPackLen - 1);
				}
				else
					ptr_skip(m_btPackLen);
			}
			// audio stream, 0x110XXXXX
			// video stream, 0x1110XXXX
			else if(VO_MPEG_STREAM_TYPE_UNKNOW != CheckMediaTypeByStreamID(btValue))
				return ParsePES(btValue);
			else if(SYSTEM_HEADER_START_CODE == btValue /*|| PRIVATE_STREAM_1 == btValue*/ || PADDING_STREAM == btValue || PRIVATE_STREAM_2 == btValue)
			{///<PRIVATE_STREAM_1 maybe contain ac3(SubstreamID==0x80),lpcm(SubstreamID==0xA0),dts(SubstreamID==0x88)
				VO_U16 wValue;
				ptr_read_word(wValue);	// packet len
				ptr_skip(wValue);
			}
			else if (PROGRAM_STREAM_MAP == btValue)
			{
				ParserPSMTable();
			}
			else if (ECM_STREAM == btValue)
			{
				if (VO_FALSE == m_DrmInfoReady)///<just parser ecm once
				{
					VO_U16 wPacketLen = 0;
					ptr_read_word(wPacketLen);	// packet length
					VO_PBYTE pEcmData = m_pFileChunk->FGetFileContentPtr(wPacketLen);
					if (!pEcmData)
					{
						m_pEcmData = new VO_BYTE[wPacketLen];
						m_pFileChunk->FRead(m_pEcmData,wPacketLen);
						pEcmData = m_pEcmData;
					}
					
					MpegDataDrmInfo DrmInfo;
					DrmInfo.pDrmData = pEcmData;
					DrmInfo.btDrmDataSize = wPacketLen;
					VO_U32 ParsedDataType = ParsedDataType_DrmInfo;
					m_fODCallback(m_pODUser, &ParsedDataType, &DrmInfo);
					m_DrmInfoReady = VO_TRUE;
				}
			}
 			else
 			{
			//	VOLOGI("Other Stream ID!!=%d",btValue);
			}

			return VO_TRUE;
		}
		else
		{
			m_pFileChunk->FBack(2);
			continue;
		}
	}
}

VO_BOOL CMpegDataParser::ParsePES(VO_U8 btStreamID)
{
	use_big_endian_read
	VO_U64 dwPESPos = m_pFileChunk->FGetFilePos() - 4;

	VO_BOOL beEncrpt = VO_FALSE;
	VO_U16 wPacketLen = 0;
	ptr_read_word(wPacketLen);	// packet length

	VO_MPEG_STREAM_TYPE StrType = CheckMediaTypeByStreamID(btStreamID);
	if ((StrType == VO_MPEG_STREAM_TYPE_AUDIO && !(m_ParserMode & VO_MPEG_PARSER_MODE_AUDIO))
		||(StrType == VO_MPEG_STREAM_TYPE_VIDEO && !(m_ParserMode & VO_MPEG_PARSER_MODE_VIDEO)))
	{
		ptr_skip(wPacketLen);
		return VO_TRUE;
	}
	// skip 0xff
	VO_U8 btValue = 0xff;
	while(0xff == btValue)
	{
		ptr_read_byte(btValue);
		wPacketLen--;
	}

	if(0x40 == (btValue & 0xc0))	// 0x01XXXXXX
	{
		ptr_skip(1);
		ptr_read_byte(btValue);
		wPacketLen -= 2;
	}

	if(!wPacketLen)
		return VO_TRUE;

	VO_S64 dwTimeStamp = 0;
	if(0x20 == (btValue & 0xf0))	// 0x0010XXXX
	{
		// PTS
		if(!ParseTimeStamp(btValue, &dwTimeStamp))
			return VO_FALSE;

		wPacketLen -= 4;

		return ParseFrame(btStreamID, wPacketLen, dwTimeStamp,dwPESPos);
	}
	else if(0x30 == (btValue & 0xf0))	// 0x0011XXXX
	{
		// PTS
		if(!ParseTimeStamp(btValue, &dwTimeStamp))
			return VO_FALSE;

		wPacketLen -= 4;

		ptr_skip(5);
		wPacketLen -= 5;
		return ParseFrame(btStreamID, wPacketLen, dwTimeStamp,dwPESPos);
	}
	else if(0x80 == (btValue & 0xc0))	// MPEG2 PES
	{
		if((btValue & 0x30) == 0x20)
		{
		//	ptr_read_byte(btValue);
		//	wPacketLen--;
			beEncrpt = VO_TRUE;
		}

		VO_U8 btFlags;
		ptr_read_byte(btFlags);
		wPacketLen--;

		VO_U8 btHeaderLen;
		ptr_read_byte(btHeaderLen);
		wPacketLen--;
	//	VOLOGI("btFlags=%d",btFlags);
		if(0x80 == (btFlags & 0xc0))
		{
			if(btHeaderLen < 5)
				return VO_FALSE;

			//PTS
			ptr_read_byte(btValue);
			if(!ParseTimeStamp(btValue, &dwTimeStamp))
				return VO_FALSE;

			wPacketLen -= 5;
			btHeaderLen -= 5;
			if(btHeaderLen > 0)
			{
				ptr_skip(btHeaderLen);
				wPacketLen -= btHeaderLen;
			}

			return ParseFrame(btStreamID, wPacketLen, dwTimeStamp,dwPESPos,beEncrpt);
		}
		else if(0xc0 == (btFlags & 0xc0))
		{
			if(btHeaderLen < 10)
				return VO_FALSE;

			// PTS
			ptr_read_byte(btValue);
			if(!ParseTimeStamp(btValue, &dwTimeStamp))
				return VO_FALSE;
				
			wPacketLen -= 5;

			//Skip DTS
			ptr_skip(5);
			wPacketLen -= 5;
			btHeaderLen -= 10;
			if(btHeaderLen > 0)
			{
				ptr_skip(btHeaderLen);
				wPacketLen -= btHeaderLen;
			}

			return ParseFrame(btStreamID, wPacketLen, dwTimeStamp,dwPESPos,beEncrpt);
		}
		else
		{
			if(btHeaderLen > 0)
			{
				ptr_skip(btHeaderLen);
				wPacketLen -= btHeaderLen;
			}

			return ParseFrame(btStreamID, wPacketLen, -1,dwPESPos,beEncrpt);
		}
	}
	else if(0x0f == btValue)
		return ParseFrame(btStreamID, wPacketLen, -1,dwPESPos);

	return VO_TRUE;
}

VO_BOOL CMpegDataParser::ParseTimeStamp(VO_U8 btValue, VO_S64* pdwTimeStamp)
{
	use_big_endian_read

	VO_S64 nTimeStamp = (VO_U32(btValue) & 0x0E) << 29;

	VO_U16 wValue;
	ptr_read_word(wValue);
	nTimeStamp |= ((VO_U32(wValue) >> 1) << 15);

	ptr_read_word(wValue);
	nTimeStamp |= (wValue >> 1);

	nTimeStamp /= 90;

	*pdwTimeStamp = (nTimeStamp < 0) ? 0 : nTimeStamp;

	return VO_TRUE;
}

VO_BOOL CMpegDataParser::ParseFrame(VO_U8 btStreamID, VO_U16 wPacketLen, VO_S64 dwTimeStamp, VO_U64 dwPESPos, VO_BOOL beEncrpt)
{
	if((m_btBlockStream == btStreamID) && (!m_fOBCallback || m_fOBCallback(m_pOBUser, m_pOBUserData, &dwTimeStamp)))
	{
		ptr_skip(wPacketLen);
		return VO_TRUE;
	}

	MpegDataParserFrame frame;
	frame.btStreamID = btStreamID;
	frame.wPacketLen = wPacketLen;
	frame.dwTimeStamp = dwTimeStamp;
	frame.pFileChunk = m_pFileChunk;
	frame.beEncrpt = beEncrpt;
	frame.dwPESPos = dwPESPos;
	VO_U8 btRes = m_fODCallback(m_pODUser, m_pODUserData, &frame);

	if(!CBRT_IS_CONTINUABLE(btRes))
		return VO_FALSE;

	if(CBRT_IS_NEEDSKIP(btRes))
		ptr_skip(wPacketLen);

	return VO_TRUE;
}

VO_VOID CMpegDataParser::InitStreamItem(StreamInfo* pInfo)
{
	if (pInfo)
	{
		pInfo->stream_id = 0;
		pInfo->stream_type = 0;
		pInfo->next = NULL;
	}
}

VO_VOID CMpegDataParser::DelStreamList()
{
	StreamInfo* tmpList = m_StreamIfo;
	while (tmpList)
	{
		m_StreamIfo = m_StreamIfo->next;
		delete tmpList;
		tmpList = m_StreamIfo;
	}
}
VO_BOOL CMpegDataParser::ParserPSMTable()
{
	if (m_StreamIfo)
	{
		return VO_TRUE;
	}
	use_big_endian_read
	
	VO_U16 wTableLen = 0;
	ptr_read_word(wTableLen);
	
	if (wTableLen < 10)
	{
		return VO_FALSE;
	}
	VO_U8 tmp = 0;
	ptr_read_byte(tmp);
	if (!(tmp >> 7))
	{
		return VO_FALSE;
	}
	ptr_skip(1);

	VO_U16 wStreamInfoLen = 0;
	ptr_read_word(wStreamInfoLen);
	ptr_skip(wStreamInfoLen);

	VO_U16 wStreamMapLen = 0;
	ptr_read_word(wStreamMapLen);

	VO_S32 wLeftByte = wStreamMapLen;
	StreamInfo* ListTail = m_StreamIfo;
	VO_U32 wDescriptorLen = 0;
	while (wLeftByte > 0)
	{
		StreamInfo* info = new StreamInfo;
		InitStreamItem(info);
		ptr_read_byte(info->stream_type);
		ptr_read_byte(info->stream_id);
		ptr_read_word(wDescriptorLen);
		ptr_skip(wDescriptorLen);
		wLeftByte -= (4 + wDescriptorLen);
		if (!ListTail)
		{
			m_StreamIfo = ListTail = info;
		}
		else
		{
			ListTail->next = info;
			ListTail = info;
		}
		
	}
	return VO_TRUE;

}
VO_VOID CMpegDataParser::InitParser(CMpegDataParser &Parser)
{
	m_btPackLen = Parser.m_btPackLen;
//	m_StreamIfo = Parser.m_StreamIfo;///<the m_StreamInfo should not be copyed to new parser,it just for Init phase
	m_DrmInfoReady = Parser.m_DrmInfoReady;
}

VO_MPEG_STREAM_TYPE CMpegDataParser::CheckMediaTypeByStreamID(VO_U32 nStreamID)
{
	if (nStreamID == PRIVATE_STREAM_1 || 0xC0 == (nStreamID & 0xE0))
	{
		return VO_MPEG_STREAM_TYPE_AUDIO;
	}
	if (0xE0 == (nStreamID & 0xF0))
	{
		return VO_MPEG_STREAM_TYPE_VIDEO;
	}
	return VO_MPEG_STREAM_TYPE_UNKNOW;
}