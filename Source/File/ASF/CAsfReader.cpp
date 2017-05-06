#include "CAsfReader.h"
#include "CAsfAudioTrack.h"
#include "CAsfVideoTrack.h"
#include "CASFMetaData.h"
#include "voWMDRM.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define VO_DEFAULT_IMAGE_AREA		320 * 240


static VO_U8 GOnData(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam)
{
	CAsfReader* pReader = (CAsfReader*)pUser;
	PAsfDataParserSample pSample = (PAsfDataParserSample)pParam;
	if(pReader->IsThumbnail())
	{
		if(pSample && (pSample->btStreamNum & 0x80))
		{
			return pReader->OnPayloadPlay(pSample->pFileChunk, pSample->btStreamNum, pSample->dwPosInMediaObj, pSample->dwLen, pSample->dwMediaObjSize, pSample->dwTimeStamp);
		}
		else
			return CBRT_CONTINUABLEANDNEEDSKIP;
	}
	else
		return pReader->OnPayloadPlay(pSample->pFileChunk, pSample->btStreamNum, pSample->dwPosInMediaObj, pSample->dwLen, pSample->dwMediaObjSize, pSample->dwTimeStamp);
}
static VO_U8 GOnDataInSeek(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam)
{
	PAsfDataParserSample pSample = (PAsfDataParserSample)pParam;
	if(pSample->dwPosInMediaObj)		//middle frame
		return CBRT_CONTINUABLEANDNEEDSKIP;			//continuable and need skip

	CAsfReader* pReader = (CAsfReader*)pUser;
	return pReader->OnNewFrameSeek(pSample->btStreamNum, pSample->dwTimeStamp, *((VO_U32*)pUserData)) ? CBRT_CONTINUABLEANDNEEDSKIP : CBRT_FALSE;
}

static VO_BOOL GOnBlock(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam)
{
	CAsfTrack* pTrack = (CAsfTrack*)pUser;
	return pTrack->OnBlock(*(VO_U32*)pParam);
}


static VO_U8 GOnDataThumbnail(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam)
{
	CAsfReader* pReader = (CAsfReader*)pUser;
	PAsfDataParserSample pSample = (PAsfDataParserSample)pParam;
	return pReader->OnPayloadThumbnail(pSample->pFileChunk, pSample->btStreamNum, pSample->dwPosInMediaObj, pSample->dwLen, pSample->dwMediaObjSize, pSample->dwTimeStamp);
}
#if 0
static VO_U8 GOnKeyFrameData(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam)
{
	CAsfReader* pReader = (CAsfReader*)pUser;
	PAsfDataParserSample pSample = (PAsfDataParserSample)pParam;
	if(pSample && (pSample->btStreamNum & 0x80))
	{
		return pReader->OnPayloadPlay(pSample->pFileChunk, pSample->btStreamNum, pSample->dwPosInMediaObj, pSample->dwLen, pSample->dwMediaObjSize, pSample->dwTimeStamp);
	}
	return CBRT_CONTINUABLEANDNEEDSKIP;
}
#endif
CAsfReader::CAsfReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
	: CBaseStreamFileReader(pFileOp, pMemOp, pLibOP, pDrmCB)
	, m_HeaderParser(&m_chunk, pMemOp)
	, m_DataParser(&m_chunk, pMemOp)
	, m_IndexParser(&m_chunk, pMemOp)
	, m_eDRMType(VO_DRMTYPE_UNKNOWN)
	, m_pTmpPayload(VO_NULL)
	, m_ForStreaming(VO_FALSE)
	,m_bThumbnail(VO_FALSE)
{
	m_pFileDataParser = &m_DataParser;
}

CAsfReader::~CAsfReader()
{
	Close();
}

