#include "vo_http_live_streaming_new.h"
#include "voLog.h"
#include "voOSFunc.h"
#include "voThread.h"
#include "CCheckTsPacketSize.h"
#include "vo_aes_engine.h"

#include "voHLSDRM.h"

#include "vo_drm_mem_stream.h"
#include "voSubtitleType.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


typedef VO_S32 ( VO_API *pvoGetParserAPI)(VO_PARSER_API * pParser);
typedef VO_S32 ( VO_API *pvoGetSource2ParserAPI)(VO_SOURCE2_API* pParser);


#ifndef MAX_FASTFETCH_FRAMECOUNT
#define MAX_FASTFETCH_FRAMECOUNT 50
#endif  
  
#define  COMMON_ONSTREAM2_MEDIA_TS          0
#define  COMMON_ONSTREAM2_MEDIA_AAC         1
#define  COMMON_ONSTREAM2_MEDIA_UNKNOWN     2

#define  COMMON_DUMP_FILE_DATA_NORMAL     0
#define  COMMON_DUMP_FILE_NEW_NORMAL      1
#define  COMMON_DUMP_FILE_NEW_ADAPTION    2

#define  HTTP_NOT_FOUND     404
#define  NEED_SKIP_CHUNCK   -3
#define  DEFAULT_MAX_DOWNLOAD_FAILED_TOLERANT_COUNT   2



#define  HLS_ID3V2_HEADER_LEN			10
#define  HLS_CALCULATE_SIZE_SYNCHSAFE(size_array)	((size_array[0] << 21) | (size_array[1] << 14) | (size_array[2] << 7) | size_array[3])
#define  HLS_TS_SYNC                      0x47




FILE * g_fpVideoFromParser = NULL;
FILE * g_fpVideoFromBufferList = NULL;
FILE * g_fpAudioFromParser = NULL;
FILE * g_fpAudioFromBufferList = NULL;
FILE * g_fpAACFromHttp = NULL;
FILE * g_fpTSFromHttp = NULL;
FILE * g_fpID3FromParser = NULL;
FILE * g_fpFileForJpDrm = NULL;



//FILE * g_fpaacoutNew = NULL;


//FILE * g_fp2;

vo_http_live_streaming_new::vo_http_live_streaming_new(void)
:m_judgementor(5)
,m_is_first_frame(VO_TRUE)
,m_new_video_file(VO_TRUE)
,m_new_audio_file(VO_TRUE)
,m_bMediaStop(VO_FALSE)
,m_is_pause( VO_FALSE )
,m_recoverfrompause( VO_FALSE )
,m_is_flush(VO_FALSE)
,m_is_seek(VO_FALSE)
,m_is_first_start(VO_FALSE)
,m_is_first_getdata(VO_FALSE)
,m_delaycount(0)
,m_isdeterminenextbitrate(VO_FALSE)
{
	m_keytag = -1;
	m_drm_eng_handle = 0;
	m_is_video_delayhappen = VO_FALSE;
	m_is_bitrate_adaptation = VO_FALSE;
	m_audiocounter = 0;
	m_videocounter = 0;
	m_last_big_timestamp = 0;
	m_timestamp_offset = 0;
	m_seekpos = 0;
	m_is_afterseek = VO_FALSE;
	m_mediatype = -1;
	m_is_mediatypedetermine = VO_FALSE;
	m_ptr_audioinfo = 0;
	m_ptr_videoinfo = 0;
	m_last_audio_timestamp = -1;
	m_last_video_timestamp = -1;
	m_download_bitrate = -1;
	m_rightbandwidth = -1;
	m_brokencount = 0;


	m_datacallback_func = 0;
    m_ptr_callbackobj = NULL;
	m_eventcallback_func = 0;
	m_ptr_eventcallbackobj = NULL;

	m_pProgramInfo = NULL;

    m_pBufferManager = NULL;

    m_iCurrentMediaType = COMMON_ONSTREAM2_MEDIA_UNKNOWN;

    InitDataBuffer();
    
	memset (&m_tsparser_api, 0, sizeof (VO_PARSER_API));
    m_tsparser_handle = NULL;
    memset (&m_aacparser_api, 0, sizeof (VO_PARSER_API));
    m_aacparser_handle = NULL;
    
	strcpy (m_szWorkPath, "");
	memset( m_last_keyurl , 0 , 1024 );
	memset( &m_drm_eng , 0 , sizeof( DRM_Callback ) );

	m_adaptationbuffer.set_callback( this , bufferframe_callback );

	m_pDrmCallback = NULL;
	m_iSeekResult = 0;
	m_bNeedUpdateTimeStamp = VO_FALSE;
	m_iVideoDelayCount = 0;

	memset(&m_sHLSUserInfo, 0 ,sizeof(S_HLS_USER_INFO));
	LoadWorkPathInfo();
	ResetAllFilters();


    m_illLastAudioOutputTime = 0;	
    m_illLastVideoOutputTime = 0;

    m_iDrmType = 0;
    m_iProcessSize = 0;
    m_pLogParam = NULL;
    m_bChuckMediaTypeChanged = VO_FALSE;
    m_bDrmNeedWork = VO_FALSE;
    m_ulMaxDownloadFailTolerantCount = DEFAULT_MAX_DOWNLOAD_FAILED_TOLERANT_COUNT;
    m_ulCurrentDownloadFailConsistentCount = 0;
    m_bWorkModeForDiscretixPlayReady = VO_FALSE;


	m_pThumbnailList = NULL;
	memset(m_strManifestURL, 0, 1024);

#ifdef _DUMP_RAWDATA_
    m_bDumpRawData = VO_TRUE;
#else
    m_bDumpRawData = VO_FALSE;
#endif

}

vo_http_live_streaming_new::~vo_http_live_streaming_new(void)
{
    CloseAllDump();
    UnInitDataBuffer();
}


VO_BOOL vo_http_live_streaming_new::open( VO_CHAR * ptr_url , VO_BOOL is_sync )
{
    VO_U32       ulThumbnailCount = 0;


    ResetAllIDs();
	VOLOGI( "vo_http_live_streaming::open" );
	close();
	perpare_drm();
	m_is_first_frame = VO_TRUE;

    m_judgementor.load_config( m_szWorkPath );

	VOLOGI("+m_manager.set_m3u_url");
	if( !m_manager.set_m3u_url( ptr_url ) )
	{
		VOLOGE( "First set m3u url failed!" );
		//send_eos();
		return VO_FALSE;
	}
	VOLOGI("-m_manager.set_m3u_url");


	ulThumbnailCount = m_manager.GetThumbnailItemCount();
	if(m_manager.GetThumbnailItemCount() != 0)
	{
	    DoNotifyForThumbnail();
	}


    VOLOGI( "++++++++++++++++Bitrate" );
    VO_S32 playlist_count = 0;
    m_manager.get_all_bandwidth( 0 , &playlist_count );
    VOLOGI( "Playlist Count: %d" , playlist_count );
    if( playlist_count != 0 )
    {
        VO_S64 * ptr_playlist = new VO_S64[playlist_count];

        if( ptr_playlist )
        {
            m_manager.get_all_bandwidth( ptr_playlist , &playlist_count );

            for( VO_S32 i = 0 ; i < playlist_count ; i++ )
            {
                VOLOGI( "Bitrate: %lld" , ptr_playlist[i] );
            }

            m_judgementor.set_playlist( ptr_playlist , playlist_count );
        }
    }
    VOLOGI( "+++++++++++++++++++++++" );

    m_judgementor.set_default_streambandwidth( m_manager.get_cur_bandwidth() );

    m_judgementor.get_right_bandwidth( m_manager.get_cur_bandwidth() , &m_rightbandwidth );

	VOLOGI("+load_ts_parser");
	load_ts_parser();
	VOLOGI("-load_ts_parser");

	m_bMediaStop = VO_FALSE;

	if( is_sync )
	{
		//m_thread = (VO_VOID*)1;
		//start_livestream();
		threadfunc( this );
	}
	else
	{
		begin();
	}

    return VO_TRUE;
}

VO_VOID vo_http_live_streaming_new::close()
{
	VOLOGI("vo_http_live_streaming::close");
	CvoGenaralDrmCallback*   pDrmCallback = NULL;

	stop_livestream( VO_FALSE );
	m_manager.close();
	free_ts_parser();
	release_drm();

	pDrmCallback = (CvoGenaralDrmCallback*)m_pDrmCallback;
	
	if(m_pDrmCallback != NULL)
	{
	    delete pDrmCallback;
		m_pDrmCallback = NULL;
	}


	if(m_pThumbnailList != NULL)
    {
        delete   []m_pThumbnailList;
		m_pThumbnailList = NULL;
    }
	
	memset (&m_tsparser_api, 0, sizeof (VO_PARSER_API));
	
}

void vo_http_live_streaming_new::thread_function()
{
	set_threadname((char*) "Live Streaming" );
	start_livestream();
}

VO_VOID vo_http_live_streaming_new::start_livestream()
{
	VO_BOOL is_from_pause = VO_FALSE;

	VO_S32 count = 0;

    VO_S32 last_sn = 0;

	VO_S32  iIndex = 0;

	VO_S32  iCharpterCount = m_manager.GetCharpterCount();
		
    m_ulCurrentDownloadFailConsistentCount = 0;
	while( (!m_bMediaStop) && (m_iSeekResult !=(-1)))  //seek failed, reach the stream end
	{
		media_item item;
        VO_S32 ret = 0;
        VO_S64 bandwidth_before = m_manager.get_cur_bandwidth();


        if(m_ulForceSelect != 0)
        {
            ret = m_manager.popup( &item , m_ulSelectBandwidth);
        }
        else
        {
            ret = m_manager.popup( &item , m_rightbandwidth);
        }

        VO_S64 bandwidth_after = m_manager.get_cur_bandwidth();

        SetTheCurrentSelStream((VO_U32) bandwidth_after);

		VO_S32 try_time = 0;
		VO_S32 longestwaittime = 30000;
		VO_BOOL   bFindCommonMedia = VO_FALSE;

        //reset the DelayCount
        m_iVideoDelayCount = 0;
		
		while(bFindCommonMedia == VO_FALSE)
		{
            if( m_manager.is_live() )
            {
                while( !m_bMediaStop && ret == -1 && try_time <= longestwaittime )
                {
                    voOS_Sleep( 100 );

                    try_time = try_time + 100;

                    ret = m_manager.popup( &item , m_rightbandwidth );
                }

                if(ret == -1 && try_time > longestwaittime)
                {
                    NotifyTheNetworkBroken();
                }
            }
            else
            {
                VO_U32 start_time = voOS_GetSysTime();
                while( !m_bMediaStop && ret == -2 && ( (voOS_GetSysTime() - start_time) <= 120000 ) )
                {
                    voOS_Sleep( 100 );

                    try_time = try_time + 100;

                    ret = m_manager.popup( &item , m_rightbandwidth );
                }

                if( ret == -2 )
                    ret = -1;
            }

		    if( ret == -1 || m_bMediaStop )
			    break;
			
			//For  Ad application
			VOLOGI("the current sequence_number:%d", item.sequence_number);
			switch(item.eMediaUsageType)
			{
			    case M3U_COMMON_USAGE_TYPE:
				{
					bFindCommonMedia = VO_TRUE;
					break;
				}
				case M3U_COMMON_AD_TYPE:
				{
					bFindCommonMedia = VO_FALSE;					
					iIndex = FindFilterIndex(item.pFilterString);
					if(iIndex != -1)
					{
					    DoNotifyForAD(&item, iIndex);
					}
					VOLOGI("The Filter Index:%d, the Ad URL:%s",iIndex,item.path);
					ret = m_manager.popup( &item , m_rightbandwidth );
					break;
				}
				default:
				{
					VOLOGI("something wrong happened");
					break;
				}
			}
			//For  Ad application
		}

		
		if( ret == -1 || m_bMediaStop )
			break;

		VOLOGI( "Begin to Download: %s\n, the stream state:%d" , item.path, m_manager.is_live());

/*
        if(strstr(item.path, ".ts") != NULL  || strstr(item.path, ".TS") != 0)
        {
            if(m_iCurrentMediaType != COMMON_ONSTREAM2_MEDIA_TS)
            {
                VOLOGI("change the media type to ts!");
				if( !m_manager.is_live() )
					m_timestamp_offset = m_manager.GetSequenceIDTimeOffset(item.sequence_number)*1000;
				else
					m_timestamp_offset = m_last_audio_timestamp;
                item.eReloadType = M3U_RELOAD_RESET_TIMESTAMP_TYPE;
                m_iCurrentMediaType = COMMON_ONSTREAM2_MEDIA_TS;
                FreeAACParser();
                free_ts_parser();
                load_ts_parser();
                memset(item.oldpath, 0, 1024);
            }
        }
        else
        {
            if(strstr(item.path, ".aac") != NULL  || strstr(item.path, ".AAC") != 0)
            {
                if(m_iCurrentMediaType != COMMON_ONSTREAM2_MEDIA_AAC)
                {
                    VOLOGI("change the media type to aac!");
                    m_iCurrentMediaType = COMMON_ONSTREAM2_MEDIA_AAC;
                    free_ts_parser();
                    FreeAACParser();
                    LoadAACParser();
                    memset(item.oldpath, 0, 1024); 
					if( !m_manager.is_live() )
						m_timestamp_offset = m_manager.GetSequenceIDTimeOffset(item.sequence_number)*1000; 
					else
						m_timestamp_offset = m_last_audio_timestamp > m_last_video_timestamp ? m_last_audio_timestamp : m_last_video_timestamp;
                    m_iCurrentMediaType = COMMON_ONSTREAM2_MEDIA_AAC;

                    DumpAACFromHttp(NULL, 0, 1);
                }
                
                m_iCurrentMediaType = COMMON_ONSTREAM2_MEDIA_AAC;
                SendMediaTrackDisable(VO_SOURCE2_TT_VIDEO);
            }
        }
*/

        m_ulCurrentSequenceIDForTimeStamp = item.sequence_number;

        if(!m_manager.is_live())
        {
		    if(item.iCharpterId != m_iLastCharpterID)
		    {
                m_timestamp_offset = m_manager.GetSequenceIDTimeOffset(item.sequence_number)*1000;
			    VOLOGI("the current  CharpterID:%d, start sequence:%d, new timeoffset:%d", item.iCharpterId, item.sequence_number, m_timestamp_offset);
			    item.eReloadType = M3U_RELOAD_RESET_TIMESTAMP_TYPE;
	        }
		    else
		    {
			    if(m_is_afterseek == VO_TRUE)
			    {
			        m_timestamp_offset = m_manager.GetSequenceIDTimeOffset(item.sequence_number)*1000;
                    m_last_audio_timestamp = m_last_video_timestamp = m_timestamp_offset;
                    VOLOGI("start sequence:%d, new timeoffset:%d", item.sequence_number, m_timestamp_offset);
                    item.eReloadType = M3U_RELOAD_RESET_TIMESTAMP_TYPE; 
			    }
		    }
        }
		else
		{   
		    if(item.sequence_number != (m_iLastSequenceID+1))
            {
                item.eReloadType = M3U_RELOAD_RESET_TIMESTAMP_TYPE;
                VOLOGI("the sequence number is not continue!");
            }
            
            if(item.eReloadType == M3U_RELOAD_RESET_TIMESTAMP_TYPE)
		    {
		        m_timestamp_offset = m_last_video_timestamp > m_last_audio_timestamp ? m_last_video_timestamp + 1 : m_last_audio_timestamp + 1;
		        VOLOGI("the live stream, the item path:%s", item.path);
				VOLOGI("sequence number:%d; the new timeoffseet:%d", item.sequence_number, m_timestamp_offset);
				if(strlen( item.oldpath ) != 0)
				{
				    memset(item.oldpath, 0, 1024);
				}
			}
		}
		
		m_iLastCharpterID = item.iCharpterId;
		m_iLastSequenceID = item.sequence_number;
		
        last_sn = item.sequence_number;

		if( is_from_pause )
		{
            if( m_manager.is_live() || bandwidth_before != bandwidth_after )
            {
                item.eReloadType = M3U_RELOAD_RESET_CONTEXT_ONLY_TYPE;
            }
			is_from_pause = VO_FALSE;
			item.oldpath[0] = '\0';
		}

		if( m_is_afterseek )
        {
			item.eReloadType = M3U_RELOAD_RESET_TIMESTAMP_TYPE;
			m_is_afterseek = VO_FALSE;
		}

		if( count % 120 == 0 )
		{
			//item.need_reload = VO_TRUE;
		}

        /*if(strlen( item.oldpath ) != 0)
        {
            //skip the old stream video data
            m_judgementor.add_starttime( m_last_audio_timestamp + 1500 );
        }
        else*/
        {
            m_judgementor.add_starttime( m_last_audio_timestamp + 1 );
        }

        VO_U32 start_time = voOS_GetSysTime();
        do 
        {
            int   iTryCount = 0;
            m_download_bitrate = GetMediaItem( &item );
            TransactionForDownloadResult(m_download_bitrate);
            if( voOS_GetSysTime() - start_time > 120000 )
                break;

            if( m_download_bitrate == -2 && !m_manager.is_live()  )
            {            
                VOLOGW( "BAInfo*** Download Fail , Need Retry! URL %s" , item.path);
                VOLOGE( "Download problem,start to wait!" );
                for( VO_S32 i = 0 ; i < 100 && !m_bMediaStop ; i++ )
                    voOS_Sleep( 20 );
            }

        } while ( m_download_bitrate == -2 && !m_bMediaStop && !m_manager.is_live() );

        if( m_download_bitrate < 0 && !m_manager.is_live() )
        {
            VOLOGW( "BAInfo*** Download Fail , Skip! URL %s" , item.path );
            m_brokencount++;
        }
        else
            m_brokencount = 0;

        if(m_manager.is_live())
        {
            TransactionForDownloadResult(m_download_bitrate);
        }

        if(m_download_bitrate == NEED_SKIP_CHUNCK)
        {
            VOLOGI("the chunck:%s in server is unavailable! Skip it", item.path);
            continue;
        }

        if( m_brokencount > 10 )
        {
            VOLOGE( "VO_LIVESRC_STATUS_NETWORKBROKEN" );
            NotifyTheNetworkBroken();
            voOS_Sleep( 100 );
            break;
        }

        if( m_download_bitrate == -2 && !m_manager.is_live() && !m_bMediaStop )
        {
            VOLOGE( "VO_LIVESRC_STATUS_NETWORKBROKEN" );
            NotifyTheNetworkBroken();
            break;
        }

        if(m_download_bitrate>0)
        {
            VOLOGI( "BAInfo***-DlSpeed: %d , Bitrate:%d" , (VO_U32)m_download_bitrate, (VO_U32)m_manager.get_cur_bandwidth() );
			VOLOGI( "BAInfo***-URL %s" , item.path );
        }

        if( m_download_bitrate == -2 )
            m_download_bitrate = -1;


        m_judgementor.add_endtime_bitrate( m_last_audio_timestamp , m_manager.get_cur_bandwidth() );

		while( m_is_pause )
		{
			voOS_Sleep( 20 );
			is_from_pause = VO_TRUE;

			if( m_bMediaStop )
				return;
		}

		m_judgementor.add_item( m_download_bitrate );
        NotifyTheBandWidth();

		m_isdeterminenextbitrate = VO_TRUE;

		//download_bitrate = m_judgementor.get_judgment( m_manager.get_cur_bandwidth() , m_is_video_delayhappen );
        //m_judgementor.get_judgment( m_manager.get_cur_bandwidth() , &m_download_bitrate , &m_bitrate_limit );
        m_judgementor.get_right_bandwidth( m_manager.get_cur_bandwidth() , &m_rightbandwidth );
		m_is_video_delayhappen = VO_FALSE;

		m_delaycount = 0;
		m_isdeterminenextbitrate = VO_FALSE;

		if( item.ptr_key )
			item.ptr_key->release();

		if( item.ptr_oldkey )
			item.ptr_oldkey->release();

		count++;

	}

    if(m_iSeekResult == -1)
    {
        VOLOGI("seek error!");
        //FlushBuffer();
        FlushDataBuffer();
    }
	
	if( !m_is_flush )
	{
		VOLOGI("Eos Ended!");
		SendEos();
		VOLOGI( "Eos Finished!" );
	}
	
	m_iSeekResult = 0;

	VOLOGI( "End of start_livestream" );

}

