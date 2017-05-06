	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		CBaseReader.h

Contains:	CBaseReader header file

Written by:	East

Change History (most recent first):
2006-12-12		East			Create file

*******************************************************************************/
#ifndef __CBaseReader_H__
#define __CBaseReader_H__


#include "voYYDef_filcmn.h"
#include "CBaseTrack.h"
#include "CvoBaseFileOpr.h"
#include "CvoBaseDrmCallback.h"
#include "CGFileChunk.h"
#include "fMacros.h"
//#ifndef __arm
#include "voCheck.h"
//#endif	//__arm
#if defined(_IOS) || defined(_SUPPORT_PARSER_METADATA)
#include "CBaseMetaData.h"
#endif


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CBaseReader
	: public CvoBaseFileOpr
	, public CvoBaseMemOpr
	, public CvoBaseDrmCallback
	, public CvoBaseObject
{
public:
	CBaseReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CBaseReader();

public:
	virtual VO_U32		init(VO_U32 nSourceOpenFlags, VO_LIB_OPERATOR* pLibOP, VO_U32 uModuleID , VO_TCHAR* pWorkingPath);
	virtual VO_U32		uninit();

	virtual VO_U32		CheckAudio(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		CheckVideo(VO_SOURCE_SAMPLE* pSample);

public:
	//open and close file is done here, because all file reader implement these operation<East Note>
	virtual VO_U32		Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);
	virtual VO_U32		Close();
	virtual VO_U32		SetParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32		GetParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32		GetInfo(VO_SOURCE_INFO* pSourceInfo);

public:
	virtual VO_U32		GetDuration();
	//return value: 0 - fail; 1 - success; 2 - need retry
	virtual VO_S32		ReadBuffer(VO_PBYTE pData, VO_S64 llFilePos, VO_U32& dwLen);

	virtual VO_U32		GetTrackCount() {return m_nTracks;}
	virtual CBaseTrack*	GetTrackByIndex(VO_U32 nIndex);
	virtual CBaseTrack* GetTrackByStreamNum(VO_U8 btStreamNum);
	virtual CBaseTrack*	GetAudioTrackInUsed();
	virtual CBaseTrack*	GetVideoTrackInUsed();
	virtual CBaseTrack*	GetOtherTrackInUsed(CBaseTrack* pTrack);

	virtual VO_U32		SetSelectTrack(VO_U32 nIndex, VO_BOOL bInUsed, VO_U64 llTs);
	virtual VO_U32		GetSelectTrack(VO_U32 nIndex, VO_BOOL& bInUsed);

public:
	virtual VO_U32		SetPlayMode(VO_SOURCE_PLAYMODE PlayMode);
	virtual VO_U32		MoveTo(VO_S64 llTimeStamp);

	virtual VO_U32		GetSeekTrack() {return m_nSeekTrack;}
	virtual VO_VOID		SetSeekTrack(VO_U32 nIndex) {m_nSeekTrack = nIndex;}

protected:
	//operate API for m_ppTracks
	virtual VO_U32		TracksCreate(VO_U32 nTrackSize);
	virtual VO_U32		TracksAdd(CBaseTrack* pNewTrack);
	virtual VO_U32		TracksRemove(CBaseTrack* pDelTrack);
	virtual VO_U32		TracksDestroy();

	//initialize all tracks
	virtual VO_U32		InitTracks(VO_U32 nSourceOpenFlags);
	//uninitialize all tracks
	virtual VO_U32		UninitTracks();

	virtual VO_U32		SelectDefaultTracks();

	//prepare tracks for playback
	virtual VO_U32		PrepareTracks();
	//unprepare tracks for playback
	virtual VO_U32		UnprepareTracks();

	virtual	VO_U32		GetFileHeadSize(VO_U32* pdwFileHeadSize) {return VO_ERR_NOT_IMPLEMENT;}
	virtual VO_U32		GetFileHeadDataInfo(headerdata_info* pHeadDataInfo) {return VO_ERR_NOT_IMPLEMENT;}
	virtual VO_U32		GetMediaTimeByFilePos(VO_FILE_MEDIATIMEFILEPOS* pParam) {return VO_ERR_NOT_IMPLEMENT;}
	virtual VO_U32		GetFilePosByMediaTime(VO_FILE_MEDIATIMEFILEPOS* pParam) {return VO_ERR_NOT_IMPLEMENT;}
	virtual VO_U32		GetSeekPosByTime(VO_FILE_MEDIATIMEFILEPOS* pParam) {return VO_ERR_NOT_IMPLEMENT;}

	virtual VO_U32		GetFileSize(VO_S64* pllFileSize);
	virtual VO_U32		GetSeekable(VO_BOOL* pIsSeekable);
	virtual VO_U32		IsNeedScan(VO_BOOL* pIsNeedScan);

	virtual VO_U32		GetFileRealSize(VO_U64 *) { return VO_ERR_NOT_IMPLEMENT; }
	virtual VO_U32		GetSeekTimeByPos(VO_U64 *) { return VO_ERR_NOT_IMPLEMENT; }
	virtual VO_U32		GetDRMType(VO_U32 * pDrmType){return VO_ERR_NOT_IMPLEMENT;}

	virtual VO_U32		InitMetaDataParser() {return VO_ERR_NOT_IMPLEMENT;}

protected:
	VO_PTR				m_hFile;
	VO_U64				m_ullFileSize;
	CGFileChunk			m_chunk;

	VO_U32				m_nTrackSize;	//size of m_ppTracks
	CBaseTrack**		m_ppTracks;		//contain tracks, valid track is not NULL and put ahead
	VO_U32				m_nTracks;

	VO_U32				m_nSeekTrack;			//the index of track which will activate file set position!!

	VO_PTR				m_hCheck;

	VO_U64				m_ullRealFileSize;//for adb push play

#if defined(_IOS) || defined(_SUPPORT_PARSER_METADATA)
	CBaseMetaData*		m_pMetaDataParser;
#endif
};

#ifdef _VONAMESPACE
}
#endif

#endif	//__CBaseReader_H__
