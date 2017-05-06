#include "vo_http_live_streaming.h"
#include "voLog.h"
#include "voOSFunc.h"
#include "voThread.h"
#include "CCheckTsPacketSize.h"
#include "vo_aes_engine.h"

#include "voHLSDRM.h"
#include "CvoBaseDrmCallback.h"

#ifdef _HLS_SOURCE_
#include "vompType.h"
#endif
#include "vo_drm_mem_stream.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


#ifndef LOG_TAG
#define LOG_TAG "vo_http_live_streaming"
#endif



typedef VO_S32 ( VO_API *pvoGetParserAPI)(VO_PARSER_API * pParser);
typedef VO_S32 ( VO_API *pvoGetSource2ParserAPI)(VO_SOURCE2_API* pParser);

#ifndef MAX_FASTFETCH_FRAMECOUNT
#define MAX_FASTFETCH_FRAMECOUNT 50
#endif  


#define  COMMON_MEDIA_TS     0
#define  COMMON_MEDIA_AAC    1

FILE * g_fpaacin;
FILE * g_fpaacout;
//extern DRMHANDLE g_engine;

vo_http_live_streaming::vo_http_live_streaming(void)
:m_judgementor(5)
,m_is_first_frame(VO_TRUE)
,m_new_video_file(VO_TRUE)
,m_new_audio_file(VO_TRUE)
,m_bMediaStop(VO_FALSE)
,m_is_pause( VO_FALSE )
,m_recoverfrompause( VO_FALSE )
,m_is_flush(VO_FALSE)
,m_is_seek(VO_FALSE)
{
	m_datacallback_func = 0;
	m_keytag = -1;
	m_drm_eng_handle = NULL;
	m_eventcallback_func = 0;
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

	memset (&m_tsparser_api, 0, sizeof (VO_PARSER_API));

    memset (&m_aacparser_api, 0, sizeof (VO_PARSER_API));
    
	strcpy (m_szWorkPath, "");
	memset( m_last_keyurl , 0 , 1024 );
	memset( &m_drm_eng , 0 , sizeof( DRM_Callback ) );

	m_adaptationbuffer.set_callback( this , bufferframe_callback );
    m_iCurrentMediaType = COMMON_MEDIA_TS;

	m_pDrmCallback = NULL;
	m_iSeekResult = 0;
	m_bNeedUpdateTimeStamp = VO_FALSE;
	m_iVideoDelayCount = 0;


	LoadWorkPathInfo();
	ResetAllFilters();

    m_iDrmType = (VO_S32)VO_DRMTYPE_Irdeto;
	m_iProcessSize = 0;

	m_pThumbnailList = NULL;

	memset(&m_sHLSUserInfo, 0 ,sizeof(S_HLS_USER_INFO));
    m_drm_eng_handle = NULL;

	//g_fp = fopen( "/sdcard/videodump.h264" , "wb+" );
    //g_fp2 = fopen( "/sdcard/dump.ts" , "wb+" );

	//For Test Ad
	//S_FilterForAdvertisementIn    sFilterTest;
	//sFilterTest.iFilterId = 3;
	//strcpy(sFilterTest.strFilterString, "#EXT-VO-ADINF");

	//Add_AdFilterInfo((void *) &sFilterTest);
	//For Test Ad

    g_fpaacin = fopen( "/sdcard/pureaacin.aac" , "wb+" );
    g_fpaacout = fopen( "/sdcard/pureaacout.aac" , "wb+" );
}

vo_http_live_streaming::~vo_http_live_streaming(void)
{
 	/*if( g_fp )
 		fclose(g_fp);
 	g_fp = 0;*/

    /*if( g_fp2 )
        fclose( g_fp2 );
    g_fp2 = 0;*/

    if(g_fpaacin)
    {
        fclose(g_fpaacin);
    }

    if(g_fpaacout)
    {
        fclose(g_fpaacout);
    }
}


