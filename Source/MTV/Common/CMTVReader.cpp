#include "CMTVReader.h"
#include "voOSFunc.h"
#include "CDumper.h"


VO_U8 CMTVReader::WhenDataParsed(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam)
{
	CMTVReader* pReader = (CMTVReader*)pUser;

	pReader->OnData(pParam);

	return VO_ERR_SOURCE_OK;
}

VO_BOOL CMTVReader::WhenBlock(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam)
{
	CMTVReader* pReader = (CMTVReader*)pUser;

	pReader->OnBlock(pParam);

	return VO_TRUE;
}


CMTVReader::CMTVReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
: CBaseStreamFileReader(pFileOp, pMemOp, pLibOP, pDrmCB)
//, m_bSeeking(VO_FALSE)
{
	//m_StepEvent.Signal();
}

CMTVReader::~CMTVReader(void)
{
	Close();
}

VO_U32 CMTVReader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	//Close();

	//open file
//	OutputDebugString(L"CMTVReader::Load\n");
	pFileSource->nMode = VO_FILE_READ_ONLY;
	m_hFile = FileOpen(pFileSource);
	if(!m_hFile)
		return VO_ERR_SOURCE_OPENFAIL;

	m_ullFileSize = FileSize(m_hFile);

	if(!m_chunk.FCreate(m_hFile, 0, 1024*128))
		return VO_ERR_SOURCE_OPENFAIL;

	if (m_pFileDataParser)
	{
		m_pFileDataParser->SetOnDataCallback(this, VO_NULL, CMTVReader::WhenDataParsed);
		m_pFileDataParser->SetOnBlockCallback(this, VO_NULL, CMTVReader::WhenBlock);
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMTVReader::Close()
{
// 	if (m_pFileDataParser)
// 	{
// 		delete m_pFileDataParser;
// 		m_pFileDataParser = VO_NULL;
// 	}

	for(VO_U8 i = 0; i < m_nTracks; i++)
	{
		if (m_ppTracks && m_ppTracks[i])
		{
			delete m_ppTracks[i];
			m_ppTracks[i] = VO_NULL;
		}
	}
		

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMTVReader::GetFileSize(VO_S64* pllFileSize)
{
	return CBaseReader::GetFileSize(pllFileSize);
}

VO_U32 CMTVReader::GetInfo(VO_SOURCE_INFO* pSourceInfo)
{
	return VO_ERR_SOURCE_OPENFAIL;
}

VO_U32 CMTVReader::MoveTo(VO_S64 llTimeStamp)
{
// 	VO_U32 t = voOS_GetSysTime();
// 	m_StepEvent.Wait(5000);
// 	t = voOS_GetSysTime() - t;
// 	CDumper::WriteLog("MoveTo wait time = %d", t);

	return VO_ERR_SOURCE_OK;
}


VO_U32	CMTVReader::OnSetPos(VO_S64* pPos)
{
	for (VO_U8 n=0; n<m_nTracks; n++)
	{
		if(m_ppTracks[n])
			((CBaseStreamFileTrack*)m_ppTracks[n])->Flush();
	}

	return VO_ERR_SOURCE_OK;
}


VO_VOID	CMTVReader::FileGenerateIndex()
{
	//m_StepEvent.Reset();

	while(IsCanGenerateIndex())
	{
		if(!m_pFileDataParser->Step())
			m_pFileDataParser->SetParseEnd(VO_TRUE);
	}

	//m_StepEvent.Signal();

	//CDumper::WriteLog(" -----------------------CMTVReader::FileGenerateIndex()");
}

VO_BOOL CMTVReader::IsCanGenerateIndex()
{
	if(m_pFileDataParser->IsParseEnd())
		return VO_FALSE;

	if(IsTrackBufferFull())
		return VO_FALSE;

	return VO_TRUE;
}