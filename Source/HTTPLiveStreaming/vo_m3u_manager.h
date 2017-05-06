#pragma once

#include "vo_m3u_reader.h"
#include "voCMutex.h"
#include "vo_thread.h"
#include "voLiveSource.h"
#include "voSource2.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


struct media_item
{
	M3U_MEDIA_RELOAD_TYPE eReloadType;
	VO_CHAR path[1024];
	VO_S32 duration;

    VO_S32 sequence_number;

	VO_CHAR oldpath[1024];

	VO_BOOL is_encrypt;
	AutoBuffer * ptr_key;
	VO_BYTE iv[16];
	ProtectionType drm_type;

	VO_BOOL is_oldencrypt;
	AutoBuffer * ptr_oldkey;
	VO_BYTE oldiv[16];
	ProtectionType olddrm_type;

	M3U_MEDIA_USAGE_TYPE  eMediaUsageType;
	VO_CHAR*              pFilterString;
	VO_S32                  iCharpterId;
    VO_CHAR               strEXTKEYLine[1024];
    VO_CHAR               strOldEXTKEYLine[1024];
};

struct playlist_item
{
	VO_CHAR path[1024];
	VO_S64 band_width;

	playlist_item * ptr_pre;
	playlist_item * ptr_next;
	//Add for flag use
	VO_U32  uFlag;
	//Add for flag use
};

struct thumbnail_item
{
	VO_CHAR   path[1024];
	VO_S32    iduration;
	VO_VOID*  pPrivate;
	thumbnail_item*  ptr_next;
};


struct media_item_internal 
{
	M3U_MEDIA_RELOAD_TYPE eReloadType;
	VO_CHAR path[1024];
	VO_S32 sequence_number;
	VO_S32 duration;
	VO_U32 deadtime;

	VO_CHAR oldpath[1024];

	VO_BOOL is_encrypt;
	AutoBuffer * ptr_key;
	VO_BYTE iv[16];
	ProtectionType drm_type;

	VO_BOOL is_oldencrypt;
	AutoBuffer * ptr_oldkey;
	VO_BYTE oldiv[16];
	ProtectionType olddrm_type;

	M3U_MEDIA_USAGE_TYPE  eMediaUsageType;
	VO_CHAR*              pFilterString;	
	VO_S32                  iCharpterId;
    VO_CHAR               strEXTKEYLine[1024];
    VO_CHAR               strOldEXTKEYLine[1024];

	media_item_internal * ptr_next;
};

