#include "CMpegReader.h"
#include "CMpegAudioTrack.h"
#include "CMpegVideoTrack.h"
#include "CMpegAVCVideoTrack.h"
#include "voOSFunc.h"
#define LOG_TAG "CMpegReader"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define VO_MPEG_MAXSCANSIZE				0x80000	//1M
#define VO_MPEG_SCANSIZE				0x10000		//64K

static VO_U8 GOnTimeStamp(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam)
{
	if (*(VO_U32 *)pUserData == ParsedDataType_LastTimeStamp)
	{
		((CMpegReader*)pUser)->SetLastTimeStamp(((PMpegDataParserFrame)pParam)->dwTimeStamp);
	}
	else if (*(VO_U32 *)pUserData == ParsedDataType_TimeStamp)
	{
		((CMpegReader*)pUser)->SetPESInfo(((PMpegDataParserFrame)pParam)->btStreamID,((PMpegDataParserFrame)pParam)->dwTimeStamp,((PMpegDataParserFrame)pParam)->dwPESPos);
	}
	
	return CBRT_CONTINUABLEANDNEEDSKIP;
}

static VO_U8 GOnData(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam)
{
	CMpegReader* pReader = (CMpegReader*)pUser;
	if (pUserData)
	{
		VO_U32 *DataType = (VO_U32*)pUserData;
		if (ParsedDataType_DrmInfo == *DataType)
		{
			return pReader->OnDrmCallBack(pParam);
		}	
	}
	PMpegDataParserFrame pFrame = (PMpegDataParserFrame)pParam;

	return pReader->OnPacket(pFrame->pFileChunk, pFrame->btStreamID, pFrame->wPacketLen, pFrame->dwTimeStamp,pFrame->beEncrpt);
}
/*
static VO_U8 GOnDrmData(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam)
{
	CMpegReader* pReader = (CMpegReader*)pUser;
	PMpegDataDrmInfo pDrmInfo = (PMpegDataDrmInfo)pParam;
	return pReader->DRMInfo_Widevine(pDrmInfo->pDrmData,pDrmInfo->btDrmDataSize);
}
*/
static VO_BOOL GOnBlock(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam)
{
	CMpegTrack* pTrack = (CMpegTrack*)pUser;
	return pTrack->OnBlock(*(VO_U32*)pParam);
}

CMpegReader::CMpegReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
	: CBaseStreamFileReader(pFileOp, pMemOp, pLibOP, pDrmCB)
	, m_DataParser(&m_chunk, pMemOp)
	, m_dwFirstTimeStamp(-1)
	, m_dwLastTimeStamp(-1)
	, m_ForStreaming(VO_FALSE)
	, m_pVideoCacheHeadDataBuf(NULL)
	, m_nVideoCacheBufSize(0)
	, m_bNeedCheckCache(VO_FALSE)
	, m_bOpenEnd(VO_FALSE)
	, m_nCurBufTime(MPEG_MAX_BUFFER_TIME_LOCAL)
	, m_uInitTrackCnt(2)
{
	m_pFileDataParser = &m_DataParser;
	m_CurPESInfo.Init();
}

CMpegReader::~CMpegReader()
{
	Close();
}

#define VO_MPEG_SCANSIZE_ONCE			0x10000

