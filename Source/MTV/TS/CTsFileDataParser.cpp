#include "CTsFileDataParser.h"
#include "CTsParseCtroller.h"
#include "CCheckTsPacketSize.h"
#include "voLog.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


#define Transport_Packet_Sync_Byte 0x47

CTsFileDataParser::CTsFileDataParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp)
: CvoStreamFileDataParser(pFileChunk, pMemOp)
, m_pTsCtrl(VO_NULL)
, m_pTsStreamBuf(VO_NULL)
, m_nFirstTimeStamp(MAX_TIMESTAMP_64)
, m_nLastTimeStamp(MAX_TIMESTAMP_64)
, m_nVideoCurrTimeStamp(0)
, m_nVideoPrevTimeStamp(0)
, m_nAudioCurrTimeStamp(0)
, m_nAudioPrevTimeStamp(0)
, m_nParseState(PARSE_NONE)
, m_nReadBufLen(TS_STREAM_PACKET_COUNT*188)
, m_nTsPacketLen(188)
{
	MemSet(&m_FileInfo, 0, sizeof(VO_SOURCE_INFO));
	m_FileInfo.Duration = 0xFFFFFFFF;
	m_nTsPacketSyncOffset = 0;
    m_ulOpenFlag = 0;
    InitAllFileDump();
}

CTsFileDataParser::~CTsFileDataParser(void)
{
	CloseAllFileDump();
	if (m_pTsCtrl)
	{
		m_pTsCtrl->Close();
		delete m_pTsCtrl;
		m_pTsCtrl = VO_NULL;
	}

	if (m_pTsStreamBuf)
	{
		MemFree(m_pTsStreamBuf);
		m_pTsStreamBuf = VO_NULL;
	}
}


VO_VOID CTsFileDataParser::Init(VO_PTR pParam)
{
    CTsParseCtroller*   pTSParser = NULL;

	m_pTsCtrl = new CTsParseCtroller;

	// tag: 20100423
	//m_pTsCtrl->SetParseType(PARSE_PLAYBACK);
	m_pTsCtrl->SetParseType(PARSE_ALL);
	// end

	pTSParser = (CTsParseCtroller*)m_pTsCtrl;
	pTSParser->SetOpenFlag(m_ulOpenFlag);

	m_pTsCtrl->Open(CTsFileDataParser::OnParsed, this);

	VO_U16 try_len = 188*TS_STREAM_PACKET_COUNT;
	VO_PBYTE data = new VO_BYTE[try_len];

	if(m_pFileChunk->FRead(data, try_len))
	{
		m_pFileChunk->FBack(try_len);
		CCheckTsPacketSize check;
		try_len = check.Check(data, try_len);

		if (try_len != 0)
		{
			m_nTsPacketLen = try_len;
			if(m_nTsPacketLen == 192)
			{
				m_nTsPacketSyncOffset = m_nTsPacketLen - 188;
			}
			else if(m_nTsPacketLen == 204)
			{
				m_nTsPacketSyncOffset = 0;
			}
			m_nReadBufLen = try_len * TS_STREAM_PACKET_COUNT;
		}
	}

	if (m_pTsStreamBuf)
	{
		MemFree(m_pTsStreamBuf);
		m_pTsStreamBuf = VO_NULL;
	}

	m_pTsStreamBuf = (VO_BYTE*)MemAlloc(m_nReadBufLen);

	delete data;
}

VO_VOID	CTsFileDataParser::Uninit()
{
	if(!m_pTsCtrl)
		return;

	m_pTsCtrl->Close();
	delete m_pTsCtrl;
	m_pTsCtrl = VO_NULL;

	if (m_pTsStreamBuf)
	{
		MemFree(m_pTsStreamBuf);
		m_pTsStreamBuf = VO_NULL;
	}
}

VO_BOOL	CTsFileDataParser::UpdateTimeStamp(VO_U64& nTimeStamp, VO_BOOL bVideo)
{
	if (m_nParseState & PARSE_NONE)
		return VO_FALSE;

	if (m_nParseState & PARSE_FIRST_TIMESTAMP)
	{
#ifdef _USE_RELATIVE_TIME
		if (nTimeStamp < m_nFirstTimeStamp)
		{
			m_nFirstTimeStamp = nTimeStamp;
		}
#else
		if (!bVideo && nTimeStamp < m_nFirstTimeStamp)
		{
			m_nFirstTimeStamp = nTimeStamp;
		}

		if (bVideo && nTimeStamp < m_nFirstVideoTimeStamp)
		{
			m_nFirstVideoTimeStamp = nTimeStamp;
		}
#endif
	}
	else if (m_nParseState & PARSE_LAST_TIMESTAMP)
	{
		if (nTimeStamp > m_nLastTimeStamp)
		{
			m_nLastTimeStamp = nTimeStamp;
		}
	}

	return VO_TRUE;
}

