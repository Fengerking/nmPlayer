#pragma once
#include "CAsfTrack.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CAsfAudioTrack :
	public CAsfTrack
{
public:
	CAsfAudioTrack(VO_U8 btStreamNum, VO_U32 dwDuration, CAsfReader *pReader, VO_MEM_OPERATOR* pMemOp);
	virtual ~CAsfAudioTrack();

public:
	virtual	VO_U32			GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32			GetMaxSampleSize(VO_U32* pdwMaxSampleSize);
	virtual VO_U32			GetCodecCC(VO_U32* pCC);
	virtual VO_U32			GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat);
	virtual VO_U32			GetWaveFormatEx(VO_WAVEFORMATEX** ppWaveFormatEx);
};

#ifdef _VONAMESPACE
}
#endif