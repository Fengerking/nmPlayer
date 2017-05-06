#include "string.h"
#include "voOSFunc.h"
#include "C_MPD_Manager.h"
#include "voLog.h"
#include "voAdaptiveStreamParser.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
C_MPD_Manager::C_MPD_Manager()
{
	m_need_send_track_audio = VO_TRUE;
	m_need_send_track_video = VO_TRUE;
    m_ulDefaultBitrate = 0;
	memset(&m_CurrentPlaySession, 0, sizeof(S_PLAY_SESSION));
	memset(m_stream_url, 0x00, 0);
  //  memset(m_url_video, 0x00, 0);
//	memset(m_url_audio, 0x00, 0);
	m_group_video_index = 0;
	m_group_audio_index = 0;
	m_chunk_video_index = 0;
	m_chunk_audio_index = 0;
	m_manifest.adaptionSet = NULL; 
	m_manifest.group_count = 0;

	//m_ulTimeStampOffsetForLive = 0; 
}

C_MPD_Manager::~C_MPD_Manager()
{
	ReleaseAll();
}


VO_VOID C_MPD_Manager::ReleaseAll()
{
	//VOLOGE("C_MPD_Manager release 1");
 if(!m_manifest.adaptionSet)
		return;
	int i = 0;
	while(i<m_MpdParser.GetGroupCount())
	{
		Representation * ptr_item = m_manifest.adaptionSet[i].m_ptr_QL_head;
		while( ptr_item )//!= m_manifest.adaptionSet[i].m_ptr_QL_tail )
		{
			Representation * ptr_temp = ptr_item;
			Seg_item* item = ptr_temp->m_ptr_SG_head;
			while(item)//!=ptr_temp->m_ptr_SG_tail){
			{
				Seg_item* item_temp= item;
				item = item->ptr_next;
				//m_reader.find((int)item_temp);
				delete item_temp;
				item_temp = NULL;
			}
			//m_reader.find((int)item);
			ptr_item = ptr_item->ptr_next;
			delete ptr_temp;
			ptr_temp = NULL;
		}
		//delete ptr_item;

		m_manifest.adaptionSet[i].m_ptr_QL_head = m_manifest.adaptionSet[i].m_ptr_QL_tail = NULL;
		i++;

	}
	 if(m_manifest.adaptionSet){delete []m_manifest.adaptionSet; m_manifest.adaptionSet = NULL; m_manifest.group_count = 0;}
	   m_ulDefaultBitrate = 0;
	  m_chunk_video_index = 0;
	  m_chunk_audio_index = 0;
	  VOLOGE("C_MPD_Manager release OK");

}

VO_U32   C_MPD_Manager::ParseMpd(VO_BYTE* pM3uData, VO_U32 ulM3uDataLength, VO_CHAR* pM3uPath)
{
	ReleaseAll();
    VO_U32   ulRet = 0; 
    if(pM3uData == NULL || pM3uPath == NULL)
    {
        return DASH_ERR_EMPTY_POINTER;
    }
	
	SetSessionUrl(pM3uPath);
    ulRet = m_MpdParser.Parse(pM3uData, ulM3uDataLength,&m_manifest,pM3uPath);
	return ulRet;
}
VO_U32   C_MPD_Manager::SetSessionUrl(VO_CHAR* pM3uPath)
{
	memcpy(m_CurrentPlaySession.strStreamURL,pM3uPath,strlen(pM3uPath));
	return 0;

}


VO_U32  C_MPD_Manager::GetMainStreamCount(VO_U32*  pMainStreamCount)
{
	     *pMainStreamCount = m_MpdParser.GetStreamCount();
		 return 0;
}

