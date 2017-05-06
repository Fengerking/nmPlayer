#include "vo_smooth_streaming.h"
#include "voLog.h"
#include "voOSFunc.h"
#include "voThread.h"
#include "fortest.h"
#ifdef _ISS_SOURCE_
#include "voVideo.h"
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifdef _IOS
#ifdef _ISS_SOURCE_
using namespace _ISS;
#endif
#endif

#define LEON_ADD_2
//2012.07.05 , anyone which audio or Video changed  must sent track info together.
//#define SENDNEWFORMAT_TOGETHER

//#define	_TEST
//#define DUMP_FILE
static FILE *out_video = NULL;
static FILE *out_audio = NULL;
static unsigned char CHANGE_CODE[]={ 0x01,0x02,0x03,0x04};


static const VO_CHAR SYNC_HEADER[] = {0x00,0x00,0x00,0x01};
#define be32toh(x) 	((x &0xff000000)>> 24 | (x & 0xff0000) >>16 << 8 | (x &0xff00) >>8 <<16 | (x & 0xff)<<24)

typedef VO_S32 ( VO_API *pvoGetSMTHParserAPI)(VO_PARSER_API * pParser);

vo_smooth_streaming::vo_smooth_streaming(void)
:m_judgementor(5)
,m_drmCustomerType(VOSMTH_DRM_COMMON_PlayReady)
{
	m_thread = 0;
	m_bMediaStop = VO_FALSE;
	m_datacallback_func = 0;
	m_eventcallback_func = 0;
	m_bRecoverFromPause = VO_FALSE;
	m_is_flush = VO_FALSE;
	m_is_pause = VO_FALSE;
	m_is_seek = VO_FALSE;
	m_is_H264 = VO_FALSE;
	m_is_first = VO_TRUE;
	download_bitrate = 0;
	m_lastFregmentDuration = 0;

	start_time = 0;
	seek_time = 0;
	m_seeked = VO_FALSE;
	m_bNeedFlushBuffer = VO_FALSE;
	m_pTrackInfoEx_Audio = NULL;
	m_pTrackInfoEx_Video = NULL;
	m_nLastTimeStamp_Video = 0;
	m_nLastTimeStamp_Audio = 0;
	m_nCurTimeStamp_Audio = 0;
	m_nCurTimeStamp_Video = 0;
	m_smthparser_handle = NULL;
	m_pDrmValue = NULL;
	m_audioCodecType = 0;
	m_videoCodecType = 0;
	m_isPushMode = VO_FALSE;
	
	m_nSyncTimeStamp = 0;
	m_bHasSyncTimeStamp = VO_FALSE;

	memset (&m_smthparser_api, 0, sizeof (VO_PARSER_API));
	strcpy (m_szWorkPath, "");

	m_manager.m_reader.SetXmlOp(&m_xmlLoad);

	m_pVologCB = NULL;

#ifdef _USE_VERIFYCALLBACK_
	m_nResponseSize = 0 ;
	m_pResponseBuffer = NULL ;
#endif
#ifdef DUMP_FILE

#ifdef WIN32
	out_video = fopen("d:/video_dump","wb");
	out_audio = fopen("d:/audio_dump","wb");
#elif defined LINUX
	out_video = fopen("data/local/ff/video_dump","wb");
	if(!out_video) VOLOGR("dump file create failed");
	out_audio = fopen("data/local/ff/audio_dump","wb");
#else
	char szFolder[1024];
	memset(szFolder, 0, 1024);
	voOS_GetAppFolder(szFolder, 1024);
	strcat(szFolder, "video_dump");
	out_video = fopen(szFolder, "wb");

	memset(szFolder, 0, 1024);
	voOS_GetAppFolder(szFolder, 1024);
	strcat(szFolder, "audio_dump");
	out_audio = fopen(szFolder, "wb");
#endif

#endif // DUMP_FILE
}

vo_smooth_streaming::~vo_smooth_streaming(void)
{
#ifdef _USE_VERIFYCALLBACK_
	if(m_pResponseBuffer)
		delete m_pResponseBuffer;
#endif
	free_SMTH_parser();
	memset (&m_smthparser_api, 0, sizeof (VO_PARSER_API));
	if(out_video)fclose(out_video); 
}
VO_VOID vo_smooth_streaming::NotifyData()
{
// 	 	UpdateTrackInfoData(VO_PID_LIVESRC_TRACKINFO);
// 	 	UpdateTrackInfoData(VO_PID_VIDEO_FORMAT);
// 	 	UpdateTrackInfoData(VO_PID_AUDIO_FORMAT);


}
VO_VOID vo_smooth_streaming::UpdateTrackInfoData(VO_U32 nID)
{
#ifdef VOME
	STATUS_BUFFER_ST status;

	switch(nID)
	{
	case VO_ISS_PID_AUDIO_TRACKINFO:
		{
			StreamIndex *pStreamIndex= NULL;
			if(m_manager.m_audio_index  != -1)
				pStreamIndex = &m_manager.m_manifest.streamIndex[m_manager.m_audio_index];
			if(!pStreamIndex ) return ;

			if(m_pTrackInfoEx_Audio)
			{
				delete []m_pTrackInfoEx_Audio;
				m_pTrackInfoEx_Audio = 0;
			}
			m_pTrackInfoEx_Audio = ( VO_LIVESRC_TRACK_INFOEX * )new VO_BYTE[ sizeof( VO_LIVESRC_TRACK_INFOEX ) + pStreamIndex->audio_QL[0].length_CPD ];
			
			VO_LIVESRC_TRACK_INFOEX *trackinfoex = m_pTrackInfoEx_Audio;
			memset( trackinfoex , 0 , sizeof( VO_LIVESRC_TRACK_INFOEX ) );

			trackinfoex->Type = VO_SOURCE_TT_AUDIO;
			trackinfoex->Codec = m_audioCodecType;
			memcpy(&trackinfoex->audio_info, &pStreamIndex->audio_QL[0].audio_info, sizeof(VO_AUDIO_FORMAT));
			trackinfoex->HeadData = trackinfoex->Padding;
			if(pStreamIndex->audio_QL[0].length_CPD > 0)
				memcpy(trackinfoex->HeadData, pStreamIndex->audio_QL[0].codecPrivateData, pStreamIndex->audio_QL[0].length_CPD);
			trackinfoex->HeadSize = pStreamIndex->audio_QL[0].length_CPD;

			VOLOGR("Reset Audio TrackInfo. Codec:%d, Channels:%d, SampleBits:%d, SampleRate:%d, HeadSize: %d", trackinfoex->Codec, trackinfoex->audio_info.Channels, trackinfoex->audio_info.SampleBits, trackinfoex->audio_info.SampleRate, trackinfoex->HeadSize);
		}
		break;
	case VO_ISS_PID_VIDEO_TRACKINFO:
		{
		
			StreamIndex *pStreamIndex= NULL;
			if(m_manager.m_video_index  != -1)
				pStreamIndex = &m_manager.m_manifest.streamIndex[m_manager.m_video_index];
			if(!pStreamIndex ) return ;
		
			QualityLevel *ql = m_manager.get_qualityLevel( m_manager.cur_bitrate_index );

			if(m_pTrackInfoEx_Video)
			{
				delete []m_pTrackInfoEx_Video;
				m_pTrackInfoEx_Video = 0;
			}	
			m_pTrackInfoEx_Video = ( VO_LIVESRC_TRACK_INFOEX * )new VO_BYTE[ sizeof( VO_LIVESRC_TRACK_INFOEX ) + ql->length_CPD ];
			VO_LIVESRC_TRACK_INFOEX *trackinfoex = m_pTrackInfoEx_Video;

			memset( trackinfoex , 0 , sizeof( VO_LIVESRC_TRACK_INFOEX ) );

			trackinfoex->Type = VO_SOURCE_TT_VIDEO;
			trackinfoex->Codec = m_videoCodecType;
			trackinfoex->video_info.Height = ql->video_info.Height;
			trackinfoex->video_info.Width = ql->video_info.Width;
			trackinfoex->HeadData = trackinfoex->Padding;
			if(ql->length_CPD > 0)
				memcpy(trackinfoex->HeadData,ql->codecPrivateData, ql->length_CPD);
			trackinfoex->HeadSize = ql->length_CPD;
			char c[255];
			for(int i = 0; i< ql->length_CPD;i++)
				sprintf(&c[i*2], "%02x", ql->codecPrivateData[i]);
			VOLOGR("HeadData:%s", c);
			VOLOGR("Reset Video TrackInfo. Codec:%d, Width:%d, Height:%d, HeadSize: %d", trackinfoex->Codec, trackinfoex->video_info.Width, trackinfoex->video_info.Height, trackinfoex->HeadSize);
		}
		break;

#if 0
	case VO_PID_LIVESRC_TRACKINFO:
		{
			VO_LIVESRC_TRACK_INFO trackInfo;
			trackInfo.nTrackCount = (m_manager.m_audio_index != -1 ? 1 : 0) + (m_manager.m_video_index != -1 ?1:0);
			trackInfo.pTrackID = new VO_U32[trackInfo.nTrackCount];
			trackInfo.pInfo = new VO_SOURCE_TRACKINFO[trackInfo.nTrackCount];

			VO_SOURCE_TRACKINFO* pInfo = NULL;
		
			StreamIndex *pStreamIndex= NULL;
			int trackid = 0;
			if(m_manager.m_audio_index  != -1)
				pStreamIndex = &m_manager.m_manifest.streamIndex[m_manager.m_audio_index];
			if(pStreamIndex)
			{
				trackInfo.pTrackID[trackid] = trackid;
				pInfo = &trackInfo.pInfo[trackid];
				pInfo->Codec = m_audioCodecType;
				pInfo->Duration = m_manager.get_duration();
				pInfo->HeadSize = pStreamIndex->audio_QL[0].length_CPD;
				pInfo->HeadData = new VO_BYTE[pInfo->HeadSize];
				memcpy(pInfo->HeadData,pStreamIndex->audio_QL[0].codecPrivateData, pInfo->HeadSize);
				pInfo->Start = 0;
				pInfo->Type = pStreamIndex->nTrackType;

				trackid ++;
			}

			pStreamIndex = NULL;
			if(m_manager.m_video_index  != -1)
				pStreamIndex = &m_manager.m_manifest.streamIndex[m_manager.m_video_index];
			if(pStreamIndex)
			{
				trackInfo.pTrackID[trackid] = trackid;
				pInfo = &trackInfo.pInfo[trackid];
				pInfo->Codec = m_videoCodecType;
				pInfo->Duration = m_manager.get_duration();
				QualityLevel *ptr_QL_head = m_manager.get_qualityLevel( m_manager.cur_bitrate_index );
				
				VO_U32 length = ptr_QL_head->length_CPD;		
				VO_PBYTE codecPrivateData = ptr_QL_head->codecPrivateData;

				pInfo->HeadSize = length;//ptr_QL_head->length_CPD;
				pInfo->HeadData = new VO_BYTE[pInfo->HeadSize];
				memcpy(pInfo->HeadData,codecPrivateData, length);
				pInfo->Start = 0;
				pInfo->Type = pStreamIndex->nTrackType;
			}

			status.id = nID;
			status.ptr_buffer =(VO_VOID*)&trackInfo;
			send_status_data(&status);

			for(int i = 0;i<trackInfo.nTrackCount;i++)
			{
				pInfo = &trackInfo.pInfo[i];
				delete pInfo->HeadData;
			}
			delete []trackInfo.pInfo;
			delete []trackInfo.pTrackID;

			break;
		}
	case VO_PID_VIDEO_FORMAT:
		{
			
			VO_VIDEO_FORMAT vf;
			QualityLevel *ql = m_manager.get_qualityLevel( m_manager.cur_bitrate_index );
			if(ql == NULL) break;
			vf.Height = ql->height;
			vf.Width = ql->width;
			vf.Type = VO_VIDEO_FRAMETYPE(0);

			status.id = nID;
			status.ptr_buffer =(VO_VOID*)&vf;

			send_status_data(&status);

			break;
		}
	case VO_PID_AUDIO_FORMAT:
		{
			VO_AUDIO_FORMAT af;
			return;
		}
#endif

	}	
#endif

}