VO_VOID vo_http_live_streaming_new::stop_livestream( VO_BOOL isflush )
{
	if( isflush )
	{
		VOLOGI("it is flush");
	}

	VOLOGI("+stop_livestream");
	m_is_flush = isflush;
	m_bMediaStop = VO_TRUE;

	vo_thread::stop();

	m_is_flush = VO_FALSE;
	m_bMediaStop = VO_FALSE;
	VOLOGI("-stop_livestream");
}

VO_VOID vo_http_live_streaming_new::load_ts_parser()
{
#ifdef _IOS
	voGetParserAPI(&m_tsparser_api);
#else
	
	if (strlen (m_szWorkPath) > 0)
		m_dlEngine.SetWorkPath ((VO_TCHAR*)m_szWorkPath);

	VOLOGI ("Work path %s", m_szWorkPath);

	vostrcpy(m_dlEngine.m_szDllFile, _T("voTsParser"));
	vostrcpy(m_dlEngine.m_szAPIName, _T("voGetParserAPI"));

#if defined _WIN32
	vostrcat(m_dlEngine.m_szDllFile, _T(".Dll"));
#elif defined LINUX
	vostrcat(m_dlEngine.m_szDllFile, _T(".so"));
#endif

	if(m_dlEngine.LoadLib(NULL) == 0)
	{
		VOLOGE ("LoadLib fail");
		return;
	}

	pvoGetParserAPI pAPI = (pvoGetParserAPI) m_dlEngine.m_pAPIEntry;
	if (pAPI == NULL)
	{
		return;
	}

	pAPI (&m_tsparser_api);
#endif
	
	VO_PARSER_INIT_INFO info;
	info.pProc = ParserProc;
	info.pUserData = this;
	info.pMemOP = NULL;
    VO_SOURCE2_INITPARAM*       pParam = NULL;

	m_tsparser_api.Open( &m_tsparser_handle , &info );
    
    if(m_pLogParam != NULL)
    {
        m_tsparser_api.SetParam(m_tsparser_handle, VO_PID_COMMON_LOGFUNC, m_pLogParam);
    }
}

VO_VOID vo_http_live_streaming_new::free_ts_parser()
{
    if(m_tsparser_handle == NULL)
    {
        return;
    }
    
	if( m_tsparser_api.Close )
	{
		m_tsparser_api.Close( m_tsparser_handle );
		m_dlEngine.FreeLib ();
		m_tsparser_handle = 0;
	}
}

VO_S64 vo_http_live_streaming_new::GetMediaItem( media_item * ptr_item )
{
	vo_webdownload_stream http_downloader;
	vo_webdownload_stream http_olddownloader;

    VO_S64    illDownloadTime = 0;
	

	VO_BOOL is_useold = VO_FALSE;

	VO_S64 retspeed;
	VO_BOOL ret = VO_TRUE;

    VOLOGI("");

    if(ptr_item->drm_type ==  THIRD_PART_DRM)
    {
        m_bWorkModeForDiscretixPlayReady = VO_TRUE;
    }
	if( (ptr_item->eReloadType != M3U_RELOAD_NULL_TYPE) && ( strlen( ptr_item->oldpath ) != 0 ) )
	{
		m_is_bitrate_adaptation = VO_TRUE;
		is_useold = VO_TRUE;


		if( ptr_item->is_oldencrypt )
		{
		    m_bDrmNeedWork = VO_TRUE;
            if(ptr_item->drm_type == AES128)
            {
			    VOLOGI( "old key url: %s" , ptr_item->ptr_oldkey->ptr_buffer );
			    VOLOGI( "old iv: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X" , ptr_item->oldiv[0] , ptr_item->oldiv[1] , ptr_item->oldiv[2] , ptr_item->oldiv[3] , ptr_item->oldiv[4] , ptr_item->oldiv[5] ,
				         ptr_item->oldiv[6] , ptr_item->oldiv[7] , ptr_item->oldiv[8] , ptr_item->oldiv[9] , ptr_item->oldiv[10] , ptr_item->oldiv[11] , ptr_item->oldiv[12] , ptr_item->oldiv[13] , ptr_item->oldiv[14] , ptr_item->oldiv[15] );

			    if( strcmp( m_last_keyurl , ( VO_CHAR * )ptr_item->ptr_oldkey->ptr_buffer ) == 0 )
			    {
				    ret = http_olddownloader.open( ptr_item->oldpath , DOWNLOAD2MEM , NULL , ptr_item->oldiv , ptr_item->olddrm_type , &m_drm_eng , m_drm_eng_handle );
				    Prepare_HLSDRM_Process( 0 , ptr_item->oldiv );
			    }
			    else
			    {
				    memset( m_last_keyurl , 0 , 1024 );
				    strcpy( m_last_keyurl , ( VO_CHAR * )ptr_item->ptr_oldkey->ptr_buffer );
				    ret = http_olddownloader.open( ptr_item->oldpath , DOWNLOAD2MEM , ptr_item->ptr_oldkey->ptr_buffer , ptr_item->oldiv , ptr_item->olddrm_type , &m_drm_eng , m_drm_eng_handle );
				    Prepare_HLSDRM_Process( ptr_item->ptr_oldkey->ptr_buffer , ptr_item->oldiv );
                }
            }
            else if(ptr_item->drm_type ==  THIRD_PART_DRM)
            {
                VOLOGI("the sequence id:%d, Key Old Stream:%s",ptr_item->sequence_number, ptr_item->strOldEXTKEYLine);
                ret = http_olddownloader.open( ptr_item->oldpath , DOWNLOAD2MEM , NULL , NULL , ptr_item->olddrm_type , &m_drm_eng , m_drm_eng_handle );
                Prepare_HLSDRM_Process( (VO_BYTE*)(ptr_item->strOldEXTKEYLine), 0);
			}
		}
		else
        {
            m_bDrmNeedWork = VO_FALSE;
		    ret = http_olddownloader.open( ptr_item->oldpath , DOWNLOAD2MEM );
        }

		if( !ret )
		{
			VO_S32 errorcode = http_olddownloader.get_lasterror();
            if(errorcode == HTTP_NOT_FOUND)
            {
                m_is_bitrate_adaptation = VO_FALSE;
                VOLOGI("need skip chunck!");
                return NEED_SKIP_CHUNCK;                
            }

			if( errorcode != 0 && errorcode < 80 )
			{
				Event event;
				event.id = VO_LIVESRC_STATUS_DRM_ERROR;
				event.param1 = errorcode;
				memset( m_last_keyurl , 0 , sizeof(m_last_keyurl) );
				VOLOGE( "DRM Engine Error! %d" , errorcode );
                if(m_eventcallback_func != NULL)
                {
                    m_eventcallback_func( m_ptr_eventcallbackobj, event.id, event.param1, event.param2 );
                }

                m_is_bitrate_adaptation = VO_FALSE;
                return -1;
			}
            else
            {
                VOLOGI("the downloadd error code:%d", errorcode);            
                if( errorcode != CONTENT_NOTFOUND )
                {
                    m_is_bitrate_adaptation = VO_FALSE;
                    return -2;
                }
                else
                {
                    m_is_bitrate_adaptation = VO_FALSE;
                    return -1;
                }
            }
		}

        //store the old timestamp
		//m_timestamp_offset = m_last_video_timestamp > m_last_audio_timestamp ? (m_last_video_timestamp + 1) : (m_last_audio_timestamp + 1);

		GetItem( &http_olddownloader , VO_FALSE , VO_TRUE );
        illDownloadTime = http_olddownloader.GetDownLoadTime();
        if((illDownloadTime != -1) && (illDownloadTime>(VO_S64)(ptr_item->duration*1000)))
        {
            VOLOGW("the duration of clip %d,  the download time:%d", ptr_item->duration*1000, (VO_S32)illDownloadTime);
        }

		VOLOGI( "*****************Get old stream Done*****************" );
        VOLOGI("Reset the mediatype, ts will contain pure audio!");
        m_mediatype = -1;
		
	}

	if( ptr_item->is_encrypt )
	{
        m_bDrmNeedWork = VO_TRUE;
        if(ptr_item->drm_type == AES128)
        {
            VOLOGI( "key url: %s" , ptr_item->ptr_key->ptr_buffer );
		    VOLOGI( "iv: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X" , ptr_item->iv[0] , ptr_item->iv[1] , ptr_item->iv[2] , ptr_item->iv[3] , ptr_item->iv[4] , ptr_item->iv[5] ,
			    ptr_item->iv[6] , ptr_item->iv[7] , ptr_item->iv[8] , ptr_item->iv[9] , ptr_item->iv[10] , ptr_item->iv[11] , ptr_item->iv[12] , ptr_item->iv[13] , ptr_item->iv[14] , ptr_item->iv[15] );

		    if( strcmp( m_last_keyurl , ( VO_CHAR * )ptr_item->ptr_key->ptr_buffer ) == 0 )
		    {
			    ret = http_downloader.persist_open( ptr_item->path , DOWNLOAD2MEM , &m_persist  , NULL , ptr_item->iv , ptr_item->drm_type , &m_drm_eng , m_drm_eng_handle );
			    Prepare_HLSDRM_Process( 0 , ptr_item->iv );
		    }
		    else
		    {
			    memset( m_last_keyurl , 0 , 1024 );
			    strcpy( m_last_keyurl , ( VO_CHAR * )ptr_item->ptr_key->ptr_buffer );
			    VOLOGI( "ptr_item->path: %s" , ptr_item->path );
			    ret = http_downloader.persist_open( ptr_item->path , DOWNLOAD2MEM , &m_persist , ptr_item->ptr_key->ptr_buffer , ptr_item->iv , ptr_item->drm_type , &m_drm_eng , m_drm_eng_handle );
			    Prepare_HLSDRM_Process( ptr_item->ptr_key->ptr_buffer , ptr_item->iv );
		    }
        }
        else if(ptr_item->drm_type ==  THIRD_PART_DRM)
        {
            VOLOGI("in the discretix DRM, the EXT_LINE:%s", ptr_item->strEXTKEYLine);
            ret = http_downloader.persist_open( ptr_item->path , DOWNLOAD2MEM , &m_persist , NULL , NULL , ptr_item->drm_type , &m_drm_eng , m_drm_eng_handle );
            Prepare_HLSDRM_Process( (VO_BYTE* )(ptr_item->strEXTKEYLine), 0);
        }
	}
	else
    {    
        m_bDrmNeedWork = VO_FALSE;
		ret = http_downloader.persist_open( ptr_item->path , DOWNLOAD2MEM , &m_persist );
    }

	if( !ret )
	{
		VO_S32 errorcode = http_downloader.get_lasterror();

        if(errorcode == HTTP_NOT_FOUND)
        {
            m_is_bitrate_adaptation = VO_FALSE;
            VOLOGI("need skip chunck!");
            return NEED_SKIP_CHUNCK;                
        }

		if( errorcode != 0 && errorcode < 80 )
		{
			Event event;
			event.id = VO_LIVESRC_STATUS_DRM_ERROR;
			event.param1 = errorcode;
			memset( m_last_keyurl , 0 , sizeof(m_last_keyurl) );
			VOLOGE( "DRM Engine Error! %d" , errorcode );
            if(m_eventcallback_func != NULL)
            {
                m_eventcallback_func( m_ptr_eventcallbackobj, event.id, event.param1, event.param2 );
            }

            m_is_bitrate_adaptation = VO_FALSE;
            return -1;
		}
        else
        {
            VOLOGI("the downloadd error code:%d", errorcode);
            if( errorcode != CONTENT_NOTFOUND )
            {
                m_is_bitrate_adaptation = VO_FALSE;
                return -2;
            }
            else
            {
                m_is_bitrate_adaptation = VO_FALSE;
                return -1;
            }
        }
	}

	if( is_useold )
	{
		//GetItem( &http_olddownloader , VO_FALSE , VO_TRUE );

		//VOLOGI( "*****************Get old stream Done*****************" );
	}

	if( m_is_bitrate_adaptation )
	{
		m_adaptationbuffer.set_adaptation();
		//add for Bitrate adaption
		//add for Bitrate adaption		
	}

	retspeed = GetItem( &http_downloader , ptr_item->eReloadType);
    illDownloadTime = http_downloader.GetDownLoadTime();

    VOLOGI("BAInfo***  The Duration of clip %d,  The download time:%d", ptr_item->duration*1000, (VO_S32)illDownloadTime);




	if( m_is_bitrate_adaptation )
		m_adaptationbuffer.set_after_adaptation();

	m_is_bitrate_adaptation = VO_FALSE;
	m_bNeedUpdateTimeStamp = VO_FALSE;

	return retspeed;
}

VO_S64 vo_http_live_streaming_new::GetItem( vo_webdownload_stream * ptr_stream , VO_S32 eReloadType , VO_BOOL is_quick_fetch )
{
	/*static */VO_S32 process_size = 0;
	VO_PBYTE ptr_buffer = NULL;
	VO_S32 readed_size = 0;
    m_iProcessSize = 0;
    VO_SOURCEDRM_CALLBACK2 * ptr_drm = (VO_SOURCEDRM_CALLBACK2*)m_drm_eng_handle;

	m_audiocounter = m_videocounter = 0;


    if( m_pDrmCallback != NULL )
    {
        if(memcmp(ptr_drm->szDRMTYPE, "DRM_CableVision_PlayReady", strlen("DRM_CableVision_PlayReady")) == 0)
        {
            m_bDrmNeedWork = VO_TRUE;
            Prepare_HLSDRM_Process(NULL, NULL);
        }
        if(memcmp(ptr_drm->szDRMTYPE, "DRM_Discretix_PlayReady", strlen("DRM_Discretix_PlayReady")) == 0)
        {
            if(m_bWorkModeForDiscretixPlayReady == VO_FALSE)
            {
                VOLOGI("set the Discretix DRM in harmonic!");            
                m_bDrmNeedWork = VO_TRUE;
                Prepare_HLSDRM_Process(NULL, NULL);
            }
        }

    }
    
    DumpTSFromDrm(NULL, 0, COMMON_DUMP_FILE_NEW_NORMAL);


    if(COMMON_ONSTREAM2_MEDIA_AAC == m_iCurrentMediaType)
    {
        if(eReloadType != M3U_RELOAD_NULL_TYPE)
        {        
            DumpAACFromHttp(NULL, 0, COMMON_DUMP_FILE_NEW_ADAPTION);
            VOLOGI("send the aac parser reset!");
            m_aacparser_api.SetParam(m_aacparser_handle, VO_AAC2_PARAM_RESET, NULL);
        }
        else
        {
            DumpAACFromHttp(NULL, 0, COMMON_DUMP_FILE_NEW_NORMAL);
        }
    }

    if(COMMON_ONSTREAM2_MEDIA_TS == m_iCurrentMediaType)
    {
        if(eReloadType != M3U_RELOAD_NULL_TYPE)
        {
            DumpTSFromHttp(NULL, 0, COMMON_DUMP_FILE_NEW_ADAPTION);
        }
        else
        {
            DumpTSFromHttp(NULL, 0, COMMON_DUMP_FILE_NEW_NORMAL);
        }
    }


	if( (eReloadType != M3U_RELOAD_NULL_TYPE)|| m_iProcessSize == 0 )
	{
		VOLOGI("get size, the reload type:%d", eReloadType);
		m_iProcessSize = readbuffer_determinesize( &ptr_buffer , ptr_stream );
		VOLOGI( "size: %d" , m_iProcessSize );

		if( m_iProcessSize == -1 || m_iProcessSize == 0 )
		{
			VOLOGE( "Data Fatal Error!" );
			m_iProcessSize = 0;
			ptr_stream->close();


            if(m_pDrmCallback != NULL)
            {
                After_HLSDRM_Process(NULL, NULL);
            }
            
			return -1;
		}

		readed_size = m_iProcessSize;
	}
	else
	{
		VOLOGI("use size: %d" , m_iProcessSize );
		ptr_buffer = new VO_BYTE[m_iProcessSize];
	}


    if(m_bChuckMediaTypeChanged == VO_TRUE)
    {
        DoTransactionForChunkMediaTypeChanged();
        eReloadType = M3U_RELOAD_RESET_TIMESTAMP_TYPE;
        VOLOGI("The File Chunck Type changed!");
    }

    DoPrePareForTheChuckMediaParser(m_iCurrentMediaType);

	do
	{

        if( COMMON_ONSTREAM2_MEDIA_TS == m_iCurrentMediaType )
        {
		    if( readed_size )
		    {
			    VO_PARSER_INPUT_BUFFER input;
			    input.nBufLen = readed_size;
			    input.nStreamID = 0;
			    input.pBuf = ptr_buffer;

			    if( eReloadType != M3U_RELOAD_NULL_TYPE )
			    {
			        if(eReloadType == M3U_RELOAD_RESET_CONTEXT_ONLY_TYPE)
			        {
			            input.nFlag = VO_PARSER_FLAG_STREAM_CHANGED;
			        }
				    else if(eReloadType == M3U_RELOAD_RESET_TIMESTAMP_TYPE)
				    {
				        input.nFlag = VO_PARSER_FLAG_STREAM_RESET_ALL;
				    }

				    VOLOGI("the reload type is %d", eReloadType);
				    eReloadType = M3U_RELOAD_NULL_TYPE;

				    VOLOGI("needreload!");

				    m_new_video_file = VO_TRUE;
				    m_new_audio_file = VO_TRUE;

                    m_is_mediatypedetermine = VO_FALSE;
                    m_mediatype = -1;
			    }
			    else
			    {
				    //VOLOGI("do not needreload!");
				    input.nFlag = 0;
				    //m_new_video_file = VO_FALSE;

			    }

			    if( m_bMediaStop )
				    break;


                DumpTSFromHttp(ptr_buffer, readed_size, 0);

			    //VOLOGI("+m_tsparser_api.Process");
			    m_tsparser_api.Process( m_tsparser_handle , &input );
			    //VOLOGI("-m_tsparser_api.Process");

			    if( readed_size < m_iProcessSize )
			    {
				    VO_S32 errorcode = ptr_stream->get_lasterror();

				    if( errorcode != 0 )
				    {
					    Event event;
					    event.id = VO_LIVESRC_STATUS_DRM_ERROR;
					    event.param1 = errorcode;
                        if(m_eventcallback_func != NULL)
                        {
                            m_eventcallback_func( m_ptr_eventcallbackobj, event.id, event.param1, event.param2 );
                        }
				    }

				    break;
			    }
		    }

		    if( is_live() && m_is_pause )
			    break;

		    if( is_quick_fetch && m_audiocounter > MAX_FASTFETCH_FRAMECOUNT && m_videocounter > MAX_FASTFETCH_FRAMECOUNT )
			    break;

		    //VOLOGI("+read_buffer");
		    readed_size = read_buffer( ptr_stream , ptr_buffer , m_iProcessSize );

			if( readed_size < m_iProcessSize && !m_bMediaStop )
			{
				VO_U32 size = 0;
				After_HLSDRM_Process( ptr_buffer + readed_size , &size );

				readed_size += size;
			}
        }
        else
        {
            if(COMMON_ONSTREAM2_MEDIA_AAC == m_iCurrentMediaType)
            {

    		    if( readed_size )
                {
                    if(eReloadType != M3U_RELOAD_NULL_TYPE )
                    {
                        m_new_audio_file = VO_TRUE;
                        m_is_mediatypedetermine = VO_FALSE;
                        eReloadType = M3U_RELOAD_NULL_TYPE;
                        VOLOGI("Set the new audio format for AAC!");
                    }
                    
                    VO_SOURCE2_SAMPLE  varSample = {0};
                    
    			    if( m_bMediaStop )
    			    {	
    			        break;
                    }


    			    varSample.uSize = readed_size;
    			    varSample.pBuffer = ptr_buffer;

                    
                    DumpAACFromHttp(ptr_buffer, readed_size, 0);
                    
    			    m_aacparser_api.SendBuffer( m_aacparser_handle , varSample);
                }

                if( is_quick_fetch && m_audiocounter > MAX_FASTFETCH_FRAMECOUNT)
                    break;
                                
                readed_size = read_buffer( ptr_stream , ptr_buffer , m_iProcessSize );
            }
        }
	}while( !m_bMediaStop && readed_size );


	delete []ptr_buffer;

	//VOLOGI("+http_downloader.close");
	ptr_stream->close();
	//VOLOGI("-http_downloader.close");

	return ptr_stream->get_download_bitrate();
}

