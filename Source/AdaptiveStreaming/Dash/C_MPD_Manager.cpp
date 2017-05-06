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
	m_need_send_track_subtitle = VO_TRUE;
	m_pure_audio_current_time = 0;
	m_ulDefaultBitrate = 0;
	memset(&m_CurrentPlaySession, 0x00, sizeof(S_PLAY_SESSION));
	m_group_video_index = 0;
	m_group_audio_index = 0;
	m_group_subtile_index = 0;
	m_chunk_video_index = 0;
	m_chunk_audio_index = 0;
	m_chunk_subtitle_index = 0;
	memset(&m_manifest, 0x00, sizeof(MpdStreamingMedia));
	m_manifest.adaptionSet = NULL; 
	m_manifest.m_period = NULL; 
	m_manifest.group_count = 0;
	m_video_stream_index =  VO_TRUE;
	m_audio_stream_index = VO_TRUE;
	m_is_dash_if = VO_FALSE;
	current_video_rpe = NULL;
	current_audio_rpe = NULL;
	current_subtile_rpe= NULL;
}

C_MPD_Manager::~C_MPD_Manager()
{
	ReleaseAll();
	memset(&m_CurrentPlaySession, 0x00, sizeof(S_PLAY_SESSION));
	memset(&m_manifest, 0x00, sizeof(MpdStreamingMedia));
}
VO_U32 C_MPD_Manager::InitParser()
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	nResult = m_MpdParser.InitParser();
	return nResult;
}

VO_U32 C_MPD_Manager::UninitParser()
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	nResult =m_MpdParser.UninitParser();

	return nResult;
}



VO_VOID C_MPD_Manager::ReleaseAll()
{
	//VOLOGE("C_MPD_Manager release 1");
	if(!m_manifest.adaptionSet)
		return;
	if(!m_manifest.m_period)
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

	if(m_manifest.adaptionSet)
	{
		delete []m_manifest.adaptionSet;
		m_manifest.adaptionSet = NULL;
		m_manifest.group_count = 0;
	}
	if(m_manifest.m_period){
		delete []m_manifest.m_period;
		m_manifest.m_period = NULL; 
		m_manifest.period_count = 0;
	}
	m_ulDefaultBitrate = 0;
	m_chunk_video_index = 0;
	m_chunk_audio_index = 0;
	VOLOGE("C_MPD_Manager release OK");

}

VO_BOOL C_MPD_Manager::ParseMpd(VO_BYTE* pMpdData, VO_U32 ulMpdDataLength, VO_CHAR* pMpdPath)
{
	ReleaseAll();
	VO_BOOL   ulRet = VO_FALSE; 
	if(pMpdData == NULL || pMpdPath == NULL)
	{
		return VO_FALSE;
	}
	if(pMpdPath!=NULL)
		SetSessionUrl(pMpdPath);
	if(pMpdData!=NULL&&pMpdPath)
		ulRet = m_MpdParser.Parse(pMpdData, ulMpdDataLength,&m_manifest,pMpdPath);
	if(!m_manifest.adaptionSet)
	{
		VOLOGE("ParseMpd fail");
		return VO_FALSE;
	}
	return ulRet;
}

VO_VOID C_MPD_Manager::Init_Period(VO_U32 period_index)
{

	m_MpdParser.Init_MPD(period_index);

}
VO_U32   C_MPD_Manager::SetSessionUrl(VO_CHAR* pMpdPath)
{
	memcpy(m_CurrentPlaySession.strStreamURL,pMpdPath,strlen(pMpdPath));
	return 0;

}


VO_U32  C_MPD_Manager::GetMainStreamCount(VO_U32*  pMainStreamCount,VO_U32 period_index)
{
	*pMainStreamCount = m_MpdParser.GetStreamCount(period_index);
	return 0;
}