VO_U32 vo_smooth_streaming::ChunkParser(VO_SOURCE_TRACKTYPE type, VO_PBYTE  pBuffer , VO_U32 uSize)
{
#ifdef _Amazon
	m_isPushMode = VO_TRUE;
	VO_PARSER_INPUT_BUFFER input;
	memset(&input, 0, sizeof(VO_PARSER_INPUT_BUFFER));
	if(VO_SOURCE_TT_VIDEO == type)
		input.nStreamID = 0; // video
	else if(VO_SOURCE_TT_AUDIO == type) 
		input.nStreamID = 1; // audio
	input.pBuf = pBuffer;
	input.nBufLen = uSize;
	VO_S64 time = 0;
	input.pReserved = &time;
	return m_smthparser_api.Process( m_smthparser_handle , &input );
#else
	return VO_ERR_LIVESRC_NOIMPLEMENT;
#endif
}

VO_U32 vo_smooth_streaming::HeaderParser( VO_PBYTE  pBuffer , VO_U32 uSize)
{
#ifdef _Amazon
	  VOLOGR("+load_SMTH_parser");
		load_SMTH_parser();
		VOLOGR("-load_SMTH_parser");

	#ifndef __USE_TINYXML
		m_xmlLoad.OpenParser();
	#endif

		VOLOGR("+m_manager.SetManifest");
		if( !m_manager.SetManifest( pBuffer, uSize) )
		{
			VOLOGR( "First set manifest url failed!" );
			//send_eos();
			return VO_FALSE;
		}
		//GetProtectionLicense(&m_manager.m_manifest.piff);
		VOLOGR("-m_manager.SetManifest");
		if(m_manager.m_audio_index>=0)m_audioCodecType= m_manager.m_manifest.streamIndex[m_manager.m_audio_index].nCodecType;
		if(m_manager.m_video_index >=0)m_videoCodecType = m_manager.m_manifest.streamIndex[m_manager.m_video_index].nCodecType;
		VOLOGR("AudioCodec:%d, VideoCodec:%d", m_audioCodecType, m_videoCodecType);

		m_bMediaStop = VO_FALSE;

		return VO_ERR_LIVESRC_OK;
#else
	return VO_ERR_LIVESRC_NOIMPLEMENT;
#endif

}

VO_BOOL vo_smooth_streaming::SetDataSource( VO_CHAR * ptr_url , VO_BOOL is_sync )
{
	VO_U32 nBegin = voOS_GetSysTime();
	close();

	VOLOGR("+load_SMTH_parser");
	if(load_SMTH_parser() != VO_ERR_NONE)
	{
		VOLOGE("Load SMTH Parser Error");
		return VO_FALSE;
	}
	VOLOGR("-load_SMTH_parser");

#ifndef __USE_TINYXML
	m_xmlLoad.OpenParser();
#endif
	m_manager.SetDrmCustomerType(m_drmCustomerType);
	VOLOGR("+m_manager.SetManifest");
	if( !m_manager.SetManifest( &m_fragment, ptr_url ) )
	{
		VOLOGR( "First set manifest url failed!" );
		//send_eos();
		return VO_FALSE;
	}

	if(m_pVologCB)
		m_smthparser_api.SetParam(m_smthparser_handle,VO_PID_COMMON_LOGFUNC, m_pVologCB);
	if(m_manager.m_manifest.piff.isUsePiff)
	{
		m_smthparser_api.SetParam(m_smthparser_handle,VOSMTH_PID_DRM_CALLBACK, m_pDrmValue);
		m_smthparser_api.SetParam(m_smthparser_handle,VOSMTH_PID_DRM_CUSTOMER, &m_drmCustomerType);
		if(m_pDrmValue && VO_ERR_NONE != GetProtectionLicense(&m_manager.m_manifest.piff))
		{
			VOLOGE("License Get error.");
			return VO_FALSE;
		}
	}
	
	VOLOGR("-m_manager.SetManifest");
	if(m_manager.m_audio_index>=0)m_audioCodecType= m_manager.m_manifest.streamIndex[m_manager.m_audio_index].nCodecType;
	if(m_manager.m_video_index >=0)m_videoCodecType = m_manager.m_manifest.streamIndex[m_manager.m_video_index].nCodecType;
	VOLOGR("AudioCodec:%d, VideoCodec:%d", m_audioCodecType, m_videoCodecType);
	//NotifyData();

	UpdateTrackInfoData(VO_ISS_PID_AUDIO_TRACKINFO);
	UpdateTrackInfoData(VO_ISS_PID_VIDEO_TRACKINFO);

	m_bMediaStop = VO_FALSE;
#ifdef WIN32
	int pos = 11200* 10000;
	seek_time = m_manager.set_pos( pos );
	//m_is_seek = VO_TRUE;
#endif

#ifndef _NEW_SOURCEBUFFER
	VO_U32 thread_id;
	voThreadHandle hthread;
	voThreadCreate( &hthread , &thread_id , threadfunc , this , 0 );
	VOLOGI("SetDataSource Cost:%d",voOS_GetSysTime() - nBegin);
#endif

	if(m_videoCodecType <=0)
		SendTrackUnavailable(VO_LIVESRC_OUTPUT_VIDEO);
	if(m_audioCodecType <=0)
		SendTrackUnavailable(VO_LIVESRC_OUTPUT_AUDIO);

	return VO_TRUE;
}

VO_VOID vo_smooth_streaming::close()
{
	stop_smoothstream( VO_FALSE );
#ifndef __USE_TINYXML
	m_xmlLoad.CloseParser();
#endif
}

void vo_smooth_streaming::thread_function()
{
	VOLOGR("Start thread!");
	start_smoothstream();
}
VO_U32 vo_smooth_streaming::threadfunc( VO_PTR pParam )
{
	vo_smooth_streaming * ptr_obj = ( vo_smooth_streaming * )pParam;
	ptr_obj->m_thread = (voThreadHandle)1;
	ptr_obj->SendBufferFromList();//start_smoothstream();
	ptr_obj->m_thread = 0;
	return 1;
}

