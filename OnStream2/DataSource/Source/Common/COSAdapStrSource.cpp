#include "COSAdapStrSource.h"
#include "voASControllerAPI.h"

#define  LOG_TAG    "COSAdapStrSource"
#include "voLog.h"


COSAdapStrSource::COSAdapStrSource(VO_SOURCE2_LIB_FUNC *pLibop)
	: COSBaseSource(pLibop)	
	, m_pProgramInfo(NULL)
	, m_nCurPlayingAudioTrack(0)
	, m_nCurPlayingVideoTrack(0)
	, m_nCurPlayingSubtitleTrack(0)
	, m_nCurSelectedAudioTrack(-1)
	, m_nCurSelectedVideoTrack(-1)
	, m_nCurSelectedSubtitleTrack(-1)
	, m_nIsCommitSelectTrack(0)
	, m_nCurSelectedNoCommitAudioTrack(-1)
	, m_nCurSelectedNoCommitVideoTrack(-1)
	, m_nCurSelectedNoCommitSubtitleTrack(-1)
{	
	memset(&m_szChunkInfo, 0,sizeof(VOOSMP_SRC_CHUNK_INFO));
	memset(m_pAudioTrackLan, 0, sizeof(char)*128*256);
	memset(m_pSubtitleTrackLan, 0, sizeof(char)*128*256);
	memset (&m_szChunkSample, 0, sizeof (VOOSMP_SRC_CHUNK_SAMPLE));

	LoadDll();
}

COSAdapStrSource::~COSAdapStrSource ()
{
}

int	COSAdapStrSource::Init(void * pSource, int nFlag, void* pInitparam, int nInitFlag)
{
	int nRC = COSBaseSource::Init(pSource, nFlag, pInitparam, nInitFlag);
	return nRC;
}

int	COSAdapStrSource::Uninit()
{
	m_nSubtitleCurTrack = -1;
	int nRC = COSBaseSource::Uninit();
	return nRC;
}


int COSAdapStrSource::GetParam (int nID, void * pValue)
{	
	int nRC = COSBaseSource::GetParam(nID, pValue);
	return nRC;
}

int COSAdapStrSource::SetParam (int nID, void * pValue)
{
	int nRC = COSBaseSource::SetParam(nID, pValue);
	return nRC;
}

int COSAdapStrSource::HandleEvent (int nID, int nParam1, int nParam2)
{
	int nRC = 0;

	VOLOGI("HandleEvent+++++ nID %d", nID);

	VOOSMP_SRC_CHUNK_INFO* pTmpSrcChunkInfo = NULL;

	if(nID == VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_INFO)
	{
		m_nEventID = VOOSMP_SRC_CB_Adaptive_Streaming_Info;

		if(nParam1 == VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_BITRATE_CHANGE)
		{
			m_nParam1 = VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_BITRATE_CHANGE;
			m_nParam2 =	nParam2;
		}
		else if(nParam1 == VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_MEDIATYPE_CHANGE)
		{
			m_nParam1 = VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_MEDIATYPE_CHANGE;

			if(nParam2 == VO_SOURCE2_MT_PUREAUDIO)
			{
				m_nParam2 = VOOSMP_AVAILABLE_PUREAUDIO;
			}
			else if(nParam2 == VO_SOURCE2_MT_PUREVIDEO)
			{
				m_nParam2 = VOOSMP_AVAILABLE_PUREVIDEO;
			}
			else if(nParam2 == VO_SOURCE2_MT_AUDIOVIDEO)
			{
				m_nParam2 = VOOSMP_AVAILABLE_AUDIOVIDEO;
			}
			else if(nParam2 == VO_SOURCE2_MT_SUBTITLE)
			{
				m_nParam2 = VOOSMP_AVAILABLE_SUBTITLE;
			}
			else
			{
				m_nParam2 =	nParam2;
			}
		}
		else if(nParam1 == VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_PROGRAMTYPE)
		{
			m_nParam1 = VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PROGRAM_TYPE;
			
			if(nParam2 == VO_SOURCE2_STREAM_TYPE_LIVE)
			{
				m_nParam2 = VOOSMP_SRC_PROGRAM_TYPE_LIVE;
			}
			else if(nParam2 == VO_SOURCE2_STREAM_TYPE_VOD)
			{
				m_nParam2 = VOOSMP_SRC_PROGRAM_TYPE_VOD;
			}
			else
			{
				m_nParam2 =	nParam2;
			}
		}
		else if(nParam1 == VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_CHUNK_BEGINDOWNLOAD)
		{
			m_nParam1 = VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_BEGINDOWNLOAD;

			pTmpSrcChunkInfo = (VOOSMP_SRC_CHUNK_INFO*)nParam2;
			memcpy(&m_szChunkInfo, pTmpSrcChunkInfo, sizeof(VOOSMP_SRC_CHUNK_INFO));			
			m_nParam2 =	(int)(&m_szChunkInfo);
		}
		else if(nParam1 == VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_CHUNK_DROPPED)
		{
			m_nParam1 = VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DROPPED;

			pTmpSrcChunkInfo = (VOOSMP_SRC_CHUNK_INFO*)nParam2;
			memcpy(&m_szChunkInfo, pTmpSrcChunkInfo, sizeof(VOOSMP_SRC_CHUNK_INFO));			
			m_nParam2 =	(int)(&m_szChunkInfo);
		}
		else if(nParam1 == VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_CHUNK_DOWNLOADOK)
		{
			m_nParam1 = VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DOWNLOADOK;

			pTmpSrcChunkInfo = (VOOSMP_SRC_CHUNK_INFO*)nParam2;
			memcpy(&m_szChunkInfo, pTmpSrcChunkInfo, sizeof(VOOSMP_SRC_CHUNK_INFO));			
			m_nParam2 =	(int)(&m_szChunkInfo);
		}
		else if(nParam1 == VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_PROGRAM_CHANGE)
		{
			m_nParam1 = VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PROGRAM_CHANGE;
			m_nParam2 =	nParam2;
		}
		else if(nParam1 == VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_FILEFORMATSUPPORTED)
		{
			m_nParam1 = VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_FILE_FORMATSUPPORTED;
			m_nParam2 =	nParam2;
		}
		else if(nParam1 == VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_LIVESEEKABLE)
		{
			m_nParam1 = VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_LIVESEEKABLE;
			m_nParam2 =	nParam2;
		}
		else if(nParam1 == VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_DISCONTINUESAMPLE)
		{
			VOOSMP_SRC_CHUNK_SAMPLE* pChunkSample;
			m_nParam1 = VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_DISCONTINUE_SAMPLE;
			pChunkSample = (VOOSMP_SRC_CHUNK_SAMPLE*)nParam2;
			memcpy(&m_szChunkSample, pChunkSample, sizeof(VOOSMP_SRC_CHUNK_SAMPLE));			
			m_nParam2 =	(int)(&m_szChunkSample);
		}
		else
		{
			m_nParam1 = nParam1;
			m_nParam2 =	nParam2;
		}

		m_nSyncMsg = 1; 
	}
	else if(nID == VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_ERROR)
	{
		m_nEventID = VOOSMP_SRC_CB_Adaptive_Streaming_Error;
		
		if(nParam1 == VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_PLAYLIST_PARSEFAIL)
		{
			m_nParam1 = VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_PLAYLIST_PARSEFAIL;
			m_nParam2 =	nParam2;
		}
		else if(nParam1 == VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_PLAYLIST_UNSUPPORTED)
		{
			m_nParam1 = VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_PLAYLIST_UNSUPPORTED;
			m_nParam2 =	nParam2;
		}
		else if(nParam1 == VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_STREAMING_UNSUPPORTED)
		{
			m_nParam1 = VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_UNSUPPORTED;
			m_nParam2 =	nParam2;
		}
		else if(nParam1 == VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_STREAMING_DOWNLOADFAIL)
		{
			m_nParam1 = VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_DOWNLOADFAIL;
			m_nParam2 =	nParam2;
		}
		else if(nParam1 == VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_STREAMING_DRMLICENSEERROR)
		{
			m_nParam1 = VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_DRMLICENSEERROR;
			m_nParam2 =	nParam2;
		}
		else if(nParam1 == VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_STREAMING_VOLIBLICENSEERROR)
		{
			m_nEventID = VOOSMP_CB_LicenseFailed;
			m_nParam1 = nParam2; //if license failed, nParam1 is license check return code
			m_nParam2 = 0;
		}
		else
		{
			m_nParam1 = nParam1;
			m_nParam2 =	nParam2;
		}
		
		m_nSyncMsg = 1; 
	}
	else if(nID == VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_WARNING)
	{
		m_nEventID = VOOSMP_SRC_CB_Adaptive_Stream_Warning;
		
		if(nParam1 == VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_DOWNLOADERROR)
		{
			m_nParam1 = VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_DOWNLOADERROR;
		}
		else if(nParam1 == VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_FILEFORMATUNSUPPORTED)
		{
			m_nParam1 = VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_FILEFORMATUNSUPPORTED;
		}
		else if(nParam1 == VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_DRMERROR)
		{
			m_nParam1 = VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_DRMERROR;
		}
		else
		{
			m_nParam1 = nParam1;
		}

		m_nParam2 =	nParam2;
		m_nSyncMsg = 1; 
	}
	else if(nID == VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CUSTOMTAG)
	{
		m_nEventID = VOOSMP_SRC_CB_Customer_Tag;
		
		if(nParam1 == VO_SOURCE2_CUSTOMERTAGID_TIMEDTAG)
		{
			m_nParam1 = VOOSMP_SRC_CUSTOMERTAGID_TIMEDTAG;
		}
		else
		{
			m_nParam1 = nParam1;
		}

		m_nParam2 =	nParam2;
		m_nSyncMsg = 1; 
	}
	else
	{
		nRC = COSBaseSource::HandleEvent (nID, nParam1, nParam2);		
		if(nRC){
			VOLOGI("HandleEvent--- nID %d", nID);		
			return nRC;
		}
	}

	nRC = callBackEvent ();

	VOLOGI("HandleEvent--- nID %d", nID);

	if((unsigned int)nRC == VOOSMP_SRC_ERR_CHUNK_SKIP)
	{
		return VO_RET_SOURCE2_ADAPTIVESTREAMING_CHUNK_SKIP;
	}
	else if ((unsigned int)nRC == VOOSMP_SRC_ERR_FORCETIMESTAMP)
	{
		return VO_RET_SOURCE2_ADAPTIVESTREAMING_FORCETIMESTAMP;
	}
	else if((unsigned int)nRC == VOOSMP_ERR_Retry)
    {
        VOLOGI("Source event return retry, %d", VO_RET_SOURCE2_NEEDRETRY);
        return VO_RET_SOURCE2_NEEDRETRY;
    }
    else
	{
		return nRC;
	}
}

