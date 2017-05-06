#include "CBaseStreamFileReader.h"
#include "voLog.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CBaseStreamFileReader::CBaseStreamFileReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
	: CBaseReader(pFileOp, pMemOp, pLibOP, pDrmCB)
	, m_pFileDataParser(VO_NULL)
	, m_chunkUseForDataParser(pFileOp, pMemOp)
	, m_memPool(pMemOp)
{
}

CBaseStreamFileReader::~CBaseStreamFileReader()
{
}

VO_VOID CBaseStreamFileReader::FileGenerateIndex()
{
	while(IsCanGenerateIndex())
	{
		if(!m_pFileDataParser->Step())
			m_pFileDataParser->SetParseEnd(VO_TRUE);
	}
}

VO_BOOL CBaseStreamFileReader::IsTrackBufferFull()
{
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed() && ((CBaseStreamFileTrack*)m_ppTracks[i])->IsGlobalBufferFull())
			return VO_TRUE;
	}

	return VO_FALSE;
}

VO_VOID CBaseStreamFileReader::OnNewDataParser(CBaseTrack* pRqstTrack, CvoFileDataParser** ppDataParser)
{
	m_chunk.SetShareFileHandle(VO_TRUE);

	if(m_chunkUseForDataParser.IsValid())
		m_chunkUseForDataParser.FLocate(m_chunk.FGetFilePos());
	else
		m_chunkUseForDataParser.FCreate(m_hFile, m_chunk.FGetFilePos());

	m_chunkUseForDataParser.SetShareFileHandle(VO_TRUE);
}

VO_VOID CBaseStreamFileReader::OnDelDataParser(CBaseTrack* pRqstTrack)
{
	m_pFileDataParser->SetBlockStream(0xFF);
	m_pFileDataParser->SetOnBlockCallback(VO_NULL, VO_NULL, VO_NULL);

	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed())
			((CBaseStreamFileTrack*)m_ppTracks[i])->SetParseForSelf(VO_FALSE);
	}

	m_chunk.SetShareFileHandle(VO_FALSE);
}

VO_VOID CBaseStreamFileReader::Flush()
{
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed())
			((CBaseStreamFileTrack*)m_ppTracks[i])->Flush();
	}

	m_pFileDataParser->SetParseEnd(VO_FALSE);
	m_pFileDataParser->SetBlockStream(0xFF);
	m_pFileDataParser->SetOnBlockCallback(VO_NULL, VO_NULL, VO_NULL);

	m_chunk.SetShareFileHandle(VO_FALSE);
}

VO_BOOL CBaseStreamFileReader::IsCanGenerateIndex()
{
	if(m_pFileDataParser->IsParseEnd())
		return VO_FALSE;

	if(IsTrackBufferFull())
		return VO_FALSE;

	return VO_TRUE;
}