VO_VOID	CMpegReader::SetLastTimeStamp(VO_S64 dwTimeStamp) 
{
	if (dwTimeStamp != -1)
	{
		m_dwLastTimeStamp = dwTimeStamp;
	}
	
}
VO_U32 CMpegReader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	Close();
	if ((nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL))
	{
		m_DataParser.SetParserMode(VO_MPEG_PARSER_MODE_THUMBNAIL);
		m_nCurBufTime = VO_MAXS32;
	}
	VO_U32 rc = CBaseStreamFileReader::Load(nSourceOpenFlags, pFileSource);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;
	if ((nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_OPENPD))
	{
		m_ForStreaming = VO_TRUE;
		m_nCurBufTime = MPEG_MAX_BUFFER_TIME_STREAMING;///<For streaming
	}
	m_DataParser.Init(VO_NULL);

	if(!(nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL))
	{
		// get last time stamp
		VO_U32 nDataType = ParsedDataType_LastTimeStamp;
		m_DataParser.SetOnDataCallback(this, &nDataType, GOnTimeStamp);
		VO_U64 ullFilePos = m_ullFileSize;

		while(m_dwLastTimeStamp == -1)
		{
			if(ullFilePos <= 0 || ullFilePos + VO_MPEG_MAXSCANSIZE <= m_ullFileSize)
				return VO_ERR_SOURCE_ERRORDATA;

			ullFilePos = (ullFilePos <= VO_MPEG_SCANSIZE) ? 0 : (ullFilePos - VO_MPEG_SCANSIZE);
			if(!m_DataParser.SetStartFilePos(ullFilePos))
				return VO_ERR_SOURCE_END;

			while(m_DataParser.Step());
		}
		if(m_dwLastTimeStamp == -1)
			return VO_FALSE;
	}
	
	rc = InitTracks(nSourceOpenFlags);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	m_DataParser.SetOnDataCallback(this, NULL, GOnData);
	if(!m_DataParser.SetStartFilePos(0))
		return VO_ERR_SOURCE_END;

	m_DataParser.SetOnDataCallback(this, VO_NULL, GOnData);
	if (m_DataParser.GetParserMode() == VO_MPEG_PARSER_MODE_THUMBNAIL)
	{
		FindAndCreateTrack();
	}
	else
	{
		FileGenerateIndex();
		for(VO_U32 i = 0; i < m_nTracks; i++)
		{
			if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed() && ((CBaseStreamFileTrack*)m_ppTracks[i])->IsIndexNull())	//still no frame, then we should not use it!!
				m_ppTracks[i]->SetInUsed(VO_FALSE);
		}
	}
	m_bOpenEnd = VO_TRUE;
	return VO_ERR_SOURCE_OK;
}

VO_U32 CMpegReader::Close()
{
	UnprepareTracks();
	UninitTracks();

	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		SAFE_DELETE(m_ppTracks[i]);
	}
	if (m_pVideoCacheHeadDataBuf)
	{
		MemFree(m_pVideoCacheHeadDataBuf);
		m_pVideoCacheHeadDataBuf = NULL;
	}
	return CBaseStreamFileReader::Close();
}

VO_U32 CMpegReader::InitTracks(VO_U32 nSourceOpenFlags)
{
	m_uInitTrackCnt = 2;
	if (m_DataParser.GetParserMode() == VO_MPEG_PARSER_MODE_THUMBNAIL)
	{
		m_uInitTrackCnt = 1;
	}
	return TracksCreate(m_uInitTrackCnt);
}