int COSAdapStrSource::LoadDll()
{
	if(m_pLibop == NULL)
	{
		VOLOGI("Load library Operator is NULL");
		return -1;
	}
	
	if (m_hDll != NULL)
	{
		m_pLibop->FreeLib(m_pLibop->pUserData, m_hDll, 0);
		m_hDll = NULL;
	}

	m_hDll = m_pLibop->LoadLib(m_pLibop->pUserData, "voAdaptiveStreamController", 0);

	if (m_hDll == NULL)
	{
		VOLOGI("Load library libvoAdaptiveStreamController failed");
		return -1;
	}

	VOGETSRCFRAPI pGetAPI = (VOGETSRCFRAPI) m_pLibop->GetAddress (m_pLibop->pUserData, m_hDll, "voGetAdaptiveStreamControllerAPI", 0);

	if (pGetAPI == NULL)
	{
		VOLOGI("get API voGetAdaptiveStreamControllerAPI failed");
		return -1;
	}

	pGetAPI(&m_SourceAPI);

	if(m_SourceAPI.Init == NULL)
	{
		VOLOGI("m_SourceAPI.Init is null");
		return -1;
	}

	return 0;
}

int COSAdapStrSource::GetMediaCount(int ssType)
{
	int nRC = UpdateTrackInfo();

	if(nRC) return 0;

	if(ssType == VOOSMP_SS_AUDIO)
	{
		return m_nAudioCount;
	}
	else if(ssType == VOOSMP_SS_VIDEO)
	{
		return m_nVideoCount;
	}
	else if(ssType == VOOSMP_SS_SUBTITLE)
	{
		return m_nSubtitleCount;
	}
	else
	{
		return 0;
	}
}

int COSAdapStrSource::SelectMediaIndex(int ssType, int nIndex)
{
	VOLOGI("@@@### ssType = %d, nIndex = %d, ACount: %d, VCount: %d, SCount: %d", ssType, nIndex, m_nAudioCount, m_nVideoCount, m_nSubtitleCount);

	int nRC = VOOSMP_ERR_ParamID;
	if ( nIndex < -1 ) return nRC;

	m_nIsCommitSelectTrack = 0;

	if(ssType == VOOSMP_SS_AUDIO)
	{
		if ( nIndex > m_nAudioCount ) return nRC;

		m_nAudioSelected = nIndex;
		m_nCurSelectedAudioTrack = nIndex;
	}
	else if(ssType == VOOSMP_SS_VIDEO)
	{
		if ( nIndex > m_nVideoCount ) return nRC;

		m_nVideoSelected = nIndex;
		m_nCurSelectedVideoTrack = nIndex;
	}
	else if(ssType == VOOSMP_SS_SUBTITLE)
	{
		if ( nIndex > m_nSubtitleCount ) return nRC;

		m_nSubtitleSelected = nIndex;
		m_nCurSelectedSubtitleTrack = nIndex;
	}
	else
	{
		return VOOSMP_ERR_Implement;
	}

	return VOOSMP_ERR_None;
}

int COSAdapStrSource::IsMediaIndexAvailable(int ssType, int nIndex)
{
	int nRC = 0;

	int nAudioSelectTrack = m_nAudioSelected;
	int	nVideoSelectTrack = m_nVideoSelected;
	int nSubTitleSelectTrack = m_nSubtitleSelected;

	if(ssType == VOOSMP_SS_SUBTITLE)
	{
		nSubTitleSelectTrack = nIndex;
		/* return IsSubtitleIndexAvaialbe( nIndex ); */
	}
	else if(ssType == VOOSMP_SS_AUDIO)
	{
		return IsAudioIndexAvaialbe ( nIndex );
	}
	else if(ssType == VOOSMP_SS_VIDEO)
	{
		return IsVideoIndexAvaialbe ( nIndex );
	}

	int nStream = IsAvaialbeIndex(&nAudioSelectTrack, &nVideoSelectTrack, &nSubTitleSelectTrack);

	if(nStream)
		return 1;
	
	return nRC;
}

