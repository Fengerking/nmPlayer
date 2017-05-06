#include "string.h"
#include "C_DASH_Entity.h"
#include "voLog.h"
#include "voOSFunc.h"



#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
typedef VO_S32 ( VO_API *pvoGetParserAPI)(VO_PARSER_API * pParser);
typedef VO_S32 ( VO_API *pvoGetSource2ParserAPI)(VO_SOURCE2_API* pParser);


C_DASH_Entity::C_DASH_Entity()
{
   
     m_pProgramInfo = NULL;
	 m_bOpenComplete = VO_FALSE;
	 m_bNewFormatArrive = VO_TRUE;
	 m_pLogParam = NULL;
	 m_bUpdateRunning = VO_TRUE;
	 m_enRefresh = VO_TRUE;
	 pure_audio = VO_FALSE; 
	 //
	 m_audio_end = VO_FALSE;
	 m_video_end = VO_FALSE;
	 m_last_index = 0;
	 m_write = VO_FALSE;
	 //
	 memset(&m_sCurrentAdaptiveStreamItemForPlayList, 0, sizeof(VO_ADAPTIVESTREAMPARSER_CHUNK));
     memset(&m_sCurrentAdaptiveStreamItemForMainStream, 0, sizeof(VO_ADAPTIVESTREAMPARSER_CHUNK));
     memset(&m_sCurrentAdaptiveStreamItemForAlterVideo, 0, sizeof(VO_ADAPTIVESTREAMPARSER_CHUNK));
     memset(&m_sCurrentAdaptiveStreamItemForAlterAudio, 0, sizeof(VO_ADAPTIVESTREAMPARSER_CHUNK));
	 memset(mpd_root_url, 0x00, sizeof(mpd_root_url));
}


C_DASH_Entity::~C_DASH_Entity()
{
   
}

VO_U32  C_DASH_Entity::Init_DASH(VO_ADAPTIVESTREAM_PLAYLISTDATA * pData , VO_SOURCE2_EVENTCALLBACK * pEventCallback)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;
    
    if(pEventCallback == NULL || pData == NULL)
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
    return ulRet;
}
    

VO_U32     C_DASH_Entity::Close_DASH()
{
	VOLOGE( "-Close_DASH" );
    VO_U32    ulRet = VO_RET_SOURCE2_OK;
    ResetAllContext();
  
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
    if(m_pProgramInfo != NULL)
    {
        DeleteAllProgramInfo();
        m_pProgramInfo = NULL;
    }
    
    m_pProgramInfo = new VO_SOURCE2_PROGRAM_INFO;
    if(m_pProgramInfo == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

    memset(m_pProgramInfo, 0, sizeof(VO_SOURCE2_PROGRAM_INFO));
    m_pProgramInfo->ppStreamInfo = NULL;
	ulRet = ParseDASHPlaylist(&m_sCurrentAdaptiveStreamItemForPlayList);
	if(m_sCurrentAdaptiveStreamItemForPlayList.pData != NULL)
    {
        delete[] m_sCurrentAdaptiveStreamItemForPlayList.pData;
        m_sCurrentAdaptiveStreamItemForPlayList.pData = NULL;
    }
	if(!m_sMpdManager.Is_Ts()&&!m_sMpdManager.m_MpdParser.Is_dash_muxed())
		ulRet = Create_ProgramInfo();
	else
		ulRet = Create_ProgramInfo_ts();

	
    m_sMpdManager.SelectDefaultAV_Index();
  	ulRet = m_sMpdManager.GetTheCurrentPlaySession(&pPlaySession);
	
	if(m_sMpdManager.IsPlaySessionReady() == VO_TRUE)
    {
        if(m_bOpenComplete == VO_FALSE && m_pEventCallbackFunc != NULL && m_pEventCallbackFunc->SendEvent != NULL)
        {
            m_pEventCallbackFunc->SendEvent(m_pEventCallbackFunc->pUserData, VO_EVENTID_SOURCE2_OPENCOMPLETE, NULL, NULL);
            m_bOpenComplete = VO_TRUE;
        }
	}
    return ulRet;
}
/*addd for formal release*/
VO_U32   C_DASH_Entity::Start_DASH()
{
    VO_U32 ulRet = 0;
	if(m_pProgramInfo->sProgramType ==VO_SOURCE2_STREAM_TYPE_LIVE )
	{
		begin();
	}
	 return ulRet;

}
/*ended for formal release*/

/*addd for formal release*/
VO_U32   C_DASH_Entity::Stop_DASH()
{
	VO_U32 ulRet = 0;
    StopPlaylistUpdate();
	return ulRet;
}
/*ended for formal release*/
VO_VOID    C_DASH_Entity::DeleteAllProgramInfo()
{
    VO_SOURCE2_STREAM_INFO*   pStreamInfo = NULL;

    if(m_pProgramInfo != NULL)
    {
        if(m_pProgramInfo->ppStreamInfo != NULL)
        {
            for( int i = 0 ; i < m_pProgramInfo->uStreamCount ; i++ )
			{
			    pStreamInfo = m_pProgramInfo->ppStreamInfo[i];
			    DeleteStreamInfo(pStreamInfo);
                delete pStreamInfo;
            }

            delete[] m_pProgramInfo->ppStreamInfo;
        }

        delete m_pProgramInfo;
        m_pProgramInfo = NULL;
    }
}
void   C_DASH_Entity::thread_function()
{
//	set_threadname( "Playlist Update" );
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
		
	 if(m_pProgramInfo->sProgramType ==VO_SOURCE2_STREAM_TYPE_LIVE)
	 {
		 m_bUpdateRunning = VO_TRUE;
		 int  pMainStreamCount = 0;
		  while(m_bUpdateRunning == VO_TRUE)
        { 
		
            ullStartTime = voOS_GetSysTime();
			ulTimeInterval = 11000;
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
                    ulRet = m_pEventCallbackFunc->SendEvent(m_pEventCallbackFunc->pUserData, VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_NEEDPARSEITEM, (VO_U32)(&sVarPlayListData), NULL);
					if(ulRet==0)
					{
					    ParseDASHPlaylist(&sVarPlayListData);
					//	Compute_Chunk_index();
					
					}
                }
            }

		  
			
			
	
            voOS_Sleep(50);
		}	

	 }
	 return 0;

}