VO_S32 vo_http_live_streaming_new::read_buffer( vo_webdownload_stream * ptr_stream , VO_PBYTE buffer , VO_S32 size )
{    
    VO_U32   ulIndex = 0;
	VOLOGR("+Read Buffer %d",size);
	VO_U32 readed = 0;
	VO_PBYTE ptr_orgbuffer = buffer;

	while( readed < size && !m_bMediaStop )
	{
		VO_S64 readsize = ptr_stream->read( buffer , size - readed );

		if( readsize == -1 )
		{
		    if((m_pDrmCallback != NULL || m_drm_eng_handle ) && (m_bDrmNeedWork == VO_TRUE))
            {
                Do_HLSDRM_Process(ptr_orgbuffer, &readed);	
            }
			return (VO_S32)readed;
		}
		else if( readsize == -2 )
		{
			voOS_Sleep( 20 );
			continue;
		}

	    if( readsize < ( size - readed ) / 10 )
        {
            voOS_Sleep( 20 );
	    }

		readed += readsize;
		buffer = buffer + readsize;
	}


    if(( ( m_pDrmCallback != NULL || m_drm_eng_handle ) && !m_bMediaStop ) && (m_bDrmNeedWork == VO_TRUE))
    {
		Do_HLSDRM_Process(ptr_orgbuffer, &readed);        
        DumpTSFromDrm(ptr_orgbuffer, readed, 0);

		if( readed < size )
			readed += read_buffer( ptr_stream , ptr_orgbuffer + readed , size - readed );
    }

	VOLOGR("-Read Buffer %d",readed);

	return (VO_S32)readed;
}

VO_S32 vo_http_live_streaming_new::readbuffer_determinesize( VO_PBYTE * ppBuffer , vo_webdownload_stream * ptr_stream)
{
	VO_BYTE buffer[1504];
	VO_BYTE bufferForID3Header[16] = {0};    
	VO_S32 buffersize = 0;
	VO_S32 iReadSize = 188*4;
    VO_U32 ulIndex = 0;
    VO_U32 ulLen = 0;
    VO_U32 ulID3LenOffset = 6;
    VO_U32 ulDataLenForID3Header = 0;
    VO_U32 ulMediaType = COMMON_ONSTREAM2_MEDIA_UNKNOWN;


	VO_S32 readsize = read_buffer( ptr_stream , buffer , sizeof( buffer ) );
    ulMediaType = CheckFileFormat(buffer, readsize);

    VOLOGI("the current chunck's media type:%d", ulMediaType);

    if(m_iCurrentMediaType == COMMON_ONSTREAM2_MEDIA_UNKNOWN)
    {
        if(ulMediaType != COMMON_ONSTREAM2_MEDIA_UNKNOWN)
        {
            m_iCurrentMediaType = ulMediaType;
        }
    }
    else
    {
        if(ulMediaType != m_iCurrentMediaType)
        {
            m_iCurrentMediaType = ulMediaType;
            m_bChuckMediaTypeChanged = VO_TRUE;
        }
    }

    

    if(COMMON_ONSTREAM2_MEDIA_TS == m_iCurrentMediaType)
    { 
        if( readsize < ((VO_S32)(sizeof(buffer))) )
        {
            VO_PARSER_INPUT_BUFFER input;
            input.nBufLen = readsize;
            input.nStreamID = 0;
            input.pBuf = buffer;

            DumpTSFromHttp(buffer, readsize, 0);
			if(m_tsparser_handle != NULL)
			{
			    m_tsparser_api.Process( m_tsparser_handle , &input );
			}
        
            return -1;
        }
       
        CCheckTsPacketSize checker;
        VO_S32 tspackersize = checker.Check( buffer, readsize);
        
        if( tspackersize == 0 )
            return 0;
        
        buffersize = ( readsize / tspackersize + 50 ) * tspackersize;

		buffersize = ( buffersize / (188*4) ) * 188 *4;
        
        *ppBuffer = new VO_BYTE[buffersize];
        
        VO_PBYTE ptr = *ppBuffer;
        
        memcpy( ptr , buffer+ulIndex , readsize-ulIndex );
        
        ptr = ptr + readsize;
        
        read_buffer( ptr_stream , ptr , buffersize - readsize );
    }
    else
    {
        if(COMMON_ONSTREAM2_MEDIA_AAC == m_iCurrentMediaType)
        {
            
            if( readsize < 1024 )
            {
	            VO_SOURCE2_SAMPLE  varInput = {0};
	            varInput.uSize= readsize;
	            varInput.pBuffer = buffer;
                DumpAACFromHttp(buffer, readsize, 0);
				if(m_aacparser_handle != NULL)
				{
			        m_aacparser_api.SendBuffer( m_aacparser_handle , varInput);
				}
	            return -1;
            }

            buffersize = ( 1504 / (188*4) ) * 188 *8;
            *ppBuffer = new VO_BYTE[buffersize];
	        VO_PBYTE ptr = *ppBuffer;
	        memcpy( ptr , buffer , readsize );
	        ptr = ptr + readsize;

	        read_buffer( ptr_stream , ptr , buffersize - readsize );
        }
    }


	return buffersize;
}

void VO_API vo_http_live_streaming_new::ParserProc(VO_PARSER_OUTPUT_BUFFER* pData)
{
	vo_http_live_streaming_new * ptr_player = (vo_http_live_streaming_new *)pData->pUserData;

	switch ( pData->nType )
	{
	case VO_PARSER_OT_AUDIO:
		{
			if( !ptr_player->m_is_mediatypedetermine )
			{
				Event event = {0};
				event.id = VO_LIVESRC_STATUS_MEDIATYPE_CHANGE;

				VO_U32 type = VO_LIVESRC_MEDIA_AUDIOVIDEO;

				switch( ptr_player->m_mediatype )
				{
				case VO_MEDIA_PURE_AUDIO:
					VOLOGI("VO_PARSER_OT_MEDIATYPE VO_LIVESRC_MEDIA_PURE_AUDIO");
					type = VO_LIVESRC_MEDIA_PURE_AUDIO;
					break;
				case VO_MEDIA_PURE_VIDEO:
					VOLOGI("VO_PARSER_OT_MEDIATYPE VO_LIVESRC_MEDIA_PURE_VIDEO");
					type = VO_LIVESRC_MEDIA_PURE_VIDEO;
					break;
				case VO_MEDIA_AUDIO_VIDEO:
					VOLOGI("VO_PARSER_OT_MEDIATYPE VO_LIVESRC_MEDIA_AUDIOVIDEO");
					type = VO_LIVESRC_MEDIA_AUDIOVIDEO;
					break;
				}

				event.param1 = (VO_U32)&type;

				if(ptr_player->m_eventcallback_func != NULL)
				{
				    ptr_player->m_eventcallback_func( ptr_player->m_ptr_eventcallbackobj, event.id, event.param1, event.param2 );
				}
				
				ptr_player->m_is_mediatypedetermine = VO_TRUE;
			}

			VO_MTV_FRAME_BUFFER * ptr_buffer = (VO_MTV_FRAME_BUFFER *)pData->pOutputData;

			if( ptr_player->m_is_bitrate_adaptation )
			{
				ptr_player->m_adaptationbuffer.send_audio( ptr_buffer );
			}
			else
				ptr_player->audio_data_arrive( ptr_buffer );

			ptr_player->m_audiocounter++;
		}
		break;
	case VO_PARSER_OT_VIDEO:
		{
			if( !ptr_player->m_is_mediatypedetermine )
			{
				Event event = {0};
				event.id = VO_LIVESRC_STATUS_MEDIATYPE_CHANGE;

				VO_U32 type = VO_LIVESRC_MEDIA_AUDIOVIDEO;

				switch( ptr_player->m_mediatype )
				{
				case VO_MEDIA_PURE_AUDIO:
					VOLOGI("VO_PARSER_OT_MEDIATYPE VO_LIVESRC_MEDIA_PURE_AUDIO");
					type = VO_LIVESRC_MEDIA_PURE_AUDIO;
					break;
				case VO_MEDIA_PURE_VIDEO:
					VOLOGI("VO_PARSER_OT_MEDIATYPE VO_LIVESRC_MEDIA_PURE_VIDEO");
					type = VO_LIVESRC_MEDIA_PURE_VIDEO;
					break;
				case VO_MEDIA_AUDIO_VIDEO:
					VOLOGI("VO_PARSER_OT_MEDIATYPE VO_LIVESRC_MEDIA_AUDIOVIDEO");
					type = VO_LIVESRC_MEDIA_AUDIOVIDEO;
					break;
				}

				event.param1 = (VO_U32)&type;

                if(ptr_player->m_eventcallback_func != NULL)
                {
                    ptr_player->m_eventcallback_func( ptr_player->m_ptr_eventcallbackobj, event.id, event.param1, event.param2 );
                }
				ptr_player->m_is_mediatypedetermine = VO_TRUE;
			}

			VO_MTV_FRAME_BUFFER * ptr_buffer = (VO_MTV_FRAME_BUFFER *)pData->pOutputData;

			if( ptr_player->m_is_bitrate_adaptation )
			{	
				ptr_player->m_adaptationbuffer.send_video( ptr_buffer );
			}
			else
				ptr_player->video_data_arrive( ptr_buffer );

			ptr_player->m_videocounter++;
		}
		break;
	case VO_PARSER_OT_STREAMINFO:
		{
			VO_PARSER_STREAMINFO * ptr_info = ( VO_PARSER_STREAMINFO * )pData->pOutputData;

			if( ptr_info->nVideoExtraSize )
			{
				VO_MTV_FRAME_BUFFER buffer;
				memset( &buffer , 0 , sizeof(VO_MTV_FRAME_BUFFER) );
				buffer.pData = (VO_PBYTE)ptr_info->pVideoExtraData;
				buffer.nSize = ptr_info->nVideoExtraSize;
				buffer.nFrameType = 0;
				//ptr_player->video_data_arrive( &buffer );

				if( ptr_player->m_mediatype == -1 )
					ptr_player->m_mediatype = VO_MEDIA_PURE_VIDEO;
				else if( ptr_player->m_mediatype == VO_MEDIA_PURE_AUDIO )
					ptr_player->m_mediatype = VO_MEDIA_AUDIO_VIDEO;

                if( ptr_player->m_ptr_videoinfo )
                {
                    VO_PBYTE ptr = (VO_PBYTE)ptr_player->m_ptr_videoinfo;
                    delete []ptr;
                    ptr_player->m_ptr_videoinfo = 0;
                }

                if( !ptr_player->m_ptr_videoinfo )
                {
                    ptr_player->m_ptr_videoinfo = ( VO_SOURCE2_TRACK_INFO * )new VO_BYTE[ sizeof( VO_SOURCE2_TRACK_INFO) + ptr_info->nVideoExtraSize ];
                }

                memset( ptr_player->m_ptr_videoinfo , 0 , sizeof( VO_SOURCE2_TRACK_INFO ) + ptr_info->nVideoExtraSize );

                ptr_player->m_ptr_videoinfo->uTrackType = VO_SOURCE2_TT_VIDEO;
				ptr_player->m_ptr_videoinfo->uCodec = VO_VIDEO_CodingH264;
				ptr_player->m_ptr_videoinfo->sVideoInfo.sFormat.Height = ptr_info->VideoFormat.height;
				ptr_player->m_ptr_videoinfo->sVideoInfo.sFormat.Width= ptr_info->VideoFormat.width;   
				ptr_player->m_ptr_videoinfo->pHeadData = ((VO_PBYTE)ptr_player->m_ptr_videoinfo) + sizeof( VO_SOURCE2_TRACK_INFO );
				
				memcpy(ptr_player->m_ptr_videoinfo->pHeadData, ptr_info->pVideoExtraData, ptr_info->nVideoExtraSize);
                ptr_player->m_ptr_videoinfo->uHeadSize = ptr_info->nVideoExtraSize;
				VOLOGI("get the new trackinfo!");
			}

			if( ptr_info->nAudioExtraSize )
			{
				if( ptr_player->m_mediatype == -1 )
					ptr_player->m_mediatype = VO_MEDIA_PURE_AUDIO;
				else if( ptr_player->m_mediatype == VO_MEDIA_PURE_VIDEO )
					ptr_player->m_mediatype = VO_MEDIA_AUDIO_VIDEO;

                if( ptr_player->m_ptr_audioinfo && ptr_player->m_ptr_audioinfo->uHeadSize < ptr_info->nAudioExtraSize )
                {
                    VO_PBYTE ptr = (VO_PBYTE)ptr_player->m_ptr_audioinfo;
                    delete []ptr;
                    ptr_player->m_ptr_audioinfo = 0;
                }

                if( !ptr_player->m_ptr_audioinfo )
                {
                    ptr_player->m_ptr_audioinfo = ( VO_SOURCE2_TRACK_INFO * )new VO_BYTE[ sizeof( VO_SOURCE2_TRACK_INFO ) + ptr_info->nAudioExtraSize ];
                }

                memset( ptr_player->m_ptr_audioinfo , 0 , sizeof( VO_SOURCE2_TRACK_INFO )+ptr_info->nAudioExtraSize );

                ptr_player->m_ptr_audioinfo->uTrackType = VO_SOURCE2_TT_AUDIO;
                ptr_player->m_ptr_audioinfo->uCodec = VO_AUDIO_CodingAAC;
                ptr_player->m_ptr_audioinfo->sAudioInfo.sFormat.Channels = ptr_info->AudioFormat.channels;
                ptr_player->m_ptr_audioinfo->sAudioInfo.sFormat.SampleBits = ptr_info->AudioFormat.sample_bits;
				ptr_player->m_ptr_audioinfo->sAudioInfo.sFormat.SampleRate = ptr_info->AudioFormat.sample_rate;
				ptr_player->m_ptr_audioinfo->pHeadData = ((VO_PBYTE)ptr_player->m_ptr_audioinfo) + sizeof( VO_SOURCE2_TRACK_INFO );

				memcpy(ptr_player->m_ptr_audioinfo->pHeadData, ptr_info->pAudioExtraData, ptr_info->nAudioExtraSize);
                ptr_player->m_ptr_audioinfo->uHeadSize = ptr_info->nAudioExtraSize;
			}
		}
	default:
		break;
	}

}


VO_S32 VO_API vo_http_live_streaming_new::ParserProcAAC(VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData)
{
	vo_http_live_streaming_new * ptr_player = (vo_http_live_streaming_new *)pUserData;
    VO_MTV_FRAME_BUFFER   varbuffer = {0};
    VO_SOURCE2_SAMPLE*    pSample = (VO_SOURCE2_SAMPLE*)pData;

    if(pSample == NULL)
    {
        return -1;
    }

	switch (nOutputType)
	{
	    case VO_SOURCE2_TT_AUDIO:
		{
			if( !ptr_player->m_is_mediatypedetermine )
            {
                ptr_player->NotifyMediaPlayType(VO_MEDIA_PURE_AUDIO);
                ptr_player->m_is_mediatypedetermine = VO_TRUE;
                ptr_player->m_mediatype = VO_MEDIA_PURE_AUDIO;
                
                //Generate the Audio Header Data
                if( ptr_player->m_ptr_audioinfo)
                {
                    VO_PBYTE ptr = (VO_PBYTE)ptr_player->m_ptr_audioinfo;
                    delete []ptr;
                    ptr_player->m_ptr_audioinfo = 0;
                }
            
                if( !ptr_player->m_ptr_audioinfo )
                {
                    ptr_player->m_ptr_audioinfo = ( VO_SOURCE2_TRACK_INFO * )new VO_BYTE[ sizeof( VO_SOURCE2_TRACK_INFO )];
                }
            
                memset( ptr_player->m_ptr_audioinfo , 0 , sizeof( VO_SOURCE2_TRACK_INFO ));
            
                ptr_player->m_ptr_audioinfo->uTrackType = VO_SOURCE2_TT_AUDIO;
                ptr_player->m_ptr_audioinfo->uCodec = VO_AUDIO_CodingAAC;
                //Generate the Audio Header Data
                
            }         
            varbuffer.pData = pSample->pBuffer;
            varbuffer.nSize = pSample->uSize;
            varbuffer.nStartTime = pSample->uTime;
            varbuffer.nFrameType = 0;
            varbuffer.nCodecType = VO_AUDIO_CodingAAC;

            
			if( ptr_player->m_is_bitrate_adaptation )
			{
				ptr_player->m_adaptationbuffer.send_audio( &varbuffer );
			}
			else
            {         
			    ptr_player->audio_data_arrive( &varbuffer );
            }
            //VOLOGI("audio data input!, the timestamp:%lld", varbuffer.nStartTime);            
			ptr_player->m_audiocounter++;
            break;
		}

        case VO_SOURCE2_TT_HINT:
        {
            VOLOGI("get the id3!");
            ptr_player->DoTransactionForID3(pData);
        }
	    default:
        {
		    break;
        }
	}

    return 0;
}