int COSAdapStrSource::GetMediaProperty(int ssType, int nIndex, VOOSMP_SRC_TRACK_PROPERTY **pProperty)
{
	if(nIndex < 0)
		return VOOSMP_ERR_Implement;

	int nRC = UpdateTrackInfo();
	if(nRC) return nRC;

	int nTrackIndex = -1;

	if(ssType == VOOSMP_SS_AUDIO)
	{
		nTrackIndex = m_pAudioTrackIndex[nIndex];
		ClearPropertyItem(&m_szAudioProtery);
	}
	else if(ssType == VOOSMP_SS_VIDEO)
	{
		nTrackIndex = m_pVideoTrackIndex[nIndex];
		ClearPropertyItem(&m_szVideoProtery);
	}
	else if(ssType == VOOSMP_SS_SUBTITLE)
	{
		nTrackIndex = m_pSubtitleTrackIndex[nIndex];

		ClearPropertyItem(&m_szSubtitleProtery);
	}
	else
	{
		return VOOSMP_ERR_Implement;
	}


	VO_SOURCE2_STREAM_INFO* pStreamInfo = NULL;
	VO_SOURCE2_TRACK_INFO* pTrackInfo = NULL;	
	
	unsigned int i, j;

	if(m_pProgramInfo == NULL || m_pProgramInfo->ppStreamInfo == NULL)
		return VOOSMP_ERR_Implement;

	for(i = 0; i < m_pProgramInfo->uStreamCount; i++)
	{
		pStreamInfo = m_pProgramInfo->ppStreamInfo[i];

		if(pStreamInfo == NULL || pStreamInfo->ppTrackInfo == NULL)
			continue;

		if(ssType == VOOSMP_SS_VIDEO)
		{
			if(nTrackIndex != (int)pStreamInfo->uStreamID)
				continue;
		}

		for(j = 0; j < pStreamInfo->uTrackCount; j++)
		{
			pTrackInfo = pStreamInfo->ppTrackInfo[j];

			if(pTrackInfo == NULL)
				continue;

			if(ssType == VOOSMP_SS_VIDEO)
			{
				if(pTrackInfo->uTrackType == VOOSMP_SS_VIDEO || pTrackInfo->uTrackType == VOOSMP_SS_VIDEO_GROUP)
				{
					m_szVideoProtery.nPropertyCount = 5;
					m_szVideoProtery.ppItemProperties = new VOOSMP_SRC_TRACK_ITEM_PROPERTY*[5];
					if(m_szVideoProtery.ppItemProperties == NULL)
						return VOOSMP_ERR_OutMemory;

					m_szVideoProtery.ppItemProperties[0] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szVideoProtery.ppItemProperties[0] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szVideoProtery.ppItemProperties[0], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szVideoProtery.ppItemProperties[0]->szKey, "description");

					m_szVideoProtery.ppItemProperties[0]->pszProperty = new char[32];
					if(m_szVideoProtery.ppItemProperties[0]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szVideoProtery.ppItemProperties[0]->pszProperty, 0, sizeof(char)*32);
					if(strlen(pTrackInfo->sVideoInfo.strVideoDesc) > 0)
					{
						strcpy(m_szVideoProtery.ppItemProperties[0]->pszProperty, pTrackInfo->sVideoInfo.strVideoDesc);

						VOLOGI("video description: %s, strlen %d, pTrackInfo->sVideoInfo.strVideoDesc[0] %d", m_szVideoProtery.ppItemProperties[0]->pszProperty, strlen(pTrackInfo->sVideoInfo.strVideoDesc), 
							pTrackInfo->sVideoInfo.strVideoDesc[0]);
					}
					else
						sprintf(m_szVideoProtery.ppItemProperties[0]->pszProperty, "Video %d", nIndex);


					m_szVideoProtery.ppItemProperties[1] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szVideoProtery.ppItemProperties[1] == NULL)
						return VOOSMP_ERR_OutMemory;
						
					memset(m_szVideoProtery.ppItemProperties[1], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szVideoProtery.ppItemProperties[1]->szKey, "codec");

					m_szVideoProtery.ppItemProperties[1]->pszProperty = new char[32];
					if(m_szVideoProtery.ppItemProperties[1]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szVideoProtery.ppItemProperties[1]->pszProperty, 0, sizeof(char)*32);
					strcpy(m_szVideoProtery.ppItemProperties[1]->pszProperty, VideoCodec[pTrackInfo->uCodec]);

					m_szVideoProtery.ppItemProperties[2] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szVideoProtery.ppItemProperties[2] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szVideoProtery.ppItemProperties[2], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szVideoProtery.ppItemProperties[2]->szKey, "bitrate");

					m_szVideoProtery.ppItemProperties[2]->pszProperty = new char[32];
					if(m_szVideoProtery.ppItemProperties[2]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szVideoProtery.ppItemProperties[2]->pszProperty, 0, sizeof(char)*32);

					if(pTrackInfo->uBitrate)
						sprintf(m_szVideoProtery.ppItemProperties[2]->pszProperty, "%d bps", (int)pTrackInfo->uBitrate);
					else
						sprintf(m_szVideoProtery.ppItemProperties[2]->pszProperty, "%d bps", (int)pStreamInfo->uBitrate);


					m_szVideoProtery.ppItemProperties[3] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szVideoProtery.ppItemProperties[3] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szVideoProtery.ppItemProperties[3], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szVideoProtery.ppItemProperties[3]->szKey, "width");

					m_szVideoProtery.ppItemProperties[3]->pszProperty = new char[32];
					if(m_szVideoProtery.ppItemProperties[3]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szVideoProtery.ppItemProperties[3]->pszProperty, 0, sizeof(char)*32);
					sprintf(m_szVideoProtery.ppItemProperties[3]->pszProperty, "%d", (int)pTrackInfo->sVideoInfo.sFormat.Width);

					m_szVideoProtery.ppItemProperties[4] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szVideoProtery.ppItemProperties[4] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szVideoProtery.ppItemProperties[4], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szVideoProtery.ppItemProperties[4]->szKey, "height");

					m_szVideoProtery.ppItemProperties[4]->pszProperty = new char[32];
					if(m_szVideoProtery.ppItemProperties[4]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szVideoProtery.ppItemProperties[4]->pszProperty, 0, sizeof(char)*32);
					sprintf(m_szVideoProtery.ppItemProperties[4]->pszProperty, "%d", (int)pTrackInfo->sVideoInfo.sFormat.Height);

					*pProperty = &m_szVideoProtery;

					return 0;
				}

				continue;
			}

			if((int)pTrackInfo->uTrackID == nTrackIndex)
			{
				if(pTrackInfo->uTrackType == VOOSMP_SS_AUDIO || pTrackInfo->uTrackType == VOOSMP_SS_AUDIO_GROUP)
				{
					m_szAudioProtery.nPropertyCount = 5;

					m_szAudioProtery.ppItemProperties = new VOOSMP_SRC_TRACK_ITEM_PROPERTY*[5];
					if(m_szAudioProtery.ppItemProperties == NULL)
						return VOOSMP_ERR_OutMemory;

					m_szAudioProtery.ppItemProperties[0] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szAudioProtery.ppItemProperties[0] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szAudioProtery.ppItemProperties[0], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szAudioProtery.ppItemProperties[0]->szKey, "description");

					m_szAudioProtery.ppItemProperties[0]->pszProperty = new char[32];
					if(m_szAudioProtery.ppItemProperties[0]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szAudioProtery.ppItemProperties[0]->pszProperty, 0, sizeof(char)*32);
					sprintf(m_szAudioProtery.ppItemProperties[0]->pszProperty, "Audio %d", nIndex);


					m_szAudioProtery.ppItemProperties[1] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szAudioProtery.ppItemProperties[1] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szAudioProtery.ppItemProperties[1], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szAudioProtery.ppItemProperties[1]->szKey, "language");

					m_szAudioProtery.ppItemProperties[1]->pszProperty = new char[32];
					if(m_szAudioProtery.ppItemProperties[1]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szAudioProtery.ppItemProperties[1]->pszProperty, 0, sizeof(char)*32);

					if(strlen(pTrackInfo->sAudioInfo.chLanguage) > 0)
						strcpy(m_szAudioProtery.ppItemProperties[1]->pszProperty, (const char *)pTrackInfo->sAudioInfo.chLanguage);
					else
						strcpy(m_szAudioProtery.ppItemProperties[1]->pszProperty, "unknown");

					m_szAudioProtery.ppItemProperties[2] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szAudioProtery.ppItemProperties[2] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szAudioProtery.ppItemProperties[2], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szAudioProtery.ppItemProperties[2]->szKey, "codec");

					m_szAudioProtery.ppItemProperties[2]->pszProperty = new char[32];
					if(m_szAudioProtery.ppItemProperties[2]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szAudioProtery.ppItemProperties[2]->pszProperty, 0, sizeof(char)*32);
					strcpy(m_szAudioProtery.ppItemProperties[2]->pszProperty, AudioCodec[pTrackInfo->uCodec]);

					/* bitrate */
					m_szAudioProtery.ppItemProperties[3] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szAudioProtery.ppItemProperties[3] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szAudioProtery.ppItemProperties[3], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));
					strcpy(m_szAudioProtery.ppItemProperties[3]->szKey, "bitrate");
					m_szAudioProtery.ppItemProperties[3]->pszProperty = new char[32];
					if(m_szAudioProtery.ppItemProperties[3]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szAudioProtery.ppItemProperties[3]->pszProperty, 0, sizeof(char)*32);
					sprintf(m_szAudioProtery.ppItemProperties[3]->pszProperty, "%d", (int)pTrackInfo->uBitrate);

					/* channel count */
					m_szAudioProtery.ppItemProperties[4] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szAudioProtery.ppItemProperties[4] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szAudioProtery.ppItemProperties[4], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));
					strcpy(m_szAudioProtery.ppItemProperties[4]->szKey, "channelcount");
					m_szAudioProtery.ppItemProperties[4]->pszProperty = new char[32];
					if(m_szAudioProtery.ppItemProperties[4]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szAudioProtery.ppItemProperties[4]->pszProperty, 0, sizeof(char)*32);
					sprintf(m_szAudioProtery.ppItemProperties[4]->pszProperty, "%d", (int)pTrackInfo->sAudioInfo.sFormat.Channels);

					*pProperty = &m_szAudioProtery;

					return 0;
				}
				else if(pTrackInfo->uTrackType == VOOSMP_SS_SUBTITLE || pTrackInfo->uTrackType == VOOSMP_SS_SUBTITLE_GROUP)
				{
					m_szSubtitleProtery.nPropertyCount = 3;

					m_szSubtitleProtery.ppItemProperties = new VOOSMP_SRC_TRACK_ITEM_PROPERTY*[3];
					if(m_szSubtitleProtery.ppItemProperties == NULL)
						return VOOSMP_ERR_OutMemory;

					m_szSubtitleProtery.ppItemProperties[0] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szSubtitleProtery.ppItemProperties[0] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szSubtitleProtery.ppItemProperties[0], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szSubtitleProtery.ppItemProperties[0]->szKey, "description");

					m_szSubtitleProtery.ppItemProperties[0]->pszProperty = new char[32];
					if(m_szSubtitleProtery.ppItemProperties[0]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szSubtitleProtery.ppItemProperties[0]->pszProperty, 0, sizeof(char)*32);
					sprintf(m_szSubtitleProtery.ppItemProperties[0]->pszProperty, "SubTitle %d", nIndex);


					m_szSubtitleProtery.ppItemProperties[1] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szSubtitleProtery.ppItemProperties[1] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szSubtitleProtery.ppItemProperties[1], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szSubtitleProtery.ppItemProperties[1]->szKey, "language");

					m_szSubtitleProtery.ppItemProperties[1]->pszProperty = new char[32];
					if(m_szSubtitleProtery.ppItemProperties[1]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szSubtitleProtery.ppItemProperties[1]->pszProperty, 0, sizeof(char)*32);

					if(strlen(pTrackInfo->sSubtitleInfo.chLanguage) > 0)
						strcpy(m_szSubtitleProtery.ppItemProperties[1]->pszProperty, (const char *)pTrackInfo->sSubtitleInfo.chLanguage);
					else
						strcpy(m_szSubtitleProtery.ppItemProperties[1]->pszProperty, "unknown");

					m_szSubtitleProtery.ppItemProperties[2] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szSubtitleProtery.ppItemProperties[2] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szSubtitleProtery.ppItemProperties[2], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szSubtitleProtery.ppItemProperties[2]->szKey, "codec");

					m_szSubtitleProtery.ppItemProperties[2]->pszProperty = new char[32];
					if(m_szSubtitleProtery.ppItemProperties[2]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szSubtitleProtery.ppItemProperties[2]->pszProperty, 0, sizeof(char)*32);
					strcpy(m_szSubtitleProtery.ppItemProperties[2]->pszProperty, "unknown");

					*pProperty = &m_szSubtitleProtery;

					return 0;
				}
			}

		}

		if(ssType == VOOSMP_SS_VIDEO)
		{
			if(nTrackIndex == (int)pStreamInfo->uStreamID)
			{
				m_szVideoProtery.nPropertyCount = 5;
				m_szVideoProtery.ppItemProperties = new VOOSMP_SRC_TRACK_ITEM_PROPERTY*[5];
				if(m_szVideoProtery.ppItemProperties == NULL)
					return VOOSMP_ERR_OutMemory;

				m_szVideoProtery.ppItemProperties[0] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
				if(m_szVideoProtery.ppItemProperties[0] == NULL)
					return VOOSMP_ERR_OutMemory;
				memset(m_szVideoProtery.ppItemProperties[0], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

				strcpy(m_szVideoProtery.ppItemProperties[0]->szKey, "description");

				m_szVideoProtery.ppItemProperties[0]->pszProperty = new char[32];
				if(m_szVideoProtery.ppItemProperties[0]->pszProperty == NULL)
					return VOOSMP_ERR_OutMemory;
				memset(m_szVideoProtery.ppItemProperties[0]->pszProperty, 0, sizeof(char)*32);
				strcpy(m_szVideoProtery.ppItemProperties[0]->pszProperty, "unknown");

				m_szVideoProtery.ppItemProperties[1] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
				if(m_szVideoProtery.ppItemProperties[1] == NULL)
					return VOOSMP_ERR_OutMemory;
				memset(m_szVideoProtery.ppItemProperties[1], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

				strcpy(m_szVideoProtery.ppItemProperties[1]->szKey, "codec");

				m_szVideoProtery.ppItemProperties[1]->pszProperty = new char[32];
				if(m_szVideoProtery.ppItemProperties[1]->pszProperty == NULL)
					return VOOSMP_ERR_OutMemory;
				memset(m_szVideoProtery.ppItemProperties[1]->pszProperty, 0, sizeof(char)*32);
				strcpy(m_szVideoProtery.ppItemProperties[1]->pszProperty, VideoCodec[0]);

				m_szVideoProtery.ppItemProperties[2] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
				if(m_szVideoProtery.ppItemProperties[2] == NULL)
					return VOOSMP_ERR_OutMemory;
				memset(m_szVideoProtery.ppItemProperties[2], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

				strcpy(m_szVideoProtery.ppItemProperties[2]->szKey, "bitrate");

				m_szVideoProtery.ppItemProperties[2]->pszProperty = new char[32];
				if(m_szVideoProtery.ppItemProperties[2]->pszProperty == NULL)
					return VOOSMP_ERR_OutMemory;
				memset(m_szVideoProtery.ppItemProperties[2]->pszProperty, 0, sizeof(char)*32);
				sprintf(m_szVideoProtery.ppItemProperties[2]->pszProperty, "%d bps", (int)pStreamInfo->uBitrate);

				m_szVideoProtery.ppItemProperties[3] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
				if(m_szVideoProtery.ppItemProperties[3] == NULL)
					return VOOSMP_ERR_OutMemory;
				memset(m_szVideoProtery.ppItemProperties[3], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

				strcpy(m_szVideoProtery.ppItemProperties[3]->szKey, "width");
				m_szVideoProtery.ppItemProperties[3]->pszProperty = new char[32];
				if(m_szVideoProtery.ppItemProperties[3]->pszProperty == NULL)
					return VOOSMP_ERR_OutMemory;
				memset(m_szVideoProtery.ppItemProperties[3]->pszProperty, 0, sizeof(char)*32);
				strcpy(m_szVideoProtery.ppItemProperties[3]->pszProperty, "unknown");

				m_szVideoProtery.ppItemProperties[4] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
				if(m_szVideoProtery.ppItemProperties[4] == NULL)
					return VOOSMP_ERR_OutMemory;
				memset(m_szVideoProtery.ppItemProperties[4], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

				strcpy(m_szVideoProtery.ppItemProperties[4]->szKey, "height");
				m_szVideoProtery.ppItemProperties[4]->pszProperty = new char[32];
				if(m_szVideoProtery.ppItemProperties[4]->pszProperty == NULL)
					return VOOSMP_ERR_OutMemory;
				memset(m_szVideoProtery.ppItemProperties[4]->pszProperty, 0, sizeof(char)*32);
				strcpy(m_szVideoProtery.ppItemProperties[4]->pszProperty, "unknown");

				*pProperty = &m_szVideoProtery;

				return 0;
			}
		}
	}

	return VOOSMP_ERR_Implement;
}

int COSAdapStrSource::CommetSelection(int bSelect)
{
	VOLOGI("@@@### CommetSelection");
	if(bSelect)
	{
		VOOSMP_SRC_CURR_TRACK_INDEX mCurIndex;		
		int nRC = GetCurMediaTrack(&mCurIndex);
		if(nRC) return nRC;
		
		int nAudioSelect = -1;
		int nVideoSelect = -1;
		int nSubTitleSelect = -1;
		int nStream = 0;

		int nAudioSelectBak = 0;
		int nSubtitleSelectBak = 0;

		if(m_nVideoSelected == -2)
		{
			nVideoSelect = mCurIndex.nCurrVideoIdx;
		}
		else
		{
			nVideoSelect = m_nVideoSelected;
		}
        
		if(m_nAudioSelected == -2)
		{
			nAudioSelect = mCurIndex.nCurrAudioIdx;
		}
		else
		{
			nAudioSelect = m_nAudioSelected;
		}

		if(m_nSubtitleSelected == -2)
		{
			nSubTitleSelect = mCurIndex.nCurrSubtitleIdx;
		}
		else
		{
			nSubTitleSelect = m_nSubtitleSelected;
		}

		nAudioSelectBak = nAudioSelect;
		nSubtitleSelectBak = nSubTitleSelect;

		VOLOGI ("nVideoSelect %d nAudioSelect %d, nSubTitleSelect %d", nVideoSelect, nAudioSelect, nSubTitleSelect);
		
		nStream = IsAvaialbeIndex(&nAudioSelect, &nVideoSelect, &nSubTitleSelect);

		VOLOGI("@@@### nStream = %d, m_nVideoSelected = %d", nStream, m_nVideoSelected);
		if(nStream == 0 )
		{
			if ( m_nVideoSelected != -2 )
			{
				nAudioSelect = -1;
				nSubTitleSelect = -1;

				if(m_nAudioSelected >= 0)
				{
					nAudioSelect = m_nAudioSelected;
				}

				if(m_nSubtitleSelected >= 0)
				{
					nSubTitleSelect = m_nSubtitleSelected;
				}
				
				nStream = IsAvaialbeIndex(&nAudioSelect, &nVideoSelect, &nSubTitleSelect);
				VOLOGI("@@@### nStream = %d", nStream);
			}
			else if ( m_nVideoSelected == -2 )
			{
				VOLOGI("@@@### AudiaoBak = %d, Subtitlebak = %d", nAudioSelectBak, nSubtitleSelectBak);
				int nRet = GetAutoSelectStreamingAudioSubtitleID(&nAudioSelectBak, &nSubtitleSelectBak);
				if (nRet == VOOSMP_ERR_None) 
				{
					nStream = 1;
					nAudioSelect = nAudioSelectBak;
					nSubTitleSelect = nSubtitleSelectBak;
				}
			}
		}

		if(nStream)
		{
			//int nProgramIndex = (nStream&0xFF0000) >> 16;

			//nRC = m_pBaseSource->SelectProgram(nProgramIndex);
			//if(nRC) return nRC;

			VOLOGI ("nVideoSelect %d nAudioSelect %d, nSubTitleSelect %d", nVideoSelect, nAudioSelect, nSubTitleSelect);
			nRC = SelectStream(nVideoSelect);
			if(nRC != VOOSMP_ERR_None && (unsigned int)nRC != VOOSMP_ERR_Implement) return nRC;

			if(m_nAudioSelected >= 0)
			{
				//if(m_nVideoSelected == -2)
				//{
					VOLOGI ("nAudioSelect %d, trackID %d", m_nAudioSelected, nAudioSelect);
					nRC = SelectTrack(nAudioSelect);
					if(nRC != VOOSMP_ERR_None && (unsigned int)nRC != VOOSMP_ERR_Implement) return nRC;
				//}
				//else
				//{
				//	VOLOGI ("nAudioSelect %d, language %s", m_nAudioSelected, m_pAudioTrackLan[m_nAudioSelected]);
				//	nRC = m_pBaseSource->SetParam(VO_PID_SOURCE2_AUDIOLANGUAGE, m_pAudioTrackLan[m_nAudioSelected]);
				//	if(nRC != VOOSMP_ERR_None && (unsigned int)nRC != VOOSMP_ERR_Implement) return nRC;
				//}
			}

			if(m_nSubtitleSelected >= 0 && m_nSubtitleCount)
			{
				//if(m_nVideoSelected == -2)
				//{
					VOLOGI ("nSubTitleSelect %d, trackID %d", m_nSubtitleSelected, nSubTitleSelect);
					nRC = SelectTrack(nSubTitleSelect);
					if(nRC != VOOSMP_ERR_None && (unsigned int)nRC != VOOSMP_ERR_Implement) return nRC;					
				//}
				//else
				//{
				//	VOLOGI ("nSubTitleSelect %d, language %s", m_nSubtitleSelected, m_pSubtitleTrackLan[m_nSubtitleSelected]);
				//	nRC = m_pBaseSource->SetParam(VO_PID_SOURCE2_SUBTITLELANGUAGE, m_pSubtitleTrackLan[m_nSubtitleSelected]);
				//	if(nRC != VOOSMP_ERR_None && (unsigned int)nRC != VOOSMP_ERR_Implement) return nRC;
				//}
			}

			if(m_nVideoSelected >= 0 || (m_nVideoSelected == -2 && mCurIndex.nCurrVideoIdx >= 0))
			{
				m_nBAStreamEnable = 0;
			}
			else
			{
				nRC = SelectStream(nVideoSelect);
				if(nRC != VOOSMP_ERR_None && (unsigned int)nRC != VOOSMP_ERR_Implement) return nRC;
				m_nBAStreamEnable = 1;
			}
		}
		else
		{
			return VOOSMP_ERR_ParamID;
		}
	}

	m_nAudioSelected = -2;
	m_nVideoSelected = -2;
	m_nSubtitleSelected = -2;

	m_nCurSelectedNoCommitAudioTrack = m_nCurSelectedAudioTrack;
	m_nCurSelectedNoCommitVideoTrack = m_nCurSelectedVideoTrack;
	m_nCurSelectedNoCommitSubtitleTrack = m_nCurSelectedSubtitleTrack;

	m_nIsCommitSelectTrack = 1;

	return VOOSMP_ERR_None;
}

int COSAdapStrSource::GetCurMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex)
{
	VOLOGI("@@@### GetCurMediaTrack");
	if(pCurIndex == NULL)
		return VOOSMP_ERR_Pointer;

	int nRC = VOOSMP_ERR_Implement;

	nRC = UpdateTrackInfo();
	if(nRC) return nRC;

	m_nVideoCurTrack = -1;
	m_nAudioCurTrack = -1;
	m_nSubtitleCurTrack = -1;
	pCurIndex->nCurrVideoIdx = -1;
	pCurIndex->nCurrAudioIdx = -1;
	pCurIndex->nCurrSubtitleIdx = -1;


	VO_SOURCE2_STREAM_INFO* pStreamInfo = NULL;
	VO_SOURCE2_TRACK_INFO* pTrackInfo = NULL;		
	unsigned int i, j;
	int n;

	if(m_pProgramInfo == NULL || m_pProgramInfo->ppStreamInfo == NULL)
		return VOOSMP_ERR_Implement;

	for(i = 0; i < m_pProgramInfo->uStreamCount; i++)
	{
		pStreamInfo = m_pProgramInfo->ppStreamInfo[i];

		if(pStreamInfo == NULL || pStreamInfo->ppTrackInfo == NULL)
			continue;

		if(!(pStreamInfo->uSelInfo&VOOSMP_SRC_TRACK_SELECT_SELECTED))
			continue;

		if(m_nBAStreamEnable == 0)
		{
			for(n = 0; n < m_nVideoCount; n++)
			{
				if((int)pStreamInfo->uStreamID == m_pVideoTrackIndex[n])
				{
					m_nVideoCurTrack = pStreamInfo->uStreamID;
					pCurIndex->nCurrVideoIdx = n;
					break;
				}
			}
		}

		for(j = 0; j < pStreamInfo->uTrackCount; j++)
		{
			pTrackInfo = pStreamInfo->ppTrackInfo[j];

			if(pTrackInfo == NULL)
				continue;

			if(pTrackInfo->uSelInfo&VOOSMP_SRC_TRACK_SELECT_SELECTED)
			{
				if(pTrackInfo->uTrackType == VO_SOURCE2_TT_AUDIO || pTrackInfo->uTrackType == VO_SOURCE2_TT_AUDIOGROUP)
				{
					for(n = 0; n < m_nAudioCount; n++)
					{
						if((int)pTrackInfo->uTrackID == m_pAudioTrackIndex[n])
						{
							m_nAudioCurTrack = pTrackInfo->uTrackID;
							pCurIndex->nCurrAudioIdx = n;
							break;
						}
					}

					if(n == m_nAudioCount)
					{
						for(n = 0; n < m_nAudioCount; n++)
						{
							if(strcmp(pTrackInfo->sAudioInfo.chLanguage, m_pAudioTrackLan[n]) == 0)
							{
								m_nAudioCurTrack = pTrackInfo->uTrackID;
								pCurIndex->nCurrAudioIdx = n;
								break;
							}
						}
					}
				}
				else if(pTrackInfo->uTrackType == VO_SOURCE2_TT_SUBTITLE || pTrackInfo->uTrackType == VO_SOURCE2_TT_SUBTITLEGROUP)
				{
					for(n = 0; n < m_nSubtitleCount; n++)
					{
						if((int)pTrackInfo->uTrackID == m_pSubtitleTrackIndex[n])
						{
							m_nSubtitleCurTrack = pTrackInfo->uTrackID;
							pCurIndex->nCurrSubtitleIdx = n;
							break;
						}
					}

					if(n == m_nSubtitleCount)
					{
						for(n = 0; n < m_nAudioCount; n++)
						{
							if(strcmp(pTrackInfo->sSubtitleInfo.chLanguage, m_pSubtitleTrackLan[n]) == 0)
							{
								m_nSubtitleCurTrack = pTrackInfo->uTrackID;
								pCurIndex->nCurrSubtitleIdx = n;
								break;
							}
						}
					}
				}
			}
		}
	}

	VOLOGI("@@@###  CurPlayingA = %d, CurPlayingV = %d. CurPlayingS = %d", m_nCurPlayingAudioTrack, m_nCurPlayingVideoTrack, m_nCurPlayingSubtitleTrack);

	return VOOSMP_ERR_None;
}

int COSAdapStrSource::GetCurPlayingMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex)
{
	VOLOGI("@@@### GetCurPlayingMediaTrack");
	if (!pCurIndex)
		return VOOSMP_ERR_Pointer;

	int nRC = UpdateTrackInfo();
	if(nRC) return nRC;

	VOLOGI("@@@###  CurA = %d, CurV = %d. CurS = %d", pCurIndex->nCurrAudioIdx, pCurIndex->nCurrVideoIdx, pCurIndex->nCurrSubtitleIdx);
	pCurIndex->nCurrAudioIdx = m_nCurPlayingAudioTrack;
	pCurIndex->nCurrVideoIdx= m_nCurPlayingVideoTrack;
	pCurIndex->nCurrSubtitleIdx = m_nCurPlayingSubtitleTrack;

	return VOOSMP_ERR_None;
}

int COSAdapStrSource::GetCurSelectedMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex)
{
	VOLOGI("@@@### GetCurSelectedMediaTrack");
	if (!pCurIndex)
		return VOOSMP_ERR_Pointer;

	if ( 0 == m_nIsCommitSelectTrack )
	{
		VOLOGI("@@@### 1  CurSelectedA = %d, CurSelectedV = %d. CurSelectedS = %d", m_nCurSelectedNoCommitAudioTrack, m_nCurSelectedNoCommitVideoTrack, m_nCurSelectedNoCommitSubtitleTrack);
		pCurIndex->nCurrAudioIdx = m_nCurSelectedNoCommitAudioTrack;
		pCurIndex->nCurrVideoIdx= m_nCurSelectedNoCommitVideoTrack;
		pCurIndex->nCurrSubtitleIdx= m_nCurSelectedNoCommitSubtitleTrack;
	}
	else
	{
		VOLOGI("@@@### 2 CurSelectedA = %d, CurSelectedV = %d. CurSelectedS = %d", m_nCurSelectedAudioTrack, m_nCurSelectedVideoTrack, m_nCurSelectedSubtitleTrack);
		pCurIndex->nCurrAudioIdx = m_nCurSelectedAudioTrack;
		pCurIndex->nCurrVideoIdx= m_nCurSelectedVideoTrack;
		pCurIndex->nCurrSubtitleIdx = m_nCurSelectedSubtitleTrack;
	}

	return VOOSMP_ERR_None;
}