VO_VOID    C_DASH_Entity::DeleteStreamInfo(VO_SOURCE2_STREAM_INFO*   pStreamInfo)
{
    VO_U32   ulIndex = 0;
    VO_SOURCE2_TRACK_INFO*    pTrackInfo = NULL;   
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
    VO_U32   ulRet = 0;
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
   	Compute_Chunk_index();
	if(m_write == VO_TRUE)
	{  
		VOLOGE("time is wrong2");
		/*FILE *x = fopen("d:/wrong","wb");
		fwrite(pPlaylistItem->pData, 1,pPlaylistItem->uDataSize,x);
		fclose(x);*/
		m_write = VO_FALSE;
	}
	VOLOGE("after  chunk_index %lld",m_sMpdManager.m_chunk_video_index);
	return  0;
}

VO_U32     C_DASH_Entity::SelectTrack_DASH(VO_U32 nTrackID)
{
	VO_U32   ulRet = 0;
	VO_U32   ulStreamIndex = 0xffffffff;
    VO_SOURCE2_TRACK_INFO*  pTrackInfo = NULL;
	VO_BOOL   bFindTrack = VO_FALSE;
    VO_U32     ulBitrate = 0;
	VO_U32     ulIndex = 0;
	VO_U32     pVideoConcern = NULL;
    VO_CHAR*  pAudioConcern = NULL;
	S_PLAY_SESSION*                    pPlaySession = NULL; 
   
	if(m_pProgramInfo == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }
	ulRet = m_sMpdManager.GetTheCurrentPlaySession(&pPlaySession);
	
	for(ulIndex=0; ulIndex<m_pProgramInfo->uStreamCount; ulIndex++)
	{
		 if( m_pProgramInfo->ppStreamInfo != NULL && m_pProgramInfo->ppStreamInfo[ulIndex] != NULL)
        {
            if(m_pProgramInfo->ppStreamInfo[ulIndex]->uSelInfo == VO_SOURCE2_SELECT_SELECTED)
            {
                ulStreamIndex = ulIndex;
                ulBitrate = m_pProgramInfo->ppStreamInfo[ulIndex]->uBitrate;
                break;
            }
        }

	}
	if(ulStreamIndex != 0xffffffff)
	{
		 for(ulIndex=0; ulIndex<m_pProgramInfo->ppStreamInfo[ulStreamIndex]->uTrackCount; ulIndex++)
        {
            if(m_pProgramInfo->ppStreamInfo[ulStreamIndex]->ppTrackInfo != NULL && m_pProgramInfo->ppStreamInfo[ulStreamIndex]->ppTrackInfo[ulIndex] != NULL)
            {
                pTrackInfo = m_pProgramInfo->ppStreamInfo[ulStreamIndex]->ppTrackInfo[ulIndex];
                if(pTrackInfo->uTrackID == nTrackID)
                {
                    bFindTrack = VO_TRUE;
                    break;
                }
            }
        }

	}
	if(bFindTrack == VO_TRUE)
	{
		switch(pTrackInfo->uTrackType)
        {
            case VO_SOURCE2_TT_AUDIO:
            {
				if(m_last_audio_track_id == nTrackID)
				{
					  pAudioConcern = m_last_chLanguage;
				}
				else
				{
					m_last_audio_track_id = nTrackID; 
                    pAudioConcern = pTrackInfo->sAudioInfo.chLanguage;
				}
                break;
            }
            case VO_SOURCE2_TT_VIDEO:
            {
                pVideoConcern = ulStreamIndex;

            }
        }
	}
	 
	   m_sMpdManager.m_need_send_track_audio = VO_TRUE;
	    VOLOGW( "SelectTrack_DASH LANG1 %s " , pAudioConcern); 
	   ulRet = m_sMpdManager.SelectCurrentPlaySession(ulBitrate, ulStreamIndex, pAudioConcern);
	   VOLOGW( "SelectTrack_DASH ulStreamIndex %d " , ulStreamIndex); 
	   /*add for force to refresh
	    if(m_sMpdManager.Is_Live())
	   {   
		VO_U64 Current_utc_time = (voOS_GetSysTime()-m_system_time+m_utc_time);
		VO_U64 Mpd_time = m_sMpdManager.m_MpdParser.GetAvailbilityTime()*1000;
		VO_U64 left_time = Current_utc_time-Mpd_time;
		VO_U64 duration = 0;
		if(m_sMpdManager.m_MpdParser.m_manifest->audio_scale!=0)
		 duration = (m_sMpdManager.m_MpdParser.m_manifest->duration_audio)*1000/m_sMpdManager.m_MpdParser.m_manifest->audio_scale;
		if(left_time>0&&duration>0)
		{
			m_sMpdManager.m_chunk_audio_index =(left_time*98)/(duration*100);
			m_sMpdManager.m_chunk_video_index = m_sMpdManager.m_chunk_audio_index ;
			VOLOGE("select_track chunk_index %d",m_sMpdManager.m_chunk_audio_index);

		}
	   }
		/*end*/
	  
	   return nTrackID;
}
VO_U32    C_DASH_Entity:: SelectProgram_DASH (VO_U32 uProgramId)
{
   return 0;
}