VO_U32  C_MPD_Manager::GetMainVideoCount(VO_U32*  pMainStreamCount)
{
	    *pMainStreamCount = m_MpdParser.GetVideoCount();
		return 0;
}
VO_U32  C_MPD_Manager::GetMainAudioCount(VO_U32*  pMainStreamCount)
{
	*pMainStreamCount = m_MpdParser.GetAudioCount();
	return *pMainStreamCount;
}
VO_U32   C_MPD_Manager::GetDuration(VO_U32*  pTimeDuration)
{
    VO_U32   ulRet = 0;
    VO_U32   ulIndex = 0;
    VO_U32   ulDuration = 0;
    *pTimeDuration =  m_MpdParser.GetDuration();
	return 0;
}
VO_U32   C_MPD_Manager::GetMaxChunkIndex(VO_U32 type)
{
	VO_U32    ul_time= 0;
	VO_U32    ul_ret = GetDuration(&ul_time);	
	VO_U32    video_really_duration = 0;
	VO_U32    audio_really_duration = 0;
	VO_U32    left_video_index = 0;
	VO_U32    left_audio_index = 0;
	VO_U32    max_video_index = 0;
	VO_U32    max_audio_index = 0;
	VO_U32    max_index = 0;
	VO_S32    check_index =0;

	if(Get_Video_Scale()!=0&&Get_duration_video()!=0)
	{
		video_really_duration = Get_duration_video()*1000/Get_Video_Scale();
		max_video_index = ul_time/video_really_duration;
		left_video_index = ul_time%video_really_duration;
	}
	if( Get_duration_audio()!=0&&Get_Audio_Scale()!=0)
	{
		audio_really_duration = Get_duration_audio()*1000/Get_Audio_Scale();
		max_audio_index = ul_time/audio_really_duration;
		left_audio_index = ul_time%audio_really_duration;
	}
     check_index = max_video_index-max_audio_index;
	 if(check_index<0)
		check_index = max_audio_index-max_video_index;

	if(check_index>3)
	{
		max_index = max_video_index;
		if(left_video_index!=0)
		{
			max_index+=1;

		}
	}
	else
	{
			if(type==1)
			{
				if(left_video_index!=0)
				{
					max_index = max_video_index+1;
				//	max_index = max_video_index;

				}
				else
					max_index = max_video_index;

			}
			else if(type ==0)
			{

				if(left_audio_index!=0)
				{
					max_index = max_audio_index+1;
				//	max_index = max_audio_index;

				}
				else
					max_index = max_audio_index;
			}

	}
	return    max_index;

}

VO_U32   C_MPD_Manager::GetTheCurrentPlaySession(S_PLAY_SESSION**  ppPlaySession)
{

    if(ppPlaySession == NULL)
    {
        return DASH_ERR_EMPTY_POINTER;
    }
    *ppPlaySession = &m_CurrentPlaySession;
    return 0;
}

VO_U32  C_MPD_Manager::SelectCurrentPlaySession(VO_U32  ulBitrate, VO_U32 video_rpe_index, VO_CHAR*  pAudioConcern)
{
	m_CurrentPlaySession.ulStreamBitrate = ulBitrate;
	if(m_CurrentPlaySession.is_pure_audio)
	{
		m_CurrentPlaySession.strAlterAudioGroupID = video_rpe_index;
	}
	else
	{
		VO_U64 video_birate = m_MpdParser.get_qualityLevel(m_group_video_index,video_rpe_index)->bitrate;
	    m_CurrentPlaySession.strAlterVideoSubStreamID  = video_rpe_index;
	}
	if(pAudioConcern)
	{
		m_CurrentPlaySession.strAlterAudioLanguage = pAudioConcern;
		VOLOGW("lang %s ",pAudioConcern);
		m_CurrentPlaySession.strAlterAudioGroupID = m_MpdParser.get_lang_group(pAudioConcern);
		VOLOGW("m_CurrentPlaySession %d ",m_CurrentPlaySession.strAlterAudioGroupID);
	}
	return 0;
    
}
VO_U32  C_MPD_Manager::SelectCurrentVideoStreamId(VO_U32 video_rpe_index)
{
	//m_CurrentPlaySession.strAlterVideoSubStreamID  = video_rpe_index;
	if(m_CurrentPlaySession.is_pure_audio)
	{
		m_CurrentPlaySession.strAlterAudioGroupID = m_group_audio_index;
		m_CurrentPlaySession.ulStreamBitrate =  m_MpdParser.get_qualityLevel(m_group_audio_index,0)->bitrate;
	}
	else
	{
		m_CurrentPlaySession.strAlterVideoSubStreamID  = video_rpe_index;
	//	m_CurrentPlaySession.strAlterAudioGroupID = m_group_audio_index;
		m_CurrentPlaySession.is_pure_audio = VO_FALSE;
	}

	return 0;
    
}
VO_U32  C_MPD_Manager::GetCurrentSequenceId(VO_U32*   pCurrentSequenceId)
{
        if(1)
        {
			*pCurrentSequenceId = m_CurrentPlaySession.ulMainStreamSequenceId;
			return 0;
		}
		else
        return DASH_ERR_NOT_EXIST;
}
VO_U32  C_MPD_Manager::GetCurrentVideoSequenceId(VO_U32*   pCurrentSequenceId)
{
	  return m_CurrentPlaySession.ulAlterVideoSequenceId;
}

