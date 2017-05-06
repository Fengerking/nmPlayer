#pragma once
#include "CFlvTrack.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CFlvAudioTrack :
	public CFlvTrack
{
public:
	CFlvAudioTrack(VO_U8 btStreamNum, VO_U32 dwDuration, PFlvAudioInfo pAudioInfo, CFlvReader* pReader, VO_MEM_OPERATOR* pMemOp);
	virtual ~CFlvAudioTrack();

public:
	virtual VO_U32		GetCodecCC(VO_U32* pCC);
	virtual VO_U32		GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat);
	virtual VO_U32		GetBitrate(VO_U32* pdwBitrate);
	virtual	VO_U32		GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32		GetWaveFormatEx(VO_WAVEFORMATEX** ppWaveFormatEx);

protected:
	PFlvAudioInfo		m_pAudioInfo;
	VO_WAVEFORMATEX*	m_pWaveFormatEx;
};


#ifdef _VONAMESPACE
}
#endif