VO_U8 CMpegReader::OnPacket(CGFileChunk* pFileChunk, VO_U8 btStreamID, VO_U16 wLen, VO_S64 dwTimeStamp,VO_BOOL beEncrpt)
{
	CMpegTrack *pTrack = (CMpegTrack*)GetTrackByStreamNum(btStreamID);
	if(!pTrack)
	{
		if(m_dwFirstTimeStamp == -1)
		{
			if (dwTimeStamp == -1)
			{
				if ((VO_MPEG_STREAM_TYPE_VIDEO == m_DataParser.CheckMediaTypeByStreamID(btStreamID)) && !m_pVideoCacheHeadDataBuf)
				{
					m_pVideoCacheHeadDataBuf = (VO_PBYTE)MemAlloc(wLen);
					if(!pFileChunk->FRead(m_pVideoCacheHeadDataBuf, wLen))
					{
						MemFree(m_pVideoCacheHeadDataBuf);
						m_pVideoCacheHeadDataBuf = NULL;
						wLen = 0;
						return CBRT_CONTINUABLEANDNEEDSKIP;
					}
					m_nVideoCacheBufSize = wLen;
					m_bNeedCheckCache = VO_TRUE;
				}
				else
					pFileChunk->FSkip(wLen);
				return CBRT_CONTINUABLENOTNEEDSKIP;
			}
			m_dwFirstTimeStamp = dwTimeStamp;
		}

		for(VO_U32 i = 0; i < 2; i++)
		{
			if(!m_ppTracks[i])
			{
				CMpegTrack* pNewTrack = VO_NULL;
				if ( btStreamID == PRIVATE_STREAM_1 )
				{
					if ((m_DataParser.GetParserMode() & VO_MPEG_PARSER_MODE_AUDIO))
					{
						VO_PBYTE pPESBuffer = new VO_BYTE[wLen];
						pFileChunk->FRead(pPESBuffer,wLen);
						if (beEncrpt)
							DRMData_Widevine_PESData(pPESBuffer,wLen,NULL,NULL);

						if ((pPESBuffer[0] >= STREAM_TYPE_AUDIO_AC3_MIN_1 && pPESBuffer[0] <= STREAM_TYPE_AUDIO_DTS_MAX_1)
							||(pPESBuffer[0] >= STREAM_TYPE_AUDIO_DTS_MIN_2 && pPESBuffer[0] <= STREAM_TYPE_AUDIO_LPCM_MAX)
							||(pPESBuffer[0] >= STREAM_TYPE_AUDIO_AC3_MIN_2 && pPESBuffer[0] <= STREAM_TYPE_AUDIO_AC3_MAX_2))
						{
							pNewTrack = new CMpegAudioTrack(btStreamID, m_dwFirstTimeStamp, m_dwLastTimeStamp - m_dwFirstTimeStamp + 1, this, m_pMemOp, pPESBuffer[0]);
							pFileChunk->FBack(wLen);
							SAFE_DELETE(pPESBuffer);
						}
						else
						{
							pFileChunk->FBack(wLen);
							SAFE_DELETE(pPESBuffer);
							return CBRT_CONTINUABLEANDNEEDSKIP;
						}
					}
					
				}
				else
				{
					if(VO_MPEG_STREAM_TYPE_VIDEO == m_DataParser.CheckMediaTypeByStreamID(btStreamID))
					{
						StreamInfo* TmpInfo = (StreamInfo*)m_DataParser.GetStreamInfo();
						while(TmpInfo)
						{
							if (TmpInfo->stream_type == STREAM_TYPE_VIDEO_H264)
							{
								pNewTrack = new CMpegAVCVideoTrack(btStreamID, m_dwFirstTimeStamp, m_dwLastTimeStamp - m_dwFirstTimeStamp + 1, this, m_pMemOp);
								break;
							}
							TmpInfo = TmpInfo->next;
						}
						if (!pNewTrack)
						{
							pNewTrack = new CMpegVideoTrack(btStreamID, m_dwFirstTimeStamp, m_dwLastTimeStamp - m_dwFirstTimeStamp + 1, this, m_pMemOp);
						}
					}
					else if(VO_MPEG_STREAM_TYPE_AUDIO == m_DataParser.CheckMediaTypeByStreamID(btStreamID)
							&&(m_DataParser.GetParserMode() & VO_MPEG_PARSER_MODE_AUDIO))
						pNewTrack = new CMpegAudioTrack(btStreamID, m_dwFirstTimeStamp, m_dwLastTimeStamp - m_dwFirstTimeStamp + 1, this, m_pMemOp);
				}
				if(!pNewTrack)
					return CBRT_CONTINUABLEANDNEEDSKIP;

				m_ppTracks[i] = pNewTrack;
				m_nTracks++;

				pTrack = pNewTrack;

				break;
			}
		}
	}

	if(pTrack)
	{
		if (pTrack->GetType() == VOTT_VIDEO && m_bNeedCheckCache)
		{
			pTrack->CheckHeadDataBuf(m_pVideoCacheHeadDataBuf,m_nVideoCacheBufSize);
			m_bNeedCheckCache = VO_FALSE;
		}
		if(!pTrack->OnPacket(pFileChunk, wLen, dwTimeStamp,beEncrpt))
			return CBRT_FALSE;

		return CBRT_CONTINUABLENOTNEEDSKIP;
	}
	else
		return CBRT_CONTINUABLEANDNEEDSKIP;
}