VO_VOID vo_http_live_streaming_new::CloseAllDump()
{
    if(m_bDumpRawData == VO_TRUE)
    {
        if(g_fpVideoFromParser != NULL)
        {
            fclose(g_fpVideoFromParser);
        }
        
        if(g_fpVideoFromBufferList != NULL)
        {
            fclose(g_fpVideoFromBufferList);
        }
        
        if(g_fpAudioFromParser != NULL)
        {
            fclose(g_fpAudioFromParser);
        }
        
        if(g_fpAudioFromBufferList != NULL)
        {
            fclose(g_fpAudioFromBufferList);
        }
        
        if(g_fpAACFromHttp != NULL)
        {
            fclose(g_fpAACFromHttp);
        }
        
        if(g_fpTSFromHttp != NULL)
        {
            fclose(g_fpTSFromHttp);
        }
        
        if(g_fpID3FromParser != NULL)
        {
            fclose(g_fpID3FromParser);
        }
    }
}

VO_VOID vo_http_live_streaming_new::Prepare_HLSDRM()
{
	CvoBaseDrmCallback*   pDrmCallback = 0;
	VO_SOURCEDRM_CALLBACK2 * ptr_drm = (VO_SOURCEDRM_CALLBACK2*)m_drm_eng_handle;
 	if( ptr_drm == NULL)
	{
	    return;
	}

    if(memcmp(ptr_drm->szDRMTYPE, "DRM_VisualOn_AES128", strlen("DRM_VisualOn_AES128")) == 0)
    {
        VOLOGI("AES 128 Prepare OK!");
        return;
    }

    if(memcmp(ptr_drm->szDRMTYPE, "DRM_CableVision_PlayReady", strlen("DRM_CableVision_PlayReady")) == 0)
    {
        pDrmCallback = new CvoBaseDrmCallback((VO_SOURCEDRM_CALLBACK*)m_drm_eng_handle);
        S_IrdetoDRM_INFO      varIrdetoDrmInfo;
        varIrdetoDrmInfo.pURL = m_manager.GetM3uURLForIrdeto();
        varIrdetoDrmInfo.pManifest = m_manager.GetManifestForIrdeto();
        
        if(pDrmCallback == NULL)
        {
           VOLOGI("Prepare DRM_CableVision_PlayReady failed!");
           return;
        }
        
        pDrmCallback->DRMInfo(VO_DRMTYPE_Irdeto, (VO_PBYTE) &varIrdetoDrmInfo);
        m_pDrmCallback = (VO_VOID*)pDrmCallback;
        
        VOLOGI("Prepare DRM_CableVision_PlayReady OK!");        
        m_pDrmCallback = (VO_VOID*)pDrmCallback;
    }
    if(memcmp(ptr_drm->szDRMTYPE, "DRM_Discretix_PlayReady", strlen("DRM_Discretix_PlayReady")) == 0)
    {
        pDrmCallback = new CvoBaseDrmCallback((VO_SOURCEDRM_CALLBACK*)m_drm_eng_handle);  
        if(pDrmCallback == NULL)
        {
            VOLOGI("Prepare DRM_CableVision_PlayReady failed!");
            return;
        }
        S_DISCRETIX_INFO    varDiscretixDrmInfo;
        memset(&varDiscretixDrmInfo, 0, sizeof(S_DISCRETIX_INFO));
        varDiscretixDrmInfo.pManifestData = m_manager.GetTheTopManifestDataPointer();
        VOLOGI("%s", varDiscretixDrmInfo.pManifestData);
        varDiscretixDrmInfo.ulManifestLength = m_manager.GetTheTopManifestLength();
        VOLOGI("The manifest length:%d", varDiscretixDrmInfo.ulManifestLength);
    
        pDrmCallback->DRMInfo(VO_DRMTYPE_Discretix_HLS, (VO_PBYTE) &varDiscretixDrmInfo);
        m_pDrmCallback = (VO_VOID*)pDrmCallback;
    
        VOLOGI("Prepare DRM_Discretix_PlayReady OK!");        
        m_pDrmCallback = (VO_VOID*)pDrmCallback;
    }
}

VO_VOID vo_http_live_streaming_new::Do_HLSDRM_Process(VO_PBYTE pData, VO_U32  *ptr_ulDataLen)
{
	VO_SOURCEDRM_CALLBACK2 * ptr_drm = (VO_SOURCEDRM_CALLBACK2*)m_drm_eng_handle;
	CvoBaseDrmCallback*    pDrmCallback = (CvoBaseDrmCallback*)m_pDrmCallback;
	VO_DRM2_DATA data;
	S_HLS_DRM_PROCESS_INFO processinfo;
	S_HLS_DRM_ASSIST_INFO assistinfo;

    if(ptr_drm == NULL || m_bDrmNeedWork == VO_FALSE)
    {
        VOLOGI("DRM need not work!");
        return;
    }
    
    if(memcmp(ptr_drm->szDRMTYPE, "DRM_VisualOn_AES128", strlen("DRM_VisualOn_AES128")) == 0)
    {
	    memset( &data , 0 , sizeof( VO_DRM2_DATA ) );
	    memset( &processinfo , 0 , sizeof(S_HLS_DRM_PROCESS_INFO) );
	    memset( &assistinfo , 0 , sizeof(S_HLS_DRM_ASSIST_INFO) );

        processinfo.eDrmProcessInfo = DecryptProcess_PROCESSING;
        data.pData = pData;
        data.nSize = *ptr_ulDataLen;
        data.pnDstSize = ptr_ulDataLen;
        processinfo.pInfo = &assistinfo;
        data.sDataInfo.pInfo = &processinfo;

        if( ptr_drm )
        {
            ptr_drm->fCallback( ptr_drm->pUserData , VO_DRM_FLAG_DRMDATA , &data , 0 );
        }
    }

    if(memcmp(ptr_drm->szDRMTYPE, "DRM_CableVision_PlayReady", strlen("DRM_CableVision_PlayReady")) == 0)
    {
        if(pDrmCallback == NULL)
        {
            return;
        }
	    memset( &processinfo , 0 , sizeof(S_HLS_DRM_PROCESS_INFO) );
	    memset( &assistinfo , 0 , sizeof(S_HLS_DRM_ASSIST_INFO) );
        processinfo.eDrmProcessInfo = DecryptProcess_PROCESSING;
        processinfo.eDrmType = VO_DRM2TYPE_Irdeto;
        assistinfo.ulSequenceID = m_iLastSequenceID;
        
        processinfo.pInfo = (VO_PBYTE)&assistinfo;
        VOLOGR("+++in vo_http_live_streaming_new::DRMData_HLS_Process");            
        pDrmCallback->DRMData(VO_DRMDATATYPE_PACKETDATA, pData, *ptr_ulDataLen, NULL, 0, (VO_PBYTE)&processinfo);
        VOLOGR("---in vo_http_live_streaming_new::DRMData_HLS_Process");            
    }

    /*
    if(memcmp(ptr_drm->szDRMTYPE, "DRM_Discretix_PlayReady", strlen("DRM_Discretix_PlayReady")) == 0)
    {
        if(pDrmCallback == NULL)
        {
            return;
        }
        memset( &processinfo , 0 , sizeof(S_HLS_DRM_PROCESS_INFO) );
        memset( &assistinfo , 0 , sizeof(S_HLS_DRM_ASSIST_INFO) );
        processinfo.eDrmProcessInfo = DecryptProcess_PROCESSING;
        processinfo.eDrmType = VO_DRM2TYPE_Discretix_HLS;
        assistinfo.ulSequenceID = m_iLastSequenceID;
        
        processinfo.pInfo = (VO_PBYTE)&assistinfo;
        VOLOGR("+++in vo_http_live_streaming_new::DRMData_HLS_Process");            
        pDrmCallback->DRMData(VO_DRMDATATYPE_PACKETDATA, pData, *ptr_ulDataLen, NULL, 0, (VO_PBYTE)&processinfo);
        VOLOGR("---in vo_http_live_streaming_new::DRMData_HLS_Process");            
    }
    */
}

VO_VOID vo_http_live_streaming_new::Prepare_HLSDRM_Process( VO_BYTE * ptr_key , VO_BYTE * ptr_iv )
{
    VO_SOURCEDRM_CALLBACK2 * ptr_drm = (VO_SOURCEDRM_CALLBACK2*)m_drm_eng_handle;
    CvoBaseDrmCallback*    pDrmCallback = (CvoBaseDrmCallback*)m_pDrmCallback;
    VO_DRM2_DATA data;
    S_HLS_DRM_PROCESS_INFO processinfo;
    S_HLS_DRM_ASSIST_INFO assistinfo;
    VO_BYTE               aKeyData[32] = {0};
    VO_CHAR*              pURL = NULL;

    if(ptr_drm == NULL || m_bDrmNeedWork == VO_FALSE)
    {
        VOLOGI("DRM need not work!");
        return;
    }
    if(ptr_drm != NULL)
    {
        VOLOGI("the string of drminfo:%s, the pDrmCallback:%p", ptr_drm->szDRMTYPE, pDrmCallback);
    }
    
    if(memcmp(ptr_drm->szDRMTYPE, "DRM_VisualOn_AES128", strlen("DRM_VisualOn_AES128")) == 0)
    {        
        if( ptr_key )
        {
            VO_DRM2_INFO info;
            memset(aKeyData, 0, 32);
            memset( &info , 0 , sizeof( VO_DRM2_INFO ) );

            vo_webdownload_stream stream;
            if( !stream.open((VO_CHAR*)ptr_key, DOWNLOAD2MEM))
            {
                VOLOGI("can't get the AES Key Data!");
                return;
            }
            else
            {
                if(DownlLoadKeyForAES(&stream, aKeyData, 16) != 16)
                {
                    VOLOGI("Get the AES KEY Error!");
                    return;
                }
            }

            
            info.pDrmInfo = aKeyData;
        
            if( ptr_drm )
                ptr_drm->fCallback( ptr_drm->pUserData , VO_DRM_FLAG_DRMINFO , &info , 0 );
        }
        
        if( ptr_iv )
        {
            VO_DRM2_DATA data;
            S_HLS_DRM_PROCESS_INFO processinfo;
            S_HLS_DRM_ASSIST_INFO assistinfo;
        
            memset( &data , 0 , sizeof( VO_DRM2_DATA ) );
            memset( &processinfo , 0 , sizeof(S_HLS_DRM_PROCESS_INFO) );
            memset( &assistinfo , 0 , sizeof(S_HLS_DRM_ASSIST_INFO) );
        
            processinfo.eDrmProcessInfo = DecryptProcess_BEGIN;
            memcpy( assistinfo.aKey , ptr_iv , 16 );
            processinfo.pInfo = &assistinfo;
            data.sDataInfo.pInfo = &processinfo;
        
            if( ptr_drm )
                ptr_drm->fCallback( ptr_drm->pUserData , VO_DRM_FLAG_DRMDATA , &data , 0 );
        }
    }

    if(memcmp(ptr_drm->szDRMTYPE, "DRM_CableVision_PlayReady", strlen("DRM_CableVision_PlayReady")) == 0)
    {
        if(pDrmCallback == NULL)
        {
            return;
        }
        
        memset( &processinfo , 0 , sizeof(S_HLS_DRM_PROCESS_INFO) );
        memset( &assistinfo , 0 , sizeof(S_HLS_DRM_ASSIST_INFO) );

        processinfo.eDrmProcessInfo = DecryptProcess_BEGIN;
        processinfo.eDrmType = VO_DRM2TYPE_Irdeto;
        assistinfo.ulSequenceID = m_iLastSequenceID;
        processinfo.pInfo = (VO_PBYTE)&assistinfo;
        VOLOGR("+++in vo_http_live_streaming_new::Prepare_HLSDRM_Process");
        pDrmCallback->DRMData(VO_DRMDATATYPE_PACKETDATA, NULL, 0, NULL, 0, (VO_PBYTE)&processinfo);
        VOLOGR("---in vo_http_live_streaming_new::Prepare_HLSDRM_Process");
    }

    /*
    if(memcmp(ptr_drm->szDRMTYPE, "DRM_Discretix_PlayReady", strlen("DRM_Discretix_PlayReady")) == 0)
    {
        if(pDrmCallback == NULL)
        {
            VOLOGI("The Disctretix pDrmCallback is NULL!");
            return;
        }
        
        memset( &processinfo , 0 , sizeof(S_HLS_DRM_PROCESS_INFO) );
        memset( &assistinfo , 0 , sizeof(S_HLS_DRM_ASSIST_INFO) );

        processinfo.eDrmProcessInfo = DecryptProcess_BEGIN;
        processinfo.eDrmType = VO_DRM2TYPE_Discretix_HLS;
        if(ptr_key != NULL)
        {
            pURL = (VO_CHAR*)ptr_key;
            memcpy(assistinfo.strURL, pURL, strlen(pURL));
            VOLOGI("The EXT_KEY:%s", pURL);
        }
        
        processinfo.pInfo = (VO_PBYTE)&assistinfo;
        VOLOGR("+++in vo_http_live_streaming_new::Prepare_HLSDRM_Process");
        pDrmCallback->DRMData(VO_DRMDATATYPE_PACKETDATA, NULL, 0, NULL, 0, (VO_PBYTE)&processinfo);
        VOLOGR("---in vo_http_live_streaming_new::Prepare_HLSDRM_Process");
    }
    */
}

VO_VOID vo_http_live_streaming_new::After_HLSDRM_Process( VO_BYTE * ptr_buf , VO_U32 * ptr_size )
{
    VO_SOURCEDRM_CALLBACK2 * ptr_drm = (VO_SOURCEDRM_CALLBACK2*)m_drm_eng_handle;
    CvoBaseDrmCallback*    pDrmCallback = (CvoBaseDrmCallback*)m_pDrmCallback;
    VO_DRM2_DATA data;
    S_HLS_DRM_PROCESS_INFO processinfo;
    S_HLS_DRM_ASSIST_INFO assistinfo;


    if(ptr_drm == NULL || m_bDrmNeedWork == VO_FALSE)
    {
        return;
    }
    
    if(memcmp(ptr_drm->szDRMTYPE, "DRM_VisualOn_AES128", strlen("DRM_VisualOn_AES128")) == 0)
    {        
        memset( &data , 0 , sizeof( VO_DRM2_DATA ) );
        memset( &processinfo , 0 , sizeof(S_HLS_DRM_PROCESS_INFO) );
        memset( &assistinfo , 0 , sizeof(S_HLS_DRM_ASSIST_INFO) );
        
        processinfo.eDrmProcessInfo = DecryptProcess_END;
        data.pData = ptr_buf;
        data.pnDstSize = ptr_size;
        processinfo.pInfo = &assistinfo;
        data.sDataInfo.pInfo = &processinfo;
        
        if( ptr_drm )
        {
            ptr_drm->fCallback( ptr_drm->pUserData , VO_DRM_FLAG_DRMDATA , &data , 0 );
        }
    }

    if(memcmp(ptr_drm->szDRMTYPE, "DRM_CableVision_PlayReady", strlen("DRM_CableVision_PlayReady")) == 0)
    {
        if(pDrmCallback == NULL)
        {
            return;
        }
        
        memset( &processinfo , 0 , sizeof(S_HLS_DRM_PROCESS_INFO) );
        memset( &assistinfo , 0 , sizeof(S_HLS_DRM_ASSIST_INFO) );
        
        processinfo.eDrmProcessInfo = DecryptProcess_END;
        processinfo.eDrmType = VO_DRM2TYPE_Irdeto;
        assistinfo.ulSequenceID = m_iLastSequenceID;
        processinfo.pInfo = (VO_PBYTE)&assistinfo;
        VOLOGI("+++in vo_http_live_streaming_new::After_HLSDRM_Process");                        
        pDrmCallback->DRMData(VO_DRMDATATYPE_PACKETDATA, NULL, 0, NULL, 0, (VO_PBYTE)&processinfo);
        VOLOGI("+++in vo_http_live_streaming_new::After_HLSDRM_Process");            
    }


    /*
    if(memcmp(ptr_drm->szDRMTYPE, "DRM_Discretix_PlayReady", strlen("DRM_Discretix_PlayReady")) == 0)
    {
        if(pDrmCallback == NULL)
        {
            return;
        }
        
        memset( &processinfo , 0 , sizeof(S_HLS_DRM_PROCESS_INFO) );
        memset( &assistinfo , 0 , sizeof(S_HLS_DRM_ASSIST_INFO) );
        
        processinfo.eDrmProcessInfo = DecryptProcess_END;
        processinfo.eDrmType = VO_DRM2TYPE_Discretix_HLS;
        processinfo.pInfo = (VO_PBYTE)&assistinfo;
        VOLOGI("+++in vo_http_live_streaming_new::After_HLSDRM_Process");                        
        pDrmCallback->DRMData(VO_DRMDATATYPE_PACKETDATA, NULL, 0, NULL, 0, (VO_PBYTE)&processinfo);
        VOLOGI("+++in vo_http_live_streaming_new::After_HLSDRM_Process");
        m_bWorkModeForDiscretixPlayReady = VO_FALSE;
    }
    */
}


VO_VOID vo_http_live_streaming_new::bufferframe_callback( VO_BOOL is_video , VO_MTV_FRAME_BUFFER * ptr_buffer , VO_PTR ptr_obj )
{
	vo_http_live_streaming_new * ptr_this = (vo_http_live_streaming_new*)ptr_obj;
	if( is_video )
	{
		ptr_this->video_data_arrive( ptr_buffer );
	}
	else
	{
		ptr_this->audio_data_arrive( ptr_buffer );
	}
}

VO_VOID vo_http_live_streaming_new::audio_data_arrive( VO_MTV_FRAME_BUFFER * ptr_buffer )
{
    audio_data_arrive_in_bufferlist(ptr_buffer);
}

VO_VOID vo_http_live_streaming_new::video_data_arrive( VO_MTV_FRAME_BUFFER * ptr_buffer )
{
    video_data_arrive_in_bufferlist(ptr_buffer);
}