//portected:

int COSAdapStrSource::UpdateTrackInfo()
{
	m_nAudioCount = 0;
	m_nVideoCount = 0;
	m_nSubtitleCount = 0;
	m_nExtendSubtitle = 0;

	int nRC = VOOSMP_ERR_Implement;

	VO_SOURCE2_STREAM_INFO* pStreamInfo = NULL;
	VO_SOURCE2_TRACK_INFO* pTrackInfo = NULL;

	unsigned int i, j;
	//int hasVideo = 0;
	//int hasAudio = 0;
	int m = 0;
	int nFound = 0;

	memset(m_pAudioTrackIndex, 0, sizeof(int)*128);
	memset(m_pVideoTrackIndex, 0, sizeof(int)*128);
	memset(m_pSubtitleTrackIndex, 0, sizeof(int)*128);
	memset(m_pAudioTrackLan, 0, sizeof(char)*2*256);
	memset(m_pSubtitleTrackLan, 0, sizeof(char)*2*256);

	nRC = GetProgramInfo(0, &m_pProgramInfo);
	if(nRC) return nRC;

	if(m_pProgramInfo == NULL || m_pProgramInfo->ppStreamInfo == NULL)
		return nRC;

	for(i = 0; i < m_pProgramInfo->uStreamCount; i++)
	{
		pStreamInfo = m_pProgramInfo->ppStreamInfo[i];

		if( pStreamInfo->uSelInfo & VOOSMP_SRC_TRACK_SELECT_SELECTED )
			m_nCurPlayingVideoTrack = m_nVideoCount;

		m_pVideoTrackIndex[m_nVideoCount] = (int)pStreamInfo->uStreamID;
		m_nVideoCount += 1;		

		if(pStreamInfo == NULL || pStreamInfo->ppTrackInfo == NULL)
			continue;

		//hasVideo = 0;
		//hasAudio = 0;

		for(j = 0; j < pStreamInfo->uTrackCount; j++)
		{
			pTrackInfo = pStreamInfo->ppTrackInfo[j];

			if(pTrackInfo == NULL)
				continue;

			if(pTrackInfo->uTrackType == VOOSMP_SS_AUDIO || pTrackInfo->uTrackType == VOOSMP_SS_AUDIO_GROUP)
			{
				nFound  = 0;

				for(m = 0; m < m_nAudioCount; m++)
				{
					if(m_pAudioTrackIndex[m] == (int)pTrackInfo->uTrackID)
					{
						nFound = 1;
						break;
					}

					if(strlen(pTrackInfo->sAudioInfo.chLanguage) > 0)
					{
						if(strcmp(m_pAudioTrackLan[m], pTrackInfo->sAudioInfo.chLanguage) == 0)
						{
							nFound = 1;
							break;
						}
					}
				}

				if(nFound == 0)
				{
					m_pAudioTrackIndex[m_nAudioCount] = (int)pTrackInfo->uTrackID;
					strcpy(m_pAudioTrackLan[m_nAudioCount], pTrackInfo->sAudioInfo.chLanguage);
					VOLOGI ("@@@@###  pStreamInfoID = %d, m_nAudioCount %d, pTrackInfo->uTrackID %d, language %s", pStreamInfo->uStreamID ,m_nAudioCount, pTrackInfo->uTrackID, m_pAudioTrackLan[m_nAudioCount]);
					m_nAudioCount += 1;
				}

				if( pTrackInfo->uSelInfo & VOOSMP_SRC_TRACK_SELECT_SELECTED )
				{
					if (nFound )
						m_nCurPlayingAudioTrack = m;
					else
						m_nCurPlayingAudioTrack = m_nAudioCount - 1;
				}

				//hasAudio = 1;
			}
			else if((pTrackInfo->uTrackType == VOOSMP_SS_SUBTITLE || pTrackInfo->uTrackType == VOOSMP_SS_SUBTITLE_GROUP))
			{
				nFound  = 0;

				for(m = 0; m < m_nSubtitleCount; m++)
				{
					if(m_pSubtitleTrackIndex[m] == (int)pTrackInfo->uTrackID)
					{
						nFound = 1;
						break;
					}

					if(strlen(pTrackInfo->sSubtitleInfo.chLanguage) > 0)
					{
						if(strcmp(m_pSubtitleTrackLan[m], pTrackInfo->sSubtitleInfo.chLanguage) == 0)
						{
							nFound = 1;
							break;
						}
					}
				}

				if(nFound == 0)
				{
					m_pSubtitleTrackIndex[m_nSubtitleCount] = (int)pTrackInfo->uTrackID;
					strcpy(m_pSubtitleTrackLan[m_nSubtitleCount], pTrackInfo->sSubtitleInfo.chLanguage);
					m_nSubtitleCount += 1;
				}

				if( pTrackInfo->uSelInfo & VOOSMP_SRC_TRACK_SELECT_SELECTED )
				{
					if ( nFound )
						m_nCurPlayingSubtitleTrack= m;
					else
						m_nCurPlayingSubtitleTrack = m_nSubtitleCount - 1;
				}
			}
		}	
	}

	return VOOSMP_ERR_None;
}

