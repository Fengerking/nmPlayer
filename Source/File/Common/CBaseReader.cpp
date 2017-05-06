	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		CBaseReader.cpp

Contains:	CBaseReader class file

Written by:	East

Change History (most recent first):
2006-12-12		East			Create file

*******************************************************************************/
#include "voString.h"
#include "CBaseReader.h"
#include "voLog.h"


#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

extern VO_U32 g_dwFRModuleID;
extern VO_HANDLE g_hInst;

#define VO_CHECK_LIB_FLAG_FILEPARSER		3

CBaseReader::CBaseReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
	: CvoBaseFileOpr(pFileOp)
	, CvoBaseMemOpr(pMemOp)
	, CvoBaseDrmCallback(pDrmCB)
	, m_hFile(VO_NULL)
	, m_ullFileSize(0)
	, m_chunk(pFileOp, pMemOp)
	, m_nTrackSize(0)
	, m_ppTracks(VO_NULL)
	, m_nTracks(0)
	, m_nSeekTrack(0xffffffff)
	, m_hCheck(VO_NULL)
#if defined(_IOS) || defined(_SUPPORT_PARSER_METADATA)
	, m_pMetaDataParser(VO_NULL)
#endif
{
}

CBaseReader::~CBaseReader()
{
	Close();
}