VO_VOID CTsFileDataParser::OnParsed(VO_PARSER_OUTPUT_BUFFER* pBuf)
{
	((CTsFileDataParser*)pBuf->pUserData)->HandleParseResult(pBuf);
}


VO_VOID CTsFileDataParser::HandleParseResult(VO_PARSER_OUTPUT_BUFFER* pBuf)
{
	//m_nStreamID = pBuf->nStreamID;
	VO_PARSER_STREAMINFO * pTrackInfo =NULL;

	switch (pBuf->nType)
	{
	case VO_PARSER_OT_TS_PROGRAM_INFO:
		{
			VOLOGI("program info parsed");

			m_nParseState		= PARSE_FIRST_TIMESTAMP;
		}
		break;
	case VO_PARSER_OT_STREAMINFO:
		{
			pTrackInfo = ( VO_PARSER_STREAMINFO * )pBuf->pOutputData;
			CreateDumpByStreamId(pTrackInfo->ulStreamId);
			// local playback ignore this info
		}
		break;
	case VO_PARSER_OT_TEXT:
        {
			if(m_nParseState & PARSE_PROGRAM_INFO)
				return;


			VO_MTV_FRAME_BUFFER* buf = (VO_MTV_FRAME_BUFFER*)pBuf->pOutputData;

#ifdef _USE_RELATIVE_TIME
			buf->nStartTime = CheckTimestamp(buf->nStartTime);
			// tag
			//			if(0 == buf->nStartTime)
			//				return;
#endif

			if(m_fODCallback)
				m_fODCallback(m_pODUser, m_pODUserData, pBuf);
			DumpFrame(pBuf);
			break;
		}
	case VO_PARSER_OT_AUDIO:
		{
			if(m_nParseState & PARSE_PROGRAM_INFO)
				return;

			//VOLOGI("audio parsed");

			VO_MTV_FRAME_BUFFER* buf = (VO_MTV_FRAME_BUFFER*)pBuf->pOutputData;
			
			if(UpdateTimeStamp(buf->nStartTime, VO_FALSE))
				return;

			// 20100331
#ifdef _USE_RELATIVE_TIME
			buf->nStartTime = CheckTimestamp(buf->nStartTime);

			// tag
//			if(0 == buf->nStartTime)
//				return;
#endif

#if 0
			if (m_nParseState & PARSE_NONE)
			{
				if (m_nAudioPrevTimeStamp == buf->nStartTime)
				{
					m_nAudioCurrTimeStamp += 50;
					buf->nStartTime = m_nAudioCurrTimeStamp;
				}
				else
				{
					if(buf->nStartTime < m_nAudioCurrTimeStamp)
						CDumper::WriteLog("audio time stamp error.");

					m_nAudioPrevTimeStamp = buf->nStartTime;
					m_nAudioCurrTimeStamp = buf->nStartTime;
				}
			}
#endif

			if(m_fODCallback)
				m_fODCallback(m_pODUser, m_pODUserData, pBuf);
			DumpFrame(pBuf);
		}
		break;
	case VO_PARSER_OT_VIDEO:
		{
			if(m_nParseState & PARSE_PROGRAM_INFO)
				return;

			//VOLOGI("video parsed");

			VO_MTV_FRAME_BUFFER* buf = (VO_MTV_FRAME_BUFFER*)pBuf->pOutputData;
			
			if(UpdateTimeStamp(buf->nStartTime, VO_TRUE))
				return;

			// 20100331
#ifdef _USE_RELATIVE_TIME
			buf->nStartTime = CheckTimestamp(buf->nStartTime);
			// tag
//			if(0 == buf->nStartTime)
//				return;
#endif

#if 0
			if (m_nParseState & PARSE_NONE)
			{
				if (m_nVideoPrevTimeStamp == buf->nStartTime)
				{
					m_nVideoCurrTimeStamp += 25;
					buf->nStartTime = m_nVideoCurrTimeStamp;
				}
				else
				{
					if(buf->nStartTime < m_nVideoCurrTimeStamp)
						CDumper::WriteLog("video time stamp error.");

					m_nVideoPrevTimeStamp = buf->nStartTime;
					m_nVideoCurrTimeStamp = buf->nStartTime;
				}
			}
#endif

			if(m_fODCallback)
				m_fODCallback(m_pODUser, m_pODUserData, pBuf);
			DumpFrame(pBuf);
		}
		break;
	case VO_PARSER_OT_ERROR:
		{
			//IssueParseResult(pBuf->nType, pBuf->pOutputData);
		}
		break;
	default:
		{
// 			pBuf->pUserData	= m_pUserData;
// 			m_pCallback(pBuf);
		}
		break;
	}
}

