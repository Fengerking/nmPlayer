
#include "CCmmbReader.h"
#include "CCmmbTrack.h"
#include "CDumper.h"
#include "voLog.h"

VO_U8 WhenDataParsed(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam)
{
	CCmmbReader* pReader = (CCmmbReader*)pUser;

	pReader->OnData(pParam);

	//return pReader->OnPacket(pSample->pFileChunk, pSample->btStreamNum, pSample->btFlags, pSample->wPacketLen, pSample->dwTimeStamp);

	return VO_ERR_SOURCE_OK;
}

VO_BOOL WhenBlock(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam)
{
	CCmmbReader* pReader = (CCmmbReader*)pUser;
	CMMB_FRAME_BUFFER* pSample = (CMMB_FRAME_BUFFER*)pParam;

	pReader->OnBlock(pSample);

	return VO_TRUE;
}

CCmmbReader::CCmmbReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
: CMTVReader(pFileOp, pMemOp, pLibOP, pDrmCB)
, m_FileDataParser(&m_chunk, pMemOp)
{
	m_pFileDataParser = &m_FileDataParser;
}

CCmmbReader::~CCmmbReader(void)
{
}


VO_VOID	CCmmbReader::OnData(VO_PTR pData)
{
	if (!m_ppTracks)
	{
		return;
	}


	CMMB_FRAME_BUFFER* pBuffer = (CMMB_FRAME_BUFFER*)pData;

// 	if (m_bSeeking)
// 	{
// 		if (pBuffer->bVideo)
// 		{
// 			if(!pBuffer->bKeyFrame)
// 				return;
// 			else
// 				m_bSeeking = VO_FALSE;
// 		}
// 		else
// 			return;
// 	}

	CCmmbTrack* pTrack = pBuffer->bVideo?(CCmmbTrack*)m_ppTracks[0]:(CCmmbTrack*)m_ppTracks[1];

	if (pTrack)
	{
		pTrack->OnData(pBuffer);
	}

}

VO_VOID	CCmmbReader::OnBlock(VO_PTR pBlock)
{

}


VO_U32 CCmmbReader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	Close();
	
	CMTVReader::Load(nSourceOpenFlags, pFileSource);

	m_FileDataParser.Init(VO_NULL);
	if(!m_FileDataParser.SetStartFilePos(0, VO_TRUE))
		return VO_ERR_SOURCE_END;
	m_FileDataParser.SetFileSize(m_ullFileSize);

	CMMB_FILE_INFO* info = m_FileDataParser.GetFileInfo();
	CDumper::WriteLog("cmmb file info: duration=%d(s), stream id=%d, audio codec=%d, video codec=%d", info->nDuration/1000, info->nStreamID, info->nAudioCodec, info->nVideoCodec);
	
	VO_BYTE track_count = 0;
	if (info->nVideoCodec != VO_VIDEO_CodingUnused)
		track_count++;
	if (info->nAudioCodec != VO_AUDIO_CodingUnused)
		track_count++;

	TracksCreate(track_count);
	if (info->nVideoCodec != VO_VIDEO_CodingUnused)
	{
		TracksAdd(new CCmmbTrack(VOTT_VIDEO, 0, info->nDuration, this, m_pMemOp));
	}

	if (info->nAudioCodec != VO_AUDIO_CodingUnused)
	{
		TracksAdd(new CCmmbTrack(VOTT_AUDIO, 1, info->nDuration, this, m_pMemOp));
	}

	InitTracks(nSourceOpenFlags);
	SelectDefaultTracks();

	m_FileDataParser.SetOnDataCallback(this, VO_NULL, WhenDataParsed);
	m_FileDataParser.SetOnBlockCallback(this, VO_NULL, WhenBlock);

	FileGenerateIndex();

	return VO_ERR_SOURCE_OK;
}

VO_U32 CCmmbReader::Close()
{
	m_FileDataParser.Uninit();

	return CMTVReader::Close();
}

VO_U32 CCmmbReader::SetParameter(VO_U32 uID, VO_PTR pParam)
{
	CBaseReader::SetParameter(uID, pParam);

	return VO_ERR_SOURCE_OK;
}

VO_U32 CCmmbReader::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	CBaseReader::GetParameter(uID, pParam);

	return VO_ERR_SOURCE_OK;
}

VO_U32 CCmmbReader::GetInfo(VO_SOURCE_INFO* pSourceInfo)
{
	CMMB_FILE_INFO* pInfo = m_FileDataParser.GetFileInfo();

	pSourceInfo->Duration	= pInfo->nDuration*1000;
	
	pSourceInfo->Tracks = 0;
	if (pInfo->nAudioCodec != VO_AUDIO_CodingUnused)
	{
		pSourceInfo->Tracks++;
	}
	
	if (pInfo->nVideoCodec != VO_VIDEO_CodingUnused)
	{
		pSourceInfo->Tracks++;
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CCmmbReader::MoveTo(VO_S64 llTimeStamp)
{
	Flush();

	VO_U64 nPos = 0;
	float p				= (float)llTimeStamp / (float)(m_FileDataParser.GetFileInfo()->nDuration);
	nPos				= p*m_ullFileSize;

	if(!m_pFileDataParser->SetStartFilePos(nPos))
		return VO_ERR_SOURCE_END;

	CDumper::WriteLog("Begin to FileGenerateIndex in MoveTo...");

	//m_bSeeking = VO_TRUE;
	FileGenerateIndex();

	return VO_ERR_SOURCE_OK;
}

