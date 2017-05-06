#include "vo_m3u_reader.h"
#include "voString.h"
#include "voOSFunc.h"
#include "voLog.h"
#include "math.h"
//#include "vo_https_stream.h"

#ifndef LOG_TAG
#define LOG_TAG "vo_m3u_reader"
#endif


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

AutoBuffer::AutoBuffer()
:ptr_buffer(0)
,buffer_size(0)
,tag(0)
{
    m_ref = 0;
	//VOLOGI( "Create %p" , this );
}

AutoBuffer::~AutoBuffer()
{
	if( ptr_buffer )
		delete []ptr_buffer;
}

VO_S32 AutoBuffer::add_ref()
{
	return ++m_ref;
}

VO_S32 AutoBuffer::release()
{
	m_ref--;
	if( m_ref <= 0 )
	{
		//VOLOGI( "Delete %p" , this );
		delete this;
		return 0;
	}

	return m_ref;
}

/*AutoBuffer_Counter::AutoBuffer_Counter()
{
	;
}

AutoBuffer_Counter::~AutoBuffer_Counter()
{
	std::map< void * , int >::iterator start = m_map.begin();
	std::map< void * , int >::iterator end = m_map.end();

	VOLOGI( "*************Begin Dump Leak*************" );
	while( start != end )
	{
		VOLOGI( "Leak at %p , ref %d" , start->first , start->second );
	}
	VOLOGI( "*************End Dump Leak*************" );
}

void AutoBuffer_Counter::add( void * ptr )
{
	if( m_map.find( ptr ) != m_map.end() )
		m_map[ptr]++;
	else
		m_map[ptr] = 1;
}

void AutoBuffer_Counter::release( void * ptr )
{
	if( m_map.find( ptr ) != m_map.end() )
	{
		m_map[ptr]--;

		if( m_map[ptr] == 0 )
		{
			m_map.erase(ptr);
		}
	}
	else
	{
		VOLOGI( "+++++++++++++++Unknown Pointer delete %p" , ptr );
	}
}*/

vo_m3u_reader::vo_m3u_reader(void)
{
    m_file_content = 0;
	m_ptr_INF_head = 0;
	m_ptr_INF_tail = 0;
	m_is_live = VO_TRUE;
	m_is_DISCONTINUITY = VO_FALSE;
	m_item_count = 0;
	m_MEDIA_SEQUENCE = 0;
	m_sequence_counter = 0;
	m_type = M3U_PLAYLIST;
	m_is_encrypt = VO_FALSE;
	m_has_globaliv = VO_FALSE;
	m_ptr_key = 0;
	m_keytag = 0;
	m_is_loop = VO_FALSE;

    memset(&m_apPrivateTagHeadForAD, 0, sizeof(m_apPrivateTagHeadForAD));
    memset(&m_sUserInfo, 0, sizeof(S_READER_USER_INFO));
    m_iPrivateTagHeadForADCount = 0;
	m_bExistUserInfo = VO_FALSE;

    m_file_content_size = 0;
    memset(m_redirectURL, 0, 1024);
    m_bGotSequenceIDFirstTime = VO_FALSE;
    m_eCurrentProtectionType = AES128;

    memset(m_strCurrentEXTKEYLine, 0, 1024);

    //For Test

	
	//For Test

	//set the m_iCharpterId
	m_iCharpterId = 0;
}

vo_m3u_reader::~vo_m3u_reader(void)
{
	destroy();
}

VO_BOOL vo_m3u_reader::parse( VO_CHAR * url )
{
	destroy();


	VOLOGI("get_m3u start");
	if( VO_FALSE == get_m3u( url ) )
	{
		VOLOGE("get_m3u failed!");
		return VO_FALSE;
	}
	VOLOGI("get_m3u end");

	get_relative_directory( url );
	get_host_directory( url);


	analyze();
	ResetUserInfo();
	return VO_TRUE;
}