VO_BOOL vo_http_live_streaming::open( VO_CHAR * ptr_url , VO_BOOL is_sync )
{
	VO_U32       ulThumbnailCount = 0;
    ResetAllIDs();
	VOLOGI( "vo_http_live_streaming::open" );
	close();
	perpare_drm();
	m_is_first_frame = VO_TRUE;

    m_judgementor.load_config( m_szWorkPath );

    if((m_sHLSUserInfo.ulstrUserNameLen >0) || (m_sHLSUserInfo.ulstrPasswdLen >0))
	{
	    VOLOGI("set the userinfo");
	    m_manager.SetParamForHttp(1, &m_sHLSUserInfo);
	}

	VOLOGI("+m_manager.set_m3u_url");
    m_manager.SetRemovePureAudio(VO_FALSE);
    if( !m_manager.set_m3u_url( ptr_url ) )
	{
		VOLOGE( "First set m3u url failed!" );
		//send_eos();
		return VO_FALSE;
	}
	VOLOGI("-m_manager.set_m3u_url");

/* 
	VOLOGI("+m_manager.set_m3u_url");
	if( !m_manager.set_m3u_url( pTest ) )
	{
		VOLOGE( "First set m3u url failed!" );
		//send_eos();
		return VO_FALSE;
	}
	VOLOGI("-m_manager.set_m3u_url");
*/



    VOLOGI("the drm %d", m_iDrmType);

	ulThumbnailCount = m_manager.GetThumbnailItemCount();
	if(m_manager.GetThumbnailItemCount() != 0)
	{
	    DoNotifyForThumbnail();
	}

	if(m_drm_eng_handle != NULL)
    {
        Prepare_HLSDRM();
	}


    VOLOGR( "++++++++++++++++Bitrate" );
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
    VOLOGR( "+++++++++++++++++++++++" );

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

VO_VOID vo_http_live_streaming::close()
{
	VOLOGI("vo_http_live_streaming::close");
	CvoGenaralDrmCallback*   pDrmCallback = NULL;

	stop_livestream( VO_FALSE );
	m_manager.close();
	free_ts_parser();
    free_aac_parser();
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

void vo_http_live_streaming::thread_function()
{
	set_threadname((char*) "Live Streaming" );
	start_livestream();
}

VO_VOID vo_http_live_streaming::start_livestream()
{
	VO_BOOL is_from_pause = VO_FALSE;

	VO_S32 count = 0;

    VO_S32 last_sn = 0;

	VO_S32  iIndex = 0;

	VO_S32  iCharpterCount = m_manager.GetCharpterCount();
		
	while( (!m_bMediaStop) && (m_iSeekResult !=(-1)))  //seek failed, reach the stream end
	{
		media_item item;

        VO_S64 bandwidth_before = m_manager.get_cur_bandwidth();

		VO_S32 ret = m_manager.popup( &item , m_rightbandwidth );

        VO_S64 bandwidth_after = m_manager.get_cur_bandwidth();

		VO_S32 try_time = 0;
		VO_S32 longestwaittime = 120000;
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

        if(strstr(item.path, ".ts") != NULL  || strstr(item.path, ".TS") != 0)
        {
            if(m_iCurrentMediaType != COMMON_MEDIA_TS)
            {
                VOLOGI("change the media type to ts!");
                item.eReloadType = M3U_RELOAD_RESET_TIMESTAMP_TYPE;
                m_iCurrentMediaType = COMMON_MEDIA_TS;
                free_aac_parser();
                free_ts_parser();
                load_ts_parser();

                if(m_manager.is_live())
                {
                    m_timestamp_offset = m_last_video_timestamp > m_last_audio_timestamp ? m_last_video_timestamp + 1 : m_last_audio_timestamp + 1;
                }
                else
                {
                    m_timestamp_offset = m_manager.GetSequenceIDTimeOffset(item.sequence_number)*1000;
                }
            }
        }
        else
        {
            if(strstr(item.path, ".aac") != NULL  || strstr(item.path, ".AAC") != 0)
            {
                if(m_iCurrentMediaType != COMMON_MEDIA_AAC)
                {
                    VOLOGI("change the media type to aac!");
                    m_iCurrentMediaType = COMMON_MEDIA_AAC;
                    free_ts_parser();
                    free_aac_parser();
                    load_aac_parser();
                    if(m_manager.is_live())
                    {
                        m_timestamp_offset = m_last_video_timestamp > m_last_audio_timestamp ? m_last_video_timestamp + 1 : m_last_audio_timestamp + 1;
                    }
                    else
                    {
                        m_timestamp_offset = m_manager.GetSequenceIDTimeOffset(item.sequence_number)*1000;
                    }

                    item.eReloadType = M3U_RELOAD_RESET_TIMESTAMP_TYPE;                
                }

                
                m_iCurrentMediaType = COMMON_MEDIA_AAC;
            }
        }


        if( ( item.sequence_number < last_sn && m_manager.is_live() )/* || strstr( item.path , "fileSequence0.ts" )*/ )
        {
            free_ts_parser();
            load_ts_parser();

            m_timestamp_offset = m_last_video_timestamp > m_last_audio_timestamp ? m_last_video_timestamp + 1 : m_last_audio_timestamp + 1;
        }

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
			        if(iCharpterCount == 1)
			        {
			            VOLOGI("the current  CharpterID:%d, start sequence:%d, new timeoffset:%d", item.iCharpterId, item.sequence_number, m_timestamp_offset);
				        item.eReloadType = M3U_RELOAD_RESET_CONTEXT_ONLY_TYPE;
			        }
				    else
				    {
				        if(item.sequence_number >= m_iLastSequenceID)
				        {
					        item.eReloadType = M3U_RELOAD_RESET_CONTEXT_ONLY_TYPE;	
				        }
					    else
					    {
					        m_timestamp_offset = m_manager.GetSequenceIDTimeOffset(item.sequence_number)*1000;
					        item.eReloadType = M3U_RELOAD_RESET_TIMESTAMP_TYPE;	
					    }
				    }

                    if(COMMON_MEDIA_AAC == m_iCurrentMediaType)
                    {
                        m_timestamp_offset = m_manager.GetSequenceIDTimeOffset(item.sequence_number)*1000;
                        item.eReloadType = M3U_RELOAD_RESET_TIMESTAMP_TYPE;
                    }
                }
		    }
        }
		else
		{
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

		//change_buffer_time( item.duration );

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
			m_is_afterseek = VO_FALSE;
		}

		if( count % 120 == 0 )
		{
			//item.need_reload = VO_TRUE;
		}

        m_judgementor.add_starttime( m_last_audio_timestamp + 1 );

        VO_U32 start_time = voOS_GetSysTime();
        do 
        {
            m_download_bitrate = GetMediaItem( &item );

            if( voOS_GetSysTime() - start_time > 120000 )
                break;

            if( m_download_bitrate == -2 && !m_manager.is_live()  )
            {
                VOLOGE( "Download problem,start to wait!" );
                for( VO_S32 i = 0 ; i < 100 && !m_bMediaStop ; i++ )
                    voOS_Sleep( 20 );
            }

        } while ( m_download_bitrate == -2 && !m_bMediaStop && !m_manager.is_live() );

        if( m_download_bitrate < 0 && !m_manager.is_live() )
        {
            m_brokencount++;
        }
        else
            m_brokencount = 0;

        if( m_brokencount > 10 )
        {
            VOLOGE( "VO_LIVESRC_STATUS_NETWORKBROKEN" );
            Event event;
            event.id = VO_LIVESRC_STATUS_NETWORKBROKEN;
            event.param1 = 0;
            m_eventcallback_func( m_ptr_eventcallbackobj , &event );
            voOS_Sleep( 100 );
            break;
        }

        
        
        if( m_download_bitrate == -2 && !m_manager.is_live() && !m_bMediaStop )
        {
            VOLOGE( "VO_LIVESRC_STATUS_NETWORKBROKEN" );
            Event event;
            event.id = VO_LIVESRC_STATUS_NETWORKBROKEN;
            event.param1 = 0;
            m_eventcallback_func( m_ptr_eventcallbackobj , &event );
            voOS_Sleep( 100 );
            break;
        }

        if( m_download_bitrate == -2 )
            m_download_bitrate = -1;

		VOLOGI( "Last bitrate: %lld; and the delycount: %d" , m_download_bitrate, m_iVideoDelayCount);

        m_judgementor.add_endtime_bitrate( m_last_audio_timestamp , m_manager.get_cur_bandwidth() );

		while( m_is_pause )
		{
			voOS_Sleep( 20 );
			is_from_pause = VO_TRUE;

			if( m_bMediaStop )
				return;
		}

		m_judgementor.add_item( m_download_bitrate );

		//download_bitrate = m_judgementor.get_judgment( m_manager.get_cur_bandwidth() , m_is_video_delayhappen );
        //m_judgementor.get_judgment( m_manager.get_cur_bandwidth() , &m_download_bitrate , &m_bitrate_limit );
        m_judgementor.get_right_bandwidth( m_manager.get_cur_bandwidth() , &m_rightbandwidth );

        //add for the Bitrate Change Event
        NotifyTheBandWidth();

		m_is_video_delayhappen = VO_FALSE;

		if( item.ptr_key )
			item.ptr_key->release();

		if( item.ptr_oldkey )
			item.ptr_oldkey->release();

		count++;
	}

	if( !m_is_flush )
	{
		VOLOGI("Ended!");

		send_eos();

		VOLOGI( "Finished!" );
	}
	
	m_iSeekResult = 0;

	VOLOGI( "End of start_livestream" );

}

VO_VOID vo_http_live_streaming::stop_livestream( VO_BOOL isflush )
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

VO_VOID vo_http_live_streaming::load_ts_parser()
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

	m_tsparser_api.Open( &m_tsparser_handle , &info );
}


VO_VOID vo_http_live_streaming::NotifyMediaPlayType(VO_U32   ulMediaPlayType)
{
    Event event;
    event.id = VO_LIVESRC_STATUS_MEDIATYPE_CHANGE;
    
    switch(ulMediaPlayType)
    {
        case VO_MEDIA_PURE_AUDIO:
        {
            event.param1 = (VO_U32)&ulMediaPlayType;
            m_eventcallback_func(m_ptr_eventcallbackobj , &event );
            VOLOGI("notify the pure audio!");
            break;
        }

        case VO_MEDIA_PURE_VIDEO:
        {
            break;
        }

        case VO_MEDIA_AUDIO_VIDEO:
        {
            break;
        }     
    }
}

VO_VOID vo_http_live_streaming::NotifyTheBandWidth()
{
    VO_S64      illBandwidth = m_manager.get_cur_bandwidth();
    if(illBandwidth != 0x7fffffffffffffffll)
    {
        VOLOGI( "VO_LIVESRC_STATUS_HSL_BITRATE" );
        Event event;
        event.id = VO_LIVESRC_STATUS_HSL_CHANGE_BITRATE;
        event.param1 = (VO_S32)illBandwidth;
        m_eventcallback_func( m_ptr_eventcallbackobj , &event );
    }
}