int COSAdapStrSource::IsAvaialbeIndex(int *paIndex, int* pvIndex, int* psIndex)
{
	int nRC = 0;

	int nAudioSelectTrack = *paIndex;
	int	nVideoSelectTrack = *pvIndex;
	int nSubTitleSelectTrack = *psIndex;

	char sAudioTrackLan[256];
	char sSubtitleTrackLan[256];

	memset(sAudioTrackLan, 0, sizeof(char)*256);
	memset(sSubtitleTrackLan, 0, sizeof(char)*256);

	VOLOGI ("A %d, V %d, S %d", *paIndex, *pvIndex, *psIndex);

	nRC = UpdateTrackInfo();
	if(nRC) return 0;

	if(*paIndex >=0)
	{
		if(*paIndex < m_nAudioCount)
		{
			nAudioSelectTrack = m_pAudioTrackIndex[*paIndex];
			strcpy(sAudioTrackLan, m_pAudioTrackLan[*paIndex]);
		}
		else
		{
			return 0;
		}
	}

	if(*pvIndex >=0)
	{
		if(*pvIndex < m_nVideoCount)
		{
			nVideoSelectTrack = m_pVideoTrackIndex[*pvIndex];
		}
		else
		{
			return 0;
		}
	}

	if(*psIndex >=0)
	{
		if(*psIndex < m_nSubtitleCount)
		{
			nSubTitleSelectTrack = m_pSubtitleTrackIndex[*psIndex];
			strcpy(sSubtitleTrackLan, m_pSubtitleTrackLan[*psIndex]);
		}
		else
		{
			return 0;
		}
	}
	
	VO_SOURCE2_STREAM_INFO* pStreamInfo = NULL;
	VO_SOURCE2_TRACK_INFO* pTrackInfo = NULL;	
	
	unsigned int i, j;

	int nFoundA = 0;
	int nFoundV = 0;
	int nFoundS = 0;

	if(m_pProgramInfo == NULL || m_pProgramInfo->ppStreamInfo == NULL)
		return 0;

	for(i = 0; i < m_pProgramInfo->uStreamCount; i++)
	{
		pStreamInfo = m_pProgramInfo->ppStreamInfo[i];

		if(pStreamInfo == NULL || pStreamInfo->ppTrackInfo == NULL)
			continue;

		nFoundA = 0;
		nFoundV = 0;
		nFoundS = 0;

		if(m_nSubtitleCount == 0)		
			nFoundS = 1;
		if(nAudioSelectTrack < 0)		
			nFoundA = 1;
		if(nSubTitleSelectTrack < 0)	
			nFoundS = 1;
		if(nVideoSelectTrack < 0)		
			nFoundV = 1;

		if(nVideoSelectTrack >= 0 && (int)pStreamInfo->uStreamID != nVideoSelectTrack)
		{
			continue;
		}

		if((int)pStreamInfo->uStreamID == nVideoSelectTrack)
		{
			nFoundV = 1;
			*pvIndex = pStreamInfo->uStreamID;
		}

		if(nVideoSelectTrack == -1/* && (pStreamInfo->uSelInfo&VOOSMP_SRC_TRACK_SELECT_SELECTED)*/)
		{
			/* -1 value note that, this is a auto select streaming, keep -1 and set to Controller */
			*pvIndex = nVideoSelectTrack;
		}

		for(j = 0; j < pStreamInfo->uTrackCount; j++)
		{
			pTrackInfo = pStreamInfo->ppTrackInfo[j];

			if(pTrackInfo == NULL)
				continue;

			if(((int)pTrackInfo->uTrackType == VOOSMP_SS_AUDIO || (int)pTrackInfo->uTrackType == VOOSMP_SS_AUDIO_GROUP)) 
			{
				if((int)pTrackInfo->uTrackID == nAudioSelectTrack)
				{
					nFoundA = 1;
					
					if(nVideoSelectTrack == -1)
					{
						if(pStreamInfo->uSelInfo&VOOSMP_SRC_TRACK_SELECT_SELECTED)
							*paIndex = (int)pTrackInfo->uTrackID;
					}
					else
					{
						*paIndex = (int)pTrackInfo->uTrackID;
					}
					continue;
				}
				
				if(strlen(sAudioTrackLan) > 0 && strcmp(pTrackInfo->sAudioInfo.chLanguage, sAudioTrackLan) == 0)
				{
					nFoundA = 1;
					if(nVideoSelectTrack == -1)
					{
						if(pStreamInfo->uSelInfo&VOOSMP_SRC_TRACK_SELECT_SELECTED)
							*paIndex = (int)pTrackInfo->uTrackID;
					}
					else
					{
						*paIndex = (int)pTrackInfo->uTrackID;
					}
					continue;
				}
			}
			else if(((int)pTrackInfo->uTrackType == VOOSMP_SS_SUBTITLE || (int)pTrackInfo->uTrackType == VOOSMP_SS_SUBTITLE_GROUP))
			{
				if((int)pTrackInfo->uTrackID == nSubTitleSelectTrack)
				{
					nFoundS = 1;
					if(nVideoSelectTrack == -1)
					{
						if(pStreamInfo->uSelInfo&VOOSMP_SRC_TRACK_SELECT_SELECTED)
							*psIndex = (int)pTrackInfo->uTrackID;
					}
					else
					{
						*psIndex = (int)pTrackInfo->uTrackID;
					}
					continue;
				}

				if(strlen(sSubtitleTrackLan) > 0 && strcmp(pTrackInfo->sSubtitleInfo.chLanguage, sSubtitleTrackLan) == 0)
				{
					nFoundS = 1;
					if(nVideoSelectTrack == -1)
					{
						if(pStreamInfo->uSelInfo&VOOSMP_SRC_TRACK_SELECT_SELECTED)
							*psIndex = (int)pTrackInfo->uTrackID;
					}
					else
					{
						*psIndex = (int)pTrackInfo->uTrackID;
					}
					continue;
				}
			}
			else if((int)pTrackInfo->uTrackType == VOOSMP_SS_MUX_GROUP)
			{
				if((nVideoSelectTrack == -1 && (pStreamInfo->uSelInfo&VOOSMP_SRC_TRACK_SELECT_SELECTED)) ||
					nVideoSelectTrack == (int)pStreamInfo->uStreamID)
				{
					if(nFoundA == 0)
						*paIndex = nAudioSelectTrack;
					
					if(nFoundS == 0)
						*psIndex = nSubTitleSelectTrack;
				}

				nFoundA = 1;
				nFoundS = 1;
			}
		}
        
        if(nVideoSelectTrack == -1)
        {
            if(nFoundA == 0 || nFoundS == 0)
                return 0;
        }

		VOLOGI ("nVideoSelectTrack %d.nFoundA %d, nFoundV %d, nFoundS %d", nVideoSelectTrack, nFoundA, nFoundV, nFoundS);
		if(nVideoSelectTrack != -1 && nFoundA && nFoundV && nFoundS)
		{
			return (m_pProgramInfo->uProgramID << 16) | (*pvIndex << 8) | 1;
		}
	}

	if(nAudioSelectTrack == -2 && nVideoSelectTrack == -2)
		return 1;

	if(nAudioSelectTrack == -2 && nSubTitleSelectTrack == -2)
		return 1;

	if(nVideoSelectTrack == -2 && nSubTitleSelectTrack == -2)
		return 1;
    
    if(nVideoSelectTrack == -1)
    {
        if(nFoundA && nFoundV && nFoundS)
		{
			return 1;
		}
    }

	return 0;
}

