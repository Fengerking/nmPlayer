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
	VO_CHAR      strVideoDesc[64];
    VO_CHAR      strAlterVideoURL[1024];
    VO_U32       strAlterAudioGroupID;
	VO_U32       strAlterAudioSubStreamID;
    VO_CHAR *      strAlterAudioLanguage;
    VO_CHAR      strAlterAudioURL[1024];
    VO_U32       ulAlterAudioSequenceId;
	VO_BOOL      is_pure_audio;
}S_PLAY_SESSION;


enum E_PLAYLIST_TYPE
{
    E_X_MEDIA_VIDEO_STREAM,
    E_X_MEDIA_AUDIO_STREAM,
    E_X_MEDIA_VIDEO_TRACK_STREAM,
    E_X_MEDIA_AUDIO_TRACK_STREAM,
	E_X_MEDIA_AV_STREAM,
	E_UNKNOWN_STREAM
};



enum E_CHUNK_TYPE
{
    E_CHUNK_VIDEO,
    E_CHUNK_AUDIO,
    E_CHUNK_AV,
    E_CHUNK_TRACK,
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
}S_CHUNCK_ITEM;




class C_MPD_Manager
{
public:
    C_MPD_Manager();
    ~C_MPD_Manager();
    VO_U32   ParseMpd(VO_BYTE* pM3uData, VO_U32 ulM3uDataLength, VO_CHAR* pM3uPath);
	VO_U32   BuildPlayList_Video(S_PLAY_SESSION**  ppPlaySession);
	VO_U32   BuildPlayList_Audio(S_PLAY_SESSION**  ppPlaySession);
    VO_U32   GetTheStreamCurrentItem(E_PLAYLIST_TYPE eStreamType, S_CHUNCK_ITEM*    pChunckItem);
	VO_U32	 GetVideoPlayListItem( S_CHUNCK_ITEM*	pChunckItem);
	VO_U32	 GetAudioPlayListItem( S_CHUNCK_ITEM*	pChunckItem);
	VO_U32	 GetVideo_TrackPlayListItem(S_CHUNCK_ITEM*	  pChunckItem);
	VO_U32	 GetAudio_TrackPlayListItem(S_CHUNCK_ITEM*	  pChunckItem);
	VO_U32   GetAV_ChunkPlatListItem(S_CHUNCK_ITEM*	  pChunckItem);
	VO_U32   GetMainVideoCount(VO_U32*  pMainStreamCount);
	VO_U32   GetMainAudioCount(VO_U32*  pMainStreamCount);
	VO_U32   GetTheDuration(VO_U32*  pTimeOffset);
    VO_U32   SetThePos(VO_U32   ulTime);
	//VO_U64   GetTimeScaleMs() { return Get_Video_Scale()==0?1:Get_Video_Scale()/1000;}
	//VO_S64  Get_Video_Scale(){return m_MpdParser.Get_Video_Scale()==0?1:m_MpdParser.Get_Video_Scale()/1000;}
	VO_U64   GetTimeScaleMs() { return m_manifest.video_scale == 1? 1: m_manifest.video_scale; }
	//VO_S64  Get_Audio_Scale(){return m_MpdParser.Get_Audio_Scale()==0?1:m_MpdParser.Get_Audio_Scale()/1000;}
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
    VO_U32   SelectDefaultAV_Index();
	VO_U32   SelectCurrentPlaySession(VO_U32  ulBitrate, VO_U32 video_rpe_index, VO_CHAR*  pAudioConcern);
	VO_U32   SelectCurrentVideoStreamId(VO_U32 video_rpe_index);
	VO_U32   SetDefaultPlaySession();
	VO_U32   GetTheCurrentPlaySession(S_PLAY_SESSION**  ppPlaySession);
	VO_U32   GetCurrentVideoSequenceId(VO_U32*   pCurrentSequenceId);
	VO_U32   GetCurrentAudioSequenceId(VO_U32*   pCurrentSequenceId);
	VO_U32   GetCurrentSequenceId(VO_U32*   pCurrentSequenceId);
	VO_U32   GetMainStreamCount(VO_U32*  pMainStreamCount);
	VO_U32   Need_Send_Track(VO_BOOL is_need_send_track){m_need_send_track_audio = is_need_send_track;};
	VO_U32   GetDuration(VO_U32*  pTimeDuration);
	VO_U32   pop_fragment(  Representation * rpe_item ,Seg_item * ptr_item, VO_CHAR * ptr_path, VO_S64 last_network_bitrate);
	VO_VOID  replace(char *source,const char *sub,const char *rep );
	VO_BOOL  IsPlaySessionReady();
	VO_VOID  Set_is_NewFormat(VO_BOOL m_bNewFormatArrive){ m_need_send_track_video = m_bNewFormatArrive; m_need_send_track_audio = m_bNewFormatArrive; };
	VO_VOID  ReleaseAll();
	VO_U64    GetUpdatePeriod(){return m_MpdParser.Get_Update_Period(); }
	VO_BOOL   Is_Live(){return m_MpdParser.Is_Dynamic();}
	VO_BOOL   Is_Ts(){return m_MpdParser.is_dash_ts();}
	VO_BOOL   Is_Muxed(){return m_MpdParser.Is_dash_muxed();}
	VO_U32    SetSessionUrl(VO_CHAR* pM3uPath);
	VO_VOID   SetUTCTime(VO_U64   pUTCTime,VO_U64 pSystem_time);
	VO_S64    GetAvailbilityTime(){return m_MpdParser.GetAvailbilityTime(); }
	MpdStreamingMedia  m_manifest;
	VO_BOOL            m_need_send_track_video;
	VO_BOOL            m_need_send_track_audio;
public:
    VO_S64             m_chunk_video_index;
	VO_S64             m_chunk_audio_index;
	VO_MPD_Parser      m_MpdParser;
private:
	S_PLAY_SESSION     m_CurrentPlaySession;
    VO_U32             m_group_video_index;
	VO_U32             m_group_audio_index;
	VO_U32             m_ulDefaultBitrate_Audio;
    VO_U32             m_ulDefaultBitrate;
//	VO_U32             m_ulTimeStampOffsetForLive;
	 VO_CHAR*           m_stream_url;
	 VO_U64             m_ulUTCTime;
     VO_U64             m_ulSystemtimeForUTC;
	 //add for template_live
	 VO_U64             m_offset_video;
	 VO_U64             m_offset_audio;
};
#ifdef _VONAMESPACE
}
#endif
#endif