VO_U32  C_MPD_Manager::GetMainVideoCount(VO_U32*  pMainStreamCount)
{
	*pMainStreamCount = m_MpdParser.GetVideoCount();
	return *pMainStreamCount;
}
VO_U32  C_MPD_Manager::GetMainAudioCount(VO_U32*  pMainStreamCount)
{
	*pMainStreamCount = m_MpdParser.GetAudioCount();
	return *pMainStreamCount;
}
VO_U32  C_MPD_Manager::GetMainAudioBitrateCount(VO_U32*  pMainStreamCount)
{
	*pMainStreamCount = m_MpdParser.GetAudioBitrateCount();
	return *pMainStreamCount;
}
VO_U32  C_MPD_Manager::GetMainSubtitleCount(VO_U32*  pMainStreamCount)
{
	*pMainStreamCount = m_MpdParser.GetSubtitleCount();
	return *pMainStreamCount;
}
VO_VOID   C_MPD_Manager::GetDuration(VO_U32*  pTimeDuration)
{
	VO_U32   ulRet = 0;
	VO_U32   ulIndex = 0;
	VO_U32   ulDuration = 0;
	*pTimeDuration =  m_MpdParser.GetDuration();
}
VO_VOID   C_MPD_Manager::GetPeriodDuration(VO_U32*  period_duration,VO_U32 period_index)
{
	VO_U32   ulRet = 0;
	VO_U32   ulIndex = 0;
	VO_U32   ulDuration = 0;
	if( m_MpdParser.m_manifest&&m_MpdParser.m_manifest->m_period)
	*period_duration =  m_MpdParser.m_manifest->m_period[period_index].period_fact_duration;
}
VO_U32   C_MPD_Manager::GetChunkDuration()
{
	VO_U32    ul_time= 0;
	VO_U32    period_index = 0;
	VO_U32    video_really_duration = 0;
	VO_U32    audio_really_duration = 0;
	if( m_MpdParser.m_manifest&&m_MpdParser.m_manifest->m_period)
	{
	if(Get_Video_Scale(period_index)!=0&&Get_duration_video()!=0)
	{
		GetPeriodDuration(&ul_time,period_index);	
		video_really_duration = Get_duration_video()*1000/Get_Video_Scale(period_index);
	}
	if( Get_duration_audio()!=0&&Get_Audio_Scale(period_index)!=0)
	{
		GetPeriodDuration(&ul_time,period_index);	
		audio_really_duration = Get_duration_audio()*1000/Get_Audio_Scale(period_index);
	}
	}
	if(video_really_duration!=0)
		return video_really_duration;
	else if(audio_really_duration!=0)
		return audio_really_duration;
	else 
		return 2000;
}
VO_U32   C_MPD_Manager::GetMaxChunkIndex(VO_U32 type)
{
	VO_U32    ul_time= 0;
	VO_U32    period_index = m_CurrentPlaySession.strPeriodIndex;
	VO_U32    video_really_duration = 0;
	VO_U32    audio_really_duration = 0;
	VO_U32    left_video_index = 0;
	VO_U32    left_audio_index = 0;
	VO_U32    max_video_index = 0;
	VO_U32    max_audio_index = 0;
	VO_U32    max_index = 0;
	VO_S32    check_index =0;
	VO_S64    m_duration_video =0;
	VO_S64    m_duration_audio = 0;
	if(Get_Video_Scale(period_index)!=0&&Get_duration_video()!=0)
	{
		GetPeriodDuration(&ul_time,period_index);	
		video_really_duration = Get_duration_video()*1000/Get_Video_Scale(period_index);
		max_video_index = ul_time/video_really_duration;
		left_video_index = ul_time%video_really_duration;
	}
	if( Get_duration_audio()!=0&&Get_Audio_Scale(period_index)!=0)
	{
		GetPeriodDuration(&ul_time,period_index);	
		audio_really_duration = Get_duration_audio()*1000/Get_Audio_Scale(period_index);
		max_audio_index = ul_time/audio_really_duration;
		left_audio_index = ul_time%audio_really_duration;
	}
	check_index = max_video_index-max_audio_index;
	if(check_index<0)
		check_index = max_audio_index-max_video_index;

	if(check_index>4&&check_index<5||check_index>1000)
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
			    VOLOGW( "AUDIO  max_index  %d",max_index);
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

VO_VOID  C_MPD_Manager::SelectCurrentPlaySession(VO_U32 video_rpe_index, VO_U32 track_id,VO_U32 subtitle_track_id)
{
	//m_CurrentPlaySession.ulStreamBitrate = ulBitrate;
	VO_U64 video_birate = 0;
	if(VO_FALSE == m_CurrentPlaySession.is_pure_audio)
	{
		Representation * rpe = m_MpdParser.get_qualityLevel(m_group_video_index,video_rpe_index);
		if(rpe!=NULL)
			video_birate = m_MpdParser.get_qualityLevel(m_group_video_index,video_rpe_index)->bitrate;
		else
		//	video_birate = m_MpdParser.get_qualityLevel(video_rpe_index,0)->bitrate;
			m_CurrentPlaySession.strAlterVideoSubStreamID  = video_rpe_index;
	}
	if(m_MpdParser.get_lang_track(track_id)!=-1)
		m_CurrentPlaySession.strAlterAudioGroupID = m_MpdParser.get_lang_track(track_id);
	m_CurrentPlaySession.strAlterAudioSubStreamID = m_MpdParser.get_rpe_index(m_CurrentPlaySession.strAlterAudioGroupID,track_id);
	if(m_MpdParser.get_lang_track(subtitle_track_id)!=-1)
		m_CurrentPlaySession.strAlterSubtileGroupID = m_MpdParser.get_lang_track(subtitle_track_id);
	VO_U32 group_audio_id = m_CurrentPlaySession.strAlterAudioGroupID;
	m_CurrentPlaySession.ulStreamBitrate  = (m_CurrentPlaySession.is_pure_audio ? m_MpdParser.get_qualityLevel(group_audio_id,0)->bitrate: video_birate);

}
VO_VOID    C_MPD_Manager::AddInitData(VODS_INITDATA_INFO * init_data_info,VO_U32 track_type)
{
	Representation * rpe_item = NULL;
	VO_U32 audio_group_id = 0;
	VO_U32 video_group_id = 0; 
	VO_U32 subtitle_group_id = 0;
	VO_U64 head_offset = init_data_info->sHeader.uOffset;
	VO_U64 head_size = init_data_info->sHeader.uSize;
	VO_U64 end_offset = head_offset+head_size-1;
	//VO_U64 sigx_box_offset = init_data_info->sSegIndex.uOffset;
	//VO_U64 sigx_box_size = init_data_info->sSegIndex.uSize
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
	VO_CHAR  ini_range[256];
	memset(ini_range,0x00,sizeof(ini_range));
	sprintf(ini_range,c,head_offset);
	strcat(ini_range,"-");
	VO_CHAR  end_range[256];
	memset(end_range,0x00,sizeof(end_range));
	sprintf(end_range,c,end_offset);
	strcat(ini_range,end_range);
	 m_is_dash_if = VO_TRUE;
	if(track_type == VO_SOURCE2_TT_AUDIO)
	{
	audio_group_id = m_CurrentPlaySession.strAlterAudioGroupID;
	VO_U32 rpe_audio_index = m_CurrentPlaySession.strAlterAudioSubStreamID;
	rpe_item = m_MpdParser.get_qualityLevel(audio_group_id,rpe_audio_index);
	if(rpe_item==NULL)
		return;
	m_MpdParser.Set_rpe_index(rpe_audio_index,0);
	m_MpdParser.Set_IndexRange(VO_TRUE,0);
	}
	else if(track_type == VO_SOURCE2_TT_VIDEO||VO_SOURCE2_TT_MUXGROUP == track_type)
	{
		video_group_id = m_CurrentPlaySession.strAlterVideoGroupID;
		VO_U32 rpe_video_index = m_CurrentPlaySession.strAlterVideoSubStreamID;
		rpe_item = NULL ;
		rpe_item = m_MpdParser.get_qualityLevel(video_group_id,rpe_video_index);
		if(rpe_item==NULL)
			return;
		m_MpdParser.Set_rpe_index(rpe_video_index,1);
		m_MpdParser.Set_IndexRange(VO_TRUE,1);
	}
	else if(VO_SOURCE2_TT_SUBTITLE == track_type )
	{
		subtitle_group_id = m_CurrentPlaySession.strAlterSubtileGroupID;
		VO_U32 rpe_subtile_index = 0;
		rpe_item = NULL;
		rpe_item = m_MpdParser.get_qualityLevel(subtitle_group_id,rpe_subtile_index);
		if(NULL ==rpe_item)
			return;
		m_MpdParser.m_is_index_range_subtitle = VO_TRUE;
	}
	//sigx_box for rpe_offset
	/*int type = (track_type == VO_SOURCE2_TT_VIDEO?1:0);
	m_MpdParser.Set_IndexRange(VO_TRUE,type);*/
	strcpy(rpe_item->ini_range,ini_range);
	strcpy(rpe_item->index_range,ini_range);
	/*rpe_item->chunk_range_offset = sigx_box_offset+sigx_box_size-INDEX_RANGE__END_OFFSET;*/

}
VO_VOID    C_MPD_Manager::AddByteRangeInfo(VODS_SEG_INDEX_INFO * index_Info_List,VO_U32 track_type)
{
	Representation * rpe_item = NULL;
	VO_U32 audio_group_id = 0;
	VO_U32 video_group_id = 0; 
	VO_U32 subtitle_group_id = 0;
	//VO_U32 period_id = m_CurrentPlaySession.strPeriodIndex;
	if(track_type == VO_SOURCE2_TT_AUDIO)
	{
	VOLOGI("AddByteRangeInfo_Audio  %d", index_Info_List->uCount);
	audio_group_id = m_CurrentPlaySession.strAlterAudioGroupID;
	VO_U32 rpe_audio_index = m_CurrentPlaySession.strAlterAudioSubStreamID;
	rpe_item = m_MpdParser.get_qualityLevel(audio_group_id,rpe_audio_index);
	if(rpe_item==NULL)
		return;
	/*m_MpdParser.m_manifest->m_period[period_id].is_index_range_period_audio = VO_FALSE;*/
	m_MpdParser.Set_rpe_index(rpe_audio_index,0);
	}
	else if(track_type == VO_SOURCE2_TT_VIDEO||VO_SOURCE2_TT_MUXGROUP == track_type)
	{
	    VOLOGI("AddByteRangeInfo_Video_Count %d", index_Info_List->uCount);
		video_group_id = m_CurrentPlaySession.strAlterVideoGroupID;
		VO_U32 rpe_video_index = m_CurrentPlaySession.strAlterVideoSubStreamID;
		rpe_item = NULL ;
		rpe_item = m_MpdParser.get_qualityLevel(video_group_id,rpe_video_index);
		m_MpdParser.Set_rpe_index(rpe_video_index,1);
		VOLOGI("AddByteRangeInfo_Video_Bitrate  %lld", rpe_item->bitrate);
		//m_MpdParser.m_manifest->m_period[period_id].is_index_range_period_video = VO_FALSE;
	}
	else if(VO_SOURCE2_TT_SUBTITLE == track_type )
	{
		subtitle_group_id = m_CurrentPlaySession.strAlterSubtileGroupID;
		VO_U32 rpe_subtitle_index = 0;
		rpe_item = NULL ;
		rpe_item = m_MpdParser.get_qualityLevel(subtitle_group_id,rpe_subtitle_index);
		/*m_MpdParser.m_manifest->m_period[period_id].is_index_range_period_subtitle = VO_FALSE;*/
	}
	rpe_item->is_init_data = VO_FALSE;
	VO_U32 count = index_Info_List->uCount;
	Seg_item * temp_item = NULL;
	int type = (track_type == VO_SOURCE2_TT_VIDEO?1:0);
	VO_U64  current_count   = rpe_item->byterange_chunk_count;
	//if(type !=0)
	//m_MpdParser.get_segmentItem(&temp_item,rpe_item,current_count);
	//else
	//m_MpdParser.get_segmentItem(&temp_item,rpe_item,current_count);
	//VO_U64   current_offset_time = temp_item->starttime;
    VO_U64   current_offset_time = rpe_item->current_sigx_offset_time;
	VO_U64   current_index_offset = 0;
	if(m_is_dash_if)
		current_index_offset = 0;
	else
		current_index_offset  = rpe_item->chunk_range_offset;
	VOLOGI("AddByteRangeInfo index_offset  %lld", current_index_offset);
	for(VO_U32 kk = 0;kk<count;kk++)
	{

		Seg_item * item = new Seg_item; 
		item->first_offset = current_index_offset+index_Info_List->pSegInfo[kk].uOffset;
		item->size =  index_Info_List->pSegInfo[kk].uSize;
		/*item->duration = index_Info_List->pSegInfo[kk].uDuration;*/
		//current_index_offset+=item->size;
		item->duration = (index_Info_List->pSegInfo[kk].uDuration*1000)/index_Info_List->pSegInfo[kk].uTimeScale;
		index_Info_List->pSegInfo[kk].uStartTime = current_offset_time;
		current_offset_time += index_Info_List->pSegInfo[kk].uDuration;
	/*	item->starttime = index_Info_List->pSegInfo[kk].uStartTime;*/
		item->starttime = (index_Info_List->pSegInfo[kk].uStartTime*1000/index_Info_List->pSegInfo[kk].uTimeScale);
		item->nIsVideo = (track_type ==VO_SOURCE2_TT_AUDIO?0:1);
		item->group_index = (item->nIsVideo?video_group_id:audio_group_id);
		VO_U32 period_index = m_CurrentPlaySession.strPeriodIndex;
		item->period_index = period_index;
		if(VO_SOURCE2_TT_MUXGROUP == track_type)
		{
			m_manifest.duration_video = item->duration;
			m_manifest.duration_audio = item->duration;
			m_MpdParser.m_manifest->m_period[period_index].period_vtimescale =index_Info_List->pSegInfo[kk].uTimeScale;
			m_MpdParser.m_manifest->m_period[period_index].period_atimescale =index_Info_List->pSegInfo[kk].uTimeScale;
		}
		else if(VO_SOURCE2_TT_VIDEO == track_type)
		{
			if(kk==0)
			{
				m_manifest.duration_video = item->duration;
				VOLOGI(" AddByteRangeInfo duration_video  %lld",m_manifest.duration_video);

			}
			m_MpdParser.m_manifest->m_period[period_index].period_vtimescale =index_Info_List->pSegInfo[kk].uTimeScale;
		}
		else if(VO_SOURCE2_TT_AUDIO == track_type )
		{
			m_MpdParser.m_manifest->m_period[period_index].period_atimescale =index_Info_List->pSegInfo[kk].uTimeScale;
			if(kk==0)
			{
				m_manifest.duration_audio = item->duration;
				VOLOGI(" AddByteRangeInfo duration_audio  %lld",m_manifest.duration_audio);
			}
		}
		else if(VO_SOURCE2_TT_SUBTITLE ==track_type )
		{
			m_MpdParser.m_manifest->m_period[period_index].period_subtitle_timescale =index_Info_List->pSegInfo[kk].uTimeScale;
			m_MpdParser.m_is_index_range_subtitle = VO_TRUE;
			if(kk==0)
			m_MpdParser.m_manifest->duration_subtitle = item->duration;
		}
		m_MpdParser.m_manifest->m_period[period_index].period_vtimescale  = 1000;
		m_MpdParser.m_manifest->m_period[period_index].period_atimescale  = 1000;
		m_MpdParser.m_manifest->m_period[period_index].period_subtitle_timescale = 1000;
		item->nOrignalStartNumber = 0;
		if( index_Info_List->pSegInfo[kk].uType == VODS_SUBSEGMENT_TYPE_INDEX)
		{
			item->is_sigx_index_chunk = VO_TRUE;
			item->is_index_parsed =  VO_FALSE;
		}
		else
		{
			item->is_sigx_index_chunk = VO_FALSE;
			item->is_index_parsed =  VO_FALSE;
		}
		 if(VO_SOURCE2_TT_SUBTITLE ==track_type )
			 m_MpdParser.add_segmentItem_subtile(rpe_item,item);
		 else
		m_MpdParser.add_segmentItem(rpe_item,item);
	}

	
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


VO_U32   C_MPD_Manager::SetDefaultPlaySession(VO_U32 period_index)
{
	m_CurrentPlaySession.ulMainStreamSequenceId = 0;
	m_CurrentPlaySession.strAlterVideoGroupID = m_group_video_index;
	m_CurrentPlaySession.strAlterVideoSubStreamID = 0;
	m_CurrentPlaySession.ulAlterVideoSequenceId = m_chunk_video_index;
	m_CurrentPlaySession.ulAlterAudioSequenceId = m_chunk_audio_index;
	m_CurrentPlaySession.strAlterAudioGroupID =  m_group_audio_index;
	m_CurrentPlaySession.strAlterSubtileGroupID = m_group_subtile_index;
	m_CurrentPlaySession.strAlterAudioLanguage = m_manifest.adaptionSet[m_group_audio_index].lang;
	m_CurrentPlaySession.strAlterAudioSubStreamID = 0;
	if( m_MpdParser.get_qualityLevel(m_group_video_index,0)!=NULL)
	{
		m_ulDefaultBitrate =  m_MpdParser.get_qualityLevel(m_group_video_index,0)->bitrate;
		Representation * audio_rpe =  m_MpdParser.get_qualityLevel(m_group_audio_index,0);
		if(audio_rpe)
		m_ulDefaultBitrate_Audio = audio_rpe->bitrate;
	}
	if(m_group_video_index!=m_group_audio_index)
		m_CurrentPlaySession.ulStreamBitrate = m_ulDefaultBitrate+m_ulDefaultBitrate_Audio;
	else
		m_CurrentPlaySession.ulStreamBitrate = m_ulDefaultBitrate;
	m_CurrentPlaySession.strPeriodIndex= period_index;
	m_CurrentPlaySession.is_pure_audio = VO_FALSE;
	return 0;
}



VO_VOID   C_MPD_Manager::SelectDefaultAV_Index(VO_U32 current_period_index)
{  
	int kid = 0;
	int video_count = 0;
	int group_count = m_MpdParser.GetGroupCount();
	if(NULL==m_MpdParser.m_manifest->adaptionSet)
		return ;
	for(int i =0 ;i< group_count; i++)
	{
		if(m_MpdParser.m_manifest->adaptionSet[i].nTrackType==VO_SOURCE_TT_VIDEO){
			video_count++; 
		}
	}
	for(int i =0 ;i<group_count; i++){
		if(m_MpdParser.m_manifest->adaptionSet[i].nTrackType==VO_SOURCE_TT_VIDEO&&!(kid &0x01)&&m_MpdParser.m_manifest->adaptionSet[i].period_index==current_period_index){
			m_group_video_index = i;//the first video 
			kid |= 0x01;
		}
		else if( m_MpdParser.m_manifest->adaptionSet[i].nTrackType==VO_SOURCE_TT_AUDIO && !(kid & 0x02)&&m_MpdParser.m_manifest->adaptionSet[i].period_index==current_period_index){
			m_group_audio_index = i;//the first audio
			kid |= 0x02;
		}
		else if(m_MpdParser.m_manifest->adaptionSet[i].nTrackType==VO_SOURCE_TT_SUBTITLE && !(kid & 0x04)&&m_MpdParser.m_manifest->adaptionSet[i].period_index==current_period_index)
		{
			m_group_subtile_index = i;
			kid |=0x04;
		}
	}

	SetDefaultPlaySession(current_period_index);
//	return 0;
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
	}
	else if(m_need_send_track_subtitle&&eStreamType == E_X_MEDIA_SUBTITLE_STREAM)
	{
		eStreamType = E_X_MEDIA_SUBTITLE_TRACK_STREAM;
	}
	if(eStreamType == E_X_MEDIA_VIDEO_STREAM &&m_MpdParser.Is_IndexRange_video())
	{
		eStreamType = E_X_MEDIA_VIDEO_Index_STREAM;
	}
	else if (eStreamType == E_X_MEDIA_AUDIO_STREAM&&m_MpdParser.Is_IndexRange_audio())
	{
		eStreamType = E_X_MEDIA_AUDIO_Index_STREAM;
	}
	else if (eStreamType == E_X_MEDIA_AV_STREAM&&m_MpdParser.Is_IndexRange_video())
	{
		eStreamType = E_X_MEDIA_AV_Index_STREAM;
	}
	else if(eStreamType == E_X_MEDIA_SUBTITLE_STREAM &&m_MpdParser.Is_IndexRange_subtile())
	{
		eStreamType = E_X_MEDIA_SUBTILE_Index_STREAM;
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
		if(ulRet!=0)
		{
			return ulRet;
		}
		else
		VOLOGW( "GetVideo_TrackPlayListItem   url: %s start_time:  %lld" , pChunckItem->strChunckItemURL,pChunckItem->StartTime);
		break;
	case E_X_MEDIA_AUDIO_TRACK_STREAM:
		ulRet = GetAudio_TrackPlayListItem(pChunckItem);
		if(ulRet!=0)
		{
			return ulRet;
		}
		else
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
	case E_X_MEDIA_SUBTITLE_STREAM:
		ulRet = GetSubtitle_ChunkPlatListItem(pChunckItem);
		if(ulRet!=0)
		{
			return ulRet;
		}
		VOLOGW( "GetSubtitle_ChunkPlatListItem   url: %s start_time:  %lld" , pChunckItem->strChunckItemURL,pChunckItem->StartTime);
		break;
	case E_X_MEDIA_SUBTITLE_TRACK_STREAM:
		ulRet = GetSubtitle_TrackChunkPlatListItem(pChunckItem);
		if(ulRet!=0)
		{
			return ulRet;
		}	
		break;
	case E_X_MEDIA_SUBTILE_Index_STREAM:
		ulRet = GetSubtitle_IndexChunkPlatListItem(pChunckItem);
		if(ulRet!=0)
		{
			return ulRet;
		}	
		break;
	case E_X_MEDIA_VIDEO_Index_STREAM:
		ulRet = GetVideo_IndexStream(pChunckItem);
		if(ulRet!=0)
		{
			return ulRet;
		}
		break;
	case E_X_MEDIA_AUDIO_Index_STREAM:
		ulRet = GetAudio_IndexStream(pChunckItem);
		if(ulRet!=0)
		{
			return ulRet;
		}
		break;
	case E_X_MEDIA_AV_Index_STREAM:
		ulRet = GetAV_IndexStream(pChunckItem);
		if(ulRet!=0)
		{
			return ulRet;
		}
		break;
	default:
		return -1;
		break;
	}
	return ulRet;

}
VO_U32   C_MPD_Manager::GetAV_IndexStream(S_CHUNCK_ITEM*    pChunckItem)
{
	VO_U32	  ulRet = VO_RET_SOURCE2_OK;
	VO_CHAR url_segment[1024];
	memset(url_segment, 0x00, sizeof(url_segment));
	VO_U32 video_group_id = m_CurrentPlaySession.strAlterVideoGroupID;
	VO_U32 rpe_video_index = m_CurrentPlaySession.strAlterVideoSubStreamID;
	VO_U32 period_index = m_CurrentPlaySession.strPeriodIndex;
	Representation * rpe_item = NULL;
	rpe_item= m_MpdParser.get_qualityLevel(video_group_id,rpe_video_index);
	if(rpe_item==NULL)
		return -1;
	Seg_item * item = NULL;
	m_MpdParser.get_segmentItem(&item,rpe_item,m_chunk_video_index);
	Seg_item * check_item = NULL;
	VO_BOOL   check_flag =  VO_TRUE;
	m_MpdParser.get_segmentItem(&check_item,rpe_item,0);
	if(check_item->is_sigx_index_chunk&&!check_item->is_index_parsed)
	{
		check_flag = VO_FALSE;
		item = check_item;
	}
	if(check_flag &&(item == NULL||m_chunk_video_index>=rpe_item->byterange_chunk_count))
	{
		return VO_RET_SOURCE2_END;
	}
	else
	{
		strcpy(pChunckItem->strChunckItemURL,rpe_item->initalUrl);
		m_MpdParser.replace(pChunckItem->strChunckItemURL,"&amp;","&");
		pChunckItem->eChunckContentType = E_CHUNK_AV;
		pChunckItem->is_SidBox = item->is_sigx_index_chunk;
		if(pChunckItem->is_SidBox)
		{
		
			if(rpe_item->m_index_range_exact)
			rpe_item->chunk_range_offset = item->first_offset+item->size-INDEX_RANGE__END_OFFSET;
			else
			rpe_item->chunk_range_offset = item->first_offset+item->size;
			rpe_item->current_sigx_offset_time = item->starttime;
			if(!m_video_stream_index&&item->is_index_parsed == VO_FALSE&&rpe_item->current_sigx_offset_time==0)
			{
				m_chunk_video_index-=1;
				m_video_stream_index =  VO_FALSE;
			}
			m_video_stream_index =  VO_FALSE;
		}
		item->is_index_parsed  =  VO_TRUE;
		pChunckItem->ullBeginTime = INAVALIBLEU64;
		pChunckItem->ullEndTime = INAVALIBLEU64;
		pChunckItem->Start_number = item->startnmber;
		pChunckItem->ulDurationInMsec = item->duration;
		pChunckItem->ulChunckIndex = m_chunk_video_index;
		pChunckItem->ulChunckOffset = item->first_offset;
		pChunckItem->ulChunckLen = item->size;
		pChunckItem->StartTime = item->starttime;
		pChunckItem->range_flag = VO_TRUE;
		VOLOGW(" range_item is  GetAV_IndexStream :%s",pChunckItem->strChunckItemURL);
		VOLOGW(" range_item is  GetAV_IndexStream len :%d, offset,%d",pChunckItem->ulChunckLen ,pChunckItem->ulChunckOffset); 
		m_chunk_video_index++;
		return VO_RET_SOURCE2_OK;
	}
}
VO_U32   C_MPD_Manager::GetVideo_IndexStream(S_CHUNCK_ITEM*    pChunckItem)
{
	VO_U32	  ulRet = VO_RET_SOURCE2_OK;
	VO_CHAR url_segment[1024];
	memset(url_segment, 0x00, sizeof(url_segment));
	VO_U32 video_group_id = m_CurrentPlaySession.strAlterVideoGroupID;
	VO_U32 rpe_video_index = m_CurrentPlaySession.strAlterVideoSubStreamID;
	VO_U32 period_index = m_CurrentPlaySession.strPeriodIndex;
	Representation * rpe_item = NULL;
	rpe_item= m_MpdParser.get_qualityLevel(video_group_id,rpe_video_index);
	if(rpe_item==NULL)
		return -1;
	Seg_item * item = NULL;
	m_MpdParser.get_segmentItem(&item,rpe_item,m_chunk_video_index);
	Seg_item * check_item = NULL;
	VO_BOOL   check_flag =  VO_TRUE;
	m_MpdParser.get_segmentItem(&check_item,rpe_item,0);
	if(check_item&&check_item->is_sigx_index_chunk&&!check_item->is_index_parsed)
	{
		check_flag = VO_FALSE;
		item = check_item;
	}
	if(check_flag&&(item == NULL||m_chunk_video_index>=rpe_item->byterange_chunk_count))
	{
		return VO_RET_SOURCE2_END;
	}
	else
	{
		strcpy(pChunckItem->strChunckItemURL,rpe_item->initalUrl);
		m_MpdParser.replace(pChunckItem->strChunckItemURL,"&amp;","&");

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
		pChunckItem->eChunckContentType = E_CHUNK_VIDEO;
		pChunckItem->is_SidBox = item->is_sigx_index_chunk;
		if(pChunckItem->is_SidBox)
		{
			if(rpe_item->m_index_range_exact)
			rpe_item->chunk_range_offset = item->first_offset+item->size-INDEX_RANGE__END_OFFSET;
			else
			rpe_item->chunk_range_offset = item->first_offset+item->size;
			rpe_item->current_sigx_offset_time = item->starttime;
			if(!m_video_stream_index&&item->is_index_parsed == VO_FALSE&&rpe_item->current_sigx_offset_time==0)
			{
				m_chunk_video_index-=1;
				m_video_stream_index =  VO_FALSE;
			}
			m_video_stream_index =  VO_FALSE;
		}
		item->is_index_parsed  =  VO_TRUE;
		pChunckItem->ullBeginTime = INAVALIBLEU64;
		pChunckItem->ullEndTime = INAVALIBLEU64;
		pChunckItem->Start_number = item->startnmber;
		pChunckItem->ulDurationInMsec = item->duration;
		pChunckItem->ulChunckIndex = m_chunk_video_index;
		pChunckItem->ulChunckOffset = item->first_offset;
		pChunckItem->ulChunckLen = item->size;
		pChunckItem->StartTime = item->starttime;
		pChunckItem->range_flag = VO_TRUE;
		VOLOGW(" range_item is  video_index :%d",m_chunk_video_index);
		VOLOGW(" range_item is  video_count %d",rpe_item->byterange_chunk_count);
		VOLOGW(" range_item is  video_StartTime %lld",pChunckItem->StartTime);
		VOLOGW(" range_item is  GetVideo_IndexStream :%s",pChunckItem->strChunckItemURL);
		VOLOGW(" range_item is  GetVideo_IndexStream len :%d, offset,%d",pChunckItem->ulChunckLen ,pChunckItem->ulChunckOffset); 
		m_chunk_video_index++;
		return VO_RET_SOURCE2_OK;
	}
	
}
VO_U32   C_MPD_Manager::GetAudio_IndexStream(S_CHUNCK_ITEM*    pChunckItem)
{
	VO_U32	   ulRet = 0;
	VO_CHAR url_segment[1024];
	memset(url_segment, 0x00, sizeof(url_segment));
	VO_U32 audio_group_id = m_CurrentPlaySession.strAlterAudioGroupID;
	VO_U32 rpe_audio_index = m_CurrentPlaySession.strAlterAudioSubStreamID;
	VO_U32 period_index = m_CurrentPlaySession.strPeriodIndex;
	Representation * rpe_item = NULL;
	rpe_item = m_MpdParser.get_qualityLevel(audio_group_id,rpe_audio_index); 
	if(rpe_item == NULL)
		return -1;
	Seg_item * item = NULL;;
	m_MpdParser.get_segmentItem(&item,rpe_item,m_chunk_audio_index);
	// add for multi_bitrate_index range sigx box
	Seg_item * check_item = NULL;
	VO_BOOL   check_flag =  VO_TRUE;
	m_MpdParser.get_segmentItem(&check_item,rpe_item,0);
	if(check_item&&check_item->is_sigx_index_chunk&&!check_item->is_index_parsed)
	{
		check_flag = VO_FALSE;
		item = check_item;
	}
	VOLOGW("GetAudio_IndexStream audio  %lld",m_chunk_audio_index);
	VOLOGW(" item is audio_index :%d",m_chunk_audio_index);
	if(check_flag&&(item == NULL||m_chunk_audio_index>=rpe_item->byterange_chunk_count))
	{
		VOLOGW("audio_index end");
		return VO_RET_SOURCE2_END;
	}
	else
	{
		strcpy(pChunckItem->strChunckItemURL,rpe_item->initalUrl);
		m_MpdParser.replace(pChunckItem->strChunckItemURL,"&amp;","&");
		pChunckItem->eChunckContentType = E_CHUNK_AUDIO;
		pChunckItem->is_SidBox = item->is_sigx_index_chunk;
		if(pChunckItem->is_SidBox)
		{
			if(rpe_item->m_index_range_exact)
			rpe_item->chunk_range_offset = item->first_offset+item->size-INDEX_RANGE__END_OFFSET;
			else
			rpe_item->chunk_range_offset = item->first_offset+item->size;
			rpe_item->current_sigx_offset_time = item->starttime;
			if(!m_audio_stream_index&&item->is_index_parsed == VO_FALSE)
			{
				m_chunk_audio_index-= 1;
			}
			m_audio_stream_index =  VO_FALSE;
		}
		item->is_index_parsed  =  VO_TRUE;
		pChunckItem->ullBeginTime = INAVALIBLEU64;
		pChunckItem->ullEndTime = INAVALIBLEU64;
		pChunckItem->Start_number = item->startnmber;
		pChunckItem->ulDurationInMsec = item->duration;
		pChunckItem->ulChunckIndex = m_chunk_audio_index;
		pChunckItem->ulChunckOffset = item->first_offset;
		pChunckItem->ulChunckLen = item->size;
		pChunckItem->StartTime = item->starttime;
		pChunckItem->range_flag = VO_TRUE;
		m_pure_audio_current_time = pChunckItem->StartTime;
		VOLOGW(" range_item is  audio_index_count :%d",rpe_item->byterange_chunk_count);
		VOLOGW(" range_item is  audio_StartTime :%lld",pChunckItem->StartTime);
		VOLOGW(" range_item is  GetAudio_IndexStream_url :%s",pChunckItem->strChunckItemURL);
		VOLOGW(" range_item is  GetAudio_IndexStream len :%d, offset,%d, audio_index,%d",pChunckItem->ulChunckLen ,pChunckItem->ulChunckOffset,m_chunk_audio_index); 
		m_chunk_audio_index++;
		return VO_RET_SOURCE2_OK;
	}
}
/*add for ts_chunk*/
VO_U32   C_MPD_Manager::GetSubtitle_IndexChunkPlatListItem(S_CHUNCK_ITEM*    pChunckItem)
{
	VO_U32	  ulRet = VO_RET_SOURCE2_OK;
	VO_CHAR url_segment[1024];
	memset(url_segment, 0x00, sizeof(url_segment));
	VO_U32 subtile_group_id = m_CurrentPlaySession.strAlterSubtileGroupID;
	VO_U32 rpe_subtitle_index = 0;
	VO_U32 period_index = m_CurrentPlaySession.strPeriodIndex;
	Representation * rpe_item = NULL;
	rpe_item= m_MpdParser.get_qualityLevel(subtile_group_id,rpe_subtitle_index);
	if(rpe_item==NULL)
		return -1;
	Seg_item * item = NULL;
	m_MpdParser.get_segmentItem(&item,rpe_item,m_chunk_subtitle_index);
	Seg_item * check_item = NULL;
	VO_BOOL   check_flag =  VO_TRUE;
	m_MpdParser.get_segmentItem(&check_item,rpe_item,0);
	if(check_item&&check_item->is_sigx_index_chunk&&!check_item->is_index_parsed)
	{
		check_flag = VO_FALSE;
		item = check_item;
	}
	if(check_flag&&(item == NULL||m_chunk_subtitle_index>=rpe_item->byterange_chunk_count))
	{
		return VO_RET_SOURCE2_END;
	}
	else
	{
		strcpy(pChunckItem->strChunckItemURL,rpe_item->initalUrl);
		pChunckItem->eChunckContentType = E_CHUNK_SUBTITLE;
		pChunckItem->is_SidBox = item->is_sigx_index_chunk;
		if(pChunckItem->is_SidBox)
		{
			item->is_index_parsed  =  VO_TRUE;
			if(rpe_item->m_index_range_exact)
				rpe_item->chunk_range_offset = item->first_offset+item->size-INDEX_RANGE__END_OFFSET;
			else
				rpe_item->chunk_range_offset = item->first_offset+item->size;
			if(!m_video_stream_index)
			{
				m_chunk_video_index-= 1;
			}
			m_video_stream_index =  VO_FALSE;
		}
		item->is_index_parsed  =  VO_TRUE;
		pChunckItem->ullBeginTime = INAVALIBLEU64;
		pChunckItem->ullEndTime = INAVALIBLEU64;
		pChunckItem->Start_number = item->startnmber;
		pChunckItem->ulDurationInMsec = item->duration;
		pChunckItem->ulChunckIndex = m_chunk_video_index;
		pChunckItem->ulChunckOffset = item->first_offset;
		pChunckItem->ulChunckLen = item->size;
		pChunckItem->StartTime = item->starttime;
		pChunckItem->range_flag = VO_TRUE;
		VOLOGW(" range_item is  GetSubtitle_IndexChunkPlatListItem :%s",pChunckItem->strChunckItemURL);
		VOLOGW(" range_item is  GetSubtitle_IndexChunkPlatListItem len :%d, offset,%d",pChunckItem->ulChunckLen ,pChunckItem->ulChunckOffset); 
		m_chunk_subtitle_index++;
		return VO_RET_SOURCE2_OK;
	}

}
VO_U32   C_MPD_Manager::GetAV_ChunkPlatListItem(S_CHUNCK_ITEM*    pChunckItem)
{
	VO_U32	  ulRet = VO_RET_SOURCE2_OK;
	VO_CHAR url_segment[1024];
	memset(url_segment, 0x00, sizeof(url_segment));
	VO_U32 video_group_id = m_CurrentPlaySession.strAlterVideoGroupID;
	VO_U32 rpe_video_index = m_CurrentPlaySession.strAlterVideoSubStreamID;
	VO_U32 period_index = m_CurrentPlaySession.strPeriodIndex;
	Representation * rpe_item = NULL;
	rpe_item= m_MpdParser.get_qualityLevel(video_group_id,rpe_video_index);
	if(rpe_item==NULL)
		return -1;
	current_video_rpe = rpe_item;
	m_MpdParser.Set_rpe_index(rpe_video_index,1);
	Seg_item * item = NULL;
	m_MpdParser.get_segmentItem(&item,m_chunk_video_index,1,period_index);
	VO_U32 index = GetMaxChunkIndex(1);
	if(item&&item->bDependForNextTimeLine&&m_chunk_video_index>=index&&!Is_Live()&&index!=0)
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
			pChunckItem->ullEndTime =  pChunckItem->ullBeginTime+DEAD_LENGTH;
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
		if(ulRet!= VO_RET_SOURCE2_FAIL)
		{
			memcpy(pChunckItem->strChunckItemURL,url_segment,strlen(url_segment));
		}
		VO_U32 ini_range = 0;
		VO_U32 size = 0;
		VO_U32 end_range = 0;
		if(item->range[0]!='\0')
			m_MpdParser.GetChunkRange(item->range,&ini_range,&end_range,&size);
		if(size!=0)
		{
			pChunckItem->ulChunckOffset = ini_range;
			pChunckItem->ulChunckLen = size;
			pChunckItem->range_flag = VO_TRUE;

		}
		else
		{
			pChunckItem->range_flag = VO_FALSE;
		}
		VOLOGW( "GetAV_ChunkPlatListItem av_url  %s start_time %lld start_number %d" , url_segment,pChunckItem->StartTime,pChunckItem->Start_number);
		m_chunk_video_index++;
		return VO_RET_SOURCE2_OK;
	}
	else 
	{
	    VOLOGW(" end av ddddddd end return end");
		return VO_RET_SOURCE2_END;
	}


}
/*end for ts_chunk*/
VO_U32   C_MPD_Manager::GetVideoPlayListItem( S_CHUNCK_ITEM*    pChunckItem)
{
	VO_U32	  ulRet = VO_RET_SOURCE2_OK;
	VO_CHAR url_segment[1024];
	memset(url_segment, 0x00, sizeof(url_segment));
	VO_U32 video_group_id = m_CurrentPlaySession.strAlterVideoGroupID;
	VO_U32 rpe_video_index = m_CurrentPlaySession.strAlterVideoSubStreamID;
	VO_U32 period_index = m_CurrentPlaySession.strPeriodIndex;
	Representation * ql=NULL;
	if(m_manifest.adaptionSet&&m_manifest.adaptionSet[0].m_ptr_QL_head!=NULL)
		ql = m_manifest.adaptionSet[0].m_ptr_QL_head;
	else
		return -1;

	if(ql->track_count!=2&&m_manifest.group_count>6&&!(m_MpdParser.m_period_count>1))
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
		rpe_item= m_MpdParser.get_qualityLevel(video_group_id+1,rpe_video_index);
	}
	VO_U64 index = GetMaxChunkIndex(1);
	VOLOGW(" max_index_video %lld",index);
	Seg_item * item = NULL;
	m_MpdParser.get_segmentItem(&item,m_chunk_video_index,1,period_index);
	if(item&&(item->bDependForNextTimeLine||m_manifest.period_count>1)&&m_chunk_video_index>=index&&!Is_Live()&&index!=0)
	{
		item =  NULL;
		VOLOGW(" item is null video_chunk_index :%d, max_index,%d",m_chunk_video_index,index);

	}
	if(item!=NULL)
	{
		pChunckItem->StartTime = item->starttime;
		if(m_MpdParser.Is_Dynamic())
		{
		/*	pChunckItem->ullBeginTime =item->live_time;;*/
			pChunckItem->ullBeginTime =item->live_time+ m_MpdParser.m_manifest->m_period[0].period_start-WORKROUND_ELEMENTAL;
			VO_U64 duration = m_MpdParser.m_manifest->m_period[0].period_fact_duration;
			if(duration!=0)
			pChunckItem->ullEndTime = pChunckItem->ullBeginTime+duration;
			else
			pChunckItem->ullEndTime = pChunckItem->ullBeginTime+DEAD_LENGTH;
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
		if(ulRet!= VO_RET_SOURCE2_FAIL)
		memcpy(pChunckItem->strChunckItemURL,url_segment,strlen(url_segment));
		VO_U32 ini_range = 0;
		VO_U32 size = 0;
		VO_U32 end_range = 0;
		if(item->range[0]!='\0')
			m_MpdParser.GetChunkRange(item->range,&ini_range,&end_range,&size);
		if(size!=0)
		{
			pChunckItem->ulChunckOffset = ini_range;
			pChunckItem->ulChunckLen = size;
			pChunckItem->range_flag = VO_TRUE;

		}
		else
		{
			pChunckItem->range_flag = VO_FALSE;
		}
		VOLOGW(" item is video1  video_chunk_index :%d, max_index,%d",m_chunk_video_index,index);
		VOLOGW(" item is video 2 start_time :%lld, duration,%lld,url,%s",pChunckItem->StartTime,pChunckItem->ulDurationInMsec, pChunckItem->strChunckItemURL);
		m_chunk_video_index++;
		return VO_RET_SOURCE2_OK;
	} 
	else
	{
		VOLOGW(" end video end return end");
		return VO_RET_SOURCE2_END;

	}


}


