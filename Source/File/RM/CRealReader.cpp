#include "CRealReader.h"
#include "CRealAudioTrack.h"
#include "CRealVideoTrack.h"
#ifdef _SUPPORT_PARSER_METADATA
#include "CRealMetaData.h"
#endif	//_SUPPORT_PARSER_METADATA

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

static VO_U8 GOnData(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam)
{
	CRealReader* pReader = (CRealReader*)pUser;
	PRealDataParserSample pSample = (PRealDataParserSample)pParam;

	return pReader->OnPacket(pSample->pFileChunk, pSample->btStreamNum, pSample->btFlags, pSample->wPacketLen, pSample->dwTimeStamp);
}

static VO_BOOL GOnBlock(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam)
{
	CRealTrack* pTrack = (CRealTrack*)pUser;
	return pTrack->OnBlock(*(VO_U32*)pParam);
}

#ifdef _VONAMESPACE
}
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CRealReader::CRealReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
	: CBaseStreamFileReader(pFileOp, pMemOp, pLibOP, pDrmCB)
	, m_HeaderParser(&m_chunk, pMemOp)
	, m_DataParser(&m_chunk, pMemOp)
	, m_bAVInterleave(VO_TRUE)
	, m_bOpenForthumbnail(VO_FALSE)
{
	m_pFileDataParser = &m_DataParser;
}

CRealReader::~CRealReader()
{
	Close();
}

VO_U32 CRealReader::init(VO_U32 nSourceOpenFlags, VO_LIB_OPERATOR* pLibOP, VO_U32 uModuleID , VO_TCHAR* pWorkingPath)
{
	VO_U32 rc = CBaseStreamFileReader::init( nSourceOpenFlags , pLibOP , uModuleID , pWorkingPath );
	if(VO_ERR_SOURCE_OK != rc)
	{
		return rc;
	}

	if ( nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL )
	{
		//it is for get thumbnail 
		m_bOpenForthumbnail = VO_TRUE;
	}
	else
	{
		m_bOpenForthumbnail = VO_FALSE;
	}

	//for debug purpose
//	m_bOpenForthumbnail = VO_TRUE;

	return rc;
}

VO_U32 CRealReader::uninit()
{
	VO_U32 rc = CBaseStreamFileReader::uninit();
	if(VO_ERR_SOURCE_OK != rc)
	{
		return rc;
	}

	m_bOpenForthumbnail = VO_FALSE;

	return rc;
}

VO_U32 CRealReader::GetDuration()
{
	VO_U32 dwDuration = CBaseStreamFileReader::GetDuration();
	
	//if we found both audio track and video track duration value is 0, then we will try to return duration of whole file.
	return ( dwDuration > 0 ? dwDuration : m_HeaderParser.GetDuration() );
}