VO_VOID vo_http_live_streaming::DumpAACPureData(VO_BYTE*   pData, VO_U32 ulLen, VO_U32 ulFlag)
{

    VO_CHAR    strPath[256] = {0};
    if(ulFlag != 0)
    {
        if(g_fpaacin != NULL)
        {
            fclose(g_fpaacin);
        }

        sprintf(strPath, "/sdcard/DumpPureAAC_%d.aac", m_iLastSequenceID);
        g_fpaacin = fopen(strPath, "wb+");
    }

    if(pData != NULL)
    {
        fwrite(pData, 1, ulLen, g_fpaacin);
    }

}


VO_VOID vo_http_live_streaming::DoTransactionForID3(VO_PTR pData)
{
    ID3Frame*   pID3Frame = NULL;
    VO_CHAR     strPath[256] = {0};
    FILE*       pFile = NULL;
	VO_LIVESRC_SAMPLE varSample = {0};
        
    if(pData == NULL)
    {
        return;
    }

    pID3Frame = (ID3Frame*)pData;
    switch(pID3Frame->nSubHead)
    {
        case SUBHEAD_HLS_TIMESTAMP:
        {
            /*
                    memset(strPath, 0, 256);
                    sprintf(strPath, "/sdcard/DumpID3_%d.dat", m_iLastSequenceID);
                    pFile = fopen(strPath, "wb+");
                    if(pFile != NULL)
                    {
                        fwrite(pID3Frame->pFrameData, 1, pID3Frame->nDataLength , pFile);
                        fclose(pFile);
                    }
                    */
            
            break;
        }
        case SUBHEAD_CUSTOMER_PIC_JPEG:
        case SUBHEAD_CUSTOMER_PIC_PNG: 
        {                    
            varSample.Sample.Buffer = pID3Frame->pFrameData;
            varSample.nTrackID = VO_LIVESRC_OUTPUT_AUDIO;
            varSample.nCodecType = VOMP_AUDIO_CodingAAC;
            varSample.Sample.Flag = VOMP_FLAG_VIDEO_EFFECT_ON;
            varSample.Sample.Size = pID3Frame->nDataLength;
 
            VOLOGI("get the id3!, the pic length:%d, the flag of sample:%d", varSample.Sample.Size, varSample.Sample.Flag);
            if( m_datacallback_func )
            {
                //VOLOGI( "+m_datacallback_func" );
                m_datacallback_func( m_ptr_callbackobj , &varSample );
                //VOLOGI( "-m_datacallback_func" );
            }

            break;
        }
        default:
        {
            break;
        }
    }
}


VO_VOID vo_http_live_streaming::load_aac_parser()
{
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
        VOLOGE ("can't get the API!");
        return;
    }

	pAPI (&m_aacparser_api);


    VO_SOURCE2_INITPARAM        varInitParam;

    VO_SOURCE2_SAMPLECALLBACK   varCallback;
    
    
	varCallback.pUserData = this;
	varCallback.SendData = ParserProcAAC;
    varInitParam.uFlag = VO_PID_SOURCE2_SAMPLECALLBACK;
    varInitParam.pInitParam = &varCallback;


	if(m_aacparser_api.Init(&m_aacparser_handle, NULL, VO_SOURCE2_FLAG_OPEN_PUSHMODE, &varInitParam) == 0)
    {
        VOLOGI("load aac ok!");
    }   
}

VO_VOID vo_http_live_streaming::free_aac_parser()
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




VO_VOID vo_http_live_streaming::free_ts_parser()
{
    if(m_tsparser_handle != 0)
    {
	    if( m_tsparser_api.Close )
	    {
		    m_tsparser_api.Close( m_tsparser_handle );
		    m_dlEngine.FreeLib ();
		    m_tsparser_handle = 0;
	    }
    }
}

