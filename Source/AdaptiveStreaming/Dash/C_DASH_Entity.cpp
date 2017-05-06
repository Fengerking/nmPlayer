#include "string.h"
#include "C_DASH_Entity.h"
#include "voLog.h"
#include "voOSFunc.h"
#include "CDataBox.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
typedef VO_S32 ( VO_API *pvoGetParserAPI)(VO_PARSER_API * pParser);
typedef VO_S32 ( VO_API *pvoGetSource2ParserAPI)(VO_SOURCE2_API* pParser);


C_DASH_Entity::C_DASH_Entity()
{

	m_pProgramInfo = NULL;
	m_hCheck =  NULL;
	m_bOpenComplete = VO_FALSE;
	m_bNewFormatArrive = VO_TRUE;
	m_bUpdateRunning = VO_TRUE;
	m_enRefresh = VO_TRUE;
	pure_audio = VO_FALSE; 
	m_audio_end = VO_FALSE;
	m_video_end = VO_FALSE;
	m_last_index = 0;
	m_write = VO_FALSE;
	m_ProgramId = 0;
	m_cur_stream_id = 0;
	m_cur_track_video_id = 0;
	m_cur_track_audio_id = 0;
	m_program_change_video = VO_TRUE;
	m_program_change_audio = VO_TRUE;
	m_program_change_subtitle = VO_TRUE;
	memset(&m_sCurrentAdaptiveStreamItemForPlayList, 0, sizeof(VO_ADAPTIVESTREAMPARSER_CHUNK));
	memset(&m_sCurrentAdaptiveStreamItemForMainStream, 0, sizeof(VO_ADAPTIVESTREAMPARSER_CHUNK));
	memset(&m_sCurrentAdaptiveStreamItemForAlterVideo, 0, sizeof(VO_ADAPTIVESTREAMPARSER_CHUNK));
	memset(&m_sCurrentAdaptiveStreamItemForAlterAudio, 0, sizeof(VO_ADAPTIVESTREAMPARSER_CHUNK));
	memset(&m_sCurrentAdaptiveStreamItemForAlterSubtitle, 0, sizeof(VO_ADAPTIVESTREAMPARSER_CHUNK));
	memset(mpd_root_url, 0x00, sizeof(mpd_root_url));
	m_pEventCallbackFunc  = NULL;
	m_pDataCallbackFunc = NULL;
	m_BA_current_chunk_flag = VO_FALSE;
	m_is_BA_mode = VO_FALSE;
	m_is_first_BA = VO_TRUE;
	m_ba_number = 0;
	Is_Need_Sync = VO_FALSE;
}


C_DASH_Entity::~C_DASH_Entity()
{

}

VO_U32  C_DASH_Entity::Init_DASH(VO_ADAPTIVESTREAM_PLAYLISTDATA * pData , VO_SOURCE2_EVENTCALLBACK * pEventCallback,VO_ADAPTIVESTREAMPARSER_INITPARAM * pInitParam)
{
	if(NULL ==pInitParam)
		 return VO_RET_SOURCE2_EMPTYPOINTOR;
	VOLOGINIT(pInitParam->strWorkPath);
	VO_U32 ret = voCheckLibInit(&m_hCheck,VO_INDEX_SRC_DASH,VO_LCS_WORKPATH_FLAG,0,pInitParam->strWorkPath);
	if(ret!=VO_ERR_NONE)
	{
		if(m_hCheck != 0)
			voCheckLibUninit(m_hCheck);
		m_hCheck = NULL;
		return ret;
	}
	VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if(NULL == pEventCallback || NULL == pData)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	m_pEventCallbackFunc = pEventCallback;
	ulRet = CommitPlayListData(pData);
	return ulRet;
}

