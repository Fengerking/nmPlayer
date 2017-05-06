/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
*																		*
************************************************************************/
/*******************************************************************************
File:		C_DASH_Entity.h

Contains:	C_DASH_Entity  header file

Written by:	Danny_Wang

Change History (most recent first):
2012-06-28 				Create file

*******************************************************************************/
#ifndef __VO_DASH_ENTITY_H__
#define __VO_DASH_ENTITY_H__

#include "C_MPD_Manager.h"
#include "voSource2.h"
#include "CDllLoad.h"
#include "voAdaptiveStreamParser.h"
#include "voParser.h"
#include "voCMutex.h"
#include "vo_thread.h"
#include "voLog.h"
#include "voCheck.h"



#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


	typedef	VO_S32 ( *pDataCallback) (VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData);
	typedef	VO_S32 ( *pEventCallback) (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);

	class C_DASH_Entity
		:public vo_thread
	{
	public:
		C_DASH_Entity();
		~C_DASH_Entity();
		//the new interface 
		VO_U32     Init_DASH(VO_ADAPTIVESTREAM_PLAYLISTDATA * pData , VO_SOURCE2_EVENTCALLBACK * pCallback,VO_ADAPTIVESTREAMPARSER_INITPARAM * pInitParam);
		VO_U32     Open_DASH();
		VO_U32     Start_DASH();
		VO_U32     Uninit_DASH();
		VO_U32     Close_DASH();
		VO_U32     Stop_DASH();	
		VO_U32     Update_DASH(VO_ADAPTIVESTREAM_PLAYLISTDATA * pData);	
		VO_U32     GetProgramInfo_DASH(VO_U32 nProgramId, _PROGRAM_INFO **ppProgramInfo);
		VO_U32     GetProgramCounts_DASH(VO_U32*  pProgramCounts);
		VO_U32     SelectProgram_DASH (VO_U32 uProgramId);
		VO_U32     SetTheSelStreamInfo();
		VO_U32     SelectStream_DASH(VO_U32 nTrackID,VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS sPrepareChunkPos);
		VO_U32     Create_ProgramInfo(VO_U32 m_program_id);
		VO_U32     Create_ProgramInfo_ts(VO_U32 m_program_id);
		VO_U32     Update_ProgramInfo(VO_U32 m_program_id);
		VO_U32     Choose_Program(VO_U32  pTimeStamp);
		VO_VOID     Change_Program(VO_U32  m_ProgramId);
		VO_U32     GetChunk_DASH(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID, VO_ADAPTIVESTREAMPARSER_CHUNK **ppItem);	
		VO_U32     Seek_DASH(VO_U64*  pTimeStamp,VO_ADAPTIVESTREAMPARSER_SEEKMODE sSeekMode);
		VO_U32     GetCurTrackInfo_DASH(VO_SOURCE2_TRACK_TYPE sTrackType , _TRACK_INFO ** ppTrackInfo);
		VO_U32     GetDuration_DASH(VO_U64 * pDuration);
	//	VO_U32     SelectTrack_DASH(VO_U32 nTrackID);
#ifdef _new_programinfo
		VO_U32     SelectTrack_DASH (VO_U32 nTrackID, VO_SOURCE2_TRACK_TYPE sTrackType);
#else
		VO_U32     SelectTrack_DASH(VO_U32 nTrackID);
#endif
		VO_U32     GetDRMInfo_DASH(VO_SOURCE2_DRM_INFO* ppDRMInfo );
		VO_U32     GetParam_DASH(VO_U32 nParamID, VO_PTR pParam );
		VO_U32     SetParam_DASH(VO_U32 nParamID, VO_PTR pParam );		
		VO_VOID    DeleteStreamInfo(_STREAM_INFO *   pStreamInfo);
		//the new interface 
		VO_VOID     StopPlaylistUpdate();
		VO_U32      ParseDASHPlaylist(VO_VOID*  pPlaylistData);
		VO_U32      PlayListUpdateForLive();
		//add for new interface
		VO_U32      CommitPlayListData(VO_VOID*  pPlaylistData);
		VO_U32      GetChunckItem(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID,VO_ADAPTIVESTREAMPARSER_CHUNK **ppChunk);
		VO_U32      SelectDefaultStream(VO_U32*  pDefaultBitrate);
		VO_VOID     SetTheCurrentSelStream(VO_U32 uBitrate);
		VO_VOID     SetEventCallbackFunc(VO_PTR   pCallbackFunc);
		VO_VOID     SetDataCallbackFunc(VO_PTR   pCallbackFunc);
		virtual void thread_function();
		VO_VOID    Compute_Chunk_index();
		VO_U32      GetLicState();
		VO_U32     SearchTrackType(VO_U32 stream_id, VO_U32 track_id);
	private:
		voCMutex            g_voMemMutex;
	protected:
		//add for the common
		VO_VOID    DeleteAllProgramInfo();
		VO_VOID    ResetAllContext();
		//add for the common
		//add for chunck format check
		VO_U32    ConvertErrorCodeToSource2(VO_U32   ulErrorCodeInDASH);
		VO_PTR				m_hCheck;
	private:
		C_MPD_Manager           m_sMpdManager;
		VO_BOOL                 m_bOpenComplete;
		VO_BOOL                 m_bNewFormatArrive;
		VO_BOOL                 pure_audio;
		VO_SOURCE2_EVENTCALLBACK*        m_pEventCallbackFunc;
		VO_SOURCE2_SAMPLECALLBACK*       m_pDataCallbackFunc;
		VO_ADAPTIVESTREAM_PLAYLISTDATA   m_sCurrentAdaptiveStreamItemForPlayList;
		VO_ADAPTIVESTREAMPARSER_CHUNK    m_sCurrentAdaptiveStreamItemForMainStream;
		VO_ADAPTIVESTREAMPARSER_CHUNK    m_sCurrentAdaptiveStreamItemForAdaption;
		VO_ADAPTIVESTREAMPARSER_CHUNK    m_sCurrentAdaptiveStreamItemForAlterAudio;
		VO_ADAPTIVESTREAMPARSER_CHUNK    m_sCurrentAdaptiveStreamItemForAlterVideo;	
		VO_ADAPTIVESTREAMPARSER_CHUNK    m_sCurrentAdaptiveStreamItemForAlterSubtitle;
		_PROGRAM_INFO*					 m_pProgramInfo;
		VO_BOOL                          m_bUpdateRunning;
		VO_BOOL                          m_enRefresh;
		VO_BOOL                          m_program_change_video;
		VO_BOOL                          m_program_change_audio;
		VO_BOOL                          m_program_change_subtitle;
		VO_BOOL                          m_write;
		VO_BOOL                          m_audio_end;
		VO_BOOL                          m_video_end;
		VO_CHAR	                         m_last_chLanguage[16];
		VO_CHAR	                         mpd_root_url[MAXURLLEN];
		VO_U64                           m_system_time;
		VO_U64                           m_utc_time;
		VO_U64                           m_last_index;
		VO_U32	                         m_ProgramId;
		VO_U32	                         m_PeriodCount;
		VO_U32							 m_cur_stream_id;
		VO_U32							 m_cur_track_video_id;
		VO_U32							 m_cur_track_audio_id;
		VO_U32                           m_last_audio_track_id; 
		VO_U32                           m_cur_subtitle_track_id;
		VO_BOOL                          m_BA_current_chunk_flag;
		VO_BOOL                          m_is_BA_mode;
		VO_BOOL                          m_is_first_BA;
		VO_U32                           m_ba_number;
		VO_BOOL                          Is_Need_Sync;
	};
#ifdef _VONAMESPACE
}
#endif

#endif