VO_VOID vo_smooth_streaming::start_smoothstream()
{
	VOLOGR("start_smoothstream!");
	VO_S32 ret = 0;
	VO_CHAR url_fragment[1024];

	VO_S32 fragment_number = 1;
	VO_U64 update_time = 0;
	VO_S64 sleep_time = 0;
	VO_S64 seek_fragment_time = 0;
	VO_S64 last_fragment_time = 0;
	VO_BOOL live_after_pause = VO_FALSE;

//	GetProtectionLicense(&m_manager.m_manifest.piff);

	seek_fragment_time = seek_time; 
	FragmentItem * ptr_item = m_manager.GetFirstChunk();
	if( !ptr_item)
		return;	
	
	if(m_is_seek)
	{
		VOLOGR("is seek pos: %lld", seek_time);

		m_manager.m_reader.get_fragmentItem( &ptr_item, seek_time );
		need_flush();
		m_is_seek = VO_FALSE;
	}
	if( !ptr_item)
		return;	


	if(m_audioCodecType > 0 )
	{	
		VO_SMTH_FRAME_BUFFER pbuffer;
		memset(&pbuffer, 0, sizeof(VO_SMTH_FRAME_BUFFER));
		is_live()  ? pbuffer.nStartTime  = m_manager.get_audiostarttime()  : pbuffer.nStartTime  = m_manager.set_pos(seek_time)/ m_manager.GetTimeScaleMs();// ptr_item->starttime/10000;//
		m_nLastTimeStamp_Audio = pbuffer.nStartTime;
#if 0
#ifdef VOME
		CreateAudioTrackInfoBuffer(&pbuffer);
#else
		if( m_audioCodecType== VO_AUDIO_CodingVC1 )
		{
			//send WmaPro headdata
			pbuffer.pData = m_manager.m_manifest.streamIndex[m_manager.m_audio_index].audio_QL[0].codecPrivateData;
			pbuffer.nSize  = m_manager.m_manifest.streamIndex[m_manager.m_audio_index].audio_QL[0].length_CPD;
		}
		else if( m_audioCodecType == VO_AUDIO_CodingAAC)
		{
			//aac
			unsigned char c;
			pbuffer.pData = &c;
			pbuffer.nSize = 1;
		}
#endif
		AddSendDataList( &pbuffer , 0, VO_TRUE );
#else
		m_manager.bitrate_changed_Audio = VO_TRUE;
#endif
	}

	if( !is_live() ) //on-demand
	{	
		while( !m_bMediaStop )
		{
		//	voCAutoLock lock (&m_voMemMutex_forSeek);
			if(m_is_seek)
			{
				VOLOGR("is seek pos: %lld", seek_time);
				m_manager.m_reader.get_fragmentItem( &ptr_item, seek_time );
				need_flush();
				m_is_seek = VO_FALSE;

				m_manager.bitrate_changed_Video = VO_TRUE;
				m_manager.bitrate_changed_Audio = VO_TRUE;
				UpdateTrackInfoData(VO_ISS_PID_VIDEO_TRACKINFO);
				UpdateTrackInfoData(VO_ISS_PID_AUDIO_TRACKINFO);
			}

			if(!ptr_item)
			{
				send_eos();
				voOS_Sleep( 500 );
				continue;
			}
			VOLOGR(" chunk starttime: %lld,streamindex %d", ptr_item->starttime, ptr_item->streamindex);
			
			start_time = ptr_item->starttime;
			memset(url_fragment, 0, 1024);

			ret = m_manager.pop_fragment(ptr_item, url_fragment, download_bitrate,m_lastFregmentDuration);

			if( ret == -1 || m_bMediaStop )
			{
				break;
			}
			if( m_manager.m_video_index != -1 && ptr_item->streamindex == m_manager.m_video_index )
			{
				m_nLastTimeStamp_Video = ptr_item->starttime/ m_manager.GetTimeScaleMs() -1;
				download_bitrate =0;
				download_bitrate = GetMediaItem( url_fragment, ptr_item->streamindex,(VO_U32)ptr_item->duration );
				m_judgementor.add_item( download_bitrate );
				//download_bitrate = m_judgementor.get_judgment( m_manager.get_cur_bitrate(), /*m_is_video_delayhappen*/ VO_FALSE );
			}
			else if(m_manager.m_audio_index != -1 && ptr_item->streamindex == m_manager.m_audio_index)
			{
				m_nLastTimeStamp_Audio = ptr_item->starttime /m_manager.GetTimeScaleMs() -1;
				download_bitrate += GetMediaItem( url_fragment, ptr_item->streamindex,(VO_U32)ptr_item->duration );
			}

			VOLOGR("Fragment download finish: F_Number:%d,D_Cost:%llds", fragment_number,download_bitrate);

			ptr_item = m_manager.GetNextChunk(ptr_item);

			fragment_number++;

		}
	}
	else //live mode
	{
		sleep_time = m_manager.m_manifest.fragment_duration / m_manager.GetTimeScaleMs();
		update_time = (m_manager.m_reader.m_ptr_FI_tail->starttime - m_manager.GetFirstChunk()->starttime) * 3 / (4 *m_manager.GetTimeScaleMs());
		VO_BOOL enRefresh = VO_FALSE;
		while( !m_bMediaStop )
		{

			if( !ptr_item || ( ptr_item->starttime - m_manager.m_reader.m_ptr_FI_head->starttime ) / m_manager.GetTimeScaleMs() >= update_time || enRefresh)
			{
				enRefresh = VO_FALSE;

				if( !ptr_item )
					voOS_Sleep(VO_U32( sleep_time) );

				if( ! m_manager.parse_manifest( &m_fragment ) )
					break;
				//else
					//for live streaming ,License can modify each time,so need to get each time
					//GetProtectionLicense(&m_manager.m_manifest.piff);

				update_time = (m_manager.m_reader.m_ptr_FI_tail->starttime - m_manager.m_reader.m_ptr_FI_head->starttime) * 3 / (4 *m_manager.GetTimeScaleMs());

				if( m_manager.m_reader.get_fragmentItem( &ptr_item, last_fragment_time ))
				{	
					ptr_item = ptr_item->ptr_next;
					start_time = ptr_item->starttime;
				}
				else
				{
					ptr_item = m_manager.m_reader.m_ptr_FI_head;
					start_time = ptr_item->starttime;
				}

				fragment_number = 1;
				VOLOGR("refresh Manifest.");
				continue;
			}

			start_time = ptr_item->starttime;
			memset(url_fragment, 0, 1024);

			ret = m_manager.pop_fragment(ptr_item, url_fragment, download_bitrate,m_lastFregmentDuration);

			if( ret == -1 || m_bMediaStop )
				break;

			if( ptr_item->streamindex == m_manager.m_video_index )
			{
				m_nLastTimeStamp_Video = ptr_item->starttime/ m_manager.GetTimeScaleMs();
				download_bitrate =0;
				download_bitrate = GetMediaItem( url_fragment, ptr_item->streamindex,(VO_U32)ptr_item->duration );
				m_judgementor.add_item( download_bitrate );
				//download_bitrate = m_judgementor.get_judgment( m_manager.get_cur_bitrate(), /*m_is_video_delayhappen*/ VO_FALSE );
			}
			else
			{
				m_nLastTimeStamp_Audio = ptr_item->starttime/ m_manager.GetTimeScaleMs();
				download_bitrate+= GetMediaItem( url_fragment, ptr_item->streamindex,(VO_U32)ptr_item->duration );
			}
			VOLOGR("Fragment download finish: %d", fragment_number);

			if( m_bRecoverFromPause )
			{
				m_bRecoverFromPause = VO_FALSE;
				enRefresh = VO_FALSE;
			}

			last_fragment_time = ptr_item->starttime;
			ptr_item = m_manager.GetNextChunk(ptr_item);
			fragment_number++;
		}
	}

	while(m_List_buffer.GetCount() )
	{
		VOLOGR("wait for relaese list");
		voOS_Sleep(1);
	}

	VOLOGR("Ended!");
	m_manager.close();
	send_eos();
	VOLOGR( "Finished!" );

	VOLOGR( "End of start_livestream" );

}

VO_VOID vo_smooth_streaming::stop_smoothstream( VO_BOOL isflush )
{
	if( isflush )
	{
		VOLOGR("it is flush");
	}

	VOLOGR("+stop_livestream");
	m_is_flush = isflush;
	
	m_bMediaStop = VO_TRUE;
	m_is_first = VO_TRUE;
	m_is_pause =VO_FALSE;

	if( m_is_flush )
	{
		need_flush();
	}	
	vo_thread::stop();

	while (m_thread != NULL )
	{
		VOLOGR("wait for stop");
		voOS_Sleep (20);
	}		

	m_is_flush = VO_FALSE;
	m_bMediaStop = VO_FALSE;

	m_lastFregmentDuration = 0;
	download_bitrate = 0;

	m_manager.last_QL_bitrate = -1;
	m_nSyncTimeStamp = 0;
	m_bHasSyncTimeStamp = VO_FALSE;
	VOLOGR("-stop_livestream");
}


