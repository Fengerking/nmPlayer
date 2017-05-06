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


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define  COMMON_ONSTREAM2_MEDIA_TS          0
#define  COMMON_ONSTREAM2_MEDIA_AAC         1
#define  COMMON_ONSTREAM2_MEDIA_UNKNOWN     2


#define  PLAYMODE_MEDIA_PURE_AUDIO          0
#define  PLAYMODE_MEDIA_PURE_VIDEO          1 
#define  PLAYMODE_MEDIA_AUDIO_VIDEO         2
#define  PLAYMODE_MEDIA_UNKNOWN             3

//the RECOMMEND_SIZE   is 16k
#define  RECOMMEND_SIZE                     16384


typedef   struct
{
    VO_U32    ulDrmType;
}S_DRM_NODE;



typedef	VO_S32 ( *pDataCallback) (VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData);
typedef	VO_S32 ( *pEventCallback) (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);

class C_DASH_Entity
:public vo_thread
{
public:
	 C_DASH_Entity();
	~C_DASH_Entity();
    //the new interface 
    VO_U32     Init_DASH(VO_ADAPTIVESTREAM_PLAYLISTDATA * pData , VO_SOURCE2_EVENTCALLBACK * pCallback);
    VO_U32     Uninit_DASH();
    VO_U32     Close_DASH();
	VO_U32     Open_DASH();
	VO_U32     Start_DASH();
	VO_U32     Stop_DASH();
	VO_U32     Update_DASH(VO_ADAPTIVESTREAM_PLAYLISTDATA * pData);	
	VO_U32     SelectStream_DASH(VO_U32 nTrackID);
	VO_U32     SetTheSelStreamInfo();
    VO_U32     GetChunk_DASH(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID, VO_ADAPTIVESTREAMPARSER_CHUNK **ppItem);	
    VO_U32     ChangeBitrate_DASH(VO_U32 uBitrate , VO_ADAPTIVESTREAMPARSER_CHUNK ** ppItem);
	VO_U32     Seek_DASH(VO_U64*  pTimeStamp);
	VO_U32     GetProgramCounts_DASH(VO_U32*  pProgramCounts);
	VO_U32     GetProgramInfo_DASH(VO_U32 nProgramId, VO_SOURCE2_PROGRAM_INFO **ppProgramInfo);
	VO_U32     Create_ProgramInfo();
	VO_U32     Create_ProgramInfo_ts();
	VO_U32     GetCurTrackInfo_DASH(VO_SOURCE2_TRACK_TYPE sTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo);
	VO_U32     GetDuration_DASH(VO_U64 * pDuration);
	VO_U32     SelectProgram_DASH (VO_U32 uProgramId);
    VO_U32     SelectTrack_DASH(VO_U32 nTrackID);
    VO_U32     GetDRMInfo_DASH(VO_SOURCE2_DRM_INFO* ppDRMInfo );
    VO_U32     GetParam_DASH(VO_U32 nParamID, VO_PTR pParam );
    VO_U32     SetParam_DASH(VO_U32 nParamID, VO_PTR pParam );
	VO_VOID    DeleteStreamInfo(VO_SOURCE2_STREAM_INFO*   pStreamInfo);
    //the new interface 
    VO_VOID     StopPlaylistUpdate();
	VO_U32      ParseDASHPlaylist(VO_VOID*  pPlaylistData);
	VO_U32      PlayListUpdateForLive();
	//add for new interface
	VO_U32      CommitPlayListData(VO_VOID*  pPlaylistData);
    VO_VOID     SendEos();
    VO_U32      GetChunckItem(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID,VO_ADAPTIVESTREAMPARSER_CHUNK **ppChunk);
	VO_U32      SelectDefaultStream(VO_U32*  pDefaultBitrate);
	VO_VOID     SetTheCurrentSelStream(VO_U32 uBitrate);
	VO_VOID     SetEventCallbackFunc(VO_PTR   pCallbackFunc);
	VO_VOID     SetDataCallbackFunc(VO_PTR   pCallbackFunc);
	virtual void thread_function();
	VO_VOID    Compute_Chunk_index();
private:
	  voCMutex            g_voMemMutex;
protected:
    //add for the common
      VO_VOID    DeleteAllProgramInfo();
	  VO_VOID    ResetAllContext();
    //add for the common
    //add for chunck format check
	  VO_U32    ConvertErrorCodeToSource2(VO_U32   ulErrorCodeInDASH);
private:
    C_MPD_Manager           m_sMpdManager;
	VO_BOOL                 m_bOpenComplete;
	VO_BOOL                 m_bWaitFirstFrame;
	VO_BOOL                 m_bNewFormatArrive;
	VO_BOOL                 m_bInBitrateAdaption;
	VO_BOOL                            m_bNewVideoFlagForXMedia;
    VO_BOOL                            m_bNewAudioFlagForXMedia;	
	VO_SOURCE2_EVENTCALLBACK*        m_pEventCallbackFunc;
	VO_SOURCE2_SAMPLECALLBACK*       m_pDataCallbackFunc;
	VO_ADAPTIVESTREAM_PLAYLISTDATA   m_sCurrentAdaptiveStreamItemForPlayList;
	VO_ADAPTIVESTREAMPARSER_CHUNK    m_sCurrentAdaptiveStreamItemForMainStream;
	VO_ADAPTIVESTREAMPARSER_CHUNK    m_sCurrentAdaptiveStreamItemForAdaption;
	VO_ADAPTIVESTREAMPARSER_CHUNK    m_sCurrentAdaptiveStreamItemForAlterAudio;
	VO_ADAPTIVESTREAMPARSER_CHUNK    m_sCurrentAdaptiveStreamItemForAlterVideo;	
	VO_BOOL                          m_bUpdateRunning;
	VO_BOOL                          m_enRefresh;
    VO_SOURCE2_PROGRAM_INFO*         m_pProgramInfo;
	VO_VOID*   m_pLogParam;
    VO_BOOL    pure_audio;
	VO_U32     m_last_audio_track_id; 
	VO_CHAR	   m_last_chLanguage[16];
	VO_CHAR		mpd_root_url[MAXURLLEN];
	VO_U64     m_system_time;
	VO_U64     m_utc_time;
	VO_BOOL    m_audio_end;
	VO_BOOL    m_video_end;
	VO_U64     m_last_index;
	VO_BOOL    m_write;



};
#ifdef _VONAMESPACE
}
#endif

#endif
