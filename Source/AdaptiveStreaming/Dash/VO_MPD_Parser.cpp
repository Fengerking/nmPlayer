
#include "VO_MPD_Parser.h"
#include "voString.h"
#include "voOSFunc.h"
#include "voLog.h"
#include "math.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define COUNTS_NUMBER 10240


#ifdef _WIN32
#define _ATO64(a) _atoi64(a) 
#elif defined WINCE
#define _ATO64(a) _atoi64(a) 
#elif defined LINUX || defined _IOS
#define _ATO64(a) atoll(a) 
#endif 



#define be16toh(x) 	(( x & 0xff00) >> 8 | ( x & 0xff) << 8)

const int AAC_SampRateTab_MY[12] = {
	96000, 88200, 64000, 48000, 44100, 32000, 
	24000, 22050, 16000, 12000, 11025,  8000
};

VO_MPD_Parser::VO_MPD_Parser()
:m_manifest(0)
,m_file_content(NULL)
,m_seg_temlate(VO_FALSE)
,m_seg_count(0)
,m_ptr_FI_head(0)
,m_ptr_FI_tail(0)
,m_ptr_sub_FI_head(0)
,m_ptr_sub_FI_tail(0)
,video_flag(0)
,audio_flag(0)
,m_pXmlLoad(NULL)
,m_is_index_range_video(VO_FALSE)
,m_is_index_range_audio(VO_FALSE)
,m_is_index_range_subtitle(VO_FALSE)
{	
	InitAVList();
	CreateAVList(COUNTS_NUMBER);
	FlushAVList();
	video_scale = 0;
	audio_scale = 0;
	subtile_scale = 0;
	m_count_audio = 0;
	m_count_video = 0;
	m_count_subtitle = 0;
	m_videoCount = 0;
	m_audioCount = 0;
	m_group_count = 0;
	m_subtitleCount = 0;
	m_duration = 0;
	m_is_depend = VO_FALSE;
	m_is_ts = VO_FALSE;
	m_is_muxed = VO_FALSE;
	m_is_pure_video =VO_FALSE ;
	m_is_pure_audio = VO_FALSE;
	m_time_line_time = 0;
	m_period_count = 0;
	m_pXmlLoad = new CXMLLoad;
	memset(mpd_base_url,0x00,sizeof(mpd_base_url));
	current_video_rpe = 0;
	current_audio_rpe = 0;
	m_audio_bitrate_count = 0;
	m_is_ttml = VO_TRUE;
	
}

VO_MPD_Parser::~VO_MPD_Parser()
{
	VOLOGE("VO_MPD_Parser destroy 1");
	destroy();
	DestroyAVList();
	//	FlushSpecTag();
	delete m_pXmlLoad;
	m_pXmlLoad = NULL;
	VOLOGE("VO_MPD_Parser destroy 2");
}



int VO_MPD_Parser::vowcslen(const vowchar_t* str) {   
	int len = 0;
	while (*str != '\0') {
		str++;
		len++;
	}
	return len;
} 

