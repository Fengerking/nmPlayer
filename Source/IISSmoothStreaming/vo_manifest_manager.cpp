#include "vo_manifest_manager.h"
#include "voString.h"
#include "voOSFunc.h"
#include "fortest.h"
#include "voLog.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifdef _IOS
#ifdef _ISS_SOURCE_
using namespace _ISS;
#endif
#endif

#define LEON_ADD_2

vo_manifest_manager::vo_manifest_manager(void)
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
	m_better_QualityLevel.bitrate = 999999999;
	m_nVideoChunk_id = 0;
	m_nAudioChunk_id = 0;

	num = 0;
	maxbitrate = 0;
	m_bStartBitrate = true;

	last_QL_bitrate = -1;

	 m_ppPlusSample = NULL;
	 m_pDRMInfo = NULL;
	m_manifest.piff.data = NULL;
	m_manifest.streamIndex = NULL; 
	m_manifest.streamCount = 0;


	m_pPlusProgramInfo = NULL;
}

vo_manifest_manager::~vo_manifest_manager(void)
{
	destroy();
	ReleaseProgramInfo();
}
VO_VOID vo_manifest_manager::replace(char *source,const char *sub,const char *rep )
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
VO_VOID vo_manifest_manager::make_requesturl(VO_CHAR *requestUrl, VO_S32 streamIndex)
{
	VO_CHAR *base_url =requestUrl;//[1024];
	memset( base_url , 0 , strlen(base_url) );
	strcpy( base_url , m_url );
	replace(base_url,"manifest","");
	replace(base_url,"Manifest","");
	strcat(base_url, m_manifest.streamIndex[streamIndex].url);
}
#ifdef _Amazon
VO_BOOL vo_manifest_manager::SetManifest( VO_PBYTE pBuffer, VO_U32 uSize)
{

    return parse_manifest( pBuffer, uSize);
}
#endif
VO_BOOL vo_manifest_manager::SetManifest( vo_http_stream *ptr_fragment, VO_CHAR * url )
{
	memset( m_url , 0 , sizeof(m_url) );
	strcpy( m_url , url );

	for(int i = 0;i<20;i++)
		replace(m_url," ","%20");
	
	if( !parse_manifest(ptr_fragment) )
		return VO_FALSE;

	make_requesturl(request_video,m_video_index);
	make_requesturl(request_audio,m_audio_index);

	return VO_TRUE;
}

VO_VOID vo_manifest_manager::destroy()
{
	if(m_manifest.piff.data) 
	{
		delete []m_manifest.piff.data;
		m_manifest.piff.data = NULL;
	}
	if(m_manifest.streamIndex)
	{
		if( m_video_index != -1)
		{
			QualityLevel * ptr_item = m_manifest.streamIndex[m_video_index].m_ptr_QL_head;
			while( ptr_item != NULL )
			{
				QualityLevel * ptr_temp = ptr_item;
				ptr_item = ptr_item->ptr_next;
				delete ptr_temp;
			}
			m_manifest.streamIndex[m_video_index].m_ptr_QL_head = m_manifest.streamIndex[m_video_index].m_ptr_QL_tail = NULL;
		}

		if(m_manifest.streamIndex)
		{
			delete []m_manifest.streamIndex;
			m_manifest.streamIndex = NULL;
			m_manifest.streamCount = 0;
		}
	}
	
}

VO_VOID vo_manifest_manager::close()
{
	destroy();
	ReleaseProgramInfo();
}