VO_U32   C_MPD_Manager::GetAudioPlayListItem(S_CHUNCK_ITEM*    pChunckItem)
{

	VO_U32	   ulRet = 0;
	VO_CHAR url_segment[1024];
	memset(url_segment, 0x00, sizeof(url_segment));
	VO_U32 audio_group_id = m_CurrentPlaySession.strAlterAudioGroupID;
	VO_U32 rpe_audio_index = m_CurrentPlaySession.strAlterAudioSubStreamID;
	VO_U32 period_index = m_CurrentPlaySession.strPeriodIndex;
	Representation * rpe_item = NULL;
	rpe_item = m_MpdParser.get_qualityLevel(audio_group_id,rpe_audio_index); 
	if(rpe_item == NULL)
		return -1;
	Seg_item * item = NULL;
	VO_U32 index = GetMaxChunkIndex(0); //get_audio_index
	m_MpdParser.get_segmentItem(&item,m_chunk_audio_index,0,period_index);
	if(item&&(item->bDependForNextTimeLine||m_manifest.period_count>1)&&m_chunk_audio_index>=index&&!Is_Live()&&index!=0)
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
			pChunckItem->ullBeginTime =item->live_time+ m_MpdParser.m_manifest->m_period[0].period_start-WORKROUND_ELEMENTAL;
			//pChunckItem->ullEndTime = pChunckItem->ullBeginTime+4000;
		    VO_U64 duration = m_MpdParser.m_manifest->m_period[0].period_fact_duration;
		    if(duration!=0)
			pChunckItem->ullEndTime = pChunckItem->ullBeginTime+duration;
			else
			pChunckItem->ullEndTime = pChunckItem->ullBeginTime+DEAD_LENGTH;
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
		else
		{
			VOLOGW(" audio url is NULL !");
		}
		VO_U32 ini_range = 0;
		VO_U32 size = 0;
		VO_U32 end_range = 0;
		m_MpdParser.GetChunkRange(item->range,&ini_range,&end_range,&size);
		if(size!=0)
		{
			pChunckItem->ulChunckOffset = ini_range;
			pChunckItem->ulChunckLen = size;
			pChunckItem->range_flag = VO_TRUE;

		}
		else
		{
			pChunckItem->range_flag = VO_FALSE;
		}
		VOLOGW( "GetAudioPlayListItem audio_url %s origianal_number %lld start_number %d m_chunk_audio_index %lld" , url_segment,item->nOrignalStartNumber,pChunckItem->Start_number,m_chunk_audio_index);
		m_chunk_audio_index++;
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
	VO_U32 video_group_id = m_CurrentPlaySession.strAlterVideoGroupID;
	VOLOGI("GetVideo_TrackPlayListItem video_group_id = %d,video_index=%d,audio_index=%d",video_group_id,m_chunk_video_index,m_chunk_audio_index);
	VO_U32 rpe_video_index = m_CurrentPlaySession.strAlterVideoSubStreamID;
	Representation * ql =  NULL;
	if(m_manifest.adaptionSet)
		ql = m_manifest.adaptionSet[0].m_ptr_QL_head;
	if(ql&&ql->track_count!=2&&m_manifest.group_count>4&&m_manifest.period_count<2)
	{
		video_group_id = 6;
		rpe_video_index = 0;
	}
	Representation * rpe_item = NULL ;
	rpe_item = m_MpdParser.get_qualityLevel(video_group_id,rpe_video_index);
	if(rpe_item&&rpe_item->track_count==2)
	{
		video_group_id =video_group_id+1;
		//add for muxed file and only muxed file
		if(m_MpdParser.Is_dash_muxed(m_CurrentPlaySession.strPeriodIndex))
		{
			video_group_id =0;
		}
		rpe_item = m_MpdParser.get_qualityLevel(video_group_id,rpe_video_index);
	}
	else if(rpe_item==NULL)
	{   
		rpe_video_index = 0;
		rpe_item = m_MpdParser.get_qualityLevel(1,rpe_video_index);
		return -1;

	}
	m_MpdParser.Set_rpe_index(rpe_video_index,1);
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
	m_MpdParser.replace(pChunckItem->strChunckItemURL,"&amp;","&");
	pChunckItem->StartTime = 0;
	VO_U32 ini_range = 0;
	VO_U32 size = 0;
	VO_U32 end_range = 0;
	m_MpdParser.GetChunkRange(rpe_item->ini_range,&ini_range,&end_range,&size);
	VO_U32 index_range = 0;
	VO_U32 index_size = 0;
	VO_U32 index_end_range = 0;
	m_MpdParser.GetChunkRange(rpe_item->index_range,&index_range,&index_end_range,&index_size);
	if(index_size==0&&size!=0)
	{
		size = 0;
	}
	if(size!=0)
	{
		VOLOGI("GetVideo_TrackPlayListItem ini_range has not be set 0  %s",pChunckItem->strChunckItemURL);
		pChunckItem->ulChunckOffset = ini_range;
		pChunckItem->ulChunckLen = size;
		pChunckItem->range_flag = VO_TRUE;

	}
	else
	{
		pChunckItem->range_flag = VO_FALSE;
	}
	/*if(rpe_item->head_range_end!=0)
	{
		pChunckItem->ulChunckOffset = 0;
		pChunckItem->ulChunckLen = rpe_item->head_range_end+1;
		pChunckItem->range_flag = VO_TRUE;
	}*/
	current_video_rpe = rpe_item;
	pChunckItem->ullBeginTime = INAVALIBLEU64;
	pChunckItem->ullEndTime = INAVALIBLEU64;
	return ulRet;
}