VO_U32 vo_smooth_streaming::load_SMTH_parser()
{
	if (strlen (m_szWorkPath) > 0)
		m_dlEngine.SetWorkPath ((VO_TCHAR*)m_szWorkPath);

	VOLOGR ("Work path %s", m_szWorkPath);

	vostrcpy(m_dlEngine.m_szDllFile, _T("voSmthParser"));
	vostrcpy(m_dlEngine.m_szAPIName, _T("voGetSMTHParserAPI"));

#if defined _WIN32
	vostrcat(m_dlEngine.m_szDllFile, _T(".Dll"));
#elif defined LINUX
	vostrcat(m_dlEngine.m_szDllFile, _T(".so"));
#elif defined _MAC_OS
	vostrcat(m_dlEngine.m_szDllFile, _T(".dylib"));
#endif

#ifdef _IOS
	voGetSMTHParserAPI (&m_smthparser_api);
#else
	if(m_dlEngine.LoadLib(NULL) == 0)
	{
		VOLOGR ("LoadLib fail");
		return VO_ERR_FAILED;
	}

	pvoGetSMTHParserAPI pAPI = (pvoGetSMTHParserAPI) m_dlEngine.m_pAPIEntry;
	if (pAPI == NULL)
	{
		return VO_ERR_FAILED;
	}

	pAPI (&m_smthparser_api);
#endif

	VO_PARSER_INIT_INFO info;
	info.pProc = ParserProc;
	info.pUserData = this;
	info.pMemOP = NULL;
	
	
	VO_U32 nRc = m_smthparser_api.Open( &m_smthparser_handle , &info );
	if(nRc != 0 || !m_smthparser_handle)
		return VO_ERR_FAILED;
	return VO_ERR_NONE;
}

VO_VOID vo_smooth_streaming::free_SMTH_parser()
{
	if( m_smthparser_api.Close )
	{
		m_smthparser_api.Close( m_smthparser_handle );
		m_dlEngine.FreeLib ();
		m_smthparser_handle = 0;
	}
} 
VO_U32 vo_smooth_streaming::GetStreamCounts (  VO_U32 *pStreamCounts)
{
	*pStreamCounts = m_manager.GetStreamCount();
	return VO_ERR_LIVESRC_OK;
}
VO_U32 vo_smooth_streaming::GetStreamInfo  (VO_U32 nStreamID, VO_SOURCE2_STREAM_INFO **ppStreamInfo)
{
	return m_manager.GetStreamInfo(nStreamID, ppStreamInfo);
}
VO_U32 vo_smooth_streaming::GetDRMInfo ( VO_SOURCE2_DRM_INFO **ppDrmInfo)
{
	return m_manager.GetDrmInfo(ppDrmInfo);
}

VO_U32 vo_smooth_streaming::SetLiveSrcCallbackPlus(VO_LIVESRC_CALLBACK *pCallback)
{
	m_pCallbackPlus = pCallback;
	return VO_ERR_LIVESRC_PLUS_OK;
}


VO_S64 vo_smooth_streaming::AddSampleToList(VO_SMTH_FRAME_BUFFER * ptr_buffer , VO_SOURCE2_TRACK_TYPE  index)
{
#ifdef _Amazon
	VO_SOURCE2_SAMPLE buffer;
	buffer.nCodecType = ptr_buffer->nCodecType;
	buffer.Time = ptr_buffer->nStartTime;
	buffer.Flag = 0 ;
	buffer.Size = ptr_buffer->nSize| (ptr_buffer->nFrameType == 0? 0x80000000: 0);	
	buffer.pBuffer = ptr_buffer->pData;

	m_pCallbackPlus->SendData(m_pCallbackPlus->pUserData, index, &buffer);

	return 0;
#else
	return VO_ERR_LIVESRC_NOIMPLEMENT;
#endif
}

VO_S64 vo_smooth_streaming::AddSendDataList(VO_SMTH_FRAME_BUFFER * ptr_buffer , VO_U32 index , VO_BOOL newfile)
{

	VOLOGR("in AddSendDataList %d",m_List_buffer.GetCount());
	if(m_bMediaStop || m_is_seek) return VO_ERR_NONE;

#ifdef _NEW_SOURCEBUFFER
	send_media_data(ptr_buffer,index,newfile);
#else
	//while(m_List_buffer.GetCount()>200 || m_bNeedFlushBuffer) voOS_Sleep(10);
	while(m_List_buffer.GetCount()>200 ) voOS_Sleep(10);

	voCAutoLock lock (&g_voMemMutex);


	DATA_BUFFER_ST *buffer = new DATA_BUFFER_ST;
	buffer->ptr_buffer = new VO_SMTH_FRAME_BUFFER;

	memset(buffer->ptr_buffer , 0x00,sizeof(VO_SMTH_FRAME_BUFFER));
	buffer->ptr_buffer->nCodecType = ptr_buffer->nCodecType;
	buffer->ptr_buffer->nEndTime = ptr_buffer->nEndTime;
	buffer->ptr_buffer->nFrameType = ptr_buffer->nFrameType;
	buffer->ptr_buffer->nPos = ptr_buffer->nPos;
	buffer->ptr_buffer->nSize = ptr_buffer->nSize;
	buffer->ptr_buffer->nStartTime = ptr_buffer->nStartTime;
	buffer->ptr_buffer->pData = new VO_BYTE[ptr_buffer->nSize];
	VOLOGR( "===AddData:ID(%d),Size(%d),TimeStamp(%llu),newfile(%d)", index,buffer->ptr_buffer->nSize,buffer->ptr_buffer->nStartTime,newfile );

	memcpy(buffer->ptr_buffer->pData,ptr_buffer->pData,buffer->ptr_buffer->nSize);
	buffer->index = index;
	buffer->newfile = newfile;

	m_List_buffer.AddTail(buffer);
#endif
	return 0;
}

VO_S64 vo_smooth_streaming::send_status_data(STATUS_BUFFER_ST *pBuffer)
{
	if(m_statuscallback_func) m_statuscallback_func(m_ptr_statuscallbackobj,pBuffer->id,pBuffer->ptr_buffer);
	return 0;
}

VO_S64 vo_smooth_streaming::SendBufferFromList()
{
	VOLOGR("+ SendBufferFromList");
	while(1)
	{

	//	VOLOGR("in SendBufferFromList %d",m_List_buffer.GetCount());
		voOS_Sleep(1);
		if(m_List_buffer.GetCount() ==0)
		{
			if(m_bNeedFlushBuffer) m_bNeedFlushBuffer = VO_FALSE;
			if(m_bMediaStop  ) break;
			continue;
		}
		voCAutoLock lock (&g_voMemMutex);
		if(m_List_buffer.GetCount()>0)
		{
			DATA_BUFFER_ST  * ptr_buffer = m_List_buffer.GetHead();

			if(!m_bMediaStop && !m_bNeedFlushBuffer && !m_is_seek){
				VOLOGR( "===SendBuffer:ID(%d),Size(%d),TimeStamp(%llu),newfile(%d)", ptr_buffer->index,ptr_buffer->ptr_buffer->nSize,ptr_buffer->ptr_buffer->nStartTime,ptr_buffer->newfile );
				send_media_data(ptr_buffer->ptr_buffer,ptr_buffer->index,ptr_buffer->newfile);
			}
			delete []ptr_buffer->ptr_buffer->pData;
			ptr_buffer->ptr_buffer->pData = NULL;
			delete ptr_buffer->ptr_buffer;
			ptr_buffer->ptr_buffer = NULL;
			delete ptr_buffer;
			ptr_buffer = NULL;
			m_List_buffer.RemoveHead();
		}
	}
	VOLOGR("- SendBufferFromList");
	return 0;
}

VO_S64 vo_smooth_streaming::GetProtectionLicense( ProtectionHeader * piff )
{
#ifdef _TEST
	m_smthparser_api.SetParam(m_smthparser_handle,VOSMTH_PID_DRM_INFO,(VO_PTR)piff);
#endif
	VOLOGR("in GetProtectionLicense");
	VO_U32 nRC = VO_ERR_NONE;
	if(piff->isUsePiff)
	{
		VOLOGI("need GetProtectionLicense");
		nRC =m_smthparser_api.SetParam(m_smthparser_handle,VOSMTH_PID_DRM_INFO,(VO_PTR)piff);
	}
	else
		nRC = VO_ERR_NONE;
	VOLOGR("out GetProtectionLicense");
	return nRC;

}