VO_U32 CRealReader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	Close();

	VO_U32 rc = CBaseStreamFileReader::Load(nSourceOpenFlags, pFileSource);
	if(VO_ERR_SOURCE_OK != rc)
	{
		return rc;
	}

	m_HeaderParser.SetFileSize(m_ullFileSize);
	if(!m_HeaderParser.ReadFromFile())
	{
		return VO_ERR_SOURCE_OPENFAIL;
	}

	rc = InitTracks(nSourceOpenFlags);
	m_HeaderParser.ReleaseTmpInfo();
	if(VO_ERR_SOURCE_OK != rc)
	{
		return rc;
	}
	
	if(!(VO_SOURCE_OPENPARAM_FLAG_INFOONLY & nSourceOpenFlags))
	{
		SelectDefaultTracks();
		rc = PrepareTracks();
		if(VO_ERR_SOURCE_OK != rc)
		{
			return rc;
		}
		

		if(m_bAVInterleave)
		{
			//one track or two tracks with same data offset
			VO_U32 dwDataOffset = 0;
			for(VO_U32 i = 0; i < m_nTracks; i++)
			{
				if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed())
				{
					dwDataOffset = ((CRealTrack*)m_ppTracks[i])->m_dwDataOffset;
					break;
				}
			}

			//for thumbnail case , we should set scan count and thumbnail flag to data parser.
			if(m_bOpenForthumbnail)
			{
				if(m_ppTracks[0] && m_ppTracks[0]->IsInUsed() && VOTT_VIDEO == m_ppTracks[0]->GetType())
				{
					VO_S32 thumbscancnt = ((CRealVideoTrack*)m_ppTracks[0])->GetThumbnailScanCount();
					if(thumbscancnt > 0)
					{
						m_DataParser.SetThumbnailScanCount( thumbscancnt );
					}
				}
				m_DataParser.SetThumbnailFlag(VO_TRUE);
			}

			m_DataParser.Init(VO_NULL);			
			
			if(!m_DataParser.SetStartFilePos(dwDataOffset, VO_TRUE))
			{
				return VO_ERR_SOURCE_END;
			}

			m_DataParser.SetOnDataCallback(this, VO_NULL, GOnData);
			FileGenerateIndex();
		}
		else
		{
			//two tracks with different data offset
			VO_U32 nFirstTrackIdx = -1, nSecondTrackIdx = -1;
			VO_U32 dwPrevDataOffset = -1;
			for(VO_U32 i = 0; i < m_nTracks; i++)
			{
				if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed())
				{
					if(dwPrevDataOffset == (VO_U32)-1)
					{
						dwPrevDataOffset = ((CRealTrack*)m_ppTracks[i])->m_dwDataOffset;
						nFirstTrackIdx = i;
					}
					else
					{
						if(((CRealTrack*)m_ppTracks[i])->m_dwDataOffset > dwPrevDataOffset)
							nSecondTrackIdx = i;
						else
						{
							nSecondTrackIdx = nFirstTrackIdx;
							nFirstTrackIdx = i;
						}
					}
				}
			}

			m_DataParser.Init(VO_NULL);
			if(!m_DataParser.SetStartFilePos(((CRealTrack*)m_ppTracks[nFirstTrackIdx])->m_dwDataOffset, VO_TRUE))
				return VO_ERR_SOURCE_END;

			m_DataParser.SetOnDataCallback(this, VO_NULL, GOnData);
			FileGenerateIndex();

			m_chunk.SetShareFileHandle(VO_TRUE);

			m_chunkUseForDataParser.FCreate(m_hFile, ((CRealTrack*)m_ppTracks[nSecondTrackIdx])->m_dwDataOffset);
			m_chunkUseForDataParser.SetShareFileHandle(VO_TRUE);

			CRealDataParser* pNewDataParser = new CRealDataParser(&m_chunkUseForDataParser, m_pMemOp);
			if(!pNewDataParser)
				return VO_ERR_OUTOF_MEMORY;

			pNewDataParser->Init(VO_NULL);
			if(!pNewDataParser->SetStartFilePos(((CRealTrack*)m_ppTracks[nSecondTrackIdx])->m_dwDataOffset, VO_TRUE))
				return VO_ERR_SOURCE_END;

			pNewDataParser->SetOnDataCallback(this, VO_NULL, GOnData);
			((CRealTrack*)m_ppTracks[nSecondTrackIdx])->SetNewDataParser(pNewDataParser);

			for(VO_U32 i = 0; i < m_nTracks; i++)
			{
				if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed())
					((CBaseStreamFileTrack*)m_ppTracks[i])->SetParseForSelf(VO_TRUE);
			}

			((CRealTrack*)m_ppTracks[nSecondTrackIdx])->TrackGenerateIndex();
		}

		//for thumbnail case, we should not check it.
		if( !m_bOpenForthumbnail )
		{
			for(VO_U32 i = 0; i < m_nTracks; i++)
			{
				if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed() && ((CBaseStreamFileTrack*)m_ppTracks[i])->IsIndexNull())	//still no frame, then we should not use it!!
					m_ppTracks[i]->SetInUsed(VO_FALSE);
			}
		}
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CRealReader::Close()
{
	UnprepareTracks();
	UninitTracks();

	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		SAFE_DELETE(m_ppTracks[i]);
	}

	return CBaseStreamFileReader::Close();
}

CRealTrack* CRealReader::NewTrack(PRealTrackInfo pTrackInfo)
{
	if(!pTrackInfo)
		return VO_NULL;

	CRealTrack* pNewTrack = VO_NULL;
	if(RMMT_IS_AUDIO(pTrackInfo->nType))
		pNewTrack = new CRealAudioTrack(pTrackInfo, this, m_pMemOp);
	else
		pNewTrack = new CRealVideoTrack(pTrackInfo, this, m_pMemOp);

	if(!pNewTrack)
		return VO_NULL;

	//set some parameters here!!
	return pNewTrack;
}

#ifdef _SUPPORT_PARSER_METADATA
VO_U32 CRealReader::InitMetaDataParser()
{
	m_pMetaDataParser = new CRealMetaData(m_pFileOp, m_pMemOp);
	if(!m_pMetaDataParser)
		return VO_ERR_OUTOF_MEMORY;

	MetaDataInitInfo initInfo;
	initInfo.eType = VO_METADATA_RM;
	initInfo.ullFilePositions[0] = m_HeaderParser.GetContentDescriptionHeaderFilePos();
	initInfo.ullFilePositions[1] = m_HeaderParser.GetMetadataSectionFilePos();
	initInfo.ullFileSize = m_ullFileSize;
	initInfo.dwReserved = 0;
	initInfo.hFile = m_hFile;

	return m_pMetaDataParser->Load(&initInfo);
}
#endif	//_SUPPORT_PARSER_METADATA

