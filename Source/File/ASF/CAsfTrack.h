#pragma once

#include "CBaseStreamFileTrack.h"
#include "CGFileChunk.h"
#include "CAsfParser.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CAsfReader;
class CAsfTrack : public CBaseStreamFileTrack
{
	friend class CAsfReader;
public:
	CAsfTrack(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, CAsfReader *pReader, VO_MEM_OPERATOR* pMemOp);
	virtual ~CAsfTrack();


	virtual VO_U32		GetTrackBufDuration();
	virtual VO_U32		GetFrameCntInIndex(){return m_pStreamFileIndex->GetCurrEntryCount();}

public:
	virtual VO_U32		GetBitrate(VO_U32* pdwBitrate);


protected:
	virtual VO_BOOL		AddPayloadInfo(CGFileChunk* pFileChunk, VO_BOOL bKeyFrame, VO_U32 dwPosInMediaObj, VO_U32 dwPayloadLen, VO_U32 dwMediaObjSize, VO_U32 dwTimeStamp, VO_PBYTE pIV = NULL/*, VO_S32 iIVLen = 8*/);
	virtual VO_VOID		Flush();


	virtual VO_U8		IsCannotGenerateIndex(CvoFileDataParser* pDataParser);

protected:
	CAsfReader*			m_pReader;

	//property buffer, if video, video-VIDEOINFOHEADER, audio-WAVEFORMATEX
	VO_U32				m_dwPropBufferSize;
	VO_PBYTE			m_pPropBuffer;

	VO_U32				m_dwBitrate;
	VO_U32				m_dwFrameRate;
	VO_U32				m_lMaxSampleSize;

	VO_U32				m_dwSizeCurrentKeyFrame;
	VO_U32				m_dwTimeStampInSeek;
	VO_U32				m_dwPrevTimeStampInSeek;

	PBaseStreamMediaSampleIndexEntry	m_pTempEntry;
};

#ifdef _VONAMESPACE
}
#endif