VO_U32  C_MPD_Manager::GetCurrentAudioSequenceId(VO_U32*   pCurrentSequenceId)
{
	  return m_CurrentPlaySession.ulAlterAudioSequenceId;
}


VO_U32   C_MPD_Manager::SetDefaultPlaySession()
{
	m_CurrentPlaySession.ulMainStreamSequenceId = 0;
	m_CurrentPlaySession.strAlterVideoGroupID = m_group_video_index;
	m_CurrentPlaySession.strAlterVideoSubStreamID = 0;
	m_CurrentPlaySession.ulAlterVideoSequenceId = m_chunk_video_index;
	m_CurrentPlaySession.ulAlterAudioSequenceId = m_chunk_audio_index;
	m_CurrentPlaySession.strAlterAudioGroupID =  m_group_audio_index;
	//m_CurrentPlaySession.strAlterAudioGroupID = m_MpdParser.get_lang_group(pAudioConcern);
	m_CurrentPlaySession.strAlterAudioSubStreamID = 0;
	if( m_MpdParser.get_qualityLevel(m_group_video_index,0)!=NULL)
	{
	m_ulDefaultBitrate =  m_MpdParser.get_qualityLevel(m_group_video_index,0)->bitrate;
	m_ulDefaultBitrate_Audio = m_MpdParser.get_qualityLevel(m_group_audio_index,0)->bitrate;
	}
	m_CurrentPlaySession.ulStreamBitrate = m_ulDefaultBitrate+m_ulDefaultBitrate_Audio;
	return 0;
}



VO_U32   C_MPD_Manager::SelectDefaultAV_Index()
{  
	int kid = 0;
	int video_count = 0;
	int group_count = m_MpdParser.GetGroupCount();
   for(int i =0 ;i< group_count; i++)
  	{
		if(m_MpdParser.m_manifest->adaptionSet[i].nTrackType==VO_SOURCE_TT_VIDEO){
		    video_count++; 
		}
	}
	for(int i =0 ;i<group_count; i++){
		if(m_MpdParser.m_manifest->adaptionSet[i].nTrackType==VO_SOURCE_TT_VIDEO&&!(kid &0x01)){
		    m_group_video_index = i;//the first video 
			kid |= 0x01;
		}
		else if( m_MpdParser.m_manifest->adaptionSet[i].nTrackType==VO_SOURCE_TT_AUDIO && !(kid & 0x02)){
			m_group_audio_index = i;//the first audio
			kid |= 0x02;
			}
	}
	 SetDefaultPlaySession();
     return 0;
}