int COSAdapStrSource::ClearPropertyItem(VOOSMP_SRC_TRACK_PROPERTY *pItemProperty)
{
	if(pItemProperty == NULL)
		return VOOSMP_ERR_None;

	if(pItemProperty->nPropertyCount > 0 && pItemProperty->ppItemProperties)
	{
		int n;

		for(n = 0; n < pItemProperty->nPropertyCount; n++)
		{
			if(pItemProperty->ppItemProperties[n])
			{
				if(pItemProperty->ppItemProperties[n]->pszProperty)
				{
					delete []pItemProperty->ppItemProperties[n]->pszProperty;
					pItemProperty->ppItemProperties[n]->pszProperty = NULL;
				}

				delete []pItemProperty->ppItemProperties[n];

				pItemProperty->ppItemProperties[n] = NULL;
			}
		}

		delete []pItemProperty->ppItemProperties;
		pItemProperty->ppItemProperties = NULL;
	}

	memset(pItemProperty, 0, sizeof(VOOSMP_SRC_TRACK_PROPERTY));

	return 0;
}

int COSAdapStrSource::IsAudioIndexAvaialbe( int nAudioIndex)
{
	if (nAudioIndex < 0) return 0;
	if (UpdateTrackInfo()) return 0;

	int nCheckResult = 0;
	int nAudioSelectTrackID = m_pAudioTrackIndex[nAudioIndex];

	VO_SOURCE2_STREAM_INFO* pStreamInfo = NULL;
	VO_SOURCE2_TRACK_INFO* pTrackInfo = NULL;	

	for(unsigned int i = 0; i < m_pProgramInfo->uStreamCount; i++)
	{
		pStreamInfo = m_pProgramInfo->ppStreamInfo[i];

		if(pStreamInfo == NULL || pStreamInfo->ppTrackInfo == NULL ||  ( (pStreamInfo->uSelInfo & VO_SOURCE2_SELECT_SELECTED) == 0) )
			continue;

		for(unsigned int j = 0; j < pStreamInfo->uTrackCount; j++)
		{
			pTrackInfo = pStreamInfo->ppTrackInfo[j];

			if( pTrackInfo == NULL)
				continue;

			if(((int)pTrackInfo->uTrackType == VOOSMP_SS_AUDIO || (int)pTrackInfo->uTrackType == VOOSMP_SS_AUDIO_GROUP)) 
			{
				if((int)pTrackInfo->uTrackID == nAudioSelectTrackID)
				{
					nCheckResult  = 1;
				}

				if(strcmp(pTrackInfo->sAudioInfo.chLanguage,  m_pAudioTrackLan[nAudioIndex]) == 0)
				{
					nCheckResult  = 1;
				}
			}
		}
	}

	return nCheckResult;
}