VO_S64 vo_smooth_streaming::GetMediaItem( VO_CHAR * ptr_url, VO_S32 index_stream,VO_U32 nChunkDuration )
{
	VOLOGR("1");
	VO_U32 ds = voOS_GetSysTime();

	if( !m_fragment.startdownload( ptr_url , DOWNLOAD2MEM ) )
	{
		VOLOGW("DownLoad. URL:%s error.",ptr_url);
		return -1;
	}
	VOLOGR("2");
	VO_S64 fragment_size = 0;
	VO_S64 read_size = 0; 
	VO_PBYTE fragment_buffer = NULL;

	fragment_size = read_size = m_fragment.get_content_length();

	fragment_buffer = new VO_BYTE[ VO_U32(fragment_size + 1) ];
	memset( fragment_buffer , 0 , VO_U32(fragment_size + 1) );

	VO_PBYTE ptr = fragment_buffer;
	VOLOGR("3");

	while( read_size != 0 && !m_bMediaStop)
	{
		voOS_Sleep(10);
		VO_S64 size = m_fragment.read( ptr , read_size );

		if( size == -2 )
			continue;

		if( size == -1 )
		{
			break;
		}

		read_size -= size;
		ptr = ptr + size;
		VOLOGR("4");

	}
	
	if(!m_is_seek){
		if(m_bMediaStop)
		{
			VOLOGR("It stopped, return GetMediaItem");
		}
		else
		{
			if( m_manager.bitrate_changed_Video == VO_TRUE)
			{
				VOLOGR("bitrate_changed_Video");

				UpdateTrackInfoData(VO_ISS_PID_VIDEO_TRACKINFO);

	#if 0
				m_manager.bitrate_changed_Video = VO_FALSE;
				VO_SMTH_FRAME_BUFFER pbuffer;
				memset(&pbuffer, 0, sizeof(VO_SMTH_FRAME_BUFFER));
	#ifdef VOME
				CreateVideoTrackInfoBuffer(&pbuffer);
	#else
				VO_S32 bitrate_index =  m_manager.cur_bitrate_index;
				QualityLevel *headData = m_manager.get_qualityLevel( bitrate_index );
				// added by Lin Jun 20110519,sometimes it's NULL
				if(!headData)
				{
					VOLOGR("no headData");
					return -1;
				}
		
				pbuffer.pData = headData->codecPrivateData;
				pbuffer.nStartTime = start_time /m_manager.GetTimeScaleMs();
				pbuffer.nSize  = headData->length_CPD;
				VOLOGR("headData size =%d",pbuffer.nSize);
	#endif
				AddSendDataList( &pbuffer , 1 , VO_TRUE );//send_media_data( &pbuffer , 1 , VO_TRUE );
	#endif
			}

			VO_PARSER_INPUT_BUFFER input;
			memset(&input, 0, sizeof(VO_PARSER_INPUT_BUFFER));
			if(index_stream == m_manager.m_video_index)
			{
				m_nCountsInOneChunk_video = 1;
				input.nStreamID = 0; // video
			}
			else
				input.nStreamID = 1; // audio

			input.pBuf = fragment_buffer;
			input.nBufLen =VO_U32( fragment_size);
			input.pReserved = &start_time;
#ifdef _VOLOG_INFO
			VO_U32 start = voOS_GetSysTime();
#endif
			m_smthparser_api.Process( m_smthparser_handle , &input );
#ifdef _VOLOG_INFO
			VO_U32 end = voOS_GetSysTime();
#endif
			VOLOGR("Process OK! Cost %f Seconds!\n" , ( end - start ) / 1000.);

		}
	}
	else{
		VOLOGR("Seek skip download chunk");
	}
	delete []fragment_buffer;
	m_fragment.closedownload();
	VOLOGR("-GetMediaItem");

	VO_U32 de = voOS_GetSysTime();
	VO_S64 br = m_fragment.get_download_bitrate();
//	if(index_stream == m_manager.m_video_index)
		VOLOGI( "BAInfo*** Download Speed %d , File Bitrate %d , URL %s.[ChunkDuration:%d,Size:%d]" , (VO_U32)br, (VO_U32)m_manager.get_cur_bitrate(), ptr_url, nChunkDuration/10000, (VO_U32)fragment_size );
	//VOLOGI("DownLoad. NetSpeed:%lld.[ChunkDuration:%d,Size:%d]",br, nChunkDuration/10000,fragment_size);
	return br;

}

void VO_API vo_smooth_streaming::ParserProc(VO_PARSER_OUTPUT_BUFFER* pData)
{
	vo_smooth_streaming * ptr_player = (vo_smooth_streaming *)pData->pUserData;
	if(ptr_player->m_is_seek) return ;

//2012.07.05 , anyone which audio or Video changed  must sent track info together.
#ifdef SENDNEWFORMAT_TOGETHER
	if(ptr_player->m_manager.bitrate_changed_Video &&( pData->nType == VO_SOURCE2_TT_VIDEO || pData->nType == VO_PARSER_OT_VIDEO) )
	{
		ptr_player->m_manager.bitrate_changed_Video = ptr_player->m_manager.bitrate_changed_Audio = VO_TRUE;
		//ptr_player->SendTrackInfo(0);
		//ptr_player->SendTrackInfo(1);
	}
#endif

	switch ( pData->nType )
	{
	case VO_SOURCE2_TT_AUDIO:
	case VO_PARSER_OT_AUDIO:
		{
			VO_SMTH_FRAME_BUFFER * ptr_buffer = (VO_SMTH_FRAME_BUFFER *)pData->pOutputData;
			if(out_audio)	fwrite(ptr_buffer->pData,1,ptr_buffer->nSize,out_audio);
			if(ptr_player->m_isPushMode)
				ptr_player->AddSampleToList(ptr_buffer, VO_SOURCE2_TT_AUDIO);
			else
			{
				ptr_buffer->nStartTime = ptr_buffer->nStartTime / ptr_player->m_manager.GetTimeScaleMs();
				if(!ptr_player->m_bHasSyncTimeStamp)
				{
					ptr_player->m_bHasSyncTimeStamp = VO_TRUE;
					ptr_player->m_nSyncTimeStamp = ptr_buffer->nStartTime;
				}
// 				ptr_buffer->nStartTime -= ptr_player->m_nSyncTimeStamp ;
// 				if(ptr_buffer->nStartTime < 0)
				if(ptr_buffer->nStartTime < ptr_player->m_nSyncTimeStamp)
						ptr_buffer->nStartTime = 0;
				ptr_player->audio_data_arrive( ptr_buffer );
			}
		}
		break;
	case VO_SOURCE2_TT_VIDEO:
	case VO_PARSER_OT_VIDEO:
		{
			VO_SMTH_FRAME_BUFFER * ptr_buffer = (VO_SMTH_FRAME_BUFFER *)pData->pOutputData;
			if(ptr_player->m_isPushMode)
				ptr_player->AddSampleToList(ptr_buffer, VO_SOURCE2_TT_VIDEO);
			else
			{	
				ptr_buffer->nStartTime = ptr_buffer->nStartTime / ptr_player->m_manager.GetTimeScaleMs();
				if(!ptr_player->m_bHasSyncTimeStamp)
				{
					ptr_player->m_bHasSyncTimeStamp = VO_TRUE;
					ptr_player->m_nSyncTimeStamp = ptr_buffer->nStartTime;
				}
				ptr_buffer->nStartTime -= ptr_player->m_nSyncTimeStamp ;
				if(ptr_buffer->nStartTime < 0)
					ptr_buffer->nStartTime = 0;
				ptr_player->video_data_arrive( ptr_buffer );
			}
			ptr_player->m_nCountsInOneChunk_video ++;
		}
		break;
	default:
		break;
	}

}
VO_VOID vo_smooth_streaming::set_VideoDelayTime(VO_PTR pValue, VO_U64 timeStamp)
{
	// now we need not think the device performance.
	//so return now.
	int delay = *((int*)pValue);
	VOLOGR("=================================Delay Time %d",delay);
#ifdef _NEW_SOURCEBUFFER
	m_manager.CheckDelayTimeStamp(delay, timeStamp);
#else

	if(delay > 130)
		m_manager.TooHightQualityLevel();
#endif
}
VO_VOID vo_smooth_streaming::audio_data_arrive( VO_SMTH_FRAME_BUFFER * ptr_buffer )
{
#ifdef _VOLOG_INFO
	static VO_U64 t = 0;
	//VOLOGR("[SSS] Recv audio frame from paraser:%lld, %lld, %d", ptr_buffer->nStartTime, ptr_buffer->nStartTime-t, ptr_buffer->nSize);
	t = ptr_buffer->nStartTime;
#endif //_VOLOG_INFO
	VOLOGR("Audio data arrive");
	VO_PBYTE pAudioData = NULL;
	if( m_audioCodecType == VO_AUDIO_CodingAAC )
	{
		//Wrap ADTS head for AAC raw data
		VO_U32 framelen = 0;
		int object, sampIdx, chanNum;

		framelen = ptr_buffer->nSize + 7; 
		pAudioData = new VO_BYTE[ framelen + 1 ];
		memset(pAudioData, 0, framelen + 1);

		object = 1;
		sampIdx = m_manager.m_manifest.streamIndex[m_manager.m_audio_index].audio_QL[0].samplerate_index;
		chanNum = int(m_manager.m_manifest.streamIndex[m_manager.m_audio_index].audio_QL[0].audio_info.Channels);

		pAudioData[0] = 0xFF; 
		pAudioData[1] = 0xF9;

		pAudioData[2] = (unsigned char)((object << 6) | (sampIdx << 2) | ((chanNum&4)>>2));

		if(framelen > 0x1FFF)
			return;

		pAudioData[3] = char(((chanNum << 6) | (framelen >> 11)));
		pAudioData[4] = char(((framelen & 0x7FF) >> 3));
		pAudioData[5] = char((((framelen & 7) << 5) | 0x1F));
		pAudioData[6] = 0xFC;	

		memcpy( pAudioData + 7, ptr_buffer->pData, ptr_buffer->nSize);

		ptr_buffer->pData = pAudioData;
		ptr_buffer->nSize = framelen;
	}

	if(ptr_buffer->nStartTime == 0)  ptr_buffer->nStartTime = 1 ;
	m_nCurTimeStamp_Audio = ptr_buffer->nStartTime;
//2012.07.05 , anyone which audio or Video changed  must sent track info together.
//#ifndef SENDNEWFORMAT_TOGETHER
	if(m_manager.bitrate_changed_Audio) SendTrackInfo(0);
//#endif

		AddSendDataList( ptr_buffer , 0 );
	if(pAudioData) delete []pAudioData;
	
}