VO_U8* CTsFileDataParser::FindPacketHeader(VO_U8* pData, int cbData, int packetSize)
{
	VO_U8* p = pData;
	VO_U8* p2 = pData + cbData - packetSize;
	while (p < p2)
	{
		if ( (*p == Transport_Packet_Sync_Byte) && (*(p + packetSize) == Transport_Packet_Sync_Byte) )
			return p;
		++p;
	}

	return 0;
}

VO_BOOL	CTsFileDataParser::StepB()
{
#if 0
	VO_BOOL bRet = m_pFileChunk->FRead(m_pTsStreamBuf, 1);

	if (!bRet)
		return VO_FALSE;

	// find start code
	if(*m_pTsStreamBuf != Transport_Packet_Sync_Byte)
		return VO_TRUE;

	bRet = m_pFileChunk->FRead(m_pTsStreamBuf+1, m_nTsPacketLen);

	if (!bRet)
		return VO_FALSE;

	VO_U32 total = 0;

	VO_U8* pStart = FindPacketHeader(m_pTsStreamBuf, m_nTsPacketLen+1, m_nTsPacketLen);

	if (!pStart)
	{
		m_pFileChunk->FBack(m_nTsPacketLen);
		return VO_TRUE;
	}
	else
	{
		// tag: 20100421
		//m_pFileChunk->FRead(m_pTsStreamBuf+(m_nTsPacketLen+1), m_nReadBufLen-(m_nTsPacketLen+1));

		VO_U32 read = 0;
		total = (m_nTsPacketLen+1);
		if(m_pFileChunk->FRead3(m_pTsStreamBuf+total, m_nReadBufLen-total, &read))
			total += read;
		else
			total -= 1;

		if(total != m_nReadBufLen)
			CDumper::WriteLog("test....%d", total);
	}
#endif
	VO_U32 read = 0;
	if(!m_pFileChunk->FRead3(m_pTsStreamBuf, m_nReadBufLen, &read))
		return VO_FALSE;
	return (VO_BOOL)(VO_ERR_SOURCE_OK == Process(m_pTsStreamBuf, read));
	//return (VO_BOOL)(VO_ERR_SOURCE_OK == Process(m_pTsStreamBuf, m_nReadBufLen));

	// end

}

CStream* CTsFileDataParser::GetStreamByIdx(VO_U32 nIdx)
{
	if(!m_pTsCtrl)
		return VO_NULL;

	return m_pTsCtrl->GetStreamByIdx(nIdx);
}

void     CTsFileDataParser::SetEITCallbackFun(void*  pFunc)
{
    CTsParseCtroller*   pTsParserController = NULL;
    if(pFunc == NULL)
	{
		return;
	}
	else
	{
		pTsParserController = (CTsParseCtroller*)m_pTsCtrl;
		if(pTsParserController != NULL)
		{
			pTsParserController->SetEITCallbackFun(pFunc);
		}
	}
}


VO_U32	CTsFileDataParser::Process(VO_BYTE* pData, VO_U32 nLen)
{
	VO_PARSER_INPUT_BUFFER buf;
	MemSet(&buf, 0, sizeof(VO_PARSER_INPUT_BUFFER));
	buf.pBuf	= pData;
	buf.nBufLen	= nLen;

	return m_pTsCtrl->Process(&buf);
}

