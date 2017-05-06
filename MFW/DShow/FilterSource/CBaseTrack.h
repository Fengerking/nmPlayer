#pragma once

#include "CMemoryOpt.h"
#include "CBaseFileParser.h"

class CBaseTrack : public CMemoryOpt
{
public:
	CBaseTrack( VO_MEM_OPERATOR *pMemOpt , VO_PTR hFileHandle , VO_SOURCE_READAPI *pReadAPI ,  int nTrackIndex);
	virtual ~CBaseTrack(void);

	virtual HRESULT	InitTrack();
	virtual HRESULT GetMediaType(CMediaType *pMT);
	virtual	HRESULT	GetTrackData(BYTE **ppBuf , LONG *pBufSize , LONGLONG* pllStart , LONGLONG* pllStop , LONGLONG llStreamTime = 0);
	
	VO_SOURCE_TRACKTYPE GetTrackType(){return mnTrackType;}

	virtual HRESULT	SetPos(LONGLONG &llPos);
	virtual HRESULT Flush();

	virtual LONGLONG	GetTrackDuration(){return mllDuration;}

	virtual HRESULT	Stop();
	virtual HRESULT	Pause();
	virtual HRESULT Run();

protected:
	
protected:
	int					mnTrackIndex;
	VO_SOURCE_READAPI * mpReadAPI; 
	VO_PTR				mhFileHandle;

	VO_SOURCE_TRACKTYPE mnTrackType;
	LONG				mnCodec;

	int					mnExtDataSize;
	BYTE *				mpExtData;

	LONGLONG			mllDuration;
	LONGLONG			mllStartTime;
};