VO_U32 CAsfReader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	/*nSourceOpenFlags |= VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL;*/ 
	// the flag is  for thunmail 
	if (nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_OPENPD)
		m_ForStreaming = VO_TRUE;

	Close();

	//open file
	pFileSource->nMode = VO_FILE_READ_ONLY;
	m_hFile = FileOpen(pFileSource);
	if(!m_hFile)
		return VO_ERR_SOURCE_OPENFAIL;

	m_ullFileSize = FileSize(m_hFile);

	if ((VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL & nSourceOpenFlags))
	{
		if(!m_chunk.FCreate(m_hFile, 0, 0x80))
			return VO_ERR_SOURCE_OPENFAIL;
		m_bThumbnail = VO_TRUE;
	}
	else
	{
		if(!m_chunk.FCreate(m_hFile, 0, VO_FILE_CHUNK_SIZE))
			return VO_ERR_SOURCE_OPENFAIL;
	}

	if(!m_HeaderParser.ReadFromFile())
		return VO_ERR_SOURCE_OPENFAIL;

	if(m_HeaderParser.GetDuration() <= 0)
		return VO_ERR_SOURCE_OPENFAIL;

	if (VO_MAXU64 == m_ullFileSize)
		m_ullFileSize = m_HeaderParser.GetFileRealSize();

	VO_PBYTE pContentEncryptionObject;
	VO_U32 nContentEncryptionObject;
	VO_PBYTE pExtendedContentEncryptionObject;
	VO_U32 nExtendedContentEncryptionObject;

	if(m_HeaderParser.GetContentEncryptObjects(&pContentEncryptionObject, &nContentEncryptionObject, &pExtendedContentEncryptionObject, &nExtendedContentEncryptionObject))
	{
		VO_WMDRM_INFO infoWMDRM = {0};
		infoWMDRM.pContentEncryptionObject			= pContentEncryptionObject;
		infoWMDRM.nContentEncryptionObject			= nContentEncryptionObject;
		infoWMDRM.pExtendedContentEncryptionObject	= pExtendedContentEncryptionObject;
		infoWMDRM.nExtendedContentEncryptionObject	= nExtendedContentEncryptionObject;

		if(DRMInfo(VO_DRMTYPE_WindowsMedia, &infoWMDRM) != VO_ERR_DRM_OK)
			return VO_ERR_SOURCE_CONTENTENCRYPT;

		m_eDRMType = VO_DRMTYPE_WindowsMedia;
	}
	else if ( m_HeaderParser.GetProtectionSystemIdentifierObject(&pContentEncryptionObject, &nContentEncryptionObject) )
	{
		VO_DRM_INFO infoDRM = {0};
		infoDRM.pDRMHeader = pContentEncryptionObject;
		infoDRM.nDRMHeader = nContentEncryptionObject;

		if(DRMInfo(VO_DRMTYPE_PlayReady, &infoDRM) != VO_ERR_DRM_OK)
			return VO_ERR_SOURCE_CONTENTENCRYPT;
		//2
		IVInfo *pIVInfo = VO_NULL;
		if ( m_HeaderParser.GetPayloadExtensionSystemEncryptionSampleID(&pIVInfo) )
			m_DataParser.SetIVInfos(pIVInfo);

		m_eDRMType = VO_DRMTYPE_PlayReady;
	}

	VO_U32 rc = InitTracks(nSourceOpenFlags);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	if(VO_SOURCE_OPENPARAM_FLAG_INFOONLY & nSourceOpenFlags)
		return VO_ERR_SOURCE_OK;

	m_IndexParser.ReadFromFile( m_HeaderParser.GetIndexObjectsFilePos(), m_HeaderParser.GetPacketSize() );
	/*add by danny * first set all the track used. because sometimes the default track don't have package data */
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		m_ppTracks[i]->SetInUsed(VO_TRUE);
 
	}
	/*end*/
	//SelectDefaultTracks();
	//m_HeaderParser.ReleaseTmpInfo();
	PrepareTracks();
	AsfDataParserInitParam initParam(m_HeaderParser.GetPacketSize(), m_HeaderParser.GetTimeOffset());
	m_DataParser.Init(&initParam);
	m_DataParser.SetOnDataCallback(this, VO_NULL, GOnData);

	if ( (VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL & nSourceOpenFlags) &&
		m_IndexParser.IsValid())
		return VO_ERR_SOURCE_OK;

	if(!m_DataParser.SetStartFilePos(m_HeaderParser.GetMediaDataFilePos()))
		return VO_ERR_SOURCE_END;
	
	FileGenerateIndex();
		/*reselect the track by danny*/
	SelectDefaultTracks();
    m_HeaderParser.ReleaseTmpInfo();
		/*end reselect*/

	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed() && ((CBaseStreamFileTrack*)m_ppTracks[i])->IsIndexNull())	//still no frame, then we should not use it!!
			m_ppTracks[i]->SetInUsed(VO_FALSE);
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAsfReader::Close()
{
	UnprepareTracks();
	UninitTracks();

	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		SAFE_DELETE(m_ppTracks[i]);
	}

	SAFE_MEM_FREE(m_pTmpPayload);

	return CBaseStreamFileReader::Close();
}