VO_U32 CMpegReader::MoveTo(VO_S64 llTimeStamp)
{
	VOLOGE("llTimeStamp==%lld:%d",llTimeStamp,voOS_GetSysTime());
	VO_S64 nTimeStamp = llTimeStamp;
	VO_S64 nStartFilePos = 0,nEndFilePos = 0,nLastValidPos = 0;

	VO_U32 nTrack = GetSeekTrack();

	VO_U32 nTimeUnit = m_nCurBufTime > 1000 ? m_nCurBufTime: 1000;///<m_nCurBufTime is the buffer time,because of the diff (500ms),so we must confirm we can generate the (m_nCurBufTime - 1) duration data in buffer
	if (nTrack < m_nTracks && m_ppTracks[nTrack])
	{
		CMpegTrack *pTrack = (CMpegTrack *)m_ppTracks[nTrack];
		VO_BOOL bRetry = VO_TRUE;
		MPEGPESInfo stMaxPESInfo;
		stMaxPESInfo.Init();

		while(bRetry && nTimeStamp >= nTimeUnit && nTimeStamp + 10000 > llTimeStamp)
		{
			VO_S64 nLoopMaxTime = nTimeStamp;
			nTimeStamp -= nTimeUnit;
			VOLOGE("nTimeStamp==%lld,nTimeUnit==%d,nLoopMaxTime == %lld:%d",nTimeStamp,nTimeUnit,nLoopMaxTime,voOS_GetSysTime());
			nStartFilePos = GetFilePosByTime(nTimeStamp,&nTimeStamp,pTrack->GetStreamNum(),NULL,&stMaxPESInfo);
			stMaxPESInfo.dwTimeStamp = nTimeStamp;
			stMaxPESInfo.dwPESPos = nStartFilePos;

			VOLOGE("nStartFilePos==%lld,nTimeStamp==%d:%d",nStartFilePos,nTimeStamp,voOS_GetSysTime());
			MPEGFrameInfo info;

			Flush();
			m_DataParser.SetStartFilePos(nStartFilePos);
			do
			{
				
				Flush2();
				FileGenerateIndex();
				
				nEndFilePos = m_DataParser.GetCurrParseFilePos();
				
				
				info.Init();
				if (VO_ERR_SOURCE_OK == pTrack->GetFrameInfoInIndex(&info,nLoopMaxTime))
				{
					VOLOGE("info.Max==%lld,info.Min==%lld,info.key==%lld:%d",info.nMaxTimeStamp,info.nMinTimeStamp,info.nNearKeyFramTimeStamp,voOS_GetSysTime());
					if (info.nNearKeyFramTimeStamp != -1)
					{
						if (info.nMaxTimeStamp >= nLoopMaxTime)
						{
							nStartFilePos = -1;
							bRetry = VO_FALSE;
							break;
						}
						else
						{
							nLastValidPos = nStartFilePos;
							nStartFilePos = nEndFilePos;///<we can not know which is last PES for every stream,so we have to skip the PES
						}
					}
					else
					{
						if (info.nMaxTimeStamp >= nLoopMaxTime )
						{
							if (nLastValidPos)
							{
								nStartFilePos = nLastValidPos;
								bRetry = VO_FALSE;
							}
							///<should happen once when seek
							if (nStartFilePos == 0)
							{
								nStartFilePos = -1;
								bRetry = VO_FALSE;///<if can not find right frame when startpos ==0,quit 
							}
							break;
						}
						else
						{
							nStartFilePos = nEndFilePos;///<we can not know which is last PES for every stream,so we have to skip the PES
						}
					}
				}
				else
				{
					nStartFilePos = 0;
					bRetry = VO_FALSE;
					break;
				}
			}while(info.nMaxTimeStamp < nLoopMaxTime);
		}
	}
	else
		nStartFilePos = 0;
	VOLOGE("nStartFilePos==%lld:%d",nStartFilePos,voOS_GetSysTime());
	if (nStartFilePos >= 0)
	{
		Flush();
		m_DataParser.SetStartFilePos(nStartFilePos);
		FileGenerateIndex();
	}
	return CBaseStreamFileReader::MoveTo(llTimeStamp);
}

