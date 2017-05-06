#ifndef __COSAdapStrSource_H__
#define __COSAdapStrSource_H__

#include "COSBaseSource.h"

class COSAdapStrSource : public COSBaseSource
{
public:
	COSAdapStrSource (VO_SOURCE2_LIB_FUNC *pLibop);
	virtual ~COSAdapStrSource (void);

	virtual int				Init(void * pSource, int nFlag, void* pInitparam, int nInitFlag);
	virtual int				Uninit(void);
	virtual int 			GetParam (int nID, void * pValue);
	virtual int 			SetParam (int nID, void * pValue);

	virtual int				GetMediaCount(int ssType);
	virtual int				SelectMediaIndex(int ssType, int nIndex);
	virtual int				IsMediaIndexAvailable(int ssType, int nIndex);
	virtual int				GetMediaProperty(int ssType, int nIndex, VOOSMP_SRC_TRACK_PROPERTY **pProperty);
	virtual int				CommetSelection(int bSelect);
	virtual int				GetCurMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex);
	virtual int				GetCurPlayingMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex);
	virtual int				GetCurSelectedMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex);

	virtual int				HandleEvent (int nID, int nParam1, int nParam2);

protected:
	virtual int				UpdateTrackInfo();
	virtual int				ClearPropertyItem(VOOSMP_SRC_TRACK_PROPERTY *pItemProperty);
	virtual int				IsAvaialbeIndex(int *paIndex, int* pvIndex, int* psIndex);
	virtual int				IsAudioIndexAvaialbe( int nAudioIndex);
	virtual int				IsVideoIndexAvaialbe( int nVideoIndex);
	virtual int				IsSubtitleIndexAvaialbe( int nSubtitleIndex);
	virtual int				GetAutoSelectStreamingAudioSubtitleID (int* pAudioID, int* pSubtitleID);
	virtual int				LoadDll();

protected:
	VOOSMP_SRC_CHUNK_INFO			m_szChunkInfo;
	VOOSMP_SRC_CHUNK_SAMPLE		m_szChunkSample;

	//a/v/s trackinfo select track...
	VO_SOURCE2_PROGRAM_INFO*	m_pProgramInfo;	
	char							m_pAudioTrackLan[128][256];
	char							m_pSubtitleTrackLan[128][256];

	int							m_nCurPlayingAudioTrack;
	int							m_nCurPlayingVideoTrack;
	int							m_nCurPlayingSubtitleTrack;

	int							m_nCurSelectedAudioTrack;
	int							m_nCurSelectedVideoTrack;
	int							m_nCurSelectedSubtitleTrack;

	int							m_nIsCommitSelectTrack;
	int							m_nCurSelectedNoCommitAudioTrack;
	int							m_nCurSelectedNoCommitVideoTrack;
	int							m_nCurSelectedNoCommitSubtitleTrack;
};

#endif // __COSAdapStrSource_H__