VO_U32   C_MPD_Manager::GetTheStreamCurrentItem(E_PLAYLIST_TYPE eStreamType, S_CHUNCK_ITEM*    pChunckItem)
{

	VO_U32	  ulRet = -1;
	if(m_need_send_track_video&&eStreamType == E_X_MEDIA_VIDEO_STREAM&&!Is_Ts())
	{
		
			eStreamType = E_X_MEDIA_VIDEO_TRACK_STREAM;

		//	m_need_send_track_video = VO_FALSE;
	}
	else if(m_need_send_track_video&&eStreamType==E_X_MEDIA_AV_STREAM&&!Is_Ts())
	{
	      eStreamType = E_X_MEDIA_VIDEO_TRACK_STREAM;

	}
	else if(m_need_send_track_audio&&eStreamType == E_X_MEDIA_AUDIO_STREAM)
	{
		eStreamType = E_X_MEDIA_AUDIO_TRACK_STREAM;
		VOLOGW( "Audio_Track!")

		//m_need_send_track_audio = VO_FALSE;

	}

	switch(eStreamType)
		{
		case E_X_MEDIA_VIDEO_STREAM:
			ulRet = GetVideoPlayListItem(pChunckItem);
			if(ulRet!=0)
			{
			return ulRet;
			}
         //   VOLOGW( "GetVideoPlayListItem get_video  url: %s start_time:  %lld live_time :%lld m_video_index %d" , pChunckItem->strChunckItemURL,pChunckItem->StartTime,pChunckItem->ullBeginTime,m_chunk_video_index);
			break;
		case E_X_MEDIA_AUDIO_STREAM:
			ulRet = GetAudioPlayListItem(pChunckItem);
		//	VOLOGW( "GetAudioPlayListItem   url: %s start_time:  %lld" , pChunckItem->strChunckItemURL,pChunckItem->StartTime);
			if(ulRet!=0)
			{
			return ulRet;
			}
			break;
		case E_X_MEDIA_VIDEO_TRACK_STREAM:
			ulRet = GetVideo_TrackPlayListItem(pChunckItem);
		//	VOLOGW( "GetVideo_TrackPlayListItem   url: %s start_time:  %lld" , pChunckItem->strChunckItemURL,pChunckItem->StartTime);
			break;
		case E_X_MEDIA_AUDIO_TRACK_STREAM:
			ulRet = GetAudio_TrackPlayListItem(pChunckItem);
			VOLOGW( "GetAudio_TrackPlayListItem   url: %s start_time:  %lld" , pChunckItem->strChunckItemURL,pChunckItem->StartTime);

			break;
		case E_X_MEDIA_AV_STREAM:
			ulRet = GetAV_ChunkPlatListItem(pChunckItem);
			if(ulRet!=0)
			{
			return ulRet;
			}
			VOLOGW( "GetAV_ChunkPlatListItem   url: %s start_time:  %lld" , pChunckItem->strChunckItemURL,pChunckItem->StartTime);
			break;
		default:
			return -1;
			break;
		}
	return ulRet;
	
}
/*add for ts_chunk*/
VO_U32   C_MPD_Manager::GetAV_ChunkPlatListItem(S_CHUNCK_ITEM*    pChunckItem)
{
	VO_U32	  ulRet = VO_RET_SOURCE2_OK;
	VO_CHAR url_segment[1024];
	 memset(url_segment, 0x00, sizeof(url_segment));
	int video_group_id = m_CurrentPlaySession.strAlterVideoGroupID;
	int rpe_video_index = m_CurrentPlaySession.strAlterVideoSubStreamID;
	Representation * rpe_item = NULL;
	rpe_item= m_MpdParser.get_qualityLevel(video_group_id,rpe_video_index);
	if(rpe_item==NULL)
		return -1;
	 Seg_item * item = NULL;
	m_MpdParser.get_segmentItem(&item,m_chunk_video_index,1);
	VO_U32 index = GetMaxChunkIndex(1);
	 if(m_chunk_video_index>=index&&!Is_Live()&&index!=0)
	 {
	    item =  NULL;
	   VOLOGW(" item is null mux_chunk-index :%d, max_index,%d",m_chunk_video_index,index);
	 }

	if(item!=NULL)
	{
		 pChunckItem->StartTime = item->starttime;
	
	if(m_MpdParser.Is_Dynamic())
	{
		pChunckItem->ullBeginTime =item->live_time;
		pChunckItem->ullEndTime =  pChunckItem->ullBeginTime+2*LIVE_NUMBER;
	}
	else
	{
		pChunckItem->ullBeginTime = INAVALIBLEU64;
		pChunckItem->ullEndTime = INAVALIBLEU64;

	}
	pChunckItem->Start_number = item->startnmber;
	pChunckItem->ulDurationInMsec = item->duration;
	pChunckItem->ulChunckIndex = m_chunk_video_index;
	pChunckItem->eChunckContentType = E_CHUNK_AV;
	ulRet = pop_fragment(rpe_item,item, url_segment, m_ulDefaultBitrate);
	memcpy(pChunckItem->strChunckItemURL,url_segment,strlen(url_segment));
	VOLOGW( "GetAV_ChunkPlatListItem av_url  %s start_time %lld start_number %d" , url_segment,pChunckItem->StartTime,pChunckItem->Start_number);
	m_chunk_video_index++;
	return VO_RET_SOURCE2_OK;
	}
	else 
		return VO_RET_SOURCE2_END;


}
/*end for ts_chunk*/
VO_U32   C_MPD_Manager::GetVideoPlayListItem( S_CHUNCK_ITEM*    pChunckItem)
{
    VO_U32	  ulRet = VO_RET_SOURCE2_OK;
    VO_CHAR url_segment[1024];
    memset(url_segment, 0x00, sizeof(url_segment));
   int video_group_id = m_CurrentPlaySession.strAlterVideoGroupID;
   int rpe_video_index = m_CurrentPlaySession.strAlterVideoSubStreamID;
    Representation * ql=NULL;
   if(m_manifest.adaptionSet[0].m_ptr_QL_head!=NULL)
     ql = m_manifest.adaptionSet[0].m_ptr_QL_head;
   else
	   return -1;

   if(ql->track_count!=2&&m_manifest.group_count>6)
   {
	   video_group_id = 6;
	   rpe_video_index = 0;

   }
   Representation * rpe_item = NULL;
   rpe_item= m_MpdParser.get_qualityLevel(video_group_id,rpe_video_index); 
   
   if(rpe_item==NULL)
   return -1;
   if(rpe_item->track_count ==2)
   {
	   rpe_item= m_MpdParser.get_qualityLevel(1,rpe_video_index);
   }
   VO_U32 index = GetMaxChunkIndex(1);
   Seg_item * item = NULL;
  /* if(m_MpdParser.Is_Dynamic())
	{

    	VO_U64 Current_time= voOS_GetSysTime()-m_ulSystemtimeForUTC+m_ulUTCTime;
		VO_S64  left_time =(Current_time-m_MpdParser.GetAvailbilityTime()*1000); 
		VO_U64 duration = (m_MpdParser.m_manifest->duration_video)*1000/m_MpdParser.m_manifest->video_scale;
		m_chunk_video_index =(left_time)/(duration);
		VOLOGI( "danny_get m_chunk_audio_index  %d" , m_chunk_video_index );
	}
	*/
  // VOLOGW("GetVideoPlayListItem m_chunk_video_index :%d",m_chunk_video_index);
   m_MpdParser.get_segmentItem(&item,m_chunk_video_index,1);
   if(m_chunk_video_index>=index&&!Is_Live()&&index!=0)
   {
	   item =  NULL;
	   VOLOGW(" item is null video_chunk_index :%d, max_index,%d",m_chunk_video_index,index);

   }
   if(item!=NULL)
   {
   pChunckItem->StartTime = item->starttime;
   if(m_MpdParser.Is_Dynamic())
   {
	   pChunckItem->ullBeginTime =item->live_time;
	   pChunckItem->ullEndTime = pChunckItem->ullBeginTime+2*LIVE_NUMBER;
   }
   else
   {
	   pChunckItem->ullBeginTime = INAVALIBLEU64;
	   pChunckItem->ullEndTime = INAVALIBLEU64;
	  
   }
   pChunckItem->Start_number = item->startnmber;
   pChunckItem->ulDurationInMsec = item->duration;
   pChunckItem->ulChunckIndex = m_chunk_video_index;
   pChunckItem->eChunckContentType = E_CHUNK_VIDEO;
   ulRet = pop_fragment(rpe_item,item, url_segment, m_ulDefaultBitrate);
   memcpy(pChunckItem->strChunckItemURL,url_segment,strlen(url_segment));
  // VO_CHAR live_time[1024];
  /// VO_CHAR Dead_time[1024];
 //  timeToSting(pChunckItem->ullBeginTime,live_time);
 //  timeToSting(pChunckItem->ullEndTime,  Dead_time);
//   VOLOGW("video live_time %s dead_time %s m_chunk_video_index %d",live_time,Dead_time,m_chunk_video_index);
//   VOLOGW( "GetVideoPlayListItem video_url %s start_time %lld start_number %d" , url_segment,pChunckItem->StartTime,pChunckItem->Start_number);
    m_chunk_video_index++;
    return VO_RET_SOURCE2_OK;
   } 
   else
   {
	   VOLOGW(" end video end return end");
	 //  m_chunk_video_index++;
	   return VO_RET_SOURCE2_END;

   }

  
}