int VO_MPD_Parser::VO_UnicodeToUTF8(const vowchar_t *szSrc, const int nSrcLen, char *strDst, const int nDstLen) 
{

	VOLOGR("Enter to VO_UnicodeToUTF8 file size: %d ", nDstLen);

	int is = 0, id = 0;
	const vowchar_t *ps = szSrc;
	unsigned char *pd = (unsigned char*)strDst;

	if (nDstLen <= 0)
		return 0;

	for (is = 0; is < nSrcLen; is++) {
		if (BYTE_1_REP > ps[is]) { /* 1 byte utf8 representation */
			if (id + 1 < nDstLen) {
				pd[id++] = (unsigned char)ps[is];
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} else if (BYTE_2_REP > ps[is]) {
			if (id + 2 < nDstLen) {
				pd[id++] = (unsigned char)(ps[is] >> 6 | 0xc0);
				pd[id++] = (unsigned char)((ps[is] & 0x3f) | 0x80);
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} else if (BYTE_3_REP > ps[is]) { /* 3 byte utf8 representation */
			if (id + 3 < nDstLen) {
				pd[id++] = (unsigned char)(ps[is] >> 12 | 0xe0);
				pd[id++] = (unsigned char)(((ps[is] >> 6)  & 0x3f) | 0x80);
				pd[id++] = (unsigned char)((ps[is] & 0x3f) | 0x80);
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} else if (BYTE_4_REP > ps[is]) { /* 4 byte utf8 representation */
			if (id + 4 < nDstLen) {
				pd[id++] = (unsigned char)(ps[is] >> 18 | 0xf0);
				pd[id++] = (unsigned char)(((ps[is] >> 12)  & 0x3f) | 0x80);
				pd[id++] = (unsigned char)(((ps[is] >> 6)  & 0x3f) | 0x80);
				pd[id++] = (unsigned char)((ps[is] & 0x3f) | 0x80);
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} /* @todo Handle surrogate pairs */
	}

	pd[id] = '\0'; /* Terminate string */

	return id; /* This value is in bytes */
}


VO_CHAR *  VO_MPD_Parser::getRepresentationUrl(int stream_index,int representation_index){
	Representation * temp = m_manifest->adaptionSet[stream_index].m_ptr_QL_head;
	char * temp_url = NULL;
	while(temp){
		if(temp->index_QL == representation_index){
			temp_url = temp->initalUrl;
			strcat(m_manifest->baseurl,temp_url);
			return m_manifest->baseurl;
		}
		temp = temp->ptr_next;
	}
	return NULL;
}


VO_BOOL VO_MPD_Parser::get_segmentItem(Seg_item ** ptr_item,  VO_U64 ini_chunk ){
	Seg_item * temp = m_ptr_FI_head;
	int j = 0;
	while(j<ini_chunk && temp){
		j++;
		temp = temp->ptr_next;
	}
	if(j ==ini_chunk){
		*ptr_item = temp;
		return VO_TRUE;
	}
	return VO_FALSE;
}

VO_BOOL VO_MPD_Parser::get_segmentItem(Seg_item ** ptr_item,Representation * rpe,  VO_U64 index ){
	Seg_item * temp = rpe->m_ptr_SG_head;
	VO_U64 j = 0;
	while(j<index && temp){
		j++;
		temp = temp->ptr_next;
	}
	if(j ==index){
		*ptr_item = temp;
		return VO_TRUE;
	}
	return VO_FALSE;
}

VO_S32 VO_MPD_Parser::get_samplerate_index( VO_S32 samplerate ) 
{
	int idx = 0;

	for (idx = 0; idx < 12; idx++) 
	{
		if (samplerate == AAC_SampRateTab_MY[idx])
			return idx;
	}

	return 4; //Default 44100
} 


#if 1
VO_VOID VO_MPD_Parser::analyze_video(VO_S64 Duration_video[1024] ,VO_S64 Timeline_video[1024],CXMLLoad *m_pXmlLoad,void *pLevel_segmentTimeLine,int flag,int group_index,VO_U32 period_index)
{
	VO_S64 i  = 0;
	m_count_video = 0;
	m_count_audio = 0;
	void  *pChildNode_temp = NULL;
	m_pXmlLoad->GetFirstChild(pLevel_segmentTimeLine,&pChildNode_temp);
	char * pTagName = NULL;
	VO_U64  d  = 0;
	VO_U64 nRCount = 0;
	VO_U64 nDuration = 0;
	VO_U64 t_M  = 0;//modify
	VO_U64 t_O  = 0;//original
	int nSize = 0;

	while(pChildNode_temp)
	{
		nRCount = 0;

		m_pXmlLoad->GetAttributeValue(pChildNode_temp,(char *)TAG_D,&pTagName,nSize);
		if(pTagName)
		{
			d = _ATO64(pTagName);
		}
		else
			d = 0;
		m_pXmlLoad->GetAttributeValue(pChildNode_temp,(char *)TAG_T,&pTagName,nSize);
		//pTagName = pChildNode_temp->ToElement()->Attribute(TAG_T);
		if(pTagName)
		{
			t_M = _ATO64(pTagName);
			nDuration = 0;
		}
		else
			nDuration = 0;


		t_O = t_M;

		//pTagName = pChildNode_temp->ToElement()->Attribute(TAG_R);
		m_pXmlLoad->GetAttributeValue(pChildNode_temp,(char *)TAG_R,&pTagName,nSize);
		if(pTagName)
		{
			nRCount= _ATO64(pTagName);
			/*if(nRCount==-1)
			nRCount = 10240; */
			if(StrCompare(pTagName,"-1")==0)	
			{
				VOLOGW("the chunk_count is -1");
				nRCount = 1024; 
			}
		}
		if(nRCount==0)
			nRCount =1;
		else
			nRCount +=1;


		if(nRCount> COUNTS_NUMBER/4)
		{
			nRCount = COUNTS_NUMBER/10;
		}
		int *countX; 
		if(flag==1)
			countX = &m_count_video;
		else if(0==flag)
			countX = &m_count_audio;
		else if(2==flag)
			countX = &m_count_subtitle;
		while(nRCount --)
		{
			Seg_item * item_temp= new Seg_item;
			VOLOGR("new Seg_item3 = %p",item_temp);
			memset( item_temp , 0 , sizeof(Seg_item));
			item_temp->duration = d;
			item_temp->nIsVideo = flag;
			item_temp->period_index = period_index;
			item_temp->group_index = group_index;
			item_temp->starttime = t_M;
			item_temp->nOrignalStartNumber = 1;
			item_temp->startnmber = *countX+1;
			Timeline_video[*countX] = item_temp->starttime;
			Duration_video[*countX] = item_temp->duration;
			if(flag ==2)
			{
				add_subtitleItem(item_temp);
			}
			else
			add_fragmentItem(item_temp);
			(*countX) ++;
			m_seg_count++;
			VOLOGR("new Seg_item3 = %p, %ld",item_temp,item_temp->starttime);
			t_M = t_O + (nDuration += d);
		}
		//pChildNode_temp = pChildNode_temp->NextSibling();
		m_pXmlLoad->GetNextSibling(pChildNode_temp,&pChildNode_temp);
	}

}
#endif
VO_VOID VO_MPD_Parser::add_subtitleItem( Seg_item * ptr_item )
{
	if(ptr_item==NULL)
	 return;
	if(subtile_scale ==0)
		subtile_scale =1;
	VO_S64 head_time = -1 ;
	VO_S64 tail_time = -1;
	VO_S64 temp_time = -1 ;
	temp_time =  ptr_item->starttime*1000/subtile_scale;
	ptr_item->ptr_next = NULL;
	if( m_ptr_sub_FI_head == NULL && m_ptr_sub_FI_tail == NULL )
	{
		m_ptr_sub_FI_head = m_ptr_sub_FI_tail = ptr_item;
	}
	else
	{
		tail_time = m_ptr_sub_FI_tail->starttime*1000/subtile_scale;
		head_time = m_ptr_sub_FI_head->starttime*1000/subtile_scale;
		if((ptr_item->group_index == 0 &&temp_time == tail_time))
		{
			m_ptr_sub_FI_tail->ptr_next = ptr_item;
			m_ptr_sub_FI_tail = ptr_item;
		}
		else if( temp_time <head_time)
		{
			ptr_item->ptr_next = m_ptr_sub_FI_head;
			m_ptr_sub_FI_head = ptr_item;
		}
		else if (temp_time>= tail_time)
		{
			m_ptr_sub_FI_tail->ptr_next = ptr_item;
			m_ptr_sub_FI_tail = ptr_item;
		}
		else
		{
			Seg_item * ptr_cur = m_ptr_sub_FI_head;
			VO_S64 cur_time = -1;
			VO_S64 cur_time_next = -1;
			while(ptr_cur->ptr_next)
			{
				cur_time_next = ptr_cur->ptr_next->starttime*1000/subtile_scale;
				cur_time = ptr_cur->starttime*1000/subtile_scale;
				if(temp_time >= cur_time && temp_time < cur_time_next)
				{
					ptr_item->ptr_next = ptr_cur->ptr_next;
					ptr_cur->ptr_next = ptr_item;
					break;
				}
				ptr_cur = ptr_cur->ptr_next;
			}
		}
	}
		
}
VO_VOID VO_MPD_Parser::add_fragmentItem( Seg_item * ptr_item )
{
	if(ptr_item==NULL)
	 return;
	if(video_scale ==0)
	{
		video_scale = 1;	   
	}
	if(audio_scale==0)
	{
		audio_scale = 1;
	}
	VO_S64 temp_time = -1 ;
	VO_S64 head_time = -1 ;
	VO_S64 tail_time = -1;
	if(ptr_item->nIsVideo)
		temp_time = ptr_item->starttime*1000/video_scale;
	else
		temp_time = ptr_item->starttime*1000/audio_scale;
	ptr_item->ptr_next = NULL;
	if( m_ptr_FI_head == NULL && m_ptr_FI_tail == NULL )
	{
		m_ptr_FI_head = m_ptr_FI_tail = ptr_item;
	}
	else
	{
		if(m_ptr_FI_tail->nIsVideo)
			tail_time = m_ptr_FI_tail->starttime*1000/video_scale;
		else
			tail_time =  m_ptr_FI_tail->starttime*1000/audio_scale;
		if(m_ptr_FI_head->nIsVideo)
			head_time = m_ptr_FI_head->starttime*1000/video_scale;
		else
			head_time =  m_ptr_FI_head->starttime*1000/audio_scale;
		if((ptr_item->group_index == 0 &&temp_time == tail_time))
		{
			m_ptr_FI_tail->ptr_next = ptr_item;
			m_ptr_FI_tail = ptr_item;
		}
		else if( temp_time <head_time)
		{
			ptr_item->ptr_next = m_ptr_FI_head;
			m_ptr_FI_head = ptr_item;
		}
		else if (temp_time>= tail_time)
		{
			m_ptr_FI_tail->ptr_next = ptr_item;
			m_ptr_FI_tail = ptr_item;
		}
		else
		{
			Seg_item * ptr_cur = m_ptr_FI_head;
			VO_S64 cur_time = -1;
			VO_S64 cur_time_next = -1;

			while(ptr_cur->ptr_next)
			{
				if(ptr_cur->ptr_next->nIsVideo)
					cur_time_next = ptr_cur->ptr_next->starttime*1000/video_scale;
				else
					cur_time_next = ptr_cur->ptr_next->starttime*1000/audio_scale;
				if(ptr_cur->nIsVideo)
					cur_time = ptr_cur->starttime*1000/video_scale;
				else
					cur_time = ptr_cur->starttime*1000/audio_scale;
				if(temp_time >= cur_time && temp_time < cur_time_next)
				{
					ptr_item->ptr_next = ptr_cur->ptr_next;
					ptr_cur->ptr_next = ptr_item;
					break;
				}
				ptr_cur = ptr_cur->ptr_next;
			}
		}
	}
}
VO_VOID  VO_MPD_Parser::Set_rpe_index(int index,int type)
{
	if(type ==1)
		current_video_rpe = index;
	else
		current_audio_rpe = index;

}
VO_BOOL VO_MPD_Parser::get_segmentItem(Seg_item ** ptr_item,  VO_S64 ini_chunk,int type,VO_U32 period_index)
{
	Seg_item * temp =  m_ptr_FI_head;
	while(m_ptr_FI_head&&m_period_count>1&&temp&&temp->period_index!=period_index)
	{
		temp = temp->ptr_next;

	}
	if(temp==NULL)
	{
		*ptr_item = NULL;
		return VO_FALSE;	
	}
	if(m_manifest->group_count>6&&type==1&&m_period_count==1)
		temp = m_manifest->adaptionSet[6].m_ptr_QL_head->m_ptr_SG_head;

	VO_S64 chunkIndex = ini_chunk;
	VOLOGW("the chunkIndex is %lld",chunkIndex);
	VO_S64 start_time = temp->starttime;
	start_time = 0;
	VO_S64 duration = (type==1)?m_manifest->duration_video:m_manifest->duration_audio;
	if(Duration_video[0]>0)
	{
		if(ini_chunk==0&&type==1)
			m_duration_time_line_video = Duration_video[0];
		if(ini_chunk==0&&type==0)
			m_duration_time_line_audio = Duration_audio[0];
	}
	else
	{
		if(ini_chunk==0&&type==1)
			m_duration_time_line_video = duration;
		if(ini_chunk==0&&type==0)
			m_duration_time_line_audio = duration;
	}

	//	VO_S64 chunk_scale = (type==1)?m_manifest->video_scale:m_manifest->audio_scale;
	/*VO_S64 chunk_scale = (type==1)?m_manifest->m_period[period_index].period_vtimescale:m_manifest->m_period[period_index].period_atimescale;*/
	VO_S64 chunk_scale = 1;
	if(m_ptr_FI_head)
	{
	if(m_ptr_FI_head->nIsVideo)
		chunk_scale = m_manifest->m_period[period_index].period_vtimescale;
	else
		chunk_scale = m_manifest->m_period[period_index].period_atimescale;
	}
#ifdef _USETIMELINE
	VO_S64 time_line_time = (temp->starttime*1000)/chunk_scale;	
#elif defined  _NOTUSETIMELINE
	VO_S64 time_line_time = 0;
#endif
	VOLOGW("time_line_time %lld",time_line_time);

	VO_S64 temp_duration = 0;
	m_time_line_time = time_line_time;
	if(chunk_scale==0)
		chunk_scale=1;
	if(time_line_time<=0)
	{
		time_line_time = 0;
	}

	VO_U64 j = 0;
	int index =(type==1?current_video_rpe:current_audio_rpe);
	while(temp)
	{
		if(temp->nIsVideo == type&&temp->period_index==period_index&&(Is_Template()||temp->representation_index ==index))
		{
			if( j == chunkIndex)
			{

				if(chunkIndex ==0)
				{
					temp_duration = (type==1)? Duration_video[0]: Duration_audio[0];
					temp->live_time = ( start_time+GetAvailbilityTime())*1000+LIVE_NUMBER+time_line_time;
					VO_CHAR live_time[1024];
					timeToSting(temp->live_time,live_time);
					VOLOGW("audio live_time %s",live_time);
					VOLOGW("aduio_index %lld",chunkIndex);
					if(Is_Dynamic()&&temp->bDependForNextTimeLine)
					{
						temp->live_time = (GetAvailbilityTime())*1000+LIVE_NUMBER;
						temp->startnmber = temp->nOrignalStartNumber;
						temp->starttime = temp->startnmber*(duration);
						m_time_line_time = 0;
					}
					if(temp->bDependForNextTimeLine&&!Is_Dynamic())
					{
						temp->starttime = 0;
						temp->startnmber = temp->nOrignalStartNumber;
						VOLOGW("static start_time %lld",temp->starttime);
					}
				}

				else
				{
					temp_duration = (type==1)? Duration_video[chunkIndex-1]: Duration_audio[chunkIndex-1];
					if(temp_duration<=0)
						temp_duration = duration;
					m_duration_time_line_video = Compute_video_duration(chunkIndex);
					m_duration_time_line_audio= Compute_audio_duration(chunkIndex);
					if(type==1)
					{

						temp->live_time = (start_time+GetAvailbilityTime())*1000+time_line_time+(m_duration_time_line_video*1000)/chunk_scale+LIVE_NUMBER;
						VO_CHAR live_time[1024];
						timeToSting(temp->live_time,live_time);
						VOLOGW("video live_time %s",live_time);
						VOLOGW("video_index %lld",chunkIndex);
						VO_U64 Mpd_time = GetAvailbilityTime()*1000;
						VO_CHAR mpd_time[1024];
						timeToSting(Mpd_time,mpd_time);
						VOLOGW("mpd_utc_time %s",mpd_time)
						int dd = 0;
					}
					else if(type ==0)
					{
						temp->live_time =(start_time+GetAvailbilityTime())*1000+time_line_time+(VO_U64)(m_duration_time_line_audio*1000)/chunk_scale+LIVE_NUMBER;
						VO_CHAR live_time[1024];
						timeToSting(temp->live_time,live_time);
						VOLOGW("audio live_time %s",live_time);
						VOLOGW("aduio_index %lld",chunkIndex);
						VO_U64 Mpd_time = GetAvailbilityTime()*1000;
						VO_CHAR mpd_time[1024];
						timeToSting(Mpd_time,mpd_time);
						VOLOGW("mpd_utc_time %s",mpd_time)
						int dd = 0;
						
					}
				}
				*ptr_item = temp;
				return VO_TRUE;
			} 
			else if(temp->bDependForNextTimeLine)
			{
				m_is_depend = temp->bDependForNextTimeLine;
				temp->startnmber = temp->nOrignalStartNumber + chunkIndex;
				if(Is_Dynamic())
				{
					temp->starttime = temp->startnmber*(duration);
					m_time_line_time = 0;
					VOLOGW("the nOrignalStartNumber is %lld",temp->nOrignalStartNumber);
					VOLOGW("the chunkIndex is %lld",chunkIndex);
				} 
				else
				{
					temp->starttime  = start_time+(chunkIndex)*(duration*1000)/chunk_scale;
			    	temp->starttime  = start_time+(chunkIndex)*(duration);
				}
				/// modified 
				temp->live_time =(GetAvailbilityTime())*1000+(chunkIndex*(duration*1000))/chunk_scale+LIVE_NUMBER;
			/*	VO_CHAR live_time[1024];
				timeToSting(temp->live_time,live_time);
				VOLOGW("video live_time %s",live_time);*/
				if(temp->startnmber <= 0)
					temp->startnmber = 0;
				*ptr_item = temp;
				return VO_TRUE;
			}

			else
				j ++;
		}
		temp = temp->ptr_next;

	}

	*ptr_item = NULL;
	return VO_FALSE;
}

VO_BOOL VO_MPD_Parser::get_segmentItem_subtitle(Seg_item ** ptr_item,  VO_U64 ini_chunk,VO_U32 period_index)
{
	Seg_item * temp =  m_ptr_sub_FI_head;
	while(m_ptr_sub_FI_head&&m_period_count>1&&temp&&temp->period_index!=period_index)
	{
		temp = temp->ptr_next;
	}
	if(temp==NULL)
	{
		*ptr_item = NULL;
		return VO_FALSE;	
	}
	VO_U64 chunkIndex = ini_chunk;
	VO_S64 start_time = temp->starttime;
	start_time = 0;
	VO_S64 temp_duration = 0;
	VO_S64 time_line_time = 0;
	VO_S64 duration = m_manifest->duration_subtitle;
	VO_S64 chunk_scale = 1;
	if(m_ptr_sub_FI_head)
	{
		chunk_scale = m_manifest->m_period[period_index].period_subtitle_timescale;
	}
	if(chunk_scale==0)
		chunk_scale=1;
	VO_U64 j = 0;
	int index = 0;
	while(temp)
	{
		if(temp->period_index==period_index&&(Is_Template()||temp->representation_index ==index))
		{
			if( j == chunkIndex)
			{

				if(chunkIndex ==0)
				{
					temp_duration = Duration_subtitle[0];
					temp->live_time = (GetAvailbilityTime()+time_line_time)*1000+LIVE_NUMBER;
					if(Is_Dynamic()&&temp->bDependForNextTimeLine)
					{
						temp->live_time = (GetAvailbilityTime())*1000+LIVE_NUMBER;
						temp->startnmber = temp->nOrignalStartNumber;
						temp->starttime = temp->startnmber;
						m_time_line_time = 0;
					}
					if(temp->bDependForNextTimeLine&&!Is_Dynamic())
					{
						temp->starttime = 0;
						temp->startnmber = temp->nOrignalStartNumber;
						VOLOGW("static start_time %lld",temp->starttime);
					}
				}

				else
				{
					temp_duration =  Duration_subtitle[chunkIndex-1];
					if(temp_duration<=0)
						temp_duration = duration;
					m_duration_time_line_subtitle = Compute_video_duration(chunkIndex);
					temp->live_time = (start_time+GetAvailbilityTime())*1000+time_line_time+(m_duration_time_line_subtitle*1000)/chunk_scale+LIVE_NUMBER;
				}
				*ptr_item = temp;
				return VO_TRUE;
			} 
			else if(temp->bDependForNextTimeLine)
			{
				m_is_depend = temp->bDependForNextTimeLine;
				temp->startnmber = temp->nOrignalStartNumber + chunkIndex;
				if(Is_Dynamic())
				{
					temp->starttime = temp->startnmber;
				} 
				else
					temp->starttime  = start_time+(chunkIndex)*(duration);
				temp->live_time =(GetAvailbilityTime())*1000+(chunkIndex*(duration*1000))/chunk_scale+LIVE_NUMBER;
				if(temp->startnmber <= 0)
					temp->startnmber = 0;
				*ptr_item = temp;
				return VO_TRUE;
			}

			else
				j ++;
		}
		temp = temp->ptr_next;
	}
	*ptr_item = NULL;
	return VO_FALSE;
}
VO_U64 VO_MPD_Parser::Compute_video_duration(VO_U64 chunk_index)
{
	VO_U64 j = 0;
	VO_U64 m_duration = 0;
	while(j<chunk_index)
	{
		m_duration+= Duration_video[j];
		j++;
	}
	return m_duration;

}

VO_U64 VO_MPD_Parser::Compute_audio_duration(VO_U64 chunk_index)
{
	VO_U64 j = 0;
	VO_U64 m_duration = 0;
	while(j<chunk_index)
	{
		m_duration+= Duration_audio[j];
		j++;
	}
	return m_duration;

}
VO_U32 VO_MPD_Parser::SetLibOp(VO_LIB_OPERATOR* libop)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	VOLOGR("SetLibOp");

	if(!libop){
		return nResult;
	}

	if(m_pXmlLoad){
		m_pXmlLoad->SetLibOperator(libop);
		nResult = VO_RET_SOURCE2_OK;
	}
	//m_pXmlLoad->OpenParser();
	return nResult;
}
VO_U32 VO_MPD_Parser::SetWorkPath(VO_TCHAR * pWorkPath)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	VOLOGR("SetWorkPath");

	if(!pWorkPath){
		return nResult;
	}

	if(m_pXmlLoad){
		m_pXmlLoad->SetWorkPath(pWorkPath);
		nResult = VO_RET_SOURCE2_OK;
	}
	return nResult;
}
#if 1
VO_BOOL  VO_MPD_Parser::parse_manifest2(VO_CHAR* url)
{
	m_period_count = 0;
	m_seg_count = 0;
	memset(m_manifest, 0, sizeof(MpdStreamingMedia));
	VO_U32 *group_count       = &m_manifest->group_count;

	Period  *pPeriod= new Period[PERIOD_MAXNUM];
	ApaptionSet *adaption_set = new ApaptionSet[ADAPTIONSET_MAXNUM];
	for(int i =0 ;i< PERIOD_MAXNUM;i++)
	{
		memset(&pPeriod[i],0x00,sizeof(Period));
	}
	for(int i =0 ;i< ADAPTIONSET_MAXNUM;i++)
	{
		memset(&adaption_set[i],0x00,sizeof(ApaptionSet));
	}
	if (m_pXmlLoad->LoadXML((char*)m_file_content,(int) m_file_content_size, voXML_FLAG_SOURCE_BUFFER)!=VO_ERR_NONE)
	{
		VOLOGR("Cannot Load manifest file on parsing!" );
		return VO_FALSE;
	}
	VO_VOID * pNode = NULL;
	VO_VOID * pLevel_repre = NULL;
	m_pXmlLoad->GetFirstChild(NULL,&pNode);
	if (!pNode) 
		return VO_FALSE;
	mpd_tag.Delete();
	mpd_tag.Init(m_pXmlLoad,pNode);
	if(mpd_tag.GetProfile() == 0||mpd_tag.GetProfile() ==1)
	{ 
		m_manifest->islive = VO_FALSE;
	}
	else if(mpd_tag.GetProfile()==2)
	{
		m_manifest->islive = VO_TRUE;
	}
	else if(mpd_tag.GetProfile()==3)
	{
		VOLOGI("Unknown Flag mpd_tag.GetProfile() == 3");
	}
	else if(mpd_tag.GetProfile()==4||mpd_tag.GetProfile()==5)
	{
		m_is_ts = VO_TRUE;
	}

	m_update_period =  mpd_tag.GetUpdatePeriod();
	m_manifest->mediaPresentationDuration = mpd_tag.GetDuration();
	m_read_time = mpd_tag.GetStartTime();

	VO_VOID *pChildNode = NULL;
	m_pXmlLoad->GetFirstChild(pNode,&pChildNode);
	if(pChildNode==NULL)
		return VO_FALSE;

	VO_S32 period_start_pre = -1;
	VO_BOOL bSegmentTimeline = VO_FALSE;

	while(pChildNode)
	{
		char* pName_1 = NULL;
		int size;
		FlushTAGList();
		m_pXmlLoad->GetTagName(pChildNode,&pName_1,size);
		Period *p_period = &pPeriod[m_manifest->period_count];
		if(StrCompare(pName_1,TAG_DASH_BASEURL_2)==0)
		{
			VOLOGW("TAG_DASH_BASEURL_2" );
			m_base_url_el.Init(m_pXmlLoad,pChildNode);
			strcpy(mpd_base_url,m_base_url_el.GetUrl());
		}
		else if(StrCompare(pName_1,"ProgramInformation")==0)
		{ 
			m_program.Init(m_pXmlLoad,pChildNode);
		}
		else if(StrCompare(pName_1,"Location")==0)
		{
			VOLOGW("Location" );
		}
		else if(StrCompare(pName_1,"Metrics")==0)
		{
			m_metrics.Init(m_pXmlLoad,pChildNode);

		}
		else if(StrCompare(pName_1,TAG_DASH_STREAMING_Period)==0||StrCompare(pName_1,TAG_DASH_STREAMING_Period_G1)==0)
		{
			VOLOGR("TAG_DASH_STREAMING_Period" );
			m_period_tag.Init(m_pXmlLoad,pChildNode);
			if(m_period_tag.GetDuration())
			{
				pPeriod[m_period_count].period_init_duration = m_period_tag.GetDuration();
			}
			if(m_period_tag.GetStart()!=-1)
			{
				pPeriod[m_period_count].period_start = m_period_tag.GetStart();
			}
			else
			{
				if(m_period_count==0)
				{
					pPeriod[m_period_count].period_start = 0;
				}
				else
				{
					pPeriod[m_period_count].period_start = pPeriod[m_period_count-1].period_start+ pPeriod[m_period_count-1].period_init_duration;
				}

			}
			if(period_start_pre==pPeriod[m_period_count].period_start)
			{
				m_pXmlLoad->GetNextSibling(pChildNode,&pChildNode);
				continue;
			} 
			period_start_pre = pPeriod[m_period_count].period_start;
			p_period->peroid_index = m_period_count;
			pPeriod[m_period_count].peroid_index = m_period_count;
			m_period_count++;
			(m_manifest->period_count)++;  
		}
		void *pLevel_group;
		m_pXmlLoad->GetFirstChild(pChildNode,&pLevel_group);
		while(pLevel_group)
		{
			char* pGroup_Name = NULL;
			m_pXmlLoad->GetTagName(pLevel_group,&pGroup_Name,size);
			ApaptionSet *pAdaptionSet= &adaption_set[(*group_count)];
			pAdaptionSet->period_index = m_period_count-1;
			if(StrCompare(pGroup_Name,TAG_DASH_BASEURL_2)==0)
			{
				m_base_url_el.Init(m_pXmlLoad,pLevel_group);
				strcpy(mpd_base_url,m_base_url_el.GetUrl());
				strcpy(p_period->period_base_url,m_base_url_el.GetUrl());
			}
			else if(0==StrCompare(pGroup_Name,"Title"))
			{
				//	pGroup_Name =  pLevel_group->ToElement()->Attribute("moreInformationURL");
			}
			else if(0==StrCompare(pGroup_Name,"Video AdaptationSet"))
			{
				m_pXmlLoad->GetFirstChild(pLevel_group,&pLevel_group);
				break;
			}
			else if(0==StrCompare(pGroup_Name,"Audio AdaptationSet"))
			{
				m_pXmlLoad->GetFirstChild(pLevel_group,&pLevel_group);
				m_pXmlLoad->GetTagName(pLevel_group,&pGroup_Name,size);
				break;
			}
			else if(0==StrCompare(pGroup_Name,TAG_DASH_STREAMING_Group)||0==StrCompare(pGroup_Name, TAG_SIMPLE_ADAPTIONSET)||0==StrCompare(pGroup_Name, TAG_SIMPLE_ADAPTIONSET_G1)||0==StrCompare(pGroup_Name, TAG_DASH_STREAMING_Group_G1)||0==StrCompare(pGroup_Name, "ContentComponent"))
			{
				VOLOGR("TAG_DASH_STREAMING_Group" );
				int ts_component = 0;
				int ini_number = 0;
				VO_CHAR	  m_base_url[256];
				m_base_url[0]='\0';
				m_grouptag.Init(m_pXmlLoad,pLevel_group);
				if(m_grouptag.Get_Period_Duration()!=0)
				{
					m_duration = m_grouptag.Get_Period_Duration();
				}
				m_is_muxed = m_grouptag.GetTrackCount()==2?VO_TRUE:VO_FALSE;
				pAdaptionSet->nTrackType = m_grouptag.GetTrackType();
				m_is_ttml = m_grouptag.IS_TTML();
				pAdaptionSet->bSegmentTimeline = VO_FALSE;
				if(strlen(m_grouptag.GetLang())!=0)
				{
					strcpy(pAdaptionSet->lang,m_grouptag.GetLang());
				}
				pAdaptionSet->nCodecType  = m_grouptag.GetCodecs();
				pAdaptionSet->track_count =  m_grouptag.GetTrackCount();
				m_is_muxed = pAdaptionSet->track_count ==2?VO_TRUE:VO_FALSE;
				pAdaptionSet->group_index = *group_count;
				(*group_count) ++;
				group_number = *group_count;
				m_pXmlLoad->GetFirstChild(pLevel_group,&pLevel_repre);
				VO_CHAR   pval_media[256];
				VO_CHAR   pval_inia[256];
				memset(pval_inia,0x00,256*sizeof(VO_CHAR));
				while(pLevel_repre)
				{
					char * rpe_name = NULL;
					m_pXmlLoad->GetTagName(pLevel_repre,&rpe_name,size);
					if(StrCompare(rpe_name,TAG_DASH_BASEURL_2)==0)
					{
						VO_CHAR* attriValue	= NULL;
						if( m_pXmlLoad->GetTagValue(pLevel_repre,&attriValue,size) ==VO_ERR_NONE )
						{
							if(size > 0 && NULL != attriValue)
							{
								strcpy(m_base_url,attriValue);
							}
						}
					}
					else if(0==StrCompare(rpe_name, "ContentComponent"))
					{
						ts_component++;
						com_tag.Init( m_pXmlLoad,pLevel_repre);
						if(StrCompare(com_tag.GetContentType(),"video/mp4")==0)
						{
							pAdaptionSet->nTrackType = VO_SOURCE_TT_VIDEO;
						} 
						else if(StrCompare(com_tag.GetContentType(),"video")==0)
						{
							pAdaptionSet->nTrackType = VO_SOURCE_TT_VIDEO;
						}
						else if(StrCompare(com_tag.GetContentType(),"video/mp2t")==0)
						{
							m_is_ts = VO_TRUE;
							pAdaptionSet->nTrackType = VO_SOURCE_TT_VIDEO;
						}
						else if(StrCompare(com_tag.GetContentType(),"audio/mp4")==0||0==StrCompare(com_tag.GetContentType(),"audio"))
						{
							strcpy(pAdaptionSet->lang,com_tag.GetLang());
							if(m_is_ts||ts_component==2)
								pAdaptionSet->nTrackType = VO_SOURCE_TT_VIDEO;
							else
								pAdaptionSet->nTrackType = VO_SOURCE_TT_AUDIO;
						}
						else if(StrCompare(com_tag.GetContentType(),"application/ttml+xml"))
						{
							m_pXmlLoad->GetNextSibling(pLevel_group,&pLevel_group);
							continue;
						}
					}
					else if(StrCompare(rpe_name, TAG_DASH_TEMPLATE)==0||StrCompare(rpe_name, TAG_DASH_TEMPLATE_G1)==0)
					{
						ini_number = 1;
						m_seg_temlate = VO_TRUE;
						m_template_tag.Init(m_pXmlLoad,pLevel_repre);	
						ini_number = m_template_tag.GetStart_number();
						if(m_template_tag.GetMediaUrl())
						{
							if(m_base_url[0]!='\0')
							{
								strcpy(pval_media,m_base_url);
								strcat(pval_media,m_template_tag.GetMediaUrl());
							}
							else
							{
								strcpy(pval_media,m_template_tag.GetMediaUrl());
							}
						}
						if(m_template_tag.GetIniaUrl())
						{
							if(m_base_url[0]!='\0')
							{
								strcpy(pval_inia,m_base_url);
								strcat(pval_inia,m_template_tag.GetIniaUrl());
							}
							else
							{
								strcpy(pval_inia,m_template_tag.GetIniaUrl());
							}
						}
						int group_index = *group_count-1;
						void *pLevel_segmentTimeLine = NULL;
						m_pXmlLoad->GetFirstChild(pLevel_repre,&pLevel_segmentTimeLine);
						 if(VO_SOURCE_TT_MAX== pAdaptionSet->nTrackType) 
						 {
							 void * rpe = NULL;
							 m_pXmlLoad->GetNextSibling(pLevel_repre,&rpe);
							 Repre_Tag tag; 
							 tag.Delete();
							 if(rpe)
							 tag.Init(m_pXmlLoad,rpe);
							 pAdaptionSet->nTrackType = tag.GetTrackType();
						 }
						if(pAdaptionSet->nTrackType == VO_SOURCE_TT_VIDEO)
						{
							m_manifest->timeScale  = m_template_tag.GetTimeScale();
							video_scale = m_manifest->timeScale;
							if(audio_scale==0)
								audio_scale = video_scale;
							if(audio_scale==0)
								audio_scale =1;
							if(video_scale==0)
								video_scale = 1;
							p_period->period_atimescale = audio_scale;
							p_period->period_vtimescale = video_scale;
							m_manifest->duration_video = m_template_tag.GetDuration();
							p_period->duration_video = m_manifest->duration_video;
							if(m_template_tag.GetStart_number()!=-1)
							{
								if(m_manifest->duration_video==0)
									m_seg_count = 0;
								else
								{
									m_seg_count = m_manifest->mediaPresentationDuration/ m_manifest->duration_video;
									if(m_seg_count==0)
									{
										m_seg_count =200;
									}
								}
								ini_number  = m_template_tag.GetStart_number();

							}

							if(pLevel_segmentTimeLine)
							{
								char * pValue = NULL;
								int nSize = 0;

								m_pXmlLoad->GetTagName(pLevel_segmentTimeLine,&pValue,nSize);
								if(StrCompare(pValue, TAG_DASH_TIMELINE) == 0)
								{
									pAdaptionSet->bSegmentTimeline = VO_TRUE;
									if(video_flag ==0)
									{
										analyze_video(Duration_video,Timeline_video,m_pXmlLoad,pLevel_segmentTimeLine,1,group_index,m_period_count-1);
										p_period->duration_video = Duration_video[0];
									}
								}
							}
						}
						else if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_AUDIO)
						{
							m_manifest->timeScale  = m_template_tag.GetTimeScale();
							audio_scale = m_manifest->timeScale;
							if(audio_scale==0)
								audio_scale =1;
							m_seg_count = 0;
							p_period->period_atimescale = audio_scale;
							m_manifest->duration_audio = m_template_tag.GetDuration();
							if(m_template_tag.GetStart_number()!=-1)
							{
								if(m_manifest->duration_audio==0)
									m_seg_count = 0;
								else
								{
									if(m_manifest->duration_audio!=0)
										m_seg_count = m_manifest->mediaPresentationDuration/ m_manifest->duration_audio;
									else
										m_seg_count = m_manifest->mediaPresentationDuration/ m_manifest->duration_video;
									if(m_seg_count==0)
									{
										m_seg_count =200;
									}

								}
								ini_number = m_template_tag.GetStart_number();
							}
							else
								m_seg_count = 0;

							if(pLevel_segmentTimeLine)
							{
								char * pValue = NULL;
								int nSize = 0;
								m_pXmlLoad->GetTagName(pLevel_segmentTimeLine,&pValue,nSize);
								if(StrCompare(pValue, TAG_DASH_TIMELINE) == 0)
								{
									pAdaptionSet->bSegmentTimeline = VO_TRUE;
									if(audio_flag ==0)
										analyze_video(Duration_audio,Timeline_audio,m_pXmlLoad,pLevel_segmentTimeLine,0,group_index,m_period_count-1);
								}
							}

						} //pAdaptionSet->nTrackType ==VO_SOURCE_TT_AUDIO
						else if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_SUBTITLE)
						{
								m_manifest->timeScale  = m_template_tag.GetTimeScale();
								subtile_scale = m_manifest->timeScale;
								if(subtile_scale==0)
									subtile_scale = 1;
								p_period->period_subtitle_timescale = subtile_scale;
								m_manifest->duration_subtitle = m_template_tag.GetDuration();
								p_period->duration_subtitle = m_manifest->duration_subtitle;
								if(m_template_tag.GetStart_number()!=-1)
								{
									if(m_manifest->duration_subtitle==0)
										m_seg_count = 0;
									else
									{
										m_seg_count = m_manifest->mediaPresentationDuration/ m_manifest->duration_video;
										if(m_seg_count==0)
										{
											m_seg_count =200;
										}
								}
								ini_number = m_template_tag.GetStart_number();
							}
							else
								m_seg_count = 0;


							if(pLevel_segmentTimeLine)
							{
								char * pValue = NULL;
								int nSize = 0;
								m_pXmlLoad->GetTagName(pLevel_segmentTimeLine,&pValue,nSize);
								if(StrCompare(pValue, TAG_DASH_TIMELINE) == 0)
								{
									pAdaptionSet->bSegmentTimeline = VO_TRUE;
									if(subtitle_flag ==0)
										analyze_video(Duration_subtitle,Timeline_subtile,m_pXmlLoad,pLevel_segmentTimeLine,2,group_index,m_period_count-1);
								}
							}
						}
						m_seg_temlate = VO_TRUE;
						if(!pAdaptionSet->bSegmentTimeline)
						{
							Seg_item * item = new Seg_item;
							memset( item , 0 , sizeof(Seg_item));
							item->starttime =  0; 
							item->startnmber = item->nOrignalStartNumber = ini_number;
							item->nIsVideo =  pAdaptionSet->nTrackType ==VO_SOURCE_TT_AUDIO ? 0 :(pAdaptionSet->nTrackType ==VO_SOURCE_TT_VIDEO? 1:0);
							item->duration =  pAdaptionSet->nTrackType ==VO_SOURCE_TT_AUDIO?m_manifest->duration_audio:m_manifest->duration_video;
							if(pAdaptionSet->nTrackType == VO_SOURCE_TT_SUBTITLE)
							{
								item->nIsVideo= 2;
								m_count_subtitle++;
								item->duration = m_manifest->duration_subtitle;
							}
							item->group_index = group_index;
							item->period_index = m_period_count-1;
							item->bDependForNextTimeLine = VO_TRUE;
							if(item->nIsVideo==1)
								m_count_video++;
							else if(item->nIsVideo==0)
							{
								m_count_audio++;
							}
							if(m_seg_count!=0&&item->nIsVideo!=2)
								add_fragmentItem(item);
							if(item->nIsVideo==2&&m_seg_count!=0)
								add_subtitleItem(item);
						}

					} //(StrCompare(rpe_name, TAG_DASH_TEMPLATE)==0
					else if(StrCompare(rpe_name, "ok")==0)
					{
						;//pLevel_repre = pLevel_repre->NextSibling();
					}
					else if(StrCompare(rpe_name, TAG_DASH_STREAMING_Representation)==0||StrCompare(rpe_name, TAG_DASH_STREAMING_Representation_G1)==0)
					{
						bSegmentTimeline = pAdaptionSet->bSegmentTimeline;
						Representation *pRepresentation=new Representation;
						memset( pRepresentation , 0 , sizeof(Representation));
						m_rpe_tag.Init(m_pXmlLoad,pLevel_repre);
						VO_CHAR	m_base_url_temp[256] = {0};
						void * p_rpe_base_url = NULL;
						int nSize = 0;
						m_pXmlLoad->GetFirstChild(pLevel_repre,&p_rpe_base_url);
						char * fist_child_tag_name = NULL;
						if(p_rpe_base_url)
						{
							m_pXmlLoad->GetTagName(p_rpe_base_url,&fist_child_tag_name,nSize);
							if(0==StrCompare(fist_child_tag_name,"BaseURL"))
							{
								m_base_url_el.Init(m_pXmlLoad,p_rpe_base_url);
								strcpy(pval_inia,m_base_url_el.GetUrl());
							}
						}
						pRepresentation->track_count = m_rpe_tag.GetTrackCount();
						/*add for muxed file*/
						if(!m_is_muxed)
							m_is_muxed = pRepresentation->track_count ==2?VO_TRUE:VO_FALSE;
						/*end*/
						pRepresentation->group_index = *group_count-1;
						memcpy(pRepresentation->mediaUrl,pval_media,strlen(pval_media));
						memcpy(pRepresentation->initalUrl,pval_inia,strlen(pval_inia));
						if(m_rpe_tag.GetTrackType() <10)
							pAdaptionSet->nTrackType = m_rpe_tag.GetTrackType();
						pRepresentation->track_count = m_rpe_tag.GetTrackCount();
						if(!m_is_ts)
							m_is_ts = m_rpe_tag.Is_TS();
						if(m_rpe_tag.GetID())
						{
							strcpy(pRepresentation->id,m_rpe_tag.GetID());
						}
						pRepresentation->bitrate = m_rpe_tag.GetBandWidth();
						if(pAdaptionSet->nTrackType == VO_SOURCE_TT_VIDEO)
						{
							pRepresentation->video_QL.Width  = m_rpe_tag.GetWidth();
							pRepresentation->video_QL.Height  = m_rpe_tag.GetHeight();
							addRepresentationLevel(pAdaptionSet,*group_count-1,pRepresentation); 
							if(m_seg_temlate)
							{
								int i = 0;
								VO_U32  orig_number= ini_number;
								while(i<(m_count_video))
								{
									Seg_item * item = new Seg_item;
									VOLOGR("new Seg_item = %p",item);
									memset(item,0,sizeof(Seg_item));
									item->duration =  Duration_video[i]<0?m_manifest->duration_video: Duration_video[i];
									item->starttime =item->duration*i;
									item->representation_index = pAdaptionSet->rpe_number;
									item->nIsVideo  =1;
									item->startnmber  = ini_number;
									item->nOrignalStartNumber = orig_number;
									item->period_index = m_period_count-1;
									ini_number++;
									add_segmentItem(pRepresentation,item);				
									i++;
								}
							}
						} //pAdaptionSet->nTrackType == VO_SOURCE_TT_VIDEO
						else if(pAdaptionSet->nTrackType == VO_SOURCE_TT_AUDIO)
						{
							pAdaptionSet->audio_QL.bitrate =  m_rpe_tag.GetBandWidth();
							addRepresentationLevel(pAdaptionSet,*group_count-1,pRepresentation);

							if(m_seg_temlate)
							{
								int i = 0; 
								VO_U32  orig_number= ini_number;

								while(i<m_count_audio)
								{
									Seg_item * item = new Seg_item;
									VOLOGR("new Seg_item2 = %p",item);
									memset(item,0,sizeof(Seg_item));
									//item->starttime = Timeline_audio[i];
									item->duration = Duration_audio[i]<0?m_manifest->duration_audio: Duration_audio[i];
									item->starttime =item->duration*i;
									item->representation_index = pAdaptionSet->rpe_number;
									item->startnmber  = ini_number;
									item->nOrignalStartNumber = orig_number;
									item->nIsVideo  =0;
									item->period_index = m_period_count-1;
									add_segmentItem(pRepresentation,item);
									i++;
								}
							}
						}//endof if(pAdaptionSet->nTrackType == VO_SOURCE_TT_AUDIO)
						else if(pAdaptionSet->nTrackType == VO_SOURCE_TT_SUBTITLE)
						{
							addRepresentationLevel(pAdaptionSet,*group_count-1,pRepresentation);
							if(m_seg_temlate)
							{
								int i = 0; 
								VO_U32  orig_number= ini_number;
								while(i<m_count_subtitle)
								{
									Seg_item * item = new Seg_item;
									VOLOGR("new Seg_item2 = %p",item);
									memset(item,0,sizeof(Seg_item));
									//item->starttime = Timeline_audio[i];
									item->duration = Duration_subtitle[i]<=0?m_manifest->duration_subtitle: Duration_subtitle[i];
									item->starttime =item->duration*i;
									item->representation_index = pAdaptionSet->rpe_number;
									item->startnmber  = ini_number;
									item->nOrignalStartNumber = orig_number;
									item->nIsVideo  =2;
									item->period_index = m_period_count-1;
									add_segmentItem(pRepresentation,item);
									i++;
								}
							}
						}
						void*  pLevel_segment = NULL;
						m_pXmlLoad->GetFirstChild(pLevel_repre,&pLevel_segment);
						int  seg_index = 0;
						while(pLevel_segment)
						{
							char * pTagName = NULL;
							int   nSize = 0;
							m_pXmlLoad->GetTagName(pLevel_segment,&pTagName,nSize);
							if(StrCompare(pTagName,TAG_DASH_TEMPLATE)==0)
							{ 
								m_seg_temlate = VO_TRUE;
								int ini_number = 1;
								m_template_tag.Init(m_pXmlLoad,pLevel_segment);
								m_time_offset = m_template_tag.Get_Time_Offset();
								pRepresentation->m_time_offset = m_time_offset;
								if(m_template_tag.GetMediaUrl())
								{
									if(m_base_url[0]!='\0')
									{
										strcpy(pval_media,m_base_url);
										strcat(pval_media,m_template_tag.GetMediaUrl());
									}
									else
									{
										strcpy(pval_media,m_template_tag.GetMediaUrl());
									}
								}
								if(m_template_tag.GetIniaUrl())
								{
									if(m_base_url[0]!='\0')
									{
										strcpy(pval_inia,m_base_url);
										strcat(pval_inia,m_template_tag.GetIniaUrl());
									}
									else
									{
										strcpy(pval_inia,m_template_tag.GetIniaUrl());
									}
								}
								strcpy(pRepresentation->initalUrl,pval_inia);
								int length_ini = strlen(pRepresentation->initalUrl);
								pRepresentation->mediaUrl[length_ini] = '\0';
								strcpy(pRepresentation->mediaUrl,pval_media);
								void *p_child = NULL;
								int group_index = *group_count-1;
								m_pXmlLoad->GetFirstChild(pLevel_segment,&p_child);
								if(pAdaptionSet->nTrackType == VO_SOURCE_TT_VIDEO)
								{
									m_template_tag.Init(m_pXmlLoad,pLevel_segment);;
									video_scale = m_template_tag.GetTimeScale();
									if(audio_scale==0)
										audio_scale = video_scale;
									p_period->period_atimescale = audio_scale;
									p_period->period_vtimescale = video_scale;
									// m_manifest->duration_video = m_template_tag->GetDuration();
									m_manifest->duration_video = m_template_tag.GetDuration();
									p_period->duration_video = m_manifest->duration_video;
									if(m_template_tag.GetStart_number())
									{
										if(m_manifest->duration_video==0)
											m_seg_count = 0;
										else
										{
											m_seg_count = m_manifest->mediaPresentationDuration/ m_manifest->duration_video;
											if(m_seg_count==0)
											{
												m_seg_count =300;
											}
										}
										ini_number  = m_template_tag.GetStart_number();
										/*
										add for start_number has been inia
										*/
										if(ini_number==0)
											ini_number=1;
										/*end*/
									}
									if(p_child)
									{
										char * pValue = NULL;
										int nSize = 0;
										m_pXmlLoad->GetTagName(p_child,&pValue,nSize);
										if(StrCompare(pValue, TAG_DASH_TIMELINE)==0)
										{
											bSegmentTimeline = VO_TRUE;
											if(video_flag ==0)
												analyze_video(Duration_video,Timeline_video,m_pXmlLoad,p_child,1,group_index,m_period_count-1);
										}
									}
									//add for the representation
								}
								else if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_AUDIO)
								{
									audio_scale =  m_template_tag.GetTimeScale();
									p_period->period_atimescale = audio_scale;
									m_manifest->duration_audio = m_template_tag.GetDuration();
									p_period->duration_audio = m_manifest->duration_audio;

									if(m_template_tag.GetStart_number())
									{
										if(m_manifest->duration_audio==0)
											m_seg_count = 0;
										else
										{
											m_seg_count = m_manifest->mediaPresentationDuration/m_manifest->duration_audio;
											if(m_seg_count==0)
											{
												m_seg_count =200;
											}
										}
										ini_number = m_template_tag.GetStart_number();
									}
									if(p_child)
									{
										char * pValue = NULL;
										m_pXmlLoad->GetTagName(p_child,&pValue,nSize);
										if(StrCompare(pValue, TAG_DASH_TIMELINE)==0)
										{
											bSegmentTimeline = VO_TRUE;
											if(audio_flag ==0)
											  analyze_video(Duration_audio,Timeline_audio,m_pXmlLoad,p_child,0,group_index,m_period_count-1);
										}
									}
								}
								if(!bSegmentTimeline)
								{
									Seg_item * item = new Seg_item;
									memset( item , 0 , sizeof(Seg_item));
									item->starttime =  0; 
									item->startnmber = item->nOrignalStartNumber = ini_number;
									item->representation_index = pAdaptionSet->rpe_number;
									item->nIsVideo =  pAdaptionSet->nTrackType ==VO_SOURCE_TT_AUDIO ? 0 :(pAdaptionSet->nTrackType ==VO_SOURCE_TT_VIDEO? 1:0);
									if(m_manifest->duration_audio!=0)
										item->duration = item->nIsVideo? m_manifest->duration_video:m_manifest->duration_audio;
									else
										item->duration = m_manifest->duration_video;
									item->group_index = group_index;
									item->bDependForNextTimeLine = VO_TRUE;
									item->period_index = m_period_count-1;
									add_fragmentItem(item);
									Seg_item * p_item = new Seg_item;
									memset( p_item , 0 , sizeof(Seg_item));
									p_item->starttime =  0; 
									p_item->startnmber =  item->nOrignalStartNumber = ini_number;
									p_item->nIsVideo =  item->nIsVideo;
									p_item->duration =  item->duration;
									p_item->group_index = group_index;
									p_item->bDependForNextTimeLine = VO_TRUE;
									p_item->period_index =  item->period_index;
									add_segmentItem(pRepresentation,p_item);
								}
							}//StrCompare(pTagName,TAG_DASH_TEMPLATE
							if(StrCompare(pTagName,TAG_DASH_HOLDER)==0)
							{
							} 
							else if(StrCompare(pTagName,TAG_DASH_SEGMENTBASE)==0)
							{
								void * temp = NULL;
								char * attriValue =  NULL;
								VO_CHAR m_index_Range[256];
								VO_BOOL m_index_Range_exact = VO_FALSE;
								memset(m_index_Range, 0x00, sizeof(m_index_Range));
								if(m_pXmlLoad->GetAttributeValue(pLevel_segment,(char*)(SEG_DASH_INDEX_RANGE),&attriValue,nSize) == VO_ERR_NONE)
								{
									if(attriValue)
									{
										strcpy(m_index_Range,attriValue);
									}
								}
								if(m_pXmlLoad->GetAttributeValue(pLevel_segment,(char*)(SEG_DASH_INDEX_RANGE_EXACT),&attriValue,nSize) == VO_ERR_NONE)
								{
									if(attriValue)
									{
										if(StrCompare(attriValue,"true") == 0)
											m_index_Range_exact = VO_TRUE;
										else
											m_index_Range_exact = VO_FALSE;
									}
								}
								temp = NULL;
								if(VO_ERR_NONE == m_pXmlLoad->GetChildByName(pLevel_segment,(char *)TAG_DASH_TIMELINE,&temp)
									&& NULL != temp)
								{

								}
								else
								{
									replace(pRepresentation->initalUrl,"$Time$","0");
								}
								m_pXmlLoad->GetFirstChild(pLevel_segment,&temp);
								m_seg_base.Init(m_pXmlLoad,temp);
								VO_CHAR *  m_seg_base_ini_url = NULL;
								m_seg_base_ini_url = m_seg_base.GetIniUrl();
								if(m_seg_base_ini_url[0]!='\0')
								{
									if(m_base_url[0]!='\0')
									{
									//	int len = strlen(m_base_url)+strlen(m_seg_base.GetIniUrl());
									//	memcpy(pval_inia ,m_base_url,len);
										strcpy(pval_inia,m_base_url);
										strcat(pval_inia,attriValue);
									}
									else
									{
										int len = strlen(m_seg_base.GetIniUrl());
										if(len)
										{
											strcpy(pval_inia,m_seg_base.GetIniUrl());
										}
									}
									strcpy(pRepresentation->initalUrl,pval_inia);
								}
								int range_len = strlen(m_seg_base.GetIni_range());
								if(range_len!=0)
									memcpy(pRepresentation->ini_range,m_seg_base.GetIni_range(),range_len);
								if(strlen(m_index_Range))
								{
									Seg_item * temp_index_chunk = new Seg_item;
									memset( temp_index_chunk , 0 , sizeof(Seg_item));
									VO_U32 m_period_index =  m_period_count-1;
									temp_index_chunk->duration =pPeriod[m_period_index].period_init_duration;
									temp_index_chunk->starttime =0;
									temp_index_chunk->is_index_parsed =  VO_FALSE;
									temp_index_chunk->nIsVideo = pAdaptionSet->nTrackType == VO_SOURCE_TT_VIDEO ?1:0;
									temp_index_chunk->representation_index =pRepresentation->rpe_item_index;
									temp_index_chunk->group_index =pRepresentation->group_index;
									temp_index_chunk->period_index = m_period_index;
									temp_index_chunk->is_sigx_index_chunk = VO_TRUE;
									temp_index_chunk->is_index_parsed = VO_FALSE;
									strcpy(temp_index_chunk->item_url,pRepresentation->initalUrl);
									strcpy(temp_index_chunk->range,m_index_Range);
									VO_U32 ini_range = 0;
									VO_U32 size = 0;
									VO_U32 end_range = 0;
									GetChunkRange(temp_index_chunk->range,&ini_range,&end_range,&size);
									if(size!=0)
									{
										temp_index_chunk->first_offset = ini_range;
										temp_index_chunk->size = size;
										if(VO_TRUE ==m_index_Range_exact)
											m_index_first_offset =end_range+INDEX_RANGE_OFFSET;
										else
											m_index_first_offset =end_range+1;
										pRepresentation->chunk_range_offset = m_index_first_offset;
										pRepresentation->m_index_range_exact = m_index_Range_exact;
										VO_U64 head_end_range = ini_range -1;
										pRepresentation->head_range_end = head_end_range;

									}
									strcpy(pRepresentation->index_range,m_index_Range);
									m_is_index_range_video = VO_TRUE;
									m_is_index_range_audio = VO_TRUE;
									if(range_len!=0)
									add_segmentItem(pRepresentation,temp_index_chunk);
									p_period->period_atimescale = 1000;
									p_period->period_vtimescale = 1000;
								
								}
							}//StrCompare(pTagName,TAG_DASH_SEGMENTBASE
							else if(StrCompare(pTagName,TAG_DASH_BASEURL_2)==0)
							{
								int nSize = 0;
								VO_CHAR* pstrAttriValue = NULL; 
								m_pXmlLoad->GetTagName(pLevel_segment,&pstrAttriValue,nSize);
								m_base_url_el.Init(m_pXmlLoad,pLevel_segment);
								char * vaule = m_base_url_el.GetUrl();
								if(vaule!=NULL)
								{
									strcpy(pRepresentation->initalUrl,vaule);
								}
								if(m_base_url[0]!='\0')
								{
								
									strcpy(m_base_url_temp,m_base_url);
									strcat(m_base_url_temp,pstrAttriValue);
								}
							}//StrCompare(pTagName,TAG_DASH_BASEURL_2
							else if(StrCompare(pTagName,TAG_DASH_STREAMING_SegmentInfo)==0||StrCompare(pTagName,TAG_DASH_SEGMENTLIST)==0)
							{
								m_seg_info.Init(m_pXmlLoad,pLevel_segment);
								pRepresentation->duration = m_seg_info.GetDuration();
								if(pAdaptionSet->nTrackType == VO_SOURCE_TT_VIDEO)
								{
									m_manifest->duration_video = pRepresentation->duration;
									m_manifest->timeScale = m_seg_info.GetTimeScale();
									video_scale = m_manifest->timeScale;
									audio_scale = video_scale;
									p_period->period_atimescale = audio_scale;
									p_period->period_vtimescale = video_scale;
									p_period->duration_video = m_manifest->duration_video;
								}
								else if(pAdaptionSet->nTrackType == VO_SOURCE_TT_AUDIO)
								{
									m_manifest->duration_audio = pRepresentation->duration;
									m_manifest->timeScale = m_seg_info.GetTimeScale();
									audio_scale = video_scale;
									p_period->duration_audio = m_manifest->duration_video;
								}
								void * pLevel_segment_1 = NULL;
								m_pXmlLoad->GetFirstChild(pLevel_segment,&pLevel_segment_1);
								char * pTagName_1 = NULL;
								int segment_Index  = 0;
								VO_CHAR	base_url[512];
								memcpy(base_url ,url,strlen(url));
								while(pLevel_segment_1)
								{
									int nSize = 0;
									m_pXmlLoad->GetTagName(pLevel_segment_1,&pTagName_1,nSize);
									if(StrCompare(pTagName_1,TAG_DASH_BASEURL_2)==0)
									{
										VO_CHAR* pstrAttriValue = NULL; 
										m_pXmlLoad->GetAttributeValue(pLevel_segment_1,(char *)TAG_DASH_BASEURL_2,&pstrAttriValue,nSize);
										//attriValue =  pLevel_segment_1->ToElement()->Attribute(TAG_DASH_BASEURL_2);
										if(pstrAttriValue)
										{
											if(strstr(pstrAttriValue , ("http://")))
											{
												strcpy(base_url ,pstrAttriValue);
											}
											else
											{
												if(m_base_url[0]!='\0')
												{
													strcpy(m_base_url_temp ,pstrAttriValue);
												}
												else
												{
													strcpy(base_url ,pstrAttriValue);
												}
												replace(base_url,"manifest.mpd","");
												replace(base_url,"Manifest.mpd","");
												replace(base_url,"vlcManifestVideo.mpd","");
												replace(base_url,"vlcManifestAudio.mpd","");
												replace(base_url,"vlc","");
											}
										}
									}//endof if(StrCompare(pTagName_1,TAG_DASH_BASEURL_2)==0)
									else if(StrCompare(pTagName_1,TAG_DASH_STREAMING_InitialisationSegmentURL)==0||StrCompare(pTagName_1,TAG_DASH_BASEURL)==0||StrCompare(pTagName_1,TAG_DASH_BASEURL_3)==0||StrCompare(pTagName_1,TAG_DASH_BASEURL_BACK)==0)
									{
										VO_CHAR* pstrAttriValue = NULL; 
										m_pXmlLoad->GetAttributeValue(pLevel_segment_1,(char *)TAG_DASH_STREAMING_SOURCEURL,&pstrAttriValue,nSize);
										if(pstrAttriValue)
										{
											if(strstr(pstrAttriValue , ("http://")))
											{
												VOLOGR( "It is HTTP url Streaming!" );
												strcpy(pRepresentation->initalUrl ,pstrAttriValue);
											}
											else
											{ 
												if(m_base_url[0]!='\0')
												{
													strcpy(pRepresentation->initalUrl ,m_base_url_temp);
												}
												else
												{
													strcpy(pRepresentation->initalUrl,pstrAttriValue);
												}
											}
										}
									}//enfof 	else if(StrCompare(pTagName_1,TAG_DASH_STREAMING_InitialisationSegmentURL)==0||StrCompare(pTagName_1,TAG_DASH_BASEURL)==0||StrCompare(pTagName_1,TAG_DASH_BASEURL_3)==0)
									else if(StrCompare(pTagName_1,TAG_DASH_REPRESENTATIONINDEX)==0)
									{
										VOLOGI("it is represention_index")
										video_flag -=1;
										audio_flag -=1;		

									}
									else if(StrCompare(pTagName_1,TAG_DASH_STREAMING_URL)==0||StrCompare(pTagName_1,TAG_DASH_SEGURL)==0)
									{
										VO_CHAR* pstrAttriValue = NULL; 
										Seg_item * ptr_item = new Seg_item;
										memset( ptr_item , 0 , sizeof(Seg_item));
										int group_index = *group_count-1;
										ptr_item->group_index = group_index;
										int nSize  = 0;
										m_pXmlLoad->GetAttributeValue(pLevel_segment_1,(char *)TAG_DASH_MEDIARANGE,&pstrAttriValue,nSize);
										if(pstrAttriValue)
										{ 	
											strcpy(ptr_item->range,pstrAttriValue);
										}
										m_pXmlLoad->GetAttributeValue(pLevel_segment_1,(char *)TAG_DASH_MEDIABASEURL,&pstrAttriValue,nSize);
										// attriValue =  pLevel_segment_1->ToElement()->Attribute(TAG_DASH_MEDIABASEURL);
										if(pstrAttriValue)
										{
											if(strstr(pstrAttriValue , ("http://")))
											{
												VOLOGR( "It is HTTP url Streaming!" );
												strcpy(ptr_item->item_url ,pstrAttriValue);
												ptr_item->starttime = pRepresentation->duration*seg_index;
												seg_index++;
											}
											else if(strstr(pstrAttriValue , (".")))
											{
												if(m_base_url[0]!='\0')
												{
													strcpy(ptr_item->item_url ,m_base_url_temp);
												}
												else
													strcpy(ptr_item->item_url ,pstrAttriValue);

												ptr_item->starttime = pRepresentation->duration*seg_index;
												if(strlen(pRepresentation->initalUrl)==0)
												{
													strcpy(pRepresentation->initalUrl,ptr_item->item_url);
													video_flag++;
													audio_flag++;
												}
												seg_index++;
											}
											else
											{
												int le= strlen(pstrAttriValue);
												const char * length = NULL;
												const char * find_url = "/";
												int index =  Common_Tag::lastIndex(pstrAttriValue,find_url);
												le = le-index-1;
												length = pstrAttriValue+index+1;
#if defined _WIN32
												ptr_item->starttime= _atoi64(length);
#elif defined LINUX
												ptr_item->starttime=  atoll(length);
#endif		 
												strcpy(ptr_item->item_url,pstrAttriValue);
											}
										}

										if(pAdaptionSet->nTrackType == VO_SOURCE_TT_VIDEO)
										{
											ptr_item->nIsVideo = 1;
										}
										else
											ptr_item->nIsVideo = 0;
										m_pXmlLoad->GetAttributeValue(pLevel_segment_1,(char *)TAG_DASH_STREAMING_SOURCEURL,&pstrAttriValue,nSize);
										// attriValue =  pLevel_segment_1->ToElement()->Attribute(TAG_DASH_STREAMING_SOURCEURL);
										if(pstrAttriValue)
										{
											if(strstr(pstrAttriValue , ("http://")))
											{
												VOLOGR( "It is HTTP url Streaming!" );
												strcpy(ptr_item->item_url ,pstrAttriValue);
												ptr_item->starttime = pRepresentation->duration*seg_index;
												seg_index++;
											}
											else if(strstr(pstrAttriValue , (".")))
											{
												strcpy(ptr_item->item_url ,pstrAttriValue);
												/*strcat(ptr_item->item_url,pstrAttriValue);*/
												ptr_item->starttime = pRepresentation->duration*seg_index;
												seg_index++;
											}
											else
											{
												int le= strlen(pstrAttriValue);
												const char * length = NULL;
												const char * find_url = "/";
												int index = Common_Tag::lastIndex(pstrAttriValue,find_url);
												le = le-index-1;
												length = pstrAttriValue+index+1;
												ptr_item->starttime= _ATO64(length); 
												strcpy(ptr_item->item_url,url);
												strcat(ptr_item->item_url,pstrAttriValue);
											}
										}  
										ptr_item->duration = pRepresentation->duration;
										ptr_item->segment_index = segment_Index;//start from
										ptr_item->period_index = m_period_count-1;

										if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_VIDEO )
										{
										//	if(video_flag ==0&&pRepresentation->rpe_item_index ==0)
											if(video_flag ==0)
											{
												Seg_item * temp = new Seg_item;
												memset( temp , 0 , sizeof(Seg_item));
												temp->duration =ptr_item->duration ;
												temp->starttime = ptr_item->starttime;
												temp->nIsVideo = 1;
												temp->representation_index =pRepresentation->rpe_item_index;
												temp->group_index =ptr_item->group_index;
												temp->period_index = ptr_item->period_index;
												memcpy(temp->item_url,ptr_item->item_url,strlen(ptr_item->item_url));
												memcpy(temp->range,ptr_item->range,strlen(ptr_item->range));
												m_manifest->duration_video = temp->duration;
												if(pRepresentation->track_count!=2||ts_component==2)
													add_fragmentItem(temp); 
											}
										}
										else if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_AUDIO&&pRepresentation->rpe_item_index ==0)
										{
											if(audio_flag==0)
											{
												Seg_item * temp = new Seg_item;
												memset( temp , 0 , sizeof(Seg_item));
												temp->duration =ptr_item->duration ;
												temp->starttime = ptr_item->starttime;
												temp->nIsVideo = 0;
												temp->group_index =ptr_item->group_index;
												temp->period_index = ptr_item->period_index;
												memcpy(temp->item_url,ptr_item->item_url,strlen(ptr_item->item_url));
												m_manifest->duration_audio = temp->duration;
												if(pRepresentation->track_count!=2)
													add_fragmentItem(temp); 
											}
										}
										add_segmentItem(pRepresentation,ptr_item);
										segment_Index++;
									}//	endof else if(StrCompare(pTagName_1,TAG_DASH_STREAMING_URL)==0||StrCompare(pTagName_1,TAG_DASH_SEGURL)==0)
									m_pXmlLoad->GetNextSibling(pLevel_segment_1,&pLevel_segment_1);
								}//endof while(pLevel_segment_1) 
							}//endof if(StrCompare(pTagName,TAG_DASH_STREAMING_SegmentInfo)==0||StrCompare(pTagName,TAG_DASH_SEGMENTLIST)==0)
							m_pXmlLoad->GetNextSibling(pLevel_segment,&pLevel_segment);
						}//endof  while(pLevel_segment) 
					} //StrCompare(rpe_name, TAG_DASH_STREAMING_Representation
					m_pXmlLoad->GetNextSibling(pLevel_repre,&pLevel_repre);
				}
			}//endof if(0==StrCompare(pGroup_Name,TAG_DASH_STREAMING_Group)
			if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_VIDEO)
			{
				pAdaptionSet->track_id =-1;
				video_flag++;
			}
			else if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_AUDIO)
				audio_flag++;
			else if(pAdaptionSet->nTrackType  == VO_SOURCE_TT_SUBTITLE )
				subtitle_flag++;
			m_pXmlLoad->GetNextSibling(pLevel_group,&pLevel_group);
		}//while(pLevel_group)
	
		if(m_is_muxed&&m_manifest->group_count!=1)
		{
			m_is_muxed = VO_FALSE;
		}
		p_period->is_muxed = m_is_muxed;
		m_pXmlLoad->GetNextSibling(pChildNode,&pChildNode);
		video_flag =0;
		audio_flag = 0;
		subtitle_flag = 0;
	}//while(pnode)
	VOLOGR("Analyze %d",video_scale);
	Period *periodIndex = new Period[m_manifest->period_count];
	memcpy(periodIndex,pPeriod,sizeof(Period)*m_manifest->period_count);
	m_manifest->m_period = periodIndex;
	if(m_period_count==1)
	{
		if( m_manifest->m_period[0].period_init_duration)
			m_manifest->mediaPresentationDuration = m_manifest->m_period[0].period_init_duration;
		else
			m_manifest->m_period[0].period_init_duration = m_manifest->mediaPresentationDuration;
		if(mpd_tag.GetLiveType()==VO_FALSE)
		m_manifest->m_period[m_period_count-1].period_start = 0;
	}
	
	m_manifest->period_count = m_period_count;
	ApaptionSet *streamIndexc = new ApaptionSet[m_manifest->group_count];
	memcpy(streamIndexc,adaption_set,sizeof(ApaptionSet)*m_manifest->group_count);
	m_manifest->adaptionSet = streamIndexc; 
	m_group_count = m_manifest->group_count;
	m_duration = m_manifest->mediaPresentationDuration;
	for(int i = 0;i<m_manifest->group_count;i++)
	{
		Representation * ql = m_manifest->adaptionSet[i].m_ptr_QL_head;
		while(ql)
		{
			if(m_ptr_FI_head==NULL)
			{
			ql->is_init_data = VO_TRUE;
		
			}
			ql = ql->ptr_next;
		}
		
	}
	if(adaption_set)
	{
		delete[]adaption_set;
		adaption_set = NULL;

	}
	if(pPeriod)
	{
		delete[]pPeriod;
	}
	VOLOGW("parser success"); 
	return VO_TRUE;

}
#endif