VO_VOID CMpegReader::OnNewDataParser(CBaseTrack* pRqstTrack, CvoFileDataParser** ppDataParser)
{
	CBaseStreamFileReader::OnNewDataParser(pRqstTrack, ppDataParser);

	CMpegDataParser* pNewDataParser = new CMpegDataParser(&m_chunkUseForDataParser, m_pMemOp);
	if(!pNewDataParser)
		return;

	pNewDataParser->Init(VO_NULL);
	pNewDataParser->SetOnDataCallback(this, VO_NULL, GOnData);
	pNewDataParser->SetBlockStream(GetOtherTrackInUsed(pRqstTrack)->GetStreamNum());
	pNewDataParser->InitParser(m_DataParser);
	m_DataParser.SetBlockStream(pRqstTrack->GetStreamNum());
	m_DataParser.SetOnBlockCallback(pRqstTrack, VO_NULL, GOnBlock);

	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed())
			((CBaseStreamFileTrack*)m_ppTracks[i])->SetParseForSelf(VO_TRUE);
	}

	*ppDataParser = pNewDataParser;
}

VO_U32 CMpegReader::GetBitrate()
{
	return 0;
//	return m_ullFileSize * 1000 / (m_dwLastTimeStamp - m_dwFirstTimeStamp + 1);
}

VO_VOID CMpegReader::Flush()
{
	m_DataParser.Reset();
	return CBaseStreamFileReader::Flush();
}
VO_VOID CMpegReader::Flush2()
{
	CMpegTrack* pTrack = NULL;
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed())
		{
			pTrack = (CMpegTrack *)m_ppTracks[i];
			pTrack->Flush();
		}
	}
}
VO_U32 CMpegReader::OnDrmCallBack(VO_PTR pParam)
{
	PMpegDataDrmInfo pDrmInfo = (PMpegDataDrmInfo)pParam;
	return DRMInfo_Widevine(pDrmInfo->pDrmData,pDrmInfo->btDrmDataSize); 
}

CMpegTrack* CMpegReader::GetTrackByIndex(VO_U32 nIndex)
{
	if ((nIndex >= 0 && nIndex < m_nTracks) && m_ppTracks[nIndex]->IsInUsed())
	{
		return (CMpegTrack*)m_ppTracks[nIndex];
	}
	return NULL;
}

VO_BOOL CMpegReader::IsCanGenerateIndex()
{
	if(m_DataParser.IsParseEnd())
		return VO_FALSE;

	if(IsTrackBufferFull() || IsTrackBufferHasEnoughFrame())
		return VO_FALSE;

	return VO_TRUE;
}
VO_BOOL CMpegReader::IsTrackBufferHasEnoughFrame()
{
	///<firstly we just generate 1 second frame for video and audio to caculate the framerate
	VO_U32 nFullBufTrack = 0,nUsedTrackCnt = 2;
	CMpegTrack* pTrack = NULL;
	
	nUsedTrackCnt = GetUsedTrackCnt();
	for(VO_U32 i = 0; i < m_uInitTrackCnt; i++)
	{
		if (!m_ppTracks[i])
		{
			continue;
		}
		pTrack = (CMpegTrack*)m_ppTracks[i];
		if(pTrack && pTrack->IsInUsed() && pTrack->GetTrackBufDuration() > m_nCurBufTime)
		{
		//	VOLOGI("Track %d :Duration:%d",i,pTrack->GetTrackBufDuration());
			nFullBufTrack++;
		}
	}
	if (nFullBufTrack < nUsedTrackCnt)
	{
		return VO_FALSE;
	}
	return VO_TRUE;

}

VO_U32 CMpegReader::GetDRMType(VO_U32 * pDrmType)
{
	*pDrmType = 0;
	if (m_DataParser.IsDrmReady())
	{
		*pDrmType = 1;
	}
	return VO_ERR_NONE;
}

