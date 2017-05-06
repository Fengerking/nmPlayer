#include "CTsReader.h"
#include "CTsTrack.h"
#include "CStream.h"
#include "voOSFunc.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CTsReader::CTsReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
: CMTVReader(pFileOp, pMemOp, pLibOP, pDrmCB)
, m_FileDataParser(&m_chunk, pMemOp)
{
	//m_pFileDataParser = new CTsFileDataParser(&m_chunk, pMemOp);
	m_pFileDataParser = &m_FileDataParser;

	m_bTest = VO_FALSE;
}

CTsReader::~CTsReader(void)
{
}

VO_VOID CTsReader::OnData(VO_PTR pData)
{
	if (m_bTest)
	{
		CDumper::WriteLog((char *)"on data...");
	}

	//CDumper::WriteLog("on data...");

	VO_PARSER_OUTPUT_BUFFER* buf = (VO_PARSER_OUTPUT_BUFFER*)pData;
	
	//CTsTrack* pTrack = buf->nType==VO_PARSER_OT_VIDEO?(CTsTrack*)m_pTracks[0]:(CTsTrack*)m_pTracks[1];
	CTsTrack* pTrack = (CTsTrack*)GetTrackByStreamNum((VO_U32)buf->nStreamID);

	if (pTrack)
	{
		pTrack->AddFrame((VO_MTV_FRAME_BUFFER*)buf->pOutputData);
	}
}

VO_VOID CTsReader::OnBlock(VO_PTR pBlock)
{

}


VO_U32 CTsReader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	if(!m_pFileDataParser)
		return VO_ERR_SOURCE_END;

	CMTVReader::Load(nSourceOpenFlags, pFileSource);

	m_FileDataParser.SetOpenFlag(nSourceOpenFlags);
	m_FileDataParser.Init(VO_NULL);
	m_FileDataParser.SetFileSize(m_ullFileSize);
	if(!m_FileDataParser.SetStartFilePos(0, VO_TRUE))
		return VO_ERR_SOURCE_END;

	VO_U8 count = 0;
	VO_SOURCE_INFO source_info;
	MemSet(&source_info, 0, sizeof(VO_SOURCE_INFO));
	
	if(!m_FileDataParser.GetFileInfo(&source_info))
		return VO_ERR_SOURCE_OPENFAIL;

	// test code 20100617
#if 0
	// only play video
	count = 1;
#endif
	// end test

	TracksCreate(source_info.Tracks);

	for (; count<source_info.Tracks; count++)
	{
		CStream* pStream = m_FileDataParser.GetStreamByIdx(count);

		if (pStream)
		{
			TracksAdd(new CTsTrack((VO_TRACKTYPE)(pStream->GetTrackType()), pStream->GetStreamID(), source_info.Duration, this, m_pMemOp, pStream));
		}
	}

	InitTracks(nSourceOpenFlags);
	SelectDefaultTracks();

// 	for(VO_U8 i = 0; i < count; i++)
// 	{
// 		if(m_pTracks[i])
// 		{
// 			m_pStreamFileTracks[i] = (CTsTrack*)m_pTracks[i];
// 		}
// 	}

	m_FileDataParser.Reset();

	// tag
    if((nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL) != VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL)
	{
        FileGenerateIndex();
	}

	CDumper::WriteLog((char *)"end load reader.");

	return VO_ERR_SOURCE_OK;
}

VO_U32 CTsReader::Close()
{
	CDumper::WriteLog((char *)"CTsReader::Close()");

	m_FileDataParser.Uninit();

	return CMTVReader::Close();
}


VO_U32 CTsReader::GetInfo(VO_SOURCE_INFO* pSourceInfo)
{
	m_FileDataParser.GetFileInfo(pSourceInfo);

	return VO_ERR_SOURCE_OK;
}

VO_U32 CTsReader::MoveTo(VO_S64 llTimeStamp)
{
#if 1

	CDumper::WriteLog((char *)"+++++++++++++track move to %u+++++++++++++", llTimeStamp);

	// tag: 20100609
	VO_U32 t = voOS_GetSysTime();
	for (VO_U8 n=0; n<m_nTracks; n++)
	{
		if(m_ppTracks[n])
			((CTsTrack*)m_ppTracks[n])->Wait();
	}

	t = voOS_GetSysTime() - t;

	CDumper::WriteLog((char *)"------------------------------MoveTo wait time = %d", t);
	// end



	VO_U32 time = voOS_GetSysTime();


	// reset the time stamp
	m_FileDataParser.Reset();
	Flush();

	VO_SOURCE_INFO info;
	m_FileDataParser.GetFileInfo(&info);
	float p				= (float)llTimeStamp / (float)info.Duration;
	VO_U64 nPos			= (VO_U64)(p*m_ullFileSize);

	// test
	if (m_ullFileSize < nPos)
	{
		CDumper::WriteLog((char *)"Seek error.");
	}
	// end test

	if(!m_FileDataParser.SetStartFilePos(nPos))
		return VO_ERR_SOURCE_END;

	CDumper::WriteLog((char *)"------------------------------Begin to generate index");
	m_bTest = VO_TRUE;

	FileGenerateIndex();
	m_bTest = VO_FALSE;

	time = voOS_GetSysTime() - time;
	CDumper::WriteLog((char *)"+++++++++++++track move to %u FINISHED %u+++++++++++++", llTimeStamp, time);

	return VO_ERR_SOURCE_OK;

#else

	CDumper::WriteLog("track move to %u", llTimeStamp);

	Flush();

	// reset the time stamp
	m_FileDataParser.Reset();

	VO_SOURCE_INFO info;
	m_FileDataParser.GetFileInfo(&info);

	VO_U64 nPos = 0;

	if (llTimeStamp == 0)
	{
		nPos = 0;
	}
	else
	{	
		VO_U32 nDuration	= llTimeStamp - (m_FileDataParser.GetFirstTimeStamp());
		float p				= (float)nDuration / (float)info.Duration;
		nPos				= p*m_ullFileSize;
	}



	if(!m_FileDataParser.SetStartFilePos(nPos))
		return VO_ERR_SOURCE_END;

	FileGenerateIndex();

	return VO_ERR_SOURCE_OK;

#endif

}