VO_VOID vo_http_live_streaming_new::send_media_data( VO_MTV_FRAME_BUFFER * ptr_buffer , VO_U32 index , VO_BOOL newfile )
{
}

VO_VOID vo_http_live_streaming_new::send_eos()
{
	VOLOGI("vo_http_live_streaming::send_eos");
}


void	vo_http_live_streaming_new::setWorkPath (const char * pWorkPath)
{
    memset(m_szWorkPath, 0, 256);
    strcpy (m_szWorkPath, pWorkPath);
	m_judgementor.load_config(m_szWorkPath);
}


VO_S32 vo_http_live_streaming_new::set_pos( VO_S32 pos )
{
	if( is_live() )
		return 0;

	m_is_seek = VO_TRUE;

	VO_S64 temp = pos;

	pos = pos / 1000;

	VOLOGI( "set_pos %lld" , temp );

	VOLOGI( "+stop_livestream %u" , voOS_GetSysTime() );
	need_flush();
	stop_livestream( VO_TRUE );

	VOLOGI( "++++++++++++++++++++++++++++++++++++++++++++++++m_manager.set_pos %u" , voOS_GetSysTime() );
	pos = m_manager.set_pos( pos );
	VOLOGI( "------------------------------------------------m_manager.set_pos %u" , voOS_GetSysTime() );


    //store the seek result, seek to the end
    if((pos == -1) ||(pos >= m_manager.get_duration()))
    {
        m_iSeekResult = -1;
	}
	VOLOGI( "stroe the return value of m_manager.set_pos:%d " , m_iSeekResult);
    //store the seek result
    
	VOLOGI( "the return value of m_manager.set_pos:%d " , pos);
	
	m_seekpos = temp;

    m_judgementor.flush();
    m_judgementor.get_right_bandwidth( m_manager.get_cur_bandwidth() , &m_rightbandwidth);

	if( !m_is_pause )
		run();

	return pos;
}

VO_VOID vo_http_live_streaming_new::start_after_seek()
{
    m_is_afterseek = VO_TRUE;
    need_flush();
	begin();
}

VO_VOID vo_http_live_streaming_new::perpare_drm()
{
}

VO_VOID vo_http_live_streaming_new::release_drm()
{
}

VO_VOID vo_http_live_streaming_new::set_DRM( void * ptr_drm )
{
    VO_SOURCEDRM_CALLBACK2 * pDrm = NULL;

	VOLOGI( "OK, We got DRM Engine %p" , ptr_drm );
	if( !ptr_drm )
	{
		return;
	}

    m_drm_eng_handle = ptr_drm;
    pDrm = (VO_SOURCEDRM_CALLBACK2*)m_drm_eng_handle;
    if(pDrm!= NULL)
    {
        VOLOGI("the drm string:%s", pDrm->szDRMTYPE);
    }
}

VO_VOID vo_http_live_streaming_new::run()
{
	if( m_is_seek )
	{
		start_after_seek();
		m_is_seek = VO_FALSE;
	}
	else
	{
		if( m_is_pause )
		{
			if( is_live() )
			{
				need_flush();
				m_recoverfrompause = VO_TRUE;
			}
		} 
	}

	m_is_pause = VO_FALSE;
}

VO_VOID vo_http_live_streaming_new::pause()
{
	VOLOGI( "vo_http_live_streaming::pause" );
	m_is_pause = VO_TRUE;

	if( is_live() )
	{
		need_flush();
	}
}

VO_VOID vo_http_live_streaming_new::need_flush()
{

}

VO_VOID vo_http_live_streaming_new::set_videodelay( int * videodelaytime, VO_S64* pCurrentPlayTime )
{
	VOLOGR( "+++++++++++++Video Delay Time: %d;+++++++++++input_video_timestamp:%lld" , *videodelaytime, *pCurrentPlayTime);

	if( m_isdeterminenextbitrate )
		return;

	m_delaycount++;

	if( m_delaycount == 1 && *videodelaytime < 180 )
		return;

	m_judgementor.add_delaytime( (VO_S64)*videodelaytime , *pCurrentPlayTime );
}

VO_VOID vo_http_live_streaming_new::set_libop(void*   pLibOp)
{
	VOLOGI( "set_libop");

    m_dlEngine.SetLibOperator((VO_LIB_OPERATOR *) pLibOp);
    if(m_pBufferManager != NULL)
    {
        m_pBufferManager->SetLibOp((VO_SOURCE2_LIB_FUNC *)pLibOp);
    }
}


VO_VOID vo_http_live_streaming_new::Add_AdFilterInfo(void*	 pAdFilterInfo)
{
    VO_BOOL    bFindFilter = VO_FALSE;
	VO_S32     iIndex = 0;

    S_FilterForAdvertisementIn*   pFilterInfo = (S_FilterForAdvertisementIn*)pAdFilterInfo;


	VOLOGI("pFilterInfo->strFilterString:%s", pFilterInfo->strFilterString);

	if(m_iFilterForAdvertisementCount>=8)
	{
	    VOLOGI("FilterInfo  is Full!");
		return;
	}

	if(pAdFilterInfo == NULL)
	{
	    VOLOGI("Add_AdFilterInfo  use NULL ptr!");
		return;
	}

	while((bFindFilter == VO_FALSE) && (iIndex<m_iFilterForAdvertisementCount))
	{
	    if(memcmp( pFilterInfo->strFilterString, m_aFilterForAdvertisement[iIndex].strFilterString , strlen(m_aFilterForAdvertisement[iIndex].strFilterString)) == 0)
        {
            VOLOGI("The Filter  exist!");
			bFindFilter = VO_TRUE;
        }
		iIndex++;
	}

	if(bFindFilter == VO_FALSE)
	{
	    m_aFilterForAdvertisement[m_iFilterForAdvertisementCount].iFilterId = pFilterInfo->iFilterId;		
		VOLOGI("pFilterInfo->strFilterString:%s", pFilterInfo->strFilterString);
		memcpy((VO_VOID*)(m_aFilterForAdvertisement[m_iFilterForAdvertisementCount].strFilterString), pFilterInfo->strFilterString, strlen(pFilterInfo->strFilterString));
		m_manager.AddAdFilterString(m_aFilterForAdvertisement[m_iFilterForAdvertisementCount].strFilterString);	
        m_iFilterForAdvertisementCount++;
		VOLOGI("Add The Filter!");
	}

	return;
}


VO_S32	vo_http_live_streaming_new::FindFilterIndex(VO_CHAR*  pFilterString)
{
    VO_S32   iIndex = 0;
	
    if(pFilterString == NULL)
    {
        return -1;
    }

    while(iIndex < m_iFilterForAdvertisementCount)
    {
	    if(memcmp( pFilterString, m_aFilterForAdvertisement[iIndex].strFilterString , strlen(m_aFilterForAdvertisement[iIndex].strFilterString)) == 0)
        {
            VOLOGI("The Filter  Find!");
			return iIndex;
        }
        iIndex++;
    }

	VOLOGI("Can't Find The Filter!");    
	return -1;

}

VO_VOID	vo_http_live_streaming_new::DoNotifyForAD( media_item * ptr_item, VO_S32  iFilterIndex)
{
	Event event = {0};
	
	event.id = VO_LIVESRC_STATUS_HSL_AD_APPLICATION;	
    event.param1 = (VO_U32)(ptr_item->path);
	event.param2 = (VO_U32)m_aFilterForAdvertisement[iFilterIndex].iFilterId;	

    if(m_eventcallback_func != NULL)
    {
        m_eventcallback_func( m_ptr_eventcallbackobj, event.id, event.param1, event.param2 );
    }

	return;
}


VO_VOID	vo_http_live_streaming_new::DoNotifyForThumbnail()
{
    VO_S32     ulCount = (VO_S32)m_manager.GetThumbnailItemCount();
	Event event;

	if(ulCount == 0)
	{
	    return;
	}

	S_Thumbnail_Item*    pThumbnailList = new S_Thumbnail_Item[ulCount];
	if(pThumbnailList == NULL)
	{
	    VOLOGI("Lack of memory!");
		return;
	}

    m_pThumbnailList = pThumbnailList;
	if(m_manager.FillThumbnailItem(pThumbnailList, ulCount) == ulCount)
	{
	    event.id = VO_LIVESRC_STATUS_HSL_FRAME_SCRUB;
		event.param1 = (VO_U32)(pThumbnailList);
		event.param2 = ulCount;
		
        if(m_eventcallback_func != NULL)
        {
            m_eventcallback_func( m_ptr_eventcallbackobj, event.id, event.param1, event.param2 );
        }
	}
}


VO_VOID vo_http_live_streaming_new::send_audio_trackinfo()
{
    VO_MTV_FRAME_BUFFER buffer;
    memset( &buffer , 0 , sizeof(VO_MTV_FRAME_BUFFER) );
    buffer.nSize = m_ptr_audioinfo->uHeadSize > 12 ? sizeof( VO_SOURCE2_TRACK_INFO ) + m_ptr_audioinfo->uHeadSize - 12 : sizeof( VO_SOURCE2_TRACK_INFO );
    buffer.pData = (VO_PBYTE)m_ptr_audioinfo;
    buffer.nStartTime = 0;

    send_media_data( &buffer , 0 , VO_TRUE );
}

VO_VOID vo_http_live_streaming_new::send_video_trackinfo()
{
    VO_MTV_FRAME_BUFFER buffer;
    memset( &buffer , 0 , sizeof(VO_MTV_FRAME_BUFFER) );
    buffer.nSize = m_ptr_videoinfo->uHeadSize > 12 ? sizeof( VO_SOURCE2_TRACK_INFO ) + m_ptr_videoinfo->uHeadSize - 12 : sizeof( VO_SOURCE2_TRACK_INFO );
    buffer.pData = (VO_PBYTE)m_ptr_videoinfo;
    buffer.nStartTime = 0;

    send_media_data( &buffer , 1 , VO_TRUE );
}

void	vo_http_live_streaming_new::ResetAllFilters()
{
    memset((void *)m_aFilterForAdvertisement, 0, sizeof(S_FilterForAdvertisementLoacal)*8);
	m_iFilterForAdvertisementCount = 0;
}


void	vo_http_live_streaming_new::ResetAllIDs()
{
    m_iLastCharpterID = 0;
	m_iLastSequenceID = 0;
}


void	vo_http_live_streaming_new::LoadWorkPathInfo()
{
#ifdef _LINUX_ANDROID
    char szPackageName[1024];
	FILE * hFile = fopen("/proc/self/cmdline", "rb");
	if (hFile != NULL)
	{  
	    fgets(szPackageName, 1024, hFile);
	    fclose(hFile);
	    if (strstr (szPackageName, "com.") != NULL)
		{
		    sprintf(m_szWorkPath, "/data/data/%s/", szPackageName);
	    }
	}
#endif //_LINUX_ANDROID
}

VO_VOID vo_http_live_streaming_new::setCpuInfo(VO_VOID* pCpuInfo)
{
    if(pCpuInfo != NULL)
    {
        m_judgementor.setCpuInfo(pCpuInfo);
    }
}


VO_VOID vo_http_live_streaming_new::setCapInfo(VO_VOID* pCapInfo)
{
    VO_SOURCE2_CAP_DATA*    pVarCapInfo = (VO_SOURCE2_CAP_DATA*)pCapInfo;
    if(pCapInfo != NULL)
    {
        m_judgementor.setCapInfo(pCapInfo);
        m_manager.SetCapBitrate((VO_U32) pVarCapInfo->nBitRate);
    }
}

VO_VOID vo_http_live_streaming_new::setStartCap( VO_VOID* pStartBitrate )
{
    VO_SOURCE2_CAP_DATA*    pVarCapInfo = (VO_SOURCE2_CAP_DATA*)pStartBitrate;

	if(pStartBitrate != NULL)
	{
		m_judgementor.setStartCapInfo(pStartBitrate);
        m_manager.SetStartBitrate(pVarCapInfo->nBitRate);
	}
}

VO_VOID vo_http_live_streaming_new::setTheMaxDownloadFailTolerantCount(VO_PTR   pulMax)
{
    VO_U32    ulMax = 0;
    if(pulMax != NULL)
    {
        m_manager.SetTheMaxDownloadFailForManifest(pulMax);
        ulMax = *((VO_U32*)pulMax);
        m_ulMaxDownloadFailTolerantCount = ulMax;
        VOLOGI("Set the Max Download Fail Tolerant Count:%d", ulMax);
    }
}

VO_VOID vo_http_live_streaming_new::setCPUWorkMode(VO_PTR  pParam)
{
    VO_U32    ulWorkMode = 0;
    if(pParam == NULL)
    {
        return;
    }
    
    ulWorkMode = *((VO_U32*)pParam);
    VOLOGI("Set the CPU workmode:%d", ulWorkMode);
    m_judgementor.SetTheCPUAdaptionWorkMode(ulWorkMode);
}

VO_VOID vo_http_live_streaming_new::SetVerificationInfo(VO_VOID*  psVerificationInfo)
{
    VO_CHAR*  pFind = NULL;
    VO_CHAR*  pUserAndPwd = NULL;
    VO_SOURCE2_VERIFICATIONINFO*   pVerifyInfo = (VO_SOURCE2_VERIFICATIONINFO*)psVerificationInfo;

    if(pVerifyInfo == NULL)
    {
        VOLOGI("the verification info is NULL!");
        return;
    }

    if(pVerifyInfo->uDataFlag != 1)
    {
        VOLOGI("Not Maxium verification!");
        return;
    }



    pUserAndPwd = (VO_CHAR*)(pVerifyInfo->pData);
    VOLOGI("the UserData:%s", pUserAndPwd);
    pFind = strstr(pUserAndPwd, ":");
    if(pFind == NULL)
    {
        return;
    }

    memcpy(m_sHLSUserInfo.strUserName, pUserAndPwd, pFind-pUserAndPwd);
    m_sHLSUserInfo.ulstrUserNameLen = pFind-pUserAndPwd;
    VOLOGI("the userid:%s", m_sHLSUserInfo.strUserName);

    memcpy(m_sHLSUserInfo.strPasswd, pFind+1, pVerifyInfo->uDataSize-(pFind-pUserAndPwd)-1);
    m_sHLSUserInfo.ulstrPasswdLen = pVerifyInfo->uDataSize-(pFind-pUserAndPwd)-1;    
    VOLOGI("the passwd:%s", m_sHLSUserInfo.strPasswd);
}



//Test for Buffer Manager
VO_S32  vo_http_live_streaming_new::InitDataBuffer()
{
    if(m_pBufferManager)
    {
        m_pBufferManager->Flush();
        delete m_pBufferManager;
    }

    m_pBufferManager = new voSourceBufferManager(5000, 20000);
    if(m_pBufferManager == NULL)
    {
        VOLOGI("lack of memory!");
        return -1;
    }
}


VO_S32  vo_http_live_streaming_new::UnInitDataBuffer()
{
    if(m_pBufferManager)
    {
        m_pBufferManager->Flush();
        delete m_pBufferManager;
        m_pBufferManager = NULL;
    }

    return 0;
}


/*
VO_S32 vo_http_live_streaming_new::InitDataBufferList(VO_U32  ulDataBufferId, VO_S32  iMaxSampleSize)
{
    VO_S32    iMaxBufferSize = 0;

	
    switch(ulDataBufferId)
    {
        //DataBuffer For Audio
		case VO_SOURCE2_TT_AUDIO:
		{
			iMaxBufferSize = 1024*1024;
            if(m_pDataBufferListForAudio != NULL)
            {
                return 0;
            }
			
			m_pDataBufferListForAudio= new voSource2DataBufferItemList(0, 20*1000);
			if(m_pDataBufferListForAudio == NULL)
			{
			    return -1;
			}

			return 0;
		}
		
        //DataBuffer For Video
        case VO_SOURCE2_TT_VIDEO:
		{
			iMaxBufferSize = 8*1024*1024;

			if(m_pDataBufferListForVideo != NULL )
            {
                return 0;
			}
			
			m_pDataBufferListForVideo = new voSource2DataBufferItemList(0, 20*1000);
			if(m_pDataBufferListForVideo == NULL)
			{
			    return -1;
			}

			return 0;
		}
		
		//DataBuffer For SubTitle
		case VO_SOURCE2_TT_SUBTITLE:
		{
			iMaxBufferSize = 1024*1024;

			if(m_pDataBufferListForSubTitle != NULL )
            {
                return 0;
			}
			
			m_pDataBufferListForSubTitle = new voSource2DataBufferItemList(0, 20*1000);
			if(m_pDataBufferListForSubTitle == NULL)
			{
			    return -1;
			}

			return 0;
		}

		default:
		{
			break;
		}
    }

	return -1;
}



VO_S32	vo_http_live_streaming_new::UInitDataBufferList(VO_U32	ulDataBufferId)
{
    switch(ulDataBufferId)
    {
        case VO_SOURCE2_TT_AUDIO:
		{
            if(m_pDataBufferListForAudio != NULL)
            {
                delete m_pDataBufferListForAudio;
				m_pDataBufferListForAudio = NULL;
                return 0;
            }			
			break;
		}

		case VO_SOURCE2_TT_VIDEO:
		{
            if(m_pDataBufferListForVideo != NULL)
            {
                delete m_pDataBufferListForVideo;
				m_pDataBufferListForVideo = NULL;
                return 0;
            }
			break;
		}

		
		case VO_SOURCE2_TT_SUBTITLE:
		{
            if(m_pDataBufferListForSubTitle!= NULL)
            {
                delete m_pDataBufferListForSubTitle;
				m_pDataBufferListForSubTitle = NULL;
                return 0;
            }
			break;
		}

		default:
		{
			return -1;
		}
    }

	return 0;
}
*/


VO_VOID vo_http_live_streaming_new::audio_data_arrive_in_bufferlist( VO_MTV_FRAME_BUFFER * ptr_buffer )
{
    VO_SOURCE2_SAMPLE    varSample = {0};
	VO_U32               ulRet = 0;
    VO_U64               ullTimeStamp = 0;

    VOLOGR("the audio timestamp:%lld, the frame type:%d, data size:%d", ptr_buffer->nStartTime+m_timestamp_offset, ptr_buffer->nFrameType, ptr_buffer->nSize);
	
	varSample.pBuffer = ptr_buffer->pData;
	varSample.uSize= ptr_buffer->nSize;
	
	if(((m_new_audio_file == VO_TRUE) || (ptr_buffer->nFrameType == 0xff)) && (ptr_buffer->nFrameType != 0xFE))
	{
        //Send the Audio TrackInfo First
        ullTimeStamp = ptr_buffer->nStartTime+m_timestamp_offset;
        if(m_is_mediatypedetermine == VO_TRUE && m_mediatype == VO_MEDIA_PURE_AUDIO)
        {
            VOLOGI("send the video track disable!");
            SendMediaTrackDisable(VO_SOURCE2_TT_VIDEO, ullTimeStamp);
        }
        
		SendAudioTrackinfo(ullTimeStamp);
        
		m_new_audio_file = VO_FALSE;
	}
	else
	{
	    varSample.uFlag= 0;
	}


	varSample.uTime = ptr_buffer->nStartTime + m_timestamp_offset;
	m_last_audio_timestamp =  varSample.uTime;
    DumpInputAudio(&varSample);

    ulRet = m_pBufferManager->AddBuffer(VO_SOURCE2_TT_AUDIO, &varSample);
    while((ulRet != VO_ERR_NONE) &&(m_bMediaStop == VO_FALSE))
    {
        VOLOGR("can't input the audio sample, the ret value is %d", ulRet);
        voOS_Sleep( 500 );
        ulRet = m_pBufferManager->AddBuffer(VO_SOURCE2_TT_AUDIO, &varSample);
    }
    
}

