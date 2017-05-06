#include "vo_m3u_manager.h"
#include "voString.h"
#include "voOSFunc.h"
#include "voLog.h"

//add 
#include "vo_http_live_streaming.h"
//add


#ifndef DEFAULT_MAX_DOWNLOAD_FAIL_COUNT_FOR_MANIFEST
#define DEFAULT_MAX_DOWNLOAD_FAIL_COUNT_FOR_MANIFEST    2
#endif


#ifndef LOG_TAG
#define LOG_TAG "vo_m3u_manager"
#endif


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


vo_m3u_manager::vo_m3u_manager(void)
{
    m_ptr_item_head = NULL;
	m_ptr_item_tail = NULL;
	m_is_new_set_url = VO_FALSE;
	m_stop_thread = VO_FALSE;
	m_sequence_start = 0;
	m_ptr_playlist_list = NULL;
	m_playlist_item_count = 0;
	m_ptr_current_playlist = NULL; 
	m_playlist_duration = 0;
	m_ptr_oldkey = 0;
	m_playlist_count = 0;
	m_max_sequencenumber = 0;
	
	memset( m_url , 0 , sizeof(m_url) );
	m_pBaseFileContentForIrdeto = NULL;
	m_BaseFileContentSizeForIrdeto = 0;
	m_eBasePlayListType = M3U_PLAYLIST;
	memset( m_urlForBaseUrlForIrdeto, 0, sizeof(m_urlForBaseUrlForIrdeto));
    m_ptr_thumbnail_head = NULL;
	m_ptr_thumbnail_tail = NULL;
	m_ithumbnailCount = 0;
    m_illStartBitrate = 0;
    m_illBitrateCap = 0;
    memset(&m_sEventCallback, 0, sizeof(VO_SOURCE2_EVENTCALLBACK));
    m_ulMaxDownloadFailCount = DEFAULT_MAX_DOWNLOAD_FAIL_COUNT_FOR_MANIFEST;
    memset(m_strEXTKEYLineOld, 0, 1024);
    
}

vo_m3u_manager::~vo_m3u_manager(void)
{
	close();
}

VO_BOOL vo_m3u_manager::set_m3u_url( VO_CHAR * url )
{
	if( strlen(m_url) != 0 )
		m_is_new_set_url = VO_TRUE;
	else
		m_is_new_set_url = VO_FALSE;

	memset( m_url , 0 , sizeof(m_url) );
	strcpy( m_url , url );	
	memset( m_urlForBaseUrlForIrdeto , 0 , sizeof(m_urlForBaseUrlForIrdeto) );
	strcpy(m_urlForBaseUrlForIrdeto, url);

	//VOLOGI("+stop_updatethread");
	stop_updatethread();
	//VOLOGI("-stop_updatethread");

	VOLOGI("+parse_m3u");
	if( !parse_m3u() )
    {
        return VO_FALSE;
    }
	VOLOGI("-parse_m3u");

    combine();


	if( /*0 &&*/ m_reader.is_live_stream() )
	{
		begin();
	}

	return VO_TRUE;
}

VO_S32 vo_m3u_manager::popup( media_item * ptr_item , VO_S64 bandwidth )
{
	if( is_live() )
		return popup_live( ptr_item , bandwidth );
	else
		return popup_notlive( ptr_item , bandwidth );
}

VO_VOID vo_m3u_manager::combine()
{
	voCAutoLock lock( &m_list_lock );

	VOLOGI( "Current Sequence Number: %d" , m_sequence_start );

	VO_BOOL new_is_live_stream = VO_FALSE;
	VO_S32 new_max_duration = 0;
	VO_CHAR * new_ptr_relative_directory = NULL;
	INF_item * ptr_new_item = NULL;

    new_is_live_stream = m_reader.is_live_stream();
	new_max_duration = m_reader.get_maxitem_duration();
	new_ptr_relative_directory = m_reader.get_relativedirectory();
	ptr_new_item = m_reader.get_INF_list();

	if( is_live() )
	{
		while( ptr_new_item && ptr_new_item->sequence < m_sequence_start )
		{
			ptr_new_item = ptr_new_item->ptr_next;
		}
	}

	media_item_internal *ptr_item = m_ptr_item_head;

	while( ptr_new_item )
	{
		while( ptr_item && ptr_new_item && ptr_item->sequence_number > ptr_new_item->sequence )
			ptr_new_item = ptr_new_item->ptr_next;
			
		while( ptr_item && ptr_new_item->sequence > ptr_item->sequence_number )
			ptr_item = ptr_item->ptr_next;

		if( ptr_item )
		{
			INFitem2mediaitem( ptr_item , ptr_new_item , new_ptr_relative_directory );
		}
		else
		{
			ptr_item = add_item( ptr_new_item , new_ptr_relative_directory );
		}

		ptr_new_item = ptr_new_item->ptr_next;
	}

	update_playlist_info();
}

media_item_internal * vo_m3u_manager::add_item( INF_item * ptr_item , VO_CHAR * ptr_relative_directory )
{
	media_item_internal * ptr = new media_item_internal;
	memset( ptr , 0 , sizeof(media_item_internal) );
	INFitem2mediaitem( ptr , ptr_item , ptr_relative_directory );

	if( m_ptr_item_head == 0 && m_ptr_item_tail == 0 )
		m_ptr_item_head = m_ptr_item_tail = ptr;
	else
	{
		m_ptr_item_tail->ptr_next = ptr;
		m_ptr_item_tail = ptr;
	}

	return ptr;
}

