#pragma once

#include "CBaseStreamFileTrack.h"
#include "CGFileChunk.h"
#include "CMpegParser.h"
#include "CMpegFileIndex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef struct
{
	VO_S64 nNearKeyFramTimeStamp;
	VO_S64 nMaxTimeStamp;
	VO_S64 nMinTimeStamp;
	VO_VOID Init(){nNearKeyFramTimeStamp = -1;nMaxTimeStamp = -1;nMinTimeStamp = -1;}
}MPEGFrameInfo;

class CMpegReader;
class CMpegTrack : public CBaseStreamFileTrack
{
	friend class CMpegReader;
public:
	CMpegTrack(VO_TRACKTYPE nType, VO_U8 btStreamID, VO_U32 dwTimeOffset, VO_U32 dwDuration, CMpegReader* pReader, VO_MEM_OPERATOR* pMemOp, VO_U8 btSubStreamID);
	virtual ~CMpegTrack();

public:
	virtual	VO_U32		GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32		GetBitrate(VO_U32* pdwBitrate);
	virtual VO_VOID		Flush();
	VO_VOID				Flush2();
	virtual VO_U64		GetTrackBufDuration();
	virtual VO_U32		GetFrameCntInIndex(){return m_pStreamFileIndex->GetCurrEntryCount();}
	virtual VO_U32		CheckHeadDataBuf(VO_PBYTE pBuf,VO_U32 nSize){return VO_ERR_SOURCE_OK;}
	virtual VO_U32		Prepare();
	virtual VO_U64		GetPreciseTrackBufDuration();
	virtual VO_U32		GetFrameInfoInIndex(MPEGFrameInfo *info,VO_S64 llTimeStamp);
	
protected:
	virtual VO_BOOL		OnPacket(CGFileChunk* pFileChunk, VO_U32 wLen, VO_S64 dwTimeStamp,VO_BOOL beEncrpt = VO_FALSE) = 0;
	virtual VO_BOOL		CheckPESBuffer();
	virtual VO_BOOL		CalculateTimeStamp(PBaseStreamMediaSampleIndexEntry pNewEntry);
	virtual VO_BOOL		PushTmpSampleToStreamFile(PBaseStreamMediaSampleIndexEntry pNewEntry);
	virtual VO_U32		Init(VO_U32 nSourceOpenFlags);	
	virtual VO_U8		IsCannotGenerateIndex(CvoFileDataParser* pDataParser);
	virtual VO_S64		CalcTrackTs(VO_S64 llTs);
protected:
	CMpegReader*		m_pReader;

	VO_U32				m_dwTimeOffset;
	VO_U32				m_dwBitrate;		//bytes per second
	VO_BOOL				m_bSetExtData;
	VO_PBYTE			m_pActPESBuffer;
//	PBaseStreamMediaSampleIndexEntry	m_pValidTimeStampEntry;///<Backup Entry for calculating the timestamp
	PBaseStreamMediaSampleIndexEntry	m_TmpStampEntryListHead;
	PBaseStreamMediaSampleIndexEntry	m_TmpStampEntryListTail;
	VO_S64								m_ValidTimeStamp;
	VO_U32								m_wUnvalidTimeStampEntry;///<the count of entry which has the same timestamp
	VO_U8								m_btSubStreamNum;
};

#ifdef _VONAMESPACE
}
#endif
