#include "CManifestManager.h"
#include "voString.h"
#include "voOSFunc.h"
#include "voLog.h"

CManifestManager::CManifestManager(void)
{
	max_chunks = 0;
	max_bitrate = 0;
	min_bitrate = 0;
	last_QL_bitrate = 0;
	m_video_index = -1;
	m_audio_index = -1;
	cur_bitrate_index = 0;
	last_bitrate_index = 0;
	bitrate_changed_Video = VO_FALSE;
	bitrate_changed_Audio = VO_FALSE;
	m_playlist_duration = 0;
	m_bNeedReduceQualityLevel = VO_TRUE; // vo_true set the better-qualitylevel to highest level in the URL
	m_nReduceNum = 0;
	memset( m_url , 0 , sizeof(m_url) );
	memset( request_video , 0 , sizeof(request_video) );
	memset( request_audio , 0 , sizeof(request_audio) );
	m_nVideoChunk_id = 0;
	m_nAudioChunk_id = 0;

	num = 0;
	maxbitrate = 0;
	m_bStartBitrate = true;

	last_QL_bitrate = -1;

	m_pSSInfo_video = NULL;
	m_pSSInfo_audio = NULL;
}

CManifestManager::~CManifestManager(void)
{
}

VO_VOID CManifestManager::SetManifest( VO_CHAR * url )
{
	memset( m_url , 0 , sizeof(m_url) );
	strcpy( m_url , url );

	for(int i = 0;i<20;i++)
		replace(m_url," ","%20");

}
VO_VOID CManifestManager::replace(char *source,const char *sub,const char *rep )
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
VO_VOID CManifestManager::make_requesturl(VO_CHAR *requestUrl, VO_SOURCE2_OUTPUT_TYPE  streamIndex)
{
	VO_CHAR *base_url =requestUrl;//[1024];
	memset( base_url , 0 , strlen(base_url) );
	strcpy( base_url , m_url );
	replace(base_url,"manifest","");
	replace(base_url,"Manifest","");
	if(VO_SOURCE2_TT_AUDIO == streamIndex)
		strcat(base_url, m_pCurSSSInfo_audio->strUrl);
	if(VO_SOURCE2_TT_VIDEO == streamIndex)
		strcat(base_url, m_pCurSSSInfo_video->strUrl);
}

VO_VOID CManifestManager::destroy()
{
}

VO_VOID CManifestManager::close()
{
	destroy();

}

VO_VOID CManifestManager::get_absolute_path( VO_CHAR * str_absolute_path , VO_CHAR * str_filename , VO_CHAR * str_relative_path )
{
	if( strstr( str_filename , "http://" ) )
	{
		strcpy( str_absolute_path , str_filename );
	}
	else if( strstr( str_filename , "../" ) == str_filename )
	{
		VO_CHAR* ptr = str_filename;
		strcpy( str_absolute_path , str_relative_path );

		VO_CHAR * ptr_pos = strrchr( str_absolute_path , '/' );
		*ptr_pos = '\0';

		while( strstr( ptr , "../" ) == ptr )
		{
			ptr_pos = strrchr( str_absolute_path , '/' );
			*ptr_pos = '\0';

			ptr = ptr + 3;
		}

		strcat( str_absolute_path , "/" );
		strcat( str_absolute_path , ptr );
	}
	else if( strstr( str_filename , "./" ) == str_filename )
	{
		strcpy( str_absolute_path , str_relative_path );
		strcat( str_absolute_path , str_filename + 2 );
	}
	else
	{
		strcpy( str_absolute_path , str_relative_path );
		strcat( str_absolute_path , str_filename );
	}
}

VO_S64 CManifestManager::GetAudioStartTime()
{
	FragmentItem * ptr_temp = m_pPlaylist;

	while(ptr_temp)
	{
		if(ptr_temp->streamindex == m_audio_index)
		{
			return ptr_temp->starttime/GetTimeScaleMs();
		}

		ptr_temp = ptr_temp->ptr_next;
	}
	return 0;
}



VO_S32 CManifestManager::get_duration()
{
	
	VOLOGI( "get duration %d" , m_playlist_duration );
	return m_playlist_duration;

}


VO_S64 CManifestManager::FindFragmentStartTime( VO_S64 pos,int nIsVideo )
{

	FragmentItem * ptr_temp = m_pPlaylist;
	FragmentItem *tmpPtr = NULL;
	while(ptr_temp && ptr_temp->ptr_next)
	{
		VOLOGI("seektime %lld,now %lld,next %lld,bVideo:%d(%d)",pos,ptr_temp->starttime,ptr_temp->ptr_next->starttime, ptr_temp->nIsVideo, nIsVideo);
		if( ptr_temp->nIsVideo == nIsVideo)
		{
			if(pos >= ptr_temp->starttime)
			{
				tmpPtr = ptr_temp;
			}
			else if(pos < ptr_temp->starttime )
			{
				if(!tmpPtr) return 0;

				return tmpPtr->starttime;
			}
		}

		ptr_temp = ptr_temp->ptr_next;
	}

	return 0;
}