VO_VOID vo_http_live_streaming_new::subtitle_data_arrive_in_bufferlist(VO_PTR  pData)
{
    VO_U32   ulRet = 0;
    if(m_pBufferManager != NULL && pData != NULL)
    {
        ulRet = m_pBufferManager->AddBuffer(VO_SOURCE2_TT_SUBTITLE,  pData);
        while((ulRet != VO_ERR_NONE) &&(m_bMediaStop == VO_FALSE))
        {
            VOLOGR("can't input the subtitle sample!");
            ulRet = m_pBufferManager->AddBuffer(VO_SOURCE2_TT_SUBTITLE,  pData);
        }
        
        VOLOGR("input the subtitle sample!");
    }
}



VO_VOID vo_http_live_streaming_new::video_data_arrive_in_bufferlist( VO_MTV_FRAME_BUFFER * ptr_buffer )
{
    VO_SOURCE2_SAMPLE    varSample = {0};
	VO_U32               ulRet = 0;
    VO_U64               ullTimeStamp = 0;


    VOLOGR("the video timestamp:%lld, the frame type:%d, data length:%d", ptr_buffer->nStartTime+m_timestamp_offset, ptr_buffer->nFrameType, ptr_buffer->nSize);

	if(  m_is_first_frame && ptr_buffer->nFrameType != 0 )
    {
        m_is_first_frame = VO_FALSE;
		VOLOGI( "No Key Frame Arrive!" );
		return;
	}
	
	m_is_first_frame = VO_FALSE;

	
	varSample.pBuffer = ptr_buffer->pData;
	varSample.uSize = ptr_buffer->nSize;

	if(((m_new_video_file == VO_TRUE) || (ptr_buffer->nFrameType == 0xff)) && (ptr_buffer->nFrameType != 0xFE))
	{
        //Send the Video TrackInfo First
        ullTimeStamp = ptr_buffer->nStartTime+m_timestamp_offset;
        SendVideoTrackinfo(ullTimeStamp);

        if(m_is_mediatypedetermine == VO_TRUE && m_mediatype == VO_MEDIA_PURE_VIDEO)
        {
            VOLOGI("send the audio track disable!");
            SendMediaTrackDisable(VO_SOURCE2_TT_AUDIO, ullTimeStamp);
        }
        
	    varSample.uFlag = VO_SOURCE2_FLAG_SAMPLE_KEYFRAME;
		m_new_video_file = VO_FALSE;
	}
	else
	{
	    varSample.uFlag = 0;
	}

 	if( ptr_buffer->nFrameType == 0 || ptr_buffer->nFrameType == 0xff )
 	{
 		varSample.uFlag = varSample.uFlag | VO_SOURCE2_FLAG_SAMPLE_KEYFRAME;		
		VOLOGI( "Key Frame Arrive!" );
 	}


    varSample.uTime = ptr_buffer->nStartTime + m_timestamp_offset;	
	m_last_video_timestamp =  varSample.uTime;
    DumpInputVideo(&varSample);

    ulRet = m_pBufferManager->AddBuffer(VO_SOURCE2_TT_VIDEO, &varSample);
    while((ulRet != VO_ERR_NONE) &&(m_bMediaStop == VO_FALSE))
    {
        VOLOGR("can't input the video sample, the ret value is %d", ulRet);
        voOS_Sleep( 500 );
        ulRet = m_pBufferManager->AddBuffer(VO_SOURCE2_TT_VIDEO, &varSample);
    }

}

VO_VOID vo_http_live_streaming_new::FlushDataBuffer()
{
    if(m_pBufferManager != NULL)
    {
        m_pBufferManager->Flush();
    }
}

VO_VOID vo_http_live_streaming_new::FlushBuffer()
{
/*
    if(m_pDataBufferListForAudio != NULL)
    {   
        voOS_Sleep( 20 );
        m_pDataBufferListForAudio->Flush();
    }
	
    if(m_pDataBufferListForVideo!= NULL)
    {
    	voOS_Sleep( 20 );
        m_pDataBufferListForVideo->Flush();
    }

    if(m_pDataBufferListForSubTitle!= NULL)
    {
        voOS_Sleep( 20 );
	    m_pDataBufferListForSubTitle->Flush();
    }

	VOLOGI("Flush all the buffer!");
*/
}


//Test for Buffer Manager




VO_VOID vo_http_live_streaming_new::ResetAllContext()
{
    m_judgementor.reset_all();
	m_adaptationbuffer.reset_all();
	m_manager.close();
	free_ts_parser();
	release_drm();
	
    m_is_first_frame = VO_TRUE;
	m_bEosReach = VO_FALSE;
    m_new_video_file = VO_TRUE;
    m_new_audio_file = VO_TRUE;
	m_bMediaStop = VO_FALSE;
    m_is_pause = VO_FALSE;
    m_recoverfrompause = VO_FALSE;
    m_is_flush = VO_FALSE;
    m_is_seek = VO_FALSE;
    m_ulCheckTime = 0;

    m_keytag = -1;
    m_drm_eng_handle = 0;
    m_is_video_delayhappen = VO_FALSE;
    m_is_bitrate_adaptation = VO_FALSE;
    m_audiocounter = 0;
    m_videocounter = 0;
    m_last_big_timestamp = 0;
    m_timestamp_offset = 0;
    m_seekpos = 0;
    m_is_afterseek = VO_FALSE;
    m_mediatype = -1;
    m_is_mediatypedetermine = VO_FALSE;
    m_iDumpVideoCount = 0;
    m_iDumpAudioCount = 0;
    m_iDumpVideoOutputCount = 0;
    m_iDumpAudioOutputCount = 0;
    m_ulAACDumpCount = 0;
    m_ulTSDumpCount= 0;
    m_ulTSFromDrm = 0;
    m_ulForceSelect = 0;
    m_ulSelectBandwidth = 0;

    m_datacallback_func = 0;
    m_ptr_callbackobj = NULL;
	m_eventcallback_func = 0;
	m_ptr_eventcallbackobj = NULL;
    m_bNeedBuffering = VO_TRUE;
    m_aacparser_handle = NULL;
    m_pLogParam = NULL;
        

	if(m_ptr_audioinfo != NULL)
	{
	    delete m_ptr_audioinfo;
		m_ptr_audioinfo = NULL;
	}


	if(m_ptr_videoinfo != NULL)
	{
	    delete m_ptr_videoinfo;
		m_ptr_videoinfo = NULL;
	}


	if(m_pDrmCallback != NULL)
	{
		CvoGenaralDrmCallback*	 pDrmCallback = NULL;
		pDrmCallback = (CvoGenaralDrmCallback*)m_pDrmCallback;
		delete pDrmCallback;
	    m_pDrmCallback = NULL;
	}


	if(m_pThumbnailList != NULL)
    {
        delete   []m_pThumbnailList;
		m_pThumbnailList = NULL;
    }

    
	memset(m_strManifestURL, 0, 1024);
    m_last_audio_timestamp = -1;
    m_last_video_timestamp = -1;
    m_download_bitrate = -1;
    m_rightbandwidth = -1;
    m_brokencount = 0;

	m_is_first_getdata = VO_TRUE;
    m_bDrmNeedWork = VO_FALSE;
    m_ulMaxDownloadFailTolerantCount = DEFAULT_MAX_DOWNLOAD_FAILED_TOLERANT_COUNT;    
    m_ulCurrentDownloadFailConsistentCount = 0;
    DeleteAllProgramInfo();
}


VO_VOID vo_http_live_streaming_new::SetTheCurrentSelStream(VO_U32 uBitrate)
{
    VO_U32   ulIndex = 0;
    if(m_pProgramInfo != NULL)
    {

        for(ulIndex=0; ulIndex<m_pProgramInfo->uStreamCount; ulIndex++)
        {
            if(m_pProgramInfo->ppStreamInfo[ulIndex] != NULL)
            {
                m_pProgramInfo->ppStreamInfo[ulIndex]->uSelInfo = 0;
            }
        }

    
        for(ulIndex=0; ulIndex<m_pProgramInfo->uStreamCount; ulIndex++)
        {
            if((m_pProgramInfo->ppStreamInfo[ulIndex] != NULL) && (m_pProgramInfo->ppStreamInfo[ulIndex]->uBitrate == uBitrate))
            {
                m_pProgramInfo->ppStreamInfo[ulIndex]->uSelInfo = 1;
                VOLOGI("the current sel stream id:%d", m_pProgramInfo->ppStreamInfo[ulIndex]->uStreamID);
                break;
            }
        }
    }

    return;
}




VO_VOID vo_http_live_streaming_new::DoTransactionForID3(VO_PTR pData)
{
/*
    ID3Frame*     pID3Frame = (ID3Frame*)pData;
    voSubtitleInfo      varSubtitleInfo = {0};
    SubtitleInfoEntry   varSubtitleInfoEntry = {0};
    voSubtitleDisplayInfo    varSubtitleDisplayInfo = {0};
    voSubtitleImageInfo      varSubtitleImageInfo = {0};
    voSubtitleImageInfoData  varSubtitleImageInfoData = {0};
    voSubtitleImageInfoDescriptor   varSubtitleImagInfoDesc = {0};
    
    if(pID3Frame != NULL)
    {
        switch(pID3Frame->nSubHead)
        {
            case SUBHEAD_HLS_TIMESTAMP:
            {
                VOLOGI("get the hls timestamp");
                break;
            }
            case SUBHEAD_CUSTOMER_PIC_JPEG:
            case SUBHEAD_CUSTOMER_PIC_PNG:    
            {
                VOLOGI("get the pic !");
                if(pID3Frame == NULL)
                {
                    return;
                }

                DumpInputID3(pData);
                
                varSubtitleImageInfoData.nSize = pID3Frame->nDataLength;
                varSubtitleImageInfoData.pPicData = pID3Frame->pFrameData;

                varSubtitleImageInfo.stImageData = varSubtitleImageInfoData;

                varSubtitleDisplayInfo.pImageInfo = &varSubtitleImageInfo;
                
                varSubtitleInfoEntry.stSubtitleDispInfo = varSubtitleDisplayInfo;
                varSubtitleInfoEntry.nDuration = 0xffffffff;
                
                varSubtitleInfoEntry.stSubtitleRectInfo.stRect.nBottom = 0xffffffff;
                varSubtitleInfoEntry.stSubtitleRectInfo.stRect.nLeft = 0xffffffff;
                varSubtitleInfoEntry.stSubtitleRectInfo.stRect.nRight = 0xffffffff;
                varSubtitleInfoEntry.stSubtitleRectInfo.stRect.nTop = 0xffffffff;

                varSubtitleInfo.nTimeStamp = m_last_audio_timestamp+1;
                varSubtitleInfo.pSubtitleEntry = &varSubtitleInfoEntry;

                subtitle_data_arrive_in_bufferlist(&varSubtitleInfo);
                break;
            }            
        }
    }
   */
    VO_SOURCE2_SAMPLE    varSample = {0};
    VO_U32               ulRet = 0;
    VO_U64               ullTimeStamp = 0;    
    ID3Frame*     pID3Frame = (ID3Frame*)pData;
    
    if(pID3Frame != NULL)
    {
        switch(pID3Frame->nSubHead)
        {
            case SUBHEAD_HLS_TIMESTAMP:
            {
                VOLOGI("get the hls timestamp");
                break;
            }
            case SUBHEAD_CUSTOMER_PIC_JPEG:
            case SUBHEAD_CUSTOMER_PIC_PNG:    
            {
                VOLOGI("get the pic !");
                if(pID3Frame == NULL)
                {
                    return;
                }

                DumpInputID3(pData);
                varSample.uFlag = VO_SOURCE2_FLAG_SAMPLE_HINT_IMAGE;
                varSample.pBuffer = pID3Frame->pFrameData;
                varSample.uSize = pID3Frame->nDataLength;
                varSample.uTime= m_last_audio_timestamp;

                ulRet = m_pBufferManager->AddBuffer(VO_SOURCE2_TT_AUDIO, &varSample);
                while((ulRet != VO_ERR_NONE) &&(m_bMediaStop == VO_FALSE))
                {
                    VOLOGR("can't input the ID3 sample, the ret value is %d", ulRet);
                    voOS_Sleep( 500 );
                    ulRet = m_pBufferManager->AddBuffer(VO_SOURCE2_TT_AUDIO, &varSample);
                }
                
                break;
            }            
        }
    }

}


VO_VOID vo_http_live_streaming_new::SendAudioTrackinfo(VO_U64   ullTimeStamp)
{
    VO_SOURCE2_SAMPLE    varSample = {0};
	VO_U32   ulRet = 0;

	varSample.uSize = 0;
	varSample.uFlag = VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT;
    varSample.pFlagData = m_ptr_audioinfo;
	varSample.pBuffer = NULL;
	varSample.uTime = ullTimeStamp;


    DumpInputAudio(&varSample);  
    VOLOGI("the new audio format: the codec type:%d, timestamp:%lld", m_ptr_audioinfo->uCodec, ullTimeStamp);
	ulRet = m_pBufferManager->AddBuffer(VO_SOURCE2_TT_AUDIO, &varSample);
	while((ulRet != 0) &&(m_bMediaStop == VO_FALSE))
	{
	    voOS_Sleep( 20 );
        ulRet = m_pBufferManager->AddBuffer(VO_SOURCE2_TT_AUDIO, &varSample);
		VOLOGR("can't input the audio trackinfo, the ret value %d", ulRet);				
	}
}

VO_VOID vo_http_live_streaming_new::SendVideoTrackinfo(VO_U64   ullTimeStamp)
{
    VO_SOURCE2_SAMPLE    varSample = {0};
	VO_U32   ulRet = 0;
	
	varSample.uSize = 0;
	varSample.uFlag = VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT;
    varSample.pFlagData = m_ptr_videoinfo;
	varSample.pBuffer = NULL;
	varSample.uTime = ullTimeStamp;

    DumpInputVideo(&varSample);
    VOLOGI("the new format: height:%d, width:%d, header Len:%d, timestamp:%lld", m_ptr_videoinfo->sVideoInfo.sFormat.Height, m_ptr_videoinfo->sVideoInfo.sFormat.Width, m_ptr_videoinfo->uHeadSize, ullTimeStamp);
	ulRet = m_pBufferManager->AddBuffer(VO_SOURCE2_TT_VIDEO, &varSample);
	while((ulRet != 0) &&(m_bMediaStop == VO_FALSE))
	{
	    voOS_Sleep( 20 );
        ulRet = m_pBufferManager->AddBuffer(VO_SOURCE2_TT_VIDEO, &varSample);
		VOLOGR("can't input the video trackinfo, the ret value %d", ulRet);				
	}
}

VO_VOID vo_http_live_streaming_new::SendEos()
{
	voOS_Sleep( 20 );
	
    VO_SOURCE2_SAMPLE    varSample = {0};
    VO_U32   ulRet = 0;
	VO_CHAR   data[20];
	
	varSample.uSize = 20;
    varSample.uFlag = VO_SOURCE2_FLAG_SAMPLE_EOS;
	varSample.pBuffer = (VO_PBYTE)data;


    if(m_pBufferManager != NULL)
    {
        varSample.uTime = m_last_video_timestamp + 1;
        ulRet = m_pBufferManager->AddBuffer(VO_SOURCE2_TT_VIDEO, &varSample);
        while(ulRet == VO_ERR_RETRY)
        {
            voOS_Sleep( 500 );
            ulRet = m_pBufferManager->AddBuffer(VO_SOURCE2_TT_VIDEO, &varSample);
        }

	    VOLOGI("send the EOS of video");
    }


    if(m_pBufferManager!= NULL)
    {
        varSample.uTime = m_last_audio_timestamp + 1;
        ulRet = m_pBufferManager->AddBuffer(VO_SOURCE2_TT_AUDIO, &varSample);
        while(ulRet == VO_ERR_RETRY)
        {
            voOS_Sleep( 500 );
            ulRet = m_pBufferManager->AddBuffer(VO_SOURCE2_TT_AUDIO, &varSample);
        }
            
	    VOLOGI("send the EOS of audio");

    }


	m_bEosReach = VO_TRUE;
}


VO_VOID vo_http_live_streaming_new::SendMediaTrackDisable(VO_U32   ulTrackType, VO_U64 ullTimeStamp)
{
    VO_SOURCE2_SAMPLE    varSample = {0};
    VO_U32   ulRet = 0;
    
    switch(ulTrackType)
    {
        case VO_SOURCE2_TT_AUDIO:
        {
            varSample.uFlag = VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE;
            varSample.uTime = ullTimeStamp;
            ulRet = m_pBufferManager->AddBuffer(VO_SOURCE2_TT_AUDIO, &varSample);
            while(ulRet == VO_ERR_RETRY)
            {
                voOS_Sleep( 50 );
                ulRet = m_pBufferManager->AddBuffer(VO_SOURCE2_TT_AUDIO, &varSample);
            }

            VOLOGI("set the audio track disable!");
            break;
        }

        case VO_SOURCE2_TT_VIDEO:
        {
            varSample.uFlag = VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE;            
            varSample.uTime = ullTimeStamp;
            ulRet = m_pBufferManager->AddBuffer(VO_SOURCE2_TT_VIDEO, &varSample);
            while(ulRet == VO_ERR_RETRY)
            {
                voOS_Sleep( 50 );
                ulRet = m_pBufferManager->AddBuffer(VO_SOURCE2_TT_VIDEO, &varSample);
            }

            VOLOGI("set the video track disable!");
            break;            
        }        

        case VO_SOURCE2_TT_SUBTITLE:
        {
            varSample.uFlag = VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE;
            ulRet = m_pBufferManager->AddBuffer(VO_SOURCE2_TT_SUBTITLE, &varSample);
            while(ulRet == VO_ERR_RETRY)
            {
                voOS_Sleep( 50 );
                ulRet = m_pBufferManager->AddBuffer(VO_SOURCE2_TT_SUBTITLE, &varSample);
            }
    
            VOLOGI("set the subtitle track disable!");            
            break;
        }
        default:
        {
            break;
        }
    }

    return;
}


VO_VOID vo_http_live_streaming_new::NotifyMediaPlayType(VO_U32   ulMediaPlayType)
{

}