VO_VOID vo_manifest_manager::get_absolute_path( VO_CHAR * str_absolute_path , VO_CHAR * str_filename , VO_CHAR * str_relative_path )
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
void vo_manifest_manager::ReleaseProgramInfo()
{
	if(!m_pPlusProgramInfo)
		return ;
	for (VO_U32 i= 0;i<m_pPlusProgramInfo->uStreamCount; i++ )
	{
		VO_SOURCE2_STREAM_INFO *ppSInfo = m_pPlusProgramInfo->ppStreamInfo[i] ;
		for (VO_U32 n= 0; n < ppSInfo->uTrackCount; n++ )
		{
			VO_SOURCE2_TRACK_INFO *ppTInfo = ppSInfo->ppTrackInfo[n];
			if(ppTInfo->pHeadData)
				delete []ppTInfo->pHeadData;
			delete ppTInfo;
		}
		if(ppSInfo->uTrackCount)
			delete []ppSInfo->ppTrackInfo;
		delete ppSInfo;
	}
	if(m_pPlusProgramInfo->uStreamCount)
		delete []m_pPlusProgramInfo->ppStreamInfo;
	
	memset(m_pPlusProgramInfo, 0x00, sizeof(VO_SOURCE2_PROGRAM_INFO));
	delete m_pPlusProgramInfo;
	m_pPlusProgramInfo = NULL;
}
VO_BOOL vo_manifest_manager::CreateProgramInfo()
{
	VOLOGR("+CreateProgramInfo");
//delete programinfo
	ReleaseProgramInfo();

	if(m_manifest.streamCount <= 0)
		return VO_FALSE;
	m_pPlusProgramInfo = new VO_SOURCE2_PROGRAM_INFO;
	m_pPlusProgramInfo->uSelInfo = VO_SOURCE2_SELECT_DEFAULT;
	m_pPlusProgramInfo->sProgramType = m_manifest.islive? VO_SOURCE2_STREAM_TYPE_LIVE :VO_SOURCE2_STREAM_TYPE_VOD;
	m_pPlusProgramInfo->uProgramID = 1;
	sprintf(m_pPlusProgramInfo->strProgramName,"%s", "ISS");

	int videoCount = 0;
	int audioCount = 0;
	m_nStreamCount = 0;
	for(VO_U32 i = 0;i < m_manifest.streamCount; i++ )
	{
		VO_CHAR *type = m_manifest.streamIndex[i].type;
		if( m_reader.StrCompare( type, "video") ==0)
		{
			videoCount += m_manifest.streamIndex[i].QL_Number;
		}
		else if(m_reader.StrCompare(type, "audio") == 0)
		{
			audioCount += 1;//m_manifest.streamIndex[i].audio_QL[0] ? 1 :0);
		}
	}
	m_nStreamCount = videoCount + 1;//add pure audio stream;

	m_pPlusProgramInfo->uStreamCount = m_nStreamCount;
	m_pPlusProgramInfo->ppStreamInfo = new VO_SOURCE2_STREAM_INFO* [m_nStreamCount];
	memset(m_pPlusProgramInfo->ppStreamInfo, 0x00, m_nStreamCount);
	for(VO_U32 n = 0; n < m_nStreamCount ;n++)
	{
		VO_SOURCE2_STREAM_INFO *pStreamInfo = m_pPlusProgramInfo->ppStreamInfo[n] = new VO_SOURCE2_STREAM_INFO;
		memset(pStreamInfo, 0x00, sizeof(VO_SOURCE2_STREAM_INFO));
	
		pStreamInfo->uTrackCount = audioCount;
		//last stream in pure audio stream
		//other is 1 video + mulitAudio
		if(n != m_nStreamCount -1)
			pStreamInfo->uTrackCount += 1;

		pStreamInfo->ppTrackInfo = new VO_SOURCE2_TRACK_INFO* [pStreamInfo->uTrackCount];
		memset(pStreamInfo->ppTrackInfo, 0x00 , pStreamInfo->uTrackCount);
		for(VO_U32 l =0; l < pStreamInfo->uTrackCount; l ++)
		{
			pStreamInfo->ppTrackInfo[l] = new VO_SOURCE2_TRACK_INFO;
			memset(pStreamInfo->ppTrackInfo[l], 0x00, sizeof(VO_SOURCE2_TRACK_INFO));
		}
		pStreamInfo->uTrackCount = 0;
	}


	
	int count = 0;
	for(VO_U32 i =0 ; i < m_manifest.streamCount; i++)
 	{
		VO_CHAR *type = m_manifest.streamIndex[i].type;
		if( m_reader.StrCompare( type, "video") ==0)
 		{
			QualityLevel * ql = m_manifest.streamIndex[i].m_ptr_QL_head;
			while(ql)
			{
				VO_SOURCE2_STREAM_INFO *pStreamInfo = m_pPlusProgramInfo->ppStreamInfo[count++];
				pStreamInfo->uSelInfo	= (count== 1? VO_SOURCE2_SELECT_DEFAULT: VO_SOURCE2_SELECT_SELECTABLE);
				pStreamInfo->uStreamID	= ql->index_QL;
				pStreamInfo->uBitrate	+= (VO_U32)ql->bitrate;

				VO_SOURCE2_TRACK_INFO *pTrackInfo = pStreamInfo->ppTrackInfo[pStreamInfo->uTrackCount] ;
				pTrackInfo->uTrackID	=	pStreamInfo->uTrackCount;
				pStreamInfo->uTrackCount++;

				pTrackInfo->uSelInfo	=	VO_SOURCE2_SELECT_DEFAULT;//VO_SOURCE2_SELECT_DEFAULT;
				memcpy(pTrackInfo->strFourCC , ql->fourCC, sizeof(pTrackInfo->strFourCC));
				pTrackInfo->uTrackType	=	VO_SOURCE2_TT_VIDEO;
				pTrackInfo->uCodec		=	m_manifest.streamIndex[i].nCodecType;
				pTrackInfo->uDuration	=	m_manifest.duration *1000/ m_manifest.timeScale;
				pTrackInfo->uChunkCounts=	m_manifest.streamIndex[i].chunksNumber;
				pTrackInfo->uBitrate	=	(VO_U32)ql->bitrate;
				pTrackInfo->sVideoInfo.sFormat.Height = ql->video_info.Height;
				pTrackInfo->sVideoInfo.sFormat.Width = ql->video_info.Width;
				pTrackInfo->uHeadSize = ql->length_CPD;
				pTrackInfo->pHeadData = new VO_BYTE[pTrackInfo->uHeadSize+1];
				memcpy(pTrackInfo->pHeadData, ql->codecPrivateData, pTrackInfo->uHeadSize);

				ql = ql->ptr_next;
			}
			continue;
		}
		else if( m_reader.StrCompare( type, "audio") ==0)
		{
  			QualityLevel_Audio * ql = &m_manifest.streamIndex[i].audio_QL[0];
  			for(VO_U32 j =0; j < m_pPlusProgramInfo->uStreamCount; j ++)
  			{
				VO_SOURCE2_STREAM_INFO *pStreamInfo2 = m_pPlusProgramInfo->ppStreamInfo[j];

  				pStreamInfo2->uBitrate	+= (VO_U32)ql->bitrate;
  		
				VOLOGR("pStreamInfo2->uTrackCount %d",pStreamInfo2->uTrackCount);

   				VO_SOURCE2_TRACK_INFO *pTrackInfo = pStreamInfo2->ppTrackInfo[pStreamInfo2->uTrackCount] ;
   				pTrackInfo->uTrackID	= pStreamInfo2->uTrackCount;
				pStreamInfo2->uTrackCount ++;

   				pTrackInfo->uSelInfo	= VO_SOURCE2_SELECT_DEFAULT;
   				memcpy(pTrackInfo->strFourCC , ql->fourCC, sizeof(pTrackInfo->strFourCC));
   				pTrackInfo->uTrackType	= VO_SOURCE2_TT_AUDIO;
   				pTrackInfo->uCodec		= m_manifest.streamIndex[i].nCodecType;
   				pTrackInfo->uDuration	= m_manifest.duration *1000/m_manifest.timeScale;
   				pTrackInfo->uChunkCounts= m_manifest.streamIndex[i].chunksNumber;
   				pTrackInfo->uBitrate	= (VO_U32) ql->bitrate;
   				pTrackInfo->sAudioInfo.sFormat.Channels = ql->audio_info.Channels;
   				pTrackInfo->sAudioInfo.sFormat.SampleBits = ql->audio_info.SampleBits;
   				pTrackInfo->sAudioInfo.sFormat.SampleRate = ql->audio_info.SampleRate;
   				pTrackInfo->uHeadSize = ql->length_CPD;
				VOLOGR("pTrackInfo->uHeadSize %d", pTrackInfo->uHeadSize);
   				pTrackInfo->pHeadData = new VO_BYTE[pTrackInfo->uHeadSize+1];
   				memcpy(pTrackInfo->pHeadData, ql->codecPrivateData, pTrackInfo->uHeadSize);
  			}
 		}
	}
	VOLOGR("-CreateProgramInfo");
	return VO_TRUE;
}
#ifdef _Amazon
VO_BOOL vo_manifest_manager::parse_manifest(VO_PBYTE pBuffer ,VO_U32 uSize)
{
	if(m_manifest.piff.data) {delete []m_manifest.piff.data; m_manifest.piff.data = NULL;}
	if(m_manifest.streamIndex){delete []m_manifest.streamIndex; m_manifest.streamIndex = NULL; m_manifest.streamCount = 0;}
	m_video_index = -1; m_audio_index = -1;

	if( !m_reader.Parse( pBuffer, uSize, &m_manifest) )
			return VO_FALSE;
	if(m_pDRMInfo) 
	{
		delete []m_pDRMInfo->strData;
		m_pDRMInfo = NULL;
	}
	if(m_ppPlusSample)
	{
		for (VO_U32 i= 0;i<m_nStreamCount; i++ )
		{
			VO_SOURCE2_STREAM_INFO *info = m_ppPlusSample[i] ;
			for (VO_U32 n= 0;n<info->nSubStreamCount; n++ )
			{
				VO_SOURCE2_SUBSTREAM_INFO *ssinfo = info->pSubStreamInfo;
				delete []ssinfo->pTrackInfo;
				ssinfo ++;
			}
			delete []info->pSubStreamInfo;
			delete info;
		}
		delete []m_ppPlusSample;
		m_nStreamCount = 0;
	}
	if(m_manifest.piff.isUsePiff)
	{
		m_pDRMInfo = new VO_SOURCE2_DRM_INFO ;
		memcpy(m_pDRMInfo->strSystemID, m_manifest.piff.systemID, sizeof( m_manifest.piff.systemID));
		m_pDRMInfo->nDataSize = m_manifest.piff.dataSize;
		m_pDRMInfo->strData = new VO_BYTE[m_pDRMInfo->nDataSize ];
		memcpy(m_pDRMInfo->strData, m_manifest.piff.data, m_pDRMInfo->nDataSize );
	}
	else
		m_pDRMInfo = NULL;

	if(m_manifest.streamCount <= 0)
		return VO_FALSE;

	m_nStreamCount = m_manifest.streamCount ;
	m_ppPlusSample = new VO_SOURCE2_STREAM_INFO *[m_nStreamCount];
	memset(m_ppPlusSample, 0x00, m_nStreamCount);
	//*m_ppPlusSample = pPlusSample;
	for(VO_U32 i =0 ;i< m_manifest.streamCount; i++)
	{
		VO_SOURCE2_STREAM_INFO *pp = m_ppPlusSample[i] = new VO_SOURCE2_STREAM_INFO;
		memset(pp, 0x00, sizeof(VO_SOURCE2_STREAM_INFO));
		VOLOGR("StreamCount:%d, StreamIndex:%d, Type:%s", m_manifest.streamCount, i, m_manifest.streamIndex[i].type);
		pp->nDuration = m_manifest.duration;
		memcpy(pp->strStreamName, m_manifest.streamIndex[i].type, sizeof(pp->strStreamName));
		pp->nStreamID= i;
		if( m_reader.StrCompare( m_manifest.streamIndex[i].type, "video") ==0)
		{ 
			QualityLevel *ql = m_manifest.streamIndex[i].m_ptr_QL_head;
			pp->nSubStreamCount = m_manifest.streamIndex[i].QL_Number;
			VO_SOURCE2_SUBSTREAM_INFO *ssi = pp->pSubStreamInfo = new VO_SOURCE2_SUBSTREAM_INFO [pp->nSubStreamCount];
			for(VO_U32 n =0; n< pp->nSubStreamCount; n++)
			{	
				ssi->nBitrate = ql->bitrate;
				memcpy(ssi->strUrl, m_manifest.streamIndex[i].url, sizeof(m_manifest.streamIndex[i].url));
				ssi->nChunkCounts =m_manifest.streamIndex[i].chunksNumber;

				ssi->nSubStreamID = ql->index_QL;
				ssi->nTrackCount = 1;

				VO_SOURCE2_TRACK_INFO *stf = ssi->pTrackInfo = ( VO_SOURCE2_TRACK_INFO * )new VO_BYTE[ sizeof( VO_SOURCE2_TRACK_INFO ) + ql->length_CPD ];
				stf->nTrackID = 1;
				memcpy(stf->strFourCC, ql->fourCC, sizeof(ql->fourCC));
				stf->nCodec = m_manifest.streamIndex[i].nCodecType;
				stf->nHeadSize = ql->length_CPD;
				stf->pHeadData = stf->Padding;
				if(stf->nHeadSize > 0)
					memcpy(stf->pHeadData, ql->codecPrivateData, ql->length_CPD);
				stf->VideoInfo.Format.Height = ql->video_info.Height;
				stf->VideoInfo.Format.Width = ql->video_info.Width;
				stf->nTrackType = VO_SOURCE2_TT_VIDEO;
				ql = ql->ptr_next;
				ssi ++;
			}
		}
		if( m_reader.StrCompare( m_manifest.streamIndex[i].type, "audio") ==0 )
		{
			for(int x = 0; x<10;x ++)
			{
				QualityLevel_Audio *ql  = &m_manifest.streamIndex[i].audio_QL[x];
				if(ql && ql->bitrate >0) 	pp->nSubStreamCount ++;
			}

			VO_SOURCE2_SUBSTREAM_INFO *ssi = pp->pSubStreamInfo = new VO_SOURCE2_SUBSTREAM_INFO[pp->nSubStreamCount];
		

			for(VO_U32 n =0; n< pp->nSubStreamCount; n++)
			{
				QualityLevel_Audio  *ql = &m_manifest.streamIndex[i].audio_QL[n] ;
				memcpy(ssi->strUrl, m_manifest.streamIndex[i].url, sizeof(m_manifest.streamIndex[i].url));
				ssi->nChunkCounts =m_manifest.streamIndex[i].chunksNumber;
				ssi->nSubStreamID = 0;

				ssi->nBitrate = ql->bitrate;			
				VO_SOURCE2_TRACK_INFO *stf = ssi->pTrackInfo = ( VO_SOURCE2_TRACK_INFO * )new VO_BYTE[ sizeof( VO_SOURCE2_TRACK_INFO ) + ql->length_CPD ];
				memcpy(stf->strFourCC, ql->fourCC, sizeof(stf->strFourCC));
				stf->nCodec = m_manifest.streamIndex[i].nCodecType;
				stf->nHeadSize = ql->length_CPD;
				stf->pHeadData = stf->Padding;
				if(stf->nHeadSize > 0)
					memcpy(stf->pHeadData, ql->codecPrivateData, ql->length_CPD);
				stf->AudioInfo.Format.Channels= ql->audio_info.Channels;
				stf->AudioInfo.Format.SampleBits = ql->audio_info.SampleBits;
				stf->AudioInfo.Format.SampleRate = ql->audio_info.SampleRate;
				memcpy(stf->AudioInfo.Language, "", sizeof(stf->AudioInfo.Language));
				stf->nTrackType = VO_SOURCE2_TT_AUDIO;
				ssi++;
			}
		}
	}


	return VO_TRUE;
}
#endif
VO_BOOL vo_manifest_manager::parse_manifest( vo_http_stream *ptr_fragment )
{
	if(m_manifest.piff.data) {delete []m_manifest.piff.data; m_manifest.piff.data = NULL;}
	if(m_manifest.streamIndex){delete []m_manifest.streamIndex; m_manifest.streamIndex = NULL; m_manifest.streamCount = 0;}
	m_video_index = -1; m_audio_index = -1;
	
	VOLOGR("1");
	if( !m_reader.Parse( ptr_fragment, m_url, &m_manifest) )
		return VO_FALSE;

	CreateProgramInfo();

	VOLOGR("2");
	int kid = 0;
	int count =0;
	for(VO_U32 i =0 ;i< m_manifest.streamCount; i++)
	{
		VOLOGR("StreamCount:%d, StreamIndex:%d, Type:%s", m_manifest.streamCount, i, m_manifest.streamIndex[i].type);
		if( m_reader.StrCompare( m_manifest.streamIndex[i].type, "video") ==0 && !(kid &0x01))
		{
			m_video_index = i;
			kid |= 0x01;

			QualityLevel * ql = m_manifest.streamIndex[i].m_ptr_QL_head;
			for(int x = 0; x < m_manifest.streamIndex[i].QL_Number; x++)
			{
				m_BitrateMap[ql->index_QL].SetBitrate(ql->index_QL,(int)ql->bitrate,ql->video_info.Width, ql->video_info.Height);			
				ql = ql->ptr_next;
			}
		}
		if( m_reader.StrCompare( m_manifest.streamIndex[i].type, "audio") ==0 && !(kid & 0x02))
		{
			m_audio_index = i;
			kid |= 0x02;
		}
	}

	if( m_video_index != -1)
	{
		max_chunks = m_manifest.streamIndex[m_video_index].chunksNumber + m_manifest.streamIndex[m_audio_index].chunksNumber;
		if( m_manifest.streamIndex[m_video_index].m_ptr_QL_head != NULL)
		{
			max_bitrate = get_qualityLevel(0)->bitrate;
			min_bitrate = get_qualityLevel(m_manifest.streamIndex[m_video_index].QL_Number - 1)->bitrate;
		}
	}

	m_playlist_duration = (VO_S32)( m_manifest.duration  / GetTimeScaleMs());
	
	ptr_fragment->closedownload();
	return VO_TRUE;
}