VO_BOOL	CTsFileDataParser::GetFileInfo(VO_SOURCE_INFO* pSourceInfo)
{
    CTsParseCtroller*    pTsCtrol = NULL;
	if(m_FileInfo.Duration != MAX_TIMESTAMP)
	{
		MemCopy(pSourceInfo, &m_FileInfo, sizeof(VO_SOURCE_INFO));
		return VO_TRUE;
	}


	VO_U32  nCanPlayback = 0;
	VO_U32  nRealRead	= 0;
	VO_U32	nLen		= 0;
	VO_U32	nReadLen	= 0;
	VO_U32	nHeaderLen	= 0;
	VO_BOOL bRet		= VO_FALSE;
	m_nFirstTimeStamp	= MAX_TIMESTAMP_64;
	m_nFirstVideoTimeStamp = MAX_TIMESTAMP_64;
	m_nLastTimeStamp	= 0;
	m_nParseState		= PARSE_PROGRAM_INFO;

	if((m_ulOpenFlag & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL) == VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL)
	{
		pTsCtrol = (CTsParseCtroller*)m_pTsCtrl;

		while (!pTsCtrol->IsProgramInfoParsed() || !pTsCtrol->IsVideoTrackReady())
		{
			bRet = m_pFileChunk->FRead3(m_pTsStreamBuf, m_nReadBufLen, &nRealRead);

			if (bRet)
			{
				nReadLen += nRealRead;
				nLen = 0;
				VO_PBYTE pStartPos = FindPacketHeader(m_pTsStreamBuf,m_nReadBufLen,m_nTsPacketLen);
				if (NULL == pStartPos)
				{
					return VO_FALSE;
				}

				nLen = pStartPos - m_pTsStreamBuf;

				while (nLen < nRealRead && ( !pTsCtrol->IsProgramInfoParsed() || !pTsCtrol->IsVideoTrackReady()))
				{
					Process(m_pTsStreamBuf+nLen-m_nTsPacketSyncOffset, m_nTsPacketLen); // m_nTsPacketLen == 188
					nHeaderLen	+= m_nTsPacketLen;
					nLen		+= m_nTsPacketLen;
				}
			}
			else
				break;
		}
		if(!bRet)
			return VO_FALSE;

		m_pFileChunk->FLocate(0);
		m_nParseState = PARSE_NONE;
		m_FileInfo.Tracks	= m_pTsCtrl->GetStreamCount();
		m_FileInfo.Duration = 0;
		MemCopy(pSourceInfo, &m_FileInfo, sizeof(VO_SOURCE_INFO));
		m_nParseState = PARSE_NONE;
		return VO_TRUE;
	}


	while (!m_pTsCtrl->IsProgramInfoParsed())
	{
		bRet = m_pFileChunk->FRead3(m_pTsStreamBuf, m_nReadBufLen, &nRealRead);

		if (bRet)
		{
			nReadLen += nRealRead;
			nLen = 0;
			VO_PBYTE pStartPos = FindPacketHeader(m_pTsStreamBuf,m_nReadBufLen,m_nTsPacketLen);
			if (NULL == pStartPos)
			{
				return VO_FALSE;
			}

			nLen = pStartPos - m_pTsStreamBuf;
            
			while (nLen < nRealRead && !m_pTsCtrl->IsProgramInfoParsed())
			{
				Process(m_pTsStreamBuf+nLen-m_nTsPacketSyncOffset, m_nTsPacketLen); // m_nTsPacketLen == 188
				nHeaderLen	+= m_nTsPacketLen;
				nLen		+= m_nTsPacketLen;
			}
		}
		else
			break;
	}
	if(!bRet)
		return VO_FALSE;

	Reset();
	// tag
	//m_pFileChunk->FLocate(nHeaderLen);
	m_pFileChunk->FLocate(0);



	nReadLen = 0;
	nLen = 0;

	//add by qichaoshen @2011-10-28   get the can playback flag
	m_pTsCtrl->GetParam(_CHECK_TS_PLAYBACK_READY, &nCanPlayback);
	//add by qichaoshen @2011-10-28   get the can playback flag

	if(m_nParseState!=PARSE_PROGRAM_INFO && m_nFirstTimeStamp==MAX_TIMESTAMP_64)
	{
		m_nParseState		= PARSE_FIRST_TIMESTAMP;

		//modify by qichaoshen @ 2011-10-28 add playback ready check
//#ifdef _USE_RELATIVE_TIME
//		while (MAX_TIMESTAMP_64 == m_nFirstTimeStamp)
//#else
//		while (MAX_TIMESTAMP_64 == m_nFirstTimeStamp || m_nFirstVideoTimeStamp==MAX_TIMESTAMP_64)
//#endif
        //modify by qichaoshen @ 2011-10-28 add playback ready check
#ifdef _USE_RELATIVE_TIME
		while ((MAX_TIMESTAMP_64 == m_nFirstTimeStamp) || !nCanPlayback)
#else
		while ((MAX_TIMESTAMP_64 == m_nFirstTimeStamp || m_nFirstVideoTimeStamp==MAX_TIMESTAMP_64) || !nCanPlayback)
#endif
	    //modify by qichaoshen @ 2011-10-28 add playback ready check
		{
			bRet = m_pFileChunk->FRead3(m_pTsStreamBuf, m_nReadBufLen, &nRealRead);

			if (bRet)
			{
				nReadLen += nRealRead;
				nLen = 0;

				VO_PBYTE pStartPos = FindPacketHeader(m_pTsStreamBuf,m_nReadBufLen,m_nTsPacketLen);
				if (NULL == pStartPos)
				{
					return VO_FALSE;
				}

				nLen = pStartPos - m_pTsStreamBuf;
                if (nLen < nRealRead)
				{
					Process(m_pTsStreamBuf+nLen-m_nTsPacketSyncOffset, nRealRead-nLen+m_nTsPacketSyncOffset);
				    //add by qichaoshen @2011-10-28
				    m_pTsCtrl->GetParam(_CHECK_TS_PLAYBACK_READY, &nCanPlayback);
				    //add by qichaoshen @2011-10-28
				}
			}
			else
				break;
		}

		if(!bRet)
		{
			//can't get the stream header data
            if( 0 == m_pTsCtrl->GetStreamCount())
			{
			    return VO_FALSE;
			}
			else
			{
				m_pTsCtrl->SetParam(_FORCE_TO_PLAY,&bRet);

				//get the firsttimestamp again
				Reset();
				m_pFileChunk->FLocate(0);

		        while ((MAX_TIMESTAMP_64 == m_nFirstTimeStamp))
				{
					bRet = m_pFileChunk->FRead3(m_pTsStreamBuf, m_nReadBufLen, &nRealRead);
					if (bRet)
					{
						Process(m_pTsStreamBuf, nRealRead);
					}
					else
						break;
				}

                //if can't the first timestamp ,the media file is error
				if(!bRet)
				{
					return VO_FALSE;
				}
			}
		}
	}

	Reset();
	m_nParseState		= PARSE_LAST_TIMESTAMP;
	m_nLastTimeStamp	= 0;
	
	VO_U64 ullFilePos = m_nFileSize;

	//Make sure find the last timestamp
	int    iTryTimesFindLastStamp = 0;
	int    iLeastTimeToGetTheLastTime = 4;

	//try at least 0x80*37600 Length in the file end
	while ( ((m_nFirstTimeStamp >= m_nLastTimeStamp) && (iTryTimesFindLastStamp<0x80))
		  || (iTryTimesFindLastStamp < iLeastTimeToGetTheLastTime))
	{
		if (ullFilePos <= 0)
		{
			if(m_nFirstTimeStamp >= m_nLastTimeStamp)
			{
			    bRet = VO_FALSE;
			    break;
			}
			else
			{
				bRet = VO_TRUE;
				break;
			}
		}
		
		ullFilePos = (ullFilePos > m_nReadBufLen)? ullFilePos-m_nReadBufLen : 0;
		m_pFileChunk->FLocate(ullFilePos);
		bRet = m_pFileChunk->FRead3(m_pTsStreamBuf, m_nReadBufLen, &nRealRead);

		VO_PBYTE pStartPos = FindPacketHeader(m_pTsStreamBuf,m_nReadBufLen,m_nTsPacketLen);
		if (NULL == pStartPos)
		{
			return VO_FALSE;
		}

		nLen = pStartPos - m_pTsStreamBuf;

		if (bRet)
		{
			Process(m_pTsStreamBuf+nLen-m_nTsPacketSyncOffset, nRealRead-nLen+m_nTsPacketSyncOffset);
		}
		else
			break;

		iTryTimesFindLastStamp++;
	}
	if(!bRet)
		return VO_FALSE;

	// tag
	//m_pFileChunk->FLocate(nHeaderLen);
	m_pFileChunk->FLocate(0);
	m_nParseState = PARSE_NONE;

	m_FileInfo.Duration	= (VO_U32)(m_nLastTimeStamp - m_nFirstTimeStamp);
	m_FileInfo.Tracks	= m_pTsCtrl->GetStreamCount();
	MemCopy(pSourceInfo, &m_FileInfo, sizeof(VO_SOURCE_INFO));

	m_nParseState = PARSE_NONE;

	// test code 20100617
#if 0
	m_FileInfo.Tracks = 1;
#endif
	// end test

	//m_pTsCtrl->SetTimeStampOffset(m_nFirstTimeStamp);

	CDumper::WriteLog((char *)"TS duration = %f(s), stream count = %d", m_FileInfo.Duration/1000.0, m_FileInfo.Tracks);

	return VO_TRUE;
}


