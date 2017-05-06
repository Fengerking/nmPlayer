#pragma once

#include "CBaseStreamFileTrack.h"
#include "CMTVReader.h"
#include "CCmmbFileDataParser.h"

class CCmmbTrack : public CBaseStreamFileTrack
{
public:
	CCmmbTrack(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, CMTVReader* pReader, VO_MEM_OPERATOR* pMemOp);
	virtual ~CCmmbTrack(void);

public:
	virtual VO_U32		Init(VO_U32 nSourceOpenFlags);

	virtual	VO_U32		GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32		GetMaxSampleSize(VO_U32* pdwMaxSampleSize);
	virtual VO_U32		GetCodec(VO_U32* pCodec);
	virtual VO_U32		GetCodecCC(VO_U32* pCC);
	virtual VO_U32		GetBitrate(VO_U32* pdwBitrate);
	virtual VO_U32		GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat);
	virtual VO_U32		GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat);

	VO_U32				OnData(CMMB_FRAME_BUFFER* pBuf);

	// for test
	CGBuffer*			GetGlobalBuf(){return m_pGlobeBuffer;};


protected:
	//GetSample and SetPos implement of key frame mode!!
	virtual VO_U32		GetSampleK(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		SetPosK(VO_S64* pPos);
	
	//GetSample and SetPos implement of normal mode!!
	virtual VO_U32		GetSampleN(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		SetPosN(VO_S64* pPos);

protected:
	CMTVReader*		m_pReader;
	VO_BYTE*		m_pHeadData;
};