#if 0
VO_BOOL  VO_MPD_Parser::parse_manifest2(VO_CHAR* url)
{
	real_count = 0;
	m_seg_count = 0;
	TiXmlDocument doc;
	VO_BOOL bSegmentTimeline = VO_FALSE;
	memset(m_manifest, 0, sizeof(MpdStreamingMedia));
	m_manifest->group_count = 0;
	VO_S32 *group_count       = &m_manifest->group_count;
	ApaptionSet adaption_set[20];
	memset(adaption_set,0x00,sizeof(ApaptionSet) * 20);
	if (!doc.LoadFile((char*)m_file_content, m_file_content_size, TIXML_DEFAULT_ENCODING))
	{
		VOLOGR("Cannot Load manifest file on parsing!" );
		return VO_FALSE;
	}
	TiXmlNode* pNode = NULL;
	/*if(strstr(url,"/verizon.mpd")){
	pNode = doc.FirstChild(TAG_DASH_STREAMING_MEDIA_G1);
	}
	else
	pNode = doc.FirstChild(TAG_DASH_STREAMING_MEDIA);*/
	pNode = doc.FirstChild(TAG_DASH_STREAMING_MEDIA);
	if(pNode==NULL)
		pNode = doc.FirstChild(TAG_DASH_STREAMING_MEDIA_G1);

	/*char * url_temp   = strrchr(url,'/');
	int len = strlen(url)- strlen(url_temp);
	url[len+1] = '\0';
	VOLOGR("Analyze %s",url);*/
	if (!pNode) 
		return VO_FALSE;
	const char* attriValue		= NULL;
	int nVal				= 0; 
	TiXmlNode* pChildNode	= NULL;
	TiXmlNode* pLevel_repre = NULL;
	VO_S64 period_index = 0;
	unknown_flag = 0;
	mpd_tag.Init(pNode);
	m_is_need_update = mpd_tag.GetLiveType();
	if(mpd_tag.GetProfile() == 0||mpd_tag.GetProfile() ==1)
	{ 
		m_is_live = VO_FALSE;
		m_manifest->islive = VO_FALSE;
	}
	else if(mpd_tag.GetProfile()==2)
	{
		m_is_live = VO_TRUE;
		m_manifest->islive = VO_TRUE;
	}
	else if(mpd_tag.GetProfile()==3)
	{
		unknown_flag = 1;
	}
	else if(mpd_tag.GetProfile()==4||mpd_tag.GetProfile()==5)
	{
		m_is_ts = VO_TRUE;
	}
	m_is_need_update = mpd_tag.GetLiveType();
	m_update_period =  mpd_tag.GetUpdatePeriod();
	m_manifest->mediaPresentationDuration = mpd_tag.GetDuration();
	m_read_time = mpd_tag.GetStartTime();
	pChildNode = pNode->FirstChild();
	if(pChildNode==NULL)
		return VO_FALSE;
	while(pChildNode)
	{
		const char* pName_1 = pChildNode->Value();
		if(StrCompare(pName_1,TAG_DASH_BASEURL_2)==0)
		{
			m_base_url_el.Init(pChildNode);

			pChildNode = pChildNode->NextSibling();
			pName_1 = pChildNode->Value();
		}
		if(StrCompare(pName_1,"ProgramInformation")==0)
		{
			m_program.Init(pChildNode);
			pChildNode = pChildNode->NextSibling();
			pName_1 = pChildNode->Value();
		}
		if(StrCompare(pName_1,"Location")==0)
		{
			pChildNode = pChildNode->NextSibling();
		}
		if(StrCompare(pName_1,"Metrics")==0)
		{
			m_metrics.Init(pChildNode);
			pChildNode = pChildNode->NextSibling();
			pName_1 = pChildNode->Value();

		}
		else if(StrCompare(pName_1,TAG_DASH_STREAMING_Period)==0||StrCompare(pName_1,TAG_DASH_STREAMING_Period_G1)==0)
		{
			m_period_tag.Init(pChildNode);
			if(m_period_tag.GetDuration())
			{
				m_manifest->mediaPresentationDuration = m_period_tag.GetDuration();
				m_duration = m_manifest->mediaPresentationDuration;

			}
			m_manifest->peroid_index = period_index;
			period_index++;
			m_period_count = period_index;
		}
		TiXmlNode * pLevel_group = pChildNode->FirstChild();
		while(pLevel_group)
		{
			const char* pGroup_Name = pLevel_group->Value();
			ApaptionSet *pAdaptionSet= &adaption_set[(*group_count)];
			pAdaptionSet->period_index = period_index;
			if(0==StrCompare(pGroup_Name,"Title"))
			{
				attriValue =  pLevel_group->ToElement()->Attribute("moreInformationURL");
				;//pLevel_group = pLevel_group->NextSibling();
			}
			else if(0==StrCompare(pGroup_Name,"Video AdaptationSet"))
			{
				pLevel_group = pLevel_group->FirstChild();
				break;
			}
			else if(0==StrCompare(pGroup_Name,"Audio AdaptationSet"))
			{
				pLevel_group = pLevel_group->FirstChild();
				pGroup_Name = pLevel_group->Value();
				break;
			}
			else if(0==StrCompare(pGroup_Name,TAG_DASH_STREAMING_Group)||0==StrCompare(pGroup_Name, TAG_SIMPLE_ADAPTIONSET)||0==StrCompare(pGroup_Name, TAG_SIMPLE_ADAPTIONSET_G1)||0==StrCompare(pGroup_Name, TAG_DASH_STREAMING_Group_G1)||0==StrCompare(pGroup_Name, "ContentComponent"))
			{
				int ts_component = 0;
				int ini_number = 0;;
				VO_CHAR	  m_base_url[256];
				m_base_url[0]='\0';
				m_grouptag.Init(pLevel_group);
				if(m_grouptag.Get_Period_Duration()!=0)
				{
					m_duration = m_grouptag.Get_Period_Duration();
				}
				m_is_muxed = m_grouptag.GetTrackCount()==2?VO_TRUE:VO_FALSE;
				pAdaptionSet->nTrackType = m_grouptag.GetTrackType();
				memcpy(pAdaptionSet->lang ,m_grouptag.GetLang(),strlen(m_grouptag.GetLang()));
				pAdaptionSet->nCodecType  = m_grouptag.GetCodecs();
				pAdaptionSet->track_count =  m_grouptag.GetTrackCount();
				m_is_muxed = pAdaptionSet->track_count ==2?VO_TRUE:VO_FALSE;
				pAdaptionSet->group_index = *group_count;
				(*group_count) ++;
				group_number = *group_count;
				pLevel_repre= pLevel_group->FirstChild();// 
				VO_CHAR   pval_media[256];
				VO_CHAR   pval_inia[256];
				while(pLevel_repre)
				{ 
					const char * rpe_name = NULL;
					rpe_name = pLevel_repre->Value();
					if(StrCompare(rpe_name,TAG_DASH_BASEURL_2)==0)
					{
						attriValue = pLevel_repre->FirstChild()->Value();
						memcpy(m_base_url,attriValue,strlen(attriValue));
						m_base_url[strlen(attriValue)] = '\0';
					}
					else if(0==StrCompare(rpe_name, "ContentComponent"))
					{
						ts_component++;
						if( pAdaptionSet->group_index ==1)
						{
							//is_TS = VO_TRUE;
						}

						// com_tag =  new contentComponent_Tag();
						// com_tag->Init(pLevel_repre);
						com_tag.Init(pLevel_repre);
						if(StrCompare(com_tag.GetContentType(),"video/mp4")==0)
						{
							pAdaptionSet->nTrackType = VO_SOURCE_TT_VIDEO;

						} 
						else if(StrCompare(com_tag.GetContentType(),"video")==0)
						{
							pAdaptionSet->nTrackType = VO_SOURCE_TT_VIDEO;
						}
						else if(StrCompare(com_tag.GetContentType(),"video/mp2t")==0)
						{
							pAdaptionSet->nTrackType = VO_SOURCE_TT_VIDEO;
						}
						else if(StrCompare(com_tag.GetContentType(),"audio/mp4")==0||0==StrCompare(com_tag.GetContentType(),"audio"))
						{
							memcpy(pAdaptionSet->lang,com_tag.GetLang(),strlen(com_tag.GetLang()));
							//if(m_is_ts) check the murphy
							if(m_is_ts||ts_component==2)
								pAdaptionSet->nTrackType = VO_SOURCE_TT_VIDEO;
							else
								pAdaptionSet->nTrackType = VO_SOURCE_TT_AUDIO;
						}
						else if(StrCompare(com_tag.GetContentType(),"text"))
						{
							pLevel_group = pLevel_group->NextSibling();
							continue;
						}
					}

					else if(StrCompare(rpe_name, TAG_DASH_TEMPLATE)==0||StrCompare(rpe_name, TAG_DASH_TEMPLATE_G1)==0)
					{
						// int ini_number = 1;
						ini_number = 1;
						m_seg_temlate = VO_TRUE;
						m_template_tag.Init(pLevel_repre);	
						ini_number = m_template_tag.GetStart_number();
						if(m_template_tag.GetMediaUrl())
						{
							if(m_base_url[0]!='\0')
							{
								/*	int len = strlen(m_base_url)+strlen(url)+strlen(m_template_tag.GetMediaUrl());
								memcpy(pval_media ,url,strlen(m_base_url)+strlen(url)+strlen(m_template_tag.GetMediaUrl()));
								strcat(pval_media,m_base_url);
								strcat(pval_media,m_template_tag.GetMediaUrl());*/
								int len =  strlen(m_base_url)+strlen(m_template_tag.GetMediaUrl());
								memcpy(pval_media ,m_base_url,strlen(m_base_url)+strlen(m_template_tag.GetMediaUrl()));
								strcat(pval_media,m_template_tag.GetMediaUrl());
								pval_media[len]= '\0';
							}
							else
							{
								//	 int len = strlen(m_base_url)+strlen(url)+strlen(m_template_tag.GetMediaUrl());
								//memcpy(pval_media ,url,strlen(m_template_tag.GetMediaUrl())+strlen(url));
								// strcat(pval_media,m_template_tag.GetMediaUrl()); 
								int len = strlen(m_base_url)+strlen(m_template_tag.GetMediaUrl());
								memcpy(pval_media ,m_template_tag.GetMediaUrl(),strlen(m_template_tag.GetMediaUrl()));
								pval_media[len]= '\0';

							}
						}

						if(m_template_tag.GetIniaUrl())
						{
							if(m_base_url[0]!='\0')
							{
								//	memcpy(pval_inia ,url,strlen(m_base_url)+strlen(url)+strlen(m_template_tag.GetIniaUrl()));
								//	strcat(pval_inia,m_base_url);
								memcpy(pval_inia ,m_base_url,strlen(m_base_url)+strlen(m_template_tag.GetIniaUrl()));
								strcat(pval_inia,m_template_tag.GetIniaUrl());
							}
							else
							{
								//	int len = strlen(m_template_tag.GetIniaUrl())+strlen(url);
								//	memcpy(pval_inia ,url,strlen(m_template_tag.GetIniaUrl())+strlen(url));
								//	strcat(pval_inia,m_template_tag.GetIniaUrl());
								int len = strlen(m_template_tag.GetIniaUrl());
								memcpy(pval_inia ,m_template_tag.GetIniaUrl(),len);
								pval_inia[len]= '\0';
							}
						}
						int group_index = *group_count-1;
						TiXmlNode *pLevel_segmentTimeLine = NULL;
						pLevel_segmentTimeLine= pLevel_repre->FirstChild();
						if(pAdaptionSet->nTrackType == VO_SOURCE_TT_VIDEO)
						{
							m_manifest->timeScale  = m_template_tag.GetTimeScale();
							video_scale = m_manifest->timeScale;
							if(audio_scale==0)
								audio_scale = video_scale;
							m_manifest->duration_video = m_template_tag.GetDuration();
							if(m_template_tag.GetStart_number()!=-1)
							{
								if(m_manifest->duration_video==0)
									m_seg_count = 0;
								else
								{
									m_seg_count = m_manifest->mediaPresentationDuration/ m_manifest->duration_video;
									if(m_seg_count==0)
									{
										m_seg_count =200;

									}
								}
								ini_number  = m_template_tag.GetStart_number();

							}
							if(pLevel_segmentTimeLine)
							{
								const char * pValue = NULL;
								pValue = pLevel_segmentTimeLine->Value();
								if(StrCompare(pValue, TAG_DASH_TIMELINE) == 0)
								{
									pAdaptionSet->bSegmentTimeline = VO_TRUE;
									if(video_flag ==0)
										analyze_video(Duration_video,Timeline_video,pLevel_segmentTimeLine,1,group_index);
								}
							}
						}
						else if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_AUDIO)
						{
							m_manifest->timeScale  = m_template_tag.GetTimeScale();
							audio_scale = m_manifest->timeScale;
							m_seg_count = 0;
							m_manifest->duration_audio = m_template_tag.GetDuration();
							if(m_template_tag.GetStart_number()!=-1)
							{
								if(m_manifest->duration_audio==0)
									m_seg_count = 0;
								else
								{
									m_seg_count = m_manifest->mediaPresentationDuration/ m_manifest->duration_video;
									if(m_seg_count==0)
									{
										m_seg_count =200;
									}

								}
								ini_number = m_template_tag.GetStart_number();
							}
							else
								m_seg_count = 0;


							if(pLevel_segmentTimeLine)
							{
								const char * pValue = NULL;
								pValue = pLevel_segmentTimeLine->Value();
								if(StrCompare(pValue, TAG_DASH_TIMELINE) == 0)
								{
									pAdaptionSet->bSegmentTimeline = VO_TRUE;
									if(audio_flag ==0)
										analyze_video(Duration_audio,Timeline_audio,pLevel_segmentTimeLine,0,group_index);
								}
							}

						}
						m_seg_temlate = VO_TRUE;
						if(!pAdaptionSet->bSegmentTimeline)
						{
							Seg_item * item = new Seg_item;
							memset( item , 0 , sizeof(Seg_item));
							item->starttime =  0; 
							item->startnmber = item->nOrignalStartNumber = ini_number;
							item->nIsVideo =  pAdaptionSet->nTrackType ==VO_SOURCE_TT_AUDIO ? 0 :(pAdaptionSet->nTrackType ==VO_SOURCE_TT_VIDEO? 1:0);
							item->duration =  pAdaptionSet->nTrackType ==VO_SOURCE_TT_AUDIO?m_manifest->duration_audio:m_manifest->duration_video;
							item->group_index = group_index;
							item->period_index = period_index;
							item->bDependForNextTimeLine = VO_TRUE;
							if(item->nIsVideo==1)
								m_count_video++;
							else if(item->nIsVideo==0)
							{
								m_count_audio++;
							}
							add_fragmentItem(item);
						}
						//pLevel_repre = pLevel_repre->NextSibling();
					}
					else if(StrCompare(rpe_name, "ok")==0)
					{
						;//pLevel_repre = pLevel_repre->NextSibling();
					}
					else if(StrCompare(rpe_name, TAG_DASH_STREAMING_Representation)==0||StrCompare(rpe_name, TAG_DASH_STREAMING_Representation_G1)==0)
					{
						bSegmentTimeline = VO_FALSE;
						Representation *pRepresentation=new Representation;
						memset( pRepresentation , 0 , sizeof(Representation));
						m_rpe_tag.Init(pLevel_repre);
						VO_CHAR	m_base_url_temp[256];
						pRepresentation->track_count = m_rpe_tag.GetTrackCount();
						/*add for muxed file*/
						if(!m_is_muxed)
							m_is_muxed = pRepresentation->track_count ==2?VO_TRUE:VO_FALSE;
						/*end*/
						pRepresentation->group_index = *group_count-1;
						memcpy(pRepresentation->mediaUrl,pval_media,strlen(pval_media));
						memcpy(pRepresentation->initalUrl,pval_inia,strlen(pval_inia));
						if(m_rpe_tag.GetTrackType() <10)
							//pAdaptionSet->nTrackType = m_rpe_tag->GetTrackType();
							pAdaptionSet->nTrackType = m_rpe_tag.GetTrackType();
						pRepresentation->track_count = m_rpe_tag.GetTrackCount();
						//pRepresentation->track_count = m_rpe_tag->GetTrackCount();
						/*	if(m_rpe_tag->GetID())
						memcpy(pRepresentation->id,m_rpe_tag->GetID(),strlen(m_rpe_tag->GetID()));
						pRepresentation->bitrate = m_rpe_tag->GetBandWidth();*/
						if(m_rpe_tag.GetID())
						{
							memcpy(pRepresentation->id,m_rpe_tag.GetID(),strlen(m_rpe_tag.GetID()));
						}
						pRepresentation->bitrate = m_rpe_tag.GetBandWidth();
						if(pAdaptionSet->nTrackType == VO_SOURCE_TT_VIDEO)
						{
							//pRepresentation->video_QL.Width  = m_rpe_tag->GetWidth();
							//pRepresentation->video_QL.Height  = m_rpe_tag->GetHeight();
							pRepresentation->video_QL.Width  = m_rpe_tag.GetWidth();
							pRepresentation->video_QL.Height  = m_rpe_tag.GetHeight();
							//if(pRepresentation->track_count!=2)
							addRepresentationLevel(pAdaptionSet,*group_count-1,pRepresentation);
							if(m_seg_temlate)
							{
								int i = 0;
								VO_U32  orig_number= ini_number;
								while(i<(m_count_video))
								{
									Seg_item * item = new Seg_item;
									VOLOGR("new Seg_item = %p",item);
									memset(item,0,sizeof(Seg_item));
									item->duration =  Duration_video[i]<0?m_manifest->duration_video: Duration_video[i];
									item->starttime =item->duration*i;
									item->representation_index = pAdaptionSet->rpe_number;
									item->nIsVideo  =1;
									item->startnmber  = ini_number;
									item->nOrignalStartNumber = orig_number;
									ini_number++;
									add_segmentItem(pRepresentation,item);				
									i++;
								}
							}
						} //video
						else if(pAdaptionSet->nTrackType == VO_SOURCE_TT_AUDIO)
						{
							pAdaptionSet->audio_QL.bitrate =  m_rpe_tag.GetBandWidth();
							addRepresentationLevel(pAdaptionSet,*group_count-1,pRepresentation);
							if(m_seg_temlate)
							{
								int i = 0;
								VO_U32  orig_number= ini_number;

								while(i<m_count_audio)
								{
									Seg_item * item = new Seg_item;
									VOLOGR("new Seg_item2 = %p",item);
									memset(item,0,sizeof(Seg_item));
									//item->starttime = Timeline_audio[i];
									item->duration = Duration_audio[i]<0?m_manifest->duration_audio: Duration_audio[i];
									item->starttime =item->duration*i;
									item->representation_index = pAdaptionSet->rpe_number;
									item->startnmber  = ini_number;
									item->nOrignalStartNumber = orig_number;
									item->nIsVideo  =0;
									add_segmentItem(pRepresentation,item);
									i++;
								}
							}
						}//endof if(pAdaptionSet->nTrackType == VO_SOURCE_TT_VIDEO)


						TiXmlNode*  pLevel_segment = NULL;
						pLevel_segment = pLevel_repre->FirstChild();
						int  seg_index = 0;
						while(pLevel_segment)
						{
							const char * pTagName = NULL;
							pTagName = pLevel_segment->Value();
							if(StrCompare(pTagName,TAG_DASH_TEMPLATE)==0)
							{ 
								m_seg_temlate = VO_TRUE;
								int ini_number = 1;
								m_template_tag.Init(pLevel_segment);
								m_time_offset = m_template_tag.Get_Time_Offset();
								pRepresentation->m_time_offset = m_time_offset;
								if(m_template_tag.GetMediaUrl())
								{
									if(m_base_url[0]!='\0')
									{
										//int len = strlen(m_base_url)+strlen(url)+strlen(m_template_tag.GetMediaUrl());
										//memcpy(pval_media ,url,strlen(m_base_url)+strlen(url)+strlen(m_template_tag.GetMediaUrl()));
										//strcat(pval_media,m_base_url);
										//strcat(pval_media,m_template_tag.GetMediaUrl());
										int len = strlen(m_base_url)+strlen(m_template_tag.GetMediaUrl());
										memcpy(pval_media ,m_base_url,strlen(m_base_url)+strlen(m_template_tag.GetMediaUrl()));
										strcat(pval_media,m_template_tag.GetMediaUrl());
										pval_media[len]= '\0';
									}
									else
									{
										/*memcpy(pval_media ,url,strlen(m_template_tag.GetMediaUrl())+strlen(url));
										int len = strlen(m_template_tag.GetMediaUrl())+strlen(url);
										strcat(pval_media,m_template_tag.GetMediaUrl());*/
										int len = strlen(m_template_tag.GetMediaUrl());
										memcpy(pval_media,m_template_tag.GetMediaUrl(),len);
										pval_media[len] = '\0';
									}

								}
								if(m_template_tag.GetIniaUrl())
								{
									if(m_base_url[0]!='\0')
									{
										/*int len = strlen(m_base_url)+strlen(url)+strlen(m_template_tag.GetIniaUrl());
										memcpy(pval_inia ,url,len);
										strcat(pval_inia,m_base_url);
										strcat(pval_inia,m_template_tag.GetIniaUrl());*/
										int len = strlen(m_base_url)+strlen(m_template_tag.GetIniaUrl());
										memcpy(pval_inia ,m_base_url,len);
										strcat(pval_inia,m_template_tag.GetIniaUrl());
										pval_inia[len]= '\0';
									}
									else
									{
										/*	memcpy(pval_inia ,url,strlen(m_template_tag.GetIniaUrl())+strlen(url));
										int len = strlen(m_template_tag.GetIniaUrl())+strlen(url);
										strcat(pval_inia,m_template_tag.GetIniaUrl());*/
										memcpy(pval_inia ,m_template_tag.GetIniaUrl(),strlen(m_template_tag.GetIniaUrl()));
										int len = strlen(m_template_tag.GetIniaUrl());
										pval_inia[len] = '\0';
									}

								}
								//	memcpy(pRepresentation->mediaUrl,pval_media,strlen(pval_media));

								memcpy(pRepresentation->initalUrl,pval_inia,strlen(pval_inia));
								pRepresentation->initalUrl[strlen(pval_inia)] = '\0';
								int length_ini = strlen(pRepresentation->initalUrl);
								pRepresentation->mediaUrl[length_ini] = '\0';
								int length = strlen(pRepresentation->mediaUrl);
								memcpy(pRepresentation->mediaUrl,pval_media,strlen(pval_media));
								length = strlen(pval_media);
								pRepresentation->mediaUrl[length] = '\0';

								TiXmlNode *p_child = NULL;
								int group_index = *group_count-1;
								p_child= pLevel_segment->FirstChild();
								if(pAdaptionSet->nTrackType == VO_SOURCE_TT_VIDEO)
								{
									// m_template_tag->Init(pLevel_segment);
									m_template_tag.Init(pLevel_segment);
									// m_manifest->timeScale = m_template_tag->GetTimeScale();
									m_manifest->timeScale = m_template_tag.GetTimeScale();
									video_scale = m_manifest->timeScale;
									audio_scale = video_scale;
									// m_manifest->duration_video = m_template_tag->GetDuration();
									m_manifest->duration_video = m_template_tag.GetDuration();
									if(m_template_tag.GetStart_number())
									{
										if(m_manifest->duration_video==0)
											m_seg_count = 0;
										else
										{
											m_seg_count = m_manifest->mediaPresentationDuration/ m_manifest->duration_video;
											if(m_seg_count==0)
											{
												m_seg_count =300;
											}
										}
										ini_number  = m_template_tag.GetStart_number();
										/*
										add for start_number has been inia
										*/
										if(ini_number==0)
											ini_number=1;
										/*end*/
									}
									if(p_child)
									{
										const char * pValue = NULL;
										pValue = p_child->Value();
										if(StrCompare(pValue, TAG_DASH_TIMELINE)==0)
										{
											bSegmentTimeline = VO_TRUE;
											if(video_flag ==0)
												analyze_video(Duration_video,Timeline_video,p_child,1,group_index);
										}
									}
									//add for the representation
								}
								else if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_AUDIO)
								{
									/* m_manifest->timeScale  = m_template_tag->GetTimeScale();
									audio_scale = m_manifest->timeScale;
									m_manifest->duration_audio = m_template_tag->GetDuration();*/
									m_manifest->timeScale  = m_template_tag.GetTimeScale();
									audio_scale = m_manifest->timeScale;
									m_manifest->duration_audio = m_template_tag.GetDuration();

									if(m_template_tag.GetStart_number())
							  {
								  if(m_manifest->duration_audio==0)
									  m_seg_count = 0;
								  else
								  {
									  m_seg_count = m_manifest->mediaPresentationDuration/ m_manifest->duration_video;
									  if(m_seg_count==0)
									  {
										  m_seg_count =200;
									  }

								  }
								  ini_number = m_template_tag.GetStart_number();
									}
									if(p_child)
							  {
								  const char * pValue = NULL;
								  pValue = p_child->Value();
								  if(StrCompare(pValue, TAG_DASH_TIMELINE)==0)
								  {
									  bSegmentTimeline = VO_TRUE;
									  if(video_flag ==0)
										  analyze_video(Duration_video,Timeline_audio,pNode,1,group_index);
								  }
							  }
								}
								if(!bSegmentTimeline)
								{
									Seg_item * item = new Seg_item;
									memset( item , 0 , sizeof(Seg_item));
									item->starttime =  0; 
									item->startnmber = item->nOrignalStartNumber = ini_number;
									item->nIsVideo =  pAdaptionSet->nTrackType ==VO_SOURCE_TT_AUDIO ? 0 :(pAdaptionSet->nTrackType ==VO_SOURCE_TT_VIDEO? 1:0);
									item->duration =  m_manifest->duration_video;
									item->group_index = group_index;
									item->bDependForNextTimeLine = VO_TRUE;
									item->period_index = period_index;
									add_fragmentItem(item);
									Seg_item * p_item = new Seg_item;
									memset( p_item , 0 , sizeof(Seg_item));
									p_item->starttime =  0; 
									p_item->startnmber =  item->nOrignalStartNumber = ini_number;
									p_item->nIsVideo =  item->nIsVideo;
									p_item->duration =  m_manifest->duration_video;
									p_item->group_index = group_index;
									p_item->bDependForNextTimeLine = VO_TRUE;
									p_item->period_index = period_index;
									add_segmentItem(pRepresentation,p_item);
								}
							}
							if(StrCompare(pTagName,TAG_DASH_HOLDER)==0)
							{


							}
							else if(StrCompare(pTagName,TAG_DASH_SEGMENTBASE)==0)
							{
								TiXmlNode * temp = NULL;
								temp=  pLevel_segment->FirstChild();
								m_seg_base.Init(temp);
								m_seg_base.GetTimeScale();
								if(m_seg_base.GetIniUrl())
								{
									if(m_base_url[0]!='\0')
									{
										/*int len = strlen(m_base_url)+strlen(url)+strlen(m_seg_base.GetIniUrl());
										memcpy(pval_inia ,url,len);
										strcat(pval_inia,m_base_url);
										strcat(pval_inia,attriValue);*/
										int len = strlen(m_base_url)+strlen(m_seg_base.GetIniUrl());
										memcpy(pval_inia ,m_base_url,len);
										strcat(pval_inia,attriValue);
										pval_inia[len]= '\0';
									}
									else
									{
										/*memcpy(pval_inia ,url,strlen(m_seg_base.GetIniUrl())+strlen(url));
										int len = strlen(m_seg_base.GetIniUrl())+strlen(url);
										strcat(pval_inia,m_seg_base.GetIniUrl());*/
										memcpy(pval_inia ,m_seg_base.GetIniUrl(),strlen(m_seg_base.GetIniUrl()));
										int len = strlen(m_seg_base.GetIniUrl());
										pval_inia[len] = '\0';
									}
									memcpy(pRepresentation->initalUrl,pval_inia,strlen(pval_inia));

								}
								int range_len = strlen(m_seg_base.GetIni_range());
								if(range_len!=0)
									memcpy(pRepresentation->ini_range,attriValue,strlen(attriValue));
								//pLevel_segment =pLevel_segment->NextSibling();
							}//include the baseUrl  the first
							else if(StrCompare(pTagName,TAG_DASH_BASEURL_2)==0)
							{
								attriValue = pLevel_segment->FirstChild()->Value();
								if(m_base_url[0]!='\0')
								{
									//	   memset(m_base_url,0,strlen(m_base_url)+strlen(attriValue));
									memcpy(m_base_url_temp ,m_base_url,strlen(attriValue)+strlen(m_base_url));
									strcat(m_base_url_temp,attriValue);
									m_base_url_temp[strlen(attriValue)+strlen(m_base_url)] = '\0';

								}
								//pLevel_segment =pLevel_segment->NextSibling();
							}
							else if(StrCompare(pTagName,TAG_DASH_STREAMING_SegmentInfo)==0||StrCompare(pTagName,TAG_DASH_SEGMENTLIST)==0)
							{
								m_seg_info.Init(pLevel_segment);
								pRepresentation->duration = m_seg_info.GetDuration();
								if(pAdaptionSet->nTrackType == VO_SOURCE_TT_VIDEO)
								{
									m_manifest->duration_video = pRepresentation->duration;
									m_manifest->timeScale = m_seg_info.GetTimeScale();
									video_scale = m_manifest->timeScale;
									audio_scale = video_scale;
								}
								else if(pAdaptionSet->nTrackType == VO_SOURCE_TT_AUDIO)
								{
									m_manifest->duration_audio = pRepresentation->duration;
									m_manifest->timeScale = m_seg_info.GetTimeScale();
									audio_scale = video_scale;
								}

								TiXmlNode * pLevel_segment_1 = NULL;
								pLevel_segment_1 = pLevel_segment->FirstChild();
								const char * pTagName_1 = NULL;
								int segment_Index  = 0;
								VO_CHAR	base_url[512];
								memcpy(base_url ,url,strlen(url));
								while(pLevel_segment_1)
								{
									pTagName_1 = pLevel_segment_1->Value();
									if(StrCompare(pTagName_1,TAG_DASH_BASEURL_2)==0)
									{
										attriValue =  pLevel_segment_1->ToElement()->Attribute(TAG_DASH_BASEURL_2);
										if(attriValue)
										{
											if(strstr(attriValue , ("http://")))
											{
												memcpy(base_url ,attriValue,strlen(attriValue));
											}
											else
											{
												if(m_base_url[0]!='\0')
												{
													//	memcpy(m_base_url_temp ,url,strlen(attriValue)+strlen(url));
													memcpy(m_base_url_temp ,attriValue,strlen(attriValue));
												}
												else
												{
													//	memcpy(base_url ,url,strlen(attriValue)+strlen(url));
													memcpy(base_url ,attriValue,strlen(attriValue));
												}

												//strcat(base_url,attriValue);
												replace(base_url,"manifest.mpd","");
												replace(base_url,"Manifest.mpd","");
												replace(base_url,"vlcManifestVideo.mpd","");
												replace(base_url,"vlcManifestAudio.mpd","");
												replace(base_url,"vlc",""); 
											}
										}//endof  if(attriValue)
										//pLevel_segment_1 = pLevel_segment_1->NextSibling();
									}//endof if(StrCompare(pTagName_1,TAG_DASH_BASEURL_2)==0)
									else if(StrCompare(pTagName_1,TAG_DASH_STREAMING_InitialisationSegmentURL)==0||StrCompare(pTagName_1,TAG_DASH_BASEURL)==0||StrCompare(pTagName_1,TAG_DASH_BASEURL_3)==0)
									{
										attriValue =  pLevel_segment_1->ToElement()->Attribute(TAG_DASH_STREAMING_SOURCEURL);
										if(attriValue)
										{
											if(strstr(attriValue , ("http://")))
											{
												VOLOGR( "It is HTTP url Streaming!" );
												memcpy(pRepresentation->initalUrl ,attriValue,strlen(attriValue));
												pRepresentation->initalUrl[strlen(attriValue)]='\0';

											}
											else
											{ 
												if(m_base_url[0]!='\0')
												{
													memcpy(pRepresentation->initalUrl ,m_base_url_temp,strlen(attriValue)+strlen(m_base_url));
												}
												else
												{
													//	 memcpy(pRepresentation->initalUrl ,url,strlen(attriValue)+strlen(url));
													memcpy(pRepresentation->initalUrl,attriValue,strlen(attriValue));
													pRepresentation->initalUrl[strlen(attriValue)] = '\0';


												}

												//memcpy(pRepresentation->initalUrl,attriValue);
												//	replace(pRepresentation->initalUrl,"manifest.mpd","");
												//	replace(pRepresentation->initalUrl,"Manifest.mpd","");
												//	replace(pRepresentation->initalUrl,"vlcManifestVideo.mpd","");
												//	replace(pRepresentation->initalUrl,"vlcManifestAudio.mpd","");
												//	replace(pRepresentation->initalUrl,"vlc","");
											}
										}
										//pLevel_segment_1 =   pLevel_segment_1->NextSibling();
									}//enfof 	else if(StrCompare(pTagName_1,TAG_DASH_STREAMING_InitialisationSegmentURL)==0||StrCompare(pTagName_1,TAG_DASH_BASEURL)==0||StrCompare(pTagName_1,TAG_DASH_BASEURL_3)==0)
									else if(StrCompare(pTagName_1,TAG_DASH_STREAMING_URL)==0||StrCompare(pTagName_1,TAG_DASH_SEGURL)==0)
									{
										Seg_item * ptr_item = new Seg_item;
										memset( ptr_item , 0 , sizeof(Seg_item));
										int group_index = *group_count-1;
										ptr_item->group_index = group_index;
										attriValue = pLevel_segment_1->ToElement()->Attribute(TAG_DASH_RANGE);
										if(attriValue)
										{ 	
											memcpy(ptr_item->range,attriValue,strlen(attriValue));
										}

										attriValue =  pLevel_segment_1->ToElement()->Attribute(TAG_DASH_MEDIABASEURL);
										if(attriValue)
										{
											if(strstr(attriValue , ("http://")))
											{
												VOLOGR( "It is HTTP url Streaming!" );
												memcpy(ptr_item->item_url ,attriValue,strlen(attriValue));
												ptr_item->starttime = pRepresentation->duration*seg_index;
												seg_index++;

											}
											else if(strstr(attriValue , (".")))
											{
												if(m_base_url[0]!='\0')
												{
													memcpy(ptr_item->item_url ,m_base_url_temp,strlen(attriValue)+strlen(base_url));
												}
												else
													//	memcpy(ptr_item->item_url ,url,strlen(attriValue)+strlen(base_url));
													memcpy(ptr_item->item_url ,attriValue,strlen(attriValue));

												// strcat(ptr_item->item_url,attriValue);
												ptr_item->starttime = pRepresentation->duration*seg_index;
												seg_index++;
											}
											else
											{
												int le= strlen(attriValue);
												const char * length = NULL;
												const char * find_url = "/";
												int index = m_comtag->lastIndex(attriValue,find_url);
												le = le-index-1;
												length = attriValue+index+1;
#if defined _WIN32
												ptr_item->starttime= _atoi64(length);
#elif defined LINUX
												ptr_item->starttime=  atoll(length);
#endif		 

												//	memcpy(ptr_item->item_url,url,strlen(url)+strlen(attriValue));
												//strcat(ptr_item->item_url,attriValue);
												memcpy(ptr_item->item_url,attriValue,strlen(attriValue));

											}
										}

										if(pAdaptionSet->nTrackType == VO_SOURCE_TT_VIDEO)
										{
											ptr_item->nIsVideo = 1;
										}
										else
											ptr_item->nIsVideo = 0;
										attriValue =  pLevel_segment_1->ToElement()->Attribute(TAG_DASH_STREAMING_SOURCEURL);
										if(attriValue)
										{
											if(strstr(attriValue , ("http://")))
											{
												VOLOGR( "It is HTTP url Streaming!" );
												memcpy(ptr_item->item_url ,attriValue,strlen(attriValue));
												ptr_item->starttime = pRepresentation->duration*seg_index;
												seg_index++;
											}
											else if(strstr(attriValue , (".")))
											{
												memcpy(ptr_item->item_url ,base_url,strlen(attriValue)+strlen(base_url));
												strcat(ptr_item->item_url,attriValue);
												ptr_item->starttime = pRepresentation->duration*seg_index;
												seg_index++;
											}
											else
											{
												int le= strlen(attriValue);
												const char * length = NULL;
												const char * find_url = "/";
												int index = m_comtag->lastIndex(attriValue,find_url);
												le = le-index-1;
												length = attriValue+index+1;
												ptr_item->starttime= _ATO64(length); 
												memcpy(ptr_item->item_url,url,strlen(url)+strlen(attriValue));
												strcat(ptr_item->item_url,attriValue);
											}
										}  
										ptr_item->duration = pRepresentation->duration;
										ptr_item->segment_index = segment_Index;//start from

										if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_VIDEO )
										{
											if(video_flag ==0&&pRepresentation->rpe_item_index ==0)
											{
												Seg_item * temp = new Seg_item;
												memset( temp , 0 , sizeof(Seg_item));
												temp->duration =ptr_item->duration ;
												temp->starttime = ptr_item->starttime;
												temp->nIsVideo = 1;
												temp->group_index =ptr_item->group_index; 
												memcpy(temp->item_url,ptr_item->item_url,strlen(ptr_item->item_url));
												memcpy(temp->range,ptr_item->range,strlen(ptr_item->range));
												if(pRepresentation->track_count!=2)
													add_fragmentItem(temp); 
												// add_fragmentItem(temp); //add one time
											}
										}
										else if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_AUDIO&&pRepresentation->rpe_item_index ==0)
										{
											if(audio_flag==0)
											{
												Seg_item * temp = new Seg_item;
												memset( temp , 0 , sizeof(Seg_item));
												temp->duration =ptr_item->duration ;
												temp->starttime = ptr_item->starttime;
												temp->nIsVideo = 0;
												temp->group_index =ptr_item->group_index; 
												memcpy(temp->item_url,ptr_item->item_url,strlen(ptr_item->item_url));
												memcpy(temp->range,ptr_item->range,strlen(ptr_item->range));
												if(pRepresentation->track_count!=2)
													add_fragmentItem(temp); 
											}
										}
										add_segmentItem(pRepresentation,ptr_item);
										segment_Index++;
									}//	endof else if(StrCompare(pTagName_1,TAG_DASH_STREAMING_URL)==0||StrCompare(pTagName_1,TAG_DASH_SEGURL)==0)

									pLevel_segment_1 =  pLevel_segment_1->NextSibling();
								}//endof while(pLevel_segment_1) 
							}//endof if(StrCompare(pTagName,TAG_DASH_STREAMING_SegmentInfo)==0||StrCompare(pTagName,TAG_DASH_SEGMENTLIST)==0)
							pLevel_segment= pLevel_segment->NextSibling();
						}//endof  while(pLevel_segment)  
						//pLevel_repre = pLevel_repre->NextSibling();
					}//endof if(StrCompare(rpe_name, TAG_DASH_STREAMING_Representation)==0||StrCompare(rpe_name, TAG_DASH_STREAMING_Representation_G1)==0) 
					pLevel_repre = pLevel_repre->NextSibling();				
				}
				//endof while(pLevel_repre)  
			}//endof if(0==StrCompare(pGroup_Name,TAG_DASH_STREAMING_Group)||0==StrCompare(pGroup_Name, TAG_SIMPLE_ADAPTIONSET)||0==StrCompare(pGroup_Name, TAG_SIMPLE_ADAPTIONSET_G1)||0==StrCompare(pGroup_Name, TAG_DASH_STREAMING_Group_G1))  

			if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_VIDEO)
				video_flag++;
			else if(pAdaptionSet->nTrackType ==VO_SOURCE_TT_AUDIO)
				audio_flag++;
			//if(pLevel_group)
			pLevel_group = pLevel_group->NextSibling();	
		}//endof while(pLevel_group) 
		pChildNode = pChildNode->NextSibling();
	}//endof while(pChildNode) 
	VOLOGR("Analyze %d",video_scale);
	m_manifest->audio_scale = audio_scale;
	m_manifest->video_scale = video_scale;
	ApaptionSet *streamIndexc = new ApaptionSet[m_manifest->group_count];
	memcpy(streamIndexc,adaption_set,sizeof(ApaptionSet)*m_manifest->group_count);
	m_manifest->adaptionSet = streamIndexc;
	return VO_TRUE;

}