VO_VOID vo_m3u_reader::get_relative_directory( VO_CHAR * url )
{
    VO_CHAR    strURL[1024] = {0};
    VO_CHAR*   ptrForParam = NULL;
    VO_U32    ulStrOriginalLen = 0;
    if(m_type == M3U_VARIANT_PLAYLIST)
    {
        memset( strURL , 0 , sizeof( strURL ) );
        strcpy( strURL , url );
    }
    else
    {
        if(strlen(m_redirectURL) != 0)
        {
            memset( strURL , 0 , sizeof( strURL ) );
            strcpy( strURL , m_redirectURL );
            //reset the m_redirectURL
            VOLOGI("Reset the m_redirectURL");
            memset(m_redirectURL, 0, 1024);
        }
        else
        {
            memset( strURL , 0 , sizeof( strURL ) );
            strcpy( strURL , url );
        }        
    }

    ulStrOriginalLen = strlen(strURL);
    ptrForParam = strrchr( strURL , '?' );
    if(ptrForParam != NULL)
    {
        memset(ptrForParam+1, 0, ulStrOriginalLen-(ptrForParam-strURL-1));
        *ptrForParam = '\0';
    }


	VO_CHAR * ptr = strrchr( strURL , '/' );
	ptr++;
	*ptr = '\0';
    memset(m_relative_directory, 0, 1024);
    memcpy(m_relative_directory, strURL, strlen(strURL));
    VOLOGI("relative URL:%s", m_relative_directory);

}

VO_VOID vo_m3u_reader::get_host_directory( VO_CHAR * url )
{
	VO_CHAR * ptr  = NULL;
	memset( m_host_directory , 0 , sizeof( m_host_directory ) );
	strcpy( m_host_directory , url );

	//skip http://  first
	if(strstr( m_host_directory, "http://") != NULL)
    {
        ptr = strchr( m_host_directory+strlen("http://") , '/');
        if(ptr != NULL)
        {
            *ptr = '\0';
        }
    }
    else
    {
        if(strstr(m_host_directory, "https://") != NULL)
        {
            ptr = strchr( m_host_directory+strlen("https://") , '/');        
            if(ptr != NULL)
            {
                *ptr = '\0';
            }
        }
    }

	VOLOGE("the host path:%s", m_host_directory);
}


VO_BOOL    vo_m3u_reader::AddPrivateTag(VO_CHAR*  pTagHead)
{
	int iIndex = 0;

    
    if(NULL == pTagHead)
    {
        return VO_FALSE;
    }

    if(m_iPrivateTagHeadForADCount >= 8)
    {
        return VO_FALSE;
    }
		
    for(iIndex=0; iIndex<m_iPrivateTagHeadForADCount; iIndex++)
    {
        if(memcmp( pTagHead , m_apPrivateTagHeadForAD[iIndex] , strlen(m_apPrivateTagHeadForAD[iIndex])) == 0)
        {
            return VO_FALSE;
        }
    }

	m_apPrivateTagHeadForAD[m_iPrivateTagHeadForADCount] = pTagHead;
	m_iPrivateTagHeadForADCount++;
	return VO_TRUE;
}

VO_VOID    vo_m3u_reader::ResetPrivateTagHeadString()
{
    memset(m_apPrivateTagHeadForAD, 0, sizeof(m_apPrivateTagHeadForAD));
    m_iPrivateTagHeadForADCount = 0;
}