VO_S64 CManifestManager::set_pos( VO_S64 pos )
{
	VO_S64 start_time = 0;
	if(pos == 0) return 0;
	start_time = FindFragmentStartTime( pos, m_video_index ==-1?0 : 1) ;//0 audio, 1 video
	VOLOGI("start_time :%lld",start_time);
	return start_time;
}

VO_BOOL CManifestManager::GetFragmentItem( FragmentItem ** ptr_item,  VO_S64 start_time )
{
	FragmentItem * ptr_temp = m_pPlaylist;

	while(ptr_temp)
	{
		if(ptr_temp->starttime == start_time )
		{
			*ptr_item = ptr_temp;
			return VO_TRUE;
		}

		ptr_temp = ptr_temp->ptr_next;
	}
	return VO_FALSE;
}

VO_SOURCE2_SUBSTREAM_INFO * CManifestManager::GetSubStreamInfoByBitrate( VO_S32 bitrate )
{
	VO_SOURCE2_SUBSTREAM_INFO * ptr_item = NULL;

	if(m_video_index == -1) return NULL;

	ptr_item = m_pSSInfo_video->pSubStreamInfo;
	VO_U32 count = m_pSSInfo_video->nSubStreamCount;
	while(count--)
	{
		if( ptr_item->nBitrate == bitrate )
			return ptr_item;

		ptr_item = ptr_item++;
	}
	return NULL;
}
FragmentItem* CManifestManager::GetFirstChunk()
{
	return m_pPlaylist;
}
FragmentItem* CManifestManager::GetNextChunk(FragmentItem *ptr_item)
{
	while(ptr_item)
	{
		ptr_item = ptr_item->ptr_next;
		if(!ptr_item) return NULL;
		if(ptr_item->streamindex == m_video_index || ptr_item->streamindex == m_audio_index)
			break;
	}
	return ptr_item;
}
VO_S32 CManifestManager::pop_fragment( FragmentItem * ptr_item, VO_CHAR * ptr_path, VO_S64 last_network_bitrate,VO_S64 &last_fragment_duration )
{

	VO_S64 url_bitrate = 0;

	if(!(ptr_item->streamindex >= 0 && ptr_item->streamindex < STREAM_NUMBER))
		return -1;
	VO_S64 chunkid = 0;
	if( ptr_item->streamindex == m_video_index )
	{
		//url_bitrate = 230000;
		if( m_bStartBitrate) // first fragmentInfo: lowest bitrate
		{
			url_bitrate =m_pSSInfo_video->pSubStreamInfo->nBitrate; //min_bitrate;// max_bitrate;//
			m_pCurSSSInfo_video = m_pSSInfo_video->pSubStreamInfo;
			m_bStartBitrate = false;
		}
		else
		{
			url_bitrate = determine_bitrate(last_network_bitrate,last_fragment_duration);
		}
		
		if( last_QL_bitrate != url_bitrate )
			bitrate_changed_Video = VO_TRUE;
		//VOLOGI("ql_information: %lld, %lld, %d",last_QL_bitrate,url_bitrate,bitrate_changed_Video);

		last_QL_bitrate = url_bitrate;
		strcat( ptr_path , request_video );
		chunkid = m_nVideoChunk_id++;

		last_fragment_duration = ptr_item->duration;
		// url_bitrate , ptr_item->starttime,m_nVideoChunk_id++);
	}
	else
	{
		url_bitrate = m_pCurSSSInfo_audio->nBitrate;
		strcat( ptr_path , request_audio );//, url_bitrate , ptr_item->starttime,m_nAudioChunk_id++);
		chunkid = m_nAudioChunk_id++;
	}
	
	
	VO_CHAR c[10];
	memset(c,0x00,sizeof(c));
#if defined _WIN32
	strcat(c,"%I64d");
	//	strcat(base_url, "/QualityLevels(%I64d)/Fragments(");
#elif defined LINUX
	strcat(c,"%lld");
	//	strcat(base_url, "/QualityLevels(%lld)/Fragments(");
#elif defined _IOS
	strcat(c,"%lld");
#elif defined _MAC_OS
	strcat(c,"%lld");
#endif
	//VOLOGE("+url: %s",ptr_path);
	char str[255];
	memset(str,0x00,sizeof(str));
	sprintf(str,c,url_bitrate);
	replace(ptr_path,"{bitrate}",str);
	memset(str,0x00,sizeof(str));
	sprintf(str,c, ptr_item->starttime);
	replace(ptr_path,"{start time}",str);
	memset(str,0x00,sizeof(str));
	sprintf(str, c, chunkid);
	replace(ptr_path,"{chunk id}",str);
	//VOLOGE("+++++++++++++url: %s",ptr_path);
	return 1;

}