VO_U32 CBaseReader::init(VO_U32 nSourceOpenFlags, VO_LIB_OPERATOR* pLibOP, VO_U32 uModuleID, VO_TCHAR* pWorkingPath)
{
#if 0
//#if !defined(__arm) && !defined(_MAC_OS) && !defined(_IOS) && !defined(_METRO)
	VO_S32 nCheckLibFlag = VO_CHECK_LIB_FLAG_FILEPARSER;
	if (VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL == (nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL) ||
		(nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_INFOONLY) )
		nCheckLibFlag |= 0x0100;
#endif
	VOLOGUNINIT();//maybe called at source wrapper.
	VOLOGINIT(pWorkingPath);
	if (VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL == (nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL))
	{
		return VO_ERR_SOURCE_OK;
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseReader::uninit()
{
//#if !defined(__arm) && !defined(_MAC_OS) && !defined(_IOS) && !defined(_METRO)
	VO_U32 ret = VO_ERR_SOURCE_OK;
	
	if (VO_NULL != m_hCheck)
	{
		m_hCheck = NULL;
	}
	VOLOGUNINIT();
	return ret;
}

VO_U32 CBaseReader::CheckAudio(VO_SOURCE_SAMPLE* pSample)
{
	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseReader::CheckVideo(VO_SOURCE_SAMPLE* pSample)
{
	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseReader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	//open file
	pFileSource->nMode = VO_FILE_READ_ONLY;
	m_hFile = FileOpen(pFileSource);
	if(!m_hFile)
		return VO_ERR_SOURCE_OPENFAIL;

	m_ullFileSize = FileSize(m_hFile);

	if(!m_chunk.FCreate(m_hFile))
		return VO_ERR_SOURCE_OPENFAIL;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseReader::Close()
{
	TracksDestroy();

	m_chunk.FDestroy();

	//close file
	if(m_hFile)
	{
		FileClose(m_hFile);
		m_hFile = VO_NULL;
	}
#if defined(_IOS) || defined(_SUPPORT_PARSER_METADATA)
	SAFE_DELETE(m_pMetaDataParser);
#endif
	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseReader::SetParameter(VO_U32 uID, VO_PTR pParam)
{
	switch(uID)
	{
	case VO_PID_SOURCE_SELTRACK:
		{
			VO_SOURCE_SELTRACK* pSelTrack = (VO_SOURCE_SELTRACK*)pParam;
			return SetSelectTrack(pSelTrack->nIndex, pSelTrack->bInUsed, pSelTrack->llTs);
		}
		break;

	case VO_PID_SOURCE_PREPARETRACKS:
		{
			VO_BOOL* pbPrepare = (VO_BOOL*)pParam;
			return (*pbPrepare) ? PrepareTracks() : UnprepareTracks();
		}
		break;

	case VO_PID_SOURCE_PLAYMODE:
		{
			VO_SOURCE_PLAYMODE* pPlayMode = (VO_SOURCE_PLAYMODE*)pParam;
			return SetPlayMode(*pPlayMode);
		}
		break;

	default:
	    break;
	}

	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CBaseReader::GetParameter(VO_U32 uID, VO_PTR pParam)
{
#if defined(_IOS) || defined(_SUPPORT_PARSER_METADATA)
	if(VO_PID_METADATA_BASE == (uID & VO_PID_METADATA_BASE))
	{
		VOLOGI("Get MetaData.ID:0x%08x",uID);
		if(!m_pMetaDataParser)
		{

			VO_U32 rc = InitMetaDataParser();
			if(VO_ERR_METADATA_OK != rc || !m_pMetaDataParser)
				return VO_ERR_METADATA_NOMETADATA;
		}

		return m_pMetaDataParser->GetParameter(uID, pParam);
	}
#endif
	switch(uID)
	{
	case VO_PID_SOURCE_SELTRACK:
		{
			VO_SOURCE_SELTRACK* pSelTrack = (VO_SOURCE_SELTRACK*)pParam;
			return GetSelectTrack(pSelTrack->nIndex, pSelTrack->bInUsed);
		}
		break;

	case VO_PID_FILE_HEADSIZE:
		return GetFileHeadSize((VO_U32*)pParam);

	case VO_PID_FILE_HEADDATAINFO:
		return GetFileHeadDataInfo((headerdata_info*)pParam);

	case VO_PID_FILE_FILEPOS2MEDIATIME:
		return GetMediaTimeByFilePos((VO_FILE_MEDIATIMEFILEPOS*)pParam);

	case VO_PID_FILE_MEDIATIME2FILEPOS:
		return GetFilePosByMediaTime((VO_FILE_MEDIATIMEFILEPOS*)pParam);

	case VO_PID_SOURCE_FILESIZE:
		return GetFileSize((VO_S64*)pParam);

	case VO_PID_FILE_GetSEEKPOSBYTIME:
		return GetSeekPosByTime( (VO_FILE_MEDIATIMEFILEPOS*)pParam );

	case VO_PID_SOURCE_SEEKABLE:
		return GetSeekable((VO_BOOL*)pParam);

	case VO_PID_SOURCE_NEEDSCAN:
		return IsNeedScan((VO_BOOL*)pParam);

	case VO_PID_SOURCE_ACTUALFILESIZE:
		return GetFileRealSize((VO_U64*)pParam);

	case VO_PID_SOURCE_GETSEEKTIMEBYPOS:
		return GetSeekTimeByPos((VO_U64*)pParam);

	case VO_PID_SOURCE_WIDEVINE:
		return GetDRMType((VO_U32*)pParam);
	case VO_PID_SOURCE_GETTHUMBNAIL:
		{
			CBaseTrack* pTrack = GetVideoTrackInUsed();
			if(pTrack){
				return pTrack->GetParameter(VO_PID_VIDEO_GETTHUMBNAIL, pParam);
			}

		}
	default:
		break;
	}

	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CBaseReader::GetInfo(VO_SOURCE_INFO* pSourceInfo)
{
	if(!pSourceInfo)
		return VO_ERR_INVALID_ARG;

	pSourceInfo->Duration = GetDuration();
	pSourceInfo->Tracks = m_nTracks;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseReader::GetDuration()
{
	VO_U32 dwDuration = 0;
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && m_ppTracks[i]->m_dwDuration > dwDuration)
			dwDuration = m_ppTracks[i]->m_dwDuration;
	}

	return dwDuration;
}

VO_S32 CBaseReader::ReadBuffer(VO_PBYTE pData, VO_S64 llFilePos, VO_U32& dwLen)
{
	if(m_hFile)
	{
		VO_S64 nRes = FileSeek(m_hFile, llFilePos, VO_FILE_BEGIN);
		if(nRes >= 0)
			nRes = FileRead(m_hFile, pData, dwLen);

		if(nRes > 0)
			dwLen = (VO_U32)(nRes);

		return (-2 == nRes) ? 2 : (nRes > 0);
	}

	return 0;
}

VO_U32 CBaseReader::SetPlayMode(VO_SOURCE_PLAYMODE PlayMode)
{
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i])
		{
			VO_U32 nRes = m_ppTracks[i]->SetPlayMode(PlayMode);
			if(VO_ERR_SOURCE_OK != nRes)
				return nRes;
		}
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseReader::MoveTo(VO_S64 llTimeStamp)
{
	return VO_ERR_SOURCE_OK;
}

CBaseTrack* CBaseReader::GetTrackByIndex(VO_U32 nIndex)
{
	return (nIndex >= 0 && nIndex < m_nTracks) ? m_ppTracks[nIndex] : VO_NULL;
}

CBaseTrack* CBaseReader::GetTrackByStreamNum(VO_U8 btStreamNum)
{
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && m_ppTracks[i]->m_btStreamNum == btStreamNum)
			return m_ppTracks[i];
	}

	return VO_NULL;
}

CBaseTrack* CBaseReader::GetAudioTrackInUsed()
{
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed() && VOTT_AUDIO == m_ppTracks[i]->GetType())
			return m_ppTracks[i];
	}

	return VO_NULL;
}

CBaseTrack* CBaseReader::GetVideoTrackInUsed()
{
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed() && VOTT_VIDEO == m_ppTracks[i]->GetType())
			return m_ppTracks[i];
	}

	return VO_NULL;
}