#endif


VO_VOID VO_MPD_Parser::replace(char *source,const char *sub,const char *rep )
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

int VO_MPD_Parser::StrCompare(const char* p1, const char* p2)
{
#if defined  WINCE
	return strcmp(p1, p2);
#elif defined  WIN32
	return stricmp(p1, p2);
#elif defined LINUX
	return strcasecmp(p1, p2);
#elif defined _IOS
	return strcasecmp(p1, p2);
#elif defined _MAC_OS
	return strcasecmp(p1, p2);
#endif
}



VO_VOID VO_MPD_Parser::addRepresentationLevel(ApaptionSet * pAdaptionSet,int group_index, Representation *pRepresentation){

	pRepresentation->rpe_item_index = pAdaptionSet->rpe_number;
	pRepresentation->byterange_chunk_count = 0;
	pRepresentation->current_sigx_offset_time = 0;
	pRepresentation->head_range_end =0;
	//pRepresentation->index_QL = pAdaptionSet->rpe_number;

	if( pAdaptionSet->m_ptr_QL_head == NULL && pAdaptionSet->m_ptr_QL_tail == NULL )
	{ 
		pAdaptionSet->m_ptr_QL_head = pAdaptionSet->m_ptr_QL_tail = pRepresentation;
	}
	else if(pAdaptionSet->m_ptr_QL_tail->bitrate<=pRepresentation->bitrate)
	{
		pAdaptionSet->m_ptr_QL_tail->ptr_next = pRepresentation;
		pRepresentation->ptr_pre = pAdaptionSet->m_ptr_QL_tail;
		pAdaptionSet->m_ptr_QL_tail = pRepresentation;
	}
	else if(pAdaptionSet->m_ptr_QL_head->bitrate>pRepresentation->bitrate)
	{
		pRepresentation->ptr_next = pAdaptionSet->m_ptr_QL_head;
		pAdaptionSet->m_ptr_QL_head = pRepresentation;
	}
	else
	{
		Representation *ptr_cur = pAdaptionSet->m_ptr_QL_head;
		while(ptr_cur)
		{
			if(pRepresentation->bitrate>ptr_cur->bitrate)
			{
				pRepresentation->ptr_next = ptr_cur->ptr_next;
				ptr_cur->ptr_next = pRepresentation;
				pRepresentation->ptr_pre =ptr_cur;
				break;
			}
			ptr_cur = ptr_cur->ptr_next;
		}
	}
	pAdaptionSet->rpe_number++;
}