CAsfTrack* CAsfReader::NewTrack(PAsfTrackInfo pTrackInfo)
{
	if(!pTrackInfo)
		return VO_NULL;

	CAsfTrack* pNewTrack = VO_NULL;
	if(pTrackInfo->bVideo)
		pNewTrack = new CAsfVideoTrack(pTrackInfo->btStreamNum, m_HeaderParser.GetDuration(), this, m_pMemOp);
	else
		pNewTrack = new CAsfAudioTrack(pTrackInfo->btStreamNum, m_HeaderParser.GetDuration(), this, m_pMemOp);

	if(!pNewTrack)
		return VO_NULL;

	pNewTrack->m_dwPropBufferSize = pTrackInfo->dwPropBufferSize;
	pNewTrack->m_pPropBuffer = NEW_BUFFER(pNewTrack->m_dwPropBufferSize);
	if(!pNewTrack->m_pPropBuffer)
	{
		delete pNewTrack;
		return VO_NULL;
	}

	memcpy(pNewTrack->m_pPropBuffer, pTrackInfo->pPropBuffer, pTrackInfo->dwPropBufferSize);
	pNewTrack->m_dwDuration = m_HeaderParser.GetDuration();
	pNewTrack->m_dwBitrate = m_HeaderParser.GetBitrate(pNewTrack->m_btStreamNum);
	PAsfStreamExtInfo psei = m_HeaderParser.GetTrackExtInfo(pNewTrack->m_btStreamNum);
	if(psei)
	{
		pNewTrack->m_dwBitrate = psei->dwBitrate;
		pNewTrack->m_dwFrameRate = psei->dwFrameRate;
		pNewTrack->m_lMaxSampleSize = psei->dwMaxSampleSize;
	}

	if(!pNewTrack->m_dwBitrate)
	{
		if(VOTT_AUDIO == pNewTrack->GetType())
			pNewTrack->m_dwBitrate = ((VO_WAVEFORMATEX*)(pNewTrack->m_pPropBuffer))->nAvgBytesPerSec;
	}
	return pNewTrack;
}
VO_BOOL CAsfReader::GetThumbNailBuffer(VO_PBYTE* ppBuffer, VO_U32 nSize, VO_U32 nPos)
{
	return ((CAsfDataParser *)m_pFileDataParser)->GetThumbNailBuffer(ppBuffer, nSize, nPos);
}
VO_U32 CAsfReader::InitTracks(VO_U32 nSourceOpenFlags)
{
	VO_U32 uTrackCount = m_HeaderParser.GetTrackInfoCount();
	VO_U32 rc = TracksCreate(uTrackCount);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	VO_U32 dwExtSize = m_HeaderParser.GetPacketSize() ? m_HeaderParser.GetPacketSize() : 0x10000;
	VO_U32 current_bitrate = 0;

	PAsfTrackInfo pti = m_HeaderParser.GetTrackInfoPtr();
	VO_U32 i = 0;
	while(pti)
	{
		if(pti->bVideo == VO_FALSE&&m_bThumbnail)
		{
			pti = pti->next;
			continue;
		}
		CAsfTrack* pNewTrack = NewTrack(pti);
		if(!pNewTrack)
			return VO_ERR_OUTOF_MEMORY;

		pNewTrack->SetBufferTime(30);
		pNewTrack->SetGlobalBufferExtSize(dwExtSize);

		m_ppTracks[i] = pNewTrack;
		current_bitrate+=pNewTrack->m_dwBitrate;
		i++;

		pti = pti->next;
	}

	m_nTracks = i;
	/*add for track*/
	VO_U32 sum_bitrate = m_HeaderParser.GetMaxBitrate();
	PAsfTrackInfo pti_check= m_HeaderParser.GetTrackInfoPtr();
	VO_U32 j = 0;
	while(pti_check && j < m_nTracks)
	{
			VO_U32 bitrate;
			VO_U32 ret = ((CAsfTrack * )m_ppTracks[j])->GetBitrate(&bitrate);
			if(bitrate==0)
			((CAsfTrack * )m_ppTracks[j])->m_dwBitrate =(sum_bitrate-current_bitrate);
			VOLOGW("InitTracks bitrate %d",((CAsfTrack * )m_ppTracks[j])->m_dwBitrate );
			pti_check = pti_check->next;
			j++;

	}
	/*end*/
	return CBaseStreamFileReader::InitTracks(nSourceOpenFlags);
}

