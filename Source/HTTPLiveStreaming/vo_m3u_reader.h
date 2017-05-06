#pragma once

#include "vo_webdownload_stream.h"
#include "DRM_API.h"
//#include <map>

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


class AutoBuffer
{
public:
	AutoBuffer();
	~AutoBuffer();

	VO_S32 add_ref();
	VO_S32 release();

	VO_PBYTE ptr_buffer;
	VO_S32 buffer_size;
	VO_S32 tag;

private:
	VO_S32 m_ref;
};

/*class AutoBuffer_Counter
{
public:
	AutoBuffer_Counter();
	~AutoBuffer_Counter();

	void add( void * ptr );
	void release( void * ptr );

private:
	std::map< void * , int > m_map;
};*/


enum M3U_STREAM_TYPE
{
    M3U_NORMAL_UNKNOWN_STREAM,
    M3U_NORMAL_TS_STREAM,
	M3U_NORMAL_AAC_STREAM,
	M3U_NORMAL_THUMBNAIl_STREAM,
	M3U_INVALID_STREAM
};

enum M3U_MEDIA_USAGE_TYPE
{
	M3U_COMMON_USAGE_TYPE,
	M3U_COMMON_AD_TYPE,
	M3U_INVALID_TYPE
};

enum M3U_MEDIA_RELOAD_TYPE
{
    M3U_RELOAD_NULL_TYPE,
	M3U_RELOAD_RESET_TIMESTAMP_TYPE,
	M3U_RELOAD_RESET_CONTEXT_ONLY_TYPE,
	M3U_RELOAD_INVALID_TYPE
};

enum M3U_PLAYLIST_TYPE
{
	M3U_VARIANT_PLAYLIST,
	M3U_PLAYLIST
};

struct INF_item
{
	//VO_CHAR title[256];
	VO_CHAR path[1024];
	VO_CHAR strCodecs[32];
	
	union
	{
		struct
		{
			VO_S32 duration;
		};

		struct
		{
			VO_S64 bandwidth;
		};
	};

	VO_S32 sequence;
    VO_U32 ulSequenceForKey;
	VO_BOOL discontinuty;
	VO_U32 deadtime;

	VO_BOOL is_encrypt;
	AutoBuffer * ptr_key;
	VO_BYTE iv[16];
	ProtectionType drm_type;

	//Add For AD
	M3U_MEDIA_USAGE_TYPE    eMediaUsageType;
	VO_CHAR*                pFilterString;
	VO_S32                  iCharpterId;
	//Add For AD

	//Add for the Discretix
    VO_CHAR                 strEXTKEYLine[1024];
    //Add for the Discretix
	INF_item * ptr_next;
};

struct  S_READER_USER_INFO
{
    VO_CHAR strUserName[256];
	VO_S32  ulstrUserNameLen;
    VO_CHAR strPasswd[256];
	VO_S32  ulstrPasswdLen;
};


class vo_m3u_reader
{
public:
	vo_m3u_reader(void);
	~vo_m3u_reader(void);

	VO_BOOL parse( VO_CHAR * url );

	VO_BOOL is_live_stream(){ return m_is_live; }
	VO_S32 get_INF_count(){ return m_item_count; }
	VO_S32 get_maxitem_duration(){ return m_TARGETDURATION; }
	VO_CHAR * get_relativedirectory(){ return m_relative_directory; }
	VO_CHAR * get_hostdirectory(){ return m_host_directory; }
	
	INF_item * get_INF_list(){ return m_ptr_INF_head; }

	M3U_PLAYLIST_TYPE get_playlist_type(){ return m_type; }

	VO_VOID set_MEDIA_SEQUENCE( VO_S32 seq ){ m_MEDIA_SEQUENCE = seq; }

	VO_S32 get_update_interval(){ if( m_ptr_INF_tail ) return m_ptr_INF_tail->duration; else return 1; }

	VO_BOOL is_loop(){ return m_is_loop; }

	VO_CHAR*   GetManifest(){return m_file_content;}
	VO_S64     GetManifestContentSize(){ return m_file_content_size;}
	
	VO_BOOL    GetDrmState(){return m_is_encrypt;}
	VO_BOOL    AddPrivateTag(VO_CHAR*	pTagHead);
	VO_VOID    ResetPrivateTagHeadString();
	VO_S32     GetCharpterCount();

    VO_VOID	   SetParamForHttp(VO_U32  uId, VO_VOID* pData);
	VO_VOID    ResetUserInfo();
	VO_U32     GetTheSequenceMin();
	VO_U32     GetTheSequenceMax();


protected:

	VO_VOID get_relative_directory( VO_CHAR * url );
	VO_VOID get_host_directory( VO_CHAR * url );
	VO_BOOL get_m3u( VO_CHAR * url );

	VO_VOID destroy();

	VO_BOOL analyze();

	VO_VOID get_line( VO_CHAR * ptr_source , VO_CHAR * ptr_dest );

	VO_VOID get_TARGETDURATION( VO_CHAR * ptr );
	VO_VOID get_MEDIA_SEQUENCE( VO_CHAR * ptr );
	VO_VOID get_INF( VO_CHAR * ptr );
	VO_VOID get_SREAM_INF( VO_CHAR * ptr );
	VO_VOID get_X_KEY( VO_CHAR * ptr );

	//
	VO_VOID get_ADItem( VO_CHAR * ptr, VO_CHAR * ptrFilterString );
	//

	VO_VOID add_INF( INF_item * ptr_item );

	VO_VOID get_iv_from_int( VO_BYTE * ptr_iv , VO_U32 value );

	VO_VOID str2key( VO_PBYTE ptr_key , VO_CHAR * str_key );

protected:
	vo_webdownload_stream m_stream;

	VO_CHAR * m_file_content;
	VO_S64 m_file_content_size;
	VO_CHAR m_relative_directory[1024];	
	VO_CHAR m_host_directory[1024];
	VO_CHAR m_redirectURL[1024];

	VO_S32 m_TARGETDURATION;
	VO_S32 m_MEDIA_SEQUENCE;

	VO_S32 m_sequence_counter;

	VO_S32 m_item_count;

	VO_U32 m_read_time;

	VO_BOOL m_is_live;
	VO_BOOL m_is_DISCONTINUITY;
	VO_BOOL m_is_loop;

	M3U_PLAYLIST_TYPE m_type;

	INF_item * m_ptr_INF_head;
	INF_item * m_ptr_INF_tail;

	VO_BOOL m_is_encrypt;
	AutoBuffer * m_ptr_key;
	VO_S32 m_keytag;
	VO_BOOL m_has_globaliv;
	VO_BYTE m_iv[16];
	ProtectionType m_drm_type;


	VO_CHAR*   m_apPrivateTagHeadForAD[8];
	VO_S32     m_iPrivateTagHeadForADCount;
	VO_S32     m_iCharpterId;

    S_READER_USER_INFO    m_sUserInfo;
    VO_BOOL    m_bExistUserInfo;
    VO_BOOL    m_bGotSequenceIDFirstTime;
    VO_U32     m_ulMediaSequenceForKey;
    VO_CHAR    m_strCurrentEXTKEYLine[1024];
    ProtectionType      m_eCurrentProtectionType;
	

	VO_S32 m_duration;
};
#ifdef _VONAMESPACE
}
#endif