VO_U32    C_DASH_Entity::CommitPlayListData(VO_VOID*  pPlaylistData)
{
	VO_U32   ulRet = VO_RET_SOURCE2_OK;
	VO_ADAPTIVESTREAM_PLAYLISTDATA*   pPlaylistItem = NULL;
	VO_PBYTE    pData = NULL;
	if(pPlaylistData == NULL)
	{
		VOLOGI("The pPlaylistData is NULL!");
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	memset(&m_sCurrentAdaptiveStreamItemForPlayList, 0, sizeof(m_sCurrentAdaptiveStreamItemForPlayList));
	pPlaylistItem = (VO_ADAPTIVESTREAM_PLAYLISTDATA*)pPlaylistData;
	pData = new VO_BYTE[pPlaylistItem->uDataSize];
	if(pData == NULL)
	{
		VOLOGI("Lack of memory!");
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	memset(pData, 0, pPlaylistItem->uDataSize);
	memcpy(pData, pPlaylistItem->pData, pPlaylistItem->uDataSize);
	m_sCurrentAdaptiveStreamItemForPlayList.pData = pData;
	m_sCurrentAdaptiveStreamItemForPlayList.uDataSize = pPlaylistItem->uDataSize;
	memcpy(mpd_root_url, pPlaylistItem->szUrl, strlen(pPlaylistItem->szUrl));
	memcpy(pPlaylistItem->szRootUrl, pPlaylistItem->szUrl, strlen(pPlaylistItem->szUrl));
	memcpy(m_sCurrentAdaptiveStreamItemForPlayList.szUrl, pPlaylistItem->szUrl, strlen(pPlaylistItem->szUrl));
	return   ulRet;
}

VO_U32     C_DASH_Entity::Uninit_DASH()
{
	VOLOGE("dash_entity unint 1");
	VO_U32   ulRet = VO_RET_SOURCE2_OK;
	ResetAllContext();
	VOLOGE("dash_entity unint 2");
	if(m_hCheck != NULL)
    {
        voCheckLibUninit(m_hCheck);
        m_hCheck = NULL;
    }
	return ulRet;
}


VO_U32     C_DASH_Entity::Close_DASH()
{
	VOLOGE( "+Close_DASH" );
	VO_U32    ulRet = VO_RET_SOURCE2_OK;
	ResetAllContext();
	VOLOGE( "-Close_DASH" );
	return ulRet;
}

VO_VOID    C_DASH_Entity::StopPlaylistUpdate()
{   
	VOLOGE( "+stop_updatethread" );
	m_bUpdateRunning = VO_FALSE;
	m_enRefresh = VO_FALSE;
	vo_thread::stop();
	VOLOGE( "-stop_updatethread" );
}

VO_U32     C_DASH_Entity::Open_DASH()
{

	VO_U32    ulRet = 0;
	S_PLAY_SESSION*     pPlaySession = NULL;
	E_PLAYLIST_TYPE     ePlaylistType = E_UNKNOWN_STREAM;
	if(NULL != m_pProgramInfo)
	{
		DeleteAllProgramInfo();
	}
	m_pProgramInfo = new _PROGRAM_INFO[PERIOD_MAXNUM];
	if(NULL == m_pProgramInfo)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	for(int i = 0;i<PERIOD_MAXNUM;i++)
	{
		memset(&m_pProgramInfo[i], 0x00, sizeof(_PROGRAM_INFO));
		m_pProgramInfo[i].ppStreamInfo = NULL;
	}
	m_sMpdManager.InitParser();
	ulRet = ParseDASHPlaylist(&m_sCurrentAdaptiveStreamItemForPlayList);
	if(NULL != m_sCurrentAdaptiveStreamItemForPlayList.pData)
	{
		delete[] m_sCurrentAdaptiveStreamItemForPlayList.pData;
		m_sCurrentAdaptiveStreamItemForPlayList.pData = NULL;
	}
	m_PeriodCount = m_sMpdManager.m_MpdParser.m_period_count;
	if(m_PeriodCount==0)
	{
		VOLOGW("m_PeriodCount is 0 return error");
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	if(!m_sMpdManager.Is_Ts()&&!m_sMpdManager.m_MpdParser.Is_dash_muxed(m_ProgramId))
		ulRet = Create_ProgramInfo(m_ProgramId);
	else
		ulRet = Create_ProgramInfo_ts(m_ProgramId);
	m_sMpdManager.SelectDefaultAV_Index(m_ProgramId);
	if(NULL != m_pEventCallbackFunc && NULL !=  m_pEventCallbackFunc->SendEvent)
		m_pEventCallbackFunc->SendEvent(m_pEventCallbackFunc->pUserData, VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_PROGRAMRESET, (VO_U32)(&m_pProgramInfo[m_ProgramId]), (VO_U32)NULL);
	if(NULL != m_pEventCallbackFunc && NULL!= m_pEventCallbackFunc->SendEvent )
		m_pEventCallbackFunc->SendEvent(m_pEventCallbackFunc->pUserData, VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_PROGRAMCHANGED, (VO_U32)(&m_pProgramInfo[m_ProgramId]), (VO_U32)NULL);
	ulRet = m_sMpdManager.GetTheCurrentPlaySession(&pPlaySession);
	if(VO_FALSE == m_bOpenComplete && NULL != m_pEventCallbackFunc && NULL != m_pEventCallbackFunc->SendEvent )
	{
			m_pEventCallbackFunc->SendEvent(m_pEventCallbackFunc->pUserData, VO_EVENTID_SOURCE2_OPENCOMPLETE,  (VO_U32)NULL,  (VO_U32)NULL);
			m_bOpenComplete = VO_TRUE;
	}
	return ulRet;
}
/*addd for formal release*/
VO_U32   C_DASH_Entity::Start_DASH()
{
	VO_U32 ulRet = VO_RET_SOURCE2_OK;
	if(m_pProgramInfo&&m_pProgramInfo->sProgramType ==VO_SOURCE2_STREAM_TYPE_LIVE )
	{
		begin();
	}
	return ulRet;

}
/*ended for formal release*/

/*addd for formal release*/
VO_U32   C_DASH_Entity::Stop_DASH()
{
	VOLOGW("+Stop_DASH" );
	VO_U32 ulRet = VO_RET_SOURCE2_OK;
	StopPlaylistUpdate();
	VOLOGW("-Stop_DASH" );
	return ulRet;
}
/*ended for formal release*/
VO_VOID    C_DASH_Entity::DeleteAllProgramInfo()
{
	_STREAM_INFO*   pStreamInfo = NULL;
	if(m_pProgramInfo!=NULL)
	{
		for(int j = 0;j<PERIOD_MAXNUM;j++)
		{
			if(&m_pProgramInfo[j] != NULL)
			{
				if(m_pProgramInfo[j].ppStreamInfo != NULL)
				{
					for( int i = 0 ; i < m_pProgramInfo[j].uStreamCount ; i++ )
					{
						pStreamInfo = m_pProgramInfo[j].ppStreamInfo[i];
						DeleteStreamInfo(pStreamInfo);
						delete pStreamInfo;
					}

					delete[] m_pProgramInfo[j].ppStreamInfo;
					m_pProgramInfo[j].ppStreamInfo = NULL;
					VOLOGW( "delete DeleteAllProgramInfo Exit1!" );
				}

			}
		}
		delete m_pProgramInfo; 
		VOLOGW( "delete DeleteAllProgramInfo Exit2!" );
		m_pProgramInfo = NULL;
		VOLOGW( "delete DeleteAllProgramInfo Exit3!" );
	}
}
void   C_DASH_Entity::thread_function()
{
	if(m_bUpdateRunning)
		PlayListUpdateForLive();
	VOLOGI( "Update Thread Exit!" );
}

VO_U32 C_DASH_Entity::PlayListUpdateForLive()
{

	S_PLAY_SESSION*     pPlaySession = NULL;
	VO_U32              ulRet = 0;
	VO_U32              ulTimeInterval = 0;
	VO_U64              ullStartTime = 0;
	VO_CHAR*            pURL = NULL;
	VO_ADAPTIVESTREAM_PLAYLISTDATA    sVarPlayListData = {0};
	ulRet = m_sMpdManager.GetTheCurrentPlaySession(&pPlaySession);
	if(ulRet != 0)
	{
		m_bUpdateRunning = VO_FALSE;
		return VO_RET_SOURCE2_OK;
	}
  
	if(NULL!=m_pProgramInfo&&m_pProgramInfo->sProgramType ==VO_SOURCE2_STREAM_TYPE_LIVE)
	{
		m_bUpdateRunning = VO_TRUE;
		int  pMainStreamCount = 0;
		while(m_bUpdateRunning == VO_TRUE)
		{ 

			ullStartTime = voOS_GetSysTime();
			/*ulTimeInterval = 10000;*/
			/*ulTimeInterval = (m_sMpdManager.GetChunkDuration())*3/2;*/
			ulTimeInterval = (m_sMpdManager.GetChunkDuration());
			if(ulTimeInterval == 0xffffffff)
			{
				voOS_Sleep(50);
				continue;
			}

			VO_U32  ulTimeInterval_now = voOS_GetSysTime() - ullStartTime;
			while(m_bUpdateRunning == VO_TRUE && ( ulTimeInterval_now <= ulTimeInterval)&&m_enRefresh==VO_TRUE) //
			{  

				ulTimeInterval_now = voOS_GetSysTime() - ullStartTime;
				voOS_Sleep( 100 );
				//VOLOGW("sleep");

			}
			if(strlen(pPlaySession->strStreamURL) != 0)
			{

				memset(&sVarPlayListData, 0, sizeof(VO_ADAPTIVESTREAM_PLAYLISTDATA));
				memcpy(sVarPlayListData.szUrl, pPlaySession->strStreamURL, strlen(pPlaySession->strStreamURL));
				memcpy(sVarPlayListData.szRootUrl, pPlaySession->strStreamURL, strlen(pPlaySession->strStreamURL));
				memcpy(sVarPlayListData.szNewUrl, pPlaySession->strStreamURL, strlen(pPlaySession->strStreamURL));
				if(m_pEventCallbackFunc != NULL && m_pEventCallbackFunc->SendEvent != NULL)
				{

					pURL = pPlaySession->strStreamURL;
					VO_DATABOX_CALLBACK DataBox_CallBack;
					CDataBox databox;
					DataBox_CallBack.MallocData = databox.MallocData;
					DataBox_CallBack.pUserData = (VO_PTR)&databox;
					sVarPlayListData.pReserve = (VO_PTR)&DataBox_CallBack;
					ulRet = m_pEventCallbackFunc->SendEvent(m_pEventCallbackFunc->pUserData, VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_NEEDPARSEITEM, (VO_U32)(&sVarPlayListData),  (VO_U32)(NULL));
					if(ulRet==0)
					{
						ParseDASHPlaylist(&sVarPlayListData);
						Update_ProgramInfo(m_ProgramId);

					}
				}
			}
			voOS_Sleep(50);
		}	

	}
	return 0;

}

VO_VOID    C_DASH_Entity::DeleteStreamInfo(_STREAM_INFO*   pStreamInfo)
{
	VO_U32   ulIndex = 0;
	_TRACK_INFO*    pTrackInfo = NULL;   
	if(pStreamInfo != NULL)
	{
		for(ulIndex=0; ulIndex<pStreamInfo->uTrackCount; ulIndex++)
		{
			pTrackInfo = pStreamInfo->ppTrackInfo[ulIndex];
			if(pTrackInfo != NULL)
			{
				delete pTrackInfo;
			}
		}
		delete[] pStreamInfo->ppTrackInfo;
		VOLOGW( " DeleteStreamInfo Exit!" );
	}
	return;
}


VO_U32    C_DASH_Entity::Update_DASH(VO_ADAPTIVESTREAM_PLAYLISTDATA * pData)
{
	VO_U32   ulRet = VO_RET_SOURCE2_OK;
	if(pData == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	VOLOGI("do update for DASH live  stream!");
	ulRet = ParseDASHPlaylist(pData);
	ulRet = ConvertErrorCodeToSource2(ulRet);
	return ulRet;
}
VO_U32       C_DASH_Entity::ConvertErrorCodeToSource2(VO_U32   ulErrorCodeInDASH)
{
	switch(ulErrorCodeInDASH)
	{
	case DASH_ERR_NONE:
		{
			return VO_RET_SOURCE2_OK;
		}
	case DASH_ERR_VOD_END:
		{
			return VO_RET_SOURCE2_END;;
		}
	case DASH_ERR_EMPTY_POINTER:
	case DASH_ERR_WRONG_MANIFEST_FORMAT:
	case DASH_ERR_LACK_MEMORY:
	case DASH_UN_IMPLEMENT:
	case DASH_ERR_NOT_ENOUGH_BUFFER:
	case DASH_ERR_NOT_EXIST:
	case DASH_ERR_NOT_ENOUGH_PLAYLIST_PARSED:
	case DASH_ERR_NEED_DOWNLOAD:
	case DASH_ERR_ALREADY_EXIST:
		{
			return VO_RET_SOURCE2_ERRORDATA;  
		}
	default:
		{
			return VO_RET_SOURCE2_NOIMPLEMENT;
		}
	}
}

VO_U32     C_DASH_Entity::ParseDASHPlaylist(VO_VOID*  pPlaylistData)
{
	voCAutoLock lock (&g_voMemMutex);
	VO_BOOL   ulRet = VO_FALSE;
	VO_ADAPTIVESTREAM_PLAYLISTDATA*   pPlaylistItem = NULL;

	if(pPlaylistData == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	pPlaylistItem = (VO_ADAPTIVESTREAM_PLAYLISTDATA*)pPlaylistData;
	m_sMpdManager.m_chunk_audio_index = 0;
	m_sMpdManager.m_chunk_video_index =0;
	VOLOGE("before  chunk_index %lld",m_sMpdManager.m_chunk_video_index);
	ulRet = m_sMpdManager.ParseMpd(pPlaylistItem->pData, pPlaylistItem->uDataSize, (VO_CHAR*)pPlaylistItem->szUrl);
	if(ulRet!=VO_FALSE)
	{
		m_sMpdManager.Init_Period(m_ProgramId);
		Compute_Chunk_index();

	}
	if(m_write == VO_TRUE)  
	{  
		VOLOGE("time is wrong2");
		m_write = VO_FALSE;

	}
	VOLOGE("after  chunk_index %lld",m_sMpdManager.m_chunk_video_index );
	return  VO_RET_SOURCE2_OK;
} 

#ifdef _new_programinfo
    VO_U32      C_DASH_Entity::SelectTrack_DASH(VO_U32 nTrackID, VO_SOURCE2_TRACK_TYPE sTrackType)
#else
	VO_U32     C_DASH_Entity::SelectTrack_DASH(VO_U32 nTrackID)
#endif
{
	voCAutoLock lock (&g_voMemMutex);
	VOLOGW( "SelectTrack_DASH  nTrackID %d " , nTrackID);
	VO_U32   ulRet = VO_RET_SOURCE2_FAIL;
	VO_U32   ulStreamIndex = 0xffffffff;
	_TRACK_INFO*  pTrackInfo = NULL;
	_TRACK_INFO*  pTrackInfo_sel = NULL;
	VO_BOOL   bFindTrack = VO_FALSE;
	VO_U32     ulBitrate = 0;
	VO_U32     ulIndex = 0;
	VO_U32     pVideoConcern = 0;
	VO_CHAR*  pAudioConcern = NULL;
	S_PLAY_SESSION*                    pPlaySession = NULL; 

	if(&m_pProgramInfo[m_ProgramId] == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	ulRet = m_sMpdManager.GetTheCurrentPlaySession(&pPlaySession);

	for(ulIndex=0; ulIndex<m_pProgramInfo[m_ProgramId].uStreamCount; ulIndex++)
	{
		if( m_pProgramInfo[m_ProgramId].ppStreamInfo != NULL && m_pProgramInfo[m_ProgramId].ppStreamInfo[ulIndex] != NULL)
		{
			if(m_pProgramInfo[m_ProgramId].ppStreamInfo[ulIndex]->uSelInfo == VO_SOURCE2_SELECT_SELECTED)
			{
				ulStreamIndex = ulIndex;
				ulBitrate = m_pProgramInfo[m_ProgramId].ppStreamInfo[ulIndex]->uBitrate;
				break;
			}
		}

	}
	if(ulStreamIndex != 0xffffffff)
	{
		for(ulIndex=0; ulIndex<m_pProgramInfo[m_ProgramId].ppStreamInfo[ulStreamIndex]->uTrackCount; ulIndex++)
		{
			if(m_pProgramInfo[m_ProgramId].ppStreamInfo[ulStreamIndex]->ppTrackInfo != NULL && m_pProgramInfo[m_ProgramId].ppStreamInfo[ulStreamIndex]->ppTrackInfo[ulIndex] != NULL)
			{
				pTrackInfo = m_pProgramInfo[m_ProgramId].ppStreamInfo[ulStreamIndex]->ppTrackInfo[ulIndex];
#ifdef _new_programinfo
				if(pTrackInfo->uASTrackID == nTrackID)

#else
				if(pTrackInfo->uTrackID == nTrackID)
#endif
				{
					bFindTrack = VO_TRUE;
					pTrackInfo_sel = pTrackInfo;
					m_pProgramInfo[m_ProgramId].ppStreamInfo[ulStreamIndex]->ppTrackInfo[ulIndex]->uSelInfo = VO_SOURCE2_SELECT_SELECTED;
					//break;
				}
				else
				{
					m_pProgramInfo[m_ProgramId].ppStreamInfo[ulStreamIndex]->ppTrackInfo[ulIndex]->uSelInfo = VO_SOURCE2_SELECT_SELECTABLE;

				}
			}
		}

	}
	if(bFindTrack == VO_TRUE)
	{
		switch(pTrackInfo_sel->uTrackType)
		{
		case VO_SOURCE2_TT_AUDIO:
			{
				if(m_last_audio_track_id == nTrackID)
				{
					m_cur_track_audio_id = m_last_audio_track_id;
				}
				else
				{
					m_last_audio_track_id = nTrackID; 
					m_cur_track_audio_id = m_last_audio_track_id;
					pAudioConcern = pTrackInfo->sAudioInfo.chLanguage;
					m_sMpdManager.m_need_send_track_audio = VO_TRUE;					
				}
				break;
			}
		case VO_SOURCE2_TT_VIDEO:
			{
				pVideoConcern = ulStreamIndex;
			    m_cur_track_video_id =nTrackID;
				break;

			}
		case VO_SOURCE2_TT_SUBTITLE:
			{
				m_cur_subtitle_track_id = nTrackID;
				m_sMpdManager.m_need_send_track_subtitle = VO_TRUE;
				break;
			}
		}
		ulRet = VO_RET_SOURCE2_OK;
	}
	m_sMpdManager.m_need_send_track_audio = VO_TRUE;
	VOLOGW( "SelectTrack_DASH m_last_audio_track_id %d " , m_last_audio_track_id); 
	VOLOGW( "SelectTrack_DASH m_cur_track_video_id %d " , m_cur_track_video_id);
	VOLOGW( "SelectTrack_DASH m_cur_subtitle_track_id %d " , m_cur_subtitle_track_id);
	m_sMpdManager.SelectCurrentPlaySession(ulStreamIndex, m_last_audio_track_id,m_cur_subtitle_track_id);
	VOLOGW( "SelectTrack_DASH strAlterAudioGroupID %d " , m_sMpdManager. m_CurrentPlaySession.strAlterAudioGroupID);
	VOLOGW( "SelectTrack_DASH strAlterVideoGroupID %d " , m_sMpdManager. m_CurrentPlaySession.strAlterVideoGroupID);
	VOLOGW( "SelectTrack_DASH strAlterSubtileGroupID %d " , m_sMpdManager. m_CurrentPlaySession.strAlterSubtileGroupID);
	VOLOGW( "SelectTrack_DASH ulStreamIndex %d " , ulStreamIndex); 
	return VO_RET_SOURCE2_OK;
}
VO_U32    C_DASH_Entity:: SelectProgram_DASH (VO_U32 uProgramId)
{
	m_ProgramId =  uProgramId;
	return 0;
}
VO_U32 C_DASH_Entity::SearchTrackType(VO_U32 ulStreamIndex, VO_U32 nTrackID)
{
	_TRACK_INFO*  pTrackInfo = NULL;
	_TRACK_INFO*  pTrackInfo_sel = NULL;
	if(ulStreamIndex != 0xffffffff)
	{
		for(VO_U32 ulIndex=0; ulIndex<m_pProgramInfo[m_ProgramId].ppStreamInfo[ulStreamIndex]->uTrackCount; ulIndex++)
		{
			if(m_pProgramInfo[m_ProgramId].ppStreamInfo[ulStreamIndex]->ppTrackInfo != NULL && m_pProgramInfo[m_ProgramId].ppStreamInfo[ulStreamIndex]->ppTrackInfo[ulIndex] != NULL)
			{
				pTrackInfo = m_pProgramInfo[m_ProgramId].ppStreamInfo[ulStreamIndex]->ppTrackInfo[ulIndex];
#ifdef _new_programinfo
				if(pTrackInfo->uASTrackID == nTrackID)

#else
				if(pTrackInfo->uTrackID == nTrackID)
#endif
				{
					pTrackInfo_sel = pTrackInfo;
					m_pProgramInfo[m_ProgramId].ppStreamInfo[ulStreamIndex]->ppTrackInfo[ulIndex]->uSelInfo = VO_SOURCE2_SELECT_SELECTED;
					break;
				}
			} 
		}

	}
	return pTrackInfo_sel->uTrackType;
}
VO_U32     C_DASH_Entity::GetCurTrackInfo_DASH(VO_SOURCE2_TRACK_TYPE sTrackType , _TRACK_INFO ** ppTrackInfo)
{
	return 0;
}


VO_U32     C_DASH_Entity::SelectStream_DASH(VO_U32 uStreamId,VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS sPrepareChunkPos)
{
	voCAutoLock lock (&g_voMemMutex);
	if(m_sMpdManager.m_manifest.group_count>6)
		uStreamId = 6;
	m_is_BA_mode = VO_FALSE;
	VOLOGI( "SelectStream_DASH %d " , uStreamId);
	m_cur_stream_id = uStreamId;
	m_cur_track_audio_id = m_last_audio_track_id;
	VO_U32   ulRet = 0;
	VO_U32   ulIndex = 0;
	VO_BOOL  bFind = VO_FALSE;
	VO_U32   ulSequenceId = 0;
	VO_U32   ulNewBitrate = 0;
	VO_U32   ulStreamIndex = 0;
	VO_ADAPTIVESTREAMPARSER_CHUNK*     pChunk = NULL;
	S_PLAY_SESSION*                    pPlaySession = NULL;
	VO_CHAR*  pVideoConcern = NULL;
	VO_CHAR*  pAudioConcern = NULL;
	if(&m_pProgramInfo[m_ProgramId] == NULL)
	{
		return VO_RET_SOURCE2_OUTPUTNOTFOUND;
	}

	if(m_pProgramInfo[m_ProgramId].ppStreamInfo == NULL)
	{  
		return VO_RET_SOURCE2_OUTPUTNOTFOUND;
	}
	ulRet = m_sMpdManager.GetTheCurrentPlaySession(&pPlaySession);
	for(ulIndex=0; ulIndex<m_pProgramInfo[m_ProgramId].uStreamCount; ulIndex++)
	{
		if(m_pProgramInfo[m_ProgramId].ppStreamInfo[ulIndex]->uStreamID == uStreamId)
		{
			bFind = VO_TRUE;
			ulNewBitrate = m_pProgramInfo[m_ProgramId].ppStreamInfo[ulIndex]->uBitrate;
			ulStreamIndex = ulIndex;
			break;
		}
	}
	if(bFind == VO_TRUE)
	{  
		if(ulNewBitrate == pPlaySession->ulStreamBitrate)
		{
			m_pProgramInfo[m_ProgramId].ppStreamInfo[ulStreamIndex]->uSelInfo = VO_SOURCE2_SELECT_SELECTED;
			m_sMpdManager.SelectCurrentPlaySession(ulStreamIndex,m_last_audio_track_id,m_cur_subtitle_track_id);
			return VO_RET_SOURCE2_OK;
		}
		pPlaySession->ulStreamBitrate = ulNewBitrate;
		memset(&m_sCurrentAdaptiveStreamItemForAdaption, 0, sizeof(VO_ADAPTIVESTREAMPARSER_CHUNK));
		pChunk = &m_sCurrentAdaptiveStreamItemForAdaption;
		if(m_sMpdManager.GetCurrentSequenceId(&ulSequenceId) != 0)
		{
			return VO_RET_SOURCE2_OUTPUTNOTFOUND;
		}
		if(ulRet == 0)
		{
			m_bNewFormatArrive = VO_TRUE;
			pPlaySession->ulMainStreamSequenceId = ulStreamIndex;

			for(ulIndex=0; ulIndex<m_pProgramInfo[m_ProgramId].uStreamCount; ulIndex++)
			{
				if(m_pProgramInfo[m_ProgramId].ppStreamInfo[ulIndex]->uStreamID == uStreamId)
				{
					m_pProgramInfo[m_ProgramId].ppStreamInfo[ulIndex]->uSelInfo = VO_SOURCE2_SELECT_SELECTED;
				}
				else
				{
					m_pProgramInfo[m_ProgramId].ppStreamInfo[ulIndex]->uSelInfo = VO_SOURCE2_SELECT_SELECTABLE;
				}
			}

		}
		VO_BOOL  pure_audio = VO_FALSE;
		int count =  m_pProgramInfo[m_ProgramId].ppStreamInfo[ulStreamIndex]->uTrackCount;
		int index = 0;
		while(count)
		{
			if(m_pProgramInfo[m_ProgramId].ppStreamInfo[ulStreamIndex]->ppTrackInfo[index]->uTrackType == VO_SOURCE2_TT_AUDIO||m_pProgramInfo[m_ProgramId].ppStreamInfo[ulStreamIndex]->ppTrackInfo[index]->uTrackType == VO_SOURCE2_TT_SUBTITLE)
			{
				pure_audio = VO_TRUE;
			}
			else
			{
				pure_audio = VO_FALSE;
				break;
			}
			index++;
			count--;
		}
		VO_BOOL m_last_status_pure_audio = pPlaySession->is_pure_audio;
		pPlaySession->is_pure_audio = pure_audio;
		if(!pPlaySession->is_pure_audio )
		{
			m_sMpdManager.m_need_send_track_video = VO_TRUE;
			m_sMpdManager.m_need_send_track_audio = VO_TRUE;
			VOLOGI("m_need_send_track_video is VO_TRUE  1111111 ");
			pPlaySession->strAlterVideoSubStreamID = ulStreamIndex;
			VO_U32  track_count = m_pProgramInfo[m_ProgramId].ppStreamInfo[ulStreamIndex]->uTrackCount -1;
			VO_U32  last_m_cur_track_video_id = m_cur_track_video_id;
#ifdef _new_programinfo
			m_cur_track_video_id =m_pProgramInfo[m_ProgramId].ppStreamInfo[ulStreamIndex]->ppTrackInfo[track_count]->uASTrackID;
#else
			m_cur_track_video_id =m_pProgramInfo[m_ProgramId].ppStreamInfo[ulStreamIndex]->ppTrackInfo[track_count]->uTrackID;
#endif
			if(!m_last_status_pure_audio)//av-av not send audio_track
				m_sMpdManager.m_need_send_track_audio = VO_FALSE;
			if(m_is_first_BA)
			{
				m_sMpdManager.m_need_send_track_audio = VO_TRUE;
				m_is_first_BA = VO_FALSE;
			}

		}
		else
		{
			m_cur_track_audio_id  = m_last_audio_track_id;
			m_sMpdManager.m_need_send_track_audio = VO_TRUE;
			VOLOGI( "select pure_audio %d",m_cur_track_audio_id);

		}
		m_sMpdManager.SelectCurrentPlaySession(ulStreamIndex,m_last_audio_track_id,m_cur_subtitle_track_id);

	}
	VO_BOOL  change_flag = VO_FALSE;
	if(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS_PRESENT == sPrepareChunkPos)
		change_flag = VO_TRUE;
	else
		change_flag = VO_FALSE;
	if(change_flag)
	{
		VOLOGI( "SelectStream_DASH  current_chunk %lld", m_sMpdManager.m_chunk_video_index);
		VOLOGI( "SelectStream_DASH  current_chunk_audio %lld", m_sMpdManager.m_chunk_audio_index);
		VOLOGI( "SelectStream_DASH  current_chunk audio_group_id %d", pPlaySession->strAlterAudioGroupID);
		m_BA_current_chunk_flag = VO_TRUE;
		if(m_ba_number==0)
		{
			m_sMpdManager.m_chunk_video_index += 1;
			VOLOGI( "SelectStream_DASH  firstBA %lld", m_sMpdManager.m_chunk_video_index);
		}
		m_ba_number++;
	}
	if(m_pEventCallbackFunc != NULL && m_pEventCallbackFunc->SendEvent != NULL)
		m_pEventCallbackFunc->SendEvent(m_pEventCallbackFunc->pUserData, VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_PROGRAMCHANGED, (VO_U32)(&m_pProgramInfo[m_ProgramId]), (VO_U32)NULL);

	return VO_RET_SOURCE2_OK;
}

VO_VOID    C_DASH_Entity::Compute_Chunk_index()
{
	if(m_sMpdManager.Is_Live())
	{
		VO_U64 Current_utc_time = (voOS_GetSysTime()-m_system_time+m_utc_time);
		VO_CHAR live_time[1024];
		timeToSting(Current_utc_time,live_time);
		VOLOGW("Current_utc_time 22222 %s",live_time);
		VO_U64 Mpd_time = m_sMpdManager.m_MpdParser.GetAvailbilityTime()*1000;
		VO_S64 period_start = m_sMpdManager.m_MpdParser.m_manifest->m_period[0].period_start;
		VO_CHAR mpd_time[1024]= {0};
		timeToSting(Mpd_time,mpd_time);
		VOLOGW("mpd_utc_time 22222 %s",mpd_time);
		VO_U64 time_line_time = m_sMpdManager.m_MpdParser.GetTimeLineTime();
		VOLOGW("time_line_time %lld",time_line_time);
		VOLOGW("period_start %lld",period_start);
		VO_S64 left_time =(Current_utc_time-Mpd_time);
		left_time-=period_start;
#ifdef _USETIMELINE
		left_time-=time_line_time;
#endif
		VO_S64 duration = 0;
		VOLOGI("left_time %lld",left_time);	
		if(m_sMpdManager.m_MpdParser.m_manifest->m_period&&m_sMpdManager.m_MpdParser.m_manifest->m_period[0].period_vtimescale!=0)
			duration = (m_sMpdManager.m_MpdParser.m_manifest->duration_video)*1000/m_sMpdManager.m_MpdParser.m_manifest->m_period[0].period_vtimescale;
		else
		{
			VOLOGI("Compute_Chunk_index NULL");	
			return;
		}
	
		if(left_time>0&&duration>0)
		{
			VO_U64 index = (left_time/(duration));
			VOLOGE("orig_index in dex %lld",index);
			VO_S64 left_index = m_last_index-index;
			if(m_last_index!=0&&left_index<-20||left_index>20)
			{
				VOLOGE("the time is wrong");
				//	   index = m_last_index+1;
				m_write = VO_TRUE;
			}
			m_last_index = index;

			if(index>15)
			{
				if(duration<10100)
				{
					index-= 15;

				}
				m_sMpdManager.m_chunk_video_index =index;
				if(m_sMpdManager.m_chunk_video_index<0)
				{
					m_sMpdManager.m_chunk_video_index+=(-m_sMpdManager.m_chunk_video_index);
				}
				m_sMpdManager.m_chunk_audio_index = m_sMpdManager.m_chunk_video_index ;
				m_sMpdManager.m_chunk_subtitle_index = 	m_sMpdManager.m_chunk_audio_index;
				VOLOGE("Compute_Chunk_index>10 chunk_index %lld",m_sMpdManager.m_chunk_video_index);
			}
			else
			{
				
			/*	m_sMpdManager.m_chunk_video_index = index-2;
				if(m_sMpdManager.m_chunk_video_index<0)
					m_sMpdManager.m_chunk_video_index +=2;*/
				m_sMpdManager.m_chunk_audio_index = m_sMpdManager.m_chunk_video_index = index;
				m_sMpdManager.m_chunk_subtitle_index = m_sMpdManager.m_chunk_audio_index;
				VOLOGE("Compute_Chunk_index<10 chunk_index %lld",m_sMpdManager.m_chunk_video_index);
			}
			if(	m_sMpdManager.m_chunk_video_index>600)
			{
				VOLOGE("Compute_Chunk_index out of bound");
			}

		}
		else if(left_time<0)
		{
			/*m_sMpdManager.m_chunk_audio_index = 0;
			m_sMpdManager.m_chunk_video_index = 0;
			m_sMpdManager.m_chunk_subtitle_index = 0;*/
			VOLOGI("left_time<0 %lld",left_time);	
			VO_S64 index = (left_time/(duration));
			m_sMpdManager.m_chunk_audio_index =(index);
			VOLOGE("left_time chunk_index %lld",m_sMpdManager.m_chunk_audio_index);
			m_sMpdManager.m_chunk_video_index =m_sMpdManager.m_chunk_audio_index;
			m_sMpdManager.m_chunk_subtitle_index = m_sMpdManager.m_chunk_audio_index;
		}


	}


}
VO_U32     C_DASH_Entity::GetChunk_DASH(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID, VO_ADAPTIVESTREAMPARSER_CHUNK **ppChunk )
{
	//voCAutoLock lock (&g_voMemMutex);
	VO_U32   ulRet = VO_RET_SOURCE2_OK;
	if(ppChunk== NULL)
	{
		return DASH_ERR_EMPTY_POINTER;
	}
	if(uID == VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE)
	{  
		VOLOGW("uID == VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE");
		//	return VO_RET_SOURCE2_END;

	}

	//if(m_bNewFormatArrive)
	//{
	//	m_sMpdManager.Set_is_NewFormat(m_bNewFormatArrive);

	//}


	if(m_sMpdManager.Is_Live()&&(voOS_GetSysTime()-m_system_time+m_utc_time)<m_sMpdManager.m_MpdParser.GetAvailbilityTime()*1000)
	{
		ulRet = VO_RET_SOURCE2_NEEDRETRY;
		VOLOGW("the  mpd utc_time is in furture  the left_time is: %lld ",voOS_GetSysTime()-m_system_time+m_utc_time-m_sMpdManager.m_MpdParser.GetAvailbilityTime()*1000);
		return ulRet;
	}
	if(m_BA_current_chunk_flag)
	{
	VOLOGI( "Current_Chunk Dash 111 video %lld" , m_sMpdManager.m_chunk_video_index);
	VOLOGI( "Current_Chunk Dash 111 audio %lld" , m_sMpdManager.m_chunk_audio_index);
	m_BA_current_chunk_flag = VO_FALSE;
	if(m_sMpdManager.m_chunk_video_index !=0)
	m_sMpdManager.m_chunk_video_index -= 1;
	S_PLAY_SESSION*                   pPlaySession = NULL;
	ulRet = m_sMpdManager.GetTheCurrentPlaySession(&pPlaySession);
	VO_BOOL m_is_pure_audio = pPlaySession->is_pure_audio;
	if(m_is_pure_audio)
	{
       m_sMpdManager.m_chunk_video_index = m_sMpdManager.m_chunk_audio_index;
		VOLOGI( "Current_Chunk Dash 333 audio %lld" , m_sMpdManager.m_chunk_audio_index);
	}
	if(m_sMpdManager.m_chunk_subtitle_index !=0)
	m_sMpdManager.m_chunk_subtitle_index-=1;
	VOLOGI( "Current_Chunk Dash 222 video %lld" , m_sMpdManager.m_chunk_video_index);
	}
	ulRet = GetChunckItem(uID, ppChunk);
	m_ba_number++;
	if(ulRet==VO_RET_SOURCE2_END&&m_ProgramId<m_PeriodCount-1&&!m_sMpdManager.Is_Live())
	{
		if(m_video_end&&m_audio_end)
		{
			VOLOGW("change to new program");
			Change_Program(m_ProgramId+1);
		}
		return VO_RET_SOURCE2_OUTPUTNOTAVALIBLE;
	}
	if(ulRet!= VO_RET_SOURCE2_OK)
		return ulRet;
	switch(uID)
	{
	case VO_SOURCE2_ADAPTIVESTREAMING_AUDIO:
		{
			if(m_sMpdManager.m_need_send_track_audio)
			{
				if(ulRet == VO_RET_SOURCE2_OK)
				{
					(*ppChunk)->uFlag = VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;
					(*ppChunk)->Type = VO_SOURCE2_ADAPTIVESTREAMING_HEADDATA;
					/*if((*ppChunk)->ullChunkSize == INAVALIBLEU64&&NULL==m_sMpdManager.m_MpdParser.m_ptr_FI_head)*/
					/*if((*ppChunk)->ullChunkSize == INAVALIBLEU64&&m_sMpdManager.m_MpdParser.m_manifest->m_period[m_ProgramId].is_index_range_period_audio==VO_TRUE)*/
					if((*ppChunk)->ullChunkSize == INAVALIBLEU64&&m_sMpdManager.current_audio_rpe->is_init_data==VO_TRUE)
					{
						(*ppChunk)->Type = VO_SOURCE2_ADAPTIVESTREAMING_INITDATA;
					}
					VOLOGW("audio VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE");
					(*ppChunk)->ullTimeScale = 1;
#ifdef _new_programinfo
					(*ppChunk)->sKeyID.uProgramID =m_ProgramId;
					(*ppChunk)->sKeyID.uStreamID = m_cur_stream_id;
					(*ppChunk)->sKeyID.uTrackID = m_cur_track_audio_id;
					(*ppChunk)->uChunkID = -1;
#endif
					if(m_program_change_audio)
					{
						(*ppChunk)->uFlag =((*ppChunk)->uFlag)|VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_PROGRAMCHANGE;
						m_program_change_audio = VO_FALSE;
						VOLOGW("program audio VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE");
					}
					
				}
				m_bNewFormatArrive = VO_FALSE;
				m_sMpdManager.m_need_send_track_audio = VO_FALSE;
			}
			else
			{
				if(ulRet!= VO_RET_SOURCE2_END&&ulRet!=VO_RET_SOURCE2_NEEDRETRY&&ulRet!=VO_RET_SOURCE2_OUTPUTNOTAVALIBLE)
				{
					(*ppChunk)->uFlag = 0;
					(*ppChunk)->uChunkID = -1;
				}
			}
			break;
		}
	case VO_SOURCE2_ADAPTIVESTREAMING_VIDEO:
		{

			if(m_sMpdManager.m_need_send_track_video)
			{
				if(ulRet == VO_RET_SOURCE2_OK)
				{
					 VOLOGW("video VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE %s",(*ppChunk)->szUrl);
					(*ppChunk)->uFlag = VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;
					(*ppChunk)->Type = VO_SOURCE2_ADAPTIVESTREAMING_HEADDATA;
					//if((*ppChunk)->ullChunkSize == INAVALIBLEU64&&NULL==m_sMpdManager.m_MpdParser.m_ptr_FI_head)
					///*if(NULL==m_sMpdManager.m_MpdParser.m_ptr_FI_head)*/
					/*if((*ppChunk)->ullChunkSize == INAVALIBLEU64&&m_sMpdManager.m_MpdParser.m_manifest->m_period[m_ProgramId].is_index_range_period_video==VO_TRUE)*/
					if((*ppChunk)->ullChunkSize == INAVALIBLEU64&&m_sMpdManager.current_video_rpe->is_init_data==VO_TRUE)
					{
						(*ppChunk)->Type = VO_SOURCE2_ADAPTIVESTREAMING_INITDATA;
					}
					(*ppChunk)->uChunkID = -1;
					if(m_sMpdManager.Is_Ts())
					{
						(*ppChunk)->Type  = VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO;
					}
					
					if(m_program_change_video)
					{
						(*ppChunk)->uFlag = ((*ppChunk)->uFlag)|VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_PROGRAMCHANGE;
						m_program_change_video = VO_FALSE;
						VOLOGW("program VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_PROGRAMCHANGE ");
					}

				}
				m_bNewFormatArrive = VO_FALSE;
				m_sMpdManager.m_need_send_track_video = VO_FALSE;
			}
			else
			{
				if(ulRet!= VO_RET_SOURCE2_END&&ulRet!=VO_RET_SOURCE2_NEEDRETRY&&ulRet!=VO_RET_SOURCE2_OUTPUTNOTAVALIBLE)
				{
					if(m_is_BA_mode)
					{
						(*ppChunk)->uFlag = 0|VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_SMOOTH_ADAPTIONEX;
					    VOLOGI( "BA_mode first video_chunk" , (*ppChunk)->szUrl, (*ppChunk)->uFlag);
						m_is_BA_mode = VO_FALSE;
					}
					else
					(*ppChunk)->uFlag = 0;
					(*ppChunk)->uChunkID = -1;
				//	VOLOGR( "Dash url  %s flag %d " , (*ppChunk)->szUrl, (*ppChunk)->uFlag);
				}
			}

			break;

		}
	case VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE:
		if(m_sMpdManager.m_need_send_track_subtitle)
			{
				if(ulRet == VO_RET_SOURCE2_OK)
				{
					(*ppChunk)->uFlag = VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;
					(*ppChunk)->Type = VO_SOURCE2_ADAPTIVESTREAMING_HEADDATA;
				    if((*ppChunk)->ullChunkSize == INAVALIBLEU64&&m_sMpdManager.current_subtile_rpe->is_init_data==VO_TRUE)
					{
						(*ppChunk)->Type = VO_SOURCE2_ADAPTIVESTREAMING_INITDATA;
					}
					if(m_sMpdManager.m_MpdParser.m_is_ttml)
					(*ppChunk)->Type = VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE;
					(*ppChunk)->ullTimeScale = 1;
#ifdef _new_programinfo
					(*ppChunk)->sKeyID.uProgramID =m_ProgramId;
					(*ppChunk)->sKeyID.uStreamID = m_cur_stream_id;
					(*ppChunk)->sKeyID.uTrackID = m_cur_subtitle_track_id;
					(*ppChunk)->uChunkID = -1;
#endif
					if(m_program_change_subtitle)
					{
						(*ppChunk)->uFlag =((*ppChunk)->uFlag)|VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_PROGRAMCHANGE;
						m_program_change_subtitle = VO_FALSE;
					}
					VOLOGW("program audio VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE");
				}
				m_sMpdManager.m_need_send_track_subtitle = VO_FALSE;
			}
			else
			{
				if(ulRet!= VO_RET_SOURCE2_END&&ulRet!=VO_RET_SOURCE2_NEEDRETRY&&ulRet!=VO_RET_SOURCE2_OUTPUTNOTAVALIBLE)
				{

					(*ppChunk)->uFlag = 0;
					(*ppChunk)->uChunkID = -1;
				}
			}

		break;
	case VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO:
		if(m_sMpdManager.m_need_send_track_video)
		{
			if(ulRet == VO_RET_SOURCE2_OK)
			{
				(*ppChunk)->uFlag = VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;
				(*ppChunk)->Type = VO_SOURCE2_ADAPTIVESTREAMING_HEADDATA;
				#ifdef _new_programinfo
				 (*ppChunk)->sKeyID.uProgramID = 1;
				 (*ppChunk)->sKeyID.uStreamID = m_cur_stream_id;
				// (*ppChunk)->sKeyID.uTrackID = m_cur_track_video_id;
				 (*ppChunk)->uChunkID = -1;
#endif
			}
			m_bNewFormatArrive = VO_FALSE;
			m_sMpdManager.m_need_send_track_video = VO_FALSE;

		}
		else
		{
			if(ulRet!= VO_RET_SOURCE2_END&&ulRet!=VO_RET_SOURCE2_NEEDRETRY&&ulRet!=VO_RET_SOURCE2_OUTPUTNOTAVALIBLE)
			{
				 if(m_is_BA_mode)
				 {
					 (*ppChunk)->uFlag = 0;
					 (*ppChunk)->uFlag = 0|VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_SMOOTH_ADAPTIONEX;
					 m_is_BA_mode = VO_FALSE;
				 }
				 else
				(*ppChunk)->uFlag = 0;	
				// VOLOGR("Dash url  %s flag %d " , (*ppChunk)->szUrl, (*ppChunk)->uFlag);
#ifdef _new_programinfo
				 (*ppChunk)->sKeyID.uProgramID = 1;
				 (*ppChunk)->sKeyID.uStreamID = m_cur_stream_id;
				// (*ppChunk)->sKeyID.uTrackID = 1;
				 (*ppChunk)->uChunkID = -1;
#endif
			}
		}
		break;
	default:
		{
			break;
		}
	}

	return ulRet;
}

VO_U32 C_DASH_Entity::GetDRMInfo_DASH(VO_SOURCE2_DRM_INFO* ppDRMInfo )
{
	return 0;
}
VO_U32 C_DASH_Entity::GetParam_DASH(VO_U32 nParamID, VO_PTR pParam )
{
	VOLOGW("GetParam_DASH 1");
	VO_U64*    pullTimeValue = NULL;
	if(pParam == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }
	VO_U64 duration = 0;
	if(m_sMpdManager.m_MpdParser.m_manifest== NULL)	
	{
		VOLOGW("GetParam_DASH m_manifest ==NULL ");
		return 0;
	}
	if(m_sMpdManager.m_MpdParser.m_manifest->m_period ==NULL)	
	{
		VOLOGW("GetParam_DASH m_period ==NULL ");
		return 0;
	}
	if(&m_sMpdManager.m_MpdParser.m_manifest->m_period[0]==NULL)
		
	{
		VOLOGW("GetParam_DASH m_period[0] ==NULL ");
		return 0;
	}
	if(m_sMpdManager.m_MpdParser.m_manifest->m_period[0].period_vtimescale!=0)
		duration = (m_sMpdManager.m_MpdParser.m_manifest->duration_video)*1000/m_sMpdManager.m_MpdParser.m_manifest->m_period[0].period_vtimescale;
	VOLOGW("GetParam_DASH 2");
    switch(nParamID)
	{
        case  VO_PID_ADAPTIVESTREAMING_DVR_WINDOWLENGTH:
        {
            pullTimeValue = (VO_U64*)pParam;
            *pullTimeValue = duration;
			VOLOGW("GetParam_DASH duration :%lld", duration); 
            break;
        }
		case VO_PID_ADAPTIVESTREAMING_DVR_ENDLENGTH:
			{
				pullTimeValue = (VO_U64*)pParam;
				*pullTimeValue = duration;
				VOLOGW("GetParam_DASH VO_PID_ADAPTIVESTREAMING_DVR_ENDLENGTH :%lld", duration); 
				break;
			}
		case VO_PID_ADAPTIVESTREAMING_DVR_LIVELENGTH:
			{
				pullTimeValue = (VO_U64*)pParam;
				*pullTimeValue = duration;
				VOLOGW("GetParam_DASH VO_PID_ADAPTIVESTREAMING_DVR_ENDLENGTH :%lld", duration); 
				break;
			}
	}
	return VO_RET_SOURCE2_OK;
}


VO_U32  C_DASH_Entity::GetChunckItem(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID,VO_ADAPTIVESTREAMPARSER_CHUNK **ppChunk)
{
	voCAutoLock lock (&g_voMemMutex);
	VO_U32   ulRet = VO_RET_SOURCE2_OK;
	S_CHUNCK_ITEM   varChunckItem = {0};
	VO_ADAPTIVESTREAMPARSER_CHUNK*     pAdaptiveItem = NULL;
	E_PLAYLIST_TYPE                        ePlayListType = E_UNKNOWN_STREAM;
	S_PLAY_SESSION*                   pPlaySession = NULL;
	ulRet = m_sMpdManager.GetTheCurrentPlaySession(&pPlaySession);
	switch(uID)
	{  
	case VO_SOURCE2_ADAPTIVESTREAMING_AUDIO:
		{
			pAdaptiveItem = &m_sCurrentAdaptiveStreamItemForAlterAudio;
			ePlayListType = E_X_MEDIA_AUDIO_STREAM;
			/*add for ts test*/
			if(m_sMpdManager.Is_Ts())
				return VO_RET_SOURCE2_END;
			/*add for ts_test*/
			VO_U32 count = 0;
			VO_U32 ret = m_sMpdManager.GetMainAudioCount(&count);
			if(count==0&&m_sMpdManager.m_MpdParser.Is_dash_muxed(m_ProgramId)&&m_audio_end)
			{
				return VO_RET_SOURCE2_END;
			}
			if(count==0&&m_sMpdManager.m_MpdParser.Is_dash_pure_video()&&m_video_end)
				return VO_RET_SOURCE2_END;
			if(count==0&&m_sMpdManager.m_MpdParser.Is_dash_pure_video())
			{
				return VO_RET_SOURCE2_OUTPUTNOTAVALIBLE;
			}
			else if(count==0&&m_sMpdManager.m_MpdParser.Is_dash_muxed(m_ProgramId))
				return VO_RET_SOURCE2_NEEDRETRY;
			break;
		}
	case VO_SOURCE2_ADAPTIVESTREAMING_VIDEO:
		{ 

			VO_U32 count_audio = 0;
			VO_U32 ret = m_sMpdManager.GetMainAudioCount(&count_audio);
			pAdaptiveItem = &m_sCurrentAdaptiveStreamItemForAlterVideo;
			VO_BOOL m_is_pure_audio = m_sMpdManager.m_MpdParser.Is_dash_pure_audio();
			if(m_sMpdManager.Is_Ts()||(count_audio==0&&m_sMpdManager.Is_Muxed(m_ProgramId)))
			{
				ePlayListType = E_X_MEDIA_AV_STREAM;
			}
			else
			{
				ePlayListType = E_X_MEDIA_VIDEO_STREAM;

			}

			VO_U64 curent_audio_time = m_sMpdManager.m_pure_audio_current_time;
			VO_U64 duration_video = m_sMpdManager.m_manifest.duration_video;
			VO_U64 duration_audio = m_sMpdManager.m_manifest.duration_audio;
			VOLOGI( "AV_SYNC duration_audio:  %lld" , duration_audio);
			VOLOGI( "AV_SYNC duration_video:  %lld" ,duration_video);
			VOLOGI( "AV_SYNC curent_audio_time:  %lld" ,curent_audio_time);
			if((duration_video!=0&&Is_Need_Sync==VO_TRUE))
			{
				VOLOGI("Is_Need_Sync  pure_audio_video video_duration!=0  First_Is_Need_Sync");
				VO_U64 curent_audio_time = m_sMpdManager.m_pure_audio_current_time;
				m_sMpdManager.m_chunk_video_index =m_sMpdManager.Findbyterange_video_time(curent_audio_time,0);
                VOLOGI("Is_Need_Sync m_chunk_video_index  %lld",m_sMpdManager.m_chunk_video_index);
				VOLOGI("Is_Need_Sync m_chunk_audio_index  %lld",m_sMpdManager.m_chunk_audio_index);
				Is_Need_Sync = VO_FALSE;

			}
			if((pPlaySession->is_pure_audio&&m_sMpdManager.m_need_send_track_video)||(m_is_pure_audio&&!m_sMpdManager.Is_Muxed(m_ProgramId)))
			{
				ulRet = VO_RET_SOURCE2_OUTPUTNOTAVALIBLE;
				VOLOGI( "AV_SYNC_send_track_video  VO_RET_SOURCE2_OUTPUTNOTAVALIBLE");
				if((duration_video!=duration_audio)&&(m_sMpdManager.m_is_dash_if||m_sMpdManager.m_MpdParser.Is_IndexRange_audio()))
				{
					VOLOGI( "AV_SYNC_send_track_video m_chunk_audio_index:  %lld" , m_sMpdManager.m_chunk_audio_index);
					m_sMpdManager.m_chunk_video_index =m_sMpdManager.Findbyterange_video_time(curent_audio_time,0);
					VOLOGI( "AV_SYNC_send_track_video m_chunk_video_index:  %lld" , m_sMpdManager.m_chunk_video_index);
					if(m_sMpdManager.current_video_rpe&&m_sMpdManager.current_video_rpe->is_init_data == VO_TRUE||duration_video==0)
					{
						VOLOGI("AV_SYNC_send_track_video video_duration==0  Is_Need_Sync");
						Is_Need_Sync = VO_TRUE;

					}
					//m_sMpdManager.m_chunk_video_index = (duration_video*(m_sMpdManager.m_chunk_audio_index))/duration_audio;
				}
				else
				{
					m_sMpdManager.m_chunk_video_index = m_sMpdManager.m_chunk_audio_index+1;
					VOLOGI( "AV_SYNC_send_track_video   the duration_video == duration_audio m_chunk_video_index:  %lld" , m_sMpdManager.m_chunk_video_index);
				}
				if(m_audio_end)
				{
					ulRet =  VO_RET_SOURCE2_END;
					VOLOGI("m_audio_end1 VO_RET_SOURCE2_END");
					m_video_end = VO_TRUE;

				} 

				return ulRet;
			}
			else if(pPlaySession->is_pure_audio&&!m_sMpdManager.m_need_send_track_video)
			{
				ulRet = VO_RET_SOURCE2_OUTPUTNOTAVALIBLE;
				VOLOGI( "AV_SYNC_send_track_video22  VO_RET_SOURCE2_OUTPUTNOTAVALIBLE");
				if((duration_video!=duration_audio)&&(m_sMpdManager.m_is_dash_if||m_sMpdManager.m_MpdParser.Is_IndexRange_audio()))
				{
					m_sMpdManager.m_chunk_video_index =m_sMpdManager.Findbyterange_video_time(curent_audio_time,0);
					VOLOGI( "AV_SYNC_send_track_video22  m_chunk_video_index:  %lld" , m_sMpdManager.m_chunk_video_index);
					VOLOGI( "AV_SYNC_send_track_video22  m_chunk_audio_index:  %lld" , m_sMpdManager.m_chunk_audio_index);
					if(m_sMpdManager.current_video_rpe&&m_sMpdManager.current_video_rpe->is_init_data == VO_TRUE||duration_video==0)
					{
						VOLOGI("AV_SYNC_send_track_video22 video_duration==0  Is_Need_Sync");
						Is_Need_Sync = VO_TRUE;

					}
					//m_sMpdManager.m_chunk_video_index = (duration_video*(m_sMpdManager.m_chunk_audio_index))/duration_audio;
				}
				else
				m_sMpdManager.m_chunk_video_index = m_sMpdManager.m_chunk_audio_index+1;
				if(m_audio_end)
				{
					ulRet =  VO_RET_SOURCE2_END;
					VOLOGI("m_audio_end2 VO_RET_SOURCE2_END");
					m_video_end = VO_TRUE;
				} 
				/*end*/
				return ulRet;
			}
			break;
		}
	case VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE:
		{
			VO_U32 count_subtitle = 0;
			pAdaptiveItem = &m_sCurrentAdaptiveStreamItemForAlterSubtitle;
			VO_U32 ret = m_sMpdManager.GetMainSubtitleCount(&count_subtitle);
			if(count_subtitle ==0||!m_sMpdManager.m_need_send_track_subtitle&&m_sMpdManager.m_MpdParser.m_is_ttml)
				return VO_RET_SOURCE2_END;
			else
				ePlayListType = E_X_MEDIA_SUBTITLE_STREAM;
			break;
		}		
	default:
		{ 
			return DASH_ERR_NOT_EXIST;
		}
	}

	// Compute_Chunk_index();
	memset(&varChunckItem, 0x00, sizeof(S_CHUNCK_ITEM));
	ulRet = m_sMpdManager.GetTheStreamCurrentItem(ePlayListType, &varChunckItem);
	if(ulRet != 0)
	{
		if(m_sMpdManager.Is_Live()&&ulRet==-1)
		{
			return VO_RET_SOURCE2_FAIL;
			VOLOGW("Live  Return VO_RET_SOURCE2_FAIL");

		}
		else if(m_sMpdManager.Is_Live())
		{
			m_enRefresh = VO_FALSE; //for force to refresh
			VOLOGW("Live  Return refresh the playlist");
			return VO_RET_SOURCE2_NEEDRETRY;

		}
		else
		{

			if(ePlayListType==E_X_MEDIA_AUDIO_STREAM)
			{
				m_audio_end = VO_TRUE;
				VOLOGW("Vod  audio Return VO_RET_SOURCE2_END");		
			}
			else if(ePlayListType==E_X_MEDIA_VIDEO_STREAM)
			{
				m_video_end = VO_TRUE;
				VOLOGW("Vod  video  Return VO_RET_SOURCE2_END");
			}
			else if(ePlayListType==E_X_MEDIA_AV_STREAM)
			{
				m_audio_end = VO_TRUE;
				m_video_end = VO_TRUE;
				VOLOGW("Vod  AV  Return VO_RET_SOURCE2_END");
			}
			return VO_RET_SOURCE2_END;
		}
	}
	else
	{
		memset(pAdaptiveItem->szUrl, 0x00, 2048);
		memcpy(pAdaptiveItem->szUrl, varChunckItem.strChunckItemURL, strlen(varChunckItem.strChunckItemURL));
		VO_CHAR * mpd_base_url = NULL;
		mpd_base_url = m_sMpdManager.m_MpdParser.GetMPDUrl();
		if(mpd_base_url!=NULL)
		{
		if(!strstr(mpd_base_url,"cdn"))
		{
			if(strstr(mpd_base_url,"http")||strstr(mpd_base_url,"ITEC"))
				strcpy(mpd_root_url, mpd_base_url);
		}
		}
		/*VO_CHAR url[MAXURLLEN] = {0};
	    memset( url, 0x00, sizeof(url) * sizeof(VO_CHAR) );
	    GetTheAbsolutePath(url,mpd_root_url,mpd_base_url);
		*/
		VO_U32 period_index =m_ProgramId;
		VO_CHAR  period_base_url[1024];
		memset(period_base_url,0x00,sizeof(period_base_url));
		strcpy(period_base_url,m_sMpdManager.m_manifest.m_period[period_index].period_base_url);
		VO_U32 len = strlen(period_base_url);
		if(len)
		memcpy(pAdaptiveItem->szRootUrl,period_base_url, strlen(period_base_url));
		else
		memcpy(pAdaptiveItem->szRootUrl,mpd_root_url, strlen(mpd_root_url));
		pAdaptiveItem->ullDuration = (varChunckItem.ulDurationInMsec);
		pAdaptiveItem->ullChunkDeadTime = varChunckItem.ullEndTime;
		pAdaptiveItem->ullChunkLiveTime = varChunckItem.ullBeginTime;
		pAdaptiveItem->ullStartTime = varChunckItem.StartTime;
		if(varChunckItem.range_flag==VO_FALSE)
		{
			pAdaptiveItem->ullChunkOffset = INAVALIBLEU64;
			pAdaptiveItem->ullChunkSize = INAVALIBLEU64;
		}
		else
		{
			pAdaptiveItem->ullChunkOffset = varChunckItem.ulChunckOffset;
			pAdaptiveItem->ullChunkSize = varChunckItem.ulChunckLen;
		}
		*ppChunk = pAdaptiveItem;
		if(varChunckItem.eChunckContentType == E_CHUNK_TRACK)
		{
			(*ppChunk)->Type = VO_SOURCE2_ADAPTIVESTREAMING_HEADDATA;
			if(m_sMpdManager.m_MpdParser.m_is_ttml&&ePlayListType==E_X_MEDIA_SUBTITLE_STREAM)
			{
				(*ppChunk)->Type = VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE;
			}
			if(pAdaptiveItem->ullChunkSize == INAVALIBLEU64&&NULL==m_sMpdManager.m_MpdParser.m_ptr_FI_head)
			{
				if(ePlayListType!=E_X_MEDIA_SUBTITLE_STREAM)
				(*ppChunk)->Type = VO_SOURCE2_ADAPTIVESTREAMING_INITDATA;	
			}
			(*ppChunk)->ullTimeScale = 1;
#ifdef _new_programinfo
			(*ppChunk)->sKeyID.uProgramID = m_ProgramId;
			(*ppChunk)->sKeyID.uStreamID = m_cur_stream_id;
			(*ppChunk)->uChunkID = -1;
			if(ePlayListType == E_X_MEDIA_AV_STREAM)
			{
				(*ppChunk)->sKeyID.uTrackID =m_cur_track_video_id;
			}
			else if(ePlayListType == E_X_MEDIA_SUBTITLE_STREAM)
			{
				(*ppChunk)->sKeyID.uTrackID =m_cur_subtitle_track_id;
			}
			else
				(*ppChunk)->sKeyID.uTrackID = (ePlayListType == E_X_MEDIA_VIDEO_STREAM ?m_cur_track_video_id:m_cur_track_audio_id);
#endif
		}
		else if(varChunckItem.eChunckContentType == E_CHUNK_VIDEO)
		{
			(*ppChunk)->Type = VO_SOURCE2_ADAPTIVESTREAMING_VIDEO;
			(*ppChunk)->ullTimeScale = m_sMpdManager.Get_Video_Scale(m_ProgramId)==0?1:m_sMpdManager.Get_Video_Scale(m_ProgramId);
#ifdef _new_programinfo
			(*ppChunk)->sKeyID.uProgramID = m_ProgramId;
			(*ppChunk)->sKeyID.uStreamID = m_cur_stream_id;
			(*ppChunk)->sKeyID.uTrackID = m_cur_track_video_id;
			(*ppChunk)->uChunkID = -1;
#endif
				if(varChunckItem.is_SidBox)
			{
				(*ppChunk)->Type = VO_SOURCE2_ADAPTIVESTREAMING_SEGMENTINDEX;
				(*ppChunk)->ullTimeScale  = 1000;
			}
			VOLOGW("Video_url  %s  Video_starrt_time %lld  Chunk_duration %lld",(*ppChunk)->szUrl,(*ppChunk)->ullStartTime,(*ppChunk)->ullDuration);

		}
		else if(varChunckItem.eChunckContentType ==E_CHUNK_AUDIO )
		{
			(*ppChunk)->Type = VO_SOURCE2_ADAPTIVESTREAMING_AUDIO;
			(*ppChunk)->ullTimeScale = m_sMpdManager.Get_Audio_Scale(m_ProgramId)==0?1: m_sMpdManager.Get_Audio_Scale(m_ProgramId);
#ifdef _new_programinfo
			(*ppChunk)->sKeyID.uProgramID = m_ProgramId;
			(*ppChunk)->sKeyID.uStreamID = m_cur_stream_id;
			(*ppChunk)->sKeyID.uTrackID = m_cur_track_audio_id;
			(*ppChunk)->uChunkID = -1;
#endif
			if(varChunckItem.is_SidBox)
			{
				(*ppChunk)->Type = VO_SOURCE2_ADAPTIVESTREAMING_SEGMENTINDEX;
				(*ppChunk)->ullTimeScale  = 1000;
				
			}
			VOLOGW("Audio_url  %s  Audio_start_time %lld  Chunk_duration %d",(*ppChunk)->szUrl,(*ppChunk)->ullStartTime,(*ppChunk)->ullDuration);
		}
		/*add for ts_parser and muxed file*/
		else if(varChunckItem.eChunckContentType == E_CHUNK_AV )
		{
			(*ppChunk)->Type  = VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO;
			if((m_sMpdManager.m_chunk_video_index==1||m_BA_current_chunk_flag)&&m_sMpdManager.Is_Ts())
			{
                VOLOGW("Ts Chunk do BA");
				(*ppChunk)->uFlag = VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;
			}
			(*ppChunk)->ullTimeScale = m_sMpdManager.Get_Video_Scale(m_ProgramId)==0?1:m_sMpdManager.Get_Video_Scale(m_ProgramId);
#ifdef _new_programinfo
			(*ppChunk)->sKeyID.uProgramID = m_ProgramId;
			(*ppChunk)->sKeyID.uStreamID = m_cur_stream_id;
			(*ppChunk)->sKeyID.uTrackID =m_cur_track_video_id;
			(*ppChunk)->uChunkID = -1;
			if(varChunckItem.is_SidBox)
			{
				(*ppChunk)->Type = VO_SOURCE2_ADAPTIVESTREAMING_SEGMENTINDEX;
				(*ppChunk)->ullTimeScale  = 1000;
				
			}
#endif
			VOLOGW("AV_url  %s  AV_time %lld  Chunk_duration %lld",(*ppChunk)->szUrl,(*ppChunk)->ullStartTime,(*ppChunk)->ullDuration);


		}
		else if(varChunckItem.eChunckContentType == E_CHUNK_SUBTITLE)
		{
			(*ppChunk)->Type  = VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE;
			(*ppChunk)->ullTimeScale = 1;
			if(m_sMpdManager.m_need_send_track_subtitle )
			{
			(*ppChunk)->uFlag = VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;
			m_sMpdManager.m_need_send_track_subtitle = VO_FALSE;
			}
#ifdef _new_programinfo
			(*ppChunk)->sKeyID.uProgramID = m_ProgramId;
			(*ppChunk)->sKeyID.uStreamID = m_cur_stream_id;
			(*ppChunk)->sKeyID.uTrackID =m_cur_subtitle_track_id;
			(*ppChunk)->uChunkID = -1;
#endif
		}
		/*add for ts_parser*/
		return ulRet;
	}

}

VO_U32  C_DASH_Entity::Create_ProgramInfo_ts(VO_U32 ulProgramId)
{
	VO_U32			  ulCount = 0;
	VO_U32			  ulStreamId = 0;
	VO_U32            ulIndex = 0;

	if(m_pProgramInfo == NULL)
	{
		return VO_RET_SOURCE2_NEEDRETRY;
	}

	if(m_sMpdManager.GetMainStreamCount(&ulCount,ulProgramId) != 0)
	{
		return VO_RET_SOURCE2_NEEDRETRY;
	}
	m_pProgramInfo[ulProgramId].sProgramType =(m_sMpdManager.Is_Live())? VO_SOURCE2_STREAM_TYPE_LIVE :VO_SOURCE2_STREAM_TYPE_VOD;
	m_pProgramInfo[ulProgramId].uProgramID = 1;
	sprintf(m_pProgramInfo[ulProgramId].strProgramName,"%s", "DASH");
	m_pProgramInfo[ulProgramId].ppStreamInfo = new _STREAM_INFO*[ulCount];
	memset(m_pProgramInfo[ulProgramId].ppStreamInfo, 0x00, ulCount);
	m_pProgramInfo[ulProgramId].uStreamCount = ulCount;
	Representation * pl = m_sMpdManager.m_manifest.adaptionSet[0].m_ptr_QL_head;
	for(int n = 0; n < ulCount ;n++)
	{
		_STREAM_INFO *pStreamInfo = m_pProgramInfo[ulProgramId].ppStreamInfo[n] = new _STREAM_INFO;
		memset(pStreamInfo, 0x00, sizeof(_STREAM_INFO));
		pStreamInfo->uTrackCount = 1;
		pStreamInfo->ppTrackInfo = new _TRACK_INFO* [pStreamInfo->uTrackCount];
		memset(pStreamInfo->ppTrackInfo, 0x00 , pStreamInfo->uTrackCount);
		//Representation * pl = m_sMpdManager.m_manifest.adaptionSet[n].m_ptr_QL_head;
		for(int l =0; l < pStreamInfo->uTrackCount; l ++)
		{
		//	Representation * pl = m_sMpdManager.m_manifest.adaptionSet[n].m_ptr_QL_head;
			pStreamInfo->uStreamID = n;
			pStreamInfo->ppTrackInfo[l] = new _TRACK_INFO;
			pStreamInfo->uBitrate = pl->bitrate;
			memset(pStreamInfo->ppTrackInfo[l], 0x00, sizeof(_TRACK_INFO));

		}
			pl = pl->ptr_next;
	} 
	Representation * ql = m_sMpdManager.m_manifest.adaptionSet[0].m_ptr_QL_head;
	for(int j = 0; j < ulCount ;j++)
	{
		_STREAM_INFO *pStreamInfo2 = m_pProgramInfo[ulProgramId].ppStreamInfo[j];
		_TRACK_INFO *pTrackInfo = pStreamInfo2->ppTrackInfo[0] ;
#ifdef _new_programinfo
		pTrackInfo->uASTrackID = j;
#else
		pTrackInfo->uTrackID = pStreamInfo2->uTrackCount;
#endif
		pTrackInfo->uSelInfo = 0;
		pTrackInfo->uBitrate = ql->bitrate;
		pTrackInfo->uTrackType = VO_SOURCE2_TT_MUXGROUP;
		pTrackInfo->uCodec  = m_sMpdManager.m_manifest.adaptionSet[0].nCodecType;
		pTrackInfo->uDuration = m_sMpdManager.m_manifest.mediaPresentationDuration *1000/ m_sMpdManager.m_manifest.m_period[ulProgramId].period_vtimescale;
		pTrackInfo->uBitrate = ql->bitrate;
		pTrackInfo->sVideoInfo.sFormat.Height = ql->video_QL.Height;
		pTrackInfo->sVideoInfo.sFormat.Width = ql->video_QL.Width;
		pTrackInfo->uHeadSize = 0;
		pTrackInfo->pHeadData = NULL;
		ql->rpe_item_index = j;
		ql = ql->ptr_next;
	}
	m_pProgramInfo[ulProgramId].ppStreamInfo[0]->uSelInfo = VO_SOURCE2_SELECT_SELECTED;
	VO_U32  count_audio = 0;
	VO_U32  ret = m_sMpdManager.GetMainAudioCount(&count_audio);
	if(count_audio!=0)
	{
#ifdef _new_programinfo
		m_last_audio_track_id = m_pProgramInfo[ulProgramId].ppStreamInfo[0]->ppTrackInfo[0]->uASTrackID;
#else
		m_last_audio_track_id = m_pProgramInfo[ulProgramId].ppStreamInfo[0]->ppTrackInfo[0]->uTrackID;
#endif
		m_cur_track_audio_id = m_last_audio_track_id;
		int len = strlen(m_pProgramInfo[ulProgramId].ppStreamInfo[0]->ppTrackInfo[0]->sAudioInfo.chLanguage);
		memcpy(m_last_chLanguage,m_pProgramInfo[ulProgramId].ppStreamInfo[0]->ppTrackInfo[0]->sAudioInfo.chLanguage,strlen(m_pProgramInfo[ulProgramId].ppStreamInfo[0]->ppTrackInfo[0]->sAudioInfo.chLanguage));
		m_last_chLanguage[len]= '\0';
	}
	VO_U32  count_subtitle= 0;
	ret = m_sMpdManager.GetMainSubtitleCount(&count_subtitle);
	if(count_subtitle!=0)
	{
		VO_U32						audio_uTrackCount = 0;
		m_sMpdManager.GetMainAudioBitrateCount(&audio_uTrackCount);
		m_cur_subtitle_track_id = audio_uTrackCount;
	}
		VO_U32  track_count = m_pProgramInfo[ulProgramId].ppStreamInfo[0]->uTrackCount -1;
#ifdef _new_programinfo
		m_cur_track_video_id =m_pProgramInfo[ulProgramId].ppStreamInfo[0]->ppTrackInfo[track_count]->uASTrackID;
#else
		m_cur_track_video_id =m_pProgramInfo[ulProgramId].ppStreamInfo[0]->ppTrackInfo[track_count]->uTrackID;
#endif
		m_cur_track_audio_id = 0;
		m_cur_stream_id = 0;
	return VO_RET_SOURCE2_OK;
}

VO_U32  C_DASH_Entity::Create_ProgramInfo(VO_U32 ulProgramId)
{
	VO_U32			  ulCount = 0;
	VO_U32			  ulIndex = 0;
	VO_U32			  ulChunckCount = 0;
	VO_U32			  ulRet = VO_RET_SOURCE2_OK;
	_STREAM_INFO*	   pStreamInfo = NULL;
	S_PLAY_SESSION*	  pPlaySession = NULL;
	VO_U32			  ulStreamId = 0;
	VO_U32			  ulTrackIndex = 0;
	VO_U32			  ulLen = 0;
	VO_BOOL			  bSel = VO_FALSE;
	VO_U32             m_audio_count = 0;
	VO_U32             m_video_count = 0;

	if(m_pProgramInfo == NULL)
	{
		return VO_RET_SOURCE2_NEEDRETRY;
	}
	if(m_sMpdManager.GetMainStreamCount(&ulCount,ulProgramId) != 0)
	{
		return VO_RET_SOURCE2_NEEDRETRY;
	}
	m_pProgramInfo[ulProgramId].ppStreamInfo = NULL;
	m_pProgramInfo[ulProgramId].sProgramType =(m_sMpdManager.Is_Live())? VO_SOURCE2_STREAM_TYPE_LIVE :VO_SOURCE2_STREAM_TYPE_VOD;
	VO_U32 m_group_audio = 0;
	m_sMpdManager.GetMainAudioCount(&m_group_audio);
	VO_U32 m_group_subtitle = 0;
	m_group_subtitle = m_sMpdManager.m_MpdParser.GetSubtitleCount();
	VO_U32 m_group_video = 0;
	m_sMpdManager.GetMainVideoCount(&m_group_video);
	VO_BOOL pure_audio = VO_FALSE;
	if(m_group_video==0&&m_group_audio!=0)
		pure_audio = VO_TRUE;
	VO_BOOL pure_video = VO_FALSE;
	if(m_group_video!=0&&m_group_audio==0)
		pure_video= VO_TRUE;
	//add for delete audio-only
	if(m_group_video!=0)
	ulCount-=m_group_audio;
#if _DISABLEPUREAUDIO
	if(m_group_audio!=0&&m_group_video!=0)
		ulCount +=1;
#endif
	//end 
	sprintf(m_pProgramInfo[ulProgramId].strProgramName,"%s", "DASH");
	m_pProgramInfo[ulProgramId].ppStreamInfo = new _STREAM_INFO*[ulCount];
	if(NULL == m_pProgramInfo[ulProgramId].ppStreamInfo)
	{
		VOLOGE("ppStreamInfo==NULL,ulCount=%d",ulCount);
		return VO_RET_SOURCE2_TRACKNOTFOUND;
	}
	memset(m_pProgramInfo[ulProgramId].ppStreamInfo, 0x00, ulCount);
	m_pProgramInfo[ulProgramId].uStreamCount = ulCount;
	for(int n = 0; n < ulCount ;n++)
	{
		_STREAM_INFO *pStreamInfo = m_pProgramInfo[ulProgramId].ppStreamInfo[n] = new _STREAM_INFO;
		memset(pStreamInfo, 0x00, sizeof(_STREAM_INFO));
		m_sMpdManager.GetMainAudioCount(&pStreamInfo->uTrackCount);
		VO_U32 subtile_count = 0;
		pStreamInfo->uTrackCount+=m_sMpdManager.GetMainSubtitleCount(&subtile_count);
#if _DISABLEPUREAUDIO
		if(n != ulCount -1)
			//end for delete audio-only add for audio only
#endif
			pStreamInfo->uTrackCount += 1;


		if(pStreamInfo->uTrackCount==0||pure_audio)
		{
			pStreamInfo->uTrackCount = 1;

		}
		if(pure_audio)
		{
			m_sMpdManager.GetMainAudioBitrateCount(&pStreamInfo->uTrackCount);
		}
		else if(!pure_video)
		{
			VO_U32						audio_uTrackCount = 0;
			m_sMpdManager.GetMainAudioBitrateCount(&audio_uTrackCount);
			pStreamInfo->uTrackCount +=(audio_uTrackCount-1);
		}

		pStreamInfo->ppTrackInfo = new _TRACK_INFO* [pStreamInfo->uTrackCount];
		memset(pStreamInfo->ppTrackInfo, 0x00 , pStreamInfo->uTrackCount);
		for(int l =0; l < pStreamInfo->uTrackCount; l ++)
		{
			pStreamInfo->ppTrackInfo[l] = new _TRACK_INFO;
			memset(pStreamInfo->ppTrackInfo[l], 0x00, sizeof(VO_SOURCE2_TRACK_INFO));
		}
		pStreamInfo->uTrackCount = 0;
		pStreamInfo ++;
	}
	int group_count =m_sMpdManager.m_manifest.group_count;
	int count = 0;
	for(VO_U32 i =0; i< group_count; i++)
	{ 
		if(m_sMpdManager.m_manifest.adaptionSet&&m_sMpdManager.m_manifest.adaptionSet[i].nTrackType == VO_SOURCE_TT_AUDIO&&m_sMpdManager.m_manifest.adaptionSet[i].period_index==ulProgramId)
		{
			Representation * ql = m_sMpdManager.m_manifest.adaptionSet[i].m_ptr_QL_head;
			while(ql)
			{
			for(int j =0; j < m_pProgramInfo[ulProgramId].uStreamCount; j ++)
			{ 
				_STREAM_INFO *pStreamInfo2 = m_pProgramInfo[ulProgramId].ppStreamInfo[j];
				pStreamInfo2->uStreamID = j;
				pStreamInfo2->uBitrate += ql->bitrate;
				VOLOGR("pStreamInfo2->uTrackCount %d",pStreamInfo2->uTrackCount);
				_TRACK_INFO *pTrackInfo = pStreamInfo2->ppTrackInfo[pStreamInfo2->uTrackCount] ;
#ifdef _new_programinfo
				pTrackInfo->uASTrackID = pStreamInfo2->uTrackCount;
#else
				pTrackInfo->uTrackID= pStreamInfo2->uTrackCount;
#endif
			//	m_sMpdManager.m_manifest.adaptionSet[i].track_id = pTrackInfo->uTrackID;
				pStreamInfo2->uTrackCount ++;
				pTrackInfo->uSelInfo = VO_SOURCE2_SELECT_DEFAULT;
				pTrackInfo->uTrackType = VO_SOURCE2_TT_AUDIO;
				pTrackInfo->uCodec  = m_sMpdManager. m_manifest.adaptionSet[i].nCodecType;
				if(m_sMpdManager.m_manifest.audio_scale!=0)
					pTrackInfo->uDuration = m_sMpdManager.m_manifest.mediaPresentationDuration *1000/m_sMpdManager.m_manifest.audio_scale;
				pTrackInfo->uBitrate = ql->bitrate;
				pTrackInfo->uHeadSize = 0;
				pTrackInfo->pHeadData = NULL;
				memcpy(pTrackInfo->sAudioInfo.chLanguage,m_sMpdManager.m_manifest.adaptionSet[i].lang,strlen(m_sMpdManager.m_manifest.adaptionSet[i].lang));
				sprintf(pTrackInfo->sAudioInfo.chLanguage,"%s-%lu",pTrackInfo->sAudioInfo.chLanguage,pTrackInfo->uASTrackID);
		 }
			ql =ql->ptr_next;
		 }
		}

	}//end for group_count
	for(VO_U32 i =0; i< group_count; i++)
	{
		if(m_sMpdManager.m_manifest.adaptionSet&&m_sMpdManager.m_manifest.adaptionSet[i].nTrackType == VO_SOURCE_TT_SUBTITLE&&m_sMpdManager.m_manifest.adaptionSet[i].period_index==ulProgramId)
		{
			Representation * ql = m_sMpdManager.m_manifest.adaptionSet[i].m_ptr_QL_head;
			for(int j =0; j < m_pProgramInfo[ulProgramId].uStreamCount; j ++)
			{
				_STREAM_INFO *pStreamInfo2 = m_pProgramInfo[ulProgramId].ppStreamInfo[j];
				pStreamInfo2->uStreamID = j;
				pStreamInfo2->uBitrate += ql->bitrate;
				_TRACK_INFO *pTrackInfo = pStreamInfo2->ppTrackInfo[pStreamInfo2->uTrackCount] ;
#ifdef _new_programinfo
				pTrackInfo->uASTrackID = pStreamInfo2->uTrackCount;
#else
				pTrackInfo->uTrackID= pStreamInfo2->uTrackCount;
#endif
				//	m_sMpdManager.m_manifest.adaptionSet[i].track_id = pTrackInfo->uTrackID;
				pStreamInfo2->uTrackCount ++;
				pTrackInfo->uSelInfo = VO_SOURCE2_SELECT_DEFAULT;
				pTrackInfo->uTrackType = VO_SOURCE2_TT_SUBTITLE;
				pTrackInfo->uBitrate = ql->bitrate;
				pTrackInfo->uHeadSize = 0;
				memcpy(pTrackInfo->sSubtitleInfo.chLanguage,m_sMpdManager.m_manifest.adaptionSet[i].lang,strlen(m_sMpdManager.m_manifest.adaptionSet[i].lang));
				sprintf(pTrackInfo->sSubtitleInfo.chLanguage,"%s-%lu",pTrackInfo->sSubtitleInfo.chLanguage,pTrackInfo->uASTrackID);
				pTrackInfo->pHeadData = NULL;
			}
		}
	}//end for group_count
	VO_U32  adaption_count = 0;
	for(VO_U32 i =0; i< group_count; i++)
	{
		if(m_sMpdManager.m_manifest.adaptionSet&&m_sMpdManager.m_manifest.adaptionSet[i].nTrackType == VO_SOURCE_TT_VIDEO&&m_sMpdManager.m_manifest.adaptionSet[i].period_index==ulProgramId)
		{
			adaption_count  = 0;
			Representation * ql = m_sMpdManager.m_manifest.adaptionSet[i].m_ptr_QL_head;
			if(ql->track_count==2&&group_count>2)
			{
				ql->rpe_track_id=-1;
			}
			while(ql&&ql->track_count!=2)
			{
				ql->rpe_track_id = -1;
				_STREAM_INFO *pStreamInfo = m_pProgramInfo[ulProgramId].ppStreamInfo[count++];
				pStreamInfo->uSelInfo = 0;
				pStreamInfo->uStreamID = count-1;
				adaption_count++;
				ql->rpe_item_index = adaption_count-1;
				pStreamInfo->uBitrate += ql->bitrate;
				_TRACK_INFO *pTrackInfo = pStreamInfo->ppTrackInfo[pStreamInfo->uTrackCount] ;
#ifdef _new_programinfo
				pTrackInfo->uASTrackID = pStreamInfo->uTrackCount+adaption_count+i;

#else
				pTrackInfo->uTrackID = pStreamInfo->uTrackCount+adaption_count+i;
#endif
				//m_sMpdManager.m_manifest.adaptionSet[i].track_id = pTrackInfo->uTrackID;
				//pTrackInfo->uTrackID = pStreamInfo->uTrackCount;
				pStreamInfo->uTrackCount++;
				pTrackInfo->uSelInfo = 0;
				pTrackInfo->uTrackType = VO_SOURCE2_TT_VIDEO;
				pTrackInfo->uCodec  = m_sMpdManager.m_manifest.adaptionSet[i].nCodecType;
				pTrackInfo->pHeadData = NULL;
				if(m_sMpdManager.m_manifest.video_scale!=0)
					pTrackInfo->uDuration = m_sMpdManager.m_manifest.mediaPresentationDuration *1000/ m_sMpdManager.m_manifest.video_scale;
				pTrackInfo->uBitrate = ql->bitrate;
				pTrackInfo->sVideoInfo.sFormat.Height = ql->video_QL.Height;
				pTrackInfo->sVideoInfo.sFormat.Width = ql->video_QL.Width;
				pTrackInfo->uHeadSize = 0;
				ql = ql->ptr_next;
			}
			continue;
		}
	}
	VO_U32  count_audio = 0;
	VO_U32  ret = m_sMpdManager.GetMainAudioCount(&count_audio);
	if(count_audio!=0)
	{
#ifdef _new_programinfo
		m_last_audio_track_id = m_pProgramInfo[ulProgramId].ppStreamInfo[0]->ppTrackInfo[0]->uASTrackID;
#else
		m_last_audio_track_id = m_pProgramInfo[ulProgramId].ppStreamInfo[0]->ppTrackInfo[0]->uTrackID;
#endif
		m_cur_track_audio_id = m_last_audio_track_id;
		int len = strlen(m_pProgramInfo[ulProgramId].ppStreamInfo[0]->ppTrackInfo[0]->sAudioInfo.chLanguage);
		memcpy(m_last_chLanguage,m_pProgramInfo[ulProgramId].ppStreamInfo[0]->ppTrackInfo[0]->sAudioInfo.chLanguage,strlen(m_pProgramInfo[ulProgramId].ppStreamInfo[0]->ppTrackInfo[0]->sAudioInfo.chLanguage));
		m_last_chLanguage[len]= '\0';
	}
	VO_U32  count_subtitle= 0;
	ret = m_sMpdManager.GetMainSubtitleCount(&count_subtitle);
	if(count_subtitle!=0)
	{
		VO_U32						audio_uTrackCount = 0;
		m_sMpdManager.GetMainAudioBitrateCount(&audio_uTrackCount);
		m_cur_subtitle_track_id = audio_uTrackCount;
	}
		VO_U32  track_count = m_pProgramInfo[ulProgramId].ppStreamInfo[0]->uTrackCount -1;
#ifdef _new_programinfo
		m_cur_track_video_id =m_pProgramInfo[ulProgramId].ppStreamInfo[0]->ppTrackInfo[track_count]->uASTrackID;
#else
		m_cur_track_video_id =m_pProgramInfo[ulProgramId].ppStreamInfo[0]->ppTrackInfo[track_count]->uTrackID;
#endif
	m_pProgramInfo[ulProgramId].ppStreamInfo[0]->uSelInfo = VO_SOURCE2_SELECT_SELECTED;
	m_cur_stream_id = 0;
	if(group_count>6&&m_sMpdManager.m_MpdParser.GetPeriodCount()==1)
		m_pProgramInfo[ulProgramId].ppStreamInfo[6]->uSelInfo = VO_SOURCE2_SELECT_SELECTED;
	return VO_RET_SOURCE2_OK;


}

VO_U32  C_DASH_Entity::Update_ProgramInfo(VO_U32 ulProgramId)
{

	VO_U32 group_count =m_sMpdManager.m_manifest.group_count;
	VO_U32  adaption_count = 0;
	for(VO_U32 i =0; i< group_count; i++)
	{
		if(m_sMpdManager.m_manifest.adaptionSet&&m_sMpdManager.m_manifest.adaptionSet[i].nTrackType == VO_SOURCE_TT_VIDEO&&m_sMpdManager.m_manifest.adaptionSet[i].period_index==ulProgramId)
		{
			adaption_count  = 0;
			Representation * ql = m_sMpdManager.m_manifest.adaptionSet[i].m_ptr_QL_head;		
			while(ql&&ql->track_count!=2)
			{
				ql->rpe_track_id = -1;
				adaption_count++;
				ql->rpe_item_index = adaption_count-1;
				ql = ql->ptr_next;
			}
			continue;
		}
	}
	VO_U32  count_audio = 0;
	VO_U32  ret = m_sMpdManager.GetMainAudioCount(&count_audio);
	S_PLAY_SESSION*	  pPlaySession = NULL;
	m_sMpdManager.m_CurrentPlaySession.strAlterAudioLanguage = m_last_chLanguage;
	ret = m_sMpdManager.GetTheCurrentPlaySession(&pPlaySession);
	VOLOGW("Update_ProgramInfo %d",pPlaySession->strAlterAudioGroupID)
	return ret;


}
VO_U32     C_DASH_Entity::Choose_Program(VO_U32  pTimeStamp)
{
	VO_U32 period_find_index = 0;
	VO_BOOL find = VO_FALSE;
	VO_U32  j = 0;
	if(!m_sMpdManager.m_manifest.m_period)
	{
		return VO_RET_SOURCE2_SEEKFAIL;
	}
	VO_U32 m_period_count = m_sMpdManager.m_manifest.period_count;
	if(m_period_count>1)
	{
		if(pTimeStamp> m_sMpdManager.m_manifest.m_period[m_period_count-1].period_start)
		{
			find = VO_TRUE;
			period_find_index = m_period_count-1;
			VOLOGW("Choose_Program1 m_ProgramId:%d  OK!", m_ProgramId); 
			VOLOGW("Choose_Program1 period_find_index:%d  OK!", period_find_index); 
		}
		else 
		{
			for(j=1;j<m_period_count;j++)
			{
				VO_U32  ul_time_pre = m_sMpdManager.m_manifest.m_period[j-1].period_start;
				VO_U32  ul_time_next = m_sMpdManager.m_manifest.m_period[j].period_start;
				if(pTimeStamp>=ul_time_pre&&pTimeStamp<ul_time_next)
				{
					find = VO_TRUE;
					period_find_index = j-1;
					VOLOGW("Choose_Program2:%d  OK!", period_find_index); 
					break;
				}
			}
		}
		if(period_find_index!= m_ProgramId&&find==VO_TRUE)
		{
			Change_Program(period_find_index);
		}
	}
	return 0;
}

VO_VOID   C_DASH_Entity::Change_Program(VO_U32  period_find_index)
{
	VO_U32 temp_program_id = period_find_index;
	VOLOGW("Change_Program1:%d  OK!", temp_program_id); 
	m_sMpdManager.Init_Period(temp_program_id); 
	if(!m_sMpdManager.Is_Ts()&&!m_sMpdManager.m_MpdParser.Is_dash_muxed(temp_program_id))
		Create_ProgramInfo(temp_program_id);
	else
		Create_ProgramInfo_ts(temp_program_id);
	m_sMpdManager.SelectDefaultAV_Index(temp_program_id);
	m_sMpdManager.m_chunk_audio_index = 0;
	m_sMpdManager.m_chunk_video_index = 0;
	m_bNewFormatArrive = VO_TRUE;
	m_sMpdManager.m_need_send_track_video = VO_TRUE;
	m_sMpdManager.m_need_send_track_audio = VO_TRUE;
	m_audio_end = VO_FALSE;
	m_video_end = VO_FALSE;
	Is_Need_Sync = VO_FALSE;
	m_sMpdManager.m_CurrentPlaySession.is_pure_audio = VO_FALSE;
	m_program_change_video = VO_TRUE;
	m_program_change_audio = VO_TRUE;
	m_program_change_subtitle = VO_TRUE;
	m_ProgramId = temp_program_id;
	VOLOGW("Change_Program2:%d  OK!", temp_program_id); 
	pure_audio = VO_FALSE; 
	if(NULL != m_pEventCallbackFunc && NULL != m_pEventCallbackFunc->SendEvent)
		m_pEventCallbackFunc->SendEvent(m_pEventCallbackFunc->pUserData, VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_PROGRAMRESET, (VO_U32)(&m_pProgramInfo[m_ProgramId]),  (VO_U32)NULL);

}
VO_U32     C_DASH_Entity::Seek_DASH(VO_U64*  pTimeStamp,VO_ADAPTIVESTREAMPARSER_SEEKMODE sSeekMode)
{

    voCAutoLock lock (&g_voMemMutex);
	VO_U64    ulRet = -1;
	VO_U64    ulTimeInput = (VO_U64)(*pTimeStamp);
	/*ulTimeInput = 2491848;*/
	VO_U32    ul_time= 0;
	VO_U32    ulDuration = 0;
	VOLOGW("Seek time11:%lld  OK!", ulTimeInput); 
	m_sMpdManager.GetDuration(&ul_time);
	if(m_pProgramInfo)
		ulRet = Choose_Program(ulTimeInput);
	else
		return VO_RET_SOURCE2_SEEKFAIL;
	VOLOGW("Seek time22 m_ProgramId :%d  OK!", m_ProgramId); 
	ulTimeInput-=  m_sMpdManager.m_manifest.m_period[m_ProgramId].period_start;
	VOLOGW("Seek time33:%lld  OK!", ulTimeInput); 
	if(m_sMpdManager.m_MpdParser.Is_IndexRange_audio())
	{
		m_sMpdManager.m_manifest.m_start_time = 0;
	}
	else
	ulTimeInput = VO_U64(ulTimeInput)*(m_sMpdManager.Get_Video_Scale(m_ProgramId))/1000;
	ulTimeInput+=  m_sMpdManager.m_manifest.m_start_time;
	m_sMpdManager.m_need_send_track_video = VO_TRUE;
	m_sMpdManager.m_need_send_track_audio = VO_TRUE;
	m_sMpdManager.m_need_send_track_subtitle = VO_TRUE;
	m_bNewFormatArrive = VO_TRUE;
	if(m_pProgramInfo&&m_pProgramInfo->sProgramType != VO_SOURCE2_STREAM_TYPE_VOD)
	{
		VO_U64  chunk_offset_duration = *pTimeStamp;
		VO_U64  chunk_duration = 0;
		if(m_sMpdManager.m_MpdParser.m_manifest&&m_sMpdManager.m_MpdParser.m_manifest->m_period[0].period_vtimescale!=0)
			chunk_duration = (m_sMpdManager.m_MpdParser.m_manifest->duration_video)*1000/m_sMpdManager.m_MpdParser.m_manifest->m_period[0].period_vtimescale;
		VO_U64 offset_index = 0;
		if(chunk_duration!=0)
			offset_index = chunk_offset_duration/chunk_duration;
		VOLOGW("Seek  offset_index :%lld   OK!", offset_index); 
		if(sSeekMode== VO_ADAPTIVESTREAMPARSER_SEEKMODE_BACKWARD)
		{
			m_sMpdManager.m_chunk_video_index-=offset_index;
			m_sMpdManager.m_chunk_audio_index-=offset_index;
			VOLOGW("Seek  VO_ADAPTIVESTREAMPARSER_SEEKMODE_BACKWARD"); 
			return 0;
		}
		else if(sSeekMode== VO_ADAPTIVESTREAMPARSER_SEEKMODE_FORWARD)
		{
			m_sMpdManager.m_chunk_video_index+=offset_index;
			m_sMpdManager.m_chunk_audio_index+=offset_index;
			VOLOGW("Seek  VO_ADAPTIVESTREAMPARSER_SEEKMODE_FORWARD"); 
			return 0;
		}
		else
		return VO_RET_SOURCE2_SEEKFAIL;
	}
	else
	{
		if(m_sMpdManager.m_MpdParser.Is_IndexRange_audio())
		{
			ulTimeInput = m_sMpdManager.Findbyterange_pos(ulTimeInput,m_ProgramId);
			
			ulTimeInput+=  m_sMpdManager.m_manifest.m_period[m_ProgramId].period_start;
			*pTimeStamp = ulTimeInput;
			VOLOGW("Seek return output: %lld",ulTimeInput);
			VOLOGW("Seek return time_stamp111: %lld",*pTimeStamp);
		}
		else
		{
			ulTimeInput = m_sMpdManager.SetThePos(ulTimeInput,m_ProgramId);
			if(ulTimeInput>m_sMpdManager.m_manifest.m_start_time)
			ulTimeInput-=m_sMpdManager.m_manifest.m_start_time;
			else
				ulTimeInput = 0;	
			ulTimeInput*=1000;
			if(m_sMpdManager.Get_Video_Scale(m_ProgramId)!=0)
			ulTimeInput/=(m_sMpdManager.Get_Video_Scale(m_ProgramId));
			else
				ulTimeInput/=(m_sMpdManager.Get_Audio_Scale(m_ProgramId));
			ulTimeInput+=  m_sMpdManager.m_manifest.m_period[m_ProgramId].period_start;
			*pTimeStamp = ulTimeInput;

		}
		ulRet = VO_RET_SOURCE2_OK;
	}


	if(ulRet == VO_RET_SOURCE2_OK)
	{
		/*if(ulTimeInput==0&&(!m_sMpdManager.m_MpdParser.Is_IndexRange_audio()))
		{
			m_sMpdManager.m_chunk_audio_index = 0;
			m_sMpdManager.m_chunk_video_index = 0;

		}*/
		
        VOLOGW("Seek time33 m_chunk_audio_index2: %lld",m_sMpdManager.m_chunk_audio_index);
		VOLOGW("Seek time33 m_chunk_video_index1: %lld",m_sMpdManager.m_chunk_video_index);
		//if(m_sMpdManager.m_manifest.period_count<1)
		// ulTimeInput = ulTimeInput*1000/m_sMpdManager.Get_Video_Scale(m_ProgramId);
		// ulTimeInput = ulTimeInput*1000/m_sMpdManager.Get_Video_Scale(m_ProgramId)+m_sMpdManager.m_manifest.m_period[m_ProgramId].period_start;
		m_bNewFormatArrive = VO_TRUE;
		m_audio_end = VO_FALSE;
		m_video_end = VO_FALSE;
	}
	else
	{
		ulRet = VO_RET_SOURCE2_SEEKFAIL;
		VOLOGI("Seek time:%d  Failed!", ulTimeInput);
	}
	VOLOGW("Seek return time_stamp: %lld",*pTimeStamp);
	return ulRet;
}

VO_U32     C_DASH_Entity::GetProgramCounts_DASH(VO_U32*  pProgramCounts)
{
	VO_U32    ulRet = 0;

	if(pProgramCounts == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	*pProgramCounts = 1;
	return 0;
}

VO_U32     C_DASH_Entity::GetProgramInfo_DASH(VO_U32 nProgramId, _PROGRAM_INFO **ppProgramInfo)
{

	*ppProgramInfo = &m_pProgramInfo[m_ProgramId];	 
	return 0;
}




VO_U32     C_DASH_Entity::SetParam_DASH(VO_U32 nParamID, VO_PTR pParam )
{
	VO_U32     ulRet = 0;

	if(pParam == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	switch(nParamID)
	{
	case VO_PID_SOURCE2_SAMPLECALLBACK:
		{
			VO_SOURCE2_SAMPLECALLBACK*   psCallbackSample = NULL;
			psCallbackSample = (VO_SOURCE2_SAMPLECALLBACK*)(pParam);
			m_pDataCallbackFunc = psCallbackSample;
			break;
		}
	case VO_PID_SOURCE2_EVENTCALLBACK:
		{
			VO_SOURCE2_EVENTCALLBACK*   psCallbackEvent = NULL;
			psCallbackEvent = (VO_SOURCE2_EVENTCALLBACK*)(pParam);
			m_pEventCallbackFunc = psCallbackEvent;
			break;
		}
	case VO_PID_ADAPTIVESTREAMING_SEGMENTINDEX_INFO:
		{
			VODS_SEG_INDEX_INFO * segment_info = NULL;
			segment_info = (VODS_SEG_INDEX_INFO *)(pParam);
			VO_U32 track_type = SearchTrackType(segment_info->uStreamID,segment_info->uTrackID);
			m_sMpdManager.AddByteRangeInfo(segment_info,track_type);
			break;
		}
	case VO_PID_ADAPTIVESTREAMING_INITDATA_INFO:
		{ 
			VODS_INITDATA_INFO * inidata_info = new VODS_INITDATA_INFO;
			inidata_info = (VODS_INITDATA_INFO *)(pParam);
			VO_U32 track_type = SearchTrackType(inidata_info->uStreamID,inidata_info->uTrackID);
			m_sMpdManager.AddInitData(inidata_info,track_type);	
			break;
		}	
	case VO_PID_ADAPTIVESTREAMING_UTC:
		{
			m_utc_time =*((VO_U64*) pParam);
			m_system_time = voOS_GetSysTime();
			m_sMpdManager.SetUTCTime(m_utc_time,m_system_time);
			break;
		}
	case  VO_PID_SOURCE2_LIBOP:
		if(pParam){
			ulRet = m_sMpdManager.m_MpdParser.SetLibOp((VO_LIB_OPERATOR*)pParam);

		}
		break;
	case  VO_PID_SOURCE2_WORKPATH:
		if(pParam)
		{
			ulRet = m_sMpdManager.m_MpdParser.SetWorkPath((VO_TCHAR *) pParam);
			VOLOGW("SetWorkPath Sucess,%s", pParam);

		}
		break;
	case VO_PID_COMMON_LOGFUNC:
		if(pParam)
		{
		//	VO_LOG_PRINT_CB * m_pVologCB = NULL;
		//	m_pVologCB = (VO_LOG_PRINT_CB *)pParam;
		//	vologInit (m_pVologCB->pUserData, m_pVologCB->fCallBack);
		}
		break;


	default:
		{
			break;
		}
	}

	return ulRet;
}

VO_VOID    C_DASH_Entity::ResetAllContext()
{
	m_bNewFormatArrive = VO_TRUE;
	m_bUpdateRunning = VO_FALSE;    
	m_bOpenComplete = VO_FALSE;
	m_pDataCallbackFunc = NULL;
	m_pEventCallbackFunc = NULL;
	Is_Need_Sync = VO_FALSE;
	DeleteAllProgramInfo();
}

VO_VOID    C_DASH_Entity::SetTheCurrentSelStream(VO_U32 uBitrate)
{
	return;
}


VO_U32    C_DASH_Entity::SetTheSelStreamInfo()
{
	VO_SOURCE2_STREAM_INFO*     pStreamInfo = NULL;
	VO_SOURCE2_TRACK_INFO*      pTrackInfo = NULL;
	S_PLAY_SESSION*    pPlaySession = NULL;
	VO_U32             ulRet = 0;
	VO_U32             ulIndex = 0;

	ulRet = m_sMpdManager.GetTheCurrentPlaySession(&pPlaySession);
	if(ulRet != 0)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}
	return 0;
}
VO_U32    C_DASH_Entity::GetDuration_DASH(VO_U64 * pDuration)
{
	VO_U32	 ulRet = 0;
	VO_U32	  ulDuration = 0;
	if(m_sMpdManager.Is_Live())
		return 0;

	if(pDuration == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	ulRet = m_sMpdManager.GetTheDuration(&ulDuration);
	if(ulRet != 0)
	{
		return VO_RET_SOURCE2_NEEDRETRY;
	}

	*pDuration = ulDuration;
	return 0;
}
VO_U32  C_DASH_Entity::SelectDefaultStream(VO_U32*  pDefaultBitrate)
{
	VO_U32   ulRet = 0;
	S_PLAY_SESSION*   pSession = NULL;
	m_sMpdManager.SetDefaultPlaySession(0);
	if(m_sMpdManager.GetTheCurrentPlaySession(&pSession) != 0)
	{
		return DASH_ERR_WRONG_MANIFEST_FORMAT;
	}
	*pDefaultBitrate = pSession->ulStreamBitrate;
	return ulRet;
}







VO_VOID C_DASH_Entity::SetDataCallbackFunc(VO_PTR   pCallbackFunc)
{
	if(pCallbackFunc != NULL)
	{
		m_pDataCallbackFunc = (VO_SOURCE2_SAMPLECALLBACK*)pCallbackFunc;
	}
}


VO_VOID C_DASH_Entity::SetEventCallbackFunc(VO_PTR   pCallbackFunc)
{
	if(pCallbackFunc != NULL)
	{
		m_pEventCallbackFunc = (VO_SOURCE2_EVENTCALLBACK*)pCallbackFunc;
	}
}

VO_U32       C_DASH_Entity::GetLicState()
{
    if(m_hCheck != NULL)
    {
        return VO_TRUE;
    }
    else
    {
        return VO_FALSE;
    }
}