VO_U8 CAsfReader::OnPayloadPlay(CGFileChunk* pFileChunk, VO_U8 btStreamNum, VO_U32 dwPosInMediaObj, VO_U32 dwLen, VO_U32 dwMediaObjSize, VO_U32 dwTimeStamp)
{
	CAsfTrack *pTrack = (CAsfTrack*)GetTrackByStreamNum(btStreamNum & 0x7F);
	if(pTrack)
	{
		if(!pTrack->AddPayloadInfo(pFileChunk, (btStreamNum & 0x80) ? VO_TRUE : VO_FALSE, dwPosInMediaObj, dwLen, dwMediaObjSize, dwTimeStamp, m_DataParser.GetIVDataByStreamNum(btStreamNum & 0x7F)))
			return CBRT_FALSE;

		return CBRT_CONTINUABLENOTNEEDSKIP;
	}
	else
		return CBRT_CONTINUABLEANDNEEDSKIP;
}

VO_U8 CAsfReader::OnPayloadThumbnail(CGFileChunk* pFileChunk, VO_U8 btStreamNum, VO_U32 dwPosInMediaObj, VO_U32 dwLen, VO_U32 dwMediaObjSize, VO_U32 dwTimeStamp)
{
	if ( (btStreamNum & 0x80) && dwPosInMediaObj == 0 )
	{
		CAsfTrack *pTrack = (CAsfTrack*)GetTrackByStreamNum(btStreamNum & 0x7F);
		if(pTrack)
		{
			pTrack->m_dwSizeCurrentKeyFrame = dwMediaObjSize;

			return CBRT_FALSE;
		}
	}

	return CBRT_CONTINUABLEANDNEEDSKIP;
}

VO_BOOL CAsfReader::OnNewFrameSeek(VO_U8 btStreamNum, VO_U32 dwTimeStamp, VO_U32 dwSeekTimeStamp)
{
	CAsfTrack *pTrack = (CAsfTrack*)GetTrackByStreamNum(btStreamNum & 0x7F);
	if(pTrack)
	{
		if(dwTimeStamp > dwSeekTimeStamp)
			return VO_FALSE;

		if ( (VOTT_VIDEO != pTrack->GetType() && VO_MAXU32 == pTrack->m_dwTimeStampInSeek) 
			|| (btStreamNum & 0x80) )
			pTrack->m_dwTimeStampInSeek = dwTimeStamp;
	}

	return VO_TRUE;
}

VO_U32 CAsfReader::SetPlayMode(VO_SOURCE_PLAYMODE PlayMode)
{
	if((VO_SOURCE_PM_PLAY != PlayMode) && (!m_HeaderParser.GetPacketSize() || !m_IndexParser.IsValid()))
		return VO_ERR_SOURCE_PLAYMODEUNSUPPORT;

	return CBaseStreamFileReader::SetPlayMode(PlayMode);
}