VO_U32   C_MPD_Manager::GetAudio_TrackPlayListItem(S_CHUNCK_ITEM*    pChunckItem)
{
	VO_U32	 ulRet = 0;
	VO_U32 audio_group_id = m_CurrentPlaySession.strAlterAudioGroupID;
	VOLOGW("GetAudio_TrackPlayListItem 1111 %d",audio_group_id);
	VO_U32 rpe_audio_index = m_CurrentPlaySession.strAlterAudioSubStreamID;
	Representation * rpe_item = m_MpdParser.get_qualityLevel(audio_group_id,rpe_audio_index);
	if(rpe_item==NULL)
		return -1;
	m_MpdParser.Set_rpe_index(rpe_audio_index,0);
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
	m_MpdParser.replace(pChunckItem->strChunckItemURL,"&amp;","&");
	VO_U32 ini_range = 0;
	VO_U32 size = 0;
	VO_U32 end_range = 0;
	m_MpdParser.GetChunkRange(rpe_item->ini_range,&ini_range,&end_range,&size);
	VO_U32 index_size = 0;
	VO_U32 index_range = 0;
    VO_U32 index_end_range = 0;
	m_MpdParser.GetChunkRange(rpe_item->index_range,&index_range,&index_end_range,&index_size);
	if(index_size==0&&size!=0)
	{
		size = 0;
	}
	if(size!=0)
	{
		pChunckItem->ulChunckOffset = ini_range;
		pChunckItem->ulChunckLen = size;
		pChunckItem->range_flag = VO_TRUE;

	}
	else
	{
		pChunckItem->range_flag = VO_FALSE;
	}
	/* if(rpe_item->head_range_end!=0)
	{
		pChunckItem->ulChunckOffset = 0;
		pChunckItem->ulChunckLen = rpe_item->head_range_end+1;
		pChunckItem->range_flag = VO_TRUE;
	}*/
	current_audio_rpe = rpe_item;
	pChunckItem->StartTime = 0;
	pChunckItem->ullBeginTime = INAVALIBLEU64;
	pChunckItem->ullEndTime = INAVALIBLEU64;
	return ulRet;

}
VO_U32   C_MPD_Manager::GetSubtitle_ChunkPlatListItem(S_CHUNCK_ITEM*    pChunckItem)
{
	VO_U32	  ulRet = VO_RET_SOURCE2_OK;
	VO_U32 subtitle_id = m_CurrentPlaySession.strAlterSubtileGroupID;
	VO_U32 rpe_subtitle_index = 0;
	VO_U32 period_index = m_CurrentPlaySession.strPeriodIndex;
	Representation * rpe_item = NULL;
	rpe_item= m_MpdParser.get_qualityLevel(subtitle_id,rpe_subtitle_index);
	if(rpe_item==NULL)
		return -1;
	Seg_item * item = NULL;
	VO_U32 index = GetMaxChunkIndex(0); //get_audio_inde;
	m_MpdParser.get_segmentItem_subtitle(&item,m_chunk_subtitle_index,period_index);
	if(item&&(item->bDependForNextTimeLine||m_manifest.period_count>1)&&m_chunk_subtitle_index>=index&&!Is_Live()&&index!=0)
	{
		item =  NULL;
		VOLOGW(" item is null subtile :%d",m_chunk_subtitle_index);
	}
	if(item&&item->bDependForNextTimeLine)
	{
		item->startnmber = item->nOrignalStartNumber+m_chunk_subtitle_index;
	}
	if(item)
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
		char str[255];
		memset(str,0x00,sizeof(str));
		sprintf(str,c,rpe_item->bitrate);
		memcpy(pChunckItem->strChunckItemURL,rpe_item->mediaUrl,strlen(rpe_item->mediaUrl));
		m_MpdParser.replace(pChunckItem->strChunckItemURL,"$Bandwidth$",str);
		m_MpdParser.replace(pChunckItem->strChunckItemURL,"$RepesendationID$",rpe_item->id);
		m_MpdParser.replace(pChunckItem->strChunckItemURL,"$RepesendationID$",rpe_item->id);
		m_MpdParser.replace(pChunckItem->strChunckItemURL,"$RepresentationID$",rpe_item->id);
		memset(str,0x00,sizeof(str));
		sprintf(str,c,item->startnmber);
		m_MpdParser.replace(pChunckItem->strChunckItemURL,"$Number$",str);
		m_MpdParser.replace(pChunckItem->strChunckItemURL,"$Number%05$",str);
		pChunckItem->StartTime =item->starttime;
		pChunckItem->ullBeginTime = INAVALIBLEU64;
		pChunckItem->ullEndTime = INAVALIBLEU64;
		pChunckItem->ulDurationInMsec = item->duration;
		pChunckItem->eChunckContentType = E_CHUNK_SUBTITLE;
		m_chunk_subtitle_index++;
		VOLOGW(" item is subtile start_time :%lld, duration,%lld,url,%s",pChunckItem->StartTime,pChunckItem->ulDurationInMsec, pChunckItem->strChunckItemURL);
		return    ulRet;
	}
	else
		return VO_RET_SOURCE2_END;

}
VO_U32   C_MPD_Manager::GetSubtitle_TrackChunkPlatListItem(S_CHUNCK_ITEM*    pChunckItem)
{
    VO_CHAR url_segment[1024];
	memset(url_segment, 0x00, sizeof(url_segment));
	VO_U32	  ulRet = VO_RET_SOURCE2_OK;
	VO_U32 subtitle_id = m_CurrentPlaySession.strAlterSubtileGroupID;
	VO_U32 rpe_subtitle_index = 0;
	VO_U32 period_index = m_CurrentPlaySession.strPeriodIndex;
	Representation * rpe_item = NULL;
	rpe_item= m_MpdParser.get_qualityLevel(subtitle_id,rpe_subtitle_index);
	if(rpe_item==NULL)
		return -1;
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
	m_MpdParser.replace(pChunckItem->strChunckItemURL,"$RepesendationID$",rpe_item->id);
	m_MpdParser.replace(pChunckItem->strChunckItemURL,"$RepresentationID$",rpe_item->id);
	pChunckItem->StartTime = 0;
	pChunckItem->ullBeginTime = INAVALIBLEU64;
	pChunckItem->ullEndTime = INAVALIBLEU64;
	//if(m_MpdParser.m_is_ttml)
	pChunckItem->ulDurationInMsec = INAVALIBLEU64;
	pChunckItem->eChunckContentType = E_CHUNK_TRACK;
	current_subtile_rpe = rpe_item;
	VOLOGW(" item is subtile_track start_time :%lld, duration,%lld,url,%s",pChunckItem->StartTime,pChunckItem->ulDurationInMsec, pChunckItem->strChunckItemURL);
//	m_chunk_subtitle_index++;
	return    ulRet;
}