/*
VO_BOOL	CTsFileDataParser::GetFileInfo(VO_SOURCE_INFO* pSourceInfo)
{
	if(m_FileInfo.Duration != MAX_TIMESTAMP)
	{
		MemCopy(pSourceInfo, &m_FileInfo, sizeof(VO_SOURCE_INFO));
		return VO_TRUE;
	}
		
	VO_U32  nRealRead	= 0;
	VO_U32	nLen		= 0;
	VO_U32	nReadLen	= 0;
	VO_U32	nHeaderLen	= 0;
	VO_BOOL bRet		= VO_FALSE;
	m_nFirstTimeStamp	= MAX_TIMESTAMP;
	m_nFirstVideoTimeStamp = MAX_TIMESTAMP;
	m_nLastTimeStamp	= 0;
	m_nParseState		= PARSE_PROGRAM_INFO;

	while (!m_pTsCtrl->IsProgramInfoParsed())
	{
		bRet = m_pFileChunk->FRead(m_pTsStreamBuf, m_nReadBufLen);

		if (bRet)
		{
			nReadLen += m_nReadBufLen;
			nLen = 0;
			while (nLen<=m_nReadBufLen && !m_pTsCtrl->IsProgramInfoParsed())
			{
				Process(m_pTsStreamBuf+nLen, 188);
				nHeaderLen	+= 188;
				nLen		+= 188;
			}
		}
		else
			break;
	}
	if(!bRet)
		return VO_FALSE;

	Reset();
	// tag
	//m_pFileChunk->FLocate(nHeaderLen);
	m_pFileChunk->FLocate(0);

	if(m_nParseState!=PARSE_PROGRAM_INFO && m_nFirstTimeStamp==MAX_TIMESTAMP)
	{
		m_nParseState		= PARSE_FIRST_TIMESTAMP;
#ifdef _USE_RELATIVE_TIME
		while (MAX_TIMESTAMP == m_nFirstTimeStamp)
#else
		while (MAX_TIMESTAMP == m_nFirstTimeStamp || m_nFirstVideoTimeStamp==MAX_TIMESTAMP)
#endif
		{
			bRet = m_pFileChunk->FRead(m_pTsStreamBuf, m_nReadBufLen);

			if (bRet)
			{
				Process(m_pTsStreamBuf, m_nReadBufLen);
			}
			else
				break;
		}
		if(!bRet)
			return VO_FALSE;
	}

	Reset();
	m_nParseState		= PARSE_LAST_TIMESTAMP;
	m_nLastTimeStamp	= 0;
	m_pFileChunk->FLocate(m_nFileSize - m_nReadBufLen*10);

	while (0 == m_nLastTimeStamp)
	{
		bRet = m_pFileChunk->FRead3(m_pTsStreamBuf, m_nReadBufLen, &nRealRead);

		if (bRet)
		{
			Process(m_pTsStreamBuf, nRealRead);
		}
		else
			break;
	}
	if(!bRet)
		return VO_FALSE;

	// tag
	//m_pFileChunk->FLocate(nHeaderLen);
	m_pFileChunk->FLocate(0);
	m_nParseState = PARSE_NONE;

	m_FileInfo.Duration	= (m_nLastTimeStamp - m_nFirstTimeStamp);
	m_FileInfo.Tracks	= m_pTsCtrl->GetStreamCount();
	MemCopy(pSourceInfo, &m_FileInfo, sizeof(VO_SOURCE_INFO));

	m_nParseState = PARSE_NONE;

	//m_pTsCtrl->SetTimeStampOffset(m_nFirstTimeStamp);

	CDumper::WriteLog("TS duration = %f(s), stream count = %d", m_FileInfo.Duration/1000.0, m_FileInfo.Tracks);

	return VO_TRUE;
}
*/