VO_U32     C_DASH_Entity::GetCurTrackInfo_DASH(VO_SOURCE2_TRACK_TYPE sTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo)
{
    return 0;
}


VO_U32     C_DASH_Entity::SelectStream_DASH(VO_U32 uStreamId)
{

	//uStreamId = 0;
	
   //voCAutoLock lock (&g_voMemMutex);

   if(m_sMpdManager.m_manifest.group_count>6)
	   uStreamId = 6;

	VOLOGI( "SelectStream_DASH %d " , uStreamId);
    VO_U32   ulRet = 0;
    VO_U32   ulIndex = 0;
    VO_BOOL  bFind = VO_FALSE;
    VO_U32   ulSequenceId = 0;
    VO_U32   ulNewBitrate = 0;
	m_bNewFormatArrive = VO_TRUE;
	 VO_U32   ulStreamIndex = 0;
    VO_ADAPTIVESTREAMPARSER_CHUNK*     pChunk = NULL;
	S_PLAY_SESSION*                    pPlaySession = NULL;
	VO_CHAR*  pVideoConcern = NULL;
    VO_CHAR*  pAudioConcern = NULL;

	
    if(m_pProgramInfo == NULL)
    {
        return VO_RET_SOURCE2_OUTPUTNOTFOUND;
    }

    if(m_pProgramInfo->ppStreamInfo == NULL)
    {  
        return VO_RET_SOURCE2_OUTPUTNOTFOUND;
    }
//	m_sMpdManager.m_need_send_track_audio = VO_TRUE;
	m_sMpdManager.m_need_send_track_video = VO_TRUE;
	m_bNewFormatArrive = VO_TRUE;
    ulRet = m_sMpdManager.GetTheCurrentPlaySession(&pPlaySession);
    for(ulIndex=0; ulIndex<m_pProgramInfo->uStreamCount; ulIndex++)
    {
        if(m_pProgramInfo->ppStreamInfo[ulIndex]->uStreamID == uStreamId)
        {
            bFind = VO_TRUE;
            ulNewBitrate = m_pProgramInfo->ppStreamInfo[ulIndex]->uBitrate;
			ulStreamIndex = ulIndex;
            break;
        }
    }


    if(bFind == VO_TRUE)
    {  
		if(ulNewBitrate == pPlaySession->ulStreamBitrate)
        {
		    m_pProgramInfo->ppStreamInfo[ulStreamIndex]->uSelInfo = VO_SOURCE2_SELECT_SELECTED;
		    ulRet = m_sMpdManager.SelectCurrentVideoStreamId(ulStreamIndex);
            return ulRet;
        }
		pPlaySession->ulStreamBitrate = ulNewBitrate;
		 memset(&m_sCurrentAdaptiveStreamItemForAdaption, 0, sizeof(VO_ADAPTIVESTREAMPARSER_CHUNK));
         pChunk = &m_sCurrentAdaptiveStreamItemForAdaption;
		 if(m_sMpdManager.GetCurrentSequenceId(&ulSequenceId) != 0)
        {
            return VO_RET_SOURCE2_OUTPUTNOTFOUND;
        }
		// ulRet = GetChunckItem(VO_ADAPTIVESTREAMPARSER_CHUNKTYPE_VIDEO ,&pChunk);
		 if(ulRet == 0)
		 {
			 m_bInBitrateAdaption = VO_TRUE;
			 m_bNewFormatArrive = VO_TRUE;
			 pPlaySession->ulMainStreamSequenceId = ulStreamIndex;
			
			 for(ulIndex=0; ulIndex<m_pProgramInfo->uStreamCount; ulIndex++)
            {
                if(m_pProgramInfo->ppStreamInfo[ulIndex]->uStreamID == uStreamId)
                {
                    m_pProgramInfo->ppStreamInfo[ulIndex]->uSelInfo = VO_SOURCE2_SELECT_SELECTED;
                }
                else
                {
                    m_pProgramInfo->ppStreamInfo[ulIndex]->uSelInfo = 0;
                }
            }
		 
		 }
		 VO_BOOL  pure_audio = VO_FALSE;
		 pPlaySession->is_pure_audio = VO_FALSE;
		  int count =  m_pProgramInfo->ppStreamInfo[ulStreamIndex]->uTrackCount;
		  int index = 0;
		  while(count)
		  {
			  if(m_pProgramInfo->ppStreamInfo[ulStreamIndex]->ppTrackInfo[index]->uTrackType == VO_SOURCE2_TT_AUDIO)
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
		  pPlaySession->is_pure_audio = pure_audio;
		  if(!pPlaySession->is_pure_audio )
		  {
			  pPlaySession->strAlterVideoSubStreamID = ulStreamIndex;
		  }
		
		  ulRet = m_sMpdManager.SelectCurrentVideoStreamId(ulStreamIndex);

	}


	return 0;
}

VO_VOID    C_DASH_Entity::Compute_Chunk_index()
{
	 if(m_sMpdManager.Is_Live())
	{
	
		VO_U64 Current_utc_time = (voOS_GetSysTime()-m_system_time+m_utc_time);
		VO_CHAR live_time[1024];
		timeToSting(Current_utc_time,live_time);
		VOLOGW("Current_utc_time %s",live_time);
		VO_U64 Mpd_time = m_sMpdManager.m_MpdParser.GetAvailbilityTime()*1000;
		VO_CHAR mpd_time[1024];
		timeToSting(Mpd_time,mpd_time);
		VOLOGW("mpd_utc_time %s",mpd_time);
		VO_U64 time_line_time = m_sMpdManager.m_MpdParser.GetTimeLineTime();
		VOLOGW("time_line_time %lld",time_line_time);
		VO_S64 left_time =(Current_utc_time-Mpd_time);
		#ifdef _USETIMELINE
		left_time-=time_line_time;
        #endif
		VO_U64 duration = 0;
		if(m_sMpdManager.m_MpdParser.m_manifest->audio_scale!=0)
			duration = (m_sMpdManager.m_MpdParser.m_manifest->duration_video)*1000/m_sMpdManager.m_MpdParser.m_manifest->video_scale;
		VOLOGE("left_time  %lld",left_time);	
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
			
			if(index>10)
			{
			if(duration<LIVE_NUMBER)
			m_sMpdManager.m_chunk_video_index =index-15;
			if(m_sMpdManager.m_chunk_video_index<0)
			{
			 m_sMpdManager.m_chunk_video_index+=(-m_sMpdManager.m_chunk_video_index);
			}
			m_sMpdManager.m_chunk_audio_index = m_sMpdManager.m_chunk_video_index ;
			VOLOGE("Compute_Chunk_index>10 chunk_index %lld",m_sMpdManager.m_chunk_video_index);
			}
			else
			{
				m_sMpdManager.m_chunk_video_index = index;
				m_sMpdManager.m_chunk_audio_index = index;
			    VOLOGE("Compute_Chunk_index<10 chunk_index %d",m_sMpdManager.m_chunk_video_index);
			}
			if(	m_sMpdManager.m_chunk_video_index>600)
			{
		      VOLOGE("Compute_Chunk_index out of bound");
			}

		}
		else if(left_time<0)
		{
			m_sMpdManager.m_chunk_audio_index =0;
			m_sMpdManager.m_chunk_video_index =0;
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
		VOLOGW("subtile return end");
		return VO_RET_SOURCE2_END;
		
	}
	
	if(m_bNewFormatArrive)
	{
		m_sMpdManager.Set_is_NewFormat(m_bNewFormatArrive);
		
	}

	/*VO_U32 tmpIndex = 0;
	VO_U32 *index = &tmpIndex;
	if(uID == VO_ADAPTIVESTREAMPARSER_CHUNKTYPE_VIDEO)
		index = &m_sMpdManager.m_chunk_video_index;
	else if(uID == VO_ADAPTIVESTREAMPARSER_CHUNKTYPE_AUDIO)
	    index = &m_sMpdManager.m_chunk_audio_index;
	tmpIndex = *index;
	*/

	if(m_sMpdManager.Is_Live()&&(voOS_GetSysTime()-m_system_time+m_utc_time+10000)<m_sMpdManager.m_MpdParser.GetAvailbilityTime()*1000)
	{
		ulRet = VO_RET_SOURCE2_NEEDRETRY;
		VOLOGW("the  mpd utc_time is in furture  the left_time is: %lld ",voOS_GetSysTime()-m_system_time+m_utc_time-m_sMpdManager.m_MpdParser.GetAvailbilityTime()*1000);
		return ulRet;
	}


	ulRet = GetChunckItem(uID, ppChunk);
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
					 VOLOGW("audio VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE");
                }
                m_bNewFormatArrive = VO_FALSE;
				m_sMpdManager.m_need_send_track_audio = VO_FALSE;
            }
				else
				{
					if(ulRet!= VO_RET_SOURCE2_END&&ulRet!=VO_RET_SOURCE2_NEEDRETRY&&ulRet!=VO_RET_SOURCE2_OUTPUTNOTAVALIBLE)
					 (*ppChunk)->uFlag = 0;
				}
            break;
        }
        case VO_SOURCE2_ADAPTIVESTREAMING_VIDEO:
		{
		
			if(m_sMpdManager.m_need_send_track_video)
            {
                if(ulRet == VO_RET_SOURCE2_OK)
                {
                    (*ppChunk)->uFlag = VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;
					(*ppChunk)->Type = VO_SOURCE2_ADAPTIVESTREAMING_HEADDATA;
					VOLOGW("video VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE ");

                }
                 m_bNewFormatArrive = VO_FALSE;
			  	m_sMpdManager.m_need_send_track_video = VO_FALSE;
            }
				else
				{
					if(ulRet!= VO_RET_SOURCE2_END&&ulRet!=VO_RET_SOURCE2_NEEDRETRY&&ulRet!=VO_RET_SOURCE2_OUTPUTNOTAVALIBLE)
					{
						(*ppChunk)->uFlag = 0;	
						VOLOGR( "Dash url  %s flag %d " , (*ppChunk)->szUrl, (*ppChunk)->uFlag);
					}
				}

          break;
           
        }
		case VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO:
			if(m_sMpdManager.m_need_send_track_video)
			{
				if(ulRet == VO_RET_SOURCE2_OK)
				{
					(*ppChunk)->uFlag = VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;
					(*ppChunk)->Type = VO_SOURCE2_ADAPTIVESTREAMING_HEADDATA;
				}
				m_bNewFormatArrive = VO_FALSE;
				m_sMpdManager.m_need_send_track_video = VO_FALSE;

			}
			else
			{
				if(ulRet!= VO_RET_SOURCE2_END&&ulRet!=VO_RET_SOURCE2_NEEDRETRY&&ulRet!=VO_RET_SOURCE2_OUTPUTNOTAVALIBLE)
				{
					(*ppChunk)->uFlag = 0;	
					VOLOGR("Dash url  %s flag %d " , (*ppChunk)->szUrl, (*ppChunk)->uFlag);
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
		return 0;
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
			if(count==0&&m_sMpdManager.m_MpdParser.Is_dash_muxed()&&m_audio_end)
			{
				return VO_RET_SOURCE2_END;
			}
			if(count==0&&m_sMpdManager.m_MpdParser.Is_dash_pure_video())
			{
				return VO_RET_SOURCE2_OUTPUTNOTAVALIBLE;
			}
			else if(count==0&&m_sMpdManager.m_MpdParser.Is_dash_muxed())
				return VO_RET_SOURCE2_NEEDRETRY;
			
            break;
        }
        case VO_SOURCE2_ADAPTIVESTREAMING_VIDEO:
        {
			VO_U32 count_audio = 0;
			VO_U32 ret = m_sMpdManager.GetMainAudioCount(&count_audio);
            pAdaptiveItem = &m_sCurrentAdaptiveStreamItemForAlterVideo;
			if(m_sMpdManager.Is_Ts()||(count_audio==0&&m_sMpdManager.Is_Muxed()))
			{
				ePlayListType = E_X_MEDIA_AV_STREAM;
				//VOLOGW("AV-chunk");
			}
			else
			{
				ePlayListType = E_X_MEDIA_VIDEO_STREAM;
				//VOLOGW("V-chunk");

			}
			if(pPlaySession->is_pure_audio&&m_sMpdManager.m_need_send_track_video)
			{
				ulRet = VO_RET_SOURCE2_OUTPUTNOTAVALIBLE;
				if(m_audio_end)
				{
					ulRet =  VO_RET_SOURCE2_END;
				    VOLOGW("pure audio so video track end");
				} 

		//		m_sMpdManager.m_chunk_video_index = (voOS_GetSysTime()-m_change_time)/(.m.m_manifest.duration_video/m_sMpdManager.Get_Video_Scale());
				return ulRet;
			}
			//else if(pPlaySession->is_pure_audio&&m_sMpdManager.GetChunk_video_index()!=0)
			else if(pPlaySession->is_pure_audio&&!m_sMpdManager.m_need_send_track_video)
			{
				ulRet = VO_RET_SOURCE2_NEEDRETRY;
				if(m_audio_end)
				{
					ulRet =  VO_RET_SOURCE2_END;
					VOLOGW("pure audio chunk so video chunk end");
				} 
				/*end*/
				return ulRet;
			}
            break;
        }
        default:
        { 
            return DASH_ERR_NOT_EXIST;
        }
    }

   // Compute_Chunk_index();
    memset(&varChunckItem, 0, sizeof(S_CHUNCK_ITEM));
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
    memset(pAdaptiveItem->szUrl, 0, 2048);
    memcpy(pAdaptiveItem->szUrl, varChunckItem.strChunckItemURL, strlen(varChunckItem.strChunckItemURL));
    memcpy(pAdaptiveItem->szRootUrl,mpd_root_url, strlen(mpd_root_url));
	pAdaptiveItem->ullDuration = varChunckItem.ulDurationInMsec;
	pAdaptiveItem->ullChunkDeadTime = varChunckItem.ullEndTime;
    pAdaptiveItem->ullChunkLiveTime = varChunckItem.ullBeginTime;
	pAdaptiveItem->ullStartTime = varChunckItem.StartTime;
	pAdaptiveItem->ullChunkOffset = INAVALIBLEU64;
	pAdaptiveItem->ullChunkSize = INAVALIBLEU64;
    *ppChunk = pAdaptiveItem;
	if(varChunckItem.eChunckContentType == E_CHUNK_TRACK)
	{
		(*ppChunk)->Type = VO_SOURCE2_ADAPTIVESTREAMING_HEADDATA;
		(*ppChunk)->ullTimeScale = 1;

	}
	else if(varChunckItem.eChunckContentType == E_CHUNK_VIDEO)
	{
		(*ppChunk)->Type = VO_SOURCE2_ADAPTIVESTREAMING_VIDEO;
		(*ppChunk)->ullTimeScale = m_sMpdManager.Get_Video_Scale()==0?1:m_sMpdManager.Get_Video_Scale();
		 VOLOGW("Video_url  %s  Video_starrt_time %lld  Chunk_duration %lld",(*ppChunk)->szUrl,(*ppChunk)->ullStartTime,(*ppChunk)->ullDuration)
	
		
	}
	else if(varChunckItem.eChunckContentType ==E_CHUNK_AUDIO )
	{
		(*ppChunk)->Type = VO_SOURCE2_ADAPTIVESTREAMING_AUDIO;
		(*ppChunk)->ullTimeScale = m_sMpdManager.Get_Audio_Scale()==0?1: m_sMpdManager.Get_Audio_Scale();
		VOLOGW("Audio_url  %s  Audio_start_time %lld  Chunk_duration %d",(*ppChunk)->szUrl,(*ppChunk)->ullStartTime,(*ppChunk)->ullDuration);
	}
	/*add for ts_parser and muxed file*/
	else if(varChunckItem.eChunckContentType == E_CHUNK_AV )
	{
		(*ppChunk)->Type  = VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO;
		if(m_sMpdManager.m_chunk_video_index==1&&m_sMpdManager.Is_Ts())
		{
			(*ppChunk)->uFlag = VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;
		}
		(*ppChunk)->ullTimeScale = m_sMpdManager.Get_Video_Scale()==0?1:m_sMpdManager.Get_Video_Scale();
	   VOLOGW("AV_url  %s  AV_time %lld  Chunk_duration %lld",(*ppChunk)->szUrl,(*ppChunk)->ullStartTime,(*ppChunk)->ullDuration);


	}
	/*add for ts_parser*/
    return ulRet;
	}
   
}