VO_U32   C_MPD_Manager::GetAudioPlayListItem(S_CHUNCK_ITEM*    pChunckItem)
{
	
	VO_U32	   ulRet = 0;
	VO_CHAR url_segment[1024];
	  memset(url_segment, 0x00, sizeof(url_segment));
    int audio_group_id = m_CurrentPlaySession.strAlterAudioGroupID;
	int rpe_audio_index = m_CurrentPlaySession.strAlterAudioSubStreamID;
	Representation * rpe_item = NULL;
	rpe_item = m_MpdParser.get_qualityLevel(audio_group_id,rpe_audio_index); 
	if(rpe_item == NULL)
	return -1;
	Seg_item * item = NULL;
	/*if(m_MpdParser.Is_Dynamic())
	{
        VO_U64 Current_time= voOS_GetSysTime()-m_ulSystemtimeForUTC+m_ulUTCTime;
		VO_S64  left_time =(Current_time-m_MpdParser.GetAvailbilityTime()*1000); 
	    VO_U64 duration = (m_MpdParser.m_manifest->duration_audio)*1000/m_MpdParser.m_manifest->audio_scale;
		m_chunk_audio_index =(left_time/(duration));
		VOLOGI( "danny_get m_chunk_audio_index  %d" , m_chunk_video_index );
	}
	*/
	VO_U64 index = GetMaxChunkIndex(0); //get_audio_index
	//VOLOGW( "GetAudioPlayListItem m_chunk_audio_index ,%d max_index  %d" , m_chunk_audio_index,index);
    m_MpdParser.get_segmentItem(&item,m_chunk_audio_index,0);
	if(m_chunk_audio_index>=index&&!Is_Live()&&index!=0)
	{
		item =  NULL;
		VOLOGW(" item is null audio_chunk_index :%d, max_index,%d",m_chunk_audio_index,index);
	}
	if(item)
	{
		pChunckItem->StartTime = item->starttime;
		if(m_MpdParser.Is_Dynamic())
		{
			 pChunckItem->ullBeginTime =item->live_time;
            //pChunckItem->ullEndTime = pChunckItem->ullBeginTime+4000;
			 pChunckItem->ullEndTime = pChunckItem->ullBeginTime+2*LIVE_NUMBER;
			//pChunckItem->ullEndTime = pChunckItem->ullBeginTime+(LIVE_NUMBER*2*item->duration*1000)/Get_Audio_Scale();
			// VOLOGW( "pChunckItem->ullEndTime  %lld" , pChunckItem->ullEndTime);

		}
		else
		{
	   pChunckItem->ullBeginTime = INAVALIBLEU64;
	   pChunckItem->ullEndTime = INAVALIBLEU64;
	
		}
	pChunckItem->Start_number = item->startnmber;
	pChunckItem->ulDurationInMsec = item->duration;
	pChunckItem->ulChunckIndex = m_chunk_audio_index;
	pChunckItem->eChunckContentType = E_CHUNK_AUDIO;
	ulRet = pop_fragment(rpe_item,item, url_segment, m_ulDefaultBitrate_Audio);
	if(url_segment)
	memcpy(pChunckItem->strChunckItemURL,url_segment,strlen(url_segment));
  // VO_CHAR live_time[1024];
  // VO_CHAR Dead_time[1024];
 //  timeToSting(pChunckItem->ullBeginTime,live_time);
 //  timeToSting(pChunckItem->ullEndTime,  Dead_time);
 //  VOLOGW("Audio live_time %s dead_time %s m_audio_chunk_index %d",live_time,Dead_time,m_chunk_audio_index);
      m_chunk_audio_index++;
 //  VOLOGW( "GetAudioPlayListItem audio_url %s start_time %lld start_number %d" , url_segment,pChunckItem->StartTime,pChunckItem->Start_number);
	return VO_RET_SOURCE2_OK;
	}
	else
	{
		VOLOGW(" end audio end return end");
		return VO_RET_SOURCE2_END;
	}

}
VO_U32   C_MPD_Manager::GetVideo_TrackPlayListItem(S_CHUNCK_ITEM*    pChunckItem)
{
	VO_U32	 ulRet = 0;
	VO_CHAR url_segment[1024];
	 memset(url_segment, 0x00, sizeof(url_segment));
	int video_group_id = m_CurrentPlaySession.strAlterVideoGroupID;
	int rpe_video_index = m_CurrentPlaySession.strAlterVideoSubStreamID;
	Representation * ql = m_manifest.adaptionSet[0].m_ptr_QL_head;
	if(ql->track_count!=2&&m_manifest.group_count>4)
	{
		video_group_id = 6;
		rpe_video_index = 0;
      
	}
	Representation * rpe_item = NULL ;
	
	rpe_item = m_MpdParser.get_qualityLevel(video_group_id,rpe_video_index);
//	if(rpe_item&&rpe_item->track_count==2)
	if(rpe_item&&rpe_item->track_count==2)
	{
		video_group_id =1;
		//add for muxed file and only muxed file
		if(m_MpdParser.Is_dash_muxed())
		{
			video_group_id =0;
		}
		rpe_item = m_MpdParser.get_qualityLevel(video_group_id,rpe_video_index);
	}
	else if(rpe_item==NULL)
	{   rpe_video_index = 0;
		rpe_item = m_MpdParser.get_qualityLevel(1,rpe_video_index);
		return -1;
		
	}
	VO_S64 birate = rpe_item->bitrate;
	pChunckItem->StartTime = 0;
	pChunckItem->eChunckContentType = E_CHUNK_TRACK;
	VO_CHAR c[10];
	 memset(c,0x00,sizeof(c));
#if defined _WIN32
		strcat(c,"%I64d");
#elif defined LINUX
		strcat(c,"%lld");
#elif defined _IOS
		strcat(c,"%lld");
#elif defined _MAC_OS
		strcat(c,"%lld");
#endif
	char str[255];
	memset(str,0x00,sizeof(str));
	sprintf(str,c,rpe_item->bitrate);
	memcpy(pChunckItem->strChunckItemURL,rpe_item->initalUrl,strlen(rpe_item->initalUrl));
    m_MpdParser.replace(pChunckItem->strChunckItemURL,"$Bandwidth$",str);
	m_MpdParser.replace(pChunckItem->strChunckItemURL,"$RepesendationID$",rpe_item->id);
	m_MpdParser.replace(pChunckItem->strChunckItemURL,"$RepresentationID$",rpe_item->id);
	m_MpdParser.replace(pChunckItem->strChunckItemURL,"mp4v","mp4");
	m_MpdParser.replace(pChunckItem->strChunckItemURL,"mp4a","mp4");
	pChunckItem->StartTime = 0;
	pChunckItem->ullBeginTime = INAVALIBLEU64;
	pChunckItem->ullEndTime = INAVALIBLEU64;
	return ulRet;
}