VO_S64 VO_MPD_Parser::find_fragment_starttime(VO_U64 pos, int nIsVideo,VO_U32 period_index){

	Seg_item * ptr_temp = m_ptr_FI_head; 
	while(m_period_count>1&&ptr_temp&&ptr_temp->period_index!=period_index)
	{
		ptr_temp = ptr_temp->ptr_next;

	}
	if(ptr_temp==NULL)
	{
		return 0;	
	}

	/*add by leon*/
	if(!ptr_temp )
		return 0;
	VO_S64 start_time = ptr_temp->starttime;
	//pos += start_time;
	Seg_item * tmpPtr = NULL;
	Seg_item * tmpPtrPre = NULL;
	if(ptr_temp->bDependForNextTimeLine&&!Is_Dynamic())
		//if(ptr_temp->bDependForNextTimeLine)//for live seek
	{
		start_time = pos;
		return start_time;

	}

	while(ptr_temp)
	{
		if( ptr_temp->nIsVideo == nIsVideo&&ptr_temp->period_index ==period_index)
		{
			if(pos >= ptr_temp->starttime)
			{
				tmpPtrPre = tmpPtr;
				tmpPtr = ptr_temp;
			}
			else if(pos < ptr_temp->starttime )
			{
				break;
			}
		}
		ptr_temp = ptr_temp->ptr_next;
	}

	return tmpPtrPre ? tmpPtrPre->starttime :start_time;
}