VO_S64	CTsFileDataParser::GetFirstTimeStamp()
{
	return m_nFirstTimeStamp;
}

VO_S64	CTsFileDataParser::Reset()
{
	m_nVideoCurrTimeStamp = 0;
	m_nAudioCurrTimeStamp = 0;

	m_pTsCtrl->ResetState();

	return VO_ERR_SOURCE_OK;
}

VO_U64	CTsFileDataParser::CheckTimestamp(VO_U64 time)
{
	if(m_nFirstTimeStamp == MAX_TIMESTAMP_64)
		return time;

	if(time < m_nFirstTimeStamp)
	{
		VOLOGI("buf->nStartTime is smaller than FirstTimeStamp");	
		return 0;
	}
	return time - m_nFirstTimeStamp;
}


VO_VOID	CTsFileDataParser::InitAllFileDump()
{
#ifdef _DUMP_FRAME
	VO_U32   ulIndex = 0;
    for(ulIndex=0; ulIndex<0x2000; ulIndex++)
    {
        m_pFileData[ulIndex] = NULL;
		m_pFileTimeStamp_Size[ulIndex] = NULL;
    }
#endif
}

VO_VOID	CTsFileDataParser::CreateDumpByStreamId(VO_U32 ulPID)
{
#ifdef _DUMP_FRAME
	VO_U32   ulIndex = 0;
	VO_CHAR    strDumpPath[256] = {0};
    FILE*      pFile = NULL;
    VO_CHAR*   pStringValue = NULL;
	sprintf(strDumpPath, "C:\\Dump\\local_original_%d_data.dat", ulPID);
	pFile = fopen(strDumpPath, "wb");
	if(pFile != NULL)
	{
		m_pFileData[ulPID] = pFile;
	}

	memset(strDumpPath, 0, 256);
	sprintf(strDumpPath, "C:\\Dump\\local_original_%d_timestamp_size.dat", ulPID);
	pFile = fopen(strDumpPath, "wb");
	if(pFile != NULL)
	{
		m_pFileTimeStamp_Size[ulPID] = pFile;
		pStringValue = "TimeStamp    Size\n";
		fwrite(pStringValue, 1, strlen(pStringValue), pFile);
		fflush(pFile);
	}
#endif
}