VO_U32  C_DASH_Entity::Create_ProgramInfo_ts()
{
    VO_U32			  ulCount = 0;
	VO_U32			  ulStreamId = 0;
	VO_U32			  ulProgramId = 0;
	VO_U32            ulIndex = 0;

	if(m_pProgramInfo == NULL)
	{
		return VO_RET_SOURCE2_NEEDRETRY;
	}
	ulProgramId = 1;
	if(m_sMpdManager.GetMainStreamCount(&ulCount) != 0)
	{
		return VO_RET_SOURCE2_NEEDRETRY;
	}
	m_pProgramInfo->sProgramType =(m_sMpdManager.Is_Live())? VO_SOURCE2_STREAM_TYPE_LIVE :VO_SOURCE2_STREAM_TYPE_VOD;
	m_pProgramInfo->uProgramID = 1;
	sprintf(m_pProgramInfo->strProgramName,"%s", "DASH");
	m_pProgramInfo->ppStreamInfo = new VO_SOURCE2_STREAM_INFO*[ulCount];
	memset(m_pProgramInfo->ppStreamInfo, 0x00, ulCount);
	m_pProgramInfo->uStreamCount = ulCount;
	for(int n = 0; n < ulCount ;n++)
	{
		VO_SOURCE2_STREAM_INFO *pStreamInfo = m_pProgramInfo->ppStreamInfo[n] = new VO_SOURCE2_STREAM_INFO;
		memset(pStreamInfo, 0x00, sizeof(VO_SOURCE2_STREAM_INFO));
		pStreamInfo->uTrackCount = 1;
		pStreamInfo->ppTrackInfo = new VO_SOURCE2_TRACK_INFO* [pStreamInfo->uTrackCount];
		memset(pStreamInfo->ppTrackInfo, 0x00 , pStreamInfo->uTrackCount);
		for(int l =0; l < pStreamInfo->uTrackCount; l ++)
		{
			pStreamInfo->ppTrackInfo[l] = new VO_SOURCE2_TRACK_INFO;
			memset(pStreamInfo->ppTrackInfo[l], 0x00, sizeof(VO_SOURCE2_TRACK_INFO));
		}
	} 
	Representation * ql = m_sMpdManager.m_manifest.adaptionSet[0].m_ptr_QL_head;
	for(int j = 0; j < ulCount ;j++)
	{
		VO_SOURCE2_STREAM_INFO *pStreamInfo2 = m_pProgramInfo->ppStreamInfo[j];
		VO_SOURCE2_TRACK_INFO *pTrackInfo = pStreamInfo2->ppTrackInfo[0] ;
		pTrackInfo->uTrackID = pStreamInfo2->uTrackCount;
		pTrackInfo->uSelInfo = 0;
		pTrackInfo->uBitrate = ql->bitrate;
		pTrackInfo->uTrackType = VO_SOURCE2_TT_VIDEO;
		pTrackInfo->uCodec  = m_sMpdManager.m_manifest.adaptionSet[0].nCodecType;
		pTrackInfo->uDuration = m_sMpdManager.m_manifest.mediaPresentationDuration *1000/ m_sMpdManager.m_manifest.video_scale;
		pTrackInfo->uBitrate = ql->bitrate;
		pTrackInfo->sVideoInfo.sFormat.Height = ql->video_QL.Height;
		pTrackInfo->sVideoInfo.sFormat.Width = ql->video_QL.Width;
		pTrackInfo->uHeadSize = ql->length_CPD;
		ql = ql->ptr_next;
	}
	m_pProgramInfo->ppStreamInfo[0]->uSelInfo = VO_SOURCE2_SELECT_SELECTED;
}