VO_S64 vo_http_live_streaming::GetMediaItem( media_item * ptr_item )
{
	vo_webdownload_stream http_downloader;
	vo_webdownload_stream http_olddownloader;
	VO_BOOL is_useold = VO_FALSE;

	VO_S64 retspeed;
	VO_BOOL ret = VO_TRUE;

    VOLOGI("");


	if( (ptr_item->eReloadType != M3U_RELOAD_NULL_TYPE) && ( strlen( ptr_item->oldpath ) != 0 ) )
	{
		m_is_bitrate_adaptation = VO_TRUE;
		is_useold = VO_TRUE;

		if( ptr_item->is_oldencrypt )
		{
            switch(m_iDrmType)
            {
                case 0:
			    {
					VOLOGI( "old key url: %s" , ptr_item->ptr_oldkey->ptr_buffer );
					
					VOLOGI( "old iv: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X" , ptr_item->oldiv[0] , ptr_item->oldiv[1] , ptr_item->oldiv[2] , ptr_item->oldiv[3] , ptr_item->oldiv[4] , ptr_item->oldiv[5] ,
						ptr_item->oldiv[6] , ptr_item->oldiv[7] , ptr_item->oldiv[8] , ptr_item->oldiv[9] , ptr_item->oldiv[10] , ptr_item->oldiv[11] , ptr_item->oldiv[12] , ptr_item->oldiv[13] , ptr_item->oldiv[14] , ptr_item->oldiv[15] );
					
					if( strcmp( m_last_keyurl , ( VO_CHAR * )ptr_item->ptr_oldkey->ptr_buffer ) == 0 )
						ret = http_olddownloader.open( ptr_item->oldpath , DOWNLOAD2MEM , NULL , ptr_item->oldiv , ptr_item->olddrm_type , &m_drm_eng , m_drm_eng_handle );
					else
					{
						memset( m_last_keyurl , 0 , 1024 );
						strcpy( m_last_keyurl , ( VO_CHAR * )ptr_item->ptr_oldkey->ptr_buffer );
						ret = http_olddownloader.open( ptr_item->oldpath , DOWNLOAD2MEM , ptr_item->ptr_oldkey->ptr_buffer , ptr_item->oldiv , ptr_item->olddrm_type , &m_drm_eng , m_drm_eng_handle );
					}
					
					break;
				}
            }
		}
		else
			ret = http_olddownloader.open( ptr_item->oldpath , DOWNLOAD2MEM );

		if( !ret )
		{
			VO_S32 errorcode = http_olddownloader.get_lasterror();

			if( errorcode != 0 && errorcode < 80 )
			{
				Event event;
				event.id = VO_LIVESRC_STATUS_DRM_ERROR;
				event.param1 = errorcode;
				memset( m_last_keyurl , 0 , sizeof(m_last_keyurl) );
				VOLOGE( "DRM Engine Error! %d" , errorcode );
                m_eventcallback_func( m_ptr_eventcallbackobj , &event );

                m_is_bitrate_adaptation = VO_FALSE;
                return -1;
			}
            else
            {
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


		VOLOGI( "*****************Get old stream Done*****************" );
		
	}

	if( ptr_item->is_encrypt )
	{
        switch(m_iDrmType)
        {
            case 0:
			{
				VOLOGI( "key url: %s" , ptr_item->ptr_key->ptr_buffer );
				
				VOLOGI( "iv: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X" , ptr_item->iv[0] , ptr_item->iv[1] , ptr_item->iv[2] , ptr_item->iv[3] , ptr_item->iv[4] , ptr_item->iv[5] ,
					ptr_item->iv[6] , ptr_item->iv[7] , ptr_item->iv[8] , ptr_item->iv[9] , ptr_item->iv[10] , ptr_item->iv[11] , ptr_item->iv[12] , ptr_item->iv[13] , ptr_item->iv[14] , ptr_item->iv[15] );
				
				if( strcmp( m_last_keyurl , ( VO_CHAR * )ptr_item->ptr_key->ptr_buffer ) == 0 )
					ret = http_downloader.persist_open( ptr_item->path , DOWNLOAD2MEM , &m_persist	, NULL , ptr_item->iv , ptr_item->drm_type , &m_drm_eng , m_drm_eng_handle );
				else
				{
					memset( m_last_keyurl , 0 , 1024 );
					strcpy( m_last_keyurl , ( VO_CHAR * )ptr_item->ptr_key->ptr_buffer );
					VOLOGI( "ptr_item->path: %s" , ptr_item->path );
					ret = http_downloader.persist_open( ptr_item->path , DOWNLOAD2MEM , &m_persist , ptr_item->ptr_key->ptr_buffer , ptr_item->iv , ptr_item->drm_type , &m_drm_eng , m_drm_eng_handle );
				}
				
				break;
			}
        }
	}
	else
		ret = http_downloader.persist_open( ptr_item->path , DOWNLOAD2MEM , &m_persist );

	if( !ret )
	{
		VO_S32 errorcode = http_downloader.get_lasterror();

		if( errorcode != 0 && errorcode < 80 )
		{
			Event event;
			event.id = VO_LIVESRC_STATUS_DRM_ERROR;
			event.param1 = errorcode;
			memset( m_last_keyurl , 0 , sizeof(m_last_keyurl) );
			VOLOGE( "DRM Engine Error! %d" , errorcode );
            m_eventcallback_func( m_ptr_eventcallbackobj , &event );

            m_is_bitrate_adaptation = VO_FALSE;
            return -1;
		}
        else
        {
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

	if( m_is_bitrate_adaptation )
		m_adaptationbuffer.set_after_adaptation();

	m_is_bitrate_adaptation = VO_FALSE;
	m_bNeedUpdateTimeStamp = VO_FALSE;

	return retspeed;
}

VO_S64 vo_http_live_streaming::GetItem( vo_webdownload_stream * ptr_stream , VO_S32 eReloadType , VO_BOOL is_quick_fetch )
{
	VO_PBYTE ptr_buffer = NULL;
	VO_S32 readed_size = 0;

    
    if( m_pDrmCallback != NULL)
    {
        Prepare_HLSDRM_Process();
    }

    if(m_iCurrentMediaType == COMMON_MEDIA_AAC)
    {
        //DumpAACPureData(0, 0, 1);
    }
    
	m_audiocounter = m_videocounter = 0;
    m_iProcessSize = 0;

	if( (eReloadType != M3U_RELOAD_NULL_TYPE)|| m_iProcessSize == 0 )
	{
		VOLOGI("get size");
		m_iProcessSize = readbuffer_determinesize( &ptr_buffer , ptr_stream );
		VOLOGI( "size: %d" , m_iProcessSize );

		if( m_iProcessSize == -1 || m_iProcessSize == 0 )
		{
			VOLOGE( "Data Fatal Error!" );
			m_iProcessSize = 0;
			ptr_stream->close();

            if(m_pDrmCallback != NULL)
            {
                After_HLSDRM_Process();
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

	if(eReloadType != M3U_RELOAD_NULL_TYPE)
	{
		//VOLOGI("reload_ts_parser");
		//reload_ts_parser();
	}

    /*if( g_fp2 && readed_size > 0 )
        fwrite( ptr_buffer , 1 , readed_size , g_fp2 );*/

	do
	{
	    if(m_iCurrentMediaType == COMMON_MEDIA_TS)
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
    					m_eventcallback_func( m_ptr_eventcallbackobj , &event );
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
        }   
        else
        {
            if(m_iCurrentMediaType == COMMON_MEDIA_AAC)
            {            
    		    if( readed_size )
                {      
                    VO_SOURCE2_SAMPLE  varSample = {0};
                    
    			    if( m_bMediaStop )
    			    {	
    			        break;
                    }


    			    varSample.uSize = readed_size;
    			    varSample.pBuffer = ptr_buffer;

                    //DumpAACPureData(ptr_buffer, readed_size, 0);
                    //VOLOGI("dump pure aac in, buffer size:%d", readed_size);
                    //if(g_fpaacin)
                    //{
                    //    fwrite(ptr_buffer, 1, readed_size, g_fpaacin);
                    //    fflush(g_fpaacin);
                    //}
                    //VOLOGI("after dump pure aac in, buffer size:%d", readed_size);
                    
    			    m_aacparser_api.SendBuffer( m_aacparser_handle , varSample);
                }
                
                readed_size = read_buffer( ptr_stream , ptr_buffer , m_iProcessSize );
            }
        }

        /*if( g_fp2 )
            fwrite( ptr_buffer , 1 , readed_size , g_fp2 );*/
		//VOLOGI("-read_buffer %d" , readed_size);

	}while( !m_bMediaStop && readed_size );

    /*if( g_fp2 )
        fclose( g_fp2 );

    if( m_audiocounter == 0 && m_videocounter == 0 )
    {
        vo_webdownload_stream * ptr_crash = 0;
        ptr_crash->close();
    }*/

	delete []ptr_buffer;

	//VOLOGI("+http_downloader.close");
	ptr_stream->close();
	//VOLOGI("-http_downloader.close");
	
	if( m_pDrmCallback != NULL)
	{
	    After_HLSDRM_Process();
	}

	return ptr_stream->get_download_bitrate();
}

VO_S32 vo_http_live_streaming::read_buffer( vo_webdownload_stream * ptr_stream , VO_PBYTE buffer , VO_S32 size )
{
	VO_S64 readed = 0;
	VO_PBYTE ptr_orgbuffer = buffer;

	while( readed < size && !m_bMediaStop )
	{
		VO_S64 readsize = ptr_stream->read( buffer , size - readed );

		if( readsize == -1 )
		{
		    if(m_pDrmCallback != NULL)
            {
                Do_HLSDRM_Process(ptr_orgbuffer, readed);	
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


    if( m_pDrmCallback != NULL)
    {
		Do_HLSDRM_Process(ptr_orgbuffer, readed);
    }

	return (VO_S32)readed;
}

VO_S32 vo_http_live_streaming::readbuffer_determinesize( VO_PBYTE * ppBuffer , vo_webdownload_stream * ptr_stream )
{
	VO_BYTE buffer[1024];
	VO_S32 buffersize;
	VO_S32 iReadSize = 188*4;

	VO_S32 readsize = read_buffer( ptr_stream , buffer , iReadSize );

    if(m_iCurrentMediaType == COMMON_MEDIA_TS)
    {
        if( readsize < (iReadSize) )
        {
	        VO_PARSER_INPUT_BUFFER input;
	        input.nBufLen = readsize;
	        input.nStreamID = 0;
	        input.pBuf = buffer;
	        m_tsparser_api.Process( m_tsparser_handle , &input );
	        return -1;
        }


        CCheckTsPacketSize checker;
        VO_S32 tspackersize = checker.Check( buffer , readsize );

        if( tspackersize == 0 )
	    {
	        return 0;
        }
    
        buffersize = ( readsize / tspackersize + 50 ) * tspackersize;


        *ppBuffer = new VO_BYTE[buffersize];

	    VO_PBYTE ptr = *ppBuffer;

	    memcpy( ptr , buffer , readsize );

	    ptr = ptr + readsize;

	    read_buffer( ptr_stream , ptr , buffersize - readsize );
    }
    else
    {
        if(m_iCurrentMediaType == COMMON_MEDIA_AAC)
        {
        
            if( readsize < (iReadSize) )
            {
	            VO_SOURCE2_SAMPLE  varInput = {0};
	            varInput.uSize= readsize;
	            varInput.pBuffer = buffer;
	            m_aacparser_api.SendBuffer( m_aacparser_handle , varInput);
	            return -1;
            }
        }

        buffersize = 1024;
        
        *ppBuffer = new VO_BYTE[buffersize];

	    VO_PBYTE ptr = *ppBuffer;

	    memcpy( ptr , buffer , readsize );

	    ptr = ptr + readsize;

	    read_buffer( ptr_stream , ptr , buffersize - readsize );
    }

	return buffersize;
}

void VO_API vo_http_live_streaming::ParserProc(VO_PARSER_OUTPUT_BUFFER* pData)
{
	vo_http_live_streaming * ptr_player = (vo_http_live_streaming *)pData->pUserData;

	switch ( pData->nType )
	{
	case VO_PARSER_OT_AUDIO:
		{
			if( !ptr_player->m_is_mediatypedetermine )
			{
				Event event;
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

				ptr_player->m_eventcallback_func( ptr_player->m_ptr_eventcallbackobj , &event );
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
				Event event;
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

				ptr_player->m_eventcallback_func( ptr_player->m_ptr_eventcallbackobj , &event );
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

                if( ptr_player->m_ptr_videoinfo && ptr_player->m_ptr_videoinfo->HeadSize < ptr_info->nVideoExtraSize )
                {
                    VO_PBYTE ptr = (VO_PBYTE)ptr_player->m_ptr_videoinfo;
                    delete []ptr;
                    ptr_player->m_ptr_videoinfo = 0;
                }

                if( !ptr_player->m_ptr_videoinfo )
                {
                    ptr_player->m_ptr_videoinfo = ( VO_LIVESRC_TRACK_INFOEX * )new VO_BYTE[ sizeof( VO_LIVESRC_TRACK_INFOEX ) + ptr_info->nVideoExtraSize ];
                }

                memset( ptr_player->m_ptr_videoinfo , 0 , sizeof( VO_LIVESRC_TRACK_INFOEX ) );

                ptr_player->m_ptr_videoinfo->Type = VO_SOURCE_TT_VIDEO;
                ptr_player->m_ptr_videoinfo->Codec = VO_VIDEO_CodingH264;
                ptr_player->m_ptr_videoinfo->video_info.Height = ptr_info->VideoFormat.height;
                ptr_player->m_ptr_videoinfo->video_info.Width = ptr_info->VideoFormat.width;
                ptr_player->m_ptr_videoinfo->HeadData = ptr_player->m_ptr_videoinfo->Padding;
                ptr_player->m_ptr_videoinfo->HeadSize = ptr_info->nVideoExtraSize;
			}

			if( ptr_info->nAudioExtraSize )
			{
				if( ptr_player->m_mediatype == -1 )
					ptr_player->m_mediatype = VO_MEDIA_PURE_AUDIO;
				else if( ptr_player->m_mediatype == VO_MEDIA_PURE_VIDEO )
					ptr_player->m_mediatype = VO_MEDIA_AUDIO_VIDEO;

                if( ptr_player->m_ptr_audioinfo && ptr_player->m_ptr_audioinfo->HeadSize < ptr_info->nAudioExtraSize )
                {
                    VO_PBYTE ptr = (VO_PBYTE)ptr_player->m_ptr_audioinfo;
                    delete []ptr;
                    ptr_player->m_ptr_audioinfo = 0;
                }

                if( !ptr_player->m_ptr_audioinfo )
                {
                    ptr_player->m_ptr_audioinfo = ( VO_LIVESRC_TRACK_INFOEX * )new VO_BYTE[ sizeof( VO_LIVESRC_TRACK_INFOEX ) + ptr_info->nAudioExtraSize ];
                }

                memset( ptr_player->m_ptr_audioinfo , 0 , sizeof( VO_LIVESRC_TRACK_INFOEX ) );

                ptr_player->m_ptr_audioinfo->Type = VO_SOURCE_TT_AUDIO;
                ptr_player->m_ptr_audioinfo->Codec = VO_AUDIO_CodingAAC;
                ptr_player->m_ptr_audioinfo->audio_info.Channels = ptr_info->AudioFormat.channels;
                ptr_player->m_ptr_audioinfo->audio_info.SampleBits = ptr_info->AudioFormat.sample_bits;
                ptr_player->m_ptr_audioinfo->audio_info.SampleRate = ptr_info->AudioFormat.sample_rate;
                ptr_player->m_ptr_audioinfo->HeadData = ptr_player->m_ptr_audioinfo->Padding;
                ptr_player->m_ptr_audioinfo->HeadSize = ptr_info->nAudioExtraSize;
			}
		}
	default:
		break;
	}

}


VO_S32 VO_API vo_http_live_streaming::ParserProcAAC(VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData)
{
	vo_http_live_streaming * ptr_player = (vo_http_live_streaming *)pUserData;
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
            }         
            varbuffer.pData = pSample->pBuffer;
            varbuffer.nSize = pSample->uSize;
            varbuffer.nStartTime = pSample->uTime;
            varbuffer.nFrameType = 0;
            varbuffer.nCodecType = VO_AUDIO_CodingAAC;
            ptr_player->audio_data_arrive( &varbuffer );
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


VO_VOID vo_http_live_streaming::bufferframe_callback( VO_BOOL is_video , VO_MTV_FRAME_BUFFER * ptr_buffer , VO_PTR ptr_obj )
{
	vo_http_live_streaming * ptr_this = (vo_http_live_streaming*)ptr_obj;
	if( is_video )
	{
		ptr_this->video_data_arrive( ptr_buffer );
	}
	else
	{
		ptr_this->audio_data_arrive( ptr_buffer );
	}
}

VO_VOID vo_http_live_streaming::audio_data_arrive( VO_MTV_FRAME_BUFFER * ptr_buffer )
{
	/*if( g_fp )
	{
		fwrite( ptr_buffer->pData , ptr_buffer->nSize , 1 , g_fp );
		fflush( g_fp );
	}*/

	VOLOGI( "@@@TimeStamp: %lld Audio" , ptr_buffer->nStartTime );

	/*if( m_manager.is_loop() )
	{
		if( m_last_big_timestamp < ptr_buffer->nStartTime )
			m_last_big_timestamp = ptr_buffer->nStartTime;
		else if( abs( m_last_big_timestamp - ptr_buffer->nStartTime ) > 5000 )
		{
			m_timestamp_offset += m_last_big_timestamp;
			m_last_big_timestamp = 0;
		}

		ptr_buffer->nStartTime = m_timestamp_offset + ptr_buffer->nStartTime;
	}*/

    //Should  add the m_timestamp_offset
	if( (ptr_buffer->nStartTime + (VO_U64)m_timestamp_offset) < (VO_U64)m_seekpos )
	{	
	    return;
	}

	if( m_new_audio_file )
	{
		m_new_audio_file = VO_FALSE;
#ifdef VOME
        send_audio_trackinfo();
        send_media_data( ptr_buffer , 0 , VO_FALSE );
#else
        send_media_data( ptr_buffer , 0 , VO_TRUE );
#endif
	}
	else
		send_media_data( ptr_buffer , 0 );

    if(m_iCurrentMediaType == COMMON_MEDIA_AAC)
    {
        VOLOGI("dump pure aac out, buffer size:%d, the timestamp:%lld", ptr_buffer->nSize, m_timestamp_offset+ptr_buffer->nStartTime);
        if(g_fpaacout)
        {
            fwrite(ptr_buffer->pData, 1, ptr_buffer->nSize, g_fpaacout);
        }
    }
    
    m_last_audio_timestamp = ptr_buffer->nStartTime + m_timestamp_offset;

    VOLOGE( "@@@TimeStamp: %llu Audio Size: %u" , ptr_buffer->nStartTime , ptr_buffer->nSize );
}

VO_VOID vo_http_live_streaming::video_data_arrive( VO_MTV_FRAME_BUFFER * ptr_buffer )
{
	if( (ptr_buffer->nStartTime + (VO_U64)m_timestamp_offset) < (VO_U64)m_seekpos )
	{	
	    return;
	}

	if(  m_is_first_frame && ptr_buffer->nFrameType != 0 )
		{
			m_is_first_frame = VO_FALSE;
			VOLOGI( "No Key Frame Arrive!" );
			return;
		}

	m_is_first_frame = VO_FALSE;

  	/*if( g_fp )
  	{
		//fwrite( &ptr_buffer->nSize , 1 , sizeof( VO_U32 ) , g_fp );
  		fwrite( ptr_buffer->pData , 1 , ptr_buffer->nSize , g_fp );
  		fflush( g_fp );
  	}*/

	/*if( m_manager.is_loop() )
	{
		if( m_last_big_timestamp < ptr_buffer->nStartTime )
			m_last_big_timestamp = ptr_buffer->nStartTime;

		ptr_buffer->nStartTime = m_timestamp_offset + ptr_buffer->nStartTime;
	}*/

	//if( m_new_video_file )
	if( ( ptr_buffer->nFrameType == 0xff || m_new_video_file ) && ptr_buffer->nFrameType != 0xFE )
	{
		m_new_video_file = VO_FALSE;
		VOLOGI("file changed!");
#ifdef VOME
        send_video_trackinfo();
        send_media_data( ptr_buffer , 1 , VO_FALSE );
#else
        send_media_data( ptr_buffer , 1 , VO_TRUE );
#endif
	}
	else
		send_media_data( ptr_buffer , 1 );

    m_last_video_timestamp = ptr_buffer->nStartTime + m_timestamp_offset;

    VOLOGI( "@@@TimeStamp: %llu Size: %u Video %s" , ptr_buffer->nStartTime , ptr_buffer->nSize , ptr_buffer->nFrameType == 0 ? "Is Key Frame" : "" );
}

VO_VOID vo_http_live_streaming::send_media_data( VO_MTV_FRAME_BUFFER * ptr_buffer , VO_U32 index , VO_BOOL newfile )
{
	//if( is_live() && m_is_pause )
		//return;

	//if( index == 1 && m_recoverfrompause && ( ptr_buffer->nFrameType == 0 || ptr_buffer->nFrameType == 0xff ) )
		//m_recoverfrompause = VO_FALSE;

#ifdef _HLS_SOURCE_
	VO_LIVESRC_SAMPLE sample;
	memset( &sample , 0 , sizeof( VO_LIVESRC_SAMPLE ) );
	sample.Sample.Buffer = ptr_buffer->pData;
	sample.Sample.Time = ptr_buffer->nStartTime + m_timestamp_offset;
    VOLOGI( "Send Data TimeStamp: %lld" , sample.Sample.Time );
	sample.Sample.Size = ptr_buffer->nSize;
	if( newfile )
	{
#ifdef VOME
		sample.Sample.Flag = OMX_BUFFERFLAG_EXTRADATA | OMX_VO_BUFFERFLAG_NEWSTREAM;
#else
		sample.Sample.Flag = VOMP_FLAG_BUFFER_NEW_FORMAT;
#endif
		if( index == 0 )
		{
			sample.nCodecType = VOMP_AUDIO_CodingAAC;
		}
		else if( index == 1 )
		{
			sample.nCodecType = VOMP_VIDEO_CodingH264;
		}
	}
	else
		sample.Sample.Flag = 0;

 	if( ptr_buffer->nFrameType == 0 || ptr_buffer->nFrameType == 0xff )
 	{
#ifdef VOME
		sample.Sample.Flag = sample.Sample.Flag | OMX_BUFFERFLAG_SYNCFRAME;
#else
 		sample.Sample.Flag = sample.Sample.Flag | VOMP_FLAG_BUFFER_KEYFRAME;
#endif
 	}

	if( index == 0 )
		sample.nTrackID = VO_LIVESRC_OUTPUT_AUDIO;
	else if( index == 1 )
		sample.nTrackID = VO_LIVESRC_OUTPUT_VIDEO;

    VOLOGI("the flag:%d", sample.Sample.Flag);
	if( m_datacallback_func )
	{
		//VOLOGI( "+m_datacallback_func" );
		m_datacallback_func( m_ptr_callbackobj , &sample );
		//VOLOGI( "-m_datacallback_func" );
	}
#else

 	OMX_BUFFERHEADERTYPE bufHead;
 	memset (&bufHead, 0, sizeof (OMX_BUFFERHEADERTYPE));
 	bufHead.nSize = sizeof (OMX_BUFFERHEADERTYPE);
 	bufHead.nAllocLen = ptr_buffer->nSize;
 
 	bufHead.nOutputPortIndex = index; // 0 Audio  1 Video
 	bufHead.nTickCount = 1;

 

 	bufHead.nFilledLen = ptr_buffer->nSize;
 	bufHead.pBuffer = ptr_buffer->pData;
 	bufHead.nTimeStamp = ptr_buffer->nStartTime;
 
 	if( newfile )
 	{
 		bufHead.nFlags = bufHead.nFlags | OMX_BUFFERFLAG_EXTRADATA | OMX_VO_BUFFERFLAG_NEWSTREAM;
 	}
 
  	if( ptr_buffer->nFrameType == 0 )
  	{
  		bufHead.nFlags = bufHead.nFlags | OMX_BUFFERFLAG_SYNCFRAME;
  	}

 
 	while( m_is_pause )
 	{
 		VOLOGI( "in pause" );
 		voOS_Sleep( 20 );
 
 		if( m_bMediaStop )
 			return;
 	}
 
 	if( m_datacallback_func )
 		m_datacallback_func( m_ptr_callbackobj , &bufHead );
#endif
}

VO_VOID vo_http_live_streaming::send_eos()
{
	VOLOGI("vo_http_live_streaming::send_eos");
#ifdef _HLS_SOURCE_
	VO_LIVESRC_SAMPLE sample;
	memset( &sample , 0 , sizeof( VO_LIVESRC_SAMPLE ) );

	VO_BYTE b;
	sample.Sample.Buffer = &b;
	sample.Sample.Size = 1;
#ifdef VOME
	sample.Sample.Flag = OMX_BUFFERFLAG_EOS;
#else
	sample.Sample.Flag = VOMP_FLAG_BUFFER_EOS;
#endif
	sample.nTrackID = VO_LIVESRC_OUTPUT_AUDIO;
    sample.Sample.Time = m_seekpos > m_last_audio_timestamp + 1 ? m_seekpos + 1 : m_last_audio_timestamp + 1;

    if( m_datacallback_func )
        m_datacallback_func( m_ptr_callbackobj , &sample );

	sample.nTrackID = VO_LIVESRC_OUTPUT_VIDEO;
    sample.Sample.Time = m_seekpos > m_last_video_timestamp + 1 ? m_seekpos + 1 : m_last_video_timestamp + 1;

	if( m_datacallback_func )
		m_datacallback_func( m_ptr_callbackobj , &sample );
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


void	vo_http_live_streaming::setWorkPath (const char * pWorkPath)
{
    memset(m_szWorkPath, 0, 256);
    strcpy (m_szWorkPath, pWorkPath);
	m_judgementor.load_config(m_szWorkPath);
}


VO_S32 vo_http_live_streaming::set_pos( VO_S32 pos )
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

VO_VOID vo_http_live_streaming::start_after_seek()
{
    m_is_afterseek = VO_TRUE;
    need_flush();
	begin();
}

VO_VOID vo_http_live_streaming::perpare_drm()
{

}


VO_VOID vo_http_live_streaming::Prepare_HLSDRM()
{

    VOLOGI("the drm type %d", m_iDrmType);
    CvoBaseDrmCallback*   pDrmCallback = new CvoBaseDrmCallback((VO_SOURCEDRM_CALLBACK*)m_drm_eng_handle);
	VO_DRM_TYPE    eDrmType = (VO_DRM_TYPE)m_iDrmType;

    switch(eDrmType)
    {
        case VO_DRMTYPE_Irdeto:
	    {
            S_IrdetoDRM_INFO      varIrdetoDrmInfo;
            varIrdetoDrmInfo.pURL = m_manager.GetM3uURLForIrdeto();
            varIrdetoDrmInfo.pManifest = m_manager.GetManifestForIrdeto();
			
			VOLOGI("+++in vo_http_live_streaming::open IRDETO_PROJECT");
            pDrmCallback->DRMInfo(VO_DRMTYPE_Irdeto, (VO_PBYTE) &varIrdetoDrmInfo);
			VOLOGI("---in vo_http_live_streaming::open IRDETO_PROJECT");
			break;
		}

		case VO_DRMTYPE_AES128:
		{
			VOLOGI("in vo_http_live_streaming::open IRDETO_PROJECT");
		}

		default:
		{
		    break;
	    }
	}    

	m_pDrmCallback = (VO_VOID*)pDrmCallback;
	
}


VO_VOID vo_http_live_streaming::Prepare_HLSDRM_Process()
{

    VOLOGI("the drm type %d", m_iDrmType);
    CvoBaseDrmCallback*    pDrmCallback = (CvoBaseDrmCallback*)m_pDrmCallback;
    S_HLS_DRM_PROCESS_INFO	varDrmProcInfo;
    S_HLS_DRM_ASSIST_INFO	varAssistInfo;
	VO_DRM_TYPE    eDrmType = (VO_DRM_TYPE)m_iDrmType;

    memset(&varDrmProcInfo, 0, sizeof(S_HLS_DRM_PROCESS_INFO));
	memset(&varAssistInfo, 0, sizeof(S_HLS_DRM_ASSIST_INFO));
    switch(eDrmType)
    {
        case VO_DRMTYPE_Irdeto:
        {
            varDrmProcInfo.eDrmProcessInfo = DecryptProcess_BEGIN;
            varDrmProcInfo.eDrmType = VO_DRM2TYPE_Irdeto;
            varAssistInfo.ulSequenceID = m_iLastSequenceID;
            varDrmProcInfo.pInfo = (VO_PBYTE)&varAssistInfo;
			VOLOGI("+++in vo_http_live_streaming::Prepare_HLSDRM_Process");
            pDrmCallback->DRMData(VO_DRMDATATYPE_PACKETDATA, NULL, 0, NULL, 0, (VO_PBYTE)&varDrmProcInfo);
			VOLOGI("---in vo_http_live_streaming::Prepare_HLSDRM_Process");
			break;
        }
        default:
        {
            break;
        }
    }

}


VO_VOID vo_http_live_streaming::Do_HLSDRM_Process(VO_PBYTE pData, VO_U32  ulDataLen)
{

    VOLOGI("the drm type %d", m_iDrmType);
    CvoBaseDrmCallback*    pDrmCallback = (CvoBaseDrmCallback*)m_pDrmCallback;
    S_HLS_DRM_PROCESS_INFO	varDrmProcInfo;
    S_HLS_DRM_ASSIST_INFO	varAssistInfo;
	VO_DRM_TYPE    eDrmType = (VO_DRM_TYPE)m_iDrmType;

    memset(&varDrmProcInfo, 0, sizeof(S_HLS_DRM_PROCESS_INFO));
	memset(&varAssistInfo, 0, sizeof(S_HLS_DRM_ASSIST_INFO));	
    switch(eDrmType)
    {
        case VO_DRMTYPE_Irdeto:
        {
            varDrmProcInfo.eDrmProcessInfo = DecryptProcess_PROCESSING;
            varDrmProcInfo.eDrmType = VO_DRM2TYPE_Irdeto;
            varAssistInfo.ulSequenceID = m_iLastSequenceID;

            varDrmProcInfo.pInfo = (VO_PBYTE)&varAssistInfo;
			VOLOGI("+++in vo_http_live_streaming::DRMData_HLS_Process");			
            pDrmCallback->DRMData(VO_DRMDATATYPE_PACKETDATA, pData, ulDataLen, NULL, 0, (VO_PBYTE)&varDrmProcInfo);
			VOLOGI("---in vo_http_live_streaming::DRMData_HLS_Process");			
			break;
        }
        default:
        {
            break;
        }
    }    

}



VO_VOID vo_http_live_streaming::After_HLSDRM_Process()
{

    VOLOGI("the drm type %d", m_iDrmType);
    CvoBaseDrmCallback*    pDrmCallback = (CvoBaseDrmCallback*)m_pDrmCallback;
    S_HLS_DRM_PROCESS_INFO	varDrmProcInfo;
    S_HLS_DRM_ASSIST_INFO	varAssistInfo;
	VO_DRM_TYPE    eDrmType = (VO_DRM_TYPE)m_iDrmType;

    memset(&varDrmProcInfo, 0, sizeof(S_HLS_DRM_PROCESS_INFO));
	memset(&varAssistInfo, 0, sizeof(S_HLS_DRM_ASSIST_INFO));
    switch(eDrmType)
    {
        case VO_DRMTYPE_Irdeto:
        {
            varDrmProcInfo.eDrmProcessInfo = DecryptProcess_END;
            varDrmProcInfo.eDrmType = VO_DRM2TYPE_Irdeto;
            varAssistInfo.ulSequenceID = m_iLastSequenceID;
            varDrmProcInfo.pInfo = (VO_PBYTE)&varAssistInfo;
			VOLOGI("+++in vo_http_live_streaming::DRMData_HLS_Process");						
            pDrmCallback->DRMData(VO_DRMDATATYPE_PACKETDATA, NULL, 0, NULL, 0, (VO_PBYTE)&varDrmProcInfo);
			VOLOGI("+++in vo_http_live_streaming::DRMData_HLS_Process");			
			break;
        }
        default:
        {
            break;
        }
    } 

}

VO_VOID vo_http_live_streaming::release_drm()
{

}

VO_VOID vo_http_live_streaming::set_DRM( void * ptr_drm )
{
	VOLOGI( "OK, We got DRM Engine %p" , ptr_drm );
	if( !ptr_drm )
	{
		return;
	}

    m_drm_eng_handle = ptr_drm;
}

VO_VOID vo_http_live_streaming::SetUserName(VO_VOID*  pStrUserName)
{
    if(pStrUserName == NULL)
    {
        return;
    }

	memcpy(m_sHLSUserInfo.strUserName, pStrUserName, strlen((VO_CHAR*)pStrUserName));
	m_sHLSUserInfo.ulstrUserNameLen =  strlen((VO_CHAR*)pStrUserName);
	VOLOGI("the username is %s:", (VO_CHAR*)pStrUserName);
}
VO_VOID vo_http_live_streaming::SetUserPassWd(VO_VOID*	pStrPassWD)
{
    if(pStrPassWD== NULL)
    {
        return;
    }

	memcpy(m_sHLSUserInfo.strPasswd, pStrPassWD, strlen((VO_CHAR*)pStrPassWD));	
	m_sHLSUserInfo.ulstrPasswdLen =  strlen((VO_CHAR*)pStrPassWD);
	VOLOGI("the password is %s", (VO_CHAR*)pStrPassWD);	
}


VO_BOOL	vo_http_live_streaming::setDrmType(VO_S32*   pIDrmType)  
{
    VO_S32  iDrmType = 0;
    if(pIDrmType == NULL)
    {
        VOLOGI( "the Drm type pointer is NULL");
		return VO_FALSE;
    }
	
	VOLOGI( "the Drm type is: %d" , *pIDrmType );
	iDrmType = *pIDrmType;
	if((iDrmType<0) || (iDrmType >VO_DRMTYPE_AES128))
	{
	    VOLOGI( "the Drm type is: %d is not support!" , iDrmType );
		return VO_FALSE;
	}

	m_iDrmType = iDrmType;
	return VO_TRUE;
}


VO_VOID vo_http_live_streaming::run()
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

VO_VOID vo_http_live_streaming::pause()
{
	VOLOGI( "vo_http_live_streaming::pause" );
	m_is_pause = VO_TRUE;

	if( is_live() )
	{
		need_flush();
	}
}

VO_VOID vo_http_live_streaming::need_flush()
{
	VOLOGI("vo_http_live_streaming::need_flush");
#ifdef _HLS_SOURCE_
	VO_LIVESRC_SAMPLE sample;
	memset( &sample , 0 , sizeof( VO_LIVESRC_SAMPLE ) );
	sample.Sample.Flag = VOMP_FLAG_BUFFER_FORCE_FLUSH;
	sample.nTrackID = VO_LIVESRC_OUTPUT_AUDIO;

	VOLOGI("+Send Flush Audio Buffer");
	if( m_datacallback_func )
		m_datacallback_func( m_ptr_callbackobj , &sample );
	VOLOGI("-Send Flush Audio Buffer");

	sample.nTrackID = VO_LIVESRC_OUTPUT_VIDEO;

	VOLOGI("+Send Flush Video Buffer");
	if( m_datacallback_func )
		m_datacallback_func( m_ptr_callbackobj , &sample );
	VOLOGI("-Send Flush Video Buffer");
#else
	Event event;
	event.id = EVENT_NEEDFLUSH;
	m_eventcallback_func( m_ptr_eventcallbackobj , &event );
#endif
}

VO_VOID vo_http_live_streaming::set_videodelay( int * videodelaytime )
{
	VOLOGI( "+++++++++++++Video Delay Time: %d;+++++++++++m_last_video_timestamp:%lld" , *videodelaytime, m_last_video_timestamp);
	if( *videodelaytime > 150 )
	{
	    m_iVideoDelayCount++;
		m_judgementor.add_delaytime((VO_S64)videodelaytime, m_last_video_timestamp);
		m_is_video_delayhappen = VO_TRUE;
	}
}

VO_VOID vo_http_live_streaming::set_libop(void*   pLibOp)
{
	VOLOGI( "set_libop");

    m_dlEngine.SetLibOperator((VO_LIB_OPERATOR *) pLibOp);
}


VO_VOID vo_http_live_streaming::Add_AdFilterInfo(void*	 pAdFilterInfo)
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


VO_S32	vo_http_live_streaming::FindFilterIndex(VO_CHAR*  pFilterString)
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

VO_VOID	vo_http_live_streaming::DoNotifyForAD( media_item * ptr_item, VO_S32  iFilterIndex)
{
	Event event;
	
	event.id = VO_LIVESRC_STATUS_HSL_AD_APPLICATION;	
    event.param1 = (VO_U32)(ptr_item->path);
	event.param2 = (VO_U32)m_aFilterForAdvertisement[iFilterIndex].iFilterId;	

	m_eventcallback_func( m_ptr_eventcallbackobj , &event );

	return;
}


VO_VOID	vo_http_live_streaming::DoNotifyForThumbnail()
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
		
		m_eventcallback_func( m_ptr_eventcallbackobj , &event );
	}
}



VO_VOID vo_http_live_streaming::send_audio_trackinfo()
{
    VO_MTV_FRAME_BUFFER buffer;
    memset( &buffer , 0 , sizeof(VO_MTV_FRAME_BUFFER) );
    buffer.nSize = m_ptr_audioinfo->HeadSize > 12 ? sizeof( VO_LIVESRC_TRACK_INFOEX ) + m_ptr_audioinfo->HeadSize - 12 : sizeof( VO_LIVESRC_TRACK_INFOEX );
    buffer.pData = (VO_PBYTE)m_ptr_audioinfo;
    buffer.nStartTime = 0;

    send_media_data( &buffer , 0 , VO_TRUE );
}

VO_VOID vo_http_live_streaming::send_video_trackinfo()
{
    VO_MTV_FRAME_BUFFER buffer;
    memset( &buffer , 0 , sizeof(VO_MTV_FRAME_BUFFER) );
    buffer.nSize = m_ptr_videoinfo->HeadSize > 12 ? sizeof( VO_LIVESRC_TRACK_INFOEX ) + m_ptr_videoinfo->HeadSize - 12 : sizeof( VO_LIVESRC_TRACK_INFOEX );
    buffer.pData = (VO_PBYTE)m_ptr_videoinfo;
    buffer.nStartTime = 0;

    send_media_data( &buffer , 1 , VO_TRUE );
}

void	vo_http_live_streaming::ResetAllFilters()
{
    memset((void *)m_aFilterForAdvertisement, 0, sizeof(S_FilterForAdvertisementLoacal)*8);
	m_iFilterForAdvertisementCount = 0;
}


void	vo_http_live_streaming::ResetAllIDs()
{
    m_iLastCharpterID = 0;
	m_iLastSequenceID = 0;
}


void	vo_http_live_streaming::LoadWorkPathInfo()
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

VO_VOID vo_http_live_streaming::setCpuInfo(VO_VOID* pCpuInfo)
{
    if(pCpuInfo != NULL)
    {
        m_judgementor.setCpuInfo(pCpuInfo);
    }
}


VO_VOID vo_http_live_streaming::setCapInfo(VO_VOID* pCapInfo)
{
    if(pCapInfo != NULL)
    {
        m_judgementor.setCapInfo(pCapInfo);
    }
}
