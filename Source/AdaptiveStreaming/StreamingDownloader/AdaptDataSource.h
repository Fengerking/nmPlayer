/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2013			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		AdaptDataSource.h

	Contains:	AdaptDataSource class file

	Written by:	Aiven

	Change History (most recent first):
	2013-10-15		Aiven			Create file

*******************************************************************************/


#ifndef _ADAPT_DATASOURCE_H_
#define _ADAPT_DATASOURCE_H_

#include "voSource2.h"
#include "voOnStreamSourceType.h"
#include "voDSType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class AdaptDataSource
{
public:
	AdaptDataSource();
	~AdaptDataSource();

	virtual VO_U32 GetVideoCount();
	virtual VO_U32 GetAudioCount ();
	virtual VO_U32 GetSubtitleCount();
	virtual VO_U32 SelectVideo(VO_S32 nIndex);
	virtual VO_U32 SelectAudio (VO_S32 nIndex);
	virtual VO_U32 SelectSubtitle(VO_S32 nIndex);
	virtual VO_BOOL IsVideoAvailable (VO_S32 nIndex);
	virtual VO_BOOL IsAudioAvailable (VO_S32 nIndex);
	virtual VO_BOOL IsSubtitleAvailable (VO_S32 nIndex);
	virtual VO_U32 CommitSelection ();
	virtual VO_U32 ClearSelection ();
	virtual VO_U32 GetVideoProperty (VO_S32 nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
	virtual VO_U32 GetAudioProperty (VO_S32 nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
	virtual VO_U32 GetSubtitleProperty (VO_S32 nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
	virtual VO_U32 GetCurrTrackSelection (VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);
	virtual VO_U32 GetCurrPlayingTrackIndex (VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);
	virtual VO_U32 GetCurrSelectedTrackIndex (VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);

	virtual VO_U32 SetProgramInfo (_PROGRAM_INFO* 	pProgramInfo);
	virtual VO_U32 GetProgramInfo (VO_SOURCE2_PROGRAM_INFO**	ppProgramInfo);

protected:
	virtual VO_U32 GetMediaCount(VO_U32 ssType);
	virtual VO_U32 SelectMediaIndex(VO_U32 ssType, VO_S32 nIndex);
	virtual VO_U32 IsMediaIndexAvailable(VO_U32 ssType, VO_S32 nIndex);
	virtual VO_U32 GetMediaProperty(VO_U32 ssType, VO_S32 nIndex, VOOSMP_SRC_TRACK_PROPERTY **pProperty);
	virtual VO_U32 CommetSelection(VO_U32 bSelect);
	virtual VO_U32 GetCurMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex);
	virtual VO_U32 GetCurPlayingMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex);
	virtual VO_U32 GetCurSelectedMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex);
	virtual VO_U32 UpdateTrackInfo();
	virtual VO_U32 IsAvaialbeIndex(VO_S32 *paIndex, VO_S32* pvIndex, VO_S32* psIndex);
	virtual VO_U32 ClearPropertyItem(VOOSMP_SRC_TRACK_PROPERTY *pItemProperty);
	virtual VO_U32 IsAudioIndexAvaialbe( VO_S32 nAudioIndex);
	virtual VO_U32 IsVideoIndexAvaialbe( VO_S32 nVideoIndex);
	virtual VO_U32 IsSubtitleIndexAvaialbe( VO_S32 nSubtitleIndex);
	virtual VO_VOID SetOutSideID( _PROGRAM_INFO *pProgInfo, VO_SOURCE2_PROGRAM_INFO *pSourceProgInfo);

	virtual VO_VOID Reset();
protected:

	VO_U32							m_nAudioCount;
	VO_U32							m_nVideoCount;
	VO_U32							m_nSubtitleCount;

	VO_S32							m_pAudioTrackIndex[128];
	VO_S32							m_pVideoTrackIndex[32];
	VO_S32							m_pSubtitleTrackIndex[128];

	VO_S32							m_nAudioSelected;
	VO_S32							m_nVideoSelected;
	VO_S32							m_nSubtitleSelected;

	VOOSMP_SRC_TRACK_PROPERTY m_szAudioProtery;
	VOOSMP_SRC_TRACK_PROPERTY m_szVideoProtery;
	VOOSMP_SRC_TRACK_PROPERTY m_szSubtitleProtery;

	
	VO_CHAR							m_pAudioTrackLan[128][256];
	VO_CHAR							m_pSubtitleTrackLan[128][256];

	VO_S32							m_nCurPlayingAudioTrack;
	VO_S32							m_nCurPlayingVideoTrack;
	VO_S32							m_nCurPlayingSubtitleTrack;

	VO_S32							m_nCurSelectedAudioTrack;
	VO_S32							m_nCurSelectedVideoTrack;
	VO_S32							m_nCurSelectedSubtitleTrack;

	VO_SOURCE2_PROGRAM_INFO*		m_pProgramInfo;	
};

#ifdef _VONAMESPACE
}
#endif

#endif