CBaseTrack* CBaseReader::GetOtherTrackInUsed(CBaseTrack* pTrack)
{
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed() && m_ppTracks[i] != pTrack && VOTT_TEXT != m_ppTracks[i]->GetType())
			return m_ppTracks[i];
	}

	return VO_NULL;
}

VO_U32 CBaseReader::SetSelectTrack(VO_U32 nIndex, VO_BOOL bInUsed, VO_U64 llTs)
{
	if(nIndex == VO_ALL)
	{
		for(VO_U32 i = 0; i < m_nTracks; i++)
		{
			if(m_ppTracks[i])
				m_ppTracks[i]->SetInUsed(bInUsed);
		}

		return VO_ERR_SOURCE_OK;
	}
	else
	{
		CBaseTrack* pTrack = GetTrackByIndex(nIndex);
		if(!pTrack)
			return VO_ERR_SOURCE_TRACKNOTFOUND;

		if(pTrack->IsInUsed() == bInUsed)
			return VO_ERR_SOURCE_OK;

		return pTrack->SetInUsed(bInUsed);
	}
}

VO_U32 CBaseReader::GetSelectTrack(VO_U32 nIndex, VO_BOOL& bInUsed)
{
	CBaseTrack* pTrack = GetTrackByIndex(nIndex);
	if(!pTrack)
		return VO_ERR_SOURCE_TRACKNOTFOUND;

	bInUsed = pTrack->IsInUsed();
	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseReader::TracksCreate(VO_U32 nTrackSize)
{
	TracksDestroy();

	m_ppTracks = NEW_OBJS(CBaseTrack*, nTrackSize);
	if(!m_ppTracks)
		return VO_ERR_OUTOF_MEMORY;

	MemSet(m_ppTracks, 0, nTrackSize * sizeof(CBaseTrack*));

	m_nTrackSize = nTrackSize;
	m_nTracks = 0;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseReader::TracksDestroy()
{
	m_nTrackSize = 0;
	SAFE_MEM_FREE(m_ppTracks);
	m_nTracks = 0;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseReader::TracksAdd(CBaseTrack* pNewTrack)
{
	for(VO_U32 i = 0; i < m_nTrackSize; i++)
	{
		if(!m_ppTracks[i])
		{
			m_ppTracks[i] = pNewTrack;
			m_nTracks++;

			return VO_ERR_SOURCE_OK;
		}
	}

	return VO_ERR_OUTOF_MEMORY;
}

VO_U32 CBaseReader::TracksRemove(CBaseTrack* pDelTrack)
{
	VO_U32 nFoundIndex = 0xffffffff;
	for(VO_U32 i = 0; i < m_nTrackSize; i++)
	{
		if(m_ppTracks[i] == pDelTrack)
		{
			nFoundIndex = i;

			m_ppTracks[i] = VO_NULL;
			m_nTracks--;

			break;
		}
	}

	if(0xffffffff == nFoundIndex)
		return VO_ERR_SOURCE_TRACKNOTFOUND;

	for(VO_U32 i = nFoundIndex; i < m_nTrackSize - 1; i++)
		m_ppTracks[i] = m_ppTracks[i + 1];
	m_ppTracks[m_nTrackSize - 1] = VO_NULL;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseReader::InitTracks(VO_U32 nSourceOpenFlags)
{
	VO_U32 rc = VO_ERR_SOURCE_OK;
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i])
		{
			rc = m_ppTracks[i]->Init(nSourceOpenFlags);
			if(VO_ERR_SOURCE_OK != rc)
				return rc;
		}
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseReader::UninitTracks()
{
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i])
			m_ppTracks[i]->Uninit();
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseReader::SelectDefaultTracks()
{
	VO_U32 nFlags = 0;	//0x1 - audio selected, 0x2 - video selected
	//VO_SOURCE_TRACKINFO TrackInfo;

	/*#6335 10/21/2011, leon, someone unwilling modify codes to GetSelectedTrack
	** so exchange the selected Track to 0/1(0 for audio ,1 for video)
	*/
	int audioSelID = -1;
	int videoSelID = -1;

	for(VO_S32 i = 0; i < (VO_S32)m_nTracks; i++)
	{
		if(m_ppTracks[i])
		{
			//add by leon #6335
			VO_U32 pCodec = 0;
			if(m_ppTracks[i]->GetCodec(&pCodec) != VO_ERR_SOURCE_OK || pCodec <=0 )
				continue;
			//add by leon #6335
			if(VOTT_AUDIO == m_ppTracks[i]->GetType() && !(nFlags & 0x1))
			{
				m_ppTracks[i]->SetInUsed(VO_TRUE);
				nFlags |= 0x1;
				audioSelID = i;
			}

			if(VOTT_VIDEO == m_ppTracks[i]->GetType() && !(nFlags & 0x2))
			{
				m_ppTracks[i]->SetInUsed(VO_TRUE);
				nFlags |= 0x2;
				videoSelID = i;
			}
//12/14/2011,leon support subtitle
//#ifdef _SUPPORT_TTEXT
			if(VOTT_TEXT == m_ppTracks[i]->GetType() && !(nFlags & 0x4))
			{
				m_ppTracks[i]->SetInUsed(VO_TRUE);
				nFlags |= 0x4;
			}
//#endif
		}
	}
	
	/*#6335 10/21/2011, leon, someone unwilling modify codes to GetSelectedTrack
	** so exchange the selected Track to 0/1(0, 1 for audio  or video)
	*/
/*	
	CBaseTrack* pTracks;
	if(audioSelID >=0)
	{
		pTracks = m_ppTracks[0];
		m_ppTracks[0] = m_ppTracks[audioSelID];
		m_ppTracks[audioSelID] = pTracks;
		if(videoSelID == 0) videoSelID = audioSelID;
	}
	
	if(videoSelID >=0 && m_nTracks != 1)
	{
		pTracks = m_ppTracks[1];
		m_ppTracks[1] = m_ppTracks[videoSelID];
		m_ppTracks[videoSelID] = pTracks;
	}
*/	

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseReader::PrepareTracks()
{
	VO_U32 rc = VO_ERR_SOURCE_OK;
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed())
		{
			rc = m_ppTracks[i]->Prepare();
			if (rc != VO_ERR_SOURCE_OK)
				return rc;
		}
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseReader::UnprepareTracks()
{
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i])
			m_ppTracks[i]->Unprepare();
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseReader::GetFileSize(VO_S64* pllFileSize)
{
	*pllFileSize = m_ullFileSize;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseReader::GetSeekable(VO_BOOL* pIsSeekable)
{
	*pIsSeekable = VO_TRUE;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseReader::IsNeedScan(VO_BOOL* pIsNeedScan)
{
	*pIsNeedScan = VO_FALSE;

	return VO_ERR_SOURCE_OK;
}