VO_VOID vo_http_live_streaming_new::NotifyTheBandWidth() 
{
    VO_S64      illBandwidth = m_manager.get_cur_bandwidth();
    VO_U32      ulBitrate = (VO_U32)illBandwidth;
    if(illBandwidth != 0x7fffffffffffffffll)
    {
        VOLOGI( "VO STATUS_HSL_BITRATE, bitrate:%d", ulBitrate);
        if(m_eventcallback_func != NULL)
        {
            m_eventcallback_func(m_ptr_eventcallbackobj, VO_EVENTID_SOURCE2_HLS_BITRATE_NOTIFY, ulBitrate, NULL);
        }
    }
}

VO_VOID  vo_http_live_streaming_new::NotifyTheNetworkBroken()
{
    VOLOGI( "The Critical Download Fail!");
    if(m_eventcallback_func != NULL)
    {
        m_eventcallback_func(m_ptr_eventcallbackobj, VO_EVENTID_SOURCE2_ERR_DOWNLOADFAIL, NULL, NULL);
    }
}

VO_VOID  vo_http_live_streaming_new::TransactionForDownloadResult(VO_S64 illDownloadResult)
{
    if(illDownloadResult<0)
    {
        m_ulCurrentDownloadFailConsistentCount++;
        if(m_ulMaxDownloadFailTolerantCount != 0 && m_ulCurrentDownloadFailConsistentCount>0 && (m_ulCurrentDownloadFailConsistentCount%m_ulMaxDownloadFailTolerantCount) == 0)
        {
            NotifyBeginWaitingDownloadFailRecover();
        }
    }
    else
    {
        if(m_ulCurrentDownloadFailConsistentCount > m_ulMaxDownloadFailTolerantCount)
        {
            NotifyRecoverFromTheDownloadFail();
        }            
        m_ulCurrentDownloadFailConsistentCount = 0;
    }
}

VO_VOID  vo_http_live_streaming_new::NotifyRecoverFromTheDownloadFail()
{
    VOLOGI( "VO STATUS_HSL RECOVER FROM DOWNLOAD FAIL!");
    if(m_eventcallback_func != NULL)
    {
        m_eventcallback_func(m_ptr_eventcallbackobj, VO_EVENTID_SOURCE2_DOWNLOAD_FAIL_RECOVER_SUCCESS, NULL, NULL);
    }
}

VO_VOID  vo_http_live_streaming_new::NotifyBeginWaitingDownloadFailRecover()
{
    VOLOGI( "VO STATUS_HSL DOWNLOAD FAIL CONSISTENT TIME:%d!", m_ulCurrentDownloadFailConsistentCount);
    if(m_eventcallback_func != NULL)
    {
        m_eventcallback_func(m_ptr_eventcallbackobj, VO_EVENTID_SOURCE2_START_DOWNLOAD_FAIL_WAITING_RECOVER, NULL, NULL);
    }
}

VO_U32  vo_http_live_streaming_new::CheckFileFormat(VO_PBYTE  pData, VO_U32 ulLen)
{
    if(pData == NULL && ulLen < 1024)
    {
        VOLOGI("The data for check file format is too little!");
        return COMMON_ONSTREAM2_MEDIA_UNKNOWN;
    }
    else
    {
        if(pData[0] == 'I' && pData[1] == 'D' && pData[2] == '3')
        {
            return COMMON_ONSTREAM2_MEDIA_AAC;
        }

    
        CCheckTsPacketSize checker;
        VO_S32 tspackersize = checker.Check(pData , ulLen);   
        if( tspackersize == 0 )
        {
            return COMMON_ONSTREAM2_MEDIA_UNKNOWN;
        }

        return COMMON_ONSTREAM2_MEDIA_TS;
    }
}

VO_VOID vo_http_live_streaming_new::DoTransactionForChunkMediaTypeChanged()
{
    VOLOGI("media chunck changed!");
    m_bChuckMediaTypeChanged = VO_FALSE;
}


VO_U32 vo_http_live_streaming_new::DoPrePareForTheChuckMediaParser(VO_U32   ulMediaPlayType)
{
    VO_U32  ulRet = 0;

    switch(ulMediaPlayType)
    {
        case COMMON_ONSTREAM2_MEDIA_TS:
        {
            if(m_tsparser_handle == NULL)
            {
                VOLOGI("change the media type to ts!");
                if( !m_manager.is_live() )
                {
                    m_timestamp_offset = m_manager.GetSequenceIDTimeOffset(m_ulCurrentSequenceIDForTimeStamp)*1000;
                    VOLOGI("the new timestamp offset:%d!", m_timestamp_offset);
                }
                else
                {
                    m_timestamp_offset = m_last_audio_timestamp;
                }

                m_iCurrentMediaType = COMMON_ONSTREAM2_MEDIA_TS;
                FreeAACParser();
                free_ts_parser();
                load_ts_parser();                
                ulRet = M3U_RELOAD_RESET_TIMESTAMP_TYPE;
            }
            break;
        }

        case COMMON_ONSTREAM2_MEDIA_AAC:
        {
            if(m_aacparser_handle == NULL)
            {
                VOLOGI("change the media type to aac!");
                m_iCurrentMediaType = COMMON_ONSTREAM2_MEDIA_AAC;
                free_ts_parser();
                FreeAACParser();
                LoadAACParser();
                if( !m_manager.is_live())
                {
                    m_timestamp_offset = m_manager.GetSequenceIDTimeOffset(m_ulCurrentSequenceIDForTimeStamp)*1000; 
                    m_last_audio_timestamp = m_timestamp_offset;
                }
                else
                {
                    m_timestamp_offset = m_last_audio_timestamp > m_last_video_timestamp ? m_last_audio_timestamp : m_last_video_timestamp;
                    m_iCurrentMediaType = COMMON_ONSTREAM2_MEDIA_AAC;
                }

                //SendMediaTrackDisable(VO_SOURCE2_TT_VIDEO);
                ulRet = M3U_RELOAD_RESET_TIMESTAMP_TYPE;
            }
            
            break;
        }
    }

    return ulRet;
}


VO_VOID vo_http_live_streaming_new::LoadAACParser()
{
#if defined _IOS
	voGetSource2AACAPI(&m_aacparser_api);
#else
    if (strlen (m_szWorkPath) > 0)
    {
        m_dlEngine.SetWorkPath ((VO_TCHAR*)m_szWorkPath);
    }
    
    VOLOGI ("Work path %s", m_szWorkPath);
    vostrcpy(m_dlEngine.m_szDllFile, _T("voAudioFR"));
    vostrcpy(m_dlEngine.m_szAPIName, _T("voGetSource2AACAPI"));
        
#if defined _WIN32
    vostrcat(m_dlEngine.m_szDllFile, _T(".Dll"));
#elif defined LINUX
     vostrcat(m_dlEngine.m_szDllFile, _T(".so"));
#endif
    
    if(m_dlEngine.LoadLib(NULL) == 0)
    {
        VOLOGE ("LoadLib fail");
        return;
    }
    
    pvoGetSource2ParserAPI  pAPI = (pvoGetSource2ParserAPI) m_dlEngine.m_pAPIEntry;
    if (pAPI == NULL)
    {
            return;
    }

	pAPI (&m_aacparser_api);    
#endif
    
    VO_SOURCE2_INITPARAM        varInitParam;
    VO_SOURCE2_SAMPLECALLBACK   varCallback;
    VO_SOURCE2_INITPARAM*       pParam = NULL;
    
    
	varCallback.pUserData = this;
	varCallback.SendData = ParserProcAAC;
    varInitParam.uFlag = VO_PID_SOURCE2_SAMPLECALLBACK;
    varInitParam.pInitParam = &varCallback;


	if(m_aacparser_api.Init(&m_aacparser_handle, NULL, VO_PID_SOURCE2_SAMPLECALLBACK, &varInitParam) == 0)
    {
        VOLOGI("load aac ok!");
        if(m_pLogParam != NULL)
        {
            m_aacparser_api.SetParam(m_aacparser_handle, VO_PID_COMMON_LOGFUNC, m_pLogParam);
        }
    }   

}


VO_VOID vo_http_live_streaming_new::FreeAACParser() 
{
    if(m_aacparser_handle != 0)
    {
	    if( m_aacparser_api.Uninit)
	    {
		    m_aacparser_api.Uninit( m_aacparser_handle );
		    m_dlEngine.FreeLib ();
		    m_aacparser_handle = 0;
	    }
    }
}



VO_VOID vo_http_live_streaming_new::SetParamToParser()
{
    if(m_tsparser_handle != NULL)
    {
        if(m_pLogParam != NULL)
        {
            m_tsparser_api.SetParam(m_tsparser_handle, VO_PID_COMMON_LOGFUNC, m_pLogParam);
        }
    }

    if(m_aacparser_handle != NULL)
    {
        if(m_pLogParam != NULL)
        {
            m_aacparser_api.SetParam(m_aacparser_handle, VO_PID_COMMON_LOGFUNC, m_pLogParam);
        }
    }
}

VO_VOID vo_http_live_streaming_new::SetParamForLog(VO_VOID*  pParam)
{
    m_pLogParam = pParam;
}



void    vo_http_live_streaming_new::DumpTSFromHttp(VO_BYTE*   pData, VO_U32 ulDataLen, VO_U32 ulFlag)
{
    VO_CHAR   strFilePath[256] = {0};
    memset(strFilePath, 0, 256);

    if(m_bDumpRawData == VO_FALSE)
    {
        return;
    }

    
    if(ulFlag != 0)
    {
		if( !g_fpTSFromHttp )
		{
			sprintf(strFilePath, "/sdcard/DumpTSFromHttp_%d.ts", m_ulTSDumpCount);
			g_fpTSFromHttp = fopen( strFilePath , "wb+" );
			m_ulTSDumpCount++;
		}
    }

    if(g_fpTSFromHttp != NULL && pData!= NULL)
    {
        fwrite(pData, 1, ulDataLen, g_fpTSFromHttp);
        fflush(g_fpTSFromHttp);
    }

}

void  vo_http_live_streaming_new::DumpTSFromDrm(VO_BYTE*   pData, VO_U32 ulDataLen, VO_U32 ulFlag)
{
    VO_CHAR   strFilePath[256] = {0};
    memset(strFilePath, 0, 256);

    if(m_bDumpRawData == VO_FALSE)
    {
        return;
    }

    
    if(ulFlag != 0)
    {
		if( !g_fpFileForJpDrm)
		{
			sprintf(strFilePath, "/sdcard/DumpTSFromDRM_%d.ts", m_ulTSFromDrm);
			g_fpFileForJpDrm = fopen( strFilePath , "wb+" );
            if(g_fpFileForJpDrm == NULL)
            {
                VOLOGI("Open file error!");
            }
            else
            {
                VOLOGI("Create file OK!");
            }
			m_ulTSFromDrm++;
		}
    }

    if(g_fpFileForJpDrm != NULL && pData!= NULL)
    {
        VOLOGI("DRM Dump size:%d", ulDataLen);
        fwrite(pData, 1, ulDataLen, g_fpFileForJpDrm);
        fflush(g_fpFileForJpDrm);
    }
}


void    vo_http_live_streaming_new::DumpAACFromHttp(VO_BYTE*   pData, VO_U32 ulDataLen, VO_U32 ulFlag)
{
    VO_CHAR   strFilePath[256] = {0};
    memset(strFilePath, 0, 256);

    if(m_bDumpRawData == VO_FALSE)
    {
        return;
    }

    
    if(ulFlag != 0)
    {
        if(g_fpAACFromHttp)
        {
            fclose(g_fpAACFromHttp);
        }

        sprintf(strFilePath, "/sdcard/DumpAACFromHttp_%d.aac", m_ulAACDumpCount);
        g_fpAACFromHttp = fopen( strFilePath , "wb+" );
        m_ulAACDumpCount++;
    }

    if(g_fpAACFromHttp != NULL && pData!= NULL)
    {
        fwrite(pData, 1, ulDataLen, g_fpAACFromHttp);
        fflush(g_fpAACFromHttp);
    }
}


void    vo_http_live_streaming_new::DumpInputVideo(VO_PTR  pSample)
{
    VO_CHAR   strFilePath[256] = {0};
    VO_SOURCE2_SAMPLE*    pvarSample = (VO_SOURCE2_SAMPLE* )pSample;

    if(m_bDumpRawData == VO_FALSE)
    {
        return;
    }
    
    if(pvarSample->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT)    
    {
        memset(strFilePath, 0, 256);
        sprintf(strFilePath, "/sdcard/DumpH264FromParser_%d.h264", m_iDumpVideoCount);
        m_iDumpVideoCount++;  

        if(g_fpVideoFromParser!= NULL)
        {
            fclose(g_fpVideoFromParser);
        }
        
        g_fpVideoFromParser = fopen(strFilePath, "wb");

		if( g_fpVideoFromParser )
		{
			VO_SOURCE2_TRACK_INFO * ptr_info = (VO_SOURCE2_TRACK_INFO *)pvarSample->pBuffer;
			return;
		}
    }


    if(g_fpVideoFromParser != NULL)
    {
        VOLOGR("the input video sample timestamp:%lld, size:%d", pvarSample->uTime, pvarSample->uSize); 
        fwrite(pvarSample->pBuffer, 1, pvarSample->uSize, g_fpVideoFromParser);
    }

    return;
}
void    vo_http_live_streaming_new::DumpInputAudio(VO_PTR  pSample)
{    
    VO_CHAR   strFilePath[256] = {0};
    VO_SOURCE2_SAMPLE*    pvarSample = (VO_SOURCE2_SAMPLE* )pSample;
    
    if(m_bDumpRawData == VO_FALSE)
    {
        return;
    }

    if(pvarSample->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT)
    {
        sprintf(strFilePath, "/sdcard/DumpAACFromParser_%d.aac", m_iDumpAudioCount);
        m_iDumpAudioCount++;
    
        if(g_fpAudioFromParser!= NULL)
        {
            fclose(g_fpAudioFromParser);
        }
        g_fpAudioFromParser = fopen(strFilePath, "wb+");

        return;
    }


    if(g_fpAudioFromParser != NULL)
    {
        VOLOGR("the input audio sample timestamp:%lld, size:%d", pvarSample->uTime, pvarSample->uSize);    
        fwrite(pvarSample->pBuffer, 1, pvarSample->uSize, g_fpAudioFromParser);
    }

    return;
}

void    vo_http_live_streaming_new::DumpInputID3(VO_PTR  pSample)
{
    VO_CHAR  strDumpPath[256] = {0};
    ID3Frame*   pID3Frame = NULL;
    FILE*       pFile = NULL;


    if(m_bDumpRawData == VO_FALSE)
    {
        return;
    }

    if(pSample == NULL)
    {
        return;
    }

    pID3Frame = (ID3Frame*)(pSample);
    
    memset(strDumpPath, 0, 256);
    sprintf(strDumpPath, "/sdcard/DumpID3_%d.dat", m_iLastSequenceID);
    pFile = fopen(strDumpPath, "wb+");
    if(pFile != NULL)
    {
        fwrite(pID3Frame->pFrameData, 1, pID3Frame->nDataLength , pFile);
        fclose(pFile);
    }
}



void    vo_http_live_streaming_new::DumpOutputVideo(VO_PTR  pSample)
{
    VO_CHAR  strDumpPath[256] = {0};
    VO_SOURCE2_SAMPLE*    pvarSample = (VO_SOURCE2_SAMPLE* )pSample;

    if(m_bDumpRawData == VO_FALSE)
    {
        return;
    }
      
    if(pvarSample->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT)
    {
        if(g_fpVideoFromBufferList!= NULL)
        {
            fclose(g_fpVideoFromBufferList);
        }
    
        sprintf(strDumpPath, "/sdcard/DumpH264FromBufferList_%d.h264", m_iDumpVideoOutputCount);
        g_fpVideoFromBufferList = fopen(strDumpPath, "wb");
        m_iDumpVideoOutputCount++;
        return;
    }
    
    if(pvarSample->uFlag == 0 || pvarSample->uFlag == VO_SOURCE2_FLAG_SAMPLE_KEYFRAME)
    {
        if(g_fpVideoFromBufferList != NULL)
        {
            VOLOGR("the output video sample timestamp:%lld, size:%d", pvarSample->uTime, pvarSample->uSize);            
            fwrite(pvarSample->pBuffer, 1, pvarSample->uSize, g_fpVideoFromBufferList);
        }
    }
    
    return;
}
void    vo_http_live_streaming_new::DumpOutputAudio(VO_PTR  pSample)
{
    VO_CHAR  strDumpPath[256] = {0};
    VO_SOURCE2_SAMPLE*    pvarSample = (VO_SOURCE2_SAMPLE* )pSample;

    if(m_bDumpRawData == VO_FALSE)
    {
        return;
    }

    if(pvarSample->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT)
    {
        if(g_fpAudioFromBufferList!= NULL)
        {
            fclose(g_fpAudioFromBufferList);
        }
    
        sprintf(strDumpPath, "/sdcard/DumpAACFromBufferList_%d.aac", m_iDumpAudioOutputCount);
        g_fpAudioFromBufferList = fopen(strDumpPath, "wb");
        m_iDumpAudioOutputCount++;        
        return;
    }
    
    if(pvarSample->uFlag == 0 || pvarSample->uFlag == VO_SOURCE2_FLAG_SAMPLE_KEYFRAME)
    {    
        if(g_fpAudioFromBufferList != NULL)
        {
            VOLOGR("the output audio sample timestamp:%lld, size:%d", pvarSample->uTime, pvarSample->uSize);            
            fwrite(pvarSample->pBuffer, 1, pvarSample->uSize, g_fpAudioFromBufferList);
        }
    }          
    return;
}


void    vo_http_live_streaming_new::DeleteAllProgramInfo()
{
	if(m_pProgramInfo != NULL)
	{
	    if(m_pProgramInfo->ppStreamInfo != NULL)
	    {
			for( int i = 0 ; i < m_pProgramInfo->uStreamCount ; i++ )
				delete m_pProgramInfo->ppStreamInfo[i];

			delete []m_pProgramInfo->ppStreamInfo;
	    }

		delete m_pProgramInfo;
		m_pProgramInfo = NULL;
	}
}


//the new interface 
VO_U32	   vo_http_live_streaming_new::Init_HLS(VO_PTR	pSource, VO_U32   nFlag, VO_SOURCE2_INITPARAM*  pInitParam)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	//char*    pTest = "https://sta1.mdialog.com/video_assets/c6bb28ec-71dc-11e1-8150-001517c3c824/streams/43cc1a0c-71e3-11e1-8150-001517c3c24/manifest.m3u8";

	char*   pURL = (char*)pSource;
	ResetAllContext();
	LoadWorkPathInfo();
	ResetAllFilters();

    if(pSource == NULL)
    {
        VOLOGI("the pSource is NULL!");
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }



	memcpy(m_strManifestURL, pURL, strlen(pURL));
    VOLOGI("the URL:%s", pURL);

	//only for test
	//memcpy(m_strManifestURL, pTest, strlen(pTest));

	//set the datacallback and eventcallback
	
    
	if(pInitParam != NULL)
    {
    
    }
    //set the datacallback and eventcallback

	return ulRet;
}