VO_U32 CRealReader::InitTracks(VO_U32 nSourceOpenFlags)
{
	VO_U32 rc = VO_ERR_SOURCE_OK;
	if( m_bOpenForthumbnail )
	{
		if( m_HeaderParser.GetTrackInfoCount() > 0 )
		{
			//just enable video track,disable audio track and other tracks.this aviod I/O speration.
			rc = TracksCreate( 1 );
		}
		else
		{
			rc = VO_ERR_SOURCE_FORMATUNSUPPORT;
		}
	}
	else
	{
		rc = TracksCreate(m_HeaderParser.GetTrackInfoCount());
	}
	
	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	VO_U32 dwExtSize = ( m_HeaderParser.GetMaxPacketSize() > 0 ) ? m_HeaderParser.GetMaxPacketSize() : 0x10000;
	PRealTrackInfo pti = m_HeaderParser.GetTrackInfoPtr();
	VO_U32 i = 0;
	while(pti)
	{
		if(!pti->dwDataOffset)
		{
			//it is impossible for real media file that data offset equal to 0.
			pti->dwDataOffset = (VO_U32)m_HeaderParser.GetMediaDataFilePos();
		}

		if( m_bOpenForthumbnail  )
		{
			if( pti->nType == RMMT_VIDEO /*or RMMT_LOGICAL_VIDEO_SUB ???*/ )
			{
				CRealTrack* pNewTrack = NewTrack(pti);
				if(!pNewTrack)
				{
					return VO_ERR_OUTOF_MEMORY;
				}

				pNewTrack->SetBufferTime(1);
				pNewTrack->SetGlobalBufferExtSize(dwExtSize);
				pNewTrack->SetThumbnailFlag(VO_TRUE);

				m_ppTracks[0] = pNewTrack;

				break; //while (pti)
			}
		}
		else
		{
			CRealTrack* pNewTrack = NewTrack(pti);
			if(!pNewTrack)
			{
				return VO_ERR_OUTOF_MEMORY;
			}

			pNewTrack->SetBufferTime(10);
			pNewTrack->SetGlobalBufferExtSize(dwExtSize);
			pNewTrack->SetThumbnailFlag(VO_FALSE);

			m_ppTracks[i] = pNewTrack;

			i++;
		}

		pti = pti->next;
	}

	if( m_bOpenForthumbnail )
	{
		m_nTracks = 1;
	}
	else
	{
		m_nTracks = i;
	}

	return CBaseStreamFileReader::InitTracks(nSourceOpenFlags);
}

VO_U32 CRealReader::PrepareTracks()
{
	m_bAVInterleave = VO_TRUE;
	VO_U32 dwPrevDataOffset = -1;
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed())
		{
			PRealFileIndexInfo pIndexInfo = m_HeaderParser.GetIndexInfoPtr();
			while(pIndexInfo)
			{
				if(pIndexInfo->stream_number == m_ppTracks[i]->GetStreamNum())
					break;

				pIndexInfo = pIndexInfo->next;
			}

			//read index records from file for every selected tracks.
			//but if one track has several index section , then it may just read one index section's records.
			//!!!!!!!!!!!!!!
			if( !m_bOpenForthumbnail )
			{
				//since RM index did not contain info of whether the frame is key frame or not, so we should avoid read index when get thumbnail
				if(!((CRealTrack*)m_ppTracks[i])->ReadIndexFromFile(&m_chunk, pIndexInfo))
				{
					return VO_ERR_SOURCE_OPENFAIL;
				}
			}
			
			if(dwPrevDataOffset == (VO_U32)-1)
			{
				dwPrevDataOffset = ((CRealTrack*)m_ppTracks[i])->m_dwDataOffset;
			}
			else
			{
				//we should not check av interleave or not when get thumbnail, it is meaningless.
				if( !m_bOpenForthumbnail )
				{
					//if audio and video data offset is not same, then a/v is not interleave
					//but if it has other tracks, and the dataoffset is not same with a/v, it may cause problem.
					if(dwPrevDataOffset != ((CRealTrack*)m_ppTracks[i])->m_dwDataOffset)
					{
						m_bAVInterleave = VO_FALSE;
					}
				}
			}
		}
	}

	return CBaseStreamFileReader::PrepareTracks();
}

VO_U8 CRealReader::OnPacket(CGFileChunk* pFileChunk, VO_U8 btStreamNum, VO_U8 btFlags, VO_U16 wLen, VO_U32 dwTimeStamp)
{
	CRealTrack *pTrack = (CRealTrack*)GetTrackByStreamNum(btStreamNum);
	if(pTrack)
	{
		if(!pTrack->OnPacket(pFileChunk, btFlags, wLen, dwTimeStamp))
		{
			return CBRT_FALSE;
		}

		return CBRT_CONTINUABLENOTNEEDSKIP;
	}
	else
	{
		return CBRT_CONTINUABLEANDNEEDSKIP;
	}
}