VO_BOOL vo_m3u_reader::get_m3u( VO_CHAR * url )
{
	//VOLOGI("begin download m3u");
    if(m_bExistUserInfo == VO_TRUE)
    {
        VOLOGI("set the user:%s and pwd:%s for http straem", m_sUserInfo.strUserName, m_sUserInfo.strPasswd);
        m_stream.SetParamForHttp(1, m_sUserInfo.strUserName, m_sUserInfo.strPasswd);
    }
	
	if( VO_FALSE == m_stream.open( url , DOWNLOAD2MEM ) )
		return VO_FALSE;
	//VOLOGI("end download m3u");

	m_read_time = voOS_GetSysTime();


    memset(m_redirectURL, 0, 1024);

	VO_S64 content_size = m_stream.get_content_length();
	VOLOGI("m3u content length: %d" , (VO_S32)content_size);

	while( content_size == -1 )
	{
		voOS_Sleep( 500 );
		content_size = m_stream.get_content_length();
	}

	m_file_content = new VO_CHAR[ (VO_U32)content_size + 1 ];
    if(m_file_content == NULL)
    {
        VOLOGE("malloc memory error!, the content_size:%d", content_size);
        m_file_content_size = 0;
        return VO_FALSE;
    }
	m_file_content_size = content_size;
	memset( m_file_content , 0 , (VO_U32)content_size + 1 );

	VO_CHAR * ptr = m_file_content;

	while( content_size != 0 )
	{
		VO_S64 size = m_stream.read( (VO_PBYTE)ptr , content_size );

		if( size == -2 )
			continue;

		//VOLOGI("never arrive here!");

		if( size == -1 )
		{
			m_stream.close();
			return VO_FALSE;
		}

		//VOLOGI("content readed: %d" , size);
		content_size -= size;
		ptr = ptr + size;
	}

	VOLOGI( "M3U Content: %s" , m_file_content );
    m_stream.GetReDirectionURL(m_redirectURL, 1024);
    VOLOGI("the m_redirectURL:%s", m_redirectURL);

	//VOLOGI("+m_stream.close");
	m_stream.close();
	//VOLOGI("-m_stream.close");

	return VO_TRUE;
}

VO_S32  vo_m3u_reader::GetCharpterCount()
{
    return m_iCharpterId+1;
}

VO_VOID    vo_m3u_reader::SetParamForHttp(VO_U32  uId, VO_VOID* pData)
{
    S_READER_USER_INFO*   pReadInfo = (S_READER_USER_INFO*)pData;
    if(pData == NULL)
	{
	    return;
	}

	memcpy(m_sUserInfo.strUserName, pReadInfo->strUserName, strlen(pReadInfo->strUserName));
    m_sUserInfo.ulstrUserNameLen = strlen(pReadInfo->strUserName);
	memcpy(m_sUserInfo.strPasswd, pReadInfo->strPasswd, strlen(pReadInfo->strPasswd));
    m_sUserInfo.ulstrPasswdLen = strlen(pReadInfo->strPasswd);
	m_bExistUserInfo = VO_TRUE;
}

VO_VOID    vo_m3u_reader::ResetUserInfo()
{
    memset(&m_sUserInfo, 0, sizeof(S_READER_USER_INFO));
	m_bExistUserInfo = VO_FALSE;
}

VO_VOID vo_m3u_reader::destroy()
{
	if( m_file_content )
	{
		delete []m_file_content;
		m_file_content = 0;
	}

	INF_item * ptr_item = m_ptr_INF_head;

	while( ptr_item )
	{
		INF_item * ptr_temp = ptr_item;
		ptr_item = ptr_item->ptr_next;

		if( ptr_temp->ptr_key )
        {
	        ptr_temp->ptr_key->release();
        }

		delete ptr_temp;
	}

// 	if( m_ptr_key )
// 		m_ptr_key->release();

	m_ptr_INF_head = m_ptr_INF_tail = NULL;
	//m_is_live = VO_TRUE;
	m_is_DISCONTINUITY = VO_FALSE;
	m_item_count = 0;
	m_sequence_counter = m_MEDIA_SEQUENCE = 0;
	m_type = M3U_PLAYLIST;
	m_ptr_key = NULL;
    m_is_encrypt = VO_FALSE;
    m_has_globaliv = VO_FALSE;
    m_keytag = 0;    
    memset(m_redirectURL, 0, 1024);
}


VO_U32     vo_m3u_reader::GetTheSequenceMin()
{
    return m_MEDIA_SEQUENCE;
}
VO_U32     vo_m3u_reader::GetTheSequenceMax()
{
    return m_sequence_counter;
}


