/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		ISS_ManifestManager.h

	Contains:	ISS_ManifestManager class file

	Written by:	Aiven

	Change History (most recent first):
	2012-08-14		Aiven			Create file

*******************************************************************************/

#ifndef _IIS_MANIFESTMANAGER_H_
#define _IIS_MANIFESTMANAGER_H_

#include "list_T.h"

#include "ISS_ManifestParser.h"
#include "voAdaptiveStreamParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define AUDIO_TRACK		(0x1)
#define VIDEO_TRACK		(0x1<<1)
#define SUBTILE_TRACK	(0x1<<2)
#define ALL_TRACK		(AUDIO_TRACK|VIDEO_TRACK|SUBTILE_TRACK)

#define COMPARE_FLAG	0x8000000000000000ULL


//#define _DEBUG_MODE_
struct PlaySession{
	list_T<FragmentItem*>* pAudioChunkList;
	list_T<FragmentItem *>::iterator	audioiter;
	VO_U32				audioflag;
	FragmentItem			curaudiofragment;

	list_T<FragmentItem*>* pVideoChunkList;
	list_T<FragmentItem *>::iterator	videoiter;
	VO_U32				videoflag;
	FragmentItem			curvideofragment;

	list_T<FragmentItem*>* pTextChunkList;
	list_T<FragmentItem *>::iterator	Textiter;
	VO_U32				textflag;
	FragmentItem			curtextfragment;

	VO_U32				curstream;
	VO_U32				curaudiotrack;
	VO_U32				curtexttrack;

	VO_BOOL				isneedtoupdate;
};


struct TrackList{
	list_T<_TRACK_INFO*>* pAudioChunkList;
	list_T<FragmentItem *>::iterator	audioiter;
};


class ISS_ManifestManager
{
public:
	ISS_ManifestManager();
	~ISS_ManifestManager();

	VO_U32	   Init();
	VO_U32	   Uninit();


	VO_U32 ParseManifest(VO_PBYTE pBuffer,  VO_U32 uSize);
	VO_U32 GetChunk(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID ,  VO_ADAPTIVESTREAMPARSER_CHUNK **ppChunk );
	VO_U32 SetPos( VO_U64* pos, VO_ADAPTIVESTREAMPARSER_SEEKMODE sSeekMode);
	VO_U32 GetDuration(VO_U64 * pDuration);
	VO_U32 GetProgramCounts(VO_U32*  pProgramCounts);	
	VO_U32 GetProgramInfoByIndex(VO_U32 index, _PROGRAM_INFO **ppProgramInfo);
	VO_U32 GetCurTrackInfo(VO_SOURCE2_TRACK_TYPE TrackType, _TRACK_INFO ** ppTrackInfo);
	VO_U32 GetDRMInfo(VO_SOURCE2_DRM_INFO** ppDRMInfo );
	VO_U32 SelectStream_IIS(VO_U32 uStreamId, VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS sPrepareChunkPos);
	VO_U32 SelectTrack_IIS(VO_U32 nTrackID);

	VO_U32 SetParam_IIS(VO_U32 nParamID, VO_PTR pParam );
	VO_U32 GetParam_IIS(VO_U32 nParamID, VO_PTR pParam );

	VO_U32 GetUpdateIntervel(VO_U32 *pos);
	VO_BOOL IsNeedToUpdate();
	VO_BOOL Is_Live_Streaming();
protected:
	VO_U32 CreateProgramInfo();
	VO_U32 ReleaseProgramInfo();
	VO_U32 CreateDrmInfo();

	VO_U32 CheckTrackStatus(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID);
	VO_VOID CheckTheUpdateFlag();
	VO_U64 GetTimeScaleMs(){ return m_pManifestInfo->timeScale == 0? 1: m_pManifestInfo->timeScale/1000; }
	VO_U32 GetCurrentSreamInfo(_PROGRAM_INFO* pProgramInfo, _STREAM_INFO** ppStreamInfo);
	VO_U32 GetCurrentTrackInfo(_STREAM_INFO* pStreamInfo, VO_SOURCE2_TRACK_TYPE TrackType, _TRACK_INFO** ppTrackInfo);
	VO_U32 GetStreamIndexByType(VO_SOURCE2_TRACK_TYPE   TrackType, StreamIndex** ppStreamIndex);
	VO_U32 GenerateChunkUrlByFragment(FragmentItem* pItem,VO_CHAR* ptr_path);
	VO_SOURCE2_TRACK_TYPE ConvertTrackType(VO_U32 ItemType);
	VO_U32 SetStreamIndexPos(VO_SOURCE2_TRACK_TYPE   TrackType, VO_U64* pos);
	VO_U32 InitPlaySession();
	VO_U32 SyncPlaySession(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS sPrepareChunkPos = VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS_NEXT);
	VO_U32 ManifestReset();
	VO_U32 CheckLookAheadCount(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID);
	
	VO_U32 CalcThelivePoint(VO_U64 *pos, VO_U64 *duration=NULL);
	VO_U32 GetDvrWindowLength(VO_U64 *pos);
	VO_U32 GetEndTime(VO_U64 *pos);
	VO_U32 GetCurrentDownloadTime(VO_U64 *pos);	
	VO_U32 GetChunkOffsetValueBySequenceStartTime(VO_U64 uStartTime, VO_U32* pTimeOffset);
	VO_U32 GetFirstChunkTimeStamp(VO_U64&	StartTime);
	VO_U32 DoumpTheRawData(VO_PBYTE pBuffer, VO_U32 nLen, VO_CHAR* pName=(VO_CHAR*)"manifest.xml");
protected:
	SmoothStreamingMedia*	m_pManifestInfo;
	ISS_ManifestParser*	m_pManifestParser;

	VO_SOURCE2_DRM_INFO *		m_pDRMInfo;
	_PROGRAM_INFO* 	m_pProgramInfo;

	PlaySession*					m_pPlaySession;

	VO_U32						m_nTrackMode;
	
	list_T<_TRACK_INFO*> m_videotracklist;
	list_T<_TRACK_INFO*> m_audiotracklist;
	list_T<_TRACK_INFO*> m_texttracklist;

	VO_ADAPTIVESTREAMPARSER_CHUNK    m_sVideoChumk;
	VO_ADAPTIVESTREAMPARSER_CHUNK    m_sAudioChumk;	
	VO_ADAPTIVESTREAMPARSER_CHUNK    m_sTextChumk;	

	VO_U64     m_basesystem_time;
	VO_U64     m_cursystem_time;	
	VO_U64     m_utc_time;
};


#ifdef _VONAMESPACE
}
#endif


#endif//_IIS_MANIFESTMANAGER_H_