VO_S64 VO_MPD_Parser::find_fragment_starttime_index(VO_U64 * pos, int nIsVideo,VO_U32 period_index )
{

	Seg_item * ptr_temp = m_ptr_FI_head;
	while(m_period_count>1&&ptr_temp->period_index!=period_index)
	{
		ptr_temp = ptr_temp->ptr_next;

	}
	if(ptr_temp==NULL)
	{
		return 0;	
	}
	/*add by leon*/
	if(!ptr_temp )
		return 0;
	VO_S64 start_time = ptr_temp->starttime;
	VO_U32 number = ptr_temp->nOrignalStartNumber;
	Seg_item * tmpPtr = NULL;
	Seg_item * tmpPtrPre = NULL;
	VO_S64 index_video = 0;
	VOLOGW("in  %lld index_video %d duration %lld",pos,index_video,ptr_temp->duration);
	if(ptr_temp->bDependForNextTimeLine&&!Is_Dynamic())
	{
		if(Get_Video_Scale(period_index) ==1)
		{
			index_video = * pos/ptr_temp->duration;
			start_time = index_video*ptr_temp->duration;
			*pos =start_time;
			//    index_video = index_video+number;
			VOLOGW("find_time 1 %lld index_video %d duration %lld",pos,index_video,m_ptr_FI_head->duration);
			return index_video;

		}
		else 
		{
			int duration  = 0;
			if(m_manifest->duration_video!=0)
				duration =  m_manifest->duration_video;
			else
				duration =  ptr_temp->duration;
			index_video = * pos/(duration); //fix the seek bug
			start_time = index_video*ptr_temp->duration;
			*pos =start_time;
			VOLOGW("find_time 2 %lld index_video %d duration %lld",pos,index_video,m_ptr_FI_head->duration);
			return index_video;

		}



	}
	VOLOGW("out");

	while(ptr_temp )
	{
		if( ptr_temp->nIsVideo == nIsVideo&&ptr_temp->period_index ==period_index&&(Is_Template()||ptr_temp->representation_index ==0))
		{

			if(* pos >= ptr_temp->starttime)
			{
				index_video++;
				tmpPtrPre = tmpPtr;
				tmpPtr = ptr_temp;
			}
			else if(*pos < ptr_temp->starttime )
			{
				break;
			}
		}
		ptr_temp = ptr_temp->ptr_next;
	}
	// return index_video;
	VOLOGW("out 2222   index_video %lld ",index_video);
	if(tmpPtr)
	*pos = tmpPtrPre?tmpPtr->starttime:0;
	return tmpPtrPre ? index_video-1 :0;

}
VO_S32  VO_MPD_Parser::get_lang_group(VO_CHAR * lang,VO_U32 period_index)
{

	for(int index = 0; index <GetGroupCount();index++)
	{
		//VOLOGW("get_lang_group %s ",m_manifest->adaptionSet[index].lang);
		if(m_manifest->adaptionSet)
		{
		if(StrCompare(m_manifest->adaptionSet[index].lang,lang)==0&&m_manifest->adaptionSet[index].period_index ==period_index)
		{
			VOLOGW("get_lang_group  index %d ",index);
			return index;
			break;
		}
		}
	}
	return -1;

}
VO_S32  VO_MPD_Parser::get_lang_track(VO_S32 track_id)
{
	for(int index = 0; index <GetGroupCount();index++)
	{
		Representation * ql = m_manifest->adaptionSet[index].m_ptr_QL_head;
		while(ql)
		{
		if(ql&&ql->rpe_track_id==track_id&&m_manifest->adaptionSet[index].track_id!=-1)
		{
			return index;
			break;
		}
		else
			ql= ql->ptr_next;
		}
	}
	return -1;

}
VO_S32  VO_MPD_Parser::get_rpe_index(VO_S32 group_id,VO_S32 track_id)
{
	Representation * ql = m_manifest->adaptionSet[group_id].m_ptr_QL_head;
	while(ql)
	{
		if(ql->rpe_track_id ==track_id )
		{
			return ql->rpe_item_index;
			break;
		}
		ql = ql->ptr_next;
	}
	return 0;
}
VO_U64 VO_MPD_Parser::GetStartTime()
{
	if(m_ptr_FI_head)
		return m_ptr_FI_head->starttime;
	else
		return 0;
}
Representation * VO_MPD_Parser::get_qualityLevel( int stream_index,int index_item )
{
	if (NULL == m_manifest || NULL == m_manifest->adaptionSet)
	{
		return NULL;
	}
	Representation * pCurRep = m_manifest->adaptionSet[stream_index].m_ptr_QL_head;
	while(pCurRep)
	{
		if( pCurRep->rpe_item_index == index_item )
			return pCurRep;
		pCurRep = pCurRep->ptr_next;
	}
	return NULL;
}