VO_VOID vo_smooth_streaming::video_data_arrive( VO_SMTH_FRAME_BUFFER * ptr_buffer )
{
	VOLOGR("Video data arrive");
	if( m_videoCodecType == VO_VIDEO_CodingH264 )
	{
		VO_BYTE *outputbuf = ptr_buffer->pData;
		VO_U32 slicesize = 0;
		VO_U32 headerlen = sizeof(SYNC_HEADER);

		for( VO_U32 i = 0 ;i < ptr_buffer->nSize; )
		{
			memcpy(&slicesize, outputbuf, headerlen);
			slicesize = (VO_U32)be32toh(slicesize);
			memcpy(outputbuf, SYNC_HEADER, headerlen);
			i += slicesize + headerlen;
			outputbuf +=  slicesize + headerlen;
		}
		
		//if( GetFrameType(ptr_buffer->pData, ptr_buffer->nSize) == 0 )
		//	ptr_buffer->nFrameType = 0;
	}
	
	VOLOGR("[SSS] Recv video frame from paraser:TimeStamp:%llu, Size%d,FrameType:%d", ptr_buffer->nStartTime, ptr_buffer->nSize,ptr_buffer->nFrameType);
	if(ptr_buffer->nStartTime == 0)	ptr_buffer->nStartTime = 1 ;
	m_nCurTimeStamp_Video = ptr_buffer->nStartTime ;
//2012.07.05 , anyone which audio or Video changed  must sent track info together.
#ifndef SENDNEWFORMAT_TOGETHER
	if(m_manager.bitrate_changed_Video ) SendTrackInfo(1);
#endif
	AddSendDataList( ptr_buffer , 1 );

}

VO_VOID vo_smooth_streaming::send_media_data( VO_SMTH_FRAME_BUFFER * ptr_buffer , VO_U32 index , VO_BOOL newfile )
{

	VOLOGR( "====================================send_media_data" );


#ifdef _ISS_SOURCE_

	VO_LIVESRC_SAMPLE sample;
	memset( &sample , 0 , sizeof( VO_LIVESRC_SAMPLE ) );
	sample.Sample.Buffer = ptr_buffer->pData;
	sample.Sample.Time = ptr_buffer->nStartTime;
	sample.Sample.Size = ptr_buffer->nSize;

	if( index == 0 )
	{	
		sample.nTrackID = VO_LIVESRC_OUTPUT_AUDIO;
		sample.nCodecType = m_audioCodecType;
		if(out_video)	fwrite(ptr_buffer->pData,1,ptr_buffer->nSize,out_audio);
		//VOLOGR( "====================================Audio CodecType: %d",m_audioCodecType );
	}
	else if( index == 1 )
	{
		sample.nTrackID = VO_LIVESRC_OUTPUT_VIDEO;
		sample.nCodecType = m_videoCodecType;
		if(out_video)	fwrite(ptr_buffer->pData,1,ptr_buffer->nSize,out_video);
		//VOLOGR( "====================================Video CodecType: %d",m_videoCodecType );
	}

#if 1
	if( newfile )
	{
//		if(index == 1 )
		{
			sample.Sample.Flag = VO_LIVESRC_FLAG_BUFFER_HEADDATA;
		}
#ifdef VOME
			sample.Sample.Flag |= OMX_BUFFERFLAG_EXTRADATA | OMX_VO_BUFFERFLAG_NEWSTREAM;
#else

			sample.Sample.Flag |= VO_LIVESRC_FLAG_BUFFER_NEW_FORMAT;
#endif	
	}
	else
		sample.Sample.Flag = 0;
#endif

	if( ptr_buffer->nFrameType == 0 && index ==1 )
	{
#ifdef VOME
		sample.Sample.Flag = sample.Sample.Flag | OMX_BUFFERFLAG_SYNCFRAME;
#else
		sample.Sample.Flag = sample.Sample.Flag | VO_LIVESRC_FLAG_BUFFER_KEYFRAME;
		VOLOGR("It is a key frame");
#endif
	}

	VO_BOOL bFlush = VO_FALSE;
	while( m_is_pause )
	{
		VOLOGR( "in pause333" );
		voOS_Sleep( 20 );

		if( !bFlush && is_live() )
		{
			if( m_eventcallback_func )
			{
				need_flush();
			}

			bFlush = VO_TRUE;
		}

		if( m_bMediaStop )
			return;

		if(m_bNeedFlushBuffer )
			return;
		if(m_is_seek)
			return ;
	}

	if( m_datacallback_func && !m_is_seek )
	{
		VOLOGR("before Data Callback. 0x%08x", sample.Sample.Flag);
		m_datacallback_func( m_ptr_callbackobj , &sample );
		if(index == 1 && !newfile)//video
			m_manager.AddtimeStamp2Map(sample.Sample.Time, m_nCountsInOneChunk_video == 1 ? 1:0 );
		VOLOGR("after Data Callback.");

		VOLOGR( "===SendData:Flag(%x), CodecType (%d),TrackID(%d),TimeStamp(%lld),BufferSize(%d)",sample.Sample.Flag, sample.nCodecType,sample.nTrackID,sample.Sample.Time,sample.Sample.Size );
	}
#endif
}

VO_VOID vo_smooth_streaming::send_eos()
{
#ifdef _ISS_SOURCE_
	VO_LIVESRC_SAMPLE sample;
	memset( &sample , 0 , sizeof( VO_LIVESRC_SAMPLE ) );
	unsigned char c[20];
	sample.Sample.Buffer = c;
	sample.Sample.Size = 20;


#ifdef VOME
	sample.Sample.Flag = OMX_BUFFERFLAG_EOS;
#else
	sample.Sample.Flag = VO_LIVESRC_FLAG_BUFFER_EOS;
#endif

// #ifdef VOME
// 	sample.Sample.Flag = OMX_BUFFERFLAG_EOS;
// 	sample.nTrackID = VO_LIVESRC_OUTPUT_AUDIO;
// 	sample.Sample.Time = 0 ;
// 	if( m_datacallback_func )
// 		m_datacallback_func( m_ptr_callbackobj , &sample );
// 
// 	sample.nTrackID = VO_LIVESRC_OUTPUT_VIDEO;
// 	sample.Sample.Time = 0 ;
// 	if( m_datacallback_func )
// 		m_datacallback_func( m_ptr_callbackobj , &sample );
// #else
// 	sample.Sample.Flag = VOMP_FLAG_BUFFER_EOS;
	sample.nTrackID = VO_LIVESRC_OUTPUT_AUDIO;
// 	sample.Sample.Time = 0 ;
// 	if( m_datacallback_func )
// 		m_datacallback_func( m_ptr_callbackobj , &sample );
// 	sample.Sample.Time = 200000 ;
	sample.Sample.Time = m_nCurTimeStamp_Audio + 1;
	if( m_datacallback_func )
		m_datacallback_func( m_ptr_callbackobj , &sample );

	sample.nTrackID = VO_LIVESRC_OUTPUT_VIDEO;
// 	sample.Sample.Time = 0;
// 	if( m_datacallback_func )
// 		m_datacallback_func( m_ptr_callbackobj , &sample );
// 	sample.Sample.Time = 200000 ;
	sample.Sample.Time = m_nCurTimeStamp_Video+1;
	if( m_datacallback_func )
		m_datacallback_func( m_ptr_callbackobj , &sample );
//#endif

#else
	OMX_BUFFERHEADERTYPE bufHead;
	memset (&bufHead, 0, sizeof (OMX_BUFFERHEADERTYPE));
	bufHead.nSize = sizeof (OMX_BUFFERHEADERTYPE);
	bufHead.nAllocLen = 0;

	bufHead.nOutputPortIndex = 0; // 0 Audio  1 Video
	bufHead.nTickCount = 1;

	VO_BYTE b;

	bufHead.nFilledLen = 0;
	bufHead.pBuffer = &b;
	bufHead.nTimeStamp = 0;

	bufHead.nFlags = bufHead.nFlags | OMX_BUFFERFLAG_EOS;

	m_datacallback_func( m_ptr_callbackobj , &bufHead );

	bufHead.nOutputPortIndex = 1;

	if( m_datacallback_func )
		m_datacallback_func( m_ptr_callbackobj , &bufHead );
#endif
}
VO_U32 vo_smooth_streaming::SetDrmCallBack(VO_PTR pValue)
{
	VOLOGR("in SetDrmCallBack. %p", pValue);
	VO_U32 nRC = VO_ERR_NONE;

	m_pDrmValue = pValue;
	VO_SOURCEDRM_CALLBACK2 *drm = (VO_SOURCEDRM_CALLBACK2 *)m_pDrmValue;
	VOLOGI("DRM Type:%s" ,drm->szDRMTYPE);
	if(strcmp(drm->szDRMTYPE, "DRM_Discretix_PlayReady") == 0)
		m_drmCustomerType = VOSMTH_DRM_Discretix_PlayReady;
	else
		m_drmCustomerType = VOSMTH_DRM_COMMON_PlayReady;

	VOLOGR("out SetDrmCallBack");
	return nRC;
}
VO_S32 vo_smooth_streaming::set_pos( VO_S32 pos )
{
	VOLOGR( "in set_pos " );
	if( is_live() && pos <= 0)
		return 0;

	//2012/02/15, modify by Leon for issue#10725
	//for if flush at this position, normal seek must flush first, but at this time seek_time has not obtain any data. so differ call.
	/*//2012/01/20 , vome2 need flush before seek.
	need_flush();*/
	if( pos >= m_manager.get_duration())
	{
		//2012/02/15, modify by Leon
		need_flush();
		//stop_smoothstream(VO_FALSE);
		seek_time = m_manager.get_duration();
	}
	else
	{

		VO_S64 ppp = VO_S64(pos) * (m_manager.GetTimeScaleMs());
		VOLOGR( "set_pos %lld" , ppp );
		seek_time = m_manager.set_pos(ppp );
		VOLOGR( "seek_time %lld" , seek_time);
		//2012/02/15, modify by Leon
		voOS_Sleep(1);
		need_flush();
	}		
	m_manager.FlushMap();
	m_is_seek = VO_TRUE;
	return seek_time/m_manager.GetTimeScaleMs() ;
}