VO_VOID	CTsFileDataParser::DumpFrame(VO_PARSER_OUTPUT_BUFFER* pBuf)
{
#ifdef _DUMP_FRAME
    FILE*   pFileDump = NULL;
	VO_MTV_FRAME_BUFFER*   pFrame = NULL;
	VO_U32   ulIndex = 0;
	VO_CHAR    strInfoDump[256] = {0};
	if(pBuf == NULL)
	{
		return;
	}

	pFrame = (VO_MTV_FRAME_BUFFER*)pBuf->pOutputData;

	pFileDump = m_pFileData[pBuf->nStreamID];
	if(pFileDump != NULL)
	{
		fwrite(pFrame->pData, 1, pFrame->nSize, pFileDump);
		fflush(pFileDump);
	}

	memset(strInfoDump, 0, 256);
	pFileDump = m_pFileTimeStamp_Size[pBuf->nStreamID];
	if(pFileDump != NULL)
	{
		sprintf(strInfoDump, "%d", pFrame->nStartTime);
		fwrite(strInfoDump, 1, strlen(strInfoDump), pFileDump);
		memset(strInfoDump, 0, 256);
		sprintf(strInfoDump, "        %d\n", pFrame->nSize);
		fwrite(strInfoDump, 1, strlen(strInfoDump), pFileDump);
		fflush(pFileDump);
	}
#endif
}

VO_VOID	CTsFileDataParser::CloseAllFileDump()
{
#ifdef _DUMP_FRAME
	VO_U32   ulIndex = 0;
	for(ulIndex=0; ulIndex<0x2000; ulIndex++)
	{
		if(m_pFileData[ulIndex] != NULL)
		{
			fclose(m_pFileData[ulIndex]);
		}


		if(m_pFileTimeStamp_Size[ulIndex] != NULL)
		{
			fclose(m_pFileTimeStamp_Size[ulIndex]);
		}
	}
#endif
}

void	CTsFileDataParser::SetOpenFlag(VO_U32 ulOpenFlag)
{
	m_ulOpenFlag = ulOpenFlag;
	CTsParseCtroller*   pTSParser = NULL;
	if(m_pTsCtrl != NULL)
	{
		pTSParser = (CTsParseCtroller*)m_pTsCtrl;
		pTSParser->SetOpenFlag(ulOpenFlag);
	}
}