VO_VOID  VO_MPD_Parser::add_segmentItem(Representation * rpe,Seg_item * ptr_item)
{
	VO_S64 temp_time = -1 ;
	VO_S64 head_time = -1 ;
	VO_S64 tail_time = -1;
	if(video_scale==0)
		video_scale =1;
	if(audio_scale==0)
		audio_scale =1;
	if(ptr_item->nIsVideo)
		temp_time = ptr_item->starttime*1000/video_scale;
	else
		temp_time = ptr_item->starttime*1000/audio_scale;
	ptr_item->ptr_next = NULL;
	if( rpe->m_ptr_SG_head == NULL &&  rpe->m_ptr_SG_tail == NULL )
	{
		rpe->m_ptr_SG_head =  rpe->m_ptr_SG_tail = ptr_item;
	}
	else
	{
		if( rpe->m_ptr_SG_head->nIsVideo)
			head_time =  rpe->m_ptr_SG_head->starttime*1000/video_scale;
		else
			head_time =   rpe->m_ptr_SG_head->starttime*1000/audio_scale;
		if(rpe->m_ptr_SG_head->nIsVideo)
			tail_time = rpe->m_ptr_SG_tail->starttime*1000/video_scale;
		else
			tail_time =  rpe->m_ptr_SG_tail->starttime*1000/audio_scale;
		if (temp_time>= tail_time)
		{
			rpe->m_ptr_SG_tail->ptr_next = ptr_item;
			rpe->m_ptr_SG_tail = ptr_item;
		}
		else if( temp_time <  head_time
			|| (ptr_item->group_index == 0 &&temp_time == head_time))
		{
			ptr_item->ptr_next =  rpe->m_ptr_SG_head;
			rpe->m_ptr_SG_head = ptr_item;
		}
		else
		{
			Seg_item * ptr_cur =  rpe->m_ptr_SG_head;
			VO_S64 cur_time = -1;
			VO_S64 cur_time_next = -1;
			while(ptr_cur->ptr_next)
			{
				if(ptr_cur->ptr_next->nIsVideo)
					cur_time_next = ptr_cur->ptr_next->starttime*1000/video_scale;
				else
					cur_time_next = ptr_cur->ptr_next->starttime*1000/audio_scale;
				if(ptr_cur->nIsVideo)
					cur_time = ptr_cur->starttime*1000/video_scale;
				else
					cur_time = ptr_cur->starttime*1000/audio_scale;
				if(temp_time >= cur_time && temp_time < cur_time_next)
				{
					ptr_item->ptr_next = ptr_cur->ptr_next;

					ptr_cur->ptr_next = ptr_item;

					break;
				}
				ptr_cur = ptr_cur->ptr_next;
			}

		}
	}
	rpe->byterange_chunk_count++;
	rpe->segmentsNumber++;//the number segments in  the representation 
}
VO_VOID  VO_MPD_Parser::add_segmentItem_subtile(Representation * rpe,Seg_item * ptr_item)
{
	VO_S64 temp_time = -1 ;
	VO_S64 head_time = -1 ;
	VO_S64 tail_time = -1;
	if(subtile_scale ==0)
		subtile_scale = 1;
	 temp_time =  ptr_item->starttime*1000/subtile_scale;
	ptr_item->ptr_next = NULL;
	if( rpe->m_ptr_SG_head == NULL &&  rpe->m_ptr_SG_tail == NULL )
	{
		rpe->m_ptr_SG_head =  rpe->m_ptr_SG_tail = ptr_item;
	}
	else
	{
		head_time =  rpe->m_ptr_SG_head->starttime*1000/subtile_scale;
		tail_time = rpe->m_ptr_SG_tail->starttime*1000/subtile_scale;
		if (temp_time>= tail_time)
		{
			rpe->m_ptr_SG_tail->ptr_next = ptr_item;
			rpe->m_ptr_SG_tail = ptr_item;
		}
		else if( temp_time <  head_time
			|| (ptr_item->group_index == 0 &&temp_time == head_time))
		{
			ptr_item->ptr_next =  rpe->m_ptr_SG_head;
			rpe->m_ptr_SG_head = ptr_item;
		}
		else
		{
			Seg_item * ptr_cur =  rpe->m_ptr_SG_head;
			VO_S64 cur_time = -1;
			VO_S64 cur_time_next = -1;
			while(ptr_cur->ptr_next)
			{
				cur_time_next = ptr_cur->ptr_next->starttime*1000/subtile_scale;
				if(temp_time >= cur_time && temp_time < cur_time_next)
				{
					ptr_item->ptr_next = ptr_cur->ptr_next;

					ptr_cur->ptr_next = ptr_item;

					break;
				}
				ptr_cur = ptr_cur->ptr_next;
			}

		}
	}
	rpe->byterange_chunk_count++;
	rpe->segmentsNumber++;//the number segments in  the representation 

}
VO_VOID    VO_MPD_Parser::GetChunkRange(char *Range_URL,VO_U32 *ini_range,VO_U32 *end_range,VO_U32 *size)
{
	VO_U32 range_start = 0;
	VO_U32 range_end = 0;
	sscanf(Range_URL, "%d-%d",    
		&range_start,
		&range_end);
	*ini_range = range_start;
	*end_range = range_end;
	if(range_end!=0)
		*size = range_end-range_start+1;



}
VO_S32 VO_MPD_Parser:: GetStreamCount(VO_S32 period_index)
{
	return m_stream_count;
}
VO_U32 VO_MPD_Parser::InitParser()
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	m_pXmlLoad->OpenParser();
	return nResult;
}