VO_VOID vo_smooth_streaming::start_after_seek()
{
	// 	/*VO_U32 thread_id;
	// 	voThreadHandle hthread;
	// 	voThreadCreate( &hthread , &thread_id , threadfunc , this , 0 );*/
	// 	vo_thread::begin();
}


VO_VOID vo_smooth_streaming::run()
{
	// 
	// 	if( m_is_seek )
	// 	{
	// 		start_after_seek();
	// 		m_is_seek = VO_FALSE;
	// 	}
	// 	else
	//start_after_seek
	if(!m_thread_handle)vo_thread::begin();

	if(m_is_pause)
		m_bRecoverFromPause = VO_TRUE;
	m_is_pause = VO_FALSE; 

}

VO_VOID vo_smooth_streaming::need_flush()
{
	VOLOGR("vo_smooth_streaming::need_flush");

#ifdef _ISS_SOURCE_
#ifndef _NEW_SOURCEBUFFER
	m_bNeedFlushBuffer = VO_TRUE;
	VOLOGR("before Flush BufferFromList size: %d",m_List_buffer.GetCount());
	while( m_bNeedFlushBuffer) voOS_Sleep(10);
#endif
	VO_LIVESRC_SAMPLE sample;
	memset( &sample , 0 , sizeof( VO_LIVESRC_SAMPLE ) );
	sample.Sample.Flag = VO_LIVESRC_FLAG_BUFFER_FORCE_FLUSH;
	sample.nTrackID = VO_LIVESRC_OUTPUT_AUDIO;

	VOLOGR("+Send Flush Audio Buffer");
	if( m_datacallback_func )
		m_datacallback_func( m_ptr_callbackobj , &sample );
	VOLOGR("-Send Flush Audio Buffer");

	sample.nTrackID = VO_LIVESRC_OUTPUT_VIDEO;

	VOLOGR("+Send Flush Video Buffer");
	if( m_datacallback_func )
		m_datacallback_func( m_ptr_callbackobj , &sample );
	VOLOGR("-Send Flush Video Buffer");
#else
	//Event event;
	//event.id = EVENT_NEEDFLUSH;
	//m_eventcallback_func( m_ptr_eventcallbackobj , &event );
#endif
}

VO_VOID vo_smooth_streaming::sendCodecInfo()
{
	// 	if( m_eventcallback_func )
	// 	{
	// 		Event pMsg;
	// 		memset(&pMsg, 0, sizeof(Event));
	// 
	// 		pMsg.nID = VO_LIVESRC_STATUS_CODEC_CHANGED;
	// 
	// 		if( m_manager.m_manifest.streamIndex[m_manager.m_video_index].videoFormat == 1)
	// 		{
	// 			m_is_H264 = VO_TRUE;
	// 
	// 			pMsg.nParam1 = 0; //audio
	// 			pMsg.nParam2 = VO_AUDIO_CodingAAC;
	// 			m_eventcallback_func( m_ptr_eventcallbackobj , &pMsg );
	// 
	// 			pMsg.nParam1 = 1; //video
	// 			pMsg.nParam2 = VO_VIDEO_CodingH264;
	// 			m_eventcallback_func( m_ptr_eventcallbackobj , &pMsg );
	// 
	// 			VOLOGR("send change codec event: H264 + AAC");
	// 		}
	// 		else if( m_manager.m_manifest.streamIndex[m_manager.m_video_index].videoFormat == 2)
	// 		{
	// 			
	// 			VOLOGR("send change codec event: WMA");
	// 
	// 			pMsg.nParam1 = 0; //audio
	// 			pMsg.nParam2 = VO_AUDIO_CodingWMA;
	// 			m_eventcallback_func( m_ptr_eventcallbackobj , &pMsg );
	// 
	// 			VOLOGR("send change codec event: VC1");
	// 
	// 			pMsg.nParam1 = 1; //video
	// 			pMsg.nParam2 = VO_VIDEO_CodingVC1;
	// 			m_eventcallback_func( m_ptr_eventcallbackobj , &pMsg );
	// 
	// 		}
	// 	}
}

/*buffer:the data  at the beginning of  NALU
2 : I frame
0 : P frame
1 : B frame
-1: error or other NALU
*/
VO_S32 vo_smooth_streaming::GetFrameFormat (VO_BYTE* buffer, VO_U32 nBufSize)
{
	int inf,i;
	long byteoffset;      // byte from start of buffer
	int bitoffset;      // bit from start of byte
	int ctr_bit=0;      // control bit for current bit posision
	int bitcounter=1;
	int len;
	int info_bit;
	int totbitoffset = 0;
	int naluType = buffer[0]&0x0f;

	if(naluType>0&&naluType<=5)//need continuous check
		buffer++;
	else//the nalu type is params info
		return -1;
	for(i=0;i<2;i++)
	{
		byteoffset= totbitoffset/8;
		bitoffset= 7-(totbitoffset%8);
		ctr_bit = (buffer[byteoffset] & (0x01<<bitoffset));   // set up control bit

		len=1;
		while (ctr_bit==0)
		{                 // find leading 1 bit
			len++;
			bitoffset-=1;           
			bitcounter++;
			if (bitoffset<0)
			{                 // finish with current byte ?
				bitoffset=bitoffset+8;
				byteoffset++;
			}
			ctr_bit=buffer[byteoffset] & (0x01<<(bitoffset));
		}
		// make infoword
		inf=0;                          // shortest possible code is 1, then info is always 0
		for(info_bit=0;(info_bit<(len-1)); info_bit++)
		{
			bitcounter++;
			bitoffset-=1;
			if (bitoffset<0)
			{                 // finished with current byte ?
				bitoffset=bitoffset+8;
				byteoffset++;
			}

			inf=(inf<<1);
			if(buffer[byteoffset] & (0x01<<(bitoffset)))
				inf |=1;
		}
		totbitoffset+=len*2-1;
		if(totbitoffset>48)
			return -1;
	}
	//(int)pow(2,(bitsUsed/2))+info-1;//pow(2,x)==1<<x
	len = (len*2-1)/2;
	inf = (1<<len)+inf-1;
	if(inf>4)
		inf-=5;
	if(inf>4)
		return -1;//error

	return inf;
}

// return -1 Unknown 0, I, 1, P, 2 B,
VO_S32 vo_smooth_streaming::GetFrameType (VO_BYTE* pInData, VO_U32 nInSize)
{
	long	m_lH264Format = 0;//VOH264_ANNEXB=0;
	VO_U32	m_dwNALWord = 0X010000;
	VO_BYTE*	pBuffer = pInData;
	int		nBufSize = int(nInSize);

	int		nNALSize = 3;

	int nFormat = -1;
	int nFrameCount = 0;

	while (nBufSize > nNALSize)
	{
		if (m_lH264Format != 0)//VOH264_ANNEXB
		{
			int		nFrameSize = 0;
			for (int i = 0; i < nNALSize; i++)
			{
				nFrameSize <<= 8;
				nFrameSize += *pBuffer++;
			}
			nBufSize -= nNALSize;

			nFormat = GetFrameFormat (pBuffer, nFrameSize);
			pBuffer += nFrameSize;
			nBufSize -= nFrameSize;

			nFrameCount++;

			if (nFormat >= 0)
			{
				if (nFormat == 0 && nBufSize > 4)
					continue;
				break;
			}

		}
		else
		{
			while (pBuffer - pInData < int(nInSize) - nNALSize)
			{
				if (!memcmp (pBuffer, &m_dwNALWord, nNALSize))
					break;
				pBuffer++;
			}

			pBuffer += nNALSize;
			nBufSize -= nNALSize;
			nFormat = GetFrameFormat (pBuffer, nBufSize);

			if (nFormat >= 0)
				break;
			else if (nFormat == -1)
			{
				while (pBuffer - pInData < int(nInSize) - nNALSize)
				{
					if (!memcmp (pBuffer, &m_dwNALWord, nNALSize))
						break;
					pBuffer++;
				}

				nBufSize = nInSize - (pBuffer - pInData);
			}

			nFrameCount++;
		}
	}

	if (nFormat == 2) // I Frame
		return 0;
	else if (nFormat == 1) // B Frame
		return 2;
	else if (nFormat == 0) // P Frame
		return 1;

	return -1;
}