VO_S64 CManifestManager::determine_bitrate( VO_S64 last_network_bitrate ,VO_S64 duration )
{
	if( last_network_bitrate == -1 )
		return 0;

	gbitrate[num] =double( last_network_bitrate);
	gduration[num++ ]=double(duration);
	VO_SOURCE2_SUBSTREAM_INFO * ptr_item = NULL;
 	if(num>2)
 	{
 		num = 0;
 		double bbb = (gbitrate[0] + gbitrate[1] + gbitrate[2]) /1000.0;
 		double ddd = (gduration[0] + gduration[1] + gduration[2]) /10000000.0;
 		//VOLOGI("local time %f ,actual time %f",bbb,ddd );
 		if(bbb<= ddd *0.4)
 		{
 			VO_S64 bt = get_cur_bitrate();
 			VO_SOURCE2_SUBSTREAM_INFO *sss = m_pSSInfo_video->pSubStreamInfo;
			VO_U32 count = m_pSSInfo_video->nSubStreamCount;
 			int upgrade = 1;
 			if(bbb <= ddd * 0.2)	upgrade = 2;
 			while(count-- && upgrade)
 			{
 				if(bt  < sss->nBitrate)
 				{
 					upgrade --;
 					ptr_item = sss;
 				}
 				sss++;
 			}
 		}
 		else if(bbb>= ddd *0.8)
 		{
			VO_SOURCE2_SUBSTREAM_INFO *tmp_ptr_item = m_pSSInfo_video->pSubStreamInfo;
			VO_U32 count = m_pSSInfo_video->nSubStreamCount;
 			int upgrade = 1;
 			while(count-- &&  upgrade)
 			{
 				if(get_cur_bitrate()  > tmp_ptr_item->nBitrate)
 				{
 					upgrade --;
 					ptr_item = tmp_ptr_item;
 				}
 				tmp_ptr_item ++;
 			}
 		}
 		else
 			ptr_item = m_pCurSSSInfo_video;
 	
 	}
	if(!ptr_item)
		ptr_item = m_pCurSSSInfo_video;

// 	if(ptr_item != NULL)
// 	{
// 		VOLOGI("last speed %lld ,next calculate speed %lld",last_network_bitrate,ptr_item->nBitrate );
// 	}
// 	else
// 		VOLOGI("last speed %lld ,next calculate speed 0",last_network_bitrate );

	VO_S64 url_bitrate = ptr_item->nBitrate;	
	m_pCurSSSInfo_video = ptr_item;

	return url_bitrate;

}

VO_S64 CManifestManager::get_cur_bitrate()
{
	return m_pCurSSSInfo_video->nBitrate;
}
void CManifestManager::SetStreamIndex(VO_SOURCE2_OUTPUT_TYPE type,  VO_U32 nIndex)
{
	switch(type)
	{
	case VO_SOURCE2_TT_VIDEO:
		m_video_index = nIndex;
		break;
	case VO_SOURCE2_TT_AUDIO:
		m_audio_index = nIndex;
		break;
	default:
		break;
	}
}
void CManifestManager::SetStreamInfo(VO_SOURCE2_OUTPUT_TYPE type,  VO_SOURCE2_STREAM_INFO *pStream)
{
	switch(type)
	{
	case VO_SOURCE2_TT_VIDEO:
		m_pSSInfo_video = pStream;
		m_pCurSSSInfo_video = m_pSSInfo_video->pSubStreamInfo;
		make_requesturl(request_video,VO_SOURCE2_TT_VIDEO);
		break;
	case VO_SOURCE2_TT_AUDIO:
		m_pSSInfo_audio = pStream;
		m_pCurSSSInfo_audio = m_pSSInfo_audio->pSubStreamInfo;
		make_requesturl(request_audio,VO_SOURCE2_TT_AUDIO);
		break;
	default:
		break;
	}
}
void CManifestManager::SetPlaylist(FragmentItem *item)
{
	m_pPlaylist = item;
}

VO_SOURCE2_SUBSTREAM_INFO * CManifestManager::GetCurSubStreamInfo(VO_SOURCE2_OUTPUT_TYPE type)
{
	if(type == VO_SOURCE2_TT_AUDIO)
		return 	m_pCurSSSInfo_audio;
	if(type == VO_SOURCE2_TT_VIDEO)
		return m_pCurSSSInfo_video;
	return NULL;
}