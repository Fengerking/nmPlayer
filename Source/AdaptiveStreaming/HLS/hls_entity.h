#ifndef __VO_HLS_ENTITY_H__
#define __VO_HLS_ENTITY_H__



//#ifdef WIN32
//#include "memorycheck.h"
//#endif

#include "hls_manager.h"
#include "voSource2.h"
#include "voAdaptiveStreamParser.h"
#include "voParser.h"
#include "voHLSDRM.h"
#include "voCMutex.h"
#include "vo_thread.h"
#include "voDSType.h"
#include "voSource2_ProgramInfo.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef   struct
{
    VO_ADAPTIVESTREAMPARSER_CHUNK    sCurrentAdaptiveStreamItem;
    VO_DRM2_HSL_PROCESS_INFO         sCurrentDrm;
    VO_ADAPTIVESTREAMPARSER_STARTEXT sCurrentStartExt;
}S_CHUNK_NODE;


typedef   struct
{
    S_CHUNK_NODE     sMainStreamChunk;
    S_CHUNK_NODE     sAltrerAudioChunk;
    S_CHUNK_NODE     sAltrerVideoChunk;
    S_CHUNK_NODE     sAltrerSubTitleChunk;
}S_CHUNK_CONTAINER;

#define  HLS_MAX_MANIFEST_RETRY_COUNT   5
#define  HLS_ROOT_MANIFEST_ID           0xffffffff
#define  HLS_MAX_STREAM_COUNT_IN_MASTER   256
#define  HLS_MAX_X_MEDIA_COUNT_IN_GROUP   256

class C_HLS_Entity	
:public vo_thread
{
public:
	C_HLS_Entity();
	~C_HLS_Entity();
	
    //the new interface 
    VO_U32     Init_HLS(VO_ADAPTIVESTREAM_PLAYLISTDATA * pData, VO_SOURCE2_EVENTCALLBACK*  pEventCallback, VO_ADAPTIVESTREAMPARSER_INITPARAM * pInitParam );
    VO_U32     Uninit_HLS();
    VO_U32     Close_HLS();
    VO_U32     Start_HLS();
    VO_U32     Stop_HLS();
	VO_U32     Open_HLS();
    VO_U32     GetChunk_HLS(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID ,  VO_ADAPTIVESTREAMPARSER_CHUNK **ppChunk );
    VO_U32     Update_HLS(VO_ADAPTIVESTREAM_PLAYLISTDATA * pData);
	VO_U32     Seek_HLS(VO_U64*  pTimeStamp, VO_ADAPTIVESTREAMPARSER_SEEKMODE sSeekMode);
	VO_U32     GetDuration_HLS(VO_U64 * pDuration);
	VO_U32     GetProgramCounts_HLS(VO_U32*  pProgramCounts);
	VO_U32     GetProgramInfo_HLS(VO_U32 nProgramId, _PROGRAM_INFO **ppProgramInfo);
	VO_U32     GetCurTrackInfo_HLS(VO_SOURCE2_TRACK_TYPE sTrackType , _TRACK_INFO ** ppTrackInfo);
	VO_U32     SelectProgram_HLS (VO_U32 uProgramId);
    VO_U32     SelectStream_HLS(VO_U32 uStreamId, VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS sPrepareChunkPos);
    VO_U32     SelectTrack_HLS(VO_SOURCE2_TRACK_TYPE  nType, VO_U32 nTrackID);
    VO_U32     GetDRMInfo_HLS(VO_SOURCE2_DRM_INFO** ppDRMInfo );
    VO_U32     GetParam_HLS(VO_U32 nParamID, VO_PTR pParam );
    VO_U32     SetParam_HLS(VO_U32 nParamID, VO_PTR pParam );
    VO_BOOL    GetLicState();	
    //the new interface 

	//add for new interface
	VO_U32  ParseHLSPlaylist(VO_VOID*  pPlaylistData, VO_U32 ulPlayListId);
    VO_U32  CommitPlayListData(VO_VOID*  pPlaylistData);
	VO_VOID ResetAllContext();
    VO_U32  PreparePlaySession();
    VO_VOID SetEventCallbackFunc(VO_PTR   pCallbackFunc);
    VO_U32  SelectDefaultStream(VO_U32*  pDefaultBitrate);
    VO_U32  GetChunckItem(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID,VO_ADAPTIVESTREAMPARSER_CHUNK **ppChunk);
    VO_U32  FillTrackInfoForStream(VO_SOURCE2_STREAM_INFO* pStreamInfo, S_PLAYLIST_NODE* pPlayListInfo);
    VO_U32  GetTrackCountByMainPlayList(S_PLAYLIST_NODE* pPlayListInfo, VO_U32*  pulCount);
    VO_U32  SetTheSelStreamInfo();
    VO_U32  GenerateTheProgramInfo();
    VO_U32  SetSelInfoInForStream(_STREAM_INFO* pStreamInfo);
	VO_U32  NotifyToParse(VO_CHAR*  pURLRoot, VO_CHAR*   pURL, VO_U32 ulPlayListId);
    VO_U32  PlayListUpdateForLive();
	virtual void thread_function();
    virtual void begin();
	VO_VOID StopPlaylistUpdate();
    VO_U32  UpdateThePlayListForLive();
    VO_U32  ResetSelInfoInForStream(_STREAM_INFO* pStreamInfo);
    VO_U32  RequestManfestAndParsing(VO_ADAPTIVESTREAM_PLAYLISTDATA*  pPlayListData, VO_CHAR*  pRootPath, VO_CHAR* pManifestURL, VO_U32 ulPlayListId);
    VO_U32  AdjustTheSequenceIDForMainStream();
    VO_U32  UpdateURL(void*  pPlayListData);
    VO_U32  PreparePlayFromSessionContext(S_SESSION_CONTEXT* pSessionContext);
    VO_U32  ResetContextForUpdateURL();

	
    //add for new interface

    //add the sample
    VO_U32   ConvertErrorCodeToSource2(VO_U32   ulErrorCodeInHLS);
    VO_VOID  InitChunkNode(S_CHUNK_NODE*  pChunkNode);
protected:


    //add for the common 	
	VO_VOID    DeleteAllProgramInfo();
	VO_VOID    DeleteStreamInfo(_STREAM_INFO*   pStreamInfo);
	
    //add for the common


    //add for parser
    VO_U32  ParserPlayList(VO_PBYTE pBuffer, VO_U32 ulBufferSize, VO_CHAR*   pPlayListPath);
    //add for parser

private:
    C_M3U_Manager           m_sM3UManager;
    VO_SOURCE2_EVENTCALLBACK*        m_pEventCallbackFunc;
	VOS2_ProgramInfo_Func *			 m_pProgFunc;
	VO_ADAPTIVESTREAM_PLAYLISTDATA   m_sCurrentAdaptiveStreamItemForPlayList;
    S_CHUNK_CONTAINER                m_sChunkContainer;
    VO_VOID*   m_pLogParam;	
    VO_BOOL                            m_bUpdateRunning;
    voCMutex                           m_ListLock;
	VO_PTR                             m_pHLicCheck;
    VO_SOURCE2_PROGRAM_TYPE            m_eProgramType;
};

#ifdef _VONAMESPACE
}
#endif


#endif