VO_BOOL vo_m3u_reader::analyze()
{
    int iIndex = 0;

	//set the m_iCharpterId
	m_iCharpterId = 0;
    m_bGotSequenceIDFirstTime = VO_FALSE;
		
	VO_BOOL   bFindFlag = VO_FALSE;
	
	if( memcmp( m_file_content , "#EXTM3U" , strlen( "#EXTM3U" ) ) )
		return VO_FALSE;

	VO_CHAR * ptr = m_file_content;
	m_duration = 0;

	while( ptr && ptr - m_file_content < m_file_content_size )
	{
		if( memcmp( ptr , "#EXT-X-TARGETDURATION" , strlen("#EXT-X-TARGETDURATION") ) == 0 )
		{
			get_TARGETDURATION( ptr );
		}
		else if( memcmp( ptr , "#EXT-X-MEDIA-SEQUENCE" , strlen("#EXT-X-MEDIA-SEQUENCE") ) == 0 )
		{
			get_MEDIA_SEQUENCE( ptr );
		}
		else if( memcmp( ptr , "#EXTINF" , strlen( "#EXTINF" ) ) == 0 )
		{
			get_INF( ptr );
		}
		else if( memcmp( ptr , "#EXT-X-ENDLIST" , strlen( "#EXT-X-ENDLIST" ) ) == 0 )
		{
			m_is_live = VO_FALSE;
		}
		else if( memcmp( ptr , "#EXT-X-DISCONTINUITY" , strlen( "#EXT-X-DISCONTINUITY" ) ) == 0 )
		{
			m_is_DISCONTINUITY = VO_TRUE;
			m_iCharpterId++;
		}
		else if( memcmp( ptr , "#EXT-X-STREAM-INF" , strlen("#EXT-X-STREAM-INF") ) == 0 )
		{
			get_SREAM_INF( ptr );
		}
		else if( memcmp( ptr , "#EXT-X-KEY" , strlen( "#EXT-X-KEY" ) ) == 0 )
		{
			get_X_KEY( ptr );
		}
		else if( memcmp( ptr , "#EXT_VISUALON_PLAYLOOP" , strlen("#EXT_VISUALON_PLAYLOOP") ) == 0 )
		{
			m_is_loop = VO_TRUE;
		}
        else if(memcmp( ptr , "#EXT-X-DX" , strlen("#EXT-X-DX"))== 0 )
        {
            VOLOGI("Get the #EXT-X-DX Tag!");
            m_eCurrentProtectionType = THIRD_PART_DRM;
        }

		else if(m_iPrivateTagHeadForADCount > 0)
		{	
		    iIndex = 0;
			bFindFlag = VO_FALSE;
		    while((bFindFlag == VO_FALSE)&& (iIndex<m_iPrivateTagHeadForADCount))
		    {
		        if(memcmp( ptr , m_apPrivateTagHeadForAD[iIndex], strlen(m_apPrivateTagHeadForAD[iIndex])) == 0 )
		        {
		            bFindFlag = VO_TRUE;
					break;
		        }

				iIndex++;
		    }

			if(bFindFlag == VO_TRUE)
		    {
		        VOLOGI( "Find in iIndex : %d" , iIndex );
		        get_ADItem( ptr, m_apPrivateTagHeadForAD[iIndex]);
		    }
		}

		ptr++;
		ptr = strstr( ptr , "#EXT" );
	}

    VOLOGI("Parse the Manifest End!");

	INF_item * ptr_item = m_ptr_INF_head;

	while( ptr_item )
	{
		ptr_item->deadtime += m_duration;
		ptr_item = ptr_item->ptr_next;
	}

	return VO_TRUE;
}

VO_VOID vo_m3u_reader::get_line( VO_CHAR * ptr_source , VO_CHAR * ptr_dest )
{
	//while( *ptr_source == '\r' || *ptr_source == '\n' )
		//ptr_source++;

	while( *ptr_source != '\r' && *ptr_source != '\n' && *ptr_source != '\0' )
	{
		*ptr_dest = *ptr_source;
		ptr_dest++;
		ptr_source++;
	}
}

VO_VOID vo_m3u_reader::get_TARGETDURATION( VO_CHAR * ptr )
{
    int  iDuration = 0;
	VO_CHAR line[ 1024 ];
	memset( line , 0 , 1024 );
	get_line( ptr , line );

	VO_CHAR * pos = line;
	pos = pos + strlen( "#EXT-X-TARGETDURATION" );

	while( *pos < '0' || *pos > '9' )
		pos++;

	sscanf( pos , "%d" , &iDuration );
	m_TARGETDURATION = iDuration;
}