VO_U32  C_DASH_Entity::Create_ProgramInfo()
{
	   VO_U32			  ulCount = 0;
	   VO_U32			  ulIndex = 0;
	   VO_U32			  ulChunckCount = 0;
	   VO_U32			  ulRet = VO_RET_SOURCE2_OK;
	   VO_SOURCE2_STREAM_INFO*	   pStreamInfo = NULL;
	   S_PLAY_SESSION*	  pPlaySession = NULL;
	   VO_U32			  ulStreamId = 0;
	   VO_U32			  ulProgramId = 0;
	   VO_U32			  ulTrackIndex = 0;
	   VO_U32			  ulLen = 0;
	   VO_BOOL			  bSel = VO_FALSE;
	   E_PLAYLIST_TYPE	  ePlayListType;
	   VO_U32             m_audio_count = 0;
	   VO_U32             m_video_count = 0;

    if(m_pProgramInfo == NULL)
    {
        return VO_RET_SOURCE2_NEEDRETRY;
    }
     ulProgramId = 1;
    if(m_sMpdManager.GetMainStreamCount(&ulCount) != 0)
    {
        return VO_RET_SOURCE2_NEEDRETRY;
    }
	 m_pProgramInfo->sProgramType =(m_sMpdManager.Is_Live())? VO_SOURCE2_STREAM_TYPE_LIVE :VO_SOURCE2_STREAM_TYPE_VOD;
	 m_pProgramInfo->uProgramID = 1;
	 VO_U32 m_group_audio = 0;
	 m_sMpdManager.GetMainAudioCount(&m_group_audio);
	 //add for delete audio-only
	 ulCount-=m_group_audio;
	 //end 
	 sprintf(m_pProgramInfo->strProgramName,"%s", "DASH");
	 m_pProgramInfo->ppStreamInfo = new VO_SOURCE2_STREAM_INFO*[ulCount];
	 memset(m_pProgramInfo->ppStreamInfo, 0x00, ulCount);
	 m_pProgramInfo->uStreamCount = ulCount;
	 for(int n = 0; n < ulCount ;n++)
	 {
	 VO_SOURCE2_STREAM_INFO *pStreamInfo = m_pProgramInfo->ppStreamInfo[n] = new VO_SOURCE2_STREAM_INFO;
	 memset(pStreamInfo, 0x00, sizeof(VO_SOURCE2_STREAM_INFO));
	 m_sMpdManager.GetMainAudioCount(&pStreamInfo->uTrackCount);
	   //if(n != ulCount -1)
	 //end for delete audio-only
	  pStreamInfo->uTrackCount += 1;
	 
	 if(pStreamInfo->uTrackCount==0)
	 {
		  pStreamInfo->uTrackCount = 1;

	 }

	 pStreamInfo->ppTrackInfo = new VO_SOURCE2_TRACK_INFO* [pStreamInfo->uTrackCount];
	 memset(pStreamInfo->ppTrackInfo, 0x00 , pStreamInfo->uTrackCount);
	  for(int l =0; l < pStreamInfo->uTrackCount; l ++)
	  {
	 pStreamInfo->ppTrackInfo[l] = new VO_SOURCE2_TRACK_INFO;
	 memset(pStreamInfo->ppTrackInfo[l], 0x00, sizeof(VO_SOURCE2_TRACK_INFO));
	  }
	 pStreamInfo->uTrackCount = 0;
     pStreamInfo ++;
	  }
	 int group_count =m_sMpdManager.m_manifest.group_count;
	 int count = 0;
	 for(VO_U32 i =0; i< group_count; i++)
	 { 
		 if(m_sMpdManager.m_manifest.adaptionSet[i].nTrackType == VO_SOURCE_TT_AUDIO&&m_sMpdManager.m_manifest.adaptionSet[i].period_index==1)
	 	{
	 	  Representation * ql = m_sMpdManager.m_manifest.adaptionSet[i].m_ptr_QL_head;
		  for(int j =0; j < m_pProgramInfo->uStreamCount; j ++)
		 { 
		   VO_SOURCE2_STREAM_INFO *pStreamInfo2 = m_pProgramInfo->ppStreamInfo[j];
		   pStreamInfo2->uStreamID = j;
		   pStreamInfo2->uBitrate += ql->bitrate;
		   VOLOGR("pStreamInfo2->uTrackCount %d",pStreamInfo2->uTrackCount);
		   VO_SOURCE2_TRACK_INFO *pTrackInfo = pStreamInfo2->ppTrackInfo[pStreamInfo2->uTrackCount] ;
		   pTrackInfo->uTrackID = pStreamInfo2->uTrackCount;
		   pStreamInfo2->uTrackCount ++;
		   pTrackInfo->uSelInfo = VO_SOURCE2_SELECT_DEFAULT;
		   pTrackInfo->uTrackType = VO_SOURCE2_TT_AUDIO;
		   pTrackInfo->uCodec  = m_sMpdManager. m_manifest.adaptionSet[i].nCodecType;
		   if(m_sMpdManager.m_manifest.audio_scale!=0)
		   pTrackInfo->uDuration = m_sMpdManager.m_manifest.mediaPresentationDuration *1000/m_sMpdManager.m_manifest.audio_scale;
		   pTrackInfo->uBitrate = ql->bitrate;
		   memcpy(pTrackInfo->sAudioInfo.chLanguage,m_sMpdManager.m_manifest.adaptionSet[i].lang,strlen(m_sMpdManager.m_manifest.adaptionSet[i].lang));
		 }
	 	}
	 
	 }//end for group_count
	   VO_U32  adaption_count = 0;
	  for(VO_U32 i =0; i< group_count; i++)
	  {
		 if(m_sMpdManager.m_manifest.adaptionSet[i].nTrackType == VO_SOURCE_TT_VIDEO&&m_sMpdManager.m_manifest.adaptionSet[i].period_index==1)
	 	{
		adaption_count  = 0;
	 	Representation * ql = m_sMpdManager.m_manifest.adaptionSet[i].m_ptr_QL_head;
		while(ql&&ql->track_count!=2)
	   {
		 VO_SOURCE2_STREAM_INFO *pStreamInfo = m_pProgramInfo->ppStreamInfo[count++];
		pStreamInfo->uSelInfo = 0;
		pStreamInfo->uStreamID = count-1;
		adaption_count++;
		ql->rpe_item_index = adaption_count-1;
	//	pStreamInfo->uStreamID  = adaption_count-1;
	//	ql->rpe_item_index = pStreamInfo->uStreamID;
		pStreamInfo->uBitrate += ql->bitrate;
		VO_SOURCE2_TRACK_INFO *pTrackInfo = pStreamInfo->ppTrackInfo[pStreamInfo->uTrackCount] ;
		pTrackInfo->uTrackID = pStreamInfo->uTrackCount;
	    pStreamInfo->uTrackCount++;
		pTrackInfo->uSelInfo = 0;
		pTrackInfo->uTrackType = VO_SOURCE2_TT_VIDEO;
		pTrackInfo->uCodec  = m_sMpdManager.m_manifest.adaptionSet[i].nCodecType;
		if(m_sMpdManager.m_manifest.video_scale!=0)
		pTrackInfo->uDuration = m_sMpdManager.m_manifest.mediaPresentationDuration *1000/ m_sMpdManager.m_manifest.video_scale;
		pTrackInfo->uBitrate = ql->bitrate;
		pTrackInfo->sVideoInfo.sFormat.Height = ql->video_QL.Height;
		pTrackInfo->sVideoInfo.sFormat.Width = ql->video_QL.Width;
		pTrackInfo->uHeadSize = ql->length_CPD;
		ql = ql->ptr_next;
	   }
	   continue;
	  }
	  }
	  VO_U32  count_audio = 0;
	  VO_U32  ret = m_sMpdManager.GetMainAudioCount(&count_audio);
      if(ret!=0)
	  {
	  m_last_audio_track_id = m_pProgramInfo->ppStreamInfo[0]->ppTrackInfo[0]->uTrackID;
	  int len = strlen(m_pProgramInfo->ppStreamInfo[0]->ppTrackInfo[1]->sAudioInfo.chLanguage);
	  memcpy(m_last_chLanguage,m_pProgramInfo->ppStreamInfo[0]->ppTrackInfo[1]->sAudioInfo.chLanguage,strlen(m_pProgramInfo->ppStreamInfo[0]->ppTrackInfo[1]->sAudioInfo.chLanguage));
	  m_last_chLanguage[len]= '\0';
	  }
	  m_pProgramInfo->ppStreamInfo[0]->uSelInfo = VO_SOURCE2_SELECT_SELECTED;
	  if(group_count>6&&m_sMpdManager.m_MpdParser.GetPeriodCount()==1)
		 m_pProgramInfo->ppStreamInfo[6]->uSelInfo = VO_SOURCE2_SELECT_SELECTED;

}