class vo_m3u_manager
	:public vo_thread
{
public:
	vo_m3u_manager(void);
	~vo_m3u_manager(void);
 
	VO_BOOL set_m3u_url( VO_CHAR * url );
	VO_S32 popup( media_item * ptr_item , VO_S64 bandwidth );
	VO_VOID close();
	VO_S32 getplaylist_duration(){ return m_playlist_duration; }

	VO_BOOL is_live(){ return m_reader.is_live_stream(); }
	VO_BOOL is_loop(){ return m_reader.is_loop(); }

	VO_S32 get_duration();

	VO_S32 set_pos( VO_S32 pos );

	VO_S64 get_cur_bandwidth();
	VO_S32 get_playlist_item_count(){ return m_playlist_count; }
	VO_S32 get_playlist_count() { return  m_playlist_count;}
	VO_U32 get_playlist_info_by_index(VO_U32 iIndex, VO_CHAR*  pURL, VO_U32*  pBitRate,VO_U32* pChunkCount);

    VO_VOID get_all_bandwidth( VO_S64 * ptr_array , VO_S32 * ptr_size );
	VO_CHAR*    GetManifestForIrdeto();
	VO_CHAR*    GetM3uURLForIrdeto();
	void        AddAdFilterString(VO_CHAR* pFilterString);
	void        ResetAdFilterString();
	
    VO_S32      GetCharpterIDTimeOffset(VO_S32  iCharpterId);
    VO_S32      GetSequenceIDTimeOffset(VO_S32  iSequenceId);
	VO_S32      GetCharpterCount(){ return m_reader.GetCharpterCount();}

	VO_VOID     SetRemovePureAudio(VO_BOOL    bRemoveAudio){ m_bRemoveAudio = bRemoveAudio; }
	VO_BOOL     GetDrmState(){return m_reader.GetDrmState();}
	playlist_item *   GetThumbnailPlayList();
	void              GenerateThumbnailInfoList();
	void              RemoveThumbnailInfoList();
	VO_U32            GetThumbnailItemCount();
	VO_S32            FillThumbnailItem(S_Thumbnail_Item* pThumbnailList, VO_U32 ulThumbnailCount);
    void              RemoveTheAudioPlayList();
	
	VO_VOID           SetParamForHttp(VO_U32  uId, VO_VOID* pData);
    VO_VOID           SetStartBitrate(VO_U32  ulStartBitrate);
    VO_VOID           SetCapBitrate(VO_U32  ulCapBitrate);
    VO_VOID           SetEventCall(VO_PTR pEventCallback);
    VO_VOID           SetTheMaxDownloadFailForManifest(VO_PTR   pulMax);
    VO_U32            GetTheTopManifestLength();
    VO_CHAR*          GetTheTopManifestDataPointer();
	
	

	media_item_internal * m_ptr_item_head;
	media_item_internal * m_ptr_item_tail;
	thumbnail_item      * m_ptr_thumbnail_head;
	thumbnail_item      * m_ptr_thumbnail_tail;
	VO_S32                m_ithumbnailCount;

protected:
	VO_VOID combine();
	VO_U32  GetStreamType(INF_item * ptr_item);

	media_item_internal * add_item( INF_item * ptr_item , VO_CHAR * ptr_relative_directory );
	VO_VOID INFitem2mediaitem( media_item_internal * ptr_dest , INF_item * ptr_src , VO_CHAR * ptr_relative_directory );

    
	thumbnail_item *  AddThumbnalItem( INF_item * ptr_item , VO_CHAR * ptr_relative_directory );

	VO_VOID update_playlist_info();

	virtual void thread_function();

	VO_VOID playlist_update();

	VO_VOID stop_updatethread();

	VO_VOID destroy();

	VO_VOID get_absolute_path( VO_CHAR * str_absolute_path , VO_CHAR * str_filename , VO_CHAR * str_relative_path );

	VO_BOOL parse_m3u();

	playlist_item * add_playlist_item( INF_item *  ptr_item );
	VO_VOID clear_playlist_item();

	VO_S32 change_bitrate( VO_S64 bandwidth );

	VO_S32 popup_live( media_item * ptr_item , VO_S64 bandwidth );
	VO_S32 popup_notlive( media_item * ptr_item , VO_S64 bandwidth );
    VO_VOID TransactionForManifestDownloadResult(VO_BOOL  bManifestParseResult);
    VO_VOID NotifyRecoverFromManifestDownloadFail();
    VO_VOID NotifyBeginWaitingManifestDownloadFailRecover();
	



private:
	vo_m3u_reader m_reader;

	VO_BOOL m_discontinuty;
	VO_BOOL m_is_live_stream;
	VO_S32 m_max_duration;
	VO_S32 m_sequence_start;
	VO_CHAR m_relative_directory[1024];

	VO_BOOL m_is_new_set_url;


	M3U_PLAYLIST_TYPE   m_eBasePlayListType;
	VO_BOOL             m_bRemoveAudio;


	voCMutex m_list_lock;

	VO_S32 m_playlist_duration;
	VO_S32 m_playlist_item_count;

	VO_BOOL m_stop_thread;

	VO_CHAR m_url[1024];
	VO_CHAR m_urlForBaseUrlForIrdeto[1024];
	VO_CHAR * m_pBaseFileContentForIrdeto;
	VO_S64 m_BaseFileContentSizeForIrdeto;

	VO_S32 m_playlist_count;

	playlist_item * m_ptr_playlist_list;
	playlist_item * m_ptr_current_playlist;

	VO_CHAR m_org_nexturl[1024];
	VO_BOOL m_is_oldencrypt;
	AutoBuffer * m_ptr_oldkey;
	VO_BYTE m_oldiv[16];
	ProtectionType m_olddrm_type;
    VO_CHAR m_strEXTKEYLineOld[1024];

    VO_S32 m_max_sequencenumber;
    VO_S64 m_illStartBitrate;
    VO_S64 m_illBitrateCap;
    VO_SOURCE2_EVENTCALLBACK    m_sEventCallback;
    VO_U32                      m_ulMaxDownloadFailCount;
    VO_U32                      m_ulCurrentDownloadFailCount;
};

#ifdef _VONAMESPACE
}
#endif