VO_VOID vo_m3u_manager::INFitem2mediaitem( media_item_internal * ptr_dest , INF_item * ptr_src , VO_CHAR * ptr_relative_directory )
{
    char   varchar = '/';

	ptr_dest->duration = ptr_src->duration;

	memset( ptr_dest->oldpath , 0 , sizeof( ptr_dest->oldpath ) );
	if( m_is_new_set_url )
	{
		strcpy( ptr_dest->oldpath , m_org_nexturl );

		ptr_dest->is_oldencrypt = m_is_oldencrypt;
        ptr_dest->drm_type = ptr_src->drm_type;
		if( m_is_oldencrypt )
		{
		    if(ptr_dest->drm_type == AES128)
            {      
			    ptr_dest->olddrm_type = m_olddrm_type;
			    memcpy( ptr_dest->oldiv , m_oldiv , 16 );
			    ptr_dest->ptr_oldkey = m_ptr_oldkey;
			    m_ptr_oldkey = 0;
            }
            memset(ptr_dest->strOldEXTKEYLine, 0, 1024);
            memcpy(ptr_dest->strOldEXTKEYLine, m_strEXTKEYLineOld, strlen(m_strEXTKEYLineOld));
		}
	}

	if( is_live() )
	{
		ptr_dest->eReloadType = (m_is_new_set_url || ptr_src->discontinuty )? M3U_RELOAD_RESET_CONTEXT_ONLY_TYPE: M3U_RELOAD_NULL_TYPE;
		m_is_new_set_url = VO_FALSE;

		if( ptr_src->discontinuty )
		{
			ptr_dest->eReloadType = M3U_RELOAD_RESET_TIMESTAMP_TYPE;
		}
	}
	else
	{
		if( ptr_src->sequence == m_sequence_start )
		{
			ptr_dest->eReloadType = (m_is_new_set_url)?M3U_RELOAD_RESET_CONTEXT_ONLY_TYPE: M3U_RELOAD_NULL_TYPE;
            memset(ptr_dest->strOldEXTKEYLine, 0, 1024);
            memcpy(ptr_dest->strOldEXTKEYLine, m_strEXTKEYLineOld, strlen(m_strEXTKEYLineOld));
			m_is_new_set_url = VO_FALSE;
		}

		if( ptr_src->discontinuty )
		{
			ptr_dest->eReloadType = M3U_RELOAD_RESET_TIMESTAMP_TYPE;
		}
	}
	ptr_dest->sequence_number = ptr_src->sequence;
	ptr_dest->deadtime = ptr_src->deadtime;


    //Add for Ad
    ptr_dest->eMediaUsageType = ptr_src->eMediaUsageType;
    ptr_dest->pFilterString= ptr_src->pFilterString;	
    //Add for Ad

	//Add for Charpter ID
	ptr_dest->iCharpterId = ptr_src->iCharpterId;

    varchar = ptr_src->path[0];
	memset( ptr_dest->path , 0 , strlen( ptr_dest->path ) );
    if(ptr_dest->eMediaUsageType == M3U_COMMON_AD_TYPE)
    {
	    get_absolute_path( ptr_dest->path , ptr_src->path , (char*)"" );
    }
	else
	{
	    if(varchar == '/')
	    {
	        //use the absolute path
            get_absolute_path(ptr_dest->path , ptr_src->path , m_reader.get_hostdirectory() );
	    }
		else
		{
	        get_absolute_path( ptr_dest->path , ptr_src->path , ptr_relative_directory );	
		}
	}

	ptr_dest->is_encrypt = ptr_src->is_encrypt;

    if( ptr_src->sequence > m_max_sequencenumber )
        m_max_sequencenumber = ptr_src->sequence;

	if( ptr_dest->is_encrypt )
	{
	    ptr_dest->drm_type = ptr_src->drm_type;
	    if(ptr_dest->drm_type == AES128)
        {   
		ptr_dest->ptr_key = ptr_src->ptr_key;

		ptr_dest->ptr_key->add_ref();

		if( memcmp( ptr_dest->ptr_key->ptr_buffer , "http" , 4 ) != 0 )
		{
			unsigned char * ptr_url = new unsigned char[1024];
			memset( ptr_url , 0 , 1024 );

			VOLOGI( "%s" , (VO_CHAR *)ptr_dest->ptr_key->ptr_buffer );
			VOLOGI( "%s" , m_reader.get_relativedirectory() );
			get_absolute_path( ( VO_CHAR * )ptr_url , (VO_CHAR *)ptr_dest->ptr_key->ptr_buffer , m_reader.get_relativedirectory() );

			delete []ptr_dest->ptr_key->ptr_buffer;
			ptr_dest->ptr_key->ptr_buffer = ptr_url;
		}

		memcpy( ptr_dest->iv , ptr_src->iv , 16 );
        }
        //Add for Discretix
        memset(ptr_dest->strEXTKEYLine, 0, 1024);
        memcpy(ptr_dest->strEXTKEYLine, ptr_src->strEXTKEYLine, strlen(ptr_src->strEXTKEYLine));
        //Add for Discretix
	}
}

VO_VOID vo_m3u_manager::update_playlist_info()
{
	m_playlist_item_count = 0;
	m_playlist_duration = 0;

	media_item_internal * ptr_item = m_ptr_item_head;

	if( m_ptr_item_head && is_live() )
		m_sequence_start = m_ptr_item_head->sequence_number;

	while( ptr_item )
	{
		m_playlist_item_count++;
		m_playlist_duration += ptr_item->duration;
		ptr_item = ptr_item->ptr_next;
	}
  
	VOLOGI( "Playlist Refreshed! Item Count: %d" , m_playlist_item_count );
}

void vo_m3u_manager::thread_function()
{
	set_threadname( (char *)"Playlist Update" );
	playlist_update();
	VOLOGI( "Update Thread Exit!" );
}

VO_VOID vo_m3u_manager::playlist_update()
{
	VO_S32 trytime = 0;

	VO_S64 count = 0;
	VO_U32 start_time = 0;
	VO_U32 end_time = 0;
    VO_U32 ulCurrentTryTime = 0;

	start_time = end_time = voOS_GetSysTime();

	while( m_reader.is_live_stream() && !m_stop_thread )
	{
		VO_S32 waittime = (VO_S32)(m_reader.get_update_interval() * 1000.);

		VOLOGI( "Playlist Update Interval: %d , Duration: %d" , waittime , m_playlist_duration );

		start_time = voOS_GetSysTime();

		if( !parse_m3u() )
		{
			trytime++;
            ulCurrentTryTime += waittime/2;
            voOS_Sleep(waittime/2);
			VOLOGI("current retry time:%d", ulCurrentTryTime);
            continue;
		}
		else
        {
            ulCurrentTryTime = 0;
            combine();
			trytime = 0;
        }

		if( m_playlist_item_count == 0 )
		{
			count++;
			if( end_time + waittime * 1.5 <= start_time && end_time + 4000 <= start_time )
				m_sequence_start = 0;

			voOS_Sleep( 20 );
			continue;
		}

		count = 0;

		end_time = voOS_GetSysTime();

		VOLOGI( "Playlist Update cost : %d" , end_time - start_time );

		if( (end_time - start_time) < (VO_U32)waittime )
		{
			waittime = waittime - ( end_time - start_time );

			VOLOGI( "Playlist Update wait: %d" , waittime );
			
			while( waittime > 0 && !m_stop_thread && m_playlist_item_count )
			{
				voOS_Sleep( 50 );
				waittime = waittime - 50;
			}
		}
	}
}