VO_U32 CAsfReader::MoveTo(VO_S64 llTimeStamp)
{
	Flush();

	VO_U32 dwBlockSize = 0;
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed())
		{
			if(((CAsfTrack*)m_ppTracks[i])->m_dwBitrate)
				dwBlockSize += ((CAsfTrack*)m_ppTracks[i])->m_dwBitrate;
			else
				dwBlockSize += 10 * ((CAsfTrack*)m_ppTracks[i])->m_lMaxSampleSize;
		}
	}

	if(m_HeaderParser.GetPacketSize())
		dwBlockSize = ((dwBlockSize + m_HeaderParser.GetPacketSize() - 1) / m_HeaderParser.GetPacketSize()) * m_HeaderParser.GetPacketSize();

	VO_U64 ullMediaDataFilePos = m_HeaderParser.GetMediaDataFilePos();
	VO_U64 ullCurrMediaDataFilePos = ullMediaDataFilePos;
	if ( llTimeStamp > 1000 && m_HeaderParser.GetPacketSize() )
	{
		VO_U64 qwGetFilePos = 0, qwFilePos = ullMediaDataFilePos;

		//get initialized file position
		if(m_IndexParser.IsValid())
		{
			VO_S64 llPacketPos = m_IndexParser.GetPacketPosByTime(m_HeaderParser.GetTimeOffset() + llTimeStamp);
			if (-1 != llPacketPos)
				qwFilePos = ullMediaDataFilePos + llPacketPos;			
		}

		if(ullMediaDataFilePos == qwFilePos)
			qwFilePos = ullMediaDataFilePos + (((m_ullFileSize - ullMediaDataFilePos) * llTimeStamp / m_HeaderParser.GetDuration()) / m_HeaderParser.GetPacketSize()) * m_HeaderParser.GetPacketSize();

		//try to get accurate file position by parsing from initialized file position
		qwGetFilePos = GetFilePosByTime( qwFilePos, static_cast<VO_U32>(llTimeStamp) );
		while(VO_MAXU64 == qwGetFilePos || 0xFFFFFFFFFFFFFFFELL == qwGetFilePos)
		{
			qwFilePos = (qwFilePos > dwBlockSize) ? qwFilePos - dwBlockSize : 0;
			qwGetFilePos = GetFilePosByTime( qwFilePos, static_cast<VO_U32>(llTimeStamp) );
		}

		ullCurrMediaDataFilePos = qwGetFilePos;
	}

	if(!m_DataParser.SetStartFilePos(ullCurrMediaDataFilePos))
		return VO_ERR_SOURCE_END;

	FileGenerateIndex();

	return CBaseStreamFileReader::MoveTo(llTimeStamp);
}

VO_VOID CAsfReader::ResetTimeStampInSeek()
{
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed())
		{
			((CAsfTrack*)m_ppTracks[i])->m_dwTimeStampInSeek = VO_MAXU32;
			((CAsfTrack*)m_ppTracks[i])->m_dwPrevTimeStampInSeek = VO_MAXU32;
		}
	}
}

VO_VOID CAsfReader::RecordTimeStampInSeek()
{
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed())
			((CAsfTrack*)m_ppTracks[i])->m_dwPrevTimeStampInSeek = ((CAsfTrack*)m_ppTracks[i])->m_dwTimeStampInSeek;
	}
}

VO_BOOL CAsfReader::TimeStampInSeekValid()
{
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed() && (VO_MAXU32 == ((CAsfTrack*)m_ppTracks[i])->m_dwTimeStampInSeek))
			return VO_FALSE;
	}

	VO_U32 dwTimeSeekedA = VO_MAXU32, dwTimeSeekedV = VO_MAXU32;

	for (VO_U32 i = 0; i < (2 > m_nTracks ? m_nTracks : 2); i++)
	{
		if (m_ppTracks[i]->GetType() == VOTT_VIDEO)
			dwTimeSeekedV = ((CAsfTrack*)m_ppTracks[i])->m_dwTimeStampInSeek;
		else
			dwTimeSeekedA = ((CAsfTrack*)m_ppTracks[i])->m_dwTimeStampInSeek;
	}

	if (dwTimeSeekedV!= VO_MAXU32 &&dwTimeSeekedA!=VO_MAXU32&& dwTimeSeekedA > dwTimeSeekedV + 500)
		return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CAsfReader::TimeStampInSeekUpdate()
{
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed() && (((CAsfTrack*)m_ppTracks[i])->m_dwPrevTimeStampInSeek == ((CAsfTrack*)m_ppTracks[i])->m_dwTimeStampInSeek))
			return VO_FALSE;
	}

	return VO_TRUE;
}