VO_VOID vo_smooth_streaming::CreateAudioTrackInfoBuffer(VO_SMTH_FRAME_BUFFER *pBuffer)
{
	VOLOGR("+ CreateAudioTrackInfoBuffer");
	VO_SMTH_FRAME_BUFFER *buffer = pBuffer;
	memset( buffer , 0 , sizeof(VO_SMTH_FRAME_BUFFER) );
	//VOLOGR("1");
	buffer->nSize = m_pTrackInfoEx_Audio->HeadSize > 12 ? sizeof( VO_LIVESRC_TRACK_INFOEX ) + m_pTrackInfoEx_Audio->HeadSize - 12 : sizeof( VO_LIVESRC_TRACK_INFOEX );
	//VOLOGR("2");
	buffer->pData = (VO_PBYTE)m_pTrackInfoEx_Audio;
	//VOLOGR("3");
	buffer->nStartTime = m_nCurTimeStamp_Audio -1;
	VOLOGR("- CreateAudioTrackInfoBuffer");
//	send_media_data( &buffer , 0 , VO_TRUE );
}

VO_VOID vo_smooth_streaming::CreateVideoTrackInfoBuffer(VO_SMTH_FRAME_BUFFER *pBuffer)
{
	VOLOGR("+ CreateVideoTrackInfoBuffer");
	VO_SMTH_FRAME_BUFFER *buffer = pBuffer ;
	memset( buffer , 0 , sizeof(VO_SMTH_FRAME_BUFFER) );
	buffer->nSize = m_pTrackInfoEx_Video->HeadSize > 12 ? sizeof( VO_LIVESRC_TRACK_INFOEX ) + m_pTrackInfoEx_Video->HeadSize - 12 : sizeof( VO_LIVESRC_TRACK_INFOEX );
	buffer->pData = (VO_PBYTE)m_pTrackInfoEx_Video;
	buffer->nStartTime = m_nCurTimeStamp_Video - 1;
	buffer->nFrameType = 0;
	VOLOGR("- CreateVideoTrackInfoBuffer");
}

int vo_smooth_streaming::GetTrackNumber()
{
	VOLOGR("+ GetTrackNumber");
	int tracks = 0;
	if(m_videoCodecType >0) tracks ++;
	if(m_audioCodecType > 0) tracks ++;
	VOLOGR("- GetTrackNumber. %d",tracks);
	return tracks;
}

VO_U32 vo_smooth_streaming::SendTrackInfo(int isVideo)
{
	VO_SMTH_FRAME_BUFFER pbuffer;
	memset(&pbuffer, 0, sizeof(VO_SMTH_FRAME_BUFFER));
#ifdef VOME
	if(isVideo)	
		CreateVideoTrackInfoBuffer(&pbuffer);
	else
		CreateAudioTrackInfoBuffer(&pbuffer);
#else
	if(isVideo){
		VO_S32 bitrate_index =  m_manager.cur_bitrate_index;
		QualityLevel *headData = m_manager.get_qualityLevel( bitrate_index );
		// added by Lin Jun 20110519,sometimes it's NULL
		if(!headData)
		{
			VOLOGR("no headData");
			return -1;
		}

		pbuffer.pData = headData->codecPrivateData;
		if(m_nCurTimeStamp_Video <1)m_nCurTimeStamp_Video =0;
		pbuffer.nStartTime = m_nCurTimeStamp_Video +1;//start_time /m_manager.GetTimeScaleMs();
		pbuffer.nSize  = headData->length_CPD;
		VOLOGR("headData size =%d",pbuffer.nSize);
	}
	else {
		if( m_audioCodecType== VO_AUDIO_CodingWMA )
		{
			//send WmaPro headdata
			pbuffer.pData = m_manager.m_manifest.streamIndex[m_manager.m_audio_index].audio_QL[0].codecPrivateData;
			pbuffer.nSize  = m_manager.m_manifest.streamIndex[m_manager.m_audio_index].audio_QL[0].length_CPD;
		}
		else if( m_audioCodecType == VO_AUDIO_CodingAAC)
		{
			//aac
			unsigned char c;
			pbuffer.pData = &c;
			pbuffer.nSize = 1;
		}
		else
		{
			pbuffer.pData = m_manager.m_manifest.streamIndex[m_manager.m_audio_index].audio_QL[0].codecPrivateData;
			pbuffer.nSize  = m_manager.m_manifest.streamIndex[m_manager.m_audio_index].audio_QL[0].length_CPD;
		}
		if(m_nCurTimeStamp_Audio <1)m_nCurTimeStamp_Audio =0;
		pbuffer.nStartTime = m_nCurTimeStamp_Audio +1;
	}
#endif

	AddSendDataList( &pbuffer , isVideo , VO_TRUE );//send_media_data( &pbuffer , 1 , VO_TRUE );
	if(isVideo)	
		m_manager.bitrate_changed_Video = VO_FALSE;
	else
		m_manager.bitrate_changed_Audio = VO_FALSE;
	return 0;
}


void vo_smooth_streaming::SetLibOp(VO_LIB_OPERATOR* pValue) 
{
	VOLOGR("+ Set Lib Op!");
		m_dlEngine.SetLibOperator(pValue);
#ifndef __USE_TINYXML
		m_xmlLoad.SetLibOperator(pValue);
#endif
	VOLOGR("- Set Lib Op!");
}

VO_U32 vo_smooth_streaming::GetPlayList(FragmentItem **ppPlayList)
{
	*ppPlayList = m_manager.GetFirstChunk();
	return 0;
}

VO_U32 vo_smooth_streaming::GetProgramInfo( VO_U32 nProgram,VO_SOURCE2_PROGRAM_INFO **ppProgramInfo)
{
	m_manager.GetProgramInfo(ppProgramInfo);
	return 0;
}

VO_U32 vo_smooth_streaming::SetParam(VO_U32 nParamID, VO_PTR pParam)
{
	switch(nParamID)
	{
	case VO_PID_COMMON_LOGFUNC:
		{
			m_pVologCB = (VO_LOG_PRINT_CB *)pParam;
			m_xmlLoad.SetParam(nParamID, m_pVologCB);
		}
		break;

	case VO_PID_SOURCE2_BACAP:
		{
			m_manager.SetBACAP((VO_SOURCE2_CAP_DATA*)pParam);
			break;
		}
	default:
		return VO_RET_SOURCE2_NOIMPLEMENT;
	}
	return VO_RET_SOURCE2_OK;
}

#ifdef _USE_VERIFYCALLBACK_
VO_U32    vo_smooth_streaming::DoVerifyCallBack(VO_PTR pParam)
{
	VO_SOURCE2_VERIFICATIONINFO *pVerificationInfo = (VO_SOURCE2_VERIFICATIONINFO *)pParam;
	VOLOGI("%s,%d, %p",pVerificationInfo->pData,pVerificationInfo->uDataSize, pVerificationInfo->pUserData);
	VO_U32 nRet =m_fragment.DoVerifyCallBack((VO_CHAR*)pVerificationInfo->pData, pVerificationInfo->uDataSize,pVerificationInfo->pUserData);
	if(nRet == VO_ERR_NONE)
	{
		VO_S64 read_size = 0; 

		read_size = m_fragment.get_content_length();
		if(read_size >m_nResponseSize )
		{
			m_nResponseSize = read_size;
			if(m_pResponseBuffer)
				delete []m_pResponseBuffer;
			m_pResponseBuffer = new VO_CHAR[ VO_U32(m_nResponseSize + 1) ];
		}
		memset( m_pResponseBuffer , 0 , m_nResponseSize +1 );

		VO_PBYTE ptr = (VO_PBYTE)m_pResponseBuffer;
		while( read_size != 0 && !m_bMediaStop)
		{
			voOS_Sleep(10);
			VO_S64 size = m_fragment.read( ptr , read_size );

			if( size == -2 )
				continue;
			if( size == -1 )
			{
				break;
			}
			read_size -= size;
			ptr = ptr + size;
		}
		pVerificationInfo->szResponse = (VO_CHAR*)m_pResponseBuffer;
		pVerificationInfo->uResponseSize = m_nResponseSize;

		return VO_ERR_NONE;
	}
	return VO_ERR_FAILED;
}
#endif

VO_VOID vo_smooth_streaming::SendTrackUnavailable(VO_LIVESRC_OUTPUT_TYPE type)
{
	VO_LIVESRC_SAMPLE sample;
	memset( &sample , 0 , sizeof( VO_LIVESRC_SAMPLE ) );
	unsigned char c[20];
	sample.Sample.Buffer = c;
	sample.Sample.Size = 20;

	sample.Sample.Flag = VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE;
	sample.nTrackID = type;
	sample.Sample.Time =  1;
	if( m_datacallback_func )
		m_datacallback_func( m_ptr_callbackobj , &sample );
}