VO_U32   C_MPD_Manager::GetAudio_TrackPlayListItem(S_CHUNCK_ITEM*    pChunckItem)
{
        VO_U32	 ulRet = 0;
		VO_CHAR url_segment[1024];
		  memset(url_segment, 0x00, sizeof(url_segment));
		int audio_group_id = m_CurrentPlaySession.strAlterAudioGroupID;
		VOLOGW("GetAudio_TrackPlayListItem 1 %d",audio_group_id)
		int rpe_audio_index = m_CurrentPlaySession.strAlterAudioSubStreamID;
		Representation * rpe_item = m_MpdParser.get_qualityLevel(audio_group_id,rpe_audio_index);
		if(rpe_item==NULL)
			return -1;
		pChunckItem->eChunckContentType = E_CHUNK_TRACK;
		memcpy(pChunckItem->strChunckItemURL,rpe_item->initalUrl,strlen(rpe_item->initalUrl));
		VO_CHAR c[10];
		 memset(c,0x00,sizeof(c));
#if defined _WIN32
		 strcat(c,"%I64d");
#elif defined LINUX
		strcat(c,"%lld");
#elif defined _IOS
		strcat(c,"%lld");
#elif defined _MAC_OS
		strcat(c,"%lld");
#endif
		char str[255];
		memset(str,0x00,sizeof(str));
		sprintf(str,c,rpe_item->bitrate);
		m_MpdParser.replace(pChunckItem->strChunckItemURL,"$Bandwidth$",str);
		m_MpdParser.replace(pChunckItem->strChunckItemURL,"$RepesendationID$",rpe_item->id);
		m_MpdParser.replace(pChunckItem->strChunckItemURL,"$RepesendationID$",rpe_item->id);
		m_MpdParser.replace(pChunckItem->strChunckItemURL,"$RepresentationID$",rpe_item->id);
		m_MpdParser.replace(pChunckItem->strChunckItemURL,"mp4v","mp4");
	    m_MpdParser.replace(pChunckItem->strChunckItemURL,"mp4a","mp4");
		pChunckItem->StartTime = 0;
		pChunckItem->ullBeginTime = INAVALIBLEU64;
		pChunckItem->ullEndTime = INAVALIBLEU64;
		return ulRet;

}

