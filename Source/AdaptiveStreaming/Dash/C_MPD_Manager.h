/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
*																		*
************************************************************************/
/*******************************************************************************
File:		C_MPD_Manager.h

Contains:	C_MPD_Manager.h  header file

Written by:	Danny_Wang

Change History (most recent first):
2012-06-28 				Create file

*******************************************************************************/
#ifndef __VO_C_MPD_Manager_H__
#define __VO_C_MPD_Manager_H__

#include "VO_MPD_Parser.h"
#include "CSourceIOUtility.h"
#include "voDSType.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
#ifdef _IOS
	using namespace _VONAMESPACE;
#endif
	typedef    struct S_PLAY_SESSION
	{
		VO_CHAR      strStreamURL[1024];
		VO_U32       ulStreamBitrate;
		VO_U32       ulMainStreamSequenceId;
		VO_U32       strAlterVideoGroupID;
		VO_U32       strAlterVideoSubStreamID;
		VO_U32       ulAlterVideoSequenceId;
		VO_U32       strAlterAudioGroupID;
		VO_U32       strAlterAudioSubStreamID;
		VO_U32       strAlterSubtileGroupID;
		VO_CHAR *    strAlterAudioLanguage;
		VO_U32       ulAlterAudioSequenceId;
		VO_BOOL      is_pure_audio;
		VO_U32       strPeriodIndex;
	}S_PLAY_SESSION;


	enum E_PLAYLIST_TYPE
	{
		E_X_MEDIA_VIDEO_STREAM,
		E_X_MEDIA_AUDIO_STREAM,
		E_X_MEDIA_VIDEO_TRACK_STREAM,
		E_X_MEDIA_AUDIO_TRACK_STREAM,
		E_X_MEDIA_SUBTITLE_TRACK_STREAM,
		E_X_MEDIA_AV_STREAM,
		E_X_MEDIA_SUBTITLE_STREAM,
		E_X_MEDIA_VIDEO_Index_STREAM,
		E_X_MEDIA_AUDIO_Index_STREAM,
		E_X_MEDIA_SUBTILE_Index_STREAM,
		E_X_MEDIA_AV_Index_STREAM,
		E_UNKNOWN_STREAM
	};



	enum E_CHUNK_TYPE
	{
		E_CHUNK_VIDEO,
		E_CHUNK_AUDIO,
		E_CHUNK_AV,
		E_CHUNK_TRACK,
		E_CHUNK_SUBTITLE,
	};


	typedef   struct
	{
		VO_CHAR               strChunckItemURL[1024];
		VO_U64				  ulDurationInMsec;
		VO_U64				  StartTime;
		VO_U32                Start_number;
		VO_U32                ulChunckIndex;
		E_CHUNK_TYPE          eChunckContentType;
		VO_U32                ulStateFlag;
		VO_U32                ulPrivateID;
		VO_VOID*              pReserve;
		VO_U64                ullBeginTime;
		VO_U64                ullEndTime;
		VO_U32                ulChunckOffset;
		VO_U32                ulChunckLen;
		VO_BOOL               range_flag;
		VO_BOOL               is_SidBox;
	}S_CHUNCK_ITEM;




	class C_MPD_Manager
	{
	public:
		C_MPD_Manager();
		~C_MPD_Manager();
		VO_U32   InitParser();
		VO_U32   UninitParser();
		VO_BOOL  ParseMpd(VO_BYTE* pMpdData, VO_U32 ulMpdDataLength, VO_CHAR* pMpdPath);
		VO_VOID  Init_Period(VO_U32 period_index);
		VO_U32   BuildPlayList_Video(S_PLAY_SESSION**  ppPlaySession);
		VO_U32   BuildPlayList_Audio(S_PLAY_SESSION**  ppPlaySession);
		VO_U32   GetTheStreamCurrentItem(E_PLAYLIST_TYPE eStreamType, S_CHUNCK_ITEM*    pChunckItem);
		VO_U32	 GetVideoPlayListItem( S_CHUNCK_ITEM*	pChunckItem);
		VO_U32	 GetAudioPlayListItem( S_CHUNCK_ITEM*	pChunckItem);
		VO_U32	 GetVideo_TrackPlayListItem(S_CHUNCK_ITEM*	  pChunckItem);
		VO_U32	 GetAudio_TrackPlayListItem(S_CHUNCK_ITEM*	  pChunckItem);
		VO_U32   GetAV_ChunkPlatListItem(S_CHUNCK_ITEM*	  pChunckItem);
		VO_U32   GetSubtitle_ChunkPlatListItem(S_CHUNCK_ITEM*    pChunckItem);
		VO_U32   GetSubtitle_TrackChunkPlatListItem(S_CHUNCK_ITEM*    pChunckItem);
		VO_U32   GetVideo_IndexStream(S_CHUNCK_ITEM*    pChunckItem);
		VO_U32   GetSubtitle_IndexChunkPlatListItem(S_CHUNCK_ITEM*    pChunckItem);
		VO_U32   GetAV_IndexStream(S_CHUNCK_ITEM*    pChunckItem);
		VO_U32   GetAudio_IndexStream(S_CHUNCK_ITEM*    pChunckItem);
		VO_U32   GetMainVideoCount(VO_U32*  pMainStreamCount);
		VO_U32   GetMainAudioCount(VO_U32*  pMainStreamCount);
		VO_U32   GetMainSubtitleCount(VO_U32*  pMainStreamCount);
		VO_U32   GetMainAudioBitrateCount(VO_U32*  pMainStreamCount);
		VO_U32   GetTheDuration(VO_U32*  pTimeOffset);
		VO_U64   SetThePos(VO_U64   ulTime,VO_U32 period_index);
		VO_U64   GetTimeScaleMs() { return m_manifest.video_scale == 1? 1: m_manifest.video_scale; }
		//VO_S64  Get_Audio_Scale(){return m_MpdParser.Get_Audio_Scale()==0?1:m_MpdParser.Get_Audio_Scale()/1000;}
		VO_S64   Get_Video_Scale(VO_U32 period_index){return m_MpdParser.Get_Video_Scale(period_index)==0?1:m_MpdParser.Get_Video_Scale(period_index);}
		VO_S64   Get_Audio_Scale(VO_U32 period_index){return m_MpdParser.Get_Audio_Scale(period_index)==0?1:m_MpdParser.Get_Audio_Scale(period_index);}
		VO_S64    Get_Video_Scale(){return m_MpdParser.Get_Video_Scale()==0?1:m_MpdParser.Get_Video_Scale();}
		VO_S64   Get_Audio_Scale(){return m_MpdParser.Get_Audio_Scale()==0?1:m_MpdParser.Get_Audio_Scale();}
		VO_S64   Get_duration_video(){return m_MpdParser.m_manifest->duration_video;}
		VO_S64   Get_duration_audio(){return m_MpdParser.m_manifest->duration_audio;}
		VO_U32   GetTheDefaultBitrate(VO_U32* pulBitrate);
		VO_U32   GetChunk_video_index(){return m_chunk_video_index;};
		VO_U32   GetChunk_audio_index(){return m_chunk_audio_index;};
		VO_U32   GetMaxChunkIndex(VO_U32 type);
		VO_S64   GetSegCount_Video(){return m_MpdParser.GetSegCount_Video();}
		VO_S64   GetSegCount_Audio(){return m_MpdParser.GetSegCount_Audio();}
		VO_VOID   SelectDefaultAV_Index(VO_U32 current_period_index);
		VO_VOID   SelectCurrentPlaySession(VO_U32 video_rpe_index, VO_U32 track_id,VO_U32 subtitle_track_id);
		VO_U32   SetDefaultPlaySession(VO_U32       strPeriodIndex);
		VO_U32   GetTheCurrentPlaySession(S_PLAY_SESSION**  ppPlaySession);
		VO_U32   GetCurrentVideoSequenceId(VO_U32*   pCurrentSequenceId);
		VO_U32   GetCurrentAudioSequenceId(VO_U32*   pCurrentSequenceId);
		VO_U32   GetCurrentSequenceId(VO_U32*   pCurrentSequenceId);
		VO_U32   GetMainStreamCount(VO_U32*  pMainStreamCount,VO_U32 period_index);
		VO_U32   Need_Send_Track(VO_BOOL is_need_send_track){m_need_send_track_audio = is_need_send_track;};
		VO_VOID   GetDuration(VO_U32*  pTimeDuration);
		VO_VOID  GetPeriodDuration(VO_U32*  period_duration,VO_U32 period_index);
		VO_U32   pop_fragment(  Representation * rpe_item ,Seg_item * ptr_item, VO_CHAR * ptr_path, VO_S64 last_network_bitrate);
		VO_VOID  replace(char *source,const char *sub,const char *rep );
		VO_BOOL  IsPlaySessionReady();
		VO_VOID  Set_is_NewFormat(VO_BOOL m_bNewFormatArrive){ m_need_send_track_video = m_bNewFormatArrive; m_need_send_track_audio = m_bNewFormatArrive; };
		VO_VOID  ReleaseAll();
		VO_U64    GetUpdatePeriod(){return m_MpdParser.Get_Update_Period(); }
		VO_BOOL   Is_Live(){return m_MpdParser.Is_Dynamic();}
		VO_BOOL   Is_Ts(){return m_MpdParser.is_dash_ts();}
		VO_BOOL   Is_Muxed(VO_U32 period_index){return m_MpdParser.Is_dash_muxed(period_index);}
		VO_U32    SetSessionUrl(VO_CHAR* pMpdPath);
		VO_VOID   SetUTCTime(VO_U64   pUTCTime,VO_U64 pSystem_time);
		VO_S64    GetAvailbilityTime(){return m_MpdParser.GetAvailbilityTime(); }
	//	VO_VOID    GetChunkRange(char *Range_URL,VO_U32 *ini_range,VO_U32 *size);
		VO_VOID    AddByteRangeInfo(VODS_SEG_INDEX_INFO * index_Info_List,VO_U32 track_type);
		VO_VOID    AddInitData(VODS_INITDATA_INFO * index_Info_List,VO_U32 track_type);
		VO_U64     Findbyterange_pos(VO_U64   ulTime,VO_U32 period_index);
		VO_U64     Findbyterange_time(VO_U64   ulTime,Representation * rpe_item);
		VO_U32     Find_byte_range_index(VO_U64   ulTime,Representation * rpe_item);
		VO_U32     Findbyterange_video_time(VO_U64   ulTime,VO_U32 period_index);
		VO_U32     GetChunkDuration();
		VO_BOOL            m_need_send_track_video;
		VO_BOOL            m_need_send_track_audio;
		VO_BOOL            m_need_send_track_subtitle;
	public:
		VO_S64             m_chunk_video_index;
		VO_S64             m_chunk_audio_index;
		VO_S64             m_chunk_subtitle_index;
		VO_MPD_Parser      m_MpdParser;
		S_PLAY_SESSION     m_CurrentPlaySession;
		MpdStreamingMedia  m_manifest;
		VO_U64             m_pure_audio_current_time;
	public:

		VO_U32             m_group_video_index;
		VO_U32             m_group_audio_index;
		VO_U32             m_group_subtile_index;
		VO_U32             m_ulDefaultBitrate_Audio;
		VO_U32             m_ulDefaultBitrate;
	//	VO_CHAR*           m_stream_url;
		VO_U64             m_ulUTCTime;
		VO_U64             m_ulSystemtimeForUTC;
		//add for template_live
		VO_U64             m_offset_video;
		VO_U64             m_offset_audio;
		//add for index_range control
		VO_BOOL            m_video_stream_index;
		VO_BOOL            m_audio_stream_index;
		VO_BOOL            m_is_dash_if;
		Representation * current_video_rpe;
		Representation * current_audio_rpe;
		Representation * current_subtile_rpe;
	};
#ifdef _VONAMESPACE
}
#endif
#endif