VO_U64 CAsfReader::GetFilePosByTime(VO_U64 ullStartFilePos, VO_U32 dwTimeStamp)
{
	if(ullStartFilePos <= m_HeaderParser.GetMediaDataFilePos())
		return m_HeaderParser.GetMediaDataFilePos();

	CAsfDataParser DataParser(&m_chunk, m_pMemOp);
	AsfDataParserInitParam initParam(m_HeaderParser.GetPacketSize(), m_HeaderParser.GetTimeOffset());
	DataParser.Init(&initParam);
	if(!DataParser.SetStartFilePos(ullStartFilePos))
		return VO_MAXU64;

	DataParser.SetOnDataCallback(this, &dwTimeStamp, GOnDataInSeek);

	ResetTimeStampInSeek();

	VO_BOOL bRet = VO_TRUE;
	while(bRet)
	{
		RecordTimeStampInSeek();

		bRet = DataParser.Step();

		if(TimeStampInSeekValid() && TimeStampInSeekUpdate())
			ullStartFilePos = m_chunk.FGetRecord();
	}

	if(!TimeStampInSeekValid())
		return VO_MAXU64;

	return ullStartFilePos;
}

VO_U64 CAsfReader::GetSizeofNearKeyFrame(VO_U64 ullStartFilePos)
{
	CAsfDataParser DataParser(&m_chunk, m_pMemOp);
	AsfDataParserInitParam initParam(m_HeaderParser.GetPacketSize(), m_HeaderParser.GetTimeOffset());
	DataParser.Init(&initParam);
	if(!DataParser.SetStartFilePos(ullStartFilePos))
		return VO_MAXU64;

	DataParser.SetOnDataCallback(this, NULL, GOnDataThumbnail);

	VO_U32 dwIDVideoTrackUsed = 0;
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		//if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed() && m_ppTracks[i]->GetType() == VO_SOURCE_TT_VIDEO)
		if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed() && m_ppTracks[i]->GetType() == VOTT_VIDEO)
		{
			dwIDVideoTrackUsed = i;
			break;
		}
	}

	((CAsfTrack*)m_ppTracks[dwIDVideoTrackUsed])->m_dwSizeCurrentKeyFrame = 0;

	VO_BOOL bRet = VO_TRUE;
	while(bRet)
	{
		bRet = DataParser.Step();

		if (((CAsfTrack*)m_ppTracks[dwIDVideoTrackUsed])->m_dwSizeCurrentKeyFrame)
			break;
	}

	return ((CAsfTrack*)m_ppTracks[dwIDVideoTrackUsed])->m_dwSizeCurrentKeyFrame;
}

VO_U64 CAsfReader::FileIndexGetFilePosByIndex(VO_U32 dwFileIndex)
{
	if(m_IndexParser.IsValid())
	{
		VO_U64 dwPacketPos = m_IndexParser.GetPacketPosByIndex(dwFileIndex);
		if(VO_MAXU64 == dwPacketPos)
			return VO_MAXU64;

		return m_HeaderParser.GetMediaDataFilePos() + dwPacketPos;
	}
	else
		return VO_MAXU64;
}

VO_U32 CAsfReader::FileIndexGetIndexByTime(VO_BOOL bForward, VO_S64 llTimeStamp)
{
	if(llTimeStamp == 0)
		return 0;

	return m_IndexParser.IsValid() ? m_IndexParser.GetIndexByTime(bForward, llTimeStamp) : -1;
}