VO_VOID vo_m3u_manager::stop_updatethread()
{
	VOLOGI( "+stop_updatethread" );
	m_stop_thread = VO_TRUE;

	vo_thread::stop();

	m_stop_thread = VO_FALSE;
	VOLOGI( "-stop_updatethread" );
}

VO_VOID vo_m3u_manager::destroy()
{
	stop_updatethread();

	media_item_internal * ptr_item = m_ptr_item_head;

	while( ptr_item )
	{
		media_item_internal * ptr_temp = ptr_item;
		ptr_item = ptr_item->ptr_next;

		if( ptr_temp->ptr_key )
			ptr_temp->ptr_key->release();

		delete ptr_temp;
	}


	m_ptr_item_head = m_ptr_item_tail = 0;
	
	thumbnail_item*      pThumbnail = m_ptr_thumbnail_head;
	thumbnail_item*      pThumbnailTemp = NULL;

	
	while(pThumbnail)
	{
	    pThumbnailTemp = pThumbnail->ptr_next;
		delete  pThumbnail;
		pThumbnail = pThumbnailTemp;
	}

	m_ptr_thumbnail_head = m_ptr_thumbnail_tail = 0;
	
}

VO_VOID vo_m3u_manager::close()
{
	destroy();
	m_is_new_set_url = VO_FALSE;
	m_stop_thread = VO_FALSE;
	clear_playlist_item();

    
	if(m_pBaseFileContentForIrdeto!= NULL)
	{
	    delete    m_pBaseFileContentForIrdeto;
		m_pBaseFileContentForIrdeto = NULL;
	}
    
}