VO_U32 CMpegReader::GetUsedTrackCnt()
{
	VO_U32 nUsed = m_uInitTrackCnt;
	if (!m_bOpenEnd)
	{
		return nUsed;
	}
	CMpegTrack * pTrack = NULL;
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if (!m_ppTracks[i])
		{
			nUsed--;
			continue;
		}
		pTrack = (CMpegTrack*)m_ppTracks[i];
		if(pTrack && !pTrack->IsInUsed())
		{
			nUsed--;
		}
	}
	return nUsed;
}

///<look for a timestamp little more than or equal to llTs
VO_S64 CMpegReader::GetFilePosByTime(VO_S64 llTs,VO_S64 *llRealTs,VO_S64 dwStreamID,MPEGPESInfo* pMinPESInfo,MPEGPESInfo* pMaxPESInfo)
{
	VOLOGE("GetFilePosByTime:llTs==%lld:%d",llTs,voOS_GetSysTime());
	if (llTs <= 0 || llTs > GetDuration())
	{
		return 0;
	}

	VO_U32 nDataType = ParsedDataType_TimeStamp;
	m_DataParser.SetOnDataCallback(this, &nDataType, GOnTimeStamp);
	
	///<first find a pos where its timestamp is smaller than llTs
	VO_S64 tmpTs = 0;
	VO_S64 llMaxPos = -1,llMinPos = -1,llDiff = 0;

	VO_U64 nTmpPos = (llTs  * m_ullFileSize) / GetDuration();
	if (pMinPESInfo && pMinPESInfo->dwTimeStamp >= 0 && pMinPESInfo->dwTimeStamp < llTs)///<just look for max pos
	{
		llMinPos = pMinPESInfo->dwPESPos;
		nTmpPos = llMinPos + ((llTs - (pMinPESInfo->dwTimeStamp))* 4 * m_ullFileSize) / GetDuration();
	}
	if (pMaxPESInfo && pMaxPESInfo->dwTimeStamp >= 0 && pMaxPESInfo->dwTimeStamp > llTs)
	{
		llMaxPos = pMaxPESInfo->dwPESPos;
		llDiff = ((pMaxPESInfo->dwTimeStamp - llTs)* 4 * m_ullFileSize) / GetDuration();

		nTmpPos = llMaxPos > llDiff ? llMaxPos - llDiff : 0;
	}
	VOLOGE("GetFilePosByTime:nTmpPos==%lld:%d",nTmpPos,voOS_GetSysTime());

	VO_U32 nCnt = 0;
	VO_BOOL bCan =(VO_BOOL)(llMinPos == -1 || llMaxPos == -1);
	do
	{
		while(bCan) 
		{
			if (nTmpPos == 0)
			{
				llMinPos = nTmpPos;
			}
			VOLOGE("m_DataParser.SetStartFilePos==%lld:%d",nTmpPos,voOS_GetSysTime());
			if(VO_FALSE == m_DataParser.SetStartFilePos(nTmpPos))
			{
				break;
			}
			m_CurPESInfo.Init();
			VO_BOOL ret = VO_TRUE;
			while(ret = m_DataParser.Step())
			{
				if ((m_CurPESInfo.dwStreamID == -1 || m_CurPESInfo.dwStreamID == dwStreamID ) && m_CurPESInfo.dwTimeStamp >= m_dwFirstTimeStamp)
				{
					VOLOGE("m_llTmpTs - m_dwFirstTimeStamp==%lld:%d",m_CurPESInfo.dwTimeStamp - m_dwFirstTimeStamp,voOS_GetSysTime());
					if (m_CurPESInfo.dwTimeStamp - m_dwFirstTimeStamp > llTs)
					{
						if (m_CurPESInfo.dwPESPos >= llMaxPos)
						{
							llMaxPos = nTmpPos;
						}
						else
							llMaxPos = m_CurPESInfo.dwPESPos;

						if (nCnt == 0)
						{
							///<find a pos where its timestamp is smaller than llTs 
							llDiff = ((m_CurPESInfo.dwTimeStamp - m_dwFirstTimeStamp - llTs)* 4 * m_ullFileSize) / GetDuration();
							
							nTmpPos = llMaxPos > llDiff ? llMaxPos - llDiff : 0;
							VOLOGE("nCnt == 0:llDiff>>==%lld,nTmpPos==%lld:%d",llDiff,nTmpPos,voOS_GetSysTime());
						}
						else
						{
							if (llMaxPos -  llMinPos <= 3*64*1024)///<64*3k
							{
								bCan = VO_FALSE;
								break;
							}
							nTmpPos = (llMinPos + llMaxPos)/2;
							VOLOGE("nCnt == 1:llMaxPos==%lld,nTmpPos==%lld:%d",llMaxPos,nTmpPos,voOS_GetSysTime());
						}
						
						break;
					}
					else if (m_CurPESInfo.dwTimeStamp - m_dwFirstTimeStamp < llTs)
					{
						llMinPos = m_CurPESInfo.dwPESPos;
						if (nCnt == 0)
						{
							///<find a pos where its timestamp is greater than llTs 
							nTmpPos = llMinPos + ((llTs - (m_CurPESInfo.dwTimeStamp - m_dwFirstTimeStamp))* 4 * m_ullFileSize) / GetDuration();
							VOLOGE("nCnt == 0:llDiff<<==%lld,nTmpPos==%lld:%d",nTmpPos -llMinPos ,nTmpPos,voOS_GetSysTime());
						}
						else
						{
							if (llMaxPos -  llMinPos <= 3*64*1024)
							{
								bCan = VO_FALSE;
								break;
							}
							nTmpPos = (llMinPos + llMaxPos)/2;
							VOLOGE("nCnt == 1:llMinPos==%lld,nTmpPos==%lld:%d",llMinPos,nTmpPos,voOS_GetSysTime());
						}
						break;
					}
					else
					{
						*llRealTs = (m_CurPESInfo.dwTimeStamp - m_dwFirstTimeStamp);
						m_DataParser.SetOnDataCallback(this, NULL, GOnData);
						return m_CurPESInfo.dwPESPos;
					}
				}
			}
			if (!ret)
			{
				bCan = VO_FALSE;
				break;
			}
			if (nCnt == 0)
			{
				 bCan =(VO_BOOL)(llMinPos == -1 || llMaxPos == -1);
			}
		}
		
	//	VOLOGE("Cnt==2 Begin:llMinPos ==%lld,llMaxPos==%lld,nTmpPos==%lld",llMinPos,llMaxPos,nTmpPos);
		nCnt++;
		bCan = VO_TRUE;
		if (llMinPos == -1)
		{
			llMinPos = 0;
		}
		if (llMaxPos == -1)
		{
			llMaxPos = m_ullFileSize;
		}
		nTmpPos = (llMinPos + llMaxPos)/2;
		VOLOGE("Cnt==2 Begin:llMinPos ==%lld,llMaxPos==%lld,nTmpPos==%lld:%d",llMinPos,llMaxPos,nTmpPos,voOS_GetSysTime());
	}while(nCnt < 2);

	nTmpPos = llMinPos;
	if(VO_TRUE == m_DataParser.SetStartFilePos(nTmpPos))
	{
		m_CurPESInfo.Init();
		while(m_DataParser.Step())
		{
			if ((m_CurPESInfo.dwStreamID == -1 || m_CurPESInfo.dwStreamID == dwStreamID ) && (m_CurPESInfo.dwTimeStamp > m_dwFirstTimeStamp) && m_CurPESInfo.dwTimeStamp - m_dwFirstTimeStamp >= llTs)
			{
				nTmpPos = m_CurPESInfo.dwPESPos;
				break;
			}
		}
	}
	

	*llRealTs = (m_CurPESInfo.dwTimeStamp - m_dwFirstTimeStamp);
	m_DataParser.SetOnDataCallback(this, NULL, GOnData);
	return nTmpPos;
}

VO_U32 CMpegReader::FindAndCreateTrack()
{
	while(IsCanGenerateIndex())
	{
		if(!m_pFileDataParser->Step())
			m_pFileDataParser->SetParseEnd(VO_TRUE);
		if (m_nTracks && m_ppTracks[0]->GetType() == VOTT_VIDEO)
		{
			break;
		}
	}
	return VO_ERR_SOURCE_OK;
}