VO_VOID vo_m3u_reader::get_MEDIA_SEQUENCE( VO_CHAR * ptr )
{
    int   iGetValue = 0;
	VO_CHAR line[ 1024 ];
	memset( line , 0 , 1024 );
	get_line( ptr , line );

	VO_CHAR * pos = line;
	pos = pos + strlen( "#EXT-X-MEDIA-SEQUENCE" );

	while( *pos < '0' || *pos > '9' )
		pos++;

	sscanf( pos , "%d" , &iGetValue);
	m_MEDIA_SEQUENCE = iGetValue;
    if(m_bGotSequenceIDFirstTime == VO_FALSE)
    {
        m_bGotSequenceIDFirstTime = VO_TRUE;
        m_sequence_counter = m_MEDIA_SEQUENCE;        
        m_ulMediaSequenceForKey = m_MEDIA_SEQUENCE;
    }
    else
    {
        m_ulMediaSequenceForKey = m_MEDIA_SEQUENCE;
    }

	VOLOGI( "MEDIA_SEQUENCE : %d" , m_sequence_counter );
}

VO_VOID vo_m3u_reader::get_INF( VO_CHAR * ptr )
{
    int iGetValue = 0;
	INF_item * ptr_item = new INF_item;
	memset( ptr_item , 0 , sizeof( INF_item ) );

	VO_CHAR line[ 1024 ];
	memset( line , 0 , 1024 );
	get_line( ptr , line );

	VO_CHAR * pos = line;
	pos = pos + strlen( "#EXTINF" );

	while( *pos < '0' || *pos > '9' )
		pos++;

	sscanf( pos , "%d" , &(iGetValue) );
	ptr_item->duration = iGetValue;

	m_duration += ptr_item->duration;

	pos = ptr + strlen( line );

	while( *pos == '\r' || *pos == '\n' )
		pos++;

	get_line( pos , ptr_item->path );

    //try 20 times
    for( VO_S32 i = 0 ; i < 20 ; i++ )
    {
        if( strstr( ptr_item->path , "#EXT-X-KEY" ) == ptr_item->path )
        {
            get_X_KEY( ptr_item->path );

            pos = pos + strlen( ptr_item->path );

            while( *pos == '\r' || *pos == '\n' )
                pos++;

            memset( ptr_item->path , 0  , sizeof( ptr_item->path ) );
            get_line( pos , ptr_item->path );
        }
        else if( strstr( ptr_item->path , "#" ) == ptr_item->path )
        {
            pos = pos + strlen( ptr_item->path );
            while( *pos == '\r' || *pos =='\n' )
                pos++;

            memset( ptr_item->path , 0  , sizeof( ptr_item->path ) );
            get_line( pos , ptr_item->path );
        }
        else
            break;
    }

	VOLOGI("%s" , ptr_item->path);

	ptr_item->eMediaUsageType = M3U_COMMON_USAGE_TYPE;
	ptr_item->pFilterString = NULL;

	add_INF( ptr_item );
}


VO_VOID vo_m3u_reader::get_ADItem( VO_CHAR * ptr , VO_CHAR * ptrFilterString )
{
	VO_CHAR * pos = NULL;

	INF_item * ptr_item = new INF_item;
	memset( ptr_item , 0 , sizeof( INF_item ) );

	VO_CHAR line[ 1024 ];
	memset( line , 0 , 1024 );
	get_line( ptr, line);

	pos = ptr + strlen( line );

	while( *pos == '\r' || *pos == '\n' )
	{	
	    pos++;
	}
	
	get_line(pos, ptr_item->path);

	VOLOGI("path:%s" , ptr_item->path);
	VOLOGI("FilterString:%s" , ptrFilterString);

    ptr_item->eMediaUsageType = M3U_COMMON_AD_TYPE;
	ptr_item->pFilterString = ptrFilterString;
	add_INF( ptr_item );
}