VO_U32 VO_MPD_Parser::UninitParser()
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	m_pXmlLoad->CloseParser();

	return nResult;
}

VO_CHAR *  VO_MPD_Parser::GetMPDUrl()
{
	if(strlen(mpd_base_url))
		return mpd_base_url;
	else
		return NULL;

}
VO_VOID VO_MPD_Parser::Init_MPD(VO_U32 period_index)
{
	VO_S32 count = 0;
	m_videoCount =0;
	m_audioCount = 0;
	m_subtitleCount = 0;
	if(!m_manifest->adaptionSet)	
	{
		VOLOGI("Init_MPD null adaptionSet"); 
		return;
	}
	if(!m_manifest->m_period)	
	{
		VOLOGI("Init_MPD null m_period"); 
		return;
	}
	for(VO_S32 i = 0;i <m_manifest->group_count; i++ )
	{
		if(m_manifest->adaptionSet&&m_manifest->adaptionSet[i].period_index == period_index)
		{
			if(m_manifest->adaptionSet[i].nTrackType == VO_SOURCE_TT_VIDEO)
			{
				if(m_is_ts||(!m_is_ts&&m_manifest->adaptionSet[i].m_ptr_QL_head->track_count!=2||m_manifest->group_count<2))
					m_videoCount += m_manifest->adaptionSet[i].rpe_number;
				m_manifest->adaptionSet[i].track_id = -1;
			}
			else if(m_manifest->adaptionSet[i].nTrackType == VO_SOURCE_TT_AUDIO)
			{
				Representation * ql = m_manifest->adaptionSet[i].m_ptr_QL_head;
				while(ql)
				{	
					ql->rpe_track_id = m_audio_bitrate_count;
					ql = ql->ptr_next;
					m_audio_bitrate_count++;
				}
				m_manifest->adaptionSet[i].track_id =m_audioCount;
				m_audioCount += 1;
				if(strlen(m_manifest->adaptionSet[i].lang)==0)
				{
					const VO_CHAR * default_lang = "en";
						strcpy(m_manifest->adaptionSet[i].lang,default_lang);
				}
			}
		}
	}
	VO_U32 subtitle_count = m_audio_bitrate_count;
//	m_subtitleCount = m_audioCount;
	for(VO_S32 i = 0;i <m_manifest->group_count; i++ )
	{
		if(m_manifest->adaptionSet[i].nTrackType == VO_SOURCE_TT_SUBTITLE )
		{
			Representation * ql = m_manifest->adaptionSet[i].m_ptr_QL_head;
			ql->rpe_track_id = subtitle_count;
			m_manifest->adaptionSet[i].track_id =subtitle_count;
			m_subtitleCount+=1;
			subtitle_count+=1;
		}
	}
	VO_S32 j = 0;
	for(;j+1<m_manifest->period_count;j++)
	{
		m_manifest->m_period[j].period_fact_duration = m_manifest->m_period[j+1].period_start- m_manifest->m_period[j].period_start;
	}
	m_manifest->m_period[j].period_fact_duration = m_manifest->m_period[j].period_init_duration;
	m_stream_count = m_videoCount + m_audioCount;
	if(m_audioCount==0&&!m_is_muxed)
		m_is_pure_video = VO_TRUE;
	m_manifest->m_start_time = m_ptr_FI_head?m_ptr_FI_head->starttime:0;
	m_duration = m_manifest->mediaPresentationDuration;
	Seg_item * pSegItem = NULL;
	VO_S64 m_start_time_video = -1;
    VO_S64 m_start_time_audio = -1;
	if(m_videoCount!=0)
	get_segmentItem(&pSegItem,0,1,period_index);
	else if(m_audioCount!=0)
		get_segmentItem(&pSegItem,0,0,period_index);
	if(pSegItem)
	{
		if(m_videoCount!=0)
		m_timeline_time = (pSegItem->starttime)/video_scale;
		else
			m_timeline_time = (pSegItem->starttime)/audio_scale;
		m_is_depend = pSegItem->bDependForNextTimeLine;
		m_manifest->duration_video = pSegItem->duration;
		m_start_time_video = pSegItem->starttime;
	}
	else
		m_manifest->duration_video = 0;
	if(!m_is_ts&&m_audioCount!=0)
	{
		get_segmentItem(&pSegItem,0,0,period_index);
		if(pSegItem)
		{
			m_manifest->duration_audio = pSegItem->duration;
			m_start_time_audio = pSegItem->starttime;
			m_manifest->m_start_time  = (m_start_time_audio/audio_scale<m_start_time_video/video_scale?m_start_time_audio:m_start_time_video);
		}
		else
			m_manifest->duration_audio = 0;
	}
	else
	{
		m_manifest->duration_audio = 0;
		m_manifest->m_start_time  = m_start_time_video;
	}
	if(m_videoCount==0&&m_audioCount!=0)
		m_is_pure_audio = VO_TRUE;
	VOLOGI("Init_MPD success"); 
}
#if 0
VO_U32 VO_MPD_Parser::DoumpTheRawData(VO_BYTE* pBuffer, VO_U32 nLen)
{
	FILE * pFile;
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	VO_U32 tmpsize = 0;

#ifdef _WIN32
	const char* pFilename = "d:\\mpd.xml";
#else
	const char* pFilename = "/sdcard/mpd.xml";
#endif

	VOLOGI("pBuffer=%lu, nLen=%lu", pBuffer, nLen);
	if (!nLen || !pBuffer)
	{
		return nResult;
	}

	pFile = fopen (pFilename,"wb");
	if (pFile!=NULL)
	{
		tmpsize = fwrite (pBuffer , 1 , nLen, pFile );
		fclose (pFile);
		nResult=VO_RET_SOURCE2_OK;
	}

	VOLOGI("tmpsize=%lu, result=%lu", tmpsize, nResult);
	return nResult;
}
#endif
VO_BOOL VO_MPD_Parser::Parse( VO_PBYTE pBuffer, VO_U32 uSize ,MpdStreamingMedia *manifest,VO_CHAR* pMpdPath)
{
	destroy();
	m_manifest = manifest;
	m_file_content_size =uSize;	
	m_file_content = new VO_CHAR[m_file_content_size+1];
	memcpy(m_file_content, (VO_CHAR*)pBuffer, m_file_content_size);
	m_file_content[m_file_content_size] = '\0';

	if( m_file_content[0] == (VO_CHAR)0xff && m_file_content[1] == (VO_CHAR)0xfe)
	{
#if defined _WIN32

		int size = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)m_file_content, -1, NULL, NULL, NULL, NULL );

		VO_CHAR *content_temp = NULL;
		content_temp = new VO_CHAR[ size + 1 ];
		memset( content_temp , 0 , size + 1 );

		WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)m_file_content, -1, content_temp, size, NULL, NULL );
		m_file_content_size = size;	

#elif defined LINUX || defined _IOS

		VO_CHAR *content_temp = new VO_CHAR[ m_file_content_size + 1 ];
		memset( content_temp , 0 , m_file_content_size + 1 );

		int SrcLen = vowcslen((vowchar_t *)m_file_content);

		VO_UnicodeToUTF8((vowchar_t *)m_file_content, SrcLen, content_temp, m_file_content_size);
#endif
		delete []m_file_content;
		m_file_content = content_temp;
	}
	if(parse_manifest2(pMpdPath)==VO_FALSE)
		return VO_FALSE;
	return VO_TRUE;
}

VO_VOID VO_MPD_Parser::destroy()
{
	//VOLOGE("Reader destroy1");
	if( m_file_content )
	{
		delete []m_file_content;
		m_file_content = 0;
	}
	Seg_item * ptr_item = m_ptr_FI_head;

	int i =0;
	while( ptr_item)//!= m_ptr_FI_tail )
	{
		Seg_item * ptr_temp = ptr_item;
		VOLOGR("1");
		ptr_item = ptr_item->ptr_next;
		i++;

		VOLOGR("delete Seg_item3 = %p,%p",ptr_temp,ptr_item);
		delete ptr_temp;
		VOLOGR("2");
		ptr_temp = NULL;
	}

	//  VOLOGE("Reader destroy2");
	m_ptr_FI_head = m_ptr_FI_tail = 0;
	Seg_item * ptr_item_sub = m_ptr_sub_FI_head;
	int ii =0;
	while( ptr_item_sub)//!= m_ptr_FI_tail )
	{
		Seg_item * ptr_temp = ptr_item;
		VOLOGR("2");
		ptr_item_sub = ptr_item_sub->ptr_next;
		ii++;
		VOLOGR("delete sub_item3 = %p,%p",ptr_temp,ptr_item);
		delete ptr_temp;
		VOLOGR("3");
		ptr_temp = NULL;
	}
   	m_ptr_sub_FI_head = m_ptr_sub_FI_tail = 0;
	video_flag = 0;
	audio_flag = 0;
	subtitle_flag = 0;
	//Flush list
	FlushAVList();
	FlushTAGList();
	


}

VO_VOID VO_MPD_Parser::hexstr2byte( VO_PBYTE ptr_hex , VO_CHAR * str_hex, VO_S32 length )
{
	for( VO_S32 i = 0 ; i < length ; i++ )
	{
		VO_CHAR temp[3];

		temp[0] = str_hex[ 2*i ];
		temp[1] = str_hex[ 2*i + 1 ];
		temp[2] = '\0';

		int value;

		sscanf( temp , "%x" , &value );

		ptr_hex[i] = value;
	}
}


void VO_MPD_Parser::CreateAVList(VO_U64 counts)
{
	DestroyAVList();

	Timeline_video = new VO_S64[counts];
	Timeline_audio = new VO_S64[counts];
	Duration_video = new VO_S64[counts];
	Duration_audio = new VO_S64[counts];
	Timeline_subtile = new VO_S64[counts];
	Duration_subtitle = new VO_S64[counts];
}
void VO_MPD_Parser::InitAVList()
{
	Timeline_video = NULL;
	Timeline_audio = NULL;
	Timeline_subtile = NULL;
	Duration_video = NULL;
	Duration_audio = NULL;
	Duration_subtitle = NULL;
}
void VO_MPD_Parser::FlushAVList()
{
	memset(Timeline_video, 0x00, sizeof(VO_S64)*COUNTS_NUMBER);
	memset(Timeline_audio, 0x00, sizeof(VO_S64)*COUNTS_NUMBER);
	memset(Timeline_subtile, 0x00, sizeof(VO_S64)*COUNTS_NUMBER);
	memset(Duration_video, 0x00, sizeof(VO_S64)*COUNTS_NUMBER);
	memset(Duration_audio, 0x00, sizeof(VO_S64)*COUNTS_NUMBER);
	memset(Duration_subtitle, 0x00, sizeof(VO_S64)*COUNTS_NUMBER);
	
}
void VO_MPD_Parser::FlushTAGList()
{
	VOLOGW("FlushTAGList");
	
	m_grouptag.Delete();
	m_period_tag.Delete();
	m_template_tag.Delete();
	m_rpe_tag.Delete();
	m_seg_base.Delete();
	m_seg_info.Delete();
	m_program.Delete();
	m_metrics.Delete();
	m_base_url_el.Reset();
	
	
}
void VO_MPD_Parser::DestroyAVList()
{
	if(Timeline_video) 
		delete []Timeline_video;
	if(Timeline_audio) 
		delete []Timeline_audio;
	if(Duration_video) 
		delete []Duration_video;
	if(Duration_audio) 
		delete []Duration_audio;

	InitAVList();
}

VO_BOOL VO_MPD_Parser::Is_Live()
{
	if (m_manifest && m_manifest->islive)
	{
		return VO_TRUE;
	}
	return VO_FALSE;
}
VO_BOOL VO_MPD_Parser::Is_Dynamic()
{
	if(mpd_tag.GetLiveType())
	{
		//if (m_manifest && m_manifest->islive)
			return VO_TRUE;
		/*else
			return VO_FALSE;*/
	}
	return VO_FALSE;
}