VO_U32	   vo_http_live_streaming_new::Uninit_HLS()	
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;
	ResetAllContext();
    Close_HLS();
    DeleteAllProgramInfo();

    return ulRet;
}
VO_U32	   vo_http_live_streaming_new::Open_HLS()
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;
	VO_U32   ulThumbnailCount = 0;


    ResetAllIDs();
	VOLOGI( "vo_http_live_streaming::open" );
	close();
	perpare_drm();
	m_is_first_frame = VO_TRUE;
	m_is_first_start = VO_FALSE;

    m_judgementor.load_config( m_szWorkPath );

    if((m_sHLSUserInfo.ulstrUserNameLen >0) || (m_sHLSUserInfo.ulstrPasswdLen >0))
	{
	    VOLOGI("set the userinfo");
	    m_manager.SetParamForHttp(1, &m_sHLSUserInfo);
	}


	VOLOGI("+m_manager.set_m3u_url");    
    m_manager.SetRemovePureAudio(VO_FALSE);
	if( !m_manager.set_m3u_url( m_strManifestURL ) )
	{
		VOLOGE( "First set m3u url failed!" );
		return VO_RET_SOURCE2_OPENFAIL;
	}
	VOLOGI("-m_manager.set_m3u_url");

	//Analysis the ManifestURL ok
    m_pProgramInfo = new VO_SOURCE2_PROGRAM_INFO;
    if(m_pProgramInfo == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

    memset(m_pProgramInfo, 0, sizeof(VO_SOURCE2_PROGRAM_INFO));
    m_pProgramInfo->ppStreamInfo = NULL;    


	ulThumbnailCount = m_manager.GetThumbnailItemCount();
	if(m_manager.GetThumbnailItemCount() != 0)
	{
	    DoNotifyForThumbnail();
	}


    VOLOGI( "++++++++++++++++Bitrate" );
    VO_S32 playlist_count = 0;
    m_manager.get_all_bandwidth( 0 , &playlist_count );
    VOLOGI( "Playlist Count: %d" , playlist_count );
    if( playlist_count != 0 )
    {
        VO_S64 * ptr_playlist = new VO_S64[playlist_count];

        if( ptr_playlist )
        {
            m_manager.get_all_bandwidth( ptr_playlist , &playlist_count );

            for( VO_S32 i = 0 ; i < playlist_count ; i++ )
            {
                VOLOGI( "Bitrate: %lld" , ptr_playlist[i] );
            }

            m_judgementor.set_playlist( ptr_playlist , playlist_count );
        }
    }
    VOLOGI( "+++++++++++++++++++++++" );

    m_judgementor.set_default_streambandwidth( m_manager.get_cur_bandwidth() );

    //m_judgementor.get_right_bandwidth( m_manager.get_cur_bandwidth() , &m_rightbandwidth );

    if(m_drm_eng_handle != NULL)
    {
        Prepare_HLSDRM();
    }

	VOLOGI("+load_ts_parser");
	load_ts_parser();
	VOLOGI("-load_ts_parser");

	m_bMediaStop = VO_FALSE;
    return ulRet;
}

VO_U32	   vo_http_live_streaming_new::Close_HLS()
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;
	CvoGenaralDrmCallback*   pDrmCallback = NULL;
	VOLOGI("vo_http_live_streaming::close");
	stop_livestream( VO_TRUE );    
    //FlushBuffer();
    FlushDataBuffer();
    SendEos();
	m_manager.close();
	free_ts_parser();
    FreeAACParser();    
	release_drm();

	pDrmCallback = (CvoGenaralDrmCallback*)m_pDrmCallback;

    CloseAllDump();
    
	if(m_pDrmCallback != NULL)
	{
	    delete pDrmCallback;
		m_pDrmCallback = NULL;
	}


	if(m_pThumbnailList != NULL)
    {
        delete   []m_pThumbnailList;
		m_pThumbnailList = NULL;
    }

    DeleteAllProgramInfo();


    if(m_ptr_audioinfo != NULL)
    {
        delete   m_ptr_audioinfo;
        m_ptr_audioinfo = NULL;
    }

    if(m_ptr_videoinfo != NULL)
    {
        delete   m_ptr_videoinfo;
        m_ptr_videoinfo = NULL;        
    }

	memset (&m_tsparser_api, 0, sizeof (VO_PARSER_API));
	return ulRet;
}
	
VO_U32	   vo_http_live_streaming_new::Start_HLS()
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if(m_is_first_start == VO_FALSE)
	{
        if(m_is_pause == VO_FALSE)
        {
           //FlushBuffer();
           FlushDataBuffer();
		}
	
	    begin();
		m_is_first_start = VO_TRUE;
		return ulRet;
    }


	if( m_is_seek )
	{
		start_after_seek();
		m_is_seek = VO_FALSE;
	}
	else
	{
		if( m_is_pause )
		{
			if( is_live() )
			{
				need_flush();
				m_recoverfrompause = VO_TRUE;
			}
		}
	}

	m_is_pause = VO_FALSE;

    return ulRet;
}

VO_U32	   vo_http_live_streaming_new::Pause_HLS()
{
	m_is_pause = VO_TRUE;
	return VO_RET_SOURCE2_OK;
}

VO_U32	   vo_http_live_streaming_new::Stop_HLS()
{
    VOLOGI("Stop HLS");
    stop_livestream(VO_TRUE);
    //FlushBuffer();
    FlushDataBuffer();
    SendEos();
    DeleteAllProgramInfo();
	return VO_RET_SOURCE2_OK;
}

VO_U32	   vo_http_live_streaming_new::Seek_HLS(VO_U64*  pTimeStamp)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;
	VO_S64   ulInTime = (VO_S64)(*pTimeStamp);
	VO_S32 pos = 0;
	if( is_live() )
	{
	    *pTimeStamp = 0;
		return ulRet;
	}

	if(pTimeStamp == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	m_is_seek = VO_TRUE;

	pos = (VO_S32)((*pTimeStamp) / 1000);

	VOLOGI( "set_pos %lld" , (*pTimeStamp) );

	VOLOGI( "+stop_livestream %u" , voOS_GetSysTime() );
	stop_livestream( VO_TRUE );
	//FlushBuffer();
    FlushDataBuffer();

	VOLOGI( "++++++++++++++++++++++++++++++++++++++++++++++++m_manager.set_pos %u" , voOS_GetSysTime() );
	pos = m_manager.set_pos( pos );
	VOLOGI( "------------------------------------------------m_manager.set_pos %u" , voOS_GetSysTime() );


    //store the seek result, seek to the end
    if((pos == -1) ||(pos >= m_manager.get_duration()))
    {
        m_iSeekResult = -1;
	}
	VOLOGI( "stroe the return value of m_manager.set_pos:%d, the return value of m_manager.set_pos:%d " , m_iSeekResult, pos);
	
    m_judgementor.flush();
    m_judgementor.get_right_bandwidth( m_manager.get_cur_bandwidth() , &m_rightbandwidth);
    m_seekpos = *pTimeStamp;

	m_pBufferManager->set_pos( m_seekpos );

    if( !m_is_pause )
	{ 
	    run();
    }

    m_is_first_frame = VO_TRUE;

	return ulRet;
}

VO_U32	   vo_http_live_streaming_new::GetDuration_HLS(VO_U64 * pDuration)
{
    *pDuration = (VO_U64)m_manager.get_duration();
    return VO_RET_SOURCE2_OK;
}
	
VO_U32	   vo_http_live_streaming_new::GetProgramCounts_HLS(VO_U32*	pProgramCounts)
{
    //Currently it's count is 1
    *pProgramCounts = 1;
	return VO_RET_SOURCE2_OK;
}


VO_U32	 vo_http_live_streaming_new::GetProgramInfo_HLS(VO_U32 nProgramId, VO_SOURCE2_PROGRAM_INFO **ppStreamInfo)
{

    VO_U32   ulRet = VO_RET_SOURCE2_OK;

    VO_S64   illBandwidthCurrent = 0;
 
	VO_U32   ulCount = 0;
	VO_U32   ulIndex = 0;
    VO_U32   ulBitrate = 0;
	VO_U32   ulChunkCount = 0;
	VO_CHAR  strURL[1024] = {0};

    
	nProgramId = 1;
	VO_U32						nStreamID;				
	VO_CHAR						strStreamName[256];		
	VO_U32						nDuration;				

	VO_U32						nStreamCount;		
	VO_SOURCE2_STREAM_INFO	    *pStreamInfo;		
	

    if(m_pProgramInfo == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

    ulCount = m_manager.get_playlist_count();

    m_pProgramInfo->uProgramID = nProgramId;
    m_pProgramInfo->sProgramType = (m_manager.is_live())?(VO_SOURCE2_STREAM_TYPE_LIVE):(VO_SOURCE2_STREAM_TYPE_VOD);

    VOLOGI("the stream count:%d", m_manager.get_playlist_count());
    m_pProgramInfo->uStreamCount = m_manager.get_playlist_count();

    m_pProgramInfo->ppStreamInfo = new VO_SOURCE2_STREAM_INFO*[m_pProgramInfo->uStreamCount];

	for( int i = 0 ; i < m_pProgramInfo->uStreamCount ; i++ )
		m_pProgramInfo->ppStreamInfo[i] = new VO_SOURCE2_STREAM_INFO;

    while(ulIndex < ulCount)
    {
    	m_manager.get_playlist_info_by_index(ulIndex, strURL, &ulBitrate, &ulChunkCount);
        m_pProgramInfo->ppStreamInfo[ulIndex]->uStreamID = (nProgramId << 7) + (ulIndex+1);
        m_pProgramInfo->ppStreamInfo[ulIndex]->uBitrate = ulBitrate;
        m_pProgramInfo->ppStreamInfo[ulIndex]->uTrackCount = 0;
        m_pProgramInfo->ppStreamInfo[ulIndex]->uSelInfo = 0;
        m_pProgramInfo->ppStreamInfo[ulIndex]->ppTrackInfo = NULL;

        VOLOGI("%d stream with the the stream id of %d, bitrate of %d, SelInfo of %d", ulIndex, m_pProgramInfo->ppStreamInfo[ulIndex]->uStreamID, 
                m_pProgramInfo->ppStreamInfo[ulIndex]->uBitrate, m_pProgramInfo->ppStreamInfo[ulIndex]->uSelInfo);        
		ulIndex++;
    }


    
    illBandwidthCurrent = m_manager.get_cur_bandwidth();
    SetTheCurrentSelStream(VO_U32(illBandwidthCurrent));

    *ppStreamInfo = m_pProgramInfo;
	return VO_RET_SOURCE2_OK;	
}


VO_U32    vo_http_live_streaming_new::GetCurSelTrackInfo_HLS(VO_U32 nOutPutType , VO_SOURCE2_TRACK_INFO **ppTrackInfo)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;
    if(ppTrackInfo == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

	switch(nOutPutType)
	{
	    case VO_SOURCE2_TT_VIDEO:
		{
            if(m_ptr_videoinfo != NULL)
            {
                *ppTrackInfo = m_ptr_videoinfo;
            }
			break;
		}

		case VO_SOURCE2_TT_AUDIO:
		{
            if(m_ptr_audioinfo != NULL)
            {
                *ppTrackInfo = m_ptr_audioinfo;
            }
 			break;
		}

		default:
		{
            *ppTrackInfo = NULL;
			break;
		}
	}


	return ulRet;
    
}


VO_U32   vo_http_live_streaming_new::SelectProgram_HLS(VO_U32 nProgramID)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	return ulRet;
}


VO_U32   vo_http_live_streaming_new::SelectStream_HLS(VO_U32 nStreamID)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;
    VO_U32   ulIndex = 0;


    VOLOGI("select stream");

    if(m_pProgramInfo == NULL)
    {
        return ulRet;
    }

    if(nStreamID == 0)
    {
        VOLOGI("auto select!");
        m_ulForceSelect = 0;
        return ulRet;
    }

    for(ulIndex=0; ulIndex<m_pProgramInfo->uStreamCount; ulIndex++)
    {
        if(m_pProgramInfo->ppStreamInfo[ulIndex] != NULL)
        {
            m_pProgramInfo->ppStreamInfo[ulIndex]->uSelInfo = 0;
        }
    }

    
    for(ulIndex=0; ulIndex<m_pProgramInfo->uStreamCount; ulIndex++)
    {
        if(m_pProgramInfo->ppStreamInfo[ulIndex] != NULL)
        {
            if(nStreamID == m_pProgramInfo->ppStreamInfo[ulIndex]->uStreamID)
            {
                m_ulForceSelect = 1;
                m_ulSelectBandwidth = m_pProgramInfo->ppStreamInfo[ulIndex]->uBitrate;
                m_pProgramInfo->ppStreamInfo[ulIndex]->uSelInfo = 1;
                VOLOGI("forse select!, the stream id:%d", m_pProgramInfo->ppStreamInfo[ulIndex]->uStreamID);
                break;
            }
        }
    }

	return ulRet;
}


VO_U32   vo_http_live_streaming_new::SelectTrack_HLS(VO_U32 nTrackID)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	return ulRet;
}


VO_U32	 vo_http_live_streaming_new::GetDRMInfo_HLS(VO_U32 nStreamId, VO_SOURCE2_DRM_INFO **ppDRMInfo)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;
    if(ppDRMInfo == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }
	else
	{
	    VO_SOURCE2_DRM_INFO*   pDrmInfo = new VO_SOURCE2_DRM_INFO;
		pDrmInfo->pDRM_Info = NULL;
		*ppDRMInfo = pDrmInfo;
		return ulRet;		
	}
}


VO_U32	  vo_http_live_streaming_new::GetSample_HLS(VO_U32  nSampleType, VO_PTR  pSample)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;
    VO_SOURCE2_SAMPLE*    pVarSample = NULL;
	VO_U32   index = nSampleType;
	VO_S32	 iErrType = VO_ERR_NONE;

    VO_U32   ulInputDuration = 0;
    VO_U64   ullInputTime = 0;
    VO_U32   ulInputFlag = 0;
    VO_U32   ulCurrentCheckTime = 0;
    VO_S32   iDelayTime = 0;
    VO_S64   illTimeInput = 0;


    if(pSample == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }
    pVarSample = (VO_SOURCE2_SAMPLE* )pSample;
        
    switch(index)
    {
        case VO_SOURCE2_TT_AUDIO:
        {
            m_illLastAudioOutputTime = pVarSample->uTime;
            ulRet = m_pBufferManager->GetBuffer(VO_SOURCE2_TT_AUDIO, pVarSample);
            break;
        }
        case VO_SOURCE2_TT_VIDEO:
        {
            ulInputDuration = pVarSample->uDuration;
            ullInputTime = pVarSample->uTime;
            ulInputFlag = pVarSample->uFlag;
            ulRet = m_pBufferManager->GetBuffer(VO_SOURCE2_TT_VIDEO, pVarSample);
            break;          
        }
        case VO_SOURCE2_TT_SUBTITLE:
        {
            //VOLOGI("try get the subtitle sample!");
            ulRet = m_pBufferManager->GetBuffer(VO_SOURCE2_TT_SUBTITLE, pVarSample);
            break;          
        }
        default:
        {
            return VO_RET_SOURCE2_ERRORDATA;
        }
    }


    if(ulRet != VO_ERR_NONE )
    {
        VOLOGR("the media:%d, return value of get buffer:%d", index, ulRet);
        return ulRet;
    }

    switch(index)
    {
        case VO_SOURCE2_TT_AUDIO:
        {
            VOLOGR("get the audio sample with Flag:%d, timestamp:%lld, size:%d", pVarSample->uFlag, pVarSample->uTime, pVarSample->uSize);

            switch(pVarSample->uFlag)
            {
                case VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT:
                {
                    VOLOGI("Get the Audio Track Info, with the timestamp:%lld", pVarSample->uTime);
                    break;
                }
                case VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE:
                {
                    VOLOGI("Get the Audio Track Unavailable Flag, with the timestamp:%lld", pVarSample->uTime);
                    break;
                }
            }
            
            DumpOutputAudio(pVarSample);
            break;
        }
        case VO_SOURCE2_TT_VIDEO:
        {
            VOLOGR("get the video sample with Flag:%d, input duration:%d, the input timestamp:%lld", pVarSample->uFlag, ulInputDuration, ullInputTime);

            switch(pVarSample->uFlag)
            {
                case VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT:
                {
                    VOLOGI("Get the Video Track Info, with the timestamp:%lld", pVarSample->uTime);
                    break;
                }
                case VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE:
                {
                    VOLOGI("Get the Video Track Unavailable Flag, with the timestamp:%lld", pVarSample->uTime);
                    break;
                }
            }
            
            if(m_ulCheckTime == 0)
            {
                m_ulCheckTime = voOS_GetSysTime();
            }
            else
            {
                ulCurrentCheckTime = voOS_GetSysTime();

                if(pVarSample->pReserve1 != NULL)
                {
                    if((*(VO_U32*)(pVarSample->pReserve1)) == VO_SOURCE2_FLAG_SAMPLE_FRAMEDROPPED)
                    {
                        VOLOGI("Buffer Manager Drop Frame Happen!, set the delay time 500ms");
                        ulRet = VO_ERR_NONE;
                    }
                }

                if((ulCurrentCheckTime-m_ulCheckTime) > 1000)
                {
                    m_ulCheckTime = ulCurrentCheckTime;
                    iDelayTime = (VO_S32)ulInputDuration;

                    //Use Audio Output Time, temp transaction
                    illTimeInput = (VO_S64)m_illLastAudioOutputTime;
                    VOLOGR("set the video delay time: %d, the input time:%lld", iDelayTime, illTimeInput);
                    set_videodelay((int*)&iDelayTime, &illTimeInput);
                }
            }
            DumpOutputVideo(pVarSample);
            break;          
        }
        case VO_SOURCE2_TT_SUBTITLE:
        {
            VOLOGI("got the subtitle sample!");            
            break;          
        }
        default:
        {
            break;
        }
    }
    
    return ulRet;
}


VO_U32	  vo_http_live_streaming_new::DownlLoadKeyForAES(vo_webdownload_stream *ptr_stream, VO_PBYTE buffer, VO_U32 ulSize)
{
    VO_U32 ulReaded = 0;
    VO_S64 illReadPerTime = 0;
    
    while( ulReaded < ulSize )
    {
        illReadPerTime = ptr_stream->read( buffer , ulSize - ulReaded );
        if( illReadPerTime == -1 )
        {
            return (VO_S32) ulReaded;
        }
        else if( illReadPerTime == -2 )
        {
            voOS_Sleep( 20 );
            continue;
        }
    
        ulReaded += (VO_U32)illReadPerTime;
        buffer = buffer + illReadPerTime;
    }
    
    return (VO_U32)ulReaded;

}