VO_VOID vo_m3u_reader::get_SREAM_INF( VO_CHAR * ptr )
{
	m_type = M3U_VARIANT_PLAYLIST;

	INF_item * ptr_item = new INF_item;
	memset( ptr_item , 0 , sizeof( INF_item ) );

	VO_CHAR line[ 1024 ];
	memset( line , 0 , 1024 );
	get_line( ptr , line );

	VO_CHAR * pos = strstr( line , "BANDWIDTH=" );
	VO_CHAR * posEnd = NULL;

	if( pos )
	{
		pos = pos + strlen( "BANDWIDTH=" );
		ptr_item->bandwidth = atoi( pos );
	}

    pos = strstr( line , "CODECS=");

	if(pos)
	{
	    pos = pos + strlen("CODECS=") + 1; // 
		posEnd = pos+2;
		while((*posEnd) != '"')
		{
		    posEnd++;
		}

		if((posEnd-pos) < 31)
		{
		    memcpy(ptr_item->strCodecs, pos, posEnd-pos);
		}
	}

	


	pos = ptr + strlen( line );

	while( *pos == '\r' || *pos == '\n' )
		pos++;

	get_line( pos , ptr_item->path );

	//add for remove the pure audio
	if(memcmp(ptr_item->strCodecs, "mp4a", sizeof("mp4a")) == 0)
	{
	    VOLOGI("this stream is pure audio!");
		if(ptr_item != NULL)
		{
		    delete ptr_item;
			ptr_item = NULL;
		}

		return;
	}

	add_INF( ptr_item );
}

VO_VOID vo_m3u_reader::get_X_KEY( VO_CHAR * ptr )
{
	//VOLOGI( "vo_m3u_reader::get_X_KEY %s" , ptr );
	VO_CHAR line[2500];
	memset( line , 0 , 2500 );


	get_line( ptr , line );

    //Add for Discretix
    memset(m_strCurrentEXTKEYLine, 0, 1024);
    memcpy(m_strCurrentEXTKEYLine, line, strlen(line));
    VOLOGI("the EXTKEY Line:%s", m_strCurrentEXTKEYLine);
    //Add for Discretix
    
	VO_CHAR * ptr_temp = strstr( line , "METHOD=" );

	if( ptr_temp )
	{	    
        m_is_encrypt = VO_TRUE;
		ptr_temp = ptr_temp + strlen( "METHOD=" );

		if( memcmp( ptr_temp , "NONE" , strlen("NONE") ) == 0 )
		{
			m_is_encrypt = VO_FALSE;
		}
		else if( memcmp( ptr_temp , "AES-128" , strlen( "AES-128" ) ) == 0 && m_eCurrentProtectionType == AES128)
		{
			m_drm_type = AES128;
			m_is_encrypt = VO_TRUE;
		}
        else if(m_eCurrentProtectionType != AES128)
        {
            m_drm_type = m_eCurrentProtectionType;
            m_is_encrypt = VO_TRUE;
        }
	}

	if( m_is_encrypt )
	{
	    if(m_drm_type == AES128)
        {   
    		ptr_temp = strstr( line , "URI=" );

    		if( ptr_temp )
    		{
    			ptr_temp = ptr_temp + strlen("URI=\"");

    			VO_S32 size;
    			VO_CHAR * ptr = strchr( ptr_temp , '\"' );

    			if( ptr )
    				size = ptr - ptr_temp;
    			else
    				size = strlen( ptr_temp );

    // 			if( m_ptr_key )
    // 			{
    // 				m_ptr_key->release();
    // 				m_ptr_key = NULL;
    // 			}
    			
    			m_keytag++;
    			m_ptr_key = new AutoBuffer;
    			m_ptr_key->tag = m_keytag;
    			m_ptr_key->buffer_size = size;
    			m_ptr_key->ptr_buffer = new unsigned char[ size + 1 ];
    			memset( m_ptr_key->ptr_buffer , 0 , size + 1 );
    			memcpy( m_ptr_key->ptr_buffer , ptr_temp , size );
    			//m_ptr_key->add_ref();
    		}
    		else
    		{
    			m_is_encrypt = VO_FALSE;
    			m_has_globaliv = VO_FALSE;

    			return;
    		}

    		ptr_temp = strstr( line , "IV=" );

    		if( ptr_temp )
    		{
    			m_has_globaliv = VO_TRUE;

    			VO_CHAR * ptr = strchr( ptr_temp , ',' );

    			if( ptr )
    			{
    				*ptr = '\0';
    			}

    			ptr_temp = ptr_temp + strlen( "IV=" );

    			if( strstr(ptr_temp,"0x") == ptr_temp || strstr(ptr_temp,"0X") == ptr_temp )
    			{
    				ptr_temp = ptr_temp + strlen( "0x" );
    				str2key( m_iv , ptr_temp );
    			}
    			else
    			{
    				//VOLOGI( "Get IV str: %s" , ptr_temp );
    				strcpy( (char *)m_iv , ptr_temp );
    			}
    		}
    		else
    		{
    			m_has_globaliv = VO_FALSE;
    		}
        }
	}
}