VO_U32 C_MPD_Manager::pop_fragment(  Representation * rpe_item ,Seg_item * ptr_item, VO_CHAR * ptr_path, VO_S64 last_network_bitrate)
{
	if(!ptr_item&&!(ptr_item->group_index >= 0))
			return -1;
	if(!m_MpdParser.Is_Template())
		{
		memcpy(ptr_path,ptr_item->item_url,strlen(ptr_item->item_url));
		}
	else
		{
		VO_CHAR c[10];
		memset(c,0x00,sizeof(c));
#if defined _WIN32
		strcat(c,"%I64d");
#elif defined LINUX
		strcat(c,"%lld");
#elif defined _IOS
		strcat(c,"%lld");
#elif defined _MAC_OS
		strcat(c,"%lld");
#endif
     memcpy(ptr_item->item_url,rpe_item->mediaUrl,strlen(rpe_item->mediaUrl));
     strcpy(ptr_item->item_url,rpe_item->mediaUrl);
	 char str[255];
	 memset(str,0x00,sizeof(str));
	 sprintf(str,c,rpe_item->bitrate);
	 m_MpdParser.replace(ptr_item->item_url,"$Bandwidth$",str);
	 memset(str,0x00,sizeof(str));
	 sprintf(str,c, ptr_item->starttime);
	 m_MpdParser.replace(ptr_item->item_url,"$Time$",str);
	 m_MpdParser.replace(ptr_item->item_url,"$RepesendationID$",rpe_item->id);
	 m_MpdParser.replace(ptr_item->item_url,"$RepresentationID$",rpe_item->id);
	 memset(str,0x00,sizeof(str));
	// sprintf(str,c,ptr_item->startnmber);
	 if(strstr(ptr_item->item_url,"%0"))
	    sprintf(str,"%05d",ptr_item->startnmber);
	 else 
		 sprintf(str,c,ptr_item->startnmber);
	 m_MpdParser.replace(ptr_item->item_url,"$Number$",str);
	 m_MpdParser.replace(ptr_item->item_url,"$Number%05$",str);
	 memset(str,0x00,sizeof(str));
	 sprintf(str,c,ptr_item->startnmber);
	 m_MpdParser.replace(ptr_item->item_url,"$Index$",str);
	 memcpy(ptr_path,ptr_item->item_url,strlen(ptr_item->item_url));
	}
	return 1 ;

}