VO_VOID vo_m3u_manager::get_absolute_path( VO_CHAR * str_absolute_path , VO_CHAR * str_filename , VO_CHAR * str_relative_path )
{
	if( strstr( str_filename , "http://" ) == str_filename || strstr( str_filename , "iprm://" ) == str_filename ||  strstr( str_filename , "https://" ) == str_filename )
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

VO_BOOL vo_m3u_manager::parse_m3u()
{
	VO_BOOL ret = VO_FALSE;
	VO_S32 try_count = 0;
	VO_S64 illContentLen = 0;
	playlist_item*   pPlayList = NULL;
    vo_http_live_streaming*   phttplivestream = NULL;
    VO_S64     illBitrateAtStart = 0;
    playlist_item *     pFirstPlayList = NULL;


    VOLOGI("the bitrate cap:%d, the start bitrate cap", (VO_U32)m_illBitrateCap, (VO_U32)m_illStartBitrate);
    
	
	//do 
	//{
		VOLOGI("+ m_reader.parse");
		ret = m_reader.parse( m_url );
        TransactionForManifestDownloadResult(ret);
		VOLOGI("- m_reader.parse");

		M3U_PLAYLIST_TYPE type = m_reader.get_playlist_type();

        if(type == M3U_PLAYLIST  && ret )
        {
        
            if(m_pBaseFileContentForIrdeto != NULL)
            {
                delete  m_pBaseFileContentForIrdeto;
                m_pBaseFileContentForIrdeto = NULL;
            }
            
            m_BaseFileContentSizeForIrdeto =  m_reader.GetManifestContentSize();
            if(m_BaseFileContentSizeForIrdeto <= 0)
            {
                return VO_FALSE;
            }
            

            VOLOGI("the manifest length:%d", (VO_U32)m_BaseFileContentSizeForIrdeto);
            m_pBaseFileContentForIrdeto = new char[(VO_U32)m_BaseFileContentSizeForIrdeto+4];
            if(m_pBaseFileContentForIrdeto != NULL && m_reader.GetManifest())
            {
                memset(m_pBaseFileContentForIrdeto, 0, (VO_U32)m_BaseFileContentSizeForIrdeto+4);
                memcpy(m_pBaseFileContentForIrdeto, m_reader.GetManifest(), (VO_U32)m_BaseFileContentSizeForIrdeto);
                m_pBaseFileContentForIrdeto[m_BaseFileContentSizeForIrdeto+3] = 0;
            }
        }
        

		while( ret && type == M3U_VARIANT_PLAYLIST )
		{
		    VOLOGI("first URL is stream url!");


            m_BaseFileContentSizeForIrdeto =  m_reader.GetManifestContentSize();
            if(m_pBaseFileContentForIrdeto != NULL)
            {
                delete  m_pBaseFileContentForIrdeto;
                m_pBaseFileContentForIrdeto = NULL;
            }    
            
            VOLOGI("the manifest length:%d", (VO_U32)m_BaseFileContentSizeForIrdeto);
            m_pBaseFileContentForIrdeto = new char[(VO_U32)m_BaseFileContentSizeForIrdeto+4];
            if(m_pBaseFileContentForIrdeto != NULL)
            {
                memset(m_pBaseFileContentForIrdeto, 0, (VO_U32)m_BaseFileContentSizeForIrdeto+4);
                memcpy(m_pBaseFileContentForIrdeto, m_reader.GetManifest(), (VO_U32)m_BaseFileContentSizeForIrdeto);
                m_pBaseFileContentForIrdeto[m_BaseFileContentSizeForIrdeto+3] = 0;
            }

			  
            m_playlist_count = 0;
			INF_item * ptr_item = m_reader.get_INF_list();

			clear_playlist_item();

			while( ptr_item )
			{
				if( !m_ptr_current_playlist )
					m_ptr_current_playlist = add_playlist_item( ptr_item );
				else
					add_playlist_item( ptr_item );
				ptr_item = ptr_item->ptr_next;

                m_playlist_count++;
			}

            if(m_illStartBitrate == 0)
            {
                if(m_illBitrateCap != 0)
                {
                    if(m_ptr_current_playlist->band_width > m_illBitrateCap)
                    {
                        pFirstPlayList = m_ptr_playlist_list;
                        while(pFirstPlayList != NULL)
                        {
                            if(pFirstPlayList->ptr_next != NULL)
                            {
                                if(pFirstPlayList->ptr_next->band_width >m_illBitrateCap)
                                {
                                    break;
                                }
                            }

                            pFirstPlayList = pFirstPlayList->ptr_next;
                        }

                        if(pFirstPlayList != NULL)
                        {
                            m_ptr_current_playlist = pFirstPlayList;
                            VOLOGI("the first playlist bitrate :%d", (VO_U32)m_ptr_current_playlist->band_width);
                        }
                    }
                }
            }
            else
            {
                if(m_illBitrateCap != 0)
                {
                    illBitrateAtStart = (m_illStartBitrate<m_illBitrateCap)?m_illStartBitrate:m_illBitrateCap;
                }
                else
                {
                    illBitrateAtStart = m_illStartBitrate;
                }

                
                pFirstPlayList = m_ptr_playlist_list;
                while(pFirstPlayList != NULL)
                {
                    if(pFirstPlayList->ptr_next != NULL)
                    {
                        if(pFirstPlayList->ptr_next->band_width >illBitrateAtStart)
                        {
                            break;
                        }
                    }
                
                    pFirstPlayList = pFirstPlayList->ptr_next;
                }
                
                if(pFirstPlayList != NULL)
                {
                    m_ptr_current_playlist = pFirstPlayList;
                    VOLOGI("the first playlist bitrate :%d", (VO_U32)m_ptr_current_playlist->band_width);
                }
            }

			//playlist_item * ptr_playlist = m_ptr_playlist_list;
			//ptr_playlist = ptr_playlist->ptr_next;
			//ptr_playlist = ptr_playlist->ptr_next;

			//while( ptr_playlist && ptr_playlist->ptr_next )
				//ptr_playlist = ptr_playlist->ptr_next;

			//if( ptr_playlist->ptr_pre )
			//{
				//ptr_playlist = ptr_playlist->ptr_pre;

 				//delete ptr_playlist->ptr_next;
 				//ptr_playlist->ptr_next = NULL;
			//}

			//m_ptr_current_playlist = ptr_playlist;

			pPlayList = GetThumbnailPlayList();
			if(pPlayList != NULL)
			{
			    ret = m_reader.parse(pPlayList->path);				
				GenerateThumbnailInfoList();
				RemoveThumbnailInfoList();
				m_playlist_count--;
			}

            //only for test
            if(m_playlist_count > 1 && m_bRemoveAudio == VO_TRUE)
            {
                RemoveTheAudioPlayList();
            }
			
			strcpy( m_url , m_ptr_current_playlist->path );

			ret = m_reader.parse( m_url );

			if( !ret )
				break;

			type = m_reader.get_playlist_type();
		}

		if( ret )
		{
			//break;
		}
		else
		{
			//VOLOGI( "Playlist Update Parse Fail!" );
		}





		try_count++;
		voOS_Sleep( 100 );
	//} while ( !ret && try_count <= 2 && !m_stop_thread );

	return ret;
}

playlist_item * vo_m3u_manager::add_playlist_item( INF_item *  ptr_item )
{
    char      varchar = '/';
    VO_U32    ulStreamFlag = 0;
	playlist_item * ptr_playlist_item = new playlist_item;
	memset( ptr_playlist_item , 0 , sizeof( playlist_item ) );

    varchar = ptr_item->path[0];
    if(varchar == '/')
    {
        //use the absolute path
        get_absolute_path( ptr_playlist_item->path , ptr_item->path , m_reader.get_hostdirectory() );
    }
	else
	{
	    get_absolute_path( ptr_playlist_item->path , ptr_item->path , m_reader.get_relativedirectory() );
	}
	
	ptr_playlist_item->band_width = ptr_item->bandwidth;

    ulStreamFlag = GetStreamType(ptr_item);

	ptr_playlist_item->uFlag = ulStreamFlag;
    VOLOGI("BAInfo*** the Stream Bitrate:%d, the Stream URL:%s", (VO_U32)(ptr_playlist_item->band_width), ptr_playlist_item->path);

	if( !m_ptr_playlist_list )
	{
		m_ptr_playlist_list = ptr_playlist_item;
	}
	else
	{
		playlist_item * ptr_entry = m_ptr_playlist_list;

		while( ptr_entry )
		{
			if( ptr_playlist_item->band_width < ptr_entry->band_width )
			{
				playlist_item * ptr_pre_entry = ptr_entry->ptr_pre;

				if( ptr_pre_entry )
				{
					ptr_playlist_item->ptr_pre = ptr_pre_entry;
					ptr_playlist_item->ptr_next = ptr_entry;
					ptr_pre_entry->ptr_next = ptr_playlist_item;
					ptr_entry->ptr_pre = ptr_playlist_item;
				}
				else
				{
					m_ptr_playlist_list = ptr_playlist_item;
					ptr_playlist_item->ptr_next = ptr_entry;
					ptr_entry->ptr_pre = ptr_playlist_item;
				}

				break;
			}

			if( !ptr_entry->ptr_next )
			{
				ptr_entry->ptr_next = ptr_playlist_item;
				ptr_playlist_item->ptr_pre = ptr_entry;

				break;
			}

			ptr_entry = ptr_entry->ptr_next;
		}
	}

	m_playlist_item_count++;

	return ptr_playlist_item;
}

VO_VOID vo_m3u_manager::clear_playlist_item()
{
	playlist_item * ptr_item = m_ptr_playlist_list;

	while( ptr_item )
	{
		playlist_item * ptr_temp = ptr_item;
		ptr_item = ptr_item->ptr_next;
		delete ptr_temp;
	}

	m_playlist_item_count = 0;
	m_ptr_playlist_list = 0;
}

VO_S32 vo_m3u_manager::change_bitrate( VO_S64 bandwidth )
{
    VO_U32     ulSequenceNumberForChange = 0xffffffff;
    VO_U32     ulItemCount = 0;
    VO_CHAR    strOldPath[1024] = {0};
    playlist_item*        ptr_itemPlayList = m_ptr_playlist_list;
    media_item_internal*  ptr_itemChuckItem = m_ptr_item_head;

	if( bandwidth == -1 )
    {
        VOLOGI("");
		return 0;
    }

	if( !m_ptr_current_playlist )
    {
        VOLOGI("");
		return 0;
    }

    if( m_playlist_count <= 1 )
    {
        VOLOGI("");
        return 0;
    }

	VOLOGI( "+++++++++++++Next Bitrate: %lld" , bandwidth );

    while( ptr_itemPlayList )
    {
        if( ptr_itemPlayList->band_width == bandwidth )
        {
            break;
        }
        ptr_itemPlayList = ptr_itemPlayList->ptr_next;
    }
    
    if( ptr_itemPlayList && ( ptr_itemPlayList != m_ptr_current_playlist || m_ptr_item_head == NULL ))
    {
		//Change the birate code
		if( m_ptr_item_head )
		{
			ptr_itemChuckItem = m_ptr_item_head;
			if( !is_live() )
			{
				while( ptr_itemChuckItem && ptr_itemChuckItem->sequence_number != m_sequence_start )
					ptr_itemChuckItem = ptr_itemChuckItem->ptr_next;
			}
			memset( m_org_nexturl , 0 , sizeof(m_org_nexturl) );
			if( m_ptr_oldkey )
				m_ptr_oldkey->release();
			m_ptr_oldkey = 0;
			memset( m_oldiv , 0 , 16 );
            memset(m_strEXTKEYLineOld, 0, 1024);

			if( ptr_itemChuckItem )
			{
				ulSequenceNumberForChange = ptr_itemChuckItem->sequence_number;
				strcpy( m_org_nexturl , ptr_itemChuckItem->path );
				m_is_oldencrypt = ptr_itemChuckItem->is_encrypt;
				m_ptr_oldkey = ptr_itemChuckItem->ptr_key;
				if( m_ptr_oldkey )
					m_ptr_oldkey->add_ref();
				memcpy( m_oldiv , ptr_itemChuckItem->iv , 16 );
				m_olddrm_type = ptr_itemChuckItem->drm_type;
                memcpy(m_strEXTKEYLineOld, ptr_itemChuckItem->strEXTKEYLine, strlen(ptr_itemChuckItem->strEXTKEYLine));
			}
		}

		memcpy(strOldPath, m_url, strlen(m_url));

        stop_updatethread();
        strcpy( m_url , ptr_itemPlayList->path );
		if( m_ptr_item_head )
		{	
		    m_reader.set_MEDIA_SEQUENCE( m_ptr_item_head->sequence_number );
        }
		else
        {      
			m_reader.set_MEDIA_SEQUENCE( 0 );
        }

		m_ptr_current_playlist = ptr_itemPlayList;

		if( m_reader.is_live_stream())
		{
			if(!parse_m3u())
			{
				if(m_playlist_item_count == 0)
				{
					VOLOGI( "Parse New playlist item Fail! and m_playlist_item_count=0   !" );
				}
				else
				{
					VOLOGI("Parse New playlist item Fail! but there items left!");
				}
				memset(m_url, 0, 1024);
				memcpy(m_url, strOldPath, strlen(strOldPath));
			}
			else
			{
				if(ulSequenceNumberForChange == 0xffffffff)
				{
					if(ulSequenceNumberForChange>m_reader.GetTheSequenceMax())
					{
						VOLOGI("Parse Content Error!, Change Bitrate Failded!, the ulSequenceNumberForChange:%d, newMin:%d, newMax:%d",
							ulSequenceNumberForChange, m_reader.GetTheSequenceMin(), m_reader.GetTheSequenceMax());
						memset(m_url, 0, 1024);
						memcpy(m_url, strOldPath, strlen(strOldPath));
					}
				}
				else
				{
					m_is_new_set_url = VO_TRUE;            
					destroy();
					combine();
				}
			}

			begin();
		}
		else
		{  
			if( !parse_m3u())
			{
				if(m_playlist_item_count == 0)
				{
					VOLOGI( "Parse New playlist item Fail! and m_playlist_item_count=0   !" );
				}
				else
				{
					VOLOGI("Parse New playlist item Fail! but there items left!");
				}
				memset(m_url, 0, 1024);
				memcpy(m_url, strOldPath, strlen(strOldPath));            
			}
			else
			{
				ulItemCount = m_reader.GetTheSequenceMax()-m_reader.GetTheSequenceMin();
				if(ulItemCount != m_playlist_item_count)
				{
					VOLOGI( "Parse Content Error!, with different chunck items!" );
					memset(m_url, 0, 1024);
					memcpy(m_url, strOldPath, strlen(strOldPath));            
				}
				else
				{
					m_is_new_set_url = VO_TRUE;            
					destroy();
					combine();
				}
			}
		}
    }

    return 0;
}

VO_S32 vo_m3u_manager::get_duration()
{
	if( is_live() )
	{
		VOLOGI( "get duration 0" );
		return 0;
	}
	else
	{
		VOLOGI( "get duration %d" , m_playlist_duration );
		return m_playlist_duration * 1000;
	}
}

VO_S32 vo_m3u_manager::set_pos( VO_S32 pos )
{
	voCAutoLock lock( &m_list_lock );

	media_item_internal * ptr_item_internal = m_ptr_item_head;

	VO_S32 total = 0;

	media_item_internal * ptr_last_item = ptr_item_internal;

	while( ptr_item_internal )
	{
		if( total + ptr_item_internal->duration > pos )
		{
			break;
		}

		total = total + ptr_item_internal->duration;

		ptr_last_item = ptr_item_internal;
		ptr_item_internal = ptr_item_internal->ptr_next;
	}

	if( ptr_item_internal )
	{
		/*if( pos <= total + ptr_item_internal->duration / 10. )
        {
            total = total - ptr_last_item->duration;
            m_sequence_start = ptr_last_item->sequence_number;
        }
        else*/
        {
            m_sequence_start = ptr_item_internal->sequence_number;
        }
	}
	else
		return -1;

    memset( m_org_nexturl , 0 , sizeof(m_org_nexturl) );

	return total;
}

VO_S32 vo_m3u_manager::popup_live( media_item * ptr_item , VO_S64 bandwidth  )
{
	voCAutoLock lock( &m_list_lock );

	if( m_ptr_item_head == NULL )
	{
		VOLOGI("playlist end!");
		return -1;
	}
	else
	{
		m_list_lock.Unlock();
		change_bitrate( bandwidth );
		m_list_lock.Lock();

		while( m_ptr_item_head && m_ptr_item_head->deadtime < voOS_GetSysTime() )
		{
			media_item_internal * ptr_item_internal = m_ptr_item_head;
			m_ptr_item_head = m_ptr_item_head->ptr_next;

			if( ptr_item_internal->ptr_key )
				ptr_item_internal->ptr_key->release();

			m_sequence_start = ptr_item_internal->sequence_number + 1;

            VOLOGI( "One time out! %u  Systime %u" , ptr_item_internal->deadtime , voOS_GetSysTime() );
			
			delete ptr_item_internal;

			if( !m_ptr_item_head )
			{
				VOLOGI("Time out leads queue empty!");
				m_ptr_item_tail = m_ptr_item_head;
				return -1;
			}
		}

		if( m_ptr_item_head )
		{
			memset( ptr_item , 0 , sizeof(media_item) );
			strcpy( ptr_item->path , m_ptr_item_head->path );
			strcpy( ptr_item->oldpath , m_org_nexturl );
			ptr_item->eReloadType= m_ptr_item_head->eReloadType;
			ptr_item->duration = m_ptr_item_head->duration;
            ptr_item->sequence_number = m_ptr_item_head->sequence_number;

			ptr_item->is_encrypt = m_ptr_item_head->is_encrypt;

			//Add For AD
	        ptr_item->eMediaUsageType = m_ptr_item_head->eMediaUsageType;
	        ptr_item->pFilterString = m_ptr_item_head->pFilterString;
            //Add For AD

			if( m_ptr_item_head->is_encrypt )
			{
				memcpy( ptr_item->iv , m_ptr_item_head->iv , 16 );

                //Add for Discretix
                memset(ptr_item->strEXTKEYLine, 0, 1024);
                memcpy(ptr_item->strEXTKEYLine, m_ptr_item_head->strEXTKEYLine, strlen(m_ptr_item_head->strEXTKEYLine));
                //Add for Discretix
				ptr_item->ptr_key = m_ptr_item_head->ptr_key;
				if( m_ptr_item_head->ptr_key )
				{
					ptr_item->ptr_key->add_ref();
				}

				ptr_item->drm_type = m_ptr_item_head->drm_type;
			}

			ptr_item->is_oldencrypt = m_ptr_item_head->is_oldencrypt;
			if( m_ptr_item_head->is_oldencrypt )
			{
				memcpy( ptr_item->oldiv , m_ptr_item_head->oldiv , 16 );
				ptr_item->olddrm_type = m_ptr_item_head->olddrm_type;
				ptr_item->ptr_oldkey = m_ptr_item_head->ptr_oldkey;
				if( m_ptr_item_head->ptr_oldkey )
				{
					ptr_item->ptr_oldkey->add_ref();
				}
                //Add for Discretix
                memset(ptr_item->strOldEXTKEYLine, 0, 1024);
                memcpy(ptr_item->strOldEXTKEYLine, m_ptr_item_head->strOldEXTKEYLine, strlen(m_ptr_item_head->strOldEXTKEYLine));
                //Add for Discretix
			}

			media_item_internal * ptr_item_internal = m_ptr_item_head;
			m_ptr_item_head = m_ptr_item_head->ptr_next;

			if( ptr_item_internal->ptr_key )
				ptr_item_internal->ptr_key->release();

			m_sequence_start = ptr_item_internal->sequence_number + 1;

			delete ptr_item_internal;


			if( m_ptr_item_head == NULL )
			{
				VOLOGI("playlist will end!");
				m_ptr_item_tail = m_ptr_item_head;
			}
		}
		else
		{
			return -1;
		}
	}

	return 1;
}

VO_S32 vo_m3u_manager::popup_notlive( media_item * ptr_item , VO_S64 bandwidth  )
{
	voCAutoLock lock( &m_list_lock );

    if( m_sequence_start > m_max_sequencenumber )
        return -1;

	m_list_lock.Unlock();
	VO_S32 ret = change_bitrate( bandwidth );
	m_list_lock.Lock();

    if( ret == -2 )
        return ret;

    if( m_ptr_item_head == NULL )
    {
        VOLOGI("playlist end!");
        return -2;
    }

	media_item_internal * ptr_item_internal = m_ptr_item_head;

	while(ptr_item_internal)
	{
		if( ptr_item_internal->sequence_number >= m_sequence_start )
			break;

		ptr_item_internal = ptr_item_internal->ptr_next;
	}

	if( !ptr_item_internal )
	{
		if( m_reader.is_loop() )
		{
			ptr_item_internal = m_ptr_item_head;
		}
		else
			return -1;
	}

	memset( ptr_item , 0 , sizeof(media_item) );
	strcpy( ptr_item->path , ptr_item_internal->path );
	strcpy( ptr_item->oldpath , m_org_nexturl );
	ptr_item->eReloadType= ptr_item_internal->eReloadType;
	ptr_item->duration = ptr_item_internal->duration;
    ptr_item->sequence_number = ptr_item_internal->sequence_number;

	ptr_item->is_encrypt = ptr_item_internal->is_encrypt;

    //Add For AD
	ptr_item->eMediaUsageType = ptr_item_internal->eMediaUsageType;
	ptr_item->pFilterString = ptr_item_internal->pFilterString;
    //Add For AD

    //Add For Charpter Id
    ptr_item->iCharpterId = ptr_item_internal->iCharpterId;

	
	if( ptr_item_internal->is_encrypt )
	{
	    if(ptr_item_internal->drm_type == AES128)
        {   
		memcpy( ptr_item->iv , ptr_item_internal->iv , 16 );
		
		ptr_item->ptr_key = ptr_item_internal->ptr_key;
		if( ptr_item_internal->ptr_key )
		{
			ptr_item->ptr_key->add_ref();
		}
        }

		ptr_item->drm_type = ptr_item_internal->drm_type;
        //Add for Discretix
        memset(ptr_item->strEXTKEYLine, 0, 1024);
        memcpy(ptr_item->strEXTKEYLine, ptr_item_internal->strEXTKEYLine, strlen(ptr_item_internal->strEXTKEYLine));
        //Add for Discretix
	}

	ptr_item->is_oldencrypt = m_ptr_item_head->is_oldencrypt;
	if( m_ptr_item_head->is_oldencrypt )
	{
	    if(ptr_item_internal->drm_type == AES128)
        {   
		memcpy( ptr_item->oldiv , m_ptr_item_head->oldiv , 16 );
		ptr_item->olddrm_type = m_ptr_item_head->olddrm_type;
		ptr_item->ptr_oldkey = m_ptr_item_head->ptr_oldkey;
		if( m_ptr_item_head->ptr_oldkey )
		{
			ptr_item->ptr_oldkey->add_ref();
		}
        }
        //Add for Discretix
        memset(ptr_item->strOldEXTKEYLine, 0, 1024);
        memcpy(ptr_item->strOldEXTKEYLine, ptr_item_internal->strOldEXTKEYLine, strlen(ptr_item_internal->strOldEXTKEYLine));
        //Add for Discretix
	}

	m_sequence_start = ptr_item_internal->sequence_number + 1;

	return 1;
}

VO_S64 vo_m3u_manager::get_cur_bandwidth()
{
	voCAutoLock lock( &m_list_lock );
	if( m_ptr_current_playlist )
		return m_ptr_current_playlist->band_width;
	else
		return 0x7fffffffffffffffll;
}

VO_U32 vo_m3u_manager::get_playlist_info_by_index(VO_U32 iIndex, VO_CHAR*  pURL, VO_U32*  pBitRate, VO_U32* pChunkCount)
{
    VO_U32   ulRet = 0xffffffff;
	VO_S32   iPlayListIndex = (VO_S32)iIndex;
	if(iPlayListIndex < m_playlist_count)
	{
	    playlist_item * pPlayList = m_ptr_playlist_list;
	    while(iIndex != 0)
	    {
	        pPlayList = pPlayList->ptr_next;
			iIndex--;
	    }

		memcpy(pURL, pPlayList->path, strlen(pPlayList->path));
		*pBitRate = pPlayList->band_width;
		*pChunkCount = m_playlist_item_count;
		return 0;
	}

	return ulRet;
}

VO_VOID vo_m3u_manager::get_all_bandwidth( VO_S64 * ptr_array , VO_S32 * ptr_size )
{
    if( !ptr_array && !ptr_size )
        return;

    if( !ptr_array )
    {
        *ptr_size = m_playlist_count;
        return;
    }

    playlist_item * ptr_item = m_ptr_playlist_list;
    
    if( !ptr_item )
        return;

    for( VO_S32 i = 0 ; i < m_playlist_count ; i++ )
    {
        if(ptr_item != NULL)
        {
            VOLOGI("the playlist url:%s", ptr_item->path);
			ptr_array[i] = ptr_item->band_width;
			ptr_item = ptr_item->ptr_next;
        }
		else
		{
		    break;
		}
    }
}

VO_CHAR*	vo_m3u_manager::GetManifestForIrdeto()
{
    return m_pBaseFileContentForIrdeto;
}

VO_CHAR*	vo_m3u_manager::GetM3uURLForIrdeto()
{
    VOLOGI("the ulr %s", m_urlForBaseUrlForIrdeto);
    return m_urlForBaseUrlForIrdeto;
}

void		vo_m3u_manager::AddAdFilterString(VO_CHAR* pFilterString)
{
    m_reader.AddPrivateTag(pFilterString);
}

void		vo_m3u_manager::ResetAdFilterString()
{
    m_reader.ResetPrivateTagHeadString();
}


VO_S32		vo_m3u_manager::GetCharpterIDTimeOffset(VO_S32	iCharpterId)
{
	voCAutoLock lock( &m_list_lock );

	media_item_internal * ptr_item_internal = m_ptr_item_head;

	VO_S32 total = 0;


	while( ptr_item_internal && (iCharpterId > ptr_item_internal->iCharpterId))
	{
	    total += ptr_item_internal->duration;
		ptr_item_internal = ptr_item_internal->ptr_next;
	}

	return total;
}

VO_S32		vo_m3u_manager::GetSequenceIDTimeOffset(VO_S32  iSequenceId)
{
	voCAutoLock lock( &m_list_lock );

	media_item_internal * ptr_item_internal = m_ptr_item_head;

	VO_S32 total = 0;


	while( ptr_item_internal && (iSequenceId > ptr_item_internal->sequence_number))
	{
	    total += ptr_item_internal->duration;
		ptr_item_internal = ptr_item_internal->ptr_next;
	}

	return total;

}

playlist_item*    vo_m3u_manager::GetThumbnailPlayList()
{
    if(m_ptr_playlist_list == NULL)
    {
        return NULL;
    }

    playlist_item*   pPlayList = m_ptr_playlist_list;
	while((pPlayList != NULL) &&
		  (pPlayList->band_width != 9999999 || pPlayList->uFlag != M3U_NORMAL_THUMBNAIl_STREAM)) 
	{
	    pPlayList = pPlayList->ptr_next;
	}

	return pPlayList;
}

VO_U32	vo_m3u_manager::GetStreamType(INF_item * ptr_item)
{
    if(ptr_item == NULL)
    {
        return M3U_INVALID_STREAM;
    }
	
    if(strcmp(ptr_item->strCodecs, "jpeg") == 0)
    {
        return M3U_NORMAL_THUMBNAIl_STREAM;
    }

	return M3U_NORMAL_UNKNOWN_STREAM;
}


thumbnail_item *  vo_m3u_manager::AddThumbnalItem( INF_item * ptr_item , VO_CHAR * ptr_relative_directory)
{
	thumbnail_item* ptr = new thumbnail_item;
	if(ptr == NULL)
	{
	    VOLOGI("lack of memory!");
		return NULL;
	}
	
	memset( ptr , 0 , sizeof(thumbnail_item) );
	get_absolute_path(ptr->path, ptr_item->path, ptr_relative_directory);
	ptr->iduration = ptr_item->duration;

	if( m_ptr_thumbnail_head == NULL && m_ptr_thumbnail_tail == NULL)
	{
		m_ptr_thumbnail_head = m_ptr_thumbnail_tail = ptr;
	}
	else
	{
		m_ptr_thumbnail_tail->ptr_next = ptr;
		m_ptr_thumbnail_tail = ptr;
	}

	m_ithumbnailCount++;

	return ptr;
}



void   vo_m3u_manager::GenerateThumbnailInfoList()
{
	voCAutoLock lock( &m_list_lock );

	VOLOGI( "Current Sequence Number: %d" , m_sequence_start );

	VO_CHAR * new_ptr_relative_directory = m_reader.get_relativedirectory();
	INF_item * ptr_new_item = m_reader.get_INF_list();
	
	if( is_live() )
	{
	    VOLOGE("the live stream has no thumbnailinfo!");
	    return;
	}

	thumbnail_item    *ptr_item = m_ptr_thumbnail_head;

	while( ptr_new_item )
	{
	    ptr_item = AddThumbnalItem( ptr_new_item , new_ptr_relative_directory );
		ptr_new_item = ptr_new_item->ptr_next;
	}

	return;
}

void   vo_m3u_manager::RemoveThumbnailInfoList()
{
    voCAutoLock lock( &m_list_lock );
	VO_BOOL    bFindThumbnail = VO_FALSE;
	
    playlist_item*   pPlayList = m_ptr_playlist_list;
	playlist_item*   pPlayListPre = m_ptr_playlist_list;
	while((pPlayListPre!=NULL))
	{
	    if(pPlayList->uFlag == M3U_NORMAL_THUMBNAIl_STREAM)
	    {
	        pPlayListPre->ptr_next = pPlayList->ptr_next;
			if(pPlayList->ptr_next != NULL)
			{
			    pPlayList->ptr_next->ptr_pre = pPlayListPre;
			}

			bFindThumbnail = VO_TRUE;
			break;
	    }

		pPlayListPre = pPlayList;
		pPlayList = pPlayList->ptr_next;
	}

	if(bFindThumbnail == VO_TRUE)
	{
		delete pPlayList;
	}
}


void    vo_m3u_manager::RemoveTheAudioPlayList()
{
    voCAutoLock lock( &m_list_lock );
	VO_BOOL    bFindAudioPlayList = VO_FALSE;
	VO_CHAR*   pFindAudioTrack = NULL;
	
    playlist_item*   pPlayList = m_ptr_playlist_list;
	playlist_item*   pPlayListPre = m_ptr_playlist_list;

	while((pPlayListPre!=NULL))
	{
	    VOLOGI("the playlist path:%s", pPlayList->path);

		if(pPlayList->path == NULL)
		{
		    break;
		}
        
	    pFindAudioTrack = strstr(pPlayList->path, "audio.m3u8");
        if(pFindAudioTrack == NULL)
        {
            pFindAudioTrack = strstr(pPlayList->path, "AUDIO.m3u8");
        }
        
	    if(pFindAudioTrack != NULL)
	    {
	        if(pPlayList == m_ptr_playlist_list)
			{
			    VOLOGI("the audio url is the first!");
                if(pPlayList->ptr_next != NULL)
				{
				    m_ptr_playlist_list = pPlayList->ptr_next;
				}
			}
			else
			{
			    pPlayListPre->ptr_next = pPlayList->ptr_next;
			    if(pPlayList->ptr_next != NULL)
			    {
			        pPlayList->ptr_next->ptr_pre = pPlayListPre;
				}
			}
			
	        VOLOGI("remove the audio playlist!");
			bFindAudioPlayList = VO_TRUE;
			break;
	    }

		pPlayListPre = pPlayList;
		pPlayList = pPlayList->ptr_next;
	}

	if(bFindAudioPlayList == VO_TRUE)
	{
		delete pPlayList;
		pPlayList = NULL;
        m_playlist_count--;
	}

	
}


VO_U32	 vo_m3u_manager::GetThumbnailItemCount()
{
    return m_ithumbnailCount;
}

VO_S32   vo_m3u_manager::FillThumbnailItem(S_Thumbnail_Item* pThumbnailList, VO_U32 ulThumbnailCount)
{
    VO_U32    ulIndex = 0;
	thumbnail_item    *ptr_item = m_ptr_thumbnail_head;
    if(pThumbnailList == NULL)
    {
        return ulIndex;
    }
	

	while((ptr_item!= NULL) && (ulIndex<ulThumbnailCount))
	{
	    memcpy(pThumbnailList[ulIndex].strThumbnailURL, ptr_item->path, strlen(ptr_item->path));
		pThumbnailList[ulIndex].ulDuration = (VO_U32)ptr_item->iduration;

		ulIndex++;
		ptr_item = ptr_item->ptr_next;
	}
    return ulIndex;
}


VO_VOID	vo_m3u_manager::SetParamForHttp(VO_U32  uId, VO_VOID* pData)
{
    m_reader.SetParamForHttp(uId, pData);
}

VO_VOID	vo_m3u_manager::SetStartBitrate(VO_U32  ulStartBitrate)
{
    m_illStartBitrate = (VO_S64)ulStartBitrate;
}

VO_VOID	vo_m3u_manager::SetCapBitrate(VO_U32  ulCapBitrate)
{
    m_illBitrateCap = (VO_S64)ulCapBitrate;
}


VO_VOID	vo_m3u_manager::SetEventCall(VO_PTR pEventCallback)
{
    VO_SOURCE2_EVENTCALLBACK*     pEventCallPtr = NULL;
    if(pEventCallback == NULL)
    {
        VOLOGI("The EventCallback is error!");
        return;
    }

    pEventCallPtr = (VO_SOURCE2_EVENTCALLBACK*)pEventCallback;
    m_sEventCallback.pUserData = pEventCallPtr->pUserData;
    m_sEventCallback.SendEvent = pEventCallPtr->SendEvent;
    VOLOGI("Set the EventCallback success!");
}

VO_VOID 	vo_m3u_manager::SetTheMaxDownloadFailForManifest(VO_PTR   pulMax)
{
    VO_U32    ulValue = 0;
    if(pulMax == NULL)
    {
        VOLOGI("Set the Max Vaule for Download Fail Error!");
    }

    ulValue = *((VO_U32*)(pulMax));
    VOLOGI("Set the Max Download fail count:%d", ulValue);
    m_ulMaxDownloadFailCount = ulValue;
}

VO_VOID vo_m3u_manager::TransactionForManifestDownloadResult(VO_BOOL  bManifestParseResult)
{
    if(bManifestParseResult == VO_FALSE)
    {
        m_ulCurrentDownloadFailCount++;
        if(m_ulMaxDownloadFailCount != 0 && m_ulCurrentDownloadFailCount>0 && (m_ulCurrentDownloadFailCount%m_ulMaxDownloadFailCount) == 0)
        {
            NotifyBeginWaitingManifestDownloadFailRecover();
        }
    }
    else
    {
        if(m_ulCurrentDownloadFailCount > m_ulMaxDownloadFailCount)
        {
            NotifyRecoverFromManifestDownloadFail();
        }            
        m_ulCurrentDownloadFailCount = 0;
    }
}

VO_VOID  	vo_m3u_manager::NotifyRecoverFromManifestDownloadFail()
{
    VOLOGI( "VO STATUS_HSL RECOVER FROM DOWNLOAD FAIL IN MANIFEST DOWNLOAD!");
    if(m_sEventCallback.pUserData != NULL && m_sEventCallback.SendEvent != NULL)
    {
        m_sEventCallback.SendEvent(m_sEventCallback.pUserData, VO_EVENTID_SOURCE2_DOWNLOAD_FAIL_RECOVER_SUCCESS, NULL, NULL);
    }
}
VO_VOID  	vo_m3u_manager::NotifyBeginWaitingManifestDownloadFailRecover()
{
    VOLOGI( "VO STATUS_HSL DOWNLOAD FAIL CONSISTENT TIME :%d IN MANIFEST DOWNLOAD!", m_ulCurrentDownloadFailCount);
    if(m_sEventCallback.pUserData != NULL && m_sEventCallback.SendEvent != NULL)
    {
        m_sEventCallback.SendEvent(m_sEventCallback.pUserData, VO_EVENTID_SOURCE2_START_DOWNLOAD_FAIL_WAITING_RECOVER, NULL, NULL);
    }
}


VO_U32	vo_m3u_manager::GetTheTopManifestLength()
{
    return m_BaseFileContentSizeForIrdeto;
}

VO_CHAR*	vo_m3u_manager::GetTheTopManifestDataPointer()
{
    if(m_BaseFileContentSizeForIrdeto > 0)
    {
        return m_pBaseFileContentForIrdeto;
    }
    else
    {
        return NULL;
    }
}