VO_VOID vo_m3u_reader::add_INF( INF_item * ptr_item )
{
	if( m_ptr_INF_head == NULL && m_ptr_INF_tail == NULL )
		m_ptr_INF_head = m_ptr_INF_tail = ptr_item;
	else
	{
		m_ptr_INF_tail->ptr_next = ptr_item;
		m_ptr_INF_tail = ptr_item;
	}

	ptr_item->deadtime = m_read_time + ptr_item->duration * 1000;
	m_read_time = m_read_time + ptr_item->duration * 1000;
	ptr_item->sequence = m_sequence_counter;
    ptr_item->ulSequenceForKey = m_ulMediaSequenceForKey;
	ptr_item->discontinuty = m_is_DISCONTINUITY;
	ptr_item->iCharpterId = m_iCharpterId;
    
    //Add for Discretix
    memset(ptr_item->strEXTKEYLine, 0, 1024);
    memcpy(ptr_item->strEXTKEYLine, m_strCurrentEXTKEYLine, strlen(m_strCurrentEXTKEYLine));
    //Add for Discretix
    VOLOGI("the Sequence ID For Order:%d, the Sequence ID For Key:%d", ptr_item->sequence, ptr_item->ulSequenceForKey);

	ptr_item->is_encrypt = m_is_encrypt;
	if( m_is_encrypt )
	{
	    ptr_item->drm_type = m_drm_type;
		ptr_item->ptr_key = m_ptr_key;

		if( m_ptr_key )
			m_ptr_key->add_ref();

		if( m_has_globaliv )
		{
			memcpy( ptr_item->iv , m_iv , 16 );
		}
		else
		{
			get_iv_from_int( ptr_item->iv , m_ulMediaSequenceForKey );
		}

		ptr_item->drm_type = m_drm_type;
	}


	m_is_DISCONTINUITY = VO_FALSE;
	m_sequence_counter++;
    m_ulMediaSequenceForKey++;

	m_item_count++;
}

VO_VOID vo_m3u_reader::get_iv_from_int( VO_BYTE * ptr_iv , VO_U32 value )
{
	memset( ptr_iv , 0 , 16 );

	VO_BYTE temp[16];
	VO_S32 size = 0;

	for( VO_S32 i = 0 ; i < 16 ; i++ )
	{
		VO_S32 val = value % 256;
		value = value / 256;

		temp[size] = (VO_BYTE)val;
		size++;

		if( value <= 0 )
			break;
	}

	for( VO_S32 i = 0 ; i < size ; i++ )
		ptr_iv[ 16 - i - 1 ] = temp[i];
}

VO_VOID vo_m3u_reader::str2key( VO_PBYTE ptr_key , VO_CHAR * str_key )
{
	VO_CHAR key[33];
	strcpy( key , "00000000000000000000000000000000\0" );

	int size = strlen( str_key );

	VO_CHAR * ptr_pos = key + 32 - size;

	strcpy( ptr_pos , str_key );

	for( VO_S32 i = 0 ; i < 16 ; i++ )
	{
		VO_CHAR temp[3];

		temp[0] = key[ 2*i ];
		temp[1] = key[ 2*i + 1 ];
		temp[2] = '\0';

		int value;

		sscanf( temp , "%x" , &value );

		ptr_key[i] = value;
	}

	VOLOGI( "key: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X" , ptr_key[0] , ptr_key[1] , ptr_key[2] , ptr_key[3] , ptr_key[4] , ptr_key[5] ,
		ptr_key[6] , ptr_key[7] , ptr_key[8] , ptr_key[9] , ptr_key[10] , ptr_key[11] , ptr_key[12] , ptr_key[13] , ptr_key[14] , ptr_key[15] );
}
