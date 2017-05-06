#pragma once

#include "CBaseStreamFileReader.h"
#include "CFlvAudioTrack.h"
#include "CFlvVideoTrack.h"
#include "CFlvParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CFlvReader :
	public CBaseStreamFileReader
{
public:
	CFlvReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CFlvReader();

public:
	virtual VO_U32		Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);
	virtual VO_U32		Close();

public:
	virtual VO_U32		FileIndexGetIndexByTime(VO_BOOL bForward, VO_S64 llTimeStamp);
	virtual VO_U64		FileIndexGetFilePosByIndex(VO_U32 dwFileIndex);

	virtual VO_VOID		OnNewDataParser(CBaseTrack* pRqstTrack, CvoFileDataParser** ppDataParser);

	inline VO_U8		OnSample(CGFileChunk* pFileChunk, VO_BOOL bVideo, VO_U8 btFlag, VO_U32 dwLen, VO_U32 dwTimeStamp, VO_U64 pos);

	virtual VO_U32		GetFileBitrate();

public:
	virtual VO_U32		SetPlayMode(VO_SOURCE_PLAYMODE PlayMode);
	virtual VO_U32		MoveTo(VO_S64 llTimeStamp);

	virtual VO_U32		GetNearKeyFrame(VO_S64 llTime, VO_S64* pllPreviousKeyframeTime, VO_S64* pllNextKeyframeTime);
	VO_BOOL				GetThumbNailBuffer(VO_PBYTE* ppBuffer, VO_U32 nSize, VO_U32 nPos);
	VO_BOOL				IsThumbnail(){return m_bThumbnail;}

protected:
	virtual VO_BOOL		InitTracks(VO_U32 nSourceOpenFlags, VO_U32 dwExtSize);

	//added by yyang to support pd module
	virtual VO_U32		GetMediaTimeByFilePos(VO_FILE_MEDIATIMEFILEPOS* pParam);
	virtual VO_U32		GetFilePosByMediaTime(VO_FILE_MEDIATIMEFILEPOS* pParam);

	virtual	VO_U32		GetFileHeadSize(VO_U32* pdwFileHeadSize);
	//
public:
	CFlvHeaderParser	m_HeaderParser;

protected:
	//CFlvHeaderParser	m_HeaderParser;
	CFlvDataParser		m_DataParser;
	VO_BOOL          	m_bThumbnail;

protected:
	CFlvAudioTrack*		m_pFlvAudioTrack;
	CFlvVideoTrack*		m_pFlvVideoTrack;
};


#ifdef _VONAMESPACE
}
#endif