VO_U32 CRealReader::MoveTo(VO_S64 llTimeStamp)
{
	if(m_bAVInterleave)
	{
		Flush();

		//get front position of two tracks
		VO_U32 dwFilePos = VO_MAXU32;
		for(VO_U32 i = 0; i < m_nTracks; i++)
		{
			if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed())
			{
				VO_U32 dwTmpFilePos = ((CRealTrack*)m_ppTracks[i])->FileIndexGetFilePosByTime(llTimeStamp);
				if((VO_U32)-1 == dwTmpFilePos)
					dwTmpFilePos = (VO_U32)m_HeaderParser.GetMediaDataFilePos();

				if(dwTmpFilePos < dwFilePos)
					dwFilePos = dwTmpFilePos;
			}
		}

		if(!m_DataParser.SetStartFilePos(dwFilePos, (dwFilePos == m_HeaderParser.GetMediaDataFilePos()) ? VO_TRUE : VO_FALSE))
			return VO_ERR_SOURCE_END;

		FileGenerateIndex();
	}

	return CBaseStreamFileReader::MoveTo(llTimeStamp);
}

VO_U32 CRealReader::OnTrackSetPosN(CRealTrack* pTrack, VO_S64 llTimeStamp)
{
	if(!m_bAVInterleave)
	{
		pTrack->Flush();

		//two tracks with different data offset
		CRealTrack* pOtherTrack = (CRealTrack*)GetOtherTrackInUsed(pTrack);
		if(pOtherTrack && pTrack->m_dwDataOffset < pOtherTrack->m_dwDataOffset)	//front track
		{
			m_DataParser.SetParseEnd(VO_FALSE);
			m_DataParser.SetBlockStream(0xFF);
			m_DataParser.SetOnBlockCallback(VO_NULL, VO_NULL, VO_NULL);

			m_chunk.SetShareFileHandle(VO_TRUE);
			VO_U32 dwTmpFilePos = pTrack->FileIndexGetFilePosByTime(llTimeStamp);
			if((VO_U32)-1 == dwTmpFilePos)
				dwTmpFilePos = (VO_U32)m_HeaderParser.GetMediaDataFilePos();

			if(!m_DataParser.SetStartFilePos(dwTmpFilePos, (dwTmpFilePos == m_HeaderParser.GetMediaDataFilePos()) ? VO_TRUE : VO_FALSE))
				return VO_ERR_SOURCE_END;
		}
		else
		{
			m_chunkUseForDataParser.SetShareFileHandle(VO_TRUE);
			CRealDataParser* pNewDataParser = new CRealDataParser(&m_chunkUseForDataParser, m_pMemOp);
			if(!pNewDataParser)
				return VO_ERR_OUTOF_MEMORY;

			pNewDataParser->Init(VO_NULL);

			VO_U32 dwTmpFilePos = pTrack->FileIndexGetFilePosByTime(llTimeStamp);
			if((VO_U32)-1 == dwTmpFilePos)
				dwTmpFilePos = (VO_U32)m_HeaderParser.GetMediaDataFilePos();

			if(!pNewDataParser->SetStartFilePos(dwTmpFilePos, (dwTmpFilePos == m_HeaderParser.GetMediaDataFilePos()) ? VO_TRUE : VO_FALSE))
				return VO_ERR_SOURCE_END;

			pNewDataParser->SetOnDataCallback(this, VO_NULL, GOnData);
			pTrack->SetNewDataParser(pNewDataParser);
		}

		pTrack->SetParseForSelf(VO_TRUE);
		pTrack->TrackGenerateIndex();
	}

	return VO_ERR_SOURCE_OK;
}

VO_VOID CRealReader::OnNewDataParser(CBaseTrack* pRqstTrack, CvoFileDataParser** ppDataParser)
{
	if(!m_bAVInterleave)
		return;

	CBaseStreamFileReader::OnNewDataParser(pRqstTrack, ppDataParser);

	CRealDataParser* pNewDataParser = new CRealDataParser(&m_chunkUseForDataParser, m_pMemOp);
	if(!pNewDataParser)
		return;

	pNewDataParser->Init(VO_NULL);
	pNewDataParser->SetOnDataCallback(this, VO_NULL, GOnData);
	pNewDataParser->SetBlockStream(GetOtherTrackInUsed(pRqstTrack)->GetStreamNum());

	m_DataParser.SetBlockStream(pRqstTrack->GetStreamNum());
	m_DataParser.SetOnBlockCallback(pRqstTrack, VO_NULL, GOnBlock);

	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed())
			((CBaseStreamFileTrack*)m_ppTracks[i])->SetParseForSelf(VO_TRUE);
	}

	*ppDataParser = pNewDataParser;
}