CTsTrack* CTsReader::GetTrackByStreamNum(VO_U32 btStreamNum)
{
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && ((CTsTrack *)m_ppTracks[i])->GetStreamPID() == btStreamNum)
			return (CTsTrack*)m_ppTracks[i];
	}

	return VO_NULL;
}


VO_VOID		      CTsReader::Flush()
{
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i])
		{
			((CTsTrack*)m_ppTracks[i])->Flush();
		}
	}

	m_pFileDataParser->SetParseEnd(VO_FALSE);
	m_pFileDataParser->SetBlockStream(0xFF);
	m_pFileDataParser->SetOnBlockCallback(VO_NULL, VO_NULL, VO_NULL);

	m_chunk.SetShareFileHandle(VO_FALSE);
}


VO_U32		CTsReader::SetParameter(VO_U32 uID, VO_PTR pParam)
{
	if(uID == VO_PID_SET_EIT_CAPTION_CALLBACK)
	{
		m_FileDataParser.SetEITCallbackFun((void*)pParam);
	}
	else
	{
		return CBaseReader::SetParameter(uID, pParam);
	}
}

VO_U32		CTsReader::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	return CBaseReader::GetParameter(uID, pParam);
}

VO_U32      CTsReader::SetSelectTrack(VO_U32 nIndex, VO_BOOL bInUsed, VO_U64 llTs)
{
	VO_SOURCE_INFO info;
    CTsTrack*    pAudioTrack = NULL;
    CTsTrack*    pVideoTrack = NULL;
    CTsTrack*    pTextTrack = NULL;
    CTsTrack*    pNewTrack = NULL;
    VO_U32       ulIndex = 0;
    VO_TRACKTYPE      eNewTrackType;
    VO_U32       ulRet = 0;
	VO_U32       ulTime = (VO_U32)llTs;
    VO_SOURCE_SAMPLE   varSample;
    
	if(nIndex>=m_nTracks)
	{
		return VO_ERR_INVALID_ARG;
	}

    pNewTrack = (CTsTrack*)(m_ppTracks[nIndex]);
	if(pNewTrack == NULL)
	{
		return VO_ERR_INVALID_ARG;
	}

    VOLOGI("Get the New Track!");

	eNewTrackType = pNewTrack->GetType();
	for(ulIndex=0; ulIndex < m_nTracks; ulIndex++)
	{
		if(m_ppTracks[ulIndex]->IsInUsed())
		{
			switch(m_ppTracks[ulIndex]->GetType())
			{
			    case VOTT_AUDIO:
				{
                    pAudioTrack = (CTsTrack*)(m_ppTracks[ulIndex]);
					break;
				}
				case VOTT_VIDEO:
				{
					pVideoTrack = (CTsTrack*)(m_ppTracks[ulIndex]);
					break;
				}
				case VOTT_TEXT:
				{
					pTextTrack = (CTsTrack*)(m_ppTracks[ulIndex]);
					break;
				}
				default:
				{
					break;
				}
			}
		}
	}

	
	switch(eNewTrackType)
	{
	    case VOTT_AUDIO:
		{
			if(pAudioTrack != NULL)
			{
				pAudioTrack->SetInUsed(VO_FALSE);
				pAudioTrack = NULL;
			}
			break;
		}
	    case VOTT_VIDEO:
		{
			if(pVideoTrack != NULL)
			{
				pVideoTrack->SetInUsed(VO_FALSE);
				pVideoTrack = NULL;
			}
			break;
		}
	    case VOTT_TEXT:
		{   
			if(pTextTrack != NULL)
			{
				pTextTrack->SetInUsed(VO_FALSE);
				pTextTrack = NULL;
			}
			break;
		}
	    default:
		{
			break;
		}
	}
    
	ulRet = pNewTrack->SetInUsed(VO_TRUE);
	memset(&varSample, 0, sizeof(VO_SOURCE_SAMPLE));
    ulRet = pNewTrack->GetSampleUnUsed(&varSample);
	while(ulRet == VO_ERR_SOURCE_OK)
	{
		if(varSample.Time<llTs)
		{
			memset(&varSample, 0, sizeof(VO_SOURCE_SAMPLE));
		    ulRet = pNewTrack->GetSampleUnUsed(&varSample);
		}
		else
		{
			break;
		}
	}


	return ulRet;
}