VO_VOID C_MPD_Manager::replace(char *source,const char *sub,const char *rep )
{

	int in,out;
	in = out = 0;
	int lsub = strlen(sub);
	int lsour = strlen(source);
	char tmp[155];
	char *p = source;
	char sourceclone[1024];
	memset(sourceclone,0x00,1024);
	memcpy(sourceclone,source,lsour);
	while(lsour >= lsub)
	{
		memset(tmp,0x00,155);
		memcpy(tmp,p,lsub);
		if(strcmp(tmp,sub)==0)
		{
			out = in +lsub;
			break;
		}
		in ++;
		p++;
		lsour -- ;
	}
	if(out >0 && in != out)
	{
		memset(source, 0 ,strlen(source));
		memcpy(source, sourceclone,in);
		strcat(source,rep);
		strcat(source, &sourceclone[out]);
	}
}

VO_U32   C_MPD_Manager::SetThePos(VO_U32   ulTime)
{
    VO_S64 start_time = 0;
	VO_U32 index = 0;
    if(ulTime == 0) 
	{
		m_chunk_video_index = index;
	    m_chunk_audio_index = index;
		return 0;
	}
	start_time = m_MpdParser.find_fragment_starttime(ulTime, m_CurrentPlaySession.ulAlterVideoSequenceId ==-1?0 : 1) ;//0 audio, 1 video
	index = m_MpdParser.find_fragment_starttime_index(start_time, m_CurrentPlaySession.ulAlterVideoSequenceId ==-1?0 : 1 );
	m_chunk_video_index = index;
	m_chunk_audio_index = index;
    VOLOGW("manager start_time :%lld",start_time);
	VOLOGW("manager m_chunk_video_index :%d",m_chunk_video_index);
	return start_time;
}
VO_U32   C_MPD_Manager::GetTheDuration(VO_U32*  pTimeOffset)
{
	*pTimeOffset = m_MpdParser.GetDuration();
	return 0;
}

VO_BOOL  C_MPD_Manager::IsPlaySessionReady()
{
    return VO_TRUE;
}    

VO_VOID   C_MPD_Manager::SetUTCTime(VO_U64  pUTCTime,VO_U64 pSystem_time)
{
    VOLOGI("the UTC Time:%ld", pUTCTime);;
    m_ulSystemtimeForUTC = pSystem_time;
	m_ulUTCTime = pUTCTime;
   // VOLOGI("the System for UTC:%ld", m_ulSystemtimeForUTC);
}