int COSAdapStrSource::IsVideoIndexAvaialbe( int nVideoIndex)
{
	return 1;
}

int COSAdapStrSource::IsSubtitleIndexAvaialbe( int nSubtitleIndex)
{
	/* TODO implementation */
	return 0;
}

int COSAdapStrSource::GetAutoSelectStreamingAudioSubtitleID (int* pAudioID, int* pSubtitleID)
{
	UpdateTrackInfo();

	if(m_pProgramInfo == NULL || m_pProgramInfo->ppStreamInfo == NULL)
		return VOOSMP_ERR_Implement;

	int nFindA = 0;
	int nFindS = 0;

	int nAudioSelectTrack = -1;
	int nSubTitleSelectTrack = -1;
	char sAudioTrackLan[256];
	char sSubtitleTrackLan[256];
	memset(sAudioTrackLan, 0, sizeof(char)*256);
	memset(sSubtitleTrackLan, 0, sizeof(char)*256);

	if(*pAudioID >=0)
	{
		if(*pAudioID < m_nAudioCount)
		{
			nAudioSelectTrack = m_pAudioTrackIndex[*pAudioID];
			strcpy(sAudioTrackLan, m_pAudioTrackLan[*pAudioID]);
		}
		else
		{
			return VOOSMP_ERR_Implement;
		}
	}

	if(*pSubtitleID >=0)
	{
		if(*pSubtitleID < m_nSubtitleCount)
		{
			nSubTitleSelectTrack = m_pSubtitleTrackIndex[*pSubtitleID];
			strcpy(sSubtitleTrackLan, m_pSubtitleTrackLan[*pSubtitleID]);
		}
		else
		{
			return VOOSMP_ERR_Implement;
		}
	}

	VOLOGI("@@@### AudioIndexCheck = %d, SubtitleCheck = %d", nAudioSelectTrack, nSubTitleSelectTrack);

	VO_SOURCE2_STREAM_INFO* pStreamInfo = NULL;
	VO_SOURCE2_TRACK_INFO* pTrackInfo = NULL;
	for(VO_U32 i = 0; i < m_pProgramInfo->uStreamCount; i++)
	{
		pStreamInfo = m_pProgramInfo->ppStreamInfo[i];

		if(pStreamInfo == NULL || pStreamInfo->ppTrackInfo == NULL)
			continue;

		if (nFindA && nFindS) return VOOSMP_ERR_None;

		if(pStreamInfo->uSelInfo&VOOSMP_SRC_TRACK_SELECT_SELECTED)
		{
			for(VO_U32  j = 0; j < pStreamInfo->uTrackCount; j++)
			{
				pTrackInfo = pStreamInfo->ppTrackInfo[j];

				if(pTrackInfo == NULL)
					continue;

				if(((int)pTrackInfo->uTrackType == VOOSMP_SS_AUDIO || (int)pTrackInfo->uTrackType == VOOSMP_SS_AUDIO_GROUP)) 
				{
					if (nFindA) continue;
					if(strlen(sAudioTrackLan) > 0 && strcmp(pTrackInfo->sAudioInfo.chLanguage, sAudioTrackLan) == 0)
					{
						nFindA = 1;
						*pAudioID = (int)pTrackInfo->uTrackID;
						VOLOGI("@@@### Audio-found, language = %s", sAudioTrackLan);
					}
					if( (int)pTrackInfo->uTrackID == nAudioSelectTrack)
					{
						nFindA = 1;
						*pAudioID = (int)pTrackInfo->uTrackID;
						VOLOGI("@@@###  Audio-found, ID = %d", *pAudioID);
					}
				}
				else if(((int)pTrackInfo->uTrackType == VOOSMP_SS_SUBTITLE || (int)pTrackInfo->uTrackType == VOOSMP_SS_SUBTITLE_GROUP))
				{
					if (nFindS) continue;
					if(strlen(sSubtitleTrackLan) > 0 && strcmp(pTrackInfo->sSubtitleInfo.chLanguage, sSubtitleTrackLan) == 0)
					{
						nFindS = 1;
						*pSubtitleID = (int)pTrackInfo->uTrackID;
						VOLOGI("@@@###  Subtitle-found, language = %s", sSubtitleTrackLan);
					}
					if((int)pTrackInfo->uTrackID == nSubTitleSelectTrack)
					{
						nFindS = 1;
						*pSubtitleID = (int)pTrackInfo->uTrackID;
						VOLOGI("@@@###  Subtitle-found, ID = %d", *pSubtitleID);
					}
				}
			}
		}
		else
		{
			continue;
		}
	}

	return VOOSMP_ERR_Implement;
}