VO_S64 vo_manifest_manager::get_audiostarttime()
{
	FragmentItem * ptr_temp = m_reader.m_ptr_FI_head;

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



VO_S32 vo_manifest_manager::get_duration()
{
	if( is_live() )
	{
		VOLOGR( "get duration 0" );
		return 0;
	}
	else
	{
		VOLOGR( "get duration %d" , m_playlist_duration );
		return m_playlist_duration;
	}
}


VO_S64 vo_manifest_manager::set_pos( VO_S64 pos )
{
	VO_S64 start_time = 0;
	if(pos == 0) return 0;
	start_time = m_reader.find_fragment_starttime( pos, m_video_index ==-1?0 : 1) ;//0 audio, 1 video
	VOLOGR("start_time :%lld",start_time);
	return start_time;
}

QualityLevel * vo_manifest_manager::get_qualityLevel( VO_S32 index_item )
{
	QualityLevel * ptr_item = NULL;

	if(m_video_index == -1) return NULL;

	ptr_item = m_manifest.streamIndex[m_video_index].m_ptr_QL_head;

	while(ptr_item)
	{
		if( ptr_item->index_QL == index_item )
			return ptr_item;

		ptr_item = ptr_item->ptr_next;
	}
	return NULL;
}
FragmentItem* vo_manifest_manager::GetFirstChunk()
{
	return m_reader.m_ptr_FI_head;
}
FragmentItem* vo_manifest_manager::GetNextChunk(FragmentItem *ptr_item)
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
VO_S32 vo_manifest_manager::pop_fragment( FragmentItem * ptr_item, VO_CHAR * ptr_path, VO_S64 last_network_bitrate,VO_S64 &last_fragment_duration )
{

	VO_S64 url_bitrate = 0;

	if(ptr_item->streamindex < 0)// && ptr_item->streamindex < STREAM_NUMBER))
		return -1;
	VO_S64 chunkid = 0;
	if( ptr_item->streamindex == m_video_index )
	{
		//url_bitrate = 230000;
		if( m_bStartBitrate) // first fragmentInfo: lowest bitrate
		{
			url_bitrate =min_bitrate;// max_bitrate;//
			cur_bitrate_index = m_manifest.streamIndex[m_video_index].QL_Number/2;// - 1;//
			QualityLevel * ql = m_manifest.streamIndex[m_video_index].m_ptr_QL_tail;
			while(ql)
			{
				if(ql->index_QL == cur_bitrate_index)
				{
					url_bitrate = ql->bitrate;
					break;
				}
				ql = ql->ptr_pre;
			}
			last_bitrate_index = cur_bitrate_index;
			m_bStartBitrate = false;
			VO_S64 x  = -1;
			url_bitrate = determine_bitrate(last_network_bitrate,x);

		}
		else
		{
			url_bitrate = determine_bitrate(last_network_bitrate,last_fragment_duration);
		}
		if(url_bitrate == -1)
			url_bitrate= last_QL_bitrate ;
		if( last_QL_bitrate != url_bitrate )
			bitrate_changed_Video = VO_TRUE;
		VOLOGR("ql_information: %lld, %lld, %d",last_QL_bitrate,url_bitrate,bitrate_changed_Video);

		last_QL_bitrate = url_bitrate;
		strcat( ptr_path , request_video );
		chunkid = m_nVideoChunk_id++;

		last_fragment_duration = ptr_item->duration;
		// url_bitrate , ptr_item->starttime,m_nVideoChunk_id++);
	}
	else
	{
		url_bitrate = m_manifest.streamIndex[m_audio_index].audio_QL[0].bitrate;
		strcat( ptr_path , request_audio );//, url_bitrate , ptr_item->starttime,m_nAudioChunk_id++);
		chunkid = m_nAudioChunk_id++;
	}
	
	
	VO_CHAR c[10];
	memset(c,0x00,sizeof(c));
#if defined _WIN32
	strcat(c,"%lld");
	//	strcat(base_url, "/QualityLevels(%I64d)/Fragments(");
#elif defined LINUX
	strcat(c,"%llu");
	//	strcat(base_url, "/QualityLevels(%lld)/Fragments(");
#elif defined _IOS
	strcat(c,"%lld");
#elif defined _MAC_OS
	strcat(c,"%lld");
#endif
	//VOLOGR("+url: %s",ptr_path);
	char str[255];
	memset(str,0x00,sizeof(str));
	sprintf(str,c,url_bitrate);
	replace(ptr_path,"{bitrate}",str);
	memset(str,0x00,sizeof(str));
// 	if(ptr_item->starttime ==18446744071587064320)
// 		int x =0;
	sprintf(str,c, ptr_item->starttime);
	replace(ptr_path,"{start time}",str);
	memset(str,0x00,sizeof(str));
	sprintf(str, c, chunkid);
	replace(ptr_path,"{chunk id}",str);
	VOLOGR("+++++++++++++url: %s,%lld",ptr_path,ptr_item->starttime);
	return 1;

}



VO_VOID vo_manifest_manager::TooHightQualityLevel()
{
	m_nReduceNum ++;
	if(m_nReduceNum > 10)
	{
		m_bNeedReduceQualityLevel = VO_TRUE;
		m_nReduceNum =0;
		VOLOGR("TooHightqualitylevel for player ,set down a level");
	}
}

VO_VOID vo_manifest_manager::CheckBetterQL()
{
	VOLOGR("+CheckBetterQL");
	//	VOLOGR("+reduce_QualityLevel bitrate(%lld),index(%d) ",m_better_QualityLevel.bitrate,m_better_QualityLevel.index_QL);
	QualityLevel * ptr_item = NULL;

	ptr_item = m_manifest.streamIndex[m_video_index].m_ptr_QL_tail;
	memcpy(&m_better_QualityLevel,ptr_item,sizeof(QualityLevel));

	while(ptr_item)
	{
		VOLOGR("BetterQL:%lld",m_better_QualityLevel.bitrate);
		if(!GetIsSmoothPlayed(ptr_item->index_QL))
			break;

		memcpy(&m_better_QualityLevel,ptr_item,sizeof(QualityLevel));
		ptr_item = ptr_item->ptr_pre;
	}
	VOLOGR("-CheckBetterQL");
}
VO_VOID vo_manifest_manager::reduce_QualityLevel()
{
	
//	VOLOGR("+reduce_QualityLevel bitrate(%lld),index(%d) ",m_better_QualityLevel.bitrate,m_better_QualityLevel.index_QL);
	QualityLevel * ptr_item = NULL;

	ptr_item = m_manifest.streamIndex[m_video_index].m_ptr_QL_head;

	while(ptr_item)
	{
		if(m_better_QualityLevel.bitrate > ptr_item->bitrate)
		{
			m_nReduceNum =0;
			memcpy(&m_better_QualityLevel,ptr_item,sizeof(QualityLevel));
			break;
		}
		ptr_item = ptr_item->ptr_next;
	}
	//VOLOGR("-reduce_QualityLevel bitrate(%lld),index(%d)",m_better_QualityLevel.bitrate,m_better_QualityLevel.index_QL);
}

#ifdef LEON_ADD_2
VO_S64 vo_manifest_manager::determine_bitrate( VO_S64 last_network_bitrate ,VO_S64 duration ){

	if( last_network_bitrate == -1 )
		return 0;

	QualityLevel  * ptr_item = NULL;
	
	if(duration != -1)
	{
		gbitrate[num] = last_network_bitrate;
		gduration[num++] = duration;

		if(num>2)
		{
			num = 0;
			VO_S64 bbb = (gbitrate[0] + gbitrate[1] + gbitrate[2])/3;

			ptr_item =  m_manifest.streamIndex[m_video_index].m_ptr_QL_tail;
			QualityLevel  *temp = ptr_item;
			while(ptr_item )
			{
				if(bbb > ptr_item->bitrate*1.5)
				{
					temp = ptr_item;
					ptr_item = ptr_item->ptr_pre;
				}
				else
				{
					break;
				}
			}
			ptr_item = temp;
			VOLOGI("BA. 1th Step. SelectedBitrate:%lld. CurrentNetSpeed:%lld ",ptr_item->bitrate,bbb)
		}
	}
	if(!ptr_item)
		ptr_item = get_qualityLevel( cur_bitrate_index); //the first ; //the birate of the index

#ifdef _NEW_SOURCEBUFFER
	CheckBetterQL();
#else

	if(m_bNeedReduceQualityLevel)reduce_QualityLevel();
	m_bNeedReduceQualityLevel = VO_FALSE;
#endif
	VO_S64 url_bitrate = ptr_item->bitrate;
	if(url_bitrate > m_better_QualityLevel.bitrate)
	{
		url_bitrate = m_better_QualityLevel.bitrate;
		cur_bitrate_index = m_better_QualityLevel.index_QL;
	}
	else
	{
		cur_bitrate_index = ptr_item->index_QL;
	}

	VOLOGI("BA. Final Step. SelectedBitrate:%lld,  RunTime MaxBitrate:%lld",url_bitrate,m_better_QualityLevel.bitrate);
	VOLOGI("BAInfo*** Run Cap %d " , (VO_U32)m_better_QualityLevel.bitrate );
	last_bitrate_index = cur_bitrate_index;

	if(m_pPlusProgramInfo)
	{
		for(VO_U32 n = 0; n < m_pPlusProgramInfo->uStreamCount;n++)
		{
			VO_SOURCE2_STREAM_INFO *pssInfo = m_pPlusProgramInfo->ppStreamInfo[n];
			if(!pssInfo)
				continue;
			if(pssInfo->uStreamID == cur_bitrate_index)
				pssInfo->uSelInfo = VO_SOURCE2_SELECT_SELECTED;
			else
				pssInfo->uSelInfo = VO_SOURCE2_SELECT_SELECTABLE;
		}
	}
	return url_bitrate;
}
#else
VO_S64 vo_manifest_manager::determine_bitrate( VO_S64 last_network_bitrate ,VO_S64 duration )
{
	if( last_network_bitrate == -1 )
		return 0;

	gbitrate[num] =double( last_network_bitrate);
	gduration[num++ ]=double(duration);
	QualityLevel * ptr_item = NULL;
 	if(num>2)
 	{
 		num = 0;
 		double bbb = (gbitrate[0] + gbitrate[1] + gbitrate[2]) /1000.0;
 		double ddd = (gduration[0] + gduration[1] + gduration[2]) /10000000.0;
 		VOLOGR("local time %f ,actual time %f",bbb,ddd );
 		if(bbb<= ddd *0.4)
 		{
 			VO_S64 bt = get_cur_bitrate();
 			QualityLevel *tmp_ptr_item = m_manifest.streamIndex[m_video_index].m_ptr_QL_tail;
 			int upgrade = 1;
 			if(bbb <= ddd * 0.2)	upgrade = 2;
 			while(tmp_ptr_item && upgrade)
 			{
 				if(bt  < tmp_ptr_item->bitrate)
 				{
 					upgrade --;
					ptr_item = tmp_ptr_item;
 				}
 				tmp_ptr_item = tmp_ptr_item->ptr_pre;
 			}
 		}
 		else if(bbb>= ddd *0.8)
 		{
 			QualityLevel* tmp_ptr_item = m_manifest.streamIndex[m_video_index].m_ptr_QL_head;
 			int upgrade = 1;
 			while(tmp_ptr_item && upgrade)
 			{
 				if(get_cur_bitrate()  > tmp_ptr_item->bitrate)
 				{
 					upgrade --;
 					ptr_item = tmp_ptr_item;
 				}
 				tmp_ptr_item = tmp_ptr_item->ptr_next;
 			}
 		}
 		else
 			ptr_item = get_qualityLevel( cur_bitrate_index);
 	
 	}
	if(!ptr_item)
		ptr_item = get_qualityLevel( cur_bitrate_index);
//	if( ! ( ptr_item = get_qualityLevel( cur_bitrate_index)) )
//		return 0;
// 	if( last_network_bitrate >= get_cur_bitrate() )
// 	{
// 		while( ptr_item->ptr_pre && last_network_bitrate > ( ptr_item->ptr_pre->bitrate + get_cur_bitrate() ) * 1.01 )
// 		{
// 			ptr_item = ptr_item->ptr_pre;
// 		}
// 	}
// 	else
// 	{
// 		while( last_network_bitrate < ptr_item->bitrate && ptr_item->ptr_next )
// 		{
// 			ptr_item = ptr_item->ptr_next;
// 		}
// 	}
	if(ptr_item != NULL)
	{
		VOLOGR("last speed %lld ,next calculate speed %lld",last_network_bitrate,ptr_item->bitrate );
	}
	else
		VOLOGR("last speed %lld ,next calculate speed 0",last_network_bitrate );

#ifdef _NEW_SOURCEBUFFER
	CheckBetterQL();
#else

	if(m_bNeedReduceQualityLevel)reduce_QualityLevel();
	m_bNeedReduceQualityLevel = VO_FALSE;
#endif
	VO_S64 url_bitrate = ptr_item->bitrate;
	if(url_bitrate > m_better_QualityLevel.bitrate)
	{
		url_bitrate = m_better_QualityLevel.bitrate;
		cur_bitrate_index = m_better_QualityLevel.index_QL;
	}
	else
	{
		cur_bitrate_index = ptr_item->index_QL;
	}
	VOLOGR("last speed %lld now netspeed %lld,  better_QualityLevel %lld",last_network_bitrate,url_bitrate,m_better_QualityLevel.bitrate );
	last_bitrate_index = cur_bitrate_index;

	return url_bitrate;

}
#endif
// VO_S64 vo_manifest_manager::determine_bitrate( VO_S64 last_network_bitrate ,VO_S64 duration )
// {
// 	if( last_network_bitrate == -1 )
// 		return 0;
// 
// 	QualityLevel *tmp_ptr_item = m_manifest.streamIndex[m_video_index].m_ptr_QL_head;
// 	QualityLevel *ptr_item  =NULL;
// 	VO_U64 rightone =0;
// 	while(tmp_ptr_item)
// 	{
// 		VO_U64 br = tmp_ptr_item->bitrate;
// 		if(br<rightone &&(!maxbitrate || br < maxbitrate) && last_network_bitrate *duration /DEFAULT_TIME_SCALE *0.5 >= tmp_ptr_item->bitrate)
// 		{
// 			ptr_item = tmp_ptr_item;
// 			rightone = tmp_ptr_item->bitrate;
// 		}
// 		tmp_ptr_item = tmp_ptr_item->ptr_next;
// 	}
// 	if (!ptr_item) /* If can't find any, pick the smallest */
// 	{
// 		ptr_item =tmp_ptr_item= m_manifest.streamIndex[m_video_index].m_ptr_QL_head;
// 		while(tmp_ptr_item)
// 		{
// 			VO_U64 br = tmp_ptr_item->bitrate;
// 			if(br<ptr_item->bitrate)
// 				ptr_item = tmp_ptr_item;
// 			tmp_ptr_item = tmp_ptr_item->ptr_next;
// 		}
// 		//maxbitrate = ptr_item->bitrate;
// 		//f->maxbitrate = rightone; /* so that this won't happen again */
// 	}
// //	if( ! ( ptr_item = get_qualityLevel( cur_bitrate_index)) )
// //		return 0;
// // 	if( last_network_bitrate >= get_cur_bitrate() )
// // 	{
// // 		while( ptr_item->ptr_pre && last_network_bitrate > ( ptr_item->ptr_pre->bitrate + get_cur_bitrate() ) * 1.01 )
// // 		{
// // 			ptr_item = ptr_item->ptr_pre;
// // 		}
// // 	}
// // 	else
// // 	{
// // 		while( last_network_bitrate < ptr_item->bitrate && ptr_item->ptr_next )
// // 		{
// // 			ptr_item = ptr_item->ptr_next;
// // 		}
// // 	}
// 	if(ptr_item != NULL)
// 	{
// 		VOLOGR("last speed %lld ,next calculate speed %lld",last_network_bitrate,ptr_item->bitrate );
// 	}
// 	else
// 		VOLOGR("last speed %lld ,next calculate speed 0",last_network_bitrate );
// 
// 	if(m_bNeedReduceQualityLevel)reduce_QualityLevel();
// 	m_bNeedReduceQualityLevel = VO_FALSE;
// 	VO_S64 url_bitrate = ptr_item->bitrate;
// 	if(url_bitrate > m_better_QualityLevel.bitrate)
// 	{
// 		url_bitrate = m_better_QualityLevel.bitrate;
// 		cur_bitrate_index = m_better_QualityLevel.index_QL;
// 	}
// 	else
// 	{
// 		cur_bitrate_index = ptr_item->index_QL;
// 	}
// 	VOLOGR("last speed %lld now netspeed %lld,  better_QualityLevel %lld",last_network_bitrate,url_bitrate,m_better_QualityLevel.bitrate );
// 	last_bitrate_index = cur_bitrate_index;
// 
// 	return url_bitrate;
// 
// }

VO_S64 vo_manifest_manager::get_cur_bitrate()
{
	QualityLevel *ql = get_qualityLevel(cur_bitrate_index);
	if(ql)
		return ql->bitrate;
	return 0;
}

VO_U32 vo_manifest_manager::GetStreamCount()
{
	return m_manifest.streamCount;
}

VO_U32 vo_manifest_manager::GetStreamInfo  (VO_U32 nStreamID, VO_SOURCE2_STREAM_INFO **ppStreamInfo)
{
	/*for(VO_U32 i = 0; i< m_nStreamCount; i++)
	{
		if(nStreamID == m_ppPlusSample[i]->nStreamID)
		{
			*ppStreamInfo = m_ppPlusSample[i];
			return VO_ERR_LIVESRC_PLUS_OK;
		}
	}*/

	return VO_ERR_LIVESRC_PLUS_FAIL;
}

VO_U32 vo_manifest_manager::GetDrmInfo  ( VO_SOURCE2_DRM_INFO **ppDrmInfo)
{
	*ppDrmInfo = m_pDRMInfo;
	return VO_ERR_LIVESRC_PLUS_OK;
}

void vo_manifest_manager::AddtimeStamp2Map( VO_U64 timeStamp, int nFlag/* = 0*/)
{
	m_BitrateMap[cur_bitrate_index].AddtimeStamp2Map(timeStamp,nFlag);
}

int vo_manifest_manager::CheckDelayTimeStamp(int delayTime, VO_U64 timeStamp)
{
	if(m_video_index == -1)
		return 0;
	int counts = m_manifest.streamIndex[m_video_index].QL_Number;
	for(int i = 0; i< counts; i++)
	{
		if(m_BitrateMap[i].CheckDelayTimeStamp(delayTime,timeStamp) == 0)
			break;
	}
	return 0;
}

VO_BOOL vo_manifest_manager::GetIsSmoothPlayed(int index)
{
	return m_BitrateMap[index].GetIsSmoothPlayed();
}

void vo_manifest_manager::FlushMap()
{
	if(m_video_index == -1)
		return ;

	int counts = m_manifest.streamIndex[m_video_index].QL_Number;
	for(int i = 0; i< counts; i++)
	{
		m_BitrateMap[i].FlushMap();
	}
}

VO_U32 vo_manifest_manager::SetBACAP(VO_SOURCE2_CAP_DATA* pCap)
{
	if(pCap == NULL)
		return VO_ERR_INVALID_ARG;
	VOLOGI("BAInfo*** Max Cap %d , Width %d , Height %d" , pCap->nBitRate,pCap->nVideoWidth,pCap->nVideoHeight );

	for(int i= 0 ;i< 20;i++)
	{
		m_BitrateMap[i].SetBACAP(pCap);
	}
	return VO_ERR_NONE;
}

VO_U32 vo_manifest_manager::SetDrmCustomerType(VOSMTH_DRM_TYPE type)
{
	m_reader.SetDrmCustomerType(type);
	return VO_ERR_NONE;
}