/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		ISS_SmoothStreaming.h

	Contains:	ISS_SmoothStreaming class file

	Written by:	Aiven

	Change History (most recent first):
	2012-08-14		Aiven			Create file

*******************************************************************************/
#ifndef _IIS_SMOOTHSTREAMING_H_
#define _IIS_SMOOTHSTREAMING_H_

#include "vo_thread.h"
#include "voSource2.h"
#include "voAdaptiveStreamParser.h"
#include "ISS_ManifestManager.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


class ISS_SmoothStreaming
:public vo_thread
{
public:
	ISS_SmoothStreaming();
	~ISS_SmoothStreaming();

	VO_U32	   Init_IIS(VO_ADAPTIVESTREAM_PLAYLISTDATA * pData, VO_SOURCE2_EVENTCALLBACK*  pEventCallback,VO_ADAPTIVESTREAMPARSER_INITPARAM * pInitParam);
	VO_U32	   Uninit_IIS();
	VO_U32	   Close_IIS();
	VO_U32	   Start_IIS();
	VO_U32	   Stop_IIS();
	VO_U32	   Open_IIS();
	VO_U32	   Update_IIS(VO_ADAPTIVESTREAM_PLAYLISTDATA * pData);
	VO_U32	   GetChunk_IIS(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID ,  VO_ADAPTIVESTREAMPARSER_CHUNK **ppChunk ); 
	VO_U32	   Seek_IIS(VO_U64*  pTimeStamp, VO_ADAPTIVESTREAMPARSER_SEEKMODE sSeekMode);
	VO_U32	   GetDuration_IIS(VO_U64 * pDuration);
	VO_U32	   GetProgramCounts_IIS(VO_U32*  pProgramCounts);
	VO_U32	   GetProgramInfo_IIS(VO_U32 nProgramId, _PROGRAM_INFO **ppProgramInfo);
	VO_U32	   GetCurTrackInfo_IIS(VO_SOURCE2_TRACK_TYPE sTrackType , _TRACK_INFO ** ppTrackInfo);
	VO_U32	   SelectProgram_IIS (VO_U32 uProgramId);
	VO_U32	   SelectStream_IIS(VO_U32 uStreamId, VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS sPrepareChunkPos);
	VO_U32	   SelectTrack_IIS(VO_U32 nTrackID);
	VO_U32	   GetDRMInfo_IIS(VO_SOURCE2_DRM_INFO** ppDRMInfo );
	VO_U32	   GetParam_IIS(VO_U32 nParamID, VO_PTR pParam );
	VO_U32	   SetParam_IIS(VO_U32 nParamID, VO_PTR pParam );
    	VO_BOOL    GetLicState();	

protected:
	VO_U32 	GetCurrentTrackIndex(VO_SOURCE2_TRACK_TYPE sTrackType , VO_U32* index);
	void 		thread_function();
	VO_VOID StopPlaylistUpdate();
	VO_VOID StartPlaylistUpdate();
	VO_U32	PlayListUpdateForLive();
	VO_U32 	NotifyProgramInfo(VO_U32 EventID);

protected:
	ISS_ManifestManager*				m_nManager;
	VO_SOURCE2_EVENTCALLBACK*		m_pEventCallbackFunc;
	VO_ADAPTIVESTREAM_PLAYLISTDATA	m_nPlaylistData;


	VO_BOOL							m_bUpdateEnalbe;
	voCMutex                           			m_Lock;	
	VO_PTR                             			m_pHLicCheck;

	VO_BOOL							m_bStartUpdate;
};


#ifdef _VONAMESPACE
}
#endif

#endif//_IIS_SMOOTHSTREAMING_H_

