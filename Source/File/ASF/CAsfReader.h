#pragma once

#include "CBaseStreamFileReader.h"
#include "CAsfTrack.h"
#include "CAsfParser.h"
#include "voDRM.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CAsfReader :
	public CBaseStreamFileReader
{
public:
	CAsfReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CAsfReader();

public:
	virtual VO_U32		Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);
	virtual VO_U32		Close();

public:
	virtual VO_U32		FileIndexGetIndexByTime(VO_BOOL bForward, VO_S64 llTimeStamp);
	virtual VO_U64		FileIndexGetFilePosByIndex(VO_U32 dwFileIndex);

	virtual VO_VOID		OnNewDataParser(CBaseTrack* pRqstTrack, CvoFileDataParser** ppDataParser);

	inline VO_U8		OnPayloadPlay(CGFileChunk* pFileChunk, VO_U8 btStreamNum, VO_U32 dwPosInMediaObj, VO_U32 dwLen, VO_U32 dwMediaObjSize, VO_U32 dwTimeStamp);
	inline VO_U8		OnPayloadThumbnail(CGFileChunk* pFileChunk, VO_U8 btStreamNum, VO_U32 dwPosInMediaObj, VO_U32 dwLen, VO_U32 dwMediaObjSize, VO_U32 dwTimeStamp);
	inline VO_BOOL		OnNewFrameSeek(VO_U8 btStreamNum, VO_U32 dwTimeStamp, VO_U32 dwSeekTimeStamp);

	VO_BOOL				ExistIndex() { return m_IndexParser.IsValid(); }

	VO_U64 				GetSizeofNearKeyFrame(VO_U64 ullStartFilePos);
	VO_BOOL				IsThumbnail(){return m_bThumbnail;}
protected:
	virtual CAsfTrack*	NewTrack(PAsfTrackInfo pTrackInfo);

	inline VO_VOID		ResetTimeStampInSeek();
	inline VO_VOID		RecordTimeStampInSeek();
	inline VO_BOOL		TimeStampInSeekValid();
	inline VO_BOOL		TimeStampInSeekUpdate();

	//return -1 if fail
	virtual VO_U64		GetFilePosByTime(VO_U64 ullStartFilePos, VO_U32 dwTimeStamp);

public:
	virtual VO_U32		SetPlayMode(VO_SOURCE_PLAYMODE PlayMode);
	virtual VO_U32		MoveTo(VO_S64 llTimeStamp);
	VO_BOOL GetThumbNailBuffer(VO_PBYTE* ppBuffer, VO_U32 nSize, VO_U32 nPos);

public:
	virtual VO_BOOL		IsWMDRM() { return (VO_DRMTYPE_WindowsMedia == m_eDRMType) ? VO_TRUE : VO_FALSE; }
	virtual VO_BOOL		IsPlayReady() { return (VO_DRMTYPE_PlayReady == m_eDRMType) ? VO_TRUE : VO_FALSE; }
	virtual VO_PBYTE	GetTmpPayload();

	virtual VO_U32		GetNearKeyFrame(VO_S64 llTime, VO_S64* pllPreviousKeyframeTime, VO_S64* pllNextKeyframeTime);
	VO_BOOL				IsTrackBufferHasEnoughFrame(VO_U32 uFrameCount);

protected:
	virtual VO_U32		InitTracks(VO_U32 nSourceOpenFlags);

	virtual VO_U32		SelectDefaultTracks();

	VO_BOOL				IsCanGenerateIndex();
#ifdef _SUPPORT_PARSER_METADATA
	virtual VO_U32		InitMetaDataParser();
#endif	//_SUPPORT_PARSER_METADATA

	virtual VO_U32		GetFileRealSize(VO_U64 *);

protected:
	CAsfHeaderParser	m_HeaderParser;
	CAsfDataParser		m_DataParser;
	CAsfIndexParser		m_IndexParser;

	VO_DRM_TYPE			m_eDRMType;
	VO_PBYTE			m_pTmpPayload;

	VO_BOOL				m_ForStreaming;
	VO_BOOL          	m_bThumbnail;

};

#ifdef _VONAMESPACE
}
#endif