VO_U32 C_MPD_Manager::pop_fragment(  Representation * rpe_item ,Seg_item * ptr_item, VO_CHAR * ptr_path, VO_S64 last_network_bitrate)
{
	if(!ptr_item&&!(ptr_item->group_index >= 0))
		return VO_RET_SOURCE2_FAIL;
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
		if(rpe_item==NULL)
		{
			return VO_RET_SOURCE2_FAIL;
		}
		/*memcpy(ptr_item->item_url,rpe_item->mediaUrl,strlen(rpe_item->mediaUrl));*/
		strcpy(ptr_item->item_url,rpe_item->mediaUrl);
		if(ptr_item->item_url[0] == 0)
			return VO_RET_SOURCE2_FAIL;
		char str[255];
		memset(str,0x00,sizeof(str));
		sprintf(str,c,rpe_item->bitrate);
		m_MpdParser.replace(ptr_item->item_url,"$Bandwidth$",str);
		memset(str,0x00,sizeof(str));
		sprintf(str,c, ptr_item->starttime);
		m_MpdParser.replace(ptr_item->item_url,"$Time$",str);
		m_MpdParser.replace(ptr_item->item_url,"$RepesendationID$",rpe_item->id);
		m_MpdParser.replace(ptr_item->item_url,"$RepresentationID$",rpe_item->id);
		m_MpdParser.replace(ptr_item->item_url,"&amp;","&");
		memset(str,0x00,sizeof(str));
		VO_CHAR *format_url_back = NULL;
		format_url_back = strstr(ptr_item->item_url,"%");
		/*VO_CHAR *format_url_back_1 = strstr(ptr_item->item_url,"d$");*/
		VO_CHAR *format_url_back_1 = NULL;
		if(format_url_back_1)
		format_url_back_1 = strstr(format_url_back,"d$");		
		VO_U32 copy_len = 0;
		VO_CHAR *format_width = NULL;
		if(format_url_back&&format_url_back_1)
		{
			copy_len = strlen(format_url_back)-strlen(format_url_back_1)+1;
			format_width = new VO_CHAR[copy_len];
		}
		else if(format_url_back&&format_url_back_1==NULL)
		{
			format_url_back_1 = strstr(format_url_back,"$");
			copy_len = strlen(format_url_back)-strlen(format_url_back_1);
			format_width = new VO_CHAR[copy_len];

		}
		if(copy_len>0)
		{
			memcpy(format_width,format_url_back,copy_len+1);
			m_MpdParser.replace(format_width,"$","");	
			format_width[copy_len]= '\0';
		}
		if(strstr(ptr_item->item_url,"%0"))
		{
			if(strstr(ptr_item->item_url,"d"))
			sprintf(str,format_width,ptr_item->startnmber);
			else
			{
				char new_format_width[255];
				strcpy(new_format_width,format_width);
				strcat(new_format_width,"d");
				sprintf(str,new_format_width,ptr_item->startnmber);
			}

		}
		else 
			sprintf(str,c,ptr_item->startnmber);
		VO_CHAR number_format[255];
		memset(number_format,0x00,sizeof(number_format));
		if((format_width))
		{
			strcat(number_format,"$Number");
			strcat(number_format,format_width);
			strcat(number_format,"$");
		}

		m_MpdParser.replace(ptr_item->item_url,"$Number$",str);
		m_MpdParser.replace(ptr_item->item_url,number_format,str);
		sprintf(str,c,ptr_item->startnmber);
		m_MpdParser.replace(ptr_item->item_url,"$Index$",str);
		m_MpdParser.replace(ptr_item->item_url,number_format,str);
		memcpy(ptr_path,ptr_item->item_url,strlen(ptr_item->item_url));
	}
	return VO_RET_SOURCE2_OK ;

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
VO_U64 C_MPD_Manager::Findbyterange_pos(VO_U64   ulTime,VO_U32 period_index)
{
    VO_U64 return_time = 0;
	VO_U32 chunk_index = 0;
	VO_U32 video_group_id = m_CurrentPlaySession.strAlterVideoGroupID;
	VO_U32 rpe_video_index = m_CurrentPlaySession.strAlterVideoSubStreamID;
	Representation * rpe_item = NULL ;
	rpe_item = m_MpdParser.get_qualityLevel(video_group_id,rpe_video_index);
	m_MpdParser.Set_rpe_index(rpe_video_index,1);
	VO_U32 audio_group_id = m_CurrentPlaySession.strAlterAudioGroupID;
	VO_U32 rpe_audio_index = m_CurrentPlaySession.strAlterAudioSubStreamID;
	Representation * rpe_item_audio = NULL ;
	rpe_item_audio = m_MpdParser.get_qualityLevel(audio_group_id,rpe_audio_index);
	VO_U32 duration = 0;
	GetDuration(&duration);
	{
		VO_U64 time = Findbyterange_time(ulTime,rpe_item);
		chunk_index = Find_byte_range_index(ulTime,rpe_item);
		return_time = time;
		m_chunk_video_index  = chunk_index;
		VOLOGW("Findbyterange_pos1 video_time: %lld",return_time);
		VOLOGW("Findbyterange_pos1 video_index: %lld",m_chunk_video_index);
		if(rpe_item_audio)
		{
			if(rpe_item_audio->byterange_chunk_count>1)
			{
				time = Findbyterange_time(ulTime,rpe_item_audio);
				return_time = time;
				chunk_index = Find_byte_range_index(ulTime,rpe_item_audio);
				VOLOGW("Findbyterange_pos1 audio_time: %lld",return_time);
			}
			m_chunk_audio_index = chunk_index;
			VOLOGW("Findbyterange_pos2 audio_index: %d",chunk_index);
		}
		else
		{
				VOLOGW("Findbyterange_pos3 pure_video: %d",chunk_index);
				m_chunk_audio_index  = m_chunk_video_index;
		}
	}
	VOLOGW("Findbyterange_pos chunk_index: %d",chunk_index);
	return return_time;

}
VO_U32   C_MPD_Manager::Findbyterange_video_time(VO_U64   ulTime,VO_U32 period_index)
{
	if(ulTime==0)
		return 0;
	VO_U32 chunk_index = 0;
	VO_U32 video_group_id = m_CurrentPlaySession.strAlterVideoGroupID;
	VO_U32 rpe_video_index = m_CurrentPlaySession.strAlterVideoSubStreamID;
	Representation * rpe_item = NULL ;
	rpe_item = m_MpdParser.get_qualityLevel(video_group_id,rpe_video_index);
	m_MpdParser.Set_rpe_index(rpe_video_index,1);
	VO_U64 time = Findbyterange_time(ulTime,rpe_item);
	chunk_index = Find_byte_range_index(ulTime,rpe_item);
	return chunk_index;

}
VO_U64   C_MPD_Manager::Findbyterange_time(VO_U64   ulTime,Representation * rpe_item)
{
	Seg_item * ptr_temp = rpe_item->m_ptr_SG_head;
	Seg_item * tmpPtr = NULL;
	Seg_item * tmpPtrPre = NULL;
	VO_S64 start_time = 0;
	if(ptr_temp == NULL)
		return 0;
	while(ptr_temp)
	{
		if(ulTime >= ptr_temp->starttime)
			{
				tmpPtrPre = tmpPtr;
				tmpPtr = ptr_temp;
			}
			else if(ulTime < ptr_temp->starttime )
			{
				break;
			}
		
		ptr_temp = ptr_temp->ptr_next;
	}

	return tmpPtr ? tmpPtr->starttime :start_time;
}
VO_U32   C_MPD_Manager::Find_byte_range_index(VO_U64 ulTime,Representation * rpe_item)
{
	Seg_item * ptr_temp = rpe_item->m_ptr_SG_head;
	VO_U32 index_video = 0;
	Seg_item * tmpPtr = NULL;
	Seg_item * tmpPtrPre = NULL;
	while(ptr_temp )
	{
		if(ulTime >= ptr_temp->starttime)
		{
			   index_video++;
			   tmpPtrPre = tmpPtr;
			   tmpPtr = ptr_temp;
		}
		else if(ulTime < ptr_temp->starttime)
		{ 
			//if((tmpPtrPre->is_sigx_index_chunk&&!tmpPtrPre->is_index_parsed)||(!tmpPtr->is_sigx_index_chunk))
			break;

		}
		ptr_temp = ptr_temp->ptr_next;
	}
	/*if(tmpPtrPre->is_sigx_index_chunk&&tmpPtrPre->is_index_parsed)
		index_video = index_video-1;*/
	return tmpPtrPre?index_video-1:0;

}
VO_U64   C_MPD_Manager::SetThePos(VO_U64   ulTime,VO_U32 period_index)
{
	VO_U64 start_time = 0;
	VO_S64 index = 0;
	if(ulTime == 0) 
	{
		m_chunk_video_index = index;
		m_chunk_audio_index = index;
		return 0;
	}
	start_time = m_MpdParser.find_fragment_starttime(ulTime, m_CurrentPlaySession.ulAlterVideoSequenceId ==-1?0 : 1,period_index) ;//0 audio, 1 video
	index = m_MpdParser.find_fragment_starttime_index(&start_time, m_CurrentPlaySession.ulAlterVideoSequenceId ==-1?0 : 1 ,period_index);
	m_chunk_video_index = index;
	m_chunk_audio_index = index;
	m_chunk_subtitle_index = index;
	VOLOGW("manager start_time :%lld",start_time);
	VOLOGW("manager m_chunk_video_index :%lld",m_chunk_video_index);
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