VO_U32     C_DASH_Entity::Seek_DASH(VO_U64*  pTimeStamp)
{

    VO_U64    ulRet = -1;
    VO_U32    ulTimeInput = (VO_U64)(*pTimeStamp);
	VO_U32    ul_time= 0;
	VO_U32    ulDuration = 0;
    VO_U32    ul_ret = m_sMpdManager.GetDuration(&ul_time);
	//ulTimeInput = 129560; 
	//ulTimeInput = 142680;
	//http://10.2.64.44/project/verizon/mediaexcel/ibc_02/dash.mpd
	VOLOGW("Seek time1:%d  OK!", ulTimeInput); 
	//ulTimeInput=164000;
	//ulTimeInput = 400000;
	ulTimeInput = VO_U64(ulTimeInput)*(m_sMpdManager.GetTimeScaleMs())/1000;
    ulTimeInput+=  m_sMpdManager.m_manifest.m_start_time;
	m_sMpdManager.m_need_send_track_video = VO_TRUE;
	m_sMpdManager.m_need_send_track_audio = VO_TRUE;
	
	if(m_pProgramInfo->sProgramType != VO_SOURCE2_STREAM_TYPE_VOD)
    {
          return DASH_ERR_NOT_EXIST;
    }
    else
	{
		ulTimeInput = m_sMpdManager.SetThePos(ulTimeInput);
		ulRet = 0;
	}
	
	
    if(ulRet == 0)
    {
        VOLOGI("Seek time3:%d  OK!", ulTimeInput);
		if(ulTimeInput==0)
		{
			m_sMpdManager.m_chunk_audio_index = 0;
			m_sMpdManager.m_chunk_video_index = 0;

		}
		 ulTimeInput = ulTimeInput*1000/m_sMpdManager.GetTimeScaleMs();
		 VOLOGI("Seek time4:%d  OK!", ulTimeInput);
		 m_bNewFormatArrive = VO_TRUE;
    }
    else
    {
        VOLOGI("Seek time:%d  Failed!", ulTimeInput);
    }
    
    return 0;
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
    
VO_U32     C_DASH_Entity::GetProgramInfo_DASH(VO_U32 nProgramId, VO_SOURCE2_PROGRAM_INFO **ppProgramInfo)
{

    *ppProgramInfo = m_pProgramInfo;
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
		case VO_PID_ADAPTIVESTREAMING_UTC:
			{
				  m_utc_time =*((VO_U64*) pParam);
				  m_system_time = voOS_GetSysTime();
				  m_sMpdManager.SetUTCTime(m_utc_time,m_system_time);

				
				break;
			}

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
	m_pLogParam = NULL;    
    m_bUpdateRunning = VO_FALSE;    
    m_bInBitrateAdaption = VO_FALSE;
    m_bOpenComplete = VO_FALSE;
    m_pDataCallbackFunc = NULL;
    m_pEventCallbackFunc = NULL;
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
	m_sMpdManager.SetDefaultPlaySession();
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