VO_VOID CAsfReader::OnNewDataParser(CBaseTrack* pRqstTrack, CvoFileDataParser** ppDataParser)
{
	VOLOGW("OnNewDataParser");
	CBaseStreamFileReader::OnNewDataParser(pRqstTrack, ppDataParser);

	CAsfDataParser* pNewDataParser = new CAsfDataParser(&m_chunkUseForDataParser, m_pMemOp);
	if(!pNewDataParser)
		return;

	AsfDataParserInitParam initParam(m_HeaderParser.GetPacketSize(), m_HeaderParser.GetTimeOffset());
	pNewDataParser->Init(&initParam);
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

#ifdef _SUPPORT_PARSER_METADATA
VO_U32 CAsfReader::InitMetaDataParser()
{
	m_pMetaDataParser = new CASFMetaData(m_pFileOp, m_pMemOp);
	if(!m_pMetaDataParser)
		return VO_ERR_OUTOF_MEMORY;

	MetaDataInitInfo initInfo;
	initInfo.eType = VO_METADATA_WM;
	initInfo.ullFilePositions[0] = m_HeaderParser.GetContentDescriptionObjectFilePos();
	initInfo.ullFilePositions[1] = m_HeaderParser.GetExtendedContentDescriptionObjectFilePos();
	initInfo.ullFilePositions[2] = m_HeaderParser.GetMetadataObjectFilePos();
	initInfo.ullFilePositions[3] = m_HeaderParser.GetMetadataLibraryObjectFilePos();
	initInfo.ullFileSize = m_ullFileSize;
	initInfo.dwReserved = 0;
	initInfo.hFile = m_hFile;

	return m_pMetaDataParser->Load(&initInfo);
}
#endif	//_SUPPORT_PARSER_METADATA

VO_PBYTE CAsfReader::GetTmpPayload()
{
	if(!m_pTmpPayload)
		m_pTmpPayload = NEW_BUFFER(m_HeaderParser.GetPacketSize() ? m_HeaderParser.GetPacketSize() : 0x10000);

	return m_pTmpPayload;
}



VO_U32 CAsfReader::GetNearKeyFrame(VO_S64 llTime, VO_S64* pllPreviousKeyframeTime, VO_S64* pllNextKeyframeTime)
{
	if(!m_IndexParser.IsValid())
		return VO_ERR_NOT_IMPLEMENT;

	if(llTime < 0)
		return VO_ERR_INVALID_ARG;

	*pllPreviousKeyframeTime = m_IndexParser.GetPrevKeyFrameTime(llTime + m_HeaderParser.GetTimeOffset());
	*pllPreviousKeyframeTime -= m_HeaderParser.GetTimeOffset();
	if(*pllPreviousKeyframeTime < 0)
		*pllPreviousKeyframeTime = 0;

	*pllNextKeyframeTime = m_IndexParser.GetNextKeyFrameTime(llTime + m_HeaderParser.GetTimeOffset());
	if(*pllNextKeyframeTime != VO_MAXS64)
		*pllNextKeyframeTime -= m_HeaderParser.GetTimeOffset();

	return VO_ERR_SOURCE_OK;
}


VO_U32 CAsfReader::SelectDefaultTracks()
{
	VOLOGR("m_nTracks %d", m_nTracks);
	VO_U8 btDefaultAudioTrackIndex = 0xFF;
	VO_U8 btDefaultVideoTrackIndex = 0xFF;

	if (m_nTracks >= 3)
	{
		AsfTrackInfo * pTracksInfo = NULL;
		for (VO_U8 b = 0; b < m_nTracks; b++)
		{
		//	pTracksInfo = (NULL == pTracksInfo) ? m_HeaderParser.GetTrackInfoPtr() : pTracksInfo->next;
			CBaseTrack *pTrack = m_ppTracks[b];
			if (pTrack->GetType() == VOTT_VIDEO)
			{
				if(!(((CBaseStreamFileTrack*)m_ppTracks[b])->IsIndexNull()))
				{
				if (0xFF == btDefaultVideoTrackIndex)
				{
					btDefaultVideoTrackIndex = b;
				} 
				else
				{
					PAsfStreamExtInfo pCurrentStreamExtInfo = m_HeaderParser.GetTrackExtInfo(b);
					PAsfStreamExtInfo pDefaultStreamExtInfo = m_HeaderParser.GetTrackExtInfo(btDefaultVideoTrackIndex);
					if (pCurrentStreamExtInfo && pDefaultStreamExtInfo && pCurrentStreamExtInfo->dwBitrate > pDefaultStreamExtInfo->dwBitrate)
					{
						btDefaultVideoTrackIndex = b;
					}
				}
			} 
			} 
			else
			{
				if (0xFF == btDefaultAudioTrackIndex)
				{
					btDefaultAudioTrackIndex = b;
				} 
				else
				{
					PAsfStreamExtInfo pCurrentStreamExtInfo = m_HeaderParser.GetTrackExtInfo(b);
					PAsfStreamExtInfo pDefaultStreamExtInfo = m_HeaderParser.GetTrackExtInfo(btDefaultAudioTrackIndex);
					if (pCurrentStreamExtInfo && pDefaultStreamExtInfo && pCurrentStreamExtInfo->dwBitrate > pDefaultStreamExtInfo->dwBitrate)
					{
						btDefaultAudioTrackIndex = b;
					}
				}
			}
		}

		if (btDefaultVideoTrackIndex >= 2)
		{
			pTracksInfo = NULL;
			for (int j = 0; j < 2; j++)
			{
			//	pTracksInfo = (NULL == pTracksInfo) ? m_HeaderParser.GetTrackInfoPtr() : pTracksInfo->next;
				if (m_ppTracks[j]->GetType() == VOTT_VIDEO)
				{
					CBaseTrack *pTrack = m_ppTracks[j];
					m_ppTracks[j] = m_ppTracks[btDefaultVideoTrackIndex];
					m_ppTracks[btDefaultVideoTrackIndex] = pTrack;
				}
			}
		}
	}

	VO_SOURCE_TRACKINFO TrackInfo;
	for(VO_U32 i = 0; i < (2 < m_nTracks ? 2 : m_nTracks); i++)
	{
		if(m_ppTracks[i])
		{
			//add by leon #6335
			if(m_ppTracks[i]->GetInfo(&TrackInfo) != VO_ERR_SOURCE_OK)
				continue;
			//add by leon #6335
			m_ppTracks[i]->SetInUsed(VO_TRUE);
		}
	}
	if(m_nTracks>2)
	{
		for(VO_U32 j = 2;j<m_nTracks;j++)
		{
		 m_ppTracks[j]->SetInUsed(VO_FALSE);

		}
	}
	

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAsfReader::GetFileRealSize(VO_U64 *pSize)
{
	*pSize = m_HeaderParser.GetFileRealSize();

	return VO_ERR_SOURCE_OK;
}


VO_BOOL CAsfReader::IsCanGenerateIndex()
{
	if(m_pFileDataParser->IsParseEnd())
		return VO_FALSE;

	if(IsTrackBufferFull() || IsTrackBufferHasEnoughFrame(0))
		return VO_FALSE;

	return VO_TRUE;
}
VO_BOOL CAsfReader::IsTrackBufferHasEnoughFrame(VO_U32 uFrameCount)
{
	///<firstly we just generate 1 second frame for video and audio to caculate the framerate
	CAsfTrack* pTrack = NULL;

	if (!m_ForStreaming)
		return VO_FALSE;

	for(VO_U32 i = 0; i < m_nTracks/**/; i++)
	{
		pTrack = (CAsfTrack*)m_ppTracks[i];
		if(m_bThumbnail)
		{
			if(	pTrack && pTrack->IsInUsed()&& pTrack->GetType()==VOTT_VIDEO && pTrack->GetFrameCntInIndex() > uFrameCount)
			{
				return VO_TRUE;
			}
			else
			{
				continue;
			}
		}
		else
		{
			//VOLOGI("Track %d :Duration:%d",i,pTrack->GetTrackBufDuration());
			if(pTrack && pTrack->IsInUsed() && pTrack->GetTrackBufDuration() < 3500)
				return VO_FALSE;
		}
	
	}
	if(m_bThumbnail)
		return VO_FALSE;
	return VO_TRUE;
}